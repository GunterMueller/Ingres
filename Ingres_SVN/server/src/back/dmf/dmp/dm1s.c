/*
**Copyright (c) 2004 Ingres Corporation
NO_OPTIM=pym_us5
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <di.h>
#include    <me.h>
#include    <pc.h>
#include    <tr.h>
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
#include    <dmpl.h>
#include    <dmpp.h>
#include    <dm1b.h>
#include    <dm0l.h>
#include    <dm0p.h>
#include    <dm1c.h>
#include    <dm1p.h>
#include    <dm1s.h>
#include    <dm2umct.h>
#include    <dm2f.h>
#include    <dmpbuild.h>
#include    <dm1r.h>


/**
**
**  Name: DM1S.C - Routines to allocate and search SEQUENTIAL tables.
**
**  Description:
**      This file contains the access method specific routines needed to
**      acess SEQuential tables.
**
**      The routines defined in this file are:
**      dm1s_allocate       - Allocates space for a new record.
**      dm1s_deallocate     - Deallocates space for a new record.
**      dm1s_search         - Sets the search limits for a scan.
**
**  History:
**      13-dec-85 (jennifer)
**          Changed for Jupiter.
**	14-jun-1991 (Derek)
**	    Changed to use new allocation routines.  Changed allocate to
**	    not change the page at allocate time.
**      23-oct-91 (jnash)
**          In dm1s_allocate, remove what was third parameter to dm0p_bicheck 
**	    as part of 6.4 -> 6.5 merge.
**	 3-nov-191 (rogerk)
**	    Added dm0l_ovfl log record during dm1s_allocate when a new overflow
**	    page is added.  This log record is used to REDO the operation of
**	    linking the new page into the overflow chain.
**	20-Nov-1991 (rmuth)
**          Added DM1P_MINI_TRAN, when inside a mini-transaction pass this
**          flag to dm1p_getfree() so it does not allocate a page dallocated
**          earlier by this transaction. see dm1p.c for explanation why.
**	    Corrected the code to pass the correct flags to dm1p_getfree().
**	7-may-1992 (bryanp)
**	    Removed temp_alloc(). Temporary tables will use the same allocation
**	    mechanism as all other tables do.
**	 7-jul-1992 (rogerk)
**	    Prototyping DMF.
**	15-jul-1992 (bryanp)
**	    Set mct_rebuild when emptying an existing temp table using the
**	    build routines. 
**	09-jun-1992 (kwatts)
**	    6.5 MPF Changes. Replaced calls of dm1c_allocate with accessors.
**	    Set up new mct_data_atts instead of mct_atts_ptr (removed).
**	29-August-1992 (rmuth)
**        File extend changes
**          - Pass the DM1P_LASTUSED_DATA to dm1p_lastused.
**          - Temp tables, Initialise some iirelation fields which we
**            cannot know until after the table has been built, ie relfhdr
**            and relpages.
**	17-sep-1992 (jnash)
**	    Reduced logging project.
**	    --  Add new param's to dmpp_allocate call.
**	3-oct-1992 (bryanp)
**	    Re-arrange includes to share transaction ID type definition.
**	05-oct-1992 (rogerk)
**	    Reduced Logging Project.  In preparation for support of Partial
**	    TCB's for the new recovery schemes, changed references to
**	    some tcb fields to use the new tcb_table_io structure fields.
**	26-oct-1992 (rogerk)
**	    Reduced Logging Project: Changed arguments to dm0p_mutex and
**	    dm0p_unmutex calls.
**	03-nov-1992 (jnash)
**	    Fill out parameter list to dmpp_allocate.  This is not
**	    really necessary as these param's are only used for system
**	    catalogs, but it is done for consistency.
**	14-dec-1992 (jnash & rogerk)
**	    Reduced logging project (phase 2):
**	      - Changed to use new recovery protocols.  Before Images are
**		no longer written and the pages updated by the allocate must
**		be mutexed while the dm0l_ovfl log record is written and
**		the pages are updated.
**	      - Changed to pass new arguments to dm0l_ovfl.
**	      - Add new param's to dmpp_allocate call.
**	      - Change pageno from i4 to DM_PAGENO type to quiet compiler.
**	      - Removed requirement that file-extend operations on system
**		catalogs must be forced to the database.  Restriction not
**		needed anymore since system catalogs now do Redo Recovery.
**	      - Changed arguments to dm0p_mutex and dm0p_unmutex calls.
**	      - Add dm1s_empty_table.
**	14-dec-1992 (rogerk)
**	    Include dm1b.h before dm0l.h for prototype definitions.
**	02-jan-1993 (jnash)
**	    More reduced logging work. Add LGreserve calls.
**	15-mar-1993 (rogerk)
**	    Reduced Logging - Phase IV:
**	    Changed Begin and End Mini log records to use LSN's rather than LAs.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**		Improve error handling and reporting.
**	24-may-1993 (bryanp)
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	26-jul-1993 (jnash)
**	    Fix multi-location recovery problem by dynamically allocating
**	    FCB memory.
**	26-jul-1993 (rmuth)
**	    Initialise mct_override_alloc.
**	26-jul-1993 (rogerk)
**	    Change generation of Before Image log records during Online Backup.
**	    All page modifiers must now call dm0p_before_image_check before
**	    changing the page to implement online backup protocols.
**	23-aug-1993 (rogerk)
**	    Took creating of table files out of dm1s_empty_table routine.  The
**	    files are now created by dm2u_file_create.  Removed unneeded
**	    parameters from the routine.
**	20-sep-1993 (jnash)
**	    Add flag param to LGreserve() call.
**	18-apr-1994 (jnash)
**	    fsync project.  Eliminate calls to dm2f_force_file().
**	11-apr-1994 (rmuth)
**	    b62263: dm1s_deallocate, Only tossed pages from the buffer cache 
**	    if in the table is in deferred I/O mode. Non-deffered I/O tables 
**	    can also have pages in the buffer cache and these need to be 
**	    tossed aswell.
**	02-Jan-1996 (ramra01)
**	    b72963: dm1s_allocate, ends up in an infinite loop when it 
**	    attempts to fix a page while there is another process busy
**	    going thru an UNDO operation dealing with the same FMAP.
**	    The page that gets fixed is no longer the last page.
**	06-mar-1996 (stial01 for bryanp)
**	    In dm1s_empty_table, don't allocate pages on the stack. Instead,
**		allocate them from dynamic memory.
**	    Added page_size argument.
**	    Pass tcb_rel.relpgsize as page_size argument to dmpp_allocate.
**	    In dm1s_deallocate, check status as well as error_code when logging
**		errors.
**      06-mar-1996 (stial01)
**          Pass page_size to dm2f_write_file()
**      21-may-1996 (thaju02)
**          Pass page size to SET_FREE_HINT_MACRO, MAP_FROM_NUMBER_MACRO
**          for 64 bit tid support and multiple fhdr pages.
**	 3-jun-96 (nick)
**	    Change heap allocation to single stream allocation ; we
**	    must enforce this else end up with free pages embedded
**	    within the file. #76131
**	24-jul-96 (nick)
**	    Add DM1P_PHYSICAL to dm1p_getfree() call for system catalogs.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          dm1s_allocate(): Set DM0L_ROW_LOCK flag if row locking is enabled.
**          dm1s_allocate(): Call dm1r_allocate() instead of dmpp_allocate
**          dm1s_allocate(): If row locking, extend file in a mini transaction.
**      06-jan-97 (dilma04)
**          In dm1s_allocate() DM1P_PHYSICAL flag is needed only when calling
**          dm1p_getfree() to allocate system catalog page.
**      10-mar-97 (stial01)
**          dm1s_allocate() Pass record to dm1r_allocate
**      21-may-97 (stial01)
**          Added flags arg to dm0p_unmutex call(s).
**          No more LK_PH_PAGE locks, buffer locked with dm0p_lock_buf_macro()
**	25-Aug-1997 (jenjo02)
**	    Added log_id parm to dm0p_close_page() calls.
**	07-Oct-1997 (nanpr01)
**	    Concurrent insert on heap table with row locking locks each 
**	    other. Star issue 5733022.
**      21-apr-98 (stial01)
**          Set DM0L_PHYS_LOCK in log rec if extension table (B90677)
**	24-Jun-1998 (jenjo02)
**	    Removed 02-Jan-1996 (ramra01) change which looped until
**	    dm1p_lastused() returned the same "last used" page twice
**	    in a row, also Nick's 3-jun-96 change to take a LK_EXTEND_LOCK.
**	    The "last used" page is now discovered and fixed by 
**	    dm1p_lastused() while the FHDR/FMAP are fixed, guaranteeing
**	    that recovery will not have deallocated the page under the
**	    covers.
**      09-feb-1999 (stial01)
**          Init mct_kperleaf (not relevant for heap, but neither is kperpage)
**      09-Feb-99 (linke01)
**          rollforwarddb failed in be/lar test, added NO_OPTIM for pym_us5
**	16-aug-1999 (nanpr01)
**	    Implementation of SIR 92299. Enabaling journalling on a
**	    online checkpoint.
**	04-May-2000 (jenjo02)
**	    Pass blank-suppressed lengths of table/owner names to 
**	    dm0l_ovfl.
**	23-Jun-2000 (jenjo02)
**	    Added fill_option to dmpp_format() prototype, set by caller
**	    to DM1C_ZERO_FILL if dmpp_format should MEfill the page,
**	    DM1C_ZERO_FILLED if the page has already been MEfilled.
**	    The page overhead space is always zero-filled.
**      23-oct-2000 (stial01)
**          Variable Page Size - Variable Page Type support (SIR 102955)
**      30-oct-2000 (stial01)
**          Pass crecord to dm1s_allocate, Pass rcb to dm1r_allocate
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      01-feb-2001 (stial01)
**          Pass page type to dm1s_empty_table (Variable Page Types SIR 102955)
**      09-mar-2001 (stial01)
**          Changes for Variable Page Type SIR 102955
**	20-may-2001 (somsa01)
**	    In dm1s_allocate(), removed crossed fix for bug 101425, as we
**	    do not need to take an LK_EXTEND_FILE lock anymore in 2.5 .
**      10-jul-2003 (stial01)
**          Fixed row lock error handling during allocate (b110521)
**      15-sep-2003 (stial01)
**          Skip allocate mini trans for etabs if table locking (b110923)
**	12-Aug-2004 (schka24)
**	    Clean up some mct data item usage.
**      31-jan-05 (stial01)
**          Added flags arg to dm0p_mutex call(s).
**/


/*
**  Forward and/or External function references.
*/

/*{
** Name: dm1s_allocate - Allocates space for a new record. 
**
** Description:
**    This routine finds space for a new record, allocates it, and 
**    returns the tid.
**
** Inputs:
**      rcb                             Pointer to record access context
**                                      which contains table control 
**                                      block (TCB) pointer, tran_id,
**                                      and lock information associated
**                                      with this request.
**      record                          Pointer to record to insert.
**      crecord				Compressed record (if compression)
**      need                            Value indicating amount of space
**                                      needed, assumes compressed if necessary.
**      dupflag                         A flag indicating if duplicates must be
**                                      checked.  For SEQuential tables it
**                                      must be DM1C_NODUPLICATES.
**
** Outputs:
**      data                            Pointer toa an area used to return
**                                      pointer to the data page.
**      tid                             Pointer to an area to return TID
**                                      of record allocated.
**      err_code                        A pointer to an area to return error
**                                      codes if return status not E_DB_OK.
**                                      Error codes returned are:
**                                      E_DM003C_BAD_TID, E_DM0044_DELETED_TID.
**      
**	Returns:
**
**	    E_DB_OK
**          E_DB_ERROR
**          E_DB_FATAL
**
**	Exceptions:
**	    none
**
** Side Effects:
**          none.
**
** History:
**      13-dec-85 (jennifer)
**          Converted for Jupiter.
**	03-aug-88 (sandyh)
**	    bug # 3087 -- added check for not temp file to sense last page
**		to prevent fixing non-aligned pages in the heap which will
**		cause corruption in the buffer manager.
**      23-oct-91 (jnash) 
**	    Remove what was third parameter to dm0p_bicheck as part of 6.4 6.5
** 	    merge.
**	 3-nov-191 (rogerk)
**	    Added dm0l_ovfl log record during dm1s_allocate when a new overflow
**	    page is added.  This log record is used to REDO the operation of
**	    linking the new page into the overflow chain.
**	20-Nov-1991 (rmuth)
**	    If this is a system relation pass DM1P_NOREDO and DM1P_MINI_TRAN
**	    do not need to pass DM1P_PHYSICAL as this is set in the
**	    rcb's 'rcb_k_duration' field.
**	7-may-1992 (bryanp)
**	    Removed temp_alloc(). Temporary tables will use the same allocation
**	    mechanism as all other tables do.
**	09-jun-1992 (kwatts)
**	    6.5 MPF Changes. Replaced calls of dm1c_allocate with accessors.
**    	29-August-1992 (rmuth)
**        Pass the DM1P_LASTUSED_DATA to dm1p_lastused.
**	17-sep-1992 (jnash)
**	    6.5 Recovery Project
**	     -  Add new param's to dmpp_allocate call.
**	     -  Change pageno from i4 to DM_PAGENO type to quiet
**		compiler.
**	03-nov-1992 (jnash)
**	    Fill out parameter list to dmpp_allocate.  This is not
**	    really necessary as these param's are only used for system
**	    catalogs, but it is done for consistency.
**	14-dec-1992 (jnash & rogerk)
**	    6.5 Recovery Project
**	      - Changed to use new recovery protocols.  Before Images are
**		no longer written and the pages updated by the allocate must
**		be mutexed while the dm0l_ovfl log record is written and
**		the pages are updated.
**	      - Changed to pass new arguments to dm0l_ovfl.
**	      - Add new param's to dmpp_allocate call.
**	      - Change pageno from i4 to DM_PAGENO type to quiet compiler.
**	      - Removed requirement that file-extend operations on system
**		catalogs must be forced to the database.  Restriction not
**		needed anymore since system catalogs now do Redo Recovery.
**	      - Added dm0p_pagetype call.
**	      - Removed old DM1P_NOREDO action - all allocates are now redone.
**	05-jan-1993 (jnash)
**	    Reduced logging project.  Add LGreserve call.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**		Improve error handling and reporting.
**	26-jul-1993 (rogerk)
**	    Change generation of Before Image log records during Online Backup.
**	    All page modifiers must now call dm0p_before_image_check before
**	    changing the page to implement online backup protocols.
**	20-sep-1993 (jnash)
**	    Add flag param to LGreserve() call.
**	02-Jan-1996 (ramra01)
**	    b72963: dm1s_allocate, ends up in an infinite loop when it
**	    attempts to fix a page while there is another process busy
**	    going thru an UNDO operation dealing with the same FMAP.
**	    The page that gets fixed is no longer the last page.
**	06-mar-1996 (stial01 for bryanp)
**	    Pass tcb_rel.relpgsize as page_size argument to dmpp_allocate.
**	06-may-1996 (nanpr01)
**	    Change all page header access as macro for New Page Format 
**	    project.
**	 3-jun-96 (nick)
**	    Use LK_EXTEND_FILE to ensure we always allocate the next 
**	    page in the file ; rollback of allocations in OpenIngres
**	    could lead to free pages which weren't at the end of the
**	    structure ( which is illegal ). #76131
**	24-jul-96 (nick)
**	    Pass DM1P_PHYSICAL for system catalogs.  Don't need to take 
**	    extend lock if we're table locking.
**	12-nov-1996 (nanpr01)
**	    Pass the timeout value for lock request. Also check for LK_TIMEOUT
**	    status and return appropriate error.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Set DM0L_ROW_LOCK flag if row locking is enabled.
**          Call dm1r_allocate() instead of dmpp_allocate
**          If row locking, extend file in a mini transaction.
**      06-jan-97 (dilma04)
**          DM1P_PHYSICAL flag is needed only when calling dm1p_getfree() to
**          allocate system catalog page.
**      10-mar-97 (stial01)
**          Pass record to dm1r_allocate
**      21-may-97 (stial01)
**          dm1s_allocate() Added flags arg to dm0p_unmutex call(s).
**          No more LK_PH_PAGE locks, buffer locked with dm0p_lock_buf_macro()
**          Note we look at page_page when the buffer is not locked,
**          since it is only set when the page is formatted.
**	07-Oct-1997 (nanpr01)
**	    Concurrent insert on heap table with row locking locks each 
**	    other. Star issue 5733022.
**      21-apr-98 (stial01)
**          dm1s_allocate() Set DM0L_PHYS_LOCK if extension table (B90677)
**	24-Jun-1998 (jenjo02)
**	    Removed 02-Jan-1996 (ramra01) change which looped until
**	    dm1p_lastused() returned the same "last used" page twice
**	    in a row, also Nick's 3-jun-96 change to take a LK_EXTEND_LOCK.
**	    The "last used" page is now discovered and fixed by 
**	    dm1p_lastused() while the FHDR/FMAP are fixed, guaranteeing
**	    that recovery will not have deallocated the page under the
**	    covers.
**      22-sep-98 (stial01)
**          dm1s_allocate() If row locking, downgrade/convert lock on new page  
**          after mini transaction is done (B92908, B92909)
**      18-feb-99 (stial01)
**          dm1s_allocate() pass buf_locked to dm1r_allocate
**	04-May-2000 (hanal04) Bug 101425 INGSRV 1165.
**	    Replaced E_DM901C with TRdisplay() when we get LK_DEADLOCK
**	    from LKrequest(). LK_DEADLOCK is a known status and does not
**	    require worrying error messages.
**	20-may-2001 (somsa01)
**	    Removed crossed fix for bug 101425, as we do not need to take
**	    an LK_EXTEND_FILE lock anymore in 2.5 .
*/
DB_STATUS
dm1s_allocate(
DMP_RCB        *rcb,
DMPP_PAGE      **data,
DM_TID         *tid,
char           *record,
char	       *crecord,
i4        need,
i4        dupflag,
i4        *err_code)
{
    DMP_RCB	*r = rcb;					
    DMP_TCB	*t = r->rcb_tcb_ptr;
    DMP_DCB	*d = t->tcb_dcb_ptr;
    DMP_TABLE_IO    *tbio = &t->tcb_table_io;
    CL_ERR_DESC	sys_err;
    DMPP_PAGE   *datapage;
    DMPP_PAGE	*newpage = 0;
    DB_STATUS   s;		    /* Status return from called routines. */
    DB_STATUS	local_status;
    i4     local_err_code;                                  
    STATUS      status;
    i4	loc_id;
    i4	config_id;
    i4	ovf_loc_id;
    i4	ovf_config_id;
    i4	dm0l_flag;
    DM_PAGENO   ovflpageno;
    bool        buf_locked = FALSE;
    DM_PAGENO	lastused_pageno;
    DM_PAGENO	datapage_pageno;
    DM_PAGENO	newpage_pageno;
    LK_LKID     new_lkid;
    bool	in_mini_trans = FALSE;
            
    if (dupflag == DM1C_ONLYDUPCHECK)
	return (E_DB_OK);

    if (datapage = *data)
    {
	if (row_locking(r))
	{
	    dm0p_lock_buf_macro(tbio, r->rcb_lk_id, data);
	    buf_locked = TRUE;
	}

	/* Get ovfl page while the buffer is locked */
	ovflpageno = 
	    DMPP_VPT_GET_PAGE_OVFL_MACRO(t->tcb_rel.relpgtype, datapage);

	/*  Check whether current page has enough space. */
	s = dm1r_allocate(r, datapage,
			t->tcb_rel.relcomptype == TCB_C_NONE ? record : crecord,
			need, tid, &buf_locked, err_code);

	if (buf_locked)
	{
	    dm0p_unlock_buf_macro(tbio, r->rcb_lk_id, data);
	    buf_locked = FALSE;
	}

	if (s == E_DB_OK || s == E_DB_ERROR)
	    return (s);

	/*
	** dm1r_allocate() returned E_DB_INFO:
	** Toss current page if it's not the end of the chain.
	*/
	if (ovflpageno)
	{
	    s = dm0p_unfix_page(r, DM0P_UNFIX, data, err_code);
	    if (s != E_DB_OK)
		return (s);
	}
    }

    for (;;)
    {
	LG_LSN		bm_lsn;
	LG_LSN		lsn;
	i4		dm1p_flag = 0;

	if (*data == 0)
	{
	    /*
	    ** We have no current page.
	    ** Locate and fix for WRITE the last used page in the file.
	    */
	    s = dm1p_lastused( r, (u_i4)DM1P_LASTUSED_DATA,
			       &lastused_pageno, data, err_code);

	    if (s != E_DB_OK)
		break;
                
	    datapage = *data;

	    if (row_locking(r))
	    {
		dm0p_lock_buf_macro(tbio, r->rcb_lk_id, data);
		buf_locked = TRUE;
	    }

	    /*	Check space on this page. */
	    s = dm1r_allocate(r, datapage,
		    t->tcb_rel.relcomptype == TCB_C_NONE ? record : crecord,
		    need, tid, &buf_locked, err_code);

	    if (s == E_DB_ERROR)
		break;

	    if (s == E_DB_OK)
	    {
		if (buf_locked)
		{
		    dm0p_unlock_buf_macro(tbio, r->rcb_lk_id, data);
		    buf_locked = FALSE;
		}
		return (s);
	    }
	}
	else
	{
	    if (row_locking(r))
	    {
		dm0p_lock_buf_macro(tbio, r->rcb_lk_id, data);
		buf_locked = TRUE;
	    }
	}

	/*
	**  Pages are only added to the last page in the table.  Make sure
	**  that we a sitting on the last page.
	*/

	if (DMPP_VPT_GET_PAGE_OVFL_MACRO(t->tcb_rel.relpgtype, datapage))
	{
	    /*
	    **  Not last page of table, re-sense the last page and
	    **  start over again.  Could also just follow the page_ovfl
	    **	pointer but this can be a long list in the worst case.
	    */

	    if (buf_locked)
	    {
		dm0p_unlock_buf_macro(tbio, r->rcb_lk_id, data);
		buf_locked = FALSE;
	    }

	    s = dm0p_unfix_page(r, DM0P_UNFIX, data, err_code);
	    if (s != E_DB_OK)
		break;
	    continue;
	}

	/*
	** No space was found on the last page of the Heap - we must allocate
	** a new page and link it onto the heap overflow chain.
	**
	** Allocations for system catalogs are always done within Mini
	** Transactions since the page locks will be released as soon as
	** the current update is complete.
	*/

	if ((t->tcb_sysrel == TCB_SYSTEM_REL) || row_locking(r) ||
		(t->tcb_extended && r->rcb_lk_type != RCB_K_TABLE)) 
	{
	    if ( r->rcb_logging )
	    {
		s = dm0l_bm(r, &bm_lsn, err_code);
		if (s != E_DB_OK)
		    break;
		in_mini_trans = TRUE;
	    }

	    /*
	    ** Set flags to allocate call to indicate that the action is
	    ** part of a mini transaction.
	    */
            dm1p_flag |= DM1P_MINI_TRAN;
            if (t->tcb_sysrel == TCB_SYSTEM_REL 
		|| (t->tcb_extended && r->rcb_lk_type != RCB_K_TABLE))
                dm1p_flag |= DM1P_PHYSICAL;
	}

	/*
	** Call allocation code to get a free page from the free maps.
	** This will possibly trigger a File Extend as well.
	**
	** dm1p_getfree() guarantees the atomicity of file extends.
	*/
	s = dm1p_getfree(r, dm1p_flag, &newpage, err_code );
	if (s != E_DB_OK)
	    break;

	/* Save lockid for new page */
	STRUCT_ASSIGN_MACRO(r->rcb_fix_lkid, new_lkid);

	/* Inform buffer manager of new page's type */
	dm0p_pagetype(tbio, newpage, r->rcb_log_id, DMPP_DATA);

	/* Extract datapage and newpage page numbers to local variables */
	newpage_pageno  = DMPP_VPT_GET_PAGE_PAGE_MACRO(t->tcb_rel.relpgtype, newpage);
	datapage_pageno = DMPP_VPT_GET_PAGE_PAGE_MACRO(t->tcb_rel.relpgtype, datapage);

	/*
	** Log operation of adding new page on the overflow chain.
	** The Log Address of the OVERFLOW record is recorded on both
	** the new and old data pages.
	**
	** Note that currently heap overflow pages are always linked to
	** the end of the overflow chain.  This code depends on that
	** fact since the new page's overflow pointer is left NIL.
	*/
	if (r->rcb_logging)
	{
	    /*
	    ** We only need BIs if the database is undergoing backup and
	    ** logging is enabled.
	    */
	    if (d->dcb_status & DCB_S_BACKUP)
	    {
		/*
		** Online Backup Protocols: Check if BIs must be logged before update.
		*/
		s = dm0p_before_image_check(r, datapage, err_code);
		if (s == E_DB_OK)
		    s = dm0p_before_image_check(r, newpage, err_code);
		if (s != E_DB_OK)
		    break;
	    }

	    /*
	    ** Get information on the locations of the updates for the 
	    ** log call below.
	    */
	    loc_id = DM2F_LOCID_MACRO(tbio->tbio_loc_count, datapage_pageno);
	    config_id = tbio->tbio_location_array[loc_id].loc_config_id;

	    ovf_loc_id = DM2F_LOCID_MACRO(tbio->tbio_loc_count, newpage_pageno);
	    ovf_config_id = tbio->tbio_location_array[ovf_loc_id].loc_config_id;

	    /* Reserve space for overflow and its clr */
	    status = LGreserve(0, r->rcb_log_id, 2, sizeof(DM0L_OVFL) * 2,
		&sys_err);
	    if (status != OK)
	    {
		(VOID) ule_format(status, &sys_err, ULE_LOG, NULL,
		    (char *)0, 0L, (i4 *)0, err_code, 0);
		(VOID) ule_format(E_DM9054_BAD_LOG_RESERVE, &sys_err,
		    ULE_LOG, NULL, (char *)0, 0L, (i4 *)0,
		    err_code, 1, 0, r->rcb_log_id);
		*err_code = E_DM9245_DM1S_ALLOCATE;
		break;
	    }

	    dm0l_flag = (is_journal(r) ? DM0L_JOURNAL : 0);

	    /* 
	    ** We use temporary physical locks for catalogs and extension
	    ** tables. The same lock protocol must be observed during recovery.
	    */
	    if (t->tcb_sysrel || 
		    (t->tcb_extended && r->rcb_lk_type != RCB_K_TABLE))
		dm0l_flag |= DM0L_PHYS_LOCK;
	    else if (row_locking(r))
		dm0l_flag |= DM0L_ROW_LOCK;

	    s = dm0l_ovfl(r->rcb_log_id, dm0l_flag, FALSE,
		&t->tcb_rel.reltid, 
		&t->tcb_rel.relid, t->tcb_relid_len,
		&t->tcb_rel.relowner, t->tcb_relowner_len,
		t->tcb_rel.relpgtype,
		t->tcb_rel.relpgsize,
		t->tcb_rel.relloccount, config_id, ovf_config_id,
		newpage_pageno,
		datapage_pageno,
		DMPP_VPT_GET_PAGE_OVFL_MACRO(t->tcb_rel.relpgtype, datapage),
		DMPP_VPT_GET_PAGE_MAIN_MACRO(t->tcb_rel.relpgtype, datapage),
		(LG_LSN *)0, &lsn, err_code);
	    if (s != E_DB_OK)
		break;
	}

	/* Mutex both of the pages */
	dm0p_mutex(tbio, (i4)0, r->rcb_lk_id, data);
	dm0p_mutex(tbio, (i4)0, r->rcb_lk_id, &newpage);

	/*
	** Link the new page onto the overflow chain of the current last page.
	*/
	DMPP_VPT_SET_PAGE_OVFL_MACRO(t->tcb_rel.relpgtype, datapage, newpage_pageno);

	DMPP_VPT_SET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype, datapage, DMPP_MODIFY);
	DMPP_VPT_SET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype, newpage, 
				(DMPP_MODIFY | DMPP_OVFL));

	if (r->rcb_logging)
	{
	    DMPP_VPT_SET_PAGE_LOG_ADDR_MACRO(t->tcb_rel.relpgtype, datapage, lsn);
	    DMPP_VPT_SET_PAGE_LOG_ADDR_MACRO(t->tcb_rel.relpgtype, newpage, lsn);
	}

	/*
	** Unmutex the buffers after completing the updates.
	*/
	dm0p_unmutex(tbio, (i4)0, r->rcb_lk_id, data);
	dm0p_unmutex(tbio, (i4)0, r->rcb_lk_id, &newpage);

	/*
	** If this is a system catalog allocation, then complete the
	** Mini Transaction used for the allocate.  After this, the
	** new page becomes a permanent addition to the table.
	*/
	if (in_mini_trans)
	{
	    s = dm0l_em(r, &bm_lsn, &lsn, err_code);
	    if (s != E_DB_OK)
		break;
	}

	/*
	** Now that the mini transaction is complete
	** it is safe to unlock the old last page
	*/
	if (buf_locked)
	{
	    dm0p_unlock_buf_macro(tbio, r->rcb_lk_id, data);
	    buf_locked = FALSE;
	}

	/*
	** Unfix the old last page and allocate space off of the newly
	** added page.
	*/
	s = dm0p_unfix_page(r, DM0P_UNFIX, data, err_code);
	if (s != E_DB_OK)
	    break;

	*data = newpage;

	/*
	** Allocate space on new page while we still hold X lock on
	** new data page
	*/
	s = dm1r_allocate(r, newpage,
			t->tcb_rel.relcomptype == TCB_C_NONE ? record : crecord,
			need, tid, &buf_locked, err_code);

	/*
	** Even if allocate failed, do the lock conversion below in
	** case we need to rollback 
	*/
	if (row_locking(r))
	{
	    local_status = dm1r_lk_convert(r, 
		DMPP_VPT_GET_PAGE_PAGE_MACRO(t->tcb_rel.relpgtype, newpage),
		&new_lkid, &local_err_code);
	    if (local_status != E_DB_OK)
	    {
		if (s == E_DB_OK)
		{
		    s = local_status;
		    *err_code = local_err_code;
		}
		else
		{
		    ule_format(local_err_code, 0, ULE_LOG, NULL, 0, 0, 0,
				&local_err_code, 0);
		}
	    }
	}

	if (s != E_DB_OK)
	    break;

	return (s);
    }

    /*	Handle error reporting. */

    /* Don't leave page fixed in local buffer */
    if (buf_locked)
    {
	dm0p_unlock_buf_macro(tbio, r->rcb_lk_id, data);
	buf_locked = FALSE;
    }

    if (newpage)
    {
	s = dm0p_unfix_page(r, DM0P_UNFIX, &newpage, &local_err_code);
	if (s)
	    ule_format(local_err_code, 0, ULE_LOG, NULL, 0, 0, 0,
			&local_err_code, 0);
    }
    if (*data)
    {
	s = dm0p_unfix_page(r, DM0P_UNFIX, data, &local_err_code);
	if (s)
	    ule_format(local_err_code, 0, ULE_LOG, NULL, 0, 0, 0,
			&local_err_code, 0);
    }

    if (*err_code >= E_DM_INTERNAL)
    {
	ule_format(*err_code, 0, ULE_LOG, NULL, 0, 0, 0, err_code, 0);
	*err_code = E_DM9245_DM1S_ALLOCATE;
    }
    return (E_DB_ERROR);
}

/*{
** Name: dm1s_deallocate - Logically deallocates data from temporary.
**
** Description:
**    This routine resets the logical end of file to 0 for 
**    termporary file only.
**
** Inputs:
**      rcb                             Pointer to record access context
**                                      which contains table control 
**                                      block (TCB) pointer, tran_id,
**                                      and lock information associated
**                                      with this request.
** Outputs:
**      err_code                        A pointer to an area to return error
**                                      codes if return status not E_DB_OK.
**                                      Error codes returned are:
**                                      E_DM009F_ILLEGAL_OPERATION
**      
**	Returns:
**
**	    E_DB_OK
**          E_DB_ERROR
**          E_DB_FATAL
**
**	Exceptions:
**	    none
**
** Side Effects:
**          none.
**
** History:
**      13-dec-85 (jennifer)
**          Converted for Jupiter.
**	7-may-1992 (bryanp)
**	    Rewrote to call build routines so that we work properly with the
**	    new extend model.
**	15-jul-1992 (bryanp)
**	    Set mct_rebuild when emptying an existing temp table using the
**	    build routines. This causes the build routines to query the current
**	    size of the file so that the resulting fhdr's and fmap's match
**	    reality.
**	09-jun-1992 (kwatts)
**	    Set up new mct_data_atts instead of mct_atts_ptr (removed).
**      xx-aug-1992 (rmuth)
**          Initialise some iirelation fields which we cannot know until
**          after the table has been built, ie relfhdr and relpages.
**	11-apr-1994 (rmuth)
**	    b62263: Only tossed pages from the buffer cache if in the
**	    table is in deferred I/O mode. Non-deffered I/O tables can
**	    also have pages in the buffer cache and these need to be 
**	    tossed aswell.
**	06-mar-1996 (stial01 for bryanp)
**	    Set mct_page_size to tcb_rel.relpgsize.
**	    In dm1s_deallocate, check status as well as error_code when logging
**		errors.
**	06-may-1996 (nanpr01)
**	    Initialize the mct->mct_acc_plv and mct_acc_tlv.
**      09-feb-1999 (stial01)
**          Init mct_kperleaf (not relevant for heap, but neither is kperpage)
*/
DB_STATUS
dm1s_deallocate(
DMP_RCB        *rcb,
i4        *err_code)
{
    DMP_RCB	*r = rcb;
    DMP_TCB	*t;
    DB_STATUS   status = E_DB_OK;
    DM2U_M_CONTEXT	mctarea;
    DM2U_M_CONTEXT	*mct = &mctarea;

    for (;;)
    {
	t = r->rcb_tcb_ptr;

	if (t->tcb_temporary == 0)
	{
	    *err_code = E_DM009F_ILLEGAL_OPERATION;
	    return (E_DB_ERROR);
	}
	t->tcb_table_io.tbio_lpageno = 0;
	if (t->tcb_loadfile)
	{
	    /* Reset rcb to indicate not loaded. */

	    r->rcb_state &= ~(RCB_LSTART | RCB_LEND);
	    r->rcb_state |= RCB_LRESTART;
	}	    

	/* Build all the mct information for manipulating table. */

	MEfill(sizeof(*mct), (u_char)'\0', (PTR)mct);
	mct->mct_oldrcb = rcb;
	mct->mct_buildrcb = rcb;
	mct->mct_location = t->tcb_table_io.tbio_location_array;
	mct->mct_loc_count = t->tcb_table_io.tbio_loc_count;
	mct->mct_structure = t->tcb_rel.relspec;
	mct->mct_data_atts = t->tcb_data_atts;
	mct->mct_key_atts = t->tcb_key_atts;
	mct->mct_relatts = t->tcb_rel.relatts;
	mct->mct_relwid = t->tcb_rel.relwid;
	mct->mct_relpages = 0;
	mct->mct_buffer = (PTR)0;
	mct->mct_keys = t->tcb_keys;
	mct->mct_klen = t->tcb_klen;
	mct->mct_kperpage = t->tcb_kperpage;
	mct->mct_kperleaf = t->tcb_kperleaf;
	mct->mct_compressed = t->tcb_rel.relcomptype;
	mct->mct_index = ((t->tcb_rel.relstat & TCB_INDEX) != 0);
	mct->mct_unique = ((t->tcb_rel.relstat & TCB_UNIQUE) != 0);
	mct->mct_d_fill = t->tcb_rel.reldfill;
	mct->mct_i_fill = t->tcb_rel.relifill;
	mct->mct_l_fill = t->tcb_rel.rellfill;
	mct->mct_buckets = t->tcb_rel.relprim;
	mct->mct_heapdirect = 0;
	mct->mct_recreate = 0;
	mct->mct_override_alloc = 0;
	mct->mct_allocation = t->tcb_rel.relallocation;
	mct->mct_extend = t->tcb_rel.relextend;
	mct->mct_page_type = t->tcb_rel.relpgtype;
	mct->mct_page_size = t->tcb_rel.relpgsize;
	mct->mct_acc_plv = t->tcb_acc_plv;
	mct->mct_acc_tlv = t->tcb_acc_tlv;

	mct->mct_index_atts	    = t->tcb_key_atts;
	mct->mct_index_attcount = t->tcb_keys;
	mct->mct_index_comp = ((t->tcb_rel.relstat & TCB_INDEX_COMP) !=0 );

	/*
	** This temporary table is still in memory -- throw away its
	** current pages and build a new table in memory
	*/
	if (r->rcb_data_page_ptr)
	{
	    status = dm0p_unfix_page( r, DM0P_UNFIX, &r->rcb_data_page_ptr, 
				      err_code);
	    if (status)
	        break;
	}
	if (r->rcb_other_page_ptr)
	{
	    status = dm0p_unfix_page( r, DM0P_UNFIX, &r->rcb_other_page_ptr, 
				      err_code);
	    if (status)
	        break;
	}

	/*
	** Toss pages in the buffer cache before rebuild table
	*/
	status = dm0p_close_page(t, r->rcb_lk_id, r->rcb_log_id, 
				 (i4)0, err_code);
	if (status != E_DB_OK)
		break;

	if (t->tcb_table_io.tbio_location_array[0].loc_fcb->fcb_state &
				FCB_DEFERRED_IO)
	{
	    mct->mct_inmemory = 1;
	    mct->mct_rebuild = MCT_REBUILD;
	}
	else
	{
	    /*
	    ** this temporary table has a disk file; rebuild over the top of
	    ** the existing disk file
	    */
	    mct->mct_inmemory = 0;
	    mct->mct_rebuild = MCT_REBUILD | MCT_USEFHDR;
	}

	/* Now call build routines to rebuild empty table. */

	status = dm1sbbegin(mct, err_code);
	if (status != E_DB_OK)
	    break;

	status = dm1sbend(mct, err_code);
	if (status != E_DB_OK)
	    break;

        /* Tables is now built so init iirelation information that we
        ** do not know until after the table is built.
        */
        t->tcb_rel.relfhdr = mct->mct_fhdr_pageno;
        t->tcb_rel.relpages = mct->mct_relpages;

	break;
    } 

    /*	Handle error reporting. */

    if (status != E_DB_OK)
    {
	if (*err_code >= E_DM_INTERNAL)
	{
	    ule_format(*err_code, 0, ULE_LOG, NULL, 0, 0, 0, err_code, 0);
	    *err_code = E_DM9246_DM1S_DEALLOCATE;
	}
    }
    return (status);
}

/*{
** Name: dm1s_search - Sets the scan limits for a SEQuential table.
**
** Description:
**      This routine sets the serach limits for a scan of a SEQuential table.
**      The low and high tids for the scan are place in the RCB.
**      These are then used to determine which records to retrieve when 
**      get next is called.
**
** Inputs:
**      rcb                             Pointer to record access context
**                                      which contains table control 
**                                      block (TCB) pointer, tran_id,
**                                      and lock information associated
**                                      with this request.
**      data                            Pointer to area to contain pointer
**                                      to data page fixed.  Not used for
**                                      SEQuentail tables.
**      key				Not used for SEQuential tables.
**      keys_given                      Not used for SEQuential tables.
**      mode                            Must be set to DM1C_EXTREME for
**                                      SEQuential tables.
**      direction                       A valfue indicating the direction of
**                                      the search.  Must be DM1C_PREV for
**                                      first or DM1C_NEXT for last.
**
** Outputs:
**      tid                             A pointer to an area to return
**                                      the tid meeting the search criteria.
**      err_code                        A pointer to an area to return error
**                                      codes if return status not E_DB_OK.
**                                      Error codes returned are:
**                                      E_DM0055_NONEXT, E_DM003C_BAD_TID,
**                                      E_DB0044_DELETED_TID.
**                    
**	Returns:
**
**	    E_DB_OK
**          E_DB_ERROR
**          E_DB_FATAL
**
**	Exceptions:
**	    none
**
** Side Effects:
**          none
**
** History:
**      13-dec-85 (jennifer)
**          Converted for Jupiter.
*/
DB_STATUS
dm1s_search(
DMP_RCB         *rcb,
DMPP_PAGE       **data,
char            *key,
i4         keys_given,
i4         mode,
i4         direction,
DM_TID          *tid,
i4         *err_code)
{
    tid->tid_i4 = -1;
    if (direction == DM1C_PREV)
    {   
	tid->tid_tid.tid_page = 0;
        tid->tid_tid.tid_line = DM_TIDEOF;
    }
    return (E_DB_OK); 
}

/*{
** Name: dm1s_empty_table  - Create an initialized heap table.
**
** Description:
**
**	This routine is used by dm2u_create and table recovery routines
**	to create an empty, initialized heap table.  It is a replacement
**	for dm2f_init_file.  
**
**	The use of this routine for normal table builds is not without
**	controversy.  The preferred approach would be to use the normal 
**	build routines, however they are not be easily adapted for
**	use by recovery.  On the other hand, the use of the same routine 
**	in online and recovery circumstances is consistent with page-
**	oriented recovery algorithms.
**
**	This routine initializes the table's physical files, including 
**	some number of data pages, one FHDR and some number of FMAPs.
**	Is is assumed that the created tables are heaps.
**
**	On input to the routine, the set of files to be initialized are
**	expected to already exist and to be opened via the file descriptors
**	contained in the location array passed in.  These files are left
**	open on exit from the routine.
**
**	Location information is conveyed to this routine by the
**	loc_array input parameter.  This array of DMP_LOCATION
**	entries includes a possible "LOC_UNAVAIL" status in the loc_status
**	field.  Locations that are unavailable (e.g. partial recovery)
**	must be so recorded in this field. 
**
** Inputs:
**	dcb				Pointer to dcb
**	tbl_id				Pointer to table id
**	allocation			File allocation amount
**	loc_plv				Page level vector
**	fhdr_pageno			FHDR page number
**	first_fmap			First fmap page number
**	last_fmap			Last fmap page number
**	first_free			First free page number
**	last_free			Last free page number 
**	first_used			First used page number
**	last_used			Last used page number
**	page_size			Page size for this table.
**	loc_count			Number of locations in table
**	loc_array			Pointer to array of location info
**
** Outputs:
**
**	err_code			Returned error code
**
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
**	18-nov-1992 (jnash)
**	    Created for the reduced logging project.
**	08-dec-1992 (rogerk)
**	    Changed to use DM2F instead of direct DI calls.
**	    Added call to guarantee space before closing the table.
**	    Removed DMPP_ACC_PLV argument from dm2f_write_file.
**	26-jul-1993 (jnash)
**	    Fix multi-location recovery problem by dynamically allocating
**	    FCB memory.  Add build_fcb_flag param, add new error message.
**	23-aug-1993 (rogerk)
**	  - Took creating of table files out of dm1s_empty_table routine.  The
**	    files are now created by dm2u_file_create.
**	  - Removed the lock_id, sync_flag and build_fcb_flag parameters.
**	18-apr-1994 (jnash)
**	    fsync project.  Eliminate calls to dm2f_force_file().
**	06-mar-1996 (stial01 for bryanp)
**	    In dm1s_empty_table, don't allocate pages on the stack. Instead,
**		allocate them from dynamic memory.
**	    Added page_size argument.
**	    Pass page_size as the page_size argument to dmpp_format.
**	06-may-1996 (nanpr01 & thaju02)
**	    Changed all page header access as macro for New Page Format 
**	    project.
**	    Pass added parameter page_size in dm1p_init_fhdr, dm1p_fminit,
**	    dm1p_fmfree routines for New Page Format project.
**	21-may-1996 (thaju02)
**	    Pass page size to SET_FREE_HINT_MACRO, MAP_FROM_NUMBER_MACRO
**	    for 64 bit tid support and multiple fhdr pages.
**	15-jan-1999 (nanpr01)
**	    Pass ptr to ptr to dm0m_deallocate routine.
**	23-Jun-2000 (jenjo02)
**	    Added fill_option to dmpp_format() prototype, set by caller
**	    to DM1C_ZERO_FILL if dmpp_format should MEfill the page,
**	    DM1C_ZERO_FILLED if the page has already been MEfilled.
**	    The page overhead space is always zero-filled.
*/
DB_STATUS
dm1s_empty_table(
DMP_DCB		*dcb,  
DB_TAB_ID	*tbl_id,  
i4		allocation,
DMPP_ACC_PLV	*loc_plv,
DM_PAGENO	fhdr_pageno,
DM_PAGENO	first_fmap,
DM_PAGENO	last_fmap,
DM_PAGENO	first_free,
DM_PAGENO	last_free,
DM_PAGENO	first_used,
DM_PAGENO	last_used,
i4		page_type,
i4		page_size,
i4		loc_count,
DMP_LOCATION	*loc_array,
i4		*err_code)
{
    DB_STATUS		status = E_DB_OK;
    i4		i;
    DMPP_PAGE           *data;
    DM1P_FHDR		*fhdr;
    DM1P_FMAP		*fmap;
    DM_PAGENO		new_fmap;
    i4	        page;
    i4		alloc_per_file;
    i4		io_count;
    DM_PAGENO		fmap_pageno;
    DM_PAGENO		data_pageno;
    DM_PAGENO		fmap_next;
    DMP_LOCATION	*loc;
    i4 		loc_id;
    i4 		num_fmaps;
    i4 		num_data_pages;
    DM_PAGENO 		ffree;
    DM_PAGENO 		lfree;
    DM_PAGENO 		fused;
    DMP_MISC		*misc_pages = 0;

    for (;;)
    {
	/*
	** Allocate space for a DATA page, a FHDR page, and a FMAP page:
	*/
	status = dm0m_allocate( (3 * page_size) + sizeof(DMP_MISC),
			(i4)0, (i4)MISC_CB, (i4)MISC_ASCII_ID,
			(char *)0, (DM_OBJECT **)&misc_pages, err_code);

	if ( status != E_DB_OK )
	    break;

    	/*
	** Initialize various page pointers.
	*/
	data = (DMPP_PAGE *)((char *)misc_pages + sizeof(DMP_MISC));
    	misc_pages->misc_data = (char *)data;
	fhdr = (DM1P_FHDR *)((char *)data + page_size);
	fmap = (DM1P_FMAP *)((char *)fhdr + page_size);

	/*
	** Create appropriate space in each file on each location.
	*/
	for (i = 0; i < loc_count; i++)
	{
	    loc = &loc_array[i];

	    /* 
	    ** Skip if location not available (should happen only in 
	    ** recovery circumstance.
	    */
	    if (loc->loc_status & LOC_UNAVAIL)
		continue;

	    /* 
	    ** Calculate size of file at this location, allocate space
	    */
	    alloc_per_file = dm2f_file_size(allocation, loc_count, i);

	    /*
	    ** Allocate the space for the table - we use dm2f_galloc rather
	    ** than dm2f_alloc since we cannot do a single dm2f_guarantee
	    ** space at the end on a partially open table (although we could
	    ** make separate dm2f_guarantee calls for each location).
	    */
	    page = 0;
	    status = dm2f_galloc_file(loc, (i4) 1, 
			    (DB_TAB_NAME *) DB_NOTAVAIL_NAME, &dcb->dcb_name, 
			    alloc_per_file, &page, err_code);
	    if (status != E_DB_OK)
		break;

	    /*
	    ** Verify that enough space was allocated.
	    */
	    if (page < (alloc_per_file - 1))
	    {
	   	TRdisplay("dm1s_empty_table: insufficient space allocated.\n");
	 	*err_code = E_DM9000_BAD_FILE_ALLOCATE;
	 	status = E_DB_ERROR;
		break;
	    }
	}
	if (status != E_DB_OK)
	    break;

	/*
	** Format and write some number of data pages, assumes data
	** page numbers are contiguous.
	**
	** THIS CODE ASSUMES THAT the initialized pages are always written
	** to the 1st location array by only forcing and flushing the first
	** of the locations created above.
	*/
	num_data_pages = last_used - first_used + 1;

	for (i = 0; i < num_data_pages; i++)
	{
	    data_pageno = first_used + i;
            loc_id = DM2F_LOCID_MACRO(loc_count, data_pageno);
	    loc = &loc_array[loc_id];

	    if (loc->loc_status & LOC_UNAVAIL)
		continue;

	    /*
	    ** Format and init empty DATA page at location specified.
	    **
	    ** It only needs to be zero-filled once.
	    */
	    (*loc_plv->dmpp_format)(page_type, page_size, data, data_pageno,
		(DMPP_DATA | DMPP_MODIFY),
		(i) ? DM1C_ZERO_FILLED : DM1C_ZERO_FILL );

	    io_count = 1;
	    status = dm2f_write_file(loc, (i4) 1, page_size, 
		(DB_TAB_NAME *) DB_NOTAVAIL_NAME, &dcb->dcb_name, &io_count,
		(i4) DMPP_VPT_GET_PAGE_PAGE_MACRO(page_type, data),
		(char *) data, err_code);
	    if (status != E_DB_OK)
		break;
	}
	if (status != E_DB_OK)
	    break;

	/*
	** Map the FHDR pageno to location, create if loc available.
	*/
	num_fmaps = last_fmap - first_fmap + 1;
	loc_id = DM2F_LOCID_MACRO(loc_count, fhdr_pageno);
	loc = &loc_array[loc_id];

	if ((loc->loc_status & LOC_UNAVAIL) == 0)
	{
	    /*
	    **  Format the File Header page
	    */
	    (*loc_plv->dmpp_format)(page_type, page_size, (DMPP_PAGE *)fhdr,
		fhdr_pageno, DMPP_MODIFY, DM1C_ZERO_FILL);

            dm1p_init_fhdr(fhdr, &fmap_next, page_type, page_size);

	    /*
	    ** FMAP hints are updated with dm1p_fmused and dm1p_fmfree,
	    ** FHDR hints need to be updated here.  We assume that
	    ** the FREE hint is set automatically when a new page is
	    ** formatted, but needs to be reset if all of bits in an FMAP 
	    ** are used.
	    */

	    /* 
	    ** This code should operate in concert with dm1p code that
	    ** does the same thing.  However, dm1p cannot currently 
	    ** operate on single pages, so it must be modified.
	    ** For now, we hard code things to work.
	    */

	    DM1P_VPT_SET_FHDR_PAGES_MACRO(page_type, fhdr, allocation);
	    DM1P_VPT_SET_FHDR_COUNT_MACRO(page_type, fhdr, 1);
	    DM1P_VPT_SET_FHDR_HWMAP_MACRO(page_type, fhdr, 1); 
	    VPS_MAP_FROM_NUMBER_MACRO(page_type, 
		    (DM1P_VPT_ADDR_FHDR_MAP_MACRO(page_type,fhdr,0)),
		    (i4) first_fmap); 
	    VPS_SET_FREE_HINT_MACRO(page_type, 
		    (DM1P_VPT_GET_FHDR_MAP_MACRO(page_type, fhdr, 0)));

	    io_count = 1;
	    status = dm2f_write_file(loc, (i4) 1, page_size, 
		(DB_TAB_NAME *) DB_NOTAVAIL_NAME, &dcb->dcb_name, &io_count,
		(i4) DM1P_VPT_GET_FHDR_PAGE_PAGE_MACRO(page_type, fhdr), 
		(char *) fhdr, err_code);
	    if (status != OK)
		break;
	}

	/*
	** Format and write some number of fmaps, assumes fmap
	** page numbers are contiguous.
	*/
	for (i = 0; i < num_fmaps; i++)
	{
	    fmap_pageno = first_fmap + i;
            loc_id = DM2F_LOCID_MACRO(loc_count, fmap_pageno);
	    loc = &loc_array[loc_id];

	    if (loc->loc_status & LOC_UNAVAIL)
		continue;

	    (*loc_plv->dmpp_format)(page_type, page_size, (DMPP_PAGE *)fmap,
		fmap_pageno, DMPP_MODIFY, DM1C_ZERO_FILL);

	    dm1p_fminit(fmap, page_type, page_size);
	    DM1P_VPT_SET_FMAP_SEQUENCE_MACRO(page_type, fmap, 0);

	    /*
	    ** Mark range of free & used pages in this FMAP.
	    ** Update FHDR hint if all pages in an FMAP are used.
	    */

	    if ( first_free < DM1P_FSEG_MACRO(page_type, page_size) * (i + 1) )
	    	 ffree = first_free;
	    else
	        ffree = DM1P_FSEG_MACRO(page_type, page_size) * (i + 1);

	    if ( last_free < ((i + 2) * DM1P_FSEG_MACRO(page_type, page_size)) - 1 )
	    	lfree = last_free;
	    else
	        lfree = ((i + 2) * DM1P_FSEG_MACRO(page_type, page_size)) - 1;

	    if (first_used < DM1P_FSEG_MACRO(page_type, page_size) * (i + 1) )
	    	fused = first_used;
	    else
	        fused = DM1P_FSEG_MACRO(page_type, page_size) * (i + 1);

	    /*
	    ** Update the FMAP page to reflect any of the new pages which
	    ** are marked used (those which were used to create new FMAP 
	    ** pages).  The used pages are between the range
	    ** of first_used and (first_free - 1).
	    **
	    ** Check to make sure there were used pages and that the 
	    ** used pages fall within the range of the current FMAP page.
	    */
	    if ( (fused <= (ffree - 1)) &&
		 ((fused / DM1P_FSEG_MACRO(page_type, page_size)) >= i) )
	    {
		dm1p_fmused(fmap, fused, ffree - 1, page_type, page_size);
	    }

	    /*
	    ** Update the FMAP page to reflect any of the new pages which
	    ** were marked free by the extend operation.
	    **
	    ** Check to make sure there were free pages and that the free pages
	    ** fall within the range of the current FMAP page.
	    */
	    if ( (ffree <= lfree) && 
		 ((ffree / DM1P_FSEG_MACRO(page_type, page_size)) >= i) )
	    {
		dm1p_fmfree(fmap, ffree, lfree, page_type, page_size);
	    }

	    /*  
	    ** Write the FMAP to disk 
	    */
	    io_count = 1;
	    status = dm2f_write_file(loc, (i4) 1, page_size, 
		(DB_TAB_NAME *) DB_NOTAVAIL_NAME, &dcb->dcb_name, &io_count,
		(i4) DM1P_VPT_GET_FMAP_PAGE_PAGE_MACRO(page_type, fmap), 
		(char *) fmap, err_code);
	    if (status != E_DB_OK)
		break;
	}
	if (status != E_DB_OK)
	    break;

	/*  Flush allocation to disk. */

	status = dm2f_flush_file(loc_array, loc_count,
		(DB_TAB_NAME *) DB_NOTAVAIL_NAME, &dcb->dcb_name, err_code);
	if (status != E_DB_OK)
	    break;

	loc->loc_fcb->fcb_last_page = 0;

	dm0m_deallocate((DM_OBJECT **)&misc_pages);

	return(E_DB_OK);
    }

    if (misc_pages)
	dm0m_deallocate((DM_OBJECT **)&misc_pages);

    /*	
    ** Handle errors. 
    */
    if (*err_code > E_DM_INTERNAL)
    {
	ule_format(*err_code, (CL_ERR_DESC *)0, ULE_LOG, NULL, (char *)0, 
	    (i4)0, (i4 *)0, err_code, 0);

  	*err_code = E_DM929C_DM1S_EMPTYTABLE;
    }

    return (E_DB_ERROR);
}
