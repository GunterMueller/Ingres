/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <di.h>
#include    <me.h>
#include    <pc.h>
#include    <sl.h>
#include    <st.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <dudbms.h>
#include    <tr.h>
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
#include    <dm1b.h>
#include    <dm1cx.h>
#include    <dm1r.h>
#include    <dm1h.h>
#include    <dmve.h>
#include    <dmpp.h>
#include    <dm0l.h>
#include    <dm0p.h>
#include    <dmftrace.h>
#include    <dmpepcb.h>

/**
**
**  Name: DMVEBTPUT.C - The recovery of a btree put key operation.
**
**  Description:
**	This file contains the routine for the recovery of a 
**	put key operation to a btree index/leaf page.
**
**          dmve_btput - The recovery of a put key operation.
**
**  History:
**	14-dec-1992 (rogerk)
**	    Written for 6.5 recovery.
**	18-jan-1992 (rogerk)
**	    Add check in redo/undo routines for case when null page pointer is
**	    passed because recovery was found to be not needed.
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
**	    Changed dmve_btundo_check delete case to pass dummy tid pointer
**	    to dm1bxsearch when calling to find spot at which to restore the 
**	    deleted key.
**	21-jun-1993 (rogerk)
**	    Removed copy of key from btput CLR records.
**	    Added error messages.
**      21-jun-1993 (mikem)
**          su4_us5 port.  Added include of me.h to pick up new macro
**          definitions for MEcopy, MEfill, ...
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	26-jul-1993 (bryanp)
**	    Replace all uses of DM_LKID with LK_LKID.
**	26-jul-1993 (rogerk)
**	    Added tracing of page information during recovery.
**	    Added tracing of btundo_check logical processing.
**	    Include respective CL headers for all cl calls.
**	15-apr-1994 (chiku)
**	    Bug56702: returned logfull indication.
**	23-may-1994 (jnash)
** 	    Consistency check was checking wrong tid during index
**	    update undo.
**      20-may-95 (hanch04)
**          Added include dmtcb.h for ndmfapi
**	06-mar-1996 (stial01 for bryanp)
**	    Pass page_size to dm1cxhas_room.
**      06-mar-1996 (stial01)
**          Pass btp_page_size to dmve_trace_page_info
**	06-may-1996 (thaju02 & nanpr01)
**	    New page format support: change page header references to use
**	    macros.
**      03-june-1996 (stial01)
**          Added DMPP_TUPLE_INFO argument to dm1cxget() 
**      22-jul-1996 (ramra01 for bryanp)
**          Pass row_version to dmppxuncompress.
**	16-sep-1996 (canor01)
**	    Pass tuple buffer to dmppxuncompress.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Do not get page lock if row locking.
**          Check/adjust bid if row locking.
**          When calling dm1cxdel(), pass reclaim_space param
**     12-dec-96 (dilma04)
**          Corrected mistake in dmve_btput() which caused inconsistency 
**          between row_locking value and DM0L_ROW_LOCK flag after lock 
**          escalation.
**      04-feb-97 (stial01)
**          dmv_unbtree_put() Tuple headers are on LEAF and overflow (CHAIN) pgs
**          Tuple headers are not on INDEX pages.
**      27-feb-97 (stial01)
**          dmve_btput() If row locking entries can shift requiring btundo_check
**          Fix page without locking and then mutex it if we have IX page lock.
**          Error if DO/REDO and only IX page lock.
**          Clean ALL deleted leaf entries if X page lock.
**          dmv_unbtree_put() Space is not reclaimed if IX page lock held 
**          or deleting the last entry on leaf page with overflow chain.
**          Log key in CLR, needed for row locking
**          Renamed dmve_btundo_check -> dmve_bid check, as it is now called
**          for UNDO and REDO. Changed this routine for REDO. 
**      07-apr-97 (stial01)
**          dmv_unbtree_put() NonUnique primary btree (V2) dups span leaf pages,
**          not overflow chain. Remove code skipping reclaim of overflow key
**          dmve_bid_check() NonUnique primary btree (V2), dups can span leaf
**      21-apr-97 (stial01)
**          dmve_bid_check: Search mode is DM1C_FIND for unique keys, DM1C_TID 
**          for non-unique keys, since we re-use unique entries in 
**          dm1b_allocate, the tidp part of the leaf entry may not match.
**      30-apr-97 (stial01)
**          dmve_bid_check: When searching for key, don't overwrite logged tidp
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          Add lock_id argument to dm0p_lock_page and dm0p_unlock_page.
**      21-may-1997 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
**      29-may-1997 (stial01)
**          dmve_btput: REDO recovery: bid check only if redo necessary
**      18-jun-1997 (stial01)
**          dmve_btput() Request IX page lock if row locking.
**      30-jun-97 (dilma04)
**          Bug 83301. Set DM0P_INTENT_LK fix action if row locking.
**      30-jun-97 (stial01)
**          Use dm0p_lock_buf_macro to lock buffer to reduce mutex duration
**          Don't unfix tabio for bid check, changed args to dmve_bid_check()
**          dmve_bid_check() Use attr/key info on leaf pages
**          Added local routines to search page for key without rcb.
**	21-jul-1997 (canor01)
**	    Include <st.h>.
**      18-aug-1997 (stial01)
**          dmv_build_bt_info: fix assignment of recovery_action for CLR recs
**      29-jul-97 (stial01)
**          Validate/adjust bid if page size != DM_COMPAT_PGSIZE
**      15-aug-97 (stephenb)
**          add DML_SCB parameter to dm2t_open() call.
**	25-Aug-1997 (jenjo02)
**	    Added log_id parm to dm2t_ufx_tabio_cb() calls.
**      06-jan-98 (stial01)
**          If page size 2k, unfix pages,tbio before bid check (B87385) 
**          dmve_bid_check() LOCK pages we fix, unfix AND UNLOCK if LK_PHYSICAL
**      15-jan-98 (stial01)
**          dm1bxclean() Fixed dm1cxclean parameters, error handling
**      19-Jun-2002 (horda03) Bug 108074
**          Prevent FORCE_ABORT transaction filling TX log
**          file by not flushing SCONCUR pages unless we
**          really need to (i.e. the table isn't locked
**          exclusively).
**	23-Jun-1998 (jenjo02)
**	    Added lock_value parm to dm0p_lock_page() and dm0p_unlock_page().
**      07-jul-98 (stial01)
**          New update_mode if row locking, changed paramater to dmve_bid_check
**	14-aug-1998 (nanpr01)
**	    Error code is getting reset to E_DB_OK on error and not making
**	    the database inconsistent.
**	01-dec-1998 (nanpr01)
**	    Get rid of DMPP_TUPLE_INFO structure. Use DMPP_TUPLE_HDR instead.
**      07-Dec-1998 (stial01)
**          Added kperleaf to distinguish from kperpage (keys per index page)
**      09-feb-99 (stial01)
**          dmve_bid_check() Pass relcmptlvl = 0 to dm1cxkperpage
**      12-apr-1999 (stial01)
**          Different att/key info for LEAF vs. INDEX pages
**      12-nov-1999 (stial01)
**          Changes to how btree key info is stored in tcb
**      08-dec-1999 (stial01)
**          Undo change for bug#85156, dmpe routines now get INTENT table locks.
**      21-dec-1999 (stial01)
**          Defer leaf clean until insert, generalize for etabs (PHYS PG locks)
**	04-May-2000 (jenjo02)
**	    Pass blank-suppressed lengths of table/owner names to 
**	    dm0l_btput, eliminating need for dmve_name_unpack().
**	24-may-2000 (somsa01)
**	    Added LEVEL1_OPTIM for HP-UX 11.0. The compiler was generating
**	    incorrect code with '-O' such that, in dmve_btput(),
**	    "log_rec->btp_page_type != TCB_PG_V1" was failing, even
**	    though log_rec->btp_page_size was equal to DM_COMPAT_PGSIZE.
**      06-jul-2000 (stial01)
**          Changes to btree leaf/index klen in tcb (b102026) 
**      23-oct-2000 (stial01)
**          Variable Page Size - Variable Page Type support (SIR 102955)
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      01-feb-2001 (stial01)
**          Arg changes for page accessors (Variable Page Type SIR 102955)
**          Call dm1cxclean to remove deleted keys
**          Pass pg type to dmve_trace_page_info
**      01-may-2001 (stial01)
**          Fixed parameters to ule_format (B104618)
**      19-Jun-2002 (horda03) Bug 108074
**          Prevent FORCE_ABORT transaction filling TX log
**          file by not flushing SCONCUR pages unless we
**          really need to (i.e. the table isn't locked
**          exclusively).
**      02-oct-2002 (stial01)
**          dmv_search() Init adf_ucollation
**	08-nov-2002 (somsa01)
**	    Removed hpb_us5 from LEVEL1_OPTIM.
**      28-mar-2003 (stial01)
**          Child on DMPP_INDEX page may be > 512, so it must be logged
**          separately in DM0L_BTPUT, DM0L_BTDEL records (b109936)
**      02-jan-2004 (stial01)
**          Added dmv_reblob_put for redo recovery when noblobjournaling,
**          and nobloblogging.
**      14-jan-2004 (stial01)
**          Cleaned up 02-jan-2004 changes.
**	19-Jan-2004 (jenjo02)
**	    Added tidsize, partno for global indexes.
**      18-feb-2004 (stial01)
**          Fixed incorrect casting of length arguments.
**	6-Apr-2004 (schka24)
**	    Don't try to open a partition (master may not be open);
**	    open the master instead.
**      13-may-2004 (stial01)
**          Remove unecessary code for NOBLOBLOGGING redo recovery.
**      31-jan-05 (stial01)
**          Added flags arg to dm0p_mutex call(s).
**	21-feb-05 (inkdo01)
**	    Init Unicode normalization flag.
**      26-sep-05 (stial01)
**          dmve_bid_check() Fixed initialization of collID (b115266)
**      29-sep-2006 (stial01)
**          Support multiple RANGE entry formats: leafkey(old-fmt), indexkey(new-fmt)
**	06-Mar-2007 (jonj)
**	    Replace dm0p_cachefix_page() with dmve_cachefix_page()
**	    for Cluster REDO recovery.
*/

/*
** Forward Declarations
*/

static DB_STATUS	dmv_rebtree_put(
				DMVE_CB             *dmve,
				DMP_TABLE_IO	    *tabio,
				DM1B_INDEX	    *page,
				DM_TID		    *bid,
				DM0L_BTPUT	    *log_rec,
				DMPP_ACC_PLV 	    *plv,
				i4                  lock_type,
				i4                  lock_action,
				i4             *err_code);

static DB_STATUS	dmv_unbtree_put(
				DMVE_CB             *dmve,
				DMP_TABLE_IO	    *tabio,
				DM1B_INDEX	    *page,
				DM_TID		    *bid,
				DM0L_BTPUT	    *log_rec,
				DMPP_ACC_PLV 	    *plv,
				i4                  lock_type,
				i4                  lock_action,
				i4             *err_code);

static DB_STATUS        dmv_compare_key(
				i4		page_type,
				i4		page_size,
				DM1B_INDEX	*leaf,
				DMVE_CB             *dmve,
				i4             lineno,
				DB_ATTS             **RangeAtts,
				i4             RangeAttCount,
				DB_ATTS             **RangeKeys,
				DB_ATTS             **LeafKeyAtts,
				i4             key_count,
				i4             RangeKlen,
				char                *LeafKey,
				i4             compression_type,
				DMPP_ACC_TLV        *tlv,
				i4                 *compare,
				i4             *err_code);

static DB_STATUS dmv_search(
				i4		page_type,
				i4		page_size,
				DM1B_INDEX	*buffer,
				DMVE_CB             *dmve,
				i4             mode,
				DB_ATTS             **atts_array,
				i4             atts_count,
				DB_ATTS             **keyatts,
				i4             key_count,
				char                *key,
				i4             klen,
				i4             compression_type,
				DMPP_ACC_TLV        *tlv,
				DM_TID              *tid,
				DM_LINE_IDX         *pos,
				i4             *err_code);

static DB_STATUS dmv_binary_search(
				i4		page_type,
				i4		page_size,
				DM1B_INDEX	*buffer,
				DMVE_CB             *dmve,
				i4             direction,
				char                *key,
				DB_ATTS             **atts_array,
				i4             atts_count,
				DB_ATTS             **keyatts,
				i4             key_count,
				i4             klen,
				i4             compression_type,
				ADF_CB              *adf_cb,
				DMPP_ACC_TLV        *tlv,
				DM_LINE_IDX         *pos,
				i4             *err_code);

static VOID dmv_build_bt_info(
				DMVE_CB             *dmve,
				DB_TAB_NAME         *table_name,
				DB_OWN_NAME         *table_owner,
				i4                 *page_type,
				i4             *page_size,
				i4             *compression_type,
				bool                *bt_unique,
				i4		    *ixklen,
				i4             *recovery_action);

/*{
** Name: dmve_btput - The recovery of a put key operation.
**
** Description:
**	This function is used to do, redo and undo a put key
**	operation to a btree index/leaf page. This function adds or
**	deletes the key from the index depending on the recovery mode.
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
**	14-dec-1992 (rogerk)
**	    Written for 6.5 recovery.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**		Correct the message parameter lengths for E_DM9665.
**	26-jul-1993 (bryanp)
**	    Replace all uses of DM_LKID with LK_LKID.
**	11-sep-1997 (thaju02) bug#85156 - on recovery lock extension
**	    table with DM2T_S.  Attempting to lock table with DM2T_IX
**	    causes deadlock, dbms resource deadlock error during recovery,
**	    pass abort, rcp resource deadlock error during recovery,
**	    rcp marks database inconsistent.
**	06-may-1996 (thaju02 & nanpr01)
**	    New Page Format Support: 
**		Change page header references to use macros.
**		Pass page size to dm1c_getaccessors().
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Do not get page lock if row locking.
**          Check/adjust bid if row locking.
**     12-dec-96 (dilma04)
**          Corrected mistake which caused inconsistency between row_locking 
**          value and DM0L_ROW_LOCK flag after lock escalation.
**      27-feb-97 (stial01)
**          dmve_btput() If row locking entries can shift requiring btundo_check
**          Fix page without locking and then mutex it if we have IX page lock.
**          Error if DO/REDO and only IX page lock.
**          Clean ALL deleted leaf entries if X page lock.
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          Add lock_id argument to dm0p_lock_page and dm0p_unlock_page.
**      21-may-1997 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
**      29-may-1997 (stial01)
**          dmve_btput: REDO recovery: bid check only if redo necessary
**      18-jun-1997 (stial01)
**          dmve_btput() Request IX page lock if row locking.
**      30-jun-97 (dilma04)
**          Bug 83301. Set DM0P_INTENT_LK fix action if row locking.
**      30-jun-97 (stial01)
**          Use dm0p_lock_buf_macro to lock buffer to reduce mutex duration
**          Don't unfix tabio for bid check, changed args to dmve_bid_check()
**      29-jul-97 (stial01)
**          Validate/adjust bid if page size != DM_COMPAT_PGSIZE
**      06-jan-98 (stial01)
**          If page size 2k, unfix pages,tbio before bid check (B87385) 
**      19-Jun-2002 (horda03) Bug 108074
**          If the table is locked exclusively, then indicate that SCONCUR
**          pages don't need to be flushed immediately.
**      23-feb-2004 (thaju02) Bug 111470 INGSRV2635
**          For rollforwarddb -b option, do not compare the LSN's on the
**          page to that of the log record.
**	01-Dec-2004 (jenjo02)
**	    Tweak to 108074; call dmve_bid_check with table's 
**	    grant_mode rather than tbl_lock_mode to ensure that
**	    bid-check pages WUNFIX bits are also properly reset.
**	13-Apr-2006 (jenjo02)
**	    CLUSTERED Btree leaf pages don't have attributes - check
**	    that before calling dmve_bid_check.
**	13-Jun-2006 (jenjo02)
**	    Puts of leaf entries may be up to
**	    DM1B_MAXLEAFLEN, not DM1B_KEYLENGTH, bytes.
**	01-Dec-2006 (kiria01) b117225
**	    Initialise the lockid parameters that will be passed to LKrequest
**	    to avoid random implicit lock conversions.
**	04-Jan-2007 (kibro01) b117125
**	    Swapped round order of checks in the IF condition so that 
**	    a 0-attribute count in a page only drops the page if the 
**	    page is not from a partition
*/
DB_STATUS
dmve_btput(
DMVE_CB		*dmve)
{
    DM0L_BTPUT		*log_rec = (DM0L_BTPUT *)dmve->dmve_log_rec;
    LG_LSN		*log_lsn = &log_rec->btp_header.lsn; 
    DMP_DCB		*dcb = dmve->dmve_dcb_ptr;
    DMP_TABLE_IO	*tbio = NULL;
    DM1B_INDEX		*page = NULL;
    DMPP_PAGE		*page_ptr = NULL;
    DB_STATUS		status = E_DB_OK;
    DB_STATUS		tmp_status;
    DMPP_ACC_PLV	*loc_plv;
    DMPP_ACC_TLV	*loc_tlv;
    LK_LKID		lockid;
    DM_TID		leaf_bid;
    char		*insert_key;
    i4		lock_action;
    i4		grant_mode;
    i4		recovery_action;
    i4		error;
    i4		loc_error;
    i4		tbl_lock_mode;
    bool		physical_page_lock = FALSE;
    bool		bid_check_done = FALSE;
    bool                bid_check = FALSE;
    bool                buf_locked = FALSE;
    bool                index_update;
    i4             opflag;
    i4             lock_type = LK_TABLE;
    i4             fix_action = 0;

    MEfill(sizeof(LK_LKID), 0, &lockid);

    /*               
    ** Store BID of insert into a local variable.  The insert BID may
    ** be modified in undo recovery by the dmve_btunto_check routine.
    */
    leaf_bid = log_rec->btp_bid;

    /*
    ** Call appropriate recovery action depending on the recovery type
    ** and record flags.  CLR actions are always executed as an UNDO
    ** operation.
    */
    recovery_action = dmve->dmve_action;
    if (log_rec->btp_header.flags & DM0L_CLR)
	recovery_action = DMVE_UNDO;

    for (;;)
    {
	/*
	** Consistency Check:  check for illegal log records.
	*/
	if ((log_rec->btp_header.type != DM0LBTPUT) ||
	    (log_rec->btp_header.length != 
		(sizeof(DM0L_BTPUT) - 
			(DM1B_MAXLEAFLEN - log_rec->btp_key_size) -
			(DB_MAXNAME - log_rec->btp_tab_size) -
			(DB_MAXNAME - log_rec->btp_own_size))))
	{
	    error = E_DM9601_DMVE_BAD_PARAMETER;
	    break;
	}

	/*
	** Check for partial recovery during UNDO operations.
	** We cannot bypass recovery of index updates even if the page
	** is on a location which is not being recovered.  This is because
	** the logged page number may not be the page that actually needs
	** the UNDO action!!!  If the page which was originally updated
	** has been since SPLIT, then the logged key may have moved to a
	** new index/leaf page.  It is that new page to which this undo
	** action must be applied.  And that new page may be on a location
	** being recovered even if the original page is not.
	**
	** If recovery is being bypassed on the entire table then no recovery
	** needs to be done.
	*/
	if ((dmve->dmve_action == DMVE_UNDO) &&
	    (dmve_location_check(dmve, (i4)log_rec->btp_cnf_loc_id) == FALSE))
	{
            ule_format(E_DM9668_TABLE_NOT_RECOVERED, (CL_ERR_DESC *)0,
                ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0,
                &loc_error, 2, 0, log_rec->btp_tbl_id.db_tab_base,
                0, log_rec->btp_tbl_id.db_tab_index);
            error = E_DM9667_NOPARTIAL_RECOVERY;
	    break;
	}

	/*
	** Get handle to a tableio control block with which to read
	** and write pages needed during recovery.
	**
	** Warning return indicates that no tableio control block was
	** built because no recovery is needed on any of the locations 
	** described in this log record.  Note that check above prevents
	** this case from occurring during UNDO recovery.
	*/
	status = dmve_fix_tabio(dmve, &log_rec->btp_tbl_id, &tbio, &error);
	if ((status == E_DB_WARN) && (error == W_DM9660_DMVE_TABLE_OFFLINE))
	    return (E_DB_OK);

	if (status != E_DB_OK)
	    break;

	tbl_lock_mode = grant_mode = DM2T_IX;

	/*
	** Get page accessors for page recovery actions.
	*/
	dm1c_getaccessors(TCB_C_DEFAULT, log_rec->btp_pg_type, 
		log_rec->btp_page_size, &loc_plv, &loc_tlv);

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
	    status = dm2t_lock_table(dcb, &log_rec->btp_tbl_id, tbl_lock_mode,
		dmve->dmve_lk_id, (i4)0, &grant_mode, &lockid, &error);
	    if (status != E_DB_OK)
		break;

	    if (grant_mode != DM2T_X) 
	    {
		/*
		** Page lock required.  If this is a system catalog page
		** or a non-leaf index page then we need to request a 
		** physical lock (and release it later). We do not lock
                ** the page if row locking.
		*/
		lock_action = LK_LOGICAL;
		if (log_rec->btp_header.flags & DM0L_PHYS_LOCK)
		    lock_action = LK_PHYSICAL;
		 
		if (log_rec->btp_header.flags & DM0L_ROW_LOCK)
		    lock_type = LK_ROW;
		else
		    lock_type = LK_PAGE;

		status = dm0p_lock_page(dmve->dmve_lk_id, dcb, 
		    &log_rec->btp_tbl_id, leaf_bid.tid_tid.tid_page, 
		    LK_PAGE,
		    lock_type == LK_PAGE ? LK_X : LK_IX,
		    lock_action, (i4)0, 
		    tbio->tbio_relid, &dmve->dmve_tran_id, (LK_LKID *)0, 
		    (i4 *)0, (LK_VALUE *)0, &error);

		if (status != E_DB_OK)
		    break;

		if (lock_action == LK_PHYSICAL)
		    physical_page_lock = TRUE;
	    }
            else
               fix_action |= DM0P_TABLE_LOCKED_X;
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
				    tbio, leaf_bid.tid_tid.tid_page,
				    fix_action, loc_plv,
				    &page_ptr, &error);
	if (status != E_DB_OK)
	    break;
	page = (DM1B_INDEX *)page_ptr;

        if (lock_type == LK_ROW)
	{
	    dm0p_lock_buf_macro(tbio, dmve->dmve_lk_id, &page_ptr);
	    buf_locked = TRUE;
	}

	/*
	** Dump debug trace info about pages if such tracing is configured.
	*/
	if (DMZ_ASY_MACRO(15))
	    dmve_trace_page_info(log_rec->btp_pg_type, log_rec->btp_page_size,
		(DMPP_PAGE *) page, loc_plv, "Page");

	index_update = ((DM1B_VPT_GET_PAGE_STAT_MACRO(log_rec->btp_pg_type, 
		page) & DMPP_INDEX) != 0);

	if (dmve->dmve_action == DMVE_UNDO)
	{
	    /*
	    ** UNDO Recovery SPLIT check:
	    **
	    ** If doing UNDO recovery, we need to verify that this is the 
	    ** correct page from which to undo the put operation.
	    ** If the row in question has been moved to a different page by
	    ** a subsequent SPLIT operation, then we have to find that new page
	    ** in order to perform the undo.
	    **
	    ** The bid_check will search for the correct BID to which to do
	    ** the recovery action and return with that value in 'leafbid'.
	    */
	   if ((LSN_GT(DM1B_VPT_ADDR_BT_SPLIT_LSN_MACRO(log_rec->btp_pg_type,
		page), log_lsn)) &&
	    (DM1B_VPT_GET_PAGE_STAT_MACRO(log_rec->btp_pg_type, page) & 
		DMPP_LEAF))
	    {
		bid_check = TRUE;
	    }

	    /*
	    ** UNDO: if page type != TCB_PG_V1, validate/adjust bid
	    */
	    if ( (log_rec->btp_pg_type != TCB_PG_V1)
		&& (index_update == FALSE) )
	    {
		bid_check = TRUE;
	    }
	}
	else
	{
	    /*
	    ** REDO recovery, the logged page is correct, but
	    ** due to page cleans the logged bid may be incorrect.
	    ** Only do the bid check if we need to REDO this update
	    */
	    if ( (log_rec->btp_pg_type != TCB_PG_V1) 
		&& (index_update == FALSE)
		&&  (LSN_LT(
		    DM1B_VPT_ADDR_PAGE_LOG_ADDR_MACRO(log_rec->btp_pg_type, page), 
		    log_lsn)))
	    {
		bid_check = TRUE;
	    }
	}

	if (bid_check && !bid_check_done)
	{
	    /* This check should be 
	    **	("Table is V1" or "This page has no attributes")
	    **	AND
	    **	"Table is not a partition"
	    ** Otherwise bug 117125 goes into here and loses its tbio
	    ** setting, but the dmve_bid_check routine has no way of
	    ** reestablishing it since it works purely on the tab_index check
	    ** b117125 (kibro01)
	    */
	    if ( (log_rec->btp_pg_type == TCB_PG_V1 ||
		DM1B_VPT_GET_BT_ATTCNT(log_rec->btp_pg_type, page) == 0 )
	           && log_rec->btp_tbl_id.db_tab_index >= 0)
	    {
		/*
		** Release our current page and the tbio
		** We will need to perform logical table open to check bid
		** (although not if we're operating on a partition).
		*/
		if (lock_type == LK_ROW)
		    dm0p_unmutex(tbio, (i4)0, dmve->dmve_lk_id, (DMPP_PAGE **)&page);

		status = dm0p_uncache_fix(tbio, DM0P_UNFIX, dmve->dmve_lk_id, 
		    dmve->dmve_log_id, &dmve->dmve_tran_id, &page_ptr, &error);
		if (status != E_DB_OK)
		    break;

		if (physical_page_lock)
		{
		    status = dm0p_unlock_page(dmve->dmve_lk_id, dcb, 
			&log_rec->btp_tbl_id, leaf_bid.tid_tid.tid_page,
			LK_PAGE, tbio->tbio_relid, &dmve->dmve_tran_id, 
			(LK_LKID *)0, (LK_VALUE *)0, &error);
		    if (status != E_DB_OK)
			break;

		    physical_page_lock = FALSE;
		}

		status = dm2t_ufx_tabio_cb(tbio, DM2T_VERIFY, 
		    dmve->dmve_lk_id, dmve->dmve_log_id, &error);
		if (status != E_DB_OK)
		    break;
		tbio = NULL;
	    }

	    insert_key = &log_rec->btp_vbuf[log_rec->btp_tab_size + 
							log_rec->btp_own_size];
	    if (recovery_action == DMVE_UNDO)
		opflag = DMVE_FINDKEY;
	    else
		opflag = DMVE_FINDPOS;

	    /* Send Table lock's grant mode, not tbl_lock_mode, to bid_check */
	    status = dmve_bid_check(dmve, opflag,
		&log_rec->btp_tbl_id, &log_rec->btp_bid, 
		&log_rec->btp_tid, insert_key, log_rec->btp_key_size,
		tbio, grant_mode, lock_type, lock_action,
		&buf_locked, &leaf_bid, &page_ptr, &error);
	    if (status != E_DB_OK)
		break;

	    bid_check_done = TRUE;

	    /*
	    ** If we had to unfix our tbio for the bid check, 
	    ** jump back to the start of the routine and start this recovery
	    ** process all over again using the BID returned from the
	    ** bid_check routine.
	    */
	    if (tbio == NULL)
		continue;

            page = (DM1B_INDEX *)page_ptr;
	}


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
		DM1B_VPT_ADDR_PAGE_LOG_ADDR_MACRO(log_rec->btp_pg_type, page), 
		log_lsn) && ((dmve->dmve_flags & DMVE_ROLLDB_BOPT) == 0))
	    {
		if (dmve->dmve_action == DMVE_DO)
		{
		    ule_format(E_DM9665_PAGE_OUT_OF_DATE, (CL_ERR_DESC *)0, 
			ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, 
			&loc_error, 8, 
			sizeof(*tbio->tbio_relid), tbio->tbio_relid,
			sizeof(*tbio->tbio_relowner), tbio->tbio_relowner, 0, 
			DM1B_VPT_GET_PAGE_PAGE_MACRO(log_rec->btp_pg_type, page),
			0, DM1B_VPT_GET_PAGE_STAT_MACRO(log_rec->btp_pg_type, 
			    page),
			0, DM1B_VPT_GET_LOG_ADDR_HIGH_MACRO(log_rec->btp_pg_type,
			    page), 
			0, DM1B_VPT_GET_LOG_ADDR_LOW_MACRO(log_rec->btp_pg_type,
			    page),
			0, log_lsn->lsn_high, 0, log_lsn->lsn_low);
		}
		page = NULL;
	    }
	    break;

	case DMVE_UNDO:
            if (LSN_LT(
		DM1B_VPT_ADDR_PAGE_LOG_ADDR_MACRO(log_rec->btp_pg_type, page), 
		log_lsn))
	    {
		ule_format(E_DM9665_PAGE_OUT_OF_DATE, (CL_ERR_DESC *)0, 
		    ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, 
		    &loc_error, 8, 
		    sizeof(*tbio->tbio_relid), tbio->tbio_relid,
		    sizeof(*tbio->tbio_relowner), tbio->tbio_relowner,
		    0, DM1B_VPT_GET_PAGE_PAGE_MACRO(log_rec->btp_pg_type, page),
		    0, DM1B_VPT_GET_PAGE_STAT_MACRO(log_rec->btp_pg_type, page),
		    0, DM1B_VPT_GET_LOG_ADDR_HIGH_MACRO(log_rec->btp_pg_type, 
			page), 
		    0, DM1B_VPT_GET_LOG_ADDR_LOW_MACRO(log_rec->btp_pg_type,
			page),
		    0, log_lsn->lsn_high, 0, log_lsn->lsn_low);
		error = E_DM9666_PAGE_LSN_MISMATCH;
		status = E_DB_ERROR;
	    }
	    break;
	}
	if (status != E_DB_OK)
	    break;

	switch (recovery_action)
	{
	case DMVE_DO:
	case DMVE_REDO:
	    status = dmv_rebtree_put(dmve, tbio, page, &leaf_bid, log_rec, 
		loc_plv, lock_type, lock_action, &error);
	    break;

	case DMVE_UNDO:
	    status = dmv_unbtree_put(dmve, tbio, page, &leaf_bid, log_rec, 
		loc_plv, lock_type, lock_action, &error);
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
    if (buf_locked == TRUE)
    {
	dm0p_unlock_buf_macro(tbio, dmve->dmve_lk_id, &page_ptr);
	buf_locked = FALSE;
    }

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

    if (physical_page_lock)
    {
	tmp_status = dm0p_unlock_page(dmve->dmve_lk_id, dcb, 
	    &log_rec->btp_tbl_id, leaf_bid.tid_tid.tid_page, 
	    LK_PAGE, tbio->tbio_relid, &dmve->dmve_tran_id, (LK_LKID *)0, 
            (LK_VALUE *)0, &loc_error);
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
        dmve->dmve_error.err_code = E_DM964E_DMVE_BTREE_PUT;

    return(status);
}


/*{
** Name: dmv_rebtree_put - Redo the Put of a btree key 
**
** Description:
**      This function adds a key to a btree index for the recovery of a
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
**	14-dec-1992 (rogerk)
**	    Written for 6.5 recovery.
**	18-jan-1992 (rogerk)
**	    Add check in redo routine for case when null page pointer is
**	    passed because redo was found to be not needed.
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
dmv_rebtree_put(
DMVE_CB             *dmve,
DMP_TABLE_IO	    *tabio,
DM1B_INDEX	    *page,
DM_TID		    *bid,
DM0L_BTPUT	    *log_rec,
DMPP_ACC_PLV 	    *plv,
i4                  lock_type,
i4                  lock_action,
i4             *err_code)
{
    LG_LSN		*page_lsn = 
		DM1B_VPT_ADDR_PAGE_LOG_ADDR_MACRO(log_rec->btp_pg_type, page);
    LG_LSN		*log_lsn = &log_rec->btp_header.lsn; 
    DB_STATUS		status = E_DB_OK;
    DM_LINE_IDX		child;
    DM_TID		temptid;
    i4			temppartno;
    char		*key;
    bool		index_update;
    i2			TidSize;

    /*
    ** If there is nothing to recover, just return.
    */
    if (page == NULL)
	return (E_DB_OK);

    key = &log_rec->btp_vbuf[log_rec->btp_tab_size + log_rec->btp_own_size];

    index_update = ((DM1B_VPT_GET_PAGE_STAT_MACRO(log_rec->btp_pg_type, page) & 
	DMPP_INDEX) != 0);

    if (index_update)
	child = log_rec->btp_bid_child;
    else
	child = bid->tid_tid.tid_line;

    /* Extract the TID size from the page */
    TidSize = DM1B_VPT_GET_BT_TIDSZ_MACRO(log_rec->btp_pg_type, page);
    
    /*
    ** We assume here that there is sufficient space on the page.  If not,
    ** then the dm1cx calls below will catch the error.
    ** The "repeating history" rules of this recorvery should guarantee
    ** that the row will fit since it must have fit originally.  If there
    ** is no room, it will be a fatal recovery error.
    **
    ** We trust that the indicated BID position at which to re-insert the
    ** key is correct.  We have no way to verify this since we don't have
    ** the logical context required to do key comparisons.
    */

    /*
    ** Mutex the page while updating it.
    */
    dm0p_mutex(tabio, (i4)0, dmve->dmve_lk_id, (DMPP_PAGE **)&page);

    /*
    ** If insert is to a non-leaf index page, then save the tidp at the
    ** position to which we are about to insert the new key.  This tidp
    ** will be restored following the dm1cxinsert call below.  Index page
    ** updates actually update the tid of the following position, not
    ** position at which the key is put.
    */
    if (index_update)
    {
        dm1cxtget(log_rec->btp_pg_type, log_rec->btp_page_size, page, child, 
			&temptid, &temppartno); 
    }

    /*
    ** Redo the insert operation.
    */
    for (;;)
    {
	/*
	** Reallocate space on the page for the key,tid pair.
	** During REDO we always need to do the allocate (even for CLRs)
	**    When finding insert position, we cleaned all deleted tuples
	**    (Also during REDO delete we reclaim space immediately)
	*/
	status = dm1cxallocate(log_rec->btp_pg_type, log_rec->btp_page_size,
			page, DM1C_DIRECT, log_rec->btp_cmp_type, 
			&dmve->dmve_tran_id, (i4)0, child,
			log_rec->btp_key_size + TidSize);
	if (status != E_DB_OK)
	{
	    dm1cxlog_error(E_DM93E0_BAD_INDEX_ALLOC, (DMP_RCB *)0, page, 
			   log_rec->btp_pg_type, log_rec->btp_page_size, child);
	    break;
	}

	/*
	** Reinsert the key,tid values.
	*/
	status = dm1cxput(log_rec->btp_pg_type, log_rec->btp_page_size, page,
			log_rec->btp_cmp_type, DM1C_DIRECT, 
			&dmve->dmve_tran_id, (i4)0, child,
			key, log_rec->btp_key_size, &log_rec->btp_tid,
			log_rec->btp_partno);
	if (status != E_DB_OK)
	{
	    dm1cxlog_error(E_DM93E4_BAD_INDEX_PUT, (DMP_RCB *)0, page, 
			   log_rec->btp_pg_type, log_rec->btp_page_size, child);
	    break;
	}

	/*
	** If the insert is to a non-leaf index page, then the logged tid
	** value must actually be inserted to the position after the one
	** to which we just put the key.  Replace the old tidp from that
	** position and insert the new one to the next entry.
	*/
	if (index_update)
	{
	    status = dm1cxtput(log_rec->btp_pg_type, log_rec->btp_page_size, 
				page, child + 1, &log_rec->btp_tid,
				log_rec->btp_partno);
	    if (status != E_DB_OK)
	    {
		dm1cxlog_error(E_DM93EB_BAD_INDEX_TPUT, (DMP_RCB *)0, page,
			log_rec->btp_pg_type, log_rec->btp_page_size, child + 1);
		break;
	    }
	    status = dm1cxtput(log_rec->btp_pg_type, log_rec->btp_page_size, 
				page, child, &temptid, temppartno);
	    if (status != E_DB_OK)
	    {
		dm1cxlog_error(E_DM93EB_BAD_INDEX_TPUT, (DMP_RCB *)0, page,
			log_rec->btp_pg_type, log_rec->btp_page_size, child);
		break;
	    }
	}

	break;
    }

    /*
    ** Write the LSN of the Put log record to the updated page.
    */
    DM1B_VPT_SET_PAGE_LOG_ADDR_MACRO(log_rec->btp_pg_type, page, *log_lsn);

    DM1B_VPT_SET_PAGE_STAT_MACRO(log_rec->btp_pg_type, page, DMPP_MODIFY);
    dm0p_unmutex(tabio, (i4)0, dmve->dmve_lk_id, (DMPP_PAGE **)&page);
    
    if (status != E_DB_OK)
    {
	*err_code = E_DM9650_REDO_BTREE_PUT;
	return(E_DB_ERROR);
    }

    return(E_DB_OK);
}

/*{
** Name: dmv_unbtree_put - UNDO of a put key operation.
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
**	14-dec-1992 (rogerk)
**	    Written for 6.5 recovery.
**	18-jan-1992 (rogerk)
**	    Add check in redo/undo routines for case when null page pointer is
**	    passed because recovery was found to be not needed.
**	15-mar-1993 (jnash)
**	    Check dmve->dmve_logging to determine if logging required.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**	21-jun-1993 (rogerk)
**	    Removed copy of key from btput CLR records.  Changed consistency
**	    checks to not try to verify key contents if the key was not logged.
**	    Added new error messages.
**	15-apr-1994 (chiku)
**	    Bug56702: returned logfull indication.
**	23-may-1994 (jnash)
** 	    Consistency check was checking wrong tid during index
**	    update undo.
**	06-may-1996 (thaju02 & nanpr01)
**	    New page format support: change page header references to use
**	    macros.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          When calling dm1cxdel(), pass reclaim_space param
**      04-feb-97 (stial01)
**          Tuple headers are on LEAF and overflow (CHAIN) pages
**          Tuple headers are not on INDEX pages.
**      27-feb-97 (stial01)
**          dmv_unbtree_put() Space is not reclaimed if IX page lock held 
**          or deleting the last entry on leaf page with overflow chain.
**          Log key in CLR, needed for row locking
**          Init flag param for dm1cxdel()
**      07-apr-97 (stial01)
**          dmv_unbtree_put() NonUnique primary btree (V2) dups span leaf pages,
**          not overflow chain. Remove code skipping reclaim of overflow key
**      21-may-1997 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
**      21-apr-1998 (stial01)
**          dmv_unbtree_put() Removed setting unecessary DM1CX_K_ROW_LOCKING.
**	13-Jun-2006 (jenjo02)
**	    Clustered TIDs may mismatch, and that's OK as long as the
**	    keys match.
*/
static DB_STATUS
dmv_unbtree_put(
DMVE_CB             *dmve,
DMP_TABLE_IO	    *tabio,
DM1B_INDEX	    *page,
DM_TID		    *bid,
DM0L_BTPUT	    *log_rec,
DMPP_ACC_PLV 	    *plv,
i4                  lock_type,
i4                  lock_action,
i4             *err_code)
{
    LG_LSN		*page_lsn = 
		DM1B_VPT_ADDR_PAGE_LOG_ADDR_MACRO(log_rec->btp_pg_type, page);
    LG_LSN		*log_lsn = &log_rec->btp_header.lsn; 
    DB_STATUS		status = E_DB_OK;
    DM_LINE_IDX		child;
    DM_TID		deltid;
    i4			delpartno;
    LG_LSN		lsn;
    char		*key;
    char		*key_ptr;
    i4		key_len;
    i4		flags;
    i4		loc_id;
    i4		loc_config_id;
    bool		index_update;
    bool                is_leaf;
    bool                Clustered;
    i4		reclaim_space;
    bool                mutex_done;
    i4             dmcx_flag = 0;
    i4             update_mode;

    /*
    ** If there is nothing to recover, just return.
    */
    if (page == NULL)
	return (E_DB_OK);

    key = &log_rec->btp_vbuf[log_rec->btp_tab_size + log_rec->btp_own_size];

    index_update = ((DM1B_VPT_GET_PAGE_STAT_MACRO(log_rec->btp_pg_type, page) & 
	DMPP_INDEX) != 0);
    is_leaf = ((DM1B_VPT_GET_PAGE_STAT_MACRO(log_rec->btp_pg_type, page) &
	DMPP_LEAF) != 0);
    Clustered = ((DM1B_VPT_GET_PAGE_STAT_MACRO(log_rec->btp_pg_type, page) &
	DMPP_CLUSTERED) != 0);

    if (index_update)
	child = log_rec->btp_bid_child;
    else
	child = bid->tid_tid.tid_line;

    /*
    ** Get information on the location to which the update is being made.
    */
    loc_id = DM2F_LOCID_MACRO(tabio->tbio_loc_count, 
	(i4) DM1B_VPT_GET_PAGE_PAGE_MACRO(log_rec->btp_pg_type, page));
    loc_config_id = tabio->tbio_location_array[loc_id].loc_config_id;

    /*
    ** Lookup the key/tid pair to delete.
    */
    dm1cxrecptr(log_rec->btp_pg_type, log_rec->btp_page_size, page, child, &key_ptr);
    dm1cxtget(log_rec->btp_pg_type, log_rec->btp_page_size, page, child, 
		&deltid, &delpartno);

    /* 
    ** For consistency check, check the deleted tid at the new position.
    */
    if (index_update)
	dm1cxtget(log_rec->btp_pg_type, log_rec->btp_page_size, page, 
		child + 1, &deltid, &delpartno);

    /*
    ** Consistency Checks:
    **
    ** Verify the deltid value. 
    ** Verify the that the key matches the one we are about to delete.
    **    As of release OpenIngres 2.0, we log the key value in CLRs as well,
    **    since we need it if row locking.
    ** 
    ** If this is a non-leaf page update, verify that there is an entry at
    ** the following position in which to update the TID value.
    ** XXXX TEST ME XXXX
    */

    /*
    ** We can only validate the key size on compressed tables; otherwise
    ** we must assume that the logged value was the correct table key length.
    */
    key_len = log_rec->btp_key_size;
    if (log_rec->btp_cmp_type != DM1CX_UNCOMPRESSED)
    {
	dm1cx_klen(log_rec->btp_pg_type, log_rec->btp_page_size, page, 
		child, &key_len);
    }

    /*
    ** Compare the key,tid pair we are about to delete with the one we logged
    ** to make sure they are identical.
    **
    ** If the keys don't match then we make an assumption here that the 
    ** mismatch is most likely due to this check being wrong (we have 
    ** garbage at the end of the tuple buffer or we allowed some sort of 
    ** non-logged update to the row) and we continue with the operation
    ** after logging the unexpected condition.
    */
    if (((log_rec->btp_key_size != 0) && 
	    ((log_rec->btp_key_size != key_len) ||
	     (MEcmp((PTR)key, (PTR)key_ptr, key_len) != 0))) ||
	(log_rec->btp_tid.tid_i4 != deltid.tid_i4 && !Clustered))
    {
	ule_format(E_DM966A_DMVE_KEY_MISMATCH, (CL_ERR_DESC *)0, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, err_code, 8, 
	    sizeof(DB_DB_NAME), tabio->tbio_dbname->db_db_name,
	    log_rec->btp_tab_size, &log_rec->btp_vbuf[0],
	    log_rec->btp_own_size, &log_rec->btp_vbuf[log_rec->btp_tab_size],
	    0, bid->tid_tid.tid_page, 0, bid->tid_tid.tid_line,
	    5, (index_update ? "INDEX" : "LEAF "),
	    0, log_rec->btp_bid.tid_tid.tid_page,
	    0, log_rec->btp_bid.tid_tid.tid_line);
	ule_format(E_DM966B_DMVE_KEY_MISMATCH, (CL_ERR_DESC *)0, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, err_code, 7, 
	    0, key_len, 0, log_rec->btp_key_size,
	    0, deltid.tid_tid.tid_page, 0, deltid.tid_tid.tid_line,
	    0, log_rec->btp_tid.tid_tid.tid_page,
	    0, log_rec->btp_tid.tid_tid.tid_line,
	    0, dmve->dmve_action);
	dmd_log(1, log_rec, 4096);
	ule_format(E_DM964F_UNDO_BTREE_PUT, (CL_ERR_DESC *)0, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, err_code, 0); 
    }

    /* 
    ** Mutex the page.  This must be done prior to the log write.
    */
    dm0p_mutex(tabio, (i4)0, dmve->dmve_lk_id, (DMPP_PAGE **)&page);

    /*
    ** During UNDO (rollback) don't reclaim space if we have IX lock on page
    ** We may need the space/bid if this transaction deleted/inserted the same
    ** key.
    */ 
    update_mode = DM1C_DIRECT;
    if (!index_update && log_rec->btp_pg_type != TCB_PG_V1 &&
	(lock_type == LK_ROW || 
	(lock_type == LK_PAGE && (log_rec->btp_header.flags & DM0L_PHYS_LOCK))))
    {
	reclaim_space = FALSE;
	update_mode |= DM1C_ROWLK;
    }
    else
	reclaim_space = TRUE;

    if (reclaim_space == TRUE)
	dmcx_flag |= DM1CX_RECLAIM;

    /*
    ** Check direction of recovery operation:
    **
    **     If this is a normal Undo, then we log the CLR for the operation
    **     and write the LSN of this CLR onto the newly updated page (unless
    **     dmve_logging is turned off - in which case the rollback is not
    **     logged and the page lsn is unchanged).
    **
    **     As of release OpenIngres 2.0, we log the key value in CLRs as well,
    **     since we need it if row locking.
    **
    **     If the record being processed is itself a CLR, then we are REDOing
    **     an update made during rollback processing.  Updates are not relogged
    **     in redo processing and the LSN is moved forward to the LSN value of
    **     of the original update.
    **
    ** The CLR for a BTPUT need not contain the key, just the bid.
    */
    if ((log_rec->btp_header.flags & DM0L_CLR) == 0)
    {
	if (dmve->dmve_logging)
	{
	    flags = (log_rec->btp_header.flags | DM0L_CLR);

	    status = dm0l_btput(dmve->dmve_log_id, flags,
		&log_rec->btp_tbl_id, 
		(DB_TAB_NAME*)&log_rec->btp_vbuf[0], log_rec->btp_tab_size, 
		(DB_OWN_NAME*)&log_rec->btp_vbuf[log_rec->btp_tab_size], log_rec->btp_own_size, 
		log_rec->btp_pg_type, log_rec->btp_page_size,
		log_rec->btp_cmp_type, 
		log_rec->btp_loc_cnt, loc_config_id,
		bid, child, &log_rec->btp_tid, log_rec->btp_key_size, key,
		log_lsn, &lsn, log_rec->btp_partno, 
		log_rec->btp_ixklen, err_code);
	    if (status != E_DB_OK)
	    {
		dm0p_unmutex(tabio, (i4)0, dmve->dmve_lk_id, (DMPP_PAGE **)&page);
		/*
		 * Bug56702: returned logfull indication.
		 */
		dmve->dmve_error.err_data = *err_code;
		ule_format(*err_code, (CL_ERR_DESC *)0, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 0); 
		*err_code = E_DM964F_UNDO_BTREE_PUT;
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
	DM1B_VPT_SET_PAGE_LOG_ADDR_MACRO(log_rec->btp_pg_type, page, lsn);

    /*
    ** Undo the insert operation.
    */
    for (;;)
    {
	/*
	** If the insert was to a non-leaf index page, then we must actually
	** delete the Key at the logged position, but the tid of the following
	** entry.  We take the tid at the position we are about to delete
	** and write it into the next entry.
	*/
	if (index_update)
	{
	    dm1cxtget(log_rec->btp_pg_type, log_rec->btp_page_size,
		page, child, &deltid, &delpartno);
	    status = dm1cxtput(log_rec->btp_pg_type, log_rec->btp_page_size, 
		page, child + 1, &deltid, delpartno);
	    if (status != E_DB_OK)
	    {
		dm1cxlog_error(E_DM93EB_BAD_INDEX_TPUT, (DMP_RCB *)0, page, 
			log_rec->btp_pg_type, log_rec->btp_page_size, child + 1);
		break;
	    }
	}

	status = dm1cxdel(log_rec->btp_pg_type, log_rec->btp_page_size, page,
			update_mode, log_rec->btp_cmp_type,
			&dmve->dmve_tran_id, (i4)0, dmcx_flag, child);
	if (status != E_DB_OK)
	{
	    dm1cxlog_error(E_DM93E2_BAD_INDEX_DEL, (DMP_RCB *)0, page, 
			   log_rec->btp_pg_type, log_rec->btp_page_size, child);
	    break;
	}

	break;
    }

    DM1B_VPT_SET_PAGE_STAT_MACRO(log_rec->btp_pg_type, page, DMPP_MODIFY);
    dm0p_unmutex(tabio, (i4)0, dmve->dmve_lk_id, (DMPP_PAGE **)&page);
    
    if (status != E_DB_OK)
    {
	*err_code = E_DM964F_UNDO_BTREE_PUT;
	return(E_DB_ERROR);
    }

    return(E_DB_OK);
}

/*{
** Name: dmve_bid_check - Check that correct page is fixed for Btree Recovery.
**
** Description:
**
** Inputs:
**      dmve				Pointer to dmve control block.
**	opflags
**	tableid				Table Id.
**	log_bid				Original Bid of key:
**					  - if put undo, bid of key to delete
**					  - if del undo, bid at which to insert
**					  - if free undo, bid of previous page
**	log_tid				Tid of data page row
**	log_key				Key value
**	log_key_size			Key length
**
** Outputs:
**	found_bid			May be updated to show new bid for key:
**					  - if put undo, bid of key to delete
**					  - if del undo, bid at which to insert
**					  - if free undo, bid of previous page
**	error
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
**	14-dec-1992 (rogerk)
**	    Written for 6.5 recovery.
**	18-jan-1993 (rogerk)
**	    Fix argument to ule_format being passed by value, not ref.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**	21-jun-1993 (rogerk)
**	    Changed undo delete case to pass dummy tid pointer to dm1bxsearch
**	    when calling to find spot at which to restore the deleted key.
**	    We can't pass the "log_tid" argument pointer because its value
**	    is overwritten by the search routine.
**	    Add error messages.
**	26-jul-1993 (rogerk)
**	    Added tracing of btundo_check logical processing.
**	06-mar-1996 (stial01 for bryanp)
**	    Pass page_size to dm1cxhas_room.
**	06-may-1996 (thaju02 & nanpr01)
**	    New page format support: change page header references to use
**	    macros.
**      03-june-1996 (stial01)
**          Added DMPP_TUPLE_INFO argument to dm1cxget() 
**      22-jul-1996 (ramra01 for bryanp)
**          Pass row_version to dmppxuncompress.
**	16-sep-1996 (canor01)
**	    Pass tuple buffer to dmppxuncompress.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Do not get page lock if row locking.
**      27-feb-97 (stial01)
**          Renamed dmve_btundo_check -> dmve_bid check, as it is now called
**          for UNDO and REDO. Changed this routine for REDO. 
**          Clean ALL deleted leaf entries if X page lock.
**      07-apr-97 (stial01)
**          dmve_bid_check() NonUnique primary btree (V2), dups can span leaf
**      21-apr-97 (stial01)
**          dmve_bid_check: Search mode is DM1C_FIND for unique keys, DM1C_TID 
**          for non-unique keys, since we re-use unique entries in 
**          dm1b_allocate, the tidp part of the leaf entry may not match.
**      30-apr-97 (stial01)
**          dmve_bid_check: When searching for key, don't overwrite logged tidp
**      21-may-1997 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
**          Non unique index: if our key matches RRANGE, check next leaf
**          (This routine used to do a dm1b_search (DM1C_TID) which would have
**          brought us to the correct leaf.)
**      30-jun-97 (dilma04)
**          Bug 83301. Set DM0P_INTENT_LK fix action if row locking.
**      30-jun-97 (stial01)
**          dmve_bid_check() Use attr/key info on leaf pages
**          Use dm0p_lock_buf_macro to lock buffer to reduce mutex duration
**      22-aug-97 (stial01)
**          dmve_bid_check() if dmv_search returns err_code == E_DM0056_NOPART,
**          we should check next page if duplicate keys and the log key
**          matches rrange key.
**      06-jan-98 (stial01)
**          dmve_bid_check() LOCK pages we fix, unfix AND UNLOCK if LK_PHYSICAL
**      09-feb-99 (stial01)
**          dmve_bid_check() Pass relcmptlvl = 0 to dm1cxkperpage
**      19-Jun-2002 (horda03) Bug 108074
**          If the table is locked exclusively, then indicate that SCONCUR
**          pages don't need to be flushed immediately. We know when that the
**          table is locked exclusively if the pg_lock_mode = LK_N.
**	18-Aug-2003 (devjo01)
**	    In crossing HORDA's bug 108074 change, needed to modify it
**	    to remove pg_lock_mode reference, since this is no longer a
**	    parameter.  Simply check for either exclusive table or 
**	    exclusive database access as "pg_lock_mode == LK_N" equivalence.
**	13-dec-04 (inkdo01)
**	    Support union in DM1B_ATTR.
**	13-Apr-2006 (jenjo02)
**	    Range attributes in the leaf can no longer be assumed
**	    to be leafatts; they may be keys only.
**	13-Jun-2006 (jenjo02)
**	    Using DM1B_MAXSTACKKEY instead of DM1B_KEYLENGTH, call dm1b_AllocKeyBuf() 
**	    to validate/allocate Leaf work space. DM1B_KEYLENGTH is now only the 
**	    max length of key attributes, not necessarily the length of the Leaf entry.
**	19-Dec-06 (kibro01) b117125
**	    In a partitioned table where we opened the master table to check
**	    attributes, the page we passed in (from the partition) is still
**	    valid, so leave it alone (don't unfix it at this stage).
*/
DB_STATUS
dmve_bid_check(
DMVE_CB             *dmve,
i4             	    opflag,
DB_TAB_ID	    *tableid,
DM_TID		    *log_bid,
DM_TID		    *log_tid,
char		    *log_key,
i4		    log_key_size,
DMP_TABLE_IO        *tbio,
i4             	    tbl_lock_mode,
i4             	    lock_type,
i4             	    lock_action,
bool                *buf_locked,
DM_TID		    *found_bid,
DMPP_PAGE	    **page_ptr,
i4             	    *err_code)
{
    DMP_DCB		*dcb = dmve->dmve_dcb_ptr;
    DMP_RCB		*rcb = NULL;
    DM_PAGENO		pageno;
    DM_PAGENO           cur_pageno;
    DM1B_INDEX          *page = NULL;
    DB_ATTS		**atts_array;
    DB_ATTS             **keyatts;
    DB_TAB_TIMESTAMP	timestamp;
    DM_LINE_IDX		pos;
    DM_TID		leaf_bid;
    DM_TID		temp_tid;
    char		*logkey_ptr;
    bool                init_done = FALSE;
    i4			atts_count;
    i4             	key_count;
    i4			uncompress_size;
    DB_STATUS		status = E_DB_OK;
    DB_STATUS           local_status;
    i4             	local_error;
    bool                have_leaf;
    i4                  lcompare;
    i4                  rcompare;
    i4             	mode;
    DM_TID              temp_log_tid;
    i4             	klen;
    i4            	kperleaf;
    char		*KeyBuf, *AllocKbuf = NULL;
    char                keybuf[DM1B_MAXSTACKKEY];
#define DMVE_MAX_ATTS 32
    DB_ATTS             db_atts[DMVE_MAX_ATTS];
    DB_ATTS             *db_atts_ptrs[DMVE_MAX_ATTS];
    DB_ATTS             *db_keys_ptrs[DMVE_MAX_ATTS];
    DB_ATTS             *atr;           /* Ptr to current attribute */
    DM1B_ATTR           attdesc;
    i4             	attno;
    DM_OBJECT           *misc_buffer = NULL;
    i4                  log_pgtype;
    i4             	log_pgsize;
    i4             	log_cmp_type;
    i4			log_ixklen;
    bool                bt_unique;
    DMPP_ACC_PLV        *loc_plv;
    DMPP_ACC_TLV        *loc_tlv;
    i4             	offset = 0;
    i4             	fix_action = 0;
    char                *str;
    i4             	recovery_action;
    DB_TAB_NAME		table_name;
    DB_OWN_NAME		table_owner;
    DB_TAB_ID		master_id;
    i4			update_mode;
    i2			abs_type;
    DB_ATTS             **RangeAtts;
    DB_ATTS		**RangeKeys;
    i4			RangeKlen;
    i4			RangeAttCount;

    if (DMZ_ASY_MACRO(6))
    {
	TRdisplay("    Bid_check: Logical Recovery on btree index.\n");
    }

    dmv_build_bt_info(dmve, &table_name, &table_owner,
	&log_pgtype, &log_pgsize, &log_cmp_type, &bt_unique, 
	&log_ixklen, &recovery_action);

    /*
    ** Get page accessors for page recovery actions.
    */
    dm1c_getaccessors(TCB_C_DEFAULT, log_pgtype, log_pgsize, 
	&loc_plv, &loc_tlv);

    for (;;)
    {
	pageno = log_bid->tid_tid.tid_page;

        /*
        ** We need to determine if the current page is still the correct page
        ** for the update action.  To do this we need attribute and key 
	** information in order to do tuple comparisons.
	** For 2k pages (or when there are no attributes stored on the leaf), 
	** we need to do a logical table open to get
	** this information. This is ok, since 'bid checking' is only
	** done during UNDO recovery for 2k pages.
	** For larger pages, the attribute and key information may be on each
	** leaf page (won't be for CLUSTERED primary).
        */
	if ( log_pgtype == TCB_PG_V1 || tbio == NULL )
	{
	    DMP_TCB             *tcb;

	    /*
	    ** Open the table with a logical OPEN operation.
	    ** This action will fail if the table cannot be fully opened 
	    ** (if part of the table is offline during this undo operation).
	    ** Since all we are after here is key and structure info, if
	    ** the put was to a partition, open the master instead.
	    ** (This will need to be changed if/when we allow structures
	    ** per partition.)
	    */
	    STRUCT_ASSIGN_MACRO(*tableid, master_id);
	    if (master_id.db_tab_index < 0)
		master_id.db_tab_index = 0;
	    status = dm2t_open(dcb, &master_id, 
			    tbl_lock_mode, DM2T_UDIRECT, DM2T_A_WRITE,
			    (i4)0, (i4)20, 0, dmve->dmve_log_id,
			    dmve->dmve_lk_id, 0, 0, dmve->dmve_db_lockmode,
			    &dmve->dmve_tran_id, &timestamp, &rcb, (DML_SCB *)0,
			    err_code);
	    if (status != E_DB_OK)
		break;

	    /*
	    ** Get correct attribute array pointers depending on the table type.
	    ** If this is a secondary index, then the keys are described by the
	    ** table attribute list.
	    */
	    tcb = rcb->rcb_tcb_ptr;
	    if (tableid->db_tab_index >= 0)
		tbio = &tcb->tcb_table_io;
	    atts_array = tcb->tcb_leafatts;
	    atts_count = tcb->tcb_leafattcnt;
	    keyatts = tcb->tcb_leafkeys;
	    key_count = tcb->tcb_keys;
	    klen = tcb->tcb_klen;
	    kperleaf = tcb->tcb_kperleaf;
	    RangeAtts = tcb->tcb_rngatts;
	    RangeAttCount = tcb->tcb_rngattcnt;
	    RangeKeys = tcb->tcb_rngkeys;
	    RangeKlen = tcb->tcb_rngklen;
	}
	else
	{
	    page = (DM1B_INDEX *) *page_ptr;
	    atts_count = DM1B_VPT_GET_BT_ATTCNT(log_pgtype, page);
	    atts_array = &db_atts_ptrs[0];
	    keyatts = &db_keys_ptrs[0];
	    for (attno = 0; attno < DMVE_MAX_ATTS; attno++)
	    {
		db_atts_ptrs[attno] = &db_atts[attno];
	    }
     
	    /*
	    ** Allocate attribute arrays if necessary.
	    ** There is a limit of 32 key componants for secondary btrees.
	    ** There is no limit for primary btree indexes
	    */
	    if (atts_count > DMVE_MAX_ATTS)
	    {
		status = dm0m_allocate((i4)( sizeof(DMP_MISC) +
		    (atts_count * sizeof(DB_ATTS)) +
		    (atts_count * sizeof(DB_ATTS *)) +
		    (atts_count * sizeof(DB_ATTS *))),
		    (i4)0, (i4)MISC_CB, (i4)MISC_ASCII_ID,
		    (char *)0, (DM_OBJECT **)&misc_buffer, err_code);
		if (status != OK)
		{
		    return (E_DB_ERROR);
		}
		((DMP_MISC *)misc_buffer)->misc_data = (char *)misc_buffer +
			sizeof(DMP_MISC);
		atts_array = 
			(DB_ATTS **)((char *)misc_buffer + sizeof(DMP_MISC));
		keyatts = (DB_ATTS **)atts_array + atts_count;
		atr = (DB_ATTS *)(keyatts + atts_count);
		for (attno = 0; attno < atts_count; attno++)
		{
		    atts_array[attno] = atr++;
		}
	    }

	    for (attno = 0, klen = 0, RangeKlen = 0, key_count = 0; 
			attno < atts_count; 
				attno++)
	    {
		atr = atts_array[attno];

		/*
		** Init attr name so dm1cxformat can compare atts with keys
		*/
		MEfill(sizeof (atr->name), (u_char)0, (char *)&atr->name);
		MEcopy((char *)&attno, sizeof (attno), (char *)&atr->name);

		DM1B_VPT_GET_ATTR(log_pgtype, log_pgsize, page, 
				(attno+1), &attdesc);
		atr->precision = 0;
		atr->collID = -1;
		atr->type = attdesc.type;
		atr->length = attdesc.len;
		abs_type = abs(atr->type);
		if (abs_type == DB_CHA_TYPE || abs_type == DB_CHR_TYPE 
		    || abs_type == DB_VCH_TYPE || abs_type == DB_TXT_TYPE 
		    || abs_type == DB_NCHR_TYPE || abs_type == DB_NVCHR_TYPE)
		    atr->collID = attdesc.u.collID;
		else atr->precision = attdesc.u.prec;
		atr->offset = offset;
		atr->flag = 0;
		atr->key = 0;
		atr->key_offset = 0;
		atr->ver_added = 0;
		atr->ver_dropped = 0;
		atr->val_from = 0;

		if (attdesc.key)
		{
		    keyatts[key_count++] = atts_array[attno];
		    atr->key = key_count;
		    atr->key_offset = offset;
		    RangeKlen += atr->length;
		}

		/* 
		** Primary index: each attribute in leaf entry is 
		** a key componant and included in klen.
		** Secondary index: all attributes in secondary index,
		** including non key fields and tidp are in klen
		*/
		klen += atr->length;
		offset += atr->length;
	    }

	    /* Check if non-key fields included (in index) */
	    if ( klen != RangeKlen && RangeKlen == log_ixklen )
	    {
		/* non-Key columns are excluded from range entries */
		RangeAtts = keyatts;
		RangeAttCount = key_count;
		RangeKeys = keyatts;
	    }
	    else 
	    {
		/* non-Key columns are included in range entries */
		RangeAtts = atts_array;
		RangeAttCount = atts_count;
		RangeKeys = keyatts;
	    }


	    kperleaf = dm1cxkperpage(log_pgtype, log_pgsize, 0, TCB_BTREE,
		atts_count, log_key_size, DM1B_PLEAF, 
		DM1B_VPT_GET_BT_TIDSZ_MACRO(log_pgtype, page), 
		0	/* Never CLUSTERED */ );
	}

        /*
        ** If the logged key is compressed, then we need to uncompress it to
        ** be able to do key comparisons below.
        */
        logkey_ptr = log_key;
        if (log_cmp_type == DM1CX_COMPRESSED)
        {
	    if ( status = dm1b_AllocKeyBuf(klen, keybuf, &KeyBuf, 
						&AllocKbuf, err_code) )
		break;
            status = (loc_tlv->dmppxuncompress)(atts_array,
                atts_count, logkey_ptr, KeyBuf, klen,
                &uncompress_size, (i4)0, (DM_TID *)0, KeyBuf, 0);
            if (status != E_DB_OK)
                break;
 
            logkey_ptr = KeyBuf;
        }

	/*
	** For unique indexes, if you are row locking and delete and insert
	** the same key, the insert will re-use the deleted leaf entry.
	** So for unique keys the search mode is DM1C_FIND, as the tidp
	** portion of the leaf may not match.
	*/
	if (bt_unique)
	    mode = DM1C_FIND;
	else
	    mode = DM1C_TID;

        break;
    }

    if (status != E_DB_OK)
    {
	ule_format(E_DM968D_DMVE_BTREE_INIT, (CL_ERR_DESC *)0, ULE_LOG,
	    NULL, (char *)0, (i4)0, (i4 *)0, err_code, 3,
	    sizeof(DB_DB_NAME), &dcb->dcb_name,
	    sizeof(DB_TAB_NAME), table_name.db_tab_name,
	    sizeof(DB_OWN_NAME), table_owner.db_own_name);
    }
    else
    {
	init_done = TRUE;
    }


    /*
    ** Start with logged page, and if necessary follow forward leaf pointers
    **
    ** If looking for a key and logged page is an overflow page, we should
    ** always find the key on that overflow page, since overflow pages don't
    ** split
    */
    page = (DM1B_INDEX *) *page_ptr;
    for ( ;status == E_DB_OK;)
    {
	if (page && DM1B_VPT_GET_PAGE_PAGE_MACRO(log_pgtype, page) != pageno)
	{
	    cur_pageno = DM1B_VPT_GET_PAGE_PAGE_MACRO(log_pgtype, page);
	    if (*buf_locked == TRUE)
	    {
		dm0p_unlock_buf_macro(tbio, dmve->dmve_lk_id, page_ptr);
		*buf_locked = FALSE;
	    }

	    status =  dm0p_uncache_fix(tbio, DM0P_UNFIX, 
		dmve->dmve_lk_id, dmve->dmve_log_id, &dmve->dmve_tran_id, 
		page_ptr, err_code); 
	    page = NULL;

	    /* 
	    ** Release page lock regardless of uncache_fix status
	    */
	    if (lock_action == LK_PHYSICAL)
	    {
		local_status = dm0p_unlock_page(dmve->dmve_lk_id, dcb, tableid,
		    cur_pageno, 
		    LK_PAGE, tbio->tbio_relid, &dmve->dmve_tran_id,
		    (LK_LKID *)0, (LK_VALUE *)0, &local_error);
	    }

	    if (status == E_DB_ERROR)
		break;
	}

	if (!page)
	{
	    if (lock_type == LK_ROW)
	    {
		fix_action |= DM0P_INTENT_LK;
	    }
            else if ( (tbl_lock_mode == LK_X) ||
		      (dcb->dcb_status & DCB_S_EXCLUSIVE) )
            {
		fix_action |= DM0P_TABLE_LOCKED_X;
            }

	    if (lock_type != LK_TABLE)
	    {
		status = dm0p_lock_page(dmve->dmve_lk_id, dcb, tableid, pageno,
		    LK_PAGE, 
		    lock_type == LK_PAGE ? LK_X : LK_IX,
		    lock_action, (i4)0, 
		    tbio->tbio_relid, &dmve->dmve_tran_id, (LK_LKID *)0, 
		    (i4 *)0, (LK_VALUE *)0, err_code);
		if (status != E_DB_OK)
		    break;
	    }

	    status = dmve_cachefix_page(dmve, (LG_LSN*)NULL,
					tbio, pageno,
					fix_action, loc_plv,
					page_ptr, err_code);

	    if (status != E_DB_OK)
		break;

	    if (lock_type == LK_ROW)
	    {
		dm0p_lock_buf_macro(tbio, dmve->dmve_lk_id, page_ptr);
		*buf_locked = TRUE;
	    }

	    page = (DM1B_INDEX *) *page_ptr;
	}

	/*
	** Compare the record's key with the leaf's high and low range keys.
	*/
	status = dmv_compare_key(log_pgtype, log_pgsize, page, dmve,
		DM1B_LRANGE, RangeAtts, RangeAttCount, RangeKeys, keyatts, key_count,
		RangeKlen, logkey_ptr, log_cmp_type, loc_tlv, &lcompare, err_code);

	if (status != E_DB_OK)
	    break;

	status = dmv_compare_key(log_pgtype, log_pgsize, page, dmve,
		DM1B_RRANGE, RangeAtts, RangeAttCount, RangeKeys, keyatts, key_count,
		RangeKlen, logkey_ptr, log_cmp_type, loc_tlv, &rcompare, err_code);

	if (status != E_DB_OK)
	    break;

	if (lcompare < DM1B_SAME)
	{
	    *err_code = E_DM0055_NONEXT;
	    status = E_DB_ERROR;
	    break;
	}

	/*
	** Compare the record's key with the leaf's high and low range keys.
	** If it falls in between then we already have the correct leaf fixed.
	*/
	if (bt_unique || (tableid->db_tab_index > 0) || 
	    (log_pgtype == TCB_PG_V1))
	{
	    if ((rcompare > DM1B_SAME) || (lcompare <= DM1B_SAME))
		have_leaf = FALSE;
	    else
		have_leaf = TRUE;
	}
	else
	{
	    /*
	    ** Non unique primary btree (V2), dups span leaf pages
	    */
	    if ((rcompare > DM1B_SAME) || (lcompare < DM1B_SAME))
		have_leaf = FALSE;
	    else
		have_leaf = TRUE;
	}

	if (have_leaf == FALSE)
	{
	    pageno = DM1B_VPT_GET_BT_NEXTPAGE_MACRO(log_pgtype, page);
	    if (pageno == 0)
	    {
		*err_code = E_DM0055_NONEXT;
		status = E_DB_ERROR;
		break;
	    }
	    else
	    {
		continue;
	    }
	}

	if (DMZ_ASY_MACRO(6))
	    TRdisplay("        Key falls within range of logged page.\n");

	if (lock_type == LK_TABLE ||
	  (lock_type == LK_PAGE && lock_action == LK_LOGICAL))
	    update_mode = DM1C_DIRECT;
	else
	    update_mode = DM1C_DIRECT | DM1C_ROWLK;

	/*
	**  Bid check prior to redo insert
	**  Clean committed deleted tuples from this leaf page
	*/
	if (opflag == DMVE_FINDPOS &&
	   log_pgtype != TCB_PG_V1 &&
	   ((DM1B_VPT_GET_PAGE_STAT_MACRO(log_pgtype, page) & DMPP_CLEAN) != 0))
	{

	    status = dm1cxclean(log_pgtype, log_pgsize, page,
		    log_cmp_type, update_mode);
	    if (status != E_DB_OK)
		break;
	}

	if (opflag == DMVE_FINDLEAF)
	{
	    /*
	    ** Since the range keys matched, we have the correct page.
	    */
	    leaf_bid.tid_tid.tid_page = 
		DM1B_VPT_GET_PAGE_PAGE_MACRO(log_pgtype, page);
	    leaf_bid.tid_tid.tid_line = 0;
	    break;
	}

	if (opflag == DMVE_FINDKEY)
	{
	    /*
	    ** For unique indexes, if you are row locking and delete and insert
	    ** the same key, the insert will re-use the deleted leaf entry.
	    ** So for unique keys the search mode is DM1C_FIND, as the tidp
	    ** portion of the leaf may not match. In this case be careful
	    ** not to overwrite the logged tidp with the tidp in the leaf entry
	    */
	    temp_log_tid.tid_i4 = log_tid->tid_i4;
	    status = dmv_search(log_pgtype, log_pgsize, page, dmve, mode,
		atts_array, atts_count, keyatts, key_count, logkey_ptr, klen, 
		log_cmp_type, loc_tlv, &temp_log_tid, &pos, err_code);
	    if (status == E_DB_OK)
	    {
		/*
		** Found logged key: Set BID to point to the entry just found
		*/
		leaf_bid.tid_tid.tid_page = 
			DM1B_VPT_GET_PAGE_PAGE_MACRO(log_pgtype, page);
		leaf_bid.tid_tid.tid_line = pos;
		break;
	    }
	    else if (*err_code == E_DM0056_NOPART )
	    {
		if (bt_unique)
		{
		    break;
		}

		/*
		** Non unique btree:
		** If our key matches RRANGE, we should check the next leaf
		*/
		if (rcompare != DM1B_SAME)
		{
		    *err_code = E_DM0055_NONEXT;
		    status = E_DB_ERROR;
		    break;
		}

		status = E_DB_OK;
		pageno = DM1B_VPT_GET_BT_NEXTPAGE_MACRO(log_pgtype, page);
		continue;
	    }
	    else
	    {
		/* A serious error */
		break;
	    }
	}

	if (opflag == DMVE_FINDPOS)
	{
	    /*
	    ** Check for available space on the logged leaf page.
	    ** Finding the correct page but finding that it does not
	    ** have sufficient space for the insert is an unexpected
	    ** problem and will cause this recovery action to fail.
	    */
	    if (dm1cxhas_room(log_pgtype, log_pgsize, page, log_cmp_type, 
		    (i4)100, kperleaf, log_key_size + 
			DM1B_VPT_GET_BT_TIDSZ_MACRO(log_pgtype, page))
				== FALSE)
	    {
		if (bt_unique)
		{
		    ule_format(E_DM966F_DMVE_BTOVFL_NOROOM, (CL_ERR_DESC *)0, 
			ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, 
			err_code, 9,
			sizeof(DB_DB_NAME), &dcb->dcb_name,
			sizeof(DB_TAB_NAME), table_name.db_tab_name,
			sizeof(DB_OWN_NAME), table_owner.db_own_name,
			0, DM1B_VPT_GET_PAGE_PAGE_MACRO(log_pgtype, page),
			0, DM1B_VPT_GET_SPLIT_LSN_HIGH_MACRO(log_pgtype, page), 
			0, DM1B_VPT_GET_SPLIT_LSN_LOW_MACRO(log_pgtype, page),
			0, DM1B_VPT_GET_BT_KIDS_MACRO(log_pgtype, page),
			0, kperleaf, 0, log_key_size +
			    DM1B_VPT_GET_BT_TIDSZ_MACRO(log_pgtype, page));
		    *err_code = E_DM0055_NONEXT;
		    status = E_DB_ERROR;
		    break;
		}

		/*
		** Non unique btree:
		** If our key matches RRANGE, we should check the next leaf
		*/
		if (rcompare != DM1B_SAME)
		{
		    *err_code = E_DM0055_NONEXT;
		    status = E_DB_ERROR;
		    break;
		}

		pageno = DM1B_VPT_GET_BT_NEXTPAGE_MACRO(log_pgtype, page);
		continue;
	    }
	    else
	    {
		/*
		** Leaf page has room for the key to be re-inserted.  
		** Search the page to find the proper position at which
		** to place it.
		**
		** This call may return NOPART if the leaf is empty.
		** In that case 'pos' will have been correctly set to 
		** the 1st position.
		*/
		status = dmv_search(log_pgtype, log_pgsize, page, dmve, 
			    DM1C_OPTIM, atts_array, atts_count,
			    keyatts, key_count, logkey_ptr, klen,
			    log_cmp_type, loc_tlv, &temp_tid, &pos, err_code);
		if ((status != E_DB_OK) && (*err_code == E_DM0056_NOPART))
		    status = E_DB_OK;

		if (status == E_DB_OK)
		{
		    leaf_bid.tid_tid.tid_page = 
			DM1B_VPT_GET_PAGE_PAGE_MACRO(log_pgtype, page);
		    leaf_bid.tid_tid.tid_line = pos;
		    break;
		}
	    }
	}
    }

    if (misc_buffer)
	dm0m_deallocate((DM_OBJECT **)&misc_buffer);

    /*
    ** If we did a logical table open, close the table
    */
    if (rcb)
    {
	/*
	** b117125 (kibro01)
	** If this page was from a partition and we actually opened the 
	** master table to check attributes, this page is still valid
	** and is expected to be present after bid_check completes, so leave
	** it alone and leave the buffer locked
	*/
	if (!(tableid->db_tab_index & DB_TAB_PARTITION_MASK))
	{
	    cur_pageno = DM1B_VPT_GET_PAGE_PAGE_MACRO(log_pgtype, page);

	    if (*buf_locked)
	    {
	        dm0p_unlock_buf_macro(tbio, dmve->dmve_lk_id, page_ptr);
	        *buf_locked = FALSE;
	    }

	    if (*page_ptr)
	    {
	        status =  dm0p_uncache_fix(tbio, DM0P_UNFIX, 
		    dmve->dmve_lk_id, dmve->dmve_log_id, &dmve->dmve_tran_id, 
		    page_ptr, err_code); 

	        /* 
	        ** Release page lock regardless of uncache_fix status
	        */
	        if (lock_action == LK_PHYSICAL)
	        {
		    local_status = dm0p_unlock_page(dmve->dmve_lk_id, dcb, tableid,
		        cur_pageno, 
		        LK_PAGE, tbio->tbio_relid, &dmve->dmve_tran_id,
		        (LK_LKID *)0, (LK_VALUE *)0, &local_error);
	        }
	    }
	}
	
	local_status = dm2t_close(rcb, DM2T_NOPURGE, &local_error);
    }
    
    /* Discard any allocated key buffer */
    if ( AllocKbuf )
	dm1b_DeallocKeyBuf(&AllocKbuf, &KeyBuf);

    if (status == E_DB_OK)
    {
	if (DMZ_ASY_MACRO(6))
	{
	    TRdisplay("    Bid_check: Returns BID (%d,%d).\n",
		leaf_bid.tid_tid.tid_page, leaf_bid.tid_tid.tid_line);
	}

	*found_bid = leaf_bid;
	return (E_DB_OK);
    }

    /*
    ** If one of the above calls returned an unexpected error, log it
    ** If initialization error, it was already printed
    */
    if (init_done == TRUE)
    {
	ule_format(*err_code, (CL_ERR_DESC *)0, ULE_LOG, NULL, 
	    (char *)0, (i4)0, (i4 *)0, err_code, 0);
    }

    /*
    ** If the quest for the sacred page has failed, then we're hosed.
    */
    if (opflag == DMVE_FINDLEAF)
	str = "PAGE";
    else if (opflag == DMVE_FINDPOS) 
	str = "POSITION";
    else
	str = "ENTRY";

    ule_format(E_DM968E_DMVE_BTREE_FIND, (CL_ERR_DESC *)0, ULE_LOG,
	NULL, (char *)0, (i4)0, (i4 *)0, err_code, 9,
	4, (recovery_action == DMVE_UNDO) ? "UNDO" : "REDO",
	sizeof(DB_DB_NAME), &dcb->dcb_name,
	sizeof(DB_TAB_NAME), table_name.db_tab_name,
	sizeof(DB_OWN_NAME), table_owner.db_own_name,
	(i4)STlength(str), str,
	0, log_bid->tid_tid.tid_page,
	0, log_bid->tid_tid.tid_line,
	4, (log_cmp_type == DM1CX_COMPRESSED) ? "ON, " : "OFF,",
	11, (bt_unique == TRUE) ? "UNIQUE.    " : "NOT UNIQUE.");


    if (init_done == TRUE)
    {
	/* Using key information, print the logged key */
    }

    *err_code = E_DM968F_DMVE_BID_CHECK;
    return (E_DB_ERROR);
}

static DB_STATUS
dmv_compare_key(
i4		page_type,
i4		page_size,
DM1B_INDEX  	*leaf,
DMVE_CB     	*dmve,
i4      	lineno,
DB_ATTS      	**RangeAtts,
i4      	RangeAttsCount,
DB_ATTS      	**RangeKeys,
DB_ATTS      	**LeafKeyAtts,
i4      	key_count,
i4      	RangeKlen,
char        	*LeafKey,
i4      	compression_type,
DMPP_ACC_TLV 	*tlv,
i4          	*compare,
i4     		*err_code)
{
    DMP_DCB             *dcb = dmve->dmve_dcb_ptr;
    DB_STATUS           s;
    char                *tmpkey;
    char		*KeyBuf, *AllocKbuf;
    char                keybuf[DM1B_MAXSTACKKEY];
    i4             	tmpkey_len;
    i4             	infinity;
    ADF_CB              adf_cb;


    /* This fcn is ONLY called when lineno is a range */
    if ( (lineno == DM1B_LRANGE) || (lineno == DM1B_RRANGE))
    {
	if (lineno == DM1B_LRANGE)
	    *compare = DM1B_SAME + 1;
	else
	    *compare = DM1B_SAME - 1;
	
	dm1cxtget(page_type, page_size, leaf, lineno, 
			(DM_TID *)&infinity, (i4*)NULL);
	if (infinity == TRUE)
	{
	    /* not same */
	    return (E_DB_OK);
	}
    }

    /* Get LRANGE/RRANGE Index key */
    if ( s = dm1b_AllocKeyBuf(RangeKlen, keybuf,
				&KeyBuf, &AllocKbuf, err_code) )
	return(s);

    tmpkey = KeyBuf;
    tmpkey_len = RangeKlen;
    s = dm1cxget(page_type, page_size, leaf, tlv, RangeAtts, RangeAttsCount, 
		    lineno, &tmpkey, (DM_TID *)0, (i4*)0,
		    &tmpkey_len, compression_type, NULL);

    if (s == E_DB_WARN && (page_type != TCB_PG_V1) )
	s = E_DB_OK;

    if (s == E_DB_OK)
    {
	/*
	** Init adf control block to use for operations on this table.
	** Set default values for adf variables - collation and timezone.
	*/
	adf_cb.adf_errcb.ad_ebuflen = 0;
	adf_cb.adf_errcb.ad_errmsgp = 0;
	adf_cb.adf_maxstring = DB_MAXSTRING;
	adf_cb.adf_tzcb = dmf_svcb->svcb_tzcb;
	adf_cb.adf_collation = dcb->dcb_collation;
	adf_cb.adf_ucollation = dcb->dcb_ucollation;
	if (adf_cb.adf_ucollation)
	{
	    if (dcb->dcb_dbservice & DU_UNICODE_NFC)
		adf_cb.adf_uninorm_flag = AD_UNINORM_NFC;
	    else adf_cb.adf_uninorm_flag = AD_UNINORM_NFD;
	}
	else 
	    adf_cb.adf_uninorm_flag = 0;

        if (CM_ischarsetUTF8())
            adf_cb.adf_utf8_flag = AD_UTF8_ENABLED;
        else
            adf_cb.adf_utf8_flag = 0;

	MEfill(sizeof(ADI_WARN), '\0', &adf_cb.adf_warncb);

	/* Compare leaf::range */
	s = adt_ixlcmp(&adf_cb, key_count,
			LeafKeyAtts, LeafKey,
			RangeKeys, tmpkey, compare);
	if (s != E_DB_OK)
	{
	    ule_format(E_DMF012_ADT_FAIL, (CL_SYS_ERR *)0, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
	}
    }
    else if (s == E_DB_ERROR)
    {
	dm1cxlog_error( E_DM93E3_BAD_INDEX_GET, (DMP_RCB *)0, leaf,
			    page_type, page_size, lineno );
    }

    /* Discard any allocated key buffer */
    if ( AllocKbuf )
	dm1b_DeallocKeyBuf(&AllocKbuf, &KeyBuf);

    return (s);
}
/*{
** Name: dmv_search - Search for a key or a key,tid pair on a page.
**
** Description:
**      This routines searches a BTREE index page for a key or key,tid pair.
**
**      This routine returns the line table position where the 
**      search ended.  It also assumes that the page pointed to by 
**      buffer is only one fixed on entry and exit.  This page
**      may change if search mode is DM1C_TID.
**
**	DM1C_FIND		    Locate the position on this page where
**      DM1C_OPTIM                  this entry is located, or the position where
**				    it would be inserted if it's not found. If
**				    the page is empty, or if this entry is
**				    greater than all entries on this page, then
**				    return E_DM0056_NOPART.
**
**	DM1C_TID                    Locate a specific entry on the page. The
**				    entry is located by comparing the TIDP of
**				    the entry on the page with the passed-in
**				    TID.
**
** Inputs:
**      dmve_cb
**      mode                            Value indicating type of search:
**                                          DM1C_FIND, DM1C_OPTIM or DM1C_TID
**      atts_array                      Pointer to a list of pointers to
**                                      attribute descriptors.
**      atts_count                      Number of entries in atts_array.
**      keyatts				Pointer to a structure describing
**                                      the attributes making up the key.
**      key_count                       Value indicating how many attributes
**                                      are in key.
**      key                             The specified key to search for.
**      klen				key length
**      buffer                          Pointer to the page to search
**      page_size
**      compression_type
**      adf_cb
**      tlv
**	tid				If mode == DM1C_TID, this value is the
**					TID which we use to search for the
**					exact entry to return.
**
** Outputs:
**      pos                             Pointer to an area to return
**                                      the position of the line table 
**                                      where search ended.
**      tid                             Pointer to an area to return
**                                      the tid of the record pointed to
**                                      by line table position.
**      err_code                        Pointer to an area to return 
**                                      the following errors when 
**                                      E_DB_ERROR is returned.
**                                      E_DM0056_NOPART
**					E_DM93BA_BXSEARCH_BADPARAM
**                                      
**	Returns:
**
**	    E_DB_OK
**          E_DB_ERROR
**          E_DB_FATAL
**	Exceptions:
**	    none
**
** Side Effects:
**
** History:
**      30-jun-97 (stial01)
**          Created similar to dm1bxsearch(), without rcb dependency 
**          This code assumes search direction = DM1C_EXACT.
**	13-Jun-2006 (jenjo02)
**	    Using DM1B_MAXSTACKKEY instead of DM1B_KEYLENGTH, call dm1b_AllocKeyBuf() 
**	    to validate/allocate Leaf work space. DM1B_KEYLENGTH is now only the 
**	    max length of key attributes, not necessarily the length of the Leaf entry.
**	11-may-2007 (gupsh01)
**	   Initialize adf_utf8_flag.
*/  
static DB_STATUS
dmv_search(
i4		page_type,
i4		page_size,
DM1B_INDEX	*buffer,
DMVE_CB             *dmve,
i4             mode,
DB_ATTS             **atts_array,
i4             atts_count,
DB_ATTS             **keyatts,
i4             key_count,
char                *key,
i4             klen,
i4             compression_type,
DMPP_ACC_TLV        *tlv,
DM_TID              *tid,
DM_LINE_IDX         *pos,
i4             *err_code)
{
    DMP_DCB             *dcb = dmve->dmve_dcb_ptr;
    char	        *keypos; 
    char		*KeyBuf, *AllocKbuf = NULL;
    char	        keybuf[DM1B_MAXSTACKKEY];
    i4	        	keylen;
    DM_TID	        buftid; 
    DB_STATUS	        s; 
    i4	        	compare;
    DM1B_INDEX	        *page; 
    CL_SYS_ERR	        sys_err;
    ADF_CB              adf_cb;


    /*
    ** Init adf control block to use for operations on this table.
    ** Set default values for adf variables - collation and timezone.
    */
    adf_cb.adf_errcb.ad_ebuflen = 0;
    adf_cb.adf_errcb.ad_errmsgp = 0;
    adf_cb.adf_maxstring = DB_MAXSTRING;
    adf_cb.adf_tzcb = dmf_svcb->svcb_tzcb;
    adf_cb.adf_collation = dcb->dcb_collation;
    adf_cb.adf_ucollation = dcb->dcb_ucollation;
    if (adf_cb.adf_ucollation)
    {
	if (dcb->dcb_dbservice & DU_UNICODE_NFC)
	    adf_cb.adf_uninorm_flag = AD_UNINORM_NFC;
	else adf_cb.adf_uninorm_flag = AD_UNINORM_NFD;
    }
    else adf_cb.adf_uninorm_flag = 0;
    MEfill(sizeof(ADI_WARN), '\0', &adf_cb.adf_warncb);

    if (CM_ischarsetUTF8())
        adf_cb.adf_utf8_flag = AD_UTF8_ENABLED;
    else
        adf_cb.adf_utf8_flag = 0;

    *err_code = E_DB_OK;
    s = E_DB_OK;
    page = (DM1B_INDEX *)buffer; 

    do
    {
	/* 
        ** Binary search the page to find the position of a
        ** key that is greater than or equal to one specified.
        ** If find multiple matching entries, get lowest such entry.
        */
        
	s = dmv_binary_search(page_type, page_size, page, dmve, 
			DM1C_EXACT, key, atts_array, atts_count,
			keyatts, key_count, klen,
			compression_type, &adf_cb, tlv, pos, err_code);
	if (s != E_DB_OK)
	{
	    ule_format(*err_code, 0, ULE_LOG, NULL, 0, 0, 0, err_code, 0);
	    *err_code = E_DM93BC_BXSEARCH_ERROR;
	    break;
	}

	if (mode != DM1C_TID)
	{
            if (*pos != DM1B_VPT_GET_BT_KIDS_MACRO(page_type, page))
		dm1cxtget(page_type, page_size, page, (*pos), 
				tid, (i4*)NULL );

	    break;
	}

	/* 
	** If the mode is DM1C_TID, then want the exact row matching the key
	** and TID values, not just the first entry matching the given key.
	** Search from this position onward, until we either locate the entry 
	** whose TIDP matches the indicated TID or run out of entries.  Note
	** that we extract the index entry using atts_array and atts_count which	
	** describe the entire entry, but compare keys using keyatts and 
	** key_count, which describe only the key if this is a secondary index.
	*/
	if ( s = dm1b_AllocKeyBuf(klen, keybuf,
				    &KeyBuf, &AllocKbuf, err_code) )
	    break;

	for (; (i4)*pos < 
		(i4)DM1B_VPT_GET_BT_KIDS_MACRO(page_type, page);
		(*pos)++)
	{
	    keypos = KeyBuf;
	    keylen = klen;
	    s = dm1cxget(page_type, page_size, page, tlv, 
			 atts_array, atts_count, *pos, 
			 &keypos, (DM_TID *)&buftid, (i4*)NULL,
			 &keylen, compression_type, NULL);

	    if (page_type != TCB_PG_V1 && s == E_DB_WARN)
		s = E_DB_OK;

	    if (s != E_DB_OK)
	    {
		dm1cxlog_error( E_DM93E3_BAD_INDEX_GET, (DMP_RCB *)0, page, 
				page_type, page_size, *pos);
		*err_code = E_DM93BC_BXSEARCH_ERROR;
	    }

	    if ( s == E_DB_OK )
	    {
		s = adt_kkcmp(&adf_cb, (i4)key_count, keyatts, keypos, key, 
		    &compare);
		if (s != E_DB_OK)
		{
		    ule_format(E_DMF012_ADT_FAIL, (CL_SYS_ERR *)0, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 0);
		    *err_code = E_DM93BC_BXSEARCH_ERROR;
		}
		if (compare != DM1B_SAME)
		{
		    *err_code = E_DM0056_NOPART;
		    s = E_DB_ERROR;
		}      
	    }

	    if ( s || (tid->tid_tid.tid_page == buftid.tid_tid.tid_page 
			&& tid->tid_tid.tid_line == buftid.tid_tid.tid_line) )
	    {
		/* Discard any allocated key buffer */
		if ( AllocKbuf )
		    dm1b_DeallocKeyBuf(&AllocKbuf, &KeyBuf);
		return (s); 
	    }
	}

	break;

    } while (FALSE);

    /* Discard any allocated key buffer */
    if ( AllocKbuf )
	dm1b_DeallocKeyBuf(&AllocKbuf, &KeyBuf);

    if (s != E_DB_OK)
	return (s);

    if (*pos == DM1B_VPT_GET_BT_KIDS_MACRO(page_type, page))
    {
	*err_code = E_DM0056_NOPART;
        return (E_DB_ERROR);
    }    
    return(E_DB_OK); 
}


/*{
** Name: dmv_binary_search - Perform a binary search on a BTREE index page
**
** Description:
**      This routines examines the BTREE index page to determine the 
**      position of the supplied key. This routine has some complexity due to
**	the fact that a page may have multiple duplicate keys.
**
**	The semantics of this routine depend on whether the provided key
**	matches one of the entries on the page or not.
**
**	If the provided key does NOT match any of the entries on the page, then
**	this routine returns the location on the page where this key would be
**	inserted. This will be a value between 0 and bt_kids. As a special case
**	of this, if the page is empty we return *pos == 0.
**
**	If the provided key DOES match one or more of the entries on the page,
**	then the value returned in 'pos' depends on the value of 'direction':
**
**	    DM1C_NEXT               Return the position immediately FOLLOWING 
**                                  the last occurrence of the key on the page.
**                                  This will be a value between 1 and bt_kids
**
**	    DM1C_EXACT,DM1C_PREV    Return the position of the first occurrence
**                                  of the key on the page. This will be
**                                  a value between 0 and (bt_kids-1).
**
**	Note that an INDEX page never has duplicate entries; only leaf and
**	overflow pages have duplicate entries.
**
** Inputs:
**      dmve_cb
**      direction                       Value to indicate the direction
**                                      of search when duplicate keys are 
**                                      found:
**					    DM1C_NEXT
**					    DM1C_PREV
**					    DM1C_EXACT
**	key				The specified key to search for.
**      atts_array                      Pointer to a list of pointers to
**                                      attribute descriptors.
**      atts_count                      Number of entries in atts_array.
**      keyatts				Pointer to a structure describing
**                                      the attributes making up the key.
**      key_count                       Value indicating how many attributes
**                                      are in key.
**      klen				key length
**      page                            Pointer to the page to search
**      page_size
**      compression_type
**      adf_cb
**      tlv
**
** Outputs:
**      pos                             Pointer to an area to return result.
**					See description above for the value
**					which is returned under various
**					combinations of input key and direction.
**	err_code			Set to an error code if an error occurs
**                                      
**	Returns:
**
**	    E_DB_OK
**	    E_DB_ERROR			Error uncompressing an entry
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      30-jun-97 (stial01)
**          Created similar to binary_search(), without rcb dependency 
**	13-Jun-2006 (jenjo02)
**	    Using DM1B_MAXSTACKKEY instead of DM1B_KEYLENGTH, call dm1b_AllocKeyBuf() 
**	    to validate/allocate Leaf work space. DM1B_KEYLENGTH is now only the 
**	    max length of key attributes, not necessarily the length of the Leaf entry.
*/
static DB_STATUS
dmv_binary_search(
i4		page_type,
i4		page_size,
DM1B_INDEX	*page,
DMVE_CB             *dmve,
i4             direction,
char                *key,
DB_ATTS             **atts_array,
i4             atts_count,
DB_ATTS             **keyatts,
i4             key_count,
i4             klen,
i4             compression_type,
ADF_CB              *adf_cb,
DMPP_ACC_TLV        *tlv,
DM_LINE_IDX         *pos,
i4             *err_code)
{
    i4			right, left, middle;
    i4			compare;
    DB_STATUS		s;
    char		*KeyBuf, *AllocKbuf = NULL;
    char		key_buf[DM1B_MAXSTACKKEY];
    char		*key_ptr;
    i4			keylen;

    
    /* If page has no records, nothing to search. */

    if (DM1B_VPT_GET_BT_KIDS_MACRO(page_type, page) == 0)
    {
        *pos = 0;
        return (E_DB_OK);
    }

    /* 
    ** Start binary search of keys in record on page.  This binary_search is
    ** different from those you'll see in textbooks because we continue
    ** searching when a matching key is found.
    **
    ** Note that we use atts_array and atts_count to extract the index entry,
    ** since these describe the entire entry, but use keyatts and key_count 
    ** to perform the comparison, since these describe the search criteria.
    */

    if ( s = dm1b_AllocKeyBuf(klen, key_buf,
				&KeyBuf, &AllocKbuf, err_code) )
	return(s);
   
    left = 0;
    right = DM1B_VPT_GET_BT_KIDS_MACRO(page_type, page) - 1;
    if (DM1B_VPT_GET_PAGE_STAT_MACRO(page_type, page) & DMPP_INDEX)
        right--;
    
    while (left <= right && s == E_DB_OK )
    {
        middle = (left + right)/2;

	key_ptr = KeyBuf;
	keylen = klen;
	s = dm1cxget(page_type, page_size, page, tlv, atts_array, atts_count, 
			middle, &key_ptr, (DM_TID *)0, (i4*)0,
			&keylen,
			compression_type, NULL);

	if (page_type != TCB_PG_V1 && s == E_DB_WARN)
	    s = E_DB_OK;

	if (s != E_DB_OK)
	    dm1cxlog_error( E_DM93E3_BAD_INDEX_GET, (DMP_RCB *)0, page, 
				page_type, page_size, middle);

	if ( s == E_DB_OK )
	{
	    s = adt_kkcmp(adf_cb, (i4)key_count, keyatts, key_ptr, key, &compare);
	    if (s != E_DB_OK)
	    {
		ule_format(E_DMF012_ADT_FAIL, (CL_SYS_ERR *)0, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 0);
	    }
	    else
	    {
		if (compare > DM1B_SAME)
		    right = middle - 1;
		else if (compare < DM1B_SAME)
		    left = middle + 1;
		else if (direction == DM1C_NEXT)
		    left = middle + 1;
		else
		    right = middle - 1;
	    }
	}
    }

    /* Discard any allocated key buffer */
    if ( AllocKbuf )
	dm1b_DeallocKeyBuf(&AllocKbuf, &KeyBuf);

    if ( s )
	*err_code = E_DM93B3_BXBINSRCH_ERROR;
    else
	*pos = left;

    return (s);
}
static VOID dmv_build_bt_info(
DMVE_CB             *dmve,
DB_TAB_NAME         *table_name,
DB_OWN_NAME         *table_owner,
i4                  *page_type,
i4             *page_size,
i4             *compression_type,
bool                *bt_unique,
i4		    *ixklen,
i4             *recovery_action)
{
    char                *tab_name_ptr;
    char                *own_name_ptr;

    switch (((DM0L_HEADER *)dmve->dmve_log_rec)->type)
    {

	case DM0LBTPUT:
	{
	    DM0L_BTPUT	*put_rec = (DM0L_BTPUT *)dmve->dmve_log_rec;

	    tab_name_ptr = &put_rec->btp_vbuf[0];
	    own_name_ptr = &put_rec->btp_vbuf[put_rec->btp_tab_size];

	    dmve_name_unpack(tab_name_ptr, put_rec->btp_tab_size, table_name,
			     own_name_ptr, put_rec->btp_own_size, table_owner);
	    *page_type = put_rec->btp_pg_type;
	    *page_size = put_rec->btp_page_size;
	    *compression_type = put_rec->btp_cmp_type;
	    if (put_rec->btp_header.flags & DM0L_BT_UNIQUE)
		*bt_unique = TRUE;
	    else
		*bt_unique = FALSE;
	    *recovery_action = dmve->dmve_action;
	    *ixklen = put_rec->btp_ixklen;
	    if (put_rec->btp_header.flags & DM0L_CLR)
		*recovery_action = DMVE_UNDO;
	}
	break;

	case DM0LBTDEL:
	{
	    DM0L_BTDEL	*del_rec = (DM0L_BTDEL *)dmve->dmve_log_rec;

	    tab_name_ptr = &del_rec->btd_vbuf[0];
	    own_name_ptr = &del_rec->btd_vbuf[del_rec->btd_tab_size];

	    dmve_name_unpack(tab_name_ptr, del_rec->btd_tab_size, table_name,
			     own_name_ptr, del_rec->btd_own_size, table_owner);
	    *page_type = del_rec->btd_pg_type;
	    *page_size = del_rec->btd_page_size;
	    *compression_type = del_rec->btd_cmp_type;
	    if (del_rec->btd_header.flags & DM0L_BT_UNIQUE)
		*bt_unique = TRUE;
	    else
		*bt_unique = FALSE;
	    *recovery_action = dmve->dmve_action;
	    *ixklen = del_rec->btd_ixklen;
	    if (del_rec->btd_header.flags & DM0L_CLR)
		*recovery_action = DMVE_UNDO;
	}
	break;

	case DM0LBTFREE:
	{
	    DM0L_BTFREE  *btf_rec = (DM0L_BTFREE *)dmve->dmve_log_rec;

	    *table_name = btf_rec->btf_tblname;
	    *table_owner = btf_rec->btf_tblowner;

	    *page_type = btf_rec->btf_pg_type;
	    *page_size = btf_rec->btf_page_size;
	    *compression_type = btf_rec->btf_cmp_type;
	    if (btf_rec->btf_header.flags & DM0L_BT_UNIQUE)
		*bt_unique = TRUE;
	    else
		*bt_unique = FALSE;
	    *recovery_action = dmve->dmve_action;
	    *ixklen = btf_rec->btf_ixklen;
	    if (btf_rec->btf_header.flags & DM0L_CLR)
		*recovery_action = DMVE_UNDO;
	}
	break;
    }

    return;
}

