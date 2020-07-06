/*
** Copyright (c) 1985, 2006 Ingres Corporation
**
**
NO_OPTIM=dr6_us5 i64_aix
*/

#include    <compat.h>
#include    <gl.h>

#include    <bt.h>
#include    <cs.h>
#include    <di.h>
#include    <cv.h>
#include    <lo.h>
#include    <me.h>
#include    <nm.h>
#include    <pc.h>
#include    <st.h>
#include    <tr.h>
#include    <tm.h>

#include    <iicommon.h>
#include    <dbdbms.h>

#include    <lg.h>
#include    <lk.h>
#include    <lkdef.h>

#include    <adf.h>
#include    <ulf.h>

#include    <dmf.h>
#include    <dmccb.h>
#include    <dmrcb.h>
#include    <dmtcb.h>
#include    <dmxcb.h>

#include    <dm.h>

#include    <dmftrace.h>
#include    <lgdstat.h>
#include    <dml.h>
#include    <dmp.h>
#include    <dmpp.h>
#include    <dmxe.h>
#include    <dm1b.h>
#include    <dm0l.h>
#include    <dm0m.h>
#include    <dm0p.h>
#include    <dm0llctx.h>
#include    <dm0pbmcb.h>
#include    <dm0s.h>
#include    <dm1c.h>
#include    <dm1r.h>
#include    <dm2f.h>
#include    <dm2t.h>
#include    <dmd.h>
#include    <scf.h>
#include    <sxf.h>
#include    <dma.h>
#include    <dm2d.h>
#include    <dm1r.h>
#include    <cx.h>
#ifdef VMS
#include <lib$routines.h>
#include <ssdef.h>
#endif

/* Given page size (which may be zero) how many Kilobytes */
#define KSIZE(pgsize) (pgsize > 0 ? pgsize / 1024 : 2)

/**
**
**  Name: DM0P.C - Buffer Pool Manager.
**
**  Description:
**      This module implements the buffer manager for DMF.  It manages the flow
**	of pages from disk to memory to a transaction back to memory and
**	potentially back to disk.  This management takes into account the
**	presence of multiple servers using the same resources without the
**	benefit of shared memory.  The multiple copy synchronization is handled
**	by keeping server locks on pages and maintaining version information
**	with these locks.
**
**	The cache lock protocol for coordinating multiple servers works as
**	follows:
**
**	    When a page is being read in a IX lock is held on the page.
**	    When a page is being written to a SIX lock is held on the page.
**	    When a page is not being read or written a IS lock is held on page.
**
**	    When a page is referenced and it is not owned by a transaction, the
**		lock is converted from IS to IX to get the new value stored with
**		the lock.  If the value is different then the value used to read
**		the page into the cache, then the page is reread from disk and
**		the new value is saved.  The lock mode is returned to IS.
**
**	    When a page is written the lock mode is converted to SIX, the page 
**		is written, the value is incremented, and the lock mode is 
**	        returned to IS and the new value is set.
**
**	    This protocol guarantees that:
**		all reads for a page wait for any write to complete;
**		a write waits for any reads to complete;
**		a value block check waits for any write to complete;
**		a write waits for a value block check to complete.
**
**	Cache locking is controlled by the characteristics of the database.  If
**	    the database was added to the server for exclusive use, cache locks
**	    are not needed, otherwise cache locking is enabled.  All system
**	    catalog pages for SCONCUR tables are always cache locked, because of
**	    the needed to recognize that the recovery process might have changed
**	    a page because system catalog pages are only physically locked.
**
**
** Pass Abort 
** ----------
** 
** Pass Abort is the procedure through which the DBMS server asks the RCP
** to perform a transaction abort.  It is used as a fault-tolerant mechanism
** to make another attempt at transaction recovery before failing.
** 
** Simple transaction abort processing is normally performed by the server.
** The RCP will normally only perform recovery on system or server failures.
** 
** When a server fails in its attempt to abort a transaction, however, it
** will issue a Pass Abort.  It signals the RCP to abort the transaction.
** 
** (Interesting note:  After the server has signalled the RCP, it simply
** returns to the user that the abort was successful.  Since the locks on
** the data are still held, nobody (including this user) may access the
** transient data until the transaction abort is completed.)
** 
** Should the RCP fail in its attempt to abort the transaction, the database 
** will be marked inconsistent, and all users of the database will be kicked
** out.
** 
** The Buffer manager will handle data in two modes : single-cache or
** multi-cache (used to be single-server/multi-server until shared caches were
** invented).
** 
** (Note: While single vs. multi server mode may only be configured at the
** Server Level, the buffer manager actually makes this decision on a
** database-by-database case.  Some databases may be served in single-cache
** mode while others are served in multi-cache mode.  The IIDBDB is currently
** always handled in multi-cache mode.  In the rest of this note I generally
** ignore the above and discuss the cache as though it always serves only one
** database.)
** 
** Cache Locking
** -------------
** 
** In multi-cache mode, the buffer manager uses cache-lock protocols to
** resolve issues of concurrent access of pages and cache invalidation.  With
** each page in the cache is a corresponding cache lock and a lock value.
** 
** Each time a page is referenced, the cache lock is re-requested to check
** its current lock value.  If the value is unchanged, then the page is assumed
** to be valid; otherwise the page is reread.
** 
** Whenever the cache updates a page, the page must be written to disk and
** its cache lock value updated.  This will serve to invalidate the page in
** other server caches.
** 
** Whenever the cache writes a page, it converts its cache lock to an exclusive
** mode to prevent other caches from concurrently reading the page.  The X
** lock is converted back down (and the lock value changed) as soon as the IO
** has completed.
** 
** 
** Fast Commit
** -----------
** 
** In single-cache mode, the buffer manager assumes that all pages in its
** cache may be referenced only through itself.  That is that no other servers
** using a different cache may open the same database and reference data.
** 
** Because of this assumption, the cache is not required to force out dirty
** pages when they are unlocked.  It may keep dirty pages cached since anyone
** requesting the pages will just use the cached version.
** 
** Single cache mode is required when using fast commit.
** 
** In single-cache mode the buffer manager does not (normally) need cache lock
** mechanisms which check/cause page invalidation and prevent concurrent IO's.
** 
** 
** RCP Interaction with Single Cache Mode
** ---------------------------------------
** 
** The Buffer Manager's single-cache handling becomes more complicated when
** Pass Aborts are factored in.
** 
** When a Pass Abort is signalled, the RCP will open the database in order
** to abort the affected transaction.  In doing so the RCP will violate the
** server's assumption that no other caches will access the same database.
** The RCP will be opening and accessing data in its cache at the same time
** the DBMS server is accessing the same database.
** 
** The bulk of the pass-abort cache issues are resolved through page locking.
** Since all pages which require abort work should be locked by the aborting
** transaction, we are assured that in most cases the RCP and server will
** not be acting on the same pages concurrently.  The Pass Abort
** protocol calls for the server to force and toss all pages belonging to the
** aborting transaction.  These pages are then protected by page locks and
** cannot be read back into the server cache until the transaction abort is
** complete.
** 
** The following are exceptions and complications to this scheme:
** 
**      Some pages may be updated by a transaction, but do not remain locked/
** 	owned by that transaction.  These pages may continue to be accessed by
** 	the server concurrent with the RCP's use.
** 
** 	Additionally, since these pages are not "owned" by the abort
** 	transaction (ownership is a buffer manager term - when a page is
** 	updated by a transaction, it is placed on that transaction's page
** 	queue - meaning it is owned by the transaction.  This makes it easy
** 	to find all pages which were touched by a particular xact), they
** 	are harder to track down to execute the force and toss semantics.
** 
** 	These pages currently include:
** 
** 	    Core System Catalog Pages
** 	    FMAP and FHDR Pages
** 	    Btree Index Pages
** 
**      When using readlock=nolock, pages may be faulted into the cache without
** 	acquiring page locks.  This allows us to read pages into the cache
** 	that may be being updated by the RCP.
** 
**      Group reads can cause pages to be faulted into the cache without
** 	acquiring pages locks in the same way that readlock=nolock does.
** 	This is because only the first page of the group is actually locked.
** 
** Code Design
** ------------
** 
** Pass abort code must be concerned with three processing intervals:
** 1) when pass abort is initially signalled; 
** 2) when the RCP is actually performing Pass Abort recovery; 
** 3) when the RCP completes Pass Abort. 
** 
** Prior to signalling the RCP to start Pass Abort processing, the
** server first invalidates all cache pages associated with the failed 
** transaction, then tosses all pages from its cache that it thinks 
** the RCP might need during Pass abort processing.  This is because 
** if the server is using fast commit, it is not verifying the 
** correctness of pages in its cache that may have been invalidated 
** by the RCP.  
** 
** Next, the server calls LG to let the RCP know that a Pass Abort 
** has occurred.  Then the server sets the buffer manager status 
** to BMCB_PASS_ABORT, entering the state where "PAWRITE" processing is 
** enabled.
** 
** Page types requiring special handling during Pass Abort are those 
** noted above:
** 
**     o Core System Catalog Pages
**     o FMAP and FHDR Pages
**     o Read-nolock pages
**     o Group read pages
**     o Btree Root and Index Pages (at least until recovery
**       issues are fixed)
** 
** Group reads are handled in gfault_page by simply disabling them 
** when PA processing is taking place.  That is, group reads default 
** to single page reads during this interval.
** 
** Readlock-nolock pages are managed by tossing them from the cache 
** immediately after reference.  Normal caching is reenabled  
** after completion of Pass Abort.  Also, readlock-nolock pages are
** cache locked when they are faulted in during Pass Abort to 
** ensure that they are not read at the same time they are being 
** updated elsewhere.
** 
** FHDR, FMAP, Btree root and index pages are flagged as "PAWRITE" 
** (in fault_page) whenever they are brought into the cache.  Pages 
** so flagged are then handled specially during Pass Abort, as follows.
** 
** During the "toss" performed at the start of Pass Abort, 
** PAWRITE pages that are fixed or busy are marked "TUNFIX", 
** causing them to be tossed from the cache when they unfixed.
** 
** Similarly, during Pass Abort fault_page code marks PAWRITE 
** pages TUNFIX when they are are faulted in.  dm0p_unfix_page 
** tosses them from the cache.  When Pass Abort is complete, 
** fault_page no longer marks PAWRITE pages TUNFIX.
** 
** Core System Catalog Pages are currently flagged as write on
** unfix (WUNFIX), making Pass Abort actions unnecessary at this 
** time.  At some point they may be treated as normal PAWRITE pages.
**
** Distributed Multi-Cache Management (DMCM) Protocol (ICL phil.p)
** ==================================================
** 
** DMCM is effectively Fast_Commit in a Multi-Cache environment :-
** 
** When a page is fixed, it is locked at the same mode as the data page
** (X or S), except read_nolock queries where the cache page is intially
** locked at the same mode as the data page and then converted down to N.
**
** (System catalog pages do NOT participate in the DMCM protocol, they are
** written/tossed on unfix for concurrency reasons.)
**
** At the end of a transaction, the pages are not flushed, but are 
** retained in cache with the a NULL lock.
** A cached page is now only tossed when absolutely necessary; as a
** result of another protocol such as pass-abort, a consistency point or
** reclaim.
**
** A cached page is now only ever written when one of the following 
** two conditions occur :-
**
** 1. Consistency point.
** 2. A session in another server requests a lock that contends with that
**    held on a cached page.
**
** (Obviously, pages are still (written and) tossed when it is necessary
** to do so to free up space in the case because a threshold has been
** reached.)
**
** When (2) occurs, the Blocking Callback routine dm0p_dmcm_callback
** which is passed as a parameter to each LKrequestWithCallback call,
** is fired; causing force_page to be called with the appropriate flag to
** ensure that the page is retained unfixed in our cache with a NULL lock.
**
** When a server attempts to fix a page, it first checks the value
** block to see if the page has been modified since we last faulted it in.
** If the value block has changed, we fault the page in. If the value
** block hasn't changed, we can use the copy already in our cache. If the
** lock mode indicated by the current transaction is higher than the mode
** at which the cached page is already locked, then we convert up to the
** required mode.
**
** READNOLOCK pages :-
**
** Leaving read_no_lock pages locked at LK_S, may cause cache locking
** deadlock because these pages will not have data page locks 
** protecting them. (The data page locks ordinarily guarantee that
** dealock is detected at the data page level before the cache page level.)
** Therefore having copied the read_no_lock page into one of the
** read_no_lock buffers, it is necessary to drop the lock to NULL.
**
** READAHEAD/Group Reads :-
**
** For reasons of concurrency, when performing a Group Read, the first page
** is locked at the same mode as the data page and all subsequent pages are
** locked at shared. Once all pages have been read, all but the first have
** their cache locked dropped to NULL - again for concurrency reasons.
**
**=============================================================================
**  
**	NOTE: should add explanations of some of the dm0p protocols - (rogerk)
**
**	Possible Future Improvements:
**
**	 o  Reduce number of calls to DIforce if possible.  This is enormously
**		expensive on many UNIX boxes.  Minimum would be once for each 
**		file for a transaction if running without fast commit.  
**		If fast commit it must be called every time unless DIforce 
**		calls are tracked against write calls in DI.
**	 o  Log more problem information before DMD_CHECK.
**       o  If running out of locks, might try converting tables from page 
**		locking to table locking for tables other then the current 
**		table.  If would require running down the XCB chain of active 
**		RCB's which is a logical level control block.  This still 
**		wouldn't solve the whole problem because it only deals with 
**		currently open tables.
**	 o  Add DI routine that informs the group read code of the largest read
**		starting at the current block that is most likely to be 
**		completed in a single I/O request.  Most useful for multi-volume
**		support so that we don't initiate multiple reads.
**
**          dm0p_allocate	- Allocate the buffer manager control block.
**	    dm0p_deallocate	- Deallocate the buffer manager control block.
**	    dm0p_close_page	- Handle page write so file can be closed.
**	    dm0p_fix_page	- Assign a page to a particular transaction.
**	    dm0p_cachefix_page	- Fix page into cache without xact context.
**	    dm0p_force_pages	- Force page owned by a transaction to disk.
**	    dm0p_tran_invalidate - Invalidate pages for a transaction from BM.
**	    dm0p_unfix_page	- Deassign a page from a transaction.
**	    dm0p_uncache_fix    - UnFix page from cache without xact context.
**	    dm0p_mutex		- Get a semaphore on a page.
**	    dm0p_unmutex	- Release a semaphore on a page.
**	    dm0p_lock_page	- Get Transaction lock on page.
**	    dm0p_unlock_page	- Release Transaction lock on page.
**	    dm0p_cp_flush	- Force dirty pages out of cache as part of CP.
**	    dm0p_wbflush_wait	- Wait for write behind flush signal
**	    dm0p_flush_pages	- Flush pages on modify queues for WB thread.
**          dm0p_mlock          - Lock Buffer Manager semaphore.
**          dm0p_munlock        - Unlock Buffer Manager semaphore.
**	    dm0p_bmid		- Return Buffer Manager Id.
**	    dm0p_cpdone		- Signal end of Consistency Point to BM.
**	    dm0p_count_connections - Validate count of servers connected to BM.
**	    dm0p_toss_pages	- Toss pages belonging to specific table or db.
**	    dm0p_1toss_page	- Toss a single page from the cache.
**	    dm0p_dbcache	- Register open/close db with BM.
**	    dm0p_tblcache	- Register open/modify table with BM.
**	    dm0p_escalate	- Escalate to table level locking.
**	    dm0p_wpass_abort	- Toss pages as part of PASS_ABORT protocol.
**          dm0p_dmcm_callback  - Blocking callback routine for DMCM.
**          dm0p_count_pages    - Return count of number of dirty pages in the
**                                cache for a specific database table.
**          dm0p_comp_xxx	- Externally visible completion wrappers.
**          dm0p_page_lock_mode - Figure out the page lock mode.
**
**	    STATIC ROUTINES
**
**	    fault_page		- Fault a page from disk into memory.
**	    gfault_page		- Fault a group of pages from disk into memory.
**	    force_page		- Force a page to disk.
**	    gforce_page		- Force a group of pages to disk.
**	    validate_page	- Validate a buffer page (check cache lock).
**	    invalidate_page	- Invalidate buffer - move to free list.
**	    move_group		- Requeue group if necessary after change to
**				  individual buffer of group.
**	    reclaim_bmc		- Get free buffer cache entry.
**	    dm0p_eset		- Set event mechanism for BM event.
**	    dm0p_ewait		- Wait for BM event.
**	    dm0p_erelease	- Signal BM event.
**	    dm0p_reclaim	- Reclaim BM cache locks (by freeing buffer).
**          dmcm_cache_flush    - Flush cache when stopping a server running
**                                DMCM.
**          complete_xxx	- Completion handlers.
**          dm0p_check_logfull  - Check to see if logfile is full.
**
**  History:
**      14-oct-1985 (derek)    
**          Created for Jupiter.
**	31-jun-1988 (rogerk)
**	    Added fast commit support.
**	    Added dm0p_cp_flush routine.
**	31-aug-1988 (rogerk)
**	    Return correct status if encounter warning in dm0p_force_pages.
**	 2-sep-1988 (rogerk)
**	    Make sure to force page in dm0p_unfix_page if mode is DM0P_WRITE.
**	30-sep-1988 (rogerk)
**	    Added write behind thread support.
**	    Added dm0p_wbflush_wait and dm0p_flush_pages routines.
**	    Added bm_wbstart, bm_wbend, bm_fcflush, bm_wbflush fields.
**	    Signal write behind threads when modify limit exceeded, wbstart
**	    limit exceeded, or when doing a consistency point.
**	31-oct-1988 (rogerk)
**	    Fixed concurrency bug in force_page caused by a buffer being
**	    force by the fast commit thread while being unfixed.  Changed
**	    force_page to determine what requeueing actions to take after
**	    write and lock requests are complete.
**	31-oct-1988 (rogerk)
**	    When put modified page on the modify priority queue, put the
**	    buffer at the end of the queue instead of the beginning.  This
**	    should cause us to do less log forces as we will generally not
**	    bump pages out of the cache immediately after modifying them.
**	10-feb-1989 (rogerk)
**	    Integrated 6.1 bug fixes - don't bump cache locks on group buffers
**	    that are not modified.  Add numargs parm to ule_format calls.
**	28-feb-1989 (rogerk)
**	    Integrated changes for shared buffer manager.
**	    Added routines dm0p_cpdone, dm0p_count_connections, dm0p_reclaim,
**	    dm0p_mlock, dm0p_munlock, dm0p_bmid, dm0p_toss_pages, dm0p_dbcache,
**	    dm0p_tblcache, dm0p_eset, dm0p_ewait, dm0p_erelease, reclaim_bmc.
**	    Added dbc_size, tbc_size, cache_name parms to dm0p_allocate.
**	    Added tcb parm to force_page, gforce_page routines.
**	    Broke up buffer manager control block into BMCB/LBMCB.
**	    Added allocation of shared memory in dm0p_allocate.
**	    Made BMCB position independent by taking out all pointers - made
**	    all buffer lists use index values rather than pointers.
**	    Added database and table caches.
**	    Added buffer manager lock list for cache locks.
**	    Added buffer manager lock and buffer manager id.
**	    Made buffer manager mutex a cross process semaphore.
**	    Made changes to CP flush for multi-server fast commit.
**	17-mar-1989 (rogerk)
**	    Fixed read nolock bug introduced with shared buffer manager changes.
**	27-mar-1989 (rogerk)
**	    Fixed bugs in shared buffer manager handling - redid checking for
**	    first connector to buffer manager lock.  Enabled new calls to
**	    LG/LK driver for shared buffer manager.
**	 3-apr-1989 (rogerk)
**	    Pass MULTITHREAD flag when deallocate buffer manager lock list.
**	    Don't decrement server count in dm0p_deallocate until assured we
**	    can disconnect from buffer manager successfully.
**	10-apr-1989 (rogerk)
**	    Added bm_name field to hold buffer manager name.
**	    Verify cache entries in dm0p_dbcache and dm0p_tblcache.
**	    Made it a server fatal error for the buffer manger connect count
**	    to be incorrect in dm0p_count_connections.
**	    Fill in the control block header information when allocate
**	    a shared memory buffer manager.
**	15-may-1989 (rogerk)
**	    Added error code parameters to dm0p_dbcache and dm0p_tblcache.
**	    Added resource limit handling in dm0p_allocate.
**	    Updated MEget_pages call to reflect CL approved interface.
**	 7-jun-1989 (rogerk)
**	    Added new error messages for more informative errors when allocating
**	    shared buffer manager.
**	    Don't flush temp tables during CP.
**	26-jun-1989 (rogerk)
**	    Destroy shared memory segment when last server disconnects rather
**	    than assuming it will be destroyed by the OS.
**	    Put checks in xDEBUG and error handling code of force_page to handle
**	    case where cache_locked page is being bumped but TCB not passed in.
**	29-jun-1989 (rogerk)
**	    Release buffer manager mutex while making IO and lock calls in
**	    gfault_page.
**	10-jul-1989 (rogerk)
**	    Turn on BUF_EXTENDPAGE status when allocate a scratch page.  This
**	    status is left on until the page is forced.  This prevents the
**	    Fast Commit threads from forcing this page.  The page must be forced
**	    by the server which is doing the allocate or else DI will barf.
**	07-aug-1989 (mikem)
**	    "fault_page" change: Do not automatically turn off BUF_EXTENDPAGE 
**	    bit at bottom of page (previous code turned it off thinking it was 
**	    an error case rather than the normal path through.)
**	21-aug-1989 (rogerk)
**	    When skipping newly-extended page during a CP, be sure to increment
**	    cpindex if appropriate.  This prevents unwanted DMD_CHECKs.
**	28-aug-1989 (rogerk)
**	    Pass correct arguments to dm0p_ewait from fault_page.
**	 4-sep-1989 (rogerk)
**	    Mark group buffer as busy while writing pages or releasing locks so
**	    another thread trying to do group faults won't choose it to reclaim.
**	 5-sep-1989 (rogerk)
**	    Don't increment IOWAIT and MWAIT statistics when have to wait in CP.
**	25-sep-1989 (rogerk)
**	    Fix PASS_ABORT problems.  Don't allow non-locked pages to be brought
**	    into the cache while the RCP is doing recovery on behalf of this
**	    server.  Added dm0p_pass_abort that notifies the Buffer Manager
**	    when the RCP is going to do recovery for us.  Added routine
**	    dm0p_recover_state to check/reset this status.  Disallow group
**	    faults when in PASS_ABORT state.  Toss readnolock buffers from
**	    cache when in this state.
**	25-sep-1989 (rogerk)
**	    Make sure we put buffer on transaction list when fix it for WRITE
**	    even if the buffer is already fixed.  If the buffer is fixed only
**	    for READ then it will not be on the transaction list.
**	25-sep-1989 (rogerk)
**	    Don't lock pages in X mode if DM0P_RPHYS action is specified.  This
**	    causes us to lock BTREE index pages in X mode when table is open
**	    for write.
**	 2-oct-1989 (rogerk)
**	    Fix NOLOCKS bug.  When can't acquire cache lock, set status to
**	    E_DB_ERROR correctly.  Was using "==" instead of "=".
**	 2-oct-1989 (rogerk)
**	    Changed to use normal semaphore rather than cross-process semaphore
**	    when not using shared buffer manager.  On some systems, cross
**	    process semaphores cannot be used on non-shared memory.
**	 2-oct-1989 (rogerk)
**	    Fix Consistency Point Bug.  If fault in buffer while in the
**	    middle of a consistency point, mark the buffer with the current
**	    CP number.  This indicates that this buffer does not need to be
**	    flushed as part of the current CP.
**	30-oct-1989 (rogerk)
**	    Fixed uninitialized variable bug in gfault_page which caused us
**	    to not use any but the 1st page of the group read.
**	30-oct-1989 (rogerk)
**	    Added handling in invalidate_page of buffers that are on the
**	    fixed queue and fixed handling of modified group buffers.
**      17-jul-90 (mikem)
**          bug #30833 - lost force aborts
**          LKrequest has been changed to return a new error status
**          (LK_INTR_GRANT) when a lock has been granted at the same time as
**          an interrupt arrives (the interrupt that caused the problem was
**          a FORCE ABORT).   Callers of LKrequest which can be interrupted
**          must deal with this new error return and take the appropriate
**          action.  Appropriate action includes signaling the interrupt
**          up to SCF, and possibly releasing the lock that was granted.
**      11-sep-1990 (jonb)
**	    Integrated ingres6301 change 280136 (rogerk):
**            Fixed shared buffer manager problem involving TCB validation.
**            There was no validation of TCB's being done when a server chose
**            a buffer to toss from the cache.  With a shared cache, it is
**            possible that another server has modifed the table and the
**            tcb held by the local table actually describes the old table
**            (and has a file descriptor for the old table).
**            The implemented fix is to write into the buffer header the tcb's
**            validation stamp - derived from its tcb lock value - each time
**            buffer is read into the cache.  Only tcb's with a matching
**            validation stamp may force these pages out of the cache.
**            Added buf_tcb_stamp to the buffer header.  Store the tcb's
**            validation stamp into it in fault_page/gfault_page.  Added new
**            arguments to dm2t_get_tcb_ptr - must now pass in buffer's
**            validation stamp in order to validate tcb.
**            Added new error message for more information when there is a
**            consistency point error.
**	12-sep-90 (jonb)
**	    Integrated ingres6301 change 280141 (rogerk):
**            Added dm0p_set_valid_stamp routine to update all cached pages
**            for a table to reflect that they are consistent with the current
**            TCB.  This is called whenever a new TCB is built.   Added checks
**            in dm0p_close_page to validate buffer tcb stamps in case we
**            are throwing away an invalid TCB.
**	17-dec-90 (jas)
**	    Smart Disk changes:  renamed 'escalate' to 'dm0p_escalate'
**	    and made it visible (non-static).
**	10-jan-91 (rachael)
**	    Added additional error messages in dm0p_unfix_page, dm0p_tran_
**	    invalidate in response to bugs 
**	 4-feb-1991 (rogerk)
**	    Changed calculation of page_count for group reads to avoid group
**	    IO's that span multi-location file boundaries.  Requesting a multi
**	    page IO that spans location boundaries takes more than one IO.
**	 4-feb-1991 (rogerk)
**	    Added 'toss_modified' argument to dm0p_toss_pages so that the
**	    trace point DM420 (toss pages from cache) can clear out modified
**	    as well as non-modified pages.
**	 4-feb-1991 (rogerk)
**	    Added support for fixed buffer priorities.
**	    Added routines dm0p_stbl_priority, dm0p_gtbl_priority to set
**	    and get a given table's priority.
**	    Added table priority cache to remember user-set table priorities.
**		(this is needed since table priorities are not currently
**		set in the system catalogs.)
**	    Added checks in fault_page for table's priority to override the
**	    buffer manager default priorities.  Added checks for BUF_PRIOFIX
**	    before altering a buffer's priority by clock or on reference.
**	25-feb-1991 (rogerk)
**	    Added trace flag DM426 to bypass group reads and only do singles.
**	    Also did same bypass if table priority has been set by the user -
**	    this allows the pages to remain in memory with a high priority.
**	    Changed dm0p_[gs]tbl_priority to take a normal DB_TAB_ID param.
**	    Added lookup of TCB to set tcb_bmpriority in dm0p_stbl_priority.
**	19-mar-1991 (walt)
**	    Don't let E_DB_WARN status leave dm0p_tran_invalidate.  It's just
**	    internal and confuses later routines if it gets out.
**	3-may-1991 (bryanp)
**	    Make dmd_checks more useful by logging detailed error information
**	    before calling dmd_check.
**	    Add message E_DM9C08_OUT_OF_LOCKS when lock quota is exceeded.
**	19-aug-1991 (rogerk)
**	    Bug 39017:
**	    Changed checks for xact abort in escalate processing.  We do
**	    not want to escalate to table level locking during transaction
**	    recovery.  The old check thought that Set Nologging transactions
**	    were always in abort state.  Changed to actually lookup the
**	    transaction state before escalating.  Exception is when running
**	    as the RCP or CSP in which case we assume we are doing transaction
**	    recovery.  Corresponding changes were made to the SVCB and the
**	    ACP, CSP and RCP to indicate the process type so we can tell when
**	    we are running in the context of a recovery process.
**	17-oct-1991 (rogerk)
**	    Great Merge of DMF : Move 6.4 changes into 6.5.  In this case,
**	    the 6.4 version of this file was moved directly to 6.5 and the
**	    6.5 changes were merged back in:
**
**	    14-jun-1991 (Derek)
**		Added new function dm0p_bicheck() that checks whether and 
**		before-image is needed, and if one is needed what type of
**		before image should be written.  
**
**		Added DM0P_MUTEX, DM0P_FHFMSYNC, DM0P_NOLOCK flags to
**		dm0p_fixpage.  Added DM0P_MRELEASE flag to dm0p_unfixpage.
**
**		Enhanced the buffer manager protocols to handle a new class
**		of pages that can be left modified in the cache until a CP
**		or close and are not owned by a transaction.  These were added
**		to allow the new pages that contain allocation information to
**		be processed with a very low overhead.  These types of pages
**		are marked BUF_PAWRITE and must be forced if a pass-abort
**		is signaled.  Also implemted related BUF_WUNFIX buffer status
**		which indicates that the specified buffer must be written
**		when released.  Added dm0p_wpass_abort routine to force out
**		all PAWRITE buffers during a pass-abort operation.
**
**		Changed TRdisplays done throughout the code in xDEBUG blocks
**		to be activated by tracepoints as well so that code could be
**		compiled with xDEBUG without causing massive TRdisplays.
**
**		Added new session level statistic gathering.
**
**		Added support in dm0p_fixpage to handle the DM0P_CREATE option
**		on already-existing pages.
**
**		Added use of new end-of-file fields in tcb (tcb_lalloc,
**		tcb_checkeof).
**	17-oct-1991 (rogerk)
**	    Added ability to call dm0p_close_page with a secondary index TCB.
**      24-oct-1991 (jnash)
**	    Initialize local priority variable caught by LINT.
**	 3-nov-1991 (rogerk)
**	    Added fixes for File Extend recovery.
**	    Added buffer status PAUNFIX which allows us to tell whether the
**	    WUNFIX condition of a buffer is because of the type of buffer or
**	    because a pass-abort operation is in progress.  This tells us
**	    whether we can reset the WUNFIX status after the pass-abort is
**	    complete.
**	19-Nov-1991 (rmuth)
**	    Added fixes for file extend and CONCUR tables when doing readahead.
**	    When faulting in a group of pages the buf_sts for any of the pages
**	    were not being set see code for comments.
**      16-dec-1991 (bryanp)
**          Add ME_IO_MASK to MEget_pages call for shared buffer cache. This
**          indicates to DI that it is advantageous for the slaves to map this
**          memory (that way, they can perform I/O directly w/o copying).
**      06-jan-92 (jnash)
**          In gforce_page, add support for > 32 gfault pages, up to
**          DM_GFAULT_MAX_PAGES.
**      21-jan-92 (jnash)
**          Revamp Pass Abort processing to introduce generic "pawrite" pages
**          (currently FHDR/FMAP, Btree ROOT and INDEX pages).
**          PAWRITE now means "perform special Pass Abort handling"
**          which means if pass abort, toss on unfix.  Eliminate BUF_PAUNFIX,
**          add BUF_TUNFIX.
**	19-feb-1992 (bryanp)
**	    Fix error message formatting problem in dm0p_allocate().
**	    Add TCB_PEND_DESTROY to tcb_status.
**	07-apr-1992 (jnash)
**	    Add support for locking a shared DMF cache in dm0p_allocate.
**	14-may-1992 (bryanp)
**	    B44260: rcb_lk_limit should not apply during transaction abort.
**	6-jul-1992 (bryanp)
**	    Prototyping DMF.
**      08-jul-1992 (mikem)
**          Fixed code problems uncovered by prototypes.
**	22-sep-1992 (bryanp)
**	    We must have a transaction active in order to call dm0l_logforce.
**	28-sep-92 (kwatts)
**	    In dm0p_allocate, add call of CSn_semaphore to give name to 
**	    the shared cache semaphore.
**	05-oct-1992 (rogerk)
**	    Reduced Logging Project.  In preparation for support of Partial
**	    TCB's for the new recovery schemes, changed references to
**	    some tcb fields to use the new tcb_table_io structure fields.
**	30-july-1992 (jnash)
**	    More Reduced Logging work.
**	      - Add page checksum routines dm0p_compute_checksum,
**		dm0p_validate_checksum, dm0p_insert_checksum, and calls 
**		to these routines.
**	      - Add trace point DMZ_BUF_MACRO(27) to disable checksum.
**	      - Add trace point DMZ_BUF_MACRO(28) to perform checksum
**		and TRdisplay errors, but will not return errors to 
**		caller. 
**	19-oct-1992 (kwatts) change from 6.4 line: 18-nov-91 (kwatts)
**	    Added DM0P_CLFORCE action to dm0p_close_page.
**	22-oct-1992 (jnash) merged 17-oct-1992 (bryanp)
**	    Some ule_format calls were passing &err_code instead of err_code.
**	26-oct-92 (rickh)
**	    Replaced references to DMP_ATTS with DB_ATTS.  Also replaced
**	    DM_CMP_LIST with DB_CMP_LIST.
**	26-oct-1992 (rogerk)
**	  - Reduced logging project: changed references to tcb fields to
**	    use new tcb_table_io structure fields.  Added support for
**	    Partial TCB's. Most buffer manager routines were changed to now 
**	    lookup tableio descriptors rather than TCB's.  This way, they
**	    can perform fix/unfix operations for routines which use partially
**	    built table descriptors.
**	  - Added new routines dm0p_cachefix_page and dm0p_uncachefix_page
**	    which take actions of fixing/unfixing pages in the cache without
**	    the transaction context assumed in dm0p_[un]fix_page.  The routines
**	    dm0p_fix_page and dm0p_unfix_page now call these new routines.
**	  - Added new routines dm0p_lock_page, dm0p_unlock_page to consolidate
**	    page locking code and so it could be called from recovery code.
**	  - Changed dm0p_mutex and dm0p_unmutex calls to take TABLE_IO args
**	    rather than TCB's.
**	  - Changed DM0P_SCRATCH page handling to not mark the new
**	    page as MODIFIED.  The page is formatted (since a garbage page
**	    cannot be left in the cache), but it is up to the caller to
**	    format the page appropriately, write the correct LSN, tran_id
**	    and sequence number, and to mark the page as modified.
**	  - Added buffer manager transaction context to be used by
**	    force_page to force the log file before writing pages.
**	  - Added buffer manager semaphore to protect usage of the
**	    above transaction context.
**	  - Removed dm0p_tran_invalidate routine.  Old users of it now use
**	    dm0p_force_pages.
**	  - Removed dm0p_invalidate routine.  Since BI recovery is no longer
**	    used, cached pages are precious and can't be indiscriminatly
**	    thrown out of the cache.
**	30-October-1992 (rmuth)
**	    - Changed use of tbio_lalloc,tbio_lpageno and tbio_checkeof.
**	    - For temporary tables use/set tbio_tmp_hw_pageno.
**	23-Oct-1992 (daveb)
**	    Attach shared semaphore for monitoring, and name the local
**	    cache semaphore too.  Note we aren't detaching the shared
**	    sem yet; not sure we need to or where the right place is.
**	14-dec-1992 (rogerk & jnash)
**	  - Reduced Logging Fixes:
**	  - Added dm0p_pagetype routine to handle newly created pages which are
**	    not handled properly in fault_page because their page type (and 
**	    special page attributes) are not yet known.
**	  - Fix bug inserting checksum on pages.
**	  - Changed fault_page routines to not set Write-on-Unfix flags on
**	    temporary tables.
**	  - Changed consistency check which looked for RELEASE actions on
**	    unfix of pages not marked as temporary locked pages to bypass
**	    temporary tables and pages fixed for read.
**	14-dec-1992 (rogerk)
**	    Include dm1b.h before dm0l.h for prototype definitions.
**	    Removed DMPP_ACC_PLV argument from dm2f_write_file.
**	27-nov-1992 (robf)
**	    Escalations are security events, in dm0p_escalate audit
**	    an escalation.
**	02-jan-1993 (jnash)
**	    More reduced logging work. Add LGreserve calls.
**      12-jan-1993 (mikem)
**          Fixed routines which were declared to return status but fell off the
**          end without returning a specific status.
**	18-jan-1993 (bryanp)
**	    Log LG/LK error codes.
**	19-jan-1993 (wolf)
**	    Remove some "^T" control characters from yesterday's integration.
**	16-feb-1993 (rogerk)
**	    Fixed shared cache partial TCB problem in dm0p_close_page.
**	18-feb-1993 (rmuth)
**	    Add dm0p_log_error this understands which messages to pass
**	    upto the higher layers this is only called from certain 
**	    routines.
**	15-mar-1993 (jnash & rogerk)
**	    Fix AV in multi-server shared cache CP's.  Don't reference through
**	    null pointer when find a buffer for which this server has no tcb.
**      26-apr-1993 (bryanp/keving/andys)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**		Added changes to allow a buffer manager to work with multiple
**		    logs. Extra have_log parameter to dm0p_allocate and 
**		    extra routine dm0p_register_loghandles
**		Use DB_FAILURE_MACRO to check return status of dm0p_reclaim
**		    as it sometimes returns E_DB_WARN which is OK in
**		    [g]fault_page
**		It's now illegal to fix more than one system catalog page for
**		    write at a time. Enforce this restriction here.
**		In before_image_check, compare the page_log_addr (which is
**		    actually an LSN) with d->dcb_backup_lsn, rather than
**		    d->dcb_backup_addr.
**		Took out special CSP process handling in page locking. The page
**		    locking behavior of CSP recovery is now handled directly
**		    by recovery code and by the locking system.
**	24-may-1993 (bryanp)
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**	    Fix an error message parameter (LK_N should have been LK_IS).
**	    Improve error message logging when lock escalation fails.
**	    If DMZ_LCK_MACRO(2) is set, trace buffer manager cache locking.
**	26-mar-1993 (kwatts)
**	    Action DM0P_CLFORCE in dm0p_close_page was forcing out fixed pages
**	    onto the free chain. Changed the option to force_page so that
**	    pages simply got written in this instance.
**	24-may-1993 (rogerk)
**	    Added dm0p_1toss_page to assist in recovery tests.
**	21-may-1993 (rogerk)
**	    Changed CP flush routine to not probe each page in the cache
**	    unless necessary.  This can prevent Consistency Points from having
**	    to page in the entire buffer cache when parts of the cache have 
**	    been paged out by the Operating System.
**      21-jun-1993 (jnash)
**          Add trace point DM1308 to extend length of CP to 30 seconds.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	26-jul-1993 (bryanp)
**	    Replace all uses of DM_LKID with LK_LKID.
**	26-jul-1993 (rogerk)
**	  - Change generation of Before Image log records during Online Backup.
**	    Instead of calling before_image_check on every dm0p_fixpage 
**	    operation, we now expect the modifier of the page to call 
**	    dm0p_before_image_check before changing its contents.
**	    Made before_image_check external and added dm0p_ prefix.
**	  - Fix dbg_dm9301 routine to not try to test the TCB cache lock if
**	    sole_server mode.  In this case there are no cache locks.
**	30-jul-1993 (shailaja)
**	    Fixed warnings generated by nested comments.
**	23-aug-1993 (andys)
**	    Fix bracketing logic in dm0p_reclaim such that it has a chance
**	    of working. 
**	    Don't use DB_FAIL_MACRO in [g]fault_page calls to dm0p_reclaim
**	    so that we know when we really can't get any more locks.
**	    In gfault_page, if dm0p_reclaim succeeds such that we retry,
**	    decrement the counter so that we retry the buffer we failed on.
**	23-aug-1993 (rachael)
**	    Fix dm0p_force_pages to restart at the beginning of the
**	    transaction list when force_page returns a status of E_DB_WARN
**      13-sep-93 (smc)
**          Commented lint pointer truncation warning, removed truncating cast
**          of page in call to ule_format.
**	20-sep-1993 (bryanp)
**	    Restore accidentally-commented-out code in dm0p_uncache_fix.
**	20-sep-1993 (andys)
**	    Fix arguments to ule_format call.
**	20-sep-1993 (jnash)
**	    Add flag param to LGreserve() calls.
**	18-oct-1993 (rachael) merged 23-mar-1993 (bryanp) -- B47950,B50252
**	    Fix handling of LK_VAL_NOTVALID from LKrequest. Anytime that you
**	    make a lock request call and ask for the value associated with the
**	    lock, LKrequest() may return the special warning code
**	    LK_VAL_NOTVALID. This means that the lock value was marked invalid
**	    by the locking system and cannot be trusted. Typically, this
**	    situation can be handled the same way as the "lock value has
**	    changed" situation is handled: discard the cached object because it
**	    isn't known to be correct. The value-not-value situation will
**	    persist until a new value is assigned to the resource, and we ensure
**	    that this new value is greater than the previous value, thus causing
**	    any other processes which have this object cached to discard their
**	    cached objects as well.
**	18-oct-1993 (rmuth)
**	    Initialise buffer Manager mananged objects.
**	18-oct-1993 (rogerk)
**	    Changed Consistency Points to not start up write-behind threads to
**	    assist in flushing the cache to keep cache contention down
**	    during CP events.
**	    Fixed prototype errors.
**	18-oct-1993 (rogerk)
**	    Changed the mlimit enforcing code in dm0p_uncache_fix and the
**	    flimit enforcing code in fault_page to select pages off of the 
**	    start of the modify priority queues rather than the end of the 
**	    queues to choose the least-recently-used pages as force victims.
**	18-oct-1993 (rogerk)
**	    Added check for mlimit/flimit during LOGFULL conditions to
**	    dm0p_uncache_fix to reduce the number of log forces executed
**	    during recovery processing.
**	    Added dm0p_check_logfull routine.
**	22-nov-1993 (jnash)
**	    Add DM304 to dump locking info upon page or table escalation
**	    deadlocks, or unconditionally if RCP.  
**	24-jan-1994 (gmanning)
**	    Give name to semaphores that are created as CS_SEM_MULTI in order 
**	    to take advantage of cross process semaphores on NT.
**	11-oct-93 (johnst)
**	    Bug #56449
**	    Changed TRdisplay format args from %x to %p where necessary to
**	    display pointer types. We need to distinguish this on, eg 64-bit
**	    platforms, where sizeof(PTR) > sizeof(int).
**	09-nov-93 (swm)
**	    Bug #58633
**	    Corrected memory_needed alignment calculations for buffer manager
**	    memory, in dm0p_allocate(), to be consistent with subsequent
**	    pointer setup to the memory. Corrected buffer manager dbcache
**	    pointer setup.
**	22-feb-1994 (andys)
**	    Integrate recent changes from ingres63p.
**	    This should fix 1.0 bug 59308, as well as 6.4 bugs 46951,
**	    55150 and 54128.
**	    Update to use BUF_MUTEX_OK bit instead of the FCBUSY bit to
**	    indicate that the buffer is mutexed while not fixed.  The FCBUSY
**	    bit now only indicates to the CP threads that another CP thread
**	    is handling that buffer.
**	23-feb-1994 (andys)
**	    Clean up code inside xDEBUG which had not been updated for 1.0
**      14-mar-94 (stephenb)
**          Add new parameter to dma_write_audit() to match current prototype.
**	18-apr-1994 (jnash)
**	    fsync project.  Eliminate dm2f_force_file() calls.
**      17-May-1994 (daveb) 59127
**          Fixed semaphore leaks, named sems correctly.  Delete
**  	    redundant CSn_semaphore call.
**      23-may-1994 (andyw)
**          Should report CSp_semaphore() failure in dm0p_mlock. Bug 58606
**	23-may-1994 (andys)
**	    Add new parameter to dm0p_log_error which says whether to print
**	    the error which we are throwing away. We pass the flag saying not 
**	    to log the error when the flag DM0P_USER_TID is passed to 
**	    dm0p_fix_page, dm0p_cachefix_page or fault_page. This flag is
**	    only currently set by dm1r_get when it is doing a get by tid.
**	    [bug 49726]
**	23-may-1994 (jnash)
**	  - Roger's 18-oct-1993 cache inflation fix was off by one page, 
**	    causing the code to never be executed in offline recovery.
**	23-may-1994 (andyw)
**	    Corrected call to ule_error() for sys_err.
**	20-jun-1994 (jnash)
**	    Eliminate obsolete "logical_logging" TRdisplay.
**	29-Dec-1994 (canor01)
**	    Add NO_OPTIM for AIX. With optimization, createdb -S iidbdb
**	    fails.
**      06-mar-1995 (harpa06 / abowler)
**          Bug #66815: Imported 6.4 change - Suppress error message reporting 
**          when a page is trying to be forced to a temorary file past EOF when
**          write-behind threads exist.        
**      04-apr-1995 (dilma04) 
**          Add support for rcb_k_duration == RCB_K_CURSOR_STABILITY and
**          rcb_k_duration == RCB_K_REPEATABLE_READ.
**	19-apr-1995 (cohmi01)
**	    Various performance changes related to the ManManX research:
**	    gfault_page() -If no free groups are found, besides writing out a
**	    modified group, signal the write-behind threads so that other
**	    ones will be written out, now that write-behinds handle groups.
**	    dm0p_flush_pages() - Add support for group buffers to write-behind 
**	    thread. Manage thresholds differently if in batch mode.
**	    dm0p_fix_page() - Always use READAHEAD when running in 'batch mode'.
**	24-apr-1995 (cohmi01)
**	    For new IOMASTER server write-along thread: add dm0p_write_along(),
**	    dm0p_no_lg_force(). Modify fix_tableio_ptr() to build new TBIO
**	    if DM0P_TBIO_BUILD specified.
**	05-may-1995 (cohmi01)
**		Obtain 'batch' status from dmf_svcb rather than bmcb.
**      25-Apr-1995 (jenjo02)
**          Combined CSi|n_semaphore functions calls into single
**          CSw_semaphore call.
**      15-may-1995 (thoro01)
**          Added NO_OPTIM hp8_us5 to this file.
**	20-jun-95 (lewda02)
**	    Make DM0P_LOCKREAD test stronger.
**      19-jun-1995 (hanch04)
**          changes NO_OPTIM for hp8_us5 to pragma optim 2 
**      12-jun-1995 (canor01)
**          add semaphore protection for memory allocation (MCT)
**      12-jun-1995 (canor01)
**          semaphore protect NMloc()'s static buffer
**	26-jun-1995 (cohmi01)
**	    Provide for priority escalation of pages read by readahead thread
**	    so that if Pri queues become imbalanced with index/root pages,
**	    the user thread still gets the benefit of a leaf having been read.
**	    Fix old bug in fault_page where victims from free queue were
**	    being taken from most recently used pages instead of LRU.
**	27-jun-1995 (cohmi01)
**	    Use bitwise & instead of == for DM0P_FORCE test in dm0p_uncache_fix.
**	17-jul-1995 (cohmi01)
**	    Backout lewda02 DM0P_LOCKREAD change - causes page locks
**	    when not needed - eg when entire table is already locked,
**	    and thwarts the intent of other rcb flags related to locking.
**	    For Readahead thread - check for interrupt in dm0p_fix_page
**	    so that cancelled/stale requests incur no extra IO.
**      27-july-1995 (carly01)
**	    b69767 - set buffer priority and type for "in memory" tables.
**	18-sep-1995 (nick)
**	    Resolve clash of tracepoints - move checksum tracepoints to
**	    new values. #63536  Ingres63p tracepoint DM427 had made it here too
**	    so rename to DM434.
**	24-oct-1995 (thaju02/stoli02)
**	    Added buffer manager single and group sync write counters.
**      08-feb-1996 (stial01)
**          dm0p_flush_pages() Don't look at bm_gmcount unless the buffer 
**          mutex is held, OR expect that the group modify queue may be
**          empty by the time you get the mutex.  
**          We saw a non zero bm_gmcount, got the mutex, by which time the
**          group modify queue was empty and we called gforce_page with
**          an uninitialized value for DM0P_GROUP *g. This resulted in a SEGV
**          in gforce_page(). Note sometimes the session would be hung
**          waiting on LOGIO.
**	25-mar-96 (nick)
**	    Various changes to fix the 'JPM' bug. #71597
**	30-may-1996 (pchang)
**	    Added support for DM0P_NOESCALATE action in dm0p_fix_page(), to be 
**	    used by sessions already holding an X lock on the FHDR when fixing 
**	    pages other an FMAP (i.e. without the DM0P_NOLOCK action).  Lock 
**	    escalation under such condition may lead to unrecoverable deadlock
**	    involving the table-level lock and the FHDR. (B76463) 
**	    Also, changed to display the E_DM9041_LK_ESCALATE_TABLE message 
**	    only prior to actual lock escalation.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**          Add page_size argument where necessary
**          Reserve correct space in log
**          Pass page_size argument to dm0l_bi
**          Pass page_size to dm2f_write_file(), dm2f_read_file()
**      17-apr-1996 (stial01)
**          Fixed incorrect reference to rpages 
**      01-may-1996 (stial01)
**          LGreserve adjustments for removal of bi_page from DM0L_BI
**      03-jun-1996 (canor01)
**          New ME for operating-system threads does not need external
**          semaphores. Remove NMloc() semaphore.
**	17-jun-96 (nick)
**	    Change LGreserve() call in dm0p_before_image_check() to reserve
**	    space for the CLR as well.
**	 4-jul-96 (pchang)
**	    More lock escalation stuff. #76463
**	17-jul-96 (nick)
**	    Use E_DM9063_LK_TABLE_MAXLOCKS instead of 
**	    E_DM9041_LK_ESCALATE_TABLE when we exceed the RCB lk limit.
**	29-aug-96 (nick)
**	    Only print E_DM9063_LK_TABLE_MAXLOCKS for user relations.
**      12-dec-1996 (cohmi01)
**          Add more info to lock timeout/deadlock msgs (Bug 79763).
**      01-aug-1996 (nanpr01 for ICL phil.p)
**          Introduce support for the Distributed Multi-Cache Management    
**          (DMCM) protocol.
**          DMCM is effectively Fast-Commit in a Multi-Cache environment.
**          All (but system catalog) pages retained in cache at the end
**          of transaction. Dirty pages retain their original lock, others
**          have the lock dropped to null.
**          Added dm0p_dmcm_callback, the function called when locking
**          contentions occur, after a lock request is made via
**          LKrequestWithCallback.
**          Added dmcm_cache_flush, called by a server running DMCM to
**          flush the cache of pages it last locked.
**          In dm0p_cachefix_page, fault_page, gfault_page and validate_page
**          calls to LKrequest have been replaced with calls to
**          LKrequestWithCallback (supplying dm0p_dmcm_callback as the
**          callback function).
**          dm0p_deallocate calls dmcm_cache_flush to empty the cache of
**          buffers locked by the current server before deallocating the
**          servers cache on server shutdown.
**          force_page modified to support an extra mode FP_NULL, used to
**          cause pages to be retained in cache with a null lock.
**          Integrate ICL gather write and DMCM with VPS:
**            Fix DM0P_BIO_EVENT, DM0P_FWAIT_EVENT, DM0P_PGMUTEX_EVENT 
**            Fixed declarations/initialization of bm_common, bm, cache_ix..
**            Added loop through all caches where necessary
**	24-Oct-1996 (jenjo02)
**	    Semaphore granularity project. Many new semaphores replace
**	    lone bmcb_mutex.
**	07-Nov-1996 (jenjo02)
**	    In gforce_page(), if group is busy, do nothing but return E_DB_WARN.
**      22-nov-96 (dilma04, stial01)
**          Row Locking Project:
**          dm0p_page_lock_mode(), dm0p_latch_page() Created;
**          dm0p_fix_page() Latch page when row locking;
**          dm0p_unfix_page() Unlatch page when row locking;
**          dm0p_lock_page() Support for LK_PH_PAGE lock requests;
**          dm0p_unlock_page() Support for LK_PH_PAGE lock requests;
**          dm0p_escalate() Release all row, value and LK_PH_PAGE locks;
**          dm0p_trans_page_lock() added *tcb argument, replaced access_mode
                with lock_mode.
**	22-Nov-1996 (jenjo02)
**	    In gforce_page(), when waiting for a busy buffer, make sure that
**	    all buffers in the group have their mutexes released.
**      12-dec-96 (dilma04)
**          In dm0p_page_lock_mode: remove unneeded check for base table;
**          add dmd_check for debugging purposes.
**	12-Dec-96 (jenjo02)
**	    Removed dm0p_match() and code which MO attached to shared buffer
**	    manager mutexes.
**      06-jan-97 (dilma04)
**          Fix few row locking problems:
**          - request to release row locking specific locks in dm0p_escalate()
**          should be made unconditionally even if locking granularity of a 
**          cursor causing escalation is page level (bug 79817);  
**          - in dm0p_unfix_page() take out the optimization to release an
**          intent page lock if no qualifying records are found on the page;
**          - to prevent bug 79611 situation because of previous change, 
**          in dm0p_trans_page_lock() acquire a physical IX lock on a page
**          if row locking and DM0P_NOESCALATE flag is set;
**	10-jan-1997 (nanpr01)
**	    Put back the space holder for DM0L_BI, DM0L_AI.
**	05-Feb-1997(jenjo02)
**	    Defined new event wait, DM0P_GWAIT_EVENT. In gforce_page(),  
**	    if the group is busy, wait for it to become unbusy instead
**	    of just returning with a warning.
**      12-feb-97 (dilma04)
**          Set Lockmode Cleanup:
**          - test rcb_iso_level for isolation level; 
**          - use macro to check for serializable transaction.
**	12-Feb-1997 (jenjo02)
**	    Removed bm_eq and references to it. This "empty queue" of buffers
**	    serves no purpose and may be an artifact pre-dating the 
**	    free priority queue 0.
**	    When waiting for or signalling DM0P_FWAIT_EVENT, protect 
**	    bm_status with its mutex.
**	    A bunch of changes to how and when modified pages are forced;
**	    don't force pages which might induce a log force if it can be
**	    avoided, ensure that free, modified, and fixed queues are
**	    properly aged, use SHARE mutex when scanning free/modified 
**	    queues.
**	    Reinstated lbm_lxid_mutex, used in those cases in which an lx_id
**	    isn't passed to this function to single-thread the use of 
**	    the BM's default lx_id.
**	24-Feb-1997(jenjo02 for somsa01)
**	    Cross-integrated 428939.
**	    Added tbio_extended to checking of Buffer Manager Force Protocols
**	    on core catalogs in fault_page(), gfault_page(), and
**	    dm0p_pagetype().
**	26-feb-1997 (dilma04)
**	    Change dm0p_trans_page_lock, dm0p_unfix_page() and dm0p_escalate()
**	    to support lock escalation for row locking and lower isolation
**	    levels.
**	24-Mar-1997 (jenjo02)
**	    Table priority project:
**	  - Cache priority may now be set when table/index is created or modified.
**	  - Removed trace point DM425, dm0p_stbl_priority(), dm0p_gtbl_priority(),
**	    bmcb_tblpsize, bmcb_tblpindex, lbm_tblpriority.
**	  - Replaced BMCB_MAX_PRI with more widely-known DB_MAX_TABLEPRI.
**	  - Added new stats for fixed priority buffers.
**	  - Moved increment of bs_force into complete_f_p() so it can be consistently
**	    maintained.
**	  - A host of repairs to group buffer concurrency and DMCM problems.
**	  - Maintain buffer cache lock mode (buf_lock_mode) for all buffers, 
**	    not just DMCM.
**	  - Removed buf_pid, which is meaningless in a private cache.
**	01-May-1997 (jenjo02)
**	  - If group buffer's group is busy being faulted or forced, wait
**	    for the group, then retry (dm0p_cachefix_page()).
**	    Added new stat for number of group waits.
**	  - dm0p_cp_flush():
**	    - Rather than waiting for a BUSY or MUTEXed buffer, mark that spot
**	      and go on to find useful work. Iteratively return to those buffers
**	      until all skipped buffers have been processed for this CP.
**	    - Force only those pages which must be included in this CP.
**	      Previously, all modified buffers encountered were written, even
**	      those which were modified after the CP's LSN was established, 
**            causing the CP thread to act as a write-behind thread and 
**            extending the time needed to take the CP.
**	  - Let fixed priority buffers be flushed - after all, they'll just
**	    be written, not tossed (dm0p_flush_pages()).
**	06-May-1997 (jenjo02)
**	  - When checking the page modified bit (dm0p_close_page()), use the page size
**	    in the buffer, not the one from the tbio. The TCB at
**	    this point is that of the base table; if an index is
**	    of a different page size, this tbio won't work!
**	  - Rescinded LSN check outlined above. It's possible for a page to
**	    be modified (LSN1 ), have this CP start (CP LSN), and have the
**	    page be modified again (LSN2). If LSN2 > CP LSN, the page won't
**	    get written.
**      07-may-97 (dilma04)
**          Bug 81895. In dm0p_lock_page(), if LK returns LK_COV_LOCK, treat 
**          this lock as a new one.
**	08-May-1997 (jenjo02)
**	    Removed resetting of BUF_LOCKED | BUF_CACHED when unlocking a page.
**	    This wasn't needed and was being done without the protection of
**	    the buffer's mutex (complete_f_p()).
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          - implemented page lock coupling for CS and RR isolation levels;
**          - added lock_id argument to dm0p_lock_page(), dm0p_unlock_page
**          and dm0p_trans_page_lock;
**          - added support for releasing locks by lock id and for releasing
**          exclusive locks.
**	19-May-1997 (jenjo02)
**	    In complete_f_p(), put empty pages at the head of the free queue
**	    so that fault_page() will pick them before valid pages, which are
**	    placed at the end of the free queue.
**      21-may-1997 (stial01)
**          Row locking: Replace LK_PH_PAGE locks with lock on buffer
**          dm0p_fix_page: Remove request for LK_PH_PAGE lock
**          dm0p_unfix_page: Remove release of LK_PH_PAGE locks
**          dm0p_cachefix_page(), fault_page(), gfault_page():
**            - Buffer may be fixed for write by multiple transactions:
**              Do not put buf with IX page lock on xn queue
**            - Removed code for non-fast-commit when row locking (not allowed) 
**            - Removed code for BUF_PAWRITE when IX lock. Pass abort of
**              row locking transactions is not permitted since there is no way
**              to coordinate page access between the dbms server and the
**              RCP server except if fast commit and cache locking (DMCM).
**          dm0p_uncache_fix: Crash server if refcount is 0 and buffer is locked
**          dm0p_escalate: Remove release of LK_PH_PAGE locks
**          dm0p_force_pages() Skip any buffers with IX lock on the xn queue
**          dm0p_mutex: Mutex not granted if buffer is locked
**          dm0p_unmutex: New flags parameter to lock/unlock the buffer
**      29-may-1997 (stial01)
**          Rename DM0P_IX_LOCK -> DM0P_INTENT_LK, BUF_IX_LOCK -> BUF_INTENT_LK
**          dm0p_fix_page: If IS/IX/SIX page lock, action |= DM0P_INTENT_LK
**        - dm0p_uncache_fix: Added additional consistency checks if row locking
**          dm0p_unmutex: zero out buf_lock_id when buffer is unlocked
**      13-jun-97 (dilma04)
**          Repeatable Read optimization in dm0p_unfix_page(): set flag to 
**          release leaf page's lock, if this page has not been updated.
**      28-Jul-97 (merja01)
**         move preprocessor stmts to col 1 to fix compile errors on axp_osf.
**	31-jul-97 (muhpa01)
**	    Removed the pragma for level 2 optimization for hp8_us5 - level 2
**	    is now the default.  Added NO_OPTIM for hp8_us5 - this cures a
**	    SIGBUS error in dm0p_allocate().
**	21-Aug-1997 (jenjo02)
**	    In dm0p_flush_pages(), lock buffer before locking group to adhere
**	    to mutex protocol and avoid deadlocks.
**	25-Aug-1997 (jenjo02)
**	    Added log_id (LG_LXID) parameter to dm0p_close_page(), dm0p_toss_pages(),
**	    dm0p_wpass_abort(), dm0p_tblcache(), complete_toss_pages(), unfix_tableio_ptr(),
**	    fix_tableio_ptr(), complete_wpass_abort() so that it can be passed into
**	    force_page() and ease the burden on the buffer manager's default log_id,
**	    lbm_lxid.
**	28-Aug-1997 (jenjo02)
**	    Well, the 21-Aug change didn't work all that well. Redid group-busy logic,
**	    in general, lock group, set busy status, unlock group, then lock buffer.
**	    Functions affected: gforce_page(), gfault_page(), dm0p_force_pages(),
**	    dm0p_flush_pages(), dm0p_write_along().
**      28-aug-97 (musro02)
**         Added nooptim for sqs_ptx to avoid undefined symbol .BKdp0
**	03-Sep-1997 (jenjo02)
**	  o Cross-integration of 427407. page_number in dm0p_1toss_page(),
**	    dm0p_trans_page_lock(), should be DM_PAGENO (u_i4), not i4.
**	  o Bug 76034 (Alex Hanshaw) cross-integration from ingres63p. Initialize
**	    fields in group buffers that were not being initialized.
**	29-Sep-1997 (merja01)
**		Remove left over their>> from a prior integration.
**	29-aug-1997 (bobmart)
**	    Allow user to configure the types of lock escalations to be
**	    logged to the errlog.
**      29-Aug-1997 (horda03,thaal01)
**          ifdef VMS around ule_format() function due to the different structure
**          of CL_ERR_DESC on VMS.Temporary fix !!
**    05-sep-1997 (bobmart)
**        Change comment character on horda03's last "ifdef VMS" integ.
**      12-nov-97 (stial01)
**          Pass lockKEY even if unlocking with lockID (for set lock_trace)
**          Consolidated rcb fields into rcb_csrr_flags.
**	19-Nov-1997 (jenjo02)
**	    complete_f_p() was releasing buffer's buf_mutex before returning with an
**	    I/O error, causing dmd_checks() when the force_page() caller then tried
**	    to release the same mutex.
**	10-Dec-1997 (jenjo02)
**	  o In functions which are apt to force more than one page, keep the TBIO
**	    instead of always releasing it; if the next page to be forced is for the
**	    same table, reuse the TBIO - if not, unfix the TBIO and fix another
**	    one. Functions affected: dm0p_force_pages(), dm0p_cp_flush(), 
**	    dm0p_flush_pages(), dm0p_toss_pages(), dm0p_wpass_abort(), dm0p_write_along().
**	  o Rewrote code which scans modified and fixed priority queues such that it
**	    can do so without taking a _queue mutex, eliminating the bulk of contention
**	    between write-behind threads and fault_page()/gfault_page().
**	  o Eliminated the single buffer (bm_fq) and group (bm_gfq) fixed state queues.
**	    These queues were never read and served no real purpose other than 
**	    contentious resources. Two queues remain, the free and modified. Buffers
**	    which are fixed or fixed and modified, appear on no state queue.
**	19-Jan-1998 (jenjo02)
**	    Renamed fields associated with transactions queues to make them more
**	    generally useful. Added bmcb_tabq_buckets, a hash table which 
**	    cross-cache links BUF_VALID pages belonging to a table. This hash 
**	    is used in functions which previously sequentially searched
**	    all caches to find pages associated with a table: dm0p_close_page(),
**	    dm0p_reparent_pages(), dm0p_toss_pages(), dm0p_dmcm_callback(), and
**	    dm0p_count_pages(). In large cache configurations, this was a 
**	    prohibitively expensive operation.
**	29-Jan-1998 (jenjo02)
**	    Refined table cache queue search techniques to avoid taking mutexes
**	    where possible.
**  26-Feb-1998 (bonro01)
**      Added cleanup code for cross-process semaphores during
**      MEsmdestroy().
**	24-Mar-1998 (jenjo02)
**	    When computing table hash queue bucket, cast table_id to (u_i4).
**	    Using i4 causes DG (at least) to come up with the wrong
**	    bucket and leads to (at least) incomplete CP errors!
**	20-May-1998 (jenjo02)
**	    Rewrote WriteBehind facility. WriteBehind threads are now cache-specific,
**	    dynamic, and demand-driven.
**	    Added cache_ix to BMCB and TBIO to avoid recomputing it all the time
**	    from page size.
**	    Set BUF_TEMPTABLE status in temptable buffers so that CP flush can detect
**	    them without the need of a TBIO.
**	01-Jun-1998 (jenjo02)
**	    In dm0p_start_wb_threads(), check for LBMCB; if STAR server, there won't be one.
**	10-Jun-1998 (jenjo02)
**	    Callers of gforce_page() are solely responsible for awakening group waiters.
**	11-Jun-1998 (jenjo02)
**	    Added flags parm to dm0p_ewait() in which to pass LK_E_INTERRUPT,
**	    used by DM0P_WBFLUSH_EVENTs.
**	    Changed event type to be a composite of DM0P_WBFLUSH_EVENT and the cache_ix
**	    being serviced by the thread so lockstat can display it.
**	23-Jun-1998 (jenjo02)
**	    Added LK_VALUE parm to dm0p_unlock_page(), dm0p_lock_page() prototypes.
**	    Don't assume a buffer is on transaction queue just because 
**	    page is fixed for write, but check explicitly. Remove buffer
**	    from txn queue when unfixed.
**	    In fault_page(), moved release of buf_mutex til after all buf_sts bits
**	    have been decided on and set.
**	    Tidied up use of DMPP_GET_PAGE_STAT macros.
**	20-Jul-1998 (jenjo02)
**	    Jumbo cache changes:
**	    LKevent() event type and value enlarged and changed from 
**	    u_i4's to *LK_EVENT structure to support more than 65536 total
**	    buffers.
**	    When row locking, newly allocated pages (DM0P_CREATE | DM0P_CREATE_COND)
**	    are now fixed LK_X, not LK_IX, and converted down to LK_IX by dm1p.c to
**	    close a concurrency window whereby multiple threads could be fixing and
**	    formatting the same page at the same time.
**	    In dm0p_cp_flush(), if the modified and fixed buffer counts are, or are 
**	    reduced to, zero, skip the remainder of the cache - there are no buffers 
**	    which need flushing.
**	19-Aug-1998 (jenjo02)
**	    In dm0p_fix_page(), return lock id into rcb_fix_lkid.
**	01-Sep-1998 (jenjo02)
**	  o Renamed DM0P_CREATE_COND flag to DM0P_CREATE_NOFMT to identify its
**	    new interpretation. When passed from dm1p (scan_map), the newly allocated
**	    page will be fixed into the cache, but will be mutexed instead of formatted; 
**	    dm1p will instead do the formatting. This fixes concurrency problems in which
**	    multiple threads fixed and eventually (re-)formatted the same page. 
**	    scan_map will retest the "free page" bit before committing to formatting
**	    the page.
**	  o Moved BUF_MUTEX waits out of bmcb_mwait, lbm_mwait, into DM0P_BSTAT where
**	    mutex waits by cache can be accounted for.
**	01-oct-1998 (somsa01)
**	    In complete_f_p(), in case of E_DM9448_DM2F_NO_DISK_SPACE
**	    and tbio_temporary, invalidate the page and return an OK
**	    status. Also, pass in log_id to complete_f_p().  (Bug #93633)
**	08-oct-1998 (somsa01)
**	    In complete_f_p(), corrected last cross-integration; cache_ix
**	    is now bm->bm_cache_ix. Also, invalidate_page() now takes four
**	    parameters.
**	22-Oct-1998 (jenjo02)
**	    Repaired some static declarations the HP compiler complained about.
**	29-Oct-1998 (jenjo02)
**	  o Added new action, DM0P_PHYSICAL_IX, which induces a physical IX
**	    lock when row locking instead of a physical X (dm0p_page_lock_mode()).
**	  o Lock requests for PHYSICAL locks made non-interruptable.
**	  o When running with Ingres, not OS threads, don't do dirty, unmutexed
**	    list scans. This can induce long-lasting loops because the looping
**	    thread fails to yield to other threads to alter the condition
**	    of the objects on the list.
**	03-Dec-1998 (jenjo02)
**	    Removed test for, and skipping of, fixed priority buffers when
**	    scanning the modified and free queues in fault_page(). 
**	    It ate CPUs and was really of no benefit.
**      15-dec-1998 (kitch01)
**	    Bug 94536. Added err_code for passing to ule_format instead of
**	    error.
**	    In functions complete_toss_pages() and complete_wpass_abort().
**	22-Dec-1998 (jenjo02)
**	    Align start of page buffers on DI_RAWIO_ALIGN for raw database
**	    locations.
**	15-jan-1999 (nanpr01)
**	    Pass pointer to pointer to appropriate function.
**	19-jan-1999 (muhpa01)
**	    Removed NO_OPTIM for hp8_us5.
**	21-Jan-1999 (jenjo02)
**	    In fault_page(), read the page if DM0P_CREATE_NOFMT and the page
**	    to be read is within the bounds of formatted pages for the table
**	    (page is being reused).
**	29-jan-1999 (hayke02 for stial01, jenjo02)
**	    Check for BUF_FIXED when scanning the modified queue for buffers
**	    to write when (i) below the free buffer limit in fault_page(),
**	    (ii) the modify limit has been reached in dm0p_uncache_fix() and
**	    (iii) when flushing modified pages from the buffer cache in
**	    dm0p_flush_pages(). This prevents E_DM934D and E_DM930C errors.
**	    This change fixes bug 94855.
**	01-Feb-1999 (jenjo02)
**	    In dm0p_pagetype(), determine buf_type unconditionally, not just 
**	    for temp tables. Added consistency check to ensure that the right 
**	    page is mapped and that it's fixed.
**      13-feb-1999 (nanpr01)
**          Support update mode lock.
**      22-Feb-1999 (bonro01)
**          Replace references to svcb_log_id with
**          dmf_svcb->svcb_lctx_ptr->lctx_lgid
**      08-Mar-1999 (wanya01)
**          dm0p_cachefix_page() Bug 91094. If the group is not busy, we 
**          make it busy, and we make it free until finishing move_group. This
**          prevent another thread from taking the same group from the free
**          queue. 
**      28-apr-1999 (hanch04)
**          Replaced STlcopy with STncpy
**      16-Apr-1999 (hanal04)
**          Prevent incorrect err_code (LK_NOLOCKS) being returned from
**          dm0p_trans_page_lock() when lock escalation error logging
**          has been disabled. b96165.
**	06-May-1999 (jenjo02)
**	  o Redesigned CP buffer picking mechanism. When a CP is signalled, the
**	    caches are quickly dirty-scanned for "candidate" pages (modified or 
**	    fixed for write and dirty, no temptable pages) and placed on an 
**	    installation-wide CP queue. The CP threads then operate only on 
**	    this working-set of buffers. Buffers which were fixed but not
**	    modified prior to the start of the CP need not be processed, but
**	    would have been using the replaced cache-bashing algorithm.
**	    New static functions cp_prep() and cp_flush().
**	  o Replaced buffer's buf_index with a DM0P_BID structure, buf_id,
**	    which contains both the buffer's index and its cache index. Thus
**	    the cache can easily be found given only a buffer, and it's no
**	    longer necessary to also pass a pointer to the buffer's DM0P_BMCB 
**	    (*bm) when the buffer is known.
**	  o Added buf_page_offset (offset to page within cache) so that it
**	    doesn't have to be constantly computed.
**	29-Jun-1999 (jenjo02)
**	    Implemented GatherWrite for cache-flushing operations.
**	12-Jul-1999 (jenjo02)
**	    Replaced CP candidate buffer list with array of buffer identifiers.
**	    Once extracted, the array is sorted into (dbid,table_id,index_id,page)
**	    order, then fed to cp_flush().
**	16-aug-1999 (nanpr01)
**	    Implementation of SIR 92299. Enabaling journalling on a
**	    online checkpoint. Turn on cache locking for iirelation catalog.
**	24-Aug-1999 (jenjo02)
**	    Added 2 new buf_type values, BMCB_FHDR and BMCB_FMAP, previously
**	    identified as "BMCB_ROOT" types. Added stats by buf_type to aid
**	    in analyzing cache LRU behavour. Bump buf_priority each time
**	    a page is found in cache, whether currently fixed or not, instead
**	    of only when not currently fixed. Added sth_count to state queue
**	    header structure to track number of elements on a state queue.
**	    When reclaiming a buffer for reuse (fault_page()), mark it as
**	    BUF_RECLAIM, signalling force_page() to remove it from whatever
**	    queue it's on but not insert it on to the free queue.
**	09-Sep-1999 (jenjo02)
**	    Check breach of RCB lock limit only if a new lock was acquired
**	    to avoid bogus lock escalations and secondary escalate deadlock 
**	    during transaction abort.
**	14-Sep-1999 (jenjo02)
**	    24-Aug-1999 change sent free buffer counts (bm_fcount) wildly out 
**	    of control, effectively shutting down the WriteBehind mechanism.
**      20-sep-1999 (stial01)
**          dm0p_unfix_page() Convert lock on dirty etab leaf pages (2k page)
**	11-Oct-1999 (jenjo02)
**	  o cachefix_page(): No need to wait for write-busy buffer if it's 
**	    being written by a CP or WB thread; it'll still be valid after
**	    the write completes.
**	  o fault_page(): ifdef'd out clock aging and its contentious mutexing.
**	    More mods to free page selection.
**	  o Test locally for security before wasting a call to dma_write_audit().
**	  o Added 2 new external functions, dm0p_lock_buf/dm0p_unlock_buf, to
**	    set and release a BUF_MLOCK on a page without excessive mutexing
**	    of the buffer's buf_mutex.
**	  o cp_flush(): Only set BUF_MUTEX on BUF_FIXED pages to prevent 
**	    unnecessary mutex waits.
**	  o cp_flush()/dm0p_flush_pages(): If synch writes occur, abandon use
**	    of GatherWrite to minimize the time the buffers are in a BUF_BUSY
**	    state and unavailable to other threads.
**	27-Oct-1999 (jenjo02)
**	    When tossing a page from the cache, invalidate its page number by
**	    setting it to -1. This keeps external viewers, for example dm1p, 
**	    from viewing the page in an inconsistent state.
**      12-nov-1999 (stial01)
**          Changes to how btree key info is stored in tcb
**	30-Nov-1999 (jenjo02)
**	    Added DB_DIS_TRAN_ID parm to LKcreate_list() prototype.
**	15-Dec-1999 (jenjo02)
**	    Removed DB_DIS_TRAN_ID parm from LKcreate_list() prototype.
**	    Added DB_DIS_TRAN_ID parm to LGbegin() prototype.
**	10-Jan-2000 (jenjo02)
**	    Implemented group priority queues, modified WriteBehind code
**	    to reduce thrashing on low-priority modified queues.
**	    When faulting in temptable pages, reduce their resultant
**	    buffer priority by one to cause them to age sooner than
**	    non-temptable pages.
**	    Removed static page state queue counts (bm_fcount, bm_mcount,
**	    bm_lcount, etc) and contentious bm_count_mutex. New macros
**	    (DM0P_COUNT_?) now tally the counts when they are of interest.
**	    Inlined CSp|v_semaphore calls. dm0p_mlock|munlock are now 
**	    macros which call dm0p_mlock_fcn()|dm0p_munlock_fcn()
**	    iff the semaphore request fails.
**	09-Mar-2000 (jenjo02)
**	    In dm0p_cp_flush(), unfix any TBIO left around before waiting
**	    for a BUF_MUTEXed page to avoid undetectable deadlock.
**	13-Apr-2000 (jenjo02)
**	    Corrected loop in gfault_page() caused by earlier failed
**	    gfault_page() and mishandling of g_valcount.
**	20-Apr-2000 (jenjo02)
**	    Spread cache allocations across multiple segments if 
**	    segment overflow occurs. Allocate CP array per cache instead
**	    of cross-cache, induce WriteBehind participation in CP flush.
**	    Moved defines of CACHENAME_SUFFIX, CACHENM_MAX to dm0pbmcb.h
**	    for exposure in cacheutil.c, which til now had the values
**	    hard-coded.
**      22-May-2000 (islsa01)
**          Declare this function dm0p_check_logfull to EXTERN instead of
**          local to this file. (B90402)
**	23-May-2000 (jenjo02)
**	    CP flush failures reported if cp_array's pages' TCBs have
**	    all been purged by the time the CP thread sees them. This
**	    is ok as long as the pages have been stamped processed
**	    for the CP and should be tolerated.
**	14-Jun-2000 (jenjo02)
**	    Simplified and streamlined interface to gforce_page().
**	    In move_group(), must wait for busy group before modifying 
**	    its attributes and/or changing its queue.
**	23-Jun-2000 (jenjo02)
**	    Added fill_option to dmpp_format() prototype, set by caller
**	    to DM1C_ZERO_FILL if dmpp_format should MEfill the page,
**	    DM1C_ZERO_FILLED if the page has already been MEfilled.
**	    The page overhead space is always zero-filled.
**      06-jul-2000 (stial01)
**          Changes to btree leaf/index klen in tcb (b102026)
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	24-Feb-2000 (devjo01)
**	    Corrected parameters to ule_format calls for E_DMA00D and
**          E_DM004B.  (b100589).
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	08-Aug-2000 (jenjo02)
**	    Remove test for WriteBehind in gfault_page(), which was
**	    causing group reads to be abandoned when they should not
**	    have been. (B102294)
**      31-aug-2000 (stial01 for jenjo02)
**          ignore E_DM0054 for indexes (B83999)
**	08-Sep-2000 (jenjo02)
**	    Rewrote checksum algorithm to use first and last 16 bytes
**	    of page instead of entire page. This makes the cost of
**	    computing a checksum static rather than logarithmic.
**	    Old algorithm retained for read-compatibility with 
**	    existing databases.
**	22-Sep-2000 (jenjo02)
**	    Added g_lock_list to DM0P_GROUP, lock list of thread that
**	    set G_BUSY state. move_group() won't wait for busy group
**	    if the lock list matches.
**	11-Oct-2000 (jenjo02)
**	    Fix endless loop in fault_page() if neither a free nor
**	    modified page can be found to force.
**      23-oct-2000 (stial01)
**          Variable Page Size - Variable Page Type support (SIR 102955)
**	 6-feb-2001 (mosjo01)
**	    Removed sqs_ptx from NO_OPTIM (seems to work optimized).
**          Consolidated dr6_us5 and rs4_us5 into one NO_OPTIM.
**
**	    For sqs_ptx ONLY, I am bypassing gfault_page() for <Temporary
**	    Tables>. This avoids DirectRead errors randomly occuring in 
**	    cldio.c (DIread and IIdio_read) with II_DIRECT_IO in effect.
**	    Added xDEBUG TRdisplay traces under gfault_page().
**	9-mar-2001 (toumi01)
**	    If this is the last (or only) CP thread to process this cache,
**	    ensure that every buffer in the CP array has been processed.
**	    This situation could occur, for example, if an assisting
**	    WriteBehind thread (which does -not- maintain cpindex) has
**	    beaten us to the CP array, processed every buffer, and closed
**	    every TCB.
**      09-mar-2001 (stial01)
**          Changes for Variable Page Type SIR 102955
**	21-mar-2001 (devjo01)
**	    When using INTERNAL threads, server could hang if all group
**	    buffers had busy TCB's.  Fixed 'all_group_tcbs_busy' logic
**	    to avoid this (b104299).
**	09-Apr-2001 (jenjo02)
**	    Changed dm2f_write_list() prototype to include pointers
**	    to GatherWrite stats collectors.
**	21-May-2001 (jenjo02)
**	  o Collapsed all those specialized "complete_...()" GatherWrite
**	    I/O completion routines into just "complete_f_p()", which
**	    is now called for all write completion operations whether
**	    GatherWrite is enabled or not.
**	  o Added "awaken_buf_waiter()" to handle the resumption of 
**	    buffer I/O, Mutex, and FC waiters in a consistent fashion,
**	    reset the status, release buf_mutex, resume waiting threads,
**	    rather than prematurely resuming the thread then resetting
**	    status then releasing buf_mutex only to have the resumed
**	    thread potentially immediately colliding on buf_mutex.
**	  o Bump priority of fixed READNOLOCK buffers when found in cache
**	    the same as non-READNOLOCK buffers to keep them from getting
**	    tossed prematurely.
**	  o In cp_flush(), skip I/O-busy buffers instead of waiting
**	    for the I/O to complete as they can be considered processed
**	    for the current CP.
**	30-may-2001 (devjo01)
**	    Add lgclustr.h for LGcluster() macro. s103715.
**      12-Jun-2001 (horda03)
**          In the port for 1.2 to 2.0 something has been lost to the change
**          I made on 29-Aug-1997. But anyhow, things have moved on, and now
**          VMS has the missing fields within the CL_ERR_DESC structure.
**          The temporary VMS fix can now be removed, which fixes Bug 104932
**      02-Aug-2001 (stial01)
**          dm0p_unfix_page() fix etab LEAF page lock conversion (b105412)
**	16-aug-2001 (toumi01)
**	    NO_OPTIM for i64_aix because of SEGV doing "createdb -S iidbdb"
**	11-jun-2002 (devjo01)
**	    In gfault_page add more logic to prevent futile gfault attempts,
**	    especially when using internal threads. (b107914, b104299).
**	28-Feb-2002 (jenjo02)
**	    Removed LK_MULTITHREAD from LKrequest/release flags. 
**	    MULTITHREAD is now an attribute of lbm_lock_list and
**	    svcb_lock_list.
**	06-Jun-2002 (jenjo02)
**	    Check that bm_flimit (free buffer limit) does not exceed
**	    90% of cache; if so, silently reduce it to that value.
** 	    Test of flimit in fault_page() was reversed, counting
**	    free pages when WB threads were running instead of when
**	    they were not. 
**      12-jun-2002 (stial01)
**          dm0p_uncache_fix() Make sure we update cache lock value for all
**          special page types and tables that use physical page locks (b107946)
**      19-Jun-2002 (horda03) Bug 108074
**          Prevent FORCE_ABORT transaction filling TX log
**          file by not flushing SCONCUR pages unless we
**          really need to (i.e. the table isn't locked
**          exclusively).
**	16-Jul-2002 (jenjo02)
**	    Incomplete CP errors can occur in multiple server installations
**	    if one server's CP thread falls behind the cpindex.
**	20-aug-2002 (devjo01) b108551
**	    Call force_page with new evcomp value BMCL_DMCM_CBACK from
**	    dm0p_dmcm_callback in order to turn off the BUF_MUTEX & 
**	    BUF_MUTEX_OK bits dm0p_dmcm_callback asserted.
**      08-Oct-2002 (hanch04)
**          Replace the i4 for length with a SIZE_TYPE (size_t) to allow
**          64-bit memory allocation.
**      08-Oct-2002 (hanch04)
**          Removed dm0m_lcopy,dm0m_lfill.  Use MEcopy, MEfill instead.
**	20-Aug-2002 (jenjo02)
**	    Support TIMEOUT=NOWAIT, new LK_UBUSY status from LKrequest().
**	25-oct-2002 (devjo01)
**	    Prevent infinite CPU loop when checking for pages already in cache.
**	    b109051.
**	15-Nov-2002 (jenjo02)
**	    Renamed "bm_wb_mutex" to "bm_mutex". Cleared up confusion 
**	    re protection of "bm_status" via -both- bmcb_status_mutex
**	    and bm_wb_mutex; now just "bm_mutex" does the job
**	    (BUG 109131).
**	13-jun-2002 (thaju02)
**	    For readlock nolock, specials page types (btree root/index, fhdr)
**	    that are fixed with the DM0P_LOCKREAD action, are to be locked
**	    but copied into readnolock buffers rather than into cache, to
**	    prevent E_DM9C01, E_DM9301. (B95771)
**	10-Jan-2003 (hanch04)
**	    Back out last change until we get a fix to work with new code.
**      17-Mar-2003 (horda03) Bug 108954 - BACKED OUT 13-Oct-2005 (hanje04)
**          In gfault_page(), if tbio id null, don't try to write the page to
**          disk.
**	18-Mar-2003 (jenjo02)
**	    Cleaned up buf_mutex state after force_page(), complete_f_p()
**	    which was inconsistent, sometimes returning with mutex
**	    locked or unlocked unexpectedly.
**	    If non-zero status or BUF_RECLAIM, returns with mutex,
**	    otherwise no mutex. (B109869, INGSRV2140).
**      28-may-2003 (stial01)
**          dm0p_trans_page_lock() if DM0P_NOWAIT, pass flag to dm0p_escalate
**          Renamed LK_ESCALATE -> LK_IGNORE_LOCK_LIMIT, used when escalating
**          AND lock requests made inside critical sections of code.
**	18-Aug-2003 (devjo01)
**	    In order to free a bit for BUF_WUNFIX_REQ in crossing fix for
**	    bug 108074 to main, removed sets of BUF_TEMP_FIXED.  This
**	    flag was never being tested, and no longer served any purpose.
**      15-sep-2003 (stial01)
**          Skip PHYS page lock/unlock for etabs if table locking (b110923)
**      18-feb-2004 (stial01)
**          Fixed incorrect casting of length arguments.
**	01-apr-2004 (devjo01)
**	    Only use CXmsg_redeem if DMCM & configured with a fast
**	    cluster long message facility (CX_HAS_FAST_CLM).
**      12-apr-2004 (stial01)
**          dm0p_uncache_fix - clear BUF_NOTXLOCK when unfixing buffer
**	01-Jul-2004 (jenjo02)
**	    When updated SEQUENCE rows are unfixed, they must be
**	    written (DM_B_SEQ_TAB_ID).
**      08-jul-2004 (stial01)
**          Additional readnolock buffer when DMPP_VPT_PAGE_HAS_SEGMENTS
**	13-oct-2004 (devjo01)
**	    Tighten check to suppress gather write when flushing page for
**	    DMCM protocols.  Change version stamp used to check when a
**	    page needs refreshing from a simple counter to instead hold
**	    the expected page LSN, and use this as a cross-check that
**	    correct page is read in.
**	02-dec-2004 (thaju02)
**	    In rnl_fixpage(), TRdisplay only if trace point DM513 set.
**	    Change %x to %p in TRdisplay fmt.
**      07-jan-2005 (stial01)
**          DM0P_CREATE_NOFMT: Put the page number onto the page (b113708)
**	17-dec-2004 (devjo01)
**	    Correct LSN version stamp logic added on 13-oct to correctly
**	    handle group writes.
**	05-jan-2005 (devjo01)
**	    In validate_page Refresh LVB when page refreshed from disk
**	    to avoid spurious E_DM9C91_BM_LSN_MISMATCH errors.
**      07-jan-2005 (stial01)
**          DM0P_CREATE_NOFMT: Put the page number onto the page (b113708)
**	12-Jan-2005 (hweho01)
**	    Rmoved rs4_us5 from NO_OPTIM list.
**      31-jan-05 (stial01)
**          Added flags arg to dm0p_mutex call(s).
**          Redo fix for b113708, DM0P_CREATE_NOFMT, don't put pageno on page.
**          dm0p_mutex, dm0p_unmutex new flag to bypass pageno consistency check
**          invalidate_page - clobber page tranid as well as page number.
**      02-feb-05 (stial01)
**          dm0p_cachefix_page() DM0P_CREATE_NOFMT, don't put pageno on page
**      14-mar-05 (stial01)
**          dm0p_uncache_fix() new flag DM0P_CREATE_NOFMT (b114112)
**	21-Apr-2005 (schka24)
**	    Fix infinite loop in cp_sort.  Sort loops needed better protection
**	    because of doing dirty reads.
**	27-Apr-2005 (schka24/jenjo02)
**	    Fix nasty bug whereby free group was grabbed before the freeing
**	    thread was quite done, leading to update confusion on buf_sts and
**	    ultimately an unmutex error crash.
**      18-Nov-2005 (horda03) Bug 115542/INGSRV3511
**          Only change the buf_hash_bucket field once the session has the hash_mutex
**          and so can modify the hash link list. Chaning it before the hash mutex is
**          held can cause a spin.
**      09-jun-05 (stial01)
**          dm0p_dmcm_callback() Return if cache lock is LK_N (b114660)
**	25-jul-2005 (abbjo03)
**	    In dm0p_cachefix_page(), control the number of times we try
**	    searching the chain of hash buckets.
**      08-aug-2005 (stial01)
**          DO use DMCM protocols for blob etabs (tbio_extended) (b115011)
**	17-oct-2005 (devjo01) b114660
**	    - Under highly concurrent cirucumstances in a cluster, it is
**	    possible for a stale callback to see a BUF_RBUSY set by
**	    dm0p_validate.  This would stall the callback thread, and in
**	    the worst case cause a scheduling deadlock where the holder
**	    of the page cache lock on the other node could not release
**	    the fatal page (typically a map page) because a blocking
**	    notification to the stalled callback thread could not be
**	    processed.
**	    - Added LK_CALLBACK_TRACE instances to held debug above.
**	28-nov-2005 (devjo01) b115583
**	    Restore tracking of which process allocated buffer page cache
**	    locks to accomidate deficiencies in the VMS DLM.  Since under
**	    VMS, locks must be operated on by the process which allocated
**	    them, we MUST toss all pages covered by a lock allocated by
**	    a DBMS being shutdown.  Note, this also would apply to a crashing
**	    DBMS, but as all DBMS processes attached to a shared cache are
**	    already obliged to abend, if one of them should crash, this is
**	    moot.
**	30-nov-2005 (abbjo03)
**	    Remove globavalue SS$_DEBUG declaration which causes unexplained
**	    compilation errors.
**	18-jan-2006 (abbjo03)
**	    Fix 30-nov-2005 change.  Need to include VMS headers (indirectly
**	    included by pthread.h).
**      08-aug-2006 (horda03) Bug 114498/INGSRV3299
**          New flag for TCB status - also now use the macro
**          TCB_FLAG_MEANING rather than putting the values here.
**      22-sep-2006 (horda03) Bug 56705
**          If a table is being Materialized, then don't try and force any pages. This
**          could cause a Semaphore deadlock on the FCB mutex.
**	01-Dec-2006 (kiria01) b117225
**	    Initialise the lockid parameter to LKrequest avoid random implicit
**	    lock conversions.
**	06-Mar-2007 (jonj)
**	    Add RedoLSN param to dm0p_cachefix_page and fault_page, supplied
**	    during online Cluster REDO page fixing.  Translate DM0P_NOLKSTALL
**	    action to LK_NOSTALL for cache locks.
**/


/* 
** External functions
*/

/*
**  Defines of other constants.
*/

/*
**	Constants to define the semaphore lock mode to dm0p_mlock():
*/
#define			SEM_SHARE	0
#define			SEM_EXCL	1

/*
**      These constants are used as action values for force_page().
**      (ICL phil.p)
**      Introduced FP_NULL for the case where a cache page is to be
**      retained in the cache with a NULL lock. This is used
**      exclusively by the DMCM protocol.
*/

#define			FP_CACHE        0x01
#define			FP_WRITE	0x02
#define			FP_TOSS		0x04
#define			FP_INVALID	0x08
#define			FP_DISOWN	0x10
#define			FP_WRITEALONG	0x20	/* like FP_WRITE, but raise  */
						/* pages priority. 	     */
#define                 FP_NULL         0x40

/*
**  These constants are used by find_page() and fault_page() to return
**  the next expected action from the caller.
*/

#define			RETRY	    1
#define			RETURN	    2
#define			ERROR	    3
#define			FAULT	    4

/*
** Value put in Buffer Manager Lock value to indicate that the buffer manager
** has been initialized.
*/
#define			BM_INIT_VALUE	    1

/* 
** Total possible number of lines of output allowed when using trace 
** point dm415.
*/	
#define	  		MAX_DM415  	    1000

/* Number of queue search restarts allowed when doing dirty queue
** searches.  If more than this many restarts, just take the relevant
** mutex instead of trying to be cute.
*/		
#define		MAX_RESTARTS 50 

/*
** Macro for determining if LK_LOCAL s/b set for a SV_PAGE lock.
**
** db_tab_base portion of test must match "user table" test in
** dm2d_release_user_tcbs.
*/
#define	DM0P_LOCK_SCOPE(tbio) \
    ((((tbio)->tbio_cache_flags & TBIO_CACHE_MULTIPLE) || \
      ((tbio)->tbio_reltid.db_tab_base <= DM_B_INDEX_TAB_ID)) ? 0 : LK_LOCAL)

/*
**  Definition of static variables and forward static functions.
*/
static DB_STATUS bm_shrbuf(
char                *cache_name,
SIZE_TYPE             segsize,
i4                 segment_ix,
i4             totbufcnt,
i4             flags,
i4                  shrbuf_init,
i4                  shrbuf_connect,
DMP_BMSCB           **bmsegpp,
i4             *err_code);

static DB_STATUS bm_check_shrseg(
	i4	segment_ix,
	DMP_BMSCB *bmsegp,
	char	*cache_name,
	i4	*err_code);

static DB_STATUS    fault_page(
		DM0P_BMCB	        *bm,
		DMP_TABLE_IO		*tbio,
		i4                 action,
		i4			page_number,
		i4			bucket,
		i4			lock_list,
		i4			access_mode,
		i4			log_id,
		DB_TRAN_ID		*tran_id,
		VOID			(*format_routine)(
					    i4		page_type,
					    i4		page_size,
					    DMPP_PAGE       *page,
					    DM_PAGENO       pageno,
					    i4         stat,
					    i4		fill_option),
		DM0P_BUFFER		**buffer,
		LG_LSN			*RedoLSN,
		i4			*err_code);

static  DB_STATUS   gfault_page(
		DM0P_BMCB	        *bm,
		DMP_TABLE_IO		*tbio,
		i4			page_number,
		i4			bucket,
		i4			lock_list,
		i4			access_mode,
		i4			log_id,
		DB_TRAN_ID		*tran_id,
		DM0P_BUFFER		**buffer,
                i4                 action,
		VOID			(*format_routine)(
					    i4		page_type,
					    i4		page_size,
					    DMPP_PAGE       *page,
					    DM_PAGENO       pageno,
					    i4         stat,
					    i4		fill_option),
		i4			*err_code);

static  DB_STATUS   force_page(
		DM0P_BUFFER         *buffer,
		DMP_TABLE_IO	    *tbio,
		i4		    operation,
		i4		    lock_list,
		i4		    log_id,
		i4		    evcomp,
		i4		    *gw_queued,
		i4             *err_code);

static  DB_STATUS   gforce_page(
		DM0P_BMCB	    *bm,
		DM0P_GROUP	    *group,
		i4		    lock_list,
		i4		    log_id,
		i4		    operation,
		i4		    evcomp,
		i4		    *gw_queued,
		i4             *err_code);

static  DB_STATUS   validate_page(
		DMP_TABLE_IO		*tbio,
		DM0P_BUFFER		*buffer,
		i4			action,
		i4			lock_list,
		i4			log_id,
                i4                 access_mode,
		i4			*err_code);

static  DB_STATUS   dm0p_reclaim(
		i4	    		lock_list,
		i4			log_id);

static  DB_STATUS   dm0p_eset(
		i4	    lock_list,
		u_i4   event_type,
		i4	    cache_ix,
		u_i4   event_object,
		i4	    return_on_error);

static  DB_STATUS   dm0p_ewait(
		i4	    lock_list,
		u_i4   event_type,
		i4	    cache_ix,
		u_i4   event_object,
		i4	    return_on_error,
		i4	    flags);

static  DB_STATUS   dm0p_erelease(
		i4	    lock_list,
		u_i4   event_type,
		i4	    cache_ix,
		u_i4   event_object,
		i4	    return_on_error);

static DB_STATUS dm0p_minit(
		DM0P_SEMAPHORE      *sem,    
		char		    *sem_name,
		i4		    sem_type);
static i4       dm0p_mlock_fcn(
		i4 		    mode, 
		DM0P_SEMAPHORE      *sem, 
		i4		    line);
 
static i4       dm0p_munlock_fcn(
		DM0P_SEMAPHORE 	    *sem,
		i4		    line);

static VOID	    move_group(
		DM0P_BMCB	*bm,
		DM0P_GROUP	*group,
		DM0P_BUFFER	*b,
		i4		lock_list,
		i4		rdelta,
		i4		mdelta,
		i4		vdelta);

static  VOID	    invalidate_page(
		DM0P_BUFFER	*buffer,
		i4		lock_list);

static  VOID	    reclaim_bmc(
		DM0P_STATE_HEAD	*bmc_list,
		DM0P_BMCACHE	*cache_array,
		DM0P_BMCACHE	**bmc_entry);

static VOID	dbg_dm9301(
		DM0P_BUFFER	    *b,
		DMP_TABLE_IO	    *tbio,
		i4		    action,
		DB_TRAN_ID          *tran_id,
		i4		    lock_list,
		i4		    log_id,
		DM0P_BLQ_HEAD	    *tran_bucket,
		DM0P_BID	    *tran_chain,
		i4		    num_bufs,
		i4		    num_forces,
		i4		    num_group_forces,
		i4		    num_waits,
		i4		    num_other_bufs,
		i4		    num_force_warns,
		i4		    num_removes,
		DB_STATUS	    sts);

static DB_STATUS    fix_tableio_ptr(
		DM0P_BUFFER	*b,
		i4		flags,
		i4		lock_list,
		i4		log_id,
		DMP_TABLE_IO	**tableio_ptr,
		i4		*err_code);

static VOID	    unfix_tableio_ptr(
		DMP_TABLE_IO	**tableio_ptr,
		i4		lock_list,
		i4		log_id,
		i4		caller_id);

static VOID dm0p_log_error( 
		i4		new_err_code,
		i4		*old_err_code, 
    		i4 	do_log);


static DM0P_BUFFER *first_fixed_syscat_page( DB_TRAN_ID *tran_id);


static DB_STATUS dm0p_check_consistency(i4 id);
					/* Check buffer manager state. */

static VOID	    dbg_dm429( 
		int 		*count_ptr,
		i4		lock_list,
		DB_TRAN_ID      *tran_id,
		i4		log_id,
		i4		action,
		i4	    	num_bufs,
		i4	    	num_other_bufs,
		DM0P_BLQ_HEAD 	*tran_bucket ); /* debugging routine for dm429*/

static bool     dm0p_no_lgforce(
		DM0P_BMCB	    *bm,
		DM0P_GROUP          *group);    /* see if may need log force */

static VOID     tabq_insert(
		DM0P_BUFFER     *b);

static VOID     tabq_delete(
		DM0P_BUFFER     *b);

static VOID     txnq_insert(
		DM0P_BUFFER     *b,
		i4		log_id);

static VOID     txnq_delete(
		DM0P_BUFFER     *b);

static DB_STATUS dmcm_cache_flush(
                     i4         *err_code);

static VOID 	dm0p_complete(
		DB_STATUS dm2f_status,
		DM0P_CLOSURE *bmcl,
		i4 *err_code);

static STATUS 	    complete_f_p(
		STATUS pstatus,
		DMP_TABLE_IO *tbio,
		DM0P_BUFFER *b,
                i4 pages,
		i4 operation,
		i4 log_id,
		i4 lock_list,
		i4 evcomp,
		i4 *err_code);

static VOID	    wb_cycle_start(
		DM0P_BMCB	*bm,
		i4 	lock_list,
		i4	why,
		char  	*where);

static VOID	    wb_cycle_end(
		DM0P_CFA	*cfa);

static STATUS	    CloneAgent(
		DM0P_CFA	*cfa,
		i4		task,
		char		*where);

static int 	check_buffer( LK_LKID *lockid, LK_LOCK_KEY *lock_key);

static DB_STATUS	 cp_prep(
		i4	lock_list,
		i4	log_id,
		i4	*err_code);

static VOID	 cp_sort(  
		DM0P_BUFFER	*buf_array,
		i4      	cp_array[], 
		i4		lo,
		i4		hi);

static DB_STATUS    cp_flush(
		DM0P_CFA	*cfa,
		i4	cache_ix,
		i4	lock_list,
		i4	log_id,
		i4	*flush_count,
		i4	*err_code);

static DM_PGCHKSUM    dm0p_compute_checksum(
		DMPP_PAGE 	*page, 
		DM_PAGESIZE	page_size);

static DM_PGCHKSUM    dm0p_compute_old_checksum(
		DMPP_PAGE 	*page, 
		DM_PAGESIZE	page_size);

static VOID	awaken_buf_waiters(
			DM0P_BUFFER	*b,
			i4		lock_list);

static VOID	CfaTrace(
			DM0P_CFA	    *cfa,
			char		    *what);
		
static VOID hashq_delete(DM0P_BUFFER *b,
		DM0P_HASH_HEAD *bucket_array,
		DM0P_BUFFER *buf_array);

static bool dm0p_check_logfull(VOID);



/*{
** Name: dm0p_allocate	- Allocate and initialize the buffer manager.
**
** Description:
**
**	This routine allocates and initializes the Buffer Manager data
**	structures and control blocks.
**
**	There are two control blocks used by the Buffer Manager: 
**
**          BMSCB:  Buffer Manager Segment Control Block
**          LBMCB:  Local Buffer Manager Control Block
**
**      The BMSCB is the control block at the beginning of each segment 
**      of buffer manager memory. Note that multiple segments of memory
**      are allocated when the installation is configured with multiple
**      buffer caches that are to be allocated separately or if segment
**	overflow occurs. The BMSCB contains the offsets to interesting 
**	sections of buffer manager memory allocated contiguous with the BMSCB.
**
**      The LBMCB contains pointers to the buffer manager data arrays.    
**      Each server must keep its own set of pointers since the BMSCB(s)
**      may be positioned at a different address(es) in different servers.
**      In a shared buffer manager environment, the BMSCB(s) are shared
**      by multiple servers, while each server has its own LBMCB.
**
**	In addition to these control blocks, this routine allocates:
**            Buffer Manager common control block  
**            Database and Table cache arrays
**
**      The following are also allocated for EACH BUFFER POOL:
**            Buffer manager control block 
**            Buffer descriptors
**            Group descriptors
**            Hash buckets
**            Data Pages
**	      GatherWrite structures, if GW enabled
**	      Consistency Point array
**
**	When the Buffer Manager is positioned in shared memory, everything
**	but the LBMCB is allocated out of a shared memory segment(s).
**      The buffer manager contains lists of buffer descriptors.
**      These lists are formed by index values rather than pointers,
**      as no pointers may be stored in the shared memory data structures.
**      The first server to start up will create the shared segment(s)
**      and initialize the data structures. Subsequent servers will merely 
**      allocate an LBMCB and set up pointers to the shared data structures.
**  
**      The buffer cache contains multiple buffer pools. A separate buffer 
**      pool is maintained for each page size.  
**
**      The size of the Buffer Cache is determined mainly by the number of
**      pages allocated for each buffer pool. The buffer[pool] and group[pool] 
**      parameters specify the number of Ingres pages stored in that pool:
**
**        # pages[pool] = buffers[pool] + (group[pool] * gpages[pool]))
**
**	There is one buffer descriptor allocated for each Ingres page (even 
**	pages that are used in group operations). In addition, each set of pages
**	used for group operations has a group descriptor.  The buffer
**	descriptors take about 150 bytes each while the group ones require
**	about 50 bytes.  There is also a (small) hash bucket header for
**	finding pages in the cache;  one bucket per page, rounded up to
**	the next even number.
**
**      Also contributing to the size of the Buffer Manager (but less
**      significantly) are the database and table caches.
**	The number of database and table cache entries is given by the
**	'dbc_size' and 'tbc_size' parameters.  These specify the number of
**	databases/tables that can remain cached even while closed.  Each
**	cache entry requires about 70 bytes.
**
**      The 'group[pool]' parameter specifies the number of group buffers
**      to be allocated for that pool. The group buffers are used for 
**      multi-page I/O's - which are used for readahead. If 'group[pool]' 
**      is zero, no readahead will be performed for that pool.
**	The 'gpages[pool]' parameter gives the number of pages in each 
**      group operation for that pool. 
**
**      The 'free_limit[pool]' parameter specifies the number of buffers 
**      which must always be available on the free list for that pool.
**      (not fixed or modified). When this limit is reached, all new 
**      requests for buffers in that pool must toss a page off the modified  
**	list rather than use one from the free list.
**
**      The 'modify_limit[pool]' parameter specifies the maximum number
**      of pages that can be left in the cache for that pool. When this
**      limit is reached, all dirty pages in that pool are written as soon  
**      as they are unfixed.
**
**      The 'wb_start[pool]' and 'wb_end[pool]' parameters specify the 
**      window used for that pool by the write behind threads (if any are
**      allocated).  This is a window on the number of modified pages
**      in that pool.  When there are at least wb_start modified pages, 
**	the write behind threads begin to write modified pages from the pool, 
**	transferring them to the free queue. When the count goes below
**	wb_end, the writing stops.  The write behind threads are started up 
**	by SCF at server initialization.
**
**	The 'cache_name' parameter specifies the name of the buffer cache if
**	a shared memory buffer manager is specified.
**
**	Some of the Buffer Manager objects that are initialized here are:
**
**	  Buffer Manager Lock - If a shared memory buffer manager is
**			specified, a lock on the specified cache will be
**			requested.  This lock will tell us whether this server
**			is the first connector - in which case the memory
**			segment should be created and initialized, or if there
**			is already a server connected - in which case we should
**			just connect to that segment.  Holding this lock will
**			tell other servers that the buffer manager has been
**			initialized.
**
**	  Buffer Manager Lock List - This is the lock list on which all
**			cache locks are held.  It is a shared lock list that
**			can be referenced by all servers using this same buffer
**			manager.  The lock list is created by the server which
**			initializes the buffer manager - servers just
**			connecting to an already existing shared buffer manager
**			just attach to the lock list.
**
**	  Buffer Manager Id - The buffer manager id is a unique identifier for
**			this buffer manager.  It is set using the value of the
**			buffer manager lock list.
**
**	  Server count - The count of servers connected to this buffer manager
**			is incremented.
**
**	  Buffer Descriptors - Each single buffer descriptor is initialized
**			and put on the priority zero free list.
**			Each buffer descriptor for group buffers is
**			initialized but is not put on any list.
**
**	  Group Descriptors - Each group descriptor is initialized and put on
**			the group free queue.
**
**	  Modify/Fixed queues - The fixed and modify queues (single and group)
**			are initialized empty.
**
**	  Database cache array - The database cache array is initialized.  The
**			entries are linked together into one list with each
**			entry being marked free.
**
**	  Table cache array - The table cache array is initialized.  The
**			entries are linked together into one list with each
**			entry being marked free.
**
** Inputs:
**	flags				Allocate flags:
**					    DM0P_SHARED_BUFMGR	- allocate
**						buffer manager in shared memory.
**					    DM0P_MASTER - create the buffer
**						manager lock list as a master
**						lock list.
**                                          DM0P_DMCM - create the buffer
**                                               manager DMCM lock list.
**	connect_only			with shared buffer manager request:
**					ask to connect to existing shared
**					buffer manager only, don't create one.
**	buffers[DM_MAX_CACHE]           The number of buffers to allocate for 
**                                      each pool.
**	free_limit[DM_MAX_CACHE]        The free page cache minimum limit for
**                                      each pool.
**	modify_limit[DM_MAX_CACHE]      The modified page cache maximum limit
**                                      for each pool.
**	group[DM_MAX_CACHE]             The number of groups to allocate for 
**                                      each pool.
**	gpages[DM_MAX_CACHE]            The number of pages in a group for  
**                                      each pool.
**	wb_start[DM_MAX_CACHE]          Number of modified pages at which to
**					start asynchronous writes for each pool.
**	wb_end[DM_MAX_CACHE]            Number of modified pages at which to
**					stop asynchronous writes for each pool.
**      cache_flags[DM_MAX_CACHE]       Flags per cache:
**                                         DM0P_NEWSEG:
**                                            Alloc cache for this page size
**                                            in its own memory segment.
**                                         DM0P_DEFER_PGALLOC:
**                                            Defer allocation of page array
**	dbc_size			Number of database cache locks to
**					maintain in the db cache list.
**	tbc_size			Number of table cache locks to maintain
**					in the table cache list.
**	cache_name			If shared memory buffer manager then
**					this indicates the cache to use.
**
** Outputs:
**	err_code			Reason for the error return status.
**					    E_DM9239_BM_BAD_PARAMETER
**					    E_DM0123_BAD_CACHE_NAME
**					    E_DM930A_DM0P_ALLOCATE
**					    E_DM9372_BM_CANT_CREATE
**	Returns:
**	    E_DB_OK
**	    E_DB_FATAL
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	15-oct-1985 (derek)
**          Created new for Jupiter.
**	30-sep-1988 (rogerk)
**	    Added wb_start and wb_end parameters.  These specify asychronous
**	    write behind window.
**	30-jan-1989 (rogerk)
**	    Added support for shared memory buffer manager.  Made buffer manager
**	    position independent and broke into BMCB/LBMCB.  Added database
**	    and table caches, BM lock, BM lock list, BM id and server count.
**	    Made buffer manager semaphore be cross-process semaphore.
**	    Added flags, tbc_size, dbc_size and cache_name parameters.
**	10-apr-1989 (rogerk)
**	    Added bm_name field to hold buffer manager name.
**	    Fill in the control block header information when allocate
**	    a shared memory buffer manager.
**	15-may-1989 (rogerk)
**	    Changed to return resource errors rather than internal dm0p
**	    error if memory or lock resource limits reached.
**	    Updated MEget_pages call to reflect CL approved interface.
**	 7-jun-1989 (rogerk)
**	    Added new error messages for more informative errors when allocating
**	    shared buffer manager.
**	19-jun-1989 (rogerk)
**	    Lower case the cache name.
**	 2-oct-1989 (rogerk)
**	    Changed to use normal semaphore rather than cross-process semaphore
**	    when not using shared buffer manager.  On some systems, cross
**	    process semaphores cannot be used on non-shared memory.
**	6-Oct-1989 (ac)
**	    Added DM0P_MASTER to create the buffer manager lock list as a
**	    master lock list.
**	1-Feb-1990 (ac)
**	    Added make the bm_lock_list noninterruptable so that the lock
**	    wait on the lock list will not be cancelled.
**      11-sep-1990 (jonb)  
**	    Integrated ingres6301 change 280136 (rogerk):
**            Initialize the buf_tcb_stamp field.
**	 4-feb-1991 (rogerk)
**	    Added support for fixed buffer priorities.
**	    Added table priority cache to remember user-set table priorities.
**		(this is needed since table priorities are not currently
**		set in the system catalogs.)
**      16-dec-1991 (bryanp)
**          Add ME_IO_MASK to MEget_pages call for shared buffer cache. This
**          indicates to DI that it is advantageous for the slaves to map this
**          memory (that way, they can perform I/O directly w/o copying).
**	19-feb-1992 (bryanp)
**	    Fix error message formatting problem in dm0p_allocate(). We don't
**	    want to pass the sys_err to ule_format when dm0m_allocate fails
**	    because the sys_err doesn't have anything useful in it.
**	07-apr-1992 (jnash)
**	    Add support for locking a shared DMF cache in dm0p_allocate.
**	28-sep-92 (kwatts)
**	    Add call of CSn_semaphore to give name to the shared cache
**	    semaphore.
**	26-oct-92 (rogerk)
**	    Reduced Logging Project:
**	      - Added buffer manager transaction context to be used by
**		force_page to force the log file before writing pages.
**	      - Added buffer manager semaphore to protect usage of the
**		above transaction context.
**	      - Took sys_err argument out of error msg calls when CSi_semaphore
**		returns an error since there is no sys_err value.
**	23-Oct-1992 (daveb)
**	    Attach shared semaphore for monitoring, and name the local
**	    cache semaphore too.  Note we aren't detaching the shared
**	    sem yet; not sure we need to or where the right place is.
**	26-apr-1993 (bryanp/keving)
**	    Added have_log argument. If this is set true then we have
**		previously allocated our local log. Otherwise logs will be 
**		allocated later and registered via dm0p_register_loghandles. 
**		Used by CSP to handle multi-logs case. 
**	18-oct-1993 (rmuth)
**	    Initialise buffer Manager mananged objects and add DMP_BMCB 
**	    statistics.
**	24-jan-1994 (gmanning)
**	    Give name to semaphores that are created as CS_SEM_MULTI in order 
**	    to take advantage of cross process semaphores on NT.
**	09-nov-93 (swm)
**	    Bug #58633
**	    Corrected memory_needed alignment calculations for buffer manager
**	    memory, in dm0p_allocate(), to be consistent with subsequent
**	    pointer setup to the memory. Corrected buffer manager dbcache
**	    pointer setup.
**      17-May-1994 (daveb) 59127
**          Fixed semaphore leaks, named sems correctly.  Delete
**  	    redundant CSn_semaphore call.
**	25-Apr-1995 (jenjo02)
**	    Combined CSi|n_semaphore functions calls into single
**	    CSw_semaphore call.
**	25-mar-96 (nick)
**	    Added lbm_fcwait initialisation.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**      01-aug-1996 (nanpr01 for ICL phil.p)
**          Create lbmcb->lbm_dmcm_lock_list for use by DMCM protocol.
**	    Added the gather write and async io amendments.
**      10-aug-1998(hweho01)
**          Semaphore "BM lx_id" is not defined in the shared memory, its 
**          type should be CS_SEM_SINGLE.
**	19-May-1998 (jenjo02)
**	    Dynamic, per-cache  WriteBehind threads.
**	02-dec-1998 (nanpr01)
**	    Making sure mlock works for private cache as well.
**	12-Jul-1999 (jenjo02)
**	    Allocate array of BIDs, one per buffer per cache, to be used
**	    to pre-sort the candidate Consistency Point buffers.
**	30-Aug-1999 (jenjo02)
**	    Make the 2k cache an option, not a requirement.
**	20-Apr-2000 (jenjo02)
**	    Monitor segment overflow; if it occurs, allocate cache
**	    in a new segment. Allocate CP array for each cache rather
** 	    than for all caches.
**	06-Jun-2002 (jenjo02)
**	    Check that bm_flimit (free buffer limit) does not exceed
**	    90% of cache; if so, silently reduce it to that value.
**	01-apr-2004 (devjo01)
**	    Only use CXmsg_redeem if DMCM & configured with a fast
**	    cluster long message facility (CX_HAS_FAST_CLM).
**      29-Sep-2004 (fanch01)
**          Add LK_LOCAL flag support as buffer names are confined to one node.
**	3-Jan-2006 (kschendel)
**	    Add connect-only flag for a caller (e.g. fastload) to connect
**	    to an existing shared cache (but never create one).  Rework
**	    connect code so that connects simply take all the parameters
**	    of the existing cache;  the old cross-check code was an artifact
**	    of the pre-config.dat configuration setup and was pointless.
**	10-May-2006 (jenjo02)
**	    Fix calculation of rounding slop needed for page-aligned
**	    lbm_pages.
**	15-Jun-2006 (kschendel)
**	    Above fix prevented overlaps, but didn't fix the underlying
**	    problem:  namely, that we can't force the CL (or the OS, even)
**	    to map the shared cache on a 64K boundary.  If one server
**	    maps a segment at address X+0 and another at X+8K, there is
**	    *no* position in the segment that is at a 64K boundary in
**	    both segments.  What happens is that the two servers think
**	    that the page pool starts at different positions -- not good!
**	    Give up on pagesize alignment, and align the page pool at
**	    ME_MPAGESIZE boundaries instead.  Hopefully that symbol is
**	    properly defined to be the shared-attach alignment (at least).
**	26-Jun-2006 (kschendel)
**	    It isn't.  Until we get this fixed at the CL level, hardcode
**	    4096 as the page alignment.  Too-small shouldn't hurt anything
**	    other than possibly messing up direct IO, but too-large is deadly.
**	20-Jun-2007 (bonro01)
**	    Initialize local pointer array for code path where
**	    shrbuf_connect=TRUE
**	    This will prevent uninitialized values from being saved in the
**	    shared memory segment and prevent abends during dm0p_deallocate()
**	    when dmf_svcb->svcb_bmseg[] pointers are referenced.
**	    This problem was introduced by change 480471.
*/

#if ME_MPAGESIZE > 4096
#define FIXME_PAGESIZE 4096
#else
#define FIXME_PAGESIZE ME_MPAGESIZE
#endif

DB_STATUS
dm0p_allocate(
i4		    flags,
bool		    connect_only,
i4             	    buffers[DM_MAX_CACHE],
i4		    free_limit[DM_MAX_CACHE],
i4		    modify_limit[DM_MAX_CACHE],
i4             	    group[DM_MAX_CACHE],
i4             	    gpages[DM_MAX_CACHE],
i4		    writebehind[DM_MAX_CACHE],
i4		    wb_start[DM_MAX_CACHE],
i4		    wb_end[DM_MAX_CACHE],
i4             	    cache_flags[DM_MAX_CACHE],
i4		    dbc_size,
i4		    tbc_size,
char		    *cache_name,
i4             	    *err_code)
{
    CL_SYS_ERR          sys_err, loc_sys_err;
    LK_LOCK_KEY		lockkey;
    LK_LKID		bm_lockid;
    LK_VALUE		lock_value;
    LK_UNIQUE		*lock_list_key;
    DB_TRAN_ID		tran_id;
    DM0L_ADDDB		add_info;
    i4			len_add_info;
    STATUS		cl_status;
    DM0P_HASH_HEAD	*bmh;
    DM0P_BUFFER		*buffer;
    DM0P_GROUP		*groups;
    DM0P_BM_COMMON      *bm_common = NULL;
    DMP_LBMCB		*lbmcb = NULL;
    LK_LLID		lock_list = dmf_svcb->svcb_lock_list;
    LK_LLID		bm_lock_list = (LK_LLID)0;
    LG_LXID		bm_lxid = 0;
    LG_DBID		bm_dbid = 0;
    DB_STATUS		status = E_DB_OK;
    i4			shrbuf_init = FALSE;
    i4			shrbuf_connect = FALSE;
    i4			count_incremented = FALSE;
    DM0P_BMCB           *bm;
    i4			rpages[DM_MAX_CACHE];
    i4			bcnt[DM_MAX_CACHE];
    DM0P_BMCB           *bmcb_ptrs[DM_MAX_CACHE];
    i4			next_buf;
    i4			next_group;
    i4			flags_arg;
    i4			local_error;
    i4			i,j,k;
    SIZE_TYPE         	pgsize;
    i4                  cache_ix;
    DMP_BMSCB           tmp_bmseg[DM_MAX_BUF_SEGMENT];
    DMP_BMSCB           *bmsegs[DM_MAX_BUF_SEGMENT];
    DMP_BMSCB           *bmsegp;
    i4                  segment_ix;
    i4			segcnt;
    char		sem_name[CS_SEM_NAME_LEN];
    i4			sem_type;
    i4			gw_dicb_size;

    SIZE_TYPE		cache_size[DM_MAX_CACHE];
    SIZE_TYPE		pool_size[DM_MAX_CACHE];
    SIZE_TYPE		seg_caches[DM_MAX_BUF_SEGMENT];
    i4			max_pages_per_cache[DM_MAX_CACHE];
    i4			excess_pages;

    if (connect_only && (flags & DM0P_SHARED_BUFMGR) == 0)
    {
	*err_code = E_DM9239_BM_BAD_PARAMETER;
	return(E_DB_ERROR);
    }

    for (cache_ix = 0; cache_ix < DM_MAX_CACHE; cache_ix++)
	bmcb_ptrs[cache_ix] = NULL;

    /*
    ** Allocate the local buffer manager control block from the DMF
    ** memory manager.  No point in proceeding if this doesn't work.
    */
    status = dm0m_allocate(sizeof(DMP_LBMCB), (i4)0, (i4)LBMCB_CB,
	(i4)LBMCB_ASCII_ID, (char *)&dmf_svcb, (DM_OBJECT **)&lbmcb,
	err_code);

    if (status)
    {
	ule_format(*err_code, (CL_ERR_DESC *)0, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, err_code, 0);
	*err_code = E_DM0112_RESOURCE_QUOTA_EXCEED;
	return (E_DB_FATAL);
    }

    /* Remember who we are. */
    PCpid( &lbmcb->lbm_pid );

    /*
    ** If Shared Buffer Manager is specified, then see if we can connect
    ** to an already existing buffer manager.  If so then we just need to
    ** initialize the local buffer manager control block.
    */
    if (flags & DM0P_SHARED_BUFMGR)
    {
	shrbuf_init = TRUE;

	/* Verify buffer manager name */
	if (cache_name == NULL || *cache_name == '\0')
	{
	    *err_code = E_DM0123_BAD_CACHE_NAME;
	    return (E_DB_ERROR);
	}

	/*
	** Lower case the cache name.
	*/
	CVlower(cache_name);

	/*
	** Request lock on this buffer manager name.  Request it Exclusive
	** to keep other servers from starting up while we are.  This also
	** blocks servers from verifying the connection count while we
	** are in the middle of connecting.
	**
	** Check the value of the lock to determine if we are the only
	** holder of the lock - if so then we need to initialize the buffer
	** manager control blocks.
	*/
	lockkey.lk_type = LK_BM_LOCK;
	MEfill(sizeof(LK_LKID), 0, &bm_lockid);
	MEmove(STlength(cache_name), (PTR)cache_name, ' ',
	    (6 * sizeof(lockkey.lk_key1)), (PTR)&lockkey.lk_key1);
	cl_status = LKrequest((LK_PHYSICAL | LK_NODEADLOCK | LK_LOCAL),
	    lock_list, &lockkey, LK_X, &lock_value,
	    &bm_lockid, (i4)0, &sys_err);
	if ((cl_status != OK) && (cl_status != LK_VAL_NOTVALID))
	{
	    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
	    ule_format(E_DM901C_BAD_LOCK_REQUEST, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 2,
		0, LK_X, 0, lock_list);
	    ule_format(E_DM937C_BM_BUFMGR_LOCK_ERROR, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
	    *err_code = E_DM930A_DM0P_ALLOCATE;
	    if (cl_status == LK_NOLOCKS)
		*err_code = E_DM004B_LOCK_QUOTA_EXCEEDED;

	    return (E_DB_ERROR);
	}

	if ((cl_status == OK) && (lock_value.lk_low == BM_INIT_VALUE))
	{
	    /* Not the first connection, connect to existing buffer manager */
	    shrbuf_connect = TRUE;
	    shrbuf_init = FALSE;
	}
	/* If first connect, and connect-only request, error */
	if (shrbuf_init && connect_only)
	{
	    cl_status = LKrelease(0, lock_list, &bm_lockid,
		(LK_LOCK_KEY *)NULL, &lock_value, &loc_sys_err);
	    *err_code = E_DM101A_JSP_CANT_CONNECT_BM;
	    return (E_DB_ERROR);
	}

	/* If we're connecting to an existing buffer manager, handle
	** things separately because we have to depend on the existing
	** buffer manager settings (sizes, number of segments, etc).
	*/
	if (shrbuf_connect)
	{
	    MEfill(sizeof(bmsegs), 0, &bmsegs);

	    do {
		/* Connect to shared segment zero, where we'll find the
		** bm-common with the number of (other) segments to connect to.
		** Pull all the relevant cache size information out of the
		** shared cache segments so that we can allocate and set up
		** our local control blocks.
		** Since we never init a shared memory segment, the size
		** params are not used and can be passed as zero.
		*/
		status = bm_shrbuf(cache_name,
				0, 0, 1, flags,
				FALSE, shrbuf_connect, &bmsegs[0],
				err_code);
		if (status != E_DB_OK)
		    break;

		status = bm_check_shrseg(0, bmsegs[0], cache_name, err_code);
		if (status != E_DB_OK)
		    break;

		/* Connect to the other shared segments, if any */
		bm_common = (DM0P_BM_COMMON *)
			((char *) bmsegs[0] + bmsegs[0]->bms_bm_common);
		segcnt = bm_common->bmcb_segcnt;
		for (segment_ix = 1;
		     segment_ix < segcnt && status == E_DB_OK;
		     segment_ix++)
		{
		    status = bm_shrbuf(cache_name,
				0, segment_ix, 1, flags,
				FALSE, shrbuf_connect,
				&bmsegs[segment_ix],
				err_code);
		    if (status == E_DB_OK)
			status = bm_check_shrseg(segment_ix,
				bmsegs[segment_ix], cache_name, err_code);
		}
		if (status != E_DB_OK)
		    break;

		/* Copy cache sizing info back into arrays, so that
		** we can set up the right stuff in our lbmcb later.
		*/
		dbc_size = bm_common->bmcb_dbcsize;
		tbc_size = bm_common->bmcb_tblcsize;
		gw_dicb_size = bm_common->bmcb_gw_dicb_size;

		for (segment_ix = 0; segment_ix < segcnt; segment_ix++)
		{
		    bmsegp = bmsegs[segment_ix];
		    for (cache_ix = 0; cache_ix < DM_MAX_CACHE; cache_ix++)
		    {
			if (bmsegp->bms_bmcb[cache_ix] != 0)
			{
			    bm = (DM0P_BMCB *) ((char *) bmsegp
					+ bmsegp->bms_bmcb[cache_ix]);
			    bmcb_ptrs[cache_ix] = bm;
			    bcnt[cache_ix] = bm->bm_hshcnt + 1;
			    group[cache_ix] = bm->bm_gcnt;
			    buffers[cache_ix] = bm->bm_sbufcnt;
			    rpages[cache_ix] = bm->bm_bufcnt - bm->bm_sbufcnt;
			}
		    }
		}

		/* Deferred allocation is meaningless if connect */
		for (cache_ix = 0; cache_ix < DM_MAX_CACHE; cache_ix++)
		    cache_flags[cache_ix] &= ~DM0P_DEFER_PGALLOC;

		/*
		** Increment the number of servers connected to the Shared
		** Buffer Manager.  This must be done while holding the Buffer
		** Manager Lock in exclusive mode.
		*/
		++bm_common->bmcb_srv_count;
		TRdisplay("%@ Connected to Shared cache '%t', connected servers %d\n",
			STlength(cache_name), cache_name,
			bm_common->bmcb_srv_count);
		count_incremented = TRUE;

		/* update status bits while holding buf manager lock */
		if (flags & DM0P_IOMASTER)
		    bm_common->bmcb_status |= BMCB_IOMASTER;	/* An IO server now exists */

		/*
		** Convert the Buffer Manager Lock to null mode.  This allows
		** other servers to connect to the buffer manager.  We retain
		** the lock in null mode so we can do a show on it to count
		** the number of servers still connected to the shared segment.
		*/
		cl_status = LKrequest(
			LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK,
			lock_list, (LK_LOCK_KEY *)NULL, LK_N, &lock_value,
			&bm_lockid, (i4)0, &sys_err);
		if (cl_status != OK)
		{
		    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
				(char *)0, (i4)0, (i4 *)0, err_code, 0);
		    ule_format(E_DM901C_BAD_LOCK_REQUEST, &sys_err, ULE_LOG, NULL,
				(char *)0, (i4)0, (i4 *)0, err_code, 2,
				0, LK_N, 0, lock_list);
		    *err_code = E_DM937C_BM_BUFMGR_LOCK_ERROR;
		}
	    } while (0);
	} /* if connecting */
    }

    if (! shrbuf_connect)
    {

	bool	seg_overflow;
	i4	totbufcnt;
	i4	page_overhead;
	i4	first_segment_overhead;
	SIZE_TYPE segsize, this_size;

	/* All of this guff is only when we're initing a shared
	** or private cache.
	*/

	/*
	** Compute page overhead
	**
	** In addition to a page-sized piece of memory, each
	** page also requires
	**
	**  o a buffer descriptor (DM0P_BUFFER)
	**  o a hash chain (DM0P_HASH_HEAD)
	**  o an entry (buffer index (i4)) in the Consistency Point
	**    array
	**  o an entry in a GatherWrite array if GatherWrite
	**	  is in use
	*/
	page_overhead = sizeof(DM0P_BUFFER)
		  + sizeof(DM0P_HASH_HEAD)
		  + sizeof(i4);
	if ( (gw_dicb_size = DIgather_setting()) != 0 )
	    page_overhead += sizeof(DM0P_CLOSURE) + gw_dicb_size;

	/*
	** The first segment will contain the additional overhead
	** of BM_COMMON and the database and table caches.
	*/
	first_segment_overhead = sizeof(DM0P_BM_COMMON) +
		(sizeof(DM0P_BMCACHE) * (dbc_size + tbc_size));

	/*
	** Check input parameters for consistency.
	*/
	for (cache_ix = 0, totbufcnt = 0, pgsize = 2048;
	     cache_ix < DM_MAX_CACHE;
	     pgsize *= 2, cache_ix++)
	{
	    cache_size[cache_ix] = 0;
	    pool_size[cache_ix] = 0;

	    if (!buffers[cache_ix])
		continue;

	    if (group[cache_ix] && !gpages[cache_ix])
	    {
		*err_code = E_DM9239_BM_BAD_PARAMETER;
		return(E_DB_ERROR);
	    }

	    /*
	    ** Compute the maximum number of pages in this cache
	    ** leaving 8 pages worth of slop for rounding and
	    ** alignment.
	    */
#ifdef LP64
	    max_pages_per_cache[cache_ix] = MAXI4;
#else /* LP64 */
	    max_pages_per_cache[cache_ix] =
		    ((MAXI4 - (sizeof(DM0P_BMCB)
			    + first_segment_overhead
			    + (cache_flags[cache_ix] & DM0P_NEWSEG)
				? sizeof(DMP_BMSCB) : 0))
		    / (pgsize + page_overhead)) - 8;

#endif /* LP64 */
	    rpages[cache_ix] = group[cache_ix] * gpages[cache_ix];

	    excess_pages = (buffers[cache_ix] + rpages[cache_ix])
			    - max_pages_per_cache[cache_ix];

	    /*
	    ** If too many pages, this cache won't fit in any segment.
	    ** Proportionally reduce the number of single pages and/or
	    ** groups to fit and issue a warning to note that this has
	    ** been done.
	    */
	    if ( excess_pages > 0 )
	    {
		f8 pct_buffers = ((f8)buffers[cache_ix] * (f8)100.0)
			/ (buffers[cache_ix] + rpages[cache_ix]);

		buffers[cache_ix] =
		    ((f8)max_pages_per_cache[cache_ix] * pct_buffers)
			/ (f8)100.0;

		if ( group[cache_ix] )
		{
		    rpages[cache_ix] =
			((max_pages_per_cache[cache_ix] - buffers[cache_ix])
			    / gpages[cache_ix]) * gpages[cache_ix];

		    group[cache_ix] = rpages[cache_ix] / gpages[cache_ix];
		}

		TRdisplay(
"%@ WARNING: %dK Cache max pages %d exceeded, reduced to pages: %d groups: %d gpages: %d\n",
			pgsize >> 10,
			max_pages_per_cache[cache_ix],
			buffers[cache_ix],
			group[cache_ix],
			gpages[cache_ix]);
	    }

	    /* Add to the total number of buffers in all caches */
	    totbufcnt += buffers[cache_ix] + rpages[cache_ix];

	    /*
	    ** Make number of buckets equal to the total number of
	    ** single and group buffers, rounded up to 2.
	    ** There appears to be no compelling reason to allocate
	    ** more buckets than elements which will hash to those
	    ** buckets.
	    */
	    bcnt[cache_ix] = buffers[cache_ix] + rpages[cache_ix];
	    bcnt[cache_ix] += (bcnt[cache_ix] % 2);

	    /* Off the internal-use only allocated flag */
	    cache_flags[cache_ix] &= ~DM0P_CACHE_ALLOCATED;

	    /* First segment overhead is no longer a factor
	    ** after the first cache
	    */
	    first_segment_overhead = 0;
	}

	/*
	** Compute the amount of memory needed for the buffer manager.
	**
	** The Local Buffer Manager Control Block (LBMCB) is has a fixed
	** size and is allocated above.
	**
	** The rest of the buffer manager space may be located in shared
	** memory so that multiple servers can use the same buffer cache.
	**
	** Separate buffer pools are managed for each page size, 2k, 4k,
	** 8k, 16k, 32k, 64k. All are optional, but at least one must
	** be specified.
	**
	** Note that these buffer pools may be physically allocated
	** in separate segments of memory OR any pool may be allocated
	** with any other.
	**
	**
	** Each segment of buffer manager memory contains the following
	** sections. Each section is quad word aligned.
	**
	**    - The Buffer Manager Segment Control Block (BMSCB) comes first.
	**      It has a fixed size.
	**      It contains offsets to interesting sections of buffer manager
	**      that were allocated contiguous with the BMSCB.
	**      For example, the BM_COMMON section, BMCACHE(s) and BMCACHE(s)
	**
	** The following sections are allocated only with the first allocated
	** segment:
	**
	**    - The BM_COMMON containing information common for all pools.
	**      It has a fixed size.
	**
	**    - Next comes the database and table cache entries.  These are
	**      used to hold cache locks on closed databases and tables so
	**      that pages can remain in the cache until they are opened
	**      again.  The number of these entries are determined by the
	**      dm0p_allocate arguments 'dbc_size' and 'tbc_size'.
	**
	** The following sections contain information for a specific
	** buffer pool. If more than one buffer pool is allocated in
	** one segment of buffer manager memory, the following sections
	** will follow for each pool.
	**
	**    - The DM0P_BMCB pool specific information. It has a fixed size.
	**
	**    - Next comes the hash buckets.  Buffers are linked onto a hash
	**      bucket according to its page no. and table id.  The space
	**      required for for the hash buckets is determined by the
	**      number of pages in the buffer pool.  The number of pages is
	**      rounded to the next power of two to get the number of
	**      buckets.  A bucket consists of a pointer to a buffer
	**      descriptor.
	**
	**    - Next comes the group descriptors.  There is one group
	**      descriptor for each set of group buffers.
	**
	**    - Next comes the buffer descriptors.  There is one buffer
	**      descriptor for each page in the buffer pool.  This includes
	**      a buffer descriptor for each group buffer page.
	**
	**    - Next comes the Consistency Point buffer id array, used to
	**      quickly elect apparently CP-eligible buffers and presort them
	**      prior to forcing.
	**
	**    - Next (if GatherWrite enabled) come the GatherWrite structures.
	**
	**    - Next comes the actual pages.
	**
	** Unless DM0P_NEWSEG is set for a pool, attempt to allocate all
	** pools in a single segment; if the segment runs out of memory,
	** start a new segment. If DM0P_NEWSEG is set, allocate that pool
	** in its own segment.
	*/
	segcnt = 0;
	for (segment_ix = 0; segment_ix < DM_MAX_BUF_SEGMENT; segment_ix++)
	{
	    /* Init temporary BMSCB for this memory segment */
	    MEfill(sizeof(tmp_bmseg[segment_ix]), 0, &tmp_bmseg[segment_ix]);
	    bmsegs[segment_ix] = 0;
	    seg_overflow = FALSE;
	    seg_caches[segment_ix] = 0;

	    /* Each segment contains a DMP_BMSCB */
	    segsize = DB_ALIGN_MACRO(sizeof(DMP_BMSCB));

	    /*
	    ** BM_COMMON, db and table caches allocated but once,
	    ** in the first segment, following DMP_BMSCB.
	    */

	    if ( segment_ix == 0 )
	    {
		/* Save offset to DM0P_BM_COMMON */
		tmp_bmseg[segment_ix].bms_bm_common = segsize;

		/* Add size of DM0P_BM_COMMON */
		segsize += DB_ALIGN_MACRO(sizeof(DM0P_BM_COMMON));

		/* Save offset to database cache */
		tmp_bmseg[segment_ix].bms_dbcache = segsize;

		/* Add size of database cache */
		segsize += DB_ALIGN_MACRO((SIZE_TYPE) sizeof(DM0P_BMCACHE)
				* dbc_size);

		/* Save offset to table cache */
		tmp_bmseg[segment_ix].bms_tblcache = segsize;

		/* Add size of table cache */
		segsize += DB_ALIGN_MACRO((SIZE_TYPE) sizeof(DM0P_BMCACHE)
				* tbc_size);
	    }

	    /*
	    ** We need to loop through config parameters for all page sizes.
	    ** 2k,4k,8k,16k,32k,64k page pools may be allocated together
	    ** or separately.
	    **
	    ** This segment will be allocated only if it is configured with
	    ** at least one buffer pool.
	    */
	    for (cache_ix = 0, pgsize = 2048;
		 cache_ix < DM_MAX_CACHE;
		 cache_ix++, pgsize *= 2)
	    {
		/*
		** If there are no buffers in this cache, or it's already
		** been allocated, skip it.
		*/
		if ( buffers[cache_ix] == 0 ||
		     cache_flags[cache_ix] & DM0P_CACHE_ALLOCATED )
		    continue;

		/*
		** Check if this cache can be combined with others in this
		** segment or should be allocated in its own segment.
		** If this segment hasn't been used yet,
		** it is the defacto "new segment".
		*/
		if ( seg_caches[segment_ix]
		    && cache_flags[cache_ix] & DM0P_NEWSEG )
		    continue;

		do
		{
		    /* Save offset to DM0P_BMCB (beginning of cache) */
		    tmp_bmseg[segment_ix].bms_bmcb[cache_ix] = segsize;

		    /* Add size of DM0P_BMCB */
		    this_size = DB_ALIGN_MACRO(sizeof(DM0P_BMCB));
		    if (MAX_SIZE_TYPE - this_size <= segsize)
		    {
			seg_overflow = TRUE;
			break;
		    }
		    segsize += this_size;

		    /* Add size of hash buckets */
		    this_size = DB_ALIGN_MACRO((SIZE_TYPE) sizeof(DM0P_HASH_HEAD)
				* bcnt[cache_ix]);
		    if (MAX_SIZE_TYPE - this_size <= segsize)
		    {
			seg_overflow = TRUE;
			break;
		    }
		    segsize += this_size;

		    /* Add size of single page buffers and group buffers */
		    this_size = DB_ALIGN_MACRO(sizeof(DM0P_BUFFER)
			* (SIZE_TYPE)(buffers[cache_ix] + rpages[cache_ix]));
		    if (MAX_SIZE_TYPE - this_size <= segsize)
		    {
			seg_overflow = TRUE;
			break;
		    }
		    segsize += this_size;

		    /* Add size of Consistency Point array, one entry for each
		    ** buffer
		    */
		    this_size = DB_ALIGN_MACRO(sizeof(i4)
			* (SIZE_TYPE)(buffers[cache_ix] + rpages[cache_ix]));
		    if (MAX_SIZE_TYPE - this_size <= segsize)
		    {
			seg_overflow = TRUE;
			break;
		    }
		    segsize += this_size;

		    /*
		    ** If GatherWrite is enabled in this installation, we need
		    ** to allocate some additional arrays which are used to
		    ** save off needed information for each GatherWrite request:
		    **
		    **    For each buffer,
		    **		a DM0P_CLOSURE structure
		    **		a structure for use by DI, the size of which
		    **		was provided by the DIgather_setting() function.
		    */
		    if ( this_size = gw_dicb_size )
		    {
			this_size = DB_ALIGN_MACRO(
				(SIZE_TYPE)(sizeof(DM0P_CLOSURE) + gw_dicb_size)
				* (buffers[cache_ix] + rpages[cache_ix]));
			if (MAX_SIZE_TYPE - this_size <= segsize)
			{
			    seg_overflow = TRUE;
			    break;
			}
		    }
		    segsize += this_size;

		    /* Add size for group descriptors */
		    if ( this_size = group[cache_ix] )
		    {
			this_size = DB_ALIGN_MACRO(
				(SIZE_TYPE) sizeof(DM0P_GROUP) * group[cache_ix]);
			if (MAX_SIZE_TYPE - this_size <= segsize)
			{
			    seg_overflow = TRUE;
			    break;
			}
		    }
		    segsize += this_size;

		    /* Save cache size excluding page pool */
		    cache_size[cache_ix] =
			segsize - tmp_bmseg[segment_ix].bms_bmcb[cache_ix];

		    /*
		    ** Add size of pages for single and group pages
		    ** Add an extra memory-page size to the segment size
		    ** for rounding slop when we assign a page-aligned
		    ** pointer to the page pool, later.
		    */
		    if (!(cache_flags[cache_ix] & DM0P_DEFER_PGALLOC))
		    {
			segsize += FIXME_PAGESIZE;
			pool_size[cache_ix] = (SIZE_TYPE) pgsize
					* (buffers[cache_ix] + rpages[cache_ix]);
			if (MAX_SIZE_TYPE - pool_size[cache_ix] <= segsize)
			{
			    seg_overflow = TRUE;
			    break;
			}
			segsize += pool_size[cache_ix];
		    }

		} while ( 0 );

		if ( seg_overflow )
		{
		    /*
		    ** Back out this cache's memory from this segment.
		    ** Move on to any remaining caches which might fit
		    ** in this segment, then start a new segment in
		    ** which to fit those which haven't yet been
		    ** allocated.
		    */
		    segsize = tmp_bmseg[segment_ix].bms_bmcb[cache_ix];
		    tmp_bmseg[segment_ix].bms_bmcb[cache_ix] = 0;
		    seg_overflow = FALSE;
		}
		else
		{
		    /* Count another cache allocated in this segment */
		    seg_caches[segment_ix]++;

		    /* Flag this cache as allocated */
		    cache_flags[cache_ix] |= DM0P_CACHE_ALLOCATED;

		    /*
		    ** If this cache is to be the sole inhabitant of
		    ** this segment, break to start a new segment.
		    */
		    if ( cache_flags[cache_ix] & DM0P_NEWSEG )
			break;
		}
	    } /* cache-fitting for */

	    /*
	    ** If any caches allocated to this segment, save the
	    ** segment size, rounded up to 1K.
	    ** Also, always allocate segment zero with the common stuff
	    ** in it.  (It's imaginable that a very huge cache might not
	    ** fit except completely by itself.)
	    */
	    if ( segment_ix == 0 || seg_caches[segment_ix] )
	    {
		++ segcnt;
		tmp_bmseg[segment_ix].bms_segsize =
		    ((segsize + 1023) / 1024) * 1024;
	    }

	} /* for each segment */

	if (shrbuf_init)
	    TRdisplay("%@ Shared cache '%t' configuration:\n",
			STlength(cache_name), cache_name);
	else
	    TRdisplay("%@ Local cache configuration:\n");

	/* Display successful segment/cache allocations */
	for (segment_ix = 0; segment_ix < segcnt; segment_ix++)
	{
	    TRdisplay("%@ %s memory segment %d size %dK:\n",
		( shrbuf_init ) ? "Shared" : "Local",
		segment_ix, tmp_bmseg[segment_ix].bms_segsize / 1024);
	    TRdisplay("%@   Cache BytesPP    Pages   Groups   Gpages    CacheK    PpoolK\n");

	    for ( cache_ix = 0, pgsize = 2048;
		  cache_ix < DM_MAX_CACHE;
		  cache_ix++, pgsize *= 2 )
	    {
		if ( tmp_bmseg[segment_ix].bms_bmcb[cache_ix] )
		    TRdisplay("%@     %2dK   %5d %8d %8d %8d %8dK %8dK\n",
			pgsize >> 10,
			pgsize + page_overhead,
			buffers[cache_ix],
			group[cache_ix],
			rpages[cache_ix],
			cache_size[cache_ix] / 1024,
			pool_size[cache_ix] / 1024);
	    }
	}

	/* Show any caches that could not be allocated */
	for ( cache_ix = 0, pgsize = 2048;
	      cache_ix < DM_MAX_CACHE;
	      cache_ix++, pgsize *= 2 )
	{
	    if ( buffers[cache_ix] &&
		  (cache_flags[cache_ix] & DM0P_CACHE_ALLOCATED) == 0 )
	    {
		TRdisplay("%@ %dK Cache allocation failed due to insufficient memory...\n",
		    pgsize >> 10);
		TRdisplay("%@     pages: %d groups: %d total gpages: %d bytes per page %d\n",
		    buffers[cache_ix],
		    group[cache_ix],
		    group[cache_ix] * gpages[cache_ix],
		    pgsize + page_overhead);

		status = E_DB_ERROR;
		*err_code = E_DM930A_DM0P_ALLOCATE;
	    }
	}

	if ( status == E_DB_OK )
	{
	    /*
	    ** If using a shared memory buffer manager, create the
	    ** shared memory segment.
	    */
	    if (shrbuf_init)
	    {
		for (segment_ix = 0; segment_ix < segcnt; segment_ix++)
		{
		    status = bm_shrbuf(cache_name,
				tmp_bmseg[segment_ix].bms_segsize,
				segment_ix, totbufcnt, flags,
				shrbuf_init, FALSE, &bmsegs[segment_ix],
				err_code);
		    if (status != E_DB_OK)
			break;
		}

		if ( status == E_DB_OK )
		{
		    for (segment_ix = 0; segment_ix < segcnt; segment_ix++)
		    {
			bmsegp = bmsegs[segment_ix];
			/* Initialize BMH_CB */
			bmsegp->bms_type = BMSCB_CB;
			bmsegp->bms_length = tmp_bmseg[segment_ix].bms_segsize;
			bmsegp->bms_owner = 0;
			bmsegp->bms_ascii_id = BMSCB_ASCII_ID;
		    }

		    bm_common = (DM0P_BM_COMMON *)((char *)bmsegs[0]  +
					    tmp_bmseg[0].bms_bm_common);
		}
	    }
	    else
	    {
		/*
		** Allocate the Buffer Manager segment(s) in local memory.
		** Get it from the DMF memory manager.
		*/
		for (segment_ix = 0; segment_ix < segcnt; segment_ix++)
		{
		    status = dm0m_allocate(tmp_bmseg[segment_ix].bms_segsize,
			    (flags & DM0P_LOCK_CACHE) ? DM0M_LOCKED : 0,
			    (i4)BMSCB_CB,
			    (i4)BMSCB_ASCII_ID, (char *)&dmf_svcb,
			    (DM_OBJECT **)&bmsegs[segment_ix], err_code);
		    if (status != E_DB_OK)
			break;
		}

		if ( status == E_DB_OK )
		    bm_common = (DM0P_BM_COMMON *)((char *)bmsegs[0] +
					tmp_bmseg[0].bms_bm_common);
		else
		{
		    ule_format(*err_code, &sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 0);
		    *err_code = E_DM0112_RESOURCE_QUOTA_EXCEED;
		}
	    }
	}
	/*
	** Set up pointers to DM0P_BMCB(s) regardless of which segment of
	** buffer manager memory it may be in
	*/
	for (segment_ix = 0; segment_ix < segcnt; segment_ix++)
	{
	    for (cache_ix = 0; cache_ix < DM_MAX_CACHE; cache_ix++)
		if (tmp_bmseg[segment_ix].bms_bmcb[cache_ix])
		    bmcb_ptrs[cache_ix] =
			(DM0P_BMCB *)((char *)bmsegs[segment_ix] +
			    tmp_bmseg[segment_ix].bms_bmcb[cache_ix]);
	}

	/* We need these two now, for the LBMCB setup */
	bmsegs[0]->bms_dbcache = tmp_bmseg[0].bms_dbcache;
	bmsegs[0]->bms_tblcache = tmp_bmseg[0].bms_tblcache;

    } /* if not connecting */


    if ( status == E_DB_OK )
    {
	/*
	** Create or connect to the buffer manager lock list.  This lock list
	** is used to hold all cache locks.  If we are connecting to an already
	** existing buffer manager, then just connect to the lock list with
	** the id given by bm_lock_key.
	*/
	flags_arg = (LK_ASSIGN | LK_NONPROTECT | LK_NOINTERRUPT | LK_MULTITHREAD);
	lock_list_key = NULL;
	if (flags & DM0P_MASTER)
	{
	    flags_arg |= LK_MASTER;
	}
	if (shrbuf_init)
	{
	    flags_arg |= LK_SHARED;
	}
	else if (shrbuf_connect)
	{
	    lock_list_key = &bm_common->bmcb_lock_key;
	    flags_arg = (LK_CONNECT | LK_NONPROTECT | LK_MULTITHREAD);
	}
	cl_status = LKcreate_list(flags_arg, (LK_LLID)0, lock_list_key,
	    &bm_lock_list, (i4)0, &sys_err);

	if (cl_status != OK)
	{
	    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
	    ule_format(E_DM901A_BAD_LOCK_CREATE, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 0);
	    if (shrbuf_connect)
	    {
		ule_format(E_DM9376_BM_LOCK_CONNECT, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 1,
		    0, bm_lock_list);
	    }
	    *err_code = E_DM937C_BM_BUFMGR_LOCK_ERROR;
	    if (cl_status == LK_NOLOCKS)
		*err_code = E_DM004B_LOCK_QUOTA_EXCEEDED;
	    status = E_DB_ERROR;
	}
	else
	{
	    /*
	    ** Allocate buffer manager transaction used as a handle to force
	    ** the log file when writing a modified buffer out of the cache.
	    */
	    MEcopy((PTR)DB_BUFMGR_NAME, sizeof(add_info.ad_dbname),
		(PTR) &add_info.ad_dbname);
	    MEcopy((PTR)DB_INGRES_NAME, sizeof(add_info.ad_dbowner),
		(PTR) &add_info.ad_dbowner);
	    MEcopy((PTR)"None", 4, (PTR) &add_info.ad_root);
	    add_info.ad_dbid = 0;
	    add_info.ad_l_root = 4;
	    len_add_info = sizeof(add_info) - sizeof(add_info.ad_root) + 4;

	    cl_status = LGadd(dmf_svcb->svcb_lctx_ptr->lctx_lgid, LG_NOTDB,
		(char *)&add_info,
		len_add_info, &bm_dbid, &sys_err);

	    if (cl_status != OK)
	    {
		(VOID) ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		    (char *)0, 0L, (i4 *)0, err_code, 0);
		(VOID) ule_format(E_DM900A_BAD_LOG_DBADD, &sys_err, ULE_LOG, NULL,
		    (char *)0, 0L, (i4 *)0, err_code, 4,
		    0, dmf_svcb->svcb_lctx_ptr->lctx_lgid,
		    sizeof(add_info.ad_dbname), (PTR) &add_info.ad_dbname,
		    sizeof(add_info.ad_dbowner), (PTR) &add_info.ad_dbowner,
		    4, (PTR) &add_info.ad_root);
		*err_code = E_DM930A_DM0P_ALLOCATE;
		if (cl_status == LG_EXCEED_LIMIT)
		    *err_code = E_DM0062_TRAN_QUOTA_EXCEEDED;
		status = E_DB_ERROR;
	    }
	    else
	    {
		cl_status = LGbegin(LG_NOPROTECT, bm_dbid, &tran_id,
		    &bm_lxid, sizeof(DB_OWN_NAME), DB_BUFMGR_OWNER,
		    (DB_DIS_TRAN_ID*)NULL, &sys_err);
		if (cl_status != OK)
		{
		    (VOID) ule_format(cl_status, &sys_err, ULE_LOG, NULL,
			(char *)0, 0L, (i4 *)0, err_code, 0);
		    ule_format(E_DM900C_BAD_LOG_BEGIN, &sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 1,
			0, bm_dbid);
		    *err_code = E_DM930A_DM0P_ALLOCATE;
		    if (cl_status == LG_EXCEED_LIMIT)
			*err_code = E_DM0062_TRAN_QUOTA_EXCEEDED;
		    status = E_DB_ERROR;
		}
	    }
	}
    }

    if ((status != E_DB_OK) || (cl_status != OK))
    {
	/*
	** Deallocate Buffer Manager Control Blocks.
	** If shared segment, don't bother to deallocate it.  Either it
	** will go away automatically, or will be destroyed by the next
	** server to connect to it.
	*/
	if (lbmcb)
	    dm0m_deallocate((DM_OBJECT **)&lbmcb);

	if ( !(shrbuf_init || shrbuf_connect))
	{
	    for (segment_ix = 0; segment_ix < DM_MAX_BUF_SEGMENT; segment_ix++)
	    {
		if ( bmsegs[segment_ix] )
		    dm0m_deallocate((DM_OBJECT **)&bmsegs[segment_ix]);
	    }
	}
	else
	{
	    if (count_incremented)
		bm_common->bmcb_srv_count--;
	    /* FIXME if we were an iomaster this leaves IOMASTER
	    ** set in the shared BM status.  Either ensure only one
	    ** iomaster, or change flag to a count.  Not a big deal
	    ** at present (Jan '06) since nobody uses them.
	    */
	    cl_status = LKrelease(0, lock_list, &bm_lockid,
		(LK_LOCK_KEY *)NULL, &lock_value, &loc_sys_err);
	    if (cl_status != OK)
	    {
		ule_format(cl_status, &loc_sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &local_error, 0);
		ule_format(E_DM901B_BAD_LOCK_RELEASE, &loc_sys_err, ULE_LOG,
		    NULL, (char *)0, (i4)0, (i4 *)0, &local_error, 1,
		    0, lock_list);
		ule_format(E_DM937C_BM_BUFMGR_LOCK_ERROR, &loc_sys_err, ULE_LOG,
		    NULL, (char *)0, (i4)0, (i4 *)0, &local_error, 0);
	    }
	    /* Don't bother disconnecting from shared memory,
	    ** we're going to fall over soon anyway.  Fixing the count
	    ** is the important bit.
	    */
	}

	if (bm_lxid)
	{
	    cl_status = LGend(bm_lxid, 0, &sys_err);
	    if (cl_status != OK)
	    {
		ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &local_error, 0);
		ule_format(E_DM900E_BAD_LOG_END, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &local_error, 1,
		    0, bm_lxid);
	    }
	}

	if (bm_dbid)
	{
	    cl_status = LGremove(bm_dbid, &sys_err);
	    if (cl_status != OK)
	    {
		ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &local_error, 0);
		ule_format(E_DM9016_BAD_LOG_REMOVE, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &local_error, 1,
		    0, bm_dbid);
	    }
	}

	if (*err_code > E_DM_INTERNAL)
	{
	    ule_format(*err_code, &sys_err, ULE_LOG, NULL,
	        (char *)0, (i4)0, (i4 *)0, err_code, 0);
	    *err_code = E_DM930A_DM0P_ALLOCATE;
	}
	return (E_DB_FATAL);
    }

    /*
    ** Set up pointers to buffer manager memory in LBMCB.  These pointers
    ** are not stored in the BMSCB so it can be position independent.
    */
    bmsegp = bmsegs[0];
    lbmcb->lbm_bm_common = bm_common;
    lbmcb->lbm_dbcache = (DM0P_BMCACHE *)((char *)bmsegp +
				bmsegp->bms_dbcache);
    lbmcb->lbm_tblcache = (DM0P_BMCACHE *)((char *)bmsegp +
				bmsegp->bms_tblcache);
    lbmcb->lbm_lock_list = bm_lock_list;
    STRUCT_ASSIGN_MACRO(bm_lockid, lbmcb->lbm_bm_lockid);
    lbmcb->lbm_bm_lockid = bm_lockid;
    lbmcb->lbm_bm_dbid = bm_dbid;
    lbmcb->lbm_num_logs = 1;
    lbmcb->lbm_bm_lxid[0] = bm_lxid;
    lbmcb->lbm_lockreclaim = 0;
    lbmcb->lbm_fcflush = 0;
    lbmcb->lbm_bmcwait = 0;
    lbmcb->lbm_status = 0;

    /*
    ** Initialize the BM log_id mutex.
    ** this mutex is used to single-thread dm0l_force calls
    ** for which a transaction's lx_id is not available
    ** and must therefore use the BM's lx_id.
    ** Note that the LBMCB is local to the server, so
    ** its mutex is a SINGLE type.
    */
    if (dm0p_minit(&lbmcb->lbm_lxid_mutex, "BM lx_id", CS_SEM_SINGLE))
	return(E_DB_FATAL);

    /*
    ** (ICL phil.p)
    ** Create DMCM lock list
    */
    if (flags & DM0P_DMCM)
    {
        cl_status = LKcreate_list(LK_ASSIGN | LK_NONPROTECT,
                                  (LK_LLID)0, (LK_UNIQUE *)0,
                                  &lbmcb->lbm_dmcm_lock_list,
                                  (i4)0, &sys_err);
        if (cl_status != OK)
        {
            ule_format(cl_status, &sys_err, ULE_LOG, NULL,
	        (char *)0, (i4)0, (i4 *)0, err_code, 0);
	    ule_format(E_DM901A_BAD_LOCK_CREATE, &sys_err, ULE_LOG, NULL,
	        (char *)0, (i4)0, (i4 *)0, err_code, 0);
	    *err_code = E_DM937C_BM_BUFMGR_LOCK_ERROR;
	    if (cl_status == LK_NOLOCKS)
	        *err_code = E_DM004B_LOCK_QUOTA_EXCEEDED;
            return(E_DB_FATAL);
        }
    }

    for (cache_ix = 0, pgsize = 2048;
	 cache_ix < DM_MAX_CACHE;
	 cache_ix++, pgsize *= 2)
    {
	if (!buffers[cache_ix])
	{
	    lbmcb->lbm_bmcb[cache_ix] = NULL;
	    lbmcb->lbm_buckets[cache_ix] = NULL;
	    lbmcb->lbm_groups[cache_ix] = NULL;
	    lbmcb->lbm_buffers[cache_ix] = NULL;
	    lbmcb->lbm_pages[cache_ix] = NULL;
	    lbmcb->lbm_cp_array[cache_ix] = NULL;
	    lbmcb->lbm_gw_closure[cache_ix] = NULL;
	    lbmcb->lbm_gw_dicb[cache_ix] = NULL;
	}
	else
	{
	    char *pp;

	    lbmcb->lbm_bmcb[cache_ix] = bmcb_ptrs[cache_ix];

	    lbmcb->lbm_buckets[cache_ix] =
			    (DM0P_HASH_HEAD *) ((char *)bmcb_ptrs[cache_ix] +
				DB_ALIGN_MACRO(sizeof(DM0P_BMCB)));

	    /*
	    ** If there are no group pages, then skip the group buffers.
	    */
	    if (rpages[cache_ix])
	    {
		lbmcb->lbm_groups[cache_ix] =
		     (DM0P_GROUP *) ((char *)lbmcb->lbm_buckets[cache_ix] +
			DB_ALIGN_MACRO((SIZE_TYPE)sizeof(DM0P_HASH_HEAD)
				* bcnt[cache_ix]));

		lbmcb->lbm_buffers[cache_ix] =
		  (DM0P_BUFFER *)((char *)lbmcb->lbm_groups[cache_ix] +
			DB_ALIGN_MACRO((SIZE_TYPE)sizeof(DM0P_GROUP)
				* group[cache_ix]));
	    }
	    else
	    {
		lbmcb->lbm_groups[cache_ix] = NULL;
		lbmcb->lbm_buffers[cache_ix] =
		    (DM0P_BUFFER *)((char *)lbmcb->lbm_buckets[cache_ix] +
		      DB_ALIGN_MACRO((SIZE_TYPE)sizeof(DM0P_HASH_HEAD)
				* bcnt[cache_ix]));
	    }

	    /* Set the pointer to this cache's CP array */
	    lbmcb->lbm_cp_array[cache_ix] = (i4*)
		((char *)lbmcb->lbm_buffers[cache_ix] +
		    DB_ALIGN_MACRO((SIZE_TYPE) sizeof(DM0P_BUFFER)
			* (buffers[cache_ix] + rpages[cache_ix])));
	    pp = (char *) lbmcb->lbm_cp_array[cache_ix]
		+ DB_ALIGN_MACRO((SIZE_TYPE) sizeof(i4)
			* (buffers[cache_ix] + rpages[cache_ix]));

	    /*
	    ** If GatherWrite is enabled in this installation, we've allocated
	    ** some additional arrays which are used to save off needed
	    ** information for each GatherWrite request:
	    **
	    **    For each buffer,
	    **		a DM0P_CLOSURE structure
	    **		a structure for use by DI, the size of which
	    **		is provided by the DIgather_setting() function.
	    **
	    ** Set the pointers to these arrays in the LBMCB:
	    */
	    if ( gw_dicb_size )
	    {
		/* GatherWrite enabled on this server? */
		if ( DIgather_setting() )
		    lbmcb->lbm_status |= LBM_GW_ENABLED;

		lbmcb->lbm_gw_closure[cache_ix] = (DM0P_CLOSURE*) pp;

		lbmcb->lbm_gw_dicb[cache_ix] = (char*) pp +
			DB_ALIGN_MACRO((SIZE_TYPE) sizeof(DM0P_CLOSURE)
				* (buffers[cache_ix] + rpages[cache_ix]));

		pp = (char *) lbmcb->lbm_gw_dicb[cache_ix]
			+ DB_ALIGN_MACRO((SIZE_TYPE) gw_dicb_size
				* (buffers[cache_ix] + rpages[cache_ix]));
	    }
	    else
	    {
		lbmcb->lbm_gw_closure[cache_ix] = NULL;
		lbmcb->lbm_gw_dicb[cache_ix] = NULL;
	    }
	    if (cache_flags[cache_ix] & DM0P_DEFER_PGALLOC)
		lbmcb->lbm_pages[cache_ix] = NULL;
	    else
	    {
		/* Pool starts on memory-page boundary.
		** NOTE: we assume that shared memory attaches occur at
		** FIXME_PAGESIZE-aligned addresses, or better.
		*/
		pp = ME_ALIGN_MACRO(pp, FIXME_PAGESIZE);
		lbmcb->lbm_pages[cache_ix] = (DMPP_PAGE *) pp;
	    }
	}

	MEfill(sizeof(lbmcb->lbm_stats[cache_ix]), 0, &lbmcb->lbm_stats[cache_ix]);
	lbmcb->lbm_stats[cache_ix].bs_pgsize = pgsize; /* for dm0pmo.c */
    }


    /*
    ** Initialise Buffer Manager Managed objects
    */
    cl_status = dm0p_mo_init( lbmcb, bm_common );
    if ( cl_status != OK )
    {
	*err_code = E_DM930E_DM0P_MO_INIT_ERR;
	return( E_DB_FATAL );
    }

    /*
    ** If connecting to a shared buffer manager,
    ** we's just about done.
    */
    if (shrbuf_connect)
    {
	dmf_svcb->svcb_lbmcb_ptr = lbmcb;
	for (segment_ix = 0; segment_ix < DM_MAX_BUF_SEGMENT; segment_ix++)
	    dmf_svcb->svcb_bmseg[segment_ix] = bmsegs[segment_ix];
	return (E_DB_OK);
    }

    /*
    ** Set the type of semaphores we'll be using,
    ** MULTI if shared BM,
    ** SINGLE if not.
    */
    sem_type = (shrbuf_init)
		? CS_SEM_MULTI
		: CS_SEM_SINGLE;

    /*
    ** Initialize the Buffer Manager Control Block.
    ** Reset the hash and transaction buckets.
    ** Init the buffer headers and state queues.
    */

    TRdisplay("%@ Initializing %s cache '%t'\n",
	( shrbuf_init ) ? "Shared" : "Local",
	( shrbuf_init ) ? STlength(cache_name) : STlength("LOCAL_CACHE"),
	( shrbuf_init ) ? cache_name : "LOCAL_CACHE");

    if ((flags & DM0P_SHARED_BUFMGR) && (cache_name != NULL))
	STmove(cache_name, ' ', DB_MAXNAME, bm_common->bmcb_name.db_name);
    else
	STmove("LOCAL_CACHE", ' ', DB_MAXNAME, bm_common->bmcb_name.db_name);

    bm_common->bmcb_id = bm_lock_list;
    bm_common->bmcb_dbcsize = dbc_size;
    bm_common->bmcb_tblcsize = tbc_size;
    bm_common->bmcb_segcnt = segcnt;
    bm_common->bmcb_srv_count = 1;
    bm_common->bmcb_cpcount = 1;
    bm_common->bmcb_cpindex = 0;
    bm_common->bmcb_cpbufs = 0;
    bm_common->bmcb_cpcheck = 0;
    bm_common->bmcb_last_lsn.lsn_high = 0;
    bm_common->bmcb_last_lsn.lsn_low = 0;
    bm_common->bmcb_lockreclaim = 0;
    bm_common->bmcb_fcflush = 0;
    bm_common->bmcb_bmcwait = 0;
    bm_common->bmcb_status = 0;
    bm_common->bmcb_totbufcnt = 0;
    bm_common->bmcb_gw_dicb_size = gw_dicb_size;

    /* Init DMP_BMSCB with offsets calculated at layout */
    for (segment_ix = 0; segment_ix < segcnt; segment_ix++)
    {
	bmsegp = bmsegs[segment_ix];
	bmsegp->bms_bm_common = tmp_bmseg[segment_ix].bms_bm_common;
	bmsegp->bms_segsize = tmp_bmseg[segment_ix].bms_segsize;
	/* DB, table cache only in segment zero (already set), make
	** sure no dirt left in other segment CB's.
	*/
	if (segment_ix != 0)
	    bmsegp->bms_tblcache = bmsegp->bms_dbcache = 0;
	STmove(bm_common->bmcb_name.db_name, ' ', DB_MAXNAME,
		bmsegp->bms_name.db_name);
	for (cache_ix = 0; cache_ix < DM_MAX_CACHE; cache_ix++)
	{
	    if (tmp_bmseg[segment_ix].bms_bmcb[cache_ix])
		bmsegp->bms_bmcb[cache_ix] =
				tmp_bmseg[segment_ix].bms_bmcb[cache_ix];
	}
    }

    /*
    ** (ICL phil.p)
    ** Does this buffer manager support DMCM?
    */
    if (flags & DM0P_DMCM)
	bm_common->bmcb_status |= BMCB_DMCM;

    /* Notice if we're running with OS threads */
    if (CS_is_mt())
	bm_common->bmcb_status |= BMCB_IS_MT;

    /* Note if cluster support exists */
    if (CXcluster_enabled())
        bm_common->bmcb_status |= BMCB_CLUSTERED;
    if (CXconfig_settings(CX_HAS_FAST_CLM))
	bm_common->bmcb_status |= BMCB_USE_CLM;

    /*
    ** Initialize the BMCB mutexes.
    */

    if (dm0p_minit(&bm_common->bmcb_cp_mutex, "BM cp mutex", sem_type))
	return (E_DB_FATAL);
    if (dm0p_minit(&bm_common->bmcb_status_mutex, "BM status mutex", sem_type))
	return (E_DB_FATAL);

    if (shrbuf_init)
    {
	bm_common->bmcb_status |= BMCB_SHARED_BUFMGR;
    }

    if (flags & DM0P_IOMASTER)
	bm_common->bmcb_status |= BMCB_IOMASTER;	/* An IO server now exists */

    /*
    ** Initialize the database cache locks.
    */
    bm_common->bmcb_dbclist.sth_stq.stq_next = BUF_ENDLIST;
    bm_common->bmcb_dbclist.sth_stq.stq_prev = BUF_ENDLIST;
    bm_common->bmcb_dbclist.sth_count = 0;
    bm_common->bmcb_dbclist.sth_lcount = 0;
    if (dm0p_minit(&bm_common->bmcb_dbclist.sth_mutex, "BM dbclist", sem_type))
	return (E_DB_FATAL);

    if (dbc_size)
    {
	for (i = 0; i < dbc_size; i++)
	{
	    lbmcb->lbm_dbcache[i].bmc_sts = BMC_FREE;
	    lbmcb->lbm_dbcache[i].bmc_index = i;
	    lbmcb->lbm_dbcache[i].bmc_refcount = 0;
	    lbmcb->lbm_dbcache[i].bmc_state.stq_next = i + 1;
	    lbmcb->lbm_dbcache[i].bmc_state.stq_prev = i - 1;
	    if (dm0p_minit(&lbmcb->lbm_dbcache[i].bmc_mutex,
			STprintf(sem_name, "BM dbcache %d", i),
			sem_type))
		return(E_DB_FATAL);
	}
	lbmcb->lbm_dbcache[0].bmc_state.stq_prev = BUF_ENDLIST;
	lbmcb->lbm_dbcache[dbc_size-1].bmc_state.stq_next = BUF_ENDLIST;
	bm_common->bmcb_dbclist.sth_stq.stq_next = 0;
	bm_common->bmcb_dbclist.sth_stq.stq_prev = dbc_size - 1;
	bm_common->bmcb_dbclist.sth_count = 0;
	bm_common->bmcb_dbclist.sth_lcount = 0;
    }

    /*
    ** Initialize the table cache locks.
    */
    bm_common->bmcb_tblclist.sth_stq.stq_next = BUF_ENDLIST;
    bm_common->bmcb_tblclist.sth_stq.stq_prev = BUF_ENDLIST;
    bm_common->bmcb_tblclist.sth_count = 0;
    bm_common->bmcb_tblclist.sth_lcount = 0;
    if (dm0p_minit(&bm_common->bmcb_tblclist.sth_mutex, "BM tblclist", sem_type))
	return (E_DB_FATAL);

    if (tbc_size)
    {
	for (i = 0; i < tbc_size; i++)
	{
	    lbmcb->lbm_tblcache[i].bmc_sts = BMC_FREE;
	    lbmcb->lbm_tblcache[i].bmc_index = i;
	    lbmcb->lbm_tblcache[i].bmc_refcount = 0;
	    lbmcb->lbm_tblcache[i].bmc_state.stq_next = i + 1;
	    lbmcb->lbm_tblcache[i].bmc_state.stq_prev = i - 1;
	    if (dm0p_minit(&lbmcb->lbm_tblcache[i].bmc_mutex,
			STprintf(sem_name, "BM tblcache %d", i),
			sem_type))
		return(E_DB_FATAL);
	}
	lbmcb->lbm_tblcache[0].bmc_state.stq_prev = BUF_ENDLIST;
	lbmcb->lbm_tblcache[tbc_size-1].bmc_state.stq_next = BUF_ENDLIST;
	bm_common->bmcb_tblclist.sth_stq.stq_next = 0;
	bm_common->bmcb_tblclist.sth_stq.stq_prev = tbc_size - 1;
	bm_common->bmcb_tblclist.sth_count = 0;
	bm_common->bmcb_tblclist.sth_lcount = 0;
    }

    /*	Set the default priority boost information. */

    bm_common->bmcb_reference[BMCB_FHDR] = 6;
    bm_common->bmcb_reference[BMCB_FMAP] = 5;
    bm_common->bmcb_reference[BMCB_ROOT] = 4;
    bm_common->bmcb_reference[BMCB_INDEX] = 3;
    bm_common->bmcb_reference[BMCB_LEAF] = 2;
    bm_common->bmcb_reference[BMCB_DATA] = 1;

    bm_common->bmcb_rereference[BMCB_FHDR] = 1;
    bm_common->bmcb_rereference[BMCB_FMAP] = 1;
    bm_common->bmcb_rereference[BMCB_ROOT] = 1;
    bm_common->bmcb_rereference[BMCB_INDEX] = 1;
    bm_common->bmcb_rereference[BMCB_LEAF] = 1;
    bm_common->bmcb_rereference[BMCB_DATA] = 1;

    bm_common->bmcb_maximum[BMCB_FHDR] = 7;
    bm_common->bmcb_maximum[BMCB_FMAP] = 7;
    bm_common->bmcb_maximum[BMCB_ROOT] = 7;
    bm_common->bmcb_maximum[BMCB_INDEX] = 6;
    bm_common->bmcb_maximum[BMCB_LEAF] = 5;
    bm_common->bmcb_maximum[BMCB_DATA] = 4;

    /*
    ** Initialize the transaction buckets.
    */
    for (i = 0; i < BMCB_TRAN_MAX; i++)
    {
	bm_common->bmcb_txnq_buckets[i].blqh_bid.bid_index = BUF_ENDLIST;
	if (dm0p_minit(&bm_common->bmcb_txnq_buckets[i].blqh_mutex,
		    STprintf(sem_name, "BM txn queue %d", i),
		    sem_type))
	    return(E_DB_FATAL);
    }

    /*
    ** Initialize the table buckets.
    */
    for (i = 0; i < BMCB_TABLE_MAX; i++)
    {
	bm_common->bmcb_tabq_buckets[i].blqh_bid.bid_index = BUF_ENDLIST;
	if (dm0p_minit(&bm_common->bmcb_tabq_buckets[i].blqh_mutex,
		    STprintf(sem_name, "BM table queue %d", i),
		    sem_type))
	    return(E_DB_FATAL);
    }

    /*
    ** Initialize pool specific control blocks DM0P_BMCB(s)
    */
    for (cache_ix = 0, pgsize = 2048; cache_ix < DM_MAX_CACHE;
		cache_ix++, pgsize *=2)
    {
	if (!(bm = bmcb_ptrs[cache_ix]))
	    continue;

	bm->bm_status = BM_ALLOCATED;
	bm->bm_cache_ix = cache_ix;
	bm->bm_pgsize = pgsize;
	bm->bm_hshcnt = bcnt[cache_ix] - 1;
	bm->bm_bufcnt = buffers[cache_ix] + rpages[cache_ix];
	bm->bm_first  = bm_common->bmcb_totbufcnt;
	bm_common->bmcb_totbufcnt += bm->bm_bufcnt;
	bm->bm_sbufcnt = buffers[cache_ix];
	bm->bm_gcnt = group[cache_ix];
	bm->bm_gpages = gpages[cache_ix];
	bm->bm_flimit = free_limit[cache_ix];
	bm->bm_mlimit = modify_limit[cache_ix];
	bm->bm_wbstart = wb_start[cache_ix];
	bm->bm_wbend = wb_end[cache_ix];
	bm->bm_cpbufs = 0;
	bm->bm_cpindex = 0;
	bm->bm_cpagents = 0;
	bm->bm_cpabufs = 0;
	bm->bm_cfa_mask = 0;
	/* Clear all those stats */
	MEfill(sizeof(bm->bm_stats), 0, &bm->bm_stats);
	bm->bm_stats.bs_pgsize = pgsize;

	/*
	** Initialize the WriteBehind thread variables.
	*/
	/* Don't let flimit exceed 90% of cache */
	if ( bm->bm_flimit > ((bm->bm_sbufcnt * 90) / 100) )
	    bm->bm_flimit = ((bm->bm_sbufcnt * 90) / 100 );
	bm->bm_wb_optim_free = bm->bm_flimit + ((bm->bm_sbufcnt * 5) / 100);

	/* Is WriteBehind to be used in this cache? */
	if (writebehind[cache_ix])
	    bm->bm_status |= BM_WB_IN_CACHE;

	if (bm->bm_gcnt)
	{
	    /*
	    ** Set group WriteBehind start to 3/4 of groups,
	    **			       end to 1/2 of groups.
	    */
	    bm->bm_gwbstart = (bm->bm_gcnt / 4) * 3;
	    bm->bm_gwbend = bm->bm_gcnt / 2;
	}
	else
	{
	    bm->bm_gwbstart = 0;
	    bm->bm_gwbend = 0;
	}

	if (dm0p_minit(&bm->bm_mutex,
			STprintf(sem_name, "BM Mutex (%d)", pgsize),
			sem_type))
	    return(E_DB_FATAL);

	/*
	** Initialize the clock mutex.
	*/
	bm->bm_clock = bm->bm_sbufcnt;
	if (dm0p_minit(&bm->bm_clock_mutex,
			STprintf(sem_name, "BM clock (%d)", pgsize),
			sem_type))
	    return(E_DB_FATAL);

	/*
	** Initialize the group state queues.
	*/
	if (rpages[cache_ix])
	    bm->bm_status |= BM_GROUP;

	for ( i = 0; i < DB_MAX_TABLEPRI; i++ )
	{
	    bm->bm_gfq[i].sth_stq.stq_next = BUF_ENDLIST;
	    bm->bm_gfq[i].sth_stq.stq_prev = BUF_ENDLIST;
	    bm->bm_gfq[i].sth_count = 0;
	    bm->bm_gfq[i].sth_lcount = 0;
	    bm->bm_gmq[i].sth_stq.stq_next = BUF_ENDLIST;
	    bm->bm_gmq[i].sth_stq.stq_prev = BUF_ENDLIST;
	    bm->bm_gmq[i].sth_count = 0;
	    bm->bm_gmq[i].sth_lcount = 0;

	    /* Don't waste mutexes if no groups in this cache */
	    if ( bm->bm_status & BM_GROUP )
	    {
		if ( (dm0p_minit(&bm->bm_gfq[i].sth_mutex,
			    STprintf(sem_name, "BM gfq (%d.%d)", pgsize, i),
			    sem_type))
		|| (dm0p_minit(&bm->bm_gmq[i].sth_mutex,
			    STprintf(sem_name, "BM gmq (%d.%d)", pgsize, i),
			    sem_type)) )
		    return(E_DB_FATAL);
	    }
	}


	/*
	** Initialize the free and modified buffer priority queues.
	*/
	for (i = 0; i < DB_MAX_TABLEPRI; i++)
	{
	    bm->bm_fpq[i].sth_stq.stq_next = BUF_ENDLIST;
	    bm->bm_fpq[i].sth_stq.stq_prev = BUF_ENDLIST;
	    bm->bm_fpq[i].sth_count = 0;
	    bm->bm_fpq[i].sth_lcount = 0;
	    if (dm0p_minit(&bm->bm_fpq[i].sth_mutex,
			STprintf(sem_name, "BM fpq (%d.%d)", pgsize, i),
			sem_type))
		return(E_DB_FATAL);
	    bm->bm_mpq[i].sth_stq.stq_next = BUF_ENDLIST;
	    bm->bm_mpq[i].sth_stq.stq_prev = BUF_ENDLIST;
	    bm->bm_mpq[i].sth_count = 0;
	    bm->bm_mpq[i].sth_lcount = 0;
	    if (dm0p_minit(&bm->bm_mpq[i].sth_mutex,
			STprintf(sem_name, "BM mpq (%d.%d)", pgsize, i),
			sem_type))
		return(E_DB_FATAL);
	}

	/*
	** Initialize the hash buckets.
	*/
	bmh = lbmcb->lbm_buckets[cache_ix];
	for (i = 0; i <= bm->bm_hshcnt; i++)
	{
	    bmh[i].hash_next = BUF_ENDLIST;
	    if (dm0p_minit(&bmh[i].hash_mutex,
			STprintf(sem_name, "BM hash (%d.%d)", pgsize, i),
			sem_type))
	    return(E_DB_FATAL);
	}

	/*
	** Initialize the buffer descriptors.
	*/
	buffer = lbmcb->lbm_buffers[cache_ix];
	for (i = 0; i < bm->bm_sbufcnt; i++)
	{
	    buffer[i].buf_sts = BUF_FREE;
	    buffer[i].buf_owner = BUF_SELF;
	    buffer[i].buf_txnq.blq_next.bid_index = BUF_ENDLIST;
	    buffer[i].buf_txnq.blq_prev.bid_index = BUF_ENDLIST;
	    buffer[i].buf_txnq_bucket = BUF_NOBUCKET;
	    buffer[i].buf_tran_id.db_high_tran = 0;
	    buffer[i].buf_tran_id.db_low_tran = 0;
	    buffer[i].buf_tabq.blq_next.bid_index = BUF_ENDLIST;
	    buffer[i].buf_tabq.blq_prev.bid_index = BUF_ENDLIST;
	    buffer[i].buf_tabq_bucket = BUF_NOBUCKET;
	    buffer[i].buf_tabid.db_tab_base = 0;
	    buffer[i].buf_tabid.db_tab_index = 0;
	    buffer[i].buf_dbid = 0;
	    buffer[i].buf_type = 0;
	    buffer[i].buf_priority = 0;
	    buffer[i].buf_refcount = 0;
	    buffer[i].buf_cpcount = 0;
	    buffer[i].buf_tcb_stamp = 0;
	    buffer[i].buf_pid = (PID)0;
	    /*
	    ** (ICL phil.p)
	    ** Initialise buf_lock_mode.
	    */
	    buffer[i].buf_lock_mode = LK_N;

	    /* Initialize the buffer mutex */
	    if (dm0p_minit(&buffer[i].buf_mutex,
			STprintf(sem_name, "BM buffer (%d.%d)", pgsize, i),
			sem_type))
		return(E_DB_FATAL);

	    /* Point buffer to appropriate page in lbm_pages array */
	    buffer[i].buf_id.bid_index = i;
	    buffer[i].buf_id.bid_cache = cache_ix;
	    buffer[i].buf_page_offset = pgsize * i;
	    buffer[i].buf_relpgtype = TCB_PG_INVALID;

	    /* Initialize hash queue pointers */
	    buffer[i].buf_next = BUF_ENDLIST;
	    buffer[i].buf_prev = BUF_ENDLIST;
	    buffer[i].buf_hash_bucket = BUF_NOBUCKET;

	    /*  Queue buffer to the free priority queue at priority 0 */
	    next_buf = bm->bm_fpq[0].sth_stq.stq_next;
	    buffer[i].buf_state.stq_next = next_buf;
	    buffer[i].buf_state.stq_prev = BUF_ENDLIST;
	    if (next_buf != BUF_ENDLIST)
		buffer[next_buf].buf_state.stq_prev = i;
	    else
		bm->bm_fpq[0].sth_stq.stq_prev = i;
	    bm->bm_fpq[0].sth_stq.stq_next = i;
	    /* Count another free buffer */
	    bm->bm_fpq[0].sth_count++;
	}

	/*
	** Initialize the groups descriptors and the individual buffer
	** descriptors that make up the group.
	*/

	if (rpages[cache_ix])
	{
	    i = bm->bm_sbufcnt;
	    groups = lbmcb->lbm_groups[cache_ix];
	    buffer = lbmcb->lbm_buffers[cache_ix];
	    for (j = 0; j < bm->bm_gcnt; j++)
	    {
		groups[j].g_sts = G_FREE;
		groups[j].g_buffer = i;
		groups[j].g_index = j;
		groups[j].g_refcount = 0;
		groups[j].g_modcount = 0;
		groups[j].g_valcount = 0;
		groups[j].g_new_pri = 0;
		groups[j].g_priority = 0;
		groups[j].g_pages = 0;
		groups[j].g_pgsize = pgsize;
		groups[j].g_lock_list = 0;
		if (dm0p_minit(&groups[j].g_mutex,
			STprintf(sem_name, "BM group (%d.%d)", pgsize, j),
			sem_type))
		    return(E_DB_FATAL);

		for (k = 0; k < bm->bm_gpages; k++)
		{
		    /*  Connect buffer and page. */

		    buffer[i].buf_cpcount = 0;
		    buffer[i].buf_id.bid_index = i;
		    buffer[i].buf_id.bid_cache = cache_ix;
		    buffer[i].buf_page_offset = pgsize * i;
		    buffer[i].buf_relpgtype = TCB_PG_INVALID;
		    buffer[i].buf_sts = BUF_FREE;
		    buffer[i].buf_owner = j;
		    buffer[i].buf_next = BUF_NOBUCKET;
		    buffer[i].buf_prev = BUF_NOBUCKET;
		    buffer[i].buf_hash_bucket = BUF_NOBUCKET;
                    buffer[i].buf_tcb_stamp = 0;
		    buffer[i].buf_txnq.blq_next.bid_index = BUF_ENDLIST;
		    buffer[i].buf_txnq.blq_prev.bid_index = BUF_ENDLIST;
		    buffer[i].buf_txnq_bucket = BUF_NOBUCKET;
		    buffer[i].buf_tran_id.db_high_tran = 0;
		    buffer[i].buf_tran_id.db_low_tran = 0;
		    buffer[i].buf_tabq.blq_next.bid_index = BUF_ENDLIST;
		    buffer[i].buf_tabq.blq_prev.bid_index = BUF_ENDLIST;
		    buffer[i].buf_tabq_bucket = BUF_NOBUCKET;
		    buffer[i].buf_tabid.db_tab_base = 0;
		    buffer[i].buf_tabid.db_tab_index = 0;
		    buffer[i].buf_dbid = 0;
		    buffer[i].buf_type = 0;
		    buffer[i].buf_priority = 0;
		    buffer[i].buf_refcount = 0;
		    buffer[i].buf_lock_mode = LK_N;
		    if (dm0p_minit(&buffer[i].buf_mutex,
			    STprintf(sem_name, "BM group (%d.%d) buf %d", pgsize, j, i),
			    sem_type))
			return(E_DB_FATAL);
		    i++;
		}

		/*  Queue all groups to the priority 0 free group queue. */
		next_group = bm->bm_gfq[0].sth_stq.stq_next;
		groups[j].g_state.stq_next = next_group;
		groups[j].g_state.stq_prev = BUF_ENDLIST;
		if (next_group != BUF_ENDLIST)
		    groups[next_group].g_state.stq_prev = j;
		else
		    bm->bm_gfq[0].sth_stq.stq_prev = j;
		bm->bm_gfq[0].sth_stq.stq_next = j;

		/*  Increment priority count of free groups. */
		bm->bm_gfq[0].sth_count++;
	    }
	}
    }

    /*
    ** If we are initializing a shared memory segment for a Multi-Server
    ** Buffer Manager, then we should release the buffer manager lock so that
    ** other servers can now connect to it.  Just convert it to NULL since all
    ** servers connected to the buffer manager should hold this lock.
    **
    ** Set the lock value so that subsequent servers that request the lock
    ** will see that they are not the only holder of it.
    **
    ** Also need to do a SHOW on the buffer manager lock list to get the
    ** lock list key so that other servers may connect to this same
    ** lock list.
    */
    if (shrbuf_init)
    {
	LK_LLB_INFO	info_block;
	u_i4		result_size;

	lock_value.lk_low = BM_INIT_VALUE;
	cl_status = LKrequest(
	    LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK,
	    lock_list, (LK_LOCK_KEY *)NULL, LK_N, &lock_value,
	    &bm_lockid, (i4)0, &sys_err);
	if (cl_status != OK)
	{
	    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
	    ule_format(E_DM901C_BAD_LOCK_REQUEST, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 2,
		0, LK_N, 0, lock_list);
	    ule_format(E_DM937C_BM_BUFMGR_LOCK_ERROR, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
	    *err_code = E_DM930A_DM0P_ALLOCATE;
	    return (E_DB_FATAL);
	}

	/*
	** Show the buffer manager lock list to get the lock key.  This
	** key will be used by other servers to connect to the shared
	** lock list.
	*/
	cl_status = LKshow(LK_S_LIST, lbmcb->lbm_lock_list, (LK_LKID *)NULL,
	    (LK_LOCK_KEY *)0, sizeof(info_block), (PTR)&info_block,
	    &result_size, (u_i4 *)NULL, &sys_err);
	if (cl_status != OK)
	{
	    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
	    ule_format(E_DM901D_BAD_LOCK_SHOW, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 1,
		0, lbmcb->lbm_lock_list);
	    ule_format(E_DM937C_BM_BUFMGR_LOCK_ERROR, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
	    *err_code = E_DM930A_DM0P_ALLOCATE;
	    return (E_DB_FATAL);
	}

	bm_common->bmcb_lock_key.lk_uhigh = info_block.llb_key[0];
	bm_common->bmcb_lock_key.lk_ulow = info_block.llb_key[1];
    }

    /* Link the BMSCB to the SVCB. */
    dmf_svcb->svcb_lbmcb_ptr = lbmcb;
    for (segment_ix = 0; segment_ix < DM_MAX_BUF_SEGMENT; segment_ix++)
	dmf_svcb->svcb_bmseg[segment_ix] = bmsegs[segment_ix];

    return (OK);
}

/*{
** Name: dm0p_deallocate	- Deallocate the buffer manager.
**
** Description:
**	This routine deallocates the buffer manager control block, and
**	invalidates the server control block pointer to the BMSCB(s).If there is
**	no BMSCB then this routine returns success.
**
**	If the Buffer Manager is allocated in local memory, then deallocate the
**	BMCB and LBMCB.  If this is a shared buffer manager, then just
**	deallocate the LBMCB.
**
**	If this is a shared buffer manager, convert the buffer manager lock to
**	exclusive and decrement the connected server count and release the lock.
**
**	Also release the buffer manager lock list.  This lock list can be
**	shared by other servers if this is a shared buffer manager.  The locks
**	will only be released if this is the only server connected to the lock
**	list.
**
**	If this is the last server connected to the buffer manager, and there
**	are still fixed or modified pages, then signal an error.
**
** Inputs:
**
** Outputs:
**      err_code                        Reason for FATAL status.
**					    E_DM930B_DM0P_DEALLOCATE
**					    E_DM9200_BM_BUSY
**	Returns:
**	    E_DB_OK
**	    E_DB_FATAL
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	15-oct-1985 (derek)
**          Created new for Jupiter.
**	28-feb-1989 (rogerk)
**	    Added support for Shared Buffer Manager.
**	 3-apr-1989 (rogerk)
**	    Pass MULTITHREAD flag when release buffer manager lock list.
**	    Don't decrement server count until assured we can disconnect from
**	    buffer manager successfully.
**	26-jun-1989 (rogerk)
**	    Destroy shared memory segment when last server disconnects rather
**	    than assuming it will be destroyed by the OS.
**	26-oct-92 (rogerk)
**	    Reduced Logging Project:
**	      - Added deallocation of the new buffer manager transaction context
**		used by force_page to force the log file before writing pages.
**	      - Clean up buffer manager semaphore to protect usage of the
**		above transaction context.
**	25-feb-1993 (keving)
**	    => A Buffer Manager may be associated with many logs. We deallocate
**	    => the txid of each log we have.
**	18-oct-1993 (rachael) merged 25-mar-1993 (bryanp) B47950,B50252
**	    Just adding a comment here to assert that we do not need to check
**	    for LK_VAL_NOTVALID when converting the buffer manager lock up to
**	    LK_X. If we do get LK_VAL_NOTVALID, then we treat it as a fatal
**	    error and crash the server. There should be no way that we can
**	    ever get LK_VAL_NOTVALID for this lock request unless some other
**	    server which is connected to this cache has crashed while holding
**	    the buffer manager lock, in which case the cache is suspect and
**	    crashing this server is an appropriate thing to do.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**      01-aug-1996 (nanpr01 for ICL phil.p)
**          Added call to dmcm_cache_flush to flush the cache of pages
**          locked by this server prior to server shutdown.
**      29-Sep-2004 (fanch01)
**          Add LK_LOCAL flag support as buffer names are confined to one node.
*/
DB_STATUS
dm0p_deallocate(i4		    *err_code)
{
    DMP_LBMCB		*lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON      *bm_common;
    DM0P_BMCB           *bm;
    LK_LLID		lock_list = dmf_svcb->svcb_lock_list;
    LK_VALUE		lock_value;
    STATUS		status = E_DB_OK;
    CL_SYS_ERR		sys_err;
    LK_RSB_INFO		info_block;
    u_i4		result_size;
    i4			name_index;
    i4		i;
    bool		shared_cache;
    char		name_buf[CACHENM_MAX + 8];
    i4                 cache_ix;
    i4                 segment_ix;
    DMP_BMSCB           *bmsegp;

    /* If not allocated, return success. */
    if (lbm == 0)
	return (E_DB_OK);

    bm_common = lbm->lbm_bm_common;
    shared_cache = bm_common->bmcb_status & BMCB_SHARED_BUFMGR;

    /*
    ** It would be nice if we could guarantee here that we hold
    ** no page, buffer, or bmcb mutex's.  Currently we have very
    ** little fault tolerance - if any server connected to this
    ** buffer manager goes down abnormally, all other servers connected
    ** to it will be killed.
    */

    /*
    ** Get Buffer Manager Lock in order to deallocate buffer manager
    ** in case another server tries to connect or disconnect from
    ** this buffer manager at the same time.
    */
    if (shared_cache)
    {
	status = LKrequest(
	    LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK,
	    lock_list, (LK_LOCK_KEY *)NULL, LK_X, &lock_value,
	    &lbm->lbm_bm_lockid, (i4)0, &sys_err);
	if (status != OK)
	{
	    ule_format(status, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
	    ule_format(E_DM901C_BAD_LOCK_REQUEST, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 2,
		0, LK_X, 0, lock_list);
	    ule_format(E_DM937C_BM_BUFMGR_LOCK_ERROR, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
	    *err_code = E_DM930B_DM0P_DEALLOCATE;
	    return (E_DB_FATAL);
	}
    }

    /*
    ** (ICL phil.p)
    ** Flush the cache of pages locked by this DMCM server.
    */
    if (bm_common->bmcb_status & BMCB_DMCM)
    {
        status = dmcm_cache_flush(err_code); 
        if (status != E_DB_OK)
            return(status);
    }

    /*
    ** Decrement count of servers connected to this buffer manager.
    ** If this is the last connection to this server, then there should
    ** be no modified or fixed pages or groups.
    ** If there are then it could be that our counts are off (this has
    ** happened in the past) - or we could have left some modified pages
    ** around in the cache - this would be bad.  We may be throwing away
    ** committed changes and probably REDO recovery is in order.
    */
    if ( --bm_common->bmcb_srv_count <= 0 )
    {
	for (cache_ix = 0; cache_ix < DM_MAX_CACHE; cache_ix++)
	{
	    if ( (bm = lbm->lbm_bmcb[cache_ix] ) && bm->bm_status & BM_ALLOCATED )
	    {
		i4	fcount,mcount,lcount,gfcount,gmcount,glcount;

		DM0P_COUNT_ALL_QUEUES(fcount,mcount,lcount);
		DM0P_COUNT_ALL_GQUEUES(gfcount,gmcount,glcount);

		if ( lcount || mcount || glcount || gmcount )
		{
		    TRdisplay("%@ dm0p_deallocate: cache %d lcount %d mcount %d glcount %d gmcount %d!\n",
			cache_ix, lcount, mcount, glcount, gmcount);

		    status = E_DB_FATAL;
		}
	    }
	}

	if ( status )
	{
	    *err_code = E_DM9200_BM_BUSY;
	    return(status);
	}

	/*
	** If this is the last connection to a shared cache, then destroy
	** the memory segment.  We are protected from having another server
	** try to connect to the segment while we are destroying it by holding
	** the buffer manager lock in Exclusive mode.
	*/
	if ( shared_cache )
	{
	    /*
	    ** Call LKshow to verify the number of users connected to the buffer
	    ** manager.  There should only be one.
	    */
	    status = LKshow(LK_S_SRESOURCE, (LK_LLID)0, &lbm->lbm_bm_lockid,
		(LK_LOCK_KEY *)0, sizeof(info_block), (PTR)&info_block,
		&result_size, (u_i4 *)NULL, &sys_err);
	    if (status != OK)
	    {
		ule_format(status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 0);
		ule_format(E_DM901D_BAD_LOCK_SHOW, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 1,
		    0, lbm->lbm_bm_lockid);
		ule_format(E_DM937C_BM_BUFMGR_LOCK_ERROR, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 0);
	    }
	    else if ( info_block.rsb_count == 1 )
	    {
		for (segment_ix = 0; 
		     segment_ix < DM_MAX_BUF_SEGMENT &&
			 (bmsegp = dmf_svcb->svcb_bmseg[segment_ix]);
		     segment_ix++)
		{
		    /*
		    ** Build shared memory key from the user supplied cache name.  A
		    ** shared memory key looks like an Ingres filename.  Use the 
		    ** 8 characters of the cache name as the file prefix and append
		    **
		    **    ".dat" for the 0-th segment
		    **    ".dan" where "n" is the segment number (segment_ix) 
		    **		 for subsequent segments.
		    */
		    STncpy(name_buf, bmsegp->bms_name.db_name, CACHENM_MAX);
		    name_buf[ CACHENM_MAX ] = '\0';

		    /* Trim off blanks and add suffix. */
		    name_index = STtrmwhite(name_buf);
		    STcopy(CACHENAME_SUFFIX, name_buf + name_index);
		    if ( segment_ix )
			CVla(segment_ix, name_buf + name_index + sizeof(CACHENAME_SUFFIX) - 2);
		    name_index = STtrmwhite(name_buf);
		    name_buf[name_index] = '\0';
		    CVlower(name_buf);

		    /*
		    ** Ignore errors trying to destroy the shared segment.  On VMS,
		    ** MEsmdestroy may fail if the last server to exit is not 
		    ** running on the same processor as the one which allocated 
		    ** the cache.
		    **
		    ** If this call fails, then hopefully the OS will free the 
		    ** segment for us since we created it with the ME_NOPERM mask.
		    ** If the OS system does not support ME_NOPERM, then the cache
		    ** will be deleted the next time a server starts up with this 
		    ** same cache.
		    */
		    CS_cp_sem_cleanup(name_buf, &sys_err);
		    (VOID) MEsmdestroy(name_buf, &sys_err);

		    dmf_svcb->svcb_bmseg[segment_ix] = (DMP_BMSCB*)NULL;
		}
	    }
	}
	else /* Deallocate non-shared cache segments */
	{
	    for (segment_ix = 0; segment_ix < DM_MAX_BUF_SEGMENT; segment_ix++)
	    {
		if ( dmf_svcb->svcb_bmseg[segment_ix] )
		    dm0m_deallocate((DM_OBJECT **)&dmf_svcb->svcb_bmseg[segment_ix]);
	    }
	}
    }

    /*
    ** Release all buffer manager's transaction context used for forcing the
    ** log file when writing buffers.
    */
    for (i=0; i < lbm->lbm_num_logs; i++)
    {
	if (lbm->lbm_bm_lxid[i])
	{
	    status = LGend(lbm->lbm_bm_lxid[i], 0, &sys_err);
	    if (status != OK)
	    {
		ule_format(status, &sys_err, ULE_LOG, NULL, 
		    (char *)0, (i4)0, (i4 *)0, err_code, 0);
		ule_format(E_DM900E_BAD_LOG_END, &sys_err, ULE_LOG, NULL, 
		    (char *)0, (i4)0, (i4 *)0, err_code, 1,
		    0, lbm->lbm_bm_lxid[i]);
	    }
	}
    }
    if (lbm->lbm_bm_dbid)
    {
	status = LGremove(lbm->lbm_bm_dbid, &sys_err);
	if (status != OK)
	{
	    ule_format(status, &sys_err, ULE_LOG, NULL, 
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
	    ule_format(E_DM9016_BAD_LOG_REMOVE, &sys_err, ULE_LOG, NULL, 
		(char *)0, (i4)0, (i4 *)0, err_code, 1,
		0, lbm->lbm_bm_dbid);
	}
    }

    /*
    ** Release the buffer manager lock list.  This list contains all the
    ** cache locks for this buffer manager.  This lock list is shared by
    ** other servers as well.  If another server is connected to the same
    ** lock list, then no locks will actually be released.  If this server is
    ** the last holder of this lock list then the locks will all be released.
    **
    ** If an error occurs, just continue as the LK rundown code should clean
    ** up for us.
    */
    status = LKrelease(LK_ALL, lbm->lbm_lock_list,
	(LK_LKID *)0, (LK_LOCK_KEY *)0, (LK_VALUE *)0, &sys_err);
    if (status != OK)
    {
	ule_format(status, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
	ule_format(E_DM901B_BAD_LOCK_RELEASE, &sys_err, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, err_code, 1,
	    0, lbm->lbm_lock_list);
	ule_format(E_DM9377_BM_LOCK_RELEASE, &sys_err, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, err_code, 1,
	    0, lbm->lbm_lock_list);
    }

    /*
    ** Release the Buffer Manager Lock.
    ** If this is last connection to this buffer manager, then the next server
    ** to request this same lock will see that it is the only holder and will
    ** reinitialize the cache.
    */
    if (shared_cache)
    {
	status = LKrelease(0, lock_list, &lbm->lbm_bm_lockid,
	    (LK_LOCK_KEY *)NULL, &lock_value, &sys_err);
	if (status != OK)
	{
	    ule_format(status, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
	    ule_format(E_DM901B_BAD_LOCK_RELEASE, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 1,
		0, lock_list);
	    ule_format(E_DM937C_BM_BUFMGR_LOCK_ERROR, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
	}
    }

    /* Deallocate the local buffer manager control block. */
    dm0m_deallocate((DM_OBJECT **)&dmf_svcb->svcb_lbmcb_ptr);

    return (E_DB_OK);
}

/*{
** Name: dm0p_register_loghandles   - Adds a set of logs to the Buffer Manager.
**
** Description:
**	Once a Buffer Manager has been allocated it has a reference to and
**	context for its local log. This routine is used by VAX recovery. It
**	allows the CSP to register a set of extra logs with the buffer manager.
**	Once this has been done the buffer manager will force all logs when
**	throwing out modified pages.
**
**	Should allow us to incrementally add logs as required, but this
**	isn't tested.
**
** Inputs:
**	lctx_array			Pointer to array of log contexts.
**	num_logs			Number of log contexts passed.
**
** Outputs:
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**	Exceptions:
**	    none
**
** Side Effects:
**	    Adds default lxids for each log to lbmcb.
**
** History:
**	2-Mar-1993 (keving)
**          Created for 6.5
**	26-apr-1993 (keving)
**	    Instead of using the lxids from the lctx_array we now start
**	    new log transactions for the buffer manager.
**	20-sep-1993 (andys)
**	    Fix arguments to ule_format call.
*/

DB_STATUS
dm0p_register_loghandles(
DMP_LCTX	    **lctx_array,
i4		    num_logs,
i4		    *err_code)
{
    DMP_LBMCB		*lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0L_ADDDB		add_info;
    i4			len_add_info;
    LG_LXID		bm_lxid;
    DB_TRAN_ID		tran_id;
    i4			i;
    i4			first_free;
    STATUS		cl_status;
    DB_STATUS		status = E_DB_OK;
    CL_SYS_ERR		sys_err;

    /* Check parameters are valid and that dm0p_allocate has already
    ** been called.
    */
  
    if ( !lbm || 
         num_logs < 0 ||
         ( (num_logs + lbm->lbm_num_logs) > DM_CNODE_MAX))
    {
	/* Could be a bit more helpful here */
	/* FIX ME */
	return (E_DB_ERROR);
    }

    first_free = lbm->lbm_num_logs;
    if (!lbm->lbm_bm_dbid)
    {
	/* Need to set up a default NOT_DB so that we can create the lxid */
	MEcopy((PTR)DB_BUFMGR_NAME, sizeof(add_info.ad_dbname),
	    (PTR) &add_info.ad_dbname);
	MEcopy((PTR)DB_INGRES_NAME, sizeof(add_info.ad_dbowner),
	    (PTR) &add_info.ad_dbowner);
	MEcopy((PTR)"None", 4, (PTR) &add_info.ad_root);
	add_info.ad_dbid = 0;
	add_info.ad_l_root = 4;
	len_add_info = sizeof(add_info) - sizeof(add_info.ad_root) + 4;

	cl_status = LGadd(lctx_array[0]->lctx_lgid, LG_NOTDB, (char *)&add_info, 
	    len_add_info, &lbm->lbm_bm_dbid, &sys_err);
	if (cl_status != OK)
	{
	    (VOID) ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		(char *)0, 0L, (i4 *)0, err_code, 0); 
	    (VOID) ule_format(E_DM900A_BAD_LOG_DBADD, &sys_err, ULE_LOG, NULL,
		(char *)0, 0L, (i4 *)0, err_code, 4, 
		0, lctx_array[0]->lctx_lgid,
		sizeof(add_info.ad_dbname), (PTR) &add_info.ad_dbname,
		sizeof(add_info.ad_dbowner), (PTR) &add_info.ad_dbowner,
		4, (PTR) &add_info.ad_root);
	    *err_code = E_DM930A_DM0P_ALLOCATE; /* FIX ME !!!!!!!!*/
	    if (cl_status == LG_EXCEED_LIMIT)
		*err_code = E_DM0062_TRAN_QUOTA_EXCEEDED;
	    status = E_DB_ERROR;
	}
    }
    if (status == E_DB_OK)
    {
	/* For each log we create a lxid and put it in the lbmcb array */
	for (i = 0; i < num_logs; i++)
	{
	    cl_status = LGbegin(LG_NOPROTECT, lbm->lbm_bm_dbid, &tran_id, 
		&bm_lxid, sizeof(DB_OWN_NAME), DB_BUFMGR_OWNER,
		(DB_DIS_TRAN_ID*)NULL, &sys_err);
	    if (cl_status != OK)
	    {
		(VOID) ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		    (char *)0, 0L, (i4 *)0, err_code, 0); 
		ule_format(E_DM900C_BAD_LOG_BEGIN, &sys_err, ULE_LOG, NULL, 
		    (char *)0, (i4)0, (i4 *)0, err_code, 1, 
		    0, lbm->lbm_bm_dbid);
		*err_code = E_DM930A_DM0P_ALLOCATE;  /* FIX ME !!!!!!!!!!!1 */
		if (cl_status == LG_EXCEED_LIMIT)
		    *err_code = E_DM0062_TRAN_QUOTA_EXCEEDED;
		status = E_DB_ERROR;
		break;
	    }
	    lbm->lbm_bm_lxid[first_free + i] = bm_lxid;
	    lbm->lbm_num_logs++;
	}
	if (status)
	{ 
	    /* Remove the transactions we succeeded in creating */
	    for  (i = 0; i < (lbm->lbm_num_logs - first_free); i++)
	    {
		bm_lxid = lbm->lbm_bm_lxid[first_free + i];
		cl_status = LGend(bm_lxid, 0, &sys_err);
		if (cl_status != OK)
		{
		    ule_format(cl_status, &sys_err, ULE_LOG, NULL, 
			(char *)0, (i4)0, (i4 *)0, err_code, 0);
		    ule_format(E_DM900E_BAD_LOG_END, &sys_err, ULE_LOG, NULL, 
			(char *)0, (i4)0, (i4 *)0, err_code, 1,
			0, bm_lxid);
		}
	    }

	    if (lbm->lbm_bm_dbid)
	    {
		cl_status = LGremove(lbm->lbm_bm_dbid, &sys_err);
		if (cl_status != OK)
		{
		    ule_format(cl_status, &sys_err, ULE_LOG, NULL, 
			(char *)0, (i4)0, (i4 *)0, err_code, 0);
		    ule_format(E_DM9016_BAD_LOG_REMOVE, &sys_err, ULE_LOG, NULL, 
			(char *)0, (i4)0, (i4 *)0, err_code, 1,
			0, lbm->lbm_bm_dbid);
		}
	    }
	}
    }
    return (status);
}

/*{
** Name: dm0p_fix_page	- Assign a page to a transaction.
**
** Description:
**	The page requested by the caller is made valid in the buffer pool. 
**	The process of making a page valid involves validating a
**	page that is already in the buffer pool and/or reading the
**	page fresh from the disk.  The buffer manager sets page locks
**	on behalf of the transaction requesting the page.  It will
**	also sets server locks for cooperating buffer managers.
**	
**	When the caller asks to fix a page, it gives it's intended
**	access for the page either READ or WRITE.  If the request was
**	for a physical lock, for example, pages looked at during a
**	btree search, the lock request is made with a qualifier that 
**	allows the lock to be released before the transaction completes.
**	If the lock was previously requested without physical locking or
**	is requested in the future without physical locking, the lock is
**	made transaction. If CREATE is requested, then the page is not
**      read from disk. This is used for extending a file.
**
**	The MUSTLOCK qualifier is used to override READLOCK=NOLOCK setting.
**	Btree fix_page calls for non-leaf index pages use this option to
**	walk the tree.
**
**	The following diagram depicts flow of control:
**
**			   CALL
**			     |
**			XACT_LOCK_PAGE
**			     |---------------+
**			  ESCALATE           |
**		+----------->|<--------------+
**		|	   MUTEX
**		|  	     |
**		+------- FIX_PAGE  ----+
**              |   +--------|	       |
**		|   |	VALIDATE_PAGE -|
**		|   +--------|         |
**		+------- FAULT_PAGE    |
**			     |<--------+
**			  RETURN  
**
**	The when a page is fixed for WRITE the transaction id is
**      written to the page buffer.  When it is fixed for shared
**      no specific transaction owns the buffer page since multiple
**      transactions can have it fixed.  A shared page becomes
**      owned by the last transaction to unfix the page.  Fixed pages
**      that are owned by a specific transaction are kept on a 
**      transaction hash queue so that they may be easily found
**      when the transaction ends.  Unless Fast Commit is in use, all
**	pages that are owned by a transaction that have been modified
**      must be written to disk before the transaction is committed.
**      This hash queue reduces the search through all pages to see
**      if any pages belong to the transaction which is ending.
** 
**      Note, physical locks are always obtained for a table, 
**      even when table level locking is specified and the page
**      is wanted for READ.  This must be done since there is no
**      way in dm0p_unfix_page to determine if the lock needs
**      to be released.
**     
**      To solve numerous race conditions and error handling
**      problems in the multi-thread environment the following
**      rules must be obeyed:
**         o  All operations that wait must be handled outside
**            the buffer manager mutex.
**         o  The lock value must change everytime the page
**            is written.  The value lock is always in 
**            IS mode except when it is being written.  Then
**            it is in SIX mode.
**         o  Group buffers never appear on the free or modifed queue
**            of the single page buffers.
**         o  Group buffers are not aged and single buffers
**            are only aged when a page needs to be read.
**         o  Disk I/O errors on writes, mark the buffer such
**            that only a dm0p_tran_invalidate call can release them.  Thus 
**            the calling transaction must abort to release the buffers.
**
**      Dm0p_fix_page is called with the following fix actions:
**
**	    DM0P_READ		- fix page for read only - the page cannot
**				  be modified while fixed for read.
**	    DM0P_PHYSICAL	- request physical lock on page.  This is
**				  probably a system catalog page or a btree
**				  index page.
**	    DM0P_WRITE		- fix page for write - the page can be
**				  modified.
**	    DM0P_CREATE		- page just been allocated and has not been
**				  formatted on disk.  Read it in and format it.
**	    DM0P_CREATE_NOFMT	- page may have just been allocated. Read it in, but
**				  mutex it insted of formatting it. It's up to
**				  the caller to decide on formatting, and the
**				  caller's responsibility to unmutex the page.
**	    DM0P_NOREADAHEAD	- do not use read-ahead.  Caller is indicating
**				  that this is not a sequential scan.
**	    DM0P_LOCKREAD	- used for BTREE index pages to make sure that
**				  readlocks are aquired on index pages even
**				  when using read-nolock.
**	    DM0P_TEMPLOCK	- 
**	    DM0P_NOWAIT		- do not wait for page lock.  If page can't be
**				  locked, return an error.
**	    DM0P_NOLOCK		- Do not place a transaction lock on this page.
**
** Inputs:
**      rcb                             The RCB that wants the page.
**      page_number                     The page number to fix.
**      action                          The specific fix action.
**
** Outputs:
**      page                            The pointer to the fixed page.
**      err_code                        Reason for error return.
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**	    E_DB_FATAL
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	15-oct-1985 (derek)
**          Created for Jupiter.
**	22-Jan-1987 (ac)
**	    Added code for readahead support.
**	 5-oct-1988 (rogerk)
**	    Added DM0P_NOWAIT action.
**	 6-dec-1988 (rogerk)
**	    After fixing a page, turn off the BUF_CACHED bit that specifies
**	    that we must validate the page when fixing it.  We can't do this
**	    in validate_page() as we don't know whether the page has been
**	    locked or not.
**	28-feb-1989 (rogerk)
**	    Added support for Shared Buffer Manager.
**	    Took out resetting of BUF_TEMP_FIXED status as it is unset in
**	    fault_page.
**	17-mar-1989 (rogerk)
**	    Fixed read nolock bug introduced with shared buffer manager changes.
**	25-sep-1989 (rogerk)
**	    Fixed PASS_ABORT problem.  When faulting in readnolock buffer, check
**	    if system is in recovery state.  If it is, then toss the buffer out
**	    of the cache.
**	25-sep-1989 (rogerk)
**	    Make sure we put buffer on transaction list when fix it for WRITE
**	    even if the buffer is already fixed.  If the buffer is fixed only
**	    for READ then it will not be on the transaction list.  Moved code
**	    to buffer on xact queue out of 1st-fix-only code.
**	25-sep-1989 (rogerk)
**	    Don't lock pages in X mode if DM0P_RPHYS action is specified.  This
**	    causes us to lock BTREE index pages in X mode when table is open
**	    for write.
**	17-jul-1990 (mikem)
**	    bug #30833 - force aborts sometimes missed.
**	    It is possible, timing related, for a lock to be granted at exactly
**	    the same time as an interrupt (such as a FORCE ABORT notification) 
**	    is recieved.  LKrequest has been changed to return a new status
**	    (LK_INTR_GRANT) if this situation occurs.  In this case the lock
**	    has been granted but dmf's action is the same as if the lock request
**	    had been interrupted, with one exception.
**
**	    The exception is that any short term locks (such as core system 
**	    catalogue pages), which are usually released by dm0p_unfix_page() 
**	    following successful completion of dm0p_fix_page().  In this case
**	    we check for this magic return and make sure the page is unlocked
**	    before returning error from dm0p_fix_page().
**	 4-feb-1991 (rogerk)
**	    Added support for fixed buffer priorities.
**	    Don't adjust buffer priority on reference if it is marked as
**	    having a fixed priority.
**	6-jun-1991 (bryanp)
**	    Added message E_DM9C08_OUT_OF_LOCKS when lock quota is exceeded.
**	19-aug-1991 (rogerk)
**	    Changed checks for xact abort in escalate processing.  We do
**	    not want to escalate to table level locking during transaction
**	    recovery.  The old check thought that Set Nologging transactions
**	    were always in abort state.  Changed to actually lookup the
**	    transaction state before escalating.  Exception is when running
**	    as the RCP or CSP in which case we assume we are doing transaction
**	    recovery. 
**	17-oct-1991 (rogerk)
**	    Merged changes made by Derek during the file extend project.
**	    Added new flags DM0P_NOLOCK, DM0P_FHFMSYNC and DM0P_MUTEX.
**	    Added trace flags in xDEBUG code.
**	    Added new session statistics collecting.
**	    Added support for DM0P_CREATE flag on existing pages.
**	19-Nov-1991 (rmuth)
**	    Removed action DM0P_FHFMSYNC.
**	14-may-1992 (bryanp)
**	    B44260: rcb_lk_limit should not apply during transaction abort.
**	05_jun_1992 (kwatts)
**	    MPF change, call to format page now uses accessor.
**	26-oct-1992 (rogerk)
**	    Reduced Logging Project:
**	      - Changes to page locking logic:  decide on page lock mode based 
**		on the original requested lock mode at table open time rather 
**		than on the currently held lock mode.  The currently held lock 
**		mode may be IX due to a previous write query in the current 
**		transaction, but if this is a read query we want to do shared 
**		page locking. Also, dm2t now sets rcb_lk_type correctly to PAGE
**		or TABLE even when the granted lock mode is SIX, so that lock 
**		mode no longer needs to be taken into account when decided 
**		whether a page lock is needed.
**	      - Added check for (action & PHYSICAL) in the case when we release
**		a physical lock granted just before receiving an interrupt 
**		notice.  Previously we were checking for rcb_k_duration which 
**		would catch any system catalog page locks but would miss other 
**		physical requests like btree index pages.
**	      - Added before image check when fixing pages for update.
**	      - Moved page lock request into dm0p_lockpage routine.
**	      - Moved work which actually fixes pages into dm0p_cachefix_page.
**	      - Changed DM0P_SCRATCH handling.  Newly allocated pages will not
**		be marked MODIFIED when formatted as a new page.  This prevents
**		background processes from writing the new page to the database
**		before the callers updates are written to the page.
**	      - Removed DM0P_MUTEX option.  Pages must now be mutexed explicitly
**		by callers while updating them and the buffer manager will no
**		mark newly allocated pages as DIRTY.
**	18-feb-1993 (rmuth)
**	    If encounter an error calling dm0p_cachefix_page use 
**	    dm0p_log_error as this routine understands which errors to
**	    preserve and pass upto the higher layers.
**	26-apr-1993 (bryanp)
**	    6.5 Cluster Project:
**		It's now illegal to fix more than one system catalog page for
**		    write at a time. Enforce this restriction here.
**		Took out special CSP page locking semantics. The page locking
**		    behavior of CSP recovery is now handled directly by the
**		    recovery code and by the locking system.
**	26-jul-1993 (rogerk)
**	    Change generation of Before Image log records during Online Backup.
**	    Instead of calling before_image_check on every dm0p_fixpage 
**	    operation, we now expect the modifier of the page to call 
**	    dm0p_before_image_check before changing its contents.
**	23-may-1994 (andys)
**	    Add new parameter to dm0p_log_error which says whether to print
**	    the error which we are throwing away. We pass the flag saying not 
**	    to log the error when the flag DM0P_USER_TID is passed to 
**	    dm0p_fix_page, dm0p_cachefix_page or fault_page. This flag is
**	    only currently set by dm1r_get when it is doing a get by tid.
**	    [bug 49726]
**	19-apr-1995 (cohmi01)
**	    When running in 'batch mode', always use READAHEAD.
**	20-jun-95 (lewda02)
**	    Make DM0P_LOCKREAD override some other page lock options.
**	17-jul-1995 (cohmi01)
**	    Backout lewda02 DM0P_LOCKREAD change - causes page locks
**	    when not needed - eg when entire table is already locked,
**	    and thwarts the intent of other rcb flags related to locking.
**	    For Readahead thread - check for rcb interrupt  indicator
**	    so that cancelled/stale requests incur no extra IO. This rcb_uiptr
**	    actually points into the prefetch request block and will be set
**	    by user thread even though we execute on behalf of readahead thr.
**	30-may-1996 (pchang)
**	    Added support for DM0P_NOESCALATE action, to be used by sessions
**	    already holding an X lock on the FHDR when fixing pages other than
**	    an FMAP (i.e. without the DM0P_NOLOCK action).  Lock escalation 
**	    under such condition may lead to unrecoverable deadlock involving 
**	    the table-level lock and the FHDR. (B76463) 
**	    Also, changed to display the E_DM9041_LK_ESCALATE_TABLE message 
**	    only prior to actual lock escalation.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	4-jul-96 (pchang)
**	    More changes to escalation handling.
**	17-jul-96 (nick)
**	    Differentiate between an LK resource derived escalation and one
**	    caused by exceeding the RCB lock limit. #77808
**	22-jul-96 (nick)
**	    Move some code out of here so we can use it elsewhere.
**      22-nov-96 (dilma04, stial01)
**          Row Locking Project:
**          - simplify access_mode calculation;
**          - call dm0p_page_lock_mode to figure out the lock mode;
**          - latch the page when row locking and no phantom protection.
**	16-Mar-1997 (jenjo02)
**	    Don't use NOREADLOCK buffer if the action is DM0P_WRITE. Read_only
**	    transactions are allowed to fix, mutex, modify, and write temporary
**	    tables.
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          - added lock_id argument to dm0p_lock_page(), dm0p_unlock_page()
**          and dm0p_trans_page_lock() calls;  
**          - implemented lock coupling for CS and RR isolation levels.
**      21-may-1997 (stial01)
**          dm0p_fix_page: Remove request for LK_PH_PAGE lock
**      29-may-1997 (stial01)
**          Rename DM0P_IX_LOCK -> DM0P_INTENT_LK, BUF_IX_LOCK -> BUF_INTENT_LK
**          dm0p_fix_page: If IS/IX/SIX page lock, action |= DM0P_INTENT_LK
**      12-nov-97 (stial01)
**          Pass lockKEY even if unlocking with lockID (for set lock_trace)
**	02-jun-1998 (kitch01)
**	    Bug 90627. Ensure lock_id structure is initialized. It is possible 
**	    that a transaction lock is not required but during lock coupling for
**	    CS and RR isolation levels we assume lock_id contains the lockid of 
**	    the current page.		
**      07-jul-98 (stial01)
**          If RCB_CS_ROW, unlock row before releasing associated pg lock B90649
**	19-Aug-1998 (jenjo02)
**	    In dm0p_fix_page(), return lock id into rcb_fix_lkid.
**      18-Sep-2003 (jenjo02) SIR 110923
**          Force group reads for blobs unless explicitly negated.
**	22-Jan-2004 (jenjo02)
**	    dm1r_unlock_row() now takes (LK_LOCK_KEY*) instead of
**	    (DM_TID*).
**	11-aug-2004 (thaju02)
**	    For online modify of btree record lsn regardless of whether
**	    pg lsn is less than bsf lsn. (B112790)
**	21-jan-2005 (thaju02)
**	    rnl_fixpage even if pg lsn is zero.
**	8-Feb-2006 (kschendel)
**	    Remove extra parm to dm1r-unlock-row, passed null as errcode
**	    pointer and caused segv's when lock release failed.  Include
**	    dm1r header.
*/
DB_STATUS
dm0p_fix_page(
DMP_RCB            *rcb,
DM_PAGENO          page_number,
i4		   action,
DMPP_PAGE          **page,
i4            *err_code)
{
    DMP_RCB		*r = rcb;
    DMP_TCB		*t = r->rcb_tcb_ptr;
    DMP_DCB		*d = t->tcb_dcb_ptr;
    i4		lock_action = 0;
    i4		access_mode;
    i4             lock_mode;
    i4		local_error;
    DMPP_PAGE		*readnolock_buffer = (DMPP_PAGE *)0;
    DB_STATUS		status;
    DMP_LBMCB           *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON      *bm_common = lbm->lbm_bm_common;
    DM0P_BUFFER	        *b;

    r->rcb_fix_lkid.lk_unique = 0;
    r->rcb_fix_lkid.lk_common = 0;

#ifdef xDEBUG
    r->rcb_s_fix++;
    if (DMZ_BUF_MACRO(1))
    TRdisplay("FIX_PAGE    : (%~t,%~t,%d,%dK) action=%v lockid=%x.\n",
	sizeof(t->tcb_dcb_ptr->dcb_name), &t->tcb_dcb_ptr->dcb_name,
	sizeof(t->tcb_rel.relid),  &t->tcb_rel.relid, page_number,
	KSIZE(t->tcb_rel.relpgsize),
	"PHYSICAL,WRITE,CREATE,NOREADAHEAD,LOCKREAD,TEMPLOCK,NOWAIT,\
NOLOCK, READAHEAD,NOESCALATE,READNOLOCK,FORCE,RELEASE,USER_TID,HI_PRIORITY",
        action, r->rcb_lk_id);
#endif

    /* If readahead thread, check for interrupt, else modify actions     */
    if (r->rcb_state & RCB_PREFET_REQUEST)
    {
	/* RCB/request is from RA thread on behalf of a user thread request. */
	/* If user thread or other RA thread made request 'stale', abort it. */
	if (*(r->rcb_uiptr) & RCB_RAHEAD_INTR)
	{
	    *err_code = E_DM0065_USER_INTR;
	    return(E_DB_ERROR);
	}
    	/* request high buf priority so when buf is unfixed by RA thread, it */
    	/* wont become LRU victim, as we known user thread will re-fix it    */
	/* shortly. Also indicate we do not wish to wait on locks.           */
	action |= (DM0P_HI_PRIORITY | DM0P_NOWAIT);
    }

    /*
    ** Choose page access mode - Read or Write.
    ** The access mode is Write if the table is being accessed in write
    ** mode unless the caller explicitly overrides with DM0P_RPHYS.
    **
    ** Note that the access mode is chosen based on the original request
    ** mode of the table open rather than the granted lock mode.  The lock
    ** may actually be held at IX mode (which is a superset of IS), but we
    ** still want to to request share locks if the table is open for read.
    **
    ** (Note that we check for (WPHYS | RPHYS) instead of just RPHYS
    ** because of definition of constants.  The constant DM0P_RPHYS is
    ** defined (DM0P_PHYSICAL | DM0P_READ) and DM0P_READ actually has
    ** no value - READ is specified by by the absence of DM0P_WRITE.)
    */
    access_mode = DM0P_READ;
    if (((r->rcb_k_mode == RCB_K_IX) || (r->rcb_k_mode == RCB_K_X) ||
               (action & DM0P_WRITE)) &&
        ((action & (DM0P_WPHYS | DM0P_RPHYS)) != DM0P_RPHYS))
    {
        access_mode = DM0P_WRITE;
    }

    if (DMZ_BUF_MACRO(83))
    {
	if ((action & DM0P_WRITE) != 0 &&
	    t->tcb_rel.reltid.db_tab_base >= 1 &&
	    t->tcb_rel.reltid.db_tab_base <= 4 &&
	    (r->rcb_state & RCB_IN_MINI_TRAN) == 0)
	{
	    /*
	    ** It's now illegal to fix more than one system catalog page for
	    ** write at a time, unless the RCB is in the midst of a mini-
	    ** transaction (such as extending a core system catalog or
	    ** allocating a new overflow page into a core system catalog).
	    **
	    ** Enforce this restriction here.
	    */
	    b = first_fixed_syscat_page(&r->rcb_tran_id);
	    if (b)
	    {
		/* function returns with SHARE lock on buf_mutex */
		if (b->buf_tabid.db_tab_base  !=
					    t->tcb_rel.reltid.db_tab_base  ||
		    b->buf_tabid.db_tab_index !=
					    t->tcb_rel.reltid.db_tab_index ||
		    b->buf_lock_key.lk_key4   != page_number)
		{
		    TRdisplay("FIX: (%d,%d,%d) rejected -- (%d,%d,%d) fixed\n",
				t->tcb_rel.reltid.db_tab_base,
				t->tcb_rel.reltid.db_tab_index,
				page_number, b->buf_tabid.db_tab_base,
				b->buf_tabid.db_tab_index,
				b->buf_lock_key.lk_key4);
		    dm0p_munlock(&b->buf_mutex);
		    *err_code = E_DM9204_BM_FIX_PAGE_ERROR;
		    return (E_DB_ERROR);
		}
		dm0p_munlock(&b->buf_mutex);
	    }
	}
    }

    /*
    ** Determine if a transaction page lock is required.
    **
    ** A transaction page lock is not needed in the following cases:
    **
    **    - If our lock mode specifies that we have a TABLE level lock.
    **    - If the caller requested the DM0P_NOLOCK override.  Presumably
    **	    the caller knows what it is doing.
    **    - We are running in readnolock mode (unless the DM0P_LOCKREAD
    **	    override was requested).  The readnolock state is indicated
    **	    by rcb_k_duration and generally means that no page locks
    **	    are required.  But even during readnolock mode, certain
    **	    physical locks are still required (btree index pages, for
    **	    example).  In these cases, the caller will pass in the
    **	    DM0P_LOCKREAD action to override readnolock protocols.
    **
    ** Notes:
    **
    ** Readnolock queries on non-btree tables currently are set in TABLE
    ** lock mode since no page locks are required.  This causes the
    ** page lock call below to be bypassed even before the readnolock
    ** state is checked.  DM0P_LOCKREAD will now override some other options.
    ** 
    ** Exclusive database access or access to a readonly database will not
    ** request any transaction page locks.  In this case the open table code
    ** will mark the lock type as table.
    */
    if (t->tcb_extended && r->rcb_lk_type == RCB_K_TABLE)
	action |= DM0P_NOLOCK;

    if (((r->rcb_lk_type == RCB_K_PAGE) || (r->rcb_lk_type == RCB_K_ROW)) &&
        ((action & DM0P_NOLOCK) == 0) &&
        (((r->rcb_k_duration & RCB_K_READ) == 0) || (action & DM0P_LOCKREAD)))
    {
	/* need a page lock */

        /* figure out the lock mode */
        dm0p_page_lock_mode(r, action, access_mode, &lock_mode);

        /* request a transaction lock */
	status = dm0p_trans_page_lock(r, t, page_number, action, lock_mode,
			&lock_action, &r->rcb_fix_lkid, err_code);
	if (status != E_DB_OK)
	    return(status);

        if (row_locking(r) &&
	     (lock_mode == LK_IS || lock_mode == LK_IX || lock_mode == LK_SIX))
        {
	    action |= DM0P_INTENT_LK;
        }
    }

    /*
    ** If this is a readnolock query and the page is not being fixed 
    ** for write (read_only transactions are allowed to write
    ** temporary tables), then pass in a private page buffer into
    ** which to read the page rather than returning a pointer to an
    ** actual cached page.
    */
    if (r->rcb_k_duration & RCB_K_READ && 
	(action & (DM0P_LOCKREAD | DM0P_WRITE)) == 0)
    {
	/*
	** Select NOREADLOCK buffer to use.  Compute which page
	** is not currently in use.
	*/
	if (DMPP_VPT_PAGE_HAS_SEGMENTS(t->tcb_rel.relpgtype) &&
		r->rcb_1rnl_page && r->rcb_2rnl_page)
	{
	    r->rcb_3rnl_page = (DMPP_PAGE *)
	    ((char *)r->rcb_rnl_cb+sizeof(DMP_MISC)+(2*t->tcb_rel.relpgsize));
	    readnolock_buffer = r->rcb_3rnl_page;
	}
	else if (r->rcb_1rnl_page)
	{
	    r->rcb_2rnl_page = (DMPP_PAGE *)
		((char *)r->rcb_rnl_cb+sizeof(DMP_MISC)+t->tcb_rel.relpgsize);
	    readnolock_buffer = r->rcb_2rnl_page;
	}
	else
	{
	    r->rcb_1rnl_page = (DMPP_PAGE *)
		((char *)r->rcb_rnl_cb+sizeof(DMP_MISC));
	    readnolock_buffer = r->rcb_1rnl_page;
	}
	action |= DM0P_READNOLOCK;
    }

    /*
    ** Check for Readahead hints.  If readahead would likely help this
    ** query (indicated by rcb_state & RCB_READAHEAD), and the caller did
    ** not specify DM0P_NOREADAHEAD, then inform the cachefix routine that
    ** readahead would be good.
    */
    if ((r->rcb_state & RCB_READAHEAD) && ((action & DM0P_NOREADAHEAD) == 0))
	action |= DM0P_READAHEAD;

    /* if this server running in batch mode, always use readahead */
    if (dmf_svcb->svcb_status & SVCB_BATCHMODE)
	action |= DM0P_READAHEAD;

    /* Force group reads for blobs unless explicitly negated. */
    if ( t->tcb_extended && !(action & DM0P_NOREADAHEAD) )
	action |= DM0P_READAHEAD;

    /*
    ** Find, Fix and possibly Fault in the requested page.
    */
    status = dm0p_cachefix_page(&t->tcb_table_io, page_number, access_mode, 
	action, r->rcb_lk_id, r->rcb_log_id, &r->rcb_tran_id, 
	readnolock_buffer, t->tcb_acc_plv->dmpp_format, page, 
	(LG_LSN*)NULL, err_code);
    if (status != E_DB_OK)
    {
	/*
	** If an error occurred, check if we have acquired a physical lock
	** on the page we were trying to read.  If so, then release that
	** lock since the page was not successfully fixed. If page was
        ** latched, release the latch as well.
	**
	** Note: the return status from the unlock call is ignored since
	** the error is logged inside unlock_page anyway.
	*/
        if (lock_action & LK_PHYSICAL)
        {
            _VOID_ dm0p_unlock_page(r->rcb_lk_id, d, &t->tcb_rel.reltid,
		page_number, LK_PAGE, &t->tcb_rel.relid, &r->rcb_tran_id,
                &r->rcb_fix_lkid, (LK_VALUE *)0, &local_error);
        }

	dm0p_log_error( E_DM9204_BM_FIX_PAGE_ERROR, err_code, 
			((action & DM0P_USER_TID) == 0)); 

	return (E_DB_ERROR);
    }

    /*
    ** If ONLINE operation, keep track of data pages that were updated after  
    ** the online operation started.
    ** This information must be collected here in the buffer manager.
    ** If we fix a page with 0 rows (but the rows were deleted after the 
    ** online operation started) we must remember this page and its lsn -
    ** so that the deletes to this page are redone during sidefile processing.
    ** 
    ** We need this information for DATA pages so that the online modify
    ** (and rollforwardb) can decide which updates having lsn > bsf_lsn
    ** should be redone (only those having lsn > rnl_lsn).
    ** 
    ** We need this information for Btree LEAF pages for rollforwarddb only
    ** so when redoing the online modify, and reading the table for modify,
    ** each DATA/LEAF page can be restored (via undo) to the rnl_lsn.
    */

    if (readnolock_buffer && (action & DM0P_READNOLOCK) && r->rcb_rnl_online)
    {
	if (((d->dcb_status & DCB_S_ROLLFORWARD) == 0) &&
	    (DMPP_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype, *page) & 
			(DMPP_DATA | DMPP_LEAF)))
	{
	    LG_LSN	*pglsn = DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(t->tcb_rel.relpgtype, *page);

	    r->rcb_rnl_online->rnl_read_cnt++;

	    /* 
	    ** If btree, must record pg lsn regardless of whether
	    ** pglsn is less than bsflsn. With btree, disassoc data page may be
	    ** read (where pglsn < bsflsn), reclaimed/deallocated and then
	    ** allocated as leaf or data (pg lsn put into rnl lsn array).
	    ** Rollforward solely relies on rnl lsn array and is unable to determine
	    ** such a situation occurred in forward processing.
	    */
	    if ( (LSN_GT(pglsn, &r->rcb_rnl_online->rnl_bsf_lsn)) || 
		 (t->tcb_rel.relspec == TCB_BTREE) )
	    {
		status = rnl_fixpage(r, *page, page_number, err_code);
	    }
	}
	else if ((d->dcb_status & DCB_S_ROLLFORWARD) &&
		 (DMPP_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype, *page)
                        & (DMPP_DATA | DMPP_LEAF | DMPP_FREE)))
	{
	    status = rfp_rnl_fixpage(r, *page, page_number, err_code);
	}

	if (status != E_DB_OK)
	    return (status);
    }

    /*
    ** save the lkid for ALL isolation level.
    ** Lock coupling for CS and RR isolation levels: now release the 
    ** previous page's lock, if the appropriate flag is set in the RCB. 
    ** Save the lockid of the current page in the RCB and clear 
    ** PAGE_HAD_RECORDS and PAGE_UPDATED bits of rcb_state. 
    */
    if ((r->rcb_lk_type != RCB_K_TABLE) && (action & DM0P_PHYSICAL) == 0)
    {
        if (DMPP_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype, *page)
                & (DMPP_LEAF|DMPP_CHAIN))
        {
	    if ((r->rcb_state & RCB_CSRR_LOCK) &&
            	(r->rcb_csrr_flags & RCB_CS_LEAF))
            {    
		_VOID_ dm0p_unlock_page(r->rcb_lk_id, d, &t->tcb_rel.reltid,
		    r->rcb_locked_leaf, LK_PAGE, &t->tcb_rel.relid,
		    &r->rcb_tran_id, &r->rcb_leaf_lkid, (LK_VALUE *)0, 
		    err_code);
                if (status != E_DB_OK)
                    return (status);
                r->rcb_csrr_flags &= ~RCB_CS_LEAF;
            }
            STRUCT_ASSIGN_MACRO(r->rcb_fix_lkid, r->rcb_leaf_lkid);
	    r->rcb_locked_leaf = page_number;
            r->rcb_state &= ~(RCB_LPAGE_HAD_RECORDS | RCB_LPAGE_UPDATED);
        }
        else if (DMPP_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype, *page)
                     & DMPP_DATA)
        {
	    if ((r->rcb_state & RCB_CSRR_LOCK) &&
            	(r->rcb_csrr_flags & RCB_CS_DATA)) 
            {
		/* 
		** If isolation level is CS and the previous row has not been 
		** updated, unlock it before unlocking the page it is on. 
		*/
		if (r->rcb_csrr_flags & RCB_CS_ROW)
		{
		    status = dm1r_unlock_row(r, (LK_LOCK_KEY *) NULL, err_code);
		    if (status != E_DB_OK)
			return (status);
		    r->rcb_csrr_flags &= ~RCB_CS_ROW;
		}
		_VOID_ dm0p_unlock_page(r->rcb_lk_id, d, &t->tcb_rel.reltid,
		    r->rcb_locked_data, LK_PAGE, &t->tcb_rel.relid,
		    &r->rcb_tran_id, &r->rcb_data_lkid, (LK_VALUE *)0, 
		    err_code);
                if (status != E_DB_OK)
                    return (status);
                r->rcb_csrr_flags &= ~RCB_CS_DATA;
            }
            STRUCT_ASSIGN_MACRO(r->rcb_fix_lkid, r->rcb_data_lkid);
	    r->rcb_locked_data = page_number;
            r->rcb_state &= ~(RCB_DPAGE_HAD_RECORDS | RCB_DPAGE_UPDATED);
        }
    }

    return (E_DB_OK);
}

/*{
** Name: dm0p_unfix_page	- Unfix page from a transaction.
**
** Description:
**	The page is unfixed for the caller.  If the reference count goes
**	to zero, the page is not fixed by any transaction at this point
**	in time.  The page is transfered into the free priority queue or
**	into the modify priority queue depending on the need for the buffer
**	to be written back to disk.  If the modify threshold has been
**	reached, the lowest priority dirty buffer is written back so that
**	this buffer can be placed on the modify queue.
**
**	The following diagram depicts the control flow:
**
**			MUTEX
**			    |
**			CHECK_PAGE
**			    |
**		        UNFIX
**		+----------|
**		|       PUT_MODIFY
**		|	    |---------+
**		|       UNMUTEX       |
**		|           |         |
**		|	FORCE         |
**		|	    |         |
**		|	WRITE         |
**		|           |         |
**		|       MUTEX         |
**		+---------->|         |
**			PUT_FREE      |
**		+-----------|<--------+
**		|	UNLOCK_CACHE
**		+---------->|
**			UNMUTEX
**			    |------------+
**			PHYSICAL_UNLOCK  |
**			    |<-----------+
**			RETURN
** Inputs:
**	rcb	    			The RCB.
**      action                          The specific unfix action.
**					  DM0P_UNFIX - Normal action mode. Page
**					    reference count is decreased.  If
**					    reference count goes to 0, page is
**					    moved off of the fixed queue and
**					    put on the free or modified queue.
**					  DM0P_FORCE - Force the page to disk
**					    but don't actually unfix it.  Page
**					    remains fixed by caller.
**					  DM0P_WRITE - Similar to DM0P_UNFIX
**					    mode except that page is forced
**					    to disk if modified.
**					  DM0P_RELEASE - Page is unfixed and
**					    taken off of the list of pages 
**					    owned this transaction.  Any 
**					    physical locks held on this page 
**					    are released.
**	page				Pointer to page to unfix.
**
** Outputs:
**      err_code                        Reason for error return status.
**	Returns:
**	    E_DB_OK
**	    E_DB_FATAL
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	15-oct-1985 (derek)
**          Created new for Jupiter.
**	22-Jan-1987 (ac)
**	    Added code for readahead support.
**	 2-sep-1988 (rogerk)
**	    Make sure to force page if mode is DM0P_WRITE.
**	19-sep-1988 (rogerk)
**	    Fix bug in above bug fix pertaining DM0P_WRITE.
**	30-sep-1988 (rogerk)
**	    Added write behind thread support.  If modify count is above
**	    asynchronous write behind window, then signal the write behind
**	    threads.
**	31-oct-1988 (rogerk)
**	    When moving buffer to modified queue, put it at the end of its
**	    priority queue instead of at the beginning.  This is so older
**	    pages will be forced out first causing us to do fewer log forces.
**	28-feb-1989 (rogerk)
**	    Added support for Shared Buffer Manager.
**	    Pass tcb pointer to force_page calls.
**	    Use STRUCT_ASSIGN to save buffer lock key.
**      11-sep-1990 (jonb)
**	    Integrate ingres6301 change 280136 (rogerk):
**            Pass buf_tcb_stamp value to dm2t_get_tcb_ptr.
**	10-jan-1991 (rachael)
**	    Added error messages e_dm934c and e_dm934d to record
**	    more information before dmd_check.
**	3-may-1991 (bryanp)
**	    Make dmd_checks more useful by logging detailed error information
**	    before calling dmd_check.
**	17-oct-1991 (rogerk)
**	    Merged changes made by Derek during the file extend project.
**	    Added new DM0P_MRELEASE action.
**	    Handle new Write-on-Unfix, Write-on-PassAbort and NoLock sts flags.
**	    Added trace flags in xDEBUG code.
**	    Added new session statistics collecting.
**	21-jan-1992 (jnash)
**	    Toss pages marked TUNFIX.
**	26-oct-1992 (rogerk)  Reduced Logging Project:
**	  - Moved page lock release into dm0p_unlockpage routine.
**	  - Moved work which actually unfixes pages into dm0p_uncachefix_page.
**	  - Removed DM0P_MRELEASE option.  Pages must now be mutexed and
**	    unmutexed explicitly by callers while updating them.
**	  - Removed comment that DM0P_RELEASE option causes the page to be
**	    written if modified - this was not true.
**	  - Changed to pass down the DM0P_RELEASE option if pages are listed
**	    as being physically locked (rcb_k_duration is RCB_K_PHYSICAL).
**	    This is used for system catalog pages.  The buffer manager no 
**	    longer assumes that such pages will be listed WUNFIX.
**	  - Changed values of unfix action flags to be bit values rather
**	    than mutually exclusive integer values.  Dm0p_unfix continues
**	    to accept only mutually exclusive settings but may OR together
**	    action specifications for dm0p_uncachefix_page calls.
**	22-feb-1994 (andys)
**	    Add call to dm0p_check_consistency.
**      04-apr-1995 (dilma04) 
**          Add support for rcb_k_duration == RCB_K_CURSOR_STABILITY and
**          rcb_k_duration == RCB_K_REPEATABLE_READ.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**      06-may-1996 (nanpr01)
**          Change the reference to the page header with macros for
**	    New Page Format.
**      22-nov-96 (dilma04)
**          Row Locking Project:
**          If row locking and no phantom protection release the latch.
**      06-jan-97 (dilma04)
**          Removed the optimization to release an intent page lock if
**          no qualifying records are found on the page. This is because
**          of the situation when there are multiple cursors opened for
**          the same transaction with different lock granularity levels.
**          In this situation a page lock can be granted in a higher mode,
**          than it was requested. Such a lock should not be released in
**          this routine, but there is no way to know about it.
**      12-feb-97 (dilma04)
**          Set Lockmode Cleanup:
**          - test rcb_iso_level for isolation level;
**          - use macro to check for serializable transaction.
**	26-feb-97 (dilma04)
**	    Lock Escalation for Lower Isolation Levels:
**	    If isolation level is repeatable_read and page being unfixed
**	    is a non-modified data page containing at least one qualifying
**	    record returned by dm2r_get(), convert page lock from physical
**	    to logical. In all other cases behave as if isolation level
**	    were cursor_stability.
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          - lock coupling for CS and RR: instead of releasing a page lock
**          here, do it in dm0p_fix_page after the next lock is acquired;
**          - moved lock conversion for RR to dm2r_get() routine; 
**          - added lock_id argument to dm0p_unlock_page() calls.
**      21-may-97 (stial01)
**          dm0p_unfix_page: Remove release of LK_PH_PAGE locks
**      13-jun-97 (dilma04)
**          Repeatable Read optimization: set flag to release leaf page's 
**          lock, if page has not been updated.
**	25-Jun-1998 (jenjo02)
**	    Don't assume a buffer is on transaction queue just because 
**	    page is fixed for write, but check explicitly. Remove buffer
**	    from txn queue when unfixed.
**	02-apr-1999 (nanpr01)
**	    Update mode lock -- For repeatable read, we must convert the
**	    update mode lock to logical shared.
**      20-sep-1999 (stial01)
**          dm0p_unfix_page() Convert lock on dirty etab leaf pages (2k page)
**	14-Mar-2003 (jenjo02)
**	    If RCB_ABORTING is set, don't fool with existing locks.
**	    (B109842). Also, be sure correct lock_id is passed
**	    dm0p_trans_page_lock().
*/

DB_STATUS
dm0p_unfix_page(
DMP_RCB		   *rcb,
i4            action,
DMPP_PAGE          **page,
i4            *err_code)
{
    DMP_RCB		*r = rcb;
    DMP_TCB		*t = r->rcb_tcb_ptr;
    DMP_DCB             *d = t->tcb_dcb_ptr;
    DMP_LBMCB           *lbm = dmf_svcb->svcb_lbmcb_ptr;
    i4                  cache_ix = t->tcb_table_io.tbio_cache_ix;
    DM0P_BMCB           *bm = lbm->lbm_bmcb[cache_ix];
    DM0P_BUFFER         *buf_array = lbm->lbm_buffers[cache_ix];
    DM0P_BUFFER		*b;
    DB_STATUS		status;
    DM_PAGENO		page_number;
    i4		page_stat;
    i4		page_index;
    i4		page_lock_bypass = FALSE;

#ifdef xDEBUG
    if (DMZ_BUF_MACRO(1))
    TRdisplay("UNFIX_PAGE  : (%~t,%~t,%d,%dK) action=%v lk_id: %x\n",
	sizeof(t->tcb_dcb_ptr->dcb_name), &t->tcb_dcb_ptr->dcb_name,
	sizeof(t->tcb_rel.relid),  &t->tcb_rel.relid, 
	DMPP_VPT_GET_PAGE_PAGE_MACRO(t->tcb_rel.relpgtype, *page),
	KSIZE(t->tcb_rel.relpgsize),
	",WRITE,,,,,,,,,,FORCE,RELEASE,,", action, 
	r->rcb_lk_id);
#endif

    /*
    ** If the fix action was of a READNOLOCK copy, then there was no
    ** actual page fix done in the buffer manager.  We simply free up
    ** our rcb_rnl_page pointer to show that the private buffer is no
    ** longer in use.
    */
    if (r->rcb_k_duration & RCB_K_READ)
    {
	/*
	** Check if the page is fixed into one of our private buffers.
	** There are cases even when RCB_K_READ is enabled that real	
	** fixes are done.  In these cases, *page will not point to
	** a private buffer and we will fall through to a real unfix.
	*/
	if (*page == r->rcb_1rnl_page)
	{
	    *page = 0;
	    rcb->rcb_1rnl_page = 0;
	    return (E_DB_OK);
	}
	else if (*page == r->rcb_2rnl_page)
	{
	    *page = 0;
	    rcb->rcb_2rnl_page = 0;
	    return (E_DB_OK);
	}
	else if (*page == r->rcb_3rnl_page)
	{
	    *page = 0;
	    rcb->rcb_3rnl_page = 0;
	    return (E_DB_OK);
	}
    }

    /*
    ** Save sufficient page information before unfixing the buffer to
    ** be able to make the unlock_page call below.
    **
    ** We need to quickly reference the buffer header to look for the
    ** page lock override flag which may be stored in the buffer header.
    ** This override is used for some special extend/allocate actions where
    ** no page locks are acquired while the page is fixed.
    ** A quick sanity check is done on the page address before looking up
    ** up the buffer contents.  If the sanity check fails, then a dmd_check
    ** will occur inside dm0p_uncache_fix (which is why we ignore it here).
    */
    page_index = ((char *)*page - (char *)lbm->lbm_pages[cache_ix])/bm->bm_pgsize;
    if ((page_index >= 0) && (page_index < bm->bm_bufcnt))
    {
	page_number = DMPP_VPT_GET_PAGE_PAGE_MACRO(t->tcb_rel.relpgtype, *page);
	b = &buf_array[page_index];
	page_lock_bypass = ((b->buf_sts & BUF_NOTXLOCK) != 0);
    }

    /* Extract page_stat from the page to a local variable */
    page_stat = DMPP_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype, *page);

    /*
    ** Unfix the page in the cache.
    ** If short term locks are being used, then specify to release the
    ** page from the transaction list.
    */
    if ((r->rcb_k_duration & RCB_K_PHYSICAL) && (action != DM0P_FORCE))
    {
	action |= DM0P_RELEASE;
    }

    /*                        
    ** If running at the CS or RR isolation level and the page lock
    ** does not have to be held until the end of transaction, set
    ** the appropriate flag in the RCB in order to release the lock 
    ** when the next lock of the same type will be acquired.
    */
    else if ((r->rcb_state & RCB_CSRR_LOCK) && action != DM0P_FORCE &&
             (action & DM0P_RELEASE) == 0 && !page_lock_bypass &&
	     r->rcb_lk_type != RCB_K_TABLE)
    {
	if (page_stat & DMPP_DATA &&
            r->rcb_data_lkid.lk_unique &&
            (r->rcb_state & RCB_DPAGE_UPDATED) == 0 && 
            (r->rcb_iso_level != RCB_REPEATABLE_READ ||
                (r->rcb_state & RCB_DPAGE_HAD_RECORDS) == 0))
        {
            r->rcb_csrr_flags |= RCB_CS_DATA;    
        }                                     
	else if (page_stat & (DMPP_LEAF|DMPP_CHAIN) && 
            r->rcb_leaf_lkid.lk_unique &&
            (r->rcb_state & RCB_LPAGE_UPDATED) == 0)
        {
            r->rcb_csrr_flags |= RCB_CS_LEAF;
        }
    }

    /* If rcb was NOT executing a prefetch request, allow lower priority */
    if (! (r->rcb_state & RCB_PREFET_REQUEST))
	action |= DM0P_HI_PRIORITY;	/* means lower pri when unfixing */

    status = dm0p_uncache_fix(&t->tcb_table_io, action,
	    r->rcb_lk_id, r->rcb_log_id, &r->rcb_tran_id, page, err_code);

    if (t->tcb_extended && r->rcb_lk_type == RCB_K_TABLE)
	return (status);

    if ( status == E_DB_OK && (r->rcb_state & RCB_ABORTING) == 0 )
    {
	/*
	** Release any physical locks taken at fix time if this is a "real"
	** unfix action (not DM0P_FORCE).
	*/
	if ( action & DM0P_RELEASE &&
	    ((r->rcb_lk_type == RCB_K_PAGE) || row_locking(r)) &&
	    !page_lock_bypass )
	{
	    /*
	    ** 2k Btree leaf pages for blob etabs
	    ** If this transaction updated the leaf, convert the lock to 
	    ** logical instead of releasing it to reserve leaf space for
	    ** rollback
	    */
	    if (t->tcb_extended && 
		    !t->tcb_table_io.tbio_temporary &&
		    t->tcb_logging &&
		    t->tcb_rel.relpgtype == TCB_PG_V1 &&
		    (page_stat & DMPP_LEAF) &&
		    (page_stat & DMPP_MODIFY))
	    {
		LK_LKID             page_lockid;
		i4			rcb_k_duration = r->rcb_k_duration;
		i4			lock_action = 0;
		i4			action = 0;

		/* Turn off PHYSICAL page lock protocols */
		r->rcb_k_duration &= ~RCB_K_PHYSICAL;

		MEfill(sizeof(LK_LKID), 0, &page_lockid);

		status = dm0p_trans_page_lock(r, t, page_number, action, LK_X,
			    &lock_action, &page_lockid, err_code);

		/* restore rcb_k_duration */
		r->rcb_k_duration = rcb_k_duration;
	    }
	    else
	    {
		status = dm0p_unlock_page(r->rcb_lk_id, d, &t->tcb_rel.reltid,
		    page_number, LK_PAGE, &t->tcb_rel.relid, &r->rcb_tran_id,
		    (LK_LKID *)0, (LK_VALUE *)0, err_code);
	    }
	}

	if ((status == E_DB_OK) && (r->rcb_state & RCB_UPD_LOCK) &&
	    ((r->rcb_iso_level == RCB_REPEATABLE_READ) || 
	     (r->rcb_iso_level == RCB_SERIALIZABLE)) && (!page_lock_bypass) &&
	    (r->rcb_lk_type == RCB_K_PAGE))
	{
	    if (((page_stat & DMPP_DATA) && 
		((r->rcb_state & RCB_DPAGE_UPDATED) == 0)) ||
	       ((page_stat & (DMPP_LEAF|DMPP_CHAIN)) && 
		((r->rcb_state & RCB_LPAGE_UPDATED) == 0)))
	    {
		i4	lock_action;
		i4  action = 0;
		LK_LKID            *lockid;

		/* Use lock_id that matches page being unfixed */
		if ( page_number == r->rcb_locked_data ) 
		    lockid = &r->rcb_data_lkid;
		else
		if ( page_number == r->rcb_locked_leaf )
		    lockid = &r->rcb_leaf_lkid;
		else
		    lockid = (LK_LKID*)NULL;

		if (r->rcb_iso_level == RCB_REPEATABLE_READ)
		    action |= DM0P_LK_CONVERT;
		status = dm0p_trans_page_lock(r, t, page_number, action, 
			    LK_S, &lock_action, lockid, err_code);
	    }
	}
    }

    if (DMZ_BUF_MACRO(15))
        (VOID) dm0p_check_consistency(5);

    if (status == E_DB_OK)
	return (E_DB_OK);

    if (*err_code > E_DM_INTERNAL)
    {
	ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL, (char *)0,
	    (i4)0, (i4 *)0, err_code, 0);
	*err_code = E_DM9208_BM_UNFIX_PAGE_ERROR;
    }

    return (E_DB_ERROR);
}

/*{
** Name: dm0p_cachefix_page	- Fix a buffer in the page cache.
**
** Description:
**	This routine finds a requested page in the Buffer Manager and fixes it.
**	If no such page exists in the cache, then a new one is read in and
**	then fixed.  While fixed, the page cannot be tossed from the cache.
**
** Inputs:
**	tbio				Table IO Control Block.
**	page_number			The page number in the table.
**	access_mode			The page access mode: read or write.
**      action                          The action to fix page
**	lock_list			Lock List Id
**	log_id				Log Id
**	tran_id				Pointer to Transaction Id
**	readnolock_buffer		Page buffer for readnolock use
**	format_routine			Routine to format new page
**	RedoLSN				Supplied during online Cluster REDO
**					recovery to check LSN of page
**					without need of SV_PAGE lock.
**
** Outputs:
**	page				Set to page in cache
**	err_code			Reason for error return status.
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**
** History:
**	26-oct-1992 (rogerk)
**          Created for Reduced Logging Project.  Code moved here from old
**	    dm0p_fix_page routine.
**	18-feb-1993 (rmuth)
**	    If encounter an error calling [g]fault_page use 
**	    dm0p_log_error as this routine understands which errors to
**	    preserve and pass upto the higher layers.
**	18-oct-1993 (rmuth)
**	    Add DMP_BMCB statistics.
**	22-feb-1994 (andys)
**	    Integrated 6.4 buffer manager changes - set buf_cpcount when
**	    fix an unmodified page for write to make sure that the buffer
**	    is marked as processed in the current CP.
**	23-may-1994 (andys)
**	    Add new parameter to dm0p_log_error which says whether to print
**	    the error which we are throwing away. We pass the flag saying not 
**	    to log the error when the flag DM0P_USER_TID is passed to 
**	    dm0p_fix_page, dm0p_cachefix_page or fault_page. This flag is
**	    only currently set by dm1r_get when it is doing a get by tid.
**	    [bug 49726]
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**      06-may-1996 (nanpr01)
**          Change the format routine paramter to i4 for u_i2.
**      01-aug-1996 (nanpr01 for ICL phil.p)
**          For the Distributed Multi-Cache Management (DMCM) protocol,
**          retain READNOLOCK pages it the cache with a NULL lock. After
**          having copied the page into the readnolock buffer, convert the
**          lock down from LK_S to LK_N.
**	01-aug-1996 (nanpr01)
**	    Do not drop the cachelocks to NULL lock unless you are faulting
**	    in the page.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Set BUF_X_LATCH, BUF_WUNFIX/BUF_PAWRITE when EXCL row locking
**	01-Apr-1997 (jenjo02)
**	    Keep stats for fixed priority pages.
**	01-May-1997 (jenjo02)
**	  - If group buffer's group is busy being faulted or forced, wait
**	    for the group, then retry.
**	  - If fixed for READNOLOCK, take SHARE lock on buffer to permit
**	    concurrent copies into readnolock buffers.
**      21-may-1997 (stial01)
**          dm0p_cachefix_page() Do not put buf with IX page lock on xn queue
**          Removed code for non-fast-commit when row locking (not allowed) 
**          Removed code for BUF_PAWRITE when IX lock. (pass abort not allowed)
**	10-Dec-1997 (jenjo02)
**	    Carefully scan the buffer hash chain sans mutex.
**	21-Jan-1998 (jenjo02)
**	    Removed code which took SHARE buffer mutex with READNOLOCK requests.
**	    Buffer gets updated by that code, so a SHARE mutex isn't strong 
**	    enough.
**	08-Sep-1998 (shero03)
**	    Change the hash algorithm to mod by the hash count.
**      12-jan-1999 (stial01)
**          dm0p_cachefix_page() DM0P_CREATE_NOFMT: mutex buffer if !SINGLEUSER
**      08-Mar-1999 (wanya01)
**          dm0p_cachefix_page() Fix bug 91094. If the group is not busy, we 
**          make it busy, and we make it free until finishing move_group. This 
**          prevent another thread from taking the same group from the free
**          queue.  
**	11-Oct-1999 (jenjo02)
**	    If found page is being written by WB or CP thread, there's no need
**	    to wait for the write to complete; the page won't be tossed and will
**	    still be valid.
**	21-Oct-1999 (jenjo02)
**	    Backed out 10-Oct-1999 change. Caused dmd_checks in dm0p_unmutex(),
**	    not really sure why.
**	10-Jan-2000 (jenjo02)
**	    Removed wait-for-busy-group. There's really no need to do that,
**	    nor mark the group busy while we rereference a group page.
**	    If refixing the first page of a group, bump the group's
**	    priority.
**	23-Jun-2000 (jenjo02)
**	    Added fill_option to dmpp_format() prototype, set by caller
**	    to DM1C_ZERO_FILL if dmpp_format should MEfill the page,
**	    DM1C_ZERO_FILLED if the page has already been MEfilled.
**	    The page overhead space is always zero-filled.
**      19-Jun-2002 (horda03) Bug 108074
**           If the action flag DM0P_TABLE_LOCKED_X defined, then there is no
**           need to have the BUF_WUNFIX flag set. Similarly, if the
**           DM0P_TABLE_LOCKED_X flag isn't set, and the page was found in
**           memory, then ensure that the BUF_WUNFIX flag is set (if
**           BUF_WUNFIX_REQ set).
**      18-Sep-2003 (jenjo02) SIR 110923
**          Extend page formatting ability to groups.
**	06-Mar-2007 (jonj)
**	    Add RedoLSN param, supplied during online Cluster REDO page fixing.
*/
DB_STATUS
dm0p_cachefix_page(
DMP_TABLE_IO	*tbio,
DM_PAGENO	page_number,
i4		access_mode,
i4		action,
i4		lock_list,
i4		log_id,
DB_TRAN_ID	*tran_id,
DMPP_PAGE	*readnolock_buffer,
VOID		(*format_routine)(
			i4		page_type,
			i4		page_size,
			DMPP_PAGE       *page,
			DM_PAGENO       pageno,
			i4         	stat,
			i4	   	fill_option),
DMPP_PAGE	**page,
LG_LSN		*RedoLSN,
i4		*err_code)
{
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    i4              cache_ix = tbio->tbio_cache_ix;
    DM0P_BMCB       *bm = lbm->lbm_bmcb[cache_ix];
    DM0P_BUFFER	    *buf_array = lbm->lbm_buffers[cache_ix];
    DM0P_HASH_HEAD  *bucket_array = lbm->lbm_buckets[cache_ix];
    DM0P_GROUP      *group_array = lbm->lbm_groups[cache_ix];
    DM0P_BSTAT	    *lbm_stats = &lbm->lbm_stats[cache_ix];
    DM0P_GROUP	    *g;
    DM0P_BUFFER	    *buffer;
    DM0P_BUFFER	    *b;
    DM0P_STATE_HEAD *sth;
    i4	    hash_bucket;
    i4         hash_chain;
    i4	    next_buf, prev_buf;
    DB_STATUS	    status;
    STATUS          cl_status = OK;
    CL_SYS_ERR      sys_err;
    char            *tbl_name;
    char            *dbs_name;
    char            tbl_buf[DB_MAXNAME];
    char            dbs_buf[DB_MAXNAME];
    i4		    restarts;

    /* For debugging RedoLSN */
    static bool RedoLSNDebug = FALSE;

    /*
    **	FIND_PAGE
    **
    **	Find requested page in buffer pool.  If page is not found call
    **	the fault page routine, to map the page from the disk into memory.
    **	If the page is found, return it to the caller.
    */

    lbm_stats->bs_fix[BMCB_PTYPES]++;
    bm->bm_stats.bs_fix[BMCB_PTYPES]++;

    if ( RedoLSN && RedoLSNDebug )
	TRdisplay("%@ dm0p_cachefix_page %d: RedoLSN %x.%x for %d,%d,%d\n", 
		    __LINE__, 
		    RedoLSN->lsn_high, RedoLSN->lsn_low,
		    tbio->tbio_reltid.db_tab_base,
		    tbio->tbio_reltid.db_tab_index,
		    page_number);

    /*
    ** Compute the hash bucket to look in for this page.
    */
    /*
     * Note that the key sum is moded by the hash count.
     * In the previous code it was anded, which works if the hash count
     * is 1 less than a power of 2, e.g. 255.
     */
    hash_bucket = (u_i4)(tbio->tbio_dcbid + 
    		    	       tbio->tbio_reltid.db_tab_base + 
	                       tbio->tbio_reltid.db_tab_index +
		    	       page_number) % bm->bm_hshcnt;

    for (;;)
    {
	/*
	** Search the chain of buckets till we find the matching buffer or
	** we reach the end of the list.
	**
	** Notice that we do this without the aid or blockage of the hash mutex.
	*/
	b = NULL;
	hash_chain = bucket_array[hash_bucket].hash_next;
	restarts = MAX_RESTARTS;

	while (hash_chain != BUF_ENDLIST && restarts)
	{
	    b = &buf_array[hash_chain];
	    if (b->buf_lock_key.lk_key4 == page_number && 
		b->buf_lock_key.lk_key1 == tbio->tbio_dcbid &&
		b->buf_lock_key.lk_key2 == tbio->tbio_reltid.db_tab_base &&
		b->buf_lock_key.lk_key3 == tbio->tbio_reltid.db_tab_index)
	    {
		dm0p_mlock(SEM_EXCL, &b->buf_mutex);
		if (b->buf_hash_bucket == hash_bucket &&
		    b->buf_lock_key.lk_key4 == page_number && 
		    b->buf_lock_key.lk_key1 == tbio->tbio_dcbid &&
		    b->buf_lock_key.lk_key2 == tbio->tbio_reltid.db_tab_base &&
		    b->buf_lock_key.lk_key3 == tbio->tbio_reltid.db_tab_index)
		{
		    break;
		}
		dm0p_munlock(&b->buf_mutex);
	    }
	    hash_chain = b->buf_next;
	    if (b->buf_hash_bucket != hash_bucket)
	    {
		hash_chain = bucket_array[hash_bucket].hash_next;
		restarts--;
	    }
	    b = NULL;
	}

	if (b == NULL)
	{
	    /*
	    **	FAULT_PAGE
	    **
	    **	Map the page from disk into memory.  If the return status
	    **  is E_DB_WARN then the operation should be restarted.
	    */

	    status = E_DB_WARN;

	    if ( action & DM0P_READAHEAD && bm->bm_status & BM_GROUP )
	    {
		status = gfault_page(bm, tbio, (i4)page_number, 
		    hash_bucket, lock_list, access_mode, log_id, tran_id,
                    &buffer, action, format_routine, err_code);
	    }
	    if (status == E_DB_WARN)
	    {
		status = fault_page(bm, tbio, action, (i4)page_number, 
		    hash_bucket, lock_list, access_mode, log_id, tran_id,
		    format_routine, &buffer, RedoLSN, err_code);
	    }

	    if (status == E_DB_WARN)
		continue;

	    if (status == E_DB_OK)
	    {
		/* buf_mutex is locked EXCL */
		b = buffer;

		lbm_stats->bs_fix[b->buf_type]++;
		bm->bm_stats.bs_fix[b->buf_type]++;

		*page = (DMPP_PAGE *)((char *)lbm->lbm_pages[cache_ix] + 
				b->buf_page_offset);

		/*
		** If this page is being fixed Readlock = NOLOCK, then
		** copy the page into a nolock buffer and unfix the
		** original page.
		*/
		if (action & DM0P_READNOLOCK)
		{
		    MEcopy((PTR)*page,
			        bm->bm_pgsize, (PTR) readnolock_buffer);
		    *page = readnolock_buffer;

                    /*
                    ** (ICL phil.p)
                    ** Having copied the page into a readnolock buffer
                    ** convert the lock down to NULL, otherwise cache
                    ** deadlock can occur.
		    ** Mark buffer as read busy for protection whilst
		    ** dropping the mutex.
		    **
		    ** Now that callbacks can force and downgrade 
		    ** fixed-for-read pages, this is no longer necessary
		    ** and we'll just leave the existing LK_S cache lock.
		    ** 05-Feb-1999 (jenjo02)
                    */

		    /*
		    **  Undo the changes made by fault_page or gfault_page
		    **  that left the buffer fixed.
		    */

		    b->buf_sts &= ~(BUF_FIXED | BUF_WRITE);
		    if (b->buf_sts & BUF_LOCKED)
			b->buf_sts |= BUF_CACHED;

		    b->buf_refcount--;

		    /*
		    ** Put the buffer back on the end of the free queue according to its
		    ** priority.
		    */
		    if (b->buf_owner == BUF_SELF)
		    {
			i4	p = b->buf_priority;

			dm0p_mlock(SEM_EXCL, &bm->bm_fpq[p].sth_mutex);
			prev_buf = bm->bm_fpq[p].sth_stq.stq_prev;
			b->buf_state.stq_prev = prev_buf;
			b->buf_state.stq_next = BUF_ENDLIST;

			if (prev_buf != BUF_ENDLIST)
			    buf_array[prev_buf].buf_state.stq_next = 
				b->buf_id.bid_index;
			else
			    bm->bm_fpq[p].sth_stq.stq_next = 
				b->buf_id.bid_index;
			bm->bm_fpq[p].sth_stq.stq_prev = 
			    b->buf_id.bid_index;
			/* Count one more free buffer, one less fixed */
			bm->bm_fpq[p].sth_count++;
			bm->bm_fpq[p].sth_lcount--;

			dm0p_munlock(&bm->bm_fpq[p].sth_mutex);

			/* Wake up any waiters on free buffers */
			if (bm->bm_status & BM_FWAIT)
			{
			    dm0p_mlock(SEM_EXCL, &bm->bm_mutex);
			    if (bm->bm_status & BM_FWAIT)
			    {
				dm0p_erelease(lock_list, DM0P_FWAIT_EVENT, 
						bm->bm_cache_ix, 0, 0);
				bm->bm_status &= ~BM_FWAIT;
			    }
			    dm0p_munlock(&bm->bm_mutex);
			}
		    }
		    else
		    {
			/* Count one less fixed buffer in group */
			g = &group_array[b->buf_owner];
			dm0p_mlock(SEM_EXCL, &g->g_mutex);
			/* Let move_group() decide state of group.  The group
			** will not become empty (we're not changing valcount.
			** Decr the group refcount by one.
			*/
			move_group(bm, g, b, lock_list, -1, 0, 0);
			dm0p_munlock(&g->g_mutex);
		    }

		    /*
		    ** Make sure that system is not in recover state.  We
		    ** cannot leave readnolock pages in the cache when the
		    ** RCP is doing abort processing on behalf of our server,
		    ** since we are not protected from RCP changes by cache
		    ** locks.
		    */
		    if (dm0p_recover_state(log_id))
		    {
			status = force_page(b, tbio, FP_TOSS, lock_list,
			    log_id, BMCL_NO_GATHER, (i4*)0, err_code);
			if (status != E_DB_OK)
			{
			    /*
			    ** Unexpected error - log error and invalidate page.
			    */
			    ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG,
				NULL, (char *)0, (i4)0, (i4 *)0,
				err_code, 0);
			    if ( force_page(b, tbio, FP_INVALID, 
				lock_list, log_id, BMCL_NO_GATHER, (i4*)0, err_code) )
				dm0p_munlock(&b->buf_mutex);

			    dm0p_log_error( E_DM9C83_DM0P_CACHEFIX_PAGE, err_code, 
				    ((action & DM0P_USER_TID) == 0)); 

			}
			return (status);
		    }
		}
                else if (action & DM0P_TABLE_LOCKED_X)
                {
                   /* As the table is locked exclusively, there is no need
                   ** for this buffer to be flushed on unfix.
                   */
                   b->buf_sts &= ~BUF_WUNFIX;
                }

		/*
		** If doing online Cluster REDO recovery (RedoLSN != NULL)
		** fault_page did not acquire a cache lock.
		** Compare the page LSN with RedoLSN; if it's GTE, REDO is
		** not required and we can return to dmve without the lock.
		** If the page LSN is LT RedoLSN, then we toss the buffer,
		** nullify RedoLSN, and start over, this time getting a
		** cache lock.
		** See dmve_cachefix_page.
		*/
		if ( RedoLSN &&
		     LSN_LT(DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(b->buf_relpgtype, *page), 
			    RedoLSN) )
		{
		    if ( RedoLSNDebug )
		    {
			LG_LSN	*PageLSN = DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(b->buf_relpgtype, *page);
			TRdisplay("%@ dm0p_cachefix_page %d: RedoLSN %x.%x PageLSN %x.%x for %d,%d,%d\n", 
				    __LINE__, 
				    RedoLSN->lsn_high, RedoLSN->lsn_low,
				    PageLSN->lsn_high, PageLSN->lsn_low,
				    b->buf_lock_key.lk_key2, b->buf_lock_key.lk_key3,
				    b->buf_lock_key.lk_key4);
		    }
		    invalidate_page(b, lock_list);
		    RedoLSN = (LG_LSN*)NULL;
		    continue;
		}

		if ( RedoLSN && RedoLSNDebug )
		{
		    LG_LSN	*PageLSN = DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(b->buf_relpgtype, *page);
		    TRdisplay("%@ dm0p_cachefix_page %d: RedoLSN %x.%x PageLSN %x.%x for %d,%d,%d\n", 
				__LINE__, 
				RedoLSN->lsn_high, RedoLSN->lsn_low,
				PageLSN->lsn_high, PageLSN->lsn_low,
				b->buf_lock_key.lk_key2, b->buf_lock_key.lk_key3,
				b->buf_lock_key.lk_key4);
		}

		/* If BUF_IOWAIT-ers, wake them now. */
		if ( b->buf_sts & BUF_IOWAIT )
		    awaken_buf_waiters(b, lock_list);

		dm0p_munlock(&b->buf_mutex);
	    }

            if ((DB_SUCCESS_MACRO(status)) &&
                (cl_status == OK))
            {
		return (status);
            }

	    dm0p_log_error( E_DM9C83_DM0P_CACHEFIX_PAGE, err_code, 
			((action & DM0P_USER_TID) == 0)); 

	    return (E_DB_ERROR);
	}


	/*
	** Buffer was found in hash chain.
	** Buffer's buf_mutex is locked.
	*/
	
	/*  Check for I/O in progress. */
	if (b->buf_sts & BUF_BUSY)
	{
	    /*
	    **	IO_RETRY
	    */

	    /*
	    ** I/O in progress. Wait for completion before retrying.
	    ** Set the busy wait event before release buf_mutex.
	    */
	    b->buf_sts |= BUF_IOWAIT;
	    
	    lbm_stats->bs_iowait[BMCB_PTYPES]++;
	    bm->bm_stats.bs_iowait[BMCB_PTYPES]++;
	    lbm_stats->bs_iowait[b->buf_type]++;
	    bm->bm_stats.bs_iowait[b->buf_type]++;
	    (VOID) dm0p_eset(lock_list, DM0P_BIO_EVENT, b->buf_id.bid_cache, 
				b->buf_id.bid_index, 0);
	    dm0p_munlock(&b->buf_mutex);

	    /*
	    ** Wait for the I/O event to complete,
	    ** then restart search of hash chain.
	    */
	    (VOID) dm0p_ewait(lock_list, DM0P_BIO_EVENT, b->buf_id.bid_cache, 
				b->buf_id.bid_index, 0, 0);
	    continue;
	}

	/*
	** If doing online Cluster REDO recovery (RedoLSN != NULL)
	** we only need a cache lock if the
	** page LSN is LT RedoLSN; if GTE, REDO isn't needed
	** and dmve will ignore the page. 
	** See dmve_cachefix_page.
	**
	** Note that if we do need a cache lock, we can't simply invalidate
	** the buffer as the page may have been modified, so we
	** call force_page(FP_TOSS) to maybe write it, then dump it from
	** the cache.
	*/
	if ( RedoLSN && !(b->buf_sts & BUF_LOCKED) )
	{
	    /* No cache lock. Look at the page LSN */
	    *page = (DMPP_PAGE *)((char *)lbm->lbm_pages[cache_ix] + 
			    b->buf_page_offset);

	    /* Toss (and write, if need be) this buffer, refault page with cache lock */
	    if ( LSN_LT(DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(b->buf_relpgtype, *page), RedoLSN) )
	    {
		if ( RedoLSNDebug )
		{
		    LG_LSN	*PageLSN = DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(b->buf_relpgtype, *page);
		    TRdisplay("%@ dm0p_cachefix_page %d: RedoLSN %x.%x PageLSN %x.%x for %d,%d,%d\n", 
				__LINE__, 
				RedoLSN->lsn_high, RedoLSN->lsn_low,
				PageLSN->lsn_high, PageLSN->lsn_low,
				b->buf_lock_key.lk_key2, b->buf_lock_key.lk_key3,
				b->buf_lock_key.lk_key4);
		}

		if ( status = force_page(b, tbio, FP_TOSS, lock_list,
					log_id, BMCL_NO_GATHER, (i4*)0, err_code) )
		{
		    /*
		    ** Unexpected error - log error and invalidate page.
		    */
		    ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG,
			NULL, (char *)0, (i4)0, (i4 *)0,
			err_code, 0);
		    if ( force_page(b, tbio, FP_INVALID, 
			lock_list, log_id, BMCL_NO_GATHER, (i4*)0, err_code) )
		    {
			dm0p_munlock(&b->buf_mutex);
		    }

		    dm0p_log_error( E_DM9C83_DM0P_CACHEFIX_PAGE, err_code, 
			    ((action & DM0P_USER_TID) == 0)); 
		    return (status);
		}
		RedoLSN = (LG_LSN*)NULL;
		continue;
	    }
	}

	if ( RedoLSN && RedoLSNDebug )
	{
	    LG_LSN	*PageLSN;
	    *page = (DMPP_PAGE *)((char *)lbm->lbm_pages[cache_ix] + 
			    b->buf_page_offset);
	    PageLSN = DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(b->buf_relpgtype, *page);
	    TRdisplay("%@ dm0p_cachefix_page %d: RedoLSN %x.%x PageLSN %x.%x for %d,%d,%d\n", 
			__LINE__, 
			RedoLSN->lsn_high, RedoLSN->lsn_low,
			PageLSN->lsn_high, PageLSN->lsn_low,
			b->buf_lock_key.lk_key2, b->buf_lock_key.lk_key3,
			b->buf_lock_key.lk_key4);
	}

	/*
	** Handle cache protocol for multiple servers.
	** Make sure that another server (or the RCP) hasn't changed the 
	** page we had stored in our cache.  If so, then we need to get a
	** new copy from disk.
	**
	** Pages not marked BUF_CACHED do not have to be validated.
	** When running FAST-COMMIT, most pages will not be marked BUF_CACHED.
	** They are protected by fast commit protocols (all users of the
	** database must go through the same cache).
	*/

	lbm_stats->bs_fix[b->buf_type]++;
	bm->bm_stats.bs_fix[b->buf_type]++;
	lbm_stats->bs_hit[b->buf_type]++;
	bm->bm_stats.bs_hit[b->buf_type]++;

	lbm_stats->bs_hit[BMCB_PTYPES]++;
	bm->bm_stats.bs_hit[BMCB_PTYPES]++;

        /*
	** (ICL phil.p)
	** When running DMCM, its possible that the last time
        ** we used this buffer, BUF_CACHED didn't get set. To
        ** get around this, we use the fact the we are running
        ** DMCM to flag that it must still be validated.
	**
	** Only check further if BUF_LOCKED; if false, there's
	** no cache lock on this page, noting that none was
	** set for readonly db, temp tables.
	**
	** If already locked LK_X, no need to validate_page()
        */

	if ( b->buf_sts & BUF_LOCKED && b->buf_lock_mode != LK_X &&
            (b->buf_sts & BUF_CACHED ||
            (((b->buf_lk_id.lk_unique != 0 &&
               bm_common->bmcb_status & BMCB_CLUSTERED )
	      || b->buf_sts & BUF_DMCM))) )
	{
	    /*
	    **	VALIDATE_PAGE
	    **
	    **	Check that this page is the most recent version.
	    **  Validate returns holding the MUTEX.
	    */
	    /*
	    ** Increase the fix reference count for a moment.
	    ** validate_page() may mark the buffer BUF_BUSY and  
	    ** release the mutex, opening a window for uncache_fix 
	    ** (which does not wait for busy buffers) to decrement
	    ** the fix count, realize it's zero, and force the
	    ** page while we're still trying to validate it.
	    */
	    /*
	    ** No longer necessary. dm0p_uncache_fix() now waits
	    ** for busy buffers.
	    ** 05-Feb-1999 (jenjo02)
	    */

	    status = validate_page(tbio, b, action, lock_list, log_id,
				   access_mode, err_code);

	    if (status != E_DB_OK)
	    {
		/* buf mutex released if error */
		if (*err_code > E_DM_INTERNAL)
		{
		    ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL, 
			(char *)0, (i4)0, (i4 *)0, err_code, 0);
		    *err_code = E_DM9C83_DM0P_CACHEFIX_PAGE;
		}
		
		return (status);
	    }
	}

	/*
	** If this is a READNOLOCK query, then copy the page to a special
	** readnolock buffer for this session.  This allows the reader to
	** continue to access the buffer without worrying about interfering
	** with a writer who wishes to update the physical contents of the
	** page.
	**
	** We must hold a page mutex while making the copy so we don't copy
	** the page while it is in the middle of being changed.
	*/
	if (action & DM0P_READNOLOCK)
	{
	    /*
	    ** If the buffer is already mutex'd, then wait for it to be
	    ** marked free.  Note that we don't actually set the BUF_MUTEX
	    ** bit since we hold the entire buffer manager locked during 
	    ** the copy.  If in the future we want to release the buffer
	    ** manager mutex during the copy, we would need to set the mutex
	    ** bit here and check for waiters when finished.
	    */

	    if (b->buf_sts & BUF_MUTEX)
	    {
		b->buf_sts |= BUF_MWAIT;

		lbm_stats->bs_mwait[BMCB_PTYPES]++;
		bm->bm_stats.bs_mwait[BMCB_PTYPES]++;
		lbm_stats->bs_mwait[b->buf_type]++;
		bm->bm_stats.bs_mwait[b->buf_type]++;

		/* Set the wait for mutex event before release the mutex. */
		(VOID) dm0p_eset(lock_list, DM0P_PGMUTEX_EVENT, 
			b->buf_id.bid_cache, b->buf_id.bid_index, 0);
		dm0p_munlock(&b->buf_mutex);

		/*
		** Wait for the page mutex to be released.
		*/
		(VOID) dm0p_ewait(lock_list, DM0P_PGMUTEX_EVENT,
			b->buf_id.bid_cache, b->buf_id.bid_index, 0, 0);
		continue;
	    }
	    /*
	    ** (Must be FIXED (on no state queue) to adjust priority)
	    **
	    ** Adjust the buffer priority unless the buffer is marked
	    ** as having a fixed priority.
	    ** The priority is incremented each time the buffer is
	    ** found in cache and fixed.
	    */
	    if ( b->buf_sts & BUF_FIXED &&
	        (b->buf_sts & BUF_PRIOFIX) == 0 &&
	         b->buf_priority < bm_common->bmcb_maximum[b->buf_type] )
	    {
	        if ( b->buf_priority += bm_common->bmcb_rereference[b->buf_type]
			> bm_common->bmcb_maximum[b->buf_type] )
		{
		    b->buf_priority = bm_common->bmcb_maximum[b->buf_type];
		}
		
		if ( b->buf_owner != BUF_SELF )
		{
		    g = &group_array[b->buf_owner];

		    /*
		    ** If this is the first buffer of the group,
		    ** set new group priority to buffer's priority.
		    **
		    ** This will become the group's priority when it is moved
		    ** to a modified or free queue.
		    */
		    if ( b->buf_id.bid_index == g->g_buffer )
		    {
			dm0p_mlock(SEM_EXCL, &g->g_mutex);
			g->g_new_pri = b->buf_priority;
			dm0p_munlock(&g->g_mutex);
		    }
		}
	    }

	    MEcopy((PTR) (char *)lbm->lbm_pages[cache_ix] + b->buf_page_offset,
		bm->bm_pgsize, (PTR) readnolock_buffer);
	    *page = readnolock_buffer;

	    dm0p_munlock(&b->buf_mutex);

	    return (E_DB_OK);
	}

	/* Store FCB pointer for possible CP */
	b->buf_cpfcb = (PTR)tbio->tbio_location_array[
	    (page_number >> DI_LOG2_EXTENT_SIZE) % tbio->tbio_loc_count].loc_fcb;


	/*  If not already fixed, remove buffer from modify or free state queue */

	if ((b->buf_sts & BUF_FIXED) == 0)
	{
	    if (b->buf_owner == BUF_SELF)
	    {
		/*
		** Remove buffer from the modify or free state queue.
		*/
		if (b->buf_sts & BUF_MODIFIED)
		    sth = &bm->bm_mpq[b->buf_priority];
		else
		    sth = &bm->bm_fpq[b->buf_priority];

		dm0p_mlock(SEM_EXCL, &sth->sth_mutex);

		next_buf = b->buf_state.stq_next;
		prev_buf = b->buf_state.stq_prev;
		if (next_buf != BUF_ENDLIST)
		    buf_array[next_buf].buf_state.stq_prev = prev_buf;
		else
		    sth->sth_stq.stq_prev = prev_buf;
		if (prev_buf != BUF_ENDLIST)
		    buf_array[prev_buf].buf_state.stq_next = next_buf;
		else
		    sth->sth_stq.stq_next = next_buf;

		/* Count one less free/mod buffer, one more fixed */
		sth->sth_count--;
		sth->sth_lcount++;

		dm0p_munlock(&sth->sth_mutex);
	    }

	    /* Buffer is now fixed and on no state queue */
	    b->buf_state.stq_next = b->buf_state.stq_prev = 0;

	    /*
	    ** Check if buffer needs to be removed from its current transaction
	    ** queue.  Buffers fixed only for READ are not kept on any xact
	    ** queue until they are unfixed.  Buffers fixed for WRITE need to
	    ** put onto the correct transaction queue.
	    **
	    ** If the buffer is being fixed for write and is on the wrong queue
	    ** or if it is being fixed for read and is on any tranaction queue,
	    ** then remove it.
	    **
	    ** This check needs only be made when the buffer is first fixed.
	    ** The code to put the buffer onto its transaction queue if
	    ** appropriate is checked below.
	    */
	    if (((b->buf_tran_id.db_low_tran != tran_id->db_low_tran) ||
		 (b->buf_tran_id.db_high_tran != tran_id->db_high_tran) ||
		 (action & DM0P_INTENT_LK) ||
		 (access_mode == DM0P_READ)) &&
		(b->buf_txnq_bucket != BUF_NOBUCKET))
	    {
		/* Delete this buffer from transaction queue */
		txnq_delete(b);
	    }
	}

	/*
	** If the buffer is locked for WRITE and is not on a transaction queue,
	** then put it on the appropriate one.  We put exclusively held buffers
	** on the queue according to the transaction id that holds it.
	**
	** Shared buffers are not put onto the transaction queue until the last
	** reference goes away, at that time they are put on the queue of the
	** last transaction to reference them.
	**
	** We can't make this check only in the case where the buffer is first
	** fixed, as it may be fixed for READ before being fixed for WRITE.
	*/
	if ((access_mode == DM0P_WRITE) && (b->buf_txnq_bucket == BUF_NOBUCKET)
	    && (action & DM0P_INTENT_LK) == 0)
	{
	    STRUCT_ASSIGN_MACRO(*tran_id, b->buf_tran_id);
	    if (tran_id->db_high_tran | tran_id->db_low_tran)
	    {
		/*  Insert buffer on the tran queue. */
		txnq_insert(b, log_id);
	    }
	}

	/* whether fixed already or not (eg just read or not), raise */
	/* the priority if requested, probably for a page that was	 */
	/* requested on behalf of a readahead thread, so the page    */
	/* stays around long enough to benefit user thread. 	 */
	if ((action & DM0P_HI_PRIORITY) && b->buf_type == BMCB_LEAF)
	{
	    b->buf_sts |= BUF_HI_TPRIO;
	    if (++(b->buf_priority) > bm_common->bmcb_maximum[b->buf_type])
		b->buf_priority = bm_common->bmcb_maximum[b->buf_type];
	}
	/*
	** Adjust the buffer priority unless the buffer is marked
	** as having a fixed priority.
	** The priority is incremented each time the buffer is
	** found in cache and fixed.
	*/
	else if ( (b->buf_sts & BUF_PRIOFIX) == 0 &&
		   b->buf_priority < bm_common->bmcb_maximum[b->buf_type] )
	{
	    if ( (b->buf_priority += bm_common->bmcb_rereference[b->buf_type])
		    > bm_common->bmcb_maximum[b->buf_type] )
		{
		    b->buf_priority = bm_common->bmcb_maximum[b->buf_type];
		}
	}

	if ( b->buf_owner != BUF_SELF )
	{
	    i4 rdelta, mdelta;

	    /*
	    ** Requeue the group queue if needed.
	    */
	    g = &group_array[b->buf_owner];
	    dm0p_mlock(SEM_EXCL, &g->g_mutex);

	    /*
	    ** If this is the first buffer of the group,
	    ** set new group priority to buffer's priority.
	    **
	    ** This will become the group's priority when it is moved
	    ** to a modified or free queue.
	    */
	    if ( b->buf_id.bid_index == g->g_buffer )
		g->g_new_pri = b->buf_priority;

	    /*
	    ** If buffer was not previously fixed, count another
	    ** fixed buffer in this group.
	    */
	    rdelta = mdelta = 0;
	    if ( (b->buf_sts & BUF_FIXED) == 0 )
	    {
		rdelta = 1;

		if ( b->buf_sts & BUF_MODIFIED )
		  mdelta = -1;
	    }

	    /* Use move_group to decide what to do with the group.
	    ** The group won't become empty.
	    */
	    move_group(bm, g, b, lock_list, rdelta, mdelta, 0);

	    dm0p_munlock(&g->g_mutex);
	}

	/*
	** Since the buffer is fixed, it is protected from being updated
	** by another server, so we can turn off the BUF_CACHED bit so
	** other fixers of this page do not have to validate it.
	*/
	b->buf_sts &= ~BUF_CACHED;

	/*  Finish initializing the buffer header. */

	b->buf_sts |= (BUF_FIXED | BUF_VALID);

	b->buf_refcount++;

	if (access_mode == DM0P_WRITE)
	    b->buf_sts |= BUF_WRITE;
	if (action & DM0P_NOLOCK)
	    b->buf_sts |= BUF_NOTXLOCK;
        if (action & DM0P_INTENT_LK)
            b->buf_sts |= BUF_INTENT_LK;

	*page = (DMPP_PAGE *)((char *)lbm->lbm_pages[cache_ix] + 
			 b->buf_page_offset);

        /* As the page existed in memory, ensure that the BUF_WUNFIX flag
        ** is set correctly. It may have been cleared due to an ABORT. The
        ** BUF_WUNFIX flag should be set if BUF_WUNFIX_REQ set and
        ** DM0P_TABLE_LOCKED_X action not set.
        */
        if (b->buf_sts & BUF_WUNFIX_REQ)
        {
           if (action & DM0P_TABLE_LOCKED_X)
           {
              b->buf_sts &= ~BUF_WUNFIX;
           }
           else
           {
              b->buf_sts |= BUF_WUNFIX;
           }
        }

	/*
	** If the CREATE flag was passed in, then format the page as
	** a new data page.  Note that most CREATE operations will format
	** the page through fault_page as we usually only CREATE pages that
	** did not previously exist in the table (and thus would not exist
	** in the cache.  It is possible, however to use CREATE to reformat
	** an existing cache page.
	**
	** If the CREATE_NOFMT flag was passed in, do not format the page.
	** Instead, mutex the page and return it to the caller.
	*/
	if (action & (DM0P_CREATE | DM0P_CREATE_NOFMT))
	{
	    if (action & DM0P_CREATE)
		(*format_routine)(tbio->tbio_page_type, tbio->tbio_page_size,
			*page, page_number, DMPP_DATA, DM1C_ZERO_FILL);
	    else if ((dmf_svcb->svcb_status & SVCB_SINGLEUSER) == 0)
		b->buf_sts |= BUF_MUTEX;
	}

        if (bm_common->bmcb_status & BMCB_FCFLUSH &&
		b->buf_sts & BUF_WRITE &&
                (b->buf_sts & BUF_MODIFIED) == 0 &&
                (DMPP_VPT_GET_PAGE_STAT_MACRO(b->buf_relpgtype, *page) & 
	          DMPP_MODIFY) == 0)
        {
            /*
            ** If the server is executing a Consistency Point, and this
            ** buffer was not dirty previous to this fix and the page 
	    ** isn't modified then mark this buffer as flushed for the 
	    ** current CP.  This is OK since the buffer was not modified 
	    ** previous to the current CP.
            **
            ** This is necessary due to Multi-Server protocols.  If
            ** multiple servers are cooperating to execute a CP, we may
            ** be modifying a page that cannot be processed by the other
            ** servers (due to not having a TCB).  If our server's CP
            ** thread has already passed this buffer, but others haven't,
            ** then a CP protocol failure will occur if this buffer gets
            ** modified before the last CP thread passes it.
            **
	    ** NOTE: I think that this update - along with the similar
	    ** buf_cpcount settings in fault_page and gfault_page are no
	    ** longer necessary.  I think they have been superceeded by the
	    ** fixes to add similar settings in force_page and gforce_page,
	    ** which is where this should have gone in the first place.
	    ** - rogerk
            */

	    b->buf_cpcount = bm_common->bmcb_cpcount;
        }

	dm0p_munlock(&b->buf_mutex);
	return (E_DB_OK);
    }
}

/*
** Name: fault_page	- Fault a page from disk into the buffer pool.
**
** Description:
**      This routine allocates a free buffer in the buffer pool and
**	reads the buffer from disk.  The buffer manager guarantees that a
**	free buffer is always available.  A free buffer is a buffer that
**	is not fixed and not modified.  If there are no free buffers, then
**	the caller will write dirty buffers in order to free them up.  The
**	caller that writes dirty buffers will return with the action set to
**	RETRY, so that a new search of the buffer pool is initiatated to see
**	if the page exists now.
**
**	The following diagram depicts the control flow:
**
**		    
**		    ALLOCATE_PAGE
**			|--------------+
**		    FREE_PAGE_WAIT     |
**		+-------|<-------------+
**		|   FORCE_PAGE
**		+------>|--------------+
**		        |
**		    INITIALIZE_BUFFER
**			|--------------+
**		    CACHE_LOCK         |
**              +-------|<-------------+
**		|   READ_PAGE
**		|       |
**		|   PAGE_TYPE
**		+------>|
**		    UNMUTEX
**			|
**		    RETURN
**
**	The caller must hold the buffer manager mutex when calling this
**	routine.  The mutex will be released while lock and I/O calls
**	are made.
**
**	When called with the DM0P_CREATE flag, a new data page is allocated
**	in the buffer cache, but no page is read from disk.  The buffer is
**	marked BUF_EXTENDPAGE until it is forced to disk.  This prevents any
**	asynchronous threads from trying to force the page until it can be
**	safely written to disk.
**
** Inputs:
**      rcb                             The RCB for the table.
**      action                          The action to fix page, indicated
**                                      if this is a CREATE request.
**	page_number			The page number in the table.
**	access_mode			The page access mode: read or write.
**	bucket				The hash bucket for this buffer.
**
** Outputs:
**	buffer				The allocated buffer.
**	err_code			Reason for error return status.
**	Returns:
**	    E_DB_OK			Success.
**	    E_DB_WARN			Restart operation.
**	    E_DB_ERROR			Error occured.
**	    E_DB_FATAL			DMF fatal error.
**	Exceptions:
**	    E_DM9202_BM_BAD_FILE_PAGE_ADDR err_code is expected
**          when extenting an old file with the new allocation scheme.
**
** Side Effects:
**	The buffer is left in BUSY state while Lock and IO calls are made.
**
** History:
**	04-jun-1986 (Derek)
**          Created for Jupiter.
**	22-Jan-1987 (ac)
**	    Added code for readahead support.
**	30-sep-1988 (rogerk)
**	    Added write behind thread support.  If free count is below
**	    free limit, then signal the write behind threads to free up some
**	    space in the buffer manager.
**	31-oct-1988 (rogerk)
**	    When moving modified buffer to new priority queue, put it at the 
**	    end of the queue instead of at the beginning.  This is so older
**	    pages will be forced out first causing us to do fewer log forces.
**	28-feb-1989 (rogerk)
**	    Added support for Shared Buffer Manager.
**	    Moved check for out-of-buffers below for performance.
**	    Moved formatting of buffer descriptor before researching hash
**	    chains after force_page.  This is to make it easier to break
**	    up the buffer manager mutex.
**	    Turn off BUF_TEMP_FIXED status before returning.
**	10-jul-1989 (rogerk)
**	    When creating scratch page, turn on BUF_EXTENDPAGE to signify
**	    that this is a newly allocated page.  This will prevent other
**	    threads (fc thread) from trying to force it out of the cache,
**	    since it can only be forced by the server which allocated it.
**	07-aug-1989 (mikem)
**	    Do not automatically turn off BUF_EXTENDPAGE bit at bottom of page
**	    (previous code turned it off thinking it was an error case rather than
**	    the normal path through.)
**	28-aug-1989 (rogerk)
**	    Pass correct arguments to dm0p_ewait routine when waiting for
**	    a buffer to become free.
**	 2-oct-1989 (rogerk)
**	    Fix NOLOCKS bug.  When can't acquire cache lock, set status to
**	    E_DB_ERROR correctly.
**	 2-oct-1989 (rogerk)
**	    Fix Consistency Point Bug.  If fault in buffer while in the
**	    middle of a consistency point, mark the buffer with the current
**	    CP number.  This indicates that this buffer does not need to be
**	    flushed as part of the current CP.
**	30-oct-1989 (rogerk)
**	    When invalidate page that got an IO error, don't decrement the
**	    fixed count as it is done in invalidate_page.
**      11-sep-1990 (jonb)
**	    Integrate ingres6301 change 280136 (rogerk):
**            When we fault in a page, write the tcb's validation stamp into
**            the buffer header.  This allows other servers attached to a common
**            buffer manager to validate their tcb's without having to re-
**            request the tcb lock.  Also, pass buf_tcb_stamp value to 
**            dm2t_get_tcb_ptr.
**	 4-feb-1991 (rogerk)
**	    Added support for fixed buffer priorities.
**	    Added checks in fault_page for table's priority to override the
**	    buffer manager default priorities.  Added checks for BUF_PRIOFIX
**	    before altering a buffer's priority by clock or on reference.
**	3-may-1991 (bryanp)
**	    Make dmd_checks more useful by logging detailed error information
**	    before calling dmd_check.
**	17-oct-1991 (rogerk)
**	    Merged changes made by Derek during the file extend project.
**	    Handle new Write-on-Unfix and Write-on-PassAbort buffer flags.
**	    Call dm1c_format to initialize page instead of doing it inline.
**	    Added trace flags in xDEBUG code.
**	    Added new session statistics collecting.
**	    Added new error messages.
**	19-Nov-1991 (rmuth)
**	    Removed the action type DM0P_FHFMSYNC, check the page type
**	    to see if it is a FMAP or FHDR and act accordingly. See
**	    code for comments.
**      21-jan-92 (jnash)
**	    If FHDR/FMAP, Btree ROOT or INDEX page faulted in, mark
**	    PAWRITE.  If Pass Abort, also mark TUNFIX.
**	05_jun_1992 (kwatts)
**	    MPF change, call to format page now uses accessor.
**	07-oct-1992 (jnash)
**	    6.5 reduced logging project.
**	     -  Validate the page checksum after reading the page.
**	26-oct-1992 (rogerk)
**	  - Reduced logging project: changed references to tcb fields to
**	    use new tcb_table_io structure fields.
**	  - Changed the code which picks buffer weighting constants to be
**	    less dependent upon the structure types and gear more off of
**	    the page type itself.  During many page recovery operations, the
**	    storage type of the table is unknown.  Also made CHAIN page
**	    type mean the same as LEAF for priority weighting since it is
**	    a type of leaf page.  Changed from old dm2t_get_tcb_ptr to new 
**	    fix_tableio_ptr.  Added check to bypass group operations with 
**	    partial tcb's.
**	  - Changed lock_mode parameter to access_mode to divorce a little
**	    bit from page/table lock modes.
**	  - Changed DM0P_SCRATCH page handling to not mark the new
**	    page as MODIFIED.  The page is formatted (since a garbage page
**	    cannot be left in the cache), but it is up to the caller to
**	    format the page appropriately, write the correct LSN, tran_id
**	    and sequence number, and to mark the page as modified.
**	  - Took out old settings of cache locks on pages marked WUNFIX
**	    or readnolock during passabort.  These are handled now by
**	    tossing the pages at unfix time.
**	  - Removed requirement that system catalog pages use cache locks.
**	    They are now handled with the special Pass-Abort protocols
**	    shared by FMAP,FHDR and Btree Index pages.
**	  - Changed error handling a little bit to make sure trace back erorr
**	    messages are logged.  Moved check for error return from page
**	    read operations to before buffer fields begin to be initialized.
**	30-October-1992 (rmuth)
**	    Changes the read past end-of-file checking, see the 
**	    tcb_lpageno check for more info.
**	14-dec-1992 (rogerk)
**	    Changed temporary tables to not set PAWRITE fields since there are
**	    no concurrency or recovery issues.
**	05-mar-1993 (andys)
**	    Use DB_FAILURE_MACRO to check return status of dm0p_reclaim
**	    as it sometimes returns E_DB_WARN which is OK for us right here
**	    right now.
**	08-feb-1993 (rmuth)
**	    If encounter an error reading or checking the page call
**	    dm0p_log_error as this routine understands which errors to
**	    preserve and pass upto the higher layers.
**	24-may-1993 (bryanp)
**	    Trace cache locking if trace point DM302 is set.
**	23-aug-1993 (andys)
**	    Don't use DB_FAIL_MACRO in [g]fault_page calls to dm0p_reclaim
**	    so that we know when we really can't get any more locks.
**	18-oct-1993 (rmuth)
**	    Add DMP_BMCB statistics.
**	18-oct-1993 (rogerk)
**	    Changed the flimit enforcing code to select pages off of the
**	    start of the modify priority queues rather than the end of
**	    the queues to choos the least-recently-used pages as force
**	    victims.
**	22-feb-1994 (andys)
**	    Add call to dm0p_check_consistency.
**	23-may-1994 (andys)
**	    Add new parameter to dm0p_log_error which says whether to print
**	    the error which we are throwing away. We pass the flag saying not 
**	    to log the error when the flag DM0P_USER_TID is passed to 
**	    dm0p_fix_page, dm0p_cachefix_page or fault_page. This flag is
**	    only currently set by dm1r_get when it is doing a get by tid.
**	    [bug 49726]
**	26-jun-1995 (cohmi01)
**	    When selecting victim from free chain, take 'next' LRU buffer.
**      24-oct-1995 (thaju02/stoli02)
**          Added increment of buffer manager single sync write counter.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	20-mar-1996 (nanpr01)
**	    Added page size as parameter to this routine. To compute
**	    the checksum pagesize is need since sizeof(*page) returns
**	    only the size of physical page.
**      06-may-1996 (nanpr01)
**          Change the reference to the page header with macros for
**	    New Page Format. 
**          Change the format routine paramter to i4 for u_i2.
**      01-aug-1996 (nanpr01 for ICL phil.p)
**          Introduced support for Distributed Multi-Cache Management (DMCM)
**          protocol. For DMCM, lock requests are made via
**          LKrequestWithCallback.
**	01-aug-1996 (nanpr01)
**	    If error condition, we have to invalidate the LKcallback
**	    routines. Page number checking was being done later had to
**	    be moved forward.
**	13-Sep-1996 (jenjo02)
**	    On successful return, assigned buffer will be returned 
**	    with its buf_mutex locked.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Set BUF_X_LATCH, BUF_WUNFIX/BUF_PAWRITE when EXCL row locking
**	12-Feb-1997 (jenjo02)
**	  - Skip over buffers that are BUF_BUSY.
**	  - Use a free buffer instead of forcing a modified buffer as
**	    long as there are free buffers.
**	  - Give preference to modified pages that won't induce a
**	    log force.
**	  - Delay waking buffer waiters until we're about to return.
**	  - Use SHARE mutex when searching modified/free queues.
**	24-Feb-1997 (jenjo02 for somsa01)
**	    Cross-integrated 428939 from main.
**	    Added tbio_extended to checking fo Buffer Manager Force Protocols
**	    on core catalogs.
**	01-Apr-1997 (jenjo02)
**	    Added stats for sychronous fixed priority forces.
**	    When looking for a buffer from the modify/free queues, initially skip
**	    over valid buffers with fixed priority; if we exhaust the queues
**	    without finding a non-fixed-priority buffers, rescan to force
**	    one.
**      21-may-1997 (stial01)
**          fault_page() Do not put buf with IX page lock on xn queue
**          Removed code for non-fast-commit when row locking (not allowed) 
**          Removed code for BUF_PAWRITE when IX lock. (pass abort not allowed)
**	19-Jan-1998 (jenjo02)
**	    When page is successfully faulted in, add it to the table's
**	    hash queue.
**	25-Jun-1998 (jenjo02)
**	    Added test for DM0P_CREATE_COND action when faulting in a non-matching
**	    page. This flag is set by dm1p when it's unsure whether the "free"
**	    datapage it's trying to fix has been formatted or not. If this flag
**	    is on and we read a "bad" page, ignore the error and call the
**	    page formatting function.
**	07-Jul-1998 (jenjo02)
**	    Defer releasing buf_mutex until after all buf_sts bits have been
**	    decided upon and set.
**	01-Sep-1998 (jenjo02)
**	    DM0P_CREATE_COND has become DM0P_CREATE_NOFMT. When passed, the 
**	    page is read but, instead of being formatted, it is mutexed and
**	    returned to the caller (dm1p.c, scan_map()) in that state.
**	03-Dec-1998 (jenjo02)
**	    Removed test for, and skipping of, fixed priority buffers when
**	    scanning the modified and free queues. It ate CPUs and was really
**	    of no benefit.
**      12-jan-1999 (stial01)
**          fault_page() DM0P_CREATE_NOFMT: mutex buffer if !SINGLEUSER
**	21-Jan-1999 (jenjo02)
**	    In fault_page(), read the page if DM0P_CREATE_NOFMT and the page
**	    to be read is within the bounds of formatted pages for the table
**	    (page is being reused).
**	27-Jan-1999 (jenjo02)
**	    In fault_page(), put the table in hash bucket before acquiring
**	    the cache lock and reading the page. This should be OK since the 
**	    buffer is busy. This will close the hole, when the cache lock is
**	    found yet the buffer is not found in the cache by traversing 
**	    through tables bucket list. 
**	24-Aug-1999 (jenjo02)
**	    When reclaiming a buffer, mark it BUF_RECLAIM to signal force_page()
**	    to remove it from whatever state queue it's on but not insert it
**	    on the free queue. A reclaimed page is in the process of becoming
**	    "fixed" and there's no need to put it on a free queue just to have
**	    to immediately remove it again.
**	14-Sep-1999 (jenjo02)
**	    Accurately count free buffers.
**	11-Oct-1999 (jenjo02)
**	    ifdef'd out clock aging. I don't know that this is really of any
**	    value, and it presents a multi-thread bottleneck on the clock_mutex.
**	    More tweaks to free/modified page search algorithms to ensure that
**	    we pick a free rather than modified page whenever possible.
**	10-Jan-2000 (jenjo02)
**	    Cleaned up free/modified page search.
**	    If faulting in a temptable page, reduce its buffer
**	    priority by one to cause earlier aging of those
**	    pages.
**	23-Jun-2000 (jenjo02)
**	    Added fill_option to dmpp_format() prototype, set by caller
**	    to DM1C_ZERO_FILL if dmpp_format should MEfill the page,
**	    DM1C_ZERO_FILLED if the page has already been MEfilled.
**	    The page overhead space is always zero-filled.
**	11-Oct-2000 (jenjo02)
**	    Turn off "search_mod" when modified queues exhausted
**	    to prevent endless loop.
**	06-Jun-2002 (jenjo02)
** 	    Test of flimit in fault_page() was reversed, counting
**	    free pages when WB threads were running instead of when
**	    they were not. 
**      19-Jun-2002 (horda03) Bug 108074.
**          Whenever BUF_WUNFIX flag is set/cleared set/clear the BUF_WUNFIX_REQ
**          flag. Thus we keep track of buffers that should be flushed from
**          cache when UNfixed (if not protected by an exclusive table level
**          lock).
**	28-Jan-2004 (jenjo02)
**	    Removed bogus
**	    DMPP_VPT_CLR_PAGE_STAT_MACRO(tbio->tbio_page_size, page, DMPP_MODIFY)
**	    cross-integrated from bug 108074, above. 
**	    "tbio_page_size" in this context rather than page_type
**	    made V1 pages look like >V1 pages, causing the DMPP_MODIFY
**	    bit (0x00000100) to be turned off in page_main, corrupting
**	    isam/hash chains.
**	01-Mar-2004 (jenjo02)
**	    Stuff partition number in buf_partno.
**      29-Sep-2004 (fanch01)
**          Conditionally add LK_LOCAL flag if database/table is confined
**          to one node.
**    28-Feb-2005 (wanfr01,jenjo02)
**          Bug 113547, INGSRV3062
**          Stop doing dirty reads if we are restarting the scan too
**          many times to avoid cpu thrashing.
**     6-sep-2005 (wanfr01, horda03)
**	    Bug 114776, INGSRV3348 
**	    Check the free queue count to avoid waiting needlessly
**	    for a WB thread, and make sure one is active if you do
**    22-sep-2006 (horda03) Bug 56705
**          Don't select a modified buffer to be forced, if the table
**          for the buffer is being materialized. This prevents a
**          semaphore deadlock.
**     20-sep-2006 (wonca01)
**          Bug 114776
**          Correction to cross integration of BUG 114776, where the
**          number of parameters passed to wb_cycle_start() differs
**          from the codeline where the change originated from.
**	28-Feb-2007 (jonj)
**	    Translate DM0P_NOLKSTALL action to LK_NOSTALL for cache locks.
**	06-Mar-2007 (jonj)
**	    Add RedoLSN parameter for online Cluster REDO.
**	    When non-NULL, do not acquire cache lock.
*/
static DB_STATUS
fault_page(
DM0P_BMCB		*bm,
DMP_TABLE_IO		*tbio,
i4                 action,
i4			page_number,
i4			bucket,
i4			lock_list,
i4			access_mode,
i4			log_id,
DB_TRAN_ID		*tran_id,
VOID			(*format_routine)(
			    i4		    page_type,
			    i4		    page_size,
			    DMPP_PAGE       *page,
			    DM_PAGENO       pageno,
			    i4         	    stat,
			    i4		    fill_option),
DM0P_BUFFER		**buffer,
LG_LSN			*RedoLSN,
i4			*err_code)
{
    DMP_LBMCB               *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BSTAT	    	    *lbm_stats = &lbm->lbm_stats[bm->bm_cache_ix];
    DM0P_BM_COMMON          *bm_common = lbm->lbm_bm_common;
    DM0P_BUFFER		    *buf_array = lbm->lbm_buffers[bm->bm_cache_ix];
    DM0P_HASH_HEAD          *bucket_array = lbm->lbm_buckets[bm->bm_cache_ix];
    i4		    	    read_page = FALSE;
    DMP_TABLE_IO	    *buf_tbio = NULL;
    DM0P_BUFFER		    *b;
    DM0P_BUFFER		    *buf;
    DMPP_PAGE		    *page;
    CL_SYS_ERR		    sys_err;
    DB_STATUS		    status;
    STATUS		    cl_status;
    i4		    	    next;
    i4		    	    next_buf, prev_buf;
    i4		    	    hash_chain;
    i4		    	    i,j,n,p;
    i4                      lk_mode;   
    i4                 	    error_code;
    i4		    	    page_stat;
    i4		    	    first_pass = TRUE;
    i4		       	    buffers_skipped;

    i4			    search_free, search_mod;
    i4			    fcount;
    i4		    	    fqcount[DB_MAX_TABLEPRI];
    i4		    	    mqcount[DB_MAX_TABLEPRI];

    i4			    fpq_count, mpq_count,start_fpq_count;
    i4			    restarts;
	i4				lock_flags;
    i4			    old_lock_id;

    /*
    **	ALLOCATE_PAGE
    **
    **	If the free list is low, then force a modified buffer to disk in order
    **	to reclaim a page.  If there are no buffers on the modify list, then
    **	wait for a free page to become available.
    */

#ifdef USE_CLOCK_AGING
    /*
    **	Decrement the priority of the next single page buffer pointed
    **  at by the clock arm.  Skip over buffers marked BUF_PRIOFIX.
    ** Lock the clock while we update it.
    */
    dm0p_mlock(SEM_EXCL, &bm->bm_clock_mutex);
    b = &buf_array[bm->bm_clock];

    if (++bm->bm_clock > bm->bm_sbufcnt)
    {
	b = &buf_array[0];
	bm->bm_clock = 1;
    }

    dm0p_munlock(&bm->bm_clock_mutex);

    if (b->buf_mutex.bms_locked == 0 &&
       (b->buf_sts & (BUF_BUSY | BUF_VALID | BUF_PRIOFIX)) == BUF_VALID &&
        b->buf_priority > 0)
    {
	/*
	** Lock the buffer, check again.
	*/
	dm0p_mlock(SEM_EXCL, &b->buf_mutex);

        if ((b->buf_sts & (BUF_BUSY | BUF_VALID | BUF_PRIOFIX)) == BUF_VALID &&
	     b->buf_priority > 0)
	{
	    i4		    old_priority;

	    /*
	    ** If Fixed, just decrement priority.
	    **
	    ** If modified or free, decrement priority and 
	    ** requeue buffer on new priority state queue.
	    */
	    old_priority = b->buf_priority--;

	    if ((b->buf_sts & BUF_FIXED) == 0)
	    {
		DM0P_STATE_HEAD	    *old_sth, *new_sth;

		/*
		** Remove buffer from the modify or free state queue
		** of its old priority.
		*/

		if (b->buf_sts & BUF_MODIFIED)
		{
		    old_sth = &bm->bm_mpq[old_priority];
		    new_sth = &bm->bm_mpq[b->buf_priority];
		}
		else
		{
		    old_sth = &bm->bm_fpq[old_priority];
		    new_sth = &bm->bm_fpq[b->buf_priority];
		}

		dm0p_mlock(SEM_EXCL, &old_sth->sth_mutex);

		next_buf = b->buf_state.stq_next;
		prev_buf = b->buf_state.stq_prev;
		if (next_buf != BUF_ENDLIST)
		    buf_array[next_buf].buf_state.stq_prev = prev_buf;
		else
		    old_sth->sth_stq.stq_prev = prev_buf;
		if (prev_buf != BUF_ENDLIST)
		    buf_array[prev_buf].buf_state.stq_next = next_buf;
		else
		    old_sth->sth_stq.stq_next = next_buf;
		old_sth->sth_count--;

		dm0p_munlock(&old_sth->sth_mutex);

		/*
		** Now put the buffer on the same state queue at its new priority.
		** Put on the end of state queue so that pages already at this
		** priority will be bumped out first.
		*/
		dm0p_mlock(SEM_EXCL, &new_sth->sth_mutex);

		prev_buf = new_sth->sth_stq.stq_prev;
		b->buf_state.stq_prev = prev_buf;
		b->buf_state.stq_next = BUF_ENDLIST;
		if (prev_buf != BUF_ENDLIST)
		    buf_array[prev_buf].buf_state.stq_next = 
			b->buf_id.bid_index;
		else
		    new_sth->sth_stq.stq_next = b->buf_id.bid_index;
		new_sth->sth_stq.stq_prev = b->buf_id.bid_index;
		new_sth->sth_count++;

		dm0p_munlock(&new_sth->sth_mutex);
	    }
	}
	dm0p_munlock(&b->buf_mutex);
    }
#endif /* USE_CLOCK_AGING */

		

    b = NULL;
    search_free = TRUE;
    search_mod  = TRUE;

    do
    {
	/*
	** Search the free queues in the next direction.
	** Buffers are added to the tail of the queues,
	** so the most-aged will clutter at the front.
	** Dirty-read the free queues, without the aid of a mutex.
	*/
	fpq_count = 0;
	start_fpq_count = -1;

	for (i = 0; b == NULL && i < DB_MAX_TABLEPRI; i++)
	{
	    /*
	    ** If this priority queue has less than the
	    ** free limit number of buffers, we may have 
	    ** hit the limit in all priority queues.
	    ** This may have already been discovered and the
	    ** WriteBehind threads started to handle the
	    ** situation; if they're running, go ahead and
	    ** take a free buffer, otherwise
	    ** count the number of free buffers in all 
	    ** priority queues, compare that to flimit,
	    ** and if we're there, fire up the WriteBehind
	    ** threads and break out to take a
	    ** modified page instead. If there are no WriteBehind
	    ** threads in this cache, break out to force a
	    ** modified page.
	    */
	    if ( bm->bm_fpq[i].sth_count &&
	         (fpq_count += bm->bm_fpq[i].sth_count) < bm->bm_flimit &&
		 (bm->bm_status & BM_WBWAIT ||
		  (bm->bm_status & BM_WB_IN_CACHE) == 0) )
	    {
		DM0P_COUNT_QUEUE(bm->bm_fpq, fpq_count);
		/* Save this for another movement check later */
		start_fpq_count = fpq_count;

		if ( fpq_count < bm->bm_flimit ) 
		{
		    if ( bm->bm_status & BM_WBWAIT )
			wb_cycle_start(bm, lock_list, 
					BM_WB_SINGLE, "FAULT_PAGE");
		    break;
		}
	    }
	    else if (start_fpq_count == -1)
	    {
		/* insure you have a starting free count */
		DM0P_COUNT_QUEUE(bm->bm_fpq, start_fpq_count);
	    }

	    /* select victim from LRU portion */
	    next = bm->bm_fpq[i].sth_stq.stq_next;
	    restarts = MAX_RESTARTS;
	    
	    while (next != BUF_ENDLIST)
	    {
		b = &buf_array[next];
		next = b->buf_state.stq_next;

		if ((bm_common->bmcb_status & BMCB_IS_MT) && restarts > 0)
		{
		    /*
		    ** Skip those buffers whose mutex is locked; they're
		    ** likely being picked off by a concurrent thread.
		    */
		    if ( b->buf_mutex.bms_locked ||
			 b->buf_sts & 
			     (BUF_MODIFIED | BUF_FIXED | BUF_BUSY | BUF_RECLAIM) ||
			 b->buf_priority != i)
		    {
			/*
			** The buffer may be changing while we look at it.
			** If it's no longer on the free queue at this
			** priority, restart the search.
			*/
			if (b->buf_sts & (BUF_MODIFIED | BUF_FIXED) ||
			    b->buf_priority != i)
			{
			    restarts--;
			    next = bm->bm_fpq[i].sth_stq.stq_next;
			}
			b = NULL;
			continue;
		    }
		}

		restarts = MAX_RESTARTS;

		/*
		** Lock the buffer, check again.
		** By the time we get the buffer's mutex,
		** it may no longer be on the free queue.
		*/
		dm0p_mlock(SEM_EXCL, &b->buf_mutex);
		next = b->buf_state.stq_next;

		if ( b->buf_sts & (BUF_MODIFIED | BUF_FIXED) )
		{
		    /* Buffer no longer on free queue, restart */
		    next = bm->bm_fpq[i].sth_stq.stq_next;
		}
		else if ( b->buf_sts & (BUF_RECLAIM | BUF_BUSY) )
		{
		    /* Buffer is free, but busy */
		    if ( b->buf_priority != i )
			next = bm->bm_fpq[i].sth_stq.stq_next;
		}
		else
		    /* Got one */
		    break;

		dm0p_munlock(&b->buf_mutex);
		b = NULL;
	    } /* while (next != BUF_ENDLIST) */

	    /*
	    ** If we haven't found a buffer, find the
	    ** lowest priority queue on which new free buffers
	    ** have appeared, and restart from there.
	    ** If none, continue with next priority.
	    */
	    if ( b == NULL )
	    {
		/* Save the free count before leaving priority */
		fqcount[i] = bm->bm_fpq[i].sth_count;

		for ( j = 0; j < i; j++ )
		{
		    if ( bm->bm_fpq[j].sth_count > fqcount[j] )
			i = --j;
		}
	    }
	} /* for each free priority */

	if ( b )
	    break;

	/* Couldn't get a free buffer, get a modified one */
	search_mod = TRUE;
	search_free = FALSE;
	
	/* If we still need a buffer, get one from the modified queues */
	while ( b == NULL && search_mod )
	{
	    buffers_skipped = 0;

	    /* Capture the current free buffer count */
	    DM0P_COUNT_QUEUE(bm->bm_fpq, fcount);

	    for (i = 0; 
		 b == NULL && i < DB_MAX_TABLEPRI;
		 i++)
	    {
		/*
		** Search the modified queues in the next direction.
		** Buffers are added to the tail of the queues,
		** so the most-aged will clutter at the beginning.
		** Dirty-read the modified queue without the aid
		** of a mutex.
		*/
		    
		next = bm->bm_mpq[i].sth_stq.stq_next;
	    	restarts = MAX_RESTARTS;
		
		while ( next != BUF_ENDLIST )
		{
		    b = &buf_array[next];
		    next = b->buf_state.stq_next;

		    if (bm_common->bmcb_status & BMCB_IS_MT && restarts > 0)
		    {
			/* 
			** Skip buffers that are mutexed or busy.
			*/
			if ( b->buf_mutex.bms_locked ||
			    (b->buf_sts & 
				(BUF_MODIFIED | BUF_FIXED | BUF_BUSY | BUF_MUTEX | BUF_RECLAIM))
				    != BUF_MODIFIED || b->buf_priority != i)
			{
			    /*
			    ** The buffer may be changing while we look at it.
			    ** If it's no longer on the modified queue at this
			    ** priority, restart the search.
			    */
			    if ((b->buf_sts & (BUF_MODIFIED | BUF_FIXED)) != BUF_MODIFIED ||
				 b->buf_priority != i)
			    {
				restarts--;
				next = bm->bm_mpq[i].sth_stq.stq_next;
			    }
			    b = NULL;
			    continue;
			}
		    }
		    restarts = MAX_RESTARTS;
		    
		    /*
		    ** Lock the buffer, then recheck it.
		    ** After waiting for the buf_mutex,
		    ** the buffer may no longer be on the modified queue.
		    */
		    dm0p_mlock(SEM_EXCL, &b->buf_mutex);

		    next = b->buf_state.stq_next;

		    if ((b->buf_sts & 
			(BUF_MODIFIED | BUF_FIXED | BUF_BUSY | BUF_MUTEX | BUF_RECLAIM))
				== BUF_MODIFIED && 
			 b->buf_priority == i)
		    {
			/*
			** During the first pass thru the priority queues
			** skip over buffers which may induce a
			** log force (as evidenced by LSN).
			** If we fail to find an otherwise eligible buffer, 
			** rescan all priorities, this time accepting any 
			** eligible buffer.
			*/
			if (first_pass)
			{
			    page = (DMPP_PAGE *)((char *)lbm->lbm_pages[bm->bm_cache_ix] + 
					b->buf_page_offset);
			    
			    if (bm_common->bmcb_last_lsn.lsn_low &&
				LSN_GT(DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(b->buf_relpgtype, page),
						&bm_common->bmcb_last_lsn))
			    {
				dm0p_munlock(&b->buf_mutex);
				b = NULL;
				buffers_skipped++;
				continue;
			    }
			}
			/*
			** Look for a Table Descriptor with which to write
			** out this page.  If we don't find one, then look
			** for another buffer to reclaim.
			*/
			status = fix_tableio_ptr(b, (i4)DM0P_NOT_MATERIALIZED, lock_list, 
					log_id, &buf_tbio, err_code);

			if (status == E_DB_OK)
                        {
			    break;
                        }

			/*
			** Table Descriptor not found.  If an error was returned,
			** just log it and continue processing.
			*/
			if (DB_FAILURE_MACRO(status))
			{
			    ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG,
				NULL, (char *)0, (i4)0, (i4 *)0,
				err_code, 0);
			    ule_format(E_DM9C80_ERROR_FINDING_TABIO_PTR,
				(CL_SYS_ERR *)0, ULE_LOG, NULL, (char *)0, 
				(i4)0, (i4 *)0, err_code, 0);
			}
		    }
		    if ((b->buf_sts & (BUF_MODIFIED | BUF_FIXED)) != BUF_MODIFIED ||
			 b->buf_priority != i)
		    {
			next = bm->bm_mpq[i].sth_stq.stq_next;
		    }
		    
		    dm0p_munlock(&b->buf_mutex);
		    b = NULL;
		} /* while (next != BUF_ENDLIST) */

		if ( b == NULL )
		{
		    /*
		    ** Didn't find a modified buffer at this priority.
		    ** If new free buffers have appeared, abandon
		    ** the modified queues.
		    */
		    DM0P_COUNT_QUEUE(bm->bm_fpq, fpq_count);
		    if ( fpq_count > fcount )
			break;
		    else
		    {
			/* Save the modified count before leaving priority */
			mqcount[i] = bm->bm_mpq[i].sth_count;

			/*
			** If we haven't found a buffer and 
			** new modified buffers have appeared,
			** find the lowest priority at which 
			** new buffers have appeared, search from there.
			*/
			for ( j = 0; j < i; j++ )
			{
			    if ( bm->bm_mpq[j].sth_count > mqcount[j] )
				i = --j;
			}
		    }
		}
	    } /* for each priority... */

	    if (b == NULL)
	    {
		/*
		** Didn't find a modified buffer at any priority.
		** If new free buffers have appeared, start again
		** with the free queues.
		*/

		if ( fpq_count > fcount )
		{
		    search_free = TRUE;
		    search_mod = FALSE;
		}
		else
		{
		    /*
		    ** We failed to find a buffer at any priority.
		    ** If we skipped any log force-inducing
		    ** buffers, rescan the modified queues,
		    ** otherwise, give up.
		    */
		    if (first_pass && buffers_skipped)
		    {
			first_pass = FALSE;
			search_mod = TRUE;
		    }
		    else
                    {
                        /* Doublecheck the fpq count under mutex protection.
                        ** if it changed, someone is freeing buffers - restart search
                        */
                        dm0p_mlock(SEM_EXCL, &bm->bm_mutex);
                        DM0P_COUNT_QUEUE(bm->bm_fpq, fpq_count);

			search_mod = FALSE;
                        if (fpq_count != start_fpq_count)
                        {
                           first_pass = TRUE;
                           search_free = TRUE;
                        }
                        dm0p_munlock(&bm->bm_mutex);
                    }

		}
	    }                             
	} /* while ( b == NULL && search_mod ) */

    } while ( b == NULL && (search_free || search_mod) );
    /*
    ** If we couldn't get a buffer off the free or modify queues, then
    ** have to wait for another session to free one.
    */
    if (b == NULL)
    {
	DM0P_COUNT_QUEUE(bm->bm_fpq, fpq_count);
	DM0P_COUNT_QUEUE(bm->bm_mpq, mpq_count);

	if ( fpq_count == 0 && mpq_count == 0 )
	{
	    *err_code = E_DM920B_BM_NO_BUFFERS;
	    return (E_DB_ERROR);
	}
	
	lbm_stats->bs_fwait++;
	bm->bm_stats.bs_fwait++;
	/* Lock the status while we change it */
	dm0p_mlock(SEM_EXCL, &bm->bm_mutex);
	bm->bm_status |= BM_FWAIT;

	/* Set the free buffer wait event before release the mutex. */

	dm0p_eset(lock_list, DM0P_FWAIT_EVENT, bm->bm_cache_ix, 0, 0);

	/* Wait for the free buffer event. */
	dm0p_munlock(&bm->bm_mutex);

        /* If you are here, this thread is stuck waiting for free pages.
        ** Wake up the write behind threads since there is no guarantee
        ** another user thread is able to free a page
        */
        wb_cycle_start(bm, lock_list,
			BM_WB_SINGLE, "FAULT_PAGE");
	dm0p_ewait(lock_list, DM0P_FWAIT_EVENT, bm->bm_cache_ix, 0, 0, 0);

	return (E_DB_WARN);
    }

    /*
    ** Mark buffer as being reclaimed.
    ** This causes other threads to ignore this buffer while it's
    ** still on a state queue (free or modified), and signals
    ** force_page to remove the buffer from all queues but not
    ** reinsert it on the free queue.
    */
    b->buf_sts |= BUF_RECLAIM;

    if ( b->buf_sts != (BUF_FREE | BUF_RECLAIM) )
    {
	/*
	** Count a synchronous write if we're about to force a modified page.
	*/
	if (b->buf_sts & BUF_MODIFIED)
	{
	    lbm_stats->bs_syncwr[BMCB_PTYPES]++;
	    bm->bm_stats.bs_syncwr[BMCB_PTYPES]++;
	    lbm_stats->bs_syncwr[b->buf_type]++;
	    bm->bm_stats.bs_syncwr[b->buf_type]++;
	}
	/*
	** Reclaim the buffer for reuse.
	** Force page will return with the buf_mutex still held.
	** The buffer will have been removed from whatever state queue it was
	** on (free or modified) and will be in limbo queue-wise and
	** have its priority set to zero.
	** This force-page can return INFO, because we're reclaiming.
	*/
	status = force_page(b, buf_tbio, (i4)FP_TOSS, lock_list, 
	    log_id, BMCL_NO_GATHER, (i4*)0, err_code);

	/*
	** Release the table io control block used to force the victim
	** buffer (if there was one).  Note that a non-modified buffer required
	** no tableio control block when tossed from the cache.
	*/
	if (buf_tbio)
	{
	    unfix_tableio_ptr(&buf_tbio, lock_list, log_id, (i4)__LINE__);
	}

	/*
	** If the force operation failed, then we can't fault in a new page.
	*/
	if (status != E_DB_OK)
	{
	    /* If INFO return, buffer was invalidated and unmutexed, so we
	    ** can't touch it any more;  return WARN from here so caller
	    ** tries again.
	    */
	    if (status == E_DB_INFO)
		return (E_DB_WARN);

	    b->buf_sts &= ~BUF_RECLAIM;
	    dm0p_munlock(&b->buf_mutex);

	    if (*err_code > E_DM_INTERNAL)
	    {
		ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL, 
		    (char *)0, (i4)0, (i4 *)0, err_code, 0);
		*err_code = E_DM920C_BM_BAD_FAULT_PAGE;
	    }
	    return (status);
	}
    }
    else
    {
	/* Remove tossed free buffer from its free queue */
	p = b->buf_priority;

	dm0p_mlock(SEM_EXCL, &bm->bm_fpq[p].sth_mutex);

	next_buf = b->buf_state.stq_next;
	prev_buf = b->buf_state.stq_prev;
	if (next_buf != BUF_ENDLIST)
	    buf_array[next_buf].buf_state.stq_prev = prev_buf;
	else
	    bm->bm_fpq[p].sth_stq.stq_prev = prev_buf;
	if (prev_buf != BUF_ENDLIST)
	    buf_array[prev_buf].buf_state.stq_next = next_buf;
	else
	    bm->bm_fpq[p].sth_stq.stq_next = next_buf;

	/* Count one less free buffer, one more fixed */
	bm->bm_fpq[p].sth_count--;
	bm->bm_fpq[p].sth_lcount++;

	dm0p_munlock(&bm->bm_fpq[p].sth_mutex);
	
	b->buf_state.stq_next = b->buf_state.stq_prev = 0;

	/* Count a free buffer reclamation */
	lbm_stats->bs_reclaim[BMCB_PTYPES]++;
	bm->bm_stats.bs_reclaim[BMCB_PTYPES]++;
	lbm_stats->bs_reclaim[b->buf_type]++;
	bm->bm_stats.bs_reclaim[b->buf_type]++;
    }

    /*
    ** Check to see that the needed page hasn't been faulted in by
    ** another thread while we've been preparing this one.
    **
    ** At this point, the buffer is on no state queue.
    */
    b->buf_sts = (BUF_RBUSY | BUF_FIXED | BUF_VALID);
    b->buf_lock_key.lk_key1 = tbio->tbio_dcbid;
    b->buf_lock_key.lk_key2 = tbio->tbio_reltid.db_tab_base;
    b->buf_lock_key.lk_key3 = tbio->tbio_reltid.db_tab_index;
    b->buf_lock_key.lk_key4 = page_number;
    b->buf_lock_key.lk_key5 = 0;
    b->buf_lock_key.lk_key6 = 0;

    /*
    ** Have to check that the page still does not exist in the buffer
    ** pool.  Its possible that another session has picked a different
    ** buffer and faulted in this page while we were getting ready
    ** to fault it in.
    */
    dm0p_mlock(SEM_EXCL, &bucket_array[bucket].hash_mutex);
    hash_chain = bucket_array[bucket].hash_next;
    while (hash_chain != BUF_ENDLIST)
    {
	buf = &buf_array[hash_chain];
	if (buf->buf_lock_key.lk_key4 != b->buf_lock_key.lk_key4 ||
	    buf->buf_lock_key.lk_key1 != b->buf_lock_key.lk_key1 ||
	    buf->buf_lock_key.lk_key2 != b->buf_lock_key.lk_key2 ||
	    buf->buf_lock_key.lk_key3 != b->buf_lock_key.lk_key3)
	{
	    hash_chain = buf->buf_next;
	    continue;
	}
	/*
	** Unlock the hash chain, lock the buffer,
	** check the buffer again.
	*/
	dm0p_munlock(&bucket_array[bucket].hash_mutex);
	dm0p_mlock(SEM_EXCL, &buf->buf_mutex);
	if (buf->buf_hash_bucket == bucket &&
	    buf->buf_lock_key.lk_key4 == b->buf_lock_key.lk_key4 &&
	    buf->buf_lock_key.lk_key1 == b->buf_lock_key.lk_key1 &&
	    buf->buf_lock_key.lk_key2 == b->buf_lock_key.lk_key2 &&
	    buf->buf_lock_key.lk_key3 == b->buf_lock_key.lk_key3)
	{
	    dm0p_munlock(&buf->buf_mutex);
	    /*
	    ** Buffer found on hash.
	    ** Invalidate the buffer we just reclaimed (b),
	    ** returning it to free priority queue zero,
	    ** then return a warning.
	    */
	    invalidate_page(b, lock_list);
	    return (E_DB_WARN);
	}
	/*
	** Buffer changed while we waited for the mutex.
	** Restart the hash chain.
	*/
	dm0p_munlock(&buf->buf_mutex);
	dm0p_mlock(SEM_EXCL, &bucket_array[bucket].hash_mutex);
	hash_chain = bucket_array[bucket].hash_next;
    }

    /* hash_mutex is still locked */
    b->buf_hash_bucket = bucket;

    /*
    ** Link this buffer into the hash queue.
    ** After this is done then another thread will not
    ** try to fault in another copy of this page since it will find
    ** this buffer in the cache - although it won't use it yet since
    ** its marked busy.
    */

    /* Put on hash queue and release the hash mutex */
    next_buf = bucket_array[bucket].hash_next;
    b->buf_next = next_buf;
    b->buf_prev = BUF_ENDLIST;
    if (next_buf != BUF_ENDLIST)
	buf_array[next_buf].buf_prev = b->buf_id.bid_index;
    bucket_array[bucket].hash_next = b->buf_id.bid_index;

    /* Finally, we can release the hash_mutex */
    dm0p_munlock(&bucket_array[bucket].hash_mutex);


    /*
    ** Complete the formatting of the buffer descriptor.
    */
    STRUCT_ASSIGN_MACRO(tbio->tbio_reltid, b->buf_tabid);
    b->buf_dbid = tbio->tbio_dcbid;
    b->buf_lock_key.lk_type = LK_BM_PAGE;
    b->buf_refcount = 1;
    b->buf_tran_id.db_high_tran = 0;
    b->buf_tran_id.db_low_tran = 0;
    b->buf_lk_id.lk_unique = 0;
    b->buf_lock_mode = LK_N;
    b->buf_pid = (PID)0;
    b->buf_relpgtype = tbio->tbio_page_type;
    /* Store FCB pointer for possible CP */
    b->buf_cpfcb = (PTR)tbio->tbio_location_array[
	(page_number >> DI_LOG2_EXTENT_SIZE) % tbio->tbio_loc_count].loc_fcb;
    /*
    ** If Partition, save the partition number in the buffer
    ** for dm0p_close_page(), perhaps others.
    */
    if ( b->buf_tabid.db_tab_index & DB_TAB_PARTITION_MASK )
    {
	DMP_TCB	*t;
	t = (DMP_TCB *)((char *)tbio - CL_OFFSETOF(DMP_TCB, tcb_table_io));
	b->buf_partno = t->tcb_partno;
    }
    else
	b->buf_partno = 0;

    /*
    ** Store tcb's validation stamp in the buffer header.  This is used so
    ** that other servers connected to a common buffer manager can validate
    ** that they have the correct TCB before they attempt to write this buffer.
    */
    b->buf_tcb_stamp = tbio->tbio_cache_valid_stamp;

    /*
    ** If the server is executing a Consistency Point, then mark this buffer
    ** as flushed for the current CP.  This is OK since the buffer is not
    ** modified.
    **
    ** This is necessary due to Multi-Server protocols.  If multiple servers
    ** are cooperating to execute a CP, we may be faulting in a page that
    ** cannot be processed by the other servers (due to not having a TCB). If
    ** our server's FC thread has already passed this buffer, but others
    ** haven't, then a CP protocol failure will occur if this buffer gets
    ** modified before the last FC thread passes it.
    */
    if (bm_common->bmcb_status & BMCB_FCFLUSH)
	b->buf_cpcount = bm_common->bmcb_cpcount;

    if (action & DM0P_NOLOCK)
	b->buf_sts |= BUF_NOTXLOCK;

    if (action & DM0P_INTENT_LK)
	b->buf_sts |= BUF_INTENT_LK;

    /*
    ** Determine whether a cache lock is required.
    **
    ** Cache locks are required when the database is concurrently served by
    ** multiple servers using different data caches, this is indicated by
    ** TBIO_CACHE_MULTIPLE or TBIO_CACHE_DMCM.
    **
    ** Note that cache locks are not required for read-only databases (in
    ** which data can never change) nor for temporary tables (which are never
    ** multiply served).
    **
    ** If online Cluster REDO (RedoLSN != NULL), do not take cache lock
    */
    if (((tbio->tbio_cache_flags & TBIO_CACHE_MULTIPLE) ||
	 ((tbio->tbio_sysrel) && 
	  (tbio->tbio_reltid.db_tab_base == DM_B_RELATION_TAB_ID) && 
	  (tbio->tbio_reltid.db_tab_index == DM_I_RELATION_TAB_ID)) ||
         (tbio->tbio_cache_flags & TBIO_CACHE_DMCM)) &&
	(! (tbio->tbio_cache_flags & TBIO_CACHE_READONLY_DB)) &&
	(! tbio->tbio_temporary) && !RedoLSN )
    {
	b->buf_sts |= BUF_LOCKED;
    }

    /*
    ** If the buffer is being fixed for write and is being locked exclusively,
    ** then it will go onto the transaction hash queue for this transaction.
    */
    if (access_mode == DM0P_WRITE)
    {
	b->buf_sts |= BUF_WRITE;
	if ((action & DM0P_INTENT_LK) == 0)
	{
	    STRUCT_ASSIGN_MACRO(*tran_id, b->buf_tran_id);
	    if (b->buf_tran_id.db_high_tran || b->buf_tran_id.db_low_tran)
	    {
		/* insert on the tran queue. */
		txnq_insert(b, log_id);
	    }
	}
    }
    
    /* Insert buffer into table queue */
    tabq_insert(b);

    /*
    ** If faulting a page for CREATE_NOFMT, mutex it now while we
    ** hold buf_mutex.
    */
    if ( (action & DM0P_CREATE_NOFMT)
	&& ((dmf_svcb->svcb_status & SVCB_SINGLEUSER) == 0))
	b->buf_sts |= BUF_MUTEX;

    /*
    ** Now the buffer is on the correct queues and the page can be
    ** read in.   The buffer is marked busy so nobody will actually
    ** read the page, they will have to wait till we are done with it.
    */

    /*
    ** Now that all of the buf_sts bits have been decided and set,
    ** release the buf_mutex we hold so we can do file I/O and lock
    ** requests.
    **
    ** With the buffer placed on the hash queue, it's viewable by other
    ** threads. If we release the buf_mutex while we're still fiddling
    ** with buf_sts, another thread may see a buf_sts value in the
    ** process of being modified and attempt to modify buf_sts at
    ** the same instant.
    */
    dm0p_munlock(&b->buf_mutex);

    /*
    ** Get cache lock if database is shared with another buffer manager.
    ** If can't get cache lock, try bumping out other cached pages to
    ** free up locks.
    */
    status = E_DB_OK;
    lock_flags = LK_PHYSICAL | LK_NODEADLOCK | DM0P_LOCK_SCOPE(tbio);
    if ( action & DM0P_NOLKSTALL )
	lock_flags |= LK_NOSTALL;
    for (;b->buf_sts & BUF_LOCKED;)
    {
	old_lock_id = b->buf_lk_id.lk_unique;
        if ((tbio->tbio_cache_flags & TBIO_CACHE_DMCM) && (!tbio->tbio_sysrel))
        {
            /*
            ** (ICL phil.p)
            ** For DMCM, faulted page is locked 
            ** shared unless we want write access.
            */
            lk_mode = LK_S;
        
            if (access_mode & DM0P_WRITE)
                lk_mode = LK_X;

	    cl_status = LKrequestWithCallback(lock_flags,
                        lbm->lbm_lock_list, 
                        (LK_LOCK_KEY *)&b->buf_lock_key, lk_mode,
	                (LK_VALUE *) &b->buf_oldvalue,
                        (LK_LKID *)&b->buf_lk_id,
	                (i4) 0 , (LKcallback) dm0p_dmcm_callback, NULL, 
			&sys_err);
        }
        else
        {
	    lk_mode = LK_IX;

	    if (! (action & DM0P_READNOLOCK))
	        lk_mode = LK_IS;

	    if (DMZ_LCK_MACRO(2))
	        dmd_lock(&b->buf_lock_key, lbm->lbm_lock_list, LK_REQUEST,
					 lock_flags, lk_mode, 0, (DB_TRAN_ID *)0, (DB_TAB_NAME *)0,
					 (DB_DB_NAME *)0);

	    cl_status = LKrequest(lock_flags, lbm->lbm_lock_list,
			(LK_LOCK_KEY *)&b->buf_lock_key, lk_mode,
	        (LK_VALUE *)&b->buf_oldvalue, (LK_LKID *)&b->buf_lk_id,
	        (i4)0, &sys_err);
        }

	if (cl_status == OK || cl_status == LK_VAL_NOTVALID)
	{
	    if ( !old_lock_id )
	    {
		/* Process which allocated lock */
		b->buf_pid = lbm->lbm_pid;
	    }
	    b->buf_lock_mode = lk_mode;
	    break;
	}

	/*  Try to reclaim cache locks by forcing free pages. */
	if (dm0p_reclaim(lock_list, log_id) != E_DB_OK)
	{
	    ule_format(E_DM9209_BM_NO_CACHE_LOCKS, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
	    *err_code = E_DM004B_LOCK_QUOTA_EXCEEDED;
	    status = E_DB_ERROR;
	    break;
	}
    }

    /*
    ** If the caller indicated to fault a newly allocated page in, then
    ** don't read from disk. If DM0P_CREATE, format it. If DM0P_CREATE_NOFMT,
    ** read the page, but don't format it. The buffer was mutexed a few 
    ** lines ago and that's the state in which we'll return it to the caller.
    **
    ** Otherwise make sure the page exists in the file (use DIsense) and
    ** read it from the disk.
    */
    page = (DMPP_PAGE *)((char *)lbm->lbm_pages[bm->bm_cache_ix] + 
		b->buf_page_offset);

    if (action & DM0P_CREATE)
    {
	(*format_routine)(tbio->tbio_page_type, tbio->tbio_page_size, page,
		page_number, DMPP_DATA, DM1C_ZERO_FILL);
    }
    /* If DM0P_CREATE_NOFMT, only read the page if within formatted bounds */
    else if ( (action & DM0P_CREATE_NOFMT) == 0 || 
		page_number <= tbio->tbio_lpageno )
    {
        read_page = TRUE;
        /*
        ** If running DMCM, and a fast cluster long message facility
	** is available, check to see if another process has forwarded
	** us a valid copy of the desired page from their cache.
        */
        for ( ;(b->buf_sts & BUF_LOCKED) &&
               (b->buf_oldvalue.lk_high != 0) &&
               (OK == cl_status) &&
               (bm_common->bmcb_status & (BMCB_DMCM|BMCB_USE_CLM)) ==
                 (BMCB_DMCM|BMCB_USE_CLM); )
        {
# if 1
	    /*
	    ** FIX-ME: usage of lk_high is incompatible with fix put
	    ** in 13-oct-2004.  This is OK for now as currently 
	    ** BMCB_USE_CLM is never being set, but will need rework
	    ** if/when this feature is enabled.
	    */
# else
            i4  bytes_forwarded;
            i4  check_buf[4];

            /*
            ** High portion of lock value block holds chit to redeem.
            ** There is no absolute guarantee that forwarded page
            ** is still in place, since the lock value block is
            ** not cleared when a receipient has received and
            ** redeemed the 'chit' stored in the lock value block,
            ** so the 'chit' you see may be stale.
            */
            cl_status = CXmsg_redeem( b->buf_oldvalue.lk_high,
                                      (PTR)check_buf, sizeof(check_buf),
                                      0, &bytes_forwarded );
            if ( OK != cl_status )
            {
                if ( E_CL2C2C_CX_E_MSG_NOSUCH != cl_status )
                {
                    /* <INC> more diagnostic info? */
                    ule_format(cl_status, (CL_SYS_ERR *)0,
                        ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0,
                        err_code, 0);
                }
                break;
            }

            if (MEcmp((PTR)&b->buf_lock_key.lk_key1, check_buf,
                      sizeof(check_buf)))
            {
                /* Stored page ident does not match requested. */
                /* <INC> more diagnostic info? */
                break;
            }

            if (bytes_forwarded != sizeof(check_buf) + bm->bm_pgsize)
            {
                /* Stored page ident does not match requested. */
                /* <INC> more diagnostic info? */
                break;
            }

            /*
            ** All looks well, retreive the rest of the page.
            */
            cl_status = CXmsg_redeem( b->buf_oldvalue.lk_high,
                                      (PTR)page, bm->bm_pgsize,
                                      sizeof(check_buf), &bytes_forwarded );
            if ( OK != cl_status )
            {
                if ( E_CL2C2C_CX_E_MSG_NOSUCH != cl_status )
                {
                    /* <INC> more diagnostic info? */
                    ule_format(cl_status, (CL_SYS_ERR *)0,
                        ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0,
                        err_code, 0);
                }
                break;
            }

            /*
            ** Release forwarded page.  If an error here, could be
            ** a race condition with more than one process trying
            ** to redeem same chit.  If so this process is the loser.
            */
            cl_status = CXmsg_release( (i4 *)&b->buf_oldvalue.lk_high );
            if ( OK != cl_status )
            {
                if ( E_CL2C2C_CX_E_MSG_NOSUCH != cl_status )
                {
                    /* <INC> more diagnostic info? */
                    ule_format(cl_status, (CL_SYS_ERR *)0,
                        ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0,
                        err_code, 0);
                }
                break;
            }

            if (DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, page)
                != page_number)
            {
                /* Page ident does not match requested. */
                /* <INC> more diagnostic info! */
                break;
            }

            read_page = FALSE;

            /* <INC> Add statistics so we see how often we save a read */
# endif
            break;
        }

	/*
	** Check that the requested page in within the current bounds
	** of the physical file.
	**
	** We do not treat this as an internal error since there are
	** expected cases where a read-past-EOF could occur - for example
	** a user get-by-tid request where the TID is beyond EOF.
	*/

        if (read_page &&
             (status == E_DB_OK) && (page_number > tbio->tbio_lalloc))
	{
	    /*	Get the latest allocation information from the file system. */

	    status = dm2f_sense_file(tbio->tbio_location_array, 
		tbio->tbio_loc_count,
		tbio->tbio_relid, tbio->tbio_dbname,
		(i4 *)&tbio->tbio_lalloc, err_code);
	    if ( (status == E_DB_OK) && (page_number > tbio->tbio_lalloc) )
	    {
		*err_code = E_DM9202_BM_BAD_FILE_PAGE_ADDR;
		status = E_DB_ERROR;
	    }
	}

	if (read_page && status == E_DB_OK)
	{
	    /*
	    ** See if our value of tcb_lpageno is stale, this can occur
	    ** if another server has extended this table.  If we are
	    ** reading a page beyond what we think lpageno is, then we
	    ** must have a stale copy.  Set checkeof to indicate that the
	    ** next dm2t_open caller should read new eof information from
	    ** the free header.
	    */
	    if (page_number > tbio->tbio_lpageno)
		tbio->tbio_checkeof = TRUE;

	    read_page = TRUE;
	    if (DMZ_SES_MACRO(10))	    
	    {
		dmd_iotrace(DI_IO_READ, (DM_PAGENO)page_number, 1, 
		  tbio->tbio_relid,
		  &tbio->tbio_location_array[0].loc_fcb->fcb_filename,
		  tbio->tbio_dbname);
	    }

	    /*
	    ** Read in the requested page.
	    */
	    n = 1;
	    status = dm2f_read_file(tbio->tbio_location_array, 
		tbio->tbio_loc_count, tbio->tbio_page_size,
		tbio->tbio_relid, tbio->tbio_dbname, &n,
		(i4)page_number, (char *)page, err_code);

#ifdef xDEBUG
	    if (DMZ_BUF_MACRO(2))
	    TRdisplay("FAULT_PAGE  : (%~t,%~t,%d,%dK)\n",
		sizeof(DB_DB_NAME), tbio->tbio_dbname,
		sizeof(DB_TAB_NAME),  tbio->tbio_relid, page_number,
		KSIZE(tbio->tbio_page_size));
#endif

	    if (status == E_DB_OK)
	    {
		status = dm0p_validate_checksum(page, tbio->tbio_page_type,
					(DM_PAGESIZE) bm->bm_pgsize);
		if (status != E_DB_OK)
		{
		    ule_format(E_DM930C_DM0P_CHKSUM, (CL_SYS_ERR *)0,
			ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0,
			err_code, 3,
			sizeof(DB_DB_NAME), tbio->tbio_dbname,
			sizeof(DB_TAB_NAME), tbio->tbio_relid,
			0, page_number);

		    *err_code = E_DM920C_BM_BAD_FAULT_PAGE;
		}
	    }
	}
    }

    /*
    ** If the page read in does not have the correct page number then the
    ** page is corrupted or has not yet been formatted.
    **
    ** Note that this check is only valid if the page was successfully
    ** read in (status is OK).
    */
    if (status == E_DB_OK &&
	DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, page) != page_number)
    {
	/*
	** If this is a DM0P_CREATE_NOFMT, we anticipate a page number 
	** mismatch, otherwise we've got an error.
	*/
	if ((action & DM0P_CREATE_NOFMT) == 0)
	{
	    ule_format(E_DM93A7_BAD_FILE_PAGE_ADDR, (CL_SYS_ERR *)0, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 10,
		0, page_number,
		sizeof(DB_TAB_NAME), tbio->tbio_relid,
		sizeof(DB_OWN_NAME), tbio->tbio_relowner,
		sizeof(DB_DB_NAME), tbio->tbio_dbname,
		0, DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, page),
		0, DMPP_VPT_GET_PAGE_STAT_MACRO(b->buf_relpgtype, page),
		0, DMPP_VPT_GET_LOG_ADDR_HIGH_MACRO(b->buf_relpgtype, page),
		0, DMPP_VPT_GET_LOG_ADDR_LOW_MACRO(b->buf_relpgtype, page),
		0, DMPP_VPT_GET_TRAN_ID_HIGH_MACRO(b->buf_relpgtype, page),
		0, DMPP_VPT_GET_TRAN_ID_LOW_MACRO(b->buf_relpgtype, page));
	    *err_code = E_DM9206_BM_BAD_PAGE_NUMBER;
	    status = E_DB_ERROR;
	}
    }

    /*	Delete any cache lock set if error occurs. */

    if (status != E_DB_OK)
    {
	if (b->buf_lk_id.lk_unique)
	{
	    STATUS	local_cl_status;

	    local_cl_status = LKrelease((i4)0, lbm->lbm_lock_list,
		(LK_LKID *)&b->buf_lk_id, (LK_LOCK_KEY *)0,
		(LK_VALUE *)&b->buf_oldvalue, &sys_err);
	    if (local_cl_status)
	    {
		ule_format(local_cl_status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 0);
	    }
	    else
	    {
		b->buf_pid = (PID)0;
		b->buf_lock_mode = 0;
	    }
	}
    }
    else if ((b->buf_sts & BUF_LOCKED) && (action & DM0P_READNOLOCK))
    {
	/*
        ** Convert IX cache lock back to IS after read completes. 
        */
	error_code = E_DMF022_DM0P_CACHE_IX_IS;

        if (((tbio->tbio_cache_flags & TBIO_CACHE_DMCM) == 0) || tbio->tbio_sysrel)
        {
	    if (DMZ_LCK_MACRO(2))
	      dmd_lock(&b->buf_lock_key, lbm->lbm_lock_list, LK_CONVERT,
		(lock_flags | LK_CONVERT), LK_IS, 
		0, (DB_TRAN_ID *)0, (DB_TAB_NAME *)0, (DB_DB_NAME *)0);        

            cl_status = LKrequest(lock_flags | LK_CONVERT,
	      lbm->lbm_lock_list, (LK_LOCK_KEY *)0, LK_IS,
	      (LK_VALUE *)0, (LK_LKID *)&b->buf_lk_id, (i4)0, 
              &sys_err);

	    if (cl_status != E_DB_OK)
	    {
	        ule_format(cl_status, (CL_ERR_DESC *)&sys_err, ULE_LOG , NULL, 
			    (char * )0, 0L, (i4 *)0, 
			    err_code, 0);

	        ule_format(E_DM9C05_CACHE_CVT_ERROR, (CL_SYS_ERR *)&sys_err,
	    		    ULE_LOG, NULL,
			    (char *)0, (i4)0, (i4 *)0, err_code, 7,
			    0, b->buf_lock_key.lk_key4,
			    0, b->buf_tabid.db_tab_base,
			    0, b->buf_tabid.db_tab_index,
			    sizeof(DB_DB_NAME), tbio->tbio_dbname,
			    0, lbm->lbm_lock_list,
			    0, b->buf_lk_id,
                            0, lk_mode);

	        dmd_check(error_code);
	    }
	    else
	    {
		b->buf_lock_mode = LK_IS;
	    }
        }
    }

    /*
    ** Reaquire buffer mutex to finish initializing descriptor.
    */
    dm0p_mlock(SEM_EXCL, &b->buf_mutex);

    /*
    ** If I/O or Lock status from above was not successful or the page
    ** read in was corrupted, then invalidate this page.  invalidate_page()
    ** will wakeup all waiters. Return with buf_mutex released.
    */
    if (status != E_DB_OK)
    {
	dm0p_log_error( E_DM920C_BM_BAD_FAULT_PAGE, err_code, 
			((action & DM0P_USER_TID) == 0)); 

	invalidate_page(b, lock_list);
	return (E_DB_ERROR);
    }

    /*
    ** Check for special Buffer Manager Force Protocols for this page.
    **
    **     BUF_WCOMMIT - Buffer must be written at Commit/Abort time.
    **
    **         This is set for normal Ingres data pages when not running
    **         in FastCommit mode.  This flag causes this page to be forced
    **         to the database when dm0p_force_pages is called.
    **
    **     BUF_WUNFIX  - Buffer must be written when unfixed.
    **
    **         This is set for Ingres Pages which are updated with temporary
    **         physical locks (Sconcur catalogs, FHDR/FMAP pages, and Btree
    **         index pages) when the server is using Multi-Cache protocols
    **         OR the server is NOT using Fast Commit OR the client is the RCP.
    **
    **         These types of pages have the following properties which give 
    **         them special force protocols:
    **
    **           - Since the pages are not locked for transaction duration,
    **             other transactions may request to look at these pages before
    **             an update has been committed.  When the "other transactions"
    **             use a different cache, then they will see out-of-date
    **             versions if we don't force updates when the page locks
    **             are released.  (Note that rather than force the pages at
    **             unfix time we could build multi-cache protocols that allow
    **             one buffer manager to request a dirty page from another
    **             buffer manager ....)
    **
    **             When Multi-Cache protocols are in use or we are the RCP
    **             (in which case multi-cache protocols are asssumed since
    **             we may be executing a Pass-Abort) then BUF_WUNFIX must
    **             be defined on these page types.
    **
    **           - The buffer manager cannot currently keep track of the
    **             "owner" of a page that is updated by multiple sessions.
    **             When fast commit is not in use, the buffer manager must
    **             force all pages updated by a transaction at commit time.
    **             Since the buffer manager cannot remember when these types
    **             of pages are updated by a transaction, it must force them
    **             at Unfix time to avoid having to fix them at commit time.
    **
    **     BUF_TUNFIX - Buffer must be tossed at unfix time.
    **
    **         This is set for Ingres Pages which are updated with temporary
    **         physical locks (Sconcur catalogs, FHDR/FMAP pages, and Btree
    **         index pages) when the server has requested a Pass-Abort action
    **         and the server is not using multi-cache Cache Locks protocols.
    **
    **         When a Pass Abort is signaled, the RCP will possibly update
    **         page of this type that is in our cache.  If we are running
    **         in single-cache mode then we will not recognize when the pages
    **         in our cache are invalid.  We therefore toss any unlocked page
    **         while the RCP is executing a Pass Abort in the assumption that
    **         it may be invalidated.
    **
    **     BUF_PAWRITE - Buffer must be forced if a Pass Abort is signaled.
    **
    **         This is set for Ingres Pages which are updated with temporary
    **         physical locks (Sconcur catalogs, FHDR/FMAP pages, and Btree
    **         index pages).
    **
    **         When a Pass Abort is signaled, we must flush out any dirty
    **         pages which may be needed by the RCP during its abort processing
    **         since the RCP will be using a different cache.  Since ownership
    **         is not assigned for these types of pages, we simply force them
    **         all when a Pass Abort occurs. (Note that PAWRITE is not set
    **         when WUNFIX is used since no dirty pages of this type is ever
    **         left in the cache)
    **
    ** The absense of BUF_WCOMMIT or BUF_WUNFIX means that in normal
    ** circumstances (non pass-abort) the page needs not be written from
    ** the cache until the next Consistency Point.
    **
    ** None of these flags need be set on temporary tables.
    **
    ** -------------------------------------------------------------------
    ** (ICL phil.p)
    ** For DMCM, want pages to reside in cache until the next consistency
    ** point, or until we receive a contending lock request. So we don't
    ** set BUF_WCOMMIT or BUF_WUNFIX.
    ** Don't need to set BUF_PAWRITE because if the RCP requests any of
    ** the pages still in our cache, the contending lock request will
    ** activate the dm0p_dmcm_callback function forcing our pages to be
    ** written and/or dropped.
    ** System catalogs do participate in DMCM. At the end of a transaction,
    ** they are tossed from cache and their lock is converted down to NULL.
    ** --------------------------------------------------------------------
    */

    /* Off the modifed bit in the page and extract page_stat to a local variable */
    DMPP_VPT_CLR_PAGE_STAT_MACRO(b->buf_relpgtype, page, DMPP_MODIFY);

    /* If faulting in an unformatted page, assume the page type is DMPP_DATA */
    if (action & DM0P_CREATE_NOFMT &&
	DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, page) != page_number)
    {
	page_stat = DMPP_DATA;
    }
    else
    {
	page_stat = DMPP_VPT_GET_PAGE_STAT_MACRO(b->buf_relpgtype, page);
    }
    
    /*
    ** Temporary tables have no recovery or concurrency requirements and
    ** do not need special unfix & commit actions.
    */
    if (tbio->tbio_temporary)
    {
	b->buf_sts |= BUF_TEMPTABLE;
    }
    else if (tbio->tbio_cache_flags & TBIO_CACHE_DMCM)
    {
        if (tbio->tbio_sysrel) 
        {
            b->buf_sts |= BUF_NULL_TUNFIX;
            b->buf_sts &= ~BUF_DMCM;
        }
        else
            b->buf_sts |= BUF_DMCM;
    }
    else
    {
        if ( ! (tbio->tbio_cache_flags & TBIO_CACHE_FASTCOMMIT))
        {
	    b->buf_sts |= BUF_WCOMMIT;
        }

        if (tbio->tbio_sysrel || tbio->tbio_extended ||
	    page_stat & ( DMPP_FHDR | DMPP_FMAP | DMPP_INDEX ) )
        {
	    if (( ! (tbio->tbio_cache_flags & TBIO_CACHE_FASTCOMMIT)) ||
	        (tbio->tbio_cache_flags & TBIO_CACHE_MULTIPLE) ||
	        (tbio->tbio_cache_flags & TBIO_CACHE_RCP_RECOVERY) ||
	        tbio->tbio_reltid.db_tab_base == DM_B_SEQ_TAB_ID ||
	        ((tbio->tbio_reltid.db_tab_base == DM_B_RELATION_TAB_ID) && 
	  	 (tbio->tbio_reltid.db_tab_index == DM_I_RELATION_TAB_ID)))
	    {
	        b->buf_sts |= (BUF_WUNFIX | BUF_WUNFIX_REQ);
	    }
	    else
	    {
	        b->buf_sts |= BUF_PAWRITE;
	    }

	    if ( ! (tbio->tbio_cache_flags & TBIO_CACHE_MULTIPLE))
	    {
	        if ( dm0p_recover_state(log_id) )
		    b->buf_sts |= BUF_TUNFIX;
	    }
        }
    }

    /*
    ** Temporary tables have no recovery or concurrency requirements and
    ** do not need special unfix & commit actions.
    */
    if (tbio->tbio_temporary)
        b->buf_sts &= ~(BUF_WCOMMIT | BUF_WUNFIX | BUF_PAWRITE |
	    BUF_TUNFIX | BUF_WUNFIX_REQ | BUF_NULL_TUNFIX);


    /*
    ** Determine the type of page that was read.
    ** Set the page's buffer priority according to its type.  Check for 
    ** tbio_cache_priority - this overrides the buffer manager default.
    */
    b->buf_type = BMCB_DATA;

    /*
    ** For Isam tables, the page type is dependent upon its page number
    ** compared to relprim and relmain.  This is a bit hacky because we
    ** try not to depend on such system catalog information in the buffer
    ** manager since its not available to most recovery operations.  Here
    ** we look at the parent TCB to checkout the relprim and relmain values
    ** (but only if we 'know' we have a real TCB).
    */
    if (tbio->tbio_table_type == TCB_ISAM)
    {
	DMP_TCB		*tcb;

	tcb = (DMP_TCB *)((char *)tbio - CL_OFFSETOF(DMP_TCB, tcb_table_io));
	if ((tcb->tcb_type == TCB_CB) && ! (tcb->tcb_status & TCB_PARTIAL))
	{
	    if (page_number == tcb->tcb_rel.relprim)
		b->buf_type = BMCB_ROOT;
	    else if ((page_number >= tcb->tcb_rel.relmain) &&  
			(page_number < tcb->tcb_rel.relprim))
		b->buf_type = BMCB_INDEX;
	}
    }

    /* Check page type if not overridden by ISAM checks: */
    if ( b->buf_type == BMCB_DATA )
    {
	if ( page_stat & DMPP_FHDR )
	    b->buf_type = BMCB_FHDR;
	else if ( page_stat & DMPP_FMAP )
	    b->buf_type = BMCB_FMAP;
	else if (page_stat & DMPP_INDEX)
	{
	    if (page_number == 0)
		b->buf_type = BMCB_ROOT;
	    else
		b->buf_type = BMCB_INDEX;
	}
	else if (page_stat & (DMPP_LEAF | DMPP_CHAIN))
	    b->buf_type = BMCB_LEAF;
	else if (page_stat & DMPP_DIRECT)
	    b->buf_type = BMCB_INDEX;
    }

    b->buf_priority = bm_common->bmcb_reference[b->buf_type];

    if (tbio->tbio_cache_priority != DM0P_NOPRIORITY && !(DMZ_BUF_MACRO(35)))
    {
	/*
	** Use TCB priority instead of buffer manager default.
	**
	** TCB priority may be set when table/index is created
	** or modified.
	*/
	b->buf_priority = tbio->tbio_cache_priority;
	b->buf_sts |= BUF_PRIOFIX;

    }

    /* boost the priority if requested, likely for a leaf page 	*/
    /* that was requested on behalf of a readahead thread, so  	*/
    /* it stays around long enough to benefit user thread 	*/
    if ((action & DM0P_HI_PRIORITY) && b->buf_type == BMCB_LEAF)
    {
	b->buf_sts |= BUF_HI_TPRIO;
	if (++(b->buf_priority) > bm_common->bmcb_maximum[b->buf_type])
	    b->buf_priority = bm_common->bmcb_maximum[b->buf_type];
    }

    /* If temptable, reduce priority by one */
    if ( b->buf_sts & BUF_TEMPTABLE )
	b->buf_priority--;

    *buffer = b;

    if (read_page)
    {
	lbm_stats->bs_reads[BMCB_PTYPES]++;
	bm->bm_stats.bs_reads[BMCB_PTYPES]++;
	lbm_stats->bs_reads[b->buf_type]++;
	bm->bm_stats.bs_reads[b->buf_type]++;
    }

    if (DMZ_BUF_MACRO(15))
	(VOID) dm0p_check_consistency(1);

    b->buf_sts &= ~(BUF_RBUSY);
	    

    /* Return with buf_mutex held */
    return (E_DB_OK);
}

/*
** Name: gfault_page	- Fault a group of pages from disk into the buffer 
**			  pool.
**
** Description:
**      This routine finds a free group in the buffer pool and
**	reads a group of pages from disk. This is so called the readahead
**	operation. A free group is a group with no fixed member pages. If there 
**	are no free groups, will get a group from the modified group queue.
**	If there are no gree groups and no modified groups, then the buffer 
**	manager will just read one page instead. The next read for the same 
**	rcb will try readahead operation again.
**
**	Any pages that already exist in single page buffers, are read in by
**	a group read and the duplicate pages are discarded by not entering
**	them into the hash queues.
**
**	The caller must have the buffer manager mutex to call this routine.
**	The mutex will be released to do I/O but will be reaquired before
**	returning.
**
** Inputs:
**      rcb                             The RCB for the table.
**      action                          The action to fix page.
**	page_number			The page number in the table for the
**					first page to read.
**	access_mode			The access mode for the first page to 
**					read - read or write.
**	bucket				The hash bucket for the first page to
**					read.
**
** Outputs:
**	buffer				The allocated buffer for the first page.
**	err_code			Reason for error return status.
**	Returns:
**	    E_DB_OK			Success.
**	    E_DB_WARN			Restart operation using single buffer.
**	    E_DB_ERROR			Error occured.
**	    E_DB_FATAL			DMF fatal error.
**	Exceptions:
**	    none
**
** Side Effects:
**	    Buffers are marked BUSY while being read.
**
** History:
**	22-Jan-1987 (ac)
**	    Created for Jupiter.
**	18-oct-1988 (rogerk)
**	    Made fixes in error handling when can't get cache locks.
**	28-feb-1989 (rogerk)
**	    Added support for Shared Buffer Manager.
**	29-jun-1989 (rogerk)
**	    Release buffer manager mutex while making IO and lock calls.
**	 4-sep-1989 (rogerk)
**	    When looking for group buffer to use, check for free group first,
**	    then look for modified group to reclaim.
**	25-sep-1989 (rogerk)
**	    Fix PASS_ABORT problem.  Don't allow group fault on database that
**	    has no cache locks while RCP is doing recovery for this server.
**	 2-oct-1989 (rogerk)
**	    Fix NOLOCKS bug.  When can't acquire cache lock, set status to
**	    E_DB_ERROR correctly.
**	 2-oct-1989 (rogerk)
**	    Fix Consistency Point Bug.  If fault in buffer while in the
**	    middle of a consistency point, mark the buffer with the current
**	    CP number.  This indicates that this buffer does not need to be
**	    flushed as part of the current CP.
**	30-oct-1989 (rogerk)
**	    Fixed uninitialized variable bug in gfault_page which caused us
**	    to not use any but the 1st page of the group read.
**      11-sep-1990 (jonb)
**	    Integrate ingres6301 change 280136 (rogerk):
**            When we fault in a page, write the tcb's validation stamp into
**            the buffer header.  This allows other servers attached to a common
**            buffer manager to validate their tcb's without having to re-
**            request the tcb lock.  Also, pass buf_tcb_stamp value to 
**	      dm2t_get_tcb_ptr.
**	 4-feb-1991 (rogerk)
**	    Changed calculation of page_count for group reads to avoid group
**	    IO's that span multi-location file boundaries.  Requesting a multi
**	    page IO that spans location boundaries takes more than one IO.
**	 4-feb-1991 (rogerk)
**	    Added support for fixed buffer priorities.
**	    Added checks in fault_page for table's priority to override the
**	    buffer manager default priorities.
**	25-feb-1991 (rogerk)
**	    Put in temporary check for table cache priority that causes
**	    us to bypass group reads on the table.  This is here until we
**	    add priority queues for group buffers.  For now, we need to
**	    bypass group reads on table's with fixed priorites to keep
**	    the pages from being bumped out by the next scan operation.
**	    Also added trace flag DM426 to bypass group reads.
**	3-may-1991 (bryanp)
**	    Make dmd_checks more useful by logging detailed error information
**	    before calling dmd_check.
**	17-oct-1991 (rogerk)
**	    Merged changes made by Derek during the file extend project.
**	    New assumption is added : the special WUNFIX pages require
**	    cache locking during PASS ABORT on a sole server, since gfault_page
**	    is not used during PASS ABORT no new code needs to be added.
**	    Added new handling for EOF checking : use tcb_checkeof flag.
**	    Added checks for incorrect page numbers when a page is read in.
**	    Added trace flags in xDEBUG code.
**	    Added new session statistics collecting.
**	    Added new error messages.
**	19-Nov-1991 (rmuth)
**	    Added fixes for file extend and CONCUR tables when doing readahead.
**	    When faulting in a group of pages the buf_sts for any of the pages
**	    were not being set see code for comments.
**      21-jan-92 (jnash)
**          Add Btree root and index pages to those marked PAWRITE.
**	07-oct-1992 (jnash)
**	    6.5 reduced logging project.
**	     -  Validate the page checksums after reading the pages.
**	26-oct-1992 (rogerk)
**	  - Reduced logging project: changed references to tcb fields to
**	    use new tcb_table_io structure fields.  Changed from old
**	    dm2t_get_tcb_ptr to new fix_tableio_ptr.  Added check to bypass
**	    group operations with partial tcb's.
**	  - Changed lock_mode parameter to access_mode to divorce a little
**	    bit from page/table lock modes.
**	  - Took out old settings of cache locks on pages marked WUNFIX
**	    or readnolock during passabort.  These are handled now by
**	    tossing the pages at unfix time.
**	  - Removed requirement that system catalog pages use cache locks.
**	    They are now handled with the special Pass-Abort protocols
**	    shared by FMAP,FHDR and Btree Index pages.
**	  - Changed error handling a little bit to make sure trace back erorr
**	    messages are logged.  Moved check for error return from page
**	    read operations to before buffer fields begin to be initialized.
**	30-oct-1992 (rmuth)
**	    Add checks for reading past EOF of a temporary table.  The actual
**	    disk-allocated space for a temporary table may not match the
**	    value in tbio_lalloc (which indicates the virtual pages allocated
**	    to the table).  If a temporary table has 30 pages, but only the 1st
**	    ten have been forced to disk, then we can only group read up to
**	    page ten.  Added tbio_tmp_hw_pageno to track the highest disk-
**	    allocated page in a temporary table.
**	14-dec-1992 (rogerk)
**	    Changed temporary tables to not set PAWRITE fields since there are
**	    no concurrency or recovery issues.
**	05-mar-1993 (andys)
**	    Use DB_FAILURE_MACRO to check return status of dm0p_reclaim
**	    as it sometimes returns E_DB_WARN which is OK for us right here
**	    right now.
**	24-may-1993 (bryanp)
**	    If trace point DM302 is set, trace internal buffer manager cache
**		locks (useful mostly for debugging and testing purposes).
**	23-aug-1993 (andys)
**	    Don't use DB_FAIL_MACRO in [g]fault_page calls to dm0p_reclaim
**	    so that we know when we really can't get any more locks.
**	    In gfault_page, if dm0p_reclaim succeeds such that we retry,
**	    decrement the counter so that we retry the buffer we failed on.
**	18-oct-1993 (rmuth)
**	    Add DMP_BMCB statistics.
**	22-feb-1994 (andys)
**	    Add call to dm0p_check_consistency.
**	19-apr-1995 (cohmi01)
**	    If no free groups are found, in addition to writting out a
**	    modified group, signal the write-behind threads so that other
**	    ones will be written out, now that write-behinds handle groups.
**      24-oct-1995 (thaju02/stoli02)
**          Added increment of buffer manager group sync write counter.
**	23-jan-1996 (angusm)
**	    Prevent group read on multi-location internal temp tables:
**	    bug 72806.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	20-mar-1996 (nanpr01)
**	    Added page size as parameter to this routine. To compute
**	    the checksum pagesize is need since sizeof(*page) returns
**	    only the size of physical page.
**      06-may-1996 (nanpr01)
**          Change the reference to the page header with macros for
**	    New Page Format. 
**      01-aug-1996 (nanpr01 & ICL phil.p)
**          Introduced changes for Disributed Multi-Cache Management (DMCM)
**          protocol.
**          Instead of locking the whole group at LK_IX, we lock the first
**          page at the same mode as the data page (or LK_X for a read_nolock
**          query) and all the subsequent pages at LK_S. (For concurrency
**          reasons we cannot lock all the pages at LK_X). The requests for
**          all but the first page, are made with NOWAIT specified. If we
**          are not granted the lock, then mark the buffer as BUF_FREE.
**          Once read, locks on all but the first pages are converted to null.
**          NB. The interface has been changed, so that we can pass in the
**              action parameter and hence check whether a query is
**              DM0P_READNOLOCK.
**	    Move the io_wait calculation after the buffer manager mutex
**	    was acquired.
**	01-Aug-1996 (nanpr01)
**	    Also check for LK_VAL_INVALID status to initialize the 
**	    buf_lock_mode field.
**	13-Sep-1996 (jenjo02)
**	    We'll return (if successful) with the group's 1st buffer's
**	    buf_mutex locked (but not the group's g_mutex).
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Set BUF_X_LATCH, BUF_WUNFIX/BUF_PAWRITE when EXCL row locking
**	24-Feb-1997 (jenjo02 for somsa01)
**	    Cross-integrated 428939 from main.
**	    Added tbio_extended to checking fo Buffer Manager Force Protocols
**	    on core catalogs.
**	26-Feb-1997 (jenjo02)
**	  - Use SHARE mutex while searching the free and modified group queues.
**	  - Give preference to modified groups that won't induce a log force.
**	13-Mar-1997 (jenjo02)
**	  - After calling gforce_page(), discard groups which may now be on
**	    the fixed queue.
**	  - When a group is selected, mark it G_RBUSY, ensure that all needed
**	    buffers are free.
**	16-Apr-1997 (jenjo02)
**	  - Moved test for recover state to the front of this function.
**	  - Removed BUF_MUSTLOCK status which was being used without the
**	    protection of the buf_mutex and replaced it with the local
**	    variable "must_lock".
**	  - Reorganized some code to close some windows that left the
**	    group unprotected while it was being updated. Use move_group()
**	    to manage group queues intelligently instead of bungling them
**	    up in here.
**      21-may-1997 (stial01)
**          gfault_page() Do not put buf with IX page lock on xn queue
**          Removed code for non-fast-commit when row locking (not allowed) 
**          Removed code for BUF_PAWRITE when IX lock. (pass abort not allowed)
**	28-Aug-1997 (jenjo02)
**	    Redid group-busy logic, in general, lock group, set busy status, 
**	    unlock group, then lock buffer.
**	19-Jan-1998 (jenjo02)
**	    When pages are successfully faulted in, add them to the table's
**	    hash queue.
**	08-Sep-1998 (shero03)
**	    Change the hash algorithm to mod by the hash count.
**	27-Jan-1999 (jenjo02)
**	    In fault_page(), put the table in hash bucket before acquiring
**	    the cache lock and reading the page. This should be OK since the 
**	    buffer is busy. This will close the hole,when the cache lock is
**	    found yet the buffer is not found in the cache by traversing 
**	    through tables bucket list. 
**      08-Mar-1999 (wanya01)  
**           gfault_page(). Caller is responsible for awakening group waiters.
**	     This is 
**           addtional fix for bug 91942(change 436539) 
**      03-Feb-2000 (hanal04) Bug 83999 INGSRV100.
**          - Pass DM0P_TBIO_WARN_INVALID to fix_tableio_ptr() so that we do
**          not log disconcerting errors when we fail to find a TCB for
**          an index because the  base table's TCB has been invalidated. 
**	05-dec-2000 (thaju02 for jenjo02)
**	    Avoid forcing out a group containing modified pages if the 
**	    group is fixed. (cont. B94855)
**	10-Jan-2000 (jenjo02)
**	     Substantially rewritten to be more intelligent. Before
**	     tossing a group, see if enough of the pages we need are already
**	     in cache. Cull down the size of the group if some pages are
**	     already in cache (if the group is pages 1,2,3,4,5 and pages
**	     3,4,5 are in cache, just read 1 and 2, for example).
**	     Implemented group priority queues.
**	13-Apr-2000 (jenjo02)
**	    Corrected loop in gfault_page() caused by earlier failed
**	    gfault_page() and mishandling of g_valcount.
**	08-Aug-2000 (jenjo02)
**	    Remove test for WriteBehind in gfault_page(), which was
**	    causing group reads to be abandoned when they should not
**	    have been.
**      19-Jun-2002 (horda03) Bug 108074.
**          Whenever BUF_WUNFIX flag is set/cleared set/clear the BUF_WUNFIX_REQ
**          flag. Thus we keep track of buffers that should be flushed from
**          cache when UNfixed (if not protected by an exclusive table level
**          lock).
**      20-Aug-2002 (devjo01)
**          Add more logic to prevent futile gfault attempts, especially
**          when using internal threads. (b107914, b104299).
**	25-oct-2002 (devjo01)
**	    Prevent infinite CPU loop when checking for pages already in cache.
**	    b109051.
**	15-Nov-2002 (jenjo02)
**	    Added group state of "G_SKIPPED" to flag those groups
**	    discarded as candidates. Previously if two "skipped" groups
**	    were encountered, we'd loop between the two and never
**	    find a group. Part cause of B107914,B104299, above.
**	22-Nov-2002 (jenjo02)
**	    Readded "attempts" lost in previous change.
**	30-Jul-2003 (jenjo02) BUG 110646
**	    If error faulting in the group, reset BUF_RBUSY before
**	    tossing the pages, or fault_page() will put this
**	    thread to sleep.
**	18-Sep-2003 (jenjo02) SIR 110923
**	    Add "create" capability for groups, group consolidation.
**	10-Nov-2003 (jenjo02)
**	    Ensure that buf_cpfcb is populated.
**	01-Mar-2004 (jenjo02)
**	    Stuff partition number in buf_partno.
**      29-Sep-2004 (fanch01)
**          Conditionally add LK_LOCAL flag if database/table is
**	    confined to one node.
**	24-Jan-2005 (jenjo02)
**	    bs_greads[BMCB_PTYPES] reverted to counting one read for
**	    the entire group.
**    28-Feb-2005 (wanfr01,jenjo02)
**          Bug 113547, INGSRV3062
**          Stop doing dirty reads if we are restarting the scan too
**          many times to avoid cpu thrashing.
**	27-Apr-2005 (schka24)
**	    Include buf-hash-bucket in dirty test for is-page-in-cache,
**	    to prevent giving up on a page prematurely.  Reorder unmutexed
**	    buf setup to (try to) ensure that other threads don't see an
**	    interesting buf before they should.  Refine cpu-spin test.
**	11-Jul-2005 (schka24)
**	    Only consolidate for create variants, where it makes sense
**	    to try to grow onto the end of an existing group.  Consolidation
**	    is a waste for normal table scans.
**	14-Oct-2005 (jenjo02)
**	    Deleted Sequent-only (sqs_ptx) code to bypass group reads
**	    of temp tables. Sequent support deprecated.
**	26-Jan-2006 (kschendel)
**	    Turn on SKIPPED before tossing pages from proposed group, to
**	    eliminate race.  If we turn it on after, the force-failure
**	    condition might already be cleared and then there is nobody
**	    to ever turn SKIPPED off;  so the group becomes unavailable.
**	7-Feb-2006 (kschendel)
**	    Fix stupid typo in above, lost set-g-to-NULL, caused looping.
**      1-Mar-2006 (wanfr01)
**          Bug 114324
**          retry with single page read if you hit end of file.
**	12-Feb-2007 (jonj)
**	    Fix to Karl's G_SKIPPED fix, On E_DB_OK return from gforce_page,
**	    G_SKIPPED has been turned off and may need to be reenabled.
**	28-Feb-2007 (jonj)
**	    Translate DM0P_NOLKSTALL action to LK_NOSTALL for cache locks.
*/
static DB_STATUS
gfault_page(
DM0P_BMCB		*bm,
DMP_TABLE_IO		*tbio,
i4			page_number,
i4			bucket,
i4			lock_list,
i4			access_mode,
i4			log_id,
DB_TRAN_ID		*tran_id,
DM0P_BUFFER		**buffer,
i4                 action,
VOID			(*format_routine)(
			    i4		    page_type,
			    i4		    page_size,
			    DMPP_PAGE       *page,
			    DM_PAGENO       pageno,
			    i4         	    stat,
			    i4		    fill_option),
i4			*err_code)
{
    DMP_LBMCB               *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BSTAT	    	    *lbm_stats = &lbm->lbm_stats[bm->bm_cache_ix];
    DM0P_BM_COMMON          *bm_common = lbm->lbm_bm_common;
    DM0P_BUFFER		    *buf_array = lbm->lbm_buffers[bm->bm_cache_ix];
    DM0P_HASH_HEAD          *bucket_array = lbm->lbm_buckets[bm->bm_cache_ix];
    DM0P_GROUP              *group_array = lbm->lbm_groups[bm->bm_cache_ix];
    DMP_TABLE_IO	    *buf_tbio;
    DM0P_GROUP		    *g = NULL, *cg;
    DM0P_BUFFER		    *b, *buf, *fbuf;
    DMPP_PAGE		    *page;
    CL_SYS_ERR		    sys_err;
    DB_STATUS		    status;
    DB_STATUS		    local_status;
    STATUS		    cl_status;
    i4		    	    local_error;
    i4		    	    i;
    i4		    	    n;
    i4		    	    ng;
    i4		    	    pages = bm->bm_gpages;
    i4		    	    pages_to_read;
    i4		    	    hash_bucket;
    i4		    	    hash_chain;
    i4		    	    next_buf;
    i4		    	    last_page;
    i4		    	    first_extent, last_extent;
    i4                 	    lock_mode;
    i4                 	    lock_flags;
    bool		    must_lock = FALSE;
    i4			    valcount;
    i4			    gfcount,gmcount;
    i4			    attempts;
    i4			    page_stat;
    i4			    restarts;
    i4			    old_lock_id;

    /*
    ** Make sure that system is not in recover state.  We cannot do group
    ** reads on sole-served databases when the RCP is doing abort processing
    ** on behalf of our server, since we are not protected from RCP changes
    ** by cache locks.
    */
    if (( ! (tbio->tbio_cache_flags & TBIO_CACHE_MULTIPLE)) && 
	(dm0p_recover_state(log_id)))
    {
	return (E_DB_WARN);
    }

    /*
    ** Now that we have weighting and prioritizing of groups, allow group
    ** reads of tables which have fixed priorities.
    **
    ** The group's priority is set to the resultant priority of the
    ** first page in the group. Each time that page is refixed
    ** its group's priority is bumped by one.
    **
    ** Skip group reads if trace flag DM426 is set,
    ** a partial TCB is being used,
    ** or if TBIO_CACHE_NO_GROUPREAD is set for table
    **      (multi-loc internal table, can't guarantee size on each extent)
    **      (bug 72806)
    */
    if ((tbio->tbio_cache_priority != DM0P_NOPRIORITY && !(DMZ_BUF_MACRO(35)))
		|| DMZ_BUF_MACRO(26)
		|| (tbio->tbio_cache_flags & TBIO_CACHE_NO_GROUPREAD))
    {
#ifdef xDEBUG
        TRdisplay("GFAULT_PAGE DM426: (%~t,%~t,%d,%dK)\n",
	     sizeof(DB_DB_NAME), tbio->tbio_dbname,
	     sizeof(DB_TAB_NAME),  tbio->tbio_relid, page_number,
	     KSIZE(tbio->tbio_page_size));
#endif
        return(E_DB_WARN);
    }
    /*
    ** Bypass group operation if a Partial TCB is being used.
    **      (multi-loc internal table, can't guarantee size on each extent)
    **      (bug 72806)
    */
    if (tbio->tbio_status & TBIO_PARTIAL_CB)
    {
#ifdef xDEBUG
        TRdisplay("GFAULT_PAGE Partial TCB: (%~t,%~t,%d,%dK)\n",
	     sizeof(DB_DB_NAME), tbio->tbio_dbname,
	     sizeof(DB_TAB_NAME),  tbio->tbio_relid, page_number,
	     KSIZE(tbio->tbio_page_size));
#endif
        return(E_DB_WARN);
    }
    if ( DMZ_BUF_MACRO(26) || 
	 tbio->tbio_cache_flags & TBIO_CACHE_NO_GROUPREAD ||
         tbio->tbio_status & TBIO_PARTIAL_CB )
    {
	return (E_DB_WARN);
    }

    /*
    ** Decide how many pages to read in.  Adjust the page count if we are
    ** near the end-of-file, or if we are reading across multi-location page
    ** boundaries.
    **
    ** If we think that a page is requested that resides past EOF, then
    ** return a warning so that a single page read will be requested.  The
    ** single page read code can determine if the page number is valid.
    **
    ** We don't DIsense for end of file here, the number of valid pages in
    ** the file can only be determined from the allocation bitmaps.
    ** This can't be performed here because of synchronizaton problems and the
    ** requirement that recursive calls to dm0p_fix_page would be required.
    ** We return with tabio_checkeof set to indicate that the table open code
    ** should sense the new high water mark on the next table open.
    */

    /*
    ** If creating new pages, limit the number of pages to
    ** those actually allocated to the file.
    */
    if ( action & (DM0P_CREATE | DM0P_CREATE_NOFMT) )
    {
	if ( (page_number + pages - 1) > tbio->tbio_lalloc )
	    pages = tbio->tbio_lalloc - page_number + 1;
    }
    else if ((page_number + pages - 1) > tbio->tbio_lpageno)
	pages = tbio->tbio_lpageno - page_number + 1;

    /* Abandon group read if single page read */
    if (pages <= 1)
    {
	tbio->tbio_checkeof = TRUE;
	return(E_DB_WARN);
    }

    if ((tbio->tbio_temporary) && 
	((page_number + pages - 1) > tbio->tbio_tmp_hw_pageno) )
    {
	/*
	** For a temporary table the physical end-of-file on disc may
	** be less than the logical end of file, as the tail of the 
	** table may be in the buffer manager and not forced to disc
	** yet. This means that a group read could try and read past
	** the physical end-of-file this check stops this.
	*/
	pages = tbio->tbio_tmp_hw_pageno - page_number + 1;
	if (pages <= 1)
	{
	    return(E_DB_WARN);
	}
    }

    if (tbio->tbio_loc_count > 1)
    {
	/*
	** If this is a multi-location table, calculate number of pages to
	** read so that we end up on multi-loc boundaries.  Don't want to
	** do group reads spanning partway into second location.
	**
	** The following algorithm is designed to allow us to read across
	** multi-loc boundary if size of group buffer can completely encompass
	** more than one location group.  That is, if the multi-loc table breaks
	** up file in 16-page groups, we can define group buffer size to be 32
	** pages and do two 16-page IO's per group read.
	*/
	last_page = page_number + pages - 1;
	first_extent = page_number >> DI_LOG2_EXTENT_SIZE;
	last_extent = last_page >> DI_LOG2_EXTENT_SIZE;
	if ((first_extent != last_extent) &&
	    ((last_page % DI_EXTENT_SIZE) != (DI_EXTENT_SIZE - 1)))
	{
	    /*
	    ** If first and last pages are in different extents and the
	    ** last page is not at the end of its extent, then adjst the
	    ** page count so that we read to the end of the previous extent.
	    */
	    pages -= (last_page % DI_EXTENT_SIZE) + 1;
	}
    }

    /*
    ** Before acquiring and possibly forcing a group, find out how
    ** many of the pages we need appear to already be in cache.
    */
    pages_to_read = 0;
    for (i = 0; i < pages; i++)
    {
	/*
	** Compute the hash bucket for this page.
	*/
        hash_bucket = (u_i4)(tbio->tbio_dcbid + 
    		                 tbio->tbio_reltid.db_tab_base + 
	                         tbio->tbio_reltid.db_tab_index +
		                 page_number + i ) % bm->bm_hshcnt;

	/*
	** Check if this page is already in the cache.
	*/
	hash_chain = bucket_array[hash_bucket].hash_next;
	b = NULL;

	attempts = 0;
	while ( hash_chain != BUF_ENDLIST )
	{
	    b = &buf_array[hash_chain];
	    hash_chain = b->buf_next;

	    if (b->buf_hash_bucket == hash_bucket &&
		b->buf_lock_key.lk_key4 == page_number + i &&
		b->buf_lock_key.lk_key1 == tbio->tbio_dcbid &&
		b->buf_lock_key.lk_key2 == tbio->tbio_reltid.db_tab_base &&
		b->buf_lock_key.lk_key3 == tbio->tbio_reltid.db_tab_index)
	    {
		/*
		** Page found in cache. If 1st page of group, abandon
		** group read.
		*/
		if ( i == 0 )
		    return(E_DB_WARN);
		break;
	    }

	    if ( b->buf_hash_bucket != hash_bucket )
	    {
		hash_chain = bucket_array[hash_bucket].hash_next;
		/* If we're spinning on this dirty search, forget it, try again
		** below with mutexes.  Assume not-in-cache for now, will make
		** sure (with mutexes) later.
		*/
		if ( ++attempts > MAX_RESTARTS )
		{
#ifdef xDEBUG
		    TRdisplay( "%@ GFAULT_PAGE spinning for (%~t,%~t:%d)\n",
			   sizeof(DB_DB_NAME), tbio->tbio_dbname,
			   sizeof(DB_TAB_NAME),tbio->tbio_relid, page_number );
#endif
		    b = NULL;
		    break;
		}
	    }
	    b = NULL;

	}
	if ( b == NULL )
	    /* Not in cache already, track number of pages to read */ 
	    pages_to_read = i+1;
    }


    /*
    ** Abandon group read if it's not worthwhile, otherwise
    ** procede to find a group to use. Later, we'll recheck
    ** the group pages ("pages") with mutexes on the hash
    ** queues to see if they've appeared or disappeared
    ** from the cache in the interim.
    */
    if ( pages_to_read <= 1 )
	return(E_DB_WARN);


    /*
    ** Find a group.
    **
    ** First, look for an existing partial group that these
    ** pages can be consolidated with.
    **
    ** Failing that, look for a free group.
    **
    ** Failing that, we may decide to force a modified group. 
    ** If no group is available, return E_DB_WARN to do a 
    ** singe-page fault.
    */
    status = E_DB_OK;
    g = NULL;
    valcount = 0;

    /*
    ** Find a partial group whose last page number is just before
    ** the start of the page group we're trying to fault.
    **
    ** If one is found, adjust the number of pages to fit the
    ** space remaining in the group, then procede to consolidate
    ** these new pages with the existing group pages.
    **
    ** (schka24) Only consolidate when we're asked to create new
    ** pages;  the new pages might well be at the end of existing
    ** ones.  In other situations the consolidation just eats up
    ** CPU time without providing much if any benefit.
    */

    if ( (action & (DM0P_CREATE | DM0P_CREATE_NOFMT)) && page_number != 0 )
    {
	for ( i = 0; i < bm->bm_gcnt; i++ )
	{
	    cg = &group_array[i];

	    /* Skip empty groups and "full" groups */
	    if ( cg->g_pages == 0 || cg->g_pages == bm->bm_gpages )
		continue;
	    /* Look at the last used buffer in the group */
	    b = &buf_array[cg->g_buffer + cg->g_pages - 1];

	    if ( b->buf_lock_key.lk_key4 != page_number -1 ||
		 b->buf_lock_key.lk_key1 != tbio->tbio_dcbid ||
		 b->buf_lock_key.lk_key2 != tbio->tbio_reltid.db_tab_base ||
		 b->buf_lock_key.lk_key3 != tbio->tbio_reltid.db_tab_index )
	    {
		continue;
	    }
	    /* Lock the group, check again */
	    dm0p_mlock(SEM_EXCL, &cg->g_mutex);
	    /* Skip empty groups and "full" groups */
	    if ( cg->g_pages == 0 || cg->g_pages == bm->bm_gpages )
	    {
		dm0p_munlock(&cg->g_mutex);
		continue;
	    }
	    /* Look at the last used buffer in the group */
	    b = &buf_array[cg->g_buffer + cg->g_pages - 1];

	    if ( b->buf_lock_key.lk_key4 != page_number -1 ||
		 b->buf_lock_key.lk_key1 != tbio->tbio_dcbid ||
		 b->buf_lock_key.lk_key2 != tbio->tbio_reltid.db_tab_base ||
		 b->buf_lock_key.lk_key3 != tbio->tbio_reltid.db_tab_index )
	    {
		dm0p_munlock(&cg->g_mutex);
		continue;
	    }

	    /* If group is busy, wait for it, then retry */
	    if ( cg->g_sts & G_BUSY )
	    {
		lbm->lbm_stats[bm->bm_cache_ix].bs_gwait++;
		bm->bm_stats.bs_gwait++;
		cg->g_sts |= G_WAIT;
		dm0p_eset(lock_list, DM0P_GWAIT_EVENT, 
			    bm->bm_cache_ix, cg->g_index, 0);
		dm0p_munlock(&cg->g_mutex);

		/* Wait for the group to become unbusy */
		dm0p_ewait(lock_list, DM0P_GWAIT_EVENT, 
			    bm->bm_cache_ix, cg->g_index, 0, 0);
		/* Retry the group */
		i--;
		continue;
	    }

	    /*
	    ** Got one.
	    **
	    ** Set the range of group page's we'll add to the
	    ** group, and mark the group read-busy.
	    */
	    g = cg;

	    /* First group buffer of concern */
	    fbuf = ++b;

	    if ( g->g_pages + pages > bm->bm_gpages )
		pages = bm->bm_gpages - g->g_pages;

	    g->g_sts |= G_RBUSY;
	    g->g_lock_list = lock_list;

	    dm0p_munlock(&g->g_mutex);
	    break;
	}
    } /* End consolidation */

    if ( g == NULL )
    {
	/* Can't consolidate, find a new group */
	for (;;)
	{
	    buf_tbio = NULL;
	    b = NULL;

	    /*
	    ** First look for free group that is not being reclaimed.
	    ** Dirty-read the free group priority queues without the 
	    ** aid of a mutex.
	    */
	    for ( i = 0; 
		  g == NULL && i< DB_MAX_TABLEPRI; 
		  i++ )
	    {
		/* Oldest groups are at the front of the queue */
		ng = bm->bm_gfq[i].sth_stq.stq_next;
		restarts = MAX_RESTARTS;

		while (ng != BUF_ENDLIST)
		{
		    g = &group_array[ng];
		    ng = g->g_state.stq_next;

		    if (bm_common->bmcb_status & BMCB_IS_MT && restarts > 0)
		    {
			/*
			** Skip groups whose mutex is locked; they're
			** likely being picked off by a concurrent thread.
			*/
			if ( g->g_mutex.bms_locked || g->g_sts != G_FREE ||
			     g->g_priority != i )
			{
			    if ( g->g_sts & (G_FIXED | G_MODIFIED) ||
				 g->g_priority != i )
			    {
				restarts--;
				ng = bm->bm_gfq[i].sth_stq.stq_next;
			    }
			    g = NULL;
			    continue;
			}
		    }
		    restarts = MAX_RESTARTS;

		    /*
		    ** Lock the group, then recheck it.
		    ** After waiting for the group mutex, the group
		    ** may no longer be on the free queue.
		    */
		    dm0p_mlock(SEM_EXCL, &g->g_mutex);

		    if ( g->g_sts == G_FREE && g->g_priority == i )
			break;

		    ng = g->g_state.stq_next;
		    
		    if ( g->g_sts & (G_FIXED | G_MODIFIED) ||
			 g->g_priority != i )
		    {
			ng = bm->bm_gfq[i].sth_stq.stq_next;
		    }
		    dm0p_munlock(&g->g_mutex);
		    g = NULL;
		} /* while */
	    } /* for each priority */

	    /*
	    ** If could not find a free group, then try to reclaim a modified
	    ** group.
	    */
	    if (g == NULL)
	    {
		i4		fpq_count;
		/*
		** If no modified (unfixed) groups, return and use single buf.
		** Also return if there is an IOmaster server;
		** it will clean up a group buffer eventually, 
		** so it is silly to do a synchronous gforce 
		** for this thread if there is a single
		** buffer available for use anyway
		*/
		DM0P_COUNT_QUEUE(bm->bm_fpq, fpq_count);
		DM0P_COUNT_QUEUE(bm->bm_gmq, gmcount);

		if (gmcount == 0 ||
		    (bm_common->bmcb_status & BMCB_IOMASTER && fpq_count))
		    return (E_DB_WARN);

		for ( i = 0; 
		      g == NULL && i< DB_MAX_TABLEPRI; 
		      i++ )
		{
		    ng = bm->bm_gmq[i].sth_stq.stq_next;
		    restarts = MAX_RESTARTS;
		    while (ng != BUF_ENDLIST)
		    {
			g = &group_array[ng];
			ng = g->g_state.stq_next;

			if (bm_common->bmcb_status & BMCB_IS_MT && restarts > 0)
			{
			    /* Skip busy groups */
			    if ( g->g_mutex.bms_locked ||
				(g->g_sts & 
				    (G_MODIFIED | G_FIXED | G_BUSY | G_SKIPPED)) 
				    != G_MODIFIED || g->g_priority != i )
			    {
				if ( (g->g_sts & (G_MODIFIED | G_FIXED)) 
					!= G_MODIFIED ||
				      g->g_priority != i )
				{
				    ng = bm->bm_gmq[i].sth_stq.stq_next;
				    restarts--;
				}
				g = NULL;
				continue;
			    }
			}
			restarts = MAX_RESTARTS;
			
			/*
			** Lock the group, check it again.
			** After waiting for the g_mutex, the group
			** may no longer be on the modified queue.
			*/
			dm0p_mlock(SEM_EXCL, &g->g_mutex);
			ng = g->g_state.stq_next;
			
			if ( (g->g_sts & (G_MODIFIED | G_FIXED | G_BUSY | G_SKIPPED)) 
				    != G_MODIFIED || g->g_priority != i )
			{
			    if ( (g->g_sts & (G_MODIFIED | G_FIXED)) 
				    != G_MODIFIED ||
				  g->g_priority != i )
			    {
				ng = bm->bm_gmq[i].sth_stq.stq_next;
			    }
			    dm0p_munlock(&g->g_mutex);
			    g = NULL;
			    continue;
			}
			/* Count a synchronous group write */
			bm->bm_stats.bs_gsyncwr++;
			lbm_stats->bs_gsyncwr++;
			break;
		    } /* while */
		} /* for each priority */
	    } /* if g == NULL */

	    /* Could not find a usable group. Return to use a single page */
	    if (g == NULL)
		return (E_DB_WARN);

	    /* Toss the group of pages so that we can reuse the group.
	    ** But first, pessimistically mark it "skipped" so that we
	    ** only try it this once.
	    ** It's possible that a concurrent thread unfixing something
	    ** in the group might clear "skipped", and then we might
	    ** find the group still busy somehow.  That's OK since we'll
	    ** just retry.  Soon enough, either the group will be usable,
	    ** or we'll manage to leave "skipped" on long enough to move
	    ** to a different group.
	    ** (If we set "skipped" AFTER finding the group busy, the busy
	    ** condition might already be clear, and then there is nobody
	    ** to clear skipped -- then it stays on forever, which is bad.)
	    */
	    g->g_sts |= G_SKIPPED;

	    status = gforce_page(bm, g, lock_list, log_id, 
				    FP_TOSS, 
				    BMCL_NO_GATHER, (i4*)NULL,
				    err_code);

	    /*
	    ** Check success of group force operation.  If we could not free up
	    ** a group buffer then no group read can be done.
	    */
	    if (DB_FAILURE_MACRO(status))
	    {
		if (*err_code > E_DM_INTERNAL)
		{
		    ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL, 
			(char *)0, (i4)0, (i4 *)0, err_code, 0);
		    *err_code = E_DM920D_BM_BAD_GROUP_FAULTPAGE;
		}
		/* Something bad going on, clear skipped just in case */
		g->g_sts &= ~G_SKIPPED;
		dm0p_munlock(&g->g_mutex);
		return (status);
	    }

	    /*
	    ** Reuse this group only if it contains no valid
	    ** (hashable) pages, otherwise abandon it and
	    ** look for another group.  (leaving skipped set.)
	    **
	    ** NB: when status == E_DB_OK, G_SKIPPED has been
	    **     turned off and must be reenabled.
	    */
	    if (status == E_DB_OK && g->g_valcount == 0)
	    {
		/*
		** Mark the group as read-busy and release its mutex.
		*/
		g->g_sts = (g->g_sts & ~G_SKIPPED) | G_RBUSY;
		g->g_lock_list = lock_list;

		/* First buffer is 1st buffer in group */
		fbuf = &buf_array[g->g_buffer];

		dm0p_munlock(&g->g_mutex);

		/*
		** All of the buffers in the group are free and can't
		** be referenced or located thru the hash queues,
		** so we won't need to mutex them until they are
		** initialized and placed on the hash queues.
		**
		** This statement of course assumes that a group isn't
		** marked free until all buffer manipulation in the
		** group is fully completed.
		*/
		break;
	    }
	    /* When status == E_DB_OK, G_SKIPPED is turned off */
	    if ( status == E_DB_OK )
		g->g_sts |= G_SKIPPED;
	    dm0p_munlock(&g->g_mutex);
	    g = NULL;		/* Couldn't toss, start all over */

	} /* for to find group */
    } /* normal group selection */
    
    /* The group is marked read-busy, its mutex released */

    /*
    ** "fbuf" points to the starting group buffer, the group's
    ** first for a new unconsolidated group, the first unused
    ** buffer in a consolidated group.
    */

    /*
    ** Recheck, this time with mutexes, how many pages we'll
    ** need to read (some may have been faulted in to the 
    ** cache by other threads by now).
    */
    pages_to_read = 0;

    for (i = 0; i < pages; i++)
    {
	/*
	** Compute the hash bucket for this page.
	*/
        hash_bucket = (u_i4)(tbio->tbio_dcbid + 
    		                 tbio->tbio_reltid.db_tab_base + 
	                         tbio->tbio_reltid.db_tab_index +
		                 page_number + i ) % bm->bm_hshcnt;

	/* Lock the hash chain while we search it */
	dm0p_mlock(SEM_EXCL, &bucket_array[hash_bucket].hash_mutex);

	/*
	** Check if this page is already in the cache.
	*/
	b = NULL;
	hash_chain = bucket_array[hash_bucket].hash_next;

	while (hash_chain != BUF_ENDLIST)
	{
	    b = &buf_array[hash_chain];
	    if (b->buf_lock_key.lk_key4 != page_number + i ||
		b->buf_lock_key.lk_key1 != tbio->tbio_dcbid ||
		b->buf_lock_key.lk_key2 != tbio->tbio_reltid.db_tab_base ||
		b->buf_lock_key.lk_key3 != tbio->tbio_reltid.db_tab_index)
	    {
		hash_chain = b->buf_next;
		b = NULL;
		continue;
	    }
	    /*
	    ** Unlock the hash chain, lock the buffer,
	    ** check the buffer again.
	    */
	    dm0p_munlock(&bucket_array[hash_bucket].hash_mutex);
	    dm0p_mlock(SEM_EXCL, &b->buf_mutex);
	    if (b->buf_hash_bucket == hash_bucket &&
		b->buf_lock_key.lk_key4 == page_number + i &&
		b->buf_lock_key.lk_key1 == tbio->tbio_dcbid &&
		b->buf_lock_key.lk_key2 == tbio->tbio_reltid.db_tab_base &&
		b->buf_lock_key.lk_key3 == tbio->tbio_reltid.db_tab_index)
	    {
		/* Page exists in cache */
		break;
	    }
	    /*
	    ** Unlock the buffer, relock the hash chain,
	    ** restart search.
	    */
	    dm0p_munlock(&b->buf_mutex);
	    dm0p_mlock(SEM_EXCL, &bucket_array[hash_bucket].hash_mutex);
	    hash_chain = bucket_array[hash_bucket].hash_next;
	    b = NULL;
	}

	if ( b == NULL )
	{
	    /* Page does not exist in cache */

	    /* hash_mutex is still locked */
	    pages_to_read = i+1;

	    /* Initialize the group's buffer and insert it onto its hash queue */
	    b = fbuf + i;

	    /* Set the status first.  As soon as we link up the buffer into
	    ** a hash queue, someone might find it.  buf_sts and buf_hash_bucket
	    ** are volatile so that their manipulation doesn't get reordered.
	    */
	    b->buf_sts |= BUF_RBUSY | BUF_VALID;

	    STRUCT_ASSIGN_MACRO(tbio->tbio_reltid, b->buf_tabid);
	    b->buf_dbid = tbio->tbio_dcbid;
	    b->buf_tran_id.db_high_tran = 0;
	    b->buf_tran_id.db_low_tran = 0;
	    b->buf_lk_id.lk_unique = 0;
	    b->buf_lock_mode = LK_N;
	    b->buf_txnq_bucket = BUF_NOBUCKET;
	    b->buf_tabq_bucket = BUF_NOBUCKET;
	    b->buf_relpgtype = tbio->tbio_page_type;
	    b->buf_pid = (PID)0;

	    b->buf_refcount = 0;

	    /*
	    ** Store tcb's validation stamp in the buffer header.  This is used so
	    ** that other servers connected to a common buffer manager can validate
	    ** that they have the correct TCB before they attempt to write this buffer.
	    */
	    b->buf_tcb_stamp = tbio->tbio_cache_valid_stamp;

	    /*
	    ** If the server is executing a Consistency Point, then mark this buffer
	    ** as flushed for the current CP.  This is OK since the buffer is not
	    ** modified.
	    **
	    ** This is necessary due to Multi-Server protocols.  If multiple servers
	    ** are cooperating to execute a CP, we may be faulting in a page that
	    ** cannot be processed by the other servers (due to not having a TCB). If
	    ** our server's FC thread has already passed this buffer, but others
	    ** haven't, then a CP protocol failure will occur if this buffer gets
	    ** modified before the last FC thread passes it.
	    */
	    if (bm_common->bmcb_status & BMCB_FCFLUSH)
		b->buf_cpcount = bm_common->bmcb_cpcount;

	    b->buf_lock_key.lk_type = LK_BM_PAGE;
	    b->buf_lock_key.lk_key1 = tbio->tbio_dcbid;
	    b->buf_lock_key.lk_key2 = tbio->tbio_reltid.db_tab_base;
	    b->buf_lock_key.lk_key3 = tbio->tbio_reltid.db_tab_index;
	    b->buf_lock_key.lk_key4 = page_number + i;
	    b->buf_lock_key.lk_key5 = 0;
	    b->buf_lock_key.lk_key6 = 0;
	    /* Store FCB pointer for possible CP */
	    b->buf_cpfcb = (PTR)tbio->tbio_location_array[
	 	(page_number >> DI_LOG2_EXTENT_SIZE) % tbio->tbio_loc_count].loc_fcb;
	    /*
	    ** If Partition, save the partition number in the buffer
	    ** for dm0p_close_page(), perhaps others.
	    */
	    if ( b->buf_tabid.db_tab_index & DB_TAB_PARTITION_MASK )
	    {
		DMP_TCB	*t;
		t = (DMP_TCB *)((char *)tbio - CL_OFFSETOF(DMP_TCB, tcb_table_io));
		b->buf_partno = t->tcb_partno;
	    }
	    else
		b->buf_partno = 0;

	    /* Put buffer in hash bucket chain so other threads can find it */
	    next_buf = bucket_array[hash_bucket].hash_next;
	    b->buf_next = next_buf;
	    b->buf_prev = BUF_ENDLIST;
	    if (next_buf != BUF_ENDLIST)
		buf_array[next_buf].buf_prev = b->buf_id.bid_index;
	    bucket_array[hash_bucket].hash_next = b->buf_id.bid_index;

	    /* Only set buf_hash_bucket at the end, since the buffer is now
	    ** find-able via the hash queue and we're running unmutexed
	    */

	    b->buf_hash_bucket = hash_bucket;
	    /* Unlock the hash mutex */
	    dm0p_munlock(&bucket_array[hash_bucket].hash_mutex);

	    /* Count another valid page in the group */
	    valcount++;
	}
	else
	{
	    /* Page is already in cache */
	    dm0p_munlock(&b->buf_mutex);

	    /*
	    ** If it's the first page of the group,
	    ** abandon group read.
	    */
	    if ( i == 0 )
		break;
	}
    }

    /* If but one or fewer pages to fault in, abandon group read */
    if ( (pages = pages_to_read) <= 1 )
    {
	if ( pages )
	{
	    /* Remove 1st new buffer in group from hash chain */
	    b = fbuf;
	    dm0p_mlock(SEM_EXCL, &b->buf_mutex);
	    hashq_delete(b, bucket_array, buf_array);

	    /*
	    ** Wake up waiters.
	    */
	    if (b->buf_sts & BUF_IOWAIT)
	    {
		dm0p_erelease(lock_list, DM0P_BIO_EVENT,
			    b->buf_id.bid_cache, b->buf_id.bid_index, 0);
	    }

	    b->buf_sts = BUF_FREE;
	    dm0p_munlock(&b->buf_mutex);
	}

	dm0p_mlock(SEM_EXCL, &g->g_mutex);
	/* Wake up anyone waiting on this group */
	if (g->g_sts & G_WAIT)
	{
	    dm0p_erelease(lock_list, DM0P_GWAIT_EVENT, bm->bm_cache_ix,
			    g->g_index, 0);
	}
	g->g_sts &= ~(G_RBUSY | G_WAIT);
	g->g_lock_list = 0;

	/* The group is still free and empty of valid pages */

	dm0p_munlock(&g->g_mutex);
	return (E_DB_WARN);
    }

    /*
    ** At this point, all needed structures are marked "busy" and
    ** their mutexes are released.
    **
    ** Each buffer has been inserted into its hash queue, but that's all.
    **
    ** "pages" contains the culled-down number of contiguous pages 
    ** we need to add to the group.
    */



    /*
    ** Determine if a cache lock is required.
    **
    ** Cache locks are required when the database is concurrently served by
    ** multiple servers using different data caches.
    **
    ** If so, first buffer in group will be marked BUF_LOCKED,
    ** remaining buffers BUF_LOCKED | BUF_CACHED.
    **
    ** Note that cache locks are not required for read-only databases (in
    ** which data can never change) nor for temporary tables (which are 
    ** never multiply served).
    */
    if (((tbio->tbio_cache_flags & TBIO_CACHE_MULTIPLE) ||
	 ((tbio->tbio_sysrel) && 
	  (tbio->tbio_reltid.db_tab_base == DM_B_RELATION_TAB_ID) && 
	  (tbio->tbio_reltid.db_tab_index == DM_I_RELATION_TAB_ID)) ||
         (tbio->tbio_cache_flags & TBIO_CACHE_DMCM)) &&
        (! (tbio->tbio_cache_flags & TBIO_CACHE_READONLY_DB)) &&
        (! tbio->tbio_temporary))
    {
	must_lock = TRUE;

	for (i = 0, b = fbuf; i < pages; b++, i++)
	{
	    if (b->buf_sts == BUF_FREE)
		continue;

	    lock_flags = LK_PHYSICAL | LK_NODEADLOCK | DM0P_LOCK_SCOPE(tbio);
	    if ( action & DM0P_NOLKSTALL )
		lock_flags |= LK_NOSTALL;
	    old_lock_id = b->buf_lk_id.lk_unique;

	    if ((tbio->tbio_cache_flags & TBIO_CACHE_DMCM) && (!tbio->tbio_sysrel))
	    {
		lock_mode = LK_S;

		if ( b == fbuf )
		{
		    if (access_mode & DM0P_WRITE)
		    {
			lock_mode = LK_X; 
		    } 
		} else {
			lock_flags |= LK_NOWAIT;
		}

		cl_status = LKrequestWithCallback(lock_flags, lbm->lbm_lock_list, 
				(LK_LOCK_KEY *)&b->buf_lock_key, lock_mode,
				(LK_VALUE *)&b->buf_oldvalue, 
				(LK_LKID *)&b->buf_lk_id, (i4)0, 
				(LKcallback) dm0p_dmcm_callback, NULL, &sys_err);
	    }
	    else
	    {
		lock_mode = LK_IX;
		
		if (DMZ_LCK_MACRO(2))
		    dmd_lock(&b->buf_lock_key, lbm->lbm_lock_list, LK_REQUEST,
			(LK_PHYSICAL | LK_NODEADLOCK), lock_mode,
			0, (DB_TRAN_ID *)0, (DB_TAB_NAME *)0, (DB_DB_NAME *)0); 

		cl_status = LKrequest(lock_flags,
		    lbm->lbm_lock_list, (LK_LOCK_KEY *)&b->buf_lock_key, lock_mode,
		    (LK_VALUE *)&b->buf_oldvalue, (LK_LKID *)&b->buf_lk_id,
		    (i4)0, &sys_err);
	    }

	    if (cl_status == OK || cl_status == LK_VAL_NOTVALID)
	    {
		if ( !old_lock_id )
		{
		    /* Process which allocated lock */
		    b->buf_pid = lbm->lbm_pid;
		}

		b->buf_lock_mode = lock_mode;
		/*
		** buf_lk_id holds lock id to indicate that the buffer
		** has been locked. We'll set BUF_LOCKED later when we
		** are under the safety of buf_mutex.
		*/
		continue;
	    }
	    else if ( cl_status == LK_BUSY && 
		      b != fbuf &&
		      (tbio->tbio_cache_flags & TBIO_CACHE_DMCM) && 
		      (!tbio->tbio_sysrel))
	    {
		/*
		** Tidy up the failed buffer. Mark as BUF_FREE
		** and remove from the hash bucket.
		*/
		dm0p_mlock(SEM_EXCL, &b->buf_mutex);
		hashq_delete(b, bucket_array, buf_array);

		/*
		** Wake up waiters.
		*/
		if (b->buf_sts & BUF_IOWAIT)
		{
		    dm0p_erelease(lock_list, DM0P_BIO_EVENT,
				b->buf_id.bid_cache, b->buf_id.bid_index, 0);
		}

		b->buf_sts = BUF_FREE;
		dm0p_munlock(&b->buf_mutex);

		/*
		** For a DMCM group read, for all pages other than
		** the first, have the lock request made with the
		** LK_NOWAIT flag - to aid concurrency. If, in such
		** a case, LK_BUSY was returned, DON'T try to reclaim
		** pages as its not necessary.
		*/ 
	    }
	    else
	    {
		if (dm0p_reclaim(lock_list, log_id) != E_DB_OK)
		{
		    ule_format(E_DM9209_BM_NO_CACHE_LOCKS, &sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 0);
		    *err_code = E_DM004B_LOCK_QUOTA_EXCEEDED;
		    status = E_DB_ERROR;
		    break;
		} 
	    }

	    /*
	    ** (ICL phil.p)
	    ** For DMCM, non system catalog, accept the fail 
	    ** without retrying unless its the first page.
	    */
	    if ((tbio->tbio_cache_flags & TBIO_CACHE_DMCM) && 
	    	!(tbio->tbio_sysrel) && i != 0)
		continue;

	    /* Decrement the counter so we retry the same buffer */
	    i--;
	}
    }

    if (DMZ_SES_MACRO(10))	    
	dmd_iotrace(DI_IO_READ, 
	    (DM_PAGENO)page_number, pages, 
	    tbio->tbio_relid, 
	    &tbio->tbio_location_array[0].loc_fcb->fcb_filename,
	    tbio->tbio_dbname);

#ifdef xDEBUG
    if (DMZ_BUF_MACRO(2))
    TRdisplay("GFAULT_PAGE : (%~t,%~t,%d..%d,%dK)\n",
	sizeof(DB_DB_NAME), tbio->tbio_dbname,
	sizeof(DB_TAB_NAME),  tbio->tbio_relid, page_number,
	page_number + pages - 1, KSIZE(tbio->tbio_page_size));
#endif

    if ( status == E_DB_OK )
    {
	n = pages;
	page = (DMPP_PAGE *)((char *)lbm->lbm_pages[bm->bm_cache_ix] +
			 fbuf->buf_page_offset);

	/* If formatting, only format the first page in group */
	if (action & DM0P_CREATE)
	{
	    n = 1;
	    (*format_routine)(tbio->tbio_page_type, tbio->tbio_page_size, page,
		    page_number, DMPP_DATA, DM1C_ZERO_FILL);
	}
	else 
	{
	    /*
	    ** If CREATE_NOFMT, only attempt to read pages which
	    ** have already been formatted.
	    */
	    if ( action & DM0P_CREATE_NOFMT )
	    {
		if ( page_number+pages-1 > tbio->tbio_lpageno &&
		     (n = tbio->tbio_lpageno - page_number + 1) < 0 )
		{
		    n = 0;
		}
	    }

	    if ( n )
	    {
		status = dm2f_read_file(tbio->tbio_location_array, 
		    tbio->tbio_loc_count, tbio->tbio_page_size,
		    tbio->tbio_relid, tbio->tbio_dbname, &n,
		    (i4)page_number, (char *) page, err_code);

        if (status)
	    TRdisplay ("%@  Failure to read %d pages for relid %d.  lpageno=%d, page to read = %d\n",
		n, tbio->tbio_relid, tbio->tbio_lpageno, page_number);
		/* Count one more Group Read */
		lbm_stats->bs_greads[BMCB_PTYPES]++;
		bm->bm_stats.bs_greads[BMCB_PTYPES]++;
	    }
	}
	/* "n" is the number of formatted pages */
    }

    /*	Drop cache locks from IX to IS. */
    if (must_lock)
    {
	/*
	** (ICL phil.p)
	** For non-DMCM, drop cache locks from IX to IS.
	** For DMCM, drop all but the first page from S to NULL.
	**
	** (CA) devjo01
	** LK_LOCAL ignored for conversions, so don't bother setting.
	*/
	lock_flags = LK_PHYSICAL | LK_NODEADLOCK | LK_CONVERT;

	for (i = 0, b = fbuf; i < pages; b++, i++)
	{
	    if (b->buf_sts == BUF_FREE)
		continue;

	    if ((tbio->tbio_cache_flags & TBIO_CACHE_DMCM) && (!tbio->tbio_sysrel))
	    {
		/*
		** Retain original lock on first page.
		*/
		if ( b == fbuf )
		    continue;

		lock_mode = LK_N;
		
		cl_status = LKrequestWithCallback(
		    lock_flags, lbm->lbm_lock_list, (LK_LOCK_KEY *)0, lock_mode,
		    (LK_VALUE *) &b->buf_oldvalue, (LK_LKID *)&b->buf_lk_id,
		    (i4)0, (LKcallback) dm0p_dmcm_callback, NULL, &sys_err);
	    }
	    else
	    {
		lock_mode = LK_IS;

		if (DMZ_LCK_MACRO(2))
		    dmd_lock(&b->buf_lock_key, lbm->lbm_lock_list, LK_CONVERT,
			lock_flags, lock_mode, 
			0, (DB_TRAN_ID *)0, (DB_TAB_NAME *)0, (DB_DB_NAME *)0); 

		cl_status = LKrequest(lock_flags,
			  lbm->lbm_lock_list, 
			  (LK_LOCK_KEY *)0, lock_mode, (LK_VALUE *)0,
			  (LK_LKID *)&b->buf_lk_id, (i4)0, 
			  &sys_err);
	    }

	    if (cl_status == OK)
	    {
		b->buf_lock_mode = lock_mode;
	    }
	    else
	    {
		ule_format(cl_status, (CL_ERR_DESC *)&sys_err, ULE_LOG , NULL, 
			    (char * )0, 0L, (i4 *)0, 
			    err_code, 0);

		ule_format(E_DM9C05_CACHE_CVT_ERROR, (CL_SYS_ERR *)&sys_err,
			    ULE_LOG, NULL,
			    (char *)0, (i4)0, (i4 *)0, err_code, 7,
			    0, b->buf_lock_key.lk_key4,
			    0, b->buf_tabid.db_tab_base,
			    0, b->buf_tabid.db_tab_index,
			    sizeof(DB_DB_NAME), tbio->tbio_dbname,
			    0, lbm->lbm_lock_list,
			    0, b->buf_lk_id,
			    0, lock_mode);

		if ((tbio->tbio_cache_flags & TBIO_CACHE_DMCM) && (!tbio->tbio_sysrel))
		{
		    dmd_check(E_DMF030_CACHE_NULL);
		}
		else
		{
		    dmd_check(E_DMF022_DM0P_CACHE_IX_IS);
		}
	    }
	}
    }


    /*
    ** Validate the checksum of those pages not marked free.
    ** If any fail the checksum then the whole group fault fails.
    ** Also validate some of the page header information.
    **
    ** Errors here cause an E_DB_WARN return code so that the buffer
    ** manager can attempt a single-page read.  This will allow the
    ** operation to continue if the bad page was not the one which the
    ** query wanted to access.
    */
    if (status == E_DB_OK)
    {
	/* Only checksum, validate those pages read in ("n") */
	for (i = 0, b = fbuf; i < n; b++, i++)
	{
	    if (b->buf_sts != BUF_FREE)
	    {
		page = (DMPP_PAGE *)((char *)lbm->lbm_pages[bm->bm_cache_ix] + 
				b->buf_page_offset);

		status = dm0p_validate_checksum(page, tbio->tbio_page_type,
				(DM_PAGESIZE) bm->bm_pgsize);
		if (status != E_DB_OK)
		{
		    ule_format(E_DM930C_DM0P_CHKSUM, (CL_SYS_ERR *)0, 
			ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, 
			err_code, 3, 
			sizeof(DB_DB_NAME), tbio->tbio_dbname,
			sizeof(DB_TAB_NAME), tbio->tbio_relid,
			0, 
			DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, page));
		    *err_code = E_DM920D_BM_BAD_GROUP_FAULTPAGE;
		    status = E_DB_WARN;
		    break;
	    	}

		/*
		** Check that the page_number is valid, but not
		** if CREATE_NOFMT; those anticipate a page mismatch.
		*/

		if ( (action & DM0P_CREATE_NOFMT) == 0 &&
		    DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, page) != 
			page_number + i )
		{
		    ule_format(E_DM93A7_BAD_FILE_PAGE_ADDR, (CL_SYS_ERR *)0, 
			ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, 
			err_code, 10,
			0, page_number + i,
			sizeof(DB_TAB_NAME), tbio->tbio_relid,
			sizeof(DB_OWN_NAME), tbio->tbio_relowner,
			sizeof(DB_DB_NAME), tbio->tbio_dbname,
			0, DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, page),
			0, DMPP_VPT_GET_PAGE_STAT_MACRO(b->buf_relpgtype, page),
			0, 
		        DMPP_VPT_GET_LOG_ADDR_HIGH_MACRO(b->buf_relpgtype,page),
			0, 
			DMPP_VPT_GET_LOG_ADDR_LOW_MACRO(b->buf_relpgtype, page),
			0, 
			DMPP_VPT_GET_TRAN_ID_HIGH_MACRO(b->buf_relpgtype, page),
			0, 
			DMPP_VPT_GET_TRAN_ID_LOW_MACRO(b->buf_relpgtype, page));
		    *err_code = E_DM9206_BM_BAD_PAGE_NUMBER;
		    status = E_DB_WARN;
		    break;
		}
	    }
	}
    }

    /* 
    ** Increment the count of valid (on hash queue) pages in the group. If
    ** an error occurred, we'll call force_page(FP_TOSS) which will 
    ** remove the pages from the hash queue and decrement g_valcount,
    ** so it must be accurate here.
    */
    g->g_valcount += valcount;

    /*
    ** If an error occurred reading in the group, then toss each buffer.
    ** Wake up any waiters of the buffers.
    */

    if (status != E_DB_OK)
    {
	/*
	** Log traceback error message.
	*/
	if (*err_code > E_DM_INTERNAL)
	{
	    ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL, 
	        (char *)0, (i4)0, (i4 *)0, err_code, 0);
            if (*err_code != E_DM9335_DM2F_ENDFILE)
	    {
	        *err_code = E_DM920D_BM_BAD_GROUP_FAULTPAGE;
	    }
	}

	for (i = 0, b = fbuf; i < pages; b++, i++)
	{
	    if (b->buf_sts == BUF_FREE)
		continue;

	    dm0p_mlock(SEM_EXCL, &b->buf_mutex);

	    /*
	    ** If a cache lock was acquired, the set appropriate
	    ** status bits so that force_page() will do the right thing.
	    */
	    if (b->buf_lk_id.lk_unique)
	    {
		if ( b == fbuf )
		    b->buf_sts |= BUF_LOCKED;
		else
		    b->buf_sts |= (BUF_LOCKED | BUF_CACHED);
	    }

	    /* Unset read-busy */
	    b->buf_sts &= ~BUF_RBUSY;

	    /*
	    ** force_page() will lock g_mutex, so we can't be
	    ** holding it here.
	    */
	    local_status = force_page(b, tbio, (i4)FP_TOSS, 
		lock_list, log_id, BMCL_NO_GATHER, (i4*)0, &local_error);
	    if (local_status != E_DB_OK)
	    {
		dm0p_munlock(&b->buf_mutex);
		ule_format(local_error, (CL_SYS_ERR *)NULL, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, &local_error, 0);
	    }
	}

	/* Lock group's mutex */
	dm0p_mlock(SEM_EXCL, &g->g_mutex);

	/* Wake up anyone waiting on this group */
	if (g->g_sts & G_WAIT)
	{
	    dm0p_erelease(lock_list, DM0P_GWAIT_EVENT, bm->bm_cache_ix,
				g->g_index, 0);
	}
	g->g_sts &= ~(G_RBUSY | G_WAIT);
	g->g_lock_list = 0;

	/* Group status is still free, and void of valid buffers */

	dm0p_munlock(&g->g_mutex);

        /* Bug 114324:  If you failed to read this page in due to EOF,
        **      Reread it without a group read
        */
        if (*err_code == E_DM9335_DM2F_ENDFILE)
        {
            TRdisplay ("%@  Retrying with single page read for this page\n");
            tbio->tbio_checkeof = TRUE;
            return(E_DB_WARN);
        }
	return (status);
    }

    /*
    ** Finish initialization of each buffer descriptor.
    ** Require each buffer mutex to do this.
    **
    ** We'll release all but the group's 1st buffer's buf_mutex.
    */
    for (i = 0, b = fbuf; i < pages; b++, i++)
    {
	if (b->buf_sts == BUF_FREE)
	    continue;

	dm0p_mlock(SEM_EXCL, &b->buf_mutex);

	/* Fix the first buffer in the group */
	if ( b == fbuf )
	{
	    /* Buffer is fixed and not on any queue */
	    b->buf_state.stq_next = b->buf_state.stq_prev = 0;

	    b->buf_refcount = 1;
	    b->buf_sts |= (BUF_FIXED);
	    if (action & DM0P_INTENT_LK)
		b->buf_sts |= BUF_INTENT_LK;

	    /* If CREATE_NOFMT, set BUF_MUTEX state */
	    if ( (action & DM0P_CREATE_NOFMT)
		&& ((dmf_svcb->svcb_status & SVCB_SINGLEUSER) == 0))
		b->buf_sts |= BUF_MUTEX;

	    /*
	    ** If the first buffer is fixed for WRITE then put it on the 
	    ** appropriate transaction queue.
	    */
	    if (access_mode == DM0P_WRITE)
	    {
		b->buf_sts |= BUF_WRITE;
		if ((action & DM0P_INTENT_LK) == 0)
		{
		    STRUCT_ASSIGN_MACRO(*tran_id, b->buf_tran_id);
		    if (tran_id->db_high_tran | tran_id->db_low_tran)
		    {
			/*  Insert buffer on the tran queue. */
			txnq_insert(b, log_id);
		    }
		}
	    }
	}

	/*
	** If a cache lock was acquired, the set appropriate
	** buffer status bits.
	*/
	if (b->buf_lk_id.lk_unique)
	{
	    if ( b == fbuf )
		b->buf_sts |= BUF_LOCKED;
	    else
		b->buf_sts |= (BUF_LOCKED | BUF_CACHED);
	}

	/* All buffers must be inserted into the table hash queue */
	tabq_insert(b);

	page = (DMPP_PAGE *)((char *)lbm->lbm_pages[bm->bm_cache_ix] + 
			b->buf_page_offset);

	/* Reset modified bit on page and copy page_stat to local variable */
	DMPP_VPT_CLR_PAGE_STAT_MACRO(b->buf_relpgtype , page, DMPP_MODIFY);
	/* If faulting in an unformatted page, assume the page type is DMPP_DATA */
	if (action & DM0P_CREATE_NOFMT &&
	    DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, page) != page_number)
	{
	    page_stat = DMPP_DATA;
	}
	else
	{
	    page_stat = DMPP_VPT_GET_PAGE_STAT_MACRO(b->buf_relpgtype, page);
	}

	/*
	** Check for special Buffer Manager Force Protocols for this page.
	**
	**     BUF_WCOMMIT - Buffer must be written at Commit/Abort time.
	**
	**         This is set for normal Ingres data pages when not running
	**         in FastCommit mode.  
	**
	**     BUF_WUNFIX  - Buffer must be written when unfixed.
	**     BUF_PAWRITE - Buffer must be forced if a Pass Abort occurs.
	**
	** Full descriptions of these flags are given in fault_page.
	** Note that BUF_TUNFIX is not set in gfault_page as a check above
	** causes group faults to be bypassed when in Pass Abort mode.
        ** ------------------------------------------------------------------
        ** (ICL phil.p)
        ** For DMCM, want pages to reside in cache until the next consistency
        ** point, or until we receive a contending lock request. So we don't
        ** set BUF_WCOMMIT or BUF_WUNFIX.
        ** Don't need to set BUF_PAWRITE because if the RCP requests any of
        ** the pages still in our cache, the contending lock request will
        ** activate the dm0p_dmcm_callback function forcing our pages to be
        ** written and/or dropped.
        ** ------------------------------------------------------------------
	*/

	/*
	** Temporary tables have no recovery or concurrency requirements and
	** do not need special unfix & commit actions.
	*/
	if (tbio->tbio_temporary)
	{
	    b->buf_sts |= BUF_TEMPTABLE;
	}
	else if (tbio->tbio_cache_flags & TBIO_CACHE_DMCM)
        {
            if (tbio->tbio_sysrel)
            {
                b->buf_sts |= BUF_NULL_TUNFIX;
                b->buf_sts &= ~BUF_DMCM;
            }
            else
                b->buf_sts |= BUF_DMCM;
        }
        else
        {
            if ( ! (tbio->tbio_cache_flags & TBIO_CACHE_FASTCOMMIT))
            {
                b->buf_sts |= BUF_WCOMMIT;
            }
            if ((tbio->tbio_sysrel)  || (tbio->tbio_extended) ||
		page_stat & ( DMPP_FHDR | DMPP_FMAP | DMPP_INDEX ))
            {
                if (( ! (tbio->tbio_cache_flags & TBIO_CACHE_FASTCOMMIT)) ||
                    (tbio->tbio_cache_flags & TBIO_CACHE_MULTIPLE) ||
	            (tbio->tbio_cache_flags & TBIO_CACHE_RCP_RECOVERY) ||
		     tbio->tbio_reltid.db_tab_base == DM_B_SEQ_TAB_ID ||
	            ((tbio->tbio_reltid.db_tab_base == DM_B_RELATION_TAB_ID) && 
	  	     (tbio->tbio_reltid.db_tab_index == DM_I_RELATION_TAB_ID)))
                {
                    b->buf_sts |= (BUF_WUNFIX | BUF_WUNFIX_REQ);
                }
                else
                {
                    b->buf_sts |= BUF_PAWRITE;
                }
            }
        }

	/*
	** Temporary tables have no recovery or concurrency requirements and
	** do not need special unfix & commit actions.
	*/
	if (tbio->tbio_temporary)
	{
            b->buf_sts &= ~(BUF_WCOMMIT | BUF_WUNFIX | BUF_PAWRITE |
		BUF_WUNFIX_REQ | BUF_TUNFIX | BUF_NULL_TUNFIX);
	}

	/*
	** Determine the type of page that was read.
	*/
	b->buf_type = BMCB_DATA;

	/*
	** For Isam tables, the page type is dependent upon its page number
	** compared to relprim and relmain.  This is a bit hacky because we
	** try not to depend on system catalog information in the buffer
	** manager since its not available to most recovery operations.  
	** Here we look at the parent TCB to checkout the relprim and 
	** relmain values (but only if we 'know' we have a real TCB).
	*/
	if (tbio->tbio_table_type == TCB_ISAM)
	{
	    DMP_TCB		*tcb;

	    tcb = (DMP_TCB *)((char *)tbio - 
				CL_OFFSETOF(DMP_TCB, tcb_table_io));
	    if ((tcb->tcb_type == TCB_CB) && 
		    ! (tcb->tcb_status & TCB_PARTIAL))
	    {
		if (page_number + i  == tcb->tcb_rel.relprim)
		    b->buf_type = BMCB_ROOT;
		else if ((page_number + i >= tcb->tcb_rel.relmain) &&  
			    (page_number + i < tcb->tcb_rel.relprim))
		    b->buf_type = BMCB_INDEX;
	    }
	}

	/* Check page type if not overridden by ISAM checks: */
	if ( b->buf_type == BMCB_DATA )
	{
	    if ( page_stat & DMPP_FHDR )
		b->buf_type = BMCB_FHDR;
	    else if ( page_stat & DMPP_FMAP )
		b->buf_type = BMCB_FMAP;
	    else if (page_stat & DMPP_INDEX)
	    {
		if (page_number + i == 0)
		    b->buf_type = BMCB_ROOT;
		else
		    b->buf_type = BMCB_INDEX;
	    }
	    else if (page_stat & (DMPP_LEAF | DMPP_CHAIN))
		b->buf_type = BMCB_LEAF;
	    else if (page_stat & DMPP_DIRECT)
		b->buf_type = BMCB_INDEX;
	}

	/*
	** Determine the buffer priority based on the page type.
	** Check for TCB priority which should override cache default.
	*/
	b->buf_priority = bm_common->bmcb_reference[b->buf_type];

	if (tbio->tbio_cache_priority != DM0P_NOPRIORITY && !(DMZ_BUF_MACRO(35)))
	{
	    /*
	    ** Use TCB priority instead of buffer manager default.
	    **
	    ** TCB priority may be set when table/index is created
	    ** or modified.
	    **
	    ** Note that this code is never executed given that
	    ** we block group reads on the entry to this function
	    ** if tbio_cache_priority has been explicitly set.
	    */
	    b->buf_priority = tbio->tbio_cache_priority;
	    b->buf_sts |= BUF_PRIOFIX;
	}

	/* If temptable, reduce priority by one */
	if ( b->buf_sts & BUF_TEMPTABLE )
	    b->buf_priority--;

	/* Increment group page read stats by page type */
	lbm_stats->bs_greads[b->buf_type]++;
	bm->bm_stats.bs_greads[b->buf_type]++;
 
	b->buf_sts &= ~(BUF_RBUSY);

	/*
	** If other than 1st page of group, fudge priority down by one.
	** If this page gets fixed, dm0p_cachefix will bump
	** buf_priority, bringing it up to its "normal" value.
	*/
	if ( b != fbuf )
	{
	    if ( (b->buf_sts & BUF_PRIOFIX) == 0 )
		b->buf_priority--;
	    if (b->buf_sts & BUF_IOWAIT)
		awaken_buf_waiters(b, lock_list);
	    /* Release the mutex on all but the 1st buffer */
	    dm0p_munlock(&b->buf_mutex);
	}
    }

    /* Return first (fixed) buffer to caller */
    *buffer = fbuf;

    if (DMZ_BUF_MACRO(15))
	    (VOID) dm0p_check_consistency(2);

    /*
    ** Relock the group,
    ** set the number of pages in use,
    ** increment the group reference count
    **   to account for the "fix" of the
    **   group's first buffer; other threads
    **   may have found and fixed other pages
    **   in this group in the interim and
    **   modified g_refcount, so we can't
    **   just arbitrarily set it to one.
    ** set new group priority to that of the
    ** first buffer in the group,
    ** move from free queue to fixed.
    */
    dm0p_mlock(SEM_EXCL, &g->g_mutex);

    /* Increment number of pages in group */
    g->g_pages += pages;

    /* Set group priority if new, not consolidated, group */
    if ( fbuf == &buf_array[g->g_buffer] )
	g->g_new_pri = fbuf->buf_priority;

    if ( g->g_refcount == 0 )
    {
	/* Move group from free queue to fixed, if currently free */
	move_group(bm, g, (DM0P_BUFFER*)NULL, lock_list, 1, 0, 0);
    }
    else
    {
	++g->g_refcount;	/* Group already fixed, count one more */
    }

    /* Wake up anyone waiting on this group */
    if (g->g_sts & G_WAIT)
    {
	dm0p_erelease(lock_list, DM0P_GWAIT_EVENT, bm->bm_cache_ix,
			g->g_index, 0);
    }
    g->g_sts &= ~(G_RBUSY | G_WAIT);
    g->g_lock_list = 0;

    dm0p_munlock(&g->g_mutex);

    /* Return with group's 1st buffer's buf_mutex locked */
    return (E_DB_OK);
}

/*
** Name: dm0p_pagetype - Give page type of newly created page.
**
** Description:
**	This routine must be called following the fixing of a page
**	with the DM0P_SCRATCH option.  
**
**	It must be called prior to unfixing the page.
**
**	(Since the buffer manager currently formats all scratch pages as
**	data pages, this call can optionally be bypassed for DATA pages.)
**
**	The routine allows the buffer manager to set any special buffer
**	fields that are particular to the page's type.  These actions
**	are normally performed by fault_page but cannot be done in
**	this case because the page has been newly formatted.
**
** Inputs:
**	tbio			    Table IO Control Block used to fix page.
**	page			    Pointer to the newly allocated page.
**	log_id			    Logging System Handle.
**	page_type		    Type of page - ONE (only one) of:
**					DMPP_FHDR
**					DMPP_FMAP
**					DMPP_DATA
**					DMPP_LEAF
**					DMPP_INDEX
**					DMPP_CHAIN
**
** Outputs:
**	none
** Returns:
**	VOID
**
** History:
**	14-dec-1992 (rogerk)
**	    Created as part of the reduced logging project.
**      27-july-1995 (carly01)
**	    b69767 - set buffer priority and type for "in memory" tables.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**      06-may-1996 (nanpr01)
**	    All page header access was replaced by macros for New Page Format
**	    project.
**      01-aug-1996 (nanpr01 for ICL phil.p)
**          Introduced support for the Distributed Multi-Cache Management
**          (DMCM) protocol. In effect, when running DMCM, don't set any of
**           the other Buffer Manager protocols.
**	24-Feb-1997 (jenjo02 for somsa01)
**	    Cross-integrated 428939 from main.
**	    Added tbio_extended to checking fo Buffer Manager Force Protocols
**	    on core catalogs.
**	27-Mar-1997 (jenjo02)
**	    Added lock/unlock of buf_mutex, which was inexplicably omitted.
**	01-Feb-1999 (jenjo02)
**	    Determine buf_type unconditionally, not just for temp tables.
**	    Added consistency check to ensure that the right page is mapped
**	    and that it's fixed.
**      19-Jun-2002 (horda03) Bug 108074.
**          Whenever BUF_WUNFIX flag is set/cleared set/clear the BUF_WUNFIX_REQ
**          flag. Thus we keep track of buffers that should be flushed from
**          cache when UNfixed (if not protected by an exclusive table level
**          lock).
*/
VOID 
dm0p_pagetype(
DMP_TABLE_IO	*tbio,
DMPP_PAGE 	*page,
i4		log_id,
i4		page_type)
{
    DMP_LBMCB           *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON      *bm_common = lbm->lbm_bm_common;
    DM0P_BMCB           *bm = lbm->lbm_bmcb[tbio->tbio_cache_ix];
    DM0P_BSTAT	        *lbm_stats = &lbm->lbm_stats[tbio->tbio_cache_ix];
    DM0P_BUFFER		*b;
    i4		page_index;
    i4		error;
    DMP_TCB		*tcb;
    DM_PAGENO		page_number;
    i4			old_buf_type;

    /*
    ** Lookup buffer associated with this page.
    */
    page_index = ((char *)page - (char *)lbm->lbm_pages[tbio->tbio_cache_ix])
						/tbio->tbio_page_size;

    /* lint truncation warning if size of ptr > int, but code valid */
    if (((int)page  & (sizeof(ALIGN_RESTRICT) - 1)) || (page_index < 0) ||
	(page_index >= bm->bm_bufcnt))
    {
	ule_format(E_DM934C_DM0P_BAD_ADDRESS, (CL_SYS_ERR *)0, ULE_LOG,NULL,
	    (char *)0, (i4)0, (i4 *)0, &error, 5,
	    0, page_index, 0, bm->bm_bufcnt, 0, page,
	    sizeof(*tbio->tbio_dbname), tbio->tbio_dbname,
	    sizeof(*tbio->tbio_relid),  tbio->tbio_relid);
	dmd_check(E_DM9306_DM0P_UNFIX_PAGE); /* XXXX new error */
    }

    b = &lbm->lbm_buffers[tbio->tbio_cache_ix][page_index];

    /* Extract page_number to a local variable */
    page_number = DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, page);

    dm0p_mlock(SEM_EXCL, &b->buf_mutex);

    /*
    **	Check the information stored in the buffer is consistent with
    **  the page being formatted.
    */
    if ( b->buf_lock_key.lk_key4 != page_number ||
	 b->buf_tabid.db_tab_base != tbio->tbio_reltid.db_tab_base ||
	(b->buf_sts & BUF_FIXED) == 0 )
    {
	ule_format(E_DM934D_DM0P_BAD_PAGE, (CL_SYS_ERR *)0, ULE_LOG,NULL,
	    (char *)0, (i4)0, (i4 *)0, &error, 9,
	    0, page_number,
	    0, b->buf_lock_key.lk_key4,
	    0, tbio->tbio_reltid.db_tab_base,
	    0, b->buf_tabid.db_tab_base,
	    0, tbio->tbio_reltid.db_tab_index,
	    0, b->buf_tabid.db_tab_index,
	    0, b->buf_sts,
	    sizeof(*tbio->tbio_dbname), tbio->tbio_dbname,
	    sizeof(*tbio->tbio_relid),  tbio->tbio_relid);
	dmd_check(E_DM9306_DM0P_UNFIX_PAGE); /* XXXX new error */
    }

    /*
    ** Set special actions based on page type.  These are described in
    ** more detail in the fault_page routine.
    */
    b->buf_sts &= ~(BUF_WUNFIX | BUF_PAWRITE | BUF_TUNFIX |
	BUF_NULL_TUNFIX | BUF_WUNFIX_REQ);

    /*
    ** Determine the type of page that was read for "in memory" tables.
    ** Set the page's buffer priority according to its type.  Check for 
    ** tbio_cache_priority - this overrides the buffer manager default.
    **
    ** Do this for all tables. For newly formatted (DM0P_CREATE|NOFMT)
    ** pages, this is the first opportunity we've had to know the
    ** type of page.
    */
    old_buf_type = b->buf_type;
    b->buf_type = BMCB_DATA;
     
    /*
    ** For Isam tables, the page type is dependent upon its page number
    ** compared to relprim and relmain.  This is a bit hacky because we
    ** try not to depend on such system catalog information in the 
    ** buffer manager since its not available to most recovery 
    ** operations.  Here we look at the parent TCB to checkout the 
    ** relprim and relmain values (but only if we 'know' we have a 
    ** real TCB).
    */
	     
    if (tbio->tbio_table_type == TCB_ISAM)
    {
	tcb = (DMP_TCB *)((char *)tbio - CL_OFFSETOF(DMP_TCB, tcb_table_io));
	if ((tcb->tcb_type == TCB_CB) && ! (tcb->tcb_status & TCB_PARTIAL))
	{
	    if (page_number == tcb->tcb_rel.relprim)
		b->buf_type = BMCB_ROOT;
	    else if ((page_number >= tcb->tcb_rel.relmain) &&
		(page_number < tcb->tcb_rel.relprim))
		b->buf_type = BMCB_INDEX;
	}
    }

    /* Check page type if not overridden by ISAM checks: */
    if ( b->buf_type == BMCB_DATA )
    {
	if ( page_type & DMPP_FHDR )
	    b->buf_type = BMCB_FHDR;
	else if ( page_type & DMPP_FMAP )
	    b->buf_type = BMCB_FMAP;
	else if (page_type & DMPP_INDEX)
	{
	    if (page_number == 0)
		b->buf_type = BMCB_ROOT;
	    else
		b->buf_type = BMCB_INDEX;
	}
	else if (page_type & (DMPP_LEAF | DMPP_CHAIN))
	    b->buf_type = BMCB_LEAF;
	else if (page_type & DMPP_DIRECT)
	    b->buf_type = BMCB_INDEX;
    }

    /*
    ** Repair the page type fix stats if it's changed 
    ** from when initially fixed.
    */
    if ( b->buf_type != old_buf_type )
    {
	lbm_stats->bs_fix[b->buf_type]++;
	bm->bm_stats.bs_fix[b->buf_type]++;

	lbm_stats->bs_fix[old_buf_type]--;
	bm->bm_stats.bs_fix[old_buf_type]--;
    }
	

    /*
    ** Determine buffer priority based on the type of data on the page.
    ** The page is fixed and on no state queue.
    */
    b->buf_priority = bm_common->bmcb_reference[b->buf_type];

    if (tbio->tbio_cache_priority != DM0P_NOPRIORITY && !(DMZ_BUF_MACRO(35)))
    {
	/*
	** Use TCB priority instead of buffer manager default.
	**
	** TCB priority may be set when table/index is created
	** or modified.
	*/
	b->buf_priority = tbio->tbio_cache_priority;
	b->buf_sts |= BUF_PRIOFIX;
    }

    /*
    ** Temporary tables have no recovery or concurrency requirements and
    ** do not need special unfix & commit actions.
    */
    if (tbio->tbio_temporary)
    {
        b->buf_sts &= ~(BUF_WUNFIX | BUF_PAWRITE | BUF_TUNFIX |
	    BUF_WUNFIX_REQ | BUF_NULL_TUNFIX);
	b->buf_sts |= BUF_TEMPTABLE;

	/* If temptable, reduce priority by one */
	b->buf_priority--;
    }
    else if (tbio->tbio_cache_flags & TBIO_CACHE_DMCM)
    {
	/* 
	** (ICL phil.p)
	** DMCM does not require any of the other Buffer Manager protocols. 
	** System catalogs are exempt from the DMCM protocol.
	*/
        if (tbio->tbio_sysrel) 
        {
            b->buf_sts |= BUF_NULL_TUNFIX;
            b->buf_sts &=~ BUF_DMCM;
        }
        else
            b->buf_sts |= BUF_DMCM;
    }
    else if ((tbio->tbio_sysrel) || (tbio->tbio_extended) ||
	    (page_type == DMPP_FHDR) ||
	    (page_type == DMPP_FMAP) ||
	    (page_type == DMPP_INDEX))
    {
	if (( ! (tbio->tbio_cache_flags & TBIO_CACHE_FASTCOMMIT)) ||
	    (tbio->tbio_cache_flags & TBIO_CACHE_MULTIPLE) ||
	    (tbio->tbio_cache_flags & TBIO_CACHE_RCP_RECOVERY) ||
	     tbio->tbio_reltid.db_tab_base == DM_B_SEQ_TAB_ID ||
	    ((tbio->tbio_reltid.db_tab_base == DM_B_RELATION_TAB_ID) && 
	     (tbio->tbio_reltid.db_tab_index == DM_I_RELATION_TAB_ID)))
	{
	    b->buf_sts |= BUF_WUNFIX;
	}
	else
	{
	    b->buf_sts |= BUF_PAWRITE;
	}

	if ( ! (tbio->tbio_cache_flags & TBIO_CACHE_MULTIPLE))
	{
	    if ( dm0p_recover_state(log_id) )
		b->buf_sts |= BUF_TUNFIX;
	}
    }

    dm0p_munlock(&b->buf_mutex);

    return;
}

/*{
** Name: dm0p_uncache_fix	- Unfix a buffer in the page cache.
**
** Description:
**	This routine unfixes a page previously fixed by dm0p_cachefix_page.
**
** Inputs:
**	tbio				Table IO Control Block.
**      action                          The action to fix page
**	lock_list			Lock List Id
**	log_id				Log Id
**	tran_id				Pointer to Transaction Id
**	page				Page to unfix
**
** Outputs:
**	page				Pointer reset to NULL
**	err_code			Reason for error return status.
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**
** History:
**	26-oct-1992 (rogerk)
**          Created for Reduced Logging Project.  Code moved here from old
**	    dm0p_unfix_page routine.
**	14-dec-1992 (rogerk)
**	    Chnaged consistency check which looked for pages being released
**	    by the transaction which had not been marked as PAWRITE by the
**	    buffer manager to bypass temporary tables and pages not fixed 
**	    for Update.
**	20-sep-1993 (bryanp)
**	    Restore accidentally-commented-out code in dm0p_uncache_fix.
**	18-oct-1993 (rmuth)
**	    Add DMP_BMCB statistics.
**	18-oct-1993 (rogerk)
**	    Changed the mlimit enforcing code to select pages off of the
**	    start of the modify priority queues rather than the end of
**	    the queues to choos the least-recently-used pages as force
**	    victims.
**	18-oct-1993 (rogerk)
**	    Added check for mlimit/flimit during LOGFULL conditions and
**	    the call to purge the current cache in this case.  This drastically
**	    reduces the amount of logforces required.
**	22-feb-1994 (andys)
**	    It is ok for mutex on buffer to be held if BUF_MUTEX_OK set.
**	23-may-1994 (jnash)
**	    The 18-oct-1993 change was off by one page, causing the code
**	    to never be executed.
**	27-jun-1995 (cohmi01)
**	    Use bitwise & instead of == for DM0P_FORCE test. 
**	24-oct-1995 (thaju02/stoli02)
**	    Added increment of buffer manager single sync write counter.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	25-mar-96 (nick)
**	    Added FCwait support.
**      06-may-1996 (nanpr01)
**          Change the reference to the page header with macros for
**	    New Page Format.
**	12-Feb-1997 (jenjo02)
**	  - Lock status mutex while signalling DM0P_FWAIT_EVENT.
**	  - If the modify limit is breached and WB threads are running,
**	    let them be responsible for forcing modified buffers instead
**	    of stifling this thread with another I/O.
**	  - Use SHARE mutex when searching modified queue for a buffer to force.
**      21-may-1997 (stial01)
**        - dm0p_uncache_fix: Crash server if refcount is 0 and buffer is locked
**          Do not put buf with IX page lock on xn queue
**      29-may-1997 (stial01)
**        - dm0p_uncache_fix: Added additional consistency checks if row locking
**	25-Aug-1997 (jenjo02)
**	    Pass log_id to dm0p_toss_pages().
**	05-Feb-1999 (jenjo02)
**	    Wait for busy buffer before proceding with the unfix!
**	12-Apr-1999 (jenjo02)
**	    When looking for a MODIFIED page to force, make sure it's not 
**	    also FIXED.
**    28-Feb-2005 (wanfr01,jenjo02)
**          Bug 113547, INGSRV3062
**          Stop doing dirty reads if we are restarting the scan too
**          many times to avoid cpu thrashing.
**	06-Mar-2007 (jonj)
**	    If unfixing during online cluster recovery and page is modified
**	    and should have a cache lock but does not, report the error, crash.
*/
DB_STATUS
dm0p_uncache_fix(
DMP_TABLE_IO	*tbio,
i4		action,
i4		lock_list,
i4		log_id,
DB_TRAN_ID	*tran_id,
DMPP_PAGE	**page,
i4		*err_code)
{
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    i4              cache_ix = tbio->tbio_cache_ix;
    DM0P_BMCB       *bm = lbm->lbm_bmcb[cache_ix];
    DM0P_BSTAT	    *lbm_stats = &lbm->lbm_stats[bm->bm_cache_ix];
    DM0P_BUFFER	    *buf_array = lbm->lbm_buffers[bm->bm_cache_ix];
    DM0P_GROUP      *group_array = lbm->lbm_groups[bm->bm_cache_ix]; 
    DM0P_GROUP      *g;
    DMPP_PAGE	    *pg = *page;
    DMP_TABLE_IO    *mb_tbio = NULL;
    DM0P_BUFFER	    *b;
    DM0P_BUFFER	    *mb;
    DM0P_STATE_HEAD *sth;
    DB_STATUS	    status = E_DB_OK;
    DB_STATUS	    sts;
    i4	    local_error;
    i4	    force_action = 0;
    i4	    next_buf, prev_buf;
    i4	    page_index;
    i4	    i;
    i4		    check_mlimits = FALSE;
    i4	    page_stat;

    /*
    ** Check that a legal page address was passed in and find the buffer
    ** descriptor for this page.
    **
    **      - Multiple Segments -     
    ** This won't work if page array is not contiguous.
    ** We have to check page address differently and find the correct offset
    ** to look up the buffer descriptor.
    */
    page_index = ((char *)pg - (char *)lbm->lbm_pages[bm->bm_cache_ix])/bm->bm_pgsize;

    /* lint truncation warning if size of ptr > int, but code valid */

    if (((int)*page  & (sizeof(ALIGN_RESTRICT) - 1)) || (page_index < 0) ||
	(page_index >= bm->bm_bufcnt))
    {
	ule_format(E_DM934C_DM0P_BAD_ADDRESS, (CL_SYS_ERR *)0, ULE_LOG,NULL,
	    (char *)0, (i4)0, (i4 *)0, err_code, 5,
	    0, page_index,
	    0, bm->bm_bufcnt,
	    0, (PTR)*page,
	    sizeof(*tbio->tbio_dbname), tbio->tbio_dbname,
	    sizeof(*tbio->tbio_relid),  tbio->tbio_relid);
	dmd_check(E_DM9306_DM0P_UNFIX_PAGE);
    }
    b = &buf_array[page_index];

    /* Lock the buffer */
    dm0p_mlock(SEM_EXCL, &b->buf_mutex);

    /* Extract page_stat once, for all */
    page_stat = DMPP_VPT_GET_PAGE_STAT_MACRO(b->buf_relpgtype, pg);


    /*
    ** If Clustered and this page is modified and does not have a cache lock
    ** but needs one, then crash. 
    **
    ** This indicates a problem with online REDO recovery
    **
    ** See "RedoLSN" references in cachefix_page.
    */
    if ( bm_common->bmcb_status & BMCB_CLUSTERED &&
	 (b->buf_sts & BUF_MODIFIED || page_stat & DMPP_MODIFY) &&
	 !(b->buf_sts & BUF_LOCKED) 
        && ((tbio->tbio_cache_flags & TBIO_CACHE_MULTIPLE
	       || (tbio->tbio_sysrel && 
		   tbio->tbio_reltid.db_tab_base == DM_B_RELATION_TAB_ID &&
		   tbio->tbio_reltid.db_tab_index == DM_I_RELATION_TAB_ID)
	       ||  tbio->tbio_cache_flags & TBIO_CACHE_DMCM)
	&& !(tbio->tbio_cache_flags & TBIO_CACHE_READONLY_DB)
	&& !tbio->tbio_temporary))
    {
	LG_LSN	*PageLSN = DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(b->buf_relpgtype, pg);

	TRdisplay("%@ uncache_fix: %d,%d,%d PageLSN %x.%x MODIFIED but not LOCKED tbio_cache_flags %x\n",
		b->buf_lock_key.lk_key2, b->buf_lock_key.lk_key3,
		b->buf_lock_key.lk_key4,
		PageLSN->lsn_high, PageLSN->lsn_low,
		tbio->tbio_cache_flags);
	TRdisplay("%8* REF=%2d CPCNT=%4d PRI=%2d TYPE=%w HASHB=%3d\n",
		    b->buf_refcount, b->buf_cpcount, b->buf_priority,
		    "FHDR,FMAP,ROOT,INDEX,LEAF,DATA", b->buf_type, b->buf_hash_bucket);
	TRdisplay("%8* TRANB=%2d TRAN=0x%x%x LKID=%x LKMODE %d V=%x%x PID=%x\n",
		    b->buf_txnq_bucket,
		    b->buf_tran_id.db_high_tran, b->buf_tran_id.db_low_tran,
		    b->buf_lk_id, b->buf_lock_mode, b->buf_oldvalue.lk_high, b->buf_oldvalue.lk_low,
		    b->buf_pid);
	TRdisplay("%8* STAMP=%x STATUS=%x %v\n",
		    b->buf_tcb_stamp, b->buf_sts,
		    "VALID,MODIFIED,FIXED,WRITE,LOCKED,CACHED,IOERROR,RECLAIM,\
RBUSY,WBUSY,WUNFIXRQ,INVALID,MUTEX,MWAIT,IOWAIT,WBBUSY,WCOMMIT,FCBUSY,\
TEMPTABLE,NOTXLOCK,WUNFIX,PAWRITE,PRIOFIX,TUNFIX,MUTEX_OK,TPRIO,FCWAIT,\
INTENT_LK,NULL_TUNFIX,DMCM,GATHER_QUEUE,MLOCK",
		    b->buf_sts);
	dmd_check(E_DM9306_DM0P_UNFIX_PAGE);
    }

# if 0
    /* 
    ** STIAL01 fix for b107946.
    */
    if ( (b->buf_sts & BUF_LOCKED) &&
	  (0 == (action & (DM0P_FORCE|DM0P_RELEASE))) )
    {
	page_stat = DMPP_VPT_GET_PAGE_STAT_MACRO(b->buf_relpgtype, pg);
	if ( (b->buf_tabid.db_tab_base < DM_SCONCUR_MAX ||
	     (page_stat & (DMPP_FHDR | DMPP_FMAP | DMPP_INDEX))) )
	{
	    /* TRdisplay( "DM0P_RELEASE s/b set.\n" ); */
	    action |= DM0P_RELEASE;
	}
    }
# endif

    /*
    ** Note when dm0p_uncache_fix is called from dm0p_unfix_page,
    ** DM0P_RELEASE is set in the action parameter when physical locks
    ** are being used for the table.  (TCB_CONCUR catalogs and etabs).
    ** 
    ** The DM0P_release flag triggers the update of cache locks when 
    ** cache locking is being done.
    **
    ** The setting of DM0P_RELEASE by dm0p_unfix_page does not cover all cases
    ** of physical page locking. It does not cover physical page
    ** locking for special FMAP, FHDR, INDEX pages. 
    **
    ** It also does not cover the unfixing of pages updated with physical locks
    ** from dmve (rollback) which calls dm0p_uncache_fix directly.
    **
    ** Since dm0p_uncache_fix is not always called from dm0p_unfix_page,
    ** test for physical page locking on special page types here.
    ** 
    ** Note checking (b->buf_sts & BUF_LOCKED) to determine if we are
    ** cache locking.
    */
    if ((b->buf_sts & BUF_LOCKED) && 
	(action & (DM0P_FORCE | DM0P_RELEASE)) == 0)
    {
	i4	page_stat;

	page_stat = DMPP_VPT_GET_PAGE_STAT_MACRO(b->buf_relpgtype, pg);
	if (b->buf_tabid.db_tab_base == DM_B_RELATION_TAB_ID ||
	    b->buf_tabid.db_tab_base == DM_B_ATTRIBUTE_TAB_ID ||
	    b->buf_tabid.db_tab_base == DM_B_INDEX_TAB_ID ||
	    (page_stat & (DMPP_FHDR | DMPP_FMAP | DMPP_INDEX)))
	{
	    action |= DM0P_RELEASE;
	}
    }

    /*
    ** 05-Feb-1999 (jenjo02)
    **
    ** Wait for busy buffer.
    **
    ** There are a few situations where this is possible:
    **
    **	o Another thread is fixing this page and is in
    **	  validate_page() which has set BUF_RBUSY and released
    **	  buf_mutex while it makes lock and/or I/O requests.
    **	o The CP thread is forcing this page (BUF_FCBUSY)
    **	o A DMCM callback has been fired and dm0p_dmcm_callback()
    **	  is forcing a fixed-for_read page in order to write it
    **	  (if modified) and downgrade its cache lock to LK_S.
    **	  (similar to the CP case).
    */
    while (b->buf_sts & (BUF_FCBUSY | BUF_BUSY))
    {
	if (b->buf_sts & BUF_FCBUSY)
	{
	    /*
	    ** don't need to do this if we don't intend to write
	    ** the buffer right now - this is different to 6.4 where
	    ** we wait regardless of action ; if the 'JPM' bug still
	    ** occurs ( it shouldn't ) then the removal of the following 
	    ** two lines is advised
	    */
	    /* (jenjo02) This looks dangerous, particularly in that we're
	    ** about to screw around with the page's dirty bits (just below)
	    ** while cp_flush is in the process of writing the page.
	    ** I've seen (rare) incomplete cp errors with no obvious cause; 
	    ** this is certainly suspect. What the hell is the 'JPM' bug???
	    if (action & DM0P_UNFIX)
		break;
	    */

	    lbm_stats->bs_fcwait[BMCB_PTYPES]++;
	    bm->bm_stats.bs_fcwait[BMCB_PTYPES]++;
	    lbm_stats->bs_fcwait[b->buf_type]++;
	    bm->bm_stats.bs_fcwait[b->buf_type]++;
	    b->buf_sts |= BUF_FCWAIT;
	    dm0p_eset(lock_list, DM0P_FCWAIT_EVENT, 
		b->buf_id.bid_cache, b->buf_id.bid_index, 0); 
	    dm0p_munlock(&b->buf_mutex);

	    /* Wait for the FC thread to finish with this buffer. */
	    dm0p_ewait(lock_list, DM0P_FCWAIT_EVENT,
		b->buf_id.bid_cache, b->buf_id.bid_index, 0, 0); 
	    dm0p_mlock(SEM_EXCL, &b->buf_mutex);
	}

	if (b->buf_sts & BUF_BUSY)
	{
	    lbm_stats->bs_iowait[BMCB_PTYPES]++;
	    bm->bm_stats.bs_iowait[BMCB_PTYPES]++;
	    lbm_stats->bs_iowait[b->buf_type]++;
	    bm->bm_stats.bs_iowait[b->buf_type]++;
	    b->buf_sts |= BUF_IOWAIT;
	    dm0p_eset(lock_list, DM0P_BIO_EVENT,
		b->buf_id.bid_cache, b->buf_id.bid_index, 0); 
	    dm0p_munlock(&b->buf_mutex);

	    /* Wait for the I/O event to complete. */
	    dm0p_ewait(lock_list, DM0P_BIO_EVENT,
		b->buf_id.bid_cache, b->buf_id.bid_index, 0, 0); 
	    dm0p_mlock(SEM_EXCL, &b->buf_mutex);
	}
    }


    /*
    **	Check the information stored in the buffer is consistent with
    **  the page being unfixed.
    */
    if ((b->buf_lock_key.lk_key4 != 
               DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, pg)) ||
	(b->buf_tabid.db_tab_base != tbio->tbio_reltid.db_tab_base) ||
	((b->buf_sts & BUF_FIXED) == 0) ||
	(((b->buf_sts & BUF_WRITE) == 0) && 
	 (DMPP_VPT_GET_PAGE_STAT_MACRO(b->buf_relpgtype, pg) & DMPP_MODIFY)))
    {
	if ((action & DM0P_CREATE_NOFMT) == 0)
	{
	    ule_format(E_DM934D_DM0P_BAD_PAGE, (CL_SYS_ERR *)0, ULE_LOG,NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 9,
		0, DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, pg),
		0, b->buf_lock_key.lk_key4,
		0, tbio->tbio_reltid.db_tab_base,
		0, b->buf_tabid.db_tab_base,
		0, tbio->tbio_reltid.db_tab_index,
		0, b->buf_tabid.db_tab_index,
		0, b->buf_sts,
		sizeof(*tbio->tbio_dbname), tbio->tbio_dbname,
		sizeof(*tbio->tbio_relid),  tbio->tbio_relid);
	    dmd_check(E_DM9306_DM0P_UNFIX_PAGE);
	}
    }

    /*
    ** Consistency Check: There are currently some strong assumptions
    ** between the use of the DM0P_RELEASE flags and the buffer
    ** status fields BUF_PAWRITE/BUF_WUNFIX.  The buffer manager sets
    ** cache protocol flags in fault_page on those types of pages that
    ** it believes to be updated with Temporary Physical locks.
    **
    ** If the buffer has been fixed for write and we are using temporary
    ** locks on the page, but the buffer manager has not recognized this
    ** as one of those types of pages then there may be some cache protocol
    ** problems.
    **
    ** (ICL phil.p)
    ** When running the DMCM protocol, we deliberately do not set any of
    ** the other cache protocol flags. So, de-activate this check
    ** otherwise it will continually be tripped.
    */
    if (!(tbio->tbio_cache_flags & TBIO_CACHE_DMCM))
    {
        if ((action & DM0P_RELEASE) &&
            (b->buf_sts & BUF_WRITE) &&
            ( ! tbio->tbio_temporary) &&
            ( ! (b->buf_sts & (BUF_PAWRITE | BUF_WUNFIX))))
        {
            TRdisplay("Buffer Manager Consistency Check: Unfix RELEASE\n");
            TRdisplay("    specified on page which was not marked as a\n");
            TRdisplay("    temporary locked page\n");
        }
    }

    /*
    ** Row locking protocols:
    ** 
    **     Row locking is supported only for fast commit servers.
    **
    **     Any buffer fixed with only an intent lock cannot be written 
    **     when the page is fixed unless the refcount is 0. Since the  
    **     page is not mutexed while it is being written, when row locking 
    **     you have the possibility of another session mutexing/updating
    **     the page while it is being written. 
    **
    **     When row locking we wait for the fast commit thread to write
    **     the buffer after it is no longer fixed.
    */
    if (b->buf_sts & BUF_INTENT_LK)
    {
        if (action & DM0P_WRITE)
	{
	    TRdisplay("Unfix-WRITE specified on buffer with INTENT lock\n");
	    action &= ~(DM0P_WRITE);
	}
	if (action & DM0P_FORCE)
	{
	    TRdisplay("Unfix-FORCE specified on buffer with INTENT lock\n");
	    dmd_check(E_DM9289_BM_BAD_MUTEX_PROTOCOL);
	}
    }

    /*
    ** Disallow unfix if same lock list has buffer locked
    */
    if (b->buf_sts & BUF_MLOCK && b->buf_lock_list == lock_list)
    {
	ule_format(E_DM9C01_FIXED_BUFFER_INFO, (CL_SYS_ERR *)0,
		    ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 6,
		    0, b->buf_lock_key.lk_key4,
		    0, b->buf_tabid.db_tab_base,
		    0, b->buf_tabid.db_tab_index,
		    0, b->buf_sts,
		    sizeof(DB_DB_NAME), tbio->tbio_dbname,
		    0, (BUF_FIXED | BUF_MLOCK) );
	dmd_check(E_DM9289_BM_BAD_MUTEX_PROTOCOL);
    }


    /*
    **  Copy dirty bits from page to buffer and reset page.
    **  IGNORE dirty bits on page if the page number is invalid
    */

    if ((DMPP_VPT_GET_PAGE_STAT_MACRO(b->buf_relpgtype, pg) & DMPP_MODIFY) &&
	b->buf_lock_key.lk_key4 ==
		DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, pg))
    {
	lbm_stats->bs_dirty[BMCB_PTYPES]++;
	bm->bm_stats.bs_dirty[BMCB_PTYPES]++;
	lbm_stats->bs_dirty[b->buf_type]++;
	bm->bm_stats.bs_dirty[b->buf_type]++;
	b->buf_sts |= BUF_MODIFIED;
	DMPP_VPT_CLR_PAGE_STAT_MACRO(b->buf_relpgtype, pg, DMPP_MODIFY);
    }

    if (action & DM0P_FORCE)
    {
	if ((b->buf_sts & BUF_INVALID) == 0)
	{
	    /* Count another force */
	    lbm_stats->bs_force[BMCB_PTYPES]++;
	    bm->bm_stats.bs_force[BMCB_PTYPES]++;
	    lbm_stats->bs_force[b->buf_type]++;
	    bm->bm_stats.bs_force[b->buf_type]++;

	    do
		status = force_page(b, tbio, FP_WRITE,
		    lock_list, log_id, BMCL_NO_GATHER, (i4*)0, err_code);
	    while ( status == E_DB_WARN );

	    if ( status != E_DB_OK )
		dm0p_munlock(&b->buf_mutex);
	}
	else
	{
	    status = E_DB_OK;
	    dm0p_munlock(&b->buf_mutex);
	}

	if (DB_FAILURE_MACRO(status) && (*err_code > E_DM_INTERNAL))
	{
	    ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
	    *err_code = E_DM9C87_DM0P_UNCACHE_FIX;
	}
	return (status);
    }

    /*
    ** Operation now involves unfixing a page.
    */
    status = E_DB_OK;

    lbm_stats->bs_unfix[BMCB_PTYPES]++;
    bm->bm_stats.bs_unfix[BMCB_PTYPES]++;
    lbm_stats->bs_unfix[b->buf_type]++;
    bm->bm_stats.bs_unfix[b->buf_type]++;

    if (--b->buf_refcount == 0)
    {
	/*
	** Check if any mutex held on the page.
	** Should not be a mutex unless the fast commit thread is
	** flushing this page.
	*/
	if ((b->buf_sts & BUF_MUTEX) && ((b->buf_sts & BUF_MUTEX_OK) == 0))
	{
	    ule_format(E_DM9C01_FIXED_BUFFER_INFO, (CL_SYS_ERR *)0,
	    		ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 6,
			0, b->buf_lock_key.lk_key4,
			0, b->buf_tabid.db_tab_base,
			0, b->buf_tabid.db_tab_index,
			0, b->buf_sts,
			sizeof(DB_DB_NAME), tbio->tbio_dbname,
			0, (BUF_FIXED | BUF_MUTEX) );
	    dmd_check(E_DM9289_BM_BAD_MUTEX_PROTOCOL);
	}

	/*
	** Check if the buffer is locked.
	*/
	if (b->buf_sts & BUF_MLOCK)
	{
	    ule_format(E_DM9C01_FIXED_BUFFER_INFO, (CL_SYS_ERR *)0,
	    		ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 6,
			0, b->buf_lock_key.lk_key4,
			0, b->buf_tabid.db_tab_base,
			0, b->buf_tabid.db_tab_index,
			0, b->buf_sts,
			sizeof(DB_DB_NAME), tbio->tbio_dbname,
			0, (BUF_FIXED | BUF_MLOCK) );
	    dmd_check(E_DM9289_BM_BAD_MUTEX_PROTOCOL);
	}

	/*
	** If buffer not already on transaction queue , put
        ** it on.  Buffers locked exclusively (fixed for write)
        ** may already be on transaction queue.  The buffers
        ** fixed for read or shared access are not placed on 
        ** a transaction queue until there are no more references.
	**
	** FHDR/FMAP pages are always fixed for write, but by
	** multiple threads, so the test for BUF_WRITE no longer
	** assures that the buffer is already on a txn queue,
	** so we'll overtly check the queue instead.
        */
	
	/* If it's already on the right transaction queue, do nothing */
	if (tran_id->db_high_tran != b->buf_tran_id.db_high_tran ||
	    tran_id->db_low_tran  != b->buf_tran_id.db_low_tran)
	{
	    /*
	    ** If the buffer is currently on a transaction queue, then
	    ** remove it.
	    */
	    if (b->buf_txnq_bucket != BUF_NOBUCKET)
	    {
		txnq_delete(b);
	    }

	    /*
	    ** Insert buffer onto new transaction queue.
	    */
	    STRUCT_ASSIGN_MACRO(*tran_id, b->buf_tran_id);
	    if (tran_id->db_high_tran | tran_id->db_low_tran)
	    {
		txnq_insert(b, log_id);
	    }
	}

	/*
	** Unfix the buffer.
	*/
	b->buf_sts &= ~(BUF_FIXED | BUF_WRITE | BUF_INTENT_LK | BUF_NOTXLOCK);


	if (b->buf_owner == BUF_SELF)
	{
	    if ((action & DM0P_HI_PRIORITY) && (b->buf_sts & BUF_HI_TPRIO))
	    {
		/* buf's priority had been raised previously, probably by  */
		/* prefetch. Now we are told it is ok to lower it.         */
		b->buf_sts &= ~BUF_HI_TPRIO;
		if (b->buf_priority > 0)
		    b->buf_priority--;
	    }

	    /*
	    ** Place buffer on the end of modified or free queue (depending on if the
	    ** page was updated or not).  If the page is part of a group
	    ** buffer then requeue the group descriptor if necessary.
	    ** Put on the end of the state queue so that pages already at this
	    ** priority will be bumped out first.
	    */

	    if (b->buf_sts & BUF_MODIFIED)
	    {
		sth = &bm->bm_mpq[b->buf_priority];
		/* Adding to modified queue, check mlimits */
		check_mlimits = TRUE;
	    }
	    else
		sth = &bm->bm_fpq[b->buf_priority];
		
	    dm0p_mlock(SEM_EXCL, &sth->sth_mutex);
	    prev_buf = sth->sth_stq.stq_prev;
	    b->buf_state.stq_next = BUF_ENDLIST;
	    b->buf_state.stq_prev = prev_buf;
	    if (prev_buf != BUF_ENDLIST)
		buf_array[prev_buf].buf_state.stq_next = 
		    b->buf_id.bid_index;
	    else
		sth->sth_stq.stq_next = b->buf_id.bid_index;
	    sth->sth_stq.stq_prev = b->buf_id.bid_index;

	    /* Count one more free/mod buffer, one less fixed */
	    sth->sth_count++;
	    sth->sth_lcount--;
	    
	    dm0p_munlock(&sth->sth_mutex);

	    if ((b->buf_sts & BUF_MODIFIED) == 0 &&
		bm->bm_status & BM_FWAIT)
	    {
		dm0p_mlock(SEM_EXCL, &bm->bm_mutex);
		if (bm->bm_status & BM_FWAIT)
		{
		    dm0p_erelease(lock_list, DM0P_FWAIT_EVENT, 
				    bm->bm_cache_ix, 0, 0);
		    bm->bm_status &= ~BM_FWAIT;
		}
		dm0p_munlock(&bm->bm_mutex);
	    }
	}
	else
	{
	    /* Count one less fixed buffer in this group */
	    g = &group_array[b->buf_owner];
	    dm0p_mlock(SEM_EXCL, &g->g_mutex);
	    move_group(bm, g, b, lock_list,
			-1,
			(b->buf_sts & BUF_MODIFIED) ? 1 : 0,
			0);
	    dm0p_munlock(&g->g_mutex);
	}

	/*
	** Compute the cache action.
	**
	** If the caller specified to write the buffer or if the buffer
	**    is marked that it must be written when unfixed, then make sure
	**    the buffer is written (through FP_WRITE or FP_CACHE actions).
	**
	** If the caller specified to release the buffer from the list of
	**    pages owned by this transaction then make sure that the force
	**    action includes CACHE (if the buffer must be written) or DISOWN 
	**    (if it doesn't).
	**
	** The Toss-on-unfix and Invalid flags in the buffer will override
	**    any of the above options or caller requests.
	*/

	if ((action & DM0P_WRITE) || (b->buf_sts & BUF_WUNFIX))
	{
	    force_action = FP_WRITE;
	}

	/*
	** Release indicates to remove the page from the transaction list.
	** It is used for system catalog and other shortterm-locked pages.
	**
	** If release is required, then specify DISOWN or CACHE (which is
	** the same as WRITE + DISOWN).
	*/
	if (action & DM0P_RELEASE)
	{
	    if (force_action == FP_WRITE)
		force_action = FP_CACHE;
	    else
		force_action = FP_DISOWN;
	}

	if (b->buf_sts & BUF_INVALID)
	    force_action = FP_INVALID;

	if (b->buf_sts & BUF_TUNFIX)
	{
	    force_action = FP_TOSS;
	}

        /*
        ** (ICL phil.p)
        ** Set appropriate force_page actions for
        ** buffers under DMCM procotol.
        ** We use the tbio_cache_flags for this check
        ** because system catalogs which do not
        ** participate under DMCM should still have
        ** these operations performed on them, but
        ** they will not have the BUF_DMCM flags set.
        */
        if (tbio->tbio_cache_flags & TBIO_CACHE_DMCM)
        {
            if (b->buf_sts & BUF_NULL_TUNFIX)
                force_action = FP_NULL;

	    /*
	    ** Whether modified or not, leave the DMCM
	    ** page cached with in its current mode, X or S.
	    ** If a conflict arises, a callback will fire
	    ** and cause this page to be forced with FP_NULL.
	    ** This avoids extra, unnecessary calls to LK to
	    ** downgrade and upgrade the cache lock.
	    ** 05-Feb-1999 (jenjo02)
	    */
        }

	/*
	**  Force the page if needed.
	**  This status will become the return status.  A failed
	**  force will get reported to the caller and the buffer
	**  will still be marked as modified.
	*/
	if (force_action)
	{
	    do
	    {
		status = force_page(b, tbio, force_action,
		    lock_list, log_id, BMCL_NO_GATHER, (i4*)0, err_code);
	    } while (status == E_DB_WARN);

	    if ( status != E_DB_OK )
		dm0p_munlock(&b->buf_mutex);
	}
	else
	    dm0p_munlock(&b->buf_mutex);

	/*
	** Recovery LOGFULL protocols:
	**
	** The buffer manager currently has a problem with cache inflation where
	** most of its cached pages get bumped up to have a very high priority
	** weighting leaving no room for lower priority pages.
	**
	** When this occurs and one of the mlimit/flimit values are reached, then
	** it becomes very hard for newly faulted in pages to stay in the cache.
	** They tend to get chosen as toss victims almost immediately before they
	** can be rereferenced enough to join the clan of high priority pages.
	**
	** When this situation occurs during recovery it can have a nasty side
	** effect.  That is that each update operation has its modified page tossed
	** from the cache (almost) immediately upon unfixing it.  This cache toss
	** results in a Log File Force which we wish to avoid as much as possible
	** during recovery.  When LOGFULL is reached the situation becomes somewhat
	** critical - we must control logspace usage and keep forces to a bare
	** minimum.
	**
	** When the flimit or mlimit is reached during logfull situations we
	** take an anti-cache-inflation measure of purging out the entire cache -
	** eliminating the flimit/mlimit along with the current group of pages.
	** This purge requires one logforce rather than 1 per page when tossing
	** each page as it is modified.
	**
	** (jenjo02)	I moved this code down here so that we can do the cache
	**	   	flush after releasing the buffer's mutex.
	*/

	if ( check_mlimits )
	{
	    i4		fcount, mcount, lcount;

	    /* Get real modified/free counts, ignore fixed */
	    DM0P_COUNT_ALL_QUEUES(fcount,mcount,lcount);
	   
	    if ( mcount + 1 >= bm->bm_mlimit || 
		 fcount < bm->bm_flimit )
	    {
		if (dm0p_check_logfull())
		{
		    dm0p_toss_pages((i4)0, (i4)0, lock_list, 
					log_id, (i4) TRUE);
		    /* Recount modified/free pages */
		    DM0P_COUNT_ALL_QUEUES(fcount,mcount,lcount);
		}
	    }

	    /*
	    ** Check if there are more modified pages than the modify limit or
	    ** the write-behind start level (the modify limit is at least as
	    ** great as the write-behind start level).
	    **
	    ** If the modify count exceeds the writebehind start level, then we
	    ** signal the write behind threads to flush pages out of the cache.
	    ** If the modify count exceeds the modify limit, then we have to
	    ** begin to synchronously write pages.
	    **
	    ** NOTE that this action is take even if the above force_page call
	    ** failed - we must be careful to preserve the "status" return value.
	    */
	    if (mcount >= bm->bm_wbstart)
	    {
		/*
		** If the write behind threads are not currently running, then
		** wake them up.
		*/
		if ( bm->bm_status & BM_WB_IN_CACHE &&
		     bm->bm_status & BM_WBWAIT )
		{
		    wb_cycle_start(bm, lock_list, 
				    BM_WB_SINGLE, "UNCACHE_FIX");
		}

		/*
		** If the modify limit has been reached, then we have to begin
		** synchronous writes.  Find and flush the oldest lowest priority page
		** that is not being reclaimed and that we have a TCB for.
		*/
		if ( mcount >= bm->bm_mlimit &&
		    ((bm->bm_status & BM_WB_IN_CACHE) == 0 ||
		      bm->bm_status & BM_WBWAIT) )
		{
		    for (i = 0, mb = NULL; 
			 mb == NULL && i < DB_MAX_TABLEPRI; 
			 i++)
		    {
			/* Oldest buffers are the head of the queue */
			next_buf = bm->bm_mpq[i].sth_stq.stq_next;

			while ( next_buf != BUF_ENDLIST )
			{
			    mb = &buf_array[next_buf];

			    /* 
			    ** Lock the buffer, then recheck it.
			    ** After waiting for the buf_mutex, the
			    ** buffer may no longer be on the modified queue.
			    */
			    dm0p_mlock(SEM_EXCL, &mb->buf_mutex);

			    next_buf = mb->buf_state.stq_next;

			    if ( (mb->buf_sts & (BUF_BUSY | BUF_WBBUSY |
						BUF_FIXED | BUF_MODIFIED))
				    != BUF_MODIFIED ||
				 mb->buf_priority != i )
			    {
				if ( (mb->buf_sts & (BUF_FIXED | BUF_MODIFIED))
					!= BUF_MODIFIED ||
				    mb->buf_priority != i )
				{
				    next_buf = bm->bm_mpq[i].sth_stq.stq_next;
				}
			    }
			    else
			    {
				/*
				** Look for a Table Descriptor with which to write
				** out this page.  If we don't find one, then look
				** for another buffer to reclaim.
				*/
				sts = fix_tableio_ptr(mb, (i4)0, lock_list, 
				    log_id, &mb_tbio, &local_error);

				if (sts == E_DB_OK)
				    break;

				/*
				** Table Descriptor not found.  If an error was 
				** returned, just log it and continue processing.
				*/
				if (DB_FAILURE_MACRO(sts))
				{
				    ule_format(local_error, (CL_SYS_ERR *)0, 
					ULE_LOG, NULL, (char *)0, (i4)0, 
					(i4 *)0, &local_error, 0);
				    ule_format(E_DM9C80_ERROR_FINDING_TABIO_PTR,
					(CL_SYS_ERR *)0, ULE_LOG, NULL, (char *)0, 
					(i4)0, (i4 *)0, &local_error, 0);
				}
			    }
			    dm0p_munlock(&mb->buf_mutex);
			    mb = NULL;
			}
		    }
		    if (mb)
		    {
			lbm_stats->bs_syncwr[BMCB_PTYPES]++;
			bm->bm_stats.bs_syncwr[BMCB_PTYPES]++;
			lbm_stats->bs_syncwr[mb->buf_type]++;
			bm->bm_stats.bs_syncwr[mb->buf_type]++;

			/*
			** Force the selected buffer.  If the force fails, log
			** the error, but continue on since the original unfix
			** operations has been successful.
			*/
			sts = force_page(mb, mb_tbio, (i4)FP_WRITE,
				    lock_list, log_id, BMCL_NO_GATHER,
				    (i4*)0, &local_error);
			if ( sts != E_DB_OK )
			{
			    dm0p_munlock(&mb->buf_mutex);
			    if (DB_FAILURE_MACRO(sts))
			    {
				ule_format(local_error, (CL_SYS_ERR *)0, ULE_LOG, NULL,
				    (char*)0, (i4)0, (i4*)0, &local_error, 0);
			    }
			}
		    }

		    /*
		    ** Release the table io control block used to force the 
		    ** victim buffer.
		    */
		    if (mb_tbio)
			unfix_tableio_ptr(&mb_tbio, lock_list, log_id, (i4)__LINE__);
		}
	    }
	}
    }
    else if (action & DM0P_WRITE)
    {
	/*
	** If action specified was DM0P_WRITE, then we need to force the
	** page to disk even if the reference count has not gone to zero.
	**
	** If force_page returns E_DB_WARN, then the page was busy and
	** we should retry the force.
	*/
	do
	{
	    status = force_page(b, tbio, FP_WRITE, lock_list, log_id,
		BMCL_NO_GATHER, (i4*)0, err_code);
	} while (status == E_DB_WARN);

	if ( status != E_DB_OK )
	    dm0p_munlock(&b->buf_mutex);
    }
    else if (b->buf_txnq_bucket != BUF_NOBUCKET &&
	     b->buf_tran_id.db_high_tran == tran_id->db_high_tran &&
	     b->buf_tran_id.db_low_tran == tran_id->db_low_tran)
    {
	/*
	** Buffer is unfixed by this transaction but is still fixed
	** by other transactions. If it is on this transaction's 
	** queue, remove it. When the last unfix occurs, the buffer
	** will be placed on the last unfixer's transaction's queue.
	*/
	txnq_delete(b);
	dm0p_munlock(&b->buf_mutex);
    }
    else
	dm0p_munlock(&b->buf_mutex);
	

    *page = NULL;

    if (DB_FAILURE_MACRO(status) && (*err_code > E_DM_INTERNAL))
    {
	ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, err_code, 0);
	*err_code = E_DM9C87_DM0P_UNCACHE_FIX;
    }

    return (status);
}

/*
** Name: force_page	- Handle special action on buffer pages.
**
** Description:
**      This routine allows the caller to either free a buffer or
**	cache the buffer.
**
**	FP_CACHE	Write the buffer if changed, but leave in the
**			cache for future reference. Convert cache lock
**			to IS and mark as CACHED.  Move to free buffer
**			queue.  Take off of transaction queue.
**			This mode is called at end transaction time to
**			force out pages touched during the transaction.
**
**      FP_NULL         Write the buffer if changed, but leave in the
**                      cache for future reference. Convert cache lock
**                      to N and mark as CACHED.  Move to free buffer
**                      queue if not fixed. Take off of transaction queue.
**
**	FP_WRITE	Write the buffer if changed move from modified
**			to free queue if not fixed.  Bump cache lock value
**			if buffer written.
**			This mode is used when the caller does not want
**			to release a page, but wants to guarantee that the
**			page is written.
**
**	FP_TOSS		Place buffer on the free queue
**			after writing the buffer to disk.  Release the
**			cache lock, take off of transaction queue.
**			This mode is used to free a spot in the buffer
**			cache to make room for a new page.
**
**	FP_DISOWN	Remove the buffer from the transaction queue.
**			This is used for bitmap allocation pages which
**			can be pass around without being written to disk.
**
**	FP_INVALID	Place buffer on the free queue.
**			Bump cache lock value but do not write page.
**			Release cache lock.
**			This mode is used to throw away a page without
**			writing its contents.
**
**	This routine is entered with the buffer manager mutex being
**	held.  If this routine has to write a page to disk, the mutex is
**	dropped during the write and regained before returning.
**
**	The caller must pass in the TCB to the table if an I/O operation
**	is required.  The caller may pass in a null pointer if it can
**	be sure no I/O is required.  There will be no I/O if the operation
**	if FP_INVALID or if the page is not modified.
**
**	The return codes are:
**
**	E_DB_OK: all well.  The buffer mutex is released UNLESS the
**		buffer was marked BUF_RECLAIM, in which case it's still held.
**
**	E_DB_INFO: A very specific error case that leaves the buffer
**		invalidated and the buf_mutex NOT held.  This only happens
**		if:  the buffer holds a temp table page, the caller is trying
**		to RECLAIM it, the caller is not the owner of the temp table,
**		and we get a DISK FULL error writing the page.
**		The reason for a special INFO status is that in the reclaim
**		case, the caller is expecting us to hold the buf mutex,
**		and that's impossible (indeed, the caller has to try again
**		with a different buffer now).
**
**	E_DB_WARN: The force operation needs to be retried for some reason,
**		e.g. the buffer was busy.  The buf mutex is held.
**
**	E_DB_ERROR: The force failed, and we hold the buf mutex.
**
**
** Inputs:
**      buffer                          The buffer to operate on.
**	operation			The operation either:
**					    FP_TOSS - toss page from cache
**					    FP_CACHE - keep page but not owned
**                                          FP_NULL - keep page not owned with
**                                                    null lock.
**					    FP_WRITE - keep page but write
**					    FP_WRITEALONG  - like FP_WRITE
**						but put in free queue at max
**						priority so it stays around.
**					    FP_INVALID - don't write page but
**						invalidate cache lock.
**					    FP_DISOWN - remove from tx queue
**	lock_list			The locking system transaction id.
**	tcb				The TCB for table buffer belongs to.
**					This may be NULL if the buffer requires
**					no IO operations (buffer not modified).
**
** Outputs:
**      err_code                        The reason for the error status.
**	Returns:
**	    E_DB_OK			Successful completion.
**	    E_DB_INFO			Ignorable error, buf invalidated.
**	    E_DB_WARN			Retry operation.
**	    E_DB_ERROR/FATAL		Severe or Fatal error.
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	18-oct-1985 (derek)
**	    Created new for Jupiter.
**	31-oct-1988 (rogerk)
**	    Changed routine to determine how to manipulate buffer queues
**	    after doing and I/O and Lock work.  This is needed since the
**	    the buffer manager mutex is given up while doing IO and lock
**	    requests, and another thread may change the buffer's status
**	    effecting what should be done with it.
**	31-oct-1988 (rogerk)
**	    Pass page log address to dm0l_force to only force log if it
**	    has not been forced already.
**	 4-dec-1988 (rogerk)
**	    Update free and modify counts when invalidating a modified buffer.
**	28-feb-1989 (rogerk)
**	    Added support for Shared Buffer Manager.
**	    Added tcb parameter.
**	26-jun-1989 (rogerk)
**	    Handle case where bumping out cache-locked pages but don't have a
**	    TCB ptr - took out references to tcb in the xDEBUG and error code.
**	10-jul-1989 (rogerk)
**	    When write out a page, turn off the BUF_EXTENDPAGE status.
**	17-oct-1991 (rogerk)
**	    Merged changes made by Derek during the file extend project.
**	    Added new FP_DISOWN action.
**	    Added trace flags in xDEBUG code.
**	    Added new handling for EOF checking : use tcb_lalloc field.
**	    Took out use of obsolete BUF_EXTENDPAGE.
**	    Added new error message.
**      2-oct-1992 (ed)
**          Use DB_MAXNAME to replace hard coded numbers
**          - also created defines to replace hard coded character strings
**          dependent on DB_MAXNAME
**	3-oct-1992 (bryanp)
**	    We must have a transaction active in order to call dm0l_logforce.
**      07-oct-92 (jnash)
**          6.5 reduced logging project.
**	     -  Insert page checksum.
**	26-oct-1992 (rogerk)
**	    Reduced logging project: 
**	      - changed references to tcb fields to use new tcb_table_io 
**		structure fields.
**	      - Added buffer manager transaction context to be used by
**		force_page to force the log file before writing pages.
**	30-October-1992 (rmuth)
**	    - Set tbio_checkeof if trying to force past tcb_alloc.
**	    - For temp tables update tbio_tmp_hw_pageno.
**	14-dec-1992 (jnash & rogerk)
**	    Removed DMPP_ACC_PLV argument from dm2f_write_file.
**	18-jan-1993 (rogerk)
**	    Removed the bypassing of log forces from the recovery process.
**	    Since CLR's are now written during UNDO processing, we must
**	    always make sure that the log file is correctly forced when
**	    forcing pages out of the buffer manager.
**	24-feb-1993 (keving)
**	    => Added changes to allow a buffer manager to work with 
**	    => multiple logs
**	24-may-1993 (bryanp)
**	    Fix an error message parameter (LK_N should have been LK_IS).
**	    If trace point DM302 is set, trace cache lock requests.
**	18-oct-1993 (rmuth)
**	    Add DMP_BMCB statistics.
**	22-feb-1994 (andys)
**	    Integrate recent changes from ingres63p:
**		11-jul-1993 (rogerk & andys)
**		    Update buf_cpcount when force out a page during a
**		    Consistency Point.  This guarantees that the page
**		    will be marked processed by the current CP even if
**		    the page is remodified by a server whose FC thread
**		    has already bypassed the page.
**		 1-nov-1993 (rogerk & andys)
**		    After a buffer has been written, and before
**		    clearing the buffer status by setting it to
**		    BUF_FREE, wake up any threads that are waiting  on
**		    this buffer. [bug 54128]
**		17-nov-1993 (rogerk)
**		    Add check for calling force on a modified page with
**		    a null TCB.  This has been occurring periodically
**		    at CODEX.  The new checks only print out some
**		    additional information, they do not prevent the
**		    ensuing access violation when the tcb is
**		    dereferenced.
**	    Put codex diagnostic inside #ifdef xDEBUG.
**	23-feb-1994 (andys)
**	    Clean up code inside xDEBUG which had not been updated for 1.0
**	18-apr-1994 (jnash)
**	    fsync project.  Eliminate dm2f_force_file() call.
**	06-mar-1995 (harpa06 / abowler)
**          Bug #66815: Imported 6.4 change - Suppress error message reporting 
**          when a page is trying to be forced to a temorary file past EOF when
**          write-behind threads exist.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	20-mar-1996 (nanpr01)
**	    Added page size as parameter to this routine. To compute
**	    the checksum pagesize is need since sizeof(*page) returns
**	    only the size of physical page.
**      06-may-1996 (nanpr01)
**	    Changed the header access as macro.
**      01-aug-1996 (nanpr01 for ICL phil.p)
**          Introduced another mode, FP_NULL, exclusively used by the
**          Distributed Multi-Cache Management (DMCM) protocol; and a new
**          action NULL_LOCK to cause a ccache lock to be converted to null.
**      01-aug-1996 (nanpr01)
**	    Initialize the err_code to 0 otherwise if the buffer is busy
**	    errors get logged in errlog.log for uninitialized error code.
**	14-Sep-1996 (jenjo02)
**	    Caller must hold buffer's buf_mutex.
**	26-sep-1996 (nanpr01)
**	    BTree corruption problem. It is possible for consistency points
**	    to write a fixed page however if we do so we cannot drop the 
**	    cache lock to NULL.. We just write the page out.
**	14-Mar-1997 (jenjo02)
**	    Reinstated lbm_lxid_mutex, used in those cases in which an lx_id
**	    isn't passed to this function to single-thread the use of 
**	    the BM's default lx_id.
**	08-apr-1997 (nanpr01)
**	    Corrected Gather write dm2f_write_list parameter list.
**	02-oct-1998 (somsa01)
**	    Pass on the log_id to complete_f_p().
**	08-Jun-1999 (jenjo02)
**	    Moved update of temptable tbio_tmp_hw_pageno to complete_f_p(),
**	    til after the write completes instead of before. GatherWrite may
**	    not yet have written the page and we can't allow gfault_page()
** 	    to think the page exists until it does.
**	29-Jun-1999 (jenjo02)
**	    Added gw_queued, a pointer to this thread's count of
**	    pages queued for gather write, which is dynamically maintained 
**	    by DI.
**	09-Apr-2001 (jenjo02)
**	    Changed dm2f_write_list() prototype to include pointers
**	    to GatherWrite stats collectors, bs_gw_pages, bs_gw_io.
**	25-Mar-2004 (wanfr01)
**	    Bug 111353, INGSRV 2613
**          force_page should leave with buffer mutexes held, do not release
**          the mutex if you hit E_DM920E
**      29-Sep-2004 (fanch01)
**          Add LK_LOCAL flag support as cache names are confined to one node.
**	13-oct-2004 (devjo01)
**	    Do not ever use gather write if downgrading a cache lock to LK_N.
**	3-May-2005 (schka24)
**	    Make the return statuses even MORE complicated, to cater to the
**	    tricky case of hitting temp table disk full (ignoreable error)
**	    on a RECLAIM.  I've never seen this happen in the wild, but ...
*/
static DB_STATUS
force_page(
DM0P_BUFFER         *buffer,
DMP_TABLE_IO	    *tbio,
i4		    operation,
i4		    lock_list,
i4		    log_id,
i4		    evcomp,
i4		    *gw_queued,
i4             	    *err_code)
{
    DM0P_BUFFER	    *b = buffer;
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BMCB	    *bm = lbm->lbm_bmcb[b->buf_id.bid_cache];
    DM0P_BSTAT	    *lbm_stats = &lbm->lbm_stats[b->buf_id.bid_cache];
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DMPP_PAGE	    *page = (DMPP_PAGE *)((char *)lbm->lbm_pages[b->buf_id.bid_cache] +  
				b->buf_page_offset);
    LG_LSN	    lsn;
    LG_LSN	    new_lsn;
    CL_SYS_ERR      sys_err;
    i4	    i;
    i4         n;
    STATUS	    cl_status;
    DB_STATUS	    status = E_DB_OK;
    DB_STATUS	    retstatus;
    bool	    is_reclaim;
    i4	    action = 0;
#define			WRITE_PAGE	0x0001
#define			TRAN_PAGE	0x0002
#define			EMPTY_PAGE	0x0004
#define			UNLOCK_PAGE	0x0008
#define			CACHE_PAGE	0x0010
#define			REQUEUE_PAGE	0x0020
#define			LOCKED_PAGE	0x0040
#define			FREE_PAGE	0x0080
#define			SIGNAL_PAGE	0x0100
#define			CHANGE_LOCK	0x0200
#define			UNMODIFY_PAGE	0x0400
#define			PRIORITY_PAGE	0x0800	/* raise to highest priority */
#define                 NULL_LOCK       0x1000  /* retain with null lock */
#define                 SEND_PAGE       0x2000  /* Send page directly to
                                                   foreign BM */
    char	    *tbl_name,
		    *dbs_name;
    char	    tbl_buf[DB_MAXNAME];
    char	    dbs_buf[DB_MAXNAME];

    /*
    **   Clarification:
    **
    **	    force_page() and complete_f_p() will return with
    **	    buf_mutex unlocked unless a WARN or ERROR status is
    **	    returned, in which case buf_mutex will remain locked.
    */

    /*
    ** Check for operations in progress.
    ** If this buffer is already busy (being written by some other session),
    ** then wait for the other session to finish and return warning
    */
    if (b->buf_sts & BUF_BUSY)
    {
	*err_code = 0;

	/*
	** Need to force any I/O's which are being done by this thread
	** as a gather write (GW).  This prevents deadlock between GW's
	** such that they aren't each waiting for I/O's by the other
	** GW which haven't been forced out yet (and won't be because
	** they are waiting for each other).
	*/

	if ( gw_queued && *gw_queued )
	{
	    dm0p_munlock(&b->buf_mutex);
	    if ( (status = dm2f_force_list()) == E_DB_OK )
		status = E_DB_WARN;
	}
	else
	{
	    status = E_DB_WARN;
	    lbm_stats->bs_iowait[BMCB_PTYPES]++;
	    bm->bm_stats.bs_iowait[BMCB_PTYPES]++;
	    lbm_stats->bs_iowait[b->buf_type]++;
	    bm->bm_stats.bs_iowait[b->buf_type]++;
	    b->buf_sts |= BUF_IOWAIT;
	    dm0p_eset(lock_list, DM0P_BIO_EVENT, 
		    b->buf_id.bid_cache, b->buf_id.bid_index, 0);
	    dm0p_munlock(&b->buf_mutex);

	    /* Wait for the I/O event to complete. */
	    dm0p_ewait(lock_list, DM0P_BIO_EVENT,
		    b->buf_id.bid_cache, b->buf_id.bid_index, 0, 0);
	}
	/* Return with buffer locked */
	dm0p_mlock(SEM_EXCL, &b->buf_mutex);
	return(status);
    }

    if (b->buf_sts == BUF_FREE)
    {
	dm0p_munlock(&b->buf_mutex);
	return(E_DB_OK);
    }
    
    /* treat FP_WRITEALONG like FP_WRITE, but increase priority of buffer */
    /* so that it stays in buffer pool longer. If Write-along thread had  */
    /* not forced it, it would have stayed in buffer pool longer by       */
    /* virtue of being on the modified queue, so simulate that behavior   */
    if (operation == FP_WRITEALONG)
    {
	operation = FP_WRITE;
	action |= PRIORITY_PAGE;
    }

    /* Remember the RECLAIM flag for later on */

    is_reclaim = (b->buf_sts & BUF_RECLAIM) != 0;

    /*
    ** Determine the I/O and Lock actions required.
    ** If the page is modified, then it should be written.
    ** If locked and modified, the cache lock should be updated.
    ** If being tossed, the cache lock should be released.
    */
    if (evcomp == BMCL_DMCM_CBACK &&
        (bm_common->bmcb_status & BMCB_USE_CLM))
        action |= SEND_PAGE;

    if (b->buf_sts & BUF_MODIFIED)
	action |= WRITE_PAGE;

    if (b->buf_sts & BUF_LOCKED)
    {
	action |= LOCKED_PAGE;

	if (action & WRITE_PAGE)
	{
	    /*
	    ** (ICL phil.p)
	    ** Under the DMCM protocol, after writing a page
	    ** leave it cached with a null lock.
	    */
	    if (b->buf_sts & BUF_DMCM)
	    {
		if ((b->buf_sts & BUF_FIXED) == 0)
		    action |= NULL_LOCK;
	    }
	    else
		action |= CHANGE_LOCK;
	}

	if (operation == FP_TOSS)
	    action |= UNLOCK_PAGE;

	/*
	** (ICL phil.p)
	** A contending lock request was made of this page.
	** Drop lock to NULL and retain page in cache if unfixed,
	** else drop lock to SHARED.
	*/
	if (operation == FP_NULL)
	    action |= NULL_LOCK;
    }

    /*
    ** If the page is being invalidated, then don't write it and
    ** always update and release any cache locks.
    */
    if (operation == FP_INVALID)
    {
	action &= ~WRITE_PAGE;
	if (b->buf_sts & BUF_LOCKED)
	    action |= (LOCKED_PAGE | UNLOCK_PAGE | CHANGE_LOCK);
	b->buf_sts |= BUF_INVALID;
	b->buf_sts &= ~BUF_IOERROR;
    }

    if (operation == FP_DISOWN)
	action = 0;

    if (b->buf_sts & BUF_IOERROR)
    {
	*err_code = E_DM920E_BM_FORCE_PAGE_ERROR;
	return (E_DB_ERROR);
    }

    /*
    **	Execute I/O and Lock action steps.
    */
    b->buf_fp_action = action;
    b->buf_fp_llid = lock_list;

    if (action & (WRITE_PAGE | CHANGE_LOCK | UNLOCK_PAGE | NULL_LOCK))
    {
	/*  Mark buffer as busy. */

	b->buf_sts |= (BUF_WBUSY);

	if (action & WRITE_PAGE)
	{
	    lbm_stats->bs_writes[BMCB_PTYPES]++;
	    bm->bm_stats.bs_writes[BMCB_PTYPES]++;
	    lbm_stats->bs_writes[b->buf_type]++;
	    bm->bm_stats.bs_writes[b->buf_type]++;

	    /* Mark this buffer for GatherWrite, if available and wanted */
	    if ( lbm->lbm_status & LBM_GW_ENABLED &&
		 evcomp != BMCL_NO_GATHER &&
		 evcomp != BMCL_DMCM_CBACK &&
		 !(action & NULL_LOCK) )
		b->buf_sts |= BUF_GW_QUEUED;
	}

	/* Release the buffer's mutex */
	dm0p_munlock(&b->buf_mutex);

	/*
	** Save log address to which the log file is known to have last
	** been flushed.  This is used below to determine if a log force
	** is needed before writing the page.
	*/
	STRUCT_ASSIGN_MACRO(bm_common->bmcb_last_lsn, lsn);

#ifdef xDEBUG
	/*
	** Check for null tcb - added for debugging of CODEX errors in which
	** this case was occurring.
	*/
	if ((action & WRITE_PAGE) && (tbio == NULL))
	    TRdisplay("Error: Force_page called with null DMP_TABLE_IO: buf %x",
			b);

	if (DMZ_BUF_MACRO(2))
	{
	    if (tbio != NULL)
	    {
		TRdisplay("FORCE_PAGE  : (%~t,%~t,%d,%dK) action=%v.\n",
		    sizeof(DB_DB_NAME), tbio->tbio_dbname,
		    sizeof(DB_TAB_NAME), tbio->tbio_relid,
		    b->buf_lock_key.lk_key4,
		    KSIZE(bm->bm_pgsize),
		    "WRITE,KEEP,EMPTY,UNLOCK,CACHE,REQUEUE,LOCKED,FREE,SIGNAL,\
CHANGE,UNMODIFY,PRIORITY,NULL", action);
	    }
	    else
	    {
		TRdisplay(
		    "FORCE_PAGE:(db %d,tab(%d,%d),%d,%dK) action=%v.\n",
		    b->buf_dbid, b->buf_tabid.db_tab_base, 
		    b->buf_tabid.db_tab_index, b->buf_lock_key.lk_key4, 
		    KSIZE(bm->bm_pgsize),
		    "WRITE,KEEP,EMPTY,UNLOCK,CACHE,REQUEUE,LOCKED,FREE,SIGNAL,\
CHANGE,UNMODIFY,PRIORITY,NULL", action);
	    }
	}
#endif
	/* 
	** Force the log before writing the data page if the log record which
	** describes the last update to the page has not yet been forced.
	**
	** If we have multiple log files open then lsn refers to the least lsn
	** that we know has been forced out in all logs.
	**
	** Log forces are not needed to flush temporary table (which aren't
	** logged) or during Rollforward recovery (when the logfile isn't used).
	*/
	if ((action & WRITE_PAGE) &&
	    (b->buf_sts & BUF_TEMPTABLE) == 0 &&
	    (! (tbio->tbio_cache_flags & TBIO_CACHE_ROLLDB_RECOVERY)) &&
	    (LSN_GT(DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(b->buf_relpgtype, page), 
		    &lsn)))
	{
	    i4	lx_id;

	    /* If we have only one log then we only need to invoke dm0l_force */
            if (lbm->lbm_num_logs == 1)
	    {
		/*
		** If we have an active transaction, then use that for the log
		** force call - otherwise use the shared buffer manager context
		** (which must be used single-threadedly since LG won't like
		** multiple threads suspending on the same lg context).
		*/
		lx_id = log_id;
		if (lx_id == 0)
		{
		    lx_id = lbm->lbm_bm_lxid[0];
		    dm0p_mlock(SEM_EXCL, &lbm->lbm_lxid_mutex);
		}
	    
		status = dm0l_force(lx_id, 
		      DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(b->buf_relpgtype, page),
				    &lsn, err_code);
		if (status != E_DB_OK)
		{
		    if (*err_code > E_DM_INTERNAL)
		    {
			ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL,
			    (char *)0, (i4)0, (i4 *)0, err_code, 0);
			*err_code = E_DM920E_BM_FORCE_PAGE_ERROR;
		    }
		}

		if (lx_id == lbm->lbm_bm_lxid[0])
		    dm0p_munlock(&lbm->lbm_lxid_mutex);
	    }
	    else
	    {
		/*
		** We lock out all other users of the default log ids, we could
		** use a semaphore per lxid.
		*/
		dm0p_mlock(SEM_EXCL, &lbm->lbm_lxid_mutex);
		/*
		** Now we force all logs attached to this Buffer Manager up
		** to the page's LSN (currently stored in page_log_addr
		** We keep track of the minimum new lsn forced to update
		** our bm_last_lsn field to optimise future forces.
		*/
		for (i=0; i < lbm->lbm_num_logs; i++)
		{
		    lx_id = lbm->lbm_bm_lxid[i];
		    status = dm0l_force(lx_id, 
		      DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(b->buf_relpgtype, page),
					&new_lsn, err_code); 

		    if (status != E_DB_OK)
		    {
			if (*err_code > E_DM_INTERNAL)
			{
			    ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL,
				(char *)0, (i4)0, (i4 *)0, err_code, 0);
			    *err_code = E_DM920E_BM_FORCE_PAGE_ERROR;
			}
		    }
		    
		    if (i == 0 || LSN_GT(&lsn, &new_lsn))
		    {
			STRUCT_ASSIGN_MACRO(new_lsn, lsn);
		    }
			
		}

		dm0p_munlock(&lbm->lbm_lxid_mutex);
	    }

	    /*
	    ** Update the new guaranteed-flushed log address if it is greater than
	    ** the old one.
	    ** 
	    ** Use bmcb_cp_mutex to protect bmcb_last_lsn.
	    */
	    if (status == E_DB_OK &&
	        LSN_GT(&lsn, &bm_common->bmcb_last_lsn))
	    {
		dm0p_mlock(SEM_EXCL, &bm_common->bmcb_cp_mutex);
		if (LSN_GT(&lsn, &bm_common->bmcb_last_lsn))
		    STRUCT_ASSIGN_MACRO(lsn, bm_common->bmcb_last_lsn);
		dm0p_munlock(&bm_common->bmcb_cp_mutex);
	    }
	}

	/*
	** If the being forced appears to be beyond the file EOF then re-sense
	** the file since it is probably an indication that our table descriptor
	** is out-of-date.  If the page is still beyond the newly returned EOF
	** then return a write error.
	*/
	if ((status == E_DB_OK) && (action & WRITE_PAGE) &&
	    (b->buf_lock_key.lk_key4 > tbio->tbio_lalloc))
	{
	    i4	numpages;

	    /*
	    ** Since tbio_lalloc is stale, tbio_lpageno is likely also
	    ** stale.  Indicate to reread EOF information from the bitmaps
	    ** the next time the table is opened.
	    */
	    tbio->tbio_checkeof = TRUE;

	    status = dm2f_sense_file(tbio->tbio_location_array, 
		    tbio->tbio_loc_count,
		    tbio->tbio_relid, tbio->tbio_dbname,
		    &numpages, err_code);
	    if (status != E_DB_OK)
	    {
		if (*err_code > E_DM_INTERNAL)
		{
		    ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 0);
		    *err_code = E_DM920E_BM_FORCE_PAGE_ERROR;
		}
	    }
	    tbio->tbio_lalloc = numpages;

/* Bug #66815 - 6.4 fix (harpa06) */
	    if ((status == E_DB_OK) &&
	    	(b->buf_lock_key.lk_key4 > tbio->tbio_lalloc))
	    {
		*err_code = E_DM9202_BM_BAD_FILE_PAGE_ADDR; 
	    }
	}

	if (DMZ_SES_MACRO(10) && (action & WRITE_PAGE))
	    dmd_iotrace(DI_IO_WRITE,
		(DM_PAGENO)b->buf_lock_key.lk_key4, 1, tbio->tbio_relid, 
		&tbio->tbio_location_array[0].loc_fcb->fcb_filename,
		tbio->tbio_dbname);

        /*
        ** (ICL phil.p)
        ** For DMCM there is no need to change the cache lock mode.
        ** If we intend to write the page, then we will already have
        ** it locked at LK_X.
	** (CA jenjo02)
	** By now BUF_DMCM may be turned off, but the cache lock is
	** still LK_X. Check the cache lock strength instead of the flag.
        */

        if ((status == E_DB_OK) && (action & CHANGE_LOCK) 
	    && b->buf_lock_mode != LK_X)
	{
	    /*
	    ** Convert the cache page lock mode to SIX so that the lock value
	    ** of the cache page lock can be stored with the lock when
	    ** the cache page lock is converted to NULL mode or is released.
	    */

	    if (DMZ_LCK_MACRO(2))
		dmd_lock(&b->buf_lock_key, lbm->lbm_lock_list, LK_CONVERT,
		    (LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK),
		    LK_SIX, 
		    0, (DB_TRAN_ID *)0, (DB_TAB_NAME *)0, (DB_DB_NAME *)0);

	    cl_status = LKrequest(
		LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK,
		lbm->lbm_lock_list, (LK_LOCK_KEY *)0, LK_SIX,
		(LK_VALUE *)0, (LK_LKID *)&b->buf_lk_id, (i4)0, &sys_err);
	    if (cl_status != OK)
	    {
		if (tbio)
		{
		    tbl_name = (char *)tbio->tbio_relid;
		    dbs_name = (char *)tbio->tbio_dbname;
		}
		else
		{
		    tbl_name = (char *)STprintf(tbl_buf, "TABLE_ID (%d,%d)",
			    			b->buf_tabid.db_tab_base,
						b->buf_tabid.db_tab_index);
		    dbs_name = (char *)STprintf(dbs_buf, "DATABASE_ID %d",
						b->buf_dbid);
		}
                ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 0);
		ule_format(E_DM9021_BM_BAD_LOCK_PAGE, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 5,
		    0, b->buf_lock_key.lk_key4, 0, tbl_name, 0, dbs_name,
		    0, LK_SIX, 0, lbm->lbm_lock_list);
		*err_code = E_DM920E_BM_FORCE_PAGE_ERROR;
		status = E_DB_ERROR;
	    }
	    else
	    {
		b->buf_lock_mode = LK_SIX;
	    }
	}
    
	/* 
        ** Bump the lock value 
        ** (ICL phil.p)
        ** CHANGE_LOCK is no longer the only mode in which we're
        ** required to increment the value block. We may also be writing
        ** the page when doing a NULL_LOCK.
        */
	if (action & WRITE_PAGE)
	{
	    b->buf_oldvalue.lk_high++;
	    b->buf_oldvalue.lk_low =
	     DMPP_VPT_GET_LOG_ADDR_LOW_MACRO(b->buf_relpgtype, page);
	}

	if ((status == E_DB_OK) && (action & WRITE_PAGE))
	{
	    /*
	    ** Calculate new checksum for the page before writing
	    ** it to the database.
	    */
	    dm0p_insert_checksum(page, b->buf_relpgtype, (DM_PAGESIZE) bm->bm_pgsize);

	    n = 1;

	    /* Prepare structures for GatherWrite */
            if ( b->buf_sts & BUF_GW_QUEUED )
	    {
		char	     *dicb; 
		DM0P_CLOSURE *bmcl;
			
		/* DI's work area for this page */
		dicb = lbm->lbm_gw_dicb[b->buf_id.bid_cache]
			+ (b->buf_id.bid_index * bm_common->bmcb_gw_dicb_size);

		/* dm0p/dm2f work areas for this page */
		bmcl = 
		    &lbm->lbm_gw_closure
			    [b->buf_id.bid_cache][b->buf_id.bid_index];

		/* Save off closure information about this request */
		bmcl->bmcl_buf = b;
		bmcl->bmcl_tbio = tbio;
		bmcl->bmcl_pages = n;
		bmcl->bmcl_operation = operation;
		bmcl->bmcl_lock_list = lock_list;
		bmcl->bmcl_log_id = log_id;
		bmcl->bmcl_evcomp = evcomp;

		/*
		** Pin the page's TBIO to keep it from being closed
		** while the I/O is pending. It'll get unpinned
		** by complete_f_p().
		*/
		dm2t_pin_tabio_cb(tbio);

	        status = dm2f_write_list(&bmcl->bmcl_dm2f, dicb,
			tbio->tbio_location_array, 
			tbio->tbio_loc_count,
			tbio->tbio_page_size,
			tbio->tbio_relid, tbio->tbio_dbname, &n, 
			(i4)b->buf_lock_key.lk_key4, (char *)page,
			dm0p_complete, (PTR)bmcl, 
			gw_queued, 
			&lbm_stats->bs_gw_pages,
			&lbm_stats->bs_gw_io,
			err_code);

		if (status == E_DB_OK)
		{
		    /* 
		    ** GatherWrite succeeded.
		    ** DI assures us that this buffer has not
		    ** been written, though other queued writes
		    ** may have. Without this assurance, there's
		    ** no way to tell if complete_f_p() has been
		    ** called for this buffer, and we definitely
		    ** DON'T want to call complete_f_p() twice!
		    */
		    return(status);
		}
	    }
	    else
	    {
		status = dm2f_write_file(tbio->tbio_location_array, 
		    tbio->tbio_loc_count,
		    tbio->tbio_page_size,
		    tbio->tbio_relid, tbio->tbio_dbname, &n, 
		    (i4)b->buf_lock_key.lk_key4, (char *)page, err_code);
	    }
	}
    }

# if 0
    /*
    **  FIX_ME devjo01 10-01-2004
    **  Current usage of lk_high is incompatible with this.  As we don't
    **  have a fast messaging system implemented on Linux at the moment,
    **  this code is disabled pending resolution of this conflict.
    */
    if (action & SEND_PAGE)
    {
        STATUS  cx_status;
        i4      chit, msglength;

        /*
        ** Send out page for pickup other buffer managers in a
        ** clustered installation.
        */
	msglength = 4 * sizeof(i4) + bm->bm_pgsize;
        do
        {
            cx_status = CXmsg_stow(&chit, NULL, msglength);
            if (cx_status)
            {
                /*
                ** Not an essential operation.  If we could not
                ** stow this away for others to pick up, due to
                ** a resource shortage, skip this, and move on.
                */
                if (E_CL2C12_CX_E_INSMEM != cx_status)
                {
                    /* <INC> diags here. */
                    ;
                }
                break;
            }

            cx_status = CXmsg_append(chit, (PTR)&b->buf_lock_key.lk_key1,
                                     0, 4 * sizeof(i4), msglength);
            if (cx_status)
            {
                /* <INC> diags here. */
                break;
            }

            cx_status = CXmsg_append(chit, (PTR)page,
                                     4 * sizeof(i4), bm->bm_pgsize, msglength);
            if (cx_status)
            {
                /* <INC> diags here. */
                break;
            }
            b->buf_oldvalue.lk_high = chit;
            break;
        } while (0);
        if ( cx_status != OK )
            action &= ~SEND_PAGE;
    }
# endif

    retstatus = complete_f_p(status, tbio, b, 1, operation, log_id,
			lock_list, evcomp, err_code);
    /* Check for special case of invalidated RECLAIM page, return INFO.
    ** If we fed an error status into completion and got an OK back out,
    ** it must be a temp-table page that doesn't belong to us and we don't
    ** care what happened to it.  (See complete_f_p for more info.)
    */
    if (retstatus == E_DB_OK && status != E_DB_OK && is_reclaim)
	retstatus = E_DB_INFO;
    return (retstatus);
}

/*
** Name: gforce_page	- Handle special action on group force of buffer pages.
**
** Description:
**      This routine allows the caller to write a group of pages out.
**
** Inputs:
DM0P_BMCB	    *bm,
DM0P_GROUP	    *group,
i4		    lock_list,
i4		    log_id,
i4		    operation,
i4                  *err_code)
**	bm				Group's cache
**	group				The group to force.
**	lock_list			lock list
**	log_id				The logging system transaction id.
**	operation			The operation either:
**					    FP_TOSS - toss pages from cache
**					    FP_CACHE - keep pages but not owned
**                                          FP_NULL - keep pages not owned with
**                                                    null lock.
**					    FP_WRITE - keep pages but write
**					    FP_INVALID - don't write pages but
**						invalidate cache lock.
**					    FP_DISOWN - remove pages from tx queue
**
**		Currently only FP_TOSS and FP_WRITE are used, though
**	        the code supports the other operations.
**
** Outputs:
**      err_code                        The reason for the error status.
**	Returns:
**	    E_DB_OK
**	    E_DB_WARN		The group could not be forced for any
**				number of reasons.
**	    E_DB_FATAL
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	22-Jan-1987 (ac)
**	    Created for Jupiter.
**	10-feb-1989 (rogerk)
**	    Changed to not bump value locks or force file if no writes
**	    are being done.
**	28-feb-1989 (rogerk)
**	    Added support for Shared Buffer Manager.
**	    Added tcb argument.
**	 4-sep-1989 (rogerk)
**	    While pages are being written or locks released (when bm mutex
**	    is released), mark group buffer as busy so it won't be chosen
**	    for reclaimation by another thread.
**	17-oct-1991 (rogerk)
**	    Merged changes made by Derek during the file extend project.
**	    Added new handling for EOF checking : use tcb_lalloc field.
**	    Added new error message.
**      06-jan-92 (jnash)
**          Add support for > 32 gfault pages, up to DM_GFAULT_MAX_PAGES.
**      07-oct-92 (jnash)
**          6.5 reduced logging project.
**	     -  Insert page checksum.
**	26-oct-1992 (rogerk)
**	    Reduced logging project: changed references to tcb fields to
**	    use new tcb_table_io structure fields.
**	30-October-1992 (rmuth)
**	    - Set tcb_checkeof if we are trying to gforce past tbio_lalloc.
**	    - If necessary update tbio_tmp_hw_pageno.
**	14-dec-1992 (jnash)
**	    Fix bug inserting checksum on pages.
**	14-dec-1992 (jnash & rogerk)
**	    Removed DMPP_ACC_PLV argument from dm2f_write_file.
**	18-jan-1993 (rogerk)
**	    Removed the bypassing of log forces from the recovery process.
**	    Since CLR's are now written during UNDO processing, we must
**	    always make sure that the log file is correctly forced when
**	    forcing pages out of the buffer manager.
**	24-may-1993 (bryanp)
**	    If trace point DM302 is set, trace cache locking activity.
**	18-oct-1993 (rmuth)
**	    Add DMP_BMCB statistics.
**	22-feb-1994 (andys)
**	    Integrate recent changes from ingres63p.
**		11-jul-1993 (rogerk & andys)
**		    Update buf_cpcount when force out a page during a
**		    Consistency Point.  This guarantees that the page
**		    will be marked processed by the current CP even if
**		    the page is remodified by a server whose FC thread
**		    has already bypassed the page.
**		 1-nov-1993 (rogerk & andys)
**		    After a buffer has been written, and before
**		    clearing the buffer status by setting it to
**		    BUF_FREE, wake up any threads that are waiting  on
**		    this buffer. [bug 54128]
**	18-apr-1994 (jnash)
**	    fsync project.  Eliminate dm2f_force_file() calls.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	20-mar-1996 (nanpr01)
**	    Added page size as parameter to this routine. To compute
**	    the checksum pagesize is need since sizeof(*page) returns
**	    only the size of physical page.
**      01-aug-1996 (nanpr01 for ICL phil.p)
**          For the Distributed Multi-Cache Management (DMCM) protocol,
**          when writing a page we do not need to convert the cache lock.
**          It will already be LK_X.
**	    Added the gather write and async io amendments.
**	15-Sep-1996 (jenjo02)
**	    Caller must hold group's g_mutex and one of its buffer's mutexes..
**	    Added buffer parm to prototype. Caller must specify
**	    which buffer in the group is currently held with its
**	    buf_mutex locked. This buf_mutex, and the group's mutex,
**	    will be left locked  when we return.
**	07-Nov-1996 (jenjo02)
**	    If group is busy, do nothing but return E_DB_WARN.
**	22-Nov-1996 (jenjo02)
**	    In gforce_page(), when waiting for a busy buffer, make sure that
**	    all buffers in the group have their mutexes released.
**	02-Dec-1996 (jenjo02)
**	    Replaced line dropped by previous integration.
**	05-Jan-1997 (jenjo02)
**	    Added new event wait type, DM0P_GWAIT_EVENT.
**	    If group is busy, wait for it to become unbusy,
**	    then return a warning.
**	13-Jan-1997 (jenjo02)
**	    Update bmcb_last_lsn as soon as we know its new value.
**	13-Mar-1997 (jenjo02)
**	  - Decrement g_modcount by the number of buffers actually written
**	    instead of blindly setting it to zero.
**	  - Watch out for FIXED buffers that may appear in the group and
**	    don't mark them freed. dm0p_cachefix_page() may add fixed
**	    buffers to this group while we are forcing modified pages.
**	08-apr-1997 (nanpr01)
**	    Corrected Gather write dm2f_write_list parameter list.
**	11-Apr-1997 (jenjo02)
**	    Instead of relying on BUF_DMCM flag to decide whether to
**	    upgrade a lock to SIX, check buf_lock_mode instead.
**	15-Apr-1997 (jenjo02)
**	    Wait for mutexed pages to be released, return warning.
**	    Mutex pages while being forced to prevent modification by 
**	    other threads.
**	28-Aug-1997 (jenjo02)
**	    Redid group-busy logic, in general, lock group, set busy status, 
**	    unlock group, then lock buffer.
**	19-Jan-1998 (jenjo02)
**	    Remove buffer from table's hash queue when buffer is made free.
**	10-Jun-1998 (jenjo02)
**	    Make caller responsible for awakening group waiters upon return
**	    from this function.
**	29-Jun-1999 (jenjo02)
**	    Added gw_queued, a pointer to this thread's count of
**	    pages queued for gather write, which is dynamically maintained 
**	    by DI.
**	10-Jan-2000 (jenjo02)
**	    Rewrote for group priority queues. Make group forces more like
**	    single-page forces by only tossing the group and its pages 
**	    when requested (operation == FP_TOSS), otherwise write the
**	    modified pages (operation == FP_WRITE), move the group to the 
**	    free queue, but leave the pages cached.
**	14-Jun-2000 (jenjo02)
**	    Simplified interface. Caller need only mutex the group before
**	    calling here, not the buffer. This function will be responsible
**	    for fixing and unfixing any needed TBIO, marking the group
**	    "BUSY" and awakening waiters when appropriate.
**	    This function will never wait for an event (dm0p_ewait()),
**	    so *gw_queued is no longer a needed input parameter.
**      18-Sep-2003 (jenjo02) SIR 110923
**          Modified to utilize GatherWrite when available,
**          "complete_f_p()" to eliminate duplicate post-force
**          buffer and page handling.
**	10-Nov-2003 (jenjo02)
**	    Tweak to last change: can't use GatherWrite if group
**	    spans multiple locations.
**	24-Jan-2005 (jenjo02)
**	    bs_gwrites[BMCB_PTYPES] reverted to counting one write
**	    for each actual write I/O.
*/
static DB_STATUS
gforce_page(
DM0P_BMCB	    *bm,
DM0P_GROUP	    *group,
i4		    lock_list,
i4		    log_id,
i4		    operation,
i4		    evcomp,
i4		    *gw_queued,
i4                  *err_code)
{
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BSTAT	    *lbm_stats = &lbm->lbm_stats[bm->bm_cache_ix];
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BUFFER	    *buf_array = lbm->lbm_buffers[bm->bm_cache_ix];
    DB_STATUS	    status = E_DB_OK;
    STATUS	    cl_status;
    DM0P_GROUP	    *g = group;
    DM0P_BUFFER	    *last_buffer;
    DM0P_BUFFER	    *b;
    DMPP_PAGE	    *page;
    DMP_TABLE_IO    *tbio = (DMP_TABLE_IO*)NULL;
    LG_LSN	    *lsn = NULL;
    LG_LSN	    last_lsn;
    LG_LSN	    new_lsn;
    i4	    	    i;
    i4	    	    j;
    i4	    	    n;
    i4	    	    event_type;
    CL_SYS_ERR      sys_err;
    i4		    pages;

    /* Action defines as as for force_page() */
    i4		    g_action, action;

    i4		    use_gw;

    /*
    ** Group's mutex must be held by caller. This function 
    ** will be responsible for marking the group write-busy and
    ** awakening group waiters, if any,
    ** and will return with the group marked unbusy and
    ** the group's mutex locked.
    ** 
    ** It will also fix and unfix any TBIO needed.
    **
    ** If the group cannot be forced for any reason, 
    ** E_DB_WARN will be returned.
    */

    /*
    ** If no valid pages in this group (all buffers are BUF_FREE), 
    ** we've nothing to do.  (Note that g_pages will also be zero.)
    */
    if ( g->g_valcount == 0 )
	return(E_DB_OK);

    /* We should never be called with a busy group, but check */
    if ( g->g_sts & G_BUSY )
	return(E_DB_WARN);
	
    /* If the group contains fixed buffers, the group can't be freed */
    if ( g->g_refcount )
	return(E_DB_WARN);

    /* Mark the group as write-busy and release its mutex */
    g->g_sts |= G_WBUSY;
    g->g_lock_list = lock_list;
    dm0p_munlock(&g->g_mutex);

    /*
    **  Lock each of the buffers in the group and check
    **  their status; if any of the group's buffers are 
    **  busy or mutexed, we can't force the group 
    **  - unlock all locked buffers and return E_DB_WARN.
    **
    **  There's no apparent reason to wait on a busy buffer
    **  rather than just returning a warning.
    */
    for (i = 0; i < g->g_pages; i++)
    {
	b = &buf_array[g->g_buffer + i];

	dm0p_mlock(SEM_EXCL, &b->buf_mutex);

	if ( b->buf_sts & (BUF_BUSY | BUF_MUTEX) )
	{
	    /* Unlock all the buffers locked up to this point */
	    for ( i; i >= 0; i-- )
	    {
		b = &buf_array[g->g_buffer + i];
		dm0p_munlock(&b->buf_mutex); 
	    }

	    /* Unbusy the group, wake up waiters, return E_DB_WARN. */
	    dm0p_mlock(SEM_EXCL, &g->g_mutex);
	    if (g->g_sts & G_WAIT)
	    {
		dm0p_erelease(lock_list, DM0P_GWAIT_EVENT,
				bm->bm_cache_ix, g->g_index, 0);
	    }
	    g->g_sts &= ~(G_WBUSY | G_WAIT);
	    g->g_lock_list = 0;

	    /* return with group mutex locked */

	    return( E_DB_WARN );
	}
    }

    /*
    ** If there are modified pages in the group
    ** look for a Table Descriptor with which to 
    ** write out this group.  If we don't find one, 
    ** return a warning. If there are no modified
    ** pages, we shouldn't need a tbio.
    */

    if ( g->g_modcount )
    {
	b = &buf_array[g->g_buffer];

	status = fix_tableio_ptr(b, 
		(i4)DM0P_TBIO_NOPARTIAL, lock_list, log_id,
		&tbio, err_code);

	if ( status )
	{
	    /*
	    ** Table Descriptor not found.  If an error was 
	    ** returned, just log it.
	    **
	    ** In all cases, we can't force the group, so
	    ** return a warning.
	    */
	    if (DB_FAILURE_MACRO(status))
	    {
		ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG,
		    NULL, (char *)0, (i4)0, (i4 *)0,
		    err_code, 0);
		ule_format(E_DM9C80_ERROR_FINDING_TABIO_PTR, 
		    (CL_SYS_ERR *)0, ULE_LOG, NULL, (char *)0,
		    (i4)0, (i4 *)0, err_code, 0);
	    }
	    
	    /* Unlock all of the buffers */
	    for (i = 0; i < g->g_pages; i++)
	    {
		b = &buf_array[g->g_buffer + i];
		dm0p_munlock(&b->buf_mutex); 
	    }

	    /* Unbusy the group, wake up waiters, return warning */
	    dm0p_mlock(SEM_EXCL, &g->g_mutex);
	    if (g->g_sts & G_WAIT)
	    {
		dm0p_erelease(lock_list, DM0P_GWAIT_EVENT,
				bm->bm_cache_ix, g->g_index, 0);
	    }
	    g->g_sts &= ~(G_WBUSY | G_WAIT);
	    g->g_lock_list = 0;

	    return( E_DB_WARN );
	}
    }

    /*	Compute the actions needed for the group of buffers. */
    g_action = 0;
    pages = 0;

    /* Use GatherWrite if wanted and available */
    use_gw = (lbm->lbm_status & LBM_GW_ENABLED &&
		 evcomp != BMCL_NO_GATHER &&
		 evcomp != BMCL_DMCM_CBACK &&
		 tbio && tbio->tbio_loc_count == 1);

    /*
    ** Mark all the buffers we're interested in as busy and mutexed,
    ** and release all buffer mutexes.
    */
    
    for (i = 0; i < g->g_pages; i++)
    {
	b = &buf_array[g->g_buffer + i];

	/* Clear the action needed on this buffer */
	action = 0;

	/* Ignore free and fixed buffer */
	if ( b->buf_sts != BUF_FREE && (b->buf_sts & BUF_FIXED) == 0 )
	{
	    /* If buffer is modified, it must be written */
	    if ( b->buf_sts & BUF_MODIFIED )
	    {
		action |= WRITE_PAGE;
		last_buffer = b;

		/*  Compute maximum log address for forcing log file. */

		page = (DMPP_PAGE *)((char *)lbm->lbm_pages[bm->bm_cache_ix] + 
				b->buf_page_offset);
		if ((lsn == 0) || 
		    (LSN_GT(DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(b->buf_relpgtype, page), 
			    lsn)))
		{
		    lsn = DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(b->buf_relpgtype, page);
		}
	    }
	    
	    if ( b->buf_sts & BUF_LOCKED )
	    {
		action |= LOCKED_PAGE;

		if ( action & WRITE_PAGE )
		{
		    if ( b->buf_sts & BUF_DMCM )
			action |= NULL_LOCK;
		    else
			action |= CHANGE_LOCK;
		}
	    }

	    /*
	    ** Determine the action required to dispose of the buffer
	    ** after the write and lock actions complete:
	    **   - If the buffer has been written (it is not fixed) 
	    **	   then the group modified count will be decremented.
	    **   - If it being tossed, then it should be taken off the buffer hash
	    **     queue and the group valid page count decremented.
	    **   - If the page belonged to a transaction which is no longer active, then
	    **     it should be take off that transaction's buffer list (TRAN_PAGE).
	    */
	    if ( operation == FP_WRITE )
	    {
		if ( action & WRITE_PAGE )
		    action |= FREE_PAGE;
	    }
	    else if ( operation == FP_TOSS )
	    {
		action |= (FREE_PAGE | EMPTY_PAGE | TRAN_PAGE);
		if ( b->buf_sts & BUF_LOCKED )
		    action |= UNLOCK_PAGE;
	    }
	    else if ( operation == FP_CACHE )
	    {
		action |= TRAN_PAGE;
		if (action & WRITE_PAGE)
		    action |= FREE_PAGE;
	    }
	    else if ( operation == FP_INVALID )
	    {
		/*
		** If the group is being invalidated, then don't write it 
		** and always update and release any cache locks.
		*/
		action &= ~WRITE_PAGE;
		if ( b->buf_sts & BUF_LOCKED )
		    action |= (LOCKED_PAGE | UNLOCK_PAGE | CHANGE_LOCK);
		b->buf_sts |= BUF_INVALID;
		b->buf_sts &= ~BUF_IOERROR;
		action |= (FREE_PAGE | EMPTY_PAGE | TRAN_PAGE);
		if ( b->buf_sts & BUF_MODIFIED )
		    action |= UNMODIFY_PAGE;
	    }
	    else if ( operation == FP_DISOWN )
	    {
		action |= TRAN_PAGE;
	    }
	    else if ( operation == FP_NULL )
	    {
		action |= TRAN_PAGE;
		if ( action & WRITE_PAGE )
		    action |= FREE_PAGE;
		if ( b->buf_sts & BUF_LOCKED )
		    action |= NULL_LOCK;
	    }
	}

	/* Update group action, set buffer action */
	g_action |= action;

	/*
	** If anything needs to be done to this buffer, mark it busy,
	** keep track of last group buffer of interest.
	**
	** Stow this buffer's "action" and thread's lock_list id
	** in each buffer. This later identifies which buffers
	** we've selected and now "own" for the force.
	*/
	if ( action )
	{
	    b->buf_fp_action = action;
	    b->buf_fp_llid = lock_list;
	    b->buf_sts |= BUF_WBUSY;
	    pages = i+1;
	}

	dm0p_munlock(&b->buf_mutex);
    }

    /*	See if any write or lock action is needed on this group. */

    /* We've release g_mutex and all of its buf_mutex'es */

    if ( g_action )
    {
	if ( g_action & WRITE_PAGE )
	{
	    STRUCT_ASSIGN_MACRO(bm_common->bmcb_last_lsn, last_lsn);

	    /* 
	    ** Force the log before writing the data page if the log record which
	    ** describes the last update to the page has not yet been forced.
	    **
	    ** If we have multiple logs we must update the bms last_lsn to be the
	    ** minimum lsn forced out of all logs.
	    **
	    ** Log forces are not needed to flush temporary table (which aren't
	    ** logged) or during Rollforward recovery (when the logfile isn't used).
	    */
	    if ((! tbio->tbio_temporary) &&
		(! (tbio->tbio_cache_flags & TBIO_CACHE_ROLLDB_RECOVERY)) &&
		(lsn && LSN_GT(lsn, &last_lsn)) )
	    {

		if (lbm->lbm_num_logs == 1)
		{
		    /* Default case. Note that this code assumes we always have
		    ** a valid log_id, the corresponding code in force_pages doesn't
		    */
		    status = dm0l_force(log_id, lsn, &last_lsn, err_code);
		    if (status != E_DB_OK)
		    {
			if (*err_code > E_DM_INTERNAL)
			    *err_code = E_DM920E_BM_FORCE_PAGE_ERROR;
		    }
		}
		else
		{
		    /* Since we are forcing multi-logs we ignore the log_id that
		    ** was passed in and use the default contexts from lbm
		    */

		    for (j=0; j < lbm->lbm_num_logs; j++)
		    {
			i4 lx_id = lbm->lbm_bm_lxid[j];
			status = dm0l_force(lx_id, lsn, &new_lsn, err_code);
			if (status != E_DB_OK)
			{
			    if (*err_code > E_DM_INTERNAL)
				*err_code = E_DM920E_BM_FORCE_PAGE_ERROR;
			}
			if (j == 0 || LSN_GT(&last_lsn, &new_lsn))
			{
			    STRUCT_ASSIGN_MACRO(new_lsn, last_lsn);
			}
		    }
		}

		/*
		** Update the new guaranteed-flushed log address if it is greater than
		** the old one.
		** 
		** Use bmcb_cp_mutex to protect bmcb_last_lsn.
		*/
		if (status == E_DB_OK &&
		    LSN_GT(&last_lsn, &bm_common->bmcb_last_lsn))
		{
		    dm0p_mlock(SEM_EXCL, &bm_common->bmcb_cp_mutex);
		    if (LSN_GT(&last_lsn, &bm_common->bmcb_last_lsn))
			STRUCT_ASSIGN_MACRO(last_lsn, bm_common->bmcb_last_lsn);
		    dm0p_munlock(&bm_common->bmcb_cp_mutex);
		}
	    }

	    /*
	    **  Check that the page addresses are within the physical bounds
	    **  of the file.
	    */

	    if ( status == E_DB_OK && 
		last_buffer->buf_lock_key.lk_key4 > tbio->tbio_lalloc )
	    {
		i4	numpages;

		status = dm2f_sense_file(tbio->tbio_location_array, 
			    tbio->tbio_loc_count, 
			    tbio->tbio_relid, tbio->tbio_dbname, 
			    &numpages, err_code);
		if (status != E_DB_OK)
		{
		    if (*err_code > E_DM_INTERNAL)
		    {
			ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL, 
			    (char *)0, (i4)0, (i4 *)0, err_code, 0);
			*err_code = E_DM920F_BM_GROUP_FORCEPAGE_ERR;
		    }
		}
		tbio->tbio_lalloc = numpages;

		if (status == E_DB_OK &&
		    last_buffer->buf_lock_key.lk_key4 > tbio->tbio_lalloc)
		{
		    ule_format(E_DM93A9_BM_FORCE_PAGE_EOF, (CL_SYS_ERR *)0, 
			ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, 
			err_code, 6,
			sizeof(DB_DB_NAME), tbio->tbio_dbname,
			sizeof(DB_TAB_NAME), tbio->tbio_relid,
			sizeof(DB_OWN_NAME), tbio->tbio_relowner,
			0, last_buffer->buf_lock_key.lk_key4,
			0, g->g_pages, 0, tbio->tbio_lalloc);
		    status = E_DB_ERROR;
		    *err_code = E_DM9202_BM_BAD_FILE_PAGE_ADDR;
		}
	    }
	}

	/*  
	** Change lock to SIX so we can signal a page change. 
	**
	** (ICL phil.p)
	** This is not necessary when running DMCM. If a page
	** has been modified, it will already be locked LK_X, 
	** thus enabling us to write. Therefore, in this case
	** we can skip the conversion.
	*/

	if ( status == E_DB_OK && g_action & CHANGE_LOCK )
	{
	    for (i = 0; i < pages; i++)
	    {
		/*
		** If page has no cache lock or if the page is not
		** being written, then don't need to bump the lock.
		*/
		b = &buf_array[g->g_buffer + i];
		if ( b->buf_fp_llid == lock_list &&
		     b->buf_fp_action & CHANGE_LOCK &&
		     b->buf_lock_mode != LK_X )
		{
		    /*
		    ** (ICL phil.p)
		    ** Skip lock conversion for DMCM protocol
		    **
		    ** (CA jon.j)
		    ** Instead, skip lock conversion if already LK_X;
		    ** BUF_DMCM may have been turned off.
		    */
		    cl_status = LKrequest( LK_PHYSICAL | LK_CONVERT 
			| LK_NODEADLOCK, lbm->lbm_lock_list, 
			(LK_LOCK_KEY *)0, LK_SIX, (LK_VALUE *)0,
			(LK_LKID *)&b->buf_lk_id, (i4)0, &sys_err);

		    if (cl_status != OK)
		    {
		       ule_format(cl_status, &sys_err, ULE_LOG, NULL, 
			 (char *)0, (i4)0, (i4 *)0, err_code, 0);
		       ule_format(E_DM9021_BM_BAD_LOCK_PAGE, &sys_err, ULE_LOG, 
			 NULL, (char *)0, (i4)0, (i4 *)0, err_code, 
			 5, 0, b->buf_lock_key.lk_key4, sizeof(DB_TAB_NAME), 
			 tbio->tbio_relid, sizeof(DB_DB_NAME), 
			 tbio->tbio_dbname, 0, LK_SIX, 0, lbm->lbm_lock_list);
			*err_code = E_DM920E_BM_FORCE_PAGE_ERROR;
			status = E_DB_ERROR;
		    }
		    else
			b->buf_lock_mode = LK_SIX;
		}
	    }
	}

	/*
	** Write all pages that need to be written,
	** post-process each page through complete_f_p()
	*/

	for (i = 0; i < pages; i++)
	{
	    /*
	    ** Find the first modified page in the group (or the next
	    ** one if this is not the first time through this loop).
	    ** We will start our multi-page write from this spot.
	    */
	    b = &buf_array[g->g_buffer + i];
	    if ( b->buf_fp_llid != lock_list || b->buf_fp_action == 0 )
		continue;
	    
	    if ( status == E_DB_OK && b->buf_fp_action & WRITE_PAGE )
	    {

		/* 
		** Look for non-dirty pages in the group.  We will write
		** either up to the next non-dirty page or up to the end
		** of the group (which ever comes first.
		*/
		for ( n = i; n < pages; n++ )
		{
		    last_buffer = &buf_array[g->g_buffer + n];
		    if ( last_buffer->buf_fp_llid == lock_list &&
			 last_buffer->buf_fp_action & WRITE_PAGE )
		    {
			page = (DMPP_PAGE *)((char *)
			 lbm->lbm_pages[bm->bm_cache_ix] +
			 last_buffer->buf_page_offset);

			/* Checksum each written page */
			dm0p_insert_checksum( page,
			    last_buffer->buf_relpgtype,
			    (DM_PAGESIZE) bm->bm_pgsize);

			/* Increment group page write stats by page type */
			lbm_stats->bs_gwrites[last_buffer->buf_type]++;
			bm->bm_stats.bs_gwrites[last_buffer->buf_type]++;
			/* Bump the lock value when writing the page */
			last_buffer->buf_oldvalue.lk_high++;
			last_buffer->buf_oldvalue.lk_low =
			 DMPP_VPT_GET_LOG_ADDR_LOW_MACRO(b->buf_relpgtype, \
			  page);
		    }
		    else
			break;
		}

		page = (DMPP_PAGE *)((char *)lbm->lbm_pages[bm->bm_cache_ix] + 
				b->buf_page_offset);

		/* "n" is number of pages to write */
		n = n - i;

		if (DMZ_SES_MACRO(10))
		{
		    dmd_iotrace(DI_IO_WRITE, 
			(DM_PAGENO)b->buf_lock_key.lk_key4, n,
			tbio->tbio_relid, 
			&tbio->tbio_location_array[0].loc_fcb->fcb_filename,
			tbio->tbio_dbname);
		}
	
		/* Count another Group Write */
		lbm_stats->bs_gwrites[BMCB_PTYPES]++;
		bm->bm_stats.bs_gwrites[BMCB_PTYPES]++;

		if ( use_gw )
		{
		    /* GatherWrite this group of pages */
		    char	     *dicb; 
		    DM0P_CLOSURE     *bmcl;
			    
		    /* DI's work area for these pages */
		    dicb = lbm->lbm_gw_dicb[b->buf_id.bid_cache]
			    + (b->buf_id.bid_index * bm_common->bmcb_gw_dicb_size);

		    /* dm0p/dm2f work areas for these pages */
		    bmcl = 
			&lbm->lbm_gw_closure
				[b->buf_id.bid_cache][b->buf_id.bid_index];

		    /* Save off closure information about this request */
		    bmcl->bmcl_buf = b;
		    bmcl->bmcl_tbio = tbio;
		    bmcl->bmcl_operation = operation;
		    bmcl->bmcl_lock_list = lock_list;
		    bmcl->bmcl_log_id = log_id;
		    bmcl->bmcl_evcomp = evcomp;
		    bmcl->bmcl_pages = n;

		    /*
		    ** Pin the page's TBIO to keep it from being closed
		    ** while the I/O is pending. It'll get unpinned
		    ** by complete_f_p().
		    */
		    dm0p_mlock(SEM_EXCL, &b->buf_mutex);
		    b->buf_sts |= BUF_GW_QUEUED;
		    dm0p_munlock(&b->buf_mutex);
		    dm2t_pin_tabio_cb(tbio);

		    status = dm2f_write_list(&bmcl->bmcl_dm2f, dicb,
			    tbio->tbio_location_array, 
			    tbio->tbio_loc_count,
			    tbio->tbio_page_size,
			    tbio->tbio_relid, tbio->tbio_dbname, &n, 
			    (i4)b->buf_lock_key.lk_key4, (char *)page,
			    dm0p_complete, (PTR)bmcl, 
			    gw_queued, 
			    &lbm_stats->bs_gw_pages,
			    &lbm_stats->bs_gw_io,
			    err_code);

		    if ( status )
		    {
			/* Force queued writes through complete_f_p() */
			if ( *gw_queued )
			    (VOID)dm2f_force_list();
		    }
		    else
		    {
			/* 
			** GatherWrite succeeded.
			** DI assures us that this page has not
			** been written, though other queued writes
			** may have. Without this assurance, there's
			** no way to tell if complete_f_p() has been
			** called for this buffer, and we definitely
			** DON'T want to call complete_f_p() twice!
			*/
		    }
		}
		else
		{
		    status = dm2f_write_file(tbio->tbio_location_array,
			tbio->tbio_loc_count, tbio->tbio_page_size,
			tbio->tbio_relid,
			tbio->tbio_dbname, &n,
			(i4)b->buf_lock_key.lk_key4,
			(char *)page, err_code);

		    status = complete_f_p(status, tbio, b, n, operation, log_id,
			    lock_list, evcomp, err_code);
		    if ( status )
			dm0p_munlock(&b->buf_mutex);
		}

		i += n - 1;
	    }
	    else
	    {
		/*
		** complete_f_p() expects buf_mutex held if these
		** flags are off.
		*/
		if ( (b->buf_fp_action & (WRITE_PAGE | CHANGE_LOCK | 
				       UNLOCK_PAGE | NULL_LOCK)) == 0 )
		{
		    dm0p_mlock(SEM_EXCL, &b->buf_mutex);
		}
		status = complete_f_p(status, tbio, b, 1, operation, log_id,
			    lock_list, evcomp, err_code);
		if ( status )
		    dm0p_munlock(&b->buf_mutex);
	    }
	}

	if (status != E_DB_OK)
	{
	    if (*err_code > E_DM_INTERNAL)
	    {
		ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL, (char *)0,
		    (i4)0, (i4 *)0, err_code, 0);
		*err_code = E_DM920F_BM_GROUP_FORCEPAGE_ERR;
	    }
	}
    } /* if ( g_action ) */

    /* Unfix the tbio if we fixed it */
    if ( tbio )
	unfix_tableio_ptr( &tbio, lock_list, (i4)0, (i4)__LINE__ );

    /*
    ** Relock the group.
    ** 
    ** Wake up any group waiters and unbusy
    ** the group.
    */
    dm0p_mlock(SEM_EXCL, &g->g_mutex);

    /* Wake up anyone waiting on this group */
    if (g->g_sts & G_WAIT)
    {
	dm0p_erelease(lock_list, DM0P_GWAIT_EVENT,
			bm->bm_cache_ix, g->g_index, 0);
    }
    g->g_sts &= ~(G_WBUSY | G_WAIT | G_SKIPPED);
    g->g_lock_list = 0;

    if (DMZ_BUF_MACRO(15))
	    (VOID) dm0p_check_consistency(4);

    /* Return with group mutex locked */
    return( status );
}

/*{
** Name: dm0p_close_page	- Write pages to disk so file can be closed.
**
** Description:
**	This routine is used to flush pages for a specified table from the
**	cache - generally so that the table can then be physically closed.
**	All modified pages belonging to the table will be forced to disk.
**
**	It may also be used by routines which require the table's modifications
**	to be reflected in the physical database so that read operations can
**	be done which bypass the Buffer Manager - Smart Disk hardware for
**	example.
**
**	Mode DM0P_CLCACHE:
**
**	   If the action DM0P_CLCACHE is specified, then the pages
**	   associated with the table are left in the cache after being
**	   written.  The next time the table is opened, we must verify
**	   that the cached pages are still valid.
**
**	   This routine does not require that the caller guarantee any
**	   exclusive access to the table being forced.  It waits for
**	   any mutexed pages and leaves fixed pages in their fixed
**	   state.  Although since this mode is used in conjunction with
**	   tossing TCB's it is believed to be impossible to find any
**	   fixed or mutexed pages unless a shared cache is in use.
**
**	Mode DM0P_CLFORCE:
**
**	   If the action DM0P_CLFORCE is specified, then the pages
**	   associated with the table are written even if currently
**	   fixed by other threads.  (They will not, however, be written
**	   if mutexed).  The pages are left in the cache after being
**	   written.  This mode is used to prepare for a Smart Disk scan
**	   which operation bypasses the cache but needs to work on the
**	   most up-to-date page copies.
**
**	   This routine does not require that the caller guarantee any
**	   exclusive access to the table being forced. It waits for any
**	   mutexed pages and leaves fixed pages in their fixed state.
**	   Since it can be called at any time, it is expected that
**	   fixed and/or mutexed pages may be found.
**
**	Mode DM0P_CLOSE:
**
**	   If the action DM0P_CLOSE is specified, then all pages
**	   associated with the table are tossed from the cache.
**
**	   This mode is used when a table purge operation is done
**	   requiring all pages associated with the old version of the
**	   table to be written and tossed.
**
**	   It is assumed that the caller holds both an exclusive table
**	   lock and an exclusive table control lock.  No pages
**	   belonging to the table should be fixed in the cache and only
**	   background cache threads can have pages temporarily
**	   mutexed.
**
**	Mode 0:
**
**	   If no action is specified (0), then modified pages for this
**	   table are not written, they are just tossed from the cache.
**	   This should only be used with temporary tables (or tables
**	   that the caller can be sure have no fastcommit changes left
**	   around in the buffer manager).
**
**	   It is assumed that since the table being affected is
**	   temporary that its access is restricted to the calling
**	   session. No pages belonging to the table should be fixed in
**	   the cache and only background cache threads can have pages
**	   temporarily mutexed.
**
**	This routine may be called for one of three reasons:
**
**	    A)	The table is being physically closed by the server as
**		part of a database close, or to make room for another
**		table to be opened.
**
**		In this case all dirty pages must be forced to disk
**		since we cannot guarantee after releasing the TCB that
**		we will later be able to write the pages.
**
**		Generally, the caller will specify the DM0P_CLCACHE action
**		since the table may later be re-opened and we may benefit
**		by leaving them cached.
**
**	    B)	An operation which will invalidate all cached pages for
**		the table is about to occur.  This may be a modify/patch
**		or some other operation which will bypass the buffer manager.
**
**		The operation will usually be followed by a physical close
**		of the table (dm2t_close) since the TCB will probably be
**		invalidated as well.
**
**		In this case we must toss all pages from the cache so that
**		we do not attempt to reuse them later.  The caller should
**		specify the DM0P_CLOSE action.
**
**	    C)	An operation is going to be requested which will read pages
**		while bypassing the buffer manager.  This could be a Smart
**		Disk scan or a Verify operation.
**
**		In this case we must force all modified pages to disk so
**		the cache-bypassing operation can see the correct data.
**		The operation may leave unmodified pages in the cache.
**
**
**	If called with a base table TCB, this routine will act on all pages
**	belonging to secondary indexes as well as the base table.  If called
**	with a secondary index TCB, the routine will act only on pages
**	belonging to that specific secondary index.
**
**	Note that due to multi-server protocols, the table specified may
**	be open and in use by a thread of another server using this same
**	buffer manager.  This routine does not prevent another server from
**	reading in or modifying a page after we have acted on it.  For this
**	reason, this routine cannot guarantee by itself that on return there
**	will be no (modified) pages for the table left in the cache.
**
**	If the caller requires (as in cases B, C above) that no (modified)
**	pages be left on return, then the caller must request an exclusive
**	table lock prior to the dm0p_close_page call.
**
**	If the caller is merely closing the table (case A) then the caller
**	needs only guarantee that there can be no modified pages left if
**	no other server has an open TCB for the table.  This will be
**	guaranteed.
**
**	Any pages that are currently fixed or in use by another server are
**	skipped by this routine.
**
**	In order to give better concurrency, the buffer manager is searched
**	without the buffer manager mutex.  When a page to force is encountered,
**	the semaphore is requested and the buffer must then be checked again to
**	make sure that it still references the same page.
**
**
** Inputs:
**      tcb		The TCB describing table for which pages are forced:
**			    If base table, all secondary index pages are forced
**			    as well.
**	lock_list	Lock list used to synchronize I/O.
**	action		Either:
**			    DM0P_CLOSE - write changed pages and toss from cache
**			    DM0P_CLCACHE - write changed pages but keep in cache
**			    DM0P_CLFORCE - Not closing TCB so as CLCACHE but
**					 allowed to have FIXED pages.
**			    0 - don't write changed pages, just toss
**
** Outputs:
**      err_code        The reason for error status.
**	Returns:
**	    E_DB_OK
**	    E_DB_FATAL
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	15-oct-1985 (derek)
**          Created new for Jupiter.
**	28-feb-1989 (rogerk)
**	    Added support for Shared Buffer Manager.
**	    Added DM0P_CLCACHE action.
**	    Added tcb parameter to force_page calls.
**	12-sep-90 (jonb)
**	    Integrated ingres6301 change 280141 (rogerk):
**            Validate buffers before tossing using this TCB - if this tcb
**            is invalid (the table has been modified by a different server
**            since building it), then we don't want to write out any pages
**            from the new file (read in by a different server) to our old
**            version of the table.
**	3-may-1991 (bryanp)
**	    Make dmd_checks more useful by logging detailed error information
**	    before calling dmd_check.
**	17-oct-1991 (rogerk)
**	    Made routine work on secondary indexes.  Previously this routine
**	    could only be called on base table tcb's - and an effect of the
**	    routine was to toss pages for the secondary indexes as well.
**	    If some well-meaning piece of code accidentally called this
**	    routine with a 2nd index TCB, the server would usually dmd_check.
**	26-oct-1992 (rogerk)
**	    Reduced logging project: changed references to tcb fields to
**	    use new tcb_table_io structure fields.
**	19-oct-1992 (kwatts) change from 6.4 line: 18-nov-91 (kwatts)
**	    Added DM0P_CLFORCE action.
**	17-dec-1992 (rogerk & jnash)
**	    Allowed routine to toss pages for secondary indexes even if
**	    the secondary index tcb cannot be found.  It is possible
**	    for index pages to be brought into the cache and to be left
**	    there after tossing the index tcb.  If we then rollback the
**	    creation of the index, we will be left with cached pages (which
**	    cannot be modified) that can have no TCB ever associated with
**	    them.  If we then build a tcb for the base table and try to
**	    do a close_page, we will find the old 2nd index pages.  We must
**	    be able to toss them even though we cannot build a tcb for them.
**	16-feb-1993 (rogerk)
**	    Fixed shared cache partial TCB problem.  If close_page is called 
**	    with a partial TCB we must verify that the pages found can be
**	    accessed through the TCB before trying to call force_page.  We
**	    must also skip secondary index pages when the passed in partial
**	    TCB does not have a link to an appropriate index tcb.
**	26-mar-1993 (kwatts)
**	    Action DM0P_CLFORCE was forcing out fixed pages
**	    onto the free chain. Changed the option to force_page() to
**	    WRITE so that pages simply get written in this instance.
**	22-feb-1994 (andys)
**	    Integrate recent changes from ingres63p.
**		 3-may-1993 (andys)
**		    If page is MUTEX'ed, wait for it to be free, mark
**		    it MUTEX'ed, then write it out and mark it
**		    unMUTEXed.  Part of fixes for bug 46951.
**		11-jun-1993 (andys)
**		    Changed routine to write out all modified pages in
**		    the table, even if the pages are currently fixed.
**		    We now check for page_stat dirty bits as well as in
**		    the buffer header and to wait for mutexed pages to
**		    become free (and set MUTEX bit while writing).
**		    These are necessary to satisfy multi-server CP
**		    protocols.  When a TCB is tossed during a CP, we
**		    must make sure that all modified pages are flushed
**		    (and marked as processed by the CP) or else we risk
**		    having all servers CP threads skipping the same
**		    page due to not having a valid TCB at the time they
**		    encounter it.  Delete consistency check which
**		    checked for pages being unexpectedly fixed when
**		    close-page was called.  Don't turn off BUF_FCBUSY
**		    when we process a page - leave that up to the
**		    module which turned it on.
**		17-nov-1993 (rogerk)
**		    Added checks for encountering fixed pages during
**		    various close_page operations.  The last change to
**		    this routine was to allow fixed pages to be
**		    processed during close calls with the expectation
**		    that fixed pages were only possible in CLFORCE or
**		    CLCACHE actions.  Recent errors lead to the belief
**		    that fixed pages are resident during other
**		    close_page actions.  Added back consistency checks
**		    to detect the conditions and changed to call
**		    force_page only in DM0P_WRITE mode when acting on a
**		    fixed page.
**		 7-dec-1993 (rogerk)
**		    If a page has I/O in progress, then wait for the
**		    I/O to complete.  If we don't do this, then garbage
**		    in a page being read in can cause us to erroneously
**		    mark the buffer as modified. If the page was being
**		    read in with READLOCK=NOLOCK then after the buffer
**		    has been copied the page would be put on the free
**		    queue.  Having a buffer marked modified on the free
**		    queue would eventually result in an access
**		    violation in force_page(). [bug 55150]
**	    Make one diagnostic for the AVIS bug #ifdef xDEBUG,
**	    take out another (stashing of force_page return value).
**	    Use tbio_cache_valid_stamp instead of buf_tcb_stamp.
**  	    Change round arguments to force_page (grr).
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**      06-may-1996 (nanpr01)
**          Change the reference to the page header with macros for
**	    New Page Format.
**	06-May-1997 (jenjo02)
**	    When checking the page modified bit, use the page size
**	    in the buffer, not the one from the tbio. The TCB at
**	    this point is that of the base table; if an index is
**	    of a different size, this tbio won't work!
**	25-Aug-1997 (jenjo02)
**	    Added log_id to prototype, passing it to force_page().
**	15-Jan-1998 (jenjo02)
**	    Rewrote to find TCB's pages using the (new) table hash
**	    queue instead of a page-by-page search of every buffer 
**	    in every cache. The hash queue is a list of all BUF_VALID
**	    pages belonging to a table (TCB), including index pages, in
**	    all caches.
**	08-Jan-2002 (jenjo02)
**	    Must force GatherWrite pages if wait for BUF_MUTEX
**	    required.
**	14-Jan-2004 (jenjo02)
**	    Check db_tab_index > 0 for index, not simply != 0.
**	26-Jan-2004 (jenjo02)
**	    Check Master TCB's index list.
**	01-Mar-2004 (jenjo02)
**	    Use Partition's TBIO when forcing a Partition page.
**	01-Apr-2004 (jenjo02)
**	    If partition's Master TCB has no pp_array, can't
**	    get TBIO. This can happen during rollfoward and
**	    is, uh, ok.
**	24-Aug-2005 (jenjo02)
**	    In multi-server, partitioned table, watch for
**	    closed Partition TCB on this server, but buffer 
**	    faulted by another server.
*/
DB_STATUS
dm0p_close_page(
DMP_TCB		    *tcb,
i4		    lock_list,
i4		    log_id,
i4		    action,
i4		    *err_code)
{
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BMCB       *bm;
    DM0P_BSTAT	    *lbm_stats;
    DMP_TABLE_IO    *tbio;
    DM0P_BUFFER	    *b;
    DMPP_PAGE	    *page;
    DMP_TCB	    *it;
    i4	    base_id = tcb->tcb_rel.reltid.db_tab_base;
    i4	    indx_id = tcb->tcb_rel.reltid.db_tab_index;
    i4	    database_id = tcb->tcb_dcb_ptr->dcb_id;
    i4         force_action;
    DB_STATUS	    status = E_DB_OK;
    i4	    	    gw_queued = 0;
    i4 	    bucket;
    DM0P_BLQ_HEAD   *tab_bucket;
    DM0P_BID	    next_bid;
    i4 	    num_bufs = 0;
    i4 	    num_other_bufs = 0;
    
    /* Compute the table's hash bucket */
    bucket = (database_id + (u_i4)base_id) & (BMCB_TABLE_MAX - 1);
    tab_bucket = &bm_common->bmcb_tabq_buckets[bucket];

    next_bid = tab_bucket->blqh_bid;

    /*
    ** Scan the table bucket chain looking for any buffers belonging
    ** to this table. 
    */
    while (next_bid.bid_index != BUF_ENDLIST)
    {
	b = &lbm->lbm_buffers[next_bid.bid_cache][next_bid.bid_index];

	if (b->buf_dbid != database_id ||
	    b->buf_tabid.db_tab_base != base_id ||
	    (indx_id && b->buf_tabid.db_tab_index != indx_id) ||
	    b->buf_tabq_bucket != bucket)
	{
	    num_other_bufs++;
	    if (b->buf_tabq_bucket == bucket)
		next_bid = b->buf_tabq.blq_next;
	    else
		next_bid = tab_bucket->blqh_bid;
	    continue;
	}

	/*
	** Lock the buffer, then check it again.
	*/
	dm0p_mlock(SEM_EXCL, &b->buf_mutex);

	next_bid = b->buf_tabq.blq_next;

	if (b->buf_dbid != database_id ||
	    b->buf_tabid.db_tab_base != base_id ||
	    (indx_id && b->buf_tabid.db_tab_index != indx_id) ||
	    b->buf_tabq_bucket != bucket)
	{
	    dm0p_munlock(&b->buf_mutex);
	    num_other_bufs++;
	    if (b->buf_tabq_bucket != bucket)
		next_bid = tab_bucket->blqh_bid;
	    continue;
	}

	num_bufs++;
	tbio = &tcb->tcb_table_io;

	/*
	** If the page is not modified and not on a transaction queue and
	** we do not have to toss the pages from the cache (action is CACHE
	** or CLFORCE), then we can skip this buffer.
	**
	** The modify state must be checked on the page itself rather than
	** just on the buffer header.  All pages that may have been modified
	** before we started the close operation must be forced to satisfy
	** multi-cache consistency point protocols.
	*/
	page = (DMPP_PAGE *)((char *)lbm->lbm_pages[b->buf_id.bid_cache]
		+ b->buf_page_offset);
	
	if (((b->buf_sts & BUF_MODIFIED) == 0) &&
	    ((DMPP_VPT_GET_PAGE_STAT_MACRO(b->buf_relpgtype, page) & 
	      DMPP_MODIFY) == 0) &&
	    (action == DM0P_CLCACHE || action == DM0P_CLFORCE) &&
	    (b->buf_txnq_bucket == BUF_NOBUCKET))
	{
	    dm0p_munlock(&b->buf_mutex);
	    continue;
	}

	/*
	** If the buffer was read in by a different version TCB, then
	** skip it and leave it up to the owner of the TCB which read
	** in the page.
	**
	** This may happen if the table has been modified by another
	** server using the same buffer manager.  We do not want to
	** write out that server's changes to our old version of the file.
	*/
	if (b->buf_tcb_stamp != tbio->tbio_cache_valid_stamp)
	{
	    dm0p_munlock(&b->buf_mutex);
	    continue;
	}

	bm = lbm->lbm_bmcb[b->buf_id.bid_cache];
	lbm_stats = &lbm->lbm_stats[b->buf_id.bid_cache];

	/*
	** Consistency Check - some close_page modes expect exclusive
	** access to the specified table.  It is not expected that any
	** session will have a page from this table fixed or mutexed.
	**
	** In CLOSE or 0 (toss) modes there should never be fixed 
	** pages found for the table being processed.  Nor should there
	** be mutexed pages unless they are being accessed by background
	** cache operations (which set the FCBUSY bit).
	**
	** In CLCACHE mode these restrictions apply only if the cache is
	** private.
	*/
	if (((action == DM0P_CLOSE) || 
	     (action == 0) ||
	     ((action == DM0P_CLCACHE) && 
		    ( ! (bm_common->bmcb_status & BMCB_SHARED_BUFMGR)))) &&
	    ((b->buf_sts & BUF_FIXED) ||
	     ((b->buf_sts & BUF_MUTEX) &&
		((b->buf_sts & BUF_MUTEX_OK) == 0))))
	{
	    /*
	    ** Test trace points and TRdisplays added while tracking down
	    ** problems encountered by CODEX using multi-server shared cache. 
	    **
	    **   418 - return an error rather than crashing to get a traceback.
	    **   419 - dmd_check rather than continuing with the close_page. 
	    **
	    ** The current default action is to just continue under the
	    ** assumption that the checks below will correctly handle the
	    ** unexpected condition.
	    */
	    TRdisplay("%@ Warning: Unexpected buffer state during close_page.\n");
	    TRdisplay("  Close action %d, Buffer 0x%x, buf status: 0x%x, page_stat: 0x%x\n",
		action, b, b->buf_sts, 
		DMPP_VPT_GET_PAGE_STAT_MACRO(b->buf_relpgtype, page));

	    ule_format(E_DM9C01_FIXED_BUFFER_INFO, (CL_SYS_ERR *)0,
			ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 6,
			0, b->buf_lock_key.lk_key4,
			0, b->buf_tabid.db_tab_base,
			0, b->buf_tabid.db_tab_index,
			0, b->buf_sts,
			sizeof(DB_DB_NAME), &tcb->tcb_dcb_ptr->dcb_name,
			0, (BUF_FIXED | BUF_MUTEX) );

	    if (DMZ_BUF_MACRO(18))
	    {
		dm0p_munlock(&b->buf_mutex);
		*err_code = E_DM9300_DM0P_CLOSE_PAGE;
		return (E_DB_ERROR);
	    }

	    if (DMZ_BUF_MACRO(19))
		dmd_check(E_DM9300_DM0P_CLOSE_PAGE);
	}

	/*
	** If the page belongs to a secondary index, and we were called with
	** a base table TCB, then find the secondary index TCB for the 
	** force_page call.  Note that if the caller-supplied TCB is a partial
	** tcb, the index may not be found.
	**
	** Look for indexes on the Master TCB.
	**
	** If the page belongs to a Partition and we were called with
	** a base (Master's) TCB, then use the Partition's TBIO.
	**
	** During rollforward, the "Master" TCB with which we are
	** called will probably be a shell and not even identified
	** as a partition Master let alone have a pp_array, so there's
	** no way to find the partition's TBIO through the Master.
	** That being the case, we'll just ignore it an fall into
	** the "what if there's no TBIO" checks below, which we
	** should pass. Eventually, we'll be called with the 
	** partition's real TCB and this buffer will be properly
	** "closed".
	**
	** NB: "Normally", each Partition's TCB is dm0p_close_page'd
	**	and then Master, not the other way round. Because
	**	Master has no underlying physical DB in and of
	**	itself (TBIO_OPEN will always be FALSE), the only
	**	way we can be here with Master's TCB is if there
	**	are indexes on the partitioned table and by now
	**	all underlying Partition TCBs should be closed and
	**	their cache pages flushed. In a shared cache,
	**	multi-server environment though, some Partition
	**	pages may have been faulted in by another server
	**	after this server's Partition TCBs have been 
	**	closed, and now we've found one of them. Great.
	**	Our TCB and TBIO have been deallocated, so we
	**	won't find a TCB that matches buf_tabid.
	**	If all of these things are true, skip the
	**	buffer and trust that all will be ok.
	*/
	if ( b->buf_tabid.db_tab_index && indx_id == 0 )
	{
	    tbio = NULL;

	    if ( b->buf_tabid.db_tab_index & DB_TAB_PARTITION_MASK )
	    {
		if ( tcb->tcb_pp_array )
		{
		    if ( it = tcb->tcb_pp_array[b->buf_partno].pp_tcb )
		    {
			if ( it->tcb_rel.reltid.db_tab_index ==
				    b->buf_tabid.db_tab_index )
			{
			    tbio = &it->tcb_table_io;
			}
		    }
		    else
		    {
			/*
			** Partition's TCB missing.. 
			** If multi-server, assume another server
			** faulted this page in and ignore
			** the buffer.
			*/
			if ( bm_common->bmcb_srv_count > 1 )
			{
			    dm0p_munlock(&b->buf_mutex);
			    continue;
			}
		    }
		}
	    }
	    else for (it = tcb->tcb_pmast_tcb->tcb_iq_next; 
		      it != (DMP_TCB *) &tcb->tcb_pmast_tcb->tcb_iq_next;
		      it = it->tcb_q_next)
	    {
		if (it->tcb_rel.reltid.db_tab_index == 
			b->buf_tabid.db_tab_index)
		{
		    tbio = &it->tcb_table_io;
		    break;
		}
	    }
	}

	/*
	** Check if access is allowed to this buffer through the tbio
	** that we have (if we have found one).  If the passed in TCB
	** is only partially built, then we may not be able to toss out
	** this buffer.  No TBIO is needed when the buffer is not modified.
	*/
	if ((b->buf_sts & BUF_MODIFIED) &&
	    (tbio) && (tbio->tbio_status & TBIO_PARTIAL_CB))
	{
	    if (( ! (tbio->tbio_status & TBIO_OPEN)) ||
		(dm2f_check_access(tbio->tbio_location_array,
				    tbio->tbio_loc_count, 
				    b->buf_lock_key.lk_key4) == FALSE))
		tbio = NULL;
	}

	/*
	** If we failed to find a valid tbio and the page is modified, then 
	** we cannot force this page and must skip it.  If the passed in tbio 
	** was not PARTIAL, then there is some sort of protocol problem.
	*/
	if ((tbio == NULL) && (b->buf_sts & BUF_MODIFIED))
	{
	    if (tcb->tcb_status & TCB_PARTIAL)
	    {
		dm0p_munlock(&b->buf_mutex);
		continue;
	    }
	    else
	    {
		ule_format(E_DM9C02_MISSING_TCB_INFO, (CL_SYS_ERR *)0,
			ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 4,
			0, b->buf_lock_key.lk_key4,
			0, b->buf_tabid.db_tab_base,
			0, b->buf_tabid.db_tab_index,
			sizeof(DB_DB_NAME), &tcb->tcb_dcb_ptr->dcb_name);
		for ( it = tcb->tcb_pmast_tcb->tcb_iq_next;
		      it != (DMP_TCB *) &tcb->tcb_pmast_tcb->tcb_iq_next;
		      it = it->tcb_q_next )
		{
		    ule_format(E_DM9C03_INDEX_TCB_INFO, (CL_ERR_DESC *)0,
				ULE_LOG, NULL, (char *)0, (i4)0,
				(i4 *)0, err_code, 4,
				0, it->tcb_rel.reltid.db_tab_base,
				0, it->tcb_rel.reltid.db_tab_index,
				sizeof(DB_TAB_NAME), &it->tcb_rel.relid,
				sizeof(DB_DB_NAME),
					&tcb->tcb_dcb_ptr->dcb_name);
		}
		dmd_check(E_DM9300_DM0P_CLOSE_PAGE);
	    }
	}

#ifdef xDEBUG
	/*
	** Consistency check for codex/avis bug in patch release.
	** If the buffer is RBUSY and the page looks like it is modified
	** then the previous release would have incorrectly updated the
	** buffer to appear modified.  This was bad.
	*/
	if ((b->buf_sts & BUF_RBUSY) &&
	    (DMPP_VPT_GET_PAGE_STAT_MACRO(b->buf_relpgtype, page) & 
	     DMPP_MODIFY))
	{
	    TRdisplay("Bug 55150 Consistency Check: Buffer 0x%x is being\n", b);
	    TRdisplay("\tfaulted in but appears modified in the close_page\n");
	    TRdisplay("\troutine. Buffer status 0x%x. Waiting for the buffer\n",
		b->buf_sts);
	    TRdisplay("\tIO to complete.\n");
	}
#endif /* xDEBUG */

	/*
	** If page has IO in progress or is mutexed, wait,
	** then retry the same buffer.
	*/
	if ( b->buf_sts & (BUF_BUSY | BUF_MUTEX) )
	{
	    /*
	    ** Need to force any I/O's which are being done by this thread
	    ** as a gather write (GW).  This prevents deadlock between GW's
	    ** such that they aren't each waiting for I/O's by the other
	    ** GW which haven't been forced out yet (and won't be because
	    ** they are waiting for each other).
	    */

	    if (gw_queued)
	    {
		dm0p_munlock(&b->buf_mutex);
		if ( status = dm2f_force_list() )
		    break;
	    }
	    else if ( b->buf_sts & BUF_BUSY )
	    {
		lbm_stats->bs_iowait[BMCB_PTYPES]++;
		bm->bm_stats.bs_iowait[BMCB_PTYPES]++;
		lbm_stats->bs_iowait[b->buf_type]++;
		bm->bm_stats.bs_iowait[b->buf_type]++;
		b->buf_sts |= BUF_IOWAIT;
		dm0p_eset(lock_list, DM0P_BIO_EVENT,
			    b->buf_id.bid_cache,
			    b->buf_id.bid_index, 0);
		dm0p_munlock(&b->buf_mutex);

		/* Wait for the I/O event to complete. */
		dm0p_ewait(lock_list, DM0P_BIO_EVENT,
			    b->buf_id.bid_cache,
			    b->buf_id.bid_index, 0, 0);
	    }
	    else if ( b->buf_sts & BUF_MUTEX )
	    {
		lbm_stats->bs_mwait[BMCB_PTYPES]++;
		bm->bm_stats.bs_mwait[BMCB_PTYPES]++;
		lbm_stats->bs_mwait[b->buf_type]++;
		bm->bm_stats.bs_mwait[b->buf_type]++;
		b->buf_sts |= BUF_MWAIT;
		dm0p_eset(lock_list, DM0P_PGMUTEX_EVENT,
				b->buf_id.bid_cache,
				b->buf_id.bid_index, 0);
		dm0p_munlock(&b->buf_mutex);

		/* Wait for the unmutex event. */
		dm0p_ewait(lock_list, DM0P_PGMUTEX_EVENT,
				b->buf_id.bid_cache,
				b->buf_id.bid_index, 0, 0);
	    }

	    /* Retry with the same buffer */
	    next_bid = b->buf_id;
	    continue;
	}

	/*
	** If the page is marked dirty then transfer the modify state to the
	** buffer header to allow force page to process it.
	*/
	if (DMPP_VPT_GET_PAGE_STAT_MACRO(b->buf_relpgtype, page) & 
	    DMPP_MODIFY)
	{
	    lbm_stats->bs_dirty[BMCB_PTYPES]++;
	    bm->bm_stats.bs_dirty[BMCB_PTYPES]++;
	    lbm_stats->bs_dirty[b->buf_type]++;
	    bm->bm_stats.bs_dirty[b->buf_type]++;
	    b->buf_sts |= BUF_MODIFIED;
	    DMPP_VPT_CLR_PAGE_STAT_MACRO(b->buf_relpgtype, page, 
				     DMPP_MODIFY);
	}

	/*
	** Turn on mutex on this page to protect it from being modified
	** while its being written. Also turn on BUF_MUTEX_OK bit to
	** signify that this buffer is busy even though it may not be fixed.
	** This is checked in dm0p_uncachefix_page.
	*/
	b->buf_sts |= (BUF_MUTEX | BUF_MUTEX_OK);

	/*
	** Choose the force option dependent close_page action mode.
	**
	** If the buffer is fixed we cannot call force_page with any mode
	** other than FP_WRITE.  Other modes will have the side effect of
	** moving it from the fixed queue.
	*/
	switch (action)
	{
	  case DM0P_CLCACHE:
	    force_action = FP_CACHE;
	    break;
	  case DM0P_CLFORCE:
	    force_action = FP_WRITE;
	    break;
	  case DM0P_CLOSE:
	    force_action = FP_TOSS;
	    break;
	  default:
	    force_action = FP_INVALID;
	}

	if (b->buf_sts & BUF_FIXED)
	{
	    force_action = FP_WRITE;
	}

	if (DMZ_BUF_MACRO(15))
	{
	    if (dm0p_check_consistency(11))
	    {
		TRdisplay("%@ Warning: BM inconsistency during close_page, before force.\n");
		TRdisplay("  Close action %d, Buffer 0x%x, buf status: 0x%x\n",
		    action, b, b->buf_sts);
	    }
	}

	/*
	** Force the page to disk (and toss it if the action mode specifies to).
	**
	** force_page() will remove the buffer from the table queue
	** if it gets tossed (invalidated).
	*/
	do
	{
	    status = force_page(b, tbio, force_action, lock_list, log_id,
				    BMCL_CLOSE_PAGE, &gw_queued, err_code);
	} while (status == E_DB_WARN);

	if ( status != E_DB_OK)
	{
	    dm0p_munlock(&b->buf_mutex);
	    break;
	}
    }

    /* Force any remaining queued GatherWrite buffers */
    if (gw_queued)
	status = dm2f_force_list();

    return (status);
}

/*{
** Name: validate_page	- Validate the cached version of a page.
**
** Description:
**      This routine manages shared pages between multiple servers.  The
**	shared pages are cached in each server when no transaction in
**	the server has locks on the pages.  The pages are locked in null mode
**	while they are cached.  When the lock is converted from null to share
**	or exclusive, the value stored with the lock is checked to see if
**	this version of the page is the most recent.  If not, the page is
**	read from the disk and the version number updated from the lock
**	value.
**
**	The buffer manager semephore must be held by the caller of this routine.
**	This mutex will be released in order to do locking and I/O and will
**	be reacquired before returning, unless there's an error.  An error
**	causes the page to be invalidated and an error returned.
**
**	Upon return: buf mutex held if OK, released if error.
**
** Inputs:
**	rcb				Rcb for lock list.
**      buffer                          The buffer to be validated.
**	action				Fix Page options:
**					    DM0P_READNOLOCK - page is being
**						read with no page lock.
**	lock_list			The lock_list to use.
**	log_id				The log_id to use.
**
** Outputs:
**      err_code                        Reason for error return status.
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**	    E_DB_FATAL
**	Exceptions:
**	    none
**
** Side Effects:
**	    The buffer is marked busy while being validated.
**
** History:
**	10-jun-1986 (Derek)
**          Created for Jupiter.
**	 6-dec-1988 (rogerk)
**	    Dont turn off BUF_CACHED bit as the page may not actually be fixed.
**	28-feb-1989 (rogerk)
**	    Added support for Shared Buffer Manager.
**	    Took buf_newvalue field out of DM0P_BUFFER.  Use stack variable
**	    to request lock value.
**	3-may-1991 (bryanp)
**	    Make dmd_checks more useful by logging detailed error information
**	    before calling dmd_check.
**	17-oct-1991 (rogerk)
**	    Merged changes made by Derek during the file extend project.
**	    Added handling for BUF_PAWRITE type buffers.  These need to
**	    go through multi-cache protocols (write at commit/unfix, validate
**	    when fix) during pass-abort operations (when the RCP is acting
**	    on the database).
**	    Added trace flags in xDEBUG code.
**	    Added new session statistics collecting.
**	 3-nov-1991 (rogerk)
**	    Added fixes for File Extend recovery.
**	    Added buffer status PAUNFIX which allows us to tell whether the
**	    WUNFIX condition of a buffer is because of the type of buffer or
**	    because a pass-abort operation is in progress.  This tells us
**	    whether we can reset the WUNFIX status after the pass-abort is
**	    complete.
**	21-jan-1992 (jnash)
**	    Remove PAWRITE validate code.  Modified technique to address
**	    PAWRITE pages simply tosses them at unfix during Pass Abort, 
**	    eliminating need for cache validation.
**	26-oct-1992 (rogerk)
**	    Reduced logging project: changed references to tcb fields to
**	    use new tcb_table_io structure fields.  Added action field in
**	    which to pass readnolock state.
**	07-oct-1992 (jnash)
**	    6.5 Reduced logging project:
**	     -  Validate the page checksum.
**	24-may-1993 (bryanp)
**	    If trace point DM302 is set, trace cache locking activity.
**	18-oct-1993 (rmuth)
**	    Add DMP_BMCB statistics.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	20-mar-1996 (nanpr01)
**	    Added page size as parameter to this routine. To compute
**	    the checksum pagesize is need since sizeof(*page) returns
**	    only the size of physical page.
**      06-may-1996 (nanpr01)
**          Change the reference to the page header with macros for
**	    New Page Format. 
**      01-aug-1996 (nanpr01 for ICL phil.p)
**          Introduced support for the Distributed Multi-Cache Management
**          (DMCM) procotol.
**          Under this procotol, pages are retained in the cache at the end
**          of a transaction. This is effectively providing fast commit in
**          a multi-cache environment.
**          When, on a call to dm0p_cachefix_page, a page is found in the
**          cache, then the following occurs :-
**          If the page is locked at NULL, then it must be validated.
**          If the page is locked at S/X (this is only done under DMCM)
**          then it must already be valid. If the new requested lock mode  
**          is higher than the old mode, it will be converted.
**          NB. Interface changed to include the access_mode parameter,
**              to enable us to decide whether or not we need to convert
**              the existing cache lock.
**	01-aug-1996 (nanpr01)
**	    Change the lock_mode to b->buf_lock_mode to log the granted
**	    lock mode of the cache lock.
**	    Cache-Deadlock Problem : If we ask for a shared cache lock but
**	    get it in higher mode, it is possible that the logical locks
**	    are held at a lower mode consequently allowing the server to fix
**	    the page and causing cache deadlock.
**	    Solution :
**	    1. Force the page out and reacquire the cache lock at given mode.
**	    This is waht is currently implemented.
**	    2. Convert the logical lock to higher mode ie in sync with cache
**	    so that the other servers are blocked.
**	29-aug-1996 (nanpr01)
**	    Try to reclaim locks if lock manager is out of locks before
**	    giving up.
**	14-Sep-1996 (jenjo02)
**	    Caller must hold buffer's buf_mutex.
**	14-Mar-1997 (jenjo02)
**	    Changed protocol to pass log_id so that it can be passed
**	    to force_page().
**	16-Apr-1997 (jenjo02)
**	    count a bs_check only if we validated the page.
**	    Keep buf_lock_mode in sync with most recent lock mode.
**	01-feb-1999 (nanpr01 & jenjo02)
**	    We must downgrade the cache lock after forcing the page to disk
**	    and reacquire the cache lock in the required mode to avoid
**	    cache deadlock for every pages not just secondary index pages.
**	05-Feb-1999 (jenjo02)
**	    If we have a strong enough DMCM cache lock, don't force the page.
**	    dm0p_dmcm_callback() changed to force fixed-for-read pages if
**	    need be to reduce the lock to LK_S and avoid this deadlock.
**	    Only unmutex and mark buffer busy if LK or I/O is needed.
**	05-jan-2005 (devjo01)
**	    Refresh LVB when page refreshed from disk to avoid spurious
**	    E_DM9C91_BM_LSN_MISMATCH errors.
**	28-Feb-2007 (jonj)
**	    Translate DM0P_NOLKSTALL action to LK_NOSTALL for cache locks.
*/
static DB_STATUS
validate_page(
DMP_TABLE_IO		*tbio,
DM0P_BUFFER		*buffer,
i4			action,
i4			lock_list,
i4			log_id,
i4                 access_mode,
i4			*err_code)
{
    DM0P_BUFFER		*b = buffer;
    DMP_LBMCB           *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BMCB		*bm = lbm->lbm_bmcb[b->buf_id.bid_cache];
    DM0P_BSTAT	        *lbm_stats = &lbm->lbm_stats[b->buf_id.bid_cache];
    DM0P_BM_COMMON      *bm_common = lbm->lbm_bm_common;
    LK_VALUE		newvalue;
    i4		lock_mode;
    CL_SYS_ERR		sys_err;
    STATUS		cl_status = OK;
    i4		n;
    i4			refresh = FALSE;
    DB_STATUS		status = E_DB_OK;
    DMPP_PAGE           *page;
    i4                  validate = FALSE;
    i4             error_code;
    i4             orig_lock_mode;
    i4             new_lock_mode = b->buf_lock_mode;
    i4             retry = 0;
    i4                  lock_flags;
    i4			target_lsn_low;	
    i4			old_lock_id;

    /*
    ** Caller must hold buffer's buf_mutex.
    ** Mark buffer BUF_RBUSY and release the
    ** mutex only if we need to make LK|I/O calls.
    */
    page = (DMPP_PAGE *)((char *)lbm->lbm_pages[b->buf_id.bid_cache] + 
			    b->buf_page_offset);

    lock_flags = LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK;
    if ( action & DM0P_NOLKSTALL )
	lock_flags |= LK_NOSTALL;

    /* 
    ** (ICL phil.p)
    ** Support cache locking strategies for 
    ** both DMCM and non-DMCM protocols.
    */
    for (; ;)
    {
	old_lock_id = b->buf_lk_id.lk_unique;

	if (b->buf_sts & BUF_DMCM)
	{
	    lock_mode = LK_S;
	    if (access_mode & DM0P_WRITE) 
		lock_mode = LK_X;

	    /* 
	    ** The cached page found need only be validated if it was kept
	    ** with a NULL lock. Otherwise, it is guaranteed to be valid.
	    */
	    if (b->buf_lock_mode == LK_N)
		validate = TRUE;

	    orig_lock_mode = b->buf_lock_mode;
	    /*
	    ** Only make lock request if the new mode
	    ** is higher than that on the cached page.
	    */
	    if (lock_mode > b->buf_lock_mode)
	    {
		/* Mark buffer busy, release its mutex */
		b->buf_sts |= BUF_RBUSY;
		dm0p_munlock(&b->buf_mutex);

		new_lock_mode = lock_mode;
		cl_status = LKrequestWithCallback(
		    lock_flags, 
		    lbm->lbm_lock_list, (LK_LOCK_KEY *)0, 
		    lock_mode, &newvalue, (LK_LKID *)&b->buf_lk_id, (i4)0, 
		    (LKcallback) dm0p_dmcm_callback, NULL, &sys_err);
	    }
	    else
	    {
		/* This is a candidate for deadlock for DMCM                 */
		/* Any pages where covering lock is weaker than cache lock   */
		/* is candidate for deadlock ...			     */

		/*
		** This is no longer necessary. Callbacks will fire if there's
		** a conflict, and dm0p_dmcm_callback() will force even a 
		** fixed-for-read page, reducing its cache lock to LK_S and
		** avoiding this deadlock.
		** 05-Feb-1999 (jenjo02)
		*/
	    }
	}
	else
	{
	    /* Mark buffer busy, release its mutex */
	    b->buf_sts |= BUF_RBUSY;
	    dm0p_munlock(&b->buf_mutex);

	    validate = TRUE;
	    lock_mode = LK_IX;
	    if (! (action & DM0P_READNOLOCK))
		lock_mode = LK_IS;
	    new_lock_mode = lock_mode;

	    if (DMZ_LCK_MACRO(2))
		dmd_lock(&b->buf_lock_key, lbm->lbm_lock_list, LK_CONVERT,
		    lock_flags,
		    lock_mode, 0, (DB_TRAN_ID *)0, (DB_TAB_NAME *)0, 
		    (DB_DB_NAME *)0);

	    cl_status = LKrequest(
		lock_flags,
		lbm->lbm_lock_list, (LK_LOCK_KEY *)&b->buf_lock_key,
		lock_mode, &newvalue, (LK_LKID *)&b->buf_lk_id,
		(i4)0, &sys_err);
	}

	if ((cl_status != OK) && (cl_status != LK_VAL_NOTVALID) &&
	    (cl_status != LK_CANCEL))
	{
	     if (cl_status == LK_NOLOCKS && !retry) 
	     {
		 retry++;
		 if (dm0p_reclaim(lock_list, log_id) == E_DB_OK)
		     continue;
	     }

	     ule_format(cl_status, &sys_err, ULE_LOG, NULL,
	       (char *)0, (i4)0, (i4 *)0, err_code, 0);
	     ule_format(E_DM9022_BM_BAD_PROTOCOL, &sys_err, ULE_LOG, NULL,
	       (char *)0, (i4)0, (i4 *)0, err_code, 5,
	       0, b->buf_lock_key.lk_key4,
	       sizeof(DB_TAB_NAME), tbio->tbio_relid,
	       sizeof(DB_DB_NAME), tbio->tbio_dbname,
	       0, lock_mode, 0, lbm->lbm_lock_list);

	     /*
	     ** The buf_lk_id might be relevant, so log that too.
	     */
	     ule_format(E_DM9022_BM_BAD_PROTOCOL, &sys_err, ULE_LOG, NULL,
	       (char *)0, (i4)0, (i4 *)0, err_code, 5,
	       0, b->buf_lock_key.lk_key4,
	       sizeof(DB_TAB_NAME), tbio->tbio_relid,
	       sizeof(DB_DB_NAME), tbio->tbio_dbname,
	       0, lock_mode, 0, b->buf_lk_id);
	   
	     dmd_check(E_DMF020_DM0P_VALIDATE);
	}

	if ( old_lock_id != b->buf_lk_id.lk_unique )
	{
	    /* Process which allocated lock */
	    b->buf_pid = lbm->lbm_pid;
	}
	b->buf_lock_mode = new_lock_mode;
	break;
    }

    if (validate && 
       ((newvalue.lk_high != b->buf_oldvalue.lk_high) ||
	(newvalue.lk_low != b->buf_oldvalue.lk_low) || 
	(cl_status == LK_VAL_NOTVALID)))
    {
	if (b->buf_sts & BUF_FIXED)
	{
	    ule_format(E_DM9C04_BUFFER_LOCK_INFO, (CL_SYS_ERR *)0,
			ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 10,
			0, b->buf_lock_key.lk_key4,
			0, b->buf_tabid.db_tab_base,
			0, b->buf_tabid.db_tab_index,
			sizeof(DB_DB_NAME), tbio->tbio_dbname,
			0, b->buf_sts,
			0, newvalue.lk_high, 0, newvalue.lk_low,
			0, b->buf_oldvalue.lk_high, 0, b->buf_oldvalue.lk_low,
			0, cl_status);
	    dmd_check(E_DMF020_DM0P_VALIDATE);
	}

	/*
	**  The cached version of the page is different then the
	**  most recent version updated by another server.  The
	**  page is refreshed from disk before returning to the
	**  caller.
	*/

#ifdef xDEBUG
	if (DMZ_BUF_MACRO(2))
	{
	    TRdisplay("REFRESH_READ: (%~t,%~t,%d,%dK)\n",
		sizeof(DB_DB_NAME), tbio->tbio_dbname,
		sizeof(DB_TAB_NAME), tbio->tbio_relid, 
		b->buf_lock_key.lk_key4,
		KSIZE(tbio->tbio_page_size));
	}
#endif

	if (DMZ_SES_MACRO(10))	    
	    dmd_iotrace(DI_IO_READ, 
		(DM_PAGENO)b->buf_lock_key.lk_key4, 1, 
		tbio->tbio_relid, 
		&tbio->tbio_location_array[0].loc_fcb->fcb_filename, 
		tbio->tbio_dbname);

	n = 1;
	status = dm2f_read_file(tbio->tbio_location_array,
	    tbio->tbio_loc_count, tbio->tbio_page_size,
	    tbio->tbio_relid, tbio->tbio_dbname, &n,
	    (i4)b->buf_lock_key.lk_key4,
	    (char *)page,
	    err_code);
	if (status != E_DB_OK)
	{
	    if (*err_code > E_DM_INTERNAL)
	    {
		ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL, (char *)0,
		    (i4)0, (i4 *)0, err_code, 0);
		*err_code = E_DM9210_BM_VALIDATE_PAGE_ERROR;
	    }
	}
	else
	{
	    /*
	    ** Crosscheck LSN sent with LVB with that on page.
	    **
	    ** Using OpenDLM the LVB is cleared if Lock is invalidated
	    ** because a remote exclusive lock holder was killed.  The LVB
	    ** is revalidated as part of failover, but the LVB remains
	    ** zeroed.  Therefore we Don't perform this check if the LVB
	    ** version number is zero.
	    */
	    target_lsn_low =
	     DMPP_VPT_GET_LOG_ADDR_LOW_MACRO(b->buf_relpgtype, page);
	    if ( (cl_status != LK_VAL_NOTVALID) &&
	         (newvalue.lk_high != 0) && 
	         (newvalue.lk_low != target_lsn_low) )
	    {
		ule_format(E_DM9C91_BM_LSN_MISMATCH, (CL_SYS_ERR *)0,
		    ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0,
		    err_code, 6,
		    0,DMPP_VPT_GET_LOG_ADDR_HIGH_MACRO(b->buf_relpgtype, page),
		    0, target_lsn_low,
		    0, newvalue.lk_low,
		    sizeof(DB_DB_NAME), tbio->tbio_dbname,
		    sizeof(DB_TAB_NAME), tbio->tbio_relid,
		    0, b->buf_lock_key.lk_key4);
		*err_code = E_DM9210_BM_VALIDATE_PAGE_ERROR;
		status = E_DB_ERROR;
	    }
	    else
	    {
		/*
		** Checksum the page to verify that it is a valid page.
		*/
		status = dm0p_validate_checksum(page, tbio->tbio_page_type,
			    (DM_PAGESIZE) bm->bm_pgsize);
		if (status != E_DB_OK)
		{
		    ule_format(E_DM930C_DM0P_CHKSUM, (CL_SYS_ERR *)0,
			ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0,
			err_code, 3,
			sizeof(DB_DB_NAME), tbio->tbio_dbname,
			sizeof(DB_TAB_NAME), tbio->tbio_relid,
			0, b->buf_lock_key.lk_key4);
		    *err_code = E_DM9210_BM_VALIDATE_PAGE_ERROR;
		}
		else
		{
		    /*
		    ** Checksum validates the page: write the new cache lock
		    ** value on the buffer.
		    */
		    refresh = TRUE;
		    STRUCT_ASSIGN_MACRO(newvalue, b->buf_oldvalue);

		    /* Turn off modified bit since just read in a new page */
		    DMPP_VPT_CLR_PAGE_STAT_MACRO(b->buf_relpgtype, page, 
					     DMPP_MODIFY);
		}
	    }
	}
    }

    if (action & DM0P_READNOLOCK)
    {
	if ((b->buf_sts & BUF_DMCM) == 0)
	{
	    error_code = E_DMF022_DM0P_CACHE_IX_IS;

	    old_lock_id = b->buf_lk_id.lk_unique;
	    cl_status = LKrequest( lock_flags,
	      lbm->lbm_lock_list,
	      (LK_LOCK_KEY *)0, LK_IS, (LK_VALUE *)0, 
	      (LK_LKID *)&b->buf_lk_id, (i4)0, &sys_err); 

	    if (cl_status != OK)
	    {
		ule_format(cl_status, (CL_ERR_DESC *)&sys_err, ULE_LOG ,NULL, 
			    (char * )0, (i4)0, (i4 *)0, err_code, 0);

		ule_format(E_DM9C05_CACHE_CVT_ERROR, (CL_SYS_ERR *)&sys_err,
			    ULE_LOG, NULL,
			    (char *)0, (i4)0, (i4 *)0, err_code, 7,
			    0, b->buf_lock_key.lk_key4,
			    0, b->buf_tabid.db_tab_base,
			    0, b->buf_tabid.db_tab_index,
			    sizeof(DB_DB_NAME), tbio->tbio_dbname,
			    0, lbm->lbm_lock_list,
			    0, b->buf_lk_id,
			    0, LK_IS );

		dmd_check(error_code);
	    }
	    else
	    {
		if ( !old_lock_id )
		{
		    /* Process which allocated lock */
		    b->buf_pid = lbm->lbm_pid;
		}
		b->buf_lock_mode = LK_IS;
	    }
	}
    }

    /* Update buffer manager statistics. */
    if (validate)
    {
	lbm_stats->bs_check[BMCB_PTYPES]++;
	bm->bm_stats.bs_check[BMCB_PTYPES]++;
	lbm_stats->bs_check[b->buf_type]++;
	bm->bm_stats.bs_check[b->buf_type]++;
    }
    if (refresh)
    {
	lbm_stats->bs_refresh[BMCB_PTYPES]++;
	bm->bm_stats.bs_refresh[BMCB_PTYPES]++;
	lbm_stats->bs_refresh[b->buf_type]++;
	bm->bm_stats.bs_refresh[b->buf_type]++;
    }


    /* Relock the buffer if we released its mutex */
    if (b->buf_sts & BUF_RBUSY)
    {
	dm0p_mlock(SEM_EXCL, &b->buf_mutex);

	if (b->buf_sts & BUF_IOWAIT)
	{
	    dm0p_erelease(lock_list, DM0P_BIO_EVENT, 
			    b->buf_id.bid_cache,
			    b->buf_id.bid_index, 0);
	}
	b->buf_sts &= ~(BUF_RBUSY | BUF_IOWAIT);
	/* If a group buffer, and readlock-nolock, and everything is OK,
	** make sure that the SKIPPED status is cleared from the group
	** header.  A gfaulter might have tried to use the group while
	** the buffer was busy.  We only have to do this if readlock-
	** nolock, because for that case we don't actually fix the
	** buffer.  (A normal fix/unfix clears SKIPPED at unfix time.)
	** If we don't do this, SKIPPED can be left on, and the group
	** in essence becomes unusable until a close or invalidate.
	*/
	if (b->buf_owner != BUF_SELF
	  && (action & DM0P_READNOLOCK) && status == E_DB_OK)
	{
	    DM0P_GROUP *group_array = lbm->lbm_groups[b->buf_id.bid_cache];
	    DM0P_GROUP *g = &group_array[b->buf_owner];

	    dm0p_mlock(SEM_EXCL, &g->g_mutex);
	    g->g_sts &= ~G_SKIPPED;
	    dm0p_munlock(&g->g_mutex);
	}
    }

    if (status == E_DB_OK)
    {
	return (E_DB_OK);	
    }

    /*
    ** Invalidate this page if got an error trying to read a new copy from
    ** disk.  Be careful that we don't invalidate a page that has changes
    ** from a previous committed transaction that has not yet been flushed
    ** to disk.  We are safe here since this routine will not be called
    ** when using Fast Commit.
    **
    ** devjo01 - 13-oct-2004 - previous comment is suspect, since DMCM
    ** IS effectively fast commit with multiple caches.
    */
    invalidate_page(b, lock_list);		/* releases buf mutex */
    return (E_DB_ERROR);
}

/*{
** Name: dm0p_lock_page	- Obtain Page Lock.
**
** Description:
**	This routine obtains a transaction page lock on a page being fixed
**	in the buffer manager.
**
** Inputs:
**	lock_list			Lock List Id
**	dcb				Pointer to Database Control Block
**	table_id			Pointer to Table Id
**	page_number			The page number in the table.
**	lock_mode			Lock Mode : LK_X, LK_S
**	lock_action			Lock request qualifiers:
**					  LK_STATUS - return lock grant status
**					  LK_PHYSICAL - use physical locks
**					  LK_NOWAIT - don't wait if can't grant
**					  LK_TEMPORARY - temporary lock
**					  LK_NONINTR - don't interrupt lock req
**	lock_timeout			Timeout of lock request
**	relid				Table name for error messages
**	tran_id				Transaction id for trace messages
**
** Outputs:
**      lock_id                         Lock id of the lock acquired.
**	new_lock			Indicates a new lock resource was needed
**	err_code			Reason for error return status.
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**
** History:
**	26-oct-1992 (rogerk)
**          Created for Reduced Logging Project.  Code moved here from old
**	    dm0p_fix_page routine.
**	22-nov-1993 (jnash)
**	    Dump lockstat info on page or table escalation deadlock and 
**	    if RCP or if DM304 set. 
**	30-may-1996 (pchang)
**	    Move the display of the E_DM9041_LK_ESCALATE_TABLE message to
**	    dm0p_fix_page() so that it only gets logged if lock escalation is
**	    really going to happen.
**      12-dec-1996 (cohmi01)
**          Add more info to lock timeout/deadlock msgs (Bug 79763).
**      22-nov-96 (dilma04)
**          Row Locking Project:
**          Support for LK_PH_PAGE lock requests
**          Call dm0p_unlock_page with lock_type paramater
**      07-may-97 (dilma04)
**          Bug 81895. If LK returns LK_COV_LOCK treat this lock as a new one.
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          - added *lock_id argument;
**          - if LKrequest() returns LK_LOG_LOCK, treat this lock as a new one;
**          - call dm0p_unlock_page with lock_id paramater;
**	23-Jun-1998 (jenjo02)
**	    Added (optional) lock_value parm to prototype, used to lock/unlock
**	    FHDR/FMAPs.
**	    Make "new_lock" optional - not all callers care.
**	28-May-1999 (nanpr01)
**	    SIR 91157 : Added LK_QUIET_TOO_MANY flag to stop logging 
**	    DMA00D message.
**	11-Oct-1999 (jenjo02)
**	    Test locally for security to avoid dma_write_audit() calls.
**	20-Aug-2002 (jenjo02)
**	    Support TIMEOUT=NOWAIT, new LK_UBUSY status from LKrequest().
**	    Write E_DM9066_LOCK_BUSY info message if DMZ_SES_MACRO(35).
**	    Return E_DM006B_NOWAIT_LOCK_BUSY.
**	12-Feb-2003 (jenjo02)
**	    Delete DM006B, return E_DM004D_LOCK_TIMER_EXPIRED instead.
**  	29-Sep-2004 (fanch01)
**          Conditionally add LK_LOCAL flag if database/table is confined
**	    to one node.
*/
DB_STATUS
dm0p_lock_page(
i4		lock_list,
DMP_DCB		*dcb,
DB_TAB_ID	*table_id,
DM_PAGENO	page_number,
i4         lock_type,
i4		lock_mode,
i4		lock_action,
i4		lock_timeout,
DB_TAB_NAME	*relid,
DB_TRAN_ID	*tran_id,
LK_LKID         *lock_id,
i4		*new_lock,
LK_VALUE	*lock_value,
i4		*err_code)
{
    LK_LOCK_KEY	    lockkey;
    CL_SYS_ERR	    sys_err;
    STATUS	    cl_status;
    i4	    local_error;
    bool	    page_locked = FALSE;
    char	    msgbuf[64];

    /*
    ** Initialize the lock key.
    */
    lockkey.lk_type = lock_type;
    lockkey.lk_key1 = (i4)dcb->dcb_id;
    lockkey.lk_key2 = table_id->db_tab_base;
    lockkey.lk_key3 = table_id->db_tab_index;
    lockkey.lk_key4 = page_number;
    lockkey.lk_key5 = 0;
    lockkey.lk_key6 = 0;

    if (dcb->dcb_bm_served == DCB_SINGLE)
    {
        lock_action |= LK_LOCAL;
    }

    /* Output a lock trace message if tracing is enabled. */

    if (DMZ_SES_MACRO(1))
	dmd_lock(&lockkey, lock_list, LK_REQUEST, lock_action, lock_mode, 
	    lock_timeout, tran_id, relid, &dcb->dcb_name);        

    /*
    ** Request the page lock.
    */
    cl_status = LKrequest(lock_action, lock_list, &lockkey,
	lock_mode, lock_value, lock_id, lock_timeout, &sys_err);

    if (cl_status == OK || cl_status == LK_VAL_NOTVALID)
    {
	if (new_lock)
	    *new_lock = FALSE;
	return (E_DB_OK);
    }
    else if (cl_status == LK_NEW_LOCK || cl_status == LK_COV_LOCK ||
	     cl_status == LK_LOG_LOCK)
    {
	if (new_lock)
	    *new_lock = TRUE;
	return (E_DB_OK);
    }
    else
    {
	/* Log everything but a new lock status. */
	if (cl_status == LK_NOLOCKS)
	{
	    if ((lock_action & LK_QUIET_TOO_MANY) == 0)
	    {
	        ule_format(E_DM004B_LOCK_QUOTA_EXCEEDED, &sys_err, ULE_LOG, 
			NULL, (char *)0, (i4)0, (i4 *)0, err_code, 0);
	    }
	    *err_code = E_DM004B_LOCK_QUOTA_EXCEEDED;
	}
	else if (cl_status == LK_DEADLOCK)
	{
          ule_format(E_DM9042_PAGE_DEADLOCK, &sys_err, ULE_LOG, NULL,
              (char *)0, (i4)0, (i4 *)0, err_code, 5,
              0, page_number, sizeof(DB_TAB_NAME), relid,
              sizeof(DB_DB_NAME), &dcb->dcb_name, 0, lock_mode,
              0, sys_err.moreinfo[0].data.string);

	    /*
	    ** DM304 enables tracing of lockstat on deadlock.
	    ** This occurs always in the RCP, as it should never encounter 
	    ** deadlocks.
	    */
	    if ((DMZ_LCK_MACRO(4)) || (dcb->dcb_status & DCB_S_RECOVER))
	    {
		dmd_lkrqst_trace(dmd_put_line, &lockkey, lock_list, 
		    LK_REQUEST, lock_action, lock_mode, lock_timeout, 
		    tran_id, relid, &dcb->dcb_name);
		dmd_lock_info(DMTR_LOCK_LISTS | DMTR_LOCK_USER_LISTS |
			DMTR_LOCK_SPECIAL_LISTS | DMTR_LOCK_RESOURCES );
	    }

	    *err_code  = E_DM0042_DEADLOCK;
	}
	else if (cl_status == LK_TIMEOUT)
	{
	    STprintf(msgbuf, "PAGE %d", page_number);
          ule_format(E_DM9043_LOCK_TIMEOUT, &sys_err, ULE_LOG, NULL,
              (char *)0, (i4)0, (i4 *)0, err_code, 6,
              sizeof(DB_TAB_NAME), relid,
              sizeof(DB_DB_NAME), &dcb->dcb_name, 0, lock_mode,
              0, "?", 0, msgbuf, 0, sys_err.moreinfo[0].data.string);
	    *err_code = E_DM004D_LOCK_TIMER_EXPIRED;
	}
	else if (cl_status == LK_RETRY)
	{
	    ule_format(E_DM9047_LOCK_RETRY, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 3,
		sizeof(DB_TAB_NAME), relid,
		sizeof(DB_DB_NAME), &dcb->dcb_name, 0, lock_mode);
	    *err_code = E_DM004F_LOCK_RETRY;
	}
	else if (cl_status == LK_INTERRUPT)
	{
	    *err_code = E_DM0065_USER_INTR;
	}
	else if (cl_status == LK_INTR_GRANT)
	{
	    page_locked = TRUE;
	    *err_code = E_DM0065_USER_INTR;
	}
	else if ( cl_status == LK_BUSY )
	{
	    *err_code = E_DM004C_LOCK_RESOURCE_BUSY;
	}
	else if ( cl_status == LK_UBUSY )
	{
	    /* Output error message iff DMZ_SES_MACRO(35) on in SCB */
	    if ( DMZ_SES_MACRO(35) )
	    {
		CS_SID	sid;
		CSget_sid(&sid);

		if ( DMZ_MACRO((GET_DML_SCB(sid))->scb_trace, 35) )
		{
		    STprintf(msgbuf, "PAGE %d", page_number);
		    ule_format(E_DM9066_LOCK_BUSY, &sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 6,
			sizeof(DB_TAB_NAME), relid,
			sizeof(DB_DB_NAME), &dcb->dcb_name, 0, lock_mode,
			0, "?", 0, msgbuf, 0, sys_err.moreinfo[0].data.string);
		}
	    }
	    *err_code = E_DM004D_LOCK_TIMER_EXPIRED;
	}
	else
	{
	    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
	    ule_format(E_DM9021_BM_BAD_LOCK_PAGE, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 5,
		0, page_number, sizeof(DB_TAB_NAME), relid,
		sizeof(DB_DB_NAME), &dcb->dcb_name, 0, lock_mode,
		0, lock_list);
	    *err_code = E_DM9204_BM_FIX_PAGE_ERROR;
	}

	/*
	** If the lock request was interrupted after being granted, and
	** it is a temporary physical lock, we need to release it before 
	** returning with an error.
	**
	** Note: the return status from the unlock call is ignored since
	** the error is logged inside unlock_page anyway.
	*/
	if ((page_locked) && (lock_action & LK_PHYSICAL))
	{
	    _VOID_ dm0p_unlock_page(lock_list, dcb, table_id, 
			page_number,
			lock_type, relid, tran_id, lock_id, lock_value, &local_error);
	}

	return (E_DB_ERROR);
    }
}

/*{
** Name: dm0p_unlock_page - Release Page Lock.
**
** Description:
**	This routine releases a transaction page lock on a page being unfixed
**	in the buffer manager.
**
**	It can only be called if a physical lock was requested when the
**	page was fixed.
**
** Inputs:
**	lock_list			Lock List Id
**	dcb				Pointer to Database Control Block
**	table_id			Pointer to Table Id
**	page_number			The page number in the table.
**	relid				Table name for error messages
**	tran_id				Transaction id for trace messages
**      lock_id                         Pointer to Lock Id used as a handle
**
** Outputs:
**	err_code			Reason for error return status.
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**
** History:
**	26-oct-1992 (rogerk)
**          Created for Reduced Logging Project.  Code moved here from old
**	    dm0p_unfix_page routine.
**      22-nov-96 (dilma04)
**          Row Locking Project:
**          Support for LK_PH_PAGE lock requests, added lock_type parameter.
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          Added support for releasing locks by lock id. 
**      12-nov-97 (stial01)
**          B80715: lockKEY always passed for set lock_trace
**	23-Jun-1998 (jenjo02)
**	    Added (optional) lock_value parm to prototype, used to lock/unlock
**	    FHDR/FMAPs.
**	    If unlocking by lock_id, skip preparing the lock key, which won't 
**	    be used anyway.
*/
DB_STATUS
dm0p_unlock_page(
i4		lock_list,
DMP_DCB		*dcb,
DB_TAB_ID	*table_id,
DM_PAGENO	page_number,
i4         lock_type,
DB_TAB_NAME	*relid,
DB_TRAN_ID	*tran_id,
LK_LKID         *lock_id,
LK_VALUE	*lock_value,
i4		*err_code)
{
    DB_STATUS       status = E_DB_OK;
    LK_LOCK_KEY     lockkey;
    CL_SYS_ERR      sys_err;
    STATUS          cl_status;
    LK_VALUE	    lk_value;

    if (lock_value == 0)
    {
	/* Initialize phoney lock value to keep locking happy */
	lk_value.lk_high = 0;
	lk_value.lk_low = 0;
    }
 
    if (lock_id == 0)
    {
	/*
	** Initialize the lock key.
	*/
	lockkey.lk_type = lock_type;
	lockkey.lk_key1 = (i4)dcb->dcb_id;
	lockkey.lk_key2 = table_id->db_tab_base;
	lockkey.lk_key3 = table_id->db_tab_index;
	lockkey.lk_key4 = page_number;
	lockkey.lk_key5 = 0;
	lockkey.lk_key6 = 0;

        /* Output a lock trace message if tracing is enabled. */
        if (DMZ_SES_MACRO(1))
	    dmd_lock(&lockkey, lock_list, LK_RELEASE, LK_SINGLE,
		0, 0, tran_id, relid, &dcb->dcb_name);
    }
    else
    {
	/* We donot have lock key */
        if (DMZ_SES_MACRO(1))
	{
	    u_i4 context = 0;
	    LK_LKB_INFO info_block;
	    u_i4 length;
	    cl_status = LKshow(LK_S_SLOCK, (i4) 0, lock_id, (LK_LOCK_KEY *) 0,
				sizeof(info_block), (PTR) &info_block, &length,
				&context, &sys_err);
	    if (cl_status != OK)
	    {
	        /* FIX_ME: need other ule_format here! */
		*err_code = E_DM9208_BM_UNFIX_PAGE_ERROR;
		return(E_DB_ERROR);
	    }
	    lockkey.lk_type = info_block.lkb_key[0];
	    lockkey.lk_key1 = info_block.lkb_key[1];
	    lockkey.lk_key2 = info_block.lkb_key[2];
	    lockkey.lk_key3 = info_block.lkb_key[3];
	    lockkey.lk_key4 = info_block.lkb_key[4];
	    lockkey.lk_key5 = info_block.lkb_key[5];
	    lockkey.lk_key6 = info_block.lkb_key[6];
	    dmd_lock(&lockkey, lock_list, LK_RELEASE, LK_SINGLE,
		0, 0, tran_id, relid, &dcb->dcb_name);
	}
    }

    cl_status = LKrelease((i4)0, lock_list, lock_id,
	lock_id ? (LK_LOCK_KEY *)0 : (LK_LOCK_KEY *)&lockkey, 
	lock_value ? lock_value : &lk_value, &sys_err);
    if (cl_status != OK)
    {
	/*
	**  Can't release physical lock, the caller probably requested the
	**  the lock without the physical flag at one time.
	*/
	if (lock_id == 0)
	{
	    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 0);
	    ule_format(E_DM9020_BM_BAD_UNLOCK_PAGE, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 4,
		0, lockkey.lk_key4, sizeof(DB_TAB_NAME), relid,
		sizeof(DB_DB_NAME), &dcb->dcb_name, 0, lock_list);
	}
	else
	{
	    /* FIX_ME: need other ule_format here! */
	}
	*err_code = E_DM9208_BM_UNFIX_PAGE_ERROR;
	status = E_DB_ERROR;
    }
 
    return (status);
}

/*
** Name: dm0p_escalate	- Escalate a table to table level locking.
**
** Description:
**      This routine escalates the table whose access context is passed
**	from page level locking to table level locking.  In the process
**	of escalating this table, all tables in the same transaction for
**	the same table are also escalated.  The resulting table level lock
**	mode will be either LK_S if initial lock mode was LK_IS, and
**	LK_X if initial lock mode was LK_IX or LK_SIX.  If table cannot
**	be escalated because of deadlock or timeout a error is returned.
**
** Inputs:
**      rcb				The record control block.
**
** Outputs:
**      err_code                        The reason for the error status.
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**	    E_DB_FATAL
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	22-oct-1985 (derek)
**          Created new for Jupiter.
**      17-jul-90 (mikem)
**          bug #30833 - lost force aborts
**          LKrequest has been changed to return a new error status
**          (LK_INTR_GRANT) when a lock has been granted at the same time as
**          an interrupt arrives (the interrupt that caused the problem was
**          a FORCE ABORT).   Callers of LKrequest which can be interrupted
**          must deal with this new error return and take the appropriate
**          action.  Appropriate action includes signaling the interrupt
**          up to SCF, and possibly releasing the lock that was granted.
**	    In the case of escalate() go ahead and delete all the page locks,
**	    and return with the interrupt status.  The calling routines will
**	    delete the granted lock as part of normal xaction semantics.
**	17-dec-90 (jas)
**	    Smart Disk change:  renamed to dm0p_escalate and made public
**	    (non-static).
**	27-nov-92 (robf)
**	    Escalations are security events, make sure they are audited.
**	24-may-1993 (bryanp)
**	    Improve error message logging when lock escalation fails.
**	1-jul-93 (robf)
**	    Pass table security label to security audit routines
**	12-aug-93 (robf)
**	    Lock escalations now RESOURCE events rather than TABLE
**	    events.
**	22-nov-1993 (jnash)
**	    Add DM304 to trace lock info on table escalation deadlock,
**	    performed unconditionally if rcp.
**      12-dec-1996 (cohmi01)
**          Add more info to lock timeout msgs (Bug 79763).
**      22-nov-96 (dilma04)
**          Row Locking Project:
**          If escalate when row-level locking, release all the row,
**          value and LK_PH_PAGE locks held by the transaction.
**      06-jan-97 (dilma04)
**          Fix bug 79817. Request to release row locking specific locks 
**          (row, value and LK_PH_PAGE) should be made unconditionally even if  
**          locking granularity of a cursor causing escalation is page level. 
**	26-feb-97 (dilma04)
**	    Extended error handling to cover problems with releasing row,
**	    value and LK_PH_PAGE locks.
**      21-may-97 (stial01)
**          dm0p_escalate: Remove release of LK_PH_PAGE locks.
**      07-jul-98 (stial01)
**          dm0p_escalate: After we escalate, scan RCB's for this 
**          transaction with same base TCB or same TCB. (B63389)
**      07-jul-98 (stial01)
**          B92086 re-init CSRR fields for all RCBs, not just the one that
**          caused the escalation. (or we will get E_CL1039_LK_RELEASE_BAD_PARAM
**          errors trying to release locks that were released during escalation.
**      22-sep-98 (stial01)
**          dm0p_escalate() Added flags parameter, check for escalate LK_NOWAIT
**      02-feb-98 (stial01)
**          dm0p_escalate() Check for LK_BUSY 
**	20-Aug-2002 (jenjo02)
**	    Support TIMEOUT=NOWAIT, new LK_UBUSY status from LKrequest().
**	    Write E_DM9066_LOCK_BUSY info message if DMZ_SES_MACRO(35).
**	    Return E_DM006B_NOWAIT_LOCK_BUSY.
**	12-Feb-2003 (jenjo02)
**	    Delete DM006B, return E_DM004D_LOCK_TIMER_EXPIRED instead.
**      29-Sep-2004 (fanch01)
**          Conditionally add LK_LOCAL flag if database/table is confined
**	    to one node.
**      15-dec-2006 (huazh01)
**          Mark the DMP_RCB for replicator's secondary index to table 
**          level locked before releasing all locks. This prevents 
**          E_CL1039_LK_RELEASE_BAD_PARAM and fixes bug 117355. 
**	20-Apr-2007 (kibro01) b118116
**	    Amended bug fix for b117355 to check that tcb_rq_next list
**	    is not empty (marked by containing its own address!).
*/
DB_STATUS
dm0p_escalate(
DMP_RCB            *rcb,
i4             flags,
i4            *err_code)
{
    CL_SYS_ERR          sys_err;
    LK_LOCK_KEY		lockkey;
    DMP_RCB		*r = rcb;
    DMP_TCB		*t = r->rcb_tcb_ptr->tcb_parent_tcb_ptr;
    DML_XCB		*x = r->rcb_xcb_ptr;
    i4		lock_mode;
    DB_STATUS		status;
    bool		lock_escalated;
    DB_STATUS		ret_status = E_DB_OK;
    i4             lock_action;

    /*	Check to see if we are already table locking. */

    if (r->rcb_lk_type == RCB_K_TABLE)
	return (E_DB_OK);

    /*	Construct the key for converting the table lock. */

    lockkey.lk_type = LK_TABLE;
    lockkey.lk_key1 = (i4)t->tcb_dcb_ptr->dcb_id;
    lockkey.lk_key2 = t->tcb_rel.reltid.db_tab_base;
    lockkey.lk_key3 = 0;
    lockkey.lk_key4 = 0;
    lockkey.lk_key5 = 0;
    lockkey.lk_key6 = 0;

    /*
    **	Scan the RCB's looking for RCB for common base table.
    **	Calculate the appropriate table level lock mode to set.
    */

    lock_mode = LK_S;
    if (r->rcb_lk_mode == LK_IX || r->rcb_lk_mode == LK_SIX)
	lock_mode = LK_X;
    else if (x)
    {
	DML_XCB	    *next_r_queue;
	DMP_RCB	    *next_rcb;

	/* Scan for RCB in this transaction with same base TCB */

	for (next_r_queue = (DML_XCB *) x->xcb_rq_next;
	    next_r_queue != (DML_XCB *) &x->xcb_rq_next;
	    next_r_queue = next_r_queue->xcb_q_next)
	{
	    /*  Calculate RCB starting address. */

	    next_rcb = (DMP_RCB *)((char *)next_r_queue -
		((char *)&((DMP_RCB*)0)->rcb_xq_next));

	    if (next_rcb->rcb_tcb_ptr->tcb_parent_tcb_ptr != t)
		continue;
	    if (next_rcb->rcb_lk_mode == LK_IX || 
		next_rcb->rcb_lk_mode == LK_SIX)
	    {	
		lock_mode = LK_X;
		break;
	    }
	}
    }

    lock_action = LK_IGNORE_LOCK_LIMIT;
    if (flags & DM0P_NOWAIT)
	lock_action |= LK_NOWAIT;

    if (!(r->rcb_tcb_ptr->tcb_table_io.tbio_cache_flags & TBIO_CACHE_MULTIPLE))
    {
	/*
	** Note, as this is for logical locks, not the buffer locks, we
	** can assert LK_LOCAL even if this is a core catalog.
	*/
        lock_action |= LK_LOCAL;
    }

    /*	We now have the new mode in lock_mode. */
    status = LKrequest(lock_action, r->rcb_lk_id, &lockkey, lock_mode, 
	(LK_VALUE *)0, (LK_LKID *)0, r->rcb_timeout, &sys_err);

    if (status != OK)
    {
	lock_escalated = FALSE;
	ret_status = E_DB_ERROR;
	/*
	**	Audit failure of lock escalation
	*/
	if ( dmf_svcb->svcb_status & (SVCB_C2SECURE) )
	    _VOID_ dma_write_audit(
		    SXF_E_RESOURCE,
		    SXF_A_FAIL | SXF_A_ESCALATE,
		    (char*)&t->tcb_rel.relid,	/* Table name */
		    sizeof(t->tcb_rel.relid),	/* Table name */
		    &t->tcb_rel.relowner,
		    I_SX270E_LOCK_ESCALATE,
		    FALSE, /* Not force */
		    err_code, NULL);

	if (status == LK_DEADLOCK)
	{
	    if ( dmf_svcb->svcb_status & (SVCB_C2SECURE) )
		 _VOID_ dma_write_audit(
		    SXF_E_RESOURCE,
		    SXF_A_FAIL | SXF_A_ESCALATE,
		    (char*)&t->tcb_rel.relid,	/* Table name */
		    sizeof(t->tcb_rel.relid),	/* Table name */
		    &t->tcb_rel.relowner,
		    I_SX2736_TABLE_DEADLOCK,
		    FALSE, /* Not force */
		    err_code, NULL);

	    ule_format(E_DM9044_ESCALATE_DEADLOCK, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 3,
		sizeof(DB_TAB_NAME), &t->tcb_rel.relid,
		sizeof(DB_DB_NAME), &t->tcb_dcb_ptr->dcb_name, 0, lock_mode);

	    /*
	    ** DM304 enables tracing of lockstat on table escalation deadlocks.
	    ** This occurs always in the RCP, as it should never encounter 
	    ** deadlocks.
	    */
	    if ((DMZ_LCK_MACRO(4)) || 
		(rcb->rcb_tcb_ptr->tcb_dcb_ptr->dcb_status & DCB_S_RECOVER))
	    {
		dmd_lock_info(DMTR_LOCK_LISTS | DMTR_LOCK_USER_LISTS |
			DMTR_LOCK_SPECIAL_LISTS | DMTR_LOCK_RESOURCES );
	    }

	    *err_code = E_DM0042_DEADLOCK;
	}
	else if (status == LK_TIMEOUT)
	{
	    if ( dmf_svcb->svcb_status & (SVCB_C2SECURE) )
		 _VOID_ dma_write_audit(
		    SXF_E_RESOURCE,
		    SXF_A_FAIL | SXF_A_ESCALATE,
		    (char*)&t->tcb_rel.relid,	/* Table name */
		    sizeof(t->tcb_rel.relid),	/* Table name */
		    &t->tcb_rel.relowner,
		    I_SX2738_LOCK_TIMEOUT,
		    FALSE, /* Not force */
		    err_code, NULL);

          ule_format(E_DM9043_LOCK_TIMEOUT, &sys_err, ULE_LOG, NULL,
              (char *)0, (i4)0, (i4 *)0, err_code, 6,
              sizeof(DB_TAB_NAME), &t->tcb_rel.relid,
              sizeof(DB_DB_NAME), &t->tcb_dcb_ptr->dcb_name, 0, lock_mode,
              sizeof(DB_OWN_NAME), t->tcb_rel.relowner.db_own_name,
              0, "ESCALATE", 0, sys_err.moreinfo[0].data.string);
	    *err_code = E_DM004D_LOCK_TIMER_EXPIRED;
	}
	else if (status == LK_INTERRUPT)
	{
	    *err_code = E_DM0065_USER_INTR;
	}
	else if (status == LK_INTR_GRANT)
	{
	    /* in the case of INTR_GRANT, the escalation will have been
	    ** successful, so keep track of the interrupt and allow the
	    ** routine to remove all the page locks.  Still return with
	    ** the E_DM0065_USER_INTR error (and status of E_DB_ERROR).
	    */

	    *err_code = E_DM0065_USER_INTR;
	    lock_escalated = TRUE;
	}
	else if ( status == LK_UBUSY )
	{
	    /* Output error message iff DMZ_SES_MACRO(35) on in SCB */
	    if ( x && DMZ_MACRO(x->xcb_scb_ptr->scb_trace, 35) )
	    {
		ule_format(E_DM9066_LOCK_BUSY, &sys_err, ULE_LOG, NULL,
		      (char *)0, (i4)0, (i4 *)0, err_code, 6,
		      sizeof(DB_TAB_NAME), &t->tcb_rel.relid,
		      sizeof(DB_DB_NAME), &t->tcb_dcb_ptr->dcb_name, 0, lock_mode,
		      sizeof(DB_OWN_NAME), t->tcb_rel.relowner.db_own_name,
		      0, "ESCALATE", 0, sys_err.moreinfo[0].data.string);
	    }
	    *err_code = E_DM004D_LOCK_TIMER_EXPIRED;
	}
	else if (status == LK_BUSY && (lock_action & LK_NOWAIT)) 
	{
	    *err_code = E_DM004C_LOCK_RESOURCE_BUSY;
	}
	else
	{
	    ule_format(status, &sys_err, ULE_LOG, NULL, (char *)0, (i4)0,
		(i4 *)0, err_code, 0);
	    ule_format(E_DM9C8C_BM_ESCALATE_FAIL, (CL_ERR_DESC *)0, ULE_LOG,
		NULL, (char *)0, (i4)0, (i4 *)0, err_code, 2,
		sizeof(DB_TAB_NAME), &t->tcb_rel.relid,
		sizeof(DB_DB_NAME), &t->tcb_dcb_ptr->dcb_name);
	    *err_code = E_DM920A_BM_ESCALATE_LOCK_ERR;
	    if ( status == LK_BUSY )
		ret_status = E_DB_ERROR;
	    else
		ret_status = E_DB_FATAL;
	}

	if (!lock_escalated)
	    return(ret_status);
    }

    /* 
    ** If it gets this far, a table lock on a valid
    ** table has been obtained, trace it if locktracing
    ** is set.  This is not done above since the table
    ** name is not known until TCB found.
    */

    /*
    **	Audit success of lock escalation
    */
    if ( dmf_svcb->svcb_status & (SVCB_C2SECURE) )
	status = dma_write_audit(
	    SXF_E_RESOURCE,
	    SXF_A_SUCCESS | SXF_A_ESCALATE,
	    (char*)&t->tcb_rel.relid,	/* Table name */
	    sizeof(t->tcb_rel.relid),	/* Table name */
	    &t->tcb_rel.relowner,
	    I_SX270E_LOCK_ESCALATE,
	    FALSE, /* Not force */
	    err_code, NULL);

    if (DMZ_SES_MACRO(1))
	dmd_lock(&lockkey, r->rcb_lk_id, LK_REQUEST, LK_LOGICAL, 
	    lock_mode, r->rcb_timeout, 
	    &r->rcb_tran_id, &t->tcb_rel.relid,
	    &t->tcb_dcb_ptr->dcb_name);    

    /*
    **	Now scan RCB's for this transaction and mark them as
    **	table level locked.
    */

    r->rcb_lk_mode = lock_mode;
    r->rcb_lk_type = RCB_K_TABLE;
    r->rcb_state |= RCB_READAHEAD;
    if (x)
    {
	DML_XCB	    *next_r_queue;
	DMP_RCB	    *next_rcb;

	/* Scan for RCB in this transaction with same base TCB */

	for (next_r_queue = (DML_XCB*) x->xcb_rq_next;
	    next_r_queue != (DML_XCB*) &x->xcb_rq_next;
	    next_r_queue = next_r_queue->xcb_q_next)
	{
	    /*  Calculate RCB starting address. */

	    next_rcb = (DMP_RCB *)((char *)next_r_queue -
		((char *)&((DMP_RCB*)0)->rcb_xq_next));

	    if (next_rcb->rcb_tcb_ptr->tcb_parent_tcb_ptr != t
		&& next_rcb->rcb_tcb_ptr != t)
		continue;
	    next_rcb->rcb_lk_mode = lock_mode;
	    next_rcb->rcb_lk_type = RCB_K_TABLE;
	    next_rcb->rcb_state |= RCB_READAHEAD;

	    /* 
	    ** Initialize all fields in the RCB related to the locks
	    ** we just released
	    */
	    if (next_rcb->rcb_state & RCB_CSRR_LOCK)
	    {
		next_rcb->rcb_locked_data = 0;
		next_rcb->rcb_data_lkid.lk_unique = 
			next_rcb->rcb_data_lkid.lk_common = 0;
		next_rcb->rcb_locked_leaf = 0;
		next_rcb->rcb_leaf_lkid.lk_unique = 
			next_rcb->rcb_leaf_lkid.lk_common = 0;
		next_rcb->rcb_row_lkid.lk_unique = 
			next_rcb->rcb_row_lkid.lk_common = 0;
		next_rcb->rcb_base_lkid.lk_unique = 
			next_rcb->rcb_base_lkid.lk_common = 0;
		next_rcb->rcb_csrr_flags = 0;
	    }
	}
    }

    /* bug 117355: 
    ** Mark the DMP_RCB for replicator's secondary index to table
    ** to table level lock. 
    ** bug 118116:
    ** Add in check that the tcb_rq_next is not the address of itself, which
    ** is used for a zero-length list, which can happen if the lock which is
    ** escalated is the secondary index of the rep-table, which has no 
    ** indices of its own (kibro01)
    */
    if (t->tcb_reptab == TCB_REPTAB && 
        t->tcb_rel.relstat & TCB_IDXD &&
	(PTR)t->tcb_iq_next->tcb_rq_next != (PTR)&t->tcb_iq_next->tcb_rq_next)
    {
	DMP_RCB	    *next_rcb;

        next_rcb = t->tcb_iq_next->tcb_rq_next;

        if (next_rcb->rcb_tcb_ptr && 
            next_rcb->rcb_tcb_ptr->tcb_parent_tcb_ptr == t)
	{
	   next_rcb->rcb_lk_mode = lock_mode;
	   next_rcb->rcb_lk_type = RCB_K_TABLE;
	   next_rcb->rcb_state |= RCB_READAHEAD;

	   if (next_rcb->rcb_state & RCB_CSRR_LOCK)
	   {
              next_rcb->rcb_locked_data = 0;
	      next_rcb->rcb_data_lkid.lk_unique =
                      next_rcb->rcb_data_lkid.lk_common = 0;
              next_rcb->rcb_locked_leaf = 0;
              next_rcb->rcb_leaf_lkid.lk_unique = 
                      next_rcb->rcb_leaf_lkid.lk_common = 0;
              next_rcb->rcb_row_lkid.lk_unique = 
                      next_rcb->rcb_row_lkid.lk_common = 0;
              next_rcb->rcb_base_lkid.lk_unique = 
              next_rcb->rcb_base_lkid.lk_common = 0;
              next_rcb->rcb_csrr_flags = 0;
	   }
        }
    }

    /*
    **  Now release all the page, row, and value held by this transaction. 
    */

    lockkey.lk_type = LK_PAGE;
    status = LKrelease(LK_PARTIAL, r->rcb_lk_id, (LK_LKID *)0,
	(LK_LOCK_KEY *)&lockkey, (LK_VALUE *)0, &sys_err);

    lockkey.lk_type = LK_ROW;
    status = LKrelease(LK_PARTIAL, r->rcb_lk_id, (LK_LKID *)0,
	(LK_LOCK_KEY *)&lockkey, (LK_VALUE *)0, &sys_err);

    lockkey.lk_type = LK_VAL_LOCK;
    status = LKrelease(LK_PARTIAL, r->rcb_lk_id, (LK_LKID *)0,
	(LK_LOCK_KEY *)&lockkey, (LK_VALUE *)0, &sys_err);

    if (status != OK)
    {
	ule_format(status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 0);
	ule_format(E_DM9064_LK_ESCALATE_RELEASE, &sys_err, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, err_code, 3,
	    sizeof(DB_TAB_NAME), &t->tcb_rel.relid,
	    sizeof(DB_DB_NAME), &t->tcb_dcb_ptr->dcb_name, 
	    0, r->rcb_lk_id);
	*err_code = E_DM920A_BM_ESCALATE_LOCK_ERR;
	return (E_DB_FATAL);
    }

    /* 
    ** Initialize all fields in the RCB related to the locks we just released.
    */
    if (r->rcb_state & RCB_CSRR_LOCK)
    {
	r->rcb_locked_data = 0;
	r->rcb_data_lkid.lk_unique = r->rcb_data_lkid.lk_common = 0;
	r->rcb_locked_leaf = 0;
	r->rcb_leaf_lkid.lk_unique = r->rcb_leaf_lkid.lk_common = 0;
	r->rcb_row_lkid.lk_unique = r->rcb_row_lkid.lk_common = 0;
	r->rcb_base_lkid.lk_unique = r->rcb_base_lkid.lk_common = 0;
	r->rcb_csrr_flags = 0;
    }

    if (DMZ_SES_MACRO(1))
	  dmd_lock(&lockkey, r->rcb_lk_id, LK_RELEASE, LK_PARTIAL, 
	      lock_mode, r->rcb_timeout, 
	      &r->rcb_tran_id, &t->tcb_rel.relid,
	      &t->tcb_dcb_ptr->dcb_name);        
    return (ret_status);
}

/*{
** Name: dm0p_force_pages	- Release pages at end of Transaction.
**
** Description:
**      At end of transaction, any pages that are held in the cache owned
**	by this transaction are taken off of the transaction list.
**
**	If pages must be forced prior to commit (Non Fast Commit) then dirty
**	pages are written to disk.
**
**	If Database is opened with Fast Commit protocol, then dirty pages do
**	not need to be forced prior to the commit, but they still need to be
**	taken off the transaction queue.
**
**	When called with the DM0P_FORCE option, then modified pages are
**	written even if Fast Commit is in use.
**
**	When called with the DM0P_TOSS option, then all pages owned by this
**	transaction are tossed (and written if modified).
**
**	Unmodified pages are also released but never need to be written.
**	(Note that the buffer manager uses special protocols for pages
**	fixed by transactions in read mode.  The page is listed as "owned"
**	by the last transaction which unfixed it.  It is possible for one
**	transaction to "steal" a page from another transaction's list.)
**	
** Inputs:
**      tran_id                         The transaction id to search with.
**	log_id				The logging system transaction id.
**	action				0 - pages disowned from transaction.
**					DM0P_FORCE - force all dirty pages
**					    owned by transaction even if
**					    using fast commit.
**	action				DM0P_TOSS - toss all pages owned by
**					    this transaction from the cache
**					    (pages are written if modified).
** Outputs:
**      err_code                        The reason for error status.
**	Returns:
**	    E_DB_OK
**	    E_DB_FATAL
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	15-oct-1985 (derek)
**          Created new for Jupiter.
**	22-Jan-1987 (ac)
**	    Added code for readahead support.
**	31-jun-1988 (rogerk)
**	    Added action parameter for fast commit.
**	31-aug-1988 (rogerk)
**	    Return correct status if encounter warning.
**	28-feb-1989 (rogerk)
**	    Added support for Shared Buffer Manager.
**	    Look up tcb for force_page to use.
**      11-sep-1990 (jonb)  
**	    Integrate ingres6301 change 280136 (rogerk):
**            Pass buf_tcb_stamp value to dm2t_get_tcb_ptr.
**	3-may-1991 (bryanp)
**	    Make dmd_checks more useful by logging detailed error information
**	    before calling dmd_check.
**	    Call dbg_dm9301 before dmd_checking with E_DM9301.
**	9-mar-1992 (bryanp)
**	    Temporary table pages are not forced, merely disowned.
**	26-oct-1992 (rogerk)
**	    Reduced logging project: 
**	      - changed references to tcb fields to use new tcb_table_io 
**		structure fields.  
**	      - Changed from old dm2t_get_tcb_ptr to new fix_tableio_ptr.
**	      - Added check to bypass group operations with partial tcb's.
**	      - Added checks to see if buffer must be forced BEFORE looking
**		up the table descriptor (for efficiency).
**	      - Added TOSS option to allow this routine to be called during
**		in place of old dm0p_tran_invalidate.
**	23-aug-1993 (rachael)
**	    Restart at the beginning of the transaction queue when force_page
**	    returns  E_DB_WARN.
**	    When a page is updated by a transaction, it is placed on
**	    that transaction's page queue, meaning it is owned by that
**	    transaction.  This makes it easy to find all pages touched
**	    by a transaction.  There is an array of hash buckets (64)
**	    used to ease locating a particular transaction's page queue;
**	    more than one transaction can hash to the same bucket.
**
**	    At the end of a transaction, pages in the cache owned by the
**	    transaction are taken off the transaction list.  In a non-fast
**	    commit server, dirty pages are written to disk; in a fast-commit
**	    server they need not be written to disk but still need to 
**	    be taken off the transaction queue. The routine that
**	    performs this work is dm0p\dm0p_force_pages.
**
**	    dm0p\dm0p_force_pages computes the hash bucket for this
**	    transaction, takes the buffer manager mutex and walks the
**	    transaction page queue looking for all pages belonging to
**	    this transaction.  When one is found, dm0p\force_page is
**	    called to write the page to disk. Dm0p\force_page checks
**	    to see if the buffer is busy. The buffer is busy if it is
**	    being written out by another thread, e.g., write-behind
**	    thread, fast commit thread, or if it is being written to
**	    disk to make room so another page can be read in.
**
**	    If the buffer is found to be busy, the buffer manager
**	    mutex is dropped, dm0p\force_page waits for the other
**	    session to finish, then retakes the buffer_mutex and
**	    returns to dm0p\dm0p_force_pages with status set to
**	    E_DB_WARN.  This is where the the problem occurs -- since
**	    the buffer manager mutex was released, the ordering of
**	    the transaction list may have been altered and we should
**	    start back at the beginning of transaction bucket.
**	    However, we skipped this step and reprocessed the same
**	    buffer, following its now stale pointer to what had been
**	    the next page in the queue.  This lead to following the
**	    wrong chain of pages and skipping over the pages
**	    belonging to our transaction.
**
**	    Since the buffer manager mutex had been dropped, we need to restart
**	    our search at the beginning of the transaction list.
**	22-feb-1994 (andys)
**	    Integrate recent changes from ingres63p:
**		17-jan-1994 (andys)
**	    	    Add diagnostics for bug 55079.
**	    	    Trace point dm429 will check if transaction queue is looped.
**	    	    Trace point dm430 will dump BM if dm429 finds a loop.
**	    Note that 6.4's trace point dm428 becomes dm430 in 6.5
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**      01-aug-1996 (nanpr01 for ICL itb)
**	    Added the gather write and async io amendments.
**	14-Mar-1997 (jenjo02)
**	    Moved reloading of tran_chain from queue head to after the mutex
**	    is taken (after call to txnq_delete()).
**      21-may-1997 (stial01)
**          dm0p_force_pages() Skip any buffers with IX lock on the xn queue
**	28-Aug-1997 (jenjo02)
**	    Redid group-busy logic, in general, lock group, set busy status, 
**	    unlock group, then lock buffer.
**	10-Dec-1997 (jenjo02)
**	    Keep reusing the TBIO as long as possible instead of unfixing it
**	    after every force. Search hash chain using SHARE semaphore.
**	26-Jan-1997 (jenjo02)
**	    Refined txn queue search. Do it without taking the chain mutex.
**	10-Jan-2000 (jenjo02)
**	    Removed use of gforce_page(). This was bad as it tossed all
**	    the group pages from the cache, including pages which don't
**	    belong to the transaction which is ending! Instead use
**	    force_page(); it will do the right thing and leave even
**	    group pages cached.
**	12-Mar-2003 (jenjo02)
**	    Release buf_mutex before calling unfix_tableio_ptr(),
**	    retry buffer to avoid rare E_CL2517_CS_SMPR_DEADLOCK
**	    error in fault_page() (B109819).
**	19-Sep-2006 (jonj)
**	    Select only buffers matching log_id + tran_id.
**	    Shared transactions will have the same tran_id but 
**	    different log_id's and we sometimes want
**	    only those buffers belonging to a specific 
**	    log_id/tran_id pair, specifically during 
**	    LOGFULL_COMMIT involving shared transactions.
*/
DB_STATUS
dm0p_force_pages(
i4		    lock_list,
DB_TRAN_ID          *tran_id,
i4		    log_id,
i4		    action,
i4             	    *err_code)
{
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BSTAT	    *lbm_stats;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DB_STATUS	    status = E_DB_OK;
    DM0P_BUFFER	    *b;
    DMP_TABLE_IO    *tbio = NULL;
    DM0P_BLQ_HEAD   *tran_bucket;
    DM0P_BID	    *tran_chain;
    DM0P_BID	    next_bid;
    i4	    	    bucket;
    DM0P_BMCB       *bm;
    i4	    	    num_bufs = 0;
    i4	    	    num_other_bufs = 0;
    i4	    	    num_force_warns = 0;
    i4	    	    num_waits = 0;
    i4	    	    num_forces = 0;
    i4	    	    num_removes = 0;
    i4	    	    force_action;
    i4         	    dm429_count = 0;    /* number of buffers in tx q so far   */
    i4	    	    gw_queued = 0;
    
    /* Compute the tran hash bucket to queue this page. */
    bucket = tran_id->db_low_tran & (BMCB_TRAN_MAX -1);
    tran_bucket = &bm_common->bmcb_txnq_buckets[bucket];

    /*
    ** Scan the transaction bucket chain looking for any buffers belonging
    ** to this transaction/log_id pair. 
    */
    next_bid   = tran_bucket->blqh_bid;

    while (next_bid.bid_index != BUF_ENDLIST && status == E_DB_OK)
    {
	b = &lbm->lbm_buffers[next_bid.bid_cache][next_bid.bid_index];

	if ((b->buf_tran_id.db_low_tran != tran_id->db_low_tran) ||
	    (b->buf_tran_id.db_high_tran != tran_id->db_high_tran) ||
	    b->buf_log_id != log_id ||
	    b->buf_txnq_bucket != bucket)
	{
	    num_other_bufs++;
	    if (DMZ_BUF_MACRO(29))
		dbg_dm429(&dm429_count, lock_list, tran_id, log_id, action,
				num_bufs, num_other_bufs, tran_bucket);
	    if (b->buf_txnq_bucket == bucket)
		next_bid = b->buf_txnq.blq_next;
	    else
		next_bid = tran_bucket->blqh_bid;
	    continue;
	}

	/*
	** Lock the buffer, then check it again.
	*/
	dm0p_mlock(SEM_EXCL, &b->buf_mutex);

	/* Extract the txn queue forward pointers */
	next_bid = b->buf_txnq.blq_next;

	if (b->buf_tran_id.db_low_tran != tran_id->db_low_tran ||
	    b->buf_tran_id.db_high_tran != tran_id->db_high_tran ||
	    b->buf_log_id != log_id ||
	    b->buf_txnq_bucket != bucket)
	{
	    dm0p_munlock(&b->buf_mutex);
	    num_other_bufs++;
	    if (b->buf_txnq_bucket != bucket)
		next_bid = tran_bucket->blqh_bid;
	    continue;
	}

	num_bufs++;

	bm = lbm->lbm_bmcb[b->buf_id.bid_cache];
	lbm_stats = &lbm->lbm_stats[b->buf_id.bid_cache];

	/*
	** If possible, just remove the buffer from the transaction list.
	**
	** If Fast Commit is in use and the buffer needs not be forced
	** to the database, then we can just remove the buffer from our
	** transaction queue and continue.  Otherwise we must fall through
	** to look up the Table Descriptor and force the page.
	**
	** (ICL phil.p)
	** DMCM buffers should be treated as fast commit, they will not
	** have BUF_WCOMMIT set.
	**
	** If the DM0P_FORCE action is specified, then force all modified
	** pages even if Fast Commit is in use.
	**
	** (Note that we fall through if the buffer is marked fixed so
	** we will have the tabio for the dmd_check below)
	*/
	if (((action == 0) &&
		( ! (b->buf_sts & BUF_WCOMMIT)) &&
		( ! (b->buf_sts & BUF_FIXED))) ||
	    ( (b->buf_sts & BUF_INTENT_LK)) ||
	    ( ! (b->buf_sts & BUF_VALID)))
	{
	    num_removes++;
	    txnq_delete(b);

	    if (DMZ_BUF_MACRO(29))
		dbg_dm429(&dm429_count, lock_list, tran_id, log_id, action,
				num_bufs, num_other_bufs, tran_bucket);

	    dm0p_munlock(&b->buf_mutex);
	    continue;
	}

	/*
	** Find the TBIO for this buffer.  It must exist since this transaction
	** is being executed by this server.  If the TBIO for this table had
	** been reclaimed, then all pages for this transaction would have
	** had to have been forced and taken off of the transaction queue.
	**
	** If the TBIO is currently busy, the buffer manager mutex is released
	** while the fix routine waits for it.  We must then restart the loop
	** at the beginning of the transaction list.  Note that while we are 
	** waiting for the TBIO, the buffer we are currently examining may be 
	** tossed by another thread.
	*/
	if (tbio &&
	    (b->buf_dbid != tbio->tbio_dcbid ||
	     b->buf_tabid.db_tab_base != tbio->tbio_reltid.db_tab_base ||
	     b->buf_tabid.db_tab_index != tbio->tbio_reltid.db_tab_index))
	{
	    /* Wrong TBIO, release buffer and TBIO, retry buffer */
	    next_bid = b->buf_id;
	    dm0p_munlock(&b->buf_mutex);
	    unfix_tableio_ptr(&tbio, lock_list, log_id, (i4)__LINE__);
	    continue;
	}

	if (tbio == NULL)
	{
	    /* Can't wait for busy TBIO if we have uncompleted gatherwrites */
	    if (gw_queued)
	    {
		status = fix_tableio_ptr(b, 0, lock_list,
		    log_id, &tbio, err_code);
		/* If we'd have to wait, force gatherwrites, then retry */
		if ( status == E_DB_WARN )
		{
		    dm0p_munlock(&b->buf_mutex);
		    if ( dm2f_force_list() )
		    {
			status = E_DB_ERROR;
			break;
		    }
		}
	    }
	    else
	    {
		status = fix_tableio_ptr(b, 
		    (i4)(DM0P_TBIO_WAIT | DM0P_TBIO_MUTEX_HELD), lock_list, 
		     log_id, &tbio, err_code);
	    }

	    if (DB_FAILURE_MACRO(status))
	    {
		/*
		** Log any errors, then continue to DMDCHECK below since no
		** tbio was returned.
		*/
		ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL, 
		    (char *)0, (i4)0, (i4 *)0, err_code, 0);
	    }
	    if (status == E_DB_WARN)
	    {
		/* TCB was busy - retry */
		num_waits++;
		if (DMZ_BUF_MACRO(29))
		    dm429_count = 0;
		/* buffer's mutex unlocked by fix_tableio_ptr() */
		/* retry this same buffer */
		next_bid = b->buf_id;
		status = E_DB_OK;
		continue;
	    }
	}

	/*
	** Make sure page is not fixed.  There is a DMF protocol bug if some
	** page has been left fixed at the end of the transaction.
	*/
	if ((b->buf_sts & BUF_FIXED))
	{
	    tran_chain = &b->buf_txnq.blq_next;
	    ule_format(E_DM9C01_FIXED_BUFFER_INFO, (CL_SYS_ERR *)0,
			ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 6,
			0, b->buf_lock_key.lk_key4,
			0, b->buf_tabid.db_tab_base,
			0, b->buf_tabid.db_tab_index,
			0, b->buf_sts,
			(tbio != 0 ? sizeof(DB_DB_NAME): sizeof("UNKNOWN") - 1),
			(tbio != 0 ? (char *)tbio->tbio_dbname: "UNKNOWN"),
			0, (BUF_FIXED | BUF_MUTEX) );
	    dbg_dm9301(b, tbio, action, tran_id, lock_list, log_id, tran_bucket,
			tran_chain, num_bufs, num_forces, 0,
			num_waits, num_other_bufs,
			num_force_warns, num_removes, status);
	    dmd_check(E_DM9301_DM0P_FORCE_PAGES);
	}

	if (tbio == NULL)
	{
	    tran_chain = &b->buf_txnq.blq_next;
	    ule_format(E_DM9C02_MISSING_TCB_INFO, (CL_SYS_ERR *)0,
			ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 4,
			0, b->buf_lock_key.lk_key4,
			0, b->buf_tabid.db_tab_base,
			0, b->buf_tabid.db_tab_index,
			sizeof("UNKNOWN")-1, "UNKNOWN");
	    dbg_dm9301(b, tbio, action, tran_id, lock_list, log_id, tran_bucket,
			tran_chain, num_bufs, num_forces, 0,
			num_waits, num_other_bufs,
			num_force_warns, num_removes, status);
	    dmd_check(E_DM9301_DM0P_FORCE_PAGES);
	}


	/*
	** Temporary table pages can simply be disowned; they don't need
	** to be written unless the DM0P_FORCE flag was explicitly given.
	*/
	force_action = FP_CACHE;
	if (action == DM0P_TOSS)
	{
	    force_action = FP_TOSS;
	}
	if (tbio->tbio_temporary && (action != DM0P_FORCE))
	    force_action = FP_DISOWN;

	/*
	** Call force_page to write the page, move it from modified
	** queue, release cache lock, and change the cache lock value.
	**
	** If the buffer is part of a group, we could in theory use
	** group force, but only if we first examined the other group
	** pages to properly set the buffer actions, etc so that we only
	** force the pages "belonging" to this transaction.
	**
	** force_page() will do the right thing, modifying
	** group state as needed and leaving the group page(s) cached.
	*/
	num_forces++;
	status = force_page(b, tbio, force_action, lock_list, log_id,
	    BMCL_FORCE_PAGES, &gw_queued, err_code);

	/* If warn returned, then go back and retry this buffer. */
	
	if ( status )
	{
	    if (status == E_DB_WARN)
	    {
		num_force_warns++;
		/* retry this same buffer */
		next_bid = b->buf_id;
		status = E_DB_OK;
	    }
	    dm0p_munlock(&b->buf_mutex);
	}

	if (DMZ_BUF_MACRO(29))
	    dm429_count = 0;
    }

    /* Force any remaining queued GatherWrite buffers */
    if (gw_queued)
	status = dm2f_force_list();

    /* Toss any tbio we may still have laying around */
    if (tbio)
	unfix_tableio_ptr(&tbio, lock_list, log_id, (i4)__LINE__);

    if (DB_FAILURE_MACRO(status) && (*err_code > E_DM_INTERNAL))
    {
	ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, err_code, 0);
	*err_code = E_DM9C88_DM0P_FORCE_PAGES;
    }

    return (status);
}

/*{
** Name: dm0p_mutex - Get a semaphore on a page.
**
** Description:
**      This routine will get a semaphore on a page.
**
** Inputs:
**	rcb	    			The RCB.
**	page				Pointer to the page to get a semaphore.
**
** Outputs:
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	30-Mar-1987 (ac)
**	    Created for Jupiter.
**	28-feb-1989 (rogerk)
**	    Added support for Shared Buffer Manager.
**	3-may-1991 (bryanp)
**	    Make dmd_checks more useful by logging detailed error information
**	    before calling dmd_check.
**	17-oct-1991 (rogerk)
**	    Merged changes made by Derek during the file extend project.
**	    Added trace flags in xDEBUG code.
**	18-oct-1993 (rmuth)
**	    Add DMP_BMCB statistics.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**      06-may-1996 (nanpr01)
**	    All page header access was replaced by macros for New Page Format
**	    project.
**      21-may-1997 (stial01)
**          dm0p_mutex: Mutex not granted if buffer is locked
*/
VOID
dm0p_mutex(
DMP_TABLE_IO	*tbio,
i4		flags,
i4		lock_list,
DMPP_PAGE	**page)
{
    DM0P_BUFFER	    *b;
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BMCB       *bm = lbm->lbm_bmcb[tbio->tbio_cache_ix];
    i4	    page_index;
    i4	    err_code;
    DM0P_BSTAT	    *lbm_stats;

    if ((dmf_svcb->svcb_status & SVCB_SINGLEUSER) == 0)
    {
#ifdef xDEBUG
	if (DMZ_BUF_MACRO(3))
	TRdisplay("MUTEX_PAGE  : (%~t,%~t,%d,%dK) lk_id=%x\n",
	    sizeof(DB_DB_NAME), tbio->tbio_dbname,
	    sizeof(DB_TAB_NAME), tbio->tbio_relid, 
	    DMPP_VPT_GET_PAGE_PAGE_MACRO(tbio->tbio_page_type, *page), 
	    KSIZE(tbio->tbio_page_size), lock_list);
#endif

	/*
	** Find buffer descriptor for this page.  The buffer
	** descriptor for the i'th page is the i'th descriptor.
	** XXXX - Multiple Segments - XXXX
	** This won't work if page array is not contiguous.
	*/
	page_index = ((char *)*page - (char *)lbm->lbm_pages[tbio->tbio_cache_ix])
						/ tbio->tbio_page_size;

	b = &lbm->lbm_buffers[tbio->tbio_cache_ix][page_index];
    
	/* Lock the buffer */
	dm0p_mlock(SEM_EXCL, &b->buf_mutex);

	/*
	** Check the information stored in the buffer is consistent with
	** the page to get a semaphore.
	*/
	if (((flags & DM0P_MINCONS_PG) == 0) &&
	    (b->buf_lock_key.lk_key4 !=
		 DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, *page)) ||
	    (b->buf_tabid.db_tab_base != tbio->tbio_reltid.db_tab_base) ||
	    ((b->buf_sts & BUF_FIXED) == 0))
	{
	    ule_format(E_DM934D_DM0P_BAD_PAGE, (CL_SYS_ERR *)0, ULE_LOG,NULL,
			(char *)0, (i4)0, (i4 *)0, &err_code, 9,
			0, 
			DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, *page),
			0, b->buf_lock_key.lk_key4,
			0, tbio->tbio_reltid.db_tab_base,
			0, b->buf_tabid.db_tab_base,
			0, tbio->tbio_reltid.db_tab_index,
			0, b->buf_tabid.db_tab_index,
			0, b->buf_sts,
			sizeof(DB_DB_NAME), tbio->tbio_dbname,
			sizeof(DB_TAB_NAME), tbio->tbio_relid);
	    dmd_check(E_DM9303_DM0P_MUTEX);
	}

	for (;;)
	{
	    if ( ((b->buf_sts & BUF_MUTEX) == 0) &&
		 (((b->buf_sts & BUF_MLOCK) == 0) 
		|| (b->buf_lock_list == lock_list)))
	    {
		/* No one is holding the page mutex. */
		b->buf_sts |= BUF_MUTEX;
		break;
	    }

	    /*	Mutex is held.  Wait for the release and retry. */
	    lbm_stats = &lbm->lbm_stats[b->buf_id.bid_cache];

	    b->buf_sts |= BUF_MWAIT;
	    lbm_stats->bs_mwait[BMCB_PTYPES]++;
	    bm->bm_stats.bs_mwait[BMCB_PTYPES]++;
	    lbm_stats->bs_mwait[b->buf_type]++;
	    bm->bm_stats.bs_mwait[b->buf_type]++;

	    /* Set the wait for page mutex event before release the mutex. */

	    dm0p_eset(lock_list, DM0P_PGMUTEX_EVENT,
			b->buf_id.bid_cache, b->buf_id.bid_index, 0);
	    dm0p_munlock(&b->buf_mutex);

	    /* Wait for the unmutex event. */

	    dm0p_ewait(lock_list, DM0P_PGMUTEX_EVENT,
			b->buf_id.bid_cache, b->buf_id.bid_index, 0, 0);
	    dm0p_mlock(SEM_EXCL, &b->buf_mutex);
	}
	
	dm0p_munlock(&b->buf_mutex);
	return;
    }
}

/*{
** Name: dm0p_unmutex - Release a semaphore on a page.
**
** Description:
**      This routine will release a semaphore on a page.
**
** Inputs:
**	rcb	    			The RCB.
**	page				Pointer to the page to release a 
**					semaphore.
**
** Outputs:
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	30-Mar-1987 (ac)
**	    Created for Jupiter.
**	28-feb-1989 (rogerk)
**	    Added support for Shared Buffer Manager.
**	3-may-1991 (bryanp)
**	    Make dmd_checks more useful by logging detailed error information
**	    before calling dmd_check.
**	17-oct-1991 (rogerk)
**	    Merged changes made by Derek during the file extend project.
**	    Added trace flags in xDEBUG code.
**      06-may-1996 (nanpr01)
**	    All page header access was replaced by macros for New Page Format
**	    project.
**      21-may-1997 (stial01)
**          dm0p_unmutex: New flags parameter to lock/unlock the buffer
**      29-may-1997 (stial01)
**          dm0p_unmutex: zero out buf_lock_id when buffer is unlocked
*/
VOID
dm0p_unmutex(
DMP_TABLE_IO	*tbio,
i4		flags,
i4		lock_list,
DMPP_PAGE	**page)
{
    DM0P_BUFFER	    *b;
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BMCB       *bm = lbm->lbm_bmcb[tbio->tbio_cache_ix];
    i4	    page_index;
    i4	    err_code;

    if ((dmf_svcb->svcb_status & SVCB_SINGLEUSER) == 0)
    {
#ifdef xDEBUG
	if (DMZ_BUF_MACRO(3))
	TRdisplay("UNMUTEX_PAGE: (%~t,%~t,%d,%dK) lk_id=%x\n",
	    sizeof(DB_DB_NAME), tbio->tbio_dbname,
	    sizeof(DB_TAB_NAME), tbio->tbio_relid,
	    DMPP_VPT_GET_PAGE_PAGE_MACRO(tbio->tbio_page_type, *page), 
	    KSIZE(tbio->tbio_page_size), lock_list);
#endif

	/*
	** Find buffer descriptor for this page.  The buffer
	** descriptor for the i'th page is the i'th descriptor.
	** XXXX - Multiple Segments - XXXX
	** This won't work if page array is not contiguous.
	*/
	page_index = ((char *)*page - (char *)lbm->lbm_pages[tbio->tbio_cache_ix])
						/ tbio->tbio_page_size;

	b = &lbm->lbm_buffers[tbio->tbio_cache_ix][page_index];

	/* Lock the buffer  */
	dm0p_mlock(SEM_EXCL, &b->buf_mutex);

	/*
	** Check the information stored in the buffer is consistent with
	** the page to release a semaphore.
	*/

	if (((flags & DM0P_MINCONS_PG) == 0) &&
	    (b->buf_lock_key.lk_key4 !=
		 DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, *page)) ||
	    (b->buf_tabid.db_tab_base != tbio->tbio_reltid.db_tab_base) ||
	    ((b->buf_sts & BUF_FIXED) == 0)) 
	{
	    ule_format(E_DM934D_DM0P_BAD_PAGE, (CL_SYS_ERR *)0, ULE_LOG,NULL,
			(char *)0, (i4)0, (i4 *)0, &err_code, 9,
			0, 
			DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, *page),
			0, b->buf_lock_key.lk_key4,
			0, tbio->tbio_reltid.db_tab_base,
			0, b->buf_tabid.db_tab_base,
			0, tbio->tbio_reltid.db_tab_index,
			0, b->buf_tabid.db_tab_index,
			0, b->buf_sts,
			sizeof(DB_DB_NAME), tbio->tbio_dbname,
			sizeof(DB_TAB_NAME), tbio->tbio_relid);
	    dmd_check(E_DM9304_DM0P_UNMUTEX);
	}

	if ( ((b->buf_sts & BUF_MUTEX) == 0)
	    || ( ((b->buf_sts & BUF_MLOCK) && (b->buf_lock_list != lock_list))))
	{
	    ule_format(E_DM9C01_FIXED_BUFFER_INFO, (CL_SYS_ERR *)0,
			ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, &err_code, 6,
			0, b->buf_lock_key.lk_key4,
			0, b->buf_tabid.db_tab_base,
			0, b->buf_tabid.db_tab_index,
			0, b->buf_sts,
			sizeof(DB_DB_NAME), tbio->tbio_dbname,
			0, (BUF_FIXED | BUF_MUTEX) );
	    dmd_check(E_DMF003_MUTEX_UNLOCK);
	}

	/*
	** Lock buffer if requested
	** Always wake up waiters in case buffer manager is waiting for mutex
	** If this lock list ever unfixes this buffer, the lock is released
	** and a warning issued.
	*/
	if (flags & DM0P_MLOCK)
	{
	    b->buf_sts |= BUF_MLOCK;
	    b->buf_lock_list = lock_list;
	}
	else if (flags & DM0P_MUNLOCK)
	{
	    b->buf_sts &= ~(BUF_MLOCK);
	    b->buf_lock_list = 0;
	}

	b->buf_sts &= ~(BUF_MUTEX);
	if ( b->buf_sts & BUF_MWAIT )
	    awaken_buf_waiters(b, lock_list);
	dm0p_munlock(&b->buf_mutex);

	return;
    }
}

/*{
** Name: dm0p_lock_buf - Get a BUF_MLOCK on a page.
**
** Description:
**      This routine will get a BUF_MLOCK on a page.
**
** Inputs:
**	tbio				The TBIO.
**	lock_list			The lock list.
**	page				Pointer to the page to get a lock.
**
** Outputs:
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	11-Oct-1999 (jenjo02)
**	    Created.
*/
VOID
dm0p_lock_buf(
DMP_TABLE_IO	*tbio,
i4		lock_list,
DMPP_PAGE	**page)
{
    DM0P_BUFFER	    *b;
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BMCB       *bm = lbm->lbm_bmcb[tbio->tbio_cache_ix];
    i4	    	    page_index;
    i4	    	    err_code;
    DM0P_BSTAT	    *lbm_stats;

    if ((dmf_svcb->svcb_status & SVCB_SINGLEUSER) == 0)
    {
#ifdef xDEBUG
	if (DMZ_BUF_MACRO(3))
	TRdisplay("MUTEX_PAGE  : (%~t,%~t,%d,%dK) lk_id=%x\n",
	    sizeof(DB_DB_NAME), tbio->tbio_dbname,
	    sizeof(DB_TAB_NAME), tbio->tbio_relid, 
	    DMPP_VPT_GET_PAGE_PAGE_MACRO(tbio->tbio_page_type, *page), 
	    KSIZE(tbio->tbio_page_size), lock_list);
#endif

	/*
	** Find buffer descriptor for this page.  The buffer
	** descriptor for the i'th page is the i'th descriptor.
	** XXXX - Multiple Segments - XXXX
	** This won't work if page array is not contiguous.
	*/
	page_index = ((char *)*page - (char *)lbm->lbm_pages[tbio->tbio_cache_ix])
						/ tbio->tbio_page_size;

	b = &lbm->lbm_buffers[tbio->tbio_cache_ix][page_index];
    
	/* Lock the buffer */
	dm0p_mlock(SEM_EXCL, &b->buf_mutex);

	/*
	** Check the information stored in the buffer is consistent with
	** the page.
	*/
	if ((b->buf_lock_key.lk_key4 != 
		 DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, *page)) ||
	    (b->buf_tabid.db_tab_base != tbio->tbio_reltid.db_tab_base) ||
	    ((b->buf_sts & BUF_FIXED) == 0))
	{
	    ule_format(E_DM934D_DM0P_BAD_PAGE, (CL_SYS_ERR *)0, ULE_LOG,NULL,
			(char *)0, (i4)0, (i4 *)0, &err_code, 9,
			0, 
			DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, *page),
			0, b->buf_lock_key.lk_key4,
			0, tbio->tbio_reltid.db_tab_base,
			0, b->buf_tabid.db_tab_base,
			0, tbio->tbio_reltid.db_tab_index,
			0, b->buf_tabid.db_tab_index,
			0, b->buf_sts,
			sizeof(DB_DB_NAME), tbio->tbio_dbname,
			sizeof(DB_TAB_NAME), tbio->tbio_relid);
	    dmd_check(E_DM9303_DM0P_MUTEX);
	}

	/* Must wait if buffer is mutexed or MLOCKed by another thread */
	while ( b->buf_sts & BUF_MUTEX ||
		(b->buf_sts & BUF_MLOCK && b->buf_lock_list != lock_list) )
	{
	    /*	Mutex/lock is held.  Wait for the release and retry. */
	    lbm_stats = &lbm->lbm_stats[b->buf_id.bid_cache];

	    b->buf_sts |= BUF_MWAIT;
	    lbm_stats->bs_mwait[BMCB_PTYPES]++;
	    bm->bm_stats.bs_mwait[BMCB_PTYPES]++;
	    lbm_stats->bs_mwait[b->buf_type]++;
	    bm->bm_stats.bs_mwait[b->buf_type]++;

	    /* Set the wait for page mutex event before release the mutex. */

	    dm0p_eset(lock_list, DM0P_PGMUTEX_EVENT,
			b->buf_id.bid_cache, b->buf_id.bid_index, 0);
	    dm0p_munlock(&b->buf_mutex);

	    /* Wait for the unmutex event. */

	    dm0p_ewait(lock_list, DM0P_PGMUTEX_EVENT,
			b->buf_id.bid_cache, b->buf_id.bid_index, 0, 0);
	    dm0p_mlock(SEM_EXCL, &b->buf_mutex);
	}

	/* No one is holding the mutex or buffer lock. */
	b->buf_sts |= BUF_MLOCK;
	b->buf_lock_list = lock_list;
	
	dm0p_munlock(&b->buf_mutex);
	return;
    }
}

/*{
** Name: dm0p_unlock_buf - Release a BUF_MLOCK on a buffer.
**
** Description:
**      This routine will release a BUF_MLOCK on a page.
**
** Inputs:
**	tbio				The TBIO.
**	lock_list			The lock list.
**	page				Pointer to the page to release a lock.
**
** Outputs:
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	11-Oct-1999 (jenjo02)
**	    Created.
*/
VOID
dm0p_unlock_buf(
DMP_TABLE_IO	*tbio,
i4		lock_list,
DMPP_PAGE	**page)
{
    DM0P_BUFFER	    *b;
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BMCB       *bm = lbm->lbm_bmcb[tbio->tbio_cache_ix];
    DM0P_BSTAT	    *lbm_stats;
    i4	    	    page_index;
    i4	    	    err_code;

    if ((dmf_svcb->svcb_status & SVCB_SINGLEUSER) == 0)
    {
#ifdef xDEBUG
	if (DMZ_BUF_MACRO(3))
	TRdisplay("UNMUTEX_PAGE: (%~t,%~t,%d,%dK) lk_id=%x\n",
	    sizeof(DB_DB_NAME), tbio->tbio_dbname,
	    sizeof(DB_TAB_NAME), tbio->tbio_relid,
	    DMPP_VPT_GET_PAGE_PAGE_MACRO(tbio->tbio_page_type, *page), 
	    KSIZE(tbio->tbio_page_size), lock_list);
#endif

	/*
	** Find buffer descriptor for this page.  The buffer
	** descriptor for the i'th page is the i'th descriptor.
	** XXXX - Multiple Segments - XXXX
	** This won't work if page array is not contiguous.
	*/
	page_index = ((char *)*page - (char *)lbm->lbm_pages[tbio->tbio_cache_ix])
						/ tbio->tbio_page_size;

	b = &lbm->lbm_buffers[tbio->tbio_cache_ix][page_index];

	/* Lock the buffer  */
	dm0p_mlock(SEM_EXCL, &b->buf_mutex);

	/*
	** Check the information stored in the buffer is consistent with
	** the page to release a semaphore.
	*/

	if ((b->buf_lock_key.lk_key4 != 
		 DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, *page)) ||
	    (b->buf_tabid.db_tab_base != tbio->tbio_reltid.db_tab_base) ||
	    ((b->buf_sts & BUF_FIXED) == 0)) 
	{
	    ule_format(E_DM934D_DM0P_BAD_PAGE, (CL_SYS_ERR *)0, ULE_LOG,NULL,
			(char *)0, (i4)0, (i4 *)0, &err_code, 9,
			0, 
			DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, *page),
			0, b->buf_lock_key.lk_key4,
			0, tbio->tbio_reltid.db_tab_base,
			0, b->buf_tabid.db_tab_base,
			0, tbio->tbio_reltid.db_tab_index,
			0, b->buf_tabid.db_tab_index,
			0, b->buf_sts,
			sizeof(DB_DB_NAME), tbio->tbio_dbname,
			sizeof(DB_TAB_NAME), tbio->tbio_relid);
	    dmd_check(E_DM9304_DM0P_UNMUTEX);
	}

	/* If not MLOCKed by this lock list, error */
	if ( (b->buf_sts & BUF_MLOCK) == 0 ||
	       b->buf_lock_list != lock_list )
	{
	    ule_format(E_DM9C01_FIXED_BUFFER_INFO, (CL_SYS_ERR *)0,
			ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, &err_code, 6,
			0, b->buf_lock_key.lk_key4,
			0, b->buf_tabid.db_tab_base,
			0, b->buf_tabid.db_tab_index,
			0, b->buf_sts,
			sizeof(DB_DB_NAME), tbio->tbio_dbname,
			0, (BUF_FIXED | BUF_MUTEX) );
	    dmd_check(E_DMF003_MUTEX_UNLOCK);
	}

	/*
	** If mutexed, we must wait. Only the CP thread should
	** be able to mutex this buffer while it is in an 
	** MLOCK'ed state.
	*/
	while ( b->buf_sts & BUF_MUTEX )
	{
	    /*	Mutex/lock is held.  Wait for the release and retry. */
	    lbm_stats = &lbm->lbm_stats[b->buf_id.bid_cache];

	    b->buf_sts |= BUF_MWAIT;
	    lbm_stats->bs_mwait[BMCB_PTYPES]++;
	    bm->bm_stats.bs_mwait[BMCB_PTYPES]++;
	    lbm_stats->bs_mwait[b->buf_type]++;
	    bm->bm_stats.bs_mwait[b->buf_type]++;

	    /* Set the wait for page mutex event before release the mutex. */

	    dm0p_eset(lock_list, DM0P_PGMUTEX_EVENT,
			b->buf_id.bid_cache, b->buf_id.bid_index, 0);
	    dm0p_munlock(&b->buf_mutex);

	    /* Wait for the unmutex event. */

	    dm0p_ewait(lock_list, DM0P_PGMUTEX_EVENT,
			b->buf_id.bid_cache, b->buf_id.bid_index, 0, 0);
	    dm0p_mlock(SEM_EXCL, &b->buf_mutex);
	}

	b->buf_lock_list = 0;

	/* Wake up BUF_MLOCK waiters */
	b->buf_sts &= ~(BUF_MLOCK);
	if ( b->buf_sts & BUF_MWAIT )
	    awaken_buf_waiters(b, lock_list);
	dm0p_munlock(&b->buf_mutex);

	return;
    }
}

/*{
** Name: invalidate_page - Invalidate a page in buffer manager
**
** Description:
**      This routine will purge the page described by the arguments from the
**	buffer manager.
**
**	The buffer contents are discarded and the buffer is placed on the
**	free queue.  If this is a member of a group buffer, then
**	the group is shuffled to keep it on the correct group queue (fixed,
**	modified, or empty).
**
**	Note that if the page is in a group buffer, and invalidating it makes
**	the group empty, it (the group) is immediately liable to being grabbed
**	and used as soon as the group mutex is released.  Holding the buf
**	mutex isn't going to stop gfault_page!  Therefore, there's nothing
**	more that a caller can validly do to the buffer once we're done with
**	it (at least not in the general case), so we'll release the buf
**	mutex before returning.
**
**	Enter holding buffer's buf_mutex, return with it released.
**
** Inputs:
**	    buffer		The buffer to be purged.
**	    lock_list		Session lock list.
**	
** Outputs:
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	22-Aug-1987 (ac)
**	    Created for Jupiter.
**	28-feb-1989 (rogerk)
**	    Added support for Shared Buffer Manager.
**	30-oct-1989 (rogerk)
**	    Modified routine to better handle buffers that are in various
**	    states - fixed/modified/free/group.  Ignore modified parameter
**	    and check buffer status field.  Fixed bug that invalidation of
**	    fixed pages was not working.
**	22-feb-1994 (andys)
**	    Integrate recent changes from ingres63p:
**       	1-nov-1993 (rogerk & andys)
**          	    When we mark the buffer status as BUF_FREE make sure we wake
**          	    up any threads that are waiting  on this buffer. [bug 54128]
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	25-mar-96 (nick)
**	    Wake up any threads waiting on IO or FC for this buffer.
**	14-Sep-1996 (jenjo02)
**	    Caller must hold buffer's buf_mutex.
**	13-Feb-1997 (jenjo02)
**	    Corrected 22-feb-1994 and 25-mar-96 corrections to wake up waiting
**	    threads. Assignments of the form
**	              bool  result = (b->buf_sts & FLAG);
**	    produce correct results if and only if FLAG 
**	    is in the low-order byte of the status word; in all other cases
**	    the result will be zero. The compiler treats this
**	    as a simple assignment and casts b->buf_sts to a byte.
**	19-Jan-1998 (jenjo02)
**	    Remove buffer from table's hash queue when buffer is made free.
**	27-Oct-1999 (jenjo02)
**	    When tossing a page from the cache, invalidate its page number by
**	    setting it to -1.
**	28-Oct-1999 (nanpr01)
**	    Fixed X-integration. The fields referenced are not there in 
**	    Ingres II 2.5.
**	10-Jan-2000 (jenjo02)
**	    If tossing a valid group page, decrement group's g_valcount.
**	2-May-2005 (schka24)
**	    Finish buffer manipulations before move-group, as the latter might
**	    make the group free (and grabbable).  Remove unused param.
*/
static VOID
invalidate_page(
DM0P_BUFFER	*buffer,
i4		lock_list)
{
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BUFFER	    *b = buffer;
    DM0P_BMCB	    *bm = lbm->lbm_bmcb[b->buf_id.bid_cache];
    DM0P_BUFFER	    *buf_array = lbm->lbm_buffers[b->buf_id.bid_cache];
    DM0P_HASH_HEAD  *bucket_array = lbm->lbm_buckets[b->buf_id.bid_cache];
    DM0P_GROUP      *group_array;
    i4	    next_buf, prev_buf;
    DM0P_GROUP	    *group;
    DM0P_STATE_HEAD *sth;
    i4	    save_priority;
    i4	    save_sts;
    i4		    buf_is_on_state_queue;
    DB_TRAN_ID      tran;
    DMPP_PAGE       *page = (DMPP_PAGE *)((char *)lbm->lbm_pages[b->buf_id.bid_cache] +
						 b->buf_page_offset);

    /* Save buffer status so we can take off of the proper state queue */
    save_sts = b->buf_sts;

    /* Save priority so can take off of correct priority queue below. */
    save_priority = b->buf_priority;

    /* Is buffer on a state queue? */
    buf_is_on_state_queue = (b->buf_state.stq_next != 0 ||
			     b->buf_state.stq_prev != 0);

    /*
    ** Take buffer off of hash bucket chain.
    */
    if (b->buf_hash_bucket != BUF_NOBUCKET)
	hashq_delete(b, bucket_array, buf_array);

    /*
    ** Take buffer off of current transaction bucket.	
    */
    if (b->buf_txnq_bucket != BUF_NOBUCKET)
	txnq_delete(b);

    /*
    ** Take buffer off of table queue
    */
    if (b->buf_tabq_bucket != BUF_NOBUCKET)
	tabq_delete(b);

    /* Clobber pageno on the page */
    DMPP_VPT_SET_PAGE_PAGE_MACRO(b->buf_relpgtype, page, (DM_PAGENO)-1);

    /* Clobber tran id on the page */
    tran.db_low_tran = 0;
    tran.db_high_tran = 0;
    DMPP_VPT_SET_PAGE_TRAN_ID_MACRO(b->buf_relpgtype, page, tran);

    /* Clobber page_stat on the page */
    DMPP_VPT_INIT_PAGE_STAT_MACRO(b->buf_relpgtype, page, 0);

    /* Clear force page action and "owner", in case caller is complete-f-p */
    b->buf_fp_action = 0;
    b->buf_fp_llid = 0;

    /*
    ** Reset buffer header fields now that buf is off all queues.
    */
    b->buf_sts = BUF_FREE;
    b->buf_priority = 0;

    /*
    ** Take buffer off of its current state queue (free or modify).
    ** If the buffer is fixed, then it's on no state queue.
    ** If the buffer is a member of a group, it won't be on any 
    ** state queue.
    */
    if ( b->buf_owner == BUF_SELF )
    {
	if ((save_sts & BUF_FIXED) == 0)
	{
	    if (save_sts & BUF_MODIFIED)
		sth = &bm->bm_mpq[save_priority];
	    else
		sth = &bm->bm_fpq[save_priority];

	    dm0p_mlock(SEM_EXCL, &sth->sth_mutex);

	    next_buf = b->buf_state.stq_next;
	    prev_buf = b->buf_state.stq_prev;
	    if (next_buf != BUF_ENDLIST)
		buf_array[next_buf].buf_state.stq_prev = prev_buf;
	    else
		sth->sth_stq.stq_prev = prev_buf;
	    if (prev_buf != BUF_ENDLIST)
		buf_array[prev_buf].buf_state.stq_next = next_buf;
	    else
		sth->sth_stq.stq_next = next_buf;
	    sth->sth_count--;
	    
	    dm0p_munlock(&sth->sth_mutex);
	}
	else
	{
	    /*
	    ** Decrement a fixed buffer count. It doesn't matter
	    ** which queue we pick to do this, so just pick
	    ** the free queue at the current priority.
	    */
	    sth = &bm->bm_fpq[save_priority];
	    dm0p_mlock(SEM_EXCL, &sth->sth_mutex);
	    sth->sth_lcount--;
	    dm0p_munlock(&sth->sth_mutex);
	}

	/*
	** Move buffer to the front of the priority zero free queue.
	*/
	dm0p_mlock(SEM_EXCL, &bm->bm_fpq[0].sth_mutex);
	next_buf = bm->bm_fpq[0].sth_stq.stq_next;
	b->buf_state.stq_next = next_buf;
	b->buf_state.stq_prev = BUF_ENDLIST;
	if (next_buf != BUF_ENDLIST)
	    buf_array[next_buf].buf_state.stq_prev = b->buf_id.bid_index;
	else
	    bm->bm_fpq[0].sth_stq.stq_prev = b->buf_id.bid_index;
	bm->bm_fpq[0].sth_stq.stq_next = b->buf_id.bid_index;
	bm->bm_fpq[0].sth_count++;
	dm0p_munlock(&bm->bm_fpq[0].sth_mutex);

	if ( bm->bm_status & BM_FWAIT )
	{
	    dm0p_mlock(SEM_EXCL, &bm->bm_mutex);
	    if (bm->bm_status & BM_FWAIT)
	    {
		dm0p_erelease(lock_list, DM0P_FWAIT_EVENT,
				bm->bm_cache_ix, 0, 0);
		bm->bm_status &= ~BM_FWAIT;
	    }
	    dm0p_munlock(&bm->bm_mutex);
	}
    }
    else
    {
	i4 rdelta, mdelta, vdelta;

	/*
	** Adjust the counts of fixed/modified buffers in this group
	** and readjust the group queues if necessary.
	**
	** Check for fixed condition first, since if buffer is listed as fixed
	** and modified, then it will be only on the fixed queue - not the
	** modify queue.
	*/
	group_array = lbm->lbm_groups[b->buf_id.bid_cache];
	group = &group_array[b->buf_owner];
	dm0p_mlock(SEM_EXCL, &group->g_mutex);
	rdelta = mdelta = vdelta = 0;
	if (save_sts & BUF_FIXED)
	    rdelta = -1;
	else if (save_sts & BUF_MODIFIED)
	    mdelta = -1;

	if ( save_sts & BUF_VALID )
	    vdelta = -1;
	move_group(bm, group, b, lock_list, rdelta, mdelta, vdelta);
	dm0p_munlock(&group->g_mutex);
    }

    /*
    ** If any other threads are currently waiting on this buffer then
    ** wake them up now that we have freed it.
    */
    if (save_sts & BUF_MWAIT)
    {
	dm0p_erelease(lock_list, DM0P_PGMUTEX_EVENT,
			b->buf_id.bid_cache, b->buf_id.bid_index, 0);
    }
    if (save_sts & BUF_IOWAIT)
    {
	dm0p_erelease(lock_list, DM0P_BIO_EVENT,
			b->buf_id.bid_cache, b->buf_id.bid_index, 0);
    }
    if (save_sts & BUF_FCWAIT)
    {
	dm0p_erelease(lock_list, DM0P_FCWAIT_EVENT,
			b->buf_id.bid_cache, b->buf_id.bid_index, 0);
    }

    if (DMZ_BUF_MACRO(15))
	(VOID) dm0p_check_consistency(8);

    /* Caller can't do anything else with the buf, so unmutex it */
    dm0p_munlock(&b->buf_mutex);
    return;
}

/*{
** Name: move_group - Check whether a group can be moved to from fixed/modified
**		     group queue to modify/free group queue.
**
** Description:
**      This routine checks whether a group can be moved to from one queue
**	to another.
**
**	If the group is seen to be empty, it's put at the front of free
**	queue 0, and marked empty.  This means that the group is grabbable
**	as soon as the caller releases the group mutex.  It's essential
**	that ALL buffer manipulations be complete before doing a move_group
**	call on an empty group (because gfault_page doesn't bother mutexing
**	the buffer headers).
**
**	The deltas are passed in, rather than being applied directly by the
**	caller, so that we don't screw around with the counts for a group
**	that's BUSY by someone else.
**
** Inputs:
**	bm				group's cache
**	group				the group number, an index to the
**					group arrary, to ckeck for.
**      buffer                          mutexed group buffer, if any
**	lock_list			Session's lock list, for busy-wait check
**	rdelta				g_refcount delta
**	mdelta				g_modcount delta
**	vdelta				g_valcount delta
**
** Outputs:
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	22-Jan-1987 (ac)
**	    Created for Jupiter.
**	28-feb-1989 (rogerk)
**	    Added support for Shared Buffer Manager.
**	22-feb-1994 (andys)
**	    Add call to dm0p_check_consistency.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	14-Sep-1996 (jenjo02)
**	    Caller must hold group's g_mutex.
**	24-Oct-1996 (jenjo02)
**	    Changed prototype to just pass pointer to the group.
**	16-Apr-1997 (jenjo02)
**	    Added code to move free group to fixed queue so that
**	    it can be consolidated in one place.
**	    Also added code to move group from modified to fixed
**	    queue when appropriate.
**	    Use logical AND and NOT to set g_sts instead of EQUAL
**	    to avoid destruction of other meaningful status bits.
**	10-Jan-2000 (jenjo02)
**	    Implemented group priority queues.
**	14-Jun-2000 (jenjo02)
**	    Must wait for busy group before modifying its
**	    attributes and/or changing its state queue.
**	22-Sep-2000 (jenjo02)
**	    Added g_lock_list to DM0P_GROUP, lock list of thread that
**	    set G_BUSY state. move_group() won't wait for busy group
**	    if the lock list matches.
**	27-Apr-2005 (jenjo02/schka24)
**	    Pass group count deltas, so that we don't mess with group
**	    counters until after we busy-wait.
*/
static VOID
move_group(
DM0P_BMCB	*bm,
DM0P_GROUP 	*g,
DM0P_BUFFER	*b,
i4	lock_list,
i4	rdelta,
i4	mdelta,
i4	vdelta)
{
    DMP_LBMCB           *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_GROUP          *group_array = lbm->lbm_groups[bm->bm_cache_ix];
    DM0P_STATE_HEAD     *sth;
    i4			next_group, prev_group;
    i4			unfixed = FALSE;
    i4			gmcount;

    /*
    ** g_priority	: priority queue this group is currently on
    ** g_new_pri	: priority to use when requeued 
    ** g_valcount	: number of buffers in group on hash queue
    ** g_pages		: number of pages in use by group
    **				(<= bm->bm_gpages)
    */

    /*
    ** If group is busy, we must wait for it unless this
    ** is the thread that set the busy status.
    */
    while ( g->g_sts & G_BUSY && g->g_lock_list != lock_list )
    {
	i4	buf_unbusy = FALSE;

	/*
	** If a group buffer was passed, it is also mutexed
	** and the mutex must be released. If not already marked
	** BUSY, mark the buffer as read-busy.
	*/
	if ( b ) 
	{
	    if ( (b->buf_sts & BUF_BUSY) == 0 )
	    {
		buf_unbusy = TRUE;
		b->buf_sts |= BUF_RBUSY;
	    }
	    dm0p_munlock(&b->buf_mutex);
	}
	
	lbm->lbm_stats[bm->bm_cache_ix].bs_gwait++;
	bm->bm_stats.bs_gwait++;
	g->g_sts |= G_WAIT;
	dm0p_eset(lock_list, DM0P_GWAIT_EVENT, 
		    bm->bm_cache_ix, g->g_index, 0);
	dm0p_munlock(&g->g_mutex);

	/* Wait for the group to become unbusy */
	dm0p_ewait(lock_list, DM0P_GWAIT_EVENT, 
		    bm->bm_cache_ix, g->g_index, 0, 0);

	dm0p_mlock(SEM_EXCL, &g->g_mutex);

	/*
	** Relock the buffer. If we marked it read-busy,
	** unbusy it and wake up any waiters.
	*/
	if ( b )
	{
	    dm0p_mlock(SEM_EXCL, &b->buf_mutex);
	    if ( buf_unbusy )
	    {
		if (b->buf_sts & BUF_IOWAIT)
		{
		    dm0p_erelease(lock_list, DM0P_BIO_EVENT,
			b->buf_id.bid_cache, b->buf_id.bid_index, 0);
		}

		b->buf_sts &= ~(BUF_RBUSY | BUF_IOWAIT);
	    }
	}
    }

    /* Now that the group isn't busy by someone else, adjust counters */

    g->g_refcount += rdelta;
    g->g_modcount += mdelta;
    g->g_valcount += vdelta;

    if (g->g_sts & G_FIXED)
    {
	/* Fixed groups are on no queue */
	if (g->g_refcount == 0)
	{
	    /* count one less fixed group */
	    g->g_sts &= ~G_FIXED;
	    unfixed = TRUE;

	    /*
	    ** Put group on the end of free or modified group 
	    ** queue dictated by the value of g_new_pri.
	    **
	    ** If the group now contains no valid buffers, move
	    ** it to the front of free priority zero.
	    */

	    g->g_priority = g->g_new_pri;

	    if (g->g_modcount == 0)
	    {
		g->g_sts &= ~G_MODIFIED;
		sth = &bm->bm_gfq[g->g_priority];
	    }
	    else
	    {
		g->g_sts |= G_MODIFIED;
		sth = &bm->bm_gmq[g->g_priority];
	    }

	    if ( g->g_valcount )
	    {
		dm0p_mlock(SEM_EXCL, &sth->sth_mutex);
		prev_group = sth->sth_stq.stq_prev;
		g->g_state.stq_next = BUF_ENDLIST;
		g->g_state.stq_prev = prev_group;
		sth->sth_stq.stq_prev = g->g_index;
		if (prev_group != BUF_ENDLIST)
		    group_array[prev_group].g_state.stq_next = g->g_index;
		else
		    sth->sth_stq.stq_next = g->g_index;
		/* Count one more free/modified group, one less fixed */
		sth->sth_count++;
		sth->sth_lcount--;

		/*
		** If the modified group count has reached a fraction
		** of the WriteBehind start limit, count the 
		** total number of modified groups and start the
		** WriteBehind threads if needed.
		*/
		if ( g->g_sts & G_MODIFIED &&
		     sth->sth_count >= (bm->bm_gwbstart / DB_MAX_TABLEPRI) &&
		     bm->bm_status & BM_WB_IN_CACHE &&
		     bm->bm_status & BM_WBWAIT )
		{
		    DM0P_COUNT_QUEUE(bm->bm_gmq, gmcount);
		    if ( gmcount >= bm->bm_gwbstart &&
		         bm->bm_status & BM_WB_IN_CACHE &&
			 bm->bm_status & BM_WBWAIT )
		    {
			wb_cycle_start(bm, lock_list, 
					BM_WB_GROUP, "MOVE_GROUP");
		    }
		}
		dm0p_munlock(&sth->sth_mutex);
	    }
	}

    }
    else if (g->g_sts & G_MODIFIED)
    {
	/*
	** A group with modified pages and fixed pages can only 
	** belong to a fixed group.
	*/
	if (g->g_modcount == 0 || g->g_refcount != 0)
	{
	    /*
	    ** Take group off of priority modified group queue.
	    */
	    sth = &bm->bm_gmq[g->g_priority];
	    dm0p_mlock(SEM_EXCL, &sth->sth_mutex);
	    next_group = g->g_state.stq_next;
	    prev_group = g->g_state.stq_prev;
	    if (next_group != BUF_ENDLIST)
		group_array[next_group].g_state.stq_prev = prev_group;
	    else
		sth->sth_stq.stq_prev = prev_group;
	    if (prev_group != BUF_ENDLIST)
		group_array[prev_group].g_state.stq_next = next_group;
	    else
		sth->sth_stq.stq_next = next_group;
	    /* Count one less modified group */
	    sth->sth_count--;

	    /* If still fixed, count another fixed group */
	    if ( g->g_refcount )
	    {
		g->g_sts |= G_FIXED;
		sth->sth_lcount++;
	    }
	    dm0p_munlock(&sth->sth_mutex);

	    g->g_sts &= ~G_MODIFIED;
	    g->g_state.stq_next = g->g_state.stq_prev = 0;

	    /*
	    ** Put group on the end of the (new) priority free queue 
	    ** if no longer fixed.
	    */
	    g->g_priority = g->g_new_pri;

	    if ( g->g_valcount && (g->g_sts & G_FIXED) == 0 )
	    {
		sth = &bm->bm_gfq[g->g_priority];
		dm0p_mlock(SEM_EXCL, &sth->sth_mutex);
		prev_group = sth->sth_stq.stq_prev;
		g->g_state.stq_next = BUF_ENDLIST;
		g->g_state.stq_prev = prev_group;
		sth->sth_stq.stq_prev = g->g_index;
		if (prev_group != BUF_ENDLIST)
		    group_array[prev_group].g_state.stq_next = g->g_index;
		else
		    sth->sth_stq.stq_next = g->g_index;
		/* Count one more free group */
		sth->sth_count++;
		dm0p_munlock(&sth->sth_mutex);
	    }
	}
    }
    /* 
    ** Group is neither fixed nor modified (i.e, it's free).
    ** If it contains fixed buffers, remove it from the free queue
    ** mark it fixed, and leave it on no queue.
    */
    else if ( g->g_refcount )
    {
	/* Take the group descriptor off of the free queue. */
	sth = &bm->bm_gfq[g->g_priority];
	dm0p_mlock(SEM_EXCL, &sth->sth_mutex);
	next_group = g->g_state.stq_next;
	prev_group = g->g_state.stq_prev;
	if (next_group != BUF_ENDLIST)
	    group_array[next_group].g_state.stq_prev = prev_group;
	else
	    sth->sth_stq.stq_prev = prev_group;
	if (prev_group != BUF_ENDLIST)
	    group_array[prev_group].g_state.stq_next = next_group;
	else
	    sth->sth_stq.stq_next = next_group;
	/* Count one less free group, one more fixed */
	sth->sth_count--;
	sth->sth_lcount++;
	dm0p_munlock(&sth->sth_mutex);

	/* Set new group priority */
	g->g_priority = g->g_new_pri;

	g->g_sts |= G_FIXED;
	g->g_state.stq_next = g->g_state.stq_prev = 0;
    }

    /* 
    ** If the group contains no valid pages, move it to the front
    ** of free queue priority zero.
    ** Putting these groups at priority zero causes them to
    ** be picked first by gfault_page() and avoids having to
    ** toss a group with valid pages.
    */
    if ( g->g_valcount == 0 )
    {
	/* If on free queue, remove it */
	if ( g->g_state.stq_next || g->g_state.stq_prev )
	{
	    /* Take the group descriptor off of the free queue. */
	    sth = &bm->bm_gfq[g->g_priority];
	    dm0p_mlock(SEM_EXCL, &sth->sth_mutex);
	    next_group = g->g_state.stq_next;
	    prev_group = g->g_state.stq_prev;
	    if (next_group != BUF_ENDLIST)
		group_array[next_group].g_state.stq_prev = prev_group;
	    else
		sth->sth_stq.stq_prev = prev_group;
	    if (prev_group != BUF_ENDLIST)
		group_array[prev_group].g_state.stq_next = next_group;
	    else
		sth->sth_stq.stq_next = next_group;
	    /* Count one less free group */
	    sth->sth_count--;
	    dm0p_munlock(&sth->sth_mutex);
	}

	g->g_priority = g->g_new_pri = g->g_pages = 0;

	/* Insert first in priority zero free queue */
	sth = &bm->bm_gfq[0];
	dm0p_mlock(SEM_EXCL, &sth->sth_mutex);
	next_group = sth->sth_stq.stq_next;
	g->g_state.stq_next = next_group;
	g->g_state.stq_prev = BUF_ENDLIST;
	if (next_group != BUF_ENDLIST)
	    group_array[next_group].g_state.stq_prev = g->g_index;
	else
	    sth->sth_stq.stq_prev = g->g_index;
	sth->sth_stq.stq_next = g->g_index;
	/* Count one more free group */
	sth->sth_count++;
	/* If group was unfixed, count one less fixed group */
	if ( unfixed )
	    sth->sth_lcount--;
	dm0p_munlock(&sth->sth_mutex);
    }

    /*
    ** Reset the skipped but so gfault_page will reconsider
    ** this group.
    */
    g->g_sts &= ~G_SKIPPED;

    if (DMZ_BUF_MACRO(15))
	(VOID) dm0p_check_consistency(9);
    
    /* Return with g_mutex still held */
    return;
}

/*{
** Name: dm0p_cp_flush - flush all dirty pages in buffer manager.
**
** Description:
**	This routine is called by the fast commit thread (dmc_fast_commit)
**	to flush all dirty pages out of the cache as part of consitency
**	point protocol.
**
**	It cycles once through the buffer manager's buffer cache forcing
**	any dirty page belonging to a fast commit database.  If a page
**	currently busy, this routine waits for it to become not busy and
**	checks again.
**
**	When running in a Shared Buffer manager configuration, there will
**	be multiple fast commit threads (one per server) running at one
**	time.  Because each server may not have an open TCB for every
**	modified buffer in the cache, some buffers may not be able to be
**	flushed by this server.  Whenever a modified buffer is encountered,
**	this routine will look up the tcb for that table.  If there is not
**	a tcb, then the buffer is skipped - we are trusting that it will
**	be processed by another fast commit thread.
**
**	In order to assure that we don't skip any buffers during the CP,
**	we keep an index pointer (bm_cpindex) that is incremented as the
**	fast commit threads cycle through the cache.  This index is set to
**	zero at the start of the consistency point.  Each time the buffer
**	pointed to by cpindex is processed (written or verified that it does
**	not need to be written), the cpindex is incremented.  When all FC
**	threads are done cycling through the cache, the cpindex pointer
**	should point to the last buffer of the cache.  If it doesn't then
**	the CP was not done properly.
**
**	Each time a Fast Commit thread processes a buffer (either writes it
**	or verifies that it does not need to be written), it sets the cpcount
**	field on the buffer to the current bm_cpcount value.  This value is
**	incremented each time a CP is done.  This marks that this buffer has
**	been processed for the current consistency point, and all other fast
**	commit threads can skip over it.
**
**	NOTE: This routine may force a page to disk that is currently FIXED
**	    by another thread!!!!!!
**	The page is mutexed while it is being forced to prevent another
**	thread from updating it while it is being written.
**
**	If a page was fixed for write before the start of this routine, but
**	is not yet dirty, then this routine assumes that the page may be
**	updated by a operation that has been logged previous to the
**	consistency point (which means the change must be forced as part
**	of the CP protocol), and marks the buffer as needing to be forced
**	at commit time.
**
**	This routine currently does all writes using single buffers and
**	does not take advantage of group writes.
**
**	If there are any write behind threads enabled, then start them up
**	in order to flush the buffer manager faster.
**
** Inputs:
**	lock_list	- transaction lock list
**	log_id		- transaction log id
**
** Outputs:
**      err_code                        Reason for error return status.
**	Returns:
**	    E_DB_OK
**	    E_DB_FATAL
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	15-jun-1987 (rogerk)
**	    Written for Road Runner.
**	30-sep-1988 (rogerk)
**	    Added write behind thread support.  Signal the write behind threads
**	    to assist in the flush.
**	28-feb-1989 (rogerk)
**	    Added support for Shared Buffer Manager.
**	    Skip buffers being written by another fast commit thread.  Skip
**	    buffers that we don't have tcb for.  Use cpcount, cpindex.  Check
**	    for incomplete cp.
**	19-mar-1989 (rogerk)
**	    Fixed semaphore unlock problem.
**	 7-jun-1989 (rogerk)
**	    Don't flush temp tables during CP.
**	10-jul-1989 (rogerk)
**	    Don't flush newly-extended pages during CP.  Those pages must
**	    be flushed by the thread which allocated them.
**	21-aug-1989 (rogerk)
**	    When skipping newly-extended page during a CP, be sure to
**	    increment cpindex if it is appropriate.
**	24-aug-1989 (rogerk)
**	    Moved code for above fix down so that correct cpcount value would
**	    be put into buffer header.
**	 5-sep-1989 (rogerk)
**	    Don't increment IOWAIT and MWAIT statistics when have to wait in
**	    CP flush.  We don't really care when the FC thread has to wait and
**	    it skews the statistics so that we can't tell if user threads are
**	    being forced to wait (which is what we really care about).
**      11-sep-1990 (jonb)
**	    Integrate ingres6301 change 280136 (rogerk):
**            Pass buf_tcb_stamp value to dm2t_get_tcb_ptr.  Added new 
**            informative error message before incomplete_cp server crash.
**	11-sep-1990 (jonb)
**	    Integrate ingres6301 change 280141 (rogerk):
**            Took out 2 args from E_DM9397_INCOMPLETE_CP error message as
**            ER does not seem to support > 10 arguments.
**	17-oct-1991 (rogerk)
**	    Merged changes made by Derek during the file extend project.
**	    Took out use of obsolete BUF_EXTENDPAGE.  Fast commit thread
**	    no longer has to worry about flushing a page which has not
**	    yet had its space allocated in the corresponding file.
**	26-oct-1992 (rogerk)
**	    Reduced logging project: 
**	      - Changed to force pages for all database, even those which
**		do not use fast commit.  New recovery algorithms require
**		all dirty pages to be forced at Consistency Points.
**	      - Took out setting of FORCE_COMMIT flag.  It is obsolete now
**		that mutexes are held while logging updates to cached pages.
**	      - changed references to tcb fields to use new tcb_table_io 
**		structure fields.  
**	      - Changed from old dm2t_get_tcb_ptr to new fix_tableio_ptr.
**	15-mar-1993 (jnash & rogerk)
**	    Fix AV in multi-server shared cache CP's.  Don't reference through
**	    null pointer when find a buffer for which this server has no tcb.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**	21-may-1993 (rogerk)
**	    Changed routine to not probe the page_stat to check for the
**	    DMPP_MODIFY bit unless the page is currently fixed.  Using the
**	    assumption that the page cannot be modified unless it is fixed,
**	    we avoid referencing pages that have been sitting in the cache
**	    unfixed and unmodified.  This allows CP's to be executed on
**	    large caches without faulting in the entire cache contents.
**      21-jun-1993 (jnash)
**          Add trace point DM1308 to extend length of CP to 30 seconds.
**	18-oct-1993 (rmuth)
**	    Add DMP_BMCB statistics.
**	18-oct-1993 (rogerk)
**	    Changed routine to not start up write-behind threads to
**	    assist in flushing the cache to keep cache contention down
**	    during CP events.
**	22-feb-1994 (andys)
**	    Integrate recent changes from ingres63p:
**	 	3-may-1993 (andys)
**   	    	    If we can't find the TCB, then move up the cpindex if 
**		    appropriate. Part of fixes for bug 46951.
**		11-jun-1993 (andys)
**	    	    Add TRdisplay in section of code that "can't happen".
**	    Update to use BUF_MUTEX_OK bit instead of the FCBUSY bit to
**	    indicate that the buffer is mutexed while not fixed.  The FCBUSY
**	    bit now only indicates to the CP threads that another CP thread
**	    is handling that buffer.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**      06-may-1996 (nanpr01)
**          Change the reference to the page header with macros for
**	    New Page Format.
**	01-aug-1996 (nanpr01)
**	    Buffer manager mutex getting released twice. Additionally
** 	    code by itb ie, calling complete_cp_flush will cause 2 call
**	    to eventrelease.
**	13-Feb-1997 (jenjo02)
**	  - After forcing the log, update BM's bmcb_last_lsn.
**	  - Rearranged mutex taking/releasing to enhance cooperation with
**	    other threads.
**	01-May-1997 (jenjo02)
**	  - Rather than waiting for a BUSY or MUTEXed buffer, mark that spot
**	    and go on to find useful work. Iteratively return to those buffers
**	    until all skipped buffers have been processed for this CP.
**	  - Force only those pages which must be included in this CP.
**	    Previously, all modified buffers encountered were written, even
**	    those which were modified after the CP's LSN was established, causing
**	    the CP thread to act as a write-behind thread and extending the
**	    time needed to take the CP.
**	06-May-1997 (jenjo02)
**	    Rescinded LSN check outlined above. It's possible for a page to
**	    be modified (LSN1 ), have this CP start (CP LSN), and have the
**	    page be modified again (LSN2). If LSN2 > CP LSN, the page won't
**	    get written.
**     14-oct-1998 (nicph02)
**          CP flush fails with error E_DM9397 although ALL buffers were 
**          flushed out (bug 92677).
**     24-nov-1998 (nicph02)
**        If call to fix_tableio_ptr fails (no TCB found), do not log
**        an error as another CP thread will find the TCB and flush it.
**        (bug 83999).
**	10-Dec-1997 (jenjo02)
**	    Keep reusing the TBIO as long as possible instead of unfixing it
**	    after every force.
**	26-Mar-1998 (jenjo02)
**	    Avoid TCB/TBIO lookup for temporary tables (which we won't flush 
**	    anyway) by setting BUF_TEMPTABLE in the buffer's status field when
**	    the page is faulted in.
**	20-Jul-1998 (jenjo02)
**	    If the modified and fixed buffer counts are, or are reduced to, zero,
**	    skip the remainder of the cache - there are no buffers which need
**	    flushing.
**	04-Mar-1999 (jenjo02)
**	    Test for no TCBs in cache sooner, when cache is first realized.
**	15-Apr-1999 (jenjo02)
**	    Removed all that "skip and return to busy buffer" code, which never
**	    proved effective and probably extended the time to complete a CP
**	    of very large caches. 
**	15-Apr-1999 (jenjo02)
**	    Removed left-over debugging spin loop if CP fails, will now 
**	    procede to dmd_check().
**	06-May-1999 (jenjo02)
**	    CP now prebuilds a list of candidate pages instead of bashing thru
**	    all caches. New function cp_prep() prepares the list for the CP,
**	    cp_flush() drains the queue.
**	09-Mar-2000 (jenjo02)
**	    Unfix any tbio before waiting for BUF_MUTEX to avoid undetectable 
**	    deadlock with the thread that holds the MUTEX.
**	23-May-2000 (jenjo02)
**	    CP flush failures may occur if a cache's TCBs have
**	    all been closed/purged by the time the CP thread sees them.
**	    Relocate test for open TCBs after buffer has been 
**	    cp-checked. This is a partial cross integration of 445937 from 3.0.
**	25-Aug-2000 (jenjo02)
**	    Whoops, that last change left the buffer's mutex locked.
**	15-Mar-2006 (jenjo02)
**	    Moved dm0l_force to cp_prep so that it's done just
**	    once per CP, not once for each CP thread.
*/
DB_STATUS
dm0p_cp_flush(
i4		lock_list,
i4		log_id,
i4		*err_code)
{
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BMCB       *bm;
    DMP_TABLE_IO    *tbio;
    DM0P_BUFFER     *b;
    DB_STATUS	    status;
    i4	    	    local_error;
    i4		    cache_ix;
    i4	    	    *cp_array;
    i4		    i;
    i4		    flush_count;

    dm0p_mlock(SEM_EXCL, &bm_common->bmcb_cp_mutex);

    lbm->lbm_fcflush++;
    bm_common->bmcb_fcflush++;

    /* Build the working set for this CP. */
    if ( status = cp_prep(lock_list, log_id, err_code) )
    {
	/* If error, error message already logged in dm0l_force */
	if (*err_code > E_DM_INTERNAL)
	    *err_code = E_DM9344_BM_CP_FLUSH_ERROR;
	dm0p_munlock(&bm_common->bmcb_cp_mutex);
	return (status);
    }

    TRdisplay("%@ Starting CP %d with %d buffers\n",
	bm_common->bmcb_cpcount, bm_common->bmcb_cpbufs);

    /* Cycle through the CP arrays and write out all modified pages. */
    if ( bm_common->bmcb_cpbufs )
    {
	dm0p_munlock(&bm_common->bmcb_cp_mutex);
	for (cache_ix = 0; 
	     cache_ix < DM_MAX_CACHE && status == E_DB_OK; 
	     cache_ix++)
	{
	    bm = lbm->lbm_bmcb[cache_ix];
	    if ( bm && bm->bm_cpbufs )
	    {
		status = cp_flush((DM0P_CFA*)NULL, 
				    cache_ix, lock_list, log_id, 
				    &flush_count, err_code);
		if ( flush_count )
		    TRdisplay("%@ CP Thread flushed %d %dK pages\n",
			flush_count, bm->bm_pgsize >> 10);
	    }
	}
	dm0p_mlock(SEM_EXCL, &bm_common->bmcb_cp_mutex);
    }

    bm_common->bmcb_cpindex = bm_common->bmcb_totbufcnt;

    /*
    ** Mark that this thread is done with the consistency point.
    ** If this is the last Consistency Point Thread then check if there
    ** were any buffers that were skipped during the CP.  If so
    ** then signal a fatal error - REDO processing is required.
    */

    if (++bm_common->bmcb_cpcheck >= bm_common->bmcb_srv_count)
    {
	bm_common->bmcb_status &= ~BMCB_FCFLUSH;

	/* All buffers at the time of the CP must have been processed */
	for (cache_ix = 0; cache_ix < DM_MAX_CACHE; cache_ix++)
	{
	    bm = lbm->lbm_bmcb[cache_ix];
	    if ( bm && bm->bm_cpindex < bm->bm_cpbufs )
	    {
		i4	bad_bufs = 0;

		cp_array = lbm->lbm_cp_array[cache_ix];

		TRdisplay("\n%@ POSSIBLE INCOMPLETE CP %d %dK cache cpindex %d cpbufs %d\n",
		    bm_common->bmcb_cpcount,
		    bm->bm_pgsize >> 10,
		    bm->bm_cpindex, bm->bm_cpbufs);
		/* Display all unprocessed buffers in cache */
		for (i = 0; i < bm->bm_cpbufs; i++)
		{
		    b = &lbm->lbm_buffers[cache_ix][cp_array[i]];

		    if (b->buf_cpcount != bm_common->bmcb_cpcount)
		    {
			bad_bufs++;
			TRdisplay("%@ %dK buf %d sts %x cpcount %d cpindex %d i %d (%d,%d)\n",
			    bm->bm_pgsize >> 10,
			    b->buf_id.bid_index,
			    b->buf_sts, b->buf_cpcount, b->buf_cpindex, i,
			    b->buf_tabid.db_tab_base, b->buf_tabid.db_tab_index);
		    }
		}

		if ( bad_bufs == 0 )
		    TRdisplay("%@ Reverified all CP buffers processed, CP is ok\n");
		else
		{
		    /*
		    ** The buffers were not all processed.  Re-verify the number
		    ** of server connections just to make sure there is not still
		    ** some Consistency Point Thread out there.  If this really is the
		    ** last Consistency Point Thread then there is something fatally
		    ** wrong with the buffer manager.
		    */
		    dm0p_count_connections();
		    if (bm_common->bmcb_cpcheck >= bm_common->bmcb_srv_count)
		    {
			/* Find the first unprocessed buffer */
			b = (DM0P_BUFFER*)NULL;
			for (i = 0; 
			     b == (DM0P_BUFFER*)NULL && i < bm->bm_cpbufs;
			     i++)
			{
			    b = &lbm->lbm_buffers[cache_ix][cp_array[i]];
			    if (b->buf_cpcount == bm_common->bmcb_cpcount)
				b = (DM0P_BUFFER*)NULL;
			}

			(VOID) fix_tableio_ptr(b, (i4)0, lock_list, log_id,
			    &tbio, &local_error);
			ule_format(E_DM9397_CP_INCOMPLETE_ERROR, (CL_SYS_ERR *)0,
			    ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0,
			    &local_error, 10,
			    0, bm->bm_cpindex,
			    0, b->buf_tabid.db_tab_base,
			    0, b->buf_tabid.db_tab_index,
			    0, b->buf_sts,
			    0, b->buf_cpcount, 0, bm_common->bmcb_cpcount,
			    0, b->buf_refcount,
			    0, tbio, 0, b->buf_tcb_stamp,
			    0, (tbio ? tbio->tbio_cache_valid_stamp : 0));
			dmd_check(E_DMF024_INCOMPLETE_CP);
		    }
		}
	    }
	}
    }
    dm0p_munlock(&bm_common->bmcb_cp_mutex);

    if (DMZ_ASY_MACRO(8))
    {
	/*
	** Extend CP duration an additional 30 seconds.
	*/
	TRdisplay("dm0p_cp_flush: CP extended 30 seconds.\n");
	CSsuspend(CS_TIMEOUT_MASK, 30, 0);
    }

    if (DMZ_ASY_MACRO(3))
    {
	LG_LSN		ecp_log_lsn;
	i4		local_error;
	DB_STATUS       local_status;

	/* Print out log EOF at beginning and end of CP */
	local_status = dm0l_force(log_id, (LG_LSN *)0, 
	    &ecp_log_lsn, &local_error);
	TRdisplay("CP EOF in LSN terms: %d,%d .. %d,%d\n",
	    bm_common->bmcb_cp_lsn.lsn_high, 
	    bm_common->bmcb_cp_lsn.lsn_low,
	    ecp_log_lsn.lsn_high, ecp_log_lsn.lsn_low);
    }

    return (status);
}

/*{
** Name: cp_prep - Prepare list of buffers to flush for CP.
**
** Description:
**
**	This function creates an array of buffers which appear
**	to need to be flushed for the CP. It dirty-peeks at
**	all buffers in all caches looking for modified or
**	fixed and modified pages, adding each to the CP
**	array.
**
** Inputs:
**	none			Caller must hold bmcb_cp_mutex EXCL
**
** Outputs:
**	lbm_cp_array		Arrays of CP-eligible buffers.
**	bmcb_cpbufs		Total buffers, all caches, in
**				need of flush.
**
** Side Effects:
**	CP arrays are created, ready for flushing.
**
** History:
**	16-Apr-1999 (jenjo02)
**	    Written
**	12-Jul-1999 (jenjo02)
**	    After extracting the CP candidate buffers, sort them
**	    into (dbid,table_id,index_id,page) order.
**	20-Apr-2000 (jenjo02)
**	    Implement per-cache CP arrays, induce WriteBehind
**	    thread assistance.
**	31-Jan-2003 (jenjo02)
**	    Adapted for Cache Flush Agents.
**	15-Mar-2006 (jenjo02)
**	    Moved dm0l_force to here so that it's done just
**	    once per CP, not once for each CP thread, plus
**	    we only force the log if there are buffers to 
**	    flush.
*/
static DB_STATUS
cp_prep(
i4	lock_list,
i4	log_id,
i4	*err_code)
{
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BMCB       *bm;
    i4	    	    *cp_array;
    i4              cache_ix;
    DM0P_BUFFER     *b;
    DMPP_PAGE	    *page, *page_pool;
    DM0P_BUFFER	    *buf_array;
    i4    	    i;
    i4		    fcount, mcount, lcount;
    i4		    gfcount, gmcount, glcount;
    i4		    cpbufs;
    DB_STATUS	    status;

    /* The caller holds the bmcb_cp_mutex EXCL */

    /* If array has already been built, return */
    if (bm_common->bmcb_status & BMCB_FCFLUSH)
	return(E_DB_OK);

    /*
    ** Mark the buffer manager as in a Consistency Point.
    ** Some buffer manager algorithms are sensitive to being in a CP state.
    */
    bm_common->bmcb_status |= BMCB_FCFLUSH;

    /*
    ** Build a array of candidate pages in each cache.
    **
    ** Any valid page that is modified or fixed for write
    ** and modified (dirty) is added to the array of candidate CP pages.
    **
    ** Pages belonging to temporary tables are never flushed,
    ** hence are ignored.
    */
    for (cache_ix = 0; cache_ix < DM_MAX_CACHE; cache_ix++)
    {
	bm = lbm->lbm_bmcb[cache_ix];
	if (!bm || !(bm->bm_status & BM_ALLOCATED))
	    continue;

	cp_array = lbm->lbm_cp_array[cache_ix];
	page_pool = lbm->lbm_pages[cache_ix];
        buf_array = lbm->lbm_buffers[cache_ix];
	cpbufs = 0;

	DM0P_COUNT_ALL_QUEUES(fcount, mcount, lcount);
	DM0P_COUNT_ALL_GQUEUES(gfcount, gmcount, glcount);

	for (i = 0; i < bm->bm_bufcnt; i++)
	{
	    /*
	    ** If the modified and fixed queues are empty,
	    ** there's no point in looking at the remaining buffers
	    ** in the cache - none need to be written for this CP.
	    */
	    if ( mcount == 0 && lcount == 0 )
	    {
		/* No modified/fixed single buffers; groups? */
		if (gmcount == 0 && glcount == 0)
		{
		    /* Break to next cache */
		    break;
		}
		else
		{
		    /*
		    ** No mod/fixed single buffers, but there are groups.
		    ** If we're not already into group buffers,
		    ** push up to groups, continue in cache.
		    */
		    if (i < bm->bm_sbufcnt)
			i = bm->bm_sbufcnt;
		}
	    }

	    b = &buf_array[i];
	    
	    page = (DMPP_PAGE *)((char *)page_pool + b->buf_page_offset);
	    
	    /*
	    ** If page is valid, not a temptable page, and modified
	    ** or fixed for write and modified, add it to the array.
	    **
	    ** Pages which are fixed for write but not yet modified don't
	    ** need to be flushed by the CP.
	    **
	    ** Read/Write-busy buffers can be likewise ignored (Read-busy
	    ** pages are being faulted in, hence can't be yet modified;
	    ** Write-busy pages are being written and don't need to be
	    ** written again).
	    */
	    if ( (b->buf_sts & (BUF_VALID | BUF_TEMPTABLE | BUF_BUSY)) == BUF_VALID
		&& (b->buf_sts & BUF_MODIFIED
		    || ((b->buf_sts & (BUF_FIXED | BUF_WRITE)) == 
				(BUF_FIXED | BUF_WRITE)
			  && DMPP_VPT_GET_PAGE_STAT_MACRO(b->buf_relpgtype, page) 
					& DMPP_MODIFY
		        )
		    )
		)
	    {
		/* Add buffer to cache's CP array */
		cp_array[cpbufs++] = i;
	    }
	}

	if ( cpbufs )
	{
	    /* Force the entire log, recording the guaranteed-flushed LSN */
	    status = dm0l_force(log_id, (LG_LSN *)0, 
			&bm_common->bmcb_cp_lsn, err_code);
	    if (status)
	    {
		/* If error, error message already logged in dm0l_force */
		if (*err_code > E_DM_INTERNAL)
		    *err_code = E_DM9344_BM_CP_FLUSH_ERROR;
		return(status);
	    }

	    /*
	    ** Update the new guaranteed-flushed log address if it is greater than
	    ** the old one.
	    ** 
	    ** Use (held) bmcb_cp_mutex to protect bmcb_last_lsn.
	    */
	    if (LSN_GT(&bm_common->bmcb_cp_lsn, &bm_common->bmcb_last_lsn))
		STRUCT_ASSIGN_MACRO(bm_common->bmcb_cp_lsn, 
					bm_common->bmcb_last_lsn);

	    /* Add to the all-cache total of buffers to process */
	    bm_common->bmcb_cpbufs += cpbufs;

	    /* Sort this CP array into (FCB,page) order */
	    if ( cpbufs > 2 )
		cp_sort(buf_array, cp_array, 0, cpbufs-1);
		
	    /*
	    ** Compute number of Agents (~log2(cpbufs-256))
	    ** thence buffers per Agent.
	    **
	    ** All CP threads process all cpbufs, starting
	    ** from buffer 0, but are included as a group 
	    ** for this computation as a single "Agent".
	    */
	    bm->bm_cpagents = 0;
	    while ( (cpbufs >> bm->bm_cpagents) >= 256 && ++bm->bm_cpagents );
	    /* Make total Agents a multiple of connected servers */
	    bm->bm_cpagents = ((bm->bm_cpagents + bm_common->bmcb_srv_count-1)
			/ bm_common->bmcb_srv_count)
			* bm_common->bmcb_srv_count;

	    /* This number * Agent id yields starting buffer */
	    bm->bm_cpabufs = cpbufs / (bm->bm_cpagents+1);

	    /* Make counts visible to WB only after sort completes */
	    bm->bm_cpbufs = cpbufs;

	    /*
	    ** This cache's CP array is ready for processing.
	    **
	    ** If there are sleeping WriteBehind Agents in this
	    ** cache, awaken them to begin the flush while we continue
	    ** constructing the remaining cache arrays.
	    ** If they're already running, they'll see the CP 
	    ** in progress and join the fray.
	    */
	    if ( bm->bm_status & BM_WB_IN_CACHE &&
		 bm->bm_status & BM_WBWAIT )
	    {
		wb_cycle_start(bm, lock_list, BM_WB_CP, "CP_PREP");
	    }
	}
    }

    return(E_DB_OK);
}

/*{
** Name: cp_sort -  Perform Quicksort of a CP array.
**
** Description:
**	Sorts an unordered CP array into 
**	(FCB,page) order using Quicksort algorithm 
**	in preparation for CP flush.
**
**	The FCB pointer serves as an abstraction of
**	the page within table and accounts for
**	multiple locations.
**
**	Note that the FCB cannot be dereferenced, serving
**	only as a inexact sort key; the FCB may be an
**	address within a different address space (server)
**	and may be in the process of changing as the
**	buffers in the CP array are being concurrently
**	used and altered while this CP is processing it.
**	
**	Because of the dirty-reference situation, this is an approximate
**	sort, not an exact one.  It's good enough to get the buffers into
**	reasonable order to minimize FD stealing, seek repositioning,
**	and maximize the chances that gather-write can be effective.
**
**	Credit for the code goes to 
**	"Handbook of Algorithms and Data Structures", by
**	Gaston H. Gonnet & Ricardo Baeza-Yates.
**	Revised along the lines of Knuth/Sedgewick, ACM Comp Surveys,
**	Dec '74, part two of example 8a.
**
** Inputs:
**	i4 	 *cp_array[]	The current partition of the
**				cp array to be sorted.
**	i4	 *lo		The low bound of the partition
**	i4	 *hi		The high bound of the partition
**      
** Outputs:
**      none
**
**    Returns:
**      none
**         
**    Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	07-Jul-1999 (jenjo02)
**	    Created.
**	20-Dec-2002 (jenjo02)
**	    Changed to sort on (FCB,page).
**	22-Apr-2005 (schka24)
**	    Recode with explicit pointers-met tests, since we're doing
**	    dirty buffer compares and values may be changing.  I.e.
**	    buf[i] < v may be true once but false later on.
**	    Plus, I couldn't read the original "Handbook" code.
*/
static VOID
cp_sort( DM0P_BUFFER *buf_array,
         i4 cp_array[], 
register i4 lo, 
register i4 hi )
{
    register i4			i, j, v;
    register DM0P_BUFFER	*b, *ib, *jb;

    while ( hi > lo )
    {
	i = lo-1;		/* -1 for ++i */
	j = hi;
	v = cp_array[j];
	b = &buf_array[v];
	for (;;)
	{
	    /* Partition around v and b, the hi'th element.
	    ** We want to end up with a left subfile (all bufs < v),
	    ** the v element itself in its final position, and a right
	    ** subfile (all bufs > v).  Either subfile may be empty.
	    ** Elements == v are exchanged and can end up on either side.
	    **
	    ** Scan from the left looking for an element that belongs
	    ** in the right side.  (If none, this stops at i == j.)
	    ** Because of dirty refs, we need an explicit i<j test;
	    ** can't even assume that buf b == buf b!
	    */
	    do
	    {
		ib = &buf_array[cp_array[++i]];
	    } while (i < j &&
		  (ib->buf_cpfcb < b->buf_cpfcb ||
		   (ib->buf_cpfcb == b->buf_cpfcb &&
		     ib->buf_lock_key.lk_key4 < b->buf_lock_key.lk_key4)));

	    cp_array[j] = cp_array[i];
	    /* Scan from the right looking for an element that belongs
	    ** in the left side.  (If none, this stops with j = i or i-1.)
	    ** Again, need explicit j>i testing because bufs may be changing;
	    ** can't assume that all buf[x<=i] are still < buf b.
	    */
	    do
	    {
		jb = &buf_array[cp_array[--j]];
	    } while (j > i &&
		  (jb->buf_cpfcb > b->buf_cpfcb ||
		   (jb->buf_cpfcb == b->buf_cpfcb &&
		     jb->buf_lock_key.lk_key4 > b->buf_lock_key.lk_key4)));

	    /* If pointers have crossed, stop;  else move the j element to
	    ** the left side and keep partitioning.
	    */
	    if (i >= j)
		break;
	    cp_array[i] = cp_array[j];
	}
	/* If j backed up one too far, fix it, then put v in its final
	** resting place
	*/
	if (i != j)
	    ++ j;
	cp_array[j] = v;		/* v == the original cp_array[hi] */

	/* Sort subfiles lo..j-1 and j+1..hi
	** Do the smallest partition first to minimize recursion.
	** Ideally we would do very small subfiles with a simpler sort,
	** but this is good enough for now.
	**
	** Recurse to (completely) sort the smaller subfile, then adjust
	** lo or hi to delimit the larger subfile and loop to sort that one.
	*/

	if ( j - lo < hi - j )
	{
	    if ( j-1 > lo )
		cp_sort( buf_array, cp_array, lo, j-1 );
	    lo = j+1;
	}
	else
	{
	    if ( hi > j+1 )
		cp_sort( buf_array, cp_array, j+1, hi );
	    hi = j-1;
	}
    }
    return;
}

/*{
** Name: cp_flush - Flush candidate CP buffers from a CP array.
**
** Description:
**	This routine traverses a cache's CP array of candidate pages
**	and flushes each now-eligible page to disk.
**
** Inputs:
**	cfa		- pointer to DM0P_CFA if CacheFlushAgent,
**			  NULL if real CP thread.
**	cache_ix	- which CP array to process
**	lock_list	- transaction lock list
**	log_id		- transaction log id
**
** Outputs:
**      err_code                        Reason for error return status.
**	Returns:
**	    DB_STATUS
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	19-Apr-1999 (jenjo02)
**	    Written, extracted from dm0p_cp_flush().
**	12-Jul-1999 (jenjo02)
**	    Modified to traverse CP array of BIDs instead of list.
**	24-Aug-1999 (jenjo02)
**	    Added "cp_thread" input parm, TRUE if caller is genuine CP
**	    thread, FALSE if not (write_behind). 
**	11-Oct-1999 (jenjo02)
**	    Only set BUF_MUTEX on BUF_FIXED buffers to prevent unneeded
**	    mutex waits.
**	    If synch writes are occuring, abandon GatherWrite to minimize the
**	    times the buffers are in a busy state.
**	10-Jan-2000 (jenjo02)
**	    Check for temptable pages and ignore them. We made this check
**	    in cp_prep(), but the page by now may have been recycled and
**	    belongs to a temptable.
**	20-Apr-2000 (jenjo02)
**	    If WriteBehind, work array from back-to-front; if CP
**	    thread, work array from front-to-back.
**	14-Jun-2000 (jenjo02)
**	    If skipping a no-longer-modified buffer, be sure to
**	    set buf_cpcount to the current CP value.
**	21-May-2001 (jenjo02)
**	    Don't wait for BUF_BUSY buffers - they will be marked
**	    processed for the CP by force_page or fault_page.
**	16-Jul-2002 (jenjo02)
**	    Incomplete CP errors can occur in multiple server installations
**	    if one server's CP thread falls behind the cpindex.
**	31-Jan-2003 (jenjo02)
**	    Rewrote algorithms for Cache Flush Agent concepts.
**	12-Mar-2003 (jenjo02)
**	    Release buf_mutex before calling unfix_tableio_ptr(),
**	    retry buffer to avoid rare E_CL2517_CS_SMPR_DEADLOCK
**	    error in fault_page() (B109819).
*/
static DB_STATUS
cp_flush(
DM0P_CFA	*CPagent,
i4		cache_ix,
i4		lock_list,
i4		log_id,
i4		*flush_count,
i4		*err_code)
{
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BSTAT	    *lbm_stats = &lbm->lbm_stats[cache_ix];
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BMCB       *bm = lbm->lbm_bmcb[cache_ix];
    i4	    	    *cp_array = lbm->lbm_cp_array[cache_ix];
    DM0P_BUFFER	    *buf_array = lbm->lbm_buffers[cache_ix];
    DMPP_PAGE	    *page_pool = lbm->lbm_pages[cache_ix];
    DMP_TABLE_IO    *tbio = NULL;
    DM0P_BUFFER     *b;
    DMPP_PAGE	    *page;
    DB_STATUS	    status = E_DB_OK;
    DB_STATUS	    sts = E_DB_OK;
    i4	    	    gw_queued = 0;
    i4		    some_tcb_not_found = FALSE;
    i4	    	    no_tcb_dbid = 0;
    DB_TAB_ID	    no_tcb_tabid;
    i4		    CPthread = (CPagent == (DM0P_CFA*)NULL);
    i4		    i;

    *flush_count = 0;
	
    /*
    ** There may be modified pages in this cache that need to be 
    ** written, but if we have no open TCBs in this cache, then
    ** we can't contribute, so skip this cache and assume
    ** that another server has the TCB necessary to flush the
    ** pages.
    */
    if ( dmf_svcb->svcb_hcb_ptr->hcb_cache_tcbcount[cache_ix] <= 0 )
    {
	/* If Agent, skip cache */
	if ( CPagent )
	    return(E_DB_OK);

	/*
	** CP threads are solely responsible for maintaining the
	** cpindex.
	**
	** If this is the last (or only) CP thread to process this
	** cache, fall through to ensure that every buffer in
	** the CP array has been processed. This situation could
	** occur, for example, if an assisting CacheFlush Agent
	** (which does -not- maintain cpindex) has beaten us to
	** the CP array, processed every buffer, and closed every
	** TCB.
	*/
	dm0p_mlock(SEM_EXCL, &bm_common->bmcb_cp_mutex);
	if ( bm_common->bmcb_cpcheck + 1 < bm_common->bmcb_srv_count )
	{
	    dm0p_munlock(&bm_common->bmcb_cp_mutex);
	    return(E_DB_OK);
	}
	dm0p_munlock(&bm_common->bmcb_cp_mutex);
    }

    /*
    ** Cycle through this cache's CP array and write out all modified pages.
    **
    ** The CP array was created by dirty-peeking at the caches'
    ** buffers; now we reexamine those buffers with a critical eye
    ** and a mutex.
    **
    ** Pages are written for all databases, whether Fast Commit is in
    ** use on them or not.  Temporary tables need not be forced.
    **
    ** Pages are written even if they are fixed - but not while they are
    ** mutexed.  This requires checking the page_stat DMPP_MODIFY value to
    ** check the page is modified as well as the BUF_MODIFIED bit.
    **
    ** If the page is Mutexed or write-busy, then we must wait for it 
    ** to become free before proceeding.
    */
    
    /* All CP threads start at the 0-th buffer in the array */
    if ( CPthread )
	i = 0;
    else
    {
	/*
	** CacheFlush Agent:
	**
	** If Primary Agent, spawn this server's share
	** of Agents, total_cpagents / server_count.
	**
	** Each Agent begins at a predetermined buffer in the  
	** array. cp_prep() computed the size of each "slice"
	** as cpbufs divided by the total number of Agents
	** needed (log2(cpbufs-256)) and stored it in bm_cpabufs,
	** so the starting point for any Agent is
	** its cfa_id+1 * bm_cpabufs. All Agents regardless
	** of where in the array they begin work, continue
	** to the end of the array.
	**
	** Note that there may be more WriteBehind Agents already
	** running than are needed for the CP. Their CPstart
	** will yield a value beyond "cpbufs", so 
	** they'll fall into the code to reset their
	** starting buffer to the current "cpindex" value.
	*/

	i = CPagent->cfa_CPstart = (CPagent->cfa_id+1) 
				    * bm->bm_cpabufs;

	/* Trace CP assist */
	if ( bm_common->bmcb_status & BMCB_CFA_TRACE )
	    CfaTrace(CPagent, "CPJOIN");

	/*
	** Spawn additional server agents, if any needed.
	** NB: bs_cfa_active gets incremented by CloneAgent().
	*/
	while ( CPagent->cfa_type == CFA_PRIMARY &&
		(lbm_stats->bs_cfa_active < 
		    bm->bm_cpagents / bm_common->bmcb_srv_count) &&
	       !(CloneAgent(CPagent, CFA_CP, "CPCLONE")) );
    }


    /*
    ** CPthreads
    **	o maintain the cpindex
    **	o must wait for WBUSY and MUTEXed buffers
    **	o skip FCBUSY buffers
    **	o set the FCBUSY bit
    **	o set buf_cpcount before write completion
    **
    ** Agents
    **	o do not change the cpindex
    **	o skip BUSY, FCBUSY and MUTEXed buffers
    **	o do not set the FCBUSY bit
    **	o wait until write completion to set
    **	  buf_cpcount
    */

    while ( bm->bm_cpindex < bm->bm_cpbufs )
    {
	for ( i ;
	      i < bm->bm_cpbufs && bm->bm_cpindex < bm->bm_cpbufs ;
	      i++ )
	{
	    /* If Agent, quit if assigned CP has completed */
	    if ( CPagent && CPagent->cfa_CPcount != bm_common->bmcb_cpcount )
		break;

	    /*
	    ** If the cp_index has gotten ahead of our 
	    ** position, move up to it.
	    **
	    ** The cp_index guarantees that all buffers in the
	    ** cp array prior to it have been successfully 
	    ** written or checked by some CP thread.
	    */
	    if ( bm->bm_cpindex > i )
	    {
		i = bm->bm_cpindex-1;
		continue;
	    }

	    b = &buf_array[cp_array[i]];

	    if ( bm_common->bmcb_status & BMCB_IS_MT )
	    {
		/* If temptable buffer, no need to write it */
		if ( b->buf_sts & BUF_TEMPTABLE )
		    b->buf_cpcount = bm_common->bmcb_cpcount;

		/* If already written for this CP, update the cpindex and move on.*/
		if (b->buf_cpcount == bm_common->bmcb_cpcount)
		{
		    /* If the cp index points to this buffer, move it up */
		    if ( CPthread && bm->bm_cpindex <= i )
		    {
			dm0p_mlock(SEM_EXCL, &bm->bm_mutex);
			if (bm->bm_cpindex == i)
			    bm->bm_cpindex++;
			dm0p_munlock(&bm->bm_mutex);
		    }
		    continue;
		}
		    
		/* Agents can safely skip BUSY and MUTEXed buffers */
		if ( CPagent && b->buf_sts & (BUF_BUSY | BUF_WBBUSY | BUF_MUTEX) )
		    continue;

		/* If buffer is being processed by another CP thread, skip it */
		if ( b->buf_sts & BUF_FCBUSY )
		    continue;
	    }

	    /* Lock the buffer, recheck everything */
	    dm0p_mlock(SEM_EXCL, &b->buf_mutex);

	    /*
	    ** Mark the buffer processed by this CP if it:
	    **
	    **    o belongs to a temporary table
	    **      (temptable pages are never flushed)
	    **    o is read busy. Read-busy pages
	    **      are being faulted in, hence won't yet
	    **      be modified. Write-busy pages are being
	    **      forced elsewhere, but the CP thread must
	    **	    wait for write completion to ensure
	    ** 	    that the I/O was successful.
	    */
	    if ( b->buf_sts & (BUF_TEMPTABLE | BUF_RBUSY) )
		b->buf_cpcount = bm_common->bmcb_cpcount;

	    /* Check everything again after acquiring the mutex: */
	    if (b->buf_cpcount == bm_common->bmcb_cpcount)
	    {
		dm0p_munlock(&b->buf_mutex);

		/* If the cp index points to this buffer, move it up */
		if ( CPthread && bm->bm_cpindex <= i )
		{
		    dm0p_mlock(SEM_EXCL, &bm->bm_mutex);
		    if (bm->bm_cpindex == i)
			bm->bm_cpindex++;
		    dm0p_munlock(&bm->bm_mutex);
		}
		continue;
	    }

	    /* Agents can safely skip all BUSY and MUTEXed buffers */
	    if ( CPagent && b->buf_sts & (BUF_BUSY | BUF_WBBUSY | BUF_MUTEX) )
	    {
		dm0p_munlock(&b->buf_mutex);
		continue;
	    }

	    /* If buffer is being processed by another CP thread, skip it */
	    if (b->buf_sts & BUF_FCBUSY)
	    {
		dm0p_munlock(&b->buf_mutex);
		continue;
	    }

	    /* CP thread must wait/retry mutexed or write-busy buffer */
	    if ( b->buf_sts & (BUF_MUTEX | BUF_WBUSY) )
	    {
		/*
		** Page is mutex locked or write-busy; 
		** Wait for the buffer, then try again.
		*/

		/*
		** Need to force any I/O's which are being done by this thread
		** as a gather write (GW).  This prevents deadlock between GW's
		** such that they aren't each waiting for I/O's by the other
		** GW which haven't been forced out yet (and won't be because
		** they are waiting for each other).
		*/
		if (gw_queued)
		{
		    dm0p_munlock(&b->buf_mutex);
		    if ( status = dm2f_force_list() )
			break;
		}
		else
		{
		    /*
		    ** If we have a tbio fixed for some other table,
		    ** unfix it to avoid deadlock.
		    */
		    if ( tbio &&
			(b->buf_dbid != tbio->tbio_dcbid ||
			 b->buf_tabid.db_tab_base != tbio->tbio_reltid.db_tab_base ||
			 b->buf_tabid.db_tab_index != tbio->tbio_reltid.db_tab_index) )
		    {
			/* Release buffer, TBIO, retry buffer */
			dm0p_munlock(&b->buf_mutex);
			unfix_tableio_ptr(&tbio, lock_list, log_id, (i4)__LINE__);
			i--;
			continue;
		    }

		    if ( b->buf_sts & BUF_MUTEX )
		    {
			lbm_stats->bs_mwait[BMCB_PTYPES]++;
			bm->bm_stats.bs_mwait[BMCB_PTYPES]++;
			lbm_stats->bs_mwait[b->buf_type]++;
			bm->bm_stats.bs_mwait[b->buf_type]++;
			b->buf_sts |= BUF_MWAIT;
			dm0p_eset(lock_list, DM0P_PGMUTEX_EVENT,
				    cache_ix, b->buf_id.bid_index, 0);
			dm0p_munlock(&b->buf_mutex);

			/* Wait for the unmutex event. */
			dm0p_ewait(lock_list, DM0P_PGMUTEX_EVENT,
				    cache_ix, b->buf_id.bid_index, 0, 0);
		    }
		    else if ( b->buf_sts & BUF_WBUSY )
		    {
			lbm_stats->bs_iowait[BMCB_PTYPES]++;
			bm->bm_stats.bs_iowait[BMCB_PTYPES]++;
			lbm_stats->bs_iowait[b->buf_type]++;
			bm->bm_stats.bs_iowait[b->buf_type]++;
			b->buf_sts |= BUF_IOWAIT;
			dm0p_eset(lock_list, DM0P_BIO_EVENT,
				    cache_ix, b->buf_id.bid_index, 0);
			dm0p_munlock(&b->buf_mutex);

			/* Wait for write-completion. */
			dm0p_ewait(lock_list, DM0P_BIO_EVENT,
				    cache_ix, b->buf_id.bid_index, 0, 0);
		    }
		}

		i--;
		continue;
	    }

	    /*
	    ** If the buffer is not valid, or is not modified then we can
	    ** skip it without flushing it.  Mark that this buffer has
	    ** been processed for this consistency point.
	    **
	    ** If the buffer is fixed, then we have to check the page itself
	    ** to see if it is modified as well as the buffer header.  We must
	    ** flush any logged update, even if that update is on a page which
	    ** has just been modified and has not yet been unfixed.  (A page
	    ** can only be modified while fixed so the state of an unfixed page
	    ** can be determined from the buffer header alone.)
	    */
	    page = (DMPP_PAGE *)((char *)page_pool + b->buf_page_offset);


	    if (((b->buf_sts & BUF_VALID) == 0) ||
		(((b->buf_sts & BUF_MODIFIED) == 0) &&
		 (((b->buf_sts & BUF_FIXED) == 0) || 
		  ((DMPP_VPT_GET_PAGE_STAT_MACRO(b->buf_relpgtype, page) & 
		    DMPP_MODIFY) == 0))))
	    {
		/* Mark buffer as having been flushed for this CP */
		b->buf_cpcount = bm_common->bmcb_cpcount;
		dm0p_munlock(&b->buf_mutex);

		/* If the cp index points to this buffer, move it up */
		if ( CPthread && bm->bm_cpindex <= i )
		{
		    dm0p_mlock(SEM_EXCL, &bm->bm_mutex);
		    if (bm->bm_cpindex == i)
			bm->bm_cpindex++;
		    dm0p_munlock(&bm->bm_mutex);
		}
		continue;
	    }

	    /* 
	    ** If we know this page belongs to a TCB which does not exist on
	    ** this server, skip the buffer without marking it processed by 
	    ** this CP.
	    */
	    if (some_tcb_not_found &&
		b->buf_dbid == no_tcb_dbid &&
		b->buf_tabid.db_tab_base == no_tcb_tabid.db_tab_base &&
		b->buf_tabid.db_tab_index == no_tcb_tabid.db_tab_index)
	    {
		dm0p_munlock(&b->buf_mutex);
		continue;
	    }

	    /*
	    ** Look for Table Descriptor for this buffer.
	    ** If the TABIO is currently busy we must wait for it in case this
	    ** buffer must be forced.  The BUSY state of the table may indicate
	    ** that dirty pages are already being forced, but we have no 	
	    ** guarantee that those forces will complete before the end of the .
	    ** CP.
	    */
	    if (!tbio ||
		b->buf_dbid != tbio->tbio_dcbid ||
		b->buf_tabid.db_tab_base != tbio->tbio_reltid.db_tab_base ||
		b->buf_tabid.db_tab_index != tbio->tbio_reltid.db_tab_index)
	    {
		if ( tbio )
		{
		    /* Wrong table, release buffer, TBIO, retry */
		    dm0p_munlock(&b->buf_mutex);
		    unfix_tableio_ptr(&tbio, lock_list, log_id, (i4)__LINE__);
		    i--;
		    continue;
		}

		/* Can't wait for busy TBIO if we have uncompleted gatherwrites */
		if (gw_queued)
		{
		    sts = fix_tableio_ptr(b, 0, lock_list,
			log_id, &tbio, err_code);
		    /* If we'd have to wait, force gatherwrites then retry */
		    if ( sts == E_DB_WARN )
		    {
			dm0p_munlock(&b->buf_mutex);
			if ( status = dm2f_force_list() )
			    break;
		    }
		}
		else
		{
		    sts = fix_tableio_ptr(b, 
			(i4)(DM0P_TBIO_WAIT | DM0P_TBIO_MUTEX_HELD), lock_list, 
			log_id, &tbio, err_code);
		}

		if (sts == E_DB_WARN)
		{
		    /*
		    ** We had to wait for a busy Table Descriptor.  Retry the 
		    ** operation on the same buffer.
		    ** buffer's mutex unlocked by fix_tableio_ptr().
		    */
		    i--;
		    continue;
		}
		/*
		** Ignore any errors and continue as though we did not find
		** a table descriptor (tbio == NULL). This may lead to an
		** incomplete CP error (since this buffer may not be written
		** out), but there is a chance another server will successfully
		** write it and spare us the embarassment of a total installation
		** crash.
		*/
	    }

	    /*
	    ** If the Table Descriptor is not known to this server,
	    ** skip the buffer without flushing it.
	    */
	    if (tbio == NULL)
	    {
		/* Remember the last TCB not know on this server */
		some_tcb_not_found = TRUE;
		no_tcb_dbid = b->buf_dbid;
		no_tcb_tabid.db_tab_base = b->buf_tabid.db_tab_base;
		no_tcb_tabid.db_tab_index = b->buf_tabid.db_tab_index;

		dm0p_munlock(&b->buf_mutex);
		continue;
	    }

	    /*
	    ** Turn on mutex on this page to protect it from being modified
	    ** while its being written and the mutex_ok bit to indicate
	    ** that the buffer may be busy even though not fixed.
	    **
	    ** The buffer only needs to be MUTEXed if it is fixed.
	    */
	    if ( b->buf_sts & BUF_FIXED )
		b->buf_sts |= (BUF_MUTEX | BUF_MUTEX_OK);

	    /*
	    ** The fastcommit busy bit is turned on to signify that
	    ** this buffer is currently being processed by a real
	    ** CP thread, not an Agent.
	    **
	    ** This prevents a CP failure which would occur if:
	    **
	    **	o Agent selects a CP buffer to write, sets
	    **	  FCBUSY, and is put to sleep by the OS before
	    **	  the write can complete.
	    **	o CP thread sees FCBUSY, ignores the buffer, and
	    **	  runs off the end of the CP queue and returns to
	    **	  dm0p_cp_flush which thinks the CP has completed,
	    **	  checks the cpindex, finds the inconsistency,
	    **	  and crashes.
	    */
	    if ( CPthread )
		b->buf_sts |= BUF_FCBUSY;

	    /*
	    ** If the page is marked dirty then mark the buffer dirty so
	    ** force_page will write it.
	    **
	    ** If the page is marked modified but the buffer is not fixed for
	    ** write, then don't write the page - this signifies a server error!
	    */
	    if (DMPP_VPT_GET_PAGE_STAT_MACRO(b->buf_relpgtype, page) & DMPP_MODIFY)
	    {
		if ((b->buf_sts & (BUF_FIXED | BUF_WRITE)) != 
				  (BUF_FIXED | BUF_WRITE))
		{
		    ule_format(E_DM9346_DIRTY_PAGE_NOT_FIXED, (CL_SYS_ERR *)0,
			ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, 
			err_code, 5,
			0, 
			DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, page),
			sizeof(DB_TAB_NAME), tbio->tbio_relid,
			sizeof(DB_OWN_NAME), tbio->tbio_relowner,
			sizeof(DB_DB_NAME), tbio->tbio_dbname,
			0, b->buf_sts);
		    dmd_check(E_DMF023_BM_CP_FLUSH_SEVERE);
		}

		lbm_stats->bs_dirty[BMCB_PTYPES]++;
		bm->bm_stats.bs_dirty[BMCB_PTYPES]++;
		lbm_stats->bs_dirty[b->buf_type]++;
		bm->bm_stats.bs_dirty[b->buf_type]++;
		b->buf_sts |= BUF_MODIFIED;
		DMPP_VPT_CLR_PAGE_STAT_MACRO(b->buf_relpgtype, page, DMPP_MODIFY);
	    }

	    /*
	    ** This page is modified. Call force_page to write it to disk.
	    ** Force_page will turn off the modified bit.
	    */

	    /* Count another force */
	    lbm_stats->bs_force[BMCB_PTYPES]++;
	    bm->bm_stats.bs_force[BMCB_PTYPES]++;
	    lbm_stats->bs_force[b->buf_type]++;
	    bm->bm_stats.bs_force[b->buf_type]++;

	    /* complete_f_p needs to know buffer's cpindex */
	    b->buf_cpindex = i;

	    /*
	    ** If CPthread, mark this buffer as processed by this
	    ** CP. If CPagent, this won't happen until the
	    ** write completes, causing the CPthreads to wait
	    ** for the Agents' I/O, just the behavior wanted;
	    ** the CP can't "complete" until all I/O is accounted
	    ** for.
	    **
	    ** If the write fails, complete_f_p() will set
	    ** bm_cpindex to this buffer, leading to a CP failure.
	    */
	    if ( CPthread )
		b->buf_cpcount = bm_common->bmcb_cpcount;

	    status = force_page(b, tbio, FP_WRITE,
		lock_list, log_id, BMCL_CP_FLUSH, &gw_queued, err_code);

	    /* Check outcome of force_page */
	    if ( status == E_DB_OK )
	    {
		(*flush_count)++;

		/* Add to CPagent's running total */
		if ( CPagent )
		    CPagent->cfa_CPflushed++;

		/* If the cp index points to this buffer, move it up */
		if ( CPthread && bm->bm_cpindex <= i )
		{
		    dm0p_mlock(SEM_EXCL, &bm->bm_mutex);
		    if (bm->bm_cpindex == i)
			bm->bm_cpindex++;
		    dm0p_munlock(&bm->bm_mutex);
		}
	    }
	    else 
	    {
		dm0p_munlock(&b->buf_mutex);

		if (status == E_DB_WARN)
		{
		    /* If retry returned from force_page(), then retry. */
		    status = E_DB_OK;
		    i--;
		}
		/* Error from force_page() */
		else 
		    break;
	    }
	}

	/* Force any remaining queued GatherWrite buffers */
	if ( gw_queued )
	{
	    sts = dm2f_force_list();
	    status = status ? status : sts;
	}
	    
	/*
	** If CPagent has finished its slice of buffers, help
	** out a straggling cpindex.
	**
	** This could easily happen, for example, if the
	** CP thread itself has not yet had a chance to start
	** in this cache (it may be processing another cache).
	** While the CPagents cannot advance the cpindex,
	** they can ensure that all the CP buffers get flushed
	** so that by the time the CP thread enters this
	** cache, all the buffers have been marked "flushed
	** for this cp", making the CP thread's job much
	** easier.
	**
	** Note that we do this only once. The Agent may
	** be unable to flush any (remaining) buffers in
	** this cache and can't by itself advance the cpindex,
	** so one shot at helping out will suffice.
	*/
	if ( CPagent && CPagent->cfa_tasks & CFA_CP &&
	     status == E_DB_OK && 
	     bm->bm_cpindex < bm->bm_cpbufs )
	{
	    i = CPagent->cfa_CPstart = bm->bm_cpindex;
	    if ( bm_common->bmcb_status & BMCB_CFA_TRACE )
		CfaTrace(CPagent, "CPCONT");
	    /* This is how we limit to one iteration */
	    CPagent->cfa_tasks &= ~CFA_CP;
	    continue;
	}
	break;
    }

    /* If there's a residual TBIO laying around, release it */
    if (tbio)
	unfix_tableio_ptr(&tbio, lock_list, log_id, (i4)__LINE__);


    if (DB_FAILURE_MACRO(status) && (*err_code > E_DM_INTERNAL))
    {
	ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL, (char *)0,
	    (i4)0, (i4 *)0, err_code, 0);
	*err_code = E_DM9344_BM_CP_FLUSH_ERROR;
    }

    return (status);
}

/*{
** Name: dm0p_start_wb_threads - Start the primary WriteBehind Agents
**
** Description:
**	This function is called during server startup to 
**	start the primary WriteBehind Agents, one per cache.
**
** Inputs:
**
** Outputs:
**
** Side Effects:
**	    none
**
** History:
**	17-Apr-1998 (jenjo02)
**	    Written.
**	01-Jun-1998 (jenjo02)
**	    Check for LBMCB; if STAR server, there won't be one.
**	31-Jan-2003 (jenjo02)
**	    Allocate a durable DM0P_CFA for each Primary Agent.
*/
STATUS
dm0p_start_wb_threads( VOID )
{
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BMCB       *bm;
    SCF_CB    	    scf_cb;
    SCF_FTC	    ftc;
    STATUS	    status = OK;
    i4		    cache_ix;
    char	    thread_name[DB_MAXNAME];
    DM0P_CFA        cfa;

    if (lbm)
    {
	for (cache_ix = 0;
	     cache_ix < DM_MAX_CACHE && status == OK;
	     cache_ix++)
	{
	    bm = lbm->lbm_bmcb[cache_ix];

	    /* 
	    ** If this cache isn't allocated or it's configured
	    ** without WriteBehind Agents, skip it.
	    */
	    if (!bm || (bm->bm_status & BM_WB_IN_CACHE) == 0)
	    {
		continue;
	    }

	    /* Start a Primary WB Agent in this cache */
	    scf_cb.scf_type = SCF_CB_TYPE;
	    scf_cb.scf_length = sizeof(SCF_CB);
	    scf_cb.scf_session = DB_NOSESSION;
	    scf_cb.scf_facility = DB_DMF_ID;
	    scf_cb.scf_ptr_union.scf_ftc = &ftc;

	    cfa.cfa_cache_ix = cache_ix;
	    cfa.cfa_type = CFA_PRIMARY;
	    cfa.cfa_tasks = 0;
	    cfa.cfa_id = 0;

	    /*
	    ** WB Agents run at a high priority and need no facilities.
	    ** They should run at the -same- priority as the CP
	    ** threads.
	    **
	    ** SCF will allocate a thread-persistent copy of
	    ** this Agent's DM0P_CFA and free it when the
	    ** thread terminates.
	    ** 
	    */
	    ftc.ftc_facilities = 0;
	    ftc.ftc_data = (char*)&cfa;
	    ftc.ftc_data_length = sizeof(cfa);
	    STprintf(thread_name, " <WriteBehind %dK> ", 
			bm->bm_pgsize >> 10);
	    ftc.ftc_thread_name = &thread_name[0];
	    ftc.ftc_priority = SCF_MAXPRIORITY - 1;
	    ftc.ftc_thread_entry = dmc_write_behind_primary;
	    ftc.ftc_thread_exit = NULL;
	    status = scf_call(SCS_ATFACTOT, &scf_cb);
	}
    }

    return(status);

}

/*{
** Name: dm0p_wbflush_wait - wait for write behind event
**
** Description:
**	This routine goes into a wait state, waiting for the write-behind
**	event to be signaled.  It is intended to be called only from a
**	primary write behind thread.
**
**	The method of waiting for the WB event is to call LKevent using
**	the buffer manager id and the cache number to specify this
**	particular event. When the buffer manager sees that the 
**	writebehind start level has been reached in a cache, LKevent
**	will be called to signal the write behind event.
**
**	At server shutdown time, all dbms task threads are interrupted in
**	order to terminate them.  When this occurs, the LKevent call in
**	this routine (the method by which the routine waits for the WB event)
**	will return with an interrupt status.  This routine will return with
**	E_DB_WARN, signifying that the thread should terminate.
**
** Inputs:
**	cache_ix	- cache being serviced by this WB thread
**	lock_list	- transaction lock list.
**
** Outputs:
**      err_code                        Reason for error return status.
**	Returns:
**	    E_DB_OK
**	    E_DB_WARN	- Session interrupted
**	    E_DB_FATAL
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	29-aug-1988 (rogerk)
**	    written.
**	28-feb-1989 (rogerk)
**	    Added support for Shared Buffer Manager.  Use dm0p_e.. routines
**	    instead of calling LKevent directly.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	20-May-1998 (jenjo02)
**	    Modified for cache-specific primary WB threads.
**	29-Jul-2002 (jenjo02)
**	    bm_mutex must be held while bs_cfa_active is updated.
**	31-Jan-2003 (jenjo02)
**	    Adapted for Cache Flush Agents.
*/
DB_STATUS
dm0p_wbflush_wait(
char	    *cfap,
i4	    lock_list,
i4	    *err_code)
{
    DM0P_CFA        *cfa = (DM0P_CFA*)cfap;
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BSTAT	    *lbm_stats = &lbm->lbm_stats[cfa->cfa_cache_ix];
    DM0P_BMCB	    *bm = lbm->lbm_bmcb[cfa->cfa_cache_ix];
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DB_STATUS	    status;

    dm0p_mlock(SEM_EXCL, &bm->bm_mutex);

    /* Check for end of WB cycle */
    wb_cycle_end(cfa);

    do
    {
	/* The primary WB agents are persistent and wait for WB event signals */
	status = dm0p_eset(lock_list, DM0P_WBFLUSH_EVENT, bm->bm_cache_ix, 0, 1);
	dm0p_munlock(&bm->bm_mutex);

	/*
	** Wait for the Write Behind event to be signalled.
	** Will return with a warning if the wait is interrupted.
	*/
	if (status == E_DB_OK)
	    status = dm0p_ewait(lock_list, DM0P_WBFLUSH_EVENT, bm->bm_cache_ix, 0,
				    1, LK_E_INTERRUPT);

	if (status == E_DB_OK)
	{
	    /* Assign cache-unique Agent id */
	    dm0p_mlock(SEM_EXCL, &bm->bm_mutex);

	    cfa->cfa_id = 0;

	    while ( bm->bm_cfa_mask & (1 << cfa->cfa_id)
		    && ++cfa->cfa_id < BITS_IN(bm->bm_cfa_mask) );

	    if ( cfa->cfa_id < BITS_IN(bm->bm_cfa_mask) )
	    {
		bm->bm_cfa_mask |= (1 << cfa->cfa_id);

		/* Count another active cache agent */
		if (++bm->bm_stats.bs_cfa_active > bm->bm_stats.bs_cfa_hwm)
		    bm->bm_stats.bs_cfa_hwm = bm->bm_stats.bs_cfa_active;

		/* Initialize Server's WB variables */
		if (++lbm_stats->bs_cfa_active > lbm_stats->bs_cfa_hwm)
		    lbm_stats->bs_cfa_hwm = lbm_stats->bs_cfa_active;

		lbm_stats->bs_cache_flushes++;
		lbm_stats->bs_wb_flushed = 0;
		lbm_stats->bs_wb_gflushed = 0;
		dm0p_munlock(&bm->bm_mutex);
	    }
	    else
	    {
		/* ...then the max Agents are running */
		status = E_DB_INFO;
		/* Wait for the next signal */
	    }
	}
	else if (status != E_DB_WARN)
	    *err_code = E_DM9345_BM_WBFLUSH_WAIT;
    } while ( status == E_DB_INFO );

    return (status);
}

/*{
** Name: wb_cycle_end - Check for end of WB Cycle.
**
** Description:
**	Called when a primary or cloned WB agent completes
**	to decrement the count of WB agents in the
**	the cache and server, and unassign the Agent's id.
**	When the cache's Agent mask is zero, the cache is marked
**	as waiting for the next WB event.
**
** Inputs:
**	cfa		Agent's DM0P_CFA
**
**	Caller must hold bm_mutex.
**
** Outputs:
**
** Side Effects:
**	    none
**
** History:
**	15-Sep-1997 (jenjo02)
**	    Written.
**	31-Jan-2003 (jenjo02)
**	    Adapted for Cache Flush Agents.
*/
static VOID
wb_cycle_end(
DM0P_CFA	*cfa)
{
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BSTAT	    *lbm_stats = &lbm->lbm_stats[cfa->cfa_cache_ix];
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BMCB	    *bm = lbm->lbm_bmcb[cfa->cfa_cache_ix];

    /* 
    ** Reset this agent's mask bit.
    ** When all bits are zero, all agents 
    ** have left the building and
    ** the cycle ends.
    */
    if ( (bm->bm_cfa_mask &= ~(1 << cfa->cfa_id)) == 0 )
    {
	bm->bm_status |= BM_WBWAIT;
	bm->bm_status &= ~(BM_WB_SINGLE | BM_WB_GROUP | BM_WB_CP);
    }

    /*
    ** Decrement the count of active WB threads
    ** in the server and cache if this is the
    ** end of a real cycle.
    */
    if ( lbm_stats->bs_cache_flushes )
	lbm_stats->bs_cfa_active--;

    if ( bm->bm_stats.bs_cache_flushes )
	bm->bm_stats.bs_cfa_active--;

    /* Trace Agent termination */
    if ( bm_common->bmcb_status & BMCB_CFA_TRACE &&
	 lbm_stats->bs_cache_flushes )
	CfaTrace(cfa, cfa->cfa_type == CFA_PRIMARY 
			? "WBWAIT" : "EXIT");

    return;
}

/*{
** Name: wb_cycle_start - Signal a WriteBehind event in a cache
**
** Description:
**	This function begins a new WriteBehind Cycle, signalling
**	a cache's primary CacheFlush Agent in each connected server.
**
** Inputs:
**	bm		The cache to flush.
**	lock_list	A lock list to use for LKevent.
**	why		Primary reason for flush:
**			    BM_WB_SINGLE	flush single buffers
**			    BM_WB_GROUP		flush group buffers
**			    BM_WB_CP		assist CP
**	where		Arbitrary string for DM414 trace
**
** Outputs:
**
** Side Effects:
**	    none
**
** History:
**	15-Sep-1997 (jenjo02)
**	    Written.
**	31-Jan-2003 (jenjo02)
**	    Adapted for Cache Flush Agents.
**      20-Sep-2006 (wonca01) BUG 114779
**          Create error during cross integration of change 479103 from Ingres 26
**          Instead of moving dm0p_munlock() an extra call was made.
*/
static VOID
wb_cycle_start(
DM0P_BMCB *bm,
i4 	   lock_list,
i4	   why,
char 	  *where)
{

    /* Is a WB cycle already in progress? */
    if (bm->bm_status & BM_WBWAIT)
    {
	DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
	DM0P_BSTAT	    *lbm_stats = &lbm->lbm_stats[bm->bm_cache_ix];
	DM0P_BM_COMMON      *bm_common = lbm->lbm_bm_common;

	dm0p_mlock(SEM_EXCL, &bm->bm_mutex);
	
	if (bm->bm_status & BM_WBWAIT)
	{
	    bm->bm_status &= ~BM_WBWAIT;
	    bm->bm_status |= why;

	    /* Count another Cache flush */
	    bm->bm_stats.bs_cache_flushes++;

	    /* Initialize WB variables for this cache */
	    bm->bm_stats.bs_wb_flushed = 0;
	    bm->bm_stats.bs_wb_gflushed = 0;

	    /*
	    ** If tracing, show session issuing the signal
	    ** and from whence it came.
	    */
	    if ( bm_common->bmcb_status & BMCB_CFA_TRACE )
	    {
		PID	    pid;
		CS_SID      sid;

		PCpid(&pid);
		CSget_sid(&sid);

		TRdisplay("%@ %dK WB cycle %d Signalled from %s by (%d,%x) for %s\n",
		    bm->bm_pgsize >> 10,
		    bm->bm_stats.bs_cache_flushes,
		    where,
		    pid,
		    sid,
		    (why == BM_WB_SINGLE) ? "SINGLE" :
		    (why == BM_WB_GROUP)  ? "GROUPS" :
		    (why == BM_WB_CP)     ? "CP" : "?");
	    }

	    dm0p_munlock(&bm->bm_mutex);

	    /* Awaken primary cache WB thread in each server */
	    dm0p_erelease(lock_list, DM0P_WBFLUSH_EVENT, 
				bm->bm_cache_ix, 0, 1);
	}
	else
	    dm0p_munlock(&bm->bm_mutex);
    }

    return;
}

/*{
** Name: CloneAgent - Clone a CacheFlushAgent
**
** Description:
**	This function spawns another CacheFlushAgent
**
** Inputs:
**	cfa			Caller's DM0P_CFA
**	task			Assigned task:
**			CFA_SINGLE - flush single page buffers
**			CFA_GROUP  - flush groups
**			CFA_CP     - Assist CP.
**	where			string for tracing
**
** Outputs:
**	Returns OK if an agent was started
**
** Side Effects:
**	Another thread is added to the server.
**
** History:
**	15-Sep-1997 (jenjo02)
**	    Written.
**	05-Feb-1999 (jenjo02)
**	    Assign lbm_stats->bs_cfa_cloned to thread name to 
**	    avoid duplicate thread names.
**	29-Jul-2002 (jenjo02)
**	    bm_mutex must be held while bs_cfa_active is updated.
**	31-Jan-2003 (jenjo02)
**	    Adapted for Cache Flush Agents.
*/
static STATUS
CloneAgent(
DM0P_CFA	*cfa,
i4		tasks,
char		*where)
{
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BSTAT	    *lbm_stats = &lbm->lbm_stats[cfa->cfa_cache_ix];
    DM0P_BMCB       *bm = lbm->lbm_bmcb[cfa->cfa_cache_ix];
    SCF_CB    	    scf_cb;
    SCF_FTC	    ftc;
    STATUS	    status;
    char	    thread_name[DB_MAXNAME];
    DM0P_CFA        Ccfa;

    /* Trace the clone operation */
    if ( bm_common->bmcb_status & BMCB_CFA_TRACE )
	CfaTrace(cfa, where);

    /* Create another WB thread */
    scf_cb.scf_type = SCF_CB_TYPE;
    scf_cb.scf_length = sizeof(SCF_CB);
    scf_cb.scf_session = DB_NOSESSION;
    scf_cb.scf_facility = DB_DMF_ID;
    scf_cb.scf_ptr_union.scf_ftc = &ftc;

    /*
    ** Clone an agent with the same priority and
    ** facilities of the creating agent.
    **
    ** SCF will allocate a thread-persistent copy of
    ** this Agent's DM0P_CFA and free it when the
    ** thread terminates.
    */
    Ccfa.cfa_cache_ix = cfa->cfa_cache_ix;
    Ccfa.cfa_type = CFA_CLONE;
    Ccfa.cfa_tasks = tasks;
    Ccfa.cfa_id = 0;

    /* Assign unique cache Agent id */
    dm0p_mlock(SEM_EXCL, &bm->bm_mutex);

    while ( bm->bm_cfa_mask & (1 << Ccfa.cfa_id)
	    && ++Ccfa.cfa_id < BITS_IN(bm->bm_cfa_mask) );

    if ( Ccfa.cfa_id < BITS_IN(bm->bm_cfa_mask) )
    {
	bm->bm_cfa_mask |= (1 << Ccfa.cfa_id);

	if (++bm->bm_stats.bs_cfa_active > bm->bm_stats.bs_cfa_hwm)
	    bm->bm_stats.bs_cfa_hwm = bm->bm_stats.bs_cfa_active;
	bm->bm_stats.bs_cfa_cloned++;
	if (++lbm_stats->bs_cfa_active > lbm_stats->bs_cfa_hwm)
	    lbm_stats->bs_cfa_hwm = lbm_stats->bs_cfa_active;
	lbm_stats->bs_cfa_cloned++;
	dm0p_munlock(&bm->bm_mutex);

	ftc.ftc_facilities = SCF_CURFACILITIES;
	ftc.ftc_data = (char*)&Ccfa;
	ftc.ftc_data_length = sizeof(Ccfa);
	STprintf(thread_name, " <CacheAgent %dK.%d> ", 
	    bm->bm_pgsize >> 10, Ccfa.cfa_id);
	ftc.ftc_thread_name = &thread_name[0];
	ftc.ftc_priority = SCF_CURPRIORITY;
	ftc.ftc_thread_entry = dmc_write_behind_clone;
	ftc.ftc_thread_exit = dm0p_wb_clone_exit;

	status = scf_call(SCS_ATFACTOT, &scf_cb);

	if ( status )
	{
	    /* Repair cache agent mask and stats */
	    dm0p_mlock(SEM_EXCL, &bm->bm_mutex);
	    bm->bm_cfa_mask &= ~(1 << Ccfa.cfa_id);
	    bm->bm_stats.bs_cfa_cloned--;
	    bm->bm_stats.bs_cfa_active--;
	    lbm_stats->bs_cfa_cloned--;
	    lbm_stats->bs_cfa_active--;
	    dm0p_munlock(&bm->bm_mutex);
	}
    }
    else
    {
	/* ...then the max Agents are running */
	status = E_DB_INFO;
	dm0p_munlock(&bm->bm_mutex);
    }


    return(status);

}

/*{
** Name: dm0p_wb_clone_exit - Cloned agent termination
**
** Description:
**	This function is called by SCF when a cloned CacheAgent terminates.
**
** Inputs:
**	SCF_FTX		Factotum thread execution cb
**	  ftx_data	pointer to Agent's CFA
**
** Outputs:
**	none
**
** Side Effects:
**	    none
**
** History:
**	10-Mar-1998 (jenjo02)
**	    Written.
**	31-Jan-2003 (jenjo02)
**	    Adapted for Cache Flush Agents.
*/
void
dm0p_wb_clone_exit(
SCF_FTX		*ftx)
{
    DM0P_CFA        *cfa = (DM0P_CFA*)ftx->ftx_data;
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BMCB       *bm = lbm->lbm_bmcb[cfa->cfa_cache_ix];

    /* Remove Agent from cache, check for end of cycle */
    dm0p_mlock(SEM_EXCL, &bm->bm_mutex);
    wb_cycle_end(cfa);
    dm0p_munlock(&bm->bm_mutex);

    return;
}

/*{
** Name: dm0p_flush_pages - flush modified pages from buffer cache.
**
** Description:
**	This routine is used to flush modified pages from a buffer cache
**	so that new pages can be read in.
**
**	It is intended to be called from a dedicated write behind thread
**	when the buffer manager reaches its modify limit.  This will keep
**	user threads from having to do synchronous writes in order to make
**	space for new pages.
**
**	This routine continues writing pages from the modify queues until
**	the write behind end level (bm_wbend) is reached.  It starts with
**	low priority pages and then continues upward.  It will also check
**	the free page count to make sure it is above the free page limit.
**	If there are not enough free pages then we will continue flushing
**	pages even if the bm_wbend level has been reached (currently, this
**	routine will write until we have 5% more pages than the free limit).
**
**	Each time a page is written, it returns back to the lowest priority
**	queue in case any new pages have been put there that can be written.
**	Hopefully this should not cause too much extra overhead scanning
**	the priority queues - if the lower priority queues are still empty
**	then it should not cost too much to verify that (one check per 
**	priority).
**
**	If this Buffer Manager is configured to be used by multiple servers
**	(shared memory buffer manager), then some buffers may not be able
**	to be written by every server (a server will not write a buffer for
**	which it does not already have an open TCB).  This routine will look
**	up the TCB for each modified buffer on the servers tcb list.  If there
**	is not a TCB then the buffer will be skipped and we will go on to the
**	next one. A count of open TCBs per cache is maintained in the DMF_SVCB;
**	if there are no open TCBs in this thread's cache, we do nothing.
**	
**
**	Note that if the servers user threads are creating new modified pages
**	faster than the write behind threads can write them out, then this
**	routine may never return - it will just go on writing pages never
**	reaching the bm_wbend limit.
**	
**
** Inputs:
**	lock_list	- transaction lock list.
**	log_id		- transaction log id.
**	cfap		- pointer to Agent's DM0P_CFA
**			  describing the task(s) to perform.
**
** Outputs:
**      err_code                        Reason for error return status.
**	Returns:
**	    E_DB_OK
**	    E_DB_FATAL
**	Exceptions:
**	    none
**
** Side Effects:
**	    May spawn additional WriteBehind agents.
**
** History:
**	29-aug-1988 (rogerk)
**	    written.
**	28-feb-1989 (rogerk)
**	    Added support for Shared Buffer Manager.
**	    Look up TCB for buffers to send to force_page - skip buffers for
**	    which we have no TCB.
**      11-sep-1990 (jonb)
**	    Integrate ingres6301 change 280136 (rogerk):
**            Pass buf_tcb_stamp value to dm2t_get_tcb_ptr.
**	24-oct-1991 (jnash)
**	    Initialize priority, caught by LINT
**	26-oct-1992 (rogerk)
**	    Reduced logging project: changed references to tcb fields to
**	    use new tcb_table_io structure fields.  Changed from old
**	    dm2t_get_tcb_ptr to new fix_tableio_ptr.
**	18-oct-1993 (rogerk)
**	    Changed consistency points to not start use the write behind
**	    threads to assist in flushing the cache.  Changed flush algorithm
**	    to not be sensitive to Consistency Points.
**	19-apr-1995 (cohmi01)
**	    Add support for group buffers to write-behind thread. Criteria
**	    for determining victim takes into account use for sec indexes.
**      08-feb-1996 (stial01)
**          dm0p_flush_pages() Don't look at bm_gmcount unless the buffer 
**          mutex is held, OR expect that the group modify queue may be
**          empty by the time you get the mutex.  
**          We saw a non zero bm_gmcount, got the mutex, by which time the
**          group modify queue was empty and we called gforce_page with
**          an uninitialized value for DM0P_GROUP *g. This resulted in a SEGV
**          in gforce_page(). Note sometimes the session would be hung
**          waiting on LOGIO.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**      01-aug-1996 (nanpr01 for ICL itb)
**	    Added the gather write and async io amendments.
**	01-Oct-1996 (jenjo02)
**	    Mutex granularity alterations. Also changed to make one pass
**	    thru caches for both groups and buffers instead of one pass
**	    for groups and one pass for buffers.
**	12-Feb-1997 (jenjo02)
**	  - Don't force buffers which might induce a log force unless we come
**	    up short during the first complete pass thru the modified queues.
**	  - Don't force groups that might induce a log force if we're able
**	    to force other group(s).
**	  - Use SHARE mutex when scanning group and modified queues.
**	03-Apr-1997 (jenjo02)
**	    Skip over fixed priority buffers during the first pass thru
**	    the modified queues.
**	    Keep rescanning the priority queues as long as there are buffers
**	    to flush, as evidenced by modify/free counts. Previously, we'd
**	    bail out after a single pass thru the priority queues even if the
**	    counts hadn't been satisfied.
**	01-May-1997 (jenjo02)
**	    Let fixed priority buffers be flushed - after all, they'll just
**	    be written, not tossed.
**	21-Aug-1997 (jenjo02)
**	    In dm0p_flush_pages(), lock buffer before locking group to adhere
**	    to mutex protocol and avoid deadlocks.
**	28-Aug-1997 (jenjo02)
**	    Redid group-busy logic, in general, lock group, set busy status, 
**	    unlock group, then lock buffer.
**	10-Dec-1997 (jenjo02)
**	    Keep reusing the TBIO as long as possible instead of unfixing it
**	    after every force.
**	20-May-1998 (jenjo02)
**	    Dynamic, cache-specific WriteBehind thread rewrite.
**	29-Oct-1998 (schte01)
**	    Moved # to column 1 to avoid compiler errors on axp_osf.
**	03-Mar-1999 (jenjo02)
**	    Don't stick around just for CP flush if this cache has already
**	    been flushed for the CP.
**	15-Apr-1999 (jenjo02)
**	    If entire cache has passed and nothing was found to flush,
**	    give up to avoid futile looping while attempting to aid a CP.
**	29-Jun-1999 (jenjo02)
**	    Employ GatherWrite when available.
**	11-Oct-1999 (jenjo02)
**	    If synch writes are occuring, terminate GatherWrite to minimize the
**	    time buffers are in a busy state.
**	10-Jan-2000 (jenjo02)
**	    Implement group priority queues. Call gforce_page with
**	    FP_WRITE to write the group pages but keep the pages in cache
**	    instead of tossing them.
**	    After writing a buffer, instead of always returning to 
**	    the priority zero queue, find the lowest priority queue on
**	    which newly modified pages have appeared, otherwise stay
**	    at the same priority. Before returning to a lower priority
**	    queue, force any GatherWrite buffers which have accumulated.
**	    This induces I/O completion on those buffers and causes them
**	    to be moved from the modified to free queues, reducing the
**	    clutter we have to pass over when rescanning the lower
**	    queues.
**	20-Apr-2000 (jenjo02)
**	    If nothing else to do and a CP flush is in progress in
**	    this cache, participate.
**	21-mar-2001 (devjo01)
**	    When using INTERNAL threads, server could hang if all group
**	    buffers had busy TCB's.  Fixed 'all_group_tcbs_busy' logic
**	    to avoid this (b104299).
**	08-Jan-2002 (jenjo02)
**	    Alternate between flushing single-page buffers and groups
**	    so that the concentration on one does not starve the
**	    server of the other.
**	31-Jan-2003 (jenjo02)
**	    Rewrote algorithms for Cache Flush Agent concepts.
**	12-Mar-2003 (jenjo02)
**	    Release buf_mutex before calling unfix_tableio_ptr(),
**	    retry buffer to avoid rare E_CL2517_CS_SMPR_DEADLOCK
**	    error in fault_page() (B109819).
**      28-Feb-2005 (wanfr01,jenjo02)
**          Bug 113547, INGSRV3062
**          Stop doing dirty reads if we are restarting the scan too
**          many times to avoid cpu thrashing.
*/
DB_STATUS
dm0p_flush_pages(
i4	    lock_list,
i4     log_id,
char 	    *cfap,
i4	    *err_code)
{
    DM0P_CFA        *cfa = (DM0P_CFA*)cfap;
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BSTAT      *lbm_stats = &lbm->lbm_stats[cfa->cfa_cache_ix];
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BMCB       *bm = lbm->lbm_bmcb[cfa->cfa_cache_ix];
    DM0P_BUFFER	    *buf_array = lbm->lbm_buffers[cfa->cfa_cache_ix];
    DM0P_BUFFER	    *b;
    DM0P_GROUP      *group_array = lbm->lbm_groups[cfa->cfa_cache_ix];
    DM0P_GROUP      *g = NULL;
    DMP_TABLE_IO    *tbio = NULL;
    DMPP_PAGE	    *page;
    i4	    	    next_buf;
    DB_STATUS	    status = E_DB_OK;
    DB_STATUS	    sts;
    i4		    i, j;
    i4	    	    ng;
    i4	    	    CheckLSN = TRUE, SkippedLSN;
    i4		    gw_queued = 0;
    i4		    some_tcb_not_found = FALSE;
    i4	    	    no_tcb_dbid = 0;
    DB_TAB_ID	    no_tcb_tabid;
    i4		    mpcount[DB_MAX_TABLEPRI];
    i4		    fcount, mcount, lcount, gmcount;
    i4		    CPflushed, Sflushed, Gflushed;
    i4		    PriFlushed;
    i4		    WBpass;
    i4		    restarts;

    /*
    ** Cache Flush Agents:
    **
    ** The "Primary" WriteBehind "thread" (started when the server
    ** comes up and persisting the life of the server) is now
    ** termed the Primary Agent. It comes to life in a cache when
    ** one of the following events occur:
    **
    ** 		o The single-buffer modified count reaches or exceeds
    **		  bm->bm_wbstart
    **		o The group-buffer modified count reaches or exceeds
    **		  bm->bm_gwbstart
    **		o A Consistency Point has found modified pages in
    **		  the cache that need flushing.
    **
    ** While performing any of these initial "tasks", any of the 
    ** other events may occur and be added to the work to be done
    ** by an Agent. Consistency Point events take precedence over
    ** WriteBehind events.
    **
    ** "Cloned" Agents are spawned as needed to assist with the
    ** tasks by overlapping I/O with other Agents. 
    **
    ** The number of active Agents is maintained algorithmically 
    ** per-cache depending on the task being performed:
    **
    **   o Single-buffer WriteBehind flush
    **	    log2(mcount - wbend - 256)
    **   o Group-buffer WriteBehind flush
    **	    log2(gmcount - gwbend - 32)
    **   o Consistency Point flush
    **	    log2(cpbufs - 256)
    **
    ** In a multi-server configuration, Agents are deployed 
    ** proportionally to each server. There is an upper limit
    ** of 32 Agents per cache per installation.
    **
    ** Each Agent is assigned a cache-unique "id" from 0-31. 
    ** When assisting with a CP flush, this id is used to divide 
    ** the CP array among the Agents.
    **
    ** All Agents persist until all assigned tasks are complete, i.e.,
    ** all CP buffers are flushed and/or the modified counts are
    ** reduced to wbend, at which time the Primary Agent waits for
    ** the next event signal and the Cloned Agents terminate.
    **
    ** A trace point, DM414, is provided to TRdisplay lots of
    ** information about the life and work of the Agents.
    ** The trace point affects all caches in all connected servers.
    */

    /* Initialize Agent */
    WBpass = 0;
    cfa->cfa_CPcount = 0;
    cfa->cfa_CPstart = 0;
    cfa->cfa_Sflushed = 0;
    cfa->cfa_Gflushed = 0;
    cfa->cfa_CPflushed = 0;

    /* Clear tracing flushes-per-priority stats */
    for (i = 0; i < DB_MAX_TABLEPRI; i++)
    {
	cfa->cfa_Spflushed[i] = 0;
	cfa->cfa_Gpflushed[i] = 0;
    }

    /*
    ** Determine dominant task for Primary Agent.
    ** Cloned Agent's tasks are specified by cloner
    */
    if ( cfa->cfa_type == CFA_PRIMARY )
    {
	cfa->cfa_tasks = 0;

	/* Why was this cycle started? */
	if ( bm->bm_status & BM_WB_SINGLE )
	    cfa->cfa_tasks |= CFA_SINGLE;
	if ( bm->bm_status & BM_WB_GROUP )
	    cfa->cfa_tasks |= CFA_GROUP;

	/* CPs take precedence over all other tasks */
	if ( bm_common->bmcb_status & BMCB_FCFLUSH && bm->bm_cpbufs )
	    cfa->cfa_tasks |= CFA_CP;
    }


    /*
    ** If this cache has no TCBs opened by this server, bashing thru the cache will
    ** only eat (lots of) CPU and be of no useful benefit, so do nothing.
    */
    
    while ( dmf_svcb->svcb_hcb_ptr->hcb_cache_tcbcount[cfa->cfa_cache_ix]
	   && cfa->cfa_tasks )
    {
	/* Init "per-pass" variables */
	Sflushed = 0;
	Gflushed = 0;
	CPflushed = 0;
	SkippedLSN = 0;

	/* 
	** If a (new) CP is in progress and pages in this cache
	** need to be flushed, join the CP to assist
	** then continue with other tasks.
	**
	** CP flushes take precedence over WriteBehind tasks
	** because the flushing of the CP buffers is in a
	** race with ongoing log writes; if the log catches
	** up to the log address of the CP (or nearly so),
	** all log writes (hence transactions) are stalled 
	** until the CP flush completes.
	*/
	if ( cfa->cfa_tasks & CFA_CP )
	{
	    /* Assign this CP only */
	    cfa->cfa_CPcount = bm_common->bmcb_cpcount;

	    cp_flush(cfa, cfa->cfa_cache_ix, lock_list, log_id,
				    &CPflushed, err_code);

	    if ( CPflushed )
		TRdisplay("%@ CP Agent %s%d flushed %d %dK pages\n",
		    (cfa->cfa_type == CFA_PRIMARY) ? "P" : "C",
		    cfa->cfa_id,
		    CPflushed, bm->bm_pgsize >> 10);

	    /* CP task is complete */
	    cfa->cfa_tasks &= ~CFA_CP;

	    /* Refresh the counts */
	    DM0P_COUNT_ALL_QUEUES(fcount, mcount, lcount);
	    DM0P_COUNT_QUEUE(bm->bm_gmq, gmcount);

	    /*
	    ** If modified counts are now below wbstart,
	    ** WB task(s) are done.
	    ** If they're at or above wbstart, assign WB task(s).
	    */
	    if ( mcount <= bm->bm_wbstart && fcount >= bm->bm_wb_optim_free )
		cfa->cfa_tasks &= ~CFA_SINGLE;
	    else 
		cfa->cfa_tasks |= CFA_SINGLE;
	    
	    if ( gmcount <= bm->bm_gwbstart )
		cfa->cfa_tasks &= ~CFA_GROUP;
	    else
		cfa->cfa_tasks |= CFA_GROUP;

	    /* Trace the end of the CP */
	    if ( bm_common->bmcb_status & BMCB_CFA_TRACE )
		CfaTrace(cfa, "CPDONE");
	}
	else
	{
	    /* (Re)count the modified/free queues */
	    DM0P_COUNT_ALL_QUEUES(fcount, mcount, lcount);
	    DM0P_COUNT_QUEUE(bm->bm_gmq, gmcount);
	}
	

	/* Trace start/continuation of WriteBehind tasks, if any */
	if ( cfa->cfa_tasks && bm_common->bmcb_status & BMCB_CFA_TRACE )
	    CfaTrace(cfa, (WBpass++) ? "WBCONT" : "WBSTART");


	/*
	** Cycle through the buffer manager writing out dirty pages.
	** Start with the lowest priority pages and continue writing
	** until there are the desired number of free pages.
	** Write pages until the modified page count is below the
	** bm_wbend window and the free page count is above the free
	** page limit.
	**
	** Make up to 2 passes thru each modified priority queue.
	**
	** During the first pass thru the modified queues,
	** do not write pages whose log records (as evidenced by LSN)
	** have not yet been written, in order to:
	**		- reduce number of log forces
	**		- avoid writing pages that were just modified
	**
	** If, at the end of a priority, more buffers need
	** flushing and we've skipped over some of these buffers,
	** restart the scan with the lowest priority queue in which
	** new modified buffers have appeared, if any.
	**
	** After writing a page, find the lowest priority at which
	** new modified buffers have appeared, if any, and restart
	** the scan at that point. If none found, the flush continues
	** with the next modified buffer in the current priority 
	** queue.
	**
	** Periodically check the group modified count and break
	** to process groups if that task is needed.
	*/
	
	for ( i = 0; 
	      cfa->cfa_tasks & CFA_SINGLE &&
		  DB_SUCCESS_MACRO(status) &&
		  i < DB_MAX_TABLEPRI;
	      i++ )
	{
	    /* Count flushes at this priority */
	    PriFlushed = 0;

	    /* Oldest buffers are at the front of the queue */
	    next_buf = bm->bm_mpq[i].sth_stq.stq_next;
	    restarts = MAX_RESTARTS;

	    while (next_buf != BUF_ENDLIST )
	    {
		/*
		** Periodically, check for a rise in the
		** modified count sufficient to clone
		** additional agents.
		**
		** The number of active agents in each 
		** server is maintained at
		**
		**    log2(distance-from-wbend divided by
		**	   number-of-connected-servers)
		**
		** Agents continue flushing until:
		**
		**	o wbend is reached or
		**	o a new CP is signalled or
		**	o the group modified count 
		**	  reaches wbstart
		*/
		while ( PriFlushed && (PriFlushed % 16) == 0
		        && ((mcount - bm->bm_wbend)
				/ bm_common->bmcb_srv_count >>
			     bm->bm_stats.bs_cfa_active) >= 256
		        && !(CloneAgent(cfa, CFA_SINGLE, "SCLONE")) )
		{
		    DM0P_COUNT_ALL_QUEUES(fcount, mcount, lcount);
		}

		/* If a new CP appears, break to join it */
		if ( bm_common->bmcb_status & BMCB_FCFLUSH &&
		     bm->bm_cpbufs &&
		     cfa->cfa_CPcount != bm_common->bmcb_cpcount )
		{
		    cfa->cfa_tasks |= CFA_CP;
		    break;
		}

		/*
		** If buffer is valid and modified, and not busy or fixed then
		** call force_page to write the buffer and put in on the free
		** queue.
		**
		** If force_page returns a warning, then ignore it and go onto
		** the next buffer.
		**
		** After forcing a page, find a lower priority at which new  
		** modified buffers have appeared and restart the scan there.
		** If none found, continue the scan with the next buffer
		** at the current priority.
		*/
		b = &buf_array[next_buf];
		next_buf = b->buf_state.stq_next;

		if (bm_common->bmcb_status & BMCB_IS_MT && restarts > 0)
		{
		    /* 
		    ** If this buffer belongs to a TCB which we previously discovered
		    ** is not open on this server, skip it.
		    */
		    if (some_tcb_not_found &&
			b->buf_dbid == no_tcb_dbid &&
			b->buf_tabid.db_tab_base == no_tcb_tabid.db_tab_base &&
			b->buf_tabid.db_tab_index == no_tcb_tabid.db_tab_index)
		    {
			if ((b->buf_sts & (BUF_MODIFIED | BUF_FIXED)) != BUF_MODIFIED ||
			     b->buf_priority != i)
			{
			    next_buf = bm->bm_mpq[i].sth_stq.stq_next;
			    --restarts;
			}
			continue;
		    }

		    /*
		    ** Take a dirty peek at the next buffer on the modified 
		    ** queue, which may be transitioning to another state while
		    ** we peek.
		    **
		    ** If it doesn't look like a good candidate, skip it, 
		    ** otherwise lock the buffer's mutex and check everything again.
		    */
		    if ( b->buf_mutex.bms_locked ||
		        (b->buf_sts & 
			(BUF_MODIFIED | BUF_VALID | BUF_BUSY | BUF_FIXED | BUF_MUTEX | BUF_WBBUSY)) 
			    != (BUF_MODIFIED | BUF_VALID) ||
			 b->buf_priority != i )
		    {
			if ((b->buf_sts & (BUF_MODIFIED | BUF_FIXED)) != BUF_MODIFIED ||
			     b->buf_priority != i)
			{
			    next_buf = bm->bm_mpq[i].sth_stq.stq_next;
			    --restarts;
			}
			continue;
		    }
		}
		restarts = MAX_RESTARTS;

		/* Lock the buffer, check everything again */
		dm0p_mlock(SEM_EXCL, &b->buf_mutex);

		/* next_buf = next buf on this priority queue */
		next_buf = b->buf_state.stq_next;

		if ( (b->buf_sts & 
		    (BUF_MODIFIED | BUF_VALID | BUF_BUSY | BUF_FIXED | BUF_MUTEX | BUF_WBBUSY)) 
			!= (BUF_MODIFIED | BUF_VALID) ||
		     b->buf_priority != i )
		{
		    if ((b->buf_sts & (BUF_MODIFIED | BUF_FIXED)) != BUF_MODIFIED ||
			 b->buf_priority != i)
		    {
			/* Buffer no longer on (same) modified priority queue */
			next_buf = bm->bm_mpq[i].sth_stq.stq_next;
		    }
		    dm0p_munlock(&b->buf_mutex);
		    continue;
		}

		/*
		** If first pass, skip buffers which might induce a log force.
		*/
		if ( CheckLSN && fcount >= bm->bm_wb_optim_free )
		{
		    if (bm_common->bmcb_last_lsn.lsn_low)
		    {
			page = (DMPP_PAGE *)((char *)lbm->lbm_pages[cfa->cfa_cache_ix] + 
					b->buf_page_offset);
			if (LSN_GT(DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(
					    b->buf_relpgtype, page),
					&bm_common->bmcb_last_lsn))
			{
			    SkippedLSN++;
			    dm0p_munlock(&b->buf_mutex);
			    continue;
			}
		    }
		}
		
		/*
		** Check if we have a table descriptor for this buffer.   
		** If not, then skip it - we can't flush it.
		*/
		if (tbio &&
		    (b->buf_dbid != tbio->tbio_dcbid ||
		     b->buf_tabid.db_tab_base != tbio->tbio_reltid.db_tab_base ||
		     b->buf_tabid.db_tab_index != tbio->tbio_reltid.db_tab_index))
		{
		    /* Wrong table, release buffer, TBIO, retry */
		    next_buf = b->buf_id.bid_index;
		    dm0p_munlock(&b->buf_mutex);
		    unfix_tableio_ptr(&tbio, lock_list, log_id, (i4)__LINE__);
		    continue;
		}

		/* Mark the buffer busy so that other WB agents will skip it */
		b->buf_sts |= BUF_WBBUSY;

		if (tbio == NULL)
		{
		    sts = fix_tableio_ptr(b, (i4)0,
			    lock_list, log_id, &tbio, err_code);

		    if (DB_FAILURE_MACRO(sts))
		    {
			/*
			** Record any errors in the error log, but continue
			** processing as though we found no table descriptor.
			*/
			ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL, 
			    (char *)0, (i4)0, (i4 *)0, err_code, 0);
		    }
		    else if (sts == E_DB_INFO)
		    {
			/* E_DB_INFO: Table doesn't exist on this server, tbio == NULL */
			/* Remember that we didn't find this table */
			some_tcb_not_found = TRUE;
			no_tcb_dbid = b->buf_dbid;
			no_tcb_tabid.db_tab_base = b->buf_tabid.db_tab_base;
			no_tcb_tabid.db_tab_index = b->buf_tabid.db_tab_index;
		    }
		}

		if (tbio)
		{
		    status = force_page(b, tbio, FP_WRITE,
			lock_list, log_id, BMCL_FLUSH_PAGES, &gw_queued, err_code);

		    /*
		    ** Check the result of the force page call.
		    */
		    if ( status )
		    {
			dm0p_munlock(&b->buf_mutex);
			if (DB_FAILURE_MACRO(status))
			    break;
		    }

		    b = NULL;

		    /* Count another buffer flushed during this cache WB cycle */
		    bm->bm_stats.bs_wb_flushed++;
		    lbm_stats->bs_wb_flushed++;
		    
		    /* Note that we've done some useful work this pass */
		    /* Count another flush at this priority */
		    PriFlushed++;
		    cfa->cfa_Spflushed[i]++;
		    /* ...and add to rolling total */
		    cfa->cfa_Sflushed++;

		    /* Adjust the free/modified buffer counts */
		    fcount++;
		    mcount--;

		    /* If we appear to be done, recheck the counts */
		    if ( mcount <= bm->bm_wbend && fcount >= bm->bm_wb_optim_free )
		    {
			DM0P_COUNT_ALL_QUEUES(fcount, mcount, lcount);
			/* Adjust for buffers still queued for GW */
			fcount += gw_queued;
			mcount -= gw_queued;

			if ( mcount <= bm->bm_wbend && fcount >= bm->bm_wb_optim_free )
			{
			    /* Then our single-buffer work is done */
			    cfa->cfa_tasks &= ~CFA_SINGLE;
			    break;
			}
		    }


		    /*
		    ** Find the lowest priority modified queue which has
		    ** new buffers. If none, continue with
		    ** next buffer (next_buf) at this priority (i).
		    */
		    for ( j = 0; j < i; j++ )
		    {
			if ( bm->bm_mpq[j].sth_count != mpcount[j] )
			{
			    /*
			    ** Before revisiting a lower-priority queue,
			    ** force any GatherWrite buffers waiting to
			    ** be written.
			    ** This completes the I/O on the buffers,
			    ** wakes up any waiters, and
			    ** shortens the modified queues.
			    */
			    if ( gw_queued )
				status = dm2f_force_list();

			    next_buf = bm->bm_mpq[j].sth_stq.stq_next;
			    i = j;
			}
		    }
		}

		if (b)
		{
		    b->buf_sts &= ~BUF_WBBUSY;
		    dm0p_munlock(&b->buf_mutex);
		}

	    } /* while not at end of modified queue [i] ... */

	    /* Add #flushed this pri to #flushed this pass */
	    Sflushed += PriFlushed;

	    /* If a new CP has appeared, break to join it */
	    if ( cfa->cfa_tasks & CFA_CP )
		break;

	    /* If single buffers are complete, break to groups */
	    if ( !(cfa->cfa_tasks & CFA_SINGLE) )
		break;

	    /*
	    ** At the end of each priority queue, check for
	    ** a spurt in group activity. If we're actively
	    ** flushing single-page buffers and the modified
	    ** groups need flushing, break out of single-page
	    ** buffers and go to work on the groups.
	    **
	    ** Similar checks in the group code periodically
	    ** checks the single-page buffer thresholds.
	    **
	    ** The net effect is to switch between single
	    ** and group buffers to keep one activity from
	    ** starving the installation of free groups or
	    ** buffers.
	    */
	    if ( PriFlushed )
	    {
		DM0P_COUNT_QUEUE(bm->bm_gmq, gmcount);
		if ( gmcount >= bm->bm_gwbstart )
		{
		    cfa->cfa_tasks |= CFA_GROUP;
		    break;
		}
	    }
		
	    /*
	    ** Before advancing to next priority, remember the
	    ** current modified count at the priority we're
	    ** about to leave.
	    */
	    if ( (mpcount[i] = bm->bm_mpq[i].sth_count) )
	    {
		/* Refresh queue counts if priority not empty */
		DM0P_COUNT_ALL_QUEUES(fcount, mcount, lcount);
		fcount += gw_queued;
		mcount -= gw_queued;
	    }

	} /* for each priority ... */

	/* If first complete pass, skip LSN checks next time */
	if ( i == DB_MAX_TABLEPRI )
	    CheckLSN = FALSE;

	/*
	** Force any remaining queued GatherWrite buffers 
	** before moving on to groups.
	*/
	if ( gw_queued && (status = dm2f_force_list()) )
	    break;

	/* If we've a TBIO still hanging around, release it */
	if (tbio)
	    unfix_tableio_ptr(&tbio, lock_list, log_id, (i4)__LINE__);

	/* If a new CP has appeared, join it */
	if ( cfa->cfa_tasks & CFA_CP )
	    continue;

	/* 
	** The non-group buffers seem under control for the moment.
	**
	** Cycle thru the modified group queues, gforcing lowest to
	** highest priority until the modified count is under control.
	**
	** Ignore groups that are busy or that don't have a TCB
	** in this server.
	*/

	/* Recount modified groups after doing single buffers */
	DM0P_COUNT_QUEUE(bm->bm_gmq, gmcount);

	/*
	** If tasks include groups and the modified count
	** has fallen below wbend, cancel groups.
	**
	** If tasks exclude groups and the modified count
	** has reached wbstart, assign groups.
	*/
	if ( cfa->cfa_tasks & CFA_GROUP )
	{
	    if ( gmcount <= bm->bm_gwbend )
		cfa->cfa_tasks &= ~CFA_GROUP;
	}
	else if ( gmcount >= bm->bm_gwbstart )
	    cfa->cfa_tasks |= CFA_GROUP;


	/* Trace start of group flush */
	if ( bm_common->bmcb_status & BMCB_CFA_TRACE && cfa->cfa_tasks & CFA_GROUP )
	    CfaTrace(cfa, "GROUPS");

	for ( i = 0; 
	      cfa->cfa_tasks & CFA_GROUP &&
		  DB_SUCCESS_MACRO(status) && i < DB_MAX_TABLEPRI;
	      i++ )
	{
	    /* Count flushes at this priority */
	    PriFlushed = 0;

	    ng = bm->bm_gmq[i].sth_stq.stq_next;

	    while (ng != BUF_ENDLIST )
	    {
		/*
		** Periodically, check for a rise in the
		** modified count sufficient to clone
		** additional agents.
		**
		** The number of active agents in each 
		** server is maintained at
		**
		**    log2(distance-from-wbend divided by
		**	   number-of-connected-servers)
		**
		** Agents continue flushing until:
		**
		**	o wbend is reached or
		**	o a new CP is signalled or
		**	o the single buffer modified count 
		**	  reaches wbstart
		*/
		while ( PriFlushed && (PriFlushed % 4) == 0
		       && ((gmcount - bm->bm_gwbend)
			       / bm_common->bmcb_srv_count >>
			    bm->bm_stats.bs_cfa_active) >= 32
		       && !(CloneAgent(cfa, CFA_GROUP, "GCLONE")) )
		{
		    DM0P_COUNT_QUEUE(bm->bm_gmq, gmcount);
		}

		/* If a new CP appears, break to join it */
		if ( bm_common->bmcb_status & BMCB_FCFLUSH &&
		     bm->bm_cpbufs &&
		     cfa->cfa_CPcount != bm_common->bmcb_cpcount )
		{
		    cfa->cfa_tasks |= CFA_CP;
		    break;
		}

		g = &group_array[ng];
		ng = g->g_state.stq_next;
		restarts = MAX_RESTARTS;

		if (bm_common->bmcb_status & BMCB_IS_MT && restarts > 0)
		{
		    /* Skip group if not modified or busy */
		    if ( g->g_mutex.bms_locked ||
			(g->g_sts & (G_MODIFIED | G_FIXED | G_BUSY))
			    != G_MODIFIED ||
			g->g_priority != i )
		    {
			if ( (g->g_sts & (G_MODIFIED | G_FIXED))
				!= G_MODIFIED ||
			    g->g_priority != i )
			{
			    ng = bm->bm_gmq[i].sth_stq.stq_next;
			    restarts--;
			}
			continue;
		    }
		}
		restarts = MAX_RESTARTS;

		/* Lock the group, check it again */
		dm0p_mlock(SEM_EXCL, &g->g_mutex);
		ng = g->g_state.stq_next;

		if ( (g->g_sts & (G_MODIFIED | G_FIXED | G_BUSY))
		    != G_MODIFIED ||
		    g->g_priority != i )
		{
		    if ( (g->g_sts & (G_MODIFIED | G_FIXED))
			    != G_MODIFIED ||
			g->g_priority != i )
		    {
			ng = bm->bm_gmq[i].sth_stq.stq_next;
		    }
		    dm0p_munlock(&g->g_mutex);
		    continue;
		}
		/*
		** Force the group of pages, moving the group from
		** the MODIFIED priority queue to the FREE priority
		** queue at the same priority. Note that the modified
		** pages are written, but not tossed, and remain valid.
		**
		*/
		status = gforce_page(bm, g,
				    lock_list, log_id, 
				    FP_WRITE,
				    BMCL_FLUSH_PAGES,
				    &gw_queued,
				    err_code);

		dm0p_munlock(&g->g_mutex);

		/*
		** Check success of group force operation.  
		** If warning, just move on to next group.
		*/
		if (DB_FAILURE_MACRO(status))
		{
		    if (*err_code > E_DM_INTERNAL)
		    {
			ule_format(*err_code, (CL_SYS_ERR *)0,ULE_LOG,NULL, 
			 (char *)0, (i4)0, (i4 *)0, err_code, 0);
			*err_code = E_DM920D_BM_BAD_GROUP_FAULTPAGE;
		    }
		    break;
		}

		if ( status == E_DB_OK )
		{
		    /* Count another group flush in this cycle */
		    bm->bm_stats.bs_wb_gflushed++;
		    lbm_stats->bs_wb_gflushed++;
		    
		    /* Count another flush at this priority */
		    PriFlushed++;
		    cfa->cfa_Gpflushed[i]++;
		    /* ...and add to rolling total */
		    cfa->cfa_Gflushed++;

		    /* Decrement modified group count by one */
		    gmcount--;

		    /* If we appear to be done, recheck the counts */
		    if ( gmcount <= bm->bm_gwbend )
		    {
			DM0P_COUNT_QUEUE(bm->bm_gmq, gmcount);
			if ( gmcount <= bm->bm_gwbend )
			{
			    cfa->cfa_tasks &= ~CFA_GROUP;
			    break;
			}
		    }
		}

		/*
		** Find the lowest priority modified queue which has
		** new groups. If none, continue with
		** next group (ng) at this priority (i).
		*/
		for ( j = 0; j < i; j++ )
		{
		    if ( bm->bm_gmq[j].sth_count != mpcount[j] )
		    {
			ng = bm->bm_gmq[j].sth_stq.stq_next;
			i = j;
		    }
		}
	    } /* while (ng != BUF_ENDLIST) */

	    /* Add to flushes this pass */
	    Gflushed += PriFlushed;

	    /* If a new CP has appeared, join it */
	    if ( cfa->cfa_tasks & CFA_CP )
		break;

	    /* If group task is complete, break */
	    if ( !(cfa->cfa_tasks & CFA_GROUP) )
		break;

	    /*
	    ** At the end of each priority queue, check for
	    ** a spurt in single-page activity. If we're actively
	    ** flushing groups and the modified single-page
	    ** buffers need flushing, break out of groups
	    ** and go to work on the buffers.
	    **
	    ** Similar checks in the single-page code periodically
	    ** checks the group threshold.
	    **
	    ** The net effect is to switch between single
	    ** and group buffers to prevent one procedure from
	    ** starving the installation of free groups or
	    ** buffers.
	    */
	    if ( PriFlushed )
	    {
		DM0P_COUNT_ALL_QUEUES(fcount, mcount, lcount);
		if ( mcount >= bm->bm_wbstart || fcount < bm->bm_wb_optim_free )
		{
		    cfa->cfa_tasks |= CFA_SINGLE;
		    break;
		}
	    }
	    
	    /*
	    ** Before leaving this priority, save its
	    ** current modified count.
	    */
	    if ( (mpcount[i] = bm->bm_gmq[i].sth_count) )
		/* Refresh modified group count */
		DM0P_COUNT_QUEUE(bm->bm_gmq, gmcount);

	} /* for each group priority... */

	/* If a new CP has appeared, join it */
	if ( cfa->cfa_tasks & CFA_CP )
	    continue;

	/* Recount single buffers after doing groups */
	DM0P_COUNT_ALL_QUEUES(fcount, mcount, lcount);

	/*
	** If tasks include single buffers and the modified count
	** has fallen below wbend, cancel single buffers.
	**
	** If tasks exclude single buffers and the modified count
	** has reached wbstart, assign single buffers.
	*/
	if ( cfa->cfa_tasks & CFA_SINGLE )
	{
	    if ( mcount <= bm->bm_wbend && fcount >= bm->bm_wb_optim_free )
		cfa->cfa_tasks &= ~CFA_SINGLE;
	}
	else if ( mcount >= bm->bm_wbstart )
	    cfa->cfa_tasks |= CFA_SINGLE;

	/*
	** For whatever reason (no TCBs, all buffers/groups busy, etc),
	** if nothing flushed this pass, quit.
	*/
	if ( Sflushed + Gflushed + CPflushed + SkippedLSN == 0 )
	    break;

    } /* while (dmf_svcb->svcb_hcb_ptr->hcb_cache_tcbcount[cfa->cfa_cache_ix] > 0) */
    
    /* Force any remaining queued GatherWrite buffers */
    if ( gw_queued )
	status = dm2f_force_list();

    /* If we've a TBIO still hanging around, release it */
    if (tbio)
	unfix_tableio_ptr(&tbio, lock_list, log_id, (i4)__LINE__);

    if (DB_FAILURE_MACRO(status) && (*err_code > E_DM_INTERNAL))
    {
	ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL, (char *)0,
	    (i4)0, (i4 *)0, err_code, 0);
	*err_code = E_DM9348_BM_FLUSH_PAGES_ERROR;
    }
    else
	status = E_DB_OK;
    
    return (status);
}
/*{
** Name: CfaTrace - Trace CacheFlushAgent activity.
**
** Description:
**			Traces CacheFlushAgent activity
**			when DMZ_BUF_MACRO(14) (DM414)
**			is set. This sets BMCB_CFA_TRACE
**			flag in bmcb_status for all
**			servers to see.
**
** Inputs:
**	cfa		Agent's task block
**	what		String denoting some activity.
**
** Outputs:
**	none
**
** Side Effects:
**	    none
**
** History:
**	31-Jan-2003 (jenjo02)
**	    Created.
*/
static VOID
CfaTrace(
DM0P_CFA	    *cfa,
char		    *what)
{
    DMP_LBMCB	*lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BSTAT  *lbm_stats = &lbm->lbm_stats[cfa->cfa_cache_ix];
    DM0P_BMCB   *bm = lbm->lbm_bmcb[cfa->cfa_cache_ix];
    PID		pid;
    CS_SID	sid;
    i4		fcount, mcount, lcount, gmcount;

    PCpid(&pid);
    CSget_sid(&sid);

    DM0P_COUNT_ALL_QUEUES(fcount, mcount, lcount);
    DM0P_COUNT_QUEUE(bm->bm_gmq, gmcount);

    TRdisplay("%@ %dK WB cycle %d Agent %s%d(%d,%x): %8s Tasks: %v\n\
\t Free: %8d Flimit:   %8d  Optim: %8d Mlimit:  %8d CPflushed: %8d\n\
\t Mod:  %8d Wbstart:  %8d  Wbend: %8d CPstart: %8d Sflushed:  %8d\n\
\t GMod: %8d GWbstart: %8d GWbend: %8d CFmask:  %8x Gflushed:  %8d\n\
\t Sact: %8d Cact:     %8d\n\
\t Spri: (%d,%d,%d,%d,%d,%d,%d,%d) Gpri: (%d,%d,%d,%d,%d,%d,%d,%d)\n\
\t Spfl: (%d,%d,%d,%d,%d,%d,%d,%d) Gpfl: (%d,%d,%d,%d,%d,%d,%d,%d)\n",
	bm->bm_pgsize >> 10,
	bm->bm_stats.bs_cache_flushes,
	(cfa->cfa_type == CFA_PRIMARY) ? "P" : "C",
	cfa->cfa_id,
	pid,
	sid,
	what,
	"S,G,CP", cfa->cfa_tasks,
	fcount,
	bm->bm_flimit,
	bm->bm_wb_optim_free,
	bm->bm_mlimit,
	cfa->cfa_CPflushed,
	mcount,
	bm->bm_wbstart,
	bm->bm_wbend,
	cfa->cfa_CPstart,
	cfa->cfa_Sflushed,
	gmcount,
	bm->bm_gwbstart,
	bm->bm_gwbend,
	bm->bm_cfa_mask,
	cfa->cfa_Gflushed,
	lbm_stats->bs_cfa_active,
	bm->bm_stats.bs_cfa_active,
	bm->bm_mpq[0].sth_count, bm->bm_mpq[1].sth_count,
	bm->bm_mpq[2].sth_count, bm->bm_mpq[3].sth_count,
	bm->bm_mpq[4].sth_count, bm->bm_mpq[5].sth_count,
	bm->bm_mpq[6].sth_count, bm->bm_mpq[7].sth_count,
	bm->bm_gmq[0].sth_count, bm->bm_gmq[1].sth_count,
	bm->bm_gmq[2].sth_count, bm->bm_gmq[3].sth_count,
	bm->bm_gmq[4].sth_count, bm->bm_gmq[5].sth_count,
	bm->bm_gmq[6].sth_count, bm->bm_gmq[7].sth_count,
	cfa->cfa_Spflushed[0], cfa->cfa_Spflushed[1],
	cfa->cfa_Spflushed[2], cfa->cfa_Spflushed[3],
	cfa->cfa_Spflushed[4], cfa->cfa_Spflushed[5],
	cfa->cfa_Spflushed[6], cfa->cfa_Spflushed[7],
	cfa->cfa_Gpflushed[0], cfa->cfa_Gpflushed[1],
	cfa->cfa_Gpflushed[2], cfa->cfa_Gpflushed[3],
	cfa->cfa_Gpflushed[4], cfa->cfa_Gpflushed[5],
	cfa->cfa_Gpflushed[6], cfa->cfa_Gpflushed[7]);

    return;
}
/*{
** Name: dm0p_write_along - unobtrusively flush modified pages from  cache.
**
** Description:
**	This routine is used to flush modified pages from the buffer cache
**	so that new pages can be read in. Unlike the dm0p_flush_pages()
**	function, it is called from the write-along thread of a dedicated
**	IOMASTER server type, and is intended to be less obtrusive than
** 	dm0p_flush_pages.
**
**	See the block comment on dmcwrite.c - dmc_write_along() for
**	more information.
**
**
**	Each time a page is written, it returns back to the lowest priority
**	queue in case any new pages have been put there that can be written.
**	Hopefully this should not cause too much extra overhead scanning
**	the priority queues - if the lower priority queues are still empty
**	then it should not cost too much to verify that (one check per 
**	priority).
**
**	Although the sessions that caused the pages to be dirty are not part
**	of this server, If the database is in the database list given    
**	to the iomaster server at startup, a TCB/TBIO will be opened
**	in this server for use in writting out the pages.
**
** Inputs:
**	lock_list	- transaction lock list.
**	log_id		- transaction log id.
**	duties 		- bitmap of duties this thread must do:
**				DM0P_WA_SINGLE - write single bufs
**				DM0P_WA_SINGLE - write group bufs
**
** Outputs:
**	num_flushes	- Number of flushes done, used by writealong thread
**			  as indication of how busy we were.  
**      err_code                        Reason for error return status.
**
**	Returns:
**	    E_DB_OK
**	    E_DB_FATAL
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	22-April-1995 (cohmi01)
**	    Adapted from dm0p_flush_pages for use in write_along thread.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	01-aug-1996 (nanpr01 for itb ICL)
**	    Converted to use gather-write.  After a force_page the table is
**	    unfixed in a completion routine which is executed when the
**	    write actually completes.
**	01-Oct-1996 (jenjo02)
**	    Mutex granularity alterations. Also changed to make one pass
**	    thru caches for both groups and buffers instead of one pass
**	    for groups and one pass for buffers.
**	25-Feb-1997 (jenjo02)
**	    Use SHARE mutex when scanning group and modified queues.
**	28-Aug-1997 (jenjo02)
**	    Redid group-busy logic, in general, lock group, set busy status, 
**	    unlock group, then lock buffer.
**	10-Dec-1997 (jenjo02)
**	    Keep reusing the TBIO as long as possible instead of unfixing it
**	    after every force.
**	10-Jan-2000 (jenjo02)
**	    Implement group priority queues.
**	12-Mar-2003 (jenjo02)
**	    Release buf_mutex before calling unfix_tableio_ptr(),
**	    retry buffer to avoid rare E_CL2517_CS_SMPR_DEADLOCK
**	    error in fault_page() (B109819).
**    28-Feb-2005 (wanfr01,jenjo02)
**          Bug 113547, INGSRV3062
**          Stop doing dirty reads if we are restarting the scan too
**          many times to avoid cpu thrashing.
*/
DB_STATUS
dm0p_write_along(
i4	    lock_list,
i4     log_id,
i4     *num_flushes,
i4	    duties,
i4	    *err_code)
{
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    i4              cache_ix;
    DM0P_BMCB       *bm;
    DM0P_BUFFER	    *buf_array;
    DMPP_PAGE       *page;
    DM0P_BUFFER	    *buffer;
    DMP_TABLE_IO    *tbio = NULL;
    i4	    next_buf;
    DB_STATUS	    status = E_DB_OK;
    i4		    i;
    i4              gw_queued = 0;
    i4		    restarts;

    /*
    ** If there's an IO_MASTER server, assume we're it and build TCBs
    ** if they don't exist on this server
    */
    i4	    fix_tableio_flags = (bm_common->bmcb_status & BMCB_IOMASTER)
				      ? DM0P_TBIO_BUILD | DM0P_TBIO_NOPARTIAL |
					DM0P_TBIO_MUTEX_HELD
				      : 0;

    DM0P_GROUP              *g;
    DM0P_GROUP              *group_array;
    i4			ng;
    i4                     desire_dirty;
    i4                     desire_free;
    i4			   fcount,mcount,gfcount,gmcount;

    *num_flushes = 0;

    for (cache_ix = 0;
	 cache_ix < DM_MAX_CACHE && DB_SUCCESS_MACRO(status);
	 cache_ix++)
    {
	bm = lbm->lbm_bmcb[cache_ix];

	/* 
	** If this cache isn't allocated or there are no modified
	** groups or single buffers, skip this cache.
	*/
	if ( bm && bm->bm_status & BM_ALLOCATED )
	{
	    DM0P_COUNT_QUEUE(bm->bm_mpq, mcount);
	    DM0P_COUNT_QUEUE(bm->bm_gmq, gmcount);

	    if ( mcount == 0 && gmcount == 0 )
		continue;
	}
	else
	    continue;

	buf_array = lbm->lbm_buffers[bm->bm_cache_ix];

	/* If duties include groups, force any groups thats look
	** like good candidates, eg ones with mostly dirty pages, whose
	** LSN indicates that we wont cause a log force. Regardless of log forces,
	** it is important to ensure that at least a few groups get freed
	** up, even at some cost to the IOMASTER server, to prevent
	** regular servers from having to write synchronously. 
	*/
	if (duties & DM0P_WA_GROUPS && gmcount)
	{
	    i4  max_dirty = 0;  /* # of dirty pages in the dirtiest group */

	    group_array = lbm->lbm_groups[bm->bm_cache_ix];

/**         desire_free = gmcount / 8;  /* try to keep 1/8th free */
	    desire_free = 1;
	    desire_dirty = bm->bm_gpages / 2;/* dirty enough to force  */
	    DM0P_COUNT_QUEUE(bm->bm_gfq, gfcount);
	    if (gfcount >= desire_free)
		continue;

	    /*
	    ** Get a picture of how dirty the dirtiest group is so that if there
	    ** are no free groups we will know which group(s) to force even if 
	    ** there are none that fit the more stringent 'very dirty' criteria/
	    */
	    for ( i = 0; i < DB_MAX_TABLEPRI; i++ )
	    {
		ng = bm->bm_gmq[i].sth_stq.stq_next;
		while (ng != BUF_ENDLIST)
		{
		    g = &group_array[ng];
		    ng = g->g_state.stq_next;

		    if (g->g_modcount > max_dirty)
			max_dirty = g->g_modcount; /* record new dirtiest group */
		    if ((g->g_sts & (G_MODIFIED | G_FIXED)) != G_MODIFIED)
			ng = bm->bm_gmq[i].sth_stq.stq_next;
		}
	    }
    /**	max_dirty  -= (max_dirty/8);  /* loosen up to increase # of forces */

	    for ( i = 0; i < DB_MAX_TABLEPRI; i++ )
	    {
		ng = bm->bm_gmq[i].sth_stq.stq_next;
		restarts = MAX_RESTARTS;

		while (DB_SUCCESS_MACRO(status) && ng != BUF_ENDLIST && 
		       gfcount < desire_free)
		{
		    g = &group_array[ng];
		    ng = g->g_state.stq_next;

		    if (bm_common->bmcb_status & BMCB_IS_MT && restarts > 0)
		    {
			/*
			** Skip groups that are busy, have their mutex locked,
			** or aren't dirty enough.
			*/
			if (g->g_mutex.bms_locked ||
			    g->g_sts & G_BUSY ||
			    g->g_modcount < max_dirty || g->g_modcount < desire_dirty)
			{
			    if ((g->g_sts & (G_MODIFIED | G_FIXED)) != G_MODIFIED)
			    {
				ng = bm->bm_gmq[i].sth_stq.stq_next;
				--restarts;
			    }
			    continue;
			}
		    }
		    restarts = MAX_RESTARTS;
		    
		    /* Lock the group, check again */
		    dm0p_mlock(SEM_EXCL, &g->g_mutex);
		    ng = g->g_state.stq_next;
		    
		    /*
		    ** By the time we get the group mutex, the group may no
		    ** longer be on the modified queue.
		    */
		    if (g->g_sts & G_BUSY ||
			g->g_modcount < max_dirty || g->g_modcount < desire_dirty ||
			!(dm0p_no_lgforce(bm, g)))
		    {
			if ((g->g_sts & (G_MODIFIED | G_FIXED)) != G_MODIFIED)
			    ng = bm->bm_gmq[i].sth_stq.stq_next;
			dm0p_munlock(&g->g_mutex);
			continue;
		    }

		    /* Force the group of pages so that we can reuse the group. */
		    *num_flushes = *num_flushes + g->g_modcount;   
		    status = gforce_page(bm, g,
					lock_list, log_id, 
					FP_WRITE, 
					BMCL_NO_GATHER,
					&gw_queued,
					err_code);

		    if (DB_FAILURE_MACRO(status))
		    {
			if (*err_code > E_DM_INTERNAL)
			{
			    ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL, 
				(char *)0, (i4)0, (i4 *)0, err_code, 0);
			    *err_code = E_DM920D_BM_BAD_GROUP_FAULTPAGE;
			}
		    }

		    dm0p_munlock(&g->g_mutex);

		    /* Pick the new oldest group off of the modified queue */
		    ng = bm->bm_gmq[i].sth_stq.stq_next;
		    /* Refresh the free group count */
		    DM0P_COUNT_QUEUE(bm->bm_gfq, gfcount);
		}
	    }
	}

	if ((duties & DM0P_WA_SINGLE) == 0)
	    break;

	/*
	** Cycle through the buffer manager writing out dirty pages.
	** Start with the lowest priority pages and continue writing
	** until there are the desired number of free pages.
	**
	** After writing a page, go back to the lowest priority to see
	** if there are any new low priority pages to flush.
	**
	** Do not write pages whose log records (as evidenced by LSN)
	** have not yet been written, in order to:
	**		- reduce number of log forces
	**		- avoid writing pages that were just modified
	*/
	DM0P_COUNT_QUEUE(bm->bm_mpq, mcount);

	for (i = 0; 
	      mcount && DB_SUCCESS_MACRO(status) && i < DB_MAX_TABLEPRI;
	     i++)
	{
	    /* Oldest buffers are at the front of the queue */
	    next_buf = bm->bm_mpq[i].sth_stq.stq_next;
	    while (next_buf != BUF_ENDLIST && DB_SUCCESS_MACRO(status))
	    {
		/*
		** If buffer is valid and modified, and not busy or fixed then
		** call force_page to write the buffer and put in on the free
		** queue.
		**
		** If force_page returns a warning, then ignore it and go onto
		** the next buffer.
		**
		** After forcing a page, go back to the start of the priority
		** queue since we have released the buffer manager mutex and
		** any pointers we might have remembered may no longer be valid.
		*/
		buffer = &buf_array[next_buf];
		next_buf = buffer->buf_state.stq_next;

		if (bm_common->bmcb_status & BMCB_IS_MT)
		{
		    if ( buffer->buf_mutex.bms_locked ||
			(buffer->buf_sts & (BUF_MODIFIED | BUF_VALID | BUF_BUSY | 
					    BUF_FIXED | BUF_MUTEX))
				!= (BUF_MODIFIED | BUF_VALID) || buffer->buf_priority != i )
		    {
			if ((buffer->buf_sts & (BUF_MODIFIED | BUF_FIXED)) != BUF_MODIFIED ||
			     buffer->buf_priority != i)
			{
			    next_buf = bm->bm_mpq[i].sth_stq.stq_next;
			}
			continue;
		    }
		}

		/*      
		** Lock the buffer's mutex.
		** After waiting for the buf_mutex, the buffer may
		** no longer be on the modified queue.
		*/
		dm0p_mlock(SEM_EXCL, &buffer->buf_mutex);

		page = (DMPP_PAGE *)((char *)lbm->lbm_pages[bm->bm_cache_ix] + 
				buffer->buf_page_offset);

		if ( (buffer->buf_sts & (BUF_MODIFIED | BUF_VALID | BUF_BUSY | 
				       BUF_FIXED | BUF_MUTEX))
			    == (BUF_MODIFIED | BUF_VALID) && 
		      buffer->buf_priority == i &&
		      bm_common->bmcb_last_lsn.lsn_low &&
		      LSN_GT(&bm_common->bmcb_last_lsn, 
			   DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(buffer->buf_relpgtype, 
							page)) )
		{
		    /* If we already have a usable TBIO, use it. */
		    if (tbio &&
			(buffer->buf_dbid != tbio->tbio_dcbid ||
			 buffer->buf_tabid.db_tab_base != tbio->tbio_reltid.db_tab_base ||
			 buffer->buf_tabid.db_tab_index != tbio->tbio_reltid.db_tab_index))
		    {
			/* Wrong table, release buffer, TBIO, retry */
			next_buf = buffer->buf_id.bid_index;
			dm0p_munlock(&buffer->buf_mutex);
			unfix_tableio_ptr(&tbio, lock_list, log_id, (i4)__LINE__);
			continue;
		    }

		    if (tbio == NULL)
		    {
			/*
			** Obtain table descriptor for this buf.
			*/
			status = fix_tableio_ptr(buffer, fix_tableio_flags,
				    lock_list, log_id, &tbio, err_code);
		    }

		    /* if we got a tbio, then force the page, else look for next */
		    if (tbio)
		    {
			++*num_flushes;   
			status = force_page(buffer, tbio, FP_WRITEALONG,
				    lock_list, log_id, BMCL_WRITE_ALONG, 
				    &gw_queued, err_code);

			if ( status )
			    dm0p_munlock(&buffer->buf_mutex);
			/*
			** Forced a page. Return to priority 0 to
			** continue flushing lowest priority pages.
			*/
			i = 0;
		    }
		}

		if ((buffer->buf_sts & (BUF_MODIFIED | BUF_FIXED)) == BUF_MODIFIED &&
		     buffer->buf_priority == i)
		{
		    next_buf = buffer->buf_state.stq_next;
		}
		else
		    next_buf = bm->bm_mpq[i].sth_stq.stq_next;

		dm0p_munlock(&buffer->buf_mutex);

		/* Refresh the free and modified queue counts */
		DM0P_COUNT_QUEUE(bm->bm_fpq, fcount);
		DM0P_COUNT_QUEUE(bm->bm_mpq, mcount);

		/* 
		** Stop if we've no need to flush more pages.
		*/
		if (mcount - gw_queued <= bm->bm_wbend && 
		    fcount + gw_queued >= bm->bm_flimit)
		{
		    next_buf = BUF_ENDLIST;
		    i = DB_MAX_TABLEPRI;
		}

	    } /* while (next_buf != BUF_ENDLIST) */
	} /* for (i = 0; ... */

	/* End of priority queues in this cache */

	/* Force any remaining queued GatherWrite buffers */
	if (gw_queued)
	    status = dm2f_force_list();
    } /* for (cache_ix = 0; ... */


    /* If we still have a tbio laying around, release id */
    if (tbio)
	unfix_tableio_ptr(&tbio, lock_list, log_id, (i4)__LINE__);

    if (DB_FAILURE_MACRO(status) && (*err_code > E_DM_INTERNAL))
    {
	ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL, (char *)0,
		(i4)0, (i4 *)0, err_code, 0);
	*err_code = E_DM9348_BM_FLUSH_PAGES_ERROR;
	return (status);
    }

    return (E_DB_OK);
}

/*
** Name:  dm0p_no_lgforce  - test if a log force on a group is likely.         
**
** Description:
**
** Inputs:
**      page_size       page size
**      group 		Group of buffers to check
**
** Outputs:
**      none
**	Returns:
**	    TRUE  - if no log force would be caused by writing this group
**	    FALSE - if writing this group might cause a log force
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	24-April-1995 (cohmi01)
**	    Added for use by the write_along thread.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**      06-may-1996 (nanpr01)
**          Changed all page header access as macro for New Page Format.
**	24-Sep-1996 (jenjo02)
**	    Caller must hold group's g_mutex.
**	24-Feb-1997 (jenjo02)
**	    Return FALSE as soon as its detected instead of waiting til
**	    all of the group buffers have been checked.
**	    Removed page_size parm from prototype. Page size can be
**	    extracted from the group structure.
**	10-Jan-2000 (jenjo02)
**	    Limit scan to g_pages (those pages actually in use by
**	    group) instead of bm_gpages.
*/
static bool       
dm0p_no_lgforce(
DM0P_BMCB	    *bm,
DM0P_GROUP	    *g)
{
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BUFFER	    *buf_array = lbm->lbm_buffers[bm->bm_cache_ix];
    DM0P_BUFFER	    *b;
    DMPP_PAGE	    *page;
    i4	    i;


    /*	Compute the actions needed for the group of buffers.           */
    /*								       */
    /* return TRUE if we will NOT have to force log before writing the */ 
    /* group, return FALSE if we will have to force the log, as at     */
    /* least one buffer had an lsn greater than the last one written   */
    /* If the log's never been forced, return FALSE immediately.       */

    if (bm_common->bmcb_last_lsn.lsn_high == 0 &&
	bm_common->bmcb_last_lsn.lsn_low  == 0)
    {
	return(FALSE);
    }

    for (i = 0; i < g->g_pages; i++)
    {
	b = &buf_array[g->g_buffer + i];
	if ((b->buf_sts & BUF_MODIFIED) == 0)
	    continue;
 
	/* If any page in the group has an LSN greater than the last-known*/
	/* log force LSN, return FALSE immediately. If we make it thru    */
	/* all of the group's buffers without this occuring, return TRUE. */
	/* Since this function ignores 'busy' buffers, this may be inexact*/
	/* but this is preferable to the overhead needed for exactness;   */
	/* worse case is we will do an occasional logforce.               */

	page = (DMPP_PAGE *)((char *)lbm->lbm_pages[bm->bm_cache_ix] + 
			b->buf_page_offset);
	if (LSN_GT(DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(b->buf_relpgtype, page),
		&bm_common->bmcb_last_lsn))
	    return(FALSE);
    }

    return(TRUE);
}

/*{
** Name: dm0p_minit - Initialize a BM semaphore.
**
** Description:
**      This routine is used to initialize a BM semaphore.
**
** Inputs:
**      sem            - Pointer to semaphore to initialize.
**	sem_name       - The name to give to the semaphore.
**	sem_type       - CS_SEM_MULTI if initializing a shared cache.
**		         CS_SEM_SINGLE if initializing a local cache.
**
** Outputs:
**      none
**      Returns:
**	    E_DB_OK    if all'swell.
**          E_DB_FATAL if any problems
**
** Side Effects:
**      None.
**
** History:
**	30-Aug-1996 (jenjo02)
**	    Invented.
*/
static DB_STATUS
dm0p_minit(DM0P_SEMAPHORE *sem, 
	char *sem_name, 
	i4  sem_type)
{
    STATUS      status;
	 
    sem->bms_locked = BM_UNLOCKED;

    if (status = CSw_semaphore(&sem->bms_semaphore, sem_type, sem_name))
    {
	i4 err_code;

	ule_format(E_DM9303_DM0P_MUTEX, (CL_SYS_ERR *)0, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, &err_code, 0);
	return (E_DB_FATAL);
    }

    return(E_DB_OK);
}

/*{
** Name: dm0p_mlock_fcn - Lock buffer manager semaphore.
**
** Description:
**      This routine is used to request a semaphore on a buffer manager
**      object and is called only if the in-lined semaphore request
**	fails.
**
**      This routine will bring the server down if an unexpected error
**      occurs.
**
** Inputs:
**	mode         - mode in which to lock it, EXCL or SHARE
**      sem   	     - Pointer to DM0P_SEMAPHORE to lock.
**	line	     - Source line number which made this call,
**		       for debugging
**
** Outputs:
** 	sem->bms_locked is set to the lock mode of the semaphore.
**
**      Returns:
**          none
**
** Side Effects:
**      Server will be brought down if an error occurs.
**
** History:
**       6-feb-1989 (rogerk)
**          Written for Terminator
**      23-may1994 (andyw)
**          Should report failure of CSp_semaphore()
**      23-may-1994 (andyw)
**          Correction to ule_format() call where sys_err is un-initialized.
**	30-Aug-1996 (jenjo02)
**	    Added mode, TRdisplay on failure.
**	10-Jan-2000 (jenjo02)
**	    Set bms_locked -before- acquiring semaphore to
**	    signal intent to other threads that check
**	    bms_locked.
**	    Inlined semaphore function calls. This function is now called
**	    only if the inlined CSp_semaphore fails and is itself
**	    expected to fail.
**	20-aug-2002 (devjo01)
**	    Return status to supress compiler warning.
**	26-Jan-2004 (kinte01)
**	    Need an include of cx.h to pick up CX error messages
**	27-Apr-2005 (schka24)
**	    Don't set hint until after we get the mutex, or someone else
**	    could change it.
*/
static i4
dm0p_mlock_fcn(
i4 		mode, 
DM0P_SEMAPHORE *sem, 
i4 		line)
{
    STATUS      status;
	 
    if (status = CSp_semaphore(mode, &sem->bms_semaphore))
    {
	i4     err_code;
	CS_SEM_STATS sstat;

	/*
	** Use CSs_semaphore() to get the name of the failed mutex,
	** if we can.
	*/
	if ((CSs_semaphore(CS_INIT_SEM_STATS, &sem->bms_semaphore, 
				&sstat, sizeof(sstat))) == OK)
	{
	    TRdisplay("%@ dm0p_mlock() CSp_semaphore failed for %s :%d\n",
			sstat.name, line);
	}
	else
	{
	    TRdisplay("%@ dm0p_mlock() CSp_semaphore failed :%d\n",
			line);
	}

	ule_format(status, (CL_ERR_DESC *)0, ULE_LOG , NULL,
		  (char * )0, 0L, (i4 *)0, &err_code, 0 );
					     
	dmd_check(E_DMF001_MUTEX_LOCK);
    }

    sem->bms_locked = (mode) ? BM_EXCL : BM_SHARE;

    return status;
}

/*{
** Name: dm0p_munlock - Unlock buffer manager semaphore.
**
** Description:
**      This routine is used to release a semaphore on a buffer manager
**      object and is called only if the in-lined semaphore request
**	fails.
**
**      This routine will bring the server down if an unexpected error
**      occurs (like semaphore not held).
**
** Inputs:
**      sem   	     - Pointer to semaphore to unlock.
**	line	     - Source line number which made this call,
**		       for debugging
**
** Outputs:
**	sem->bms_locked is set to unlocked state.
**      Returns:
**          none
**
** Side Effects:
**      Server will be brought down if an error occurs.
**
** History:
**       6-feb-1989 (rogerk)
**          Written for Terminator
**      23-may-1994 (andyw)
**          Should report failure of CSp_semaphore()
**      23-may-1994 (andyw)
**          Correction to ule_format() where sys_err is un-initialized.
**	30-Aug-1996 (jenjo02)
**	    Added TRdisplay on failure.
**	10-Jan-2000 (jenjo02)
**	    Inlined semaphore function calls. This function is now called
**	    only if the inlined CSv_semaphore fails.
**	20-aug-2002 (devjo01)
**	    return status to quiet compiler warning.
*/
static i4
dm0p_munlock_fcn(
DM0P_SEMAPHORE  *sem,
i4 		line)
{
    STATUS      status;

    sem->bms_locked = BM_UNLOCKED;

    if (status = CSv_semaphore(&sem->bms_semaphore))
    {
	i4     err_code;
	CS_SEM_STATS sstat;
	/*
	** Use CSs_semaphore() to get the name of the failed mutex,
	** if we can.
	*/
	if ((CSs_semaphore(CS_INIT_SEM_STATS, &sem->bms_semaphore,
				&sstat, sizeof(sstat))) == OK)
	{
	    TRdisplay("%@ dm0p_munlock() CSv_semaphore failed for %s :%d\n",
			sstat.name, line);
	}
	else
	{
	    TRdisplay("%@ dm0p_munlock() CSv_semaphore failed :%d\n",
			line);
	}

	ule_format(status, (CL_ERR_DESC *)0, ULE_LOG , NULL,
		  (char * )0, 0L, (i4 *)0, &err_code, 0 );
					     
	dmd_check(E_DMF003_MUTEX_UNLOCK);
    }

    return status;
}

/*{
** Name: dm0p_bmid - Return unique buffer manager id.
**
** Description:
**	This routine returns the unique id of this buffer manager.  The
**	id is guaranteed to be unique among a local dbms installation
**	(where a single logging/locking system is used).  It may not be
**	unique between two buffer managers on separate nodes of a cluster
**	installation.
**
**	This id is used to lock databases for fast commit to make sure
**	that all access to a database open with fast commit goes through
**	the same buffer manager.
**
** Inputs:
**	none
**
** Outputs:
**	Returns: buffer manager id
**
** Side Effects:
**	none
**
** History:
**	 6-feb-1989 (rogerk)
**	    Written for Terminator
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
*/
i4
dm0p_bmid(void)
{
    return(dmf_svcb->svcb_lbmcb_ptr->lbm_bm_common->bmcb_id);
}

/*{
** Name: dm0p_cpdone - Inform buffer manager of completion of Consitency Point
**
** Description:
**	This routine informs the buffer manager that a consistency point
**	has been completed and it should do whatever is necessary to
**	prepare for the next one.
**
**	This includes resetting the buffer index's for monitoring the progress
**	of the CP, and incrementing the cpcount which identifies the next
**	consistency point.
**
** Inputs:
**	none
**
** Outputs:
**	none
**	Returns:
**	    none
**
** Side Effects:
**	none
**
** History:
**	 6-feb-1989 (rogerk)
**	    Written for Terminator
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	06-May-1999 (jenjo02)
**	    Reset CP queue and count of cp buffers.
*/
VOID
dm0p_cpdone(void)
{
    DMP_LBMCB	        *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON      *bm_common = lbm->lbm_bm_common;
    DM0P_BMCB		*bm;
    i4			cache_ix;

    /* Lock the cp information */
    dm0p_mlock(SEM_EXCL, &bm_common->bmcb_cp_mutex);

    /*
    ** Reset cpindex - this is used to keep track of how many pages were processed
    **     during the consistency point.
    ** Reset cpcheck - this is used to track the number of servers which have
    **     completed the consistency point.
    ** Increment cpcount - this is used to identify the consistency point so
    **     the fast commit threads can tell which pages have been processed
    **     for the current consistency point.
    ** Reset bmcb_cpbufs - this is the count of CP-eligible buffers in all caches.
    ** Reset bm_cpbufs - this is the count of CP-eligible buffers in 
    **	   each cache.
    */
    if (bm_common->bmcb_cpindex)
    {
	bm_common->bmcb_cpindex = 0;
	bm_common->bmcb_cpcheck = 0;
	bm_common->bmcb_cpbufs = 0;

	for ( cache_ix = 0; cache_ix < DM_MAX_CACHE; cache_ix++ )
	{
	    bm = lbm->lbm_bmcb[cache_ix];
	    if ( bm )
	    {
	       bm->bm_cpbufs = 0;
	       bm->bm_cpindex = 0;
	    }
	}

	if (++bm_common->bmcb_cpcount == 0)
	    bm_common->bmcb_cpcount++;
    }
    dm0p_munlock(&bm_common->bmcb_cp_mutex);
}

/*{
** Name: dm0p_count_connections - Check number of connections to Buffer Manager
**
** Description:
**	This routine verifies the number of servers that are connected to the
**	buffer manager and updates bm_srv_count if needed.  It is called
**	at the beginning of each consistency point in order to verify that
**	that the cpcheck, cpindex algorithms will work.  These algorithms will
**	not work unless the server count is correct.
**
**	The number of connections is verified by checking the number of
**	processes which hold the buffer manager lock (using LKshow).  Each
**	server connected to this buffer manager must hold this lock.
**
**	Since it is possible for a server to exit abnormally without adjusting
**	the bm_srv_count, this routine must be called at the start of each
**	CP to make sure that we start with a good number.  If a server exits
**	abnormally during a consistency point, the logging system must restart
**	the CP to make sure that all buffers are flushed.
**
**	Currently, the server does not know how to recover from an abnormal
**	disconnection from the buffer manager.  If this routine sees a bad
**	connect count it will shut down the server.  In the future, this
**	routine may be able to fix up the buffer manager and continue normally.
**
** Inputs:
**	none
**
** Outputs:
**	none
**	Returns:
**	    none
**
** Side Effects:
**	none
**
** History:
**	 6-feb-1989 (rogerk)
**	    Written for Terminator
**	10-apr-1989 (rogerk)
**	    Cause Fatal error if buffer manager connect count is wrong rather
**	    than just giving warning and expecting the logging system to handle
**	    any errors.
**	18-oct-1993 (rachael) merged 25-mar-1993 (bryanp) - B47950, B50252
**	    Just adding a comment here to assert that we do not need to check
**	    for LK_VAL_NOTVALID when converting the buffer manager lock up to
**	    LK_X. If we do get LK_VAL_NOTVALID, then we treat it as a fatal
**	    error and crash the server. There should be no way that we can
**	    ever get LK_VAL_NOTVALID for this lock request unless some other
**	    server which is connected to this cache has crashed while holding
**	    the buffer manager lock, in which case the cache is suspect and
**	    crashing this server is an appropriate thing to do.
**	15-Sep-1996 (jenjo02)
**	    Caller must hold bmcb_cp_mutex.
**      29-Sep-2004 (fanch01)
**          Add LK_LOCAL flag support as buffer names are confined to one node.
*/
VOID
dm0p_count_connections(void)
{
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    LK_RSB_INFO	    info_block;
    LK_VALUE	    lock_value;
    u_i4	    result_size;
    i4	    new_count;
    i4	    error;
    STATUS	    cl_status;
    CL_SYS_ERR	    sys_err;

    if ((bm_common->bmcb_status & BMCB_SHARED_BUFMGR) == 0)
	return;

    /*
    ** Get Buffer Manager lock exclusive to check the number of
    ** connections to it.
    */
    cl_status = LKrequest(
	LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK | LK_LOCAL,
	(LK_LLID)dmf_svcb->svcb_lock_list, (LK_LOCK_KEY *)NULL, LK_X,
	&lock_value, &lbm->lbm_bm_lockid, (i4)0, &sys_err);
    if (cl_status != OK)
    {
	ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &error, 0);
	ule_format(E_DM901C_BAD_LOCK_REQUEST, &sys_err, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, &error, 2,
	    0, LK_X, 0, dmf_svcb->svcb_lock_list);
	ule_format(E_DM937C_BM_BUFMGR_LOCK_ERROR, &sys_err, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, &error, 0);
    }

    /*
    ** Call LKshow to get number of connections to buffer manager lock -
    ** and thus the number of servers connected to the buffer manager.
    */
    cl_status = LKshow(LK_S_SRESOURCE, (LK_LLID)0, &lbm->lbm_bm_lockid,
	(LK_LOCK_KEY *)0, sizeof(info_block), (PTR)&info_block,
	&result_size, (u_i4 *)NULL, &sys_err);
    if (cl_status == OK)
    {
	new_count = info_block.rsb_count;
	if (new_count != bm_common->bmcb_srv_count)
	{
	    ule_format(E_DM937D_BM_CONNECTIONS, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, &error, 3,
		DB_MAXNAME, bm_common->bmcb_name.db_name,
		0, bm_common->bmcb_srv_count, 0, new_count);
	    dmd_check(E_DMF025_BM_CONNECT_COUNT);
	}
    }
    else
    {
	ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &error, 0);
	ule_format(E_DM901D_BAD_LOCK_SHOW, &sys_err, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, &error, 1,
	    0, lbm->lbm_bm_lockid);
	ule_format(E_DM937C_BM_BUFMGR_LOCK_ERROR, &sys_err, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, &error, 0);
    }

    /*
    ** Release exclusive lock so other servers can count connections.
    */
    cl_status = LKrequest(
	LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK | LK_LOCAL,
	(LK_LLID)dmf_svcb->svcb_lock_list, (LK_LOCK_KEY *)NULL, LK_N,
	&lock_value, &lbm->lbm_bm_lockid, (i4)0, &sys_err);
    if (cl_status != OK)
    {
	ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &error, 0);
	ule_format(E_DM901C_BAD_LOCK_REQUEST, &sys_err, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, &error, 2,
	    0, LK_N, 0, dmf_svcb->svcb_lock_list);
	ule_format(E_DM937C_BM_BUFMGR_LOCK_ERROR, &sys_err, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, &error, 0);
    }
}

/*{
** Name: dm0p_toss_pages - toss pages for a database/table from cache
**
** Description:
**	This routine scans through the buffer manager and tosses any pages
**	that belong to the specified database/table.
**
**	It cannot guarantee that upon exit there will be NO pages left
**	in the cache for the specified database/table - only that any
**	left are protected by other servers.
**
**	This routine is called when a server notices that a change may
**	have occurred to a table (or entire database) that may invalidate
**	the information in our cache.  This includes specific DBU operations
**	such as MODIFY (which will change the structure of a table without
**	changing the cache locks on each page of the table) or simply the
**	event of a database open by another server (which may have then
**	changed pages without bumping cache locks).
**
**	Since other servers and/or sessions may be connected to this buffer
**	manager, this routine does not guarantee upon returning that all
**	pages are tossed from the buffer cache.  Pages that are fixed by
**	other threads are not tossed - these are assumed to be valid.  Also
**	as this routine scans through the cache, new pages on the specified
**	table/database may be being faulted in by another session.
**
**	Unless directed otherwise by the 'toss_modified' flag, this routine
**	will skip over modified pages in the cache.  These are assumed to
**	be validated and protected by the server which has modified them.
**
** Inputs:
**	dbid		- database id - 0 for all databases
**	tabid		- table id - 0 for all tables
**	lock_list	- lock list to do LK waits on
**	toss_modified	- whether to toss modified pages as well.
**
** Outputs:
**	None
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	none
**
** History:
**	 6-feb-1989 (rogerk)
**	    Created for Terminator.
**	10-apr-1989 (rogerk)
**	    Allow to toss all pages for all databases.
**	 4-feb-1991 (rogerk)
**	    Added 'toss_modified' argument to allow caller to completely
**	    flush the data cache.  This is used for trace point DM421.
**	26-oct-1992 (rogerk)
**	    Reduced logging project: changed references to tcb fields to
**	    use new tcb_table_io structure fields.  Changed from old
**	    dm2t_get_tcb_ptr to new fix_tableio_ptr.
**	18-oct-1993 (rmuth)
**	    Add DMP_BMCB statistics.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**      01-aug-1996 (nanpr01 for ICL itb)
**	    Added the gather write and async io amendments.
**	08-May-1997 (jenjo02)
**	    Check buf_sts before dbid, tabid. We may be called while holding   
**	    a buffer's mutex and don't want to error trying to take that
**	    mutex a second time.
**	25-Aug-1997 (jenjo02)
**	    Added log_id to prototype, passing it to force_page().
**	10-Dec-1997 (jenjo02)
**	    Keep reusing the TBIO as long as possible instead of unfixing it
**	    after every force.
**	15-Jan-1998 (jenjo02)
**	    When database_id and table are both known, use much faster
**	    table hash queue to find all pages belonging to the table.
**	    The hash queue is a list of all BUF_VALID
**	    pages belonging to a table (TCB), including index pages, in
**	    all caches.
**	23-Jan-1998 (jenjo02)
**	    When the table is not know, search all table hash queues instead
**	    of sequentially searching all caches. This will yield the same
**	    result (find all pages for a database), but will be less costly.
**	12-Mar-2003 (jenjo02)
**	    Release buf_mutex before calling unfix_tableio_ptr(),
**	    retry buffer to avoid rare E_CL2517_CS_SMPR_DEADLOCK
**	    error in fault_page() (B109819).
*/
VOID
dm0p_toss_pages(
i4	    dbid,
i4	    tabid,
i4	    lock_list,
i4	    log_id,
i4	    toss_modified)
{
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BSTAT	    *lbm_stats;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BMCB       *bm;
    DB_STATUS	    status = E_DB_OK;
    DM0P_BUFFER	    *b;
    DMPP_PAGE	    *page;
    DMP_TABLE_IO    *tbio = NULL;
    i4	    error;
    i4		    i;
    i4	    	    gw_queued = 0;
    DM0P_BLQ_HEAD   *tab_bucket;
    DM0P_BID	    next_bid;
    
    /*
    ** If both database and table are known, search only the
    ** table's hash queue.
    */
    if (dbid && tabid)
    {
	i = (dbid + (u_i4)tabid) & (BMCB_TABLE_MAX - 1);
    }
    else
    {
	/* Table not know. Search all table hash queues */
	i = 0;
    }

    tab_bucket = &bm_common->bmcb_tabq_buckets[i];

    next_bid = tab_bucket->blqh_bid;

    /*
    ** Scan the buffer manager looking for buffers belonging to this
    ** table/database.  When find such a page, toss it out of the cache.
    **
    ** Don't toss MODIFIED, FIXED or MUTEX pages.  These are all assumed
    ** to be valid (the fixer, modifier must have verified the page's
    ** contents and must have a lock on the page to protect it from being
    ** invalidated by another server/session).
    **
    ** (If the caller has specified 'toss_modified' then MODIFIED pages
    ** will be tossed - FIXED and MUTEX pages cannot be tossed)
    **
    ** We have to wait for BUSY buffers since they may need to be tossed,
    ** but the session which has marked them busy is not going to toss
    ** them (for instance they are being written by the FC thread).  In
    ** this case, force_page will return a warning and we need to retry.
    */
    for (;;)
    {
	if (next_bid.bid_index == BUF_ENDLIST)
	{
	    /*
	    ** End of this hash chain.
	    ** If searching for a specific table, we're done,
	    ** otherwise move on to next table hash.
	    */

	    if ((!dbid || !tabid) &&
	        ++i < BMCB_TABLE_MAX)
	    {
		tab_bucket = &bm_common->bmcb_tabq_buckets[i];
		next_bid   = tab_bucket->blqh_bid;
		continue;
	    }
	    break;
	}
	else
	    b = &lbm->lbm_buffers[next_bid.bid_cache][next_bid.bid_index];
	
	/*
	** If buffer does not belong to this database/table, or is  
	** fixed or busy, of if page is modified and "toss_modified"
	** is not asserted, then skip it.
	*/
	if ((b->buf_sts & BUF_VALID) == 0 ||
	    (dbid && b->buf_dbid != dbid) ||
	    (tabid && b->buf_tabid.db_tab_base != tabid) ||
	    (b->buf_sts & BUF_MODIFIED && !(toss_modified)) ||
	     b->buf_sts & (BUF_FIXED | BUF_MUTEX) ||
	     b->buf_tabq_bucket != i)
	{
	    if (b->buf_tabq_bucket == i)
		next_bid = b->buf_tabq.blq_next;
	    else
		next_bid = tab_bucket->blqh_bid;
	    continue;
	}

	/* Lock the buffer, then check again */
	dm0p_mlock(SEM_EXCL, &b->buf_mutex);

	next_bid = b->buf_tabq.blq_next;

	if ((b->buf_sts & BUF_VALID) == 0 ||
	    (dbid && b->buf_dbid != dbid) ||
	    (tabid && b->buf_tabid.db_tab_base != tabid) ||
	    (b->buf_sts & BUF_MODIFIED && !(toss_modified)) ||
	     b->buf_sts & (BUF_FIXED | BUF_MUTEX) ||
	     b->buf_tabq_bucket != i)
	{
	    dm0p_munlock(&b->buf_mutex);
	    if (b->buf_tabq_bucket != i)
		next_bid = tab_bucket->blqh_bid;
	    continue;
	}

	lbm_stats = &lbm->lbm_stats[b->buf_id.bid_cache];
	bm = lbm->lbm_bmcb[b->buf_id.bid_cache];

	/*
	** We must look up the TCB pointer in order to toss a modified
	** page since force_page will need it to do the write.
	**
	** If we can't find the TCB, then skip the page.
	*/
	if (b->buf_sts & BUF_MODIFIED)
	{
	    /* If we already have a TBIO, try to use it */
	    if (!tbio ||
		(b->buf_dbid != tbio->tbio_dcbid ||
		 b->buf_tabid.db_tab_base != tbio->tbio_reltid.db_tab_base ||
		 b->buf_tabid.db_tab_index != tbio->tbio_reltid.db_tab_index))
	    
	    {
		/* Wrong table, release buffer, TBIO, retry */
		if ( tbio )
		{
		    next_bid = b->buf_id;
		    dm0p_munlock(&b->buf_mutex);
		    unfix_tableio_ptr(&tbio, lock_list, log_id, (i4)__LINE__);
		    continue;
		}

		/* 
		** Look up the TCB for this buffer.
		** If we can't find a valid TCB, then don't try to toss it.
		*/
		status = fix_tableio_ptr(b, (i4)0, lock_list, log_id,
		    &tbio, &error);

		if (DB_FAILURE_MACRO(status))
		{
		    /*
		    ** Record any errors in the error log, but continue
		    ** processing as though we found no table descriptor.
		    */
		    ule_format(error, (CL_SYS_ERR *)0, ULE_LOG, NULL, 
			(char *)0, (i4)0, (i4 *)0, &error, 0);
		}
		if ((status != E_DB_OK) || (tbio == NULL))
		{
		    dm0p_munlock(&b->buf_mutex);
		    continue;
		}
		
		/* Count another force */
		lbm_stats->bs_force[BMCB_PTYPES]++;
		bm->bm_stats.bs_force[BMCB_PTYPES]++;
		lbm_stats->bs_force[b->buf_type]++;
		bm->bm_stats.bs_force[b->buf_type]++;
	    }
	}

	/*
	** Toss this page.
	** The TCB pointer may be NULL if the page is not modified.
	*/
	status = force_page(b, tbio, FP_TOSS, lock_list, log_id,
		BMCL_TOSS_PAGES, &gw_queued, &error);

	/*
	** Check for retry needed.  Force_page will return a warning if the
	** buffer has an outstanding I/O on it (it is being forced out by
	** another thread).
	*/
	if ( status )
	{
	    if (status == E_DB_WARN)
	    {
		/* Retry with the same buffer */
		next_bid   = b->buf_id;
	    }
	    dm0p_munlock(&b->buf_mutex);
	}
    }

    /* Force any remaining queued GatherWrite buffers */
    if (gw_queued)
	(VOID)dm2f_force_list();

    if (tbio)
	/* If TBIO left around, release it now */
	unfix_tableio_ptr(&tbio, lock_list, log_id, (i4)__LINE__);


    return;
}

/*{
** Name: dm0p_dbcache - register open/close of db with buffer manager
**
** Description:
**	This routine is called whenever a server opens or closes a database.
**	The open call should be made after the database open has been registered
**	with the Logging System and the close call should be made before the
**	close has been registered with the Logging System.  This assures that
**	recovery is done correctly when a server exits abnormally.
**
**	This routine actually serves two puposes:
**	    1) It manages the dbcache list (locks and open counts).
**	    2) It verifes any cached pages on the database.
**
**	The buffer manager keeps a set of database cache entries.  These are
**	used to enable the buffer manager to keep pages cached after the
**	database has been closed.  Each entry has an open-count (the number of
**	servers that have this database open) and a cache lock value.  The
**	cache lock value tells us whether the database has been updated by a
**	different buffer manager (thus making changes without reflecting those
**	changes in our cache).  The open count tells us whether another server
**	already has this db open (thus it has already verified that all cached
**	pages are OK).
**
**	NOTE:  The open count in these enties is not guaranteed to be the
**	    actual count of opens of this database.  It is only guaranteed that
**	    if the count is > 0, some server (1 or more) must have this
**	    database open.  It is possible (through the reclaiming of cache
**	    entries) for the open count of an entry to be zero even though the
**	    database is open by some server.  The only effect of this situation
**	    is that we may toss pages that we could actually have used.
**
**	NOTE:  There is no guarantee that there is a database cache entry for
**	    each open database in the buffer manager.  In fact it is expected
**	    that there will not be.
**
**	Each time a database is opened by a server, this routine is called to
**	check the dbcache entry.  If there not one, or the open count is 0 and
**	the cache lock value is changed, then all pages for this database are
**	tossed from the cache.  If there is no cache entry, then one is
**	created.  The open count for that database is incremented.
**
**	Each time a database is closed, this routine is called to register the
**	close.  The entry for this database is looked up and if found, its open
**	count is decremented.  If there is no entry for this database, then one
**	is created with an open count of zero.  Thus if the database is opened
**	again in the near future, we should have a database cache entry to
**	check.
**
**	Each time a cache entry is referenced, it is moved to the top of the
**	dbcache list.  Each time a new entry is built, the last entry of the
**	list is scavanged to build the new one.
**
**	If we decide to purge the cache, return(E_DB_WARN) so the
**	caller can decide if other stuff tied to the DB needs to
**	be cleaned up as well.
**
**
** Inputs:
**	dbid		- database id
**	open		- TRUE if open, FALSE if close
**	write_mode	- TRUE if open for write
**
** Outputs:
**	err_code	- reason for error status
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR	- database cache lock could not be aquired
**	    E_DB_WARN	- Database cache was purged.
**	Exceptions:
**	    none
**
** Side Effects:
**	All cached pages for this database may be tossed from cache
**	if value of db cache lock has been changed by another server.
**
** History:
**	 6-feb-1989 (rogerk)
**	    Created for Terminator.
**	10-apr-1989 (rogerk)
**	    Make sure entry is valid before using it.
**	15-may-1989 (rogerk)
**	    Add error code parameter, return LOCK_QUOTA_EXCEEDED if run out
**	    of locks.
**	 4-feb-1991 (rogerk)
**	    Added new argument to dm0p_toss_pages calls.
**	24-may-1993 (bryanp)
**	    If trace point DM302 is set, trace cache locking activity.
**	18-oct-1993 (rachael) merged 25-mar-1993 (bryanp) -- B47950, B50252
**	    If LKrequest() returns LK_VAL_NOTVALID, then we must assume that
**	    cached information for this database is invalid and we must toss all
**	    cached pages.
**	18-oct-1993 (rmuth)
**	    Add DMP_BMCB statistics.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	13-Jan-2005 (jenjo02)
**	    Return E_DB_WARN if we decide to purge the cache.
*/
DB_STATUS
dm0p_dbcache(
i4	    dbid,
i4	    open,
i4	    write_mode,
i4	    lock_list,
i4	    *err_code)
{
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BMCACHE    *bmc = NULL;
    i4		    reclaim = FALSE;
    i4		    value_changed = FALSE;
    LK_LKID	    lock_id;
    LK_VALUE	    lock_value;
    LK_LOCK_KEY	    lock_key;
    CL_SYS_ERR	    sys_err;
    STATUS	    cl_status;
    DB_STATUS	    status = E_DB_OK;
    i4	    next_dbc, prev_dbc;

    for (;;)
    {
	/*
	** Search list for entry matching this database.
	*/
	dm0p_mlock(SEM_EXCL, &bm_common->bmcb_dbclist.sth_mutex);
	next_dbc = bm_common->bmcb_dbclist.sth_stq.stq_next;
	while (next_dbc != BUF_ENDLIST)
	{
	    bmc = &lbm->lbm_dbcache[next_dbc];
	    next_dbc = bmc->bmc_state.stq_next;

	    if (bmc->bmc_dbid != dbid || 
	       (bmc->bmc_sts & BMC_VALID) == 0)
	    {
		bmc = NULL;
		continue;
	    }

	    /* 
	    ** Unlock the queue, lock the bmc, relock the queue,
	    ** check again.
	    */
	    dm0p_munlock(&bm_common->bmcb_dbclist.sth_mutex);
	    dm0p_mlock(SEM_EXCL, &bmc->bmc_mutex);
	    dm0p_mlock(SEM_EXCL, &bm_common->bmcb_dbclist.sth_mutex);
	    if ((bmc->bmc_dbid == dbid) && (bmc->bmc_sts & BMC_VALID))
	    {
		break;
	    }
	    if (next_dbc != bmc->bmc_state.stq_next)
		next_dbc = bm_common->bmcb_dbclist.sth_stq.stq_next;
	    dm0p_munlock(&bmc->bmc_mutex);
	    bmc = NULL;
	}

	/*
	** If the buffer is busy, wait for it to be free.
	*/
	if (bmc && (bmc->bmc_sts & BMC_BUSY))
	{
	    lbm->lbm_bmcwait++;
	    bm_common->bmcb_bmcwait++;
	    bmc->bmc_sts |= BMC_WAIT;
	    dm0p_eset(lock_list, DM0P_BMCWAIT_EVENT, 0, bmc->bmc_index, 0);

	    dm0p_munlock(&bmc->bmc_mutex);
	    dm0p_munlock(&bm_common->bmcb_dbclist.sth_mutex);
	    dm0p_ewait(lock_list, DM0P_BMCWAIT_EVENT, 0, bmc->bmc_index, 0, 0);
	    continue;
	}

	/*
	** If can't find entry, build a new one.
	**
	** We still hold the dbclist mutex.
	*/
	if (bmc == NULL)
	{
	    reclaim_bmc(&bm_common->bmcb_dbclist, lbm->lbm_dbcache, &bmc);
	    if (bmc)
	    {
		/* reclaim_bmc() returns with bmc_mutex held */
		reclaim = TRUE;
		bmc->bmc_sts = (BMC_BUSY | BMC_VALID);
		bmc->bmc_dbid = dbid;
		bmc->bmc_tabid = 0;
		bmc->bmc_refcount = 0;

		/* Get cache lock for this database */
		lock_key.lk_type = LK_BM_DATABASE;
		lock_key.lk_key1 = dbid;
		lock_key.lk_key2 = lock_key.lk_key3 = lock_key.lk_key4 = 0;
		lock_key.lk_key5 = lock_key.lk_key6 = 0;

		if (DMZ_LCK_MACRO(2))
		    dmd_lock(&lock_key, lbm->lbm_lock_list, LK_REQUEST,
			    (LK_PHYSICAL | LK_NODEADLOCK),
			    LK_IS, 0, (DB_TRAN_ID *)0, (DB_TAB_NAME *)0,
			    (DB_DB_NAME *)0);

		cl_status = LKrequest((LK_PHYSICAL | LK_NODEADLOCK),
		    lbm->lbm_lock_list, &lock_key, LK_IS,
		    &bmc->bmc_lkvalue, &bmc->bmc_lkid, (i4)0, &sys_err);
		if (cl_status != OK && cl_status != LK_VAL_NOTVALID)
		{
		    /* Unexpected lock error */
		    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 0);
		    ule_format(E_DM9378_BM_NO_DBCACHE_LOCKS, &sys_err,
		     ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0,
		     err_code, 0);

		    /*
		    ** Treat lock error as though we do not have an entry.
		    ** Mark the cache entry as free so it can be reused.
		    */
		    if (bmc->bmc_sts & BMC_WAIT)
		    {
			dm0p_erelease(lock_list, DM0P_BMCWAIT_EVENT,
					0, bmc->bmc_index, 0);
		    }
		    bmc->bmc_sts = BMC_FREE;
		    dm0p_munlock(&bmc->bmc_mutex);
		    bmc = NULL;
		}
	    }
	}

	/*
	** If couldn't allocate an entry (cache list is empty), then we aren't
	** doing caching of database information when the db is closed.  If
	** this is an open we still need to bump the value lock for the
	** database to inform other servers that are caching information that
	** we have just opened the database.
	**
	** We hold bmc_mutex if there's a bmc, and dbclist's mutex.
	*/
	if (bmc == NULL)
	{
	    /* release the dbclist's mutex */
	    dm0p_munlock(&bm_common->bmcb_dbclist.sth_mutex);

	    if (open && write_mode)
	    {
		lock_key.lk_type = LK_BM_DATABASE;
		lock_key.lk_key1 = dbid;
		lock_key.lk_key2 = lock_key.lk_key3 = lock_key.lk_key4 = 0;
		lock_key.lk_key5 = lock_key.lk_key6 = 0;
		MEfill(sizeof(LK_LKID), 0, &lock_id);

		if (DMZ_LCK_MACRO(2))
		    dmd_lock(&lock_key, lbm->lbm_lock_list, LK_REQUEST,
			    (LK_PHYSICAL | LK_NODEADLOCK),
			    LK_SIX, 0, (DB_TRAN_ID *)0, (DB_TAB_NAME *)0,
			    (DB_DB_NAME *)0);

		cl_status = LKrequest((LK_PHYSICAL | LK_NODEADLOCK),
		    lbm->lbm_lock_list, &lock_key, LK_SIX,
		    &lock_value, &lock_id, (i4)0, &sys_err);
		if (cl_status == OK || cl_status == LK_VAL_NOTVALID)
		{
		    lock_value.lk_low++;
		    cl_status = LKrelease((i4)0, lbm->lbm_lock_list,
			&lock_id, &lock_key, &lock_value, &sys_err);
		}
		if (cl_status != OK)
		{
		    if (cl_status == LK_NOLOCKS)
		    {
			if (dm0p_reclaim(lock_list, 0) == E_DB_OK)
			    continue;
		    }

		    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 0);
		    *err_code = E_DM9379_BM_CANT_OPEN_DB;
		    if (cl_status == LK_NOLOCKS)
		    {
			ule_format(E_DM9379_BM_CANT_OPEN_DB, &sys_err, ULE_LOG, NULL,
			    (char *)0, (i4)0, (i4 *)0, err_code, 0);
			*err_code = E_DM004B_LOCK_QUOTA_EXCEEDED;
		    }
		    return (E_DB_ERROR);
		}
	    }
	    if (open)
	    {
		dm0p_toss_pages(dbid, (i4)0, lock_list, (i4)0, 0);
		status = E_DB_WARN;
	    }
	    return (status);
	}

	/*
	** Requeue to front of dbcache list,
	** but only if its not already on the front of the list.
	**
	** We still have an EXCL lock on dbclist and bmc_mutex.
	*/
	if (bm_common->bmcb_dbclist.sth_stq.stq_next != bmc->bmc_index)
	{
	    /* Take out of current position in list */
	    next_dbc = bmc->bmc_state.stq_next;
	    prev_dbc = bmc->bmc_state.stq_prev;
	    if (next_dbc != BUF_ENDLIST)
		lbm->lbm_dbcache[next_dbc].bmc_state.stq_prev = prev_dbc;
	    else
		bm_common->bmcb_dbclist.sth_stq.stq_prev = prev_dbc;
	    if (prev_dbc != BUF_ENDLIST)
		lbm->lbm_dbcache[prev_dbc].bmc_state.stq_next = next_dbc;
	    else
		bm_common->bmcb_dbclist.sth_stq.stq_next = next_dbc;

	    /* Put on front of list */
	    next_dbc = bm_common->bmcb_dbclist.sth_stq.stq_next;
	    bmc->bmc_state.stq_next = next_dbc;
	    bmc->bmc_state.stq_prev = BUF_ENDLIST;
	    if (next_dbc != BUF_ENDLIST)
		lbm->lbm_dbcache[next_dbc].bmc_state.stq_prev = bmc->bmc_index;
	    else
		bm_common->bmcb_dbclist.sth_stq.stq_prev = bmc->bmc_index;
	    bm_common->bmcb_dbclist.sth_stq.stq_next = bmc->bmc_index;
	}

	/* Unlock the dbclist's mutex */
	dm0p_munlock(&bm_common->bmcb_dbclist.sth_mutex);

	if (open)
	{
	    /*
	    ** Check the value of the cache lock and then increment it.
	    ** If the value has changed then we may have to toss all cached
	    ** pages for this database.
	    **
	    ** If we get a lock error trying to validate/bump the lock,
	    ** then release the lock, free the cache entry and retry.
	    ** If we fail trying to build a new entry then we will return
	    ** with an error.
	    */

	    /* Set busy in case we haven't already done this */
	    bmc->bmc_sts |= BMC_BUSY;

	    if (DMZ_LCK_MACRO(2))
	    {
		lock_key.lk_type = LK_BM_DATABASE;
		lock_key.lk_key1 = bmc->bmc_dbid;
		lock_key.lk_key2 = lock_key.lk_key3 = lock_key.lk_key4 = 0;
		lock_key.lk_key5 = lock_key.lk_key6 = 0;
		dmd_lock(&lock_key, lbm->lbm_lock_list, LK_CONVERT,
			    (LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK),
			    LK_SIX, 0, (DB_TRAN_ID *)0, (DB_TAB_NAME *)0,
			    (DB_DB_NAME *)0);
	    }

	    cl_status = LKrequest((LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK),
		lbm->lbm_lock_list, (LK_LOCK_KEY *)0, LK_SIX,
		&lock_value, &bmc->bmc_lkid, (i4)0, &sys_err);
	    if (cl_status != OK && cl_status != LK_VAL_NOTVALID)
	    {
		ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
		ule_format(E_DM901C_BAD_LOCK_REQUEST, &sys_err,
		 ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, err_code, 2,
		 0, LK_SIX, 0, lbm->lbm_lock_list);
		cl_status = LKrelease((i4)0, lbm->lbm_lock_list,
		&bmc->bmc_lkid, (LK_LOCK_KEY *)0, &lock_value, &sys_err);
		if (cl_status != OK)
		{
		    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 0);
		    ule_format(E_DM901B_BAD_LOCK_RELEASE, &sys_err,
		     ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0, err_code, 1,
		     0, lbm->lbm_lock_list);
		}

		if (bmc->bmc_sts & BMC_WAIT)
		{
		    dm0p_erelease(lock_list, DM0P_BMCWAIT_EVENT, 0,
				    bmc->bmc_index, 0);
		}
		bmc->bmc_sts = BMC_FREE;
		dm0p_munlock(&bmc->bmc_mutex);
		continue;
	    }

	    if (((lock_value.lk_low != bmc->bmc_lkvalue.lk_low) ||
		(lock_value.lk_high != bmc->bmc_lkvalue.lk_high)) ||
		cl_status == LK_VAL_NOTVALID)
	    {
		value_changed = TRUE;
		STRUCT_ASSIGN_MACRO(lock_value, bmc->bmc_lkvalue);
	    }

	    if (write_mode)
		bmc->bmc_lkvalue.lk_low++;

	    if (DMZ_LCK_MACRO(2))
		dmd_lock(&lock_key, lbm->lbm_lock_list, LK_CONVERT,
			    (LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK),
			    LK_IS, 0, (DB_TRAN_ID *)0, (DB_TAB_NAME *)0,
			    (DB_DB_NAME *)0);

	    cl_status = LKrequest((LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK),
		lbm->lbm_lock_list, (LK_LOCK_KEY *)0, LK_IS,
		&bmc->bmc_lkvalue, &bmc->bmc_lkid, (i4)0, &sys_err);
	    if (cl_status != OK)
	    {
		ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 0);
		ule_format(E_DM901C_BAD_LOCK_REQUEST, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 2,
		    0, LK_IS, 0, lbm->lbm_lock_list);
		cl_status = LKrelease((i4)0, lbm->lbm_lock_list,
		    &bmc->bmc_lkid, (LK_LOCK_KEY *)0, &lock_value, &sys_err);
		if (cl_status != OK)
		{
		    ule_format(E_DM901B_BAD_LOCK_RELEASE, &sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 1,
			0, lbm->lbm_lock_list);
		}
		if (bmc->bmc_sts & BMC_WAIT)
		{
		    dm0p_erelease(lock_list, DM0P_BMCWAIT_EVENT, 0,
				    bmc->bmc_index, 0);
		}
		bmc->bmc_sts = BMC_FREE;
		dm0p_munlock(&bmc->bmc_mutex);
		continue;
	    }
	    
	    /*
	    ** if there was no cache entry for this database, or
	    ** the cache value changed and nobody connected to this
	    ** buffer manager has the db open, then toss all cached
	    ** pages for this database.
	    */
	    if ((value_changed && bmc->bmc_refcount <= 0) || reclaim)
	    {
		dm0p_toss_pages(dbid, (i4)0, lock_list, (i4)0, 0);
		status = E_DB_WARN;
	    }
	    bmc->bmc_refcount++;
	}
	else
	{
	    if (!reclaim)
		bmc->bmc_refcount--;
	    if (bmc->bmc_refcount < 0)
		bmc->bmc_refcount = 0;
	}

	if (bmc->bmc_sts & BMC_WAIT)
	{
	    dm0p_erelease(lock_list, DM0P_BMCWAIT_EVENT, 0, bmc->bmc_index, 0);
	}
	bmc->bmc_sts &= ~(BMC_WAIT | BMC_BUSY);

	dm0p_munlock(&bmc->bmc_mutex);
	return (status);
    }
}

/*{
** Name: dm0p_tblcache - register build/modify of table with buffer manager
**
** Description:
**	This routine manages the table cache lock array.  This is used to allow
**	us to close tables (physicallay - ie. TCB thrown away) without tossing
**	all pages belonging to the table from the buffer manager.  This will
**	allow pages to remain in the cache to be used by other servers that
**	still have the table open, or this server the next time it opens the
**	table.  This is also necessary to allow us to save cached information
**	when entire databases are closed.
**
**	This routine is necessary to implement multi-server protocols.  It is
**	not needed if the database is accessable through only one buffer
**	manager at a time.
**
**	Whenever a TCB is built for a table, this routine is called to see if
**	any cached pages for that table are still valid.  If a server using a
**	different buffer manager has modified the table while bypassing the
**	buffer manager page cache locks (any dbu operation), then we must toss
**	all pages for that table from the cache.
**
**	If we have a table cache entry, and the value of its lock is not
**	changed then we know any information in the cache for that table is
**	still valid.  If the lock value has changed, or if we have no cache
**	entry, then dm0p_toss_pages() is called to toss any pages belonging to
**	this table from the cache.
**
**	Whenever a server makes a change to a table that necessitates tossing
**	pages from the cache, it will call this routine with the MODIFY action.
**	This will bump the value of the cache lock - thus when any other buffer
**	manager next opens this table, it will toss all cached pages.
**
**	If the action specified is MODIFY, and we are unable to acquire and
**	bump the cache lock, then this routine will return E_DB_WARN.  In this
**	case the caller should not continue with the table modifying operation
**	as other servers will not see that they must invalidate cached pages
**	for that table.
**
** Inputs:
**	dbid		- database id
**	tabid		- table id
**	action		- cache action:
**			    DM0P_VERIFY - check cache lock to see if cached
**				pages for specified table are still valid.
**			    DM0P_MODIFY - bump cache lock value in order
**				to invalidate cached pages in other servers.
**	lock_list	- lock list to use for waits
**
** Outputs:
**	err_code	- reason for error status
**	Returns:
**	    E_DB_OK
**	    E_DB_WARN	- table cache lock could not be acquired
**	Exceptions:
**	    none
**
** Side Effects:
**	none
**
** History:
**	 6-feb-1989 (rogerk)
**	    Created for Terminator.
**	10-apr-1989 (rogerk)
**	    Make sure entry is valid before using it.
**	15-may-1989 (rogerk)
**	    Add error code parameter, return LOCK_QUOTA_EXCEEDED if run out
**	    of locks.
**	 4-feb-1991 (rogerk)
**	    Added new argument to dm0p_toss_pages call.
**	24-may-1993 (bryanp)
**	    If trace point DM302 is set, trace cache locking activity.
**	18-oct-1993 (rachael) merged 25-mar-1993 (bryanp) -- B47950, B50252
**	    If LKrequest() returns LK_VAL_NOTVALID, then we must assume that
**	    cached information for this table is invalid and we must toss all
**	    cached pages.
**	18-oct-1993 (rmuth)
**	    Add DMP_BMCB statistics.
**	25-Aug-1997 (jenjo02)
**	    Added log_id to prototype, passing it to dm0p_toss_pages().
*/
DB_STATUS
dm0p_tblcache(
i4	    dbid,
i4	    tabid,
i4	    action,
i4	    lock_list,
i4	    log_id,
i4	    *err_code)
{
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BMCACHE    *bmc = NULL;
    i4		    reclaim = FALSE;
    i4		    value_changed = FALSE;
    LK_LKID	    lock_id;
    LK_VALUE	    lock_value;
    LK_LOCK_KEY	    lock_key;
    CL_SYS_ERR	    sys_err;
    STATUS	    cl_status;
    i4	    next_tblc, prev_tblc;

    for (;;)
    {
	/*
	** Search list for entry matching this database.
	*/
	dm0p_mlock(SEM_EXCL, &bm_common->bmcb_tblclist.sth_mutex);
	next_tblc = bm_common->bmcb_tblclist.sth_stq.stq_next;
	while (next_tblc != BUF_ENDLIST)
	{
	    bmc = &lbm->lbm_tblcache[next_tblc];
	    next_tblc = bmc->bmc_state.stq_next;
	    if (bmc->bmc_tabid != tabid || 
		bmc->bmc_dbid != dbid ||
	       (bmc->bmc_sts & BMC_VALID) == 0)
	    {
		bmc = NULL;
		continue;
	    }

	    /*
	    ** Unlock the queue, lock the bmc, relock the queue,
	    ** then check the bmc again.
	    */
	    dm0p_munlock(&bm_common->bmcb_tblclist.sth_mutex);
	    dm0p_mlock(SEM_EXCL, &bmc->bmc_mutex);
	    dm0p_mlock(SEM_EXCL, &bm_common->bmcb_tblclist.sth_mutex);

	    if (bmc->bmc_tabid == tabid && 
		bmc->bmc_dbid == dbid &&
		bmc->bmc_sts & BMC_VALID)
	    {
		break;
	    }

	    if (next_tblc != bmc->bmc_state.stq_next)
		next_tblc = bm_common->bmcb_tblclist.sth_stq.stq_next;
	    dm0p_munlock(&bmc->bmc_mutex);
	    bmc = NULL;
	}

	/*
	** If the buffer is busy, wait for it to be free.
	*/
	if (bmc && (bmc->bmc_sts & BMC_BUSY))
	{
	    lbm->lbm_bmcwait++;
	    bm_common->bmcb_bmcwait++;
	    bmc->bmc_sts |= BMC_WAIT;
	    dm0p_eset(lock_list, DM0P_BMCWAIT_EVENT, 0,  bmc->bmc_index, 0);

	    dm0p_munlock(&bm_common->bmcb_tblclist.sth_mutex);
	    dm0p_munlock(&bmc->bmc_mutex);
	    dm0p_ewait(lock_list, DM0P_BMCWAIT_EVENT, 0,  bmc->bmc_index, 0, 0);
	    continue;
	}

	/*
	** If can't find entry, build a new one.
	*/
	if (bmc == NULL)
	{
	    reclaim_bmc(&bm_common->bmcb_tblclist, lbm->lbm_tblcache, &bmc);
	    if (bmc)
	    {
		/* reclaim_bmc() returns with bmc_mutex locked */
		reclaim = TRUE;
		bmc->bmc_sts = (BMC_BUSY | BMC_VALID);
		bmc->bmc_dbid = dbid;
		bmc->bmc_tabid = tabid;
		bmc->bmc_refcount = 0;

		/* Get cache lock for this table */
		lock_key.lk_type = LK_BM_TABLE;
		lock_key.lk_key1 = dbid;
		lock_key.lk_key2 = tabid;
		lock_key.lk_key3 = lock_key.lk_key4 = 0;
		lock_key.lk_key5 = lock_key.lk_key6 = 0;

		if (DMZ_LCK_MACRO(2))
		    dmd_lock(&lock_key, lbm->lbm_lock_list, LK_REQUEST,
			    (LK_PHYSICAL | LK_NODEADLOCK),
			    LK_IS, 0, (DB_TRAN_ID *)0, (DB_TAB_NAME *)0,
			    (DB_DB_NAME *)0);

		cl_status = LKrequest((LK_PHYSICAL | LK_NODEADLOCK),
		    lbm->lbm_lock_list, &lock_key, LK_IS,
		    &bmc->bmc_lkvalue, &bmc->bmc_lkid, (i4)0, &sys_err);
		if (cl_status != OK && cl_status != LK_VAL_NOTVALID)
		{
		    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 0);
		    ule_format(E_DM937A_BM_NO_TBLCACHE_LOCKS, &sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 0);

		    /*
		    ** Treat lock error as though we do not have an entry.
		    ** Mark the cache entry as free so it can be reused.
		    */
		    if (bmc->bmc_sts & BMC_WAIT)
		    {
			dm0p_erelease(lock_list, DM0P_BMCWAIT_EVENT, 0,
					bmc->bmc_index, 0);
		    }
		    bmc->bmc_sts = BMC_FREE;
		    dm0p_munlock(&bmc->bmc_mutex);
		    bmc = NULL;
		}
	    }
	}

	/*
	** If couldn't allocate an entry (cache list is empty), then we aren't
	** doing caching of table information when the table is closed.  If
	** this is a MODIFY action we still need to bump the value lock for the
	** table to inform other servers that are caching information.
	*/
	if (bmc == NULL)
	{
	    /* release the tblclist mutex */
	    dm0p_munlock(&bm_common->bmcb_tblclist.sth_mutex);

	    if (action & DM0P_MODIFY)
	    {
		lock_key.lk_type = LK_BM_TABLE;
		lock_key.lk_key1 = dbid;
		lock_key.lk_key2 = tabid;
		lock_key.lk_key3 = lock_key.lk_key4 = 0;
		lock_key.lk_key5 = lock_key.lk_key6 = 0;
		MEfill(sizeof(LK_LKID), 0, &lock_id);

		if (DMZ_LCK_MACRO(2))
		    dmd_lock(&lock_key, lbm->lbm_lock_list, LK_REQUEST,
			    (LK_PHYSICAL | LK_NODEADLOCK),
			    LK_SIX, 0, (DB_TRAN_ID *)0, (DB_TAB_NAME *)0,
			    (DB_DB_NAME *)0);

		cl_status = LKrequest((LK_PHYSICAL | LK_NODEADLOCK),
		    lbm->lbm_lock_list, &lock_key, LK_SIX,
		    &lock_value, &lock_id, (i4)0, &sys_err);
		if (cl_status == OK || cl_status == LK_VAL_NOTVALID)
		{
		    lock_value.lk_low++;
		    cl_status = LKrelease((i4)0, lbm->lbm_lock_list,
			&lock_id, &lock_key, &lock_value, &sys_err);
		}
		if (cl_status != OK)
		{
		    if (cl_status == LK_NOLOCKS)
		    {
			if (dm0p_reclaim(lock_list, 0) == E_DB_OK)
			    continue;
		    }

		    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 0);
		    *err_code = E_DM937B_BM_CANT_CHANGE_TABLE;
		    if (cl_status == LK_NOLOCKS)
		    {
			ule_format(E_DM937B_BM_CANT_CHANGE_TABLE, &sys_err,
			    ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0,
			    err_code, 0);
			*err_code = E_DM004B_LOCK_QUOTA_EXCEEDED;
		    }
		    return (E_DB_ERROR);
		}
	    }
	    if (action & DM0P_VERIFY)
		dm0p_toss_pages(dbid, tabid, lock_list, log_id, 0);
	    return (E_DB_OK);
	}

	/*
	** Requeue to front of tblcache list,
	** but only if its not already first on the list.
	**
	** We still hold bmc_mutex and tblclist's mutex.
	*/
	if (bm_common->bmcb_tblclist.sth_stq.stq_next != bmc->bmc_index)
	{
	    next_tblc = bmc->bmc_state.stq_next;
	    prev_tblc = bmc->bmc_state.stq_prev;
	    if (next_tblc != BUF_ENDLIST)
		lbm->lbm_tblcache[next_tblc].bmc_state.stq_prev = prev_tblc;
	    else
		bm_common->bmcb_tblclist.sth_stq.stq_prev = prev_tblc;
	    if (prev_tblc != BUF_ENDLIST)
		lbm->lbm_tblcache[prev_tblc].bmc_state.stq_next = next_tblc;
	    else
		bm_common->bmcb_tblclist.sth_stq.stq_next = next_tblc;

	    /* Put on front of list */
	    next_tblc = bm_common->bmcb_tblclist.sth_stq.stq_next;
	    bmc->bmc_state.stq_next = next_tblc;
	    bmc->bmc_state.stq_prev = BUF_ENDLIST;
	    if (next_tblc != BUF_ENDLIST)
		lbm->lbm_tblcache[next_tblc].bmc_state.stq_prev = bmc->bmc_index;
	    else
		bm_common->bmcb_tblclist.sth_stq.stq_prev = bmc->bmc_index;
	    bm_common->bmcb_tblclist.sth_stq.stq_next = bmc->bmc_index;
	}

	dm0p_munlock(&bm_common->bmcb_tblclist.sth_mutex);

	/*
	** Mark the cache entry and release the semaphore while we get/set
	** the cache lock value.
	*/
	bmc->bmc_sts |= BMC_BUSY;

	/*
	** Get the current value of the cache lock.
	*/

	if (DMZ_LCK_MACRO(2))
	{
	    lock_key.lk_type = LK_BM_TABLE;
	    lock_key.lk_key1 = bmc->bmc_dbid;
	    lock_key.lk_key2 = bmc->bmc_tabid;
	    lock_key.lk_key3 = lock_key.lk_key4 = 0;
	    lock_key.lk_key5 = lock_key.lk_key6 = 0;

	    dmd_lock(&lock_key, lbm->lbm_lock_list, LK_CONVERT,
			    (LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK),
			    LK_SIX, 0, (DB_TRAN_ID *)0, (DB_TAB_NAME *)0,
			    (DB_DB_NAME *)0);
	}

	cl_status = LKrequest((LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK),
	    lbm->lbm_lock_list, (LK_LOCK_KEY *)0, LK_SIX,
	    &lock_value, &bmc->bmc_lkid, (i4)0, &sys_err);
	if (cl_status != OK && cl_status != LK_VAL_NOTVALID)
	{
	    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 0);
	    ule_format(E_DM901C_BAD_LOCK_REQUEST, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 2,
		0, LK_SIX, 0, lbm->lbm_lock_list);
	    break;
	}

	if (((lock_value.lk_low != bmc->bmc_lkvalue.lk_low) ||
	    (lock_value.lk_high != bmc->bmc_lkvalue.lk_high)) ||
	    cl_status == LK_VAL_NOTVALID)
	{
	    value_changed = TRUE;
	    STRUCT_ASSIGN_MACRO(lock_value, bmc->bmc_lkvalue);
	}

	if (action & DM0P_MODIFY)
	    bmc->bmc_lkvalue.lk_low++;

	if (DMZ_LCK_MACRO(2))
	    dmd_lock(&lock_key, lbm->lbm_lock_list, LK_CONVERT,
			    (LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK),
			    LK_IS, 0, (DB_TRAN_ID *)0, (DB_TAB_NAME *)0,
			    (DB_DB_NAME *)0);

	cl_status = LKrequest((LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK),
	    lbm->lbm_lock_list, (LK_LOCK_KEY *)0, LK_IS,
	    &bmc->bmc_lkvalue, &bmc->bmc_lkid, (i4)0, &sys_err);
	if (cl_status != OK)
	{
	    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 0);
	    ule_format(E_DM901C_BAD_LOCK_REQUEST, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 2,
		0, LK_IS, 0, lbm->lbm_lock_list);
	    break;
	}
	    
	/*
	** if there was no cache entry for this database, or the cache value
	** changed then toss all cached pages for this table.
	*/
	if (value_changed || reclaim)
	    dm0p_toss_pages(dbid, tabid, lock_list, log_id, 0);

	if (bmc->bmc_sts & BMC_WAIT)
	{
	    dm0p_erelease(lock_list, DM0P_BMCWAIT_EVENT, 0, bmc->bmc_index, 0);
	}
	bmc->bmc_sts &= ~(BMC_WAIT | BMC_BUSY);

	dm0p_munlock(&bmc->bmc_mutex);
	return (E_DB_OK);
    }

    /*
    ** Got an error trying to do lock requests for this table.
    ** Free the cache entry and return a warning.
    */
    if (bmc->bmc_sts & BMC_VALID)
    {
	cl_status = LKrelease((i4)0, lbm->lbm_lock_list,
	    &bmc->bmc_lkid, (LK_LOCK_KEY *)0, &lock_value, &sys_err);
	if (cl_status != OK)
	{
	    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 0);
	    ule_format(E_DM901B_BAD_LOCK_RELEASE, &sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 1,
		0, lbm->lbm_lock_list);
	}
    }

    if (bmc->bmc_sts & BMC_WAIT)
    {
	dm0p_erelease(lock_list, DM0P_BMCWAIT_EVENT, 0, bmc->bmc_index, 0);
    }
    bmc->bmc_sts = BMC_FREE;

    dm0p_munlock(&bmc->bmc_mutex);


    if (action & DM0P_MODIFY)
    {
	ule_format(E_DM937B_BM_CANT_CHANGE_TABLE, &sys_err, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, err_code, 0);
    }
    *err_code = E_DM937A_BM_NO_TBLCACHE_LOCKS;
    return (E_DB_ERROR);
}

/*{
** Name: reclaim_bmc - find last recently used cache entry and free it
**
**	This routine is called whenever a new database or table cache entry is
**	to be built.  This routine takes the last available entry off of the
**	specified list and frees it.
**
**	An entry is available for freeing as long as it is not marked BUSY.
**	Entries are only marked busy while being built or freed.
**
**	If there are no non-free entries, then a NULL pointer will be passed
**	back.
**
**	Freeing the cache entry involves releasing the cache lock and marking
**	its status as BMC_FREE.
**
** Description:
**
** Inputs:
**	bmc_list	- Head of list of cache entries to scan
**	bmc		- Pointer to entry that has been freed.
**
** Outputs:
**	None
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	none
**
** History:
**	 6-feb-1989 (rogerk)
**	    Created for Terminator.
**	15-Sep-1996 (jenjo02)
**	    Caller must hold lock on bmc_list's mutex.
**	    If bmc returned here, its bmc_mutex will be locked.
*/
static VOID
reclaim_bmc(
DM0P_STATE_HEAD	*bmc_list,
DM0P_BMCACHE	*cache_array,
DM0P_BMCACHE	**bmc_entry)
{
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BMCACHE    *bmc = NULL;
    i4	    prev_bmc;
    i4	    error;
    STATUS	    cl_status;
    CL_SYS_ERR	    sys_err;

    prev_bmc = bmc_list->sth_stq.stq_prev;
    while (prev_bmc != BUF_ENDLIST)
    {
	bmc = &cache_array[prev_bmc];
	prev_bmc = bmc->bmc_state.stq_prev;
	if (bmc->bmc_sts & BMC_BUSY)
	{
	    bmc = NULL;
	    continue;
	}

	/*
	** Unlock the queue, lock the bmc, relock the queue,
	** then check bmc again.
	*/
	dm0p_munlock(&bmc_list->sth_mutex);
	dm0p_mlock(SEM_EXCL, &bmc->bmc_mutex);
	dm0p_mlock(SEM_EXCL, &bmc_list->sth_mutex);

	if ((bmc->bmc_sts & BMC_BUSY) == 0)
	    break;
	if (prev_bmc != bmc->bmc_state.stq_prev)
	    prev_bmc = bmc_list->sth_stq.stq_prev;

	dm0p_munlock(&bmc->bmc_mutex);
	bmc = NULL;
    }

    if (bmc == NULL)
    {
	*bmc_entry = NULL;
	return;
    }

    if (bmc->bmc_sts & BMC_VALID)
    {
	cl_status = LKrelease((i4)0, lbm->lbm_lock_list,
	    &bmc->bmc_lkid, (LK_LOCK_KEY *)0, (LK_VALUE *)0, &sys_err);
	if (cl_status != OK)
	{
	    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &error, 0);
	    ule_format(E_DM901B_BAD_LOCK_RELEASE, &sys_err, ULE_LOG, NULL, (char *)0,
		(i4)0, (i4 *)0, &error, 1, 0, lbm->lbm_lock_list);
	}
    }

    bmc->bmc_sts = BMC_FREE;
    *bmc_entry = bmc;
}

/*{
** Name: dm0p_eset - setup for event wait on buffer manager event
**
** Description:
**	Call LK to set up for event wait call.  Uses cross-process
**	lock event calls.  Should be used only for events signaled
**	in the Buffer Manger routines.
**
**	The lock type and value define the event.  These two arguments
**	should be identical between sessions that are cooperating on
**	the same event.  Since these events are handled accross DBMS
**	servers, the event arguments must be constant accross DBMS
**	processes (which means addresses of structures should not be
**	used as event values).
**
**	In general, the event_value argument should be the unique buffer
**	manager id (bm_id).  This serves to make sure that common events use
**	identical values between servers that are connected to a common buffer
**	manager, but that two servers connected to different buffer managers
**	will not use common event values.
**
**	The event type should be chosen from the list of event type values in
**	DM0P.H.  For event types that specify a set of event objects (such as
**	waits on individual buffers, or groups of structures), the event value
**	is shifted over and a value indicating the individual event object
**	should be added to the event type (for example, when waiting on a busy
**	buffer, the index of the buffer in the buffer array is added to the
**	event type).
**
**	Unless the return_on_error argument is specified, this will
**	dmd_check upon any LK error.
**
**	If the LK call is interrupted (and return_on_error is specified),
**	E_DB_WARN is returned.
**
**	The LK_EVENT is prepared as:
**
**		type_high	DM0P_._EVENT in high-order
**				cache_ix in low-order, if qualified by cache.
**		type_low	buf_index, g_index, 0, depending on event type.
**		value		always bmcb_id.
**
** Inputs:
**	lock_list	- list to do wait on
**	event_type	- The event type.
**	cache_ix	- the cache qualifier
**	event_object	- The object which is being waited on.
**	return_on_error	- if not specified, routine will dmd_check on an
**			  unexpected event.
**
** Outputs:
**	None
**	Returns:
**	    E_DB_OK
**	    E_DB_WARN	- LKevent call interrupted
**	    E_DB_ERROR	- LKevent error
**	Exceptions:
**	    none
**
** Side Effects:
**	Will crash server if error and return_on_error not specified.
**
** History:
**	 6-feb-1989 (rogerk)
**	    Created for Terminator.
**       11-jan-1993 (mikem)
**          su4_us5 port.  Compiler complained about no return at end of
**          function (it doesn't understand that dmd_check() never returns.
**	25-mar-96 (nick)
**	    Added DM0P_FCWAIT_EVENT support.
**	05-Feb-1997 (jenjo02)
**	    Added DM0P_GWAIT_EVENT.
**	20-Jul-1998 (jenjo02)
**	    Modified prototype to include cache_ix and event_object, 
**	    and exclude event value which was always bm_common->bmcb_id anyway.
*/
static DB_STATUS
dm0p_eset(
i4	    lock_list,
u_i4   event_type,
i4	    cache_ix,
u_i4   event_object,
i4	    return_on_error)
{
    DMP_LBMCB		*lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON	*bm_common = lbm->lbm_bm_common;
    STATUS		status;
    i4		error;
    CL_SYS_ERR		sys_err;
    LK_EVENT		lk_event;

    lk_event.type_high = event_type | cache_ix;
    lk_event.type_low = event_object;
    lk_event.value = bm_common->bmcb_id;

    status = LKevent((LK_E_SET | LK_E_CROSS_PROCESS),
	lock_list, &lk_event, &sys_err);

    if (status == OK)
	return (E_DB_OK);

    if ((status == LK_INTERRUPT) && (return_on_error))
	return (E_DB_WARN);

    ule_format(status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &error, 0);
    ule_format(E_DM904B_BAD_LOCK_EVENT, &sys_err, ULE_LOG, NULL, 
	(char *)0, (i4)0, (i4 *)0, &error, 3,
	0, LK_E_SET, 0, event_type, 0, lock_list);

    if (event_type > DM0P_EVENT_SHIFT)
	event_type = (event_type / DM0P_EVENT_SHIFT) * DM0P_EVENT_SHIFT;
    switch(event_type)
    {
      case DM0P_WBFLUSH_EVENT:
	ule_format(E_DM9307_DM0P_WBEHIND_SIGNAL, (CL_SYS_ERR *)0, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, &error, 0);
	break;

      case DM0P_FWAIT_EVENT:
      case DM0P_BIO_EVENT:
      case DM0P_PGMUTEX_EVENT:
      case DM0P_BMCWAIT_EVENT:
      case DM0P_FCWAIT_EVENT:
      case DM0P_GWAIT_EVENT:
      default:
	break;
    }

    if (return_on_error)
	return (E_DB_ERROR);

    dmd_check(E_DMF015_EVENT_SET);

    /* dmd_check() should never return */
    return (E_DB_ERROR);
}

/*{
** Name: dm0p_ewait - wait for buffer manager event
**
** Description:
**	Call LK to wait for buffer manager event.  Uses cross-process
**	lock event calls.  Should be used only for events signaled
**	in the Buffer Manger routines.
**
**	Unless the return_on_error argument is specified, this will
**	dmd_check upon any LK error.
**
**	If the DM0P_WBFLUSH_EVENT event type is specified, then the LKevent
**	call will be interruptable.
**
**	If the LK call is interrupted (and return_on_error is specified),
**	E_DB_WARN is returned.
**
**	The LK_EVENT is prepared as:
**
**		type_high	DM0P_._EVENT in high-order
**				cache_ix in low-order, if qualified by cache.
**		type_low	buf_index, g_index, 0, depending on event type.
**		value		always bmcb_id.
**
** Inputs:
**	lock_list	- list to do wait on
**	event_type	- The event type.
**	cache_ix	- the cache qualifier
**	event_object	- The object which is being waited on.
**	return_on_error	- if not specified, routine will dmd_check on an
**			  unexpected event.
**
** Outputs:
**	None
**	Returns:
**	    E_DB_OK
**	    E_DB_WARN	- LKevent call interrupted
**	    E_DB_ERROR	- LKevent error
**	Exceptions:
**	    none
**
** Side Effects:
**	Will crash server if error and return_on_error not specified.
**
** History:
**	 6-feb-1989 (rogerk)
**	    Created for Terminator.
**       11-jan-1993 (mikem)
**          su4_us5 port.  Compiler complained about no return at end of
**          function (it doesn't understand that dmd_check() never returns.
**	25-mar-96 (nick)
**	     Added DM0P_FCWAIT_EVENT support.
**	05-Feb-1997 (jenjo02)
**	    Added DM0P_GWAIT_EVENT.
**	11-Jun-1998 (jenjo02)
**	    Added flags parm in which to pass LK_E_INTERRUPT,
**	    used by DM0P_WBFLUSH_EVENTs.
**	20-Jul-1998 (jenjo02)
**	    Modified prototype to include cache_ix and event_object, 
**	    and exclude event value which was always bm_common->bmcb_id anyway.
*/
static DB_STATUS
dm0p_ewait(
i4	    lock_list,
u_i4   event_type,
i4	    cache_ix,
u_i4   event_object,
i4	    return_on_error,
i4	    flags)
{
    DMP_LBMCB		*lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON	*bm_common = lbm->lbm_bm_common;
    i4		event_flag;
    i4		error;
    STATUS		status;
    CL_SYS_ERR		sys_err;
    LK_EVENT		lk_event;

    lk_event.type_high = event_type | cache_ix;
    lk_event.type_low = event_object;
    lk_event.value = bm_common->bmcb_id;

    event_flag = (flags | LK_E_WAIT | LK_E_CROSS_PROCESS);

    status = LKevent(event_flag, lock_list, &lk_event, &sys_err);

    if (status == OK)
	return (E_DB_OK);

    if ((status == LK_INTERRUPT) && (return_on_error))
	return (E_DB_WARN);

    ule_format(status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &error, 0);
    ule_format(E_DM904B_BAD_LOCK_EVENT, &sys_err, ULE_LOG, NULL, 
	(char *)0, (i4)0, (i4 *)0, &error, 3,
	0, LK_E_WAIT, 0, event_type, 0, lock_list);

    if (event_type > DM0P_EVENT_SHIFT)
	event_type = (event_type / DM0P_EVENT_SHIFT) * DM0P_EVENT_SHIFT;
    switch(event_type)
    {
      case DM0P_WBFLUSH_EVENT:
	ule_format(E_DM9307_DM0P_WBEHIND_SIGNAL, (CL_SYS_ERR *)0, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, &error, 0);
	break;

      case DM0P_FWAIT_EVENT:
      case DM0P_BIO_EVENT:
      case DM0P_PGMUTEX_EVENT:
      case DM0P_BMCWAIT_EVENT:
      case DM0P_FCWAIT_EVENT:
      case DM0P_GWAIT_EVENT:
      default:
	break;
    }

    if (return_on_error)
	return (E_DB_ERROR);

    dmd_check(E_DMF007_EVENT_WAIT);

    /* dmd_check() should never return */
    return (E_DB_ERROR);
}

/*{
** Name: dm0p_erelease - signal buffer manager event
**
** Description:
**	Call LK to wake up event waiters.  Uses cross-process
**	lock event calls.  Should be used only for events signaled
**	in the Buffer Manger routines.
**
**	Unless the return_on_error argument is specified, this will
**	dmd_check upon any LK error.
**
**	If the LK call is interrupted (and return_on_error is specified),
**	E_DB_WARN is returned.
**
**	The LK_EVENT is prepared as:
**
**		type_high	DM0P_._EVENT in high-order
**				cache_ix in low-order, if qualified by cache.
**		type_low	buf_index, g_index, 0, depending on event type.
**		value		always bmcb_id.
**
** Inputs:
**	lock_list	- list to do wait on
**	event_type	- The event type.
**	cache_ix	- the cache qualifier
**	event_object	- The object which is being waited on.
**	return_on_error	- if not specified, routine will dmd_check on an
**			  unexpected event.
**
** Outputs:
**	None
**	Returns:
**	    E_DB_OK
**	    E_DB_WARN	- LKevent call interrupted
**	    E_DB_ERROR	- LKevent error
**	Exceptions:
**	    none
**
** Side Effects:
**	Will crash server if error and return_on_error not specified.
**
** History:
**	 6-feb-1989 (rogerk)
**	    Created for Terminator.
**       11-jan-1993 (mikem)
**          su4_us5 port.  Compiler complained about no return at end of
**          function (it doesn't understand that dmd_check() never returns.
**	25-mar-96 (nick)
**	    Added DM0P_FCWAIT_EVENT support.
**	05-Feb-1997 (jenjo02)
**	    Added DM0P_GWAIT_EVENT.
**	20-Jul-1998 (jenjo02)
**	    Modified prototype to include cache_ix and event_object, 
**	    and exclude event value which was always bm_common->bmcb_id anyway.
*/
static DB_STATUS
dm0p_erelease(
i4	    lock_list,
u_i4   event_type,
i4	    cache_ix,
u_i4   event_object,
i4	    return_on_error)
{
    DMP_LBMCB		*lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON	*bm_common = lbm->lbm_bm_common;
    STATUS		status;
    i4		error;
    CL_SYS_ERR		sys_err;
    LK_EVENT		lk_event;

    lk_event.type_high = event_type | cache_ix;
    lk_event.type_low = event_object;
    lk_event.value = bm_common->bmcb_id;

    status = LKevent((LK_E_CLR | LK_E_CROSS_PROCESS),
	lock_list, &lk_event, &sys_err);

    if (status == OK)
	return (E_DB_OK);

    if ((status == LK_INTERRUPT) && (return_on_error))
	return (E_DB_WARN);

    ule_format(status, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &error, 0);
    ule_format(E_DM904B_BAD_LOCK_EVENT, &sys_err, ULE_LOG, NULL, 
	(char *)0, (i4)0, (i4 *)0, &error, 3,
	0, LK_E_CLR, 0, event_type, 0, lock_list);

    if (event_type > DM0P_EVENT_SHIFT)
	event_type = (event_type / DM0P_EVENT_SHIFT) * DM0P_EVENT_SHIFT;
    switch(event_type)
    {
      case DM0P_WBFLUSH_EVENT:
	ule_format(E_DM9307_DM0P_WBEHIND_SIGNAL, (CL_SYS_ERR *)0, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, &error, 0);
	break;

      case DM0P_FWAIT_EVENT:
      case DM0P_BIO_EVENT:
      case DM0P_PGMUTEX_EVENT:
      case DM0P_BMCWAIT_EVENT:
      case DM0P_FCWAIT_EVENT:
      case DM0P_GWAIT_EVENT:
      default:
	break;
    }

    if (return_on_error)
	return (E_DB_ERROR);

    dmd_check(E_DMF006_EVENT_RELEASE);
    /* dmd_check() should never return */
    return (E_DB_ERROR);
}

/*{
** Name: dm0p_reclaim - Toss some buffers to reclaim lock resources
**
** Description:
**	This routine is called when the buffer manager cannot allocate
**	a cache lock.  It tries to throw out buffers that have locks
**	in order to reclaim lock resources.  It only throws out one
**	lock resource.
**
**	It may reclaim a lock from the buffer pool or from the database or
**	table cache pool.
**
**	If no buffers can be found to reclaim, a warning is returned.
**
** Inputs:
**	lock_list			The lock list to use.
**	log_id				The log id to use.
**	none
**
** Outputs:
**	None
**	Returns:
**	    E_DB_OK
**	    E_DB_WARN	- No locks to reclaim
**	    E_DB_ERROR
**	Exceptions:
**	    none
**
** Side Effects:
**
** History:
**	 6-feb-1989 (rogerk)
**	    Created for Terminator.
**	 2-oct-1989 (rogerk)
**	    Make sure buffer is valid when attempting to reclaim it.
**      11-sep-1990 (jonb)
**	    Integrate ingres6301 change 280136 (rogerk):
**            Pass buf_tcb_stamp value to dm2t_get_tcb_ptr.
**	26-oct-1992 (rogerk)
**	    Reduced logging project: changed references to tcb fields to
**	    use new tcb_table_io structure fields.  Changed from old
**	    dm2t_get_tcb_ptr to new fix_tableio_ptr.
**	23-aug-1993 (andys)
**	    Fix bracketing logic in dm0p_reclaim such that it has a chance
**	    of working. 
**	18-oct-1993 (rmuth)
**	    Add DMP_BMCB statistics.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	14-Mar-1997 (jenjo02)
**	    Added lg_id to protocol so we can pass the transaction's
**	    logid to force_page().
*/
static DB_STATUS
dm0p_reclaim(
i4	    lock_list,
i4	    log_id)
{
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BMCB       *bm;
    DMP_TABLE_IO    *tbio = NULL;
    DM0P_BUFFER	    *b;
    DM0P_BLQ_HEAD   *tab_bucket;
    DM0P_BID	    next_bid;
    DB_STATUS	    status;
    i4		    reclaim = 0;
    i4	    error;
    i4	    i;

    lbm->lbm_lockreclaim++;
    bm_common->bmcb_lockreclaim++;

    /*
    ** Scan buffer manager for victim to free up a lock resource.
    ** Find a non-busy, non-fixed buffer that has a cache lock on it
    ** and that we have an open TCB for.
    **
    ** Use the table hash queues to find BUF_VALID pages in all caches
    ** instead of sequentially looking at every buffer in every cache.
    */
    for (i = 0; i < BMCB_TABLE_MAX && reclaim == 0; i++)
    {
	tab_bucket = &bm_common->bmcb_tabq_buckets[i];

	next_bid = tab_bucket->blqh_bid;

	while (next_bid.bid_index != BUF_ENDLIST && reclaim == 0)
	{
	    b = &lbm->lbm_buffers[next_bid.bid_cache][next_bid.bid_index];

	    if ((b->buf_sts & BUF_VALID) == 0 ||
		 b->buf_sts & (BUF_BUSY | BUF_FIXED) ||
		(b->buf_sts & BUF_LOCKED) == 0 ||
		 b->buf_tabq_bucket != i)
	    {
		if (b->buf_tabq_bucket == i)
		    next_bid = b->buf_tabq.blq_next;
		else
		{
		    /* Buffer's no longer in this hash chain */
		    /* Restart search of this chain */
		    next_bid = tab_bucket->blqh_bid;
		}
		continue;
	    }

	    /* Lock the buffer, then check again */
	    dm0p_mlock(SEM_EXCL, &b->buf_mutex);

	    /* Extract next pointers for table hash chain */
	    next_bid = b->buf_tabq.blq_next;

	    bm = lbm->lbm_bmcb[b->buf_id.bid_cache];

	    if ((b->buf_sts & BUF_VALID) &&
		((b->buf_sts & (BUF_BUSY | BUF_FIXED)) == 0) &&
		(b->buf_sts & BUF_LOCKED) &&
		 b->buf_tabq_bucket == i)
	    {
		/*
		** Found a buffer that is locked, and not busy.  Check if we have
		** a TCB for this table.  If we do, then we can toss the page.
		** If the tcb is busy, then wait for it to be free and retry.
		*/
		status = fix_tableio_ptr(b, 
		    (i4)(DM0P_TBIO_WAIT | DM0P_TBIO_MUTEX_HELD), lock_list,
		    log_id, &tbio, &error);

		if (DB_FAILURE_MACRO(status))
		{
		    /*
		    ** Log any errors, but continue as though we did not find a
		    ** table descriptor.
		    */
		    ule_format(error, (CL_SYS_ERR *)0, ULE_LOG, NULL, 
			(char *)0, (i4)0, (i4 *)0, &error, 0);
		}
		if (status == E_DB_WARN)
		{
		    /*
		    ** Table Descriptor was busy, go back and retry with the
		    ** same buffer.
		    ** buffer's mutex unlocked by fix_tableio_ptr().
		    */
		    next_bid = b->buf_id;
		    continue;
		}

		if (tbio == NULL)
		{
		    dm0p_munlock(&b->buf_mutex);
		    continue;
		}

		/*
		** Call force_page to toss the buffer, release the locks ....
		** This should not return with a warning since we verified above
		** that the buffer is not busy.
		*/

		status = force_page(b, tbio, FP_TOSS,
		    lock_list, log_id, BMCL_NO_GATHER, (i4*)0, &error);

		/*
		** Release the table io control block.
		*/
		unfix_tableio_ptr(&tbio, lock_list, log_id, (i4)__LINE__);

		/*
		** Check completion status from the force page call.
		*/
		if (status != E_DB_OK)
		{
		    dm0p_munlock(&b->buf_mutex);

		    ule_format(error, (CL_SYS_ERR *)0, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, &error, 0);
		    ule_format(E_DM9309_DM0P_RECLAIM, (CL_SYS_ERR *)0, ULE_LOG, 
			NULL, (char *)0, (i4)0, (i4 *)0, &error, 0);
		    return (E_DB_ERROR);
		}
		reclaim++;
		break;
	    }
	    if (b->buf_tabq_bucket != i)
	    {
		/* Buffer's no longer in this hash chain */
		/* Restart search of this chain */
		next_bid = tab_bucket->blqh_bid;
	    }
	    dm0p_munlock(&b->buf_mutex);
	}
    }

    /*
    ** If a buffer was found and reclaimed, then return OK, otherwise
    ** return a warning.
    */
    if (reclaim)
	return (E_DB_OK);
    return (E_DB_WARN);
}

/*{
** Name: dm0p_pass_abort - Inform BM of transaction PASS_ABORT status.
**
** Description:
**	This routine is called when the server fails for some reason in
**	abort processing and must signal the RCP to do the abort processing.
**
**	If the server does not keep cache locks on the database being
**	recovered (sole_server or sole_cache), then it must take care not
**	to bring into the cache any pages that might be invalidated by
**	the RCP during abort processing.
**
**	Before signaling the RCP to do recovery, the server will toss all
**	cached pages for this database from the cache.  New pages that are
**	faulted in after this are assumed to be OK.
**
**	In most cases, new pages can be brought into the cache without risk,
**	due to page locking algorithms.  Since all pages that the RCP will
**	update must be locked by the transaction (exeption is system catalog
**	pages - and these have cache locks), then normally any thread wishing
**	to reference one of these pages will have to wait until abort processing
**	is complete to read the new page.  The execption is read-nolock and
**	group faults, which read in pages without locks.
**
**	When this routine is called, it will mark the buffer manager status so
**	that group faults will be disallowed, and pages read in with read-nolock
**	will be immediately tossed from the cache.
**
**	Places in the buffer manager which need to check this PASS_ABORT status
**	should do so with the dm0p_recover_state routine.  This routine will
**	be responsible for checking the PASS_ABORT status and checking the
**	installation's recovery progress to determine when to turn off the
**	PASS_ABORT status.
**
**	This routine is called with two modes.  If 'recovery_started' is
**	not set, then a status will be set which indicates that the routine
**	dm0p_recover_state should not reset PASS_ABORT state yet, as the
**	installation's recover status will not yet be set.  This routine
**	should be called again with 'recovery_started' set after the RCP
**	has been signaled to do this abort.  After this is done, then
**	dm0p_recover_state should reset PASS_ABORT status when it sees that
**	the RCP is not doing any work.
**
** Inputs:
**	recovery_started    - indicates whether the RCP has been signaled to
**			      start recovery processing.
**
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	none
**
** History:
**	25-sep-1989 (rogerk)
**	    Created for Terminator.
*/
VOID
dm0p_pass_abort(i4	    recovery_started)
{
    DMP_LBMCB	        *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON      *bm_common = lbm->lbm_bm_common;

    /* Lock buffer manager status */
    dm0p_mlock(SEM_EXCL, &bm_common->bmcb_status_mutex);

    if (recovery_started == 0)
	bm_common->bmcb_status |= (BMCB_PASS_ABORT | BMCB_PREPASS_ABORT);
    else
	bm_common->bmcb_status &= ~BMCB_PREPASS_ABORT;

    dm0p_munlock(&bm_common->bmcb_status_mutex);
}

/*{
** Name: dm0p_recover_state - Check server status for PASS_ABORT.
**
** Description:
**	This routine by the group fault code and readnolock code.  It checks
**	to see whether the server is in a state where non-locked pages should
**	not be brought into the buffer manager.
**
**	When the server signals the RCP to abort a transaction for it, it must
**	set a state in the buffer manager which indicates that non-locked pages
**	must not be read into the cache - they risk being invalidated by the
**	RCP.  This status is the PASS_ABORT status.
**
**	This routine needs only be called if the database is not using cache
**	locking to protect against updates by other processes.
**
**	If the server is in PASS_ABORT state, and the RCP has already been
**	signaled to do the abort (indicated by the absense of the PREPASS_ABORT
**	status), then this routine will check the recovery system (using
**	LGevent) to see if the RCP is finished with recovery processing.  If
**	there is no recovery going on, then this routine will turn off the
**	PASS_ABORT status.
**
**	NOTE: This routine assumes the caller holds the buffer manager mutex.
**
** Inputs:
**	lg_id		- Logging system identifier.
**
** Outputs:
**	none
**	Returns:
**	    TRUE	- Server is in PASS_ABORT state.  No non-locked pages
**			  should be brought into the buffer manager.
**	    FALSE	- Server is not in PASS_ABORT state.
**
**	Exceptions:
**	    none
**
** Side Effects:
**	If server is in PASS_ABORT state, but the RCP is not doing recovery, 
**	then this routine will turn off the PASS_ABORT status.
**
** History:
**	25-sep-1989 (rogerk)
**	    Created for Terminator.
**	18-jan-1993 (bryanp)
**	    if the passed-in lg_id is 0, then we cannot call LG to check the
**	    system state, so we just assume that the pass-abort is still
**	    underway. This occurs, for example, when rcb_log_id is 0 in the
**	    rcb passed to dm0p_fix_page, as is the case for internal RCB's used
**	    by dm2t to update system catalogs during table close processing.
**	    Improve error message logging.
*/
bool
dm0p_recover_state(i4	    lg_id)
{
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    CL_SYS_ERR	    sys_err;
    i4		    mask;
    i4		    events;
    STATUS	    status;
    i4	    err_code;

    if ((bm_common->bmcb_status & BMCB_PASS_ABORT) == 0)
	return (FALSE);

    /*
    ** If we are in PASS_ABORT state, check the recovery system to see if
    ** recovery is complete.
    **
    ** Don't do this while in PREPASS_ABORT state, as the RCP may not yet
    ** have been told to do the abort, and we may turn off PASS_ABORT state
    ** prematurely.
    **
    ** Don't bother trying to call the logging system if we don't have an lg_id.
    */
    if ((bm_common->bmcb_status & BMCB_PREPASS_ABORT) == 0 && lg_id != 0)
    {
	mask = LG_E_RCPRECOVER | LG_E_RECOVER;
	status = LGevent(LG_NOWAIT, lg_id, mask, &events, &sys_err);
	if (status != OK)
	{
	    ule_format(status, (CL_SYS_ERR *)&sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &err_code, 0);
	}
	if ((status == OK) && ((events & mask) == 0))
	{
	    bm_common->bmcb_status &= ~BMCB_PASS_ABORT;
	}
    }

    return ((bm_common->bmcb_status & BMCB_PASS_ABORT) ? TRUE : FALSE);
}

/*{
** Name: dm0p_set_valid_stamp - Set TCB validation values on cached buffers.
**
** Description:
**	This routine is called when a new TCB is built.
**	It scans through the buffer manager looking for cached pages for
**	that TCB and updates their valid_stamp to show that they belong
**	to the just-built TCB.
**
**	The valid_stamp is used to recognize invalid TCB's when a table
**	is modified.  It prevents old TCB's from writing out pages that
**	belong to a new version of the table.
**
**	This routine must be called AFTER verifying that the cached
**	pages are all valid.  This is done through the dm0p_tblcache routine.
**
**	The caller of this routine must keep a table control lock on
**	the table between building the tcb and calling this routine to
**	ensure that the table cannot be modified (and the TCB become
**	invalid) before updating the buffer tcb_valid stamps.
**
**	The valid_stamp is updated for secondary index pages as well
**	as for base table pages.
**
** Inputs:
**	db_id		    - database id of table
**	table_id	    - base table id
**      page_size           - page size
**	validation_stamp    - the tcb validation stamp
**
** Outputs:
**	none
**	Returns:
**	    none
**
**	Exceptions:
**	    none
**
** History:
**	19-aug-1990 (rogerk)
**	    Created for Terminator.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	07-May-1998 (jenjo02)
**	    Modified to search Table's hash queue for buffers instead
**	    of searching entire cache.
*/
VOID
dm0p_set_valid_stamp(
i4	    db_id,
i4	    table_id,
i4     page_size,
i4	    validation_stamp)
{
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BUFFER	    *b;
    DM0P_BLQ_HEAD   *tab_bucket;
    DM0P_BID	    next_bid;
    i4	    bucket;
    
    /* Compute the table's hash bucket */
    bucket = (db_id + (u_i4)table_id) & (BMCB_TABLE_MAX - 1);
    tab_bucket = &bm_common->bmcb_tabq_buckets[bucket];

    /*
    ** Scan the table bucket chain looking for any buffers belonging
    ** to this table. 
    */
    next_bid = tab_bucket->blqh_bid;

    while (next_bid.bid_index != BUF_ENDLIST)
    {
	b = &lbm->lbm_buffers[next_bid.bid_cache][next_bid.bid_index];

	if ((b->buf_sts & BUF_VALID) == 0 ||
	     b->buf_dbid != db_id ||
	     b->buf_tabid.db_tab_base != table_id ||
	     b->buf_tabq_bucket != bucket)
	{
	    if (b->buf_tabq_bucket == bucket)
		next_bid = b->buf_tabq.blq_next;
	    else
		next_bid = tab_bucket->blqh_bid;
	    continue;
	}

	/*
	** Lock the buffer, then check it again.
	*/
	dm0p_mlock(SEM_EXCL, &b->buf_mutex);

	/* Extract the next pointers for the table's hash */
	next_bid = b->buf_tabq.blq_next;

	/*
	** If buffer belongs to the table, update its tcb stamp.
	*/
	if (b->buf_sts & BUF_VALID &&
	     b->buf_dbid == db_id &&
	     b->buf_tabid.db_tab_base == table_id)
	{
	    b->buf_tcb_stamp = validation_stamp;
	}
	else
	{
	    if (b->buf_tabq_bucket != bucket)
		next_bid = tab_bucket->blqh_bid;
	}

	dm0p_munlock(&b->buf_mutex);
    }
}

/*
** Name: dbg_dm9301		- Debugging support routine for E_DM9301.
**
** Description:
**	Before crashing the server with the fatal error
**	E_DM9301_DM0P_FORCE_PAGES, this routine is called. It attempts to
**	print out as much auxiliary information as possible before the
**	server crashes.
**
** Inputs:
**	b		Buffer which triggered the fatal error
**	t		Table Control Block (may be 0)
**	action		Action flags passed to dm0p_force_pages
**	tran_id		Transaction ID
**	lock_list	Lock List identifier
**	log_id		Logging system transaction ID
**	tran_bucket	Which transaction bucket this buffer was in.
**	tran_chain	Current pointer in the transaction queue linked list
**	num_bufs	Number of buffers processed by dm0p_force_pages
**	num_forces	Number of force_page calls issued
**	num_group_forces Number of gforce_page calls issued
**	num_waits	Number of times a TCB was waited for.
**	num_other_bufs	Number of buffers on this list for other xacts
**	num_force_warns	Number of times E_DB_WARN came back from force_page
**	num_removes	Number of buffers successfully processed
**	sts		Status of last call to dm2t_get_tcb_ptr().
**
** Outputs:
**	None
**
** Returns:
**	VOID
**
** History:
**	23-jul-1991 (bryanp)
**	    Created as part of working on Bug 33545.
**	19-feb-1992 (bryanp)
**	    Add TCB_PEND_DESTROY to tcb_status.
**	26-oct-1992 (rogerk)
**	    Reduced logging project: changed references to tcb fields to
**	    use new tcb_table_io structure fields.
**	26-jul-1993 (bryanp)
**	    Replace all uses of DM_LKID with LK_LKID.
**	26-jul-1993 (rogerk)
**	    Fix dbg_dm9301 routine to not try to test the TCB cache lock if
**	    sole_server mode.  In this case there are no cache locks.
**	22-feb-1994 (andys)
**	    Update buf_sts diagnostics to include new flags.
**	20-jun-1994 (jnash)
**	    Eliminate obsolete "logical_logging" TRdisplay.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	25-mar-96 (nick)
**	    Add FCWAIT and TPRIO to buf_sts output.
**      07-Dec-1998 (stial01)
**          Added kperleaf to distinguish from kperpage (keys per index page)
**      12-Apr-1999 (stial01)
**          dbg_dm9301() Distinguish leaf/index info for BTREE/RTREE indexes
**	14-Jan-2004 (jenjo02)
**	    Check db_tab_index > 0 for index, not simply != 0.
**	26-Jan-2004 (jenjo02)
**	    Check Master's index list.
**	09-Mar-2007 (jonj)
**	    Cleanup some TRdisplay stuff that was wrong.
*/
static VOID
dbg_dm9301(
DM0P_BUFFER	    *b,
DMP_TABLE_IO	    *tbio,
i4		    action,
DB_TRAN_ID          *tran_id,
i4		    lock_list,
i4		    log_id,
DM0P_BLQ_HEAD       *tran_bucket,
DM0P_BID	    *tran_chainp,
i4		    num_bufs,
i4		    num_forces,
i4		    num_group_forces,
i4		    num_waits,
i4		    num_other_bufs,
i4		    num_force_warns,
i4		    num_removes,
DB_STATUS	    sts)
{
    DM0P_BID        tran_chain = *tran_chainp;
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    i4              cache_ix;
    DM0P_BMCB       *bm;
    DM0P_BUFFER     *buf_array;
    DM0P_BUFFER	    *tmp_b;
    DMP_HCB	    *hcb = dmf_svcb->svcb_hcb_ptr;
    DMP_TCB	    *tcb = NULL;
    char	    *path;
    i4		    l_path;
    STATUS	    status;
    LOCATION	    loc;
    i4	    err_code;
    CL_ERR_DESC	    sys_err;
    i4		    i;
    i4	    buf_count;
    DM0P_BID        *next_trbuf;
    DM0P_GROUP	    *g;
    i4              ksize;

    /* Open II_CONFIG!dm9301.log */
    if ((status = NMloc(FILES, FILENAME, "dm9301.log", &loc)) != OK)
    {
	ule_format(status, (CL_ERR_DESC *)0, ULE_LOG , NULL, 
		    (char * )0, 0L, (i4 *)0, 
		    &err_code, 0);
	return;
    }

    LOtos(&loc, &path);
    l_path = STlength(path);

    if ((status = TRset_file(TR_F_OPEN, path, l_path, &sys_err)) != OK)
    {
	ule_format(status, (CL_ERR_DESC *)&sys_err, ULE_LOG , NULL, 
		    (char * )0, 0L, (i4 *)0, 
		    &err_code, 0);
	return;
    }

    ule_format(E_DM9301_DM0P_FORCE_PAGES, (CL_ERR_DESC *)0, ULE_LOG , NULL,
		    (char * )0, 0L, (i4 *)0, 
		    &err_code, 0);

    if (tbio)
	tcb = (DMP_TCB *)((char *)tbio - CL_OFFSETOF(DMP_TCB, tcb_table_io));

    if (b->buf_sts & BUF_FIXED)
	TRdisplay("Crashing because this buffer is still fixed.\n");
    else
	TRdisplay("Crashing due to null TBIO (%p)\n", tbio);


    TRdisplay("dm0p_force_pages(%x, %x->%x.%x, %x, %x, err_code)\n",
		lock_list, tran_id, tran_id->db_high_tran, tran_id->db_low_tran,
		log_id, action);

    TRdisplay("num_bufs=%d num_forces=%d num_group_forces=%d num_waits=%d\n",
	    num_bufs, num_forces, num_group_forces, num_waits);
    TRdisplay("num_other_bufs=%d num_force_warns=%d num_removes=%d\n",
	    num_other_bufs, num_force_warns, num_removes);
    TRdisplay("tran_bucket=%p (*bucket=%d), tran_chain=%d, sts=%x\n",
	    tran_bucket, *tran_bucket, tran_chain, sts);

    TRdisplay("The buffer we were working with:\n");
    bm = lbm->lbm_bmcb[b->buf_id.bid_cache];
    TRdisplay("    BUF: 0x%p (%dK,%d) OWNER: %d DB: 0x%x TAB: (%d,%d) PG: %d\n",
		b, KSIZE(bm->bm_pgsize), b->buf_id.bid_index,
		b->buf_owner, b->buf_dbid,
		b->buf_tabid.db_tab_base, b->buf_tabid.db_tab_index,
		b->buf_lock_key.lk_key4);
    TRdisplay("%8* REF=%2d CPCNT=%4d PRI=%2d TYPE=%w HASHB=%3d\n",
		b->buf_refcount, b->buf_cpcount, b->buf_priority,
		"FHDR,FMAP,ROOT,INDEX,LEAF,DATA", b->buf_type, b->buf_hash_bucket);
    TRdisplay("%8* TRANB=%2d TRAN=0x%x%x LKID=%x LKMODE %d V=%x%x PID=%x\n",
		b->buf_txnq_bucket,
		b->buf_tran_id.db_high_tran, b->buf_tran_id.db_low_tran,
		b->buf_lk_id, b->buf_lock_mode, b->buf_oldvalue.lk_high, b->buf_oldvalue.lk_low,
		b->buf_pid);
    TRdisplay("%8* STAMP=%x STATUS=%x %v\n",
		b->buf_tcb_stamp, b->buf_sts,
		"VALID,MODIFIED,FIXED,WRITE,LOCKED,CACHED,IOERROR,RECLAIM,\
RBUSY,WBUSY,WUNFIXRQ,INVALID,MUTEX,MWAIT,IOWAIT,WBBUSY,WCOMMIT,FCBUSY,\
TEMPTABLE,NOTXLOCK,WUNFIX,PAWRITE,PRIOFIX,TUNFIX,MUTEX_OK,TPRIO,FCWAIT,\
INTENT_LK,NULL_TUNFIX,DMCM,GATHER_QUEUE,MLOCK",
		b->buf_sts);

    if (tcb == 0)
    {
	/* there's not a TCB... why? */
	DMP_HASH_ENTRY      *h;
	DM_MUTEX	*hm;
	DMP_TCB		*it;
	DMP_TCB		*found_tcb;
	DMP_TCB		*t_tmp;
	i4		h_ix, hm_ix;

	TRdisplay("There's not a TCB ... why? ... looking ...\n");

	do
	{
	    /*
	    ** Get hash bucket in which this TCB should be kept.
	    */
	    hm_ix = HCB_MUTEX_HASH_MACRO(b->buf_dbid,b->buf_tabid.db_tab_base);
	    hm = &hcb->hcb_mutex_array[hm_ix];
	    h_ix = HCB_HASH_MACRO(b->buf_dbid,b->buf_tabid.db_tab_base,b->buf_tabid.db_tab_index);
	    h = &hcb->hcb_hash_array[h_ix];

	    /* Lock the tcb hash queue semaphore */
	    dm0s_mlock(hm);

	    /*
	    ** Search TCB chain off of this bucket for the base table
	    ** specified.  If we are actually looking for a secondary index
	    ** TCB, then we search for the base table and find the index TCB
	    ** from the base TCB.
	    **
	    ** If we find the tcb but it is busy, and the caller has specified
	    ** to wait for it, then wait for it to be released and research the
	    ** hash queue to see if it is still there.  Otherwise return with a
	    ** warning.
	    */
	    found_tcb = 0;
	    for (t_tmp = h->hash_q_next; t_tmp != (DMP_TCB*) h;
		 t_tmp = t_tmp->tcb_q_next)
	    {
		if ((t_tmp->tcb_dcb_ptr->dcb_id == b->buf_dbid) &&
		    (t_tmp->tcb_rel.reltid.db_tab_base ==
						    b->buf_tabid.db_tab_base))
		{
		    found_tcb = t_tmp;
		    TRdisplay("Resetting TCB to be %p (value of found_tcb)\n",
				found_tcb);
		    tcb = found_tcb;
		    break;
		}
	    }

	    if (found_tcb && found_tcb->tcb_status & TCB_BUSY)
	    {
		TRdisplay("***Found TCB at %p, but it's busy (%x)\n",
			    found_tcb, found_tcb->tcb_status);
		break;
	    }
	    else if (found_tcb == 0)
	    {
		TRdisplay("***Could NOT find TCB!\n");
		TRdisplay("   HASH bucket searched was %x (%d)\n",
			h_ix, h_ix);
		TRdisplay("   Looking for DB=%x, table=(%d,0)\n",
		    b->buf_dbid, b->buf_tabid.db_tab_base);

		for (t_tmp = h->hash_q_next; t_tmp != (DMP_TCB*) h;
		     t_tmp = t_tmp->tcb_q_next)
		{
		    TRdisplay("   At %p, found DB=%x, table=(%d,%d)\n",
				t_tmp, t_tmp->tcb_dcb_ptr->dcb_id,
				t_tmp->tcb_rel.reltid.db_tab_base,
				t_tmp->tcb_rel.reltid.db_tab_index);
		}
		break;
	    }

	    /*
	    ** Verify that the tcb is valid.  It could be that this table
	    ** has been modified by another server and this tcb describes
	    ** the old state of the table.
	    **
	    ** Make sure that the validation stamp of the tcb we found
	    ** matches the level required by the caller.  If we have an
	    ** old level TCB, then just return that the requested TCB
	    ** does not exist.
	    **
	    ** Note: this check will usually be made even in cases when TCB
	    ** validation is not required for a server (when running 
	    ** SOLE_SERVER).  In these cases, the tbio_cache_valid_stamp 
	    ** should always be zero and should always pass this check.
	    **
	    ** If the caller has requested to bypass validation, then skip
	    ** the check.
	    */
	    if (found_tcb && 
		(found_tcb->tcb_table_io.tbio_cache_valid_stamp != 
							b->buf_tcb_stamp))
	    {
		TRdisplay("Found TCB at %p, but TCB stamp is %x, not %x\n",
		    found_tcb, found_tcb->tcb_table_io.tbio_cache_valid_stamp,
		    b->buf_tcb_stamp);
	    }

	} while (FALSE);

	/*
	** If have found base table and need secondary index, check 2nd
	** index chain off of tcb.
	*/
	if ((b->buf_tabid.db_tab_index > 0) && (found_tcb))
	{
	    TRdisplay("found TCB at %p, but need index table %d\n",
			found_tcb, b->buf_tabid.db_tab_index);
	    for (it = found_tcb->tcb_pmast_tcb->tcb_iq_next;
		it != (DMP_TCB *) &found_tcb->tcb_pmast_tcb->tcb_iq_next;
		it = it->tcb_q_next)
	    {
		TRdisplay("   Index TCB at %p is for table (%d,%d)\n",
			    it, it->tcb_rel.reltid.db_tab_base,
			    it->tcb_rel.reltid.db_tab_index);
		if (it->tcb_rel.reltid.db_tab_index ==
						b->buf_tabid.db_tab_index)
		{
		    TRdisplay("***This Index TCB is a winner!\n");
		    break;
		}
		else
		{
		    TRdisplay("BZZZT! Wanna play again?\n");
		}
	    }
	}

	dm0s_munlock(hm);
    }

    /* what is the lock value for this TCB? */
    if ((tcb) && (tcb->tcb_dcb_ptr->dcb_served == DCB_MULTIPLE))
    {
	LK_LKID          lockid;
	LK_VALUE         lockvalue;
	CL_SYS_ERR       sys_err;
	i4          local_error;
	STATUS	     stat;

	TRdisplay("DM2T_VERIFY_TCB (%~t,%~t)\n",
	    sizeof(tcb->tcb_dcb_ptr->dcb_name), &tcb->tcb_dcb_ptr->dcb_name,
	    sizeof(tcb->tcb_rel.relid), &tcb->tcb_rel.relid);

	/* Get the lock value see if it is the same. */

	STRUCT_ASSIGN_MACRO(tcb->tcb_lk_id, lockid);
	stat = LKrequest(LK_PHYSICAL | LK_CONVERT,
	    dmf_svcb->svcb_lock_list, 0, LK_N, (LK_VALUE * )&lockvalue,
	    &lockid, 0L, &sys_err); 
	if (stat != OK && stat != LK_VAL_NOTVALID)
	{
	    ule_format(stat,&sys_err,ULE_LOG,NULL,0,0,0,&local_error, 0);
	    ule_format(E_DM901C_BAD_LOCK_REQUEST, &sys_err, ULE_LOG, NULL,
		0, 0, 0, &local_error, 2, 0, LK_N, 0, dmf_svcb->svcb_lock_list);
	}
	if (stat == LK_VAL_NOTVALID)
	    TRdisplay("Got LK_VAL_NOTVALID from lockrequest!!!!!!!\n");

	if (tcb->tcb_table_io.tbio_cache_valid_stamp != lockvalue.lk_low)
	{
	    TRdisplay("TCB valid stamp is wrong -- it's %x, not %x\n",
		    tcb->tcb_table_io.tbio_cache_valid_stamp, lockvalue.lk_low);
	}
	if ((stat == OK) && 
	    (tcb->tcb_table_io.tbio_cache_valid_stamp == lockvalue.lk_low))
	    TRdisplay("TCB valid stamp is correctly set to %x\n",
			tcb->tcb_table_io.tbio_cache_valid_stamp);
    }

    if (tcb)
    {
	TRdisplay("    Ref_count: %d  %v %w %w %w\n",
	    tcb->tcb_ref_count,
            TCB_FLAG_MEANING, tcb->tcb_status,
	    "PERMANENT,TEMPORARY", tcb->tcb_temporary,
	    "USER,SYSTEM", tcb->tcb_sysrel,
	    ",LOAD", tcb->tcb_loadfile);
	TRdisplay("    Additional attributes: %w %w %w %w\n",
	    "NOLOGGING,", tcb->tcb_logging,
	    ",NOFILE", tcb->tcb_nofile,
	    ",SECINDEX", tcb->tcb_update_idx,
	    ",NOTIDS", tcb->tcb_no_tids);
	TRdisplay("    Mutex: %#.4{%x %}\n", (sizeof(DM_MUTEX) / 4),
	    &tcb->tcb_mutex, 0);
	TRdisplay("    INDEX  Next :0x%p  Previous: 0x%p Type: %9w  Count: %d\n",
	    tcb->tcb_iq_next, tcb->tcb_iq_prev, 
	    "NONUNIQUE, UNIQUE", tcb->tcb_index_type,
	    tcb->tcb_index_count);
	TRdisplay("    FREE     Next: 0x%p   Previous: 0x%p\n",
	    tcb->tcb_fq_next, tcb->tcb_fq_prev);
	TRdisplay("    ACTIVE   Next: 0x%p   Previous: 0x%p\n",
	    tcb->tcb_rq_next, tcb->tcb_rq_prev);
	TRdisplay("    Parent TCB: 0x%p FCB: 0x%p  DCB: 0x%p  Hash Bucket: 0x%p\n",
	    tcb->tcb_parent_tcb_ptr, 
	    tcb->tcb_table_io.tbio_location_array[0].loc_fcb, 
	    tcb->tcb_dcb_ptr, tcb->tcb_hash_bucket_ptr);
	TRdisplay("    INDEX Keys: %6.2{%5.2d%}\n", tcb->tcb_ikey_map, 0);
	TRdisplay("    Valid Stamp:0x%x   Tuple Adds: %d   Page Adds: %d\n",
	    tcb->tcb_table_io.tbio_cache_valid_stamp, 
	    tcb->tcb_tup_adds, tcb->tcb_page_adds);
	if (tcb->tcb_rel.relspec == TCB_BTREE || 
	    tcb->tcb_rel.relspec == TCB_RTREE)

	    TRdisplay("    KEY Count: %d   IXklen %d Leafklen: %d   PerIndex: %d PerLeaf\n",
		tcb->tcb_keys, 
		tcb->tcb_ixklen, 
		tcb->tcb_klen,
		tcb->tcb_kperpage, 
		tcb->tcb_kperleaf);
	else
	    TRdisplay("    KEY Count: %d   Length: %d   Perpage: %d\n",
		tcb->tcb_keys, tcb->tcb_klen, tcb->tcb_kperpage);
	TRdisplay("    LCT: 0x%p   Load Pageno: %d  Load Allocate: %d\n",
	    tcb->tcb_lct_ptr, tcb->tcb_table_io.tbio_lpageno, 
	    tcb->tcb_table_io.tbio_lalloc);
	TRdisplay("    Lockid: %x%x  C and CHAR attributes: %d (key %d)\n",
	    tcb->tcb_lk_id.lk_unique,
	    tcb->tcb_lk_id.lk_common, tcb->tcb_comp_atts_count,
	    tcb->tcb_comp_katts_count);
	TRdisplay("    Relation Information:\n");
	TRdisplay("        Name: (%~t,%~t)   Id: (%d,%d)\n",
	    sizeof(tcb->tcb_rel.relid), &tcb->tcb_rel.relid,
	    sizeof(tcb->tcb_rel.relowner), &tcb->tcb_rel.relowner, 
	    tcb->tcb_rel.reltid.db_tab_base, tcb->tcb_rel.reltid.db_tab_index);
	TRdisplay("        Attributes: %d   Width: %d   Structure: %w\n%8* Status: %v\n",
	    tcb->tcb_rel.relatts, tcb->tcb_rel.relwid,
	    ",,,HEAP,,ISAM,,HASH,,,,BTREE,,RTREE", tcb->tcb_rel.relspec,
	    "CATALOG,NOUPDATE,PROTUP,INTEG,CONCUR,VIEW,VBASE,INDEX,BINARY,\
COMPRESSED,INDEX_COMPRESSED,?,PROTALL,PROTECT,EXCATALOG,JON,UNIQUE,\
INDEXED,JOURNAL,NORECOVER,\
ZOPTSTAT,DUPLICATES,MULTI_LOC,GATEWAY,RULE,SYSTEM_MAINTAINED",
	    tcb->tcb_rel.relstat);
	TRdisplay("        Tuples: %d Pages Total: %d Primary: %d Main: %d\n",
	    tcb->tcb_rel.reltups, tcb->tcb_rel.relpages, tcb->tcb_rel.relprim,
	    tcb->tcb_rel.relmain);
	TRdisplay("        Location: %~t       Save Date: %d\n",
	    sizeof(tcb->tcb_rel.relloc), &tcb->tcb_rel.relloc, tcb->tcb_rel.relsave);
	TRdisplay("    Attribute Information\n");
	if ( ! (tcb->tcb_status & TCB_PARTIAL))
	{
TRdisplay("        NAME                    OFFSET  TYPE    LENGTH PREC KEY KOFFSET\n"); 
	TRdisplay("%#.#{%8* %.#s  %4.2d  %8.2w  %4.2d %4.2d %3.2d    %4.2d\n%}",
	    tcb->tcb_rel.relatts, 
	    sizeof(DB_ATTS), &tcb->tcb_atts_ptr[1], 
	    sizeof(((DB_ATTS *)0)->name),
	    &((DB_ATTS *)0)->name, &((DB_ATTS *)0)->offset, 
	    ",,,DATE,,MONEY,,,,,,,,,,,,,,,CHAR,VCHAR,,,,,,,,,INT,FLOAT,CHAR,,,,,TEXT",
	     &((DB_ATTS *)0)->type,
	    &((DB_ATTS *)0)->length, &((DB_ATTS *)0)->precision, 
	    &((DB_ATTS *)0)->key,
	    &((DB_ATTS *)0)->key_offset); 
	if (tcb->tcb_keys)
	{
	    TRdisplay("    Key Information\n");
	    TRdisplay("        NAME                    OFFSET  TYPE    LENGTH PREC KEY KOFFSET\n"); 
	    TRdisplay("%#[%8* %.#s  %4.2d  %8.2w  %4.2d %4.2d %3.2d    %4.2d\n%]",
		tcb->tcb_rel.relkeys, tcb->tcb_key_atts,
		sizeof(((DB_ATTS *)0)->name), &((DB_ATTS *)0)->name, &((DB_ATTS *)0)->offset, 
		",,,DATE,,MONEY,,,,,,,,,,,,,,,CHAR,VCHAR,,,,,,,,,INT,FLOAT,CHAR,,,,,TEXT",
		&((DB_ATTS *)0)->type,
		&((DB_ATTS *)0)->length, &((DB_ATTS *)0)->precision, &((DB_ATTS *)0)->key,
		&((DB_ATTS *)0)->key_offset); 
	}
	}
    }

    TRdisplay("    Transaction Queues%58*-\n");
    for (i = 0; i < BMCB_TRAN_MAX; i++)
    {
	buf_count = bm_common->bmcb_totbufcnt;
	next_trbuf = &bm_common->bmcb_txnq_buckets[i].blqh_bid;
	if (next_trbuf->bid_index != BUF_ENDLIST)
	    TRdisplay("%8* Tran Bucket[%d]:\n", i);
	while (next_trbuf->bid_index != BUF_ENDLIST)
	{ 
	    tmp_b = &lbm->lbm_buffers[next_trbuf->bid_cache][next_trbuf->bid_index];

	    TRdisplay ("%12* BUF(0x%p)DB(%d)TBL(%d,%d)PG(%d)PRI(%d)TYPE(%w)\n",
		tmp_b, tmp_b->buf_lock_key.lk_key1, tmp_b->buf_lock_key.lk_key2,
		tmp_b->buf_lock_key.lk_key3, tmp_b->buf_lock_key.lk_key4,
		tmp_b->buf_priority, "ROOT,INDEX,LEAF,DATA", tmp_b->buf_type);
	    if (--buf_count < 0)
	    {
		TRdisplay("%12* BUFFER QUEUE INFINITE?.\n");
		break;
	    }
	    next_trbuf = &tmp_b->buf_txnq.blq_next;
	}
    }

    for (cache_ix = 0, ksize = 2; cache_ix < DM_MAX_CACHE; cache_ix++, ksize *= 2)
    {
	bm = lbm->lbm_bmcb[cache_ix];
	if (!bm || !(bm->bm_status & BM_ALLOCATED))
	    continue;

	buf_array = lbm->lbm_buffers[bm->bm_cache_ix];
	TRdisplay("    Fixed Buffers (%2dK) %58*-\n", ksize);
	{
	    for (i = 0; i < bm->bm_bufcnt; i++)
	    {
		tmp_b = &buf_array[i];
		if (tmp_b->buf_sts & BUF_FIXED)
		{
		    TRdisplay (
			"%12* BUF(0x%p)DB(%d)TBL(%d,%d)PG(%d)PRI(%d)TYPE=%w\n",
			tmp_b, tmp_b->buf_lock_key.lk_key1, 
			tmp_b->buf_lock_key.lk_key2,
			tmp_b->buf_lock_key.lk_key3,
			tmp_b->buf_lock_key.lk_key4, tmp_b->buf_priority,
			"ROOT,INDEX,LEAF,DATA", tmp_b->buf_type);
		}
	    }
	}


	if (bm->bm_status & BM_GROUP)
	{
	    DM0P_GROUP     *group_array = lbm->lbm_groups[bm->bm_cache_ix];

	    TRdisplay("    FIXED Groups (%2dK) %52*-\n", ksize);

	    for (i = 0; i < bm->bm_gcnt; i++)
	    {
		g = &group_array[i];
		
		if (g->g_sts & G_FIXED)
		{
		    TRdisplay(
		    "%8* Gr: %d  Status: %v  Ref_cnt: %d  Mod_cnt: %d  Buffer: %d\n",
			g->g_index, "FIXED,MODIFIED,WBUSY,RBUSY,WAIT", g->g_sts, 
			g->g_refcount, g->g_modcount, g->g_buffer);
		}
	    }
	}
    }

    return;
}

/*{
** Name: dm0p_wpass_abort - write modified pages as part of passabort protocol.
**
** Description:
**	This routine scans through the buffer manager and tosses any pages
**	marked BUF_PAWRITE that belong to the specified database.
**
**	Pages marked BUF_PAWRITE are allowed to be kept in the cache modified
**	even though no transaction locks protect their consistency.
**
**	When a pass-abort is signaled (RCP will recover database while it
**	is in use) - these pages must be tossed from the cache so that the
**	RCP will see valid versions of the pages.
**
**	After being tossed, these pages may be reread, but buffer manager
**	protocols will insure that the pages are written each time they
**	are changed (ie not allowed to remain modified in the cache) while
**	the pass-abort is in progress.
**
**	This routine should always be called AFTER having set the server
**	pass-abort status (BMCB_PASS_ABORT), to make sure that pages which
**	are changed and/or unfixed after this routine has finished will
**	be forced to disk if modified as part of the PAWRITE protocol
**	(which states that a PAWRITE page must be written at unfix time
**	during a pass-abort).
**
** Inputs:
**	dbid		- database id - 0 for all databases
**	lock_list	- lock list to do LK waits on
**
** Outputs:
**	None
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	none
**
** History:
**	17-oct-1991 (rogerk)
**	    Created as part of the 6.4 -> 6.5 merge.  This routine was
**	    written to replace the changes made to dm0p_toss_pages by
**	    the file extend project.
**	21-jan-1992 (jnash)
**	    Modify Pass Abort protocol to set BUF_TUNFIX on pages
**	    passed over because they were fixed or busy.
**	26-oct-1992 (rogerk)
**	    Reduced logging project: changed references to tcb fields to
**	    use new tcb_table_io structure fields.  Changed from old
**	    dm2t_get_tcb_ptr to new fix_tableio_ptr.
**	18-oct-1993 (rmuth)
**	    Add DMP_BMCB statistics.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**      01-aug-1996 (nanpr01 for ICL itb)
**	    Added the gather write and async io amendments.
**	25-Aug-1997 (jenjo02)
**	    Added log_id to prototype, passing it to force_page().
**	10-Dec-1997 (jenjo02)
**	    Keep reusing the TBIO as long as possible instead of unfixing it
**	    after every force.
**	23-Jan-1998 (jenjo02)
**	    Rewrote to find pages using table hash queues instead of doing
**	    sequential scan of all caches.
**	12-Mar-2003 (jenjo02)
**	    Release buf_mutex before calling unfix_tableio_ptr(),
**	    retry buffer to avoid rare E_CL2517_CS_SMPR_DEADLOCK
**	    error in fault_page() (B109819).
*/
VOID
dm0p_wpass_abort(
i4	    dbid,
i4	    lock_list,
i4	    log_id)
{
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BMCB       *bm;
    DMP_TABLE_IO    *tbio = NULL;
    DB_STATUS	    status = E_DB_OK;
    DM0P_BUFFER	    *b;
    DMPP_PAGE	    *page;
    DM0P_BLQ_HEAD   *tab_bucket;
    DM0P_BID	    next_bid;
    i4	    error;
    i4		    i;
    i4	    	    gw_queued = 0;

    /*
    ** Scan the buffer manager looking for PAWRITE buffers belonging to this
    ** database.  When find such a page, toss it out of the cache.
    **
    ** Use the table hash queues to find BUF_VALID pages in all caches
    ** instead of sequentially looking at every buffer in every cache.
    */
    for (i = 0; i < BMCB_TABLE_MAX; i++)
    {
	tab_bucket = &bm_common->bmcb_tabq_buckets[i];

	next_bid = tab_bucket->blqh_bid;

	while (next_bid.bid_index != BUF_ENDLIST)
	{
	    b = &lbm->lbm_buffers[next_bid.bid_cache][next_bid.bid_index];

	    /*
	    ** If buffer does not belong to this database or is not marked
	    ** PAWRITE, then skip it.
	    */
	    if (((dbid != 0) && b->buf_dbid != dbid) ||
		(b->buf_sts & BUF_PAWRITE) == 0 ||
		b->buf_tabq_bucket != i)
	    {
		/*
		** If the buffer's no longer in this hash queue,
		** restart the queue.
		*/
		if (b->buf_tabq_bucket == i)
		    next_bid = b->buf_tabq.blq_next;
		else
		    next_bid = tab_bucket->blqh_bid;
		continue;
	    }

	    /* Lock the buffer, check again */
	    dm0p_mlock(SEM_EXCL, &b->buf_mutex);

	    /* Extract forward table chain pointers */
	    next_bid = b->buf_tabq.blq_next;

	    if (((dbid != 0) && (b->buf_dbid != dbid)) ||
		((b->buf_sts & BUF_PAWRITE) == 0) ||
		((b->buf_sts & BUF_VALID) == 0) ||
		b->buf_tabq_bucket != i)
	    {
		if (b->buf_tabq_bucket != i)
		    next_bid = tab_bucket->blqh_bid;
		dm0p_munlock(&b->buf_mutex);
		continue;
	    }

	    /*
	    ** If fixed or busy, then mark it toss on unfix
	    */
	    if (b->buf_sts & (BUF_FIXED | BUF_MUTEX))
	    {
		b->buf_sts |= BUF_TUNFIX;
		dm0p_munlock(&b->buf_mutex);
		continue;
	    }

	    bm = lbm->lbm_bmcb[b->buf_id.bid_cache];

	    /*
	    ** If the buffer is modified, then we need its TCB to toss it.
	    */
	    if (b->buf_sts & BUF_MODIFIED)
	    {
		if (!tbio ||
		    b->buf_dbid != tbio->tbio_dcbid ||
		    b->buf_tabid.db_tab_base != tbio->tbio_reltid.db_tab_base ||
		    b->buf_tabid.db_tab_index != tbio->tbio_reltid.db_tab_index)
		{
		    /* Wrong table, release buffer, TBIO, retry */
		    if ( tbio )
		    {
			next_bid = b->buf_id;
			dm0p_munlock(&b->buf_mutex);
			unfix_tableio_ptr(&tbio, lock_list, log_id, (i4)__LINE__);
			continue;
		    }

		    /* 
		    ** Look up the TCB for this buffer.
		    ** If we can't find a valid TCB, then don't try to toss it.
		    */
		    status = fix_tableio_ptr(b, 
			(i4)(DM0P_TBIO_WAIT | DM0P_TBIO_MUTEX_HELD), lock_list,
			log_id, &tbio, &error);

		    if (DB_FAILURE_MACRO(status))
		    {
			/*
			** Log any errors, but continue as though we did not find a
			** table descriptor.
			*/
			ule_format(error, (CL_SYS_ERR *)0, ULE_LOG, NULL, 
			    (char *)0, (i4)0, (i4 *)0, &error, 0);
		    }
		    if (status == E_DB_WARN)
		    {
			/*
			** Table Descriptor was busy, go back and retry with the
			** same buffer.
			** buffer's mutex unlocked by fix_tableio_ptr().
			*/
			next_bid = b->buf_id;
			continue;
		    }
		}
		else
		{
		    status = E_DB_OK;
		}


		/*
		** Consistency Check: FIXME FIXME FIXME XXXXXXXXXXXXX
		**
		** Who am I?  Why am I here?
		**
		** If we did not find a table descriptor for a buffer which is
		** modified then we are in trouble.  This situation should be
		** impossible in a single-server case, but with shared cache it
		** can occur.  If this buffer is needed by the RCP then recovery
		** is screwed.
		**
		** Also note error handling for force_page error below.  Page
		** is invalidated????? I think that if we lose pages in a fast
		** commit database that we have to bring the server down.
		*/
		if (status != E_DB_OK)
		{
		    dmd_check(E_DM9300_DM0P_CLOSE_PAGE);
		}
	    }

	    /*
	    ** Toss this page.
	    ** The TBIO pointer may be NULL if the page is not modified.
	    */
	    status = force_page(b, tbio, FP_TOSS,
		lock_list, log_id, BMCL_WPASS_ABORT, &gw_queued, &error);

	    /*
	    ** Check for retry needed.  Force_page will return a warning if the
	    ** buffer has an outstanding I/O on it (it is being forced out by
	    ** another thread).
	    */
	    if ( status )
	    {
		if (status == E_DB_WARN)
		    next_bid = b->buf_id;
		dm0p_munlock(&b->buf_mutex);
	    }
	}
    }

    /* Force any remaining queued GatherWrite buffers */
    if (gw_queued)
	(VOID) dm2f_force_list();

    /* If there's a TBIO left around, release it */
    if (tbio)
	unfix_tableio_ptr(&tbio, lock_list, log_id, (i4)__LINE__);


    return;
}

/*
** Name: dm0p_reparent_pages	- Move a set of pages from one table to another
**
** Description:
**	The new in-memory build facility of DM1X allows a new version of a
**	table to be constructed directly into buffer manager page frames,
**	rather than in a file on disk. This feature is used by in-memory
**	temporary tables.
**
**	The build is done using a "build TCB", which is yet another temporary
**	table which persists only for the duration of the build and exists
**	merely in order to provide an RCB for the build routines to use to
**	fix and unfix pages.
**
**	At the completion of the build, the page frames which have been built
**	containing the new table's pages must be "re-parented" into the
**	ownership of the desired table. After this is done, the build TCB
**	will be thrown away.
**
**	This routine takes all current pages in the buffer manager page
**	frames which are owned by "old_parent" and switches them to have the
**	ownership of "new_parent".
**
**	Currently, this routine only works if both old_parent and new_parent
**	are base tables (not secondary indices). This is sufficient for the
**	current requirements of temporary tables, which don't support secondary
**	indices.
**
** Inputs:
**	old_parent		    table which currently owns the pages
**	new_parent		    table which should be given the pages
**
** Outputs:
**	err_code		    Set appropriately if an error occurs
**
** Returns:
**	E_DB_OK
**	E_DB_ERROR
**
** History:
**	1-may-1992 (bryanp)
**	    Created for the in-memory builds project.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	15-Jan-1998 (jenjo02)
**	    Rewrote to find TCB's pages using the (new) table hash
**	    queue instead of a page-by-page search of every buffer 
**	    in the cache. The hash queue is a list of all pages
**	    belonging to a table (TCB), including index pages, in
**	    all caches.
**	08-Sep-1998 (shero03)
**	    Change the hash algorithm to mod by the hash count.
**	14-Jan-2004 (jenjo02)
**	    Check db_tab_index > 0 for index, not simply != 0.
*/
DB_STATUS
dm0p_reparent_pages(
DMP_TCB	    *old_parent,
DMP_TCB	    *new_parent,
i4	    *err_code)
{
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BMCB       *bm = lbm->lbm_bmcb[old_parent->tcb_table_io.tbio_cache_ix];
    DM0P_BUFFER     *buf_array = lbm->lbm_buffers[bm->bm_cache_ix];
    DM0P_HASH_HEAD  *bucket_array = lbm->lbm_buckets[bm->bm_cache_ix];
    DB_STATUS	    status = E_DB_OK;
    DM0P_BUFFER	    *b;
    i4	    base_id = old_parent->tcb_rel.reltid.db_tab_base;
    i4	    database_id = old_parent->tcb_dcb_ptr->dcb_id;
    i4		    i;
    i4	    hash_bucket;
    i4	    hash_chain;
    DM0P_BLQ_HEAD   *tab_bucket;
    DM0P_BID	    next_bid;

    if (old_parent->tcb_temporary != TCB_TEMPORARY ||
	new_parent->tcb_temporary != TCB_TEMPORARY ||
	old_parent->tcb_rel.relpgsize != new_parent->tcb_rel.relpgsize)
    {
	*err_code = E_DM002A_BAD_PARAMETER;
	return (E_DB_ERROR);
    }
    
    /* Compute the old parent table's hash bucket */
    i = (database_id + (u_i4)base_id) & (BMCB_TABLE_MAX - 1);
    tab_bucket = &bm_common->bmcb_tabq_buckets[i];

    /*
    ** Scan the table bucket chain looking for any buffers belonging
    ** to this table. 
    **
    ** Note that all pages come from the same cache (bm->bm_cache_ix).
    */
    next_bid = tab_bucket->blqh_bid;

    while (next_bid.bid_index != BUF_ENDLIST)
    {
	b = &lbm->lbm_buffers[next_bid.bid_cache][next_bid.bid_index];

	if (b->buf_dbid != database_id ||
	    b->buf_tabid.db_tab_base != base_id ||
	    b->buf_tabq_bucket != i)
	{
	    if (b->buf_tabq_bucket == i)
		next_bid = b->buf_tabq.blq_next;
	    else
		next_bid = tab_bucket->blqh_bid;
	    continue;
	}

	/*
	** Lock the buffer, then check it again.
	*/
	dm0p_mlock(SEM_EXCL, &b->buf_mutex);

	/* Extract the next pointers for the table's hash */
	next_bid = b->buf_tabq.blq_next;

	if (b->buf_dbid != database_id ||
	    b->buf_tabid.db_tab_base != base_id ||
	    b->buf_tabq_bucket != i)
	{
	    if (b->buf_tabq_bucket != i)
		next_bid = tab_bucket->blqh_bid;
	    dm0p_munlock(&b->buf_mutex);
	    continue;
	}

	/*
	** Consistency Check - the buffer should not be left fixed or in mutex
	** state by this server since the TCB is not currently in use.
	*/
	if ((b->buf_sts & (BUF_FIXED | BUF_MUTEX)) &&
	    ((b->buf_sts & BUF_MUTEX_OK) == 0))
	{
	    ule_format(E_DM9C01_FIXED_BUFFER_INFO, (CL_SYS_ERR *)0,
	    		ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 6,
			0, b->buf_lock_key.lk_key4,
			0, b->buf_tabid.db_tab_base,
			0, b->buf_tabid.db_tab_index,
			0, b->buf_sts,
			sizeof(old_parent->tcb_dcb_ptr->dcb_name),
				&old_parent->tcb_dcb_ptr->dcb_name,
			0, (BUF_FIXED | BUF_MUTEX) );
	    dmd_check(E_DM9300_DM0P_CLOSE_PAGE);
	}

	/*
	** temporary tables can't have secondary indexes.
	*/
	if (b->buf_tabid.db_tab_index > 0)
	{
	    *err_code = E_DM002A_BAD_PARAMETER;
	    status = E_DB_ERROR;
	    dm0p_munlock(&b->buf_mutex);
	    return(E_DB_ERROR);
	}

	/*
	** Now reparent the buffer. This requires:
	**	removing the buffer from its old hash queue.
	**	updating the b->buf_tabid field.
	**	updating the b->buf_lock_key field.
	**	moving the buffer to the correct hash queue.
	*/

	/* Remove buffer from parent table's hash queue */
	tabq_delete(b);

	b->buf_tabid.db_tab_base = new_parent->tcb_rel.reltid.db_tab_base;
	b->buf_tabid.db_tab_index = new_parent->tcb_rel.reltid.db_tab_index;
	b->buf_lock_key.lk_key2 = new_parent->tcb_rel.reltid.db_tab_base;
	b->buf_lock_key.lk_key3 = new_parent->tcb_rel.reltid.db_tab_index;

	/*
	** Compute the new hash bucket for this page.
	*/
        /*
         * Note that the key sum is moded by the hash count.
         * In the previous code it was anded, which works if the hash count
         * is 1 less than a power of 2, e.g. 255.
         */
	hash_bucket = (u_i4)(new_parent->tcb_dcb_ptr->dcb_id +
			          new_parent->tcb_rel.reltid.db_tab_base + 
			          new_parent->tcb_rel.reltid.db_tab_index +
			          b->buf_lock_key.lk_key4) % bm->bm_hshcnt;
	
	/*
	** Optimization: don't shuffle hash queues if we rehashed
	**	  	 to the same bucket.
	*/
	if (hash_bucket != b->buf_hash_bucket)
	{
	    /* Remove buffer from its old hash queue */
	    hashq_delete(b, bucket_array, buf_array);

	    /* Put the buffer into its new hash queue. */
	    dm0p_mlock(SEM_EXCL, &bucket_array[hash_bucket].hash_mutex);
	    hash_chain = bucket_array[hash_bucket].hash_next;
	    b->buf_next = hash_chain;
	    b->buf_prev = BUF_ENDLIST;
	    if (hash_chain != BUF_ENDLIST)
		buf_array[hash_chain].buf_prev = b->buf_id.bid_index;
	    bucket_array[hash_bucket].hash_next = b->buf_id.bid_index;
	    b->buf_hash_bucket = hash_bucket;
	    dm0p_munlock(&bucket_array[hash_bucket].hash_mutex);
	}

	/* Insert buffer into new table hash queue */
	tabq_insert(b);

	/*
	** Unlock the buffer mutex.
	*/
	dm0p_munlock(&b->buf_mutex);
    }

    return (status);
}

/*
** Name: fix_tableio_ptr - Get Table IO descriptor for read/write operations
**
** Description:
**	This routine fixes a table io descriptor from the table cache to
**	use for IO operations.
**
**	If a Table Descriptor is found for the specified table, but it is
**	listed as being partially open, then the tbio is checked for access
**	to the desired page.  If the location of the specified page is
**	not open in the table descriptor, then the tbio is not returned.
**
**	If a Table Descriptor is found, it is validated against the current
**	buffer by comparing its cache lock value with the version stored
**	in the cached buffer.  If they do not match, then the tbio is not
**	returned.
**
**	If no Valid Table Descriptor is found which gives access for requested 
**	buffer, a warning is returned, unless DM0P_TBIO_BUILD was specified,
**      in which case the DCB is searched for, and a new TBIO is built
**	if the DCB can be located. This is only done for write_along threads,
**	so they can write pages for tables that of course are not open
**	in this server.
**	
**
**	If the TCB is currently busy, this routine will return a warning
**	unless the DM0P_TBIO_WAIT option is specified (in which case it
**	will wait out the BUSY state).
**
**	If the caller holds the buffer manager mutexed, then it must pass
**	the DM0P_TBIO_MUTEX_HELD flag.  This indicates that the routine should
**	release the mutex while waiting out a BUSY tcb state.
**
**	The fix_tableio_ptr routine is modified by the following options:
**
**	    DM0P_TBIO_WAIT		- Wait for TCB if it is busy
**	    DM0P_TBIO_MUTEX_HELD	- Indicates caller holds BM mutex
**	    DM0P_TBIO_NOPARTIAL		- Indicates that a partially open
**					  table descriptor should never be
**					  returned.
**	    DM0P_TBIO_BUILD		- build tbio if not there already
**          DM0P_NOT_MATERIALIZED       - Don't return table descriptor for
**                                        tables that haven't been materialized yet.
**
** Inputs:
**	b				buffer for which table descriptor needed
**	flags				options to fix request:
**					    DM0P_TBIO_WAIT
**					    DM0P_TBIO_MUTEX_HELD
**					    DM0P_TBIO_NOPARTIAL
**					    DM0P_TBIO_BUILD
**	lock_list			Lock List Id
**	log_id				Log transaction id
**
** Outputs:
**	tableio_ptr			Set to point to fixed tableio cb.
**	err_code			Reason for error return status.
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**
** History:
**	26-oct-1992 (rogerk)
**          Created for Reduced Logging Project.
**	15-Sep-1996 (jenjo02)
**	    Caller must hold buffer's buf_mutex EXCL, which replaces the
**	    functionality of "bmcb_mutex". The DM0P_TBIO_WAIT now
**	    applies to buf_mutex, having deprecated bmcb_mutex.
**	23-Sep-1997 (jenjo02)
**	    When a BUSY is returned from dm2t_fx_tabio_cb() and DM0P_TBIO_WAIT
**	    DM0P_TBIO_MUTEX_HELD are specified, release the buffer mutex before
**	    calling dm2t_wt_tabio_ptr(), but don't reacquire it before returning
**	    E_DB_WARN. In every case the caller just released the buffer mutex
**	    anyway and went on to retry the buffer.
**      31-aug-2000 (stial01 for jenjo02)
**          ignore E_DM0054 for indexes (B83999)
**	18-May-2007 (kibro01) b118353
**	    Amend fix for b56705 to check for materializing rather than deferred
**	    to avoid fastload locking when trying to fix a page which it itself
**	    has in the cache (i.e. any single-thread using all the cache)
**	14-Jan-2004 (jenjo02)
**	    Check db_tab_index > 0 for index, not simply != 0.
*/
static DB_STATUS
fix_tableio_ptr(
DM0P_BUFFER	*b,
i4		flags,
i4		lock_list,
i4		log_id,
DMP_TABLE_IO	**tableio_ptr,
i4		*err_code)
{
    DMP_TABLE_IO    *tbio;
    DB_STATUS	    status;

    /*
    ** Look up table descriptor in the TCB cache
    */
    status = dm2t_fx_tabio_cb(b->buf_dbid, &b->buf_tabid, (DB_TRAN_ID *)0, 
	lock_list, log_id, (i4)DM2T_NOWAIT, &tbio, err_code);

    if (status != E_DB_OK)
    {
	/*
	** If BUSY warning was returned then wait for the busy status to
	** be turned off.  We must release the buffer mutex (if held)
	** while waiting out the busy state.
	*/
	if ((flags & DM0P_TBIO_WAIT) && (*err_code == W_DM9C51_DM2T_FIX_BUSY))
	{
	    /* Release buffer's mutex before we wait for TCB */
	    if (flags & DM0P_TBIO_MUTEX_HELD)
		dm0p_munlock(&b->buf_mutex);

	    dm2t_wt_tabio_ptr(b->buf_dbid, &b->buf_tabid, 
		(i4)0, lock_list);

	    /* Don't reacquire buffer's mutex; just return E_DB_WARN */
	}
	else if ((*err_code == W_DM9C50_DM2T_FIX_NOTFOUND) ||
		    (*err_code == E_DM0054_NONEXISTENT_TABLE && 
		    b->buf_tabid.db_tab_index > 0))
	{
	    if (flags & DM0P_TBIO_BUILD)
	    {
		DMP_DCB             *dcb;
		DMP_TCB             *tcb = NULL;

		/* see if this database is known to us, as might be likely */
		/* with a write_along thread. If so, build a tcb.  */
		status = dm2d_get_dcb(b->buf_dbid, &dcb, err_code);
		if (status != E_DB_OK)
		    *err_code = E_DM9C81_DM0P_GET_TABIO;
		else
		{
	            if (flags & DM0P_TBIO_MUTEX_HELD)
			dm0p_munlock(&b->buf_mutex);

		    status = dm2t_fix_tcb(b->buf_dbid, 
		      &b->buf_tabid, &b->buf_tran_id,
		      lock_list, (i4)0, DM2T_NOWAIT, dcb, &tcb, err_code);

	    	    if (flags & DM0P_TBIO_MUTEX_HELD)
			dm0p_mlock(SEM_EXCL, &b->buf_mutex);

		    /* if status ok, we have obtained a tbio, set it here */
		    /* as call to dm2t_fx_tabio_cb() would have */
		    if (status == E_DB_OK)
		        tbio = &tcb->tcb_table_io;
		}
	    }
	    else
	        status = E_DB_INFO;
	}
	else if (status != E_DB_WARN)
	{
	    ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 0);
	    ule_format(E_DM9C84_DM0P_FIX_TABIO, (CL_SYS_ERR *)0, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, err_code, 4,
		0, b->buf_dbid, 0, b->buf_tabid.db_tab_base,
		0, b->buf_tabid.db_tab_index, 0, b->buf_lock_key.lk_key4);
	    *err_code = E_DM9C81_DM0P_GET_TABIO;
	}

    }

    /* re-check status for error return, as we may have overridden it */
    if (status != E_DB_OK)
    {
	*tableio_ptr = NULL;
	return (status);
    }

    /*
    ** Check if this Table Descriptor matches the caller's requirements:
    **
    **    - if the table's cache lock value does not match the value on
    **      page buffer then it cannot be used to perform IO operations
    **      with this page.
    **
    **    - if the caller requested NOPARTIAL and this table descriptor
    **      is partially open, then we cannot return it.
    **
    **    - if the partially open table descriptor does not allow access to
    **      the location which holds the indicated page, then we cannot
    **      return it.
    ** Amend fix for b56705 to check for materializing rather than deferred
    ** to avoid fastload locking when trying to fix a page which it itself
    ** has in the cache (i.e. any single-thread using all the cache) 
    ** b118353 (kibro01)
    */
    if ((tbio->tbio_cache_valid_stamp != b->buf_tcb_stamp) ||
         ( (flags & DM0P_NOT_MATERIALIZED) && 
           (tbio->tbio_location_array [0].loc_fcb->fcb_state & FCB_MATERIALIZING) ) ||
	 ((tbio->tbio_status & TBIO_PARTIAL_CB) &&
		(   (flags & DM0P_TBIO_NOPARTIAL) ||
		    ( ! (tbio->tbio_status & TBIO_OPEN)) ||
		    (dm2f_check_access(tbio->tbio_location_array,
					tbio->tbio_loc_count, 
					b->buf_lock_key.lk_key4) == FALSE))))
    {
	unfix_tableio_ptr(&tbio, lock_list, log_id, (i4)__LINE__);
	status = E_DB_INFO;
    }

    *tableio_ptr = tbio;
    return (status);
}

/*
** Name: unfix_tableio_ptr - Release Table IO Control Block
**
** Description:
**	This routine releases a Table IO Control Block fixed by fix_tableio_ptr.
**	It calls the table cache routines to unfix the current handle to
**	the tabio.
**
**	This routine will DMDCHECK if the release request fails.
**
** Inputs:
**	tableio_ptr			Pointer to tableio cb to release.
**	lock_list			Lock List Id
**	log_id				Log transaction id
**	caller_id			Unique identifier for caller of
**					routine used for debugging purposes.
**
** Outputs:
**	err_code			Reason for error return status.
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**
** History:
**	26-oct-1992 (rogerk)
**          Created for Reduced Logging Project.
**	10-Jan-2000 (jenjo02)
**	    Don't pass DM2T_NORELUPDAT to dm2t_ufx_tabio_cb().
**	    If it happens that this is a CP or WB thread and
**	    it's the last reference to the TCB, the
**	    iirelation tuple and page counts won't get updated!
**	14-Jun-2000 (jenjo02)
**	    Pass **tableio_ptr, zero it before returning.
*/
static VOID
unfix_tableio_ptr(
DMP_TABLE_IO	**tableio_ptr,
i4		lock_list,
i4		log_id,
i4		caller_id)
{
    DB_STATUS	    status;
    i4	    error;

    status = dm2t_ufx_tabio_cb(*tableio_ptr, (i4)0, 
	lock_list, log_id, &error);
    if (status != E_DB_OK)
    {
	ule_format(error, (CL_SYS_ERR *)0, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, &error, 0);
	ule_format(E_DM9C82_DM0P_UNFIX_TABIO_INFO, (CL_SYS_ERR *)0, ULE_LOG, 
	    NULL, (char *)0, (i4)0, (i4 *)0, &error, 2,
	    0, caller_id, 0, *tableio_ptr);
	dmd_check(E_DMF028_DM0P_UNFIX_TABIO);
    }
    *tableio_ptr = (DMP_TABLE_IO*)NULL;
}

/*
** Name: dm0p_compute_old_checksum	- Compute page checksum
**
** Description:
**	Computes checksum using every byte in the page, folds the
**	result into a DM_PGCHKSUM if needed.
**
** Inputs:
**	page			    Pointer to page to checksum
**	page_size		    page_size of this page.
**
** Outputs:
**	none
**
** Returns:
**	DM_PGCHKSUM
**
** History:
**	08-Sep-2000 (jenjo02)
**	    Renamed from dm0p_compute_checksum, left here for
**	    read-compatibility with pages that do not yet contain
**	    the quick checksum value.
*/
DM_PGCHKSUM
dm0p_compute_old_checksum(
DMPP_PAGE 	*page, 
DM_PAGESIZE	page_size)
{
    register u_i4 	*p = (u_i4 *)page;
    register u_i4	full_chksm = 0;
    register i4 	i;
    i4			tmp_result = 0;
    i4			shift;

    /* Use i4 for checksum, unwind the loop */
    for (i = 1 + (i4) page_size / (sizeof(*p) * 16); --i; p += 16)
       full_chksm ^= p[0] ^ p[1] ^ p[2] ^ p[3] ^ p[4] ^ p[5] ^ p[6] ^ p[7] ^
              p[8] ^ p[9] ^ p[10] ^ p[11] ^ p[12] ^ p[13] ^ p[14] ^ p[15];

    /* Fold into DM_PGCHKSUM result if need be */
    if ( (shift = sizeof(full_chksm) / sizeof(DM_PGCHKSUM)) )
    {
	while ( shift-- )
	{
	    tmp_result ^= full_chksm & 0xffff;
	    full_chksm = full_chksm >> 16;
	}
	full_chksm = tmp_result;
    }

    return((DM_PGCHKSUM)full_chksm);
}

/*
** Name: dm0p_compute_checksum	- Compute page checksum
**
** Description:
**	Computes the page checksum using the first and last
**	16 bytes on the page, folds the result into a DM_PGCHKSUM.
**
** Inputs:
**	page			    Pointer to page to checksum
**	page_size		    page_size of this page.
**
** Outputs:
**	none
**
** Returns:
**	DM_PGCHKSUM
**
** History:
**	10-july-1992 (jnash)
**	    Created as part of the 6.5 reduced logging project.
**	20-mar-1996 (nanpr01)
**	    Added page size as parameter to this routine. To compute
**	    the checksum pagesize is need since sizeof(*page) returns
**	    only the size of physical page.
**	08-Sep-2000 (jenjo02)
**	    Compute checksum using only first and last 16 bytes on
**	    page instead of contents of entire page.
*/
DM_PGCHKSUM
dm0p_compute_checksum(
DMPP_PAGE 	*page, 
DM_PAGESIZE	page_size)
{
    register char 	*pp = (char *)page;
    register u_i4 	*p;
    register u_i4	full_chksm = 0;
    register i4 	i;
    i4			tmp_result = 0;
    i4			shift;

    for ( i = 2; i--; )
    {
	p = (u_i4 *)pp;
        full_chksm ^= p[0] ^ p[1] ^ p[2] ^ p[3] ;
	pp += page_size - (sizeof(u_i4) * 4);
    }

    /* Fold into DM_PGCHKSUM result if need be */
    if ( (shift = sizeof(full_chksm) / sizeof(DM_PGCHKSUM)) )
    {
	while ( shift-- )
	{
	    tmp_result ^= full_chksm & 0xffff;
	    full_chksm = full_chksm >> 16;
	}
	full_chksm = tmp_result;
    }

    return((DM_PGCHKSUM)full_chksm);
}
/*
** Name: dm0p_validate_checksum	- Validate page checksum
**
** Description:
**	Compute the checksum of the contents of a page and validate
**	it with that on the page.
**
**	DM431 disables checksumming.
**	DM432 checks the checksum, but only writes error if failure.
**
** Inputs:
**	page			    Pointer to page to checksum
**	page_size		    page_size of this page.
**
** Outputs:
**	none
**
** Returns:
**	E_DB_OK if checksum matches
**	E_DB_ERROR if checksum does not match
**
** History:
**	10-july-1992 (jnash)
**	    Created as part of the reduced logging project.
**	27-oct-1992 (jnash)
**	    Add DM428 to check checksum and report errors to 
**	    dbms.log, but not to the caller.
**	18-sep-1995 (nick)
**	    Clash of tracepoints - move checksum DM427 to DM431 and DM428
**	    to DM432.
**	20-mar-1996 (nanpr01)
**	    Added page size as parameter to this routine. To compute
**	    the checksum pagesize is need since sizeof(*page) returns
**	    only the size of physical page.
**      06-may-1996 (nanpr01)
**          Change the reference to the page header with macros for
**	    New Page Format. Also added the page_size parameter in this
**	    routine.
**	08-Sep-2000 (jenjo02)
**	    If new, quick, checksum mismatches, try old algorithm
**	    before returning error.
*/
DB_STATUS 
dm0p_validate_checksum(
DMPP_PAGE 	*page,
i4              relpgtype,
DM_PAGESIZE	page_size)
{
    DM_PGCHKSUM	new_chksm, old_chksm;

    if ( DMZ_BUF_MACRO(31) || (dmf_svcb->svcb_checksum == FALSE) )
	return(E_DB_OK);

    if ( DMPP_VPT_GET_PAGE_STAT_MACRO(relpgtype, page) & DMPP_CHKSUM )
    {
	/*
	** Save the existing checksum, zero its place for the check
	*/
	old_chksm = DMPP_VPT_GET_PAGE_CHKSUM_MACRO(relpgtype, page);
	DMPP_VPT_SET_PAGE_CHKSUM_MACRO(relpgtype, page, 0);

	new_chksm = dm0p_compute_checksum(page, page_size);
	DMPP_VPT_SET_PAGE_CHKSUM_MACRO(relpgtype, page, old_chksm);

	if (old_chksm != new_chksm)
	{
	    DMPP_VPT_SET_PAGE_CHKSUM_MACRO(relpgtype, page, 0);
	    new_chksm = dm0p_compute_old_checksum(page, page_size);
	    DMPP_VPT_SET_PAGE_CHKSUM_MACRO(relpgtype, page, old_chksm);
	    if (old_chksm != new_chksm)
	    {
		/*
		** Checksum failure, check for trace point overrride
		*/
		if ( DMZ_BUF_MACRO(32) )
		{
		    TRdisplay(
			"dm0p_validate_checksum: checksum failure, page %d, page_size %d\n",
			DMPP_VPT_GET_PAGE_PAGE_MACRO(relpgtype, page),
			page_size);
		    return(E_DB_OK);
		}

		return(E_DB_ERROR);
	    }
	}

    }

    return(E_DB_OK);
}

/*
** Name: dm0p_insert_checksum	- Insert checksum on page
**
** Description:
**	Compute the checksum of the contents of a page and insert
**	it onto page.
**
** Inputs:
**	page			    Pointer to page to checksum
**	page_size		    page_size of this page.
**
** Outputs:
**	page			    With checksum filled in
**
** Returns:
**	VOID
**
** History:
**	10-july-1992 (jnash)
**	    Created as part of the reduced logging project.
**	20-mar-1996 (nanpr01)
**	    Added page size as parameter to this routine. To compute
**	    the checksum pagesize is need since sizeof(*page) returns
**	    only the size of physical page.
**      06-may-1996 (nanpr01)
**          Change the reference to the page header with macros for
**	    New Page Format. Also added the page_size parameter in this
**	    routine.
*/
VOID 
dm0p_insert_checksum(
DMPP_PAGE 	*page,
i4              relpgtype,
DM_PAGESIZE	page_size)
{
    if ( (dmf_svcb->svcb_checksum == FALSE) || DMZ_BUF_MACRO(31))
    {
	/*
	** Override checksum generation (and checking).
	*/
	DMPP_VPT_CLR_PAGE_STAT_MACRO(relpgtype, page, DMPP_CHKSUM);
	DMPP_VPT_SET_PAGE_CHKSUM_MACRO(relpgtype, page, 0);
	return;
    }
    else
    {
	/*
	** Checksum is computed with the checksum itself zeroed and the
	** checksum bit asserted.
	*/
	DMPP_VPT_SET_PAGE_STAT_MACRO(relpgtype, page, DMPP_CHKSUM);
	DMPP_VPT_SET_PAGE_CHKSUM_MACRO(relpgtype, page, 0);
	DMPP_VPT_SET_PAGE_CHKSUM_MACRO(relpgtype, page, 
		dm0p_compute_checksum(page, page_size));
    }

    return;
}

/*
** Name: dm0p_before_image_check	- Check whether BI is needed.
**
** Description:
**	This routine is called just before a page is modified to determine
**	whether a Before Image needs to be generated for the page before 
**	it is updated.
**
**	The before_image_check must be called anytime a page is modified
**	as part of a DML operation and must be done BEFORE mutexing the
**	page so as to avoid logfull stalling issues.
**
**	Before Images are currently used only for Online Backup processing.
**	Whenever a page is modified while an Online Backup is in progress,
**	a Before Image is logged.  These BI's will be copied to the database
**	Dump file to be used to restore the database to a consistent state
**	at the start of a Rollforward.
**
**	If we recognize that this page has already been logged during the
**	current backup operation, then we need not log a new Before Image.
**	We recognize this by checking the Log Sequence Number of the last
**	update to this page.  If the page has already been updated since the
**	beginning of the checkpoint, then a Before Image of it must have
**	already been logged, so we need not log another.
**
**	(Note that this optimization depends on the fact that we currently
**	do not allow a backup to start while there are transactions in
**	progress on a database.  Without this restriction it would be possible
**	for us to miss logging a needed BI if the backup were started between
**	the call to this routine and the update to the page.)
**
** Inputs:
**	rcb		- Record Control Block for current operation.
**			  Can be used to locate Database Control Block.
**	page		- Pointer to page which might need a Before Image.
**
** Outputs:
**      err_code
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
**	17-oct-1991 (rogerk)
**	    Great Merge of DMF : Move 6.4 changes into 6.5.  In this case,
**	    the 6.4 version of this file was moved directly to 6.5 and the
**	    6.5 changes were merged back in:
**
**	    13-mar-1990 (derek)
**		Created from inline DM2R code for Jupiter.
**	17-sep-1991 (rogerk)
**	    Reduced Logging: completely rewrote routine.
**	    The routine used to be used to determine if a Before Image was
**	    needed for a number of different uses, most of them involving
**	    Undo recovery.  It logged BI's for normal DML statements, Free
**	    Space pages during certain operations and Btree Indexes during
**	    Online Backup.  With the 6.5 recovery algorithms, Before Images
**	    are logged only for Online Backup processing.
**	02-jan-1993 (jnash)
**	    Reduced logging.  Add LGreserve call.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**		In before_image_check, compare the page_log_addr (which is
**		    actually an LSN) with d->dcb_backup_lsn, rather than
**		    d->dcb_backup_addr.
**	26-jul-1993 (rogerk)
**	    Change generation of Before Image log records during Online Backup.
**	    Instead of calling before_image_check on every dm0p_fixpage 
**	    operation, we now expect the modifier of the page to call 
**	    dm0p_before_image_check before changing its contents.
**	    Made before_image_check external and added dm0p_ prefix.
**	20-sep-1993 (jnash)
**	    Add flag param to LGreserve() calls.
**      06-may-1996 (nanpr01)
**          Changed all page header access as macro for New Page Format.
**      01-may-1996 (stial01)
**          LGreserve adjustments for removal of bi_page from DM0L_BI
**	17-jun-96 (nick)
**	    There is a recovery operation CLR associated with a DM0L_BI
**	    record and hence we MUST reserve log space for this as well.
**	10-jan-1997 (nanpr01)
**	    Put back the space holder for DM0L_BI, DM0L_AI.
*/
DB_STATUS
dm0p_before_image_check(
DMP_RCB		*rcb,
DMPP_PAGE	*page,
i4		*err_code)
{
    DMP_DCB	    *d = rcb->rcb_tcb_ptr->tcb_dcb_ptr;
    DMP_RCB	    *r;
    DMP_TCB	    *t;
    DMP_TABLE_IO    *tbio;
    LG_LSN	    lsn;
    i4	    loc_id;
    i4	    loc_config_id;
    DB_STATUS	    status;
    STATUS	    cl_status;
    CL_SYS_ERR	    sys_err;

   /*
   ** Check if the database is currently being backed up.  If not, then
   ** no Before Image is required.
   */
   if ((d->dcb_status & DCB_S_BACKUP) == 0)
	return (E_DB_OK);

    /*
    ** If session is in nologging mode then bypass BI logging.
    */
    if (rcb->rcb_logging == 0)
	return (E_DB_OK);

    status = E_DB_OK;
    r = rcb;
    t = r->rcb_tcb_ptr;
    tbio = &t->tcb_table_io;

    /*
    ** Get information on the location to which the update is being made.
    */
    loc_id = DM2F_LOCID_MACRO(tbio->tbio_loc_count, 
		(i4) DMPP_VPT_GET_PAGE_PAGE_MACRO(t->tcb_rel.relpgtype, page));
    loc_config_id = tbio->tbio_location_array[loc_id].loc_config_id;

    /*
    ** Determine if a BI is needed.  A BI is needed if this page is being
    ** touched for the first time since the backup began.  This is determined
    ** by the log sequence number of the last update to the page which is
    ** recorded in the buffer header.
    */
    if (LSN_LT(DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(t->tcb_rel.relpgtype, page), 
		&d->dcb_backup_lsn))
    {
	/* reserve space for BI */
	cl_status = LGreserve(0, r->rcb_log_id, 2,
			(sizeof(DM0L_BI) - sizeof(DMPP_PAGE) + 
			t->tcb_rel.relpgsize ) * 2,
			&sys_err);
	if (cl_status != OK)
	{
	    (VOID) ule_format(cl_status, &sys_err, ULE_LOG, NULL,
		    (char *)0, 0L, (i4 *)0, err_code, 0);
	    (VOID) ule_format(E_DM9054_BAD_LOG_RESERVE, &sys_err,
		    ULE_LOG, NULL, (char *)0, 0L, (i4 *)0,
		    err_code, 1, 0, r->rcb_log_id);
	    *err_code = E_DM9C86_DM0P_BICHECK;
	    return(E_DB_ERROR);
	}

	status = dm0l_bi(rcb->rcb_log_id, DM0L_DUMP,
	    &t->tcb_rel.reltid, &t->tcb_rel.relid, &t->tcb_rel.relowner, 
	    t->tcb_rel.relpgtype, t->tcb_rel.relloccount, loc_config_id, 
	    DM0L_BI_DUMP, DMPP_VPT_GET_PAGE_PAGE_MACRO(t->tcb_rel.relpgtype, page),
	    t->tcb_rel.relpgsize, page, (LG_LSN *)0, &lsn, err_code);

    }

    if ((status != E_DB_OK) && (*err_code > E_DM_INTERNAL))
    {
	ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL, 
	    (char *)0, (i4)0, (i4 *)0, err_code, 0);
	ule_format(E_DM9C85_DM0P_BICHECK_INFO, (CL_SYS_ERR *)0, ULE_LOG, 
	    NULL, (char *)0, (i4)0, (i4 *)0, err_code, 4,
	    sizeof(DB_DB_NAME), d->dcb_name.db_db_name, 
	    sizeof(DB_TAB_NAME), &rcb->rcb_tcb_ptr->tcb_rel.relid,
	    sizeof(DB_OWN_NAME), &rcb->rcb_tcb_ptr->tcb_rel.relowner, 
	    0, DMPP_VPT_GET_PAGE_PAGE_MACRO(t->tcb_rel.relpgtype, page));
	*err_code = E_DM9C86_DM0P_BICHECK;
    }

    return (status);
}

/*
** Name: dm0p_log_error - log an error and preserving and error messages
**			  we may want to passs to the upper layers.
**
** Description:
**
** Inputs:
**    i4	new_err_code	The new error code we have just generated.
**    i4  *old_err_code    Ptr to older error code.
**    i4	do_log		Should we log the error we discard?
**
** Outputs:
**      err_code
**
**	Returns:
**	    None
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	18-feb-1993 (rmuth)
**	    Created.
**	23-may-1994 (andys)
**	    Add do_log parameter which dictates whether we should log the error 
**	    or not.
*/
static VOID
dm0p_log_error(
    i4	new_err_code,
    i4	*old_err_code,
    i4 	do_log)
{

    if ( (*old_err_code == E_DM9206_BM_BAD_PAGE_NUMBER ) ||
         (*old_err_code == E_DM920C_BM_BAD_FAULT_PAGE ) ||
         (*old_err_code <= E_DM_INTERNAL ) )
    {
	/*
	** We want to pass the error message in old_err_code to the
	** upper layers but still issue a trace back message .
	*/
	if (do_log)
	    ule_format(new_err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL, (char *)0,
		   (i4)0, (i4 *)0, &new_err_code, 0);
    }
    else
    {
	/*
	** Opposite of the above so can overwrite old_err_code
	*/
	if (do_log)
	    ule_format(*old_err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL, (char *)0,
		   (i4)0, (i4 *)0, old_err_code, 0);
	*old_err_code = new_err_code;
    }

    return;
}

/*
** Name: dm0p_1toss_page - Toss a single page from the cache
**
** Description:
**	Looks for the specified page in the buffer cache and tosses
**	it if found.  Note exceptions below when the toss request will
**	not be honored.
**
**	If the page is currently in use (fixed or mutexed) then the
**	request is ignored.
**
**	If the page is modified and the server has no valid TCB for the
**	table then the page will not be tossed.
**
** Inputs:
**	dbid			Database id
**	table_id		Table id
**	page_number		Page number
**	lock_list		Lock list for event wait calls
**
** Outputs:
**	none
**
** Returns:
**	VOID
**
** History:
**	24-may-1993 (rogerk)
**	    Created for reduced logging test tracepoint.
**	18-oct-1993 (rmuth)
**	    Add DMP_BMCB statistics.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	26-jul-96 (nick)
**	    Rewrite for(;;) as do {} while() to remove compiler warning.
**	03-Sep-1997 (jenjo02)
**	    Cross-integration of 427407. page_number in dm0p_1toss_page(),
**	    dm0p_trans_page_lock(), should be DM_PAGENO (u_i4), not i4.
**	08-Sep-1998 (shero03)
**	    Change the hash algorithm to mod by the hash count.
*/
VOID
dm0p_1toss_page(
i4	    dbid,
DB_TAB_ID   *table_id,
DM_PAGENO   page_number,
i4	    lock_list)
{
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    i4              cache_ix = 0;  /* FIX ME what is this */
    DM0P_BMCB       *bm = lbm->lbm_bmcb[cache_ix];
    DMP_TABLE_IO    *tbio = NULL;
    DB_STATUS	    status = E_DB_OK;
    DM0P_BUFFER	    *b;
    DM0P_BUFFER     *buf_array = lbm->lbm_buffers[cache_ix];
    DM0P_HASH_HEAD  *bucket_array = lbm->lbm_buckets[cache_ix];
    i4	    hash_chain;
    i4	    hash_bucket;
    i4	    error;

    /*
    ** Compute the hash bucket to look in for this page.
    */
    hash_bucket = (u_i4)(dbid + 
    		              table_id->db_tab_base + 
	                      table_id->db_tab_index +
		              page_number) % bm->bm_hshcnt;

    /*
    ** Search the chain of buckets till we find the matching buffer or
    ** we reach the end of the list.
    */
    b = NULL;
    dm0p_mlock(SEM_SHARE, &bucket_array[hash_bucket].hash_mutex);
    hash_chain = bucket_array[hash_bucket].hash_next;

    while (hash_chain != BUF_ENDLIST)
    {
	b = &buf_array[hash_chain];
	hash_chain = b->buf_next;
	if (b->buf_lock_key.lk_key4 != page_number || 
	    b->buf_lock_key.lk_key1 != dbid ||
	    b->buf_lock_key.lk_key2 != table_id->db_tab_base ||
	    b->buf_lock_key.lk_key3 != table_id->db_tab_index)
	{
	    b = NULL;
	    continue;
	}
	/*
	** Unlock the hash chain, lock the buffer, and recheck.
	*/
	dm0p_munlock(&bucket_array[hash_bucket].hash_mutex);
	dm0p_mlock(SEM_EXCL, &b->buf_mutex);
	if (b->buf_hash_bucket == hash_bucket &&
	    b->buf_lock_key.lk_key4 == page_number && 
	    b->buf_lock_key.lk_key1 == dbid &&
	    b->buf_lock_key.lk_key2 == table_id->db_tab_base &&
	    b->buf_lock_key.lk_key3 == table_id->db_tab_index)
	{
	    break;
	}
	/*
	** Relock the hash chain, check our position, unlock the buffer,
	** and continue.
	*/
	dm0p_munlock(&b->buf_mutex);
	dm0p_mlock(SEM_SHARE, &bucket_array[hash_bucket].hash_mutex);
	hash_chain = bucket_array[hash_bucket].hash_next;
	b = NULL;
    }

    /*
    ** If the page was not found, do nothing.
    */
    if (b == NULL)
    {
	dm0p_munlock(&bucket_array[hash_bucket].hash_mutex);
	return;
    }

    do 
    {
	/*
	** If the page is fixed or busy then we can't toss it.
	*/
	if ((b->buf_sts & (BUF_FIXED | BUF_MUTEX)) == 0 &&
	     b->buf_sts & BUF_VALID)
	{
	    /*
	    ** If the buffer is modified then we need the table_io cb to toss it.
	    */
	    if (b->buf_sts & BUF_MODIFIED)
	    {
		status = fix_tableio_ptr(b, (i4)0, lock_list, 
					(i4)0, &tbio, &error);
		if ((status != E_DB_OK) || (tbio == NULL))
		    break;
	    }

	    /*
	    ** Toss this page.
	    ** The TCB pointer may be NULL if the page is not modified.
	    */
	    do
		status = force_page(b, tbio, FP_TOSS, lock_list, (i4)0, 
				    BMCL_NO_GATHER, (i4*)0, &error);
	    while ( status == E_DB_WARN );

	    if ( status == E_DB_OK )
		b = NULL;

	    /*
	    ** Release the table io control block.
	    */
	    if (tbio)
		unfix_tableio_ptr(&tbio, lock_list, (i4)0, (i4)__LINE__);
	}
    } while (FALSE);

    if ( b )
	dm0p_munlock(&b->buf_mutex);

    /* Record any errors in the error log */
    if (DB_FAILURE_MACRO(status))
    {
	ule_format(error, (CL_SYS_ERR *)0, ULE_LOG, NULL, 
	    (char *)0, (i4)0, (i4 *)0, &error, 0);
    }

    return;
}

static DM0P_BUFFER *
first_fixed_syscat_page( DB_TRAN_ID *tran_id )
{
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BUFFER	    *b;
    DM0P_BLQ_HEAD   *tran_bucket;
    DM0P_BID        *tran_chain;
    i4	    tran_next;
    
    /* Compute the tran hash bucket to queue this page. */
    tran_bucket = &bm_common->bmcb_txnq_buckets[
	tran_id->db_low_tran & (BMCB_TRAN_MAX -1)];

    /*
    ** Scan the hash bucket chain looking for any buffers belonging
    ** to this transaction. 
    */
    dm0p_mlock(SEM_SHARE, &tran_bucket->blqh_mutex);
    tran_chain = &tran_bucket->blqh_bid;
    while (tran_chain->bid_index != BUF_ENDLIST)
    {
	b = &lbm->lbm_buffers[tran_chain->bid_cache][tran_chain->bid_index];

	tran_chain = &b->buf_txnq.blq_next;
	tran_next = b->buf_txnq.blq_next.bid_index;

	if ((b->buf_tran_id.db_low_tran != tran_id->db_low_tran) ||
	    (b->buf_tran_id.db_high_tran != tran_id->db_high_tran))
	{
	    continue;
	}

	/*
	** Unlock the bucket chain, lock the buffer, check again.
	*/
	dm0p_munlock(&tran_bucket->blqh_mutex);
	dm0p_mlock(SEM_EXCL, &b->buf_mutex);

	if (b->buf_tran_id.db_low_tran == tran_id->db_low_tran &&
	    b->buf_tran_id.db_high_tran == tran_id->db_high_tran &&
	    (b->buf_tabid.db_tab_base >= 1 && b->buf_tabid.db_tab_base <= 4))
	{
	    /* Return with share lock on the buffer */
	    return (b);
	}
	dm0p_mlock(SEM_SHARE, &tran_bucket->blqh_mutex);
	if (tran_next != b->buf_txnq.blq_next.bid_index)
	    tran_chain = &tran_bucket->blqh_bid;
	dm0p_munlock(&b->buf_mutex);
    }
    dm0p_munlock(&tran_bucket->blqh_mutex);

    return ((DM0P_BUFFER *)0);
}

/*
** Name: dm0p_check_logfull - Check for installation logfull state.
**
** Description:
**	This routine queries the logging system and returns TRUE if
**	the system is currently in a logfull state and FALSE otherwise.
**
** Inputs:
**	dbid			Database id
**	table_id		Table id
**	page_number		Page number
**	lock_list		Lock list for event wait calls
**
** Outputs:
**	none
**
** Returns:
**	TRUE if system is in logfull state.
**	FALSE otherwise.
**
** History:
**	18-oct-1993 (rogerk)
**	    Created.
**      22-May-2000 (islsa01)
**          Declare this function dm0p_check_logfull to EXTERN instead of
**          local to this file. (B90402)  
**	11-Nov-2005 (jenjo02)
**	    Made static again. The way it was being used externally
**	    was bogus.
*/
static bool
dm0p_check_logfull(VOID)
{
    STATUS	    cl_status;
    CL_ERR_DESC	    sys_err;
    i4	    length;
    i4	    lgd_status;
    i4	    error;

    cl_status = LGshow(LG_S_LGSTS, (PTR)&lgd_status, 
		    sizeof(lgd_status), &length, &sys_err);
    if ((cl_status == OK) && 
	(lgd_status & LGD_LOGFULL))
    {
	return (TRUE);
    }

    if (cl_status != OK)
    {
	ule_format(E_DM9017_BAD_LOG_SHOW, &sys_err, ULE_LOG, NULL, 
	    (char *)0, (i4)0, (i4 *)0, &error, 1, 
	    0, LG_S_LGSTS);
	TRdisplay("Error showing the LOGFULL status in dm0p_check_logfull\n");
    }

    return (FALSE);
}

/*
** Name: dm0p_check_consistency	- Run consistency checks on the buffer manager.
**
** Description:
**	This routine can be run periodically from buffer manager routines
**	when enabled by trace points (see DM415) to help track down buffer
**	manager consistency bugs.
**
**	It scans through the buffer manager looking for anything that looks
**	to be illegal.  Current stuff that is checked:
**
**	Inconsistencies are reported to the server trace log.
**
**	The following tracepoints alter the termination of this routine:
**
**	    DM416 - dmd_check if any inconsistencies
**	    DM417 - signal the debugger if inconsistencies
**	    DM434 - dump BM on first failure
**
** Inputs:
**	id		identifier of calling routine
**
** Outputs:
**	None
**
** Returns:
**	E_DB_OK		- nothing wrong
**	E_DB_ERROR	- buffer manager inconsistent
**
** History:
**	17-nov-1993 (rogerk)
**	    Created as part of working on codex shared buffer manager problems.
**	28-dec-1993 (andys)
**	    Add timestamps.
**	    If trace point dm427 is set, dump control blocks on first
**	    inconsistency.
**	    After MAX_DM415 TRdisplays, don't do any more checking.
**	22-feb-1994 (andys)
**	    Cast args to dmd_fmt_cb.
**	22-feb-1994 (andys)
**	    Change args to dmd_fmt_cb to fit in with new arguments.
**	18-sep-1995 (nick)
**	    Change DM427 to DM434.  Note that the use of this tracepoint is 
**	    almost but not quite the same as DM430 ( but could probably
**	    be so ).
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**	10-Jan-2000 (jenjo02)
**	    Implement group priority queues.
**
*/
static DB_STATUS
dm0p_check_consistency(i4 id)
{
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DMP_BMSCB       *bmsegp;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    i4              cache_ix;
    i4              segment_ix;
    DM0P_BMCB       *bm;
    DM0P_BUFFER	    *b;
    DM0P_BUFFER     *buf_array;
    DM0P_GROUP      *group_array;
    DM0P_GROUP	    *g;
    STATUS	    already_locked = 0;
    i4	    free_count = 0;
    i4	    modify_count = 0;
    i4	    fixed_count = 0;
    i4	    free_g_count = 0;
    i4	    modify_g_count = 0;
    i4	    fixed_g_count = 0;
    i4	    next_buf;
    i4	    cnt;
    i4	    i, x;
    i4	    ret_val = E_DB_OK;
    static i4	    failed = 0;
    static i4      dm415_count = 0;	/* count of TRdisplays with dm415   */
    i4		    fcount,mcount,lcount,gfcount,gmcount,glcount;

    if (dm415_count > MAX_DM415)
	return E_DB_OK;
    /*
    ** Make sure we hold the appropriate mutex. Assume deadlock error just
    ** indicates that our session already holds the mutex. 
    */

    for (cache_ix = 0; cache_ix < DM_MAX_CACHE; cache_ix++)
    {
	bm = lbm->lbm_bmcb[cache_ix];
	if (!bm || !(bm->bm_status & BM_ALLOCATED))
	    continue;

	buf_array = lbm->lbm_buffers[cache_ix];
	group_array = lbm->lbm_groups[cache_ix];

	/*
	** Check Fixed Buffers.
	*/
	for (i = 0; i < bm->bm_bufcnt; i++)
	{
	    b = &buf_array[i];

	    if (b->buf_sts & BUF_FIXED)
	    {
		fixed_count++;
	    }
	}


	/*
	** Check free queues.
	*/
	for (i = 0; i < DB_MAX_TABLEPRI; i++)
	{
	    already_locked = CSp_semaphore(SEM_SHARE, 
		    &bm->bm_fpq[i].sth_mutex.bms_semaphore);
	    for (next_buf = bm->bm_fpq[i].sth_stq.stq_next;
		 next_buf != BUF_ENDLIST;
		 next_buf = b->buf_state.stq_next)
	    {
		b = &buf_array[next_buf];

		if (next_buf > bm->bm_sbufcnt)
		{
		    TRdisplay(
		       " %@ Group buffer on %dK non-group free queue: 0x%x\n",  
			KSIZE(bm->bm_pgsize), b);
		    dm415_count++;
		    ret_val = E_DB_ERROR;
		}
		if (b->buf_sts & BUF_FIXED)
		{
		    TRdisplay(" %@ Fixed buffer on %dK free queue: 0x%x\n", 
			KSIZE(bm->bm_pgsize), b);
		    dm415_count++;
		    ret_val = E_DB_ERROR;
		}
		if (b->buf_sts & BUF_MODIFIED)
		{
		    TRdisplay(" %@ Modified buffer on %dK free queue: 0x%x\n", 
			KSIZE(bm->bm_pgsize), b);
		    dm415_count++;
		    ret_val = E_DB_ERROR;
		}
		if (b->buf_priority != i)
		{
		    TRdisplay(
			" %@ Priority wrong on %dK free queue %d: 0x%x\n", 
			KSIZE(bm->bm_pgsize), i, b);
		    dm415_count++;
		    ret_val = E_DB_ERROR;
		}
		if (free_count++ > bm->bm_bufcnt)
		{
		    TRdisplay(" %@ %dK Free Queue [%d] possibly looped.\n", 
				KSIZE(bm->bm_pgsize), i);
		    dm415_count++;
		    ret_val = E_DB_ERROR;
		    break;
		}
	    }
	    if (!already_locked)
			CSv_semaphore(&bm->bm_fpq[i].sth_mutex.bms_semaphore);
	}

	/*
	** Check modified queues.
	*/
	for (i = 0; i < DB_MAX_TABLEPRI; i++)
	{
	    already_locked = CSp_semaphore(SEM_SHARE, 
		    &bm->bm_mpq[i].sth_mutex.bms_semaphore);
	    for (next_buf = bm->bm_mpq[i].sth_stq.stq_next;
		 next_buf != BUF_ENDLIST;
		 next_buf = b->buf_state.stq_next)
	    {
		b = &buf_array[next_buf];

		if (b->buf_sts & BUF_FIXED)
		{
		    TRdisplay(" %@ Fixed buffer on %dK modify queue: 0x%x\n", 
				KSIZE(bm->bm_pgsize), b);
		    dm415_count++;
		    ret_val = E_DB_ERROR;
		}
		if (b->buf_priority != i)
		{
		    TRdisplay(
			" %@ Priority wrong on %dK modify queue %d: 0x%x\n",
			KSIZE(bm->bm_pgsize), i, b);
		    dm415_count++;
		    ret_val = E_DB_ERROR;
		}
		if (modify_count++ > bm->bm_bufcnt)
		{
		    TRdisplay(" %@ %dK Modify Queue [%d] possibly looped.\n", 
				KSIZE(bm->bm_pgsize), i);
		    dm415_count++;
		    ret_val = E_DB_ERROR;
		    break;
		}
	    }
	    if (!already_locked)
			CSv_semaphore(&bm->bm_mpq[i].sth_mutex.bms_semaphore);
	}

	if (bm->bm_status & BM_GROUP)
	{
	    /*
	    ** Check Fixed Groups.
	    */
	    for (x = 0; x < bm->bm_gcnt; x++)
	    {
		g = &group_array[x];

		if (g->g_sts & G_FIXED)
		{
		    fixed_g_count++;
		    /*
		    ** Cycle through buffers in this group.
		    */
		    cnt = 0;
		    for (i = 0; i < g->g_pages; i++)
		    {
			b = &buf_array[g->g_buffer + i];
			if (b->buf_sts & BUF_FIXED)
			    cnt++;
		    }
		    if (cnt == 0)
		    {
			TRdisplay(" %@ %dK Fixed group has no fixed pages: 0x%x\n",
				    KSIZE(bm->bm_pgsize), g);
			dm415_count++;
			ret_val = E_DB_ERROR;
		    }
		}
	    }

	    /*
	    ** Check Free Group queue.
	    */
	    for (x = 0; x < DB_MAX_TABLEPRI; x++)
	    {
		already_locked = CSp_semaphore(
			SEM_SHARE, &bm->bm_gfq[x].sth_mutex.bms_semaphore);
		for (next_buf = bm->bm_gfq[x].sth_stq.stq_next;
		     next_buf != BUF_ENDLIST;
		     next_buf = g->g_state.stq_next)
		{
		    g = &group_array[next_buf];

		    if (g->g_sts & G_FIXED)
		    {
			TRdisplay(" %@ Fixed group on %dK free queue: 0x%x\n", 
				    KSIZE(bm->bm_pgsize), g);
			dm415_count++;
			ret_val = E_DB_ERROR;
		    }
		    if (g->g_sts & G_MODIFIED)
		    {
			TRdisplay(" %@ Modified group on %dK free queue: 0x%x\n",
				    KSIZE(bm->bm_pgsize), g);
			dm415_count++;
			ret_val = E_DB_ERROR;
		    }

		    /*
		    ** Cycle through buffers in this group.
		    */
		    for (i = 0; i < g->g_pages; i++)
		    {
			b = &buf_array[g->g_buffer + i];

			if (b->buf_sts & BUF_FIXED)
			{
			    TRdisplay(" %@ Fixed buffer in %dK free group: 0x%x\n", 
				    KSIZE(bm->bm_pgsize), b);
			    dm415_count++;
			    ret_val = E_DB_ERROR;
			}
			if (b->buf_sts & BUF_MODIFIED)
			{
			    TRdisplay(
				" %@ Modified buffer in %dK free group: 0x%x\n", 
				KSIZE(bm->bm_pgsize), b);
			    dm415_count++;
			    ret_val = E_DB_ERROR;
			}
		    }

		    if (free_g_count++ > bm->bm_gcnt)
		    {
			TRdisplay(" %@ %dK Free Group Queue possibly looped.\n",
				    KSIZE(bm->bm_pgsize));
			dm415_count++;
			ret_val = E_DB_ERROR;
			break;
		    }

		    next_buf = g->g_state.stq_next;
		}
		if (!already_locked)
			CSv_semaphore(&bm->bm_gfq[x].sth_mutex.bms_semaphore);
	    }

	    /*
	    ** Check Modified Group queue.
	    */
	    for (x = 0; x < DB_MAX_TABLEPRI; x++)
	    {
		already_locked = CSp_semaphore(
		    SEM_SHARE, &bm->bm_gmq[x].sth_mutex.bms_semaphore);
		for (next_buf = bm->bm_gmq[x].sth_stq.stq_next;
		     next_buf != BUF_ENDLIST;
		     next_buf = g->g_state.stq_next)
		{
		    g = &group_array[next_buf];

		    if ((g->g_sts & G_MODIFIED) == 0)
		    {
			TRdisplay(
			    " %@ Non-Modified group on %dK modified queue: 0x%x\n", 
			    KSIZE(bm->bm_pgsize), g);
			dm415_count++;
			ret_val = E_DB_ERROR;
		    }

		    /*
		    ** Cycle through buffers in this group.
		    */
		    cnt = 0;
		    for (i = 0; i < g->g_pages; i++)
		    {
			b = &buf_array[g->g_buffer + i];

			if (b->buf_sts & BUF_MODIFIED)
			    cnt++;
			if (b->buf_sts & BUF_FIXED)
			{
			    TRdisplay(
				" %@ Fixed buffer in %dK modified group: 0x%x\n", 
				KSIZE(bm->bm_pgsize), b);
			    dm415_count++;
			    ret_val = E_DB_ERROR;
			}
		    }
		    if (cnt == 0)
		    {
			TRdisplay(
			    " %@ %dK Modified group has no modified pages: 0x%x\n", 
			    KSIZE(bm->bm_pgsize), g);
			dm415_count++;
			ret_val = E_DB_ERROR;
		    }

		    if (modify_g_count++ > bm->bm_gcnt)
		    {
			TRdisplay(
			    " %@ %dK Modified Group Queue possibly looped.\n", 
			    KSIZE(bm->bm_pgsize));
			dm415_count++;
			ret_val = E_DB_ERROR;
			break;
		    }

		    next_buf = g->g_state.stq_next;
		}
		if (!already_locked)
			CSv_semaphore(&bm->bm_gmq[x].sth_mutex.bms_semaphore);
	    }
	}

	/*
	** Consistency check buffer manager counts.
	*/
	DM0P_COUNT_ALL_QUEUES(fcount,mcount,lcount);
	DM0P_COUNT_ALL_GQUEUES(gfcount,gmcount,glcount);
	
	if (fcount != free_count)
	{
	    TRdisplay(
	      " %@ %dK Free count does not match # of free pages: %d vs. %d\n",
		KSIZE(bm->bm_pgsize), fcount, free_count);
	    dm415_count++;
	    ret_val = E_DB_ERROR;
	}
	if (mcount != modify_count)
	{
	    TRdisplay(
	 " %@ %dK Modify count does not match # of modified pages: %d vs. %d\n",
		KSIZE(bm->bm_pgsize), mcount, modify_count);
	    dm415_count++;
	    ret_val = E_DB_ERROR;
	}
	if (lcount != fixed_count)
	{
	    TRdisplay(
	     " %@ %dK Fixed count does not match # of fixed pages: %d vs. %d\n",
		KSIZE(bm->bm_pgsize), lcount, fixed_count);
	    dm415_count++;
	    ret_val = E_DB_ERROR;
	}
	if (gfcount != free_g_count)
	{
	    TRdisplay(
	" %@ %dK Group free count does not match # of free groups: %d vs. %d\n",
		KSIZE(bm->bm_pgsize), gfcount, free_g_count);
	    dm415_count++;
	    ret_val = E_DB_ERROR;
	}
	if (gmcount != modify_g_count)
	{
	    TRdisplay(
" %@ %dK Group modified count does not match # of modified groups: %d vs. %d\n",
		KSIZE(bm->bm_pgsize), gmcount, modify_g_count);
	    dm415_count++;
	    ret_val = E_DB_ERROR;
	}
	if (glcount != fixed_g_count)
	{
	    TRdisplay(
      " %@ %dK Group fixed count does not match # of fixed groups: %d vs. %d\n",
		KSIZE(bm->bm_pgsize), glcount, fixed_g_count);
	    dm415_count++;
	    ret_val = E_DB_ERROR;
	}
    }

    if (ret_val != E_DB_OK)
    {
	TRdisplay(" %@ DM0P_CHECK_CONSISTENCY: errors reported from id %d.\n", id);
	dm415_count++;

	if (DMZ_BUF_MACRO(16))
	    dmd_check(E_DM9207_BM_INTERNAL_ERROR);

#ifdef VMS
	if (DMZ_BUF_MACRO(17))
	{
	    lib$signal(SS$_DEBUG);
	}
#endif
    }

    if (DMZ_BUF_MACRO(34) && (!failed) && (ret_val != E_DB_OK))
    {
	i4	dmd_param = 1;
	/* 
	** This is our first failure...
	*/
	failed++;
	for (segment_ix = 0; segment_ix < DM_MAX_BUF_SEGMENT; segment_ix++)
	{
	    if ( (bmsegp = dmf_svcb->svcb_bmseg[segment_ix]))
		dmd_fmt_cb(&dmd_param, (DM_OBJECT *) bmsegp);
	}
	dmd_fmt_cb(&dmd_param, (DM_OBJECT *) lbm);

    }
    return (ret_val);
}

/*
** Name: dbg_dm429
**
** Description:
**	This routine works in conjunction with dm0p_force_pages to
**	keep track of how long the transaction queues are.
**	It should only be called when trace point dm429 is set.
**	If it finds the queues looped it prints a message once only to
**	II_DBMS_LOG.
**	If trace point dm430 is set then it also does a BM dump.
**
** Inputs:
** 	count_ptr 			pointer to current count of queue length
** 	lock_list			lock_list for tx
**      tran_id                         The transaction id to search with.
**	log_id				The logging system transaction id.
**	action				action passed to dm0p_force_pages
**	num_bufs;			number of bufs looked at
**	num_other_bufs			number of bufs in chain
**	tran_bucket			pointer to start point of chain
**
** Outputs:
**	None
**
** Returns:
**	None
**
** History:
**	17-jan-1994 (andys)
**	    Created as part of work on bug 55079.
**	22-feb-1994 (andys)
**	    Cast args to dmd_fmt_cb.
**	22-feb-1994 (andys)
**	    Change args to dmd_fmt_cb to fit in with new arguments.
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**
*/
static VOID	    
dbg_dm429( 
int 		*count_ptr,
i4		lock_list,
DB_TRAN_ID      *tran_id,
i4		log_id,
i4		action,
i4	    	num_bufs,
i4	    	num_other_bufs,
DM0P_BLQ_HEAD   *tran_bucket
)
{
    DMP_LBMCB	    *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DMP_BMSCB       *bmsegp;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    i4              segment_ix;
    static i4  dumped = FALSE;  /* have we already done a dmd_fmt_cb? */
    static i4  diag = FALSE;    /* did we already print diagnostics?  */

    /*
    ** Keep a count of the number of buffers we traverse
    */
    if ((*count_ptr)++ > bm_common->bmcb_totbufcnt)
    {
	/*
	** We have counted more buffers on the transaction 
	** queue than there are buffers, so we assume this 
	** queue is looped. 
	*/
	if (diag == FALSE)
	{
	    /*
	    ** Only print this diagnostic once
	    */
	    diag = TRUE;
	    TRdisplay("%@ Possible loop in tx queue\n");
	    TRdisplay("lock_list %x log_id %x action %x \n", 
		    lock_list, log_id, action) ;
	    TRdisplay("queue for tx %x %x\n", 
		    tran_id->db_high_tran,
		    tran_id->db_low_tran);
	    TRdisplay("num_bufs %x\n", num_bufs);
	    TRdisplay("num_other_bufs %x\n", num_other_bufs);
		TRdisplay("tx q start %x\n",  *tran_bucket);
	}
	/*
	** As we think the queue is looped,
	** we will print out the buffers until
	** we have printed enough to see the looping.
	*/
	if (DMZ_BUF_MACRO(30) && (dumped == FALSE)) 
	{ 
	    i4	dmd_param = 1;
	    /* 
	    ** If trace point dm430 is set, dump the
	    ** buffer manager control blocks, once only.
	    ** Will probably happen once for each server 
	    ** in a shared cache environment.
	    */
	    dumped = TRUE; 
	    for (segment_ix = 0; segment_ix < DM_MAX_BUF_SEGMENT; segment_ix++)
	    {
		if ( (bmsegp = dmf_svcb->svcb_bmseg[segment_ix]))
		    dmd_fmt_cb(&dmd_param, (DM_OBJECT *) bmsegp);
	    }
	    dmd_fmt_cb(&dmd_param, (DM_OBJECT *) lbm);
	}
    } 
}


/*{
** Name: dm0p_page_lock_mode - figure out the page lock mode
**
** Description:
**     Calculates the lock mode required based on locking granularity
**     level, page type and transaction serializability.
**
**
**     Covering page lock (S or X) is required:
**         -  if page-level locking granularity is chosen;
**         -  for special page types (non-leaf Btree index,
**            fhdr, fmap);
**
**     To provide phantom protection, serializable reader should
**     also acquire S lock (or X lock, if readlock == exclusive):
**         -  on a Btree leaf page;
**         -  on a HEAP or ISAM page (except when fetching by tid
**            from base table);
**
**     In all other cases acquire an intended lock (IS or IX) on the
**     page instead of covering lock.
**
**     Exclusive or Shared lock mode depends on the access mode.
**
** Inputs:
**      rcb                             The RCB that wants the page lock.
**      action                          Fix actions used by caller.
**      access_mode                     The page access mode: read or write.
**
** Outputs:
**      lock_mode                       LK_IS | LK_IX | LK_S | LK_SIX | LK_X
**
** Returns:
**      None 
**
** Side Effects:
**      None 
**
** History:
**      22-nov-96 (dilma04, stial01)
**          Row Locking Project:
**          Created.
**      12-dec-96 (dilma04)
**          Remove unneeded check for base table;
**          Add dmd_check for debugging purposes.
**      12-feb-97 (dilma04)
**          Set Lockmode Cleanup:
**          Use rcb_iso_level instead of rcb_k_duration to check for 
**          serializable transaction. This fixes b79724.
**	20-Jul-1998 (jenjo02)
**	    When locking a newly allocated page (DM0P_CREATE | DM0P_CREATE_NOFMT)
**	    and row locking, lock the page X; dm1p will downgrade the lock to 
**	    LK_IX after the FHDR/FMAP updates have completed.
**	29-Oct-1998 (jenjo02)
**	    Added new action, DM0P_PHYSICAL_IX, which induces a physical IX
**	    lock when row locking instead of a physical X.
*/
VOID
dm0p_page_lock_mode(
    DMP_RCB     *rcb,
    i4     action,
    i4     access_mode,
    i4     *lock_mode)
{
    DMP_RCB             *r = rcb;
    DMP_TCB             *t = r->rcb_tcb_ptr;
    bool                phantom_protect_lock = FALSE;


    if (row_locking(r) && serializable(r) && (action & DM0P_PHANPRO) &&
        !(action & DM0P_USER_TID) && t->tcb_rel.relspec != TCB_HASH)
    {
        phantom_protect_lock = TRUE;
        if (t->tcb_rel.relspec != TCB_BTREE) 
        {
            /* If this is a HEAP or ISAM table, upgrade 
            ** locking granularity to page level. 
            */
            r->rcb_lk_type = RCB_K_PAGE;
        }
    }
 
    if ((r->rcb_lk_type == RCB_K_PAGE) || (action & DM0P_PHYSICAL) || 
         (phantom_protect_lock == TRUE) || (action & DM0P_LOCKREAD))
    {
        /* request a covering lock */
        *lock_mode = LK_S;
        if (access_mode == DM0P_WRITE)
        {
            if (row_locking(r) && action & (DM0P_PHANPRO | DM0P_PHYSICAL_IX))
	    {
                /* phantom protection for update or delete */
                *lock_mode = LK_SIX;
		/* get IX instead of X if requested */
		if (action & DM0P_PHYSICAL_IX)
		    *lock_mode = LK_IX;
	    }
            else
		if (r->rcb_state & RCB_UPD_LOCK)
		    *lock_mode = LK_U;
		else
		    *lock_mode = LK_X;
        }
    }
    else if (row_locking(r))
    {
        /* request an intended page lock */
	if (access_mode == DM0P_WRITE)
	{
	    /* If fixing a newly allocated/free page (dm1p), lock it X */
	    if (action & (DM0P_CREATE | DM0P_CREATE_NOFMT))
		*lock_mode = LK_X;
	    else
		*lock_mode = LK_IX;
	}
	else
	    *lock_mode = LK_IS;
    }
    else
        /*
        ** It is not expected for this to occur,
        ** dmd_check to see when this happens.
        */
        dmd_check(E_DM93F5_ROW_LOCK_PROTOCOL);
} 


/*{
** Name: dm0p_trans_page_lock
**
** Description:
**	Calculates the locking action required based upon the various
**	inputs.  Failure to obtain a page lock due to a resource error 
**	will potentially lead to table level lock escalation attempt.
**
**
** Inputs:
**      rcb                             The RCB that wants the page lock.
** 	page_number			Page to lock.
**	action				Fix actions used by caller.
**	lock_mode		        LK_IS | LK_IX | LK_S | LK_SIX | LK_X	
**
** Outputs:
**	lock_action			Lock action calculated and used
**					to lock the page.
**      lock_id                         Handle to a lock acquired on the page 
**      err_code                        Reason for error return.
**
** Returns:
**	E_DB_OK
**	E_DB_ERROR
**
** Side Effects:
**	    May escalate to table lock on relation identified by RCB.
**
** History:
**	13-jul-96 (nick)
**	    Created from code within dm0p_fix_page()
**	29-aug-96 (nick)
**	    Only print maxlocks exceeded message for user relations.
**      22-nov-96 (dilma04, stial01)
**          Row Locking Project:
**            - Added *tcb argument;
**            - Replaced access_mode argument with lock_mode which is now
**            calculated in dm0p_page_lock_mode(). 
**      06-jan-97 (dilma04)
**          Acquire a PHYSICAL IX lock on the page if row locking and
**          DM0P_NOESCALATE flag is set. This is needed to prevent 
**          bug 79611 situation which can occur because the intended
**          page locks are not always physical like it used to be 
**          before the optimization to release them was removed from 
**          dm0p_unfix_page. 
**      12-feb-97 (dilma04)
**          Set Lockmode Cleanup:
**          Test rcb_iso_level for isolation level.
**	26-feb-97 (dilma04)
**	    Lock escalation for row locking and lower isolation levels.
**	    - do not count physical and intend page locks for 'maxlock' limit;
**	    - allow lock escalation for cursor stability and repeatable read;
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          - added lock_id argument that is passed to dm0p_lock_page();
**          - if CS or RR, request any (not only shared) lock in physical mode.
**	03-Sep-1997 (jenjo02)
**	    Cross-integration of 427407. page_number in dm0p_1toss_page(),
**	    dm0p_trans_page_lock(), should be DM_PAGENO (u_i4), not i4.
**	22-Jul-1998 (jenjo02)
**	    Added fake LK_VALUE to dm0p_lock_page() call to allow callers
**	    of this function to convert SIX and higher locks to a lower mode.
**	29-Oct-1998 (jenjo02)
**	    PHYSICAL lock requests made non-interruptable.
**      15-apr-1999 (stial01)
**          dm0p_trans_page_lock() If row locking, do not count ANY page
**          locks for per table limit. (check_reclaim will request X page
**          locks)
**      16-Apr-1999 (hanal04)
**          If logging of lock escalation messages has been disabled we
**          must mimick ule_format()'s initialisation of err_code. b96165.
**	09-Sep-1999 (jenjo02)
**	    Check breach of RCB lock limit only if a new lock was acquired
**	    to avoid bogus lock escalations and secondary escalate deadlock 
**	    during transaction abort.
**	11-Oct-1999 (jenjo02)
**	    Test locally for security to avoid setup and call to 
**	    dma_write_audit().
**	24-Feb-2000 (devjo01)
**	    Corrected parameters to ule_format calls for E_DMA00D and
**	    E_DM004B.  (b100589).
**	14-Mar-2003 (jenjo02)
**	    Check (new) RCB_ABORTING flag, set by dmx_abort().
**	    If on, don't escalate (B109842).
**	30-Dec-2003 (jenjo02)
**	    Added DM0P_RCB_RELTID action flag. This is set
**	    when intent locking a page prior to setting a 
**	    row lock from within a Global secondary index.
**	    When this action is set, the rcb_reltid pointed
**	    to in the RCB will be used in the lock
**	    value instead of the TCB's reltid value.
**      22-sep-2004 (fanch01)
**          Remove meaningless LK_TEMPORARY flag from LKrequest call.
*/
DB_STATUS
dm0p_trans_page_lock(
    DMP_RCB	*rcb,
    DMP_TCB     *tcb,
    DM_PAGENO   page_number,
    i4     action,
    i4     lock_mode,
    i4     *lock_action,
    LK_LKID     *lock_id,
    i4     *err_code)
{
    DMP_RCB             *r = rcb;
    DMP_TCB             *t = tcb;
    DMP_DCB		*d = t->tcb_dcb_ptr;
    i4		new_lock = 0;
    bool		lock_resource_error = FALSE;
    bool		xact_abort;
    DB_STATUS		status;
    LK_VALUE		lk_value;
    i4			lock_limit_breached = FALSE;
    DB_TAB_ID		*reltid;

    /*
    ** Determine the lock action:
    **
    **     LK_STATUS	- return special status if granted lock is new.
    **			  This info is used to maintain the number of locks
    **			  held on this table. LK_STATUS has to be set only
    **			  if requested lock is counted for per table limit.
    **     LK_PHYSICAL	- short term physical lock - will be released when
    **	  		  the page is unfixed.
    **     LK_NOWAIT	- return error if the lock is busy 
    **     LK_TEMPORARY	- 
    **     LK_NONINTR	- 
    */
    *lock_action = LK_STATUS;

    if ((action & DM0P_PHYSICAL) || (r->rcb_k_duration & RCB_K_PHYSICAL))
    {
        *lock_action = (LK_PHYSICAL | LK_NONINTR);
    }
    else if (row_locking(r) && action & (DM0P_CREATE | DM0P_CREATE_NOFMT))
    {
        *lock_action = (LK_PHYSICAL | LK_NONINTR);
    }
    else if ((r->rcb_state & RCB_CSRR_LOCK) && (action & DM0P_LK_CONVERT) == 0)
    {
        *lock_action = (LK_PHYSICAL | LK_NONINTR);
    }
    else if (row_locking(r))
    {
	/*
	** request a logical lock, but do not count it for per table limit
	** Even if requesting covering X lock, as from check_reclaim
	*/
	*lock_action = 0;
    }

    *lock_action |= LK_QUIET_TOO_MANY;
    if (action & DM0P_NOWAIT)
        *lock_action |= LK_NOWAIT;
    if (r->rcb_k_duration & RCB_K_TEMPORARY)
        *lock_action |= LK_NONINTR;
    if (action & DM0P_IGNORE_LKLIMIT)
	*lock_action |= LK_IGNORE_LOCK_LIMIT;
 
    /* Initialize fake LK_VALUE */
    lk_value.lk_high = 0;
    lk_value.lk_low = 0;
    lk_value.lk_mode = LK_N;

    /*
    ** Request the page lock - if the request fails, check for lock
    ** quota/resource error.  In these cases we attempt to free up
    ** locks by escalating to table level.
    **
    ** "reltid" is normally this TCB's tcb_rel.reltid,
    ** but may be overriden when intent page-locking a data
    ** page in a Global Secondary Index (dm1r_lock_row()), in
    ** which case we must lock using the reltid of the indexed
    ** record. In that case, DM0P_RCB_RELTID has been set and 
    ** rcb_reltid points to the appropriate reltid.
    */
    if ( action & DM0P_RCB_RELTID )
	reltid = r->rcb_reltid;
    else
	reltid = &t->tcb_rel.reltid;

    status = dm0p_lock_page(r->rcb_lk_id, d, reltid,
		page_number, LK_PAGE, lock_mode, *lock_action, 
                r->rcb_timeout, &t->tcb_rel.relid, &r->rcb_tran_id, 
                lock_id, &new_lock, &lk_value, err_code);
    if (status != E_DB_OK)
    {
	/*
	** Figure out if we need to write an audit record here, if
	** lock escalation occurred. We write the record here since
	** dm0p_lock_page doesn't have the full context.
	*/
	if ( dmf_svcb->svcb_status & (SVCB_C2SECURE) )
	{
	    i4	msgid = 0;
	    switch(*err_code)
	    {
		case E_DM0042_DEADLOCK:
		    msgid = I_SX2737_PAGE_DEADLOCK;
		    break;
		case E_DM004D_LOCK_TIMER_EXPIRED:
		    msgid = I_SX2738_LOCK_TIMEOUT;
		    break;
		case E_DM004B_LOCK_QUOTA_EXCEEDED:
		case E_DM004C_LOCK_RESOURCE_BUSY:
		    msgid = I_SX2739_LOCK_LIMIT;
		    break; 
	    }
	    if (msgid != 0)
	    {
		(void)dma_write_audit(
			    SXF_E_RESOURCE,
			    SXF_A_FAIL | SXF_A_LIMIT,
			    (char*)&t->tcb_rel.relid,	/* Table name */
			    sizeof(t->tcb_rel.relid),	/* Table name */
			    &t->tcb_rel.relowner,
			    msgid,
			    FALSE, /* Not force */
			    err_code, NULL);
	    }
	}

	if (*err_code != E_DM004B_LOCK_QUOTA_EXCEEDED)
	    return (E_DB_ERROR);

	/*
	** If a lock resource error was returned, follow through to
	** lock escalation logic below.
	*/
	lock_resource_error = TRUE;
    }

    /*
    ** If the lock request call returned that this was a new
    ** transaction lock, then increment the number of locks held
    ** on this table.  Note that physical locks are not included
    ** in this count since LK will return LK_NEW_LOCK only on 
    ** logical locks.
    */
    if (new_lock)
    {
	if ( ++r->rcb_lk_count > r->rcb_lk_limit )
	    lock_limit_breached = TRUE;
    }

    /*
    ** If we have exceeded the number of page locks at which we should 
    ** escalate, or if we have exceeded one of the lock quotas, then 
    ** escalate from page to table level locking, provided that the
    ** DM0P_NOESCALATE flag is not set.  This allows us to free up page 
    ** lock resources.
    **
    ** Note - we do not want to escalate lock levels while processing
    ** transaction recovery as this can lead to deadlock (which is
    ** bad during recovery). 
    **
    ** Thus, during transaction recovery we distinguish between "hard"
    ** limits (imposed by the CL/System) and "soft" limits
    ** (rcb_lk_limit). If transaction recovery encounters a hard limit,
    ** it has no recourse but to return an error, and either pass-abort
    ** to the RCP or mark the DB inconsistent (if this is already the
    ** RCP). However, the soft limits need not be enforced during
    ** transaction recovery. We expect that in normal cases a
    ** transaction will not lock objects which it doesn't already have
    ** locked, but even if it does, that should not cause us to return
    ** an error. That is, we do not enforce rcb_lk_limit during
    ** transaction recovery (B44260).
    **
    ** If hard lock limits are exceeded during recovery processing, we
    ** return a QUOTA error rather than attempting to escalate.  This
    ** will cause the abort processing to be passed over to the RCP for
    ** completion.  The RCP should never get hard lock-quota errors
    ** during abort processing.
    */
    if (action & DM0P_NOESCALATE)
    {
	if (lock_resource_error)
	{
	    /* 
	    ** We have hit a resource "hard" limit but lock escalation is
	    ** not allowed -- return an error. 
	    */
	    ule_format(E_DM004B_LOCK_QUOTA_EXCEEDED, (CL_SYS_ERR *)0, 
		    ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0,
		    err_code, 0);
	    *err_code = E_DM004B_LOCK_QUOTA_EXCEEDED;
	    status = E_DB_ERROR;
	}
    }
    else
    {
	if ( lock_resource_error || lock_limit_breached )
	{
	    /*
	    ** ESCALATE
	    **
	    ** Either no more locks available, or limit on this table
	    ** has been exceeded, so escalate to table level locking.
	    */

	    /*
	    ** Check the transaction state to avoid escalation during
	    ** transaction recovery.  
	    **
	    ** We assume that any work done inside the RCP or CSP is 
	    ** recovery related.  We make this assumption because we
	    ** cannot do a dmxe_check_abort on a transaction being aborted
	    ** by the RCP/CSP because the true log_id for the recovered
	    ** session is not known.
	    **
	    ** Some DMF utilities allocate RCB's without filling in the
	    ** rcb_log_id field - preventing us from checking the
	    ** transaction state.  We currently just assume that these
	    ** are not undergoing abort processing.
	    */
	    xact_abort = FALSE;

	    if ( dmf_svcb->svcb_status & (SVCB_RCP | SVCB_CSP) ||
		 r->rcb_state & RCB_ABORTING )
	    {
		xact_abort = TRUE;
	    }
	    else if (r->rcb_log_id)
	    {
		/*
		** Not a Recovery Process and the transaction handle
		** is known, so query the state of the tranaction.
		** When the transaction status is unknown (error status
		** returned) we will assume that it is not aborting and
		** attempt the lock escalation.
		*/
		dmxe_check_abort(r->rcb_log_id, &xact_abort);

		/* Remember in RCB for next time */
		if ( xact_abort )
		    r->rcb_state |= RCB_ABORTING;
	    }

	    if ((xact_abort) && (lock_resource_error))
	    {
		/* "hard" limit -- forced to return an error */
		ule_format(E_DM9C08_OUT_OF_LOCKS, (CL_SYS_ERR *)0, ULE_LOG,
				NULL, (char *)0, (i4)0, (i4 *)0, 
				err_code, 0);
		*err_code = E_DM004B_LOCK_QUOTA_EXCEEDED;
		status = E_DB_ERROR;
	    }
	    else if (xact_abort == FALSE)
	    {
		bool    log_esc = FALSE;
		i4	esc_flags = 0;

		if ( lock_limit_breached )
		{
		    /* maxlocks on resource exceeded */
		    if ((t->tcb_rel.relstat & TCB_CATALOG) != 0)
		    {
			if ((DMZ_LCK_MACRO(5)) || 
			    (dmf_svcb->svcb_log_err & SVCB_LOG_LPR_SC))
			    log_esc = TRUE;
		    }
		    else /* user table */
		    {
			if ((DMZ_LCK_MACRO(6)) || 
			    (dmf_svcb->svcb_log_err & SVCB_LOG_LPR_UT))
			    log_esc = TRUE;
		    }
		    if (log_esc)
		    {
			ule_format(E_DM9063_LK_TABLE_MAXLOCKS, (CL_SYS_ERR *)0, 
			    ULE_LOG, NULL, (char *)0, (i4)0, 
			    (i4 *)0, err_code, 4, 
			    sizeof(DB_TAB_NAME), &t->tcb_rel.relid, 
			    sizeof(DB_DB_NAME), &d->dcb_name,
			    0, r->rcb_lk_count,
			    0, r->rcb_lk_limit);
		    }
                    /* b96165 */
                    else
                    {
                        *err_code = E_DB_OK;
                    }
		}
		else
		{
		    if ((t->tcb_rel.relstat & TCB_CATALOG) != 0)  /* sys cat */
		    {
			if ((DMZ_LCK_MACRO(7)) || 
			    (dmf_svcb->svcb_log_err & SVCB_LOG_LPT_SC))
			    log_esc = TRUE;
		    }
		    else /* user table */
		    {
			if ((DMZ_LCK_MACRO(8)) || 
			    (dmf_svcb->svcb_log_err & SVCB_LOG_LPT_UT))
			    log_esc = TRUE;
		    }
		    if (log_esc)
		    {
			LK_LLB_INFO     info_block;
			u_i4           result_size;
			CL_ERR_DESC	sys_err;

			status = LKshow(LK_S_LIST, r->rcb_lk_id, 
					(LK_LKID *)NULL, (LK_LOCK_KEY *)0, 
					sizeof(info_block), (PTR)&info_block,
					&result_size, (u_i4 *)NULL, &sys_err);

			ule_format(E_DMA00D_TOO_MANY_LOG_LOCKS, 
			   (CL_ERR_DESC *)0, ULE_LOG, NULL, (char *)0, 0L, 
			   (i4 *)0, err_code, 4, 0, 0, 0, 
			   *lock_action, 0, info_block.llb_max_lkb, 0, 
			   info_block.llb_max_lkb);

			ule_format(E_DM004B_LOCK_QUOTA_EXCEEDED, &sys_err, 
				ULE_LOG, NULL, (char *)0, (i4)0, 
				(i4 *)0, err_code, 0);

		        /* locks per txn */
		        ule_format(E_DM9041_LK_ESCALATE_TABLE, (CL_SYS_ERR *)0, 
		    	    ULE_LOG, NULL, (char *)0, (i4)0, 
			    (i4 *)0, err_code, 2, 
			    sizeof(DB_TAB_NAME), &t->tcb_rel.relid, 
			    sizeof(DB_DB_NAME), &d->dcb_name);
		    }
                    /* b96165 */
                    else
                    {
                        *err_code = E_DB_OK;
                    }
		}
		if (action & DM0P_NOWAIT)
		    esc_flags = DM0P_NOWAIT;
		status = dm0p_escalate(r, esc_flags, err_code);
	    }

	    /*
	    ** The "fall-through" case is: 
	    ** xact_abort && (cl_status != LK_NOLOCKS)
	    ** We just allow this lock request and keep on processing,
	    ** hoping that a transaction which is undergoing abort
	    ** processing makes only a "small" number of these lock
	    ** requests (if it makes a large number, eventually we'll 
	    ** hit a hard limit and return an error) (B44260).
	    */
	}
    }
    return(status);
}


/*
** Name: dm0p_dmcm_callback - callback function for DMCM related lock requests.
**
** Description:
**   Under the DMCM protocol, at the end of a transaction, 'dirty' cached 
**   pages are retained with their original lock; 'clean' pages are retained
**   with a null lock.
**   When another session requires that page, the LKrequestWithCallback 
**   call in dm0p_cachefix_page/fault_page will cause this routine to be 
**   called if a locking contention has occurred.
**   The desired page will either be written and dropped or simply dropped
**   from the cache depending upon what actions were previously performed on
**   it.
** 
**   The complexity of the callback stems from the fact that it can be
**   scheduled to be called at any point after a contending cache lock
**   request occurs. 
**   Secondly, if the cache page is dirty, we have to fix the appropriate
**   TCB, to enable a write to be performed. But to do this, we have to
**   release the buffer manager mutex.
**
**   These two features mean that on executing the callback, we have to
**   ensure that the buffer is still in a state to enable execution. 
**
**   That is,
**   1) The buffer data (page etc.) has not changed.
**   2) The buffer is valid.
**   3) The buffer is locked.
**
**   If any of these three checks fail, there is nothing for the callback
**   to do - except, if necessary, unfix the tcb its just fixed.
**
** Inputs:
**   lock_mode - the mode of lock requested by the caller who is contending with
**               the current cache page lock.
**   ptr       - NOT used.
**   lockid    - the LK identifier of the resource on which the locking
**               contention has occurred.
**
** Outputs:
**   None.
**
** Returns:
**	OK	- page is written out if modified.  Cache lock is converted
**		  downward.
**	E_DB_INFO - page was marked with BUF_RBUSY.  This could be due
**		to either the page being faulted in, or a validate being
**		performed in another session.  Current lock state on this
**		page cannot be reliably determined.   This status indicates
**		that the operation should be tried again after a pause for
**		the BUF_RBUSY to clear.
**	E_DB_WARN - callback was "stale".  Page has either been recycled,
**		or the cache lock has already been downgraded.  Callback
**		can safely be ignored.
**	E_DB_ERROR - Fatal failure of routine.  We actually call dmd_check
**		instead of returning this code, as not handling a callback
**		is fatal to the server.
**
** History:
**      01-aug-1996 (nanpr01 for ICL phil.p)
**        Created.
**	01-aug-1996 (nanpr01)
**	  Put the has_bucket calculation & LKshow call inside the loop.
**	  Reinitialize found inside the loop.
**	  Also initialize the cache_op stack variable accoring to the
**	  dmcm protocol.
**	  Added lock_key parameter in the routine since lockid by itself
**	  can be reused and causes confusion and dmd_check error.
**	19-aug-1996 (nanpr01)
**	    Also it may be likely that the lock has been dropped when 
**	    this routine is called. Also is is possible that the other
**	    server has gotten the lock and this server has requested it
**	    and is waiting for the other server to give up the resource
**	    when this routine is called.
**	    If the buffer belongs to group buffer do no bother. Just call
**	    force_page and it will take care of it.
**	01-Nov-1996 (jenjo02)
**	    Altered code to fit with new semaphore scheme.
**	10-Dec-1996 (nanpr01)
**	    Inadvertently dropped code was added back.
**	08-Apr-1997 (jenjo02)
**	  - Use new LK_S_SLOCKQ function to silence LK errors if lockid
**	    can't be found (a "normal" condition).
**	  - If fix_tableio_ptr() returns OK without waiting and releasing
**	    the buffer's mutex, there's no need to rescan and recheck the buffer.
**	  - Check this server's locklist id against that returned by LKshow()
**	    to close a window whereby the lock info matches in every other 
**	    respect.
**	  - If lockid or buffer is stale, return E_DB_WARN so lk_callback()
**	    can track statistics.
**	19-Jan-1998 (jenjo02)
**	    Modified to find buffer using (new) table hash chain instead of 
**	    searching buffer hash in every cache.
**	20-aug-2002 (devjo01) b108551
**	    Call force_page with new evcomp value BMCL_DMCM_CBACK from
**	    dm0p_dmcm_callback in order to turn off the BUF_MUTEX & 
**	    BUF_MUTEX_OK bits dm0p_dmcm_callback asserted.
**	24-May-2005 (jenjo02)
**	    Remove lock list id compare when validating LK_S_LOCKQ; the
**	    one returned from LKshow will be the list id on which the
**	    lock resides - in a shared cache environment, this won't be
**	    the same as lbm_lock_list. Instead we rely on the fact that
**	    it's only the BM that makes lock requests for LK_BM_PAGE locks,
**	    hence the list id is of no import.
**	03-Oct-2005 (fanch01)
**	    BUG115322.  Test for buf_lock_mode == LK_N could peek inside a
**	    busy buffer.  This causes locks to incorrectly be ignored and
**	    required callbacks to be dropped which later causes an internal
**	    deadlock.  Simply move the mode test after the busy test.
**	17-oct-2005 (devjo01) b114660
**	    Under highly concurrent cirucumstances in a cluster, it is
**	    possible for a stale callback to see a BUF_RBUSY set by
**	    dm0p_validate.  This would stall the callback thread, and in
**	    the worst case cause a scheduling deadlock where the holder
**	    of the page cache lock on the other node could not release
**	    the fatal page (typically a map page) because a blocking
**	    notification to the stalled callback thread could not be
**	    processed.
*/
DB_STATUS
dm0p_dmcm_callback( 
      i4         	lock_mode,
      PTR        	ptr,      
      LK_LKID    	*lockid,
      LK_LOCK_KEY	*lock_key)
{
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BMCB       *bm;
    DB_STATUS       status;
    DM0P_BUFFER     *b;
    DMP_TABLE_IO    *tbio = NULL;
    i4         err_code;
    i4         log_id;
    CL_SYS_ERR      sys_err;
    u_i4           length;
    u_i4           context = 0;
    LK_LKB_INFO     info_block;
    i4	    lk_type  = lock_key->lk_type;
    i4         db_id    = lock_key->lk_key1;
    i4         table_id = lock_key->lk_key2;
    i4         index_id = lock_key->lk_key3;
    i4         page_no  = lock_key->lk_key4;
    DM0P_BLQ_HEAD   *tab_bucket;
    DM0P_BID	    next_bid;
    i4		    bucket;
    DM0P_BSTAT	    *lbm_stats;

    LK_CALLBACK_TRACE( 30, lock_key );

    /* Discover the table's hash chain root */
    bucket = (db_id + (u_i4)table_id) & (BMCB_TABLE_MAX - 1);
    tab_bucket = &bm_common->bmcb_tabq_buckets[bucket];

    for (;;)
    {
	/*
	** Use lockid, to find resource info. and use this
	** to scan the hash chain for the right buffer.
	*/
	status = LKshow(LK_S_SLOCKQ, (i4) 0, lockid, (LK_LOCK_KEY *) 0,
			    sizeof(info_block), (PTR) &info_block, &length,
			    &context, &sys_err);

	/*
	** If the LKshow fails, we assume it is because the lock that
	** triggered the callback no longer exists. In this case, or
	** if the current occupant of "lockid" no longer matches, there
	** is nothing for the callback to do.
	*/
	if (status != OK ||
	    info_block.lkb_key[0] != lk_type  ||
	    info_block.lkb_key[1] != db_id    ||
	    info_block.lkb_key[2] != table_id ||
	    info_block.lkb_key[3] != index_id ||
	    info_block.lkb_key[4] != page_no)
	{
	    LK_CALLBACK_TRACE( 31, lock_key );
	    return(E_DB_WARN);                                         
	}

	/* reinitialize the err_code */
	err_code = E_DB_OK;
	b = NULL;

	/*
	** Search the table's hash chain for matching buffer.
	*/
	next_bid = tab_bucket->blqh_bid;

	while (next_bid.bid_index != BUF_ENDLIST)
	{
	    b = &lbm->lbm_buffers[next_bid.bid_cache][next_bid.bid_index];

	    if (b->buf_lock_key.lk_type != lk_type  ||
		b->buf_lock_key.lk_key4 != page_no  ||
		b->buf_lock_key.lk_key1 != db_id    ||
		b->buf_lock_key.lk_key2 != table_id ||
		b->buf_lock_key.lk_key3 != index_id ||
		b->buf_tabq_bucket != bucket)
	    {
		if (b->buf_tabq_bucket == bucket)
		    next_bid = b->buf_tabq.blq_next;
		else
		    next_bid = tab_bucket->blqh_bid;
		b = NULL;
		continue;
	    }

	    /*
	    ** Lock buffer, recheck key.
	    */
	    dm0p_mlock(SEM_EXCL, &b->buf_mutex);

	    if (b->buf_lock_key.lk_type == lk_type  &&
		b->buf_lock_key.lk_key4 == page_no  &&
		b->buf_lock_key.lk_key1 == db_id    &&
		b->buf_lock_key.lk_key2 == table_id &&
		b->buf_lock_key.lk_key3 == index_id &&
		b->buf_tabq_bucket == bucket)
	    {
		break;
	    }
	    if (b->buf_tabq_bucket == bucket)
		next_bid = b->buf_tabq.blq_next;
	    else
		next_bid = tab_bucket->blqh_bid;
	    dm0p_munlock(&b->buf_mutex);
	    b = NULL;
	}

	/*
	** If the buffer is no longer in the cache, give up.
	*/ 
	if (b == NULL)
	{
    	    /*
    	    ** Fatal error if do not find matching buffer.
    	    */
	    /* it is also likely that by the time this callback routine was
	    ** invoked, lock may have been dropped by the thread (to make room
	    ** for other buffer and some other session in the same server 
	    ** requested the page and waiting on the cache lock ...
            */
	    if (info_block.lkb_grant_mode < info_block.lkb_request_mode)
	    {
		LK_CALLBACK_TRACE( 32, lock_key );
	        return(E_DB_WARN);
	    }

	    /* between the time LKshow was called and buffer manager is mutexed
	    ** it is possible that lock may have been dropped in a highly
	    ** concurrent situation. Just check and make sure that did not
	    ** happen. Specially with native threads this will be 
	    ** definitely happen.
	    */
	    status = LKshow(LK_S_SLOCKQ, (i4) 0, lockid, (LK_LOCK_KEY *) 0,
			sizeof(info_block), (PTR) &info_block, &length,
			&context, &sys_err);
 
	    /*
	    ** If the LKshow fails, we assume it is becasue the lock that
	    ** triggered the callback no longer exists. In this case, or if
	    ** the current occupant of "lockid" no longer matches, there
	    ** is nothing for the callback to do.
	    */
	    if (status != OK ||
	        info_block.lkb_key[0] != lk_type  ||
	        info_block.lkb_key[1] != db_id    ||
	        info_block.lkb_key[2] != table_id ||
	        info_block.lkb_key[3] != index_id ||
	        info_block.lkb_key[4] != page_no)
	    {
		LK_CALLBACK_TRACE( 33, lock_key );
                return(E_DB_WARN);
	    }

	    if (check_buffer(lockid, lock_key))
	    {
		LK_CALLBACK_TRACE( 34, lock_key );
		continue;
	    }

            ule_format(E_DM93F4_NO_BUFFER, (CL_SYS_ERR *)0, ULE_LOG,
                NULL, (char *)0, (i4)0, (i4 *)0, &err_code, 4,
                0, db_id, 0, table_id, 0, index_id, 0, page_no);

	    dmd_check(E_DM93F4_NO_BUFFER);
	}

	bm = lbm->lbm_bmcb[b->buf_id.bid_cache];
	lbm_stats = &lbm->lbm_stats[b->buf_id.bid_cache];

	/*
	** (ICL phil.p)
	** Buffer currently being read/write by another thread?
	** Wait. When woken, need to research the queues.
	**
	** (devjo01) Logic here was changed to instead return a
	** distinct code back to callback manager.  Callback is
	** then put back on the end of the queue and will be
	** tried again later.  Blocking here led to a fiendish
	** scheduling deadlock under clusters, where a local session
	** entering dm0p_validate would mark the buffer RBUSY then
	** block on a cache lock held on a remote node.  Remote node
	** would not release the lock because it was waiting for
	** a cache lock which could not be granted because the local
	** callback thread for the local node was blocked on the
	** RBUSY.  
	**
	** N.B. Even though I no longer wait for the assumed
	** I/O event, statistics are still incremented as before.
	*/
	if (b->buf_sts & BUF_BUSY)
	{
	    lbm_stats->bs_iowait[BMCB_PTYPES]++;
	    bm->bm_stats.bs_iowait[BMCB_PTYPES]++;
	    lbm_stats->bs_iowait[b->buf_type]++;
	    bm->bm_stats.bs_iowait[b->buf_type]++;
	    LK_CALLBACK_TRACE( 35, lock_key );
	    dm0p_munlock(&b->buf_mutex);

	    /*
	    ** Don't wait for the "I/O" event to complete.
	    ** Return to caller with distinct status.
	    */
	    return(E_DB_INFO);
	}

	/* 
	** We may get the callback after we have released the lock
	** Return if cache lock is LK_N (b114660)
	**
	** fanch01 - Make sure we do this *after* the busy test.
	** (b115322)
	*/
	if (b->buf_lock_mode == LK_N)
	{
	    LK_CALLBACK_TRACE( 36, lock_key );
	    dm0p_munlock(&b->buf_mutex);
	    return (E_DB_WARN);
	}

	/*
	** If page is mutex locked, then wait for the mutex to be
	** released and try again with the same buffer.
	*/
	if (b->buf_sts & BUF_MUTEX)
	{
	    lbm_stats->bs_mwait[BMCB_PTYPES]++;
	    bm->bm_stats.bs_mwait[BMCB_PTYPES]++;
	    lbm_stats->bs_mwait[b->buf_type]++;
	    bm->bm_stats.bs_mwait[b->buf_type]++;
	    b->buf_sts |= BUF_MWAIT;
	    dm0p_eset(lbm->lbm_dmcm_lock_list, DM0P_PGMUTEX_EVENT,
			b->buf_id.bid_cache, b->buf_id.bid_index, 0);
	    LK_CALLBACK_TRACE( 37, lock_key );
	    dm0p_munlock(&b->buf_mutex);

	    /* Wait for the unmutex event. */
	    dm0p_ewait(lbm->lbm_dmcm_lock_list, DM0P_PGMUTEX_EVENT,
			b->buf_id.bid_cache, b->buf_id.bid_index, 0, 0);
	    continue;
	}

	/*
	** While waiting for the buffer or for its semaphore,
	** the lock which invoked this callback may have
	** been resolved or dropped, or the buffer's contents may
	** no longer be valid.
	** In either case, ignore this callback.
	*/
	if ((b->buf_sts & BUF_VALID) == 0 ||
	     (b->buf_sts & BUF_LOCKED) == 0 ||
	      b->buf_lk_id.lk_unique != lockid->lk_unique ||
	      b->buf_lk_id.lk_common != lockid->lk_common)
	{
	    LK_CALLBACK_TRACE( 42, lock_key );
	    dm0p_munlock(&b->buf_mutex);
	    return(E_DB_WARN);
	}

	/* 
	** We cannot guarantee when the callback will be triggered.
	** We could still be in the middle of a transaction and the
	** callback has been called because there isn't a data page
	** lock to protect the cache page (DM0P_READNOLOCK).
	** In this scenario we mark the page as BUF_TUNFIX and let 
	** the existing (non-DMCM) code write/toss it on the unfix 
	** at the end of the transaction, but leave BUF_DMCM on.
	*/ 

	/*
	** If unfixed, write the page if modified, reduce cache lock to LK_N
	**
	** If fixed for Write, or fixed for Read and cache lock is not
	** too strong, mark page BUF_TUNFIX, leave BUF_DMCM on, 
	** and let the existing (non-DMCM) code write/toss it on the 
	** unfix at the end of transaction.
	**
	** If fixed for Read and we hold a stronger cache lock than we need, 
	** write the page if modified, and reduce cache lock to LK_S.
	** (see complete_f_p()).
	** 05-Feb-1999 (jenjo02)
	*/

	if (b->buf_sts & BUF_FIXED)
	{ 
	    if (b->buf_sts & BUF_WRITE ||
		b->buf_lock_mode <= LK_S)
	    {
		b->buf_sts |= BUF_NULL_TUNFIX;
		LK_CALLBACK_TRACE( 38, lock_key );
		dm0p_munlock(&b->buf_mutex);
		return(E_DB_OK);
	    }
	}

	/*
	** Get a TBIO if page is modified.
	*/
	if (b->buf_sts & BUF_MODIFIED)
	{
	    status = fix_tableio_ptr(b, (DM0P_TBIO_WAIT | DM0P_TBIO_MUTEX_HELD), 
			lbm->lbm_dmcm_lock_list, (i4)0,
			&tbio, &err_code);

	    /*
	    ** If the TCB was found and no wait occurred, buf_mutex was
	    ** not released, so its status hasn't changed.
	    */
	    if (status != E_DB_OK)
	    {
		/*
		** If the TCB was busy, buf_mutex was released, we must retry.
		** buffer's mutex unlocked by fix_tableio_ptr().
		*/
		if (status == E_DB_WARN)
		{
		    LK_CALLBACK_TRACE( 39, lock_key );
		    continue;
		}

		/*
		** The TCB was not found in the cache.
		**
		** Protocol Violation.
		** Page is dirty, but its TCB does not reside in the cache.
		*/
		dmd_check(E_DM93F2_NO_TCB);
	    }
	}
	break;
    }

    log_id = 0;
    
    /*
    ** Turn on mutex on this page to protect it from being modified
    ** while its being written. Also set mutex_ok to indicate that
    ** page may be busy even though it may not be fixed.
    */
    b->buf_sts |= (BUF_MUTEX | BUF_MUTEX_OK);

    /*
    ** If buffer manager was busy, retry.
    */
    do
    {
        status = force_page(b, tbio, FP_NULL, lbm->lbm_dmcm_lock_list, 
                            log_id, BMCL_DMCM_CBACK, (i4*)0, &err_code);
    } while (status == E_DB_WARN);

    /*
    ** unfix table i/o ptr 
    */
    if (tbio != NULL)
    {
        unfix_tableio_ptr(&tbio, lbm->lbm_dmcm_lock_list, (i4)0, (i4)__LINE__);
    }


    /*
    ** If the force page failed, don't return 
    ** until after the tbio ptr has been unfixed.
    */
    if (status != E_DB_OK)
    {
	LK_CALLBACK_TRACE( 40, lock_key );
	dm0p_munlock(&b->buf_mutex);
        return(err_code);
    }

    LK_CALLBACK_TRACE( 41, lock_key );
    return(E_DB_OK);
}

/*
** Name: dmcm_cache_flush - flush cache when closing server running DMCM.
**
** Description:
**
**   	Distributed Multi-Cache Management (DMCM) is effectively Fast_Commit
**   	in a Multi_Cache environment. As cached pages are not written at the
**   	end of a transaction, it is possible for a server's cache to still
**   	contain locked pages (possbily unwritten) when the server is shut
**   	down.
**   	This routine is called by dm0p_deallocate - prior to deallocating
**   	the cache - to flush all the pages still locked.
**   	No fixed, busy, or mutexed pages are expected.
**   	Still-locked pages will be force_page'd with the FP_TOSS flag.
**
** Inputs:
**
** Outputs:
**   None.
**
** Returns:
**   E_DB_OK			Successful completion.
**   E_DB_WARN			Retry operation.
**   E_DB_ERROR
**   E_DB_FATAL			Fatal error.
**
** History:
**      01-aug-1996 (nanpr01 for ICL phil.p)
**          Created.
**          (Actually this is a copy and simplification of dm0p_cp_flush).
**	01-Nov-1996 (jenjo02)
**	    Altered code to fit new semaphore scheme.
**	23-Apr-1997 (jenjo02)
**	    Removed buf_pid and references to it. In DMCM, this is a
**	    private cache and can only be locked or referenced by this
**	    server. Fixed, busy, or mutexed pages should never be encountered.
**	    Should this function be exposed to a server crash, this might
**	    have to change.
**	28-Nov-2005 (devjo01)
**	    Enhancements for multi-server multi-cache DMCM.  
*/
static DB_STATUS
dmcm_cache_flush(
i4		*err_code)
{
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    i4              cache_ix;
    DM0P_BMCB       *bm;
    DM0P_BUFFER     *buf_array;
    DMP_TABLE_IO    *tbio = 0;
    DM0P_BUFFER	    *buffer;
    DM0P_BSTAT	    *lbm_stats;
    DB_STATUS       status = E_DB_OK;
    PID		    tosspid = 0;
    i4              i;

    if ( NULL == bm_common )
    {
	/* Buffer manager not fully initialized? */
	return status;
    }

    if ( ((bm_common->bmcb_status & (BMCB_SHARED_BUFMGR|BMCB_CLUSTERED)) ==
	  (BMCB_SHARED_BUFMGR|BMCB_CLUSTERED)) &&
         !CXconfig_settings( CX_HAS_DLM_GLC ) )
    {
	/*
	** We are running in a cluster, and the underlying DLM does not
	** have a GLC (group lock container).  This means that the distributed
	** locks corresponding to the Ingres locks will be implicitly
	** released if the process should exit.  This is not an issue
	** with shared cache if the process should crash, since if a server
	** attached to a shared cache should crash, all bets are off, and
	** ALL servers attached to the cache must also crash.
	**
	** On a clean shutdown however, we must sacrifice cached pages
	** whose DLM locks are owned by the disconnecting process.  The
	** alternative of having shared BM_PAGE locks allocated and owned
	** by the RCP was rejected, since the significant overhead of
	** making the passing lock requests to the RCP/CSP on an ongoing
	** basis would typically far outweight the occasional hit of
	** uncaching a portion of the pages.
	**
	** The VMS cluster is the only configuration laboring under this
	** limitation.
	*/
	tosspid = lbm->lbm_pid;
    }
    else if ( bm_common->bmcb_srv_count > 1 )
    {
	/* Not the last process connected to server, OK to do nothing. */
	return status;
    }

    for (cache_ix = 0; DB_SUCCESS_MACRO(status) &&
                       cache_ix < DM_MAX_CACHE; cache_ix++)
    {
	bm = lbm->lbm_bmcb[cache_ix];
	if (!bm || !(bm->bm_status & BM_ALLOCATED))
	    continue;
 
	buf_array = lbm->lbm_buffers[cache_ix];
	lbm_stats = &lbm->lbm_stats[cache_ix];

	for (i = 0; DB_SUCCESS_MACRO(status) && i < bm->bm_bufcnt; i++)
	{
	    buffer = &buf_array[i];
       
	    if ( tosspid )
	    {
		if ( tosspid == buffer->buf_pid &&
		     (buffer->buf_sts & BUF_VALID) )
		{
		    /* Lock the buffer, then check again */
		    dm0p_mlock(SEM_EXCL, &buffer->buf_mutex);

		    if (tosspid != buffer->buf_pid ||
		        !(buffer->buf_sts & BUF_VALID))
		    {
			dm0p_munlock(&buffer->buf_mutex);
			continue;
		    }

		    /*
		    ** We must look up the TBIO pointer in order to toss
		    ** a modified page since force_page will need it to 
		    ** do the write.
		    */
		    if (buffer->buf_sts & BUF_MODIFIED)
		    {
			/* If we already have a TBIO, try to use it */
			if (!tbio ||
			    (buffer->buf_dbid != tbio->tbio_dcbid ||
			     buffer->buf_tabid.db_tab_base !=
			      tbio->tbio_reltid.db_tab_base ||
			     buffer->buf_tabid.db_tab_index != 
			      tbio->tbio_reltid.db_tab_index))
			{
			    /* Wrong table, release buffer, TBIO, retry */
			    if ( tbio )
			    {
				dm0p_munlock(&buffer->buf_mutex);
				unfix_tableio_ptr(&tbio,
				  lbm->lbm_dmcm_lock_list, (i4)0, __LINE__);
				i--;
				continue;
			    }

			    /* 
			    ** Look up the TCB for this buffer.
			    */
			    status = fix_tableio_ptr(buffer, 
				(i4)(DM0P_TBIO_WAIT | DM0P_TBIO_MUTEX_HELD), 
				lbm->lbm_dmcm_lock_list, (i4)0, &tbio, 
				err_code);

			    if (status == E_DB_WARN)
			    {
				/*
				** We had to wait for a busy Table Descriptor.
				** Retry the operation on the same buffer.
				** 
				** Buffer's mutex unlocked by fix_tableio_ptr().
				*/
				status = OK;
				i--;
				continue;
			    }

			    if (DB_FAILURE_MACRO(status))
			    {
				ule_format( *err_code, (CL_SYS_ERR *)0,
				 ULE_LOG, NULL, (char *)0, (i4)0, (i4 *)0,
				 err_code, 0);
			    }
			    if ((status != E_DB_OK) || (tbio == NULL))
			    {
				dm0p_munlock(&buffer->buf_mutex);

				/* Fatal */
				break;
			    }
			}
			    
			/* Count another force */
			lbm_stats->bs_force[BMCB_PTYPES]++;
			bm->bm_stats.bs_force[BMCB_PTYPES]++;
			lbm_stats->bs_force[buffer->buf_type]++;
			bm->bm_stats.bs_force[buffer->buf_type]++;
		    }

		    /*
		    ** Toss this page.
		    ** The TBIO pointer may be NULL if the page is not modified.
		    */
		    status = force_page(buffer, tbio, FP_TOSS,
		     lbm->lbm_dmcm_lock_list, (i4)0,
		     BMCL_NO_GATHER, (i4*)0, err_code);

		    /*
		    ** Check for retry needed.  Force_page will return a 
		    ** warning if the buffer has an outstanding I/O on it 
		    ** (it is being forced out by another thread).
		    */
		    if ( status )
		    {
			if (status == E_DB_WARN)
			{
			    /* Retry with the same buffer */
			    i--;
			    status = OK;
			}
			dm0p_munlock(&buffer->buf_mutex);
		    }
		}
	    }
	    else
	    {
		/*
		** Is buffer modified, but not busy, mutexed, or fixed?
		**
		** If more than just modified, something's wrong, so
		** don't write the buffer. This should never happen
		** during normal server shutdown, but we'll check for it just
		** in case to keep from writing a questionable page.
		*/ 
		if (buffer->buf_mutex.bms_locked == 0 &&
		   (buffer->buf_sts & 
		    (BUF_MODIFIED | BUF_BUSY | BUF_MUTEX | BUF_FIXED))
			== (BUF_MODIFIED))
		{
		    dm0p_mlock(SEM_EXCL, &buffer->buf_mutex);

		    /* Check again after semaphore wait,
		       which should never happen */
		    if ((buffer->buf_sts &
		       (BUF_MODIFIED | BUF_BUSY | BUF_MUTEX | BUF_FIXED))
			!= (BUF_MODIFIED))
		    {
			dm0p_munlock(&buffer->buf_mutex);
			continue;
		    }

		    status = fix_tableio_ptr(buffer, 
			(i4)(DM0P_TBIO_WAIT | DM0P_TBIO_MUTEX_HELD), 
			lbm->lbm_dmcm_lock_list, (i4)0, &tbio, err_code);

		    if (status == E_DB_WARN)
		    {
			/*
			** We had to wait for a busy Table Descriptor.
			** Retry the operation on the same buffer.
			** This should never happen.
			** buffer's mutex unlocked by fix_tableio_ptr().
			*/
			i--;
			continue;
		    }

		    status = force_page(buffer, tbio, FP_TOSS,
			lbm->lbm_dmcm_lock_list, (i4)0, BMCL_NO_GATHER, 
			(i4*)0, err_code);
	      
		    unfix_tableio_ptr(&tbio, lbm->lbm_dmcm_lock_list,
		       (i4)0, (i4)__LINE__);

		    if ( status )
			dm0p_munlock(&buffer->buf_mutex);
		}
	    }
	}
    }

    return(E_DB_OK);
}

/*
** Name: dm0p_count_pages
**
** Description:
**   Count the number of dirty pages for the table identified by table_id,
**   that currently reside in the cache.
**   Used by the TCB callback routine dm2t_tcb_callback, to ensure that if
**   a TCB is flushed from the cache, no dirty pages for that table are left
**   behind.
**
** Inputs:
**   Database and table identifiers.
**
** Outputs:
**   None.
**
** Returns:
**   Count of number of cached pages for the table concerned.
**
** History:
**      01-aug-1996 (nanpr01 for ICL phil.p)
**          Created.
**      01-aug-1996 (nanpr01)
**	    Loop through all buffers for matching table.
**	01-Nov-1996 (jenjo02)
**	    Added mutex protections of buffers.
**	23-Apr-1997 (jenjo02)
**	    Removed check of buf_pid vs server pid, which
**	    is nonsensical in a private cache.
**	21-Jan-1998 (jenjo02)
**	    Converted sequential all-cache scan to table hash
**	    list scan.
**	23-Jan-1998 (jenjo02)
**	    Modified to return when first modified page is found rather
**	    than continuing the scan. dm2t_fix_tcb() only wants to 
**	    know if there are any, not how many.
*/
i4 
dm0p_count_pages( i4        db_id,
                  i4        table_id)
{
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BUFFER	    *b;
    DM0P_BLQ_HEAD   *tab_bucket;
    DM0P_BID	    next_bid;
    i4		    bucket;
    
    /* Compute the table's hash bucket */
    bucket = (db_id + (u_i4)table_id) & (BMCB_TABLE_MAX - 1);
    tab_bucket = &bm_common->bmcb_tabq_buckets[bucket];

    /*
    ** Scan the table bucket chain looking for any buffers belonging
    ** to this table. 
    */
    next_bid = tab_bucket->blqh_bid;

    while (next_bid.bid_index != BUF_ENDLIST)
    {
	b = &lbm->lbm_buffers[next_bid.bid_cache][next_bid.bid_index];
    
	if (b->buf_tabid.db_tab_base != table_id ||
	    b->buf_dbid != db_id ||
	   (b->buf_sts & BUF_MODIFIED) == 0 ||
	    b->buf_tabq_bucket != bucket)
	{
	    if (b->buf_tabq_bucket == bucket)
		next_bid = b->buf_tabq.blq_next;
	    else
		next_bid = tab_bucket->blqh_bid;
	    continue;
	}

	/* Lock the buffer, recheck */
	dm0p_mlock(SEM_SHARE, &b->buf_mutex);

	if ((b->buf_sts & BUF_MODIFIED) &&
	    (b->buf_tabid.db_tab_base == table_id) &&
	    (b->buf_dbid == db_id) &&
	     b->buf_tabq_bucket == bucket)
	{
	    dm0p_munlock(&b->buf_mutex);
	    return(1);
	}
	if (b->buf_tabq_bucket == bucket)
	    next_bid = b->buf_tabq.blq_next;
	else
	    next_bid = tab_bucket->blqh_bid;
	dm0p_munlock(&b->buf_mutex);
    }

    return(0);
}

/*{
** Name: dm0p_complete - completion handler for a Buffer Managment 
**			 GatherWrite operation
**
** Description:
**
**	This is the completion handler called on the completion of
**	GatherWrite.  force_page passes this as the completion handler
**	parameter to dm2f_write_list().  The routine is meant to perform 
**	completion operations for both force_page() and its caller.
**
** Inputs:
**
**	dm2f_status	DB status on entry
**	bmcl		Buffer Manager closure control block
**	error		Reason for any error status on entry
**
** Outputs:
**
**	None
**
**	Returns:
**	    VOID
**	Exceptions:
**	    None
**
** Side Effects:
**
**	None
**
** History:
**
**	01-aug-1996 (nanpr01 for anb & itb ICL)
**	    Created.
**	    Fixed the mutexing.
**	    Added WRITE_ALONG option.
**	01-Nov-1996 (jenjo02)
**	    Acquire buffer's buf_mutex.
**	02-oct-1998 (somsa01)
**	    Added another parameter to complete_f_p() (which should be the
**	    log_id, but for now is 0).
**	21-May-2001 (jenjo02)
**	    All specialized completion processing now handled by
**	    complete_f_p().
*/
static VOID
dm0p_complete( 
DB_STATUS dm2f_status, 
DM0P_CLOSURE *bmcl, 
i4 	*err_code)
{
    if ( complete_f_p(dm2f_status, 
		    bmcl->bmcl_tbio,
		    bmcl->bmcl_buf,
		    bmcl->bmcl_pages, 
		    bmcl->bmcl_operation, 
		    bmcl->bmcl_log_id, 
		    bmcl->bmcl_lock_list, 
		    bmcl->bmcl_evcomp, 
		    err_code) )
	dm0p_munlock(&bmcl->bmcl_buf->buf_mutex);
    return;
}

/*{
** Name: complete_f_p
**
** Description:
**
**	This routine is called by dm0p_complete when GatherWrite I/O
**	completes and directly for synchronous write completion
**	by force_page().
**
** Inputs:
**
**	pstatus		DB status on entry
**	tbio		Table IO control block
**	b		Buffer descriptor
**	action		Requested action
**	operation	Requested operation
**	log_id		log id to be used
**	lock_list	lock list to be used
**	evcomp		who initiated the write?
**	err_code	reason for any error status on entry
**			(may be overwritten)
**
** Outputs:
**
**	err_code	reason for error status on exit
**
**	Returns:
**	    E_DB_OK	with buf_mutex unlocked.
**	    E_DB_ERROR	with buf_mutex still locked.
**	Exceptions:
**	    None
**
** Side Effects:
**
**	None
**
** History:
**
**	01-aug-1996 (nanpr01 for anb & itb ICL)
**	    Created
**	    Correct the mutexing; it is assumed that the BM mutex is
**	    held when the routine is entered.
**	01-Nov-1996 (jenjo02)
**	    Changed to use buf_mutex instead of obsolete bmcb_mutex.
**	12-Feb-1997 (jenjo02)
**	    Delay waking up buffer waiters until we're about to return.
**	    Moved code which updates bmcb_last_lsn to force_page() so that
**	    it can be updated as soon as its new value is known.
**	01-Apr-1997 (jenjo02)
**	    Made complete_f_p() the sole keeper of bs_force stat.
**	    Added bs_toss to count pages tossed from cache to make room for another.
**	    Added bs_priforce, bs_pritoss stats to track fixed priority forces.
**	08-May-1997 (jenjo02)
**	    Removed resetting of BUF_LOCKED | BUF_CACHED when unlocking a page.
**	    This wasn't needed and was being done without the protection of
**	    the buffer's mutex.
**	19-May-1997 (jenjo02)
**	    In complete_f_p(), put empty pages at the head of the free queue
**	    so that fault_page() will pick them before valid pages, which are
**	    placed at the end of the free queue.
**	19-Nov-1997 (jenjo02)
**	    Do NOT release buf_mutex on I/O error. Caller expects buf_mutex to
**	    be locked!
**	12-Jan-1998 (jenjo02)
**	    Wake up waiters on buffer, even if I/O error occurs.
**	19-Jan-1998 (jenjo02)
**	    Remove buffer from table's hash queue when buffer is made free.
**	01-oct-1998 (somsa01)
**	    In case of E_DM9448_DM2F_NO_DISK_SPACE and tbio_temporary,
**	    invalidate the page and return an OK status.
**	    (Bug #93633)
**	08-oct-1998 (somsa01)
**	    Corrected last cross-integration; cache_ix is now bm->bm_cache_ix.
**	    Also, invalidate_page() now takes four parameters.
**	01-feb-1999 (nanpr01 & jenjo02)
**	    Check the FIXED status before marking it FREE.
**	07-May-1999 (jenjo02)
**	    Removed gather_flag parm. Buffer mutex semantics are
**	    not changed when gather write is used.
**	    Removed lsn parm, which isn't needed.
**	24-Aug-1999 (jenjo02)
**	    If buffer is marked BUF_RECLAIM, remove it from whatever state
**	    queue it's on, but don't insert it onto a free queue.
**	14-Sep-1999 (jenjo02)
**	    Accurately count free buffers.
**	27-Oct-1999 (jenjo02)
**	    When tossing a page from the cache, invalidate its page number by
**	    setting it to -1.
**	10-Jan-2000 (jenjo02)
**	    If tossing group page, decrement group's g_valcount.
**	21-May-2001 (jenjo02)
**	    Added evcomp parm and associated special processing 
**	    culled from old "complete_...()" functions.
**      18-Sep-2003 (jenjo02) SIR 110923
**          Modified to anticipate multiple buffers, as when forcing
**          groups.
**	27-Apr-2005 (schka24/jenjo02)
**	    Don't move-group until we're fully done with the buffer, if there
**	    is any chance that we're emptying the group.  (Which makes the
**	    group grabbable, and gfault-page might start setting up for
**	    reading, unmutexed.)
**	3-May-2005 (schka24)
**	    Fix the fix to bug 93633, SET should have been GET, don't
**	    invalidate if we're the owner.
**	    Also, status handling needed updated for group force.
*/
static DB_STATUS
complete_f_p(
DB_STATUS pstatus, 
DMP_TABLE_IO *tbio, 
DM0P_BUFFER *b, 
i4 pages, 
i4 operation, 
i4 log_id, 
i4 lock_list, 
i4 evcomp,
i4 *err_code)
{
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BMCB	    *bm = lbm->lbm_bmcb[b->buf_id.bid_cache];
    DM0P_BSTAT	    *lbm_stats = &lbm->lbm_stats[b->buf_id.bid_cache];
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BUFFER     *buf_array = lbm->lbm_buffers[b->buf_id.bid_cache];
    DM0P_HASH_HEAD  *bucket_array = lbm->lbm_buckets[b->buf_id.bid_cache];
    DM0P_GROUP      *group_array = lbm->lbm_groups[b->buf_id.bid_cache];
    DM0P_GROUP	    *g;
    DM0P_STATE_HEAD *sth;
    DMPP_PAGE	    *page;
    i4		    save_priority;
    DB_STATUS	    retstatus = pstatus;
    DB_STATUS	    big_status;
    CL_SYS_ERR      sys_err;
    STATUS	    cl_status;
    i4	    	    next_buf, prev_buf;
    char            *tbl_name,
                    *dbs_name;
    char            tbl_buf[DB_MAXNAME],
                    dbs_buf[DB_MAXNAME];
    i4		    buf_is_on_state_queue;
    i4         	    orig_err_code = *err_code;
    i4		    action, i;
    i4		    mdelta = 0, vdelta = 0;

    /*
    **   Clarification:
    **
    **	    force_page() and complete_f_p() will return with
    **	    buf_mutex unlocked unless a WARN or ERROR status is
    **	    returned, in which case buf_mutex will remain locked.
    **
    ** pstatus = status of page or group force operation.
    ** retstatus = status we'll return, which might be worse (if some sort
    **    of locking error occurred), or better (see the disk full/temp table
    **    comment below).
    */
    for ( i = 0; i < pages; i++, b++ )
    {
	/*
	** If buffer's force_page lock list doesn't match,
	** skip this buffer; it wasn't the object of "force" by
	** this thread.
	*/
	if ( b->buf_fp_llid != lock_list )
	    continue;

	g = NULL;
	action = b->buf_fp_action;

	page = (DMPP_PAGE *)((char *)lbm->lbm_pages[b->buf_id.bid_cache] +  
						b->buf_page_offset);
	
	if (action & (WRITE_PAGE | CHANGE_LOCK | UNLOCK_PAGE | NULL_LOCK ))
	{
	    if (pstatus != E_DB_OK && action & WRITE_PAGE
				  && *err_code > E_DM_INTERNAL)
	    {
		if (*err_code != E_DM9448_DM2F_NO_DISK_SPACE)
		    ule_format(*err_code, (CL_SYS_ERR *)0, ULE_LOG, NULL, 
			(char *)0, (i4)0, (i4 *)0, err_code, 0);
		*err_code = E_DM920E_BM_FORCE_PAGE_ERROR;
	    }

	    if (pstatus == E_DB_OK && (action & CHANGE_LOCK) 
			     && ((action & UNLOCK_PAGE) == 0)
			     && ((action & NULL_LOCK) == 0))
	    {
		if (DMZ_LCK_MACRO(2))
		    dmd_lock(&b->buf_lock_key, lbm->lbm_lock_list, LK_CONVERT,
			(LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK),
			LK_IS,
			0, (DB_TRAN_ID *)0, (DB_TAB_NAME *)0, (DB_DB_NAME *)0);

		cl_status = LKrequest(
		    LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK,
		    lbm->lbm_lock_list, (LK_LOCK_KEY *)0, LK_IS,
		    (LK_VALUE *)&b->buf_oldvalue, (LK_LKID *)&b->buf_lk_id,
		    (i4)0, &sys_err);
		if (cl_status != OK)
		{
		    if (tbio)
		    {
			tbl_name = (char *)tbio->tbio_relid;
			dbs_name = (char *)tbio->tbio_dbname;
		    }
		    else
		    {
			tbl_name = (char *)STprintf(tbl_buf, "TABLE_ID (%d,%d)",
						    b->buf_tabid.db_tab_base,
						    b->buf_tabid.db_tab_index);
			dbs_name = (char *)STprintf(dbs_buf, "DATABASE_ID %d",
						    b->buf_dbid);
		    }
		    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 0);
		    ule_format(E_DM9021_BM_BAD_LOCK_PAGE, &sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 5,
			0, b->buf_lock_key.lk_key4, 0, tbl_name, 0, dbs_name,
			0, LK_IS, 0, lbm->lbm_lock_list);
		    *err_code = E_DM920E_BM_FORCE_PAGE_ERROR;
		    retstatus = E_DB_ERROR;
		}
		else
		{
		    b->buf_lock_mode = LK_IS;
		}
	    }
	    else if (pstatus == E_DB_OK && action & UNLOCK_PAGE)
	    {
		/*  Uncache the current page. */
		cl_status = LKrelease((i4)0, lbm->lbm_lock_list,
		    (LK_LKID *)&b->buf_lk_id, (LK_LOCK_KEY *)0,
		    (LK_VALUE *)&b->buf_oldvalue, &sys_err);

		if (cl_status != OK)
		{
		    if (tbio)
		    {
			tbl_name = (char *)tbio->tbio_relid;
			dbs_name = (char *)tbio->tbio_dbname;
		    }
		    else
		    {
			tbl_name = (char *)STprintf(tbl_buf, "TABLE_ID (%d,%d)",
						    b->buf_tabid.db_tab_base,
						    b->buf_tabid.db_tab_index);
			dbs_name = (char *)STprintf(dbs_buf, "DATABASE_ID %d",
						    b->buf_dbid);
		    }
		    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 0);
		    ule_format(E_DM9020_BM_BAD_UNLOCK_PAGE, &sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 4,
			0, b->buf_lock_key.lk_key4, 0, tbl_name, 0, dbs_name,
			0, lbm->lbm_lock_list);
		    *err_code = E_DM920E_BM_FORCE_PAGE_ERROR;
		    retstatus = E_DB_ERROR;
		}
		else                          
		{
		    /*
		    ** (ICL phil.p)
		    ** I can't think of any scenario in which DMCM
		    ** will use UNLOCK_PAGE, but just in case ....
		    **
		    if (b->buf_sts & BUF_DMCM)
		    {
			b->buf_lock_mode = 0;
		    }
		    ** (CA jon.j)
		    ** How about FP_TOSS? Happens all the time!
		    */
		    b->buf_lock_mode = LK_N;
		    b->buf_pid = (PID)0;
		}
	    }
	    else if (pstatus == E_DB_OK && action & NULL_LOCK)
	    {
		i4		lock_mode = LK_N;
		/*
		** (ICL phil.p)
		** Retain page in cache with NULL lock if unfixed,
		** else downgrade lock to LK_S.
		** Used exclusively by the DMCM protocol.
		** If FIXED, protocols have ensured that it's only
		** fixed for read, not write.
		** 05-Feb-1999 (jenjo02)
		*/

		if (b->buf_sts & BUF_DMCM)
		{
		    /* NULL_LOCK only set if not fixed for write */
		    if (b->buf_sts & BUF_FIXED)
			lock_mode = LK_S;

		    cl_status = LKrequestWithCallback(
			LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK,
			lbm->lbm_lock_list, (LK_LOCK_KEY *)0, lock_mode,
			(LK_VALUE *) &b->buf_oldvalue, (LK_LKID *)&b->buf_lk_id,
			(i4)0, (LKcallback) dm0p_dmcm_callback, NULL, 
			&sys_err);
		}
		else
		{
		    cl_status = LKrequest(
			LK_PHYSICAL | LK_CONVERT | LK_NODEADLOCK,
			lbm->lbm_lock_list, (LK_LOCK_KEY *)0, lock_mode,
			(LK_VALUE *) &b->buf_oldvalue, (LK_LKID *)&b->buf_lk_id,
			(i4)0, &sys_err);
		}

		if (cl_status != OK)
		{
		    if (tbio)
		    {
			tbl_name = (char *)tbio->tbio_relid;
			dbs_name = (char *)tbio->tbio_dbname;
		    }
		    else
		    {
			tbl_name = (char *)STprintf(tbl_buf, "TABLE_ID (%d,%d)",
						    b->buf_tabid.db_tab_base,
						    b->buf_tabid.db_tab_index);
			dbs_name = (char *)STprintf(dbs_buf, "DATABASE_ID %d",
						    b->buf_dbid);
		    }
		    ule_format(cl_status, &sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 0);
		    ule_format(E_DM9021_BM_BAD_LOCK_PAGE, &sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, err_code, 5,
			0, b->buf_lock_key.lk_key4, 0, tbl_name, 0, dbs_name,
			0, b->buf_lock_mode, 0, lbm->lbm_lock_list);
		    *err_code = E_DM920E_BM_FORCE_PAGE_ERROR;
		    retstatus = E_DB_ERROR;
		}
		else
		{
		    b->buf_lock_mode = lock_mode;
		}
	    }

	    /* Relock the buffer */
	    dm0p_mlock(SEM_EXCL, &b->buf_mutex);
	}

	/* Unpin GatherWrite TBIO */
	if ( b->buf_sts & BUF_GW_QUEUED )
	{
	    dm2t_unpin_tabio_cb(tbio, lock_list);
	    b->buf_sts &= ~BUF_GW_QUEUED;
	}

	/*  Make changes to the buffer as a result of the operation. */

	if (pstatus != E_DB_OK)
	{
	    b->buf_sts |= BUF_IOERROR;

	    /* Special stuff to do if we ran out of disk space trying to
	    ** force the page.  Regular tables are allocated explicitly, and
	    ** a disk-full is unexpected and very bad.  Temp tables however
	    ** are spilled to disk as-you-go.  We don't want to return an
	    ** error (and crash out the writer) if the writer is someone
	    ** other than the owner, such as a sync-writer or a background
	    ** thread (Cache Agent, etc).
	    ** If on the other hand this session is the temp table owner,
	    ** just handle it like any other I/O error.
	    */

	    if ((orig_err_code == E_DM9448_DM2F_NO_DISK_SPACE) &&
		b->buf_sts & BUF_TEMPTABLE)
	    {
		DB_TRAN_ID		page_tran_id;
		LG_TRANSACTION	tr;
		i4		length;
		DB_STATUS		locstatus;

		STRUCT_ASSIGN_MACRO(
			DMPP_VPT_GET_PAGE_TRAN_ID_MACRO(
				b->buf_relpgtype, page),
			page_tran_id);
		MEcopy((char *)&log_id, sizeof(log_id), (char *)&tr);
		locstatus = LGshow(LG_S_ATRANSACTION, (PTR)&tr, sizeof(tr),
				    &length, &sys_err);

		if (locstatus == OK &&
		    (page_tran_id.db_high_tran != tr.tr_eid.db_high_tran ||
		     page_tran_id.db_low_tran != tr.tr_eid.db_low_tran) )
		{
		    retstatus = E_DB_OK;

		    /* Invalidate out the page so that the buffer can be
		    ** reused.  There's not much else that can be done with it,
		    ** nor is there any good way to (immediately) notify the
		    ** owner.  Fortunately, this being a temp table, there's
		    ** little point in the owner modifying the page unless
		    ** he's going to re-read it eventually, and at that point
		    ** the owner will get an error.
		    ** (Of course, if the owner never does re-read this page,
		    ** so much the better!)
		    ** Invalidate-page does all the completion actions normally
		    ** done below, including dropping the buf mutex, so just
		    ** continue on to the next buffer.
		    */
		    invalidate_page(b, lock_list);
		    continue;
		}
	    }
	}
	else
	{
	    /*
	    ** Update the buffers Modified and Lock status based on the actions
	    ** just performed.
	    **
	    ** If the server is executing a Consistency Point, then mark this
	    ** buffer as flushed for the current CP.
	    **
	    ** This is necessary due to Multi-Server protocols. If multiple servers are
	    ** cooperating to execute a CP, we may be flushing a page that cannot be
	    ** processed by the other servers (due to not having a TCB). If our
	    ** server's CP thread has already passed this buffer, but others haven't,
	    ** then a CP protocol failure will occur if this buffer gets re-modified
	    ** before the next CP thread reaches it.
	    */

	    if (action & WRITE_PAGE)
	    {
		action |= UNMODIFY_PAGE;
		b->buf_sts &= ~(BUF_MODIFIED);

		if (bm_common->bmcb_status & BMCB_FCFLUSH)
		    b->buf_cpcount = bm_common->bmcb_cpcount;
		
		/*
		** If a temporary table see if the page written is past
		** the table's highwater mark.  If so the update this value.
		*/
		if (b->buf_sts & BUF_TEMPTABLE &&
		    (b->buf_lock_key.lk_key4 > tbio->tbio_tmp_hw_pageno) )
		{
		    tbio->tbio_tmp_hw_pageno = b->buf_lock_key.lk_key4;
		}
	    }
	    if (action & LOCKED_PAGE)
	    {
		if (action & UNLOCK_PAGE)
		    b->buf_sts &= ~(BUF_LOCKED | BUF_CACHED);
		else if ((operation == FP_CACHE) ||
			 (operation == FP_NULL))
		    b->buf_sts |= BUF_CACHED;
	    }

	    /*
	    ** Determine the action required to move buffer to correct buffer queues:
	    **   - If the buffer has been written and is not fixed, 
	    **	   then it should be moved to the free queue (FREE_PAGE).
	    **   - If it being tossed, then it should be taken off the buffer hash
	    **     queues and put on the free queue at priority 0 (EMPTY_PAGE)
	    **	   unless BUF_RECLAIM is indicated, which will leave the buffer on
	    **	   no state queue.
	    **   - If the page belonged to a transaction which is no longer active, then
	    **     it should be take off that transaction's buffer list (TRAN_PAGE).
	    */
	    switch(operation)
	    {
	      case FP_WRITE:
		if (((b->buf_sts & BUF_FIXED) == 0) && (action & WRITE_PAGE))
		    action |= FREE_PAGE;
		break;

	      case FP_CACHE:
		action |= TRAN_PAGE;
		if (action & WRITE_PAGE)
		    action |= FREE_PAGE;
		break;

	      case FP_TOSS:
		action |= (FREE_PAGE | EMPTY_PAGE | TRAN_PAGE);
		break;

	      case FP_INVALID:
		if ((b->buf_sts & BUF_FIXED) == 0)
		{
		    action |= (FREE_PAGE | EMPTY_PAGE | TRAN_PAGE);
		    if (b->buf_sts & BUF_MODIFIED)
			action |= UNMODIFY_PAGE;
		}
		break;

	      case FP_DISOWN:
		action |= TRAN_PAGE;
		break;

	      case FP_NULL:
		action |= TRAN_PAGE;
		if (((b->buf_sts & BUF_FIXED) == 0) && (action & WRITE_PAGE))
		    action |= FREE_PAGE;
		break;
	    }

	    /*
	    ** Save the priority for later when we remove the buffer from its
	    ** curent state queue.  Save this value since we may change the
	    ** priority to zero below.
	    */
	    save_priority = b->buf_priority;

	    /* Is the buffer on a state queue? */
	    buf_is_on_state_queue = (b->buf_state.stq_next != 0 ||
				      b->buf_state.stq_prev != 0);

	    /*
	    ** If page belonged to transaction that is no longer active, then
	    ** take the page off of the transaction buffer queue.
	    */
	    if ((action & TRAN_PAGE) && (b->buf_txnq_bucket != BUF_NOBUCKET))
	    {
		txnq_delete(b);
	    }

	    /*
	    ** If the page is being tossed, then take it off of its hash queue.
	    */
	    if (action & EMPTY_PAGE)
	    {
		DB_TRAN_ID	tran;

		hashq_delete(b, bucket_array, buf_array);

		/* Remove buffer from its table queue */
		if (b->buf_tabq_bucket != BUF_NOBUCKET)
		    tabq_delete(b);


		/*
		** If reclaiming a page for fault_page, count
		** it as a replacement operation, not a toss.
		*/
		if ( b->buf_sts & BUF_RECLAIM )
		{
		    lbm_stats->bs_replace[BMCB_PTYPES]++;
		    bm->bm_stats.bs_replace[BMCB_PTYPES]++;
		    lbm_stats->bs_replace[b->buf_type]++;
		    bm->bm_stats.bs_replace[b->buf_type]++;
		}
		else
		{
		    lbm_stats->bs_toss[BMCB_PTYPES]++;
		    bm->bm_stats.bs_toss[BMCB_PTYPES]++;
		    lbm_stats->bs_toss[b->buf_type]++;
		    bm->bm_stats.bs_toss[b->buf_type]++;
		}
	 
		/* Off all BUT the following status bits */
		b->buf_sts &= (BUF_WBUSY | BUF_IOWAIT | BUF_MUTEX | BUF_MWAIT | BUF_RECLAIM);
		b->buf_priority = 0;

		/* Clobber pageno on the page */
		DMPP_VPT_SET_PAGE_PAGE_MACRO(b->buf_relpgtype, page, (DM_PAGENO)-1);

		/* Clobber tran id on the page */
		tran.db_low_tran = 0;
		tran.db_high_tran = 0;
		DMPP_VPT_SET_PAGE_TRAN_ID_MACRO(b->buf_relpgtype, page, tran);

		/* Clobber page_stat on the page */
		DMPP_VPT_INIT_PAGE_STAT_MACRO(b->buf_relpgtype, page, 0);
	    }

	    /*
	    ** If the page has been made free, then take it off of its current
	    ** state queue and put it on the appropriate free priority queue.
	    ** If it is a group page then call move_group to do this.
	    **
	    ** If the page was previously modified, then update the modify and
	    ** free counts and signal any waiters who are waiting for new free pages.
	    */
	    if (action & FREE_PAGE)
	    {
		if (b->buf_owner == BUF_SELF)
		{
		    /* Adjust priority if needed */
		    if (action & PRIORITY_PAGE)
			b->buf_priority = DB_MAX_TABLEPRI -1;

		    /*
		    ** Remove buffer from the modify or free state queue.
		    ** If only fixed or fixed and modified, the buffer
		    ** is on no state queue.
		    */
		    if (action & UNMODIFY_PAGE)
		    {
			sth = &bm->bm_mpq[save_priority];
		    }
		    else
		    {
			/*
			** If we're not changing priority and 
			** the buffer's already last (or first,
			** if EMPTY_PAGE) on its
			** free priority queue, don't waste
			** time (and mutexes) unhooking and rehooking
			** it to the same position.
			*/
			if ( (b->buf_sts & BUF_RECLAIM) == 0 &&
			    b->buf_priority == save_priority && (
			   (action & EMPTY_PAGE && b->buf_state.stq_prev == BUF_ENDLIST) ||
			   ((action & EMPTY_PAGE) == 0 && b->buf_state.stq_next == BUF_ENDLIST))
			   )
			{
			    sth = 0;
			}
			else
			{
			    sth = &bm->bm_fpq[save_priority];
			}
		    }

		    if (sth)
		    {
			if (buf_is_on_state_queue)
			{
			    /* Remove buffer from current state queue */
			    dm0p_mlock(SEM_EXCL, &sth->sth_mutex);

			    next_buf = b->buf_state.stq_next;
			    prev_buf = b->buf_state.stq_prev;
			    if (next_buf != BUF_ENDLIST)
				buf_array[next_buf].buf_state.stq_prev = prev_buf;
			    else
				sth->sth_stq.stq_prev = prev_buf;
			    if (prev_buf != BUF_ENDLIST)
				buf_array[prev_buf].buf_state.stq_next = next_buf;
			    else
				sth->sth_stq.stq_next = next_buf;
			    sth->sth_count--;

			    /*
			    ** If reclaiming free/modified buffer, it's about
			    ** to become fixed, so up the fixed count.
			    */
			    if ( b->buf_sts & BUF_RECLAIM )
				sth->sth_lcount++;

			    dm0p_munlock(&sth->sth_mutex);
			    b->buf_state.stq_next = b->buf_state.stq_prev = 0;
			}

			/*
			** Put buffer on the end of the free queue unless
			** EMPTY_PAGE in which case we'll put it on the
			** front of the free queue so that fault_page() will
			** use these before BUF_VALID buffers.
			*/
			if ( (b->buf_sts & BUF_RECLAIM) == 0 )
			{
			    sth = &bm->bm_fpq[b->buf_priority];
			    dm0p_mlock(SEM_EXCL, &sth->sth_mutex);
			    if (action & EMPTY_PAGE)
			    {
				next_buf = sth->sth_stq.stq_next;
				b->buf_state.stq_next = next_buf;
				b->buf_state.stq_prev = BUF_ENDLIST;
				if (next_buf != BUF_ENDLIST)
				    buf_array[next_buf].buf_state.stq_prev = 
					b->buf_id.bid_index;
				else
				    sth->sth_stq.stq_prev =
					b->buf_id.bid_index;
				sth->sth_stq.stq_next =
					b->buf_id.bid_index;
			    }
			    else
			    {
				prev_buf = sth->sth_stq.stq_prev;
				b->buf_state.stq_prev = prev_buf;
				b->buf_state.stq_next = BUF_ENDLIST;
				if (prev_buf != BUF_ENDLIST)
				    buf_array[prev_buf].buf_state.stq_next =
					b->buf_id.bid_index;
				else
				    sth->sth_stq.stq_next =
					b->buf_id.bid_index;
				sth->sth_stq.stq_prev =
					b->buf_id.bid_index;
			    }
			    sth->sth_count++;
			    dm0p_munlock(&sth->sth_mutex);
			}

			/* 
			** If we moved a modified page to the free queue
			** and it's not being reclaimed,
			** wake up anyone waiting on a free page.
			*/
			if ( bm->bm_status & BM_FWAIT &&
			     action & UNMODIFY_PAGE && 
			     (b->buf_sts & BUF_RECLAIM) == 0 )
			{
			    dm0p_mlock(SEM_EXCL, &bm->bm_mutex);
			    if (bm->bm_status & BM_FWAIT)
			    {
				dm0p_erelease(lock_list, DM0P_FWAIT_EVENT, 
						bm->bm_cache_ix, 0, 0);
				bm->bm_status &= ~BM_FWAIT;
			    }
			    dm0p_munlock(&bm->bm_mutex);
			}
		    }
		}
		else
		{
		    /* Group buffer */
		    if ( action & (UNMODIFY_PAGE | EMPTY_PAGE) )
		    {
			g = &group_array[b->buf_owner];
			dm0p_mlock(SEM_EXCL, &g->g_mutex);
			if ( action & UNMODIFY_PAGE )
			    mdelta = -1;
			if ( action & EMPTY_PAGE )
			    vdelta = -1;
			/* We'll hold the g_mutex until nearly the end for this buf */
		    }
		}
	    }
	}

	if (DMZ_BUF_MACRO(15))
	{
	    if (dm0p_check_consistency(7))
	    {
		TRdisplay("%@ Warning: BM inconsistency during force_page.\n");
		TRdisplay("  Force operation %d, action %d, Buffer 0x%x\n",
		    operation, action, b);
	    }
	}

	/* Perform final completion work based on write initiator */
	if ( evcomp != BMCL_NO_GATHER )
	{
	    if ( evcomp == BMCL_FLUSH_PAGES )
	    {
		/* WriteBehind thread/agent */
		b->buf_sts &= ~(BUF_WBBUSY);
	    }
	    else if ( evcomp == BMCL_CP_FLUSH )
	    {
		/* CP thread/agent */
		b->buf_sts &= ~(BUF_MUTEX | BUF_FCBUSY | BUF_MUTEX_OK);

		/* If error, destroy cpindex to force incomplete CP */
		if ( pstatus )
		{
		    /* unmark this buffer as processed by this CP */
		    if (b->buf_cpcount == bm_common->bmcb_cpcount)
			b->buf_cpcount--;

		    dm0p_mlock(SEM_EXCL, &bm->bm_mutex);
		    bm->bm_cpindex =  b->buf_cpindex;
		    dm0p_munlock(&bm->bm_mutex);
		}
	    }
	    else if ( evcomp == BMCL_CLOSE_PAGE || evcomp == BMCL_DMCM_CBACK )
	    {
		/* dm0p_close_page() */
		b->buf_sts &= ~(BUF_MUTEX | BUF_MUTEX_OK);
	    }
	    else if ( evcomp == BMCL_TOSS_PAGES || evcomp == BMCL_WPASS_ABORT)
	    {
		/* Nothing more to do normally, but if error, invalidate
		** the page globally.
		*/
		if (pstatus != E_DB_OK)
		{
		    i4		msg, local_error;
		    DB_STATUS	local_status;

		    msg = E_DM9308_DM0P_TOSS_PAGES;
		    if (evcomp == BMCL_WPASS_ABORT)
			msg = E_DM93A8_DM0P_WPASS_ABORT;
		    /* Unexpected error - log error and invalidate the page */
		    ule_format(msg, (CL_SYS_ERR *)0, ULE_LOG, 
			    NULL, (char *)0, (i4)0, (i4 *)0, err_code, 4,
			    0, b->buf_dbid, 0, b->buf_tabid.db_tab_base,
			    0, b->buf_tabid.db_tab_index, 0, 
			    DMPP_VPT_GET_PAGE_PAGE_MACRO(b->buf_relpgtype, page));
		    /* If here, we skipped all the buf processing except for
		    ** setting IOERROR.  Invalidate this buffer.
		    */
		    local_status = force_page(b, (DMP_TABLE_IO *)NULL, 
			    FP_INVALID, lock_list, log_id, BMCL_NO_GATHER, 
			    (i4*)0, &local_error);
		    if (local_status != E_DB_OK)
			dm0p_munlock(&b->buf_mutex);

		    /* Need to return OK else we'll get into even worse
		    ** trouble real soon, with mutex issues and such.  Skip
		    ** the remainder of processing on this buf, force did it.
		    */
		    retstatus = E_DB_OK;
		    continue;
		}
	    }
	}
	
	/*
	** Un-write-busy the buffer, awaken I/O, Mutex, or CP waiters.
	**
	** If the buffer is being RECLAIMed by fault_page(), return
	** with buf_mutex held, otherwise release it if status is OK.
	*/
	b->buf_sts &= ~(BUF_WBUSY);

	/* Clear force page action and "owner" */
	b->buf_fp_action = 0;
	b->buf_fp_llid = 0;
	
	if ( b->buf_sts & (BUF_IOWAIT | BUF_MWAIT | BUF_FCWAIT) )
	    awaken_buf_waiters(b, lock_list);

	/* Now, and only now, finish up with adjusting the group structure.
	** This call might make the group empty, which makes it immediately
	** grabbable by gfault_page, which messes with the buffer header
	** un-mutexed.  We had better be *completely* finished with the buffer
	** at this point.  (clearly, we are doing the last page of the force
	** loop if the group becomes empty here -- else something is
	** badly wrong!)
	*/
	if (g != NULL)
	{
	    move_group(bm, g, b, lock_list, 0, mdelta, vdelta);
	    dm0p_munlock(&g->g_mutex);
	}

	/* Release mutex if not first buffer, or OK and not RECLAIM */
	if ( i ||  
 	    (retstatus == E_DB_OK && (b->buf_sts & BUF_RECLAIM) == 0) )
	{
	    dm0p_munlock(&b->buf_mutex);
	}
    }

    /* Return with mutex if returning error or if RECLAIM */

    return (retstatus);
}

/*{
** Name: tabq_delete - Delete buffer from table queue
**
** Description:
**      This routine will delete a buffer from the table queue
**
** Inputs:
**      b                               buffer pointer 
**
** Outputs:
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	13-Jan-1998 (jenjo02)
**	    invented
*/
static VOID
tabq_delete(DM0P_BUFFER *b)
{
    DMP_LBMCB           *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON      *bm_common = lbm->lbm_bm_common;
    DM0P_BID            *next, *prev;

    /* Lock the table bucket queue */
    dm0p_mlock(SEM_EXCL, &bm_common->bmcb_tabq_buckets[b->buf_tabq_bucket].blqh_mutex);
    next = &b->buf_tabq.blq_next;
    prev = &b->buf_tabq.blq_prev;
    if (next->bid_index != BUF_ENDLIST)
	lbm->lbm_buffers[next->bid_cache][next->bid_index].buf_tabq.blq_prev = *prev;
				
    if (prev->bid_index != BUF_ENDLIST)
	lbm->lbm_buffers[prev->bid_cache][prev->bid_index].buf_tabq.blq_next = *next;
    else
	bm_common->bmcb_tabq_buckets[b->buf_tabq_bucket].blqh_bid = *next;

    dm0p_munlock(&bm_common->bmcb_tabq_buckets[b->buf_tabq_bucket].blqh_mutex);

    b->buf_tabq_bucket = BUF_NOBUCKET;

    return;
}


/*{
** Name: tabq_insert - Insert buffer onto table queue
**
** Description:
**      This routine will insert a buffer onto a table's queue
**
** Inputs:
**      b                               buffer pointer 
**
** Outputs:
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	13-Jan-1998 (jenjo02)
**	    invented
**	19-Sep-2006 (jonj)
**	    Don't modify buffer until the queue mutex is acquired.
*/
static VOID
tabq_insert(
DM0P_BUFFER *b)
{
    DMP_LBMCB           *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON      *bm_common = lbm->lbm_bm_common;
    DM0P_BLQ_HEAD       *tabq_bucket;
    i4			bucket;

    /* Compute the tabq hash bucket to queue this page. */
    /*
    ** The buffer is hashed on database_id and table_id
    ** only so that indexes on the table will hash here also.
    */
    bucket = (b->buf_dbid + b->buf_tabid.db_tab_base)
		& (BMCB_TABLE_MAX - 1);
    tabq_bucket = &bm_common->bmcb_tabq_buckets[bucket];

    /* Lock the table bucket queue */
    dm0p_mlock(SEM_EXCL, &tabq_bucket->blqh_mutex);

    /*  Insert buffer on the tabq queue. */

    b->buf_tabq.blq_next = tabq_bucket->blqh_bid;

    b->buf_tabq.blq_prev.bid_index = BUF_ENDLIST;
    if (tabq_bucket->blqh_bid.bid_index != BUF_ENDLIST)
    {
	lbm->lbm_buffers[tabq_bucket->blqh_bid.bid_cache]
			[tabq_bucket->blqh_bid.bid_index].buf_tabq.blq_prev
				= b->buf_id;
    }

    tabq_bucket->blqh_bid = b->buf_id;

    b->buf_tabq_bucket = bucket;

    dm0p_munlock(&tabq_bucket->blqh_mutex);

    return;
}


/*{
** Name: txnq_delete - Delete buffer from transaction queue
**
** Description:
**      This routine will delete a buffer from the transaction queue
**
** Inputs:
**      b                               buffer pointer 
**
** Outputs:
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      06-mar-1996 (stial01)
**          Moved this code into this function.
**	19-Sep-2006 (jonj)
**	    Delay mutex release until after buffer is cleaned.
*/
static VOID
txnq_delete(DM0P_BUFFER *b)
{
    DMP_LBMCB           *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON      *bm_common = lbm->lbm_bm_common;
    DM0P_BLQ_HEAD       *tran_bucket;
    DM0P_BID            *next, *prev;

    /* Lock the transaction bucket queue */
    tran_bucket = &bm_common->bmcb_txnq_buckets[b->buf_txnq_bucket];

    dm0p_mlock(SEM_EXCL, &tran_bucket->blqh_mutex);
    next = &b->buf_txnq.blq_next;
    prev = &b->buf_txnq.blq_prev;
    if (next->bid_index != BUF_ENDLIST)
	lbm->lbm_buffers[next->bid_cache][next->bid_index].buf_txnq.blq_prev = *prev;
				
    if (prev->bid_index != BUF_ENDLIST)
	lbm->lbm_buffers[prev->bid_cache][prev->bid_index].buf_txnq.blq_next = *next;
    else
	tran_bucket->blqh_bid = *next;

    b->buf_tran_id.db_high_tran = 0;
    b->buf_tran_id.db_low_tran = 0;
    b->buf_log_id = 0;
    b->buf_txnq_bucket = BUF_NOBUCKET;

    dm0p_munlock(&tran_bucket->blqh_mutex);

    return;
}


/*{
** Name: txnq_insert - Insert buffer onto transaction queue
**
** Description:
**      This routine will insert a buffer onto the transaction queue
**
** Inputs:
**      b                               buffer pointer 
**
** Outputs:
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      06-mar-1996 (stial01)
**          Moved this code into this function.
**	15-Sep-1996 (jenjo02)
**	    Caller must hold buffer's buf_mutex.
**	19-Sep-2006 (jonj)
**	    Don't modify buffer until the queue mutex is acquired.
**	    Stow log_id of transaction that's doing the insert;
**	    shared transactions will have the same tran_id but 
**	    different log_id's and we sometimes need to know
**	    only those buffers belonging to a specific 
**	    log_id/tran_id pair in dm0p_force_pages(),
**	    specifically during LOGFULL_COMMIT involving
**	    shared transactions.
*/
static VOID
txnq_insert(
DM0P_BUFFER *b,
i4	    log_id)
{
    DMP_LBMCB           *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BM_COMMON      *bm_common = lbm->lbm_bm_common;
    DM0P_BLQ_HEAD       *tran_bucket;
    i4			bucket;

    /* Compute the tran hash bucket to queue this page. */
    bucket = b->buf_tran_id.db_low_tran & (BMCB_TRAN_MAX -1);
    tran_bucket = &bm_common->bmcb_txnq_buckets[bucket];

    /* Lock the transaction bucket queue */
    dm0p_mlock(SEM_EXCL, &tran_bucket->blqh_mutex);

    /*  Insert buffer on the tran queue. */

    b->buf_txnq.blq_next = tran_bucket->blqh_bid;

    b->buf_txnq.blq_prev.bid_index = BUF_ENDLIST;
    if (tran_bucket->blqh_bid.bid_index != BUF_ENDLIST)
    {
	lbm->lbm_buffers
	    [tran_bucket->blqh_bid.bid_cache]
	    [tran_bucket->blqh_bid.bid_index].buf_txnq.blq_prev = b->buf_id;
    }

    tran_bucket->blqh_bid = b->buf_id;

    /* Lastly, update the buffer */
    b->buf_txnq_bucket = bucket;
    b->buf_log_id = log_id;

    dm0p_munlock(&tran_bucket->blqh_mutex);

    return;
}

/*{
** Name: bm_shrbuf - Create/attach to shared memory buffer segment 
**
** Description:
**      This routine will allocate/attach to shared memory buffer segment 
**
**	If connecting, and NOT initing, the size and total buffer
**	counts are ignored and can be zero.
**
** Inputs:
**      cache_name                      Name of shared buffer cache
**      segsize                         Size of shared buffer segment
**      segment_ix                      Segment index: 
**                                        Multiple buffer caches may be 
**                                        allocated in multiple memory segments
**      totbufcnt                       Total buffer count (for error display)
**      flags
**      shrbuf_init
**      shrbuf_connect
**      bmsegpp
**
** Outputs:
**      bmsegp                          Buffer manager segment pointer returned
**	err_code			Reason for error return status.
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**
** Side Effects:
**	    none
**
** History:
**      06-mar-1996 (stial01)
**          Multiple buffer pools in support of variable page sizes.
**      12-Sep-1997(merja01)
**          Removed local variable status. This was being set to
**          DB_OK and being returned as DB_OK regardless of whether the
**          MEget_pages was successful.  The value of cl_status is used
**          to indicate if MEget_pages was successful.  This change will
**          return DB_OK if cl_status is zero and DB_ERROR if MEget_pages 
**          failed and cl_status is non-zero.
**      23-aug-2002 (devjo01)
**          Remove special logic that returned alternate error code
**          E_DM938C_BM_CLUSTER_CONNECT if ME_NO_SUCH_SEGMENT was detected
**          in a clustered Ingres environment.  Lock key for buffer name
**          is now forced to be unique per node.
**      19-sep-2002 (devjo01)
**          If running shared cache & NUMA cluster (currently a future
**          enhancement), make sure shared mem is local to RAD.
**	07-oct-2004 (thaju02)
**	    Change pages, allocated_pages to SIZE_TYPE.
*/
static DB_STATUS
bm_shrbuf(
char           *cache_name,
SIZE_TYPE      segsize,
i4             segment_ix,
i4             totbufcnt,
i4             flags,
i4             shrbuf_init,
i4             shrbuf_connect,
DMP_BMSCB      **bmsegpp,
i4             *err_code)
{
    i4             name_index;
    char           name_buf[CACHENM_MAX + 8];
    STATUS         cl_status;
    SIZE_TYPE      pages;
    i4             flags_arg;
    CL_SYS_ERR     sys_err;
    SIZE_TYPE      allocated_pages;
    DB_STATUS	   status;

    /*
    ** Build shared memory key from the user supplied cache name.  A
    ** shared memory key looks like an Ingres filename.  Use the first
    ** 8 characters of the cache name as the file prefix and append
    **
    **    ".dat"        for the first or only segment
    **    ".dan"        where "n" is the segment number (segment_ix)
    **			for subsequent segments.
    */
    STncpy(name_buf, cache_name, CACHENM_MAX);
    name_buf[ CACHENM_MAX ] = '\0';

    /* Trim off blanks and add '.DATn' suffix. */
    name_index = STtrmwhite(name_buf);
    STcopy(CACHENAME_SUFFIX, name_buf + name_index);
    if ( segment_ix )
	CVla(segment_ix, name_buf + name_index + sizeof(CACHENAME_SUFFIX) - 2);
    name_index = STtrmwhite(name_buf);
    name_buf[name_index] = '\0';
    CVlower(name_buf);

    /*
    ** Connect to the shared memory segment.  If this is the first
    ** connection, then specify to create it.  If we try to create
    ** it and it already exists, then destroy it and try again.
    */
    pages = (segsize + ME_MPAGESIZE - 1) / ME_MPAGESIZE;
    flags_arg = (ME_MSHARED_MASK | ME_IO_MASK);
    if (shrbuf_init)
	flags_arg |= (ME_CREATE_MASK | ME_NOTPERM_MASK);
    if (CXnuma_user_rad())
        flags_arg |= ME_LOCAL_RAD;

    /* If cache locking requested, set flags accordingly. */
    if (flags & DM0P_LOCK_CACHE)
	flags_arg |= ME_LOCKED_MASK;

    do
    {
	*bmsegpp = (DMP_BMSCB*)NULL;

	cl_status = MEget_pages(flags_arg, pages, name_buf,
	    (PTR *)bmsegpp, &allocated_pages, &sys_err);

	if (shrbuf_init && cl_status == ME_ALREADY_EXISTS)
	{
	    /*
	    ** Memory segment already exists - destroy it and recreate
	    ** a new one.  We cannot use a cache that has no connections
	    ** to it is it is impossible to tell if it has been trashed.
	    */
	    CS_cp_sem_cleanup(name_buf, &sys_err);
	    cl_status = MEsmdestroy(name_buf, &sys_err);
	    if (cl_status != OK)
	    {
		ule_format(E_DM9374_BM_CANT_DESTROY, &sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, err_code, 0);
	    }
	}
    } while ( cl_status == OK && *bmsegpp == (DMP_BMSCB*)NULL );

    if ( cl_status == OK )
	status = E_DB_OK;
    else
    {
	status = E_DB_ERROR;
	*err_code = E_DM9372_BM_CANT_CREATE;

	if (shrbuf_connect)
	{
	    *err_code = E_DM9373_BM_CANT_CONNECT;
	}

	if (cl_status == ME_OUT_OF_MEM)
	{
	    ule_format(E_DM938D_SHARED_MEM_ALLOCATE, &sys_err, ULE_LOG,
		NULL, (char *)0, (i4)0, (i4 *)0, err_code, 3,
		0, totbufcnt, (i4)sizeof(SIZE_TYPE), segsize, 0, pages);
	    ule_format(*err_code, &sys_err, ULE_LOG, NULL, (char *)0,
		(i4)0, (i4 *)0, err_code, 0);
	    *err_code = E_DM0112_RESOURCE_QUOTA_EXCEED;
	}
	else if ((cl_status == ME_NO_MLOCK)      ||
		(cl_status == ME_LOCK_FAIL)     ||
		(cl_status == ME_NO_SHMEM_LOCK) ||
		(cl_status == ME_LCKPAG_FAIL))
	{
	    ule_format(cl_status, &sys_err, ULE_LOG, NULL, (char *)0,
		    (i4)0, (i4 *)0, err_code, 0);
	    *err_code = E_DM93AE_BM_CACHE_LOCK;
	}
    }

    return(status);
}

/*
** Name: bm_check_shrseg -- Check validity of shared buffer manager segment.
**
** Description:
**
**	This routine is used to make some basic validity checks on
**	a shared memory segment that we've attached to, thinking that
**	it's a buffer manager segment.  Buffer manager segments
**	have to start with a valid BMSCB segment control block,
**	and each cache in the segment has to have a valid BMCB.
**	If the segment looks bad, a message is logged and we return
**	an error status.
**
** Inputs:
**	segment_ix			Segment index (for messages?)
**	bmsegp				Pointer to base of segment
**	cache_name			The shared cache name, for messages
**	err_code			Where to store E_xxx error code
**
** Outputs:
**	returns E_DB_OK or error.
**
** History:
**	6-Jan-2006 (kschendel)
**	    Extracted to streamline connects.
**
*/

static DB_STATUS
bm_check_shrseg(i4 segment_ix, DMP_BMSCB *bmsegp,
	char *cache_name, i4 *err_code)
{
    CL_SYS_ERR sys_err;
    DB_STATUS status;			/* Usual status thing */
    DM0P_BMCB *bm;			/* Cache control block addr */
    i4 cache_ix;			/* Cache number index */
    i4 pgsize;				/* Cache page size */

    status = E_DB_OK;
    if (bmsegp == NULL || bmsegp->bms_type != BMSCB_CB)
    {
	ule_format(E_DM937F_BM_NOT_A_BUFMGR, &sys_err,
		    ULE_LOG, NULL, (char *)0, (i4)0,
		    (i4 *)0, err_code, 1, 0, cache_name);
	*err_code = E_DM9373_BM_CANT_CONNECT;
	status = E_DB_ERROR;
    }

    /* Validate DM0P_BMCB(s) */
    for (cache_ix = 0, pgsize = 2048; 
	 cache_ix < DM_MAX_CACHE && status == E_DB_OK;
	 cache_ix++, pgsize *= 2)
    {
	if ( bmsegp->bms_bmcb[cache_ix] )
	{
	    bm = (DM0P_BMCB *)((char *)bmsegp + bmsegp->bms_bmcb[cache_ix]);
	    if (bm->bm_pgsize != pgsize)
	    {
		ule_format(E_DM937F_BM_NOT_A_BUFMGR, &sys_err, 
			ULE_LOG, NULL, (char *)0, (i4)0,
			(i4 *)0, err_code, 1, 0, cache_name);
		*err_code = E_DM9373_BM_CANT_CONNECT;
		status = E_DB_ERROR;
	    }
	}
    }

    return (status);

} /* bm_check_shrseg */

/*{
** Name: dm0p_alloc_pgarray	- Allocate a page array.
**
** Description:
**      This routine dynamically allocates a page array.
**
** Inputs:
**
** Outputs:
**      err_code                        Reason for FATAL status.
**					    E_DM930B_DM0P_DEALLOCATE
**					    E_DM9200_BM_BUSY
**	Returns:
**	    E_DB_OK
**	    E_DB_FATAL
**          E_DB_ERROR
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      06-mar-1996 (stial01)
**          Created for variable page size project.
**	22-Dec-1998 (jenjo02)
**	    Align page pool properly for RAWIO.
**	25-Apr-2007 (kibro01) b115996
**	    Ensure the misc_data pointer in a MISC_CB is set correctly and
**	    that we correctly allocate sizeof(DMP_MISC), not sizeof(DM_OBJECT)
**	    since that's what we're actually using (the type is MISC_CB)
*/
DB_STATUS
dm0p_alloc_pgarray(
i4             page_size,
i4             *err_code)
{
    DMP_LBMCB           *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BMCB           *bm = lbm->lbm_bmcb[DM_CACHE_IX(page_size)];
    DM0P_BM_COMMON      *bm_common = lbm->lbm_bm_common;
    char                *page_buf;
    DMPP_PAGE           *page_array;
    bool                duplicate_cache = FALSE;
    DB_STATUS           status;

    /*
    ** ALWAYS issue an warning to the log if this is requested,
    ** The dba should know there may be a configuration error
    ** alison FIX ME add error 
    */

    /* 
    ** We can only add a cache if this is not a shared buffer manager
    */
    if (bm_common->bmcb_status & BMCB_SHARED_BUFMGR)
    {
	/* alison FIX ME error to log */
	return (E_DB_ERROR);
    }

    /*
    ** We can only add a cache if we are the RCP or SINGLEUSER 
    **
    ** We only want to support this as a last resort for RCP and 
    ** JSP rollforwarddb where we may have been told not to allocate.
    ** a cache that we end up needing.
    */
    if (!(dmf_svcb->svcb_status & (SVCB_RCP | SVCB_SINGLEUSER)))
    {
	/* alison FIX ME error to log */
	return (E_DB_ERROR);
    }

    if (!bm)
    {
	/* 
	** We can't alloc the page array if we didnt previously
	** allocate the buffer array.
	** alison FIX ME error to log 
	*/
	return (E_DB_ERROR);
    }

    /* Allocate the page array, same number of pages as buffer array */
    status = dm0m_allocate((bm->bm_bufcnt * page_size) + sizeof(DMP_MISC)
	+ page_size,
	(i4) 0, (i4)MISC_CB, (i4)MISC_ASCII_ID,
	(char *) lbm, (DM_OBJECT **)&page_buf, err_code);

    if (status)
    {
	ule_format(*err_code, (CL_ERR_DESC *)0, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, err_code, 0);
	*err_code = E_DM0112_RESOURCE_QUOTA_EXCEED;
	return (E_DB_FATAL);
    }

    page_array = (DMPP_PAGE *)ME_ALIGN_MACRO((char *)page_buf 
			+ sizeof(DMP_MISC), page_size);
    ((DMP_MISC*)page_buf)->misc_data = (char*)page_array;

    /*
    ** Lock the buffer manager database
    **  Note: This doesn't seem to be necessary for the limited cases
    **        where we are allowing the page array to be allocated
    **        dynamically, but I'm locking the buffer manager database 
    **        just in case
    */

    /*
    ** Update the page array pointer for this cache
    */
    if (!lbm->lbm_pages[bm->bm_cache_ix])
	lbm->lbm_pages[bm->bm_cache_ix] = page_array; /* Now everyone can see the page array*/
    else
	duplicate_cache = TRUE; /* Somebody else already allocated it */

    if (duplicate_cache)
    {
	dm0m_deallocate((DM_OBJECT **)&page_buf);
    }

    return (E_DB_OK);
}

/*{
** Name: check_buffer - Checks the entire to see if the buffer is present
**			for the lock_key specified..
**
** Description:
**      This routine loops through all the caches to check if the buffer
**	exists given the lock_key.
**
** Inputs:
**	lock_id and lock_key.
** Outputs:
**	1	if buffer is found.
**	0	if buffer was not found.
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      01-feb-1999 (nanpr01)
**          Created for DMCM consistency check.
*/
static int 
check_buffer(
LK_LKID           *lockid,
LK_LOCK_KEY       *lock_key)
{
    DMP_LBMCB       *lbm = dmf_svcb->svcb_lbmcb_ptr;
    DM0P_BSTAT	    *lbm_stats;
    DM0P_BM_COMMON  *bm_common = lbm->lbm_bm_common;
    DM0P_BMCB       *bm;
    i4              cache_ix;
    register DM0P_BUFFER    *b;
    DM0P_BUFFER     *buf_array;
    DMPP_PAGE	    *page;
    int	     	    found = 0;
    register i4     i;
    i4	    local_error;

    for (cache_ix = 0; cache_ix < DM_MAX_CACHE; cache_ix++)
    {
	bm = lbm->lbm_bmcb[cache_ix];
	if (!bm || !(bm->bm_status & BM_ALLOCATED))
	    continue;

	buf_array = lbm->lbm_buffers[cache_ix];
	lbm_stats = &lbm->lbm_stats[cache_ix];

	    /*
	    ** Start each rescan at the first skipped buffer of the 
	    ** previous scan.
	    */
	    for (i = 0; i < bm->bm_bufcnt; i++)
	    {
		b = &buf_array[i];
		/* Lock the buffer */
		dm0p_mlock(SEM_EXCL, &b->buf_mutex);
		if (b->buf_lock_key.lk_type == lock_key->lk_type &&
		    b->buf_lock_key.lk_key4 == lock_key->lk_key4 &&
		    b->buf_lock_key.lk_key1 == lock_key->lk_key1 &&
		    b->buf_lock_key.lk_key2 == lock_key->lk_key2 &&
		    b->buf_lock_key.lk_key3 == lock_key->lk_key3)
		{
		    if (b->buf_sts & BUF_BUSY)
        	    {
			lbm_stats->bs_iowait[BMCB_PTYPES]++;
			bm->bm_stats.bs_iowait[BMCB_PTYPES]++;
			lbm_stats->bs_iowait[b->buf_type]++;
			bm->bm_stats.bs_iowait[b->buf_type]++;
            		b->buf_sts |= BUF_IOWAIT;
            		dm0p_eset(lbm->lbm_dmcm_lock_list, DM0P_BIO_EVENT,
			    b->buf_id.bid_cache, b->buf_id.bid_index, 0);
            		dm0p_munlock(&b->buf_mutex);
 
            		/* Wait for the I/O event to complete. */
            		dm0p_ewait(lbm->lbm_dmcm_lock_list, DM0P_BIO_EVENT,
			    b->buf_id.bid_cache, b->buf_id.bid_index, 0, 0);
		        i--;
            		continue;
        	    }
		    dm0p_munlock(&b->buf_mutex);
		    found = 1;
		    break;
		} 
		dm0p_munlock(&b->buf_mutex);
	    }
	    if (found)
		break;
    }

    return (found);
}

/*{
** Name: awaken_buf_waiters - Resume threads waiting on BUF_IOWAIT,
**	   		      BUF_MWAIT, and BUF_FCWAIT.
**
** Description:
**	Provides a consistent algorithm for awakening buffer waiters;
**
** Inputs:
**	b 		Buffer
**	lock_list	Session's lock list.
** Outputs:
**	none
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    Waiting threads are resumed.
**
** History:
**	21-May-2001 (jenjo02)
**	    Written.
**	15-Nov-2002 (jenjo02)
**	    Retain buf_mutex until all dm0p_erelease() calls
**	    have been made to prevent race condition with
**	    LKevent().
**	18-Mar-2003 (jenjo02)
**	    Retain buf_mutex in all cases.
*/
static VOID 
awaken_buf_waiters(
DM0P_BUFFER       *b,
i4	          lock_list)
{
    if ( b->buf_sts & BUF_MWAIT )
	dm0p_erelease(lock_list, DM0P_PGMUTEX_EVENT,
		    b->buf_id.bid_cache,
		    b->buf_id.bid_index, 0);
    if ( b->buf_sts & BUF_IOWAIT )
	dm0p_erelease(lock_list, DM0P_BIO_EVENT,
		    b->buf_id.bid_cache,
		    b->buf_id.bid_index, 0);
    if ( b->buf_sts & BUF_FCWAIT )
	dm0p_erelease(lock_list, DM0P_FCWAIT_EVENT,
		    b->buf_id.bid_cache,
		    b->buf_id.bid_index, 0);

    b->buf_sts &= ~(BUF_MWAIT | BUF_IOWAIT | BUF_FCWAIT);

    return;
}



/* Name: hashq_delete - Remove buffer from its hash queue
**
** Description:
**	This is a convenience routine to take a buffer off of its hash queue.
**	The buffer is marked as not being on any hash queue.
**
**	The caller is expected to mutex the buffer.  We'll mutex the hash queue
**	only for the duration of the unlinking.
**
** Inputs:
**	b		The buffer to be unlinked
**	bucket_array	Base of the hash queue headers for this cache
**	buf_array	Base of the buffer headers for this cache
**			(Caller is likely to have these last two handy anyway)
**
** Outputs:
**	none
**
** History:
**	27-Apr-2005 (schka24)
**	    Written.
**      18-Nov-2005 (horda03) Bug 115542/INGSRV3511
**          Only change the buf_hash_bucket when there
**          is no possibility for the thread to be descheduled, such that
**          another thread traversing the hash queue will always see the
**          buffer is not part of the link list and so restart its dirty scan.
**          On internal threads this could cause an infinite loop.
*/

static VOID
hashq_delete(DM0P_BUFFER *b,
	DM0P_HASH_HEAD *bucket_array, DM0P_BUFFER *buf_array)
{

    DM0P_HASH_HEAD *hhead;		/* Hash chain to unlink from */


    hhead = &bucket_array[b->buf_hash_bucket];

    dm0p_mlock(SEM_EXCL, &hhead->hash_mutex);
    
    /* Change the buf_hash_bucket now that the session holds the mutex.
    ** In an Ingres thread system, the session could let another session
    ** be scheduled because its been waiting too long for the mutex. If this
    ** new session happens to do a dirty scan of the hash queue this buffer is
    ** on the session will spin forever.
    */
    b->buf_hash_bucket = BUF_NOBUCKET;

    if (b->buf_next != BUF_ENDLIST)
	buf_array[b->buf_next].buf_prev = b->buf_prev;
    if (b->buf_prev != BUF_ENDLIST)
	buf_array[b->buf_prev].buf_next = b->buf_next;
    else
	hhead->hash_next = b->buf_next;
    dm0p_munlock(&hhead->hash_mutex);

} /* hashq_delete */



DB_STATUS
rnl_fixpage(
DMP_RCB		*rcb,
DMPP_PAGE	*page,
DM_PAGENO	page_number,
i4		*err_code)
{
    DMP_RCB		*r = rcb;
    DMP_TCB		*t = r->rcb_tcb_ptr;
    DB_STATUS		status = E_DB_OK;
    DMP_RNL_ONLINE	*rnl = r->rcb_rnl_online;
    DMP_RNL_LSN		*rnl_lsn;
    DMP_MISC		*new_xmap;
    char		*new_map;
    i4			new_cnt;
    i4			size;
    DMP_MISC		*new_xlsn;
    DMP_RNL_LSN		*new_lsn;
    i4			new_max;
    i4			i;


    do
    {
	if (page_number > rnl->rnl_xmap_cnt)
	{

	    /* Need to realloc bigger rnl_map */
	    new_cnt = page_number + (page_number/5);
	    if (new_cnt > rnl->rnl_dm1p_max)
		new_cnt = rnl->rnl_dm1p_max;
	    size = sizeof(DMP_MISC) + ((new_cnt + 1) / 8) + 1;

	    status = dm0m_allocate( size, DM0M_ZERO, (i4) MISC_CB, 
		(i4) MISC_ASCII_ID, (char *) 0, 
		(DM_OBJECT **) &new_xmap, err_code);
	    if ( status != E_DB_OK )
		break;
	
	    if (DMZ_SRT_MACRO(13))
		TRdisplay("ONLINE MODIFY: %~t Realloc size %d %p xmap \
%d -> %d\n", 
		    sizeof(DB_TAB_NAME), t->tcb_rel.relid, 
		    size, new_xmap, rnl->rnl_xmap_cnt, new_cnt);

	    new_map = (char *)&new_xmap[1];
	    new_xmap->misc_data = (char *)new_map;
	    MEcopy(rnl->rnl_map, ((rnl->rnl_xmap_cnt + 1) / 8) + 1, new_map);
	    dm0m_deallocate((DM_OBJECT **)&rnl->rnl_xmap);
	    rnl->rnl_xmap = new_xmap;
	    rnl->rnl_map = new_map;
	    rnl->rnl_xmap_cnt = new_cnt;
	}

	/* Page has been updated after DM0LBSF log record */
	/* ONLY save lsn if we don't have an LSN for this page */
	if (BTtest(page_number, (char *)rnl->rnl_map) == 0) 
	{
	    if (DMZ_SRT_MACRO(13))
		TRdisplay("RNL page %d lsn low %d)\n", page_number,
		    DMPP_VPT_GET_LOG_ADDR_LOW_MACRO(t->tcb_rel.relpgtype,
			page));

	    if (rnl->rnl_lsn_cnt == rnl->rnl_lsn_max)
	    {
		new_max = rnl->rnl_lsn_max * 2;

		if (new_max > rnl->rnl_dm1p_max)
		     new_max = rnl->rnl_dm1p_max; 

		size = sizeof(DMP_MISC) + (sizeof(DMP_RNL_LSN) * new_max);

		status = dm0m_allocate( size, DM0M_ZERO, (i4) MISC_CB, 
		    (i4) MISC_ASCII_ID, (char *) 0, 
		    (DM_OBJECT **) &new_xlsn, err_code);
		if ( status != E_DB_OK )
		    break;

		if (DMZ_SRT_MACRO(13))
		    TRdisplay("ONLINE MODIFY: %~t Realloc size %d %p \
xlsn %d -> %d\n", 
			sizeof(DB_TAB_NAME), t->tcb_rel.relid, 
			size, new_xlsn, rnl->rnl_lsn_max, new_max);

		new_lsn = (DMP_RNL_LSN *) &new_xlsn[1];
		new_xlsn->misc_data = (char *)new_lsn;

		MEcopy((PTR)rnl->rnl_lsn,
			rnl->rnl_lsn_cnt * sizeof(DMP_RNL_LSN), new_lsn);
		dm0m_deallocate((DM_OBJECT **)&rnl->rnl_xlsn);
		rnl->rnl_xlsn = new_xlsn;
		rnl->rnl_lsn = new_lsn;
		rnl->rnl_lsn_max = new_max;
	    }

	    BTset(page_number, (char *)rnl->rnl_map);
	    rnl_lsn = rnl->rnl_lsn + rnl->rnl_lsn_cnt;
	    rnl_lsn->rnl_page = page_number;
	    MEcopy((PTR)
		 DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(t->tcb_rel.relpgtype, page),
		 sizeof(LG_LSN), (PTR)&rnl_lsn->rnl_lsn);
	    rnl->rnl_lsn_cnt++;
	}

    } while (FALSE);

    return (status);
}


/*
** Name: rfp_rnl_fixpage - check page lsn
**
** Description:
**      Determine if page is at the appropriate state in which to read it in.
**	If page lsn does not match lsn at time that page was rnl read, then
**	return E_DB_INFO with error code of E_DM9CB1 to rollforwarddb. 
**	For rollforwarddb of online modify only.
**
** Inputs:
**      rcb ptr  
**      page ptr
**	page_number
**	error code ptr
**
** Outputs:
**      error code	
**	    E_DM9CB1_PAGE_LSN_MISMATCH
**	status
**	    E_DB_OK
**	    E_DB_INFO
**
**      Exceptions:
**          none
**
** Side Effects:
**	n/a
**
** History:
**	08-apr-2004 (thaju02)
**	    Created.
**	07-may-2004 (thaju02)
**	    For rolldb, only read those pages that were read rnl during forward 
**	    online modify processing.
**	12-may-2004 (thaju02)
**	    Changed E_DM9666 to the online modify specific E_DM9CB1.
*/
DB_STATUS
rfp_rnl_fixpage(
DMP_RCB         *rcb,
DMPP_PAGE       *page,
DM_PAGENO       page_number,
i4              *err_code)
{
    DMP_RCB             *r = rcb;
    DMP_TCB             *t = r->rcb_tcb_ptr;
    DB_STATUS           status = E_DB_OK;
    DMP_RNL_ONLINE      *rnl = r->rcb_rnl_online;
    DMP_RNL_LSN         *rnl_lsn = rnl->rnl_lsn;
    LG_LSN		*pg_lsn = 
	DMPP_VPT_ADDR_PAGE_LOG_ADDR_MACRO(t->tcb_rel.relpgtype, page);
    i4			i;
    bool		read = FALSE;

    for (i = 0; i < rnl->rnl_lsn_cnt; i++)
    {
	if (page_number == rnl_lsn[i].rnl_page)
	{
	    if (LSN_LT(pg_lsn, &rnl_lsn[i].rnl_lsn))
	    {
		/* rolldb must wait for this page */
		rnl->rnl_lsn_wait = &rnl_lsn[i];
		*err_code = E_DM9CB1_RNL_LSN_MISMATCH;
		status = E_DB_INFO;
		break;
	    }
	    read = TRUE;
	}
    }

    if (LSN_LT(pg_lsn, &rnl->rnl_bsf_lsn))
	read = TRUE;

    if (!status && !read)
    {
	/* we did not rnl this page, skip it */
	*err_code = E_DM9CB2_NO_RNL_LSN_ENTRY;
	status = E_DB_INFO;
    }
    return(status);
}
