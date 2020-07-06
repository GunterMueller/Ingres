/*
** Copyright (c) 1992, 2005 Ingres Corporation
**
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <di.h>
#include    <er.h>
#include    <me.h>
#include    <nm.h>
#include    <pc.h>
#include    <st.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <tr.h>
#include    <ulf.h>
#include    <dmf.h>
#include    <dm.h>
#include    <dmccb.h>
#include    <lk.h>
#include    <cx.h>
#include    <lkdef.h>
#include    <lgkdef.h>

#if defined(conf_CLUSTER_BUILD)

/*
** Name: LKDLOCK.C	- Cluster Global Deadlock searching services.
**
** Description:
**	This portion of the locking system is used solely by the DMFCSP Cluster
**	Support Process and is thus only part of the Cluster Locking System.
**
** History:
**	Summer, 1992 (bryanp)
**	    Working on the new logging and locking system.
**	16-feb-1993 (andys)
**	    Check for LLB_WAITING status before we try to access the
**	    llb_wait field. We try to only set llb_wait when we are waiting
**	    and to simultaneously set LLB_WAITING.
**	15-mar-1993 (andys)
**	    Removed "#if 0" to allow deadlock searching to proceed in 
**	    LK_deadlock. Updated code to use 6.5 routines.
**	24-may-1993 (bryanp)
**	    New arguments to LK_wakeup for mutex optimizations.
**	    Also added lkreq argument to LK_wakeup for shared locklists.
**	21-jun-1993 (bryanp)
**	    Worked on deadlock detection bugs introduced by the 6.5 usage of
**	    fair scheduling lock request algorithms in the cluster.
**	30-jun-1993 (shailaja)
**	    Fixed compiler warning on empty translation unit.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	26-jul-1993 (bryanp)
**	    Include <tr.h>
**	21-aug-1995 (dougb)
**	    As a related update to the fix for bug #70696, ensure we return
**	    SS$_INSFMEM when required. The bug fix may increase the fan-out
**	    for a deadlock search (potentially, by a large amount).  Thus,
**	    it is a "bad" idea to have any fixed limits on the number of
**	    output messages.  We must also update msg_cnt to reflect the
**	    number of messages which really should have been sent.
**	30-aug-1995 (dougb) bug #70696
**	    Send out messages specifying the maximum request mode of the
**	    waiting request.
**	31-aug-1995 (dougb) bug #70696
**	    Recurse through the local wait-for graph to exhaust it
**	    completely.
**	18-sep-1995 (duursma) bug #71331
**	    Don't recurse to the same RSB for convert requests.
**	19-sep-1995 (dougb) bug #71332
**	    Ensure we don't update the lock database while a server process is
**	    attempting to use it.  In particular, avoid a race condition when
**	    updating lkb->lkreq.status.  Don't want CSP making an update
**	    while a DBMS thread is executing code in LKrequest().
**	    Correct LK_CANCEL and SS$_CANCELGRANT handling in found_it().  The
**	    LK_CANCEL return value is a major error here.  SS$_CANCELGRANT
**	    means that the request has been granted -- but it should still be
**	    stalled if necessary (don't call LK_wakeup() with LK_NOSTATUS).
**	20-sep-1995 (duursma) bug #71334
**	    In order to detect initial local portions of global deadlock
**	    loops, change LKdeadlock so that it sends out deadlock messages
**	    about each of the blocking resources on the local node.
**	    Split off new routine walk_wait_for_graph() to facilitate
**	    this change.
**	04-jan-1996 (duursma) bug #71334
**	    Added more_info() routine and various other debugging printouts.
**	    Removed some comments in LKdeadlock() which were no longer valid.
**	    Changed the meaning of OK and LK_DEADLOCK. OK now means no deadlock
**	    was detected (but the search may have to continue), while
**	    LK_DEADLOCK means that deadlock was detected (and taken care of).
**	23-Feb-1996 (jenjo02)
**	    Brought up-to-date semaphore-wise. lkb_mutex has been supplanted
**	    by a host of new and exciting semaphores.
**	15-May-1996 (jenjo02)
**	    Brought forward repairs to lkrqst.c's deadlock() function to
**	    like code in walk_wait_for_graph().
**	30-may-1996 (pchang)
**	    Added LLB as a parameter to the call to LK_allocate_cb() to enable
**	    the reservation of SBK table resource for use by the recovery
**	    process. (B76879)
**	    Also, removed trigraph from comment while I'm here.
**	21-aug-1996 (boama01)
**	    Multiple chgs to LKdeadlock(), walk_wait_for_graph() and found_it()
**	    for bugs uncovered in Cluster tests:
**          - Replaced "i4 *rsh_table" with "RSH *rsh_table" to prevent
**            access violation in DMFCSP.
**          - Replaced "RSB *rsb_hash_table" with "RSH *rsb_hash_table".
**          - Replaced (void)LK_mutex/unmutex(&rsb_hash_bucket->rsb_mutex)
**                with (void)LK_mutex/unmutex(&rsb_hash_bucket->rsh_mutex)
**	      in the "Search the RSB hash bucket for a matching key" logic;
**            the rsb_hash_bucket at the moment points to the RSH block.
**	    - Improved and supplemented DEADLOCK_DEBUG traces; moved more_info
**	      call in LKdeadlock() to where it makes sense; also added
**	      search_length decrement in recursive part of walk_wait_for_graph.
**	    - Replaced (void)LG_unmutex(&original_rsb->rsb_mutex) with
**            (void)LK_unmutex(&original_rsb->rsb_mutex) to fix errors
**              E_CL250A_CS_NO_SEMAPHORE  E_DMA42F_LG_UNMUTEX
**	        E_CL2517_CS_SMPR_DEADLOCK
**            when running clustered VMS installation.
**	    - found_it() was prototyped and coded with a *LKD parm, but called
**	      from walk_wait_for_graph() with a *LKB parm!  Needless to say,
**	      this caused mem corruption probs.  A *LKD parm is pointless
**	      anyway, since this can be determined globally.  Chgd found_it()
**	      and its callers to only use *LLB parm.  Also removed an extra
**	      unmutex of RSB in walk_wait_for_graph(), before it calls
**	      found_it().
**	14-Nov-1996 (jenjo02)
**	    Deadlock() cleanup. Instead of taking all those SHARED mutexes
**	    (RSB, LLB) and checking for rsb_dlock, etc., do the following:
**	      o Whenever a value which affects the wait-for graph is
**	        about to be changed, lock an installation-wide mutes,
**	        lkd_dlock_mutex, make the change, and release the mutex.
**	      o All variables which affect the graph must be mutexed in
**	        this manner.
**		    - the RSB grant and wait queues
**		    - llb_wait (LKB *)
**		    - LLB_WAITING status
**		    - lkb_state
**		    - lkb_request_mode
**	      o The deadlock() code will take an EXCL lock on lkd_dlock_mutex
**		while it does its business. If the protocol is adhered to,
**	        this guarantees that the graph will not change while its
**		being computed, therefore there is no need for deadlock()
**	        to take any lower-level mutexes.
**	16-Dec-1996 (jenjo02)
**	    In found_it(), lock lkd_dlock_mutex before calling LK_resume().
**	19-Mar-1997 (jenjo02)
**	  - Instead of scanning the LLB array for lock lists of interest,
**	    use the queue of LLBs that haven't been deadlock searched. This
**	    queue was created for the interval-based deadlock detection 
**	    thread.
**	  - Changed walk_wait_for_graph() to return LK_DEADLOCK if a deadlock
**	    was detected instead of calling found_it(). Caller is responsible 
**	    for calling found_it(). Caller must also acquire lkd_dlock_mutex
**	    before calling walk_wait_for_graph().
**	14-Aug-1997 (toumi01)
**	    Move #ifdef etc. to column 1, as required by many compilers.
**	16-Nov-1998 (jenjo02)
**	    Cross-process thread identity changed to CS_CPID structure
**	    from PID and SID.
**	14-Jan-1999 (jenjo02)
**	    Recheck LLB_ON_DLOCK_LIST state after waiting for mutex in
**	    LKdeadlock().
**	16-feb-1999 (nanpr01)
**	    Support for update mode lock.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	12-apr-2001 (devjo01)
**	    s103715 (Portable cluster support)
**	20-Aug-2002 (jenjo02)
**	    Updated to current LK architecture:
**	    - LLB may have multiple waiting lock requests (llb_lkb_wait)
**	      instead of just one (llb_wait), hence the "unit of wait"
**	      is an LKB, not an LLB.
**	    - Lock requestors are responsible for cancelling themselves
**	      when a deadlock occurs. Attempts to LK_cancel() a 
**	      lock request in an indeterminate state from another thread
**	      are frought with timing-related problems.
**	    - Mutexing scheme in wait-for graph totally redone (for the
**	      last time?); it'd be nice if the two functions "deadlock()"
**	      in LKrequest and "walk_wait_for_graph" could be combined
**	      into one so they don't keep getting out of synch!
**	17-sep-2003 (abbjo03)
**	    Changes to use VMS headers provided by HP.
**	23-oct-2003 (devjo01)
**	    Correct structure names for rsb_name within VMS only scope.
**	24-oct-2003 (devjo01)
**	    Change ALL instances of rsb_name references to conform to an
**	    LK_LOCK_KEY.
**	24-oct-2003 (devjo01)
**	    Change instances of lkb_lksb.lock_id references to 
**	    lkb_cx_req.cx_lock_id
**	26-jan-2004 (devjo01)
**	    Use CX_EXTRACT_LOCK_ID macro to copy OS varying cx_lock_id
**	    to the LKDSM lockid member, and/or a standard LK_UNIQUE.
**	    This allows for a consistent external DLM lock id
**	    representation across all platforms while allowing the
**	    internal representation to conform to the native DLM's
**	    expectation.
**      7-oct-2004 (thaju02)
**          Use SIZE_TYPE to allow memory pools > 2Gig.
**	30-Dec-2004 (schka24)
**	    Remove special case for iixprotect.
**	10-may-2005 (devjo01)
**	    Made generic and moved into RCP as part of sirs 114136 & 114504.
**	06-jun-2005 (devjo01)
**	    Corrected extern reference to LK_read_ppp_ctx.
**	    Correct other compiler warnings not returned under VMS.
**	16-nov-2005 (devjo01)
**	    Rename CXdlm_mark_as_deadlocked to CXdlm_mark_as_posted,
**	    and set cx_status locally to conform with new more
**	    generic usage of CXdlm_mark_as_posted.
**	21-nov-2006 (jenjo02/abbjo03)
**	    Don't discard messages originating from this node where lock_key
**	    does not match lkb_id->lock_key.  Enhance some trace messages.
**	    Remove commented out code.
**	02-jan-2007 (jenjo02/abbjo03)
**	    Additional trace code made dependent on II_CLUSTER_DEADLOCK_DEBUG
**	    logical.  Make LK_cont_dsmc and LK_read_ppp_ctx local statics.
**	    Reduce MAX_DSMS_PER_BUFFER to 744 to make the buffer fit within
**	    64K.
**	03-jan-2007 (abbjo03)
**	    Supplement 2-jan change: LK_DEADLOCK_MSG also has to depend on
**	    lk_cluster_deadlock_debug.
**	18-jan-2007 (jenjo02/abbjo030
**	    Compute max messages per buffer instead of hard-coding it.
**	    Add msg_seq to LK_DSM for debugging.  Add TRflush()es to ensure
**	    we don't miss debugging info.
*/


/*
** The LK_compatible matrix is defined in LKRQST.C, as is LK_convert_mode.
*/
GLOBALREF char	LK_compatible[];

static CX_PPP_CTX	 LK_read_ppp_ctx;

static LK_DSM_CONTEXT	*LK_cont_dsmc;

GLOBALCONSTREF char LK_convert_mode [8] [8];

static bool lk_cluster_deadlock_debug = FALSE;


static char *dsm_svc_name(void);


#if defined(VMS) && defined(conf_CLUSTER_BUILD)
#define LKDEADLOCK_DEBUG
#endif

#ifdef LKDEADLOCK_DEBUG

/*
** Debugging routine to print out additional info on resource
*/
# define LK_DEADLOCK_MSG( msg, node, lkbid ) \
   TRdisplay( "%@ LKdist_deadlock_thread %d:%x %s.\n", node, lkbid, msg ), TRflush()

static void
more_info( RSB *rsb, i4  code )
{
# if 0	/* Enabling this section breaks IPM linkage as uld_syscat_namestr 
           drags half the back-end in. */
    char *name;
    i4  rstype = rsb->rsb_name.lk_type;

    if (rstype == LK_PAGE || rstype == LK_BM_PAGE) {
	/* we only care about page level locks */
	i4 reltid = rsb->rsb_name.lk_key2;
	i4 reltidx= rsb->rsb_name.lk_key3;
	i4 pagenr = rsb->rsb_name.lk_key4;

	if (reltidx != 0) reltid = reltidx;	/* Use indexid if nonzero */
	name = uld_syscat_namestr(reltid);
	TRdisplay("LKdeadlock (%d) %s page %d\n", code, name, pagenr);
    }
# endif
}

#else
# define LK_DEADLOCK_MSG( msg, node, lkbid )
#endif /* LKDEADLOCK_DEBUG */


/*
** Name: LKdist_deadlock_thread	- Perform cluster wide deadlock detection.
**
** Description:
**
**	This routine forms the body of the thread responsible for 
**	continuation of distributed deadlock searches.
**
**	Please see the distributed deadlock addendum to LKdeadlock_thread
**	for an overview of this process.
**
**	This routine consumes LK_DSM messages sent to it from other
**	nodes.  LK_DSM messages are generated by LK_local_deadlock
**	as it traverses the originating node's segment of the deadock
**	graph.
**
**	If a message processed here represents a lock that ultimately
**	originated from the node we are running on, we check if the lock
**	is still waiting.
**
**	If it isn't, there is no deadlock, and we're done.  
**
**	If it is we check to see if the incoming message is for the same
**	resource.  If it is then the original lock is deadlocked.
**
**	Otherwise the deadlock graph is just passing through our local
**	node again for another resource.  Traversing the local section
**	of the deadlock graph for blockers of this resource may in turn
**	generate additonal messages to karoom about the installation.
**
**
** Inputs:
**	dmf_cb			DMF control block (currently unused).
**
** Outputs:
**	none
**
**	Returns:
**	    Typically will not return for the life of the server.
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	Summer, 1992 (bryanp)
**	    Working on the new logging and locking system.
**	16-feb-1993 (andys)
**	    Check for LLB_WAITING status before we try to access the
**	    llb_wait field. We try to only set llb_wait when we are waiting
**	    and to simultaneously set LLB_WAITING.
**	24-may-1993 (bryanp)
**	    New arguments to LK_wakeup for mutex optimizations.
**	    Also added lkreq argument to LK_wakeup for shared locklists.
**	21-jun-1993 (bryanp)
**	    Worked on deadlock detection bugs introduced by the 6.5 usage of
**	    fair scheduling lock request algorithms in the cluster.
**	21-aug-1995 (dougb) bug #70696
**	    As a related update to the fix for bug #70696, ensure we return
**	    SS$_INSFMEM when required. The bug fix may increase the fan-out
**	    for a deadlock search (potentially, by a large amount).  Thus,
**	    it is a "bad" idea to have any fixed limits on the number of
**	    output messages.  We must also update msg_cnt to reflect the
**	    number of messages which really should have been sent.
**	    Update the comments above to reflect reality (eg. what status is
**	    actually returned).
**	30-aug-1995 (dougb) bug #70696
**	    Send out messages specifying the maximum request mode of the
**	    waiting request.
**	31-aug-1995 (dougb) bug #70696
**	    Recurse through the local wait-for graph to exhaust it
**	    completely.
**	18-sep-1995 (duursma) bug #71331
**	    Don't recurse to the same RSB for convert requests.
**	19-sep-1995 (dougb) bug #71332
**	    Ensure we don't update the lock database while a server process is
**	    attempting to use it.  In particular, avoid a race condition when
**	    updating lkb->lkreq.status.  Don't want CSP making an update while
**	    a DBMS thread is executing code in LKrequest().  Hold lk_mutex.
**	    Use new interface to found_it() -- may return an error status now.
**	20-sep-1995 (duursma) bug #71334
**	    Split off new walk_wait_for_graph() and reorganized.
**	23-Feb-1996 (jenjo02)
**	    Brought up-to-date semaphore-wise. lkb_mutex has been supplanted
**	    by a host of new and exciting semaphores.
**	30-may-1996 (pchang)
**	    Added LLB as a parameter to the call to LK_allocate_cb() to enable
**	    the reservation of SBK table resource for use by the recovery
**	    process. (B76879)
**	21-aug-1996 (boama01)
**          Replaced "RSB *rsb_hash_table" with "RSH *rsb_hash_table".
**          Replaced "i4 *rsh_table" with "RSH *rsh_table" to prevent
**            access violation in DMFCSP.
**          Replaced (void)LK_mutex/unmutex(&rsb_hash_bucket->rsb_mutex)
**              with (void)LK_mutex/unmutex(&rsb_hash_bucket->rsh_mutex)
**	      in the "Search the RSB hash bucket for a matching key" logic;
**            the rsb_hash_bucket at the moment points to the RSH block.
**	    Improved and supplemented DEADLOCK_DEBUG traces; moved more_info
**	      call to where it makes sense.
**	14-Nov-1996 (jenjo02)
**	    Deadlock() cleanup. Instead of taking all those SHARED mutexes
**	    (RSB, LLB) and checking for rsb_dlock, etc., do the following:
**	      o Whenever a value which affects the wait-for graph is
**	        about to be changed, lock an installation-wide mutes,
**	        lkd_dlock_mutex, make the change, and release the mutex.
**	      o All variables which affect the graph must be mutexed in
**	        this manner.
**		    - the RSB grant and wait queues
**		    - llb_wait (LKB *)
**		    - LLB_WAITING status
**		    - lkb_state
**		    - lkb_request_mode
**	      o The deadlock() code will take an EXCL lock on lkd_dlock_mutex
**		while it does its business. If the protocol is adhered to,
**	        this guarantees that the graph will not change while its
**		being computed, therefore there is no need for deadlock()
**	        to take any lower-level mutexes.
**	16-Dec-1996 (jenjo02)
**	    In found_it(), lock lkd_dlock_mutex before calling LK_resume().
**	19-Mar-1997 (jenjo02)
**	  - Instead of scanning the LLB array for lock lists of interest,
**	    use the queue of LLBs that haven't been deadlock searched. This
**	    queue was created for the interval-based deadlock detection 
**	    thread.
**	  - Changed walk_wait_for_graph() to return LK_DEADLOCK if a deadlock
**	    was detected instead of calling found_it(). Caller is responsible 
**	    for calling found_it(). Caller must also acquire lkd_dlock_mutex
**	    before calling walk_wait_for_graph().
**	14-Jan-1999 (jenjo02)
**	    Recheck LLB_ON_DLOCK_LIST state after waiting for mutex.
**	10-may-2005 (devjo01)
**	    Renamed from LKdeadlock to LKdist_deadlock_thread, made generic and 
**	    moved into RCP as part of sirs 114136 & 114504.
**	    - Removed whole "in_msg == NULL" section.  Routine is only
**	      called to process distributed deadlock continuation messages.
**	      Initial DSMs are created in LKdeadlock_thread.
**	    - Removed parameters related to outputting messages, as this
**	      is now handled by the LK "dsm" functions.
**	    - Use CXdlm_mark_as_deadlocked && LKcp_resume to assure
**	      a deadlock victim is awoken only once, and in the same
**	      context, and with the same code as if the DLM had detected
**	      deadlock itself.
**	12-Feb-2007 (jonj)
**	    To avoid CPU saturation, periodically hiccup-wait to give
**	    other RCP threads (e.g. LKdeadlock_thread) a chance to run.
**	    A real hiccup wait would be nice here; the best we can do 
**	    is a 1-second suspend.
**	    Do NOT cycle through the RSB grant queue, just call LK_local_deadlock
**	    on the first-encountered incompatible lock, which will in turn
**	    cycle through the RSB grant queue and find all that are
**	    incompatible.
**	    Add "must_retry" logic to avoid RSB mutex deadlocks with
**	    the competing LKdeadlock_thread.
**	21-Feb-2007 (jonj)
**	    Actually, we must cycle through the RSB grant/convert queue to
**	    ensure that continuation messages are sent for all incompatible
**	    blocked locks.
**	    Add no-read CXppp_read() before calling LK_flush_dsm_context to
**	    release just-consumed message space and unstall any stalled
**	    readers.
**	    If all grant/convert locks are compatible, we must check for
**	    incompatible locks on the wait queue as they may well be the
**	    blocking lock(s) revealed by CXdlm_get_blki().
*/
DB_STATUS
LKdist_deadlock_thread( DMC_CB *dmc_cb )
{
#   define MAX_DSMS_PER_CALL	8	
    i4			max_dsms_per_buffer = 65535 / sizeof(LK_DSM);
    LKD			*lkd = (LKD *)LGK_base.lgk_lkd_ptr;
    LLB			*llb;
    LKB			*lkb, *top_lkb;
    RSB			*rsb;
    RSH			*rsb_hash_bucket;
    u_i4		rsb_hash_value;
    LK_LOCK_KEY		*lock_key;
    RSH			*rsh_table;
    SIZE_TYPE		end_offset, lkb_offset;
    i4			matching_rsb;
    bool		found;
    STATUS		ret_val;
    SIZE_TYPE		*sbk_table;
    i4			 err_code, status;
    LK_DSM		*in_msg;
    u_i4		msgcount;
    char		*param;
    i4			must_retry;
    bool		all_compat;

    LK_DEADLOCK_MSG( "Dist Deadlock thread started", lkd->lkd_csid, 0 );

    /* Get number of configured nodes into "msgcount" */
    status = CXcluster_nodes( &msgcount, NULL );
    if ( status )
    {
	ule_format(status, (CL_ERR_DESC *)0, ULE_LOG,
		NULL, (char *)0, 0L, (i4 *)0, &err_code, 0);
	return E_DB_ERROR;
    }

    if ( msgcount <= 1 )
    {
	/*
	** Only one node in "cluster" so we can't have continuation 
	** messages.  Suspend thread until shutdown.
	*/
	status = CSsuspend( CS_BIO_MASK, 0, 0 );
	/* When interrupted, it's time to shutdown */
	if (status == E_CS0008_INTERRUPTED)
	    return(E_DB_OK);

	return E_DB_ERROR;
    }

    NMgtAt("II_CLUSTER_DEADLOCK_DEBUG", &param );
    if (param && *param)
	lk_cluster_deadlock_debug = TRUE;

    status = CXppp_alloc( &LK_read_ppp_ctx, dsm_svc_name(), 2 * (msgcount - 1),
                          max_dsms_per_buffer * sizeof(LK_DSM),
			  lk_cluster_deadlock_debug );
    if ( status )
    {
	ule_format(status, (CL_ERR_DESC *)0, ULE_LOG,
		NULL, (char *)0, 0L, (i4 *)0, &err_code, 0);
	return E_DB_ERROR;
    }

    status = CXppp_listen( LK_read_ppp_ctx );
    if ( status )
    {
	ule_format(status, (CL_ERR_DESC *)0, ULE_LOG,
		NULL, (char *)0, 0L, (i4 *)0, &err_code, 0);
	return E_DB_ERROR;
    }

    /*
    ** Allocate a separate block of LK_DSM messages
    ** for each potential member of the cluster.  
    **
    ** Only required if we cannot depend on DLM to
    ** handle distributed deadlock detection.
    */
    status = LK_alloc_dsm_context( &LK_cont_dsmc );
    if ( status )
    {
	ule_format(status, (CL_ERR_DESC *)0, ULE_LOG,
		NULL, (char *)0, 0L, (i4 *)0, &err_code, 0);
	return E_DB_ERROR;
    }

    for ( ; /* Exit only on thread death */ ; )
    {
#if defined(LKDEADLOCK_DEBUG)
	int nummsgs;
#endif
	/*
	** Get a pointer to buffered LK_DSM messages.  We don't ask
	** for the maximum possible number of contiguous messages
	** available to avoid any one LK_DSM source monopolizing
	** this thread for too many iterations.
	*/
	if (status == OK)
	    status = CXppp_read( LK_read_ppp_ctx, sizeof(LK_DSM), 
	                     MAX_DSMS_PER_CALL, 0, (char **)&in_msg,
			     &msgcount );

	/* When interrupted, it's time to shutdown */
	if (status == E_CS0008_INTERRUPTED)
	    return(E_DB_OK);

#if defined(LKDEADLOCK_DEBUG)
      if (lk_cluster_deadlock_debug)
      {
	if (status)
	    TRdisplay("%@ LKdist_deadlock_thread: CXppp_read error %x "
		"msgcount %d\n", status, msgcount), TRflush();
      }
#endif

	/* Other status values are unexpected. */
	if ( OK != status )
	{
	    ule_format( status, (CL_ERR_DESC *)0, ULE_LOG,
			NULL, (char *)0, 0L, (i4 *)0, &err_code, 0);
	    break;
	}

	/* It's more useful to know the number of msgs received */
	lkd->lkd_stat.gdlck_call += msgcount;

#if defined(LKDEADLOCK_DEBUG)
	nummsgs = msgcount;
#endif

	/* Loop over all messages received */
	while ( msgcount-- > 0 )
	{
	    lock_key = &in_msg->lock_key;

#if defined(LKDEADLOCK_DEBUG)
	  if (lk_cluster_deadlock_debug)
	  {
	    {
		char	keydispbuf[256];

		TRdisplay("\n\n%@ LKdist_deadlock_thread: %8d [%d of %d]\n\t"
		    "Continue gbl srch to %d lkblks, from node %x\n\to_tran=%x%x,"
		    " tran=%x%x, stamp=%x\n\tlkb_id = %x, key = %s\n\tcum_mode %d"
		    " dlm %x state %2w\n",
		    in_msg->msg_seq,
		    nummsgs - msgcount, nummsgs, lkd->lkd_lbk_count, in_msg->o_node,
		    in_msg->o_tran_id.lk_uhigh, in_msg->o_tran_id.lk_ulow,
		    in_msg->tran_id.lk_uhigh,   in_msg->tran_id.lk_ulow,
		    in_msg->o_stamp, in_msg->o_lkb_id,
		    LKkey_to_string(lock_key, keydispbuf), 
		    in_msg->lock_cum_mode, in_msg->o_lockid.lk_ulow,
		    ",GR,CV,WT", in_msg->lock_state); TRflush();
	    }
	  }
#endif

	    /*
	    ** First, if the original transaction is from the current node,
	    ** check to see if the transaction still exists. If not, ignore
	    ** the continue deadlock search message. The original transaction
	    ** may have already finished by the time the obsolete continue
	    ** deadlock search message comes in.
	    */

	    top_lkb = NULL;

	    if (in_msg->o_node == lkd->lkd_csid)
	    {
		/*
		** We don't need the deadlock queue  mutex, as it is
		** guaranteed that the LKB is removed from the deadlock
		** queue by LKdeadlock_thread before the distributed
		** search began.
		*/
		rsb = (RSB*)NULL;
		if ( in_msg->o_lkb_id && in_msg->o_lkb_id <= lkd->lkd_sbk_count )
		{
		    sbk_table = (SIZE_TYPE *)LGK_PTR_FROM_OFFSET(lkd->lkd_sbk_table);
		    lkb = (LKB *)LGK_PTR_FROM_OFFSET(sbk_table[in_msg->o_lkb_id]);
		    llb = (LLB *)LGK_PTR_FROM_OFFSET(lkb->lkb_lkbq.lkbq_llb);
		    rsb = (RSB *)LGK_PTR_FROM_OFFSET(lkb->lkb_lkbq.lkbq_rsb);

		    if ( lkb->lkb_type == LKB_TYPE && 
			 llb && llb->llb_type == LLB_TYPE &&
			 rsb && rsb->rsb_type == RSB_TYPE )
		    {
			if (ret_val = LK_mutex(SEM_EXCL, &rsb->rsb_mutex))
			{
			    ule_format( ret_val, (CL_ERR_DESC *)0, ULE_LOG,
					NULL, (char *)0, 0L, (i4 *)0, &err_code, 0);
			    TRdisplay("%@ Panic: %s %d rsb_mutex failed\n",
				       __FILE__, __LINE__); TRflush();
			    in_msg++;
			    continue;
			}

			/*
			** After waiting for the mutex, recheck everything
			** to ensure that the lock is still what we think 
			** it was.
			*/

			/*
			** NB: In the continuation case, the lock_key in in_msg->lock_key
			**     is not necessarily the original lock key. Instead, check
			**     that the original distributed lockid still matches the LKB
			*/

			/* Same lock request and transaction? */
			if ( rsb->rsb_type != RSB_TYPE || lkb->lkb_type != LKB_TYPE ||
			     llb->llb_type != LLB_TYPE ||
			     lkb->lkb_lkreq.request_id != in_msg->o_stamp ||
			     in_msg->o_tran_id.lk_uhigh != llb->llb_name[0] || 
			     in_msg->o_tran_id.lk_ulow != llb->llb_name[1] )
			{
			    (VOID)LK_unmutex(&rsb->rsb_mutex);
			    rsb = (RSB*)NULL;
			}
		    }
		    else
			rsb = (RSB*)NULL;
		}

		/*
		** If anything about the lock has changed, then
		** ignore the message.
		*/
		if ( rsb == (RSB*)NULL )
		{
		  if (lk_cluster_deadlock_debug)
		  {
		    LK_DEADLOCK_MSG( "lock is no longer valid",
			in_msg->o_node, in_msg->o_lkb_id );
		  }
		    in_msg++;
		    continue;
		}

		/*
		** If the original transaction exists, check if the transaction
		** is no longer in the waiting state. The lock could have
		** been granted by the time the deadlock search message came
		** in. If the transaction is still in waiting state, check
		** to see it is waiting on the same lock that required deadlock
		** search.
		**
		** Note that we now hold the rsb_mutex. 
		** This prevents the lkb state from changing.
		*/

		if ((lkb->lkb_state != LKB_WAITING
		     && lkb->lkb_state != LKB_CONVERT)
		     || lkb->lkb_attribute & LKB_DEADLOCKED)
		{
		  if (lk_cluster_deadlock_debug)
		  {
		    if ( lkb->lkb_attribute & LKB_DEADLOCKED )
		      LK_DEADLOCK_MSG( "orig lock is deadlocked", \
			in_msg->o_node, in_msg->o_lkb_id );
		    else
		      LK_DEADLOCK_MSG( "orig lock no longer blocked", \
			in_msg->o_node, in_msg->o_lkb_id );
		  }

		    (void)LK_unmutex(&rsb->rsb_mutex);
		    in_msg++;
		    continue;
		}

		/* Now check for continuation on a different lock key */

		if ( rsb->rsb_name.lk_type == lock_key->lk_type &&
		     rsb->rsb_name.lk_key1 == lock_key->lk_key1 &&
		     rsb->rsb_name.lk_key2 == lock_key->lk_key2 &&
		     rsb->rsb_name.lk_key3 == lock_key->lk_key3 &&
		     rsb->rsb_name.lk_key4 == lock_key->lk_key4 &&
		     rsb->rsb_name.lk_key5 == lock_key->lk_key5 &&
		     rsb->rsb_name.lk_key6 == lock_key->lk_key6)
		{
		    /*
		    ** End of cycle, we're deadlocked, but ignore if already
		    ** posted
		    */
		    if (CXdlm_mark_as_posted(&lkb->lkb_cx_req))
		    {
		      if (lk_cluster_deadlock_debug)
		      {
			LK_DEADLOCK_MSG( "orig lock is deadlocked", \
			    in_msg->o_node, in_msg->o_lkb_id );
		      }

			lkb->lkb_cx_req.cx_status = E_CL2C21_CX_E_DLM_DEADLOCK;

			/* Mark the LKB as deadlocked while RSB mutex is held */
			lkb->lkb_attribute |= LKB_DEADLOCKED;
				
			/* Save information about blocker */
			lkb->lkb_lkreq.blocking_cpid.pid = in_msg->o_blk_pid;
			lkb->lkb_lkreq.blocking_cpid.sid = in_msg->o_blk_sid;
			lkb->lkb_lkreq.blocking_mode = in_msg->o_blk_gtmode;

			/* Count another global deadlock */
			lkd->lkd_stat.gdeadlock++;

			/* Resume the unfortunate */
			LKcp_resume(&lkb->lkb_lkreq.cpid);
		    }
		    LK_unmutex(&rsb->rsb_mutex);
		    in_msg++;
		    continue;
		}

		/*
		** Original lkb is not directly deadlocked, but we may
		** still deadlock if there are transactions blocked on
		** the resource in in_msg that are blocked on this node.
		*/
		LK_unmutex(&rsb->rsb_mutex);
		top_lkb = lkb;
	    }

	    /*
	    ** Original transaction is from a different node or
	    ** key is not for the originating lkb.
	    */

	    /*
	    **	Search the RSB hash bucket for a matching key.
	    */
	    rsh_table = (RSH *) LGK_PTR_FROM_OFFSET(lkd->lkd_rsh_table);

	    rsb_hash_value = (lock_key->lk_type + lock_key->lk_key1 +
		lock_key->lk_key2 + lock_key->lk_key3 + lock_key->lk_key4 +
		lock_key->lk_key5 + lock_key->lk_key6);

	    rsb_hash_bucket = &rsh_table
		[(unsigned)rsb_hash_value % lkd->lkd_rsh_size];

	    if (ret_val = LK_mutex(SEM_EXCL, &rsb_hash_bucket->rsh_mutex))
	    {
		ule_format( ret_val, (CL_ERR_DESC *)0, ULE_LOG,
			    NULL, (char *)0, 0L, (i4 *)0, &err_code, 0);
		in_msg++;
		continue;
	    }

#ifdef LKDEADLOCK_DEBUG
	  if (lk_cluster_deadlock_debug)
	  {
	    TRdisplay("%@ LKdist_deadlock_thread: search for RSB matching key\n"); TRflush();
	  }
#endif /* LKDEADLOCK_DEBUG */
	    matching_rsb = 0;
	    for (rsb = (RSB *) rsb_hash_bucket;
		 rsb->rsb_q_next;
		)
	    {
		rsb = (RSB *)LGK_PTR_FROM_OFFSET(rsb->rsb_q_next);

		if (rsb->rsb_name.lk_type == lock_key->lk_type &&
		    rsb->rsb_name.lk_key1 == lock_key->lk_key1 &&
		    rsb->rsb_name.lk_key2 == lock_key->lk_key2 &&
		    rsb->rsb_name.lk_key3 == lock_key->lk_key3 &&
		    rsb->rsb_name.lk_key4 == lock_key->lk_key4 &&
		    rsb->rsb_name.lk_key5 == lock_key->lk_key5 &&
		    rsb->rsb_name.lk_key6 == lock_key->lk_key6)
		{
		    /*
		    ** Unlock the hash queue, lock the RSB, recheck.
		    */
		    (void)LK_unmutex(&rsb_hash_bucket->rsh_mutex);

		    if (ret_val = LK_mutex(SEM_EXCL, &rsb->rsb_mutex))
		    {
			ule_format( ret_val, (CL_ERR_DESC *)0, ULE_LOG,
				    NULL, (char *)0, 0L, (i4 *)0, &err_code, 0);
			TRdisplay("%@ Panic: %s %d RSB mutex failed\n",
					__FILE__, __LINE__); TRflush();
			break;
		    }
		    if (rsb->rsb_name.lk_type == lock_key->lk_type &&
			rsb->rsb_name.lk_key1 == lock_key->lk_key1 &&
			rsb->rsb_name.lk_key2 == lock_key->lk_key2 &&
			rsb->rsb_name.lk_key3 == lock_key->lk_key3 &&
			rsb->rsb_name.lk_key4 == lock_key->lk_key4 &&
			rsb->rsb_name.lk_key5 == lock_key->lk_key5 &&
			rsb->rsb_name.lk_key6 == lock_key->lk_key6)
		    {
			matching_rsb = 1;
			break;
		    }
		    (void)LK_unmutex(&rsb->rsb_mutex);
		    (void)LK_mutex(SEM_EXCL, &rsb_hash_bucket->rsh_mutex);
		    rsb = (RSB *)rsb_hash_bucket;
		}
	    }

	    if (matching_rsb == 0)
	    {
		/* Resource not found. */
		if ( !ret_val )
		    (void)LK_unmutex(&rsb_hash_bucket->rsh_mutex);

	      if (lk_cluster_deadlock_debug)
	      {
		LK_DEADLOCK_MSG( "Continuation resource not found", \
			in_msg->o_node, in_msg->o_lkb_id );
	      }
		in_msg++;
		continue;
	    }

#ifdef LKDEADLOCK_DEBUG
	    more_info( rsb, 2 );
#endif

	    /*
	    ** RSB was found, and its mutex is locked.
	    */

	    /*
	    ** Prime the pump by looking for LKBs that are blocked, and
	    ** incompatible with passed lock mode.
	    **
	    ** If there are no blocked LLB's on this resource then there
	    ** is no deadlock.
	    */ 

	    /* Perform continue deadlock search operation. */

	    lkd->lkd_stat.cnt_gdlck_call++;

	    /*
	    ** Don't get rsb_mutex twice, and don't let
	    ** LK_local_deadlock release it.
	    */
	    /* Tell LK_local_deadlock the LKdist_deadlock_thread holds the mutex */ 
	    rsb->rsb_dlock_depth = RSB_MUTEXED_DIST;

	    /* Placehold outgoing messages JIC we find deadlock here */
	    LK_mark_dsm_context( LK_cont_dsmc );

	    /* 
	    ** Traverse grant queue, and for all granted or converting locks
	    ** which are incompatible with the cummulative lock mode, and are
	    ** on waiting LLB's, start a local deadlock search.
	    */
	    end_offset = LGK_OFFSET_FROM_PTR(&rsb->rsb_grant_q_next);
	    lkb_offset = rsb->rsb_grant_q_next;

	    must_retry = 0;

	    /*
	    ** If all grant/convert locks are compatible with cumulative lock
	    ** mode, then check for incompatible waiters. CXdlm_get_blki()
	    ** does this and we may have been sent this msg because a waiter
	    ** on this resource is ahead of and blocking a waiter on another
	    ** node, yet all grant/convert locks on this node are compatible.
	    */
	    for ( all_compat = TRUE; !must_retry; )
	    {
		while ( lkb_offset != end_offset && !must_retry )
		{
		    lkb = (LKB *)LGK_PTR_FROM_OFFSET(lkb_offset);

		    /*
		    ** We may encounter LKBs which were DEADLOCKED during a previous
		    ** deadlock() cycle but which haven't had time to be cancelled
		    ** yet. If we do, skip over them.
		    */
		    if (lkb->lkb_attribute & LKB_DEADLOCKED)
		    {
			lkb_offset = lkb->lkb_q_next;
			continue;
		    }

		    lkd->lkd_stat.dlock_locks_examined++;

		    /*
		    ** If this lock's request mode is incompatible with the
		    ** max request mode of the waiters, then this lock request is
		    ** blocking the resource and may be causing deadlock.
		    */

		    if ( ((LK_compatible[lkb->lkb_request_mode] >>
				    in_msg->lock_cum_mode) & 1) == 0 )
		    {
			all_compat = FALSE;

			llb = (LLB *)LGK_PTR_FROM_OFFSET(lkb->lkb_lkbq.lkbq_llb);

			/* Skip this LLB if no waiters */
			if ( llb->llb_waiters )
			{
			    /*
			    ** Potential blocker. Set blocking info if this
			    ** is the 1st hop of a distributed search.
			    */
			    if ( in_msg->o_blk_pid == 0 )
			    {
				in_msg->o_blk_pid = lkb->lkb_lkreq.cpid.pid;
				in_msg->o_blk_sid = lkb->lkb_lkreq.cpid.sid;
			    }

#if defined(LKDEADLOCK_DEBUG)
			  if (lk_cluster_deadlock_debug)
			  {
			    TRdisplay("%@ LKdist_deadlock_thread %d:%x Searching "
				"local graph, top_lkb %x dlm %x state %2w\n"
				"    lkb %x mode %d dlm %x\n",
				in_msg->o_node, in_msg->o_lkb_id,
				top_lkb ? top_lkb->lkb_id : 0,
				in_msg->o_lockid.lk_ulow, ",GR,CV,WT",
				in_msg->lock_state, lkb->lkb_id, lkb->lkb_grant_mode,
				lkb->lkb_cx_req.cx_lock_id); TRflush();
			  }
#endif

			    if ( LK_local_deadlock( lkb, top_lkb,
				   in_msg->lock_cum_mode, in_msg, LK_cont_dsmc,
				   0, &must_retry) )
			    {
			      if (top_lkb)
			      {
				/* Original lock is deadlocked, post and resume it */
				if (CXdlm_mark_as_posted(&top_lkb->lkb_cx_req))
				{
				    top_lkb->lkb_cx_req.cx_status =
					 E_CL2C21_CX_E_DLM_DEADLOCK;
						       
				    /* Save information about blocker */
				    top_lkb->lkb_lkreq.blocking_cpid.pid =
					 in_msg->o_blk_pid;
				    top_lkb->lkb_lkreq.blocking_cpid.sid =
					 in_msg->o_blk_sid;
				    top_lkb->lkb_lkreq.blocking_mode =
					 in_msg->o_blk_gtmode;

				    /* Toss accumulated DSM's for this resource */
				    LK_unmark_dsm_context( LK_cont_dsmc );

				    if (lk_cluster_deadlock_debug)
				    {
				      LK_DEADLOCK_MSG( "orig lock is deadlocked(2)", \
					in_msg->o_node, in_msg->o_lkb_id );
				    }

				    /* Count another global deadlock */
				    lkd->lkd_stat.gdeadlock++;

				    /* Resume the unfortunate */
				    LKcp_resume(&top_lkb->lkb_lkreq.cpid);
				    break;
				}
				else
				{
				    /* We were not really deadlocked */
				    top_lkb->lkb_attribute &= ~LKB_DEADLOCKED;
				}
			      }
			      else
				TRdisplay("%@ LKdist_deadlock_thread: LK_local_deadlock"
					" returned null top_lkb\n");

			    }
			}
		    }
		    lkb_offset = lkb->lkb_q_next;
		}

		/*
		** End of queue.
		**
		** If all that we looked at are compatible with cumulative mode
		** and we haven't checked the wait queue, then cycle
		** back to do the waiters.
		*/
		if ( all_compat
		     && end_offset == LGK_OFFSET_FROM_PTR(&rsb->rsb_grant_q_next)
		     && rsb->rsb_waiters )
		{
		    end_offset = LGK_OFFSET_FROM_PTR(&rsb->rsb_wait_q_next);
		    lkb_offset = rsb->rsb_wait_q_next;
		}
		else
		    break;
	    }

	    /* Release the resource block */
	    rsb->rsb_dlock_depth = 0;
	    LK_unmutex(&rsb->rsb_mutex);

	    if ( must_retry )
	    {
		/* Toss accumulated DSM's for this resource */
		LK_unmark_dsm_context( LK_cont_dsmc );

		/* Wait 1sec, stay on this msg, retry from the top */
		status = CSsuspend(CS_TIMEOUT_MASK, 1, 0);

		if ( status == E_CS0009_TIMEOUT )
		    status = OK;
		else
		    break;

		TRdisplay("%@ LKdist_deadlock_thread: retrying msg %d\n",
			    in_msg->msg_seq);
		msgcount++;
	    }
	    else
	    {
		in_msg++;
	    }
	}

	/* Issue a no-read read to release the message space for reuse */
        (VOID) CXppp_read( LK_read_ppp_ctx, sizeof(LK_DSM), 
	                     0, 0, (char **)&in_msg,
			     &msgcount );

        /* Send accumulated cont messages after all input msgs have been digested */
	LK_flush_dsm_context( LK_cont_dsmc, &lkd->lkd_stat.cnt_gdlck_sent );

	/* Perodically hiccup to give LKdeadlock_thread a slice */
	if ( (lkd->lkd_stat.gdlck_call % (2 * MAX_DSMS_PER_CALL)) == 0 )
	{
	    status = CSsuspend(CS_TIMEOUT_MASK, 1, 0);

	    if ( status == E_CS0009_TIMEOUT )
		status = OK;
	}


    } /* End outer for loop */

    return ( E_DB_FATAL );
}


/* 
** Name: dsm_svc_name - return "service" name used by DSM communications.
**
** Description:
**
**	Generates service name (once) & returns pointer to it (always).
**
** Inputs:
**	None
**
** Outputs:
**	None
**
** Returns:
**	Pointer to cached service name.
**
** Exceptions:
**	None
**
** Side Effects:
**	None
**
** History:
**	17-May-2005 (devjo01)
**	    Created.
**
*/
static char *dsm_svc_name(void)
{
    static	char	service_name[10] = { '\0', };
    char		*instid;

    if ( '\0' == service_name[0] )
    {
	STcopy("II_CSP", service_name );
	NMgtAt("II_INSTALLATION", &instid );
	if ( instid && instid[0] && instid[1] )
	{
	    service_name[6] = '_';
	    service_name[7] = instid[0];
	    service_name[8] = instid[1];
	    service_name[9] = '\0';
	}
    }
    return service_name;
}

/*
** 
** Name: LK_dsm_context_priv_init - Initialization function for
**       LK_DSM_CONTEXT_PRIV.
**
** Description:
**
**	Initializes LK_DSM_CONTEXT_PRIV.
**
** Inputs:
**	lk_dsm_priv	- Structure to initialize.
**
** Outputs:
**	None
**
** Returns:
**	None
**
** Exceptions:
**	None
**
** Side Effects:
**	None
**
** History:
**	13-May-2005 (fanch01)
**	    Created.
**
*/
static void
LK_dsm_context_priv_init( LK_DSM_CONTEXT_PRIV *lk_dsm_priv )
{
    lk_dsm_priv->lk_dsm_ctx_head = 0;
    lk_dsm_priv->lk_dsm_ctx_tail = 0;
    lk_dsm_priv->lk_dsm_ctx_mark = 0;
    lk_dsm_priv->lk_dsm_ctx_handle_alloc = 0;
    lk_dsm_priv->lk_dsm_ctx_active = 0;
    lk_dsm_priv->lk_dsm_ctx_handle = NULL;
}

/*
** 
** Name: LK_alloc_dsm_context - Allocate a DSM context.
**
** Description:
**
**	Allocates all resources associated with an LK_DSM_CONTEXT.  Does NOT
**	open communication channels.  Buffers are allocated for all configured
**	nodes, except if writeonly set when buffers are not allocated for the
**	local node.
**	
** Inputs:
**	dsmc		- DSM context to allocate and initialize.
**
** Outputs:
**	None
**
** Returns:
**	OK on success,
**	other value indicates error
**
** Exceptions:
**	None
**
** Side Effects:
**	None
**
** History:
**	13-May-2005 (fanch01)
**	    Created.
**	12-Feb-2007 (jonj)
**	    Add message sequencing lk_dsm_ctx_msg_seq to aid debugging.
**
*/
STATUS
LK_alloc_dsm_context( LK_DSM_CONTEXT **hdsmc )
{
    STATUS status = -1 /* FIX-ME */;
    CX_NODE_INFO *node_info;
    LK_DSM_CONTEXT *dsmc;
    LK_DSM_CONTEXT_PRIV *lk_dsm_ctx_priv;
    i4 node;

    do
    {
	dsmc = (LK_DSM_CONTEXT *)MEreqmem(0, sizeof(LK_DSM_CONTEXT),
			 TRUE, &status);
	if ( NULL == ( *hdsmc = dsmc ) )
	{
	    break;
	}

	/* Read cluster configuration from config.dat. */
	if ( CXcluster_nodes( NULL, &dsmc->cx_configuration) )
	{
	    break;
	}

	for ( node = 0; node < CX_MAX_NODES; node++ )
	{
	    node_info = &dsmc->cx_configuration->cx_nodes[node];

	    /* node configured? */
	    if ( node_info->cx_node_number )
	    {
		lk_dsm_ctx_priv = &dsmc->lk_dsm_ctx_priv[node];

		/* allocate buffer */
		lk_dsm_ctx_priv->lk_dsm_ctx_ring = (LK_DSM *)
		    MEreqmem(0, sizeof(LK_DSM) * LK_DSM_CONTEXT_NELEM,
                             TRUE, &status);
		if (NULL == lk_dsm_ctx_priv->lk_dsm_ctx_ring)
		{
		    MEfree((PTR)dsmc );
		    *hdsmc = NULL;
		    break;
		}

		/* initialize private context */
		LK_dsm_context_priv_init( lk_dsm_ctx_priv );

		/* Init message sequence once */
		lk_dsm_ctx_priv->lk_dsm_ctx_msg_seq = 0;
	    }
	}

	status = OK;
    } while ( 0 );

    if (status == OK)
    {
	/* mark context as good */
	dsmc->lk_dsm_magic = LK_DSM_CONTEXT_MAGIC;
    }
    return (status);
}

/*
** 
** Name: LK_free_dsm_context - Free a DSM context.
**
** Description:
**
**	Close all open channels, deallocate buffers.
**	
** Inputs:
**	dsmc		- DSM context to free.
**
** Outputs:
**	None
**
** Returns:
**	OK on success,
**	other value indicates error
**
** Exceptions:
**	None
**
** Side Effects:
**	None
**
** History:
**	13-May-2005 (fanch01)
**	    Created.
**
*/
STATUS
LK_free_dsm_context( LK_DSM_CONTEXT **hdsmc )
{
    STATUS status = OK;
    CX_NODE_INFO *node_info;
    LK_DSM_CONTEXT *dsmc = *hdsmc;
    LK_DSM_CONTEXT_PRIV *lk_dsm_ctx_priv;
    i4 node;

    for ( ;; )
    {
	if (dsmc->lk_dsm_magic != LK_DSM_CONTEXT_MAGIC)
	{
	    /* double free (or potential uninitialized) error */
	    status = -1;
	    break;
	}
	/* set to invalid regardless of later error */
	dsmc->lk_dsm_magic = 0;

	for ( node = 0; node < CX_MAX_NODES; node++ )
	{
	    node_info = &dsmc->cx_configuration->cx_nodes[node];

	    /* node configured? */
	    if ( node_info->cx_node_number )
	    {
		lk_dsm_ctx_priv = &dsmc->lk_dsm_ctx_priv[node];

		/* deallocate buffer */
		MEfree( (PTR) lk_dsm_ctx_priv->lk_dsm_ctx_ring );

		/* node has transport open? */
		if ( lk_dsm_ctx_priv->lk_dsm_ctx_handle_alloc != 0 )
		{
		    lk_dsm_ctx_priv->lk_dsm_ctx_handle_alloc = 0;
		    CXppp_disconnect( lk_dsm_ctx_priv->lk_dsm_ctx_handle );
		    CXppp_free( &lk_dsm_ctx_priv->lk_dsm_ctx_handle );
		}
	    }
	}
	MEfree( (PTR)dsmc );
	*hdsmc = NULL;
	break;
    }

    return (status);
}

/*
** 
** Name: LK_mark_dsm_context - Sets a DSM context 'mark'.
**
** Description:
**
**	Makes a note of the current position for the output buffers. The
**	buffer is speculatively populated.  If it is determined that there
**	is no need to send the accumulated DSM records they can be released
**	by LK_unmark_dsm_context.
**	
** Inputs:
**	dsmc		- DSM context to set mark on.
**
** Outputs:
**	None
**
** Returns:
**	OK on success,
**	other value indicates error
**
** Exceptions:
**	None
**
** Side Effects:
**	None
**
** History:
**	13-May-2005 (fanch01)
**	    Created.
**
*/
STATUS
LK_mark_dsm_context( LK_DSM_CONTEXT *dsmc )
{
    STATUS status = OK;
    CX_NODE_INFO *node_info;
    LK_DSM_CONTEXT_PRIV *lk_dsm_ctx_priv;
    i4 node;

    for ( ;; )
    {
	if (dsmc->lk_dsm_magic != LK_DSM_CONTEXT_MAGIC)
	{
	    /* double free (or potential uninitialized) error */
	    status = -1;
	    break;
	}

	for ( node = 0; node < CX_MAX_NODES; node++ )
	{
	    node_info = &dsmc->cx_configuration->cx_nodes[node];

	    /* node configured? */
	    if ( node_info->cx_node_number )
	    {
		lk_dsm_ctx_priv = &dsmc->lk_dsm_ctx_priv[node];

		/* set mark */
		lk_dsm_ctx_priv->lk_dsm_ctx_mark = lk_dsm_ctx_priv->lk_dsm_ctx_head;
	    }
	}

	break;
    }

    return (status);
}

/*
** 
** Name: LK_unmark_dsm_context - Releases a DSM context 'mark'.
**
** Description:
**
**	Restore current end of unflushed regions to position saved by mark.
**	This releases the speculative entries that were added to the output
**	buffer which were previously marked by LK_mark_dsm_context.
**	
** Inputs:
**	dsmc		- DSM context to release mark of.
**
** Outputs:
**	None
**
** Returns:
**	OK on success,
**	other value indicates error
**
** Exceptions:
**	None
**
** Side Effects:
**	None
**
** History:
**	13-May-2005 (fanch01)
**	    Created.
**	12-Feb-2007 (jonj)
**	    Reset message sequence number.
**
*/
STATUS
LK_unmark_dsm_context( LK_DSM_CONTEXT *dsmc )
{
    STATUS status = OK;
    CX_NODE_INFO *node_info;
    LK_DSM_CONTEXT_PRIV *lk_dsm_ctx_priv;
    i4 node;

    for ( ;; )
    {
	if (dsmc->lk_dsm_magic != LK_DSM_CONTEXT_MAGIC)
	{
	    /* double free (or potential uninitialized) error */
	    status = -1;
	    break;
	}

	for ( node = 0; node < CX_MAX_NODES; node++ )
	{
	    node_info = &dsmc->cx_configuration->cx_nodes[node];

	    /* node configured? */
	    if ( node_info->cx_node_number )
	    {
		lk_dsm_ctx_priv = &dsmc->lk_dsm_ctx_priv[node];

		/* Reset message sequence */
		if ( lk_dsm_ctx_priv->lk_dsm_ctx_mark <=
			lk_dsm_ctx_priv->lk_dsm_ctx_head )
		    lk_dsm_ctx_priv->lk_dsm_ctx_msg_seq -=
		    	(lk_dsm_ctx_priv->lk_dsm_ctx_head -
			lk_dsm_ctx_priv->lk_dsm_ctx_mark);
		else
		    lk_dsm_ctx_priv->lk_dsm_ctx_msg_seq -=
			(lk_dsm_ctx_priv->lk_dsm_ctx_head +
			LK_DSM_CONTEXT_NELEM -
			lk_dsm_ctx_priv->lk_dsm_ctx_mark);

		/* restore head position to mark */
		lk_dsm_ctx_priv->lk_dsm_ctx_head = lk_dsm_ctx_priv->lk_dsm_ctx_mark;
	    }
	}

	break;
    }

    return (status);
}

/*
** 
** Name: LK_flush_dsm_context_write - Write from a private DSM context output
**	buffer.
**
** Description:
**
**	Write out count elements from the buffer starting at tail.
**	
** Inputs:
**	lk_dsm_ctx_priv	- DSM context to write from.
**	count		- Number of LK_DSM entries to write.
**
** Outputs:
**	None
**
** Returns:
**	OK on success,
**	other value indicates error
**
** Exceptions:
**	None
**
** Side Effects:
**	None
**
** History:
**	13-May-2005 (fanch01)
**	    Created.
**	21-Feb-2007 (jonj)
**	    Add optional *writes output param where number of
**	    records written may be accumulated.
**
*/
static STATUS
LK_flush_dsm_context_write( LK_DSM_CONTEXT_PRIV *lk_dsm_ctx_priv, 
i4 count, u_i4 *writes )
{
    /* write out last piece of buffer */
#if defined(LKDEADLOCK_DEBUG)
    if (lk_cluster_deadlock_debug)
    {
      TRdisplay("%@ LK_flush_dsm_context_write: call CXppp_write count %d\n",
	  count); TRflush();
    }
#endif
    STATUS status =
	CXppp_write( lk_dsm_ctx_priv->lk_dsm_ctx_handle, (char *)
	   &lk_dsm_ctx_priv->lk_dsm_ctx_ring[lk_dsm_ctx_priv->lk_dsm_ctx_tail],
	   sizeof(LK_DSM), count );

    if (status != OK)
    {
	/* log warning */
	TRdisplay("%@ LK_flush_dsm_context_write: CXppp_write error %x head %d"
		  " tail %d mark %d seq %d\n", status,
		lk_dsm_ctx_priv->lk_dsm_ctx_head,
		lk_dsm_ctx_priv->lk_dsm_ctx_tail,
		lk_dsm_ctx_priv->lk_dsm_ctx_mark,
		lk_dsm_ctx_priv->lk_dsm_ctx_msg_seq); TRflush();

	/* handle other failure contexts? */

	/* non-fatal anticipated error */
    }
    else if ( writes )
        *writes += count;

    return (status);
}

/*
** 
** Name: LK_flush_dsm_context - Flush the DSM output channels.
**
** Description:
**
**	Loop over buffers for each node apart from local node.  If the
**	current end of the write region is not at the beginning of the
**	write area, open channel if not done already, and ship out all the
**	DSM records.  This may require two writes per channel if the leading
**	edge of the write region has wrapped past the end of the ring buffer.
**	
** Inputs:
**	dsmc		- DSM context to flush.
**
** Outputs:
**	None
**
** Returns:
**	OK on success,
**	other value indicates error
**
** Exceptions:
**	None
**
** Side Effects:
**	None
**
** History:
**	13-May-2005 (fanch01)
**	    Created.
**	21-Feb-2007 (jonj)
**	    Add optional *writes output param where number of
**	    records written may be accumulated.
*/
STATUS
LK_flush_dsm_context( LK_DSM_CONTEXT *dsmc, u_i4 *writes )
{
    STATUS status = OK;
    CX_NODE_INFO *node_info;
    LK_DSM_CONTEXT_PRIV *lk_dsm_ctx_priv;
    LK_DSM_CONTEXT_PRIV lk_dsm_ctx_copy;
    char	nodename[CX_MAX_NODE_NAME_LEN];
    i4 		node;

    for ( ;; )
    {
	if (dsmc->lk_dsm_magic != LK_DSM_CONTEXT_MAGIC)
	{
	    /* double free (or potential uninitialized) error */
	    status = -1;
	    break;
	}

	for ( node = 0; node < CX_MAX_NODES; node++ )
	{
	    node_info = &dsmc->cx_configuration->cx_nodes[node];

	    /* node configured? */
	    if ( node_info->cx_node_number )
	    {
		lk_dsm_ctx_priv = &dsmc->lk_dsm_ctx_priv[node];

		/* was channel was active */
		if ( lk_dsm_ctx_priv->lk_dsm_ctx_active == 0 )
		    continue;

		/* was transport layer opened? */
		if ( lk_dsm_ctx_priv->lk_dsm_ctx_handle_alloc == 0 )
		{
		    status = CXppp_alloc( &lk_dsm_ctx_priv->lk_dsm_ctx_handle,
		                          0, 0, 0, lk_cluster_deadlock_debug );
		    if ( status != OK )
		    {
			/* log warning */
			TRdisplay("%@ LK_flush_dsm_context: alloc error %x\n",
			    status); TRflush();

			/* handle other failure contexts? */

			/* reset context */
			LK_dsm_context_priv_init( lk_dsm_ctx_priv );

			/* non-fatal anticipated error */
			continue;
		    }
		    STlcopy( node_info->cx_node_name, nodename,
		             node_info->cx_node_name_l );
		    status = CXppp_connect(
		      lk_dsm_ctx_priv->lk_dsm_ctx_handle, 
		        node_info->cx_node_name, dsm_svc_name() );
		    if ( status != OK )
		    {
			/* log warning */
			TRdisplay("%@ LK_flush_dsm_context: connect error %x\n",
			    status); TRflush();

			/* handle other failure contexts? */

			/* reset context */
			LK_dsm_context_priv_init( lk_dsm_ctx_priv );

			/* non-fatal anticipated error */
			continue;
		    }
		    lk_dsm_ctx_priv->lk_dsm_ctx_handle_alloc = 1;
		}

		/* save a copy in case of problems */
		lk_dsm_ctx_copy = *lk_dsm_ctx_priv;

		/* writes wrapped past end of buffer? */
		if ( lk_dsm_ctx_priv->lk_dsm_ctx_head <
		     lk_dsm_ctx_priv->lk_dsm_ctx_tail)
		{
#if defined(LKDEADLOCK_DEBUG)
		  if (lk_cluster_deadlock_debug)
		  {
		    TRdisplay("%@ LK_flush_dsm_context: call _write head %d"
			" tail %d\n", LK_DSM_CONTEXT_NELEM,
			lk_dsm_ctx_priv->lk_dsm_ctx_tail); TRflush();
		  }
#endif
		    status = LK_flush_dsm_context_write( lk_dsm_ctx_priv,
				 LK_DSM_CONTEXT_NELEM - lk_dsm_ctx_priv->lk_dsm_ctx_tail,
				 writes );
		    if ( status != OK )
		    {
			if (status == E_CL2C30_CX_E_OS_UNEX_FAIL)
			{
			    /*
			    ** Most likely a link disconnect.  Try resending
			    ** all messages to the same node.
			    */
			    *lk_dsm_ctx_priv = lk_dsm_ctx_copy;
			    lk_dsm_ctx_priv->lk_dsm_ctx_handle_alloc = 0;
			    TRdisplay("%@ LK_flush_dsm_context: attempt to "
				"resend to node %d\n",
				node_info->cx_node_number); TRflush();
			    --node;
			    continue;
			}
			else
			{
			    TRdisplay("%@ LK_flush_dsm_context: "
				"unexpected error %x\n", status); TRflush();
			    break;
			}
		    }

		    /* mark between tail and end of buffer? */
		    if ( lk_dsm_ctx_priv->lk_dsm_ctx_mark >
			 lk_dsm_ctx_priv->lk_dsm_ctx_tail)
		    {
			lk_dsm_ctx_priv->lk_dsm_ctx_mark = 0;
		    }

		    /* wrap tail to beginning */
		    lk_dsm_ctx_priv->lk_dsm_ctx_tail = 0;
		}

		/* more to flush out? */
		if ( lk_dsm_ctx_priv->lk_dsm_ctx_head >
		     lk_dsm_ctx_priv->lk_dsm_ctx_tail )
		{
#if defined(LKDEADLOCK_DEBUG)
		  if (lk_cluster_deadlock_debug)
		  {
		    TRdisplay("%@ LK_flush_dsm_context: call _write head %d"
			" tail %d\n", lk_dsm_ctx_priv->lk_dsm_ctx_head,
			lk_dsm_ctx_priv->lk_dsm_ctx_tail); TRflush();
		  }
#endif
		    status = LK_flush_dsm_context_write( lk_dsm_ctx_priv,
				 lk_dsm_ctx_priv->lk_dsm_ctx_head
				 - lk_dsm_ctx_priv->lk_dsm_ctx_tail,
				 writes );

		    if ( status != OK )
		    {
			if (status == E_CL2C30_CX_E_OS_UNEX_FAIL)
			{
			    /*
			    ** Most likely a link disconnect.  Try resending
			    ** all messages to the same node.
			    */
			    *lk_dsm_ctx_priv = lk_dsm_ctx_copy;
			    lk_dsm_ctx_priv->lk_dsm_ctx_handle_alloc = 0;
			    TRdisplay("%@ LK_flush_dsm_context: attempt to "
				"resend to node %d\n",
				node_info->cx_node_number); TRflush();
			    --node;
			    continue;
			}
			else
			{
			    TRdisplay("%@ LK_flush_dsm_context: "
				"unexpected error %x\n", status); TRflush();
			    break;
			}
		    }

		    /* mark between start of buffer and head? */
		    if ( lk_dsm_ctx_priv->lk_dsm_ctx_mark <
			 lk_dsm_ctx_priv->lk_dsm_ctx_head)
		    {
			lk_dsm_ctx_priv->lk_dsm_ctx_mark = lk_dsm_ctx_priv->lk_dsm_ctx_head;
		    }

		    /* tail catches head */
		    lk_dsm_ctx_priv->lk_dsm_ctx_tail = lk_dsm_ctx_priv->lk_dsm_ctx_head;
		}

		/* mark that we're caught up */
		lk_dsm_ctx_priv->lk_dsm_ctx_active = 0;
	    }
	}

	break;
    }

    return (status);
}

/*
** 
** Name: LK_get_free_dsm - Returns writeable record in the DSM output buffer.
**
** Description:
**
**	Returns pointer to current leading edge of write area, and advances
**	the leading edge.  If leading edge starts intruding on trailing edge
**	of write area then flush out some records, advancing begin of write
**	area as needed.  If this flush moves begin of write area past mark
**	pointer, adjust mark pointer also.
**	
** Inputs:
**	dsmc		- DSM context to return record from.
**	node		- Target node for the LK_DSM record.
**
** Outputs:
**	None
**
** Returns:
**	LK_DSM* to the writeable record.
**	NULL indicates error.
**
** Exceptions:
**	None
**
** Side Effects:
**	None
**
** History:
**	13-May-2005 (fanch01)
**	    Created.
**	12-Feb-2007 (jonj)
**	    Add lk_dsm_ctx_msg_seq.
**
*/
LK_DSM *
LK_get_free_dsm( LK_DSM_CONTEXT *dsmc, i4 node )
{
    STATUS status = OK;
    CX_NODE_INFO *node_info;
    LK_DSM_CONTEXT_PRIV *lk_dsm_ctx_priv;
    LK_DSM *lkdsm;
    i4 next_head;

    for ( ;; )
    {
	if (dsmc->lk_dsm_magic != LK_DSM_CONTEXT_MAGIC)
	{
	    /* double free (or potential uninitialized) error */
	    lkdsm = NULL;
	    break;
	}

	node_info = &dsmc->cx_configuration->cx_nodes[node - 1];

	/* node configured? */
	if ( node_info->cx_node_number == 0 )
	{
	    /* requested node not configured */
	    lkdsm = NULL;
	    break;
	}

	lk_dsm_ctx_priv = &dsmc->lk_dsm_ctx_priv[node - 1];

	/* next lkdsm */
	next_head = (lk_dsm_ctx_priv->lk_dsm_ctx_head + 1) % LK_DSM_CONTEXT_NELEM;
	lkdsm = &lk_dsm_ctx_priv->lk_dsm_ctx_ring[lk_dsm_ctx_priv->lk_dsm_ctx_head];

	/* overrun the tail? */
	if (next_head == lk_dsm_ctx_priv->lk_dsm_ctx_tail )
	{
	    /* complain */
	    TRdisplay("%@ warning: %s %d overrun tail for node(%d)\n",
			    __FILE__, __LINE__, node); TRflush();
	    LK_flush_dsm_context(dsmc, NULL);
	}

	/* update head */
	lk_dsm_ctx_priv->lk_dsm_ctx_head = next_head;
	/* indicate activity */
	lk_dsm_ctx_priv->lk_dsm_ctx_active = 1;

	/* Stamp with msg sequence */
	lkdsm->msg_seq = ++lk_dsm_ctx_priv->lk_dsm_ctx_msg_seq;

	break;
    }

    return (lkdsm);
}
/*
** 
** Name: LK_get_next_dsm - Returns pointer to next DSM 
** 			   between "tail" and "head"
**
** Description:
**
**	Returns pointer to next DSM between "tail" and "head" so one
**	may peruse messages accumulated for write.
**	
** Inputs:
**	dsmc		- DSM context to return record from.
**	node		- Target node for the LK_DSM record.
**	dsm_num		- index to next LK_DSM from previous call.
**
** Outputs:
**	dsm		- Pointer to LK_DSM, must be NULL on
**			  first call to this function.
**			  Will be NULL when head reached.
**	dsm_num		- updated to index of next LK_DSM
**
** Returns:
**	VOID
**
** Exceptions:
**	None
**
** Side Effects:
**	None
**
** History:
**	16-Feb-2007 (jonj)
**	    Created.
*/
VOID
LK_get_next_dsm( 
LK_DSM_CONTEXT *dsmc, 
i4 		node,
LK_DSM		**dsm,
i4		*dsm_num )
{
    CX_NODE_INFO 	*node_info;
    LK_DSM_CONTEXT_PRIV *ctx;
    LK_DSM 		*lkdsm = NULL;

    /* double free (or potential uninitialized) error? */
    if (dsmc->lk_dsm_magic == LK_DSM_CONTEXT_MAGIC)
    {
	node_info = &dsmc->cx_configuration->cx_nodes[node - 1];

	/* node configured? */
	if ( node_info->cx_node_number != 0 )
	{
	    ctx = &dsmc->lk_dsm_ctx_priv[node - 1];

	    if ( !(*dsm) )
		/* First "next", start at tail */
		*dsm_num = ctx->lk_dsm_ctx_tail;

	    /* Quit when we get to head */
	    if ( *dsm_num != ctx->lk_dsm_ctx_head )
	    {
		/* Return LK_DSM, update "next" */
		lkdsm = &ctx->lk_dsm_ctx_ring[*dsm_num];
		*dsm_num = (*dsm_num + 1) % LK_DSM_CONTEXT_NELEM;
	    }
	}
    }

    *dsm = lkdsm;

    return;
}

#else

DB_STATUS
LKdist_deadlock_thread( DMC_CB *dmc_cb )
{
    return E_DB_ERROR;
}

#endif
