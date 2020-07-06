/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <me.h>
#include    <pc.h>
#include    <st.h>
#include    <bt.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ddb.h>
#include    <cs.h>
#include    <lk.h>
#include    <scf.h>
#include    <ulf.h>
#include    <ulm.h>
#include    <adf.h>
#include    <ade.h>
#include    <dmf.h>
#include    <tm.h>
#include    <sxf.h>
#include    <dmtcb.h>
#include    <dmrcb.h>
#include    <dmscb.h>
#include    <dmacb.h>
#include    <qefmain.h>
#include    <qsf.h>
#include    <qefrcb.h>
#include    <qefcb.h>
#include    <qefnode.h>
#include    <psfparse.h>
#include    <qefact.h>
#include    <qefqp.h>
#include    <qefdsh.h>
#include    <qefscb.h>
#include    <er.h>

#include    <dmmcb.h>
#include    <dmucb.h>
#include    <ex.h>

#include    <dudbms.h>
#include    <qefqeu.h>
#include    <qeuqcb.h>
#include    <rqf.h>
#include    <tpf.h>
#include    <qefkon.h>
#include    <qefqry.h>
#include    <qefcat.h>
#include    <qefprotos.h>
#include    <rdf.h>
/**
**
**  Name: QEQVALID.C - query validation routines
**
**  Description:
**      The routines in this file perform all query validation
**  semantics. This includes checking timestamps on relations
**  and the use of pattern matching in keys.
**
**  As a byproduct, the relations are opened and keys are produced. 
**
**          qeq_validate - validate relations: open as necessary
**			 - check for proper use of pattern matching
**			 - in keys
**	    qeq_ksort	 - key sorting and placement
**	    qeq_ade	 - initialize ADE_EXCBs.
**	    qeq_ade_base - initialize ADE_EXCB pointer array.
**          qeq_audit     - Audit access to table and views, also alarms
**	    qeq_vopen	 - Open a valid struct.
**
**
**  History:
**      1-jul-86 (daved)    
**          written
**	19-aug-87 (puree)
**	    modified qeq_validate for repositioning a table.
**	09-dec-87 (puree)
**	    modified qeq_validate not to return upon error.
**      10-dec-87 (puree)
**          Converted all ulm_palloc to qec_palloc
**	06-jan-88 (puree)
**	    Modified qeq_validate to set QEQP_OBSOLETE flag in an invalid DSH.
**	10-feb-88 (puree)
**	    Convert ulm_openstream to qec_mopen, rename qec_palloc to
**	    qec_malloc.
**	11-feb-88 (puree)
**	    Modify qeq_ade to initialize VLT dsh using ADE_SVIRGIN CX.
**	19-feb-88 (puree)
**	    Modify qeq_validate to follow the key list in positioning
**	    the first tuple for a multiple ranges keys.
**	22-feb-88 (puree)
**	    Modify qeq_validate to validate and set table lock modes based 
**	    on the estimated nunber of pages touched.
**	11-mar-88 (puree)
**	    remove DSH_QUEUE structure. DMT_CB's can be linked together
**	    using the standard jupiter control block header.
**	10-may-88 (puree)
**	    Modify qeq_validate to set obsolesence flag in DSH, not in QP.
**	08-aug-88 (puree)
**	    Modify qeq_validate to recognize E_DM0114_FILE_NOT_FOUND.
**	11-aug-88 (puree)
**	    Clean up lint errors.
**	22-aug-88 (puree)
**	    Modify qeq_validate to initialize ade_excb for qp_qual CX for
**	    QP nodes.
**	02-sep-88 (puree)
**	    Modify qeq_ade for new VLUP/VLT scratch space management.
**	    Rename fields in DSH and QEN_ADF.
**	05-oct-88 (puree)
**	    Modify transaction handling for invalid query.  If a table in
**	    the topmost action is invalid, we will just terminate the 
**	    current query without executing it.  If a table in other actions
**	    is invalid, the current query will aborted.  Terminating and
**	    aborting a query take their usual semantics.
**
**	    In this module, qeq_validate is modified to return E_DB_WARN
**	    if the action is the topmost one. Else, it returns E_DB_ERROR.
**	14-oct-88 (puree)
**	    Modify qeq_validate to handle E_DM0055_NONEXT during positioning.
**	02-dec-88 (puree)
**	    Fix qeq_ksort to break off from the comparing loop if the two
**	    OR lists do not have the same attribute.
**	19-feb-89 (paul)
**	    Enhanced for rules processing. qeq_ade extended to initialize 
**	    parameter arrays for CALLPROC statements.
**	04-apr-89 (jrb)
**	    Properly initialized db_prec field for decimal project.
**	18-apr-89 (paul)
**	    Modify to support cursor update and delete as regular query plans.
**	13-jun-89 (paul)
**	    Fix bug 6346; Add a new parameter to qeq_validate to indicate
**	    whether only the tables in the valid list should be opened or
**	    the tables should be opened and the action initialized.
**      07-jul-89 (jennifer)
**          Added routine to audit direct updates to secure system catalogs,
**          to audit the access of tables and views, to indicate which 
**          alarm events happened, and to re-qualify access to all 
**          views (This forces a view MAC check in DMF each time
**          a query is run).
**	07-aug-89 (neil)
**	    Add consistency check to avoid AV when a cb gets trashed.
**	23-Mar-90 (anton)
**	    Add CSswitch call to prevent stored procedure loops from
**	    hanging UNIX servers.
**	13-apr-90 (nancy)
**	    Bug 20408.  Determine if this node does a key_by_tid.  If so, don't
**	    position here since it is done in qen_orig().
**	18-dec-90 (kwatts)
**	    Smart Disk integration: Added code to check for a Smart Disk program
**	    and if appropriate point at it from the rcb and set position type
**	    to SDSCAN.
**	06-mar-91 (davebf)
**	    Protect against a retry loop due to a max-i4 vl_est_pages.
**	16-apr-91 (davebf)
**	    Enabled vl_size_sensitive test since OPC now sets it
**	25-jun-91 (stevet)
**	    Fix B38161: Return error E_QE0018_BAD_PARAM_IN_CB if parameter 
**          is not found when expected.
**	07-aug-91 (stevet)
**	    Fix B37995: Added new parameter to qeq_ksort to indicate whether
**	    finding the MIN of the MAX keys and MAX of the MIN keys is required.
**	07-aug-91 (davebf)
**	    Extended scope of vl_size_sensitive test to avoid scaling
**	    estimate for lock granularity purposes when not size sensitive
**	    (b39056)
**      09-sep-91 (mikem)
**          bug #39632
**          Changed the logic of qeq_validate() to take advantage of the new
**          returns added to DMT_OPEN.  DMT_OPEN now returns # of tuples and #
**          of pages of a table following an open.  The new logic now uses this
**          information rather than an extra DMT_SHOW call to get the same
**          information.
**
**          Prior to this change QEF, while validating DBP QEPs, would first
**          call DMT_SHOW for this information and subsequently call DMT_OPEN.
**          This extra call was expensive on platforms like the RS6000 or
**          DECSTATION where cross process semaphores must be implemented as
**          system calls.  This change allows QEF (qeq_validate()) to eliminate
**          the extra DMT_SHOW call.
**	15-oct-91 (bryanp)
**	    Initialize dmt_char_array in the dmt_show_cb.
**      12-dec-91 (davebf)
**          Avoid executing ahd_constant in qeq_validate when QEA_AGGF (b41134)
**	15-jan-92 (rickh)
**	    In qeq_validate, don't re-initialize node status when chasing
**	    the postfix list at record position time.
**	12-feb-1992 (nancy)
**	    Bug 39225 -- Add QEA_IF and QEA_RETURN to statement types that
**	    need build-key initialization so that local variables can be
**	    initialized. Modify logic so key-build is done first.
**	12-feb-1992 (nancy)
**	    Bug 38565 -- Add new error/return status for table size change -
**	    E_QE0301_TBL_SIZE_CHANGE - so that scf will NOT increment retry
**	    counter.
**	11-mar-92 (jhahn)
**	    Added support for QEF interrupts.
**      08-dec-92 (anitap)
**          Included header files qsf.h and psfparse.h
**	04_dec_92 (kwatts)
**	    Created a procedure qeq_ade_base to do the base to address
**	    conversion part of qeq_ade. Did this so that qeq_validate could
**	    call qeq_ade_prts to convert a table of bases to a table of
**	    pointers outside qeq_ade. Part of Smart Disk project.
**	10-mar-1993 (markg)
**	    Modified code in qeq_audit() to not flush the audit log each time
**	    a security alarm is triggered.
**	24-mar-93 (jhahn)
**	    Various fixes for support of statement level rules. (FIPS)
**	18-may-93 (nancy)
**	    Bug 48032.  Only do size_sensitive check if init_action is TRUE.
**	    This avoids QE0115 error in a dbp, which cannot be retried.  
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	20-jul-1993 (pearl)
**	    Bug 45912.    On the 2nd invocation of a db proc, the QEN_NOR's are
**          sorted, and may be in a different order than it is assumed to be
**          in on the 1st invocation, when the structures are newly created.
**          Change the traversal of the QEN_NOR structures when ordering the
**          keys within the ranges in qeq_ksort().  Get the pointer to the
**          QEN_NOR structure from the dsh_kor_nor array.  Change the parameter
**          from dsh_key to pointer to dsh strucutre so we can get at the
**          array.  Change call to qeq_ksort()  to pass dsh instead
**          of dsh_key.
**      5-oct-93 (eric)
**          Added support for resource lists, and valid structs on their own
**          actions.
**	31-jan-94 (jhahn)
**	    Fixed support for resource lists.
**	22-feb-94 (jhahn)
**	    Another fix for support of resource lists.
**      19-dec-94 (sarjo01) from 18-feb-94 (markm)
**          Added "mustlock" flag that ensures we don't do any dirty reads
**          when the query is an update (58681).
**      01-nov-95 (ramra01)
**          noadf_mask preparation for qef optimization, reduced calls to
**          qen_u21_execute_cx. Cleanup compiler warnings.
**	7-nov-95 (inkdo01)
**	    Changes to replace QEN_ADF, QEN_OJINFO structure instances in
**	    various QEN_NODE and QEF_AHD structures.
**      06-feb-96 (johnst)
**          Bug 72202
**          Changed qee_ade_base() arguments to match the interface of
**          qeq_ade_base() from which it was originally cloned off,
**          restoring the two {qen_base, num_bases} arguments instead of passing
**          the higher level struct QEN_ADF, as (eric) defined it.
**          The Smart Disk code in qeq_validate() needs to call both
**          qee_ade_base() and qeq_ade_base() passing in the actual address
**          arrays and number of bases used for the Smart Disk case.
**          This enables ICL SCAFS to work again.
**      16-may-96 (ramra01)
**          Bug 76552
**	    Size sensitive option needs to refresh rdf cache if plan fails.
**	10-jul-96 (nick)
**	    Don't set LK_X on core catlogs just 'cos they don't have
**	    timestamps set - #77614.  Tidy some variable usage.
**	18-jul-96 (nick)
**	    If an action's resource is open but hasn't yet been used, then
**	    ensure we update the statement number. #76670
**	 4-oct-96 (nick)
**	    Tidy some stuff.
**	17-oct-96 (nick)
**	    Tidy some more stuff.
**	20-dec-96 (kch)
**	    In qeq_validate(), we now only calculate the qe90 statistic
**	    collection overhead once. This change fixes bug 78122.
**      13-jun-97 (dilma04)
**          In qeq_vopen(), set DMT_CONSTRAINT flag in the DMT_CB, if 
**          executing procedure that supports integrity constraint.
**	11-Aug-1997 (jenjo02)
**	    Changed ulm_streamid from (PTR) to (PTR*) so that ulm
**	    can destroy those handles when the memory is freed.
**	19-Aug-1997 (jenjo02)
**	    Distinguish between private and shared ULM streams.
**	    Piggyback ulm_palloc() calls with ulm_openstream() where appropriate.
**	07-Nov-1997 (jenjo02)
**	    Restored assignment of ulm_streamid to dsh_streamid in qeq_validate().
**      18-nov-97 (stial01)
**          qeq_vopen() QEF may specify DMT_C_EST_PAGES.
**      02-feb-98 (stial01)
**          Back out 18-nov-97 change.
**	16-jul-98 (shust01)
**	    SEGV possible due to initializing qep_node at time local variable
**	    is definied.  It points 64 bytes into action structure, but action
**	    is a variable sized structure, and there are times it is only 64
**	    bytes in size (action = QEA_INVOKE_RULE, for example).
**	    qep_node is initialized later on if it is needed.
**      07-jul-98 (sarjo01)
**          Bug 88784: Be sure to bump sequence number in dmt_cb each time
**          a table resource is opened with qeq_vopen(). A looping db proc
**          that updates the same row can produce ambiguous update error
**          if it thinks the same instance of a statement updates a record
**          twice.
**	28-oct-1998 (somsa01)
**	    In qeq_vopen(), in the case of session temporary tables, its
**	    locks will reside on session lock list.  (Bug #94059)
**	12-nov-1998 (somsa01)
**	    In qeq_vopen(), refined check for a Global Session Temporary
**	    Table.  (Bug #94059)
**	03-Dec-1998 (jenjo02)
**	    Repaired page count change calculation.
**	08-dec-1998 (somsa01)
**	    In qeq_valid(), if we find an unused open valid struct, don't
**	    just replace qer_inuse_vl with it as qer_inuse_vl may be set.
**	    Instead, add the open valid struct to the chain of qer_inuse_vl,
**	    making it the first in the chain.  (Bug #79195)
**      17-may-1999 (stial01)
**          qeq_vopen() QEF will defer lock escalation to DMF.
[@history_template@]...
**	17-mar-1999 (somsa01)
**	    Added include of pc.h and lk.h to resolve references to locking
**	    ids.
**      15-may-2000 (stial01)
**          Remove Smart Disk code
**      22-nov-2000 (stial01)
**          Check for lock timer expired during table validation (B103150)
**	18-Jan-2001 (jenjo02)
**	    Subvert calls to qeu_secaudit(), qeu_secaudit_detail()
**	    if not C2SECURE.
**	2-jan-03 (inkdo01)
**	    Removed use of noadf_mask (added in Nov. 1995) to prevent
**	    concurrency errors.
**	8-jan-04 (toumi01)
**	    Add support for exchange nodes.
**	23-feb-04 (inkdo01)
**	    Changed qef_rcb->error to dsh->dsh_error for || thread safety
**	    (and others).
**	16-Jul-2004 (schka24)
**	    Remove qeq_iterate, never called.
**	16-dec-04 (inkdo01)
**	    Add various collID's.
**    25-Oct-2005 (hanje04)
**        Add prototype for qeq_vopen() to prevent compiler
**        errors with GCC 4.0 due to conflict with implicit declaration.
**	28-Dec-2005 (kschendel)
**	    Use DSH references whenever possible instead of QEF RCB
**	    references.  (This simplifies the code, eliminating many
**	    RCB references, and reflects the new primacy of the DSH
**	    thanks to the parallel query changes.)
**/


/*
** Forward Declarations.
*/

static
DB_STATUS
qeq_audit(
QEF_RCB      *qef_rcb,
QEE_DSH	     *dsh,
QEF_AUD      *aud,
DB_STATUS   *status );

static DB_STATUS
qeq_qpart_find(
QEE_DSH		*dsh,
QEN_ORIG	*orig);

static DB_STATUS
qeq_vopen(
        QEF_RCB     *qef_rcb,
        QEF_AHD     *action,
        QEE_DSH     *dsh,
        QEF_VALID   *vl);




GLOBALREF QEF_S_CB		*Qef_s_cb;

/*{
** Name: QEQ_VALIDATE	- validate an action
**
** Description:
**      The relations for this action are opened and checked 
**  for the proper timestamp. Keys are then created and checked
**  for the appropriate use of pattern matching characters. 
**
** Inputs:
**      qef_rcb
**	action			action to validate
**	    .ahd_valid		relation validation list
**	    .ahd_mkey		key builder
**	reset
**	init_action		TRUE if the action should be initialized
**				FALSE if only the valid list should be run
**	    
**
** Outputs:
**	qef_rcb
**	    .error.err_code	one of the following
**				E_QE0000_OK
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0019_NON_INTERNAL_FAIL
**				E_QE0002_INTERNAL_ERROR
**				E_QE0023_INVALID_QUERY
**				E_QE0105_CORRUPTED_CB - new generic CB error.
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    relations are opened and keys are created.
**
** History:
**	1-july-86 (daved)
**          written
**	14-feb-87 (daved)
**	    mark invalid queries as non-repeat so that cleanup will happen
**	    properly.
**	19-aug-87 (puree)
**	    reset dsh_qen_status->node_status when repositioning a table in 
**	    an orig node.
**	09-dec-87 (puree)
**	    return immediately on warning or error.  also zero out the
**	    action pointer in dsh in the beginning.  This is necessary
**	    for qeq_fetch to recognize that there is no row qualified
**	    on the specified constant expression.
**	06-jan-88 (puree)
**	    If a QP is found to be invalid, set the QEQP_OBSOLETE in the
**	    DSH qp_status flag so that qee_clean up will destroy the DSH
**	    in case of a repeat query plan.
**	01-feb-88 (puree)
**	    Reset dmt_db_id every time we validate.  This is a temporary
**	    fix to test shared query plan.  Resetting db id should be done
**	    in qee_fetch.
**	02-feb-88 (puree)
**	    add reset flag to the function arguments.
**	19-feb-88 (puree)
**	    Modify qeq_validate to follow the key list in positioning
**	    the first tuple for a multiple ranges keys.
**	22-feb-88 (puree)
**	    Implement table validation and set lock mode based on the
**	    estimated page count.
**	11-mar-88 (puree)
**	    remove DSH_QUEUE structure. DMT_CB's can be linked together
**	    using the standard jupiter control block header.
**	10-may-88 (puree)
**	    Mark dsh_qp_status for obsolesence, not qp_status.  QP is a
**	    read-only object to QEF.
**	08-aug-88 (puree)
**	    Watch for E_DM0114_FILE_NOT_FOUND from DMT_SHOW and DMT_OPEN.
**	    This error makes the query invalid.
**	22-aug-88 (puree)
**	    QP node may have query paramters for its qualification CX.
**	    The ade_excb must be initialized here.
**	19-sep-88 (puree)
**	    Fix bug setting DSH_QP_OBSOLETE flag.  The flag must be set in
**	    the DSH, not in QP as it was before the fix.
**	05-oct-88 (puree)
**	    Implement new transaction handling for invalid query.  If a table
**	    in the topmost action of a query is found invalid, return the
**	    error E_QE0023_INVALID_QUERY with status E_DB_WARN.  If the
**	    action is not the topmost one, return E_QE0023_INVALID_QUERY
**	    with status E_DB_ERROR.  This allows the caller to terminate or
**	    abort the current query as appropriate.
**	14-oct-88 (puree)
**	    Return E_DB_OK if DMF returns E_DM0055_NONEXT.  Many QEP nodes
**	    still need to be executed.  During DMR_GETs we will receive
**	    the required E_DM0055...
**	20-oct-88 (puree)
**	    Fix for size_sensitive semantics.
**      15-feb-89 (davebf)
**          Temporary disable of tuple count mismatch error after DMT_SHOW
**	18-apr-89 (paul)
**	    Modify to support cursor update and delete as regular query plans.
**	13-jun-89 (paul)
**	    Fix bug 6346; Add a new parameter to qeq_validate to indicate
**	    whether only the tables in the valid list should be opened or
**	    the tables should be opened and the action initialized.
**      07-jul-89 (jennifer)
**          Added routine to audit direct updates to secure system catalogs,
**          to audit the access of tables and views, to indicate which 
**          alarm events happened, and to re-qualify access to all 
**          views (This forces a view MAC check in DMF each time
**          a query is run).
**          Must audit success and failure.
**	07-aug-89 (neil)
**	    Add consistency check to avoid AV when cbs[vl_dmr_cb] gets trashed.
**	23-Mar-90 (anton)
**	    Add CSswitch call to prevent stored procedure loops from
**	    hanging UNIX servers.
**	13-apr-90 (nancy)
**	    Bug 20408.  Determine if this node does a key_by_tid.  If so, don't
**	    position here since it is done in qen_orig().
**	28-nov-90 (teresa)
**	    bug 5446.  Invalidate QP is size estimate is too far off from reality.
**	04-jan-91 (davebf)
**	    Init dmt_cb_shw.dmt_char_array
**	04-jan-91 (davebf)
**	    Don't destroy QP of QUEL shared QP when recompiled due to
**	    pattern matching characters in string parameter.
**	18-dec-90 (kwatts)
**	    Smart Disk integration: Added code to check for a Smart Disk program
**	    and if appropriate point at it from the rcb and set position type
**	    to SDSCAN. Also took opportunity to introduce a QEN_ORIG pointer and
**	    tidy up the code.
**	06-mar-91 (davebf)
**	    Protect against a retry loop due to a max-i4 vl_est_pages.
**	16-apr-91 (davebf)
**	    Enabled vl_size_sensitive test since OPC now sets it
**      25-jun-91 (stevet)
**          Fix B38161: Return error E_QE0018_BAD_PARAM_IN_CB if parameter
**          is not found when expected.
**	07-aug-91 (davebf)
**	    Extended scope of vl_size_sensitive test to avoid scaling
**	    estimate for lock granularity purposes when not size sensitive
**	    (b39056)
**      09-sep-91 (mikem)
**          bug #39632
**          Changed the logic of qeq_validate() to take advantage of the new
**          returns added to DMT_OPEN.  DMT_OPEN now returns # of tuples and #
**          of pages of a table following an open.  The new logic now uses this
**          information rather than an extra DMT_SHOW call to get the same
**          information.
**
**          Prior to this change QEF, while validating DBP QEPs, would first
**          call DMT_SHOW for this information and subsequently call DMT_OPEN.
**          This extra call was expensive on platforms like the RS6000 or
**          DECSTATION where cross process semaphores must be implemented as
**          system calls.  This change allows QEF (qeq_validate()) to eliminate
**          the extra DMT_SHOW call.
**	15-oct-91 (bryanp)
**	    Initialize dmt_char_array in the dmt_show_cb.
**      12-dec-91 (davebf)
**          Avoid executing ahd_constant in qeq_validate when QEA_AGGF (b41134)
**	14-jan-1992 (nancy)
**	    Bug #39225 -- local variables in dbp's do not get initialized in
**	    IF and RETURN statements.  Add types QEA_IF and QEA_RETURN to 
**	    section for initializing cx's to build keys. (need opc part also)
**	14-jan-1992 (nancy)
**	    Bug 38565 -- Add new error/return status for table size change -
**	    E_QE0301_TBL_SIZE_CHANGE - so that scf will NOT increment retry
**	    counter.  This allows a large insert to proceed when rules are
**	    fired which reference the table which is growing.
**	15-jan-92 (rickh)
**	    In qeq_validate, don't re-initialize node status when chasing
**	    the postfix list at record position time.
**	11-mar-92 (jhahn)
**	    Added support for QEF interrupts.
**	24-mar-93 (jhahn)
**	    Various fixes for support of statement level rules. (FIPS)
**	18-may-93 (nancy)
**	    Bug 48032.  Only do size_sensitive check if init_action is TRUE.
**	    If init_action is FALSE then we are inside a database procedure 
**	    and we don't need to know if the qep needs recompiling.  This
**	    avoids QE0301 error for table size change and thus avoids QE0115 
**	    error which is fatal dbp error.  
**	5-jul-93 (robf)
**	    Reworked ORANGE code, security auditing.
**      20-jul-1993 (pearl)
**          Bug 45912.    On the 2nd invocation of a db proc, the QEN_NOR's are
**          sorted, and may be in a different order than it is assumed to be
**          in on the 1st invocation, when the structures are newly created.
**          Change the traversal of the QEN_NOR structures when ordering the
**          keys within the ranges in qeq_ksort().  Get the pointer to the
**          QEN_NOR structure from the dsh_kor_nor array.  Change the parameter
**          from dsh_key to pointer to dsh strucutre so we can get at the
**          array.  Change call to qeq_ksort()  to pass dsh instead
**	    lists.
**	15-sep-93 (robf)
**	    Pass DMT_UPD_ROW_SECLABEL option to DMF when row security
**	    label attribute is being updated.
**	    Rework security auditing to get audit info from the action
**	    rather than the qp header. This allows action-specific auditing.
**          of dsh_key.
**	5-oct-93 (eric
**	    Added support for resource lists. This changes the way that we
**	    validate query plans. Now we validate resource lists, not valid
**	    lists.
**      7-jan-94 (robf)
**          Rework secure changes after mainline integration. 
**	31-jan-94 (jhahn)
**	    Fixed support for resource lists.  Moved some code into the
**	    procedure "qeq_release_dsh_resources".
**	22-feb-94 (jhahn)
**	    Another fix for support of resource lists.
**      19-jun-95 (inkdo01)
**          Collapse orig_tkey/ukey nat's into single orig_flag flag word.
**      21-aug-95 (ramra01)
**          setup for ORIGAGG
**      01-nov-95 (ramra01)
**          noadf_mask preparation for qef optimization, reduced calls to
**          qen_u21_execute_cx.
**	7-nov-95 (inkdo01)
**	    Changes to replace QEN_ADF, QEN_OJINFO structure instances in
**	    various QEN_NODE and QEF_AHD structures.
**	05-Dec-95 (prida01)
**	    Add qe90 tracing for open file. This is a dio and costs in the query
**	    it should be noted in the qe90 tracing. (bug 72937)
**      06-feb-96 (johnst)
**          Bug 72202
**          Changed qee_ade_base() arguments to match the interface of
**          qeq_ade_base() from which it was originally cloned off,
**          restoring the two {qen_base, num_bases} arguments instead of passing
**          the higher level struct QEN_ADF, as (eric) defined it.
**          The Smart Disk code in qeq_validate() needs to call both
**          qee_ade_base() and qeq_ade_base() passing in the actual address
**          arrays and number of bases used for the Smart Disk case.
**          This enables ICL SCAFS to work again.
**          Removed (ramra01) comment around qee_ade_base() call as it is now
**          fixed.
**      16-may-96 (ramra01)
**          Bug 76552
**          Size sensitive option needs to refresh rdf cache if plan fails.
**	10-jul-96 (nick)
**	    Remove unused variable.
**	18-jul-96 (nick)
**	    We weren't updating the statement number on resources which are
**	    open but not yet in use ( i.e. they are sitting on the free_vl
**	    list ).  This breaks the deferred update semantics in DMF. #76670
**	    Also, the addition of qe90 tracing on 5-dec-95 above broke 
**	    the error handling in this routine as it trashed status ; I've
**	    fixed this.
**	20-dec-96 (kch)
**	    We now only calculate the qe90 statistic collection overhead once.
**	    This change fixes bug 78122.
**	12-feb-97 (inkdo01)
**	    Changes for MS Access OR transform - such QEF_VALIDs skip most
**	    validation (since corr. table is in memory). Just have to plug
**	    addr of QEE_MEM_CONSTTAB struct in dmr_cb slot of dsh_cbs.
**	6-nov-97 (inkdo01)
**	    Avoid executing ahd_constant for QEA_SAGG (75803).
**	07-Nov-1997 (jenjo02)
**	    Restored assignment of ulm_streamid to dsh_streamid in qeq_validate().
**	13-nov-97 (inkdo01)
**	    Improved (?) prida fix for bug 72937. Grab stats for DMR_POSITION
**	    logic only.
**	16-jul-98 (shust01)
**	    SEGV possible due to initializing qep_node at time local variable
**	    is definied.  It points 64 bytes into action structure, but action
**	    is a variable sized structure, and there are times it is only 64
**	    bytes in size (action = QEA_INVOKE_RULE, for example).
**	    qep_node is initialized later on if it is needed.
**	27-jul-98 (hayke02)
**	    Switch off simple aggregate optimization for the max() function
**	    (orig_flag &= ~ORIG_MAXOPT) if we fail to position on the key
**	    value(s) (i.e. the call to qen_position() returns with status
**	    E_DB_WARN and the err_code is E_QE0015_NO_MORE_ROWS). This
**	    change fixes bug 91022.
**	21-jul-98 (inkdo01)
**	    Added support for topsort-less descending sorts.
**	02-sep-1998 (nanpr01)
**	    We must invalidate the RDF cache if the timestamp doesnot match.
**	    Actually QE0023 has been returned in many other cases where really
**	    an RDF invalidation is not required. This bug was noticed when
**	    I disabled RDF cache refresh for every DEFQRY and DEFCURS in
**	    pstshwtab.c.
**	03-Dec-1998 (jenjo02)
**	    Repaired page count change calculation.
**	08-dec-1998 (somsa01)
**	    If we find an unused open valid struct, don't just replace
**	    qer_inuse_vl with it as qer_inuse_vl may be set. Instead,
**	    add the open valid struct to the chain of qer_inuse_vl, making
**	    it the first in the chain.  (Bug #79195)
**	27-jan-2000 (hayke02)
**	    We now set AD_1RSVD_EXEDBP in the ADI_WARN block before
**	    the call to ade_execute_cx() to create the keys, if we are
**	    executing a DBP. This prevents adc_ixopt() being called,
**	    which in turn prevents incorrect results when a DBP
**	    parameter is used in multiple predicates. This change
**	    fixes bug 99239.
**	26-jul-00 (inkdo01)
**	    Only increment statement number if query plan requires deferred
**	    processing (refinement to fix for 76670).
**	30-jan-01 (inkdo01)
**	    Changes for hash aggregation (support QEA_HAGGF action).
**	20-apr-01 (inkdo01)
**	    Add ahd_agoflow ADF code to QEA_HAGGF initialization.
**	18-mar-02 (inkdo01)
**	    Add open call for sequences.
**	2-jan-03 (inkdo01)
**	    Removed use of noadf_mask (added in Nov. 1995) to prevent
**	    concurrency errors.
**	6-jan-03 (inkdo01)
**	    Fix to above fix to eliminate possible SEGV's (what a slob!).
**	8-jan-03 (inkdo01)
**	    Minor fix to sequence logic to open 'em even if no resources
**	    (could be a constant query).
**	06-Feb-2003 (jenjo02)
**	    If DMS_OPEN_SEQ fails, set err_code in qef_rcb.
**	12-feb-03 (inkdo01)
**	    Reset dms_db_id so repeat queries work ok.
**	29-dec-03 (inkdo01)
**	    DSH is now parameter, "function" replaces "reset".
**	2-feb-04 (inkdo01)
**	    Add preparation of qualified partitions bit map for partitioned
**	    tables.
**	25-feb-04 (inkdo01)
**	    Assign DSH addr to dmr_q_arg instead of QEF_CB for thread safety.
**	27-feb-04 (inkdo01)
**	    Changed dsh_qen_status from ptr to array of QEN_STATUSs to 
**	    ptr to arrays of ptrs.
**	11-mar-04 (inkdo01)
**	    Save key structure in qen_status for partitioned table keyed
**	    retrievals.
**	26-mar-04 (inkdo01)
**	    Save top action's ahd_valid ptr in DSH for part. opening.
**	1-apr-04 (inkdo01)
**	    Avoid dmf_call's from ORIG nodes unless it's for ORIGAGG (they
**	    interfere with || query logic).
**	8-apr-04 (inkdo01)
**	    Add dsh to QEF_CHECK_FOR_INTERRUPT macro.
**	12-may-04 (inkdo01)
**	    Support added slot in cbs array for master DMR_CB of 
**	    partitioned table.
**	14-may-04 (inkdo01)
**	    Fix above change for action header defined DMR_CB's.
**	8-Jun-2004 (schka24)
**	    Move above to vopen so that it happens for all vl opens.
**	11-june-04 (inkdo01)
**	    Change to use local dsh rather than global (eliminates 
**	    SEGV in || processing).
**	15-july-04 (inkdo01)
**	    Make same change for first dsh_act_ptr ref (not sure why I 
**	    didn't make this change before).
**	30-Jul-2004 (jenjo02)
**	    Use dsh_dmt_id rather than qef_dmt_id transaction context.
**	26-aug-04 (inkdo01)
**	    Enable global base arrays.
**	20-Sep-2004 (jenjo02)
**	    If ORIG keys haven't been made yet (like in a SQ),
**	    call qen_keyprep() to make them.
**	04-Nov-2004 (jenjo02)
**	    Only CSswitch on root DSH or if not OS-threaded.
**	13-Dec-2005 (kschendel)
**	    Remaining inline QEN_ADF changed to pointers, fix here.
**	    Don't ask for DMF qual unless there is really a qualification cx.
**	16-Jan-2006 (kschendel)
**	    Qen-status now addressable from XADDRS structure.
**	31-May-2006 (kschendel)
**	    Remove a little origagg dead code.
**	16-Jun-2006 (kschendel)
**	    Multi-dimensional partition qualification.
**	12-jan-06 (kibro01) b117485
**	    When a table open fails the error is in dmt_cb, since that
**	    was what was passed into dmf_call
**	12-apr-2007 (dougi)
**	    Init ahd_rskcurr CX for KEYSET processing.
**	13-Feb-2007 (kschendel)
**	    Replace CSswitch with better CScancelCheck.
**	17-dec-2007 (dougi)
**	    Support parameterized first/offset "n".
**	29-jan-2008 (dougi)
**	    Check for 0-valued first/offset "n", too.
*/
DB_STATUS
qeq_validate(
QEF_RCB		*qef_rcb,
QEE_DSH		*dsh,
QEF_AHD		*action,
i4		function,
bool		init_action )
{
    QEF_VALID	    *vl;
    QEE_VALID	    *qevl;
    DB_STATUS	    status = E_DB_OK;
    QEF_CB	    *qef_cb = dsh->dsh_qefcb;
    ADF_CB	    *adfcb = dsh->dsh_adf_cb;
    PTR		    *cbs = dsh->dsh_cbs;		/* control blocks for 
							** DMU, ADF 
							*/
    QEN_NODE	    *qep_node;
    QEN_ORIG	    *orig_node;
    DMR_CB	    *dmr_cb;
    DMT_CB	    *dmt_cb;
    QEN_TKEY	    *tkptr;
    QEN_ADF	    *qen_adf;
    ADE_EXCB	    *ade_excb;
    QEF_QP_CB       *qp = dsh->dsh_qp_ptr;
    i4		    dmr_index;
    bool	    key_by_tid;
    TIMERSTAT	    init_tstat;
    TIMERSTAT	    open_tstat;
    TIMERSTAT	    loop_tstat;
    i4		    i;
    i4		    val1 = 0;
    i4		    val2 = 0;
    QEE_RESOURCE    *resource;
    QEF_RESOURCE    *qefresource;
    ULM_RCB	    ulm;
    DMR_CHAR_ENTRY  dmr_char_entry;
    STATUS	    csret;
    i4		    cson;
    QEN_ADF	    *sjn_adf;	
    QEN_ADF	    *jn_adf;
    RDF_CB          rdf_cb;
    DB_STATUS       status1 = E_DB_OK;
    bool	    reset = ((function & FUNC_RESET) != 0);

    /* This call will prevent a server which implements CSswitch from
       getting into a loop if a user has a looping stored procedure. */

    CScancelCheck(dsh->dsh_sid);

    if (QEF_CHECK_FOR_INTERRUPT(qef_cb, dsh) == E_DB_ERROR)
	return (E_DB_ERROR); /* so user can abort a looping procedure */

    /* set up to allocate memory out of the dsh's stream */
    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
    ulm.ulm_streamid_p = &dsh->dsh_streamid;

    /* clean up action pointer, it will be set if qeq_validate completed */

    dsh->dsh_act_ptr = (QEF_AHD *)NULL;

    /* validate relations */
    if (!reset)
    {
	/* Give DMT a new statement number, so that it can properly maintain
	** the deferred update semantics (but only if deferred processing
	** is required).
	*/
	if (qp->qp_status & QEQP_DEFERRED)
		dsh->dsh_stmt_no = qef_cb->qef_stmt++;

	/* only validate the resource list before the execution of the
	** first action 
	*/
	if (action == qp->qp_ahd)
	{
	    /* for each resource */
	    for (i = 0; i < qp->qp_cnt_resources; i += 1)
	    {
		resource = &dsh->dsh_resources[i];
		qefresource = (QEF_RESOURCE *) resource->qer_qefresource;

		/* if this is a resource that needs validating */

		/* First check for in-memory table (MS Access OR transform) */
		if (qefresource->qr_type == QEQR_TABLE &&
		    qefresource->qr_resource.qr_tbl.qr_tbl_type
			== QEQR_MCNSTTBL)
		{
		    i4	tab_index;
		    QEF_MEM_CONSTTAB	*qef_con_p;
		    QEE_MEM_CONSTTAB	*qee_con_p;

		    tab_index = qefresource->qr_resource.qr_tbl.
			qr_lastvalid->vl_dmr_cb;
		    qef_con_p = qefresource->qr_resource.qr_tbl.
			qr_cnsttab_p;
		    qee_con_p = resource->qer_resource.qer_tbl.
							qer_cnsttab_p;
		    /* Init. MEM_CONSTTAB and stick addr in dsh_cbs */
		    qee_con_p->qer_tab_p = qef_con_p->qr_tab_p;
		    qee_con_p->qer_rowcount = qef_con_p->qr_rowcount;
		    qee_con_p->qer_rowsize = qef_con_p->qr_rowsize;
		    qee_con_p->qer_scancount = 0;
		    cbs[tab_index] = (PTR)qee_con_p;
				/* VERY IMPORTANT - stick QEE_MEM_CONSTTAB
				** ptr here for qen_orig to find later */
		    continue;	/* skip rest of the loop */
		}

		if (qefresource->qr_type == QEQR_TABLE &&
		     qefresource->qr_resource.qr_tbl.qr_tbl_type
							== QEQR_REGTBL &&
		     resource->qer_resource.qer_tbl.qer_free_vl == NULL &&
		     resource->qer_resource.qer_tbl.qer_inuse_vl == NULL
		    )
		{
		    vl = qefresource->qr_resource.qr_tbl.qr_valid;

		    status = qeq_vopen(qef_rcb, action, dsh, vl);
		    dmt_cb = (DMT_CB *) cbs[vl->vl_dmf_cb];

		    if (status != E_DB_OK)
			break;

		    /* 
		    ** if there aren't empty resource structs then
		    ** allocate them, otherwise take one off the list.
		    */
		    if (resource->qer_resource.qer_tbl.qer_empty_vl == NULL)
		    {
			ulm.ulm_psize = sizeof (QEE_VALID);
			status = qec_malloc(&ulm);
			if (status != E_DB_OK)
			{
			    dsh->dsh_error.err_code = ulm.ulm_error.err_code;
			    return (status);
			}
                        /* cast to quiesce the compiler warning */
			qevl = (QEE_VALID *)ulm.ulm_pptr; 
		    }
		    else
		    {
			qevl = resource->qer_resource.qer_tbl.qer_empty_vl;
			resource->qer_resource.qer_tbl.qer_empty_vl = 
							    qevl->qevl_next;
			if (qevl->qevl_next != NULL)
			    qevl->qevl_next->qevl_prev = NULL;
			qevl->qevl_next = qevl->qevl_prev = NULL;
		    }

		    qevl->qevl_qefvl = (PTR) vl;
		    if (resource->qer_resource.qer_tbl.qer_free_vl == NULL)
		    {
			qevl->qevl_next = qevl->qevl_prev = NULL;
			resource->qer_resource.qer_tbl.qer_free_vl = qevl;
		    }

		    /* this is the real validation step for tables */
		    if (vl->vl_timestamp.db_tab_high_time != 
			dmt_cb->dmt_timestamp.db_tab_high_time ||
			vl->vl_timestamp.db_tab_low_time !=
			dmt_cb->dmt_timestamp.db_tab_low_time)
		    {
			/* relation out of date */
			dsh->dsh_qp_status |= DSH_QP_OBSOLETE;
			dsh->dsh_error.err_code = E_QE0023_INVALID_QUERY;
			status = E_DB_WARN;

                        qeu_rdfcb_init((PTR) &rdf_cb, qef_cb);
                        rdf_cb.rdf_rb.rdr_tabid.db_tab_base = 
                                           dmt_cb->dmt_id.db_tab_base;
                        rdf_cb.rdf_rb.rdr_tabid.db_tab_index = 
                                           dmt_cb->dmt_id.db_tab_index;                           
                        status1 = rdf_call(RDF_INVALIDATE, (PTR) &rdf_cb);
                        if (DB_FAILURE_MACRO(status1))
                        {
                            dsh->dsh_error.err_code = rdf_cb.rdf_error.err_code;
                            return(status1);
                        }
			break;
		    }

		    /* 
		    ** The semantics of the size_sensitive field in the valid
		    ** list is defined below:
		    **
		    **   If the size_sensitive is FALSE, the query plan is
		    **   totally independent of actual number of pages in the
		    **   relation
		    ** 
		    **   If the size_sensitive is TRUE, the the estimated number
		    **   of affected pages in the query plan must be adjusted by
		    **   the proportion of the actual page count (from DMF) to
		    **   the estimated page count (in the query plan).
		    **
		    **   If the difference of the adjusted value and the 
		    **   estimated number (specified in the query plan) is 
		    **   greater than a certain level (2 times + 5), the 
		    **   query plan is invalid.
		    */

		    /* Skip all this if the number of pages hasn't changed */
		    if (vl->vl_size_sensitive && init_action == TRUE &&
			vl->vl_total_pages != dmt_cb->dmt_page_count)
		    {
			i4	adj_affected_pages, adj_est_pages;
			i4 growth_factor;

			/*
			** scale estimated pages by growth factor, if any,
			** using integer arithmetic...
			*/
			growth_factor = ((dmt_cb->dmt_page_count -
					  vl->vl_total_pages) * 100)
					 / vl->vl_total_pages;

			/* Account for growth or shrinkage */
			growth_factor = +growth_factor;
			
			adj_affected_pages = vl->vl_est_pages + 
				((vl->vl_est_pages * growth_factor) / 100);

			/* if the scaled estimated pages is more than
			** 2 times the original estimate + 5, cause a recompile 
			** with new RDF cache */
		    
			/* 
			** calculate comparand so that overflow becoming 
			** negative can be detected 
			*/
			adj_est_pages = (2 * vl->vl_est_pages) + 5;

			if (adj_affected_pages > adj_est_pages)
			{
			    dsh->dsh_qp_status |= DSH_QP_OBSOLETE;
			    dsh->dsh_error.err_code = E_QE0301_TBL_SIZE_CHANGE;
			    status = E_DB_WARN;

                            qeu_rdfcb_init((PTR) &rdf_cb, qef_cb);
                            rdf_cb.rdf_rb.rdr_tabid.db_tab_base = 
                                           dmt_cb->dmt_id.db_tab_base;
                            rdf_cb.rdf_rb.rdr_tabid.db_tab_index = 
                                           dmt_cb->dmt_id.db_tab_index;                           
                            status1 = rdf_call(RDF_INVALIDATE, (PTR) &rdf_cb);
                            if (DB_FAILURE_MACRO(status1))
                            {
                                dsh->dsh_error.err_code = 
                                      rdf_cb.rdf_error.err_code;
                                return(status1);
                            }

			    break;
			}
		    }   /* end if size_sensitive */
		}
	    }
	    /* For each sequence (if any) - open with DMF call. */
	    {
		DMS_CB		*dms_cb;
		QEF_SEQUENCE	*qseqp;

		for (qseqp = qp->qp_sequences; qseqp; qseqp = qseqp->qs_qpnext)
		{
		    dms_cb = (DMS_CB *)dsh->dsh_cbs[qseqp->qs_cbnum];
		    dms_cb->dms_tran_id = dsh->dsh_dmt_id;
		    dms_cb->dms_db_id = qef_rcb->qef_db_id;
		    status = dmf_call(DMS_OPEN_SEQ, dms_cb);
		    if (status != E_DB_OK)
		    {
			dsh->dsh_error.err_code = 
			      dms_cb->error.err_code;
			return(status);
		    }
		}
	    }

	} /* end if first action */

	if (action->ahd_valid)
	    dsh->dsh_ahdvalid = action->ahd_valid;
					/* save for partition opening */
	for (vl = action->ahd_valid;
		status == E_DB_OK && vl != NULL; 
		vl = vl->vl_next
	    )
	{
	    bool vl_found = FALSE;

	    if (vl->vl_flags & QEF_MCNSTTBL) continue;
				/* skip all this stuff for in-memory tabs */
	    resource = &dsh->dsh_resources[vl->vl_resource->qr_id_resource];

	    /* if there's an unused open valid struct then use that */
	    if (resource->qer_resource.qer_tbl.qer_free_vl != NULL &&
		 resource->qer_resource.qer_tbl.qer_free_vl->qevl_qefvl == 
								    (PTR) vl
		)
	    {
		resource->qer_resource.qer_tbl.qer_free_vl->qevl_next =
			resource->qer_resource.qer_tbl.qer_inuse_vl;
		resource->qer_resource.qer_tbl.qer_inuse_vl = 
			resource->qer_resource.qer_tbl.qer_free_vl;
		resource->qer_resource.qer_tbl.qer_free_vl = NULL;
		vl_found = TRUE;
	    }
	    else
	    {
		/* See if we've already opened the table */
		for (qevl = resource->qer_resource.qer_tbl.qer_inuse_vl;
			qevl != NULL;
			qevl = qevl->qevl_next
		    )
		{
		    if (qevl->qevl_qefvl == (PTR) vl)
		    {
			vl_found = TRUE;
			break;
		    }
		}
	    }

	    if (!vl_found)
	    {
                /*
                ** Open the table and add it to the inuse list
                */
                if (resource->qer_resource.qer_tbl.qer_empty_vl == NULL)
                {
                    ulm.ulm_psize = sizeof (QEE_VALID);
                    status = qec_malloc(&ulm);
                    if (status != E_DB_OK)
                    {
                        dsh->dsh_error.err_code = ulm.ulm_error.err_code;
                        return (status);
                    }
                    /* quiesce the compiler warning */
                    qevl = (QEE_VALID *)ulm.ulm_pptr;
                }
                else
                {
                    qevl = resource->qer_resource.qer_tbl.qer_empty_vl;
                    resource->qer_resource.qer_tbl.qer_empty_vl =
                            qevl->qevl_next;
                    if (qevl->qevl_next != NULL)
                        qevl->qevl_next->qevl_prev = NULL;
                    qevl->qevl_next = qevl->qevl_prev = NULL;
                }

                qevl->qevl_qefvl = (PTR) vl;
                if (resource->qer_resource.qer_tbl.qer_inuse_vl == NULL)
                {
                    qevl->qevl_next = qevl->qevl_prev = NULL;
                    resource->qer_resource.qer_tbl.qer_inuse_vl = qevl;
                }
                else
                {
                    qevl->qevl_next =
                        resource->qer_resource.qer_tbl.qer_inuse_vl;
                    qevl->qevl_prev = NULL;
                    qevl->qevl_next->qevl_prev = qevl;
                    resource->qer_resource.qer_tbl.qer_inuse_vl = qevl;
                }

                status = qeq_vopen(qef_rcb, action, dsh, vl);
                if (status != E_DB_OK)
                    break;
	    }
		/* 
		** If we've opened the table already, then DMR_ALTER the 
		** statement number
		*/
	    dmt_cb = (DMT_CB *) cbs[vl->vl_dmf_cb];
	    if (dmt_cb->dmt_sequence != dsh->dsh_stmt_no)
	    {
	        /* set up to pass it to DMF */
	        dmr_char_entry.char_id = DMR_SEQUENCE_NUMBER;
	        dmr_char_entry.char_value = dsh->dsh_stmt_no;
	        dmt_cb->dmt_sequence = dsh->dsh_stmt_no;

	        dmr_cb = (DMR_CB *) cbs[vl->vl_dmr_cb];
	        dmr_cb->dmr_char_array.data_in_size = 
					    sizeof(DMR_CHAR_ENTRY);
	        dmr_cb->dmr_char_array.data_address = 
					    (PTR)&dmr_char_entry;

	        status = dmf_call(DMR_ALTER, dmr_cb);
	        if (DB_FAILURE_MACRO(status))    
	        {
		    dsh->dsh_error.err_code = dmr_cb->error.err_code;
		    break;
		}
	    }

	    /*
	    ** Make sure the table was locked with the correct lock mode
	    **
	    ** If the table open open failed with lock timeout while 
	    ** validating ALL query plan table resources, we performed the
	    ** table validation with readlock=nolock which requests a table
	    ** control lock.
	    **
	    ** The current action requires this table resource so we must 
	    ** now try to lock the table with the correct lock mode.
	    ** Try to open a new cursor with the correct lock mode.
	    ** 
	    ** We could alter the lock mode... (the way it is done in scsraat)
	    ** but this won't necessarily get the table lock mode we would
	    ** have gotten by calling open (which uses session lockmode info)
	    **
	    ** Close the cursor opened with readlock nolock AFTER we request
	    ** the lock we need, so that the control lock is held while
	    ** the table lock is requested.
	    */ 
	    if (dmt_cb->dmt_lock_mode == DMT_N)
	    {
		PTR		nolock_access_id;
		PTR		new_access_id;

		nolock_access_id = dmt_cb->dmt_record_access_id;
		dmt_cb->dmt_record_access_id = (PTR)0;

		/* default for lock granularity setting purposes */
		switch (dmt_cb->dmt_access_mode)
		{
		    case DMT_A_READ:
		    case DMT_A_RONLY:
			dmt_cb->dmt_lock_mode = DMT_IS;
			break;

		    case DMT_A_WRITE:
			dmt_cb->dmt_lock_mode = DMT_IX;
			break;
		}
		status = dmf_call(DMT_OPEN, dmt_cb);
		if (DB_FAILURE_MACRO(status))    
		{
		    if (dmt_cb->error.err_code == E_DM004D_LOCK_TIMER_EXPIRED)
			dsh->dsh_error.err_code = E_QE1004_TABLE_NOT_OPENED;
		    else
			dsh->dsh_error.err_code = dmt_cb->error.err_code;
		    dmt_cb->dmt_record_access_id = nolock_access_id;
		    break;
		}
		else
		{
		    new_access_id = dmt_cb->dmt_record_access_id;
		    dmt_cb->dmt_record_access_id = nolock_access_id;
		    status = dmf_call(DMT_CLOSE, dmt_cb);
		    dmt_cb->dmt_record_access_id = new_access_id;
		    /* set the table record access ids into the DMR_CBs */
		    ((DMR_CB*) cbs[vl->vl_dmr_cb])->dmr_access_id =
					    dmt_cb->dmt_record_access_id;
		    
		    if (status)
		    {
			i4 err;
			qef_error(dmt_cb->error.err_code, 0L, status, &err,
			    &dsh->dsh_error, 0);
			break;
		    }
		}
	    }

	}   /* end of the validation loop */

	if (status != E_DB_OK)
	{
	    if ((dsh->dsh_error.err_code == E_QE0023_INVALID_QUERY ||
	         dsh->dsh_error.err_code == E_QE0301_TBL_SIZE_CHANGE) &&
		dsh->dsh_qp_ptr->qp_ahd != action)
	    {
		status = E_DB_ERROR;
	    }
	    return (status);
	}

	/* 
	** Now we must handle any security audit requirements.
        */
	if  ( (action->ahd_audit != NULL) && 
	      (Qef_s_cb->qef_state & QEF_S_C2SECURE)
	    )
	{
	    QEF_AUD     *aud;
	    QEF_ART     *art;
	    i4          i;
  
	    aud = (QEF_AUD *)action->ahd_audit;
	    status = qeq_audit(qef_rcb, dsh, aud, &status);
	    if (status != E_DB_OK)
	    	return (status);	    
	}
	/*
	** Check if query plan needs QUERY_SYSCAT privilege, if so
	** make sure the session has it, and invalidate the query plan
	** if not.
	*/
        if (action->ahd_flags & QEA_NEED_QRY_SYSCAT)
	{
	    /*
	    ** Query needs QUERY_SYSCAT, so verify session has it
	    */
	    if (!(qef_cb->qef_fl_dbpriv & DBPR_QUERY_SYSCAT))
	    {
		/* Session missing priv, so invalidate */
		status = E_DB_WARN;
		dsh->dsh_qp_status |= DSH_QP_OBSOLETE;
		dsh->dsh_error.err_code = E_QE0023_INVALID_QUERY;
		return (status);
	    }
	}
    }	/* end of the if-not-reset block */
        
    /* At this point determine if this request is only to validate the
    ** query by opening all tables and checking timestamps or if we are
    ** actually about to execute the specified. If we are going to execute
    ** the specified action, the code below will initialize the action for
    ** processing. 'init_action' is TRUE in this case. 'init_action' is
    ** FALSE if we are only interested in opening the tables for the QP;
    ** this is the case after a ROLLBACK or COMMIT action has been taken.
    ** The actual problem being fixed when this code was added was that the
    ** action initialization for the first action in a DB procedure also
    ** initialized all local variables in the DB procedure to their default
    ** values. Thus, after a COMMIT or ROLLBACK in a DB procedure, all
    ** local variables lost their current values.
    */
    if (!init_action)
    {
	return (status);
    }

    /* Drop through IF we actually going to execute the specified action */

    /* put the query parameters into place */

    if (qef_rcb->qef_pcount && qef_rcb->qef_param)
        dsh->dsh_param	= qef_rcb->qef_param->dt_data;
    dsh->dsh_result	= (PTR) dsh->dsh_qef_output;

    /* initialize the queries begining cpu, dio, and wall clock numbers */
    if (ult_check_macro(&qef_cb->qef_trace, 91, &val1, &val2))
    {
	cson = 1;
	csret = CSaltr_session((CS_SID) 0, CS_AS_CPUSTATS, 
		    ( PTR ) &cson);

	if (csret != OK && csret != E_CS001F_CPUSTATS_WASSET)
	{
	    dsh->dsh_error.err_code = csret;
	    return(E_DB_ERROR);
	}

	status = qen_bcost_begin(dsh, &init_tstat, NULL);
	if (DB_FAILURE_MACRO(status))
	{
	    return(status);
	}

	dsh->dsh_tcpu = init_tstat.stat_cpu;
	dsh->dsh_tdio = init_tstat.stat_dio;
	dsh->dsh_twc = init_tstat.stat_wc;
    }

    /* initialize cpu and dio overhead numbers if needed */
    if (ult_check_macro(&qef_cb->qef_trace, 90, &val1, &val2))
    {
	cson = 1;
	csret = CSaltr_session((CS_SID) 0, CS_AS_CPUSTATS, 
		    ( PTR ) &cson);

	if (csret != OK && csret != E_CS001F_CPUSTATS_WASSET)
	{
	    dsh->dsh_error.err_code = csret;
	    return(E_DB_ERROR);
	}

	if ((dsh->dsh_cpuoverhead == 0.0) && (dsh->dsh_diooverhead == 0.0))
	{
	    status = qen_bcost_begin(dsh, &init_tstat, NULL);
	    if (DB_FAILURE_MACRO(status))
	    {
		return(status);
	    }

	    for (i = 1; i < 2000; i++)
	    {
		status = qen_bcost_begin(dsh, &loop_tstat, NULL);
		if (DB_FAILURE_MACRO(status))
		{
		    return(status);
		}
	    }

	    status = qen_ecost_end(dsh, &init_tstat, NULL);
	    if (DB_FAILURE_MACRO(status))
	    {
		return(status);
	    }

	    dsh->dsh_cpuoverhead = init_tstat.stat_cpu / 2000.0;
	    dsh->dsh_diooverhead = init_tstat.stat_dio / 2000.0;
	}
    }

    /* initialize the ADE_EXCBs */

    /*
    ** Fix B38161: Return E_QE0018_BAD_PARAM_IN_CB if qen_pcx_idx is
    **             positive but qef_pcount is 0.
    */
    if (action->ahd_mkey && 
		action->ahd_mkey->qen_pcx_idx && !qef_rcb->qef_pcount)
    {
        dsh->dsh_error.err_code = E_QE0018_BAD_PARAM_IN_CB;
        return (E_DB_ERROR);
    }

    /* 
    ** position the first record where appropriate if action contains a QEP.
    ** also check constant qualifications.
    */
    if ((action->ahd_atype > QEA_DMU && action->ahd_atype < QEA_UTL) ||
	    (action->ahd_atype == QEA_RAGGF) || 
	    (action->ahd_atype == QEA_HAGGF) || 
	    (action->ahd_atype == QEA_LOAD) ||
	    (action->ahd_atype == QEA_PUPD) || 
	    (action->ahd_atype == QEA_RETURN) || 
	    (action->ahd_atype == QEA_IF) || 
	    (action->ahd_atype == QEA_PDEL)
	)
    {

	/* Copy first "n" and offset "n" values for SELECTs or CTAS. */
	if ((action->ahd_atype == QEA_GET || action->ahd_atype == QEA_APP) &&
			action->qhd_obj.qhd_qep.ahd_firstncb >= 0)
	{
	    QEA_FIRSTN	    *firstncbp = (QEA_FIRSTN *)dsh->dsh_cbs[action->
						qhd_obj.qhd_qep.ahd_firstncb];
	    PTR		    valp;
	    bool	    offval = FALSE, firval = FALSE;

	    firstncbp->get_count = 0;

	    firstncbp->get_offsetn = action->qhd_obj.qhd_qep.ahd_offsetn;
	    if (action->qhd_obj.qhd_qep.ahd_qepflag & AHD_PARM_OFFSETN)
	    {
		if (qp->qp_status & QEQP_ISDBP)
		{
		    /* Procedure parameter or local variable - must be either
		    ** 2-byte integer or 4-byte integer. */
		
		    valp = dsh->dsh_row[action->qhd_obj.qhd_qep.ahd_offsetn];
		    if (qp->qp_row_len[action->qhd_obj.qhd_qep.ahd_offsetn] 
							== 2)
			firstncbp->get_offsetn = *(i2 *)valp;
		    else firstncbp->get_offsetn = *(i4 *)valp;
		}
		else
		{
		    /* Repeat query parm - load value from param array. */
		    valp = dsh->dsh_param[action->qhd_obj.qhd_qep.ahd_offsetn];
		    if (qp->qp_params[action->qhd_obj.qhd_qep.ahd_offsetn-1]->
							db_length == 2)
			firstncbp->get_offsetn = *(i2 *)valp;
		    else firstncbp->get_offsetn = *(i4 *)valp;
		}
		offval = TRUE;
	    }

	    firstncbp->get_firstn = action->qhd_obj.qhd_qep.ahd_firstn;
	    if (action->qhd_obj.qhd_qep.ahd_qepflag & AHD_PARM_FIRSTN)
	    {
		if (qp->qp_status & QEQP_ISDBP)
		{
		    /* Procedure parameter or local variable - must be either
		    ** 2-byte integer or 4-byte integer. */
		
		    valp = dsh->dsh_row[action->qhd_obj.qhd_qep.ahd_firstn];
		    if (qp->qp_row_len[action->qhd_obj.qhd_qep.ahd_firstn] == 2)
			firstncbp->get_firstn = *(i2 *)valp;
		    else firstncbp->get_firstn = *(i4 *)valp;
		}
		else
		{
		    /* Repeat query parm - load value from param array. */
		    valp = dsh->dsh_param[action->qhd_obj.qhd_qep.ahd_firstn];
		    if (qp->qp_params[action->qhd_obj.qhd_qep.ahd_firstn-1]
							->db_length == 2)
			firstncbp->get_firstn = *(i2 *)valp;
		    else firstncbp->get_firstn = *(i4 *)valp;
		}
		firval = TRUE;
	    }

	    /* Validate "first n"/"offset n" values. */
	    if (firval && firstncbp->get_firstn <= 0)
	    {
		dsh->dsh_error.err_code = E_QE00A8_BAD_FIRSTN;
		status = E_DB_ERROR;
		return(status);
	    }
	    else if (offval && firstncbp->get_offsetn < 0)
	    {
		dsh->dsh_error.err_code = E_QE00A9_BAD_OFFSETN;
		status = E_DB_ERROR;
		return(status);
	    }
	}

	if (action->ahd_atype == QEA_AGGF || 
	    action->ahd_atype == QEA_RAGGF ||
	    action->ahd_atype == QEA_HAGGF)    
	{
	    status = qeq_ade(dsh, 
			    action->qhd_obj.qhd_qep.ahd_by_comp,
			    (PTR *) dsh->dsh_param);
	    if (status) 
		return (status);
	}

	if (action->ahd_atype == QEA_HAGGF)    
	{
	    status = qeq_ade(dsh, 
			    action->qhd_obj.qhd_qep.u1.s2.ahd_agoflow,
			    (PTR *) dsh->dsh_param);
	    if (status) 
		return (status);
	}

        status = qeq_ade(dsh, 
    	    action->ahd_mkey,
    	    (PTR *) dsh->dsh_param);
        if (status) 
	    return (status);

	/* create the keys */
	qen_adf	= action->ahd_mkey;
	if (qen_adf != NULL)
	{
	    ade_excb    = (ADE_EXCB*) cbs[qen_adf->qen_pos];	
	    ade_excb->excb_seg = ADE_SMAIN;
	    if (dsh->dsh_qp_ptr->qp_qmode == QEQP_EXEDBP)
		qef_cb->qef_adf_cb->adf_warncb.ad_1rsvd_cnt |= AD_1RSVD_EXEDBP;
	    status = ade_execute_cx(adfcb, ade_excb);
	    adfcb->adf_warncb.ad_1rsvd_cnt &= ~AD_1RSVD_EXEDBP;
	    if (status != E_DB_OK)
	    {
		if ((status = qef_adf_error(&adfcb->adf_errcb,
			      status, qef_cb, &dsh->dsh_error)) != E_DB_OK)
		    return (status);
	    }
	    /* handle the condition where the qualification failed */
	    if (ade_excb->excb_value != ADE_TRUE)
	    {
		/* if qp->qp_status has bits set for QEQP_RPT 
		** and QEQP_SHAREABLE and not set for QEQP_SQL_QRY,
		** we don't destroy the query plan */
		if(!(dsh->dsh_qp_ptr->qp_status & QEQP_SHAREABLE) ||
		   (dsh->dsh_qp_ptr->qp_status & QEQP_SQL_QRY)    ||
		   !(dsh->dsh_qp_ptr->qp_status & QEQP_RPT)
		  )
		    dsh->dsh_qp_status |= DSH_QP_OBSOLETE; /* destroy plan */
		/* else don't destroy plan */

		/* force recompile */
		dsh->dsh_error.err_code = E_QE0023_INVALID_QUERY;
		if (dsh->dsh_qp_ptr->qp_ahd == action)
		    status = E_DB_WARN;
		else 
		    status = E_DB_ERROR;
		return (status);
	    }
	}

        if ((action->ahd_atype == QEA_RETURN) ||
	    (action->ahd_atype == QEA_IF))
	{
	    return (status);
	}

	status = qeq_ade(dsh,          
			action->qhd_obj.qhd_qep.ahd_current,
			(PTR *) dsh->dsh_param);
	if (status)
	    return (status);

	status = qeq_ade(dsh,         
			action->qhd_obj.qhd_qep.ahd_constant,
			(PTR *) dsh->dsh_param);
	if (status) 
	    return (status);

	if (action->qhd_obj.qhd_qep.ahd_qepflag & AHD_KEYSET)
	{
	    status = qeq_ade(dsh,         
			action->qhd_obj.qhd_qep.ahd_scroll->ahd_rskcurr,
			(PTR *) dsh->dsh_param);
	    if (status) 
		return (status);
	}

	/* check constant qualification	    
	** except in AGGF & SAGG which will evaluate this CX 
	** in the action interpreter */

	if (action->ahd_atype != QEA_AGGF && action->ahd_atype != QEA_SAGG)
	{
	    qen_adf	    = action->qhd_obj.qhd_qep.ahd_constant;
	    if (qen_adf)
	    {
		ade_excb    = (ADE_EXCB*) cbs[qen_adf->qen_pos];	
		ade_excb->excb_seg = ADE_SMAIN;

		/* process the constant expression */
		status = ade_execute_cx(adfcb, ade_excb);
		if (status != E_DB_OK)
		{
		    if ((status = qef_adf_error(&adfcb->adf_errcb, 
				status, qef_cb, &dsh->dsh_error)) != E_DB_OK)
			return (status);
		}
		/* handle the condition where the qualification failed */
		if (ade_excb->excb_value != ADE_TRUE)
		    goto noposition;
	    }
	}

	/* position records */
	for (qep_node = action->qhd_obj.qhd_qep.ahd_postfix; 
		    qep_node; qep_node = qep_node->qen_postfix)
	{
	    status = qeq_ade(dsh,         
			    qep_node->qen_fatts, 
			    (PTR *) dsh->dsh_param);
	    if (status) 
		return (status);

	    status = qeq_ade(dsh,         
			    qep_node->qen_prow,
			    (PTR *) dsh->dsh_param);
	    if (status) 
		return (status);

	    dmr_index = -1;
	    switch (qep_node->qen_type)
	    {
                case QE_FSMJOIN:
		case QE_ISJOIN:
		case QE_CPJOIN:
                    sjn_adf = qep_node->node_qen.qen_sjoin.sjn_joinkey;
		    status = qeq_ade(dsh,
				    sjn_adf,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

                    sjn_adf = qep_node->node_qen.qen_sjoin.sjn_okcompare;
		    status = qeq_ade(dsh,
				    sjn_adf,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);
                    sjn_adf = qep_node->node_qen.qen_sjoin.sjn_okmat;
		    status = qeq_ade(dsh,
				    sjn_adf, 
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

                    sjn_adf = qep_node->node_qen.qen_sjoin.sjn_itmat;
		    status = qeq_ade(dsh,
				    sjn_adf, 
				    (PTR *) dsh->dsh_param);
		    if (status) 
		    	return (status);

                    sjn_adf = qep_node->node_qen.qen_sjoin.sjn_jqual;
		    status = qeq_ade(dsh,
				    sjn_adf, 
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);
                    
		    /* Look at individual OJ ADF chunks */
		    if (qep_node->node_qen.qen_sjoin.sjn_oj == NULL)
			break;		/* no OJ, just exit case now */
                    sjn_adf = qep_node->node_qen.qen_sjoin.sjn_oj->oj_oqual;
		    status = qeq_ade(dsh,
			            sjn_adf,	
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

                    sjn_adf = qep_node->node_qen.qen_sjoin.sjn_oj->oj_equal; 
		    status = qeq_ade(dsh,
			            sjn_adf,	
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

                    sjn_adf = qep_node->node_qen.qen_sjoin.sjn_oj->oj_rnull; 
		    status = qeq_ade(dsh,
			            sjn_adf,	
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

                    sjn_adf = qep_node->node_qen.qen_sjoin.sjn_oj->oj_lnull;
		    status = qeq_ade(dsh,
			            sjn_adf,	
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);


		    break;

		case QE_SORT:
		    dmr_index = qep_node->node_qen.qen_sort.sort_load;
		    status = qeq_ade(dsh,
				    qep_node->node_qen.qen_sort.sort_mat,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

		    break;

		case QE_TSORT:
		    dmr_index = qep_node->node_qen.qen_tsort.tsort_load;
		    status = qeq_ade(dsh,
				    qep_node->node_qen.qen_tsort.tsort_mat,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

		    break;

		case QE_QP:
		    status = qeq_ade(dsh,
				    qep_node->node_qen.qen_qp.qp_qual,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

		    break;

		case QE_SEJOIN:
		    status = qeq_ade(dsh,
				    qep_node->node_qen.qen_sejoin.sejn_oqual,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

		    status = qeq_ade(dsh,
				    qep_node->node_qen.qen_sejoin.sejn_kqual,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

		    status = qeq_ade(dsh,
				    qep_node->node_qen.qen_sejoin.sejn_okmat,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

		    status = qeq_ade(dsh,
				    qep_node->node_qen.qen_sejoin.sejn_itmat,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

		    status = qeq_ade(dsh,
				 qep_node->node_qen.qen_sejoin.sejn_kcompare,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

		    status = qeq_ade(dsh,
				    qep_node->node_qen.qen_sejoin.sejn_ccompare,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

		    break;   

		case QE_TJOIN:
		    dmr_index = qep_node->node_qen.qen_tjoin.tjoin_get;

                    jn_adf = qep_node->node_qen.qen_tjoin.tjoin_jqual;
		    status = qeq_ade(dsh,
				    jn_adf,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

                    jn_adf = qep_node->node_qen.qen_tjoin.tjoin_isnull;
		    status = qeq_ade(dsh, 
				     jn_adf,
				    (PTR *) dsh->dsh_param);
		    if (status) return (status);
 
		    /* Look at individual OJ ADF chunks */
		    if (qep_node->node_qen.qen_tjoin.tjoin_oj == NULL)
			break;		/* no OJ, just exit case now */
                    jn_adf = qep_node->node_qen.qen_tjoin.tjoin_oj->oj_oqual;
		    status = qeq_ade(dsh,
				    jn_adf,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

                    jn_adf = qep_node->node_qen.qen_tjoin.tjoin_oj->oj_equal;
		    status = qeq_ade(dsh,
				     jn_adf,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

                    jn_adf = qep_node->node_qen.qen_tjoin.tjoin_oj->oj_rnull;
		    status = qeq_ade(dsh,
				     jn_adf,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

                    jn_adf = qep_node->node_qen.qen_tjoin.tjoin_oj->oj_lnull;
		    status = qeq_ade(dsh, 
				     jn_adf,
				    (PTR *) dsh->dsh_param);
                    if (status) 
			return (status);

		    jn_adf = qep_node->node_qen.qen_tjoin.tjoin_isnull;
		    status = qeq_ade(dsh, 
				     jn_adf,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);
		    break;

		case QE_HJOIN:
                    jn_adf = qep_node->node_qen.qen_hjoin.hjn_btmat;
		    status = qeq_ade(dsh,
				    jn_adf, 
				    (PTR *) dsh->dsh_param);
		    if (status) 
		    	return (status);

                    jn_adf = qep_node->node_qen.qen_hjoin.hjn_ptmat;
		    status = qeq_ade(dsh,
				    jn_adf, 
				    (PTR *) dsh->dsh_param);
		    if (status) 
		    	return (status);

                    jn_adf = qep_node->node_qen.qen_hjoin.hjn_jqual;
		    status = qeq_ade(dsh,
				    jn_adf, 
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);
                    
		    /* Look at individual OJ ADF chunks */
		    if (qep_node->node_qen.qen_hjoin.hjn_oj == NULL)
			break;		/* no OJ, just exit case now */
                    jn_adf = qep_node->node_qen.qen_hjoin.hjn_oj->oj_oqual;
		    status = qeq_ade(dsh,
			            jn_adf,	
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

                    jn_adf = qep_node->node_qen.qen_hjoin.hjn_oj->oj_equal; 
		    status = qeq_ade(dsh,
			            jn_adf,	
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

                    jn_adf = qep_node->node_qen.qen_hjoin.hjn_oj->oj_rnull; 
		    status = qeq_ade(dsh,
			            jn_adf,	
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

                    jn_adf = qep_node->node_qen.qen_hjoin.hjn_oj->oj_lnull;
		    status = qeq_ade(dsh,
			            jn_adf,	
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);


		    break;

		case QE_KJOIN:
		    dmr_index = qep_node->node_qen.qen_kjoin.kjoin_get;
		    dmr_cb = (DMR_CB*) cbs[dmr_index];
		    if (qep_node->node_qen.qen_kjoin.kjoin_iqual != NULL
		      && ! qep_node->node_qen.qen_kjoin.kjoin_tqual)
		    {

			/* these two assigns will cause lint errors */
			dmr_cb->dmr_q_fcn = (PTR) qen_dmr_cx;
			dmr_cb->dmr_q_arg = (PTR) dsh;
		    }
		    else
		    {
			dmr_cb->dmr_q_fcn = (PTR)NULL;
			dmr_cb->dmr_q_arg = (PTR)NULL;
		    }
                    jn_adf = qep_node->node_qen.qen_kjoin.kjoin_kqual;
		    status = qeq_ade(dsh,
				    jn_adf,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);
 
                    jn_adf = qep_node->node_qen.qen_kjoin.kjoin_jqual;  
		    status = qeq_ade(dsh,
				    jn_adf,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

                    jn_adf = qep_node->node_qen.qen_kjoin.kjoin_iqual;
		    status = qeq_ade(dsh,
				    jn_adf,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);
                
                    jn_adf = qep_node->node_qen.qen_kjoin.kjoin_kcompare; 
		    status = qeq_ade(dsh,
				     jn_adf,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

                    jn_adf = qep_node->node_qen.qen_kjoin.kjoin_key;
		    status = qeq_ade(dsh,
				    jn_adf,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

		    /* Look at individual OJ ADF chunks */
		    if (qep_node->node_qen.qen_kjoin.kjoin_oj == NULL)
			break;		/* no OJ, just exit case now */
                    jn_adf = qep_node->node_qen.qen_kjoin.kjoin_oj->oj_oqual;
		    status = qeq_ade(dsh,
				    jn_adf,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

                    jn_adf = qep_node->node_qen.qen_kjoin.kjoin_oj->oj_equal;
		    status = qeq_ade(dsh,
		              	    jn_adf,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

                    jn_adf = qep_node->node_qen.qen_kjoin.kjoin_oj->oj_rnull;
		    status = qeq_ade(dsh,
		              	    jn_adf,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

                    jn_adf = qep_node->node_qen.qen_kjoin.kjoin_oj->oj_lnull;
		    status = qeq_ade(dsh,
		              	    jn_adf,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);

		    break;

		case QE_ORIG:
	        case QE_ORIGAGG:
		  {
		    QEN_STATUS *ns;
		    QEN_PART_INFO *partp;

		    orig_node = &qep_node->node_qen.qen_orig;
		    dmr_index = orig_node->orig_get;
		    dmr_cb = (DMR_CB*) cbs[dmr_index];
		    status = qeq_ade(dsh,
				    orig_node->orig_qual,
				    (PTR *) dsh->dsh_param);
		    if (status) 
			return (status);
		    partp = orig_node->orig_part;
		    if (partp != NULL && partp->part_qual.part_qdim != NULL)
		    {
			QEN_PART_QUAL *pqual = &partp->part_qual;

			/* Run all the CX virgin segs first */
			do
			{
			    status = qeq_ade(dsh, pqual->part_qpart,
				    (PTR *) dsh->dsh_param);
			    if (status) 
				return (status);
			    pqual = pqual->part_qnext;
			} while (pqual != NULL);
			/* Now figure out the qualifying partitions */
			status = qeq_qpart_find(dsh, orig_node);
			if (status) 
			    return (status);
		    }

		    /* Init status, prior to possible break (for MS Access
		    ** OR transform) */
		    ns = dsh->dsh_xaddrs[qep_node->qen_num]->qex_status;
		    ns->node_status = QEN0_INITIAL;
		    ns->node_access = 0;
		    ns->node_rcount = 0;
		    ns->node_pcount = 0;
		    ns->node_cpu = 0;
		    ns->node_dio = 0;
		    ns->node_wc = 0;
		    ns->node_nbcost = 0;
		    ns->node_necost = 0;

		    if (orig_node->orig_flag & ORIG_MCNSTTAB)
		    {
			dmr_index = -1;
			if (orig_node->orig_qual)
			{	/* kludge to fill in buffer addr */
			    ADE_EXCB	*ade_excb;
			    ade_excb = (ADE_EXCB *)cbs[orig_node->
					orig_qual->qen_pos];
        		    ade_excb->excb_bases[orig_node->orig_qual->
				   qen_input + ADE_ZBASE] = 
				dsh->dsh_row[qep_node->qen_row];
				/* use ORIG node row buffer for qual code */
			}
			break;	/* rest is DMR fiddling */
		    } 

#ifdef xDEBUG
		    if (dmr_cb->dmr_access_id == (PTR)NULL)
		    {
			dsh->dsh_error.err_code = E_QE1004_TABLE_NOT_OPENED;
			status = E_DB_ERROR;
			return(status);
		    }
#endif /* xDEBUG */
		    /* Don't run qualification in DMF unless there is one,
		    ** and it doesn't involve tid.  (Tid is generated in
		    ** qef, the raw row contains no tid.)
		    */
		    dmr_cb->dmr_q_fcn = (PTR)NULL;
		    dmr_cb->dmr_q_arg = (PTR)NULL;
		    if (orig_node->orig_qual && (orig_node->orig_flag & ORIG_TKEY) == 0)
		    {
			dmr_cb->dmr_q_fcn = (PTR)qen_dmr_cx;
			dmr_cb->dmr_q_arg = (PTR)dsh;
		    }

		    /* 
		    ** If qe90, prepare node stats for DMR_POSITION (72937)
		    */
		    if (ult_check_macro(&qef_cb->qef_trace, 90, &val1, &val2))
    		    {
 			status = qen_bcost_begin(dsh, &open_tstat, ns);
        		if (DB_FAILURE_MACRO(status))
         		return(status);
    		    }

		    /* position the cursor to the first record */
		    if (orig_node->orig_pkeys == 0)
		    {
			dmr_cb->dmr_position_type = DMR_ALL;

			dmr_cb->dmr_flags_mask = 0;
			if (orig_node->orig_flag & ORIG_READBACK)
				dmr_cb->dmr_position_type = DMR_LAST;
			if (qep_node->qen_type == QE_ORIGAGG)
			{	/* only position non-partitioned tables */
			    status = dmf_call(DMR_POSITION, dmr_cb);
			    if (status != E_DB_OK)
			    {
				if (dmr_cb->error.err_code == E_DM0055_NONEXT)
				{
				    status = E_DB_OK;
				}
				else
				{
				    dsh->dsh_error.err_code = 
						dmr_cb->error.err_code;
				    return (status);
				}
			    }
			}
		    }
		    else
		    {
			bool	readback = (orig_node->orig_flag & 
							ORIG_READBACK);
			/* sort the keys and determine which ones to use */
			tkptr = (QEN_TKEY*) cbs[orig_node->orig_keys];
			/*
			** If orig_keys haven't been constructed yet,
			** call qen_keyprep() to do it.
			*/
			if ( tkptr == (QEN_TKEY*)NULL )
			{
			    if ( status = qen_keyprep(dsh, qep_node) )
				return(status);
			}
			else
			{
			    status = qeq_ksort(tkptr, orig_node->orig_pkeys,
				     dsh, (bool)TRUE, readback);
			    if (status != E_DB_OK)
				return(status);
			    ns->node_u.node_orig.node_keys = tkptr->qen_nkey;
					/* save for partitioned tables */
			}

			/* don't position on tids or unique keys because
			** these are handled in QEN_ORIG
			*/
			if (orig_node->orig_pkeys != NULL &&
			    orig_node->orig_pkeys->key_kor != NULL &&
			    orig_node->orig_pkeys->key_kor->kor_keys != NULL &&
			    orig_node->orig_pkeys->key_kor->kor_keys->kand_attno == 0
			    )
			{
			    key_by_tid = TRUE;
			}
			else
			{
			    key_by_tid = FALSE;
			}
			if (!(orig_node->orig_flag & ORIG_UKEY) &&
			   (key_by_tid == FALSE) && 
			    qep_node->qen_type == QE_ORIGAGG)
			{
			    dsh->dsh_qef_rowcount = 0;
			    status = qen_position(tkptr, dmr_cb, dsh,
				orig_node->orig_ordcnt, readback);
			    if (status != E_DB_OK)
			    {
				if (dsh->dsh_error.err_code == 
						E_QE0015_NO_MORE_ROWS)
				{
				    orig_node->orig_flag &= ~ORIG_MAXOPT;
				    status = E_DB_OK;
				}
				else
				    return (status);
			    }
			    if (dmr_cb->dmr_flags_mask == DMR_PREV)
				ns->node_access = QEN_READ_PREV;
						/* descending sort logic */
			}
		    }
		    /* 
		    ** If qe90, update node stats with cost of position (72937)
		    */
    		    if (ult_check_macro(&qef_cb->qef_trace, 90, &val1, &val2))
    		    {
			status = qen_ecost_end(dsh, &open_tstat, ns);
			if (DB_FAILURE_MACRO(status))
			return(status);
		    }
		  }

		    break;
 
	        case QE_EXCHANGE:
		/*
		** EXCH_FIXME we need validation logic here
		*/
		    break;

		default:
		    break;

	    }  /* end of switch (qep_node->qen_type) */

	    /* reset the row pointer in the DMR_CB for safety. */
	    if (dmr_index >= 0)
	    {
		/* set index for open cb */
		dmr_cb = (DMR_CB*) dsh->dsh_cbs[dmr_index];
		/* assign the output row and its size */
		dmr_cb->dmr_data.data_address = 
		    dsh->dsh_row[qep_node->qen_row];
		dmr_cb->dmr_data.data_in_size = qep_node->qen_rsz;
	    }		
	}
    }
    /* validate the remote update action */
    if (action->ahd_atype == QEA_RUP)
    {
	status = qeq_ade(dsh,
			action->qhd_obj.qhd_qep.ahd_current,
			(PTR *) dsh->dsh_param);
	if (status) 
	    return (status);

	status = qeq_ade(dsh,
			action->qhd_obj.qhd_qep.ahd_constant,
			(PTR *) dsh->dsh_param);
	if (status) 
	    return (status);

    }

    /* Set up the DSH structure by setting its current action pointer */
    dsh->dsh_act_ptr = action;

    dsh->dsh_error.err_code = E_QE0000_OK;
    return (status);

    /*
    ** We come here because we cannot position a table.  The action will
    ** not be executed.  This is good for most actions except for 
    ** aggregate function actions.  These actions need their ADE_SINIT
    ** ADE_SFINIT CX's to be executed, regardless.
    */
noposition:
    dsh->dsh_qef_rowcount = 0;
    dsh->dsh_error.err_code = E_QE0015_NO_MORE_ROWS;
    if (action->ahd_atype == QEA_AGGF ||
	action->ahd_atype == QEA_RAGGF ||
	action->ahd_atype == QEA_HAGGF ||
	action->ahd_atype == QEA_SAGG)
    {
	/* initialize the aggregate result */
	qen_adf = action->qhd_obj.qhd_qep.ahd_current;
	ade_excb = (ADE_EXCB*) cbs[qen_adf->qen_pos];	
	ade_excb->excb_seg = ADE_SINIT;
	if (qen_adf->qen_uoutput > -1)
	 if (qp->qp_status & QEQP_GLOBAL_BASEARRAY)
	    dsh->dsh_row[qen_adf->qen_uoutput] = dsh->dsh_qef_output->dt_data;
	 else ade_excb->excb_bases[ADE_ZBASE + qen_adf->qen_uoutput] =
		    dsh->dsh_qef_output->dt_data;

	status = ade_execute_cx(adfcb, ade_excb);
	if (status != E_DB_OK)
	{
	    if ((status = qef_adf_error(&adfcb->adf_errcb, status, 
		qef_cb, &dsh->dsh_error)) != E_DB_OK)
		return (status);
	}
	dsh->dsh_qef_remnull |= ade_excb->excb_nullskip;

	/* finalize the aggregate result now */
	ade_excb->excb_seg = ADE_SFINIT;
	status = ade_execute_cx(adfcb, ade_excb);
	if (status != E_DB_OK)
	{
	    if ((status = qef_adf_error(&adfcb->adf_errcb, status, 
		qef_cb, &dsh->dsh_error)) != E_DB_OK)
		return (status);
	}
    }
    return (E_DB_WARN);
}

/*{
** Name: QEQ_VOPEN	- Open a valid list entry.
**
** Description:
**      This handles opening a valid struct by setting up for and calling
**	DMT_OPEN. 
**
** Inputs:
**  qef_rcb -
**	The rcb for the current query
**  action -
**	the current action being executed
**  dsh -
**	the dsh for the current query
**  vl -
**	the valid struct to open
**
** Outputs:
**
**	Returns:
**	    success or error status.
**	Exceptions:
**	    none
**
** Side Effects:
**	    The table gets opened.
**
** History:
**      5-oct-93 (eric)
**          created by extracting out of qeq_validate.
**      19-dec-94 (sarjo01) from 18-feb-94 (markm)
**          Added "mustlock" flag that ensures we don't do any dirty reads
**          when the query is an update (58681).
**	10-jul-96 (nick)
**	    Change method of determining 'temporary tables' ; some core
**	    catalogs can have zero timestamps and this was causing exclusive 
**	    locks to be taken on them. #77614
**	    Change temporary_table variable from i4  to bool. 
**      13-jun-97 (dilma04)
**          Set DMT_CONSTRAINT flag in the DMT_CB, if executing procedure 
**          that supports integrity constraint.
**	28-oct-1998 (somsa01)
**	    In the case of session temporary tables, its locks will reside on
**	    session lock list.  (Bug #94059)
**	12-nov-1998 (somsa01)
**	    Refined check for a Global Session Temporary Table.  (Bug #94059)
**	01-dec-2000 (thaju02)
**	    Addition of DMT_FORCE_NOLOCK, to prevent dmt_open/dmt_set_lock_value
**	    from overriding the DMT_N lock mode. (cont. B103150)
**	2-jan-04 (inkdo01)
**	    Change qeq_vopen() to external function.
**	22-jan-04 (inkdo01)
**	    Change qeq_vopen() back to static function.
**	8-Jun-2004 (schka24)
**	    Move setting of partition-master dmrcb slot to here.
**	30-Jul-2004 (jenjo02)
**	    Use dsh_dmt_id rather than qef_dmt_id transaction context.
**	29-Oct-2004 (jenjo02)
**	    Temp tables are readily identified by vl_flags & QEF_SESS_TEMP;
**	    remove other unreliable tests, specifically db_tab_base <=
**	    DM_SCONCUR_MAX which is unfortunately true for temp tables
**	    now that db_tab_base is an i4 instead of a u_i4.
**	2-Mar-2005 (schka24)
**	    It turns out that the original test that Jon replaced (above) was
**	    broken.  Unfortunately, that's what made temp tables work.
**	    In particular, we must not assume that gtt's always get opened
**	    in DIRECT mode;  otherwise, insert/select queries like
**	    insert into gtt select * from gtt
**	    never terminate, as the select keeps seeing the rows just
**	    inserted.
**	    
[@history_template@]...
*/
static DB_STATUS
qeq_vopen(
	QEF_RCB	    *qef_rcb, 
	QEF_AHD	    *action, 
	QEE_DSH	    *dsh, 
	QEF_VALID   *vl)
{
    PTR		*cbs = dsh->dsh_cbs;
    DMT_CB	*dmt_cb;
    DB_STATUS	status = E_DB_OK;
    QEF_QP_CB	*qp = dsh->dsh_qp_ptr;
    DMT_CHAR_ENTRY  dmt_char_entry[2];

    for (;;)
    {
	/* 
	** If there is no control block, or the table is already opened,
	** skip validation for this entry
	*/

	dmt_cb = (DMT_CB *) cbs[vl->vl_dmf_cb];
	if (dmt_cb == (DMT_CB *)NULL)
	    break;

	if (dmt_cb->dmt_record_access_id != (PTR)NULL)
	    break;   /* table is already opened */

	/* For partitioned tables, copy master DMR_CB addr to 
	** vl_dmr_cb location in array.  (Unless master slot is
	** already set, in which case this must be a REPEATED re-use.)
	*/
	if (vl->vl_partition_cnt > 1)
	    if (dsh->dsh_cbs[vl->vl_dmr_cb - 1] != NULL)
		dsh->dsh_cbs[vl->vl_dmr_cb] = dsh->dsh_cbs[vl->vl_dmr_cb - 1];
	    else
		dsh->dsh_cbs[vl->vl_dmr_cb - 1] = dsh->dsh_cbs[vl->vl_dmr_cb];

	if (vl->vl_flags & QEF_SET_INPUT)
	{
	    STRUCT_ASSIGN_MACRO(* (DB_TAB_ID *)
				(dsh->dsh_row[vl->vl_tab_id_index]),
				dmt_cb->dmt_id);
	}

	dmt_cb->dmt_tran_id	= dsh->dsh_dmt_id;
	dmt_cb->dmt_db_id = qef_rcb->qef_db_id;
	dmt_cb->dmt_sequence = dsh->dsh_stmt_no;
        dmt_cb->dmt_mustlock = vl->vl_mustlock;
	dmt_cb->dmt_flags_mask = 0;

	if (qef_rcb->qef_qacc == QEF_READONLY)
	    dmt_cb->dmt_access_mode = DMT_A_READ;
	else
	    dmt_cb->dmt_access_mode = vl->vl_rw;

	if ((qp->qp_status & QEQP_DEFERRED) != 0)
	    dmt_cb->dmt_update_mode = DMT_U_DEFERRED;
	else
	    dmt_cb->dmt_update_mode = DMT_U_DIRECT;

	if ( vl->vl_flags & QEF_SESS_TEMP )
	{
	    /* Lock mode is kinda irrelevant with session temps.
	    ** We do however want to pass the session-temp flag so that
	    ** any gtt locks live on the SCB lock list, not the XCB.
	    */
	    dmt_cb->dmt_lock_mode = DMT_X;
	    dmt_cb->dmt_flags_mask |= DMT_SESSION_TEMP;
	}
	else
	{
	    /* Use the QEP's estimated number of pages to set lock mode.
	    ** Page level locking will be used unless the query plan 
	    ** estimates that more than 10 pages will be used, in which
	    ** case table level locking will be used.  Table level locking
	    ** is always used with temporary tables.
	    **
	    ** This is a change from the previous strategy where DMT_SHOW 
	    ** was used to retrieve the actual page count of the relation 
	    ** and then the lockmode choice was based on the estimated page
	    ** count scaled by the amount the table had grown since the 
	    ** query plan had been built.  The problem with this is that 
	    ** making an extra DMT_SHOW for every query in a compiled DBP 
	    ** (like tp1) is significant overhead (especially on some 
	    ** machines like the RS6000 and decstation where cross process 
	    ** semaphores are very expensive).
	    */

			
	    /* default for lock granularity setting purposes */
	    switch (dmt_cb->dmt_access_mode)
	    {
		case DMT_A_READ:
		case DMT_A_RONLY:
		    dmt_cb->dmt_lock_mode = DMT_IS;
		    break;

		case DMT_A_WRITE:
		    dmt_cb->dmt_lock_mode = DMT_IX;
		    break;
	    }

	    /*
	    ** Qef should not escalate to table locking if  vl_est_pages > 10.
	    ** Qef does not know the correct maxlock setting for this table.
	    ** 
	    ** QEF should also never escalate when a lower isolation level
	    ** has been specified.
	    ** QEF does not know the isolation level.
	    **
	    ** QEF could do a DMT_SHOW to find out these settings,
	    ** but to avoid an extra expensive DMT_SHOW, QEF will pass
	    ** the est_pages and total_pages values to dmf.
	    */
	    dmt_char_entry[0].char_id = DMT_C_EST_PAGES;
	    dmt_char_entry[0].char_value = vl->vl_est_pages;
	    dmt_char_entry[1].char_id = DMT_C_TOTAL_PAGES;
	    dmt_char_entry[1].char_value = vl->vl_total_pages;
	    dmt_cb->dmt_char_array.data_in_size = 2 * sizeof(DMR_CHAR_ENTRY);
	    dmt_cb->dmt_char_array.data_address = (PTR)dmt_char_entry;
	}	

        /*
        ** If executing a procedure that supports an integrity constraint, 
        ** set DMT_CONSTRAINTS flag in the DMT_CB.
        */
        if (qef_rcb->qef_intstate & QEF_CONSTR_PROC)
        {
            dmt_cb->dmt_flags_mask |= DMT_CONSTRAINT;
        }

	/* open and check time stamp for each non-temp table */

	status = dmf_call(DMT_OPEN, dmt_cb);
	if (status && dmt_cb->error.err_code == E_DM004D_LOCK_TIMER_EXPIRED)
	{
	    /* Try to do validate with lockmode DMT_N (B103150) */
	    dmt_cb->dmt_flags_mask |= DMT_FORCE_NOLOCK;
	    dmt_cb->dmt_lock_mode = DMT_N;
	    status = dmf_call(DMT_OPEN, dmt_cb);
	}

	/* Reset dmt_char_array for future calls */
	dmt_cb->dmt_char_array.data_address = 0;
	dmt_cb->dmt_char_array.data_in_size = 0;

        /*
        ** Turn off row update bit, since later calls may check for
        ** (in)valid flags_mask
        */
	if (status != E_DB_OK)
	{
	    if (dmt_cb->error.err_code == E_DM0054_NONEXISTENT_TABLE ||
		dmt_cb->error.err_code == E_DM0114_FILE_NOT_FOUND)
	    {
		dsh->dsh_qp_status |= DSH_QP_OBSOLETE;
		dsh->dsh_error.err_code = E_QE0023_INVALID_QUERY;
		status = E_DB_WARN;
	    }
	    else if (dmt_cb->error.err_code 
		      != E_DM0129_SECURITY_CANT_UPDATE)
	    {
		dsh->dsh_error.err_code = dmt_cb->error.err_code;
	    }
	    break;
	}

	/* Prepend the DMT_CB into a list of the opened tables in the
	** data segment header (DSH)
	*/
	dmt_cb->q_prev = (PTR)NULL;
	if (dsh->dsh_open_dmtcbs == (DMT_CB *)NULL)
	{
	    dmt_cb->q_next = (PTR)NULL;
	}
	else
	{
	    dsh->dsh_open_dmtcbs->q_prev = (PTR)dmt_cb;
	    dmt_cb->q_next = (PTR)(dsh->dsh_open_dmtcbs);
	}
	dsh->dsh_open_dmtcbs = dmt_cb;

	/* Consistency check due to common AV */
	if (cbs[vl->vl_dmr_cb] == NULL)
	{
	    i4		err;
	    char		reason[100];

	    status = E_DB_ERROR;
	    STprintf(reason, "vl_dmr_cb = %d, cbs[%d] = NULL, ahd_atype = %d",
		     vl->vl_dmr_cb, vl->vl_dmr_cb, action->ahd_atype);
	    qef_error(E_QE0105_CORRUPTED_CB, 0L, status, &err,
		      &dsh->dsh_error, 2,
		      sizeof("qeq_validate")-1, "qeq_validate",
		      STlength(reason), reason);
	    dsh->dsh_error.err_code = E_QE0025_USER_ERROR;
	    break;
	}
	/* set the table record access ids into the DMR_CBs */
	((DMR_CB*) cbs[vl->vl_dmr_cb])->dmr_access_id =
				dmt_cb->dmt_record_access_id;
    }

    return(status);
}

/*{
** Name: QEQ_KSORT	- key sorting and placement
**
** Description:
**      At this point, the user's parameters (dsh_param) and constants have
**  been converted to their key formats and reside in dsh_key. 
**  The QEF_KEY structures point at these keys. What we want to do is 
**  order the keys for maximum effectiveness. This requires that we
**  find the MIN of the MAX keys and the MAX of the MIN keys. Furthermore,
**  we need to sort the KOR clauses.
**
**  The first step is to order the keys within the ranges.
**  The second step is to order the ranges.
**  The third step is to set the current key pointer in qen_tkey to the
**  first. The first key is set in qeq_validate.
**
** Inputs:
**      qen_tkey                        top of actual keys pointer
**	qef_key				top of formal keys pointer
**	dsh_key				buffer containing keys
**	dsh				Data segment header for thread 
**
** Outputs:
**      qen_tkey			top of actual keys pointer
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    the elements pointed to by qen_tkey are filled in.
**
** History:
**      29-jul-86 (daved)
**          written
**	02-dec-88 (puree)
**	    break off from comparing OR keys loop if the keys are not the
**	    same attributes.
**	04-apr-89 (jrb)
**	    Initialized db_prec properly.
**      7-aug-91 (stevet)
**          Fix B37995: Added new parameter to qeq_ksort to indicate whether
**          finding the MIN of the MAX keys and MAX of the MIN keys is required.
**      20-jul-1993 (pearl)
**          Bug 45912.    On the 2nd invocation of a db proc, the QEN_NOR's are
**          sorted, and may be in a different order than it is assumed to be
**          in on the 1st invocation, when the structures are newly created.
**          Change the traversal of the QEN_NOR structures when ordering the
**          keys within the ranges in qeq_ksort().  Get the pointer to the
**          QEN_NOR structure from the dsh_kor_nor array.  Change the parameter
**          from dsh_key to pointer to dsh strucutre so we can get at the
**          array.  Change call to qeq_ksort()  to pass dsh instead
**          of dsh_key.
**	21-jul-98 (inkdo01)
**	    Support topsort-less descending sort by sorting ORs descending.
[@history_template@]...
*/
DB_STATUS
qeq_ksort(
QEN_TKEY           *qen_tkey,
QEF_KEY            *qef_key,
QEE_DSH            *dsh,
bool               keysort,
bool		   desc )
{
    ADF_CB	    *adf_cb;
    DB_STATUS	    status;
    QEF_KOR	    *qef_kor;
    QEF_KATT	    *qef_katt;
    QEF_KAND	    *qef_kand;
    QEN_NOR	    *qen_nor;
    QEN_NOR	    *qen_max;
    QEN_NOR	    *qen_1dup;	    /* duplicate values */
    QEN_NOR	    *qen_2dup;	    /* duplicate values */
    QEN_NOR	    **qen_prev;	    /* points to next list ptr */
    QEN_NOR	    **max_prev;	    /* pointer to next list ptr */
    QEN_NKEY	    *qen_nkey;
    PTR             dsh_key;
    DB_DATA_VALUE   adc_dv1;
    DB_DATA_VALUE   adc_dv2;
    i4		    result;

    if (qef_key == 0)
	return (E_DB_OK);

    adf_cb = dsh->dsh_adf_cb;
    dsh_key = dsh->dsh_key;
    /* 
    ** If keysort is TRUE, than order the keys within the ranges, if FALSE then
    ** just order the ranges.
    */
    if(keysort)
    {
	/* as we traverse the qef_key structure down the list of ranges (KOR)
	** nodes
	*/
    
	for 
	(
	    qef_kor = qef_key->key_kor;
	    qef_kor; 
	    qef_kor = qef_kor->kor_next
	)
	{

	    /* bug 45912: find the right QEN_NOR structure */
	    qen_nor = dsh->dsh_kor_nor[qef_kor->kor_id];
	    /* double check */
	    if (!qen_nor || qen_nor->qef_kor != qef_kor)
	    {
	    	dsh->dsh_error.err_code = E_QE0085_ERROR_SORTING;
		return (E_DB_ERROR);
	    }

	    /* find the min, max, or equality key for each attribute in the
	    ** range 
	    */
	    for
	    (
		qef_kand = qef_kor->kor_keys, qen_nkey = qen_nor->qen_key;
		qef_kand;
		qef_kand = qef_kand->kand_next, qen_nkey = qen_nkey->nkey_next
	    )    
	    {
		/* assume the first value is the desired key */
		qef_katt = qef_kand->kand_keys;
		qen_nkey->nkey_att = qef_katt;
		qen_nkey->nkey_type = qef_kand->kand_type;

		/* if this is a min or max list and there are more
		** elements in it, find the max or min.
		** Note that we know this is a min or max list if there are
		** multiple elements in it because we do not group equality
		** keys in this manner. Equality keys can be grouped in
		** separate ranges (KOR lists).
		*/
		for 
		(
		    qef_katt = qef_katt->attr_next;
		    qef_katt;
		    qef_katt = qef_katt->attr_next
		)
		{
		    /* set new value */
		    adc_dv1.db_datatype = qef_katt->attr_type;
		    adc_dv1.db_prec     = qef_katt->attr_prec;
		    adc_dv1.db_length   = qef_katt->attr_length;
		    adc_dv1.db_data	    = (char *) dsh_key + qef_katt->attr_value;
		    adc_dv1.db_collID   = qef_katt->attr_collID;

		    /* set current max */
		    adc_dv2.db_datatype = qef_katt->attr_type;
		    adc_dv2.db_prec     = qef_katt->attr_prec;
		    adc_dv2.db_length   = qef_katt->attr_length;
		    adc_dv2.db_data	    = (char *) dsh_key + 
					qen_nkey->nkey_att->attr_value;
		    adc_dv2.db_collID   = qef_katt->attr_collID;
		    status = adc_compare(adf_cb, &adc_dv1, 
			&adc_dv2, &result);
		    if (status != E_DB_OK)
		    {
			status = qef_adf_error(&adf_cb->adf_errcb,
			     status, dsh->dsh_qefcb, &dsh->dsh_error);
			if (status == E_DB_ERROR)
			    return (status);
			/* else, status has been changed to OK and we continue
			** processing.
			*/
		    }
		    if 
		    (
			/* a new max */
			(
			    result < 0 
			    && 
			    qef_kand->kand_type == QEK_MIN 
			)
			||
			(
			    result > 0
			    &&
			    qef_kand->kand_type == QEK_MAX
			)
		    )
		    qen_nkey->nkey_att = qef_katt;
		}
	    }
	}
    }
    /* put this OR list into the proper order off of qen_tkey. Proper
    ** order is defined as increasing order. This can be changed
    ** in the future (ie when we can perform merge sorts in decreasing
    ** order
    */
    /* for each OR element, find the max of the remaining elements. Put this
    ** element at the front of the queue. This will have the effect of
    ** sorting the keys in increasing order.
    ** This is currently only performed for single attribute keys. more
    ** precisely, only the first key is considered.
    */

    qen_1dup		= 0;
    qen_2dup		= 0;
    qen_tkey->qen_nkey	= 0;
    for (;;)
    {
	/* assume the first element in list is the max */
        qen_max		= qen_tkey->qen_keys;
	max_prev	= &qen_tkey->qen_keys;
	/* stop if there are no more OR elements */
	if (qen_max == 0)
	    break;
	/* compare the current max to all remaining elements on the list.
	** find the read max
	*/
	for
	(
	    qen_prev	= &qen_max->qen_next,
	    qen_nor	= qen_max->qen_next;
	    qen_nor;
	)
	{
            QEN_NKEY	    *curkey;	/* current key of multi-attribute
					** key being compared */
            QEN_NKEY        *maxkey;    /* equivalent max key of multi-attribute
                                        ** key being compared */
	    curkey = qen_nor->qen_key;
	    maxkey = qen_max->qen_key;
	    result = -1;		
	    do
	    {
		/* compare the two OR lists - need to compare all elements
                ** if we are to eliminate equal keys */

		if (curkey->nkey_att->attr_attno != maxkey->nkey_att->attr_attno)
		    break;

		adc_dv1.db_datatype = curkey->nkey_att->attr_type;
		adc_dv1.db_prec     = curkey->nkey_att->attr_prec;
		adc_dv1.db_length   = curkey->nkey_att->attr_length;
		adc_dv1.db_data	= (char *) dsh_key + 
					    curkey->nkey_att->attr_value;
		adc_dv1.db_collID   = curkey->nkey_att->attr_collID;

		adc_dv2.db_datatype = maxkey->nkey_att->attr_type;
		adc_dv2.db_prec     = maxkey->nkey_att->attr_prec;
		adc_dv2.db_length   = maxkey->nkey_att->attr_length;
		adc_dv2.db_data	= (char *) dsh_key + 
					maxkey->nkey_att->attr_value;
		adc_dv2.db_collID   = maxkey->nkey_att->attr_collID;
		status = adc_compare(adf_cb, &adc_dv1, 
		    &adc_dv2, &result);
		if (status != E_DB_OK)
		{
		    status = qef_adf_error(&adf_cb->adf_errcb, status, 
			dsh->dsh_qefcb, &dsh->dsh_error);
		    if (status == E_DB_ERROR)
			return (status);
		    /* else, status has been changed to OK and we continue
		    ** processing.
		    */
		}
		curkey = curkey->nkey_next; /* look at multi-attribute keys
					    ** until the attributes are not
					    ** equal */
		maxkey = maxkey->nkey_next;
	    } while ( curkey && maxkey && (result == 0));

	    /* if result is positive, qen_nor is the new max */
	    if (result > 0 || desc && result < 0)
	    {
		qen_max	    = qen_nor;
		max_prev    = qen_prev;
	    }
	    else if (result == 0)
	    {
		if (!maxkey)
		{
		    /* duplicate values are not used in keying */
		    /* remove element from list */
		    /* if the max key has less qualifications then the current
		    ** key is more restrictive and can safely be removed
		    ** FIXME - check if this assertion is TRUE, i.e. do
                    ** we only look at exact keys here??
		    */
		    *qen_prev = qen_nor->qen_next;

		    /* add element to dup list */
		    if (!qen_1dup)
		    {
			qen_1dup = qen_nor;
			qen_2dup = qen_nor;
		    }
		    else
		    {
			qen_2dup->qen_next = qen_nor;
			qen_2dup = qen_nor;
		    }
		    /* set the current and previous pointers 
		    ** the previous pointer stays the same because we removed
		    ** an element from the list.
		    */
		    qen_nor = qen_nor->qen_next;
		    continue;
		}
		else if (!curkey)
		{   /* current key is less restrictive than max key and
                    ** max key is a subset of the current key, so choose
                    ** a new max , FIXME - should probably remove the
                    ** max key since it is a subset */
		    qen_max	= qen_nor;
		    max_prev    = qen_prev;
		}
	    }
	    /* set the current and previous pointers */
	    qen_prev	= &qen_nor->qen_next;
	    qen_nor	= qen_nor->qen_next;

	}
	/* shuffle the list - this removes the MAX from the list */
	*max_prev	    = qen_max->qen_next;

	/* put the current max in the list in sorted order */	
	qen_max->qen_next   = qen_tkey->qen_nkey;
	qen_tkey->qen_nkey  = qen_max;
    }	
    /* reset the list */
    /* next key pointer is first key in list */
    qen_tkey->qen_keys	    = qen_tkey->qen_nkey;
    /* attach the duplicate KOR nodes, they will be reused later 
    ** note that the next key pointer will not point to the beginning of the
    ** list.
    */
    if (qen_1dup)
    {
	qen_2dup->qen_next = qen_tkey->qen_keys;
	qen_tkey->qen_keys = qen_1dup;
    }
    return (E_DB_OK);
}

/*{
** Name: QEQ_ADE_BASE		- initialize a set of BASE pointers
**
** Description:
**  A given address array is initialised by reading the given qen_base array 
**  and generating addresses into the address array.
**
** Inputs:
**      dsh                             current data segment header
**      qen_base			array of bases
**	excb_base			pointers to bases
**	num_bases			Number of bases to process	
**	param				user parameter array
**
** Outputs:
**	excb_base			array updated with pointers to rows
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**	Exceptions:
**	    none
**
** Side Effects:
**	    None.
**
** History:
**      03-dec-1992 (kwatts)
**          Broken out of qeq_ade code. qeq_ade now calls this to perform the
**	    lowest level of QEN_BASE conversion. 
**          virgin segment.
**      20-may-94 (eric)
**          Split this routine into two parts: qee_ade_base (called by qee_ade)
**          and qeq_ade_base (continued to be called by qeq_ade). All static
**          CX base initializations go into qee_ade_base() and all query
**          startup specific initializations go into qeq_ade_base(). This is
**          to improve performance.
**	26-aug-04 (inkdo01)
**	    Support global base array in parameter address resolution.
[@history_line@]...
[@history_template@]...
*/
DB_STATUS
qeq_ade_base(
QEE_DSH            *dsh,
QEN_BASE	   *qen_base,
PTR		   *excb_base,
i4		   num_bases,
PTR		   *param )
{
    DB_STATUS	    	status = E_DB_OK;
    QEN_BASE		*loc_qen_base = qen_base;
    PTR			*loc_excb_base = excb_base;
    i4			i;

    /* Convert each base to the correct address */

    for (i = 0; i < num_bases; i++, loc_qen_base++, loc_excb_base++)
    {
	switch (loc_qen_base->qen_array)
	{
	    case QEN_PARM:
		if (dsh->dsh_qp_ptr->qp_status & QEQP_GLOBAL_BASEARRAY)
		    dsh->dsh_row[loc_qen_base->u.qen_parm_row] =
			param[loc_qen_base->qen_index];
		else *loc_excb_base = param[loc_qen_base->qen_index];
		break;

	    case QEN_PLIST:
		*loc_excb_base = 
			(PTR) dsh->dsh_usr_params[loc_qen_base->qen_index];
		break;

	    case QEN_OUT:
	    case QEN_VTEMP:
	    case QEN_DMF_ALLOC:
		*loc_excb_base = 0;
		break;

	    case QEN_ROW:
	    case QEN_KEY:
	    case QEN_ADF_AG:
		break;
	}
    }
    return(status);
}

/*{
** Name: QEQ_ADE	- initialize the ADE_EXCB
**
** Description:
**      The QEN_ADF struct is used to initialize the control block used
**  for compiled expression evaluation. The ADE_EXCB is initialized by
**  reading the qen_base array and moving addresses into the base array.
**  In addition, the required amount of scratch space for ADE is allocated.
**
** Inputs:
**      dsh                             current data segment header
**      qen_adf                         ade_excb initializer
**	param				user parameter array
**
** Outputs:
**      qef_rcb
**	    .error.err_code                  errors in allocating space
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**	Exceptions:
**	    none
**
** Side Effects:
**	    The ade_excbs are initialized in the DSH
**
** History:
**      1-aug-86 (daved)
**          written 
**      10-dec-87 (puree)
**          Converted all ulm_palloc to qec_palloc
**	10-feb-88 (puree)
**	    Convert ulm_openstream to qec_mopen.  Convert qec_palloc to
**	    qec_malloc.
**	11-feb-88 (puree)
**	    Using ADE_SVIRGIN CX to initialize VLT dsh's.
**	02-sep-88 (puree)
**	    New architecture for VLUP/VLT scratch space handling.  Each
**	    ADE_CX needs its own space.
**	24-mar-89 (jrb)
**	    Cleaned up adf_ag_struct initialization; added db_prec.
**      5-dec-89 (eric)
**          make constant expressions go faster by putting them into the
**          virgin segment.
**	04_dec_1992 (kwatts)
**	    Call new routine qeq_ade_base to create the table of pointers
**	    from bases.
**      20-May-94 (eric)
**          We now only execute this routine if we need to do query startup
**          specific initializations, or if there is a Virgin segment to
**          execute. All static (non-query startup specific issues)
**          initializations are now in qee_ade().
**      11-Aug-2003 (huazh01)
**          Initialize the memory returned by qen_mopen() to zero before we
**          execute the VIRGIN instructions. 
**          This fixes bug 110196. INGSRV2253.
**	26-aug-04 (inkdo01)
**	    Add support for global base array.
**	11-march-2008 (dougi)
**	    Fix broken ade_excb->excb_bases[ADE_GLOBALBASE] in parallel plans.
*/
DB_STATUS
qeq_ade(
QEE_DSH            *dsh,
QEN_ADF            *qen_adf,
PTR		   *param )
{
    ADE_EXCB			*ade_excb;
    QEN_BASE			*qen_base;
    PTR				*excb_bases;
    PTR				*bases;
    QEE_VLT			*vlt;
    GLOBALREF QEF_S_CB		*Qef_s_cb;
    i4				length;
    DB_STATUS			status = E_DB_OK;
    ULM_RCB			ulm;

    if (qen_adf == NULL || 
	    (qen_adf->qen_mask & (QEN_SVIRGIN | QEN_HAS_PARAMS)) == 0
	)
    {
	/* Either no CX or nothing to do */
	return (E_DB_OK);
    }

    ade_excb = (ADE_EXCB*) dsh->dsh_cbs[qen_adf->qen_pos];
    excb_bases  = &ade_excb->excb_bases[ADE_ZBASE];
    qen_base = qen_adf->qen_base;
    if (dsh->dsh_qp_ptr->qp_status & QEQP_GLOBAL_BASEARRAY)
    {
	bases = dsh->dsh_row;
	ade_excb->excb_bases[ADE_GLOBALBASE] = (PTR)bases; /* reset in case broken
							** by parallel plan */
    }
    else bases = ade_excb->excb_bases;

    /* initialize the temps */
    if ((qen_adf->qen_mask & QEN_HAS_PARAMS) != 0)
    {
	qeq_ade_base(dsh, qen_base, excb_bases, qen_adf->qen_sz_base, param);
    }

    /* create the VLUP/VLT scratch space if reqiured */

    if (qen_adf->qen_pcx_idx > 0)
    {
	/* get the length of space for VLUP/VLT */
	status = ade_vlt_space(dsh->dsh_adf_cb, ade_excb->excb_cx,
	    ade_excb->excb_nbases, bases, &length);

	if (status)
	{
	    if ((status = qef_adf_error(&dsh->dsh_adf_cb->adf_errcb, 
		status, dsh->dsh_qefcb, &dsh->dsh_error)) != E_DB_OK)
		return (status);
	}

	if (length > 0)
	{
	    vlt  = &dsh->dsh_vlt[(qen_adf->qen_pcx_idx) - 1];

	    if (vlt->vlt_size < length)
	    {
		STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);

		/* if the space exists but is not big enough, release it */
		ulm.ulm_streamid_p = &vlt->vlt_streamid;

		if (vlt->vlt_streamid != (PTR)NULL)
		{
		    status = ulm_closestream(&ulm);
		    if (status != E_DB_OK)
		    {
			dsh->dsh_error.err_code = ulm.ulm_error.err_code;
			return (status);
		    }
		    /* ULM has nullified vlt_streamid */
		}

		/* Open the stream and allocate the new space in one action */
		ulm.ulm_flags = ULM_PRIVATE_STREAM | ULM_OPEN_AND_PALLOC;
		ulm.ulm_psize = ulm.ulm_blocksize = length;

		status = qec_mopen(&ulm);
		if (status != E_DB_OK)
		{
		    dsh->dsh_error.err_code = ulm.ulm_error.err_code;
		    return (status);
		}
		/* ULM has stored streamid in vlt_streamid */

		/* store information in the descriptor */
		vlt->vlt_ptr = ulm.ulm_pptr;
		vlt->vlt_size  = length;

		/* b110196, provide clean working area */ 
		MEfill(length, 0, vlt->vlt_ptr); 
	    }

	    /* set the base address */
	    ade_excb->excb_bases[ADE_VLTBASE] = vlt->vlt_ptr;
	}
	else
	{
	    ade_excb->excb_bases[ADE_VLTBASE] = (PTR)NULL;
	}
    }
    else
    {
	ade_excb->excb_bases[ADE_VLTBASE] = (PTR)NULL;
    }

    if ((qen_adf->qen_mask & QEN_SVIRGIN) != 0)
    {
        /* Now execute the SVIRGIN segment of the CX */
	ade_excb->excb_seg = ADE_SVIRGIN;
	status = ade_execute_cx(dsh->dsh_adf_cb, ade_excb);
	if (status != E_DB_OK)
	{
	    if ((status = qef_adf_error(&dsh->dsh_adf_cb->adf_errcb,
			  status, dsh->dsh_qefcb, &dsh->dsh_error)) != E_DB_OK)
		return (status);
	}
    }

    return (status);
}

/*{
** Name: QEQ_QPART_FIND - prepare bit map of qualified partitions 
**
** Description:
**	This routine processes the array of partition qualifying 
**	constant "rows", calling the "adt_whichnnn" functions to determine
**	the partitioned table logical partitions they correspond to.
**	A bit map is prepared with bits set corresponding to the 
**	qualified physical partitions.
**
**	In the multi-dimension case, the qualification constants identify
**	logical partitions, not physical partitions.  The qualifying
**	logical partitions are computed first, and that result is used
**	to calculate a bit map of physical partitions.
**
** Inputs:
**	dsh		- ptr to the DSH for this thread
**	orig		- ptr to current ORIG node
**
** Outputs:
**	dsh		- dsh_row contains ptr to updated bit map
**
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** History:
**	2-feb-04 (inkdo01)
**	    Written for table partitioning.
**	20-feb-04 (inkdo01)
**	    Changed bit setting loops because range value sequence 
**	    isn't necessarily the same as partition sequence.
**	13-may-04 (inkdo01)
**	    Add logic to handle "between" preds on a single partitioning 
**	    column.
**	18-May-2004 (schka24)
**	    Clearing phase of between started clearing one bit too soon.
**	22-Jul-2005 (schka24)
**	    Record physical partitions rather than logical partitions,
**	    so that orig can hand out partitions to threads better.
**	1-sep-2006 (dougi)
**	    Fix Karl's rewrite (which he neglected to note in history) so it
**	    onlydereferences pwork_map in the multi-dimension case.
*/

static DB_STATUS
qeq_qpart_find(
QEE_DSH		*dsh,
QEN_ORIG	*orig)

{
    DB_STATUS	    status = E_DB_OK;
    QEF_CB	    *qef_cb = dsh->dsh_qefcb;
    ADF_CB	    *adf_cb = dsh->dsh_adf_cb;
    QEN_PART_INFO   *partp = orig->orig_part;
    QEN_PART_QUAL   *pqualp;
    PTR		    result_map;		/* Overall result map (physical) */
    PTR		    lwork_map;		/* Logical partition map */
    PTR		    pwork_map;		/* Physical partition map for dim */
    PTR		    qpconstp;
    DB_PART_DIM	    *dimp;
    QEF_QP_CB	    *qp = dsh->dsh_qp_ptr;
    i4		    qpcrowsz;
    i4		    i, j;
    i4		    totparts = partp->part_totparts;
    i4		    map_bytes;
    u_i2	    partno, bpartno;
    bool	    notyet;
    bool	    between;
    bool	    onedim;

    /* Set up pointers to result, work bit maps.  Each dimension
    ** qualification computes its result into the work bit map,
    ** and ANDs that with what we have so far in the result bit map.
    ** If there is only one dimension possible, we can shortcut
    ** this a bit by pointing the work pointer at the result bit map.
    */

    map_bytes = (totparts + 7) / 8;
    result_map = dsh->dsh_row[partp->part_qpix];
    if (partp->part_qual.part_qdim->nparts == totparts)
    {
	lwork_map = result_map;		/* Just one dimension */
	onedim = TRUE;
    }
    else
    {
	lwork_map = dsh->dsh_row[partp->part_qpwlix];
	pwork_map = dsh->dsh_row[partp->part_qpwpix];
	MEfill(map_bytes, -1, result_map);
	onedim = FALSE;
    }

    /* Loop over each dimension having a qualification */
    pqualp = &partp->part_qual;
    do
    {
	dimp = pqualp->part_qdim;
	between = (pqualp->part_qflags & PART_QUAL_LE) &&
			(pqualp->part_qflags & PART_QUAL_GE);
	qpconstp = dsh->dsh_row[pqualp->part_qpcix];
	qpcrowsz = qp->qp_row_len[pqualp->part_qpcix]
					/ pqualp->part_prowcnt;
	MEfill(map_bytes, 0, lwork_map);
	if (!onedim)
	    MEfill(map_bytes, 0, pwork_map);

	/* Set up fields then iterate over the constant "rows" calling
	** appropriate adt_whichnnn function to get partition number(s).
	** Set corresponding bits in bit map. */

	for (i = 0; i < pqualp->part_prowcnt; i++, qpconstp += qpcrowsz)
	{
	    switch (dimp->distrule) {
	      case DBDS_RULE_HASH:
		status = adt_whichpart_hash(adf_cb, dimp, qpconstp, &partno);
		break;
	      case DBDS_RULE_LIST:
		status = adt_whichpart_list(adf_cb, dimp, qpconstp, &partno);
		break;
	      case DBDS_RULE_RANGE:
		status = adt_whichpart_range(adf_cb, dimp, qpconstp, &partno);
		break;
	    }

	    if (status != E_DB_OK)
	    {
		if ((status = qef_adf_error(&adf_cb->adf_errcb, 
		    status, qef_cb, &dsh->dsh_error)) != E_DB_OK)
		    return (status);
	    }

	    /* Now set the bit - if range qualification, set all bits
	    ** less than partition no. (if <) or greater than (if >). 
	    ** NOTE: logical partition numbers are 1-origin. The following
	    ** code effectively maps them back to 0-origin for use in
	    ** logical->physical partition mapping algorithms. */

	    if (pqualp->part_qflags & (PART_QUAL_LE | PART_QUAL_GE))
	    {
		if ((!between && (pqualp->part_qflags & PART_QUAL_LE)) ||
			i >= 1)
		{
		    /* This loop for <= (set bits up through partition N)
		    ** or the second half of between (clear bits from N+1 to end).
		    ** to end).
		    */
		    for (j = 0, notyet = TRUE; j < dimp->nbreaks && 
						    (notyet || between); j++)
		    {
			bpartno = dimp->part_breaks[j].partseq;
			if (!between)
			   BTset(bpartno-1, lwork_map);
			if (bpartno == partno)
			   /* Either stop here (<=) or this is last bit to leave
			   ** on (if between), start clearing next trip thru
			   */
			   notyet = FALSE;
			else if (!notyet && between)
			   BTclear(bpartno-1, lwork_map);
		    }
		}
		else
		{
		    /* This loop for >=, or first part of BETWEEN.
		    ** Set bits starting at N through the end.  (if between,
		    ** second trip thru will clear extraneous bits).
		    */
		    for (j = 0, notyet = TRUE; j < dimp->nbreaks; j++)
		    {
			bpartno = dimp->part_breaks[j].partseq;
			if (notyet && bpartno != partno)
			    continue;
			notyet = FALSE;		/* set all remaining bits */
			BTset(bpartno-1, lwork_map);
		    }
		}
	    }
	    else
		BTset((i4)partno-1, lwork_map);
	}

	/* All done if single dimension, lwork_map == result_map */
	if (onedim)
	    return(E_DB_OK);

	/* For the multi-dimension case, we now need to extract each logical
	** partition bit, and laboriously turn on the physical partition bits
	** that correspond to the logical partition.  (For a pretty picture,
	** see qenorig.c.)
	*/
	i = -1;
	while ( (i = BTnext(i, lwork_map, totparts)) != -1)
	{
	    /* Set bits for logical partition i */
	    partno = i * dimp->stride;
	    do
	    {
		/* Set bits for logical partition piece, then skip the gap */
		for (j = dimp->stride; j > 0; --j)
		{
		    BTset(partno, pwork_map);
		    ++partno;
		}
		partno = partno + (dimp->nparts - 1) * dimp->stride;
	    } while (partno < totparts);
	}
	/* AND this partial result against the whole */
	BTand(totparts, pwork_map, result_map);

	/* Move to next qual */
	pqualp = pqualp->part_qnext;
    } while (pqualp != NULL);

    return (E_DB_OK);
}

/*{
** Name: QEQ_AUDIT	- Audit table, view and alarm events.
**
** Description:
**      When any query is run the table and view access must
**      be audited.  Also if alarm events are associated 
**      with the query these must be audited.
**
** Inputs:
**      qef_rcb                 Pointer to qef control block.
**      aud                     Pointer to audit information.
**      status                  Status of qeu_audit call to determine
**                              success or failure auditing.
**	    
**
** Outputs:
**	qef_rcb
**	    .error.err_code	Any error returned from
**                              SCF or DMF unless status
**                              indicates failure, then original
**                              error remains.
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	    NB 'status' and error code will refer to the _last_ error
**	       if multiple errors are found.
**	Exceptions:
**	    none
**
** Side Effects:
**
** History:
**	07-jul-89 (jennifer)
**          written
**	19-nov-1992 (pholman)
**	    Modified to use qeu_secaudit rather than the obsolete qeu_audit
**	10-mar-1993 (markg)
**	    Modified call to qeu_secaudit so that audit records don't get 
**	    flushed every time a security alarm is triggered.
**	06-jul-93 (robf)
**	   Merge auditing access to secure catalogs here, and make generic
**	   so we don't need to list all the possible secure catalogs (which
**	   tended to get out of date)
**	24-nov-93 (robf)
**         Fire alarms by group and role as well as user/public
*/
static
DB_STATUS
qeq_audit(
QEF_RCB      *qef_rcb,
QEE_DSH	     *dsh,
QEF_AUD      *aud,
DB_STATUS   *status )
{
    QEF_CB	    *qef_cb = dsh->dsh_qefcb;
    DB_STATUS       local_status;
    QEF_ART         *art;
    QEF_APR         *apr;
    DB_ERROR	    e_error;
    i4              i;
    i4		    j;
    i4		    eventtype;
    i4	    msgid;

    for (; aud; aud = aud->qef_next)
    {
	/* 
        ** Go through the list of audit blocks and perform the audits.
        */

	/*  Process Audit's. */

	for (i=0, art=aud->qef_art; 
	     i< aud->qef_cart && Qef_s_cb->qef_state & QEF_S_C2SECURE;
	     i++)
	{
	    if (art[i].qef_tabtype & QEFART_VIEW)
	    {
		eventtype = SXF_E_VIEW;    
		msgid     = I_SX2027_VIEW_ACCESS;
	    }
	    else
	    {
		/* Default is table access */
		eventtype = SXF_E_TABLE;
		msgid     = I_SX2026_TABLE_ACCESS;
	    }
	    local_status = qeu_secaudit(FALSE, qef_cb->qef_ses_id, 
	      		    art[i].qef_tabname.db_tab_name,
			    &art[i].qef_ownname, sizeof(DB_TAB_NAME),
			    eventtype, msgid, 
			    (SXF_A_SUCCESS | art[i].qef_mode), &e_error);

	    if (local_status != E_DB_OK)
	    {
		if (*status == E_DB_OK)
		{
		    dsh->dsh_error = e_error;
		    *status = local_status;
		}
	    }
	    /*
	    ** If updating a secure catalog log a security  event
	    */
	    if((art[i].qef_tabtype&QEFART_SECURE)  && 
		art[i].qef_mode!=SXF_A_SELECT)
	    {
		     local_status = qeu_secaudit(TRUE, qef_cb->qef_ses_id, 
	      		   art[i].qef_tabname.db_tab_name,
			   &art[i].qef_ownname, sizeof(DB_TAB_NAME),
	      		   SXF_E_SECURITY,
	      		   I_SX201C_SEC_CAT_UPDATE, 
			   (SXF_A_SUCCESS | art[i].qef_mode), &e_error);
		    if (local_status != E_DB_OK)
		    {
			if (*status == E_DB_OK)
			{
			    dsh->dsh_error = e_error;
			    *status = local_status;
			}
		    }
	    }
	}

	/*  Process Alarm's. */
	for (j=0, apr = aud->qef_apr; j < aud->qef_capr; j++)
	{
	    /*
	    ** Find the ART for this AUD to get table info.
	    */
	    for (i = 0; i < aud->qef_cart; i++)
	    {
		if (apr[j].qef_tabid.db_tab_base ==
		    art[i].qef_tabid.db_tab_base
			&&
		    apr[j].qef_tabid.db_tab_index ==
		    art[i].qef_tabid.db_tab_index)
		{
		    /*
		    ** Got the ART info, so check if firing the alarm
		    */
		    if (!(apr[j].qef_user.db_own_name[0] == ' ' ||  ( 
			/* USER */
			apr[j].qef_gtype==QEFA_USER &&
			!MEcmp( ( PTR ) &apr[j].qef_user,
			       ( PTR ) &qef_cb->qef_user,
			       ( u_i2 ) sizeof(qef_cb->qef_user ) )  
		     ) || (
			/* GROUP */
			apr[j].qef_gtype==QEFA_GROUP &&
			!MEcmp( ( PTR ) &apr[j].qef_user,
			       ( PTR ) qef_cb->qef_group,
			       ( u_i2 ) sizeof(apr[i].qef_user ) )  
		     ) || (
			/* ROLE */
			apr[j].qef_gtype==QEFA_APLID &&
			!MEcmp( ( PTR ) &apr[j].qef_user,
			       ( PTR ) qef_cb->qef_aplid,
			       ( u_i2 ) sizeof(apr[i].qef_user ) )  
		     )))
		    {
			/*
			** Not firing the alarm so go to the next
			*/
			break;
		    }
		    /*
		    ** Fire the alarm
		    */
		    /*
		    ** Write audit record for alarm.
		    */
		    if ( Qef_s_cb->qef_state & QEF_S_C2SECURE )
		    {
			local_status = qeu_secaudit(FALSE, qef_cb->qef_ses_id, 
				art[i].qef_tabname.db_tab_name,
				&art[i].qef_ownname, sizeof(DB_TAB_NAME),
				SXF_E_ALARM, I_SX2028_ALARM_EVENT, 
				(SXF_A_SUCCESS | art[i].qef_mode), &e_error); 

			if (local_status != E_DB_OK)
			{
			    if (*status == E_DB_OK)
			    {
				dsh->dsh_error = e_error;
				*status = local_status;
			    }
			}
		    }
		    /*
		    ** Raise dbevents if necessary
		    */
		    if(apr[j].qef_flags & QEFA_F_EVENT)
		    {
		        char evtxt[DB_EVDATA_MAX+1];
			/*
			** If no event text add some
			*/
			if(!apr[j].qef_ev_l_text)
			{
			   STprintf(evtxt,ERx("Security alarm raised on table %.*s.%.*s on success when %s by %.*s"),
				qec_trimwhite(sizeof(DB_OWN_NAME),
			        	(char*)&art[i].qef_ownname), 
			        (char*)&art[i].qef_ownname, 
	      		    	qec_trimwhite(sizeof(art[i].qef_tabname.db_tab_name),
	      		    		(char*)&art[i].qef_tabname.db_tab_name),
	      		    	&art[i].qef_tabname.db_tab_name,
				art[i].qef_mode==SXF_A_UPDATE?"update":
				art[i].qef_mode==SXF_A_INSERT?"insert":
				art[i].qef_mode==SXF_A_DELETE?"delete":
				art[i].qef_mode==SXF_A_SELECT?"select":
							      "access",
				qec_trimwhite(sizeof(DB_OWN_NAME),
			        	(char*)&qef_cb->qef_user), 
					(char*)&qef_cb->qef_user
				);
			   
			    qef_rcb->qef_ev_l_text=STlength(evtxt);
			    qef_rcb->qef_evtext=evtxt;
			}
			else
			{
			    qef_rcb->qef_ev_l_text=apr[j].qef_ev_l_text;
			    qef_rcb->qef_evtext=apr[j].qef_evtext;
			}
			qef_rcb->qef_evname= &apr[j].qef_evname;
			qef_rcb->qef_evowner= &apr[j].qef_evowner;
			local_status=qeu_evraise(qef_cb,qef_rcb);
			if(local_status!=E_DB_OK)
			{
				*status=E_DB_SEVERE;
			        dsh->dsh_error.err_code = E_QE028D_ALARM_DBEVENT_ERR;
				break;
			}
		    }
		    break;
		}
	    }
	}
    } /* end for all aud structures. */
    return (*status);
}
