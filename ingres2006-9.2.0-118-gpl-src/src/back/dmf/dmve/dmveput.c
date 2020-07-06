/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <me.h>
#include    <pc.h>
#include    <st.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <lg.h>
#include    <lk.h>
#include    <adf.h>
#include    <ulf.h>
#include    <dmf.h>
#include    <dm.h>
#include    <dmp.h>
#include    <dmtcb.h>
#include    <dm2t.h>
#include    <dm2r.h>
#include    <dm2f.h>
#include    <dmftrace.h>
#include    <dm1c.h>
#include    <dm1r.h>
#include    <dm1h.h>
#include    <dmve.h>
#include    <dmpp.h>
#include    <dm1b.h>
#include    <dm0l.h>
#include    <dm0p.h>

/**
**
**  Name: DMVEPUT.C - The recovery of a put record operation.
**
**  Description:
**	This file contains the routine for the recovery of a 
**	put record operation.
**
**          dmve_put - The recovery of a put record operation.
**
**
**  History:
**	26-oct-1992 (jnash & rogerk)
**	    Rewritten for 6.5 recovery.
**	11-jan-1992 (jnash)
**	    Don't include row in log record if CLR.
**	26-jan-1993 (jnash)
**	    Fix bug introduced by last change -- preserve record size 
**	    calculation.  
**	15-mar-1993 (jnash & rogerk)
**	    Reduced Logging - Phase IV:
**	    Changed log record format: added database_id, tran_id and LSN
**		in the log record header.  The LSN of a log record is now
**		obtained from the record itself rather than from the dmve
**		control block.
**	    Check dmve->dmve_logging to determine if logging required.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**		Correct the message parameter lengths for E_DM9665.
**	24-may-1993 (bryanp)
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**	21-jun-1993 (rogerk)
**	    Add error messages.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	26-jul-1993 (bryanp)
**	    Replace all uses of DM_LKID with LK_LKID.
**	26-jul-1993 (rogerk)
**	    Added tracing of page information during recovery.
**	    Include respective CL headers for all cl calls.
**      18-oct-1993 (jnash)
**          Add page dump on certain errors.
**	18-oct-1993 (rogerk)
**	    Use dmpp_reallocate instead of dmpp_allocate for checking
**	    available space on page.
**	15-apr-1994 (chiku)
**	    Bug56702: return logfull indication.
**      20-may-95 (hanch04)
**          Added include dmtcb.h for ndmfapi
**	06-mar-1996 (stial01 for bryanp)
**	    Pass tbio_page_size as page_size argument to dmpp_allocate.
**	    Pass tbio_page_size as the page_size argument to dmpp_reclen.
**	06-mar-1996 (stial01 for bryanp)
**	    Remove DB_MAXTUP dependencies in favor of DM_MAXTUP, the internal
**	    DMF maximum tuple size.
**      06-mar-1996 (stial01)
**          Pass put_page_size to dmve_trace_page_header,dmve_trace_page_info
**	06-may-1996 (thaju02)
**	    New page format support: change page header references to macros.
**      20-may-1996 (ramra01)
**          Added DMPP_TUPLE_INFO to get/put accessor routine
**      03-june-1996 (stial01)
**          Use DMPP_INIT_TUPLE_INFO_MACRO to init DMPP_TUPLE_INFO
**	19-Nov-1996 (hanch04)
**	    set row_version for dmv_reput.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Delete accessor: added reclaim_space 
**          Put accessor: changed DMPP_TUPLE_INFO param to table_version
**          Do not get page lock if row locking.
**          Add lock_type argument to dm0p_lock_page and dm0p_unlock_page
**      10-mar-97 (stial01)
**          dmv_put() If IX page lock held, mutex page before looking at it
**          dmv_put: Clean ALL deleted tuples if X page lock.
**          dmv_unput() Space is not reclaimed if IX page lock held
**          Added dmve_clean routine
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          Add lock_id argument to dm0p_lock_page and dm0p_unlock_page.
**      21-may-1997 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
**      29-may-1997 (stial01)
**          Use dm0p_lock_buf_macro to lock buffer to reduce duration dmve  
**          holds buffer mutex.
**      18-jun-1997 (stial01)
**          dmve_put() Request IX page lock if row locking.
**      30-jun-97 (dilma04)
**          Bug 83301. Set DM0P_INTENT_LK fix action if row locking.
**	25-Aug-1997 (jenjo02)
**	    Added log_id parm to dm2t_ufx_tabio_cb() calls.
**      28-may-1998 (stial01)
**          Support VPS system catalogs, changed parameters for dmve_clean.
**	23-Jun-1998 (jenjo02)
**	    Added lock_value parm to dm0p_lock_page() and dm0p_unlock_page().
**	    Utilize lock_id value in both of those functions.
**	    Consolidate redundant page unfix/unlock code.
**      07-jul-1998 (stial01)
**          New update_mode if row locking, changed paramater to dmve_clean
**	14-aug-1998 (nanpr01)
**	    Error code is getting reset to E_DB_OK on error and not making
**	    the database inconsistent.
**	01-dec-1998 (nanpr01)
**	    Get rid of DMPP_TUPLE_INFO structure. Use DMPP_TUPLE_HDR instead.
**      08-dec-1999 (stial01)
**          Undo change for bug#85156, dmpe routines now get INTENT table locks.
**      21-dec-1999 (stial01)
**          Page cleans only before put.
**	04-May-2000 (jenjo02)
**	    Pass blank-suppressed lengths of table/owner names to 
**	    dm0l_put, eliminating need for dmve_name_unpack().
**      05-jun-2000 (stial01)
**          Set DM1C_ROWLK in update flags when physical page locking (b101708)
**      25-may-2000 (stial01)
**          Changed criteria for setting reclaim_space
**      23-oct-2000 (stial01)
**          Variable Page Size - Variable Page Type support (SIR 102955)
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      01-feb-2001 (stial01)
**          Arg changes for page accessors (Variable Page Type SIR 102955)
**          Pass pg type to dmve_trace_page_info
**      01-may-2001 (stial01)
**          Fixed parameters to ule_format (B104618)
**	05-Mar-2002 (jenjo02)
**	    Add IISEQUENCE table to the list of un-logged rows
**	    to only prefix-check.
**      19-Jun-2002 (horda03) Bug 108074
**          Prevent FORCE_ABORT transaction filling TX log
**          file by not flushing SCONCUR pages unless we
**          really need to (i.e. the table isn't locked
**          exclusively).
**      10-feb-2004 (stial01)
**          Support for 256K rows, rows spanning pages.
**      31-jan-05 (stial01)
**          Added flags arg to dm0p_mutex call(s).
**      21-feb-2005 (stial01)
**          Cross integrate 470401 (inifa01) for INGSRV2569 b111886
**          New flag DM1C_DMVE_COMP passed to dmpp_delete data is compressed
**      09-may-2005 (horda03) Bug 114471/INGSRV3294
**          Temporary solution to allow ROLLFORWARDDB of journals that 
**          have log records with the DM0L_TEMP_IIRELATION flag set
**          incorrectly. THis flag should only be set for iirtemp
**	06-Mar-2007 (jonj)
**	    Replace dm0p_cachefix_page() with dmve_cachefix_page()
**	    for Cluster REDO recovery.
*/

static DB_STATUS	dmv_reput(
				DMVE_CB             *dmve,
				DMP_TABLE_IO	    *tabio,
				DMPP_PAGE	    *page,
				DM0L_PUT	    *log_rec,
				DMPP_ACC_PLV 	    *plv,
				i4             lock_type,
				i4             lock_action,
				i4             *err_code);

static DB_STATUS	dmv_unput(
				DMVE_CB             *dmve,
				DMP_TABLE_IO	    *tabio,
				DMPP_PAGE	    *page,
				DM0L_PUT	    *log_rec,
				DMPP_ACC_PLV 	    *plv,
				i4             lock_type,
				i4             lock_action,
				i4             *err_code);


/*{
** Name: dmve_put - The recovery of a put record operation.
**
** Description:
**	This function is used to do, redo and undo a put
**	record operation of a transaction. This function adds the 
**	record to a table or deletes the record from a table.
**
**	As of 6.5, this modules is used for both system catalog  
**	and other page types.
**
** Inputs:
**	dmve_cb
**	    .dmve_log_rec	    The log record of the system catalogs put 
**				    operation.
**	    .dmve_action	    Should be DMVE_DO, DMVE_REDO, or DMVE_UNDO.
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
**	    E_DB_FATAL			Operation was partially completed,
**					the transaction must be aborted.
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	26-oct-1992 (jnash)
**	    Rewritten for 6.5 recovery.  Now tables may be "partially 
**	    opened" by this routine, in accordance with new dm2t and buffer 
**	    manager protocols.  
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**		Correct the message parameter lengths for E_DM9665.
**	26-jul-1993 (bryanp)
**	    Replace all uses of DM_LKID with LK_LKID.
**	06-mar-1996 (stial01 for bryanp)
**	    Remove DB_MAXTUP dependencies in favor of DM_MAXTUP, the internal
**	    DMF maximum tuple size.
**	06-may-1996 (thaju02)
**	    New Page Format Support: 
**		Change page header references to macros.
**		Pass page size to dm1c_getaccessors().
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Do not get page lock if row locking.
**          Add lock_type argument to dm0p_lock_page and dm0p_unlock_page
**      10-mar-97 (stial01)
**          dmv_put() If IX page lock held, mutex page before looking at it
**          dmv_put: Clean ALL deleted tuples if X page lock.
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          Add lock_id argument to dm0p_lock_page and dm0p_unlock_page.
**      21-may-1997 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
**      18-jun-1997 (stial01)
**          dmve_put() Request IX page lock if row locking.
**      30-jun-97 (dilma04)
**          Bug 83301. Set DM0P_INTENT_LK fix action if row locking.
**      11-sep-1997 (thaju02) bug#85156 - on recovery lock extension
**          table with DM2T_S.  Attempting to lock table with DM2T_IX
**          causes deadlock, dbms resource deadlock error during recovery,
**          pass abort, rcp resource deadlock error during recovery,
**          rcp marks database inconsistent.
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
dmve_put(
DMVE_CB		*dmve)
{
    DM0L_PUT		*log_rec = (DM0L_PUT *)dmve->dmve_log_rec;
    LG_LSN		*log_lsn = &log_rec->put_header.lsn; 
    DMP_DCB		*dcb = dmve->dmve_dcb_ptr;
    DMP_TABLE_IO	*tbio = NULL;
    DMPP_PAGE		*page = NULL;
    DMPP_PAGE		*page_ptr = NULL;
    DB_STATUS		status = E_DB_OK;
    DB_STATUS		tmp_status;
    DMPP_ACC_PLV	*loc_plv;
    DMPP_ACC_TLV	*loc_tlv;
    LK_LKID		lockid;
    LK_LKID		page_lockid;
    i4		lock_action = LK_LOGICAL;
    i4		grant_mode;
    i4		recovery_action;
    i4		error;
    i4		loc_error;
    bool		physical_page_lock = FALSE;
    i4             lock_type = LK_TABLE;
    bool                buf_locked = FALSE;
    i4             fix_action = 0;

    MEfill(sizeof(LK_LKID), 0, &lockid);
    MEfill(sizeof(LK_LKID), 0, &page_lockid);

    for (;;)
    {
	/*
	** Consistency Check:  check for illegal log records.
	*/
	if ((log_rec->put_header.type != DM0LPUT) ||
	    (log_rec->put_header.length != 
		(sizeof(DM0L_PUT) + log_rec->put_rec_size -
			(DB_MAXNAME - log_rec->put_tab_size) -
			(DB_MAXNAME - log_rec->put_own_size))))
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
	status = dmve_fix_tabio(dmve, &log_rec->put_tbl_id, &tbio, &error);
	if (DB_FAILURE_MACRO(status))
	    break;
	if ((status == E_DB_WARN) && (error == W_DM9660_DMVE_TABLE_OFFLINE))
	    return (E_DB_OK);

	/*
	** Get page accessors for page recovery actions.
	*/
	dm1c_getaccessors(TCB_C_DEFAULT, log_rec->put_pg_type, 
		log_rec->put_page_size, &loc_plv, &loc_tlv);

	/*
	** Get required Table/Page locks before we can write the page.
	**
	** Some Ingres pages are locked only temporarily while they are
	** updated and then released immediately after the update.  The
	** instances of such page types that are recovered through this 
	** routine are system catalog pages.
	**
	** Except for these system catalog pages, we expect that any page
	** which requires recovery here has already been locked by the
	** original transaction and that the following lock requests will
	** not block.
	**
	** Note that if the database is locked exclusively, or if an X table
	** lock is granted then no page lock is requried.
	*/
	if ((dcb->dcb_status & DCB_S_EXCLUSIVE) == 0)
	{
	    /*
	    ** Request IX lock in preparation of requesting an X page lock
	    ** below.  If the transaction already holds an exclusive table
	    ** lock, then an X lock will be granted.  In this case we can
	    ** bypass the page lock request.
	    */
	    status = dm2t_lock_table(dcb, &log_rec->put_tbl_id, DM2T_IX,
		dmve->dmve_lk_id, (i4)0, &grant_mode, &lockid, &error);
	    if (status != E_DB_OK)
		break;

	    if (grant_mode != DM2T_X)
	    {
		/*
		** Page lock required.  If this is a system catalog page
		** (indicated by the logged page type) then we need to
		** request a physical lock (and release it later).
		*/
		if (log_rec->put_header.flags & DM0L_PHYS_LOCK)
		    lock_action = LK_PHYSICAL;
 
                if (log_rec->put_header.flags & DM0L_ROW_LOCK)
		    lock_type = LK_ROW;
		else
		    lock_type = LK_PAGE;

		status = dm0p_lock_page(dmve->dmve_lk_id, dcb, 
		    &log_rec->put_tbl_id, 
		    log_rec->put_tid.tid_tid.tid_page, LK_PAGE, 
		    lock_type == LK_PAGE ? LK_X : LK_IX, 
		    lock_action, (i4)0, tbio->tbio_relid, 
		    &dmve->dmve_tran_id, &page_lockid, 
		    (i4 *)0, (LK_VALUE *)0, &error);
		if (status != E_DB_OK)
		    break;
		if (lock_action == LK_PHYSICAL)
		    physical_page_lock = TRUE;
	    }
	    else
            {
                fix_action |= DM0P_TABLE_LOCKED_X;
            }
	}

	/*
	** Currently during REDO recovery we must have exclusive control
	** of the page being recovered. This allows us to clean all
	** deleted tuples from the page regardless of transaction id.
	** Since page cleans are not logged, this simplifies REDO recovery.
	** 
	** rollforwarddb DMVE_DO:
	**     We should have X table or database lock.
	** 
	** Offline recovery by RCP, DMVE_REDO:
	**     We should have X database lock.
	**
	** Online recovery by RCP, DMVE_REDO:
	**     Fast commit db -> full database REDO, X database lock
	**
	**     Non-fast commit db -> redo of transactions being recovered.
	**         We disallow row locking when multiple servers are using
	**         different data caches. (And online recovery will not get
	**         exclusive access to database).
	*/
	if (lock_type == LK_ROW && 
	    (dmve->dmve_action == DMVE_REDO || dmve->dmve_action == DMVE_DO))
	{
	    error = E_DM93F5_ROW_LOCK_PROTOCOL;
	    status = E_DB_ERROR;
	    break;
	}

	/*
	** Fix the page we need to recover in cache for write.
	*/
        if (lock_type == LK_ROW)
            fix_action |= DM0P_INTENT_LK;
	status = dmve_cachefix_page(dmve, log_lsn,
				    tbio, log_rec->put_tid.tid_tid.tid_page,
				    fix_action, loc_plv,
				    &page_ptr, &error);
	if (status != E_DB_OK)
	    break;
	page = page_ptr;

        if (lock_type == LK_ROW)
	{
	    dm0p_lock_buf_macro(tbio, dmve->dmve_lk_id, &page);
	    buf_locked = TRUE;
	}

	/*
	** Dump debug trace info about pages if such tracing is configured.
	*/
	if (DMZ_ASY_MACRO(15))
	    dmve_trace_page_info(log_rec->put_pg_type, log_rec->put_page_size, 
		page, loc_plv, "DATA");

	/*
	** Compare the LSN on the page with that of the log record
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
            if (LSN_GTE(
		DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(log_rec->put_pg_type, page), 
		log_lsn) && ((dmve->dmve_flags & DMVE_ROLLDB_BOPT) == 0))
	    {
		if (dmve->dmve_action == DMVE_DO)
		{
		    ule_format(E_DM9665_PAGE_OUT_OF_DATE, (CL_ERR_DESC *)0, 
			ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, 
			&loc_error, 8, 
			sizeof(*tbio->tbio_relid), tbio->tbio_relid,
			sizeof(*tbio->tbio_relowner), tbio->tbio_relowner,
			0, DMPP_VPT_GET_PAGE_PAGE_MACRO(log_rec->put_pg_type, 
			    page), 
			0, DMPP_VPT_GET_PAGE_STAT_MACRO(log_rec->put_pg_type,
			    page),
			0, DMPP_VPT_GET_LOG_ADDR_HIGH_MACRO(log_rec->put_pg_type,
			    page), 
			0, DMPP_VPT_GET_LOG_ADDR_LOW_MACRO(log_rec->put_pg_type,
			    page),
			0, log_lsn->lsn_high, 0, log_lsn->lsn_low);
		}
		page = NULL;
	    }
	    break;

	case DMVE_UNDO:
            if (LSN_LT(
		DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(log_rec->put_pg_type, page), 
		log_lsn))
	    {
		ule_format(E_DM9665_PAGE_OUT_OF_DATE, (CL_ERR_DESC *)0, 
		    ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, 
		    &loc_error, 8, 
		    sizeof(*tbio->tbio_relid), tbio->tbio_relid,
		    sizeof(*tbio->tbio_relowner), tbio->tbio_relowner,
		    0, DMPP_VPT_GET_PAGE_PAGE_MACRO(log_rec->put_pg_type, page), 
		    0, DMPP_VPT_GET_PAGE_STAT_MACRO(log_rec->put_pg_type, page),
		    0, DMPP_VPT_GET_LOG_ADDR_HIGH_MACRO(log_rec->put_pg_type, 
			page), 
		    0, DMPP_VPT_GET_LOG_ADDR_LOW_MACRO(log_rec->put_pg_type, 
			page),
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
	if (log_rec->put_header.flags & DM0L_CLR)
	    recovery_action = DMVE_UNDO;

	switch (recovery_action)
	{
	case DMVE_DO:
	case DMVE_REDO:
	    status = dmv_reput(dmve, tbio, page, log_rec, loc_plv, 
			lock_type, lock_action, &error);
	    break;

	case DMVE_UNDO:
	    status = dmv_unput(dmve, tbio, page, log_rec, loc_plv, 
			lock_type, lock_action, &error); 
	    break;
	}

	break;
    }

    if (status != E_DB_OK)
    {
	ule_format(error, (CL_ERR_DESC *)0, ULE_LOG, NULL, 
	    (char *)0, (i4)0, (i4 *)0, &error, 0);
    }

    if (buf_locked == TRUE)
	dm0p_unlock_buf_macro(tbio, dmve->dmve_lk_id, &page);

    /*
    ** Unfix the updated page.  No need to force it to disk - it
    ** will be tossed out through normal cache protocols if Fast
    ** Commit or at the end of the abort if non Fast Commit.
    */
    if (page_ptr)
    {
	tmp_status = dm0p_uncache_fix(tbio, DM0P_UNFIX, dmve->dmve_lk_id, 
		dmve->dmve_log_id, &dmve->dmve_tran_id, &page_ptr, &loc_error);
	if (tmp_status != E_DB_OK)
	{
	    ule_format(loc_error, (CL_ERR_DESC *)0, ULE_LOG, NULL, 
		(char *)0, (i4)0, (i4 *)0, &loc_error, 0);
	    if (tmp_status > status)
	        status = tmp_status;
	}
    }

    /*
    ** If a short term physical lock was acquired on the page
    ** then release it.
    */
    if (physical_page_lock)
    {
	tmp_status = dm0p_unlock_page(dmve->dmve_lk_id, dcb, 
	    &log_rec->put_tbl_id, log_rec->put_tid.tid_tid.tid_page, 
	    LK_PAGE, tbio->tbio_relid, &dmve->dmve_tran_id, &page_lockid, 
            (LK_VALUE *)0, &loc_error);
	if (tmp_status != E_DB_OK)
	{
	    ule_format(loc_error, (CL_ERR_DESC *)0, ULE_LOG, NULL, 
		(char *)0, (i4)0, (i4 *)0, &loc_error, 0);
	    if (tmp_status > status)
	        status = tmp_status;
	}
    }

    /* 
    ** Release our handle to the table control block.
    */
    if (tbio)
    {
	tmp_status = dm2t_ufx_tabio_cb(tbio, DM2T_VERIFY, 
	    dmve->dmve_lk_id, dmve->dmve_log_id, &loc_error);
	if (tmp_status != E_DB_OK)
	{
	    ule_format(loc_error, (CL_ERR_DESC *)0, ULE_LOG, NULL, 
		(char *)0, (i4)0, (i4 *)0, &loc_error, 0);
	    if (tmp_status > status)
	        status = tmp_status;
	}
    }

    if (status != E_DB_OK)
	dmve->dmve_error.err_code = E_DM960D_DMVE_PUT;

    return(status);
}


/*{
** Name: dmv_reput - Redo the Put of a record 
**
** Description:
**      This function adds a record to a table for the recovery of a
**	put record operation.
**
** Inputs:
**      dmve				Pointer to dmve control block.
**      tabio				Pointer to table io control block
**      page				Pointer to the page to which to insert
**	log_record			Pointer to the log record
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
**	06-oct-1992 (jnash)
**	    Rewritten for 6.5 recovery project.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**	18-oct-1993 (rogerk)
**	    Use dmpp_reallocate instead of dmpp_allocate for checking
**	    available space on page.
**	06-mar-1996 (stial01 for bryanp)
**	    Pass tbio_page_size as page_size argument to dmpp_allocate.
**	06-may-1996 (thaju02 & nanpr01)
**	    New page format support: change page header references to macros.
**      20-may-1996 (ramra01)
**          Added DMPP_TUPLE_INFO to get/put accessor routine
**      03-june-1996 (stial01)
**          Use DMPP_INIT_TUPLE_INFO_MACRO to init DMPP_TUPLE_INFO
**      22-jul-1996 (ramra01 for bryanp)
**          Pass row_version argument to dmpp_get and dmpp_put.
**	19-Nov-1996 (hanch04)
**	    set row_version.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Put accessor: changed DMPP_TUPLE_INFO param to table_version
**      21-may-1997 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
**      30-Mar-2004 (hanal04) Bug 107828 INGSRV2701
**          A sysmod will cause a new iirelation to be created using
**          a journalled iirelation entry even though the table it represents
**          may be non-journalled. If the PUT into iirtemp has been flagged 
**          as part of this type of operation we must still perform the PUT
**          or the user table will no longer be present in iirelation. 
**          However, the last journalled values must be applied. This change 
**          assumes that rollforward processing has kept iirelation entries 
**          consistent with the tables that they represent. See rogerk's 
**          changes for REPLACE operations (405077). That being the case we 
**          can mask the current iirelation record into the PUT log record
**          in order to maintain consistency between iirelation entries that
**          represent non-journalled user tables and the physical table.
*/
static DB_STATUS
dmv_reput(
DMVE_CB             *dmve,
DMP_TABLE_IO	    *tabio,
DMPP_PAGE	    *page,
DM0L_PUT	    *log_rec,
DMPP_ACC_PLV 	    *plv,
i4             lock_type,
i4             lock_action,
i4             *err_code)
{
    DMP_DCB		*dcb = dmve->dmve_dcb_ptr;
    LG_LSN		*page_lsn = 
    DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(log_rec->put_pg_type, page);
    LG_LSN		*log_lsn = &log_rec->put_header.lsn; 
    DB_STATUS		status = E_DB_OK;
    DM_TID		loc_tid;
    i4		record_size;
    char		*record;
    char		*log_row;
    i4                  update_mode;
 
    /*
    ** If there is nothing to recover, just return.
    */
    if (page == NULL)
	return (E_DB_OK);

    log_row = &log_rec->put_vbuf[log_rec->put_tab_size + log_rec->put_own_size];

    /*
    ** Redo recovery must follow row locking protocols if phys locking
    ** NOTE we can't be row locking during redo recovery
    */
    if (log_rec->put_header.flags & DM0L_PHYS_LOCK)
	update_mode = (DM1C_DIRECT | DM1C_ROWLK);
    else
	update_mode = DM1C_DIRECT;

    /*
    ** Consistency Checks:
    ** 
    ** Make sure that there is not already a tuple at the spot at which 
    ** we are about to write our record.
    **
    ** Also guarantee that sufficient space exists on the page.
    ** The "repeating history" rules of this recorvery should guarantee
    ** that the row will fit since it must have fit originally.  If there
    ** is no room, this is a fatal recovery error.
    */
    if (log_rec->put_tid.tid_tid.tid_line < 
	(u_i2) DMPP_VPT_GET_PAGE_NEXT_LINE_MACRO(log_rec->put_pg_type, page))
    {
	status = (*plv->dmpp_get)(log_rec->put_pg_type, log_rec->put_page_size,
	    page, &log_rec->put_tid, &record_size, &record, NULL, NULL, NULL);
	if (status != E_DB_WARN)
	{
	    ule_format(E_DM966E_DMVE_ROW_OVERLAP, (CL_ERR_DESC *)0, ULE_LOG,
		NULL, (char *)0, (i4)0, (i4 *)0, err_code, 5, 
		sizeof(DB_DB_NAME), tabio->tbio_dbname->db_db_name,
		log_rec->put_tab_size, &log_rec->put_vbuf[0],
		log_rec->put_own_size, &log_rec->put_vbuf[log_rec->put_tab_size],
		0, log_rec->put_tid.tid_tid.tid_page,
		0, log_rec->put_tid.tid_tid.tid_line);
	    dmd_log(1, log_rec, 4096);
	    dmve_trace_page_header(log_rec->put_pg_type, log_rec->put_page_size,
		page, plv, "DATA");
	    dmve_trace_page_contents(log_rec->put_pg_type, 
		log_rec->put_page_size, page, "DATA");
	    *err_code = E_DM963B_REDO_PUT;
	    return(E_DB_ERROR);
	}
    }

    if (log_rec->put_pg_type != TCB_PG_V1)
    {
	/* 
	** Check for sufficient space on the page to insert the row.
	** During REDO, we may need to clean deleted records
	*/
	status = (*plv->dmpp_reallocate)(log_rec->put_pg_type, 
	    log_rec->put_page_size, page,
	    log_rec->put_tid.tid_tid.tid_line, log_rec->put_rec_size);
	if (status != E_DB_OK)
	{
	    status = dmve_clean(page, plv, tabio, dmve->dmve_lk_id, 
		    lock_type, lock_action, err_code);
	    status = (*plv->dmpp_reallocate)(log_rec->put_pg_type, 
		log_rec->put_page_size, page,
		log_rec->put_tid.tid_tid.tid_line, log_rec->put_rec_size);
	}
	if (status != E_DB_OK)
	{
	    ule_format(E_DM9674_DMVE_PAGE_NOROOM, (CL_ERR_DESC *)0, ULE_LOG,
		NULL, (char *)0, (i4)0, (i4 *)0, err_code, 6, 
		sizeof(DB_DB_NAME), tabio->tbio_dbname->db_db_name,
		log_rec->put_tab_size, &log_rec->put_vbuf[0],
		log_rec->put_own_size, &log_rec->put_vbuf[log_rec->put_tab_size],
		0, log_rec->put_tid.tid_tid.tid_page,
		0, log_rec->put_tid.tid_tid.tid_line, 
		0, log_rec->put_rec_size);
	    dmd_log(1, log_rec, 4096);
	    dmve_trace_page_header(log_rec->put_pg_type, log_rec->put_page_size,
		page, plv, "DATA");
	    dmve_trace_page_contents(log_rec->put_pg_type, 
		log_rec->put_page_size, page, "DATA");
	    *err_code = E_DM963B_REDO_PUT;
	    return(E_DB_ERROR);
	}
    }

    if ((dmve->dmve_action == DMVE_DO) &&
	(log_rec->put_header.flags & DM0L_NONJNL_TAB) &&
        (log_rec->put_header.flags & DM0L_TEMP_IIRELATION) &&
        /* TEMPORARY FIX for bogus DM0L_TEMP_IIRELATION due to the
        ** fix for Bug 107828.
        */
        (log_rec->put_tab_size == 7) &&
        (STbcompare( log_rec->put_vbuf, 7, "iirtemp", 7, TRUE) == 0))
    {
	/* The row is a temporary iirelation entry which represents a
	** non-journaled table. For consistency purposes we must mask
	** in the current iirelation values.
	*/

	DMP_RELATION   *log_rel = (DMP_RELATION *)log_row;
	DMP_RELATION   cur_rel;
	DB_STATUS      rel_status;
        DB_TAB_ID      rel_id;

	MEcopy((PTR) &log_rel->reltid, sizeof(DB_TAB_ID), (PTR)&rel_id);

	/* We'll get a warning if we had a non-journalled CREATE */
        /* Don't treat that as an error                          */
        rel_status = dmve_get_iirel_row(dmve, &rel_id, &cur_rel); 

        if(DB_FAILURE_MACRO(rel_status))
	{
            *err_code = E_DM963B_REDO_PUT;
	    return(E_DB_ERROR);
	}
	if(rel_status == E_DB_OK)
	{
	    MEcopy((PTR) &cur_rel, sizeof(DMP_RELATION), (PTR)log_rel);
	}
    }

    /*
    ** Mutex the page while updating it.
    */
    dm0p_mutex(tabio, (i4)0, dmve->dmve_lk_id, &page);

    /*
    ** Redo the insert operation.
    */
    (*plv->dmpp_put)(log_rec->put_pg_type, log_rec->put_page_size, page,
	update_mode, &dmve->dmve_tran_id, &log_rec->put_tid,
	log_rec->put_rec_size, log_row, (i4)0, log_rec->put_row_version,
	&log_rec->put_seg_hdr);

    /*
    ** Write the LSN of the Put log record to the updated page.
    */
    DMPP_VPT_SET_PAGE_LOG_ADDR_MACRO(log_rec->put_pg_type, page, *log_lsn);
    DMPP_VPT_SET_PAGE_STAT_MACRO(log_rec->put_pg_type, page, DMPP_MODIFY);

    dm0p_unmutex(tabio, (i4)0, dmve->dmve_lk_id, &page);
    
    return(E_DB_OK);
}

/*{
** Name: dmv_unput - UNDO of a put operation.
**
** Description:
**
** Inputs:
**      dmve				Pointer to dmve control block.
**      tabio				Pointer to table io control block
**      page				Pointer to page on which row was insert
**	log_record			Pointer to the log record
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
**	06-oct-1992 (jnash & rogerk)
**	    Written for 6.5 recovery project.
**	11-jan-1993 (jnash)
**	    Don't include row in log record if CLR.
**	26-jan-1993 (jnash)
**	    Fix bug introduced by last change -- we do need the record size.  
**	15-mar-1993 (jnash)
**	    Check dmve->dmve_logging to determine if logging required.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**      18-oct-1993 (jnash)
**          Add page dump if error.
**	15-apr-1994 (chiku)
**	    Bug56702: return logfull indication.
**	06-mar-1996 (stial01 for bryanp)
**	    Pass tbio_page_size as the page_size argument to dmpp_reclen.
**	06-may-1996 (thaju02 & nanpr01)
**	    New page format support: change page header references to macros.
**	    Pass to the routine correct record size.
**      20-may-1996 (ramra01)
**          Added DMPP_TUPLE_INFO to get/put accessor routine
**      03-june-1996 (stial01)
**          Use DMPP_INIT_TUPLE_INFO_MACRO to init DMPP_TUPLE_INFO
**      22-jul-1996 (ramra01 for bryanp)
**          Pass row_version argument to dmpp_get.
**          Pass row_version argument to dm0l_put.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Delete accessor: added reclaim_space
**      10-mar-97 (stial01)
**          dmv_unput() Space is not reclaimed if IX page lock held
**      21-may-1997 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
*/
static DB_STATUS
dmv_unput(
DMVE_CB             *dmve,
DMP_TABLE_IO	    *tabio,
DMPP_PAGE	    *page,
DM0L_PUT	    *log_rec,
DMPP_ACC_PLV 	    *plv,
i4              lock_type,
i4              lock_action,
i4             *err_code)
{
    LG_LSN		*page_lsn = 
	DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(log_rec->put_pg_type, page);
    LG_LSN		*log_lsn = &log_rec->put_header.lsn; 
    LG_LSN		lsn;
    DB_STATUS		status = E_DB_OK;
    char		*log_row;
    char		*record = NULL;
    i4		record_size;
    i4		compare_size;
    i4		flags;
    i4             reclaim_space;
    i4             update_mode;
 
    /*
    ** If there is nothing to recover, just return.
    */
    if (page == NULL)
    {
	return (E_DB_OK);
    }

    /*
    ** If large pages, the undo-put (delete) should not reclaim space
    ** If row locking or physical page locking, don't shift the data on the page
    ** If uncompressed data we never need to shift the data on the page
    */ 
    if (tabio->tbio_page_type != TCB_PG_V1 ||
	log_rec->put_comptype == TCB_C_NONE ||
	(log_rec->put_header.flags & DM0L_PHYS_LOCK))
	reclaim_space = FALSE;
    else
	reclaim_space = TRUE;

    log_row = &log_rec->put_vbuf[log_rec->put_tab_size + log_rec->put_own_size];


    /*
    ** Make sure there is a record at this tid.
    ** Note that this get call assumes that there will be no uncompression
    ** done and that the get call will always return a pointer to the
    ** row rather than copying the row.
    */
    status = (*plv->dmpp_get)(log_rec->put_pg_type, log_rec->put_page_size,
	page, &log_rec->put_tid, &record_size, &record, NULL, NULL, NULL);
    if ((status != E_DB_OK) || (record == NULL))
    {
	ule_format(E_DM966D_DMVE_ROW_MISSING, (CL_ERR_DESC *)0, ULE_LOG,
	    NULL, (char *)0, (i4)0, (i4 *)0, err_code, 6, 
	    sizeof(DB_DB_NAME), tabio->tbio_dbname->db_db_name,
	    log_rec->put_tab_size, &log_rec->put_vbuf[0],
	    log_rec->put_own_size, &log_rec->put_vbuf[log_rec->put_tab_size],
	    0, log_rec->put_tid.tid_tid.tid_page,
	    0, log_rec->put_tid.tid_tid.tid_line,
	    0, status);
	dmd_log(1, log_rec, 4096);
	dmve_trace_page_header(log_rec->put_pg_type, log_rec->put_page_size, 
	    page, plv, "DATA");
	dmve_trace_page_contents(log_rec->put_pg_type, log_rec->put_page_size,
	    page, "DATA");
	*err_code = E_DM963A_UNDO_PUT;
	return(E_DB_ERROR);
    }

    /*
    ** Compare the row we are about to delete with the one we logged to
    ** make sure they are identical.  If they are not identical then we make
    ** an assumption here that the mismatch is more likely due to this check
    ** being wrong (we have garbage at the end of the tuple buffer or we
    ** allowed some sort of non-logged update to the row) and we continue with
    ** the operation after logging the unexpected condition.
    **
    ** The key size and value can only be verified if the log record included
    ** a copy of the key.  (CLR records do not).
    **
    ** If the table effected is IIRELATION we only compare a prefix of the
    ** row since iirelation is periodically updated without logging the
    ** changes. IISEQUENCE tuples are also updated without logging, so
    ** treat them the same way.
    */
    (*plv->dmpp_reclen)(log_rec->put_pg_type, log_rec->put_page_size, page,
		(i4)log_rec->put_tid.tid_tid.tid_line, &record_size);

    /*
    ** Skip the check if this is the REDO of an UNDO, identified by a
    ** zero length row.
    */
    if (log_rec->put_rec_size)
    {
	if (log_rec->put_tbl_id.db_tab_base == DM_1_RELATION_KEY ||
	    log_rec->put_tbl_id.db_tab_base == DM_B_SEQ_TAB_ID)
	    compare_size = DB_MAXNAME + 16;
	else
	    compare_size = record_size;

	if ((log_rec->put_rec_size != record_size) ||
	    (MEcmp((PTR)log_row, (PTR)record, compare_size) != 0))
	{
	    ule_format(E_DM966C_DMVE_TUPLE_MISMATCH, (CL_ERR_DESC *)0, ULE_LOG,
		NULL, (char *)0, (i4)0, (i4 *)0, err_code, 8, 
		sizeof(DB_DB_NAME), tabio->tbio_dbname->db_db_name,
		log_rec->put_tab_size, &log_rec->put_vbuf[0],
		log_rec->put_own_size, &log_rec->put_vbuf[log_rec->put_tab_size],
		0, log_rec->put_tid.tid_tid.tid_page,
		0, log_rec->put_tid.tid_tid.tid_line,
		0, record_size, 0, log_rec->put_rec_size,
		0, dmve->dmve_action);
	    dmd_log(1, log_rec, 4096);
	    dmve_trace_page_header(log_rec->put_pg_type, log_rec->put_page_size,
		page, plv, "DATA");
	    dmve_trace_page_contents(log_rec->put_pg_type, 
		log_rec->put_page_size, page, "DATA");
	    ule_format(E_DM963A_UNDO_PUT, (CL_ERR_DESC *)0, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0); 
	}
    }

    /* 
    ** Mutex the page.  This must be done prior to the log write.
    */
    dm0p_mutex(tabio, (i4)0, dmve->dmve_lk_id, &page);

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
    if ((log_rec->put_header.flags & DM0L_CLR) == 0)
    {
	if (dmve->dmve_logging)
	{
	    flags = (log_rec->put_header.flags | DM0L_CLR);

	    /* 
	    ** The CLR for a PUT need not contain the entire row, just the tid.
	    */
	    status = dm0l_put(dmve->dmve_log_id, flags, &log_rec->put_tbl_id, 
		(DB_TAB_NAME*)&log_rec->put_vbuf[0], log_rec->put_tab_size, 
		(DB_OWN_NAME*)&log_rec->put_vbuf[log_rec->put_tab_size], log_rec->put_own_size, 
		&log_rec->put_tid, 
		log_rec->put_pg_type, log_rec->put_page_size,
		log_rec->put_comptype,
		log_rec->put_cnf_loc_id, log_rec->put_loc_cnt,
		0, log_row, log_lsn, log_rec->put_row_version, 
		&log_rec->put_seg_hdr, &lsn, err_code);
	    if (status != E_DB_OK)
	    {
		dm0p_unmutex(tabio, (i4)0, dmve->dmve_lk_id, &page);

	    	/*
	     	 * Bug56702: return logfull indication.
	      	 */
	    	dmve->dmve_error.err_data = *err_code;
		ule_format(*err_code, (CL_ERR_DESC *)0, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 0); 
		*err_code = E_DM963A_UNDO_PUT;
		return(E_DB_ERROR);
	    }
	}
    }
    else
    {
	/*
	** If we are processing recovery of an Insert CLR (redo-ing the undo
	** of an insert) then we don't log a CLR but instead save the LSN
	** of the log record we are processing with which to update the
	** page lsn's.
	*/
	lsn = *log_lsn;
    }

    /* 
    ** Write the LSN of the delete record onto the page, unless nologging.
    */
    if (dmve->dmve_logging)
	DMPP_VPT_SET_PAGE_LOG_ADDR_MACRO(log_rec->put_pg_type, page, lsn);

    DMPP_VPT_SET_PAGE_STAT_MACRO(log_rec->put_pg_type, page, DMPP_MODIFY);

    /*
    ** Remove the inserted record to complete the undo operation.
    */
    if (lock_type == LK_ROW || (log_rec->put_header.flags & DM0L_PHYS_LOCK))
	update_mode = (DM1C_DIRECT | DM1C_ROWLK);
    else
	update_mode = DM1C_DIRECT;

    if (log_rec->put_comptype != TCB_C_NONE)
	update_mode |= DM1C_DMVE_COMP;

    (*plv->dmpp_delete)(log_rec->put_pg_type, log_rec->put_page_size, page,
		update_mode, reclaim_space, &dmve->dmve_tran_id, 
		&log_rec->put_tid, record_size);

    dm0p_unmutex(tabio, (i4)0, dmve->dmve_lk_id, &page);

    return(E_DB_OK);
}



/*{
** Name: dmve_clean - Reclaim space from tuples marked deleted.
**
** Description:
**
** Inputs:
**      page                            Pointer to page to clean
**      plv                             Pointer to page level accessor
**      tabio                           Pointer to table io control block
**      lock_list                       lock list id
**      lock_type
**      lock_action
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
**      10-mar-1997 (stial01)
**          Created.
**      21-may-1997 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
**      28-may-1998 (stial01) 
**          dmve_clean() Support VPS system catalogs
**      07-jul-1998 (stial01) 
**          Changed paramaters
*/
DB_STATUS
dmve_clean(
DMPP_PAGE           *page,
DMPP_ACC_PLV        *plv,
DMP_TABLE_IO        *tabio,
i4             lock_list,
i4             lock_type,
i4             lock_action,
i4             *err_code)
{
    DB_STATUS	    status = E_DB_OK;
    DB_STATUS       get_status;
    i4         get_rec_size;
    char            *get_rec;
    DM_TID          get_tid;
    i4             pos;

    /*
    ** Page clean before REDO insert can always clean the page 
    ** if we have a database or table lock
    **
    ** If physical page lock, we can only clean the data page if
    ** all updates are committed. This is only the case for VPS etabs
    ** which have TCB_PG_V3 page format which have no tuple headers.
    */
    if (lock_type == LK_PAGE && lock_action == LK_PHYSICAL)
    {
	CL_SYS_ERR      sys_err;
	i4              len;
	LG_LSN          oldest_lsn;

	status = LGshow(LG_S_OLD_LSN, (PTR)&oldest_lsn,
		    sizeof(oldest_lsn), &len, &sys_err);
	/* Return error if we cannot clean deleted records */
	if (status != E_DB_OK ||
	    LSN_GTE(DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(tabio->tbio_page_type, page), 
		    &oldest_lsn))
	    return (E_DB_ERROR);
    }

    for (get_tid.tid_tid.tid_line = 0;
	    get_tid.tid_tid.tid_line < 
	    DMPP_VPT_GET_PAGE_NEXT_LINE_MACRO(tabio->tbio_page_type, page);
		get_tid.tid_tid.tid_line++)
    {
	get_status = (*plv->dmpp_get)(tabio->tbio_page_type, 
		tabio->tbio_page_size, page, &get_tid,
		&get_rec_size, &get_rec, NULL, NULL, NULL);
	
	if (get_status == E_DB_ERROR)
	{
	    /*
	    ** DM1C_GET returned an error - this indicates that
	    ** something is wrong with the tuple at the current tid.
	    **
	    ** If user is running with special PATCH flag, then skip
	    ** this row and go on to the next, otherwise return with
	    ** an error.
	    */
	    if (DMZ_REC_MACRO(1))
		continue;
	    *err_code = E_DM938B_INCONSISTENT_ROW;
	    status = E_DB_ERROR;
	    break;
	}

	/*
	** Skip if not a deleted record
	*/
	if (get_status != E_DB_WARN || get_rec == NULL)
	    continue;

	pos = get_tid.tid_tid.tid_line;

	/* NOTE get_rec_len is not filled in by dmpp_get */
	(*plv->dmpp_reclen)(tabio->tbio_page_type, tabio->tbio_page_size, 
		page, (i4)pos, &get_rec_size);

	/*
	** Mutex the page while updating it.
	*/
	dm0p_mutex(tabio, (i4)0, lock_list, &page);

	(*plv->dmpp_delete)(tabio->tbio_page_type, tabio->tbio_page_size, page,
		(i4)DM1C_DIRECT, (i4)TRUE, (DB_TRAN_ID *)0,
		&get_tid, get_rec_size);

	dm0p_unmutex(tabio, (i4)0, lock_list, &page);
    }

    return (status);
}
