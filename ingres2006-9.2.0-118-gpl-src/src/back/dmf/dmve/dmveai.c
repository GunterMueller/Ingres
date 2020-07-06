/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <me.h>
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
#include    <dmve.h>
#include    <dmtcb.h>
#include    <dm2t.h>
#include    <dm2f.h>
#include    <dmpp.h>
#include    <dm1b.h>
#include    <dm1c.h>
#include    <dm0p.h>
#include    <dm0l.h>
#include    <dmftrace.h>


/**
**
**  Name: DMVEAI.C - The Redo recovery of a Page Image log record.
**
**  Description:
**	This file contains the routine for the recovery of a page image record.
**
**          dmve_aipage - The recovery of a page image log record.
**
**  History:
**	14-dec-1992 (rogerk)
**	    Written for Reduced logging project.
**	15-mar-1993 (rogerk)
**	    Reduced Logging - Phase IV:
**	    Changed log record format: added database_id, tran_id and LSN
**		in the log record header.  The LSN of a log record is now
**		obtained from the record itself rather than from the dmve
**		control block.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**		Correct the message parameter lengths for E_DM9665.
**	24-may-1993 (bryanp)
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**      21-jun-1993 (mikem)
**          su4_us5 port.  Added include of me.h to pick up new macro
**          definitions for MEcopy, MEfill, ...
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	26-jul-1993 (bryanp)
**	    Replace all uses of DM_LKID with LK_LKID.
**	26-jul-1993 (rogerk)
**	    Added tracing of page information during recovery.
**	23-aug-1993 (rogerk)
**	    Fix processing of physical page locks.
**	23-may-1994 (andys)
**	    Add cast(s) to quiet qac/lint
**      20-may-95 (hanch04)
**          Added include dmtcb.h for ndmfapi
**	06-mar-1996 (stial01 for bryanp)
**	    Support variable page sizes through the ai_page_size field.
**      06-mar-1996 (stial01)
**          Pass ai_page_size to dmve_trace_page_info()
**      01-may-1996 (stial01)
**          Adjustments for removal of ai_page from DM0L_AI
**	06-may-1996 (thaju02)
**	    New Page Format Project: Change page header references to use
**	    macros.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Add lock_type argument to dm0p_lock_page and dm0p_unlock_page
**	10-jan-1997 (nanpr01)
**	    Put back the ai_page back to DM0L_AI.
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          Add lock_id argument to dm0p_lock_page and dm0p_unlock_page.
**      21-may-1997 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
**	25-Aug-1997 (jenjo02)
**	    Added log_id parm to dm2t_ufx_tabio_cb() calls.
**	23-Jun-1998 (jenjo02)
**	    Added lock_value parm to dm0p_lock_page() and dm0p_unlock_page().
**	    Utilize lock_id value in both of those functions.
**	    Consolidate redundant page unfix/unlock code.
**	14-aug-1998 (nanpr01)
**	    Error code is getting reset to E_DB_OK on error and not making
**	    the database inconsistent.
**      08-dec-1999 (stial01)
**          Undo change for bug#85156, dmpe routines now get INTENT table locks.
**      23-oct-2000 (stial01)
**          Variable Page Size - Variable Page Type support (SIR 102955)
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
**      08-Oct-2002 (hanch04)
**          Removed dm0m_lcopy,dm0m_lfill.  Use MEcopy, MEfill instead.
**      31-jan-05 (stial01)
**          Added flags arg to dm0p_mutex call(s).
**/

static DB_STATUS dmv_reaipage(
			DMVE_CB             *dmve,
			DMP_TABLE_IO	    *tabio,
			DMPP_PAGE	    *page,
			DM0L_AI		    *log_rec,
			DMPP_ACC_PLV	    *loc_plv,
			i4             *err_code);


/*{
** Name: dmve_aipage - The recovery of a page.
**
** Description:
**
** Inputs:
**	dmve_cb
**	    .dmve_log_rec	    The after image log record.
**	    .dmve_action	    Should only be DMVE_UNDO.
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
**	14-dec-1992 (rogerk)
**	    Written for Reduced logging project.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**		Correct the message parameter lengths for E_DM9665.
**	26-jul-1993 (bryanp)
**	    Replace all uses of DM_LKID with LK_LKID.
**	23-aug-1993 (rogerk)
**	    Fix processing of physical page locks.
**	    Use log record flag to determine whether physical locks
**	    should be used.  Remove code which locked the same page
**	    twice in a row.
**	06-may-1996 (thaju02)
**	    New Page Format Project: 
**		Change page header references to macros.
**	    	Pass page size to dm1c_getaccessors().
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Add lock_type argument to dm0p_lock_page and dm0p_unlock_page
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          Add lock_id argument to dm0p_lock_page and dm0p_unlock_page.
**	11-sep-1997 (thaju02) bug#85156 - on recovery lock extension
**	    table with DM2T_S.  Attempting to lock table with DM2T_IX
**	    causes deadlock, dbms resource deadlock error during recovery, 
**	    pass abort, rcp resource deadlock error during recovery, 
**	    rcp marks database inconsistent.
**      19-Jun-2002 (horda03) Bug 108074
**          If the table is locked exclusively, then indicate that SCONCUR
**          pages don't need to be flushed immediately.
**	23-feb-2004 (thaju02) Bug 111470 INGSRV2635
**	    For rollforwarddb -b option, do not compare the LSN's on the
**	    page to that of the log record.    
**	01-Dec-2006 (kiria01) b117225
**	    Initialise the lockid parameters that will be passed to LKrequest
**	    to avoid random implicit lock conversions. Renamed page_lock_id
**	    to page_lockid for consistency.
**	06-Mar-2007 (jonj)
**	    Replace dm0p_cachefix_page() with dmve_cachefix_page()
**	    for Cluster REDO recovery.
*/
DB_STATUS
dmve_aipage(
DMVE_CB		*dmve_cb)
{
    DMVE_CB		*dmve = dmve_cb;
    DM0L_AI		*log_rec = (DM0L_AI *)dmve->dmve_log_rec;
    LG_LSN		*log_lsn = &log_rec->ai_header.lsn; 
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
    i4			lock_action;
    i4			grant_mode;
    i4			recovery_action;
    i4			fix_option = 0;
    i4			error;
    i4			loc_error;
    bool		page_recover;
    bool		physical_page_lock = FALSE;

    MEfill(sizeof(LK_LKID), 0, &lockid);
    MEfill(sizeof(LK_LKID), 0, &page_lockid);

    for (;;)
    {
	/*
	** Consistency Check:  check for illegal log records.
	** After Image log records are used only for Forward processing.
	*/
	if ((log_rec->ai_header.type != DM0LAI) ||
	    (log_rec->ai_header.length < sizeof(DM0L_AI)) ||
	    (dmve->dmve_action == DMVE_UNDO))
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
	status = dmve_fix_tabio(dmve, &log_rec->ai_tbl_id, &tbio, &error);
	if (DB_FAILURE_MACRO(status))
	    break;
	if ((status == E_DB_WARN) && (error == W_DM9660_DMVE_TABLE_OFFLINE))
	    return (E_DB_OK);

	/*
	** Get page accessors for page recovery actions.
	*/
	dm1c_getaccessors(TCB_C_DEFAULT, log_rec->ai_pg_type, 
		log_rec->ai_page_size, &loc_plv, &loc_tlv);

	/*
	** Check recovery requirements for this operation.  If partial
	** recovery is in use, then we may not need to recover all
	** the pages touched by the original update.
	**
	** Also, not all split operations require a data page to be allocated,
	** only those to leaf's in a non secondary index.  If the logged data
	** page is page # 0, then there is no data page to recover.
	*/
	page_recover = dmve_location_check(dmve, (i4)log_rec->ai_loc_id);

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
	    status = dm2t_lock_table(dcb, &log_rec->ai_tbl_id, DM2T_IX,
		dmve->dmve_lk_id, (i4)0, &grant_mode, &lockid, &error);
	    if (status != E_DB_OK)
		break;

	    if (grant_mode != DM2T_X)
	    {
		/*
		** Page lock required.
		** Check if lock must be taken in physical mode (for btree
		** index page or system catalog page operations).
		*/
		if (log_rec->ai_header.flags & DM0L_PHYS_LOCK)
		    lock_action = LK_PHYSICAL;
	
		if (page_recover)
		{
		    status = dm0p_lock_page(dmve->dmve_lk_id, dcb, 
			&log_rec->ai_tbl_id, (DM_PAGENO) log_rec->ai_pageno,
			LK_PAGE, LK_X, lock_action, (i4)0,
                        tbio->tbio_relid, &dmve->dmve_tran_id, &page_lockid, 
                        (i4 *)0, (LK_VALUE *)0, &error);
		    if (status != E_DB_OK)
			break;

		    if (lock_action == LK_PHYSICAL)
			physical_page_lock = TRUE;
		}
	    }
            else
               fix_option |= DM0P_TABLE_LOCKED_X;
	}

	/*
	** Fix the pages we need to recover in cache for write.
	*/
	if (page_recover)
	{
	    status = dmve_cachefix_page(dmve, log_lsn,
					tbio, log_rec->ai_pageno,
					fix_option, loc_plv, 
					&page_ptr, &error);
	    if (status != E_DB_OK)
		break;
	}

	page = page_ptr;

	/*
	** Dump debug trace info about pages if such tracing is configured.
	*/
	if (DMZ_ASY_MACRO(15))
	    dmve_trace_page_info(log_rec->ai_pg_type, log_rec->ai_page_size,
		page, loc_plv, "Page");

	/*
	** Compare the LSN's on the pages with that of the log record
	** to determine what recovery will be needed.
	**
	**   - During Forward processing, if the page's LSN is greater than
	**     the log record then no recovery is needed.
	**
	**   - Currently, during rollforward processing it is unexpected
	**     to find that a recovery operation need not be applied because
	**     of the page's LSN.  This is because rollforward must always
	**     begin from a checkpoint that is previous to any journal record
	**     begin applied.  In the future this requirement may change and
	**     Rollforward will use the same expectations as Redo.
	*/
	if (page && LSN_GTE(
	    DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(log_rec->ai_pg_type, page), 
	    log_lsn) && ((dmve->dmve_flags & DMVE_ROLLDB_BOPT) == 0))
	{
	    if (dmve->dmve_action == DMVE_DO)
	    {
		ule_format(E_DM9665_PAGE_OUT_OF_DATE, (CL_ERR_DESC *)0, 
		    ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, 
		    &loc_error, 8, 
		    sizeof(*tbio->tbio_relid), tbio->tbio_relid,
		    sizeof(*tbio->tbio_relowner), tbio->tbio_relowner,
		    0, DMPP_VPT_GET_PAGE_PAGE_MACRO(log_rec->ai_pg_type, page), 
		    0, DMPP_VPT_GET_PAGE_STAT_MACRO(log_rec->ai_pg_type, page),
		    0, DMPP_VPT_GET_LOG_ADDR_HIGH_MACRO(log_rec->ai_pg_type,page),
		    0, DMPP_VPT_GET_LOG_ADDR_LOW_MACRO(log_rec->ai_pg_type, page),
		    0, log_lsn->lsn_high, 0, log_lsn->lsn_low);
	    }
	    page = NULL;
	}

	status = dmv_reaipage(dmve, tbio, page, log_rec, loc_plv, &error);

	break;
    }

    if (status != E_DB_OK)
    {
	ule_format(error, (CL_ERR_DESC *)0, ULE_LOG, NULL, 
	    (char *)0, (i4)0, (i4 *)0, &error, 0);
    }

    /*
    ** Unfix the updated pages.  No need to force them to disk - they
    ** will be tossed out through normal cache protocols if Fast
    ** Commit or at the end of the abort if non Fast Commit.
    */
    if (page_ptr)
    {
	tmp_status = dm0p_uncache_fix(tbio, DM0P_UNFIX, dmve->dmve_lk_id, 
		dmve->dmve_log_id, &dmve->dmve_tran_id, 
		(DMPP_PAGE **) &page_ptr, &loc_error);
	if (tmp_status != E_DB_OK)
	{
	    ule_format(loc_error, (CL_ERR_DESC *)0, ULE_LOG, NULL, 
		(char *)0, (i4)0, (i4 *)0, &loc_error, 0);
	    if (tmp_status > status)
	        status = tmp_status;
	}
    }

    /*
    ** If a short term physical lock was acquired on the pages
    ** then release them.
    */
    if (physical_page_lock)
    {
	tmp_status = dm0p_unlock_page(dmve->dmve_lk_id, dcb, 
	    &log_rec->ai_tbl_id, (DM_PAGENO) log_rec->ai_pageno, LK_PAGE,
	    tbio->tbio_relid, &dmve->dmve_tran_id, &page_lockid, 
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
	dmve->dmve_error.err_code = E_DM965C_DMVE_AIPAGE;

    return(status);
}

/*{
** Name: dmv_reaipage - REDO of a Page Image log record.
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
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**	23-may-1994 (andys)
**	    Add cast(s) to quiet qac/lint
**	06-mar-1996 (stial01 for bryanp)
**	    Support variable page sizes through the ai_page_size field.
**	    Use dm0m_lcopy to handle 64K page sizes.
**      01-may-1996 (stial01)
**          Adjustments for removal of ai_page from DM0L_AI
**	06-may-1996 (thaju02)
**	    New Page Format Project: Change page header references to macros.
**	10-jan-1997 (nanpr01)
**	    Put back the ai_page back to DM0L_AI.
**      21-may-1997 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
*/
static DB_STATUS
dmv_reaipage(
DMVE_CB             *dmve,
DMP_TABLE_IO	    *tabio,
DMPP_PAGE	    *page,
DM0L_AI		    *log_rec,
DMPP_ACC_PLV	    *loc_plv,
i4             *err_code)
{
    LG_LSN		*log_lsn = &log_rec->ai_header.lsn; 
    LG_LSN		lsn; 
    DB_TAB_NAME		table_name;
    DB_OWN_NAME		table_owner;
    DB_STATUS		status;
    i4		mask;

    /*
    ** If recovery was found to be unneeded then we can just return.
    */
    if (page == NULL)
	return (E_DB_OK);

    /*
    ** Mutex the pages while updating them.
    */
    dm0p_mutex(tabio, (i4)0, dmve->dmve_lk_id, &page);

    /*
    ** Restore the contents of the page from the after image.
    */
    MEcopy((char *)&log_rec->ai_page,
		log_rec->ai_page_size, (char *)page);
    DMPP_VPT_SET_PAGE_STAT_MACRO(log_rec->ai_pg_type, page, DMPP_MODIFY);
    DMPP_VPT_SET_PAGE_LOG_ADDR_MACRO(log_rec->ai_pg_type, page, *log_lsn);
    
    /*
    ** Since this process may have changed the type of page, we must inform
    ** the buffer manager of its new page type.
    */
    mask = (DMPP_FHDR|DMPP_FMAP|DMPP_DATA|DMPP_LEAF|DMPP_INDEX|DMPP_CHAIN);
    dm0p_pagetype(tabio, page, dmve->dmve_log_id, 
	(DMPP_VPT_GET_PAGE_STAT_MACRO(log_rec->ai_pg_type, page) & mask));

    dm0p_unmutex(tabio, (i4)0, dmve->dmve_lk_id, &page);

    return(E_DB_OK);
}
