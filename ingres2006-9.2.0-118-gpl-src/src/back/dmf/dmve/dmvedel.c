/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <pc.h>
#include    <me.h>
#include    <sl.h>
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
#include    <dm1c.h>
#include    <dm1r.h>
#include    <dm1h.h>
#include    <dmve.h>
#include    <dmpp.h>
#include    <dm1b.h>
#include    <dm0l.h>
#include    <dm0p.h>
#include    <dmftrace.h>

/*
**  Name: DMVEDEL.C - The recovery of a delete record operation.
**
**  Description:
**	This file contains the routine for the recovery of a delete record 
**	operation.
**
**          dmve_del - The recovery of a delete record operation.
**
**
**  History:
**      30-jun-86 (ac)    
**          Created new for Jupiter.
**	15-Jun-88 (rogerk)
**	    Added REDO processing code - dmv_redel routine.
**	 3-may-1989 (rogerk)
**	    When searching for secondary index tuples to update, build a key
**	    for the index rather than the entire tuple.  The search routines
**	    expect a key, not a tuple.
**	12-may-89 (anton)
**	    Local collation support.
**	29-may-89 (rogerk)
**	    Check status from dm1c_comp_rec calls.
**	15-aug-89 (rogerk)
**	    Added support for Non-SQL Gateway.  Put in safety check for log
**	    records for gateway tables that are accidentally logged.  No log
**	    records should be written for operations on gateway tables.
**	31-aug-1990 (mikem)
**	    bug #32984
**	    Redo recovery of compressed deletes would fail if the tuple delete
**	    had never actually made it to disk.  We were comparing a compressed
**	    tuple to an uncompressed tuple, and thus never deleting the old
**	    tuple.  The result of this was that in the case of replaces one
**	    might end up with both the old and new tuple.  In the case of 
**	    deletes one may end up with the deleted tuple back in the relation.
**	14-jun-1991 (Derek)
**	    Made changes related to changes in access method specific interface
**	    changes.
**	25-sep-1991 (mikem) integrated following change: 18-Oct-90 (walt)
**	    Extend fix for bug # 33443 (bad undo of replace in btree secondary)
**	    to dmve_del also.
**	25-sep-1991 (mikem) integrated following change: 19-nov-90 (rogerk)
**	    Changed to use the rcb's adf control block rather than allocating
**	    local adf cb's off of the stack.  This was done as part of the DBMS
**	    timezone changes.
**	25-sep-1991 (mikem) integrated following change: 14-jan-1991 (bryanp)
**	    Added support for Btree index compression -- new arguments to
**	    dm1bxinsert() & dm1bxdelete().
**	7-july-1992 (jnash)
**	    Add DMF function prototypes.
**	3-oct-1992 (bryanp)
**	    Re-arrange includes to share transaction ID type definition.
**	26-oct-1992 (jnash & rogerk)
**	    Rewritten for 6.5 recovery.
**	14-dec-1992 (rogerk)
**	    Include dm1b.h before dm0l.h for prototype definitions.
**	16-dec-1992 (jnash)
**	    Use dmpp_reallocate instead of dmpp_allocate for checking
**	    available space on page.
**	17-dec-1992 (rogerk)
**	    Fixed problem that left pages fixed in redo recovery.
**	18-jan-1993 (rogerk)
**	    Add check in undo for case when a null page pointer is passed in 
**	    because no recovery is needed on it.
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
**	    Added error messages.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	26-jul-1993 (bryanp)
**	    Replace all uses of DM_LKID with LK_LKID.
**	26-jul-1993 (rogerk)
**	    Added tracing of page information during recovery.
**      11-aug-93 (ed)
**          added missing includes
**	20-sep-1993 (rogerk)
**	    Final support added for non-journaled tables.  Deletions of core
**	    catalog rows are always journaled while insertions of core catalog
**	    rows are never journaled.  Full explanation of non journaled table 
**	    handling given in DMRFP.
**      18-oct-1993 (jnash)
**          Add page dump on certain errors.
**	31-jan-1994 (jnash)
**	    In dmv_redel(), save table name and owner in rfp action list if
**	    nonjournaled table delete from iirelation.  This is used to
**	    write warning to errlog.log when table is deleted (in rfp
**	    process_et_action()).
**	15-apr-1994 (chiku)
**	    Bug56702: return logfull indication.
**      20-may-95 (hanch04)
**          Added include dmtcb.h for ndmfapi
**	23-dec-1996 (thaju02)
**	    Bug #78398 - on hp8_us5, sigbus occurs during rollforward (redo)
**	    of iirelation delete (dmv_redel).  Extract table name and owner, 
**	    using MEcopy, instead of STRUCT_ASSIGN_MACRO.
**	06-mar-1996 (stial01 for bryanp)
**	    Pass tbio_page_size as the page_size argument to dmpp_reallocate.
**	    Pass tbio_page_size as the page_size argument to dmpp_reclen.
**	06-mar-1996 (stial01 for bryanp)
**	    Remove DB_MAXTUP dependencies in favor of DM_MAXTUP, the internal
**	    DMF maximum tuple size.
**      06-mar-1996 (stial01)
**          Pass del_page_size to dmve_trace_page_header, dmve_trace_page_info
**	06-may-1996 (thaju02)
**	    New page format support: change page header references to use
**	    macros. Account for tuple header.
**      20-may-1996 (ramra01)
**          Added DMPP_TUPLE_INFO to get/put accessor routine
**      03-june-1996 (stial01)
**          Use DMPP_INIT_TUPLE_INFO_MACRO to init DMPP_TUPLE_INFO
**      22-jul-1996 (ramra01 for bryanp)
**          Pass row_version argument to dmpp_get.
**          Pass row_version argument to dm0l_del.
**    19-Nov-1996 (hanch04)
**        set row_version for dmv_redel.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Delete accessor: added reclaim_space 
**          Put accessor: changed DMPP_TUPLE_INFO param to table_version
**          Do not get page lock if row locking.
**          Add lock_type argument to dm0p_lock_page and dm0p_unlock_page
**      10-mar-97 (stial01)
**          dmv_del() If IX page lock held, mutex page before looking at it
**          dmv_del: Clean ALL deleted tuples if X page lock.
**          dmv_redel() Space is reclaimed when redoing DELETE
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          Add lock_id argument to dm0p_lock_page and dm0p_unlock_page.
**      21-may-1997 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
**      29-may-1997 (stial01)
**          Use dm0p_lock_buf_macro to lock buffer to reduce duration dmve  
**          holds buffer mutex.
**      18-jun-1997 (stial01)
**          dmve_del() Request IX page lock if row locking.
**      30-jun-97 (dilma04)
**          Bug 83301. Set DM0P_INTENT_LK fix action if row locking.
**	25-Aug-1997 (jenjo02)
**	    Added log_id parm to dm2t_ufx_tabio_cb() calls.
**      28-may-1998 (stial01)
**          Support VPS system catalogs, changed parameters for dmve_clean
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
**          Page cleans necessary only before put
**	04-May-2000 (jenjo02)
**	    Pass blank-suppressed lengths of table/owner names to 
**	    dm0l_del, eliminating need for dmve_name_unpack().
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
**      18-feb-2004 (stial01)
**          Fixed incorrect casting of length arguments.
**      31-jan-05 (stial01)
**          Added flags arg to dm0p_mutex call(s).
**      21-feb-2005 (stial01)
**          Cross integrate 470401 (inifa01) for INGSRV2569 b111886
**          New flag DM1C_DMVE_COMP passed to dmpp_delete data is compressed
**	06-Mar-2007 (jonj)
**	    Replace dm0p_cachefix_page() with dmve_cachefix_page()
**	    for Cluster REDO recovery.
*/


/*
** Forward declarations of static functions:
*/
static DB_STATUS	dmv_redel(
				DMVE_CB             *dmve,
				DMP_TABLE_IO	    *tabio,
				DMPP_PAGE	    *page,
				DM0L_DEL	    *log_rec,
				DMPP_ACC_PLV 	    *plv,
				i4             lock_type,
				i4             lock_action,
				i4             *error);

static DB_STATUS	dmv_undel(
				DMVE_CB             *dmve,
				DMP_TABLE_IO	    *tabio,
				DMPP_PAGE	    *page,
				DM0L_DEL	    *log_rec,
				DMPP_ACC_PLV 	    *plv,
				i4             lock_type,
				i4             lock_action,
				i4             *error);


/*{
** Name: dmve_del - The recovery of a delete record operation.
**
** Description:
**	This function is used to do, redo and undo(only for btree leaf page) 
**	a delete record operation of a transaction. This function deletes the 
**	record from a table or adds the record to a btree leaf page.
**
** Inputs:
**	dmve_cb
**	    .dmve_log_rec	    The log record of the delete operation.
**	    .dmve_action	    Should be DMVE_DO, DMVE_REDO, or DMVE_UNDO(
**				    for btree leaf page only)
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
**	30-jun-86 (ac)
**          Created for Jupiter.
**	15-jun-88 (rogerk)
**	    Added REDO support.
**	15-aug-89 (rogerk)
**	    Added support for Non-SQL Gateway.  Put in safety check for log
**	    records for gateway tables that are accidentally logged.  No log
**	    records should be written for operations on gateway tables.
**	25-sep-1991 (mikem) integrated following change: 18-Oct-90 (walt)
**	    Extend fix for bug # 33443 (bad undo of replace in btree secondary)
**	    to dmve_del also.
**	25-sep-1991 (mikem) integrated following change: 14-jan-1991 (bryanp)
**	    Added support for Btree index compression -- new arguments to
**	    dm1bxinsert().
**	15-jul-1992 (kwatts)
**	    MPF change. Added the TCB parameter on call of dm1bxinsert.
**	26-oct-1992 (jnash & rogerk)
**	    Rewritten for 6.5 recovery.  Now tables may be "partially 
**	    opened" by this routine, in accordance with new dm2t and buffer 
**	    manager protocols.  
**	17-dec-1992 (rogerk)
**	    Fixed problem that left pages fixed in redo recovery.  Check
**	    proper page_ptr value to decide whether to unfix page, not the 
**	    temporary 'page' value.
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
**	    New page format support: change page header references to use
**	    macros.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Do not get page lock if row locking.
**          Add lock_type argument to dm0p_lock_page and dm0p_unlock_page
**      10-mar-97 (stial01)
**          dmv_del() If IX page lock held, mutex page before looking at it
**          dmv_del: Clean ALL deleted tuples if X page lock.
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          Add lock_id argument to dm0p_lock_page and dm0p_unlock_page.
**      21-may-1997 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
**      18-jun-1997 (stial01)
**          dmve_del() Request IX page lock if row locking.
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
**	 8-Oct-2003 (wanfr01) 
**	    Bug 111057, INGSRV 2542
**	    The fact that REDO is allowed to skip records for non-journaled
**	    tables means the UNDO has a risk of E_DM9665.  Ignore E_DM9665 
**	    when doing UNDO for non-journaled tables.
**      23-feb-2004 (thaju02) Bug 111470 INGSRV2635
**          For rollforwarddb -b option, do not compare the LSN's on the
**          page to that of the log record.
**	01-Dec-2006 (kiria01) b117225
**	    Initialise the lockid parameters that will be passed to LKrequest
**	    to avoid random implicit lock conversions.
*/
DB_STATUS
dmve_del(
DMVE_CB		*dmve)
{
    DM0L_DEL		*log_rec = (DM0L_DEL *)dmve->dmve_log_rec;
    LG_LSN		*log_lsn = &log_rec->del_header.lsn; 
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
    bool                buf_locked = FALSE;
    i4             lock_type = LK_TABLE;
    i4             fix_action = 0;

    MEfill(sizeof(LK_LKID), 0, &lockid);
    MEfill(sizeof(LK_LKID), 0, &page_lockid);

    for (;;)
    {
	/*
	** Consistency Check:  check for illegal log records.
	*/
	if ((log_rec->del_header.type != DM0LDEL) ||
	    (log_rec->del_header.length != 
		(sizeof(DM0L_DEL) + log_rec->del_rec_size -
			(DB_MAXNAME - log_rec->del_tab_size) -
			(DB_MAXNAME - log_rec->del_own_size))))
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
	status = dmve_fix_tabio(dmve, &log_rec->del_tbl_id, &tbio, &error);
	if (DB_FAILURE_MACRO(status))
	    break;
	if ((status == E_DB_WARN) && (error == W_DM9660_DMVE_TABLE_OFFLINE))
	    return (E_DB_OK);

	/*
	** Get page accessors for page recovery actions.
	*/
	dm1c_getaccessors(TCB_C_DEFAULT, log_rec->del_pg_type, 
		log_rec->del_page_size, &loc_plv, &loc_tlv);

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
	    status = dm2t_lock_table(dcb, &log_rec->del_tbl_id, DM2T_IX,
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
		if (log_rec->del_header.flags & DM0L_PHYS_LOCK)
		    lock_action = LK_PHYSICAL;
 
                if (log_rec->del_header.flags & DM0L_ROW_LOCK)
		    lock_type = LK_ROW;
		else
		    lock_type = LK_PAGE;

		status = dm0p_lock_page(dmve->dmve_lk_id, dcb, 
		    &log_rec->del_tbl_id, 
		    log_rec->del_tid.tid_tid.tid_page, LK_PAGE,
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
				    tbio, log_rec->del_tid.tid_tid.tid_page,
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
	    dmve_trace_page_info(log_rec->del_pg_type, log_rec->del_page_size,
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
		DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(log_rec->del_pg_type, page), 
		log_lsn) && ((dmve->dmve_flags & DMVE_ROLLDB_BOPT) == 0))
	    {
		if (dmve->dmve_action == DMVE_DO)
		{
		    ule_format(E_DM9665_PAGE_OUT_OF_DATE, (CL_ERR_DESC *)0, 
			ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, 
			&loc_error, 8, 
			sizeof(*tbio->tbio_relid), tbio->tbio_relid,
			sizeof(*tbio->tbio_relowner), tbio->tbio_relowner, 0, 
			DMPP_VPT_GET_PAGE_PAGE_MACRO(log_rec->del_pg_type, page),
			0, DMPP_VPT_GET_PAGE_STAT_MACRO(log_rec->del_pg_type, 
			    page),
			0, DMPP_VPT_GET_LOG_ADDR_HIGH_MACRO(log_rec->del_pg_type,
			    page), 
			0, DMPP_VPT_GET_LOG_ADDR_LOW_MACRO(log_rec->del_pg_type,
			    page),
			0, log_lsn->lsn_high, 0, log_lsn->lsn_low);
		}
		page = NULL;
	    }
	    break;

	case DMVE_UNDO:
            if (LSN_LT(
		DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(log_rec->del_pg_type, page), 
		log_lsn))
	    {
		/* Non-journaled table records are not necessarily redone during
		   journal processing */
		if (!(log_rec->del_header.flags & DM0L_NONJNL_TAB))
		{
		   ule_format(E_DM9665_PAGE_OUT_OF_DATE, (CL_ERR_DESC *)0, 
		    ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, 
		    &loc_error, 8, 
		    sizeof(*tbio->tbio_relid), tbio->tbio_relid,
		    sizeof(*tbio->tbio_relowner), tbio->tbio_relowner,
		    0, DMPP_VPT_GET_PAGE_PAGE_MACRO(log_rec->del_pg_type, page),
		    0, DMPP_VPT_GET_PAGE_STAT_MACRO(log_rec->del_pg_type, page),
		    0, DMPP_VPT_GET_LOG_ADDR_HIGH_MACRO(log_rec->del_pg_type, 
			page), 
		    0, DMPP_VPT_GET_LOG_ADDR_LOW_MACRO(log_rec->del_pg_type,
			page),
		    0, log_lsn->lsn_high, 0, log_lsn->lsn_low);
		   error = E_DM9666_PAGE_LSN_MISMATCH;
		   status = E_DB_ERROR;
		}
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
	if (log_rec->del_header.flags & DM0L_CLR)
	    recovery_action = DMVE_UNDO;

	/*
	** Call appropriate recovery action depending on the recovery type.
	*/
	switch (recovery_action)
	{
	case DMVE_DO:
	case DMVE_REDO:
	    status = dmv_redel(dmve, tbio, page, log_rec, loc_plv, 
			lock_type, lock_action, &error);
	    break;

	case DMVE_UNDO:
	    status = dmv_undel(dmve, tbio, page, log_rec, loc_plv, 
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
	    &log_rec->del_tbl_id, log_rec->del_tid.tid_tid.tid_page, 
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
	dmve->dmve_error.err_code = E_DM9607_DMVE_DEL;

    return(status);
}


/*{
** Name: dmv_redel - Redo the delete of a record 
**
** Description:
**      This function removes a record from a table for the recovery of a
**	delete record operation.
**
** Inputs:
**      dmve				Pointer to dmve control block.
**      tabio				Pointer to table io control block
**      page				Pointer to the page of the delete
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
**	26-oct-1992 (jnash & rogerk)
**	    Rewritten for 6.5 recovery project.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**	20-sep-1993 (rogerk)
**	    Handling for non-journaled tables.  During rollforward, we
**	    must be aware that system catalog rows for non-journaled tables
**	    may not exist.  Repeating history is not enforced for these rows.
**	    If a delete operation is requested on a core catalog row that
**	    represents a non-journaled table, ignore row-not-exist errors.
**	    If the row is found to exist, then record in the rollforward action
**	    list that we successfully deleted the row.  Without this action
**	    a subsequent undo of the delete will be bypassed.
**      18-oct-1993 (jnash)
**          Add page dump on certain errors.
**	31-jan-1994 (jnash)
**	    Save table name and owner in rfp action list if nonjournaled table 
**	    drop (delete from iirelation).
**	23-dec-1996 (thaju02)
**	    Bug #78398 - on hp8_us5, sigbus occurs during rollforward (redo)
**	    of iirelation delete.  Extract table name and owner, using 
**	    MEcopy, instead of STRUCT_ASSIGN_MACRO.
**	06-mar-1996 (stial01 for bryanp)
**	    Pass tbio_page_size as the page_size argument to dmpp_reclen.
**	06-may-1996 (thaju02)
**	    New page format support: change page header references to use
**	    macros. Account for tuple header.
**      20-may-1996 (ramra01)
**          Added DMPP_TUPLE_INFO to get accessor routine
**      03-june-1996 (stial01)
**          Use DMPP_INIT_TUPLE_INFO_MACRO to init DMPP_TUPLE_INFO
**      22-jul-1996 (ramra01 for bryanp)
**          Pass row_version argument to dmpp_get.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Delete accessor: added reclaim_space
**      10-mar-97 (stial01)
**          dmv_redel() Space is reclaimed when redoing DELETE
**      21-may-1997 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
**	14-Jan-2004 (jenjo02)
**	    Check db_tab_index > 0 for index, not just != 0.
*/
static DB_STATUS
dmv_redel(
DMVE_CB             *dmve,
DMP_TABLE_IO	    *tabio,
DMPP_PAGE	    *page,
DM0L_DEL	    *log_rec,
DMPP_ACC_PLV 	    *plv,
i4             lock_type,
i4             lock_action,
i4             *err_code)
{
    LG_LSN		*page_lsn = 
	DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(log_rec->del_pg_type, page);
    LG_LSN		*log_lsn = &log_rec->del_header.lsn; 
    DM_TID		temp_tid;
    DB_STATUS		status = E_DB_OK;
    i4		record_size;
    i4		compare_size;
    char		*record;
    char		*log_row;
    RFP_REC_ACTION	*action;
    DMP_RELATION	*relrow;
    DB_TAB_ID		reltid;
    i4             reclaim_space;
    i4                  update_mode;
 
    /*
    ** If there is nothing to recover, just return.
    */
    if (page == NULL)
	return (E_DB_OK);

    /*
    ** If large pages, the redo delete should not reclaim space
    ** If row locking or physical page locking, don't shift the data on the page
    ** If uncompressed data we never need to shift the data on the page
    */ 
    if (tabio->tbio_page_type != TCB_PG_V1 ||
	log_rec->del_comptype == TCB_C_NONE ||
	(log_rec->del_header.flags & DM0L_PHYS_LOCK))
	reclaim_space = FALSE;
    else
	reclaim_space = TRUE;

    /*
    ** Redo recovery must follow row locking protocols if phys locking
    ** NOTE we can't be row locking during redo recovery
    */
    if (log_rec->del_header.flags & DM0L_PHYS_LOCK)
	update_mode = (DM1C_DIRECT | DM1C_ROWLK);
    else
	update_mode = DM1C_DIRECT;

    if (log_rec->del_comptype != TCB_C_NONE)
	update_mode |= DM1C_DMVE_COMP;

    log_row = &log_rec->del_vbuf[log_rec->del_tab_size + log_rec->del_own_size];


    /*
    ** Consistency Checks:
    ** 
    ** Make sure that the tuple in question exists before attempting to
    ** delete it.
    **
    ** We ignore not-exist situations in rollforwarddb when the tuple is a
    ** system catalog row describing a non-journaled table.  Repeating history
    ** is not fully implemented for system catalogs; rows describing non-
    ** journaled tables are not re-inserted into the core catalogs.
    */
    status = (*plv->dmpp_get)(log_rec->del_pg_type, log_rec->del_page_size, 
	page, &log_rec->del_tid, &record_size, &record, NULL, NULL, NULL);
    if (status != E_DB_OK)
    {
	if ((dmve->dmve_action == DMVE_DO) &&
	    (log_rec->del_header.flags & DM0L_NONJNL_TAB))
	{
	    return (E_DB_OK);
	}

	ule_format(E_DM966D_DMVE_ROW_MISSING, (CL_ERR_DESC *)0, ULE_LOG,
	    NULL, (char *)0, (i4)0, (i4 *)0, err_code, 6, 
	    sizeof(DB_DB_NAME), tabio->tbio_dbname->db_db_name,
	    log_rec->del_tab_size, &log_rec->del_vbuf[0],
	    log_rec->del_own_size, &log_rec->del_vbuf[log_rec->del_tab_size],
	    0, log_rec->del_tid.tid_tid.tid_page,
	    0, log_rec->del_tid.tid_tid.tid_line,
	    0, status);
	dmd_log(1, log_rec, 4096);
	dmve_trace_page_header(log_rec->del_pg_type, log_rec->del_page_size, 
	    page, plv, "DATA");
	dmve_trace_page_contents(log_rec->del_pg_type, log_rec->del_page_size,
	    page, "DATA");
	*err_code = E_DM961E_DMVE_REDEL;
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
    ** If the table effected is IIRELATION we only compare a prefix of the
    ** row since iirelation is periodically updated without logging the
    ** changes. IISEQUENCE tuples are also updated without logging, so
    ** treat them the same way.
    */
    (*plv->dmpp_reclen)(log_rec->del_pg_type, log_rec->del_page_size, page,
			(i4)log_rec->del_tid.tid_tid.tid_line, &record_size);

    compare_size = record_size;
    if (log_rec->del_tbl_id.db_tab_base == DM_1_RELATION_KEY ||
        log_rec->del_tbl_id.db_tab_base == DM_B_SEQ_TAB_ID)
	compare_size = DB_MAXNAME + 16;
    else
	compare_size = record_size;

    if ((log_rec->del_rec_size != record_size) ||
	(MEcmp((PTR)log_row, (PTR)record, compare_size) != 0))
    {
	ule_format(E_DM966C_DMVE_TUPLE_MISMATCH, (CL_ERR_DESC *)0, ULE_LOG,
	    NULL, (char *)0, (i4)0, (i4 *)0, err_code, 8, 
	    sizeof(DB_DB_NAME), tabio->tbio_dbname->db_db_name,
	    log_rec->del_tab_size, &log_rec->del_vbuf[0],
	    log_rec->del_own_size, &log_rec->del_vbuf[log_rec->del_tab_size],
	    0, log_rec->del_tid.tid_tid.tid_page,
	    0, log_rec->del_tid.tid_tid.tid_line,
	    0, record_size, 0, log_rec->del_rec_size,
	    0, dmve->dmve_action);
	dmd_log(1, log_rec, 4096);
	dmve_trace_page_header(log_rec->del_pg_type, log_rec->del_page_size,
	    page, plv, "DATA");
	dmve_trace_page_contents(log_rec->del_pg_type, log_rec->del_page_size,
	    page, "DATA");
	ule_format(E_DM964B_REDO_DEL, (CL_ERR_DESC *)0, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, err_code, 0); 
    }


    /*
    ** Mutex the page while updating it.
    */
    dm0p_mutex(tabio, (i4)0, dmve->dmve_lk_id, &page);

    /*
    ** Redo the delete operation.
    */
    (*plv->dmpp_delete)(log_rec->del_pg_type, log_rec->del_page_size, page,
	update_mode, reclaim_space, &dmve->dmve_tran_id,
	&log_rec->del_tid, log_rec->del_rec_size);

    /*
    ** Write the LSN of the Del log record to the updated page.
    */
    DMPP_VPT_SET_PAGE_LOG_ADDR_MACRO(log_rec->del_pg_type, page, *log_lsn);
    DMPP_VPT_SET_PAGE_STAT_MACRO(log_rec->del_pg_type, page, DMPP_MODIFY);

    dm0p_unmutex(tabio, (i4)0, dmve->dmve_lk_id, &page);

    /*
    ** Non-journal table handling:  When we delete a system catalog row
    ** that describes a non-journaled table during journal recovery table
    ** we pend an action item recording this event.  This information will
    ** be referred to if this delete is undone to determine whether or not
    ** to put back the deleted row.  We only want to put the row back during
    ** journal recovery if the row was actually deleted.  If this routine
    ** found that the row to delete already did not exist, then we don't want
    ** undo recovery to recreate the row.
    */
    if ((dmve->dmve_action == DMVE_DO) &&
	(log_rec->del_header.flags & DM0L_NONJNL_TAB))
    {
	status = rfp_add_action(dmve, &dmve->dmve_tran_id, log_lsn, 
			RFP_DELETE, &action, err_code);
	if (status != E_DB_OK)
	{
	    ule_format(*err_code, (CL_ERR_DESC *)0, ULE_LOG,
		NULL, (char *)0, (i4)0, (i4 *)0, err_code, 0); 
	    *err_code = E_DM961E_DMVE_REDEL;
	    return(E_DB_ERROR);
	}

	action->rfp_act_dbid = log_rec->del_header.database_id;
	STRUCT_ASSIGN_MACRO(log_rec->del_tbl_id, action->rfp_act_tabid);
	STRUCT_ASSIGN_MACRO(log_rec->del_tid, action->rfp_act_tid);

	/*
	** If this is an iirelation delete: 
	**  o Determine if it is associated with a drop index operation.
	**    This has special handling in iirelation replace recovery.
	**  o Save info used during UNDO to determine if the delete 
	**    has actually been done.  This information is also used  
	**    when the ET is processed to issue a warning that the table 
	**    has been dropped by recovery.
	*/
	if ((log_rec->del_tbl_id.db_tab_base == DM_1_RELATION_KEY) &&
	    (log_rec->del_tbl_id.db_tab_index == 0))
	{
	    /* Extract user table id from iirelation row */
	    relrow = (DMP_RELATION *) log_row;

	    MEcopy((PTR)&relrow->reltid, sizeof(DB_TAB_ID), (PTR)&reltid);
	    STRUCT_ASSIGN_MACRO(reltid, action->rfp_act_usertabid);
	    MEcopy((PTR)&relrow->relid, sizeof(DB_TAB_NAME),
			(PTR)&action->rfp_act_tabname);
	    MEcopy((PTR)&relrow->relowner, sizeof(DB_OWN_NAME), 
			(PTR)&action->rfp_act_tabown);
	    action->rfp_act_index_drop = (reltid.db_tab_index > 0);
	}
    }
    
    return(E_DB_OK);
}

/*{
** Name: dmv_undel - UNDO of a del operation.
**
** Description:
**      This function restores the logged copy of a record to a table for the 
**	recovery of a delete record operation.
**
** Inputs:
**      dmve				Pointer to dmve control block.
**      tabio				Pointer to table io control block
**      page				Pointer to the page of the delete
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
**	16-dec-1992 (jnash)
**	    Use dmpp_reallocate instead of dmpp_allocate for checking
**	    available space on page.
**	18-jan-1993 (rogerk)
**	    Add check for case when a null page pointer is passed in because
**	    no recovery is needed on it.
**	15-mar-1993 (jnash)
**	    Check dmve->dmve_logging to determine if logging required.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**	20-sep-1993 (rogerk)
**	    Handling for non-journaled tables.  During rollforward, we
**	    must be aware that system catalog rows for non-journaled tables
**	    may not exist.  Repeating history is not enforced for these rows.
**	    If a delete operation is requested on a core catalog row that
**	    represents a non-journaled table and then rolled back, we do
**	    not want to restore the row if it did not exist at the start of
**	    the rollforward operation.  Check the rollforward action list first
**	    to see if we really deleted the row.
**      18-oct-1993 (jnash)
**          Add page dump on certain errors.
**	15-apr-1994 (chiku)
**	    Bug56702: return logfull indication.
**	06-mar-1996 (stial01 for bryanp)
**	    Pass tbio_page_size as the page_size argument to dmpp_reallocate.
**	06-may-1996 (thaju02)
**	    New page format support: change page header references to use
**	    macros. Account for tuple header.
**      20-may-1996 (ramra01)
**          Added DMPP_TUPLE_INFO to get/put accessor routine
**      03-june-1996 (stial01)
**          Use DMPP_INIT_TUPLE_INFO_MACRO to init DMPP_TUPLE_INFO
**      22-jul-1996 (ramra01 for bryanp)
**          Pass row_version argument to dmpp_get.
**          Pass row_version argument to dm0l_del.
**    19-Nov-1996 (hanch04)
**        set row_version for dmv_undel.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Put accessor: changed DMPP_TUPLE_INFO param to table_version
**      21-may-1997 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
*/
static DB_STATUS
dmv_undel(
DMVE_CB             *dmve,
DMP_TABLE_IO	    *tabio,
DMPP_PAGE	    *page,
DM0L_DEL	    *log_rec,
DMPP_ACC_PLV 	    *plv,
i4             lock_type,
i4             lock_action,
i4             *err_code)
{
    DMP_DCB		*dcb = dmve->dmve_dcb_ptr;
    LG_LSN		*page_lsn = 
	DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(log_rec->del_pg_type, page);
    LG_LSN		*log_lsn = &log_rec->del_header.lsn; 
    LG_LSN		*delete_lsn;
    LG_LSN		lsn;
    DM_TID		loc_tid;
    DB_STATUS		status = E_DB_OK;
    char		*log_row;
    char		*record = NULL;
    i4		record_size;
    i4		flags;
    RFP_REC_ACTION	*action;
    i4             update_mode;
 
    /*
    ** If there is nothing to recover, just return.
    */
    if (page == NULL)
    {
	return (E_DB_OK);
    }

    /*
    ** Non-journal table handling:  When we delete a system catalog row
    ** that describes a non-journaled table during journal recovery table
    ** we pend an action item recording the event.  If later this routine
    ** is called to undo the delete, we need to lookup the action to verify
    ** that the original row was actually deleted by journal recovery.  
    ** If the row was not actually deleted because it already did not exist
    ** at the start of journal recovery, then we don't want this undo
    ** operation to restore the row.
    */
    if ((dmve->dmve_action == DMVE_DO) &&
	(log_rec->del_header.flags & DM0L_NONJNL_TAB))
    {
	delete_lsn = ((log_rec->del_header.flags & DM0L_CLR) ? 
			    &log_rec->del_header.compensated_lsn : log_lsn);
		
	rfp_lookup_action(dmve, &dmve->dmve_tran_id, delete_lsn, 
			    RFP_REC_LSN, &action);
	if (action == NULL)
	{
	    return (E_DB_OK);
	}

	/* Delete the action item since it has fulfilled its purpose. */
	rfp_remove_action(dmve, action);
    }

    log_row = &log_rec->del_vbuf[log_rec->del_tab_size + log_rec->del_own_size];

    /*
    ** Make sure that there is not already a tuple at the spot at which 
    ** we are about to write our record.
    **
    ** Also guarantee that sufficient space exists on the page.
    ** The "repeating history" rules of this recorvery should guarantee
    ** that the row will fit since it must have fit originally.  If there
    ** is no room, this is a fatal recovery error.
    */
    if (log_rec->del_tid.tid_tid.tid_line < 
	(u_i2) DMPP_VPT_GET_PAGE_NEXT_LINE_MACRO(log_rec->del_pg_type, page))
    {
	status = (*plv->dmpp_get)(log_rec->del_pg_type, log_rec->del_page_size,
	    page, &log_rec->del_tid, &record_size, &record, NULL, NULL, NULL);
	if (status != E_DB_WARN)
	{
	    ule_format(E_DM966E_DMVE_ROW_OVERLAP, (CL_ERR_DESC *)0, ULE_LOG,
		NULL, (char *)0, (i4)0, (i4 *)0, err_code, 5, 
		sizeof(DB_DB_NAME), tabio->tbio_dbname->db_db_name,
		log_rec->del_tab_size, &log_rec->del_vbuf[0],
		log_rec->del_own_size, &log_rec->del_vbuf[log_rec->del_tab_size],
		0, log_rec->del_tid.tid_tid.tid_page,
		0, log_rec->del_tid.tid_tid.tid_line);
	    dmd_log(1, log_rec, 4096);
	    dmve_trace_page_header(log_rec->del_pg_type, log_rec->del_page_size,
		page, plv, "DATA");
	    dmve_trace_page_contents(log_rec->del_pg_type, 
		log_rec->del_page_size, page, "DATA");
	    *err_code = E_DM964A_UNDO_DEL;
	    return(E_DB_ERROR);
	}
    }

    if (log_rec->del_pg_type != TCB_PG_V1)
    {
	/* 
	** Check for sufficient space on the page to insert the row.
	** If UNDO and row locking, we should not need to clean deleted records
	*/
	status = (*plv->dmpp_reallocate)(log_rec->del_pg_type, 
	    log_rec->del_page_size, page,
	    log_rec->del_tid.tid_tid.tid_line, log_rec->del_rec_size);
	if (status != E_DB_OK && lock_type != LK_ROW)
	{
	    status = dmve_clean(page, plv, tabio, dmve->dmve_lk_id, 
		    lock_type, lock_action, err_code);
	    status = (*plv->dmpp_reallocate)(log_rec->del_pg_type, 
		log_rec->del_page_size, page,
		log_rec->del_tid.tid_tid.tid_line, log_rec->del_rec_size);
	}
	if (status != E_DB_OK)
	{
	    ule_format(E_DM9674_DMVE_PAGE_NOROOM, (CL_ERR_DESC *)0, ULE_LOG,
		NULL, (char *)0, (i4)0, (i4 *)0, err_code, 6, 
		sizeof(DB_DB_NAME), tabio->tbio_dbname->db_db_name,
		log_rec->del_tab_size, &log_rec->del_vbuf[0],
		log_rec->del_own_size, &log_rec->del_vbuf[log_rec->del_tab_size],
		0, log_rec->del_tid.tid_tid.tid_page,
		0, log_rec->del_tid.tid_tid.tid_line, 
		0, log_rec->del_rec_size);
	    dmd_log(1, log_rec, 4096);
	    dmve_trace_page_header(log_rec->del_pg_type, log_rec->del_page_size,
		page, plv, "DATA");
	    dmve_trace_page_contents(log_rec->del_pg_type, 
		log_rec->del_page_size, page, "DATA");
	    *err_code = E_DM964A_UNDO_DEL;
	    return(E_DB_ERROR);
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
    if ((log_rec->del_header.flags & DM0L_CLR) == 0)
    {
	if (dmve->dmve_logging)
	{
	    flags = (log_rec->del_header.flags | DM0L_CLR);

	    status = dm0l_del(dmve->dmve_log_id, flags, &log_rec->del_tbl_id, 
		(DB_TAB_NAME*)&log_rec->del_vbuf[0], log_rec->del_tab_size, 
		(DB_OWN_NAME*)&log_rec->del_vbuf[log_rec->del_tab_size], log_rec->del_own_size, 
		&log_rec->del_tid, 
		log_rec->del_pg_type, log_rec->del_page_size,
		log_rec->del_comptype,
		log_rec->del_cnf_loc_id, log_rec->del_loc_cnt,
		log_rec->del_rec_size, log_row, log_lsn,
		log_rec->del_row_version,	
		&log_rec->del_seg_hdr, &lsn, err_code);
	    if (status != E_DB_OK)
	    {
		dm0p_unmutex(tabio, (i4)0, dmve->dmve_lk_id, &page);

                /*
		 * Bug56702: return logfull indication.
		 */
		dmve->dmve_error.err_data = *err_code;
		ule_format(*err_code, (CL_ERR_DESC *)0, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 0); 

		*err_code = E_DM964A_UNDO_DEL;
		return(E_DB_ERROR);
	    }
	}
    }
    else
    {
        /*
        ** If we are processing recovery of a Delete CLR (redo-ing the
        ** undo of a delete (wow)) then we don't log a CLR but instead
        ** save the LSN of the log record we are processing with which to
        ** update the page lsn's.
        */
        lsn = *log_lsn;
    }


    /* 
    ** Write the LSN of the delete record onto the page.
    */
    if (dmve->dmve_logging)
	DMPP_VPT_SET_PAGE_LOG_ADDR_MACRO(log_rec->del_pg_type, page, lsn);
    DMPP_VPT_SET_PAGE_STAT_MACRO(log_rec->del_pg_type, page, DMPP_MODIFY);

    /*
    ** Replace the deleted record to complete the undo operation.
    */
    update_mode = DM1C_DIRECT;
    if (lock_type == LK_ROW || (log_rec->del_header.flags & DM0L_PHYS_LOCK))
	update_mode |= DM1C_ROWLK;
    (*plv->dmpp_put)(log_rec->del_pg_type, log_rec->del_page_size, page,
		update_mode, &dmve->dmve_tran_id, &log_rec->del_tid,
		log_rec->del_rec_size, log_row, 0, log_rec->del_row_version,
		&log_rec->del_seg_hdr);

    dm0p_unmutex(tabio, (i4)0, dmve->dmve_lk_id, &page);

    return(E_DB_OK);
}
