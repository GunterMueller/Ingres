/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <pc.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <tr.h>
#include    <lg.h>
#include    <lk.h>
#include    <dmf.h>
#include    <dm.h>
#include    <dmpp.h>
#include    <dmp.h>
#include    <dm1b.h>
#include    <dm0pbmcb.h>
#include    <dm0l.h>
#include    <dmd.h>

/**
**
**  Name: DMDBUFFER.C - Buffer manager trace routines.
**
**  Description:
**      This file contains routines for buffer manager tracing.
**
**
**  History:
**      01-oct-86 (derek)
**	28-feb-89 (rogerk)
**	    Added support for shared buffer manager.  Added new bmcb fields.
**	    Use lbmcb pointer to print stats, not bmcb.
**	28-mar-90 (greg)
**	    Change %3d to %5d, buffer sizes bigger than 999 being chopped
**	    Could have cache bigger than 20MB so not %4d
**	07-july-1992 (jnash)
**	    Add DMF Function prototyping.
**	3-oct-1992 (bryanp)
**	    Re-arrange includes to share transaction ID type definition.
**	24-may-1993 (bryanp)
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	26-jul-1993 (rogerk)
**	    Include respective CL headers for all cl calls.
**      06-mar-1996 (stial01)
**          Variable Page Size Project: Changes for new BM control blocks.
**	25-mar-96 (nick)
**	    Print FCwait statistic.
**	01-Apr-1997 (jenjo02)
**	    Table priority project:
**	    Added new stats for fixed priority tables.
**	01-May-1997 (jenjo02)
**	    Added new stat for group waits.
**	03-Apr-1998 (jenjo02)
**	    Added new stats for per-cache WriteBehind threads.
**	01-Sep-1998 (jenjo02)
**	  o Moved BUF_MUTEX waits out of bmcb_mwait, lbm_mwait, into DM0P_BSTAT where
**	    mutex waits by cache can be accounted for.
**	24-Aug-1999 (jenjo02)
**	    Added BMCB_FHDR, BMCB_FMAP page types, stat collection by
**	    page type.
**	10-Jan-2000 (jenjo02)
**	    Added stats by page type for groups.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	4-Feb-2002 (rigka01) Bug #106435 
**	    Use new TRdisplay datatype 'u' to display unsigned data; using 
**	    'd' causes large numbers to show up as negative when 
**	    for "set trace point dm420" output such as FIX CALLS.
**/


/*{
** Name: dmd_buffer	- Debug buffer manager information.
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	    {@return_description@}
**	Exceptions:
**	    [@description_or_none@]
**
** Side Effects:
**	    [@description_or_none@]
**
** History: 
**      01-oct-86 (derek)
**	28-feb-89 (rogerk)
**	    Added support for shared buffer manager.  Added new bmcb fields.
**	    Use lbmcb pointer to print stats, not bmcb.
**	28-mar-90 (greg)
**	    Change %3d to %5d, buffer sizes bigger than 999 being chopped
**	    Could have cache bigger than 20MB so not %4d
**      01-Nov-94 (Bin Li)
**       Fix bug 61055, trace point  dm420 output is wrong. "FAST COMMIT
**       FLUSHES" should be "CONSISTENCY POINT FLUSHES".
**      06-mar-1996 (stial01)
**          Variable Page Size Project: Changes for new BM control blocks.
**	25-mar-96 (nick)
**	    Print FCwait statistic.
**	01-Apr-1997 (jenjo02)
**	    Table priority project:
**	    Added new stats for fixed priority tables.
**	01-May-1997 (jenjo02)
**	    Added new stat for group waits.
**	03-Apr-1998 (jenjo02)
**	    Added new stats for per-cache WriteBehind threads.
**	01-Sep-1998 (jenjo02)
**	  o Moved BUF_MUTEX waits out of bmcb_mwait, lbm_mwait, into 
**	    DM0P_BSTAT where mutex waits by cache can be accounted for.
**	24-Aug-1999 (jenjo02)
**	    Added BMCB_FHDR, BMCB_FMAP page types, stat collection by
**	    page type.
**	09-Apr-2001 (jenjo02)
**	    Added GatherWrite stats.
**      4-Feb-2002 (rigka01) Bug #106435
**          Use new TRdisplay datatype 'u' to display unsigned data; using
**          'd' causes large numbers to show up as negative when
**          for "set trace point dm420" output such as FIX CALLS.
**	31-Jan-2003 (jenjo02)
**	    bs_stat name changes for Cache Flush Agents.
*/
VOID
dmd_buffer(VOID)
{
	DMP_LBMCB		*lbmcb = dmf_svcb->svcb_lbmcb_ptr;
	DM0P_BM_COMMON          *bm_common = lbmcb->lbm_bm_common;
	i4                     cache_ix;
	DM0P_BMCB               *bm;
	DM0P_BSTAT              *bs;
	i4			i;
	i4			pid;
	i4			fcount, mcount, lcount;
	i4			gfcount, gmcount, glcount;


	/*
	** Note that this can be a shared buffer manager and the values
	** being dumped may be changed while we are writing them.
	** No locks are taken on the buffer manager while the control blocks
	** are dumped here.
	*/

	if (bm_common->bmcb_srv_count > 1)
	{
	    PCpid(&pid);
	    TRdisplay("Connected to Server Pid : %d\n", pid);
	}

	TRdisplay ("   Buffer Status: %v\n",
	    "FCFLUSH,SHARED,PASS_ABORT,PREPASS_ABORT,IOMASTER,DMCM,MT", 
		bm_common->bmcb_status);
	TRdisplay("");
	TRdisplay("    Buffer Manager Id: %d  Connected servers %d\n",
	    bm_common->bmcb_id, bm_common->bmcb_srv_count);
	TRdisplay("    CP count: %d  CP index : %d  CP check : %d\n",
	    bm_common->bmcb_cpcount, bm_common->bmcb_cpindex, bm_common->bmcb_cpcheck);
	TRdisplay("    Database cache size: %d  Table cache size: %d\n",
	    bm_common->bmcb_dbcsize, bm_common->bmcb_tblcsize);
	TRdisplay("    Statistics%64*-\n");
	TRdisplay("          Lock reclaim: %d  CP flushes: %d\n",
	    lbmcb->lbm_lockreclaim, lbmcb->lbm_fcflush);

	for (cache_ix = 0; cache_ix < DM_MAX_CACHE; cache_ix++) 
	{
	    bm = lbmcb->lbm_bmcb[cache_ix];

	    if (!bm || (bm->bm_status & BM_ALLOCATED) == 0)
		continue;

	    bs = &lbmcb->lbm_stats[cache_ix];

	    DM0P_COUNT_ALL_QUEUES(fcount, mcount, lcount);
	    DM0P_COUNT_ALL_GQUEUES(gfcount, gmcount, glcount);

	    TRdisplay("    Buffer Cache Configuration (%2dK) %41*-\n",
			bm->bm_pgsize/1024);
	    TRdisplay("    Buffer count: %d  Bucket count: %d Group count: %d Size: %d\n", 
		bm->bm_bufcnt, bm->bm_hshcnt, bm->bm_gcnt, bm->bm_gpages);
	    TRdisplay("    Free count:  %d Limit: %d Modify count: %d Limit: %d\n",
		fcount, bm->bm_flimit, 
		mcount, bm->bm_mlimit); 

	    TRdisplay("    Free group count:  %d Modify group count: %d\n",
		gfcount, gmcount); 
	    TRdisplay("    Fixed count: %d Group fixed count: %d\n", 
		lcount, glcount);

	    /* Only show WriteBehind stats if WB in use in this cache */
	    if (bm->bm_status & BM_WB_IN_CACHE)
	    {
		TRdisplay("    WB start limit: %d WB end limit: %d\n",
		    bm->bm_wbstart, bm->bm_wbend);
		TRdisplay("    WB flushes: %d, Agents cloned: %d\n",
		    bs->bs_cache_flushes, bs->bs_cfa_cloned);
		TRdisplay("    Agents active: %d, Agent hwm: %d\n",
		    bs->bs_cfa_active, bs->bs_cfa_hwm);
		TRdisplay("    WB pages flushed: %d, WB groups flushed: %d\n",
		    bs->bs_wb_flushed, bs->bs_wb_gflushed);
	    }

	    TRdisplay("    Summary Statistics (%2dK) %57*-\n", bm->bm_pgsize/1024);
	    TRdisplay("             GWAIT     GSYNC  FREEWAIT  GW PAGES    GW I/O\n");
	    TRdisplay("        %5(%10u%)\n",
		bs->bs_gwait, bs->bs_gsyncwr, bs->bs_fwait,
		bs->bs_gw_pages, bs->bs_gw_io);

	    TRdisplay("         FIX CALLS      HITS     CHECK   REFRESH      READ      TOSS\n");

	    TRdisplay("        %6(%10u%)\n",
		bs->bs_fix[BMCB_PTYPES], bs->bs_hit[BMCB_PTYPES],
		bs->bs_check[BMCB_PTYPES], bs->bs_refresh[BMCB_PTYPES],
		bs->bs_reads[BMCB_PTYPES], bs->bs_toss[BMCB_PTYPES]);
	    TRdisplay("       UNFIX CALLS     DIRTY     FORCE     WRITE    IOWAIT      SYNC\n");
	    TRdisplay("        %6(%10u%)\n",
		bs->bs_unfix[BMCB_PTYPES], bs->bs_dirty[BMCB_PTYPES],
		bs->bs_force[BMCB_PTYPES], bs->bs_writes[BMCB_PTYPES],
		bs->bs_iowait[BMCB_PTYPES], bs->bs_syncwr[BMCB_PTYPES]);
	    TRdisplay("             MWAIT    FCWAIT   RECLAIM   REPLACE    GREADS   GWRITES\n");
	    TRdisplay("        %6(%10u%)\n", 
		bs->bs_mwait[BMCB_PTYPES], bs->bs_fcwait[BMCB_PTYPES],
		bs->bs_reclaim[BMCB_PTYPES], bs->bs_replace[BMCB_PTYPES],
		bs->bs_greads[BMCB_PTYPES], bs->bs_gwrites[BMCB_PTYPES]);

	    /* Show stats by page type: */
	    for (i = 0; i < BMCB_PTYPES; i++)
	    {
		if (bs->bs_fix[i] || bs->bs_reclaim[i])
		{
		    TRdisplay("        Stats for %w pages:\n",
			"FHDR,FMAP,ROOT,INDEX,LEAF,DATA", i);
		    TRdisplay("         FIX CALLS      HITS     CHECK   REFRESH      READ      TOSS\n");

		    TRdisplay("        %6(%10u%)\n",
			bs->bs_fix[i], bs->bs_hit[i],
			bs->bs_check[i], bs->bs_refresh[i],
			bs->bs_reads[i], bs->bs_toss[i]);
		    TRdisplay("       UNFIX CALLS     DIRTY     FORCE     WRITE    IOWAIT      SYNC\n");
		    TRdisplay("        %6(%10u%)\n",
			bs->bs_unfix[i], bs->bs_dirty[i],
			bs->bs_force[i], bs->bs_writes[i],
			bs->bs_iowait[i], bs->bs_syncwr[i]);
		    TRdisplay("             MWAIT    FCWAIT   RECLAIM   REPLACE    GREADS   GWRITES\n");
		    TRdisplay("        %6(%10u%)\n", 
			bs->bs_mwait[i], bs->bs_fcwait[i],
			bs->bs_reclaim[i], bs->bs_replace[i],
			bs->bs_greads[i], bs->bs_gwrites[i]);
		}
	    }
	}
}
