/*
** Copyright (c) 1992, 2005 Ingres Corporation
**
NO_OPTIM=dr6_us5 rs4_us5 ris_u64 i64_aix r64_us5
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <di.h>
#include    <er.h>
#include    <me.h>
#include    <pc.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <tr.h>
#include    <ulf.h>
#include    <dmf.h>
#include    <dm.h>
#include    <lk.h>
#include    <cx.h>
#include    <lkdef.h>
#include    <lgkdef.h>

#ifdef VMS
	    /* pick up definition of SS$_DEBUG */
#include    <ssdef.h>
#include    <starlet.h>
#include    <lib$routines.h>
#endif

/**
**
**  Name: LKRLSE.C - Implements the LKrelease function of the locking system
**
**  Description:
**	This module contains the code which implements LKrelease.
**	
**	    LKrelease -- release a lock or a lock list.
**
**  History:
**	Summer, 1992 (bryanp)
**	    Working on the new portable logging and locking system.
**	15-oct-1992 (bryanp)
**	    Issue E_CL1039 correctly.
**	20-oct-1992 (bryanp)
**	    When cancelling a lock event wait, remove LLB from LKD EW queue.
**	18-jan-1993 (bryanp)
**	    Call CL_CLEAR_ERR to clear the system error code.
**	24-may-1993 (bryanp)
**	    Mutex optimizations: Release the LK_mutex before sending the message
**		to the CSP.
**	    Set lkreq.pid to this process's Process ID.
**	    Added lkreq argument to LK_wakeup for shared locklists. 
**	30-jun-1993 (shailaja)
**	    Fixed compiler warnings.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	26-jul-1993 (bryanp)
**	    Include <tr.h>
**	    Added some casts to pacify compilers.
**	20-sep-1993 (bryanp)
**	    Initialize unlock_status in LK_release_lock().
**	20-sep-1993 (bryanp) merged 23-mar-1993 (bryanp)
**	    Fix a number of bugs surrounding handling of invalid lock values:
**	    1) When a lock is marked invalid, we should not be resetting the
**		value to 0. Resetting the value to zero can cause the value to
**		eventually be re-incremented back into a range of lock values
**		which have been seen before, which can confuse callers into
**		believing that a lock value has not changed when in fact it
**		has (the lock was set to a value, invalidated, reset to 0, and
**		then eventually re-incremented back to the already seen value).
**	    2) When a lock is marked invalid, we must notify VMS of this so
**		that VMS can notify any other nodes.
**	12-oct-1993 (tad)
**	    Bug #56449
**	    Changed %x to %p for pointer values in LK_do_unlock().
**	17-nov-1994 (medji01)
**	    Mutex Granularity Project
**		- Changed LK_xmutex() calls to pass mutex field address.
**		- Added lkd pointer definitions where necessary.
**		- Remove LKD semaphore acquisition and free logic from 
**		  LKrelease().
**		- Acquire the LKD semaphore in LK_release() when searching
**		  LKH-LKB for a specific lock to be freed.
**		- Acquire RSB mutex while moving LKB from conversion to
**		  grant queue in LK_do_cancel().
**		- Acquire LKD, LKH, and RSB mutexes before calling 
**		  LK_do_unlock() in LK_do_cancel().
**		- In LK_release() and LK_cancel() there is no longer a need to
**		  free and reacquire the LKD mutex before calling LK_csp_send().
**		- Acquire the LKD semaphore in LK_cancel_event_wait() during
**		  the search of the event wait queue.
**		- Remove lkd mutex acquisition from LK_cancel_lock()
**		- Acquire LKD, LLB, LKH, and RSB semaphores when removing 
**		  an LKB in LK_do_unlock().
**	28-mar-1995 (medji01)
**	    64K+ Lock Limit Project
**		- Removed references to id_instance in LK_ID.
**		- Changed error messages to reference id_id instead
**		  of id_instance.
**	19-jul-1995 (canor01)
**	    Mutex Granularity Project
**		- Acquire LKD semaphore before calling LK_cancel_event_wait()
**		  or LK_deallocate_cb()
**		- Acquire LKD mutex before LLB mutex when both are required
**	11-Dec-1995 (jenjo02)
**	    Give LKevent wait queue its own mutex to isolate the code and
**	    avoid blocking on lkd_mutex.
**	    Moved LK_cancel_event_wait from here to lkevent.c to isolate
**	    all LKevent-type code.
**	    LK_deallocate_cb() handles its own (new) semaphores, no longer
**	    necesssary to acquire LKD mutex prior to the call.
**	    Maintenance of lkd_llb|rsb|lkb_inuse moved to 
**	    LK_allocate|deallocate_cb().
**	15-jan-1996 (duursma) bug #71335
**	    Fixed bug 71335 by ensuring that the proper status code is returned
**	    from LK_do_unlock().
**	21-aug-1996 (boama01)
**	    Corrections arising from Cluster Support testing on Alpha, but
**	    regarding bugs that affect all platforms:
**	    - Incorporated diagnostics using new global csp_debug, for runtime
**	      activation; added GLOBALREF for it; also improved and
**	      supplemented traces.
**	    - Changes to LK_try_wait() and all its callers; when Clustered RSB
**	      ends up unused, we deallocate it, but that unmutexes it too; must
**	      let callers know this so they do not attempt unmutex as well.
**	      Chg *RSB parm to **RSB so caller can see it's been nulled.
**	    - Changes to LKrelease(), LK_release() and LK_cancel(); do not
**	      return E_DMA017_LK_MUTEX_RELEASED anymore, as no mutex held by
**	      caller is released here.  This caused havoc in caller's response
**	      to LK_cancel() calls.
**	27-aug-96 (nick)
**	    LK_PARTIAL doesn't always mean lock escalation ; only increment
**	    release_partial if the lock key identifies the lock type as
**	    LK_PAGE. #71724
**	05-Sep-1996 (jenjo02)
**	    While spinning on dlock_mutex, release the llb_mutex to
**	    avoid a deadlock with deadlock().
**	08-Oct-1996 (jenjo02)
**	    Wrapped update of lkd_rqst_id in lkd_mutex.
**      01-aug-1996 (nanpr01 for ICL keving)
**          LK Blocking Callbacks Project. If removing an LKB with a callback
**          then decrement the rsb's rsb_cback_count.
**	15-Nov-1996 (jenjo02)
**	    Replaced rsb_dlock algorithm with lkd_dlock_mutex block of 
**	    wait-for matrix.
**	16-Nov-1996 (jenjo02)
**	    Ifdef'd code which check for an LLB already on the event wait
**	    queue. This code must have been originally placed to track
**	    a rare bug, one I've never encountered. Cleaned up code to 
**	    ensure that the event status bits are maintained correctly.
**	18-Nov-1996 (jenjo02)
**	    When undoing a conversion (LK_do_cancel()), restore lkb_request_mode 
**	    (the mode that failed), to the previously granted mode (lkb_grant_mode).
**      22-nov-1996 (dilma04/stial01)
**          Row-Level Locking Project:
**          Ignore invalid requests to release non-existing LK_PH_PAGE lock.   
**	06-Dec-1996 (jenjo02)
**	    Rearranged manipulation of RSH to avoid holding rsh_mutex too long.
**	    Removed lkd_mutex protection of lkd_rqst_id, which was creating
**	    significant blockage. lkd_rqst_id is paired with a lock list when
**	    it's needed, so duplicates which might result from not mutexing
**	    its value should be ok. Increment lkd_rqst_id while holding the
**	    llb_mutex to at least ensure that its value will be unique within
**	    a lock list.
**	18-dec-1996 (wonst02)
**	    Add lock_key parameter to CSsuspend call to track locks.
**	21-Jan-1997 (jenjo02)
**	    in LK_do_cancel(), if LLB is on the deadlock wait queue, remove it.
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          Supress error message and return LK_GRANTED if trying to 
**          release a logical lock within a transaction.
**      13-jun-97 (dilma04)
**          Do not decrement lkb_count, if trying to release a logical lock.
**          Remove code unneeded after optimization is made to release CS/RR
**          locks by handle.
**	06-Aug-1997 (jenjo02)
**	  o Moved handling of deadlocked LKB to LK_cancel() from lkrqst. 
**	  o Don't make CSget_sid() call unless necessary.
**	28-Oct-1997 (jenjo02)
**	    Added LK_NOLOG flag bit. If set and certain tolerable errors
**	    occur (lock not found), the error will not be logged, though
**	    the corresponding bad status will still be returned.
**	07-Jul-1998 (jenjo02)
**	    Remove spare unmutex of dlock_mutex in LK_do_cancel().
**	08-Sep-1998 (jenjo02)
**	    Added PHANTOM_RESUME_BUG debug code.
**      05-Nov-1998 (hanal04)
**          Tighten up the completion handle by adding the sid
**          and pid to the completion handle. This prevents the wrong
**          lkb->lkreq being used in LK_get_completion_status. b90388.
**          Crossed from 1.2/01.
**	    Search LKH/RSH without the aid of a mutex.
**	    Removed all those "CB_CHECK" macros, which just clutter the code.
**	16-Nov-1998 (jenjo02)
**	  o Cross-process thread identity changed to CS_CPID structure
**	    from PID and SID.
**	  o Installed macros to use ReaderWriter locks when CL-available
**	    for blocking the deadlock matrix.
**	  o Removed PHANTOM_RESUME_BUG code.
**	14-Jan-1999 (jenjo02)
**	    Recheck LLB_ON_DLOCK_LIST state after acquiring mutex in 
**	    LK_do_cancel().
**	08-Feb-1999 (jenjo02)
**	    Mods to accomodate a list of waiting LKBs per LLB instead of
**	    just one.
**	07-apr-1999 (nanpr01)
**	    Clear lockid field if passed after a successful lock release.
**	06-May-1999 (ahaal01)
**	    Added NO_OPTIM for rs4_us5 to eliminate error when trying to
**	    bring up recovery server.
**      21-May-1999 (hweho01)
**          Added NO_OPTIM for ris_u64 to eliminate error when trying to
**          bring up recovery server.
**	02-Aug-1999 (jenjo02)
**	    Added LLB parm to LK_deallocate_cb() prototype.
**	    Test (new) rsb_converters, rsb_waiters, before calling
**	    LK_try_convert/LK_try_wait.
**	06-Oct-1999 (jenjo02)
**	    Handle to LLB is now LLB_ID, no longer LK_ID.
**	22-Oct-1999 (jenjo02)
**	    Deleted lkd_dlock_lock.
**	30-Nov-1999 (jenjo02)
**	    Made "related" lock list an attribute of the PARENT_SHARED
**	    (referencing) list rather than the SHARED (referenced) list.
**	    Cleaned up and reorganized code in LK_release_lock which
**	    deals with this.
**	24-Aug-2000 (jenjo02)
**	    SHARED lock lists which are undergoing recovery may have
**	    a list of related lock lists. Modified LK_release_lock()
**	    to deal with this.
**        30-mar-94 (rachael)
**           Added call to CSnoresnow to provide a temporary fix/workaround
**           for sites experiencing bug 48904. CSnoresnow checks the current
**           thread for being CSresumed too early.  If it has been, that is if 
**           scb->cs_mask has the EDONE bit set, CScancelled is called.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**      25-May-1999 (hanal04)
**          In LK_do_cancel() once we have decided that the lock has not
**          been granted mark the lkb as LKB_CANCELLED whilst holding the
**          lkd_dlock_mutex. b97075.
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	12-apr-2001 (devjo01)
**	    s103715 (generic cluster support).
**	16-aug-2001 (toumi01)
**	    speculative i64_aix NO_OPTIM change for beta xlc_r - FIXME !!!
**	17-may-2002 (devjo01)
**	    Move constants for LK_do_unlock 'flag' arg to lkdef.h.
**      23-aug-2002 (devjo01)
**          Use LKB_DISTRIBUTED_LOCK instead of LKD_CLUSTER_SYSTEM
**          to support LK_LOCAL flag.
**      24-Jan-2003 (horda03) Bug 109528.
**          llb_status_mutex now used to protect updates to llb_status.
**      23-Jul-2003 (hweho01)
**          Turned off optimization for AIX 64-bit build (r64_us5).
**          Eliminated error when started up recovery server. 
**          Compiler : VisualAge 5.023.
**	17-sep-2003 (abbjo03)
**	    Changes to use VMS headers provided by HP.
**      7-oct-2004 (thaju02)
**          Use SIZE_TYPE to allow memory pools > 2Gig.
**	21-jul-2005 (devjo01)
**	    Pass CX_F_PROXY_OK to CXdlm routines, as these resquests
**	    can be redirected at need when running under VMS.
**      18-oct-2005 (horda03) Bug 115405/INGSRV3463
**          Fixed E_CL1037 error message parameters.
**	23-nov-2005 (devjo01) b115583
**	    Add return of distinct code LK_NOTHELD to provide better
**	    diagnostics and handling of locks implicitly freed by the
**	    exit of a process under Clustered Ingres for OpenVMS.
**      13-mar-2006 (horda03) Bug 112560/INGSRV2880
**          Modified LKrelease() to enable release of all
**          locks for a specified table.
**	21-nov-2006 (abbjo03)
**	    In LK_do_unlock, return OK instead of LK_NOTHELD.
**	20-Jun-2007 (jonj)
**	    Tally local vs distributed deadlocks when they are
**	    detected in LKrequest rather than here.
**	01-Oct-2007 (jonj)
**	    CBACK now embedded in LKB, not separate structure.
*/

#ifdef VMS
#define	SIGNAL_DEBUGGER		lib$signal(SS$_DEBUG)
#else
#define SIGNAL_DEBUGGER
#endif

#ifdef CSP_DEBUG
GLOBALREF i4  csp_debug;
#endif

static STATUS	LK_release(i4 flags, LK_LLID lock_list_id, LK_LKID *lockid,
			    LK_LOCK_KEY *lock_key, LK_VALUE *value,
			    CL_ERR_DESC *sys_err);



/*{
** Name: LKrelease	- Release a lock or a whole lock list.
**
** Description:
**      This function can perform the following functions:
**	release a whole lock list, release a partial lock list,
**	release a specific physical lock.  Releasing the whole lock
**	list is usually requested when a transaction commits or
**	aborts.  Releasing part of the lock list is used when converting
**	to larger granularity locking to release the lower granularity
**	resources.  Releasing a specific lock is used for objects that
**	are just locked while being looked at or changed and are locked
**	at a finer granularity.  A lock can be released by lock identifier
**	or by lock key.  If a value is specified and the lock is currently
**	granted in X or SIX mode the value is copied into the lock.  If
**	no value is specified the value is set to 0.
**
** Inputs:
**      flags	                        Must be zero, LK_PARTIAL, LK_AL_PHYS or
**					LK_ALL.
**      lli                             Lock list identifier.
**
**	lockid				If flags != LK_ALL then this
**					is a release of a specific
**					lock.
**	lockkey				If flags == zero  and lockid
**					was not specified or flag ==
**					LK_PARTIAL then this is the lockkey
**					and is used to determine the lock.
**	value				If this is a single lock release
**					and granted mode is SIX or X then
**					this value is copied to the lock.
**
** Outputs:
**	Returns:
**	    OK				Successful completion.
**	    LK_BADPARAM			Bad parameter.
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	Summer, 1992 (bryanp)
**	    Working on the new portable logging and locking system.
**	18-jan-1993 (bryanp)
**	    Call CL_CLEAR_ERR to clear the system error code.
**	24-may-1993 (bryanp)
**	    Check for and handle E_DMA017_LK_MUTEX_RELEASED from LK_release.
**	17-nov-1994 (medji01)
**	    Mutex Granularity Project
**		- Remove LKD semaphore acquisition and free logic from 
**		  LKrelease().
**	21-aug-1996 (boama01)
**	    Removed handling of E_DMA017_LK_MUTEX_RELEASED from LK_release;
**	    it will not return this anymore.
**	18-dec-1996 (wonst02)
**	    Add lock_key parameter to CSsuspend call to track locks.
**	07-apr-1999 (nanpr01)
**	    Clear lockid field if passed after a successful lock release.
*/
STATUS
LKrelease(
i4		    flags,
LK_LLID		    lock_list_id,
LK_LKID		    *lockid,
LK_LOCK_KEY	    *lock_key,
LK_VALUE	    *value,
CL_ERR_DESC	    *sys_err)
{
    STATUS	status;
    i4	err_code;

    LK_WHERE("LKrelease")

    CL_CLEAR_ERR(sys_err);

    /* Remove any Rogue Resumes */
    CSnoresnow( "LK!LKRLSE.C::LKrelease_1", 1);

    status = LK_release(flags, lock_list_id, lockid, lock_key, value, sys_err);

    /* Clear lockid field if passed and lock release is successful */
    if (lockid && (status == OK || status == LK_NOTHELD))
    {
	lockid->lk_unique = 0;
	lockid->lk_common = 0;
    }
    
    return(status);
}

/*{
** Name: LK_release	- Release a lock or a whole lock list.
**
** Description:
**      This function can perform the following functions:
**	release a whole lock list, release a partial lock list,
**	release a specific physical lock.  Releasing the whole lock
**	list is usually requested when a transaction commits or
**	aborts.  Releasing part of the lock list is used when converting
**	to larger granularity locking to release the lower granularity
**	resources.  Releasing a specific lock is used for objects that
**	are just locked while being looked at or changed and are locked
**	at a finer granularity.  A lock can be released by lock identifier
**	or by lock key.  If a value is specified and the lock is currently
**	granted in X or SIX mode the value is copied into the lock.  If
**	no value is specified the value is set to 0.
**
**	If the list released is a shared lock list, then the locks are not
**	actually released until the last reference to the shared list is
**	released.
**
** Inputs:
**      flags	                        Can be ONE or NONE of following:
**					LK_ALL - release all locks on lock list
**					LK_PARTIAL - release partial list
**					LK_AL_PHYS - release all physical locks
**					Optionally may include:
**					LK_MULTITHREAD - lock list is shared
**      lli                             Lock list identifier.
**
**	lockid				If flags != LK_ALL then this
**					is a release of a specific
**					lock.
**	lockkey				If flags == zero  and lockid
**					was not specified or flag ==
**					LK_PARTIAL then this is the lockkey
**					and is used to determine the lock.
**	value				If this is a single lock release
**					and granted mode is SIX or X then
**					this value is copied to the lock.
**
** Outputs:
**	Returns:
**	    OK			Successful completion. If cluster, must suspend
**	    LK_GRANTED		Successful completion. No suspension needed on
**					cluster
**	    LK_BADPARAM		Bad parameter.
**	    other		All hell broke loose.
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	Summer, 1992 (bryanp)
**	    Working on the new portable logging and locking system.
**	15-oct-1992 (bryanp)
**	    Issue E_CL1039 correctly.
**	16-feb-1993 (bryanp)
**	    When we synchronously release a physical lock without invoking
**	    LK_release_lock, return LK_GRANTED so that the cluster code knows
**	    it doesn't need to suspend.
**	24-may-1993 (bryanp)
**	    Mutex optimizations: Release the LK_mutex before sending the message
**		to the CSP. Return special E_DMA017_LK_MUTEX_RELEASED so that
**		our caller knows we've released the mutex for him.
**	    Set lkreq.pid to this process's Process ID.
**	26-jul-1993 (bryanp)
**	    Added some casts to pacify compilers.
**	17-nov-1994 (medji01)
**	    Mutex Granularity Project
**		- Changed LK_xmutex() calls to pass mutex field address.
**		- Acquire the LKH semaphore in LK_release() when searching
**		  LKH-LKB for a specific lock to be freed.
**              - There is no longer a need to free and reacquire the
**                LKD mutex before calling LK_csp_send().
**	28-mar-1995 (medji01)
**	    64K+ Lock Limit Project
**		- Removed references to id_instance in LK_ID.
**		- Changed error messages to reference id_id instead
**		  of id_instance.
**	21-aug-1996 (boama01)
**	    Mutex Granularity Project: medji01 removed the defunct free/lock
**	    of LKD around LK_csp_send(), but did not eliminate returning the
**	    E_DMA017_LK_MUTEX_RELEASED status.  This is obsolete.  Now
**	    returning status from LK_csp_send (OK or FAIL) (also removed
**	    leftover comments).
**	08-Oct-1996 (jenjo02)
**	    Wrapped update of lkd_rqst_id in lkd_mutex.
**      22-nov-1996 (dilma04/stial01)
**          Row-Level Locking Project:
**          Ignore invalid requests to release non-existing LK_PH_PAGE lock.   
**      22-jan-1997 (dilma04)
**          Ignore invalid requests to release a logical lock within a 
**          transaction. 
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          Supress error message and return LK_GRANTED if trying to 
**          release a logical lock within a transaction.
**      13-jun-97 (dilma04)
**          Do not decrement lkb_count, if trying to release a logical lock.
**          Remove code unneeded after optimization is made to release CS/RR
**          locks by handle.
**	28-Oct-1997 (jenjo02)
**	    Added LK_NOLOG flag bit. If set and certain tolerable errors
**	    occur (lock not found), the error will not be logged, though
**	    the corresponding bad status will still be returned.
**	28-Feb-2002 (jenjo02)
**	    LK_MULTITHREAD is now an attribute of a lock list as well as
**	    a possible input flag.
**	    Removed resetting of LLB_WAITING, which is now contrived
**	    based on the value of llb_waiters.
**	05-Apr-2004 (jenjo02)
**	    Check RSB==NULL during unmutexed scan of llb->lkb; highly
**	    concurrent activity on a shared lock list (lock+release
**	    at the same time) may yield odd intermediate values.
**	22-Jun-2004 (jenjo02)
**	    Repairs to concurrency problems with physical locks on
**	    multithread/shared lock lists. RSBs must be mutexed
**	    and rechecked while modifying lkb_count.
**	10-Aug-2004 (jenjo02)
**	    Inserted some CS_breakpoints for debuggging.
**	    If lock can't be found, mutex the LLB; the
**	    lock list may be in the process of being
**	    converted to a SHARED list, in which case
**	    all the locks will be relocated to the SHARED
**	    lock list.
**	14-Nov-2007 (jonj)
**	    Don't assume that it's ok to ignore LK_mutex() failures;
**	    if the mutex owner is dead, we can't make assumptions
**	    about the mutexed structure.
**	    
*/
static STATUS
LK_release(
i4		    flags,
LK_LLID		    lock_list_id,
LK_LKID		    *lockid,
LK_LOCK_KEY	    *lock_key,
LK_VALUE	    *value,
CL_ERR_DESC	    *sys_err)
{
    LKD                 *lkd = (LKD *)LGK_base.lgk_lkd_ptr;
    LLB			*llb;
    LLB			*proc_llb = NULL;
    LLB			*prev_llb;
    LLB			*next_llb;
    LKB			*lkb = NULL;
    RSB			*rsb;
    LLBQ		*next_llbq;
    LLBQ		*last_llbq;
    LKBQ		*lkbq;
    LK_ID		input_lkb_id;
    LLB_ID		*input_list_id = (LLB_ID *) &(lock_list_id);
    STATUS		status, MutexStatus;
    LKH    		*lkh_table;
    SIZE_TYPE		*lbk_table;
    SIZE_TYPE		*sbk_table;
    i4		err_code;
    SIZE_TYPE		end_offset;
    SIZE_TYPE		llbq_offset;
    LKREQ		lkreq;
    CL_ERR_DESC		local_sys_err;
    STATUS		local_status;
    LKH			*lkh_hash_bucket;

    LK_WHERE("LK_release")

    /* Validate the lock list id. */

    if (input_list_id->id_id == 0 ||
	(i4) input_list_id->id_id > lkd->lkd_lbk_count)
    {
	ule_format(E_CL1036_LK_RELEASE_BAD_PARAM, (CL_ERR_DESC *)0, ULE_LOG,
			NULL, (char *)0, 0L, (i4 *)0, &err_code, 2,
			0, input_list_id->id_id, 0, lkd->lkd_lbk_count);
	CS_breakpoint();
	return(LK_BADPARAM);
    }

    lbk_table = (SIZE_TYPE *)LGK_PTR_FROM_OFFSET(lkd->lkd_lbk_table);
    llb = (LLB *)LGK_PTR_FROM_OFFSET(lbk_table[input_list_id->id_id]);

    if ( llb->llb_status & LLB_MULTITHREAD )
	flags |= LK_MULTITHREAD;

    if ((llb->llb_type != LLB_TYPE) || 
	llb->llb_id.id_instance != input_list_id->id_instance ||
	(llb->llb_waiters && ((flags & LK_MULTITHREAD) == 0))
       )
    {
	ule_format(E_CL1037_LK_RELEASE_BAD_PARAM, (CL_ERR_DESC *)0, ULE_LOG,
			NULL, (char *)0, 0L, (i4 *)0, &err_code, 4,
			0, *(u_i4*)input_list_id, 0, llb->llb_type, 
			0, *(u_i4*)&llb->llb_id,
			0, llb->llb_status);
	CS_breakpoint();
        return(LK_BADPARAM);
    }

    /*
    ** If this is a handle to a shared lock list then switch to use the shared
    ** list.  Save the pointer to the handle in case the entire list is to
    ** be released.  In this case we will release the handle.
    */
    proc_llb = llb;
    if (llb->llb_status & LLB_PARENT_SHARED)
    {
	if (llb->llb_shared_llb == 0)
	{
	    ule_format(E_CL1053_LK_RELEASE_BADPARAM, (CL_ERR_DESC *)0, ULE_LOG,
			NULL, (char *)0, 0L, (i4 *)0, &err_code, 1,
			0, input_list_id->id_id);
	    CS_breakpoint();
	    return (LK_BADPARAM);
	}

	llb = (LLB *)LGK_PTR_FROM_OFFSET(llb->llb_shared_llb);

	if ((llb->llb_type != LLB_TYPE) ||
	    ((llb->llb_status & LLB_SHARED) == 0))
	{
	    ule_format(E_CL1052_LK_RELEASE_BADPARAM, (CL_ERR_DESC *)0, ULE_LOG,
			NULL, (char *)0, 0L, (i4 *)0, &err_code, 3,
			0, input_list_id->id_id, 0, llb->llb_type, 
			0, llb->llb_status);
	    CS_breakpoint();
	    return (LK_BADPARAM);
	}
	/* Use multithread semantics */
	flags |= LK_MULTITHREAD;
    }

    /* If this is a release of a specific lock, find the LKB. */
	
    if ((flags & (LK_ALL | LK_PARTIAL | LK_AL_PHYS)) == 0)
    {
	/*  Find the LKB by lockid or by lock_key. */

	if (lockid)
	{
	    input_lkb_id = lockid->lk_unique;
	    if (input_lkb_id == 0 ||
		input_lkb_id > lkd->lkd_sbk_count)
	    {
		ule_format(E_CL1038_LK_RELEASE_BAD_PARAM,
			(CL_ERR_DESC *)0, ULE_LOG,
			NULL, (char *)0, 0L, (i4 *)0, &err_code, 2,
			 0, input_lkb_id, 0, lkd->lkd_sbk_count);
		CS_breakpoint();
		return (LK_BADPARAM);
	    }

	    sbk_table = (SIZE_TYPE *)LGK_PTR_FROM_OFFSET(lkd->lkd_sbk_table);
	    lkb = (LKB *)LGK_PTR_FROM_OFFSET(sbk_table[input_lkb_id]);

	    if (lkb->lkb_type != LKB_TYPE ||
		lkb->lkb_lkbq.lkbq_llb != LGK_OFFSET_FROM_PTR(llb))
	    {
		ule_format(E_CL1039_LK_RELEASE_BAD_PARAM,
			(CL_ERR_DESC *)0, ULE_LOG,
			NULL, (char *)0, 0L, (i4 *)0, &err_code, 3,
			0, input_lkb_id, 0, lkb->lkb_type, 
			0, lkb->lkb_id);
		CS_breakpoint();
		return (LK_BADPARAM);
	    }
	}
	else if (lock_key)
	{
	    SIZE_TYPE	lkh_offset;
	    i4	next_lkbq;
	    SIZE_TYPE	llb_offset = LGK_OFFSET_FROM_PTR(llb);
	    i4  llb_mutexed = FALSE;

	    lkh_table = (LKH *)LGK_PTR_FROM_OFFSET(lkd->lkd_lkh_table);
	    lkh_hash_bucket = &lkh_table
		[((unsigned)(lock_key->lk_type + lock_key->lk_key1 +
		    lock_key->lk_key2 + lock_key->lk_key3 +
		    lock_key->lk_key4 + lock_key->lk_key5 + 
		    lock_key->lk_key6 + (unsigned)LGK_OFFSET_FROM_PTR(llb)))
		% lkd->lkd_lkh_size];
	    lkbq = (LKBQ *)lkh_hash_bucket;

	    /* Search the hash without a mutex */
	    lkh_offset = LGK_OFFSET_FROM_PTR(lkbq);
	    next_lkbq = lkbq->lkbq_next;

	    while ( lkb == NULL )
	    {
		while ( next_lkbq )
		{
		    lkbq = (LKBQ *)LGK_PTR_FROM_OFFSET(next_lkbq);
		    next_lkbq = lkbq->lkbq_next;

		    if (lkbq->lkbq_llb == llb_offset)
		    {
			rsb = (RSB *)LGK_PTR_FROM_OFFSET(lkbq->lkbq_rsb);
			if ( rsb &&
			    rsb->rsb_name.lk_type == lock_key->lk_type &&
			    rsb->rsb_name.lk_key1 == lock_key->lk_key1 &&
			    rsb->rsb_name.lk_key2 == lock_key->lk_key2 &&
			    rsb->rsb_name.lk_key3 == lock_key->lk_key3 &&
			    rsb->rsb_name.lk_key4 == lock_key->lk_key4 &&
			    rsb->rsb_name.lk_key5 == lock_key->lk_key5 &&
			    rsb->rsb_name.lk_key6 == lock_key->lk_key6)
			{
			    lkb = (LKB *)((char *)lkbq - CL_OFFSETOF(LKB,lkb_lkbq));
			    break;
			}
		    }
		    if ( lkbq->lkbq_next != next_lkbq ||
			 lkbq->lkbq_lkh  != lkh_offset )
		    {
			lkbq = (LKBQ *)lkh_hash_bucket;
			next_lkbq = lkbq->lkbq_next;
		    }
		}
		/* Not found. Try again with llb_mutex */
		if ( lkb == NULL && llb_mutexed == FALSE )
		{
		    if ( MutexStatus = LK_mutex(SEM_EXCL, &llb->llb_mutex) )
			return(MutexStatus);
		    llb_mutexed = TRUE;
		    if ( llb == proc_llb && llb->llb_status & LLB_PARENT_SHARED )
		    {
			/*
			** Then this lock list has been converted
			** to a SHARED list handle and all locks
			** moved to the shared list.
			** Redo the request, this time looking for
			** the lock on the SHARED list.
			*/
			(VOID)LK_unmutex(&llb->llb_mutex);
			return( LK_release(flags, lock_list_id, lockid,
					   lock_key, value, sys_err) );
		    }
		    /* Restart from the top of the list */
		    lkbq = (LKBQ *)lkh_hash_bucket;
		    next_lkbq = lkbq->lkbq_next;
		}
		else
		    break;
	    }
	    if ( llb_mutexed )
		(VOID)LK_unmutex(&llb->llb_mutex);
	}
	else
	{
	    ule_format(E_CL103A_LK_RELEASE_BAD_PARAM, (CL_ERR_DESC *)0, ULE_LOG,
			NULL, (char *)0, 0L, (i4 *)0, &err_code, 0);
	    return (LK_BADPARAM);
	}

	if (lkb == 0)
	{
	    if ((flags & LK_NOLOG) == 0)
	    {
		ule_format(E_CL103B_LK_RELEASE_BAD_PARAM, (CL_ERR_DESC *)0, ULE_LOG,
			    NULL, (char *)0, 0L, (i4 *)0, &err_code, 2,
			    0, lkb, 0, (lkb ? lkb->lkb_attribute : -1));
	    }
	    CS_breakpoint();
	    return (LK_BADPARAM);
	}

	if ( (lkb->lkb_attribute & LKB_PHYSICAL) == 0 )
	    return(OK);

	/* 
	** If multithreaded/shared list, must mutex the RSB
	** while changing the lkb_count.
	*/
	rsb = (RSB*)LGK_PTR_FROM_OFFSET(lkb->lkb_lkbq.lkbq_rsb);
	if ( flags & LK_MULTITHREAD &&
	    (MutexStatus = LK_mutex(SEM_EXCL, &rsb->rsb_mutex)) )
	{
	    return(MutexStatus);
	}

	if ( --lkb->lkb_count != 0 )
	{
	    if ( flags & LK_MULTITHREAD )
		LK_unmutex(&rsb->rsb_mutex);
	    return(OK);
	}
	if ( flags & LK_MULTITHREAD )
	{
	    /* This is the lock we intend to free */
	    if (!lock_key)
		STRUCT_ASSIGN_MACRO(rsb->rsb_name, lkreq.lock_key);
	    /* Release the mutex; we'll recheck the LKB vs RSB */
	    LK_unmutex(&rsb->rsb_mutex);
	    /* Fall through to release the lock */
	}
    }

#ifdef CSP_DEBUG
    if (csp_debug)
    {
    LK_UNIQUE	lock_id;

    CX_EXTRACT_LOCK_ID( &lock_id, &lkb->lkb_cx_req.cx_lock_id );
    TRdisplay("%@ LK_release Entry:");
    if (lkb)      TRdisplay("  DLM LKid %x.%x",
	lock_id.lk_uhigh, lock_id.lk_ulow );
    if (lockid)   TRdisplay("  lock_id %x.%x", 
	lockid->lk_unique, lockid->lk_common);
    TRdisplay("  flags=%x\n",flags);
    if (lock_key) TRdisplay("  lk_type %d, key (%x %x %x %x %x %x) (%t)\n",
	lock_key->lk_type,
	lock_key->lk_key1, lock_key->lk_key2, lock_key->lk_key3,
	lock_key->lk_key4, lock_key->lk_key5, lock_key->lk_key6,
	24, &lock_key->lk_key1);
    TRflush();
    }
#endif
    /*	See if this is a partial release.  */

    if (flags & LK_PARTIAL)
    {
	if (lock_key == 0)
	{
	    ule_format(E_CL103C_LK_RELEASE_BAD_PARAM, (CL_ERR_DESC *)0, ULE_LOG,
			NULL, (char *)0, 0L, (i4 *)0, &err_code, 0);
	    return (LK_BADPARAM);
	}
    }

    /*
    ** All parameters have been checked; now either call LK_release_lock()
    ** directly or send a message to the CSP to call LK_release_lock().
    */

    lkreq.function = LKREQ_RELEASE_LOCK;
    lkreq.llb_offset = LGK_OFFSET_FROM_PTR(proc_llb);
    lkreq.lkb_offset = LGK_OFFSET_FROM_PTR(lkb);
    lkreq.flags = flags;
    if (lock_key)
	STRUCT_ASSIGN_MACRO(*lock_key, lkreq.lock_key);
    if (value)
	STRUCT_ASSIGN_MACRO(*value, lkreq.value);

    status = LK_release_lock(&lkreq);
    return (status);
}

/*
** Name: LK_release_lock	- body of the lock release handling.
**
** Description:
**	This routine implements the bulk of lock releasing. In non-cluster
**	locking systems, it is called directly by LK_release. In cluster
**	locking systems, it is called by the CSP process as the result of a
**	"release-lock" message queued to the CSP.
**
** Inputs:
**	lkreq			- the lock release message packet.
**
** Outputs:
**	None, but many side effects.
**
** Returns:
**	STATUS
**
** History:
**	16-feb-1993 (bryanp)
**	    Added history comments, cleaned up handling of LK_do_unlock status.
**	24-may-1993 (bryanp)
**	    Added lkreq argument to LK_wakeup for shared locklists. 
**	20-sep-1993 (bryanp)
**	    Initialize unlock_status in LK_release_lock().
**	17-nov-1994 (medji01)
**	    Mutex Granularity Project
**		- Changed LK_xmutex() calls to pass mutex field address.
**              - Acquire the LKD mutex before calling LK_deallocate_cb().
**	28-mar-1995 (medji01)
**	    64K+ Lock Limit Project
**		- Removed references to id_instance in LK_ID.
**		- Changed error messages to reference id_id instead
**		  of id_instance.
**	11-Dec-1995 (jenjo02)
**	    LK_deallocate_cb() handles its own (new) semaphores, no longer
**	    necesssary to acquire LKD mutex prior to the call.
**	    Cleaned up semaphore handling in general.
**	24-feb-1996 (duursma)
**	    Fix to above change, which would cause an immediate return if
**	    on a cluster system.
**	27-aug-96 (nick)
**	    release_partial becomes the escalation counter to the outside 
**	    world ; only increment if we are using LK_PARTIAL with a lock
**	    type of LK_PAGE as using LK_PARTIAL with other types isn't
**	    a result of escalation.
**	24-Aug-2000 (jenjo02)
**	    SHARED lock lists which are undergoing recovery may have
**	    a list of related lock lists. Modified LK_release_lock()
**	    to deal with this. Free related lock lists, if we must,
**	    after all other locks have been released.
**	18-Sep-2001 (jenjo02)
**	    When deallocating lock/resource, set LLB_ALLOC to 
**	    keep scavenge() from stealing our RSB/LKB stash.
**	01-Jul-2004 (jenjo02)
**	    Release SHARED LLB's mutex before acquiring the handle
**	    and LLB queue mutexes to avoid deadlock.
**	10-Aug-2004 (jenjo02)
**	    After mutexing LLB, check if it's now been 
**	    converted to a SHARED list, which changes the
**	    semantics.
**	13-Mar-2006 (jenjo02)
**	    Deprecated lkd_rqst_id, use llb_stamp instead.
**	19-Mar-2007 (jonj)
**	    Add IsRundown parm to LK_cancel_event_wait()
*/
STATUS
LK_release_lock(LKREQ *lkreq)
{
    LKD			*lkd = (LKD *)LGK_base.lgk_lkd_ptr;
    LLB			*llb;
    SIZE_TYPE		related_llb;
    LLB			*proc_llb = NULL;
    LLB			*prev_llb;
    LLB			*next_llb;
    LKB			*lkb = NULL;
    RSB			*rsb;
    LLBQ		*next_llbq;
    LLBQ		*last_llbq;
    LKBQ		*lkbq;
    STATUS		status;
    LKH    		*lkh_table;
    i4			err_code;
    SIZE_TYPE		end_offset;
    SIZE_TYPE		llbq_offset;
    CL_ERR_DESC		sys_err;
    STATUS		unlock_status = OK;
    STATUS		mutex_status;

    LK_WHERE("LK_release_lock")

    if (lkd->lkd_status & LKD_CSP_RUNDOWN)
	return(OK);

    llb = (LLB *)LGK_PTR_FROM_OFFSET(lkreq->llb_offset);

    /* Extract the related LLB from the "handle" LLB */
    related_llb = llb->llb_related_llb;

    /*
    ** If this is a handle to a shared lock list then switch to use the shared
    ** list.  Save the pointer to the handle in case the entire list is to
    ** be released.  In this case we will release the handle.
    */
    if (llb->llb_status & LLB_PARENT_SHARED)
    {
	proc_llb = llb;
	llb = (LLB *)LGK_PTR_FROM_OFFSET(proc_llb->llb_shared_llb);
    }

    if ((mutex_status = LK_mutex(SEM_EXCL, &llb->llb_mutex)))
    {
	TRdisplay("Panic: LK_mutex failure in LK_release_lock!\n");
	return (mutex_status);
    }

    if ( llb->llb_status & LLB_PARENT_SHARED && proc_llb == NULL )
    {
	/*
	** Then this list was converted to a shared list while
	** we waited for the mutex. Switch to the SHARED list;
	** that's where the locks are now.
	*/
	lkreq->flags |= LK_MULTITHREAD;
	proc_llb = llb;
	llb = (LLB *)LGK_PTR_FROM_OFFSET(proc_llb->llb_shared_llb);
	(VOID)LK_unmutex(&proc_llb->llb_mutex);
	if ( mutex_status = LK_mutex(SEM_EXCL, &llb->llb_mutex) )
	    return(mutex_status);
    }


    /* Preserve our RSB/LKB stash */
    llb->llb_status |= LLB_ALLOC;

    /* Update llb_stamp while holding the llb_mutex */
    lkreq->request_id = ++llb->llb_stamp;

    if ((lkreq->flags & (LK_ALL | LK_PARTIAL | LK_AL_PHYS)) == 0)
    {
	/* Release the single lock passed in lkreq */
	lkd->lkd_stat.release++;

	lkb = (LKB *)LGK_PTR_FROM_OFFSET(lkreq->lkb_offset);

	/*
	** If a multithreaded/shared lock list, the RSB mutex
	** was released before we were called, giving another
	** thread an opportunity to manipulate the lock.
	** Recheck that the LKB/RSB still match and that
	** the lkb_count is still zero.
	*/
	rsb = (RSB*)LGK_PTR_FROM_OFFSET(lkb->lkb_lkbq.lkbq_rsb);

	if ( rsb )
	{
	    if ( mutex_status = LK_mutex(SEM_EXCL, &rsb->rsb_mutex) )
		return(mutex_status);

	    if ( lkreq->flags & LK_MULTITHREAD )
	    {
		/*
		** If LKB is no longer owned by this LLB or if it
		** is now linked to another lock resource,
		** then this lock release is invalidated.
		*/
	        if ( lkb->lkb_lkbq.lkbq_llb != LGK_OFFSET_FROM_PTR(llb) ||
		     MEcmp((char*)&lkreq->lock_key, 
			   (char*)&rsb->rsb_name, sizeof(LK_LOCK_KEY)) )
		{
		    (VOID)LK_unmutex(&rsb->rsb_mutex);
		    rsb = (RSB*)NULL;
		}
	    }

	    if ( rsb && lkb->lkb_count != 0 )
	    {
		(VOID)LK_unmutex(&rsb->rsb_mutex);
		rsb = (RSB*)NULL;
	    }
	}

	if ( rsb )
	{
	    /* Tell LK_do_unlock we hold the mutex already */
	    lkb->lkb_attribute |= LKB_RSB_IS_MUTEXED;
	    unlock_status = LK_do_unlock(lkb, &lkreq->value, (i4)0);
	}
	else
	    /* Not the same lock, ignore the unlock request */
	    unlock_status = OK;
    }
    else if (lkreq->flags & LK_PARTIAL)
    {
        if (lkreq->flags & LK_RELEASE_TBL_LOCKS)
        {
           /* This is a release of all locks held on a table.
           ** The locks are released in reverse order from which
           ** they were taken.
           */
           end_offset = LGK_OFFSET_FROM_PTR(&llb->llb_llbq);
           for (llbq_offset = llb->llb_llbq.llbq_prev;
                llbq_offset != end_offset;
               )
           {
               /*
               **  Move backwards from the LLBQ in the LKB to the beginning of the
               **  LKB.
               */
               next_llbq = (LLBQ *)LGK_PTR_FROM_OFFSET(llbq_offset);

               /*  Compute the address of the LKB. */

               lkb = (LKB *)((char *)next_llbq - CL_OFFSETOF(LKB,lkb_llbq));

               /*       Skip to the next LLBQ in case we unlock this LKB. */

               llbq_offset = next_llbq->llbq_prev;

               /* See if the lock is for the table. */

               rsb = (RSB *)LGK_PTR_FROM_OFFSET(lkb->lkb_lkbq.lkbq_rsb);

               if (rsb->rsb_name.lk_key1 == lkreq->lock_key.lk_key1 &&
                   rsb->rsb_name.lk_key2 == lkreq->lock_key.lk_key2 &&
                   rsb->rsb_name.lk_key3 == lkreq->lock_key.lk_key3)
               {
                   unlock_status = LK_do_unlock(lkb, (LK_VALUE *) 0, (i4)0);
               }
           }
        }
        else
        {
	    if (lkreq->lock_key.lk_type == LK_PAGE)
	        lkd->lkd_stat.release_partial++;

	    end_offset = LGK_OFFSET_FROM_PTR(&llb->llb_llbq);

	    for (llbq_offset = llb->llb_llbq.llbq_next;
	        llbq_offset != end_offset;
	        )
	    {
	        next_llbq = (LLBQ *)LGK_PTR_FROM_OFFSET(llbq_offset);

	        /*  Compute the address of the LKB. */

	        lkb = (LKB *)((char *)next_llbq - CL_OFFSETOF(LKB,lkb_llbq));

	        /*	Skip to the next LLBQ in case we unlock this LKB. */

	        llbq_offset = next_llbq->llbq_next;

	        /*	See if the partial key matches. */

	        rsb = (RSB *)LGK_PTR_FROM_OFFSET(lkb->lkb_lkbq.lkbq_rsb);

	        if (rsb->rsb_name.lk_type == lkreq->lock_key.lk_type &&
	            rsb->rsb_name.lk_key1 == lkreq->lock_key.lk_key1 &&
	            rsb->rsb_name.lk_key2 == lkreq->lock_key.lk_key2)
	        {
		    unlock_status = LK_do_unlock(lkb, (LK_VALUE *) 0, (i4)0);
	        }
	    }
	}
    }
    else if (lkreq->flags & LK_AL_PHYS)
    {
	/* Release all physical page locks on list */
	end_offset = LGK_OFFSET_FROM_PTR(&llb->llb_llbq);

	for (llbq_offset = llb->llb_llbq.llbq_next;
	    llbq_offset != end_offset;
	    )
	{
	    next_llbq = (LLBQ *)LGK_PTR_FROM_OFFSET(llbq_offset);
	    /*	Skip to the next LLBQ in case we unlock this LKB. */
	    llbq_offset = next_llbq->llbq_next;

	    /*  Compute the address of the LKB. */

	    lkb = (LKB *)((char *)next_llbq - CL_OFFSETOF(LKB,lkb_llbq));


	    /*	See if is the page lock and is physical attribute. */
	    if ( lkb->lkb_attribute & LKB_PHYSICAL )
	    {
		rsb = (RSB *)LGK_PTR_FROM_OFFSET(lkb->lkb_lkbq.lkbq_rsb);

		if ( rsb->rsb_name.lk_type == LK_PAGE )
		    unlock_status = LK_do_unlock(lkb, (LK_VALUE *) 0, (i4)0);
	    }
	}
    }

    if ( (lkreq->flags & LK_ALL) == 0 )
    {
	/* Not a release of entire list, we're done */
	llb->llb_status &= ~(LLB_ALLOC);
	(VOID) LK_unmutex(&llb->llb_mutex);

        /*
        ** (ICL keving)
        ** This used to return OK with a comment that it should be the
        ** status from lk_do_unlock. I have made it this status.
        */
        return (unlock_status);
    }

    /* The request is to deallocate the entire list: */

    lkd->lkd_stat.release_all++;

    /*
    ** The request is to deallocate the entire list.  If this is a shared
    ** list, then just deallocate the handle and decrement the reference
    ** count to the shared list.  If the reference count goes to zero then
    ** deallocate the shared list as well.
    **
    ** If this is a shared list then we have assigned proc_llb to point to
    ** the parent handle above.
    */
    if (proc_llb)
    {
	/* Deallocate the handle llb.  There can be no locks to free */

	/*
	** First, release the SHARED LLB's mutex to avoid deadlock.
	** We'll reacquire it, below, to decrement the share count
	** after removing the handle from the LLB queue and
	** freeing it.
	*/
	(VOID) LK_unmutex(&llb->llb_mutex);

	if ( (mutex_status = LK_mutex(SEM_EXCL, &proc_llb->llb_mutex)) ||
	     (mutex_status = LK_mutex(SEM_EXCL, &lkd->lkd_llb_q_mutex)) )
	{
	    TRdisplay("Panic: LK_mutex failure in LK_release_lock!\n");
	    return (mutex_status);
	}
	next_llb = (LLB *)LGK_PTR_FROM_OFFSET(proc_llb->llb_q_next);
	prev_llb = (LLB *)LGK_PTR_FROM_OFFSET(proc_llb->llb_q_prev);
	next_llb->llb_q_prev = proc_llb->llb_q_prev;
	prev_llb->llb_q_next = proc_llb->llb_q_next;
	(VOID) LK_unmutex(&lkd->lkd_llb_q_mutex);

#ifdef xEVENT_DEBUG
	if (LK_llb_on_list(proc_llb, lkd))
	{
	    (VOID) LK_unmutex(&proc_llb->llb_mutex);
	    ule_format(E_DMA016_LKEVENT_LIST_ERROR, (CL_ERR_DESC *)0, ULE_LOG,
			NULL, (char *)0, 0L, (i4 *)0, &err_code, 1,
			0, proc_llb->llb_id.id_id);
	    TRdisplay("ERROR: Lock list %d still on event list!\n",
		    proc_llb->llb_id.id_id);
	    SIGNAL_DEBUGGER;
	    return (LK_BADPARAM);
	}
#endif

	/*
	** The deallocate will also free the proc llb_mutex.
	*/
	LK_deallocate_cb(proc_llb->llb_type, (PTR)proc_llb, proc_llb);

	/*
	** If there are still connections to this shared list, then just
	** return, otherwise fall through to release the shared list.
	*/
	if ( mutex_status = LK_mutex(SEM_EXCL, &llb->llb_mutex) )
	    return(mutex_status);

	if (--llb->llb_connect_count > 0)
	{
	    llb->llb_status &= ~(LLB_ALLOC);
	    (VOID) LK_unmutex(&llb->llb_mutex);

	    return (OK);
	}
    }

    /*
    **	At this point it must be a release of all locks.
    **	Release all locks for this lock list the reverse order
    **  that they were requested.
    */


    end_offset = LGK_OFFSET_FROM_PTR(&llb->llb_llbq.llbq_next);
    while (llb->llb_llbq.llbq_next != end_offset)
    {
	/*
	**  Move backwards from the LLBQ in the LKB to the beginning of the
	**  LKB.
	*/

	last_llbq = (LLBQ *)LGK_PTR_FROM_OFFSET(llb->llb_llbq.llbq_prev);
	lkb = (LKB *)((char *)last_llbq - CL_OFFSETOF(LKB,lkb_llbq));

	/*  Unlock this LKB. */
        /*
        ** (ICL keving)
        ** If it fails return status. Old code would ignore it ...
        */
	unlock_status = LK_do_unlock(lkb, (LK_VALUE *) 0, (i4)0);
        if (unlock_status)
	{
	    llb->llb_status &= ~(LLB_ALLOC);
	    (VOID)LK_unmutex(&llb->llb_mutex);
            return(unlock_status);
	}
    }

    /*
    ** If list is waiting on an event, then turn off the event wait status.
    */
    if ((status = LK_cancel_event_wait(llb, lkd, FALSE, &sys_err)))
    {
	llb->llb_status &= ~(LLB_ALLOC);
	(VOID) LK_unmutex(&llb->llb_mutex);
	return (status);
    }

    if (mutex_status = LK_mutex(SEM_EXCL, &lkd->lkd_llb_q_mutex))
    {
	TRdisplay("Panic: LK_mutex failure in LK_release_lock!\n");
	return (mutex_status);
    }
    next_llb = (LLB *)LGK_PTR_FROM_OFFSET(llb->llb_q_next);
    prev_llb = (LLB *)LGK_PTR_FROM_OFFSET(llb->llb_q_prev);
    next_llb->llb_q_prev = llb->llb_q_prev;
    prev_llb->llb_q_next = llb->llb_q_next;
    (VOID) LK_unmutex(&lkd->lkd_llb_q_mutex);

#ifdef xEVENT_DEBUG
    if (LK_llb_on_list(llb, lkd))
    {
        (VOID) LK_unmutex(&llb->llb_mutex);
	ule_format(E_DMA016_LKEVENT_LIST_ERROR, (CL_ERR_DESC *)0, ULE_LOG,
		    NULL, (char *)0, 0L, (i4 *)0, &err_code, 1,
		    0, llb->llb_id.id_id);
	TRdisplay("ERROR: Lock list %d still on event list!\n",
		llb->llb_id.id_id);
	SIGNAL_DEBUGGER;
	return (LK_BADPARAM);
    }
#endif

    /*
    ** The deallocate also frees the llb_mutex.
    */
    LK_deallocate_cb(llb->llb_type, (PTR)llb, llb);

    /* Handle the related list(s), if any */
    while ( related_llb )
    {
	llb = (LLB *)LGK_PTR_FROM_OFFSET(related_llb);
	/*
	** If the reference count goes to zero and the recovery flag
	** is set, release the locks on the related LLB and deallocate
	** it.
	*/
	if ( mutex_status = LK_mutex(SEM_EXCL, &llb->llb_mutex) )
	    return(mutex_status);
	llb->llb_status |= LLB_ALLOC;

	/* Save the forward link, clear in this LLB */
	related_llb = llb->llb_related_llb;
	llb->llb_related_llb = 0;

	if (--llb->llb_related_count == 0 && (lkreq->flags & LK_RELATED))
	{
	    end_offset = LGK_OFFSET_FROM_PTR(&llb->llb_llbq.llbq_next);
	    while (llb->llb_llbq.llbq_next != end_offset)
	    {
		/*
		**  Move backwards from the LLBQ in the LKB to the beginning of the
		**  LKB.
		*/

		last_llbq = (LLBQ *)
			LGK_PTR_FROM_OFFSET(llb->llb_llbq.llbq_prev);
		lkb = (LKB *)((char *)last_llbq - CL_OFFSETOF(LKB,lkb_llbq));

		/*  Unlock this LKB. */
                /*
                ** (ICL keving)
                ** If it fails return status. Old code would ignore it ...
                */
                unlock_status = LK_do_unlock(lkb, (LK_VALUE *) 0, (i4)0);
                if (unlock_status)
		{
		    llb->llb_status &= ~(LLB_ALLOC);
		    (VOID)LK_unmutex(&llb->llb_mutex);
                    return (unlock_status);
		}
	    }

	    /* Deallocate the related LLB. */
	    if ( mutex_status = LK_mutex(SEM_EXCL, &lkd->lkd_llb_q_mutex) )
		return(mutex_status);
	    next_llb = (LLB *)LGK_PTR_FROM_OFFSET(llb->llb_q_next);
	    prev_llb = (LLB *)LGK_PTR_FROM_OFFSET(llb->llb_q_prev);
	    next_llb->llb_q_prev = llb->llb_q_prev;
	    prev_llb->llb_q_next = llb->llb_q_next;
	    (VOID) LK_unmutex(&lkd->lkd_llb_q_mutex);

#ifdef xEVENT_DEBUG
	    if (LK_llb_on_list(llb, lkd))
	    {
		(VOID)LK_unmutex(&llb->llb_mutex);
		ule_format(E_DMA016_LKEVENT_LIST_ERROR,
			    (CL_ERR_DESC *)0, ULE_LOG,
			    NULL, (char *)0, 0L, (i4 *)0, &err_code, 1,
			    0, llb->llb_id.id_id);
		TRdisplay("ERROR: Lock list %d still on event list!\n",
			llb->llb_id.id_id);
		SIGNAL_DEBUGGER;
		return (LK_BADPARAM);
	    }
#endif

	    /*
	    ** The deallocate will also free the related_llb's llb_mutex
	    */
	    LK_deallocate_cb(llb->llb_type, (PTR)llb, llb);
	}
	else
	{
	    llb->llb_status &= ~(LLB_ALLOC);
	    (VOID) LK_unmutex(&llb->llb_mutex);
	}
    }

    return (OK);
}

/*{
** Name: LK_cancel	- Cancel lock request.
**
** Description:
**      This routine is called to cancel a lock request that is waiting
**	and needs to be aborted, typically for timeout or interrupt.
**	This is currently called by the implementation of waiting that
**	occurs in the context of the user in the LK routines.  This is
**	not an external call at this point.
**
** Inputs:
**      flags                           The flags from LKrequest().
**	lock_list_id			The lock list to cancel.
**
** Outputs:
**	err_code			system specific error.
**	Returns:
**	    OK
**	    LK_CANCEL
**	    LK_BADPARAM
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	Summer, 1992 (bryanp)
**	    Working on the new portable logging and locking system.
**	20-oct-1992 (bryanp)
**	    When cancelling a lock event wait, remove LLB from LKD EW queue.
**	24-may-1993 (bryanp)
**	    Mutex optimizations: Release the LK_mutex before sending the message
**		to the CSP. Return special E_DMA017_LK_MUTEX_RELEASED so that
**		our caller knows we've released the mutex for him.
**	21-jun-1993 (bryanp)
**	    Build a completion handle so we can get the result status.
**	    Don't need to send a request to the CSP unless the lock list is
**		waiting.
**	17-nov-1994 (medji01)
**	    Mutex Granularity Project
**		- Changed LK_xmutex() calls to pass mutex field address.
**		- There is no longer a need to release and reacquire the LKD
**		  mutex before and after calling LK_csp_send().
**	28-mar-1995 (medji01)
**	    64K+ Lock Limit Project
**		- Removed references to id_instance in LK_ID.
**		- Changed error messages to reference id_id instead
**		  of id_instance.
**	11-Dec-1995 (jenjo02)
**	    Defined LKevent-specific mutex to protect event wait queue
**	    (lkd_ew_q_mutex) instead of blocking much more by relying
**	    on lkd_mutex.
**	21-aug-1996 (boama01)
**	    Mutex Granularity Project: medji01 removed the defunct free/lock
**	    of LKD around LK_csp_send(), but did not eliminate returning the
**	    E_DMA017_LK_MUTEX_RELEASED status.  This caused caller havoc in
**	    a Cluster.  Now returning status from LK_csp_send (OK or FAIL)
**	    (also removed leftover comments).
**	04-Oct-1996 (jenjo02)
**	    Added *lkb to LK_COMP_HANDLE.
**	06-Aug-1997 (jenjo02)
**	    Moved handling of deadlocked LKB here from lkrqst. LKdeadlock_thread()
**	    has called here with flags == LK_DEADLOCKED to indicate what's going on.
**      05-Nov-1998 (hanal04)
**          Added sid and pid to completion handle to protect from possible
**          duplicate lkd_rqst_id. b90388. Crossed from 1.2/01
**	19-Mar-2007 (jonj)
**	    Add IsRundown parm to LK_cancel_event_wait()
*/
/* ARGSUSED */
STATUS
LK_cancel(
i4		    flags,
LK_LLID		    lock_list_id,
SIZE_TYPE	    *lkb_to_cancel,
LK_COMP_HANDLE	    *completion_handle,
CL_ERR_DESC	    *sys_err)
{
    LKD			*lkd = (LKD *)LGK_base.lgk_lkd_ptr;
    LLB                 *llb;
    LKB                 *lkb;
    LLB_ID		*input_lock_list_id = (LLB_ID *) &lock_list_id;
    STATUS		status;
    i4		err_code;
    STATUS		local_status;
    CL_ERR_DESC		local_sys_err;
    SIZE_TYPE		*lbk_table;
    LKREQ		lkreq;

    LK_WHERE("LK_cancel")

    /* Validate the lock list id. */

    /*
    ** If input lock list is shared, it must be the
    ** referencing LLB (LLB_PARENT_SHARED). 
    ** The referenced (LLB_SHARED) list is never suspended, hence should 
    ** never be the object of a LK_cancel():
    */

    if (input_lock_list_id->id_id == 0 || 
	(i4) input_lock_list_id->id_id > lkd->lkd_lbk_count)
    {
	ule_format(E_CL1028_LK_CANCEL_BADPARAM, (CL_ERR_DESC *)0, ULE_LOG,
			NULL, (char *)0, 0L, (i4 *)0, &err_code, 2,
			0, input_lock_list_id->id_id, 0, lkd->lkd_lbk_count);
	return (LK_BADPARAM);
    }

    lbk_table = (SIZE_TYPE *)LGK_PTR_FROM_OFFSET(lkd->lkd_lbk_table);
    llb = (LLB *)LGK_PTR_FROM_OFFSET(lbk_table[input_lock_list_id->id_id]);

    if (llb->llb_type != LLB_TYPE ||
	llb->llb_status & LLB_SHARED ||
	llb->llb_id.id_instance != input_lock_list_id->id_instance) 
    {
	ule_format(E_CL1029_LK_CANCEL_BADPARAM, (CL_ERR_DESC *)0, ULE_LOG,
			NULL, (char *)0, 0L, (i4 *)0, &err_code, 3,
			0, *(u_i4*)&input_lock_list_id, 0, llb->llb_type, 
			0, *(u_i4*)&llb->llb_id);
	return (LK_BADPARAM);
    }

    if ((local_status = LK_mutex(SEM_EXCL, &llb->llb_mutex)))
    {
	TRdisplay("Panic: LK_mutex failure in LK_cancel!\n");
	return (local_status);
    }

    /* If cancelling a lock, set up the LKB */
    if (lkb_to_cancel && *lkb_to_cancel)
	lkb = (LKB *)LGK_PTR_FROM_OFFSET(*lkb_to_cancel);
    else
	lkb = (LKB *)NULL;

    /*
    ** If waiting for completion of an LKevent call, then cancel that.
    */
    if (llb->llb_status & LLB_EWAIT)
    {
	status = LK_cancel_event_wait(llb, lkd, FALSE, sys_err);
	if (status)
	{
	    (VOID) LK_unmutex(&llb->llb_mutex);
	    return (status);
	}
    }

    /*
    ** If the lock has already been granted (LKB is no longer waiting)
    ** return LK_CANCEL.
    */
    if (lkb == (LKB *)NULL || lkb->lkb_state == LKB_GRANTED)
    {
	if (lkb)
	    lkb->lkb_attribute &= ~LKB_DEADLOCKED;
	(VOID) LK_unmutex(&llb->llb_mutex);
	return (LK_CANCEL);
    }

    lkd->lkd_stat.cancel++;
    LK_do_cancel(lkb_to_cancel);

    (VOID) LK_unmutex(&llb->llb_mutex);
    return (OK);
}

/*{
** Name: LK_do_cancel	- Cancel waiting lock request.
**
** Description:
**      This routine cancels any request currently waiting for the lock list.
**
** Inputs:
**      llb                             Pointer to lock list.
**
** Outputs:
**	Returns:
**	    LK_CANCEL			lock list was not waiting
**	    OK				Non-cluster system
**	    VMS status value		On a VMSCluster system, the return
**					status from SYS$DEQ is returned.
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	Summer, 1992 (bryanp)
**	    Working on the new portable logging and locking system.
**	05-mar-1993 (andys)
**	    Return status.
**	28-nov-1994 (medji01)
**	    Mutex Granularity Project
**		- Acquire RSB mutex while moving LKB from conversion to
**		  grant queue.
**	15-Dec-1995 (jenjo02)
**	    Redoing mutexes again.
**	    Caller must have locked llb_mutex prior to call;
**	    it remains locked on return from this function.
**	21-aug-1996 (boama01)
**	    Chgd *RSB parm to **RSB in LK_try_convert() call, for passage to
**	    LK_try_wait(); test if it's null (RSB was deallocated) before
**	    unmutexing the RSB.
**      01-aug-1996 (nanpr01 for ICL keving)
**          LK_try_convert now returns a status.
**	15-Nov-1996 (jenjo02)
**	    Replaced rsb_dlock algorithm with lkd_dlock_mutex block of 
**	    wait-for matrix.
**	18-Nov-1996 (jenjo02)
**	    When undoing a conversion, restore lkb_request_mode (the mode that
**	    failed), to the previously granted mode (lkb_grant_mode).
**	21-Jan-1997 (jenjo02)
**	    If LLB is on the deadlock wait queue, remove it.
**	05-Aug-1997 (jenjo02)
**	  o Also clear llb_wait LKB when resetting LLB_WAITING status.
**	  o If LLB is deadlocked and the cancel is successful, awaken
**	    the deadlocked thread.
**	22-Jun-1998 (jenjo02)
**	    Immediately lock the dlock mutex to prevent resumption of this
**	    thread while we're attempting to cancel a deadlocked LLB.
**	07-Jul-1998 (jenjo02)
**	    Remove spare unmutex of dlock_mutex in LK_do_cancel().
**	14-Jan-1999 (jenjo02)
**	    Recheck LLB_ON_DLOCK_LIST state after acquiring mutex.
**	13-Mar-2006 (jenjo02)
**	    Take and hold the RSB mutex early on to single-thread
**	    simultaneous lock cancels on the same resource.
**	21-Mar-2007 (jonj)
**	    Release MULTITHREAD llb_mutex before calling CXdlm_cancel
**	    to unblock other threads while we perhaps CSsuspend.
**	19-Oct-2007 (jonj)
**	    Guard against null RSB, LLB, remove from stall queue if 
**	    it's on it. During LK_rundown, pretty much anything 
**	    can happen.
**	14-Nov-2007 (jonj)
**	    Don't assume that it's ok to ignore LK_mutex() failures;
**	    if the mutex owner is dead, we can't make assumptions
**	    about the mutexed structure.
**	    LKB_STALLED attribute replaces lkreq.result_flags.
*/
STATUS
LK_do_cancel(SIZE_TYPE *lkb_to_cancel)
{
    LKD		    *lkd = (LKD *)LGK_base.lgk_lkd_ptr;
    LLB		    *llb;
    LKB		    *lkb;
    LKB		    *next_lkb;
    LKB		    *prev_lkb;
    LLBQ	    *next_llbq;
    LLBQ	    *prev_llbq;
    RSB		    *rsb;
    STATUS	    status = OK;
    STATUS	    local_status;
    CL_ERR_DESC     sys_err;
    bool	    rsbIsMutexed = FALSE;

    LK_WHERE("LK_do_cancel")

    /*
    ** We were called with the LLB's mutex held.
    */

    lkb = (LKB *)LGK_PTR_FROM_OFFSET(*lkb_to_cancel);

    llb = (LLB *)LGK_PTR_FROM_OFFSET(lkb->lkb_lkbq.lkbq_llb);

    rsb = (RSB *)LGK_PTR_FROM_OFFSET(lkb->lkb_lkbq.lkbq_rsb);

    /* Lock the (non-NULL) RSB, watch for dead mutex holder (LK_rundown) */
    if ( rsb )
    {
	if ( local_status = LK_mutex(SEM_EXCL, &rsb->rsb_mutex) )
	    return(local_status);
	rsbIsMutexed = TRUE;
    }

    /* Remove LKB from deadlock and wait queues. */
    if (lkb->lkb_attribute & (LKB_ON_DLOCK_LIST | LKB_ON_LLB_WAIT))
    {
	/* lkd_dw_q_mutex protects both queues */
	if (local_status = LK_mutex(SEM_EXCL, &lkd->lkd_dw_q_mutex))
	    return(local_status);
	if (lkb->lkb_attribute & LKB_ON_DLOCK_LIST)
	{
	    next_llbq = (LLBQ *)LGK_PTR_FROM_OFFSET(lkb->lkb_dead_wait.llbq_next);
	    prev_llbq = (LLBQ *)LGK_PTR_FROM_OFFSET(lkb->lkb_dead_wait.llbq_prev);
	    next_llbq->llbq_prev = lkb->lkb_dead_wait.llbq_prev;
	    prev_llbq->llbq_next = lkb->lkb_dead_wait.llbq_next;
	    lkb->lkb_attribute &= ~(LKB_ON_DLOCK_LIST);
	    lkd->lkd_dw_q_count--;
	}

	/* Remove LKB from LLB's wait queue */
	if (lkb->lkb_attribute & LKB_ON_LLB_WAIT)
	{
	    next_llbq = (LLBQ *)LGK_PTR_FROM_OFFSET(lkb->lkb_llb_wait.llbq_next);
	    prev_llbq = (LLBQ *)LGK_PTR_FROM_OFFSET(lkb->lkb_llb_wait.llbq_prev);
	    next_llbq->llbq_prev = lkb->lkb_llb_wait.llbq_prev;
	    prev_llbq->llbq_next = lkb->lkb_llb_wait.llbq_next;
	    lkb->lkb_attribute &= ~LKB_ON_LLB_WAIT;
	    if ( llb )
		llb->llb_waiters--;
	}
	(VOID)LK_unmutex(&lkd->lkd_dw_q_mutex);
    }

    /* If lock request is stalled, remove it from stall queue */
    if ( lkb->lkb_attribute & LKB_STALLED )
    {
	/* Lock the stall queue */
	if ( local_status = LK_mutex(SEM_EXCL, &lkd->lkd_stall_q_mutex) )
	    return(local_status);

	/* Check again after waiting for mutex */
	if ( lkb->lkb_attribute & LKB_STALLED )
	{
	    LLBQ	*llbq = &lkb->lkb_lkreq.lkreq_stall_wait;
	    LLBQ	*next_llbq, *prev_llbq;
	    
	    next_llbq = (LLBQ*)LGK_PTR_FROM_OFFSET(llbq->llbq_next);
	    prev_llbq = (LLBQ*)LGK_PTR_FROM_OFFSET(llbq->llbq_prev);

	    next_llbq->llbq_prev = llbq->llbq_prev;
	    prev_llbq->llbq_next = llbq->llbq_next;

	    /* Show no longer on stall queue */
	    lkb->lkb_attribute &= ~LKB_STALLED;
	}

	/* Release the stall queue */
	(VOID)LK_unmutex(&lkd->lkd_stall_q_mutex);
    }

    /* If the LKB is no longer waiting, return LK_CANCEL */
    if (lkb->lkb_state == LKB_GRANTED)
    {
	lkb->lkb_attribute &= ~LKB_DEADLOCKED;
	if ( rsbIsMutexed )
	    (VOID) LK_unmutex(&rsb->rsb_mutex);
	return(LK_CANCEL);
    }

    /* If no RSB or no LLB, do nothing further */
    if ( !rsb || !llb )
    {
	if ( rsbIsMutexed )
	    (VOID) LK_unmutex(&rsb->rsb_mutex);
	return(OK);
    }

#ifdef CSP_DEBUG
    if (csp_debug)
	TRdisplay("%@ LK_do_cancel: llb->llb_status %x state %x\n", 
	    llb->llb_status, lkb->lkb_state);
#endif

    /* LKB is either waiting for a new lock or a lock conversion */
    if (lkb->lkb_state == LKB_WAITING)
    {
	llb->llb_status &= ~(LLB_ENQWAITING | LLB_GDLK_SRCH);
	
	/* LKB will be freed, clear client's offset */
	*lkb_to_cancel = 0;
	
	/*  Unlock this request, freeing the LKB. */
	/* Tell LK_do_unlock we hold the mutex already */
	lkb->lkb_attribute |= LKB_RSB_IS_MUTEXED;
	return(LK_do_unlock(lkb, (LK_VALUE *) 0, LK_DO_UNLOCK_CANCEL));
    }
    else if (lkb->lkb_state == LKB_CONVERT)
    {
	/*
	**  Undo the conversion.  Take the LKB from the convert queue
	**  and put it back on the granted queue.  Call try_convert()
	**  to handle scheduling converter or waiters.
	*/

	/*
	** In a Cluster system, we must not only undo the conversion
	** locally, but we may also need to dequeue the lock request from the
	** distributed lock manager.
	*/
	if (lkb->lkb_attribute & LKB_DISTRIBUTED_LOCK)
	{
	    if (lkb->lkb_cx_req.cx_lki_flags & CX_LKI_F_QUEUED)
	    {
		lkd->lkd_stat.deq++;

		if ((llb->llb_status & LLB_GDLK_SRCH) == 0)
		    lkd->lkd_stat.gdlck_search--;

		/*	Issue the DEQ lock request. */

		if ( llb->llb_status & LLB_MULTITHREAD )
		    (VOID)LK_unmutex(&llb->llb_mutex);

		status = CXdlm_cancel( CX_F_PROXY_OK, &lkb->lkb_cx_req );

		/* Reacquire LLB mutex */
		if ( llb->llb_status & LLB_MULTITHREAD &&
		     (local_status = LK_mutex(SEM_EXCL, &llb->llb_mutex)) )
		{
		    return(local_status);
		}
#ifdef CSP_DEBUG
		if (csp_debug)
		{
		    LK_UNIQUE	lock_id;

		    CX_EXTRACT_LOCK_ID( &lock_id, &lkb->lkb_cx_req.cx_lock_id );
		    TRdisplay(
		     "%@ CSP: DLM Lock conversion request %x.%x cancelled\n;",
		     lock_id.lk_uhigh, lock_id.lk_ulow);
		}
#endif
		if ( E_CL2C25_CX_E_DLM_NOTHELD == status )
		{
		    /* Lock ID no longer valid, but we don't care. */
		    status = OK;
		}

		lkb->lkb_attribute |= LKB_CONCANCEL;
		lkb->lkb_attribute &= ~LKB_ENQWAIT;

		/* If the conversion request has already been granted. 
		** Do nothing. Go ahead to remove the LKB from the RSB 
		** convert queue to the head of the RSB grant queue.
		*/
	    }
	}

	/*  Remove the LKB from the RSB convert queue. */

	next_lkb = (LKB *)LGK_PTR_FROM_OFFSET(lkb->lkb_q_next);
	prev_lkb = (LKB *)LGK_PTR_FROM_OFFSET(lkb->lkb_q_prev);
	next_lkb->lkb_q_prev = lkb->lkb_q_prev;
	prev_lkb->lkb_q_next = lkb->lkb_q_next;

	/*  Insert LKB onto the head of the RSB grant queue. */

	lkb->lkb_q_next = rsb->rsb_grant_q_next;
	lkb->lkb_q_prev = LGK_OFFSET_FROM_PTR(&rsb->rsb_grant_q_next);
	next_lkb = (LKB *)LGK_PTR_FROM_OFFSET(rsb->rsb_grant_q_next);
	next_lkb->lkb_q_prev =
	    rsb->rsb_grant_q_next = LGK_OFFSET_FROM_PTR(lkb);

	llb->llb_status &= ~(LLB_ENQWAITING | LLB_GDLK_SRCH);

	lkb->lkb_state = LKB_GRANTED;

	lkb->lkb_attribute &= ~LKB_DEADLOCKED;

	/* Restore the failed requested mode to the granted mode */
	lkb->lkb_request_mode = lkb->lkb_grant_mode;

	if ( lkb->lkb_cback.cback_fcn != (LKcallback)NULL )
            rsb->rsb_cback_count++;

	/*
	** Count one less coverter. If there are still converters or
	** waiters, try granting some locks.
	*/
	if ( --rsb->rsb_converters || rsb->rsb_waiters )
	    status = LK_try_convert(&rsb, rsb->rsb_grant_mode);
	else
	    rsb->rsb_convert_mode = rsb->rsb_grant_mode;

        /*  Unlock the RSB */
	/* (boama01, 21-aug-1996) Make sure it wasn't deallocated first */
	if (rsb)
	    (VOID) LK_unmutex(&rsb->rsb_mutex);
    }
    else
    {
	lkb->lkb_attribute &= ~LKB_ENQWAIT;
	(VOID) LK_unmutex(&rsb->rsb_mutex);
    }

    return(status);
}

/*{
** Name: LK_do_unlock	- Unlock a specific request.
**
** Description:
**      Verify that the lock has no child locks. Then dequeue the lock,
**      and figure out if anybody else waiting on the convert 
**      queue or on the wait queue can run.
**
** Inputs:
**      lkb                             The lock to unlock.
**	value				New lock value if we're unlocking a
**					lock which was held >= SIX.
**	flag				Lock cancellation flags (VMS Cluster)
**
** Outputs:
**	Returns:
**	    In a non-cluster locking system, this routine cannot fail, and
**	    always returns OK.
**
**	    In a VMSCluster system, this routine returns a status code which
**	    indicates the success or failure of the
**	    SYS$DEQ call which was made to dequeue the lock request.
**	    This routine will also soon need to be enhanced to take a sys_err
**	    variable that it can fill in upon error to record the precise
**	    VMS error which occurred.
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	Summer, 1992 (bryanp)
**	    Working on the new portable logging and locking system.
**	16-feb-1993 (bryanp)
**	    Changed from returning VMS status to returning Ingres status. For
**	    now, precise VMS status is lost; in the future we need to change
**	    this routine to take a sys_err parameter which it can fill in with
**	    the precise VMS status in case of error.
**	12-oct-1993 (tad)
**	    Bug #56449
**	    Changed %x to %p for pointer values.
**	14-dec-1994 (medji01)
**	    Mutex Granularity Project
**		- Acquire LKD, LLB, LKH, and RSB semaphores when removing 
**		  an LKB.
**	15-Dec-1995 (jenjo02)
**	    Mutex redux. Caller must hold llb_mutex. Function returns
**	    with it still held.
**	15-jan-1996 (duursma) bug #71335
**	    Fixed bug 71335 by ensuring that the proper status code is returned.
**	21-aug-1996 (boama01)
**	    Chgd *RSB parm to **RSB in LK_try_wait() and LK_try_convert()
**	    calls; test if it has been nulled (RSB was deallocated) before
**	    unmutexing the RSB.
**      01-aug-1996 (nanpr01 for ICL keving)
**          LK Blocking Callbacks Project. If removing an LKB with a callback
**          then decrement the rsb's rsb_cback_count
**          LK_try_wait/convert routines now return a status. Move code
**          around so that if these routines fail status will be returned.
**	15-Nov-1996 (jenjo02)
**	    Replaced rsb_dlock algorithm with lkd_dlock_mutex block of 
**	    wait-for matrix.
**	26-mar-1997 (nanpr01)
**	    Misc fixes for VMS-Alpha porting.
**	02-Aug-1999 (jenjo02)
**	    Need to lock lkd_dlock_lock only if there are resource
**	    waiters or converters.
**	21-nov-2006 (abbjo03)
**	    Return OK instead of LK_NOTHELD.
**	21-Mar-2007 (jonj)
**	    Release MULTITHREAD llb_mutex before calling CXdlm_release
**	    to unblock other threads while we perhaps CSsuspend.
**	14-Nov-2007 (jonj)
**	    Don't assume that it's ok to ignore LK_mutex() failures;
**	    if the mutex owner is dead, we can't make assumptions
**	    about the mutexed structure.
*/
i4
LK_do_unlock(
LKB                 *lkb,
LK_VALUE	    *value,
i4		    flag)
{
    LKD		    *lkd = (LKD *)LGK_base.lgk_lkd_ptr;
    RSB		    *rsb;
    RSB		    *next_rsb;
    RSB		    *prev_rsb;
    LKB	            *prev_lkb;
    LKB		    *next_lkb;
    LKB		    *grant_lkb;
    LLBQ	    *next_llbq;
    LLBQ	    *prev_llbq;
    LKBQ	    *next_lkbq;
    LKBQ	    *prev_lkbq;
    SIZE_TYPE	    lkb_offset;
    SIZE_TYPE	    end_offset;
    i4	    status = E_DB_OK;
    LLB		    *llb;
    i4		    save_state = lkb->lkb_state;
    i4		    save_grant_mode = lkb->lkb_grant_mode;
    i4		    new_group_mode;
    i4		    not_yet_granted = 0;
    CL_ERR_DESC	    sys_err;
    i4	    err_code;
    RSH		    *rsb_hash_bucket;
    LKH		    *lkb_hash_bucket;
    STATUS	    local_status = 0;

    LK_WHERE("LK_do_unlock")

    rsb = (RSB *)LGK_PTR_FROM_OFFSET(lkb->lkb_lkbq.lkbq_rsb);
    llb = (LLB *)LGK_PTR_FROM_OFFSET(lkb->lkb_lkbq.lkbq_llb);
    
    if ( (lkb->lkb_attribute & LKB_RSB_IS_MUTEXED) == 0 &&
	(local_status = LK_mutex(SEM_EXCL, &rsb->rsb_mutex)) )
    {
	return (local_status);
    }
    /* We no longer care... */
    lkb->lkb_attribute &= ~LKB_RSB_IS_MUTEXED;
    

    /*
    ** Has this lock been granted yet? If it has not, then we don't invalidate
    ** the lock value block, even if we were requesting it at a high mode.
    */
    if (LK_DO_UNLOCK_CANCEL == flag)
	not_yet_granted = 1;

    if (lkb->lkb_state == LKB_WAITING)
    {
	not_yet_granted = 1;
    }

    /*
    **	Handle value blocks first.  If current mode is SIX or higher
    **  then the value is stored from the caller into the RSB.  If no
    **	value is passed, then the value is marked invalid, unless we
    **	are cancelling a not-yet-granted lock request.
    */

    if (lkb->lkb_grant_mode >= LK_SIX)
    {
	if (value)
	{
	    lkb->lkb_cx_req.cx_value[0] = rsb->rsb_value[0] = value->lk_high;
	    lkb->lkb_cx_req.cx_value[1] = rsb->rsb_value[1] = value->lk_low;
	    lkb->lkb_cx_req.cx_value[2] = rsb->rsb_invalid = 0;
	}
	else if (not_yet_granted == 0)
	{
	    /*
	    ** The resource is marked as "value not valid", but it retains
	    ** whatever old value it had. Until the next time that its value
	    ** is set, calls that request the value will be given the old value
	    ** AND the special return code LK_VAL_NOTVALID.
	    **
	    ** IMPORTANT NOTE: We maintain a distinction between values
	    ** marked as invalid by Ingres, and value blocks marked
	    ** invalid by the DLM.   "Valid" status for Ingres, is simply
	    ** another piece of data maintained in the value block.
	    ** True DLM "invalid" status indicates a lock holder has
	    ** abended while holding a DLM lock in update mode,  and
	    ** some level of recovery will be required.  We will never
	    ** intentionally invalidate a value block during normal 
	    ** operation of Ingres.
	    */
	    lkb->lkb_cx_req.cx_value[2] = rsb->rsb_invalid = 1;
	}
    }

    /*
    ** If this is a Cluster locking system installation, then in addition
    ** to locally unlocking the lock in our own data structures, we must also
    ** notify the DLM to unlock the lock.   CX DLM takes care of any
    ** complexities introduced by dequeuing an ungranted request, so 
    ** double release previously required by VMS is no longer required.
    */
    if (lkb->lkb_attribute & LKB_DISTRIBUTED_LOCK)
    {
	i4	dlmflag = CX_F_PROXY_OK;

	if ((LK_DO_UNLOCK_RUNDOWN == flag) &&
	    (lkb->lkb_cx_req.cx_lki_flags & CX_LKI_F_PCONTEXT))
	{
	    /*
	    **  Releasing a lock whose DLM component has already
	    ** been implicitly released through the death of the
	    ** owning process.  Set release flag to only clear
	    ** CX_REQ structure, and not actually call DLM.
	    */
	    dlmflag = CX_F_CLEAR_ONLY;
	}

	if ((lkb->lkb_cx_req.cx_lki_flags & CX_LKI_F_QUEUED) ||
	    lkb->lkb_state == LKB_GRANTED)
	{
	    /*	Change various counters. */

	    lkd->lkd_stat.deq++;

	    /*	Issue the DEQ lock request. */
	    if ( llb->llb_status & LLB_MULTITHREAD )
		(VOID)LK_unmutex(&llb->llb_mutex);

	    status = CXdlm_release( dlmflag, &lkb->lkb_cx_req );

	    /* Reacquire LLB mutex */
	    if ( llb->llb_status & LLB_MULTITHREAD &&
		 (local_status = LK_mutex(SEM_EXCL, &llb->llb_mutex)) )
	    {
		return(local_status);
	    }

	    /*  Convert this code to something meaningful to LK */
	    if ( E_CL2C25_CX_E_DLM_NOTHELD == status )
	    {
		/* Lock ID no longer valid. */
		status = OK;
	    }
	}
#ifdef CSP_DEBUG
	else
	{
	    if (csp_debug)
		TRdisplay("%@ CSP: no Deq, lkb %p llb %p lkb_attribute %x lkb_state %x\n", 
		    lkb, llb, lkb->lkb_attribute, lkb->lkb_state); 
	}
#endif
    }

    /* Remove LKB from the resource queue. */

    prev_lkb = (LKB *)LGK_PTR_FROM_OFFSET(lkb->lkb_q_prev);
    next_lkb = (LKB *)LGK_PTR_FROM_OFFSET(lkb->lkb_q_next);
    next_lkb->lkb_q_prev = lkb->lkb_q_prev;
    prev_lkb->lkb_q_next = lkb->lkb_q_next;

    /* Adjust RSB waiters/converters */
    if ( lkb->lkb_state == LKB_WAITING )
	rsb->rsb_waiters--;
    else if ( lkb->lkb_state == LKB_CONVERT )
	rsb->rsb_converters--;

    /* Remove LKB from the LLB's LKB queue. */

    prev_llbq = (LLBQ *)LGK_PTR_FROM_OFFSET(lkb->lkb_llbq.llbq_prev);
    next_llbq = (LLBQ *)LGK_PTR_FROM_OFFSET(lkb->lkb_llbq.llbq_next);
    next_llbq->llbq_prev = lkb->lkb_llbq.llbq_prev;
    prev_llbq->llbq_next = lkb->lkb_llbq.llbq_next;

    /* Remove LKB from the LKB hash queue. */

    lkb_hash_bucket = (LKH *)LGK_PTR_FROM_OFFSET(lkb->lkb_lkbq.lkbq_lkh);
    if ((local_status = LK_mutex(SEM_EXCL, &lkb_hash_bucket->lkh_mutex)))
    {
	(VOID)LK_unmutex(&rsb->rsb_mutex);
	TRdisplay("Panic: LKH mutex failure in LK_do_unlock!\n"); 
        return (local_status);
    }
    if (lkb->lkb_lkbq.lkbq_next)
    {
	next_lkbq = (LKBQ *)LGK_PTR_FROM_OFFSET(lkb->lkb_lkbq.lkbq_next);
	next_lkbq->lkbq_prev = lkb->lkb_lkbq.lkbq_prev;
    }
    prev_lkbq = (LKBQ *)LGK_PTR_FROM_OFFSET(lkb->lkb_lkbq.lkbq_prev);
    prev_lkbq->lkbq_next = lkb->lkb_lkbq.lkbq_next;
    (VOID) LK_unmutex(&lkb_hash_bucket->lkh_mutex);

    /* Clear LKH stuff out of the LKB */
    lkb->lkb_lkbq.lkbq_lkh = lkb->lkb_lkbq.lkbq_llb = 
	lkb->lkb_lkbq.lkbq_rsb = 
	lkb->lkb_lkbq.lkbq_next = lkb->lkb_lkbq.lkbq_prev = 0;
    
    prev_lkb = (LKB *)LGK_PTR_FROM_OFFSET(lkb->lkb_q_prev);
    
    /*	Decrement count of locks held by this LLB. */

    llb->llb_lkb_count--;
    if ((lkb->lkb_attribute & LKB_PHYSICAL) == 0)
    {
    	llb->llb_llkb_count--;
    }

    if (save_state == LKB_GRANTED && (lkb->lkb_cback.cback_fcn != (LKcallback) NULL))
    {
        /* Removing a granted LKB with a callback */
        rsb->rsb_cback_count--;
    }

    /*
    ** LKB_CANCEL indicates that the freeing of the lock block must be
    ** deferred until the enq_complete() AST is delivered.
    */
    if ((lkb->lkb_attribute & LKB_CANCEL) == 0)
    {
	LK_deallocate_cb( lkb->lkb_type, (PTR)lkb, llb );
    }

    /* Unlocking depends on the current state of the lock. */

    switch (save_state)
    {
    case LKB_GRANTED:

	/* Check to see if the grant/conversion queue is empty. */

        if (rsb->rsb_grant_q_next ==
				LGK_OFFSET_FROM_PTR(&rsb->rsb_grant_q_next))
        {
	    /* Check to see if wait queue is also empty. */

	    if ( rsb->rsb_waiters )
		local_status = LK_try_wait(&rsb, LK_N);
	    else
            {
		/*
		** Take RSB off the RSH hash queue
		*/
		rsb_hash_bucket = (RSH *)LGK_PTR_FROM_OFFSET(rsb->rsb_rsh_offset);
		if (local_status = LK_mutex(SEM_EXCL, &rsb_hash_bucket->rsh_mutex))
		{
		    (VOID)LK_unmutex(&rsb->rsb_mutex);
		    return (local_status);
		}

		if (rsb->rsb_q_next)
		{
		    next_rsb = (RSB *)LGK_PTR_FROM_OFFSET(rsb->rsb_q_next);
		    next_rsb->rsb_q_prev = rsb->rsb_q_prev;
		}
		prev_rsb = (RSB *)LGK_PTR_FROM_OFFSET(rsb->rsb_q_prev);
		prev_rsb->rsb_q_next = rsb->rsb_q_next;
		(VOID)LK_unmutex(&rsb_hash_bucket->rsh_mutex);

		/*
		** The deallocate also unlocks the rsb_mutex.
		*/
		LK_deallocate_cb( rsb->rsb_type, (PTR)rsb, llb );
		rsb = (RSB *)NULL; /* avoid memory leaks! */
            }

            break;
        }

	/*
	**  Try conversions if no granted locks or the unlocked lock might have
	**  been blocking conversions.  Fall through to the code for current
	**  lock was on the conversion queue.
	*/

    case LKB_CONVERT:

	/*
	**  If this lock was on the head of the conversion queue or there are
	**  no granted locks on the queue or the grant mode of the lock that
	**  was unlocked might have been blocking conversions then recompute
	**  the new group mode and try granting some conversions.
	*/

	grant_lkb = (LKB *)LGK_PTR_FROM_OFFSET(rsb->rsb_grant_q_next);

        if ((save_state == LKB_CONVERT && prev_lkb->lkb_state == LKB_GRANTED) ||
	    (save_state == LKB_GRANTED && grant_lkb->lkb_state != LKB_GRANTED)||
	    save_grant_mode == rsb->rsb_convert_mode)
	{
	    new_group_mode = LK_N;
	    end_offset = LGK_OFFSET_FROM_PTR(&rsb->rsb_grant_q_next);

	    for (lkb_offset = rsb->rsb_grant_q_next;
		lkb_offset != end_offset;
		lkb_offset = next_lkb->lkb_q_next)
	    {
		next_lkb = (LKB *)LGK_PTR_FROM_OFFSET(lkb_offset);

		if (next_lkb->lkb_grant_mode > new_group_mode)
		    new_group_mode = next_lkb->lkb_grant_mode;
	    }
	    if ( rsb->rsb_converters || rsb->rsb_waiters )
		local_status = LK_try_convert(&rsb, new_group_mode);
	    else
		rsb->rsb_grant_mode = rsb->rsb_convert_mode = new_group_mode;
	}
        break;


    case LKB_WAITING:

	/*
	** If this lock was at the head of the wait queue and there are
	** no conversions, try granting some other waiters.
	*/

	if ( rsb->rsb_waiters && rsb->rsb_converters == 0 )
            local_status = LK_try_wait(&rsb, rsb->rsb_grant_mode);
	else
	    rsb->rsb_convert_mode = rsb->rsb_grant_mode;

        break;

    }

    if (rsb)
	(VOID) LK_unmutex(&rsb->rsb_mutex);

    if (local_status != E_DB_OK)
      status = local_status;

    return (status);
}
