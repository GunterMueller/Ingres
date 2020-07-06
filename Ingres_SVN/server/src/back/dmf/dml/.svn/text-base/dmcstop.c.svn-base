/*
** Copyright (c) 1985, 2005 Ingres Corporation
**
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <pc.h>
#include    <tr.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <lk.h>
#include    <cx.h>
#include    <lg.h>
#include    <scf.h>
#include    <ulf.h>
#include    <dmf.h>
#include    <ercl.h>
#include    <pm.h>
#include    <dmccb.h>
#include    <dmrcb.h>
#include    <dmxcb.h>
#include    <dmtcb.h>
#include    <dm.h>
#include    <dmp.h>
#include    <dmpp.h>
#include    <dm0m.h>
#include    <dm1b.h>
#include    <dm0l.h>
#include    <dm0p.h>
#include    <dm0s.h>
#include    <dm0llctx.h>
#include    <dm0pbmcb.h>
#include    <dmd.h>

/**
** Name: DMCSTOP.C - Functions used to stop a server.
**
** Description:
**      This file contains the functions necessary to stop a server.
**      This file defines:
**    
**      dmc_stop_server() -  Routine to perform the stop 
**                           server operation.
**
** History:
**      01-sep-1985 (jennifer)
**          Created for Jupiter.      
**	18-nov-1985 (derek)
**	    Completed code for dmc_stop_server().
**	28-feb-1989 (rogerk)
**	    Added support for shared buffer manager.
**	 3-apr-1989 (rogerk)
**	    Moved dm0p_deallocate call to above dm0l_deallocate.
**	 8-jul-1992 (walt)
**	    Prototyping DMF.
**	3-oct-1992 (bryanp)
**	    Re-arrange includes to share transaction ID type definition.
**	14-dec-1992 (rogerk)
**	    Include dm1b.h before dm0l.h for prototype definitions.
**	24-may-1993 (bryanp)
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	20-sep-1993 (bryanp)
**	    The STAR server starts only a minimal DMF. When stopping such a
**		server, we must be careful only to stop those things that were
**		started (e.g., don't print statistics for the buffer manager,
**		since we don't have a buffer manager in Star).
**	25-oct-93 (vijay)
**		Fix cast error.
**	31-jan-1994 (bryanp) B58376, B58377, B58378
**	    Log LK status if LK call fails.
**	    Check return code from dm0m_destroy.
**	    log scf_error.err_code if scf_call fails.
**	06-Dec-1995 (jenjo02)
**	    Added display of LK/LG statistics, display_sem() function.
**	06-mar-1996 (stial01)
**	    Variable Page Size project:
**	    dmc_stop_server() Display statistics for all buffer pools.
**	25-mar-96 (nick)
**	    Add FC wait statistic in dmc_stop_server().
**	19-Apr-1996 (jenjo02)
**	    Added display of lbm_bmcwait, BM semaphore to BM stats.
**	11-Jul-1996 (jenjo02)
**	    Added LG buffer profile display.
**	13-Sep-1996 (jenjo02)
**	    BM mutex granularity project.
**	01-Oct-1996 (jenjo02)
**	    Added display of lgs_max_wq (max length of LG write queue)
**	    and lgs_max_wq_count (#times that max hit). 
**	24-Oct-1996 (jenjo02)
**	    Replaced hcb_mutex with hash_mutex, one for each hash entry.
**	    Added hcb_tq_mutex to protect TCB free queue.
**	09-Jan-1997 (jenjo02)
**	    When displaying LG/LK semaphore stats, display the full
**	    semaphore name instead of truncating it.
**	17-Jan-1997 (jenjo02)
**	    Replaced svcb_mutex with svcb_dq_mutex (DCB queue) and
**	    svcb_sq_mutex (SCB queue).
**      27-feb-1997 (stial01)
**          Release the single server lock list if it was allocated
**	01-Apr-1997 (jenjo02)
**	    Table cache priority project:
**	    Added new per-priority statistics.
**	01-May-1997 (jenjo02)
**	    Added group wait statistic.
**	03-Apr-1998 (jenjo02)
**	    Added new WriteBehind thread stats.
**	01-Sep-1998 (jenjo02)
**	  o Moved BUF_MUTEX waits out of bmcb_mwait, lbm_mwait, into DM0P_BSTAT where
**	    mutex waits by cache can be accounted for.
**      28-jul-1998 (rigka01)
**          Cross integrate fix for bug 90691 from 1.2 to 2.0 code line
**          In highly concurrent environment, temporary file names may not
**          be unique so improve algorithm for determining names
**          Handle svcb_DM2F_TempTblCnt_mutex and svcb_DM2F_TempTblCnt
**	24-Aug-1999 (jenjo02)
**	    BM stats expanded to show counts by page type.
**	10-Jan-2000 (jenjo02)
**	    Added stats by page type for groups.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	19-Apr-2001 (jenjo02)
**	    Added stats for GatherWrite I/O.
**      10-may-2001 (stial01)
**          Print btree statistics at server shutdown
**	05-Mar-2002 (jenjo02)
**	    Call dms_stop_server() for Sequence Generators.
**      20-sept-2004 (huazh01)
**          Pull out the fix for b90691 as it has been 
**          rewrittten. 
**          INGSRV2643, b111502. 
**      05-Aug-2005 (hanje04)
**          Back out change 478041 as it should never have been X-integrated
**          into main in the first place.
**/

/*
** Definition of static variables and forward static functions.
*/
static VOID	display_sem(CS_SEMAPHORE *sem);

/*{
** Name: dmc_stop_server - Stops a server.
**
**  INTERNAL DMF call format:    status = dmc_stop_server(&dmc_cb);
**
**  EXTERNAL call format:        status = dmf_call(DMC_STOP_SERVER,&dmc_cb);
**
** Description:
**    The dmc_stop_server function handles the deletion of a server.
**    This causes all control information assocaited with a server to be 
**    released.  A server cannot be stopped until all active threads have 
**    been ended.
**
** Inputs:
**      dmc_cb 
**          .type                           Must be set to DMC_CONTROL_CB.
**          .length                         Must be at least sizeof(DMC_CB).
**          .dmc_op_type                    Must be DMC_SERVER_OP.
**          .dmc_id                         Must be a unique server id
**                                          used to identify the server.
**
** Output:
**      dmc_cb 
**          .error.err_code                 One of the following error numbers.
**                                          E_DM0000_OK                
**                                          E_DM000C_BAD_CB_TYPE
**                                          E_DM000B_BAD_CB_LENGTH
**                                          E_DM001A_BAD_FLAG
**                                          E_DM002D_BAD_SERVER_ID
**                                          E_DM003F_DB_OPEN
**                                          E_DM004A_INTERNAL_ERROR
**                                          E_DM005B_SESSION_OPEN
**                                          E_DM0065_USER_INTR
**                                          E_DM0064_USER_ABORT
**					    E_DM0083_ERROR_STOPPING_SERVER
**                                          E_DM0112_RESOURCE_QUOTA_EXCEEDED
**
**      Returns:
**          E_DB_OK                         Function completed normally. 
**          E_DB_WARN                       Function completed normally with 
**                                          a termination status which is in 
**                                          dmc_cb.error.err_code.
**          E_DB_ERROR                      Function completed abnormally with 
**                                          a termination status which is in
**                                          dmc_cb.error.err_code.
**          E_DB_FATAL                      Function completed with a fatal
**                                          error which must be handled
**                                          immediately.  The fatal status is in
**                                          dmc_cb.error.err_code.
** History:
**      01-sep-1985 (jennifer)
**          Created for Jupiter.
**	18-nov-1985 (derek)
**	    Completed code.
**	28-feb-1989 (rogerk)
**	    Added support for shared buffer manager.
**	    Use new LBMCB to print stats, not BMCB.
**	 3-apr-1989 (rogerk)
**	    Moved dm0p_deallocate call to above dm0l_deallocate.
**	    Just in case BM is corrupt and can't be closed, we should not
**	    exit normally from the logging system.
**	20-sep-1993 (bryanp)
**	    If only a minimal DMF was started (i.e., this is STAR), then don't
**		attempt to stop anything, because nothing was started. Just
**		quickly return OK. We detect this because the svcb_bmcb_ptr is
**		NULL, which is rather a cheesy technique, but it works.
**	06-oct-93 (swm)
**	    Bug #56437
**	    Eliminated i4 cast since dmc_id is a PTR.
**	31-jan-1994 (bryanp) B58376, B58377, B58378
**	    Log LK status if LK call fails.
**	    Check return code from dm0m_destroy.
**	    log scf_error.err_code if scf_call fails.
**  	24-oct-1995 (thaju02/stoli02)
**          Added single and group synchronous writes statistics printout.
**	06-Dec-1995 (jenjo02)
**	    Added display of LK/LG statistics.
**	06-mar-1996 (stial01)
**	    Variable Page Size project:
**	    dmc_stop_server() Display statistics for all buffer pools.
**	25-mar-96 (nick)
**	    Add FC wait statistic.
**	19-Apr-1996 (jenjo02)
**	    Added display of lbm_bmcwait, BM semaphore to BM stats.
**	11-Jul-1996 (jenjo02)
**	    Added LG buffer profile display.
**	24-Oct-1996 (jenjo02)
**	    Replaced hcb_mutex with hash_mutex, one for each hash entry.
**	    Added hcb_tq_mutex to protect TCB free queue.
**	09-Jan-1997 (jenjo02)
**	    When displaying LG/LK semaphore stats, display the full
**	    semaphore name instead of truncating it.
**      27-feb-1997 (stial01)
**          Release the single server lock list if it was allocated
**	01-Apr-1997 (jenjo02)
**	    Table cache priority project:
**	    Added new per-priority statistics.
**	01-May-1997 (jenjo02)
**	    Added group wait statistic.
**	03-Apr-1998 (jenjo02)
**	    Added new WriteBehind thread stats.
**	01-Sep-1998 (jenjo02)
**	  o Moved BUF_MUTEX waits out of bmcb_mwait, lbm_mwait, into DM0P_BSTAT where
**	    mutex waits by cache can be accounted for.
**	24-Aug-1999 (jenjo02)
**	    BM stats expanded to show counts by page type.
**      20-sept-2004 (huazh01)
**          Pull out the fix for b90691 as it has been 
**          rewritten. INGSRV2643, b111502.
**	31-Jan-2003 (jenjo02)
**	    bs_stat name changes for Cache Flush Agents.
**	15-Jan-2004 (schka24)
**	    HCB mutexes changed around, reflect here.
**	6-Feb-2004 (schka24)
**	    No more temp name mutex.
**	22-Apr-2005 (fanch01)
**	    Add dmfcsp_class_stop call for cluster dbms class failover.
**	06-May-2005 (fanch01)
**	    Wrap dmfcsp_class_stop call and related code in
**	    conf_CLUSTER_BUILD.  Breaks non-cluster builds because it pulls
**	    in symbols from dmfcsp which aren't available.
**      11-May-2005 (stial01)
**          Pass svcb->svcb_csp_dbms_id to dmfcsp_class_stop
**          Move this code to before dm0l_deallocate
**	15-Aug-2005 (jenjo02)
**	    Implement multiple DMF LongTerm memory pools and
**	    ShortTerm session memory.
**	15-Mar-2005 (jenjo02)
**	    Replace manual log stats reporting with call to
**	    common dmd_log_info().
**	30-Aug-2006 (jonj)
**	    Add HCB tcblimit, tcbreclaims to TABLE stats.
**	    Sum memory stats from all pools.
*/

DB_STATUS
dmc_stop_server(
DMC_CB    *dmc_cb)
{
    DMC_CB		    *dmc = dmc_cb;
    DM_SVCB		    *svcb = dmf_svcb;
    i4		    	    status = E_DB_ERROR;
    i4		    	    error, local_error;
    CL_SYS_ERR		    sys_err;
    SCF_CB		    scf_cb;
    LG_STAT	    	    lgstat;
    LK_MUTEX		    lkmutex;
    LG_MUTEX		    lgmutex;
    CS_SEM_STATS	    sstat;
    i4		    	    length;
    DMP_LBMCB		    *lbm = svcb->svcb_lbmcb_ptr;
    DM0P_BMCB		    *bm;
    i4  		    i;
    DM0P_BUFFER		    *b;
    STATUS		    sem_status;
    DM0P_BSTAT		    *bs;
    i4                      cache_ix;
    i4		    	    count_per_tick;
    DMP_HCB		    *hcb;
    u_i4		    alloc, dealloc, expand;
#if defined(conf_CLUSTER_BUILD)
    i4			csp_class_stop;
    char		*svr_class_name;
#endif

    if (!lbm)
	return (E_DB_OK);	    /* this is the star server. just return */

#if defined(conf_CLUSTER_BUILD)
    csp_class_stop = CXcluster_enabled() && !CXconfig_settings(CX_HAS_CSP_ROLE);
    if (csp_class_stop)
    {
	svr_class_name = PMgetDefault(3);
    }
#endif

    /*  Write DMF statistics to the log. */
    hcb = svcb->svcb_hcb_ptr;

    TRdisplay("\n%29*- DMF Statistics Report %28*-\n\n");
    TRdisplay("    SESSION:   Begin:     %8d   End:       %8d\n",
	svcb->svcb_stat.ses_begin, svcb->svcb_stat.ses_end);
    /* Sum allocs, deallocs, expands from all pools */
    alloc = dealloc = expand = 0;
    for ( i = 0; i < svcb->svcb_pools; i++ )
    {
	alloc   += svcb->svcb_stat.mem_alloc[i];
	dealloc += svcb->svcb_stat.mem_dealloc[i];
	expand  += svcb->svcb_stat.mem_expand[i];
    }
    TRdisplay("    MEMORY:    Allocate:  %8d   Deallocate:%8d   Expand:    %8d\n",
	alloc, dealloc, expand);
    TRdisplay("    DATABASE:  Open:      %8d   Close:     %8d   Add:       %8d\n",
	svcb->svcb_stat.db_open, svcb->svcb_stat.db_close,
	svcb->svcb_stat.db_add);
    TRdisplay("               Remove:    %8d\n",
	svcb->svcb_stat.db_delete);
    TRdisplay("    TRANSACT:  Begin:     %8d   Commit:    %8d   Abort:     %8d\n",
	svcb->svcb_stat.tx_begin, svcb->svcb_stat.tx_commit,
	svcb->svcb_stat.tx_abort);
    TRdisplay("               Savepoint: %8d\n",
	svcb->svcb_stat.tx_savepoint);
    TRdisplay("    TABLE:     Open:      %8d   Close:     %8d   Build:     %8d\n",
	svcb->svcb_stat.tbl_open, svcb->svcb_stat.tbl_close,
	svcb->svcb_stat.tbl_build);
    TRdisplay("               Validate:  %8d   Invalidate:%8d   Update:    %8d\n",
	svcb->svcb_stat.tbl_validate, svcb->svcb_stat.tbl_invalid,
	svcb->svcb_stat.tbl_update);
    TRdisplay("               Create:    %8d   Destroy:   %8d\n",
	svcb->svcb_stat.tbl_create, svcb->svcb_stat.tbl_destroy);
    TRdisplay("               TCBlimit:  %8d   TCBreclaim:%8d\n",
	hcb->hcb_tcblimit, hcb->hcb_tcbreclaim);
    TRdisplay("    UTILITY:   Create:    %8d   Destroy:   %8d\n",
	svcb->svcb_stat.utl_create, svcb->svcb_stat.utl_destroy);
    TRdisplay("               Modify:    %8d   Index:     %8d   Relocate:  %8d\n",
	svcb->svcb_stat.utl_modify, svcb->svcb_stat.utl_index,
	svcb->svcb_stat.utl_relocate);
    TRdisplay("    RECORD:    Get:       %8d   Put:       %8d   Delete:    %8d\n",
	svcb->svcb_stat.rec_get, svcb->svcb_stat.rec_put,
	svcb->svcb_stat.rec_delete);
    TRdisplay("               Replace:   %8d   Load:      %8d   Position:  %8d\n",
	svcb->svcb_stat.rec_replace, svcb->svcb_stat.rec_load,
	svcb->svcb_stat.rec_position);
    TRdisplay("               Pos_all:   %8d   Pos_range: %8d   Pos_exact: %8d\n",
	svcb->svcb_stat.rec_p_all, svcb->svcb_stat.rec_p_range,
	svcb->svcb_stat.rec_p_exact);
    TRdisplay("    BTREE:     LeafSplit: %8d   IxSplit:   %8d\n",
	svcb->svcb_stat.btree_leaf_split, svcb->svcb_stat.btree_index_split);
    TRdisplay("               Repos:     %8d   Pglock:    %8d   Nologging: %8d\n",
	svcb->svcb_stat.btree_repos, svcb->svcb_stat.btree_pglock,
	svcb->svcb_stat.btree_nolog); 
    TRdisplay("               Check Rng: %8d   Search:    %8d\n",
	svcb->svcb_stat.btree_repos_chk_range, svcb->svcb_stat.btree_repos_search);
    TRdisplay("    BUFFER:    Reclaim:   %8d   Cache_wait:%8d\n",
	lbm->lbm_lockreclaim, lbm->lbm_bmcwait);
    TRdisplay("               FC_flushes:%8d\n",
	lbm->lbm_fcflush);

    for (cache_ix = 0; cache_ix < DM_MAX_CACHE; cache_ix++)
    {
	if (!(bm = lbm->lbm_bmcb[cache_ix]))
	    continue;

	bs = &lbm->lbm_stats[cache_ix];
	TRdisplay("\n    %2dK:       Buffers:   %8d   Buckets:   %8d   Free_wait: %8d\n",
	    bm->bm_pgsize/1024, bm->bm_bufcnt,bm->bm_hshcnt, bs->bs_fwait);

	if (bm->bm_status & BM_WB_IN_CACHE)
	{
	    TRdisplay("               WB_flushes:%8d   AgentHWM:  %8d   Clones:    %8d\n",
		bs->bs_cache_flushes, bs->bs_cfa_hwm, bs->bs_cfa_cloned);
	}

	TRdisplay("               G_count:   %8d   G_size:    %8d   G_sync:    %8d   G_wait:    %8d\n",
	    bm->bm_gcnt, bm->bm_gpages, bs->bs_gsyncwr, bs->bs_gwait); 
	    
	TRdisplay("               Fix:       %8d   Hit:       %8d   Read:      %8d\n",
	    bs->bs_fix[BMCB_PTYPES], bs->bs_hit[BMCB_PTYPES],
	    bs->bs_reads[BMCB_PTYPES]);
	TRdisplay("               Unfix:     %8d   Dirty:     %8d   Force:     %8d\n",
	    bs->bs_unfix[BMCB_PTYPES], bs->bs_dirty[BMCB_PTYPES],
	    bs->bs_force[BMCB_PTYPES]);
	TRdisplay("               Write:     %8d   Toss:      %8d   Reclaim:   %8d\n",
	    bs->bs_writes[BMCB_PTYPES], bs->bs_toss[BMCB_PTYPES],
	    bs->bs_reclaim[BMCB_PTYPES]);
	TRdisplay("               Check:     %8d   Refresh:   %8d   Sync_write:%8d\n",
	    bs->bs_check[BMCB_PTYPES], bs->bs_refresh[BMCB_PTYPES],
	    bs->bs_syncwr[BMCB_PTYPES]);
	TRdisplay("               M_wait:    %8d   IO_wait:   %8d   FC_wait:   %8d\n",
	    bs->bs_mwait[BMCB_PTYPES], bs->bs_iowait[BMCB_PTYPES],
	    bs->bs_fcwait[BMCB_PTYPES]);
	TRdisplay("               Replace:   %8d   G_read:    %8d   G_write:   %8d\n",
	    bs->bs_replace[BMCB_PTYPES],
	    bs->bs_greads[BMCB_PTYPES], bs->bs_gwrites[BMCB_PTYPES]);
	TRdisplay("               GW_pages:  %8d   GW_IO:     %8d\n",
	    bs->bs_gw_pages, bs->bs_gw_io);

	/* Show stats by page type: */
	for (i = 0; i < BMCB_PTYPES; i++)
	{
	    if (bs->bs_fix[i] || bs->bs_reclaim[i])
	    {
		TRdisplay("       Stats for %w pages:\n",
		    "FHDR,FMAP,ROOT,INDEX,LEAF,DATA", i);
		TRdisplay("               Fix:       %8d   Hit:       %8d   Read:      %8d\n",
		    bs->bs_fix[i], bs->bs_hit[i],
		    bs->bs_reads[i]);
		TRdisplay("               Unfix:     %8d   Dirty:     %8d   Force:     %8d\n",
		    bs->bs_unfix[i], bs->bs_dirty[i],
		    bs->bs_force[i]);
		TRdisplay("               Write:     %8d   Toss:      %8d   Reclaim:   %8d\n",
		    bs->bs_writes[i], bs->bs_toss[i],
		    bs->bs_reclaim[i]);
		TRdisplay("               Check:     %8d   Refresh:   %8d   Sync_write:%8d\n",
		    bs->bs_check[i], bs->bs_refresh[i],
		    bs->bs_syncwr[i]);
		TRdisplay("               M_wait:    %8d   IO_wait:   %8d   FC_wait:   %8d\n",
		    bs->bs_mwait[i], bs->bs_iowait[i],
		    bs->bs_fcwait[i]);
		TRdisplay("               Replace:   %8d   G_read:    %8d   G_write:   %8d\n",
		    bs->bs_replace[i],
		    bs->bs_greads[i], bs->bs_gwrites[i]);
	    }
	}
    }

    TRdisplay("\n%29*- LG Statistics %28*-\n\n");

    dmd_log_info(DMLG_STATISTICS | DMLG_BUFFER_UTIL);

    status = LGshow(LG_S_MUTEX, (PTR)&lgmutex, sizeof(lgmutex), &length, &sys_err);

    if (status)
    {
	ule_format(status, (CL_ERR_DESC *)&sys_err,
		    ULE_LOG, NULL, 0, 0, 0, &error, 0);
	TRdisplay("Can't show logging semaphore statistics.\n\n");
    }
    else
    {
	for (i = 0; i < lgmutex.count; i++)
	{
	TRdisplay("    %32s Requests  Exclusive:  %8d Shared: %8d\n",
		  lgmutex.stats[i].name,
		  lgmutex.stats[i].excl_requests,
		  lgmutex.stats[i].share_requests);
	TRdisplay("   %33*  Collisions Exclusive: %8d Shared: %8d\n",
		  lgmutex.stats[i].excl_collisions,
		  lgmutex.stats[i].share_collisions);
	TRdisplay("   %33*  Wait loops: %8d Redispatches: %8d\n",
		  lgmutex.stats[i].spins,
		  lgmutex.stats[i].redispatches);
	}
    }

    TRdisplay("\n");

    /*
    ** Format and display LK statistics
    */
    dmd_lock_info(DMTR_LOCK_STATISTICS | DMTR_SMALL_HEADER);

    /*
    ** Display LK semaphore statistics
    */
    status = LKshow(LK_S_MUTEX, 0, 0, 0, sizeof(lkmutex), 
		    (PTR)&lkmutex, (u_i4 *)&length, (u_i4 *)&error,
		    &sys_err); 
    if (status)
    {
	ule_format(status, (CL_ERR_DESC *)&sys_err,
		    ULE_LOG, NULL, 0, 0, 0, &error, 0);
	TRdisplay("Can't show locking semaphore statistics.\n");
    }
    else
    {
	TRdisplay("\n");
	for (i = 0; i < lkmutex.count; i++)
	{
	TRdisplay("    %32s Requests  Exclusive:  %8d Shared: %8d\n",
		  lkmutex.stats[i].name,
		  lkmutex.stats[i].excl_requests,
		  lkmutex.stats[i].share_requests);
	TRdisplay("   %33*  Collisions Exclusive: %8d Shared: %8d\n",
		  lkmutex.stats[i].excl_collisions,
		  lkmutex.stats[i].share_collisions);
	TRdisplay("   %33*  Wait loops: %8d Redispatches: %8d\n",
		  lkmutex.stats[i].spins,
		  lkmutex.stats[i].redispatches);
	}
    }

    TRdisplay("%80*-\n\n");

    for (;;)
    {
        /*	Verify control block parameters. */

	error = E_DM000C_BAD_CB_TYPE;
	if (dmc->dmc_op_type != DMC_SERVER_OP)
	    break;
	error = E_DM0070_SERVER_STOPPED;
	if (svcb == 0 || (svcb->svcb_status & SVCB_ACTIVE) == 0)
	    break;
	error = E_DM002D_BAD_SERVER_ID;
	if (dmc->dmc_id != svcb->svcb_id)
	    break;
	error = E_DM005B_SESSION_OPEN;
	if (svcb->svcb_cnt.cur_session)
	    break;
	error = E_DM003F_DB_OPEN;
	if (svcb->svcb_cnt.cur_database)
	    break;

	/* Destroy/deallocate any remaining DML_SEQ's */
	if ( svcb->svcb_seq )
	    dms_stop_server();

	/*
	** Close the buffer manager and deallocate the BMCB.  Do this
	** before notifying the logging system as we should not shut down
	** normally if the buffer manager cannot be closed.
	*/
	status = dm0p_deallocate(&error);
	if (status != E_DB_OK)
	    break;

	/* Release HCB's mutexes */
	hcb = svcb->svcb_hcb_ptr;
	dm0s_mrelease(&hcb->hcb_tq_mutex);
	for (i = 0; i < HCB_MUTEX_ARRAY_SIZE; i++)
	{
	    dm0s_mrelease(&hcb->hcb_mutex_array[i]);
	}

	/*  Deallocate the HCB. */
	dm0m_deallocate((DM_OBJECT **)&svcb->svcb_hcb_ptr);


#if defined(conf_CLUSTER_BUILD)
	if ( csp_class_stop )
	{
	    /*
	    ** Notify the CSP of the class stop.
	    ** Do this before we free the log context
	    */
	    status = dmfcsp_class_stop(svr_class_name, CXnode_number(NULL), 
	    		dmc->dmc_id, svcb->svcb_csp_dbms_id);
	    if (status != OK)
	    {
		error = E_DM0083_ERROR_STOPPING_SERVER;
		break;
	    }
	}
#endif

	/*  Deallocate the LCTX. */

	dm0s_mrelease(&svcb->svcb_lctx_ptr->lctx_mutex);
	status = dm0l_deallocate(&svcb->svcb_lctx_ptr, &error);
	if (status != E_DB_OK)
	    break;

	/*  Release the server lock list. */

	status = LKrelease(LK_ALL, svcb->svcb_lock_list, (LK_LKID *)0, 
            (LK_LOCK_KEY *)0,
	    (LK_VALUE *)0, &sys_err);
	if (status != OK)
	{
	    ule_format( status, &sys_err, ULE_LOG , NULL, 
	            (char * )0, 0L, (i4 *)0, 
                    &local_error, 0);
	    ule_format( E_DM901B_BAD_LOCK_RELEASE, &sys_err, ULE_LOG , NULL, 
	            (char * )0, 0L, (i4 *)0, 
                    &local_error, 1, 0, svcb->svcb_lock_list);
	    error = E_DM0083_ERROR_STOPPING_SERVER;
	    break;
	}

	if (svcb->svcb_ss_lock_list != NULL)
	{
	    /*  Release the single server lock list. */

	    status = LKrelease(LK_ALL, svcb->svcb_ss_lock_list, (LK_LKID *)0, 
		(LK_LOCK_KEY *)0,
		(LK_VALUE *)0, &sys_err);
	    if (status != OK)
	    {
		ule_format( status, &sys_err, ULE_LOG , NULL, 
			(char * )0, 0L, (i4 *)0, 
			&local_error, 0);
		ule_format( E_DM901B_BAD_LOCK_RELEASE, &sys_err, ULE_LOG , NULL, 
			(char * )0, 0L, (i4 *)0, 
			&local_error, 1, 0, svcb->svcb_lock_list);
		error = E_DM0083_ERROR_STOPPING_SERVER;
		break;
	    }
	}

	/*  Deallocate mutexes. */

	dm0s_mrelease(&svcb->svcb_dq_mutex);
	dm0s_mrelease(&svcb->svcb_sq_mutex);

	/*  Destroy LongTerm memory pools */

	(VOID) dm0m_destroy((DML_SCB*)NULL, &error);

	/*  Deallocate the DMF_SVCB. */

	scf_cb.scf_type = SCF_CB_TYPE;
	scf_cb.scf_length = sizeof(SCF_CB);
	scf_cb.scf_session = DB_NOSESSION;
	scf_cb.scf_facility = DB_DMF_ID;
	scf_cb.scf_scm.scm_functions = 0;
	scf_cb.scf_scm.scm_in_pages = 
                (sizeof(DM_SVCB) + SCU_MPAGESIZE - 1) / SCU_MPAGESIZE;
	scf_cb.scf_scm.scm_addr = (char *)dmf_svcb;
	status = scf_call(SCU_MFREE, &scf_cb);
	if (status != E_DB_OK)
	{
	    ule_format(scf_cb.scf_error.err_code, (CL_SYS_ERR *)0, 
		ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, 
		&local_error, 0);
	    error = E_DM0083_ERROR_STOPPING_SERVER;
	    break;
	}

	/*  Mark DMF_SVCB as deallocated. */
	dmf_svcb = 0;

	return (E_DB_OK);
    }

    dmc->error.err_code = error;
    return (E_DB_ERROR);
}

/*{
** Name: display_sem - display a semaphore's statistics
**
** Description: Displays a semaphore's statistics
**
** Inputs:
**      sem				    Semaphore in question 
**
** Output:				    None
**
** History:
**	06-Dec-1995 (jenjo02)
**	    Created.
*/
VOID
display_sem(
CS_SEMAPHORE *sem)
{
CS_SEM_STATS 	sstat;

    if ((CSs_semaphore(CS_INIT_SEM_STATS, sem, &sstat, sizeof(sstat))) == OK)
    {
	TRdisplay("    %12s Requests  Exclusive:  %8d Shared: %8d\n",
			    sstat.name,
			    sstat.excl_requests,
			    sstat.share_requests);
	TRdisplay("                 Collisions Exclusive: %8d Shared: %8d\n", 
				    sstat.excl_collisions,
				    sstat.share_collisions);

	if (sstat.type == CS_SEM_MULTI)
	{
	    TRdisplay("                 Wait loops: %8d Redispatches: %8d\n",
					sstat.spins,
					sstat.redispatches);
	}
    }

    return;

}
