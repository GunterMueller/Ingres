/*
**Copyright (c) 2004 Ingres Corporation
*/

/*
** NO_OPTIM = nc4_us5
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <di.h>
#include    <pc.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <lg.h>
#include    <lk.h>
#include    <ulf.h>
#include    <adf.h>
#include    <dmf.h>
#include    <dm.h>
#include    <dmp.h>
#include    <dm1b.h>
#include    <dm1p.h>
#include    <dm1cx.h>
#include    <dm1c.h>
#include    <dmve.h>
#include    <dm0p.h>
#include    <dmpp.h>
#include    <dm0l.h>
#include    <dmtcb.h>
#include    <dm2t.h>
#include    <dm2f.h>
#include    <dmftrace.h>

/**
**
**  Name: DMVEBTFREE.C - The recovery of a Btree Free Overflow operation.
**
**  Description:
**	This file contains the routines for the recovery of the action of
**	deallocating a leaf overflow page from a btree overflow chain.
**
**          dmve_btfree - The recovery of a Free Overflow operation.
**          dmve_rebtfree - Redo Recovery of a Free Overflow operation.
**          dmve_unbtfree - Undo Recovery of a Free Overflow operation.
**
**  History:
**	14-dec-1992 (rogerk)
**	    Written for Reduced Logging Project.
**	15-mar-1993 (jnash & rogerk)
**	    Reduced Logging - Phase IV:
**	    Changed log record format: added database_id, tran_id and LSN
**		in the log record header.  The LSN of a log record is now
**		obtained from the record itself rather than from the dmve
**		control block.
**	    Check dmve->dmve_logging to determine if logging required.
**	    Also, fix bug where *log_lsn, not lsn, inserted onto page 
**	    during REDO of UNDO.
**	12-apr-1993 (rogerk)
**	    Fixed problem in code which checked for splits which affect
**	    the key range of newly allocated overflow pages.  The duplicate
**	    key was being extracted from the wrong place in the btfree log
**	    record.   
**	15-apr-1993 (rogerk)
**	    Fixed coding bug which caused us to sometimes skip undo recovery.
**	26-apr-1993 (rogerk)
**	    Fixed undo routine to handle case where it is called in redo mode
**	    (redo of an undo of a btfree) when only the overflow page needs
**	    recovery (prev ptr is null).
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**		Correct the message parameter lengths for E_DM9665.
**	24-may-1993 (bryanp)
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**	21-jun-1993 (rogerk)
**	    Added error messages.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	26-jul-1993 (bryanp)
**	    Replace all uses of DM_LKID with LK_LKID.
**	26-jul-1993 (rogerk)
**	    Added tracing of page information during recovery.
**	15-apr-1994 (chiku)
**	    Bug56702: return logfull indication.
**      20-may-95 (hanch04)
**          Added include dmtcb.h for ndmfapi
**	06-mar-1996 (stial01 for bryanp)
**	    Pass page_size argument to dm1cxformat.
**      06-mar-1996 (stial01)
**          Limit kperpage to (DM_TIDEOF + 1) - DM1B_OFFSEQ
**          Pass btf_page_size to dmve_trace_page_info
**	06-may-1996 (thaju02 & nanpr01)
**	    New page format support: change page header references to use
**	    macros.
**      22-nov-96 (dilma04)
**          Row Locking Project:
**          Add lock_type argument to dm0p_lock_page and dm0p_unlock_page
**      04-feb-97 (stial01)
**          dmv_unbtfree() Tuple headers are on LEAF and overflow (CHAIN) pages
**          Tuple headers are not on INDEX pages.
**      27-feb-97 (stial01)
**          dmve_btundo_check renamed dmve_bid_check and parameters changed.
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          Add lock_id argument to dm0p_lock_page and dm0p_unlock_page.
**      21-may-1997 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
**      12-jun-97 (stial01)
**          Pass new attribute, key parameters to dm1cxformat
**      30-jun-97 (stial01)
**          Don't unfix tabio for bid check, changed args to dmve_bid_check()
**          Removed unecessary physical lock request/release code.
**	25-Aug-1997 (jenjo02)
**	    Added log_id parm to dm2t_ufx_tabio_cb() calls.
**      06-jan-98 (stial01)
**          Unfix pages,tbio before bid check (B87385) 
**	23-Jun-1998 (jenjo02)
**	    Added lock_value parm to dm0p_lock_page() and dm0p_unlock_page().
**	    Utilize lock_id value in both of those functions.
**      07-jul-98 (stial01)
**          Changed parameter to dmve_bid_check
**	14-aug-1998 (nanpr01)
**	    Error code is getting reset to E_DB_OK on error and not making
**	    the database inconsistent.
**      07-Dec-1998 (stial01)
**          Added kperleaf to distinguish from kperpage (keys per index page)
**      09-feb-99 (stial01)
**          dmv_unbtfree() Pass relcmptlvl = 0 to dm1cxkperpage
**      22-jun-1999 (musro02)
**          Added NO_OPTIM for NCR (nc4_us5) to fix error:
**                invalid register for instruction: %bp in testb
**      23-oct-2000 (stial01)
**          Variable Page Size - Variable Page Type support (SIR 102955)
**      08-dec-1999 (stial01)
**          Undo change for bug#85156, dmpe routines now get INTENT table locks.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      01-feb-2001 (stial01)
**          Pass pg type to dmve_trace_page_info (Variable Page Type SIR 102955)
**      19-Jun-2002 (horda03) Bug 108074
**          Prevent FORCE_ABORT transaction filling TX log
**          file by not flushing SCONCUR pages unless we
**          really need to (i.e. the table isn't locked
**          exclusively).
**	16-Dec-2004 (jenjo02)
**	    Added support for Global/partitioned indexes.
**      31-jan-05 (stial01)
**          Added flags arg to dm0p_mutex call(s).
**	06-Mar-2007 (jonj)
**	    Replace dm0p_cachefix_page() with dmve_cachefix_page()
**	    for Cluster REDO recovery.
**      14-jan-2008 (stial01)
**          Call dmve_unfix_tabio instead of dm2t_ufx_tabio_cb
**/

/*
** Forward Declarations.
*/

static DB_STATUS dmv_rebtfree(
			DMVE_CB             *dmve,
			DMP_TABLE_IO	    *tabio,
			DM1B_INDEX	    *ovfl,
			DM1B_INDEX	    *prev,
			DM0L_BTFREE	    *log_rec,
			DMPP_ACC_PLV	    *loc_plv,
			i4             *err_code);

static DB_STATUS dmv_unbtfree(
			DMVE_CB             *dmve,
			DMP_TABLE_IO	    *tabio,
			DM1B_INDEX	    *ovfl,
			DM1B_INDEX	    *prev,
			DM0L_BTFREE	    *log_rec,
			DMPP_ACC_PLV	    *loc_plv,
			i4             *err_code);


/*{
** Name: dmve_btfree - The recovery of a Btree Free Overflow operation.
**
** Description:
**
** Inputs:
**	dmve_cb
**	    .dmve_log_rec	    The log record of the alloc operation.
**	    .dmve_action	    Should be DMVE_DO, DMVE_REDO, or DMVE_UNDO
**	    .dmve_dcb_ptr	    Pointer to DCB.
**	    .dmve_tran_id	    The physical transaction id.
**	    .dmve_lk_id		    The transaction lock list id.
**	    .dmve_log_id	    The logging system database id.
**	    .dmve_db_lockmode	    The lockmode of the database. Should be 
**				    DM2T_X or DM2T_S.
**
** Outputs:
**	dmve_cb
**	    .dmve_error.err_code    The reason for error status.
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	14-dec-1992 (jnash & rogerk)
**	    Reduced Logging Project: Written with new 6.5 recovery protocols.
**	15-mar-1993 (rogerk)
**	    Reduced Logging - Phase IV:  Added LSN argument to btundo_check.
**	12-apr-1993 (rogerk)
**	    Fixed problem in code which checked for splits which affect
**	    the key range of newly allocated overflow pages.  The duplicate
**	    key was being extracted from the wrong place in the btfree log
**	    record.  Also fixed code which attempted to unfix any pages
**	    before calling undo check to correctly unfix the overflow page
**	    (ovfl_page) rather than trying to unfix prev_page twice.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**		Correct the message parameter lengths for E_DM9665.
**	26-jul-1993 (bryanp)
**	    Replace all uses of DM_LKID with LK_LKID.
**      11-sep-1997 (thaju02) bug#85156 - on recovery lock extension
**          table with DM2T_S.  Attempting to lock table with DM2T_IX
**          causes deadlock, dbms resource deadlock error during recovery,
**          pass abort, rcp resource deadlock error during recovery,
**          rcp marks database inconsistent.
**	06-may-1996 (thaju02 & nanpr01)
**	    New Page Format Support: 
**		Change page header references to use macros.
**		Pass page size to dm1c_getaccessors().
**      22-nov-96 (dilma04)
**          Row Locking Project:
**          Add lock_type argument to dm0p_lock_page and dm0p_unlock_page
**      27-feb-97 (stial01)
**          dmve_btundo_check renamed dmve_bid_check and parameters changed.
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          Add lock_id argument to dm0p_lock_page and dm0p_unlock_page.
**      30-jun-97 (stial01)
**          Don't unfix tabio for bid check, changed args to dmve_bid_check()
**          Removed unecessary physical lock request/release code.
**          When logging DM0L_BTFREE records, we never set DM0L_PHYS_LOCK.
**          Generally DM0L_PHYS_LOCK is set for INDEX not LEAF pages.
**          The recovery of a Btree Free Overflow operation is done only 
**          on LEAF pages.
**      06-jan-98 (stial01)
**          Unfix pages,tbio before bid check (B87385) 
**      19-Jun-2002 (horda03) Bug 108074
**          If the table is locked exclusively, then indicate that SCONCUR
**          pages don't need to be flushed immediately.
**      23-feb-2004 (thaju02) Bug 111470 INGSRV2635
**          For rollforwarddb -b option, do not compare the LSN's on the
**          page to that of the log record.
**	01-Dec-2006 (kiria01) b117225
**	    Initialise the lockid parameters that will be passed to LKrequest
**	    to avoid random implicit lock conversions.
*/
DB_STATUS
dmve_btfree(
DMVE_CB		*dmve_cb)
{
    DMVE_CB		*dmve = dmve_cb;
    DM0L_BTFREE		*log_rec = (DM0L_BTFREE *)dmve->dmve_log_rec;
    LG_LSN		*log_lsn = &log_rec->btf_header.lsn; 
    DMP_DCB		*dcb = dmve->dmve_dcb_ptr;
    DMP_TABLE_IO	*tbio = NULL;
    DM1B_INDEX		*ovfl = NULL;
    DM1B_INDEX		*prev = NULL;
    DMPP_PAGE		*ovfl_page = NULL;
    DMPP_PAGE		*prev_page = NULL;
    DB_STATUS		status = E_DB_OK;
    DB_STATUS		tmp_status;
    DMPP_ACC_PLV	*loc_plv;
    DMPP_ACC_TLV	*loc_tlv;
    LK_LKID		lockid;
    LK_LKID		ovfl_lockid;
    LK_LKID		prev_lockid;
    DM_TID		leaf_bid;
    DM_TID		tid;
    i4		lock_action;
    i4		grant_mode;
    i4		recovery_action;
    i4		error;
    i4		loc_error;
    char		*duplicate_key;
    DM_PAGENO		prev_page_number;
    i4		prev_loc_id;
    i4		loc_id;
    i4		tbl_lock_mode;
    i4             lock_type = LK_TABLE;
    bool		ovfl_page_recover;
    bool		prev_page_recover;
    bool		bid_check_done = FALSE;
    bool                physical_ovfl_page_lock = FALSE;
    bool                physical_prev_page_lock = FALSE;
    bool                buf_locked = FALSE;
    i4                  fix_action = 0;

    MEfill(sizeof(LK_LKID), 0, &lockid);
    MEfill(sizeof(LK_LKID), 0, &ovfl_lockid);
    MEfill(sizeof(LK_LKID), 0, &prev_lockid);

    /*
    ** Store previous page number into a local variable.  This value may
    ** be modified in undo recovery by the dmve_bid_check routine.
    */
    prev_page_number = log_rec->btf_prev_pageno;

    for (;;)
    {
	/*
	** Consistency Check:  check for illegal log records.
	*/
	if ((log_rec->btf_header.type != DM0LBTFREE) ||
	    (log_rec->btf_header.length != 
                (sizeof(DM0L_BTFREE) -
                        (DM1B_KEYLENGTH - log_rec->btf_lrange_len) - 
                        (DM1B_KEYLENGTH - log_rec->btf_rrange_len) - 
                        (DM1B_KEYLENGTH - log_rec->btf_dupkey_len))))
	{
	    error = E_DM9601_DMVE_BAD_PARAMETER;
	    break;
	}


	/*
	** Get handle to a tableio control block with which to read
	** and write pages needed during recovery.
	**
	** Warning return indicates that no tableio control block was
	** built because no recovery is needed on any of the locations 
	** described in this log record.
	*/
	status = dmve_fix_tabio(dmve, &log_rec->btf_tbl_id, &tbio, &error);
	if (DB_FAILURE_MACRO(status))
	    break;
	if ((status == E_DB_WARN) && (error == W_DM9660_DMVE_TABLE_OFFLINE))
	    return (E_DB_OK);

	tbl_lock_mode = DM2T_IX;

	/*
	** Get page accessors for page recovery actions.
	*/
	dm1c_getaccessors(TCB_C_DEFAULT, log_rec->btf_pg_type, 
		log_rec->btf_page_size, &loc_plv, &loc_tlv);

	/*
	** Check recovery requirements for this operation.
	** Note that the previous page we are using may be different than
	** the one logged in the original operation.
	*/
	loc_id = DM2F_LOCID_MACRO(tbio->tbio_loc_count,
						(i4) prev_page_number);
	prev_loc_id = tbio->tbio_location_array[loc_id].loc_config_id;

	ovfl_page_recover = dmve_location_check(dmve, 
					(i4)log_rec->btf_ovfl_loc_id);
	prev_page_recover = dmve_location_check(dmve, prev_loc_id);

	/*
	** Check for partial recovery during UNDO operations.
	** We cannot bypass recovery of the leaf page even if the page
	** is on a location which is not being recovered.  This is because
	** the logged page number may not be the page that actually needs
	** the UNDO action!!!  If the page which was originally updated
	** has been since SPLIT, then the ovfl chain may have moved to a
	** new leaf page.  It is that new page to which this undo
	** action must be applied.  And that new page may be on a location
	** being recovered even if the original page is not.
	**
	** If recovery is being bypassed on the entire table then no recovery
	** needs to be done.
	*/
	if ((dmve->dmve_action == DMVE_UNDO) &&
	    (! prev_page_recover))
	{
            ule_format(E_DM9668_TABLE_NOT_RECOVERED, (CL_ERR_DESC *)0,
                ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0,
                &loc_error, 2, 0, log_rec->btf_tbl_id.db_tab_base,
                0, log_rec->btf_tbl_id.db_tab_index);
            error = E_DM9667_NOPARTIAL_RECOVERY;
	    break;
	}

	/*
	** Get required Table/Page locks before we can start the updates.
	** Note that if the database is locked exclusively, or if an X table
	** lock is granted then no page locks are requried.
	*/
	if ((dcb->dcb_status & DCB_S_EXCLUSIVE) == 0)
	{
	    /*
	    ** Request IX lock in preparation of requesting an X page lock
	    ** below.  If the transaction already holds an exclusive table
	    ** lock, then an X lock will be granted.  In this case we can
	    ** bypass the page lock request.
	    */
	    status = dm2t_lock_table(dcb, &log_rec->btf_tbl_id, tbl_lock_mode, 
		dmve->dmve_lk_id, (i4)0, &grant_mode, &lockid, &error);
	    if (status != E_DB_OK)
		break;

	    if (grant_mode != DM2T_X)
	    {
		/*        
		** Page locks required.
		*/
		lock_action = LK_LOGICAL;
		lock_type = LK_PAGE;
	
		/*
		** Lock the overflow page.
		*/
		if (ovfl_page_recover)
		{
		    status = dm0p_lock_page(dmve->dmve_lk_id, dcb, 
			&log_rec->btf_tbl_id, log_rec->btf_ovfl_pageno,
			LK_PAGE, LK_X, lock_action, (i4)0, 
                        tbio->tbio_relid, &dmve->dmve_tran_id, &ovfl_lockid, 
                        (i4 *)0, (LK_VALUE *)0, &error);
		    if (status != E_DB_OK)
			break;

		}

		/*
		** Lock the previous overflow page.
		*/
		if (prev_page_recover)
		{
		    status = dm0p_lock_page(dmve->dmve_lk_id, dcb, 
			&log_rec->btf_tbl_id, prev_page_number, 
			LK_PAGE,
			LK_X, lock_action, (i4)0, tbio->tbio_relid, 
			&dmve->dmve_tran_id, &prev_lockid,
                        (i4 *)0, (LK_VALUE *)0, &error);
		    if (status != E_DB_OK)
			break;
		}
	    }
            else
               fix_action |= DM0P_TABLE_LOCKED_X;
	}

	/*
	** Fix the pages we need to recover in cache for write.
	*/
	if (ovfl_page_recover)
	{
	    status = dmve_cachefix_page(dmve, log_lsn,
					tbio, log_rec->btf_ovfl_pageno,
					fix_action, loc_plv,
					&ovfl_page, &error);
	    if (status != E_DB_OK)
		break;
	}

	if (prev_page_recover)
	{
	    status = dmve_cachefix_page(dmve, log_lsn,
					tbio, prev_page_number,
					fix_action, loc_plv,
					&prev_page, &error);
	    if (status != E_DB_OK)
		break;
	}
	ovfl = (DM1B_INDEX *)ovfl_page;
	prev = (DM1B_INDEX *)prev_page;

	/*
	** Dump debug trace info about pages if such tracing is configured.
	*/
	if (DMZ_ASY_MACRO(15))
	{
	    dmve_trace_page_info(log_rec->btf_pg_type, log_rec->btf_page_size,
		(DMPP_PAGE *) ovfl, loc_plv, "OVFL"); 
	    dmve_trace_page_info(log_rec->btf_pg_type, log_rec->btf_page_size,
		(DMPP_PAGE *) prev, loc_plv, "PREV"); 
	}

	/*
	** UNDO Recovery SPLIT check:
	**
	** If doing UNDO recovery, we need to verify that we have the correct 
	** previous page onto which to link the new overflow page.  If the
	** parent leaf page of this overflow chain has been split since the
	** overflow page was freed then the overflow chain may now belong
	** to a different leaf chain.
	**
	** If the previous page is a LEAF and it has been split since the
	** free operation was logged then perform a logical search to find
	** the spot to which the overflow chain should be put.
	*/
	if ((dmve->dmve_action == DMVE_UNDO) &&
	    (prev_page_recover) &&
	    (DM1B_VPT_GET_PAGE_STAT_MACRO(log_rec->btf_pg_type, prev) & 
		DMPP_LEAF) &&
	    (LSN_GT(DM1B_VPT_ADDR_BT_SPLIT_LSN_MACRO(log_rec->btf_pg_type, prev),
		log_lsn)) &&
	    ( ! bid_check_done))
	{
	    /*
	    ** Release our current page and the tbio and call bid_check
	    ** to perform a logical search for the desired row.
	    */
	    status = dm0p_uncache_fix(tbio, DM0P_UNFIX, dmve->dmve_lk_id, 
		dmve->dmve_log_id, &dmve->dmve_tran_id, &prev_page, &error);
	    if (status != E_DB_OK)
		break;

	    if (ovfl_page_recover)
	    {
		status = dm0p_uncache_fix(tbio, DM0P_UNFIX, dmve->dmve_lk_id, 
		    dmve->dmve_log_id, &dmve->dmve_tran_id, &ovfl_page, &error);
		if (status != E_DB_OK)
		    break;
	    }

	    if (physical_prev_page_lock)
	    {
		status = dm0p_unlock_page(dmve->dmve_lk_id, dcb, 
		    &log_rec->btf_tbl_id, prev_page_number, 
		    LK_PAGE, 
		    tbio->tbio_relid, &dmve->dmve_tran_id, &prev_lockid, 
		    (LK_VALUE *)0, &error);
		if (status != E_DB_OK)
		    break;

		physical_prev_page_lock = FALSE;
	    }

	    if (physical_ovfl_page_lock)
	    {
		status = dm0p_unlock_page(dmve->dmve_lk_id, dcb, 
		    &log_rec->btf_tbl_id, log_rec->btf_ovfl_pageno, 
		    LK_PAGE, 
		    tbio->tbio_relid, &dmve->dmve_tran_id, &ovfl_lockid, 
		    (LK_VALUE *)0, &error);
		if (status != E_DB_OK)
		    break;

		physical_ovfl_page_lock = FALSE;
	    }

	    status = dmve_unfix_tabio(dmve, &tbio, 0, &error);
	    if (status != E_DB_OK)
		break;
	    tbio = NULL;

	    duplicate_key = &log_rec->btf_vbuf[0];

	    leaf_bid.tid_tid.tid_page = log_rec->btf_prev_pageno;
	    leaf_bid.tid_tid.tid_line = 0;
	    tid.tid_i4 = 0;

	    status = dmve_bid_check(dmve, DMVE_FINDLEAF, 
		&log_rec->btf_tbl_id, &leaf_bid, &tid,
		duplicate_key, log_rec->btf_klen, 
		tbio, tbl_lock_mode, lock_type, lock_action,
		&buf_locked, &leaf_bid, &prev_page, &error);
	    if (status != E_DB_OK)
		break;

	    /*
	    ** Jump back to the start of the routine and start this recovery
	    ** process all over again using the BID returned from the
	    ** bid_check routine.  Set that we have done this action so
	    ** that we don't loop through here on the second pass.
	    */
	    prev_page_number = leaf_bid.tid_tid.tid_page;
	    bid_check_done = TRUE;
	    continue;
	}

	/*
	** Compare the LSN's on the pages with that of the log record
	** to determine what recovery will be needed.
	**
	**   - During Forward processing, if the page's LSN is greater than
	**     the log record then no recovery is needed.
	**
	**   - During Backward processing, it is an error for a page's LSN
	**     to be less than the log record LSN.
	**
	**   - Currently, during rollforward processing it is unexpected
	**     to find that a recovery operation need not be applied because
	**     of the page's LSN.  This is because rollforward must always
	**     begin from a checkpoint that is previous to any journal record
	**     begin applied.  In the future this requirement may change and
	**     Rollforward will use the same expectations as Redo.
	*/
	switch (dmve->dmve_action)
	{
	case DMVE_DO:
	case DMVE_REDO:
	    if (ovfl && LSN_GTE(
		DM1B_VPT_ADDR_PAGE_LOG_ADDR_MACRO(log_rec->btf_pg_type, ovfl), 
		log_lsn) && ((dmve->dmve_flags & DMVE_ROLLDB_BOPT) == 0))
	    {
		if (dmve->dmve_action == DMVE_DO)
		{
		    ule_format(E_DM9665_PAGE_OUT_OF_DATE, (CL_ERR_DESC *)0, 
			ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, 
			&loc_error, 8, 
			sizeof(*tbio->tbio_relid), tbio->tbio_relid,
			sizeof(*tbio->tbio_relowner), tbio->tbio_relowner, 0, 
			DM1B_VPT_GET_PAGE_PAGE_MACRO(log_rec->btf_pg_type, ovfl),
			0, 
			DM1B_VPT_GET_PAGE_STAT_MACRO(log_rec->btf_pg_type, ovfl),
			0, DM1B_VPT_GET_LOG_ADDR_HIGH_MACRO(log_rec->btf_pg_type,
			    ovfl), 
			0, DM1B_VPT_GET_LOG_ADDR_LOW_MACRO(log_rec->btf_pg_type,
			    ovfl),
			0, log_lsn->lsn_high, 0, log_lsn->lsn_low);
		}
		ovfl = NULL;
	    }
	    if (prev && LSN_GTE(
		DM1B_VPT_ADDR_PAGE_LOG_ADDR_MACRO(log_rec->btf_pg_type, prev), 
		log_lsn) && ((dmve->dmve_flags & DMVE_ROLLDB_BOPT) == 0))
	    {
		if (dmve->dmve_action == DMVE_DO)
		{
		    ule_format(E_DM9665_PAGE_OUT_OF_DATE, (CL_ERR_DESC *)0, 
			ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, 
			&loc_error, 8, 
			sizeof(*tbio->tbio_relid), tbio->tbio_relid,
			sizeof(*tbio->tbio_relowner), tbio->tbio_relowner, 0, 
			DM1B_VPT_GET_PAGE_PAGE_MACRO(log_rec->btf_pg_type, prev),
			0, DM1B_VPT_GET_PAGE_STAT_MACRO(log_rec->btf_pg_type, 
			    prev),
			0, DM1B_VPT_GET_LOG_ADDR_HIGH_MACRO(log_rec->btf_pg_type, 
			    prev), 
			0, DM1B_VPT_GET_LOG_ADDR_LOW_MACRO(log_rec->btf_pg_type,
			    prev),
			0, log_lsn->lsn_high, 0, log_lsn->lsn_low);
		}
		prev = NULL;
	    }
	    break;

	case DMVE_UNDO:
	    if (ovfl && (LSN_LT(
		DM1B_VPT_ADDR_PAGE_LOG_ADDR_MACRO(log_rec->btf_pg_type, ovfl), 
		log_lsn)))
	    {
		ule_format(E_DM9665_PAGE_OUT_OF_DATE, (CL_ERR_DESC *)0, 
		    ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, 
		    &loc_error, 8, 
		    sizeof(*tbio->tbio_relid), tbio->tbio_relid,
		    sizeof(*tbio->tbio_relowner), tbio->tbio_relowner,
		    0, DM1B_VPT_GET_PAGE_PAGE_MACRO(log_rec->btf_pg_type, ovfl),
		    0, DM1B_VPT_GET_PAGE_STAT_MACRO(log_rec->btf_pg_type, ovfl),
		    0, DM1B_VPT_GET_LOG_ADDR_HIGH_MACRO(log_rec->btf_pg_type, 
			ovfl), 
		    0, DM1B_VPT_GET_LOG_ADDR_LOW_MACRO(log_rec->btf_pg_type,
			ovfl),
		    0, log_lsn->lsn_high, 0, log_lsn->lsn_low);
		error = E_DM9666_PAGE_LSN_MISMATCH;
		status = E_DB_ERROR;
	    }
	    if (prev && (LSN_LT(
		DM1B_VPT_ADDR_PAGE_LOG_ADDR_MACRO(log_rec->btf_pg_type, prev), 
		log_lsn)))
	    {
		ule_format(E_DM9665_PAGE_OUT_OF_DATE, (CL_ERR_DESC *)0, 
		    ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, 
		    &loc_error, 8, 
		    sizeof(*tbio->tbio_relid), tbio->tbio_relid,
		    sizeof(*tbio->tbio_relowner), tbio->tbio_relowner,
		    0, DM1B_VPT_GET_PAGE_PAGE_MACRO(log_rec->btf_pg_type, prev),
		    0, DM1B_VPT_GET_PAGE_STAT_MACRO(log_rec->btf_pg_type, prev),
		    0, DM1B_VPT_GET_LOG_ADDR_HIGH_MACRO(log_rec->btf_pg_type, 
			prev), 
		    0, DM1B_VPT_GET_LOG_ADDR_LOW_MACRO(log_rec->btf_pg_type,
			prev),
		    0, log_lsn->lsn_high, 0, log_lsn->lsn_low);
		error = E_DM9666_PAGE_LSN_MISMATCH;
		status = E_DB_ERROR;
	    }
	    break;
	}
	if (status != E_DB_OK)
	    break;

	/*
	** Call appropriate recovery action depending on the recovery type
	** and record flags.  CLR actions are always executed as an UNDO
	** operation.
	*/
	recovery_action = dmve->dmve_action;
	if (log_rec->btf_header.flags & DM0L_CLR)
	    recovery_action = DMVE_UNDO;

	switch (recovery_action)
	{
	case DMVE_DO:
	case DMVE_REDO:
	    status = dmv_rebtfree(dmve, tbio, ovfl, prev, log_rec, 
		loc_plv, &error);
	    break;

	case DMVE_UNDO:
	    status = dmv_unbtfree(dmve, tbio, ovfl, prev, log_rec, 
		loc_plv, &error);
	    break;
	}
	break;
    }

    if (status != E_DB_OK)
    {
	ule_format(error, (CL_ERR_DESC *)0, ULE_LOG, NULL, 
		(char *)0, (i4)0, (i4 *)0, &error, 0);
    }

    /*
    ** Error handling: this implies that the abort action failed and the
    ** database will be marked inconsistent by the caller.
    */

    if (ovfl_page)
    {
	tmp_status = dm0p_uncache_fix(tbio, DM0P_UNFIX, dmve->dmve_lk_id, 
		dmve->dmve_log_id, &dmve->dmve_tran_id, 
		(DMPP_PAGE **) &ovfl_page, &loc_error);
	if (tmp_status != E_DB_OK)
	{
	    ule_format(loc_error, (CL_ERR_DESC *)0, ULE_LOG, NULL, 
		(char *)0, (i4)0, (i4 *)0, &loc_error, 0);
	    if (tmp_status > status)
	        status = tmp_status;
	}
    }

    if (prev_page)
    {
	tmp_status = dm0p_uncache_fix(tbio, DM0P_UNFIX, dmve->dmve_lk_id, 
		dmve->dmve_log_id, &dmve->dmve_tran_id, 
		(DMPP_PAGE **) &prev_page, &loc_error);
	if (tmp_status != E_DB_OK)
	{
	    ule_format(loc_error, (CL_ERR_DESC *)0, ULE_LOG, NULL, 
		(char *)0, (i4)0, (i4 *)0, &loc_error, 0);
	    if (tmp_status > status)
	        status = tmp_status;
	}
    }

    if (tbio)
    {
	tmp_status = dmve_unfix_tabio(dmve, &tbio, 0, &loc_error);
	if (tmp_status > status)
	    status = tmp_status;
    }

    if (status != E_DB_OK)
	dmve->dmve_error.err_code = E_DM9654_DMVE_BTREE_FREE;

    return(status);
}

/*{
** Name: dmv_rebtfree - Redo the Btree Free Overflow operation.
**
** Description:
**      This function redoes the updates to a btree required to free an
**	overflow leaf page.
**
** Inputs:
**      dmve				Pointer to dmve control block.
**      tabio				Pointer to table io control block
**	log_rec				Alloc log record.
**	plv				Pointer to page level accessor 
**
** Outputs:
**	error				Pointer to Error return area
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	14-dec-1992 (jnash & rogerk)
**	    Written for 6.5 recovery project.
**	15-apr-1993 (rogerk)
**	    Fixed coding bug which caused us to sometimes skip undo recovery.
**	    Only return immediately if BOTH ovfl and leaf are null, not if
**	    either is null.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**	06-may-1996 (thaju02 & nanpr01)
**	    New page format support: change page header references to use
**	    macros.
**      21-may-1997 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
*/
static DB_STATUS
dmv_rebtfree(
DMVE_CB             *dmve,
DMP_TABLE_IO	    *tabio,
DM1B_INDEX	    *ovfl,
DM1B_INDEX	    *prev,
DM0L_BTFREE	    *log_rec,
DMPP_ACC_PLV	    *loc_plv,
i4             *err_code)
{
    LG_LSN		*log_lsn = &log_rec->btf_header.lsn; 

    /*
    ** If recovery was found to be unneeded then we can just return.
    */
    if ((ovfl == NULL) && (prev == NULL))
	return (E_DB_OK);

    /*
    ** Mutex the overflow pages while updating them.
    */
    if (ovfl)
	dm0p_mutex(tabio, (i4)0, dmve->dmve_lk_id, (DMPP_PAGE **) &ovfl);
    if (prev)
	dm0p_mutex(tabio, (i4)0, dmve->dmve_lk_id, (DMPP_PAGE **) &prev);

    /*
    ** Redo the Deallocate operation.
    **
    ** Unlink the page from the overflow chain by changing the previous page's
    ** overflow pointer to point past the deallocated page.
    **
    ** The unlinked overflow page is not itself updated other than to move
    ** its LSN forward.  It should be formatted later as a Free page when
    ** the DEALLOC log record is processed.
    */
    if (prev)
    {
	DM1B_VPT_SET_PAGE_OVFL_MACRO(log_rec->btf_pg_type, prev,
	    log_rec->btf_nextovfl);

	DM1B_VPT_SET_PAGE_STAT_MACRO(log_rec->btf_pg_type, prev, DMPP_MODIFY);
	DM1B_VPT_SET_PAGE_LOG_ADDR_MACRO(log_rec->btf_pg_type, prev, *log_lsn);
    }

    if (ovfl)
    {
	DM1B_VPT_SET_PAGE_STAT_MACRO(log_rec->btf_pg_type, ovfl, DMPP_MODIFY);
	DM1B_VPT_SET_PAGE_LOG_ADDR_MACRO(log_rec->btf_pg_type, ovfl, *log_lsn);
    }

    if (ovfl)
	dm0p_unmutex(tabio, (i4)0, dmve->dmve_lk_id, (DMPP_PAGE **) &ovfl);
    if (prev)
	dm0p_unmutex(tabio, (i4)0, dmve->dmve_lk_id, (DMPP_PAGE **) &prev);
    
    return(E_DB_OK);
}

/*{
** Name: dmv_unbtfree - Undo the Btree Deallocate Overflow operation.
**
** Description:
**      This function undoes the updates to a btree done by the deallocation
**	of a Btree Overflow Leaf Page.
**
** Description:
**
** Inputs:
**      dmve				Pointer to dmve control block.
**      tabio				Pointer to table io control block
**	fhdr				Table's FHDR page.
**	fmap				Table's FMAP page.
**	log_rec				Alloc log record.
**	plv				Pointer to page level accessor 
**
** Outputs:
**	error				Pointer to Error return area
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	14-dec-1992 (jnash & rogerk)
**	    Written for 6.5 recovery project.
**	15-mar-1993 (jnash)
**	    Check dmve->dmve_logging to determine if logging required.
**	    Also, fix bug where *log_lsn, not lsn, moved into page_log_addr.
**	12-apr-1993 (rogerk)
**	    Fixed problem in code which checked for splits which affect
**	    the key range of newly allocated overflow pages.  The duplicate
**	    key was being extracted from the wrong place in the btfree log
**	    record.   
**	26-apr-1993 (rogerk)
**	    Fixed routine to handle case where it is called in redo mode
**	    (redo of an undo of a btfree) when only the overflow page needs
**	    recovery (prev ptr is null).  In redo mode the range key and
**	    other information subject to affect by splits should be extracted
**	    from the log record rather than from the previous page.  Partial
**	    recovery should only be disallowed in true UNDO operations.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**	15-apr-1994 (chiku)
**	    Bug56702: return logfull indication.
**	06-mar-1996 (stial01 for bryanp)
**	    Pass page_size argument to dm1cxformat.
**	06-may-1996 (thaju02 & nanpr01)
**	    New page format support: change page header references to use
**	    macros. Also pass the page_size param in DM1B_OVER macro.
**	    Changed btree key per page calculation to account for tuple
**	    header.
**      04-feb-97 (stial01)
**          Tuple headers are on LEAF and overflow (CHAIN) pages
**          Tuple headers are not on INDEX pages.
**      21-may-1997 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
**      12-jun-97 (stial01)
**          dmv_unbtfree() Pass new attribute, key parameters to dm1cxformat
**      09-feb-99 (stial01)
**          dmv_unbtfree() Pass relcmptlvl = 0 to dm1cxkperpage
**	19-Jan-2004 (jenjo02)
**	    Added btf_tidsize to log_rec for global indexes.
**	13-Apr-2006 (jenjo02)
**	    Add btf_ixklen to log_rec.
*/
static DB_STATUS
dmv_unbtfree(
DMVE_CB             *dmve,
DMP_TABLE_IO	    *tabio,
DM1B_INDEX	    *ovfl,
DM1B_INDEX	    *prev,
DM0L_BTFREE	    *log_rec,
DMPP_ACC_PLV	    *loc_plv,
i4             *err_code)
{
    LG_LSN		*log_lsn = &log_rec->btf_header.lsn; 
    LG_LSN		lsn; 
    DM_TID		lrange_tid;
    DM_TID		rrange_tid;
    i4		dm0l_flags;
    i4		kperpage;
    i4		loc_id;
    i4		prev_loc_id;
    i4		prev_pageno;
    i4		mainpageno;
    i4		nextpageno;
    i4		nextovfl;
    i4		lrange_len;
    i4		rrange_len;
    char		*lrange_ptr;
    char		*rrange_ptr;
    char		*dupkey_ptr;
    bool		is_index;
    DB_STATUS		status = E_DB_OK;

    /*
    ** If recovery was found to be unneeded then we can just return.
    */
    if ((ovfl == NULL) && (prev == NULL))
	return (E_DB_OK);

    /*
    ** During normal UNDO operations, we cannot recover the overflow page
    ** without having the leaf page to give us range key values.  The overflow
    ** page can't be recovered in an independent fashion because if a split
    ** occurred on the parent leaf after the dm0l_free was logged, the range
    ** keys which need to be restored to the overflow page may now be different
    ** than the key values in the dm0l_free log record.  In this case the 
    ** range keys must be copied off of the previous overflow page.
    **
    ** If this routine is called to redo an undo operation, then independent
    ** recovery is allowed - the values to use are always those recorded in
    ** the recovered log record.
    */
    if ((dmve->dmve_action == DMVE_UNDO) && (prev == NULL))
    {
	*err_code = E_DM9655_UNDO_BTREE_FREE;
	return (E_DB_ERROR);
    }

    /*
    ** Get possibly dynamic information needed for undo operation.
    **
    ** During REDO of and undo operation, we simply extract this infomration
    ** from the dm0l_free CLR.  During true UNDO, however, we must get the
    ** dynamic information from the previous overflow page.
    **
    ** During undo recover, we cannot use all of the logged information
    ** to restore the overflow page because this information may have been
    ** invalidated by a split to the primary leaf page.
    **
    ** If the leaf page has been split since we deallocated this overflow page,
    ** then the overflow chain's range keys have certainly been updated and
    ** it is even possible that the chain itself has moved to a new leaf page.
    **
    ** The caller will have verified that the correct previous page has been
    ** passed to us; we must extract the range keys and mainpage pointer
    ** from that page rather than using the logged values.
    */
    if (dmve->dmve_action == DMVE_UNDO)
    {
	is_index = ((DM1B_VPT_GET_PAGE_STAT_MACRO(log_rec->btf_pg_type, prev) &
			DMPP_INDEX) != 0);
	prev_pageno = DM1B_VPT_GET_PAGE_PAGE_MACRO(log_rec->btf_pg_type, prev);
	loc_id = DM2F_LOCID_MACRO(tabio->tbio_loc_count, (i4) prev_pageno);
	prev_loc_id = tabio->tbio_location_array[loc_id].loc_config_id;

	dm1cxrecptr(log_rec->btf_pg_type, log_rec->btf_page_size, prev,
		DM1B_LRANGE, &lrange_ptr);
	lrange_len = log_rec->btf_klen;
	if (log_rec->btf_cmp_type == DM1CX_COMPRESSED)
	{
	    dm1cx_klen(log_rec->btf_pg_type, log_rec->btf_page_size, prev, 
			DM1B_LRANGE, &lrange_len);
	}
	dm1cxtget(log_rec->btf_pg_type, log_rec->btf_page_size, prev, 
		DM1B_LRANGE, &lrange_tid, (i4*)NULL);

	dm1cxrecptr(log_rec->btf_pg_type, log_rec->btf_page_size, prev, 
		DM1B_RRANGE, &rrange_ptr);
	rrange_len = log_rec->btf_klen;
	if (log_rec->btf_cmp_type == DM1CX_COMPRESSED)
	{
	    dm1cx_klen(log_rec->btf_pg_type, log_rec->btf_page_size, prev,
		DM1B_LRANGE, &rrange_len);
	}
	dm1cxtget(log_rec->btf_pg_type, log_rec->btf_page_size, prev, 
		DM1B_RRANGE, &rrange_tid, (i4*)NULL);

	nextpageno = DM1B_VPT_GET_BT_NEXTPAGE_MACRO(log_rec->btf_pg_type, prev);
	nextovfl = DM1B_VPT_GET_PAGE_OVFL_MACRO(log_rec->btf_pg_type, prev);

	/*
	** If the previous page is a leaf, the overflow page will point back to
	** it, otherwise use the previous page's page_main pointer.
	*/
	mainpageno = DM1B_VPT_GET_PAGE_MAIN_MACRO(log_rec->btf_pg_type, prev);
	if (DM1B_VPT_GET_PAGE_STAT_MACRO(log_rec->btf_pg_type, prev) & DMPP_LEAF)
	    mainpageno = DM1B_VPT_GET_PAGE_PAGE_MACRO(log_rec->btf_pg_type, prev);
    }
    else
    {
	prev_pageno = log_rec->btf_prev_pageno;
	prev_loc_id = log_rec->btf_prev_loc_id;
	lrange_len = log_rec->btf_lrange_len;
	lrange_tid = log_rec->btf_lrtid;
	rrange_len = log_rec->btf_rrange_len;
	rrange_tid = log_rec->btf_rrtid;
	nextpageno = log_rec->btf_nextpage;
	nextovfl   = log_rec->btf_nextovfl;
	mainpageno = log_rec->btf_mainpage;

	lrange_ptr = &log_rec->btf_vbuf[log_rec->btf_dupkey_len];
	rrange_ptr = &log_rec->btf_vbuf[log_rec->btf_dupkey_len + lrange_len];
    }

    /*
    ** Mutex the overflow pages while updating them.
    */
    if (ovfl)
	dm0p_mutex(tabio, (i4)0, dmve->dmve_lk_id, (DMPP_PAGE **) &ovfl);
    if (prev)
	dm0p_mutex(tabio, (i4)0, dmve->dmve_lk_id, (DMPP_PAGE **) &prev);

    /*
    ** Check direction of recovery operation:
    **
    **     If this is a normal Undo, then we log the CLR for the operation
    **     and write the LSN of this CLR onto the newly updated page (unless
    **     dmve_logging is turned off - in which case the rollback is not
    **     logged and the page lsn is unchanged).
    **
    **     If the record being processed is itself a CLR, then we are REDOing
    **     an update made during rollback processing.  Updates are not relogged
    **     in redo processing and the LSN is moved forward to the LSN value of
    **     of the original update.
    */
    if ((log_rec->btf_header.flags & DM0L_CLR) == 0)
    {
	if (dmve->dmve_logging)
	{
	    dm0l_flags = (log_rec->btf_header.flags | DM0L_CLR);
	    dupkey_ptr = &log_rec->btf_vbuf[0];

	    status = dm0l_btfree(dmve->dmve_log_id, dm0l_flags, 
		&log_rec->btf_tbl_id, &log_rec->btf_tblname, 
		&log_rec->btf_tblowner,
		log_rec->btf_pg_type, log_rec->btf_page_size,
		log_rec->btf_cmp_type, 
		log_rec->btf_loc_cnt, log_rec->btf_klen,
		log_rec->btf_ovfl_loc_id, prev_loc_id,
		log_rec->btf_ovfl_pageno, prev_pageno,
		mainpageno, nextpageno, nextovfl,
		log_rec->btf_dupkey_len, dupkey_ptr,
		lrange_len, lrange_ptr, &lrange_tid,
		rrange_len, rrange_ptr, &rrange_tid,
		log_lsn, &lsn, log_rec->btf_tidsize, 
		log_rec->btf_ixklen, err_code);
	    if (status != E_DB_OK)
	    {
		if (ovfl) 
		    dm0p_unmutex(tabio, (i4)0, dmve->dmve_lk_id, (DMPP_PAGE **) &ovfl);
		if (prev) 
		    dm0p_unmutex(tabio, (i4)0, dmve->dmve_lk_id, (DMPP_PAGE **) &prev);

		/*
		 * Bug56702: return logfull indication.
		 */
		dmve->dmve_error.err_data = *err_code;
		ule_format(*err_code, (CL_ERR_DESC *)0, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 0); 
		*err_code = E_DM9655_UNDO_BTREE_FREE;
		return(E_DB_ERROR);
	    }
	}
    }
    else
    {
	/*
	** If we are processing recovery of an Update Ovfl CLR (redo-ing the 
	** undo of an allocate) then we don't log a CLR but instead save the 
	** LSN of the log record we are processing with which to update the 
	** page lsn's.
	*/
	lsn = *log_lsn;
    }

    /*
    ** Undo the Deallocate operation.
    ** 
    ** Reformat the old page as a btree leaf overflow page and link it into
    ** the overflow chain by pointing the previous overflow page to it.
    */
    if (prev)
    {
	DM1B_VPT_SET_PAGE_OVFL_MACRO(log_rec->btf_pg_type, prev,
	    log_rec->btf_ovfl_pageno);
	DM1B_VPT_SET_PAGE_STAT_MACRO(log_rec->btf_pg_type, prev, DMPP_MODIFY);

	if (dmve->dmve_logging)
	    DM1B_VPT_SET_PAGE_LOG_ADDR_MACRO(log_rec->btf_pg_type, prev, lsn);
    }

    while (ovfl)
    {
	/* Sanity check tidsize */
	if ( log_rec->btf_tidsize != sizeof(DM_TID) &&
	     log_rec->btf_tidsize != sizeof(DM_TID8) )
	{
	    log_rec->btf_tidsize = sizeof(DM_TID);
	}
	/*
	** Calculate maximum keys that will fit on a leaf/overflow page 
	*/
	kperpage = dm1cxkperpage(log_rec->btf_pg_type, log_rec->btf_page_size,
		0, TCB_BTREE, 0, log_rec->btf_klen, DM1B_PLEAF,
		log_rec->btf_tidsize, 0);

	/*
	** Format overflow leaf page.
	*/
	status = dm1cxformat(log_rec->btf_pg_type, log_rec->btf_page_size, ovfl,
	    loc_plv, log_rec->btf_cmp_type,
	    kperpage, log_rec->btf_klen,
	    (DB_ATTS **)0, (i4)0, (DB_ATTS **)0, (i4)0,
	    log_rec->btf_ovfl_pageno, DM1B_POVFLO, TCB_BTREE,
	    log_rec->btf_tidsize, 0);
	if (status != E_DB_OK)
	{
	    dm1cxlog_error(E_DM93E5_BAD_INDEX_FORMAT, (DMP_RCB *)0, ovfl, 
			   log_rec->btf_pg_type, log_rec->btf_page_size, 0);
	    *err_code = E_DM9655_UNDO_BTREE_FREE;
	    break;
	}

	/*
	** Fill in the range key values.
	*/
	status = dm1cxreplace(log_rec->btf_pg_type, log_rec->btf_page_size, 
	    ovfl, DM1C_DIRECT, log_rec->btf_cmp_type,
	    (DB_TRAN_ID *)0, (i4)0, 
	    DM1B_LRANGE, lrange_ptr, lrange_len, &lrange_tid, (i4)0);
	if (status != E_DB_OK)
	{
	    dm1cxlog_error(E_DM93E6_BAD_INDEX_REPLACE, (DMP_RCB *)0, 
	       ovfl, log_rec->btf_pg_type, log_rec->btf_page_size, DM1B_LRANGE);
	    *err_code = E_DM9655_UNDO_BTREE_FREE;
	    break;
	}

	status = dm1cxreplace(log_rec->btf_pg_type, log_rec->btf_page_size,
			ovfl, DM1C_DIRECT, log_rec->btf_cmp_type,
			(DB_TRAN_ID *)0, (i4)0,
			DM1B_RRANGE, rrange_ptr, rrange_len, 
			&rrange_tid, (i4)0);
	if (status != E_DB_OK)
	{
	    dm1cxlog_error(E_DM93E6_BAD_INDEX_REPLACE, (DMP_RCB *)0, 
	       ovfl, log_rec->btf_pg_type, log_rec->btf_page_size, DM1B_RRANGE);
	    *err_code = E_DM9655_UNDO_BTREE_FREE;
	    break;
	}

	/*
	** Link the new overflow page into the overflow chain.
	*/
	DM1B_VPT_SET_PAGE_MAIN_MACRO(log_rec->btf_pg_type, ovfl, mainpageno);
	DM1B_VPT_SET_PAGE_OVFL_MACRO(log_rec->btf_pg_type, ovfl, nextovfl);
	DM1B_VPT_SET_BT_NEXTPAGE_MACRO(log_rec->btf_pg_type, ovfl, nextpageno);

	DM1B_VPT_SET_PAGE_STAT_MACRO(log_rec->btf_pg_type, ovfl, DMPP_MODIFY);

	if (dmve->dmve_logging)
	    DM1B_VPT_SET_PAGE_LOG_ADDR_MACRO(log_rec->btf_pg_type, ovfl, lsn);

	break;
    }

    if (ovfl)
	dm0p_unmutex(tabio, (i4)0, dmve->dmve_lk_id, (DMPP_PAGE **) &ovfl);
    if (prev)
	dm0p_unmutex(tabio, (i4)0, dmve->dmve_lk_id, (DMPP_PAGE **) &prev);
    
    return(status);
}
