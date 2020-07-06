/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <pc.h>
#include    <st.h>
#include    <cv.h>
#include    <ex.h>
#include    <sr.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ddb.h>
#include    <cs.h>
#include    <lk.h>
#include    <me.h>
#include    <scf.h>
#include    <ulf.h>
#include    <gca.h>
#include    <cut.h>
#include    <ulm.h>
#include    <ulh.h>
#include    <adf.h>
#include    <ade.h>
#include    <qsf.h>
#include    <dmf.h>
#include    <dmhcb.h>
#include    <dmmcb.h>
#include    <dmtcb.h>
#include    <dmccb.h>
#include    <dmrcb.h>
#include    <dmscb.h>
#include    <dmucb.h>
#include    <qefmain.h>
#include    <qefkon.h>
#include    <qefrcb.h>
#include    <qefcb.h>
#include    <qefnode.h>
#include    <qefqeu.h>
#include    <psfparse.h>	
#include    <qefact.h>
#include    <qefqp.h>
#include    <qefdsh.h>
#include    <qefexch.h>
#include    <qefscb.h>
#include    <qeuqcb.h>
#include    <rqf.h>
#include    <tm.h>
#include    <dudbms.h>
#include    <tpf.h>
#include    <qefqry.h>
#include    <qefcat.h>
#include    <sxf.h>
#include    <qefprotos.h>

/**
**
**  Name: QEE.C - environment management
**
**  Description:
**      The routines in this file provide management of the DSH
**  structures. 
**
**	    qee_shutdown- remove all dynamic DSHs for all sessions to shut
**			  down the server.
**          qee_cleanup - remove dynamic DSHs for a session
**	    qee_destroy - subfunction of qee_cleanup.
**	    qee_fetch	- find or create a DSH
**	    qee_create	- build a DSH. called by qee_fetch.
**	    qee_cract	- creating action specific control blocks and buffers.
**	    qee_ade	- create the ADE_EXCB
**	    qee_dbparam	- substitute DBP user parameters
**	    qee_update_byref_params	- substitute back DBP user parameters
**	    qee_build_byref_tdata	-  Build a tdata for sending back byref
**					  parameters.
**	    qee_build_byref_tdesc	- Build a tdesc for sending back byref
**					  parameters.
**          qee_hfile   - set up hold file
**	    qee_buildHoldFile - build hold file structures
**	    qee_joinTempTables - build temporary tables for join nodes
**	    qee_buildTempTable - build structures for a temporary table
**	    qee_allocateDMTCB - allocate a DMF table control block
**	    qee_allocateDMRCB - allocate a DMF record control block
**	    qee_hashInit - initialize a QEN_HASH_BASE structure (for hash joins)
**	    qee_hashaggInit - initialize a QEN_HASH_AGGREGATE structure 
**	    qee_resolve_excbs - Resolve EXCB addresses into XADDRS structure
**
**  History:
**      11-jun-86 (daved)    
**          written
**	7-oct-87 (puree)
**	    in qee_ade, initialize adf_excb->excb_seg to ADE_SINIT to be
**	    used by qea_fetch in fetching from a constant function.
**	22-oct-87 (puree)
**	    modified qee_fetch for query constant support.
**	30-nov-87 (puree)
**	    converted SCF_SEMAPHORE to CS_SEMAPHORE.
**	10-dec-87 (puree)
**	    converted all ulm_palloc to qec_palloc.
**	15-dec-87 (puree)
**	    Changed DSH object naming.
**	06-jan-88 (puree)
**	    modified qee_cleanup to handle QEQP_OBSOLETE flag.
**	14-jan-88 (puree)
**	    modified qee_cleanup to destroy ULH object before QSO object.
**	19-jan-88 (puree)
**	    modified qee_fetch for new ULH protocol.
**	20-feb-88 (puree)
**	    convert ulm_openstream to qec_mopen.  rename qec_palloc to
**	    qec_malloc.
**	25-feb-88 (puree)
**	    Removed reference to dsh_chain.  It has never been used
**	    anywhere else.
**	22-feb-88 (puree)
**	    modified qee_cract not to set table lock modes.  
**	11-mar-88 (puree)
**	    Remove  DSH_QUEUE structure. DMT_CB's can be linked together 
**	    using the standard jupiter control block header.
**	10-may-88 (puree)
**	    remove a portion off from qee_cleanup.  create qee_destroy.
**	27-jul-88 (puree)
**	    modify qee_fetch to return E_QE0023_INVALID_QUERY if the
**	    root returned by QSF is a null.
**	01-aug-88 (puree)
**	    modify qee_cract and qee_create for list of end-of-block actions.
**	04-aug-88 (puree)
**	    fix bug in qee_fetch to also release lock on QSF object with 
**	    null root.
**	08-aug-88 (puree)
**	    modify qee_hfile for computation of the size of the hold file
**	    tuples.
**	09-aug-88 (puree)
**	    modify qee_hfile again.  Hold file tuples need to be as big as
**	    the row buffer.
**	12-sep-88 (puree)
**	    Temporary modify qee_create to add maximum row size 
**	    (DB_MAXTUP) to the row length specified in the QP.
**	04-oct-88 (puree)
**	    Removed tempory fix in qee_create.  QP is fixed in OPC.
**	07-oct-88 (puree)
**	    Fix key structure allocation in qee_cract.  The structures must
**	    be kept in the same order as they are in the QP.
**	11-oct-88 (puree)
**	    Always close VLT memory stream in qee_destroy.  Otherwise, when
**	    ULH destroys an object in its LRU queue, the VLT memory will be
**	    left dangling around.
**	31-aug-88 (carl)
**	    modified qee_destroy and qee_create for Titan
**	12-sep-88 (puree)
**	    Temporary modify qee_create to add maximum row size 
**	    (DB_MAXTUP) to the row length specified in the QP.
**	04-oct-88 (puree)
**	    Removed tempory fix in qee_create.  QP is fixed in OPC.
**	07-oct-88 (puree)
**	    Fix key structure allocation in qee_cract.  The structures must
**	    be kept in the same order as they are in the QP.
**	11-oct-88 (puree)
**	    Always close VLT memory stream in qee_destroy.  Otherwise, when
**	    ULH destroys an object in its LRU queue, the VLT memory will be
**	    left dangling around.
**	15-jan-89 (paul)
**	    Add rules enhancements to DSH creation and cleanup.
**	30-jan-89 (carl)
**	    modified to use new DB_DISTRIB definition
**	19-feb-89 (paul)
**	    Extend qee_dbparam to handle CALLPROC parameter lists.
**	    Initialize QEA_EMESSAGE actions.
**	18-apr-89 (paul)
**	    Modify dsh setup and cleanup to handle cursor update and delete as
**	    regular query plans.
**	31-may-89 (neil)
**	    Issue error on invalid DBP param name or usage.
**	31-may-89 (neil)
**	    Issue error on invalid DBP param name or usage.
**	03-jun-89 (carl)
**	    fixed qee_cleanup to set (reinitialize) qef_cb->qef_odsh to the
**	    current dsh being released (NULL)
**	06-jul-89 (neil)
**	    Ignore case on DBP param names.
**	07-sep-89 (neil)
**	    Alerters: Added initialization of new EVENT actions.
**	09-dec-89 (fred)
**	    Added large datatype support.
**	16-feb-90 (mikem)
**	    BYTE_ALIGN fix for qee_dbparam().  On BYTE_ALIGN machines parameters
**	    must be aligned before calling adc_cvinto().  This fix, fixes the
**	    problem using brute force (declaring a max sized buffer on the
**	    stack, and doing the MEcopy when necessary).  We may want to change
**	    this solution to something more elegant (ie. have opc allocate
**	    aligned memory for these parameters at query "compile" time, so
**	    that they are aligned by the time they get to this routine).
**	12-sep-90 (carl)
**	    replaced qee_d4_tds with equivalent minimal in-line code
**	10-dec-90 (neil)
**	    Alerters: Added initialization of new EVENT actions in qee_cract.
**	07-jan-91 (davebf)
**	    Init new fields in qen_hold.  Allocate qen_shd's for in-memory
**	    holds.
**	07-jan-91 (davebf)
**	    Init qen_shd's associated with in-memory hold files.
**	14-mar-91 (fpang)
**	    Have qee_cleanup() close any open cursors. This needs to be
**	    done to release the locks that the cursors may have on the
**	    temp tables. 
**	    Fixes B34452, - Access conflict error message (E_PS0D21)
**	    from local when 'commit' is issued w/o user issued 'close's
**	    where cursors are multi-site and require temp tables.
**	29-jul-91 (markg)
**	    Changed DBP missing parameter error handling in qee_dbparam() to
**	    include parameter name and database procedure name in the error
**	    message. Bug 38765.
**	14-jan-92 (nancy)
**	    Bug 39225, add support under IF and RETURN for initializing
**	    local dbp variables in qee_cract().
**	25-mar-92 (rickh)
**	    Allocate the tid hold file for right joins.  This involves
**	    splitting some code out of qee_hfile into a new routine:
**	    qee_buildHoldFile.  Added initialization of oj_ijFlagsFile.
**	10-feb-1992 (bryanp)
**	    Changed dmt_location from a DB_LOC_NAME to a DM_DATA.
**	15-jun-92 (davebf)
**	    Sybil Merge
**	04-sep-92 (fpang)
**	    In qee_create(), added explicit break after distributed processing
**	    code, also initialized local variable, ddq_p.
**	28-oct-92 (rickh)
**	    Replaced references to ADT_CMP_LIST with DB_CMP_LIST.
**	03-nov-92 (ralph)
**	    Don't ignore case with STbcompare() when checking database
**	    procedure parameters.
**	09-nov-92 (jhahn)
**	    Added handling for byrefs.
**	14-dec-92 (jhahn)
**	    Cleaned up handling of byrefs.
**      14-dec-92 (anitap)
**          Included <psfparse.h> for CREATE SCHEMA.
**	21-dec-92 (anitap)
**	    Prototyped qee_cleanup(), qee_destroy(), qee_fetch(), qee_create(),
**          qee_cract(), qee_ade(), qee_dbparam(), qee_hfile(), 
**	    qee_buildHoldFile(), qee_joinTempTables(), qee_buildTempTable(), 
**	    qee_allocateDMTCB(), qee_allocateDMRCB().
**	28-dec-92 (anitap)
**	    Initialize dsh_exeimm field in DSH for Execute Immediate Processing.
**	    Added support in qee_fetch().
**      12-jan-93 (mikem)
**          lint cleanup in qee_create().  Removed initialization of ddq_p when
**          it was declared.  It was already initialized in the body of the
**          procedure.
**	04-mar-93 (anitap)
**	    Removed duplicate definitions of qee functions.
**	18-mar-93 (jhahn)
**	    Added support for set input procedures.
**	02-apr-93 (jhahn)
**	    Added support for statement
**	    level unique indexes.
**	17-may-93 (jhahn)
**	    Added more support for statement
**	    level unique indexes.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	9-jul-93 (rickh)
**	    Made MEcopy call conform to prototype.
**      20-jul-1993 (pearl)
**          Bug 45912.  Allocate memory for (new) dsh_kor_nor array in
**          qee_create().  Initialize new field qef_kor in QEN_NOR structure
**          in qee_cract(), and save pointer to QEN_NOR structure in the
**          dsh_kor_nor array.  In qee_fetch(), initialize dsh_kor_nor
**          to null at create time.
**	26-jul-1993 (jhahn)
**	    Moved coercing of returned byref procedure parameters from the
**	    back end to the front end.
**	    Added qee_nested_byrefs(), qee_build_byref_tdata() and
**	    qee_build_byref_tdesc().
**	27-jul-1993 (jhahn)
**	    Replaced QEF_SAVED_RSRC with QEF_DBPROC_QP so as to be consistent
**	    with anitap's integration.
**	23-aug-1993 (bryanp)
**	    Initialize the dmt_owner field in the DMT_CB. This allows the
**	    TEMP_TABLE_MACRO in qeq_cleanup to work properly. (B53993)
**      14-sep-93 (smc)
**          Made session_id a CS_SID.
**	01-sep-93 (jhahn)
**	    Added support for multiple query plans for set input procedures.
**	5-oct-93 (eric)
**	    Added support for resource lists.
**	13-jan-94 (rickh)
**	    Added a cast to a mecopy call.
**	 7-jan-94 (swm)
**	    Bug #58635
**	    Added PTR cast for qsf_owner which has changed type to PTR.
**	31-jan-94 (jhahn)
**	    Fixed support for resource lists.
**	08-feb-94 (rickh)
**	    TID temp table for right ISJOINs and CPJOINs now has two sort
**	    attributes.
**      20-nov-1994 (andyw)
**          Added NO_OPTIM due to Solaris 2.4/Compiler 3.00 errors
**      19-dec-94 (sarjo01) from 16-sep-94 (johna)
**          Initialize dmt->dmt_mustlock. Bug 58681
**      1-june-1995 (hanch04)
**          Removed NO_OPTIM su4_us5, installed sun patch
**      20-aug-95   (ramra01)
**          Simple aggregate optimization to modify at runtime the call
**          to a special node ORIGAGG in the qee_cract function.
**	7-nov-1995 (inkdo01)
**	    Numerous changes to replace many QEN_ADF and some QEN_OJINFO and
**	    QEN_TEMP_TABLE structure instances by pointers, in a variety of
**	    QEN_NODE and QEF_AHD structures.
**      06-feb-96 (johnst)
**          Bug #72202.
**          Changed qee_ade_base() arguments to match the interface of
**          qeq_ade_base() from which it was originally cloned off,
**          restoring the two {qen_base, num_bases} arguments instead of passing
**          the higher level struct QEN_ADF, as (eric) defined it.
**          The Smart Disk code in qeq_validate() needs to call both
**          qee_ade_base() and qeq_ade_base() passing in the actual address
**          arrays and number of bases used by the Smart Disk case.
**      14-Feb-95   (ramra01)
**	    In the case of the WHERE predicate with an OR condition
**	    this will require DMF to reposition itself for the
**	    next key. This is not possible thru the aggf optimization
**	    code path. This will require to go thru the ORIG node. b74040
**	05-May-96 (ramra01)
**          In the case where an aggregate computation involves the tid
**          ex: max(tid), min(tid) do not use the aggf optim path as
**          the value is not set in the row buffer offset until return
**          from DMF to the QEF orig node. This is indicated by a non-neg
**          number in the orig_tid element specifying the offset to place
**          the value if the tid value is needed.
**	13-jun-96 (nick)
**	    LINT directed changes.
**	 1-oct-96 (nick)
**	    Ensure dsh_row entries are nulled out when creating a new DSH.
**	07-Aug-1997 (jenjo02)
**	    Changed *dsh to **dsh in qee_destroy() and qee_cleanup() prototypes
**	    and set the pointer to null if the DSH is destroyed.
**	11-Aug-1997 (jenjo02)
**	    Changed ulm_streamid from (PTR) to (PTR*) so that ulm
**	    can destroy those handles when the memory is freed.
**	19-Aug-1997 (jenjo02)
**	    Distinguish between private and shared ULM streams.
**	    Piggyback ulm_palloc() calls with ulm_openstream() where appropriate.
**	21-aug-1997 (kch,bobmart)
**	    Ensure dsh_cpuoverhead and dsh_diooverhead members are initialized 
**	    when creating a new DSH.
**      10-Aug-1998 (nanpr01)
**          qee_destroy() Reset vlt_ptr, vlt_size after freeing the memory.
**          Removed redundant ifdef VLT_FIXED code.
**	17-mar-1999 (somsa01)
**	    Added include of pc.h and lk.h to resolve references to locking
**	    ids.
**	21-may-1999 (hanch04)
**	    Replace STbcompare with STcasecmp,STncasecmp,STcmp,STncmp
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	10-Jan-2001 (jenjo02)
**	    Supply session's SID to QSF in qsf_sid.
**	24-jan-01 (inkdo01)
**	    Change certain fields in QEF_QEP structure to reflect 
**	    introduction of union  - for hash aggregation project.
**	20-mar-2001 (somsa01)
**	    In qee_dbparam(), to save stack space, allocate space for
**	    align_buf dynamically.
**	01-oct-2001 (devjo01)
**	    Rename QEN_HASH_DUP to QEF_HASH_DUP.
**	21-Dec-01 (gordy)
**	    GCA_COL_ATT no longer defined with DB_DATA_VALUE.
**      08-may-2002 (stial01)
**          Fixed initialization of ascii_id in dmt_cb
**	8-jan-04 (toumi01)
**	    Added support for exchange.
**	9-feb-04 (toumi01)
**	    Allocate and init EXCH_CB persistent context for exchange
**	    parent node execution.
**	27-feb-04 (inkdo01)
**	    Changed dsh ptr to arrays of structs (SHD, HOLD, STATUS, HASH,
**	    HASHAGG) to ptrs to arrays of ptrs to the structs.
**	19-may-04 (toumi01)
**	    Zero out the whole exch_cb when it is allocated so that all
**	    addresses, bools, trace flags, etc. are initialized.
**	30-Sep-2004 (bonro01)
**	    Align buffers to size specified by ALIGN_RESTRICT for 
**	    the platform. Fix for bug in change 470380.
**	01-Apr-2005 (bonro01)
**	    Disable optimization for Solaris AMD64 to resolve 
**	    iidbms abend
**	6-Dec-2005 (kschendel)
**	    Remove improper/obsolete u_i2 casts while I'm in here.
**	13-Dec-2005 (kschendel)
**	    Remaining inline QEN_ADF structs moved to pointers, fix here.
**	28-Dec-2005 (kschendel)
**	    Use DSH references whenever possible instead of QEF RCB
**	    references.  (This simplifies the code, eliminating many
**	    RCB references, and reflects the new primacy of the DSH
**	    thanks to the parallel query changes.)
**	16-Jan-2006 (kschendel)
**	    Move qen-status pointers to new XADDR CB, along with copies
**	    of node ADE_EXCB addresses.  Add routine to resolve same.
[@history_template@]...
**/
/*
NO_OPTIM = a64_sol
*/

static DB_STATUS qee_create(
	QEF_RCB		*qef_rcb,
	QEE_DSH		*dsh,
	ULM_RCB		*ulm);


static DB_STATUS 
qee_buildHoldFile(
	QEF_RCB             *qef_rcb,
        QEE_DSH             *dsh,
        QEN_NODE            *node,
        ULM_RCB             *ulm,
        i4                  att_count,
        i4             row_no,
        i4                  hfile_no);

static DB_STATUS 
qee_joinTempTables(
	QEF_RCB             *qef_rcb,
        QEE_DSH             *dsh,
        QEN_NODE            *node,
        ULM_RCB             *ulm);
 
static DB_STATUS 
qee_buildTempTable(
        QEF_RCB             *qef_rcb,
        QEE_DSH             *dsh,
        ULM_RCB             *ulm,
        QEN_TEMP_TABLE      *QPtempTable);

static DB_STATUS 
qee_allocateDMTCB(
        QEF_RCB             *qef_rcb,
        QEE_DSH             *dsh,
        ULM_RCB             *ulm,
        i4             row_no,
        DMT_CB              **parentsDMTCBptr,
	DMT_ATTR_ENTRY	    **attr_list,
	i4		    attr_count);

static DB_STATUS 
qee_allocateDMRCB(
        QEF_RCB             *qef_rcb,
        QEE_DSH             *dsh,
        ULM_RCB             *ulm,
        i4             row_no,
        DMR_CB              **parentsDMRCBptr);

static DB_STATUS
qee_update_nested_byrefs(
	QEF_RCB		*qef_rcb,
	QEE_DSH		*dsh);

static DB_STATUS
qee_build_byref_tdesc(
	QEF_RCB		*qef_rcb,
	QEE_DSH		*dsh);

static DB_STATUS
qee_build_byref_tdata(
	QEF_RCB		*qef_rcb,
	QEE_DSH		*dsh);

static DB_STATUS
qee_hashInit(
        QEF_RCB             *qef_rcb,
        QEE_DSH             *dsh,
        QEN_NODE	    *node,
	ULM_RCB		    *ulm);

static DB_STATUS
qee_hashaggInit(
        QEF_RCB             *qef_rcb,
        QEE_DSH             *dsh,
        QEF_QEP		    *qepp,
	ULM_RCB		    *ulm);

/* Table of primes used to select number of hash-join partitions.
** Primes are used to help reduce or eliminate interactions with
** hash table-partitioning, and should give good distributions.
*/

READONLY i4 primes[] = {
	7, 11, 13, 17, 19, 23, 29, 31,
	37, 41, 43, 47, 53, 59, 61, 67,
	71, 73, 79, 83, 89, 97, 101, 103,
	107, 109, 113, 127, 131, 137, 139, 149,
	151, 157, 163, 167, 173, 179, 181, 191,
	193, 197, 199, 211, 223, 227, 229, 233,
	239, 241, 251, 257, 263, 269, 271, 277,
	0};


/*{
** Name: QEE_SHUTDOWN	- remove all DSHs from facility
**
** Description:
**      All DSH's not currently active are released. 
**  Currently active DSHs are associated with a session and must
**  be released using QEE_CLEANUP.
**
** Inputs:
**      none
**
** Outputs:
**      none
**	Returns:
**	    E_DB_OK
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	1-jul-86 (daved)
**          written
**	11-mar-87 (daved)
**	    use ULH
**	30-nov-87 (puree)
**	    convert SCF_SEMAPHORE to CS_SEMAPHORE.
[@history_line@]...
*/
DB_STATUS
qee_shutdown()
{
    GLOBALREF QEF_S_CB	*Qef_s_cb;
    ULH_RCB		ulh_rcb;


    ulh_rcb.ulh_hashid = Qef_s_cb->qef_ulhid;
    (VOID)ulh_clean(&ulh_rcb);
    return (E_DB_OK);
}


/*{
** Name: qee_cleanup	- remove dynamic DSH for a session
**
** Description:
**      Go through the cache of DSH and remove those that
** belong to the specified session. If the session id is
** Null, remove all DSH structures. The removal of a DSH
** entails releasing any QSF locks on the object and then
** deallocating the memory stream. 
**
**
** Manipulation of a session's queues doesn't need semaphore locks
** because actions on a session are sequential. However,
** manipulation of the facility's queues does require a semaphore.
**
** Inputs:
**	qef_rcb		    request block
**	dsh		    dsh to remove. zero means remove all.
**
** Outputs:
**	none
**	Returns:
**	    E_DB_OK
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	11-jun-86 (daved)
**          written
**	25-jul-86 (daved)
**	    remove DSHs dependent on this DSH
**	14-feb-87 (daved)
**	    unlock the QP when it is not in use.
**	11-mar-87 (daved)
**	    incorporate use of ULH
**	30-nov-87 (puree)
**	    converted SCF_SEMAPHORE to CS_SEMAPHORE.
**	09-jan-88 (puree)
**	    If a repeat query DSH is marked with QEQP_OBSOLETE, destroy
**	    both the QSF object and the ULH object. 
**	03-jun-89 (carl)
**	    fixed to set (reinitialize) qef_cb->qef_odsh to the current 
**	    dsh being released (NULL)
**	14-mar-91 (fpang)
**	    Have qee_cleanup() close any open cursors. This needs to be
**	    done to release the locks that the cursors may have on the
**	    temp tables. 
**	    Fixes B34452, - Access conflict error message (E_PS0D21)
**	    from local when 'commit' is issued w/o user issued 'close's
**	    where cursors are multi-site and require temp tables.
**	21-dec-92 (anitap)
**	    Prototyped.
**	07-Aug-1997 (jenjo02)
**	    Changed *dsh to **dsh in qee_destroy() and qee_cleanup() prototypes
**	    and set the pointer to null if the DSH is destroyed.
**	22-sep-03 (inkdo01)
**	    Removed TRdisplay code for tp qe128.
[@history_line@]...
*/
DB_STATUS 
qee_cleanup(
	QEF_RCB		*qef_rcb,
	QEE_DSH		**pdsh)
{
    QEF_CB			*qef_cb;
    QEE_DSH			*dsh;
    QEE_DSH			*next;
    DB_STATUS			status = E_DB_OK;


    qef_cb = qef_rcb->qef_cb;

    /* return now if there are no open DSHs */
    if (qef_cb->qef_odsh == (PTR)NULL)
	return (E_DB_OK);

    if (pdsh == (QEE_DSH**)NULL || *pdsh == (QEE_DSH *)NULL)
    {
	if(qef_cb->qef_c1_distrib & DB_3_DDB_SESS)
	{
	    /* Check if we got here with cursors still opened that have
	    ** temporary tables associated with them. This can happen
	    ** if COMMIT is called w/o a close. If cursors are open with
	    ** temp tables, go through and close them because qee_destroy()
	    ** will try to delete the temp tables that may be locked by the
	    ** cursor(s). 
	    ** Fixes B34452 (fpang, 14-mar-1991) */

	    for (dsh = (QEE_DSH *)qef_cb->qef_odsh; 
		 dsh && (qef_cb->qef_open_count > 0); 
		 dsh = dsh->dsh_next)
	    {
		qef_cb->qef_open_count--;
		status = qeq_c1_close(qef_rcb, dsh);
		if (status)
		    return (status);
	    }
	}
	/* release all DSHs for the session */
	for (dsh = (QEE_DSH *)qef_cb->qef_odsh; dsh; dsh = next)
	{
	    /* grab the next dsh now, because the pointer will be destroyed 
	    ** in qee_destroy.
	    */
	    next = dsh->dsh_next;
	    status = qee_destroy(qef_cb, qef_rcb, &dsh);	    
	    if (status != E_DB_OK)
		break;
	}
    }
    else
    {	/* release only the specified DSH */
	/* and destroy the caller's DSH pointer */
	status = qee_destroy(qef_cb, qef_rcb, pdsh);
    }
    return (status);
}



/*{
** Name: qee_destroy	- destroy/release a DSH.
**
** Description:
**
** Inputs:
**	qef_cb		    QEF session control block
**	qef_rcb		    QEF request block
**	dsh		    DSH to destroy/release
**
** Outputs:
**	none
**	Returns:
**	    E_DB_OK
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	10-may-88 (puree)
**	    Adopted from qee_cleanup.
**	31-aug-88 (carl)
**	    modified for Titan: traverse the termination list to destroy all
**	    temporary tables persisting until now
**	11-oct-88 (puree)
**	    Always close VLT memory stream.  Otherwise, when ULH destroys 
**	    an object in its LRU queue, the VLT memory will be left 
**	    dangling around.
**	27-jan-89 (carl)
**	    test for DDB trace point 50 to skip destroying temporary tables
**	    to allow OPF/OPC to debug multi-site query plan; such survived
**	    temporary tables must be manually cleaned up
**	18-apr-89 (paul)
**	    Modify dsh setup and cleanup to handle cursor update and delete as
**	    regular query plans.
**	06-feb-90 (fpang)
**	    Before destroying temporary tables, restore qef_cb->qef_dsh 
**	    because it may have been set to null by previous calls to
**	    qee_destroy(). The destroy temp table routines sometimes try
**	    to access qef_cb->qef_dsh, and will AV if it is null. 
**	    A better fix would probably be to set qef_cb->qef_dsh to null
**	    only if it is the last dsh, by checking if dsh_next is null.
**	    This fixes B35591 (SEGV if 1. open cursor 1 multi-site,
**				       2. open cursor 2 single site,
**				       3. commit)..
**	21-dec-92 (anitap)
**	    Prototyped.
**	31-jan-94 (jhahn)
**	    Fixed support for resource lists.
**	27-april-98(angusm)
**	    bug 85267: unget buffer now allocated once via MEreqmem
**	    instead of via ULM so needs to be cleaned up here.
**	30-jul-97 (nanpr01)
**	    After memory deallocation set the streamid to NULL.
**	07-Aug-1997 (jenjo02)
**	    Changed *dsh to **dsh in qee_destroy() and qee_cleanup() prototypes
**	    and set the pointer to null if the DSH is destroyed.
**	    Backed out Probal's 30-jul change which was setting dsh_handle to
**	    NULL in the just-deallocated DSH.
**	27-april-98(angusm)
**	    bug 85267: unget buffer now allocated once via MEreqmem
**	    instead of via ULM so needs to be cleaned up here.
**      10-Aug-1998 (nanpr01)
**          qee_destroy() Reset vlt_ptr, vlt_size after freeing the memory.
**          Removed redundant ifdef VLT_FIXED code.
**	4-may-99 (inkdo01)
**	    Changes for hash joins.
**	23-mar-01 (inkdo01)
**	    Changes for hash aggregation.
**	22-sep-03 (inkdo01)
**	    Removed TRdisplay code for tp qe128.
**	8-apr-04 (inkdo01)
**	    Watch out for child DSHs of || queries when detaching from 
**	    session chain (they aren't there to begin with).
**	23-apr-04 (toumi01)
**	    Skip qsf cleanup for || query child DSH processing.
**	15-Jul-2004 (jenjo02)
**	    qee_destroy is only called on "root" DSHs. Parallel
**	    Kids' DSHs are cleaned up by the kids and never 
**	    get here.
**	19-Jul-2004 (schka24)
**	    Close execution-time memory stream if repeated query.
**	27-May-2005 (schka24)
**	    Don't adjust hash memory out of sort usage.
**	15-Dec-2005 (kschendel)
**	    Factor out duplicated VLT space release.
**	25-Feb-2006 (kschendel)
**	    Close QP as part of destroy, just in case.
**	28-jan-2008 (dougi)
**	    Reset scrollable cursor workarea in repeat queries.
*/
DB_STATUS 
qee_destroy(
	QEF_CB		*qef_cb,
	QEF_RCB		*qef_rcb,
	QEE_DSH		**pdsh)
{
    GLOBALREF QEF_S_CB		*Qef_s_cb;
    QEE_DSH			*dsh = *pdsh;
    QEE_DSH			*next;
    QEF_QP_CB			*qp;
    DB_STATUS			status = E_DB_OK, sav_status = E_DB_OK;
    DB_ERROR			sav_error;
    i4			err;
    i4				i;
    i4                          shd_cnt; 
    QSF_RCB			qsf_rcb;
    ULM_RCB			ulm;
    ULH_RCB			ulh_rcb;
    QEE_DDB_CB			*qee_p = dsh->dsh_ddb_cb;
    bool			ddb_b;

    if (qef_cb->qef_c1_distrib & DB_3_DDB_SESS)
	ddb_b = TRUE;
    else
    {
	/* not distributed session */
	ddb_b = FALSE;

	/* Remove the DSH from QEF session control block */
	if (qef_cb->qef_dsh == (PTR)dsh)
	{
	    qef_cb->qef_dsh = (PTR)NULL;
	}
    }


    /* Remove the DSH from session list of opened DSHs */
    if (qef_cb->qef_odsh == (PTR)dsh)
    {	/* The DSH is at the beginning of the queue */
	next = dsh->dsh_next;
	qef_cb->qef_odsh = (PTR)next;
	if (next)
	    next->dsh_prev = (QEE_DSH *)NULL;
    }
    else 
    {	/* Not at beginning of the queue */
	next = dsh->dsh_next;
	dsh->dsh_prev->dsh_next = next;
	if (next)
	    next->dsh_prev = dsh->dsh_prev;
    }
    /* If dsh_aqp_dsh is not set (no associcated DSH), delete the DSH 
    ** dependent on this one.  If it is set, this is a remote update DSH.
    ** No query plan is dependent on a remote update DSH.
    ** If this assumption changes, the QEE_DSH struct should change.
    ** [kschendel] Translation:  the "associated" DSH is the cursor select
    ** DSH, i.e. the parent.  It may have where-current-of delete and
    ** update queries that depend on it.  If we are closing the cursor
    ** select, make sure all dependent where-current-of queries are closed
    ** too.  If we are closing a dependent query just take it out of the
    ** cursor select's dependent list.
    */
    if (dsh->dsh_aqp_dsh == (QEE_DSH *)NULL)
    {
	/* Remove the DSHs dependent on this DSH */
	for (next = dsh->dsh_dep_dsh; next; next = dsh->dsh_dep_dsh)
	{
	    status = qee_destroy(qef_cb, qef_rcb, &next);
	    if (status != E_DB_OK)
		return (status);
	}
    }
    else
    {	/* A remote update or remote delete DSH.  Remove this DSH from the
	** dsh_dep_dsh list of the associated QP.  Find the DSH pointing to the
	** current DSH.
	*/
	for (next = dsh->dsh_aqp_dsh; next->dsh_dep_dsh != dsh; 
	    next->dsh_dep_dsh)
	{
	    /* do nothing */;
	}

	/* adjust the pointers in the linked list */

	next->dsh_dep_dsh = next->dsh_dep_dsh->dsh_dep_dsh;
    }

#ifdef xDEBUG
    (VOID) qe2_chk_qp(dsh);
#endif 

    /* Lock the QP if it has not been locked. */
    qsf_rcb.qsf_next = (QSF_RCB *)NULL;
    qsf_rcb.qsf_prev = (QSF_RCB *)NULL;
    qsf_rcb.qsf_length = sizeof(QSF_RCB);
    qsf_rcb.qsf_type = QSFRB_CB;
    qsf_rcb.qsf_owner = (PTR)DB_QEF_ID;
    qsf_rcb.qsf_ascii_id = QSFRB_ASCII_ID;
    qsf_rcb.qsf_sid = qef_rcb->qef_cb->qef_ses_id;
    qsf_rcb.qsf_obj_id.qso_handle = dsh->dsh_qp_handle;
    if ((dsh->dsh_qp_status & DSH_QP_LOCKED) == 0)
    {
	qsf_rcb.qsf_lk_state = QSO_SHLOCK;
	status = qsf_call(QSO_LOCK, &qsf_rcb);
	if (status)
	{
	    qef_rcb->error.err_code = qsf_rcb.qsf_error.err_code;
	    return (status);
	}
    }
    dsh->dsh_qp_status |= DSH_QP_LOCKED;
    qp = dsh->dsh_qp_ptr;   /* the QP ptr */

    if (ddb_b)
    {
	i4	    val_1, val_2;
	bool	    trace_50;
	QEQ_DDQ_CB  *ddq_p = & qp->qp_ddq_cb;
	QES_DDB_SES *dds_p = & qef_cb->qef_c2_ddb_ses;
	i4	    sav_rowcount = qef_rcb->qef_rowcount;
					    /* must save row count before
					    ** commiting the CDB association
					    ** and/or dropping all temporary 
					    ** tables */
	bool	    restored_dsh = FALSE;


	if (ult_check_macro(& qef_cb->qef_trace, QEF_TRACE_DDB_NO_DROP_50, 
	    & val_1, & val_2))
	    trace_50 = TRUE;
	else	    
	    trace_50 = FALSE;

	status = E_DB_OK;		    /* preset */
	sav_status = E_DB_OK;		    /* must preset */

	/* Fix for B35591.
	** If qef_cb->qef_dsh is null, set it to this dsh,
	** Lower level calls may blow up if it is not set.
	*/
	if (qef_cb->qef_dsh == (PTR)NULL)
	{
		qef_cb->qef_dsh = (PTR)dsh;
		restored_dsh = TRUE;
	}

	if (qee_p->qee_d3_status & QEE_05Q_CDB)
	{
	    qee_p->qee_d3_status &= ~QEE_05Q_CDB;
					    /* reset */
	    status = qed_u11_ddl_commit(qef_rcb);
	    if (status)
	    {
		/* save error status */

		sav_status = status;
		STRUCT_ASSIGN_MACRO(qef_rcb->error, sav_error);
	    }
	}

	if ((! qef_cb->qef_abort) 
	    && 
	    (ddq_p->qeq_d3_elt_cnt > 0)
	    &&
	    ! trace_50)
	{
	    status = qeq_d6_tmp_act(qef_rcb, dsh);
	    if (sav_status == E_DB_OK && status)
	    {
		/* save error status */

		sav_status = status;
		STRUCT_ASSIGN_MACRO(qef_rcb->error, sav_error);
	    }
	}

	/* Fix for B35591
	** Reset qef_cb->qef_dsh to null, if we restored it above.
	*/
	if (restored_dsh == TRUE)
	{
		qef_cb->qef_dsh = (PTR)NULL;
		restored_dsh = FALSE;
	}

	qef_rcb->qef_rowcount = sav_rowcount;	
					/* restore output row count */
	qef_cb->qef_dsh = (PTR)NULL;	/* deferred action for STAR */
    }

    /* Close the QP just to make sure it's closed.  It should be,
    ** but a redundant close ought to be fast and harmless.
    */
    qeq_close_dsh_nodes(dsh);

    /*
    ** clean up dynamically allocated unget 
    ** buffer for FSM join holds (bug 85267)
    */
    if (dsh->dsh_hold)
    {
	for (i = 0; i < qp->qp_hld_cnt; i++)
	{
	    if (dsh->dsh_hold[i]->unget_buffer != (PTR)NULL)
	    {
		MEfree(dsh->dsh_hold[i]->unget_buffer);
	        dsh->dsh_hold[i]->unget_buffer = (PTR)NULL;	
	    }
	}
    }
    /* Whether the QP is a repeat query or not, always release the sort 
    ** buffers.  This is to allow better recycle of buffers. */

    if (dsh->dsh_shd != (QEN_SHD **)NULL)
    {
	for (i = 0, shd_cnt = qp->qp_sort_cnt + qp->qp_hld_cnt; 
             i < shd_cnt; i++)
	{
	    if (dsh->dsh_shd[i]->shd_streamid != (PTR)NULL)
	    {
		if((dsh->dsh_shd[i]->shd_options & SHD1_DSH_POOL) == 0)
		{
		    /* stream comes from sort pool */
		    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_s_ulmcb, ulm);
		    ulm.ulm_streamid_p = &dsh->dsh_shd[i]->shd_streamid;
		    qef_cb->qef_s_used -= dsh->dsh_shd[i]->shd_size;
		}
		else
		{
		    /* stream comes from dsh pool */
		    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
		    ulm.ulm_streamid_p = &dsh->dsh_shd[i]->shd_streamid;
		}

		if ((status = ulm_closestream(&ulm)) != E_DB_OK)
		{
		    qef_rcb->error.err_code = ulm.ulm_error.err_code;
		    return (status);
		}
		/* ULM has nullified shd_streamid */
	    }
	}
    }
    /* Note: The qen_shd structures will be cleaned up in qee_fetch.
    **       Also, do not release the qen_shd structure.  It's a part
    **       of and uses the same memory stream as the DSH. 
    */

    /* Whether the QP is a repeat query or not, always release the hash 
    ** buffers.  This is to allow better recycle of buffers. */

    if (dsh->dsh_hash != (QEN_HASH_BASE **)NULL)
    {
	for (i = 0; i < qp->qp_hash_cnt; i++)
	{
	    if (dsh->dsh_hash[i]->hsh_streamid != (PTR)NULL)
	    {
		STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_s_ulmcb, ulm);
		ulm.ulm_streamid_p = &dsh->dsh_hash[i]->hsh_streamid;
		qef_cb->qef_h_used -= dsh->dsh_hash[i]->hsh_size;

		if ((status = ulm_closestream(&ulm)) != E_DB_OK)
		{
		    qef_rcb->error.err_code = ulm.ulm_error.err_code;
		    return (status);
		}
		/* ULM has nullified hsh_streamid */
		dsh->dsh_hash[i]->hsh_size = 0;
	    }
	    dsh->dsh_hash[i]->hsh_currlink = dsh->dsh_hash[i]->hsh_toplink;
						/* reset currlink */
	}
    }
    /* Note: Do not release the qen_hash structure.  It's a part
    **       of and uses the same memory stream as the DSH. Structure
    **       is init'ed in qee_fetch for new and re-used DSH's.
    */

    /* Same deal for hash aggregate structures - just free the stream. */
    if (dsh->dsh_hashagg != (QEN_HASH_AGGREGATE **)NULL)
    {
	for (i = 0; i < qp->qp_hashagg_cnt; i++)
	{
	    if (dsh->dsh_hashagg[i]->hshag_streamid != (PTR)NULL)
	    {
		STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_s_ulmcb, ulm);
		ulm.ulm_streamid_p = &dsh->dsh_hashagg[i]->hshag_streamid;
		qef_cb->qef_h_used -= dsh->dsh_hashagg[i]->hshag_halloc;

		if ((status = ulm_closestream(&ulm)) != E_DB_OK)
		{
		    qef_rcb->error.err_code = ulm.ulm_error.err_code;
		    return (status);
		}
		/* ULM has nullified hshag_streamid */
	    }
	    dsh->dsh_hashagg[i]->hshag_currlink = 
					dsh->dsh_hashagg[i]->hshag_toplink;
						/* reset currlink */
	}
    }

    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);

    /* 
    ** Always close VLT memory stream.  This is necessary even for REPEATED
    ** because when we are out memory and ULH is asked to destroy an object
    ** at the head of its LRU queue, the DSH will be destroyed but the 
    ** VLT streams will be left dangling around.
    */

    if (dsh->dsh_vlt != (QEE_VLT *)NULL)
    {
	for (i = 0; i < qp->qp_pcx_cnt; i++)
	{
	    if (dsh->dsh_vlt[i].vlt_streamid != (PTR)NULL)
	    {
		ulm.ulm_streamid_p = &dsh->dsh_vlt[i].vlt_streamid;
		if ((status = ulm_closestream(&ulm)) != E_DB_OK)
		{
		    qef_rcb->error.err_code = ulm.ulm_error.err_code;
		    return (status);
		}
		/* ULM has nullified vlt_streamid */
	    }
	    dsh->dsh_vlt[i].vlt_ptr = (PTR)NULL;
	    dsh->dsh_vlt[i].vlt_size = 0;
	}
    }

    /* For a repeat query, unlock the QP and release the ULH object for
    ** the DSH unless the QP is obsolete.  In which case, both objects
    ** will be destroyed */ 

    if (qp->qp_status & QEQP_RPT)
    {
	ulh_rcb.ulh_hashid = Qef_s_cb->qef_ulhid;
	ulh_rcb.ulh_object = (ULH_OBJECT *)dsh->dsh_handle;

	/* Close the execution time memory stream if we opened one.
	** Don't have to do this unless repeated query, as non-repeated
	** query uses DSH stream for execution time memory.
	** Paranoia check that we aren't releasing the DSH stream here.
	** (The execution time stream == the DSH stream for exch kids
	** as well as non-repeat queries.)
	*/
	if (dsh->dsh_qstreamid != NULL
	  && dsh->dsh_qstreamid != dsh->dsh_streamid)
	{
	    ulm.ulm_streamid_p = &dsh->dsh_qstreamid;
	    if ((status = ulm_closestream(&ulm)) != E_DB_OK)
	    {
		qef_rcb->error.err_code = ulm.ulm_error.err_code;
		return (status);
	    }
	    /* ulm nullifies qstreamid */
	}

	/* If DSH_QP_OBSOLETE, destroy both the QSF and ULH objects */
	if (dsh->dsh_qp_status & DSH_QP_OBSOLETE)
	{
	    status = qsf_call(QSO_DESTROY, &qsf_rcb);
	    if (status)
	    {
		qef_rcb->error.err_code = qsf_rcb.qsf_error.err_code;
		return (status);
	    }
	    status = ulh_destroy(&ulh_rcb, (unsigned char *)NULL, 0);
	    if (status != E_DB_OK)
	    {
		qef_error(ulh_rcb.ulh_error.err_code, 0L, status, &err,
		    &qef_cb->qef_rcb->error, 0);
		qef_rcb->error.err_code = E_QE0101_DESTROY_DSH_OBJECT;
		return (status);
	    }
	}

	/* Not obsolete, just unlock the QP, and release the ULH object */
	else
	{
	    if (qp->qp_status & QEQP_SCROLL_CURS)
	    {
		/* Reset stuff in scrollable cursor workarea. */
		QEE_RSET_SPOOL	*sptr;

		sptr = (QEE_RSET_SPOOL *)dsh->dsh_cbs[qp->qp_rssplix];
		sptr->sp_rowcount = 0;
		sptr->sp_current = 0;
		sptr->sp_curpage = -1;
		sptr->sp_flags = SP_BEGIN;
	    }

	    /* Mark that no tables are open */
	    qeq_release_dsh_resources(dsh);
	    
	    /* Unlock the QP */
	    status = qsf_call(QSO_UNLOCK, &qsf_rcb);
	    if (status)
	    {
		qef_rcb->error.err_code = qsf_rcb.qsf_error.err_code;
		return (status);
	    }
	    dsh->dsh_qp_status = 0;	       /* mark QP not locked */
	    status = ulh_release(&ulh_rcb);
	    if (status != E_DB_OK)
	    {
		qef_error(ulh_rcb.ulh_error.err_code, 0L, status, &err,
		    &qef_cb->qef_rcb->error, 0);
		qef_rcb->error.err_code = E_QE0104_RELEASE_DSH_OBJECT;
		return (status);
	    }
	}
    }
    else
    {	/* Not a repeat query, always destroy the QP and the DSH.  Since
	** DSH is in QEF memory, we will just close its memory stream.
	*/
	status = qsf_call(QSO_DESTROY, &qsf_rcb);
	if (status)
	{
	    qef_rcb->error.err_code = qsf_rcb.qsf_error.err_code;
	    return (status);
	}
	ulm.ulm_streamid_p = &dsh->dsh_handle;
	status = ulm_closestream(&ulm);			
	if (status)
	{
	    qef_rcb->error.err_code = ulm.ulm_error.err_code;
	    return (status);
	}
    }
    /* Destroy the caller's DSH pointer */
    *pdsh = (QEE_DSH*)NULL;
    if (status)
	return(status);
    else if (sav_status)
    {
	STRUCT_ASSIGN_MACRO(sav_error, qef_rcb->error);
	return(sav_status);
    }
    else 
	return(E_DB_OK);
}


/*{
** Name: QEE_FETCH	- Find a DSH structure or create a new one
**
** Description:
**      For a repeat query, the DSH structure is stored in ULH cache so
**	it can be reused.  The hash key the the DSH is the full QP id
**	(db_cursor_id + dub_cur_name).  To minimize the overhead of 
**	computing the hash key, the DSH's are also keyed on the
**	db_cursor_id portion of the key.  All the 6.0 application programs
**	will have this field unique.  This is not true for 5.0 programs
**	
**	When a DSH for a repeat query is requested, qee_fetch first ask
**	ULH for existing DSH, using the short key.  If one or more is found,
**	the full key is checked for the exact match.  If matched, the existing
**	DSH is used.  Otherwise, a new DSH is created and cataloged in 
**	the ULH cache.
**
**	A DSH for a non-repeat query is never saved.  It is built and
**	destroyed for each execution.
**
**	This routine also assumes that if a DSH is an cursor update DSH,
**	the DSH for the retrieve cursor command is open and thus residing
**	in a queue off of the session control block.
**
**
** Inputs:
**      qef_rcb                 request block
**	    .qef_cb		session control block
**	    .qef_qp		query plan id
**	    .qef_qso_handle	query plan handle if not zero.
**	page_count		number of pages in set input table parameter.
**				-1 if this is not for a set input procedure.
**
** Outputs:
**      dsh                             data segment header
**	qef_rcb
**	    .error.err_code		one of the following
**				E_QE0000_OK
**				E_QE0002_INTERNAL_ERROR
**				E_QE000D_NO_MEMORY
**				E_QE000E_ACTIVE_COUNT_EXCEEDED
**				E_QE000F_OUT_OF_OTHER_RESOURCES
**				E_QE0014_NO_QUERY_PLAN
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0019_NON_INTERNAL_FAIL
**				E_QE0023_INVALID_QUERY
**
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	1-jul-86 (daved)
**          written
**	11-mar-87 (daved)
**	    use ULH
**	2-oct-87 (puree)
**	    if the qp_qconst_mask in the QEF_QP_CB is non-zero, allocate
**	    ADK_CONST_BLK, set the mask in it, and store its pointer into
**	    the newly created DSH.
**	25-oct-87 (thurston)
**          Made sure that the ADK_CONST_BLK in the DSH gets zeroed at init
**	    time.
**	30-nov-87 (puree)
**	    Converted SCF_SEMAPHORE to CS_SEMAPHORE.
**	10-dec-87 (puree)
**	    Converted all ulm_palloc to qec_palloc.
**	15-dec-87 (puree)
**	    Changed DSH object naming.
**	19-jan-88 (puree)
**	    Modified logic to catalog a repeat query DSH into ULH class due
**	    to simplified ULH protocol.
**	25-feb-88 (puree)
**	    Removed dsh_chain.  Never used.
**	27-jul-88 (puree)
**	    return E_QE0023_INVALID_QUERY if there is no root to the QP
**	    from QSF.  This is possible if there was a race in OPC when the 
**	    query plan was built.
**	04-aug-88 (puree)
**	    also release lock on QSF object with null root because
**	    QSO_GETHANDLE has already put the lock on the object.
**	08-jan-89 (carl)
**	    modified to initialize the DSH for single-site repeat query for 
**	    Titan
**	15-jan-89 (paul)
**	    Initialize new DSH fields dsh_stack, dsh_usr_params, dsh_depth_act
**	    to support rules processing.
**	18-apr-89 (paul)
**	    Modify dsh setup and cleanup to handle cursor update and delete as
**	    regular query plans.
**	6-jan-90 (eric)
**	    moved the allocation of the ADE_CONST_BLK info to before the call
**	    to qee_create. This is because qee_create() executes the VIRGIN
**	    segment of some CXs and the constant block strucuture is needed
**	    then. Also, adf_cb->adf_constants is set to point to it.
**	07-jan-91 (davebf)
**	    Init new fields in qen_hold.  Allocate qen_shd's for in-memory
**	    holds.
**	30-oct-92 (barbara)
**	    For Star delete cursor, use cursor dsh (same as update cursor).
**	21-dec-92 (anitap)
**	    Prototyped.
**	28-dec-92 (anitap)
**	    Initialize new dsh field dsh_exeimm to support Execute Immediate 
**	    processing in CREATE SCHEMA, FIPS constraints and CREATE VIEW (WCO).
**      20-jul-1993 (pearl)
**          Bug 45912.  Allocate memory for (new) dsh_kor_nor array in
**          qee_create().  Initialize new field qef_kor in QEN_NOR structure
**          in qee_cract(), and save pointer to QEN_NOR structure in the
**          dsh_kor_nor array.  In qee_fetch(), initialize dsh_kor_nor
**          to null at create time.
**	26-aug-93 (jhahn)
**	    Added support for set input procedures.
**	5-oct-93 (eric)
**	    Added support for resource lists.
**      20-may-94 (eric)
**          Fix for bug 60540: we call qsf to get the query plan in qeq_dsh()
**	    now because we need to query plan earlier to look up the DSH
**	    correctly. This means that we pass in the QP, and the qsf_rcb
**	    that we used to look it up with.
**	4-may-99 (inkdo01)
**	    Changes for hash joins.
**	jan-00 (inkdo01)
**	    More changes for hash joins.
**	3-mar-00 (inkdo01)
**	    Init dsh_srowcount for "select first n ...".
**	23-mar-01 (inkdo01)
**	    Changes for hash aggregation.
**	9-may-01 (inkdo01)
**	    Init hsh_rbptr, hshag_rbptr for repeat queries (fixes 104680)
**	30-may-01 (inkdo01)
**	    Init dmh_db_id, dmh_tran_id for repeat queries (fixes more 
**	    manifestations of 104680)
**	22-july-03 (inkdo01)
**	    It appears the tran ID can change (with autocommit or with an
**	    explicit commit right before query exec) after this call and
**	    before query exec (bug 110603), so dmh_tran_id will be 0'ed 
**	    here and init'ed in qenhjoin and qeahaggf.
**	19-jan-04 (inkdo01)
**	    Init dsh_threadno (for parallel queries).
**	23-feb-04 (inkdo01)
**	    Init. added fields for || queries (dsh_child/cnext, ...) and
**	    allocate space (as necessary) for QEE_DDB_CB and QEF_RCB that
**	    are now ptrs in DSH, not instances.
**	25-feb-04 (inkdo01)
**	    Load QEF_CB ptr into DSH.
**	26-mar-04 (inkdo01)
**	    Init new dsh_ahdvalid field to hold ptr to top AHD valid list.
**	15-Jul-2004 (jenjo02)
**	    Deprecated dsh_child, dsh_cnext.
**	16-Jul-2004 (schka24)
**	    Move dsh-row clearing to here so that it happens even for
**	    repeated query execution: some rows contain things like
**	    partition qualification bitmaps that need to start out zero.
**	    Initialize query-time memory stream id.
**	24-Mar-2005 (schka24)
**	    Simplify query-constant business, all we need is a flag.
**	3-Jun-2005 (schka24)
**	    Remove unused hash subpartition stuff.
**	14-Nov-2005 (schka24)
**	    Remove hash prevpr, not used any more.
**	14-Dec-2005 (kschendel)
**	    Initialize the DSH's ADF CB pointer.
**	13-Feb-2007 (kschendel)
**	    Set up DSH sid for cancel checking.
**	13-dec-2007 (dougi)
**	    Remove dsh_srowcount - moved to action header specific block.
**	25-Jan-2008 (kibro01) b116600
**	    Because hash joins and parallel queries can amend the dsh_row
**	    value, store the original and reuse that when giving back a qep.
*/
DB_STATUS
qee_fetch(
	QEF_RCB		*qef_rcb,
	QEF_QP_CB	*qp,
	QEE_DSH		**dsh,
	i4		page_count,
	QSF_RCB		*qsf_rcb)
{
    GLOBALREF QEF_S_CB *Qef_s_cb;
    QEE_DSH	    *ndsh   = (QEE_DSH *)NULL;
    QEE_DSH	    *next;
    DMH_CB	    *dmhcb;
    DB_STATUS	    status = E_DB_OK;
    i4	    err;
    i4	    	    have_dsh = FALSE;
    i4	    	    new_dsh  = FALSE;
    i4		    i, j;
    ULM_RCB	    ulm;
    ULH_RCB	    ulh_rcb;
    bool	    ddb_b;
    bool	    is_set_input = page_count >=0;

#ifdef xDEBUG
    QEF_AHD	    *act;
#endif 

    if (qef_rcb->qef_cb->qef_c1_distrib & DB_3_DDB_SESS)
	ddb_b = TRUE;
    else
	ddb_b = FALSE;

#ifdef xDEBUG
    if (qp->qp_length != sizeof(QEF_QP_CB) || qp->qp_type != QEQP_CB)
    {
	(VOID) qe2_chk_qp(0);
    }
    /* 1.  Check that we good count of action */
    i = qp->qp_ahd_cnt;
    if (i == 0)
    {
	status = E_DB_ERROR;
	qef_error(E_QE1001_NO_ACTIONS_IN_QP, 0L, status, &err,
	    &qef_rcb->error, 0);
	goto err_exit;
    }
    /* 2.  Check that we have enough actions */
    act = qp->qp_ahd;
    while (act && i)
    {
	act = act->ahd_next;
	--i;
    }
    if (act)
    {
	status = E_DB_ERROR;
	qef_error(E_QE1002_BAD_ACTION_COUNT, 0L, status, &err,
	    &qef_rcb->error, 0);
	goto err_exit;
    }
#endif 

    ulh_rcb.ulh_hashid = Qef_s_cb->qef_ulhid;
    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);

    if (qp->qp_status & QEQP_RPT)
    {	
	/* For repeat query, find a suitable available DSH in ULH cache. */

	struct
	{
		PTR	qso_handle;
		CS_SID	session_id;
	} ulh_name;

	ulh_name.qso_handle = qsf_rcb->qsf_obj_id.qso_handle;
	ulh_name.session_id = qef_rcb->qef_cb->qef_ses_id;

	status = ulh_getmember(&ulh_rcb, (unsigned char *) &qp->qp_id, 
		    (i4)sizeof(DB_CURSOR_ID),  (unsigned char *) &ulh_name, 
		    (i4)sizeof(ulh_name), ULH_DESTROYABLE, 0);
	if (DB_FAILURE_MACRO(status))
	{
	    qef_error(ulh_rcb.ulh_error.err_code, 0L, status, &err,
		&qef_rcb->error, 0);
	    qef_rcb->error.err_code = E_QE0102_CATALOG_DSH_OBJECT;
	    goto err_exit;
	}

	ndsh = (QEE_DSH *)(ulh_rcb.ulh_object->ulh_uptr);
	ulm.ulm_streamid = ulh_rcb.ulh_object->ulh_streamid;

	if (ndsh != (QEE_DSH *)NULL)
	{
	    have_dsh = TRUE;
	    ndsh->dsh_qp_status = DSH_QP_LOCKED;
	}
    }
    else
    {
	/* For a non-repeat query, create the DSH structure in QEF memory */
	/* Open QEF memory stream and allocate the DSH */
	ulm.ulm_flags = ULM_PRIVATE_STREAM | ULM_OPEN_AND_PALLOC;
	ulm.ulm_psize = sizeof (QEE_DSH);

	status = qec_mopen(&ulm);
	if (status)
	{
	    qef_rcb->error.err_code = ulm.ulm_error.err_code;
	    goto err_exit;
	}

	ndsh = (QEE_DSH *) ulm.ulm_pptr;
	have_dsh = TRUE;
	new_dsh = TRUE;
    }

    /* Allocate and initialize the DSH if not found in ULH cache */

    if (ndsh == (QEE_DSH *)NULL || new_dsh)
    {
	if (ndsh == (QEE_DSH*)NULL)
	{
	    ulm.ulm_psize = sizeof (QEE_DSH);
	    status = qec_malloc(&ulm);
	    if (status)
	    {
		qef_rcb->error.err_code = ulm.ulm_error.err_code;
		goto err_exit;
	    }

	    ndsh = (QEE_DSH *) ulm.ulm_pptr;

	}

	/* Allocate QEE_DDB_CB if necessary. */
	if (ddb_b)
	{
	    ulm.ulm_psize = sizeof(QEE_DDB_CB);
	    status = qec_malloc(&ulm);
	    if (status)
	    {
		qef_rcb->error.err_code = ulm.ulm_error.err_code;
		goto err_exit;
	    }
	    ndsh->dsh_ddb_cb = (QEE_DDB_CB *)ulm.ulm_pptr;
	}
	else ndsh->dsh_ddb_cb = (QEE_DDB_CB *) NULL;
	
	/* Allocate QEF_RCB savearea if necessary */
	ndsh->dsh_saved_rcb = ndsh->dsh_exeimm_rcb = (QEF_RCB *) NULL;
	if (qp->qp_status & (QEQP_CALLPR_RCB | QEQP_EXEIMM_RCB))
	{
	    ulm.ulm_psize = sizeof(QEF_RCB);
	    status = qec_malloc(&ulm);
	    if (status)
	    {
		qef_rcb->error.err_code = ulm.ulm_error.err_code;
		goto err_exit;
	    }
	    if (qp->qp_status & QEQP_CALLPR_RCB)
		ndsh->dsh_saved_rcb = (QEF_RCB *) ulm.ulm_pptr;
	    else ndsh->dsh_exeimm_rcb = (QEF_RCB *) ulm.ulm_pptr;
				/* save RCB addr in right spot */
	}
	
	if (qp->qp_status & QEQP_RPT)
	{	
	    ulh_rcb.ulh_object->ulh_uptr = (PTR)ndsh;
	    ndsh->dsh_handle = (PTR)ulh_rcb.ulh_object;
	    if(ddb_b)
		qee_d1_qid(ndsh);
	    ndsh->dsh_qstreamid = NULL;
	}
	else
	{
	    ndsh->dsh_handle = ulm.ulm_streamid;
	    /* For nonrepeat query, use dsh stream for execution too */
	    ndsh->dsh_qstreamid = ulm.ulm_streamid;
	}
	ndsh->dsh_next = (QEE_DSH *)NULL;
	ndsh->dsh_prev = (QEE_DSH *)NULL;
	ndsh->dsh_length = sizeof(QEE_DSH);
	ndsh->dsh_type = QEDSH_CB;
	ndsh->dsh_owner	= (PTR)DB_QEF_ID;
	ndsh->dsh_ascii_id = QEDSH_ASCII_ID;
	ndsh->dsh_key = (PTR)NULL;
	ndsh->dsh_vlt = (QEE_VLT *)NULL;
	ndsh->dsh_cbs = (PTR *)NULL;
	ndsh->dsh_row = (PTR *)NULL;
	ndsh->dsh_hold = (QEN_HOLD **)NULL;
	ndsh->dsh_hash = (QEN_HASH_BASE **)NULL;
	ndsh->dsh_hashagg = (QEN_HASH_AGGREGATE **)NULL;
	ndsh->dsh_xaddrs = (QEE_XADDRS **)NULL;
	ndsh->dsh_qconst_cb = (ADK_CONST_BLK *)NULL;
	ndsh->dsh_dep_dsh = (QEE_DSH *)NULL;
	ndsh->dsh_aqp_dsh = (QEE_DSH *)NULL;
	ndsh->dsh_qp_ptr = qp;
	ndsh->dsh_qp_handle = qsf_rcb->qsf_obj_id.qso_handle;
	ndsh->dsh_qp_status = DSH_QP_LOCKED;
	ndsh->dsh_shd = (QEN_SHD **)NULL;
	ndsh->dsh_stack = NULL;
	ndsh->dsh_exeimm = NULL;
	ndsh->dsh_usr_params = NULL;
	ndsh->dsh_depth_act = 0;
	ndsh->dsh_kor_nor = (QEN_NOR **)NULL;
	ndsh->dsh_resources = (QEE_RESOURCE *) NULL;
	ndsh->dsh_streamid = ulm.ulm_streamid;
	ndsh->dsh_cpuoverhead = 0.0;
	ndsh->dsh_diooverhead = 0.0;
	ndsh->dsh_threadno = 0;
	ndsh->dsh_root = ndsh;
	ndsh->dsh_ahdvalid = (QEF_VALID *) NULL;

	/* if the qp is flagged as possibly needing query constants,
	** allocate a ADF_CONST_BLK, zero it, and store its pointer in the dsh.
	** This needs to be done before qee_create is called, since it will
	** be executing the VIRGIN segment of some CXs, and they will need this.
	*/

	if (qp->qp_status & QEQP_NEED_QCONST)
	{
	    if (ndsh->dsh_qconst_cb == (ADK_CONST_BLK *)NULL)
	    {
		ulm.ulm_psize = sizeof (ADK_CONST_BLK);
		status = qec_malloc(&ulm);
		if (status)
		{
		    qef_rcb->error.err_code = ulm.ulm_error.err_code;
		    goto err_exit;
		}
		ndsh->dsh_qconst_cb = (ADK_CONST_BLK *) ulm.ulm_pptr;
	    }
	    qef_rcb->qef_cb->qef_adf_cb->adf_constants = ndsh->dsh_qconst_cb;

	    /* Make sure we zero out the ADK_CONST_BLK before the query begins  */

	    MEfill( sizeof(ADK_CONST_BLK), (u_char) 0, 
		    (PTR)ndsh->dsh_qconst_cb);
	}

	/* Create the data segment of the query plan */

	status = qee_create(qef_rcb, ndsh, &ulm);
	if (status)
	{
	    goto err_exit;
	}
	MEcopy((PTR) &qef_rcb->qef_qp,  sizeof(DB_CURSOR_ID), 
	    (PTR) &ndsh->dsh_qp_id);
    }
    else
    {
	/* if the qp is flagged as possibly needing query constants,
	** allocate a ADF_CONST_BLK, zero it, and store its pointer in the dsh.
	** This is a previously used DSH. This means that we need to allocate
	** (probably not), and zero out the constant block info. The zeroing
	** out is needed to make sure that the constant expressions (such as
	** date('now')) get reevaluated for this query (rather than using the
	** data from the last query.
	*/

	if (qp->qp_status & QEQP_NEED_QCONST)
	{
	    if (ndsh->dsh_qconst_cb == (ADK_CONST_BLK *)NULL)
	    {
		ulm.ulm_psize = sizeof (ADK_CONST_BLK);
		status = qec_malloc(&ulm);
		if (status)
		{
		    qef_rcb->error.err_code = ulm.ulm_error.err_code;
		    goto err_exit;
		}
		ndsh->dsh_qconst_cb = (ADK_CONST_BLK *) ulm.ulm_pptr;
	    }
	    qef_rcb->qef_cb->qef_adf_cb->adf_constants = ndsh->dsh_qconst_cb;

	    /* Make sure we zero out the ADK_CONST_BLK before the query begins  */

	    MEfill( sizeof(ADK_CONST_BLK), (u_char) 0, 
		    (PTR)ndsh->dsh_qconst_cb);
	}
    }

    /* Consistency check */
    if (ndsh->dsh_qp_ptr != qp ||
	ndsh->dsh_qp_handle != qsf_rcb->qsf_obj_id.qso_handle)
    {
	ndsh->dsh_qp_status |= DSH_QP_OBSOLETE;
	qef_rcb->error.err_code = E_QE0023_INVALID_QUERY;
	status = E_DB_ERROR;
	goto err_exit;
    }

    /* Clean up the DSH: */
    ndsh->dsh_open_dmtcbs = (DMT_CB *)NULL;
    ndsh->dsh_act_ptr = (QEF_AHD *)NULL; /* mark cursor not opened */
    ndsh->dsh_positioned = FALSE;
    ndsh->dsh_qefcb = qef_rcb->qef_cb;
    ndsh->dsh_sid = qef_rcb->qef_cb->qef_ses_id;

    /* ADF CB is the session's.
    ** FIXME this is probably wrong in a parallel query environment,
    ** since two query kids could want to use the error info
    ** simultaneously.  Hasn't caused a problem, YET...
    */
    ndsh->dsh_adf_cb = qef_rcb->qef_cb->qef_adf_cb;

    /* Copy the session's DMF transaction context, if any */
    ndsh->dsh_dmt_id = qef_rcb->qef_cb->qef_dmt_id;

    /* set initial number of potential in-memory sort/holds for this query */
    ndsh->dsh_shd_cnt = qp->qp_sort_cnt + qp->qp_hld_cnt;

    /* Clean up QEN_HOLD array */
    if (ndsh->dsh_hold)
    {
	for (i = 0; i < qp->qp_hld_cnt; i++)
	{
	    ndsh->dsh_hold[i]->hold_tid = 0;
	    ndsh->dsh_hold[i]->hold_status = 0;
	    ndsh->dsh_hold[i]->hold_medium = 0;
	    ndsh->dsh_hold[i]->hold_buffer_status = 0;
	    ndsh->dsh_hold[i]->hold_reset_counter = 0;
	    ndsh->dsh_hold[i]->hold_reset_pending = FALSE;
	    ndsh->dsh_hold[i]->hold_inner_status = 0;
	    ndsh->dsh_hold[i]->unget_status = 0;
	    ndsh->dsh_hold[i]->unget_buffer = (PTR)NULL;
	}
    }

    /* Clean up QEN_STATUS array */
    if (ndsh->dsh_xaddrs)
    {
	QEE_XADDRS *xaddrs = ndsh->dsh_xaddrs[0];
	for (i = 0; i < qp->qp_stat_cnt; i++, xaddrs++)
	{
	    if (xaddrs->qex_status)
		MEfill( sizeof(QEN_STATUS), (u_char)0, (PTR)xaddrs->qex_status);
	}
    }

    /* Clean up QEN_SHD array -- sort type only -- in case this is
    ** a repeat query */

    if (qp->qp_sort_cnt != 0)
    {
	for (i = 0; i < qp->qp_sort_cnt; i++)
         MEfill( sizeof(QEN_SHD), (u_char)0, (PTR)ndsh->dsh_shd[i] );
    }

    /* Clean up/init QEN_HASH array */
    if (ndsh->dsh_hash)
    {
	for (i = 0; i < qp->qp_hash_cnt; i++)
	{
	    QEN_HASH_BASE *hbase = ndsh->dsh_hash[i];

	    hbase->hsh_streamid = NULL;
	    hbase->hsh_htable = NULL;
	    hbase->hsh_freefile = NULL;
	    hbase->hsh_openfiles = NULL;
	    hbase->hsh_availfiles = NULL;
	    hbase->hsh_cartprod = NULL;
	    hbase->hsh_firstbr = NULL;
	    hbase->hsh_nextbr = NULL;
	    hbase->hsh_rbptr = NULL;
	    hbase->hsh_htsize = 0;
	    hbase->hsh_fcount = 0;
	    hbase->hsh_jcount = 0;
	    hbase->hsh_ccount = 0;
	    hbase->hsh_maxreclvl = 0;
	    hbase->hsh_bwritek = 0;
	    hbase->hsh_pwritek = 0;
	    hbase->hsh_memparts = 0;
	    hbase->hsh_rpcount = 0;
	    hbase->hsh_bvdcnt = 0;
	    hbase->hsh_lastloc = -1;
	    hbase->hsh_currpart = -1;
	    hbase->hsh_brcount = 0;
	    hbase->hsh_prcount = 0;
	    hbase->hsh_size = 0;
	    hbase->hsh_flags &= (HASH_WANT_LEFT | HASH_WANT_RIGHT
			| HASH_CHECKNULL); 
				/* drop all but OJ & null check flags */
	    hbase->hsh_currows_left = 0;
	    hbase->hsh_currbucket = 0;
	    hbase->hsh_count1 = 0;
	    hbase->hsh_count2 = 0;
	    hbase->hsh_count3 = 0;
	    hbase->hsh_count4 = 0;

	    /* Init top LINK structure. */
	    hbase->hsh_toplink->hsl_next = (QEN_HASH_LINK *)NULL;
	    hbase->hsh_toplink->hsl_prev = (QEN_HASH_LINK *)NULL;
	    hbase->hsh_toplink->hsl_brcount = 0;
	    hbase->hsh_toplink->hsl_prcount = 0;
	    hbase->hsh_toplink->hsl_level = 0;
	    hbase->hsh_toplink->hsl_pno = -1;
	    hbase->hsh_toplink->hsl_flags = 0;

	    /* And init the top array of partition descriptors. */
	    for (j = 0; j < hbase->hsh_pcount; j++)
	    {
		QEN_HASH_PART	*hpart;

		hpart = &hbase->hsh_toplink->hsl_partition[j];

		hpart->hsp_bptr = NULL;
		hpart->hsp_bfile = (QEN_HASH_FILE *) NULL;
		hpart->hsp_pfile = (QEN_HASH_FILE *) NULL;
		hpart->hsp_bvptr = NULL;
		hpart->hsp_offset = 0;
		hpart->hsp_brcount = 0;
		hpart->hsp_prcount = 0;
		hpart->hsp_currbn = 0;
		hpart->hsp_reclevel = 0;
		hpart->hsp_flags  = (HSP_BALLSAME | HSP_PALLSAME);
	    }

	    /* Tidy up some stuff in the hash join's DMH_CB. */
	    dmhcb = (DMH_CB *)hbase->hsh_dmhcb;
	    dmhcb->dmh_file_context = (PTR) NULL;
	    dmhcb->dmh_buffer = (PTR) NULL;
	    dmhcb->dmh_locidxp = (i4 *) NULL;
	    dmhcb->dmh_numlocs = 0;
	    dmhcb->dmh_rbn = 0;
	    dmhcb->dmh_func = 0;
	    dmhcb->dmh_flags_mask = 0;
	    dmhcb->dmh_db_id = qef_rcb->qef_db_id;
	    dmhcb->dmh_tran_id = (PTR) NULL;
	}
    }

    /* Clean up/init QEN_HASH_AGGREGATE array */
    if (ndsh->dsh_hashagg)
    {
	for (i = 0; i < qp->qp_hashagg_cnt; i++)
	{
	    ndsh->dsh_hashagg[i]->hshag_streamid = NULL;
	    ndsh->dsh_hashagg[i]->hshag_htable = NULL;
	    ndsh->dsh_hashagg[i]->hshag_lastrow = NULL;
	    ndsh->dsh_hashagg[i]->hshag_freefile = NULL;
	    ndsh->dsh_hashagg[i]->hshag_openfiles = NULL;
	    ndsh->dsh_hashagg[i]->hshag_rbptr = NULL;
	    ndsh->dsh_hashagg[i]->hshag_halloc = 0;
	    ndsh->dsh_hashagg[i]->hshag_rcount = 0;
	    ndsh->dsh_hashagg[i]->hshag_gcount = 0;
	    ndsh->dsh_hashagg[i]->hshag_ocount = 0;
	    ndsh->dsh_hashagg[i]->hshag_fcount = 0;
	    ndsh->dsh_hashagg[i]->hshag_curr_bucket = 0;
	    ndsh->dsh_hashagg[i]->hshag_lastloc = -1;
	    ndsh->dsh_hashagg[i]->hshag_flags = 0;

	    /* Init top LINK structure. */
	    ndsh->dsh_hashagg[i]->hshag_toplink->hsl_next = (QEN_HASH_LINK *)NULL;
	    ndsh->dsh_hashagg[i]->hshag_toplink->hsl_prev = (QEN_HASH_LINK *)NULL;
	    ndsh->dsh_hashagg[i]->hshag_toplink->hsl_brcount = 0;
	    ndsh->dsh_hashagg[i]->hshag_toplink->hsl_prcount = 0;
	    ndsh->dsh_hashagg[i]->hshag_toplink->hsl_level = 0;
	    ndsh->dsh_hashagg[i]->hshag_toplink->hsl_pno = -1;
	    ndsh->dsh_hashagg[i]->hshag_toplink->hsl_flags = 0;

	    /* And init the top array of partition descriptors. */
	    for (j = 0; j < ndsh->dsh_hashagg[i]->hshag_pcount; j++)
	    {
		QEN_HASH_PART	*hpart;

		hpart = &ndsh->dsh_hashagg[i]->hshag_toplink->hsl_partition[j];

		hpart->hsp_bptr = NULL;
		hpart->hsp_bfile = (QEN_HASH_FILE *) NULL;
		hpart->hsp_pfile = (QEN_HASH_FILE *) NULL;
		hpart->hsp_bvptr = NULL;
		hpart->hsp_offset = 0;
		hpart->hsp_brcount = 0;
		hpart->hsp_prcount = 0;
		hpart->hsp_currbn = 0;
		hpart->hsp_reclevel = 0;
		hpart->hsp_flags  = (HSP_BALLSAME | HSP_PALLSAME);
	    }

	    /* Tidy up some stuff in the hash join's DMH_CB. */
	    dmhcb = (DMH_CB *)ndsh->dsh_hashagg[i]->hshag_dmhcb;
	    dmhcb->dmh_file_context = (PTR) NULL;
	    dmhcb->dmh_buffer = (PTR) NULL;
	    dmhcb->dmh_locidxp = (i4 *) NULL;
	    dmhcb->dmh_numlocs = 0;
	    dmhcb->dmh_rbn = 0;
	    dmhcb->dmh_func = 0;
	    dmhcb->dmh_flags_mask = 0;
	    dmhcb->dmh_db_id = qef_rcb->qef_db_id;
	    dmhcb->dmh_tran_id = (PTR) NULL;
	}
    }

    /* Clean up/init DSH rows.  The ones we really need to clean up are
    ** rows that hold partition qualification bitmaps, eg for a qualified
    ** orig.  But by the time we figure out which rows NEED to be cleared,
    ** we might as well do them all.
    */
    if (qp->qp_row_len != NULL)
    {
	for (i = 0; i < qp->qp_row_cnt; ++i)
	{
	    if (qp->qp_row_len[i] > 0)
	    {
	        ndsh->dsh_row[i] = ndsh->dsh_row_orig[i];
		MEfill(qp->qp_row_len[i], 0, ndsh->dsh_row[i]);
	    }
	}
    }


    /* If this a remote update (QEA_RUP) or remote delete (QEA_RDEL) QP,    */
    /* adjust the associated QP list */
    if (qp->qp_ahd->ahd_atype == QEA_RUP 
	|| qp->qp_ahd->ahd_atype == QEA_RDEL
	|| qp->qp_ahd->ahd_atype == QEA_D9_UPD)
    {
	PTR	    handle;

	/* find the associated QP. If it is not available on the 
	** session control block list, it is not open.
	*/
	if (ddb_b && (   qp->qp_ahd->ahd_atype == QEA_D9_UPD
		      || qp->qp_ahd->ahd_atype == QEA_RDEL))
	    handle = qp->qp_ahd->qhd_obj.qhd_d1_qry.qeq_q13_csr_handle;
	else
	    handle = qp->qp_ahd->qhd_obj.qhd_qep.ahd_qhandle;

	for (next = (QEE_DSH *)qef_rcb->qef_cb->qef_odsh; next;
	     next = next->dsh_next)
	{
	    /* found an active DSH */
	    if (next->dsh_qp_handle == handle)
		break;
	}
	/* if not found, the query is not open, put the RUP DSH on the
	** env queue. This should never happen unless the RUP query
	** is a repeat query.
	*/
	if (next == (QEE_DSH *)NULL)
	{
	    if (qp->qp_status & QEQP_RPT)
	    {
		have_dsh = FALSE;
		ulh_rcb.ulh_object = (ULH_OBJECT *)ndsh->dsh_handle;
		status = ulh_release(&ulh_rcb);
		if (status != E_DB_OK)
		{
		    qef_error(ulh_rcb.ulh_error.err_code, 0L, status, &err,
			&qef_rcb->error, 0);
		    qef_rcb->error.err_code = E_QE0104_RELEASE_DSH_OBJECT;
		    goto err_exit;
		}
	    }
	    status = E_DB_ERROR;
	    qef_rcb->error.err_code = E_QE0008_CURSOR_NOT_OPENED;
	    goto err_exit;
	}
	ndsh->dsh_dep_dsh = next->dsh_dep_dsh;
	ndsh->dsh_aqp_dsh = next;
	next->dsh_dep_dsh = ndsh;
    }

    /* give to caller */

    *dsh = ndsh;
    next = (QEE_DSH *)qef_rcb->qef_cb->qef_odsh;
    ndsh->dsh_next = next;
    if (next)
    {
	next->dsh_prev = ndsh;
    }

    qef_rcb->qef_cb->qef_dsh = (PTR)ndsh;
    qef_rcb->qef_cb->qef_odsh = (PTR)ndsh;

#ifdef xDEBUG
    (VOID) qe2_chk_qp(ndsh);
#endif 
    return (status);

err_exit:
    if (status != E_DB_OK)
    {
	    if ( qp->qp_status & QEQP_RPT)
	    {
		qsf_call(QSO_UNLOCK, qsf_rcb);
		if (have_dsh)
		    ulh_destroy(&ulh_rcb, (unsigned char *)NULL, 0);
	    }
	    else
	    {
		qsf_call(QSO_DESTROY, qsf_rcb);
		if (have_dsh)
		    ulm_closestream(&ulm);
	    }
	*dsh = (QEE_DSH *)NULL;
    }
    return (status);
}

/*{
** Name: QEE_CREATE	- fill in the query data segment.
**
** Description:
**      The QP is traversed and all necessary control blocks are created.
** 
** Inputs:
**	 qef_rcb			request block
**       dsh				data segment header
**	 ulm				memory stream out of which the
**					DSH is created.
**
** Outputs:
**      qef_rcb                         
**	    .error.err_code                  one of the following
**				E_QE0000_OK
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0019_NON_INTERNAL_FAIL
**				E_QE0002_INTERNAL_ERROR
**				E_QE000D_NO_MEMORY_LEFT
**				E_QE000F_OUT_OF_OTHER_RESOURCES
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    the DSH is expanded
**
** History:
**	2-jul-86 (daved)
**          written
**      10-dec-87 (puree)
**          Converted all ulm_palloc to qec_palloc.
**	01-aug-88 (puree)
**	    Implement list of end-of-block actions to handle while/endloop
**	    combination.
**	12-sep-88 (puree)
**	    Temporary fix for problem with row sizes in QP.  Always add
**	    DB_MAXTUP to the row length specified by the QP.
**	04-oct-88 (puree)
**	    Removed temporary fix.  QP is fixed in OPC.
**	24-dec-88 (carl)
**	    modified for Titan: create status structure to support temporary 
**	    tables
**	15-jan-89 (paul)
**	    Add initialization for parameter structures for nested
**	    procedure calls.
**	12-may-89 (carl)
**	    modified to create space for tuple descriptor for RQF for SELECT 
**	    and CURSOR queries
**	07-jan-91 (davebf)
**	    Init new fields in qen_hold.  Allocate qen_shd's for in-memory
**	    holds.
**	11-aug-92 (rickh)
**	    Allocate space for temporary table descriptors.
**	04-sep-92 (fpang)
**	    Added explicit break after distributed processing code.
**	    Also initialized local variable, ddq_p.
**	21-dec-92 (anitap)
**	    Prototyped.
**      12-jan-93 (mikem)
**          lint cleanup.  Removed initialization of ddq_p when it was
**          declared.  It was already initialized in the body of the
**          procedure.
**      20-jul-1993 (pearl)
**          Bug 45912.  Allocate memory for (new) dsh_kor_nor array in
**          qee_create().  Initialize new field qef_kor in QEN_NOR structure
**          in qee_cract(), and save pointer to QEN_NOR structure in the
**          dsh_kor_nor array.  In qee_fetch(), initialize dsh_kor_nor
**          to null at create time.
**	 1-oct-96 (nick)
**	    Zero out dsh_row entries. #78815
**	13-feb-97 (inkdo01)
**	    Add QEE_MEM_CONSTTAB structure to resource heirarchy for MS Access
**	    OR transforms.
**	3-mar-99 (inkdo01)
**	    Add support for hash joins (QEN_HASH_BASE init).
**	1-feb-01 (inkdo01)
**	    Add support for hash aggregate actions (QEN_HASH_AGGREGATE init).
**	18-mar-02 (inkdo01)
**	    Add support for sequences (alloc/format DMS_CBs).
**	06-Feb-2003 (jenjo02)
**	    Deleted duplicate block of sequence allocation code.
**	16-Jul-2004 (schka24)
**	    Move dsh-row clearing to qee-fetch; make static.
**	05-Aug-2004 (jenjo02)
**	    Get as much memory as possible with one call to
**	    ULM, then carve it up.
**	26-aug-04 (inkdo01)
**	    Add support for global base arrays.
**	6-Dec-2005 (kschendel)
**	    Don't need to allocate compare-list pointers any more.
**	16-Jan-2006 (kschendel)
**	    Allocate XADDRS structures;  simplify things by putting all
**	    the pointer arrays together at the start, then all the structs.
**	25-Jan-2008 (kibro01) b116600
**	    Because hash joins and parallel queries can amend the dsh_row
**	    value, store the original and reuse that when giving back a qep.
*/

static DB_STATUS
qee_create(
	QEF_RCB		*qef_rcb,
	QEE_DSH		*dsh,
	ULM_RCB		*ulm)
{
    DB_STATUS	    status = E_DB_OK;
    DB_STATUS	    ulm_status = E_DB_OK;
    QEF_QP_CB	    *qp;
    GCA_TD_DATA	    *gca_p;
    QEQ_DDQ_CB	    *ddq_p;	
    QEE_DDB_CB	    *qee_p = dsh->dsh_ddb_cb;
    QEF_DDB_REQ	    *ddr_p = & qef_rcb->qef_r3_ddb_req;
    QEF_AHD	    **eobptr;
    ULM_RCB	    eob_ulm;
    i4		    i, j, np;
    GLOBALREF QEF_S_CB *Qef_s_cb;
    QEF_RESOURCE    *qp_resource;
    PTR		    Mem, ptrs;
    bool	    globalbase;
			    

#ifdef xDEBUG
    (VOID) qe2_chk_qp(dsh);
#endif 

    qp = dsh->dsh_qp_ptr;
    ddq_p = & qp->qp_ddq_cb;
    gca_p = (GCA_TD_DATA *) qp->qp_sqlda;
    globalbase = (qp->qp_status & QEQP_GLOBAL_BASEARRAY) ? TRUE : FALSE;

    /* Compute the amount of memory we need for the easy stuff */
    ulm->ulm_psize = 0;

    /* Pointer arrays.  Counting qp_hld_cnt twice is not a mistake,
    ** one is for the SHD pointer and one is for the HOLD pointer.
    ** All pointer arrays should be included in "np" so that we can
    ** put all the pointer arrays together, at the start.
    */
    /* 2*row_cnt because we want to keep the original values */
    np = qp->qp_stat_cnt + qp->qp_sort_cnt + qp->qp_hld_cnt
	+ qp->qp_kor_cnt + qp->qp_hld_cnt + qp->qp_tempTableCount
	+ qp->qp_cb_cnt + qp->qp_hash_cnt + qp->qp_hashagg_cnt
	+ (2 * qp->qp_row_cnt) + qp->qp_cp_cnt;
    ulm->ulm_psize += DB_ALIGN_MACRO(np * sizeof(PTR));
    /* Actual structures */
    i = DB_ALIGN_MACRO(qp->qp_stat_cnt * sizeof(QEN_STATUS));
    i += DB_ALIGN_MACRO(qp->qp_stat_cnt * sizeof(QEE_XADDRS));
    i += DB_ALIGN_MACRO((qp->qp_sort_cnt + qp->qp_hld_cnt) * sizeof(QEN_SHD));
    i += DB_ALIGN_MACRO(qp->qp_hld_cnt * sizeof(QEN_HOLD));
    i += DB_ALIGN_MACRO(qp->qp_tempTableCount * sizeof(QEE_TEMP_TABLE));
    i += DB_ALIGN_MACRO(qp->qp_pcx_cnt * sizeof(QEE_VLT));
    if (!globalbase)
	i += DB_ALIGN_MACRO(qp->qp_key_sz);
    i += DB_ALIGN_MACRO(qp->qp_hash_cnt * sizeof(QEN_HASH_BASE));
    i += DB_ALIGN_MACRO(qp->qp_hashagg_cnt * sizeof(QEN_HASH_AGGREGATE));
    i += DB_ALIGN_MACRO(qp->qp_cnt_resources * sizeof (QEE_RESOURCE));
    ulm->ulm_psize += i;

    if ( qp->qp_row_cnt > 0 && qp->qp_row_len != NULL)
    {
	/* Ensure all row buffers are aligned properly for DMF,
	** thus we align each one, not the sum
	*/

	for (i = 0; i < qp->qp_row_cnt; i++)
	{
	    if (qp->qp_row_len[i] > 0)
		ulm->ulm_psize += DB_ALIGN_MACRO(qp->qp_row_len[i]);
	}
    }

    if ( qp->qp_cp_cnt > 0 )
    {
	for (i = 0; i < qp->qp_cp_cnt; i++)
	    ulm->ulm_psize += DB_ALIGN_MACRO(qp->qp_cp_size[i] *
				  sizeof(QEF_USR_PARAM));
    }

    /* Doubt that this could ever happen... */
    if ( ulm->ulm_psize == 0 )
	return(E_DB_OK);
	
    /* Now allocate it, zero the whole mess */
    if (ulm_status = qec_malloc(ulm))
    {
	qef_rcb->error.err_code = ulm->ulm_error.err_code;
	return(ulm_status);
    }
    MEfill(ulm->ulm_psize, 0, ulm->ulm_pptr);

    /* "ptrs" is the next available pointer-array slot.
    ** "Mem" is the next available aligned structure slot.
    ** Note that as we go, no need to re-align "ptrs" because all of
    ** the pointers are together.
    */
    ptrs = ulm->ulm_pptr;
    Mem = ME_ALIGN_MACRO(((PTR *)ptrs + np), DB_ALIGN_SZ);

    /* to break out of */
    for (;;)
    {
	/* Allocate the array of QEE_XADDRS for all nodes, and allocate
	** the QEN_STATUS entries too
	*/
	if (qp->qp_stat_cnt != 0)
	{
	    QEN_STATUS	*stat;
	    QEE_XADDRS	*xaddrs;

	    dsh->dsh_xaddrs = (QEE_XADDRS **) ptrs;
	    ptrs += qp->qp_stat_cnt * sizeof(PTR);
	    stat = (QEN_STATUS*)Mem;
	    Mem += DB_ALIGN_MACRO(qp->qp_stat_cnt * sizeof(QEN_STATUS));
	    xaddrs = (QEE_XADDRS *) Mem;
	    Mem += DB_ALIGN_MACRO(qp->qp_stat_cnt * sizeof(QEE_XADDRS));
	    for (i = 0; i < qp->qp_stat_cnt; i++)
	    {
	    	dsh->dsh_xaddrs[i] = xaddrs;
		xaddrs->qex_status = stat;
		/* Resolve the rest of the execution addresses later */
		++ xaddrs;
		++ stat;
	    }
	}

	/* Allocate the array of QEN_SHDs for sort nodes and for hold files */
	j = qp->qp_sort_cnt + qp->qp_hld_cnt;
	if (j != 0)
	{
	    QEN_SHD	*shd;
	    dsh->dsh_shd = (QEN_SHD **) ptrs;
	    ptrs += j * sizeof(PTR);
	    shd = (QEN_SHD*)Mem;
	    Mem += DB_ALIGN_MACRO(j * sizeof(QEN_SHD));
	    for (i = 0; i < j; i++)
		dsh->dsh_shd[i] = shd++;
	}

        /* Allocate the array of PTR's to QEN_NOR's; bug 45912 */
        if (qp->qp_kor_cnt != 0)
        {
            dsh->dsh_kor_nor = (QEN_NOR **) ptrs;
	    ptrs += qp->qp_kor_cnt * sizeof(PTR);
       }

	/* allocate the array of QEN_HOLDs for the nodes that need them */
	if (qp->qp_hld_cnt)
	{
	    QEN_HOLD	*hold;

	    dsh->dsh_hold = (QEN_HOLD **) ptrs;
	    ptrs += qp->qp_hld_cnt * sizeof(PTR);
	    hold = (QEN_HOLD *)Mem;
	    Mem += DB_ALIGN_MACRO(qp->qp_hld_cnt * sizeof(QEN_HOLD));
	    for (i = 0; i < qp->qp_hld_cnt; i++)
		dsh->dsh_hold[i] = hold++;
	}

	/* Allocate the array of QEE_TEMP_TABLE for all nodes */
	dsh->dsh_tempTables = (QEE_TEMP_TABLE **) NULL;
	if (qp->qp_tempTableCount != 0)
	{
	    QEE_TEMP_TABLE	*ttabp;
	    dsh->dsh_tempTables = (QEE_TEMP_TABLE **) ptrs;
	    ptrs += qp->qp_tempTableCount * sizeof(PTR);
	    ttabp = (QEE_TEMP_TABLE *)Mem;
	    Mem += DB_ALIGN_MACRO(qp->qp_tempTableCount * sizeof(QEE_TEMP_TABLE));
	    for (i = 0; i < qp->qp_tempTableCount; i++)
		dsh->dsh_tempTables[i] = ttabp++;
	}

	/* Allocate the array of VLUP/VLT descriptors */
	if (qp->qp_pcx_cnt)
	{
	    dsh->dsh_vlt = (QEE_VLT *)Mem;
	    Mem += DB_ALIGN_MACRO(qp->qp_pcx_cnt * sizeof(QEE_VLT));
	}

	/* Allocate buffer for keys */
	if (qp->qp_key_sz && !globalbase)
	{
	    dsh->dsh_key = Mem;
	    Mem += DB_ALIGN_MACRO(qp->qp_key_sz);
	}
	
	/* Allocate array of pointers to DMF/ADE control blocks.
	** The control blocks themselves will be allocated later */
	if (qp->qp_cb_cnt)
	{
	    dsh->dsh_cbs = (PTR *)ptrs;
	    ptrs += qp->qp_cb_cnt * sizeof(PTR);
	}

	/* Allocate array of pointers to row buffers */
	if (qp->qp_row_cnt)
	{
	    dsh->dsh_row = (PTR*)ptrs;
	    ptrs += qp->qp_row_cnt * sizeof(PTR);
	    dsh->dsh_row_orig = (PTR*)ptrs;
	    ptrs += qp->qp_row_cnt * sizeof(PTR);

	    /* Place the row buffers, properly aligned */
	    if ( qp->qp_row_len != NULL )
	    {
		for (i = 0; i < qp->qp_row_cnt; i++)
		{
		    if ( qp->qp_row_len[i] > 0 )
		    {
			dsh->dsh_row[i] = Mem;
			dsh->dsh_row_orig[i] = Mem;
			Mem += DB_ALIGN_MACRO(qp->qp_row_len[i]);
		    }
		}
	    }

	    /* If we're using the global base array, move key buffer
	    ** address into dsh_key. */
	    if (globalbase && qp->qp_key_row >= 0)
		dsh->dsh_key = dsh->dsh_row[qp->qp_key_row];
	}

	/* Allocate array of pointers to hash join base structures */
	if (qp->qp_hash_cnt)
	{
	    QEN_HASH_BASE	*hash;

	    dsh->dsh_hash = (QEN_HASH_BASE **) ptrs;
	    ptrs += qp->qp_hash_cnt * sizeof(PTR);
	    hash = (QEN_HASH_BASE *)Mem;
	    Mem += DB_ALIGN_MACRO(qp->qp_hash_cnt * sizeof(QEN_HASH_BASE));
	    for (i = 0; i < qp->qp_hash_cnt; i++)
		dsh->dsh_hash[i] = hash++;
	}

	/* Allocate array of hash aggregate structures */
	if (qp->qp_hashagg_cnt)
	{
	    QEN_HASH_AGGREGATE	*hashagg;

	    dsh->dsh_hashagg = (QEN_HASH_AGGREGATE **) ptrs;
	    ptrs += qp->qp_hashagg_cnt * sizeof(PTR);
	    hashagg = (QEN_HASH_AGGREGATE *)Mem;
	    Mem += DB_ALIGN_MACRO(qp->qp_hashagg_cnt * sizeof(QEN_HASH_AGGREGATE));
	    for (i = 0; i < qp->qp_hashagg_cnt; i++)
		dsh->dsh_hashagg[i] = hashagg++;
	}

	/* Allocate array of pointers to QEF_USR_PARAM arrays for nested */
	/* procedure calls. Also allocate the parameter arrays themselves.  */
	/* Note these parameter structures are different from those used on */
	/* a user invocation of a database procedure. User requests to QEF  */
	/* for procedure execution pass the parameters as an array of	    */
	/* pointers, one pointer for each actual parameter. For a nested    */
	/* CALLPROC statement, a pointer to a contiguous array of	    */
	/* parameters is constructed. This first structure is an array of   */
	/* pointers to such parameter lists, one entry for each CALLPROC in */
	/* the QP. */
	if (qp->qp_cp_cnt)
	{
	    dsh->dsh_usr_params = (PTR *)ptrs;
	    ptrs += qp->qp_cp_cnt * sizeof(PTR);

	    /* Allocate each QEF_USR_PARAM array. The size of each parameter */
	    /* list has been stored in the array qp->qp_cp_cnt */
	    for (i = 0; i < qp->qp_cp_cnt; i++)
	    {
		dsh->dsh_usr_params[i] = Mem;
		Mem += DB_ALIGN_MACRO(qp->qp_cp_size[i] * sizeof(QEF_USR_PARAM));
	    }
	}

	/* Allocate array of qee resource descriptions */
	if (qp->qp_cnt_resources)
	{
	    dsh->dsh_resources = (QEE_RESOURCE *)Mem;
	    Mem += DB_ALIGN_MACRO(qp->qp_cnt_resources * sizeof(QEE_RESOURCE));

	    /* Also, fill in the qee resource descriptions */
	    for (qp_resource = qp->qp_resources;
		    qp_resource != NULL;
		    qp_resource = qp_resource->qr_next
		)
	    {
		QEE_TBL_RESOURCE *qee_tbl_res;

		i = qp_resource->qr_id_resource;

		dsh->dsh_resources[i].qer_qefresource = (PTR) qp_resource;

		if (qp_resource->qr_type == QEQR_TABLE)
		{
		    qee_tbl_res = 
		     &dsh->dsh_resources[i].qer_resource.qer_tbl;
		    qee_tbl_res->qer_inuse_vl = NULL;
		    qee_tbl_res->qer_free_vl = NULL;
		    qee_tbl_res->qer_empty_vl = NULL;
		    if (qp_resource->qr_resource.qr_tbl.qr_tbl_type !=
			    QEQR_MCNSTTBL) qee_tbl_res->qer_cnsttab_p = NULL;
		    else
		    {
			ulm->ulm_psize = sizeof(QEE_MEM_CONSTTAB);
			if (ulm_status = qec_malloc(ulm))
			{
			    qef_rcb->error.err_code = ulm->ulm_error.err_code;
			    break;
			}
			qee_tbl_res->qer_cnsttab_p = 
				    (QEE_MEM_CONSTTAB *) ulm->ulm_pptr;
		    }
		}
	    }
	}

	/* Allocate/format DMS_CBs for sequence operators. */
	if (qp->qp_sequences != NULL)
	{
	    QEF_SEQUENCE	*qseqp;
	    DMS_CB		*dms_cb;
	    DMS_SEQ_ENTRY	*dseqp;

	    for (qseqp = qp->qp_sequences; qseqp != NULL; 
				qseqp = qseqp->qs_qpnext)
	    {
		ulm->ulm_psize = sizeof(DMS_CB) + sizeof(DMS_SEQ_ENTRY);
		if (ulm_status = qec_malloc(ulm))
		{
		    qef_rcb->error.err_code = ulm->ulm_error.err_code;
		    break;
		}
		dms_cb = (DMS_CB *)ulm->ulm_pptr;
		dseqp = (DMS_SEQ_ENTRY *)((char *)dms_cb + sizeof(DMS_CB));

		/* Init. the control blocks. */
		dms_cb->q_next = (PTR)NULL;
		dms_cb->q_prev = (PTR)NULL;
		dms_cb->length 	= sizeof (DMS_CB);
		dms_cb->type = DMS_SEQ_CB;
		dms_cb->owner = (PTR)DB_QEF_ID;
		dms_cb->ascii_id = DMS_ASCII_ID;
		dms_cb->dms_db_id = qef_rcb->qef_db_id;
		dms_cb->dms_flags_mask = (qseqp->qs_flag & QS_NEXTVAL) 
			? DMS_NEXT_VALUE : DMS_CURR_VALUE;
		dms_cb->dms_seq_array.data_address = (PTR)dseqp;
		dms_cb->dms_seq_array.data_in_size = sizeof(DMS_SEQ_ENTRY);

		/* Now init DMS_SEQ_ENTRY structure. */
		MEfill(sizeof(DMS_SEQ_ENTRY), 0, (PTR)dseqp);
		STRUCT_ASSIGN_MACRO(qseqp->qs_seqname, dseqp->seq_name);
		STRUCT_ASSIGN_MACRO(qseqp->qs_owner, dseqp->seq_owner);
		STRUCT_ASSIGN_MACRO(qseqp->qs_id, dseqp->seq_id);
		dseqp->seq_value.data_address = dsh->dsh_row[qseqp->qs_rownum];
		dseqp->seq_value.data_out_size = qp->qp_row_len[qseqp->qs_rownum];

		dsh->dsh_cbs[qseqp->qs_cbnum] = (PTR)dms_cb;
	    }
	}

	if (qef_rcb->qef_cb->qef_c1_distrib & DB_3_DDB_SESS)
	{
	    ulm_status = qee_d2_tmp(qef_rcb, dsh, ulm);	/* set up */
	    if (ulm_status)
		qed_u10_trap();

	    if (ulm_status == E_DB_OK && ddq_p->qeq_d4_total_cnt > 0)
	    {
		ulm_status = qee_d3_agg(qef_rcb, dsh, ulm);
		if (ulm_status)
		    qed_u10_trap();
	    }

	    if (qp->qp_qmode == QEQP_01QM_RETRIEVE	/* SELECT query ? */
		||
		qp->qp_qmode == QEQP_02QM_DEFCURS)	/* CURSOR query ? */
	    {
		/* embed info whether attribute names will be returned by LDB */

		if (gca_p->gca_result_modifier & GCA_NAMES_MASK)
		    qee_p->qee_d3_status |= QEE_07Q_ATT;
		else
		    qee_p->qee_d3_status &= ~QEE_07Q_ATT;
	    }

	    if (ddq_p->qeq_d8_mask & QEQ_D8_INGRES_B)
		qee_p->qee_d3_status |= QEE_05Q_CDB;
	    break;
	}
	else
	{
	    /*
	    ** Allocate buffer for the array of end-of-block actions.  This
	    ** array is used by qee_cract to recognize an end of while/if block.
	    */
	    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, eob_ulm);

	    /* Open the stream and allocate the buffer in one action */
	    eob_ulm.ulm_flags = ULM_PRIVATE_STREAM | ULM_OPEN_AND_PALLOC;
	    eob_ulm.ulm_psize = eob_ulm.ulm_blocksize =
			(2 * qp->qp_ahd_cnt * sizeof(PTR));

	    ulm_status = qec_mopen(&eob_ulm);
	    if (ulm_status)
	    {
		qef_rcb->error.err_code = eob_ulm.ulm_error.err_code;
		break;
	    }

	    eobptr = (QEF_AHD **)(eob_ulm.ulm_pptr);
	    /* 
	    ** Call qee_cract to allocate and initialize action-specific control
	    ** blocks and buffers.
	    */
	    status = qee_cract(qef_rcb, dsh, qp->qp_ahd, (QEF_AHD **)eobptr, 
				 (i4) 0, ulm);

	    /* Close temporary memory stream, regardless of qee_cract status */
	    ulm_status = ulm_closestream(&eob_ulm);			
	    if (ulm_status)
	    {
		qef_rcb->error.err_code = eob_ulm.ulm_error.err_code;
		break;
	    }
	    break;
	}
    }
    if (ulm_status > status)
	return (ulm_status);
    else
	return (status);
}

/*{
** Name: QEE_CRACT	- allocate and initialize action-specific control
**			  blocks and buffers.
**
** Description:
**	Traversing the linked lists of action headers, QEE_CRACT allocates 
**	and initialize the data structures specified in each action.  The 
**	control blocks to be allocated and initialized are the dmt_cb's, the
**	dmr_cb's, ade_excb's, and key arrays.  QEE_CRACT follows the list of
**	ahd_next in each action header.  Notice that an IF action has TRUE
**	and FALSE lists that need to be processed.  This is done in as
**	recusive calls.  The ahd_next of an IF action point to the next
**	action at the end of the IF block.  QEN_QP nodes are also handled 
**	recursively.
**
** Inputs:
**      qef_rcb                 session control block
**      dsh                     data segment header
**	bgn_act			The first action header to be processed.
**	eobptr			Pointer to the list of end-of-block actions.
**				qee_cract terminates when the current action
**				is one of the action in the list (the action
**				is not processed).
**	eobcount		Count of action pointers in the eob list.
**      ulm                     ULM control block containing memory stream
**				for the DSH.
**
** Outputs:
**      qef_rcb                         
**	    .error.err_code                  one of the following
**				E_QE0000_OK
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0019_NON_INTERNAL_FAIL
**				E_QE0002_INTERNAL_ERROR
**				E_QE000D_NO_MEMORY_LEFT
**				E_QE000F_OUT_OF_OTHER_RESOURCES
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    the DSH is expanded
**
**
** History:
**      8-sep-86 (daved)
**          written
**	28-sep-87 (rogerk)
**	    Look for QEA_LOAD type.
**      10-dec-87 (puree)
**          Converted all ulm_palloc to qec_palloc.
**	22-feb-88 (puree)
**	    Do not set table lock mode here.  It will be set later by
**	    qeq_validate based on the estimated number of pages touched.
**      03-aug-88 (puree)
**	    Modify for IF actions for DB procedure.
**	07-oct-88 (puree)
**	    Fix key structure allocation.  The structures must
**	    be kept in the same order as they are in the QP.
**	07-sep-89 (neil)
**	    Alerters: Added initialization of new EVENT actions.
**	14-jan-92 (nancy)
**	    Bug 39225, add support under IF and RETURN for initializing
**	    local dbp variables.
**	10-feb-1992 (bryanp)
**	    Changed dmt_location from a DB_LOC_NAME to a DM_DATA.
**	21-dec-92 (anitap)
**	    Prototyped.
**	18-mar-93 (jhahn)
**	    Added support for set input procedures.
**	02-apr-93 (jhahn)
**	    Added support for statement
**	    level unique indexes.
**	17-may-93 (jhahn)
**	    Added more support for statement
**	    level unique indexes.
**      20-jul-1993 (pearl)
**          Bug 45912.  Allocate memory for (new) dsh_kor_nor array in
**          qee_create().  Initialize new field qef_kor in QEN_NOR structure
**          in qee_cract(), and save pointer to QEN_NOR structure in the
**          dsh_kor_nor array.  In qee_fetch(), initialize dsh_kor_nor
**          to null at create time.
**	23-aug-1993 (bryanp)
**	    Initialize the dmt_owner field in the DMT_CB. This allows the
**	    TEMP_TABLE_MACRO in qeq_cleanup to work properly. (B53993)
**      19-dec-94 (sarjo01) from 16-sep-94 (johna)
**          Initialize dmt->dmt_mustlock. Bug 58681
**      20-aug-95 (ramra01)
**          Modify the action header at runtime to call a new node ORIGAGG
**          if there is potential for performance gains with simple aggregates.
**      14-Feb-95 (ramra01)
**          In the case of the WHERE predicate with an OR condition
**          this will require DMF to reposition itself for the
**          next key. This is not possible thru the aggf optimization
**          code path. This will require to go thru the ORIG node. b74040
**	7-nov-1995 (inkdo01)
**	    Numerous changes to replace many QEN_ADF and some QEN_OJINFO and
**	    QEN_TEMP_TABLE structure instances by pointers, in a variety of
**	    QEN_NODE and QEF_AHD structures.
**      14-Feb-96 (ramra01)
**          In the case of the WHERE predicate with an OR condition
**          this will require DMF to reposition itself for the
**          next key. This is not possible thru the aggf optimization
**          code path. This will require to go thru the ORIG node. b74040
**      05-May-96 (ramra01)
**          In the case where an aggregate computation involves the tid
**          ex: max(tid), min(tid) do not use the aggf optim path as
**          the value is not set in the row buffer offset until return
**          from DMF to the QEF orig node. This is indicated by a non-neg
**          number in the orig_tid element specifying the offset to place
**          the value if the tid value is needed.
**	12-feb-97 (inkdo01)
**	    Change to support MS Access OR transforms.
**	27-oct-98 (inkdo01)
**	    Support for row producing procedures (QEA_FOR, QEA_RETROW).
**	3-mar-99 (inkdo01)
**	    Added support for hash joins (QE_HJOIN).
**	20-oct-99 (inkdo01)
**	    Add code to estimate qualified rows per KOR in ORIG nodes.
**	26-jan-01 (inkdo01)
**	    Added support for hash aggregation (QEA_HAGGF).
**	30-may-01 (inkdo01)
**	    Moved init of dmh_db_id, dmh_tran_id to qee_fetch to fix 104680.
**	19-jan-04 (inkdo01)
**	    Format DMT_CBs for partitions, too (for table partitioning).
**	6-may-04 (inkdo01)
**	    No need to preallocate partition DMT_CBs - removed some of
**	    19-jan-04 change.
**	11-may-04 (inkdo01)
**	    Save 2nd copy of master ptab DMR_CB in cbs array.
**	6-Jun-2005 (schka24)
**	    Filename is now part of DMH_CB (so that || kids don't share it).
**	6-Dec-2005 (kschendel)
**	    Don't need to allocate sort compare-lists any more.
**	17-Jun-2006 (kschendel)
**	    Init all CX's for multi-dimensional partition qualification.
**	29-jan-2007 (dougi)
**	    Allocate/format scrollable cursor structures.
**	29-june-2007 (dougi)
**	    Fix storage allocation for scrollable cursors.
**	2-oct-2007 (dougi)
**	    Stow sp_rowbuff addr in dsh_row array for KEYSET posn'd upd/del.
**	18-dec-2007 (dougi)
**	    Alloc/init QEA_FIRST structure for offset/first "n".
**	30-jan-2008 (dougi)
**	    Fix result row sizes of scroll files.
*/
DB_STATUS
qee_cract(
	QEF_RCB            *qef_rcb,
	QEE_DSH            *dsh,
	QEF_AHD		   *bgn_act,
	QEF_AHD		   **eobptr,
	i4		   eobcount,
	ULM_RCB            *ulm)
{
    GLOBALREF QEF_S_CB *Qef_s_cb;
    QEF_AHD	    *act;
    QEF_AHD	    *end_act;
    QEF_VALID	    *vl;
    DMT_CB	    *dmt_cb;
    DMR_CB	    *dmr_cb;
    DMH_CB	    *dmh_cb;
    EXCH_CB	    *exch_cb;
    QEF_KEY	    *qef_key;
    QEF_KOR	    *qef_kor;
    QEF_KAND	    *qef_kand;
    QEF_KATT	    *qef_katt;
    QEN_TKEY	    *tkptr;
    QEN_NOR	    *orkptr;
    QEN_NKEY	    *nkptr;
    DMR_ATTR_ENTRY  *attr;
    DMR_ATTR_ENTRY  **attr_array;
    QEN_NODE	    *qep_node;
    PTR		    key_ptr;
    DB_STATUS	    status = E_DB_OK;
    i4	    key_len;
    i4	    row_no;
    i4		    dmt_index;
    i4		    dmr_index;
    i4		    i, j;
    QEN_ADF	    *qen_adf[12];
    bool	    key_by_tid = FALSE;
    bool            key_by_position = FALSE;
    QEF_KEY         *qef_pkey;
    QEN_PART_INFO   *partp;

    if (bgn_act == (QEF_AHD *)NULL)
	return (E_DB_OK);

#ifdef xDEBUG
    (VOID) qe2_chk_qp(dsh);
#endif 


    /* Walk through the actions and create dmt_cb control blocks.
    ** This is a new interface between OPC and QEF.  Now all the user
    ** tables used in the query will be listed in the validation list
    ** of the first action header. The valid lists of the rest of the 
    ** actions only refer to the temporary tables:
    **
    ** Also, notice that the timestamp of a relation that does not
    ** require validation will be all zero.  This includes certain
    ** system catalog as well as temporary table */
    for (act = bgn_act; (qee_chkeob(act, eobptr, eobcount) == FALSE);
	 act = act->ahd_next)
    {
	for (vl = act->ahd_valid; vl; vl = vl->vl_next)
	{
	    i = vl->vl_dmf_cb;
	    if (i >= 0 && dsh->dsh_cbs[i] == 0)
	     for (j = 0; j <= 0 /* vl->vl_partition_cnt */; j++)
		/* temporarily disable loop - just alloc primary DMT_CB */
	    {
		ulm->ulm_psize = sizeof(DMT_CB);
		status = qec_malloc(ulm);
		if (status != E_DB_OK)
		{
		    qef_rcb->error.err_code = ulm->ulm_error.err_code;
		    return (status);
		}
		dsh->dsh_cbs[i+j] = (PTR) ulm->ulm_pptr;
		dmt_cb 	= (DMT_CB*) ((char *) ulm->ulm_pptr); 
		dmt_cb->q_next = (PTR)NULL;
		dmt_cb->q_prev = (PTR)NULL;
		dmt_cb->length 	= sizeof (DMT_CB);
		dmt_cb->type = DMT_TABLE_CB;
		dmt_cb->owner = (PTR)DB_QEF_ID;
		dmt_cb->ascii_id = DMT_ASCII_ID;
		dmt_cb->dmt_db_id = qef_rcb->qef_db_id;
		dmt_cb->dmt_record_access_id = (PTR)NULL;
		/* Copy table ID or (if partitioned not the master
		** table) partition ID. */
		STRUCT_ASSIGN_MACRO((j == 0) ? vl->vl_tab_id :
			vl->vl_part_tab_id[j-1], dmt_cb->dmt_id);
		dmt_cb->dmt_flags_mask = 0;
		MEfill(sizeof(DB_OWN_NAME), (u_char)' ',
			(PTR)&dmt_cb->dmt_owner);
		dmt_cb->dmt_char_array.data_address = 0;
		dmt_cb->dmt_char_array.data_in_size = 0;
                dmt_cb->dmt_mustlock = vl->vl_mustlock;
		if (act->ahd_flags & QEA_DIR)
		    dmt_cb->dmt_update_mode = DMT_U_DIRECT;
		else
		    dmt_cb->dmt_update_mode = DMT_U_DEFERRED;
		/* Allocate the location array. Allow for 1 location */
		ulm->ulm_psize = sizeof(DB_LOC_NAME);
		if (status = qec_malloc(ulm))
		{
		    qef_rcb->error.err_code = ulm->ulm_error.err_code;
		    return (status);
		}
		dmt_cb->dmt_location.data_address = ulm->ulm_pptr;
	    }
	}

	if (act->ahd_atype == QEA_IF)	    /* special processing for IF */
	{
	    *(eobptr+eobcount) = act;
	    *(eobptr+eobcount+1) = act->ahd_next;
	    status = qee_cract(qef_rcb, dsh, act->qhd_obj.qhd_if.ahd_true, 
			    eobptr, eobcount + 2, ulm);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    status = qee_cract(qef_rcb, dsh, act->qhd_obj.qhd_if.ahd_false, 
			    eobptr, eobcount + 2, ulm);
	    if (DB_FAILURE_MACRO(status))
		return (status);
	}
    }
    end_act = act;

    /* now walk through the actions and QEPs creating the dmr_cb's,
    ** qee_ade, and various key structures.
    */
    i = j = 0;
    dsh->dsh_error.err_code = E_DB_OK;
    for (act = bgn_act; act != end_act; act = act->ahd_next)
    {

	/* initialize the action header */
	switch (act->ahd_atype)
	{
	    case QEA_DMU:
		break;

	    case QEA_HAGGF:	    
		/* Get the hash aggregation overflow CX (HAGGF only)
		*/
		qen_adf[j++] = act->qhd_obj.qhd_qep.u1.s2.ahd_agoflow;
	    case QEA_APP:
	    case QEA_LOAD:
	    case QEA_SAGG:
	    case QEA_PROJ:
	    case QEA_RAGGF:	    
	    case QEA_AGGF:
		/* Get the by-list projection CX - it doesn't always
		** exist
		*/
		qen_adf[j++] = act->qhd_obj.qhd_qep.ahd_by_comp;
	    case QEA_UPD:
	    case QEA_PUPD:
	    case QEA_DEL:
	    case QEA_PDEL:
	    case QEA_GET:
	    case QEA_FOR:
	    case QEA_RETROW:
		/* initialize the ADE_CX control blocks */
		qen_adf[j++] = act->ahd_mkey;
		qen_adf[j++] = act->qhd_obj.qhd_qep.ahd_current;
		qen_adf[j++] = act->qhd_obj.qhd_qep.ahd_constant;
		if (act->qhd_obj.qhd_qep.ahd_qepflag & AHD_KEYSET)
		    qen_adf[j++] = act->qhd_obj.qhd_qep.ahd_scroll->
								ahd_rskcurr;
 
		/* initialize the QEP. */
		for (qep_node = act->qhd_obj.qhd_qep.ahd_postfix; 
		    qep_node; qep_node = qep_node->qen_postfix)
		{
		    QEN_NOR	**lastnor;
		    QEN_NKEY	**lastnkey;
		    i4     val_1, val_2;
		    i4		orcount, dios_per_or = -1;
                    QEF_CB	*qef_cb;

		    qef_cb = qef_rcb->qef_cb; 
		    /* create the ADE_EXCB to print tuples for debugging */
		    qen_adf[j++] = qep_node->qen_prow;
		    /* create the ADE_EXCB for function attributes */
		    if (qep_node->qen_fatts)
			qen_adf[j++] = qep_node->qen_fatts;
		    key_len	= 0;
		    key_ptr	= 0;
		    dmr_index	= -1;
		    dmt_index	= -1;

                    qep_node->qen_func
                       = Qef_s_cb->qef_func[qep_node->qen_type].func_name;

		    if (act->ahd_atype == QEA_SAGG && 
		        qep_node->qen_type == QE_ORIG && 
                        !(ult_check_macro(& qef_cb->qef_trace, 
                                         QEF_TRACE_NO_AGGF_OPTIM,
                                         & val_1, & val_2)) &&
                        qep_node == act->qhd_obj.qhd_qep.ahd_qep &&
			!(act->qhd_obj.qhd_qep.ahd_qepflag & AHD_ANY))
                    {
                        qef_pkey = qep_node->node_qen.qen_orig.orig_pkeys;

                        if (qef_pkey != NULL && qef_pkey->key_kor != NULL &&
                            qef_pkey->key_kor->kor_keys != NULL &&
                            qef_pkey->key_kor->kor_keys->kand_attno == 0)
                        {
                            key_by_tid = TRUE;
                        }
                        else
                        if (qef_pkey != NULL && qef_pkey->key_kor != NULL &&
                            qef_pkey->key_kor->kor_next != NULL)
                        {
                            key_by_position = TRUE;
                        }

                        if ((qep_node->node_qen.qen_orig.orig_flag & ORIG_UKEY)
			      || qep_node->node_qen.qen_orig.orig_part
                              || (key_by_tid == TRUE)
                              || (key_by_position == TRUE
                              || qep_node->node_qen.qen_orig.orig_tid >= 0 ))
                        {
                            /*  no performance gain in aggregates */ 
                            /*  The check option sets a select any() on tid */
                            /*  Aggr computation involves tid like max(tid) */
                        }
                        else
                        { 
			    qep_node->qen_func 
			        = Qef_s_cb->qef_func[QE_ORIGAGG].func_name;
                            qep_node->qen_type = QE_ORIGAGG;
                        }
                    }

		    switch (qep_node->qen_type)
		    {
			case QE_TJOIN:
			    /* create the ADE_EXCB to qualify tuples */
			    qen_adf[j++] =
			    	qep_node->node_qen.qen_tjoin.tjoin_jqual;
			    qen_adf[j++] = 
				(qep_node->node_qen.qen_tjoin.tjoin_oj) ?
				qep_node->node_qen.qen_tjoin.tjoin_oj->oj_oqual :
				(QEN_ADF *)NULL;
			    qen_adf[j++] = 
				(qep_node->node_qen.qen_tjoin.tjoin_oj) ?
				qep_node->node_qen.qen_tjoin.tjoin_oj->oj_equal :
				(QEN_ADF *)NULL;
			    qen_adf[j++] = 
				(qep_node->node_qen.qen_tjoin.tjoin_oj) ?
				qep_node->node_qen.qen_tjoin.tjoin_oj->oj_rnull :
				(QEN_ADF *)NULL;
			    qen_adf[j++] = 
				(qep_node->node_qen.qen_tjoin.tjoin_oj) ?
				qep_node->node_qen.qen_tjoin.tjoin_oj->oj_lnull :
				(QEN_ADF *)NULL;
			    qen_adf[j++] = 
				qep_node->node_qen.qen_tjoin.tjoin_isnull;
			    dmr_index = qep_node->node_qen.qen_tjoin.tjoin_get;
			    break;

			case QE_KJOIN:
			    /* create the ADE_EXCB to qualify tuples */
			    qen_adf[j++] = 
				qep_node->node_qen.qen_kjoin.kjoin_kqual;
			    qen_adf[j++] = 
				qep_node->node_qen.qen_kjoin.kjoin_jqual;
			    qen_adf[j++] = 
				qep_node->node_qen.qen_kjoin.kjoin_iqual;
			    qen_adf[j++] = 
				qep_node->node_qen.qen_kjoin.kjoin_kcompare;
			    qen_adf[j++] = 
				qep_node->node_qen.qen_kjoin.kjoin_key;
			    qen_adf[j++] = 
				(qep_node->node_qen.qen_kjoin.kjoin_oj) ?
				qep_node->node_qen.qen_kjoin.kjoin_oj->oj_oqual :
				(QEN_ADF *)NULL;
			    qen_adf[j++] = 
				(qep_node->node_qen.qen_kjoin.kjoin_oj) ?
				qep_node->node_qen.qen_kjoin.kjoin_oj->oj_equal :
				(QEN_ADF *)NULL;
			    qen_adf[j++] = 
				(qep_node->node_qen.qen_kjoin.kjoin_oj) ?
				qep_node->node_qen.qen_kjoin.kjoin_oj->oj_rnull :
				(QEN_ADF *)NULL;
			    qen_adf[j++] = 
				(qep_node->node_qen.qen_kjoin.kjoin_oj) ?
				qep_node->node_qen.qen_kjoin.kjoin_oj->oj_lnull :
				(QEN_ADF *)NULL;




			    dmr_index = qep_node->node_qen.qen_kjoin.kjoin_get;
			    qef_key = qep_node->node_qen.qen_kjoin.kjoin_kys;
			    if (qef_key == 0)
				break;
			    key_len  = qef_key->key_width;
			    /* allocate the key array */
			    ulm->ulm_psize = key_len * sizeof (DMR_ATTR_ENTRY*);
			    if (status = qec_malloc(ulm))
			    {
				qef_rcb->error.err_code = 
					ulm->ulm_error.err_code;
				return (status);
			    }
			    key_ptr = ulm->ulm_pptr;
			    /* fill in the key array */
			    attr_array = (DMR_ATTR_ENTRY **) key_ptr;
			    /* use the top of the key list, the first element */
			    for 
			    (
				qef_kand = qef_key->key_kor->kor_keys, i = 0;
				qef_kand; 
				qef_kand = qef_kand->kand_next, i++
			    )
			    {
				qef_katt	      = qef_kand->kand_keys;
				ulm->ulm_psize = sizeof (DMR_ATTR_ENTRY);
				if (status = qec_malloc(ulm))
				{
				    qef_rcb->error.err_code = 
					    ulm->ulm_error.err_code;
				    return (status);
				}
				attr_array[i] = attr = 
					(DMR_ATTR_ENTRY *) ulm->ulm_pptr;
				attr->attr_number   = qef_katt->attr_attno;
				attr->attr_operator = qef_katt->attr_operator;
			        row_no = qep_node->
				    node_qen.qen_kjoin.kjoin_key->qen_output;
				attr->attr_value = 
				    (char *) dsh->dsh_row[row_no] + 
				    qef_katt->attr_value;
			    }

			    break;

			case QE_ORIGAGG:
			case QE_ORIG:
			    qen_adf[j++] = 
				qep_node->node_qen.qen_orig.orig_qual;
			    partp = qep_node->node_qen.qen_orig.orig_part;
			    if (partp != NULL && partp->part_qual.part_qdim != NULL)
			    {
				QEN_PART_QUAL *pqual = &partp->part_qual;
				do
				{
				    if (status = qee_ade(qef_rcb, dsh, pqual->part_qpart))
					return (status);
				    pqual = pqual->part_qnext;
				} while (pqual != NULL);
			    }
			    if (!(qep_node->node_qen.qen_orig.orig_flag
				& ORIG_MCNSTTAB))
			     dmr_index = qep_node->node_qen.qen_orig.orig_get;
			    qef_key = qep_node->node_qen.qen_orig.orig_pkeys;
			    if (qef_key == 0)
				break;
			    /* allocate key storage structures */
			    /* first allocate the list header QEN_TKEY */
			    ulm->ulm_psize = sizeof (QEN_TKEY);
			    key_len  = qef_key->key_width;
			    if (status = qec_malloc(ulm))
			    {
				qef_rcb->error.err_code = 
						    ulm->ulm_error.err_code;
				return (status);
			    }
			    tkptr = (QEN_TKEY *) ulm->ulm_pptr;
			    i = qep_node->node_qen.qen_orig.orig_keys;
			    dsh->dsh_cbs[i] = (PTR) tkptr;
			    tkptr->qen_nkey = (QEN_NOR *)NULL;
			    tkptr->qen_keys = (QEN_NOR *)NULL;
			    lastnor = (QEN_NOR **) &tkptr->qen_keys;

			    /* allocate QEN_NOR for QEF_KORs */
			    for (qef_kor = qef_key->key_kor, orcount = 0; qef_kor;
				 qef_kor = qef_kor->kor_next, orcount++)
			    {
				ulm->ulm_psize = sizeof (QEN_NOR);
				if (status = qec_malloc(ulm))
				{
				    qef_rcb->error.err_code 
						= ulm->ulm_error.err_code;
				    return (status);
				}
				orkptr = (QEN_NOR *) ulm->ulm_pptr;
				orkptr->qen_next = (QEN_NOR *)NULL;
				orkptr->qen_key	= (QEN_NKEY *)NULL;
                                /*
                                ** bug 45912; save info on which qef_kor
                                ** this qen_nor is associated with
                                */
                                orkptr->qef_kor = qef_kor;
                                dsh->dsh_kor_nor[qef_kor->kor_id] = orkptr;		
				/* append to the list in QEN_TKEY */
				*lastnor = orkptr;
				lastnor = (QEN_NOR **) &orkptr->qen_next;
    				lastnkey = (QEN_NKEY **)&orkptr->qen_key;

				/* allocate QEN_NKEY for QEF_KAND */
				for (qef_kand = qef_kor->kor_keys; qef_kand;
				     qef_kand = qef_kand->kand_next)
				{
				    ulm->ulm_psize = sizeof (QEN_NKEY);
				    if (status = qec_malloc(ulm))
				    {
					qef_rcb->error.err_code 
						= ulm->ulm_error.err_code;
					return (status);
				    }
				    nkptr = (QEN_NKEY *) ulm->ulm_pptr;
				    nkptr->nkey_att = (QEF_KATT *)NULL;
				    nkptr->nkey_next = (QEN_NKEY *)NULL;

				    /* append it to the list in QEN_NOR */
				    *lastnkey = nkptr;
				    lastnkey = (QEN_NKEY **) &nkptr->nkey_next;
				}
			    }
			    if (orcount && qep_node->node_qen.qen_orig.orig_qualrows > 0)
				dios_per_or = qep_node->node_qen.qen_orig.orig_qualrows /
									orcount;
			    /* allocate the key array */
			    ulm->ulm_psize = key_len * sizeof (DMR_ATTR_ENTRY*);
			    if (status = qec_malloc(ulm))
			    {
				qef_rcb->error.err_code = 
					ulm->ulm_error.err_code;
				return (status);
			    }
			    key_ptr = ulm->ulm_pptr;
			    /* fill in the key array */
			    attr_array = (DMR_ATTR_ENTRY **) key_ptr;
			    /* allocate the dmr_attr_entry elements */
			    for (i = 0; i < key_len; i++)
			    {
				ulm->ulm_psize = sizeof (DMR_ATTR_ENTRY);
				if (status = qec_malloc(ulm))
				{
				    qef_rcb->error.err_code = 
					    ulm->ulm_error.err_code;
				    return (status);
				}
				attr_array[i] = 
					(DMR_ATTR_ENTRY *) ulm->ulm_pptr;
			    }
			    break;

			case QE_EXCHANGE:
			    qen_adf[j++] =
				qep_node->node_qen.qen_exch.exch_mat;
			    ulm->ulm_psize = sizeof (EXCH_CB);
			    if (status = qec_malloc(ulm))
			    {
				qef_rcb->error.err_code = 
						    ulm->ulm_error.err_code;
				return (status);
			    }
			    i = qep_node->node_qen.qen_exch.exch_exchcb;
			    dsh->dsh_cbs[i] = ulm->ulm_pptr;
			    MEfill( (ulm->ulm_psize), (u_char) 0,
				(PTR)ulm->ulm_pptr);
			    exch_cb = (EXCH_CB *)ulm->ulm_pptr;
			    exch_cb->ascii_id = EXCH_ASCII_ID;
			    break;

			case QE_FSMJOIN:
                        case QE_ISJOIN:
                        case QE_CPJOIN:  
			    qen_adf[j++] = 
				qep_node->node_qen.qen_sjoin.sjn_joinkey;
			    qen_adf[j++] = 
				qep_node->node_qen.qen_sjoin.sjn_okcompare;
			    qen_adf[j++] =
				qep_node->node_qen.qen_sjoin.sjn_okmat;
			    qen_adf[j++] =
				qep_node->node_qen.qen_sjoin.sjn_itmat;
			    qen_adf[j++] =
				qep_node->node_qen.qen_sjoin.sjn_jqual;
			    qen_adf[j++] = 
				(qep_node->node_qen.qen_sjoin.sjn_oj) ?
				qep_node->node_qen.qen_sjoin.sjn_oj->oj_oqual :
				(QEN_ADF *)NULL;
			    qen_adf[j++] = 
				(qep_node->node_qen.qen_sjoin.sjn_oj) ?
				qep_node->node_qen.qen_sjoin.sjn_oj->oj_equal :
				(QEN_ADF *)NULL;
			    qen_adf[j++] = 
				(qep_node->node_qen.qen_sjoin.sjn_oj) ?
				qep_node->node_qen.qen_sjoin.sjn_oj->oj_rnull :
				(QEN_ADF *)NULL;
			    qen_adf[j++] = 
				(qep_node->node_qen.qen_sjoin.sjn_oj) ?
				qep_node->node_qen.qen_sjoin.sjn_oj->oj_lnull :
				(QEN_ADF *)NULL;
			    /* allocate the hold file structures */
			    if (status = qee_hfile(qef_rcb, dsh, qep_node, ulm))
				return (status);
			    /* allocate temporary table structures */
			    if (status = qee_joinTempTables(qef_rcb, dsh, 
				    qep_node, ulm))
				return (status);
			    break;

                        case QE_HJOIN:  
			    qen_adf[j++] =
				qep_node->node_qen.qen_hjoin.hjn_btmat;
			    qen_adf[j++] =
				qep_node->node_qen.qen_hjoin.hjn_ptmat;
			    qen_adf[j++] =
				qep_node->node_qen.qen_hjoin.hjn_jqual;
			    qen_adf[j++] = 
				(qep_node->node_qen.qen_hjoin.hjn_oj) ?
				qep_node->node_qen.qen_hjoin.hjn_oj->oj_oqual :
				(QEN_ADF *)NULL;
			    qen_adf[j++] = 
				(qep_node->node_qen.qen_hjoin.hjn_oj) ?
				qep_node->node_qen.qen_hjoin.hjn_oj->oj_equal :
				(QEN_ADF *)NULL;
			    qen_adf[j++] = 
				(qep_node->node_qen.qen_hjoin.hjn_oj) ?
				qep_node->node_qen.qen_hjoin.hjn_oj->oj_rnull :
				(QEN_ADF *)NULL;
			    qen_adf[j++] = 
				(qep_node->node_qen.qen_hjoin.hjn_oj) ?
				qep_node->node_qen.qen_hjoin.hjn_oj->oj_lnull :
				(QEN_ADF *)NULL;
			    /* Allocate/init hash partition file control block. */
			    ulm->ulm_psize = sizeof (DMH_CB);
			    if (status = qec_malloc(ulm))
			    {
				qef_rcb->error.err_code = 
						    ulm->ulm_error.err_code;
				return (status);
			    }
			    i = qep_node->node_qen.qen_hjoin.hjn_dmhcb;
			    dsh->dsh_cbs[i] = ulm->ulm_pptr;
			    dmh_cb = (DMH_CB *)ulm->ulm_pptr;
			    dmh_cb->q_next = (PTR)NULL;
			    dmh_cb->q_prev = (PTR)NULL;
			    dmh_cb->length = sizeof (DMH_CB);
			    dmh_cb->type = (i2)DMH_PFILE_CB;
			    dmh_cb->owner = (PTR)DB_QEF_ID;
			    dmh_cb->ascii_id = DMH_ASCII_ID;
			    if (status = qee_hashInit(qef_rcb, dsh, qep_node,
					ulm))
				return(status);
			    break;

			case QE_SEJOIN:
			    qen_adf[j++] = 
				qep_node->node_qen.qen_sejoin.sejn_oqual;
			    qen_adf[j++] = 
				qep_node->node_qen.qen_sejoin.sejn_kqual;
			    qen_adf[j++] =
				qep_node->node_qen.qen_sejoin.sejn_okmat;
			    qen_adf[j++] =
				qep_node->node_qen.qen_sejoin.sejn_itmat;
			    qen_adf[j++] =
				qep_node->node_qen.qen_sejoin.sejn_kcompare;
			    qen_adf[j++] =
				qep_node->node_qen.qen_sejoin.sejn_ccompare;
			    /* readjust index if no correlation */
			    if (qen_adf[j-1] == NULL)
				--j;
			    /* allocate the hold file structures */
			    if (status = qee_hfile(qef_rcb, dsh, qep_node, ulm))
				return (status);
			    break;

			case QE_SORT:
			    qen_adf[j++] =
				qep_node->node_qen.qen_sort.sort_mat;
			    dmr_index = qep_node->node_qen.qen_sort.sort_load;
			    dmt_index = qep_node->node_qen.qen_sort.sort_create;
			    break;

			case QE_TSORT:
			    qen_adf[j++] =
				qep_node->node_qen.qen_tsort.tsort_mat;

			    /* Allocate the LOAD DMR_CB for the sorter */
			    ulm->ulm_psize = sizeof(DMR_CB);
			    if (status = qec_malloc(ulm))
			    {
				qef_rcb->error.err_code =
						ulm->ulm_error.err_code;
				return (status);
			    }
			    /* Initialize the LOAD DMR_CB */
			    dmr_index = qep_node->node_qen.qen_tsort.tsort_load;
			    dsh->dsh_cbs[dmr_index] = (PTR) ulm->ulm_pptr;
			    dmr_cb = (DMR_CB *) ulm->ulm_pptr;
			    dmr_cb->q_next = (PTR)NULL;
			    dmr_cb->q_prev = (PTR)NULL;
			    dmr_cb->length = sizeof (DMR_CB);
			    dmr_cb->type = (i2)DMR_RECORD_CB;
			    dmr_cb->owner = (PTR)DB_QEF_ID;
			    dmr_cb->ascii_id = DMR_ASCII_ID;
			    dmr_cb->dmr_access_id = (PTR)NULL;
			    dmr_cb->dmr_attr_desc.ptr_in_count  = 0;
			    dmr_cb->dmr_attr_desc.ptr_address   = 0;
			    dmr_cb->dmr_attr_desc.ptr_size	= 0;

			    /* clear the qualification function */	
			    dmr_cb->dmr_q_fcn = 0;
			    dmr_cb->dmr_q_arg = 0;

			    /* Let common code init the GET DMR_CB */
			    dmr_index = qep_node->node_qen.qen_tsort.tsort_get;
			    dmt_index = qep_node->
					    node_qen.qen_tsort.tsort_create;
			    break;

			case QE_QP:
			    qen_adf[j++] = qep_node->node_qen.qen_qp.qp_qual;
			    if (status = qee_cract(qef_rcb, dsh, 
				    qep_node->node_qen.qen_qp.qp_act,
				    eobptr, eobcount, ulm))
				return (status);
			    break;
		    }

		    if (dmr_index >= 0)
		    {
			/* Allocate the DMR_CB */
			ulm->ulm_psize = sizeof(DMR_CB);
		        if (status = qec_malloc(ulm))
		        {
			    qef_rcb->error.err_code = ulm->ulm_error.err_code;
			    return (status);
			}
			/* Initialize the DMR_CB */
			dsh->dsh_cbs[dmr_index] = (PTR) ulm->ulm_pptr;
			if ((qep_node->qen_type == QE_ORIG &&
			    qep_node->node_qen.qen_orig.orig_part) ||
			    (qep_node->qen_type == QE_TJOIN &&
			    qep_node->node_qen.qen_tjoin.tjoin_part) ||
			    (qep_node->qen_type == QE_KJOIN &&
			    qep_node->node_qen.qen_kjoin.kjoin_part))
			  dsh->dsh_cbs[dmr_index-1] = (PTR)ulm->ulm_pptr;
				/* save a 2nd copy for ptab master */
			dmr_cb = (DMR_CB *) ulm->ulm_pptr;
			dmr_cb->q_next = (PTR)NULL;
			dmr_cb->q_prev = (PTR)NULL;
			dmr_cb->length = sizeof (DMR_CB);
			dmr_cb->type = (i2)DMR_RECORD_CB;
			dmr_cb->owner = (PTR)DB_QEF_ID;
			dmr_cb->ascii_id = DMR_ASCII_ID;
			dmr_cb->dmr_access_id = (PTR)NULL;

			/* clear the qualification function */	
			dmr_cb->dmr_q_fcn = 0;
			dmr_cb->dmr_q_arg = 0;

			/* allocate the key array for the DMR_CB */
			dmr_cb->dmr_attr_desc.ptr_in_count  = key_len;
			dmr_cb->dmr_attr_desc.ptr_address   = key_ptr;
			dmr_cb->dmr_attr_desc.ptr_size	    =
						     sizeof (DMR_ATTR_ENTRY);
			
			/* DIO per disjunct (for readahead determination). */
			dmr_cb->dmr_s_estimated_records = dios_per_or;

			/* assign the output row and its size */
			dmr_cb->dmr_data.data_address = 
			    dsh->dsh_row[qep_node->qen_row];
			dmr_cb->dmr_data.data_in_size = qep_node->qen_rsz;
			/* mark table not accessed */
		    }		
		    if (dmt_index >= 0 && dsh->dsh_cbs[dmt_index] == 0)
		    {
			/* allocate the DMT_CB */
			ulm->ulm_psize = sizeof(DMT_CB);
			if (status = qec_malloc(ulm))
			{
			    qef_rcb->error.err_code = ulm->ulm_error.err_code;
			    return (status);
			}
			dsh->dsh_cbs[dmt_index]	= ulm->ulm_pptr;
			dmt_cb = (DMT_CB *) (ulm->ulm_pptr);
			dmt_cb->q_next = (PTR)NULL;
			dmt_cb->q_prev = (PTR)NULL;
			dmt_cb->length = sizeof (DMT_CB);
			dmt_cb->type = DMT_TABLE_CB;
			dmt_cb->owner = (PTR)DB_QEF_ID;
			dmt_cb->ascii_id = DMT_ASCII_ID;
			dmt_cb->dmt_db_id = qef_rcb->qef_db_id;
			dmt_cb->dmt_record_access_id = (PTR)NULL;
			dmt_cb->dmt_flags_mask = 0;

                        if (act->ahd_atype == QEA_GET)
                                dmt_cb->dmt_mustlock = FALSE;
                        else
                                dmt_cb->dmt_mustlock = TRUE;

			dmt_cb->dmt_char_array.data_address = 0;
			dmt_cb->dmt_char_array.data_in_size = 0;
			/* Allocate the location array. Allow for 1 location */
			ulm->ulm_psize = sizeof(DB_LOC_NAME);
			if (status = qec_malloc(ulm))
			{
			    qef_rcb->error.err_code = ulm->ulm_error.err_code;
			    return (status);
			}
			dmt_cb->dmt_location.data_address = ulm->ulm_pptr;
		    }
		    /* allocate the qee_ade structs (to flush our local
		    ** tracking array so it doesn't overflow)
		    */
		    for (;j > 0;)
		    {
			if (status = qee_ade(qef_rcb, dsh, qen_adf[--j]))
			    return (status);
		    }
		} /* end postfix node loop */

		/* allocate qee_ade structs if no nodes */
		for (;j > 0;)
		{
		    if (status = qee_ade(qef_rcb, dsh, qen_adf[--j]))
			return (status);
		}
		/* allocate the output control block if it wasn't allocated
		** as an input CB in the QEP and if it is needed.
		*/
		if ((i = act->qhd_obj.qhd_qep.ahd_odmr_cb) >= 0 &&
		     dsh->dsh_cbs[i] == 0)
		{
		    /* allocate the DMR_CB */
		    ulm->ulm_psize = sizeof(DMR_CB);
		    if (status = qec_malloc(ulm))
		    {
			qef_rcb->error.err_code = ulm->ulm_error.err_code;
			return (status);
		    }
		    dsh->dsh_cbs[i] = (PTR) ulm->ulm_pptr;
		    dmr_cb = (DMR_CB *) ulm->ulm_pptr;
		    dmr_cb->q_next = (PTR)NULL;
		    dmr_cb->q_prev = (PTR)NULL;
		    dmr_cb->length = sizeof (DMR_CB);
		    dmr_cb->type = (i2)DMR_RECORD_CB;
		    dmr_cb->owner = (PTR)DB_QEF_ID;
		    dmr_cb->ascii_id = DMR_ASCII_ID;
		    dmr_cb->dmr_access_id = (PTR)NULL;

		    /* clear the qualification function */	
		    dmr_cb->dmr_q_fcn = 0;
		    dmr_cb->dmr_q_arg = 0;

		    /* assign the output row and its size */
		    /* the output row number is */
                    if(act->ahd_atype == QEA_GET)
		       i = act->qhd_obj.qhd_qep.ahd_ruprow;
                    else
                       i = (act->qhd_obj.qhd_qep.ahd_current) ?
                         	act->qhd_obj.qhd_qep.ahd_current->qen_output : -1;
		    dmr_cb->dmr_data.data_address = dsh->dsh_row[i];
		    dmr_cb->dmr_data.data_in_size = 
			dsh->dsh_qp_ptr->qp_row_len[i];
		}

		/* If this is hash aggregate, alloc/init the hash aggregate
		** partition file control block and init the hash aggregation 
		** control block. */
		if (act->ahd_atype == QEA_HAGGF)
		{
		    ulm->ulm_psize = sizeof (DMH_CB);
		    if (status = qec_malloc(ulm))
		    {
			qef_rcb->error.err_code = 
					    ulm->ulm_error.err_code;
			return (status);
		    }
		    i = act->qhd_obj.qhd_qep.u1.s2.ahd_agdmhcb;
		    dsh->dsh_cbs[i] = ulm->ulm_pptr;
		    dmh_cb = (DMH_CB *)ulm->ulm_pptr;
		    dmh_cb->q_next = (PTR)NULL;
		    dmh_cb->q_prev = (PTR)NULL;
		    dmh_cb->length = sizeof (DMH_CB);
		    dmh_cb->type = (i2)DMH_PFILE_CB;
		    dmh_cb->owner = (PTR)DB_QEF_ID;
		    dmh_cb->ascii_id = DMH_ASCII_ID;
		    status = qee_hashaggInit(qef_rcb, dsh, 
					&act->qhd_obj.qhd_qep, ulm);
		    if (status != E_DB_OK) return(status);
		}
		    
		/*
		** Initialize failed update temptable if there is a statement
		** level unique index associated with this action.
		*/
		if (act->ahd_flags & QEF_STATEMENT_UNIQUE)
		{
		    status = qee_buildTempTable(qef_rcb, dsh, ulm,
			    act->qhd_obj.qhd_qep.u1.s1.ahd_failed_a_temptable);
		    if (DB_FAILURE_MACRO(status))
			return (status);
		    if (act->qhd_obj.qhd_qep.u1.s1.ahd_b_row >= 0)
		    {
			status = qee_buildTempTable(qef_rcb, dsh, ulm,
				act->qhd_obj.qhd_qep.u1.s1.ahd_failed_b_temptable);
			if (DB_FAILURE_MACRO(status))
			    return (status);
		    }
		}
		/* Initialize any after rule action list for this QEP_AHD 
		** node. A rule action list will exist if this is an update,
		** insert or delete QEP_AHD and there are after rules that may
		** apply to this update. */
		if (act->qhd_obj.qhd_qep.ahd_after_act != NULL)
		{
		    status = qee_cract(qef_rcb, dsh,
				act->qhd_obj.qhd_qep.ahd_after_act,
				eobptr, eobcount, ulm);
		    if (DB_FAILURE_MACRO(status))
			return (status);
		}

		/* Initialize the before rules, too. */
		if (act->qhd_obj.qhd_qep.ahd_before_act != NULL)
		{
		    status = qee_cract(qef_rcb, dsh,
				act->qhd_obj.qhd_qep.ahd_before_act,
				eobptr, eobcount, ulm);
		    if (DB_FAILURE_MACRO(status))
			return (status);
		}

		/* If this is a "first "n"/offset "n" action header, 
		** allocate/init the QEA_FIRSTN structure. */
		if (act->qhd_obj.qhd_qep.ahd_firstncb >= 0)
		{
		    ulm->ulm_psize = DB_ALIGN_MACRO(sizeof(QEA_FIRSTN));
		    if (status = qec_malloc(ulm))
		    {
			qef_rcb->error.err_code = ulm->ulm_error.err_code;
			return (status);
		    }

		    MEfill(ulm->ulm_psize, (u_char)0, ulm->ulm_pptr);
		    dsh->dsh_cbs[act->qhd_obj.qhd_qep.ahd_firstncb] =
							ulm->ulm_pptr;
		}

		/* If this is a scrollable cursor, allocate and format the
		** QEE_RSET_SPOOL structure. */
		if (act->ahd_atype == QEA_GET && 
			(act->qhd_obj.qhd_qep.ahd_qepflag & AHD_SCROLL))
		{
		    QEE_RSET_SPOOL *sptr;
		    PTR		Mem;

		    /* Add it up and allocate once. */
		    i = DB_ALIGN_MACRO(sizeof(QEE_RSET_SPOOL));
		    i += DB_ALIGN_MACRO(2 * sizeof(DMR_CB));
		    i += DB_ALIGN_MACRO(sizeof(DMT_CB));
		    i += DB_ALIGN_MACRO(sizeof(DB_LOC_NAME));
		    if (act->qhd_obj.qhd_qep.ahd_qepflag & AHD_KEYSET)
			i += (DB_ALIGN_MACRO(act->qhd_obj.qhd_qep.
					ahd_scroll->ahd_rsrsize) +
					DB_ALIGN_MACRO(sizeof(DMR_CB)));

		    ulm->ulm_psize = i;
		    if (status = qec_malloc(ulm))
		    {
			qef_rcb->error.err_code = ulm->ulm_error.err_code;
			return (status);
		    }

		    MEfill(i, (u_char)0, ulm->ulm_pptr);
		    sptr = (QEE_RSET_SPOOL *) ulm->ulm_pptr;
		    Mem = (PTR) sptr;
		    dsh->dsh_cbs[dsh->dsh_qp_ptr->qp_rssplix] = (PTR)sptr;

		    Mem += DB_ALIGN_MACRO(sizeof(QEE_RSET_SPOOL));
		    sptr->sp_wdmrcb = (DMR_CB *)Mem;
		    Mem += DB_ALIGN_MACRO(sizeof(DMR_CB));
		    sptr->sp_rdmrcb = (DMR_CB *)Mem;
		    Mem += DB_ALIGN_MACRO(sizeof(DMR_CB));
		    if (act->qhd_obj.qhd_qep.ahd_qepflag & AHD_KEYSET)
		    {
			dsh->dsh_cbs[act->qhd_obj.qhd_qep.ahd_scroll->
							ahd_rsbtdmrix] = Mem;
			Mem += DB_ALIGN_MACRO(sizeof(DMR_CB));
		    }
		    sptr->sp_dmtcb = (DMT_CB *)Mem;
		    Mem += DB_ALIGN_MACRO(sizeof(DMT_CB));

		    /* Init QEE_RSET_SPOOL. */
		    sptr->sp_rowcount = 0;
		    sptr->sp_current = 0;
		    sptr->sp_curpage = -1;
		    if (act->qhd_obj.qhd_qep.ahd_qepflag & AHD_KEYSET)
		    {
			sptr->sp_rowbuff = Mem + 
				DB_ALIGN_MACRO(sizeof(DB_LOC_NAME));
		    }
		    else sptr->sp_rowbuff = (PTR) NULL;
		    dsh->dsh_row[act->qhd_obj.qhd_qep.ahd_scroll->ahd_rsrrow] =
					sptr->sp_rowbuff;
		    sptr->sp_flags = SP_BEGIN;	/* position before 1st row */

		    /* Set up write DMR_CB. */
		    dsh->dsh_cbs[act->qhd_obj.qhd_qep.ahd_scroll->ahd_rswdmrix] = 
				(PTR)sptr->sp_wdmrcb;
		    dmr_cb = sptr->sp_wdmrcb;
		    dmr_cb->q_next = (PTR)NULL;
		    dmr_cb->q_prev = (PTR)NULL;
		    dmr_cb->length = sizeof (DMR_CB);
		    dmr_cb->type = (i2)DMR_RECORD_CB;
		    dmr_cb->owner = (PTR)DB_QEF_ID;
		    dmr_cb->ascii_id = DMR_ASCII_ID;
		    dmr_cb->dmr_access_id = (PTR)NULL;

		    /* clear the qualification function */	
		    dmr_cb->dmr_q_fcn = 0;
		    dmr_cb->dmr_q_arg = 0;

		    /* assign the output row and its size */
		    /* the output row number is */
                    i = (act->qhd_obj.qhd_qep.ahd_current) ?
                         	act->qhd_obj.qhd_qep.ahd_current->qen_output : -1;
		    dmr_cb->dmr_data.data_address = dsh->dsh_row[i];
		    dmr_cb->dmr_data.data_in_size = 
				act->qhd_obj.qhd_qep.ahd_scroll->ahd_rsrsize;

		    /* Set up read DMR_CB. */
		    dsh->dsh_cbs[act->qhd_obj.qhd_qep.ahd_scroll->ahd_rsrdmrix] = 
				(PTR)sptr->sp_rdmrcb;
		    dmr_cb = sptr->sp_rdmrcb;
		    dmr_cb->q_next = (PTR)NULL;
		    dmr_cb->q_prev = (PTR)NULL;
		    dmr_cb->length = sizeof (DMR_CB);
		    dmr_cb->type = (i2)DMR_RECORD_CB;
		    dmr_cb->owner = (PTR)DB_QEF_ID;
		    dmr_cb->ascii_id = DMR_ASCII_ID;
		    dmr_cb->dmr_access_id = (PTR)NULL;

		    /* clear the qualification function */	
		    dmr_cb->dmr_q_fcn = 0;
		    dmr_cb->dmr_q_arg = 0;

		    /* assign the output row and its size */
		    /* the output row number is */
                    i = (act->qhd_obj.qhd_qep.ahd_current) ?
                         	act->qhd_obj.qhd_qep.ahd_current->qen_output : -1;
		    dmr_cb->dmr_data.data_address = dsh->dsh_row[i];
		    dmr_cb->dmr_data.data_in_size = 
				act->qhd_obj.qhd_qep.ahd_scroll->ahd_rsrsize;

		    /* Set up DMT_CB. */
		    dsh->dsh_cbs[act->qhd_obj.qhd_qep.ahd_scroll->ahd_rsdmtix] = 
				(PTR)sptr->sp_dmtcb;
		    dmt_cb = sptr->sp_dmtcb;
		    dmt_cb->q_next = (PTR)NULL;
		    dmt_cb->q_prev = (PTR)NULL;
		    dmt_cb->length = sizeof (DMT_CB);
		    dmt_cb->type = DMT_TABLE_CB;
		    dmt_cb->owner = (PTR)DB_QEF_ID;
		    dmt_cb->ascii_id = DMT_ASCII_ID;
		    dmt_cb->dmt_db_id = qef_rcb->qef_db_id;
		    dmt_cb->dmt_record_access_id = (PTR)NULL;
		    dmt_cb->dmt_flags_mask = 0;
                    dmt_cb->dmt_mustlock = FALSE;
		    dmt_cb->dmt_char_array.data_address = 0;
		    dmt_cb->dmt_char_array.data_in_size = 0;
		    dmt_cb->dmt_location.data_address = Mem;
		}

		/* Resolve execution-time addresses of ADE_EXCB's for this
		** query plan fragment into our XADDRS structure.
		** Don't resolve below any QP nodes, since the recursive
		** call to cract already did that.
		*/
		if (act->qhd_obj.qhd_qep.ahd_qep != NULL)
		    qee_resolve_xaddrs(dsh, act->qhd_obj.qhd_qep.ahd_qep, FALSE);

		break;

    	    case QEA_RUP:
		/* create the ADE struct here. This DSH will be attached
		** to the associated DSH in QEE_FETCH. The addresses
		** for the ADE_CX struct will be filled in in QEQ_VALIDATE
		*/
		status = qee_ade(qef_rcb, dsh, 
			act->qhd_obj.qhd_qep.ahd_current);
		status = qee_ade(qef_rcb, dsh, 
			act->qhd_obj.qhd_qep.ahd_constant);
		/* Initialize the after rule action list if it exists */
		if (act->qhd_obj.qhd_qep.ahd_after_act != NULL)
		{
		    status = qee_cract(qef_rcb, dsh,
				act->qhd_obj.qhd_qep.ahd_after_act,
				eobptr, eobcount, ulm);
		    if (DB_FAILURE_MACRO(status))
			return (status);
		}
		/* And the before rule list, too. */
		if (act->qhd_obj.qhd_qep.ahd_before_act != NULL)
		{
		    status = qee_cract(qef_rcb, dsh,
				act->qhd_obj.qhd_qep.ahd_before_act,
				eobptr, eobcount, ulm);
		    if (DB_FAILURE_MACRO(status))
			return (status);
		}
		break;

    	    case QEA_RDEL:
		/* Initialize the after rule action list if it exists */
		if (act->qhd_obj.qhd_qep.ahd_after_act != NULL)
		{
		    status = qee_cract(qef_rcb, dsh,
				act->qhd_obj.qhd_qep.ahd_after_act,
				eobptr, eobcount, ulm);
		    if (DB_FAILURE_MACRO(status))
			return (status);
		}
		/* And the before rule list, too. */
		if (act->qhd_obj.qhd_qep.ahd_before_act != NULL)
		{
		    status = qee_cract(qef_rcb, dsh,
				act->qhd_obj.qhd_qep.ahd_before_act,
				eobptr, eobcount, ulm);
		    if (DB_FAILURE_MACRO(status))
			return (status);
		}
		break;

	    case QEA_IF:
		status = qee_ade(qef_rcb, dsh, 
			act->ahd_mkey);
		if (status)
		    break;

		status = qee_ade(qef_rcb, dsh, 
				act->qhd_obj.qhd_if.ahd_condition);
		if (status)
		    break;
		status = qeq_ade(dsh, 
				act->qhd_obj.qhd_if.ahd_condition,
				(PTR *) 0);
		qef_rcb->error.err_code = dsh->dsh_error.err_code;
		break;

	    case QEA_RETURN:
		status = qee_ade(qef_rcb, dsh, 
			act->ahd_mkey);
		if (status)
		    break;

		status = qee_ade(qef_rcb, dsh, 
				act->qhd_obj.qhd_return.ahd_rtn);
		if (status)
		    break;

		status = qeq_ade(dsh, 
				act->qhd_obj.qhd_return.ahd_rtn,
				(PTR *) 0);
		qef_rcb->error.err_code = dsh->dsh_error.err_code;
		break;

	    case QEA_MESSAGE:
	    case QEA_EMESSAGE:
		status = qee_ade(qef_rcb, dsh, 
				act->qhd_obj.qhd_message.ahd_mnumber);
		if (status)
		    break;

		status = qeq_ade(dsh, 
				act->qhd_obj.qhd_message.ahd_mnumber,  
				(PTR *) 0);
		if (status)
		{
		    qef_rcb->error.err_code = dsh->dsh_error.err_code;
		    break;
		}

		status = qee_ade(qef_rcb, dsh, 
				act->qhd_obj.qhd_message.ahd_mtext);
		if (status)
		    break;

		status = qeq_ade(dsh, 
				act->qhd_obj.qhd_message.ahd_mtext, 
				(PTR *) 0);
		qef_rcb->error.err_code = dsh->dsh_error.err_code;
		break;

	    case QEA_CALLPROC:
		status = qee_ade(qef_rcb, dsh,
				act->qhd_obj.qhd_callproc.ahd_procparams);
		if (status)
		    break;

		status = qeq_ade(dsh,
				act->qhd_obj.qhd_callproc.ahd_procparams, 
				(PTR *) 0);

		if (act->qhd_obj.qhd_callproc.ahd_procp_return)
		{
		    status = qee_ade(qef_rcb, dsh,
				act->qhd_obj.qhd_callproc.ahd_procp_return);
		    if (status)
			break;

		    status = qeq_ade(dsh,
				act->qhd_obj.qhd_callproc.ahd_procp_return,
				(PTR *) 0);
		}
		qef_rcb->error.err_code = dsh->dsh_error.err_code;
		break;

	    case QEA_PROC_INSERT:
		status = qee_ade(qef_rcb, dsh,
				act->qhd_obj.qhd_proc_insert.ahd_procparams);
		if (status)
		    break;

		status = qeq_ade(dsh,
				act->qhd_obj.qhd_proc_insert.ahd_procparams, 
				(PTR *) 0);
		if (status)
		{
		    qef_rcb->error.err_code = dsh->dsh_error.err_code;
		    break;
		}

		status = qee_buildTempTable(qef_rcb, dsh, ulm,
			    act->qhd_obj.qhd_proc_insert.ahd_proc_temptable);
		break;

	    case QEA_EVRAISE:
		status = qee_ade(qef_rcb, dsh,
				act->qhd_obj.qhd_event.ahd_evvalue);
		if (status)
		    break;

		status = qeq_ade(dsh,
				act->qhd_obj.qhd_event.ahd_evvalue, 
				(PTR *) 0);
		qef_rcb->error.err_code = dsh->dsh_error.err_code;
		break;
	    case QEA_EVREGISTER:
	    case QEA_EVDEREG:
		break;
		
	    default:
		break;
	}
    }
#ifdef xDEBUG
    (VOID) qe2_chk_qp(dsh);
#endif 
    return (status);
}


/*{
** Name: QEE_CHKEOB	- check for end of action block 
**
** Description:
**	Processing of actions in IF blocks of a DB procedure requires
**	recursive execution of qee_cract to be able to process nested
**	blocks.  Processing of action in an IF block needs to terminate
**      at the end of the block (or at the begining of the block for a
**	WHILE statement).  However, with endloop semantics, the
**	processing must also terminate when the end of any outer block
**	is encountered.  Thus all the end actions of all the outer 
**	blocks must be watched for.
**
**	If count is zero, this procedure will not return a TRUE
**	termination condition until the end of the action list is reached.
**	This is used for recursively processing rule action lists which
**	do not follow the scoping rules for IF and WHILE statement in
**	the current QP.
**	
**	Given an action header pointer, this routine checks whether the
**	action is the beginning or the ending action of any outer blocks.
**
** Inputs:
**      act			    The current action pointer
**      eobptr                      Ptr to array of beginning and ending
**				    actions of all outer blocks.  
**      count			    count of elements in the array
**
** Outputs:
**      none
**	Returns:
**	    TRUE		    if the termination condition is reached.
**	    FALSE		    otherwise
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	03-aug-88 (puree)
**	    create of DB procedure.
**	15-jan-89 (paul)
**	    added comments above that constrain qee_chkeob to specifically
**	    allow a zero length action list.
*/
bool
qee_chkeob(
QEF_AHD	    *act,
QEF_AHD	    **eobptr,
i4	    count )
{

    if (act == (QEF_AHD *)NULL)
	return (TRUE);

    if (count == 0)
	return (FALSE);

    while (count)
    {
	--count;
	if (*(eobptr+count) == act)
	    return (TRUE);
    }
    return (FALSE);
}


/*{
** Name: QEE_ADE	- create the ADE_EXCB
**
** Description:
**      The QEN_ADF struct is used to initialize the control block used
**	for compiled expression evaluation. The ADE_EXCB and the base
**	array are created. If there are no parameters in the ADE_CX struct,
**	QEQ_QEA is called to initiliaze the base array.
**
** Inputs:
**      qef_rcb                         qef request block
**      dsh                             current data segment header
**      qen_adf                         ade_excb initializer
**
** Outputs:
**      qef_rcb
**	    .error.err_code		errors in allocating space
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**	Exceptions:
**	    none
**
** Side Effects:
**	    The ade_excbs are created in the DSH
**
** History:
**      1-aug-86 (daved)
**          written 
**      7-oct-87 (puree)
**          initialize adf_excb->excb_seg to ADE_SINIT to be used by
**          qea_fetch for fetching from a constant function.
**      10-dec-87 (puree)
**          Converted all ulm_palloc to qec_palloc.
**	08-dec-89 (fred)
**	    Added large object support.  This entails initializing a few new
**	    members of the ADE_EXCB structure.
**	21-dec-92 (anitap)
**	    Prototyped.
**      20-may-94 (eric)
**          Added initialization of the base array. We now initialize the
**          static parts of the base array here, and initialize only 
**          the parts that change from query to query in QEQ.
**      06-feb-96 (johnst)
**          Bug #72202.
**          Changed qee_ade_base() arguments to match the interface of
**          qeq_ade_base() from which it was originally cloned off,
**          restoring the two {qen_base, num_bases} arguments instead of passing
**          the higher level struct QEN_ADF, as (eric) defined it.
**          The Smart Disk code in qeq_validate() needs to call both
**          qee_ade_base() and qeq_ade_base() passing in the actual address
**          arrays and number of bases used by the Smart Disk case.
**	27-mar-97 (inkdo01)
**	    Added init of ADE_LABBASE entry in excb_bases for suport of 
**	    branching in non-CNF Boolean expressions.
**	15-mar-04 (inkdo01)
**	    Save QEN_ADF addr in base array for later || query init.
**	25-aug-04 (inkdo01)
**	    Place dsh_row addr into excb_bases[ADE_GLOBALBASE] when global
**	    base array logic is enabled.
[@history_template@]...
*/
DB_STATUS
qee_ade(
	QEF_RCB            *qef_rcb,
	QEE_DSH            *dsh,
	QEN_ADF            *qen_adf)
{
    ULM_RCB		    ulm;
    ADE_EXCB		    *adf_excb;
    DB_STATUS		    status;
    QEF_QP_CB		    *qp = dsh->dsh_qp_ptr;
    GLOBALREF QEF_S_CB	    *Qef_s_cb;


#ifdef xDEBUG
    (VOID) qe2_chk_qp(dsh);
#endif 

    if (qen_adf == NULL)
	return (E_DB_OK);

    /* consistency check */
    if (qen_adf->qen_ade_cx == NULL || qen_adf->qen_pos >= qp->qp_cb_cnt)
    {
	qef_rcb->error.err_code = E_QE0002_INTERNAL_ERROR;
	return(E_DB_ERROR);
    }
    /* Format the local ULM request block */

    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);

    if (qp->qp_status & QEQP_RPT)
	ulm.ulm_streamid_p = &((ULH_OBJECT*)dsh->dsh_handle)->ulh_streamid;
    else
	ulm.ulm_streamid_p = &dsh->dsh_handle;

    /* Allocate the ADE_EXCB control block.
    ** The bases are at the tail end of the ADE_EXCB
    */
    ulm.ulm_psize = sizeof (ADE_EXCB) + 
			(sizeof (PTR) * (qen_adf->qen_sz_base + ADE_ZBASE));
    if ((status = qec_malloc(&ulm)) == E_DB_OK)
    {
	adf_excb		= (ADE_EXCB*) ulm.ulm_pptr;
	adf_excb->excb_bases[ADE_CXBASE] = qen_adf->qen_ade_cx;
	adf_excb->excb_bases[ADE_NULLBASE] = (PTR) NULL;
	adf_excb->excb_bases[ADE_LABBASE] = (PTR) NULL;
	adf_excb->excb_bases[ADE_QENADF] = (PTR) qen_adf;
	if (qp->qp_status & QEQP_GLOBAL_BASEARRAY)
	    adf_excb->excb_bases[ADE_GLOBALBASE] = (PTR) dsh->dsh_row;
	else adf_excb->excb_bases[ADE_GLOBALBASE] = (PTR) NULL;
	adf_excb->excb_nbases	= qen_adf->qen_sz_base + ADE_ZBASE;
	adf_excb->excb_cx	= qen_adf->qen_ade_cx;
	adf_excb->excb_seg	= ADE_SINIT;	/* for qea_fetch */
	adf_excb->excb_continuation = 0;
	adf_excb->excb_limited_base = ADE_NOBASE;
	adf_excb->excb_size = 0;
	/* put adf execution block into the DSH */
	dsh->dsh_cbs[qen_adf->qen_pos] = (PTR) adf_excb;

	/* Initialize the DSH static parts of the base array */
	qee_ade_base(dsh, qen_adf->qen_base, &adf_excb->excb_bases[ADE_ZBASE],
			qen_adf->qen_sz_base);
    }    
    else
    {
	/* return */
	qef_rcb->error.err_code = ulm.ulm_error.err_code;
    }

#ifdef xDEBUG
    (VOID) qe2_chk_qp(dsh);
#endif 

    return (status);
}

/*{
** Name: QEE_ADE_BASE	- initialize the invariant BASE pointers
**
** Description:
**  A given address array is initialised by reading the given qen_base array 
**  and generating addresses into the address array. Only the portions of
**  the base array that won't change for the life of the query are initialized
**  here. Base array elements that change once per query invocation are
**  initialized in qeq_ade() (in qeq_ade_base()). Base array elements that
**  change once per tuple are set in QEN.
**
** Inputs:
**      dsh                             current data segment header
**      qen_base			array of bases
**	excb_base			pointers to bases
**      num_bases                       Number of bases to process
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
**      16-May-94 (eric)
**          Created (from qeq_ade_base).
**      06-feb-96 (johnst)
**          Bug #72202.
**          Changed qee_ade_base() arguments to match the interface of
**          qeq_ade_base() from which it was originally cloned off,
**          restoring the two {qen_base, num_bases} arguments instead of passing
**          the higher level struct QEN_ADF, as (eric) defined it.
**          The Smart Disk code in qeq_validate() needs to call both
**          qee_ade_base() and qeq_ade_base() passing in the actual address
**          arrays and number of bases used by the Smart Disk case.
[@history_template@]...
*/
DB_STATUS
qee_ade_base(
	QEE_DSH            *dsh,
	QEN_BASE           *qen_base,
	PTR                *excb_base,
	i4                num_bases)
{
    DB_STATUS	    	status = E_DB_OK;
    ADF_AG_STRUCT	*adf_ag_struct;
    i4			i;

    /* Convert each base to the correct address */

    for (i = 0; i < num_bases; i++, qen_base++, excb_base++)
    {
	switch (qen_base->qen_array)
	{
	    case QEN_ROW:
		*excb_base = dsh->dsh_row[qen_base->qen_index];
		break;
	    case QEN_KEY:
		*excb_base = dsh->dsh_key;
		break;
	    case QEN_ADF_AG:
		adf_ag_struct = (ADF_AG_STRUCT*) 
			    dsh->dsh_row[qen_base->qen_index];
		*excb_base = (PTR) dsh->dsh_row[qen_base->qen_index];
		/* setup for processing an aggregate */
		adf_ag_struct->adf_agfi	    = qen_base->u.qen_fid;

		/* the query compiler should have allocated enough space 
		** qen_ag_len + sizeof (ADF_AG_STRUCT)
		*/
		adf_ag_struct->adf_agwork.db_datatype = DB_NODT;
		adf_ag_struct->adf_agwork.db_prec = 0;
		adf_ag_struct->adf_agwork.db_data = (PTR) (adf_ag_struct + 1);
		adf_ag_struct->adf_agwork.db_length = qen_base->qen_ag_len;
		adf_ag_struct->adf_agwork.db_collID = -1;
		break;

	    case QEN_PARM:
	    case QEN_PLIST:
		*excb_base = 0;
		break;

	    case QEN_OUT:
	    case QEN_VTEMP:
	    case QEN_DMF_ALLOC:
		break;
	}
    }
    return(status);
}

/*{
** Name: QEE_HFILE	- build the hold file environment
**
** Description:
**	This routine allocates and initializes the DMT_CB and DMR_CB for
**	the hold file of a node.  It does the same for the node's tid
**	hold file.
**
**	Note on the tuples for the hold file:
**
**	A tuple in a hold file hold an intermediate results from the inner
**	node.  The inner node tuple is materialized into the row buffer
**	specified in the QP. The row buffer does not resemble the image of
**	inner tuples.  In fact, the fields for inner tuple attributes may
**	not be contiguous but is memory-aligned for the datatype of each
**	attribute.  To simplified hold file get and put operations,  the
**	entire row buffer, including spaces between attribute fields, is
**	retrieved from/put into the hold file.  This makes it necessary
**	for the hold file to have the tuple size as big as the size of the
**	row buffer.
**
**	Note on the tuples for the tid hold file:
**
**	For right joins inside CPJOINs and ISJOINs, we remember the tids
**	(from the above hold file) which inner joined.  We remember these
**	tids in a special tid hold file.  To identify right joins, we
**	later rescan this tid hold file against the entire inner relation.
**	Confused?  See the documentation in QEN_CPJOIN and QEN_ISJOIN.
**
**
** Inputs:
**	qef_rcb			QEF request block
**      dsh                     QEF data segment structure
**      node                    QEF node structure
**	ulm			ULM request block
**
** Outputs:
**	Returns:
**	    E_DB_{OK, ERROR}
**	Exceptions:
**	    none
**
** Side Effects:
**	    Additional structures are created and initialized.
**
** History:
**      16-apr-87 (daved)
**          written
**      10-dec-87 (puree)
**          Converted all ulm_palloc to qec_palloc
**	08-aug-88 (puree)
**	    Per Eric Lundblad suggestion, QEF will now take the size of
**	    the hold file tuple from the sum of attribute width.  The
**	    row length given in the QP to be used as the buffer for the
**	    hold file rows may be bigger than the actual row size for
**	    memory alignment purpose.
**	09-aug-88 (puree)
**	    Continue fixing problem with hold file tuple length.  We have
**	    to use the total row size for the hold file tuple length because
**	    the join node will materialize the inner tuple into the row
**	    buffer as specified in the CX bases.  Since OPC aligns each
**	    attribute field on datatype boundary, the row buffer will actually
**	    be bigger than the sum of attribute lengths.  So we decide to
**	    build the hold file with the row length, which is guaranteed
**	    to be equal to or greater than the sum of attribute length.
**	10-feb-1992 (bryanp)
**	    Changed dmt_location from a DB_LOC_NAME to a DM_DATA.
**	25-mar-92 (rickh)
**	    Added tid hold file allocation.  Broke most of this routine	
**	    out into qee_buildHoldFile.  Added initialization of oj_ijFlagsFile.
**	11-aug-92 (rickh)
**	    Removed hold file for isjoining tids.  This has been replaced with
**	    a temporary table.
**	21-dec-92 (anitap)
**	    Prototyped.
**	7-nov-1995 (inkdo01)
**	    Changes to replace QEN_OJINFO structure instances by pointers in
**	    QEN_SJOIN and QEN_SEJOIN.
[@history_template@]...
*/
DB_STATUS
qee_hfile(
	QEF_RCB		    *qef_rcb,
	QEE_DSH		    *dsh,
	QEN_NODE	    *node,
	ULM_RCB		    *ulm)
{
    DB_STATUS	    status = E_DB_OK;
    i4	    row_no;
    i4		    hfile_no;
    i4		    att_count;
    
    /* build hold file for whole tuples from inner child	*/

    switch (node->qen_type)
    {
	case QE_FSMJOIN:
        case QE_ISJOIN:
        case QE_CPJOIN: 
	    hfile_no	= node->node_qen.qen_sjoin.sjn_hfile;
	    att_count	= node->node_qen.qen_sjoin.sjn_inner->qen_natts;
	    row_no	= node->node_qen.qen_sjoin.sjn_itmat->qen_output;
	    break;
	case QE_SEJOIN:
	    hfile_no	= node->node_qen.qen_sejoin.sejn_hfile;
	    att_count	= node->node_qen.qen_sejoin.sejn_inner->qen_natts;
	    row_no	= node->node_qen.qen_sejoin.sejn_itmat->qen_output;
	    break;

	default:
	    hfile_no = -1;
	    break;
    }

    if( hfile_no >= 0 )
    {
	status = qee_buildHoldFile(	qef_rcb, dsh, node, ulm,
					att_count, row_no, hfile_no );
	if ( status )	return( status );
    }

    /* during right joins, build tid hold file for inner joins	*/

    switch (node->qen_type)
    {
        case QE_CPJOIN: 
	    hfile_no	= (node->node_qen.qen_sjoin.sjn_oj) ?
	   	node->node_qen.qen_sjoin.sjn_oj->oj_tidHoldFile : -1;
	    att_count	= 1;	/* one attribute:  the tid	*/
	    row_no  	= (node->node_qen.qen_sjoin.sjn_oj) ?
	   	node->node_qen.qen_sjoin.sjn_oj->oj_heldTidRow : -1;
	    break;

	case QE_FSMJOIN:
	    hfile_no	= (node->node_qen.qen_sjoin.sjn_oj) ?
	   	node->node_qen.qen_sjoin.sjn_oj->oj_ijFlagsFile : -1;
	    att_count	= 1;	/* one attribute:  the flag */
	    row_no  	= (node->node_qen.qen_sjoin.sjn_oj) ?
	   	node->node_qen.qen_sjoin.sjn_oj->oj_ijFlagsRow : -1;
	    break;

	default:
	    hfile_no = -1;
	    break;
    }

    if( hfile_no >= 0 )
    {
	status = qee_buildHoldFile(	qef_rcb, dsh, node, ulm,
					att_count, row_no, hfile_no );
	if ( status )	return( status );
    }

    return (status);
}


/*{
** Name: QEE_JOINTEMPTABLES - build temporary tables for join nodes
**
** Description:
**	This routine allocates and initializes the DMT_CB and DMR_CB for
**	the temporary tables of a join node.
**
** Inputs:
**	qef_rcb			QEF request block
**      dsh                     QEF data segment structure
**      node                    QEF node structure
**	ulm			ULM request block
**
** Outputs:
**	Returns:
**	    E_DB_{OK, ERROR}
**	Exceptions:
**	    none
**
** Side Effects:
**	    Additional structures are created and initialized.
**
** History:
**	11-aug-92 (rickh)
**	    Created.
**	12-oct-92 (rickh)
**	    Add CPJOINs.
**	21-dec-92 (anitap)
**	    Prototyped.
**	08-feb-94 (rickh)
**	    TID temp table for right ISJOINs and CPJOINs now has two sort
**	    attributes.
[@history_template@]...
*/
static DB_STATUS
qee_joinTempTables(
	QEF_RCB		    *qef_rcb,
	QEE_DSH		    *dsh,
	QEN_NODE	    *node,
	ULM_RCB		    *ulm)
{
    QEN_TEMP_TABLE  *tempTable;
    QEE_TEMP_TABLE  *dshTempTable;
    DMT_CB	    *dmt_cb;
    DMT_ATTR_ENTRY  **attr_entry;
    DMT_KEY_ENTRY   **keyArray;
    DMT_KEY_ENTRY   *key;
    DB_STATUS	    status = E_DB_OK;

    for (;;)	/* the usual and customary code block */
    {

    /* build temporary tables for right joins */

        switch (node->qen_type)
        {
            case QE_CPJOIN:
            case QE_ISJOIN:
		tempTable = (node->node_qen.qen_sjoin.sjn_oj) ?
		    node->node_qen.qen_sjoin.sjn_oj->oj_innerJoinedTIDs :
		    (QEN_TEMP_TABLE *)NULL;
		if ( tempTable && tempTable->ttb_tempTableIndex < 0 )
		    tempTable = (QEN_TEMP_TABLE *) NULL;
	        break;

	    default:
		tempTable = (QEN_TEMP_TABLE *) NULL;
	        break;
        }

	if ( tempTable != (QEN_TEMP_TABLE *) NULL )
	{
	    /*
	    ** an attribute list was compiled into the temporary table
	    ** descriptor.  the following routine will plug that
	    ** attribute list into the corresponding DMT_CB.
	    */

	    status = qee_buildTempTable( qef_rcb, dsh, ulm, tempTable );
	    if ( status ) break;

	    /*
	    ** set up to sort on the two i4s into which the TID will
	    ** be split.
	    **
	    */

	    dshTempTable =
		dsh->dsh_tempTables[ tempTable->ttb_tempTableIndex ];
	    dmt_cb = dshTempTable->tt_dmtcb;

	    dmt_cb->dmt_key_array.ptr_address  = (PTR) tempTable->ttb_key_list;
	    dmt_cb->dmt_key_array.ptr_in_count = tempTable->ttb_key_count;
	    dmt_cb->dmt_key_array.ptr_size = sizeof (DMT_KEY_ENTRY);

	}	/* endif build right join tid temp table */

	break;
    }	/* end of the usual and customary code block */

    return( status );
}

/*{
** Name: QEE_BUILDTEMPTABLE - build structures for a temporary table
**
** Description:
**	This routine allocates and initializes the DMT_CB and DMR_CB for
**	a temporary table.
**
** Inputs:
**	qef_rcb			QEF request block
**      dsh                     QEF data segment structure
**	ulm			ULM request block
**	QPtempTable		pointer to the QP temporary table descriptor
**
** Outputs:
**	Returns:
**	    E_DB_{OK, ERROR}
**	Exceptions:
**	    none
**
** Side Effects:
**	    Additional structures are created and initialized.
**
** History:
**	11-aug-92 (rickh)
**	    Created.
**	21-dec-92 (anitap)
**	    Prototyped.
**	02-apr-93 (jhahn)
**	    Added parameters to qee_allocateDMTCB for support of statement
**	    level unique indexes.
**
[@history_template@]...
*/
static DB_STATUS
qee_buildTempTable(
	QEF_RCB		    *qef_rcb,
	QEE_DSH		    *dsh,
	ULM_RCB		    *ulm,
	QEN_TEMP_TABLE      *QPtempTable)
{
    QEE_TEMP_TABLE  *dshTempTable;   
    DB_STATUS	    status = E_DB_OK;

    for (;;)	/* the usual and customary code block */
    {

	dshTempTable = dsh->dsh_tempTables[ QPtempTable->ttb_tempTableIndex ];
	dshTempTable->tt_statusFlags = TT_EMPTY;
	dshTempTable->tt_tuple = QPtempTable->ttb_tuple;

	/* Allocate the DMR_CB */
	if ( status = qee_allocateDMRCB( qef_rcb, dsh, ulm,
	      dshTempTable->tt_tuple, &dshTempTable->tt_dmrcb  ) )
	    return( status );

	/* Allocate the DMT_CB */
	if ( status = qee_allocateDMTCB( qef_rcb, dsh, ulm,
	      dshTempTable->tt_tuple, &dshTempTable->tt_dmtcb,
	      QPtempTable->ttb_attr_list, QPtempTable->ttb_attr_count  ) )
	    return( status );

	break;
    }	/* end of the usual and customary code block */

    return( status );
}

/*{
** Name: QEE_BUILDHOLDFILE	- build the hold file structures
**
** Description:
**	This routine allocates and initializes the DMT_CB and DMR_CB for
**	a hold file of a node.  This can be either the hold file sheltering
**	whole tuples from the inner child OR the hold file collecting
**	inner join tids for right join cases.
**
** Inputs:
**	qef_rcb			QEF request block
**      dsh                     QEF data segment structure
**      node                    QEF node structure
**	ulm			ULM request block
**	att_count		number of attributes in tuple
**	row_no			dsh row of tuple
**	hfile_no		index into array of hold file descriptors
**
** Outputs:
**	Returns:
**	    E_DB_{OK, ERROR}
**	Exceptions:
**	    none
**
** Side Effects:
**	    Additional structures are created and initialized.
**
** History:
**	25-mar-92 (rickh)
**	    Cut out of qee_hfile so that it could be called twice:  once
**	    for the whole tuple hold file and once for the tid hold file.
**	12-aug-92 (rickh)
**	    Extracted the dmrcb and dmtcb allocators so they could be called
**	    for temporary tables too.
**	21-dec-92 (anitap)
**	    Prototyped.
**	02-apr-93 (jhahn)
**	    Added parameters to qee_allocateDMTCB for support of statement
**	    level unique indexes.
**	06-mar-96 (nanpr01)
**	    Initialize the hold file pagesize. We can probably use a specific
**	    page size. But for now use the maxtup size of the installation.
**	24-nov-97 (inkdo01)
**	    Init shd_free, too.
**	
[@history_template@]...
*/
static DB_STATUS
qee_buildHoldFile(
	QEF_RCB		    *qef_rcb,
	QEE_DSH		    *dsh,
	QEN_NODE	    *node,
	ULM_RCB		    *ulm,
	i4		    att_count,
	i4	    	    row_no,
	i4		    hfile_no)
{
    DB_STATUS	    status = E_DB_OK;
    QEN_HOLD	    *qen_hold;
    QEN_SHD         *qen_shd;
    i4              shd_no;
    
    shd_no = dsh->dsh_qp_ptr->qp_sort_cnt + hfile_no;
    qen_hold = dsh->dsh_hold[hfile_no];
    qen_shd = dsh->dsh_shd[shd_no];
    if (att_count > 0)
    {
	/* Allocate the DMR_CB */
	if ( status = qee_allocateDMRCB( qef_rcb, dsh, ulm, row_no,
	     &qen_hold->hold_dmr_cb  ) )
	    return( status );

	/* Allocate the DMT_CB */
	if ( status = qee_allocateDMTCB( qef_rcb, dsh, ulm, row_no,
	     &qen_hold->hold_dmt_cb, (DMT_ATTR_ENTRY **) NULL, 0))
	    return( status );

	/* Initialize parts of the SHD */
	qen_shd->shd_node = node;
        qen_shd->shd_streamid = (PTR)NULL; 
	qen_shd->shd_vector = (PTR *)NULL;
	qen_shd->shd_mdata = (DM_MDATA *)NULL;
	qen_shd->shd_free = (DM_MDATA *)NULL;
	qen_shd->shd_tup_cnt = 0;
	qen_shd->shd_next_tup = 0;
	qen_shd->shd_row = dsh->dsh_row[row_no];
	qen_shd->shd_width = dsh->dsh_qp_ptr->qp_row_len[row_no];
	qen_shd->shd_options = 0;
	qen_shd->shd_dups = 0;   
	
	/* Initialize the hold structure page size */
	{
   	  DMC_CB          dmc_cb, *dmc = &dmc_cb;
#define PG_SIZE         2048
#define NO_OF_POOL      6
   	  DMC_CHAR_ENTRY  dmc_char[NO_OF_POOL];
   	  DB_STATUS       status;
   	  i4              i;
	  i4	  page_sz;
 
   	  MEfill( sizeof(dmc_cb), (u_char) 0, (PTR) dmc);
   	  dmc->type = DMC_CONTROL_CB;
   	  dmc->length = sizeof(DMC_CB);
   	  dmc->dmc_op_type = DMC_BMPOOL_OP;
   	  dmc->dmc_flags_mask = 0;
   	  dmc->dmc_char_array.data_address = (PTR) &dmc_char;
   	  dmc->dmc_char_array.data_out_size = sizeof(DMC_CHAR_ENTRY)*NO_OF_POOL;
 
   	  status = dmf_call(DMC_SHOW, (PTR) dmc);
   	  if (DB_FAILURE_MACRO(status))
   	  {
        	/*
        	**  FIX ME - Print out Error
        	*/
   	  }
   	  for (i = 0, page_sz = PG_SIZE; i < NO_OF_POOL; i++, page_sz *= 2)
   	  {
       	      if (dmc_char[i].char_value == DMC_C_OFF)
		continue;
	      qen_hold->hold_pagesize = page_sz;
   	  }
	}
    }
    else
    {
	qen_hold->hold_dmr_cb = (DMR_CB *)NULL;
	qen_hold->hold_dmt_cb = (DMT_CB *)NULL;
    }
    return (status);
}

/*{
** Name: QEE_ALLOCATEDMRCB - allocate and initialize a dmrcb
**
** Description:
**	This routine allocates and initializes the DMR_CB for a temporary
**	table.
**
** Inputs:
**	qef_rcb			QEF request block
**      dsh                     QEF data segment structure
**	ulm			ULM request block
**	row_no			dsh row of tuple
**	parentsDMRCBptr		pointer to parent's DMRCB pointer
**
** Outputs:
**	Returns:
**	    E_DB_{OK, ERROR}
**	Exceptions:
**	    none
**
** Side Effects:
**	    Additional structures are created and initialized.
**
** History:
**	12-aug-92 (rickh)
**	    Carved out of the hold file allocator so that it could also
**	    be called by the temporary table allocator.
**	21-dec-92 (anitap)
**	    Prototyped.
**	
[@history_template@]...
*/
static DB_STATUS
qee_allocateDMRCB(
	QEF_RCB		    *qef_rcb,
	QEE_DSH		    *dsh,
	ULM_RCB		    *ulm,
	i4	    	    row_no,
	DMR_CB		    **parentsDMRCBptr)
{
    DMR_CB	    *dmr_cb;
    i4		    att_length;
    DB_STATUS	    status = E_DB_OK;

    for (;;)	/* something to break out of */
    {
	ulm->ulm_psize = sizeof(DMR_CB);
	if (status = qec_malloc(ulm))
	{
	    qef_rcb->error.err_code = ulm->ulm_error.err_code;
	    break;
	}
	/* Initialize the DMR_CB */
	*parentsDMRCBptr = (DMR_CB *) ulm->ulm_pptr;
	dmr_cb = (DMR_CB*) ulm->ulm_pptr;
	dmr_cb->q_next = (PTR)NULL;
	dmr_cb->q_prev = (PTR)NULL;
	dmr_cb->length = sizeof (DMR_CB);
	dmr_cb->type = (i2)DMR_RECORD_CB;
	dmr_cb->owner = (PTR)DB_QEF_ID;
	dmr_cb->ascii_id = DMR_ASCII_ID;
	dmr_cb->dmr_access_id = (PTR)NULL;

	/* Allocate the key array for the DMR_CB */
	dmr_cb->dmr_attr_desc.ptr_in_count  = 0;
	dmr_cb->dmr_attr_desc.ptr_address   = 0;

	/* Assign the output row and its size.  See routine header comment
	** for qee_hfile about the size of the hold file tuple (att_length). 
	*/
	att_length = dsh->dsh_qp_ptr->qp_row_len[row_no];
	dmr_cb->dmr_data.data_in_size = att_length;
	dmr_cb->dmr_data.data_address = dsh->dsh_row[row_no];

	/* Clear the qualification function */	
	dmr_cb->dmr_q_fcn = 0;
	dmr_cb->dmr_q_arg = 0;

	break;
    }	/* end of the usual and customary code block */

    return( status );
}

/*{
** Name: QEE_ALLOCATEDMTCB - allocate and initialize a dmtcb
**
** Description:
**	This routine allocates and initializes the DMT_CB for a temporary
**	table.
**
** Inputs:
**	qef_rcb			QEF request block
**      dsh                     QEF data segment structure
**	ulm			ULM request block
**	row_no			dsh row of tuple
**	parentsDMTCBptr		pointer to parent's DMTCB pointer
**	attr_list		pointer to attribute list for table. NULL if
**				a default one should be created.
**	attr_count		number of attributes (if attr_list != NULL)
**
** Outputs:
**	Returns:
**	    E_DB_{OK, ERROR}
**	Exceptions:
**	    none
**
** Side Effects:
**	    Additional structures are created and initialized.
**
** History:
**	12-aug-92 (rickh)
**	    Carved out of the hold file allocator so that it could also
**	    be called by the temporary table allocator.
**	21-dec-92 (anitap)
**	    Prototyped.
**	02-apr-93 (jhahn)
**	    Added attr_list and attr_count for support of statement
**	    level unique indexes.
**	06-mar-96 (nanpr01 & ramra01)
**	    Calculating the attribute count for hold file for 2000 byte
**	    long is wrong since it will calculate it as 2 rather than 1.
**	7-Apr-2004 (schak24)
**	    Don't leave junk in attr default fields.
**	
[@history_template@]...
*/
static DB_STATUS
qee_allocateDMTCB(
	QEF_RCB		    *qef_rcb,
	QEE_DSH		    *dsh,
	ULM_RCB		    *ulm,
	i4	    	    row_no,
	DMT_CB		    **parentsDMTCBptr,
	DMT_ATTR_ENTRY	    **attr_list,
	i4		    attr_count)
{
    DMT_CB	    *dmt_cb;
    i4		    att_length;
    i4		    i,k;
    DMT_ATTR_ENTRY  **attr_entry;
    char	    buf[DB_MAXNAME];
    DB_STATUS	    status = E_DB_OK;

    for (;;)	/* something to break out of */
    {
	ulm->ulm_psize = sizeof(DMT_CB);
	if (status = qec_malloc(ulm))
	{
	    qef_rcb->error.err_code = ulm->ulm_error.err_code;
	    break;
	}

	/* Initialiaze the DMT_CB */
	*parentsDMTCBptr = (DMT_CB *) ulm->ulm_pptr;
	dmt_cb = (DMT_CB *) (ulm->ulm_pptr);
	dmt_cb->q_next = (PTR)NULL;
	dmt_cb->q_prev = (PTR)NULL;
	dmt_cb->length = sizeof (DMT_CB);
	dmt_cb->type = DMT_TABLE_CB;
	dmt_cb->owner = (PTR)DB_QEF_ID;
	dmt_cb->ascii_id = DMT_ASCII_ID;
	dmt_cb->dmt_db_id = qef_rcb->qef_db_id;
	dmt_cb->dmt_record_access_id = (PTR)NULL;
	dmt_cb->dmt_flags_mask = DMT_DBMS_REQUEST;

	/* exclusive locking on table */
	dmt_cb->dmt_lock_mode = DMT_X;
	dmt_cb->dmt_update_mode = DMT_U_DIRECT;
	dmt_cb->dmt_access_mode = DMT_A_WRITE;
	dmt_cb->dmt_char_array.data_address = 0;
	dmt_cb->dmt_char_array.data_in_size = 0;
	dmt_cb->dmt_key_array.ptr_in_count = 0;
	dmt_cb->dmt_key_array.ptr_address = 0;
        dmt_cb->dmt_mustlock = TRUE;
	/*
	** Allocate the location array. Assume that all internal temporary
	** tables which we'll use have exactly one location.
	*/
	ulm->ulm_psize = sizeof(DB_LOC_NAME);
	if (status = qec_malloc(ulm))
	{
	    qef_rcb->error.err_code = ulm->ulm_error.err_code;
	    break;
	}
	dmt_cb->dmt_location.data_address = ulm->ulm_pptr;
	if (attr_list != NULL)
	{
	    dmt_cb->dmt_attr_array.ptr_address  = (PTR) attr_list;
	    dmt_cb->dmt_attr_array.ptr_in_count = attr_count;
	}
	else
	{
	    /* 
	    ** Allocate the attribute array so we can build the
	    ** hold table.
	    ** NOTE:  All attributes of the hold file is collapsed into
	    **	  a sequence of DB_CHA_TYPE data. Thus if the
	    **	  total length of the hold file tuple is less than 
	    **	  DB_CHAR_MAX, the hold file will have a single attribute
	    **	  named 'a0'.
	    */
	    att_length = dsh->dsh_qp_ptr->qp_row_len[row_no];
	    i = (att_length / (DB_CHAR_MAX + 1)) + 1;
	    dmt_cb->dmt_attr_array.ptr_in_count = i;
	    ulm->ulm_psize = i * sizeof(PTR);
	    if (status = qec_malloc(ulm))
	    {
		qef_rcb->error.err_code = ulm->ulm_error.err_code;
		break;
	    }
	    attr_entry  = (DMT_ATTR_ENTRY**) ulm->ulm_pptr;

	    i = 0;
	    buf[0] = 'a';
	    ulm->ulm_psize = sizeof(DMT_ATTR_ENTRY);
	    while (att_length > 0)
	    {
		if (status = qec_malloc(ulm))
		{
		    qef_rcb->error.err_code = ulm->ulm_error.err_code;
		    break;
		}
		attr_entry[i] = (DMT_ATTR_ENTRY*) ulm->ulm_pptr;

		CVla((i4) i, &buf[1]);
		STmove(buf, ' ', sizeof(attr_entry[i]->attr_name),
		       (char*) &attr_entry[i]->attr_name);
		attr_entry[i]->attr_type = DB_CHA_TYPE;
		if (att_length > DB_CHAR_MAX)
		    k = DB_CHAR_MAX;
		else
		    k = att_length;
		att_length -= k;
		attr_entry[i]->attr_size = k;
		attr_entry[i]->attr_precision = 0;
		attr_entry[i]->attr_flags = 0;
		SET_CANON_DEF_ID(attr_entry[i]->attr_defaultID, DB_DEF_NOT_DEFAULT);
		attr_entry[i]->attr_default = NULL;
		attr_entry[i]->attr_collID = -1;
		i++;
	    }
	    dmt_cb->dmt_attr_array.ptr_address  = (PTR) attr_entry;
	}
	dmt_cb->dmt_attr_array.ptr_size = sizeof (DMT_ATTR_ENTRY);

	break;
    }	/* end of the usual and customary code block */

    return( status );
}

/*{
** Name: qee_dbparam	- Substitute user's parementers in DB procedure
**			  DSH row buffers.
**
** Description:
**      This routine is called when the calling routine recognizes a 
**	QP for a DB procedure requires the execution parameters.  This
**	routine also expects that the DSH must have been located and 
**	initialized.  Consequently, these assumptions are not verified.
**	Instead, the routine proceeds to get the parameters from the QEF
**	request block (qef_rcb) or the parameters explicitly passed to
**	this routine and substitutes these values into the 
**	corresponding row buffers in the DSH.
**
**	The arguments are constrained as follows:
**	    if nested == TRUE
**		parameters are passed through callers_dsh and the
**		number of parameters is in ucount.
**	    else if nested == FALSE
**		qef_rcb.qef_cp_param contains a pointer to an array of
**		QEF_USR_PARAM structures containing the actual arguments
**		for the call and the number of arguments is in
**		qef_rcb.qef_pcount.
**
**	The substitution is done by scanning the parameter array in the QP,
**	matching it with corresponding entries in the qef_rcb parameter
**	list.  In anticipation that most DB procedures have their parameter
**	list in order, the scanning is done in the sort/merge manner.
**
**	To allow for general datatype conversions, adc_cvinto is called 
**	for the substitution of each parameter.
**
**	An error message is generated if a parameter is not supplied
**	and the default value is not specified in the QP.
**	
** Inputs:
**	qef_rcb			QEF request block
**	dsh			QEF data segment structure
**	callers_dsh		callers data segment (if nested)
**	cp_params		list of parameters (if nested)
**	ucount			number of passed parameters (if nested)
**	nested			is this a nested procedure invocation or
**				is it from the front end?
**
** Outputs:
**	qef_rcb
**	    .error.err_code	    one of the following
**				    E_QE0113_PARAM_CONVERSION
**				    E_QE011B_PARAM_MISSING
**				    E_QE0123_PARAM_INVALID
**				    E_QE0124_PARAM_MANY
**	Returns:
**	    {@return_description@}
**	Exceptions:
**	    [@description_or_none@]
**
** Side Effects:
**	    [@description_or_none@]
**
** History:
**      18-may-88 (puree)
**          Created for DB procedure.
**	18-feb-89 (paul)
**	    Modified to support an array of pointers to
**	    parameters or an array of parameters.
**	    This is used for QEA_CALLPROC actions. See comments in the
**	    procedure header for details.
**	31-may-89 (neil)
**	    Issue error on invalid DBP param name or usage. Reduced copy
**	    overhead.
**	06-jul-89 (neil)
**	    Ignore case on DBP param names.  When the procedure is created PSF
**	    lowercases all names.  During execution we can only match names
**	    if we ignore case as is the default in INGRES.  An error occurred
**	    if a parameter was passed in uppercase - it wasn't found.
**	16-feb-90 (mikem)
**	    BYTE_ALIGN fix for qee_dbparam().  On BYTE_ALIGN machines parameters
**	    must be aligned before calling adc_cvinto().  This fix, fixes the
**	    problem using brute force (declaring a max sized buffer on the
**	    stack, and doing the MEcopy when necessary).  We may want to change
**	    this solution to something more elegant (ie. have opc allocate
**	    aligned memory for these parameters at query "compile" time, so
**	    that they are aligned by the time they get to this routine).
**	    Warning: this bug fix does allocate a ~2k buffer on the stack; the
**	    hope is that this routine is called at a low enough level to make
**	    this not be a problem with stack size.
**	29-jul-1991 (markg)
**	    Changed DBP missing parameter error handling to include parameter
**	    name and database procedure name in error message. This involved 
**	    creating a new message E_DM011B, the old message E_DM0114 is now 
**	    obsolete. Bug 38765.
**	03-nov-92 (ralph)
**	    Don't ignore case with STbcompare() when checking database
**	    procedure parameters.
**	09-nov-92 (jhahn)
**	    Added handling for byrefs.
**	14-dec-92 (jhahn)
**	    Cleaned up handling of byrefs.
**	21-dec-92 (anitap)
**	    Prototyped.
**	15-sep-97 (nanpr01)
**	    bug 84970 : When user passes a character value into a procedure's 
**	    decimal parameter, he should recieve E_QE0113. However, he gets 
**	    E_US100F instead E_QE0113.
**	3-Apr-98 (thaju02)
**	    If error was an adp peripheral (blob) move error, report a 
**	    qef peripheral move error.
**	3-feb-99 (inkdo01)
**	    Assure we only do this once for a row producing procedure.
**	20-mar-2001 (somsa01)
**	    To save stack space, allocate space for align_buf dynamically.
**	23-feb-04 (inkdo01)
**	    Use DSH versions of various RCB fields for || query thread 
**	    safety.
**	21-june-06 (dougi)  SIR 116219
**	    Add support for IN, OUT, INOUT parameter modes.
*/
DB_STATUS
qee_dbparam(
	QEF_RCB		*qef_rcb,
	QEE_DSH		*dsh,
	QEE_DSH       	*callers_dsh,
	QEF_CP_PARAM	cp_params[],
	i4		ucount,
	bool 		nested)
{
    QEF_QP_CB	    *qp = dsh->dsh_qp_ptr;
    QEF_CB	    *qef_cb = dsh->dsh_qefcb;
    ADF_CB	    *adf_cb = dsh->dsh_adf_cb;
    QEF_USR_PARAM   *uparm;
    QEF_USR_PARAM   uparm_val;
    QEF_DBP_PARAM   *dbparm;
    DB_STATUS	    status;
    i4	    err;
    i4		    pcount, uindex, i, j;
    i4		    actuals_used;	/* Number of actual parameters used */
    i4		    non_inout = 0;
    DB_DATA_VALUE   row_dbv;    
    DB_DATA_VALUE   *byref_dbvp;    

#ifdef BYTE_ALIGN
    ALIGN_RESTRICT  *align_buf;
    PTR		    save_old_ptr;
#endif /* BYTE_ALIGN */

    /* If this is from the front end get the count of actual parameters for this procedure call */
    if (!nested)
	ucount = qef_rcb->qef_pcount;
    pcount = qp->qp_ndbp_params;    /* count of formal parameters in QP */
    uindex = 0;
    actuals_used = 0;			    /* No actuals used yet */

    if (dsh->dsh_qp_status & DSH_DBPARMS_DONE) return(E_DB_OK);
					    /* If already done (as in rowproc)
					    ** don't do it again */
    dsh->dsh_qp_status |= DSH_DBPARMS_DONE;

    /* Scan the QP array in the outer loop */
    for (i = 0; i < pcount; i++)
    {
	/* Get the current param in the QP.  Set up data value ptr to
	** the corresponding row buffer in the DSH */
	dbparm = qp->qp_dbp_params + i;

	/* If it isn't a IN or INOUT parm, skip over it. */
	if (dbparm->dbp_mode != DBP_PMIN &&
	    dbparm->dbp_mode != DBP_PMINOUT)
	{
	    non_inout++;
	    continue;
	}

	STRUCT_ASSIGN_MACRO(dbparm->dbp_dbv, row_dbv);
	row_dbv.db_data = (PTR)((char *)(dsh->dsh_row[dbparm->dbp_rowno]) +
				dbparm->dbp_offset);
	byref_dbvp = (DB_DATA_VALUE *)
	    ((char *)(dsh->dsh_row[dbparm->dbp_byref_value_rowno]) +
				dbparm->dbp_byref_value_offset);
	/* Scan the user's array in the inner loop.
	** Notice that the user's array is scanned from the position last
	** examined (uindex), wrapping around if necessary.  If a match 
	** is found, uindex is updated to the next entry which will be
	** where the next scan starts.
	*/
	for (j = 0; j < ucount; j++)
	{   
	    /* Get the current user parm */
	    if (nested)
	    {
		STRUCT_ASSIGN_MACRO(cp_params[uindex].ahd_parm,
				    uparm_val);
		uparm_val.parm_dbv.db_data = (PTR)
		    ((char *) callers_dsh->
		     dsh_row[cp_params[uindex].ahd_parm_row] +
		     cp_params[uindex].ahd_parm_offset);
		uparm = &uparm_val;
	    }
	    else
	    {
	    	uparm = *(qef_rcb->qef_usr_param + uindex);
	    }
	    /* If the names are the same (w/o blanks and ignore case) */
	    if (   (STncmp(uparm->parm_name, 
			       dbparm->dbp_name, uparm->parm_nlen ) == 0)
		&& (   uparm->parm_nlen == DB_MAXNAME
		    || dbparm->dbp_name[uparm->parm_nlen] == ' ')
	       )
	    {
#		ifdef BYTE_ALIGN
		if (ME_NOT_ALIGNED_MACRO(uparm->parm_dbv.db_data))
		{
		    align_buf = (ALIGN_RESTRICT *)MEreqmem(0,
							(DB_GW3_MAXTUP - 1) /
							sizeof(ALIGN_RESTRICT),
							FALSE, NULL);

		    MEcopy( uparm->parm_dbv.db_data,
			    uparm->parm_dbv.db_length,
			    ( PTR ) align_buf);
		    save_old_ptr = (PTR) uparm->parm_dbv.db_data;
		    uparm->parm_dbv.db_data = (PTR) align_buf;

		    /* A match is found.  Substitute user's parameter into
		    ** the corresponding DSH buffer */

		    status = adc_cvinto(adf_cb, &uparm->parm_dbv, &row_dbv);
		    uparm->parm_dbv.db_data = (PTR) save_old_ptr;

		    MEfree((PTR)align_buf);
		}
		else
#		endif /* BYTE_ALIGN */
		{
		    /* A match is found.  Substitute user's parameter into
		    ** the corresponding DSH buffer */
		    status = adc_cvinto(adf_cb, &uparm->parm_dbv, &row_dbv);
		}
	    
		if (status != E_DB_OK)
		{
		    if ((adf_cb->adf_errcb.ad_errcode == E_AD2009_NOCOERCION) ||
			(adf_cb->adf_errcb.ad_errcode == E_AD2004_BAD_DTID) ||
			(adf_cb->adf_errcb.ad_errcode == E_AD5003_BAD_CVTONUM))
		    {
			(VOID) qef_error(E_QE0113_PARAM_CONVERSION, 0L,
				 status, &err, &dsh->dsh_error, 1,
				 qec_trimwhite(uparm->parm_nlen,
					       uparm->parm_name),
				 uparm->parm_name);
			return (status);
		    }

		    if (adf_cb->adf_errcb.ad_errcode == E_AD7009_ADP_MOVE_ERROR)
		    {
			(VOID) qef_error(E_QE011D_PERIPH_CONVERSION, 0L,
				 status, &err, &dsh->dsh_error, 1,
				 qec_trimwhite(uparm->parm_nlen,
				 uparm->parm_name), uparm->parm_name);
			return (status);
		    }
		    if ((status = qef_adf_error(&adf_cb->adf_errcb, 
			status, qef_cb, &dsh->dsh_error)) != E_DB_OK)
			    return (status);
		}
		if (nested)
		{
		    if (uparm->parm_flags & QEF_IS_BYREF)
			STRUCT_ASSIGN_MACRO(uparm->parm_dbv, *byref_dbvp);
		    else if (uparm->parm_flags & QEF_IS_OUTPUT)
			STRUCT_ASSIGN_MACRO(uparm->parm_dbv, *byref_dbvp);
		    else
			byref_dbvp->db_datatype = DB_NODT;
		}
		else
		{
		    uparm->parm_index = i;
		}
		++uindex;
		uindex = uindex % ucount;
		actuals_used++;			/* Track the parameters used */
		break;
	    }
	    ++uindex;
	    uindex = uindex % ucount;
	}
	/* End (or break from) of the inner scan loop.  If no match found
	** (loop index = count of user's parameters), use the default value
	** in the QP. */
	if (j == ucount)
	{   /* missing parameters */
	    if (dbparm->dbp_dbv.db_data == NULL || 
		dbparm->dbp_dbv.db_length == 0)
	    {	/* no default value speicified */
		(VOID) qef_error(E_QE011B_PARAM_MISSING, 0L,
			 status, &err, &dsh->dsh_error, 2,
			 qec_trimwhite(DB_MAXNAME, dbparm->dbp_name),
			 dbparm->dbp_name,
			 qec_trimwhite(DB_MAXNAME, qp->qp_id.db_cur_name),
			 qp->qp_id.db_cur_name);
		return (E_DB_ERROR);
	    }
	    else
	    {
		(VOID)MEcopy((PTR)dbparm->dbp_dbv.db_data, 
		       row_dbv.db_length, (PTR)row_dbv.db_data);
		byref_dbvp->db_datatype = DB_NODT; /* no parameter passed, so
						   ** it's not passed byref */
	    }
	}
    }

    /*
    ** If there are any actual parameters that are unused then issue an error
    ** here.  Note that in order to prevent any loss of performance for a
    ** good procedure invocation, this error lookup is another scan on both
    ** lists - but guaranteed to produce an error.
    */
    if (non_inout + actuals_used < ucount)
    {
	for (i = 0; i < ucount; i++)		/* San user list */
	{   
	    if (nested)
		uparm = &cp_params[i].ahd_parm;
	    else
	    	uparm = *(qef_rcb->qef_usr_param + i);
	    /* Scan DBP list for [mis]match */
	    for (j = 0, dbparm = qp->qp_dbp_params; j < pcount; j++, dbparm++)
	    {
		/* If the names are the same (w/o blanks and ignore case) */
		if (   (STncmp(uparm->parm_name,
				   dbparm->dbp_name, uparm->parm_nlen) == 0)
		    && (   uparm->parm_nlen == DB_MAXNAME
			|| dbparm->dbp_name[uparm->parm_nlen] == ' ')
		   )
		{
		    break;
		}
	    }
	    if (j == pcount)	/* Exhausted parameters */
	    {
		(VOID) qef_error(E_QE0123_PARAM_INVALID, 0L, E_DB_ERROR,
				 &err, &dsh->dsh_error, 1,
				 qec_trimwhite(uparm->parm_nlen,
					       uparm->parm_name),
				 uparm->parm_name);
		return (E_DB_ERROR);
	    }
	}
	/* Fell out so must have specified a valid parameter twice */
	(VOID) qef_error(E_QE0124_PARAM_MANY, 0L, E_DB_ERROR,
			 &err, &dsh->dsh_error, 2,
			 sizeof(ucount), &ucount,
			 sizeof(pcount), &pcount);
	return (E_DB_ERROR);
    }
    return (E_DB_OK);
}

/*{
** Name: qee_return_dbparam - copy OUT, INOUT parameter values back to
**	caller
**
** Description:
**      This routine is called when the called routine is finished. It
**	loops over the formal parameter descriptors looking for OUT and
**	INOUT parameters whose values must be returned to the caller.
**	
** Inputs:
**	qef_rcb			QEF request block
**	dsh			QEF data segment structure
**	callers_dsh		callers data segment (if nested)
**	cp_params		list of parameters (if nested)
**	ucount			number of passed parameters (if nested)
**
** Outputs:
**	Returns:
**	Exceptions:
**
** Side Effects:
**
** History:
**	9-june-06 (dougi)
**	    Written to allow procedures to return parameter values.
*/
DB_STATUS
qee_return_dbparam(
	QEF_RCB		*qef_rcb,
	QEE_DSH		*dsh,
	QEE_DSH       	*callers_dsh,
	QEF_CP_PARAM	cp_params[],
	i4		ucount)
{
    QEF_QP_CB	    *qp = dsh->dsh_qp_ptr;
    QEF_CB	    *qef_cb = dsh->dsh_qefcb;
    ADF_CB	    *adf_cb = dsh->dsh_adf_cb;
    QEF_USR_PARAM   *uparm;
    QEF_USR_PARAM   uparm_val;
    QEF_DBP_PARAM   *dbparm;
    DB_STATUS	    status;
    i4	    err;
    i4		    pcount, uindex, i, j;
    i4		    actuals_used;	/* Number of actual parameters used */
    DB_DATA_VALUE   row_dbv;    
    DB_DATA_VALUE   *byref_dbvp;    


    /* If this is from the front end, just return - we're not interested. */
    if (callers_dsh == (QEE_DSH *) NULL)
	return(E_DB_OK);

    pcount = qp->qp_ndbp_params;    /* count of formal parameters in QP */
    uindex = 0;
    actuals_used = 0;			    /* No actuals used yet */

    /* Scan the QP array in the outer loop */
    for (i = 0; i < pcount; i++)
    {
	/* Get the current param in the QP.  Set up data value ptr to
	** the corresponding row buffer in the DSH */
	dbparm = qp->qp_dbp_params + i;

	/* If it isn't a OUT or INOUT parm, skip over it. */
	if (dbparm->dbp_mode != DBP_PMOUT &&
	    dbparm->dbp_mode != DBP_PMINOUT)
	    continue;

	STRUCT_ASSIGN_MACRO(dbparm->dbp_dbv, row_dbv);
	row_dbv.db_data = (PTR)((char *)(dsh->dsh_row[dbparm->dbp_rowno]) +
				dbparm->dbp_offset);

	/* Scan the actual parameter array in the inner loop.
	** Notice that this array is scanned from the position last
	** examined (uindex), wrapping around if necessary.  If a match 
	** is found, uindex is updated to the next entry which will be
	** where the next scan starts.
	*/
	for (j = 0; j < ucount; j++)
	{   
	    /* Get the current actual parm */
	    STRUCT_ASSIGN_MACRO(cp_params[uindex].ahd_parm, uparm_val);
	    uparm_val.parm_dbv.db_data = (PTR) ((char *) callers_dsh->
		     dsh_row[cp_params[uindex].ahd_parm_row] +
		     cp_params[uindex].ahd_parm_offset);
	    uparm = &uparm_val;

	    /* If the names are the same (w/o blanks and ignore case) */
	    if (   (STncmp(uparm->parm_name, 
			       dbparm->dbp_name, uparm->parm_nlen ) == 0)
		&& (   uparm->parm_nlen == DB_MAXNAME
		    || dbparm->dbp_name[uparm->parm_nlen] == ' ')
	       )
	    {
		/* A match is found.  Move formal parameter back into 
		** caller's DSH buffer */
		status = adc_cvinto(adf_cb, &row_dbv, &uparm->parm_dbv);
	    
		if (status != E_DB_OK)
		{
		    if ((adf_cb->adf_errcb.ad_errcode == E_AD2009_NOCOERCION) ||
			(adf_cb->adf_errcb.ad_errcode == E_AD2004_BAD_DTID) ||
			(adf_cb->adf_errcb.ad_errcode == E_AD5003_BAD_CVTONUM))
		    {
			(VOID) qef_error(E_QE0113_PARAM_CONVERSION, 0L,
				 status, &err, &dsh->dsh_error, 1,
				 qec_trimwhite(uparm->parm_nlen,
					       uparm->parm_name),
				 uparm->parm_name);
			return (status);
		    }

		    if (adf_cb->adf_errcb.ad_errcode == E_AD7009_ADP_MOVE_ERROR)
		    {
			(VOID) qef_error(E_QE011D_PERIPH_CONVERSION, 0L,
				 status, &err, &dsh->dsh_error, 1,
				 qec_trimwhite(uparm->parm_nlen,
				 uparm->parm_name), uparm->parm_name);
			return (status);
		    }
		    if ((status = qef_adf_error(&adf_cb->adf_errcb, 
			status, qef_cb, &dsh->dsh_error)) != E_DB_OK)
			    return (status);
		}
		++uindex;
		uindex = uindex % ucount;
		actuals_used++;			/* Track the parameters used */
		break;
	    }
	    ++uindex;
	    uindex = uindex % ucount;
	}
    }

    /*
    ** No more error checking is needed. Parameter checks were made on entry
    ** to the called procedure. 
    */

    return (E_DB_OK);
}

/*{
** Name: qee_update_byref_params	- Put byref parameters back
**					  where the came from.
**
** Description:
**	This routine will either copy parameters back to the calling routine
**	(if this is a nested call) by calling qee_update_nested_byrefs()
**	or fill out a tdesc or tdata message (if this comes from the front
**	nd) by calling qee_build_byref_tdesc() or qee_build_byref_tdata().
**	If this procedure call has been initiated from the front end then
**	the first time qee_update_byref_params() is called the tdesc message
**	will be filled out, the error E_QE0306_ALLOCATE_BYREF_TUPLE will then
**	be returned which will signal SCF to allocate the tdata message and
**	call back QEF. The second time through this routine the tdata message
**	will be filled out.
**	
** Inputs:
**	qef_rcb			QEF request block
**	dsh			QEF data segment structure
**
** Outputs:
**	qef_rcb
**	    .error.err_code	    one of the following
**				    E_QE0113_PARAM_CONVERSION
**				    E_QE0306_ALLOCATE_BYREF_TUPLE
**	Returns:
**	    {@return_description@}
**	Exceptions:
**	    [@description_or_none@]
**
** Side Effects:
**	    [@description_or_none@]
**
** History:
**      9-nov-92 (jhahn)
**          Created for DB procedures.
**	26-jul-1993 (jhahn)
**	    Moved coercing of returned byref procedure parameters from the
**	    back end to the front end. Split into qee_update_nested_byrefs(),
**	    qee_build_byref_tdesc() and qee_build_byref_tdata().
**	27-jul-1993 (jhahn)
**	    Replaced QEF_SAVED_RSRC with QEF_DBPROC_QP so as to be consistent
**	    with anitap's integration.
**	1-feb-99 (inkdo01)
**	    Just return, if this is a row-producing procedure.
*/
DB_STATUS
qee_update_byref_params(
	QEF_RCB		*qef_rcb,
	QEE_DSH		*dsh)
{
    DB_STATUS	status;
    
    if (dsh->dsh_stack != NULL)
    {
	return (qee_update_nested_byrefs(qef_rcb, dsh));
    }
    else if (dsh->dsh_qp_ptr->qp_status & QEQP_ROWPROC)
    {
	/* Call "byref_tdesc" to assure there are NO byref parms. */
	status = qee_build_byref_tdesc(qef_rcb, dsh);
	if (status) return (status);
	return (E_DB_OK);
    }
    else
    {
	if (qef_rcb->qef_output == NULL)
	{
	    return (E_DB_OK);
	}
	/* FIXME!! (jhahn) The use of QEF_DBPROC_QP was added to be consistent
	**         with anitap's integration. This is a temporary fix as my use
	**	   of this flag has nothing to do with dbproc qp's. A more
	**	   mnemonic solution should be introduced with the next
	**	   integration
	*/
	else if (qef_rcb->qef_intstate & QEF_DBPROC_QP)
	{
	    qef_rcb->qef_intstate &= ~QEF_DBPROC_QP;
	    return (qee_build_byref_tdata(qef_rcb, dsh));
	}
	else
	{
	    status = qee_build_byref_tdesc(qef_rcb, dsh);
	    if (status)
		return (status);
	    qef_rcb->qef_intstate |= QEF_DBPROC_QP;
	    qef_rcb->error.err_code = E_QE0306_ALLOCATE_BYREF_TUPLE;
	    return (E_DB_ERROR);
	}
    }
}

/*{
** Name: qee_update_nested_byrefs	- Put byref parameters back
**					  where the came from.
**
** Description:
**	This routine puts byref parameters back to where they were copied
**	from in qee_dbparam. qee_dbparam should have saved pointers to the
**	sourceof the byref parameters. Conversions will be done if necessary.
**	
** Inputs:
**	qef_rcb			QEF request block
**	dsh			QEF data segment structure
**
** Outputs:
**	qef_rcb
**	    .error.err_code	    one of the following
**				    E_QE0113_PARAM_CONVERSION
**	Returns:
**	    {@return_description@}
**	Exceptions:
**	    [@description_or_none@]
**
** Side Effects:
**	    [@description_or_none@]
**
** History:
**      26-july-1993 (jhahn)
**          Created from qee_update_byref_parameters.
*/
static DB_STATUS
qee_update_nested_byrefs(
	QEF_RCB		*qef_rcb,
	QEE_DSH		*dsh)
{
    QEF_QP_CB	    *qp = dsh->dsh_qp_ptr;
    QEF_CB	    *qef_cb = qef_rcb->qef_cb;
    ADF_CB	    *adf_cb = dsh->dsh_adf_cb;
    QEF_DBP_PARAM   *dbparm;
    DB_STATUS	    status;
    i4	    err;
    i4		    pcount, i;
    DB_DATA_VALUE   row_dbv, callers_dbv;
    DB_DATA_VALUE   *callers_dbvp;

    pcount = qp->qp_ndbp_params;    /* count of formal parameters in QP */

    /* Scan the QP array */
    for (i = 0; i < pcount; i++)
    {
	/* Get the current param in the QP.  Set up data value ptr to
	** the corresponding row buffer in the DSH */
	dbparm = qp->qp_dbp_params + i;
	STRUCT_ASSIGN_MACRO(dbparm->dbp_dbv, row_dbv);
	row_dbv.db_data = (PTR)((char *)(dsh->dsh_row[dbparm->dbp_rowno]) +
				dbparm->dbp_offset);
	callers_dbvp = (DB_DATA_VALUE *)
	    ((char *)(dsh->dsh_row[dbparm->dbp_byref_value_rowno]) +
	     dbparm->dbp_byref_value_offset);
	if (callers_dbvp->db_datatype != DB_NODT)
	{
	    /* A match is found.  Substitute user's parameter into
	    ** the corresponding DSH buffer */
	    status = adc_cvinto(adf_cb, 
				&row_dbv, callers_dbvp);
	    
	    if (status != E_DB_OK)
	    {
		if ((adf_cb->adf_errcb.ad_errcode == E_AD2009_NOCOERCION) ||
		    (adf_cb->adf_errcb.ad_errcode == E_AD2004_BAD_DTID))
		{
		    (VOID) qef_error(E_QE0113_PARAM_CONVERSION, 0L,
				     status, &err, &qef_cb->qef_rcb->error, 1,
				     qec_trimwhite(DB_MAXNAME,
						   dbparm->dbp_name),
				     dbparm->dbp_name);
		    return (status);
		}

		if ((status = qef_adf_error(&adf_cb->adf_errcb, 
			status, qef_cb, &qef_rcb->error)) != E_DB_OK)
		    return (status);
	    }
	}
    }
    return (E_DB_OK);
}

/*{
** Name: qee_build_byref_tdata		- Build a tdata for sending back byref
**					  parameters.
**
** Description:
**	This routine fills out a tuple data message for sending byref
**	parameters back to the front end. It will do no conversions. The
**	parameters will be sent back in the same format they were declared
**	as.
**	
** Inputs:
**	qef_rcb			QEF request block
**	dsh			QEF data segment structure
**
** Outputs:
**	qef_rcb
**	    .error.err_code	    one of the following
**				    E_QE0113_PARAM_CONVERSION
**	Returns:
**	    {@return_description@}
**	Exceptions:
**	    [@description_or_none@]
**
** Side Effects:
**	    [@description_or_none@]
**
** History:
**      26-july-1993 (jhahn)
**          Created from qee_update_byref_parameters.
**	13-jan-94 (rickh)
**	    Added a cast to the mecopy call.
**	22-june-06 (dougi)  SIR 116219
**	    Add support for IN, OUT, INOUT parameter modes.
*/
static DB_STATUS
qee_build_byref_tdata(
	QEF_RCB		*qef_rcb,
	QEE_DSH		*dsh)
{
    QEF_QP_CB	    	*qp = dsh->dsh_qp_ptr;
    DB_STATUS	    	status;
    i4			uindex;
    i4			ucount = qef_rcb->qef_pcount;
    char		*data_area = qef_rcb->qef_output->dt_data;
    char		*next_data;
    QEF_USR_PARAM	*uparm;
    
    next_data = (char *) data_area;
    /* Scan the QP array */
    for (uindex = 0; uindex < ucount; uindex++)
    {
	uparm = *(qef_rcb->qef_usr_param + uindex);
	if (uparm->parm_flags & (QEF_IS_BYREF | QEF_IS_OUTPUT))
	{
	    /* Get the current param in the QP.  Set up data value ptr to
	    ** the corresponding row buffer in the DSH */
	    QEF_DBP_PARAM *dbp = qp->qp_dbp_params + uparm->parm_index;
	    i4  dbp_length = dbp->dbp_dbv.db_length;

	    MEcopy((PTR) (dsh->dsh_row[dbp->dbp_rowno]) + dbp->dbp_offset,
		   dbp_length, ( PTR ) next_data);
	    next_data += dbp_length;
	}
    }

    if (next_data - data_area !=
	qef_rcb->qef_output->dt_size)
    {
	qef_rcb->error.err_code = E_QE0002_INTERNAL_ERROR;
	return (E_DB_ERROR);
    }
    return (E_DB_OK);
}

/*{
** Name: qee_build_byref_tdesc		- Build a tdesc for sending back byref
**					  parameters.
**
** Description:
**	This routine fills out a tuple data message for sending byref
**	parameters back to the front end. It will do no conversions. The
**	parameters will be sent back in the same format they were declared
**	as.
**	
** Inputs:
**	qef_rcb			QEF request block
**	dsh			QEF data segment structure
**
** Outputs:
**	qef_rcb
**	    .error.err_code	    one of the following
**				    E_QE0113_PARAM_CONVERSION
**	Returns:
**	    {@return_description@}
**	Exceptions:
**	    [@description_or_none@]
**
** Side Effects:
**	    [@description_or_none@]
**
** History:
**      26-july-1993 (jhahn)
**          Created from qee_update_byref_parameters.
**	3-feb-99 (inkdo01)
**	    Check for rowproc error.
**	21-Dec-01 (gordy)
**	    GCA_COL_ATT no longer defined with DB_DATA_VALUE: use DB_COPY macro.
**	21-june-06 (dougi)  SIR 116219
**	    Add support for IN, OUT, INOUT parameter modes.
*/
static DB_STATUS
qee_build_byref_tdesc(
	QEF_RCB		*qef_rcb,
	QEE_DSH		*dsh)
{
    QEF_QP_CB	    	*qp = dsh->dsh_qp_ptr;
    QEF_CB		*qef_cb = qef_rcb->qef_cb;
    DB_STATUS	    	status;
    i4			uindex;
    i4			ucount = qef_rcb->qef_pcount;
    GCA_COL_ATT		*next_descriptor;
    GCA_TD_DATA		*desc_area =
	(GCA_TD_DATA *) (qef_rcb->qef_output->dt_data);
    i4			tuple_size;
    QEF_USR_PARAM	*uparm;
    i4			err;
    
    next_descriptor = desc_area->gca_col_desc;
    tuple_size = 0;
    /* Scan the QP array */
    for (uindex = 0; uindex < ucount; uindex++)
    {
	uparm = *(qef_rcb->qef_usr_param + uindex);
	if (uparm->parm_flags & (QEF_IS_BYREF | QEF_IS_OUTPUT))
	{
	    /* Get the current param in the QP.  Set up data value ptr to
	    ** the corresponding row buffer in the DSH */
	    QEF_DBP_PARAM *dbp = qp->qp_dbp_params + uparm->parm_index;
	    DB_DATA_VALUE dbp_dbv;
	    i4  name_length;

	    /* But first make sure it's not a row producing procedure. */
	    if (qp->qp_status & QEQP_ROWPROC)
	    {
/* return(E_DB_OK); */
		status = E_DB_ERROR;
		(VOID) qef_error(E_QE030C_BYREF_IN_ROWPROC, 0L,
				     status, &err, &qef_cb->qef_rcb->error, 1,
				     qec_trimwhite(DB_MAXNAME,
						   qp->qp_id.db_cur_name),
				     qp->qp_id.db_cur_name);
		return (status);
	    }

	    STRUCT_ASSIGN_MACRO(dbp->dbp_dbv, dbp_dbv);
	    dbp_dbv.db_data = NULL;

	    DB_COPY_DV_TO_ATT( &dbp_dbv, next_descriptor );
#ifdef BYTE_ALIGN
	    MECOPY_CONST_MACRO((PTR) &next_descriptor->gca_l_attname,
			       sizeof(i4),
			       (PTR) &name_length);
#else
	    name_length = next_descriptor->gca_l_attname;
#endif
	    next_descriptor = (GCA_COL_ATT *)
		((char *)next_descriptor + name_length +
		 sizeof(next_descriptor->gca_attdbv) + 
		 sizeof(next_descriptor->gca_l_attname));
	    tuple_size += dbp_dbv.db_length;
	}
    }

    /* If we get this far and it's a row producing proc, just leave. */
    if (qp->qp_status & QEQP_ROWPROC) return(E_DB_OK);

    if ((char *) next_descriptor - (char *) desc_area !=
	qef_rcb->qef_output->dt_size)
    {
	qef_rcb->error.err_code = E_QE0002_INTERNAL_ERROR;
	return (E_DB_ERROR);
    }
    desc_area->gca_tsize = tuple_size;

    return (E_DB_OK);
}

/*{
** Name: qee_hashInit	- Initialize base structure for hash join.
**
** Description:
**	Persistent hash join info is initialized here. More transient
**	stuff (which varies from execution to execution) is initialized
**	in qee_fetch.
**
**	The most ad-hoc task here is allocating memory.  Hash joining
**	runs fastest if we can keep from spilling any of the build
**	partitions.  Once we spill, it's reasonable to assume that all
**	the partitions will spill more or less evenly, and the next
**	best is to try to fit one partition's worth in memory.
**	That is hard to figure, so we basically use what is available
**	in that case.  The calculations are quite arbitrary.
**
**	The ideal way to proceed would be to allocate ALL of the
**	query's hash joins together, so that big joins could get lots
**	of memory.  This is (as yet) not feasible, so assume that
**	each join gets an even fraction of available hash memory.
**
**	Caution: not all of the DSH stuff (like dsh_qefcb etc) is set
**	up at this point.  This is called during DSH initialization.
**
** Inputs:
**	qef_rcb			QEF request block
**	dsh			QEF data segment structure
**
** Outputs:
**	qef_rcb
**	    .error.err_code
**	Returns:
**	    {@return_description@}
**	Exceptions:
**	    [@description_or_none@]
**
** Side Effects:
**	    [@description_or_none@]
**
** History:
**      3-mar-99 (inkdo01)
**	    Written for hash join.
**	20-dec-99 (inkdo01)
**	    Changes for table driven memory computation and more accurate
**	    partition, subpartition count determination.
**	jan-00 (inkdo01)
**	    Changes for cartprod logic.
**	7-feb-00 (inkdo01)
**	    Added flag for CO joins.
**	22-aug-01 (inkdo01)
**	    prepare HASH_CHECKNULL flag.
**	3-oct-01 (inkdo01)
**	    Fix key cols to look like a single BYTE(n) col (if no nullables).
**	13-jan-03 (inkdo01)
**	    Add 2nd hash workrow for enhanced CP algorithm.
**	17-jan-03 (inkdo01)
**	    Fix SEGV when build/probe source row size is bigger than proposed
**	    buffer size (bug 109481).
**	5-sep-04 (inkdo01)
**	    Add hsh_brptr/prptr to save row array addrs across resets for 
**	    global base array support.
**	14-dec-04 (inkdo01)
**	    Replace unused cmp_flags by cmp_collID (for collations).
**	6-Jun-2005 (schka24)
**	    Refine memory usage calculations.
**	31-Aug-2005 (schka24)
**	    Use large bit-vector even for small joins.
**	7-Dec-2005 (kschendel)
**	    Compare-list is built into the node now.
*/
static DB_STATUS
qee_hashInit(
	QEF_RCB		*qef_rcb,
	QEE_DSH		*dsh,
	QEN_NODE	*hnode,
	ULM_RCB		*ulm)
{
    QEF_QP_CB	*qp = dsh->dsh_qp_ptr;
    QEF_CB	*qefcb = qef_rcb->qef_cb;
    QEN_HJOIN	*hjoin = &hnode->node_qen.qen_hjoin;
    i4		i, nhash;
    QEN_HASH_BASE	*hbase = dsh->dsh_hash[hjoin->hjn_hash];
    i4		rowsz, buildmem;
    i4		buildrows, htsize, bufsize;
    i4		availmem, needmem;
    i4		browsz;
    i4		browsperpart;
    GLOBALREF QEF_S_CB *Qef_s_cb;
    DB_STATUS	status;

    /* Figure the available hash memory.
    ** Allow for N hash joins, which isn't really right, since this doesn't
    ** take threading into account, nor serializing points (sorts),
    ** nor the possibility that some joins are larger than others!
    ** FIXME what about PC joins, is bmem right, or too big?
    ** FIXME might be better if opc passed brows, not bmem.
    */

    buildmem = hjoin->hjn_bmem;
    nhash = qp->qp_hash_cnt;
    if (nhash <= 0) nhash = 1;
    if (qp->qp_status & QEQP_PARALLEL_PLAN)
	nhash = nhash * 4;		/* bogus, FIXME */
    nhash = nhash + qp->qp_hashagg_cnt;
    availmem = qefcb->qef_h_max / nhash;
    /* Deflate available memory slightly to allow for overage */
    availmem = (availmem / 11) * 10;

    /* Try to fit the build side (inflated by 20%) into available memory.
    ** Don't forget to allow for hash pointers, overheads.  Allow
    ** for 300K of overhead memory (an I/O buffer, file contexts, partition
    ** descriptors).
    */
    browsz = qp->qp_row_len[hjoin->hjn_brow];
    /* OPC should be aligning these, but just make sure */
    hbase->hsh_browsz = browsz = DB_ALIGN_MACRO(browsz);
    rowsz = qp->qp_row_len[hjoin->hjn_prow];
    hbase->hsh_prowsz = DB_ALIGN_MACRO(rowsz);
    if (buildmem < MAXI4 / 2)
	buildmem = buildmem + buildmem / 5;	/* Add 20% */
    else
	buildmem = MAXI4 / 2;		/* Avoid absurd sizes */
    buildrows = buildmem / browsz;
    htsize = ((buildrows * 11) / 10) * sizeof(PTR);
    needmem = buildmem + htsize + 300000;
    if (needmem < availmem)
    {
	/* It fits.  Inflate memory a bit more, just in case, as
	** long as there's plenty of room.
	*/
	if (needmem < (availmem / 3) * 2)
	    needmem = needmem + needmem / 10;
	if (needmem < 512 * 1024)
	    needmem = 512 * 1024;	/* arbitrary floor of 512K */
    }
    else
    {
	/* Can't fit the build side in memory.  Do what we can. */

	/* Take another 10% fudge off of available memory, and
	** take a maximum if it's still pretty big.  (Reeealy big
	** joins will spill to disk and squeezing a few more Mb
	** just risks QEF-out-of-memory with little benefit.)
	*/
	availmem = (availmem / 11) * 10;
	if (availmem > 64*1024*1024)
	    availmem = 64 * 1024 * 1024;	/* Plucked from the air */
	if (availmem < 1024 * 1024)
	    availmem = 1024 * 1024;		/* 1 Mb floor for large join */
	needmem = availmem;
    }
    hbase->hsh_msize = needmem;
    /* Solve back for # build rows given memory we'll use, then figure
    ** out memory for just the partition buffers.
    */
    buildrows = (needmem - 300000) / (browsz + (11*sizeof(PTR))/10);
    needmem = buildrows * browsz;

    /* Number of partitions and partition buffer size aren't all that
    ** critical, as long as we total up to the desired number of rows.
    ** There's a mild preference for larger buffers, to allow for the
    ** case where the input distribution is not even, but that's
    ** a bit tricky to do, so I don't try too hard.
    */
    bufsize = 16 * 1024;
    /* Make sure either kind of row fits */
    while (bufsize < browsz || bufsize < hbase->hsh_prowsz)
	bufsize = bufsize << 1;
    /* Use larger buffers for large joins.  More ad-hoc-ery. */
    while (bufsize * 200 < needmem && bufsize < 128*1024)
    {
	/* 16, 32, 64, 96, 128 for no particular reason */
	if (bufsize == 16*1024)
	    bufsize = 32*1024;
	else
	    bufsize += 32*1024;
    }

    /* Find the number of partitions to use.
    ** If we hit the max, crank up the buffer size and try again.
    */
    for (;;)
    {
	browsperpart = bufsize / browsz;
	i = 0;
	while (primes[i]*browsperpart < buildrows && primes[i] != 0)
	    ++i;
	if (primes[i] != 0)
	    break;			/* We got an answer */
	bufsize += 32 * 1024;		/* Try a larger buffer */
    }
    hbase->hsh_csize = bufsize;
    hbase->hsh_pcount = primes[i];

    /* Figure out bit-filter size.  Originally this was a function of the
    ** number of build rows-per-partition, but it turns out that we want
    ** large bit-filters even for small joins.  If OPF has booger'ed a
    ** row estimate in the low direction, the bit-filter might be the only
    ** thing that saves us from hours-long joins.
    ** Make it a multiple of 8 so join can use word-clearing tricks.
    */

    hbase->hsh_bvsize = 2048 * 8;
    if (hbase->hsh_pcount > 150)
	hbase->hsh_bvsize = 1024 * 8;	/* Smaller if lotsa partitions */

    hbase->hsh_brptr = dsh->dsh_row[hjoin->hjn_brow];
    hbase->hsh_prptr = dsh->dsh_row[hjoin->hjn_prow];
    hbase->hsh_dmhcb = dsh->dsh_cbs[hjoin->hjn_dmhcb];
    hbase->hsh_kcount = hjoin->hjn_hkcount;
    hbase->hsh_streamid = (PTR) NULL;
    hbase->hsh_rbptr = (PTR) NULL;
    hbase->hsh_maxreclvl = 0;
    hbase->hsh_bwritek = 0;
    hbase->hsh_pwritek = 0;
    hbase->hsh_memparts = 0;
    hbase->hsh_rpcount = 0;
    hbase->hsh_bvdcnt = 0;
    hbase->hsh_flags = 0;
    hbase->hsh_nodep = hjoin;
    hbase->hsh_count1 = 0;
    hbase->hsh_count2 = 0;
    hbase->hsh_count3 = 0;
    hbase->hsh_count4 = 0;

    /* If outer join, set flags. */
    if (hjoin->hjn_oj)
    switch (hjoin->hjn_oj->oj_jntype)
    {
     case DB_LEFT_JOIN:
	hbase->hsh_flags |= HASH_WANT_LEFT;
	break;

     case DB_RIGHT_JOIN:
	hbase->hsh_flags |= HASH_WANT_RIGHT;
	break;

     case DB_FULL_JOIN:
	hbase->hsh_flags |= (HASH_WANT_LEFT | HASH_WANT_RIGHT);
	break;

     default: break;
    }

    /* Translate new nullable-key flag to old */
    if (hjoin->hjn_nullablekey)
	hbase->hsh_flags |= HASH_CHECKNULL;

    /* Allocate array of level 0 partition descriptors, plus the
    ** level 0 recursion link structure and the buffer for assembling 
    ** rows which split across partition blocks.
    **
    ** Note: this stuff is allocated in the DSH stream, not the new 
    ** stream. If recursive partitioning is required, subsequent LINK and
    ** PARTITION structures will be allocated in the new stream, as well
    ** as the partition buffers and hash bucket array.
    */
    rowsz = hbase->hsh_browsz;
    if (rowsz < hbase->hsh_prowsz)
	rowsz = hbase->hsh_prowsz;
    ulm->ulm_psize = hbase->hsh_pcount * sizeof (QEN_HASH_PART) +
		sizeof (QEN_HASH_LINK) + rowsz + rowsz;
    status = qec_malloc(ulm);
    if (status != E_DB_OK)
    {
	qef_rcb->error.err_code = ulm->ulm_error.err_code;
	return (status);
    }
    hbase->hsh_workrow = ulm->ulm_pptr;
    hbase->hsh_workrow2 = (PTR)&((char *)ulm->ulm_pptr)[rowsz];
    hbase->hsh_currlink = (QEN_HASH_LINK *)&((char *)ulm->ulm_pptr)[2*rowsz];
    hbase->hsh_toplink = hbase->hsh_currlink;
    hbase->hsh_toplink->hsl_partition = 
	(QEN_HASH_PART *)&((char *)hbase->hsh_toplink)[sizeof(QEN_HASH_LINK)];
				/* partition array follows link struct */

    return(E_DB_OK);
}


/*{
** Name: qee_hashaggInit	- Initialize base structure for hash aggregate.
**
** Description:
**	Persistent hash aggregation info is initialized here. More transient
**	stuff (which varies from execution to execution) is initialized
**	in qea_haggf upon the first execution of the action.
**	
** Inputs:
**	qef_rcb			QEF request block
**	dsh			QEF data segment structure
**	qepp			Ptr to action specific action header structure
**	ulm			Ptr to memory mgmt control block
**
** Outputs:
**	qef_rcb
**	    .error.err_code
**	Returns:
**	    {@return_description@}
**	Exceptions:
**	    [@description_or_none@]
**
** Side Effects:
**	    [@description_or_none@]
**
** History:
**      26-jan-01 (inkdo01)
**	    Written for hash aggregation.
**	9-feb-01 (inkdo01)
**	    Minor changes to sync with qefdsh.h and qefact.h changes.
**	29-mar-01 (inkdo01)
**	    Various additions to support hash table overflow handling.
**	2-oct-01 (inkdo01)
**	    Minor adjustments to chunk size to allow for pointer chain.
**	4-oct-01 (inkdo01)
**	    Turn keylist into a single aggregate BYTE coplumn for ease
**	    of comparison.
**	31-may-02 (inkdo01)
**	    Change minimum value of hshag_chsize to be hshag_pbsize, so 
**	    overflow routine recurses at the right time.
**	31-aug-04 (inkdo01)
**	    Changes to support global base arrays.
**	14-dec-04 (inkdo01)
**	    Replace unused cmp_flags by cmp_collID (for collations).
**	5-May-2005 (schka24)
**	    Use true pointer size, not align-restrict, for chunk calculations
**	    (align-restrict need not be as big as a pointer).
**	6-Jun-2005 (schka24)
**	    Rework memory allocation estimates to give more memory to
**	    large aggregations.
**	30-Jun-2005 (schka24)
**	    Clip giant hash agg estimate sizes.
**	18-Oct-2005 (hweho01)
**	    Modified the chunk_size assignment, so it will be rounded up  
**          to the sizeof(ALIGN_RESTRICT).
**	7-Dec-2005 (kschendel)
**	    Hash-agg action header now points to (concatenated) key
**	    compare entry, don't need to build it here.
**	27-Jun-2006 (kschendel)
**	    Adjust number of partitions down if dictated by low available mem.
**	29-Aug-2006 (kschendel)
**	    Clipping memory usage at 32M is a bad idea, try something else.
*/
static DB_STATUS
qee_hashaggInit(
        QEF_RCB             *qef_rcb,
        QEE_DSH             *dsh,
        QEF_QEP		    *qepp,
	ULM_RCB		    *ulm)

{
    QEN_HASH_AGGREGATE *haggp = dsh->dsh_hashagg[qepp->u1.s2.ahd_agcbix];
    QEF_QP_CB	*qp = dsh->dsh_qp_ptr;
    QEF_CB	*qefcb = qef_rcb->qef_cb;
    i4		off = 0, rowsz;
    i4		i, index, chunk_size, htsize, rows_per_chunk;
    i4		aggest, nhash, available;
    i4		nparts_ix;
    DB_STATUS	status;
    bool	globalbase;

    /* Nice looking prime numbers.  We could use the same table as hash
    ** join, but hash agg runs differently and prefers many partitions.
    */
    static i4 aggparts [] = {23, 61, 157, 317};

    /* Most of the fields in the hash aggregation structure are simply 
    ** set to 0. However, the "by expression" attribute descriptors are
    ** used to generate sort key descriptors and the hash table size 
    ** and chunk sizes are also set. */

    haggp->hshag_htable = NULL;
    haggp->hshag_streamid = NULL;
    haggp->hshag_1stchunk = NULL;
    haggp->hshag_toplink = haggp->hshag_currlink = (QEN_HASH_LINK *) NULL;
    haggp->hshag_openfiles = haggp->hshag_freefile = (QEN_HASH_FILE *) NULL;
    haggp->hshag_lastrow = (QEF_HASH_DUP *) NULL;
    haggp->hshag_rbptr = (PTR) NULL;
    haggp->hshag_dmhcb = dsh->dsh_cbs[qepp->u1.s2.ahd_agdmhcb];
    haggp->hshag_halloc = 0;
    haggp->hshag_rcount = 0;
    haggp->hshag_gcount = 0;
    haggp->hshag_ocount = 0;
    haggp->hshag_fcount = 0;
    haggp->hshag_curr_bucket = -1;
    haggp->hshag_flags = 0;

    /* Just one concatenated key, put compare-list entry ptr where
    ** hashagg code can find it easily.
    */
    haggp->hshag_keycount = 1;
    haggp->hshag_keylist = qepp->u1.s2.ahd_aghcmp;

    /* Set buffer sizes (agg. work, hash, overflow). */
    globalbase = (dsh->dsh_qp_ptr->qp_status & 
			QEQP_GLOBAL_BASEARRAY) ? TRUE : FALSE;
    index = (globalbase) ? qepp->u1.s2.ahd_agwbuf :
	qepp->ahd_current->qen_base[
		qepp->u1.s2.ahd_agwbuf-ADE_ZBASE].qen_index;
    haggp->hshag_wbufsz = dsh->dsh_qp_ptr->qp_row_len[index];
    index = (globalbase) ? qepp->u1.s2.ahd_aghbuf :
	qepp->ahd_by_comp->qen_base[
		qepp->u1.s2.ahd_aghbuf-ADE_ZBASE].qen_index;
    haggp->hshag_hbufsz = dsh->dsh_qp_ptr->qp_row_len[index];
    index = (globalbase) ? qepp->u1.s2.ahd_agobuf :
	qepp->u1.s2.ahd_agoflow->qen_base[
		qepp->u1.s2.ahd_agobuf-ADE_ZBASE].qen_index;
    haggp->hshag_obufsz = dsh->dsh_qp_ptr->qp_row_len[index];

    /* Lots of partitions if lots of input expected.  With luck, we
    ** won't need 'em, but we will if there are lots of output groups.
    ** Unlike hash join, hash agg partition buffers are just I/O holders,
    ** so more partitions beats bigger ones.
    */
    aggest = qepp->u1.s2.ahd_aggest;
    haggp->hshag_pbsize = HASHAG_PBSIZE;
    nparts_ix = 0;
    if (aggest > 100000)
	nparts_ix = 3;		/* pretty bogus code here */
    else if (aggest > 25000)
	nparts_ix = 2;
    else if (aggest > 5000)
	nparts_ix = 1;
    haggp->hshag_pcount = aggparts[nparts_ix];

    /* Hash table size should be allocated based on capacity of
    ** chunk space. That is, however many chunks are to be allowed
    ** times the number of work rows in each times 1.1 should be
    ** the size of the hash table. */

    /* Compute "chunk" size (blocks in which work buffers are 
    ** held) and the max. permissable number of them. The idea 
    ** is to allocate blocks with enough room to hold at least
    ** 100 work rows, with minimum size equal to the partition 
    ** buffer size (this is required for the proper functioning
    ** of the overflow/recursion logic).
    **
    ** The chunk link pointer is at the front, so we don't care about
    ** alignment beyond that (each chunk is allocated separately, so
    ** the starts will be aligned).
    */

    chunk_size = 100 * haggp->hshag_wbufsz;
    if (chunk_size > haggp->hshag_pbsize)
					/* for the ptr chain thru chunks */
	chunk_size += (sizeof(PTR) + DB_ALIGN_SZ -1) & (~(DB_ALIGN_SZ -1));	
    else
	chunk_size = haggp->hshag_pbsize;

    haggp->hshag_chsize = chunk_size;
    rows_per_chunk = (chunk_size-sizeof(PTR)) / haggp->hshag_wbufsz;

    /* Total memory is roughly:
    ** M = 1.1*R*N*sizeof(PTR) + N*C + 2*obufsz + Np*(P+Ovh)
    ** Where: R = rows per chunk, N = number of chunks, C = chunk size,
    ** Np = number of partitions (chosen based on size estimate, above),
    ** P = partition buffer size, Ovh = per-partition overhead (a partition
    ** descriptor and a file context).
    ** Solving for number of chunks N:
    ** N = (M - 2*obufsz - Np*(P+Ovh)) / (1.1*R*sizeof(PTR) + C)
    ** given an M that is our (conservative) share of qef_h_max.
    **
    ** But, for really puny aggregations, under 100 estimated groups,
    ** skip the fancy math and just take N = 20.  (allows for at least
    ** 2000 groups and typically takes about 200K of memory.)
    */

    if (aggest <= 100)
	haggp->hshag_chmax = 20;
    else
    {
	nhash = qp->qp_hash_cnt;
	if (nhash <= 0) nhash = 1;
	if (qp->qp_status & QEQP_PARALLEL_PLAN)
	    nhash = nhash * 4;		/* bogus, FIXME */
	nhash = nhash + qp->qp_hashagg_cnt;

	available = qefcb->qef_h_max / nhash;
	/* We might have picked a number of chunks that can't be fit
	** within the allowed memory, try fewer chunks as needed.
	*/
	for (;;)
	{
	    i = 2*haggp->hshag_obufsz
		+ haggp->hshag_pcount * (haggp->hshag_pbsize + sizeof(QEN_HASH_PART)
				+ sizeof(QEN_HASH_FILE) + sizeof(SR_IO));
	    if (available > i || nparts_ix == 0) break;
	    --nparts_ix;
	    haggp->hshag_pcount = aggparts[nparts_ix];
	}
	haggp->hshag_chmax = (available - i)
		/ ((11*rows_per_chunk*sizeof(PTR))/10 + haggp->hshag_chsize);
	if (haggp->hshag_chmax <= 0)
	    haggp->hshag_chmax = 10;
	/* This is awkward.  If we simply follow the calculation, we try
	** to use as much memory as is available;  this sounds plausible,
	** but it means that the hash table ends up being really big, and
	** it's allocated all at once.  So lots of hash-agg queries running
	** at once can suck memory dry.  More or less arbitrarily, clip
	** the hash table size to 10 meg.  Also, if we have more chunks
	** than aggest suggests output groups, reduce chmax.  (Aggest
	** is very close to a wild guess, but having more than 100x the
	** wild guess seems excessive.)
	*/
	if (haggp->hshag_chmax > aggest)
	    haggp->hshag_chmax = aggest;
	i = (10 * 1024 * 1024) / (sizeof(PTR) * rows_per_chunk);
	if (haggp->hshag_chmax > i)
	    haggp->hshag_chmax = i;
    }

    /* Calculate hash table size (in entries) and max # entries allowed */

    haggp->hshag_htmax = rows_per_chunk * haggp->hshag_chmax;
    haggp->hshag_htsize = (haggp->hshag_htmax * 11) / 10;

    /* Allocate array of level 0 partition descriptors, plus the
    ** level 0 recursion link structure and the buffer for assembling 
    ** rows which split across partition blocks.
    **
    ** Note: this stuff is allocated in the DSH stream, not the new 
    ** stream. If recursive partitioning is required, subsequent LINK and
    ** PARTITION structures will be allocated in the new stream, as well
    ** as the chunks, partition buffers and hash bucket array.
    **
    ** The work row comes first, so align its size so that the next thing
    ** (the top hash link structure) is aligned.
    */
    rowsz = haggp->hshag_obufsz;
    rowsz = DB_ALIGN_MACRO(rowsz);
    ulm->ulm_psize = haggp->hshag_pcount * sizeof (QEN_HASH_PART) +
		sizeof (QEN_HASH_LINK) + rowsz;
    status = qec_malloc(ulm);
    if (status != E_DB_OK)
    {
	qef_rcb->error.err_code = ulm->ulm_error.err_code;
	return (status);
    }
    haggp->hshag_workrow = ulm->ulm_pptr;
    haggp->hshag_currlink = (QEN_HASH_LINK *)&((char *)ulm->ulm_pptr)[rowsz];
    haggp->hshag_toplink = haggp->hshag_currlink;
    haggp->hshag_toplink->hsl_partition = 
	(QEN_HASH_PART *)&((char *)haggp->hshag_toplink)[sizeof(QEN_HASH_LINK)];
				/* partition array follows link struct */

    return(E_DB_OK);
}

/*
** Name - qee_resolve_xaddrs -- Resolve subtree execution time addresses
**
** Description:
**
**	Many of the execution time addresses used in a query plan are
**	unchanging throughout the life of the DSH (including repeated
**	executions).  Some of them, particularly ADE_EXCB addresses,
**	are tedious to find at runtime:  we have to get a QEN_ADF
**	address, see if it's null, get the CB number, and index into
**	the dsh_cbs array.  The QEE_XADDRS structure for a node is
**	just a way to hold resolved, commonly-used addresses for
**	that node to make things a little easier.
**
**	This routine walks a node QP subtree, resolving ADE_EXCB
**	addresses into the XADDRS structures as it goes.  Other
**	XADDRS things are not resolved here.  The ADE_EXCB addresses
**	are tricky since they are very node-type dependent.
**
**	Note!  We assume that the XADDRS structures were initially
**	cleared, and that there is no need to explicitly null out
**	things that might not be in use for any given node.
**
**	The caller can specify whether to walk through QP nodes into
**	the QP subquery actions, or not.  QEE setup time doesn't need
**	to delve below the QP because it hits all the QP actions
**	recursively.  EXCH child setup does need to follow below
**	the QP since nobody else will do it.
**
** Inputs:
**	dsh			The (child) thread's DSH
**	node			Topmost node to resolve
**	follow_qp		TRUE to resolve below QP's, FALSE to not.
**
** Outputs:
**	None
**
**	Each node's QEE_XADDRS control block is filled in as needed.
**
** History:
**	16-Jan-2006 (kschendel)
**	    Written.
**
*/

/* Local structdef solely for resolving ADE_EXCB addresses. */

struct _qee_xoffsets {
    size_t	qen_adf_offset;		/* within qen_node or qen_ojinfo */
    size_t	qee_xaddrs_offset;
};

/* One array of _qee_xoffsets for each node type that we have to deal
** with.  Since size_t is unsigned, and zero is a valid offset, the
** driver code will have to compute the number of elements (via the
** usual array size / struct size calculation).
*/

/* Outer join stuff common to many of the join nodes */
static const struct _qee_xoffsets qee_oj_xoffsets[] = {
    {CL_OFFSETOF(QEN_OJINFO, oj_oqual), CL_OFFSETOF(QEE_XADDRS, qex_onqual)},
    {CL_OFFSETOF(QEN_OJINFO, oj_equal), CL_OFFSETOF(QEE_XADDRS, qex_eqmat)},
    {CL_OFFSETOF(QEN_OJINFO, oj_rnull), CL_OFFSETOF(QEE_XADDRS, qex_rnull)},
    {CL_OFFSETOF(QEN_OJINFO, oj_lnull), CL_OFFSETOF(QEE_XADDRS, qex_lnull)}
};

/* Simple join: CP join, FSM join, PSM join */
static const struct _qee_xoffsets qee_sjn_xoffsets[] = {
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_sjoin.sjn_itmat),
		CL_OFFSETOF(QEE_XADDRS, qex_itmat)},
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_sjoin.sjn_okmat),
		CL_OFFSETOF(QEE_XADDRS, qex_okmat)},
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_sjoin.sjn_okcompare),
		CL_OFFSETOF(QEE_XADDRS, qex_kcompare)},
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_sjoin.sjn_joinkey),
		CL_OFFSETOF(QEE_XADDRS, qex_joinkey)},
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_sjoin.sjn_jqual),
		CL_OFFSETOF(QEE_XADDRS, qex_jqual)},
    {CL_OFFSETOF(QEN_NODE, qen_fatts), CL_OFFSETOF(QEE_XADDRS, qex_fatts)},
    {CL_OFFSETOF(QEN_NODE, qen_prow), CL_OFFSETOF(QEE_XADDRS, qex_prow)}
};

/* K-join */
static const struct _qee_xoffsets qee_kjoin_xoffsets[] = {
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_kjoin.kjoin_key),
		CL_OFFSETOF(QEE_XADDRS, qex_keyprep)},
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_kjoin.kjoin_kcompare),
		CL_OFFSETOF(QEE_XADDRS, qex_kcompare)},
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_kjoin.kjoin_kqual),
		CL_OFFSETOF(QEE_XADDRS, qex_joinkey)},
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_kjoin.kjoin_jqual),
		CL_OFFSETOF(QEE_XADDRS, qex_jqual)},
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_kjoin.kjoin_iqual),
		CL_OFFSETOF(QEE_XADDRS, qex_origqual)},
    {CL_OFFSETOF(QEN_NODE, qen_fatts), CL_OFFSETOF(QEE_XADDRS, qex_fatts)},
    {CL_OFFSETOF(QEN_NODE, qen_prow), CL_OFFSETOF(QEE_XADDRS, qex_prow)}
};

/* Hash join */
static const struct _qee_xoffsets qee_hjn_xoffsets[] = {
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_hjoin.hjn_btmat),
		CL_OFFSETOF(QEE_XADDRS, qex_otmat)},
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_hjoin.hjn_ptmat),
		CL_OFFSETOF(QEE_XADDRS, qex_itmat)},
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_hjoin.hjn_jqual),
		CL_OFFSETOF(QEE_XADDRS, qex_jqual)},
    {CL_OFFSETOF(QEN_NODE, qen_fatts), CL_OFFSETOF(QEE_XADDRS, qex_fatts)},
    {CL_OFFSETOF(QEN_NODE, qen_prow), CL_OFFSETOF(QEE_XADDRS, qex_prow)}
};

/* T-join */
static const struct _qee_xoffsets qee_tjoin_xoffsets[] = {
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_tjoin.tjoin_isnull),
		CL_OFFSETOF(QEE_XADDRS, qex_prequal)},
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_tjoin.tjoin_jqual),
		CL_OFFSETOF(QEE_XADDRS, qex_jqual)},
    {CL_OFFSETOF(QEN_NODE, qen_fatts), CL_OFFSETOF(QEE_XADDRS, qex_fatts)},
    {CL_OFFSETOF(QEN_NODE, qen_prow), CL_OFFSETOF(QEE_XADDRS, qex_prow)}
};

/* SE-join */
static const struct _qee_xoffsets qee_sejn_xoffsets[] = {
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_sejoin.sejn_itmat),
		CL_OFFSETOF(QEE_XADDRS, qex_itmat)},
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_sejoin.sejn_okmat),
		CL_OFFSETOF(QEE_XADDRS, qex_okmat)},
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_sejoin.sejn_ccompare),
		CL_OFFSETOF(QEE_XADDRS, qex_ccompare)},
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_sejoin.sejn_kcompare),
		CL_OFFSETOF(QEE_XADDRS, qex_kcompare)},
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_sejoin.sejn_kqual),
		CL_OFFSETOF(QEE_XADDRS, qex_joinkey)},
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_sejoin.sejn_oqual),
		CL_OFFSETOF(QEE_XADDRS, qex_prequal)},
    {CL_OFFSETOF(QEN_NODE, qen_fatts), CL_OFFSETOF(QEE_XADDRS, qex_fatts)},
    {CL_OFFSETOF(QEN_NODE, qen_prow), CL_OFFSETOF(QEE_XADDRS, qex_prow)}
};

/* Sort nodes (not top sorts) */
static const struct _qee_xoffsets qee_sort_xoffsets[] = {
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_sort.sort_mat),
		CL_OFFSETOF(QEE_XADDRS, qex_otmat)},
    {CL_OFFSETOF(QEN_NODE, qen_fatts), CL_OFFSETOF(QEE_XADDRS, qex_fatts)},
    {CL_OFFSETOF(QEN_NODE, qen_prow), CL_OFFSETOF(QEE_XADDRS, qex_prow)}
};

/* Top sort nodes */
static const struct _qee_xoffsets qee_tsort_xoffsets[] = {
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_tsort.tsort_mat),
		CL_OFFSETOF(QEE_XADDRS, qex_otmat)},
    {CL_OFFSETOF(QEN_NODE, qen_fatts), CL_OFFSETOF(QEE_XADDRS, qex_fatts)},
    {CL_OFFSETOF(QEN_NODE, qen_prow), CL_OFFSETOF(QEE_XADDRS, qex_prow)}
};

/* Orig nodes.  (The qualification CX address here is rarely used.
** More commonly, orig qualification is done in DMF via call-back to
** QEF, and the call-back doesn't need this stuff.)
*/
static const struct _qee_xoffsets qee_orig_xoffsets[] = {
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_orig.orig_qual),
		CL_OFFSETOF(QEE_XADDRS, qex_origqual)},
    {CL_OFFSETOF(QEN_NODE, qen_fatts), CL_OFFSETOF(QEE_XADDRS, qex_fatts)},
    {CL_OFFSETOF(QEN_NODE, qen_prow), CL_OFFSETOF(QEE_XADDRS, qex_prow)}
};

/* QP nodes */
static const struct _qee_xoffsets qee_qp_xoffsets[] = {
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_qp.qp_qual),
		CL_OFFSETOF(QEE_XADDRS, qex_jqual)},
    {CL_OFFSETOF(QEN_NODE, qen_fatts), CL_OFFSETOF(QEE_XADDRS, qex_fatts)},
    {CL_OFFSETOF(QEN_NODE, qen_prow), CL_OFFSETOF(QEE_XADDRS, qex_prow)}
};

/* Exchange nodes.  At present, the resolved "materialize" CX isn't
** referenced this way, but we'll fill it in anyway.
*/
static const struct _qee_xoffsets qee_exch_xoffsets[] = {
    {CL_OFFSETOF(QEN_NODE, node_qen.qen_exch.exch_mat),
		CL_OFFSETOF(QEE_XADDRS, qex_otmat)},
    {CL_OFFSETOF(QEN_NODE, qen_fatts), CL_OFFSETOF(QEE_XADDRS, qex_fatts)},
    {CL_OFFSETOF(QEN_NODE, qen_prow), CL_OFFSETOF(QEE_XADDRS, qex_prow)}
};


void
qee_resolve_xaddrs(QEE_DSH *dsh,
	QEN_NODE *node, bool follow_qp)
{

    i4 nxo;			/* Number of xoffsets entries */
    PTR *cbs = dsh->dsh_cbs;
    QEE_XADDRS *node_xaddrs = dsh->dsh_xaddrs[node->qen_num];
    QEN_ADF *qadf;
    QEN_NODE *outer, *inner;	/* Subnode pointers */
    QEN_OJINFO *oj;		/* Outer join info if any */
    const struct _qee_xoffsets *xo;	/* Offsets to fix up */

    /* Set up table pointer by node type:
    ** Offset array and number of entries;  OJINFO pointer if any;
    ** outer child pointer, inner child pointer.
    */

    switch (node->qen_type)
    {
    case QE_CPJOIN:
    case QE_FSMJOIN:
    case QE_ISJOIN:
	/* These three are "simple join". */
	xo = &qee_sjn_xoffsets[0];
	nxo = sizeof(qee_sjn_xoffsets) / sizeof(struct _qee_xoffsets);
	oj = node->node_qen.qen_sjoin.sjn_oj;
	outer = node->node_qen.qen_sjoin.sjn_out;
	inner = node->node_qen.qen_sjoin.sjn_inner;
	break;

    case QE_EXCHANGE:
	xo = &qee_exch_xoffsets[0];
	nxo = sizeof(qee_exch_xoffsets) / sizeof(struct _qee_xoffsets);
	oj = NULL;
	outer = node->node_qen.qen_exch.exch_out;
	inner = NULL;
	break;

    case QE_HJOIN:
	xo = &qee_hjn_xoffsets[0];
	nxo = sizeof(qee_hjn_xoffsets) / sizeof(struct _qee_xoffsets);
	oj = node->node_qen.qen_hjoin.hjn_oj;
	outer = node->node_qen.qen_hjoin.hjn_out;
	inner = node->node_qen.qen_hjoin.hjn_inner;
	break;

    case QE_KJOIN:
	xo = &qee_kjoin_xoffsets[0];
	nxo = sizeof(qee_kjoin_xoffsets) / sizeof(struct _qee_xoffsets);
	oj = node->node_qen.qen_kjoin.kjoin_oj;
	outer = node->node_qen.qen_kjoin.kjoin_out;
	inner = NULL;
	break;

    case QE_ORIG:
    case QE_ORIGAGG:
	xo = &qee_orig_xoffsets[0];
	nxo = sizeof(qee_orig_xoffsets) / sizeof(struct _qee_xoffsets);
	oj = NULL;
	outer = NULL;
	inner = NULL;
	break;

    case QE_QP:
	xo = &qee_qp_xoffsets[0];
	nxo = sizeof(qee_qp_xoffsets) / sizeof(struct _qee_xoffsets);
	oj = NULL;
	outer = NULL;
	inner = NULL;
	break;

    case QE_SEJOIN:
	xo = &qee_sejn_xoffsets[0];
	nxo = sizeof(qee_sejn_xoffsets) / sizeof(struct _qee_xoffsets);
	oj = NULL;		/* SEjoin doesn't allow OJ?! */
	outer = node->node_qen.qen_sejoin.sejn_out;
	inner = node->node_qen.qen_sejoin.sejn_inner;
	break;

    case QE_SORT:
	xo = &qee_sort_xoffsets[0];
	nxo = sizeof(qee_sort_xoffsets) / sizeof(struct _qee_xoffsets);
	oj = NULL;
	outer = node->node_qen.qen_sort.sort_out;
	inner = NULL;
	break;

    case QE_TJOIN:
	xo = &qee_tjoin_xoffsets[0];
	nxo = sizeof(qee_tjoin_xoffsets) / sizeof(struct _qee_xoffsets);
	oj = node->node_qen.qen_tjoin.tjoin_oj;
	outer = node->node_qen.qen_tjoin.tjoin_out;
	inner = NULL;
	break;

    case QE_TSORT:
	xo = &qee_tsort_xoffsets[0];
	nxo = sizeof(qee_tsort_xoffsets) / sizeof(struct _qee_xoffsets);
	oj = NULL;
	outer = node->node_qen.qen_tsort.tsort_out;
	inner = NULL;
	break;

    default:
	nxo = 0;
	oj = NULL;
	outer = inner = NULL;
	break;
    } /* switch */

    /* Whirl around to resolve the offsets for the node */
    while (--nxo >= 0)
    {
	qadf = * (QEN_ADF **) ((char *)node + xo->qen_adf_offset);
	if (qadf != NULL)
	    * (ADE_EXCB **) ((char *) node_xaddrs + xo->qee_xaddrs_offset)
				= (ADE_EXCB *) cbs[qadf->qen_pos];
	++ xo;
    }

    /* Ditto for the OJ stuff if there is any */
    if (oj != NULL)
    {
	xo = &qee_oj_xoffsets[0];
	nxo = sizeof(qee_oj_xoffsets) / sizeof(struct _qee_xoffsets);
	while (--nxo >= 0)
	{
	    qadf = * (QEN_ADF **) ((char *)oj + xo->qen_adf_offset);
	    if (qadf != NULL)
		* (ADE_EXCB **) ((char *) node_xaddrs + xo->qee_xaddrs_offset)
				    = (ADE_EXCB *) cbs[qadf->qen_pos];
	    ++ xo;
	}
    }

    /* Recursively do left, right subtrees */

    if (outer != NULL)
	qee_resolve_xaddrs(dsh, outer, follow_qp);

    if (inner != NULL)
	qee_resolve_xaddrs(dsh, inner, follow_qp);

    if (follow_qp && node->qen_type == QE_QP)
    {
	QEF_AHD *act;

	/* Special casing for QP nodes.  Caller wants us to resolve the
	** addresses in the sub-plan underneath the QP.  Thanks to the
	** stupid distinction between nodes and action headers, this
	** has to be handled specially.
	** We don't do xaddrs for action headers (yet), just resolve the
	** node subtree.
	*/
	for (act = node->node_qen.qen_qp.qp_act;
	     act != NULL;
	     act = act->ahd_next)
	{
	    if (act->ahd_flags & QEA_NODEACT)
		qee_resolve_xaddrs(dsh, act->qhd_obj.qhd_qep.ahd_qep, follow_qp);
	}
    }

} /* qee_resolve_xaddrs */

