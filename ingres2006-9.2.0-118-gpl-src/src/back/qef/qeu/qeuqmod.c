/* Copyright (c) 1995, 2005 Ingres Corporation
**
**
*/

#include    <compat.h>
#include    <gl.h>
#include    <me.h>
#include    <pc.h>
#include    <st.h>
#include    <iicommon.h>
#include    <usererror.h>
#include    <dbdbms.h>
#include    <dudbms.h>
#include    <ddb.h>
#include    <cui.h>
#include    <bt.h>
#include    <tm.h>
#include    <er.h>
#include    <cs.h>
#include    <lk.h>
#include    <scf.h>
#include    <sxf.h>
#include    <ulb.h>
#include    <ulf.h>
#include    <ulm.h>
#include    <adf.h>
#include    <cm.h>
#include    <qsf.h>
#include    <dmf.h>
#include    <dmacb.h>
#include    <dmucb.h>
#include    <dmtcb.h>
#include    <dmrcb.h>
#include    <qefmain.h>
#include    <qefrcb.h>
#include    <qefcb.h>
#include    <qefscb.h>
#include    <qefqeu.h>
#include    <qeuqcb.h>
#include    <psfindep.h>
#include    <qeugrant.h>
#include    <qeurevoke.h>
#include    <ade.h>
#include    <dmmcb.h>
#include    <ex.h>
#include    <qefnode.h>
#include    <psfparse.h>
#include    <qefact.h>
#include    <qefqp.h>
#include    <qefdsh.h>
#include    <rqf.h>
#include    <tpf.h>
#include    <qefkon.h>
#include    <qefqry.h>
#include    <qefcat.h>
#include    <qefprotos.h>
#include    <rdf.h>

/*
** NO_OPTIM = rs4_us5 dg8_us5 sgi_us5 i64_aix
*/

/**
**
**  Name: QEUQMOD.C - provide qrymod support for RDF and PSF.
**
**  Description:
**      The routines defined in this file provide the qrymod semantics for
**	RDF (relation description facility).	Qrymod provides query 
**	modification to query trees in the form of added protections, 
**	integrities and views.  The routines in this file provide create,
**	destroy and retrieval capability on the qrymod objects.
**
**	This file also provides a method by which RDF can aquire the
**	optimizer statistics.
**
**	    qeu_credbp	    - store DB procedure definition
**	    qeu_drpdbp	    - destroy DB procedure definition
**	    qeu_dbp_status  - update status of existing database procedure
**          qeu_cview	    - store a view definition
**          qeu_dview	    - destroy a view definition
**          qeu_cprot	    - store protection information
**          qeu_dprot	    - destroy protection information
**          qeu_cintg	    - store integrity infoformation
**          qeu_dintg	    - destroy integrity information
**	    qeu_d_cascade   - perform cascading destruction of a DBMS object
**          qeu_dstat	    - destroy optimizer statistics
**	    qeu_qid	    - obtain query text id for a view.
**	    qeu_dbdelete    - delete tuples in iidbdepends based on the
**			      dependent id.
**	    qeu_priv_revoke - revoke privileges
**	    qeu_v_col_drop  - fix any column dependencies in iitree 
**
**
**  History:
**      13-jun-86 (daved)    
**          written
**      18-jul-86 (jennifer)
**          Initial coding of qrymod routines.
**      03-aug-86 (jennifer)
**          Eliminated the rview, rinteg, rprot, and
**          rstat from the query mod routines.
**          These will be done by RDF, eventually they
**          will be replaced with Query Plans RDF will
**          use to get the information by handing a 
**          Query Plan to QEF.  These routines could
**          not be handled in a single call since the
**          number of tuples to retrieve for trees, 
**          statistics, etc is not known and therefore
**          can be very large.  Since these routines kept 
**          not state between calls, it was not possible
**          to get n, return and get n more.  Therefore
**          RDF will make QEU_GET calls to perform these
**          fucntions until we can change to Query Plans.
**      20-aug-86 (jennifer)
**	    Changed all transaction calls from QET_xxxx
**          to QEU_xxx.  
**      03-sep-86 (jennifer)
**          Fixed bug in QEU_DSTAT where ulm end stream
**          called without begin.
**	1-jul-87 (daved)
**	    use new catalog iidbdepends, implement destruction of
**	    permits if base table goes away. remove use of iiviews.
**	10-sep-87 (puree)
**	    call qef_error to issue error message before cleaning up.
**	06-nov-87 (puree)
**	    change error from qet_escalate to QE0005_CANNOT_ESCALATE
**	    and error returned by qeu_qid to QE0097_NOT_A_VIEW.
**	01-dec-87 (puree)
**	    also set error status if local error is encountered.
**      10-dec-87 (puree)
**          Converted all ulm_palloc to qec_palloc.
**	20-jan-88 (puree)
**	    Convert all qet_escalate calls to in-line codes.
**	28-jan-88 (puree)
**	    Added debug code to qeu_cintg to trace illegal error codes 
**	    returned from called routines.
**	10-feb-88 (puree)
**	    Convert ulm_openstream to qec_mopen. Rename qec_palloc to
**	    qec_malloc.
**	09-mar-88 (puree)
**	    Change one of error codes returned by qeu_dview.
**	02-may-88 (puree)
**	    Add qeu_credbp and qeu_drpdbp for DB procedure support.
**	02-aug-88 (puree)
**	    Modify qeu_credbp for unique procedure id support.
**	    Remove qeu_showtab.  All qrymod catalogs now have hard coded
**	    table ids.
**	19-sep-88 (puree)
**	    Modify qeu_dbdelete.  Open iixdbdepends with IX instead of 
**	    IS lock mode and DMT_A_WRITE instead of DMT_A_READ for the
**	    access mode.
**	05-oct-88 (puree)
**	    modified qeu_cview due to change in user error 5102 format by PSF.
**	26-oct-88 (puree)
**	    fix string length for message 5102.
**	12-mar-89 (ralph)
**	    GRANT Enhancements, Phase 1:
**	    Add routines to process iiusergroup and iiapplication_id
**	17-may-89 (ralph)
**	    GRANT Enhancements, Phase 2:
**	    Moved qeu_aplid(), qeu_[cad]aplid, qeu_group, qeu_[cd]group
**	    to separate source files.
**      29-jun-89 (jennifer)
**          Added security auditing of procedures, protections and views.
**	11-oct-89 (ralph)
**	    Fix auditing problem
**	03-nov-89 (neil)
**	    Alerters: Support event permissions in qeu_cprot and qeu_dprot. 
**	02-jan-90 (nancy) -- bug #9208, fix qeu_dview user gets error when
**	    dropping table that has views on it.
**	16-jan-90 (ralph)
**	    Revise qeu_dprot() to prevent deletion of security_alarm tuples
**	    from iiprotect when dropping permits & vice-versa.
**	09-feb-90 (neil)
**	    Alerters - added auditing for EVENT objects.
**	2-may-90 (andre)
**	    add code to perform cascading drop of synonyms in qeu_dview().
**	04-may-90 (carol)
**	    Add cascading drops of comments.
**	08-aug-90 (ralph)
**	    Initialize new iiprotect fields in qeu_access_perm
**	    Mark audit access as SUCCESS when create/drop protection (bug 20702?)
**	    Indicate I_QE202D when successful create security alarm (bug 20804)
**	    Indicate I_QE202E when successful drop   security alarm (bug 20806)
**	24-sep-90 (ralph)
**	    Audit outcome when performing dbproc operations (bug 20688)
**	    Audit implicit drop of SELECT/ALL TO ALL (bug 20942)
**	02-feb-91 (davebf)
**	    Added init of qeu_cb.qeu_mask
**	04-feb-91 (neil)
**	    Alerters - fix DROP EVENT when permissions.
**	09-jul-91 (andre)
**	    qeu_cprot() will split privilege descriptors specifying more than
**	    one privilege.
**	17-oct-91 (rogerk)
**	    Part of 6.4 -> 6.5 merge changes. Must initialize new dmt_char_array
**	    field for DMT_SHOW calls.
**	10-feb-1992 (bryanp)
**	    Changed dmt_location from a DB_LOC_NAME to a DM_DATA.
**	    Initialize dmu_char_array fields in dmu_cb for DMU_DESTROY_TABLE.
**	26-jun-92 (andre)
**	    defined a new function - qeu_d_cascade() which will handle cascading
**	    destruction of tables, views dbprocs, and dbevents.  It is based
**	    largely on qeu_dview() with some code added to handle special
**	    processing which arises in conjunction with destruction of dbevents
**	    and dbprocs + some changes to ebsure that dbprocs dependent on
**	    objects being dropped are marked dormant
**
**	    body of qeu_dview() will be replaced with a call to dmt_show() to
**	    determine whether the object passed by the caller exists and is a
**	    table or a view followed by a call to qeu_d_cascade() to handle the
**	    rest of processing
**	26-jun-92 (andre)
**	    defined qeu_dbp_status().  This function will handle requests to
**	    change status of an existing dbproc.  The change may include
**	    inserting a list of independent objects and privileges into
**	    IIDBDEPENDS and IIPRIV, respectively and updating
**	    IIPROCEDURE.db_mask1 to reflect newly discovered information about
**	    the dbproc.
**	26-jul-92 (andre)
**	    defined qeu_priv_revoke() and qeu_add_lost_privs()
**	02-aug-92 (andre)
**	    defined qeu_revoke()
**	19-aug-1992 (andys)
**	    Integrate change from ingres63p:
**            17-Oct-91 (wojtek) Integration from 6302p:
**         	03-sep-91 (bentley)
**          	    Integration of ingresug change 92001 (8-April-91 (rmuth))
**                  Bug #33494, modified qeu_dintg(). When dropping an Integrity
**          	    always update the tables timestamp, currently only does it
**          	    if drop all integrites on table.
**          	04-Oct-91 (pauland)
**          	    Correction of integration 03-sep-91 (bentley).
**      23-nov-92 (terjeb)
**          The hp compiler does not like to autoincrement something that
**          has been casted to another type. I therefore assign to a tmp
**          variable before calling CMcpyincr. This change was done in 
**          qeu_bld_qtxt_tuple().
**	30-nov-1992 (pholman)
**	    C2: replace obsolete qeu_audit with qeu_secaudit.
**	11-dec-92 (rickh)
**	    Enable writing of 2nd integrity tuple for CHECK CONSTRAINTS.
**	21-jan-1993 (bryanp)
**	    Fixed edit check in qeu_cintg related to 2nd integrity tuple.
**	01-feb-93 (andre)
**	    undefined qeu_bld_qtxt_tuple() and replaced it with
**	    qeu_assemble_text() which will not try to build query text tuples
**	    (this honor will be left to RDF); instead it will simply accumulate
**	    text using DB_TEXT_STRING format 
**	15-apr-93 (rickh)
**	    Folded in casts that the Alpha porters discovered were needed to
**	    shut up their compiler.
**	27-apr-93 (anitap)
**	    Removed the following change:
**		23-apr-93 (anitap)
**	    	    Added support for creating implicit schema for procedures.
**	30-jun-1993 (shailaja)
**	    Cast to quieten ANSI compiler
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	18-jun-93 (rickh)
**	    Drop IIKEY tuples when exterminating a CONSTRAINT.
**	6-jul-93 (robf)
**	    Pass security label to qeu_secaudit()
**      07-jul-93 (anitap)
**          Added arguments qef_rcb and qeuq_cb to qea_schema() instead of
**          PTR in qeu_cview() and qeu_credbp().
**      14-sep-93 (smc)
**          Fixed up many casts, mostly because of session ids becomming 
**          CS_SID.
**	21-sep-93 (stephenb)
**	    Creation of protection on an event is a control action on an event
**	    not a create action, changed access mask in call to qeu_secaudit()
**	    to SXF_A_CONTROL when defining protection on event.
**	23-sep-93 (stephenb)
**	    Creation/deletion of an integrity is an auditable event, added 
**	    calls to qeu_secaudit() in approriate routines.
**	6-oct-93 (stephenb)
**	    Set tablename in dmu_cb in qeu_d_cascade() so that we can audit 
**	    drop temp table.
**	4-oct-93 (rickh)
**	    When creating an integrity, scan IIINTEGRITY with shared access.
**	24-nov-93 (stephenb)
**	    Tidy up revoke/drop permit auditing so that we audit drop permit all
**	    or revoke all only once, and so that revoke is audited only in
**	    qeu_revoke() and not in qeu_dprot() as well.
**	28-mar-1995 (canor01)
**	    Turn off optimization for AIX 4.1 (rs4_us5)
**	 3-jan-1996 (nick)
**	    Reorder the taking of locks in qeu_d_cascade() to match that in 
**	    'creates' - this reduces the possibility of deadlock with 
**	    concurrent 'drops'.
**	04-08-1996 (sarjo01)
**	    Bug 81111: qeu_d_cascade(): do a readonly scan of iixpriv and
**	    iidbdepends so that if no dependencies are found, no X page
**	    locks are left on these catalogs, thereby increasing
**	    concurrency for DROP TABLE.
**	05-07-1997 (sarjo01)
**	    Bug 82047: qeu_d_cascade(): Problem introduced by above change:  
**	    If table to be dropped has a primary key constraint but no
**	    otner dependencies, iidbdepends must be reopened in X mode. 
**	07-08-1997 (sarjo01)
**	    Fix for 82047 reintroduced inadvertant taking of IX locks on
**	    iidbdepends. Reworked last change. 
**      22-jul-96 (ramra01)
**          Alter table add drop column implementation. Cascade/restrict
**          examine objects dependent on dropped column.
**	23-jan-97 (shero03)
**	    B79960:  Fixup column id in iitree when a column in that table is
**	    dropped.  (Externalize qeu_v_col_drop.)
**	27-mar-1997 (shero03)
**	    Broke the finddependencyandfixtree into two separate routines 
**	    That way all the dependent trees can be check for dependencies 
**	    before any tree is changed.
**	11-Aug-1997 (jenjo02)
**	    Changed ulm_streamid from (PTR) to (PTR*) so that ulm
**	    can destroy those handles when the memory is freed.
**	19-Aug-1997 (jenjo02)
**	    Distinguish between private and shared ULM streams.
**	    Piggyback ulm_palloc() calls with ulm_openstream() where appropriate.
**	27-Jul-1998 consi01 bug 87642 star problem INGSRV 233
**	    ensure that the view_opened flag is set/unset correctly when
**	    table reopened for writing. Add comments in places where this
**	    problem could surface again.
**      18-aug-1998 (hweho01)
**          Turn off optimization for DG/88K (dg8_us5).
**	17-mar-1999 (somsa01)
**	    Added include of pc.h and lk.h to resolve references to locking
**	    ids.
**	23-Mar-1999 (kosma01)
**	    This must be a tough module to optimize. Just compiling this
**	    on sgi_us5 with optimization gets:
**	    Signal: Segmentation fault in Loop Nest Optimizer phase.
**	    Error: Signal Segmentation fault in phase Loop Nest Optimizer -- processing aborted
**	    cc ERROR:  /usr/lib32/cmplrs/be died due to signal 4
**	    cc ERROR:  core dumped
**	    --- remove compiler optimization for sgi_us5
**      30-Mar-1999 (hanal04)
**          Do not drop PERMITs on DBPs that are being marked dormant. b96033.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	10-Jan-2001 (jenjo02)
**	    Supply session's SID to QSF in qsf_sid.
**	24-Jan-2001 (jenjo02)
**	    If RDF fails to get (QEF) memory, garbage collect,
**	    then retry.
**	26-Jan-2001 (jenjo02)
**	    Subvert calls to qeu_secaudit(), qeu_secaudit_detail()
**	    if not C2SECURE.
**	16-aug-2001 (toumi01)
**	    speculative i64_aix NO_OPTIM change for beta xlc_r - FIXME !!!
**	28-aug-2002 (gygro01) ingsrv1859
**	    b108474, which includes b93927, b108635, 108636 and b108638.
**	    It resolves cleanup problems by drop column cascade processing
**	    mainly for referential constraints and its dependent objects 
**	    (indexes, rules and procedures) as well as cleanup problems in 
**	    iidbdepends for other constraints (primary/unique).
**	    Basically now we will only drop those dependent object, if the
**	    dependent constraint was dropped as column bitmap testing against
**	    the drop column for table based rules or db procedures is not
**	    possible as they don't have bitmaps set. Proper bitmap check,
**	    especially for referential constraints, is only valid at the
**	    constraint level.
**	    Changes were made in que_d_cascade(), qeu_dintg(), getIIKEYtuples()
**	    and replaceIIKEYtuples().
**      29-Apr-2003 (hanal04) Bug 109682 INGSRV 2155, Bug 109564 INGSRV 2237
**          Added qeu_d_check_conix() to determine whether a table/index
**          is being used by a constraint.
**          Modify qeu_d_cascade() to prevent user indexes being dropped
**          as part of a drop constraint operation.
**      01-Oct-2003 (inifa01) Bug 110429 INGSRV 2380
**          Dropping a unique constraint where base table is index causes
**          the base table to be dropped.
**          Fix to bug 109682/109564 has added iidepends entries for this case
**	    , which allows dependency check to retrieve the base table as an
**	    object to be dropped while dropping the constraint.
**	    FIX: If when dropping a constraint, the 'index' is not a secondary
**          index then don't add 'index' to temp table list of objects to drop.
**	19-Feb-2004 (schka24)
**	    Handle drop of partitioned tables.
**      15-mar-2004 (horda03) Bug 111919/INGSRV2745
**          Fix for 109682/INGSRV2155 results in an X lock on IIDBDEPENDS when
**          a CREATE TABLE .. AS SELECT... is executed. This severly impacts
**          performance. Reducing the lock level to S.
**	30-mar-04 (toumi01)
**	    move qefdsh.h below qefact.h for QEF_VALID definition
**	21-Nov-2004 (kodse01)
**	    BUG 113504
**	    Set the bits DB_REFERENCES, DB_COPY_INTO and DB_COPY_FROM in 
**	    all_privs variable.
**	16-dec-04 (inkdo01)
**	    Added various collID's.
**	16-Mar-2005 (kodse01)
**	    BUG 113504
**	    Turning off the DMT_C_PERMITS bit if all the permits are revoked	
**	    using revoke command.
**	13-Feb-2006 (kschendel)
**	    Fix a couple annoying int == NULL warnings.
**      13-mar-2006 (horda03) Bug 112560/INGSRV2880
**          Take an IX lock on the base table before checking to see if the table
**          has any constraints. The check on constrainst will be done using a dirty
**          (no lock) scan of the IIDBDEPENDS to prevent table modify stalling/being
**          stalled by other sessions performing ALTER TABLE commands against other
**          tables in the DB.
**     05-oct-2006 (stial01)
**          qeu_d_check_conix(), open IIDBDEPENDS lockmode DMT_N. (b112560)
**/

FUNC_EXTERN char	    *STskipblank();
static DB_STATUS
qeu_new_prot_tuple(
QEF_CB	    *qef_cb,
QEUQ_CB	    *qeuq_cb,
QEU_CB	    *prot_qeu,
QEU_CB      *tree_qeu,
QEU_CB      *text_qeu,
QEU_CB      *dpnd_qeu,
i4	    permno,
i2	    priv_descr_no,
bool	    bld_priv_descr,
i4	    *error);

static DB_STATUS
qeu_enter_dependencies(
QEF_CB			*qef_cb,
QEUQ_CB			*qeuq_cb,
DB_TAB_ID		*dep_id,
DB_QMODE		dep_type,
i2			priv_descr_no);

static DB_STATUS
qeu_scan_perms(
QEUQ_CB		*qeuq_cb,
QEF_CB          *qef_cb,
QEU_CB		*prot_qeu,
DB_PROTECTION   *ptuple,
DB_PROTECTION   *new_ptuple,
bool		indep_privs,
QEU_NEWPERM_INFO *newperm_info,
ULM_RCB         *ulm,
i4		*permno,
i4		relstat,
i4		*error);

static bool
qeu_grant_compatible(
DB_PROTECTION	    *ptuple,
bool		    where_clause,
bool		    term_specified);

static DB_STATUS
qeu_match_privs(
QEUQ_CB			*qeuq_cb,
QEF_CB          	*qef_cb,
QEU_CB			*prot_qeu,
QEU_REVOKE_INFO   	*privs_to_check,
QEU_REVOKE_INFO   	*grant_option_to_check,
QEU_PERM_DEPTH_INFO	*perm_depth_info,
i4			list_size,
QEU_PDI_TBL_DESCR	*pdi_tbl_descr,
DB_PROTECTION   	*ptuple,
DB_OWN_NAME		*obj_owner,
i4			*error);

static DB_STATUS
qeu_priv_revoke(
QEUQ_CB			*qeuq_cb,
QEF_CB          	*qef_cb,
QEU_REVOKE_INFO 	*privs_lost,
QEU_REVOKE_INFO 	*grant_option_lost,
i4			list_size,
QEU_PDI_TBL_DESCR	*pdi_tbl_descr);

static DB_STATUS
qeu_add_lost_privs(
QEF_CB			*qef_cb,
QEU_CB			*temp_qeu,
QEU_REVOKE_INFO		*privs_lost,
QEU_REVOKE_INFO		*grant_option_lost,
QEU_REVOKE_INFO 	*temp_row,
i4			list_size);

static DB_STATUS
qeu_construct_perm_text(
QEUQ_CB		*qeuq_cb,
QEF_CB		*qef_cb,
QEU_CB		*qtxt_qeu,
ULM_RCB		*ulm,
DB_PROTECTION	*prot_tuple,
i4		new_perm,
QEF_DATA	**new_qtxt,
i4		*error);

static DB_STATUS
qeu_assemble_text(
DB_TEXT_STRING	**result,
i4		*max_size,
char		*str,
i4  		str_len,
ULM_RCB         *ulm,
i4		*error);

static DB_STATUS
qeu_perm_col_list(
DB_TEXT_STRING  **result,
i4              *max_size,
DB_PROTECTION	*prot_tuple,
DMT_ATT_ENTRY	*att_array,
ULM_RCB		*ulm,
i4		*error);

static DB_STATUS
qeu_0make_grant_perm_text(
DB_TEXT_STRING  **result,
DB_PROTECTION	*prot_tuple,
DMT_ATT_ENTRY	*att_array,
ULM_RCB		*ulm,
i4         *error);

static DB_STATUS
qeu_1make_create_permit_text(
DB_TEXT_STRING  **result,
QEF_CB		*qef_cb,
QEU_CB		*qtxt_qeu,
ULM_RCB         *ulm,
DB_PROTECTION   *prot_tuple,
DB_IIQRYTEXT    *read_qtxt_tuple,
i4		*qtxt_seq,
DMT_ATT_ENTRY	*att_array,
i4         *error);

static DB_STATUS
qeu_0skip_words(
i4		num_words,
char		**cur_char,
char		**last_char,
QEF_CB          *qef_cb,
QEU_CB          *qtxt_qeu,
DB_TEXT_STRING	*in_txt,
ULM_RCB         *ulm,
i4		*qtxt_seq,
DB_TEXT_STRING	**out_buf,
i4		*buf_capacity,
bool		add_to_text,
i4		*error);

static DB_STATUS
qeu_1skip_white_chars(
char		**cur_char,
char		**last_char,
QEF_CB          *qef_cb,
QEU_CB          *qtxt_qeu,
DB_TEXT_STRING	*in_txt,
ULM_RCB         *ulm,
i4		*qtxt_seq,
DB_TEXT_STRING	**out_buf,
i4		*buf_capacity,
bool		add_to_text,
i4		*error);

static DB_STATUS
qeu_2skip_delimited_string(
char		**cur_char,
char		**last_char,
char		*delim,
QEF_CB          *qef_cb,
QEU_CB          *qtxt_qeu,
DB_TEXT_STRING	*in_txt,
ULM_RCB         *ulm,
i4		*qtxt_seq,
DB_TEXT_STRING	**out_buf,
i4		*buf_capacity,
bool		add_to_text,
i4		*error);

static VOID
qeu_prv_not_revoked_err(
DB_PROTECTION	    *priv,
QEUQ_CB		    *qeuq_cb,
bool		    user_lacked_privs,
i4		    *error);

static i4
qeu_q_integ(
i4		    *integ_no,
DB_INTEGRITY	    *integ_tuple);

static DB_STATUS
qeu_nxt_prv_descr_no(
QEF_CB		    *qef_cb,
QEUQ_CB		    *qeuq_cb,
i2		    *descr_no,
i4		    *error);

static i4
qeu_mpt2_qual(
QEU_MATCH_PRIV_INFO	*new_info,
QEU_MATCH_PRIV_INFO	*info_tuple);

#ifndef	TEMP_TABLES_WORK
static i4
qeu_mpt1_qual(
QEU_MATCH_PRIV_INFO	*key_info,
QEU_MATCH_PRIV_INFO	*info_tuple);
#endif

static DB_STATUS
qeu_depth(
QEF_CB			*qef_cb,
QEUQ_CB			*qeuq_cb,
DB_TAB_ID		*obj_id,
QEU_PDI_TBL_DESCR	*pdi_tbl_descr,
DB_OWN_NAME		*obj_owner,
i4			*error);

static i4
qeu_pdp2_qual(
QEU_PERM_DEPTH_INFO	*info_tuple,
DB_PROTECTION		*prot_tuple);

static i4
qeu_pdp1_qual(
DB_PROTECTION		*cur_prot_tuple,
DB_PROTECTION		*prot_tuple);

static i4
qeu_pdp3_qual(
DB_PROTECTION		*cur_prot_tuple,
DB_PROTECTION		*prot_tuple);

static i4
qeu_q_consdep(
	DB_TAB_ID	*tbl_id,
	DB_IIDBDEPENDS	*dep_tuple);

static i4
qeu_iipriv_by_type(
	i2		*dep_type,
	DB_IIPRIV	*iipriv_tuple);

static DB_STATUS
deleteIIKEYtuples(
    QEF_CB		*qef_cb,
    QEU_CB		*iikeyQEU_CB,
    DMR_ATTR_ENTRY	**iikey_keyPtrArray,
    bool		*iikey_opened,
    DB_CONSTRAINT_ID	*constraintID,
    i4		*error
);

static DB_STATUS
replaceIIKEYtuples(
    QEF_CB		*qef_cb,
    QEU_CB		*iikeyQEU_CB,
    DMR_ATTR_ENTRY	**iikey_keyPtrArray,
    bool		*iikey_opened,
    DB_CONSTRAINT_ID	*constraintID,
    i2			attidfrom,
    i4		*error
);

static DB_STATUS
getIIKEYtuples(
    QEF_CB              *qef_cb,
    QEU_CB              *iikeyQEU_CB,
    DMR_ATTR_ENTRY      **iikey_keyPtrArray,
    bool                *iikey_opened,
    DB_CONSTRAINT_ID    *constraintID,
    i2                  *keypos,
    i2                  *attid,
    i4             *error
);

static DB_STATUS
qeu_crt_pdi_tbl(
	QEF_CB			*qef_cb,
        QEU_PDI_TBL_DESCR       *pdi_tbl_descr,
        PTR                     db_id,
	DB_TAB_ID		*obj_id,
        i4                 *error);

static DB_STATUS
qeu_drop_pdi_tbl(
	QEF_CB			*qef_cb,
	QEU_PDI_TBL_DESCR       *pdi_tbl_descr,
	bool			force,
	DB_ERROR		*err_blk,
	i4			*error);

static DB_STATUS
qeu_destr_dbp_qp(
	QEF_CB		*qef_cb,
	DB_OWN_NAME	*dbp_schema,
	DB_DBP_NAME	*dbp_name,
	DB_ERROR	*err_blk);

static DB_STATUS
qeu_v_finddependency(
	ULM_RCB		*ulm,
	QEF_CB		*qef_cb,
	QEU_CB		*tqeu,
	DB_TAB_ID	*dep_id,
	DB_TAB_ID	*base_id,
	DB_TREE_ID	*tree_id,
	i2		obj_num,
	DB_NAME		*obj_name,
	DB_OWN_NAME	*obj_owner,
	DB_QMODE	depobj_type,
	i4		col_attno,
	bool 		cascade,
	bool		*drop_depobj,
	DB_ERROR	*err_blk);

static DB_STATUS
qeu_d_check_consix(
      QEF_CB          *qef_cb,
      QEUQ_CB         *qeuq_cb,
      ULM_RCB         *ulm,
      DB_TAB_ID       *ixid,
      i4              *error,
      bool            *found_another);

static VOID
preorder(
	PST_QNODE           *qnode,
	i4		    irngtab,
	i4             col_attno,
	bool                *drop_depobj);

static VOID
fixtree(
	PST_QNODE           *qnode,
	i4             irngtab,
	i4             col_attno,
	bool                *rep_depobj);


/*{
** Name: QEU_CREDBP	- store a DB procedure definition
**
** Synopsis:
**	DB_STATUS   status;
**	QEUQ_CB	    *qeuq_cb;
**
**	    status = qef_call(QEU_CREDBP, &qeuq_cb);
**
** Description:
**	Append a procedure description tuple to iiprocedure and append
**	the procedure definition text to iiqrytext.  The procedure id
**	is created from the system timestamp and is used both in the
**	db_txtid field of the DB_PROCEDURE and in the db_txtid field of
**	the DB_IIQRYTEXT structures.  Before being appended into the 
**	catalogs, the input iiprocedure tuple and the associated 
**	iiqrytext tuple are updated with this timestamp.  The order of
**	appending tuples into catalogs is described below:
**  
**	    - append the iiprocedure tuple.
**	    - append the iiqrytext tuples.
**	    - append the iiprocedure_parameter tuples.
**
**	If there is no current transaction, a multi-statement transaction
**	is created on behalf of the user.
**
** Inputs:
**      qef_cb                  qef session control block
**      qeuq_cb
**	    .qeuq_eflag	        designate error handling semantis
**				for user errors.
**		QEF_INTERNAL	return error code.
**		QEF_EXTERNAL	send message to user.
**	    .qeuq_cq		number of query text tuples
**	    .qeuq_qry_tup	query text tuples
**	    .qeuq_culd		number of procedure parameter tuples
**	    .qeuq_uld_tup	procedure parameter tuples
**	    .qeuq_dbp_tup	iiprocedure tuple
**	    .qeuq_db_id		database id
**	    .qeuq_d_id		dmf session id
**	    .qeuq_indep		independent object and privilege descriptors
**		psq_objs	points to descriptor(s) of independent object(s)
**				on which a new dbproc depends
**		psq_objprivs	descriptor(s) of table-wide privileges on which
**				a new dbproc depends
**		psq_colprivs	descriptor(s) of column-specific privileges on
**				which a new dbproc depends
**
** Outputs:
**      qeuq_cb
**	    .error.err_code	one of the following
**				E_QE0002_INTERNAL_ERROR
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0022_ABORTED
**				E_QE0116_CANT_ALLOCATE_DBP_ID
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	02-may-88 (puree)
**	    created for DB procedure.
**	02-aug-88 (puree)
**	    add DMF call to get a unique id to be used as the procedure id.
**      29-jun-89 (jennifer)
**         Added security auditing.
**	25-jun-92 (andre)
**	    call qeu_enter_dependencies() to record objects and privileges on
**	    which the new dbproc depends.
**	19-jan-93 (rickh)
**	    Corrected some problems caused by yesterday's integration.
**	28-jan-93 (jhahn)
**	    Added appending of procedure parameter tuples.
**	04-mar-93 (andre)
**	    parameter tuples are pointed to by qeuq_uld_tup and not by
**	    qeuq_qry_tup.
**	23-mar-93 (anitap)
**	    Added support for creating implicit schema for procedures.
**      07-jul-93 (anitap)
**          Added arguments qef_rcb and qeuq_cb to qea_schema() instead of
**          PTR. Changed assignment of flag to IMPLICIT_SCHEMA.
**	27-oct-93 (andre)
**	    As a part of fix for bug 51852 (which was reported in 6.4 but can
**	    and will manifest itself in 6.5 as soon as someone places some 
**	    stress on the DBMS), we want to use a dbproc id (really guaranteed 
**	    to be unique) instead of timestamps (allegedly unique, but in fact 
**	    may be non-unique if several objects get created in rapid succession
**	    on a reasonably fast box) to identify IIQRYTEXT tuples associated 
**	    with a given dbproc.  This id (pumped through randomizing function)
**	    will be used to key into IIQRYTEXT 
**
**	    store creation time in the new attribute of IIPROCEDURE
*/
DB_STATUS
qeu_credbp(
QEF_CB          *qef_cb,
QEUQ_CB		*qeuq_cb)
{
    DB_IIQRYTEXT    *qtuple;
    DB_PROCEDURE    *dbptuple;
    QEF_DATA	    *next;
    DB_STATUS	    status, local_status;
    DB_ERROR	    e_error;
    i4	    error;
    bool	    transtarted = FALSE;	    
    bool	    tbl_opened = FALSE;
    bool	    err_already_reported = FALSE;
    i4		    i;
    QEU_CB	    tranqeu;
    QEU_CB	    qeu;
    DMU_CB	    dmu_cb;
    DB_TAB_ID	    randomized_id;
    DB_QRY_ID	    creation_time;
    i4		    flag = IMPLICIT_SCHEMA;
    QEF_RCB         *qef_rcb = (QEF_RCB *)NULL;	
    i4		    exit_loop;

    for (exit_loop = FALSE; !exit_loop; )
    {
	/* Check the control block. */

	if (qeuq_cb->qeuq_type != QEUQCB_CB || 
	    qeuq_cb->qeuq_length != sizeof(QEUQ_CB))	
	{
	    error = E_QE0017_BAD_CB;
	    status = E_DB_ERROR;
	    break;
	}

        if ((qeuq_cb->qeuq_db_id == 0)
	    || (qeuq_cb->qeuq_dbp_tup == (QEF_DATA *)NULL)
	    || (qeuq_cb->qeuq_qry_tup == (QEF_DATA *)NULL)
	    || (qeuq_cb->qeuq_indep == (PTR) NULL)
	   ) 
	{
	    error = E_QE0018_BAD_PARAM_IN_CB;
	    status = E_DB_ERROR;
	    break;
	}
		
	/* 
	** Check to see if a transaction is in progress,
	** if so, set transtarted flag to FALSE, otherwise
	** set it to TRUE after beginning a transaction.
	** If we started a transaction then we will abort
        ** it if an error occurs, otherwise we will just
        ** return the error and let the caller abort it.
	*/

	if (qef_cb->qef_stat == QEF_NOTRAN)
	{
	    tranqeu.qeu_type = QEUCB_CB;
	    tranqeu.qeu_length = sizeof(QEUCB_CB);
	    tranqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
	    tranqeu.qeu_d_id = qeuq_cb->qeuq_d_id;
	    tranqeu.qeu_flag = 0;
	    tranqeu.qeu_mask = 0;
	    status = qeu_btran(qef_cb, &tranqeu);
	    if (DB_FAILURE_MACRO(status))
	    {	
		error = tranqeu.error.err_code;
		break;	
	    }	    
	    transtarted = TRUE;
	}
	/* escalate the transaction to MST */
	if (qef_cb->qef_auto == QEF_OFF)
	    qef_cb->qef_stat = QEF_MSTRAN;

	dbptuple = (DB_PROCEDURE *)qeuq_cb->qeuq_dbp_tup->dt_data;

	/* Get the unique object id (table id) from DMF.  This is the 
	** procedure id which is unique within a database */
	
	dmu_cb.type = DMU_UTILITY_CB;
	dmu_cb.length = sizeof(DMU_CB);
	dmu_cb.dmu_flags_mask = 0;
	dmu_cb.dmu_tran_id = qef_cb->qef_dmt_id;
	dmu_cb.dmu_db_id = qeuq_cb->qeuq_db_id;

	status = dmf_call(DMU_GET_TABID, &dmu_cb);
	if (status != E_DB_OK)
	{
	    error = dmu_cb.error.err_code;
	    break;
	}	
	
	dbptuple->db_procid.db_tab_base = dmu_cb.dmu_tbl_id.db_tab_base;
	dbptuple->db_procid.db_tab_index = dmu_cb.dmu_tbl_id.db_tab_index;

	/*
	** use dbproc id to compute "randomized" id which will be used to key 
	** into IIQRYTEXT
	*/
	randomized_id.db_tab_base = 
	    ulb_rev_bits(dbptuple->db_procid.db_tab_base);
	randomized_id.db_tab_index = 
	    ulb_rev_bits(dbptuple->db_procid.db_tab_index);

	/* store "randomized" id as a text id */
	dbptuple->db_txtid.db_qry_high_time = randomized_id.db_tab_base;
	dbptuple->db_txtid.db_qry_low_time = randomized_id.db_tab_index;

        /* store creation date */
        TMnow((SYSTIME *) &creation_time);
        dbptuple->db_created = creation_time.db_qry_high_time;

	/* Now open iiprocedure table. */
	qeu.qeu_type = QEUCB_CB;
        qeu.qeu_length = sizeof(QEUCB_CB);
        qeu.qeu_db_id = qeuq_cb->qeuq_db_id;
        qeu.qeu_lk_mode = DMT_IX;
        qeu.qeu_flag = DMT_U_DIRECT;
        qeu.qeu_access_mode = DMT_A_WRITE;
        qeu.qeu_tab_id.db_tab_base = DM_B_DBP_TAB_ID; 	
        qeu.qeu_tab_id.db_tab_index = DM_I_DBP_TAB_ID; 	
	qeu.qeu_mask = 0;

	status = qeu_open(qef_cb, &qeu);
	if (DB_FAILURE_MACRO(status))
	{
	    error = qeu.error.err_code;
	    break;
	}
	tbl_opened = TRUE;

	/* Append the tuple to iiprocedure.  Assume only one tuple per call.*/
	qeu.qeu_count = 1;
        qeu.qeu_tup_length = sizeof(DB_PROCEDURE);
	qeu.qeu_input = qeuq_cb->qeuq_dbp_tup;
	status = qeu_append(qef_cb, &qeu);
	if (DB_FAILURE_MACRO(status))
	{
	    error = qeu.error.err_code;
	    break;
	}
	    
	status = qeu_close(qef_cb, &qeu);    
	if (DB_FAILURE_MACRO(status))
	{
	    error = qeu.error.err_code;
	    break;
	}
	tbl_opened = FALSE;

	/* This is a security event, must audit. */
        
	if ( Qef_s_cb->qef_state & QEF_S_C2SECURE )
	{
	    status = qeu_secaudit(FALSE, qef_cb->qef_ses_id, 
		  (char *)&dbptuple->db_dbpname, &dbptuple->db_owner,
		  sizeof(dbptuple->db_dbpname), SXF_E_PROCEDURE,
		  I_SX2012_DBPROC_CREATE, SXF_A_SUCCESS | SXF_A_CREATE,
		  &e_error);

	    if (status != E_DB_OK)
	    {
		error = e_error.err_code;
		break;
	    }
	}
	
	/* Now open the iiqrytext system catalog. */
	qeu.qeu_tab_id.db_tab_base = DM_B_QRYTEXT_TAB_ID;
	qeu.qeu_tab_id.db_tab_index = 0L;
	status = qeu_open(qef_cb, &qeu);
	if (DB_FAILURE_MACRO(status))
	{
	    error = qeu.error.err_code;
	    break;
	}
	tbl_opened = TRUE;
    
	/* Insert query text id into all query text tuples */
	next = qeuq_cb->qeuq_qry_tup;
	for (i = 0; i < qeuq_cb->qeuq_cq; i++)
	{
	    qtuple = (DB_IIQRYTEXT*) next->dt_data;
	    next = next->dt_next;
	    qtuple->dbq_txtid.db_qry_high_time = randomized_id.db_tab_base;
	    qtuple->dbq_txtid.db_qry_low_time = randomized_id.db_tab_index;
	    if (i < (qeuq_cb->qeuq_cq -1) && next == 0)
	    {
		error = E_QE0018_BAD_PARAM_IN_CB;
		status = E_DB_ERROR;
		break;
	    }
	}
	if (status != E_DB_OK)    		
	    break;

	/* Append the query text tuples to the iiqrytext table */
	qeu.qeu_count = qeuq_cb->qeuq_cq;
        qeu.qeu_tup_length = sizeof(DB_IIQRYTEXT);
	qeu.qeu_input = qeuq_cb->qeuq_qry_tup;
	status = qeu_append(qef_cb, &qeu);
	if (DB_FAILURE_MACRO(status))
	{
	    error = qeu.error.err_code;
	    break;
	}
	    
	status = qeu_close(qef_cb, &qeu);    
	if (DB_FAILURE_MACRO(status))
	{
	    error = qeu.error.err_code;
	    break;
	}
	tbl_opened = FALSE;

	if (qeuq_cb->qeuq_culd > 0)	/* if we have parameters */
	{
	    /* Now open the iiprocedure_parameter system catalog. */
	    qeu.qeu_tab_id.db_tab_base = DM_B_IIPROC_PARAM_TAB_ID;
	    qeu.qeu_tab_id.db_tab_index = 0L;
	    status = qeu_open(qef_cb, &qeu);
	    if (DB_FAILURE_MACRO(status))
	    {
		error = qeu.error.err_code;
		break;
	    }
	    tbl_opened = TRUE;
    
	    /* Insert procedure id into all procedure parameter tuples */
	    next = qeuq_cb->qeuq_uld_tup;
	    for (i = 0; i < qeuq_cb->qeuq_culd; i++)
	    {
		DB_PROCEDURE_PARAMETER *proc_param;
		if (next == NULL)
		{
		    error = E_QE0018_BAD_PARAM_IN_CB;
		    status = E_DB_ERROR;
		    break;
		}
		proc_param = (DB_PROCEDURE_PARAMETER *) next->dt_data;
		next = next->dt_next;
		proc_param->dbpp_procedureID.db_tab_base =
		    dbptuple->db_procid.db_tab_base;
		proc_param->dbpp_procedureID.db_tab_index =
		    dbptuple->db_procid.db_tab_index;
	    }
	    if (status != E_DB_OK)    		
		break;

	    /* Append the procedure parameter tuples to the
	    ** iiprocedure_parameters table */
	    qeu.qeu_count = qeuq_cb->qeuq_culd;
	    qeu.qeu_tup_length = sizeof(DB_PROCEDURE_PARAMETER);
	    qeu.qeu_input = qeuq_cb->qeuq_uld_tup;
	    status = qeu_append(qef_cb, &qeu);
	    if (DB_FAILURE_MACRO(status))
	    {
		error = qeu.error.err_code;
		break;
	    }
	    
	    status = qeu_close(qef_cb, &qeu);    
	    if (DB_FAILURE_MACRO(status))
	    {
		error = qeu.error.err_code;
		break;
	    }
	    tbl_opened = FALSE;
	}
	/*
	** if the new dbproc depends on other objects, this will be recorded in 
	** IIDBDEPENDS
	** 
	** if the new dbproc depends on some privileges, this will be recorded
	** in IIPRIV
	*/
	status = qeu_enter_dependencies(qef_cb, qeuq_cb, &dmu_cb.dmu_tbl_id,
	    (DB_QMODE) DB_DBP, (i2) 0);
	if (DB_FAILURE_MACRO(status))
	    err_already_reported = TRUE;    /*
					    ** qeu_enter_dependencies() reports
					    ** its own errors
					    */
	/* call qea_schema() to create implicit schema */
	status = qea_schema(qef_rcb, qeuq_cb, qef_cb, 
		(DB_SCHEMA_NAME *) &dbptuple->db_owner, flag);
	if (status != E_DB_OK)
	{
	   error = qeuq_cb->error.err_code;
	   break;
	}

	exit_loop = TRUE;
    } /* end for */

    /* call qef_error to handle error messages, if any */
    if (DB_FAILURE_MACRO(status))
	if (!err_already_reported)
	    (VOID) qef_error(error, 0L, status, &error, &qeuq_cb->error, 0);
	else
	    err_already_reported = FALSE;
    
    /* Close off all the tables. */
    if (tbl_opened)
    {
	local_status = qeu_close(qef_cb, &qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(qeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }
    if (transtarted)
    {
	if (status == E_DB_OK)
	{
	    status = qeu_etran(qef_cb, &tranqeu);
	    if (DB_FAILURE_MACRO(status))
	    {
		(VOID) qef_error(tranqeu.error.err_code, 0L, status, 
			    &error, &qeuq_cb->error, 0);
	    }
	}
	
	if (status != E_DB_OK)
	{
	    local_status = qeu_atran(qef_cb, &tranqeu);
	    if (local_status != E_DB_OK)
	    {
		(VOID) qef_error(tranqeu.error.err_code, 0L, local_status, 
			    &error, &qeuq_cb->error, 0);
		if (local_status > status)
		    status = local_status;
	    }
	}
    }

    return (status);
}

/*{
** Name: QEU_DBP_STATUS	- update status of an existing database procedure 
**
** Synopsis:
**	DB_STATUS   status;
**	QEUQ_CB	    *qeuq_cb;
**
**	    status = qef_call(QEU_DBP_STATUS, &qeuq_cb);
**
** Description:
**	Update status of an existing database procedure.  The change may include
**	inserting a list of independent objects and privileges into IIDBDEPENDS
**	and IIPRIV, respectively and updating IIPROCEDURE.db_mask1 to reflect
**	newly discovered information about the dbproc.
**	First we will open IIPROCEDURE and read the tuple.
**	If the dbp_mask[0] contained in the tuple supplied by the caller is a
**	   subset of IIPROCEDURE.db_mask1,
**	     we have nothing to add and will return
**	else
**	    if DB_DBP_INDEP_LIST bit is set in dbp_mask[0] contained in the
**	       tuple supplied by the caller and is not set in
**	       IIPROCEDURE.db_mask1
**	         we will call qeu_enter_dependencies() to enter the list of
**		 independent objects and privileges into IIDBDEPENDS and IIPRIV,
**		 respectively
**	    endif
**	    update IIPROCEDURE.dbp_mask1 from db_mask[0] contained in the
**	    tuple supplied by the caller;
**	endif
**	
**	If there is no current transaction, a multi-statement transaction
**	is created on behalf of the user.
**
** Inputs:
**      qef_cb                  qef session control block
**      qeuq_cb
**	    .qeuq_eflag	        designate error handling semantis
**				for user errors.
**		QEF_INTERNAL	return error code.
**		QEF_EXTERNAL	send message to user.
**	    .qeuq_dbp_tup	iiprocedure tuple
**	      .dt_data
**	        .db_dbpname     name of the procedure
**		.db_owner	owner of the procedure
**		.db_mask[0]	new value for dbp_mask1
**		.db_dbp_ubt_id	id of the dbproc's underlying base table if one
**				was found; (0,0) otherwise; guaranteed to be 
**				valid only if we are called to (possibly among 
**				other things) enter dbproc's independent 
**				object/privilege list
**	    .qeuq_db_id		database id
**	    .qeuq_d_id		dmf session id
**	    .qeuq_indep		independent object and privilege descriptors
**		psq_objs	points to descriptor(s) of independent object(s)
**				on which a new dbproc depends
**		psq_objprivs	descriptor(s) of table-wide privileges on which
**				a new dbproc depends
**		psq_colprivs	descriptor(s) of column-specific privileges on
**				which a new dbproc depends
**
** Outputs:
**      qeuq_cb
**	    .error.err_code	one of the following
**				E_QE0002_INTERNAL_ERROR
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0022_ABORTED
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	26-jun-92 (andre)
**	    written for GRANT/REVOKE project
**	05-feb-93 (andre)
**	    if we end up altering a DB_PROCEDURE tuple, call RDF to invalidate
**	    the cache entry for the dbproc
**	01-sep-93 (andre)
**	    when calling us to enter independent object/privilege list, caller 
*	    will pass id of the dbproc's underlying base table which we must 
**	    copy into DB_PROCEDURE.db_dbp_ubt_id 
*/
DB_STATUS
qeu_dbp_status(
QEF_CB          *qef_cb,
QEUQ_CB		*qeuq_cb)
{
    GLOBALREF QEF_S_CB      *Qef_s_cb;
    DB_PROCEDURE	    dbptuple, *in_dbp_tuple;
    RDF_CB		    rdfcb, *rdf_inv_cb = &rdfcb;
    RDR_RB		    *rdf_inv_rb = &rdf_inv_cb->rdf_rb;
    QEF_DATA		    dbpqef_data;
    DB_STATUS		    status, local_status;
    i4		    error;
    bool		    transtarted = FALSE;
    bool		    dbp_opened = FALSE;
    bool		    err_already_reported = FALSE;
    DMR_ATTR_ENTRY	    dbpkey_array[2];
    DMR_ATTR_ENTRY	    *dbpkey_ptr_array[2];
    i4			    i;
    QEU_CB		    tranqeu;
    QEU_CB		    dbpqeu;
    i4			    exit_loop;

    for (exit_loop = FALSE; !exit_loop; )
    {
	/* Check the control block. */

	if (qeuq_cb->qeuq_type != QEUQCB_CB || 
	    qeuq_cb->qeuq_length != sizeof(QEUQ_CB))	
	{
	    error = E_QE0017_BAD_CB;
	    status = E_DB_ERROR;
	    break;
	}

        if ((qeuq_cb->qeuq_db_id == 0)
	    || (qeuq_cb->qeuq_dbp_tup == (QEF_DATA *)NULL)
	    || (qeuq_cb->qeuq_indep == (PTR) NULL)
	   ) 
	{
	    error = E_QE0018_BAD_PARAM_IN_CB;
	    status = E_DB_ERROR;
	    break;
	}
		
	/* 
	** Check to see if a transaction is in progress,
	** if so, set transtarted flag to FALSE, otherwise
	** set it to TRUE after beginning a transaction.
	** If we started a transaction then we will abort
        ** it if an error occurs, otherwise we will just
        ** return the error and let the caller abort it.
	*/

	if (qef_cb->qef_stat == QEF_NOTRAN)
	{
	    tranqeu.qeu_type = QEUCB_CB;
	    tranqeu.qeu_length = sizeof(QEUCB_CB);
	    tranqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
	    tranqeu.qeu_d_id = qeuq_cb->qeuq_d_id;
	    tranqeu.qeu_flag = 0;
	    tranqeu.qeu_mask = 0;
	    status = qeu_btran(qef_cb, &tranqeu);
	    if (DB_FAILURE_MACRO(status))
	    {	
		error = tranqeu.error.err_code;
		break;	
	    }	    
	    transtarted = TRUE;
	}
	/* escalate the transaction to MST */
	if (qef_cb->qef_auto == QEF_OFF)
	    qef_cb->qef_stat = QEF_MSTRAN;

	dbpqeu.qeu_type = QEUCB_CB;
        dbpqeu.qeu_length = sizeof(QEUCB_CB);
        dbpqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
        dbpqeu.qeu_lk_mode = DMT_IX;
        dbpqeu.qeu_flag = DMT_U_DIRECT;
        dbpqeu.qeu_access_mode = DMT_A_WRITE;
	dbpqeu.qeu_mask = 0;
	dbpqeu.qeu_qual = dbpqeu.qeu_f_qual = 0;
	dbpqeu.qeu_qarg = dbpqeu.qeu_f_qarg = 0;

	dbpqeu.qeu_tab_id.db_tab_base = DM_B_DBP_TAB_ID;
	dbpqeu.qeu_tab_id.db_tab_index  = DM_I_DBP_TAB_ID;
	status = qeu_open(qef_cb, &dbpqeu);
	if (status != E_DB_OK)
	{
	    error = dbpqeu.error.err_code;
	    break;
	}
	dbp_opened = TRUE;

	dbpqeu.qeu_count = 1;
	dbpqeu.qeu_tup_length = sizeof(DB_PROCEDURE);
	dbpqeu.qeu_output = dbpqeu.qeu_input = &dbpqef_data;
	dbpqef_data.dt_next = 0;
	dbpqef_data.dt_size = sizeof(DB_PROCEDURE);
	dbpqef_data.dt_data = (PTR) &dbptuple;

	dbpqeu.qeu_flag = 0;
	in_dbp_tuple = (DB_PROCEDURE *) qeuq_cb->qeuq_dbp_tup->dt_data;
	dbpqeu.qeu_klen = 2;
	dbpqeu.qeu_key = dbpkey_ptr_array;
    
	dbpkey_ptr_array[0] = &dbpkey_array[0];
	dbpkey_ptr_array[0]->attr_number = DM_1_DBP_KEY;
	dbpkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	dbpkey_ptr_array[0]->attr_value = (char *) &in_dbp_tuple->db_dbpname;
    
	dbpkey_ptr_array[1] = &dbpkey_array[1];
	dbpkey_ptr_array[1]->attr_number = DM_2_DBP_KEY;
	dbpkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	dbpkey_ptr_array[1]->attr_value = (char *) &in_dbp_tuple->db_owner;

	dbpqeu.qeu_getnext = QEU_REPO;

	status = qeu_get(qef_cb, &dbpqeu);
	if (status != E_DB_OK)
	{
	    error = dbpqeu.error.err_code;
	    break;
	}

	/*
	** first make sure that the tuple supplied by the user is conveying new
	** information - if two sessions were dealing with the same dbproc, they
	** both could try to enter the same data into catalogs
	*/
	if (BTsubset((char *) in_dbp_tuple->db_mask, (char *) dbptuple.db_mask,
		BITS_IN(in_dbp_tuple->db_mask[0])))
	{
	    break;
	}

	/*
	** if the caller passed us an independent object/privilege list and
	** there is currently none in IIDBDEPENDS/IIPRIV, call
	** qeu_enter_dependencies() to enter the list
	*/
	if (   in_dbp_tuple->db_mask[0] & DB_DBP_INDEP_LIST
	    && ~dbptuple.db_mask[0] & DB_DBP_INDEP_LIST)
	{
	    status = qeu_enter_dependencies(qef_cb, qeuq_cb,
		&dbptuple.db_procid, (DB_QMODE) DB_DBP, (i2) 0);

	    if (DB_FAILURE_MACRO(status))
	    {
		err_already_reported = TRUE;
		break;
	    }

	    /* copy id of the dbproc's underlying base table */
	    dbptuple.db_dbp_ubt_id.db_tab_base = 
		in_dbp_tuple->db_dbp_ubt_id.db_tab_base;
	    dbptuple.db_dbp_ubt_id.db_tab_index = 
		in_dbp_tuple->db_dbp_ubt_id.db_tab_index;
	}

	/* finally, we update IIPROCEDURE.dbp_mask1 from db_mask[0] */
	dbptuple.db_mask[0] = in_dbp_tuple->db_mask[0];

	/*
	** must set qeu_klen to 0 so qeu_replace() will know to update the
	** current tuple
	*/
	dbpqeu.qeu_klen = 0;
	dbpqeu.qeu_key = (DMR_ATTR_ENTRY **) NULL;

	/*
	** qeu_flag was set to 0 which is good (as long as it is not QEU_BY_TID)
	*/
	status = qeu_replace(qef_cb, &dbpqeu);
	if (status != E_DB_OK)
	{
	    error = dbpqeu.error.err_code;
	    break;
	}

	/*
	** now we need to invalidate the entry for this dbproc from
	** the RDF's cache
	*/
	qeu_rdfcb_init((PTR) rdf_inv_cb, qef_cb);

	rdf_inv_rb->rdr_types_mask  = RDR_PROCEDURE;

	/* dbproc info is cached by name and owner - store them in RDF_CB */
	STRUCT_ASSIGN_MACRO(dbptuple.db_owner, rdf_inv_rb->rdr_owner);
	STRUCT_ASSIGN_MACRO(dbptuple.db_dbpname,
			    rdf_inv_rb->rdr_name.rdr_prcname);

	status = rdf_call(RDF_INVALIDATE, rdf_inv_cb);
	if (DB_FAILURE_MACRO(status))
	{
	    error = rdf_inv_cb->rdf_error.err_code;
	    break;
	}

	exit_loop = TRUE;
    }
    /* call qef_error to handle error messages, if any */
    if (DB_FAILURE_MACRO(status))
	if (!err_already_reported)
	    (VOID) qef_error(error, 0L, status, &error, &qeuq_cb->error, 0);
	else
	    err_already_reported = FALSE;
    
    /* Close off all the tables. */
    if (dbp_opened)
    {
	local_status = qeu_close(qef_cb, &dbpqeu);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(dbpqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (transtarted)
    {
	if (status == E_DB_OK)
	{
	    status = qeu_etran(qef_cb, &tranqeu);
	    if (DB_FAILURE_MACRO(status))
	    {
		(VOID) qef_error(tranqeu.error.err_code, 0L, status, 
			    &error, &qeuq_cb->error, 0);
	    }
	}
	
	if (status != E_DB_OK)
	{
	    local_status = qeu_atran(qef_cb, &tranqeu);
	    if (local_status != E_DB_OK)
	    {
		(VOID) qef_error(tranqeu.error.err_code, 0L, local_status, 
			    &error, &qeuq_cb->error, 0);
		if (local_status > status)
		    status = local_status;
	    }
	}
    }

    return (status);
}

/*{
** Name: QEU_DRPDBP	- delete a DB procedure definition
**
** Synopsis:
**	DB_STATUS   status;
**	QEUQ_CB	    *qeuq_cb;
**
**	    status = qef_call(QEU_DRPDBP, &qeuq_cb);
**
** Description:
**	Given a dbproc name and a dbproc owner name, this function will try to
**	lookup the dbproc in IIPROCEDURE.  If successful, it will then invoke
**	qeu_d_cascade() to handle cascading destruction of the dbproc.
**
**	If there is no current transaction, a multi-statement transaction is
**	created on behalf og the user.
**
**
** Inputs:
**      qef_cb                  qef session control block
**      qeuq_cb
**	    .qeuq_eflag	        designate error handling semantics
**				for user errors.
**		QEF_INTERNAL	return error code.
**		QEF_EXTERNAL	send message to user.
**	    .qeuq_dbp_tup	iiprocedure tuple containing the following:
**		.db_dbpname	name of the procedure to be deleted.
**		.db_owner	owner of the above procedure.
**	    .qeuq_db_id		database id
**	    .qeuq_d_id		dmf session id
**
** Outputs:
**      qeuq_cb
**	    .error.err_code	one of the following
**				E_QE0002_INTERNAL_ERROR
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0022_ABORTED
**				E_QE0117_NONEXISTENT_DBP
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	02-may-88 (puree)
**	    created for DB procedure.
**	24-jun-92 (andre)
**	    changed the function so that after successful lookup of a database
**	    procedure qeu_d_cascade() will be called to perform cascading
**	    destruction
**	10-sep-93 (andre)
**	    set QEU_FORCE_QP_INVALIDATION in dqeuq.qeuq_flag_mask
**	    before calling qeu_d_cascade() which will know then to invalidate 
**	    QPs for this dbproc
**	08-oct-93 (andre)
**	    qeu_d_cascade() expects one more parameter - an address of a 
**	    DMT_TBL_ENTRY describing table/view/index being dropped; for all 
**	    other types of objects, NULL must be passed
**	28-oct-93 (andre)
**	    (fix for bug 51087)
**	    if the dbproc to be dropped does not exist (which would require 
**	    that RDF cache contain a stale entry describing a no longer
**	    existant dbproc), issue error 2405 (which says that the dbproc was 
**	    not found) rather than QE0117 which is more appropriate when 
**	    attempting to process creation/destruction of privileges on a 
**	    dbproc.
*/
DB_STATUS
qeu_drpdbp(
QEF_CB          *qef_cb,
QEUQ_CB		*qeuq_cb)
{
    DB_STATUS	    status, local_status;
    DB_ERROR	    err;
    i4	    error;
    bool	    transtarted = FALSE;	    
    bool	    dbp_opened = FALSE;
    QEU_CB	    tranqeu;
    QEU_CB	    dbpqeu;
    QEF_DATA	    dbpqef_data;
    DB_PROCEDURE    dbptuple, *dbp_key_tuple;
    DMR_ATTR_ENTRY  dbpkey_array[2];
    DMR_ATTR_ENTRY  *dbpkey_ptr_array[2];
    DB_ERROR	    e_error;
    i4		    exit_loop;

    for (exit_loop = FALSE; !exit_loop; )
    {
	/* Check the control block. */

	if (qeuq_cb->qeuq_type != QEUQCB_CB || 
	    qeuq_cb->qeuq_length != sizeof(QEUQ_CB))	
	{
	    error = E_QE0017_BAD_CB;
	    status = E_DB_ERROR;
	    break;
	}

        if (qeuq_cb->qeuq_db_id == 0)
	{
	    error = E_QE0018_BAD_PARAM_IN_CB;
	    status = E_DB_ERROR;
	    break;
	}
		
	/* 
	** Check to see if a transaction is in progress,
	** if so, set transtarted flag to FALSE, otherwise
	** set it to TRUE after beginning a transaction.
	** If we started a transaction then we will abort
        ** it if an error occurs, otherwise we will just
        ** return the error and let the caller abort it.
	*/

	if (qef_cb->qef_stat == QEF_NOTRAN)
	{
	    tranqeu.qeu_type = QEUCB_CB;
	    tranqeu.qeu_length = sizeof(QEUCB_CB);
	    tranqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
	    tranqeu.qeu_d_id = qeuq_cb->qeuq_d_id;
	    tranqeu.qeu_flag = 0;
	    tranqeu.qeu_mask = 0;
	    status = qeu_btran(qef_cb, &tranqeu);
	    if (DB_FAILURE_MACRO(status))
	    {	
		error = tranqeu.error.err_code;
		break;	
	    }	    
	    transtarted = TRUE;
	}
	/* escalate the transaction to MST */
	if (qef_cb->qef_auto == QEF_OFF)
	    qef_cb->qef_stat = QEF_MSTRAN;

	/* Read the query text id and dbproc id of the dbproc from iiprocedure 
	** open the iiprocedure in write mode. We will delete a tuple
	** from this catalog later.
	**
	** Now open iiprocedure table.
        */
	dbpqeu.qeu_type = QEUCB_CB;
        dbpqeu.qeu_length = sizeof(QEUCB_CB);
        dbpqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
        dbpqeu.qeu_lk_mode = DMT_IX;
        dbpqeu.qeu_flag = DMT_U_DIRECT;
        dbpqeu.qeu_access_mode = DMT_A_WRITE;
        dbpqeu.qeu_tab_id.db_tab_base = DM_B_DBP_TAB_ID; 	
        dbpqeu.qeu_tab_id.db_tab_index = DM_I_DBP_TAB_ID; 	
	dbpqeu.qeu_mask = 0;

	status = qeu_open(qef_cb, &dbpqeu);
	if (DB_FAILURE_MACRO(status))
	{
	    error = dbpqeu.error.err_code;
	    break;
	}
	dbp_opened = TRUE;

	/*
	** Get the iiprocedure tuple that matches procedure and owner names 
	** Assume only one iiprocedure tuple per call
	*/
	dbp_key_tuple = (DB_PROCEDURE *) qeuq_cb->qeuq_dbp_tup->dt_data;

	dbpqeu.qeu_count = 1;
        dbpqeu.qeu_tup_length = sizeof(DB_PROCEDURE);
	dbpqeu.qeu_output = &dbpqef_data;
	dbpqef_data.dt_next = 0;
        dbpqef_data.dt_size = sizeof(DB_PROCEDURE);
        dbpqef_data.dt_data = (PTR) &dbptuple;
	dbpqeu.qeu_getnext = QEU_REPO;
	dbpqeu.qeu_klen = 2;       
	dbpqeu.qeu_key = dbpkey_ptr_array;
	
	dbpkey_ptr_array[0] = &dbpkey_array[0];
	dbpkey_ptr_array[0]->attr_number = DM_1_DBP_KEY;
	dbpkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	dbpkey_ptr_array[0]->attr_value = (char *) &dbp_key_tuple->db_dbpname;

	dbpkey_ptr_array[1] = &dbpkey_array[1];
	dbpkey_ptr_array[1]->attr_number = DM_2_DBP_KEY;
	dbpkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	dbpkey_ptr_array[1]->attr_value = (char *) &dbp_key_tuple->db_owner;
    
	dbpqeu.qeu_qual = 0;
	dbpqeu.qeu_qarg = 0;
 
	status = qeu_get(qef_cb, &dbpqeu);
	if (status != E_DB_OK)
	{
	    if (dbpqeu.error.err_code == E_QE0015_NO_MORE_ROWS)
		error = 2405L;
	    else
		error = dbpqeu.error.err_code;
	}

	exit_loop = TRUE;
    }
    
    if (status != E_DB_OK)
    {
	/* 
	** error 2405 takes one argument; other errors which we expect
	** here take 0 arguments
	*/
	if (error == 2405L)
	{
	    (VOID) qef_error(error, 0L, status, &error, &qeuq_cb->error, 1,
	        qec_trimwhite(sizeof(dbp_key_tuple->db_dbpname),
		    (char *) &dbp_key_tuple->db_dbpname),
	        (PTR) &dbp_key_tuple->db_dbpname);
	}
	else
	{
	    (VOID) qef_error(error, 0L, status, &error, &qeuq_cb->error, 0);
	}
    }
    else
    {
	QEUQ_CB     dqeuq;

	/*
	** perform cascading destruction - qeu_d_cascade() handles its
	** own error reporting
	*/
	STRUCT_ASSIGN_MACRO(*qeuq_cb, dqeuq);
	dqeuq.qeuq_rtbl = &dbptuple.db_procid;
	dqeuq.qeuq_flag_mask &= ~((i4) QEU_DROP_TEMP_TABLE);
	dqeuq.qeuq_flag_mask |= QEU_FORCE_QP_INVALIDATION;
	STRUCT_ASSIGN_MACRO(dbptuple.db_txtid, dqeuq.qeuq_qid);
	dqeuq.qeuq_dbp_tup = &dbpqef_data;

	/* perform cascading destruction of the dbproc */
	status = qeu_d_cascade(qef_cb, &dqeuq, (DB_QMODE) DB_DBP, 
	    (DMT_TBL_ENTRY *) NULL);

	if (status != E_DB_OK)
	{
	    STRUCT_ASSIGN_MACRO(dqeuq.error, qeuq_cb->error);
	}
    }

    if ( status == E_DB_OK && Qef_s_cb->qef_state & QEF_S_C2SECURE )
    {
	/* This is a security event, must audit. */
	status = qeu_secaudit(FALSE, qef_cb->qef_ses_id, 
	      (char *)&dbptuple.db_dbpname, &dbptuple.db_owner,
	      sizeof(dbptuple.db_dbpname), SXF_E_PROCEDURE,
	      I_SX2013_DBPROC_DROP, SXF_A_SUCCESS | SXF_A_DROP,
	      &e_error);

	if (DB_FAILURE_MACRO(status))
	    (VOID) qef_error(e_error.err_code, 0L, status, &e_error.err_code, &qeuq_cb->error, 0);
    }

    /* Close IIPROCEDURE */

    if (dbp_opened)
    {
	local_status = qeu_close(qef_cb, &dbpqeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(dbpqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (transtarted)
    {
	if (status == E_DB_OK)
	{
	    status = qeu_etran(qef_cb, &tranqeu);
	    if (DB_FAILURE_MACRO(status))
	    {
		(VOID) qef_error(tranqeu.error.err_code, 0L, status, 
			    &error, &qeuq_cb->error, 0);
	    }
	}
	
	if (status != E_DB_OK)
	{
	    local_status = qeu_atran(qef_cb, &tranqeu);
	    if (local_status != E_DB_OK)
	    {
		(VOID) qef_error(tranqeu.error.err_code, 0L, local_status, 
			    &error, &qeuq_cb->error, 0);
		if (local_status > status)
		    status = local_status;
	    }
	}
    }

    return (status);
}

/*{
** Name: QEU_CVIEW	- store a view definition
**
** External QEF call:   status = qef_call(QEU_CVIEW, &qeuq_cb);
**
** Description:
**      A view is created. The following algorithm is employed.  Given the
**	base table id(or a list of ids), the DMU_CB that creates tables,
**	and the tree and iiqrytext information, QEU_CVIEW does the following.
**	The view is created and entry are made in the iidbdepends catalog
**	connecting the table ids with the view id.  A unique qrytext id is
**	created and is used to update the iiqrytext tuples before appending 
**	them. The tree tuples are updated with their table_id and tree_id 
**	before being appended to the tree catalog.
**
** Inputs:
**      qef_cb                  qef session control block
**      qeuq_cb
**	    .qeuq_eflag	        designate error handling semantis
**				for user errors.
**		QEF_INTERNAL	return error code.
**		QEF_EXTERNAL	send message to user.
**	    .qeuq_tsz		number of base relations
**	    .qeuq_tbl_id	array of table ids of base relation
**	    .qeuq_cq		number of query text tuples
**	    .qeuq_qry_tup	query text tuples
**	    .qeuq_ct		number of tree catalog tuples
**	    .qeuq_tre_tup	tree catalog tuples
**	    .qeuq_dmf_cb	dmu_cb for creating the view	    
**	    .qeuq_db_id		database id
**	    .qeuq_d_id		dmf session id
**	    .qeuq_indep		independent object and privilege descriptors
**		psq_objs	points to an independent object descriptor
**		psq_objprivs	descriptor(s) of table-wide privileges on which
**				a new view depends
**		psq_colprivs	descriptor(s) of column-specific privileges on
**				which a new view depends
**
** Outputs:
**      qeuq_cb
**	    .error.err_code	one of the following
**				E_QE0002_INTERNAL_ERROR
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0022_ABORTED
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	16-jun-86 (daved)
**          written
**      18-jul-86 (jennifer)
**          Initial coding.
**      20-aug-86 (jennifer)
**	    Changed all transaction calls from QET_xxxx
**          to QEU_xxx.  
**	23-dec-86 (daved) 
**	    close off tables, even in error condition
**	1-june-87 (daved)
**	    create views with no base tables
**	16-june-87 (daved)
**	    alter each base table in a view to have the dmt_base_view
**	    set. this is done by useing the dmt_c_views characteristic
**	17-june-87 (daved)
**	    if index is on base table, make sure that base table is entered
**	    into the IIDBDEPENDS table.
**	28-jul-87 (daved)
**	    perform the alter table commands prior to opening the qrymod
**	    tables incase the qrymod tables are used in a view.
**      10-dec-87 (puree)
**          Converted all ulm_palloc to qec_palloc
**	20-jan-88 (puree)
**	    Convert all qet_escalate calls to in-line codes.
**	05-oct-88 (puree)
**	    modified qeu_cview due to change in user error 5102 format by PSF.
**	24-oct-88 (puree)
**	    fix string length for message 5102.
**      29-jun-89 (jennifer)
**          Added security auditing.
**	05-mar-90 (andre)
**	    If a QUEL view owned by the DBA has been successfully created,
**	    insert an "access" permit into IIPROTECT (needed by the FEs for
**	    backward compatibility.)
**	21-jan-92 (andre)
**	    qeuq_cb->qeuq_indep will point to a structure describing objects and
**	    privileges on which a new view depends
**	29-may-92 (andre)
**	    store the list of privileges on which this view (and all SELECT
**	    privileges on it) will depend into IIPRIV and declare dependence
**	    between the new view and the privilege descriptor.
**	25-jun-92 (andre)
**	    call qeu_enter_dependencies() to record objects and privileges on
**	    which the new view depends.
**	15-sep-92 (andre)
**	    when calling qeu_enter_dependencies(), do not try to differentiate
**	    between a QUEL view and an SQL view (i.e. always pass DB_VIEW as
**	    the object type.)
**      01-mar-93 (andre)
**          having created the view, pass its id to the caller
**	19-may-93 (anitap)
**	    Added support for creation of implicit schema for views.
**      07-jul-93 (anitap)
**          Added arguments qef_rcb and qeuq_cb to qea_schema() instead of
**          PTR. Changed assignment of flag to IMPLICIT_SCHEMA.
**	26-oct-93 (andre)
**	    As a part of fix for bug 51852 (which was reported in 6.4 but can
**	    and will manifest itself in 6.5 as soon as someone places some 
**	    stress on the DBMS), we want to use object id (really guaranteed to
**	    be unique) instead of timestamps (allegedly unique, but in fact may
**	    be non-unique if several objects get created in rapid succession on
**	    a reasonably fast box) to identify IIQRYTEXT and IITREE tuples 
**	    associated with a given view.  
**
**	    This requires that instead of computing the query text (and tree) id
**	    here, we trust dm2u_create() to compute it (by calling
**	    ulb_rev_bits() on the components of view id) and return it inside
**	    dmu_cb->dmu_qry_id.
**
**	    Another interesting point:
**	    instead of keying into IITREE by object id, we could key in using 
**	    the "randomized" id which would permit us to do away with the 
**	    timestamp in IITREE altogether - this would require that we key 
**	    IITREE on first half of "randomized" id, object type, and second 
**	    half of "randomized" - whether this happens or not depends on how 
**	    long this whole thing will take and how much RDF resistance this 
**	    idea is likely to encounter.)
**	07-Jun-2005 (inifa01) b111211 INGSRV2580
**	    Verifydb reports error; S_DU1619_NO_VIEW iirelation indicates that
**	    there is a view defined for table a (owner ingres), but none exists,
**	    when view(s) on a table are dropped.
**	    Were making DMT_ALTER call on DMT_C_VIEW, to turn on iirelation.relstat
**	    TCB_VBASE bit, indicating view is defined over table(s).  TCB_VBASE
**	    not tested anywhere in 2.6 and not turned off when view is dropped.
**	    Removing DMT_ALTER call here so TCB_VBASE is no longer set.
**	    All references to TCB_VBASE will be removed.
**	23-nov-2006 (dougi)
**	    Reinstate code to do DMT_C_VIEWS calls.
**	12-dec-2006 (dougi)
**	    Skip derived tables.
*/
DB_STATUS
qeu_cview(
QEF_CB          *qef_cb,
QEUQ_CB		*qeuq_cb)
{
    DMU_CB	    *dmu_cb;
    DB_IIQRYTEXT    *qtuple;
    DB_IITREE	    *ttuple;
    QEF_DATA	    *next;
    DB_STATUS	    status, local_status;
    i4	    error;
    bool	    transtarted = FALSE;	    
    bool	    tbl_opened = FALSE;
    bool	    err_already_reported = FALSE;
    i4		    i;
    QEU_CB	    tranqeu;
    QEU_CB	    qeu;
    DMT_CB	    dmt_cb;
    DB_TAB_ID	    viewid;
    DB_TAB_ID	    randomized_id;
    DMT_CHAR_ENTRY  char_array[1];
    i4		    flag = IMPLICIT_SCHEMA;		
    QEF_RCB         *qef_rcb = (QEF_RCB *)NULL;
    i4		    exit_loop;
    
    for (exit_loop = FALSE; !exit_loop; )
    {

	/* Check the control block. */

	error = E_QE0017_BAD_CB;
	status = E_DB_ERROR;
	if (qeuq_cb->qeuq_type != QEUQCB_CB ||
            qeuq_cb->qeuq_length != sizeof(QEUQ_CB))	
	    break;

	/* 
        ** For a view, query text and tree tuples 
        ** must be provided, as well as the dmf control
        ** block to create the view; not to mention the list of independent
	** objects/privileges on which the view depends
        */

	error = E_QE0018_BAD_PARAM_IN_CB;
	if (   (qeuq_cb->qeuq_cq == 0 || qeuq_cb->qeuq_qry_tup == 0)
            || (qeuq_cb->qeuq_ct == 0 || qeuq_cb->qeuq_tre_tup == 0)
            || (qeuq_cb->qeuq_db_id == 0)
            || (qeuq_cb->qeuq_d_id == 0)
            || (qeuq_cb->qeuq_dmf_cb == 0)
	    || (qeuq_cb->qeuq_indep == 0)
		/*
		** qeuq_rtbl must be non-NULL so that we can pass ID of the
		** view to the caller
		*/
	    || (qeuq_cb->qeuq_rtbl == (DB_TAB_ID *) NULL))

	    break;
		

	/* 
	** Check to see if a transaction is in progress,
	** if so, set transtarted flag to FALSE, otherwise
	** set it to TRUE after beginning a transaction.
	** If we started a transaction them we will abort
        ** it if an error occurs, otherwise we will just
        ** return the error and let the caller abort it.
	*/

	if (qef_cb->qef_stat == QEF_NOTRAN)
	{
	    tranqeu.qeu_type = QEUCB_CB;
	    tranqeu.qeu_length = sizeof(QEUCB_CB);
	    tranqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
	    tranqeu.qeu_d_id = qeuq_cb->qeuq_d_id;
	    tranqeu.qeu_flag = 0;
	    tranqeu.qeu_mask = 0;
	    status = qeu_btran(qef_cb, &tranqeu);
	    if (status != E_DB_OK)
	    {	
		error = tranqeu.error.err_code;
		break;	
	    }	    
	    transtarted = TRUE;
	}
	/* escalate the transaction to MST */
	if (qef_cb->qef_auto == QEF_OFF)
	    qef_cb->qef_stat = QEF_MSTRAN;

	/* Now create the view */	
	dmu_cb = (DMU_CB *)qeuq_cb->qeuq_dmf_cb;
	dmu_cb->dmu_flags_mask = 0;

	if (qeuq_cb->qeuq_flag_mask & QEU_VGRANT_OK)
	{
	    dmu_cb->dmu_flags_mask |= DMU_VGRANT_OK;
	}

	/* 
	** recall that dm2u_create() will compute text id and store it in 
	** dmu_cb->dmu_qry_id
	*/
	dmu_cb->dmu_tran_id = qef_cb->qef_dmt_id;

	status = dmf_call(DMU_CREATE_TABLE, dmu_cb);
	if (status != E_DB_OK)
	{
	    if (qeuq_cb->qeuq_eflag == QEF_EXTERNAL &&
		dmu_cb->error.err_code == E_DM0078_TABLE_EXISTS)
	    {
		(VOID) qef_error(5102L, 0L, status, &error, &qeuq_cb->error, 2,
		(sizeof("CREATE VIEW") - 1), "CREATE VIEW",
		sizeof(dmu_cb->dmu_table_name), (PTR) &dmu_cb->dmu_table_name);
		error = E_QE0025_USER_ERROR;
	    }
	    else
		error = dmu_cb->error.err_code;
	    break;
	}	

	/* text/tree id is in dmu_cb->dmu_qry_id */
	randomized_id.db_tab_base  = dmu_cb->dmu_qry_id.db_qry_high_time;
	randomized_id.db_tab_index = dmu_cb->dmu_qry_id.db_qry_low_time;

	/* Save table id of view just created. */
	
	viewid.db_tab_base = dmu_cb->dmu_tbl_id.db_tab_base;
	viewid.db_tab_index = dmu_cb->dmu_tbl_id.db_tab_index;

	/* give the caller id of the newly created view */
	STRUCT_ASSIGN_MACRO(viewid, (*qeuq_cb->qeuq_rtbl));

	/* mark the base tables as being used in a view. 
	**
        ** For each table associated with this view
        ** build a view tuple and write it to table.
        */
	dmt_cb.length = sizeof(DMT_CB);
	dmt_cb.type = DMT_TABLE_CB;
	dmt_cb.dmt_tran_id = qef_cb->qef_dmt_id;
	dmt_cb.dmt_flags_mask		    = 0;
	dmt_cb.dmt_db_id		    = qeuq_cb->qeuq_db_id;
	dmt_cb.dmt_char_array.data_address  = (PTR) char_array;
	dmt_cb.dmt_char_array.data_in_size  = sizeof(DMT_CHAR_ENTRY);
	char_array[0].char_id		    = DMT_C_VIEWS;
	char_array[0].char_value	    = DMT_C_ON;
	for (i = 0; i < qeuq_cb->qeuq_tsz; i++)	
	{
	    dmt_cb.dmt_id.db_tab_base = qeuq_cb->qeuq_tbl_id[i].db_tab_base;
	    dmt_cb.dmt_id.db_tab_index = qeuq_cb->qeuq_tbl_id[i].db_tab_index;

	    /* Skip over derived tables (tabl_id = {0, 0}). */
	    if (dmt_cb.dmt_id.db_tab_base == 0 &&
		dmt_cb.dmt_id.db_tab_index == 0)
		continue;

	    status = dmf_call(DMT_ALTER, &dmt_cb);
	    if (status != E_DB_OK)
	    {
		error = dmt_cb.error.err_code;
		break;
	    }
	    
	}
	if (status != E_DB_OK)
	    break;

	/*
	** initialize invariable fields in qeu that will be used when accessing
	** various catalogs
	*/
	qeu.qeu_type	    = QEUCB_CB;
        qeu.qeu_length	    = sizeof(QEUCB_CB);
        qeu.qeu_db_id	    = qeuq_cb->qeuq_db_id;
	qeu.qeu_d_id	    = qeuq_cb->qeuq_d_id;
        qeu.qeu_lk_mode	    = DMT_IX;
        qeu.qeu_flag	    = DMT_U_DIRECT;
        qeu.qeu_access_mode = DMT_A_WRITE;
	qeu.qeu_mask	    = 0;

	/* Now Update the iiqrytext and iitree system tables. */
	qeu.qeu_tab_id.db_tab_base = DM_B_QRYTEXT_TAB_ID;
	qeu.qeu_tab_id.db_tab_index = 0L;
	status = qeu_open(qef_cb, &qeu);
	if (status != E_DB_OK)
	{
	    error = qeu.error.err_code;
	    break;
	}
	tbl_opened = TRUE;

	/* Update all query text tuples with query id. */

	next = qeuq_cb->qeuq_qry_tup;
	for (i=0; i<qeuq_cb->qeuq_cq; i++)
	{
	    qtuple = (DB_IIQRYTEXT*) next->dt_data;
	    next = next->dt_next;
	    qtuple->dbq_txtid.db_qry_high_time
                       = randomized_id.db_tab_base;
	    qtuple->dbq_txtid.db_qry_low_time
                       = randomized_id.db_tab_index;
	    if (i < (qeuq_cb->qeuq_cq - 1) && next == 0)
	    {
		error = E_QE0018_BAD_PARAM_IN_CB;
		status = E_DB_ERROR;
		break;
	    }
	}
	if (status != E_DB_OK)    		
	    break;

	qeu.qeu_count = qeuq_cb->qeuq_cq;
        qeu.qeu_tup_length = sizeof(DB_IIQRYTEXT);
	qeu.qeu_input = qeuq_cb->qeuq_qry_tup;
	status = qeu_append(qef_cb, &qeu);
	if (status != E_DB_OK)
	{
	    error = qeu.error.err_code;
	    break;
	}
	    
	status = qeu_close(qef_cb, &qeu);    
	if (status != E_DB_OK)
	{
	    error = qeu.error.err_code;
	    break;
	}
	tbl_opened = FALSE;
	

        qeu.qeu_tab_id.db_tab_base = DM_B_TREE_TAB_ID;
        qeu.qeu_tab_id.db_tab_index = DM_I_TREE_TAB_ID;
	status = qeu_open(qef_cb, &qeu);
	if (status != E_DB_OK)
	{
	    error = qeu.error.err_code;
	    break;
	}
	tbl_opened = TRUE;
    		
	/* Update all tree information tuples with tree id. */

	next = qeuq_cb->qeuq_tre_tup;
	for (i = 0; i < qeuq_cb->qeuq_ct; i++)
	{
	    ttuple = (DB_IITREE*) next->dt_data;
	    next = next->dt_next;
	    ttuple->dbt_trid.db_tre_high_time
                       = randomized_id.db_tab_base;
	    ttuple->dbt_trid.db_tre_low_time
                       = randomized_id.db_tab_index;
	    ttuple->dbt_tabid.db_tab_base
                       = viewid.db_tab_base;
	    ttuple->dbt_tabid.db_tab_index
                       = viewid.db_tab_index;
	    if (i < (qeuq_cb->qeuq_ct - 1) && next == 0)
	    {
		error = E_QE0018_BAD_PARAM_IN_CB;
		status = E_DB_ERROR;
		break;
	    }
	}
	if (status != E_DB_OK)    		
	    break;

	qeu.qeu_count = qeuq_cb->qeuq_ct;
        qeu.qeu_tup_length = sizeof(DB_IITREE);
	qeu.qeu_input = qeuq_cb->qeuq_tre_tup;
	status = qeu_append(qef_cb, &qeu);
	if (status != E_DB_OK)
	{
	    error = qeu.error.err_code;
	    break;
	}
	    
	status = qeu_close(qef_cb, &qeu);    
	if (status != E_DB_OK)
	{
	    error = qeu.error.err_code;
	    break;
	}
	tbl_opened = FALSE;
	
	if (   qeuq_cb->qeuq_flag_mask & QEU_DBA_VIEW
	    && qeuq_cb->qeuq_flag_mask & QEU_QUEL_VIEW)
	{
	    status = qeu_access_perm(qef_cb, &qeu, dmu_cb, QEU_TO_PUBLIC);

	    if (DB_FAILURE_MACRO(status))
	    {
		error = qeu.error.err_code;
		break;
	    }
	}
	
	/*
	** if the new view depends on other objects, this will be recorded in 
	** IIDBDEPENDS
	** 
	** if the new view depends on some privileges, this will be recorded in
	** IIPRIV
	*/
	status = qeu_enter_dependencies(qef_cb, qeuq_cb, &viewid,
	    (DB_QMODE) DB_VIEW, (i2) 0);

	if (DB_FAILURE_MACRO(status))
	{
	    /* qeu_enter_dependencies() reports its own errors */
	    err_already_reported = TRUE;
	    break;
	}

	/* call qea_schema() to create implicit schema */
	status = qea_schema(qef_rcb, qeuq_cb, qef_cb,
		(DB_SCHEMA_NAME *) &dmu_cb->dmu_owner, flag);
	if (status != E_DB_OK)
	{
	   error = qeuq_cb->error.err_code;
	   break;
	}
	
	if (transtarted)
	{
	    status = qeu_etran(qef_cb, &tranqeu);
	    if (status != E_DB_OK)
	    {
		error = tranqeu.error.err_code;
		break;
	    }
	}

	exit_loop = TRUE;
    } /* end for */

    if (status == E_DB_OK)
    {
	/* SUCCESS - we are done */
        qeuq_cb->error.err_code = E_QE0000_OK;
        return (E_DB_OK);
    }

    /*
    ** Only come here if error occurred. Unless the error has already been
    ** reported (as is done by qeu_enter_dependencies) call qef_error to handle
    ** error messages
    */ 

    if (!err_already_reported)
	(VOID) qef_error(error, 0L, status, &error, &qeuq_cb->error, 0);
    else
	err_already_reported = FALSE;
    
    /* Close off all the tables. */

    if (tbl_opened)
    {
	local_status = qeu_close(qef_cb, &qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(qeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (transtarted)
    {
        local_status = qeu_atran(qef_cb, &tranqeu);
        if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(tranqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    return (status);
}


/*{
** Name: QEU_DVIEW	- destroy a view definition
**
** External QEF call:   status = qef_call(QEU_DVIEW, &qeuq_cb);
**
** Description:
**	A table or a view and all views that depend on table or view are
**	destroyed.  Note, you can only destroy one table or view at a time.
**
**	This is a cascading view destruction.  Since it is not known
**	how many views are directly and indirectly dependent on the table 
**	(view) specified, a decision was made to make once pass to determine 
**	all dependent views which require destruction.  Then each of the 
**	dependents and their associated query text and tree information are 
**	deleted.
**
**	To insure this unbounded list of dependents can be built it is placed 
**	in a temporary table.  Once all dependents are found the table is 
**	rescanned.  Each tuple contains the table id of a view to destroy 
**	and the query id of that view.  All related information is destroyed 
**	and the view destroyed.  When the list is exhausted, then a final scan
**	of the IIDBDEPENDS system table is made to find all tables which are
**	a base for the original view that is being destroyed.  All view
**	tuples containing this view id are deleted, thereby cleaning up the 
**	IIDBDEPENDS table.  This last operation could be more efficient if a 
**	secondary index on view-id existed on this table.
**
** Inputs:
**      qef_cb                  qef session control block
**      qeuq_cb
**	    .qeuq_eflag	        designate error handling semantis
**				for user errors.
**		QEF_INTERNAL	return error code.
**		QEF_EXTERNAL	send message to user.
**	    .qeuq_rtbl		table id of view or table
**          .qeuq_qid           query id of view, if
**                              zero view is a base table. 
**	    .qeuq_db_id		database id
**	    .qeuq_d_id		dmf session id
**
** Outputs:
**      qeuq_cb
**	    .error.err_code	one of the following
**				E_QE0002_INTERNAL_ERROR
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0022_ABORTED
**				E_QE0023_INVALID_QUERY
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	16-jun-86 (daved)
**          written
**      22-jul-86 (jennifer)
**          Initially coded.
**      20-aug-86 (jennifer)
**	    Changed all transaction calls from QET_xxxx
**          to QEU_xxx.  
**	24-sep-86 (daved)
**	    corrected various spelling mistakes and changed the test
**	    when deleting the iiqrytext entry to not care if delete
**	    deleted anything when not deleting a view.
**	    Also use DMT_DBMS_REQUEST when creating the temporary relation.
**	23-dec-86 (daved) 
**	    close off tables, even in error condition
**	10-sep-87 (puree)
**	    call qef_error to issue error message before cleaning up.
**	06-nov-87 (puree)
**	    change error from qet_escalate to QE0005_CANNOT_ESCALATE.
**      10-dec-87 (puree)
**          Converted all ulm_palloc to qec_palloc
**	20-jan-88 (puree)
**	    Convert all qet_escalate calls to in-line codes.
**	10-feb-88 (puree)
**	    Convert ulm_openstream qec_mopen.  Rename qec_palloc to 
**	    qec_malloc.
**	09-mar-88 (puree)
**	    Change E_QE0027_BAD_QUERY_ID to E_QE0023_INVALID_QUERY to 
**	    allow retry by SCF when query text for a view is not found.
**	12-oct-88 (puree)
**	    Modified parameters to qeu_dprot.  Also handle error from qeu_dprot
**	08-nov-89 (neil)
**	    Clear qeuq_flag_mask before calling qeu_dprot to avoid collisions
**	    with other protection deletions.
**	02-jan-90 (nancy)
**	    Check for qef error mapping of NONEXISTENT TABLE after call to
**	    qeu_dprot since error code is already mapped and this is an error
**	    that can be ignored. (bug 9208 -- caused E_PS0903 error when 
**	    dropping table that had several views on it)
**	2-may-90 (andre)
**	    drop synonyms defined on tables,viewsa, indices being dropped.
**	04-may-90 (carol)
**	    Add cascading drops of comments.
**	21-jun-90 (linda)
**	    Change the check for whether or not the object is a view from
**		if ((qeuq_cb->qeuq_qid.db_qry_high_time  != 0) ||
**		    qeuq_cb->qeuq_qid.db_qry_low_time != 0))
**	    to a check based on a new field in qeuq_cb, qeuq_status_mask,
**	    from dmt_tbl_entry, telling us what kind of object we have.
**	    In particular we want to check for a gateway table, which has
**	    non-zero db_qry_high_time and db_qry_low_time, but must be
**	    treated differently from a view.
**	10-dec-90 (neil)
**	    Clear qeuq_flag_mask before calling qeu_dprot to avoid collisions
**	    with other protection deletions.
**	14-jan-91 (andre)
**	    use the newly defined function, qeu_dqrymod_objects(), to drop
**	    qrymod objects defined on a table or view being dropped.
**	10-feb-1992 (bryanp)
**	    Changed dmt_location from a DB_LOC_NAME to a DM_DATA.
**	    Initialize dmu_char_array fields in dmu_cb for DMU_DESTROY_TABLE.
**	09-jun-92 (andre)
**	    - when dropping a table/view, in addition to dropping tables
**	      and permits dependent on it as recorded in IIDBDEPENDS, we
**	      also need to:
**		- scan IIDBDEPENDS looking for dbprocs which depend on the
**		  object being dropped and remember to mark them as "dormant"
**		- if during rescanning the temp table we come across a dbproc
**		  entry, we must check IIDBDEPENDS for other dbprocs dependent
**		  on this dbproc
**		- scan IIPRIV for descriptions of privileges on the object
**		  being dropped on which other objects (priviilege descriptor 0)
**		  depend and enter them into the temp table
**	    - for views being destroyed, we must remember to destroy
**	      IIPRIV tuples constituting descriptors of privileges on which
**	      they or some privileegs on them depend;
**	    - for dbprocs being marked as dormant because some object on which
**	      they depend is being destroyed or marked as dormant, we must
**	      destroy IIPRIV tuples constituting descriptors of privileges on
**	      which they depend;
**	16-jun-92 (andre)
**	    add the object type to the temp. table to be able to distinguish
**	    views from dbprocs
**	23-jun-92 (andre)
**	    moved almost all code into a new function - qeu_d_cascade() - which
**	    will implement cascading destruction of various DBMS objects
**	    (tables, views, dbevents, dbprocs); qeu_dview() will call dmt_show()
**	    to figure out whether the object is a view or a base table and call
**	    qeu_d_cascade() to do all the work.
**	16-mar-93 (andre)
**	    if the object being dropped is an index, set obj_type to DB_INDEX 
**	    rather than DB_TABLE
**	08-oct-93 (andre)
**	    qeu_d_cascade() expects one more parameter - an address of a 
**	    DMT_TBL_ENTRY describing table/view/index being dropped; for all 
**	    other types of objects, NULL must be passed
*/
DB_STATUS
qeu_dview(
QEF_CB          *qef_cb,
QEUQ_CB		*qeuq_cb)
{
    DMT_SHW_CB          dmt_show;
    DMT_TBL_ENTRY       dmt_tbl_entry;
    DB_QMODE            obj_type;
    DB_STATUS		status;

    dmt_show.type = DMT_SH_CB;
    dmt_show.length = sizeof(DMT_SHW_CB);
    dmt_show.dmt_char_array.data_in_size = 0;
    dmt_show.dmt_char_array.data_out_size  = 0;
    STRUCT_ASSIGN_MACRO((*qeuq_cb->qeuq_rtbl), dmt_show.dmt_tab_id);
    dmt_show.dmt_flags_mask = DMT_M_TABLE;
    dmt_show.dmt_db_id	= qeuq_cb->qeuq_db_id;
    dmt_show.dmt_session_id = qeuq_cb->qeuq_d_id;
    dmt_show.dmt_table.data_address = (PTR) &dmt_tbl_entry;
    dmt_show.dmt_table.data_in_size = sizeof(DMT_TBL_ENTRY);
    dmt_show.dmt_char_array.data_address = (PTR)NULL;
    dmt_show.dmt_char_array.data_in_size = 0;
    dmt_show.dmt_char_array.data_out_size  = 0;

    status = dmf_call(DMT_SHOW, &dmt_show);

    if (status != E_DB_OK && dmt_show.error.err_code != E_DM0114_FILE_NOT_FOUND)
    {
	i4	    error;

	(VOID) qef_error(dmt_show.error.err_code, 0L, status, &error,
	    &qeuq_cb->error, 0);
	return(status);
    }

    if (dmt_tbl_entry.tbl_status_mask & DMT_VIEW)
	obj_type = DB_VIEW;
    else if (dmt_tbl_entry.tbl_status_mask & DMT_IDX)
	obj_type = DB_INDEX;
    else
	obj_type = DB_TABLE;

    qeuq_cb->qeuq_flag_mask &= ~((i4) QEU_DROP_TEMP_TABLE);
    return(qeu_d_cascade(qef_cb, qeuq_cb, obj_type, &dmt_tbl_entry));
}

/*{
** Name: QEU_CPROT	- store protection information.
**
** External QEF call:   status = qef_call(QEU_CPROT, &qeuq_cb);
**
** Description:
**      Add protection, tree, and query text information to the appropriate
**	catalogs (iiprotect, iitree, iiqrytext).  A qrytext id is created 
**	and used to update the qrytext tuples before placing them in their 
**	catalog.  A tree id is created and used to update the tree catalog 
**	tuples before placing them in their catalog.  A protection number is 
**	created and used in conjunction with the tree id and qrytext id to 
**	update the protection tuples.  The protection tuples are then 
**	appended to their catalog.
**
**	The protection number is obtained by scanning for all protections on 
**	this table and finding the highest protection number.  
**
**	A protection may or may not have tree information (for the
**	qualification clause).
**
**	A protection for an event arrives with a dummy unique event id.  We
**	first confirm that the event exists (give a user error if it doesn't).
**	If it does we replace the dummy id with the exisiting tuples id and
**	then we continue processing like a regular protection tuple.
**
** Inputs:
**      qef_cb                  qef session control block
**      qeuq_cb
**	    .qeuq_eflag	        designate error handling semantis
**				for user errors.
**		QEF_INTERNAL	return error code.
**		QEF_EXTERNAL	send message to user.
**	    .qeuq_rtbl		DB_TAB_ID of table/procedure getting on which
**				the protection is being defined.
**				In case of event this is a dummy non-zero id.
**          .qeuq_tsz           Number of independent tables in permit
**	    .qeuq_tbl_id	array of base table id protection refers to
**				in the qualification clause of the create
**				permit statement.
**	    .qeuq_ct		number of tree tuples
**	    .qeuq_tre_tup	tree tuples
**	    .qeuq_cq		number of qrytext tuples
**	    .qeuq_qry_tup	qrytext tuples
**	    .qeuq_cp		number of protection tuples
**	    .qeuq_pro_tup	protection tuples
**          .qeuq_permit_mask   mask to indicate permit type;
**		QEU_PERM_OR_SECALM  must be set
**		QEU_DBP_PROTECTION  creating permit on a dbproc
**		QEU_EV_PROTECTION   creating permit on a dbevent
**		QEU_SEQ_PROTECTION  creating permit on a sequence
**		QEU_SPLIT_PERM	    permit needs to be "split"
**		
**	    .qeuq_dbp_tup	iiprocedure tuple containing the following:
**		.db_dbpname	name of the procedure to be deleted.
**		.db_owner	owner of the above procedure.
**	    .qeuq_culd		number of event/sequence tuples (must be 1).
**	    .qeuq_uld_tup	iievent tuple:
**		.dbe_name	name of the event to be protected.
**		.dbe_owner	owner of the above event.
**	    .qeuq_uld_tup	iisequence tuple:
**		.dbs_name	name of the sequence to be protected.
**		.dbs_owner	owner of the above sequence.
**	    .qeuq_db_id		database id
**	    .qeuq_d_id		dmf session id
**	
** Outputs:
**      qeuq_cb
**          .qeuq_pno           permit number
**	    .error.err_code	one of the following
**				E_QE0002_INTERNAL_ERROR
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0022_ABORTED
**				E_QE0117_NONEXISTENT_DBP
**				E_QE020E_EV_PERMIT_ABSENT
**				E_QE0239_SEQ_PERMIT_ABSENT
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	16-jun-86 (daved)
**          written
**      22-jul-86 (jennifer)
**          Initial coding.
**      20-aug-86 (jennifer)
**	    Changed all transaction calls from QET_xxxx
**          to QEU_xxx.  
**	23-dec-86 (daved) 
**	    close off tables, even in error condition
**      10-dec-87 (puree)
**          Converted all ulm_palloc to qec_palloc
**	20-jan-88 (puree)
**	    Convert all qet_escalate calls to in-line codes.
**	16-aug-88 (puree)
**	    Modify for protection on a database procedure.
**      29-jun-89 (jennifer)
**          Added security auditing.
**	11-oct-89 (ralph)
**	    Fix auditing problem
**	03-nov-89 (neil)
**	    Alerters: Support event permissions creation.
**	09-feb-90 (neil)
**	    Alerters - added auditing for EVENT permissions.
**	5-mar-90 (andre)
**	    IIPROTECT tuples inserted to indicate accessability of the object
**	    to its owner get special treatment.
**	08-aug-90 (ralph)
**	    Mark audit access as SUCCESS (bug 20702?)
**	    Indicate I_QE202D when successful creation of security alarm
**	09-jul-91 (andre)
**	    For table-wide and dbevent permits, we may be told to split permits
**	    descibing multiple privileges into privilege descriptors describing
**	    one privilege each.
**	17-oct-91 (rogerk)
**	    Part of 6.4 -> 6.5 merge changes. Must initialize new dmt_char_array
**	    field for DMT_SHOW calls.
**	07-jul-92 (andre)
**	    caller must provide a structure describing privileges on which new
**	    privileges depend.  If a permit depends on some privilege, then
**	    after creating a new permit we will define a new privilege
**	    descriptor in IIPRIV and will describe dependency of the new permit
**	    on the descriptor.
**
**	    when scanning existing IIPROTECT tuples, check for existing permits
**	    which were specified using the same syntax and represent a superset
**	    of privileges which will be specified by one of permits created as a
**	    part of this statement (remember that permits specifying multiple
**	    privileges get split) (i.e. identical syntax, object, privilege
**	    (disregarding DB_GRANT_OPTION), grantor, grantee, etc. and the
**	    columns are a superset).  Once such tuple is found, there are 3
**	    cases to consider:
**		- existing permit specifies some privilege P WGO and the permit
**		  being created specifies P [WGO] - new permit will not be
**		  created;
**		- existing permit specifies some privilege P [WGO] and the
**		  permit being created specifies P without GRANT OPTION - new
**		  permit will not be created;
**		- existing permit specifies some privilege P without GRANT
**		  OPTION and the permit being created specifies P WGO - existing
**		  permit will be destroyed and new permit will be created
**
**	    If during scanning the list of existing permits on the object we
**	    find a permit which specifies the same privilege (with possible
**	    exception of GRANT OPTION) but cannot be merged with this permit, we
**	    will remember its number and will make the new permit depend on the
**	    same privilege descriptor as the existing permit, thus saving
**	    ourselves the expense of creating the new privilege descriptor
**	18-jul-92 (andre)
**	    qeu_cprot() will no longer accept requests to create ALL/RETRIEVE TO
**	    ALL permits (which used to be represented by
**	    qeuq_cb->qeuq_permit_mask being set to 1 or 2).  When creating a new
**	    permit, qeu_scan_perms() will determine whether the DMT_ALL_PROT
**	    and/or DMT_RETRIEVE_PRO need to be turned off in relstat.
**	21-aug-92 (andre)
**	    qeu_scan_perms() will compute "depth" of permits created by 
**	    users on objects owned by other users.  "Depth" of a permit
**	    is a number associated with a permit such that given two permits 
**	    on some object X we will guarantee that permit with depth N will 
**	    not depend on permit with depth N+k where k>=0, thus making
**	    it possible for unloaddb to recreate permits in the correct
**	    order
**	16-sep-92 (andre)
**	    privilege map is populated using bitwise ops so we cannot use
**	    BTnext() to determine all bits that are set; we will use bitwise AND
**	    instead
**	28-sep-92 (andre)
**	    correct code that was made unportable by the implicit assumption 
**	    that bool can be assigned a result of a random boolean expression
**	09-sep-93 (andre)
**	    if creating a security_alarm on a view, alter timestamp of its 
**	    underlying base table - this will ensure that any QPs dependent on
**	    that view will get invalidated and, where appropriate, the newly
**	    created security_alarm will get attached to query trees
**	27-oct-93 (andre)
**	    instead of using timestamps, we will use a "randomized" combination 
**	    of table/view/dbproc/dbevent id and permit number.  This eliminates
**	    possibility of any two permits ever having duplicate text or tree id
**	4-nov-93 (robf)
**          Audit event security label. 
**	24-nov-93 (andre)
**	    qeu_v_ubt_id() will take address of DB_ERROR and will report its own
**	    errors
**	22-jul-96 (ramra01 for bryanp)
**	    Alter table drop col permissions for cascade/restrict
**	13-may-02 (inkdo01)
**	    Add support for sequence protections.
*/
DB_STATUS
qeu_cprot(
QEF_CB          *qef_cb,
QEUQ_CB		*qeuq_cb)
{
    GLOBALREF QEF_S_CB *Qef_s_cb;
    DB_IIDBDEPENDS  *dtuple;
    DB_PROTECTION   *ptuple, *new_ptuple, *callers_ptuple;
    DB_STATUS	    status, local_status;
    DB_ERROR	    e_error;
    i4	    error;
    bool	    transtarted = FALSE;	    
    bool	    tbl_opened = FALSE;
    bool	    indep_privs;
    bool	    err_already_reported = FALSE;
    register i4     i;
    i4		    permno;
    QEU_CB	    tranqeu, qeu;
    QEU_CB	    protqeu, *prot_qeu = (QEU_CB *) NULL,
		    treeqeu, *tree_qeu = (QEU_CB *) NULL,
		    textqeu, *text_qeu = (QEU_CB *) NULL,
		    dpndqeu, *dpnd_qeu = (QEU_CB *) NULL;
    DMT_CB	    dmt_cb;
    ULM_RCB         ulm; 
    QEF_DATA	    qef_data, prot_qefdata;
    DB_PROCEDURE    dbptuple;
    DB_IIEVENT	    evexists, *evcheck;	/* Event tuple for existence check */
    DB_IISEQUENCE   seqexists, *seqcheck; /* Sequence tuple for existence check */
    DMT_CHAR_ENTRY  char_array[2];
    DMR_ATTR_ENTRY  key_array[3];
    DMR_ATTR_ENTRY  *key_ptr_array[3];
    DMT_SHW_CB	    dmt_shw_cb;
    DMT_TBL_ENTRY   table;
    i4         msgid;
    i4	    relstat = 0L;
    i4              type;    
    i4		    access;
    QEU_NEWPERM_INFO	newperm_info;
    PSQ_INDEP_OBJECTS	*indep_objs = (PSQ_INDEP_OBJECTS *) qeuq_cb->qeuq_indep;
    char	    *sec_object;
    i4	    	    sec_obj_len;
    DB_OWN_NAME	    *sec_owner;
    i4		    exit_loop, exit_loop2;

    for (i=0; i< 3; i++)
	key_ptr_array[i] = &key_array[i];

    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
    ulm.ulm_streamid = (PTR)NULL;

    for (exit_loop = FALSE; !exit_loop; )
    {
	/* Check the control block. */

	error = E_QE0017_BAD_CB;
	status = E_DB_ERROR;
	if (qeuq_cb->qeuq_type != QEUQCB_CB || 
	    qeuq_cb->qeuq_length != sizeof(QEUQ_CB))	
	    break;

	error = E_QE0018_BAD_PARAM_IN_CB;
        if ((qeuq_cb->qeuq_rtbl == 0)
            || (qeuq_cb->qeuq_db_id == 0)
	    || (~qeuq_cb->qeuq_permit_mask & QEU_PERM_OR_SECALM)
	    || (   qeuq_cb->qeuq_permit_mask & QEU_DBP_PROTECTION
		&& qeuq_cb->qeuq_permit_mask & QEU_EV_PROTECTION
	       )
	    || (   qeuq_cb->qeuq_permit_mask & QEU_SPLIT_PERM
		&& qeuq_cb->qeuq_permit_mask & QEU_DBP_PROTECTION
	       )
	    || qeuq_cb->qeuq_permit_mask &
		~(QEU_PERM_OR_SECALM | QEU_DBP_PROTECTION | QEU_EV_PROTECTION |
		    QEU_SEQ_PROTECTION | QEU_SPLIT_PERM)
            || (qeuq_cb->qeuq_d_id == 0)
	    || !indep_objs)

	    break;
		
	/*
	** note that only permits created to indicate accessability of a
	** newly created object to its owner will have no IIQRYTEXT tuple
	*/
	if (((DB_PROTECTION *) qeuq_cb->qeuq_pro_tup->dt_data)->dbp_popset
							!= DB_CAN_ACCESS &&
	    (qeuq_cb->qeuq_cq == 0 || qeuq_cb->qeuq_qry_tup == 0)

	    ||

	    (qeuq_cb->qeuq_cp != 1 || qeuq_cb->qeuq_pro_tup == 0))
	{
	    break;
	}

	/* Validate event/sequence protection */
	if (   (qeuq_cb->qeuq_permit_mask & (QEU_EV_PROTECTION | QEU_SEQ_PROTECTION))
	    && (qeuq_cb->qeuq_culd != 1 || qeuq_cb->qeuq_uld_tup == NULL)
	   )
	   break;

	/* 
	** Check to see if a transaction is in progress,
	** if so, set transtarted flag to FALSE, otherwise
	** set it to TRUE after beginning a transaction.
	** If we started a transaction then we will abort
        ** it if an error occurs, otherwise we will just
        ** return the error and let the caller abort it.
	*/

	if (qef_cb->qef_stat == QEF_NOTRAN)
	{
	    tranqeu.qeu_type = QEUCB_CB;
	    tranqeu.qeu_length = sizeof(QEUCB_CB);
	    tranqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
	    tranqeu.qeu_d_id = qeuq_cb->qeuq_d_id;
	    tranqeu.qeu_flag = 0;
	    tranqeu.qeu_mask = 0;
	    status = qeu_btran(qef_cb, &tranqeu);
	    if (status != E_DB_OK)
	    {	
		error = tranqeu.error.err_code;
		break;	
	    }	    
	    transtarted = TRUE;
	}
	/* escalate the transaction to MST */
	if (qef_cb->qef_auto == QEF_OFF)
	    qef_cb->qef_stat = QEF_MSTRAN;

	if (qeuq_cb->qeuq_permit_mask & QEU_DBP_PROTECTION)
	{
	    /* If defining protection on a DB procedure, open the
	    ** iiprocedure catalog and get the procedure tuple.  This
	    ** verifies that the procedure exist and insures that an
	    ** exclusive lock is placed on the procedure.
	    */
	    qeu.qeu_type = QEUCB_CB;
	    qeu.qeu_length = sizeof(QEUCB_CB);
	    qeu.qeu_db_id = qeuq_cb->qeuq_db_id;
	    qeu.qeu_lk_mode = DMT_IX;
	    qeu.qeu_flag = DMT_U_DIRECT;
	    qeu.qeu_access_mode = DMT_A_WRITE;
	    qeu.qeu_tab_id.db_tab_base = DM_B_DBP_TAB_ID; 	
	    qeu.qeu_tab_id.db_tab_index = DM_I_DBP_TAB_ID; 	
	    qeu.qeu_mask = 0;

	    status = qeu_open(qef_cb, &qeu);
	    if (DB_FAILURE_MACRO(status))
	    {
		error = qeu.error.err_code;
		break;
	    }
	    tbl_opened = TRUE;

	    /* Get the iiprocedure tuple with matching name and owner. */
	    MEcopy((PTR) qeuq_cb->qeuq_dbp_tup->dt_data, 
		    sizeof(dbptuple), (PTR) &dbptuple);	

	    qeu.qeu_count = 1;
	    qeu.qeu_tup_length = sizeof(DB_PROCEDURE);
	    qeu.qeu_output = &qef_data;
	    qef_data.dt_next = 0;
	    qef_data.dt_size = sizeof(DB_PROCEDURE);
	    qef_data.dt_data = (PTR)&dbptuple;
	    qeu.qeu_getnext = QEU_REPO;
	    qeu.qeu_klen = 2;       
	    qeu.qeu_key = key_ptr_array;
	    key_ptr_array[0]->attr_number = DM_1_DBP_KEY;
	    key_ptr_array[0]->attr_operator = DMR_OP_EQ;
	    key_ptr_array[0]->attr_value = (char *) &dbptuple.db_dbpname;
	    key_ptr_array[1]->attr_number = DM_2_DBP_KEY;
	    key_ptr_array[1]->attr_operator = DMR_OP_EQ;
	    key_ptr_array[1]->attr_value = (char *) &dbptuple.db_owner;
	
	    qeu.qeu_qual = 0;
	    qeu.qeu_qarg = 0;
     
	    status = qeu_get(qef_cb, &qeu);
	    if (status != E_DB_OK)
	    {
		if (qeu.error.err_code == E_QE0015_NO_MORE_ROWS)
		    error = E_QE0117_NONEXISTENT_DBP;
		else
		    error = qeu.error.err_code;
		break;
	    }

	    /* Close the iiprocedure */
	    status = qeu_close(qef_cb, &qeu);    
	    if (DB_FAILURE_MACRO(status))
	    {
		error = qeu.error.err_code;
		break;
	    }
	    tbl_opened = FALSE;
	}
	else if (qeuq_cb->qeuq_permit_mask & QEU_EV_PROTECTION)
	{
	    /*
	    ** If defining a protection on  an event, open iievent and
	    ** get the event tuple.  This verifies that the event exist.
	    ** Lock/access modes IX, WRITE are used instead of IS, READ to
	    ** avoid creating orphaned permissions if readlock=nolock.
	    */
	    qeu.qeu_type = QEUCB_CB;
	    qeu.qeu_length = sizeof(QEUCB_CB);
	    qeu.qeu_db_id = qeuq_cb->qeuq_db_id;
	    qeu.qeu_flag = DMT_U_DIRECT;
	    qeu.qeu_lk_mode = DMT_IX;
	    qeu.qeu_access_mode = DMT_A_WRITE;
	    qeu.qeu_tab_id.db_tab_base = DM_B_EVENT_TAB_ID; 	
	    qeu.qeu_tab_id.db_tab_index = DM_I_EVENT_TAB_ID; 	
	    qeu.qeu_mask = 0;

	    status = qeu_open(qef_cb, &qeu);
	    if (DB_FAILURE_MACRO(status))
	    {
		error = qeu.error.err_code;
		break;
	    }
	    tbl_opened = TRUE;

	    /* Fetch the event tuple with matching name and owner */
	    evcheck  = (DB_IIEVENT *)qeuq_cb->qeuq_uld_tup->dt_data;
	    qeu.qeu_count = 1;
	    qeu.qeu_tup_length = sizeof(DB_IIEVENT);
	    qeu.qeu_output = &qef_data;
	    qef_data.dt_next = 0;
	    qef_data.dt_size = sizeof(DB_IIEVENT);
	    qef_data.dt_data = (PTR)&evexists;
	    qeu.qeu_getnext = QEU_REPO;
	    qeu.qeu_klen = 2;
	    qeu.qeu_key = key_ptr_array;
	    key_ptr_array[0]->attr_number = DM_1_EVENT_KEY;
	    key_ptr_array[0]->attr_operator = DMR_OP_EQ;
	    key_ptr_array[0]->attr_value = (char *)&evcheck->dbe_name;
	    key_ptr_array[1]->attr_number = DM_2_EVENT_KEY;
	    key_ptr_array[1]->attr_operator = DMR_OP_EQ;
	    key_ptr_array[1]->attr_value = (char *)&evcheck->dbe_owner;
	    qeu.qeu_qual = 0;
	    qeu.qeu_qarg = 0;
	    status = qeu_get(qef_cb, &qeu);
	    if (status != E_DB_OK)
	    {
		if (qeu.error.err_code == E_QE0015_NO_MORE_ROWS)
		{
		    (VOID)qef_error(E_QE020E_EV_PERMIT_ABSENT, 0L,
				    status, &error, &qeuq_cb->error, 2,
				    qec_trimwhite(sizeof(evcheck->dbe_name),
						  (char *)&evcheck->dbe_name),
				    (PTR)&evcheck->dbe_name,
				    qec_trimwhite(sizeof(evcheck->dbe_owner),
						  (char *)&evcheck->dbe_owner),
				    (PTR)&evcheck->dbe_owner);
		    error = E_QE0025_USER_ERROR;
		}
		else
		{
		    error = qeu.error.err_code;
		}
		break;
	    } /* If not OK */

	    status = qeu_close(qef_cb, &qeu);    	/* Close iievent */
	    if (DB_FAILURE_MACRO(status))
	    {
		error = qeu.error.err_code;
		break;
	    }
	    tbl_opened = FALSE;
	}
	else if (qeuq_cb->qeuq_permit_mask & QEU_SEQ_PROTECTION)
	{
	    /*
	    ** If defining a protection on a sequence, open iisequence and
	    ** get the sequence tuple.  This verifies that the sequence exists.
	    ** Lock/access modes IX, WRITE are used instead of IS, READ to
	    ** avoid creating orphaned permissions if readlock=nolock.
	    */
	    qeu.qeu_type = QEUCB_CB;
	    qeu.qeu_length = sizeof(QEUCB_CB);
	    qeu.qeu_db_id = qeuq_cb->qeuq_db_id;
	    qeu.qeu_flag = DMT_U_DIRECT;
	    qeu.qeu_lk_mode = DMT_IX;
	    qeu.qeu_access_mode = DMT_A_WRITE;
	    qeu.qeu_tab_id.db_tab_base = DM_B_SEQ_TAB_ID; 	
	    qeu.qeu_tab_id.db_tab_index = DM_I_SEQ_TAB_ID; 	
	    qeu.qeu_mask = 0;

	    status = qeu_open(qef_cb, &qeu);
	    if (DB_FAILURE_MACRO(status))
	    {
		error = qeu.error.err_code;
		break;
	    }
	    tbl_opened = TRUE;

	    /* Fetch the sequence tuple with matching name and owner */
	    seqcheck  = (DB_IISEQUENCE *)qeuq_cb->qeuq_uld_tup->dt_data;
	    qeu.qeu_count = 1;
	    qeu.qeu_tup_length = sizeof(DB_IISEQUENCE);
	    qeu.qeu_output = &qef_data;
	    qef_data.dt_next = 0;
	    qef_data.dt_size = sizeof(DB_IISEQUENCE);
	    qef_data.dt_data = (PTR)&seqexists;
	    qeu.qeu_getnext = QEU_REPO;
	    qeu.qeu_klen = 2;
	    qeu.qeu_key = key_ptr_array;
	    key_ptr_array[0]->attr_number = DM_1_SEQ_KEY;
	    key_ptr_array[0]->attr_operator = DMR_OP_EQ;
	    key_ptr_array[0]->attr_value = (char *)&seqcheck->dbs_name;
	    key_ptr_array[1]->attr_number = DM_2_SEQ_KEY;
	    key_ptr_array[1]->attr_operator = DMR_OP_EQ;
	    key_ptr_array[1]->attr_value = (char *)&seqcheck->dbs_owner;
	    qeu.qeu_qual = 0;
	    qeu.qeu_qarg = 0;
	    status = qeu_get(qef_cb, &qeu);
	    if (status != E_DB_OK)
	    {
		if (qeu.error.err_code == E_QE0015_NO_MORE_ROWS)
		{
		    (VOID)qef_error(E_QE0239_SEQ_PERMIT_ABSENT, 0L,
				    status, &error, &qeuq_cb->error, 2,
				    qec_trimwhite(sizeof(seqcheck->dbs_name),
						  (char *)&seqcheck->dbs_name),
				    (PTR)&evcheck->dbe_name,
				    qec_trimwhite(sizeof(seqcheck->dbs_owner),
						  (char *)&seqcheck->dbs_owner),
				    (PTR)&seqcheck->dbs_owner);
		    error = E_QE0025_USER_ERROR;
		}
		else
		{
		    error = qeu.error.err_code;
		}
		break;
	    } /* If not OK */

	    status = qeu_close(qef_cb, &qeu);    	/* Close iisequence */
	    if (DB_FAILURE_MACRO(status))
	    {
		error = qeu.error.err_code;
		break;
	    }
	    tbl_opened = FALSE;
	}
	else
	{
	    /* 
	    ** If defining protection on a table, alter the table tuple in the
	    ** iirelation to indicate that protections exist. This also 
	    ** verifies that the table exists and insures that an exclusive
	    ** lock is placed on the table.
	    */
	    dmt_cb.dmt_flags_mask = 0;
	    dmt_cb.dmt_db_id = qeuq_cb->qeuq_db_id;
	    dmt_cb.dmt_char_array.data_in_size = sizeof(DMT_CHAR_ENTRY) * 1;
	    dmt_cb.dmt_char_array.data_address = (PTR) char_array;
	    char_array[0].char_id = DMT_C_PERMITS;
	    char_array[0].char_value = DMT_C_ON;

	    dmt_cb.length = sizeof(DMT_CB);
	    dmt_cb.type = DMT_TABLE_CB;
	    dmt_cb.dmt_id.db_tab_base = qeuq_cb->qeuq_rtbl->db_tab_base;
	    dmt_cb.dmt_id.db_tab_index = qeuq_cb->qeuq_rtbl->db_tab_index;
	    dmt_cb.dmt_tran_id = qef_cb->qef_dmt_id;

	    status = dmf_call(DMT_ALTER, &dmt_cb);
	    if (status != E_DB_OK)
	    {
		error = dmt_cb.error.err_code;
		break;
	    }

	    /* Must get table name, owner  for security auditing */

	    dmt_shw_cb.length = sizeof(DMT_SHW_CB);
	    dmt_shw_cb.type = DMT_SH_CB;
	    dmt_shw_cb.dmt_db_id = qeuq_cb->qeuq_db_id;
	    dmt_shw_cb.dmt_session_id = qeuq_cb->qeuq_d_id;
	    dmt_shw_cb.dmt_flags_mask = DMT_M_TABLE;
	    dmt_shw_cb.dmt_table.data_address = (char *)&table;
	    dmt_shw_cb.dmt_table.data_in_size = sizeof(DMT_TBL_ENTRY);
	    dmt_shw_cb.dmt_char_array.data_in_size = 0;
	    dmt_shw_cb.dmt_char_array.data_out_size  = 0;
	    dmt_shw_cb.dmt_tab_id.db_tab_base = 
			    qeuq_cb->qeuq_rtbl->db_tab_base;
	    dmt_shw_cb.dmt_tab_id.db_tab_index = 
			    qeuq_cb->qeuq_rtbl->db_tab_index;

	    /* issue DMT_SHOW call */

	    if ((status = dmf_call(DMT_SHOW, &dmt_shw_cb)) != E_DB_OK)
	    {
		error = dmt_shw_cb.error.err_code;
		break;
	    }
	    sec_object = table.tbl_name.db_tab_name;
	    sec_obj_len = sizeof(table.tbl_name.db_tab_name);
	    sec_owner  = &table.tbl_owner;

	    relstat = table.tbl_status_mask;
	}

	/* Get a memory stream for reading catalog tuples. */
        for (exit_loop2 = FALSE; !exit_loop2; ) 	/* to break out of */
	{
	    /*
	    ** we allocate space for 2 IIPROTECT tuples since one of them will
	    ** be used to scan existing permits while the other will hold a
	    ** copy of the tuple passed by the user;
	    ** we will make a copy of the caller's tuple since we may make some
	    ** changes to the privilege map and it would be nice to isolate the
	    ** caller from the changes
	    */
	    /* Open stream and allocate list in one action */
	    ulm.ulm_flags = ULM_PRIVATE_STREAM | ULM_OPEN_AND_PALLOC;
	    ulm.ulm_psize = sizeof(DB_PROTECTION) * 2;
	    if ((status = qec_mopen(&ulm)) != E_DB_OK)
		break;

	    ptuple = (DB_PROTECTION *) ulm.ulm_pptr;

	    /* allocate IIDBDEPENDS tuple if it will be needed */
	    if (qeuq_cb->qeuq_tsz)
	    {
		ulm.ulm_psize = sizeof(DB_IIDBDEPENDS);
		if ((status = qec_malloc(&ulm)) != E_DB_OK)
		{
		    ulm_closestream(&ulm);
		    break;
		}
		dtuple = (DB_IIDBDEPENDS *) ulm.ulm_pptr;
	    }
	    exit_loop2 = TRUE;
	}
	if (status != E_DB_OK)
	{
	    error = E_QE001E_NO_MEM;
	    break;
	}

	/* Now get the protection number for this table. */
	protqeu.qeu_type		= QEUCB_CB;
        protqeu.qeu_length		= sizeof(QEUCB_CB);
        protqeu.qeu_tab_id.db_tab_base  = DM_B_PROTECT_TAB_ID;
        protqeu.qeu_tab_id.db_tab_index = DM_I_PROTECT_TAB_ID;
        protqeu.qeu_db_id		= qeuq_cb->qeuq_db_id;
        protqeu.qeu_lk_mode		= DMT_IX;
        protqeu.qeu_flag		= DMT_U_DIRECT;
        protqeu.qeu_access_mode		= DMT_A_WRITE;
	protqeu.qeu_mask		= 0;

	status = qeu_open(qef_cb, &protqeu);
	if (status != E_DB_OK)
	{
	    error = protqeu.error.err_code;
	    break;
	}
    		
	prot_qeu = &protqeu;
	prot_qeu->qeu_count = 1;
        prot_qeu->qeu_tup_length = sizeof(DB_PROTECTION);
	prot_qeu->qeu_input =
	    prot_qeu->qeu_output = &prot_qefdata;
	prot_qefdata.dt_next = 0;
        prot_qefdata.dt_size = sizeof(DB_PROTECTION);
        prot_qefdata.dt_data = (PTR) ptuple;
	
	prot_qeu->qeu_klen = 2;       
	prot_qeu->qeu_key = key_ptr_array;
	key_ptr_array[0]->attr_number = DM_1_PROTECT_KEY;
	key_ptr_array[0]->attr_operator = DMR_OP_EQ;
	key_ptr_array[0]->attr_value =
	    (char *) &qeuq_cb->qeuq_rtbl->db_tab_base;
	key_ptr_array[1]->attr_number = DM_2_PROTECT_KEY;
	key_ptr_array[1]->attr_operator = DMR_OP_EQ;
	key_ptr_array[1]->attr_value =
	    (char *) &qeuq_cb->qeuq_rtbl->db_tab_index;
        
	prot_qeu->qeu_qual = 0;
	prot_qeu->qeu_qarg = 0;
     
	indep_privs = (indep_objs->psq_objprivs || indep_objs->psq_colprivs);
	    
	/* make a copy of the IIPROTECT tuple supplied by the caller */
	callers_ptuple = (DB_PROTECTION *) qeuq_cb->qeuq_pro_tup->dt_data;
	new_ptuple = ptuple + 1;
	STRUCT_ASSIGN_MACRO((*callers_ptuple), (*new_ptuple));

	/*
	** scan existing protection tuples for this object and
	**   - compute "depth" of a permit if grantor of a permit does not 
	**     own the object on which the permit is being granted, 
	**   - determine the next protection number,
	**   - determine whether there already exists a permit representing some
	**     of the privileges specified by the caller such that we can avoid
	**     creating one or more new permits,
	**   - determine whether there is a permit specifying the same privilege
	**     which would save us the expence of generating a new privilege
	**     descriptor
	*/
 
	status = qeu_scan_perms(qeuq_cb, qef_cb, prot_qeu, ptuple, new_ptuple,
	    indep_privs, &newperm_info, &ulm, &permno, relstat, &error);
	if (status != E_DB_OK)
	{
	    break;
	}

	/*
	** qeu_scan_perms() may have determined that the privileges specified by
	** the caller have already been granted and new permits will not be
	** created.  If that was not the case, we will try to create permits.
	*/
	if (new_ptuple->dbp_popset & ~((i4) DB_GRANT_OPTION))
	{
	    i2		hi_priv_descr_no;
	    i2		priv_descr_no;
	    i2		saved_descr_no;
	    bool	need_priv_descr_no;
	    bool	with_grant_option;
	    u_i4	obj_opmap;

	    /*
	    ** if there are tuples to be inserted into IITREE, open the catalog
	    ** now
	    */
	    if (qeuq_cb->qeuq_ct)
	    {
		STRUCT_ASSIGN_MACRO(protqeu, treeqeu);
		treeqeu.qeu_tab_id.db_tab_base = DM_B_TREE_TAB_ID;
		treeqeu.qeu_tab_id.db_tab_index = DM_I_TREE_TAB_ID;
		status = qeu_open(qef_cb, &treeqeu);
		if (status != E_DB_OK)
		{
		    error = treeqeu.error.err_code;
		    break;
		}

		tree_qeu = &treeqeu;	/* IITREE has been opened */

		tree_qeu->qeu_count = qeuq_cb->qeuq_ct;
		tree_qeu->qeu_tup_length = sizeof(DB_IITREE);
		tree_qeu->qeu_input = qeuq_cb->qeuq_tre_tup;
	    }

	    new_ptuple->dbp_treeid.db_tre_high_time = 
		new_ptuple->dbp_treeid.db_tre_low_time = 0;

	    /*
	    ** we will insert text into IIQRYTEXT unless we are creating an
	    ** access permit; so open IIQRYTEXT now
	    */
	    if (new_ptuple->dbp_popset != DB_CAN_ACCESS)
	    {
		STRUCT_ASSIGN_MACRO(protqeu, textqeu);
		textqeu.qeu_tab_id.db_tab_base = DM_B_QRYTEXT_TAB_ID;
		textqeu.qeu_tab_id.db_tab_index = DM_I_QRYTEXT_TAB_ID;
		status = qeu_open(qef_cb, &textqeu);
		if (status != E_DB_OK)
		{
		    error = textqeu.error.err_code;
		    break;
		}

		text_qeu = &textqeu;   /* IIQRYTEXT has been opened */

		text_qeu->qeu_count = qeuq_cb->qeuq_cq;
		text_qeu->qeu_tup_length = sizeof(DB_IIQRYTEXT);
		text_qeu->qeu_input = qeuq_cb->qeuq_qry_tup;
	    }

	    /*
	    ** if the permit involves a WHERE-clause referencing a
	    ** different table we will need to insert tuples into IIDBDEPENDS; 
	    ** if so, open it now
	    */
	    if (qeuq_cb->qeuq_tsz)
	    {
		/* open the iidbdepends table */
		STRUCT_ASSIGN_MACRO(protqeu, dpndqeu);
		dpndqeu.qeu_tab_id.db_tab_base = DM_B_DEPENDS_TAB_ID;
		dpndqeu.qeu_tab_id.db_tab_index = DM_I_DEPENDS_TAB_ID;
		status = qeu_open(qef_cb, &dpndqeu);
		if (status != E_DB_OK)
		{
		    error = dpndqeu.error.err_code;
		    break;
		}

		dpnd_qeu = &dpndqeu;	/* IIDBDEPENDS has been opened */
		
		dpnd_qeu->qeu_count = 1;
		dpnd_qeu->qeu_tup_length = sizeof(DB_IIDBDEPENDS);
		dpnd_qeu->qeu_input = &qef_data;
		qef_data.dt_next = 0;
		qef_data.dt_size = sizeof(DB_IIDBDEPENDS);
		qef_data.dt_data = (PTR) dtuple;
	    }

	    /* 
	    ** if independent privileges have been specified and
	    ** for one or more of privileges being granted we have
	    ** not found an existing privilege descriptor on which it
	    ** can depend, scan IIPRIV looking for the highest
	    ** privilege descriptor number created for this object
	    ** and store a number one higher in hi_priv_descr_no
	    */
	    if (   indep_privs 
	        && (new_ptuple->dbp_popset & DB_DELETE &&
	    	    !newperm_info.qeu_descr_nums[QEU_DELETE_DESCR]
		    ||    
		    new_ptuple->dbp_popset & DB_APPEND &&
		    !newperm_info.qeu_descr_nums[QEU_INSERT_DESCR]
		    ||
		    new_ptuple->dbp_popset & DB_REPLACE &&
		    !newperm_info.qeu_descr_nums[QEU_UPDATE_DESCR]
    	           )
		)
	    {
		status = qeu_nxt_prv_descr_no(qef_cb, qeuq_cb,
		    &hi_priv_descr_no, &error);
		if (DB_FAILURE_MACRO(status))
		    break;
	    }

	    /*
	    ** unless we are about to create a security alarm or a special
	    ** "access permit", make a copy of a privilege map
	    ** (turn off DB_GRANT_OPTION since we are interested in "real"
	    ** privileges; also turn off DB_TEST and DB_AGGREGATE bits since for
	    ** our purposes they are adequately represented by DB_RETRIEVE) +
	    ** store an indication of whether permit(s) are being granted WGO
	    */
	    if (!(new_ptuple->dbp_popset & (DB_ALARM | DB_CAN_ACCESS)))
	    {
		obj_opmap =
		    new_ptuple->dbp_popset & ~((i4) (DB_TEST | DB_AGGREGATE));

		/*
		** remember that permit is being granted WITH GRANT OPTION, then
		** turn off the WGO bit in obj_opmap
		*/
		if (with_grant_option = ((obj_opmap & DB_GRANT_OPTION) != 0))
		{
		    obj_opmap &= ~((i4) DB_GRANT_OPTION);
		}
	    }

	    /*
	    ** prot_qefdata.dt_data was pointing at the ptuple which was used
	    ** for scanning existing IIPROTECT tuples; now we reset it to point
	    ** at the copy (possibly modified) of the permit supplied by the
	    ** caller
	    */
	    prot_qefdata.dt_data = (PTR) new_ptuple;

	    /*
	    ** we need to set qeu_count here because it could have been reset
	    ** to 0 if qeu_scan_perms() read all the existing permits
	    */
	    prot_qeu->qeu_count = qeuq_cb->qeuq_cp;

	    /*
	    ** if creating a permit on a table (other than the infamous "access
	    ** permit") or on a dbevent, we may need to create more than one
	    ** permit tuple to ensure that each specifies exactly one privilege
	    */

	    if (qeuq_cb->qeuq_permit_mask & QEU_SPLIT_PERM)
	    {
		bool		sql_perm, grant_perm;
		char		*priv_name;
		u_i2		priv_len;
		QEF_DATA	*priv_tuple;
		register char   *priv_start;
		DB_IIQRYTEXT    *qtuple;
		i4		priv_map = 0;

		/*
		** remember the location of the privilege name inside the
		** IIQRYTEXT tuple; the location depends on whether the permit
		** is being created using CREATE PERMIT, DEFINE PERMIT, or GRANT
		*/

		/* find the beginning of the privilege name placeholder */
		for (priv_tuple = qeuq_cb->qeuq_qry_tup, i = 0;
		     i < qeuq_cb->qeuq_cq;
		     priv_tuple = priv_tuple->dt_next, i++
		    )
		{
		    register char   *qtext_end;

		    if (!priv_tuple)
		    {
			status = E_DB_ERROR;
			error = E_QE0018_BAD_PARAM_IN_CB;
			break;
		    }

		    qtuple = (DB_IIQRYTEXT *) priv_tuple->dt_data;

		    /* look for '?' which starts a privilege name placeholder */
		    for (priv_start = (char *) qtuple->dbq_text.db_t_text,
			 qtext_end = (char *) qtuple->dbq_text.db_t_text +
			    qtuple->dbq_text.db_t_count;
			 (priv_start < qtext_end && CMcmpcase(priv_start, "?"));
			 priv_start = CMnext(priv_start)
			)
		    ;

		    if (priv_start < qtext_end)
		    {
			/*
			** found the placeholder;
			** priv_start points at the beginning of a privilege
			** name placeholder; if the placeholder spans two
			** IIQRYTEXT tuples, we can use priv_tuple to correctly
			** insert a privilege name
			*/
			break;
		    }
		}

		if (i == qeuq_cb->qeuq_cq)
		{
		    /* failed to find the placeholder; something is fishy */
		    status = E_DB_ERROR;
		    error = E_QE0018_BAD_PARAM_IN_CB;
		}

		if (status != E_DB_OK)
		    break;

		/*
		** figure out if this is an SQL permit to ensure that correct
		** privilege name gets inserted into query text; in GRANT
		** statement, placeholder is long enough to accomodate
		** REFERENCES (i.e. 10), whereas for CREATE PERMIT template is
		** still 6 chars long
		*/
		if (sql_perm = ((qtuple->dbq_status & DBQ_SQL) != 0))
		    grant_perm = (new_ptuple->dbp_flags & DBP_GRANT_PERM) != 0;
		else
		    grant_perm = FALSE;

		/*
		** privilege map is constructed using bitwise ops, but here we
		** want to use BTnext() to scan through the privileges, so first
		** we will translate the privilege map
		*/
		if (obj_opmap & DB_RETRIEVE)
		    BTset(DB_RETP, (char *) &priv_map);
		if (obj_opmap & DB_REPLACE)
		    BTset(DB_REPP, (char *) &priv_map);
		if (obj_opmap & DB_DELETE)
		    BTset(DB_DELP, (char *) &priv_map);
		if (obj_opmap & DB_APPEND)
		    BTset(DB_APPP, (char *) &priv_map);
		if (obj_opmap & DB_COPY_INTO)
		    BTset(DB_COPY_INTOP, (char *) &priv_map);
		if (obj_opmap & DB_COPY_FROM)
		    BTset(DB_COPY_FROMP, (char *) &priv_map);
		if (obj_opmap & DB_REFERENCES)
		    BTset(DB_REFP, (char *) &priv_map);
		if (obj_opmap & DB_EVREGISTER)
		    BTset(DB_EVRGP, (char *) &priv_map);
		if (obj_opmap & DB_EVRAISE)
		    BTset(DB_EVRAP, (char *) &priv_map);
		if (obj_opmap & DB_NEXT)
		    BTset(DB_NEXTP, (char *) &priv_map);
		
		for (i = BTnext(-1, (char *) &priv_map, BITS_IN(priv_map));
		     i != -1;
		     i = BTnext(i, (char *) &priv_map, BITS_IN(priv_map))
		    )
		{
		    /*
		    ** assume that we won't need a privilege descriptor number
		    */
		    need_priv_descr_no = FALSE;

		    switch (i)
		    {
			case DB_RETP:
			{
			    new_ptuple->dbp_popset = new_ptuple->dbp_popctl =
				DB_RETRIEVE | DB_TEST | DB_AGGREGATE;

			    if (grant_perm)
			    {
				priv_name = "  select  ";
				priv_len = (u_i2) sizeof("  select  ") - 1;
			    }
			    else if (sql_perm)
			    {
				priv_name = "select";
				priv_len = (u_i2) sizeof("select") - 1;
			    }
			    else
			    {
				priv_name = "retrieve";
				priv_len = (u_i2) sizeof("retrieve") - 1;
			    }

			    break;
			}
			case DB_REPP:
			{
			    new_ptuple->dbp_popset = new_ptuple->dbp_popctl =
				DB_REPLACE;

			    if (grant_perm)
			    {
				priv_name = "  update  ";
				priv_len = (u_i2) sizeof("  update  ") - 1;
			    }
			    else if (sql_perm)
			    {
				priv_name = "update";
				priv_len = (u_i2) sizeof("update") - 1;
			    }
			    else
			    {
				priv_name = "replace ";
				priv_len = (u_i2) sizeof("replace ") - 1;
			    }

			    if (indep_privs)
			    {
				need_priv_descr_no = TRUE;
				saved_descr_no = newperm_info.
				    qeu_descr_nums[QEU_UPDATE_DESCR];
			    }

			    break;
			}
			case DB_DELP:
			{
			    new_ptuple->dbp_popset = new_ptuple->dbp_popctl =
				DB_DELETE;

			    if (grant_perm)
			    {
				priv_name = "  delete  ";
				priv_len = (u_i2) sizeof("  delete  ") - 1;
			    }
			    else if (sql_perm)
			    {
				priv_name = "delete";
				priv_len = (u_i2) sizeof("delete") - 1;
			    }
			    else
			    {
				priv_name = "delete  ";
				priv_len = (u_i2) sizeof("delete  ") - 1;
			    }

			    if (indep_privs)
			    {
				need_priv_descr_no = TRUE;
				saved_descr_no = newperm_info.
				    qeu_descr_nums[QEU_DELETE_DESCR];
			    }

			    break;
			}
			case DB_APPP:
			{
			    new_ptuple->dbp_popset = new_ptuple->dbp_popctl =
				DB_APPEND;

			    if (grant_perm)
			    {
				priv_name = "  insert  ";
				priv_len = (u_i2) sizeof("  insert  ") - 1;
			    }
			    else if (sql_perm)
			    {
				priv_name = "insert";
				priv_len = (u_i2) sizeof("insert") - 1;
			    }
			    else
			    {
				priv_name = "append  ";
				priv_len = (u_i2) sizeof("append  ") - 1;
			    }

			    if (indep_privs)
			    {
				need_priv_descr_no = TRUE;
				saved_descr_no = newperm_info.
				    qeu_descr_nums[QEU_INSERT_DESCR];
			    }

			    break;
			}
			case DB_COPY_INTOP:
			{
			    new_ptuple->dbp_popset = new_ptuple->dbp_popctl =
				DB_COPY_INTO;

			    if (grant_perm)
			    {
				priv_name = " copy_into";
				priv_len = (u_i2) sizeof(" copy_into") - 1;
			    }
			    else if (sql_perm)
			    {
				priv_name = "copy_into";
				priv_len = (u_i2) sizeof("copy_into") - 1;
			    }
			    else
			    {
				priv_name = "copy_into";
				priv_len = (u_i2) sizeof("copy_into") - 1;
			    }

			    if (indep_privs)
			    {
				need_priv_descr_no = TRUE;
				saved_descr_no = newperm_info.
				    qeu_descr_nums[QEU_INSERT_DESCR];
			    }

			    break;
			}
			case DB_COPY_FROMP:
			{
			    new_ptuple->dbp_popset = new_ptuple->dbp_popctl =
				DB_COPY_FROM;

			    if (grant_perm)
			    {
				priv_name = " copy_from";
				priv_len = (u_i2) sizeof(" copy_from") - 1;
			    }
			    else if (sql_perm)
			    {
				priv_name = "copy_from";
				priv_len = (u_i2) sizeof("copy_from") - 1;
			    }
			    else
			    {
				priv_name = "copy_from";
				priv_len = (u_i2) sizeof("copy_from") - 1;
			    }

			    if (indep_privs)
			    {
				need_priv_descr_no = TRUE;
				saved_descr_no = newperm_info.
				    qeu_descr_nums[QEU_INSERT_DESCR];
			    }

			    break;
			}
			case DB_REFP:
			{
			    new_ptuple->dbp_popset = new_ptuple->dbp_popctl =
				DB_REFERENCES;

			    /* 
			    ** REFERENCES can only be specified using GRANT and
			    ** there will be no independent privileges since it
			    ** can only be granted on a base table
			    */
			    priv_name = "references";
			    priv_len = (u_i2) sizeof("references") - 1;

			    break;
			}
			case DB_EVRGP:
			{
			    new_ptuple->dbp_popset = new_ptuple->dbp_popctl =
				DB_EVREGISTER;
			    priv_name = "register";
			    priv_len = (u_i2) sizeof("register") - 1;
			    break;
			}
			case DB_EVRAP:
			{
			    new_ptuple->dbp_popset = new_ptuple->dbp_popctl =
				DB_EVRAISE;
			    priv_name = "raise   ";
			    priv_len = (u_i2) sizeof("raise   ") - 1;
			    break;
			}
			case DB_NEXTP:
			{
			    new_ptuple->dbp_popset = new_ptuple->dbp_popctl =
				DB_NEXT;
			    priv_name = "next    ";
			    priv_len = (u_i2) sizeof("next    ") - 1;
			    break;
			}
		    }

		    /* now insert privilege name into the IIQRYTEXT tuple(s) */
		    {
			char		*c, *last;
			QEF_DATA	*t = priv_tuple;

			qtuple = (DB_IIQRYTEXT *) t->dt_data;
			last = (char *)(qtuple->dbq_text.db_t_text +
					qtuple->dbq_text.db_t_count);

			for (c = priv_start; *priv_name != EOS; )
			{
			    if (c > last)	/* priv name spans two tuples */
			    {
				t = t->dt_next;
				qtuple = (DB_IIQRYTEXT *) t->dt_data;
				c = (char *)qtuple->dbq_text.db_t_text;
				last = c + qtuple->dbq_text.db_t_count;
			    }
			    
			    CMcpyinc(priv_name, c);
			}
		    }

		    if (with_grant_option)
		    {
			new_ptuple->dbp_popset |= DB_GRANT_OPTION;
			new_ptuple->dbp_popctl |= DB_GRANT_OPTION;
		    }

		    /*
		    ** if we need to supply qeu_new_prot_tuple() with
		    ** independent privilege descriptor number, it will be
		    ** determined either by using number of the permit granting
		    ** the same privilege on the same set of attributes (as was
		    ** determined by qeu_scan_perms()) or by using the next
		    ** available privilege descriptor number
		    */
		    if (need_priv_descr_no)
		    {
			if (saved_descr_no != 0)
			{
			    /*
			    ** new permit will depend on an existing privilege 
			    ** descriptor whose number was obtained by 
			    ** qeu_scan_perms() 
			    */
			    priv_descr_no = saved_descr_no;
			}
			else
			{
			    priv_descr_no = hi_priv_descr_no++;
			}
		    }
		    else
		    {
			priv_descr_no = 0;
		    }

		    status = qeu_new_prot_tuple(qef_cb, qeuq_cb, prot_qeu,
			tree_qeu, text_qeu, dpnd_qeu, ++permno, priv_descr_no, 
			(bool) (need_priv_descr_no && !saved_descr_no), &error);
		    if (status != E_DB_OK)
			break;
		}
	    }	/* if may need to split permit tuples */
	    else
	    {
		/*
		** if we are creating a real permit (as opposed to a
		** security_alarm or a "access permit") and the new
		** permit will depend on some privilege(s), determine
		** whether we can reuse an existing privilege descriptor
		** or will have to generate a brand new privilege
		** descriptor
		*/
		if (indep_privs)
		{
		    if (obj_opmap & DB_REPLACE)
		    {
			need_priv_descr_no = TRUE;
			saved_descr_no = newperm_info.
			    qeu_descr_nums[QEU_UPDATE_DESCR];
		    }
		    else if (obj_opmap & DB_DELETE)
		    {
			need_priv_descr_no = TRUE;
			saved_descr_no = newperm_info.
			    qeu_descr_nums[QEU_DELETE_DESCR];
		    }
		    else if (obj_opmap & DB_APPEND)
		    {
			need_priv_descr_no = TRUE;
			saved_descr_no = newperm_info.
			    qeu_descr_nums[QEU_INSERT_DESCR];
		    }
		    else
		    {
			need_priv_descr_no = FALSE;
		    }
		}
		else
		{
		    need_priv_descr_no = FALSE;
		}

		/*
		** if we need to supply qeu_new_prot_tuple() with
		** independent privilege descriptor number, it will be
		** determined either by using number of the permit granting
		** the same privilege on the same set of attributes (as was
		** determined by qeu_scan_perms()) or by using the next
		** available privilege descriptor number
		*/
		if (need_priv_descr_no)
		{
		    if (saved_descr_no != 0)
		    {
			/*
			** new permit will depend on an existing privilege 
			** descriptor whose number was obtained by 
			** qeu_scan_perms() 
			*/
			priv_descr_no = saved_descr_no;
		    }
		    else
		    {
			priv_descr_no = hi_priv_descr_no++;
		    }
		}
		else
		{
		    priv_descr_no = 0;
		}

		status = qeu_new_prot_tuple(qef_cb, qeuq_cb, prot_qeu, tree_qeu,
		    text_qeu, dpnd_qeu, ++permno, priv_descr_no,
		    (bool) (need_priv_descr_no && !saved_descr_no), &error);
	    }

	    if (status != E_DB_OK)
		break;

	    /*
	    ** copy dbp_txtid as determined by qeu_new_perm_tuple() into the
	    ** callers tuple to help avoid duplicate query text ids when
	    ** processing GRANT statement
	    */
	    STRUCT_ASSIGN_MACRO(new_ptuple->dbp_txtid,
				callers_ptuple->dbp_txtid);
	}

	/*
	** qeu_scan_perms() may have determined that newly created permit(s)
	** subsume some of the existing permits which will now be deleted
	*/
	if (newperm_info.qeu_perms_to_remove)
	{
	    QEU_REM_PERMS	*p;
	    QEUQ_CB		dprot_qeuqcb;

	    STRUCT_ASSIGN_MACRO((*qeuq_cb), dprot_qeuqcb);

	    /*
	    ** we must tell qeu_dprot() to
	    **	- remove specified permit (QEU_PERM)
	    **	- on the speciifed object type (same as the one for which permit
	    **	  was being created as was specified in
	    **	  qeuq_cb->qeuq_permit_mask)
	    **	- and skip verifying whether destroying the permit would render
	    **	  some object abandoned (QEU_SKIP_ABANDONED_OBJ_CHECK) since we
	    **	  claim that we are removing permits which specified a subset of
	    **	  privileges of the newly created permit(s)
	    */
	    dprot_qeuqcb.qeuq_permit_mask =
		QEU_PERM | QEU_SKIP_ABANDONED_OBJ_CHECK |
		    qeuq_cb->qeuq_permit_mask &
			(QEU_DBP_PROTECTION | QEU_EV_PROTECTION);

	    dprot_qeuqcb.qeuq_flag_mask = 0;

	    for (p = newperm_info.qeu_perms_to_remove; p; p = p->qeu_next)
	    {
		dprot_qeuqcb.qeuq_pno = p->qeu_permno;

		status = qeu_dprot(qef_cb, &dprot_qeuqcb);

		if (status != E_DB_OK)
		{
		    STRUCT_ASSIGN_MACRO(dprot_qeuqcb.error, qeuq_cb->error);
		    error = qeuq_cb->error.err_code;
		    break;
		}
	    }

	    if (status != E_DB_OK)
		break;
	}

	/*
	** qeu_scan_perms() may have determined that DMT_ALL_PROT or
	** DMT_RETRIEVE_PRO need to be reset in the relstat; if so, do it now
	*/
	if (newperm_info.qeu_flags & (QEU_SET_ALL_TO_ALL | QEU_SET_RETR_TO_ALL))
	{
	    DMT_CHAR_ENTRY	*cur_char = char_array;

	    dmt_cb.dmt_flags_mask = 0;
	    dmt_cb.dmt_db_id = qeuq_cb->qeuq_db_id;
	    dmt_cb.dmt_char_array.data_in_size = 0;
	    dmt_cb.dmt_char_array.data_address = (PTR) char_array;

	    if (newperm_info.qeu_flags & QEU_SET_ALL_TO_ALL)
	    {
		cur_char->char_value = DMT_C_OFF;
		cur_char->char_id = DMT_C_ALLPRO;
		dmt_cb.dmt_char_array.data_in_size += sizeof(DMT_CHAR_ENTRY);
		cur_char++;
	    }

	    if (newperm_info.qeu_flags & QEU_SET_RETR_TO_ALL)
	    {
		cur_char->char_value = DMT_C_OFF;
		cur_char->char_id = DMT_C_RETPRO;
		dmt_cb.dmt_char_array.data_in_size += sizeof(DMT_CHAR_ENTRY);
	    }

	    dmt_cb.length = sizeof(DMT_CB);
	    dmt_cb.type = DMT_TABLE_CB;
	    dmt_cb.dmt_id.db_tab_base = qeuq_cb->qeuq_rtbl->db_tab_base;
	    dmt_cb.dmt_id.db_tab_index = qeuq_cb->qeuq_rtbl->db_tab_index;
	    dmt_cb.dmt_tran_id = qef_cb->qef_dmt_id;

	    status = dmf_call(DMT_ALTER, &dmt_cb);
	    if (status != E_DB_OK)
	    {
		error = dmt_cb.error.err_code;
		break;
	    }
	}
	
	/* 
	** one last hurdle - if creating a security alarm on a view, we need to
	** alter the timestamp of the view's underlying base table to force 
	** invalidation of QPs of queries referencing that view
	*/
	if (relstat & DMT_VIEW && new_ptuple->dbp_popset & DB_ALARM)
	{
	    DB_TAB_ID		ubt_id;

	    status = qeu_v_ubt_id(qef_cb, qeuq_cb->qeuq_rtbl, &ubt_id, 
		&qeuq_cb->error);
	    if (DB_FAILURE_MACRO(status))
	    {
		err_already_reported = TRUE;
		break;
	    }

	    if (ubt_id.db_tab_base)
	    {
		status = qeu_mdfy_timestamp(qef_cb, &ubt_id, &error);
		if (DB_FAILURE_MACRO(status))
		    break;
	    }
	}

	/* This is a security event, must audit. */
	if ( Qef_s_cb->qef_state & QEF_S_C2SECURE )
	{
	    if (qeuq_cb->qeuq_permit_mask & QEU_DBP_PROTECTION)
	    {
		type = SXF_E_PROCEDURE;
		access = SXF_A_CONTROL | SXF_A_SUCCESS;
		msgid = I_SX2017_PROT_DBP_CREATE;
		sec_object = (char *)&dbptuple.db_dbpname;
		sec_obj_len = sizeof(dbptuple.db_dbpname);
		sec_owner  = &dbptuple.db_owner;
	    }
	    else if (qeuq_cb->qeuq_permit_mask & QEU_EV_PROTECTION)
	    {
		/* Confusing? The security event is the permission on the event */ 
		type = SXF_E_EVENT;
		access = SXF_A_CONTROL | SXF_A_SUCCESS;
		msgid = I_SX2030_PROT_EV_CREATE;
		sec_object = (char *)&evcheck->dbe_name;
		sec_obj_len = sizeof(evcheck->dbe_name);
		sec_owner  = &evcheck->dbe_owner;
	    }
	    else if (qeuq_cb->qeuq_permit_mask & QEU_SEQ_PROTECTION)
	    {
		/* The security event is the permission on the sequence */ 
		type = SXF_E_EVENT;
		access = SXF_A_CONTROL | SXF_A_SUCCESS;
		msgid = I_SX2049_PROT_SEQ_CREATE;
		sec_object = (char *)&seqcheck->dbs_name;
		sec_obj_len = sizeof(seqcheck->dbs_name);
		sec_owner  = &seqcheck->dbs_owner;
	    }
	    else	/* Must be a table */
	    {
		type = SXF_E_TABLE;
		access = SXF_A_SUCCESS | SXF_A_CONTROL;
		msgid = (new_ptuple->dbp_popset & DB_ALARM)
		    ? I_SX202D_ALARM_CREATE : I_SX2016_PROT_TAB_CREATE;

		sec_object = table.tbl_name.db_tab_name;
		sec_obj_len = sizeof(table.tbl_name.db_tab_name);
		sec_owner  = &table.tbl_owner;
	    }
	    status = qeu_secaudit(FALSE, qef_cb->qef_ses_id, 
		  sec_object, sec_owner, sec_obj_len, type, msgid, access,
		  &e_error);
	}

	exit_loop = TRUE;
    } /* end for */

    /* call qef_error to handle error messages, if any */

    if (status != E_DB_OK && !err_already_reported)
	(VOID) qef_error(error, 0L, status, &error, &qeuq_cb->error, 0);
    
    /* Close off all the tables. */

    if (tbl_opened)
    {
	local_status = qeu_close(qef_cb, &qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(qeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    /* close IIPROTECT if it has been opened */
    if (prot_qeu)
    {
	local_status = qeu_close(qef_cb, prot_qeu);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(prot_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    /* close IITREE if it has been opened */
    if (tree_qeu)
    {
	local_status = qeu_close(qef_cb, tree_qeu);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(tree_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    /* close IIQRYTEXT if it has been opened */
    if (text_qeu)
    {
	local_status = qeu_close(qef_cb, text_qeu);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(text_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    /* close IIDBDEPENDS if it has been opened */
    if (dpnd_qeu)
    {
	local_status = qeu_close(qef_cb, dpnd_qeu);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(dpnd_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (ulm.ulm_streamid != (PTR)NULL)
    {
	/* 
        ** Close off the memory stream created for reading
        ** protection tuples. 
        */

	(VOID) ulm_closestream(&ulm);
    }    	

    if (transtarted)
    {
	if (status == E_DB_OK)
	{
	    status = qeu_etran(qef_cb, &tranqeu);
	    if (status != E_DB_OK)
	    {
		(VOID) qef_error(tranqeu.error.err_code, 0L, status, 
			    &error, &qeuq_cb->error, 0);
	    }
	}
	else
	{
	    local_status = qeu_atran(qef_cb, &tranqeu);
	    if (local_status != E_DB_OK)
	    {
		(VOID) qef_error(tranqeu.error.err_code, 0L, local_status, 
			    &error, &qeuq_cb->error, 0);
		if (local_status > status)
		    status = local_status;
	    }
	}
    }

    return (status);
}

/*{
** Name: QEU_DPROT	- destroy protection information
**
** External QEF call:   status = qef_call(QEU_DPROT, &qeuq_cb);
**
** Description:
**      Given a protection number, this routine will remove protection 
**	information and qrytext information. If protection id zero (0)
**	is given, all protection information for a table/procedure will 
**	be deleted.  If the object is a table, iirelation will be updated
**	to show that there is no more protection information for the table.
**
**	A protection removed for an event may come from a DROP EVENT statement
**	or a remove protection request.  If from a DROP EVENT statement (the
**	event tuple count, qeuq_culd, is zero) then we just behave like drop
**	procedure and drop all protections.  If from a drop protection
**	we first confirm that the event exists (and give a user error if it
**	doesn't).  If it does exist we replace the dummy id that was sent
**	with the exisiting tuple's id and then we continue processing like a
**	regular protection tuple. Exactly the same process is followed when 
**	processing a DROP SEQUENCE or remove protection from a sequence request.
**
** Inputs:
**      qef_cb                  qef session control block
**      qeuq_cb
**	    .qeuq_eflag	        designate error handling semantis
**				for user errors.
**		QEF_INTERNAL	return error code.
**		QEF_EXTERNAL	send message to user.
**	    .qeuq_rtbl		DB_TAB_ID of the table/procedure.
**				In case of event this is a dummy non-zero id
**				unless called from DROP EVENT.
**	    .qeuq_pno		protection number. 
**          .qeuq_permit_mask   permission mask
**		QEU_PERM_OR_SECALM  dropping permits AND secirity_alarms on a
**				    table or a view in conjunction with
**				    destroying the object on which they are
**				    defined (QEU_DROP_ALL must also be set)
**		QEU_PERM	    dropping permit(s) only; could be set if a
**				    user is requesting destruction of permit(s)
**				    on an object or if in the process of
**				    cascading destruction of a table, we came
**				    across a QUEL-style permit dependent on this
**				    table
**		QEU_SECALM	    dropping security_alarm(s) only
**		QEU_DROP_ALL	    dropping all specified objects (permits
**				    and/or security_alarms)
**		QEU_DBP_PROTECTION  dropping permit(s) on a dbproc
**		QEU_EV_PROTECTION   dropping permit(s) on a dbevent
**		QEU_SEQ_PROTECTION  dropping permit(s) on a sequence
**		QEU_SKIP_ABANDONED_OBJ_CHECK
**				    avoid checking whether destroying specified
**				    permit(s) would render any objects abandoned
**	    .qeuq_flag_mask     Operation modifier
**		QEU_DROP_CASCADE    permit is being dropped in conjunction with
**				    cascading destruction of an object (this
**				    will happen only if this is a QUEL-style
**				    permit dependent on the table being dropped)
**
**		QEU_FORCE_QP_INVALIDATION
**				    force QP invalidation by altering timestamp
**				    of appropriate base table; we expect this 
**			            bit to be set only when processing user's 
**				    DROP PERMIT query 
**	    .qeuq_culd		number of event/sequence tuples (must be 1 if from
**				drop permit and 0 if from drop event/sequence).
**	    .qeuq_uld_tup	iievent tuple (always set for events). 
**		.dbe_name	name of the event to be unprotected.
**		.dbe_owner	owner of the above event.
**	    .qeuq_uld_tup	iisequence tuple (always set for sequences). 
**		.dbs_name	name of the sequence to be unprotected.
**		.dbs_owner	owner of the above sequence.
**	    .qeuq_db_id		database id
**	    .qeuq_d_id		dmf session id
**	
** Outputs:
**      qeuq_cb
**	    .error.err_code	one of the following
**				E_QE0002_INTERNAL_ERROR
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0022_ABORTED
**				E_QE020E_EV_PERMIT_ABSENT
**				E_QE0239_SEQ_PERMIT_ABSENT
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	16-jun-86 (daved)
**          written
**      20-aug-86 (jennifer)
**	    Changed all transaction calls from QET_xxxx
**          to QEU_xxx.  
**	23-dec-86 (daved) 
**	    close off tables, even in error condition
**      10-dec-87 (puree)
**          Converted all ulm_palloc to qec_palloc
**	20-jan-88 (puree)
**	    Convert all qet_escalate calls to in-line codes.
**	16-aug-88 (puree)
**	    Modify for protection on a database procedure.
**      29-jun-89 (jennifer)
**          Added security auditing.
**	03-nov-89 (neil)
**	    Alerters: Support event permissions deletion.
**	16-jan-90 (ralph)
**	    Revise qeu_dprot() to prevent deletion of security_alarm tuples
**	    from iiprotect when dropping permits & vice-versa.
**	09-feb-90 (neil)
**	    Alerters - added auditing for EVENT permissions.
**	08-aug-90 (ralph)
**	    Indicate SUCCESS in audit when successful drop protection
**	    Indicate I_QE202E when successful drop security alarm (bug 20806)
**	    Audit implicit drop of SELECT/ALL TO ALL (bug 20942)
**	24-sep-90 (ralph)
**	    Audit implicit drop of SELECT/ALL TO ALL (bug 20942)
**	04-feb-91 (neil)
**	    Alerters - fix DROP EVENT when permissions.
**	17-oct-91 (rogerk)
**	    Part of 6.4 -> 6.5 merge changes. Must initialize new dmt_char_array
**	    field for DMT_SHOW calls.
**	24-jun-92 (andre)
**	    make minor changes to take fuller advantage of qeu_delete()'s
**	    ability to delete multiple tuples by key subject to qualification
**	    function when deleting IITREE and IIQRYTEXT tuples
**	22-jul-92 (andre)
**	    to destroy all permits and/or security alarms, caller will be
**	    expected to set QEU_DROP_ALL in qeuq_permit_mask.  This will remove
**	    the need to reserve permit number 0, although presently I will
**	    forego making changes to have all permits be numbered starting with
**	    0 - to some degree because users are used to seeing ALL_TO_ALL as
**	    permit 0 and to some degree because I got more pressing things to do
**
**	    caller will no longer be able to request destruction of "special"
**	    permits (ALL/RETRIEVE_TO_ALL) which will no longer be created
**	    
**	    If QEU_DROP_CASCADE is set in qeuq_flag_mask, a permit is being
**	    destroyed in conjunction with cascading destruction of a table on
**	    which the permit depended; if its destruction renders some objects
**	    abandoned, they themselves will be destroyed;
**	    otherwise, unless QEU_SKIP_ABANDONED_OBJ_CHECK is set, check whether
**	    destroying specified permit(s) will render any objects abandoned and
**	    if so, prevent user from destroying the specified permit(s)
**	21-aug-92 (andre)
**	    if we are destroying a permit NOT as a part of dropping ALL 
**	    permits defined on an object and the permit specifies grantable 
**	    privilege, we may need to call qeu_depth() to recompute, if 
**	    necessary, depth(s) of some of permits defined on this object
**	11-dec-92 (andre)
**	    add code to handle dropping of permits that specify REFERENCES
**	    privilege
**	10-jun-93 (andre)
**	    added a new argument (object owner name) to interfaces of
**	    qeu_match_privs() and qeu_depth() to correctly handle cases when a
**	    permit which was dropped specified a privilege granted to the
**	    object's owner
**	16-jul-93 (andre)
**	    if we drop a permit which was granted by a user to him/herself, do
**	    not even bother trying to recompute depths of remaining permits or
**	    to determine whether any objects or permits became abandoned as a
**	    result - because no permit depth should need to be recomputed upward
**	    (some could be recomputed downward, but we are all for status quo)
**	    and no objects or privileges will become abandoned
**
**	    instead of trying to recompute depths of permits when some permit 
**	    was dropped, we will be recomputing permit depths when we discover
**	    that even though one of the permits conveying a given privilege PRIV
**	    to a grantee G has been dropped or altered, there are other
**	    permit(s) conveying PRIV to G but the depth of these other permits
**	    (as determined by qeu_match_privs()) is greater than that of the
**	    permit that was destroyed or altered
**
**	    added perm_depth_info to the interface of qeu_match_privs()
**	25-jul-93 (andre)
**	    added code to allocate a QEU_PDI_TBL_DESCR structure and call
**	    qeu_crt_pdi_tbl() to initialize the structure and create and
**	    open the temp table which will be used to keep track of
**	    privileges whose depths have changed as a result of destruction 
**	    of the specified permit
**
**	    added code to invoke qeu_drop_pdi_tbl() to handle closing and
**	    destruction of the temp table
**	13-sep-93 (andre)
**	    When dropping permit(s) or security_alarm(s), the caller may request
**	    that we force invalidation of QPs depending on the object permit(s)
**	    on which have been dropped: 
**	      - if dropping permit(s)/security_alarm(s) on a table, this will 
**	  	be accomplished by altering table's timestamp
**	      - if dropping permit(s)/security_alarm(s) on a view, this will
**              be accomplished by altering timestamp of a base table on which 
**		the view was defined (id of which has been recorded in the view 
**		tree at view creation time)
**	      - if dropping permit(s) on a database procedure, this will be 
**	        accommplished by altering timestamp of a table whose id has 
**	   	been recorded in the IIPROCEDURE catalog
**	      - if dropping permits on a dbevent, we have no timestamps to 
**		alter and since DROP PERMIT operates in RESTRICTed manner, 
**		we can claim that no dbproc or repeat query QP needs to be 
**		invalidated as a result of destroying specified permit(s)
**	21-sep-93 (stephenb)
**	    Creation of protection on an event is a control action on an event
**	    not a create action, changed access mask in call to qeu_secaudit()
**	    to SXF_A_CONTROL when defining protection on event.
**	15-nov-93 (andre)
**	    since an integrity/constraint defined on a table may have the same 
**	    query text id as a permit defined on the same table, when dropping 
**	    text associated with a given permit/security_alarm, we must specify 
**	    three keys: txtid1, txtid2, and mode
**	16-nov-93 (andre)
**	    If dropping a permit or security_alarm on a base table, avoid 
**	    altering its timestamp if it is a catalog (as far as I can tell, we 
**	    NEED to avoid doing it for IIPROTECT, IIPROCEDURE, IISYNONYM, 
**	    IICOMMENT, and IIRULE), but by generalizing it to "all catalogs", 
**	    I ensure that I do not overlook something now or in the future.
**	    This is done to avoid a sutuation where we first alter
**	    a catalog's timestamp (which opens it in exclusive mode), and then 
**	    try to update the catalog itself (e.g. in conjunction with cascading
**	    destruction of privileges and objects (such as views and rules on 
**	    views) rendered abandoned by destruction of privilege represented 
**	    by the permit
**	24-nov-93 (stephenb)
**	    Tidy up drop permit auditing so that we audit drop permit all
**	    only once, and so that we do not audit here for revoke, we have
**	    already done that in qeu_revoke().
**	24-nov-93 (andre)
**	    qeu_v_ubt_id() will take address of DB_ERROR and will report its own
**	    errors
**	07-mar-94 (andre)
**	    fix for bug 58075:
**	    in addition to forcing invalidation of dbproc QPs when the status 
**	    of a dbproc is changed, we will also invoke qeu_destr_dbp_qp() to 
**	    actually destroy dbproc's QP if one exists in QSF
**	27-jun-94 (robf/arc)
**	    Add fix for case where a DROP DBEVENT in B1 when permits exist
**	    on the event then a bad dbevent security label may be passed
**	    to SXF when doing security auditing. We don't audit
**	    a drop permit on dbevent while doing a drop dbevent.
**	19-nov-1996 (nanpr01)
**	    b79442 : Alter table cascade operation will get 
**	    E_QE0018 Illegal parameter in control block.
**	10-Jan-2001 (jenjo02)
**	    Before returning with E_DB_WARN, ensure that the memory
**	    stream is closed.
**	13-may-02 (inkdo01)
**	    Add support for sequence protections.
**	25-mar-03 (inkdo01)
**	    Minor fix to sequence support (fixes revoke processing).
*/
DB_STATUS
qeu_dprot(
QEF_CB          *qef_cb,
QEUQ_CB	    	*qeuq_cb)
{
    GLOBALREF QEF_S_CB *Qef_s_cb;
    DB_IIQRYTEXT    *qtuple;
    DB_IITREE	    *ttuple;
    DB_PROTECTION   *ptuple;
    DB_STATUS	    status, local_status;
    DB_ERROR	    e_error;
    i4	    error;
    bool	    transtarted = FALSE;	    
    bool	    tbl_opened = FALSE;
    bool	    qtext_opened = FALSE;
    bool	    prot_opened = FALSE;
    bool	    permits_exist = FALSE;
    bool	    tbl_altered = FALSE;
    bool	    qps_invalidated = FALSE;
    bool	    dbp_opened = FALSE;
    bool	    audit_dropall = FALSE;
    i4		    i;
    i2		    tree_mode = DB_PROT;
    i4		    txt_mode = DB_PROT;
    QEU_CB	    tranqeu;
    QEU_CB	    pqeu;
    QEU_CB	    qqeu;
    QEU_CB	    tqeu;
    QEU_CB	    dbpqeu;
    DMT_CB	    dmt_cb;
    ULM_RCB         ulm;
    QEF_DATA	    pqef_data;
    QEF_DATA	    qqef_data;
    QEF_DATA	    tqef_data;
    DMT_CHAR_ENTRY  char_array[4];
    DMR_ATTR_ENTRY  pkey_array[3];
    DMR_ATTR_ENTRY  *pkey_ptr_array[3];
    DMR_ATTR_ENTRY  qkey_array[3];
    DMR_ATTR_ENTRY  *qkey_ptr_array[3];
    DMR_ATTR_ENTRY  tkey_array[3];
    DMR_ATTR_ENTRY  *tkey_ptr_array[3];
    DMT_SHW_CB	    dmt_shw_cb;
    DB_TAB_ID	    ubt_id;
    DMT_TBL_ENTRY   table;
    bool	    event_opened = FALSE;
    bool	    err_already_reported = FALSE;
    QEU_CB	    eqeu;		/* For event checks */
    DB_IIEVENT	    evexists, *evcheck;	/* Event tuple for existence check */
    DB_IISEQUENCE   seqexists, *seqcheck; /* Sequence tuple for existence check */
    DMR_ATTR_ENTRY  ekey_array[3];	/* Event keys */
    DMR_ATTR_ENTRY  *ekey_ptr_array[3];
    QEF_DATA	    eqef_data;
    bool	    sequence_opened = FALSE;
    QEU_CB	    sqqeu;		/* For sequence checks */
    DMR_ATTR_ENTRY  sqkey_array[3];	/* Sequence keys */
    DMR_ATTR_ENTRY  *sqkey_ptr_array[3];
    QEF_DATA	    sqqef_data;
    i4         msgid;
    i4              type;    
    DB_PROCEDURE    *dbptuple;
    bool	    dropped_perms = FALSE;
    bool	    check_for_abandoned_objs = FALSE;
    char	    *sec_object;
    i4	    	    sec_obj_len;
    DB_OWN_NAME	    *sec_owner;
    DB_OWN_NAME	    obj_owner;
    QEU_PDI_TBL_DESCR	*pdi_tbl_descr = (QEU_PDI_TBL_DESCR *) NULL;
    i4		    exit_loop, exit_loop2;
    
    for (i=0; i< 3; i++)
    {
	pkey_ptr_array[i] = &pkey_array[i];
	qkey_ptr_array[i] = &qkey_array[i];
	tkey_ptr_array[i] = &tkey_array[i];
	ekey_ptr_array[i] = &ekey_array[i];
	sqkey_ptr_array[i] = &sqkey_array[i];
    }

    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
    ulm.ulm_streamid = (PTR)NULL;

    for (exit_loop = FALSE; !exit_loop; )
    {

	/* Check the control block. */

	error = E_QE0017_BAD_CB;
	status = E_DB_ERROR;
	if (qeuq_cb->qeuq_type != QEUQCB_CB ||
	    qeuq_cb->qeuq_length != sizeof(QEUQ_CB))	
	    break;

	error = E_QE0018_BAD_PARAM_IN_CB;
	if (   (qeuq_cb->qeuq_rtbl == 0)
	    || (qeuq_cb->qeuq_db_id == 0)
	    || (!(qeuq_cb->qeuq_permit_mask &
		    (QEU_PERM_OR_SECALM | QEU_PERM | QEU_SECALM))
	       )
	    || (   qeuq_cb->qeuq_permit_mask & QEU_PERM_OR_SECALM
		&& (   qeuq_cb->qeuq_permit_mask &
			 (  QEU_PERM | QEU_SECALM | QEU_DBP_PROTECTION
			  | QEU_EV_PROTECTION | QEU_SEQ_PROTECTION
			 )
		    || ~qeuq_cb->qeuq_permit_mask & QEU_DROP_ALL
		    || ~qeuq_cb->qeuq_permit_mask & QEU_SKIP_ABANDONED_OBJ_CHECK
		   )
	       )
	    || (   qeuq_cb->qeuq_permit_mask & QEU_PERM
		&& qeuq_cb->qeuq_permit_mask & (QEU_PERM_OR_SECALM | QEU_SECALM)
	       )
	    || (   qeuq_cb->qeuq_permit_mask & QEU_SECALM
		&& qeuq_cb->qeuq_permit_mask &
		     (  QEU_PERM_OR_SECALM | QEU_PERM | QEU_DBP_PROTECTION
		      | QEU_EV_PROTECTION | QEU_SKIP_ABANDONED_OBJ_CHECK
		      | QEU_SEQ_PROTECTION
		     )
	       )
	    || (   qeuq_cb->qeuq_permit_mask & QEU_DBP_PROTECTION
		&& qeuq_cb->qeuq_permit_mask & QEU_EV_PROTECTION
		&& qeuq_cb->qeuq_permit_mask & QEU_SEQ_PROTECTION
	       )
	    || (   qeuq_cb->qeuq_permit_mask &
		    (QEU_DBP_PROTECTION | QEU_EV_PROTECTION |
				QEU_SEQ_PROTECTION)
		&& ~qeuq_cb->qeuq_permit_mask & QEU_PERM
	       )
	    || (   qeuq_cb->qeuq_flag_mask & QEU_DROP_CASCADE
		&& ~qeuq_cb->qeuq_flag_mask & QEU_DROP_COLUMN
		&& (   ~qeuq_cb->qeuq_permit_mask & QEU_PERM
		    || qeuq_cb->qeuq_permit_mask &
			 (  QEU_DROP_ALL | QEU_DBP_PROTECTION
			  | QEU_EV_PROTECTION | QEU_SKIP_ABANDONED_OBJ_CHECK
	 		  | QEU_SEQ_PROTECTION
			 )
		   )
	       )
	    || (qeuq_cb->qeuq_permit_mask &
		    ~(  QEU_PERM_OR_SECALM | QEU_PERM | QEU_SECALM
		      | QEU_DROP_ALL | QEU_DBP_PROTECTION | QEU_EV_PROTECTION
		      | QEU_SEQ_PROTECTION
		      | QEU_SKIP_ABANDONED_OBJ_CHECK | QEU_REVOKE_PRIVILEGES)
	       )
	    || (   qeuq_cb->qeuq_permit_mask & QEU_PERM_OR_SECALM
		&& qeuq_cb->qeuq_flag_mask & QEU_FORCE_QP_INVALIDATION
	       )
	    || (qeuq_cb->qeuq_d_id == 0))
	    break;

	/* Validate event protection */
	if (   (qeuq_cb->qeuq_permit_mask & QEU_EV_PROTECTION)
	    && (qeuq_cb->qeuq_uld_tup == NULL)
	   )
	   break;
	/* Validate sequence protection */
	if (   (qeuq_cb->qeuq_permit_mask & QEU_SEQ_PROTECTION)
	    && (qeuq_cb->qeuq_uld_tup == NULL)
	   )
	   break;
		
	/* 
	** Check to see if a transaction is in progress,
	** if so, set transtarted flag to FALSE, otherwise
	** set it to TRUE after beginning a transaction.
	** If we started a transaction them we will abort
	** it if an error occurs, otherwise we will just
	** return the error and let the caller abort it.
	*/

	if (qef_cb->qef_stat == QEF_NOTRAN)
	{
	    tranqeu.qeu_type = QEUCB_CB;
	    tranqeu.qeu_length = sizeof(QEUCB_CB);
	    tranqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
	    tranqeu.qeu_d_id = qeuq_cb->qeuq_d_id;
	    tranqeu.qeu_flag = 0;
	    tranqeu.qeu_mask = 0;
	    status = qeu_btran(qef_cb, &tranqeu);
	    if (status != E_DB_OK)
	    {	
		error = tranqeu.error.err_code;
		break;	
	    }	    
	    transtarted = TRUE;
	}
	/* escalate the transaction to MST */
	if (qef_cb->qef_auto == QEF_OFF)
	    qef_cb->qef_stat = QEF_MSTRAN;

	if (qeuq_cb->qeuq_permit_mask & QEU_EV_PROTECTION)
	{
	    /*
	    ** If DROP PERMIT ON EVENT (not DROP EVENT) confirm event exists.
	    ** Lock/access modes here are IS, READ as we are dropping objects
	    ** & don't care about orphaned permissions even if readlock=nolock.
	    ** If from DROP EVENT (qeuq_culd = 0) then just make sure we set
	    ** evcheck for later auditing.
	    */
	    evcheck  = (DB_IIEVENT *)qeuq_cb->qeuq_uld_tup->dt_data;
	    if (qeuq_cb->qeuq_culd != 0)	/* Then DROP PERMIT */
	    {
		eqeu.qeu_type = QEUCB_CB;	/* Open iievent */
		eqeu.qeu_length = sizeof(QEUCB_CB);
		eqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
		eqeu.qeu_flag = DMT_U_DIRECT;
		eqeu.qeu_lk_mode = DMT_IS;
		eqeu.qeu_access_mode = DMT_A_READ;
		eqeu.qeu_tab_id.db_tab_base = DM_B_EVENT_TAB_ID; 	
		eqeu.qeu_tab_id.db_tab_index = DM_I_EVENT_TAB_ID; 	
		eqeu.qeu_mask = 0;

		status = qeu_open(qef_cb, &eqeu);
		if (DB_FAILURE_MACRO(status))
		{
		    error = eqeu.error.err_code;
		    break;
		}
		event_opened = TRUE;

		/* Fetch the event tuple with matching name and owner */
		eqeu.qeu_count = 1;
		eqeu.qeu_tup_length = sizeof(DB_IIEVENT);
		eqeu.qeu_output = &eqef_data;
		eqef_data.dt_next = 0;
		eqef_data.dt_size = sizeof(DB_IIEVENT);
		eqef_data.dt_data = (PTR)&evexists;
		eqeu.qeu_getnext = QEU_REPO;
		eqeu.qeu_klen = 2;
		eqeu.qeu_key = ekey_ptr_array;
		ekey_ptr_array[0]->attr_number = DM_1_EVENT_KEY;
		ekey_ptr_array[0]->attr_operator = DMR_OP_EQ;
		ekey_ptr_array[0]->attr_value = (char *)&evcheck->dbe_name;
		ekey_ptr_array[1]->attr_number = DM_2_EVENT_KEY;
		ekey_ptr_array[1]->attr_operator = DMR_OP_EQ;
		ekey_ptr_array[1]->attr_value = (char *)&evcheck->dbe_owner;
		eqeu.qeu_qual = 0;
		eqeu.qeu_qarg = 0;
		status = qeu_get(qef_cb, &eqeu);
		if (status != E_DB_OK)
		{
		    if (eqeu.error.err_code == E_QE0015_NO_MORE_ROWS)
		    {
			(VOID)qef_error(E_QE020E_EV_PERMIT_ABSENT, 0L,
				      status, &error, &qeuq_cb->error, 2,
				      qec_trimwhite(sizeof(evcheck->dbe_name),
						    (char *)&evcheck->dbe_name),
				      (PTR)&evcheck->dbe_name,
				      qec_trimwhite(sizeof(evcheck->dbe_owner),
						   (char *)&evcheck->dbe_owner),
				      (PTR)&evcheck->dbe_owner);
			error = E_QE0025_USER_ERROR;
		    }
		    else
		    {
			error = eqeu.error.err_code;
		    }
		    break;
		} /* If not OK */

		/* Set the input unique event (table) id to the fetched event */
		STRUCT_ASSIGN_MACRO(evexists.dbe_uniqueid, *qeuq_cb->qeuq_rtbl);

		status = qeu_close(qef_cb, &eqeu);    	/* Close iievent */
		if (DB_FAILURE_MACRO(status))
		{
		    error = eqeu.error.err_code;
		    break;
		}
		event_opened = FALSE;
	    } /* If from DROP PERMIT */
	}
	else if (qeuq_cb->qeuq_permit_mask & QEU_SEQ_PROTECTION)
	{
	    /*
	    ** If DROP PERMIT ON SEQUENCE (not DROP SEQUENCE) confirm sequence exists.
	    ** Lock/access modes here are IS, READ as we are dropping objects
	    ** & don't care about orphaned permissions even if readlock=nolock.
	    ** If from DROP SEQUENCE (qeuq_culd = 0) then just make sure we set
	    ** seqcheck for later auditing.
	    */
	    seqcheck  = (DB_IISEQUENCE *)qeuq_cb->qeuq_uld_tup->dt_data;
	    if (qeuq_cb->qeuq_culd != 0)	/* Then DROP PERMIT */
	    {
		sqqeu.qeu_type = QEUCB_CB;	/* Open iisequence */
		sqqeu.qeu_length = sizeof(QEUCB_CB);
		sqqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
		sqqeu.qeu_flag = DMT_U_DIRECT;
		sqqeu.qeu_lk_mode = DMT_IS;
		sqqeu.qeu_access_mode = DMT_A_READ;
		sqqeu.qeu_tab_id.db_tab_base = DM_B_SEQ_TAB_ID; 	
		sqqeu.qeu_tab_id.db_tab_index = DM_I_SEQ_TAB_ID; 	
		sqqeu.qeu_mask = 0;

		status = qeu_open(qef_cb, &sqqeu);
		if (DB_FAILURE_MACRO(status))
		{
		    error = sqqeu.error.err_code;
		    break;
		}
		sequence_opened = TRUE;

		/* Fetch the sequence tuple with matching name and owner */
		sqqeu.qeu_count = 1;
		sqqeu.qeu_tup_length = sizeof(DB_IISEQUENCE);
		sqqeu.qeu_output = &sqqef_data;
		sqqef_data.dt_next = 0;
		sqqef_data.dt_size = sizeof(DB_IISEQUENCE);
		sqqef_data.dt_data = (PTR)&seqexists;
		sqqeu.qeu_getnext = QEU_REPO;
		sqqeu.qeu_klen = 2;
		sqqeu.qeu_key = sqkey_ptr_array;
		sqkey_ptr_array[0]->attr_number = DM_1_SEQ_KEY;
		sqkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
		sqkey_ptr_array[0]->attr_value = (char *)&seqcheck->dbs_name;
		sqkey_ptr_array[1]->attr_number = DM_2_SEQ_KEY;
		sqkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
		sqkey_ptr_array[1]->attr_value = (char *)&seqcheck->dbs_owner;
		sqqeu.qeu_qual = 0;
		sqqeu.qeu_qarg = 0;
		status = qeu_get(qef_cb, &sqqeu);
		if (status != E_DB_OK)
		{
		    if (sqqeu.error.err_code == E_QE0015_NO_MORE_ROWS)
		    {
			(VOID)qef_error(E_QE0239_SEQ_PERMIT_ABSENT, 0L,
				      status, &error, &qeuq_cb->error, 2,
				      qec_trimwhite(sizeof(seqcheck->dbs_name),
						    (char *)&seqcheck->dbs_name),
				      (PTR)&seqcheck->dbs_name,
				      qec_trimwhite(sizeof(seqcheck->dbs_owner),
						   (char *)&seqcheck->dbs_owner),
				      (PTR)&seqcheck->dbs_owner);
			error = E_QE0025_USER_ERROR;
		    }
		    else
		    {
			error = sqqeu.error.err_code;
		    }
		    break;
		} /* If not OK */

		/* Set the input unique sequence (table) id to the fetched sequence */
		STRUCT_ASSIGN_MACRO(seqexists.dbs_uniqueid, *qeuq_cb->qeuq_rtbl);

		status = qeu_close(qef_cb, &sqqeu);    	/* Close iisequence */
		if (DB_FAILURE_MACRO(status))
		{
		    error = sqqeu.error.err_code;
		    break;
		}
		sequence_opened = FALSE;
	    } /* If from DROP PERMIT */
	}
	else if (!(qeuq_cb->qeuq_permit_mask & QEU_DBP_PROTECTION))
	{
	    /* This is a security event, prepare for audit. */
	    type = SXF_E_TABLE;
	    msgid = I_SX2018_PROT_TAB_DROP;

	    /* Must get table name, owner */

	    dmt_shw_cb.length = sizeof(DMT_SHW_CB);
	    dmt_shw_cb.type = DMT_SH_CB;
	    dmt_shw_cb.dmt_db_id = qeuq_cb->qeuq_db_id;
	    dmt_shw_cb.dmt_session_id = qeuq_cb->qeuq_d_id;
	    dmt_shw_cb.dmt_flags_mask = DMT_M_TABLE;
	    dmt_shw_cb.dmt_table.data_address = (char *)&table;
	    dmt_shw_cb.dmt_table.data_in_size = sizeof(DMT_TBL_ENTRY);
	    dmt_shw_cb.dmt_char_array.data_in_size = 0;
	    dmt_shw_cb.dmt_char_array.data_out_size  = 0;
	    dmt_shw_cb.dmt_tab_id.db_tab_base = 
			       qeuq_cb->qeuq_rtbl->db_tab_base;
	    dmt_shw_cb.dmt_tab_id.db_tab_index = 
			       qeuq_cb->qeuq_rtbl->db_tab_index;

	    /* issue DMT_SHOW call */

	    if ((status = dmf_call(DMT_SHOW, &dmt_shw_cb)) == E_DB_OK)
	    {
		sec_object = table.tbl_name.db_tab_name;
		sec_obj_len = sizeof(table.tbl_name.db_tab_name);
		sec_owner  = &table.tbl_owner;
	    }
	    else
	    {
		error = dmt_shw_cb.error.err_code;
		break;
	    }
	}

	/* Get a memory stream for reading catalog tuples. */
	for (exit_loop2 = FALSE; !exit_loop2; ) 	/* to break out of */
	{
	    /* Open stream and allocate list in one action */
	    ulm.ulm_flags = ULM_PRIVATE_STREAM | ULM_OPEN_AND_PALLOC;
	    ulm.ulm_psize = sizeof(DB_PROTECTION);

	    if ((status = qec_mopen(&ulm)) != E_DB_OK)
		break;

	    ptuple = (DB_PROTECTION*) ulm.ulm_pptr;

	    ulm.ulm_psize = sizeof(DB_IIQRYTEXT);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    qtuple = (DB_IIQRYTEXT*) ulm.ulm_pptr;

	    ulm.ulm_psize = sizeof(DB_IITREE);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    ttuple = (DB_IITREE*) ulm.ulm_pptr;

	    exit_loop2 = TRUE;
	}
	if (status != E_DB_OK)
	{
	    error = E_QE001E_NO_MEM;
	    break;
	}

	error = E_QE0000_OK;
    
	/* Open the iitree, iiqrytext and iiprotect system catalog */

	pqeu.qeu_type = QEUCB_CB;
	pqeu.qeu_length = sizeof(QEUCB_CB);
	pqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
	pqeu.qeu_lk_mode = DMT_IX;
	pqeu.qeu_flag = DMT_U_DIRECT;
	pqeu.qeu_access_mode = DMT_A_WRITE;
	pqeu.qeu_mask = 0;
		
	STRUCT_ASSIGN_MACRO(pqeu, qqeu);
	STRUCT_ASSIGN_MACRO(pqeu, tqeu);

	pqeu.qeu_tab_id.db_tab_base  = DM_B_PROTECT_TAB_ID;
	pqeu.qeu_tab_id.db_tab_index  = DM_I_PROTECT_TAB_ID;
	status = qeu_open(qef_cb, &pqeu);
	if (status != E_DB_OK)
	{
	    error = pqeu.error.err_code;
	    break;
	}
	prot_opened = TRUE;
	qqeu.qeu_tab_id.db_tab_base = DM_B_QRYTEXT_TAB_ID;
	qqeu.qeu_tab_id.db_tab_index = 0L;
	status = qeu_open(qef_cb, &qqeu);
	if (status != E_DB_OK)
	{
	    error = qqeu.error.err_code;
	    break;
	}
	qtext_opened = TRUE;
	tqeu.qeu_tab_id.db_tab_base  = DM_B_TREE_TAB_ID;
	tqeu.qeu_tab_id.db_tab_index  = DM_I_TREE_TAB_ID;
	status = qeu_open(qef_cb, &tqeu);
	if (status != E_DB_OK)
	{
	    error = tqeu.error.err_code;
	    break;
	}
	tbl_opened = TRUE;
	pqeu.qeu_count = 1;
	pqeu.qeu_tup_length = sizeof(DB_PROTECTION);
	pqeu.qeu_output = &pqef_data;
	pqef_data.dt_next = 0;
	pqef_data.dt_size = sizeof(DB_PROTECTION);
	pqef_data.dt_data = (PTR) ptuple;

	    
	qqeu.qeu_count = 1;
	qqeu.qeu_tup_length = sizeof(DB_IIQRYTEXT);
	qqeu.qeu_output = &qqef_data;
	qqef_data.dt_next = 0;
	qqef_data.dt_size = sizeof(DB_IIQRYTEXT);
	qqef_data.dt_data = (PTR) qtuple;


	tqeu.qeu_count = 1;
	tqeu.qeu_tup_length = sizeof(DB_IITREE);
	tqeu.qeu_output = &tqef_data;
	tqef_data.dt_next = 0;
	tqef_data.dt_size = sizeof(DB_IITREE);
	tqef_data.dt_data = (PTR) ttuple;

	/* 
	** Position Protect table and get all
	** protections matching the specified table
	** id.  For each of these find the associated 
	** query text and tree tuples and delete them.
	*/	

	pqeu.qeu_getnext = QEU_REPO;
	pqeu.qeu_klen = 2;       
	pqeu.qeu_key = pkey_ptr_array;
	pkey_ptr_array[0]->attr_number = DM_1_PROTECT_KEY;
	pkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	pkey_ptr_array[0]->attr_value = (char*) 
				&qeuq_cb->qeuq_rtbl->db_tab_base;
	pkey_ptr_array[1]->attr_number = DM_2_PROTECT_KEY;
	pkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	pkey_ptr_array[1]->attr_value = (char*) 
				 &qeuq_cb->qeuq_rtbl->db_tab_index;
	
	pqeu.qeu_qual = 0;
	pqeu.qeu_qarg = 0;
     
	for(;;)	    /* loop for all protection tuples */
	{
	    FUNC_EXTERN	    i4	    qeu_tr_ts_cmp();

	    status = qeu_get(qef_cb, &pqeu);
	    if (status != E_DB_OK)
	    {
		error = pqeu.error.err_code;
		if (error == E_QE0015_NO_MORE_ROWS)
		{
		    /* If you get no more rows looking for a specific
		    ** protection number it is an error (report it),
		    ** otherwise it is OK.
		    */
		    if (~qeuq_cb->qeuq_permit_mask & QEU_DROP_ALL)
		    {
			(VOID)qef_error(5204, 0L, status, &error,
					&qeuq_cb->error, 1,
					sizeof(qeuq_cb->qeuq_pno),
					&qeuq_cb->qeuq_pno);
			error = E_QE0025_USER_ERROR;
		    }
		    else
		    {
			error = E_QE0000_OK;
			status = E_DB_OK;
		    }
		}
		break;
			
	    }
	    pqeu.qeu_getnext = QEU_NOREPO;
	    pqeu.qeu_klen = 0;

	    if (   ~qeuq_cb->qeuq_permit_mask & QEU_DROP_ALL
		&& qeuq_cb->qeuq_pno != ptuple->dbp_permno)
	    {
		permits_exist = TRUE;	    /* Note other permits exist */
		continue;
	    }

	    /*
	    ** If we are dropping permits only, ensure this is not a
	    ** security_alarm.  If we are dropping security_alarms 
	    ** only, ensure this is not a permit.  If we are dropping only
	    ** permits or only security_alarms and and current tuple wil not be
	    ** dropped, remember that there are some permits/security_alarms
	    ** defined on the table/view
	    */
	    if ((qeuq_cb->qeuq_permit_mask & QEU_PERM) &&
		(ptuple->dbp_popset & DB_ALARM))
	    {
		permits_exist = TRUE;	    /* Note other permits exist */
		continue;
	    }
	    if ((qeuq_cb->qeuq_permit_mask & QEU_SECALM) &&
		!(ptuple->dbp_popset & DB_ALARM))
	    {
		permits_exist = TRUE;	    /* Note other permits exist */
		continue;
	    }

	    /*
	    ** if function called to drop a permission that applies to a 
	    ** specific table column, indicated by QEU_DROP_COLUMN flag,
	    ** then attribute number of column passed in qeuq_ano must be
	    ** tested against the column bitmap of the permission tuple.
	    **
	    ** if all bits in the bitmap are set on, then the permission
	    ** applies to all columns.  If so, then the permission tuple
	    ** will not be deleted.
	    **
	    ** If the column bit is set on for the specific column and not
	    ** all bits are set, then the tuple will be deleted if the
	    ** QEU_DROP_CASCADE flag is set.  Otherwise the call is for a
	    ** RESTRICT drop column which can only be allowed if there are no
	    ** column dependent objects.  In this case an error code is
	    ** returned.
	    */

	    if (qeuq_cb->qeuq_flag_mask & QEU_DROP_COLUMN)
	    {
	  	if ( (!BTtest (qeuq_cb->qeuq_ano,
		     		(char *) ptuple->dbp_domset)) ||
		     (BTcount((char *) ptuple->dbp_domset,
	       			DB_COL_BITS) == DB_COL_BITS )  )
		{
	    	   if ( !(qeuq_cb->qeuq_permit_mask & QEU_DROP_ALL) )
		   {
		       /* Don't leak memory stream! */
		       if ( ulm.ulm_streamid )
			   ulm_closestream(&ulm);
		       return (E_DB_WARN);
		   }

		   permits_exist = TRUE;
		   continue;
		}

		if (!(qeuq_cb->qeuq_flag_mask & QEU_DROP_CASCADE))
		{
		   error = E_QE016B_COLUMN_HAS_OBJECTS;
		   status = E_DB_ERROR;
		   break;
		}
	    }

	    
	    /* 
            ** Now delete IITREE tuple(s) for the given table id that match 
	    ** the tree id stored in the IIPROTECT tuple 
	    */	    
	    tqeu.qeu_klen = 3;       
	    tqeu.qeu_key = tkey_ptr_array;
	    tkey_ptr_array[0]->attr_number = DM_1_TREE_KEY;
	    tkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	    tkey_ptr_array[0]->attr_value = (char*) 
				&qeuq_cb->qeuq_rtbl->db_tab_base;
	    tkey_ptr_array[1]->attr_number = DM_2_TREE_KEY;
	    tkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	    tkey_ptr_array[1]->attr_value = (char*) 
                                 &qeuq_cb->qeuq_rtbl->db_tab_index;
        
	    tkey_ptr_array[2]->attr_number = DM_3_TREE_KEY;
	    tkey_ptr_array[2]->attr_operator = DMR_OP_EQ;
	    tkey_ptr_array[2]->attr_value = (char *) &tree_mode;

	    tqeu.qeu_f_qual = 0;
	    tqeu.qeu_f_qarg = 0;
	    tqeu.qeu_qual = ( PTR ) qeu_tr_ts_cmp;
	    tqeu.qeu_qarg = ( PTR ) &ptuple->dbp_treeid;
            tqeu.qeu_tup_length = sizeof(DB_IITREE);
	    status = qeu_delete(qef_cb, &tqeu);
	    if (status != E_DB_OK)
	    {
		error = tqeu.error.err_code;
		break;
	    }

	    /* 
            ** Now delete IIQRYTEXT tuples using query id associated with 
	    ** this protection and mode (DB_PROT).
	    */

	    qqeu.qeu_klen = 3;       

	    qqeu.qeu_key = qkey_ptr_array;

	    qkey_ptr_array[0]->attr_number = DM_1_QRYTEXT_KEY;
	    qkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	    qkey_ptr_array[0]->attr_value = (char*) 
				&ptuple->dbp_txtid.db_qry_high_time;

	    qkey_ptr_array[1]->attr_number = DM_2_QRYTEXT_KEY;
	    qkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	    qkey_ptr_array[1]->attr_value = (char*) 
				&ptuple->dbp_txtid.db_qry_low_time;

	    qkey_ptr_array[2]->attr_number = DM_3_QRYTEXT_KEY;
	    qkey_ptr_array[2]->attr_operator = DMR_OP_EQ;
	    qkey_ptr_array[2]->attr_value = (char*) &txt_mode;
        
	    qqeu.qeu_qual = qqeu.qeu_f_qual = 0;
	    qqeu.qeu_qarg = qqeu.qeu_f_qarg = 0;

            qqeu.qeu_tup_length = sizeof(DB_IIQRYTEXT);
	    status = qeu_delete(qef_cb, &qqeu);
	    if (status != E_DB_OK)
	    {
		error = qqeu.error.err_code;
		break;
	    }

	    /* Now delete the protection tuple. */
            pqeu.qeu_tup_length = sizeof(DB_PROTECTION);
	    status = qeu_delete(qef_cb, &pqeu);
	    if (status != E_DB_OK)
	    {
		error = pqeu.error.err_code;
		break;
	    }

	    /* 
	    ** if we were asked to delete a specific permit on a table or 
	    ** a view, remove IIDBDEPENDS tuples that reference the
	    ** deleted permit and, if permit was defined on a view and
	    ** depended on some privilege descriptor on which no other
	    ** permit depends, IIPRIV tuple.
	    **
	    ** Permits on dbevents, sequences and dbprocs will have no
	    ** dependencies recorded in IIDBDEPENDS and neither will
	    ** security alarms.  If dropping all permits on a table or a view,
	    ** postpone calling qeu_dbdelete() until we are done with all
	    ** permits as the function offers special handling for that case
	    */
	    if ( ( !(qeuq_cb->qeuq_permit_mask &
		     (QEU_DBP_PROTECTION | QEU_EV_PROTECTION | QEU_SEQ_PROTECTION 
		    | QEU_DROP_ALL))
		  && ~ptuple->dbp_popset & DB_ALARM
		 )
	    	|| (qeuq_cb->qeuq_flag_mask & QEU_DROP_COLUMN)
	       )
	    {
		/* 
		** IIDBDEPENDS may indicate that a permit depends on
		** some privilege descriptor; we will remove the
		** IIDBDEPENDS tuple, then check whether there are any
		** more permits dependent on the privilege descriptor,
		** and if none are found, delete that privilege
		** descriptor from IIPRIV
		*/
		status = qeu_dbdelete(qef_cb, qeuq_cb, qeuq_cb->qeuq_rtbl, 
			(i4) DB_PROT, (i4) ptuple->dbp_permno, &pqeu.error);
		if (status != E_DB_OK)
		{
		    error = pqeu.error.err_code;
		    break;
		}
	    }

	    /* This is a security event, must audit. */
	    if ( Qef_s_cb->qef_state & QEF_S_C2SECURE )
	    {
		if (qeuq_cb->qeuq_permit_mask & QEU_DBP_PROTECTION)
		{
		    /* Must be a procedure not a table. */
		    type = SXF_E_PROCEDURE;
		    msgid = I_SX2019_PROT_DBP_DROP;
		    dbptuple = (DB_PROCEDURE *)qeuq_cb->qeuq_dbp_tup->dt_data;
		    sec_object = (char *)&dbptuple->db_dbpname;
		    sec_obj_len = sizeof(dbptuple->db_dbpname);
		    sec_owner  = &dbptuple->db_owner;
		}
		else if (qeuq_cb->qeuq_permit_mask & QEU_EV_PROTECTION)
		{
		    type = SXF_E_EVENT;
		    sec_object = (char *)&evcheck->dbe_name;
		    sec_obj_len = sizeof(evcheck->dbe_name);
		    sec_owner  = &evcheck->dbe_owner;
		    if (qeuq_cb->qeuq_culd == 0)	/* Not specific permit */
		    {
			    msgid=0; /* Nothing to audit */
		    }
		    else
		    {
			    msgid = I_SX2031_PROT_EV_DROP;
		    }
		}
		else if (qeuq_cb->qeuq_permit_mask & QEU_SEQ_PROTECTION)
		{
		    type = SXF_E_EVENT;
		    sec_object = (char *)&seqcheck->dbs_name;
		    sec_obj_len = sizeof(seqcheck->dbs_name);
		    sec_owner  = &seqcheck->dbs_owner;
		    if (qeuq_cb->qeuq_culd == 0)	/* Not specific permit */
		    {
			    msgid=0; /* Nothing to audit */
		    }
		    else
		    {
			    msgid = I_SX204A_PROT_SEQ_DROP;
		    }
		}
		else	/* Must be a table */
		{
		    type = SXF_E_TABLE;
		    if  (ptuple->dbp_popset & DB_ALARM)
			msgid = I_SX202E_ALARM_DROP;
		    else
			msgid = I_SX2018_PROT_TAB_DROP;

		    sec_object = table.tbl_name.db_tab_name;
		    sec_obj_len = sizeof(table.tbl_name.db_tab_name);
		    sec_owner  = &table.tbl_owner;
		}
		/*
		** only audit here if we are not using drop permit all and
		** we have not been called from qeu_revoke() 
		** Do not audit if dropping a dbevent either.
		*/
		if (!(qeuq_cb->qeuq_permit_mask & 
		      (QEU_DROP_ALL | QEU_REVOKE_PRIVILEGES)) &&
		      msgid!=0)
		{
		    status = qeu_secaudit(FALSE, qef_cb->qef_ses_id, 
		      sec_object, sec_owner, sec_obj_len, type, msgid, 
		      SXF_A_SUCCESS | SXF_A_CONTROL, 
		      &e_error);
		} 
		else  if(msgid!=0)
		{
		    /* audit drop permit all later */
		    audit_dropall = TRUE;
		}
		if (status != E_DB_OK)
		    break;
	    }

	    /* if we have just dropped a permit, remember it */
	    if (~ptuple->dbp_popset & DB_ALARM)
	    {
		dropped_perms = TRUE;

		/*
		** unless
		**  - we have already found one permit that could result
		**    in some object becoming abandoned or
		**  - we were told to not check for abandoned objects
		** we will have to check whether dropping this permit could
		** render some object and/or permit abandoned.
		** To qualify, a permit must meet the following conditions:
		**  - it must not be an "old" (pre-6.5) permit,
		**  - grantee must be different from the grantor (currently
		**    only a user can be a grantor)
		**  - it must be GRANT-compatible or must specify SELECT
		**    privilege (unfortunately, QUEL views can depend on
		**    non-GRANT compatible permits)
		**
		** if permit specified some privilege WGO (must have been
		** GRANT-compatible), in addition to rendering some object
		** abandoned destroying it could also render some permit(s)
		** abandoned
		*/

		if (   ~qeuq_cb->qeuq_permit_mask & 
			    QEU_SKIP_ABANDONED_OBJ_CHECK
		    && !check_for_abandoned_objs
		    && ptuple->dbp_flags & DBP_65_PLUS_PERM
		    && (!(   ptuple->dbp_gtype == DBGR_USER
			  && !MEcmp((PTR) &ptuple->dbp_grantor,
				    (PTR) &ptuple->dbp_owner,
				    sizeof(ptuple->dbp_owner))
			 )
		       )
		    && (   ptuple->dbp_popset & DB_RETRIEVE
		        || qeu_grant_compatible(ptuple,
		    	       (bool) (   ptuple->dbp_treeid.db_tre_high_time
				       || ptuple->dbp_treeid.db_tre_low_time),
			       (bool) (STskipblank((char *) &ptuple->dbp_term,
				           sizeof(DB_TERM_NAME)) != 
					       (char *) NULL))
			   )
		   )
		{
		    check_for_abandoned_objs = TRUE;
		}
	    }

	    /* If doing a specific protection or security_alarm, we're done. */
	    if (~qeuq_cb->qeuq_permit_mask & QEU_DROP_ALL)
	    {
		break;
	    }
	}
	if (status != E_DB_OK)
	    break;
	/*
	** Audit "drop permit all" here, outside the loop, that way we only get
	** one audit record. But only if we have not been called from 
	** qeu_revoke().
	*/
	if (audit_dropall && 
	    !(qeuq_cb->qeuq_permit_mask & QEU_REVOKE_PRIVILEGES) &&
	     Qef_s_cb->qef_state & QEF_S_C2SECURE )
	{
	    status = qeu_secaudit(FALSE, qef_cb->qef_ses_id, 
	    	sec_object, sec_owner, sec_obj_len, type, msgid, 
	    	SXF_A_SUCCESS | SXF_A_CONTROL, 
	        &e_error);
	}

	/* 
	** if we were asked to delete all permits on a table or a view, 
	** remove all IIDBDEPENDS tuples that reference some permit defined
	** on this table or view and all IIPRIV tuples that represent
	** privilege descriptors (if any) created to represent privileges
	** on this object (applies only to views).  
	**
	** we postponed calling qeu_dbdelete() until now since it
	** contains an optimization for the case when we are dropping all
	** permits on an object: all IIDBDEPENDS tuples expressing
	** dependencies of any permit on this object will be removed as
	** will all IIPRIV tuples for this object with privilege
	** descriptor # > 0
	**
	** Permits on dbevents, sequences and dbprocs will have no
	** dependencies recorded in IIDBDEPENDS and neither will
	** security alarms, so if we were asked to drop all permits on
	** a dbproc or a dbevent or all security_alarms on a table, there
	** will be no IIDBDEPENDS and IIPRIV tuples to delete.  
	*/
	if (   dropped_perms
	    && qeuq_cb->qeuq_permit_mask & QEU_DROP_ALL
	    && !(qeuq_cb->qeuq_flag_mask & QEU_DROP_COLUMN)
	    && !(qeuq_cb->qeuq_permit_mask &
		 (QEU_DBP_PROTECTION | QEU_EV_PROTECTION | QEU_SEQ_PROTECTION 
		| QEU_SECALM))
	   )
	{
	    status = qeu_dbdelete(qef_cb, qeuq_cb, qeuq_cb->qeuq_rtbl, 
	        (i4) DB_PROT, (i4) 0, &pqeu.error);
	    if (status != E_DB_OK)
	    {
		error = pqeu.error.err_code;
		break;
	    }
	}

	/*
	** If dropping all permits on a table,
	** BUT NOT the table itself,
	** and no permits or security alarms remain,
	** turn off DMT_C_PERMITS for the table.
	*/
	if (!(qeuq_cb->qeuq_permit_mask &
	          (QEU_DBP_PROTECTION | QEU_EV_PROTECTION | QEU_SEQ_PROTECTION)) &&
	    (~qeuq_cb->qeuq_permit_mask & QEU_PERM_OR_SECALM) &&
	    !(permits_exist) &&			  /* No permits/secalms, && */
	     (qeuq_cb->qeuq_permit_mask & QEU_DROP_ALL) &&  /* Droping all,&& */
	     (~qeuq_cb->qeuq_permit_mask & QEU_SECALM) /* permits */
	   )
	{
	    dmt_cb.dmt_char_array.data_address = (PTR) char_array;
	    dmt_cb.dmt_char_array.data_in_size = 
					sizeof(DMT_CHAR_ENTRY) * 1;
	    char_array[0].char_id = DMT_C_PERMITS;
	    char_array[0].char_value = DMT_C_OFF;

	    dmt_cb.length = sizeof(DMT_CB);
	    dmt_cb.type = DMT_TABLE_CB;
	    dmt_cb.dmt_id.db_tab_base = qeuq_cb->qeuq_rtbl->db_tab_base;
	    dmt_cb.dmt_id.db_tab_index = qeuq_cb->qeuq_rtbl->db_tab_index;
	    dmt_cb.dmt_tran_id = qef_cb->qef_dmt_id;
	    dmt_cb.dmt_flags_mask = 0;
	    dmt_cb.dmt_db_id = qeuq_cb->qeuq_db_id;

	    status = dmf_call(DMT_ALTER, &dmt_cb);
	    if (status != E_DB_OK)
	    {
		error = dmt_cb.error.err_code;
		break;
	    }

	    /*
	    ** remember that the table/view permit(s)/security_alarm(s) on 
	    ** which have been dropped has been "altered" which allows us to 
	    ** conclude that its timestamp has been changed
	    */
	    tbl_altered = TRUE;
	}

	/*
	** if we were told to force invalidation of QPs dependent on the object
	** permit(s)/security_alarm(s) on which have been dropped, now's a good
	** time to do it; recall that there is not a heck of a lot that we can 
	** do if permit(s) on dbevent or sequence have been dropped
	*/
	if (   qeuq_cb->qeuq_flag_mask & QEU_FORCE_QP_INVALIDATION
	    && ~qeuq_cb->qeuq_permit_mask & QEU_EV_PROTECTION
	    && ~qeuq_cb->qeuq_permit_mask & QEU_SEQ_PROTECTION)
	{
	    if (qeuq_cb->qeuq_permit_mask & QEU_DBP_PROTECTION)
	    {
		/* 
		** permit(s) dropped on a dbproc - need to read IIPROCEDURE 
		** tuple describing this dbproc and change timestamp of a table
		** whose id was recorded in db_dbp_ubt_id at [re]creation time
		*/

		dbptuple = (DB_PROCEDURE *) qeuq_cb->qeuq_dbp_tup->dt_data;

		status = qeu_p_ubt_id(qef_cb, &dbptuple->db_owner, 
		    &dbptuple->db_dbpname, &ubt_id, &error);
  		if (DB_FAILURE_MACRO(status))
		    break;
	    }
	    else
	    {
		/*
		** permit(s) or security_alarm(s) dropped on a table or view -
		** if the object is a base table, is not a catalog, and its 
		** timestamp has not already been altered (as would happen if 
		** the user issued DROP PERMIT ALL and there were no more 
		** security_alarms left lying around), we must do it now; if, 
		** on the other hand, the object is a view, we need to determine
		** id of some user base table used in its definition and alter 
		** its timestamp
		*/
		if (table.tbl_status_mask & DMT_VIEW)
		{
		    status = qeu_v_ubt_id(qef_cb, qeuq_cb->qeuq_rtbl, &ubt_id, 
			&qeuq_cb->error);
		    if (DB_FAILURE_MACRO(status))
		    {
			err_already_reported = TRUE;
			break;
		    }
		}
		else if (!tbl_altered && ~table.tbl_status_mask & DMT_CATALOG)
		{
		    ubt_id.db_tab_base  = qeuq_cb->qeuq_rtbl->db_tab_base;
		    ubt_id.db_tab_index = qeuq_cb->qeuq_rtbl->db_tab_index;
		}
		else
		{
		    /* 
		    ** if the base table was DMT_ALTER'd, QPs depending on it 
		    ** have been invalidated - otherwise, remember that if any 
		    ** other object is affected by destruction of specified 
		    ** permit(s), we need to try to force invalidation of QPs
		    ** which depend on it
		    */
		    qps_invalidated = tbl_altered;

		    ubt_id.db_tab_base = ubt_id.db_tab_index = 0;
		}
	    }
		
	    if (ubt_id.db_tab_base)
	    {
	        status = qeu_mdfy_timestamp(qef_cb, &ubt_id, &error);
	        if (DB_FAILURE_MACRO(status))
		    break;
		
		/*
		** remember that we forced invalidation of QPs that may depend
		** on permits that were dropped
		*/
		qps_invalidated = TRUE;
	    }
	}

	/* if we dropped permit(s) on a dbproc, destroy its QP */
	if (qeuq_cb->qeuq_permit_mask & QEU_DBP_PROTECTION && dropped_perms)
	{
	    dbptuple = (DB_PROCEDURE *) qeuq_cb->qeuq_dbp_tup->dt_data;

	    status = qeu_destr_dbp_qp(qef_cb, &dbptuple->db_owner,
		&dbptuple->db_dbpname, &qeuq_cb->error);
	    if (DB_FAILURE_MACRO(status))
	    {
		err_already_reported = TRUE;
		break;
	    }
	}

	/*
	** unless we were told to skip checking whether destroying specified
	** permit(s) rendered some objects abandoned, if we have destroyed some
	** permits which could result in objects and/or permits becoming
	** abandoned, we need to determine whether any, in fact, became
	** abandoned.  If the caller specified that cascading destruction must
	** take place, all abandoned objects will be destroyed; otherwise, if
	** any object would become abandoned, it will be treated as an error.
	**
	** Unless we were processing a request to drop ALL permits on an object,
	** first we will determine whether any privileges on which some DBMS
	** object could depend have been destroyed.  This will require
	** that we walk the list of remaining permits and try to
	** determine whether one or more of them provide the same
	** privilege as the permit that was destroyed.
	*/
	if (check_for_abandoned_objs)
	{
	    QEU_REVOKE_INFO	*privs_lost, *grant_option_lost;

	    ulm.ulm_psize = 2 * sizeof(QEU_REVOKE_INFO);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		error = E_QE001E_NO_MEM;
		break;
	    }
	    privs_lost = (QEU_REVOKE_INFO *) ulm.ulm_pptr;

	    if (qeuq_cb->qeuq_permit_mask & QEU_DROP_ALL)
	    {
		/*
		** if all permits have been dropped, we can safely claim that
		** PUBLIC lost all privileges along with ability to grant all
		** privileges; *privs_lost will represent the fact that
		** ALL privileges were lost to PUBLIC and *grant_option_lost 
		** will represent the fact that GRANT OPTION FOR ALL PRIVILEGES
		** was lost to PUBLIC
		*/
	        STRUCT_ASSIGN_MACRO((*qeuq_cb->qeuq_rtbl), 
				    privs_lost->qeu_obj_id);

		if (qeuq_cb->qeuq_permit_mask & QEU_DBP_PROTECTION)
		{
		    /* dropped all permits on a dbproc */
		    privs_lost->qeu_priv = DB_EXECUTE;
		}
		else if (qeuq_cb->qeuq_permit_mask & QEU_EV_PROTECTION)
		{
		    /* dropped all permits on a dbevent */
		    privs_lost->qeu_priv = DB_EVREGISTER | DB_EVRAISE;
		}
		else if (qeuq_cb->qeuq_permit_mask & QEU_SEQ_PROTECTION)
		{
		    /* dropped all permits on a dbevent */
		    privs_lost->qeu_priv = DB_NEXT;
		}
		else
		{
		    /* dropped all permits on a table or view */
		    privs_lost->qeu_priv = 
			DB_RETRIEVE | DB_DELETE | DB_REPLACE | DB_APPEND |
			DB_REFERENCES | DB_COPY_INTO | DB_COPY_FROM;

		    /*
		    ** set all bits in qeu_attrmap to indicate that privileges
		    ** on the table and all of its attributes have been lost
		    */
		    for (i = 0; i < DB_COL_WORDS; i++)
		    {
			privs_lost->qeu_attrmap[i] = ~((i4) 0);
		    }
		}
		
		privs_lost->qeu_gtype = DBGR_PUBLIC;
		MEfill(sizeof(privs_lost->qeu_authid), ' ',
		    (PTR) &privs_lost->qeu_authid);

		grant_option_lost = privs_lost + 1;

		STRUCT_ASSIGN_MACRO((*privs_lost), (*grant_option_lost));
		privs_lost->qeu_flags = QEU_PRIV_LOST;
		grant_option_lost->qeu_flags = QEU_GRANT_OPTION_LOST;
	    }
	    else
	    {
		QEU_PERM_DEPTH_INFO	    *perm_depth_info;

		/* 
		** ptuple describes permit that was destroyed; 
		** *privs_lost will describe privilege that was lost;
		** if permit specified privilege WGO, *grant_option_lost 
		** will describe privilege GRANT OPTION FOR which was
		** lost
		*/
		STRUCT_ASSIGN_MACRO(ptuple->dbp_tabid, privs_lost->qeu_obj_id);
		privs_lost->qeu_priv = ptuple->dbp_popset & ~DB_GRANT_OPTION;
		if (privs_lost->qeu_priv & DB_RETRIEVE)
		    privs_lost->qeu_priv &= ~((i4) (DB_TEST | DB_AGGREGATE));
		privs_lost->qeu_flags = QEU_PRIV_LOST;
		privs_lost->qeu_gtype = ptuple->dbp_gtype;
		STRUCT_ASSIGN_MACRO(ptuple->dbp_owner, privs_lost->qeu_authid);
		for (i = 0; i < DB_COL_WORDS; i++)
		{
		    privs_lost->qeu_attrmap[i] = ptuple->dbp_domset[i];
		}

		if (ptuple->dbp_popset & DB_GRANT_OPTION)
		{
		    grant_option_lost = privs_lost + 1;
		    STRUCT_ASSIGN_MACRO((*privs_lost), (*grant_option_lost));

		    /* only GRANT statement allows creation of privileges WGO */
		    grant_option_lost->qeu_flags = 
			QEU_GRANT_OPTION_LOST | QEU_GRANT_COMPATIBLE_ONLY;
		    privs_lost->qeu_flags |= QEU_GRANT_COMPATIBLE_ONLY;

		    /* 
		    ** we dropped a permit that specified a grantable
		    ** privilege - if there are other permits granting that
		    ** privilege to the grantee of the newly dropped permit, we
		    ** need to check whether the aggregate depth of privilege
		    ** conveyed by these permits is greater than that of the
		    ** permit that was dropped.  If so, qeu_depth() will be
		    ** called to determine whether this will require that we
		    ** recompute depths of other permits.
		    ** 
		    ** We will pass to qeu_match_privs() a structure
		    ** (QEU_PERM_DEPTH_INFO) describing the privilege that was
		    ** conveyed by the permit that was dropped along with depth
		    ** of that permit and a slot to place aggregate depth of
		    ** remaining permits (if any) conveying the same privilege
		    ** to the grantee of the permit that was dropped
		    ** 
		    ** A comment in qeu_match_privs() describes the algorithm
		    ** for computing aggregate depth of a privilege based on
		    ** depths of permits conveying that privilege
		    */

		    /* first allocate the structure */
		    ulm.ulm_psize = sizeof(QEU_PERM_DEPTH_INFO);
		    if ((status = qec_malloc(&ulm)) != E_DB_OK)
		    {
			ulm_closestream(&ulm);
			error = E_QE001E_NO_MEM;
			break;
		    }
		    
		    perm_depth_info = (QEU_PERM_DEPTH_INFO *) ulm.ulm_pptr;

		    perm_depth_info->qeu_pdi_flags = 0;
		    perm_depth_info->qeu_old_depth = ptuple->dbp_depth;
		    perm_depth_info->qeu_new_depth = QEU_NEW_DEPTH_INVALID;
		    perm_depth_info->qeu_gtype = ptuple->dbp_gtype;
		    STRUCT_ASSIGN_MACRO(ptuple->dbp_owner,
		    			perm_depth_info->qeu_authid);
		    perm_depth_info->qeu_priv = ptuple->dbp_popset;
		    if (   ptuple->dbp_obtype == DBOB_TABLE
			|| ptuple->dbp_obtype == DBOB_VIEW)
		    {
			for (i = 0; i < DB_COL_WORDS; i++)
			{
			    perm_depth_info->qeu_attrmap[i] =
				ptuple->dbp_domset[i];
			}
		    }
		    
		    /* 
		    ** allocate a QEU_PDI_TBL_DESCR structure and 
		    ** call qeu_crt_pdi_tbl() to, among other things,
		    ** create and open the temp table which will be used to
		    ** keep track of privileges whose depth may change as a
		    ** result of destroying the specified permit
		    */
		    ulm.ulm_psize = sizeof(QEU_PDI_TBL_DESCR);
		    if ((status = qec_malloc(&ulm)) != E_DB_OK)
		    {
			ulm_closestream(&ulm);
			error = E_QE001E_NO_MEM;
			break;
		    }

		    pdi_tbl_descr = (QEU_PDI_TBL_DESCR *) ulm.ulm_pptr;

		    status = qeu_crt_pdi_tbl(qef_cb, pdi_tbl_descr, 
			qeuq_cb->qeuq_db_id, &ptuple->dbp_tabid, &error);
		    if (DB_FAILURE_MACRO(status))
		    {
			break;
		    }
		}
		else
		{
		    /*
		    ** no grantable privilege was lost as a result of
		    ** destruction of the specified permit
		    */
		    grant_option_lost = (QEU_REVOKE_INFO *) NULL;

		    /*
		    ** there are no permits whose depth would be affected by
		    ** destruction of the specified permit
		    */
		    perm_depth_info = (QEU_PERM_DEPTH_INFO *) NULL;

		    /*
		    ** need to tell qeu_match_privs() whether the permit
		    ** was GRANT-compatible
		    */
		    if (qeu_grant_compatible(ptuple,
		        (bool) (   ptuple->dbp_treeid.db_tre_high_time
			        || ptuple->dbp_treeid.db_tre_low_time),
		        (bool) (STskipblank((char *) &ptuple->dbp_term, 
			    	sizeof(DB_TERM_NAME)) != (char *)NULL))
		       )
		    {
			privs_lost->qeu_flags |= QEU_GRANT_COMPATIBLE_ONLY;
		    }
		}

		pqeu.qeu_klen = 2;

		/*
		** save the name of the object owner so we can hand it over
		** to qeu_match_privs() and qeu_depth().  We cannot save the 
		** pointer to the name since ptuple gets passed to 
		** qeu_match_privs() and qeu_depth() and could, conceivably, 
		** get overwritten
		*/
		STRUCT_ASSIGN_MACRO(ptuple->dbp_obown, obj_owner);
		
		status = qeu_match_privs(qeuq_cb, qef_cb, &pqeu, privs_lost,
		    grant_option_lost, perm_depth_info, 1, pdi_tbl_descr,
		    ptuple, &obj_owner, &error);
		if (status != E_DB_OK)
		{
		    break;
		}
	    }

	    if (   privs_lost->qeu_priv 
		|| (grant_option_lost && grant_option_lost->qeu_priv)
	       )
	    {
		QEUQ_CB		rqeuq_cb;

		/* 
		** *privs_lost now describes privileges (if any) lost
		** as the result of destructions of specified permit.
		**
		** *grant_option_lost, if grant_option_lost is not NULL, 
		** now describes privileges which the grantee of the permit 
		** which was dropped will no longer be able to grant.
		**
		** from this point on, processing is very similar to that 
		** involved in revoking a privilege, i.e. we will determine
		** which objects and/or permits will become abandoned
		** as a result of loss of this privilege and, depending
		** on whether cascading destruction was specified, will
		** either destroy abandoned objects/permits or report an
		** error.  
		*/
		STRUCT_ASSIGN_MACRO((*qeuq_cb), rqeuq_cb);
		rqeuq_cb.qeuq_permit_mask = QEU_REVOKE_PRIVILEGES;

		/* 
		** if the caller has requested that we force invalidation of
		** query plans affected by the destruction of specified
		** permit(s), turn off QEU_FORCE_QP_INVALIDATION bit in 
		** rqeuq_cb.qeuq_flag_mask since qeu_priv_revoke() is not 
		** prepared to find it there - instead, if we have not been 
		** able to locate id of some base table whose timestamp could 
		** be altered to effect QP invalidation, we will set 
		** QEU_INVALIDATE_QPS in privs_lost->qeu_flags and 
		** grant_option_lost->qeu_flags
		*/
		if (rqeuq_cb.qeuq_flag_mask & QEU_FORCE_QP_INVALIDATION)
		{
		    rqeuq_cb.qeuq_flag_mask &= ~QEU_FORCE_QP_INVALIDATION;

		    if (!qps_invalidated)
		    {
			if (privs_lost->qeu_priv)
			{
			    privs_lost->qeu_flags |= QEU_INVALIDATE_QPS;
			}

			if (grant_option_lost && grant_option_lost->qeu_priv)
			{
			    grant_option_lost->qeu_flags |= QEU_INVALIDATE_QPS;
			}
		    }
		}

		status = qeu_priv_revoke(&rqeuq_cb, qef_cb, privs_lost, 
		    grant_option_lost, 1, pdi_tbl_descr);
		if (status != E_DB_OK)
		{
		    err_already_reported = TRUE;
		    STRUCT_ASSIGN_MACRO(rqeuq_cb.error, qeuq_cb->error);
		    break;
		}
	    }
	    
	    /*
	    ** now that we can be sure that no permits representing
	    ** abandoned privileges are left lying around, we can recompute
	    ** depth of permits (if any) that were affected by destruction
	    ** of the specified permit
	    */
	    if (pdi_tbl_descr && pdi_tbl_descr->qeu_pdi_num_rows > 0)
	    {
		status = qeu_depth(qef_cb, qeuq_cb, qeuq_cb->qeuq_rtbl, 
		    pdi_tbl_descr, &obj_owner, &error);
	        if (status != E_DB_OK)
	    	    break;
	    }
	}

	/* Check if the permit removed earlier is the last
	** permit or not. If it is the last permit, and table is not dropped
	** then turn off the DMT_C_PERMITS for the table
	*/

	if (dropped_perms)
	{
	status = qeu_close(qef_cb, &pqeu);    
	if (status != E_DB_OK)
	{
	    error = pqeu.error.err_code;
	    break;
	}
	pqeu.qeu_tab_id.db_tab_base  = DM_B_PROTECT_TAB_ID;
	pqeu.qeu_tab_id.db_tab_index  = DM_I_PROTECT_TAB_ID;
	status = qeu_open(qef_cb, &pqeu);
	if (status != E_DB_OK)
	{
	    error = pqeu.error.err_code;
	    break;
	}
	prot_opened = TRUE;

	pqeu.qeu_count = 1;
	pqeu.qeu_tup_length = sizeof(DB_PROTECTION);
	pqeu.qeu_output = &pqef_data;
	pqef_data.dt_next = 0;
	pqef_data.dt_size = sizeof(DB_PROTECTION);
	pqef_data.dt_data = (PTR) ptuple;

	pqeu.qeu_getnext = QEU_REPO;
	pqeu.qeu_klen = 2;       
	pqeu.qeu_key = pkey_ptr_array;
	pkey_ptr_array[0]->attr_number = DM_1_PROTECT_KEY;
	pkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	pkey_ptr_array[0]->attr_value = (char*) 
				&qeuq_cb->qeuq_rtbl->db_tab_base;
	pkey_ptr_array[1]->attr_number = DM_2_PROTECT_KEY;
	pkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	pkey_ptr_array[1]->attr_value = (char*) 
				 &qeuq_cb->qeuq_rtbl->db_tab_index;
	
	pqeu.qeu_qual = 0;
	pqeu.qeu_qarg = 0;
	
	status = qeu_get(qef_cb, &pqeu);
	if (status != E_DB_OK)
	{
	    error = pqeu.error.err_code;
	    if (error == E_QE0015_NO_MORE_ROWS)
	    {
	        /* Change DMT_C_PERMITS */
	        if (!(qeuq_cb->qeuq_permit_mask &
	           (QEU_DBP_PROTECTION | QEU_EV_PROTECTION | QEU_SEQ_PROTECTION)) &&
	           (~qeuq_cb->qeuq_permit_mask & QEU_PERM_OR_SECALM) &&
	           !(permits_exist) && /* No permits/secalms, && */
	           (~qeuq_cb->qeuq_permit_mask & QEU_SECALM))
	        {
	            dmt_cb.dmt_char_array.data_address = (PTR) char_array;
	            dmt_cb.dmt_char_array.data_in_size = 
					sizeof(DMT_CHAR_ENTRY) * 1;
	            char_array[0].char_id = DMT_C_PERMITS;
	            char_array[0].char_value = DMT_C_OFF;

	            dmt_cb.length = sizeof(DMT_CB);
	            dmt_cb.type = DMT_TABLE_CB;
	            dmt_cb.dmt_id.db_tab_base = qeuq_cb->qeuq_rtbl->db_tab_base;
	            dmt_cb.dmt_id.db_tab_index = qeuq_cb->qeuq_rtbl->db_tab_index;
	            dmt_cb.dmt_tran_id = qef_cb->qef_dmt_id;
	            dmt_cb.dmt_flags_mask = 0;
	            dmt_cb.dmt_db_id = qeuq_cb->qeuq_db_id;

	            status = dmf_call(DMT_ALTER, &dmt_cb);
	            if (status != E_DB_OK)
	            {
		        error = dmt_cb.error.err_code;
		        break;
	            }
	        }
	    }
	}
	}

	/* Close off all the tables. */

	status = qeu_close(qef_cb, &qqeu);    
	if (status != E_DB_OK)
	{
	    error = qqeu.error.err_code;
	    break;
	}
	qtext_opened = FALSE;
	status = qeu_close(qef_cb, &pqeu);    
	if (status != E_DB_OK)
	{
	    error = pqeu.error.err_code;
	    break;
	}
	prot_opened = FALSE;
	status = qeu_close(qef_cb, &tqeu);    
	if (status != E_DB_OK)
	{
	    error = tqeu.error.err_code;
	    break;
	}
	tbl_opened = FALSE;

	/*
	** if we have created and opened a temp table to keep track of
	** changes in depth of privileges as a result of destruction of the
	** specified permit, call qeu_drop_pdi_tbl() to close it and drop it
	*/
	if (pdi_tbl_descr)
	{
	    status = qeu_drop_pdi_tbl(qef_cb, pdi_tbl_descr, (bool) FALSE,
		&qeuq_cb->error, &error);
	    if (DB_FAILURE_MACRO(status))
		break;
	}

	/* 
        ** Close the input stream created for the protection tuples.
	**
	** NOTE: we must close and drop the PDI table BEFORE releasing
	** memory since control blocks for it has been allocated out of the
	** memory stream
        */

	ulm_closestream(&ulm);

	error = E_QE0000_OK;
	exit_loop = TRUE;
    } /* end for */

    if (status == E_DB_OK)
    {	
	if (transtarted)
	{
	    status = qeu_etran(qef_cb, &tranqeu);
	    if (status != E_DB_OK)
	    {	
		(VOID) qef_error(tranqeu.error.err_code, 0L, status, &error,
		    &qeuq_cb->error, 0);
                return (status);
	    }
	}
	qeuq_cb->error.err_code = E_QE0000_OK;
	return (E_DB_OK);
    }

    /* Some kind of error occurred. */
    /* call qef_error to handle error messages */

    if (!err_already_reported)
	(VOID) qef_error(error, 0L, status, &error, &qeuq_cb->error, 0);
    
    /* Close off all the tables. */

    local_status = E_DB_OK;

    /* Close off all the tables. */

    if (qtext_opened)
    {
	local_status = qeu_close(qef_cb, &qqeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(qqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (prot_opened)
    {
	local_status = qeu_close(qef_cb, &pqeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(pqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (tbl_opened)
    {
	local_status = qeu_close(qef_cb, &tqeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(tqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }
    if (event_opened)
    {
	local_status = qeu_close(qef_cb, &eqeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(eqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    /*
    ** if we have created and opened a temp table to keep track of
    ** changes in depth of privileges as a result of destruction of the
    ** specified permit, call qeu_drop_pdi_tbl() to close it and drop it
    */
    if (pdi_tbl_descr)
    {
        local_status = qeu_drop_pdi_tbl(qef_cb, pdi_tbl_descr, (bool) TRUE,
	    &qeuq_cb->error, &error);
        if (DB_FAILURE_MACRO(local_status))
	{
	    (VOID) qef_error(error, 0L, local_status, &error, 
		&qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (ulm.ulm_streamid != (PTR)NULL)
    {
	/* 
        ** Close the input stream created for the protection tuples.
	**
	** NOTE: we must try to close and drop the PDI table BEFORE releasing
	** memory since control blocks for it has been allocated out of the
	** memory stream
        */

	(VOID) ulm_closestream(&ulm);
    }
    if (transtarted)
    {
	local_status = qeu_atran(qef_cb, &tranqeu);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(tranqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    return (status);
}

/*{
** Name: QEU_REVOKE	- revoke privilege(s)
**
** External QEF call:   status = qef_call(QEU_REVOKE, &qeuq_cb);
**
** Description:
**	given a grantor, grantee, object id, and a map of privileges to revoke,
**	this function will destroy or alter, as appropriate, existing permit
**	tuples and then will invoke qeu_priv_revoke() to carry out cascading or
**	restricted revocation, as specified by the caller.
**
**	If user explcitly specified a privilege and no permits specifying that
**	privilege by the specified grantor to the specified grantee have been
**	found, caller will be notified
**
**	if user issued REVOKE ALL [PRIVILEGES], we will complain only of no
**	permits satisfied the criteria
**
** Inputs:
**      qef_cb                  qef session control block
**      qeuq_cb
**	    .qeuq_eflag	        designate error handling semantics
**				for user errors.
**		QEF_INTERNAL	return error code.
**		QEF_EXTERNAL	send message to user.
**	    .qeuq_rtbl		DB_TAB_ID of the table/procedure/dbevent.
**          .qeuq_permit_mask   permission mask
**		QEU_REVOKE_PRIVILEGES	must be set
**		QEU_REVOKE_ALL		processing REVOKE ALL [PRIVILEGES]
**		QEU_DBP_PROTECTION	revoking privilege(s) on a dbproc
**		QEU_EV_PROTECTION	revoking privilege(s) on a dbevent
**		QEU_SEQ_PROTECTION	revoking privilege(s) on a sequence
**	    .qeuq_flag_mask     Operation modifier
**		QEU_DROP_CASCADE	REVOKE CASCADE was specified; otherwise
**					will perform RESTRICTED revocation
**	    .qeuq_pro_tup	IIPROTECT tuple describing privileges being
**				revoked
**		.dbp_tabid	id of the table/dbproc/dbevent on which
**				privilege(s) and/or ability to grant
**				privilege(s) is being revoked
**		.dbp_grantor	user revoking privilege(s)
**		.dbp_gtype	type of grantee from whom privilege(s) are being
**				revoked; one of DBGR_USER, DBGR_GROUP,
**				DBGR_APLID, DBGR_PUBLIC
**		.dbp_owner	grantee from whom privilege(s) are being revoked
**		.dbp_popset	privilege(s) being revoked; if DB_GRANT_OPTION
**				is set, only GRANT OPTION FOR privilege(s)
**				specified in dbp_popset is being revoked
**		.dbp_domset	if revoking privilege on a table/view, this
**				describes attributes for which privilkege is
**				being revoked
**		.dbp_obname	object name
**		.dbp_obown	object owner
**		.dbp_obtype	object type
**		
**	    .qeuq_uld_tup	iievent tuple (always set for events). 
**		.dbe_name	name of the event to be unprotected.
**		.dbe_owner	owner of the above event.
**	    .qeuq_uld_tup	iievent tuple (always set for sequences). 
**		.dbs_name	name of the sequence to be unprotected.
**		.dbs_owner	owner of the above sequence.
**	    .qeuq_db_id		database id
**	    .qeuq_d_id		dmf session id
**	    .qeuq_v2b_col_xlate	address of function that will translate a map of
**				attributes of an updateable view into a map of
**				attributes of a specified underlying table or
**				view
**	    .qeuq_b2v_col_xlate	address of function that will translate a map of
**				a table or view X into a map of attributes of an
**				updateable view which is defined over X
**	
** Outputs:
**      qeuq_cb
**	    .error.err_code	one of the following
**				E_QE0002_INTERNAL_ERROR
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0022_ABORTED
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	03-aug-92 (andre)
**          written
**	16-aug-92 (andre)
**	    use functions provided by the caller inside
**	    qeuq_cb->qeuq_v2b_col_xlate and qeuq_cb->qeuq_b2v_col_xlate to
**	    correctly update independent privilege descriptors
**	17-aug-92 (andre)
**	    if privilege is being revoked on a subset of attributes to which a
**	    permit applies and the original permit depended on some privilege
**	    descriptor, we may not be able to update that privilege descriptor
**      24-aug-92 (andre)
**	    qeu_depth() must be invoked to recompute (if necessary)
**	    depth of permits if we are processing a permit specifying a
**	    grantable privilege and one of the following takes place:
**	      - we are removing some attributes from a map associated
**		with a permit specifying GRANT OPTION FOR a table privilege or
**	      - we are clearing DB_GRANT_OPTION bit in dbp_popset of the permit
**	03-feb-93 (andre)
**	    if revoking privilege(s) on a table, view, or database procedure and
**	    some permit(s) had to be destroyed or modified, we need to
**	    invalidate permit trees (for tables and views only) and the table or
**	    database procedure descriptors in the RDF cache
**	23-apr-93 (anitap)
**	    Fixed compiler warning.
**	10-jun-93 (andre)
**	    added a new argument (object owner name) to interfaces of
**	    qeu_match_privs() and qeu_depth() to correctly handle cases when 
**	    privileges were revoked from the object's owner
**	09-jul-93 (andre)
**	    explicitly cast priv_tuple->db_descr_no to (i4) to keep ACC happy
**	23-jul-93 (andre)
**	    qeu_depth() will no longer be invoked whenever we drop or alter a 
**	    permit; instead, it will be invoked after we call qeu_match_privs()
**	    to recompute depths of permits affected by changes in the aggregate
**	    depth of privileges which were conveyed by permits that were dropped
**	    or altered
**	25-jul-93 (andre)
**	    added code to allocate a QEU_PDI_TBL_DESCR structure and call
**	    qeu_crt_pdi_tbl() to initialize the structure and create and
**	    open the temp table which will be used to keep track of
**	    privileges whose depths have changed as a result of destruction 
**	    of specified privilege(s)
**
**	    added code to invoke qeu_drop_pdi_tbl() to handle closing and
**	    destruction of the temp table
**	15-sep-93 (andre)
**	    if revoking privilege(s) on a table, view, or database procedure,
**	    we need to force invalidation of QPs which may depend on 
**	    privilege(s) being revoked.  Here's how it will work:
**		if (revoking privilege(s) on a base table (except if it is 
**	              a core catalog, which I do not expect to happen very 
**		      often))
**		{
**		    we will alter the base table's timestamp 
**		}
**		else if (revoking privilege(s) on a view for which id of an 
**		 	 underlying base table was determined at view creation 
**			 time) 
**		{
**		    we will alter the timestamp of the view's UBT
**		}
**		else if (revoking privilege(s) on a dbproc which is not marked 
**		         abandoned for which id of an underlying base table was
**			 determined at [re]creation time)
**		{
**		    we will alter the timestamp of the dbproc's UBT
**		}
**		else if (qeu_match_privs() determines that some privileges 
**			 will be lost to the specified grantee)
**		{
**		    set QEU_FORCE_QP_INVALIDATION in qeuq_flag_mask when 
**		    calling qeu_priv_revoke() which will do its best to 
**		    invalidate QPs affected by loss of privileges, destruction 
**		    of objects, or having dbprocs downgraded to dormant
**		}
**	02-nov-93 (andre)
**	    if a user atempting to revoke a privilege on another user's object
**	    him/herself possesses no privileges on that object, instead of 
**	    returning E_QE0253_ALL_PRIV_NONE_REVOKED or 
**	    E_QE0254_SOME_PRIV_NOT_REVOKED, we will return a message that tells
**	    a user that either the object did not exist or he lacked requried 
**	    privileges on it.  This is more in line with SQL92 specification 
** 	    which treats the latter as Access Rule violation, but the former - 
**	    as completion condition (i.e. warning.)
**
**	    We are doing it only for cases when a user is attempting to revoke 
**	    a privilege on an object owned by someone else, because object's 
**	    owner has implicit privileges on it (in the case of ungrantable 
**	    dbprocs, the point gets somewhat moot, I must admit)
**	29-oct-93 (robf)
**           Add COPY_INTO/COPY_FROM privileges. 
**	4-nov-93 (robf)
**           Update auditing to stop duplication from qeu_dprot()
**	16-nov-93 (andre)
**	    do not try to alter timestamps of catalogs to force QEP invalidation
**	    because this could result in a situation where we try to update a 
**	    catalog which we have already opened in excl. mode (which is done 
**	    as a part of altering a timestamp.)  As far as I can tell, this 
**	    MUST be done for IIPROTECT, IIRULE, IIPROCEDURE, IICOMMENT, and 
**	    IISYNONYM, but I prefer to play it safe and avoid doing it for all 
**	    catalogs, thus reducing the chance that I might miss something + 
**	    making future maintenence less painful.
**	24-nov-93 (andre)
**	    qeu_v_ubt_id() will take address of DB_ERROR and will report its own
**	    errors
**	25-nov-93 (stephenb)
**	    Tidy up audititng so that we only audit REVOKE ALL once
**	07-mar-94 (andre)
**	    fix for bug 58075:
**	    in addition to forcing invalidation of dbproc QPs when the status 
**	    of a dbproc is changed, we will also invoke qeu_destr_dbp_qp() to 
**	    actually destroy dbproc's QP if one exists in QSF
**	14-may-02 (inkdo01)
**	    Added support for sequence privileges.
*/
DB_STATUS
qeu_revoke(
QEF_CB          *qef_cb,
QEUQ_CB	    	*qeuq_cb)
{
    GLOBALREF QEF_S_CB *Qef_s_cb;
    DB_PROTECTION   *prot_tuple, *privs_to_revoke;
    RDF_CB	    rdfcb, *rdf_inv_cb = &rdfcb;
    RDR_RB	    *rdf_inv_rb = &rdf_inv_cb->rdf_rb;
    QEU_REVOKE_INFO *revoke_info, *info_p;
    DB_IIXDBDEPENDS *xdep_tuple;
    DB_IIDBDEPENDS  *dep_tuple;
    DB_IIDBDEPENDS  *dep2_tuple;
    DB_IIPRIV	    *priv_tuple;
    QEU_PERM_DEPTH_INFO	*perm_depth_info, *depth_info_p;
    bool	    revoke_grant_option;
    bool	    check_priv_descr;
    bool	    perm_altered = FALSE;
    bool	    cant_access;
    DB_STATUS	    status, local_status;
    DB_ERROR	    e_error;
    i4	    error;
    bool	    transtarted = FALSE;
    bool	    inv_obj_cache = FALSE;
    bool	    inv_tree_cache = FALSE;
    bool            inv_perm_trees = FALSE;
    bool	    audit_revall = FALSE;
    i4		    i;
    i4		    num_revoke_info_recs;
    i4		    num_depth_info_recs;
    i4		    xdep_qmode = DB_PROT;
    i2		    xdep_permno;
    i2		    priv_descrno;
    QEU_CB	    tranqeu;
    QEU_CB	    protqeu, *prot_qeu = (QEU_CB *) NULL;
    QEU_CB	    qtxtqeu, *qtxt_qeu = (QEU_CB *) NULL;
    QEU_CB	    xdepqeu, *xdep_qeu = (QEU_CB *) NULL;
    QEU_CB	    depqeu,  *dep_qeu  = (QEU_CB *) NULL;
    QEU_CB          dep2qeu, *dep2_qeu = (QEU_CB *) NULL;
    QEU_CB	    privqeu, *priv_qeu = (QEU_CB *) NULL;
    DMT_CB	    dmt_cb;
    ULM_RCB         ulm;
    QEF_DATA	    prot_qefdata;
    QEF_DATA	    xdep_qefdata;
    QEF_DATA	    dep_qefdata;
    QEF_DATA	    dep2_qefdata;
    QEF_DATA	    priv_qefdata;
    DMT_CHAR_ENTRY  char_array[4];
    DMR_ATTR_ENTRY  prot_key_array[2];
    DMR_ATTR_ENTRY  *protkey_ptr_array[2];
    DMR_ATTR_ENTRY  xdep_key_array[4];
    DMR_ATTR_ENTRY  *xdepkey_ptr_array[4];
    DMR_ATTR_ENTRY  dep2_key_array[4];
    DMR_ATTR_ENTRY  *dep2key_ptr_array[4];
    DMR_ATTR_ENTRY  priv_key_array[3];
    DMR_ATTR_ENTRY  *privkey_ptr_array[3];
    i4		    attrmap[DB_COL_WORDS];
    i4		    *map_from, *map_to;
    DMT_SHW_CB	    dmt_shw_cb;
    DMT_TBL_ENTRY   table;
    bool	    err_already_reported = FALSE;
    i4         msgid;
    i4              type;    
    DB_PROCEDURE    *dbptuple;
    DB_TAB_ID	    ubt_id;
    char	    *sec_object;
    i4	    	    sec_obj_len;
    DB_OWN_NAME	    *sec_owner;
    QEU_PDI_TBL_DESCR   *pdi_tbl_descr = (QEU_PDI_TBL_DESCR *) NULL;
    i4		    exit_loop, exit_loop2;
    bool	    doaudit=TRUE;
    
    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
    ulm.ulm_streamid = (PTR)NULL;

    for (exit_loop = FALSE; !exit_loop; )
    {
	/* Check the control block. */

	error = E_QE0017_BAD_CB;
	status = E_DB_ERROR;
	if (qeuq_cb->qeuq_type != QEUQCB_CB ||
            qeuq_cb->qeuq_length != sizeof(QEUQ_CB))	
	    break;

	error = E_QE0018_BAD_PARAM_IN_CB;
	if (   (qeuq_cb->qeuq_rtbl == 0)
	    || (qeuq_cb->qeuq_db_id == 0)
	    || (~qeuq_cb->qeuq_permit_mask & QEU_REVOKE_PRIVILEGES)
	    || (   qeuq_cb->qeuq_permit_mask & QEU_DBP_PROTECTION
		&& qeuq_cb->qeuq_permit_mask & QEU_EV_PROTECTION
		&& qeuq_cb->qeuq_permit_mask & QEU_SEQ_PROTECTION
	       )
	    || (qeuq_cb->qeuq_permit_mask &
		    ~(QEU_REVOKE_PRIVILEGES | QEU_REVOKE_ALL |
		      QEU_DBP_PROTECTION | QEU_EV_PROTECTION |
		      QEU_SEQ_PROTECTION)
	       )
	    || (qeuq_cb->qeuq_flag_mask & ~QEU_DROP_CASCADE)
	    || (   !qeuq_cb->qeuq_pro_tup
	        || qeuq_cb->qeuq_pro_tup->dt_size != sizeof(DB_PROTECTION)
		|| qeuq_cb->qeuq_pro_tup->dt_data == NULL
	       )
	    || ((  qeuq_cb->qeuq_permit_mask & QEU_EV_PROTECTION ||
		   qeuq_cb->qeuq_permit_mask & QEU_SEQ_PROTECTION)
	        && qeuq_cb->qeuq_uld_tup == NULL
	       )
            || (qeuq_cb->qeuq_d_id == 0))
	{
	    break;
	}

	/*
	** verify that DB_PROTECTION structure passed by the caller is
	** consistent
	*/

	privs_to_revoke = (DB_PROTECTION *) qeuq_cb->qeuq_pro_tup->dt_data;
	if (   !(   privs_to_revoke->dbp_obtype == DBOB_DBPROC
		 && qeuq_cb->qeuq_permit_mask & QEU_DBP_PROTECTION
		 && privs_to_revoke->dbp_popset & DB_EXECUTE
		 && !(privs_to_revoke->dbp_popset &
		          ~(i4) (DB_EXECUTE | DB_GRANT_OPTION)
		     )
	        )
	    && !(   privs_to_revoke->dbp_obtype == DBOB_EVENT
		 && qeuq_cb->qeuq_permit_mask & QEU_EV_PROTECTION
		 && privs_to_revoke->dbp_popset & (DB_EVRAISE | DB_EVREGISTER)
		 && !(privs_to_revoke->dbp_popset &
			  ~(i4) (DB_EVRAISE | DB_EVREGISTER | DB_GRANT_OPTION)
		  )
	        )
	    && !(   privs_to_revoke->dbp_obtype == DBOB_SEQUENCE
		 && qeuq_cb->qeuq_permit_mask & QEU_SEQ_PROTECTION
		 && privs_to_revoke->dbp_popset & DB_NEXT
		 && !(privs_to_revoke->dbp_popset &
			  ~(i4) (DB_NEXT | DB_GRANT_OPTION)
		  )
	        )
	    && !(   (   privs_to_revoke->dbp_obtype == DBOB_TABLE
		     || privs_to_revoke->dbp_obtype == DBOB_VIEW)
		 && !(qeuq_cb->qeuq_permit_mask &
		          (QEU_DBP_PROTECTION | QEU_EV_PROTECTION |
			   QEU_SEQ_PROTECTION))
		 && privs_to_revoke->dbp_popset &
		        (DB_RETRIEVE | DB_REPLACE | DB_APPEND | DB_DELETE |
			 DB_REFERENCES | DB_COPY_INTO | DB_COPY_FROM)
		 && !(privs_to_revoke->dbp_popset &
		          ~(i4) (DB_RETRIEVE | DB_TEST | DB_AGGREGATE |
			         DB_REPLACE | DB_APPEND | DB_DELETE |
			         DB_COPY_INTO | DB_COPY_FROM  |
				 DB_REFERENCES | DB_GRANT_OPTION)
		     )
		)
	   )
	{
	    break;
	}

	/*
	** remember whether we need to check whether a user attempting 
	** privilege revocation himself possesses any privileges on the object
	** if the check needs to be performed, we will set cant_access to TRUE
	** and hope to find an IIPROTECT tuple which would allow us to reset it
	** to FALSE; otherwise it will be initialized to FALSE 
	*/
	if (MEcmp((PTR) &privs_to_revoke->dbp_grantor, 
		(PTR) &privs_to_revoke->dbp_obown,
		sizeof(privs_to_revoke->dbp_grantor)))
	{
	    /*
	    ** until proven otherwise, we will assume that the user attempting
	    ** to revoke privilege(s) on another user's object himself possesses
	    ** no privilege on that object; if we find an IIPROTECT tuple
	    ** granting him (or PUBLIC) some privilege on the object, we will
	    ** reset cant_access to FALSE
	    */
	    cant_access = TRUE;
	}
	else
	{
	    /* owner of the object possesses implicit privileges on it */
	    cant_access = FALSE;
	}

	status = E_DB_OK;

	/* 
	** Check to see if a transaction is in progress,
	** if so, set transtarted flag to FALSE, otherwise
	** set it to TRUE after beginning a transaction.
	** If we started a transaction them we will abort
        ** it if an error occurs, otherwise we will just
        ** return the error and let the caller abort it.
	*/

	if (qef_cb->qef_stat == QEF_NOTRAN)
	{
	    tranqeu.qeu_type = QEUCB_CB;
	    tranqeu.qeu_length = sizeof(QEUCB_CB);
	    tranqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
	    tranqeu.qeu_d_id = qeuq_cb->qeuq_d_id;
	    tranqeu.qeu_flag = 0;
	    tranqeu.qeu_mask = 0;
	    status = qeu_btran(qef_cb, &tranqeu);
	    if (status != E_DB_OK)
	    {	
		error = tranqeu.error.err_code;
		break;	
	    }	    
	    transtarted = TRUE;
	}
	/* escalate the transaction to MST */
	if (qef_cb->qef_auto == QEF_OFF)
	    qef_cb->qef_stat = QEF_MSTRAN;

	/*
	** remember whether we may need to look for and, if found, modify
	** privilege descriptors on which permits affected by REVOKE depend;
	** if it becomes necessary to update a map of attributes in the
	** privilege descriptor on which a permit affected by revocation of
	** privileges depends, we will need to open IIDBDEPENDS for reading by
	** key
	**
	** To qualify, we must be revoking column-specific UPDATE or INSERT (in
	** preparation for adding support for column-specific INSERT) privilege
	** on a view which is owned by the grantor of privileges and which is
	** not marked as "always grantable"
	*/
	if (   privs_to_revoke->dbp_obtype == DBOB_VIEW
	    && privs_to_revoke->dbp_popset & (DB_REPLACE | DB_APPEND)
	    && !MEcmp((PTR) &privs_to_revoke->dbp_grantor,
		      (PTR) &privs_to_revoke->dbp_obown,
		      sizeof(privs_to_revoke->dbp_obown))
	    && BTcount((char *) privs_to_revoke->dbp_domset, DB_MAX_COLS + 1) !=
		DB_MAX_COLS + 1
	   )
	{
	    dmt_shw_cb.length = sizeof(DMT_SHW_CB);
	    dmt_shw_cb.type = DMT_SH_CB;
	    dmt_shw_cb.dmt_db_id = qeuq_cb->qeuq_db_id;
	    dmt_shw_cb.dmt_session_id = qeuq_cb->qeuq_d_id;
	    dmt_shw_cb.dmt_flags_mask = DMT_M_TABLE;
	    dmt_shw_cb.dmt_table.data_address = (char *) &table;
	    dmt_shw_cb.dmt_table.data_in_size = sizeof(DMT_TBL_ENTRY);
	    dmt_shw_cb.dmt_char_array.data_address = (PTR) NULL;
	    dmt_shw_cb.dmt_char_array.data_in_size = 0;
	    dmt_shw_cb.dmt_char_array.data_out_size  = 0;
	    STRUCT_ASSIGN_MACRO((*qeuq_cb->qeuq_rtbl), dmt_shw_cb.dmt_tab_id);

	    /* issue DMT_SHOW call */

	    if ((status = dmf_call(DMT_SHOW, &dmt_shw_cb)) != E_DB_OK)
	    {
		error = dmt_shw_cb.error.err_code;
		break;
	    }

	    check_priv_descr = (table.tbl_status_mask & DMT_VGRANT_OK) ? FALSE
								       : TRUE;
	}
	else
	{
	    check_priv_descr = FALSE;
	}

	/* Get a memory stream for reading catalog tuples. */
        for (exit_loop2 = FALSE; !exit_loop2; )            /* to break out of */
	{
	    /*
	    ** allocate space for two IIPROTECT tuples: 1 for a copy of a tuple
	    ** passed in by the user (we may decide to make changes to it, and
	    ** it wouldn't be nice to mess around with caller's copy), and 1 for
	    ** reading IIPROTECT tuples 
	    */
	    /* Open stream and allocate memory with one effort */
	    ulm.ulm_flags = ULM_PRIVATE_STREAM | ULM_OPEN_AND_PALLOC;
	    ulm.ulm_psize = sizeof(DB_PROTECTION) * 2;

	    if ((status = qec_mopen(&ulm)) != E_DB_OK)
		break;
	    
	    prot_tuple = (DB_PROTECTION*) ulm.ulm_pptr;

	    /*
	    ** caller passed in an IIPROTECT tuple describing privileges being
	    ** revoked - copy it and remember whether a privilege or just GRANT
	    ** OPTION FOR privilege is being revoked
	    */
	    STRUCT_ASSIGN_MACRO((*privs_to_revoke), prot_tuple[1]);
	    privs_to_revoke = prot_tuple + 1;
	    if (revoke_grant_option =
		    ((privs_to_revoke->dbp_popset & DB_GRANT_OPTION) != 0))
	    {
		privs_to_revoke->dbp_popset &= ~DB_GRANT_OPTION;
	    }

	    /*
	    ** if one of privileges being revoked is SELECT, turn off DB_TEST
	    ** and DB_AGGREGATE bits since they are adequately represented by
	    ** DB_RETRIEVE bit
	    */
	    if (privs_to_revoke->dbp_popset & DB_RETRIEVE)
		privs_to_revoke->dbp_popset &= ~((i4) (DB_TEST | DB_AGGREGATE));

	    /*
	    ** we need to allocate an array of QEU_REVOKE_INFO structures to
	    ** keep track of privileges which were actually revoked; if revoking
	    ** only grant option for privileges, allocate one QEU_REVOKE_INFO
	    ** structure per privilege, if revoking privileges, allocate two
	    ** QEU_REVOKE_INFO structures per privilege - one to keep track of
	    ** privileges which were lost and the other of GRANT OPTION FOR
	    ** privileges which were lost
	    */

	    num_revoke_info_recs = 
		BTcount((char *) &privs_to_revoke->dbp_popset,
		    BITS_IN(privs_to_revoke->dbp_popset));

	    if (!revoke_grant_option)
	        num_revoke_info_recs <<= 1;

	    ulm.ulm_psize = num_revoke_info_recs * sizeof(QEU_REVOKE_INFO);

	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    revoke_info = (QEU_REVOKE_INFO *) ulm.ulm_pptr;

	    ulm.ulm_psize = sizeof(DB_IIXDBDEPENDS);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    xdep_tuple = (DB_IIXDBDEPENDS *) ulm.ulm_pptr;

	    /*
	    ** if we may need to alter independent privilege descriptor on which
	    ** a permit depends, allocate memory for 2 IIDBDEPENDS tuples - one
	    ** will be used when we read or update IIDBDEPENDS by TID and the
	    ** other - when we read it by key; 
	    ** otherwise allocate just one tuple which will be used to read
	    ** IIDBDEPENDS by TID
	    */
	    ulm.ulm_psize = (check_priv_descr)	? sizeof(DB_IIDBDEPENDS) * 2
						: sizeof(DB_IIDBDEPENDS);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    dep_tuple = (DB_IIDBDEPENDS *) ulm.ulm_pptr;
	    if (check_priv_descr)
		dep2_tuple = dep_tuple + 1;

	    ulm.ulm_psize = sizeof(DB_IIPRIV);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    priv_tuple = (DB_IIPRIV *) ulm.ulm_pptr;

	    /*
	    ** rather than invoking qeu_depth() every time we drop or
	    ** alter a permit, we will accumulate descriptions of
	    ** dropped/altered permits using array of QEU_REVOKE_INFO
	    ** structures, call qeu_match_privs() to determine which,
	    ** if any, of the privileges conveyed by dropped/altered permits 
	    ** have become abandoned + to determine the new aggregate depth of 
	    ** privileges to the grantees of dropped/altered permits whose 
	    ** (privileges' that is) depth was affected by the revocation
	    **
	    ** AFTER we processed all permits and are sure that there are
	    ** no more permits representing abandoned privileges 
	    ** (qeu_priv_revoke() will take care of that), we will invoke 
	    ** qeu_depth() to recompute depth of permits; this requires that 
	    ** instead of allocating a single QEU_PERM_DEPTH_INFO structure, we 
	    ** allocate as many QEU_PERM_DEPTH_INFO structures as we have
	    ** allocated QEU_REVOKE_INFO strucures to store descriptions of
	    ** dropped/altered grantable privileges
	    */
	    num_depth_info_recs = 
		(revoke_grant_option) ? num_revoke_info_recs
				      : num_revoke_info_recs >> 1;
    
	    ulm.ulm_psize = sizeof(QEU_PERM_DEPTH_INFO) * num_depth_info_recs;
    
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    perm_depth_info = (QEU_PERM_DEPTH_INFO *) ulm.ulm_pptr;
    
	    exit_loop2 = TRUE;
	}
	if (status != E_DB_OK)
	{
	    error = E_QE001E_NO_MEM;
	    break;
	}

	error = E_QE0000_OK;
    
	/*
	** Open iiprotect, iiqrytext, iixdbdepends, iidbdepends (twice if it may
	** need to be updated), and iipriv system catalogs
	*/

	protqeu.qeu_type = QEUCB_CB;
        protqeu.qeu_length = sizeof(QEUCB_CB);
        protqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
        protqeu.qeu_lk_mode = DMT_IX;
        protqeu.qeu_flag = DMT_U_DIRECT;
        protqeu.qeu_access_mode = DMT_A_WRITE;
	protqeu.qeu_mask = 0;
	protqeu.qeu_qual = protqeu.qeu_f_qual = 0;
	protqeu.qeu_qarg = protqeu.qeu_f_qarg = 0;

	STRUCT_ASSIGN_MACRO(protqeu, qtxtqeu);
	STRUCT_ASSIGN_MACRO(protqeu, xdepqeu);
	STRUCT_ASSIGN_MACRO(protqeu, depqeu);
	if (check_priv_descr)
	    STRUCT_ASSIGN_MACRO(protqeu, dep2qeu);
	STRUCT_ASSIGN_MACRO(protqeu, privqeu);

        protqeu.qeu_tab_id.db_tab_base  = DM_B_PROTECT_TAB_ID;
        protqeu.qeu_tab_id.db_tab_index  = DM_I_PROTECT_TAB_ID;
	status = qeu_open(qef_cb, &protqeu);
	if (status != E_DB_OK)
	{
	    error = protqeu.error.err_code;
	    break;
	}
	prot_qeu = &protqeu;

	qtxtqeu.qeu_tab_id.db_tab_base  = DM_B_QRYTEXT_TAB_ID;
	qtxtqeu.qeu_tab_id.db_tab_index = DM_I_QRYTEXT_TAB_ID;
	status = qeu_open(qef_cb, &qtxtqeu);
	if (status != E_DB_OK)
	{
	    error = qtxtqeu.error.err_code;
	    break;
	}
	qtxt_qeu = &qtxtqeu;

	xdepqeu.qeu_tab_id.db_tab_base  = DM_B_XDEPENDS_TAB_ID;
	xdepqeu.qeu_tab_id.db_tab_index = DM_I_XDEPENDS_TAB_ID;
	status = qeu_open(qef_cb, &xdepqeu);
	if (status != E_DB_OK)
	{
	    error = xdepqeu.error.err_code;
	    break;
	}
	xdep_qeu = &xdepqeu;

	depqeu.qeu_tab_id.db_tab_base  = DM_B_DEPENDS_TAB_ID;
	depqeu.qeu_tab_id.db_tab_index = DM_I_DEPENDS_TAB_ID;
	status = qeu_open(qef_cb, &depqeu);
	if (status != E_DB_OK)
	{
	    error = depqeu.error.err_code;
	    break;
	}
	dep_qeu = &depqeu;

	if (check_priv_descr)
	{
	    /*
	    ** only open IIDBDEPENDS for the second time if we may need to
	    ** update it
	    */
	    dep2qeu.qeu_tab_id.db_tab_base  = DM_B_DEPENDS_TAB_ID;
	    dep2qeu.qeu_tab_id.db_tab_index = DM_I_DEPENDS_TAB_ID;
	    status = qeu_open(qef_cb, &dep2qeu);
	    if (status != E_DB_OK)
	    {
		error = dep2qeu.error.err_code;
		break;
	    }
	    dep2_qeu = &dep2qeu;
	}

	privqeu.qeu_tab_id.db_tab_base  = DM_B_PRIV_TAB_ID;
	privqeu.qeu_tab_id.db_tab_index = DM_I_PRIV_TAB_ID;
	status = qeu_open(qef_cb, &privqeu);
	if (status != E_DB_OK)
	{
	    error = privqeu.error.err_code;
	    break;
	}
	priv_qeu = &privqeu;

        prot_qeu->qeu_tup_length = sizeof(DB_PROTECTION);
	/*
	** we will be reading IIPROTECT and occasionally deleting, updating, or
	** inserting tuples
	*/
	prot_qeu->qeu_output = prot_qeu->qeu_input = &prot_qefdata;
	prot_qefdata.dt_next = 0;
        prot_qefdata.dt_size = sizeof(DB_PROTECTION);
        prot_qefdata.dt_data = (PTR) prot_tuple;

	xdep_qeu->qeu_tup_length = sizeof(DB_IIXDBDEPENDS);
	xdep_qeu->qeu_output = &xdep_qefdata;
	xdep_qefdata.dt_next = 0;
	xdep_qefdata.dt_size = sizeof(DB_IIXDBDEPENDS);
	xdep_qefdata.dt_data = (PTR) xdep_tuple;
	
	dep_qeu->qeu_count = 1;
	dep_qeu->qeu_tup_length = sizeof(DB_IIDBDEPENDS);
	dep_qeu->qeu_output = &dep_qefdata;
	if (check_priv_descr)
	    dep_qeu->qeu_input = &dep_qefdata;
	dep_qefdata.dt_next = 0;
	dep_qefdata.dt_size = sizeof(DB_IIDBDEPENDS);
	dep_qefdata.dt_data = (PTR) dep_tuple;

	if (check_priv_descr)
	{
	    dep2_qeu->qeu_count = 1;
	    dep2_qeu->qeu_tup_length = sizeof(DB_IIDBDEPENDS);
	    dep2_qeu->qeu_output = &dep2_qefdata;
	    dep2_qefdata.dt_next = 0;
	    dep2_qefdata.dt_size = sizeof(DB_IIDBDEPENDS);
	    dep2_qefdata.dt_data = (PTR) dep2_tuple;
	}

	priv_qeu->qeu_tup_length = sizeof(DB_IIPRIV);
	priv_qeu->qeu_output = priv_qeu->qeu_input = &priv_qefdata;
	priv_qefdata.dt_next = 0;
	priv_qefdata.dt_size = sizeof(DB_IIPRIV);
	priv_qefdata.dt_data = (PTR) priv_tuple;
	
	prot_qeu->qeu_key = protkey_ptr_array;
	
	protkey_ptr_array[0] = prot_key_array;
	protkey_ptr_array[0]->attr_number = DM_1_PROTECT_KEY;
	protkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	protkey_ptr_array[0]->attr_value =
	    (char *) &qeuq_cb->qeuq_rtbl->db_tab_base;

	protkey_ptr_array[1] = prot_key_array + 1;
	protkey_ptr_array[1]->attr_number = DM_2_PROTECT_KEY;
	protkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	protkey_ptr_array[1]->attr_value =
	    (char *) &qeuq_cb->qeuq_rtbl->db_tab_index;

	xdep_qeu->qeu_key = xdepkey_ptr_array;
	xdep_qeu->qeu_getnext = QEU_REPO;
	xdep_qeu->qeu_klen = 4;

	xdepkey_ptr_array[0] = xdep_key_array;
	xdepkey_ptr_array[0]->attr_number = DM_1_XDEPENDS_KEY;
	xdepkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	xdepkey_ptr_array[0]->attr_value =
	    (char *) &qeuq_cb->qeuq_rtbl->db_tab_base;

	xdepkey_ptr_array[1] = xdep_key_array + 1;
	xdepkey_ptr_array[1]->attr_number = DM_2_XDEPENDS_KEY;
	xdepkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	xdepkey_ptr_array[1]->attr_value =
	    (char *) &qeuq_cb->qeuq_rtbl->db_tab_index;

	xdepkey_ptr_array[2] = xdep_key_array + 2;
	xdepkey_ptr_array[2]->attr_number = DM_3_XDEPENDS_KEY;
	xdepkey_ptr_array[2]->attr_operator = DMR_OP_EQ;
	xdepkey_ptr_array[2]->attr_value = (char *) &xdep_qmode;

	xdepkey_ptr_array[3] = xdep_key_array + 3;
	xdepkey_ptr_array[3]->attr_number = DM_4_XDEPENDS_KEY;
	xdepkey_ptr_array[3]->attr_operator = DMR_OP_EQ;
	xdepkey_ptr_array[3]->attr_value = (char *) &xdep_permno;

	dep_qeu->qeu_key = (DMR_ATTR_ENTRY **) NULL;
	dep_qeu->qeu_flag = QEU_BY_TID;
	dep_qeu->qeu_getnext = QEU_NOREPO;
	dep_qeu->qeu_klen = 0;

	/* initialize the key structure only if we may need to use it */
	if (check_priv_descr)
	{
	    dep2_qeu->qeu_key = dep2key_ptr_array;

	    /*
	    ** this QEU_CB will be used to look up IIDBDEPENDS tuples that
	    ** describe dependence on the same privilege descriptor as the one
	    ** that was located by a TID join from IIXDBDEPENDS
	    */
	    dep2key_ptr_array[0] = dep2_key_array;
	    dep2key_ptr_array[0]->attr_number = DM_1_DEPENDS_KEY;
	    dep2key_ptr_array[0]->attr_operator = DMR_OP_EQ;
	    dep2key_ptr_array[0]->attr_value =
		(char *) &dep_tuple->dbv_indep.db_tab_base;

	    dep2key_ptr_array[1] = dep2_key_array + 1;
	    dep2key_ptr_array[1]->attr_number = DM_2_DEPENDS_KEY;
	    dep2key_ptr_array[1]->attr_operator = DMR_OP_EQ;
	    dep2key_ptr_array[1]->attr_value =
		(char *) &dep_tuple->dbv_indep.db_tab_index;

	    dep2key_ptr_array[2] = dep2_key_array + 2;
	    dep2key_ptr_array[2]->attr_number = DM_3_DEPENDS_KEY;
	    dep2key_ptr_array[2]->attr_operator = DMR_OP_EQ;
	    dep2key_ptr_array[2]->attr_value = (char *) &dep_tuple->dbv_itype;

	    dep2key_ptr_array[3] = dep2_key_array + 3;
	    dep2key_ptr_array[3]->attr_number = DM_4_DEPENDS_KEY;
	    dep2key_ptr_array[3]->attr_operator = DMR_OP_EQ;
	    dep2key_ptr_array[3]->attr_value = (char *) &dep_tuple->dbv_i_qid;
	}
	
	priv_qeu->qeu_key = privkey_ptr_array;

	privkey_ptr_array[0] = priv_key_array;
	privkey_ptr_array[0]->attr_number = DM_1_PRIV_KEY;
	privkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	privkey_ptr_array[0]->attr_value =
	    (char *) &qeuq_cb->qeuq_rtbl->db_tab_base;

	privkey_ptr_array[1] = priv_key_array + 1;
	privkey_ptr_array[1]->attr_number = DM_2_PRIV_KEY;
	privkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	privkey_ptr_array[1]->attr_value =
	    (char *) &qeuq_cb->qeuq_rtbl->db_tab_index;

	privkey_ptr_array[2] = priv_key_array + 2;
	privkey_ptr_array[2]->attr_number = DM_3_PRIV_KEY;
	privkey_ptr_array[2]->attr_operator = DMR_OP_EQ;
	privkey_ptr_array[2]->attr_value = (char *) &priv_descrno;
	
	{
	    /*
	    ** Initialize the array of records of revoked privileges
	    */
	    i2		grant_compat_flag;
	    i4		p;

	    /*
	    ** if privilege(s) are being revoked by the object's owner on his
	    ** table or view, it will affect GRANT-compatible as well as
	    ** non-GRANT-compatible permits; otherwise only GRANT-compatible
	    ** permits will be affected (non-GRANT-compatible permit can be
	    ** granted only on tables and view and only by the object's owner)
	    */
	    if (   (   privs_to_revoke->dbp_obtype == DBOB_TABLE
		    || privs_to_revoke->dbp_obtype == DBOB_VIEW)
		&& !MEcmp((PTR) &privs_to_revoke->dbp_grantor,
			  (PTR) &privs_to_revoke->dbp_obown,
			  sizeof(privs_to_revoke->dbp_obown))
	       )
	    {
		grant_compat_flag = 0;
	    }
	    else
	    {
		grant_compat_flag = QEU_GRANT_COMPATIBLE_ONLY;
	    }

	    if (!revoke_grant_option)
	    {
		QEU_REVOKE_INFO		*info_p2;

		for (i = -1, info_p = revoke_info, 
		     info_p2 = revoke_info + num_revoke_info_recs / 2;
		     (i = BTnext(i, (char *) &privs_to_revoke->dbp_popset,
			    BITS_IN(privs_to_revoke->dbp_popset))) != -1;
		     info_p++, info_p2++
		    )
		{
		    STRUCT_ASSIGN_MACRO(privs_to_revoke->dbp_tabid,
					info_p->qeu_obj_id);
		    info_p->qeu_priv = 0;
		    BTset(i, (char *) &info_p->qeu_priv);
		    info_p->qeu_flags = QEU_PRIV_LOST | grant_compat_flag;
		    info_p->qeu_gtype = privs_to_revoke->dbp_gtype;
		    STRUCT_ASSIGN_MACRO(privs_to_revoke->dbp_owner,
			info_p->qeu_authid);
		    for (p = 0; p < DB_COL_WORDS; p++)
			info_p->qeu_attrmap[p] = 0;

		    /* 
		    ** the record of GRANT OPTION FOR privilege will be
		    ** almost identical, so make a copy here
		    */
		    STRUCT_ASSIGN_MACRO((*info_p), (*info_p2));
		    info_p2->qeu_flags &= ~QEU_PRIV_LOST;
		    info_p2->qeu_flags |= QEU_GRANT_OPTION_LOST;
		}
	    }
	    else
	    {
	        for (i = -1, info_p = revoke_info;
		     (i = BTnext(i, (char *) &privs_to_revoke->dbp_popset,
			    BITS_IN(privs_to_revoke->dbp_popset))) != -1;
		     info_p++
		    )
	        {
		    STRUCT_ASSIGN_MACRO(privs_to_revoke->dbp_tabid,
				        info_p->qeu_obj_id);
		    info_p->qeu_priv = 0;
		    BTset(i, (char *) &info_p->qeu_priv);
		    info_p->qeu_flags = 
			QEU_GRANT_OPTION_LOST | grant_compat_flag;
		    info_p->qeu_gtype = privs_to_revoke->dbp_gtype;
		    STRUCT_ASSIGN_MACRO(privs_to_revoke->dbp_owner,
		                        info_p->qeu_authid);
		    for (p = 0; p < DB_COL_WORDS; p++)
		        info_p->qeu_attrmap[p] = 0;
	        }
	    }
	}
	
	/*
	** initialize the array of records which will be used to compute
	** aggregate depth of privileges affected by destruction/alteration
	** of permits conveying grantable privileges
	*/
	perm_depth_info->qeu_pdi_flags = 0;
	perm_depth_info->qeu_old_depth = QEU_OLD_DEPTH_INVALID;
	perm_depth_info->qeu_new_depth = QEU_NEW_DEPTH_INVALID;
	perm_depth_info->qeu_gtype = privs_to_revoke->dbp_gtype;
	STRUCT_ASSIGN_MACRO(privs_to_revoke->dbp_owner,
			    perm_depth_info->qeu_authid);

	if (revoke_grant_option)
	{
	    info_p = revoke_info;
	}
	else
	{
	    info_p = revoke_info + num_revoke_info_recs / 2;
	}
	
	perm_depth_info->qeu_priv = info_p->qeu_priv;

	for (i = 1, depth_info_p = perm_depth_info + 1, info_p++;
	     i < num_depth_info_recs;
	     i++, depth_info_p++, info_p++)
	{
	    STRUCT_ASSIGN_MACRO((*perm_depth_info), (*depth_info_p));
	    depth_info_p->qeu_priv = info_p->qeu_priv;
	}

	/* 
        ** Position Protect table to get all protections matching the specified
	** table id.  
        */	
	prot_qeu->qeu_count = 1;
	prot_qeu->qeu_getnext = QEU_REPO;
	prot_qeu->qeu_klen = 2;

	/*
	** if revoking privilege(s) on a table, view, or database procedure
	** (i.e. NOT on dbevent or sequence) and at least one permit tuple is affected, we
	** will call RDF to invalidate the tuple(s) tree as well as the object
	** on which the permit was defined in RDF's cache.
	**
	** initialize RDF_CB which will be used in calls to RDF  
	*/

	if (~qeuq_cb->qeuq_permit_mask & QEU_EV_PROTECTION &&
	    ~qeuq_cb->qeuq_permit_mask & QEU_SEQ_PROTECTION)
	{
	    /*
	    ** remember that if any permit tuple gets destroyed or modified,
	    ** RDF cache entry for the object itself will need to be invalidated
	    */
	    inv_obj_cache = TRUE;
	    
	    qeu_rdfcb_init((PTR) rdf_inv_cb, qef_cb);

	    rdf_inv_rb->rdr_types_mask  = RDR_PROTECT;

	    if (qeuq_cb->qeuq_permit_mask & QEU_DBP_PROTECTION)
	    {
		rdf_inv_rb->rdr_types_mask |= RDR_PROCEDURE;

		/* dbproc info is cached by dbproc name and owner */
		STRUCT_ASSIGN_MACRO(privs_to_revoke->dbp_obown,
				    rdf_inv_rb->rdr_owner);

		/*
		** can't use STRUCT_ASSIGN_MACRO because src and dest are of
		** different types
		*/
		MEcopy((PTR) &privs_to_revoke->dbp_obname,
		    sizeof(rdf_inv_rb->rdr_name.rdr_prcname),
		    (PTR) &rdf_inv_rb->rdr_name.rdr_prcname);
	    }
	    else
	    {
		/*
		** if we were asked to revoke specific privileges on a table or
		** view, set RDR2_ALIAS in rdr_2types_mask in case we need to
		** flush permit trees from the QTREE cache
		*/
		if (   !revoke_grant_option
		    && ~qeuq_cb->qeuq_permit_mask & QEU_REVOKE_ALL)
		{
		    inv_perm_trees = TRUE;
		    rdf_inv_rb->rdr_2types_mask |= RDR2_ALIAS;
		}

		/* table info is cached by table id */
		STRUCT_ASSIGN_MACRO((*qeuq_cb->qeuq_rtbl),
				    rdf_inv_rb->rdr_tabid);
	    }
	}
	
	for(;;)	    /* loop for all protection tuples */
	{
            status = qeu_get(qef_cb, prot_qeu);
	    if (status != E_DB_OK)
	    {
		if (prot_qeu->error.err_code == E_QE0015_NO_MORE_ROWS)
		{
		    status = E_DB_OK;
		}
		else
		{
		    error = prot_qeu->error.err_code;
		}
		break;
			
	    }

	    if (prot_qeu->qeu_getnext == QEU_REPO)
	    {
		prot_qeu->qeu_getnext = QEU_NOREPO;
		prot_qeu->qeu_klen = 0;
	    }

	    /* access permits and security_alarms will not be affected */
	    if (prot_tuple->dbp_popset & (DB_CAN_ACCESS | DB_ALARM))
		continue;
		
	    /*
	    ** if the user attempting to revoke privilege(s) does not own the 
	    ** object, we need to check whether he himself possesses any 
	    ** privileges on the object; if we haven't found a tuple describing
	    ** a privilege granted to him or to public yet, see if the current 
	    ** tuple would fulfill the requirement
	    ** In order to qualify, this permit must
	    **	- be a modern (6.5 and later) permit and
	    **  - must name PUBLIC or the user attempting to revoke the 
	    **	  privilege as the grantee
	    */
	    if (   cant_access
		&& prot_tuple->dbp_flags & DBP_65_PLUS_PERM
		&& (   prot_tuple->dbp_gtype == DBGR_PUBLIC
		    || (   prot_tuple->dbp_gtype == DBGR_USER
			&& !MEcmp((PTR) &prot_tuple->dbp_owner,
				  (PTR) &privs_to_revoke->dbp_grantor,
				  sizeof(prot_tuple->dbp_owner))
		       )
		   )
	       )
	    {
		/* 
		** user attempting to rrevoke a privilege or PUBLIC possesses
		** some privilege on the object
		*/
		cant_access = FALSE;
	    }

	    /*
	    ** this permit will be affected by REVOKE if
	    **   - it is a modern (6.5 and later) permit
	    **   - it specifies privilege being revoked
	    **	 - if GRANT OPTION FOR is being revoked, it must specify WITH
	    **	   GRANT OPTION
	    **	 - it was granted by specified grantor to specified grantee
	    **	 - if privilege is being revoked on a table or a view,
	    **	   attributes listed in the permit must form a non-empty
	    **	   intersection with attributes privilege or GRANT OPTION FOR
	    **	   privilege on which is being revoked
	    */
	    if (   ~prot_tuple->dbp_flags & DBP_65_PLUS_PERM
		|| !(prot_tuple->dbp_popset & privs_to_revoke->dbp_popset)
		|| (   revoke_grant_option
		    && ~prot_tuple->dbp_popset & DB_GRANT_OPTION)
		|| prot_tuple->dbp_gtype != privs_to_revoke->dbp_gtype
		|| MEcmp((PTR) &prot_tuple->dbp_grantor,
		       (PTR) &privs_to_revoke->dbp_grantor,
		       sizeof(prot_tuple->dbp_grantor))
		|| MEcmp((PTR) &prot_tuple->dbp_owner,
		       (PTR) &privs_to_revoke->dbp_owner,
		       sizeof(prot_tuple->dbp_owner))
	       )
	    {
		continue;
	    }

	    if (   prot_tuple->dbp_obtype == DBOB_TABLE
		|| prot_tuple->dbp_obtype == DBOB_VIEW)
	    {
		bool	    found = FALSE;
		i4	    *perm_map, *privs_lost_map, *res_map;

		perm_map = prot_tuple->dbp_domset;
		privs_lost_map = privs_to_revoke->dbp_domset;
		res_map = attrmap;
		
		for (i = 0;
		     i < DB_COL_WORDS;
		     i++, perm_map++, privs_lost_map++, res_map++
		    )
		{
		    if (*res_map = *perm_map & *privs_lost_map)
			found = TRUE;
		}

		if (!found)
		{
		    continue;
		}
	    }

	    /* 
	    ** we need to set depth_info_p to NULL here to enable us to
	    ** sink up QEU_PERM_DEPTH_INFO structures and revoke_info
	    ** structures used to track a given privilege
	    */
	    depth_info_p = (QEU_PERM_DEPTH_INFO *) NULL;

	    /*
	    ** add info about this permit to the description of revoked
	    ** privileges which we maintain
	    */

	    for (i = 0, info_p = revoke_info; 
		 i < num_revoke_info_recs;
		 i++, info_p++
		)
	    {
		i4	j;
		
		if (info_p->qeu_flags & QEU_GRANT_OPTION_LOST)
		{
		    /* 
		    ** update depth_info_p to keep it in sink (as far as
		    ** tracked privilege is concerned) with info_p
		    */
		    if (depth_info_p == (QEU_PERM_DEPTH_INFO *) NULL)
		    {
			depth_info_p = perm_depth_info;
		    }
		    else
		    {
			depth_info_p++;
		    }
		}

		/*
		** records are specialized by privilege which they track as well
		** as whether the permit specifies only a privilege or
		** GRANT OPTION FOR a privilege
		*/
		if (   !(info_p->qeu_priv & prot_tuple->dbp_popset)
		    || (   info_p->qeu_flags & QEU_GRANT_OPTION_LOST
			&& ~prot_tuple->dbp_popset & DB_GRANT_OPTION)
		   )
		{
		    continue;
		}

		/*
		** if dealing with permit on a table or a view, map of
		** attributes from the current permit which will be affected by 
		** revocation has been computed and placed into attrmap;
		** otherwise (permit applies to a dbproc or a dbevent), simply
		** set all bits in the attribute map since only object-wide
		** privileges may be granted/revoked on dbprocs and dbevents
		*/
		if (   prot_tuple->dbp_obtype == DBOB_TABLE
		    || prot_tuple->dbp_obtype == DBOB_VIEW)
		{
		    for (j = 0, map_from = attrmap,
			 map_to = info_p->qeu_attrmap;
			 j < DB_COL_WORDS;
			 j++, map_from++, map_to++
			)
		    {
			*map_to |= *map_from;
		    }
		}
		else
		{
		    for (j = 0; j < DB_COL_WORDS; j++)
			info_p->qeu_attrmap[j] = ~((i4) 0);
		}
		
		/*
		** if the permit specified a grantable privilege, we may need
		** to update aggregate "old depth" if the permit's depth
		** is lower than the aggregate "old depth" computed so far
		** (or if this is the first permit for a given privilege)
		*/
		if (   depth_info_p
		    && (   depth_info_p->qeu_old_depth == QEU_OLD_DEPTH_INVALID
			|| prot_tuple->dbp_depth < depth_info_p->qeu_old_depth)
		   )
		{
		    depth_info_p->qeu_old_depth = prot_tuple->dbp_depth;
		}
	    }

	    /*
	    ** remember that at least one permit has been changed or destroyed
	    */
	    perm_altered = TRUE;
	    
	    /*
	    ** if (this is a permit on a dbproc, dbevent, sequence, or the permit's
	    **     attribute map is a subset of that in privs_to_revoke)
	    ** {
	    **   if (revoking GRANT OPTION FOR privilege only)
	    **	 {
	    **	   update IIPROTECT tuple to reflect that it no longer grants
	    **	   GRANT OPTION;
	    **	   call qeu_construct_perm_text() to update query text
	    **	   corresponding to the permit;
	    **	 }
	    **	 else
	    **	 {
	    **	   destroy this permit;
	    **	 }
	    ** }
	    ** else
	    ** {
	    **   // privilege or ability to grant privilege is being revoked
	    **	 // on a subset of attributes in the permit's attribute map
	    **
	    **   update the map of attributes to which the permit applies
	    **	 and call qeu_construct_perm_text() to update the permits's
	    **	 query text;
	    **
	    **   if (GRANT OPTION FOR privilege was being revoked)
	    **	 {
	    **	   generate a new permit which would convey the same privilege
	    **	   as the original permit, albeit WITHOUT GRANT OPTION, on the
	    **	   attributes to which REVOKE applied (qeu_construct_perm_text()
	    **	   will be used to generate query text for the new permit)
	    **	   // Conveniently enough, attrmap contains a map of attributes
	    **     // which were affected by this REVOKE
	    **	 }
	    ** }
	    */

	    if (   prot_tuple->dbp_obtype == DBOB_DBPROC
		|| prot_tuple->dbp_obtype == DBOB_EVENT
		|| prot_tuple->dbp_obtype == DBOB_SEQUENCE
		|| BTsubset((char *) prot_tuple->dbp_domset,
		    (char *) privs_to_revoke->dbp_domset,
		    BITS_IN(prot_tuple->dbp_domset))
	       )
	    {
		if (revoke_grant_option)
		{
		    prot_tuple->dbp_popset &= ~DB_GRANT_OPTION;
		    prot_tuple->dbp_popctl &= ~DB_GRANT_OPTION;
		    
		    /*
		    ** now call qeu_replace() to replace IIPROTECT tuple
		    */
		    status = qeu_replace(qef_cb, prot_qeu);
		    if (status != E_DB_OK)
		    {
			error = prot_qeu->error.err_code;
			break;
		    }

		    /*
		    ** and finally, call qeu_construct_perm_text() to
		    ** reconstruct text of the permit based on the permit tuple
		    */
		    status = qeu_construct_perm_text(qeuq_cb, qef_cb, qtxt_qeu,
			&ulm, prot_tuple, FALSE, (QEF_DATA **) NULL, &error); 
		    if (status != E_DB_OK)
			break;
		}
		else
		{
		    QEUQ_CB	dprot_qeuqcb;

		    STRUCT_ASSIGN_MACRO((*qeuq_cb), dprot_qeuqcb);
		    dprot_qeuqcb.qeuq_rtbl = &prot_tuple->dbp_tabid;
		    dprot_qeuqcb.qeuq_pno = prot_tuple->dbp_permno;

		    /*
		    ** tell qeu_dprot() to destroy a permit without
		    ** checking for abandoned objects. Also tell qeu_dprot()
		    ** that we are processing REVOKE PRIVILEGES.
		    */
		    dprot_qeuqcb.qeuq_permit_mask =
			QEU_PERM | QEU_SKIP_ABANDONED_OBJ_CHECK | 
			QEU_REVOKE_PRIVILEGES;

		    /*
		    ** if permit is on a dbproc, sequence or a dbevent, tell
		    ** qeu_dprot() about it
		    */
		    if (prot_tuple->dbp_obtype == DBOB_DBPROC)
			dprot_qeuqcb.qeuq_permit_mask |=
			    QEU_DBP_PROTECTION;
		    else if (prot_tuple->dbp_obtype == DBOB_EVENT)
			dprot_qeuqcb.qeuq_permit_mask |=
			    QEU_EV_PROTECTION;
		    else if (prot_tuple->dbp_obtype == DBOB_SEQUENCE)
			dprot_qeuqcb.qeuq_permit_mask |=
			    QEU_SEQ_PROTECTION;

		    dprot_qeuqcb.qeuq_flag_mask = 0;

		    doaudit=FALSE;
		    status = qeu_dprot(qef_cb, &dprot_qeuqcb);
		    if (status != E_DB_OK)
		    {
			STRUCT_ASSIGN_MACRO(dprot_qeuqcb.error,
					    qeuq_cb->error);
			error = qeuq_cb->error.err_code;
			break;
		    }

		    /*
		    ** if we are revoking specific (as opposed to ALL)
		    ** privileges on a table or a view and this permit had a
		    ** tree associated with it, call rdf_invalidate() to flush
		    ** the tree from the QTREE cache
		    **
		    ** if revoking "all" privileges, simply remember that some
		    ** permit(s) associated with this object involved trees
		    */
		    if (   prot_tuple->dbp_treeid.db_tre_high_time
			|| prot_tuple->dbp_treeid.db_tre_low_time
		       )
		    {
			if (inv_perm_trees)
			{
			    rdf_inv_rb->rdr_sequence = prot_tuple->dbp_permno;
			    status = rdf_call(RDF_INVALIDATE, rdf_inv_cb);
			    if (DB_FAILURE_MACRO(status))
			    {
				error = rdf_inv_cb->rdf_error.err_code;
				break;
			    }
			}
			else
			{
			    inv_tree_cache = TRUE;
			}
		    }
		}
	    }
	    else
	    {
		/*
		** privilege or ability to grant privilege is being revoked on a
		** subset of attributes in the permit's attribute map
		**
		** If this permit might depend on a privilege descriptor (this
		** was determined before) and this permit specifies a privilege
		** other than SELECT, we need to look it up now.
		**
		** Then we will update the map of attributes to which the
		** permit applies and call qeu_construct_perm_text() to
		** update the permits's query text.
		**
		** if (the permit depended on a privilege descriptor)
		** {
		**   call (*qeuq_cb->qeuq_v2b_col_xlate)() to update the map of
		**   attributes in the local copy of the independent privilege
		**   descriptor;
		**
		**   if (there already exists a privilege descriptor that
		**       adequately describes privilege on which the newly
		**       modified permit depends)
		**   {
		**     update IIDBDEPENDS tuple to indicate that it now depends
		**     on a different privilege descriptor;
		**      
		**     if (no other permits depend on the privilege descriptor
		**         on which the newly modified permit used to depend)
		**     {
		**       delete that privilege descriptor
		**     }
		**   }
		**   else if (there are no other permits dependent on this
		**            privilege descriptor)
		**   {
		**     update existing privilege descriptor from the local copy
		**   }
		**   else
		**   {
		**     create a new privilege descriptor based on the
		**     updated local copy;
		**     update IIDBDEPENDS to indicate that this permit now
		**     depends on a different privilege descriptor
		**   }
		** }
		**
		** if only the GRANT OPTION FOR privilege was being revoked, we
		** need to generate a new permit which would convey the same
		** privilege as the original permit, albeit WITHOUT GRANT
		** OPTION, on the attributes to which REVOKE applied
		** (conveniently enough, attrmap contains a map of attributes 
		** which were affected by this REVOKE.)
		** If the original permit depended on some privilege descriptor,
		** the new descriptor will depend on a descriptor whose
		** attribute map will be derived using
		** (*qeuq_cb->qeuq_v2b_col_xlate)()
		*/

		bool		descr_found = FALSE;

		if (   check_priv_descr
		    && prot_tuple->dbp_popset & (DB_REPLACE | DB_APPEND)
		   )
		{
		    /*
		    ** first check if the permit depends on any privilege
		    ** descriptor - this requires that we look in IIXDBDEPENDS
		    ** by table id, qmode (DB_PROT) and permit number
		    */
		    xdep_permno = prot_tuple->dbp_permno;
		    xdep_qeu->qeu_count = 1;
		    status = qeu_get(qef_cb, xdep_qeu);
		    if (status != E_DB_OK)
		    {
			if (xdep_qeu->error.err_code == E_QE0015_NO_MORE_ROWS)
			{
			    /* permit does not depend on any privileges */
			    status = E_DB_OK;
			}
			else
			{
			    error = xdep_qeu->error.err_code;
			    break;
			}
		    }
		    else
		    {
			/*
			** now do a TID join into IIDBDEPENDS to determine
			** descriptor number - tuple MUST exist
			*/
			dep_qeu->qeu_tid = xdep_tuple->dbvx_tidp;
			dep_qeu->qeu_count = 1;
			status = qeu_get(qef_cb, dep_qeu);
			if (status != E_DB_OK)
			{
			    error = dep_qeu->error.err_code;
			    break;
			}

			/*
			** finally, look up the descriptor tuple in IIPRIV
			** by table id and desriptor number found in
			** IIDBDEPENDS.dbv_i_qid
			*/
			priv_qeu->qeu_count = 1;
			priv_descrno = dep_tuple->dbv_i_qid;

			priv_qeu->qeu_getnext = QEU_REPO;
			priv_qeu->qeu_klen = 3;
			
			status = qeu_get(qef_cb, priv_qeu);
			if (status != E_DB_OK)
			{
			    error = priv_qeu->error.err_code;
			    break;
			}

			/*
			** IIPRIV tuple representing a privilege descriptor on
			** which the original permit depended is in *priv_tuple
			*/
			descr_found = TRUE;
		    }
		}

		{
		    i4	    *lost_priv_map, *perm_map;

		    for (i = 0,
			 perm_map = prot_tuple->dbp_domset,
			 lost_priv_map = attrmap;

			 i < DB_COL_WORDS;
			 
			 i++, perm_map++, lost_priv_map++
			)
		    {
			*perm_map &= ~*lost_priv_map;
		    }
		}

		/*
		** now call qeu_replace() to replace IIPROTECT tuple
		*/
		status = qeu_replace(qef_cb, prot_qeu);
		if (status != E_DB_OK)
		{
		    error = prot_qeu->error.err_code;
		    break;
		}

		/*
		** call qeu_construct_perm_text() to reconstruct
		** text of the permit based on the permit tuple
		*/
		status = qeu_construct_perm_text(qeuq_cb, qef_cb,
		    qtxt_qeu, &ulm, prot_tuple, FALSE, (QEF_DATA **) NULL,
		    &error);
		if (status != E_DB_OK)
		    break;

		if (descr_found)
		{
			/*
			** will be set to TRUE if no other permits depend on
			** this privilege descriptor; FALSE otherwise
			*/
		    bool	reuse_priv_descr;
			/*
			** contains TID of the IIPRIV tuple representing a
			** privilege descriptor on which the newly modified
			** permit used to depend
			*/
		    u_i4	iipriv_tid = priv_qeu->qeu_tid;
			/*
			** will be reset to TRUE if there already exists a
			** privilege descriptor which adequately describes
			** privileges on which the newly modified permit depends
			*/
		    bool	found_matching_descr = FALSE;
			/*
			** if (found_matching_descr), will contain number of the
			** descriptor; otherwise will contain the highest
			** descriptor number which was found during the
			** traversal of descriptors associated with this view
			*/
		    i4		descr_no = 0;
			/*
			** will contain a copy of a privilege descripttor on
			** which the newly modified permit will depend; we need
			** to save a copy because *priv_tuple will be changed
			** during the scan of IIPRIV
			*/
		    DB_IIPRIV   save_iipriv;

		    /*
		    ** scan the list of privilege descriptors to determine
		    ** whether there is already one which accurately describes
		    ** the privilege on which the newly modified permit depends;
		    ** if one is found, we will remember that fact + its number,
		    ** otherwise we will rememebr the highest descriptor number
		    ** seen
		    */

		    /*
		    ** compute a map of attributes of the view's underlying
		    ** table or view on privilege on which the updated permit
		    ** will depend
		    */
		    status = (*qeuq_cb->qeuq_v2b_col_xlate)(
			&prot_tuple->dbp_tabid, prot_tuple->dbp_domset,
			&priv_tuple->db_indep_obj_id,
			priv_tuple->db_priv_attmap);
		    if (DB_FAILURE_MACRO(status))
		    {
			error = E_QE0002_INTERNAL_ERROR;
			break;
		    }

		    /*
		    ** save the contents of *priv_tuple which describe the
		    ** privilege on which the newly modified permit will
		    ** depend
		    */
		    STRUCT_ASSIGN_MACRO((*priv_tuple), save_iipriv);
		    
		    /*
		    ** scan privilege descriptors looking for one that would
		    ** adequately describes privilege on which the newly
		    ** modified permit depends or, failing that, the highest
		    ** descriptor number used so far
		    */
		    priv_qeu->qeu_count = 1;
		    priv_qeu->qeu_getnext = QEU_REPO;
		    priv_qeu->qeu_klen = 2;

		    for (;;)
		    {
			status = qeu_get(qef_cb, priv_qeu);
			if (status != E_DB_OK)
			{
			    if (priv_qeu->error.err_code ==
				    E_QE0015_NO_MORE_ROWS)
			    {
				status = E_DB_OK;
			    }
			    break;
			}

			if (priv_qeu->qeu_getnext == QEU_REPO)
			{
			    priv_qeu->qeu_getnext = QEU_NOREPO;
			    priv_qeu->qeu_klen = 0;
			}

			if (priv_tuple->db_descr_no == 0)
			{
			    /*
			    ** descriptor 0 describes priviledges on which a
			    ** view itself depends - it is of no interest to
			    ** us here
			    */
			    continue;
			}

			if ( (i4) (priv_tuple->db_descr_no) >  descr_no)
			{
			    /*
			    ** remember the highest descriptor number seen
			    ** so far
			    */
			    descr_no = priv_tuple->db_descr_no;
			}

			/*
			** now check if the privilege described by this
			** descriptor is the same as that on which the newly
			** modified tuple depends
			*/
			if (save_iipriv.db_priv_map != priv_tuple->db_priv_map)
			{
			    continue;
			}

			for (i = 0; i < DB_COL_WORDS; i++)
			{
			    if (save_iipriv.db_priv_attmap[i] !=
				    priv_tuple->db_priv_attmap[i])
			    {
				break;
			    }
			}

			if (i == DB_COL_WORDS)
			{
			    /* we found a descriptor we were hoping for */
			    found_matching_descr = TRUE;
			    descr_no = priv_tuple->db_descr_no;
			    break;
			}
		    }
		    if (status != E_DB_OK)
		    {
			error = priv_qeu->error.err_code;
			break;
		    }

		    /*
		    ** check whether there are other permits dependent on this
		    ** privilege descriptor
		    */
		    dep2_qeu->qeu_getnext = QEU_REPO;
		    dep2_qeu->qeu_klen = 4;

		    for (i = 0; i < 2; i++)
		    {
			status = qeu_get(qef_cb, dep2_qeu);
			if (status != E_DB_OK)
			{
			    if (    i == 1
				&& dep2_qeu->error.err_code ==
				       E_QE0015_NO_MORE_ROWS
			       )
			    {
				status = E_DB_OK;
			    }
			    break;
			}

			if (dep2_qeu->qeu_getnext == QEU_REPO)
			{
			    dep2_qeu->qeu_getnext = QEU_NOREPO;
			    dep2_qeu->qeu_klen = 0;
			}
		    }
		    if (status != E_DB_OK)
		    {
			error = dep2_qeu->error.err_code;
			break;
		    }

		    reuse_priv_descr = (i < 2);

		    if (found_matching_descr)
		    {
			/*
			** update IIDBDEPENDS to reflect that the newly
			** modified permit depends on a different privilege
			** descriptor than the original permit
			*/
			dep_tuple->dbv_i_qid = descr_no;
			status = qeu_replace(qef_cb, dep_qeu);
			if (status != E_DB_OK)
			{
			    error = dep_qeu->error.err_code;
			    break;
			}

			/*
			** if no other permits depend on the privilege
			** descriptor on which the original permit depended,
			** delete that privilege descriptor using previously
			** saved TID
			*/
			if (reuse_priv_descr)
			{
			    priv_qeu->qeu_flag = QEU_BY_TID;
			    priv_qeu->qeu_getnext = QEU_NOREPO;
			    priv_qeu->qeu_klen = 0;
			    priv_qeu->qeu_tid = iipriv_tid;

			    priv_qeu->qeu_tup_length = sizeof(DB_IIPRIV);
			    status = qeu_delete(qef_cb, priv_qeu);
			    if (status != E_DB_OK)
			    {
				error = priv_qeu->error.err_code;
				break;
			    }

			    /*
			    ** reset qeu_flag since the next access will NOT be
			    ** by tid
			    */
			    priv_qeu->qeu_flag = 0;
			}
		    }
		    else if (reuse_priv_descr)
		    {
			/*
			** replace the current IIPRIV tuple by TID using the
			** previously saved copy of the tuple
			*/
			priv_qeu->qeu_flag = QEU_BY_TID;
			priv_qeu->qeu_getnext = QEU_NOREPO;
			priv_qeu->qeu_klen = 0;
			priv_qeu->qeu_tid = iipriv_tid;

			STRUCT_ASSIGN_MACRO(save_iipriv, (*priv_tuple));
			
			status = qeu_replace(qef_cb, priv_qeu);
			if (status != E_DB_OK)
			{
			    error = priv_qeu->error.err_code;
			    break;
			}
			
			/*
			** reset qeu_flag since the next access will NOT be
			** by tid
			*/
			priv_qeu->qeu_flag = 0;
		    }
		    else
		    {
			/* need to create a new descriptor */
			STRUCT_ASSIGN_MACRO(save_iipriv, (*priv_tuple));
			priv_tuple->db_descr_no = ++descr_no;

			/*
			** tell qeu_append() that we are appending one row
			** this is necessary since qeu_get() has reset
			** qeu_count to 0 after it ran out of rows
			*/
			priv_qeu->qeu_count = 1;
			
			status = qeu_append(qef_cb, priv_qeu);
			if (status != E_DB_OK)
			{
			    error = priv_qeu->error.err_code;
			    break;
			}

			/*
			** now update IIDBDEPENDS to reflect that the newly
			** modified permit depends on a different privilege
			** descriptor than the original permit
			*/
			dep_tuple->dbv_i_qid = descr_no;
			status = qeu_replace(qef_cb, dep_qeu);
			if (status != E_DB_OK)
			{
			    error = dep_qeu->error.err_code;
			    break;
			}
		    }
		}
		
		if (revoke_grant_option)
		{
		    QEF_DATA		    *new_qtxt, *qefdata_p;
		    DB_TIME_ID		    timeid;
		    QEUQ_CB		    cprot_qeuqcb;
		    PSQ_INDEP_OBJECTS	    indep_objs;
		    PSQ_COLPRIV		    colpriv;

		    /*
		    ** if GRANT OPTION FOR privilege is being revoked, we need
		    ** to generate a brand new permit which would convey
		    ** privilege (WITHOUT GRANT OPTION) on attributes on which
		    ** GRANT OPTION FOR privilege was revoked;
		    ** we will use the old permit tuple as a base and update a
		    ** few of its fields
		    */

		    /*
		    ** turn off DB_GRANT_OPTION bit to indicate that GRANT
		    ** OPTION FOR privilege will not be granted by this permit
		    */
		    prot_tuple->dbp_popset &= ~DB_GRANT_OPTION;
		    prot_tuple->dbp_popctl &= ~DB_GRANT_OPTION;

		    /*
		    ** this permit will apply only to those attributes on which
		    ** GRANT OPTION FOR privilege was revoked
		    */
		    for (i = 0,
			 map_from = attrmap, map_to = prot_tuple->dbp_domset;
			 i < DB_COL_WORDS;
			 i++, map_from++, map_to++
			)
		    {
			*map_to = *map_from;
		    }

		    /*
		    ** now we call qeu_construct_perm_text() to construct query
		    ** text for the new permit
		    */
		    status = qeu_construct_perm_text(qeuq_cb, qef_cb, qtxt_qeu,
			&ulm, prot_tuple, TRUE, &new_qtxt, &error);
		    if (status != E_DB_OK)
			break;

		    TMnow((SYSTIME *) &timeid);

		    prot_tuple->dbp_timestamp.db_tim_high_time =
			timeid.db_tim_high_time;
		    prot_tuple->dbp_timestamp.db_tim_low_time  =
			timeid.db_tim_low_time;

		    /*
		    ** populate structure describing privileges, if any, on
		    ** which the new permit will depend
		    */
		    indep_objs.psq_objs = (PSQ_OBJ *) NULL;
		    indep_objs.psq_objprivs = (PSQ_OBJPRIV *) NULL;
		    indep_objs.psq_grantee = &privs_to_revoke->dbp_grantor;
		    if (descr_found)
		    {
			indep_objs.psq_colprivs = &colpriv;
			colpriv.psq_next = (PSQ_COLPRIV *) NULL;
			colpriv.psq_objtype = PSQ_OBJTYPE_IS_TABLE;
			colpriv.psq_privmap = priv_tuple->db_priv_map;
			STRUCT_ASSIGN_MACRO(priv_tuple->db_indep_obj_id,
					    colpriv.psq_tabid);

			status = (*qeuq_cb->qeuq_v2b_col_xlate)(
			    &prot_tuple->dbp_tabid, prot_tuple->dbp_domset,
			    &colpriv.psq_tabid, colpriv.psq_attrmap);
			if (DB_FAILURE_MACRO(status))
			{
			    error = E_QE0002_INTERNAL_ERROR;
			    break;
			}
		    }
		    else
		    {
			indep_objs.psq_colprivs = (PSQ_COLPRIV *) NULL;
		    }
		    
		    /*
		    ** finally we are ready to construct the QEUQ_CB which can
		    ** be given to qeu_cprot() to build a new permit tuple
		    */
		    STRUCT_ASSIGN_MACRO((*qeuq_cb), cprot_qeuqcb);
		    cprot_qeuqcb.qeuq_tsz = 0;
		    cprot_qeuqcb.qeuq_tbl_id = (DB_TAB_ID *) NULL;
		    cprot_qeuqcb.qeuq_ct = 0;
		    cprot_qeuqcb.qeuq_tre_tup = (QEF_DATA *) NULL;
		    for (i = 1, qefdata_p = new_qtxt->dt_next;
			 qefdata_p;
			 i++, qefdata_p = qefdata_p->dt_next
			)
		    ;
		    cprot_qeuqcb.qeuq_cq = i;
		    cprot_qeuqcb.qeuq_qry_tup = new_qtxt;
		    cprot_qeuqcb.qeuq_cp = 1;
		    cprot_qeuqcb.qeuq_pro_tup = &prot_qefdata;
		    cprot_qeuqcb.qeuq_permit_mask = QEU_PERM_OR_SECALM;
		    cprot_qeuqcb.qeuq_dbp_tup =
			cprot_qeuqcb.qeuq_uld_tup = (QEF_DATA *) NULL;
		    cprot_qeuqcb.qeuq_culd = 0;
		    cprot_qeuqcb.qeuq_indep = (PTR) &indep_objs;

		    status = qeu_cprot(qef_cb, &cprot_qeuqcb);
		    if (status != E_DB_OK)
		    {
			error = cprot_qeuqcb.error.err_code;
			STRUCT_ASSIGN_MACRO(cprot_qeuqcb.error, qeuq_cb->error);
			break;
		    }
		}
	    }
       	}

	if (status != E_DB_OK)
	    break;
	/*
	** Audit here, outside the loop for revoke all, that way we only 
	** audit once.
	*/
	if ( audit_revall && Qef_s_cb->qef_state & QEF_S_C2SECURE )
	{
	    status = qeu_secaudit(FALSE, qef_cb->qef_ses_id, 
		(char *)&privs_to_revoke->dbp_obname, 
		&privs_to_revoke->dbp_obown,
		sizeof(privs_to_revoke->dbp_obname), type, msgid, 
	    	SXF_A_SUCCESS | SXF_A_CONTROL, &e_error);
	}

	/*
	** if the user specified privileges to be revoked explicitly, we will
	** verify that all specified privileges existed; otherwise, if
	** ALL PRIVILEGES was specified, we will be satisfied if at least one
	** permit was affected by the operation.
	*/
	if (!perm_altered)
	{
	    qeu_prv_not_revoked_err(privs_to_revoke, qeuq_cb, cant_access,
		&error);
	    qeuq_cb->error.err_code = E_QE0000_OK;
	    break;
	}
	else
	{
	    /*
	    ** if we are revoking priivlege(s) on a table, view, or database
	    ** procedure, flush the object privileges on which were revoked
	    ** from the RDF cache
	    */

	    if (inv_obj_cache)
	    {
		/*
		** if we revoking specific privileges on a table or view,
		** RDR2_ALIAS would be set in rdr_2types_mask - need to reset it
		** before proceeding with invalidation
		*/
		rdf_inv_rb->rdr_2types_mask &= ~RDR2_ALIAS;
		status = rdf_call(RDF_INVALIDATE, rdf_inv_cb);
		if (DB_FAILURE_MACRO(status))
		{
		    error = rdf_inv_cb->rdf_error.err_code;
		    break;
		}
	    }
	    
	    /*
	    ** if we were revoking ALL privileges on a table or view and some of
	    ** the permits involved trees, now's a good time to invalidate them
	    ** from the RDF cache
	    */
	    if (qeuq_cb->qeuq_permit_mask & QEU_REVOKE_ALL)
	    {
		if (inv_tree_cache)
		{
		    /* drop permit trees */
		    rdf_inv_rb->rdr_2types_mask |= RDR2_CLASS;
		    status = rdf_call(RDF_INVALIDATE, rdf_inv_cb);
		    if (DB_FAILURE_MACRO(status))
		    {
			error = rdf_inv_cb->rdf_error.err_code;
			break;
		    }
		}
	    }
	    else
	    {
		/*
		** scan the list of revoked privileges to determine whether all
		** of privileges specified by the user could be revoked;
		** 
		** we will do it by comparing attribute map in
		** privs_to_revoke->dbp_domset with attribute maps in
		** privs_revoked[i].qeu_attrmap;
		**
		** if we were told to revoke privilege(s) (as opposed to
		** GRANT OPTION FOR privilege(s)), limit search to the first 
		** half of records, since the second half would describes 
		** attributes on which GRANT OPTION FOR privilege(s) was revoked
		** as a consequence of revoking the privilege itself
		*/

		i = (revoke_grant_option) ? num_revoke_info_recs
					  : num_revoke_info_recs / 2;
		for (info_p = revoke_info + i - 1; i > 0; i--, info_p--)
		{
		    if (!BTsubset((char *) privs_to_revoke->dbp_domset,
				  (char *) info_p->qeu_attrmap,
				  BITS_IN(info_p->qeu_attrmap))
		       )
		    {
			break;
		    }
		}

		if (i > 0)
		{
		    /*
		    ** produce a warning message indicating that some of
		    ** specified privileges were not revoked because they did
		    ** not exist
		    */
		    qeu_prv_not_revoked_err(privs_to_revoke, qeuq_cb, 
			cant_access, &error);
		    qeuq_cb->error.err_code = E_QE0000_OK;
		}
	    } 
	}

	/*
	** Now we will call qeu_match_privs() to determine which (if any)
	** of the privileges revoked by the current user from the specified
	** grantee have been lost to that grantee (i.e. there are no
	** permits granted to the specified grantee which convey the same
	** privileges.)  If it turns out that any of the privileges
	** specified by the current user have been lost to the grantee,
	** invoke qeu_priv_revoke() to determine whether some objects and/or
	** permits have become abandoned as a result.  
	** Only then will we invoke qeu_depth() to recompute depth of
	** permits which were affected by revocation of specified privileges
	*/
	
	{
	    QEU_REVOKE_INFO		*privs_revoked, *grant_option_revoked;
	    i4				list_size;
	    i4				privs_lost = FALSE;

	    /* 
	    ** first scan the array of records describing privileges
	    ** which were revoked and if any of them have empty attribute
	    ** maps (privilege has not been revoked), reset their privilege map
	    */ 
	    for (i = 0, info_p = revoke_info; 
		 i < num_revoke_info_recs;
	    	 i++, info_p++
		)
	    {
		if (BTnext(-1, (char *) info_p->qeu_attrmap,
			BITS_IN(info_p->qeu_attrmap)) == -1)
		{
		    info_p->qeu_priv = 0;
		}
	    }

	    if (revoke_grant_option)
	    {
		privs_revoked = (QEU_REVOKE_INFO *) NULL;
		grant_option_revoked = revoke_info;
		list_size = num_revoke_info_recs;
	    }
	    else
	    {
		privs_revoked = revoke_info;
		list_size = num_revoke_info_recs / 2;
		grant_option_revoked = privs_revoked + list_size;
	    }

	    /*
	    ** In addition to determining which of the privileges revoked
	    ** by the current user from some grantee G have been totally
	    ** lost to G, qeu_match_privs() is also responsible for computing
	    ** aggregate depth of those privileges which G has not lost
	    ** because other users granted those privilege(s) to G or to PUBLIC
	    **
	    ** Accordingly, we need to copy maps of attributes on which 
	    ** the current user revoked privilege(s) from G from
	    ** QEU_REVOKE_INFO structures describing lost ability to grant
	    ** privileges into corresponding elements of QEU_PERM_DEPTH_INFO
	    ** array
	    */
	    for (i = 0, info_p = grant_option_revoked, 
		 depth_info_p = perm_depth_info; 

		 i < num_depth_info_recs;

		 i++, info_p++, depth_info_p++)
	    {
		if (info_p->qeu_priv)
		{
		    i4		j;

		    for (j = 0, map_from = info_p->qeu_attrmap, 
			 map_to = depth_info_p->qeu_attrmap;

			 j < DB_COL_WORDS;

			 j++, map_from++, map_to++)
		    {
			*map_to = *map_from;
		    }
		}
	    }

	    /* 
	    ** allocate a QEU_PDI_TBL_DESCR structure and 
	    ** call qeu_crt_pdi_tbl() to, among other things,
	    ** create and open the temp table which will be used to
	    ** keep track of privileges whose depth may change as a
	    ** result of destroying specified privilege(s)
	    */
	    ulm.ulm_psize = sizeof(QEU_PDI_TBL_DESCR);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		error = E_QE001E_NO_MEM;
		break;
	    }

	    pdi_tbl_descr = (QEU_PDI_TBL_DESCR *) ulm.ulm_pptr;

	    status = qeu_crt_pdi_tbl(qef_cb, pdi_tbl_descr, qeuq_cb->qeuq_db_id,
		&privs_to_revoke->dbp_tabid, &error);
	    if (DB_FAILURE_MACRO(status))
	    {
		break;
	    }

	    prot_qeu->qeu_klen = 2;

	    status = qeu_match_privs(qeuq_cb, qef_cb, prot_qeu, privs_revoked,
		grant_option_revoked, perm_depth_info, list_size, pdi_tbl_descr,
		prot_tuple, &privs_to_revoke->dbp_obown, &error);
	    if (status != E_DB_OK)
	    {
		break;
	    }

	    /* 
	    ** scan the array of records describing privileges which were
	    ** revoked again, and if qeu_match_privs() determined that some
	    ** of revoked privileges were permanently lost to the specified
	    ** grantee, call qeu_priv_revoke() to perform remaining
	    ** processing
	    */
	    for (i = 0, info_p = revoke_info; 
		 i < num_revoke_info_recs; 
		 i++, info_p++) 
	    {
		if (info_p->qeu_priv)
		{
		    privs_lost = TRUE;
		    break;
		}
	    }

	    if (privs_lost)
	    {
		QEUQ_CB		rqeuq_cb;

		/* 
		** privs_revoked, if non-NULL, now describes privileges 
		** (if any) lost to the specified grantee
		**
		** grant_option_revoked, if not NULL, now describes privileges 
		** which the specified grantee will no longer be able to grant.
		**
		** now we need to determine which objects and/or permits 
		** will become abandoned as a result of loss of privilege(s)
		** described by privs_revoked and GRANT OPTION FOR
		** privilege(s) described by grant_option_revoked, and, 
		** depending on whether cascading destruction was specified, 
		** will either destroy abandoned objects/permits or report 
		** an error - and qeu_priv_revoke() is just the fucntion
		** to do the job
		*/
		STRUCT_ASSIGN_MACRO((*qeuq_cb), rqeuq_cb);
		rqeuq_cb.qeuq_permit_mask = QEU_REVOKE_PRIVILEGES;

		/*
		** if specified grantee lost privilege(s) on a table, view, or 
		** dbproc, we need to force invalidation of QPs that may be 
		** affected by this loss.  In order to do it, we will alter 
		** timestamp of a base table privilege(s) on which are being 
		** lost, UBT of a view privilege(s) on which are being lost, 
		** or UBT of a dbproc privilege(s) on which are being lost.  
		** 
		** Here will will determine whether we have a table whose 
		** timestamp we can alter; if not, and if we are about to 
		** perform cascading revocation, we will advise 
		** qeu_priv_revoke() to do what it can to invalidate QPs 
		** dependent on privileges which are revoked or objects which 
		** are destroyed in the course of cascading revocation
		*/

		if (privs_to_revoke->dbp_obtype == DBOB_TABLE)
		{
		    /*
		    ** since we cannot alter timestamp of core catalogs, we 
		    ** need to call DMT_SHOW() to make sure that the table 
		    ** privileeg(s) on which are being revoked is not one of 
		    ** them
		    */
		    dmt_shw_cb.length = sizeof(DMT_SHW_CB);
		    dmt_shw_cb.type = DMT_SH_CB;
		    dmt_shw_cb.dmt_db_id = qeuq_cb->qeuq_db_id;
		    dmt_shw_cb.dmt_session_id = qeuq_cb->qeuq_d_id;
		    dmt_shw_cb.dmt_flags_mask = DMT_M_TABLE;
		    dmt_shw_cb.dmt_table.data_address = (char *) &table;
		    dmt_shw_cb.dmt_table.data_in_size = sizeof(DMT_TBL_ENTRY);
		    dmt_shw_cb.dmt_char_array.data_address = (PTR) NULL;
		    dmt_shw_cb.dmt_char_array.data_in_size = 0;
		    dmt_shw_cb.dmt_char_array.data_out_size  = 0;
		    STRUCT_ASSIGN_MACRO((*qeuq_cb->qeuq_rtbl), 
			dmt_shw_cb.dmt_tab_id);

		    /* issue DMT_SHOW call */

		    if ((status = dmf_call(DMT_SHOW, &dmt_shw_cb)) != E_DB_OK)
		    {
			error = dmt_shw_cb.error.err_code;
			break;
		    }

		    if (table.tbl_status_mask & DMT_CATALOG)
		    {
			ubt_id.db_tab_base = ubt_id.db_tab_index = 0;
		    }
		    else
		    {
			ubt_id.db_tab_base  = qeuq_cb->qeuq_rtbl->db_tab_base;
			ubt_id.db_tab_index = qeuq_cb->qeuq_rtbl->db_tab_index;
		    }
		}
		else if (privs_to_revoke->dbp_obtype == DBOB_VIEW)
		{
		    status = qeu_v_ubt_id(qef_cb, &privs_to_revoke->dbp_tabid,
			&ubt_id, &qeuq_cb->error);
		    if (DB_FAILURE_MACRO(status))
		    {
			err_already_reported = TRUE;
			break;
		    }
		}
		else if (privs_to_revoke->dbp_obtype == DBOB_DBPROC)
		{
		    status = qeu_p_ubt_id(qef_cb, &privs_to_revoke->dbp_obown,
			(DB_DBP_NAME *) &privs_to_revoke->dbp_obname, &ubt_id,
			&error);
		    if (DB_FAILURE_MACRO(status))
			break;
		}
		else	/* revoking a privilege on a dbevent or sequence */
		{
		    ubt_id.db_tab_base = ubt_id.db_tab_index = 0;
		}

		/* 
		** if the caller has requested that we force invalidation of
		** query plans affected by the destruction of specified
		** privilege(s) and we have not been able to locate id of some 
		** base table whose timestamp could be altered to effect QP 
		** invalidation, we will set QEU_INVALIDATE_QPS in qeu_flags 
		** to let qeu_priv_revoke() know that it is responsible for
		** invalidation of QPs which may be affected by cascading
		** destruction of privileges
		*/
		if (!ubt_id.db_tab_base)
		{
		    for (i = 0, info_p = revoke_info; 
			 i < num_revoke_info_recs; 
			 i++, info_p++) 
		    {
			if (info_p->qeu_priv)
			{
			    info_p->qeu_flags |= QEU_INVALIDATE_QPS;
			}
		    }
		}

		status = qeu_priv_revoke(&rqeuq_cb, qef_cb, privs_revoked, 
		    grant_option_revoked, list_size, pdi_tbl_descr);
		if (status != E_DB_OK)
		{
		    err_already_reported = TRUE;
		    STRUCT_ASSIGN_MACRO(rqeuq_cb.error, qeuq_cb->error);
		    break;
		}
		
		/*
		** if, before calling qeu_priv_revoke(), we saved id of a table
		** whose timestamp needs to be altered to cause invalidation 
		** of QPs which may depend on privilege(s) revoked by the user,
		** now we will finally alter the timestamp and invalidate the 
		** object entry in the RDF cache
		*/

		if (ubt_id.db_tab_base)
		{
		    status = qeu_mdfy_timestamp(qef_cb, &ubt_id, &error);
		    if (DB_FAILURE_MACRO(status))
			break;

		    /*
		    ** if we revoking specific privileges on a table or view,
		    ** RDR2_ALIAS and RDR2_CLASS could be set in 
		    ** rdr_2types_mask - need to reset it before proceeding 
		    ** with invalidation
		    */
		    rdf_inv_rb->rdr_2types_mask &= ~(RDR2_ALIAS | RDR2_CLASS);

		    /* 
		    ** if the object privileges on which were revoked was not a
		    ** base table, rdf_inv_rb->rdr_tabid needs to be overwritten
		    ** with the id of the view's or dbproc's underlying base 
		    ** table
		    */
		    if (privs_to_revoke->dbp_obtype != DBOB_TABLE)
		    {
		        STRUCT_ASSIGN_MACRO(ubt_id, rdf_inv_rb->rdr_tabid);
		    }

		    status = rdf_call(RDF_INVALIDATE, rdf_inv_cb);
		    if (DB_FAILURE_MACRO(status))
		    {
		        error = rdf_inv_cb->rdf_error.err_code;
		        break;
		    }
	        }

		/* 
		** if we were called to revoke privilege(s) on a dbproc, now 
		** we will destroy dbproc's QP
		*/
		if (privs_to_revoke->dbp_obtype == DBOB_DBPROC)
		{
		    status = qeu_destr_dbp_qp(qef_cb, 
			&privs_to_revoke->dbp_obown,
			(DB_DBP_NAME *) &privs_to_revoke->dbp_obname,
			&qeuq_cb->error);
		    if (DB_FAILURE_MACRO(status))
		    {
			err_already_reported = TRUE;
			break;
		    }
		}
	    }

	    /*
	    ** now that we can be sure that no permits representing
	    ** abandoned privileges are left lying around, we can recompute
	    ** depth of permits (if any) that were affected by destruction
	    ** of the specified permit
	    */
	    if (pdi_tbl_descr->qeu_pdi_num_rows > 0)
	    {
		status = qeu_depth(qef_cb, qeuq_cb, &privs_to_revoke->dbp_tabid,
		    pdi_tbl_descr, &privs_to_revoke->dbp_obown, &error);
	        if (status != E_DB_OK)
	    	    break;
	    }
	}

	error = E_QE0000_OK;
	exit_loop = TRUE;
    } /* end for */

    /* We are done or some kind of error occurred. */
    /* call qef_error to handle error messages */

    if (status != E_DB_OK && !err_already_reported)
	(VOID) qef_error(error, 0L, status, &error, &qeuq_cb->error, 0);
    
    /* Close off all the tables. */

    local_status = E_DB_OK;

    /* Close off all the tables. */

    if (prot_qeu)
    {
	local_status = qeu_close(qef_cb, prot_qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(prot_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (qtxt_qeu)
    {
	local_status = qeu_close(qef_cb, qtxt_qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(qtxt_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (xdep_qeu)
    {
	local_status = qeu_close(qef_cb, xdep_qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(xdep_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (dep_qeu)
    {
	local_status = qeu_close(qef_cb, dep_qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(dep_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (dep2_qeu)
    {
	local_status = qeu_close(qef_cb, dep2_qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(dep2_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (priv_qeu)
    {
	local_status = qeu_close(qef_cb, priv_qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(priv_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    /*
    ** if we have created and opened a temp table to keep track of
    ** changes in depth of privileges as a result of destruction of 
    ** specified privilege(s), call qeu_drop_pdi_tbl() to close it and drop it
    */
    if (pdi_tbl_descr)
    {
        local_status = qeu_drop_pdi_tbl(qef_cb, pdi_tbl_descr, (bool) TRUE,
	    &qeuq_cb->error, &error);
        if (DB_FAILURE_MACRO(local_status))
	{
	    (VOID) qef_error(error, 0L, local_status, &error, 
		&qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (ulm.ulm_streamid != (PTR)NULL)
    {
	/* 
	** Close the input stream 
	** NOTE: we must try to close and drop the PDI table BEFORE releasing
	** memory since control blocks for it has been allocated out of the
	** memory stream
	*/

	(VOID) ulm_closestream(&ulm);
    }

    if (transtarted)
    {
        if (status == E_DB_OK)
        {	
	    status = qeu_etran(qef_cb, &tranqeu);
	    if (status != E_DB_OK)
	    {	
		(VOID) qef_error(tranqeu.error.err_code, 0L, status, &error,
		     &qeuq_cb->error, 0);
	    }
	    else
	    {
	        qeuq_cb->error.err_code = E_QE0000_OK;
	    }
	}
	
	if (status != E_DB_OK)
        {
	    local_status = qeu_atran(qef_cb, &tranqeu);
	    if (local_status != E_DB_OK)
	    {
	        (VOID) qef_error(tranqeu.error.err_code, 0L, local_status, 
			    &error, &qeuq_cb->error, 0);
	        if (local_status > status)
		    status = local_status;
	    }
	}
    }

    return (status);
}

/*{
** Name: QEU_CINTG 	- store integrity information
**
** External QEF call:   status = qef_call(QEU_CINTG, &qeuq_cb);
**
** Description:
**	Add integrity , tree, and query text information to the appropriate 
**	catalogs (iiinteg , iitree, iiqrytext).  A qrytext id is created and 
**	used to update the qrytext tuples before placing them in their catalog.
**	A tree id is created and used to update the tree catalog tuples 
**	before placing them in their catalog. A integrity number is created 
**	and used in conjunction with the tree id and qrytext id to update the 
**	integrity tuples.  The integrity tuples are then appended to their 
**	catalog.
**
**	An integrity number is obtained by scanning for all integrities on 
**	this table and finding the highest integrity number.
**
** Inputs:
**      qef_cb                  qef session control block
**      qeuq_cb
**	    .qeuq_eflag	        designate error handling semantis
**				for user errors.
**		QEF_INTERNAL	return error code.
**		QEF_EXTERNAL	send message to user.
**	    .qeuq_rtbl		table id integrity refers to
**	    .qeuq_ct		number of tree tuples
**	    .qeuq_tre_tup	tree tuples
**	    .qeuq_cq		number of qrytext tuples
**	    .qeuq_qry_tup	qrytext tuples
**	    .qeuq_ci		number of integrity tuples
**	    .qeuq_int_tup	integrity tuples
**	    .qeuq_db_id		database id
**	    .qeuq_d_id		dmf session id
**	
** Outputs:
**      qeuq_cb
**          .qeuq_ino		integrity number returned.
**	    .error.err_code	one of the following
**				E_QE0002_INTERNAL_ERROR
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0022_ABORTED
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	16-jun-86 (daved)
**          written
**      20-aug-86 (jennifer)
**	    Changed all transaction calls from QET_xxxx
**          to QEU_xxx.  
**	23-dec-86 (daved) 
**	    close off tables, even in error condition
**      10-dec-87 (puree)
**          Converted all ulm_palloc to qec_palloc
**	20-jan-88 (puree)
**	    Convert all qet_escalate calls to in-line codes.
**	28-jan-88 (puree)
**	    Added debug code to trace illegal error codes returned 
**	    from called routines.
**	11-dec-92 (rickh)
**	    Enable writing of 2nd integrity tuple for CHECK CONSTRAINTS.
**	21-jan-1993 (bryanp)
**	    Change edit check from qeuq_cb->qeuq_ci != 1 to qeuq_cb->qeuq_cb > 2
**	    so that attempts to add 2 integrity tuples aren't rejected with
**	    QE0018.
**	16-mar-93 (andre)
**	    having successfully created an integrity, return its number inside
**	    qeuq_cb->qeuq_ino; this will enable code generating rules to
**	    enforce constraints to record dependence of rule(s) on a constraint
**	    which they enforce
**	27-mar-93 (andre)
**	    a new REFERENCES constraint will depend on a UNIQUE or PRIMARY KEY
**	    constraint on <referenced columns> and, if the owner of the
**	    <referenced table> is different from that of the
**	    <referencing table>, on REFERENCES privilege on <referenced
**	    columns>.  If IIINTEGRITY tuples passed by the caller represent a
**	    REFERENCES constraint, we must insert a record of object and
**	    privilege (if any) on which it will depend into IIDBDEPENDS and
**	    IIINTEGRITY
**	23-sep-93 (stephenb)
**	    Creation of an integrity is an auditable event, added call to 
**	    qeu_secaudit().
**	4-oct-93 (rickh)
**	    When creating an integrity, scan IIINTEGRITY with shared access.
**	    This should cut down on the deadlocks we get when creating
**	    NOT NULL columns, integrities, and CONSTRAINTS.
**	27-oct-93 (andre)
**	    As a part of fix for bug 51852 (which was reported in 6.4 but can
**	    and will manifest itself in 6.5 as soon as someone places some 
**	    stress on the DBMS), we want to use a combination of the first half
**	    of table id and the integrity number (really guaranteed to
**	    be unique) instead of timestamps (allegedly unique, but in fact may
**	    be non-unique if several objects get created in rapid succession on
**	    a reasonably fast box) to identify IIQRYTEXT and IITREE tuples
**          associated with a given integrity or constraint.  This combination 
**	    id (pumped through randomizing function) will be used to key into 
**	    IIQRYTEXT and will be stored instead of the timestamp in IITREE 
**
**	    store creation time in the new attribute of IIINTEGRITY
**	11-nov-93 (andre)
**	    Store text/tree id in IIINTEGRITY tuple only if there are text/tree
**	    tuples associated with it.
*/
DB_STATUS
qeu_cintg(
QEF_CB          *qef_cb,
QEUQ_CB		    *qeuq_cb)
{
    GLOBALREF QEF_S_CB *Qef_s_cb;
    DB_IIQRYTEXT    *qtuple;
    DB_IITREE	    *ttuple;
    DB_INTEGRITY    *ituple;
    QEF_DATA	    *next;
    DB_STATUS	    status, local_status;
    i4	    error;
    bool	    transtarted = FALSE;	    
    bool	    tbl_opened = FALSE;
    i4		    i;
    i4		    number;
    QEU_CB	    tranqeu;
    QEU_CB	    qeu;
    DMT_CB	    dmt_cb;
    ULM_RCB         ulm;        
    DB_TAB_ID	    randomized_id;
    DB_QRY_ID	    creation_time;
    QEF_DATA	    qef_data;
    QEF_DATA	    *tupleStream;
    DMT_CHAR_ENTRY  char_array[2];
    DMR_ATTR_ENTRY  key_array[3];
    DMR_ATTR_ENTRY  *key_ptr_array[3];
    i4		    exit_loop;
    
    for (i=0; i< 3; i++)
	key_ptr_array[i] = &key_array[i];

    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
    ulm.ulm_streamid = (PTR)NULL;

    for (exit_loop = FALSE; !exit_loop; )
    {
	/* Check the control block. */

	status = E_DB_ERROR;
	error = E_QE0017_BAD_CB;
	if (qeuq_cb->qeuq_type != QEUQCB_CB ||
            qeuq_cb->qeuq_length != sizeof(QEUQ_CB))	
	    break;

	error = E_QE0018_BAD_PARAM_IN_CB;
	if (   (qeuq_cb->qeuq_cq == 0 || qeuq_cb->qeuq_qry_tup == 0)
            || (qeuq_cb->qeuq_ci > 2 || qeuq_cb->qeuq_int_tup == 0)
            || (qeuq_cb->qeuq_rtbl == 0)
            || (qeuq_cb->qeuq_db_id == 0)
            || (qeuq_cb->qeuq_d_id == 0))

	    break;
		

	/* 
	** Check to see if a transaction is in progress,
	** if so, set transtarted flag to FALSE, otherwise
	** set it to TRUE after beginning a transaction.
	** If we started a transaction then we will abort
        ** it if an error occurs, otherwise we will just
        ** return the error and let the caller abort it.
	*/

	if (qef_cb->qef_stat == QEF_NOTRAN)
	{
	    tranqeu.qeu_type = QEUCB_CB;
	    tranqeu.qeu_length = sizeof(QEUCB_CB);
	    tranqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
	    tranqeu.qeu_d_id = qeuq_cb->qeuq_d_id;
	    tranqeu.qeu_flag = 0;
	    tranqeu.qeu_mask = 0;
	    status = qeu_btran(qef_cb, &tranqeu);
	    if (status != E_DB_OK)
	    {	
		error = tranqeu.error.err_code;
		break;	
	    }	    
	    transtarted = TRUE;
	}
	/* escalate the transaction to MST */
	if (qef_cb->qef_auto == QEF_OFF)
	    qef_cb->qef_stat = QEF_MSTRAN;

	/* 
        ** Now alter the table tuple in the relation table
        ** to indicate protections exist. This validates that
        ** the table exists and insures that an exclusive
        ** lock exists on the table.
        */

	dmt_cb.dmt_flags_mask = 0;
	dmt_cb.dmt_db_id = qeuq_cb->qeuq_db_id;
	dmt_cb.dmt_char_array.data_in_size = sizeof(DMT_CHAR_ENTRY) * 1;
	dmt_cb.dmt_char_array.data_address = (PTR) char_array;
	char_array[0].char_id = DMT_C_INTEGRITIES;
	char_array[0].char_value = DMT_C_ON;

	dmt_cb.length = sizeof(DMT_CB);
	dmt_cb.type = DMT_TABLE_CB;
	dmt_cb.dmt_id.db_tab_base = qeuq_cb->qeuq_rtbl->db_tab_base;
	dmt_cb.dmt_id.db_tab_index = qeuq_cb->qeuq_rtbl->db_tab_index;
	dmt_cb.dmt_tran_id = qef_cb->qef_dmt_id;

	status = dmf_call(DMT_ALTER, &dmt_cb);
	if (status != E_DB_OK)
	{
	    error = dmt_cb.error.err_code;
	    break;
	}

	/*
	** Get a memory stream for reading the protection 
        ** tuples.
	*/
	/* Open stream and allocate memory with one effort */
	ulm.ulm_flags = ULM_PRIVATE_STREAM | ULM_OPEN_AND_PALLOC;
	ulm.ulm_psize = ulm.ulm_blocksize = sizeof(DB_INTEGRITY);

	if ((status = qec_mopen(&ulm)) != E_DB_OK)
	{
	    error = E_QE001E_NO_MEM;
	    break;
	}

	ituple = (DB_INTEGRITY*) ulm.ulm_pptr;
    
	/*
	** Now get the integrity number for this table.
        */
	
	qeu.qeu_type = QEUCB_CB;
        qeu.qeu_length = sizeof(QEUCB_CB);
        qeu.qeu_tab_id.db_tab_base  = DM_B_INTEGRITY_TAB_ID;
        qeu.qeu_tab_id.db_tab_index  = DM_I_INTEGRITY_TAB_ID;
        qeu.qeu_db_id = qeuq_cb->qeuq_db_id;
        qeu.qeu_lk_mode = DMT_IS;
        qeu.qeu_flag = DMT_U_DIRECT;
        qeu.qeu_access_mode = DMT_A_READ;
	qeu.qeu_mask = 0;
         	
	status = qeu_open(qef_cb, &qeu);
	if (status != E_DB_OK)
	{
	    error = qeu.error.err_code;
	    break;
	}
	tbl_opened = TRUE;
	qeu.qeu_count = 1;
        qeu.qeu_tup_length = sizeof(DB_INTEGRITY);
	qeu.qeu_input = &qef_data;
	qeu.qeu_output = &qef_data;
	qef_data.dt_next = 0;
        qef_data.dt_size = sizeof(DB_INTEGRITY);
        qef_data.dt_data = (PTR) ituple;
	
	qeu.qeu_getnext = QEU_REPO;
	qeu.qeu_klen = 2;       
	qeu.qeu_key = key_ptr_array;
	key_ptr_array[0]->attr_number = DM_1_INTEGRITY_KEY;
	key_ptr_array[0]->attr_operator = DMR_OP_EQ;
	key_ptr_array[0]->attr_value = (char*) 
				&qeuq_cb->qeuq_rtbl->db_tab_base;
	key_ptr_array[1]->attr_number = DM_2_INTEGRITY_KEY;
	key_ptr_array[1]->attr_operator = DMR_OP_EQ;
	key_ptr_array[1]->attr_value = (char*) 
                                 &qeuq_cb->qeuq_rtbl->db_tab_index;
        
	qeu.qeu_qual = 0;
	qeu.qeu_qarg = 0;
     
	/* 
        ** Get all integrity tuples for this table
        ** and determine the next integrity number.
        */
 
	status = E_DB_OK;
	number = 0;
	while (status == E_DB_OK)
	{
            status = qeu_get(qef_cb, &qeu);
	    if (status != E_DB_OK)
	    {
		error = qeu.error.err_code;
		break;
	    }
	    qeu.qeu_getnext = QEU_NOREPO;
            qeu.qeu_klen = 0;
	    if (ituple->dbi_number > number)
                number = ituple->dbi_number;
	}
	if (error != E_QE0015_NO_MORE_ROWS)
	    break;
 
	status = E_DB_OK;

	/*
	** Now re-open the table with exclusive page access so we can
	** write new integrity tuples.
	*/

	status = qeu_close(qef_cb, &qeu);    
	if (status != E_DB_OK)
	{
	    error = qeu.error.err_code;
	    break;
	}
	tbl_opened = FALSE;

        qeu.qeu_lk_mode = DMT_IX;
        qeu.qeu_access_mode = DMT_A_WRITE;
	status = qeu_open(qef_cb, &qeu);
	if (status != E_DB_OK)
	{
	    error = qeu.error.err_code;
	    break;
	}
	tbl_opened = TRUE;

	/* 
	** store integrity number inside QEUQ_CB where the caller can see it 
        ** (we have to add 1 to derive the next integrity number)
	*/
	qeuq_cb->qeuq_ino = ++number;
	
	/* 
	** only compute a "randomized" id if there are any text or tree tuples 
	** associated with the constraint (currently, there is text associated 
	** with all integrities and constraints, but trees are only associated 
	** with "old-style" integrities)
	*/
	if (qeuq_cb->qeuq_cq || qeuq_cb->qeuq_ct)
	{
	    /*
	    ** compute a randomized id based on the first half of table id and 
	    ** the integrity number; this id will be used to determine IIQRYTEXT
	    ** and IITREE tuple(s) associated with this integrity or constraint
	    */
	    randomized_id.db_tab_base = 
	        ulb_rev_bits(qeuq_cb->qeuq_rtbl->db_tab_base);
	    randomized_id.db_tab_index = ulb_rev_bits((u_i4) number);
	}

	/* 
        ** Now update the new integrity tuples with
        ** the query text id, and the tree id.
        */

	for ( i = 0, tupleStream = qeuq_cb->qeuq_int_tup;
	      i < qeuq_cb->qeuq_ci;
	      i++, tupleStream = tupleStream->dt_next )
	{
	    ituple = (DB_INTEGRITY *) tupleStream->dt_data;
	    ituple->dbi_number = number;

	    /* 
	    ** if there are text tuple(s) for this constraint/integrity, store
	    ** text id in the IIINTEGRITY tuple; otherwise set it to 0
	    */
	    if (qeuq_cb->qeuq_cq)
	    {
	        ituple->dbi_txtid.db_qry_high_time = randomized_id.db_tab_base;
		ituple->dbi_txtid.db_qry_low_time  = randomized_id.db_tab_index;
	    }
	    else
	    {
		ituple->dbi_txtid.db_qry_high_time =
		    ituple->dbi_txtid.db_qry_low_time = 0;
	    }

	    /*
	    ** if there are tree tuple(s) for this constraint/integrity, store
	    ** tree id in the IIINTEGRITY tuple; otherwise set it to 0
	    */
	    if (qeuq_cb->qeuq_ct)
	    {
		ituple->dbi_tree.db_tre_high_time = randomized_id.db_tab_base;
		ituple->dbi_tree.db_tre_low_time  = randomized_id.db_tab_index;
	    }
	    else
	    {
		ituple->dbi_tree.db_tre_high_time =
		    ituple->dbi_tree.db_tre_low_time = 0;
	    }
	}

        /* store creation date */
        TMnow((SYSTIME *) &creation_time);
        ituple->dbi_created = creation_time.db_qry_high_time;

	qeu.qeu_count = qeuq_cb->qeuq_ci;
        qeu.qeu_tup_length = sizeof(DB_INTEGRITY);
	qeu.qeu_input = qeuq_cb->qeuq_int_tup;
	status = qeu_append(qef_cb, &qeu);
	if (status != E_DB_OK)
	{
	    error = qeu.error.err_code;
	    break;
	}
	    
	status = qeu_close(qef_cb, &qeu);    
	if (status != E_DB_OK)
	{
	    error = qeu.error.err_code;
	    break;
	}
	tbl_opened = FALSE;

	/* Now Update the iiqrytext and iitree system tables. */

	qeu.qeu_tab_id.db_tab_base = DM_B_QRYTEXT_TAB_ID;
	qeu.qeu_tab_id.db_tab_index = 0L;
	status = qeu_open(qef_cb, &qeu);
	if (status != E_DB_OK)
	{
	    error = qeu.error.err_code;
	    break;
	}
	tbl_opened = TRUE;
    
	/* Update all query text tuples with query id. */

	next = qeuq_cb->qeuq_qry_tup;
	for (i = 0; i < qeuq_cb->qeuq_cq; i++)
	{
	    qtuple = (DB_IIQRYTEXT*) next->dt_data;
	    next = next->dt_next;
	    qtuple->dbq_txtid.db_qry_high_time = randomized_id.db_tab_base;
	    qtuple->dbq_txtid.db_qry_low_time  = randomized_id.db_tab_index;
	    if (i < (qeuq_cb->qeuq_cq -1) && next == 0)
	    {
		error = E_QE0018_BAD_PARAM_IN_CB;
		status = E_DB_ERROR;
		break;
	    }
	}
	if (status != E_DB_OK)    		
	    break;

	qeu.qeu_count = qeuq_cb->qeuq_cq;
        qeu.qeu_tup_length = sizeof(DB_IIQRYTEXT);
	qeu.qeu_input = qeuq_cb->qeuq_qry_tup;
	status = qeu_append(qef_cb, &qeu);
	if (status != E_DB_OK)
	{
	    error = qeu.error.err_code;
	    break;
	}
	    
	status = qeu_close(qef_cb, &qeu);    
	if (status != E_DB_OK)
	{
	    error = qeu.error.err_code;
	    break;
	}
	tbl_opened = FALSE;
	
	qeu.qeu_tab_id.db_tab_base = DM_B_TREE_TAB_ID;
	qeu.qeu_tab_id.db_tab_index = DM_I_TREE_TAB_ID;
	status = qeu_open(qef_cb, &qeu);
	if (status != E_DB_OK)
	{
	    error = qeu.error.err_code;
	    break;
	}
	tbl_opened = TRUE;
    		
	/* Update all tree text tuples with tree id. */

	next = qeuq_cb->qeuq_tre_tup;
	for (i=0; i<qeuq_cb->qeuq_ct; i++)
	{
	    ttuple = (DB_IITREE*) next->dt_data;
	    next = next->dt_next;
	    ttuple->dbt_trid.db_tre_high_time = randomized_id.db_tab_base;
	    ttuple->dbt_trid.db_tre_low_time  = randomized_id.db_tab_index;
	    if (i < (qeuq_cb->qeuq_ct -1) && next == 0)
	    {
		error = E_QE0018_BAD_PARAM_IN_CB;
		status = E_DB_ERROR;
		break;
	    }
	}
	if (status != E_DB_OK)    		
	    break;

	qeu.qeu_count = qeuq_cb->qeuq_ct;
	qeu.qeu_tup_length = sizeof(DB_IITREE);
	qeu.qeu_input = qeuq_cb->qeuq_tre_tup;
	status = qeu_append(qef_cb, &qeu);
	if (status != E_DB_OK)
	{
	    error = qeu.error.err_code;
	    break;
	}
	    
	status = qeu_close(qef_cb, &qeu);    
	if (status != E_DB_OK)
	{
	    error = qeu.error.err_code;
	    break;
	}
	tbl_opened = FALSE;

	/*
	** if creating a REFERENCES constraint, we need to enter description of
	** a UNIQUE or PRIMARY KEY constraint and of privilege on <referenced
	** columns> (if any) on which the new constraint depends into
	** IIDBDEPENDS and IIPRIV.
	*/
	if (ituple->dbi_consflags & CONS_REF)
	{
	    i2			    descr_no;
	    PSQ_INDEP_OBJECTS	    *indep_objs =
		(PSQ_INDEP_OBJECTS *) qeuq_cb->qeuq_indep;

	    /*
	    ** a REFERENCES privilege will depend at least on a UNQIE or PRIMARY
	    ** KEY constraint - caller must supply the independent
	    ** object/privilege description
	    */
	    if (!indep_objs || !indep_objs->psq_objs)
	    {
		error = E_QE0018_BAD_PARAM_IN_CB;
		status = E_DB_ERROR;
		break;
	    }

	    /*
	    ** if the constraint depends on some privileges, first call
	    ** qeu_nxt_prv_descr_no() to get the next available privilege
	    ** descriptor number; then call qeu_enter_dependencies() to insert
	    ** approriate tuples into IIDBDEPENDS and IIPRIV
	    */
	    if (indep_objs->psq_colprivs)
	    {
		status = qeu_nxt_prv_descr_no(qef_cb, qeuq_cb,
		    &descr_no, &error);
		if (DB_FAILURE_MACRO(status))
		    break;
	    }
	    else
	    {
		descr_no = 0;
	    }

	    status = qeu_enter_dependencies(qef_cb, qeuq_cb,
		qeuq_cb->qeuq_rtbl, (DB_QMODE) DB_CONS, descr_no);
	    if (status != E_DB_OK)
	    {
		error = qeuq_cb->error.err_code;
		break;
	    }
	}
	/* 
        ** Close off the memory stream created for reading
        ** integrity tuples -after- last dereference of "ituple".
        */
	ulm_closestream(&ulm);
    
	if (transtarted)
	{
	    status = qeu_etran(qef_cb, &tranqeu);
	    if (status != E_DB_OK)
	    {
		error = tranqeu.error.err_code;
		break;
	    }
	}

	exit_loop = TRUE;
    } /* end for */

    if ( status == E_DB_OK && Qef_s_cb->qef_state & QEF_S_C2SECURE )
    {
	DB_ERROR	e_error;
	DB_TAB_ID	tabid;
	tabid.db_tab_base=
		       qeuq_cb->qeuq_rtbl->db_tab_base;
	tabid.db_tab_index=
		       qeuq_cb->qeuq_rtbl->db_tab_index;
	/*
	** We need to audit successful create of integrity
	*/
	status = qeu_secaudit( FALSE, qef_cb->qef_ses_id, 
		qeuq_cb->qeuq_tabname.db_tab_name, &qef_cb->qef_user,
		sizeof(qeuq_cb->qeuq_tabname.db_tab_name), SXF_E_TABLE,
		I_SX203D_INTEGRITY_CREATE, SXF_A_SUCCESS | SXF_A_CONTROL,
		&e_error);
	if (status != E_DB_OK)
	    error = e_error.err_code;
    }
    if ( status == E_DB_OK )
    {
	/* SUCCESS - we are done */
	qeuq_cb->error.err_code = E_QE0000_OK;
	return (E_DB_OK);
    }

    /* call qef_error to handle error messages */

    (VOID) qef_error(error, 0L, status, &error, &qeuq_cb->error, 0);
    
    /* Close off all the tables. */

    local_status = E_DB_OK;
    if (ulm.ulm_streamid != (PTR)NULL)
    {
	/* 
        ** Close off the memory stream created for reading
        ** protection tuples. 
        */

	ulm_closestream(&ulm);

    }    	
    if (tbl_opened)		    /* if iitree opened, close it */
    {
	local_status = qeu_close(qef_cb, &qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(qeu.error.err_code, 0L, local_status, &error, 
		    &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (transtarted)
    {
        local_status = qeu_atran(qef_cb, &tranqeu);
        if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(tranqeu.error.err_code, 0L, local_status, 
		&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    return (status);
}


/*{
** Name: QEU_DINTG	- destroy integrity information
**
** External QEF call:   status = qef_call(QEU_DINTG, &qeuq_cb);
**
** Description:
**      Given an integrity number, this routine will remove integrity 
**	information and qrytext information. The relation is always updated 
**	to ensure that timestamp information is correct. 
**
** Inputs:
**      qef_cb                  qef session control block
**      qeuq_cb
**	    .qeuq_eflag	        designate error handling semantis
**				for user errors.
**		QEF_INTERNAL	return error code.
**		QEF_EXTERNAL	send message to user.
**	    .qeuq_rtbl		table id integrity refers to
**	    .qeuq_ino		integrity number. Zero means all.
**	    .qeuq_db_id		database id
**	    .qeuq_d_id		dmf session id
**	integ			if TRUE, destroy "old style" INGRES 
**				integrity(ies)
**	cons			if TRUE, destroy constraint(s)
**	
** Outputs:
**      qeuq_cb
**	    .error.err_code	one of the following
**				E_QE0002_INTERNAL_ERROR
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0022_ABORTED
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	16-jun-86 (daved)
**          written
**      20-aug-86 (jennifer)
**	    Changed all transaction calls from QET_xxxx
**          to QEU_xxx.  
**	23-dec-86 (daved) 
**	    close off tables, even in error condition
**      10-dec-87 (puree)
**          Converted all ulm_palloc to qec_palloc
**	20-jan-88 (puree)
**	    Convert all qet_escalate calls to in-line codes.
**	24-jun-92 (andre)
**	    make minor changes to take fuller advantage of qeu_delete()'s
**	    ability to delete multiple tuples by key subject to qualification
**	    function when deleting IITREE and IIQRYTEXT tuples
** 	19-aug-92 (andys)
**          Integrate fix for b33494 from ingres63p. 
**	    When dropping an integrity always update the tables timestamp, 
**	    this was previously only done if all integrities on table were 
**	    dropped. The original author of this change was rmuth.
** 	19-aug-92 (andys)
**	    Change misleading comment at head of function. We no longer only 
**	    update iirelation on the 'drop integrity on tablename all' case,
**	    we always do it.
**	20-mar-93 (andre)
**	    added integ and cons to the interface; 
**	    if a tuple represents an "old style" INGRES integrity and !integ, 
**	    the tuple will not be deleted;
**	    if a tuple represents a constraint and !cons, the tuple will not be
**          deleted
**	    if a user requested that we drop a specific INGRES integrity by 
**	    supplying its number but a tuple indicates that this a constraint,
**	    notify user that integrity with specified number does not exist
**
**          having deleted description(s) of one or more REFERENCES constraints,
**	    we will call qeu_dbdelete() to remove descriptions of dependencies
**	    of constraints which were deleted on other DBMS objects
**
**	    if the constraint being deleted was a CHECK constraint that has
**	    rendered some attribute(s) "known not nullable", we will make sure
**	    to delete both IIINTEGRITY tuples associated with the constraint
**	28-apr-93 (vijay)
**	    AIX compiler does not allow LHS typecasting.
**	18-jun-93 (rickh)
**	    Drop IIKEY tuples when exterminating a CONSTRAINT.
**	23-sep-93 (stephenb)
**	    Drop integrity is an auditable event, add call to qeu_secaudit().
**	15-nov-93 (andre)
**	    since an integrity/constraint defined on a table may have the same 
**	    query text id as a permit defined on the same table, when dropping 
**	    text associated with a given integrity/constraint, we must specify 
**	    three keys: txtid1, txtid2, and mode
**	22-jul-96 (ramra01 for bryanp)
**	    Alter table drop col check for integrity restrict/cascade
**	06-feb-1997 (shero03)
**	    B80069: call qeu_v_col_drop to patch the iitree data when dropping
**	    a column from a table with an integrity.
**	24-mar-1997 (nanpr01)
**	    B81224 : more referential integrity bug.
**	04-apr-1997 (nanpr01)
**	    qeu_v_col_drop was setting some tqeu field which needs to be 
**	    reset back. So use a tempqeu instead of the original tqeu.
**	    This is causing bug 81408.
**	06-may-1997 (nanpr01)
**	    A previous fix ignored to set the drop_depobj variable. #81767.
**	13-apr-1999 (nanpr01)
**	    bug 96359 : we are dropping the more unique constraint entries
**	    than required. For unique constraints, do not reset the qeuq_cb.
**	    qeuq_ano & qeuq_keyattid.
**	02-sep-2002 (gygro01) ingsrv1859
**	    b108474 (b108638): Too many referential constraints are dropped
**	    by drop column cascade processing, even referential constraints 
**	    on the same table without any dependencies to the dropped column. 
**
**	    And during drop column cascade processing iidbdepend rows of
**	    removed referential constraints needs to be dropped one by one
**	    using the integrity number, because during drop column not all 
**	    referential constraints of a table will be affected. 
**	    (b108474 (b108636))
*/

#define	NUMBER_OF_IIQRYTEXT_KEYS	3
#define	NUMBER_OF_IITREE_KEYS		3
#define	NUMBER_OF_IIINTEGRITY_KEYS	3
#define	NUMBER_OF_IIKEY_KEYS		2

#define	STUFF_KEY_POINTERS( pointerArray, keyArray, numberOfKeys )	\
	for ( i = 0; i < numberOfKeys; i++ )	\
	{ pointerArray[ i ] = keyArray + i; }

DB_STATUS
qeu_dintg(
QEF_CB          *qef_cb,
QEUQ_CB		*qeuq_cb,
bool		integ,
bool		cons)
{
    GLOBALREF QEF_S_CB *Qef_s_cb;
    DB_INTEGRITY    *ituple;
    DB_STATUS	    status, local_status;
    i4	    error;
    bool	    transtarted = FALSE;	    
    bool	    tbl_opened = FALSE;
    bool	    qtext_opened = FALSE;
    bool	    intg_opened = FALSE;
    bool	    iikey_opened = FALSE;
    bool	    ref_cons_dropped = FALSE;
    bool	    get_2nd_tuple = FALSE;
    bool	    knn_check_cons;
    bool	    constraintWithKeys;
    i4		    i;
    i2		    tree_mode = DB_INTG;
    i4		    txt_mode = DB_INTG;
    QEU_CB	    tranqeu;
    QEU_CB	    iqeu;
    QEU_CB	    qqeu;
    QEU_CB	    tqeu, tempqeu;
    QEU_CB	    iikeyQEU_CB;
    DMT_CB	    dmt_cb;
    ULM_RCB         ulm;        
    QEF_DATA	    iqef_data;
    DMT_CHAR_ENTRY  char_array[2];
    DMR_ATTR_ENTRY  ikey_array[ NUMBER_OF_IIINTEGRITY_KEYS ];
    DMR_ATTR_ENTRY  *ikey_ptr_array[ NUMBER_OF_IIINTEGRITY_KEYS ];
    DMR_ATTR_ENTRY  qkey_array[ NUMBER_OF_IIQRYTEXT_KEYS ];
    DMR_ATTR_ENTRY  *qkey_ptr_array[ NUMBER_OF_IIQRYTEXT_KEYS ];
    DMR_ATTR_ENTRY  tkey_array[ NUMBER_OF_IITREE_KEYS ];
    DMR_ATTR_ENTRY  *tkey_ptr_array[ NUMBER_OF_IITREE_KEYS ];
    DMR_ATTR_ENTRY  iikey_keyArray[ NUMBER_OF_IIKEY_KEYS ];
    DMR_ATTR_ENTRY  *iikey_keyPtrArray[ NUMBER_OF_IIKEY_KEYS ];
    i4		    exit_loop;
    DB_TREE_ID      last_tree_id = {0, 0};


    STUFF_KEY_POINTERS( ikey_ptr_array, ikey_array,
			NUMBER_OF_IIINTEGRITY_KEYS );
    STUFF_KEY_POINTERS( qkey_ptr_array, qkey_array,
			NUMBER_OF_IIQRYTEXT_KEYS );
    STUFF_KEY_POINTERS( tkey_ptr_array, tkey_array,
			NUMBER_OF_IITREE_KEYS );
    STUFF_KEY_POINTERS( iikey_keyPtrArray, iikey_keyArray,
			NUMBER_OF_IIKEY_KEYS );

    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
    ulm.ulm_streamid = (PTR)NULL;

    for (exit_loop = FALSE; !exit_loop; )
    {

	/* Check the control block. */

	error = E_QE0017_BAD_CB;
	status = E_DB_ERROR;
	if (qeuq_cb->qeuq_type != QEUQCB_CB ||
            qeuq_cb->qeuq_length != sizeof(QEUQ_CB))	
	    break;

	error = E_QE0018_BAD_PARAM_IN_CB;
	if (   (qeuq_cb->qeuq_rtbl == 0)
	    || (qeuq_cb->qeuq_db_id == 0)
            || (qeuq_cb->qeuq_d_id == 0))

	    break;
		

	/* 
	** Check to see if a transaction is in progress,
	** if so, set transtarted flag to FALSE, otherwise
	** set it to TRUE after beginning a transaction.
	** If we started a transaction them we will abort
        ** it if an error occurs, otherwise we will just
        ** return the error and let the caller abort it.
	*/

	if (qef_cb->qef_stat == QEF_NOTRAN)
	{
	    tranqeu.qeu_type = QEUCB_CB;
	    tranqeu.qeu_length = sizeof(QEUCB_CB);
	    tranqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
	    tranqeu.qeu_d_id = qeuq_cb->qeuq_d_id;
	    tranqeu.qeu_flag = 0;
	    tranqeu.qeu_mask = 0;
	    status = qeu_btran(qef_cb, &tranqeu);
	    if (status != E_DB_OK)
	    {	
		error = tranqeu.error.err_code;
		break;	
	    }	    
	    transtarted = TRUE;
	}
	/* escalate the transaction to MST */
	if (qef_cb->qef_auto == QEF_OFF)
	    qef_cb->qef_stat = QEF_MSTRAN;

	/* 
        ** Get a memory stream for reading IIINTEGRITY tuples; we do not need 
	** to allocate memory for IIQRYTEXT and IITREE tuples since we won't 
	** be reading them and qeu_delete() allocates its own memory
        */
	/* Open stream and allocate memory with one effort */
	ulm.ulm_flags = ULM_PRIVATE_STREAM | ULM_OPEN_AND_PALLOC;
        ulm.ulm_psize = sizeof(DB_INTEGRITY);

        if ((status = qec_mopen(&ulm)) != E_DB_OK)
        {
	    error = E_QE001E_NO_MEM;
	    break;
        }

        ituple = (DB_INTEGRITY*) ulm.ulm_pptr;

	/* 
        ** Open the Tree, Query Text and Integrity System 
        ** tables.
        */

	iqeu.qeu_type = QEUCB_CB;
        iqeu.qeu_length = sizeof(QEUCB_CB);
        iqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
        iqeu.qeu_lk_mode = DMT_IX;
        iqeu.qeu_flag = DMT_U_DIRECT;
        iqeu.qeu_access_mode = DMT_A_WRITE;
	iqeu.qeu_mask = 0;
	iqeu.qeu_qual = iqeu.qeu_f_qual = (PTR) 0; 
	iqeu.qeu_qarg = iqeu.qeu_f_qarg = (PTR) 0;
     
	STRUCT_ASSIGN_MACRO(iqeu, qqeu);
	STRUCT_ASSIGN_MACRO(iqeu, tqeu);
	STRUCT_ASSIGN_MACRO(iqeu, iikeyQEU_CB );

        iqeu.qeu_tab_id.db_tab_base  = DM_B_INTEGRITY_TAB_ID;
        iqeu.qeu_tab_id.db_tab_index  = DM_I_INTEGRITY_TAB_ID;
	status = qeu_open(qef_cb, &iqeu);
	if (status != E_DB_OK)
	{
	    error = iqeu.error.err_code;
	    break;
	}
	intg_opened = TRUE;

	qqeu.qeu_tab_id.db_tab_base = DM_B_QRYTEXT_TAB_ID;
	qqeu.qeu_tab_id.db_tab_index = 0L;
	status = qeu_open(qef_cb, &qqeu);
	if (status != E_DB_OK)
	{
	    error = qqeu.error.err_code;
	    break;
	}
	qtext_opened = TRUE;

        tqeu.qeu_tab_id.db_tab_base  = DM_B_TREE_TAB_ID;
        tqeu.qeu_tab_id.db_tab_index  = DM_I_TREE_TAB_ID;
	status = qeu_open(qef_cb, &tqeu);
	if (status != E_DB_OK)
	{
	    error = tqeu.error.err_code;
	    break;
	}
	tbl_opened = TRUE;

	iqeu.qeu_count = 1;
        iqeu.qeu_tup_length = sizeof(DB_INTEGRITY);
	iqeu.qeu_output = &iqef_data;
	iqef_data.dt_next = 0;
        iqef_data.dt_size = sizeof(DB_INTEGRITY);
        iqef_data.dt_data = (PTR) ituple;

            
	qqeu.qeu_count = 1;
        qqeu.qeu_tup_length = sizeof(DB_IIQRYTEXT);
	qqeu.qeu_output = (QEF_DATA *) NULL;


	tqeu.qeu_count = 1;
        tqeu.qeu_tup_length = sizeof(DB_IITREE);
	tqeu.qeu_output = (QEF_DATA *) NULL;

	/* 
        ** Position INTEGRITY table to get all tuples matching the specified
	** table id.  If asked to delete a specific integrity, we will use a
	** qualification function to ensure that DMF only returns IIINTEGRITY
	** tuple(s) corresponding to the desired constraint; otherwise we will
	** read all IIINTEGRITY tuples for this table.
	** 
	** For each IIINTEGRITY tuple returned by DMF (except as described
	** below), we will find the associated query text and tree tuples and
	** delete them.  The one exception is the IIINTEGRITY tuples
	** representing CHECK constraint that renders some attribute(s) "known
	** not nullable".  Since there will be two IIINTEGRITY tuples
	** representing such constraint, we will delete corresponding IITREE and
	** IIQRYTEXT tuples when we find the first (dbi_seq == 1) of these
	** tuples.  When we come across the second tuple, we will destroy it and
	** will not attempt to destroy IITREE or IIQRYTEXT tuples
        */	

	iqeu.qeu_getnext = QEU_REPO;

	iqeu.qeu_key = ikey_ptr_array;

	ikey_ptr_array[0]->attr_number = DM_1_INTEGRITY_KEY;
	ikey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	ikey_ptr_array[0]->attr_value = (char*) 
				&qeuq_cb->qeuq_rtbl->db_tab_base;

	ikey_ptr_array[1]->attr_number = DM_2_INTEGRITY_KEY;
	ikey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	ikey_ptr_array[1]->attr_value = (char*) 
                                 &qeuq_cb->qeuq_rtbl->db_tab_index;
        
	if (qeuq_cb->qeuq_ino != 0)
	{
	    iqeu.qeu_qarg = ( PTR ) &qeuq_cb->qeuq_ino;
	    iqeu.qeu_qual = ( PTR ) qeu_q_integ;
	}

	iqeu.qeu_klen = 2;       

	/*
	** set up qqeu and tqeu to delete IIQRYTEXT and IITREE tuples, 
	** respectively, corresponding to integrity or constraint being deleted
	*/

	tqeu.qeu_klen = 3;       
	tqeu.qeu_qual = ( PTR ) qeu_tr_ts_cmp;
	tqeu.qeu_qarg = ( PTR ) &ituple->dbi_tree;

	tqeu.qeu_key = tkey_ptr_array;

	tkey_ptr_array[0]->attr_number = DM_1_TREE_KEY;
	tkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	tkey_ptr_array[0]->attr_value = 
	    (char *) &qeuq_cb->qeuq_rtbl->db_tab_base;

	tkey_ptr_array[1]->attr_number = DM_2_TREE_KEY;
	tkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	tkey_ptr_array[1]->attr_value = 
	    (char *) &qeuq_cb->qeuq_rtbl->db_tab_index;
        
	tkey_ptr_array[2]->attr_number = DM_3_TREE_KEY;
	tkey_ptr_array[2]->attr_operator = DMR_OP_EQ;
	tkey_ptr_array[2]->attr_value = (char *) &tree_mode;
        
	qqeu.qeu_klen = 3;       

	qqeu.qeu_key = qkey_ptr_array;

	qkey_ptr_array[0]->attr_number = DM_1_QRYTEXT_KEY;
	qkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	qkey_ptr_array[0]->attr_value = 
	    (char *) &ituple->dbi_txtid.db_qry_high_time;

	qkey_ptr_array[1]->attr_number = DM_2_QRYTEXT_KEY;
	qkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	qkey_ptr_array[1]->attr_value = 
	    (char *) &ituple->dbi_txtid.db_qry_low_time;

	qkey_ptr_array[2]->attr_number = DM_3_QRYTEXT_KEY;
	qkey_ptr_array[2]->attr_operator = DMR_OP_EQ;
	qkey_ptr_array[2]->attr_value = (char *) &txt_mode;
        
	/* beginning of loop through IIINTEGRITY */

	for(;;)
	{
	    constraintWithKeys = FALSE;

            status = qeu_get(qef_cb, &iqeu);
	    if (status != E_DB_OK)
	    {
		error = iqeu.error.err_code;
		if (error == E_QE0015_NO_MORE_ROWS && qeuq_cb->qeuq_ino == 0)
		{
			error = E_QE0000_OK;
			status = E_DB_OK;
		}
		break;
	    }

	    /* UNIQUE and REFERENTIAL CONSTRAINTS have iikey tuples */

	    if ( ituple->dbi_consflags & ( CONS_UNIQUE | CONS_REF ) )
	    {	constraintWithKeys = TRUE; }

	    /* 
	    ** if asked to destroy a specific integrity and the tuple describes
	    ** a constraint, tell the user that integrity with specified number
	    ** does no exist
	    */
	    if (   qeuq_cb->qeuq_ino != 0 
		&& integ 
		&& ituple->dbi_consflags != CONS_NONE)
	    {
		error = E_QE0015_NO_MORE_ROWS;
		status = E_DB_ERROR;
		break;
	    }

	    if (iqeu.qeu_getnext == QEU_REPO)
	    {
	        iqeu.qeu_getnext = QEU_NOREPO;
                iqeu.qeu_klen = 0;
	    }

            /*
	    ** if this tuple represents a constraint and we were not asked to 
	    ** drop constraints (as would happen if a user enters 
	    ** DROP INTEGRITY ALL), skip to the next tuple
	    */
	    if (ituple->dbi_consflags != CONS_NONE && !cons)
		continue;

	    /*
	    ** if function called to drop an integrity that applies to a 
	    ** specific table column, indicated by QEU_DROP_COLUMN flag,
	    ** then attribute number of column passed in qeuq_ano must be
	    ** tested against the column bitmap of the integrity tuple.
	    **
	    ** if all bits in the bitmap are set on, then the constraint or
	    ** integrity applies to all columns.  If so, then the integrity
	    ** will not be deleted.
	    **
	    ** If the column bit is set on for the specific column and not
	    ** all bits are set, then the tuple will be deleted if the
	    ** QEU_DROP_CASCADE flag is set.  Otherwise the call is for a
	    ** RESTRICT drop column which can only be allowed if there are no
	    ** column dependent objects.  In this case an error code is
	    ** returned.
	    */

	    if (qeuq_cb->qeuq_flag_mask & QEU_DROP_COLUMN)
	    {
	    	ULM_RCB   ulm;
		DB_TREE_ID tree_id;
		DB_ERROR  err_blk;
		bool	  cascade;
		i2	  attid;
		bool      drop_depobj = FALSE;

		if (constraintWithKeys)
		{
		    /* b108474 (b108638)
		    ** Basically the keydropped flag is used to
		    ** indicate that a unique or primary constraint 
		    ** key has been dropped during a drop column 
		    ** processing.
		    ** 
		    ** Its indication is that dependent referential
		    ** constraints from other tables than the dropped
		    ** column, need to be dropped as well. And because
		    ** those referential constraints do not have the
		    ** originally dropped column referenced, the attid
		    ** will be changed to the referential constraint
		    ** attid to have them dropped.
		    ** In the case of dropping referential constraints
		    ** due to a dependency to a primary constraint 
		    ** que_dintg() is called from que_d_cascade(), 
		    ** because dependency record was found in iidbdepends.
		    ** iidbdepend rows.
		    */

		    if (qeuq_cb->qeuq_keydropped)
		    {
			/* get the key matching with keypos from iikey cat */
			status = getIIKEYtuples( qef_cb, &iikeyQEU_CB,
					iikey_keyPtrArray, &iikey_opened,
					&ituple->dbi_cons_id,  
					&qeuq_cb->qeuq_keypos, &attid, &error );
			if (status != E_DB_OK) break;
			/* 
			** adjust the attribute id  so that the referenced
			** integrity columns are dropped correctly
			*/

			if ((cons && !integ) && /*called from que_d_cascade*/
			    ((ituple->dbi_consflags & CONS_UNIQUE) == 0))
			{
			    qeuq_cb->qeuq_ano = attid; 
			    qeuq_cb->qeuq_keyattid = attid; 
			}
		    }
		    else
		    {
			/* 
			** get the keypos matching attid to be dropped from
			** iikey cat 
			** It is ok to get an error if the attribute being 
			** dropped is not in key.
			*/
			status = getIIKEYtuples( qef_cb, &iikeyQEU_CB,
					iikey_keyPtrArray, &iikey_opened,
					&ituple->dbi_cons_id,  
					&qeuq_cb->qeuq_keypos, 
					(i2 *) &qeuq_cb->qeuq_ano, &error );
		    }
		}

	        if (( (BTtest (qeuq_cb->qeuq_ano,
			(char *) ituple->dbi_columns.db_domset)) &&
		    (BTcount((char *) ituple->dbi_columns.db_domset,
			DB_COL_BITS) != DB_COL_BITS) ) &&
	            ( !(qeuq_cb->qeuq_flag_mask & QEU_DROP_CASCADE) ))
	        {
		  error = E_QE016B_COLUMN_HAS_OBJECTS;
		  status = E_DB_ERROR;
		  break;
	        }
		/* 
		** If it is cascade, check to see if we need to drop
		** the column
		*/
		if ( (BTtest (qeuq_cb->qeuq_ano,
			(char *) ituple->dbi_columns.db_domset)) &&
			(BTcount((char *) ituple->dbi_columns.db_domset,
				DB_COL_BITS) != DB_COL_BITS) ) 
		    drop_depobj = TRUE;
		
		STRUCT_ASSIGN_MACRO(ituple->dbi_tree, tree_id);
		if (tree_id.db_tre_high_time != 0)
		{
		   if ((tree_id.db_tre_high_time == 
					last_tree_id.db_tre_high_time) &&
		       (tree_id.db_tre_low_time == 
					last_tree_id.db_tre_low_time))
		      continue;
		   else
		      STRUCT_ASSIGN_MACRO(tree_id, last_tree_id);
			
		   /*
		   **  Create a stream for iitree tuples
		   **  Call qeu_v_col_drop to fix up iitree entries for this 
		   */
		
		   STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
		   ulm.ulm_streamid = (PTR)NULL;
		   if ((status = qec_mopen(&ulm)) != E_DB_OK)
		   {
	               error = E_QE001E_NO_MEM;
		       status = E_DB_ERROR;
		       break;
		   }

		   STRUCT_ASSIGN_MACRO(tqeu, tempqeu);
		   cascade = (qeuq_cb->qeuq_flag_mask & QEU_DROP_CASCADE) ? 
					TRUE : FALSE;
		   if ((status = qeu_v_col_drop(&ulm, qef_cb,
				&tempqeu, (DB_TAB_ID *) NULL, 
				qeuq_cb->qeuq_rtbl, &tree_id, 
				ituple->dbi_number, (DB_NAME *) NULL,
				(DB_OWN_NAME *) NULL,
				DB_INTG, qeuq_cb->qeuq_ano,
				cascade,   
				&err_blk)) != E_DB_OK)
		   {
		       ulm_closestream(&ulm);
		       error = err_blk.err_code;
		       status = E_DB_ERROR;
		       break;
		   }
		
		   ulm_closestream(&ulm);
		   if (!ref_cons_dropped && ituple->dbi_consflags & CONS_REF)
			ref_cons_dropped = TRUE;

		   if (( constraintWithKeys == TRUE ) && (!drop_depobj))
		   {
			status = replaceIIKEYtuples( qef_cb, &iikeyQEU_CB,
					iikey_keyPtrArray, &iikey_opened,
					&ituple->dbi_cons_id,  
					qeuq_cb->qeuq_ano, &error );
		        if (status != E_DB_OK) 
			   break;
		   }
		   if ((!drop_depobj) && (qeuq_cb->qeuq_ino == 0))
		        continue; 
		   else if ((!drop_depobj) && (qeuq_cb->qeuq_ino != 0))
			    break;
	       }
	       else
	       {
		  /* 
		  ** if add constraints leave it for compilers to
		  ** detect it and give error message. but 
		  ** be sure to invalidate the cache
		  */
		   if (( constraintWithKeys == TRUE ) && (!drop_depobj))
		   {
			status = replaceIIKEYtuples( qef_cb, &iikeyQEU_CB,
					iikey_keyPtrArray, &iikey_opened,
					&ituple->dbi_cons_id,  
					qeuq_cb->qeuq_ano, &error );
		        if (status != E_DB_OK) 
			   break;
		   }
		   if ((!drop_depobj) && (qeuq_cb->qeuq_ino == 0))
		        continue; 
		   else if ((!drop_depobj) && (qeuq_cb->qeuq_ino != 0))
			    break;
	       }
	    }

	    /*
	    ** remember whether this tuple corresponds to a CHECK constraint
	    ** that renders some attribute(s) "known not nullable" (a KNN
	    ** constraint)
	    **
	    ** @FIXME
	    **	in 6.5, we will not be creating a second tuple for CHECK
	    **	constraints that render some attributes "known not nullable"
	    **	unless this CHECK constraint represents NOT NULL constraint
	    **	specified for an attribute.  Accordingly, I am setting
	    **	knn_check_cons to TRUE if the current IIINTEGRITY tuple
	    **	represents a CHECK constraint that renders some attribute(s)
	    **	"known not nullable" AND was created to represent NOT NULL
	    **	specified for an attribute.  This last consdition must be
	    **	removed as soon as we start generating second tuple for all KNN
	    **	(known not nullable) constraints
	    ** @END_OF_FIXME
	    */
	    knn_check_cons =
		(   ituple->dbi_consflags & CONS_CHECK
		 && ituple->dbi_consflags & CONS_KNOWN_NOT_NULLABLE
		 && ituple->dbi_consflags & CONS_NON_NULLABLE_DATATYPE
		);
	    
	    /*
	    ** if we were asked to drop a specific constraint and that
	    ** constraint turns out to be a CHECK constraint that rendered some
	    ** attributes "known not nullable", there should be two tuples
	    ** associated with it.  When we see the first tuple, we must
	    ** remember to look for the second one, and when we find the second
	    ** one, we must remember that we saw both tuples
	    */
	    if (qeuq_cb->qeuq_ino != 0 && knn_check_cons)
	    {
		get_2nd_tuple = !get_2nd_tuple;
	    }

	    /*
	    ** if dropping a CHECK constraint that rendered some attributes
	    ** "known not nullable", we will destroy IITREE and IIQRYTEXT only
	    ** when deleting the IIINTEGRITY tuple with (dbi_seq==1) is found.
	    ** This is purely arbitrary, but I wanted to avoid trying to do it
	    ** for both tuples and then having to figure out wehther the fact
	    ** that there are no IITREE or IIQRYTEXT tuples is good or bad
	    */
	    if (!knn_check_cons || ituple->dbi_seq == 1)
	    {
		/* 
		** trees will be associated ONLY with integrities; delete all
		** tree entries associated with this IIINTEGRITY tuple.
		*/	    

		if (ituple->dbi_consflags == CONS_NONE)
		{
        	    tqeu.qeu_tup_length = sizeof(DB_IITREE);
		    status = qeu_delete(qef_cb, &tqeu);
		    if (status != E_DB_OK)
		    {
			error = tqeu.error.err_code;
			break;
		    }
		}

		/* 
		** Now delete IIQRYTEXT tuples for the query id associated with
		** this IIINTEGRITY tuple AND mode (DB_INTG).
		*/	    

        	qqeu.qeu_tup_length = sizeof(DB_IIQRYTEXT);
		status = qeu_delete(qef_cb, &qqeu);
		if (status != E_DB_OK)
		{
		    error = qqeu.error.err_code;
		    break;
		}
	    }
	    
	    /* delete IIKEY tuples for CONSTRAINTS with keys */

	    if ( constraintWithKeys == TRUE )
	    {
		status = deleteIIKEYtuples( qef_cb, &iikeyQEU_CB,
				iikey_keyPtrArray, &iikey_opened,
				&ituple->dbi_cons_id, &error );
		if ( status != E_DB_OK )	{ break; }
		qeuq_cb->qeuq_keydropped = TRUE;
	    }	/* end deleting IIKEY tuples */

	    /* 
	    ** Now delete the integrity tuple.
	    */

            iqeu.qeu_tup_length = sizeof(DB_INTEGRITY);
	    status = qeu_delete(qef_cb, &iqeu);
	    if (status != E_DB_OK)
	    {
		error = iqeu.error.err_code;
		break;
	    }

	    /* 
	    ** remember whether we have deleted at least one REFERENCES 
	    ** constraint, in which case we will have to invoke qeu_dbdelete() 
	    ** to delete IIDBDEPENDS tuples describing objects on which newly 
	    ** dropped constraint(s) used to depend
	    */
	    if (!ref_cons_dropped && ituple->dbi_consflags & CONS_REF)
		ref_cons_dropped = TRUE;
		
	    /* If asked to destroy a specific constraint or integrity and
	    **   - either this was not a CHECK constraint that rendered some
	    **	   attributes "known not nullable" or
	    **	 - we have deleted both tuples associated with a CHECK
	    **	   constraint that rendered some attributes "known not nullable"
	    */

	    /* b108474 (b108636)
	    ** During drop column cascade processing, iidbdepends record of a
	    ** dropped referenced constraint need to be dropped one by one with
	    ** the according integration number. 
	    ** Main point here is that during drop column processing not all the
	    ** referential constraint of a table will be affected, so we need to
	    ** remove it specifically.	
	    */

	    if (qeuq_cb->qeuq_flag_mask & QEU_DROP_COLUMN &&
		  ref_cons_dropped &&
		  ituple->dbi_consflags & CONS_REF)
	    {
	    	  DB_ERROR            err_blk;

		  status = qeu_dbdelete(qef_cb, qeuq_cb, qeuq_cb->qeuq_rtbl,
		  (i4) DB_CONS, (i4) ituple->dbi_number, &err_blk);
		  if ( status != E_DB_OK )	{ break; }

		  ref_cons_dropped = FALSE;
	    }


	    if (qeuq_cb->qeuq_ino != 0 && !get_2nd_tuple)
		break;
	}
	
	if (status != E_DB_OK)
	    break;

	/*
	** if at least one REFERENCES constraint has been dropped, we need to 
	** invoke qeu_dbdelete() to delete IIDBDEPENDS tuples describing 
	** dependence of newly dropped REFERENCES constraint(s) on other DBMS 
	** objects
	*/
	if (ref_cons_dropped)
	{
	    DB_ERROR		err_blk;

	    status = qeu_dbdelete(qef_cb, qeuq_cb, qeuq_cb->qeuq_rtbl,
		(i4) DB_CONS, (i4) qeuq_cb->qeuq_ino, &err_blk);
	    if (DB_FAILURE_MACRO(status))
	    {
		error = err_blk.err_code;
		break;
	    }
	}

	/*
	** Unless we were asked to drop all integrities AND all constraints (as
	** would happen when a table is being dropped), update the timestamp in
	** the relation table to indicate that one or more  integrities or
	** constraints have been dropped.
	** If any other integrities or constraints still exist on the table we
	** will set the integrities bit in relstat; otherwise, we will update
	** the relation table to indicate that there are no more constraints on
	** the table.
	*/

	if (qeuq_cb->qeuq_ino != 0 || !integ || !cons ||
	    qeuq_cb->qeuq_flag_mask & QEU_DROP_COLUMN)
	{
	    /* Find at least one IIINTEGRITY tuple that has the same table id */
	    iqeu.qeu_getnext = QEU_REPO;
	    iqeu.qeu_klen = 2;       

	    /*
	    ** if we were asked to destroy a specific integrity or constraint,
	    ** we must remember to reset the qualification function ptr and the
	    ** arg ptr since they would be set above to ensure that we only get
	    ** tuples corresponding to the integrity or constraint being
	    ** destroyed
	    */
	    if (qeuq_cb->qeuq_ino != 0)
	    {
		iqeu.qeu_qual = 0;
		iqeu.qeu_qarg = 0;
	    }

	    status = qeu_get(qef_cb, &iqeu);
	    if (status != E_DB_OK )
	    {
		error = iqeu.error.err_code;
		if ( error == E_QE0015_NO_MORE_ROWS )
		{
		    /* No more Integrities left on the table */
		    error = E_QE0000_OK;
		    status = E_DB_OK;	
		    char_array[0].char_value = DMT_C_OFF;
		}
		else
		{ 
		    break;
		}
	    }
	    else
	    {
		    /*
		    ** There are still integrities and/or constraints on this
		    ** table
		    */
		    char_array[0].char_value = DMT_C_ON;
	    }
			    
	    dmt_cb.dmt_flags_mask = 0; 
	    dmt_cb.dmt_db_id = qeuq_cb->qeuq_db_id;
	    dmt_cb.dmt_char_array.data_in_size = sizeof(DMT_CHAR_ENTRY);
	    dmt_cb.dmt_char_array.data_address = (PTR) char_array;
	    char_array[0].char_id = DMT_C_INTEGRITIES;
	    dmt_cb.length = sizeof(DMT_CB);
	    dmt_cb.type = DMT_TABLE_CB;
	    dmt_cb.dmt_id.db_tab_base = qeuq_cb->qeuq_rtbl->db_tab_base;
	    dmt_cb.dmt_id.db_tab_index = qeuq_cb->qeuq_rtbl->db_tab_index;
	    dmt_cb.dmt_tran_id = qef_cb->qef_dmt_id;

	    status = dmf_call(DMT_ALTER, &dmt_cb);
	    if (status != E_DB_OK)
	    {
		error = dmt_cb.error.err_code;
		break;
	    }	  
	}

	if ( Qef_s_cb->qef_state & QEF_S_C2SECURE )
	{
	    DB_ERROR	e_error;
	    DB_TAB_ID	tabid;
	    tabid.db_tab_base=
		       qeuq_cb->qeuq_rtbl->db_tab_base;
	    tabid.db_tab_index=
		       qeuq_cb->qeuq_rtbl->db_tab_index;
	    /*
	    ** We need to audit successful drop of integrity
	    */
	    status = qeu_secaudit( FALSE, qef_cb->qef_ses_id, 
		qeuq_cb->qeuq_tabname.db_tab_name, &qef_cb->qef_user,
		sizeof(qeuq_cb->qeuq_tabname.db_tab_name), SXF_E_TABLE,
		I_SX2036_INTEGRITY_DROP, SXF_A_SUCCESS | SXF_A_CONTROL,
		&e_error);
	    if (status != E_DB_OK)
	    {
	        error = e_error.err_code;
	    	break;
	    }
	}

	/* Close off all the tables. */

	status = qeu_close(qef_cb, &qqeu);    
	if (status != E_DB_OK)
	{
	    error = qqeu.error.err_code;
	    break;
	}
	qtext_opened = FALSE;
	status = qeu_close(qef_cb, &iqeu);    
	if (status != E_DB_OK)
	{
	    error = iqeu.error.err_code;
	    break;
	}
	intg_opened = FALSE;
	status = qeu_close(qef_cb, &tqeu);    
	if (status != E_DB_OK)
	{
	    error = tqeu.error.err_code;
	    break;
	}
	tbl_opened = FALSE;

	if ( iikey_opened )
	{
	    status = qeu_close(qef_cb, &iikeyQEU_CB );    
	    if (status != E_DB_OK)
	    {
	        error = iikeyQEU_CB.error.err_code;
	        break;
	    }
	    iikey_opened = FALSE;
	}	/* end if IIKEY opened */

	/* 
        ** Close the input stream created for the integrity tuples.
        */

	ulm_closestream(&ulm);

	
	if (transtarted)
	{
	    status = qeu_etran(qef_cb, &tranqeu);
	    if (status != E_DB_OK)
	    {	
		error = tranqeu.error.err_code;
		break;
	    }
	}

	exit_loop = TRUE;
    } /* end for */

    if (status == E_DB_OK)
    {
	/* SUCCESS - we are done */
	qeuq_cb->error.err_code = E_QE0000_OK;
	return (E_DB_OK);
    }

    /* call qef_error to handle error messages */

    (VOID) qef_error(error, 0L, status, &error, &qeuq_cb->error, 0);
    
    /* Close off all the tables. */

    if (ulm.ulm_streamid != (PTR)NULL)
    {
	/* 
        ** Close the input stream created for the integrity tuples.
        */

	(VOID) ulm_closestream(&ulm);
    }

    /* Close off all the tables. */

    if (qtext_opened)
    {
	local_status = qeu_close(qef_cb, &qqeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(qqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if ( iikey_opened )
    {
	local_status = qeu_close(qef_cb, &iikeyQEU_CB );    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error( iikeyQEU_CB.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (intg_opened)
    {
	local_status = qeu_close(qef_cb, &iqeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(iqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (tbl_opened)
    {
	local_status = qeu_close(qef_cb, &tqeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(tqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }
    if (transtarted)
    {
	local_status = qeu_atran(qef_cb, &tranqeu);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(tranqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    return (status);
}

/*{
** Name: deleteIIKEYtuples - drop dependent IIKEY tuples
**
** Description:
**	This routine is passed a CONSTRAINT id.  This function deletes
**	all IIKEY tuples with that CONSTRAINT id.
**
**
** Inputs:
**      qef_cb                  qef session control block
**      iikeyQEU_CB		control block for qeu operations on IIKEY
**	iikey_keyPtrArray	array of pointers to keys on IIKEY
**	iikey_opened		TRUE if IIKEY already opened. FALSE otherwise.
**	constraintID		CONSTRAINT ID whose corresponding IIKEY
**				tuples should be deleted.
**
** Outputs:
**      iikeyQEU_CB
**	    .error.err_code	one of the following
**				E_QE0002_INTERNAL_ERROR
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0022_ABORTED
**				E_QE0023_INVALID_QUERY
**
**	iikey_keyPtrArray	keys are pointed at the CONSTRAINT ID
**	iikey_opened		set to TRUE if IIKEY successfully opened
**
**	error			error returned by qeu_open of qeu_delete
**
**	Returns:
**	    { E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	21-jun-93 (rickh)
**	    Created.
*/

static DB_STATUS
deleteIIKEYtuples(
    QEF_CB		*qef_cb,
    QEU_CB		*iikeyQEU_CB,
    DMR_ATTR_ENTRY	**iikey_keyPtrArray,
    bool		*iikey_opened,
    DB_CONSTRAINT_ID	*constraintID,
    i4		*error
)
{
    DB_STATUS	    status = E_DB_OK;

    /* open IIKEY if we haven't done so already */

    if ( *iikey_opened == FALSE )
    {
	iikeyQEU_CB->qeu_tab_id.db_tab_base = DM_B_IIKEY_TAB_ID;
	iikeyQEU_CB->qeu_tab_id.db_tab_index = 0L;
	status = qeu_open(qef_cb, iikeyQEU_CB );
	if (status != E_DB_OK)
	{
	    *error = iikeyQEU_CB->error.err_code;
	    return( status );
	}
	*iikey_opened = TRUE;

	iikeyQEU_CB->qeu_count = 1;
        iikeyQEU_CB->qeu_tup_length = sizeof( DB_IIKEY );
	iikeyQEU_CB->qeu_output = (QEF_DATA *) NULL;

	iikeyQEU_CB->qeu_klen = 2;       
	iikeyQEU_CB->qeu_key = iikey_keyPtrArray;

	iikey_keyPtrArray[0]->attr_number = DM_1_IIKEY_KEY;
	iikey_keyPtrArray[0]->attr_operator = DMR_OP_EQ;

	iikey_keyPtrArray[1]->attr_number = DM_2_IIKEY_KEY;
	iikey_keyPtrArray[1]->attr_operator = DMR_OP_EQ;
        
	iikeyQEU_CB->qeu_qual = iikeyQEU_CB->qeu_f_qual = 0;
	iikeyQEU_CB->qeu_qarg = iikeyQEU_CB->qeu_f_qarg = 0;

    }	/* end opening IIKEY */

    /* position IIKEY on the CONSTRAINT id from this integrity tuple */

    iikey_keyPtrArray[0]->attr_value = (char *) &constraintID->db_tab_base;
    iikey_keyPtrArray[1]->attr_value = (char *) &constraintID->db_tab_index;

    /* at last we may delete all IIKEY tuples with this CONSTRAINT id */

    status = qeu_delete( qef_cb, iikeyQEU_CB );
    if ( status != E_DB_OK)	{ *error = iikeyQEU_CB->error.err_code; }

    return( status );
}

/*{
** Name: replaceIIKEYtuples - replace dependent IIKEY tuples attribute
**
** Description:
**	This routine is passed a CONSTRAINT id.  This function replaces
**	the attids of all IIKEY tuples with that CONSTRAINT id.
**
**
** Inputs:
**      qef_cb                  qef session control block
**      iikeyQEU_CB		control block for qeu operations on IIKEY
**	iikey_keyPtrArray	array of pointers to keys on IIKEY
**	iikey_opened		TRUE if IIKEY already opened. FALSE otherwise.
**	constraintID		CONSTRAINT ID whose corresponding IIKEY
**				tuples should be deleted.
**	attidfrom		attid from which the attid's will be 
**				decremented.
**
** Outputs:
**      iikeyQEU_CB
**	    .error.err_code	one of the following
**				E_QE0002_INTERNAL_ERROR
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0022_ABORTED
**				E_QE0023_INVALID_QUERY
**
**	iikey_keyPtrArray	keys are pointed at the CONSTRAINT ID
**	iikey_opened		set to TRUE if IIKEY successfully opened
**
**	error			error returned by qeu_open or qeu_replace
**
**	Returns:
**	    { E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	21-feb-97 (nanpr01)
**	    Created. For alter table drop column.
**	02-sep-02 (gygro01) ingsrv1859
**	    b108474 (b108638) If IIKEY is already opened que_klen
**	    does not get reinitialised. If it has been previously
**	    set to 0 will result in a full tablescan and wrong
**	    iikey attribute id seting and deletions.
*/

static DB_STATUS
replaceIIKEYtuples(
    QEF_CB		*qef_cb,
    QEU_CB		*iikeyQEU_CB,
    DMR_ATTR_ENTRY	**iikey_keyPtrArray,
    bool		*iikey_opened,
    DB_CONSTRAINT_ID	*constraintID,
    i2			attidfrom,
    i4		*error
)
{
    GLOBALREF QEF_S_CB *Qef_s_cb;
    DB_STATUS	    status = E_DB_OK;
    QEF_DATA	    iikey_data;
    DB_IIKEY	    *ituple;
    ULM_RCB	    ulm;

    /* open IIKEY if we haven't done so already */

    if ( *iikey_opened == FALSE )
    {
	iikeyQEU_CB->qeu_tab_id.db_tab_base = DM_B_IIKEY_TAB_ID;
	iikeyQEU_CB->qeu_tab_id.db_tab_index = 0L;
	status = qeu_open(qef_cb, iikeyQEU_CB );
	if (status != E_DB_OK)
	{
	    *error = iikeyQEU_CB->error.err_code;
	    return( status );
	}
	*iikey_opened = TRUE;

	iikeyQEU_CB->qeu_count = 1;
        iikeyQEU_CB->qeu_tup_length = sizeof( DB_IIKEY );
	iikeyQEU_CB->qeu_output = (QEF_DATA *) NULL;

	iikeyQEU_CB->qeu_klen = 2;       
	iikeyQEU_CB->qeu_key = iikey_keyPtrArray;

	iikey_keyPtrArray[0]->attr_number = DM_1_IIKEY_KEY;
	iikey_keyPtrArray[0]->attr_operator = DMR_OP_EQ;

	iikey_keyPtrArray[1]->attr_number = DM_2_IIKEY_KEY;
	iikey_keyPtrArray[1]->attr_operator = DMR_OP_EQ;
        
	iikeyQEU_CB->qeu_qual = iikeyQEU_CB->qeu_f_qual = 0;
	iikeyQEU_CB->qeu_qarg = iikeyQEU_CB->qeu_f_qarg = 0;

    }	/* end opening IIKEY */

    /* position IIKEY on the CONSTRAINT id from this integrity tuple */

    iikey_keyPtrArray[0]->attr_value = (char *) &constraintID->db_tab_base;
    iikey_keyPtrArray[1]->attr_value = (char *) &constraintID->db_tab_index;

    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
    /* Open stream and allocate memory with one effort */
    ulm.ulm_flags = ULM_PRIVATE_STREAM | ULM_OPEN_AND_PALLOC;
    ulm.ulm_psize = ulm.ulm_blocksize = sizeof(DB_IIKEY);

    if ((status = qec_mopen(&ulm)) != E_DB_OK)
    {
	*error = E_QE001E_NO_MEM;
	return(status);
    }
 
    ituple = (DB_IIKEY *) ulm.ulm_pptr;

    /* b108474 (b108638) reinitialisation of que_klen
    ** to avoid full tablescans.
    */
    iikeyQEU_CB->qeu_klen = 2;
    iikeyQEU_CB->qeu_count = 1;
    iikeyQEU_CB->qeu_tup_length = sizeof(DB_IIKEY);
    iikeyQEU_CB->qeu_output = iikeyQEU_CB->qeu_input = &iikey_data;
    iikeyQEU_CB->qeu_getnext = QEU_REPO;
    iikey_data.dt_next = 0;
    iikey_data.dt_size = sizeof(DB_IIKEY);
    iikey_data.dt_data = (PTR) ituple;


    for (; ;)
    {
        status = qeu_get( qef_cb, iikeyQEU_CB );
	if (status != E_DB_OK)
	{
	    if (iikeyQEU_CB->error.err_code == E_QE0015_NO_MORE_ROWS)
		status = E_DB_OK;
	    else 
	    	*error = iikeyQEU_CB->error.err_code;
	    break;
	}
	iikeyQEU_CB->qeu_getnext = QEU_NOREPO;

	if (ituple->dbk_column_id.db_att_id == attidfrom)
	{
	    iikeyQEU_CB->qeu_klen = 0;
	    status = qeu_delete(qef_cb, iikeyQEU_CB);
 
	    if (status != E_DB_OK)
	    {
		*error = iikeyQEU_CB->error.err_code; 
		break;
	    }
	}
	else if (ituple->dbk_column_id.db_att_id > attidfrom)
	{
	    iikeyQEU_CB->qeu_klen = 0;
	    ituple->dbk_column_id.db_att_id--;
	    status = qeu_replace(qef_cb, iikeyQEU_CB);
 
	    if (status != E_DB_OK)
	    {
		*error = iikeyQEU_CB->error.err_code; 
		break;
	    }
	}
    }
    ulm_closestream(&ulm);

    return( status );
}

/*{
** Name: getIIKEYtuples - get dependent IIKEY tuples attribute
**
** Description:
**	This routine is passed a CONSTRAINT id.  This function gets 
**	the attids of the dependent col from IIKEY tuples .
**
**
** Inputs:
**      qef_cb                  qef session control block
**      iikeyQEU_CB		control block for qeu operations on IIKEY
**	iikey_keyPtrArray	array of pointers to keys on IIKEY
**	iikey_opened		TRUE if IIKEY already opened. FALSE otherwise.
**	constraintID		CONSTRAINT ID whose corresponding IIKEY
**				tuples should be deleted.
**	keypos			key position
**	attidfrom		attid of the key.
**
** Outputs:
**      iikeyQEU_CB
**	    .error.err_code	one of the following
**				E_QE0002_INTERNAL_ERROR
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0022_ABORTED
**				E_QE0023_INVALID_QUERY
**
**	iikey_keyPtrArray	keys are pointed at the CONSTRAINT ID
**	iikey_opened		set to TRUE if IIKEY successfully opened
**
**	error			error returned by qeu_open or qeu_replace
**
**	Returns:
**	    { E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	26-mar-97 (nanpr01)
**	    Created. For alter table drop column.
**	02-sep-02 (gygro01) ingsrv1859
**	    b108474 (b108638) If IIKEY is already opened que_klen
**	    does not get reinitialised. If it has been previously
**	    set to 0 will result in a full tablescan and wrong
**	    iikey attribute id seting and deletions.
*/


static DB_STATUS
getIIKEYtuples(
    QEF_CB		*qef_cb,
    QEU_CB		*iikeyQEU_CB,
    DMR_ATTR_ENTRY	**iikey_keyPtrArray,
    bool		*iikey_opened,
    DB_CONSTRAINT_ID	*constraintID,
    i2			*keypos,
    i2			*attid,
    i4		*error
)
{
    GLOBALREF QEF_S_CB *Qef_s_cb;
    DB_STATUS	    status = E_DB_OK;
    QEF_DATA	    iikey_data;
    DB_IIKEY	    *ituple;
    ULM_RCB	    ulm;

    /* open IIKEY if we haven't done so already */

    if ( *iikey_opened == FALSE )
    {
	iikeyQEU_CB->qeu_tab_id.db_tab_base = DM_B_IIKEY_TAB_ID;
	iikeyQEU_CB->qeu_tab_id.db_tab_index = 0L;
	status = qeu_open(qef_cb, iikeyQEU_CB );
	if (status != E_DB_OK)
	{
	    *error = iikeyQEU_CB->error.err_code;
	    return( status );
	}
	*iikey_opened = TRUE;

	iikeyQEU_CB->qeu_count = 1;
        iikeyQEU_CB->qeu_tup_length = sizeof( DB_IIKEY );
	iikeyQEU_CB->qeu_output = (QEF_DATA *) NULL;

	iikeyQEU_CB->qeu_klen = 2;       
	iikeyQEU_CB->qeu_key = iikey_keyPtrArray;

	iikey_keyPtrArray[0]->attr_number = DM_1_IIKEY_KEY;
	iikey_keyPtrArray[0]->attr_operator = DMR_OP_EQ;

	iikey_keyPtrArray[1]->attr_number = DM_2_IIKEY_KEY;
	iikey_keyPtrArray[1]->attr_operator = DMR_OP_EQ;
        
	iikeyQEU_CB->qeu_qual = iikeyQEU_CB->qeu_f_qual = 0;
	iikeyQEU_CB->qeu_qarg = iikeyQEU_CB->qeu_f_qarg = 0;

    }	/* end opening IIKEY */

    /* position IIKEY on the CONSTRAINT id from this integrity tuple */

    iikey_keyPtrArray[0]->attr_value = (char *) &constraintID->db_tab_base;
    iikey_keyPtrArray[1]->attr_value = (char *) &constraintID->db_tab_index;

    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
    /* Open stream and allocate memory with one effort */
    ulm.ulm_flags = ULM_PRIVATE_STREAM | ULM_OPEN_AND_PALLOC;
    ulm.ulm_psize = ulm.ulm_blocksize = sizeof(DB_IIKEY);

    if ((status = qec_mopen(&ulm)) != E_DB_OK)
    {
	*error = E_QE001E_NO_MEM;
	return(status);
    }
 
    ituple = (DB_IIKEY *) ulm.ulm_pptr;

    /* b108474 (b108638) reinitialisation of que_klen
    ** to avoid full tablescans.
    */
    iikeyQEU_CB->qeu_klen = 2; 
    iikeyQEU_CB->qeu_count = 1;
    iikeyQEU_CB->qeu_tup_length = sizeof(DB_IIKEY);
    iikeyQEU_CB->qeu_output = iikeyQEU_CB->qeu_input = &iikey_data;
    iikeyQEU_CB->qeu_getnext = QEU_REPO;
    iikey_data.dt_next = 0;
    iikey_data.dt_size = sizeof(DB_IIKEY);
    iikey_data.dt_data = (PTR) ituple;


    for (; ;)
    {
        status = qeu_get( qef_cb, iikeyQEU_CB );
	if (status != E_DB_OK)
	{
	    *error = iikeyQEU_CB->error.err_code;
	    break;
	}
	iikeyQEU_CB->qeu_getnext = QEU_NOREPO;

	if (*keypos == 0)
	{
	    if (ituple->dbk_column_id.db_att_id == *attid)
	    {
		*keypos = ituple->dbk_position;
		break;
	    }
	}
	if (ituple->dbk_position == *keypos)
	{
	    *attid = ituple->dbk_column_id.db_att_id;
	    break;
	}
    }
    ulm_closestream(&ulm);

    return( status );
}

/*{
** Name: QEU_D_CASCADE 	- perform cascading destruction of a DBMS object
**
** Description:
**	Destroy the specified object and all objects dependent on it that would
**	otherwise become abandoned.  This function will handle destruction of
**	tables/indices, views, dbprocs, and dbevents if the object is being
**	destroyed with imlicit or explicit <drop behaviour> of CASCADE.
**
**	Let X be the object being destroyed
**
**	If X is a table or a view,
**	    let T represent a set of DBMS objects consisting of X and all
**	    views which directly or indirectly depend on X;
**	    let P represent a set of DBMS objects consisting of database
**	    procedures which directly or indirectly depend on some object(s)
**	    in T
**
**	    All objects in T will be destroyed.  In addition, for every
**	    object T' in T we will
**	      - destroy all non-GRANT compatible permits dependent on T',
**	      - destroy qrymod objects (permits, integrities, comments, etc.)
**	        defined on T',
**	      - destroy tuples in IIDBDEPENDS for all DBMS objects dependent
**	        on T', 
**	      - if T' is a view we will
**		- delete tuples in IITREE representing its tree,
**		- delete tuples in IIQRYTEXT representing its text, and
**		- delete tuples in IIDBDEPENDS defining T' as a dependent of
**		  base tables and views
**		- delete tuples in IIPRIV representing descriptors created
**		  for T' or privileges on T'
**
**	    All objects in P will be marked as "dormant" (IIPROCEDURE.dbp_mask1
**	    will be modified to indicate that they are neither grantable nor
**	    active.)  In addition for every object P' in P we will
**	      - destroy permits on P' since they would become abandoned,
**	      - delete tuples in IIDBDEPENDS for all dbprocs dependent on P',
**	      - delete tuples in IIDBDEPENDS defining P' as a dependent of other
**	        DBMS objects (tables, views, dbprocs, and dbevents)
**	      - delete tuples in IIPRIV representing a descriptor created for P'
**
**	If X is a database procedure,
**	    let P represent a set of DBMS objects consisting of X and all
**	    database procedures which directly or indirectly depend on X
**
**	    X will be destroyed and all objects in P - {X} will be marked as
**	    "dormant" (IIPROCEDURE.dbp_mask1 will be modified to indicate that
**	    they are neither grantable nor active.)  In addition for every
**	    object P' in P - {X} we will
**	      - destroy permits on P' since they would become abandoned,
**	      - delete tuples in IIDBDEPENDS for all dbprocs dependent on P',
**	      - delete tuples in IIDBDEPENDS defining P' as a dependent of other
**	        DBMS objects (tables, views, dbprocs, and dbevents)
**	      - delete tuples in IIPRIV representing a descriptor created for P'
**
**	If X is a dbevent,
**	    let P represent a set of DBMS objects consisting of X and
**	    all database procedures which directly or indirectly depend on X
**
**	    X will be destroyed.  In addition we will
**	      - destroy IIQRYTEXT tuple associated with X
**	      - destroy any permits on X
**	      
**	    All objects in P - {X} will be marked dormant.  In addition for
**	    every object P' in P - {X} we will
**	      - destroy permits on P' since they would become abandoned,
**	      - delete tuples in IIDBDEPENDS for all dbprocs dependent on P',
**	      - delete tuples in IIDBDEPENDS defining P' as a dependent of other
**	        DBMS objects (tables, views, dbprocs, and dbevents)
**	      - delete tuples in IIPRIV representing a descriptor created for P'
**	    
**	This is a cascading destruction.  Since it is not known how many DBMS
**	objects are directly and indirectly dependent on the object specified,
**	a decision was made to make once pass to determine all dependent objects
**	which require destruction (views) or being marked as dormant (dbprocs.)
**	Then each of the dependents and their associated catalog entries will be
**	destroed as described above.
**
**	To insure this unbounded list of dependents can be built it is placed 
**	in a temporary table.  Once all dependents are found the table is 
**	rescanned.  Each tuple contains the table id and type of an object to
**	be destroyed and, for views, the id of IIQRYTEXT tuple for that view.
**
** Inputs:
**      qef_cb                  qef session control block
**      qeuq_cb
**	    .qeuq_eflag	        designate error handling semantics
**				for user errors.
**		QEF_INTERNAL	return error code.
**		QEF_EXTERNAL	send message to user.
**	    .qeuq_rtbl		id of view, table, dbevent, dbproc, or synonym
**          .qeuq_qid           query text id if view, dbevent or dbproc;
**				0 otherwise
**	    .qeuq_flag_mask	bits of information
**		QEU_FORCE_QP_INVALIDATION
**				force invalidation of QPs which may be
**				affected by destruction of specified object and
**				any objects that depend on it
**  // if obj_type == DB_EVENT
**	    .qeuq_uld_tup
**		.dt_data	IIEVENT tuple describing the dbevent
**		  .dbe_name	Event name.
**		  .dbe_owner	Event owner.
**  // else if obj_type == DB_DBP
**	    .qeuq_dbp_tup
**		.dt_data        IIPROCEDURE tuple describing the dbproc
**		  .db_dbpname	name of the procedure to be destroyed
**		  .db_owner	owner of the procedure to be destroyed
**  // else if obj_type == DB_SEQUENCE
**	    .qeuq_uld_tup
**		.dt_data	IISEQUENCE tuple describing the sequence
**		  .dbs_name	Sequence name.
**		  .dbs_owner	Sequence owner.
**  // endif
**	    .qeuq_flag_mask	useful bits of information
**		QEU_DROP_TEMP_TABLE	dropping a temporary table
**		QEU_DROP_CASCADE	cascading destruction
**		QEU_DROP_COLUMN		drop objs assoc with specific column
**	    .qeuq_db_id		database id
**	    .qeuq_d_id		dmf session id
**	    .qeuq_ano		column attr number for drop column.
**	    .qeuq_ino		if dropping a constraint, this will contain
**				integrity number for the constraint
**	obj_type		type of object being destroyed
**	    DB_TABLE		destroying a base table
**	    DB_INDEX		destroying an index
**	    DB_VIEW		destroying a view
**	    DB_DBP		destroying a database procedure
**	    DB_EVENT		destroying a dbevent
**	    DB_SEQUENCE		destroying a sequence
**	    DB_CONS		destroying an integrity constraint
**	    DB_SYNONYM		reclassify dbprocs dependent on a synonym which 
**				was dropped as dormant (caller handles 
**				destruction of the synonym itself)
**
**	tbl_entry		if obj_type in (DB_TABLE, DB_VIEW, DB_INDEX),
**				    address of a DMT_TBL_ENTRY structure 
**				    describing object to be dropped;
**				otherwise
**				    NULL
**
** Outputs:
**      qeuq_cb
**	    .error.err_code	one of the following
**				E_QE0002_INTERNAL_ERROR
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0022_ABORTED
**				E_QE0023_INVALID_QUERY
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**
** NOTE: history comments prior to 6/23/92 have been copied from qeu_dview()
** hsitory section since this function started up as an almost perfect copy of
** qeu_dview() (qeu_dview() will call this function so it will lose most of its
** body)
**	16-jun-86 (daved)
**          written
**      22-jul-86 (jennifer)
**          Initially coded.
**      20-aug-86 (jennifer)
**	    Changed all transaction calls from QET_xxxx
**          to QEU_xxx.  
**	24-sep-86 (daved)
**	    corrected various spelling mistakes and changed the test
**	    when deleting the iiqrytext entry to not care if delete
**	    deleted anything when not deleting a view.
**	    Also use DMT_DBMS_REQUEST when creating the temporary relation.
**	23-dec-86 (daved) 
**	    close off tables, even in error condition
**	10-sep-87 (puree)
**	    call qef_error to issue error message before cleaning up.
**	06-nov-87 (puree)
**	    change error from qet_escalate to QE0005_CANNOT_ESCALATE.
**      10-dec-87 (puree)
**          Converted all ulm_palloc to qec_palloc
**	20-jan-88 (puree)
**	    Convert all qet_escalate calls to in-line codes.
**	10-feb-88 (puree)
**	    Convert ulm_openstream qec_mopen.  Rename qec_palloc to 
**	    qec_malloc.
**	09-mar-88 (puree)
**	    Change E_QE0027_BAD_QUERY_ID to E_QE0023_INVALID_QUERY to 
**	    allow retry by SCF when query text for a view is not found.
**	12-oct-88 (puree)
**	    Modified parameters to qeu_dprot.  Also handle error from qeu_dprot
**	08-nov-89 (neil)
**	    Clear qeuq_flag_mask before calling qeu_dprot to avoid collisions
**	    with other protection deletions.
**	02-jan-90 (nancy)
**	    Check for qef error mapping of NONEXISTENT TABLE after call to
**	    qeu_dprot since error code is already mapped and this is an error
**	    that can be ignored. (bug 9208 -- caused E_PS0903 error when 
**	    dropping table that had several views on it)
**	2-may-90 (andre)
**	    drop synonyms defined on tables,viewsa, indices being dropped.
**	04-may-90 (carol)
**	    Add cascading drops of comments.
**	21-jun-90 (linda)
**	    Change the check for whether or not the object is a view from
**		if ((qeuq_cb->qeuq_qid.db_qry_high_time  != 0) ||
**		    qeuq_cb->qeuq_qid.db_qry_low_time != 0))
**	    to a check based on a new field in qeuq_cb, qeuq_status_mask,
**	    from dmt_tbl_entry, telling us what kind of object we have.
**	    In particular we want to check for a gateway table, which has
**	    non-zero db_qry_high_time and db_qry_low_time, but must be
**	    treated differently from a view.
**	10-dec-90 (neil)
**	    Clear qeuq_flag_mask before calling qeu_dprot to avoid collisions
**	    with other protection deletions.
**	14-jan-91 (andre)
**	    use the newly defined function, qeu_dqrymod_objects(), to drop
**	    qrymod objects defined on a table or view being dropped.
**	10-feb-1992 (bryanp)
**	    Changed dmt_location from a DB_LOC_NAME to a DM_DATA.
**	    Initialize dmu_char_array fields in dmu_cb for DMU_DESTROY_TABLE.
**	09-jun-92 (andre)
**	    - when dropping a table/view, in addition to dropping tables
**	      and permits dependent on it as recorded in IIDBDEPENDS, we
**	      also need to:
**		- scan IIDBDEPENDS looking for dbprocs which depend on the
**		  object being dropped and remember to mark them as "dormant"
**		- if during rescanning the temp table we come across a dbproc
**		  entry, we must check IIDBDEPENDS for other dbprocs dependent
**		  on this dbproc
**		- scan IIPRIV for descriptions of privileges on the object
**		  being dropped on which other objects (priviilege descriptor 0)
**		  depend and enter them into the temp table
**	    - for views being destroyed, we must remember to destroy
**	      IIPRIV tuples constituting descriptors of privileges on which
**	      they or some privileegs on them depend;
**	    - for dbprocs being marked as dormant because some object on which
**	      they depend is being destroyed or marked as dormant, we must
**	      destroy IIPRIV tuples constituting descriptors of privileges on
**	      which they depend;
**	16-jun-92 (andre)
**	    add the object type to the temp. table to be able to distinguish
**	    views from dbprocs
**
*************
**
**	23-jun-92 (andre)
**	    created qeu_d_cascade() based on code in qeu_dview()
**	24-jun-92 (andre)
**	    made changes to handle cascading destruction of a dbevent.
**	24-jun-92 (andre)
**	    made changes to handle cascading destruction of a database procedure
**	16-sep-92 (andre)
**	    when entering rows into the temp table based on info found in
**	    IIPRIV, translate DB_QUEL_VIEW into DB_VIEW since when destroying
**	    views, we need not differentiate between SQL and QUEL views
**	    (besides, it makes for a slghtly more elegant code)
**	17-sep-92 (andre)
**	    DB_QUEL_VIEW will no longer be used as an object type in IIPRIV (as
**	    a matter of fact the constant has been undefined).
**	05-feb-93 (andre)
**	    upon destroying a table, view, or database procedure, or marking a
**	    database procedure dormant, call RDF to invalidate the cache entry
**	    describing the object
**	16-mar-93 (andre)
**	    qeu_d_cascade() will learn to handle destruction of integrity
**	    constraints.  In particular, it will scan IIDBDEPENDS looking for
**	    objects that depend on the constraint being destroyed which can
**	    be of type RULE (for CHECK and REFERENCES constraint), DBPROC (for
**	    REFERENCES constraint), or INDEX (for UNIQUE constraint), or
**	    CONSTRAINT (for UNIQUE constraint).
**	      - dependent rule will be added to the temp table only if it was
**	        found when we were looking for objects dependent on a specific
**	        constraint (if it was found when we were searching for objects
**		dependent on ALL constraints defined on a table because this
**		table itself will be dropped, rules will get dropped using
**		existing mechanism)
**	      - dependent DBPROC will be always added to the table
**	      - dependent INDEX will be added to the temp table only if it was
**	        found when we were looking for objects dependent on a specific
**		constraint (if it was found when we were searching for objects
**		dependent on ALL constraints defined on a table because this
**		table itself will be dropped, indices will get dropped using
**		existing mechanism)
**	      - dependent CONSTRAINT will be added to the temp table only if it 
**		was found when we were looking for objects dependent on a 
**		specific constraint or if it was defined on a different table 
**		that the independent constraint
**
**	    In the past, a dbproc would be destroyed only if its destruction was
**	    explicitly requested.  With advent of dbprocs generated by the
**	    system to enforce constraints and CHECK OPTION, this will change so
**	    that system-generated dbprocs will be destroyed rather than marked
**	    dormant if some object on which they depend is being destroyed
**	17-mar-93 (andre)
**	    added a flag attribute to the temp table.  It will be used to 
**	    indicate that the dbproc described  in the tuple should be dropped 
**	    or that we need to look for dbprocs and constraints on other 
**	    tables which depend on some constraint on the current table
**	14-apr-93 (andre)
**	    tell qeu_drule() that it is not being called as a part of DROP RULE
**	    processing
**
**	    having dropped a specific rule (i.e. the table itself is not
**	    being dropped), we will invalidate RDF cache entry for the table
**	27-jun-93 (andre)
**	    rather than delete the temp table tuple which was inserted as a
**	    reminder that we must check for objects associated with other tables
**	    which depend on constraints defined on the table being dropped, we
**	    will simply learn to ignore it during the second pass through the
**	    loop.
**	    The reason for this change is that DMF was reusing space which used
**	    to be occupied by the newly deleted tuple to store a next tuple
**	    which was inserted into the temp table.  Since our cursor was past
**	    that space on the page, we wouldn't get to see that tuple during the
**	    first scan of the temp table (when we accumulate ids of objects to
**	    be dropped or otherwise adversely affected) and thus "forget" to
**	    process some of the objects dependent on the table being dropped
**	03-sep-93 (andre)
**	    When dropping a table referenced by a REF constraint, we append to
**	    the temp table a row describing the constraint + rows describing ALL
**	    rules and dbprocs which were created to enforce that constraint. It
**	    just so happens that some of these rules were defined on the
**	    referenced table.  During the second pass through the temp table, we
**	    generally encounter the row describing the referenced table before
**	    we encounter rows describing constraint-enforcing rules which were
**	    defined on the referenced table.  ALL rules defined on the
**	    referenced table are destroyed as a part of destroying that table,
**	    so by the time we get to the row describing a constraint-enforcign
**	    rule, it may no longer be there - and that's OK.
**
**	    Currently, we fall out of the loop and since the error is E_QE0015,
**	    the code erroneously assumes that we are done with the destruction.
**	    To correct this problem, a check will be added to ensure that if a
**	    rule was not found during the second pass, we will simply proceed on
**	    to the next row.
**	10-sep-93 (andre)
**	    Once we destroy a database object (e.g. table, dbproc, constraint),
**	    it is desirable that QPs which may be affected by this destruction 
**	    be invalidated.  We decided to accompish this by altering the
**	    timestamp of some base table on which the object being dropped 
**	    depends (of course, if the object being dropped is itself a table 
** 	    or an index, QPs dependent on it will be invalidated by virtue of 
**	    that object no longer existing).  
**
**	    Of course we would like to avoid altering table timestamps unless 
**	    absolutely necessary.  In some cases (when downgrading status
**	    of a dbproc which depended on a dbevent being dropped, view being 
**	    dropped for which we could not determine the ubt_id, synonym being 
**	    dropped if we could not alter timestamp of some base table to 
**	    invalidate QPs dependent on it,or another dbproc being dropped or 
**	    downgraded, dropping rules on T1 which depend on a constraint 
**	    on T2 which is being dropped, or dropping a constraint on one
**	    table which depended on a constraint created on a different table),
**	    we will always try to force QP invalidation.  In ALL OTHER CASES
**	    (which, at least for now, consists of dropping views and synonyms), 
**	    we will expect the caller to explicitly request (by setting 
**	    QEU_FORCE_QP_INVALIDATION bit in qeuq_cb->qeuq_flag_mask) that 
**	    invalidation be performed.
**
**	    To that end, the following procedure will be followed when 
**	    determining whether destruction of a given object warrants 
**	    invalidation of a timestamp of a base table on which that object 
**	    depends:
**		if (the object being dropped is a base table or an index) 
**		{
**		    we need not alter timestamps of any object dependent on it
**		}
**		else if (    the object being dropped is a view 
**		   	 and the caller has explicitly requested that we 
**			     force QP invalidation
**			 and we have not already altered timestamp of some 
**			     base table on which this view depends
**			)
**		{
**		    alter timestamp of the base table whose id (if any) was 
**		    entered into the view tree at creation time
**		}
**		else if (the object being dropped is a constraint)
**		{
**		    // this will happen if 
**		    //   - we were called to drop a specific constraint as a 
**		    //     part of ALTER TABLE DROP CONSTRAINT processing 
**		    //   - we were called to drop a UNIQUE constraint (or a 
**		    //     table on which a UNIQUE constraint was defined) 
**		    //     on which a REFERENCES constraint depends AND the two
**		    //     constraints were defined on DIFFERENT tables
**		    //   - we were called to destroy a constraint which depended
**		    //     on a privilege which was revoked
**		    in the first and the third case, it will be the
**		    caller's responsibility to tell us what to do (to 
**		    FORCE_QP_INVALIDATION in the former case and NOT to 
**		    FORCE_QP_INVALIDATION in the latter); 
**		    in the second case, we will ALWAYS tell qeu_dintg() to 
**		    alter timestamp of a base table on which the constraint 
**		    depends (note that until we support creation of constraints
**		    on views, base table timestamp will be altered even if we 
**	    	    don't explicitly request it since qeu_dintg() always 
**		    calls DMT_ALTER)
**		}
**		else if (the object being dropped is a rule)
**		{
**		    // this will happen if 
**		    //  - a rule was created on referenced table to enforce 
**		    //    a REFERENCES constraint on a table DIFFERENT from the
**		    //	  referenced table or
**		    //  - a rule was created to enforce a specific constraint 
**		    //    being dropped and it was created on the SAME
**		    //    table as the constraint
**		    in the first case we will tell qeu_drule() to alter 
**		    timestamp of a base table on which the rule depends while 
**		    in the second case we will not explicitly request it 
**		    (again, since we will encounter a rule here only if it was
**		    created to enforce a constraint, the timestamp of the table
**		    on which the rule was defined will be altered regardless of
**		    whether we explicitly request it since qeu_drule() ALWAYS 
**		    calls DMT_ALTER)
**		}
**		else if (    the object is a dbproc
**			 and it is not marked DORMANT
**			 and we have not already altered timestamp of some 
**			     base table on which this dbproc depends
**			)
**		{
**		    we will always alter timestamp of the table whose id
**		    (if any) was entered into IIPROCEDURE tuple
**		}
**		else	// the object is a dbevent
**		{
**		    // the only object that may depend on a dbevent is a
**		    // dbproc; if any dbproc depends on the dbevent being 
**		    // dropped, we will deal with it as described above
**		}
**	6-oct-93 (stephenb)
**	    Set tablename in dmu_cb so that we can audit drop temp table.
**	07-oct-93 (andre)
**	    added yet another parameter - tbl_entry
**
**	    if asked to drop an index which was created by the DBMS on the 
**	    FOREIGN KEY in order to improve perforcreated on the FOREIGN KEY 
**	    to improve performance, we need to remove IIDBDEPENDS tuple which
**	    describes dependence of the index on the constraint.  
**	    Such indix will be created as SYSTEM_GENERATED but not 
**	    NOT_DROPPABLE - the only way to ascertain that it was, indeed, 
**	    created as a part of creating the REF constraint is to look up the 
**	    tuple in IIDBDEPENDS - we will be lazy and try to delete such tuple 
**	22-oct-93 (andre)
**	    qeu_d_cascade() will now handle reclassifying to dormant dbprocs 
**	    dependent on a synonym which was dropped.  
**	    Caller (qeu_drop_synonym()) will handle destruction of the synonym 
**	    itself - our responsibility is to find and reclassify dbprocs that 
**	    depend on it
**	28-oct-93 (andre)
**	    (fix for bug 56180)
**	    when we scan IIDBDEPENDS to collect information about objects 
**	    dependent on any constraint defined on the table being dropped, we 
**	    use a qualification function to avoid returning tuples
**	    describing objects which automatically get dropped as a part of 
**	    dropping a table.  
**
**	    Unfortunately, if a temp table row which was inserted to remind us 
**	    to look for such object was the last row in the temp table, we were
**	    forgetting to reset qeu_qual so that, when trying to destroy 
**	    IIDBDEPENDS tuples describing dependence of objects on the table 
**	    and on any constraint on the table, we would NOT drop tuples which 
**	    described dependence of objects (e.g. index created to enforce 
**	    UNIQUE constraint on the table) which automatically get dropped 
**	    as a part of dropping the table
**
**	    To fix this bug, we will reset vqeu.qeu_qual and vqeu.qeu_qarg,
**	    immediately after we are done looking for objects dependent on
**	    any constraint defined on the table being dropped (in the past
**	    we used to do it after reading the next temp table tuple which was
**	    precisely what caused the bug since we never got to that code if 
**	    there were no more temp table tuples.)
**	24-nov-93 (andre)
**	    qeu_v_ubt_id() will take address of DB_ERROR and will report its own
**	    errors
**	24-jan-94 (andre)
**	    (fix for bug 59078)
**	    text of dbprocs created to enforce a constraint was not being 
**	    dropped because we failed to initialize queryid from 
**	    dbptuple->db_txtid after we read the tuple during the second pass 
**	    (note that we don't know queryid during the first pass since this 
**	    requires that we read IIPROCEDURE which happends during the second 
**	    pass; dbprocs explicitly dropped by a user are a special case in 
**	    that a caller provides us with queryid which we can store into the 
**	    temp table.)
**	 3-jan-1996 (nick)
**	    Reorder the taking of locks to match that in 'create view' - this
**	    reduces the possibility of deadlock with 'drop view' ( #64549 )
**      04-08-1996 (sarjo01)
**          Bug 81111: do a readonly scan of iixpriv and
**          iidbdepends so that if no dependencies are found, no X page
**          locks are left on these catalogs, thereby increasing
**          concurrency for DROP TABLE.
**      05-07-1997 (sarjo01)
**          Bug 82047: qeu_d_cascade(): Problem introduced by above change:
**          If table to be dropped has a primary key constraint but no
**          otner dependencies, iidbdepends must be reopened in X mode.
**      07-08-1997 (sarjo01)
**          Fix for 82047 reintroduced inadvertant taking of IX locks on
**          iidbdepends. Reworked last change.
**	22-jul-1996 (ramra01)
**	    Alter Table add/drop column implementation for cascade/restrict
**	25-Feb-1996 (nanpr01)
**	    Fix Alter table drop column bugs.
**	04-mar-1997 (nanpr01)
**	    Bugs 79631, 80128, 80239: Alter table drop col fails to recognize
**	    referential intergrity, unique constraints.
**	7-apr-98 (inkdo01)
**	    Changes to support indexes shared amongst constraints. Only
**	    drop index when it serves no remaining constraints (other than
**	    the constraint being dropped).
**	27-Jul-1998 consi01 bug 87642 star problem INGSRV 233
**	    ensure that the view_opened flag is set/unset correctly when
**	    table reopened for writing. Add comments in places where this
**	    problem could surface again.
**      30-Mar-1999 (hanal04)
**          Do not drop PERMITs on DBPs that are being marked dormant. b96033.
**	2-mar-00 (inkdo01)
**	    Invalidate base table assoc. with indexes dropped with constraints
**	    (bug 100702).
**	14-may-02 (inkdo01)
**	    Add support for sequences.
**	28-aug-2002 (gygro01) ingsrv1859
**	    b108474 (b108635)- During drop column cascade processing system 
**	    generated rules and dbp dependent on referential constraints are 
** 	    not handled properly. There are sometimes dropped, if they shouldn't
**	    have or not dropped if they should have been due to not existing 
**	    checking for dbp's or checking not existing column bitmaps on table
**	    based rules.
**	    (b108636) Another point is that all iidbdepends related rows of the
**	    drop column table were removed in any cases, which leaves an logical
**	    inconsistency and causes problems cleaning the constraints properly
**	    afterwards.
**	    Basically the fix checks if the dependent constraints was dropped
**	    (as this will happen first) and than drop or not the dependent
**	    objects (index, rules and dbp) and clean the iidbdepends entries
**	    at the end of the processing of each object rather than doing it
**	    at the beginning, when we do not know, what we need to know.
**      29-Apr-2003 (hanal04) Bug 109682 INGSRV 2155, Bug 109564 INGSRV 2237
**          When dropping constraints do not drop the underlying index if
**          that index is a user index.
**	31-jul-2003 (penga03)
**	    After reading iidbdepends and iixpriv, putting all dependent views 
**	    and dbprocs in the temporary table, we need go through the temporary 
**	    table to remove those duplicate rows, otherwise, when trying to 
**	    delete the tables listed in the temporary table, an error "table no 
**	    longer exists" will occur for those tables that have duplicate rows.
**      01-Oct-2003 (inifa01) Bug 110429 INGSRV 2380
**          Dropping a unique constraint where base table is index causes
**          the base table to be dropped.
**          Fix to bug 109682/109564 has added iidepends entries for this case
**          , which allows dependency check to retrieve the base table as an
**	    object to be dropped while dropping the constraint.
**          FIX: If when dropping a constraint, the 'index' is not a secondary
**          index then don't add 'index' to temp table list of objects to drop.
**	19-Feb-2004 (schka24)
**	    Deal with partitioned tables.
**	7-Apr-2004 (schka24)
**	    Don't leave junk in attr default fields.
**	13-Jun-2006 (thaju02)
**	    If we fail to reopen iidbdepends with X mode, error handling will
**	    segv attempting to close it.  After getting tuple from iidbdepends 
**	    and in changing to X mode, set view_opened to reflect if iidbdepends
**	    is open/closed. (B116247)
**       6-Oct-2006 (hanal04) Bug 116791
**          During an alter table drop column we may have a view based on a
**          view based on the altered table. In this instance orig_objtype_row
**          will be false while processing the end view but we should still
**          check the view dependencies.
**
*/
DB_STATUS
qeu_d_cascade(
QEF_CB          *qef_cb,
QEUQ_CB		*qeuq_cb,
DB_QMODE	obj_type,
DMT_TBL_ENTRY	*tbl_entry)
{
    GLOBALREF QEF_S_CB *Qef_s_cb;
    RDF_CB	    rdfcb, *rdf_inv_cb = &rdfcb;
    RDR_RB	    *rdf_inv_rb = &rdf_inv_cb->rdf_rb;
    DB_IIDBDEPENDS  *dtuple;
    DB_IIPRIV	    *prtuple;
    DB_IIXPRIV	    *xprtuple;
    DB_PROCEDURE    *dbptuple;
    DB_IIXPROCEDURE *xdbptuple;
    DB_IIRULE	    *rultuple;
    DB_IIRULEIDX    *xrultuple;
    DB_INTEGRITY    *integtuple;
    DB_STATUS	    status, local_status;
    DB_TAB_ID	    view_ubt_id;
    i4	    error = E_QE0000_OK;
    i4		    zero_cons_number = 0;
    bool	    transtarted;	    
    bool	    tbl_opened;
    bool	    view_opened;
    bool	    qtext_opened;
    bool	    temp_opened;
    bool	    temp_created;
    bool	    priv_opened;
    bool	    xpriv_opened;
    bool	    dbp_opened;
    bool	    dbpp_opened;
    bool	    xdbp_opened;
    bool	    event_opened;
    bool	    seq_opened;
    bool	    rul_opened;
    bool	    xrul_opened;
    bool	    integ_opened;
    bool	    drop_view;
    bool	    drop_view_col;
    bool	    drop_event;
    bool	    drop_sequence;
    bool	    dbp_affected;
    bool	    drop_specific_rules = FALSE;
    bool	    err_already_reported = FALSE;
    bool	    cascade;				
    bool	    drop_col;
    bool	    orig_objtype_row;
    DB_TAB_ID	    save_tbl;
    i4		    drop_col_attno;
    i4		    drop_col_attid;
    i4		    i;
    i2		    qmode;
    DMT_CB	    dmt_cb;
    DB_LOC_NAME	    temp_table_loc;
    DMU_CB	    dmu_cb;
    QEU_CB	    tranqeu;
    QEU_CB	    tempqeu;		    /* for temporary table */
    QEU_CB	    vqeu;		    /* for iidbdepends table */
    QEU_CB	    qqeu;		    /* for iiqrytext table */
    QEU_CB	    tqeu;		    /* for iitree table */
    QEU_CB	    prqeu;		    /* for iipriv tuples */
    QEU_CB	    xprqeu;		    /* for iixpriv tuples */
    QEU_CB	    dbpqeu;		    /* for iiprocedure tuples */
    QEU_CB	    dbppqeu;		    /* for iiprocedure_parameter
					    ** tuples
					    */
    QEU_CB	    xdbpqeu;		    /* for iixprocedure tuples */
    QEU_CB	    evqeu;		    /* For iievent table */
    QEU_CB	    seqqeu;		    /* For sequence table */
    QEU_CB	    rulqeu;
    QEU_CB	    xrulqeu;
    QEU_CB	    iqeu;		    /* For integrity tuples */
    ULM_RCB         ulm;   
    DB_QRY_ID       queryid;
    DB_TAB_ID       currentid;
    DB_TAB_ID       origtid;
    i4		    cur_obj_type, real_obj_type;
    i4		    cons_number;
    i4		    cur_temp_flags;
    DB_ERROR	    err_blk;
    QEF_DATA	    tempqef_data;
    QEF_DATA	    vqef_data;
    QEF_DATA	    prqef_data;
    QEF_DATA	    xprqef_data;
    QEF_DATA	    dbpqef_data;
    QEF_DATA	    xdbpqef_data;
    QEF_DATA	    rulqef_data;
    QEF_DATA	    xrulqef_data;
    QEF_DATA	    iqef_data;
    DMR_ATTR_ENTRY  vkey_array[4];
    DMR_ATTR_ENTRY  *vkey_ptr_array[4];
    DMR_ATTR_ENTRY  qkey_array[3];
    DMR_ATTR_ENTRY  *qkey_ptr_array[3];
    DMR_ATTR_ENTRY  tkey_array[3];
    DMR_ATTR_ENTRY  *tkey_ptr_array[3];
    DMR_ATTR_ENTRY  prkey_array[2];
    DMR_ATTR_ENTRY  *prkey_ptr_array[2];
    DMR_ATTR_ENTRY  xprkey_array[2];
    DMR_ATTR_ENTRY  *xprkey_ptr_array[2];
    DMR_ATTR_ENTRY  xdbpkey_array[2];
    DMR_ATTR_ENTRY  *xdbpkey_ptr_array[2];
    DMR_ATTR_ENTRY  dbppkey_array[2];
    DMR_ATTR_ENTRY  *dbppkey_ptr_array[2];
    DMR_ATTR_ENTRY  xrulkey_array[2];
    DMR_ATTR_ENTRY  *xrulkey_ptr_array[2];
    DMR_ATTR_ENTRY  integkey_array[2];
    DMR_ATTR_ENTRY  *integkey_ptr_array[2];
    i4		    exit_loop, exit_loop2;
    
#define	    TEMP_TBL_DEGREE	7
    DMT_ATTR_ENTRY  attr_array[TEMP_TBL_DEGREE];
    DMT_ATTR_ENTRY  *attr_ptr_array[TEMP_TBL_DEGREE];
    i4		    saveklen;
    u_i2	    num_partitions;

    struct TEMP_ROW
    {
	i4	    temp_base;
	i4	    temp_index;
	i4	    temp_1qryid;
        i4	    temp_2qryid;
	i4        temp_i_qid;	/* b108474 (b108635) needed by
					** drop_col cascade procesing
					** for constraint dependent rules
					** and dbp to save integrity_no
					** to check, if dependent 
					** constraint has been dropped.
					*/
	i4	    temp_flags;
					/*
					** should be the only bit set
					**
					** may be set only in a tuple
					** with type set to DB_CONS
					** will be used to remind us to
					** look for objects dependent on any
					** constraint defined on the table
					** being dropped
					*/
#define	ALL_CONSTRAINTS			0x01
					/*
					** will appear in conjunction with
					** FORCE_DBP_QP_INVALIDATION
					**
					** may be set only in tuples 
					** describing dbprocs
					** will be set to indicate that the 
					** dbproc needs to be dropped because 
					** it was named in DROP PROCEDURE 
					** statement
					*/
#define	DROP_THIS_DBPROC		0x02
					/*
					** currently does not appear in
					** conjunction with any other bits,
					** but this may change
					** 
					** may be set only in tuples
					** describing views for which no 
					** underlying base table id was
					** recorded at creation time 
					** AND in tuples describing synonyms for
					** which we were unable to determine an
					** id of an object whose timestamp 
					** could be altered
					** will be set to indicate that
					** we need to determine underlying
					** base table id for objects
					** dependent on the object described
					** by the tuple
					*/
#define FIND_UBT_ID  			0x04
					/*
					** may appear in conjunction with
					** DROP_THIS_DBPROC
					**
					** may be set only in tuples
					** describing dbprocs
					** will be set to indicate that the 
					** timestamp of a table whose id is 
					** stored in DB_PROCEDURE.db_dbp_ubt_id
					** needs to be invalidated
					*/
#define FORCE_DBP_QP_INVALIDATION	0x08
					/*
					** should be the only bit set
					**
					** may be set only in tuples describing
					** base tables 
					** will be set to indicate that 
					** timestamp of a table whose id is 
					** stored in the tuple should be 
					** altered (to force invalidation of QPs
					** that depend on it or some object
					** defined over it)
					*/
#define ALTER_TIMESTAMP			0x10
					/*
					** currently does not appear in
					** conjunction with any other bits,
					** but this may change
					**
					** may be set in tuples describing 
					** constraints 
					** will be set to indicate that
					** timestamp of the table over which
					** the constraint was defined needs 
					** to be altered
					*/
#define	ALTER_UBT_TIMESTAMP		0x20
					/*
					** will be set as a reminder to delete 
					** IIDBDEPENDS tuple(s) naming this 
					** object as the dependent object - 
					** (at least initially) will be set 
					** when we are called to drop an index 
					** which was created to improve 
					** performance of queries enforcing 
					** REF constraint so that there is an 
					** IIDBDEPENDS tuple describing 
					** dependence of the index on the 
					** constraint; note that this will not 
					** be done when we are dropping the 
					** index as a part of destroying the 
					** constraint since ALL IIDBDEPENDS 
					** tuples describing dependence of 
					** objects on a constraint are already 
					** being deleted
					*/
#define	DELETE_IIDBDEP_TUPLE		0x40
					/*
					** will be set to indicate that the
					** temp row represents the original
					** object type passed to qeu_d_cascade.
					** it will be used to determine if
					** column specific dependencies should
					** be checked with QEU_DROP_COLUMN
					*/
#define ORIG_OBJTYPE_ROW		0x80
					/*
					** when set indicates that the row
					** object is subject to column bitmap
					** dependency checks
					*/
#define DROP_BY_COLUMN			0x100
					/*
					** when set indicates that a column
					** is being dropped.
					*/
#define DROP_BY_COLUMN_CHECK		0x200
					/* 
					** when set indicates that although this
					** view does not directly depend on the
					** dropped column, it could depend on
					** another column which may change its
					** column number.  The iitree must be
					** checked.
					*/
#define CHECK_OTHER_CONS		0x400
					/*
					** set for indexes dependent on
					** constraints. Causes check for 
					** other constraints on which index
					** may be dependant (shared indexes)
					** in which case index is NOT
					** dropped (though dbdepends row is).
					*/
#define DROP_COL_CHK_CONS		0x800
					/* b108474 (b108635, b108636)
					** Set during drop column cascade
					** processing for system generated obj
					** dependent on referential constraints.
					** Intially only used for dbp and rules
					** and for indexes only to prevent 
					** having its iidbdepends rows removed.
					*/
	DB_QMODE    temp_obj_type;	/* one of DB_TABLE, DB_INDEX, DB_VIEW, 
					** DB_DBP, DB_EVENT, DB_CONS, or 
					** DB_SYNONYM
					*/
    } *temprow, temprow2;
    i4 count, k, j;

    transtarted = tbl_opened = view_opened = qtext_opened = temp_opened =
	temp_created = priv_opened = xpriv_opened = dbp_opened = xdbp_opened =
	    event_opened = seq_opened = dbp_affected = dbpp_opened = rul_opened = 
		xrul_opened = integ_opened = FALSE;

    /* 
    ** remember whether we will be dropping any views, dbevents, sequences, dbprocs; 
    ** once set to FALSE, 
    ** - drop_view may be reset to TRUE if we have to drop view(s) dependent on 
    **   the table being dropped,
    ** - dbp_affected may be reset to TRUE if we find dbproc(s) that need to be
    **   dropped or marked dormant as a result of destruction of specified 
    **	 object
    */
    drop_view = (obj_type == DB_VIEW) ? TRUE : FALSE;
    drop_event = (obj_type == DB_EVENT) ? TRUE : FALSE;
    drop_sequence = (obj_type == DB_SEQUENCE) ? TRUE : FALSE;
    dbp_affected = (obj_type == DB_DBP) ? TRUE : FALSE;

    STRUCT_ASSIGN_MACRO(*qeuq_cb->qeuq_rtbl, save_tbl);
    drop_col = (qeuq_cb->qeuq_flag_mask & QEU_DROP_COLUMN) ? TRUE : FALSE;
    cascade = (qeuq_cb->qeuq_flag_mask & QEU_DROP_CASCADE) ? TRUE : FALSE;
    drop_col_attno = (drop_col) ? qeuq_cb->qeuq_ano : 0;
    drop_col_attid = (drop_col) ? qeuq_cb->qeuq_aid : 0;

    num_partitions = 0;
    if (tbl_entry != NULL)
	num_partitions = tbl_entry->tbl_nparts;
    
    /* Initialize array of pointer for attributes. */

    for (i=0; i < TEMP_TBL_DEGREE; i++)
	attr_ptr_array[i] = &attr_array[i];	

    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
    ulm.ulm_streamid = (PTR)NULL;

    for (exit_loop = FALSE; !exit_loop; )
    {
	/* Check the control block. */

	error = E_QE0017_BAD_CB;
	status = E_DB_ERROR;
	if (qeuq_cb->qeuq_type != QEUQCB_CB ||
            qeuq_cb->qeuq_length != sizeof(QEUQ_CB))	
	    break;

	error = E_QE0018_BAD_PARAM_IN_CB;
	if (   (qeuq_cb->qeuq_rtbl == 0)
            || (qeuq_cb->qeuq_db_id == 0)
            || (qeuq_cb->qeuq_d_id == 0))

	    break;
		
	/* 
	** Check to see if a transaction is in progress,
	** if so, set transtarted flag to FALSE, otherwise
	** set it to TRUE after beginning a transaction.
	** If we started a transaction them we will abort
        ** it if an error occurs, otherwise we will just
        ** return the error and let the caller abort it.
	*/

	/* 
        ** Initialize session control block to point
        ** to active control block.
        */
	
	if (qef_cb->qef_stat == QEF_NOTRAN)
	{
	    tranqeu.qeu_type = QEUCB_CB;
	    tranqeu.qeu_length = sizeof(QEUCB_CB);
	    tranqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
	    tranqeu.qeu_d_id = qeuq_cb->qeuq_d_id;
	    tranqeu.qeu_flag = 0;
	    tranqeu.qeu_mask = 0;
	    status = qeu_btran(qef_cb, &tranqeu);
	    if (status != E_DB_OK)
	    {	
		error = tranqeu.error.err_code;
		break;	
	    }	    
	    transtarted = TRUE;
	}
	/* escalate the transaction to MST */
	if (qef_cb->qef_auto == QEF_OFF)
	    qef_cb->qef_stat = QEF_MSTRAN;

	/* 
	** Create a temporary table to hold the list of DBMS objects dependent
	** on the object to be deleted.
	*/

	MEmove(4, "base", ' ', DB_MAXNAME,
             (char *)&attr_array[0].attr_name);
	attr_array[0].attr_type = DB_INT_TYPE;
	attr_array[0].attr_size = 4;
        attr_array[0].attr_precision = 0;
        attr_array[0].attr_flags = 0;
        attr_array[0].attr_collID = -1;
	SET_CANON_DEF_ID(attr_array[0].attr_defaultID, DB_DEF_NOT_DEFAULT);
	attr_array[0].attr_default = NULL;

	for (i =1; i < TEMP_TBL_DEGREE; i++)
	    STRUCT_ASSIGN_MACRO(attr_array[0], attr_array[i]);
	MEmove(5, "index", ' ', DB_MAXNAME,
                 (char *)&attr_array[1].attr_name);
	MEmove(6, "qryid1", ' ', DB_MAXNAME,
                  (char *)&attr_array[2].attr_name);
	MEmove(6, "qryid2", ' ', DB_MAXNAME,
                  (char *)&attr_array[3].attr_name);
	MEmove(5, "i_qid", ' ', DB_MAXNAME,
                  (char *)&attr_array[4].attr_name);
	MEmove(sizeof("flags") - 1, "flags", ' ', DB_MAXNAME,
	    (char *)&attr_array[5].attr_name);
	MEmove(sizeof("objtype") - 1, "objtype", ' ', DB_MAXNAME,
	    (char *)&attr_array[6].attr_name);

	attr_array[6].attr_size = sizeof(DB_QMODE);
	
	dmt_cb.length = sizeof(DMT_CB);
	dmt_cb.type = DMT_TABLE_CB;
	dmt_cb.dmt_flags_mask = DMT_DBMS_REQUEST;
        dmt_cb.dmt_db_id = qeuq_cb->qeuq_db_id;
	dmt_cb.dmt_char_array.data_address = 0;
	dmt_cb.dmt_char_array.data_in_size = 0;
	dmt_cb.dmt_attr_array.ptr_address = (PTR) attr_ptr_array;
	dmt_cb.dmt_attr_array.ptr_in_count = TEMP_TBL_DEGREE;
	dmt_cb.dmt_attr_array.ptr_size = sizeof (DMT_ATTR_ENTRY);
	dmt_cb.dmt_tran_id = qef_cb->qef_dmt_id;	    
	MEmove(8, "$default", ' ', sizeof(DB_LOC_NAME),
                  (char *)&temp_table_loc);
	dmt_cb.dmt_location.data_address = (PTR)&temp_table_loc;
	dmt_cb.dmt_location.data_in_size = sizeof(DB_LOC_NAME);

	status = dmf_call(DMT_CREATE_TEMP, &dmt_cb);
	if (status != E_DB_OK)
	{
	    error = dmt_cb.error.err_code;
	    if (error == E_DM0078_TABLE_EXISTS)
	    {
		error = E_QE0050_TEMP_TABLE_EXISTS;
		status = E_DB_SEVERE;
	    }
	    break;
	}
	temp_created = TRUE;

	/* 
        ** Get a memory stream for reading tuples
        */
	
        for (exit_loop2 = FALSE; !exit_loop2; )            /* to break out of */
	{
	    /* Open stream and allocate list in one action */
	    ulm.ulm_flags = ULM_PRIVATE_STREAM | ULM_OPEN_AND_PALLOC;
	    ulm.ulm_psize = sizeof(struct TEMP_ROW);

	    if ((status = qec_mopen(&ulm)) != E_DB_OK)
		break;
	    temprow = (struct TEMP_ROW *) ulm.ulm_pptr;

	    ulm.ulm_psize = sizeof(DB_IIDBDEPENDS);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    dtuple = (DB_IIDBDEPENDS*) ulm.ulm_pptr;

	    ulm.ulm_psize = sizeof(DB_IIPRIV);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    prtuple = (DB_IIPRIV *) ulm.ulm_pptr;

	    ulm.ulm_psize = sizeof(DB_IIXPRIV);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    xprtuple = (DB_IIXPRIV *) ulm.ulm_pptr;

	    ulm.ulm_psize = sizeof(DB_INTEGRITY);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
	    	ulm_closestream(&ulm);
		break;
	    }
	    integtuple = (DB_INTEGRITY *) ulm.ulm_pptr;

	    exit_loop2 = TRUE;
	}
	if (status != E_DB_OK)
	{
	    error = E_QE001E_NO_MEM;
	    break;
	}

	/* 
        ** Open the Temporary table and the iitree, iiqrytext, iidbdepends,
	** iipriv, iixpriv, and iiintegrity System tables.
        */

	qqeu.qeu_type = QEUCB_CB;
        qqeu.qeu_length = sizeof(QEUCB_CB);
        qqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
        qqeu.qeu_lk_mode = DMT_IX;
        qqeu.qeu_flag = DMT_U_DIRECT;
        qqeu.qeu_access_mode = DMT_A_WRITE;
	qqeu.qeu_mask = 0;
	qqeu.qeu_qual = qqeu.qeu_f_qual = 0;
	qqeu.qeu_qarg = qqeu.qeu_f_qarg = 0;

	STRUCT_ASSIGN_MACRO(qqeu, vqeu);
	STRUCT_ASSIGN_MACRO(qqeu, tqeu);
	STRUCT_ASSIGN_MACRO(qqeu, tempqeu);
	STRUCT_ASSIGN_MACRO(qqeu, prqeu);
	STRUCT_ASSIGN_MACRO(qqeu, xprqeu);
	STRUCT_ASSIGN_MACRO(qqeu, iqeu);

        vqeu.qeu_lk_mode = DMT_IS;
        vqeu.qeu_access_mode = DMT_A_READ;

        xprqeu.qeu_lk_mode = DMT_IS;
        xprqeu.qeu_access_mode = DMT_A_READ;

        tempqeu.qeu_tab_id.db_tab_base  = dmt_cb.dmt_id.db_tab_base;
        tempqeu.qeu_tab_id.db_tab_index  = dmt_cb.dmt_id.db_tab_index;
	status = qeu_open(qef_cb, &tempqeu);
	if (status != E_DB_OK)
	{
	    error = tempqeu.error.err_code;
	    break;
	}
	temp_opened = TRUE;

	qqeu.qeu_tab_id.db_tab_base = DM_B_QRYTEXT_TAB_ID;
	qqeu.qeu_tab_id.db_tab_index = DM_I_QRYTEXT_TAB_ID;
	status = qeu_open(qef_cb, &qqeu);
	if (status != E_DB_OK)
	{
	    error = qqeu.error.err_code;
	    break;
	}
	qtext_opened = TRUE;

        tqeu.qeu_tab_id.db_tab_base  = DM_B_TREE_TAB_ID;
        tqeu.qeu_tab_id.db_tab_index  = DM_I_TREE_TAB_ID;
	status = qeu_open(qef_cb, &tqeu);
	if (status != E_DB_OK)
	{
	    error = tqeu.error.err_code;
	    break;
	}
	tbl_opened = TRUE;

	vqeu.qeu_tab_id.db_tab_base = DM_B_DEPENDS_TAB_ID;    		
	vqeu.qeu_tab_id.db_tab_index = DM_I_DEPENDS_TAB_ID;    		
	status = qeu_open(qef_cb, &vqeu);
	if (status != E_DB_OK)
	{
	    error = vqeu.error.err_code;
	    break;
	}
	view_opened = TRUE;

        prqeu.qeu_tab_id.db_tab_base  = DM_B_PRIV_TAB_ID;
        prqeu.qeu_tab_id.db_tab_index  = DM_I_PRIV_TAB_ID;
	status = qeu_open(qef_cb, &prqeu);
	if (status != E_DB_OK)
	{
	    error = prqeu.error.err_code;
	    break;
	}
	priv_opened = TRUE;

        xprqeu.qeu_tab_id.db_tab_base  = DM_B_XPRIV_TAB_ID;
        xprqeu.qeu_tab_id.db_tab_index  = DM_I_XPRIV_TAB_ID;
	status = qeu_open(qef_cb, &xprqeu);
	if (status != E_DB_OK)
	{
	    error = xprqeu.error.err_code;
	    break;
	}
	xpriv_opened = TRUE;

	iqeu.qeu_tab_id.db_tab_base = DM_B_INTEGRITY_TAB_ID;
	iqeu.qeu_tab_id.db_tab_index = DM_I_INTEGRITY_TAB_ID;
	iqeu.qeu_lk_mode = DMT_IS;
	iqeu.qeu_flag = DMT_U_DIRECT;
	iqeu.qeu_access_mode = DMT_A_READ;
	status = qeu_open(qef_cb, &iqeu);
	if (status != E_DB_OK)
	{
	    error = iqeu.error.err_code;
	    break;
	}
	integ_opened = TRUE;

	/* Set up all control blocks to retrieve one tuple. */

	tempqeu.qeu_count = 1;
        tempqeu.qeu_tup_length = sizeof(struct TEMP_ROW);
	tempqeu.qeu_output = &tempqef_data;
	tempqef_data.dt_next = 0;
        tempqef_data.dt_size = sizeof(struct TEMP_ROW);
        tempqef_data.dt_data = (PTR) temprow;

	vqeu.qeu_count = 1;
        vqeu.qeu_tup_length = sizeof(DB_IIDBDEPENDS);
	vqeu.qeu_output = &vqef_data;
	vqef_data.dt_next = 0;
        vqef_data.dt_size = sizeof(DB_IIDBDEPENDS);
        vqef_data.dt_data = (PTR) dtuple;

	prqeu.qeu_count = 1;
        prqeu.qeu_tup_length = sizeof(DB_IIPRIV);
	prqeu.qeu_output = &prqef_data;
	prqef_data.dt_next = 0;
        prqef_data.dt_size = sizeof(DB_IIPRIV);
        prqef_data.dt_data = (PTR) prtuple;

	xprqeu.qeu_count = 1;
        xprqeu.qeu_tup_length = sizeof(DB_IIXPRIV);
	xprqeu.qeu_output = &xprqef_data;
	xprqef_data.dt_next = 0;
        xprqef_data.dt_size = sizeof(DB_IIXPRIV);
        xprqef_data.dt_data = (PTR) xprtuple;

	iqeu.qeu_count = 1;
	iqeu.qeu_tup_length = sizeof(DB_INTEGRITY);
	iqeu.qeu_output = &iqef_data;
	iqef_data.dt_next = 0;
	iqef_data.dt_size = sizeof(DB_INTEGRITY);
	iqef_data.dt_data = (PTR) integtuple;

	/* 
        ** First we must write a record describing the object we wish to delete
	** to the temporary file to get a starting tuple.
        */

	tempqeu.qeu_input = &tempqef_data;
	temprow->temp_base = qeuq_cb->qeuq_rtbl->db_tab_base;
	temprow->temp_index = qeuq_cb->qeuq_rtbl->db_tab_index;
	temprow->temp_i_qid = 0;

	/*
	** if asked to drop a constraint, we will store its number in
	** temp_1qryid
	*/
	if (obj_type == DB_CONS)
	{
	    temprow->temp_1qryid = qeuq_cb->qeuq_ino;
	    temprow->temp_2qryid = 0;

	    /*
	    ** if the caller told us to alter timestamp of a table over
	    ** which the constraint was defined, set ALTER_UBT_TIMESTAMP to
	    ** remind us to ask qeu_dintg() to alter the timestamp (actually, 
	    ** since qeu_dintg() always alter timestamp of the object over which
	    ** the integrity was defined and integrities/constraints can
	    ** only be defined on base tables, this bit is being set with an
	    ** eye on the future when we may decide to support creation of
	    ** constraints on views)
	    */
	    if (qeuq_cb->qeuq_flag_mask & QEU_FORCE_QP_INVALIDATION)
	    {
	        temprow->temp_flags = ALTER_UBT_TIMESTAMP;
	    }
	    else
	    {
		temprow->temp_flags = 0;
	    }
	}
	else
	{
	    temprow->temp_1qryid = qeuq_cb->qeuq_qid.db_qry_high_time;
	    temprow->temp_2qryid = qeuq_cb->qeuq_qid.db_qry_low_time;

	    if (obj_type == DB_DBP)
	    {
	        /* 
	        ** remember that this dbproc will have to go + remember to
	        ** invalidate its QP
	        */
	        temprow->temp_flags = 
		    DROP_THIS_DBPROC | FORCE_DBP_QP_INVALIDATION;
	    }
	    else if (obj_type == DB_VIEW)
	    {
	        /* dropping a view */
	        if (qeuq_cb->qeuq_flag_mask & QEU_FORCE_QP_INVALIDATION)
	        {
	            /*
	            ** if asked to force invalidation of QPs that may depend
		    ** on this view, determine whether the view was defined over
	            ** some base table 
	            */
	            status = qeu_v_ubt_id(qef_cb, qeuq_cb->qeuq_rtbl,
	    	        &view_ubt_id, &qeuq_cb->error);
	            if (DB_FAILURE_MACRO(status))
		    {
			err_already_reported = TRUE;
			error = qeuq_cb->error.err_code;
		        break;
		    }
	        
	            /* 
	            ** if ubt_id was determined, we will insert an additional
	            ** tuple to remind us to alter ubt's timestamp; otherwise, 
		    ** we will set FIND_UBT_ID bit to indicate that we need to 
		    ** look for a ubt_id for objects dependent on this view
	            */
	            temprow->temp_flags = 
		        (view_ubt_id.db_tab_base) ? 0 : FIND_UBT_ID;
	        }
	        else
	        {
		    view_ubt_id.db_tab_base = view_ubt_id.db_tab_index = 0;
		    temprow->temp_flags = 0;
	        }
	    }
	    else if (   obj_type == DB_INDEX
		 && tbl_entry->tbl_2_status_mask & DMT_SYSTEM_GENERATED
		 && ~tbl_entry->tbl_2_status_mask & DMT_NOT_DROPPABLE)
	    {
	        /*
	        ** this index was created in the course of creating a REF 
		** constraint now that we were asked to drop it, we need to 
		** remember to delete IIDBDEPENDS tuple proclaiming its 
		** dependence on the REF constraint
	        */
	        temprow->temp_flags = DELETE_IIDBDEP_TUPLE;
	    }
	    else if (obj_type == DB_SYNONYM)
	    {
		/*
		** synonym has been dropped; we were called to look for dbprocs
		** dependent on that synonym.  If we could not alter a timestamp
		** of some base table to force invalidation of QPs affected by 
		** destruction of the synonym, we will set FIND_UBT_ID bit to 
		** indicate that we need to look for a ubt_id for dbprocs 
		** dependent on this synonym
		*/
		if (qeuq_cb->qeuq_flag_mask & QEU_FORCE_QP_INVALIDATION)
		{
		    temprow->temp_flags = FIND_UBT_ID;
		}
		else
		{
		    temprow->temp_flags = 0;
		}
	    }
	    else
	    {
	        temprow->temp_flags = 0;
	    }
	}

	temprow->temp_obj_type = obj_type;
	temprow->temp_flags |= ORIG_OBJTYPE_ROW;

	status = qeu_append(qef_cb, &tempqeu);	
	if (status != E_DB_OK)
	{
	    error = tempqeu.error.err_code;
	    break;
	}

	if (obj_type == DB_TABLE)
	{
	    /*
	    ** if we were asked to destroy a base table, append a row to the 
	    ** temp table which will remind us to look up objects dependent on 
	    ** any constraints that may be defined on this table
	    ** If this is a drop column request then find ALL_CONSTAINT
	    ** only if cascade is specified.
	    */         
	    if ((!drop_col) || (cascade))
	    {
	        temprow->temp_obj_type = DB_CONS;
	        temprow->temp_flags = ALL_CONSTRAINTS;
	        status = qeu_append(qef_cb, &tempqeu);
	        if (status != E_DB_OK)
	        {
		    error = tempqeu.error.err_code;
		    break;
	        }
	    }
	}
	else if (obj_type == DB_VIEW && view_ubt_id.db_tab_base != 0)
	{
	    /*
	    ** if we were asked to destroy a view and we were able to
	    ** determine the id of some base table used in its definition,
	    ** append a row to the temp table which will remind us alter
	    ** that table's timestamp 
	    */
	    temprow->temp_base     = view_ubt_id.db_tab_base;
	    temprow->temp_index    = view_ubt_id.db_tab_index;
	    temprow->temp_1qryid   = temprow->temp_2qryid = 0;
	    temprow->temp_flags    = ALTER_TIMESTAMP;
	    temprow->temp_obj_type = DB_TABLE;

	    status = qeu_append(qef_cb, &tempqeu);
	    if (status != E_DB_OK)
	    {
		error = tempqeu.error.err_code;
		break;
	    }
	}

	/* 
        ** Read the temporary table.  Position it for all.  For each 
	** tuple take the table id and use it as key for reading
	** iidbdepends tuples and iixpriv tuples (which will be used to do a
	** TID-join into IIPRIV to obtain info about dependent objects) for all
	** views/dbprocs dependent on this view/table/dbproc/dbevent/sequence. 
	** For each dependent object found, write it to the temporary table.
        ** NOTE-you will see all tuples appended since this is a direct 
	** update.  This will continue until all dependent views and dbprocs
	** have been processed.
	** 
	** Care must be exercised to avoid getting caught in an infinite loop
	** if dealing with dbprocs calling each other. To avoid having to rescan
	** the temporary table each time we come across a dbproc to make sure
	** that it has not already been added to the temp table, we will instead
	** destroy IIDBDEPENDS and IIPRIV tuples describing objects dependent on
	** the current dbproc or on privilege on the current dbproc immediately
	** after entering into the temp table descriptions of objects dependent
	** on the current dbproc 
        */	

	tempqeu.qeu_getnext = QEU_REPO;
	tempqeu.qeu_klen = 0;       

	vqeu.qeu_key = vkey_ptr_array;

	vkey_ptr_array[0] = &vkey_array[0];
	vkey_ptr_array[0]->attr_number = DM_1_DEPENDS_KEY;
	vkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	vkey_ptr_array[0]->attr_value = (char *) &currentid.db_tab_base;

	vkey_ptr_array[1] = &vkey_array[1];
	vkey_ptr_array[1]->attr_number = DM_2_DEPENDS_KEY;
	vkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	vkey_ptr_array[1]->attr_value = (char *) &currentid.db_tab_index;

	vkey_ptr_array[2] = &vkey_array[2];
	vkey_ptr_array[2]->attr_number = DM_3_DEPENDS_KEY;
	vkey_ptr_array[2]->attr_operator = DMR_OP_EQ;
	vkey_ptr_array[2]->attr_value = (char *) &cur_obj_type;

	/*
	** if dropping a constraint, we may need to scan IIDBDEPENDS for
	** objects that depend on the constraint, which will require that we
	** key into IIDBDEPENDS using all four keys
	*/
	    
	vkey_ptr_array[3] = &vkey_array[3];
	vkey_ptr_array[3]->attr_number = DM_4_DEPENDS_KEY;
	vkey_ptr_array[3]->attr_operator = DMR_OP_EQ;
	vkey_ptr_array[3]->attr_value = (char *) &cons_number;

	xprqeu.qeu_key = xprkey_ptr_array;

	xprkey_ptr_array[0] = &xprkey_array[0];
	xprkey_ptr_array[0]->attr_number = DM_1_XPRIV_KEY;
	xprkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	xprkey_ptr_array[0]->attr_value = (char *) &currentid.db_tab_base;

	xprkey_ptr_array[1] = &xprkey_array[1];
	xprkey_ptr_array[1]->attr_number = DM_2_XPRIV_KEY;
	xprkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	xprkey_ptr_array[1]->attr_value = (char *) &currentid.db_tab_index;

	iqeu.qeu_key = integkey_ptr_array;

	integkey_ptr_array[0] = &integkey_array[0];
	integkey_ptr_array[0]->attr_number = DM_1_INTEGRITY_KEY;
	integkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	integkey_ptr_array[0]->attr_value = (char *) &currentid.db_tab_base;

	integkey_ptr_array[1] = &integkey_array[1];
	integkey_ptr_array[1]->attr_number = DM_2_INTEGRITY_KEY;
	integkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	integkey_ptr_array[1]->attr_value = (char *) &currentid.db_tab_index;

	/*
	** initialize RDF_CB which will be used to invalidate cache entries for
	** tables and views that get destroyed, database procedures that get
	** marked dormant, tables s.t. QUEL-style permit(s) defined on it
	** had to be dropped as a result of destroying a table involved in the
	** permit's qualification, and tables a rule on which has been
	** dropped.  
	** 
	** NOTE that since dbevent information is not cached, we will not be
	** trying to invalidate a cache entry having destroyed a dbevent; if
	** dbevent information ever gets cached, this will have to change
	*/
	qeu_rdfcb_init((PTR) rdf_inv_cb, qef_cb);
	
	for(;;)
	{
		/*
		** will be set to TRUE if we are looking for objects dependent
		** on a specific constraint; FALSE otherwise
		*/
	    bool	    specific_constraint;
	    
            status = qeu_get(qef_cb, &tempqeu);
	    if (status != E_DB_OK)
	    {
		error = tempqeu.error.err_code;
		break;
	    }

	    /* Keep scanning until no more rows, do not reposition. */

	    tempqeu.qeu_getnext = QEU_NOREPO;
	    tempqeu.qeu_klen = 0;

	    orig_objtype_row =
	    		(temprow->temp_flags & ORIG_OBJTYPE_ROW) ? TRUE : FALSE;

	    /* 
	    ** if this tuple describes a rule, go to the next tuple since no 
	    ** object depends on rules
	    */
	    if (temprow->temp_obj_type == DB_RULE)
	    {
		continue;
	    }

	    /*
	    ** if the tuple describes a base table whose timestamp needs
	    ** to be altered, go to the next tuple since we will deal with
	    ** timestamps during the second pass
	    */
	    if (   temprow->temp_obj_type == DB_TABLE 
		&& temprow->temp_flags == ALTER_TIMESTAMP)
	    {
		continue;
	    }

	    /* Get current object id and type from tuple read from temporary */

            currentid.db_tab_base = temprow->temp_base;
            currentid.db_tab_index = temprow->temp_index;

	    /* remember real object type */
	    real_obj_type = temprow->temp_obj_type;

	    /* remember temp_flags */
	    cur_temp_flags = temprow->temp_flags;

	    /*
	    ** if current object is a view or an index (as would happen when we
	    ** are asked to DROP a UNIQUE constraint), we must set cur_obj_type
	    ** to DB_TABLE since in IIDBDEPENDS independent object type for
	    ** tables, indices, AND views is set to DB_TABLE
	    */
	    if (real_obj_type == DB_VIEW || real_obj_type == DB_INDEX)
	    {
		cur_obj_type = DB_TABLE;
	    }
	    else
	    {
		cur_obj_type = real_obj_type;
	    }

	    /* 
            ** Now read the iidbdepends for objects dependent on the current
	    ** object(s) ( (S) is added to account for temp rows indicating that
	    ** we will search for objects dependent on "all constraints defined
	    ** on the table")
            */

	    vqeu.qeu_getnext = QEU_REPO;

	    /*
	    ** for objects other than constraints, we will always access
	    ** IIDBDEPENDS using 3 keys (id, idx, and type.)
	    ** 
	    ** If a temp table record describes "all constraints on a table"
	    ** (which will be represented by temp_obj_type==DB_CONS and
	    ** temp_flags = ALL_CONSTRAINTS and will happen when we are
	    ** destroying a table and want to destroy all objects that depend on
	    ** constraints defined on it), we will also use three keys.
	    ** If, however, we need to destroy a specific constraint, we will
	    ** access IIDBDEPENDS using all 4 keys
	    */
	    if (cur_obj_type == DB_CONS)
	    {
		if (temprow->temp_flags != ALL_CONSTRAINTS)
	        {
		    cons_number = temprow->temp_1qryid;
		    vqeu.qeu_klen = 4;

		    /*
		    ** remember that we are searching for objects dependent on a
		    ** specific constraint
		    */
		    specific_constraint = TRUE;
		}
		else
		{
		    vqeu.qeu_klen = 3;
		    specific_constraint = FALSE;

		    /* 
		    ** when searching for objects dependent on any of 
		    ** constraints defined on the table that is being dropped, 
		    ** we are only interested in dependent database procedures,
		    ** rules on other tables which were defined to enforce some
		    ** REFERENCES constraint on this table, and in constraints
		    ** defined on tables other than the one being dropped;
		    ** qeu_q_consdep() will filter out dependent objects which
		    ** will be of no interest to us
		    */
		    if (drop_col)
		    {
		        vqeu.qeu_qual = 0;
		    	vqeu.qeu_qarg = 0;
		    }
		    else
		    {
		        vqeu.qeu_qual = ( PTR ) qeu_q_consdep;
		        vqeu.qeu_qarg = (PTR) &currentid;
		    }
		}
	    }
	    else
	    {
		vqeu.qeu_klen = 3;
		specific_constraint = FALSE;
	    }

	    for(;;)
	    {
		status = qeu_get(qef_cb, &vqeu);
		if (status != E_DB_OK)
		{
		    error = vqeu.error.err_code;
		    break;
		}
		if (vqeu.qeu_access_mode == DMT_A_READ)
		{
		    /* Change over to X mode scan */
		    qeu_close(qef_cb, &vqeu); /* FIX for 87642 may be needed */
		    view_opened = FALSE;
                    vqeu.qeu_lk_mode = DMT_IX;
                    vqeu.qeu_access_mode = DMT_A_WRITE;
		    status = qeu_open(qef_cb, &vqeu);
                    if (status != E_DB_OK)
                    {
			error = vqeu.error.err_code;
			break;
		    }
		    view_opened = TRUE;
		    vqeu.qeu_getnext = QEU_REPO;
                    status = qeu_get(qef_cb, &vqeu);
                    if (status != E_DB_OK)
                    {
                        error = vqeu.error.err_code;
                        break;
                    }
		}

		/* Don't reposition, continue scan. */

		vqeu.qeu_getnext = QEU_NOREPO;
		vqeu.qeu_klen = 0;

		/* init temp_flags here - we may reset it later */
		temprow->temp_flags = 0;

		/*
		** For each tuple retrieved from iidbdepends, write it
		** to the temporary table. 
		** If this is a protection tuple, destroy the protection.
		**
		**  16-jun-92 (andre)
		**      processing of IIDBDEPENDS tuples will change in that 
		**      in addition to specifying independent object id, we will
		**      also specify the independent object type which is a part
		**      of the key.
		**
		**      IIDBDEPENDS will contain nine (see (16-mar-93 (andre)))
		**	types of dependencies:
		**
		**	  - view depends on underlying objects
		**	  - dbproc depends on underlying objects (tables/views,
		**	    dbevents, other dbprocs)
		**	  - permit depends on a table (non-GRANT compat. perms)
		**	  - permit depends on a privilege descriptor contained
		**	    in IIPRIV
		**
		**	The temp table will only contain descriptions of 
		**	dbprocs, constraints, rules, and tables/views/indices 
		**	(if we were asked to drop a dbevent, it will also 
		**	contain that dbevent).  We will not concern ourselves 
		**	with recording info about permits or privilege
		**	descriptors because those will get destroyed as a part
		**	of destroying the object to which they pertain.
		**  16-mar-93 (andre)
		**	IIDBDEPENDS may contain more types of dependencies:
		**	    - rules created to enforce a constraint depend on
		**	      that constraint
		**	    - dbprocs created to enforce a constraint depend on
		**	      that constraint
		**	    - indices created to enforce a constraint depend on
		**	      that constraint
		**	    - REFERENCES constraints depend on UNIQUE
		**	      constraints on the <referencing columns>
		**	    - REFERENCES constraints depend on REFERENCES
		**	      privilege on <referenced columns> (if the owner of
		**	      the <referenced table> is different from that of
		**	      the <referencing table>)
		**  22-oct-93 (andre)
		**	IIDBDEPENDS may also contain rows describing 
		**	dependencies between synonyms and dbprocs in whose 
		**	definitions they appear
		*/
		
		switch (dtuple->dbv_dtype)
		{
		    case DB_PROT:
		    {
		        QEUQ_CB	    tqeuq_cb;

		        tqeuq_cb.qeuq_eflag  	  = QEF_INTERNAL;
		        tqeuq_cb.qeuq_type   	  = QEUQCB_CB;
		        tqeuq_cb.qeuq_length 	  = sizeof(QEUQ_CB);
		        tqeuq_cb.qeuq_d_id   	  = qeuq_cb->qeuq_d_id;
		        tqeuq_cb.qeuq_db_id  	  = qeuq_cb->qeuq_db_id;
		        tqeuq_cb.qeuq_rtbl   	  = &dtuple->dbv_dep;
		        tqeuq_cb.qeuq_permit_mask = QEU_PERM;
		        tqeuq_cb.qeuq_pno	  = dtuple->dbv_qid;

			/* 
			** we are setting QEU_FORCE_QP_INVALIDATION bit
			** because the permit is being dropped on a table or
			** view different from the one being dropped
			*/

		        tqeuq_cb.qeuq_flag_mask = QEU_FORCE_QP_INVALIDATION;

			if (orig_objtype_row && drop_col)
			{
			   tqeuq_cb.qeuq_ano = drop_col_attno;
			   tqeuq_cb.qeuq_aid = drop_col_attid;
			   tqeuq_cb.qeuq_flag_mask |= QEU_DROP_COLUMN;

			   if (cascade)
			      tqeuq_cb.qeuq_flag_mask |= QEU_DROP_CASCADE;
			}
			else
			{
			   tqeuq_cb.qeuq_flag_mask |= QEU_DROP_CASCADE;
			}
    
		        status = qeu_dprot(qef_cb, &tqeuq_cb);
		        if (status != E_DB_OK)
		        {
			    STRUCT_ASSIGN_MACRO(tqeuq_cb.error, qeuq_cb->error);
			    error = qeuq_cb->error.err_code;

			    if (status == E_DB_WARN)
			    {
			       error = status = E_DB_OK;
			       continue;
			    }

			    break;
		        }

		        /*
		        ** need to call RDF to invalidate the permit tree (had 
			** their not been a tree, we would not have a permit 
			** dependent on a table/view) + to invalidate the cache
			** entry for the table on which the permit was defined
		        */

		        STRUCT_ASSIGN_MACRO(dtuple->dbv_dep,
					    rdf_inv_rb->rdr_tabid);
    
		        /* first invalidate the permit tree */
		        rdf_inv_rb->rdr_sequence = dtuple->dbv_qid;
		        rdf_inv_rb->rdr_2types_mask |= RDR2_ALIAS;
			rdf_inv_rb->rdr_types_mask  |= RDR_PROTECT;
		    
		        status = rdf_call(RDF_INVALIDATE, rdf_inv_cb);
		        if (DB_FAILURE_MACRO(status))
		        {
			    error = rdf_inv_cb->rdf_error.err_code;
			    break;
		        }

		        /*
		        ** now invalidate the cache entry for the table on
		        ** which the permit was defined
		        */
		        rdf_inv_rb->rdr_2types_mask &= ~RDR2_ALIAS;
			rdf_inv_rb->rdr_types_mask  &= ~RDR_PROTECT;
		        status = rdf_call(RDF_INVALIDATE, rdf_inv_cb);
		        if (DB_FAILURE_MACRO(status))
		        {
			    error = rdf_inv_cb->rdf_error.err_code;
			    break;
		        }

		        continue;
		    }
		    case DB_RULE:
		    {
		        /*
		        ** remember that we will have to drop at least one rule;
		        ** this will tell the code below to set up for reading
		        ** IIRULEIDX and IIRULE tuples
		        */
		        drop_specific_rules = TRUE;

			/*
			** if the rule was defined on a different table
			** than the constraint on which it depends, we will
			** need to alter timestamp of that table.  Since we
			** cannot tell id of the table from the IIDBDEPENDS
			** tuple (which contains the rule id rather than the
			** table id), we will overload temp_1qryid and
			** temp_2qryid to contain id of the table on which the
			** constraint was defined - during the second pass, we
			** will read the IIRULE tuple and if the rule was
			** defined on a table different from the constraint, we
			** will tell qeu_drule() to alter timestamp of the 
			** rule's table
			*/

			temprow->temp_1qryid = currentid.db_tab_base;
			temprow->temp_2qryid = currentid.db_tab_index;

			/* b108474 store integrity_no of dependent
			** constraint (b108635) and set iidbdepends 
			** cleanup flag (b108636).
			*/		
			if (drop_col && cascade
			    && dtuple->dbv_itype == DB_CONS)
			{
				temprow->temp_i_qid  = dtuple->dbv_i_qid;
				temprow->temp_flags |=
				(DELETE_IIDBDEP_TUPLE|DROP_COL_CHK_CONS);
			}

		        break;
		    }
		    case DB_CONS:
		    {
		        /* store integrity number in temp table row */
		        temprow->temp_1qryid = dtuple->dbv_qid;
		        temprow->temp_2qryid = (i4) 0;

			/*
			** we need to determine whether destroying the 
			** constraint we just found should be accompanied
			** by changing the timestamp of the table on which
			** it was defined
			** if it was defined on the same table as the
			** constraint on which it depends, we will NOT need to
			** change the timestamp; otherwise, we will (and we will
			** have to set ALTER_UBT_TIMESTAMP in temp_flags to 
			** remind us of this)
			**
			** we expect to find a constraint when looking for 
			** constraints dependent on ANY constraint on a table 
			** being dropped OR on a specific constraint being 
			** dropped; 
			** in the former case, we are guaranteed (by the 
			** qualification function) that the constraint we just 
			** found was defined on a table other than the one being
			** dropped
			** in the latter case, we actually need to compare ids 
			** of tables on which the two constraints were defined
			*/
			if (   cur_temp_flags == ALL_CONSTRAINTS
			    || currentid.db_tab_base != 
				   dtuple->dbv_dep.db_tab_base
			    || currentid.db_tab_index != 
				   dtuple->dbv_dep.db_tab_index
			   )
			{
			    temprow->temp_flags |= ALTER_UBT_TIMESTAMP;
			}

			if (orig_objtype_row && drop_col)
			   temprow->temp_flags |= DROP_BY_COLUMN;

			/* b108474 (b108636)If we are here in a drop column
			** cascade processing, just means that a referential 
			** constraint dependency record on this table was found
			** in iidbdepends. We do not want to remove this record
			** from iidbdepends at this stage of processing (after
			** this case statement) as it's not known, if this
			** constraint has any relation to the dropped column.
			** If there would be any relation, iidbdepends record
			** will be removed later together with the referential
			** constraint.
			*/
			if (drop_col && cascade
			    && dtuple->dbv_itype == DB_CONS)
			    temprow->temp_flags |= DROP_COL_CHK_CONS;

		        break;
		    }
		    case DB_INDEX:
		    {
			if (drop_col)
			{
			   /*
			   ** Find out the col to be dropped is dependent on
			   ** the index or not. If not continue
			   */
			   iqeu.qeu_klen = 2;
			   iqeu.qeu_getnext = QEU_REPO;
			   iqeu.qeu_qarg = (PTR) &dtuple->dbv_i_qid;
			   iqeu.qeu_qual = (PTR) qeu_q_integ;
			   status = qeu_get(qef_cb, &iqeu);

			   if (status != E_DB_OK)
			   {
				error = iqeu.error.err_code;
				if (error == E_QE0015_NO_MORE_ROWS &&
				    qeuq_cb->qeuq_ino == 0)
				{
				    error = E_QE0000_OK;
				    status = E_DB_OK;
				    continue;
				}
				break;
			   }
			   if (integtuple->dbi_consflags != CONS_REF)
			   {
			       if ( (BTtest (qeuq_cb->qeuq_ano,
				    (char *) integtuple->dbi_columns.db_domset))
				  && (BTcount((char *) integtuple->dbi_columns.
				      db_domset,DB_COL_BITS) != DB_COL_BITS) )
			       {
				   break;
			       }
			       else
				   continue;
			   }	
			   else {
				/* 
				** If we are deleting col from reference table,
				** we may be now at referenced table and
				** col no may be different
				*/

				/* b108474 (b108636) store integrity_no of 
				** dependent constraint and set iidbdepends 
				** cleanup flag and base table id.
				** Basically only used here for indexes to
				** prevent deletion of iidbdepends records.
				**
				** FIXME LATER: CHECK_OTHER_CONS might be 
				** needed during drop_col processing as well,
				** but is was not set until now, so I leave
				** it as is.
				** May need to be fixed if a index is used by
				** two ref constraints in order not to drop
				** the index.
				*/		
				if (drop_col && cascade
				    && dtuple->dbv_itype == DB_CONS)
				{
				    temprow->temp_i_qid  = dtuple->dbv_i_qid;
				    temprow->temp_flags |=
				    (DELETE_IIDBDEP_TUPLE|DROP_COL_CHK_CONS);
				    temprow->temp_1qryid = currentid.db_tab_base;
				    temprow->temp_2qryid = currentid.db_tab_index;
			  	}

				break;
			   }
			}
			else
			{
			   /* 
			   ** found an index dependent on a specific 
			   ** constraint 
			   */
			   if (dtuple->dbv_itype == DB_CONS && dtuple->dbv_dep.db_tab_index == 0)
			   {
			       /* index found is base table since dtuple->dbv_dep.db_tab_index 
			       ** is zero.  Don't add this object to temp table list of objects 
			       ** to drop as a result of dropping the constraint (b110429).
			       */

			       continue;
			   }

			   temprow->temp_1qryid = temprow->temp_2qryid = (i4) 0;
			   if (specific_constraint)
		 		temprow->temp_flags |= CHECK_OTHER_CONS;
			}
			break;
		    }
		    case DB_VIEW:
		    {
		        /* get the query id for this view */
		        status = qeu_qid(qeuq_cb, &dtuple->dbv_dep, 
			    (DB_QRY_ID *) &temprow->temp_1qryid, &err_blk);
		        if (status != E_DB_OK)
		        {
			    error = err_blk.err_code;
			    break;
		        }

			if (drop_col)
			{
                           if(orig_objtype_row)
                           {
                               origtid.db_tab_base = currentid.db_tab_base;
                               origtid.db_tab_index = currentid.db_tab_index;
                           }
		           status = qeu_v_finddependency(&ulm, qef_cb, &tqeu,
						   &dtuple->dbv_dep,
						   &origtid, 
						   (DB_TREE_ID *) NULL,
						   (i2) 0,
						   (DB_NAME *) NULL,
						   (DB_OWN_NAME *) NULL,
						   (DB_QMODE)dtuple->dbv_dtype,
						   drop_col_attno,
			    			   cascade,  
						   &drop_view_col,
						   &err_blk);

		           if (status != E_DB_OK)
		           {
			       error = err_blk.err_code;
			       break;
		           }

			   if (!drop_view_col)
			   {
			      temprow->temp_flags |= DROP_BY_COLUMN_CHECK;
			      temprow->temp_1qryid = currentid.db_tab_base;
			      temprow->temp_2qryid = currentid.db_tab_index;
			      break;
			   }
			}

		        /* remember that we will drop at least one view */
		        drop_view = TRUE;

			/*
			** a view can depend on a base table or on a view
			** if this view depended on a view for which ubt id
			** could not be found, see if we'll have better
			** luck with this one
			*/
			if (   real_obj_type == DB_VIEW 
			    && cur_temp_flags & FIND_UBT_ID)
		        {
			    status = qeu_v_ubt_id(qef_cb, &dtuple->dbv_dep,
			        &view_ubt_id, &qeuq_cb->error);
			    if (DB_FAILURE_MACRO(status))
			    {
				err_already_reported = TRUE;
			        error = qeuq_cb->error.err_code;
			        break;
			    }
		        
			    /* 
			    ** if ubt_id was determined, we will insert an 
			    ** additional tuple to remind us to alter ubt's 
			    ** timestamp; otherwise, we will set FIND_UBT_ID bit
			    ** to indicate that we need to look for a ubt_id for
			    ** objects dependent on this view
			    */
			    if (!view_ubt_id.db_tab_base)
				temprow->temp_flags |= FIND_UBT_ID;
		        }
		        else
		        {
			    view_ubt_id.db_tab_base = 
				view_ubt_id.db_tab_index = 0;
		        }

			break;
		    }
		    case DB_DBP:
		    {

			/* b108474 (b108636) store integrity_no of 
			** dependent constraint and set iidbdepends
			** cleanup flag and base table id.
			*/		
			  if (drop_col && cascade
				&& dtuple->dbv_itype == DB_CONS)
			  {
				temprow->temp_i_qid  = dtuple->dbv_i_qid;
				temprow->temp_flags |=
				(DELETE_IIDBDEP_TUPLE|DROP_COL_CHK_CONS);
				temprow->temp_1qryid = currentid.db_tab_base;
				temprow->temp_2qryid = currentid.db_tab_index;
			  }
			  else
			  {
		        	temprow->temp_1qryid = temprow->temp_2qryid = (i4) 0;
			  }
			

			/* 
			** remember that destruction of object specified by the
			** caller will affect some other dbprocs.  This dbproc 
			** will be dropped if it is a system-generated dbproc; 
			** otherwise it will be marked dormant.  This 
			** determination will be made later (once we have 
			** finished populating the temp table.)
			*/
		        dbp_affected = TRUE;

			/*
			** if this dbproc depended on
			**  - another dbproc,
			**  - dbevent, or
			**  - view or synonym for which ubt_id could not be 
			**    located
			** remember to force invalidation of this dbproc's QP
			*/
			if (   real_obj_type == DB_EVENT
			    || real_obj_type == DB_SEQUENCE
			    || real_obj_type == DB_DBP
			    || (   (   real_obj_type == DB_VIEW
				    || real_obj_type == DB_SYNONYM)
				&& cur_temp_flags & FIND_UBT_ID
			       )
			   )
			{
			    temprow->temp_flags |= FORCE_DBP_QP_INVALIDATION;
			}

			break;
		    }
		    default:
		    {
			status = E_DB_ERROR;
			error = E_QE0169_UNKNOWN_OBJTYPE_IN_DROP;
			break;
		    }
		}

		if (status != E_DB_OK)
		    break;

		temprow->temp_obj_type = (DB_QMODE) dtuple->dbv_dtype;
		temprow->temp_base = dtuple->dbv_dep.db_tab_base;
		temprow->temp_index = dtuple->dbv_dep.db_tab_index;
		tempqeu.qeu_count = 1;

		/*
		** tuple being added represents a
		**  - dbproc dependent on the current object,
		**  - view dependent on the current object,
		**  - rule dependent on the current object, or
		**  - constraint dependent on the current object
		*/
		status = qeu_append(qef_cb, &tempqeu);
		if (status != E_DB_OK)
		{
		    error = tempqeu.error.err_code;
		    break;
		}

		if (temprow->temp_flags & DROP_BY_COLUMN_CHECK)
		    continue;

		if (dtuple->dbv_dtype == DB_VIEW && view_ubt_id.db_tab_base)
		{
		    /*
		    ** we were able to determine the id of some base table used
		    ** in the definition of view s.t. we could not find
		    ** ubt_id of the view on which this view depended - append 
		    ** a row to the temp table which will remind us to alter 
		    ** that table's timestamp 
		    */
		    temprow->temp_base     = view_ubt_id.db_tab_base;
		    temprow->temp_index    = view_ubt_id.db_tab_index;
		    temprow->temp_1qryid   = temprow->temp_2qryid = 0;
		    temprow->temp_flags    = ALTER_TIMESTAMP;
		    temprow->temp_obj_type = DB_TABLE;

		    status = qeu_append(qef_cb, &tempqeu);
		    if (status != E_DB_OK)
		    {
			error = tempqeu.error.err_code;
			break;
		    }
		}

		/*
		** as I mentioned above, if the current IIDBDEPENDS tuple
		** represents the fact that some dbproc depends on the current
		** dbproc, we must erase this tuple immediately to avoid getting
		** caught in an infinite loop as may occur when the two dbprocs
		** call each other recursively
		**
		** vqeu.qeu_klen has been set to 0 and vqeu.qeu_flag is set to
		** DMT_U_DIRECT (the important thing is that it is not set to
		** QEU_BY_TID) - qeu_delete() will delete the current tuple
		*/

		/* b108474 (b108636) During drop column cascade processing
		** iidbdepends records of constraints dependent object
		** (indexes, rules, dbp's) can't be dropped here as it is
		** not known at this stage, if those obejcts will be dropped
		** later on. iidbdepends rows of those object will be handled
		** later on due to the previously set mask DELETE_IIDBDEP_TUPLE.
		**
		** FIXME LATER:
		** It's possible that iidbdepends records in other cases as the
		** described above will be dropped here as well unnecessarily
		** during drop column processing.
		*/

		if ((dtuple->dbv_itype == DB_DBP && 
		     dtuple->dbv_dtype == DB_DBP) || 
		    (drop_col && 
		     !(temprow->temp_flags & DROP_COL_CHK_CONS)))

		{
		    if (drop_col)
		    {
			saveklen = vqeu.qeu_klen;
		        vqeu.qeu_klen = 0;
		    }
        	    vqeu.qeu_tup_length = sizeof(DB_IIDBDEPENDS);
		    if ((status = qeu_delete(qef_cb, &vqeu)) != E_DB_OK)
		    {
			error = vqeu.error.err_code;
			break;
		    }
		    if (drop_col)
		        vqeu.qeu_klen = saveklen;
		}
	    }

	    if (error != E_QE0015_NO_MORE_ROWS)
		break;
	    status = E_DB_OK;

	    /*
	    ** (fix for bug 56180)
	    ** if during this pass we were looking for dbprocs dependent
	    ** on any constraint defined on this table and for constraints 
	    ** and rules defined on tables different from the current one which
	    ** are dependent on any constraint defined on this table, we were 
	    ** using the qualification function.  Having completed our
	    ** search, we need to reset vqeu.qeu_qual and vqeu.qeu_qarg to
	    ** ensure that we do not miss any tuples while searching for
	    ** objects dependent on objects described by the next temp table
	    ** tuple OR when we get around to actually destroying the
	    ** IIDBDEPENDS tuples
	    */
	    if (vqeu.qeu_qual)
	    {
		vqeu.qeu_qual = vqeu.qeu_qarg = NULL;
	    }

	    /* 
	    ** if we have been looking for objects dependent on constraint(s), 
	    ** we must proceed to the next temp table tuple since no views or 
	    ** dbprocs will depend on constraints; same goes for synonyms
	    */
	    if (cur_obj_type == DB_CONS || cur_obj_type == DB_SYNONYM)
		continue;

	    /*
	    ** now read IIPRIV for descriptions of the object we were asked to
	    ** destroy and of views and/or dbprocs dependent on that object
	    ** or on some privilege on it
	    */

	    xprqeu.qeu_getnext = QEU_REPO;
	    xprqeu.qeu_klen = 2;

	    /*
	    ** we will be retrieving from IIPRIV by TID and, in some cases we
	    ** will also need to delete the current tuple by TID to avoid
	    ** infinite loop when processing dbprocs which call each other
	    */
	    prqeu.qeu_flag = QEU_BY_TID;
	    prqeu.qeu_getnext = QEU_NOREPO;
	    prqeu.qeu_klen = 0;
	    
	    for(;;)
	    {
		status = qeu_get(qef_cb, &xprqeu);
		if (status != E_DB_OK)
		{
		    error = xprqeu.error.err_code;
		    break;
		}
                if (xprqeu.qeu_access_mode == DMT_A_READ)
                {
		    /* Change over X mode scan */
                    qeu_close(qef_cb, &xprqeu);/* FIX for 87642 may be needed*/
                    xprqeu.qeu_lk_mode = DMT_IX;
                    xprqeu.qeu_access_mode = DMT_A_WRITE;
                    status = qeu_open(qef_cb, &xprqeu);
                    if (status != E_DB_OK)
                    {
                        error = xprqeu.error.err_code;
                        break;
                    }
                    xprqeu.qeu_getnext = QEU_REPO;
                    status = qeu_get(qef_cb, &xprqeu);
                    if (status != E_DB_OK)
                    {
                        error = xprqeu.error.err_code;
                        break;
                    }
                }
		/* Don't reposition, continue scan. */

		xprqeu.qeu_getnext = QEU_NOREPO;
		xprqeu.qeu_klen = 0;

		/*
		** now do a TID-join into IIPRIV to get the actual tuple
		*/
		prqeu.qeu_tid = xprtuple->dbx_tidp;

		status = qeu_get(qef_cb, &prqeu);
		if (status != E_DB_OK)
		{
		    error = prqeu.error.err_code;
		    break;
		}

		/*
		** we are only interested in dependencies of objects (i.e. views
		** and dbprocs) on privileges on the current object, so we are
		** only interested in dependencies recorded in 0-th descriptor
		** 
		** NOTE: IIPRIV may contain tuples describing privileges on 
		**	 which some constraints depend; we are not interested in
		**	 looking for them here because the REFERENCES privilege
		**	 (if any) will be defined on the same columns as the
		**	 UNIQUE or PRIMARY KEY constraint on which the
		**	 REFERENCES constraint depends, so we will add that
		**	 constraint the the temp table when looking at
		**	 IIDBDEPENDS;
		**	 
		**	 when we start supporting assertions, it is likely that
		**	 REFERENCES privileges on which they will depend will be
		**	 recorded in IIPRIV and at that time, we may have to
		**	 consider IIPRIV tuples which describe dependencies of
		**	 assertions on REFERENCES privilege on objects involved
		**	 in the assertion
		*/

		if (prtuple->db_descr_no != 0)
		    continue;

		if (orig_objtype_row && drop_col)
		{
		   if ( (!BTtest (drop_col_attno,
		   		(char *) prtuple->db_priv_attmap)) ||
		   	(BTcount((char *) prtuple->db_priv_attmap,
	       			 DB_COL_BITS) == DB_COL_BITS ) )
		   {
		      continue;
		   }

		   if (!cascade)
		   {
		      error = E_QE016B_COLUMN_HAS_OBJECTS;
		      status = E_DB_ERROR;
		      break;
		   }
		}

		temprow->temp_flags = 0;

		/*
		** tuple belongs to a privilege descriptor 0 - add the dependent
		** object to the temp table;
		** descriptor 0 describes privileges on underlying objects on
		** which a given dbproc (and all permits on it) or a view (and
		** SELECT permits on it) depend
		*/

		if (prtuple->db_dep_obj_type == DB_VIEW)
		{
		    /* get the query id for this view */
		    status = qeu_qid(qeuq_cb, &prtuple->db_dep_obj_id,
			(DB_QRY_ID *) &temprow->temp_1qryid, &err_blk);
		    if (status != E_DB_OK)
		    {
			error = err_blk.err_code;
			break;
		    }

		    /* remember that we will drop at least one view */
		    drop_view = TRUE;

		    /*
		    ** a view can depend on a base table or on a view
		    ** if this view depended on a view for which ubt id
		    ** could not be found, see if we'll have better
		    ** luck with this one
		    */
		    if (   real_obj_type == DB_VIEW 
		        && cur_temp_flags & FIND_UBT_ID)
		    {
		        status = qeu_v_ubt_id(qef_cb, &dtuple->dbv_dep,
		            &view_ubt_id, &qeuq_cb->error);
		        if (DB_FAILURE_MACRO(status))
			{
			    err_already_reported = TRUE;
			    error = qeuq_cb->error.err_code;
		            break;
			}
		        
		        /* 
		        ** if ubt_id was determined, we will insert an 
		        ** additional tuple to remind us to alter ubt's 
		        ** timestamp; otherwise, we will set FIND_UBT_ID bit
		        ** to indicate that we need to look for a ubt_id for
		        ** objects dependent on this view
		        */
		        if (!view_ubt_id.db_tab_base)
			    temprow->temp_flags |= FIND_UBT_ID;
		    }
		    else
		    {
			view_ubt_id.db_tab_base = view_ubt_id.db_tab_index = 0;
		    }
		           
		}
		else	    /* must be DB_DBP */
		{
		    temprow->temp_1qryid = temprow->temp_2qryid = (i4) 0;

		    /* 
		    ** remember that destruction of object specified by the
		    ** caller will affect some dbprocs.  This dbproc 
		    ** will be dropped if it is a system-generated dbproc; 
		    ** otherwise it will be marked dormant.  This 
		    ** determination will be made later (once we have 
		    ** finished populating the temp table.)
		    */
		    dbp_affected = TRUE;

		    /*
		    ** if this dbproc depended on a privilege on 
		    **  - another dbproc,
		    **  - dbevent,
		    **  - sequence, or
		    **  - view for which ubt_id could not be located
		    ** remember to force invalidation of this dbproc's QP
		    */
		    if (   real_obj_type == DB_EVENT
		        || real_obj_type == DB_SEQUENCE
		        || real_obj_type == DB_DBP
		        || (   real_obj_type == DB_VIEW
			    && cur_temp_flags & FIND_UBT_ID)
		       )
		    {
		        temprow->temp_flags |= FORCE_DBP_QP_INVALIDATION;
		    }
		}

		temprow->temp_base = prtuple->db_dep_obj_id.db_tab_base;
		temprow->temp_index = prtuple->db_dep_obj_id.db_tab_index;
		temprow->temp_obj_type = prtuple->db_dep_obj_type;
		tempqeu.qeu_count = 1;

		/*
		** tuple being added represents a
		**  - dbproc dependent on a privilege on the current object or
		**  - view dependent on a privilege on the current object
		*/
		status = qeu_append(qef_cb, &tempqeu);
		if (status != E_DB_OK)
		{
		    error = tempqeu.error.err_code;
		    break;
		}

		if (   prtuple->db_dep_obj_type == DB_VIEW 
		    && view_ubt_id.db_tab_base)
		{
		    /*
		    ** we were able to determine the id of some base table used
		    ** in the definition of view s.t. we could not find
		    ** ubt_id of the view on which this view depended - append 
		    ** a row to the temp table which will remind us to alter 
		    ** that table's timestamp 
		    */
		    temprow->temp_base     = view_ubt_id.db_tab_base;
		    temprow->temp_index    = view_ubt_id.db_tab_index;
		    temprow->temp_1qryid   = temprow->temp_2qryid = 0;
		    temprow->temp_flags    = ALTER_TIMESTAMP;
		    temprow->temp_obj_type = DB_TABLE;

		    status = qeu_append(qef_cb, &tempqeu);
		    if (status != E_DB_OK)
		    {
			error = tempqeu.error.err_code;
			break;
		    }
		}

		/*
		** as I mentioned above, if the current IIPRIV tuple represents
		** the fact that some dbproc depends on a privilege on the
		** current dbproc, we must erase this tuple immediately to avoid
		** getting caught in an infinite loop as may occur when the two
		** dbprocs call each other recursively
		**
		** prqeu.qeu_klen has been set to 0 and qeu_flag is set to
		** QEU_BY_TID - which is exactly what we need to delete the
		** tuple by TID
		*/
		if (   cur_obj_type == DB_DBP
		    && prtuple->db_dep_obj_type == DB_DBP)
		{
        	    prqeu.qeu_tup_length = sizeof(DB_IIPRIV);
		    if ((status = qeu_delete(qef_cb, &prqeu)) != E_DB_OK)
		    {
			error = prqeu.error.err_code;
			break;
		    }
		}
	    }

	    if (error != E_QE0015_NO_MORE_ROWS)
		break;
	    status = E_DB_OK;
	}
	if (error != E_QE0015_NO_MORE_ROWS)
	    break;

	status = E_DB_OK;

	/*
	** Remove those duplicate rows in tempque (temporary table).
	*/
	tempqeu.qeu_getnext = QEU_REPO;
	count = 0;
	for (;;)
	{
	    if (qeu_get(qef_cb, &tempqeu) != E_DB_OK)
		break;
	    tempqeu.qeu_getnext = QEU_NOREPO;
	    count++;	
	}

	for (k = 0; k < count; k++)
	{
	    tempqeu.qeu_getnext = QEU_REPO;
	    j = 0;
	    for (;;)
	    {
		if (j > k) 
		    break;
		if (qeu_get(qef_cb, &tempqeu) != E_DB_OK) 
		    break;
		tempqeu.qeu_getnext = QEU_NOREPO;
		j++;
	    }
		
	    temprow2.temp_base = temprow->temp_base;
	    temprow2.temp_index = temprow->temp_index;
	    temprow2.temp_1qryid = temprow->temp_1qryid;
	    temprow2.temp_2qryid = temprow->temp_2qryid;
	    temprow2.temp_i_qid = temprow->temp_i_qid;
	    temprow2.temp_flags = temprow->temp_flags;
	    temprow2.temp_obj_type = temprow->temp_obj_type;
		
	    for (;;)
	    {
		if (qeu_get(qef_cb, &tempqeu) != E_DB_OK)
		    break;
		
		if (temprow->temp_base == temprow2.temp_base 
		    && temprow->temp_index == temprow2.temp_index 
		    && temprow->temp_1qryid == temprow2.temp_1qryid 
		    && temprow->temp_2qryid == temprow2.temp_2qryid 
		    && temprow->temp_i_qid == temprow2.temp_i_qid 
		    && temprow->temp_flags == temprow2.temp_flags 
		    && temprow->temp_obj_type == temprow2.temp_obj_type)
		{			
		    if (qeu_delete(qef_cb, &tempqeu) != E_DB_OK) 
			break;
		    count--;
		}
		
		tempqeu.qeu_getnext = QEU_NOREPO;
	    }
	}

	/* 
	** Now we have a list consisting of zero or more tables, views, rules, 
	** constraints, and indices or a dbevent and/or dbprocs to be destroyed
	** and dbprocs to be marked as DORMANT in the temporary table.
	** 
	** Not to mention at most one tuple which was used to remind us
	** to look for objects dependent on ANY constraint on the table
	** being dropped + 0 or more tuples reminding us to alter timestamps
	** of base table's on which views being dropped depend. Finally,
	** some of the tuples describing constraints, rules, and dbprocs may
	** have bits set in temp_flags reminding us to alter timestamps of
	** base tables on which they depend.
	** 
	** Next we will read the temporary table.  For each object,
	**   if the tuple was created to remind us to alter timestamp of
	**      some view's underlying base table
	**     - do so
	**   else if the current object is a table/view/index T 
        **     - destroy qrymod objects (protections, synonyms, comments, etc.)
	**       on T (if any),
	**     - destroy T,
	**     - delete tuples in iidbdepends for all views/dbprocs dependent
	**       on T, 
	**     - delete tuples in iitree - only if T is a view,
	**     - delete tuples in iiqrytext - only if T is a view ,
	**     - delete tuples in iidbdepends defining T as a dependent of
	**       base tables (views) - if T is a view, or, if T is an index 
	**	 created in the course of creating a REF constraint, delete 
	** 	 tuples in IIDBDEPENDS proclaiming its dependence on the REF
	**	 constraint and 
	**     - delete tuples in IIPRIV representing descriptors created for
	**       T or privileges on T (if T is a view) or
	**	 constraints on T (if T is a base table)
	**   else if the current object is a dbproc P
	**     - destroy protections on P since P is now (at least) dormant and
	**       all permits on it would become "abandoned",
	**     - if the temp table row directs us to force invalidation of
	**	 dbproc QPs, do so 
	**     - if this is the dbproc which we were asked to drop OR if it is a
	**       system-generated dbproc which was dependent on some object 
	**	 being dropped
	**       - delete it from IIPROCEDURE,
	**       - delete tuples in iiqrytext,
	**     - otherwise mark the dbproc as "dormant" in IIPROTECT,
	**     - delete tuples in iidbdepends for all dbprocs dependent on P,
	**     - delete tuples in iidbdepends defining P as a dependent of
	**       tables, views, dbprocs, and dbevents, and
	**     - delete tuples in IIPRIV representing a descriptor created for P
	**   else if the current object is a dbevent E
	**     - destroy E
	**     - destroy protections on E,
	**     - delete tuples in iidbdepends for all dbprocs dependent on E,
	**     - delete tuples in iiqrytext
	**   else if the current object is a sequence S
	**     - destroy S
	**     - destroy protections on S,
	**     - delete tuples in iidbdepends for all dbprocs dependent on S,
	**     - delete tuples in iiqrytext
	**   else if the current object is a rule R
	**     - if the temp table row directs us to force invalidation of
	**       QPs that may involve R, do so
	**     - destroy R
	**   else if the current object is a constraint C
	**     - if the temp table row directs us to force invalidation of
	**       QPs that may involve C, do so
	**     - destroy C
	**     - delete tuples in IIDBDEPENDS for all objects that depend on C
	**   else if the current object is a synonym S
	**     - skip this row because our caller will handle destruction of 
	**	 the synonym
        */

	tempqeu.qeu_getnext = QEU_REPO;
	tempqeu.qeu_klen = 0;       
     
	/*
	** if we need to drop and/or mark as dormant 1 or more dbprocs, open
	** IIPROCEDURE and IIXPROCEDURE + allocate space and initialize
	** structures for retrieving data from them now; 
	*/

	if (dbp_affected)
	{
	    /* 
	    ** if we may need to mark one or more dbprocs as dormant, access 
	    ** to IIPROCEDURE will be performed by a TID-join from 
	    ** IIXPROCEDURE - open it and allocate space and initialize 
	    ** structure to be used for retrieving data from it now
	    */
	    STRUCT_ASSIGN_MACRO(qqeu, xdbpqeu);

	    xdbpqeu.qeu_tab_id.db_tab_base  = DM_B_XDBP_TAB_ID;
	    xdbpqeu.qeu_tab_id.db_tab_index  = DM_I_XDBP_TAB_ID;
	    status = qeu_open(qef_cb, &xdbpqeu);
	    if (status != E_DB_OK)
	    {
		error = xdbpqeu.error.err_code;
		break;
	    }
	    xdbp_opened = TRUE;
    
	    xdbpqeu.qeu_getnext = QEU_REPO;
	    xdbpqeu.qeu_flag = 0;
	    xdbpqeu.qeu_klen = 2;

	    ulm.ulm_psize = sizeof(DB_IIXPROCEDURE);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    xdbptuple = (DB_IIXPROCEDURE *) ulm.ulm_pptr;

	    xdbpqeu.qeu_count = 1;
	    xdbpqeu.qeu_tup_length = sizeof(DB_IIXPROCEDURE);
	    xdbpqeu.qeu_output = &xdbpqef_data;
	    xdbpqef_data.dt_next = 0;
	    xdbpqef_data.dt_size = sizeof(DB_IIXPROCEDURE);
	    xdbpqef_data.dt_data = (PTR) xdbptuple;

	    xdbpqeu.qeu_key = xdbpkey_ptr_array;

	    xdbpkey_ptr_array[0] = &xdbpkey_array[0];
	    xdbpkey_ptr_array[0]->attr_number = DM_1_XDBP_KEY;
	    xdbpkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	    xdbpkey_ptr_array[0]->attr_value = (char *) &currentid.db_tab_base;

	    xdbpkey_ptr_array[1] = &xdbpkey_array[1];
	    xdbpkey_ptr_array[1]->attr_number = DM_2_XDBP_KEY;
	    xdbpkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	    xdbpkey_ptr_array[1]->attr_value = (char *) &currentid.db_tab_index;

	    /* open IIPROCEDURE */
	    STRUCT_ASSIGN_MACRO(qqeu, dbpqeu);

	    dbpqeu.qeu_tab_id.db_tab_base  = DM_B_DBP_TAB_ID;
	    dbpqeu.qeu_tab_id.db_tab_index  = DM_I_DBP_TAB_ID;
	    status = qeu_open(qef_cb, &dbpqeu);
	    if (status != E_DB_OK)
	    {
		error = dbpqeu.error.err_code;
		break;
	    }
            dbp_opened = TRUE;

            ulm.ulm_psize = sizeof(DB_PROCEDURE);
            if ((status = qec_malloc(&ulm)) != E_DB_OK)
            {
	        error = E_QE001E_NO_MEM;
	        ulm_closestream(&ulm);
	        break;
	    }
	    dbptuple = (DB_PROCEDURE *) ulm.ulm_pptr;

	    dbpqeu.qeu_count = 1;
	    dbpqeu.qeu_tup_length = sizeof(DB_PROCEDURE);
	    dbpqeu.qeu_output = dbpqeu.qeu_input = &dbpqef_data;
	    dbpqef_data.dt_next = 0;
	    dbpqef_data.dt_size = sizeof(DB_PROCEDURE);
	    dbpqef_data.dt_data = (PTR) dbptuple;

	    /* 
	    ** IIPROCEDURE will be used to access by TID except when deleting a
	    ** tuple 
	    */
	    dbpqeu.qeu_flag = QEU_BY_TID;
	    dbpqeu.qeu_klen = 0;
	    dbpqeu.qeu_getnext = QEU_NOREPO;
	       
	    /* open IIPROCEDURE_PARAMETER */
	    STRUCT_ASSIGN_MACRO(qqeu, dbppqeu);

	    dbppqeu.qeu_tab_id.db_tab_base  = DM_B_IIPROC_PARAM_TAB_ID;
	    dbppqeu.qeu_tab_id.db_tab_index  = DM_I_IIPROC_PARAM_TAB_ID;
	    status = qeu_open(qef_cb, &dbppqeu);
	    if (status != E_DB_OK)
	    {
	        error = dbppqeu.error.err_code;
	        break;
	    }
	    dbpp_opened = TRUE;

	    dbppqeu.qeu_flag = 0;
	    dbppqeu.qeu_klen = 2;
	    dbppqeu.qeu_key = dbppkey_ptr_array;

	    dbppkey_ptr_array[0] = &dbppkey_array[0];
	    dbppkey_ptr_array[0]->attr_number = DM_1_IIPROC_PARAM_KEY;
	    dbppkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	    dbppkey_ptr_array[0]->attr_value = (char *) &currentid.db_tab_base;
    
	    dbppkey_ptr_array[1] = &dbppkey_array[1];
	    dbppkey_ptr_array[1]->attr_number = DM_2_IIPROC_PARAM_KEY;
	    dbppkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	    dbppkey_ptr_array[1]->attr_value = (char *) &currentid.db_tab_index;
	}

	/* 
	** if need to drop one or more views, we will need to delete
	** some IITREE tuples - initialize a QEU_CB for deleting tuples
	** from IITREE now
	*/
	if (drop_view)
	{
	    tqeu.qeu_count = 0;
	    tqeu.qeu_tup_length = sizeof(DB_IITREE);
	    tqeu.qeu_output = (QEF_DATA *) NULL;
	    tqeu.qeu_getnext = QEU_REPO;
	    tqeu.qeu_klen = 3;       
	    tqeu.qeu_key = tkey_ptr_array;

	    tkey_ptr_array[0] = &tkey_array[0];
	    tkey_ptr_array[0]->attr_number = DM_1_TREE_KEY;
	    tkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	    tkey_ptr_array[0]->attr_value = (char *) &currentid.db_tab_base;

	    tkey_ptr_array[1] = &tkey_array[1];
	    tkey_ptr_array[1]->attr_number = DM_2_TREE_KEY;
	    tkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	    tkey_ptr_array[1]->attr_value =
	        (char *) &currentid.db_tab_index;
    
	    qmode = DB_VIEW;
	    tkey_ptr_array[2] = &tkey_array[2];
	    tkey_ptr_array[2]->attr_number = DM_3_TREE_KEY;
	    tkey_ptr_array[2]->attr_operator = DMR_OP_EQ;
	    tkey_ptr_array[2]->attr_value = (char *) &qmode;
	}

	/*
	** if need to drop one or more views, a dbevent, a sequence, or may need to drop one
	** or more dbprocs, we will need to delete IIQRYTEXT tuples - initialize
	** a QEU_CB for deleting tuples from IIQRYTEXT now
	*/
	if (drop_view || drop_event || drop_sequence || dbp_affected)
	{
	    qqeu.qeu_count = 0;
	    qqeu.qeu_tup_length = sizeof(DB_IIQRYTEXT);
	    qqeu.qeu_output = (QEF_DATA *) NULL;
	    qqeu.qeu_getnext = QEU_REPO;
	    qqeu.qeu_klen = 2;       
	    qqeu.qeu_key = qkey_ptr_array;

	    qkey_ptr_array[0] = &qkey_array[0];
	    qkey_ptr_array[0]->attr_number = DM_1_QRYTEXT_KEY;
	    qkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	    qkey_ptr_array[0]->attr_value =
	        (char *) &queryid.db_qry_high_time;

	    qkey_ptr_array[1] = &qkey_array[1];
	    qkey_ptr_array[1]->attr_number = DM_2_QRYTEXT_KEY;
	    qkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	    qkey_ptr_array[1]->attr_value =
	        (char *) &queryid.db_qry_low_time;
	}

        /* 
        ** if we need to drop any specific rules (i.e. not as a part of 
        ** destroying the table on which they were defined), we will need to 
	  ** read IIRULEIDX and do a TID-join into IIRULE.
        */

        if (drop_specific_rules)
        {
            /* allocate space for IIRULE and IIRULEIDX tuples */
    	    ulm.ulm_psize = sizeof(DB_IIRULE);
    	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
	        ulm_closestream(&ulm);
	        error = E_QE001E_NO_MEM;
	        break;
	    }
	    rultuple = (DB_IIRULE *) ulm.ulm_pptr;

	    ulm.ulm_psize = sizeof(DB_IIRULEIDX);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
	        ulm_closestream(&ulm);
	        error = E_QE001E_NO_MEM;
	        break;
	    }
	    xrultuple = (DB_IIRULEIDX *) ulm.ulm_pptr;


	    /* open IIRULE */
	    STRUCT_ASSIGN_MACRO(qqeu, rulqeu);
	    rulqeu.qeu_tab_id.db_tab_base  = DM_B_RULE_TAB_ID;
	    rulqeu.qeu_tab_id.db_tab_index = DM_I_RULE_TAB_ID;
	    
	    status = qeu_open(qef_cb, &rulqeu);
	    if (status != E_DB_OK)
	    {
		error = rulqeu.error.err_code;
		break;
	    }
	    rul_opened = TRUE;
	    
	    /* open IIRULEIDX */
	    STRUCT_ASSIGN_MACRO(qqeu, xrulqeu);
	    xrulqeu.qeu_tab_id.db_tab_base  = DM_B_RULEIDX_TAB_ID;
	    xrulqeu.qeu_tab_id.db_tab_index = DM_I_RULEIDX_TAB_ID;
	    
	    status = qeu_open(qef_cb, &xrulqeu);
	    if (status != E_DB_OK)
	    {
		error = xrulqeu.error.err_code;
		break;
	    }
	    xrul_opened = TRUE;


	    /* set up a QEU_CB fro reading IIRULE tuples */
	    rulqeu.qeu_count = 1;
	    rulqeu.qeu_tup_length = sizeof(DB_IIRULE);
	    rulqeu.qeu_output = &rulqef_data;
	    rulqef_data.dt_next = 0;
	    rulqef_data.dt_size = sizeof(DB_IIRULE);
	    rulqef_data.dt_data = (PTR) rultuple;

	    /* set up a QEU_CB fro reading IIRULEIDX tuples */
	    xrulqeu.qeu_count = 1;
	    xrulqeu.qeu_tup_length = sizeof(DB_IIRULEIDX);
	    xrulqeu.qeu_output = &xrulqef_data;
	    xrulqef_data.dt_next = 0;
	    xrulqef_data.dt_size = sizeof(DB_IIRULEIDX);
	    xrulqef_data.dt_data = (PTR) xrultuple;

	    /*
	    ** initialize keys that will be used to access IIRULEIDX
	    */
	    xrulqeu.qeu_key = xrulkey_ptr_array;
	    xrulqeu.qeu_getnext = QEU_REPO;
	    xrulqeu.qeu_klen = 2;       

	    /*
	    ** we will key into IIRULEIDX using rule id recorded as the
	    ** dependent object id in IIDBDEPENDS
	    */
	    xrulkey_ptr_array[0] = &xrulkey_array[0];
	    xrulkey_ptr_array[0]->attr_number = DM_1_RULEIDX_KEY;
	    xrulkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	    xrulkey_ptr_array[0]->attr_value =
		(char *) &currentid.db_tab_base;

	    xrulkey_ptr_array[1] = &xrulkey_array[1];
	    xrulkey_ptr_array[1]->attr_number = DM_2_RULEIDX_KEY;
	    xrulkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	    xrulkey_ptr_array[1]->attr_value =
		(char *) &currentid.db_tab_index;

	    /*
	    ** IIRULE will only be accessed by doing a TID-join from IIRULEIDX
	    */
	    rulqeu.qeu_flag = QEU_BY_TID;
	    rulqeu.qeu_getnext = QEU_NOREPO;
	    rulqeu.qeu_klen = 0;
	    rulqeu.qeu_key = (DMR_ATTR_ENTRY **) NULL;
        }

	for(;;)
	{
	    bool		drop_cur_dbp;

            status = qeu_get(qef_cb, &tempqeu);
	    if (status != E_DB_OK)
	    {
		error = tempqeu.error.err_code;
		break;
	    }
	    
	    tempqeu.qeu_getnext = QEU_NOREPO;

	    /*
	    ** if this row was inserted to remind us to look for objects defined
	    ** on other tables which depend on constraints defined on table
	    ** being dropped, simply ignore that row
	    ** if this row describes a synonym, skip it because the caller is 
	    ** responsible for destroying it
	    */
	    if (   (   temprow->temp_obj_type == DB_CONS
		    && temprow->temp_flags == ALL_CONSTRAINTS)
		|| temprow->temp_obj_type == DB_SYNONYM
	       )
	    {
		continue;
	    }
	    
            currentid.db_tab_base = temprow->temp_base;
            currentid.db_tab_index = temprow->temp_index;
	    cur_obj_type = (i4) temprow->temp_obj_type;
            cons_number = queryid.db_qry_high_time = temprow->temp_1qryid;
            queryid.db_qry_low_time = temprow->temp_2qryid;

    	    /*
	    ** if this tuple was created to remind us to alter a table's 
	    **    timestamp 
	    **   - do so 
	    ** else if this is a table, an index or a view T,
	    **	 - destroy any qrymod objects (e.g. synonyms, comments, permits)
	    **     that exist on T,
	    **   - destroy T,
	    **   - delete partitioning catalog rows if T was partitioned
	    ** else if this is a dbproc P
	    **	 - if we need to force invalidation of this dbproc QPs
	    **	     - do so 
	    **   - if this dbproc needs to be dropped (e.g. caller requested 
	    **     that this dbproc be dropped or it is a system-generated 
	    **     dbproc which depends on an object which is being dropped),
	    **       delete IIPROCEDURE tuple for P
	    **   - otherwise mark P as "dormant" in IIPROCEDURE
	    **   - destroy permits on P,
	    ** else if this is a dbevent 
	    **   - destroy E
	    **	 - destroy permits on E
	    ** else if this is a sequence
	    **   - destroy S
	    **	 - destroy permits on S
	    ** else if this is a rule R
	    **   - destroy R
	    ** else if this is a constraint C
	    **   - destroy C
	    */

	    /* b108474 (b108635): Addition to above description. During drop
	    ** column cascade operation it is not clear at this stage, if 
	    ** these objects need to be destroyed. This will be done now by
	    ** checking column dependencies.
	    */

	    switch (cur_obj_type)
	    {
		case DB_TABLE:
		{
		    if (temprow->temp_flags == ALTER_TIMESTAMP)
		    {
			DB_TAB_ID		base_tbl_id;

			/* 
			** this tuple was entered to remind us to alter
			** timestamp of a base table to force invalidation of
			** QPs which may depend on the view being dropped
			*/
			base_tbl_id.db_tab_base  = temprow->temp_base;
			base_tbl_id.db_tab_index = temprow->temp_index;

			status = qeu_mdfy_timestamp(qef_cb, &base_tbl_id,
			    &error);
			if (DB_FAILURE_MACRO(status))
			{
			    break;
			}
			else
			{
			    /* we are done with this tuple */
			    continue;
			}
		    }

		    /* otherwise, fall through */
		}
		case DB_VIEW:
		case DB_INDEX:
	        {
		    QEUQ_CB		tqeuq_cb;
		    DB_TAB_ID		db_tab_id[1];
		    DMU_CHAR_ENTRY	chr;
		    bool		found_another = FALSE;
	    
		    STRUCT_ASSIGN_MACRO(*qeuq_cb, tqeuq_cb);

		    /*
		    ** set up information needed by qeu_drop_synonym(),
		    ** qeu_dprot(), and qeu_dcomment().
		    */
		    db_tab_id[0].db_tab_base = currentid.db_tab_base;
		    db_tab_id[0].db_tab_index = currentid.db_tab_index;
		    tqeuq_cb.qeuq_rtbl = db_tab_id;
		       
		    /* Fix up any dependent iitree */
		    if ( (cur_obj_type == DB_VIEW) &&
		    	 (temprow->temp_flags & DROP_BY_COLUMN_CHECK) )
		    {	
		       DB_TAB_ID	dbv_tab_id;
		       dbv_tab_id.db_tab_base = temprow->temp_1qryid;
		       dbv_tab_id.db_tab_index = temprow->temp_2qryid;
		       status = qeu_v_col_drop(&ulm, qef_cb, &tqeu,
		    			   &currentid, 
		    			   &dbv_tab_id,
		    			   (DB_TREE_ID *) NULL,
		    			   (i2) 0,
		    			   (DB_NAME *) NULL,
		    			   (DB_OWN_NAME *) NULL,
		    			   cur_obj_type,
		    			   drop_col_attno,
		       			   cascade,  
		    			   &err_blk);

		       if (status != E_DB_OK)
		       {
		           error = err_blk.err_code;
		           break;
		       }

		       continue;
		    }
    
		    /* these are needed for dropping protection tuples */
		    tqeuq_cb.qeuq_qid.db_qry_high_time = 0;
		    tqeuq_cb.qeuq_qid.db_qry_low_time = 0;
		    tqeuq_cb.qeuq_tsz = 1;
    
		    /*
		    ** when destroying table, view, or index, invalidation of 
		    ** QPs dependent the object being dropped will be taken 
		    ** care of here - make sure that functions responsible for 
		    ** destroying qrymod objects do not try to get in on the act
		    */
		    tqeuq_cb.qeuq_flag_mask &= ~QEU_FORCE_QP_INVALIDATION;

		    if ( (cur_obj_type == DB_TABLE) && (drop_col) )
		    {
		       tqeuq_cb.qeuq_ano = drop_col_attno;
		       tqeuq_cb.qeuq_aid = drop_col_attid;
		       tqeuq_cb.qeuq_flag_mask |= QEU_DROP_COLUMN;

		       if (cascade)
			  tqeuq_cb.qeuq_flag_mask |= QEU_DROP_CASCADE;

		    }

		    status = qeu_dqrymod_objects(qef_cb, &tqeuq_cb,
		        (i4 *) NULL,
			(i4 *) NULL);

		    if (DB_FAILURE_MACRO(status))
		    {
		        if (tqeuq_cb.error.err_code == 
				E_QE0031_NONEXISTENT_TABLE)
		        {
			    /* ignore nonexistent tables. per comment above */
			    status = E_DB_OK;
			    continue;
		        }
		        else
		        {
			    error = tqeuq_cb.error.err_code;
			    break;
		        }
		    }
    

		    /* b108474 (b108636) check if the dependent referential
		    ** constraint exists. Basically here for dependent
		    ** indexes it's only necessary to have iidbdepends
		    ** cleaned up properly. 
		    */

		    if (cur_obj_type == DB_INDEX &&
			  temprow->temp_flags & DROP_COL_CHK_CONS)
		    {
			 /* Set base table id to check constraint*/
			  currentid.db_tab_base = temprow->temp_1qryid;
			  currentid.db_tab_index = temprow->temp_2qryid;
			  iqeu.qeu_klen = 2;
			  iqeu.qeu_getnext = QEU_REPO;
			  iqeu.qeu_qarg = (PTR) &temprow->temp_i_qid;
			  iqeu.qeu_qual = (PTR) qeu_q_integ;

			  status = qeu_get(qef_cb, &iqeu);

			  /* Set it back to the object id, as it was */
			  currentid.db_tab_base = temprow->temp_base;
			  currentid.db_tab_index = temprow->temp_index;

			  if (!DB_FAILURE_MACRO(status))
			  {
				 /*Constraint exists let's keep it as is*/
				 continue;
			  }
			  else
			  {
				 if (iqeu.error.err_code != E_QE0015_NO_MORE_ROWS)
				 {
				     error = iqeu.error.err_code;
				     break;
				 }
				 else
				 {
				     /* Dependent constraint has been dropped.
				     ** Let's have this system generated index
				     ** dropped as well
				     */
				     error = E_QE0000_OK;
				     status = E_DB_OK;
				 }
			  }
		    }

		    /* Next, see if this is an index being destroyed
		    ** as part of "drop constraint" processing. If so, 
		    ** and if there is still another constraint using 
		    ** the index, we mustn't drop it yet. This function
		    ** loops over "iidbdepends" looking for other 
		    ** constraints on which the index depends. 
		    **
		    ** Empirical observation (definitely not a reading
		    ** of the actual code - that would take far too
		    ** long!!) has shown that the iidbdepends
		    ** rows for the target constraint are already gone.
		    ** So the called function just looks for one more 
		    ** iidbdepends row refer3encing the index.
		    **
		    ** FIXME LATER: (Seen during b108474 fixing)
		    ** CHECK_OTHER_CONS has never been set for drop col
		    ** processing, even an index used by 2 constraints
		    ** could have been dropped.
		    ** Whoever fixes this, needs to be aware that the
		    ** above empirical observation may have changed,
		    ** especially for drop column processing.
		    */

		    /* If we are dropping a constraint check to see
		    ** whether the underlying index is a user index
		    */
		    if (dtuple->dbv_itype == DB_CONS)
		    {
			DB_TAB_ID        indexid;
                        DMT_TBL_ENTRY     dmt_table;
                        DMT_SHW_CB       dmt_show;

			if(dtuple->dbv_dtype == DB_INDEX) 
			{
			    indexid.db_tab_base = temprow->temp_base;
                            indexid.db_tab_index = temprow->temp_index;

                            /*
                            ** initialize the control block that will be 
                            ** used to look up information on tables and views
                            */
                            dmt_show.type = DMT_SH_CB;
                            dmt_show.length = sizeof(DMT_SHW_CB);
                            dmt_show.dmt_char_array.data_in_size  = 0;
                            dmt_show.dmt_char_array.data_out_size = 0;
                            dmt_show.dmt_flags_mask = DMT_M_TABLE;
                            dmt_show.dmt_db_id = qeuq_cb->qeuq_db_id;
                            dmt_show.dmt_session_id = qeuq_cb->qeuq_d_id;
                            dmt_show.dmt_table.data_address = (PTR) &dmt_table;
                            dmt_show.dmt_table.data_in_size = 
                                                    sizeof(DMT_TBL_ENTRY);
                            dmt_show.dmt_char_array.data_address = (PTR) NULL;
                            dmt_show.dmt_char_array.data_in_size = 0;
                            dmt_show.dmt_char_array.data_out_size = 0;
			    STRUCT_ASSIGN_MACRO(indexid, dmt_show.dmt_tab_id);

                            status = dmf_call(DMT_SHOW, &dmt_show);
                            if (   status != E_DB_OK
                                && dmt_show.error.err_code != 
                                        E_DM0114_FILE_NOT_FOUND)
                            {
                                error = dmt_show.error.err_code;
                                break;
                            }

			    /* Do not drop user indexes as part of a
			    ** drop constraint.
			    **
			    ** We manage to get here for base tables
			    ** Do not skip the drop in these instances.
			    */
                            if(((dmt_table.tbl_2_status_mask & 
                                         DMT_SYSTEM_GENERATED) == 0) &&
                               (dmt_table.tbl_id.db_tab_index != 0))
                            {
                                break;
                            }

			}
			   
			if(dtuple->dbv_dtype == DB_INDEX &&
			   temprow->temp_flags & CHECK_OTHER_CONS)
		        {
			    DB_TAB_ID indexid;

			    indexid.db_tab_base = temprow->temp_base;
			    indexid.db_tab_index = temprow->temp_index;
			    status = qeu_d_check_consix(qef_cb, qeuq_cb,
				    &ulm, &indexid, &error, &found_another);
			    if (status != E_DB_OK) break;
			}
		    }


		    /* 
		    ** Now destroy the table.  This insures its existence
		    ** before doing all of the other work, and places
		    ** an exclusive lock on the table. Note, ignore
		    ** error that table does not exist.  If a view 
		    ** depended on more than one table, if one of the
		    ** other tables has already been deleted, then 
		    ** the view may have already been deleted.
		    **
		    ** if we are dropping a temporary table, tell
		    ** dmu_destroy_table() about it
		    **
		    ** If the table is partitioned, DMF drops all the
		    ** partitions (since it's going through and doing
		    ** all indexes, etc anyway).
		    */

		    if (found_another) break;	/* don't drop */
		    if ( (cur_obj_type == DB_TABLE) && (drop_col) )
		    {
			qeuq_cb->qeuq_keydropped = tqeuq_cb.qeuq_keydropped;
			qeuq_cb->qeuq_keypos = tqeuq_cb.qeuq_keypos;
			qeuq_cb->qeuq_keyattid = tqeuq_cb.qeuq_keyattid;
		        goto flushcache;
		    } 
		    dmu_cb.length = sizeof(DMU_CB);
		    dmu_cb.type = DMU_UTILITY_CB;
		    dmu_cb.dmu_flags_mask = 0;
		    dmu_cb.dmu_tbl_id.db_tab_base = currentid.db_tab_base;
		    dmu_cb.dmu_tbl_id.db_tab_index = currentid.db_tab_index;
		    dmu_cb.dmu_db_id = qeuq_cb->qeuq_db_id;
		    dmu_cb.dmu_tran_id = qef_cb->qef_dmt_id;
		    if (qeuq_cb->qeuq_flag_mask & QEU_DROP_TEMP_TABLE)
		    {
		        chr.char_id = DMU_TEMP_TABLE;
		        chr.char_value = DMU_C_ON;
		        dmu_cb.dmu_char_array.data_address = (char *) &chr;
		        dmu_cb.dmu_char_array.data_in_size = 
			    sizeof(DMU_CHAR_ENTRY);
			dmu_cb.dmu_table_name = qeuq_cb->qeuq_tabname;
		    }
		    else
		    {
		        dmu_cb.dmu_char_array.data_address = 0;
		        dmu_cb.dmu_char_array.data_in_size = 0;
		    }
		    status = dmf_call(DMU_DESTROY_TABLE, &dmu_cb);
		    if (DB_FAILURE_MACRO(status))
		    {
		        /* ignore nonexistent tables. per comment above */
		        if (dmu_cb.error.err_code == E_DM0054_NONEXISTENT_TABLE)
		        {
			    status = E_DB_OK;
			    continue;
		        }
		        error = dmu_cb.error.err_code;
		        break;
		    }

		    /* if the table was partitioned, wipe the partitioning
		    ** info out of the partitioning catalogs.
		    ** FIXME This will need to be changed when we allow
		    ** partitioned global indexes, as they will have their
		    ** own partitioning info.  I guess we'll need a dmt-show
		    ** or something to discover whether an index is
		    ** partitioned.  For now we only need to worry about the
		    ** base object being dropped.
		    */
		    if (num_partitions > 0
			&& currentid.db_tab_base == tbl_entry->tbl_id.db_tab_base
			&& currentid.db_tab_index == tbl_entry->tbl_id.db_tab_index)
		    {
			/* Don't mess with the "invalidate" rdfcb.
			** We're already using 6K of stack for this routine,
			** what's another few hundred.
			** Delete by setting to "No partitions".
			*/
			DB_PART_DEF empty_partdef;
			RDF_CB del_rdfcb;

			empty_partdef.ndims = 0;
			empty_partdef.nphys_parts = 0;

			qeu_rdfcb_init((PTR) &del_rdfcb, qef_cb);
			STRUCT_ASSIGN_MACRO(currentid, del_rdfcb.rdf_rb.rdr_tabid);
			del_rdfcb.rdf_rb.rdr_newpart = &empty_partdef;
			status = rdf_call(RDF_PART_UPDATE, &del_rdfcb);
			if (DB_FAILURE_MACRO(status))
			{
			    error = rdf_inv_cb->rdf_error.err_code;
			    break;
			}
		    }
		    /*
		    ** finally, tell RDF to flush the entry for the table along
		    ** with any trees that may have been associated with permits
		    ** on that table from RDF cache
		    */
flushcache:
		    STRUCT_ASSIGN_MACRO(currentid, rdf_inv_rb->rdr_tabid);
		    rdf_inv_rb->rdr_2types_mask |= RDR2_KILLTREES;
		
		    status = rdf_call(RDF_INVALIDATE, rdf_inv_cb);
		    if (DB_FAILURE_MACRO(status))
		    {
		        error = rdf_inv_cb->rdf_error.err_code;
		        break;
		    }

		    /*
		    ** reset the RDR2_KILLTREES to avoid having to reset it 
		    ** before other invocations of rdf_invalidate()
		    */
		    rdf_inv_rb->rdr_2types_mask &= ~RDR2_KILLTREES;
		    
		    /*
		    ** If this was an index, invalidate the base table defns, too.
		    */
		    if (cur_obj_type == DB_INDEX)
		    {
			rdf_inv_rb->rdr_tabid.db_tab_index = 0;
			status = rdf_call(RDF_INVALIDATE, rdf_inv_cb);
			if (DB_FAILURE_MACRO(status))
			{
		            error = rdf_inv_cb->rdf_error.err_code;
		            break;
			}
		    }

		    if ( (cur_obj_type == DB_TABLE) && (drop_col) )
			continue;
		    else
			break;
	        }
		case DB_DBP:
	        {
		    QEUQ_CB		pqeuq_cb;
		    DB_PROCEDURE	*dbp_tuple;
		    DB_TAB_ID		dbp_ubt_id;

		    dbp_ubt_id.db_tab_base = dbp_ubt_id.db_tab_index = 0;

		    if (temprow->temp_flags & DROP_THIS_DBPROC)
		    {
			/* 
			** caller has explicitly requested that we destroy this
			** dbproc
			*/

		        DMR_ATTR_ENTRY	    dbpkey_array[2];
		        DMR_ATTR_ENTRY	    *dbpkey_ptr_array[2];
    
		        /* 
			** get IIPROCEDURE tuple describing this dbproc by
			** name and owner supplied by the caller in order to
			** determine id of its underlying base table (unless
			** the dbproc is abandoned, of course); 
			**
			** then delete the tuple just read
			*/
		        dbp_tuple = 
			    (DB_PROCEDURE *) qeuq_cb->qeuq_dbp_tup->dt_data;

		        dbpqeu.qeu_flag = 0;
		        dbpqeu.qeu_klen = 2;
			dbpqeu.qeu_getnext = QEU_REPO;
		        dbpqeu.qeu_key = dbpkey_ptr_array;
	    	
		        dbpkey_ptr_array[0] = &dbpkey_array[0];
		        dbpkey_ptr_array[0]->attr_number = DM_1_DBP_KEY;
		        dbpkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
		        dbpkey_ptr_array[0]->attr_value = 
			    (char *) &dbp_tuple->db_dbpname;
	    	
		        dbpkey_ptr_array[1] = &dbpkey_array[1];
		        dbpkey_ptr_array[1]->attr_number = DM_2_DBP_KEY;
		        dbpkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
		        dbpkey_ptr_array[1]->attr_value = 
			    (char *) &dbp_tuple->db_owner;
    
		        status = qeu_get(qef_cb, &dbpqeu);

			if (status != E_DB_OK)
			{
			    error = dbpqeu.error.err_code;
			    break;
			}
    
			/* save id of the dbproc's UBT if one is known */
			if (   dbptuple->db_mask[0] & DB_DBP_INDEP_LIST
                            && dbptuple->db_dbp_ubt_id.db_tab_base != 0)
			{
			    dbp_ubt_id.db_tab_base = 
				dbptuple->db_dbp_ubt_id.db_tab_base;
			    dbp_ubt_id.db_tab_index = 
				dbptuple->db_dbp_ubt_id.db_tab_index;
			}

			/* 
			** delete the IIPROCEDURE tuple which we have just read
			** 
			** it is important that we reset qeu_klen, qeu_getnext,
			** and qeu_key in dbpqeu to their "usual" values since 
			** most of the time we access IIPROCEDURE via a TID 
			** join from IIXPROCEDURE + we need to do it anyway to 
			** tell qeu_delete() to delete the "current" tuple
			*/
			dbpqeu.qeu_klen = 0;
			dbpqeu.qeu_getnext = QEU_NOREPO;
			dbpqeu.qeu_key = (DMR_ATTR_ENTRY **) NULL;

			status = qeu_delete(qef_cb, &dbpqeu);

		        /* 
		        ** reset qeu_flag in dbpqeu which was changed in this
		        ** block - it is rather important since most of the
		        ** time we access IIPROCEDURE via a TID join from
		        ** IIXPROCEDURE
		        */
		        dbpqeu.qeu_flag = QEU_BY_TID;
    
		        if (status != E_DB_OK)
		        {
		            error = dbpqeu.error.err_code;
		            break;
		        }

			drop_cur_dbp = TRUE;
		    }
		    else
		    {
			/* 
			** look up description of the dbproc (by doing a TID 
			** join from IIXPROCEDURE); if it is system-generated, 
			** it must be destroyed, otherwise it must be marked 
			** dormant
			*/
		        status = qeu_get(qef_cb, &xdbpqeu);
		        if (status != E_DB_OK)
		        {
		            error = xdbpqeu.error.err_code;
		            break;
		        }
    
		        /*
		        ** now do a TID-join into IIPROCEDURE
		        */
		        dbpqeu.qeu_tid = xdbptuple->dbx_tidp;
    
		        status = qeu_get(qef_cb, &dbpqeu);
		        if (status != E_DB_OK)
		        {
		            error = dbpqeu.error.err_code;
		            break;
		        }

			/* 
			** save id of the dbproc's UBT if one is known and we 
			** were asked to force invalidation of this dbproc's QPs
			*/
			if (   temprow->temp_flags & FORCE_DBP_QP_INVALIDATION
			    && dbptuple->db_mask[0] & DB_DBP_INDEP_LIST
                            && dbptuple->db_dbp_ubt_id.db_tab_base != 0)
			{
			    dbp_ubt_id.db_tab_base = 
				dbptuple->db_dbp_ubt_id.db_tab_base;
			    dbp_ubt_id.db_tab_index = 
				dbptuple->db_dbp_ubt_id.db_tab_index;
			}

			/* b108474 check if the dependent referential
			** constraint exists in order to make the call,
			** if this system generated dbp needs to be dropped
			** or not.
			*/

			if (temprow->temp_flags & DROP_COL_CHK_CONS)
			{
			    /* Set base table id to check constraint*/
			    currentid.db_tab_base = temprow->temp_1qryid;
			    currentid.db_tab_index = temprow->temp_2qryid;
			    iqeu.qeu_klen = 2;
			    iqeu.qeu_getnext = QEU_REPO;
			    iqeu.qeu_qarg = (PTR) &temprow->temp_i_qid;
			    iqeu.qeu_qual = (PTR) qeu_q_integ;

			    status = qeu_get(qef_cb, &iqeu);

			    /* Set it back to the object id, as it was */
			    currentid.db_tab_base = temprow->temp_base;
			    currentid.db_tab_index = temprow->temp_index;

			    if (!DB_FAILURE_MACRO(status))
			    {
				   /*Constraint exists let's keep it as is*/
				   continue;
			    }
			    else
			    {
				   if (iqeu.error.err_code != E_QE0015_NO_MORE_ROWS)
				   {
				 	 error = iqeu.error.err_code;
				  	 break;
				   }
				   else
				   {
				  	/* Dependent constraint has been dropped.
					** Let's have this system generated dbp
					** dropped as well
				  	*/
				  	  error = E_QE0000_OK;
				  	  status = E_DB_OK;
				   }
			    }
			}
			

			if (dbptuple->db_mask[0] & DBP_SYSTEM_GENERATED)
			{
			    /* 
			    ** dbproc was created by the system - since an 
			    ** object on which it depends is being destroyed, 
			    ** it shall be destroyed as well. 
			    **
			    ** first we delete the IIPROCEDURE tuple by TID
			    */

	    		    dbpqeu.qeu_tup_length = sizeof(DB_PROCEDURE);
			    status = qeu_delete(qef_cb, &dbpqeu);
			    if (status != E_DB_OK)
			    {
				error = dbpqeu.error.err_code;
				break;
			    }

			    drop_cur_dbp = TRUE;

			    /* 
			    ** need to copy query text id from the IIPROCEDURE 
			    ** tuple into "queryid" to ensure that the text 
			    ** gets deleted (until this point, queryid will have
			    ** contained 0's since those are the values we 
			    ** insert into the temp table for all dbprocs 
			    ** except for the one being explicitly dropped by 
			    ** the user)
			    */
			    STRUCT_ASSIGN_MACRO(dbptuple->db_txtid, queryid);
			}
			else
			{
			    drop_cur_dbp = FALSE;
			}

		        /*
		        ** set dbp_tuple to dbptuple since the code below 
			** expects dbp_tuple to point at a IIPROCEDURE tuple 
			** describing the dbproc which was either dropped or 
			** marked dormant
		        */
		        dbp_tuple = dbptuple;
		    }
    
		    /* 
		    ** if we need to drop this dbproc
		    **   (we have already deleted the IIPROCEDURE tuple)
		    **   we need to delete description of the dbproc parameters
		    **   from IIPROCEDURE_PARAMETERS
		    ** otherwise 
		    **   mark this dbproc as dormant in IIPROCEDURE
		    **
		    ** if necessary and possible, alter timestamp of the
		    ** dbproc's underlying base table to force invalidation of
		    ** QPs involving this dbproc
		    **
		    ** flush this dbproc's QP(s) from QSF cache
		    **   
		    ** invalidate the RDF cache entry corresponding to this 
		    ** dbproc
		    */
    
		    if (drop_cur_dbp)
		    {
	    		dbppqeu.qeu_tup_length = sizeof(DB_PROCEDURE_PARAMETER);
		        /* delete dbproc parameters by proc id */
		        status = qeu_delete(qef_cb, &dbppqeu);
		        if (status != E_DB_OK)
		        {
		            error = dbppqeu.error.err_code;
		            break;
		        }

                        /* b96033 - Only drop permits for DROPPED DBP */
                        /* now destroy all permits on the dbproc */
   
                        STRUCT_ASSIGN_MACRO(*qeuq_cb, pqeuq_cb);
 
                        /*
                        ** set up information needed by qeu_dprot()
                        */
                        pqeuq_cb.qeuq_rtbl = &currentid;
                        pqeuq_cb.qeuq_pno = 0;
                        pqeuq_cb.qeuq_permit_mask =
                            QEU_PERM | QEU_DBP_PROTECTION | QEU_DROP_ALL |
                            QEU_SKIP_ABANDONED_OBJ_CHECK;
 
                        /*
                        ** note that we have not set QEU_FORCE_QP_INVALIDATION
                        ** bit here - because we have already taken care of
                        ** altering the timestamp of the dbproc's UBT                                   */

                        pqeuq_cb.qeuq_flag_mask = 0;
 
                        /*
                        ** if the user has requested that destroy this dbproc,
                        ** dbpqef_data will not contain a copy of a tuple
                        ** describing the dbproc, but qeuq_cb->qeuq_dbp_tup
                        ** (supplied by the caller) will
                        */
 
                        if (temprow->temp_flags != DROP_THIS_DBPROC)
                            pqeuq_cb.qeuq_dbp_tup = &dbpqef_data;
 
                        status = qeu_dprot(qef_cb, &pqeuq_cb);
                        if (DB_FAILURE_MACRO(status))
                        {
                            error = pqeuq_cb.error.err_code;
                            break;
                        }

		    }
		    else
		    {
		        /*
			** IIPROCEDURE tuple has already been retrieved; now we
		        ** will replace the tuple after resetting dbp_mask1 to 
			** indicate that the dbproc is "dormant"
		        */

		        /*
		        ** reset bits indicating that a dbproc is grantable, 
			** active, or has a valid independent privilege list
		        */
		        dbptuple->db_mask[0] &=
		            ~(DB_DBPGRANT_OK | DB_ACTIVE_DBP | 
				  DB_DBP_INDEP_LIST);
    
			/* zero out id of the dbproc's underlying base table */
			dbptuple->db_dbp_ubt_id.db_tab_base = 
			    dbptuple->db_dbp_ubt_id.db_tab_index = 0;

		        /*
		        ** we are replacing by tid - conveniently qeu_flag is 
			** set to QEU_BY_TID
		        */
		        status = qeu_replace(qef_cb, &dbpqeu);
		        if (status != E_DB_OK)
		        {
		            error = dbpqeu.error.err_code;
		            break;
		        }
		    }
    
		    /*
		    ** having read the IIPROCEDURE tuple, we would save the
		    ** id of the dbproc's underlying base table iff we need to
		    ** alter its timestamp, which is what we are about to do
		    */
		    if (dbp_ubt_id.db_tab_base != 0)
		    {
                        status = qeu_mdfy_timestamp(qef_cb, &dbp_ubt_id, 
			    &error);
                        if (DB_FAILURE_MACRO(status))
                        {
                            break;
                        }
		    }

		    /* 
		    ** if we dropped or marked dormant a dbproc, destroy its QP
		    */

		    status = qeu_destr_dbp_qp(qef_cb, &dbptuple->db_owner,
			&dbptuple->db_dbpname, &qeuq_cb->error);
		    if (DB_FAILURE_MACRO(status))
		    {
			err_already_reported = TRUE;
			error = qeuq_cb->error.err_code;
			break;
		    }

		    /*
		    ** finally, call RDF to invalidate cache entry for this 
		    ** dbproc
		    */
		    STRUCT_ASSIGN_MACRO(dbp_tuple->db_dbpname,
				        rdf_inv_rb->rdr_name.rdr_prcname);
		    STRUCT_ASSIGN_MACRO(dbp_tuple->db_owner,
				        rdf_inv_rb->rdr_owner);

		    rdf_inv_rb->rdr_types_mask |= RDR_PROCEDURE;
		
		    status = rdf_call(RDF_INVALIDATE, rdf_inv_cb);
		    if (DB_FAILURE_MACRO(status))
		    {
		        error = rdf_inv_cb->rdf_error.err_code;
		        break;
		    }

		    /*
		    ** reset RDR_PROCEDURE bit in rdr_types_mask to avoid having
		    ** to unnecessarily reset it elsewhere
		    */
		    rdf_inv_rb->rdr_types_mask &= ~RDR_PROCEDURE;

		    break;
	        }
		case DB_EVENT:
	        {
		    QEUQ_CB		    pqeuq_cb;
		    DB_IIEVENT	    	    *evtuple;
		    DMR_ATTR_ENTRY	    evkey_array[2];
		    DMR_ATTR_ENTRY	    *evkey_ptr_array[2];
    
		    /*
		    ** first open IIEVENT and delete the tuple 
		    */
    
		    STRUCT_ASSIGN_MACRO(qqeu, evqeu);
    
		    evqeu.qeu_flag = DMT_U_DIRECT;
		    evqeu.qeu_tab_id.db_tab_base  = DM_B_EVENT_TAB_ID;
		    evqeu.qeu_tab_id.db_tab_index = DM_I_EVENT_TAB_ID;
    
		    status = qeu_open(qef_cb, &evqeu);
		    if (status != E_DB_OK)
		    {
		        error = evqeu.error.err_code;
		        break;
		    }
		    event_opened = TRUE;
    
		    evqeu.qeu_count = 0;
		    evqeu.qeu_tup_length = sizeof(DB_IIEVENT);
		    evqeu.qeu_output = (QEF_DATA *) NULL;
    
		    /* delete dbevent by event name and owner */
		    evtuple  = (DB_IIEVENT *) qeuq_cb->qeuq_uld_tup->dt_data;
		    evqeu.qeu_klen = 2;
		    evqeu.qeu_key = evkey_ptr_array;
	    	
		    evkey_ptr_array[0] = &evkey_array[0];
		    evkey_ptr_array[0]->attr_number = DM_1_EVENT_KEY;
		    evkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
		    evkey_ptr_array[0]->attr_value = 
			(char *) &evtuple->dbe_name;
	    	
		    evkey_ptr_array[1] = &evkey_array[1];
		    evkey_ptr_array[1]->attr_number = DM_2_EVENT_KEY;
		    evkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
		    evkey_ptr_array[1]->attr_value = 
			(char *) &evtuple->dbe_owner;
    
		    evqeu.qeu_qual = evqeu.qeu_f_qual = NULL;
		    evqeu.qeu_qarg = evqeu.qeu_f_qarg = NULL;
    
		    status = qeu_delete(qef_cb, &evqeu);
		    if (status != E_DB_OK)
		    {
		        error = evqeu.error.err_code;
		        break;
		    }
    
		    /* now destroy permits on this dbevent */
		    STRUCT_ASSIGN_MACRO(*qeuq_cb, pqeuq_cb);
    
		    /*
		    ** note that qeuq_rtbl and qeuq_uld_tup have been set by the
		    ** caller
		    */
		    pqeuq_cb.qeuq_culd = 0; 	/* Indicates DROP EVENT */
		    pqeuq_cb.qeuq_pno = 0;	
		    pqeuq_cb.qeuq_permit_mask =
		        QEU_PERM | QEU_EV_PROTECTION | QEU_DROP_ALL |
		        QEU_SKIP_ABANDONED_OBJ_CHECK;
		        
		    /* 
		    ** note that we have not set QEU_FORCE_QP_INVALIDATION bit 
		    ** here - because if any dbproc depended on this
		    ** dbevent, it will be marked dormant and the timestamp of
		    ** its underlying base table will be invalidated
		    */
		    pqeuq_cb.qeuq_flag_mask = 0;
		        
		    status = qeu_dprot(qef_cb, &pqeuq_cb);
		    if (status != E_DB_OK)
		    {
		        error = pqeuq_cb.error.err_code;
		        break;
		    }
		    break;
	        }
		case DB_SEQUENCE:
	        {
		    QEUQ_CB		    pqeuq_cb;
		    DB_IISEQUENCE    	    *seqtuple;
		    DMR_ATTR_ENTRY	    seqkey_array[2];
		    DMR_ATTR_ENTRY	    *seqkey_ptr_array[2];
    
		    /*
		    ** first open IISEQUENCE and delete the tuple 
		    */
    
		    STRUCT_ASSIGN_MACRO(qqeu, seqqeu);
    
		    seqqeu.qeu_flag = DMT_U_DIRECT;
		    seqqeu.qeu_tab_id.db_tab_base  = DM_B_SEQ_TAB_ID;
		    seqqeu.qeu_tab_id.db_tab_index = DM_I_SEQ_TAB_ID;
    
		    status = qeu_open(qef_cb, &seqqeu);
		    if (status != E_DB_OK)
		    {
		        error = seqqeu.error.err_code;
		        break;
		    }
		    seq_opened = TRUE;
    
		    seqqeu.qeu_count = 0;
		    seqqeu.qeu_tup_length = sizeof(DB_IISEQUENCE);
		    seqqeu.qeu_output = (QEF_DATA *) NULL;
    
		    /* delete sequence by name and owner */
		    seqtuple  = (DB_IISEQUENCE *) qeuq_cb->qeuq_uld_tup->dt_data;
		    seqqeu.qeu_klen = 2;
		    seqqeu.qeu_key = seqkey_ptr_array;
	    	
		    seqkey_ptr_array[0] = &seqkey_array[0];
		    seqkey_ptr_array[0]->attr_number = DM_1_SEQ_KEY;
		    seqkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
		    seqkey_ptr_array[0]->attr_value = 
			(char *) &seqtuple->dbs_name;
	    	
		    seqkey_ptr_array[1] = &seqkey_array[1];
		    seqkey_ptr_array[1]->attr_number = DM_2_SEQ_KEY;
		    seqkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
		    seqkey_ptr_array[1]->attr_value = 
			(char *) &seqtuple->dbs_owner;
    
		    seqqeu.qeu_qual = seqqeu.qeu_f_qual = NULL;
		    seqqeu.qeu_qarg = seqqeu.qeu_f_qarg = NULL;
    
		    status = qeu_delete(qef_cb, &seqqeu);
		    if (status != E_DB_OK)
		    {
		        error = seqqeu.error.err_code;
		        break;
		    }
    
		    /* now destroy permits on this sequence */
		    STRUCT_ASSIGN_MACRO(*qeuq_cb, pqeuq_cb);
    
		    /*
		    ** note that qeuq_rtbl and qeuq_uld_tup have been set by the
		    ** caller
		    */
		    pqeuq_cb.qeuq_culd = 0; 	/* Indicates DROP SEQUENCE */
		    pqeuq_cb.qeuq_pno = 0;	
		    pqeuq_cb.qeuq_permit_mask =
		        QEU_PERM | QEU_SEQ_PROTECTION | QEU_DROP_ALL |
		        QEU_SKIP_ABANDONED_OBJ_CHECK;
		        
		    /* 
		    ** note that we have not set QEU_FORCE_QP_INVALIDATION bit 
		    ** here - because if any dbproc depended on this
		    ** sequence, it will be marked dormant and the timestamp of
		    ** its underlying base table will be invalidated
		    */
		    pqeuq_cb.qeuq_flag_mask = 0;
		        
		    status = qeu_dprot(qef_cb, &pqeuq_cb);
		    if (status != E_DB_OK)
		    {
		        error = pqeuq_cb.error.err_code;
		        break;
		    }
		    break;
	        }
		case DB_RULE:
		{
		    QEUQ_CB		rqeuq_cb;
		    bool		dep_cons_dropped=FALSE;

		    /* b108474 (b108635) check if the dependent 
		    ** referential constraint exists in order to 
		    ** make the call, if this system generated 
		    ** rule needs to be dropped or not.
		    */
		    if (temprow->temp_flags & DROP_COL_CHK_CONS)
		    {
			  /* Set base table id to check constraint*/
			  currentid.db_tab_base = temprow->temp_1qryid;
			  currentid.db_tab_index = temprow->temp_2qryid;
			  iqeu.qeu_klen = 2;
			  iqeu.qeu_getnext = QEU_REPO;
			  iqeu.qeu_qarg = (PTR) &temprow->temp_i_qid;
			  iqeu.qeu_qual = (PTR) qeu_q_integ;

			  status = qeu_get(qef_cb, &iqeu);
			  /* Set it back to the object id, as it was */
			  currentid.db_tab_base = temprow->temp_base;
			  currentid.db_tab_index = temprow->temp_index;
			  if (!DB_FAILURE_MACRO(status))
			  {
				   /*Constraint exists let's keep it as is*/
				   continue;
			  }
			  else
			  {
				if (iqeu.error.err_code != E_QE0015_NO_MORE_ROWS)
				{
				    error = iqeu.error.err_code;
				    break;
				}
				else
				{
				    /* Dependent constraint has been dropped.
				    ** Let's have this system generated rule
				    ** dropped as well
				    */
				    dep_cons_dropped=TRUE;
				    error = E_QE0000_OK;
				    status = E_DB_OK;
				}
			  }
		    }


		    /*
		    ** use rule id to look up a tuple in IIRULEIDX, do a tid
		    ** join into IIRULE, and use the information obtained 
		    ** from IIRULE tuple to call qeu_drule() to destroy that
		    ** rule
		    **
		    ** As discussed in the (03-sep-93) comment, it is possible
		    ** that the rule has already been deleted, in which case, we
		    ** want to proceed on to the next row
		    */

		    status = qeu_get(qef_cb, &xrulqeu);
		    if (DB_FAILURE_MACRO(status))
		    {
		        if (xrulqeu.error.err_code == E_QE0015_NO_MORE_ROWS)
			{
			    /*
			    ** the rule has already been dropped - on to the
			    ** next row...
			    */
			    status = E_DB_OK;
			    /*Bug 108474 - break to clean dbdepend record*/
			    if (dep_cons_dropped)
			        break;
			    
			    continue;			    
			}
			else
			{
		            error = xrulqeu.error.err_code;
		            break;
			}
	            }
    
	            rulqeu.qeu_tid = xrultuple->dbri_tidp;
	            status = qeu_get(qef_cb, &rulqeu);
	            if (DB_FAILURE_MACRO(status))
	            {
		        error = rulqeu.error.err_code;
		        break;
	            }

		    STRUCT_ASSIGN_MACRO(*qeuq_cb, rqeuq_cb);

		    /* 
		    ** set up QEUQ_CB so that qeu_drule() will perform the 
		    ** same procesing as for DROP RULE statement
		    */
		    rqeuq_cb.qeuq_cr = 1;
		    rqeuq_cb.qeuq_rul_tup = &rulqef_data;

		    /*
		    ** if the rule was defined on a table other than the
		    ** constraint which it was created to enforce, tell
		    ** qeu_drule() to force invalidation of QPs which may be
		    ** affected by the destruction of the rule; otherwise, tell
		    ** it not to bother
		    */
		    if (   rultuple->dbr_tabid.db_tab_base != 
			       temprow->temp_1qryid
			|| rultuple->dbr_tabid.db_tab_index !=
			       temprow->temp_2qryid
		       )
		    {
			rqeuq_cb.qeuq_flag_mask |= QEU_FORCE_QP_INVALIDATION;
		    }
		    else
		    {
			rqeuq_cb.qeuq_flag_mask &= ~QEU_FORCE_QP_INVALIDATION;
		    }

		    /* b108474 (b108635)
		    ** Reset the mask to make sure that it is only set for
		    ** needed cases. Shouldn't make a difference, but can't
		    ** be sure that I have checked all the eventualities.
		    */
		    if (rqeuq_cb.qeuq_flag_mask & QEU_DEP_CONS_DROPPED)
			  rqeuq_cb.qeuq_flag_mask &= ~QEU_DEP_CONS_DROPPED;
		    
		    if (drop_col) 
		    {
		       rqeuq_cb.qeuq_ano = drop_col_attno;
		       rqeuq_cb.qeuq_aid = drop_col_attid;
		       rqeuq_cb.qeuq_flag_mask |= QEU_DROP_COLUMN;

		       if (cascade)
			  rqeuq_cb.qeuq_flag_mask |= QEU_DROP_CASCADE;
		       if (rqeuq_cb.qeuq_keydropped)
			  rqeuq_cb.qeuq_ano = rqeuq_cb.qeuq_keyattid;
		       /* b108474 (b108635) constraint is dropped */
		       if (dep_cons_dropped)
			  rqeuq_cb.qeuq_flag_mask |= QEU_DEP_CONS_DROPPED;
		    }

		    status = qeu_drule(qef_cb, &rqeuq_cb, (bool) FALSE);
		    if (DB_FAILURE_MACRO(status))
		    {
			error = rqeuq_cb.error.err_code;
			break;
		    }

		    /*
		    ** need to call RDF to invalidate the rule tree + to
		    ** invalidate the cache entry for the table on which the
		    ** rule was defined 
		    */

		    STRUCT_ASSIGN_MACRO(rultuple->dbr_tabid,
					rdf_inv_rb->rdr_tabid);

		    /* first invalidate the rule tree */
		    STRUCT_ASSIGN_MACRO(rultuple->dbr_name,
			rdf_inv_rb->rdr_name.rdr_rlname);
		    STRUCT_ASSIGN_MACRO(rultuple->dbr_owner,
			rdf_inv_rb->rdr_owner);
		    rdf_inv_rb->rdr_2types_mask |= RDR2_ALIAS;
		    rdf_inv_rb->rdr_types_mask  |= RDR_RULE;
		
		    status = rdf_call(RDF_INVALIDATE, rdf_inv_cb);
		    if (DB_FAILURE_MACRO(status))
		    {
			error = rdf_inv_cb->rdf_error.err_code;
			break;
		    }

		    /*
		    ** now invalidate the cache entry for the table on
		    ** which the rule was defined
		    */
		    rdf_inv_rb->rdr_2types_mask &= ~RDR2_ALIAS;
		    rdf_inv_rb->rdr_types_mask  &= ~RDR_RULE;
		    status = rdf_call(RDF_INVALIDATE, rdf_inv_cb);
		    if (DB_FAILURE_MACRO(status))
		    {
			error = rdf_inv_cb->rdf_error.err_code;
			break;
		    }

		    /* 
		    ** qeu_drule() took care of destroying IIRULE tuple, rule 
		    ** text and tree; the IIDBDEPENDS tuple describing 
		    ** dependence of a rule on the dbproc will be deleted when 
		    ** we deal with the constraint itself - proceed on to the 
		    ** next temp table row
		    */

		    /* b108474 (b108635) break to clean iidbdepends record*/
		    if (dep_cons_dropped)
			  break;

		    continue;

		}
		case DB_CONS:
		{
		    QEUQ_CB		cqeuq_cb;

		    /* 
		    ** call qeu_dintg() (whose interface needs to be changed to
		    ** enable caller specify that that one or both of 
		    ** integrities and constraints should be dropped (to destroy
		    ** the constraint
		    */
		    STRUCT_ASSIGN_MACRO((*qeuq_cb), cqeuq_cb);

		    /* 
		    ** set up QEUQ_CB to ask qeu_dintg() to destroy a specific 
		    ** integrity
		    */
		    cqeuq_cb.qeuq_rtbl = &currentid;
		    cqeuq_cb.qeuq_ino  = cons_number;

		    /*
		    ** NOTE: if/when we start supporting creation of constraints
		    ** on views, we will have to tell qeu_dintg() to force
		    ** invalidation of QPs which may depend on the view
		    */

		    status = qeu_dintg(qef_cb, &cqeuq_cb, (bool) FALSE, 
			(bool) TRUE);
		    if (DB_FAILURE_MACRO(status))
		    {
			error = cqeuq_cb.error.err_code;
			break;
		    }

		    qeuq_cb->qeuq_keydropped = cqeuq_cb.qeuq_keydropped;
		    qeuq_cb->qeuq_keyattid = cqeuq_cb.qeuq_keyattid;
		    qeuq_cb->qeuq_keypos = cqeuq_cb.qeuq_keypos;
		    break;
		}
	    }
	    if (status != E_DB_OK)
		break;

	    /* 
            ** Find all IIDBDEPENDS tuples describing dependence of DBMS objects
	    ** or non-GRANT compatible permits on the current object and of
	    ** permits on privilege descriptors on the current object and DELETE
	    ** them.  The number of keys used depends on the type of the object
	    ** described by the current temp table row:
	    ** 	- if it is a constraint, we will use all 4 elements of the key 
	    **    (since we do not want to delete IIDBDEPENDS tuples describing
	    **    dependence of DBMS objects on the table itself or on any 
	    **    objects associated with it; 
	    **  - otherwise we will use only the first two elements of the key 
	    **    to ensure that we pick tuples describing dependence of DBMS 
	    **    objects on the current object (e.g. a table) as well as all 
	    **    objects associated with it (e.g. constraints).
            */
	    if (!drop_col)
	    {
	        vqeu.qeu_getnext = QEU_REPO;
	        vqeu.qeu_klen = (cur_obj_type == DB_CONS) ? 4 : 2;       
     
	        vqeu.qeu_tup_length = sizeof(DB_IIDBDEPENDS);

		status = qeu_delete(qef_cb, &vqeu);
		if (status != E_DB_OK)
		{
		    if (vqeu.qeu_access_mode == DMT_A_READ)
		    {
			/* Change over to X mode */
			qeu_close(qef_cb, &vqeu);
			view_opened = FALSE; /* consi01 bug 87642 */
			vqeu.qeu_lk_mode = DMT_IX;
			vqeu.qeu_access_mode = DMT_A_WRITE;
			status = qeu_open(qef_cb, &vqeu);
			if (status != E_DB_OK)
			{
			    error = vqeu.error.err_code;
			    break;
			}
			view_opened = TRUE; /* consi01 bug 87642 */
			status = qeu_delete(qef_cb, &vqeu);
			if (status != E_DB_OK)
			{
			    error = vqeu.error.err_code;
			    break;
			}
		    }
		    else
		    {
			error = vqeu.error.err_code;
			break;
		    }
		}     
	    }

	    /* destroy IITREE tuples only for views */
	    if (cur_obj_type == DB_VIEW)
	    {
	    	tqeu.qeu_tup_length = sizeof(DB_IITREE);
		/* 
		** Now delete IITREE tuples associated with this view
		*/	    

		status = qeu_delete(qef_cb, &tqeu);
		if (status != E_DB_OK)
		{
		    error = tqeu.error.err_code;
		    break;
		}
	    }

	    /* 
	    ** destroy query text for views and dbevents and for dbprocs that 
	    ** need to be destroyted
	    */
	    if (   cur_obj_type == DB_VIEW || cur_obj_type == DB_EVENT
		|| (cur_obj_type == DB_DBP && drop_cur_dbp))
	    {
	    	qqeu.qeu_tup_length = sizeof(DB_IIQRYTEXT);
		/* 
		** Now delete IIQRYTEXT tuples for query id associated with 
		** this view, dbevent, or dbproc.
		*/	    
	    
		status = qeu_delete(qef_cb, &qqeu);
		if (status != E_DB_OK)
		{
		    error = qqeu.error.err_code;
		    break;
		}

		/* if we are deleting a view, we must find the query text
		** entry. This test is for RDF so that it can determine if
		** a bad view id was sent.
		*/
		if 
		(
		    /* a view */
		    (qeuq_cb->qeuq_status_mask & DMT_VIEW)
		    &&
		    /* no tuples found */
		    qqeu.qeu_count == 0
		    /* and deleting the original entry */
		    && 
		    queryid.db_qry_low_time 
					== qeuq_cb->qeuq_qid.db_qry_low_time
		    &&
		    queryid.db_qry_high_time 
					== qeuq_cb->qeuq_qid.db_qry_high_time
		)
		{
		    error = E_QE0023_INVALID_QUERY;
		    status = E_DB_ERROR;
		    break;
		}
	    }

	    /*
	    ** we need to delete IIDBDEPENDS tuple(s) if the object being 
	    ** dropped is a view, dbproc, or if we planted a reminder that 
	    ** there may be such tuples for this object (by setting 
	    ** DELETE_IIDBDEP_TUPLE bit in temp_flags)
	    */
	    if (   cur_obj_type == DB_VIEW 
		|| cur_obj_type == DB_DBP
		|| temprow->temp_flags & DELETE_IIDBDEP_TUPLE)
	    {
		/* 
		** Delete tuples in iidbdepends table which describe the DBMS
		** objects on which the current object depended 
		*/
		status = qeu_dbdelete(qef_cb, qeuq_cb, &currentid, cur_obj_type,
			(i4) 0, &err_blk);
		if (status != E_DB_OK)
		{
		    error = err_blk.err_code;
		    break;
		}

	    }

	    if (cur_obj_type == DB_VIEW || cur_obj_type == DB_DBP)
	    {
		/*
		** and finally delete tuples representing privilege descriptors
		** created for the current object
		*/
		prqeu.qeu_getnext = QEU_REPO;
		prqeu.qeu_klen = 2;       
		prqeu.qeu_flag = 0;
		
		prqeu.qeu_key = prkey_ptr_array;

	        prkey_ptr_array[0] = &prkey_array[0];
		prkey_ptr_array[0]->attr_number = DM_1_PRIV_KEY;
		prkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
		prkey_ptr_array[0]->attr_value =
		    (char *) &currentid.db_tab_base;

		prkey_ptr_array[1] = &prkey_array[1];
		prkey_ptr_array[1]->attr_number = DM_2_PRIV_KEY;
		prkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
		prkey_ptr_array[1]->attr_value =
		    (char *) &currentid.db_tab_index;

        	prqeu.qeu_tup_length = sizeof(DB_IIPRIV);

		status = qeu_delete(qef_cb, &prqeu);
		if (status != E_DB_OK)
		{
		    error = prqeu.error.err_code;
		    break;
		}
	    }
	}

	if (error == E_QE0015_NO_MORE_ROWS)
	{
	    status = E_DB_OK;
	    qeuq_cb->error.err_code = E_QE0000_OK;
	}

	exit_loop = TRUE;
    } /* end for */

    /*
    ** we get here if we are done or if some error was encountered - status
    ** will tell us which
    */
    if (status != E_DB_OK && !err_already_reported)
    {
	/* certain errors requrie special treatment */
	if (error == E_QE0169_UNKNOWN_OBJTYPE_IN_DROP)
	{
	    (VOID) qef_error(error, 0L, status, &error, &qeuq_cb->error, 1,
		sizeof(dtuple->dbv_dtype), &dtuple->dbv_dtype);
	}
	else
	{
	    (VOID) qef_error(error, 0L, status, &error, &qeuq_cb->error, 0);
	}
    }
    
    /* Close off all the tables that have been opened. */

    if (temp_opened)
    {
	local_status = qeu_close(qef_cb, &tempqeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(tempqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (view_opened)
    {
	local_status = qeu_close(qef_cb, &vqeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(vqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (qtext_opened)
    {
	local_status = qeu_close(qef_cb, &qqeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(qqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (tbl_opened)
    {
	local_status = qeu_close(qef_cb, &tqeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(tqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (priv_opened)
    {
	local_status = qeu_close(qef_cb, &prqeu);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(prqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (xpriv_opened)
    {
	local_status = qeu_close(qef_cb, &xprqeu);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(xprqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (dbp_opened)
    {
	local_status = qeu_close(qef_cb, &dbpqeu);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(dbpqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (dbpp_opened)
    {
	local_status = qeu_close(qef_cb, &dbppqeu);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(dbppqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (xdbp_opened)
    {
	local_status = qeu_close(qef_cb, &xdbpqeu);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(xdbpqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (event_opened)
    {
	local_status = qeu_close(qef_cb, &evqeu);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(evqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (seq_opened)
    {
	local_status = qeu_close(qef_cb, &seqqeu);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(seqqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (rul_opened)
    {
	local_status = qeu_close(qef_cb, &rulqeu);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(rulqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (xrul_opened)
    {
	local_status = qeu_close(qef_cb, &xrulqeu);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(xrulqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (integ_opened)
    {
	local_status = qeu_close(qef_cb, &iqeu);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(iqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    /* Destroy the temporary table. */
    if (temp_created)
    {
	dmt_cb.dmt_flags_mask = 0;
	local_status = dmf_call(DMT_DELETE_TEMP, &dmt_cb);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(dmt_cb.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }	    

    if (ulm.ulm_streamid != (PTR)NULL)
    {
	/* 
        ** Close the input stream created for the view
        ** tuples. 
        */

	(VOID) ulm_closestream(&ulm);
    }

    if (transtarted)
    {
	if (status == E_DB_OK)
	{
	    status = qeu_etran(qef_cb, &tranqeu);
	    if (status != E_DB_OK)
	    {
		(VOID) qef_error(tranqeu.error.err_code, 0L, status, &error,
		    &qeuq_cb->error, 0);
	    }
	}

	if (status != E_DB_OK)
	{
	    local_status = qeu_atran(qef_cb, &tranqeu);
	    if (local_status != E_DB_OK)
	    {
		(VOID) qef_error(tranqeu.error.err_code, 0L, local_status, 
			    &error, &qeuq_cb->error, 0);
		if (local_status > status)
		    status = local_status;
	    }
	}
    }

    return (status);
}

/*{
** Name: QEU_DSTAT	- destroy statistics information
**
** External QEF call:   status = qef_call(QEU_DSTAT, &qeuq_cb);
**
** Description:
**      Given a table id, this routine will destroy the statistic and 
**	histogram information. 
**
** Inputs:
**      qef_cb                  qef session control block
**      qeuq_cb
**	    .qeuq_eflag	        designate error handling semantis
**				for user errors.
**		QEF_INTERNAL	return error code.
**		QEF_EXTERNAL	send message to user.
**	    .qeuq_rtbl		table id of base relation
**	    .qeuq_db_id		database id
**	    .qeuq_d_id		dmf session id
**	
** Outputs:
**      qeuq_cb
**	    .error.err_code	one of the following
**				E_QE0002_INTERNAL_ERROR
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0022_ABORTED
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	03-aug-86 (jennifer)
**          written
**      20-aug-86 (jennifer)
**	    Changed all transaction calls from QET_xxxx
**          to QEU_xxx.  
**      03-sep-86 (jennifer)
**          Fixed bug in QEU_DSTAT where ulm end stream
**          called without begin.
**	23-dec-86 (daved) 
**	    close off tables, even in error condition
**	20-jan-88 (puree)
**	    Convert all qet_escalate calls to in-line codes.
**      06-mar-1996 (nanpr01)
**          Change for variable page size project. Previously qeu_delete
**          was using DB_MAXTUP to get memory. With the bigger pagesize
**          it was unnecessarily allocating much more space than was needed.
**          Instead we initialize the correct tuple size, so that we can
**          allocate the actual size of the tuple instead of max possible
**          size.
**	23-jul-1996 (ramra01)
**	    Alter Table drop col delete/update stats
**      14-mar-2003 (hayke02, horda03)
**	    Add extra code to the above change, which was assuming that satno
**	    and hatno would always be ordered. When we find an atno greater
**	    than the atno we want to drop, we now decrement by one as before,
**	    but now increment by DB_MAX_COLS so that it now lies outside the
**	    range of possible atno's - this will avoid the decremetend atno
**	    being the same as an existing, out-of-order atno. Once we have
**	    finished with the delete and replaces, we then scan through the
**	    catalogs again to decrement by DB_MAX_COLS those attributes greater
**	    than the deleted attribute. This change fixes bug 109728.
{@history_line@}...
*/
DB_STATUS
qeu_dstat(
QEF_CB          *qef_cb,
QEUQ_CB	    	*qeuq_cb)
{
    GLOBALREF QEF_S_CB      *Qef_s_cb;
    i4	    error;
    bool	    transtarted = FALSE;	    
    bool	    stat_opened = FALSE;
    bool	    histo_opened = FALSE;
    DB_STATUS	    status, local_status;
    i4		    i;
    QEU_CB	    tranqeu;
    QEU_CB	    sqeu;
    QEU_CB	    hqeu;
    QEF_DATA	    sqef_data;
    QEF_DATA	    hqef_data;
    DB_1ZOPTSTAT    *stuple;
    DB_2ZOPTSTAT    *htuple; 
    ULM_RCB	    ulm;
    DMT_CB	    dmt_cb;
    DMT_CHAR_ENTRY  char_array[2];
    DMR_ATTR_ENTRY  skey_array[3];
    DMR_ATTR_ENTRY  *skey_ptr_array[3];
    DMR_ATTR_ENTRY  hkey_array[3];
    DMR_ATTR_ENTRY  *hkey_ptr_array[3];
    i4		    exit_loop;
    bool	    stats_exist = FALSE;
    
    for (i=0; i< 3; i++)
    {
	skey_ptr_array[i] = &skey_array[i];
	hkey_ptr_array[i] = &hkey_array[i];
    }

    for (exit_loop = FALSE; !exit_loop; )
    {

	/* Check the control block. */

	error = E_QE0017_BAD_CB;
	status = E_DB_ERROR;
	if (qeuq_cb->qeuq_type != QEUQCB_CB ||
            qeuq_cb->qeuq_length != sizeof(QEUQ_CB))	
	    break;

	/* 
        ** Check input parameters for validity.
        */

	error = E_QE0018_BAD_PARAM_IN_CB;
	if (   (qeuq_cb->qeuq_rtbl == 0)
	    || (qeuq_cb->qeuq_db_id == 0)
            || (qeuq_cb->qeuq_d_id == 0))

	    break;
		

	/* 
	** Check to see if a transaction is in progress,
	** if so, set transtarted flag to FALSE, otherwise
	** set it to TRUE after beginning a transaction.
	** If we started a transaction them we will abort
        ** it if an error occurs, otherwise we will just
        ** return the error and let the caller abort it.
	*/

	if (qef_cb->qef_stat == QEF_NOTRAN)
	{
	    tranqeu.qeu_type = QEUCB_CB;
	    tranqeu.qeu_length = sizeof(QEUCB_CB);
	    tranqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
	    tranqeu.qeu_d_id = qeuq_cb->qeuq_d_id;
	    tranqeu.qeu_flag = 0;
	    tranqeu.qeu_mask = 0;
	    status = qeu_btran(qef_cb, &tranqeu);
	    if (status != E_DB_OK)
	    {	
		error = tranqeu.error.err_code;
		break;	
	    }	    
	    transtarted = TRUE;
	}
	/* escalate the transaction to MST */
	if (qef_cb->qef_auto == QEF_OFF)
	    qef_cb->qef_stat = QEF_MSTRAN;

	error = E_QE0000_OK;
    
	/* 
        ** Open the Statistics and Histogram System 
        ** tables.
        */

	sqeu.qeu_type = QEUCB_CB;
        sqeu.qeu_length = sizeof(QEUCB_CB);
        sqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
        sqeu.qeu_lk_mode = DMT_IX;
        sqeu.qeu_flag = DMT_U_DIRECT;
        sqeu.qeu_access_mode = DMT_A_WRITE;
	sqeu.qeu_mask = 0;
         	
	STRUCT_ASSIGN_MACRO(sqeu, hqeu);

        sqeu.qeu_tab_id.db_tab_base  = DM_B_STATISTICS_TAB_ID;
        sqeu.qeu_tab_id.db_tab_index  = DM_I_STATISTICS_TAB_ID;
	status = qeu_open(qef_cb, &sqeu);
	if (status != E_DB_OK)
	{
	    error = sqeu.error.err_code;
	    break;
	}
	stat_opened = TRUE;
        hqeu.qeu_tab_id.db_tab_base  = DM_B_HISTOGRAM_TAB_ID;
        hqeu.qeu_tab_id.db_tab_index  = DM_I_HISTOGRAM_TAB_ID;
	status = qeu_open(qef_cb, &hqeu);
	if (status != E_DB_OK)
	{
	    error = hqeu.error.err_code;
	    break;
	}
	histo_opened = TRUE;

	/* 
        ** Position Statistics table and delete all
        ** tuples matching the specified table id.  
        */	

	sqeu.qeu_getnext = QEU_REPO;
	sqeu.qeu_key = skey_ptr_array;
	skey_ptr_array[0]->attr_number = DM_1_STATISTICS_KEY;
	skey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	skey_ptr_array[0]->attr_value = (char*) 
				&qeuq_cb->qeuq_rtbl->db_tab_base;
	skey_ptr_array[1]->attr_number = DM_2_STATISTICS_KEY;
	skey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	skey_ptr_array[1]->attr_value = (char*) 
                                 &qeuq_cb->qeuq_rtbl->db_tab_index;
        
	sqeu.qeu_qual = 0;
	sqeu.qeu_qarg = 0;
	sqeu.qeu_f_qual = 0;
	sqeu.qeu_f_qarg = 0;
	sqeu.qeu_tup_length = sizeof(DB_1ZOPTSTAT);
	sqeu.qeu_input = &sqef_data;
	sqeu.qeu_output = &sqef_data;
	sqef_data.dt_next = 0;
	sqef_data.dt_size = sizeof(DB_1ZOPTSTAT);
     
	if (qeuq_cb->qeuq_flag_mask & QEU_DROP_COLUMN)
	{
	   STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
	   /* Open stream and allocate memory with one effort */
	   ulm.ulm_flags = ULM_PRIVATE_STREAM | ULM_OPEN_AND_PALLOC;
	   ulm.ulm_psize = sizeof (DB_1ZOPTSTAT);

	   if ((status = qec_mopen(&ulm)) != E_DB_OK)
	   {
	      error = E_QE001E_NO_MEM;
	      break;
	   }

	   stuple = (DB_1ZOPTSTAT*) ulm.ulm_pptr;
	   sqef_data.dt_data = (PTR) stuple;

	   for (;;)
	   {
	       sqeu.qeu_klen = 2;       
	       sqeu.qeu_count = 1;

	       status = qeu_get(qef_cb, &sqeu);
	       if (status != E_DB_OK)
	       {
	          if (sqeu.error.err_code == E_QE0015_NO_MORE_ROWS)
	             status = E_DB_OK;
	          
		  break;
	       }

	       sqeu.qeu_getnext = QEU_NOREPO;

	       if (stuple->db_atno == qeuq_cb->qeuq_ano)
	       {
	          sqeu.qeu_klen = 0;       
		  status = qeu_delete(qef_cb, &sqeu);

		  if (status != E_DB_OK)
		     break;
	       }
	       else if (stuple->db_atno > qeuq_cb->qeuq_ano)
	       {
	          sqeu.qeu_klen = 0;       
		  /* Bug 109728 - decrement by 1 and increment by DB_MAX_COLS
		  ** to avoid duplicates */
		  stuple->db_atno += (DB_MAX_COLS - 1);
		  status = qeu_replace(qef_cb, &sqeu);

		  if (status != E_DB_OK)
		     break;

		  stats_exist = TRUE;
	       }
	       else
	       {
		  stats_exist = TRUE;
	       }

	       continue;
	   }

	   if (status != E_DB_OK)
	      break;

	    /* Bug 109728 - re-scan iistatistics and decrement by DB_MAX_COLS
	    ** those atno's greater than the deleted attribute */
	    if (stats_exist)
	    {
		sqeu.qeu_getnext = QEU_REPO;

		for (;;)
		{
		    sqeu.qeu_klen = 2;       
		    sqeu.qeu_count = 1;

		    status = qeu_get(qef_cb, &sqeu);
		    if (status != E_DB_OK)
		    {
		    if (sqeu.error.err_code == E_QE0015_NO_MORE_ROWS)
			status = E_DB_OK;

		    break;
		    }

		    sqeu.qeu_getnext = QEU_NOREPO;

		    if (stuple->db_atno > DB_MAX_COLS)
		    {
			sqeu.qeu_klen = 0;       
			stuple->db_atno -= DB_MAX_COLS;
			status = qeu_replace(qef_cb, &sqeu);

			if (status != E_DB_OK)
			    break;
		    }
		}
	    }
	}
	else
	{
	   sqeu.qeu_klen = 2;       
	   status = qeu_delete(qef_cb, &sqeu);
	   if (status != E_DB_OK)
	   {
	       error = sqeu.error.err_code;
	       break;
	   }
	}
	/* 
        ** Position Histogram table and delete all
        ** tuples matching the specified table
        ** id.  
        */	
	hqeu.qeu_getnext = QEU_REPO;
	hqeu.qeu_klen = 2;       
	hqeu.qeu_key = hkey_ptr_array;
	hkey_ptr_array[0]->attr_number = DM_1_HISTOGRAM_KEY;
	hkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	hkey_ptr_array[0]->attr_value = (char*) 
				&qeuq_cb->qeuq_rtbl->db_tab_base;
	hkey_ptr_array[1]->attr_number = DM_2_HISTOGRAM_KEY;
	hkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	hkey_ptr_array[1]->attr_value = (char*) 
                                 &qeuq_cb->qeuq_rtbl->db_tab_index;
        
	hqeu.qeu_qual = 0;
	hqeu.qeu_qarg = 0;
	hqeu.qeu_f_qual = 0;
	hqeu.qeu_f_qarg = 0;
	hqeu.qeu_tup_length = sizeof(DB_2ZOPTSTAT);
	hqeu.qeu_input = &hqef_data;
	hqeu.qeu_output = &hqef_data;
	hqef_data.dt_next = 0;
	hqef_data.dt_size = sizeof(DB_2ZOPTSTAT);
     
	if (qeuq_cb->qeuq_flag_mask & QEU_DROP_COLUMN)
	{
	   ulm.ulm_psize = sizeof (DB_2ZOPTSTAT);
	   if ((status = qec_malloc(&ulm)) != E_DB_OK)
	   {
	      ulm_closestream(&ulm);
	      error = E_QE001E_NO_MEM;
	      break;
	   }
	   htuple = (DB_2ZOPTSTAT*) ulm.ulm_pptr;
	   hqef_data.dt_data = (PTR) htuple;

	   for (;;)
	   {
	       hqeu.qeu_klen = 2;       
	       hqeu.qeu_count = 1;

	       status = qeu_get(qef_cb, &hqeu);
	       if (status != E_DB_OK)
	       {
	          if (hqeu.error.err_code == E_QE0015_NO_MORE_ROWS)
	             status = E_DB_OK;
	          
		  break;
	       }

	       hqeu.qeu_getnext = QEU_NOREPO;

	       if (htuple->db_atno == qeuq_cb->qeuq_ano)
	       {
	          hqeu.qeu_klen = 0;       
		  status = qeu_delete(qef_cb, &hqeu);

		  if (status != E_DB_OK)
		     break;
	       }
	       else if (htuple->db_atno > qeuq_cb->qeuq_ano)
	       {
	          hqeu.qeu_klen = 0;       
		  /* Bug 109728 - decrement by 1 and increment by DB_MAX_COLS
		  ** to avoid duplicates */
		  htuple->db_atno += (DB_MAX_COLS - 1);
		  status = qeu_replace(qef_cb, &hqeu);

		  if (status != E_DB_OK)
		     break;

		  stats_exist = TRUE;
	       }
	       else
	       {
		  stats_exist = TRUE;
	       }

	       continue;
	   }

	   if (status != E_DB_OK)
	      break;

	    /* Bug 109728 - re-scan iihistogram and decrement by DB_MAX_COLS
	    ** those atno's greater than the deleted attribute */
	    if (stats_exist)
	    {
		hqeu.qeu_getnext = QEU_REPO;

		for (;;)
		{
		    hqeu.qeu_klen = 2;       
		    hqeu.qeu_count = 1;

		    status = qeu_get(qef_cb, &hqeu);
		    if (status != E_DB_OK)
		    {
		    if (hqeu.error.err_code == E_QE0015_NO_MORE_ROWS)
			status = E_DB_OK;

		    break;
		    }

		    hqeu.qeu_getnext = QEU_NOREPO;

		    if (htuple->db_atno > DB_MAX_COLS)
		    {
			hqeu.qeu_klen = 0;       
			htuple->db_atno -= DB_MAX_COLS;
			status = qeu_replace(qef_cb, &hqeu);

			if (status != E_DB_OK)
			    break;
		    }
		}
	    }
	}
	else
	{
	   hqeu.qeu_klen = 2;       
	   status = qeu_delete(qef_cb, &hqeu);

	   if (status != E_DB_OK)
	   {
	      error = hqeu.error.err_code;
	      break;
	   }
	}
		
	/* 
	** Now alter the table tuple in the relation table
	** to indicate statistics do not exist.
	*/

	if (stats_exist == FALSE)
	{
	   dmt_cb.dmt_flags_mask = 0;
	   dmt_cb.dmt_db_id = qeuq_cb->qeuq_db_id;
	   dmt_cb.dmt_char_array.data_in_size = sizeof(DMT_CHAR_ENTRY) * 1;
	   dmt_cb.dmt_char_array.data_address = (PTR) char_array;
	   char_array[0].char_id = DMT_C_ZOPTSTATS;
	   char_array[0].char_value = DMT_C_OFF;

	   dmt_cb.length = sizeof(DMT_CB);
	   dmt_cb.type = DMT_TABLE_CB;
	   dmt_cb.dmt_id.db_tab_base = qeuq_cb->qeuq_rtbl->db_tab_base;
	   dmt_cb.dmt_id.db_tab_index = qeuq_cb->qeuq_rtbl->db_tab_index;
           dmt_cb.dmt_tran_id = qef_cb->qef_dmt_id;
   
	   status = dmf_call(DMT_ALTER, &dmt_cb);

	   if (status != E_DB_OK)
	   {
	       error = dmt_cb.error.err_code;
	       break;
	   }
	}

	/* Close off all the tables. */

	status = qeu_close(qef_cb, &sqeu);    
	if (status != E_DB_OK)
	{
	    error = sqeu.error.err_code;
	    break;
	}
	stat_opened = FALSE;
	status = qeu_close(qef_cb, &hqeu);    
	if (status != E_DB_OK)
	{
	    error = hqeu.error.err_code;
	    break;
	}
	histo_opened = FALSE;

	
	if (transtarted)
	{
	    status = qeu_etran(qef_cb, &tranqeu);
	    if (status != E_DB_OK)
	    {	
		error = tranqeu.error.err_code;
		break;
	    }
	}

	exit_loop = TRUE;
    } /* end for */

    if (qeuq_cb->qeuq_flag_mask & QEU_DROP_COLUMN)
       ulm_closestream(&ulm);

    if (status == E_DB_OK)
    {
	/* SUCCESS - we are done */
        qeuq_cb->error.err_code = E_QE0000_OK;
        return (E_DB_OK);
    }

    /* call qef_error to handle error messages */

    (VOID) qef_error(error, 0L, status, &error, &qeuq_cb->error, 0);
    
    /* Close off all the tables. */

    local_status = E_DB_OK;
    if (stat_opened)
    {
	local_status = qeu_close(qef_cb, &sqeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(sqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (histo_opened)
    {
	local_status = qeu_close(qef_cb, &hqeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(hqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (transtarted)
    {
	local_status = qeu_atran(qef_cb, &tranqeu);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(tranqeu.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    return (status);
}

/*{
** Name: qeu_qid	- Obtain query text id for a view
**
** Description:
{@comment_line@}...
**
** Inputs:
**	qeuq_cb				ptr to request block
**      tab_id                          ptr to a DB_TAB_ID of the view.
**	qtext_id			ptr to an empty area of type
**					DB_QRY_ID to revceive the query id
**					of the view.
**	err_blk				ptr to DB_ERROR area.
**
** Outputs:
**      qtext_id                        query text id of the view.
**	Returns:
**	    DB_STATUS
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      06-nov-87 (puree)
**          change error return from QE001D to QE0097_NOT_A_VIEW.
**	17-oct-91 (rogerk)
**	    Part of 6.4 -> 6.5 merge changes. Must initialize new dmt_char_array
**	    field for DMT_SHOW calls.
[@history_template@]...
*/
DB_STATUS
qeu_qid(
QEUQ_CB		*qeuq_cb,
DB_TAB_ID	*tab_id,
DB_QRY_ID	*qtext_id,
DB_ERROR	*err_blk)
{
    DB_STATUS		status;
    DMT_SHW_CB		dmt_shw_cb;
    DMT_TBL_ENTRY	table;


    /* Set up dmt_show call input parameters. */

    dmt_shw_cb.length = sizeof(DMT_SHW_CB);
    dmt_shw_cb.type = DMT_SH_CB;
    dmt_shw_cb.dmt_db_id = qeuq_cb->qeuq_db_id;
    dmt_shw_cb.dmt_session_id = qeuq_cb->qeuq_d_id;
    dmt_shw_cb.dmt_flags_mask = DMT_M_TABLE;
    dmt_shw_cb.dmt_table.data_address = (char *)&table;
    dmt_shw_cb.dmt_table.data_in_size = sizeof(DMT_TBL_ENTRY);
    dmt_shw_cb.dmt_char_array.data_address = (PTR)NULL;
    dmt_shw_cb.dmt_char_array.data_in_size = 0;
    dmt_shw_cb.dmt_char_array.data_out_size  = 0;
    STRUCT_ASSIGN_MACRO(*tab_id, dmt_shw_cb.dmt_tab_id);

    /* issue DMT_SHOW call */

    if ((status = dmf_call(DMT_SHOW, &dmt_shw_cb)) != E_DB_OK)
    {
	STRUCT_ASSIGN_MACRO(dmt_shw_cb.error, *err_blk);
	return(status);
    }

    /* if the table is a view, return query text id to the caller */

    if (table.tbl_status_mask & DMT_VIEW)
    {
	STRUCT_ASSIGN_MACRO(table.tbl_qryid, *qtext_id);
	return(E_DB_OK);
    }
    else
    {
	err_blk->err_code = E_QE0097_NOT_A_VIEW;
	return(E_DB_ERROR);
    }
}

/*{
** Name: qeu_dbdelete	- delete tuples in iidbdepends based on the
**			  id of the dependent object.
**
** Description:
**    Delete IIDBDEPENDS tuples describing dependence of specified
**    object on other DBMS objects.  A view depends on its owner's
**    tables/views and other users' QUEL views used in the view's definition.
**    A permit on a view may depend on privileges contained in a privilege
**    descriptor.  A [REFERENCES] constraint depends on a UNIQUE or PRIMARY KEY
**    constraint and may depend on privilege(s) contained in a privilege
**    descriptor.
**
**    Permit involving a WHERE-clause can depend on other tables.  
**    Permit granted by a user on his/her view which is not "always grantable" 
**    may depend on privileges on the view's underlying table or view.  
**    Tuples describing the former dependency will be destroyed
**    without examining them.  A (there should be at most one) tuple 
**    describing the latter dependency will be examined to determine
**    the privilege descriptor number.  Having done that, we will examine
**    IIDBDEPENDS to see if there are any other permits dependent on this
**    privilege descriptor.  If none are found, the IIPRIV tuple
**    representing the privilege descriptor will be deleted; otherwise it
**    will be left intact.
**
**    If a constraint depended on a privilege descriptor, that privilege
**    descriptor must be dropped since no two constraints will depend on the
**    same descriptor.  We do not try to reuse descriptors on which constraints
**    depend because
**	- it is more complicated to establish whether a given constraint depends
**	  on the same privilege (i.e. names the same set of <referenced
**	  columns>) as some existing constraint
**	- I am betting that not too many constraints defined on a table will
**	  refer the same set of columns of any given table
**    
**    Special handling will be provided for the case when we are
**    dropping all permits or constraints on a table or a view: all IIDBDEPENDS
**    tuples expressing dependency of some permit/constraint on the specified
**    object on another object will be destroyed as will be all IIPRIV tuples
**    representing privilege descriptors (if any) created to describe
**    privileges on which some permit/constraint on the specified object depends
**    
**
** Inputs:
**	qef_cb			session control block.
**	qeuq_cb			ptr to request block
**      tab_id                  ptr to DB_TAB_ID of the dependent object
**	type			type of dependent object.
**	qid			secondary id of the dependent object.
**				if type==DB_PROT and qid==0, we are
**				dropping all permits on a table/view
**				and special code will handle this case
**	err_blk			ptr to DB_ERROR area.
**
** Outputs:
**      none
**	Returns:
**	    DB_STATUS
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      19-sep-88 (puree)
**          Change lock mode on iixdbdepends from IS to IX and access mode
**	    from DMT_A_READ to DMT_A_WRITE.  DMF has trouble when the user
**	    sets readlock = nolock.
**	18-jun-92 (andre)
**	    use qeu_delete() to delete IIDBDEPENDS tuples by TID
**	11-jul-92 (andre)
**	    when deleting tuples describing on which objects a given
**	    permit depends, we will distinguish between tuples representing
**	    dependency of a permit on table(s) used in its WHERE-clause and a
**	    tuple representing dependency of a permit on a privilege descriptor
**	    (see above)
**	12-jul-92 (andre)
**	    Added special handling for the case when all permits on a
**	    table/view are being destroyed (see above)
**	21-mar-93 (andre)
**	    now we can be called to delete IIDBDEPENDS tuples describing objects
** 	    on which one or more newly dropped REFERENCES constraints used to 
**	    depend
**	27-mar-93 (andre)
**	    no two constraints will share the same privilege descriptor;
**	    consequently, we do not need to examine IIDBDEPENDS tuples looking
**	    for a privilege descriptor number if called in the course of
**	    destroying a specific constraint
**	09-jun-93 (andre)
**	    we were trying to key into IIDBDEPENDS using an i2 for the value of
**	    the fourth key (i_qid) even though it is an i4.  As usual, this 
** 	    seemed to work well on VMS but is liable to bomb on UNIX.
[@history_template@]...
*/
DB_STATUS
qeu_dbdelete(
QEF_CB		*qef_cb,
QEUQ_CB		*qeuq_cb,
DB_TAB_ID	*tab_id,
i4		type,
i4		qid,
DB_ERROR	*err_blk)
{
    DB_STATUS	    status, local_status;
    i4	    error;
    i4		    i;
    QEU_CB	    base_cb, idx_cb;
    QEU_CB	    priv_cb;
    bool	    base_opened = FALSE;
    bool	    idx_opened = FALSE;
    bool	    priv_opened = FALSE;
    DB_IIXDBDEPENDS idx_tuple;
    QEF_DATA	    qef_data;
    DMR_ATTR_ENTRY  key_array[4];
    DMR_ATTR_ENTRY  *key_ptr_array[4];
    i4		    exit_loop;

    for (exit_loop = FALSE; !exit_loop; )
    {
        /* Open the iixdbdepends table */
        idx_cb.qeu_tab_id.db_tab_base  = DM_B_XDEPENDS_TAB_ID;
        idx_cb.qeu_tab_id.db_tab_index  = DM_I_XDEPENDS_TAB_ID;
        idx_cb.qeu_db_id = qeuq_cb->qeuq_db_id;
        idx_cb.qeu_eflag = QEF_INTERNAL;
        idx_cb.qeu_flag = DMT_U_DIRECT;

        idx_cb.qeu_lk_mode = DMT_IX;
        idx_cb.qeu_access_mode = DMT_A_WRITE;
        idx_cb.qeu_mask = 0;
    	
        if ((status = qeu_open(qef_cb, &idx_cb)) != E_DB_OK)
        {
	    STRUCT_ASSIGN_MACRO(idx_cb.error, *err_blk);
	    break;
        }

	idx_opened = TRUE;

        /* Open the iidbdepends table */
        base_cb.qeu_tab_id.db_tab_base  = DM_B_DEPENDS_TAB_ID;
        base_cb.qeu_tab_id.db_tab_index  = DM_I_DEPENDS_TAB_ID;
        base_cb.qeu_db_id = qeuq_cb->qeuq_db_id;
        base_cb.qeu_eflag = QEF_INTERNAL;
    
        base_cb.qeu_flag = DMT_U_DIRECT;
        base_cb.qeu_lk_mode = DMT_IX;
        base_cb.qeu_access_mode = DMT_A_WRITE;
        base_cb.qeu_mask = 0;
	
        if ((status = qeu_open(qef_cb, &base_cb)) != E_DB_OK)
        {
	    STRUCT_ASSIGN_MACRO(base_cb.error, *err_blk);
	    break;
        }

	base_opened = TRUE;

        /* 
        ** Scan IIXDBDEPENDS looking for tuples corresponding to the
	** specified object and get a tid.  
	**
	** If deleting tuples describing objects on which a specific permit
	** (as opposed to all permits) on a table/view or a constraint (as
	** opposed to all constraints) on a base table depended, we will examine
	** each IIDBDEPENDS tuple before deleting it, and if it describes
	** dependence of a permit or constraint on a privilege descriptor, we 
	** will record the descriptor number before deleting the IIDBDEPENDS 
	** tuple.  If called in the course of destroying a specific permit, then
	** after we run out of IIDBDEPENDS tuples to delete, we will determine
	** whether there are any other permits dependent on the same privilege
	** descriptor and if none is found, delete the corresponding IIPRIV
	** tuple.  We do not need to be as careful with IIDBDEPENDS tuples
	** describing dependence of a constraint on a privilege descriptor
	** because we know that no two constraints will share the same privilege
	** descriptor
	**
	** If destroying all permits or constraints on a table/view, we will
	** delete all IIDBDEPENDS tuples representing dependence of some permit
	** or constraints defined on the specified object + all IIPRIV
	** tuples representing privileges on which some permit or constraint 
	** on this object depended
	** 
	** For objects other than permits and constraints (i.e. views or 
	** dbprocs), we will simply delete corresponding tuples in iidbdepends. 
        **
        ** set up QEU_CB for reading tuple from iixdbdepends.
        */
        idx_cb.qeu_getnext = QEU_REPO;
        idx_cb.qeu_output = &qef_data;
        qef_data.dt_next = 0;
        qef_data.dt_size = sizeof(DB_IIXDBDEPENDS);
        qef_data.dt_data = (PTR) &idx_tuple; 
        idx_cb.qeu_count = 1;
        idx_cb.qeu_tup_length = sizeof(DB_IIXDBDEPENDS);
        idx_cb.qeu_klen = 4;       
        idx_cb.qeu_key = key_ptr_array;
    
        for (i=0; i< 4; i++)
	    key_ptr_array[i] = &key_array[i];
    
        key_ptr_array[0]->attr_number = DM_1_XDEPENDS_KEY;
        key_ptr_array[0]->attr_operator = DMR_OP_EQ;
        key_ptr_array[0]->attr_value = (char *) &(tab_id->db_tab_base);

        key_ptr_array[1]->attr_number = DM_2_XDEPENDS_KEY;
        key_ptr_array[1]->attr_operator = DMR_OP_EQ;
        key_ptr_array[1]->attr_value = (char *) &(tab_id->db_tab_index);

        key_ptr_array[2]->attr_number = DM_3_XDEPENDS_KEY;
        key_ptr_array[2]->attr_operator = DMR_OP_EQ;
        key_ptr_array[2]->attr_value = (char *) &type;

        key_ptr_array[3]->attr_number = DM_4_XDEPENDS_KEY;
        key_ptr_array[3]->attr_operator = DMR_OP_EQ;
        key_ptr_array[3]->attr_value = (char *) &qid;

        idx_cb.qeu_qual = 0;
        idx_cb.qeu_qarg = 0;
    
        /* set up base_cb for reading/deleting iidbdepends tuple by tid */
        base_cb.qeu_klen = 0;
        base_cb.qeu_flag = QEU_BY_TID;
	base_cb.qeu_qual = 0;
	base_cb.qeu_qarg = 0;
     
	if ((type == DB_PROT || type == DB_CONS) && qid != 0)
	{
	    /* dropping a specific permit or constraints on a table or a view */

	    DB_IIDBDEPENDS 	base_tuple;
	    QEF_DATA	        base_qef_data;

		/* 
		** if a permit/constraint depended on a privilege descriptor,
		** descriptor's number will be greater than 0
		*/
	    i2			priv_descr_no = 0;

	    /* we will be reading IIDBDEPENDS tuples by TID */
	    base_cb.qeu_getnext = QEU_NOREPO;
            base_cb.qeu_output = &base_qef_data;
            base_qef_data.dt_next = 0;
            base_qef_data.dt_size = sizeof(DB_IIDBDEPENDS);
            base_qef_data.dt_data = (PTR) &base_tuple; 
            base_cb.qeu_count = 1;
            base_cb.qeu_tup_length = sizeof(DB_IIDBDEPENDS);

	    for (;;)
	    {
	        /* get a tuple from the index table, iixdbdepends */

	        if ((status = qeu_get(qef_cb, &idx_cb)) != E_DB_OK)
	        {
	            if (idx_cb.error.err_code == E_QE0015_NO_MORE_ROWS)
		        status = E_DB_OK;
		    else
	                STRUCT_ASSIGN_MACRO(idx_cb.error, *err_blk);

	            break;
	        }

	        idx_cb.qeu_getnext = QEU_NOREPO;    /* no repo after 1st get */
    
		/* get the tid from the index tuple */
	        base_cb.qeu_tid = idx_tuple.dbvx_tidp;

		/* 
		** if we haven't found an IIDBDEPENDS tuple describing 
		** dependence of this permit or constraints on a privilege 
		** descriptor, examine this tuple before deleting it;
		** otherwise just delete it
		*/
		if (!priv_descr_no)
		{
	            if ((status = qeu_get(qef_cb, &base_cb)) != E_DB_OK)
	            {
	                STRUCT_ASSIGN_MACRO(base_cb.error, *err_blk);
	                break;
	            }

		    if (base_tuple.dbv_itype == DB_PRIV_DESCR)
		        priv_descr_no = base_tuple.dbv_i_qid;
		}

		/* now delete this tuple by TID */
	        if ((status = qeu_delete(qef_cb, &base_cb)) != E_DB_OK)
	        {
	            STRUCT_ASSIGN_MACRO(base_cb.error, *err_blk);
	            break;
	        }
	    }

	    if (status != E_DB_OK)
		break;

	    /* 
	    ** if the permit depended on some privilege descriptor, check
	    ** whether there are any other permits dependent on that privilege
	    ** descriptor; if none are found, delete the the IIPRIV tuple
	    ** representing this privilege descriptor
	    ** 
	    ** if a constraint depended on a privilege descriptor, simply
	    ** destroy it as no two constraints will depend on the same
	    ** privilege descriptor
	    */
	    if (priv_descr_no)
	    {
		/*
		** if a permit depended on this privilege descriptor, we can
		** delete it (the descriptor) only if there are no other permits
		** dependent on it; if a constraint depended on this
		** descriptor, we can safely destroy it since no two constraints
		** will depend on the same privilege descriptor
		*/
		if (type == DB_PROT)
		{
		    i4		indep_obj_type = DB_PRIV_DESCR;

		        /* 
		        ** NOTE: we may not use priv_descr_no to key into 
		        **       IIDBDEPENDS because it is an i2 and the 4-th 
			**	 key of IIDBDEPENDS is an i4; priv_descr_no is 
			**	 quite well suited for keying into IIPRIV 
			**	 though, where the third key IS an i2
		        */
		    i4		indep_obj_no = priv_descr_no;
		    
		    /* 
		    ** prepare to read a tuple from IIDBDEPENDS; we will
		    ** reuse key_ptr_array which was set up for accessing
		    ** IIXDBDEPENDS and needs only minor modifications
		    */

		    base_cb.qeu_flag = 0;
		    base_cb.qeu_getnext = QEU_REPO;
		    base_cb.qeu_klen = 4;       
		    base_cb.qeu_key = key_ptr_array;
		
		    key_ptr_array[0]->attr_number = DM_1_DEPENDS_KEY;

		    key_ptr_array[1]->attr_number = DM_2_DEPENDS_KEY;

		    key_ptr_array[2]->attr_number = DM_3_DEPENDS_KEY;
		    key_ptr_array[2]->attr_value = (char *) &indep_obj_type;

		    key_ptr_array[3]->attr_number = DM_4_DEPENDS_KEY;
		    key_ptr_array[3]->attr_value = (char *) &indep_obj_no;

		    if ((status = qeu_get(qef_cb, &base_cb)) != E_DB_OK)
		    {
			if (base_cb.error.err_code == E_QE0015_NO_MORE_ROWS)
			    status = E_DB_OK;
			else
			{
			    STRUCT_ASSIGN_MACRO(base_cb.error, *err_blk);
			    break;
			}
		    }
		    else
		    {
			/* 
			** there is at least one other permit or constraint 
			** dependent on this privilege descriptor - IIPRIV tuple
			** correspondint to the descriptor cannot be removed
			*/
			break;
		    }
		}

		/* 
		** we only get here if there are no more permits 
		** dependent on the privilege descriptor on which the
		** permit being destroyed depended OR
		** if the privilege descriptor descriptor described privileges
		** on which a constraint depended
		**
		** Open IIPRIV and delete the tuple corresponding to
		** that privilege descriptor
		*/

		priv_cb.qeu_tab_id.db_tab_base  = DM_B_PRIV_TAB_ID;
		priv_cb.qeu_tab_id.db_tab_index = DM_I_PRIV_TAB_ID;
		priv_cb.qeu_db_id = qeuq_cb->qeuq_db_id;
		priv_cb.qeu_eflag = QEF_INTERNAL;
		priv_cb.qeu_flag = DMT_U_DIRECT;

		priv_cb.qeu_lk_mode = DMT_IX;
		priv_cb.qeu_access_mode = DMT_A_WRITE;
		priv_cb.qeu_mask = 0;
		
		if ((status = qeu_open(qef_cb, &priv_cb)) != E_DB_OK)
		{
		    STRUCT_ASSIGN_MACRO(priv_cb.error, *err_blk);
		    break;
		}

		priv_opened = TRUE;

	        priv_cb.qeu_tup_length = sizeof(DB_IIPRIV);
		priv_cb.qeu_qual = priv_cb.qeu_f_qual = 0;
		priv_cb.qeu_qarg = priv_cb.qeu_f_qarg = 0;
		priv_cb.qeu_flag = 0;
		priv_cb.qeu_klen = 3;
		priv_cb.qeu_key = key_ptr_array;

		/* we will reuse key_ptr_array again */
		key_ptr_array[0]->attr_number = DM_1_PRIV_KEY;

		key_ptr_array[1]->attr_number = DM_2_PRIV_KEY;

		key_ptr_array[2]->attr_number = DM_3_PRIV_KEY;
		key_ptr_array[2]->attr_value = (char *) &priv_descr_no;

		if ((status = qeu_delete(qef_cb, &priv_cb)) != E_DB_OK)
		{
		    STRUCT_ASSIGN_MACRO(priv_cb.error, *err_blk);
		    break;
		}
	    }
	}
	else
	{
	    /* 
	    ** if dropping all permits or constraints on a table or a view, 
	    ** we want to delete all IIDBDEPENDS tuples expressing dependency of
	    ** some permit/constraint on the specified object on other objects,
	    ** therefore we will key into IIXDBDEPENDS on the first 3
	    ** keys (tab_base, tab_idx, and dtype)
	    */
	    if (type == DB_PROT || type == DB_CONS)
	    {
		idx_cb.qeu_klen = 3;
	    }

	    for (;;)
	    {
	        /* get a tuple from the index table, iixdbdepends */

	        if ((status = qeu_get(qef_cb, &idx_cb)) != E_DB_OK)
	        {
	            if (idx_cb.error.err_code == E_QE0015_NO_MORE_ROWS)
		        status = E_DB_OK;
		    else
	                STRUCT_ASSIGN_MACRO(idx_cb.error, *err_blk);

	            break;
	        }

	        idx_cb.qeu_getnext = QEU_NOREPO;    /* no repo after 1st get */
    
	        /*
	        ** get the tid from the index tuple and delete the 
		** corresponding tuple from iidbdepends table.
	        */
	        base_cb.qeu_tid = idx_tuple.dbvx_tidp;
	        if ((status = qeu_delete(qef_cb, &base_cb)) != E_DB_OK)
	        {
	            STRUCT_ASSIGN_MACRO(base_cb.error, *err_blk);
	            break;
	        }
	    }

	    if (status != E_DB_OK)
		break;

	    if (type == DB_PROT || type == DB_CONS)
	    {
		i2		priv_descr_no = 1;
		i2		dep_type = type;

		/* 
		** now we must delete all IIPRIV tuples representing privilege 
		** descriptors created to express dependency of permits or 
		** constraints on specified object on some privileges on that
		** object's underlying object (for permits) or on base table
		** which is the <referenced table> of a REFERENCES constraint
		*/

		priv_cb.qeu_tab_id.db_tab_base  = DM_B_PRIV_TAB_ID;
		priv_cb.qeu_tab_id.db_tab_index = DM_I_PRIV_TAB_ID;
		priv_cb.qeu_db_id = qeuq_cb->qeuq_db_id;
		priv_cb.qeu_eflag = QEF_INTERNAL;
		priv_cb.qeu_flag = DMT_U_DIRECT;

		priv_cb.qeu_lk_mode = DMT_IX;
		priv_cb.qeu_access_mode = DMT_A_WRITE;
		priv_cb.qeu_mask = 0;
		
		if ((status = qeu_open(qef_cb, &priv_cb)) != E_DB_OK)
		{
		    STRUCT_ASSIGN_MACRO(priv_cb.error, *err_blk);
		    break;
		}

		priv_opened = TRUE;

		/*
		** we need to provide DMF with a qualification function to
		** ensure that only tuples representing privilege descriptors
		** on which objects of appropriate type depend are dropped
		*/
		priv_cb.qeu_qual = ( PTR ) qeu_iipriv_by_type;
		priv_cb.qeu_qarg = (PTR) &dep_type;

	        priv_cb.qeu_tup_length = sizeof(DB_IIPRIV);
		priv_cb.qeu_f_qual = 0;
		priv_cb.qeu_f_qarg = 0;
		priv_cb.qeu_flag = 0;
		priv_cb.qeu_klen = 3;
		priv_cb.qeu_key = key_ptr_array;

		/* 
		** we will reuse key_ptr_array again except that for
		** the last element of the key we will replace == with >=
		** and will delete privilege descriptors with numbers >= 1
		** (recall that privilege descriptor 0 may be created
		** to express privileges on which a view and all permits
		** granting SELECT privilege on it depend.)
		*/
		key_ptr_array[0]->attr_number = DM_1_PRIV_KEY;

		key_ptr_array[1]->attr_number = DM_2_PRIV_KEY;

		key_ptr_array[2]->attr_number = DM_3_PRIV_KEY;
		key_ptr_array[2]->attr_operator = DMR_OP_GTE;
		key_ptr_array[2]->attr_value = (char *) &priv_descr_no;

		if ((status = qeu_delete(qef_cb, &priv_cb)) != E_DB_OK)
		{
		    STRUCT_ASSIGN_MACRO(priv_cb.error, *err_blk);
		    break;
		}
	    }
	}

	exit_loop = TRUE;
    }

    /* No more tuples, or error, close tables */

    if (idx_opened && (local_status = qeu_close(qef_cb, &idx_cb)) != E_DB_OK)
    {
	/* report original error before we lose it */
	if (status != E_DB_OK)
	{
	    (VOID) qef_error(err_blk->err_code, 0L, status, &error, err_blk, 0);
	}
	STRUCT_ASSIGN_MACRO(idx_cb.error, *err_blk);

	if (local_status > status)
	    status = local_status;
    }

    if (base_opened && (local_status = qeu_close(qef_cb, &base_cb)) != E_DB_OK)
    {
	/* report original error before we lose it */
	if (status != E_DB_OK)
	{
	    (VOID) qef_error(err_blk->err_code, 0L, status, &error, err_blk, 0);
	}
	STRUCT_ASSIGN_MACRO(base_cb.error, *err_blk);

	if (local_status > status)
	    status = local_status;
    }

    if (priv_opened && (local_status = qeu_close(qef_cb, &priv_cb)) != E_DB_OK)
    {
	/* report original error before we lose it */
	if (status != E_DB_OK)
	{
	    (VOID) qef_error(err_blk->err_code, 0L, status, &error, err_blk, 0);
	}
	STRUCT_ASSIGN_MACRO(priv_cb.error, *err_blk);

	if (local_status > status)
	    status = local_status;
    }

    return(status);
}

/*{
** Name: qeu_access_perm    - create an access permit on a table.
**
** Description:
**		This function will be used for creation of special "access"
**		permits on tables.  Presently, we intend to use it only for QUEL
**		view owned by the DBA.  This is done to give FEs help-type
**		commands) backward compatibility.  
** Inputs:
**	qef_cb			session control block.
**	qeu_cb			ptr to qeu CB
**	dmu_cb			ptr to DMU control block
**	flag		bit masks used to specify type of permit being granted
**		QEU_TO_PUBLIC - granting to public
**				granting to the owner otherwise
**
** Outputs:
**      qeu_cb
**	    error
**		err_code	error code, if some error was encountered
**
**	Returns:
**	    DB_STATUS
**	Exceptions:
**	    none
**
** Side Effects:
**	    If successful, will cause an "access" permit to be inserted into
**	    IIPROTECT and will set a bit in relstat to indicat that a permit
**	    exists.
**
** History:
**      5-mar-90 (andre)
**	    written.
**	08-aug-90 (ralph)
**	    Initialize new iiprotect fields in qeu_access_perm
**	07-jul-92 (andre)
**	    qeu_cprot() expects qeuq_cb->qeuq_indep to point at an independent
**	    object/privilege structure.  In the privilege(s) being created do
**	    not depend on any other privileges, independent privilege lists are
**	    expected to be set to NULL.
**	21-mar-94 (andre)
**	    set both dbp_popset and dbp_popctl to DB_CAN_ACCESS
*/
DB_STATUS
qeu_access_perm(
QEF_CB          *qef_cb,
QEU_CB		*qeu_cb,
DMU_CB		*dmu_cb,
i4		flag)
{
    QEUQ_CB		    qeuq_cb;
    QEF_DATA		    qef_qrydata;    /* will contain IIPROTECT tuple */
    DB_PROTECTION	    ptuple;
    register DB_PROTECTION  *protup = &ptuple;
    register i4		    *domset = ptuple.dbp_domset;
    register i4	    i;
    DB_STATUS		    status;
    DB_TIME_ID              timeid;
    PSQ_INDEP_OBJECTS	    indep_privs;

    qeuq_cb.qeuq_flag_mask = 0;
        
    /* no IIQRYTEXT or IITREE tuples will be inserted */
    qeuq_cb.qeuq_qry_tup = qeuq_cb.qeuq_tre_tup = (QEF_DATA *) NULL;
    qeuq_cb.qeuq_cq      = qeuq_cb.qeuq_ct      = 0;

    /* creating accessibility permit (not a ALL/RETR_TO_ALL permit) */
    qeuq_cb.qeuq_permit_mask = QEU_PERM_OR_SECALM;

    /* initialize IIPROTECT tuple */
    (VOID)MEfill(sizeof(ptuple), (u_char) 0, (PTR) protup);

    /* init time stamp */
    TMnow((SYSTIME *)&timeid);
    protup->dbp_timestamp.db_tim_high_time = timeid.db_tim_high_time;
    protup->dbp_timestamp.db_tim_low_time  = timeid.db_tim_low_time;

    /* init object/owner name */
    STRUCT_ASSIGN_MACRO(dmu_cb->dmu_owner, protup->dbp_obown);
    STRUCT_ASSIGN_MACRO(dmu_cb->dmu_table_name, protup->dbp_obname);

    /* init object type/status flags */
    protup->dbp_obtype = DBOB_VIEW;	    /*@FIX_ME@ - where is this info?? */
    protup->dbp_obstat = ' ';		    /*@FIX_ME@ - where is this info?? */

    /* init grantor name */
    STRUCT_ASSIGN_MACRO(qef_cb->qef_user, protup->dbp_grantor);
    
    /* init grantee type and name */
    if (flag & QEU_TO_PUBLIC)
    {
	protup->dbp_gtype = DBGR_PUBLIC;
	MEfill(sizeof(DB_OWN_NAME), ' ', (char *)&protup->dbp_owner);
    }
    else
    {
	protup->dbp_gtype = DBGR_USER;
	
	/* name owner as the grantee */
	STRUCT_ASSIGN_MACRO(dmu_cb->dmu_owner, protup->dbp_owner);
    }

    (VOID)MEfill(sizeof(protup->dbp_reserve), /* Init reserved block */
		(u_char)' ', (PTR)protup->dbp_reserve);

    /* giving permission on the newly created object */
    STRUCT_ASSIGN_MACRO(dmu_cb->dmu_tbl_id, protup->dbp_tabid);

    /* creating an "access" permit to indicate that one may access the object */
    protup->dbp_popset = protup->dbp_popctl = DB_CAN_ACCESS;

    /* ... all of its columns */
    for (i = 0; i <= DB_COL_BITS; i++)
    {
	BTset(i, (char *) domset);
    }

    /* ... any time */
    protup->dbp_pdbgn = 0;
    protup->dbp_pdend = 1440;
    protup->dbp_pwbgn = 0;
    protup->dbp_pwend = 6;

    /* ... from any terminal */
    MEfill(sizeof(protup->dbp_term), ' ', (PTR) &protup->dbp_term);

    /* Create the protection definition. */
    qeuq_cb.qeuq_cp = 1;
    qef_qrydata.dt_next = NULL;
    qef_qrydata.dt_size = sizeof(DB_PROTECTION);
    qef_qrydata.dt_data = (PTR) protup;

    qeuq_cb.qeuq_pro_tup = &qef_qrydata;

    /* execute the QEF update operation */
    qeuq_cb.qeuq_eflag  = QEF_EXTERNAL;
    qeuq_cb.qeuq_type   = QEUQCB_CB;
    qeuq_cb.qeuq_length = sizeof(QEUQ_CB);

    /* no dependent tables */
    qeuq_cb.qeuq_tsz    = 0;
    qeuq_cb.qeuq_tbl_id = (DB_TAB_ID *) NULL;

    /* store id of the newly created object */
    qeuq_cb.qeuq_rtbl = &dmu_cb->dmu_tbl_id;

    qeuq_cb.qeuq_d_id   = qeu_cb->qeu_d_id;
    qeuq_cb.qeuq_db_id  = qeu_cb->qeu_db_id;

    /* initialize the independent object/privilege structure */
    indep_privs.psq_objs	= (PSQ_OBJ *) NULL;
    indep_privs.psq_objprivs	= (PSQ_OBJPRIV *) NULL;
    indep_privs.psq_colprivs    = (PSQ_COLPRIV *) NULL;
    indep_privs.psq_grantee	= &dmu_cb->dmu_owner;

    qeuq_cb.qeuq_indep = (PTR) &indep_privs;

    status = qeu_cprot(qef_cb, &qeuq_cb);

    if (status != E_DB_OK)
    {
	qeu_cb->error.err_code = qeuq_cb.error.err_code;
    }
    return(status);
}

/*
** Name: qeu_new_prot_tuple - modify catalogs to record creation of a new permit
**			      tuple
**
** Description:	    Insert a tuple representing new permit into IIPROTECT;
**		    unless we are creating an "access permit", insert tuple(s)
**		    into IIQRYTEXT;
**		    if there is a tree associated with the new IIPROTECT tuple,
**		    insert it into IITREE;
**		    if there is dependency information associated with the new
**		    IIPROTECT tuple, insert it into IIDBDEPENDS.
**		    if the caller indicated that the new permit depends on some
**		    privilege descriptor and, possibly, that the privilege
**		    descriptor should be built, call qeu_enter_dependencies() to
**		    do the work.
**
**		    NOTE: caller is expected to open and close all relevant
**			  catalogs.
**
** Input parameters:
**
**      qef_cb                  qef session control block
**      qeuq_cb
**	    .qeuq_rtbl		DB_TAB_ID of table/procedure getting on which
**				the protection is being defined.
**				In case of event this is a dummy non-zero id.
**          .qeuq_tsz           Number of independent tables in permit
**	    .qeuq_tbl_id	array of base table id protection refers to
**				in the qualification clause of the create
**				permit statement.
**	    .qeuq_ct		number of tree tuples
**	    .qeuq_tre_tup	tree tuples
**	    .qeuq_cq		number of qrytext tuples
**	    .qeuq_qry_tup	qrytext tuples
**	    .qeuq_pro_tup	protection tuples
**				NOTE: this is the tuple passed by the caller;
**				      since in some cases we coudl be modifying
**				      the permit's privilege map, we decided to
**				      make a copy of the tuple and use it
**				      instead; however, it is important that we
**				      store new permit number in the caller's
**				      tuple since it may be used later (in
**				      qeu_enter_dependencies()).
**	    .qeuq_indep		describes privileges, if any, on which the new
**				permit depends;
**				will be used only if we are told to build a new
**				privilege descriptor
**				
**	prot_qeu		pointer to a QEU_CB used to access IIPROTECT
**				catalog
**	    qeu_input
**		dt_data		pointer to the tuple which will be inserted
**	tree_qeu		pointer to a QEU_CB used to access IITREE
**				catalog; NULL if catalog will not be needed
**	text_qeu		pointer to a QEU_CB used to access IIQRYTEXT
**				catalog; NULL if catalog will not be needed
**	dpnd_qeu		pointer to a QEU_CB used to access IIDBDEPENDS
**				catalog; NULL if catalog will not be needed
**	permno			number to be assigned to the new permit
**      priv_descr_no		if > 0, represents the number of a
**				privilege descriptor on which this permit 
**				will depend; 
**				otherwise we are either creating a 
**				security_alarm or we are creating a permit 
**				which does not depend on any privileges
**	bld_priv_descr		if TRUE, we must build a privilege
**				descriptor
**
** Output parameters:
**
**	error			will contain error number if some unexpected
**				condtion occurs
**
** Side effects:
**	none
**
** History:
**
**	10-jul-91 (andre)
**	    written
**	10-jul-92 (andre)
**	    added priv_descr_no and bld_priv_descr to the interface
**	    if new permit depends on some privilege, we will call
**	    qeu_enter_dependencies() to build a new priivlege descriptor (if
**	    necessary) and describe the dependency in IIDBDEPENDS.
**	23-apr-93 (anitap)
**	    Fixed compiler warning.
**	27-oct-93 (andre)
**	    As a part of fix for bug 51852 (which was reported in 6.4 but can
**	    and will manifest itself in 6.5 as soon as someone places some 
**	    stress on the DBMS), we want to use a combination of the first half
**	    of table id and the permit number (really guaranteed to
**	    be unique) instead of timestamps (allegedly unique, but in fact may
**	    be non-unique if several objects get created in rapid succession on
**	    a reasonably fast box) to identify IIQRYTEXT and IITREE tuples
**          associated with a given permit.  This combination id (pumped through
**	    randomizing function) will be used to key into IIQRYTEXT and will 
**	    be stored instead of the timestamp in IITREE 
**	14-Jan-2004 (jenjo02)
**	    Check db_tab_index > 0 for index, not simply != 0.
*/
static DB_STATUS
qeu_new_prot_tuple(
QEF_CB	    *qef_cb,
QEUQ_CB	    *qeuq_cb,
QEU_CB	    *prot_qeu,
QEU_CB      *tree_qeu,
QEU_CB      *text_qeu,
QEU_CB      *dpnd_qeu,
i4	    permno,
i2	    priv_descr_no,
bool	    bld_priv_descr,
i4	    *error)
{
    DB_STATUS		    status;
    register i4	    i;
    register QEF_DATA	    *next;
    DB_PROTECTION	    *ptuple;
    DB_IIQRYTEXT	    *qtuple;
    DB_IITREE		    *ttuple;
    DB_TAB_ID       	    randomized_id;

    /* update permit number in the caller's copy of the tuple */
    ptuple = (DB_PROTECTION *) qeuq_cb->qeuq_pro_tup->dt_data;
    ptuple->dbp_permno = permno;

    /*
    ** now get the address of the copy which we will be inserting into IIPROTECT
    ** and store the new permit's number and other useful values in it
    */
    ptuple = (DB_PROTECTION *) prot_qeu->qeu_input->dt_data;
    ptuple->dbp_permno = permno;

    /*
    ** if there will be text and/or tree tuples associated with this IIPROTECT 
    ** tuple, compute a "randomized" id which will be used to join this tuple 
    ** to IIQRYTEXT and IITREE (if any) tuples
    */
    if (ptuple->dbp_popset != DB_CAN_ACCESS || qeuq_cb->qeuq_ct)
    {
	randomized_id.db_tab_base = 
	    ulb_rev_bits(qeuq_cb->qeuq_rtbl->db_tab_base);
	randomized_id.db_tab_index = ulb_rev_bits((u_i4) permno);

	if (ptuple->dbp_popset != DB_CAN_ACCESS)
	{
	    /* no query text is generated for "access" permits */
	    ptuple->dbp_txtid.db_qry_high_time = randomized_id.db_tab_base;
	    ptuple->dbp_txtid.db_qry_low_time  = randomized_id.db_tab_index;
	}

	if (qeuq_cb->qeuq_ct)
	{
	    ptuple->dbp_treeid.db_tre_high_time = randomized_id.db_tab_base;
	    ptuple->dbp_treeid.db_tre_low_time  = randomized_id.db_tab_index;
	}
    }

    /* This code assumes only one protection tuple exists per call. */
    status = qeu_append(qef_cb, prot_qeu);
    if (status != E_DB_OK)
    {
	*error = prot_qeu->error.err_code;
	return(status);
    }
	
    /*
    ** if we were creating a special permit to indicate accessability of a
    ** newly created object to its owner (accessibility permit), we are done
    */
    if (ptuple->dbp_popset == DB_CAN_ACCESS)
    {
	return(E_DB_OK);
    }
    
    /* Now Update the iiqrytext system catalog. */

    /* Insert query text id into all query text tuples. */
    for (next = qeuq_cb->qeuq_qry_tup, i = 0;
	 i < qeuq_cb->qeuq_cq;
	 next = next->dt_next, i++)
    {
	if (!next)
	{
	    *error = E_QE0018_BAD_PARAM_IN_CB;
	    return(E_DB_ERROR);
	}
	qtuple = (DB_IIQRYTEXT*) next->dt_data;
	qtuple->dbq_txtid.db_qry_high_time = randomized_id.db_tab_base;
	qtuple->dbq_txtid.db_qry_low_time  = randomized_id.db_tab_index;
    }

    /* Put the query text tuples into iiqrytext */
    status = qeu_append(qef_cb, text_qeu);
    if (status != E_DB_OK)
    {
	*error = text_qeu->error.err_code;
	return(status);
    }

    /* If tree tuples are given, update the IITREE system catalog */
    if (qeuq_cb->qeuq_ct)
    {
	/* Insert tree id into all tree tuples. */
	for (next = qeuq_cb->qeuq_tre_tup, i = 0;
	     i < qeuq_cb->qeuq_ct;
	     next = next->dt_next, i++)
	{
	    if (!next)
	    {
		*error = E_QE0018_BAD_PARAM_IN_CB;
		return(E_DB_ERROR);
	    }
	    ttuple = (DB_IITREE*) next->dt_data;
	    ttuple->dbt_trid.db_tre_high_time = randomized_id.db_tab_base;
	    ttuple->dbt_trid.db_tre_low_time = randomized_id.db_tab_index;
	}

	/* Put the tree tuples into iitree */
	status = qeu_append(qef_cb, tree_qeu);
	if (status != E_DB_OK)
	{
	    *error = tree_qeu->error.err_code;
	    return(status);
	}
    } /* end if tree information */

    /* update dependencies table */
    if (qeuq_cb->qeuq_tsz)
    {
	DB_IIDBDEPENDS  *dtuple = (DB_IIDBDEPENDS *) dpnd_qeu->qeu_input->dt_data;

	/* initialize static elements of IIDBDEPENDS tuple */
	dtuple->dbv_itype		= DB_TABLE;
	dtuple->dbv_dep.db_tab_base	= qeuq_cb->qeuq_rtbl->db_tab_base;
	dtuple->dbv_dep.db_tab_index	= qeuq_cb->qeuq_rtbl->db_tab_index;
	dtuple->dbv_dtype		= DB_PROT;
	dtuple->dbv_qid			= permno;
	dtuple->dbv_i_qid		= 0;
	
	/* build the dependency tuple */
	for (i = 0; i < qeuq_cb->qeuq_tsz; i++)	
	{
	    if (qeuq_cb->qeuq_tbl_id[i].db_tab_base == 0 &&
		qeuq_cb->qeuq_tbl_id[i].db_tab_index == 0)
		continue;		/* skip derived table entries */

	    dtuple->dbv_indep.db_tab_base = qeuq_cb->qeuq_tbl_id[i].db_tab_base;

	    /* if we are about to enter an index into IIDBDEPENDS, 
	    ** make an entry for the index's base table.
	    */
	    if (qeuq_cb->qeuq_tbl_id[i].db_tab_index > 0)
	    {
		dtuple->dbv_indep.db_tab_index = 0;
		status = qeu_append(qef_cb, dpnd_qeu);
		if (status != E_DB_OK)
		{
		    *error = dpnd_qeu->error.err_code;
		    return(status);
		}
	    }

	    dtuple->dbv_indep.db_tab_index =
		qeuq_cb->qeuq_tbl_id[i].db_tab_index; 
	    status = qeu_append(qef_cb, dpnd_qeu);
	    if (status != E_DB_OK)
	    {
		*error = dpnd_qeu->error.err_code;
		return(status);
	    }
	}
    }

    /*
    ** If a permit depends on some privilege, we need to call
    ** qeu_enter_dependencies() to record that fact.  Caller provided us with
    ** indication of whether we need to actually build a new descriptor or use
    ** an existing one.
    */
    if (priv_descr_no > 0)
    {
	PSQ_INDEP_OBJECTS   loc_indep;
	PSQ_INDEP_OBJECTS   *indep_objs =
				(PSQ_INDEP_OBJECTS *) qeuq_cb->qeuq_indep;
	
	union
	{
	    PSQ_OBJPRIV	    obj_priv;
	    PSQ_COLPRIV	    col_priv;
	} privs;
	
	i4		    cur_priv;

	loc_indep.psq_objs	= (PSQ_OBJ *) NULL;
	loc_indep.psq_objprivs	= (PSQ_OBJPRIV *) NULL;
	loc_indep.psq_colprivs	= (PSQ_COLPRIV *) NULL;
	loc_indep.psq_grantee	= indep_objs->psq_grantee;
	
	/*
	** if we are told to build a privilege descriptor, we need to find the
	** appropiate descriptor among those passed by the caller (recall that
	** the list generated by the caller applies to all privileges specified
	** by the user whereas permits are created for one privilege at a time)
	** 
	** privilege descriptors may be required only for permit specifying
	** DELETE, INSERT, or UPDATE privilege.  We will scan the list passed by
	** the user looking for a descriptor specifying appropriate privilege,
	** copy it into our local copy and attach it to the local independent
	** object/privilege list which will get passed to qeu_enter_dependencies
	*/

	if (bld_priv_descr)
	{
	    cur_priv =
		ptuple->dbp_popset & (i4) (DB_DELETE | DB_REPLACE | DB_APPEND);

	    /*
	    ** we generate independent privileges only for permits on views;
	    ** DELETE will always depend on table-wide privilege whereas INSERT
	    ** and UPDATE will always depend on column-specific privileges
	    */
	    if (cur_priv == (i4) DB_DELETE)
	    {
		if (indep_objs->psq_objprivs)
		{
		    STRUCT_ASSIGN_MACRO((*indep_objs->psq_objprivs),
			privs.obj_priv);
		    privs.obj_priv.psq_next = (PSQ_OBJPRIV *) NULL;
		    loc_indep.psq_objprivs  = &privs.obj_priv;
		}
	    }
	    else
	    {
		PSQ_COLPRIV	*p;

		for (p = indep_objs->psq_colprivs;
		     (p != (PSQ_COLPRIV *) NULL && p->psq_privmap != cur_priv);
		     p = p->psq_next
		    )
		;

		if (p)
		{
		    STRUCT_ASSIGN_MACRO((*p), privs.col_priv);
		    privs.col_priv.psq_next = (PSQ_COLPRIV *) NULL;
		    loc_indep.psq_colprivs = &privs.col_priv;
		}
	    }
	}
	
	qeuq_cb->qeuq_indep = (PTR) &loc_indep;

	status = qeu_enter_dependencies(qef_cb, qeuq_cb, qeuq_cb->qeuq_rtbl,
	    (DB_QMODE) DB_PROT, priv_descr_no);

	/*
	** first things - first: we must restore the caller's independent
	** privilege list
	*/
	qeuq_cb->qeuq_indep = (PTR) indep_objs;

	/* now we examine the status */
	if (status != E_DB_OK)
	{
	    *error = qeuq_cb->error.err_code;
	    return(status);
	}
    }

    return(E_DB_OK);
}

/*
** Name:    qeu_enter_dependencies - enter objects/privileges on which a
**				     specified object depends into appropriate
**				     catalogs
**
** Description:
**	For a given object (view or dbproc), permit, or constraint, enter
**	objects and privileges on which the object depends into IIDBDEPENDS and
**	IIPRIV, respectively
**
** Input:
**
**	qef_cb		    QEF session control block
**	qeuq_cb		    QEU query CB
**	  qeuq_db_id	    database id
**	  qeuq_indep	    structure describing objects/privileges on which the
**			    specified object depends
**	    psq_objs		list of structures describing independent
**				objects for a view or a dbproc
**	    psq_objprivs	list of structures describing independent
**				object-wide privileges for a view, dbproc, or
**				permit
**	    psq_colprivs	list of structures describing independent
**				column-specific privileges for a view, dbproc,
**				or permit
**	  qeuq_pro_tup	    if dep_type==DB_PROT, ptr to QEF_DATA containing
**			    a description of a new IIPROTECT tuple
**	    dt_data		ptr to an IIPROTECT tuple describing the permit
**	      dbp_permno	    permit number
**	  qeuq_int_tup	    if dep_type==DB_CONS, ptr to QEF_DATA containing
**			    a description of a new IIINTEGRITY tuple
**	    dt_data		ptr to an IIINTEGRITYtuple describing the
**				constraint
**		dbi_number	    integrity number
**	  qeuq_flag
**	    QEU_QUEL_VIEW   entering dependency information for a QUEL view
**	dep_id		    dependent object id
**	dep_type		dependent object type
**	    DB_VIEW		dependent object is of type VIEW
**	    DB_DBP		dependent object is of type DBPROC
**	    DB_PROT		dependent object is of type PERMIT
**	    DB_CONS		dependent object is of type constraint
**	priv_descr_no	    number to be assigned to an independent privilege
**			    descriptor if one is created
**
** Output:
**	qeuq_cb
**	  error 	    filled in if an error occurs
**
** Side effects:
**	allocates and deallocates memory
**
** Returns:
**	E_DB_{OK,ERROR}
**
** History:
**
**	01-jun-92 (andre)
**	    written
**	10-jun-92 (andre)
**	    rename db_priv_flags to db_dep_obj_type - dependent object type
**	26-jun-92 (andre)
**	    caller will no longer pass in a (QEU_CB *); we will allocate one on
**	    stack and initialize it here;
**
**	    caller will pass in a (QEUQ_CB *) from which we will obtain the
**	    independent object list, error block and the value for
**	    QEU_CB.qeu_db_id - thus eliminating need to pass indep and err_blk
**	    separately
**
**	    Finally, this function will do its own error reporting so the caller
**	    should not call qef_error() if this function returns bad status - so
**	    we don't need to pass in &error either
**	10-jul-92 (andre)
**	    added priv_descr_no to the interface - this specifies the number of
**	    an independent privilege descriptor on which an object depends;
**	    For views and dbprocs it will be set to 0 indicating that the
**	    dependency is implied and does not need to be explicitly recorded in
**	    IIDBDEPENDS, for permits, however, this number will be non-zero and
**	    we will record the dependency of a new permit on the specified
**	    descriptor
**
**	    When entering dependencies for a new permit, instead of reporting
**	    an error here we will pass it to the caller inside
**	    qeuq_cb->error.err_code since caller of qeu_new_prot_tuple() prefers
**	    to report its own errors
**	16-sep-92 (andre)
**	    when inserting rows into IIDBDEPENDS for QUEL view, set dependent
**	    object type to DB_VIEW and not to DB_QUEL_VIEW (DB_QUEL_VIEW is
**	    intended for use in IIPRIV only where it enables us to determine
**	    whether a given view is abandoned if the only remaining privilege
**	    on which it depends is not GRANT-compatible).  DB_QUEL_VIEW will go
**	    away after the next integration since the plan is to add a flag
**	    field to IIPRIV and have a bit in it to indicate whether the
**	    privilege described by the row needs to be GRANT-compatible
**	17-sep-92 (andre)
**	    caller will no longer indicate to us whether we are recording
**	    independent objects/privileges for a QUEL view.  Instead, we will
**	    check whether (qeuq_cb->qeuq_flag & QEU_QUEL_VIEW) and if so, will
**	    NOT set DB_GRANT_COMPATIBLE_PRIV in DB_IIPRIV.db_prv_flags
**	27-mar-93 (andre)
**	    made minor changes to record objects/privileges on which a
**	    constraint depends
**	21-oct-93 (andre)
**	    dbprocs can depend on synonyms - made changes to enable recording of
**	    dependencies on synonyms
**	14-may-02 (inkdo01)
**	    Add support for sequence privileges.
**	14-Jan-2004 (jenjo02)
**	    Check db_tab_index > 0 for index, not simply != 0.
*/
static DB_STATUS
qeu_enter_dependencies(
QEF_CB			*qef_cb,
QEUQ_CB			*qeuq_cb,
DB_TAB_ID		*dep_id,
DB_QMODE		dep_type,
i2			priv_descr_no)
{
    GLOBALREF QEF_S_CB	    *Qef_s_cb;
    ULM_RCB		    ulm;
    i4		    error;
    PSQ_INDEP_OBJECTS       *indep = (PSQ_INDEP_OBJECTS *) qeuq_cb->qeuq_indep;
    DB_ERROR		    *err_blk = &qeuq_cb->error;
    QEU_CB		    qeucb, *qeu = &qeucb;
    QEF_DATA		    qef_data;
    bool		    tbl_opened = FALSE;
    DB_STATUS		    status = E_DB_OK;
    DB_IIDBDEPENDS	    *dtuple = (DB_IIDBDEPENDS *) NULL;
    i4			    i;
    i4			    exit_loop, exit_loop2;

    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
    ulm.ulm_streamid = (PTR)NULL;

    qeu->qeu_type = QEUCB_CB;
    qeu->qeu_length = sizeof(QEUCB_CB);
    qeu->qeu_db_id = qeuq_cb->qeuq_db_id;
    qeu->qeu_lk_mode = DMT_IX;
    qeu->qeu_flag = DMT_U_DIRECT;
    qeu->qeu_access_mode = DMT_A_WRITE;
    qeu->qeu_mask = 0;
    qeu->qeu_qual = qeu->qeu_f_qual = 0;
    qeu->qeu_qarg = qeu->qeu_f_qarg = 0;
    qeu->qeu_klen = 0;
    qeu->qeu_key = (DMR_ATTR_ENTRY **) NULL;

    for (exit_loop = FALSE; !exit_loop; ) 	/* something to break out of */
    {
	if (indep->psq_objs)
	{
	    PSQ_OBJ		*indep_objs;
	    DB_TAB_ID		*indep_id;
	    
	    /* 
	    ** Allocate space for an IIDBDEPENDS tuple 
	    */
	    /* Open stream and allocate memory with one effort */
	    ulm.ulm_flags = ULM_PRIVATE_STREAM | ULM_OPEN_AND_PALLOC;
	    ulm.ulm_psize = sizeof(DB_IIDBDEPENDS);

	    if ((status = qec_mopen(&ulm)) != E_DB_OK)
	    {
		error = E_QE001E_NO_MEM;
		break;
	    }

	    dtuple = (DB_IIDBDEPENDS*) ulm.ulm_pptr;

	    qeu->qeu_tab_id.db_tab_base = DM_B_DEPENDS_TAB_ID;
	    qeu->qeu_tab_id.db_tab_index = DM_I_DEPENDS_TAB_ID;	 
	    status = qeu_open(qef_cb, qeu);
	    if (status != E_DB_OK)
	    {
		error = qeu->error.err_code;
		break;
	    }

	    tbl_opened = TRUE;
		    
	    qeu->qeu_count = 1;
	    qeu->qeu_tup_length = sizeof(DB_IIDBDEPENDS);
	    qeu->qeu_input = &qef_data;
	    qef_data.dt_next = 0;
	    qef_data.dt_size = sizeof(DB_IIDBDEPENDS);
	    qef_data.dt_data = (PTR) dtuple;

	    /*
	    ** Now format the tuples needed to update IIDBDEPENDS.
	    ** We need to record dependence of an object on objects used in its
	    ** definition.
	    ** For a view, these objects can be tables and views;
	    ** for a database procedure they can be tables, views, dbprocs, or
	    ** dbevents
	    ** for a constraint (for now it will only be a REFERENCES
	    ** constraint), the independent object will be another constraint
	    */
	    
	    /* populate static elements of IIDBDEPENDS tuple */
	    dtuple->dbv_dep.db_tab_base	    = dep_id->db_tab_base;	    
	    dtuple->dbv_dep.db_tab_index    = dep_id->db_tab_index;
	    dtuple->dbv_dtype		    = dep_type;

	    if (dep_type == DB_CONS)
	    {
		/*
		** if recording independent object info for a constraint, copy
		** integrity number of dependent constraint into IIDBDEPENDS
		** tuple
		*/
		DB_INTEGRITY	*ituple =
		    (DB_INTEGRITY *) qeuq_cb->qeuq_int_tup->dt_data;

		dtuple->dbv_qid   = ituple->dbi_number;

	    }
	    else if (dep_type == DB_PROT)
	    {
		DB_PROTECTION	*ptuple =
		    (DB_PROTECTION *) qeuq_cb->qeuq_pro_tup->dt_data;

		dtuple->dbv_qid = ptuple->dbp_permno;
	    }
	    else
	    {
		dtuple->dbv_qid	  = 0;
	    }

	    dtuple->dbv_i_qid = 0;
	    
	    /* 
	    ** For each object on which this object depends, build a
	    ** dependency tuple and write it to IIDBDEPENDS.
	    */

	    for (indep_objs = indep->psq_objs;
		 indep_objs;
		 indep_objs = indep_objs->psq_next
		)
	    {
		/* determine type of independent object */
		if (indep_objs->psq_objtype & PSQ_OBJTYPE_IS_TABLE)
		{
		    dtuple->dbv_itype = DB_TABLE;
		}
		else if (indep_objs->psq_objtype & PSQ_OBJTYPE_IS_DBPROC)
		{
		    dtuple->dbv_itype = DB_DBP;
		}
		else if (indep_objs->psq_objtype & PSQ_OBJTYPE_IS_DBEVENT)
		{
		    dtuple->dbv_itype = DB_EVENT;
		}
		else if (indep_objs->psq_objtype & PSQ_OBJTYPE_IS_SEQUENCE)
		{
		    dtuple->dbv_itype = DB_SEQUENCE;
		}
		else if (indep_objs->psq_objtype & PSQ_OBJTYPE_IS_SYNONYM)
		{
		    dtuple->dbv_itype = DB_SYNONYM;
		}
		else if (indep_objs->psq_objtype & PSQ_OBJTYPE_IS_CONSTRAINT)
		{
		    dtuple->dbv_itype = DB_CONS;

		    /*
		    ** for independent constraint, copy its integrity number
		    ** into IIDBDEPENDS tuple
		    */
		    dtuple->dbv_i_qid = indep_objs->psq_2id;
		}
		else
		{
		    /* this should never happen */
		    continue;
		}
		
		for (indep_id = indep_objs->psq_objid,
		     i = indep_objs->psq_num_ids;
		     
		     i > 0;
		     
		     indep_id++, i--
		    )
		{
		    /*
		    ** if we are about to enter an index into IIDBDEPENDS, 
		    ** make an entry for the index's base table.
		    */
		    dtuple->dbv_indep.db_tab_base = indep_id->db_tab_base;
		    if (dtuple->dbv_itype == DB_TABLE && indep_id->db_tab_index > 0)
		    {
			dtuple->dbv_indep.db_tab_index = 0; 
			status = qeu_append(qef_cb, qeu);
			if (status != E_DB_OK)
			{
			    error = qeu->error.err_code;
			    break;
			}
		    }

		    dtuple->dbv_indep.db_tab_index = indep_id->db_tab_index;
		    status = qeu_append(qef_cb, qeu);
		    if (status != E_DB_OK)
		    {
			error = qeu->error.err_code;
			break;
		    }
		}

		if (status != E_DB_OK)
		    break;
	    }

	    if (status != E_DB_OK)
		break;

	    status = qeu_close(qef_cb, qeu);    
	    if (status != E_DB_OK)
	    {
		error = qeu->error.err_code;
		break;
	    }

	    tbl_opened = FALSE;
	}

	/*
	** check if we need to enter table-wide and/or column-specific
	** privileges into IIPRIV
	*/
	if (indep->psq_objprivs || indep->psq_colprivs)
	{
	    DB_IIPRIV	    *priv_tuple;

	    /*
	    ** allocate memory for IIPRIV tuple
	    **
	    ** if no IIDBDEPENDS tuples had to be created, we need to open
	    ** the memory stream first
	    */
	    ulm.ulm_psize = sizeof(DB_IIPRIV);
	    if (ulm.ulm_streamid == (PTR)NULL)
	    {
		/* Open stream and allocate memory in one action */
		ulm.ulm_flags = ULM_PRIVATE_STREAM | ULM_OPEN_AND_PALLOC;
		status = qec_mopen(&ulm);
	    }
	    else
	    {
		status = qec_malloc(&ulm);
	    }

	    if (status != E_DB_OK)
	    {
		if (ulm.ulm_streamid != (PTR)NULL)
		    ulm_closestream(&ulm);
		error = E_QE001E_NO_MEM;
		break;
	    }
	    priv_tuple = (DB_IIPRIV *) ulm.ulm_pptr;

	    qeu->qeu_tab_id.db_tab_base = DM_B_PRIV_TAB_ID;
	    qeu->qeu_tab_id.db_tab_index = DM_I_PRIV_TAB_ID;
	    status = qeu_open(qef_cb, qeu);
	    if (status != E_DB_OK)
	    {
		error = qeu->error.err_code;
		break;
	    }

	    tbl_opened = TRUE;
		    
	    qeu->qeu_count = 1;
	    qeu->qeu_tup_length = sizeof(DB_IIPRIV);
	    qeu->qeu_input = &qef_data;
	    qef_data.dt_next = 0;
	    qef_data.dt_size = sizeof(DB_IIPRIV);
	    qef_data.dt_data = (PTR) priv_tuple;

	    /*
	    ** build a descriptor of privileges on which the new view, dbproc,
	    ** permit, or constraint depends
	    */

	    /* populate static elements of IIPRIV tuple */
	    priv_tuple->db_dep_obj_id.db_tab_base = dep_id->db_tab_base;
	    priv_tuple->db_dep_obj_id.db_tab_index = dep_id->db_tab_index;
	    priv_tuple->db_descr_no = priv_descr_no;
	    priv_tuple->db_dep_obj_type = dep_type;
	    if (   dep_type == DB_VIEW
	        && qeuq_cb->qeuq_flag_mask & QEU_QUEL_VIEW)
	    {
		priv_tuple->db_prv_flags = 0;
	    }
	    else
	    {
		priv_tuple->db_prv_flags = DB_GRANT_COMPATIBLE_PRIV;
	    }
	    STRUCT_ASSIGN_MACRO((*indep->psq_grantee), priv_tuple->db_grantee);

	    /* for no particular reason, handle object-wide privileges first */
	    if (indep->psq_objprivs)	
	    {
		PSQ_OBJPRIV	*priv_descr;
		
		for (i = DB_COL_WORDS - 1; i >= 0; i--)
		    priv_tuple->db_priv_attmap[i] = ~((i4) 0);

		for (priv_descr = indep->psq_objprivs;
		     priv_descr;
		     priv_descr = priv_descr->psq_next
		    )
		{
		    priv_tuple->db_indep_obj_id.db_tab_base =
			priv_descr->psq_objid.db_tab_base;
		    priv_tuple->db_indep_obj_id.db_tab_index =
			priv_descr->psq_objid.db_tab_index;

		    /*
		    ** priv_descr->psq_privmap may describe multiple privileges;
		    ** here we need to separate them
		    */

		    for (i = -1;
			 (i = BTnext(i, (char *) &priv_descr->psq_privmap,
			          BITS_IN(priv_descr->psq_privmap))) != -1;
			)
		    {
			priv_tuple->db_priv_map = (i4) 0;

			BTset(i, (char *) &priv_tuple->db_priv_map);

			status = qeu_append(qef_cb, qeu);
			if (status != E_DB_OK)
			{
			    error = qeu->error.err_code;
			    break;
			}
		    }

		    if (status != E_DB_OK)
			break;
		}

		if (status != E_DB_OK)
		    break;
	    }

	    /*
	    ** now check whether there are any independent column-specific
	    ** privileges
	    */
	    if (indep->psq_colprivs)
	    {
		PSQ_COLPRIV	    *priv_descr;

		for (priv_descr = indep->psq_colprivs;
		     priv_descr;
		     priv_descr = priv_descr->psq_next
		    )
		{
		    priv_tuple->db_indep_obj_id.db_tab_base =
			priv_descr->psq_tabid.db_tab_base;
		    priv_tuple->db_indep_obj_id.db_tab_index =
			priv_descr->psq_tabid.db_tab_index;

		    /*
		    ** copy map of attributes to which the privilege applies
		    */
		    for (i = DB_COL_WORDS - 1; i >= 0; i--)
			priv_tuple->db_priv_attmap[i] =
			    priv_descr->psq_attrmap[i];

		    priv_tuple->db_priv_map = priv_descr->psq_privmap;

		    status = qeu_append(qef_cb, qeu);
		    if (status != E_DB_OK)
		    {
			error = qeu->error.err_code;
			break;
		    }
		}

		if (status != E_DB_OK)
		    break;
	    }

	    status = qeu_close(qef_cb, qeu);    
	    if (status != E_DB_OK)
	    {
		error = qeu->error.err_code;
		break;
	    }

	    tbl_opened = FALSE;
	}

	/*
	** if we were entering dependencies for a permit or a constraint, we may
	** need to record dependency existing between a permit or a constraint
	** and a privilege descriptor.
	*/
	if (   (   dep_type == (DB_QMODE) DB_PROT
		|| dep_type == (DB_QMODE) DB_CONS)
	    && priv_descr_no > 0)
	{
	    /* 
	    ** Allocate space for an IIDBDEPENDS tuple 
	    */
	    if (!dtuple)
	    {
		ulm.ulm_psize = sizeof(DB_IIDBDEPENDS);
		if (ulm.ulm_streamid == (PTR)NULL)
		{
		    /* Open stream and allocate memory in one action */
		    ulm.ulm_flags = ULM_PRIVATE_STREAM | ULM_OPEN_AND_PALLOC;
		    status = qec_mopen(&ulm);
		}
		else
		{
		    status = qec_malloc(&ulm);
		}

		if (status != E_DB_OK)
		{
		    if (ulm.ulm_streamid != (PTR)NULL)
			ulm_closestream(&ulm);
		    error = E_QE001E_NO_MEM;
		    break;
		}
		dtuple = (DB_IIDBDEPENDS*) ulm.ulm_pptr;
	    }

	    qeu->qeu_tab_id.db_tab_base = DM_B_DEPENDS_TAB_ID;
	    qeu->qeu_tab_id.db_tab_index = DM_I_DEPENDS_TAB_ID;	 
	    status = qeu_open(qef_cb, qeu);
	    if (status != E_DB_OK)
	    {
		error = qeu->error.err_code;
		break;
	    }

	    tbl_opened = TRUE;
		    
	    qeu->qeu_count = 1;
	    qeu->qeu_tup_length = sizeof(DB_IIDBDEPENDS);
	    qeu->qeu_input = &qef_data;
	    qef_data.dt_next = 0;
	    qef_data.dt_size = sizeof(DB_IIDBDEPENDS);
	    qef_data.dt_data = (PTR) dtuple;

	    /*
	    ** Now format the tuples needed to update IIDBDEPENDS.
	    ** We need to record dependence of a permit on a privilege
	    ** descriptor.
	    */
	    
	    /* populate static elements of IIDBDEPENDS tuple */
	    dtuple->dbv_dep.db_tab_base =
		dtuple->dbv_indep.db_tab_base = dep_id->db_tab_base;
	    dtuple->dbv_dep.db_tab_index =
		dtuple->dbv_indep.db_tab_index = dep_id->db_tab_index;
	    dtuple->dbv_dtype = dep_type;

	    /*
	    ** dbv_qid needs to be set to the permit (if we were called to
	    ** record dependence of a new permit on a privilege) or integrity
	    ** (if we were called to record dependence of a new constraint on a
	    ** privilege) number
	    */
	    if (dep_type == (DB_QMODE) DB_PROT)
	    {
		DB_PROTECTION	*ptuple =
		    (DB_PROTECTION *) qeuq_cb->qeuq_pro_tup->dt_data;

		dtuple->dbv_qid = ptuple->dbp_permno;
	    }
	    else if (dep_type == (DB_QMODE) DB_CONS)
	    {
		DB_INTEGRITY	*ituple =
		    (DB_INTEGRITY *) qeuq_cb->qeuq_int_tup->dt_data;

		dtuple->dbv_qid = ituple->dbi_number;
	    }

	    dtuple->dbv_itype		    = DB_PRIV_DESCR;
	    dtuple->dbv_i_qid		    = priv_descr_no;

	    status = qeu_append(qef_cb, qeu);
	    if (status != E_DB_OK)
	    {
		error = qeu->error.err_code;
		break;
	    }

	    status = qeu_close(qef_cb, qeu);    
	    if (status != E_DB_OK)
	    {
		error = qeu->error.err_code;
		break;
	    }

	    tbl_opened = FALSE;
	}

	/* 
	** Close the input stream created for the IIDBDEPENDS and IIPRIV tuples. 
	*/

	if (ulm.ulm_streamid != (PTR) NULL)
	    ulm_closestream(&ulm);

	exit_loop = TRUE;
    }

    if (status == E_DB_OK)
	return(E_DB_OK);

    /*
    ** we get here only if something went wrong 
    */

    /*
    ** when recording dependencies for permits, rather than report the error
    ** here, just return the error code inside qeuq_cb->error.err_code
    */
    if (dep_type == (DB_QMODE) DB_PROT)
	qeuq_cb->error.err_code = error;
    else
	(VOID) qef_error(error, 0L, status, &error, err_blk, 0);

    /* if the memory stream is still open, close it now */
    if (ulm.ulm_streamid != (PTR) NULL)
	ulm_closestream(&ulm);

    if (tbl_opened)
    {
	DB_STATUS	stat;
	
	stat = qeu_close(qef_cb, qeu);
	if (stat != E_DB_OK)
	{
	    /*
	    ** if we were trying to record dependencies between permits and
	    ** privilege descriptors, we want to return the last error code to
	    ** the caller so we'll have to report the previous message here
	    */
	    if (dep_type == (DB_QMODE) DB_PROT)
	    {
		(VOID) qef_error(qeuq_cb->error.err_code, 0L, stat, &error,
		    err_blk, 0);
		qeuq_cb->error.err_code = qeu->error.err_code;
	    }
	    else
	    {
		(VOID) qef_error(qeu->error.err_code, 0L, stat, &error,
		    err_blk, 0);
	    }
	    if (stat > status)
		status = stat;
	}
    }

    return(status);
}
/*
** qeu_tr_ts_cmp() -	compare timestamp against that found in IITREE tuple.
**
** Description:
**	This function is passed by qeu_dprot() and qeu_dintg() inside QEU_CB to
**	qeu_delete() to be used in qualifying tuples returned by dmr_get() when
**	it is desired to drop IITREE tuple associated with a specific IIPROTECT
**	or IIINTEGRITY tuple.  Function compares tree id found in an IITREE
**	tuple against that passed by the caller and if they match, returns
**	DMR_F_RETURN indicating to QEF that the tuple must be returned;
**	otherwise DMR_F_NEXT will be returned and DMF will go on to the next
**	tuple
**	
**  Input:
**	treeid	    ptr to a treeid found in IIPROTECT or IIINTEGRITY tuple
**	tree_tuple  ptr to the tuple being considered
**	
**  Output:
**	None
**	
**  Returns:
**	DMR_F_NEXT if IDs are not equal, DMR_F_RETURN otherwise.
*/
i4
qeu_tr_ts_cmp(
DB_TREE_ID	*treeid,
DB_IITREE	*tree_tuple)
{
    return((   treeid->db_tre_high_time == tree_tuple->dbt_trid.db_tre_high_time
            && treeid->db_tre_low_time == tree_tuple->dbt_trid.db_tre_low_time
	   )
	       ? DMR_F_RETURN : DMR_F_NEXT);
}

/*
** Name: qeu_scan_perms	 - scan existing permit tuples to determine permit
**			   number for the new permit + compare existing permits
**			   with those user is trying to create to determine if
**			   we can save ourselves some of the effort
**
** Description:
**	Scan existing protection tuples for this object and determine the next
**	protection number.
**	If attempting to create a permit (as opposed to security_alarm), check
**	for existing permits which represent a superset of privileges (without
**	taking into consideration the attributes to which the privilege applies)
**	which will be specified by one of permits created as a part of this
**	statement (recall that permits specifying multiple privileges get split)
**	(i.e. object, privilege (disregarding DB_GRANT_OPTION), grantor,
**	grantee, etc.).
**	Once such tuple is found, there are 5 cases to consider:
**	    - existing permit specifies some privilege P WGO, the permit being
**	      created specifies P [WGO], both were specified using the same 
**	      statement syntax, and the attribute map associated with existing
**	      permit is a superset of the attribute map associated the permit
**	      being created - new permit should not be created;
**	      
**	    - existing permit specifies some privilege P [WGO], the permit being
**	      created specifies P without GRANT OPTION, both were specified
**	      using the same statement syntax, and the attribute map associated
**	      with existing permit is a superset of the attribute map associated
**	      the permit being created - new permit should not be created;
**	      
**	    - existing permit specifies some privilege P [WGO], the permit
**	      being created specifies P WGO, both were specified using the same
**	      statement syntax, and the attribute map associated with existing
**	      permit is a subset of the attribute map associated the permit
**	      being created - existing permit should be destroyed and new permit
**	      should be created 
**
**	    - existing permit specifies some privilege P without GRANT OPTION,
**	      the permit being created specifies P [WGO], both were specified
**	      using the same statement syntax, and the attribute map associated
**	      with existing permit is a subset of the attribute map associated
**	      the permit being created - existing permit should be destroyed and
**	      new permit should be created
**
**	    - existing permit specifies some privilege P [WGO], the permit
**	      being created specifies P [WGO], the attribute map associated with
**	      existing permit is identical to the attribute map associated the
**	      permit being created, but the permits cannot be collapsed because
**	      they were specified using different statement syntax (or for some
**	      other reason, although I am hard pressed to come up with a good
**	      one - maybe if two permits are created using CREATE/DEFINE PERMIT
**	      statement and one or both of them involve a WHERE-clause) - new
**	      permit will be depend on the same descriptor as the existing one
**
** Input:
**	qeuq_cb			QEU query CB
**	    qeuq_ct		if non-zero, permit definition involved a
**				WHERE-clause so it cannot be merged with any
**				other permits
**	qef_cb			qef session control block
**	
**	prot_qeu		QEU control block which was used to open
**				IIPROTECT and is preinitialized for reading
**				IIPROTECT tuples describing existing privileges
**				for the specified object
**				
**	ptuple			buffer into which existing IIPROTECT tuples will
**				be read
**				
**	new_ptuple		DB_PROTECTION tuple describing privileges which
**				user is attempting to create
**	    dbp_popset		map of privileges the user is trying to create
**	    
**	indep_privs		TRUE if the privileges which the user is
**				attempting to create depend on any other
**				privileges; FALSE otherwise
**				
**	ulm			pre-opened memory stream to be used for
**				allocating descriptions of permits to be removed
**				(if any) and privilege descriptors to be reused
**
**	relstat			if processing permit on a table or a view,
**				IIRELATION.relstat; otherwise 0
**
** Output:
**	permno			highest permit number found - new permit
**				tuple(s) should be numbered starting with
**				(permno + 1)
**
**	new_ptuple
**	    dbp_domset		some bits may be removed from the attribute map
**				if IIPROTECT tuples which are identical (except
**				that the column map of the existing tuple may be
**				a superset of the column map specified by the
**				caller and the privilege map of the existing
**				tuple may specify DB_GRANT_OPTION even if the
**				privilege map specified by the caller does not)
**	error		        will contain description of an error if
**				unexpected error is encountered
**
**	newperm_info		may contain descriptions of one or more permits
**				to be removed + of existing privilege
**				descriptors which may be reused
**				also may contain indications whether
**				ALL/RETR_TO_ALL bits must be turned on in
**				relstat after new permits have been created
**
** Returns:
**	E_DB_{OK,ERROR}
**
** Side effects:
**	memory may be allocated
**
** History:
**
**	08-jul-92 (andre)
**	    written for GRANT/REVOKE project
**	17-jul-92 (andre)
**	    if creating a permit granting some privilege(s) to PUBLIC and
**	    the existing permits did not convey to PUBLIC either RETRIEVE or ALL
**	    (SELECT, INSERT, DELETE, and UPDATE - I would prefer if this bit did
**	    not require or indicate REFERENCES), but with addition of new
**	    permits PUBLIC will possess RETRIEVE and/or ALL, then
**	    QEU_SET_RETR_TO_ALL and/or QEU_SET_ALL_TO_ALL will be set in
**	    newperm_info->qeu_flags to indicate to the caller to reset (negative
**	    logic) DMT_RETRIEVE_PRO and or DMT_ALL_PROT in IIRELATION.relstat
**
**	    added relstat to the interface
**	21-aug-92 (andre)
**	    qeu_scan_perms() will compute "depth" of permits created by 
**	    users on objects owned by other users.  
**
**	    "Depth" of a permit is a number associated with a permit such 
**	    that given two permits on some object X we will guarantee that 
**	    permit with depth N will not depend on permit with depth N+k 
**	    where k>=0, thus making it possible for unloaddb to recreate 
**	    permits in the correct order.
**
**	    Permits granted by object's owner will always have depth of 0;
**	    depth for permits granted by a user U who does not own the object
**	    will be computed as N+1 where N is the largest depth of a
**	    permit granting that privilege WGO which was granted to U
**	    or to PUBLIC.  
**
**	    If new_ptuple->dbp_popset specifies multiple privileges, a 
**	    single "depth" value will be computed for all permits which
**	    will be generated as the result.  While an overkill, it
**          makes the computation significantly less expensive and produces 
**	    a number that satisfies the definition of a permit "depth".
**	16-sep-92 (andre)
**	    privilege map is populated using bitwise ops so we cannot use
**	    BTnext() to determine all bits that are set; we will use bitwise AND
**	    instead
*/
static DB_STATUS
qeu_scan_perms(
QEUQ_CB		*qeuq_cb,
QEF_CB          *qef_cb,
QEU_CB		*prot_qeu,
DB_PROTECTION   *ptuple,
DB_PROTECTION   *new_ptuple,
bool		indep_privs,
QEU_NEWPERM_INFO *newperm_info,
ULM_RCB         *ulm,
i4		*permno,
i4		relstat,
i4		*error)
{
    DB_STATUS	    status = E_DB_OK, loc_stat;
    i4		    common_priv;
    i4		    i;
    bool	    not_a_perm;
    bool	    unmergeable;
    i4		    exit_loop;
    
	/*
	** TRUE if the new permit is a proper subset of the existing permit;
	** if anytime in the course of processing of a given tuple
	** new_perm_is_subset is TRUE, but the characteristic of the new tuple
	** does not fall with that of the current tuple, the two cannot be
	** merged
	*/
    bool	    new_perm_is_subset;
	/*
	** TRUE if the existing permit is a proper subset of the new permit
	** if anytime in the course of processing of a given tuple
	** cur_perm_is_subset is TRUE, but the characteristic of the current
	** tuple does not fall with that of the new tuple, the two cannot be
	** merged
	*/
    bool	    cur_perm_is_subset;

	/*
	** set to TRUE if new permit's attribute map is a subset
	** (not necessarily proper) of the current tuple's attribute map
	*/
    bool	    new_map_is_subset;

	/*
	** set to TRUE if current tuple's attribute map is a subset
	** (not necessarily proper) of the new permit's attribute map
	*/
    bool	    cur_map_is_subset;

	/*
	** TRUE if the new permit is limited to a specific terminal;
	** FALSE otherwise 
	*/
    bool	    new_term_specified;

	/*
	** TRUE if the existing permit is limited to a specific terminal;
	** FALSE otherwise
	*/
    bool	    cur_term_specified;

	/*
	** TRUE if the new permit would specified privilege(s) WGO;
	** FALSE otherwise
	*/
    bool	    new_privs_wgo;

	/*
	** TRUE if the current permit specifies privilege(s) WGO;
	** FALSE otherwise
	*/
    bool	    cur_privs_wgo;

	/*
	** TRUE if the new permit may affect setting of DMT_ALL_PROT or
	** DMT_RETRIEVE_PRO in IIRELATION.relstat; FALSE otherwise
	*/
    bool	    may_affect_all_or_retr_to_all;

	/*
	** TRUE if permit is being granted by a user on another user's
	** object; FALSE otherwise
	*/
    bool	    grantor_not_owner;
	/*
	** if it was determined that the permits being created could affect
	** setting of DMT_ALL_PROT and/or DMT_RETRIEVE_PRO in relstat, this var
	** will contain a map of existing grant-compatible privileges which have
	** already been granted to PUBLIC
	*/
    i4		    public_privs;

	/*
	** if it was determined that the permits being created could affect
	** setting of DMT_ALL_PROT and/or DMT_RETRIEVE_PRO in relstat, this
	** array will contain a map of attributes on which UPDATE has been
	** granted - public will be known to possess UPDATE if the first
	** DB_MAX_COLS+1 bits in this array are set
	*/
    i4		    updt_to_pub_attrs[DB_COL_WORDS];

    i2		    new_stmt_syntax;

    QEU_CB	    depqeu, *dep_qeu = (QEU_CB *) NULL;
    QEU_CB	    xdepqeu, *xdep_qeu = (QEU_CB *) NULL;

    newperm_info->qeu_perms_to_remove = (QEU_REM_PERMS *) NULL;
    for (i = 0; i < QEU_NUM_DESCRS; i++)
	newperm_info->qeu_descr_nums[i] = 0;
    newperm_info->qeu_flags = 0;
    
    /* 
    ** depth of permit must be set to 0 if the permit is being granted
    ** by the object's owner; otherwise it will be recomputed below
    */
    new_ptuple->dbp_depth = 0;
    grantor_not_owner = 
	MEcmp((PTR) &new_ptuple->dbp_grantor, (PTR) &new_ptuple->dbp_obown,
	    sizeof(DB_OWN_NAME)) != 0;

    /*
    ** determine whether the new permit is limited to a specific terminal
    ** (if a permit is not limited to a specific terminal,
    ** DB_PROTECTION.dbp_term will be set to all blanks)
    */
    new_term_specified =
	STskipblank((char *) &new_ptuple->dbp_term, sizeof(DB_TERM_NAME)) !=
	    (char *) NULL;

    /* determine whether the new permit would specify grantable privilege(s) */
    new_privs_wgo = (new_ptuple->dbp_popset & DB_GRANT_OPTION) != 0;

    /* remember the syntax of the statement used to specify new permit(s) */
    new_stmt_syntax = new_ptuple->dbp_flags & (DBP_SQL_PERM | DBP_GRANT_PERM);
    
    /*
    ** the first permission number should be 2. 0 and 1 are reserved
    ** for the special permits - all to all, retrieve to all.
    */
    *permno = 1;
    
    prot_qeu->qeu_getnext = QEU_REPO;

    if (!(not_a_perm = ((new_ptuple->dbp_popset & DB_ALARM) != 0)))
    {
	/*
	** remember whether the permit(s) being created can be merged with
	** existing tuples, i.e. remember whether the permit definition
	** involved a WHERE clause.
	*/
	unmergeable = (qeuq_cb->qeuq_ct != 0);

	/*
	** determine whether the new permit specifies a privilege which may
	** affect setting of DMT_ALL_PROT or DMT_RETRIEVE_PRO in
	** IIRELATION.relstat
	** NOTE: setting of DMT_ALL_PROT will not be affected by presense
	** 	 or absense of REFERENCES TO PUBLIC
	*/
	if (
		    /* must be a permit on a table/view/(index?) */
	       (   new_ptuple->dbp_obtype == DBOB_TABLE
		|| new_ptuple->dbp_obtype == DBOB_VIEW
		|| new_ptuple->dbp_obtype == DBOB_INDEX
	       )
		    /* may not be an "access permit" */
	    && ~new_ptuple->dbp_popset & (i4) DB_CAN_ACCESS
		    /* must name PUBLIC as grantee */
	    && new_ptuple->dbp_gtype == DBGR_PUBLIC
		    /*
		    **     relstat indicates that PUBLIC does not have ALL
		    ** and either relstat indicates that PUBLIC has RETR but the
		    **            new permit involves other privilege(s)
		    **	   or     relstat indicates that PUBLIC does not have
		    **		  RETR and the new permit involves at least RETR
		    */
	    && (   relstat & DMT_ALL_PROT
		&& (      ~relstat & DMT_RETRIEVE_PRO
		        && new_ptuple->dbp_popset &
			    (i4) (DB_DELETE | DB_APPEND | DB_REPLACE)
		    ||     relstat & DMT_RETRIEVE_PRO
		        && new_ptuple->dbp_popset & (i4) DB_RETRIEVE
	           )
	       )
	    && qeu_grant_compatible(new_ptuple, unmergeable, new_term_specified)
	   )
	{
	    may_affect_all_or_retr_to_all = TRUE;

	    if (new_ptuple->dbp_popset & (i4) DB_REPLACE)
	    {
		MEcopy((PTR) new_ptuple->dbp_domset,
		    sizeof(updt_to_pub_attrs), (PTR) updt_to_pub_attrs);
	    }
	    else
	    {
		MEfill(sizeof(updt_to_pub_attrs), (u_char) '\0',
		    (PTR) updt_to_pub_attrs);
	    }

	    /*
	    ** public_privs will contain table-wide privileges granted to PUBLIC
	    ** concequently, if new_ptuple->dbp_popset specifies table-wide
	    ** privileges, we will copy them into public_privs; otherwise if
	    ** new_ptuple->dbp_popset specifies column-specific UPDATE
	    ** (currently the only GRANT-compatible column-specific privilege),
	    ** we will only copy DB_REPLACE into public_privs if
	    ** new_ptuple->dbp_popset indicates that UPDATE is being granted on
	    ** the table-wide basis
	    */
	    if (   ~new_ptuple->dbp_popset & (i4) DB_REPLACE
	        || BTcount((char *) updt_to_pub_attrs, DB_MAX_COLS + 1) ==
		       DB_MAX_COLS + 1
	       )
	    {
		public_privs = new_ptuple->dbp_popset & ~((i4) DB_GRANT_OPTION);
	    }
	    else
	    {
		public_privs = 0;
	    }
	}
	else
	{
	    /*
	    ** the new permit(s) will not affect settings of DMT_ALL_PROT or
	    ** DMT_RETRIEVE_PRO in IIRELATION.relstat
	    */
	    may_affect_all_or_retr_to_all = FALSE;
	}
    }

    while (status == E_DB_OK)
    {
	QEF_DATA	    dep_qefdata, xdep_qefdata;
	DB_IIDBDEPENDS	    dep_tuple;
	DB_IIXDBDEPENDS	    xdep_tuple;
	DMR_ATTR_ENTRY	    xdep_key_array[4];
	DMR_ATTR_ENTRY	    *xdepkey_ptr_array[4];
	i4		    d_type;

	status = qeu_get(qef_cb, prot_qeu);
	if (status != E_DB_OK)
	{
	    if (prot_qeu->error.err_code != E_QE0015_NO_MORE_ROWS)
	    {
		*error = prot_qeu->error.err_code;
		break;
	    }
	    else
	    {
		status = E_DB_OK;
		break;
	    }
	}
	if (prot_qeu->qeu_getnext == QEU_REPO)
	{
	    prot_qeu->qeu_getnext = QEU_NOREPO;
	    prot_qeu->qeu_klen = 0;
	}

	if (ptuple->dbp_permno > *permno)
	    *permno = ptuple->dbp_permno;

	/*
	** if the user is attempting to create a securuty_alarm or we have
	** determined that the permit tuple(s) being created cannot be merged
	** with any existing permits, we are done with this tuple
	*/
	if (not_a_perm || unmergeable)
	    continue;

	/*
	** if this tuple represents an "access" permit or a security alarm,
	** do not try to merge it
	*/
	if (ptuple->dbp_popset & (i4) (DB_CAN_ACCESS | DB_ALARM))
	    continue;

	/*
	** if this is the "old-style" permit, do not try to merge new permit
	** with it
	*/
	if (~ptuple->dbp_flags & DBP_65_PLUS_PERM)
	    continue;

	/*
	** if the existing permit involved a WHERE-clause, do not try to merge
	** new permit with it
	*/
	if (   ptuple->dbp_treeid.db_tre_high_time
	    || ptuple->dbp_treeid.db_tre_low_time
	   )
	    continue;

	if (may_affect_all_or_retr_to_all)
	{
	    /*
	    ** determine whether the existing permit is limited to a specific
	    ** terminal
	    ** (if a permit is not limited to a specific terminal,
	    ** DB_PROTECTION.dbp_term will be set to all blanks)
	    **
	    ** Note: code outside of this IF-statement assumes that
	    ** if (may_affect_all_or_retr_to_all)
	    **	  then cur_term_specified has already been computed
	    */
	    cur_term_specified =
		STskipblank((char *) &ptuple->dbp_term, sizeof(DB_TERM_NAME)) !=
		    (char *) NULL;

	    /*
	    ** if it was determined that the new permit may affect settings of
	    ** DMT_ALL_PROT or DMT_RETRIEVE_PRO in relstat, before we proceed to
	    ** determine whether the existing permit can be merged into new
	    ** permit or whether the existing permit subsumes some of the
	    ** privileges specified in the new permit, we may need to add
	    ** information about this permit the data maintained about all
	    ** existing grant-compatible permits granting privileges to PUBLIC
	    */
	    if (   ptuple->dbp_gtype == DBGR_PUBLIC
		&& !(public_privs & ptuple->dbp_popset)
		&& qeu_grant_compatible(ptuple, (bool) FALSE,
		       cur_term_specified)
	       )
	    {
		if (ptuple->dbp_popset & ~((i4) DB_GRANT_OPTION)
		        & (i4) DB_REPLACE)
		{
		    BTor(DB_MAX_COLS + 1, (char *) ptuple->dbp_domset,
			     (char *) updt_to_pub_attrs);
		    if (BTcount((char *) updt_to_pub_attrs, DB_MAX_COLS + 1) ==
			    DB_MAX_COLS + 1
		       )
		    {
			public_privs |= (i4) DB_REPLACE;
		    }
		}
		else
		{
		    public_privs |=
			ptuple->dbp_popset & ~((i4) DB_GRANT_OPTION);
		}
	    }
	}
	
	/*
	** if this tuple does not specify any of the privileges specified
	** by the caller, we are done with this tuple
	*/
	common_priv =
	    new_ptuple->dbp_popset & ptuple->dbp_popset &
		~((i4) DB_GRANT_OPTION);

	if (!common_priv)
	    continue;

	/*
	** try to determine if the existing permit is either a proper subset
	** or a (not necesarily proper) superset of the permit being created
	*/

	/*
	** determine whether the current permit specifies grantable
	** privilege(s)
	*/
	cur_privs_wgo = (ptuple->dbp_popset & DB_GRANT_OPTION) != 0;

	/* 
	** if a user U who does not own the object is trying to grant
	** privilege(s) on it and the existing permit conveys grantable 
	** privilege to U or to PUBLIC, make sure that the "depth" of the 
	** new permit will be greater than the depth of this permit
	*/
	if (   grantor_not_owner
	    && cur_privs_wgo
	    && new_ptuple->dbp_depth < ptuple->dbp_depth + 1
	    && (   ptuple->dbp_gtype == DBGR_PUBLIC
		|| (   ptuple->dbp_gtype == DBGR_USER
		    && !MEcmp((PTR) &ptuple->dbp_owner, 
			      (PTR) &new_ptuple->dbp_grantor, 
			      sizeof(DB_OWN_NAME))
		   )
	       )
	   )
	{
	    new_ptuple->dbp_depth = ptuple->dbp_depth + 1;
	}

	if (new_privs_wgo && cur_privs_wgo)
	{
	    /* both new and existing permits specify grantable privilege(s) */
	    new_perm_is_subset = cur_perm_is_subset = FALSE;
	}
	else if (new_privs_wgo)
	{
	    cur_perm_is_subset = TRUE;
	    new_perm_is_subset = FALSE;
	}
	else if (cur_privs_wgo)
	{
	    new_perm_is_subset = TRUE;
	    cur_perm_is_subset = FALSE;
	}
	else
	{
	    /* neither permit specifies	grantable privilege(s) */
	    new_perm_is_subset = cur_perm_is_subset = FALSE;
	}

	/*
	** compare the attribute maps - if they are identical and the new permit
	** will depend on some privilege, we'll need to determine the number of
	** the privilege descriptor on which the current permit depends
	*/
	if (BTsubset((char *) new_ptuple->dbp_domset,
		(char *) ptuple->dbp_domset, BITS_IN(ptuple->dbp_domset)))
	{
	    new_map_is_subset = TRUE;
	}
	else
	{
	    new_map_is_subset = FALSE;
	}

	if (BTsubset((char *) ptuple->dbp_domset,
		(char *) new_ptuple->dbp_domset, BITS_IN(ptuple->dbp_domset)))
	{
	    cur_map_is_subset = TRUE;
	}
	else
	{
	    cur_map_is_subset = FALSE;
	}

	if (new_map_is_subset && cur_map_is_subset)
	{
	    /* two attribute maps are identical */

	    /*
	    ** if the new permit depends on some privilege, it will depend on
	    ** the same privilege descriptor as this permit, unless common_priv
	    ** includes DB_RETRIEVE (recall that SELECT privilege is represented
	    ** by 3 bits - DB_RETRIEVE | DB_TEST | DB_AGGREGATE)
	    ** SELECT privilege on views depends on the same privilege
	    ** descriptor on which the view itself depends and this dependency
	    ** is implied and is not recorded in IIDBDEPENDS
	    */
	    if (indep_privs && ~common_priv & ((i4) DB_RETRIEVE))
	    {
		i4	offset;

		/*
		** now we will look up the descriptor number on which the 
		** current permit depends.  To do it, we will first
		** fetch an IIXDBDEPENDS tuple and then do a TID join
		** into IIDBDEPENDS
		*/
		if (!xdep_qeu)
		{
		    /* 
		    ** need to open IIXDBDEPENDS and IIDBDEPENDS for reading
		    */
		    xdepqeu.qeu_type = QEUCB_CB;
		    xdepqeu.qeu_length = sizeof(QEUCB_CB);
		    xdepqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
		    xdepqeu.qeu_lk_mode = DMT_IS;
		    xdepqeu.qeu_flag = 0;
		    xdepqeu.qeu_access_mode = DMT_A_READ;
		    xdepqeu.qeu_mask = 0;
		    xdepqeu.qeu_eflag = QEF_INTERNAL;
		    xdepqeu.qeu_qual = xdepqeu.qeu_f_qual = 0;
		    xdepqeu.qeu_qarg = xdepqeu.qeu_f_qarg = 0;

		    xdepqeu.qeu_tab_id.db_tab_base = DM_B_XDEPENDS_TAB_ID;
		    xdepqeu.qeu_tab_id.db_tab_index = DM_I_XDEPENDS_TAB_ID;

		    if ((status = qeu_open(qef_cb, &xdepqeu)) != E_DB_OK)
		    {
			*error = xdepqeu.error.err_code;
			break;
		    }

		    xdep_qeu = &xdepqeu;

		    STRUCT_ASSIGN_MACRO(xdepqeu, depqeu);

		    depqeu.qeu_tab_id.db_tab_base = DM_B_DEPENDS_TAB_ID;
		    depqeu.qeu_tab_id.db_tab_index = DM_I_DEPENDS_TAB_ID;

		    if ((status = qeu_open(qef_cb, &depqeu)) != E_DB_OK)
		    {
			*error = depqeu.error.err_code;
			break;
		    }

		    dep_qeu = &depqeu;

		    xdep_qeu->qeu_count = 1;
		    xdep_qeu->qeu_output = &xdep_qefdata;
		    xdep_qeu->qeu_tup_length = 
			xdep_qefdata.dt_size = sizeof(DB_IIXDBDEPENDS);
		    xdep_qefdata.dt_next = 0;
		    xdep_qefdata.dt_data = (PTR) &xdep_tuple;

		    dep_qeu->qeu_count = 1;
		    dep_qeu->qeu_output = &dep_qefdata;
		    dep_qeu->qeu_tup_length = 
			dep_qefdata.dt_size = sizeof(DB_IIDBDEPENDS);
		    dep_qefdata.dt_next = 0;
		    dep_qefdata.dt_data = (PTR) &dep_tuple;

		    xdep_qeu->qeu_key = xdepkey_ptr_array;

		    /* will fetch IIXDBDEPENDS tuples using all 4 keys */
		    xdep_qeu->qeu_klen = 4;
		    xdep_qeu->qeu_getnext = QEU_REPO;

		    for (i = 0; i < 4; i++)
			xdepkey_ptr_array[i] = &xdep_key_array[i];
		
		    xdepkey_ptr_array[0]->attr_number = DM_1_XDEPENDS_KEY;
		    xdepkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
		    xdepkey_ptr_array[0]->attr_value = 
			(char *) &ptuple->dbp_tabid.db_tab_base;

		    xdepkey_ptr_array[1]->attr_number = DM_2_XDEPENDS_KEY;
		    xdepkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
		    xdepkey_ptr_array[1]->attr_value = 
			(char *) &ptuple->dbp_tabid.db_tab_index;

		    d_type = (i4) DB_PROT;
		    xdepkey_ptr_array[2]->attr_number = DM_3_XDEPENDS_KEY;
		    xdepkey_ptr_array[2]->attr_operator = DMR_OP_EQ;
		    xdepkey_ptr_array[2]->attr_value = (char *) &d_type;

		    xdepkey_ptr_array[3]->attr_number = DM_4_XDEPENDS_KEY;
		    xdepkey_ptr_array[3]->attr_operator = DMR_OP_EQ;
		    xdepkey_ptr_array[3]->attr_value = 
			(char *) &ptuple->dbp_permno;

		    /* will fetch IIDBDEPENDS tuples by TID */
		    dep_qeu->qeu_klen = 0;
		    dep_qeu->qeu_flag = QEU_BY_TID;
		    dep_qeu->qeu_key = (DMR_ATTR_ENTRY **) NULL;
		}
		
		if (common_priv & DB_DELETE)
		    offset = QEU_DELETE_DESCR;
		else if (common_priv & DB_APPEND)
		    offset = QEU_INSERT_DESCR;
		else if (common_priv & DB_REPLACE)
		    offset = QEU_UPDATE_DESCR;

		/* 
		** store the descriptor number only if one has not
		** been stored before 
		*/
		for (exit_loop = FALSE; 
		     newperm_info->qeu_descr_nums[offset] == 0 && !exit_loop; )
		{
		    if ((status = qeu_get(qef_cb, xdep_qeu)) != E_DB_OK)
		    {
			if (xdep_qeu->error.err_code == E_QE0015_NO_MORE_ROWS)
			{
			    /* 
			    ** strictly speaking this is very irregular - 
			    ** a permit which we think should depend on some 
			    ** privilege has no record of the dependency -
			    ** I think I should generate an error if
			    ** this happens
			    */
			    status = E_DB_OK;
			    dep_tuple.dbv_i_qid = 0;
			    break;
			}
		        *error = xdep_qeu->error.err_code;
		        break;
		    }

		    dep_qeu->qeu_tid = xdep_tuple.dbvx_tidp;

		    if ((status = qeu_get(qef_cb, dep_qeu)) != E_DB_OK)
		    {
			/* retrieving by TID - can't get no_more_rows error */
			*error = dep_qeu->error.err_code;
			break;
		    }

		    exit_loop = TRUE;
		}

		if (status != E_DB_OK)
		    break;

		/* save the descriptor number */
		newperm_info->qeu_descr_nums[offset] = dep_tuple.dbv_i_qid;
	    }
	}
	else if (new_map_is_subset)
	{
	    if (cur_perm_is_subset)
		continue;

	    new_perm_is_subset = TRUE;
	}
	else if (cur_map_is_subset)
	{
	    if (new_perm_is_subset)
		continue;

	    cur_perm_is_subset = TRUE;
	}
	else
	{
	    /*
	    ** each map contains attrribute(s) not contained in the other map,
	    ** we are done with this tuple
	    */
	    continue;
	}
	
	/*
	** I delayed checking of whether the two permits have the same grantor,
	** grantee, and were specified using the same syntax until now so we
	** would have a chance to compare the two attribute maps to determine
	** whether there already is a privilege descriptor in IIPRIV which could
	** be used for the new permit
	*/
    
	/*
	** if grantor of the existing permit is different from the grantor of
	** the new permit, permits cannot be merged
	*/
	if (MEcmp((PTR) &ptuple->dbp_grantor, (PTR) &new_ptuple->dbp_grantor,
		sizeof(ptuple->dbp_grantor)))
	    continue;

	/*
	** if grantee type or grantee name of the existing permit is different
	** from those of the new permit, permits cannot be merged
	*/
	if (   ptuple->dbp_gtype != new_ptuple->dbp_gtype
	    || MEcmp((PTR) &ptuple->dbp_owner, (PTR) &new_ptuple->dbp_owner,
	           sizeof(ptuple->dbp_owner)))
	    continue;

	/*
	** if the new permit and the existing permit were specified using
	** different syntax, they cannot be merged.
	** 
	*/
	if ((ptuple->dbp_flags & (DBP_SQL_PERM | DBP_GRANT_PERM)) !=
	        new_stmt_syntax)
	    continue;

	if (new_ptuple->dbp_pdbgn > ptuple->dbp_pdbgn)
	{
	    if (cur_perm_is_subset)
		continue;

	    new_perm_is_subset = TRUE;
	}
	else if (new_ptuple->dbp_pdbgn < ptuple->dbp_pdbgn)
	{
	    if (new_perm_is_subset)
		continue;

	    cur_perm_is_subset = TRUE;
	}

	if (new_ptuple->dbp_pdend < ptuple->dbp_pdend)
	{
	    if (cur_perm_is_subset)
		continue;

	    new_perm_is_subset = TRUE;
	}
	else if (new_ptuple->dbp_pdend > ptuple->dbp_pdend)
	{
	    if (new_perm_is_subset)
		continue;

	    cur_perm_is_subset = TRUE;
	}

	if (new_ptuple->dbp_pwbgn > ptuple->dbp_pwbgn)
	{
	    if (cur_perm_is_subset)
		continue;

	    new_perm_is_subset = TRUE;
	}
	else if (new_ptuple->dbp_pwbgn < ptuple->dbp_pwbgn)
	{
	    if (new_perm_is_subset)
		continue;

	    cur_perm_is_subset = TRUE;
	}

	if (new_ptuple->dbp_pwend < ptuple->dbp_pwend)
	{
	    if (cur_perm_is_subset)
		continue;

	    new_perm_is_subset = TRUE;
	}
	else if (new_ptuple->dbp_pwend > ptuple->dbp_pwend)
	{
	    if (new_perm_is_subset)
		continue;

	    cur_perm_is_subset = TRUE;
	}

	if (!may_affect_all_or_retr_to_all)
	{
	    /*
	    ** determine whether the existing permit is limited to a specific
	    ** terminal - if may_affect_all_or_retr_to_all is set, we have
	    ** already done it
	    */
	    cur_term_specified =
		STskipblank((char *) &ptuple->dbp_term, sizeof(DB_TERM_NAME)) !=
		    (char *) NULL;
        }

	if (new_term_specified && cur_term_specified)
	{
	    if (MEcmp((PTR) &ptuple->dbp_term, (PTR) &new_ptuple->dbp_term,
		   sizeof(ptuple->dbp_term)))
		continue;
	}
	else if (new_term_specified)
	{
	    if (cur_perm_is_subset)
		continue;

	    new_perm_is_subset = TRUE;
	}
	else if (cur_term_specified)
	{
	    if (new_perm_is_subset)
		continue;

	    cur_perm_is_subset = TRUE;
	}

	/*
	** at this point there are three possible outcomes:
	**  (1) neither permit is a subset of the other
	**  (2) new permit is a subset of the existing permit
	**  (3) existing permit is a subset of the new permit
	**
	** if (1) or (2) is true, we will avoid creating a new permit specifying
	** privilege described by common_priv;
	** otherwise we will remember to delete the existing permit
	*/
	if (cur_perm_is_subset)
	{
	    QEU_REM_PERMS   *p;

	    /*
	    ** we need to remember to delete the IIQRYTEXT tuple and IIDBDEPENDS
	    ** tuple associated with this permit
	    */

	    ulm->ulm_psize = sizeof(QEU_REM_PERMS);
	    if ((status = qec_malloc(ulm)) != E_DB_OK)
	    {
		ulm_closestream(ulm);
		*error = E_QE001E_NO_MEM;
		break;
	    }

	    p = (QEU_REM_PERMS *) ulm->ulm_pptr;

	    p->qeu_next = newperm_info->qeu_perms_to_remove;
	    p->qeu_permno = ptuple->dbp_permno;

	    newperm_info->qeu_perms_to_remove = p;
	}
	else
	{
	    /*
	    ** we will not be creating a permit specifying common_priv, so turn
	    ** off the corresponding bit in the map of privileges to be granted
	    ** by this perm; if there are no more privileges to be granted,
	    ** there is no need to continue
	    */
	    new_ptuple->dbp_popset &= ~common_priv;
	    new_ptuple->dbp_popctl &= ~common_priv;

	    if (!(new_ptuple->dbp_popset & ~((i4) DB_GRANT_OPTION)))
	    {
		/*
		** if the privilege map contain no privileges, we will not be
		** creating any new permits, so there is no reason to continue
		*/
		break;
	    }
	}
    }

    /* 
    ** we get here if we ran out of tuples or an error was encountered;
    ** if IIDBDEPENDS and IIXDBDEPENDS have been opened, close them now 
    */
    if (dep_qeu && (loc_stat = qeu_close(qef_cb, dep_qeu)) != E_DB_OK)
    {
	/* 
	** if we got here as a result of encountering an error, report
	** the original error before overwriting it
	*/
	if (status != E_DB_OK)
	{
	    (VOID) qef_error(*error, 0L, status, error, &qeuq_cb->error, 0);
	}

	*error = dep_qeu->error.err_code;

	if (loc_stat > status)
	    status = loc_stat;
    }

    if (xdep_qeu && (loc_stat = qeu_close(qef_cb, xdep_qeu)) != E_DB_OK)
    {
	/* 
	** if we got here as a result of encountering an error, report
	** the original error before overwriting it
	*/
	if (status != E_DB_OK)
	{
	    (VOID) qef_error(*error, 0L, status, error, &qeuq_cb->error, 0);
	}

	*error = xdep_qeu->error.err_code;

	if (loc_stat > status)
	    status = loc_stat;
    }

    if (status == E_DB_OK && may_affect_all_or_retr_to_all)
    {
	i4	all_privs =
		    (i4) (DB_RETRIEVE | DB_REPLACE | DB_APPEND | DB_DELETE |
                          DB_REFERENCES | DB_COPY_INTO | DB_COPY_FROM);
		    
	/*
	** determine whether we will need to reset DMT_ALL_PROT and/or
	** DMT_RETRIEVE_PRO
	*/

	if (relstat & DMT_RETRIEVE_PRO && public_privs & (i4) DB_RETRIEVE)
	{
	    newperm_info->qeu_flags |= QEU_SET_RETR_TO_ALL;
	}

	if (relstat & DMT_ALL_PROT && (public_privs & all_privs) == all_privs)
	{
	    newperm_info->qeu_flags |= QEU_SET_ALL_TO_ALL;
	}
    }
    
    return(status);
}

/*
** Name: qeu_grant_compatible - determine whether a permit tuple specifies
**			        GRANT-compatible privilege(s)
**
** Description:
**  Examine a permit tuple to determine whether it specifies GRANT-compatible
**  privilege(s).  In particular, we will verify that
**	- permit is unconditional (no WHERE clause),
**	- permit is not limited WRT time of day or days of the week,
**	- permit is not limited to a specific terminal, and
**	- either represents table-wide privilege(s) or represents a
**	  column-specific UPDATE or REFERENCES (in the future this should be
**	  extended to "UPDATE, INSERT, or REFERENCES")
**
** Input:
**	ptuple			IIPROTECT tuple
**	where_clause		TRUE if where_clause was specified
**	term_specified		TRUE if permit applies to a specific terminal
**	
** Output:
**	NONE
**
** Side effects:
**	NONE
**
** Returns:
**	TRUE		    permit represents GRANT-compatible privilege(s)
**	FALSE		    otherwise
**
** History:
**
**	17-jul-92 (andre)
**	    written
**	11-dec-92 (andre)
**	    column-specific REFERENCES permits are GRANT-compatible
*/
static bool
qeu_grant_compatible(
DB_PROTECTION	    *ptuple,
bool		    where_clause,
bool		    term_specified)
{
    if (ptuple->dbp_obtype == DBOB_DBPROC || ptuple->dbp_obtype == DBOB_EVENT
		|| ptuple->dbp_obtype == DBOB_SEQUENCE)
    {
	/*
	** permits on dbevents, sequences and dbprocs are guaranteed to be
	** GRANT-compatible since they can only be created using GRANT statement
	*/
	return(TRUE);
    }
    else
    {
	return (	    /* no WHERE clause */
		   !where_clause
			/* not limited to a specific terminal */
		&& !term_specified
			/* must be valid all day every day */
		&& ptuple->dbp_pdbgn == 0
		&& ptuple->dbp_pdend == 1440
		&& ptuple->dbp_pwbgn == 0
		&& ptuple->dbp_pwend == 6
		    /*
		    ** of the GRANT-compatible permits on tables/views (not
		    ** counting REFERENCES which will not be represented by
		    ** DMT_ALL_PROT), only UPDATE  may be column-specific
		    */
		&& (   ptuple->dbp_popset & (DB_REPLACE | DB_REFERENCES)
		    || BTcount((char *) ptuple->dbp_domset, DB_MAX_COLS + 1) ==
			   DB_MAX_COLS + 1
		   )
	       );
    }
}

/*
** Name: qeu_match_privs - scan existing permit tuples to determine whether
**			   a specified <auth id> still posseses specified 
**			   privilege(s) and/or ability to grant specified 
**			   privilege(s)
**
** Description:
**	Given a description of privilege(s) which may be possessed by an 
**	<auth id> AUTH and/or description of privilege(s) which may be 
**	granted by an <auth id> AUTH, scan existing permits and determine 
**	whether AUTH does not possess some/all of the specified privileges 
**	and/or whether AUTH does not possess ability to grant some/all 
**	of the specified privileges.
**
**	It is assumed (and will NOT be verified) that if a caller requested
**	that we check whether an <auth id> possesses privilege(s) and
**	whether an <auth id> possesses ability to grant priivlege(s), the
**	descriptors in both lists will be arranged in the same order (i.e. n-th
**	element in each list will deal with the same privilege.)
**
**	To specify a privilege PRIV and/or ability to grant privilege PRIV by
**	AUTH, an existing permit PERM must meet the following requirements:
**
**	  - if the description of the PRIV indicates that it must be
**	    GRANT-compatible, privilege conveyed by the perm must be
**	    GRANT-compatible,
**	  - if we are interested in the ability to grant a privilege PRIV,
**	    PERM must specify PRIV WGO; otherwise a permit specifying
**	    PRIV [WGO] is sufficient,
**	  - PERM must name AUTH or PUBLIC as the grantee, and
**	  -    PRIV must apply to a dbproc or a dbevent
** 	    or the set of attributes to which the privilege described by
**	       PERM applies is a superset (not necessarily proper) of those 
**	       described by PRIV
**	    or PRIV is UPDATE in which case we will replace the set of 
**	       attributes to which PRIV applies with a set difference of itself 
**	       and that in PERM
**
**	But wait, it gets more interesting...
**	
**	To determine whether an <auth id> still possesses a privilege, it is not
**	sufficient to find some permit granting that privilege to the <auth id>
**	or to PUBLIC.  Consider this example:
**
**	    U1: create table U1.T (i i);
**		grant select on U1.T to PUBLIC with grant option;
**	    U2: grant select on U1.T to PUBLIC with grant option;
**	    U1: revoke select on U1.T from public cascade;
**	    
**	If we were to claim that an <auth id> still possesses a privilege as
**	long as there is a permit granting it that privilege, we would
**	mistakenly fail to destroy the permit created by U2.
**
**	To avoid cases like this as well as possible cycles in the privilege
**	graph, we need to keep track of privileges checked so far and discard
**	those whose existence is not supported (directly or indirectly) by a
**	privilege granted by the object owner.
**
**	This can be reduced to searching for a path in a privilege graph defined
**	as follows:
**	    for a DBMS object X owned by U let G(X) be a directed graph
**	    consisting of
**	      case:
**	        (1) if X is a table or a view, then nodes labelled (U,P,C) and
**		    (U,P',C) for every legal privilege P on every column of X
**		    and nodes labelled (A,P,C) for every <auth id> A s.t. there
**		    is a permit granting P WGO on column C of X to A or to
**		    PUBLIC
**		    and nodes labelled (A,P',C) for every <auth id> A s.t. there
**		    is a permit granting P [WGO] on column C of X to A or to
**		    PUBLIC.
**	        (2) if X is a dbproc or a dbevent, then then nodes labelled
**		    (U,P) and (U,P') for every legal privilege P on X
**		    and nodes labelled (A,P) for every <auth id> A s.t. there is
**		    a permit granting P WGO on X to A or to PUBLIC,
**		    and nodes labelled (A,P') for every <auth id> A s.t. there
**		    is a permit granting P [WGO] on X to A or to PUBLIC.
**	    and
**	      case:
**	        (1) if privilege(s) apply to a table or a view X, then edges
**		    from node (A,P,C) to node (B,P,C) existing iff B granted
**		    privilege P WGO on column C of X to A or to PUBLIC
**		    and edges from node (A,P',C) to node (B,P',C) existing iff B
**		    granted privilege P [WGO] on column C of X to A or to
**		    PUBLIC.
**	        (2) if privilege(s) apply to a dbproc or a dbevent X, then edges
**	            (A,P) to node (B,P) existing iff B granted privilege P WGO
**		    on X to A or to PUBLIC
**		    and edges from node (A,P') to node (B,P') existing iff B
**		    granted privilege P [WGO] on X to A or to PUBLIC.
**
**	If X is a table or a view owned by U2, we will say that an <auth id> U1
**	possesses a privilege P WGO on a set of attributes S of X if in the
**	privilege graph G(X) described above, for every attribute s in S there
**	is a path from node (U1,P,s) to node (U2,P,s).
**
**	If X is a table or a view, we will say that an <auth id> U1 possesses a
**	privilege P on a set of attributes S of X if in the privilege graph G(X)
**	described above, for every attribute s in S there exists an edge from
**	(U1,P',s) to (U2,P',s) or there exists some other <auth id> U3 such that
**	there is an edge from (U1,P',s) to (U3,P',s) and there is a path from
**	node (U3,P,s) to node (U2,P,s).
**	
**	If X is a dbevent or a dbproc, we will say that an <auth id> U1
**	possesses a privilege P WGO on X if in the privilege graph G(X)
**	described above, there is a path from node (U1,P) to node (U2,P).
**
**	If X is a dbevent or a dbproc, we will say that an <auth id> U1
**	possesses a privilege Pon X if in the privilege graph G(X) described
**	above, there is an edge from (U1,P') to (U2,P') or there exists some
**	other <auth id> U3 such that there is an edge from (U1,P') to (U3,P')
**	and there is a path from node (U3,P) to node (U2,P).
**	
**	The privilege graph may contain cycles and in order to avoid them, we
**	will store representation of nodes already visited in a table with
**	tuples of type QEU_MATCH_PRIV_INFO.
**
**	Of course, we will not build a privilege graph described above, but
**	having reduced the problem to something we learned to deal with in
**	CS174, here is a brief outline of the algorithm we will use to determine
**	whether user possesses any of specified privileges and or ability
**	to grant any of specified privileges:
**
**        for (every description of a privilege being sought for a specified
**	       <auth id>)
**	  {
**	    insert a row (qeu_depth set to 0, qeu_offset set to offset
**	    of the description in the list, and qeu_flags set to
**	    QEU_PRIV_SOUGHT) into the temp table
**	  }
**	  for (every description of GRANT OPTION FOR privilege being sought for
**	       a specified <auth id>)
**	  {
**	    insert a row (qeu_depth set to 0, qeu_offset set to offset of the
**	    description in the list, and qeu_flags set to QEU_PRIV_WGO_SOUGHT)
**	    into the temp table
**	  }
**
**	  
**	  for (depth = 1, new_rows = TRUE;
**	       new_rows == TRUE && there are still some privileges to satisfy;
**	       depth++)
**	  {
**	    new_rows = FALSE;
**	    
**	    for (every permit defined for this object)
**	    {
**	      // we will be looking for permits that would convey privileges
**	      // that would satisfy those in row(s) added during the previous
**	      // traversal (qeu_depth==depth-1)
**	      //
**	      // processing during the first traversal (depth==1) will be
**	      // somewhat different from that during the subsequent traversals:
**	      //  - during the first traversal we will be looking for permits
**	      //    naming as grantee the <auth id> specified by the caller or
**	      //    PUBLIC, while during the subsequent traversals we will only
**	      //    consider permits granted to <auth id>s of type USER and try
**	      //    to key into the temp table based on <auth id> (among other
**	      //    things) to determine whether the permit specifies applicable
**	      //    privilege; we don't need to check the temp table to
**	      //    determine whether a permit would would be of interest during
**	      //    the first traversal because we know full well the <auth id>
**	      //    in the rows that were added during the previous traversal
**	      //    (they are the rows we loaded into the table to get the
**	      //    algorithm going), but during subsequent traversals, the rows
**	      //    added during the previous traversal may contain many
**	      //    different <auth id>s; I claim that permits naming PUBLIC as
**	      //    the grantee need only be considered during the first
**	      //    traversal because they would not convey any additional
**	      //    privileges during the subsequent traversals, i.e. if they
**	      //    didn't provide sufficient privileges to the grantee
**	      //    specified by the caller, there is no way for them to
**	      //    provide those privileges during subsequent traversals
**	      //  - after the first traversal we will only consider permits
**	      //    specifying privileges WGO;  this is true because during the
**	      //    first traversal we may be able to find a permit created by
**	      //    the object owner and grating some privilege to the specified
**	      //    <auth id> or to PUBLIC which would satisfy some privilege
**	      //    being sought by the caller, but during subsequent traversals
**	      //    we will be looking for privileges possessed to other users
**	      //    which would enable them to grant soem privilege to the
**	      //    <auth id> specified by the caller, so permits which do not
**	      //    specify grantable proivileges will clearly not be of any use
**
**	      if (privilege specified by the permit is not of interest to us)
**	        skip this permit
**
**	      if (   depth > 1
**	          && (   grantee type specified by the permit is not USER
**		      || permit does not specify a grantable privilege
**		     )
**		 )
**	      {
**		skip this permit
**	      }
**
**	      if (grantee type is GROUP or ROLE)
**	        skip this permit
**
**	      if (   depth == 1 &&
**	             grantee type is not PUBLIC &&
**		     grantee name != <auth id> suppled by the caller
**	         )
**	      {
**	        skip this permit
**	      }
**
**	      Let P refer to the current permit tuple
**	      
**	      // during the first traversal, we will be scanning the temp table
**	      // based on qeu_priv (== dbp_priv & ~DB_GRANT_OPTION) and
**	      // qeu_depth (== 0);
**	      // during subsequent traversals we will add
**	      // qeu_gtype (== DBGR_USER) and qeu_authid (== dbp_owner) to the
**	      // keys used for access
**
**	      scan the temp table and for every row satisfying the specified
**	      keys
**	      {
**		Let R refer to the current temp table row
**		
**		if (   (   P specifies grantable privilege
**		        || R was added to check for privilege (and not GRANT
**			   OPTION FOR privilege)
**		       )
**		    && (   P specifies privilege on dbproc
**		        || P specifies privilege on dbevent
**			|| P specifies some privilege on a superset
**		           of attributes of a table listed in R
**			|| P specifies UPDATE on some of the attributes
**			   of a table listed in R
**		       )
**		   )
**		{
**	          if (P was granted by the owner of the object)
**		  {
**		    if (temp table row was added to check for privilege)
**		    {
**		      update appropriate element of privs_to_check list and,
**		      if appropriate, update map of privileges being sought;
**		    }
**		    else
**		    {
**		      update appropriate element of grant_option_to_check list
**		      and, if appropriate, update map of privileges GRANT
**		      OPTION FOR which is being sought
**		    }
**		      
**		    if (we have verified that user possesses all privileges
**		        and GRANT OPTION FOR all privileges specified by the
**			caller)
**		    {
**		      we are done
**		    }
**		  }
**		  else
**		  {
**		    prepare to insert a new temp table row R' constructed as
**		    follows:
**		      R'.qeu_priv = R.qeu_priv
**		      R'.qeu_depth = depth
**		      R'.qeu_gtype = DBGR_USER
**		      R'.qeu_authid = P.dbp_grantor
**		      R'.offset = R.offset
**		      R'.qeu_flags = R.qeu_flags
**		      R'.qeu_attrmap = R.qeu_attrmap INTERSECT P.dbp_popset
**
**		    if (   R' describes a privilege on a dbproc or a dbevent
**			|| R' describes a privilege other than UPDATE
**		       )
**		    {
**		      if (there exists no temp table row R" that
**			  matches R' on all fields with likely exception of
**			  qeu_depth
**		       )
**		      {
**		        insert R' into the temp table
**		      }
**		    }
**		    else    // R' describes UPDATE
**		    {
**		      for (every temp table row R" which matches R' on
**		           all fields with possible exception of qeu_attrmap
**			   and likely exception of qeu_depth)
**		      {
**		        replace R'.qeu_attrmap with set difference of itself and
**			R".qeu_attrmap
**		      }
**
**		      if (R'.qeu_attrmap is not empty)
**		      {
**		        insert R' into the temp table
**		      }
**		    }
**		  }
**		}
**	      }
**	    }
**	  }
**
** Input:
**	qeuq_cb			QEU query CB
**	    qeuq_db_id		database id
**	qef_cb			qef session control block
**	
**	prot_qeu		QEU control block which was used to open
**				IIPROTECT and is preinitialized for reading
**				IIPROTECT tuples describing existing privileges
**				for the specified object
**				
**	privs_to_check 		array of QEU_REVOKE_INFO structures
**				describing privileges which may be possessed 
**				by a specified <auth id>;
**				may be set to NULL if the caller is not 
**				interested in determining whether a given 
**				<auth id> possesses privilege(s)
**	    qeu_priv		privilege map
**	    qeu_flags		additional info about the privilege described
**		QEU_GRANT_COMPATIBLE_ONLY
**				privilege must be GRANT-compatible
**	    qeu_gtype		grantee type
**	    qeu_authid		grantee name (unless qeu_gtype == DBGR_PUBLIC)
**	    qeu_attrmap 	map of attributes to which privilege
**				described by qeu_attrmap applied
**
**	grant_option_to_check	array of QEU_REVOKE_INFO structures
**				describing privileges which may be granted 
**				by a specified <auth id>;
**				may be set to NULL if the caller is not
**				interested in determining whether a given
**                              <auth id> is able to grant some privilege(s)
**	    qeu_priv		privilege map
**	    qeu_flags		additional info about the privilege described
**		QEU_GRANT_COMPATIBLE_ONLY
**				privilege must be GRANT-compatible
**	    qeu_gtype		grantee type
**	    qeu_authid		grantee name (unless qeu_gtype == DBGR_PUBLIC)
**	    qeu_attrmap 	map of attributes to which privilege
**				described by qeu_attrmap applied
**				
**	perm_depth_info		array of QEU_PERM_DEPTH_INFO structures
**				corresponding to elements of 
**				grant_option_to_check; should be non-NULL iff
**				grant_option_to_check is non-NULL AND
**				pdi_tbl_descr is non-NULL 
**
**				Each entry will describe a privilege for which
**				the caller wants us to find aggregate depth
**				(defined in the history comment for 16-jul-93)
**
**				may be NULL if the caller is not asking that we
**				recompute depths of privileges affected by 
**				revocation of specified privileges; typically 
**				this would happen when the caller is certain 
**				that no depths need to be recomputed)
**				
**	list_size		number of elements in privs_to_check and/or 
**				grant_option_to_check lists of privilege 
**				descriptions
**
**	pdi_tbl_descr		description of the temp table which will
**				be used to collect descriptions of privileges 
**				whose aggregate depth has increased as a result
**				of revocation of specified privileges; 
**				will be NULL iff perm_depth_info is NULL
**	ptuple			buffer into which existing IIPROTECT tuples will
**				be read
**
**	obj_owner		name of the object owner;
**				may be set to NULL if the caller is certain that
**				the name of the object owner is not the same as
**				privs_to_check.qeu_authid or
**				grant_option_to_check.qeu_authid
**				
** Output:
**
**	privs_to_check		if not NULL, elements of the list will
**				describe which of specified privileges the 
**				specified <auth_id> does not possess
**	    qeu_priv		if non-zero, will indicate the privilege
**				that a specified <auth id> does not possess 
**	    qeu_attrmap		if qeu_priv is non-zero, will describe 
**				attributes on which privilege described in 
**				qeu_priv is not longer possessed by the 
**				specified <auth id>
**      grant_option_to_check   if not NULL, elements of the list will
**                              describe which of specified privileges the
**                              specified <auth_id> may not grant
**          qeu_priv            if non-zero, will indicate the privilege
**                              that a specified <auth id> may not grant
**          qeu_attrmap         if qeu_priv is non-zero, will describe
**                              attributes on which privilege described in
**                              qeu_priv may not be granted by the
**                              specified <auth id>
**	error		        will contain description of an error if
**				unexpected error is encountered
**
** Returns:
**	E_DB_{OK,ERROR}
**
** Side effects:
**	memory may be allocated
**
** History:
**
**	23-jul-92 (andre)
**	    written for GRANT/REVOKE project
**	06-aug-92 (andre)
**	    changed the interface to accept a list of descriptions of
**	    privileges for which it is required to determine whether a
**	    specified <auth id> possesses specified privilege(s) and/or 
**	    GRANT OPTION FOR specified privilege(s)
**	18-aug-92 (andre)
**	    added code to verify that a permit granting a privilege to the
**	    specified <auth id> is not itself abandoned (see more in the
**	    description of the algorithm above)
**	11-dec-92 (andre)
**	    added code to check whether a user possesses adequate REFERENCES
**	    privilege
**	10-jun-93 (andre)
**	    A user may lose a privilege on his/her own object only if that
**	    object is a view or dbproc and he/she lost a privilege on that
**	    view's or dbproc's underlying object.
**	    In other words, if some user issued a query to revoke some privilege
**	    on an object X from the X's owner (or from some other user who - for
**	    whatever reason - chose to grant privilege to the X's owner), X's
**	    owner is guaranteed to lose no privileges on X as a result.
**
**	    Added a new parameter, obj_owner, containing (when != NULL) the name
**	    of the owner of the object on which some privilege(s) need to be
**	    checked.  If the grantee from whom privileges are being revoked is
**	    the owner of the object, we can safely claim that he/she will not
**	    lose any privileges as a result of revoking specified privilege(s)
**	    granted to him by some user
**	16-jul-93 (andre)
**	    if we are asked to determine whether a specified grantee G still
**	    possesses a grantable privilege PRIV, the caller is also interested
**	    in knowing whether depths of permits dependent on that privilege may
**	    need to be recomputed.
**
**	    For that purpose, I added a new parameter - perm_depth_info - to the
**	    interface of qeu_match_privs().  perm_depth_info is an array (of
**	    size "list_size") of QEU_PERM_DEPTH_INFO structures describing
**	    privilege which may have been lost as a result of destruction (or
**	    alteration) of some permits along with aggregate depth of permits
**	    which were destroyed (or altered)
**
**	    Aggregate depth of a privilege conveyed by one or more permits which
**	    were DESTROYED OR ALTERED is defined as the lowest depth of any
**	    such permit for a given privilege (e.g. INSERT)
**
**	    If it turns out that privilege which was conveyed by
**	    destroyed/altered permits is also conveyed by some other permits
**	    which are still intact, this function will be expected to compute
**	    aggregate depth of a given privilege conveyed by these remaining
**	    permits.
**
**	    Aggregate depth of a privilege conveyed by EXISTING permits will be
**	    computed as follows:
**		- if no permits are found, then it is undefined
**		- if a privilege can be granted only on object-wide basis
**		  (e.g. EXECUTE, RAISE), we just look for a lowest-depth permit
**		  granted to the specified grantee or to PUBLIC 
**		- if a privilege can be granted on a per column basis 
**		  (so far it means UPDATE or REFERENCES), privilege on different
**		  columns may depend on permits with different depths; in that 
**		  case for each column we will keep track of the permit with 
**		  lowest depth on which privilege on that column depends; we 
**		  will derive the aggregate depth by finding the highest depth 
**		  value among all columns 
**
**	    In order to compute aggregate depth of a privilege PRIV conveyed by
**	    existing permits, we will collect information about depths of
**	    existing permits granting PRIV to a given grantee G or to PUBLIC
**	    during the first iteration of the outermost loop (i.e. when
**	    depth==1) only.  After we finish processing all tuples in the temp
**	    table, we will compute aggregate depth of a privilege conveyed to G
**	    or to PUBLIC by existing permits and store it into qeu_new_depth
**	    field of the appropriate element of perm_depth_info[].
**	25-jul-93 (andre)
**	    Added pdi_tbl_descr to the interface.  If the caller
**	    requested that we recompute depth of grantable privilege(s) and 
**	    the new aggregate depth for some privilege(s) is greater than
**	    the old aggregate depth, description(s) of these privilege(s) 
**	    will be added to the temp table which will eventually be
**	    handed to qeu_depth() to recompute depths of permits affected by
**	    revocation of specified privilege(s)
**	7-Apr-2004 (schka24)
**	    Don't leave junk in attr default fields.
*/
static DB_STATUS
qeu_match_privs(
QEUQ_CB			*qeuq_cb,
QEF_CB          	*qef_cb,
QEU_CB			*prot_qeu,
QEU_REVOKE_INFO   	*privs_to_check,
QEU_REVOKE_INFO   	*grant_option_to_check,
QEU_PERM_DEPTH_INFO     *perm_depth_info,
i4			list_size,
QEU_PDI_TBL_DESCR	*pdi_tbl_descr,
DB_PROTECTION   	*ptuple,
DB_OWN_NAME		*obj_owner,
i4			*error)
{
    DB_STATUS	    	status = E_DB_OK, local_status;
    GLOBALREF QEF_S_CB	*Qef_s_cb;
    DB_OWN_NAME	    	*auth_id;
    i2		    	auth_type;
    i4		    	priv_map, grant_priv_map;
    i4		    	i, j;
    i4			grant_compatible;
    QEU_REVOKE_INFO	*info_p;
    QEU_PERM_DEPTH_INFO	*depth_info;
    bool		temp_created = FALSE;
    QEU_CB		tmp1qeu, *tmp1_qeu = (QEU_CB *) NULL;
    QEU_CB		tmp2qeu, *tmp2_qeu = (QEU_CB *) NULL;
    QEF_DATA            in_tmp1_qefdata, out_tmp1_qefdata;
    QEF_DATA		tmp2_qefdata;
    DB_LOC_NAME         temp_loc_name;
    QEU_MATCH_PRIV_INFO	info1, *read_tmp1_row = &info1;
    QEU_MATCH_PRIV_INFO info2, *write_tmp1_row = &info2;
    QEU_MATCH_PRIV_INFO info3, *tmp2_row = &info3;
#ifndef	    TEMP_TABLES_WORK
    QEU_MATCH_PRIV_INFO	info4;
#endif
    DMT_ATTR_ENTRY      temp_array[QEU_MATCH_PRIV_TBL_DEGREE];
    DMT_ATTR_ENTRY 	*temp_ptr_array[QEU_MATCH_PRIV_TBL_DEGREE];
    DMR_ATTR_ENTRY	tmp1_key_array[4], tmp2_key_array[1];
    DMR_ATTR_ENTRY	*tmp1key_ptr_array[4], *tmp2key_ptr_array[1];
    DMT_CB		dmt_cb;
    i4			temp_priv;
    i2			tmp1_depth;
    i2			tmp1_gtype;
    DB_OWN_NAME		tmp1_authid;
    ULM_RCB		ulm;
    i4			depth;
    i4			tuples_added;
    QEU_DEPTH_LIST	**depth_list, *depth_p;
    i4			exit_loop, exit_loop2;
    
    if (list_size == 0)
	return(E_DB_OK);

    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
    ulm.ulm_streamid = (PTR)NULL;
    
    if (grant_option_to_check)
    {
	auth_id = &grant_option_to_check->qeu_authid;
	auth_type = grant_option_to_check->qeu_gtype;
	grant_compatible = 
	    grant_option_to_check->qeu_flags & QEU_GRANT_COMPATIBLE_ONLY;
    }
    else if (privs_to_check)
    {
	auth_id = &privs_to_check->qeu_authid;
	auth_type = privs_to_check->qeu_gtype;
	grant_compatible = 
	    privs_to_check->qeu_flags & QEU_GRANT_COMPATIBLE_ONLY;
    }
    else
    {
	/* this should never happen */
	return(E_DB_OK);
    }

    /*
    ** if we were asked whether the object owner will still possess some 
    ** privileges after these privileges have been revoked from him by some 
    ** user.  The answer is a resounding YES, so we just zero out maps of 
    ** privileges and return
    */
    if (   auth_type == DBGR_USER
	&& obj_owner
	&& !MEcmp((PTR) auth_id, (PTR) obj_owner, sizeof(*obj_owner)))
    {
        if (info_p = privs_to_check)
	{
            for (i = 0; i < list_size; i++, info_p++)
	    {
		info_p->qeu_priv = 0;
	    }
	}

        if (   (info_p = grant_option_to_check)
	    && (depth_info = perm_depth_info))
	{
            for (i = 0; i < list_size; i++, info_p++, depth_info++)
	    {
		/* 
		** explicit revocation of privileges from the object's
		** owner will have no effect on the privileges possessed by
		** him/her
		*/
		info_p->qeu_priv = 0;

		/* permits granted by object owner always have depth 0 */
		depth_info->qeu_new_depth = 0;
	    }
 	}

	return(E_DB_OK);
    }

    for (exit_loop = FALSE; !exit_loop; )
    {
        /* 
	** create the temporary table which will be used to keep track
	** of the <auth id>s for which privileges have been checked
	*/
    
        /* initialize array of attribute pointers for the temporary table */
        for (i = 0; i < QEU_MATCH_PRIV_TBL_DEGREE; i++)
	    temp_ptr_array[i] = temp_array + i;
    
	temp_array[0].attr_type = DB_INT_TYPE;
	temp_array[0].attr_size = 4;
	temp_array[0].attr_precision = 0;
	temp_array[0].attr_flags = 0;
	temp_array[0].attr_collID = -1;
	SET_CANON_DEF_ID(temp_array[0].attr_defaultID, DB_DEF_NOT_DEFAULT);
	temp_array[0].attr_default = NULL;

	for (i = 1; i < QEU_MATCH_PRIV_TBL_DEGREE; i++)
	    STRUCT_ASSIGN_MACRO(temp_array[0], temp_array[i]);

	/* not all attributes are i4's + we need to assign them names */

	/* first attribute - privilege map */
	MEmove(sizeof("priv") - 1, "priv", ' ',
	    DB_MAXNAME, (char *) &temp_array[0].attr_name);

	/* 
	** second attribute - number of traversal during which a row was
	** added - also referred to as depth
	*/
	temp_array[1].attr_size = 2;
	MEmove(sizeof("depth") - 1, "depth", ' ',
	    DB_MAXNAME, (char *) &temp_array[1].attr_name);

	/* third attribute - grantee type */
	temp_array[2].attr_size = 2;
	MEmove(sizeof("grantee_type") - 1, "grantee_type", ' ',
	    DB_MAXNAME, (char *) &temp_array[2].attr_name);

	/* fourth attribute - auth id that is expected to possess a privilege */
	temp_array[3].attr_type = DB_CHA_TYPE;
	temp_array[3].attr_size = sizeof(DB_OWN_NAME);
	MEmove(sizeof("auth_id") - 1, "auth_id", ' ',
	    DB_MAXNAME, (char *) &temp_array[3].attr_name);

	/* 
	** fifth atribute - offset into the list supplied by the caller
	** to which this row pertains
	*/
	temp_array[4].attr_size = 2;
	MEmove(sizeof("offset") - 1, "offset", ' ',
	    DB_MAXNAME, (char *) &temp_array[4].attr_name);

	/* sixth attribute -  informational flags */
	temp_array[5].attr_size = 2;
	MEmove(sizeof("flags") - 1, "flags", ' ',
	    DB_MAXNAME, (char *) &temp_array[5].attr_name);

	/* attributes 7-16 constitute an attribute map */
	{
	    char	c[sizeof("attmrapa") - 1];
	    char	*p = c + (i4) (sizeof("attmrapa") - 2);

	    MEmove((sizeof("attmrapa") - 1), "attrmapa", ' ',
		(sizeof("attmrapa") - 1), c);
	    (*p)--;

	    for (i = 6; i < QEU_MATCH_PRIV_TBL_DEGREE; i++)
	    {
		(*p)++;
		MEmove((sizeof("attmrapa") - 1), c, ' ',
		    DB_MAXNAME, (char *) &temp_array[i].attr_name);
	    }
	}

	dmt_cb.length = sizeof(DMT_CB);
	dmt_cb.type = DMT_TABLE_CB;
	dmt_cb.dmt_flags_mask = DMT_DBMS_REQUEST;
        dmt_cb.dmt_db_id = qeuq_cb->qeuq_db_id;
	dmt_cb.dmt_char_array.data_address = 0;
	dmt_cb.dmt_char_array.data_in_size = 0;
	dmt_cb.dmt_attr_array.ptr_address = (PTR) temp_ptr_array;
	dmt_cb.dmt_attr_array.ptr_in_count = QEU_MATCH_PRIV_TBL_DEGREE;
	dmt_cb.dmt_attr_array.ptr_size = sizeof (DMT_ATTR_ENTRY);
	dmt_cb.dmt_tran_id = qef_cb->qef_dmt_id;	    
	MEmove(sizeof("$default") - 1, "$default", ' ', 
	    sizeof(DB_LOC_NAME), (char *) &temp_loc_name);
	dmt_cb.dmt_location.data_address = (PTR) &temp_loc_name;
	dmt_cb.dmt_location.data_in_size = sizeof(DB_LOC_NAME);

	status = dmf_call(DMT_CREATE_TEMP, &dmt_cb);
	if (status != E_DB_OK)
	{
	    if (dmt_cb.error.err_code == E_DM0078_TABLE_EXISTS)
	    {
		*error = E_QE0050_TEMP_TABLE_EXISTS;
		status = E_DB_SEVERE;
	    }
	    else
	    {
		*error = dmt_cb.error.err_code;
	    }
	    break;
	}
	temp_created = TRUE;

#ifdef TEMP_TABLES_WORK
	/*
	** now we modify the temporary table to BTREE on privilege, depth,
	** grantee type and grantee_name
	*/
	{
	    DMU_CB	    dmucb, *dmu_cb = &dmucb;
	    DMU_KEY_ENTRY   *key_ptr_array[4];
	    DMU_KEY_ENTRY   key_array[4];
	    DMU_CHAR_ENTRY  dmu_chars[2];

	    dmu_cb->type			= DMU_UTILITY_CB;
	    dmu_cb->length			= sizeof(DMU_CB);
	    dmu_cb->dmu_flags_mask		= 0;
	    dmu_cb->dmu_db_id			= qeuq_cb->qeuq_db_id;
	    dmu_cb->dmu_tran_id			= qef_cb->qef_dmt_id;
	    STRUCT_ASSIGN_MACRO(dmt_cb.dmt_id, dmu_cb->dmu_tbl_id);
	    dmu_cb->dmu_location.data_address	= NULL;
	    dmu_cb->dmu_location.data_in_size	= 0;
	    dmu_cb->dmu_key_array.ptr_address	= (PTR) key_ptr_array;
	    dmu_cb->dmu_key_array.ptr_size      = sizeof(DMU_KEY_ENTRY);
	    dmu_cb->dmu_key_array.ptr_in_count  = 4;
	    for (i = 0; i < 4; i++)
	    {
		key_ptr_array[i] = key_array + i;
		STRUCT_ASSIGN_MACRO(temp_array[i].attr_name,
		    key_ptr_array[i]->key_attr_name);
		key_ptr_array[i]->key_order = DMU_ASCENDING;
	    }
	    
	    dmu_cb->dmu_char_array.data_address = (PTR) dmu_chars;
	    dmu_cb->dmu_char_array.data_in_size = sizeof(dmu_chars);
	    dmu_chars[0].char_id		= DMU_STRUCTURE;
	    dmu_chars[0].char_value		= DB_BTRE_STORE;
	    dmu_chars[1].char_id                = DMU_TEMP_TABLE;
	    dmu_chars[1].char_value             = DMU_C_ON;

	    status = dmf_call(DMU_MODIFY_TABLE, dmu_cb);
	    if (status != E_DB_OK)
	    {
		*error = dmu_cb->error.err_code;
		break;
	    }
	}
#endif

	/*
	** if we were asked to compute aggregate privilege depth of privilege(s)
	** dependent on grantable privilege(s) conveyed by permits which were 
	** dropped or altered, we need to allocate an array of QEU_DEPTH_LIST 
	** structures which will be used for that purpose
	*/
	if (perm_depth_info)
	{
	    for (exit_loop2 = FALSE; !exit_loop2; )
	    {
		/* Open stream and allocate list in one action */
		ulm.ulm_flags = ULM_PRIVATE_STREAM | ULM_OPEN_AND_PALLOC;
		ulm.ulm_psize = list_size * sizeof(QEU_DEPTH_LIST *);

                if ((status = qec_mopen(&ulm)) != E_DB_OK)
                    break;
    
                depth_list = (QEU_DEPTH_LIST **) ulm.ulm_pptr;

		/*
		** since QEU_DEPTH_LIST includes a variable length array, we 
		** need to determine number of elements of that array before we
		** know how much memory to request
		*/

		for (i = 0, depth_info = perm_depth_info; 
		     i < list_size; 
		     i++, depth_info++)
		{
		    i4		num_col_entries;
		    i2		*col_depth;

		    if (depth_info->qeu_priv & (DB_REPLACE | DB_REFERENCES))
		    {
			/*
			** these privileges can be granted on per column basis -
			** we will have to keep track of depths of permits 
			** granting privilege on a particular column
			*/

			/* 
			** figure out how many entries will be required to keep
			** track of depths of privilege on individual columns +
			** the entry for the table-wide privilege 
			*/
			for (num_col_entries = 0, j = -1;
			     (j = BTnext(j, (char *) depth_info->qeu_attrmap,
				DB_MAX_COLS + 1)) != -1;
			     num_col_entries = j + 1)
			;
		    }
		    else
		    {
			/* 
			** privilege can be granted only on per object basis - 
			** no need to keep track of depths on per-column basis
			*/
			num_col_entries = 0;
		    }

		    ulm.ulm_psize = sizeof(QEU_DEPTH_LIST) -
			sizeof(depth_list[i]->qeu_col_depths) +
			num_col_entries * 
			    sizeof(depth_list[i]->qeu_col_depths[0]);
		    
		    if ((status = qec_malloc(&ulm)) != E_DB_OK)
		    {
			ulm_closestream(&ulm);
			break;
		    }

		    depth_p = depth_list[i] = (QEU_DEPTH_LIST *) ulm.ulm_pptr;

		    depth_p->qeu_cur_aggr_depth = QEU_AGGR_DEPTH_UNKNOWN;
		    depth_p->qeu_col_depth_list_len = num_col_entries;

		    for (j = 0, col_depth = depth_p->qeu_col_depths; 
			 j < num_col_entries; 
			 j++, col_depth++)
		    {
			*col_depth = 
			    BTtest(j, (char *) depth_info->qeu_attrmap)
			        ? QEU_DEPTH_UNKNOWN : QEU_DEPTH_IRRELEVANT;
		    }
		}

		exit_loop2 = TRUE;
	    }

            if (status != E_DB_OK)
            {
                *error = E_QE001E_NO_MEM;
                break;
            }
	}

	/* 
	** now open the temporary table; get an exclusive lock on it to
	** save a few page-level locks
	*/
	tmp1qeu.qeu_type = QEUCB_CB;
        tmp1qeu.qeu_length = sizeof(QEUCB_CB);
        tmp1qeu.qeu_db_id = qeuq_cb->qeuq_db_id;
        tmp1qeu.qeu_lk_mode = DMT_X;
        tmp1qeu.qeu_flag = DMT_U_DIRECT;
        tmp1qeu.qeu_access_mode = DMT_A_WRITE;
	tmp1qeu.qeu_mask = 0;
#ifdef	TEMP_TABLES_WORK
	tmp1qeu.qeu_qual = 0;
	tmp1qeu.qeu_qarg = 0;
#else
	tmp1qeu.qeu_qual = ( PTR ) qeu_mpt1_qual;
	tmp1qeu.qeu_qarg = (PTR) &info4;
#endif
	tmp1qeu.qeu_f_qual = 0;
	tmp1qeu.qeu_f_qarg = 0;
        tmp1qeu.qeu_tab_id.db_tab_base  = dmt_cb.dmt_id.db_tab_base;
        tmp1qeu.qeu_tab_id.db_tab_index = dmt_cb.dmt_id.db_tab_index;
	STRUCT_ASSIGN_MACRO(tmp1qeu, tmp2qeu);
	status = qeu_open(qef_cb, &tmp1qeu);
	if (status != E_DB_OK)
	{
	    *error = tmp1qeu.error.err_code;
	    break;
	}
	tmp1_qeu = &tmp1qeu;

	tmp2qeu.qeu_qual = ( PTR ) qeu_mpt2_qual;
	tmp2qeu.qeu_qarg = (PTR) write_tmp1_row;
	status = qeu_open(qef_cb, &tmp2qeu);
	if (status != E_DB_OK)
	{
	    *error = tmp2qeu.error.err_code;
	    break;
	}
	tmp2_qeu = &tmp2qeu;

	tmp1_qeu->qeu_count = 1;
	tmp1_qeu->qeu_tup_length = sizeof(QEU_MATCH_PRIV_INFO);

	tmp1_qeu->qeu_output = &out_tmp1_qefdata;
	out_tmp1_qefdata.dt_next = 0;
	out_tmp1_qefdata.dt_size = sizeof(QEU_MATCH_PRIV_INFO);
	out_tmp1_qefdata.dt_data = (PTR) read_tmp1_row;

	tmp1_qeu->qeu_input = &in_tmp1_qefdata;
	in_tmp1_qefdata.dt_next = 0;
	in_tmp1_qefdata.dt_size = sizeof(QEU_MATCH_PRIV_INFO);
	in_tmp1_qefdata.dt_data = (PTR) write_tmp1_row;

	tmp2_qeu->qeu_count = 1;
	tmp2_qeu->qeu_tup_length = sizeof(QEU_MATCH_PRIV_INFO);

	tmp2_qeu->qeu_input = (QEF_DATA *) NULL;
	
	tmp2_qeu->qeu_output = &tmp2_qefdata;
	tmp2_qefdata.dt_next = 0;
	tmp2_qefdata.dt_size = sizeof(QEU_MATCH_PRIV_INFO);
	tmp2_qefdata.dt_data = (PTR) tmp2_row;

#ifdef	    TEMP_TABLES_WORK
	tmp1_qeu->qeu_key = tmp1key_ptr_array;

	tmp1key_ptr_array[0] = tmp1_key_array;
	tmp1key_ptr_array[0]->attr_number   = 1;
	tmp1key_ptr_array[0]->attr_operator = DMR_OP_EQ;
	tmp1key_ptr_array[0]->attr_value    = (char *) &temp_priv;

	tmp1key_ptr_array[1] = tmp1_key_array + 1;
	tmp1key_ptr_array[1]->attr_number   = 2;
	tmp1key_ptr_array[1]->attr_operator = DMR_OP_EQ;
	tmp1key_ptr_array[1]->attr_value    = (char *) &tmp1_depth;

	tmp1key_ptr_array[2] = tmp1_key_array + 2;
	tmp1key_ptr_array[2]->attr_number   = 3;
	tmp1key_ptr_array[2]->attr_operator = DMR_OP_EQ;
	tmp1key_ptr_array[2]->attr_value    = (char *) &tmp1_gtype;

	tmp1key_ptr_array[3] = tmp1_key_array + 3;
	tmp1key_ptr_array[3]->attr_number   = 4;
	tmp1key_ptr_array[3]->attr_operator = DMR_OP_EQ;
	tmp1key_ptr_array[3]->attr_value    = (char *) &tmp1_authid;

	tmp2_qeu->qeu_key = tmp2key_ptr_array;

	tmp2key_ptr_array[0] = tmp2_key_array;
	tmp2key_ptr_array[0]->attr_number   = 1;
	tmp2key_ptr_array[0]->attr_operator = DMR_OP_EQ;
	tmp2key_ptr_array[0]->attr_value    = (char *) &temp_priv;
#else
	tmp1_qeu->qeu_key = tmp2_qeu->qeu_key = (DMR_ATTR_ENTRY **) NULL;
#endif

	/*
	** next we will load initial rows into the temp table to get the whole
	** thing going;
	*/
	
	/* first initialize a few static attributes */
	write_tmp1_row->qeu_depth = 0;
	write_tmp1_row->qeu_gtype = auth_type;
	STRUCT_ASSIGN_MACRO((*auth_id), write_tmp1_row->qeu_authid);

        /* 
        ** if asked to check whether a specified <auth id> possesses some 
        ** privilege(s), compute the map of these privileges; at the
	** same time, add rows to the temp table describing privileges
	** which are being sought
        */
        priv_map = 0;
        if (info_p = privs_to_check)
	{
	    write_tmp1_row->qeu_flags = QEU_PRIV_SOUGHT;

            for (i = 0; i < list_size; i++, info_p++)
	    {
		if (info_p->qeu_priv)
		{
		    priv_map |= info_p->qeu_priv;

		    write_tmp1_row->qeu_priv = info_p->qeu_priv;
		    write_tmp1_row->qeu_offset = i;
		    for (j = 0; j < DB_COL_WORDS; j++)
			write_tmp1_row->qeu_attrmap[j] = info_p->qeu_attrmap[j];
		    
		    status = qeu_append(qef_cb, tmp1_qeu);
		    if (status != E_DB_OK)
		    {
			*error = tmp1_qeu->error.err_code;
			break;
		    }
		}
	    }
	    if (status != E_DB_OK)
		break;
	}

        /* 
        ** if asked to check whether a specified <auth id> possesses 
        ** GRANT OPTION FOR some privilege(s), compute the map of these 
	** privileges
        */
        grant_priv_map = 0;
        if (info_p = grant_option_to_check)
	{
	    write_tmp1_row->qeu_flags = QEU_PRIV_WGO_SOUGHT;

            for (i = 0; i < list_size; i++, info_p++)
	    {
		if (info_p->qeu_priv)
		{
	            grant_priv_map |= info_p->qeu_priv;

		    write_tmp1_row->qeu_priv = info_p->qeu_priv;
		    write_tmp1_row->qeu_offset = i;
		    for (j = 0; j < DB_COL_WORDS; j++)
		    	write_tmp1_row->qeu_attrmap[j] = info_p->qeu_attrmap[j];

		    status = qeu_append(qef_cb, tmp1_qeu);
		    if (status != E_DB_OK)
		    {
			*error = tmp1_qeu->error.err_code;
			break;
		    }
		}
	    }
	    if (status != E_DB_OK)
		break;
 	}

#ifdef	TEMP_TABLES_WORK
	/*
	** during the first traversal, we will be keying into the temp table by
	** privilege and depth alone, but after that we will only consider
	** permits granted to USERs
	*/
	tmp1_gtype = DBGR_USER;
#else
	info4.qeu_gtype = DBGR_USER;
#endif
	
	for (depth = 1, tuples_added = TRUE;
	     (priv_map || grant_priv_map) && tuples_added;
	     depth++
	    )
	{
	    /*
	    ** assume that no new tuples will be added during this traversal -
	    ** reset to TRUE if we do
	    */
	    tuples_added = FALSE;

#ifdef	TEMP_TABLES_WORK
	    /*
	    ** we will limit our search to temp table tuples which were 
	    ** added during the last traversal
	    */
	    tmp1_depth = depth - 1;
#else
	    info4.qeu_depth = depth - 1;
#endif
	
	    prot_qeu->qeu_count = 1;
	    prot_qeu->qeu_getnext = QEU_REPO;
	    prot_qeu->qeu_klen = 2;

	    while (status == E_DB_OK)
	    {
		status = qeu_get(qef_cb, prot_qeu);
		if (status != E_DB_OK)
		{
		    if (prot_qeu->error.err_code != E_QE0015_NO_MORE_ROWS)
		    {
			*error = prot_qeu->error.err_code;
		    }
		    else
		    {
			status = E_DB_OK;
		    }
		    break;
		}

		if (prot_qeu->qeu_getnext == QEU_REPO)
		{
		    prot_qeu->qeu_getnext = QEU_NOREPO;
		    prot_qeu->qeu_klen = 0;
		}

		/*
		** if this tuple represents an "access" permit or a 
		** security alarm, do not consider it
		*/
		if (ptuple->dbp_popset & (i4) (DB_CAN_ACCESS | DB_ALARM))
		    continue;

		/*
		** if this is the "old-style" permit, do not consider it
		*/
		if (~ptuple->dbp_flags & DBP_65_PLUS_PERM)
		    continue;

		/* 
		** if the privilege specified by the caller was GRANT-compatible
		** and the current permit is not, do not consider it
		*/
		if (   grant_compatible
		    && !qeu_grant_compatible(ptuple,
			(bool) (   ptuple->dbp_treeid.db_tre_high_time
				|| ptuple->dbp_treeid.db_tre_low_time),
			(bool) (STskipblank((char *) &ptuple->dbp_term, 
				    sizeof(DB_TERM_NAME)) != (char *) NULL))
		   )
		{
		    continue;
		}

		/*
		** if the privilege specified in this permit is not in priv_map
		** and either is not in grant_priv_map or the permit does not
		** specify a privilege WITH GRANT OPTION, skip this tuple
		*/
		if (   !(priv_map & ptuple->dbp_popset)
		    && (   !(grant_priv_map & ptuple->dbp_popset)
			|| ~ptuple->dbp_popset & DB_GRANT_OPTION
		       )
		   )
		{
		    continue;
		}

		/*
		** after the first traversal, we are only interested in permits
		** specifying grantable privilege (dbp_popset & DB_GRANT_OPTION)
		** granted to USERs
		*/
		if (   depth > 1
		    && (   ptuple->dbp_gtype != DBGR_USER
			|| ~ptuple->dbp_popset & DB_GRANT_OPTION
		       )
		   )
		{
		    continue;
		}

		/*
		** during the first traversal, if grantee specified by this
		** tuple is different from the <auth id> which was specified by
		** the caller and it is not PUBLIC, do not consider it
		*/
		if (   depth == 1
		    && ptuple->dbp_gtype != DBGR_PUBLIC
		    && (   ptuple->dbp_gtype != auth_type
			|| MEcmp((PTR) &ptuple->dbp_owner, (PTR) auth_id,
				sizeof(ptuple->dbp_owner))
		       )
		   )
		{
		    continue;
		}

		/* 
		** if a permit was granted by a user to himself, it is of
		** no use to us
		*/
		if (   ptuple->dbp_gtype == DBGR_USER
		    && !MEcmp((PTR) &ptuple->dbp_owner, 
			      (PTR) &ptuple->dbp_obown,
			      sizeof(ptuple->dbp_owner))
		   )
		{
		    continue;
		}

		/*
		** scan the rows which were added to the temp table during the
		** previous traversal and check if this permit applies to any
		** of them; during the first traversal, we scan by privilege and
		** depth; in subsequent traversals we scan by privilege, depth,
		** grantee type and grantee name
		*/
		tmp1_qeu->qeu_getnext = QEU_REPO;
#ifdef	TEMP_TABLES_WORK
		/*
		** turn off DB_GRANT_OPTION bit since GRANT OPTION FOR privilege
		** is being sought, we would not get this far unless the permit
		** specified a grantable privilege;
		** turn off DB_TEST and DB_AGGREGATE bits because they are
		** represented by DB_RETRIEVE
		*/
		temp_priv = ptuple->dbp_popset & ~((i4) DB_GRANT_OPTION);
		if (temp_priv & DB_RETRIEVE)
		    temp_priv &= ~((i4) (DB_TEST | DB_AGGREGATE));

		if (depth == 1)
		{
		    tmp1_qeu->qeu_klen = 2;
		}
		else
		{
		    tmp1_qeu->qeu_klen = 4;
		    STRUCT_ASSIGN_MACRO(ptuple->dbp_owner, tmp1_authid);
		}
#else
		info4.qeu_priv = ptuple->dbp_popset & ~((i4) DB_GRANT_OPTION);
		if (info4.qeu_priv & DB_RETRIEVE)
		    info4.qeu_priv &= ~((i4) (DB_TEST | DB_AGGREGATE));
		    
		if (depth > 1)
		    STRUCT_ASSIGN_MACRO(ptuple->dbp_owner, info4.qeu_authid);
		/*
		** position for all; qualification function will get us the
		** right tuples
		*/
		tmp1_qeu->qeu_klen = 0;
#endif

		tmp1_qeu->qeu_count = 1;
		
		for (;;)
		{
		    status = qeu_get(qef_cb, tmp1_qeu);
		    if (status != E_DB_OK)
		    {
			if (tmp1_qeu->error.err_code == E_QE0015_NO_MORE_ROWS)
			{
			    /* ran out of rows */
			    status = E_DB_OK;
			}
			else
			{
			    *error = tmp1_qeu->error.err_code;
			}
			break;
		    }

		    if (tmp1_qeu->qeu_getnext == QEU_REPO)
		    {
			tmp1_qeu->qeu_getnext = QEU_NOREPO;
			tmp1_qeu->qeu_klen = 0;
		    }

		    /*
		    ** determine whether this row will be affected by the
		    ** current permit and skip to the next temp table row if it
		    ** won't
		    */
		    if (   ~ptuple->dbp_popset & DB_GRANT_OPTION
			&& read_tmp1_row->qeu_flags & QEU_PRIV_WGO_SOUGHT)
		    {
			continue;
		    }

		    if (   ptuple->dbp_obtype == DBOB_TABLE
			|| ptuple->dbp_obtype == DBOB_VIEW)
		    {
			i4	    applies;
			
			/*
			** if the privilege specified in the permit is
			** UPDATE or REFERENCES, then the attribute maps
			** in the temp table row and in the current permit
			** must have non-empty intersection; for other
			** privileges the attribute map in the permit must
			** be a superset of that in the temp table row
			** 
			** we will need a map of attributes to determine
			** whether a new row needs to added to the temp
			** table, so to save a few cycles, we'll store the
			** intersection of the attribute maps in the
			** write_tmp1_row
			*/
			if (ptuple->dbp_popset & (DB_REPLACE | DB_REFERENCES))
			{
			    applies = FALSE;	    /* assume the worst */
			    
			    for (i = 0; i < DB_COL_WORDS; i++)
			    {
				if (write_tmp1_row->qeu_attrmap[i] =
					read_tmp1_row->qeu_attrmap[i] &
					    ptuple->dbp_domset[i]
				   )
				{
				    applies = TRUE;
				}
			    }
			}
			else
			{
			    /* here it is easier to be an optimist */
			    applies = TRUE;
			    
			    for (i = 0; i < DB_COL_WORDS; i++)
			    {
				if (read_tmp1_row->qeu_attrmap[i] &
					~ptuple->dbp_domset[i])
				{
				    applies = FALSE;
				    break;
				}
				else
				{
				    write_tmp1_row->qeu_attrmap[i] =
					read_tmp1_row->qeu_attrmap[i];
				}
			    }
			}

			if (!applies)
			    continue;
		    }

		    if (!MEcmp((PTR) &ptuple->dbp_grantor,
			       (PTR) &ptuple->dbp_obown,
			       sizeof(DB_OWN_NAME))
		       )
		    {
			i4	*map;
			i4	satisfied = TRUE;
			
			if (read_tmp1_row->qeu_flags & QEU_PRIV_SOUGHT)
			{
			    map = &priv_map;
			    info_p = privs_to_check + read_tmp1_row->qeu_offset;
			}
			else
			{
			    map = &grant_priv_map;
			    info_p = grant_option_to_check +
				read_tmp1_row->qeu_offset;
			}

			/* 
			** note that for dbprocs and dbevents and for
			** table privileges other than UPDATE or REFERENCES
			** we don't need to check the attribute map
			*/
			if (ptuple->dbp_popset & (DB_REPLACE | DB_REFERENCES))
			{
			    for (i = 0; i < DB_COL_WORDS; i++)
			    {
			        if (info_p->qeu_attrmap[i] &=
				        ~ptuple->dbp_domset[i])
			        {
			            satisfied = FALSE;
			        }
			    }
			}

			if (satisfied)
			{
			    *map &= ~ptuple->dbp_popset;
			    info_p->qeu_priv &= ~ptuple->dbp_popset;

			    /*
			    ** if 
			    **   - we were asked to compute depth of a privilege
			    **     and 
			    **   - we are examining permits granting the 
			    **     privilege to the specified grantee or PUBLIC
			    **     (first iteration of the outermost loop), and
			    **   - the current temp table row indicates that a 
			    **     privilege must be grantable 
			    ** simply reset the aggregate depth for the
			    ** privilege to 0 and avoid resetting depths of
			    ** privilege for individual columns
			    */
			    if (   perm_depth_info
				&& depth == 1
				&& read_tmp1_row->qeu_flags & 
				       QEU_PRIV_WGO_SOUGHT
			       )
			    {
				depth_list[read_tmp1_row->qeu_offset]->
				    qeu_cur_aggr_depth = 0;
			    }

			    if (!priv_map && !grant_priv_map)
			        break;
			}
		    }
		    else
		    {
			i4	    found = FALSE;

			/*
			** we may need to insert a new row into the temp table
			** unless it turns out that we have inserted an
			** identical row before; to prepare for such
			** eventuality, build the row now (besides,
			** qeu_mpt2_qual() will get it from dmr_get(), so we had
			** beter do it here)
			*/
			write_tmp1_row->qeu_priv   = read_tmp1_row->qeu_priv;
			write_tmp1_row->qeu_depth  = depth;
			write_tmp1_row->qeu_gtype  = DBGR_USER;
			write_tmp1_row->qeu_offset = read_tmp1_row->qeu_offset;
			write_tmp1_row->qeu_flags  = read_tmp1_row->qeu_flags;
			STRUCT_ASSIGN_MACRO(ptuple->dbp_grantor, 
					    write_tmp1_row->qeu_authid);
			/* 
			** now scan the temporary table to determine
			** whether the new row needs to be added
			*/

		        tmp2_qeu->qeu_count = 1;
		        tmp2_qeu->qeu_getnext = QEU_REPO;
#ifdef	TEMP_TABLES_WORK
		        tmp2_qeu->qeu_klen = 1;
#else
			/*
			** position for all - qual function will get us the
			** right rows
			*/
			tmp2_qeu->qeu_klen = 0;
#endif

			for (;;)
			{
			    status = qeu_get(qef_cb, tmp2_qeu);
			    if (status != E_DB_OK)
			    {
				if (tmp2_qeu->error.err_code ==
				        E_QE0015_NO_MORE_ROWS)
				{
				    status = E_DB_OK;
				}
				else
				{
				    *error = tmp2_qeu->error.err_code;
				}
				break;
			    }

			    if (tmp2_qeu->qeu_getnext == QEU_REPO)
			    {
				tmp2_qeu->qeu_getnext = QEU_NOREPO;
				tmp2_qeu->qeu_klen = 0;
			    }

			    /*
			    ** the qualification function, qeu_mpt2_qual() has
			    ** already verified that the tuple satisfies our
			    ** requirements except for the case when we are
			    ** dealing with UPDATE or REFERENCES privilege,
			    ** in which case we did not insist that the
			    ** qeu_attrmap in the existing tuple must be a
			    ** superset of write_tmp1_row->qeu_attrmap;
			    ** if it turns out that it is, we are done;
			    ** otherwise, we can turn off some bits in
			    ** write_tmp1_row->qeu_attrmap and look for more
			    ** existing tuples to work with
			    */

			    /* 
			    ** assume that we won't have to insert this row
			    */
			    found = TRUE;

			    if (   write_tmp1_row->qeu_priv == DB_REPLACE
				|| write_tmp1_row->qeu_priv == DB_REFERENCES)
			    {
				/*
				** when dealing with UPDATE or REFERENCES, we
				** will not insist that the attribute map in an
				** existing row be a superset of a row we
				** are considering for insertion - instead
				** we would be satisfied if a union of maps
				** of existing rows were a superset of the
				** attribute map in write_tmp1_row->qeu_attrmap
				*/
				for (i = 0; i < DB_COL_WORDS; i++)
				{
				    if (write_tmp1_row->qeu_attrmap[i] &=
					~tmp2_row->qeu_attrmap[i])
				    {
					found = FALSE;
				    }
				}

				if (found)
				    break;
			    }
			}
			if (status != E_DB_OK)
			    break;

			if (!found)
			{
			    /* 
			    ** insert new row into the temp table;
			    ** this row represents the fact that if the
			    ** user whose ID is in write_tmp1_row->qeu_authid
			    ** possesses GRANT OPTION FOR privilege found in
			    ** write_tmp1_row->qeu_priv on attributes in
			    ** write_tmp1_row->qeu_attrmap, then the
			    ** current user possesses that privilege (if
			    ** qeu_flags & QEU_PRIV_SOUGHT) or GRANT OPTION
			    ** FOR that privilege (if qeu_flags &
			    ** QEU_GRANT_OPTION_SOUGHT)
			    */
		    	    tmp1_qeu->qeu_count = 1;
			    status = qeu_append(qef_cb, tmp1_qeu);
			    if (status != E_DB_OK)
			    {
				*error = tmp1_qeu->error.err_code;
				break;
			    }

			    /* 
			    ** remember that at least one tuple was
			    ** added during this traversal 
			    */
			    tuples_added = TRUE;
			}
		    }

		    /*
		    ** if 
		    **   - we were asked to compute depth of a privilege and 
		    **   - we are examining permits granting the privilege to 
		    **     the specified grantee or PUBLIC (first iteration of 
		    **	   the outermost loop), and
		    **   - the current temp table row indicates that a 
		    **     privilege must be grantable 
		    ** we need to determine whether the current permit will have
		    ** any effect on the aggregate depth of the privilege
		    */
		    if (   perm_depth_info
			&& depth == 1
			&& read_tmp1_row->qeu_flags & QEU_PRIV_WGO_SOUGHT
		       )
		    {
			depth_p = depth_list[read_tmp1_row->qeu_offset];

			/*
			** if 
			**   - we are yet to come up with a value for the 
			**     aggregate depth (because depth of privileges
			**     on some of the columns is not known) or
			**   - the depth of the permit is lower than the
			**     aggregate depth computed so far
			** privilege described by the permit may enable
			** us to get a more favorable estimate of the
			** aggregate depth of the privilege (or, in case of
			** privileges which may be granted on per-column
			** basis, at least determine depth of privilege on some 
			** new columns
			*/
			if (   depth_p->qeu_cur_aggr_depth ==
			           QEU_AGGR_DEPTH_UNKNOWN
			    || depth_p->qeu_cur_aggr_depth > ptuple->dbp_depth
			   )
			{
			    /*
			    ** the case where sufficient privilege was granted
			    ** by the object owner has been handled above
			    **
			    ** If the privilege can only be granted on
			    ** per-object basis, simply reset aggregate
			    ** depth; otherwise (privilege in question is
			    ** UPDATE or REFERENCES) update privileeg depth
			    ** for columns foir which is was unknown or
			    ** greater than the depth of the current permit
			    ** and recompute the current aggregate depth if
			    ** possible
			    */

			    if (ptuple->dbp_popset & 
				    (DB_REPLACE | DB_REFERENCES))
			    {
						/* so call me an optimist */
				i4     aggr_depth = 0;  
				i2	*col_depth;

				for (i = 0, col_depth = depth_p->qeu_col_depths;
				     i < depth_p->qeu_col_depth_list_len;
				     i++, col_depth++
				    )
				{
				    if (*col_depth == QEU_DEPTH_IRRELEVANT)
					continue;
				    
				    if (   (   *col_depth == QEU_DEPTH_UNKNOWN
					    || *col_depth > ptuple->dbp_depth)
					&& BTtest(i, (char *)ptuple->dbp_domset)
				       )
				    {
					*col_depth = ptuple->dbp_depth;
				    }
				    
				    if (aggr_depth != QEU_AGGR_DEPTH_UNKNOWN)
				    {
					if (*col_depth == QEU_DEPTH_UNKNOWN)
					{
					    aggr_depth = QEU_AGGR_DEPTH_UNKNOWN;
					}
					else if (*col_depth > aggr_depth)
					{
					    aggr_depth = *col_depth;
					}
				    }
				}
				
				depth_p->qeu_cur_aggr_depth = aggr_depth;
			    }
			    else
			    {
				depth_p->qeu_cur_aggr_depth = ptuple->dbp_depth;
			    }
			}
		    }
		}
		
		/*
		** if an error occurred or we are done, avoid looking
		** at more permits
		*/
		if (status != E_DB_OK || !priv_map && !grant_priv_map)
		    break;
	    }
	    if (status != E_DB_OK)
		break;
	}

	exit_loop = TRUE;
    }

    /*
    ** if we were asked to determine grantable privilege depth and no errors 
    ** have been encountered so far, we need to propagate information from
    ** structures hanging off elements of depth_list into elements of
    ** perm_depth_info; 
    **
    ** if a given privilege was determined to NOT be abandoned, we simply need 
    ** to store depth_list[i]->qeu_cur_aggr_depth into 
    ** perm_depth_info[i].qeu_new_depth; if, on the other hand, a privilege 
    ** (UPDATE or REFERENCES) was found to be abandoned only on some of the 
    ** columns specified in perm_depth_info[i].qeu_attrmap, we need to reset
    ** perm_depth_info[i].qeu_attrmap to describe only those attributes on
    ** which the privilege have not been abandoned + compute
    ** perm_depth_info[i].qeu_new_depth as a number equal to the largest 
    ** depth associated with any of the columns on which the privilege has 
    ** not been abandoned
    */
    if (status == E_DB_OK && perm_depth_info && pdi_tbl_descr)
    {
	for (i = 0,
	     info_p = grant_option_to_check,
	     depth_info = perm_depth_info;

	     i < list_size;

	     i++, info_p++, depth_info++
	    )
	{
	    depth_p = depth_list[i];

	    if (!info_p->qeu_priv)
	    {
		/* 
		** privilege has not been abandoned - we will use the
		** aggregate depth value stored in *depth_p to determine the
		** new depth for the privilege
		*/
		depth_info->qeu_new_depth = depth_p->qeu_cur_aggr_depth;
	    }
	    else if (info_p->qeu_priv & (DB_REPLACE | DB_REFERENCES))
	    {
		/*
		** if the privilege can be granted on per-column basis,
		** update depth_info->qeu_attrmap[] to represent only those
		** columns on which the privilege has not been lost and
		** recompute depth of privilege on those columns based on
		** depths of privilege on each of the columns
		*/

		bool		empty_map = TRUE;
		i4		aggr_depth = 0;

		for (j = 0; j < DB_COL_WORDS; j++)
		{
		    if (depth_info->qeu_attrmap[j] &= ~info_p->qeu_attrmap[j])
			empty_map = FALSE;
		}

		if (empty_map)
		{
		    /* 
		    ** this privilege is abandoned - we will not try to
		    ** recompute depth of permits dependent on it
		    */
		    continue;
		}
		else
		{
		    for (j = -1; 
		         (j = BTnext(j, (char *) depth_info->qeu_attrmap,
		                     DB_MAX_COLS + 1)) != -1;
		        )
		    {
		        if (depth_p->qeu_col_depths[j] > aggr_depth)
			    aggr_depth = depth_p->qeu_col_depths[j];
		    }
			
		    depth_info->qeu_new_depth = aggr_depth;
		}
	    }

	    /*
	    ** if the new aggregate depth is greater than the old one,
	    ** insert this row into the temp table used to keep track of
	    ** privileges whose aggregate depth has increased as a result of
	    ** revocation of specified privileges
	    */
	    if (depth_info->qeu_new_depth > depth_info->qeu_old_depth)
	    {
		QEU_PERM_DEPTH_INFO	*pdi_row;
		
		pdi_row = &pdi_tbl_descr->qeu_pdi_write_row;

		STRUCT_ASSIGN_MACRO((*depth_info), (*pdi_row));

		/* 
		** it makes out life easier if the privilege map does not have
		** DB_GRANT_OPTION bit set - a tuple would not be inserted into
		** the temp table if the privilege described by it did not 
		** specify a grantable privilege
		**
		** also, turn off DB_TEST and DB_AGGREGATE bits if the permit
		** specify SELECT privilege
		*/
		pdi_row->qeu_priv &= ~((i4) DB_GRANT_OPTION);
		if (pdi_row->qeu_priv & DB_RETRIEVE)
		    pdi_row->qeu_priv = DB_RETRIEVE;

		pdi_tbl_descr->qeu_pdi_qeucb.qeu_count = 1;

		status = qeu_append(qef_cb, &pdi_tbl_descr->qeu_pdi_qeucb);
		if (status != E_DB_OK)
		{
		    *error = pdi_tbl_descr->qeu_pdi_qeucb.error.err_code;
		    break;
		}

		/* remember that another tuple has been successfully inserted */
		pdi_tbl_descr->qeu_pdi_num_rows++;
	    }
	}
    }

    /* we get here if we are done or if something went wrong */

    /* close temporary table if it was opened */
    if (tmp1_qeu)
    {
	local_status = qeu_close(qef_cb, tmp1_qeu);
	if (local_status != E_DB_OK)
	{
	    if (status != E_DB_OK)
	    {
		/*
		** if we got here as a result of an error, display that error
		** before overwriting it
		*/
		(VOID) qef_error(*error, 0L, local_status, error,
		    &qeuq_cb->error, 0);
	    }
	    *error = tmp1_qeu->error.err_code;
	    if (local_status > status)
		status = local_status;
	}
    }

    /* close temporary table if it was opened for the second time */
    if (tmp2_qeu)
    {
	local_status = qeu_close(qef_cb, tmp2_qeu);
	if (local_status != E_DB_OK)
	{
	    if (status != E_DB_OK)
	    {
		/*
		** if we got here as a result of an error, display that error
		** before overwriting it
		*/
		(VOID) qef_error(*error, 0L, local_status, error,
		    &qeuq_cb->error, 0);
	    }
	    *error = tmp1_qeu->error.err_code;
	    if (local_status > status)
		status = local_status;
	}
    }

    /* if temp table was created, destroy it now */
    if (temp_created)
    {
	dmt_cb.dmt_flags_mask = 0;
	local_status = dmf_call(DMT_DELETE_TEMP, &dmt_cb);
	if (local_status != E_DB_OK)
	{
	    if (status != E_DB_OK)
	    {
		(VOID) qef_error(*error, 0L, local_status, error,
		    &qeuq_cb->error, 0);
	    }
	    *error = dmt_cb.error.err_code;
	    if (local_status > status)
		status = local_status;
	}
    }	    

    if (ulm.ulm_streamid != (PTR)NULL)
    {
        /*
        ** Close the input stream created for structures used to compute depth 
	** of privileges
        */

        (VOID) ulm_closestream(&ulm);
    }

    return(status);
}

/*
** Name: qeu_priv_revoke - attempt to revoke specified privileges
**
** Description:
**	This function can be called in conjunction with processing REVOKE
**	(qeuq_permit_mask & QEU_REVOKE_PRIVILEGES) or DROP PERMIT if it was
**	determined that some privilege on which other permits or objects may
**	depend will be lost as a result of destruction of the specified permit.
**	
**	Setting of QEU_DROP_CASCADE in qeuq_flag_mask will indicate that any
**	objects rendered abandoned by the destruction of the specified privilege
**	are to be destroyed as well.
**
**	Caller will be able to specify that a privilege, ability to grant a
**	privilege, or both are being lost.  To indicate that a privilege is
**	being lost, privs_lost->dbp_popset must have the privilege bit set.
**	To indicate that ability to grant a privilege is being lost,
**	grant_option_lost->dbp_popset must have the privilege bit set.
**
**	This function expects that the caller destroyed or altered permit(s)
**	conveying specified privilege and/or ability to grant specified
**	privilege to the specified grantee before calling this function.
**	To a large degree this is dictated by requirements of DROP PERMIT where,
**	for example, a user may be losing UPDATE on a set of columns even though
**	there is a permit specifying UPDATE on that set of columns, except that
**	it was not GRANT-compatible, in which case it would be erroneous to have
**	qeu_priv_revoke() behave as if we were told to revoke the UPDATE
**	privilege.
**	
**	The algorithm will be as follows:
**
**        create a temporary table which will contain a list of descriptions
**	  of privileges which were lost and insert row(s) describing
**	  privilege and/or ability to grant a privilege which were specified by
**	  the caller into it to get the algorithm going (NOTE that the caller 
**	  will NEVER specify privileges lost by the object's owner since even
**	  though one CAN issue a REVOKE statement revoking some privilege(s)
**	  from the object's owner, it will have no real effect and
**	  qeu_match_privs() will ensure that qeu_revoke_privs() is never told
**	  that the owner of an object lost some privilege(s) on that object);
**
**	  scan the temporary table without repositioning and for every row
**	  {
**	    if (current row indicates that GRANT OPTION FOR a privilege was 
**		lost)
**	    {
**	      if (ability to grant privilege P on object X was lost by a user U)
**	      { 
**		// case where a user loses GRANT OPTION FOR EXECUTE on his
**		// dbproc will be handled below where we handle loss of
**		// EXECUTE privilege by the dbproc owner along with loss of
**		// GRANT OPTION FOR EXECUTE
**		if (the current row does NOT describe that
**		    GRANT OPTION FOR EXECUTE was lost by the owner of a dbproc)
**		{
**	          look for permits on X granted by U, or, if the temp table
**		  record indicates that the privilege was lost by the object's 
**		  owner, consider all permits, specifying P [WGO] + if
**		  X is a table or a view, intersection of the attribute map
**		  associated with a permit and of that associated with privilege
**		  ability to grant which is being lost must be non-empty.
**
**		  for each such permit
**		  {
**	            if (<drop behaviour> is RESTRICT)
**	            {
**	              report to the user that he may not perform desired 
**		      operation because it would render some object or permit 
**		      abandoned
**	            }
**	            else
**	            {
**		      if (the privilege associated with the permit is a subset 
**			  of the privilege which the grantor of the permit may 
**			  no longer grant (a mouthful, indeed))
**		      {
**		        destroy that permit;
**		      }
**		      else // the permit's grantor no longer possesses ability
**			   // to grant some (but not all) privilege described by
**			   // the permit
**		      {
**		        modify IIPROTECT and IIQRYTEXT tuples associated with 
**			the permit so it no longer conveys the privilege which 
**			the grantor may no longer grant 
**		      }
**
**		      if (the current row does NOT indicate that the privilege
**			  was lost by the object's owner)
**		      {
**		        determine which privilege and/or ability to grant which
**		        privilege, if any, would be lost to the grantee of the
**		        permit and insert their description(s) into the 
**			temporary table
**		      }
**		    }
**		  }
**	      
**	          if (the current row indicates that a privilege was lost by the
**	  	      object's owner)
**	          {
**	  	    if (the current row contains an indication 
**		        (qeu_flags & QEU_INVALIDATE_QPS) that QPs which could 
**			depend on loss of this privilege should be invalidated)
**		    {
**		      do so now
**
**		      // note that because we postponed processing loss of
**		      // GRANT OPTION FOR EXECUTE on a dbproc by its owner, 
**		      // we know that the object on which its owner lost 
**		      // GRANT OPTION FOR <privilege> is a view
**		      if (we found id of a base table used in the view's
**			  definition and altered its timestamp)
**		      {
**		        reset QEU_INVALIDATE_QPS bit in read_temp_row->qeu_flags
**		        because we want to avoid trying to force invalidation 
**			of QPs dependent on views and dbprocs whose
**			definitions involve the current view
**		      }
**		    }
**		
**		    append a record to the temp table indicating that PUBLIC 
**		    lost the <privilege> and GRANT OPTION FOR <privilege> 
**		    specified by the current record (this will ensure that
**		    during a subsequent pass we will pick up all objects owned
**		    by other users which depend on the privilege which the
**		    owner of the object may no longer grant)
**	          }
**	        }
**	      }
**	      else // current row indicates that PUBLIC lost ability to grant
**	           // some privilege (since groups and roles may not be
**		   // recepients of privileges WGO)
**	      {
**	        look for permits on X granted by users other than its owner and
**		specifying P [WGO] + if X is a table or a view, intersection of
**		the attribute map associated with PERM and of that associated
**		with privilege being lost must be non-empty.
**
**		for each such permit
**		{
**		  if (the grantor of the permit no longer possesses ability
**		      to grant at least some privileges described by the permit)
**		  {
**	            if (<drop behaviour> is RESTRICT)
**	            {
**	              report to the user that he may not perform desired
**		      operation because it would render some object or permit
**		      abandoned
**	            }
**		    else
**		    {
**		      if (the privilege associated with the permit is a subset
**		          of the privilege which the grantor may no longer 
**			  grant)
**		      {
**		        destroy that permit;
**		      }
**		      else // the permit's grantor no longer possesses ability
**			   // to grant some (but not all) privilege described
**			   // by the permit
**		      {
**		        modify IIPROTECT and IIQRYTEXT tuples associated with
**			the permit so it no longer conveys the privilege which
**			will be lost
**		      }
**
**		      determine which privilege and/or ability to grant which
**		      privilege, if any, would be lost to the grantee of the
**		      permit and insert their description(s) into the 
**		      temporary table describing privileges that will be 
**		      revoked
**		    }
**		  }
**		}
**	      }
**	    }
**	    
**	    if (privilege P or ability to grant privilege P on 
**	        object X was lost by a user AUTH)
**	    {
**
**	      if (AUTH lost EXECUTE or GRANT OPTION FOR EXECUTE on 
**		  his/her dbproc)
**	      {
**	        objects (i.e. dbprocs) that will be affected by this loss must
**		be found in IIDBDEPENDS;
**
**		if (the dbproc is marked GRANTABLE and GRANT OPTION FOR EXECUTE
**		    is being revoked)
**		{
**		  if (<drop behaviour> is RESTRICT)
**		  {
**		    if (there is at least one permit defined on the dbproc)
**		      return an error
**		  }
**		  else
**		  {
**		    // with dbprocs we can take a shortcut of wiping out all
**		    // permits because there is only one dbproc privilege - 
**		    // EXECUTE - and it is always object-wide
**		    destroy all permits defined on the dbproc without checking
**		    for abandoned objects;
**
**		    add a record to the temporary table that PUBLIC lost
**		    EXECUTE and GRANT OPTION FOR EXECUTE on this dbproc;
**		    this will ensure that during the next pass we will pick up
**		    all dbprocs owned by other users which may depend on this
**		    dbproc
**		  }
**
**		  remember to reset DB_DBPGRANT_OK bit in IIPROCEDURE.db_mask[0]
**		}
**
**		if (the dbproc is ACTIVE and EXECUTE was lost)
**		{
**                if (<drop behaviour> is RESTRICT)
**		  {
**		    return error
**		  }
**		  else
**		  {
**		    remember to reset DB_ACTIVE_DBP bit in 
**		    IIPROCEDURE.db_mask[0]
**		  }
**		}
**
**		if (no bits need to be reset in IIPROCEDURE.db_mask[0])
**	        {
**		  we are done with this temp table row since the status of the
**		  dbproc has not changed
**		}
**		else
**		{
**		  update IIPROCEDURE tuple accordingly
**
**		  if (the current row indicates that QPs which could depend
**		      on this dbproc should be invalidated)
**		  {
**		    do so now
**
**		    // do not bother resetting QEU_INVALIDATE_QPS in qeu_flags
**		    // because we unconditionally set that bit in a record
**		    // created to indicate that owner of a dbproc lost 
**		    // [GRANT OPTION FOR] EXECUTE on his dbproc as a result 
**		    // of losing [GRANT OPTION FOR] EXECUTE on some dbproc 
**		    // used in its definition
**		  }
**		}
**
**		scan IIDBDEPENDS for dbprocs owned by the current dbproc's owner
**		which depend on it and for each such dbproc D add a row to the
**		temp table indicating that ITS OWNER no longer possesses EXECUTE
**		or GRANT OPTION FOR EXECUTE on D (depending on what is being
**		revoked on the current dbproc)
**	      }
**
**	      // now we need to process dependencies of dbprocs and views on
**	      // privileges on objects owned by users different from their
**	      // owners and of privileges on views on privileges on the views'
**	      // underlying tables, and of REFERENCES constraints on tables on
**	      // privileges on <referenced table>
**		  
**	      look for privilege descriptors in IIPRIV which describe
**	      dependence of objects (dbprocs and views), permits and constraints
**	      on AUTH possessing P on X + if X is a table or a view,
**	      intersection of the attribute map associated with the privilege
**	      descriptor and that associated with privilege being lost 
**	      must be non-empty.
**
**	      for each such descriptor
**	      {
**		if (the privilege descriptor describes dependency of a
**		    dbproc on a privilege)
**		{
**		  if (AUTH lost privilege P on X on which the dbproc depends
**		      and on ability to grant which permits on the
**		      dbproc depend)
**		  {
**	            if (<drop behaviour> is RESTRICT)
**	            {
**	              report to the user that he may not perform desired 
**		      operation because it would render some object 
**		      or permit abandoned
**	            }
**
**		    add a row to our temporary table indicating that OWNER of
**		    the dbproc (i.e. AUTH) lost ability to execute as well as 
**		    the ability to GRANT execute privilege on this dbproc
**		  }
**		  else  // AUTH lost ability to grant privilege P on X on which
**			// permits on the dbproc depend;
**		  {
**		    add a row to our temporary table indicating that AUTH
**		    lost ability to GRANT execute privilege on this
**		    dbproc
**		  }
**		}
**		else if (the privilege descriptor describes dependency
**		         of a view on a privilege P on X)
**		{
**		  // recall that views depend on SELECT privilege
**		  if (AUTH lost SELECT on X on which the view depends)
**		  {
**	            if (<drop behaviour> is RESTRICT)
**	            {
**	              report to the user that he may not perform desired 
**		      operation because it would render some object 
**		      or permit abandoned
**	            }
**
**		    perform cascading destruction of the view 
**		    (via qeu_d_cascade())
**		  }
**		  else  // AUTH lost ability to grant select on X on which
**			// permits granting SELECT on the view depend
**		  {
**		    add a row to our temporary table indicating that AUTH
**		    lost ability to GRANT select privilege on this view
**		  }
**		}
**		else if (    the privilege descriptor describes dependency
**			     of a constraint C on a privilege P on X
**			 and user AUTH is losing P
**		        )
**		{
**		  if (<drop behaviour> is RESTRICT)
**		  {
**		    report to the user that he may not perform desiredoperation
**		    because it would render some object or permit abandoned
**		  }
**
**		  destroy that constraint (by calling qeu_d_cascade())
**		}
**		else	// privilege descriptor describes dependence of a
**			// permit on a view V on user AUTH being able to grant 
**			// privilege P on X which is V's underlying table
**	
**		{
**	          if (<drop behaviour> is RESTRICT)
**	          {
**	            report to the user that he may not perform desired 
**		    operation because it would render some object 
**		    or permit abandoned
**	          }
**
**		  determine which privilege and/or ability to grant which
**		  privilege, if any, would be lost to the grantee of the
**		  permit and insert their description(s) into the temporary
**		  table
**
**		  if (the privilege associated with the permit is a subset of
**		      the privilege which the grantor of the permit may no
**		      longer grant (a mouthful, indeed))
**		  {
**		    destroy that permit;
**		  }
**		  else // the permit's grantor no longer possesses ability
**		       // to grant some (but not all) privilege described by
**		       // the permit
**		  {
**		    modify IIPROTECT and IIQRYTEXT tuples associated with the
**		    permit so it no longer conveys the privilege which the
**		    grantor may no longer grant 
**		  }
**		}
**	      }
**	    }
**	    else  // privilege P or ability to grant privilege P on 
**	          // object X was lost by PUBLIC
**	    {
**	      look for privilege descriptors in IIPRIV which describe
**	      dependence of objects (dbprocs or views), permits and constraints
**	      on some user, group, or role AUTH' possessing P on X + if X is a
**	      table or a view, intersection of the attribute map associated with
**	      the privilege descriptor and that associated with privilege being
**	      lost must be non-empty.
**
**	      for each such descriptor
**	      {
**		if (the privilege descriptor describes dependency of a
**		    dbproc on a privilege)
**		{
**		  if (AUTH' lost privilege P on X on which the dbproc depends
**		      and on ability to grant which permits on the 
**		      dbproc depend)
**		  {
**	            if (<drop behaviour> is RESTRICT)
**	            {
**	              report to the user that he may not perform desired 
**		      operation because it would render some object 
**		      or permit abandoned
**	            }
**
**		    add a row to our temporary table indicating that owner of
**		    the dbproc lost ability to execute as well as the
**		    ability to GRANT execute privilege on this dbproc
**		  }
**		  else if (user AUTH' lost ability to grant privilege P on X
**		           on which permits on the dbproc depend
**		  {
**		    add a row to our temporary table indicating that AUTH'
**		    lost ability to GRANT execute privilege on this dbproc
**		  }
**		}
**		else if (the privilege descriptor describes dependency
**		         of a view on a privilege P on X)
**		{
**		  // recall that views depend on SELECT privilege
**		  if (AUTH' lost SELECT on X on which the view depends)
**		  {
**	            if (<drop behaviour> is RESTRICT)
**	            {
**	              report to the user that he may not perform desired 
**		      operation because it would render some object 
**		      or permit abandoned
**	            }
**
**		    perform cascading destruction of the view 
**		    (via qeu_d_cascade())
**		  }
**		  else if (user AUTH' lost ability to grant select on X on
**		           which permits granting SELECT on the view depend)
**		  {
**		    add a row to our temporary table indicating that AUTH'
**		    lost ability to GRANT select privilege on this view
**		  }
**		}
**		else if (    the privilege descriptor describes dependency
**			     of a constraint C on a privilege P on X
**			 and user who created C is losing P
**			)
**		{
**		  if (<drop behaviour> is RESTRICT)
**		  {
**		    report to the user that he may not perform desiredoperation
**		    because it would render some object or permit abandoned
**		  }
**
**		  destroy that constraint (by calling qeu_d_cascade())
**		}
**		else	// privilege descriptor describes dependence of a
**			// permit on a view V on user AUTH' being able to grant 
**			// privilege P on X which is V's underlying table
**	
**		{
**	          if (<drop behaviour> is RESTRICT)
**	          {
**	            report to the user that he may not perform desired 
**		    operation because it would render some object 
**		    or permit abandoned
**	          }
**
**		  determine which privilege and/or ability to grant which
**		  privilege, if any, would be lost to the grantee of the
**		  permit and insert their description(s) into the temporary
**		  table
**
**		  if (the privilege associated with the permit is a subset of
**		      the privilege which the grantor of the permit may no
**		      longer grant (a mouthful, indeed))
**		  {
**		    destroy that permit;
**		  }
**		  else // the permit's grantor no longer possesses ability
**		       // to grant some (but not all) privilege described by
**		       // the permit
**		  {
**		    modify IIPROTECT and IIQRYTEXT tuples associated with the
**		    permit so it no longer conveys the privilege which the
**		    grantor may no longer grant 
**		  }
**		}
**	      }
**	    }
**        }
**
** Input:
**	qeuq_cb			QEU query CB
**	  qeuq_flag_mask
**	    QEU_DROP_CASCADE	if set, cascading revocation is taking place
**	  qeuq_permit_mask
**	    QEU_REVOKE_PRIVILEGES  must be set to indicate that we are
**				   revoking privileges
**	  .qeuq_v2b_col_xlate	address of function that will translate a map of
**				attributes of an updateable view into a map of
**				attributes of a specified underlying table or
**				view; will be set to NULL if we were called in
**				the course of processing DROP PERMIT since DROP
**				PERMIT should never result in cascading
**				destruction of permits
**	  .qeuq_b2v_col_xlate	address of function that will translate a map of
**				a table or view X into a map of attributes of an
**				updateable view which is defined over X; will be
**				set to NULL if we were called in the course of
**				processing DROP PERMIT since DROP PERMIT should
**				never result in cascading destruction of permits
**	qef_cb			qef session control block
**	privs_lost     		array of QEU_REVOKE_INFO's describing privileges
**				being revoked from a specified <auth id>;
**				may be set to NULL if no privileges are
**				being revoked
**	    qeu_obj_id		id of the object
**	    qeu_priv		privilege map; caller is expected to have
**				gotten rid of DB_GRANT_OPTION, DB_TEST, 
**				and DB_AGGREGATE bits
**	    qeu_attrmap 	map of attributes to which privilege
**				described by dbp_popset applies
**	    qeu_gtype		type of <auth id> losing privilege
**	    qeu_authid		<auth id> losing privilege
**	    qeu_flags		bits of useful information
**	     QEU_INVALIDATE_QPS invalidate QPs affected by cascading loss of
**				specified privilege(s)
**	grant_option_lost    	array of QEU_REVOKE_INFO's describing ability to
**				grant which privileges is being revoked 
**				from a speciifed <auth id>
**				may be set to NULL if ability to grant
**				privilege(s) is not being revoked (as would 
**				happen if a user is dropping a permit 
**				specifying some privilege but not WGO)
**          qeu_obj_id          id of the object
**	    qeu_priv		privilege map; caller is expected to have
**				gotten rid of DB_GRANT_OPTION, DB_TEST, 
**				and DB_AGGREGATE bits
**          qeu_attrmap         map of attributes to which privilege
**                              described by dbp_popset applies
**	    qeu_gtype           type of <auth id> losing ability to grant
**				privilege
**	    qeu_authid          <auth id> losing ability to grant privilege
**	    qeu_flags           bits of useful information
**	     QEU_INVALIDATE_QPS invalidate QPs affected by cascading loss of 
** 				GRANT OPTION FOR specified privilege(s)
**	grant_compatible_only	if TRUE, GRANT-compatible privilege is being
**				revoked, but there may be some permits conveying
**				non-GRANT-compatible privileges specifying the
**				same action on a subset (not necessarity proper)
**				of specified attributes
**	pdi_tbl_descr		srtucture containing description of a temp
**				table used to track changes in depth of
**				privileges
**				may be NULL if no grantable privileges have
**				been revoked
**
** Output:
**
**	qeuq_cb
**	    error		filled in if some error was encountered
**				NOTE: this function reports its own errors
**
** Returns:
**	E_DB_{OK,ERROR}
**
** Side effects:
**	memory may be allocated and deallocated
**
** History:
**
**	26-jul-92 (andre)
**	    written for GRANT/REVOKE project
**	06-aug-92 (andre)
**	    changed interface to accept variable length arrays of
**	    QEU_REVOKE_INFO describing which privileges have been lost 
**	    and/or GRANT OPTION FOR which privileges have been lost 
**	    to specified grantee
**	16-aug-92 (andre)
**	    now that qeuq_v2b_col_xlate and qeuq_b2v_col_xlate are set, we can
**	    use them to correctly compute maps of attributes in independent
**	    privilege descriptors and maps of attributes for which a specified
**	    privilege is lost
**      24-aug-92 (andre)
**	    qeu_depth() must be invoked to recompute (if necessary)
**	    depth of permits if we are processing a permit specifying a
**	    grantable privilege and one of the following takes place:
**	      - we are removing some attributes from a map associated
**		with a permit specifying GRANT OPTION FOR a table privilege or
**	      - we are clearing DB_GRANT_OPTION bit in dbp_popset of the permit
**	11-dec-92 (andre)
**	    add code to handle revocation of REFERENCES privilege
**	04-feb-93 (andre)
**	    if destroying a permit with which a tree is associated, call RDF to
**	    invalidate the QTREE cache entry;
**
**	    if a permit on a table, view, or database procedure had to be
**	    destroyed or altered, call RDF to invalidate the cache entry for the
**	    object to which the permit applied;
**	27-mar-93 (andre)
**	    if some constraints depends on the privilege being revoked then
**	      if RESTRICTed revocation was specified
**		report an error
**	      else
**		call qeu_d_cascade() to destroy the constraint and any objects
**		that depend on it
**	    we neeed to call qeu_d_cascade() rather than qeu_dintg() because
**	    the dbprocs and rules which were created to enforce the constraint
**	    must be destroyed as well and qeu_d_cascade() knows how to do it
**	10-jun-93 (andre)
**	    added a new argument (object owner name) to interfaces of
**	    qeu_match_privs() and qeu_depth() to correctly handle cases when 
**	    privileges were revoked from the object's owner
**	09-jul-93 (andre)
**	    We were using an i2 to contain value for the 4-th key into 
**	    IIDBDEPENDS even though i_qid is an i4.  As always, this worked 
** 	    fine on VMS but failed on UNIX (IIDBDEPENDS tuple could not be 
**	    found)
**	26-jul-93 (andre)
**	    added pdi_tbl_descr
**
**	    qeu_depth() will no longer be invoked whenever we drop or alter
**	    a permit; instead, qeu_match_privs() will be responsible 
**	    for storing descriptions of grantable privileges whose depth 
**	    has increased in the table whose description is contained in 
**	    *pdi_tbl_descr.  qeu_depth() will be invoked by the caller of
**	    this function once we can guarantee that there are no longer 
**	    any permits representing abandoned privilege(s)
**	15-sep-93 (andre)
**	    Once we alter/destroy a permit on an object, destroy an object, or
**	    downgrade status of a dbproc, we need to force invalidation of QPs
**	    which may have depended on the newly destroyed/altered object or 
**	    privilege.  This is accomplished by altering the timestamp of some 
**	    base table on which the object which was dropped or altered, or an
**	    object on which a permit was dropped or altered, depended.
**
**	    Caller of qeu_priv_revoke() will attempt to force invalidation of
**	    QPs which could depend on permit(s) which were dropped (if called 
**	    from qeu_dprot()) or privileges which were revoked (if caller 
**	    from qeu_revoke()).  If caller was unable to find a base table
**	    whose timestamp could be altered to effect QP invalidation, it will
**	    pass the responsibility on to this function by setting 
**	    QEU_INVALIDATE_QPS in qeu_flags.  In order to minimize number of
**	    calls to DMF to alter tables' timestamps(), we will only do so
**	    after removing all permits affected by loss of GRANT OPTION FOR 
**	    some privilege by the object's owner.  This will work correctly
**	    because 
**	      - we can guarantee that the caller will never request that 
**	    	we process cascading revocation of a privilege from the 
**		object's owner (in other words, we will never duplicate work
**		performed by the caller),
**	      - once we destroy/alter permits affected by owner's loss of 
**		GRANT OPTION FOR some privilege, there will be no other changes
**		to the status of the object resulting from the loss of those 
**		privileges.
**	    
**	    The following rules govern setting QEU_INVALIDATE_QPS bit in
**	    QEU_REVOKE_INFO.qeu_flags:
**	      - caller may set this bit to indicate that we should try to 
**		invalidate QPs affected by cascaded revocation of privilege(s)
**		specified by the user,
**	      - when entering new tuples into the temp table, if the bit is set
**		in the current row, it will be set in the new row
**	      - once we force invalidation of QPs affected by revocation of
**		privileges on a given VIEW, this bit will be reset to spare
**		us the unnecessary work of invalidating QPs dependent on
**		views and dbprocs in definitions of which this view appears
**	      - when entering a new tuple describing the fact that 
**		[GRANT OPTION FOR] EXECUTE on a dbproc was lost by its owner,
**		this bit will be set regardless of whether it is set in the
**		current row
**	    QEU_INVALIDATE_QPS will be propagated from the current temp table
**	    row to newly added rows + if the current row describes privilege
**	    lost on a dbproc and a new row describes privileges lost by another
**	    dbproc's owner (who may be the same as the owner of the first 
**	    dbproc) on that dbproc, QEU_INVALIDATE_QPS will be set in the new 
**	    row
**	07-oct-93 (andre)
**	    qeu_d_cascade() expects one more parameter - an address of a 
**	    DMT_TBL_ENTRY describing table/view/index being dropped; for all 
**	    other types of objects, NULL must be passed
**	29-oct-93 (andre)
**	    fixed a regression introduced during the October integration.
**	    If GRANT OPTION FOR a privilege was lost by the object's owner,
**	    we destroy/alter all permits on that object that used to convey 
**	    this privilege and then enter records that PUBLIC has lost the 
**	    privilege and the ability to grant that privilege on the object.  
**	    Unfortunately, I neglected to copy the attribute map from the temp 
**	    table row describing GRANT OPTION FOR privilege lost by the object's
**	    owner to the rows being constructed.  As a result, there contents 
**	    depended on their previous usage and the results were 
**	    non-deterministic.  This oversight has now been corrected.
**	24-nov-93 (andre)
**	    qeu_v_ubt_id() will take address of DB_ERROR and will report its own
**	    errors
**	10-feb-94 (andre)
**	    fix for bug 60459:
**	    having invoked dmt_show to obtain name of a table, we were checking
**	    incorrectly checking status, instead of local_status, to determine 
**	    whether the call succeeded.  As a result, we would end up calling 
**	    qef_error() with dmt_show.error.err_code which can be 0 and result 
**	    in qef_error() issuing QE0018
**	07-mar-94 (andre)
**	    fix for bug 58075:
**	    in addition to forcing invalidation of dbproc QPs when the status 
**	    of a dbproc is changed, we will also invoke qeu_destr_dbp_qp() to 
**	    actually destroy dbproc's QP if one exists in QSF
**	14-may-02 (inkdo01)
**	    Add support for sequence privileges.
**	7-Apr-2004 (schka24)
**	    Don't leave junk in attr default fields.
*/
static DB_STATUS
qeu_priv_revoke(
QEUQ_CB			*qeuq_cb,
QEF_CB          	*qef_cb,
QEU_REVOKE_INFO 	*privs_lost,
QEU_REVOKE_INFO 	*grant_option_lost,
i4			list_size,
QEU_PDI_TBL_DESCR	*pdi_tbl_descr)
{
    DB_STATUS	    		status = E_DB_OK, local_status;
    GLOBALREF QEF_S_CB 		*Qef_s_cb;
    RDF_CB			rdfcb, *rdf_inv_cb = &rdfcb;
    RDR_RB			*rdf_inv_rb = &rdf_inv_cb->rdf_rb;
    DB_IIPRIV			*priv_tuple;
    DB_IIXPRIV			*xpr_tuple;
    DB_PROTECTION		*prot_tuple, *prot_2tuple;
    DB_PROCEDURE		*proc_tuple, *proc_dprot_tuple;
    DB_IIXPROCEDURE		*xdbp_tuple;
    DB_IIDBDEPENDS		*dep_tuple;
    DB_IIEVENT			*ev_dprot_tuple;
    DB_IISEQUENCE		*seq_dprot_tuple;
    QEU_PERM_DEPTH_INFO		*perm_depth_info;
    bool			temp_created = FALSE;
    bool			inv_obj_cache = FALSE;
    bool			err_already_reported = FALSE;
    QEU_CB			privqeu, *priv_qeu = (QEU_CB *) NULL;
    QEU_CB			xprqeu, *xpr_qeu = (QEU_CB *) NULL;
    QEU_CB			protqeu, *prot_qeu = (QEU_CB *) NULL;
    QEU_CB			prot2qeu, *prot2_qeu = (QEU_CB *) NULL;
    QEU_CB			qtxtqeu, *qtxt_qeu = (QEU_CB *) NULL;
    QEU_CB			tempqeu, *temp_qeu = (QEU_CB *) NULL;
    QEU_CB			procqeu, *proc_qeu = (QEU_CB *) NULL;
    QEU_CB			xdbpqeu, *xdbp_qeu = (QEU_CB *) NULL;
    QEU_CB			depqeu, *dep_qeu = (QEU_CB *) NULL;
    QEF_DATA			priv_qefdata;
    QEF_DATA			xpr_qefdata;
    QEF_DATA			prot_qefdata;
    QEF_DATA			prot_2qefdata;
    QEF_DATA			in_temp_qefdata, out_temp_qefdata;
    QEF_DATA			proc_qefdata;
    QEF_DATA			xdbp_qefdata;
    QEF_DATA			dep_qefdata;
    QEF_DATA			dprot_proc_qefdata, dprot_ev_qefdata;
    QEF_DATA			dprot_seq_qefdata;
    ULM_RCB			ulm;
    DB_LOC_NAME			temp_loc_name;
    DMT_TBL_ENTRY		dmt_tbl_entry;
    QEU_REVOKE_INFO 		*read_temp_row, *write_temp_row;
    QEU_REVOKE_INFO		*priv_revoked, *gropt_revoked;
    DMR_ATTR_ENTRY		prot_array[2];
    DMR_ATTR_ENTRY		*prot_ptr_array[2];
    DMR_ATTR_ENTRY              prot_2array[2];
    DMR_ATTR_ENTRY              *prot_2ptr_array[2];
    DMR_ATTR_ENTRY		xpr_array[3];
    DMR_ATTR_ENTRY		*xpr_ptr_array[3];
    DMT_ATTR_ENTRY		temp_array[QEU_REVOKE_TBL_DEGREE];
    DMT_ATTR_ENTRY		*temp_ptr_array[QEU_REVOKE_TBL_DEGREE];
    DMR_ATTR_ENTRY		xdbp_array[2];
    DMR_ATTR_ENTRY              *xdbp_ptr_array[2];
    DMR_ATTR_ENTRY		dep_array[4];
    DMR_ATTR_ENTRY		*dep_ptr_array[4];
    DMT_CB			dmt_cb, dmt_alt_cb;
    DMT_CHAR_ENTRY		char_array[2];
    DMT_SHW_CB			dmt_show;
    i4				i;
    i4				*map_from, *map_to;
    DB_TAB_ID			obj_id, obj_2id, xpr_id, dep_id;
    i4				attrmap[DB_COL_WORDS];
    i4				xpr_obj_priv;
    i4				indep_obj_type;
    i4				priv_descr_no;
    i4			error;
    i4				exit_loop, exit_loop2;

    if (!privs_lost && !grant_option_lost)
    {
	return(E_DB_OK);
    }
    
    /* initialize array of attribute pointers for the temporary table */
    for (i = 0; i < QEU_REVOKE_TBL_DEGREE; i++)
	temp_ptr_array[i] = temp_array + i;

    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
    ulm.ulm_streamid = (PTR)NULL;

    for (exit_loop = FALSE; !exit_loop; )	/* somet'ng to break out of */
    {
	/* Check the control block. */

	error = E_QE0017_BAD_CB;
	status = E_DB_ERROR;
	if (qeuq_cb->qeuq_type != QEUQCB_CB ||
            qeuq_cb->qeuq_length != sizeof(QEUQ_CB))	
	    break;

	error = E_QE0018_BAD_PARAM_IN_CB;
	if (   ~qeuq_cb->qeuq_permit_mask & QEU_REVOKE_PRIVILEGES
            || (qeuq_cb->qeuq_db_id == 0)
            || (qeuq_cb->qeuq_d_id == 0))

	    break;
		
	/* 
	** create a temporary table to hold a list of privileges which
	** will be revoked
	*/
	temp_array[0].attr_type = DB_INT_TYPE;
	temp_array[0].attr_size = 4;
	temp_array[0].attr_precision = 0;
	temp_array[0].attr_flags = 0;
	temp_array[0].attr_collID = -1;
	SET_CANON_DEF_ID(temp_array[0].attr_defaultID, DB_DEF_NOT_DEFAULT);
	temp_array[0].attr_default = NULL;

	for (i = 1; i < QEU_REVOKE_TBL_DEGREE; i++)
	    STRUCT_ASSIGN_MACRO(temp_array[0], temp_array[i]);

	/* not all attributes are i4's + we need to assign them names */

	/* first attribute - first half of object id */
	MEmove((sizeof("id_base") - 1), "id_base", ' ',
	    DB_MAXNAME, (char *) &temp_array[0].attr_name);

	/* second attribute - second half of object id */
	MEmove((sizeof("id_idx") - 1), "id_idx", ' ',
	    DB_MAXNAME, (char *) &temp_array[1].attr_name);

	/* third attribute - privilege map */
	MEmove((sizeof("priv_map") - 1), "priv_map", ' ',
	    DB_MAXNAME, (char *) &temp_array[2].attr_name);

	/* fourth attribute - flag field */
	temp_array[3].attr_size = 2;
	MEmove((sizeof("flags") - 1), "flags", ' ',
	    DB_MAXNAME, (char *) &temp_array[3].attr_name);

	/* fifth atribute - authorization id type */
	temp_array[4].attr_size = 2;
	MEmove((sizeof("auth_type") - 1), "auth_type", ' ',
	    DB_MAXNAME, (char *) &temp_array[4].attr_name);

	/* sixth attribute - authorization id */
	temp_array[5].attr_type = DB_CHA_TYPE;
	temp_array[5].attr_size = sizeof(DB_OWN_NAME);
	MEmove((sizeof("auth_id") - 1), "auth_id", ' ',
	    DB_MAXNAME, (char *) &temp_array[5].attr_name);

	/* attributes 7-16 constitute an attribute map */
	{
	    char	c[sizeof("attmrapa") - 1];
	    char	*p = c + (i4)(sizeof("attmrapa") - 2);

	    MEmove((sizeof("attmrapa") - 1), "attrmapa", ' ',
		(sizeof("attmrapa") - 1), c);
	    (*p)--;

	    for (i = 6; i < QEU_REVOKE_TBL_DEGREE; i++)
	    {
		(*p)++;
		MEmove((sizeof("attmrapa") - 1), c, ' ',
		    DB_MAXNAME, (char *) &temp_array[i].attr_name);
	    }
	}

	dmt_cb.length = sizeof(DMT_CB);
	dmt_cb.type = DMT_TABLE_CB;
	dmt_cb.dmt_flags_mask = DMT_DBMS_REQUEST;
        dmt_cb.dmt_db_id = qeuq_cb->qeuq_db_id;
	dmt_cb.dmt_char_array.data_address = 0;
	dmt_cb.dmt_char_array.data_in_size = 0;
	dmt_cb.dmt_attr_array.ptr_address = (PTR) temp_ptr_array;
	dmt_cb.dmt_attr_array.ptr_in_count = QEU_REVOKE_TBL_DEGREE;
	dmt_cb.dmt_attr_array.ptr_size = sizeof (DMT_ATTR_ENTRY);
	dmt_cb.dmt_tran_id = qef_cb->qef_dmt_id;	    
	MEmove((sizeof("$default") - 1), "$default", ' ', 
	    sizeof(DB_LOC_NAME), (char *) &temp_loc_name);
	dmt_cb.dmt_location.data_address = (PTR) &temp_loc_name;
	dmt_cb.dmt_location.data_in_size = sizeof(DB_LOC_NAME);

	status = dmf_call(DMT_CREATE_TEMP, &dmt_cb);
	if (status != E_DB_OK)
	{
	    if (dmt_cb.error.err_code == E_DM0078_TABLE_EXISTS)
	    {
		error = E_QE0050_TEMP_TABLE_EXISTS;
		status = E_DB_SEVERE;
	    }
	    else
	    {
		error = dmt_cb.error.err_code;
	    }
	    break;
	}
	temp_created = TRUE;

	for (exit_loop2 = FALSE; !exit_loop2; )            /* to break out of */
	{
	    /* Open stream and allocate list in one action */
	    ulm.ulm_flags = ULM_PRIVATE_STREAM | ULM_OPEN_AND_PALLOC;
	    ulm.ulm_psize = 4 * sizeof(QEU_REVOKE_INFO);

	    if ((status = qec_mopen(&ulm)) != E_DB_OK)
		break;
	    
	    read_temp_row = (QEU_REVOKE_INFO *) ulm.ulm_pptr;
	    write_temp_row = read_temp_row + 1;
	    /* 
	    ** the other two records are will be referenced through
	    ** priv_revoked and gropt_revoked:
	    **     priv_revoked = read_temp_row + 2;
	    **     gropt_revoked = read_temp_row + 3;
	    */

	    ulm.ulm_psize = sizeof(DB_PROTECTION) * 2;
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    prot_tuple = (DB_PROTECTION *) ulm.ulm_pptr;
	    prot_2tuple = prot_tuple + 1;

	    ulm.ulm_psize = sizeof(DB_IIPRIV);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    priv_tuple = (DB_IIPRIV *) ulm.ulm_pptr;

	    ulm.ulm_psize = sizeof(DB_IIXPRIV);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    xpr_tuple = (DB_IIXPRIV *) ulm.ulm_pptr;

	    /*
	    ** allocate one tuple for reading IIPROCEDURE and one for passing to
	    ** qeu_dprot() if some permits on dbproc(s) need to be destroyed
	    */
	    ulm.ulm_psize = sizeof(DB_PROCEDURE) * 2;
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    proc_tuple = (DB_PROCEDURE *) ulm.ulm_pptr;
	    proc_dprot_tuple = proc_tuple + 1;
	    dprot_proc_qefdata.dt_data = (PTR) proc_dprot_tuple;

	    ulm.ulm_psize = sizeof(DB_IIXPROCEDURE);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;

	    }
	    xdbp_tuple = (DB_IIXPROCEDURE *) ulm.ulm_pptr;

	    ulm.ulm_psize = sizeof(DB_IIDBDEPENDS);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    dep_tuple = (DB_IIDBDEPENDS *) ulm.ulm_pptr;

	    /*
	    ** allocate a tuple that will be used when calling qeu_dprot() to
	    ** destroy permit(s) on a dbevent
	    */
	    ulm.ulm_psize = sizeof(DB_IIEVENT);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    ev_dprot_tuple = (DB_IIEVENT *) ulm.ulm_pptr;
	    dprot_ev_qefdata.dt_data = (PTR) ev_dprot_tuple;

	    /*
	    ** allocate a tuple that will be used when calling qeu_dprot() to
	    ** destroy permit(s) on a sequence
	    */
	    ulm.ulm_psize = sizeof(DB_IISEQUENCE);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    seq_dprot_tuple = (DB_IISEQUENCE *) ulm.ulm_pptr;
	    dprot_seq_qefdata.dt_data = (PTR) seq_dprot_tuple;

	    ulm.ulm_psize = sizeof(QEU_PERM_DEPTH_INFO);
	    if ((status = qec_malloc(&ulm)) != E_DB_OK)
	    {
		ulm_closestream(&ulm);
		break;
	    }
	    perm_depth_info = (QEU_PERM_DEPTH_INFO *) ulm.ulm_pptr;

	    exit_loop2 = TRUE;
	}
	if (status != E_DB_OK)
	{
	    error = E_QE001E_NO_MEM;
	    break;
	}

	/*
	** initialize the control block that will be used to look up information
	** on tables and views
	*/
	dmt_show.type				= DMT_SH_CB;
	dmt_show.length				= sizeof(DMT_SHW_CB);
	dmt_show.dmt_char_array.data_in_size	= 0;
	dmt_show.dmt_char_array.data_out_size	= 0;
	dmt_show.dmt_flags_mask			= DMT_M_TABLE;
	dmt_show.dmt_db_id			= qeuq_cb->qeuq_db_id;
	dmt_show.dmt_session_id			= qeuq_cb->qeuq_d_id;
	dmt_show.dmt_table.data_address		= (PTR) &dmt_tbl_entry;
	dmt_show.dmt_table.data_in_size		= sizeof(DMT_TBL_ENTRY);
	dmt_show.dmt_char_array.data_address	= (PTR) NULL;
	dmt_show.dmt_char_array.data_in_size	= 0;
	dmt_show.dmt_char_array.data_out_size	= 0;

	/*
	** initialize the control block that will be used if it is necessary to
	** reset some bits in IIRELATION.tbl_status_mask for some table or view
	*/
	dmt_alt_cb.length			= sizeof(DMT_CB);
	dmt_alt_cb.type				= DMT_TABLE_CB;
	dmt_alt_cb.dmt_tran_id			= qef_cb->qef_dmt_id;
	dmt_alt_cb.dmt_flags_mask		= 0;
	dmt_alt_cb.dmt_db_id			= qeuq_cb->qeuq_db_id;
	dmt_alt_cb.dmt_char_array.data_address	= (PTR) char_array;

	/*
	** initialize the control block which will be used to call RDF to
	** invalidate cache entries
	*/
	qeu_rdfcb_init((PTR) rdf_inv_cb, qef_cb);

	/* 
        ** Open the Temporary table and the iiprotect, iiqrytext, iipriv,
	** iixprocedure, iiprocedure and iixpriv System tables.
	** IIPROTECT will be opened twice - once for reading (and possibly
	** updating and/or deleting) tuples in this function and once for
	** reading tuples in qeu_match_privs()
        */

	privqeu.qeu_type = QEUCB_CB;
        privqeu.qeu_length = sizeof(QEUCB_CB);
        privqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
        privqeu.qeu_lk_mode = DMT_IX;
        privqeu.qeu_flag = DMT_U_DIRECT;
        privqeu.qeu_access_mode = DMT_A_WRITE;
	privqeu.qeu_mask = 0;
	privqeu.qeu_qual = privqeu.qeu_f_qual = 0;
	privqeu.qeu_qarg = privqeu.qeu_f_qarg = 0;

	STRUCT_ASSIGN_MACRO(privqeu, protqeu);
	STRUCT_ASSIGN_MACRO(privqeu, prot2qeu);
	STRUCT_ASSIGN_MACRO(privqeu, qtxtqeu);
	STRUCT_ASSIGN_MACRO(privqeu, xprqeu);
	STRUCT_ASSIGN_MACRO(privqeu, tempqeu);
	STRUCT_ASSIGN_MACRO(privqeu, procqeu);
	STRUCT_ASSIGN_MACRO(privqeu, xdbpqeu);
	STRUCT_ASSIGN_MACRO(privqeu, depqeu);

        tempqeu.qeu_tab_id.db_tab_base  = dmt_cb.dmt_id.db_tab_base;
        tempqeu.qeu_tab_id.db_tab_index = dmt_cb.dmt_id.db_tab_index;
	status = qeu_open(qef_cb, &tempqeu);
	if (status != E_DB_OK)
	{
	    error = tempqeu.error.err_code;
	    break;
	}
	temp_qeu = &tempqeu;

	protqeu.qeu_tab_id.db_tab_base  = DM_B_PROTECT_TAB_ID;
	protqeu.qeu_tab_id.db_tab_index = DM_I_PROTECT_TAB_ID;
	status = qeu_open(qef_cb, &protqeu);
	if (status != E_DB_OK)
	{
	    error = protqeu.error.err_code;
	    break;
	}
	prot_qeu = &protqeu;

	prot2qeu.qeu_tab_id.db_tab_base  = DM_B_PROTECT_TAB_ID;
	prot2qeu.qeu_tab_id.db_tab_index = DM_I_PROTECT_TAB_ID;
	status = qeu_open(qef_cb, &prot2qeu);
	if (status != E_DB_OK)
	{
	    error = prot2qeu.error.err_code;
	    break;
	}
	prot2_qeu = &prot2qeu;

	qtxtqeu.qeu_tab_id.db_tab_base  = DM_B_QRYTEXT_TAB_ID;
	qtxtqeu.qeu_tab_id.db_tab_index = DM_I_QRYTEXT_TAB_ID;
	status = qeu_open(qef_cb, &qtxtqeu);
	if (status != E_DB_OK)
	{
	    error = qtxtqeu.error.err_code;
	    break;
	}
	qtxt_qeu = &qtxtqeu;

	privqeu.qeu_tab_id.db_tab_base  = DM_B_PRIV_TAB_ID;
	privqeu.qeu_tab_id.db_tab_index = DM_I_PRIV_TAB_ID;
	status = qeu_open(qef_cb, &privqeu);
	if (status != E_DB_OK)
	{
	    error = privqeu.error.err_code;
	    break;
	}
	priv_qeu = &privqeu;

	xprqeu.qeu_tab_id.db_tab_base  = DM_B_XPRIV_TAB_ID;
	xprqeu.qeu_tab_id.db_tab_index = DM_I_XPRIV_TAB_ID;
	status = qeu_open(qef_cb, &xprqeu);
	if (status != E_DB_OK)
	{
	    error = xprqeu.error.err_code;
	    break;
	}
	xpr_qeu = &xprqeu;

	procqeu.qeu_tab_id.db_tab_base  = DM_B_DBP_TAB_ID;
	procqeu.qeu_tab_id.db_tab_index = DM_I_DBP_TAB_ID;
	status = qeu_open(qef_cb, &procqeu);
	if (status != E_DB_OK)
	{
	    error = procqeu.error.err_code;
	    break;
	}
	proc_qeu = &procqeu;

	xdbpqeu.qeu_tab_id.db_tab_base  = DM_B_XDBP_TAB_ID;
	xdbpqeu.qeu_tab_id.db_tab_index = DM_I_XDBP_TAB_ID;
	status = qeu_open(qef_cb, &xdbpqeu);
	if (status != E_DB_OK)
	{
	    error = xdbpqeu.error.err_code;
	    break;
	}
	xdbp_qeu = &xdbpqeu;

	depqeu.qeu_tab_id.db_tab_base  = DM_B_DEPENDS_TAB_ID;
	depqeu.qeu_tab_id.db_tab_index = DM_I_DEPENDS_TAB_ID;
	status = qeu_open(qef_cb, &depqeu);
	if (status != E_DB_OK)
	{
	    error = depqeu.error.err_code;
	    break;
	}
	dep_qeu = &depqeu;

	temp_qeu->qeu_count = 1;
	temp_qeu->qeu_tup_length = sizeof(QEU_REVOKE_INFO);
	/*
	** we will need two QEF_DATA structures pointing to two separate
	** QEU_REVOKE_INFO tuples since we may be reading and writing at the
	** same time
	*/
	temp_qeu->qeu_output = &out_temp_qefdata;
	out_temp_qefdata.dt_next = 0;
	out_temp_qefdata.dt_size = sizeof(QEU_REVOKE_INFO);
	out_temp_qefdata.dt_data = (PTR) read_temp_row;

	temp_qeu->qeu_input = &in_temp_qefdata;
	in_temp_qefdata.dt_next = 0;
	in_temp_qefdata.dt_size = sizeof(QEU_REVOKE_INFO);
	in_temp_qefdata.dt_data = (PTR) write_temp_row;

	prot_qeu->qeu_count = 1;
	prot_qeu->qeu_tup_length = sizeof(DB_PROTECTION);
	/* we may need to read and possibly update IIPROTECT tuples */
	prot_qeu->qeu_output = prot_qeu->qeu_input = &prot_qefdata;
	prot_qefdata.dt_next = 0;
	prot_qefdata.dt_size = sizeof(DB_PROTECTION);
	prot_qefdata.dt_data = (PTR) prot_tuple;

	prot2_qeu->qeu_count = 1;
	prot2_qeu->qeu_tup_length = sizeof(DB_PROTECTION);
	prot2_qeu->qeu_output = &prot_2qefdata;
	prot_2qefdata.dt_next = 0;
	prot_2qefdata.dt_size = sizeof(DB_PROTECTION);
	prot_2qefdata.dt_data = (PTR) prot_2tuple;

	priv_qeu->qeu_count = 1;
	priv_qeu->qeu_tup_length = sizeof(DB_IIPRIV);
	/* we will be reading and sometimes updating IIPRIV tuples */
	priv_qeu->qeu_output = priv_qeu->qeu_input = &priv_qefdata;
	priv_qefdata.dt_next = 0;
	priv_qefdata.dt_size = sizeof(DB_IIPRIV);
	priv_qefdata.dt_data = (PTR) priv_tuple;

	xpr_qeu->qeu_count = 1;
	xpr_qeu->qeu_tup_length = sizeof(DB_IIXPRIV);
	xpr_qeu->qeu_output = &xpr_qefdata;
	xpr_qefdata.dt_next = 0;
	xpr_qefdata.dt_size = sizeof(DB_IIXPRIV);
	xpr_qefdata.dt_data = (PTR) xpr_tuple;

	proc_qeu->qeu_count = 1;
	proc_qeu->qeu_tup_length = sizeof(DB_PROCEDURE);
	proc_qeu->qeu_output = proc_qeu->qeu_input = &proc_qefdata;
	proc_qefdata.dt_next = 0;
	proc_qefdata.dt_size = sizeof(DB_PROCEDURE);
	proc_qefdata.dt_data = (PTR) proc_tuple;

	xdbp_qeu->qeu_count = 1;
	xdbp_qeu->qeu_tup_length = sizeof(DB_IIXPROCEDURE);
	xdbp_qeu->qeu_output = &xdbp_qefdata;
	xdbp_qefdata.dt_next = 0;
	xdbp_qefdata.dt_size = sizeof(DB_IIXPROCEDURE);
	xdbp_qefdata.dt_data = (PTR) xdbp_tuple;

	dep_qeu->qeu_count = 1;
	dep_qeu->qeu_tup_length = sizeof(DB_IIDBDEPENDS);
	dep_qeu->qeu_output = &dep_qefdata;
	dep_qefdata.dt_next = 0;
	dep_qefdata.dt_size = sizeof(DB_IIDBDEPENDS);
	dep_qefdata.dt_data = (PTR) dep_tuple;

	/* 
	** now append to the temporary table row(s) describing privilege
	** being revoked and/or ability to grant privilege being revoked
	*/
	status = qeu_add_lost_privs(qef_cb, temp_qeu, privs_lost,
	    grant_option_lost, write_temp_row, list_size);
	if (status != E_DB_OK)
	{
	    error = temp_qeu->error.err_code;
	    break;
	}

	/* 
	** we are almost ready to fire off the algorithm, but first set
	** up the necessary fields to be able to access IIPROTECT, IIDBDEPENDS,
	** IIXPRIV and IIXPROCEDURE by key and IIPRIV and IIPROCEDURE by TID +
	** reposition the temporary table for all to read from the beginning 
	*/
	prot_qeu->qeu_key = prot_ptr_array;
	prot_qeu->qeu_klen = 2;

	prot_ptr_array[0] 		 = prot_array;
	prot_ptr_array[0]->attr_number 	 = DM_1_PROTECT_KEY;
	prot_ptr_array[0]->attr_operator = DMR_OP_EQ;
	prot_ptr_array[0]->attr_value    = (char *) &obj_id.db_tab_base;

	prot_ptr_array[1] 		 = prot_array + 1;
	prot_ptr_array[1]->attr_number   = DM_2_PROTECT_KEY;
	prot_ptr_array[1]->attr_operator = DMR_OP_EQ;
	prot_ptr_array[1]->attr_value    = (char *) &obj_id.db_tab_index;

	prot2_qeu->qeu_key = prot_2ptr_array;
	prot2_qeu->qeu_klen = 2;

	prot_2ptr_array[0] 		  = prot_2array;
	prot_2ptr_array[0]->attr_number   = DM_1_PROTECT_KEY;
	prot_2ptr_array[0]->attr_operator = DMR_OP_EQ;
	prot_2ptr_array[0]->attr_value    = (char *) &obj_2id.db_tab_base;

	prot_2ptr_array[1] 		  = prot_2array + 1;
	prot_2ptr_array[1]->attr_number   = DM_2_PROTECT_KEY;
	prot_2ptr_array[1]->attr_operator = DMR_OP_EQ;
	prot_2ptr_array[1]->attr_value    = (char *) &obj_2id.db_tab_index;

	xpr_qeu->qeu_key = xpr_ptr_array;
	xpr_qeu->qeu_klen = 3;

	xpr_ptr_array[0] 		 = xpr_array;
	xpr_ptr_array[0]->attr_number    = DM_1_XPRIV_KEY;
	xpr_ptr_array[0]->attr_operator  = DMR_OP_EQ;
	xpr_ptr_array[0]->attr_value     = (char *) &xpr_id.db_tab_base;

	xpr_ptr_array[1] 		 = xpr_array + 1;
	xpr_ptr_array[1]->attr_number    = DM_2_XPRIV_KEY;
	xpr_ptr_array[1]->attr_operator  = DMR_OP_EQ;
	xpr_ptr_array[1]->attr_value     = (char *) &xpr_id.db_tab_index;

	xpr_ptr_array[2] 		 = xpr_array + 2;
	xpr_ptr_array[2]->attr_number    = DM_3_XPRIV_KEY;
	xpr_ptr_array[2]->attr_operator  = DMR_OP_EQ;
	xpr_ptr_array[2]->attr_value     = (char *) &xpr_obj_priv;

	xdbp_qeu->qeu_key = xdbp_ptr_array;
	xdbp_qeu->qeu_klen = 2;

	xdbp_ptr_array[0] 		 = xdbp_array;
	xdbp_ptr_array[0]->attr_number   = DM_1_XDBP_KEY;
	xdbp_ptr_array[0]->attr_operator = DMR_OP_EQ;
	xdbp_ptr_array[0]->attr_value    = (char *) &obj_id.db_tab_base;

	xdbp_ptr_array[1] 		 = xdbp_array + 1;
	xdbp_ptr_array[1]->attr_number   = DM_2_XDBP_KEY;
	xdbp_ptr_array[1]->attr_operator = DMR_OP_EQ;
	xdbp_ptr_array[1]->attr_value    = (char *) &obj_id.db_tab_index;

	dep_qeu->qeu_key = dep_ptr_array;

	dep_ptr_array[0] 		 = dep_array;
	dep_ptr_array[0]->attr_number    = DM_1_DEPENDS_KEY;
	dep_ptr_array[0]->attr_operator  = DMR_OP_EQ;
	dep_ptr_array[0]->attr_value     = (char *) &dep_id.db_tab_base;

	dep_ptr_array[1] 		 = dep_array + 1;
	dep_ptr_array[1]->attr_number    = DM_2_DEPENDS_KEY;
	dep_ptr_array[1]->attr_operator  = DMR_OP_EQ;
	dep_ptr_array[1]->attr_value     = (char *) &dep_id.db_tab_index;

	dep_ptr_array[2] 		 = dep_array + 2;
	dep_ptr_array[2]->attr_number    = DM_3_DEPENDS_KEY;
	dep_ptr_array[2]->attr_operator  = DMR_OP_EQ;
	dep_ptr_array[2]->attr_value     = (char *) &indep_obj_type;

	dep_ptr_array[3] 		 = dep_array + 3;
	dep_ptr_array[3]->attr_number    = DM_4_DEPENDS_KEY;
	dep_ptr_array[3]->attr_operator  = DMR_OP_EQ;
	dep_ptr_array[3]->attr_value     = (char *) &priv_descr_no;

	priv_qeu->qeu_flag = QEU_BY_TID;
	priv_qeu->qeu_getnext = QEU_NOREPO;
	priv_qeu->qeu_klen = 0;

	proc_qeu->qeu_flag = QEU_BY_TID;
	proc_qeu->qeu_getnext = QEU_NOREPO;
	proc_qeu->qeu_klen = 0;

	temp_qeu->qeu_getnext = QEU_REPO;
	temp_qeu->qeu_count = 1;
	temp_qeu->qeu_klen = 0;

	/* here we go ... */
	for (;;)
	{
	    bool	    tbl_priv_lost;
	    bool	    priv_lost_by_owner;

	    /*
	    ** if some permits had to be dropped or altered because of the loss
	    ** of [GRANT OPTION FOR] privileges on a table, view, or dbproc
	    ** described in read_temp_row, invalidate the RDF cache entry now
	    */
	    if (inv_obj_cache)
	    {
		inv_obj_cache = FALSE;
		
		/*
		** if permits on a dbproc were dropped or altered, we need to
		** tell RDF to invalidate the cache entry for the dbproc (as
		** opposed to a table or view)
		*/
		if (read_temp_row->qeu_priv & DB_EXECUTE)
		{
		    /*
		    ** dbproc name and owner have already been saved in RDF_CB
		    */
		    rdf_inv_rb->rdr_types_mask |= RDR_PROCEDURE;
		}
		else
		{
		    STRUCT_ASSIGN_MACRO(read_temp_row->qeu_obj_id,
					rdf_inv_rb->rdr_tabid);
		}
		
		status = rdf_call(RDF_INVALIDATE, rdf_inv_cb);
		if (DB_FAILURE_MACRO(status))
		{
		    error = rdf_inv_cb->rdf_error.err_code;
		    break;
		}

		/*
		** reset RDR_PROCEDURE in rdf_inv_rb->rdr_types_mask to avoid
		** having to do it elsewhere
		*/
		rdf_inv_rb->rdr_types_mask &= ~RDR_PROCEDURE;
	    }

	    status = qeu_get(qef_cb, temp_qeu);
	    if (status != E_DB_OK)
	    {
		if (temp_qeu->error.err_code == E_QE0015_NO_MORE_ROWS)
		{
		    /* ran out of rows - we are done */
		    status = E_DB_OK;
		}
		else
		{
		    error = temp_qeu->error.err_code;
		}
		break;
	    }

	    /* Keep scanning until no more rows, do not reposition. */
	    temp_qeu->qeu_getnext = QEU_NOREPO;

	    obj_id.db_tab_base = obj_2id.db_tab_base = 
		xpr_id.db_tab_base = read_temp_row->qeu_obj_id.db_tab_base;
		
	    obj_id.db_tab_index = obj_2id.db_tab_index =
		xpr_id.db_tab_index = read_temp_row->qeu_obj_id.db_tab_index;

	    xpr_obj_priv = read_temp_row->qeu_priv;

	    /*
	    ** remember whether the current temp table row indicates that
	    ** a privilege and/or ability to grant privilege on a table or view
	    ** (as opposed to dbproc or dbevent) is being lost; this will
	    ** help us determine whether to worry about attribute maps
	    */
	    if (xpr_obj_priv &
		   (DB_RETRIEVE | DB_REPLACE | DB_DELETE | DB_APPEND |
		   DB_REFERENCES))
	    {
		tbl_priv_lost = TRUE;
	    }
	    else
	    {
		tbl_priv_lost = FALSE;
	    }

	    /*
	    ** remember whether the current row describes the fact that owner
	    ** of an object lost [GRANT OPTION FOR] <privilege> on his object
	    */
	    priv_lost_by_owner = 
		(read_temp_row->qeu_flags & QEU_PRIV_LOST_BY_OWNER) != 0;

	    if (read_temp_row->qeu_flags & QEU_GRANT_OPTION_LOST)
	    {
		/*
		** the case where owner of a dbproc lost GRANT OPTION FOR
		** EXECUTE on his dbproc will be handled separately
		*/
		if (   read_temp_row->qeu_gtype == DBGR_USER
		    && (   read_temp_row->qeu_priv != DB_EXECUTE
			|| !priv_lost_by_owner)
		   )
		{
		    /*
		    ** look for permits granted by user U whose auth_id is in
		    ** read_temp_row->qeu_authid or, if the GRANT OPTION FOR
		    ** privilege was lost by the object's owner, by any 
		    ** <auth id>, which (permits, that is) may be
		    ** affected by U's loss of grant option
		    */
		    prot_qeu->qeu_getnext = QEU_REPO;
		    prot_qeu->qeu_klen = 2;
		    prot_qeu->qeu_count = 1;

		    while (status == E_DB_OK)
		    {
			status = qeu_get(qef_cb, prot_qeu);
			if (status != E_DB_OK)
			{
			    if (prot_qeu->error.err_code ==
				    E_QE0015_NO_MORE_ROWS)
			    {
				/* ran out of rows - we are done */
				status = E_DB_OK;
			    }
			    else
			    {
				error = prot_qeu->error.err_code;
			    }
			    break;
			}

			if (prot_qeu->qeu_getnext == QEU_REPO)
			{
			    prot_qeu->qeu_getnext = QEU_NOREPO;
			    prot_qeu->qeu_klen = 0;
			}

			/*
			** if this tuple represents an "access" permit or a
			** security_alarm, skip it
			*/
			if (prot_tuple->dbp_popset & (DB_CAN_ACCESS | DB_ALARM))
			    continue;

			/*
			** if this is an "older" (i.e. pre-6.5) permit, skip it
			*/
			if (~prot_tuple->dbp_flags & DBP_65_PLUS_PERM)
			    continue;
			
			/*
			** if the permit is not specifying privilege grant
			** option for which is being revoked, skip it
			*/
			if (!(prot_tuple->dbp_popset & read_temp_row->qeu_priv))
			    continue;

			/* 
			** if privilege is not being revoked from all users
			** because GRANT OPTION FOR it was lost by the object
			** (view, to be more precise since we have postponed
			** ealing with users losing GRANT OPTION FOR EXECUTEon
			** their dbprocs until later) owner and permit was not 
			** granted by U, skip it
			*/
			if (   !priv_lost_by_owner
			    && MEcmp((PTR) &prot_tuple->dbp_grantor,
			             (PTR) &read_temp_row->qeu_authid,
				     sizeof(read_temp_row->qeu_authid)))
			{
			    continue;
			}

			/*
			** if object on which privilege is being revoked is a
			** table or a view, attribute map of the
			** privilege being revoked should have non-empty
			** intersection with attribute map in the permit
			*/
			if (tbl_priv_lost)
			{
			    i4		*tmp_map, *perm_map, *res_map;
			    bool	found = FALSE;

			    for (i = 0,
				 tmp_map = read_temp_row->qeu_attrmap,
				 perm_map = prot_tuple->dbp_domset,
				 res_map = attrmap;

				 i < DB_COL_WORDS;

				 i++, tmp_map++, perm_map++, res_map++
			        )
			    {
				if ((*res_map = *tmp_map & *perm_map))
				    found = TRUE;
			    }

			    if (!found)
				continue;
			}

			/*
			** if we got here, this permit will have to be either
			** altered or destroyed; if the current <drop behaviour>
			** (as described by setting of QEU_DROP_CASCADE in
			** qeuq_cb->qeuq_flag_mask) is not CASCADE, report error
			** to the caller
			*/
			if (~qeuq_cb->qeuq_flag_mask & QEU_DROP_CASCADE)
			{
			    error = E_QE0250_ABANDONED_PERMIT;
			    status = E_DB_ERROR;
			    break;
			}

			/*
			** if the permit was on a table or a view,
			** and its attribute map is not a subset of the
			** attribute map of the privilege grant option for which
			** is being revoked, we will modify IIPROTECT tuple to
			** correctly reflect privileges that may be granted by
			** the grantor of the permit and call
			** qeu_construct_perm_text() to generate permit text
			** corresponding to privileges granted by the permit;
			** 
			** otherwise we will simply destroy this permit
			** (remembering to tell qeu_dprot() to not pursue any
			** abandoned objects)
			**
			** dbproc, sequence and dbevent permits will be always destroyed
			*/

			if (   prot_tuple->dbp_obtype == DBOB_DBPROC
			    || prot_tuple->dbp_obtype == DBOB_EVENT
			    || prot_tuple->dbp_obtype == DBOB_SEQUENCE
			    || BTsubset((char *) prot_tuple->dbp_domset,
				   (char *) read_temp_row->qeu_attrmap,
				   BITS_IN(read_temp_row->qeu_attrmap))
			   )
			{
			    QEUQ_CB	dprot_qeuqcb;

			    STRUCT_ASSIGN_MACRO((*qeuq_cb), dprot_qeuqcb);
			    dprot_qeuqcb.qeuq_rtbl = &prot_tuple->dbp_tabid;
			    dprot_qeuqcb.qeuq_pno = prot_tuple->dbp_permno;

			    /*
			    ** tell qeu_dprot() to destroy a permit without
			    ** checking for abandoned objects
			    */
			    dprot_qeuqcb.qeuq_permit_mask =
				QEU_PERM | QEU_SKIP_ABANDONED_OBJ_CHECK;

			    /*
			    ** if permit is on a dbproc or a dbevent, tell
			    ** qeu_dprot() about it
			    */
			    if (prot_tuple->dbp_obtype == DBOB_DBPROC)
			    {
				DB_DBP_NAME	*p;

				dprot_qeuqcb.qeuq_permit_mask |=
				    QEU_DBP_PROTECTION;
				    
				dprot_qeuqcb.qeuq_dbp_tup = &dprot_proc_qefdata;
				p = (DB_DBP_NAME *) &prot_tuple->dbp_obname;
				
				STRUCT_ASSIGN_MACRO((*p),
				    proc_dprot_tuple->db_dbpname);
				    
				STRUCT_ASSIGN_MACRO(prot_tuple->dbp_obown,
				    proc_dprot_tuple->db_owner);
			    }
			    else if (prot_tuple->dbp_obtype == DBOB_EVENT)
			    {
				DB_NAME	    *p;

				dprot_qeuqcb.qeuq_permit_mask |=
				    QEU_EV_PROTECTION;
				    
				dprot_qeuqcb.qeuq_uld_tup = &dprot_ev_qefdata;
				p = (DB_NAME *) &prot_tuple->dbp_obname;
				
				STRUCT_ASSIGN_MACRO((*p),
				    ev_dprot_tuple->dbe_name);
				    
				STRUCT_ASSIGN_MACRO(prot_tuple->dbp_obown,
				    ev_dprot_tuple->dbe_owner);
			    }
			    else if (prot_tuple->dbp_obtype == DBOB_SEQUENCE)
			    {
				DB_NAME	    *p;

				dprot_qeuqcb.qeuq_permit_mask |=
				    QEU_SEQ_PROTECTION;
				    
				dprot_qeuqcb.qeuq_uld_tup = &dprot_seq_qefdata;
				p = (DB_NAME *) &prot_tuple->dbp_obname;
				
				STRUCT_ASSIGN_MACRO((*p),
				    seq_dprot_tuple->dbs_name);
				    
				STRUCT_ASSIGN_MACRO(prot_tuple->dbp_obown,
				    seq_dprot_tuple->dbs_owner);
			    }

			    dprot_qeuqcb.qeuq_flag_mask = 0;

			    status = qeu_dprot(qef_cb, &dprot_qeuqcb);
			    if (status != E_DB_OK)
			    {
				STRUCT_ASSIGN_MACRO(dprot_qeuqcb.error,
						    qeuq_cb->error);
				error = qeuq_cb->error.err_code;
				break;
			    }

			    /*
			    ** if there was a tree associated with the newly
			    ** destroyed permit, flush it out of RDF cache
			    ** (we assume that this could only happen for
			    ** table/view permits since GRANT does not allow for
			    ** a qualification, thus we do only need to set
			    ** RDR2_ALIAS in rdr_2types_mask)
			    */
			    if (   prot_tuple->dbp_treeid.db_tre_high_time
				|| prot_tuple->dbp_treeid.db_tre_low_time
			       )
			    {
				rdf_inv_rb->rdr_sequence =
				    prot_tuple->dbp_permno;
				rdf_inv_rb->rdr_2types_mask |= RDR2_ALIAS;
				rdf_inv_rb->rdr_types_mask  |= RDR_PROTECT;

				status = 
				    rdf_call(RDF_INVALIDATE, rdf_inv_cb);

				if (DB_FAILURE_MACRO(status))
				{
				    error = rdf_inv_cb->rdf_error.err_code;
				    break;
				}

				/*
				** reset RDR2_ALIAS in rdr_2types_mask and
				** RDR_PROTECT in rdr_types_mask to save
				** others the need to do it
				*/
				rdf_inv_rb->rdr_2types_mask &= ~RDR2_ALIAS;
				rdf_inv_rb->rdr_types_mask  &= ~RDR_PROTECT;
			    }
			}
			else
			{
			    /*
			    ** permit specifies privileges on a table or a view
			    ** and the privilege ability to grant which has been
			    ** lost by the permit's grantor applies to a subset
			    ** of attributes in the permit;
			    ** 
			    ** we will update the map of attributes to which
			    ** the permit applies and then call
			    ** qeu_construct_perm_text() to update the permits's
			    ** query text;
			    */
			    i4	    *lost_priv_map, *perm_map;

			    for (i = 0,
				 perm_map = prot_tuple->dbp_domset,
				 lost_priv_map = attrmap;

				 i < DB_COL_WORDS;
				 
				 i++, perm_map++, lost_priv_map++
				)
			    {
				*perm_map &= ~*lost_priv_map;
			    }

			    /*
			    ** now call qeu_replace() to replace IIPROTECT tuple
			    */
			    status = qeu_replace(qef_cb, prot_qeu);
			    if (status != E_DB_OK)
			    {
				error = prot_qeu->error.err_code;
				break;
			    }

			    /*
			    ** and finally, call qeu_construct_perm_text() to
			    ** reconstruct text of the permit based on the
			    ** permit tuple
			    */
			    status = qeu_construct_perm_text(qeuq_cb, qef_cb,
				qtxt_qeu, &ulm, prot_tuple, FALSE,
				(QEF_DATA **) NULL, &error);
			    if (status != E_DB_OK)
			    {
				break;
			    }
			}

			/*
			** if dropped or altered a permit on a table/view or
			** dbproc, remember to invalidate the RDF cache entry
			** for the object
			*/
			if (   !inv_obj_cache
			    && (   prot_tuple->dbp_obtype == DBOB_DBPROC
				|| tbl_priv_lost
			       )
			   )
			{
			    inv_obj_cache = TRUE;

			    /*
			    ** if the permit was defined on a dbproc, save
			    ** dbproc name and owner into rdf_inv_rb since
			    ** dbproc info is cached by name and owner (unlike
			    ** table info which is cached by id)
			    */
			    if (prot_tuple->dbp_obtype == DBOB_DBPROC)
			    {
				STRUCT_ASSIGN_MACRO(prot_tuple->dbp_obown,
						    rdf_inv_rb->rdr_owner);

				/*
				** can't use STRUCT_ASSIGN_MACRO because src
				** and dest are of different types
				*/
				MEcopy((PTR) &prot_tuple->dbp_obname,
				    sizeof(rdf_inv_rb->rdr_name.rdr_prcname),
				    (PTR) &rdf_inv_rb->rdr_name.rdr_prcname);
			    }
			}

			/*
			** cascading destruction is taking place;
			** 
			** Unless the current row indicates that the object's
			** owner lost GRANT OPTION FOR privilege (in which
			** case that privilege will be lost to all <auth id>s
			** and we will append a row to the temp table indicating
			** that PUBLIC lost the privilege and GRANT OPTION FOR
			** privilege after we are done processing all permits),
			** determine which privileges will be lost by the
			** grantee of the permit as a result of permit grantor's
			** having lost ability to grant privileges described
			** by *read_temp_row
			**
			** grantee of the permit may lose some privilege and, if
			** the permit specified GRANT OPTION, ability to grant
			** some privilege
			*/
			if (!priv_lost_by_owner)
			{
			    QEU_PERM_DEPTH_INFO	    *depth_info_p;
			    QEU_PDI_TBL_DESCR	    *pdi_tbl_descr_p;
			    
			    /* 
			    ** prot_tuple describes permit that was either 
			    ** destroyed or altered; 
			    ** *priv_revoked will describe privilege that 
			    ** was lost;
			    ** if permit specified privilege WGO, *gropt_revoked
			    ** will describe privilege GRANT OPTION FOR which 
			    ** was lost
			    */
			    priv_revoked = read_temp_row + 2;
			    STRUCT_ASSIGN_MACRO(prot_tuple->dbp_tabid, 
						priv_revoked->qeu_obj_id);

			    priv_revoked->qeu_priv = 
				prot_tuple->dbp_popset & ~DB_GRANT_OPTION;
			    if (priv_revoked->qeu_priv & DB_RETRIEVE)
				priv_revoked->qeu_priv &=
				    ~((i4) (DB_TEST | DB_AGGREGATE));

			    /*
			    ** if the current row contains a reminder to
			    ** invalidate QPs which may be affected by loss of 
			    ** specified privilege, propagate this reminder to
			    ** the potential new row
			    */
			    priv_revoked->qeu_flags = QEU_PRIV_LOST |
				(read_temp_row->qeu_flags & QEU_INVALIDATE_QPS);
			    
			    priv_revoked->qeu_gtype = prot_tuple->dbp_gtype;
			    STRUCT_ASSIGN_MACRO(prot_tuple->dbp_owner, 
						priv_revoked->qeu_authid);

			    /*
			    ** if object is not a dbproc or a dbevent, privilege
			    ** may be lost on a subset of attributes in the
			    ** permit tuple
			    */
			    if (tbl_priv_lost)
			    {
				i4	    *lost_priv_map, *priv_map;

				for (i = 0,
				     priv_map = priv_revoked->qeu_attrmap,
				     lost_priv_map = attrmap;

				     i < DB_COL_WORDS;
				     
				     i++, priv_map++, lost_priv_map++
				    )
				{
				    *priv_map = *lost_priv_map;
				}
			    }

			    if (prot_tuple->dbp_popset & DB_GRANT_OPTION)
			    {
			        gropt_revoked = read_temp_row + 3;
			        STRUCT_ASSIGN_MACRO((*priv_revoked), 
						    (*gropt_revoked));

				/*
				** if the current row contains a reminder to
				** invalidate QPs which may be affected by
				** loss of specified privilege, propagate
				** this reminder to the potential new row
				*/
				gropt_revoked->qeu_flags =
				    read_temp_row->qeu_flags & 
					QEU_INVALIDATE_QPS;
			        /* 
				** only GRANT statement allows creation of 
				** privileges WGO 
				*/
			    	gropt_revoked->qeu_flags |= 
				    QEU_GRANT_OPTION_LOST | 
					QEU_GRANT_COMPATIBLE_ONLY;

			    	priv_revoked->qeu_flags |= 
				    QEU_GRANT_COMPATIBLE_ONLY;

				/*
				** if
				**  - the permit we have just dropped or altered
				**    was defined on the table on which the user
				**    explicitly revoked privilege(s) and 
				**  - the privilege described by the permit
				**    is available to the grantee of the permit
				**    through other sources,
				** we want qeu_match_privs() to determine new
				** aggregate depth of the privilege and if it
				** greater than the depth of the permit that was
				** dropped/altered, add it to the temp table
				** which was created to keep track of such
				** privileges
				*/

				if (   prot_tuple->dbp_tabid.db_tab_base ==
				       pdi_tbl_descr->qeu_pdi_obj_id.db_tab_base
				    && prot_tuple->dbp_tabid.db_tab_index ==
				       pdi_tbl_descr->qeu_pdi_obj_id.
								db_tab_index)
				{
				    perm_depth_info->qeu_pdi_flags = 0;
				    perm_depth_info->qeu_old_depth =
					prot_tuple->dbp_depth;
				    perm_depth_info->qeu_new_depth =
					QEU_NEW_DEPTH_INVALID;
				    perm_depth_info->qeu_gtype =
					prot_tuple->dbp_gtype;
				    STRUCT_ASSIGN_MACRO(prot_tuple->dbp_owner,
					perm_depth_info->qeu_authid);
				    perm_depth_info->qeu_priv =
					prot_tuple->dbp_popset;

				    if (tbl_priv_lost)
				    {
					for (i = 0; i < DB_COL_WORDS; i++)
					{
					    perm_depth_info->qeu_attrmap[i] =
						attrmap[i];
					}
				    }

				    depth_info_p = perm_depth_info;
				    pdi_tbl_descr_p = pdi_tbl_descr;
				}
				else
				{
				    depth_info_p = (QEU_PERM_DEPTH_INFO *) NULL;
				    pdi_tbl_descr_p = (QEU_PDI_TBL_DESCR *)NULL;
				}
			    }
			    else
			    {
				bool		where_clause, term_specified;

				if (   prot_tuple->dbp_treeid.db_tre_high_time
				    || prot_tuple->dbp_treeid.db_tre_low_time)
				{
				    where_clause = TRUE;
				}
				else
				{
				    where_clause = FALSE;
				}

				if (STskipblank((char *) &prot_tuple->dbp_term,
					sizeof(DB_TERM_NAME)) != (char *) NULL)
				{
				    term_specified = TRUE;
				}
				else
				{
				    term_specified = FALSE;
				}

			        gropt_revoked = (QEU_REVOKE_INFO *) NULL;

				/*
				** since the privilege conveyed by the permit
				** which we dropped or altered was not
				** grantable, we do not want qeu_match_privs()
				** to compute aggregate depth of the privilege
				** which was conveyed to the grantee of the
				** permit even if it is available to him/her
				** through other channels
				*/
				depth_info_p = (QEU_PERM_DEPTH_INFO *) NULL;
				pdi_tbl_descr_p = (QEU_PDI_TBL_DESCR *) NULL;

			        /*
			        ** need to tell qeu_match_privs() whether 
				** the permit was GRANT-compatible
			        */
			        if (qeu_grant_compatible(prot_tuple,
					where_clause, term_specified))
			        {
				    priv_revoked->qeu_flags |= 
					QEU_GRANT_COMPATIBLE_ONLY;
			        }
			    }

			    /*
			    ** qeu_match_privs() will determine whether any
			    ** privileges and/or ability to grant any privileges
			    ** will be lost to the grantee of the current permit
			    **
			    ** it is important that we give qeu_match_privs() a
			    ** different qeu_cb from the one used to scan
			    ** permits since it will alter the current position
			    ** in the catalog
			    */
			    status = qeu_match_privs(qeuq_cb, qef_cb, prot2_qeu,
				priv_revoked, gropt_revoked, depth_info_p, 1,
				pdi_tbl_descr_p, prot_2tuple,
				&prot_tuple->dbp_obown, &error);

			    if (status != E_DB_OK)
			    {
				break;
			    }

			    /*
			    ** if qeu_match_privs() has determined that grantee
			    ** of the permit will lose some privileges and/or
			    ** ability to grant some privileges, append them to
			    ** our temporary table
			    */
			    if (   priv_revoked->qeu_priv
				|| (gropt_revoked && gropt_revoked->qeu_priv)
			       )
			    {
				status = qeu_add_lost_privs(qef_cb, temp_qeu,
				    priv_revoked, gropt_revoked, 
				    write_temp_row, 1);
				if (status != E_DB_OK)
				{
				    error = temp_qeu->error.err_code;
				    break;
				}
			    }
			}
		    }

		    if (status != E_DB_OK)
		    {
			break;
		    }
		    
		    if (priv_lost_by_owner)
		    {
			/*
			** if the current row indicates that we must try to
			** force invalidation of QPs which may be affected by
			** loss of GRANT OPTION FOR privilege by the view's
			** owner, attempt to determine id of some base table
			** used in the view's definition and, if successful,
			** alter that table's timestamp, then reset
			** QEU_PRIV_LOST_BY_OWNER in qeu_flags to avoid
			** unnecessarily altering timestamps when processing
			** loss of privileges by owners of views/dbprocs in
			** whose definition this view appears
			*/
			if (read_temp_row->qeu_flags & QEU_INVALIDATE_QPS)
			{
			    DB_TAB_ID		view_ubt_id;
			
			    status = qeu_v_ubt_id(qef_cb, 
				&read_temp_row->qeu_obj_id, &view_ubt_id, 
				&qeuq_cb->error);
			    if (DB_FAILURE_MACRO(status))
			    {
				err_already_reported = TRUE;
			        error = qeuq_cb->error.err_code;
				break;
			    }

			    if (view_ubt_id.db_tab_base)
			    {
				status = qeu_mdfy_timestamp(qef_cb,
				    &view_ubt_id, &error);
				if (DB_FAILURE_MACRO(status))
				{
				    break;
				}

				read_temp_row->qeu_flags &= ~QEU_INVALIDATE_QPS;
			    }
			}

			/* 
			** finally, append rows indicating that the privilege
			** and GRANT OPTION FOR privilege described in the
			** current row has been lost to PUBLIC 
			*/
			priv_revoked = read_temp_row + 2;
			gropt_revoked = read_temp_row + 3;
			
			STRUCT_ASSIGN_MACRO(read_temp_row->qeu_obj_id,
					    priv_revoked->qeu_obj_id);

			priv_revoked->qeu_priv = read_temp_row->qeu_priv;

			/* 
			** if we were told to, but were unable, to invalidate 
			** QPs which could be affected by loss of 
			** GRANT OPTION FOR this privilege by the view's owner,
			** propagate the reminder to the new rows being 
			** constructed
			*/
			priv_revoked->qeu_flags = 
			    read_temp_row->qeu_flags & QEU_INVALIDATE_QPS
				| QEU_PRIV_LOST | QEU_GRANT_COMPATIBLE_ONLY;

			priv_revoked->qeu_gtype = DBGR_PUBLIC;

			/*
		        ** if object is not a dbproc or a dbevent, privilege
		        ** may be lost on a subset of attributes in the
		        ** permit tuple
		        */
		        if (tbl_priv_lost)
		        {
			    i4	    *lost_priv_map, *priv_map;

			    for (i = 0,
			         priv_map = priv_revoked->qeu_attrmap,
			         lost_priv_map = read_temp_row->qeu_attrmap;

			         i < DB_COL_WORDS;
				     
			         i++, priv_map++, lost_priv_map++
			        )
			    {
			        *priv_map = *lost_priv_map;
			    }
		        }

			STRUCT_ASSIGN_MACRO((*priv_revoked), (*gropt_revoked));

			/* 
			** if we were told to, but were unable, to invalidate 
			** QPs which could be affected by loss of 
			** GRANT OPTION FOR this privilege by the view's owner,
			** propagate the reminder to the new rows being 
			** constructed
			*/
			gropt_revoked->qeu_flags =
			    read_temp_row->qeu_flags & QEU_INVALIDATE_QPS
				| QEU_GRANT_OPTION_LOST 
				| QEU_GRANT_COMPATIBLE_ONLY;

		        status = qeu_add_lost_privs(qef_cb, temp_qeu,
			    priv_revoked, gropt_revoked, write_temp_row, 1);
		        if (status != E_DB_OK)
		        {
			    error = temp_qeu->error.err_code;
			    break;
		        }
		    }
		}
		else if (read_temp_row->qeu_gtype == DBGR_PUBLIC)
		{
		    /*
		    ** look for permits which may be affected by PUBLIC 
		    ** losing some privilege (i.e. permits granted by 
		    ** users other than the object owner)
		    */
		    prot_qeu->qeu_getnext = QEU_REPO;
		    prot_qeu->qeu_klen = 2;
		    prot_qeu->qeu_count = 1;

		    while (status == E_DB_OK)
		    {
			status = qeu_get(qef_cb, prot_qeu);
			if (status != E_DB_OK)
			{
			    if (prot_qeu->error.err_code ==
				    E_QE0015_NO_MORE_ROWS)
			    {
				/* ran out of rows - we are done */
				status = E_DB_OK;
			    }
			    else
			    {
				error = prot_qeu->error.err_code;
			    }
			    break;
			}

			if (prot_qeu->qeu_getnext == QEU_REPO)
			{
			    prot_qeu->qeu_getnext = QEU_NOREPO;
			    prot_qeu->qeu_klen = 0;
			}

			/*
			** if this tuple represents an "access" permit or a
			** security_alarm, skip it
			*/
			if (prot_tuple->dbp_popset & (DB_CAN_ACCESS | DB_ALARM))
			    continue;

			/*
			** if this is an "older" (i.e. pre-6.5) permit, skip it
			*/
			if (~prot_tuple->dbp_flags & DBP_65_PLUS_PERM)
			    continue;
			
			/*
			** if the permit is not specifying privilege grant
			** option for which is being revoked, skip it
			*/
			if (!(prot_tuple->dbp_popset & read_temp_row->qeu_priv))
			    continue;

			/* if permit was granted by the object owner, skip it */
			if (!MEcmp((PTR) &prot_tuple->dbp_grantor,
			           (PTR) &prot_tuple->dbp_obown,
				   sizeof(prot_tuple->dbp_obown)))
			{
			    continue;
			}

			/*
			** if object on which privilege is being revoked is a
			** table or a view (or index?), attribute map of the
			** privilege being revoked should have non-empty
			** intersection with attribute map in the permit
			*/
			if (tbl_priv_lost)
			{
			    i4		*tmp_map, *perm_map, *res_map;
			    bool	found = FALSE;

			    for (i = 0,
				 tmp_map = read_temp_row->qeu_attrmap,
				 perm_map = prot_tuple->dbp_domset,
				 res_map = attrmap;

				 i < DB_COL_WORDS;
				 i++, tmp_map++, perm_map++, res_map++
			        )
			    {
				if ((*res_map = *tmp_map & *perm_map))
				    found = TRUE;
			    }

			    if (!found)
				continue;
			}

			/*
			** next we need to determine whether the grantor of the
			** permit no longer possesses some privileges and/or
			** ability to grant some privileges specified in the
			** permit; unlike the case when the privilege was lost
			** by a grantee (where, with exception of the list of
			** columns, we simply copied the permit tuple), we will
			** need to copy name of the grantor into dbp_owner to
			** indicate that we are interested in ability to grant
			** privileges by the grantor of this permit
			*/

			gropt_revoked = read_temp_row + 3;
			STRUCT_ASSIGN_MACRO(prot_tuple->dbp_tabid, 
					    gropt_revoked->qeu_obj_id);

			gropt_revoked->qeu_priv = 
			    prot_tuple->dbp_popset & ~DB_GRANT_OPTION;
			if (gropt_revoked->qeu_priv & DB_RETRIEVE)
			    gropt_revoked->qeu_priv &=
			        ~((i4) (DB_TEST | DB_AGGREGATE));

		        gropt_revoked->qeu_flags = 
			    QEU_GRANT_OPTION_LOST | QEU_GRANT_COMPATIBLE_ONLY;
		        gropt_revoked->qeu_gtype = DBGR_USER;
			STRUCT_ASSIGN_MACRO(prot_tuple->dbp_grantor, 
					    gropt_revoked->qeu_authid);

		        /*
		        ** if object is not a dbproc or a dbevent, privilege
		        ** may be lost on a subset of attributes in the
		        ** permit tuple
		        */
		        if (tbl_priv_lost)
		        {
			    i4	    *lost_priv_map, *priv_map;

			    for (i = 0, priv_map = gropt_revoked->qeu_attrmap,
				 lost_priv_map = attrmap;

				 i < DB_COL_WORDS;
				     
				 i++, priv_map++, lost_priv_map++
			        )
			    {
			        *priv_map = *lost_priv_map;
			    }
		        }

			/*
			** ask qeu_match_privs() to determine whether grantor of
			** the permit may not grant some privilege specified in
			** the permit
			**
			** it is important that we give qeu_match_privs() a
			** different qeu_cb from the one used to scan
			** permits since it will alter the current position
			** in the catalog
			*/
			status = qeu_match_privs(qeuq_cb, qef_cb, prot2_qeu,
			    (QEU_REVOKE_INFO *) NULL, gropt_revoked,
			    (QEU_PERM_DEPTH_INFO *) NULL, 1,
			    (QEU_PDI_TBL_DESCR *) NULL, prot_2tuple,
			    &prot_tuple->dbp_obown, &error);

			if (status != E_DB_OK)
			{
			    break;
			}

			/*
			** if qeu_match_privs() has determined that grantor of
			** the permit may still grant all of the privileges
			** named in the permit, we are done with this tuple
			*/
			if (!gropt_revoked->qeu_priv)
			    continue;

			/*
			** if we got here, this permit will have to be either
			** altered or destroyed; if the current <drop behaviour>
			** (as described by setting of QEU_DROP_CASCADE in
			** qeuq_cb->qeuq_flag_mask) is not CASCADE, report error
			** to the caller
			*/
			if (~qeuq_cb->qeuq_flag_mask & QEU_DROP_CASCADE)
			{
			    error = E_QE0250_ABANDONED_PERMIT;
			    status = E_DB_ERROR;
			    break;
			}

			/*
			** cascading destruction is taking place;
			** 
			** if the permit was on a table or view,
			** and its attribute map is not a subset of the
			** attribute map of the privilege which the grantor of
			** the permit may no longer grant, we will modify the
			** IIPROTECT tuple to only specify the privileges that
			** the grantor may grant and call
			** qeu_construct_perm_text() to generate permit text
			** corresponding to privileges granted by the permit;
			**
			** NOTE: qeu_match_privs() returned a map of
			**       attributes on which the grantor may not grant
			**       privileges specified in the permit inside
			**	 gropt_revoked->qeu_attrmap
			** 
			** otherwise we will simply destroy this permit
			** (remembering to tell qeu_dprot() to not pursue any
			** abandoned objects)
			**
			** dbproc, sequence and dbevent permits will be always destroyed
			**
			** NOTE: permits being destroyed or altered here CANNOT
			**       have trees associated with them because they
			**	 are granted by users other than the object
			**	 owner, which allows us to conclude that they
			**	 were not created using CREATE/DEFINE PERMIT
			*/

			if (   prot_tuple->dbp_obtype == DBOB_DBPROC
			    || prot_tuple->dbp_obtype == DBOB_EVENT
			    || prot_tuple->dbp_obtype == DBOB_SEQUENCE
			    || BTsubset((char *) prot_tuple->dbp_domset,
				(char *) gropt_revoked->qeu_attrmap,
				BITS_IN(gropt_revoked->qeu_attrmap))
			   )
			{
			    QEUQ_CB	dprot_qeuqcb;

			    STRUCT_ASSIGN_MACRO((*qeuq_cb), dprot_qeuqcb);
			    dprot_qeuqcb.qeuq_rtbl = &prot_tuple->dbp_tabid;
			    dprot_qeuqcb.qeuq_pno = prot_tuple->dbp_permno;

			    /*
			    ** tell qeu_dprot() to destroy a permit without
			    ** checking for abandoned objects
			    */
			    dprot_qeuqcb.qeuq_permit_mask =
				QEU_PERM | QEU_SKIP_ABANDONED_OBJ_CHECK;

			    /*
			    ** if permit is on a dbproc or a dbevent, tell
			    ** qeu_dprot() about it
			    */
			    if (prot_tuple->dbp_obtype == DBOB_DBPROC)
			    {
				DB_DBP_NAME	*p;

				dprot_qeuqcb.qeuq_permit_mask |=
				    QEU_DBP_PROTECTION;
				    
				dprot_qeuqcb.qeuq_dbp_tup = &dprot_proc_qefdata;
				p = (DB_DBP_NAME *) &prot_tuple->dbp_obname;
				
				STRUCT_ASSIGN_MACRO((*p),
				    proc_dprot_tuple->db_dbpname);
				    
				STRUCT_ASSIGN_MACRO(prot_tuple->dbp_obown,
				    proc_dprot_tuple->db_owner);
			    }
			    else if (prot_tuple->dbp_obtype == DBOB_EVENT)
			    {
				DB_NAME	    *p;

				dprot_qeuqcb.qeuq_permit_mask |=
				    QEU_EV_PROTECTION;
				    
				dprot_qeuqcb.qeuq_uld_tup = &dprot_ev_qefdata;
				p = (DB_NAME *) &prot_tuple->dbp_obname;
				
				STRUCT_ASSIGN_MACRO((*p),
				    ev_dprot_tuple->dbe_name);
				    
				STRUCT_ASSIGN_MACRO(prot_tuple->dbp_obown,
				    ev_dprot_tuple->dbe_owner);
			    }
			    else if (prot_tuple->dbp_obtype == DBOB_SEQUENCE)
			    {
				DB_NAME	    *p;

				dprot_qeuqcb.qeuq_permit_mask |=
				    QEU_SEQ_PROTECTION;
				    
				dprot_qeuqcb.qeuq_uld_tup = &dprot_seq_qefdata;
				p = (DB_NAME *) &prot_tuple->dbp_obname;
				
				STRUCT_ASSIGN_MACRO((*p),
				    seq_dprot_tuple->dbs_name);
				    
				STRUCT_ASSIGN_MACRO(prot_tuple->dbp_obown,
				    seq_dprot_tuple->dbs_owner);
			    }

			    dprot_qeuqcb.qeuq_flag_mask = 0;

			    status = qeu_dprot(qef_cb, &dprot_qeuqcb);
			    if (status != E_DB_OK)
			    {
				STRUCT_ASSIGN_MACRO(dprot_qeuqcb.error,
						    qeuq_cb->error);
				error = qeuq_cb->error.err_code;
				break;
			    }
			}
			else
			{
			    /*
			    ** permit specifies privileges on a table or a view
			    ** and the privilege being revoked applies to a
			    ** subset of attributes in the permit;
			    ** 
			    ** we will update the map of attributes to which
			    ** the permit applies and then call
			    ** qeu_construct_perm_text() to update the permits's
			    ** query text
			    ** if permit specifies a grantable privilege, call 
			    ** qeu_depth to recompute depth of permit(s) 
			    ** affected by this change
			    */
			    i4	    *lost_priv_map, *perm_map;

			    for (i = 0,
				 perm_map = prot_tuple->dbp_domset,
				 lost_priv_map = gropt_revoked->qeu_attrmap;

				 i < DB_COL_WORDS;
				 
				 i++, perm_map++, lost_priv_map++
				)
			    {
				*perm_map &= ~*lost_priv_map;
			    }

			    /*
			    ** now call qeu_replace() to replace IIPROTECT tuple
			    */
			    status = qeu_replace(qef_cb, prot_qeu);
			    if (status != E_DB_OK)
			    {
				error = prot_qeu->error.err_code;
				break;
			    }

			    /*
			    ** and finally, call qeu_construct_perm_text() to
			    ** reconstruct text of the permit based on the
			    ** permit tuple
			    */
			    status = qeu_construct_perm_text(qeuq_cb, qef_cb,
				qtxt_qeu, &ulm, prot_tuple, FALSE,
				(QEF_DATA **) NULL, &error);
			    if (status != E_DB_OK)
			    {
				break;
			    }
			}

			/*
			** if dropped or altered a permit on a table/view or
			** dbproc, remember to invalidate the RDF cache entry
			** for the object
			*/
			if (   !inv_obj_cache
			    && (   prot_tuple->dbp_obtype == DBOB_DBPROC
				|| tbl_priv_lost
			       )
			   )
			{
			    inv_obj_cache = TRUE;

			    /*
			    ** if the permit was defined on a dbproc, save
			    ** dbproc name and owner into rdf_inv_rb since
			    ** dbproc info is cached by name and owner (unlike
			    ** table info which is cached by id)
			    */
			    if (prot_tuple->dbp_obtype == DBOB_DBPROC)
			    {
				STRUCT_ASSIGN_MACRO(prot_tuple->dbp_obown,
						    rdf_inv_rb->rdr_owner);

				/*
				** can't use STRUCT_ASSIGN_MACRO because src
				** and dest are of different types
				*/
				MEcopy((PTR) &prot_tuple->dbp_obname,
				    sizeof(rdf_inv_rb->rdr_name.rdr_prcname),
				    (PTR) &rdf_inv_rb->rdr_name.rdr_prcname);
			    }
			}

			/*
			** determine which privileges will be lost by the
			** grantee of the permit as a result of permit grantor's
			** having lost ability to grant privileges described by
			** *gropt_revoked  (which came to describe these
			** privileges as a side-effect of calling
			** qeu_match_privs() - the only catch is that we
			** overwrote permit grantee name with permit grantor
			** name and grantee type with DBGR_USER to determine
			** which of the privileges the grantor may no longer
			** grant - so we will undo it here; 
			**
			** grantee of the permit may lose some privilege and, if
			** the permit specified GRANT OPTION, ability to grant
			** some privilege
			*/
			{
			    QEU_PERM_DEPTH_INFO     *depth_info_p;
			    QEU_PDI_TBL_DESCR       *pdi_tbl_descr_p;
			    
			    STRUCT_ASSIGN_MACRO(prot_tuple->dbp_owner,
						gropt_revoked->qeu_authid);
			    gropt_revoked->qeu_gtype = prot_tuple->dbp_gtype;

			    /* 
			    ** do not forget to propagate QEU_INVALIDATE_QPS
			    ** from the current row
			    */
			    gropt_revoked->qeu_flags |=
				read_temp_row->qeu_flags & QEU_INVALIDATE_QPS;

			    priv_revoked = read_temp_row + 2;
			    STRUCT_ASSIGN_MACRO((*gropt_revoked),
			    			(*priv_revoked));

			    /* 
			    ** overwrite flag from gropt_revoked -
			    ** priv_revoked will indicate that a privilege
			    ** may be lost + since it was granted by a user
			    ** other than the object's owner, it must be
			    ** GRANT-compatible + do not forget to propagate
			    ** QEU_INVALIDATE_QPS from the current row
			    */
			    priv_revoked->qeu_flags = 
				QEU_PRIV_LOST | QEU_GRANT_COMPATIBLE_ONLY |
				(read_temp_row->qeu_flags & QEU_INVALIDATE_QPS);

			    if (prot_tuple->dbp_popset & DB_GRANT_OPTION)
			    {
				/*
				** if
				**  - the permit we have just dropped or altered
				**    was defined on the table on which the user
				**    explicitly revoked privilege(s) and 
				**  - the privilege described by the permit
				**    is available to the grantee of the permit
				**    through other sources,
				** we want qeu_match_privs() to determine new
				** aggregate depth of the privilege and if it
				** greater than the depth of the permit that was
				** dropped/altered, add it to the temp table
				** which was created to keep track of such
				** privileges
				*/

				if (   prot_tuple->dbp_tabid.db_tab_base ==
				       pdi_tbl_descr->qeu_pdi_obj_id.db_tab_base
				    && prot_tuple->dbp_tabid.db_tab_index ==
				       pdi_tbl_descr->qeu_pdi_obj_id.
								db_tab_index)
				{
				    perm_depth_info->qeu_pdi_flags = 0;
				    perm_depth_info->qeu_old_depth =
					prot_tuple->dbp_depth;
				    perm_depth_info->qeu_new_depth =
					QEU_NEW_DEPTH_INVALID;
				    perm_depth_info->qeu_gtype =
					prot_tuple->dbp_gtype;
				    STRUCT_ASSIGN_MACRO(prot_tuple->dbp_owner,
					perm_depth_info->qeu_authid);
				    perm_depth_info->qeu_priv =
					prot_tuple->dbp_popset;

				    if (tbl_priv_lost)
				    {
					for (i = 0; i < DB_COL_WORDS; i++)
					{
					    perm_depth_info->qeu_attrmap[i] =
						gropt_revoked->qeu_attrmap[i];
					}
				    }

				    depth_info_p = perm_depth_info;
				    pdi_tbl_descr_p = pdi_tbl_descr;
				}
				else
				{
				    depth_info_p = (QEU_PERM_DEPTH_INFO *) NULL;
				    pdi_tbl_descr_p = (QEU_PDI_TBL_DESCR *)NULL;
				}
			    }
			    else
			    {
				gropt_revoked = (QEU_REVOKE_INFO *) NULL;
				
				/*
				** since the privilege conveyed by the permit
				** which we dropped or altered was not
				** grantable, we do not want qeu_match_privs()
				** to compute aggregate depth of the privilege
				** which was conveyed to the grantee of the
				** permit even if it is available to him/her
				** through other channels
				*/
				depth_info_p = (QEU_PERM_DEPTH_INFO *) NULL;
				pdi_tbl_descr_p = (QEU_PDI_TBL_DESCR *) NULL;

			    }

			    /*
			    ** qeu_match_privs() will determine whether any
			    ** privileges and/or ability to grant any privileges
			    ** will be lost to the grantee of the current permit
			    **
			    ** it is important that we give qeu_match_privs() a
			    ** different qeu_cb from the one used to scan
			    ** permits since it will alter the current position
			    ** in the catalog
			    */
			    status = qeu_match_privs(qeuq_cb, qef_cb, prot2_qeu,
				priv_revoked, gropt_revoked, depth_info_p, 1,
				pdi_tbl_descr_p, prot_2tuple,
				&prot_tuple->dbp_obown, &error);

			    if (status != E_DB_OK)
			    {
				break;
			    }

			    /*
			    ** if qeu_match_privs() has determined that grantee
			    ** of the permit will lose some privileges and/or
			    ** ability to grant some privileges, append them to
			    ** our temporary table
			    */
			    if (   priv_revoked->qeu_priv
				|| (gropt_revoked && gropt_revoked->qeu_priv))
			    {
				status = qeu_add_lost_privs(qef_cb, temp_qeu,
				    priv_revoked, gropt_revoked,
				    write_temp_row, 1);
				if (status != E_DB_OK)
				{
				    error = temp_qeu->error.err_code;
				    break;
				}
			    }
			}
		    }

		    if (status != E_DB_OK)
		    {
			break;
		    }
		}
	    }

	    /*
	    ** first part of the algorithm dealt with dependencies of permits
	    ** granted by users who do not themselves own an object on that
	    ** object; now we will deal with dependencies of objects (i.e.
	    ** dbprocs and views) on privileges on the objects used in their
	    ** definition + of permits on views and dbprocs on privileges on the
	    ** objects used in the definition of views and dbprocs + of
	    ** REFERENCES constraints on REFERENCES privilege on <referenced
	    ** columns>
	    */

	    if (read_temp_row->qeu_gtype == DBGR_USER)
	    {
		/*
		** privilege and/or ability to grant privilege was lost
		** by an <auth id>
		*/

		/*
		** if EXECUTE and/or ability to grant EXECUTE on a dbproc X was
		** lost by its owner, we will
		**   - read the IIPROCEDURE tuple (actually, we will read
		**     IIXPROCEDURE by dbproc id and then do a TID-join into
		**     IIPROCEDURE) for that dbproc; this will enable us to
		**     determine whether the dbproc is currently marked 
		**     grantable or active and to reset these bits appropriatly,
		**   - destroy all permits on X,
		**   - add a record to the temp table indicating that
		**     PUBLIC no longer possesses EXECUTE or GRANT OPTION FOR
		**     EXECUTE on X,
		**   - reset bits in DB_DBPGRANT_OK and DB_ACTIVE_DBP
		**     bits in IIPROCEDURE tuple describing X as appropriate,
		**   - scan IIDBDEPENDS looking for other dbprocs owned by 
		**     the same user which depend on X and for each such 
		**     dbproc X' we will record that the user may not 
		**     EXECUTE and/or grant EXECUTE on X' (depending on 
		**     what was lost on X)
		*/
		if (read_temp_row->qeu_priv & DB_EXECUTE && priv_lost_by_owner)
		{
		    i4  	upd_iiproc = 0;

		    xdbp_qeu->qeu_getnext = QEU_REPO;
		    xdbp_qeu->qeu_klen = 2;
		    xdbp_qeu->qeu_count = 1;

		    status = qeu_get(qef_cb, xdbp_qeu);
		    if (status != E_DB_OK)
		    {
			error = xdbp_qeu->error.err_code;
			break;
		    }

		    proc_qeu->qeu_tid = xdbp_tuple->dbx_tidp;
		    status = qeu_get(qef_cb, proc_qeu);
		    if (status != E_DB_OK)
		    {
			error = proc_qeu->error.err_code;
			break;
		    }

		    /*
		    ** if the dbproc is marked as active or grantable we
		    ** must reset one or both of these bits depending on
		    ** whether a user is losing a privilege or ability to
		    ** grant privilege;  if a dbproc which was marked
		    ** grantable no longer is grantable, we will destroy all
		    ** permits defined on it.
		    ** 
		    ** if RESTRICTed revocation was specified, and the
		    ** dbproc is marked as active and the owner of the
		    ** dbproc would no longer be able to execute his/her
		    ** dbproc, or it is marked grantable
		    ** and there is at least one outstanding permit defined
		    ** on it, return an error
		    */
		    if (   proc_tuple->db_mask[0] & DB_DBPGRANT_OK
		        && read_temp_row->qeu_flags & QEU_GRANT_OPTION_LOST)
		    {
		        /*
		        ** if caller requested RESTRICTed revocation, check
		        ** whether any permits will have to be dropped and
		        ** if so, report an error;
		        ** otherwise call qeu_dprot() to destroy all permits
		        ** on the dbproc WITHOUT checking for abandoned
		        ** objects
		        */
			    
		        if (~qeuq_cb->qeuq_flag_mask & QEU_DROP_CASCADE)
		        {
			    prot_qeu->qeu_getnext = QEU_REPO;
			    prot_qeu->qeu_klen = 2;
			    prot_qeu->qeu_count = 1;
 
			    status = qeu_get(qef_cb, prot_qeu);
			    if (status != E_DB_OK)
			    {
			        if (prot_qeu->error.err_code ==
				        E_QE0015_NO_MORE_ROWS)
			        {
				    /*
				    ** no permits defined on a dbproc - we
				    ** have nothing to complain about (yet)
				    */
				    status = E_DB_OK;
			        }
			        else
			        {
				    error = prot_qeu->error.err_code;
				    break;
			        }
			    }
			    else
			    {
			        /*
			        ** there exists at least one permit -
			        ** RESTRICTed revocation may not proceed
			        */
			        error = E_QE0252_ABANDONED_DBPROC;
			        status = E_DB_ERROR;
			        break;
			    }
		        }
		        else
		        {
			    QEUQ_CB	    dprot_qeuqcb;

			    STRUCT_ASSIGN_MACRO((*qeuq_cb), dprot_qeuqcb);
			    dprot_qeuqcb.qeuq_rtbl = &proc_tuple->db_procid;
			    dprot_qeuqcb.qeuq_pno = 0;

			    /*
			    ** tell qeu_dprot() to destroy all permits on a
			    ** dbproc without checking for abandoned objects
			    */
			    dprot_qeuqcb.qeuq_permit_mask =
			        QEU_PERM | QEU_SKIP_ABANDONED_OBJ_CHECK |
			        QEU_DBP_PROTECTION | QEU_DROP_ALL;

			    dprot_qeuqcb.qeuq_dbp_tup = &dprot_proc_qefdata;
			    STRUCT_ASSIGN_MACRO(proc_tuple->db_dbpname,
			        proc_dprot_tuple->db_dbpname);
			    STRUCT_ASSIGN_MACRO(proc_tuple->db_owner,
			        proc_dprot_tuple->db_owner);

			    dprot_qeuqcb.qeuq_flag_mask = 0;

			    status = qeu_dprot(qef_cb, &dprot_qeuqcb);
			    if (status != E_DB_OK)
			    {
			        STRUCT_ASSIGN_MACRO(dprot_qeuqcb.error,
						    qeuq_cb->error);
			        error = qeuq_cb->error.err_code;
			        break;
			    }
		        }

		        /* IIPROCEDURE tuple will need to be updated */
		        upd_iiproc |= DB_DBPGRANT_OK;

		        /*
		        ** finally, we need to record that PUBLIC may
		        ** neither execute the dbproc nor grant permits
		        ** on it
		        */
		        priv_revoked = read_temp_row + 2;
		        gropt_revoked = read_temp_row + 3;
			
		        STRUCT_ASSIGN_MACRO(read_temp_row->qeu_obj_id,
					    priv_revoked->qeu_obj_id);

		        STRUCT_ASSIGN_MACRO(read_temp_row->qeu_obj_id,
			    		    gropt_revoked->qeu_obj_id);

		        priv_revoked->qeu_priv = gropt_revoked->qeu_priv = 
			    read_temp_row->qeu_priv;

		        priv_revoked->qeu_flags = 
			    QEU_PRIV_LOST | QEU_GRANT_COMPATIBLE_ONLY
			    | QEU_INVALIDATE_QPS;

		        gropt_revoked->qeu_flags = QEU_GRANT_OPTION_LOST | 
			    QEU_GRANT_COMPATIBLE_ONLY | QEU_INVALIDATE_QPS;

		        priv_revoked->qeu_gtype = 
			    gropt_revoked->qeu_gtype = DBGR_PUBLIC;

		        status = qeu_add_lost_privs(qef_cb, temp_qeu,
			    priv_revoked, gropt_revoked, write_temp_row, 1);
		        if (status != E_DB_OK)
		        {
			    error = temp_qeu->error.err_code;
			    break;
		        }
		    }
			
		    if (   proc_tuple->db_mask[0] & DB_ACTIVE_DBP
		        && read_temp_row->qeu_flags & QEU_PRIV_LOST)
		    {
		        /*
		        ** if user requested that the RESTRICTed
		        ** revocation take place, we may not proceed
		        */
		        if (~qeuq_cb->qeuq_flag_mask & QEU_DROP_CASCADE)
		        {
			    error = E_QE0252_ABANDONED_DBPROC;
			    status = E_DB_ERROR;
			    break;
		        }

		        /*
		        ** remember that ACTIVE bit needs to be reset in
		        ** IIPROCEDURE
		        */
		        upd_iiproc |= DB_ACTIVE_DBP;
		    }

		    if (upd_iiproc != 0)
		    {
		        proc_tuple->db_mask[0] &= ~upd_iiproc;
		        status = qeu_replace(qef_cb, proc_qeu);
		        if (status != E_DB_OK)
		        {
			    error = proc_qeu->error.err_code;
			    break;
		        }

			/*
			** if the current row indicates that we must attempt
			** to invalidate QPs which may be affected by loss of
			** privilege on the dbproc, do so by altering the
			** timestamp of the dbproc's underlying base table whose
			** id (if any) has been stored in IIPROCEDURE tuple when
			** the dbproc was [re]created
			**
			** the one exception is when we are running UPGRADEDB in
			** which case we will not bother with trying to look up
			** dbproc's ubt_id because it is not going to be there
			*/
			if (   read_temp_row->qeu_flags & QEU_INVALIDATE_QPS
			    && proc_tuple->db_dbp_ubt_id.db_tab_base
			    && ~qef_cb->qef_sess_flags & QEF_RUNNING_UPGRADEDB)
			{
			    status = qeu_mdfy_timestamp(qef_cb,
			        &proc_tuple->db_dbp_ubt_id, &error);
			    if (DB_FAILURE_MACRO(status))
			    {
				break;
			    }
			}

			/* destroy the dbproc's QP */
			status = qeu_destr_dbp_qp(qef_cb, &proc_tuple->db_owner,
			    &proc_tuple->db_dbpname, &qeuq_cb->error);
		        if (DB_FAILURE_MACRO(status))
		        {
			    err_already_reported = TRUE;
			    error = qeuq_cb->error.err_code;
			    break;
		        }

		        /*
		        ** at the very least, we have reset the bit that
		        ** indicated that this dbproc is grantable or
		        ** active - remember to flush its description out of
		        ** RDF cache
		        */
		        if (!inv_obj_cache)
		        {
			    inv_obj_cache = TRUE;

			    /*
			    ** save dbproc name and owner into rdf_inv_rb
			    ** since dbproc info is cached by name and owner
			    ** (unlike table info which is cached by id)
			    */
			    STRUCT_ASSIGN_MACRO(proc_tuple->db_owner,
					        rdf_inv_rb->rdr_owner);
			    STRUCT_ASSIGN_MACRO(proc_tuple->db_dbpname,
			        rdf_inv_rb->rdr_name.rdr_prcname);
		        }
		    }
		    else
		    {
		        /*
		        ** nothing in the status of the dbproc changed - we
		        ** are done with this temp table row
		        */
		        continue;
		    }

		    /*
		    ** now scan all IIDBDEPENDS tuples describing
		    ** dbprocs owned by this user which depend on his/her
		    ** dbproc for which he lost EXECUTE or the ability to
		    ** grant EXECUTE; for each such dbproc we will enter a
		    ** row into the temporary table indicating that the
		    ** dbproc owner no longer possesses (depending on the
		    ** setting of read_temp_row->qeu_flags) EXECUTE or
		    ** GRANT OPTION FOR EXECUTE on the dbproc
		    */

		    /* first set up gropt_revoked */
		    gropt_revoked = read_temp_row + 3;

		    gropt_revoked->qeu_priv = read_temp_row->qeu_priv;

		    /*
		    ** remember that GRANT OPTION FOR privilege is lost by
		    ** the dbproc owner + that once we drop all permits on this
		    ** dbproc, we must attempt to invalidate QPs affected by
		    ** cascading destruction of privileges
		    */
		    gropt_revoked->qeu_flags = 
		        QEU_GRANT_OPTION_LOST | QEU_GRANT_COMPATIBLE_ONLY |
			QEU_INVALIDATE_QPS | QEU_PRIV_LOST_BY_OWNER;

		    gropt_revoked->qeu_gtype = DBGR_USER;
		    STRUCT_ASSIGN_MACRO(read_temp_row->qeu_authid,
		        gropt_revoked->qeu_authid);

		    /*
		    ** at the very least the user will lose ability to
		    ** grant privilege on his dbprocs; if the user lost
		    ** EXECUTE privilege on the current dbproc, then he will
		    ** be losing it on all dbprocs owned by him which invoke
		    ** the current dbproc
		    */
		    if (upd_iiproc & DB_ACTIVE_DBP)
		    {
		        priv_revoked = read_temp_row + 2;

		        priv_revoked->qeu_priv = read_temp_row->qeu_priv;

		        /*
		        ** remember that GRANT OPTION FOR privilege is lost by
		        ** the dbproc owner + that once we drop all permits 
			** on this dbproc, we must attempt to invalidate QPs 
			** affected by cascading destruction of privileges
		        */
		        priv_revoked->qeu_flags =
			    QEU_PRIV_LOST | QEU_GRANT_COMPATIBLE_ONLY |
			    QEU_INVALIDATE_QPS | QEU_PRIV_LOST_BY_OWNER;

		        priv_revoked->qeu_gtype = DBGR_USER;
		        STRUCT_ASSIGN_MACRO(read_temp_row->qeu_authid,
					    priv_revoked->qeu_authid);
		    }
		    else
		    {
		        priv_revoked = (QEU_REVOKE_INFO *) NULL;
		    }

		    dep_qeu->qeu_getnext = QEU_REPO;
		    dep_qeu->qeu_count = 1;
		    dep_qeu->qeu_klen = 3;
		    STRUCT_ASSIGN_MACRO(read_temp_row->qeu_obj_id, dep_id);
		    indep_obj_type = DB_DBP;
			
		    for (;;)
		    {
		        status = qeu_get(qef_cb, dep_qeu);
		        if (status != E_DB_OK)
		        {
			    if ((error = dep_qeu->error.err_code) ==
			             E_QE0015_NO_MORE_ROWS)
			    {
			        /* no more rows - we are done */
			        status = E_DB_OK;
			    }
			    break;
		        }

		        if (dep_qeu->qeu_getnext == QEU_REPO)
		        {
			    dep_qeu->qeu_getnext = QEU_NOREPO;
			    dep_qeu->qeu_klen = 0;
		        }

		        /*
		        ** gropt_revoked and, if appropriate,
		        ** priv_revoked have been preinitialized except
		        ** for the dependent dbproc id which we now know
		        */
		        STRUCT_ASSIGN_MACRO(dep_tuple->dbv_dep,
			    gropt_revoked->qeu_obj_id);
		        if (priv_revoked)
		        {
			    STRUCT_ASSIGN_MACRO(dep_tuple->dbv_dep,
			        priv_revoked->qeu_obj_id);
		        }
			    
		        status = qeu_add_lost_privs(qef_cb, temp_qeu,
			    priv_revoked, gropt_revoked, write_temp_row, 1);
		        if (status != E_DB_OK)
		        {
			    error = temp_qeu->error.err_code;
			    break;
		        }
		    }
		    if (status != E_DB_OK)
		    {
		        break;
		    }

		    /* we are done with this temp table row */
		    continue;
		}

		/* 
		** now we will look in IIPRIV (by keying into IIXPRIV and
		** doing a TID-join into IIPRIV) for objects and permits
		** which depend on the privilege and/or ability to grant
		** privilege being lost
		*/
		xpr_qeu->qeu_getnext = QEU_REPO;
		xpr_qeu->qeu_klen = 3;
		xpr_qeu->qeu_count = 1;

		/* 
		** first set up priv_revoked and gropt_revoked;
		*/
		priv_revoked = read_temp_row + 2;
		gropt_revoked = read_temp_row + 3;

		/*
		** set qeu_flags in priv_revoked and gropt_revoked:
		** - set QEU_PRIV_LOST_BY_OWNER since loss of privileges on
		**   which an object or permit depends results in loss of
		**   corresponding privilege by the owner of the object
		** - set QEU_INVALIDATE_QPS if it is set in the current row or
		**   if the current row describes a privilege lost on a dbproc
		**   (note that this cannot be a privilege lost by the
		**   dbproc's owner since such cases were handled above)
		** - QEU_PRIV_LOST will be set in priv_revoked->qeu_flags
		** - QEU_GRANT_OPTION_LOST will be set in
		**   gropt_revoked->qeu_flags
		*/
		if (read_temp_row->qeu_priv == DB_EXECUTE)
		{
		    priv_revoked->qeu_flags = gropt_revoked->qeu_flags = 
			QEU_INVALIDATE_QPS;
		}
		else
		{
		    priv_revoked->qeu_flags = gropt_revoked->qeu_flags =
			read_temp_row->qeu_flags & QEU_INVALIDATE_QPS;
		}

		priv_revoked->qeu_flags |= 
		    QEU_PRIV_LOST | QEU_PRIV_LOST_BY_OWNER;
		gropt_revoked->qeu_flags |= 
		    QEU_GRANT_OPTION_LOST | QEU_PRIV_LOST_BY_OWNER;

		priv_revoked->qeu_gtype = gropt_revoked->qeu_gtype = 
		    read_temp_row->qeu_gtype;

		STRUCT_ASSIGN_MACRO(read_temp_row->qeu_authid,
				    priv_revoked->qeu_authid);
		STRUCT_ASSIGN_MACRO(read_temp_row->qeu_authid,
				    gropt_revoked->qeu_authid);

		/*
		** in the cases when a dbproc depends on a privilege which is 
		** being revoked or ability to grant which is being revoked, 
		** we will need to record that the owner of the dbproc lost 
		** EXECUTE and/or ability to grant EXECUTE on the dbproc 
		** 
		** in the cases when a view depends on a privilege (i.e. SELECT)
		** GRANT OPTION FOR which is being revoked, we will need to 
		** record that the owner of the view lost GRANT OPTION FOR 
		** SELECT on his view
		** 
		** in the cases when a view depends on a privilege (i.e. SELECT)
		** which is being revoked, we will call qeu_dview() to
		** destroy it and will not need to enter anything into the temp
		** table
		**
		** Similarly, if a privilege in which a constraint depends (i.e.
		** REFERENCES) is being revoked, we will call qeu_d_cascade() to
		** destroy the constraint and all objects which were created to
		** enforce it
		*/

		while (status == E_DB_OK)
		{
		    DB_PROTECTION		*prot_p;

		    status = qeu_get(qef_cb, xpr_qeu);
		    if (status != E_DB_OK)
		    {
		        if (xpr_qeu->error.err_code == E_QE0015_NO_MORE_ROWS)
		        {
			    /* ran out of rows - we are done */
			    status = E_DB_OK;
		        }
		        else
		        {
			    error = xpr_qeu->error.err_code;
		        }
		        break;
		    }

		    if (xpr_qeu->qeu_getnext == QEU_REPO)
		    {
		        xpr_qeu->qeu_getnext = QEU_NOREPO;
		        xpr_qeu->qeu_klen = 0;
		    }

		    /* 
		    ** IIXPRIV is keyed on independent object id and
		    ** privilege, but it also carries the <auth id> of
		    ** the owner of the object or permit to which the privilege
		    ** must be granted; before we proceed, check whether that
		    ** name matches read_temp_row->qeu_authid; if not, there is
		    ** no reason to proceed
		    */
		    if (MEcmp((PTR) &xpr_tuple->dbx_grantee,
			    (PTR) &read_temp_row->qeu_authid,
			    sizeof(xpr_tuple->dbx_grantee)))
		    {
			continue;
		    }

		    /*
		    ** Now do a TID-join into IIPRIV to get the
		    ** information on the attributes to which the privileges
		    ** must apply (for tables and views only) and the object
		    ** which depends on the <auth id> possessing a privilege
		    ** which is being revoked from him/her
		    */
		    priv_qeu->qeu_tid = xpr_tuple->dbx_tidp;
		    status = qeu_get(qef_cb, priv_qeu);
		    if (status != E_DB_OK)
		    {	
			error = priv_qeu->error.err_code;
			break;
		    }

		    /* 
		    ** if the independent privilege is defined on a table
		    ** or a view, verify that the intersection of the map 
		    ** of attributes privilege and/or ability to grant 
		    ** privilege on which is being revoked and the map in 
		    ** IIPRIV tuple is non-empty; otherwise the
		    ** revocation being processed will have no effect on the
		    ** object or permit described in the IIPRIV tuple
		    */

		    if (tbl_priv_lost)
		    {
			i4		*tmp_map, *indep_map, *res_map;
			bool		found = FALSE;

			for (i = 0,
			     tmp_map = read_temp_row->qeu_attrmap,
			     indep_map = priv_tuple->db_priv_attmap,
			     res_map = attrmap;

			     i < DB_COL_WORDS;

			     i++, tmp_map++, indep_map++, res_map++
			    )
		        {
			    if ((*res_map = *tmp_map & *indep_map))
			        found = TRUE;
		        }

		        if (!found)
			    continue;
		    }

		    /* 
		    ** if this point is reached, we know that the privilege
		    ** being revoked will have some effect on the object, permit
		    ** or constraint (for constraint this will be the case only
		    ** if the user lost the privilege and not the ability to
		    ** grant the privilege) which depends on it (it is not 
		    ** necessarily the case with dbprocs where we will wait
		    ** until the temp table row describing privilege and or
		    ** ability to grant privilege which was lost by the dbproc's
		    ** owner is read to determine whether the privilege and/or
		    ** ability to grant privilege which are being lost by the
		    ** dbproc owner will have some effect on the dbproc)
		    **
		    ** if the descriptor number is 0, we must be dealing
		    ** with dependence of a dbproc or a view on a privilege
		    ** on an object; otherwise we must be dealing with a
		    ** dependence of a permit on a view on a privilege on
		    ** the underlying table or of constraint on a privilege
		    ** on the referenced table
		    */
		    if (   priv_tuple->db_descr_no == 0
			&& priv_tuple->db_dep_obj_type == DB_DBP)
		    {
			QEU_REVOKE_INFO		*info_p;

			/*
			** if the user lost a privilege on which a dbproc
			** depends, add a record to the temp table
			** indicating that the owner of the dbproc lost
			** EXECUTE and GRANT OPTION FOR EXECUTE on his
			** dbproc; otherwise (user lost GRANT OPTION FOR
			** privilege on which a dbproc depends), add a
			** record to the temp table indicating that the
			** owner of the dbproc lost GRANT OPTION FOR EXECUTE
			** on his dbproc
			*/
			STRUCT_ASSIGN_MACRO(priv_tuple->db_dep_obj_id,
			    gropt_revoked->qeu_obj_id);
			gropt_revoked->qeu_priv = DB_EXECUTE;
			gropt_revoked->qeu_flags |= QEU_GRANT_COMPATIBLE_ONLY;

			if (read_temp_row->qeu_flags & QEU_PRIV_LOST)
			{
			    info_p = priv_revoked;
			    STRUCT_ASSIGN_MACRO(priv_tuple->db_dep_obj_id,
						info_p->qeu_obj_id);
			    info_p->qeu_priv = DB_EXECUTE;
			    info_p->qeu_flags |= QEU_GRANT_COMPATIBLE_ONLY;
			}
			else
			{
			    info_p = (QEU_REVOKE_INFO *) NULL;
			}

		        status = qeu_add_lost_privs(qef_cb, temp_qeu,
			    info_p, gropt_revoked, write_temp_row, 1);
		        if (status != E_DB_OK)
		        {
			    error = temp_qeu->error.err_code;
			    break;
		        }
		    }
		    else if (   priv_tuple->db_descr_no == 0
			     && priv_tuple->db_dep_obj_type == DB_VIEW
			    )
		    {
			/*
			** if (a user lost privilege (i.e. SELECT) on which a
			**     view depends)
			** {
			**   if (the view was defined in QUEL and only
			**       GRANT-compatible SELECT is being revoked)
			**   {
			**     check whether there are some permits conveying
			**     non-GRANT-compatible privileges that would keep
			**     this view from becoming abandoned
			**   }
			**
			**   if (the view is abandoned)
			**   {
			**     we will invoke qeu_d_cascade() to perform
			**     cascading destruction of a view and will not add
			**     any records to the temp table
			**   }
			** }
			** else	    // user lost GRANT OPTION FOR privilege 
			**	    // (i.e. SELECT) on which a view depends
			** {
			**   we will add a record to the temp table indicating
			**   that the view's owner lost GRANT OPTION FOR SELECT
			**   on the view
			** }
			*/
			if (read_temp_row->qeu_flags & QEU_PRIV_LOST)
			{
			    QEUQ_CB		dview_qeuqcb;

			    /*
			    ** if the view was defined using QUEL and only
			    ** GRANT-compatible privilege is getting revoked,
			    ** determine whether there are some permit(s)
			    ** conveying non-GRANT-compatible privileges which
			    ** would keep the view from getting abandoned
			    */
			    if (   ~priv_tuple->db_prv_flags &
					DB_GRANT_COMPATIBLE_PRIV 
			        && read_temp_row->qeu_flags &
					QEU_GRANT_COMPATIBLE_ONLY)
			    {
				priv_revoked->qeu_priv =
				    read_temp_row->qeu_priv;
				priv_revoked->qeu_flags &= 
				    ~QEU_GRANT_COMPATIBLE_ONLY;
				for (
				     i = 0,
				     map_from = priv_tuple->db_priv_attmap,
				     map_to = priv_revoked->qeu_attrmap;

				     i < DB_COL_WORDS;

				     i++, map_from++, map_to++
				    )
				{
				    *map_to = *map_from;
				}

				status = qeu_match_privs(qeuq_cb, qef_cb,
				    prot2_qeu, priv_revoked,
				    (QEU_REVOKE_INFO *) NULL,
				    (QEU_PERM_DEPTH_INFO *) NULL, 1,
				    (QEU_PDI_TBL_DESCR *) NULL, prot_2tuple,
				    (DB_OWN_NAME *) NULL, &error);

				if (status != E_DB_OK)
				{
				    break;
				}

				/*
				** if the owner of this QUEL view possess
				** sufficient non-GRANT-compatible privilege to
				** keep the view from becoming abandoned, we
				** will not destroy the view - we are done with
				** this descriptor
				*/
				if (priv_revoked->qeu_priv == 0)
				    continue;
			    }
			    
			    STRUCT_ASSIGN_MACRO(priv_tuple->db_dep_obj_id, 
				dmt_show.dmt_tab_id);

			    status = dmf_call(DMT_SHOW, &dmt_show);
			    if (   status != E_DB_OK 
				&& dmt_show.error.err_code != 
					E_DM0114_FILE_NOT_FOUND)
			    {
				error = dmt_show.error.err_code;
				break;
			    }

			    /* 
			    ** owner of the view no longer possesses
			    ** privilege on which a view depends - if
			    ** RESTRICTed revocation was specified, return
			    ** error; otherwise call qeu_d_cascade() to perform
			    ** cascading destruction of the view
			    */
			    if (~qeuq_cb->qeuq_flag_mask & QEU_DROP_CASCADE)
			    {
				error = E_QE0251_ABANDONED_VIEW;
				status = E_DB_ERROR;
				break;
			    }

			    status = E_DB_OK;

			    STRUCT_ASSIGN_MACRO((*qeuq_cb), dview_qeuqcb);

			    /*
			    ** if we were told to force invalidation of QPs
			    ** affected by cascading destruction of privielges,
			    ** tell qeu_d_cascade() to try to invalidate QPs
			    ** dependent on the view being dropped
			    */
			    if (read_temp_row->qeu_flags & QEU_INVALIDATE_QPS)
			    {
				dview_qeuqcb.qeuq_flag_mask |=
				    QEU_FORCE_QP_INVALIDATION;
			    }

			    dview_qeuqcb.qeuq_rtbl = &priv_tuple->db_dep_obj_id;
			    STRUCT_ASSIGN_MACRO(dmt_tbl_entry.tbl_qryid,
				dview_qeuqcb.qeuq_qid);
			    
			    /*
			    ** qeu_d_cascade will take care of invalidating RDF
			    ** cache entries for all objects that get destroyed
			    ** (or, in case of database procedures, reclassified
			    ** to dormant)
			    */
			    status = qeu_d_cascade(qef_cb, &dview_qeuqcb,
				(DB_QMODE) DB_VIEW, &dmt_tbl_entry);
			    if (status != E_DB_OK)
			    {
				error = dview_qeuqcb.error.err_code;
				break;
			    }
			}
			else
			{
			    /* 
			    ** user loses GRANT OPTION FOR SELECT on
			    ** his/her view
			    */
			    STRUCT_ASSIGN_MACRO(priv_tuple->db_dep_obj_id,
				gropt_revoked->qeu_obj_id);
			    gropt_revoked->qeu_priv = DB_RETRIEVE;
			    gropt_revoked->qeu_flags |=
			        QEU_GRANT_COMPATIBLE_ONLY;
			    /* 
			    ** set all bits in the attribute map to
			    ** indicate that ability to grant SELECT is lost 
			    ** on the view and on its every column
			    */
			    for (i = 0; i < DB_COL_WORDS; i++)
				gropt_revoked->qeu_attrmap[i] = ~((i4) 0);

			    status = qeu_add_lost_privs(qef_cb, temp_qeu,
				(QEU_REVOKE_INFO *) NULL, gropt_revoked, 
				write_temp_row, 1);
			    if (status != E_DB_OK)
			    {
				error = temp_qeu->error.err_code;
				break;
			    }
			}
		    }
		    else if (priv_tuple->db_dep_obj_type == DB_CONS)
		    { 
			QEUQ_CB	    drop_cons_qeuqcb;

			if (~read_temp_row->qeu_flags & QEU_PRIV_LOST)
			{
			    /*
			    ** if the user only lost ability to grant privilege
			    ** on which a constraint depends, constraint will
			    ** not be affected, so skip to the next temp table
			    ** row
			    */
			    continue;
			}
			
			/* 
			** user lost REFERENCES privilege on some or all
			** <referenced columns> of a constraint - the
			** constraints will become abandoned
			**
			** first we need to determine number of the constraint
			** which depends on the privilege - this requires that
			** we do a join into IIDBDEPENDS to get integrity number
			*/
			dep_qeu->qeu_getnext = QEU_REPO;
			dep_qeu->qeu_count = 1;
			dep_qeu->qeu_klen = 4;
			STRUCT_ASSIGN_MACRO(priv_tuple->db_dep_obj_id, dep_id);
			indep_obj_type = DB_PRIV_DESCR;
			priv_descr_no = priv_tuple->db_descr_no;

			status = qeu_get(qef_cb, dep_qeu);
			if (status != E_DB_OK)
			{
			    error = dep_qeu->error.err_code;
			    break;
			}
			
			/*
			** if RESTRICTed revocation was specified, we will
			** report an error; otherwise we will call
			** qeu_d_cascade() to destroy the constraint and all
			** objects that depend on it
			*/
			if (~qeuq_cb->qeuq_flag_mask & QEU_DROP_CASCADE)
			{
			    error = E_QE0257_ABANDONED_CONSTRAINT;
			    status = E_DB_ERROR;
			    break;
			}

			STRUCT_ASSIGN_MACRO((*qeuq_cb), drop_cons_qeuqcb);

			/*
			** if we were told to force invalidation of QPs
			** affected by cascading destruction of privielges,
			** tell qeu_d_cascade() to try to invalidate QPs
			** which may depend on the constraint being dropped
			*/
			if (read_temp_row->qeu_flags & QEU_INVALIDATE_QPS)
			{
			    drop_cons_qeuqcb.qeuq_flag_mask |=
			        QEU_FORCE_QP_INVALIDATION;
		        }

			drop_cons_qeuqcb.qeuq_rtbl = &priv_tuple->db_dep_obj_id;
			drop_cons_qeuqcb.qeuq_ino = dep_tuple->dbv_qid;
			
			status = qeu_d_cascade(qef_cb, &drop_cons_qeuqcb,
			    (DB_QMODE) DB_CONS, (DMT_TBL_ENTRY *) NULL);
			if (status != E_DB_OK)
			{
			    error = drop_cons_qeuqcb.error.err_code;
			    break;
			}
		    }
		    else
		    {
			/* 
			** user lost a privilege and/or GRANT OPTION FOR
			** privilege on a view's underlying table on which a
			** permit on a view depends
			**
			** first determine number of the permit which
			** depends on the privilege - this requires that
			** we do a join into IIDBDEPENDS to get a permit
			** number - and then scan IIPROTECT to find that permit
			**
			** Note that there could be multiple permits on the
			** view which depend on this privilege descriptor, but
			** it is sufficient that we find just one of them since
			** that will enable us to determine which privileges the
			** owner of the object may no longer grant on his view
			** and record it in the temporary table;
			** if a privilege is being revoked on a subset of
			** attributes listed in the privilege descriptor, we
			** will update the descriptor here and will take care of
			** permits when we get to the row in the temporary table
			** that indicates that the owner of the object may not
			** grant privilege conveyed by the permits
			*/
			dep_qeu->qeu_getnext = QEU_REPO;
			dep_qeu->qeu_count = 1;
			dep_qeu->qeu_klen = 4;
			STRUCT_ASSIGN_MACRO(priv_tuple->db_dep_obj_id, dep_id);
			indep_obj_type = DB_PRIV_DESCR;
			priv_descr_no = priv_tuple->db_descr_no;

			status = qeu_get(qef_cb, dep_qeu);
			if (status != E_DB_OK)
			{
			    error = dep_qeu->error.err_code;
			    break;
			}
			
			/* 
			** now scan IIPROTECT looking for a permit with
			** specified number
			**
			** CAUTION: we will reset obj_id (key used when
			** accessing IIPROTECT through prot_qeu and
			** IIPROCEDURE through xdbp_qeu);  while it is safe
			** to do it here, anyone (myself included) making future
			** changes should be careful not to foul things up
			*/
			STRUCT_ASSIGN_MACRO(priv_tuple->db_dep_obj_id, obj_id);
			prot_qeu->qeu_getnext = QEU_REPO;
			prot_qeu->qeu_klen = 2;
			prot_qeu->qeu_count = 1;

			for (;;) 
			{ 
			    status = qeu_get(qef_cb, prot_qeu); 
			    if (status != E_DB_OK) 
			    {
				error = prot_qeu->error.err_code;
				break;
			    }

			    if (prot_qeu->qeu_getnext == QEU_REPO)
			    {
				prot_qeu->qeu_getnext = QEU_NOREPO;
				prot_qeu->qeu_klen = 0;
			    }

			    if (prot_tuple->dbp_permno == dep_tuple->dbv_qid)
			    {
				/* we found our permit */
				break;
			    }
			}

			if (status != E_DB_OK)
			    break;

			/* 
			** if RESTRICTed revocation was specified, report error
			*/
			if (~qeuq_cb->qeuq_flag_mask & QEU_DROP_CASCADE)
			{
			    error = E_QE0250_ABANDONED_PERMIT;
			    status = E_DB_ERROR;
			    break;
			}

			STRUCT_ASSIGN_MACRO(prot_tuple->dbp_tabid,
			    gropt_revoked->qeu_obj_id);
			gropt_revoked->qeu_priv =
			    prot_tuple->dbp_popset & ~((i4) DB_GRANT_OPTION);
			gropt_revoked->qeu_flags |= QEU_GRANT_COMPATIBLE_ONLY;
			    
			if (BTsubset((char *) priv_tuple->db_priv_attmap,
				(char *) read_temp_row->qeu_attrmap,
				BITS_IN(read_temp_row->qeu_attrmap)))
			{
			    for (
				 i = 0, map_from = prot_tuple->dbp_domset,
				 map_to = gropt_revoked->qeu_attrmap;

				 i < DB_COL_WORDS;

				 i++, map_from++, map_to++
				)
			    {
				*map_to = *map_from;
			    }
			}
			else
			{
			    /*
			    ** privilege is being lost on a subset of attributes
			    ** listed in the privilege descriptor; we need to
			    **   - update the map of attributes in the privilege
			    **	   descriptor and write it out to disk and
			    **	 - invoke (*qeuq_cb->qeuq_b2v_col_xlate)() to
			    **	   determine on which attributes of the view the
			    **	   user may no longer grant the privilege
			    */
			    for (i = 0; i < DB_COL_WORDS; i++)
				priv_tuple->db_priv_attmap[i] &= ~attrmap[i];
			    status = qeu_replace(qef_cb, priv_qeu);
			    if (status != E_DB_OK)
			    {
				error = priv_qeu->error.err_code;
				break;
			    }
			    
			    status = (*qeuq_cb->qeuq_b2v_col_xlate)(
				&priv_tuple->db_dep_obj_id,
				gropt_revoked->qeu_attrmap,
				&priv_tuple->db_indep_obj_id, attrmap);
			    if (DB_FAILURE_MACRO(status))
			    {
				error = E_QE0002_INTERNAL_ERROR;
				break;
			    }
			}

			/*
			** NOTE: it is safe to be telling qeu_add_lost_privs()
			**       that the privilege being lost is
			**	 GRANT-compatible since the privilege being lost
			**	 would not be SELECT and the only time when this
			**	 becomes important is when we try to determine
			**	 whether a QUEL view has become abandoned (i.e.
			**	 whether there are some permits conveying
			**	 non-GRANT-compatible SELECT which would keep
			**	 the view  from becoming abandoned)
			**
			** NOTE 2: we are recording that the owner of the view
			**         may not grant privilege on his view but not
			**	   that he may not perform action on the view
			**	   because this latter piece of information is
			**	   of no benefit since no catalog describes
			**	   dependencies of objects owned by U on
			**	   U possessing privileges on other objects
			**	   owned by him/her
			*/
		        status = qeu_add_lost_privs(qef_cb, temp_qeu,
			    (QEU_REVOKE_INFO *) NULL, gropt_revoked, 
			    write_temp_row, 1);
		        if (status != E_DB_OK)
		        {
			    error = temp_qeu->error.err_code;
			    break;
		        }
		    }
		}

		if (status != E_DB_OK)
		    break;
	    } 
	    else
	    {
		/*
		** privilege and/or ability to grant privilege was lost
		** by PUBLIC
		*/
		bool			grnt_compatible_priv_lost;
		QEU_REVOKE_INFO		*priv_rp, *gropt_rp;

		/*
		** remember whether the temp table row specifies that PUBLIC
		** lost only GRANT-compatible privilege
		*/
		grnt_compatible_priv_lost =
		    (read_temp_row->qeu_flags & QEU_GRANT_COMPATIBLE_ONLY) != 0;

		/* 
		** setup priv_revoked and gropt_revoked - not a lot is
		** known at this point
		*/
		priv_revoked = read_temp_row + 2;
		gropt_revoked = read_temp_row + 3;

		/*
		** set qeu_flags in priv_revoked and gropt_revoked:
		** - set QEU_PRIV_LOST_BY_OWNER since loss of privileges on
		**   which an object or permit depends results in loss of
		**   corresponding privilege by the owner of the object
		** - set QEU_INVALIDATE_QPS if it is set in the current row or
		**   if the current row describes a privilege lost on a dbproc
		** - QEU_PRIV_LOST will be set in priv_revoked->qeu_flags
		** - QEU_GRANT_OPTION_LOST will be set in 
		**   gropt_revoked->qeu_flags
		*/
		if (read_temp_row->qeu_priv == DB_EXECUTE)
		{
		    priv_revoked->qeu_flags = gropt_revoked->qeu_flags = 
			QEU_INVALIDATE_QPS;
		}
		else
		{
		    priv_revoked->qeu_flags = gropt_revoked->qeu_flags =
			read_temp_row->qeu_flags & QEU_INVALIDATE_QPS;
		}

		priv_revoked->qeu_flags |= 
		    QEU_PRIV_LOST | QEU_PRIV_LOST_BY_OWNER;
		gropt_revoked->qeu_flags |= 
		    QEU_GRANT_OPTION_LOST | QEU_PRIV_LOST_BY_OWNER;

		priv_revoked->qeu_gtype = gropt_revoked->qeu_gtype = DBGR_USER;

		/* 
		** now we will look in IIPRIV (by keying into IIXPRIV and
		** doing a TID-join into IIPRIV) for objects and permits
		** which depend on the privilege and/or ability to grant
		** privilege being lost by PUBLIC
		*/
		xpr_qeu->qeu_getnext = QEU_REPO;
		xpr_qeu->qeu_klen = 3;
		xpr_qeu->qeu_count = 1;

		/*
		** we look for descriptors such that
		** - they describe dependence of objects or permits owned by
		**   some user U on the specified privilege, and
		** - if the privilege is defined on a table/view, the privilege
		**   must be being lost on at least some of attributes on which
		**   a permit or an object depends +
		** - U himself does not possess the privilege being lost to
		**   PUBLIC on which his/her permit or object depends
		** 
		** in the cases when a dbproc depends on a privilege which is 
		** being revoked or ability to grant which is being revoked, 
		** we will need to record that the owner of the dbproc lost 
		** EXECUTE and/or ability to grant EXECUTE on the dbproc 
		** 
		** in the cases when a view depends on a privilege (i.e. SELECT)
		** GRANT OPTION FOR which is being revoked, we will need to 
		** record that the owner of the view lost GRANT OPTION FOR 
		** SELECT on his view
		** 
		** in the cases when a view depends on a privilege (i.e. SELECT)
		** which is being revoked, we will call qeu_dview() to
		** destroy it and will not need to enter anything into the temp
		** table
		**
		** Similarly, if the creator of a constraint no longer possess a
		** privilege if a privilege on which a constraint depends (i.e.
		** REFERENCES), we will call qeu_d_cascade() to destroy the
		** constraint and all objects which were created to enforce it
		*/

		while (status == E_DB_OK)
		{
		    bool			priv_lost, grant_option_lost;

		    status = qeu_get(qef_cb, xpr_qeu);
		    if (status != E_DB_OK)
		    {
		        if (xpr_qeu->error.err_code == E_QE0015_NO_MORE_ROWS)
		        {
			    /* ran out of rows - we are done */
			    status = E_DB_OK;
		        }
		        else
		        {
			    error = xpr_qeu->error.err_code;
		        }
		        break;
		    }

		    if (xpr_qeu->qeu_getnext == QEU_REPO)
		    {
		        xpr_qeu->qeu_getnext = QEU_NOREPO;
		        xpr_qeu->qeu_klen = 0;
		    }

		    /*
		    ** Now do a TID-join into IIPRIV to get the
		    ** information on the attributes to which the privileges
		    ** must apply (for tables and views only) and the object
		    ** which depends on the <auth id> possessing a privilege
		    ** which is being revoked from PUBLIC
		    */
		    priv_qeu->qeu_tid = xpr_tuple->dbx_tidp;
		    status = qeu_get(qef_cb, priv_qeu);
		    if (status != E_DB_OK)
		    {	
			error = priv_qeu->error.err_code;
			break;
		    }

		    /* 
		    ** if the independent privilege is defined on a table
		    ** or a view, verify that the intersection of the map 
		    ** of attributes privilege and/or ability to grant 
		    ** privilege on which is being revoked and the map in 
		    ** IIPRIV tuple is non-empty; otherwise the
		    ** revocation being processed will have no effect on the
		    ** object or permit described in the IIPRIV tuple
		    */

		    if (tbl_priv_lost)
		    {
			i4		*tmp_map, *indep_map, *res_map;
			bool		found = FALSE;

			for (i = 0,
			     tmp_map = read_temp_row->qeu_attrmap,
			     indep_map = priv_tuple->db_priv_attmap,
			     res_map = attrmap;

			     i < DB_COL_WORDS;

			     i++, tmp_map++, indep_map++, res_map++
			    )
		        {
			    if ((*res_map = *tmp_map & *indep_map))
			        found = TRUE;
		        }

		        if (!found)
			    continue;
		    }

		    /*
		    ** Next we will determine whether the owner of the object or
		    ** permit in his own right possesses those of the privileges
		    ** being lost by PUBLIC on which his/her object or permit
		    ** depends
		    */

		    if (read_temp_row->qeu_flags & QEU_PRIV_LOST)
		    {
			priv_rp = priv_revoked;

			STRUCT_ASSIGN_MACRO(read_temp_row->qeu_obj_id,
					    priv_rp->qeu_obj_id);

			priv_rp->qeu_priv = read_temp_row->qeu_priv;

			if (grnt_compatible_priv_lost)
			    priv_rp->qeu_flags |= QEU_GRANT_COMPATIBLE_ONLY;
			else
			    priv_rp->qeu_flags &= ~QEU_GRANT_COMPATIBLE_ONLY;

			STRUCT_ASSIGN_MACRO(priv_tuple->db_grantee,
					    priv_rp->qeu_authid);
			if (tbl_priv_lost)
			{
			    for (
				 i = 0, map_from = attrmap,
				 map_to = priv_rp->qeu_attrmap;

				 i < DB_COL_WORDS;

				 i++, map_from++, map_to++
				)
			    {
				*map_to = *map_from;
			    }
			}
		    }
		    else
		    {
			priv_rp = (QEU_REVOKE_INFO *) NULL;
		    }

		    if (read_temp_row->qeu_flags & QEU_GRANT_OPTION_LOST)
		    {
			gropt_rp = gropt_revoked;

			if (priv_rp)
			{
			    STRUCT_ASSIGN_MACRO((*priv_rp), (*gropt_rp));
			    gropt_rp->qeu_flags &= ~QEU_PRIV_LOST;
			    gropt_rp->qeu_flags |= QEU_GRANT_OPTION_LOST;
			}
			else
			{
			    STRUCT_ASSIGN_MACRO(read_temp_row->qeu_obj_id,
				gropt_rp->qeu_obj_id);

			    gropt_rp->qeu_priv = read_temp_row->qeu_priv;

			    if (grnt_compatible_priv_lost)
				gropt_rp->qeu_flags |= 
				    QEU_GRANT_COMPATIBLE_ONLY;
			    else
				gropt_rp->qeu_flags &=
				    ~QEU_GRANT_COMPATIBLE_ONLY;

			    STRUCT_ASSIGN_MACRO(priv_tuple->db_grantee,
						gropt_rp->qeu_authid);
			    if (tbl_priv_lost)
			    {
				for (
				     i = 0, map_from = attrmap,
				     map_to = gropt_rp->qeu_attrmap;

				     i < DB_COL_WORDS;

				     i++, map_from++, map_to++
				    )
				{
				    *map_to = *map_from;
				}
			    }
			}
		    }
		    else
		    {
			gropt_rp = (QEU_REVOKE_INFO *) NULL;
		    }

		    status = qeu_match_privs(qeuq_cb, qef_cb, prot2_qeu,
			priv_rp, gropt_rp, (QEU_PERM_DEPTH_INFO *) NULL, 1, 
			(QEU_PDI_TBL_DESCR *) NULL, prot_2tuple, 
			(DB_OWN_NAME *) NULL, &error);

		    if (status != E_DB_OK)
		    {
			break;
		    }

		    /*
		    ** if the owner of the object or permit named in IIPRIV
		    ** tuple owns in his own right all of the privileges named
		    ** in this IIPRIV tuple which were revoked from PUBLIC and
		    ** on which his object or permit depends, skip to the next
		    ** IIPRIV tuple
		    */
		    priv_lost = priv_rp && priv_rp->qeu_priv;
		    grant_option_lost = gropt_rp && gropt_rp->qeu_priv;

		    if (!priv_lost && !grant_option_lost)
		    {
			continue;
		    }

		    /*
		    ** if this point is reached, we know that the privilege
		    ** being revoked will have some effect on the object or
		    ** permit which depends on it (with dbprocs we will wait
		    ** until the temp table row describing privilege and/or
		    ** ability to grant privilege which was lost by the dbproc's
		    ** owner is read, to determine whether the privilege and/or
		    ** ability to grant privilege which are being lost by the
		    ** dbproc owner will have some effect on the dbproc);
		    **
		    ** if IIPRIV tuple indicates that the dependent object is of
		    ** type constraint, it will be destroyed only if the
		    ** privilege (and not GRANT OPTION FOR privilege) is being
		    ** lost by the constraint's creator as a result of a
		    ** privilege being revoked from PUBLIC
		    **
		    ** if the descriptor number is 0, we must be dealing
		    ** with dependence of a dbproc or a view on a privilege
		    ** on an object; otherwise we must be dealing with a
		    ** dependence of a permit on a view on a privilege on
		    ** the underlying table or of constraint on a privilege
		    ** on referenced table
		    */
		    if (   priv_tuple->db_descr_no == 0
			&& priv_tuple->db_dep_obj_type == DB_DBP)
		    {
			/*
			** at the very least, owner of the dbproc lost ability
			** to grant privileges on it
			** 
			** if the user lost a privilege on which a dbproc
			** depends, add a record to the temp table
			** indicating that the owner of the dbproc lost
			** EXECUTE and GRANT OPTION FOR EXECUTE on his
			** dbproc; otherwise (user lost GRANT OPTION FOR
			** privilege on which a dbproc depends), add a
			** record to the temp table indicating that the
			** owner of the dbproc lost GRANT OPTION FOR EXECUTE
			** on his dbproc
			*/
			STRUCT_ASSIGN_MACRO(priv_tuple->db_dep_obj_id,
			    gropt_revoked->qeu_obj_id);
			gropt_revoked->qeu_priv = DB_EXECUTE;
			gropt_revoked->qeu_flags |= QEU_GRANT_COMPATIBLE_ONLY;
			STRUCT_ASSIGN_MACRO(priv_tuple->db_grantee,
			    gropt_revoked->qeu_authid);

			if (priv_lost)
			{
			    priv_rp = priv_revoked;
			    STRUCT_ASSIGN_MACRO(priv_tuple->db_dep_obj_id,
						priv_rp->qeu_obj_id);
			    priv_rp->qeu_priv = DB_EXECUTE;
			    priv_rp->qeu_flags |= QEU_GRANT_COMPATIBLE_ONLY;
			    STRUCT_ASSIGN_MACRO(priv_tuple->db_grantee,
				priv_rp->qeu_authid);
			}
			else
			{
			    priv_rp = (QEU_REVOKE_INFO *) NULL;
			}

		        status = qeu_add_lost_privs(qef_cb, temp_qeu,
			    priv_rp, gropt_revoked, write_temp_row, 1);
		        if (status != E_DB_OK)
		        {
			    error = temp_qeu->error.err_code;
			    break;
		        }
		    }
		    else if (   priv_tuple->db_descr_no == 0
			     && priv_tuple->db_dep_obj_type == DB_VIEW
			    )
		    {
			/*
			** if (a user lost privilege (i.e. SELECT) on which a
			**     view depends)
			** {
			**   if (the view defined was in QUEL and only
			**       GRANT-compatible SELECT is being revoked)
			**   {
			**     check whether there are some permits conveying
			**     non-GRANT-compatible privileges that would keep
			**     this view from being abandoned
			**   }
			**
			**   if (the view is abandoned)
			**   {
			**     we will invoke qeu_d_cascade() to perform
			**     cascading destruction of a view and will not add
			**     any records to the temp table
			**   }
			** }
			** else	    // user lost GRANT OPTION FOR privilege 
			**	    // (i.e. SELECT) on which a view depends
			** {
			**   we will add a record to the temp table indicating
			**   that the view's owner lost GRANT OPTION FOR SELECT
			**   on the view
			** }
			*/
			if (priv_lost)
			{
			    QEUQ_CB		dview_qeuqcb;

			    /*
			    ** if the view was defined using QUEL and only
			    ** GRANT-compatible privilege is getting revoked,
			    ** determine whether there are some permit(s)
			    ** conveying non-GRANT-compatible privileges which
			    ** would keep the view from getting abandoned
			    */
			    if (   ~priv_tuple->db_prv_flags &
					DB_GRANT_COMPATIBLE_PRIV
			        && grnt_compatible_priv_lost)
			    {
				priv_revoked->qeu_priv =
				    read_temp_row->qeu_priv;
				priv_revoked->qeu_flags &= 
				    ~QEU_GRANT_COMPATIBLE_ONLY;
				STRUCT_ASSIGN_MACRO(priv_tuple->db_grantee,
				    priv_revoked->qeu_authid);
				for (
				     i = 0,
				     map_from = priv_tuple->db_priv_attmap,
				     map_to = priv_revoked->qeu_attrmap;

				     i < DB_COL_WORDS;

				     i++, map_from++, map_to++
				    )
				{
				    *map_to = *map_from;
				}

				status = qeu_match_privs(qeuq_cb, qef_cb,
				    prot2_qeu, priv_revoked,
				    (QEU_REVOKE_INFO *) NULL, 
				    (QEU_PERM_DEPTH_INFO *) NULL, 1, 
				    (QEU_PDI_TBL_DESCR *) NULL, prot_2tuple,
				    (DB_OWN_NAME *) NULL, &error);

				if (status != E_DB_OK)
				{
				    break;
				}

				/*
				** if the owner of this QUEL view possess
				** sufficient non-GRANT-compatible privilege to
				** keep the view from becoming abandoned, we
				** will not destroy the view - we are done with
				** this descriptor
				*/
				if (priv_revoked->qeu_priv == 0)
				    continue;
			    }
			    
			    STRUCT_ASSIGN_MACRO(priv_tuple->db_dep_obj_id, 
				dmt_show.dmt_tab_id);

			    status = dmf_call(DMT_SHOW, &dmt_show);
			    if (   status != E_DB_OK 
				&& dmt_show.error.err_code != 
					E_DM0114_FILE_NOT_FOUND)
			    {
				error = dmt_show.error.err_code;
				break;
			    }

			    /* 
			    ** owner of the view no longer possesses
			    ** privilege on which a view depends - if
			    ** RESTRICTed revocation was specified, return
			    ** error; otherwise call qeu_d_cascade() to perform
			    ** cascading destruction of the view
			    */
			    if (~qeuq_cb->qeuq_flag_mask & QEU_DROP_CASCADE)
			    {
				error = E_QE0251_ABANDONED_VIEW;
				status = E_DB_ERROR;
				break;
			    }

			    status = E_DB_OK;

			    STRUCT_ASSIGN_MACRO((*qeuq_cb), dview_qeuqcb);
			    dview_qeuqcb.qeuq_rtbl = &priv_tuple->db_dep_obj_id;

			    /*
			    ** if we were told to force invalidation of QPs
			    ** affected by cascading destruction of privielges,
			    ** tell qeu_d_cascade() to try to invalidate QPs
			    ** dependent on the view being dropped
			    */
			    if (read_temp_row->qeu_flags & QEU_INVALIDATE_QPS)
			    {
				dview_qeuqcb.qeuq_flag_mask |=
				    QEU_FORCE_QP_INVALIDATION;
			    }

			    STRUCT_ASSIGN_MACRO(dmt_tbl_entry.tbl_qryid,
				dview_qeuqcb.qeuq_qid);
			    
			    /*
			    ** qeu_d_cascade will take care of invalidating RDF
			    ** cache entries for all objects that get destroyed
			    ** (or, in case of database procedures, reclassified
			    ** to dormant)
			    */
			    status = qeu_d_cascade(qef_cb, &dview_qeuqcb,
				(DB_QMODE) DB_VIEW, &dmt_tbl_entry);
			    if (status != E_DB_OK)
			    {
				error = dview_qeuqcb.error.err_code;
				break;
			    }
			}
			else
			{
			    /* 
			    ** user loses GRANT OPTION FOR SELECT on
			    ** his/her view
			    */
			    STRUCT_ASSIGN_MACRO(priv_tuple->db_dep_obj_id,
				gropt_revoked->qeu_obj_id);
			    gropt_revoked->qeu_priv = DB_RETRIEVE;
			    gropt_revoked->qeu_flags |=
			        QEU_GRANT_COMPATIBLE_ONLY;
			    STRUCT_ASSIGN_MACRO(priv_tuple->db_grantee,
				gropt_revoked->qeu_authid);
			    /* 
			    ** set all bits in the attribute map to
			    ** indicate that ability to grant SELECT is lost 
			    ** on the view and on its every column
			    */
			    for (i = 0; i < DB_COL_WORDS; i++)
				gropt_revoked->qeu_attrmap[i] = ~((i4) 0);

			    status = qeu_add_lost_privs(qef_cb, temp_qeu,
				(QEU_REVOKE_INFO *) NULL, gropt_revoked, 
				write_temp_row, 1);
			    if (status != E_DB_OK)
			    {
				error = temp_qeu->error.err_code;
				break;
			    }
			}
		    }
		    else if (priv_tuple->db_dep_obj_type == DB_CONS)
		    {
			QEUQ_CB	    drop_cons_qeuqcb;

			if (!priv_lost)
			{
			    /*
			    ** if the user only lost ability to grant privilege
			    ** on which a constraint depends, constraint will
			    ** not be affected, so skip to the next temp table
			    ** row
			    */
			    continue;
			}
			
			/* 
			** user lost REFERENCES privilege on some or all
			** <referenced columns> of a constraint - the
			** constraints will become abandoned
			**
			** first we need to determine number of the constraint
			** which depends on the privilege - this requires that
			** we do a join into IIDBDEPENDS to get integrity number
			*/
			dep_qeu->qeu_getnext = QEU_REPO;
			dep_qeu->qeu_count = 1;
			dep_qeu->qeu_klen = 4;
			STRUCT_ASSIGN_MACRO(priv_tuple->db_dep_obj_id, dep_id);
			indep_obj_type = DB_PRIV_DESCR;
			priv_descr_no = priv_tuple->db_descr_no;

			status = qeu_get(qef_cb, dep_qeu);
			if (status != E_DB_OK)
			{
			    error = dep_qeu->error.err_code;
			    break;
			}
			
			/*
			** if RESTRICTed revocation was specified, we will
			** report an error; otherwise we will call
			** qeu_d_cascade() to destroy the constraint and all
			** objects that depend on it
			*/
			if (~qeuq_cb->qeuq_flag_mask & QEU_DROP_CASCADE)
			{
			    error = E_QE0257_ABANDONED_CONSTRAINT;
			    status = E_DB_ERROR;
			    break;
			}

			STRUCT_ASSIGN_MACRO((*qeuq_cb), drop_cons_qeuqcb);

			/*
			** if we were told to force invalidation of QPs
			** affected by cascading destruction of privielges,
			** tell qeu_d_cascade() to try to invalidate QPs
			** which may depend on the constraint being dropped
			*/
			if (read_temp_row->qeu_flags & QEU_INVALIDATE_QPS)
			{
			    drop_cons_qeuqcb.qeuq_flag_mask |=
			        QEU_FORCE_QP_INVALIDATION;
		        }

			drop_cons_qeuqcb.qeuq_rtbl = &priv_tuple->db_dep_obj_id;
			drop_cons_qeuqcb.qeuq_ino = dep_tuple->dbv_qid;
			
			status = qeu_d_cascade(qef_cb, &drop_cons_qeuqcb,
			    (DB_QMODE) DB_CONS, (DMT_TBL_ENTRY *) NULL);
			if (status != E_DB_OK)
			{
			    error = drop_cons_qeuqcb.error.err_code;
			    break;
			}
		    }
		    else if (grant_option_lost)
		    {
			/* 
			** user lost GRANT OPTION FOR privilege on a view's
			** underlying table on which a permit on a view depends
			**
			** first determine number of the permit which
			** depends on the privilege - this requires that
			** we do a join into IIDBDEPENDS to get a permit
			** number - and the scan IIPROTECT to find that permit
			**
			** Note that there could be multiple permits on the
			** view which depend on this privilege descriptor, but
			** it is sufficient that we find just one of them since
			** that will enable us to determine which privileges the
			** owner of the object may no longer grant on his view
			** and record it in the temporary table;
			** if a privilege is being revoked on a subset of
			** attributes listed in the privilege descriptor, we
			** will update the descriptor here and will take care of
			** permits when we get to the row in the temporary table
			** that indicates that the owner of the object may not
			** grant privilege conveyed by the permits
			*/
			dep_qeu->qeu_getnext = QEU_REPO;
			dep_qeu->qeu_count = 1;
			dep_qeu->qeu_klen = 4;
			STRUCT_ASSIGN_MACRO(priv_tuple->db_dep_obj_id, dep_id);
			indep_obj_type = DB_PRIV_DESCR;
			priv_descr_no = priv_tuple->db_descr_no;

			status = qeu_get(qef_cb, dep_qeu);
			if (status != E_DB_OK)
			{
			    error = dep_qeu->error.err_code;
			    break;
			}
			
			/* 
			** now scan IIPROTECT looking for a permit with
			** specified number
			**
			** CAUTION: we will reset obj_id (key used when
			** accessing IIPROTECT through prot_qeu and
			** IIPROCEDURE through xdbp_qeu);  while it is safe
			** to do it here, anyone (myself included) making future
			** changes should be careful not to foul things up
			*/
			STRUCT_ASSIGN_MACRO(priv_tuple->db_dep_obj_id, obj_id);
			prot_qeu->qeu_getnext = QEU_REPO;
			prot_qeu->qeu_klen = 2;
			prot_qeu->qeu_count = 1;

			for (;;) 
			{ 
			    status = qeu_get(qef_cb, prot_qeu); 
			    if (status != E_DB_OK) 
			    {
				error = prot_qeu->error.err_code;
				break;
			    }

			    if (prot_qeu->qeu_getnext == QEU_REPO)
			    {
				prot_qeu->qeu_getnext = QEU_NOREPO;
				prot_qeu->qeu_klen = 0;
			    }

			    if (prot_tuple->dbp_permno == dep_tuple->dbv_qid)
			    {
				/* we found our permit */
				break;
			    }
			}

			if (status != E_DB_OK)
			    break;

			/* 
			** if RESTRICTed revocation was specified, report error
			*/
			if (~qeuq_cb->qeuq_flag_mask & QEU_DROP_CASCADE)
			{
			    error = E_QE0250_ABANDONED_PERMIT;
			    status = E_DB_ERROR;
			    break;
			}

			STRUCT_ASSIGN_MACRO(prot_tuple->dbp_tabid,
			    gropt_revoked->qeu_obj_id);
			gropt_revoked->qeu_priv =
			    prot_tuple->dbp_popset & ~((i4) DB_GRANT_OPTION);
			gropt_revoked->qeu_flags |= QEU_GRANT_COMPATIBLE_ONLY;
			STRUCT_ASSIGN_MACRO(priv_tuple->db_grantee,
			    gropt_revoked->qeu_authid);

			/*
			** gropt_revoked->qeu_attrmap describes attributes of
			** the view's underlying table on which the user lost
			** GRANT OPTION FOR privilege (as was computed by
			** qeu_match_privs())
			*/
			    
			if (BTsubset((char *) priv_tuple->db_priv_attmap,
				(char *) gropt_revoked->qeu_attrmap,
				BITS_IN(gropt_revoked->qeu_attrmap)))
			{
			    for (
				 i = 0, map_from = prot_tuple->dbp_domset,
				 map_to = gropt_revoked->qeu_attrmap;

				 i < DB_COL_WORDS;

				 i++, map_from++, map_to++
				)
			    {
				*map_to = *map_from;
			    }
			}
			else
			{
			    /*
			    ** copy a map of attributes on which
			    ** GRANT OPTION FOR privilege is being lost by the
			    ** user from gropt_revoked->qeu_attrmap to attrmap
			    ** since we plan to store in
			    ** gropt_revoked->qeu_attrmap a map of attributes of
			    ** the view on which its owner may no longer grant
			    ** privilege
			    */
			    for (i = 0; i < DB_COL_WORDS; i++)
				attrmap[i] = gropt_revoked->qeu_attrmap[i];
			    
			    /*
			    ** privilege is being lost on a subset of attributes
			    ** listed in the privilege descriptor; we need to
			    **   - update the map of attributes in the privilege
			    **	   descriptor and write it out to disk and
			    **	 - invoke (*qeuq_cb->qeuq_b2v_col_xlate)() to
			    **	   determine on which attributes of the view the
			    **	   user may no longer grant the privilege
			    */
			    for (i = 0; i < DB_COL_WORDS; i++)
				priv_tuple->db_priv_attmap[i] &= ~attrmap[i];
			    status = qeu_replace(qef_cb, priv_qeu);
			    if (status != E_DB_OK)
			    {
				error = priv_qeu->error.err_code;
				break;
			    }
			    
			    status = (*qeuq_cb->qeuq_b2v_col_xlate)(
				&priv_tuple->db_dep_obj_id,
				gropt_revoked->qeu_attrmap,
				&priv_tuple->db_indep_obj_id, attrmap);
			    if (DB_FAILURE_MACRO(status))
			    {
				error = E_QE0002_INTERNAL_ERROR;
				break;
			    }
			}

			/*
			** NOTE: it is safe to be telling qeu_add_lost_privs()
			**       that the privilege being lost is
			**	 GRANT-compatible since the privilege being lost
			**	 would not be SELECT and the only time when this
			**	 becomes important is when we try to determine
			**	 whether a QUEL view has become abandoned (i.e.
			**	 whether there are some permits conveying
			**	 non-GRANT-compatible SELECT which would keep
			**	 the view  from becoming abandoned)
			**
			** NOTE 2: we are recording that the owner of the view
			**         may not grant privilege on his view but not
			**	   that he may not perform action on the view
			**	   because this latter piece of information is
			**	   of no benefit since no catalog describes
			**	   dependencies of objects owned by U on
			**	   U possessing privileges on other objects
			**	   owned by him/her
			*/
		        status = qeu_add_lost_privs(qef_cb, temp_qeu,
			    (QEU_REVOKE_INFO *) NULL, gropt_revoked, 
			    write_temp_row, 1);
		        if (status != E_DB_OK)
		        {
			    error = temp_qeu->error.err_code;
			    break;
		        }
		    }
		}

		if (status != E_DB_OK)
		    break;

		/*
		** as far as we can tell at this point, revocation of privileges
		** and /or ability to grant privileges specified in
		** read_temp_row->qeu_priv from PUBLIC will be allowed to happen
		** (i.e. caller specified <drop behaviour> of CASCADE or a
		** RESTRICTed revocation will not result in any objects becoming
		** abandoned.)
		**
		** I am saying "as far as we can tell" because if there was a
		** dbproc whose status (i.e. ACTIVE and/or GRANTABLE) may have
		** to be changed as a result of PUBLIC losing this privilege and
		** the caller has specified <drop behaviour> of RESTRICT, we
		** will abort the whole operation, but until then we assume that
		** it is safe to proceed
		*/

		/*
		** ... which brings us to the last order of business: if PUBLIC
		** lost one or more of (SELECT, INSERT, DELETE, UPDATE) on a
		** table, we may need to turn ON (using negative logic) ALL
		** and/or RETRIEVE TO ALL bits.
		** First, look up the table description.  If ALL TO ALL bit is
		** off, turn it on; if RETRIEVE TO ALL bit is off and public is
		** losing SELECT, turn it on
		*/

		if (   tbl_priv_lost
		    && read_temp_row->qeu_flags & QEU_PRIV_LOST
		    && read_temp_row->qeu_priv &
		           (DB_RETRIEVE | DB_APPEND | DB_DELETE | DB_REPLACE)
		   )
		{
		    i4		chars_to_alter = 0;
		    
		    STRUCT_ASSIGN_MACRO(read_temp_row->qeu_obj_id,
			dmt_show.dmt_tab_id);
		    status = dmf_call(DMT_SHOW, &dmt_show);
		    if (   status != E_DB_OK
			&& dmt_show.error.err_code != E_DM0114_FILE_NOT_FOUND)
		    {
			error = dmt_show.error.err_code;
			break;
		    }

		    if (~dmt_tbl_entry.tbl_status_mask & DMT_ALL_PROT)
		    {
			/*
			** tbl_status_mask indicates that PUBLIC possessed ALL
		    	** privileges - but not any longer
			*/
			char_array[chars_to_alter].char_id = DMT_C_ALLPRO;
			char_array[chars_to_alter].char_value = DMT_C_ON;
			chars_to_alter++;
		    }

		    if (   ~dmt_tbl_entry.tbl_status_mask & DMT_RETRIEVE_PRO
			&& read_temp_row->qeu_priv & DB_RETRIEVE
		       )
		    {
			/*
			** tbl_status_mask indicates that PUBLIC possessed
			** RETRIEVE - but not any longer
			*/
			char_array[chars_to_alter].char_id = DMT_C_RETPRO;
			char_array[chars_to_alter].char_value = DMT_C_ON;
			chars_to_alter++;
		    }

		    if (chars_to_alter > 0)
		    {
			dmt_alt_cb.dmt_char_array.data_in_size =
			    chars_to_alter * sizeof(DMT_CHAR_ENTRY);

			dmt_alt_cb.dmt_id.db_tab_base =
			    read_temp_row->qeu_obj_id.db_tab_base;
			dmt_alt_cb.dmt_id.db_tab_index =
			    read_temp_row->qeu_obj_id.db_tab_index;

			status = dmf_call(DMT_ALTER, &dmt_alt_cb);
			if (status != E_DB_OK)
			{
			    error = dmt_alt_cb.error.err_code;
			    break;
			}

			/*
			** remember that we need to flush the RDF cache entry
			** associated with this table or view
			*/
			inv_obj_cache = TRUE;
		    }
		}
	    }
	}

	exit_loop = TRUE;
    }

    /*
    ** we get here if we are done or if some error was encountered - status
    ** will tell us which
    */
    if (status != E_DB_OK && !err_already_reported)
    {
	i4         name_len, owner_len;

	/* some errors require that we pass parameters */
	if (error == E_QE0250_ABANDONED_PERMIT)
	{
	    char	*str;
	    i4		str_len;

	    switch (prot_tuple->dbp_obtype)
	    {
		case DBOB_TABLE:
		    str = "table";
		    str_len = sizeof("table") - 1;
		    break;
		case DBOB_VIEW:
		    str = "view";
		    str_len = sizeof("view") - 1;
		    break;
		case DBOB_INDEX:
		    str = "index";
		    str_len = sizeof("index") - 1;
		    break;
		case DBOB_DBPROC:
		    str = "database procedure";
		    str_len = sizeof("database procedure") - 1;
		    break;
		case DBOB_EVENT:
		    str = "dbevent";
		    str_len = sizeof("dbevent") - 1;
		    break;
		case DBOB_SEQUENCE:
		    str = "sequence";
		    str_len = sizeof("sequence") - 1;
		    break;
		default:
		    str = "UNKNOWN OBJECT TYPE";
		    str_len = sizeof("UNKNOWN OBJECT TYPE") - 1;
		    break;
	    }

	    name_len = qec_trimwhite(sizeof(prot_tuple->dbp_obname),
		(char *) &prot_tuple->dbp_obname);
	    owner_len = qec_trimwhite(sizeof(prot_tuple->dbp_obown),
		(char *) &prot_tuple->dbp_obown);

	    (VOID) qef_error(error, 0L, status, &error, &qeuq_cb->error, 4,
		(i4) sizeof(prot_tuple->dbp_permno),
		(i4 *) &prot_tuple->dbp_permno,
		(i4) str_len, (i4 *) str,
		(i4) name_len, (i4 *) &prot_tuple->dbp_obname,
		(i4) owner_len, (i4 *) &prot_tuple->dbp_obown);
	}
	else if (error == E_QE0251_ABANDONED_VIEW)
	{
	    owner_len = qec_trimwhite(sizeof(priv_tuple->db_grantee),
		(char *) &priv_tuple->db_grantee);
	    name_len = qec_trimwhite(sizeof(dmt_tbl_entry.tbl_name),
		(char *) &dmt_tbl_entry.tbl_name);

	    (VOID) qef_error(error, 0L, status, &error, &qeuq_cb->error, 2,
		(i4) name_len, (i4 *) &dmt_tbl_entry.tbl_name,
		(i4) owner_len, (i4 *) &dmt_tbl_entry.tbl_owner);
	}
	else if (error == E_QE0252_ABANDONED_DBPROC)
	{
	    name_len = qec_trimwhite(sizeof(proc_tuple->db_dbpname),
		(char *) &proc_tuple->db_dbpname);
	    owner_len = qec_trimwhite(sizeof(proc_tuple->db_owner),
		(char *) &proc_tuple->db_owner);
		
	    (VOID) qef_error(error, 0L, status, &error, &qeuq_cb->error, 2,
		(i4) name_len, (i4 *) &proc_tuple->db_dbpname,
		(i4) owner_len, (i4 *) &proc_tuple->db_owner);
	}
	else if (error == E_QE0257_ABANDONED_CONSTRAINT)
	{
	    /*
	    ** we need to do a fair amount of work to produce this message:
	    **	- find IIINTEGRITY tuple corresponding to the
	    **	  constraint so that we can supply the name of the
	    **	  constraint that nearly became abandoned
	    **	- look up description of the table so that we can
	    **	  supply schema and table names
	    */
	    DB_INTEGRITY	ituple;
	    QEU_CB		iqeu, *i_qeu = (QEU_CB *) NULL;
	    QEF_DATA		iqef_data;
	    DMR_ATTR_ENTRY	ikey_array[2];
	    DMR_ATTR_ENTRY	*ikey_ptr_array[2];
	    i4			integ_no;
				    /*
				    ** will be reset to FALSE if an error occurs
				    ** while looking up necessary information
				    */
	    bool		print_msg = TRUE;

	    local_status = E_DB_OK;

	    for (exit_loop = FALSE; !exit_loop; )
	    {
		ikey_ptr_array[0] = ikey_array;
		ikey_ptr_array[1] = ikey_array + 1;

		iqeu.qeu_type = QEUCB_CB;
		iqeu.qeu_length = sizeof(QEUCB_CB);
		iqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
		iqeu.qeu_lk_mode = DMT_IS;
		iqeu.qeu_flag = DMT_U_DIRECT;
		iqeu.qeu_access_mode = DMT_A_READ;
		iqeu.qeu_mask = 0;
		iqeu.qeu_f_qual = 0;
		iqeu.qeu_f_qarg = 0;

		iqeu.qeu_tab_id.db_tab_base  = DM_B_INTEGRITY_TAB_ID;
		iqeu.qeu_tab_id.db_tab_index  = DM_I_INTEGRITY_TAB_ID;
		local_status = qeu_open(qef_cb, &iqeu);
		if (local_status != E_DB_OK)
		{
		    error = iqeu.error.err_code;
		    break;
		}

		i_qeu = &iqeu;
		
		i_qeu->qeu_count = 1;
		i_qeu->qeu_tup_length = sizeof(DB_INTEGRITY);
		i_qeu->qeu_output = &iqef_data;
		iqef_data.dt_next = 0;
		iqef_data.dt_size = sizeof(DB_INTEGRITY);
		iqef_data.dt_data = (PTR) &ituple;

		i_qeu->qeu_klen = 2;
		i_qeu->qeu_getnext = QEU_REPO;
		i_qeu->qeu_key = ikey_ptr_array;

		ikey_ptr_array[0]->attr_number = DM_1_INTEGRITY_KEY;
		ikey_ptr_array[0]->attr_operator = DMR_OP_EQ;
		ikey_ptr_array[0]->attr_value =
		    (char *) &dep_tuple->dbv_dep.db_tab_base;

		ikey_ptr_array[1]->attr_number = DM_2_INTEGRITY_KEY;
		ikey_ptr_array[1]->attr_operator = DMR_OP_EQ;
		ikey_ptr_array[1]->attr_value = 
		    (char *) &dep_tuple->dbv_dep.db_tab_index;

		integ_no = dep_tuple->dbv_qid;
		i_qeu->qeu_qarg = ( PTR ) &integ_no;
		i_qeu->qeu_qual = ( PTR ) qeu_q_integ;

		local_status = qeu_get(qef_cb, i_qeu);
		if (local_status != E_DB_OK)
		{
		    /* we were supposed to find the integrity */
		    error = i_qeu->error.err_code;
		    break;
		}
		
		exit_loop = TRUE;
	    }

	    if (local_status != E_DB_OK)
	    {
		(VOID) qef_error(error, 0L, local_status, &error,
		    &qeuq_cb->error, 0);
		if (local_status > status)
		    status = local_status;

		/*
		** remember to not produce the message since an error occurred
		** while trying to look up name of the integrity
		*/
		print_msg = FALSE;
	    }

	    /* close IIINTEGRITY if it has been opened */
	    if (i_qeu)
	    {
		local_status = qeu_close(qef_cb, i_qeu);
		if (local_status != E_DB_OK)
		{
		    (VOID) qef_error(i_qeu->error.err_code, 0L, local_status,
			&error, &qeuq_cb->error, 0);
		    if (local_status > status)
			status = local_status;

		    /*
		    ** remember to not produce the message since an error
		    ** occurred while trying to close IIINTEGRITY
		    */
		    print_msg = FALSE;
		}
	    }

	    if (print_msg)
	    {
		/* no new errors so far - look up table info by table id */

		STRUCT_ASSIGN_MACRO(dep_tuple->dbv_dep, dmt_show.dmt_tab_id);
		local_status = dmf_call(DMT_SHOW, &dmt_show);
		if (   local_status != E_DB_OK
		    && dmt_show.error.err_code != E_DM0114_FILE_NOT_FOUND)
		{
		    error = dmt_show.error.err_code;
		    (VOID) qef_error(error, 0L, local_status, &error,
			&qeuq_cb->error, 0);
		    if (local_status > status)
			status = local_status;

		    /*
		    ** remember to not produce the message since an error
		    ** occurred while trying to obtain name of the table
		    */
		    print_msg = FALSE;
		}
	    }

	    if (print_msg)
	    {
		/*
		** finaly, we have all the information that we need - produce
		** the bloody message
		*/
		(VOID) qef_error(E_QE0257_ABANDONED_CONSTRAINT, 0L, status,
		    &error, &qeuq_cb->error, 3,
		    qec_trimwhite(sizeof(ituple.dbi_consname),
			(char *) &ituple.dbi_consname),
		    (i4 *) &ituple.dbi_consname,
		    qec_trimwhite(sizeof(dmt_tbl_entry.tbl_owner),
			(char *) &dmt_tbl_entry.tbl_owner),
		    (i4 *) &dmt_tbl_entry.tbl_owner,
		    qec_trimwhite(sizeof(dmt_tbl_entry.tbl_name),
			(char *) &dmt_tbl_entry.tbl_name),
		    (i4 *) &dmt_tbl_entry.tbl_name);
	    }
	}
	else
	{
	    (VOID) qef_error(error, 0L, status, &error, &qeuq_cb->error, 0);
	}
    }
    
    /* close catalogs which have been opened */

    if (temp_qeu)
    {
	local_status = qeu_close(qef_cb, temp_qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(temp_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (prot_qeu)
    {
	local_status = qeu_close(qef_cb, prot_qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(prot_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (prot2_qeu)
    {
	local_status = qeu_close(qef_cb, prot2_qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(prot2_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (qtxt_qeu)
    {
	local_status = qeu_close(qef_cb, qtxt_qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(qtxt_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (priv_qeu)
    {
	local_status = qeu_close(qef_cb, priv_qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(priv_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (xpr_qeu)
    {
	local_status = qeu_close(qef_cb, xpr_qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(xpr_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (proc_qeu)
    {
	local_status = qeu_close(qef_cb, proc_qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(proc_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (xdbp_qeu)
    {
	local_status = qeu_close(qef_cb, xdbp_qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(xdbp_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    if (dep_qeu)
    {
	local_status = qeu_close(qef_cb, dep_qeu);    
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(dep_qeu->error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }

    /* Destroy the temporary table. */
    if (temp_created)
    {
	dmt_cb.dmt_flags_mask = 0;
	local_status = dmf_call(DMT_DELETE_TEMP, &dmt_cb);
	if (local_status != E_DB_OK)
	{
	    (VOID) qef_error(dmt_cb.error.err_code, 0L, local_status, 
			&error, &qeuq_cb->error, 0);
	    if (local_status > status)
		status = local_status;
	}
    }	    

    if (ulm.ulm_streamid != (PTR) NULL)
    { 
	/* 
	** Close the input stream created for the view tuples. 
	*/ 
	(VOID) ulm_closestream(&ulm); 
    }

    return(status);
}

/*
** Name: qeu_add_lost_privs  -  add description of privileges and/or ability to
**				grant privileges to a temporary table
**
** Description:
**	Given description(s) of privileges and/or ability to grant privileges
**	which were lost by a specified <auth id>, add records describing them to
**	a temporary table.  If the privilege being lost is specified, and
**	ability to grantg privileges is specified and they both deal with the
**	same privilege, one row will be added to the table describing both;
**	otherwise two separate rows may be needed
**
** Input:
**	qef_cb		QEF cb
**	
**	temp_qeu	QEU CB which was used to open a temporary table and will
**			be used to append row(s) to it
**			
**	privs_lost     		array of QEU_REVOKE_INFO's describing
**				privileges being revoked from a specified 
**				<auth id>;
**				may be set to NULL if no privileges are
**				being revoked
**	    qeu_obj_id		id of the object
**	    qeu_priv		privilege map
**          qeu_flags           additional info about privileges
**		QEU_PRIV_LOST	indicates that privilege described in
**				qeu_priv has been lost
**              QEU_GRANT_COMPATIBLE_ONLY
**                              set if only GRANT-compatible privilege
**                              has been revoked
**	    qeu_gtype		type of <auth id> losing privilege
**	    qeu_authid		<auth id> losing privilege
**	    qeu_attrmap 	map of attributes to which privilege
**				described by qeu_priv applies
**	    
**	grant_option_lost    	array of QEU_REVOKE_INFO's describing 
**				privileges GRANT OPTION FOR which has
**				been revoked from a speciifed <auth id>;
**				may be set to NULL if ability to grant
**				privilege(s) is not being revoked (as would 
**				happen if a user is dropping a permit 
**				specifying some privilege but not WGO)
**	    qeu_obj_id		id of the object
**          qeu_priv            privilege map
**	    qeu_flags		additional info about privileges
**		QEU_GRANT_OPTION_LOST	
**				indicates that GRANT OPTION FOR privilege 
**				described in qeu_priv has been lost
**		QEU_GRANT_COMPATIBLE_ONLY
**				set if only GRANT-compatible privilege 
**				has been revoked 
**	    qeu_gtype           type of <auth id> losing ability to grant
**				privilege
**	    qeu_authid          <auth id> losing ability to grant privilege
**          qeu_attrmap         map of attributes to which privilege
**                              described by qeu_priv applies
**
**	temp_row		pointer to a temporary table record which must
**				be populated and appended to the temporary table
**
**	list_size		number of elements in privs_lost and/or
**				grant_option_lost lists
**
** Output:
**	temp_qeu
**	    error		filled in if an error occurred
**
** Returns:
**	E_DB_{OK,ERROR...}
**
** History:
**
**	27-jul-92 (andre)
**	    written
**	16-sep-92 (andre)
**	    privilege map is populated using bitwise ops so we cannot use
**	    BTnext() to determine all bits that are set; we will use bitwise AND
**	    instead
*/
static DB_STATUS
qeu_add_lost_privs(
QEF_CB			*qef_cb,
QEU_CB			*temp_qeu,
QEU_REVOKE_INFO		*privs_lost,
QEU_REVOKE_INFO		*grant_option_lost,
QEU_REVOKE_INFO 	*temp_row,
i4			list_size)
{
    i4			i;
    i4			*map_from, *map_to;
    DB_STATUS		status = E_DB_OK;
    
    for (i = 0; i < list_size; i++)
    {
	i2		priv_flag = 0;
	i4		privs;
	i4		j;

	/* 
	** if both privs_lost list and grant_option_lost have been
	** specified, we will be processing elements of privs_lost and
	** grant_option_lost in parallel
	*/
	if (privs_lost && privs_lost[i].qeu_priv)
	{
	    priv_flag = QEU_PRIV_LOST;

	    /* 
	    ** determine whether we can represent privileges being revoked 
	    ** and privileges ability to grant which is being revoked using 
	    ** one tuple
	    */
	    if (grant_option_lost && grant_option_lost[i].qeu_priv)
	    {
	        /* 
	        ** turn off flag bits that are guaranteed to make the two
	        ** elements different
	        */
	        privs_lost[i].qeu_flags &= ~QEU_PRIV_LOST;
	        grant_option_lost[i].qeu_flags &= ~QEU_GRANT_OPTION_LOST;

		if (!MEcmp((PTR) (privs_lost + i), 
			(PTR) (grant_option_lost + i), sizeof(QEU_REVOKE_INFO)))
		{
		    priv_flag |= QEU_GRANT_OPTION_LOST;
		}

		/* reset the flags */
		privs_lost[i].qeu_flags |= QEU_PRIV_LOST;
		grant_option_lost[i].qeu_flags |= QEU_GRANT_OPTION_LOST;
	    }

	    STRUCT_ASSIGN_MACRO(privs_lost[i], (*temp_row));
	    temp_row->qeu_flags |= priv_flag;

    	    privs = temp_row->qeu_priv;

	    /* 
	    ** if multiple privileges are named in privs_lost[i].qeu_priv, 
	    ** insert separate rows for each of them
	    */

	    if (privs & DB_RETRIEVE)
	    {
		temp_row->qeu_priv = DB_RETRIEVE;
		if ((status = qeu_append(qef_cb, temp_qeu)) != E_DB_OK)
		    return(status);
	    }

	    if (privs & DB_REPLACE)
	    {
		temp_row->qeu_priv = DB_REPLACE;
		if ((status = qeu_append(qef_cb, temp_qeu)) != E_DB_OK)
		    return(status);
	    }

	    if (privs & DB_DELETE)
	    {
		temp_row->qeu_priv = DB_DELETE;
		if ((status = qeu_append(qef_cb, temp_qeu)) != E_DB_OK)
		    return(status);
	    }
	    
	    if (privs & DB_APPEND)
	    {
		temp_row->qeu_priv = DB_APPEND;
		if ((status = qeu_append(qef_cb, temp_qeu)) != E_DB_OK)
		    return(status);
	    }

	    if (privs & DB_EVREGISTER)
	    {
		temp_row->qeu_priv = DB_EVREGISTER;
		if ((status = qeu_append(qef_cb, temp_qeu)) != E_DB_OK)
		    return(status);
	    }

	    if (privs & DB_EVRAISE)
	    {
		temp_row->qeu_priv = DB_EVRAISE;
		if ((status = qeu_append(qef_cb, temp_qeu)) != E_DB_OK)
		    return(status);
	    }

	    if (privs & DB_NEXT)
	    {
		temp_row->qeu_priv = DB_NEXT;
		if ((status = qeu_append(qef_cb, temp_qeu)) != E_DB_OK)
		    return(status);
	    }

	    if (privs & DB_EXECUTE)
	    {
		temp_row->qeu_priv = DB_EXECUTE;
		if ((status = qeu_append(qef_cb, temp_qeu)) != E_DB_OK)
		    return(status);
	    }

	    if (privs & DB_REFERENCES)
	    {
		temp_row->qeu_priv = DB_REFERENCES;
		if ((status = qeu_append(qef_cb, temp_qeu)) != E_DB_OK)
		    return(status);
	    }
        }

        /* 
        ** if ability to grant privileges is being revoked and either
        ** privileges themselves are not being revoked or the privileges 
        ** being revoked are different from privileges ability to grant which 
        ** is being revoked, we will append a separate tuple describing
        ** privileges ability to grant which is being revoked
        */

        if (   grant_option_lost 
	    && grant_option_lost[i].qeu_priv 
	    && ~priv_flag & QEU_GRANT_OPTION_LOST
           )
        {
	    STRUCT_ASSIGN_MACRO(grant_option_lost[i], (*temp_row));
	    privs = temp_row->qeu_priv;

	    /* 
	    ** if multiple privileges are named in 
	    ** grant_option_lost[i].qeu_priv, insert separate rows 
	    ** for each of them 
	    */ 
	    if (privs & DB_RETRIEVE)
	    {
		temp_row->qeu_priv = DB_RETRIEVE;
		if ((status = qeu_append(qef_cb, temp_qeu)) != E_DB_OK)
		    return(status);
	    }

	    if (privs & DB_REPLACE)
	    {
		temp_row->qeu_priv = DB_REPLACE;
		if ((status = qeu_append(qef_cb, temp_qeu)) != E_DB_OK)
		    return(status);
	    }

	    if (privs & DB_DELETE)
	    {
		temp_row->qeu_priv = DB_DELETE;
		if ((status = qeu_append(qef_cb, temp_qeu)) != E_DB_OK)
		    return(status);
	    }
	    
	    if (privs & DB_APPEND)
	    {
		temp_row->qeu_priv = DB_APPEND;
		if ((status = qeu_append(qef_cb, temp_qeu)) != E_DB_OK)
		    return(status);
	    }

	    if (privs & DB_EVREGISTER)
	    {
		temp_row->qeu_priv = DB_EVREGISTER;
		if ((status = qeu_append(qef_cb, temp_qeu)) != E_DB_OK)
		    return(status);
	    }

	    if (privs & DB_EVRAISE)
	    {
		temp_row->qeu_priv = DB_EVRAISE;
		if ((status = qeu_append(qef_cb, temp_qeu)) != E_DB_OK)
		    return(status);
	    }

	    if (privs & DB_NEXT)
	    {
		temp_row->qeu_priv = DB_NEXT;
		if ((status = qeu_append(qef_cb, temp_qeu)) != E_DB_OK)
		    return(status);
	    }

	    if (privs & DB_EXECUTE)
	    {
		temp_row->qeu_priv = DB_EXECUTE;
		if ((status = qeu_append(qef_cb, temp_qeu)) != E_DB_OK)
		    return(status);
	    }

	    if (privs & DB_REFERENCES)
	    {
		temp_row->qeu_priv = DB_REFERENCES;
		if ((status = qeu_append(qef_cb, temp_qeu)) != E_DB_OK)
		    return(status);
	    }
        }
    }

    return(E_DB_OK);
}

/*
** Name: qeu_construct_perm_text - [re]construct permit text based on
**				   IIPROTECT tuple
**
** Description:
**	This function will reconstruct permit's query text based on IIPROTECT
**	tuple describing it.
**
**	For permits on database procedures, sequences and dbevents (which can only be
**	created using GRANT statement) and permits on tables/views which grant
**	table-wide privilege and were created using GRANT statement, the
**	procedure will be the simplest - we will delete existing IIQRYTEXT
**	tuple(s) and build new ones based on IIPROTECT tuple.
**
**	For permits on tables and views which convey table-wide privilege we
**	will delete existing IIQRYTEXT tuple(s) and build IIQRYTEXT tuple(s)
**	based on the IIPROTECT tuple using syntax in which the permit was
**	originally specified (I expect that this will happen only to permits
**	created using GRANT statement when GRANT OPTION is revoked.)
**
**	For permits on tables/view which convey column-specific privileges we
**	will need to obtain description of table and its attributes by calling
**	dmt_show().  
**
**	If the permit involved a WHERE-clause and/or AT-clause (i.e. the
*	permit was created using CREATE PERMIT), we will build up
**	text up to the beginning of the TO <grantee> and then attach the
**	remainder of the permit text at the end.
**
** Input:
**	qeuq_cb		QEU query CB
**	qef_cb		QEF session control block
**	qtxt_qeu	QEU control block which was used to open IIQRYTEXT for
**			writing and should be used here
**	ulm		ULM request CB describing a previously opened stream
**	prot_tuple	IIPROTECT tuple describing a permit
**	new_perm	TRUE if constructing text for a new permit (i.e. there
**			are no existing tuples to destroy)
**
** Output:
**	*new_qtxt	if (new_perm) head of the chain of QEF_DATA structures
**			which were generated
**	error		will be filled in if an error occurs
**			NOTE: this function will NOT report its errors - it will
**			just return them inside *error
**
** History:
**
**	28-jul-92 (andre)
**	    written
**	04-aug-92 (andre)
**	    added new_perm and new_qtxt to the interface; this will enable
**	    callers of qeu_construct_perm_text() to request that a query text be
**	    construcuted for a new permit (this is completely true for permits
**	    speicfied using GRANT statement, for permits created using CREATE
**	    PERMIT, prot_tuple->dbp_txtid must identify a permit whose query
**	    text can be used to construct a new permit text)
**	02-feb-93 (andre)
**	    we will no longer be allocating IIQRYTEXT tuple to be used for
**	    writing here; instead, once the permit text has been assembled in
**	    DB_TEXT_STRING format, we will call RDF to repackage it into
**	    IIQRYTEXT format
**
**	    changed new_qtxt from (QEF_DATA *) to (QEF_DATA **); RDF will set
**	    *new_qtxt to point at the first QEF_DATA in the chain
**	21-jun-1997 (nanpr01)
**	    Another segv problem which Roger's RDF change unveiled.
**	10-Jan-2001 (jenjo02)
**	    Set error from rdf_error.err_code on rdf_call()
**	    failures to prevent spurious E_QE0018 errors.
*/
static DB_STATUS
qeu_construct_perm_text(
QEUQ_CB		*qeuq_cb,
QEF_CB		*qef_cb,
QEU_CB		*qtxt_qeu,
ULM_RCB		*ulm,
DB_PROTECTION	*prot_tuple,
i4		new_perm,
QEF_DATA	**new_qtxt,
i4		*error)
{
    DB_STATUS		status = E_DB_OK;
    GLOBALREF QEF_S_CB	*Qef_s_cb;
    DMR_ATTR_ENTRY      qtxt_array[4];
    DMR_ATTR_ENTRY      *qtxt_ptr_array[4];
    DMT_SHW_CB          dmt_shw_cb;
    i4			qtxt_mode = DB_PROT;
    i4			qtxt_seq;
    i4			priv;
    i4			i;
    DMT_TBL_ENTRY	*tbl_entry;
    DMT_ATT_ENTRY	**att_pptr;
    DMT_ATT_ENTRY	*att_entry, *att_array;
    DB_TEXT_STRING	*perm_str = (DB_TEXT_STRING *) NULL;
    QEF_DATA		*qtxt_tups;
    i4		qt_count;

    qtxt_qeu->qeu_tup_length = sizeof(DB_IIQRYTEXT);

    qtxt_qeu->qeu_key = qtxt_ptr_array;

    qtxt_ptr_array[0]		     = qtxt_array;
    qtxt_ptr_array[0]->attr_number   = DM_1_QRYTEXT_KEY;
    qtxt_ptr_array[0]->attr_operator = DMR_OP_EQ;
    qtxt_ptr_array[0]->attr_value    =
	(char *) &prot_tuple->dbp_txtid.db_qry_high_time;

    qtxt_ptr_array[1]		     = qtxt_array + 1;
    qtxt_ptr_array[1]->attr_number   = DM_2_QRYTEXT_KEY;
    qtxt_ptr_array[1]->attr_operator = DMR_OP_EQ;
    qtxt_ptr_array[1]->attr_value    =
	(char *) &prot_tuple->dbp_txtid.db_qry_low_time;

    qtxt_ptr_array[2]		     = qtxt_array + 2;
    qtxt_ptr_array[2]->attr_number   = DM_3_QRYTEXT_KEY;
    qtxt_ptr_array[2]->attr_operator = DMR_OP_EQ;
    qtxt_ptr_array[2]->attr_value    = (char *) &qtxt_mode;

    qtxt_ptr_array[3]		     = qtxt_array + 3;
    qtxt_ptr_array[3]->attr_number   = DM_4_QRYTEXT_KEY;
    qtxt_ptr_array[3]->attr_operator = DMR_OP_EQ;
    qtxt_ptr_array[3]->attr_value    = (char *) &qtxt_seq;

    /*
    ** if permit is on a table or a view and does not convey table-wide
    ** privilege, we need to obtain description of a table/view and of its
    ** attributes
    */
    if (   (   prot_tuple->dbp_obtype == DBOB_TABLE
	    || prot_tuple->dbp_obtype == DBOB_VIEW
	    || prot_tuple->dbp_obtype == DBOB_INDEX
	   )
	&& BTcount((char *) prot_tuple->dbp_domset,
	       BITS_IN(prot_tuple->dbp_domset)) <
		    BITS_IN(prot_tuple->dbp_domset)
       )
    {
	/* Set up dmt_show call input parameters. */

	dmt_shw_cb.length = sizeof(DMT_SHW_CB);
	dmt_shw_cb.type = DMT_SH_CB;
	dmt_shw_cb.dmt_db_id = qeuq_cb->qeuq_db_id;
	dmt_shw_cb.dmt_session_id = qeuq_cb->qeuq_d_id;
	dmt_shw_cb.dmt_flags_mask = DMT_M_TABLE;
	dmt_shw_cb.dmt_char_array.data_address = (PTR)NULL;
	dmt_shw_cb.dmt_char_array.data_in_size = 0;
	dmt_shw_cb.dmt_char_array.data_out_size  = 0;
	STRUCT_ASSIGN_MACRO(prot_tuple->dbp_tabid, dmt_shw_cb.dmt_tab_id);

	/* allocate space for a DMT_TBL_ENTRY */
	ulm->ulm_psize = sizeof(DMT_TBL_ENTRY);
	if ((status = qec_malloc(ulm)) != E_DB_OK)
	{
	    *error = E_QE001E_NO_MEM;
	    return(status);
	}
	tbl_entry = (DMT_TBL_ENTRY *) ulm->ulm_pptr;
	dmt_shw_cb.dmt_table.data_address = (char *) tbl_entry;
	dmt_shw_cb.dmt_table.data_in_size = sizeof(DMT_TBL_ENTRY);

	/* issue DMT_SHOW call */

	if ((status = dmf_call(DMT_SHOW, &dmt_shw_cb)) != E_DB_OK)
	{
	    *error = dmt_shw_cb.error.err_code;
	    return(status);
	}

	/*
	** we need to allocate an array of pointers to DMT_ATT_ENTRY and an
	** array of DMT_ATT_ENTRY's for it to point to; remember that the number
	** of elements in these arays must be one greater than the
	** tbl_entry->tbl_attr_count (TID is treated as the 0'th attribute)
	*/
	ulm->ulm_psize =
	    sizeof(DMT_ATT_ENTRY *) * (tbl_entry->tbl_attr_count + 1);
	if ((status = qec_malloc(ulm)) != E_DB_OK)
	{
	    *error = E_QE001E_NO_MEM;
	    return(status);
	}
	
	dmt_shw_cb.dmt_attr_array.ptr_address  = (PTR) ulm->ulm_pptr;
	dmt_shw_cb.dmt_attr_array.ptr_in_count = tbl_entry->tbl_attr_count + 1;
	dmt_shw_cb.dmt_attr_array.ptr_size     = sizeof(DMT_ATT_ENTRY);

	ulm->ulm_psize =
	    sizeof(DMT_ATT_ENTRY) * (tbl_entry->tbl_attr_count + 1);
	if ((status = qec_malloc(ulm)) != E_DB_OK)
	{
	    *error = E_QE001E_NO_MEM;
	    return(status);
	}
	att_array = (DMT_ATT_ENTRY *) ulm->ulm_pptr;

	/*
	** now make elements of the array of pointers to DMT_ATT_ENTRY
	** structures point at the allocated structures
	*/
	att_entry = att_array;
	att_pptr = (DMT_ATT_ENTRY **) dmt_shw_cb.dmt_attr_array.ptr_address;
	for (i = tbl_entry->tbl_attr_count;
	     i >= 0;
	     i--, att_entry++, att_pptr++
	    )
	{
	    *att_pptr = att_entry;
	}
	     
	/* call dmt_show() to get description of attributes */
	dmt_shw_cb.dmt_flags_mask = DMT_M_ATTR;
	if ((status = dmf_call(DMT_SHOW, &dmt_shw_cb)) != E_DB_OK)
	{
	    *error = dmt_shw_cb.error.err_code;
	    return(status);
	}
    }

    /* here goes nada... */
    if (prot_tuple->dbp_flags & DBP_GRANT_PERM)
    {
	status = qeu_0make_grant_perm_text(&perm_str, prot_tuple,
	    att_array, ulm, error);
	if (DB_FAILURE_MACRO(status))
	{
	    return(status);
	}
    }
    else
    {
	DB_IIQRYTEXT	    txt_tuple, *read_qtxt_tuple = &txt_tuple;
	QEF_DATA	    in_qtxt_qefdata;
	
	/*
	** when generating query text using CREATE/DEFINE PERMIT syntax, we will
	** reuse the existing permit text to as large degree as possible - here
	** we set up qtxt_qeu to enable qeu_1make_create_permit_text() to read
	** existing query text tuples
	*/
	
	/* will read one tuple at a time */
	qtxt_qeu->qeu_count = 1;

	qtxt_qeu->qeu_output = &in_qtxt_qefdata;
	in_qtxt_qefdata.dt_next = 0;
	in_qtxt_qefdata.dt_size = sizeof(DB_IIQRYTEXT);
	in_qtxt_qefdata.dt_data = (PTR) read_qtxt_tuple;

	status = qeu_1make_create_permit_text(&perm_str, qef_cb, qtxt_qeu, ulm,
	    prot_tuple, read_qtxt_tuple, &qtxt_seq, att_array, error);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    }

    /* call RDF to repackage permit text into IIQRYTEXT format */
    {
	RDF_CB		    rdfcb, *rdf_cb = &rdfcb;
	RDR_RB		    *rdf_rb = &rdf_cb->rdf_rb;
	RDF_QT_PACK_RCB     pack_rcb;

	rdf_cb->rdf_info_blk	= NULL;
	rdf_rb->rdr_db_id	= qeuq_cb->qeuq_db_id;
	rdf_rb->rdr_session_id	= qeuq_cb->qeuq_d_id;

	rdf_rb->rdr_l_querytext = perm_str->db_t_count;
	rdf_rb->rdr_querytext   = (PTR) perm_str->db_t_text;
	rdf_rb->rdr_r1_distrib  = qef_cb->qef_c1_distrib;

	rdf_rb->rdr_status	=
	    (prot_tuple->dbp_flags & DBP_SQL_PERM) ? DBQ_SQL : 0;

	rdf_rb->rdr_instr	= RDF_USE_CALLER_ULM_RCB;

	rdf_rb->rdf_fac_ulmrcb	= (PTR) ulm;
	rdf_rb->rdr_fcb		= Qef_s_cb->qef_rdffcb;

	rdf_rb->rdr_qt_pack_rcb	= &pack_rcb;
	pack_rcb.rdf_qt_tuples	= &qtxt_tups;
	pack_rcb.rdf_qt_tuple_count = &qt_count;
	pack_rcb.rdf_qt_mode	= DB_PROT;

	/* If RDF can't get (QEF) memory, garbage collect and retry */
	while ( status == E_DB_OK &&
		(status = rdf_call(RDF_PACK_QTEXT, rdf_cb)) &&
		 rdf_cb->rdf_error.err_code == E_UL0005_NOMEM )
	{
	    status = qec_mfree(ulm);
	}
	if ( status )
	{
	    *error = rdf_cb->rdf_error.err_code;
	    return(status);
	}
    }

    /*
    ** if we were asked to generate text for a new permit, return address of
    ** the first element of the QEF_DATA list to the caller;
    ** otherwise,
    **	- copy text id from the IIPROTECT tuple representing the permit
    **    to the new IIQRYTEXT tuple(s),
    **	- delete IIQRYTEXT tuples corresponding to the old permit, and
    **	- insert newly generated tuples into IIQRYTEXT 
    */
    if ( status == E_DB_OK && new_perm )
    {
	*new_qtxt = qtxt_tups;
    }
    else if ( status == E_DB_OK )
    {
	QEF_DATA	*data_p;
	DB_IIQRYTEXT 	*qtxt_p;
	DB_QRY_ID	*qry_id = &prot_tuple->dbp_txtid;

	for (data_p = qtxt_tups; data_p; data_p = data_p->dt_next)
	{
	    qtxt_p = (DB_IIQRYTEXT *) data_p->dt_data;

	    qtxt_p->dbq_txtid.db_qry_high_time = qry_id->db_qry_high_time;
	    qtxt_p->dbq_txtid.db_qry_low_time  = qry_id->db_qry_low_time;
	}
	
	qtxt_qeu->qeu_klen = 3;
	status = qeu_delete(qef_cb, qtxt_qeu);
	if (status != E_DB_OK)
	{
	    *error = qtxt_qeu->error.err_code;
	    return(status);
	}

	qtxt_qeu->qeu_input = qtxt_tups;
	qtxt_qeu->qeu_count = qt_count;

	status = qeu_append(qef_cb, qtxt_qeu);
	if (status != E_DB_OK)
	{
	    *error = qtxt_qeu->error.err_code;
	    return(status);
	}
    }

    return(status);
}

/*
** Name:    qeu_assemble_text	- add a string of specified length to a
**			          DB_TEXT_STRING
**
** Description:
**	This function will add text passed by the caller to a DB_TEXT_STRING
**	buffer; if the buffer is not big enough to accomodate the new string, a
**	new buffer (its size will be at least twice the size of the current
**	buffer AND big enough to accomodate the new string) will be allocated,
**	and contents of the current buffer followed by the new string will be
**	copied into it.
**
** Input:
**	result 		addr of a ptr to the DB_TEXT STRING buffer being
**			constructed; *result be expected to be set to NULL
**			before the first buffer has been allocated
**	max_size	maximum number of bytes that can be accomodated by
**			result->db_t_text; will be set to 0 for the first string
**			being added
**	str		string to be added to IIQRYTEXT tuple
**	str_len		length of the string to be added to IIQRYTEXT tuple
**	ulm		must point to preopened memory stream if (write_to_mem);
**
** Output
**	out_str		if a bigger buffer had to be allocated, *out_str will
**			point at that buffer
**	error		filled in if an error occurred 
**
** Returns:
**	E_DB_{OK,ERROR,...}
**
** History:
**
**	01-feb-93 (andre)
**	    written on the ruins of qeu_bld_qtxt_tuple()
**	09-jul-93 (andre)
**	    cast db_t_count to i4  to keep ACC happy
*/
static DB_STATUS
qeu_assemble_text(
DB_TEXT_STRING	**result,
i4		*max_size,
char		*str,
i4  		str_len,
ULM_RCB         *ulm,
i4		*error)
{
    DB_TEXT_STRING	*buf = *result;
    DB_STATUS		status = E_DB_OK;

    /*
    ** figure out if the result buffer is big enough to accomodate the new
    ** string; if not, compute the size of the new buffer: it will be big enough
    ** and will be <size of old buffer> * 2**k
    */
    if (!buf || ( (i4) (buf->db_t_count + str_len) > *max_size))
    {
	/* compute size of the new buffer that will have to be allocated */

	i4		    need = (buf) ? buf->db_t_count + str_len : str_len;
	DB_TEXT_STRING	    *new_buf;

	if (!buf)	/* first time through */
	    *max_size = 240;
	
	for (; *max_size < need; *max_size <<= 1)
	;

	ulm->ulm_psize = *max_size + sizeof(DB_TEXT_STRING) - 1;
	if ((status = qec_malloc(ulm)) != E_DB_OK)
	{
	    *error = E_QE001E_NO_MEM;
	    return(status);
	}

	new_buf = (DB_TEXT_STRING *) ulm->ulm_pptr;

	if (buf && buf->db_t_count)
	{
	    /*
	    ** if not the first time through, copy text from the old
	    ** buffer into the new buffer
	    */

	    MEcopy((PTR) buf->db_t_text,
		new_buf->db_t_count = buf->db_t_count,
		(PTR) new_buf->db_t_text);
	}
	else
	{
	    new_buf->db_t_count = 0;
	}

	buf = *result = new_buf;
    }

    MEcopy((PTR) str, str_len, (PTR) (buf->db_t_text + buf->db_t_count));
    buf->db_t_count += str_len;

    return(E_DB_OK);
}

/*
** Name: qeu_perm_col_list - build query text describing columns to
**			     which a permit applies
**
** Description:
**    Given a map of attributes associated with a permit tuple, this
**    function will build query text describing this list and append
**    it to IIQRYTEXT catalog
**
** Input:
**	result 		addr of a ptr to the DB_TEXT_STRING buffer in which we
**			accumulate the text - will be passed to
**			qeu_assemble_text()
**	max_size	maximum number of bytes that can be accomodated by
**			result->db_t_text  - will be passed to
**			qeu_assemble_text()
**    	prot_tuple      IIPROTECT tuple describing a new permit
**          dbp_domset  map of attributes to which the new permit applies
**    	att_array	array of DMT_ATT_ENTRYs describing attributes of
**                      the table/view on which a permit was granted/created
**	ulm		an address of a preopened memory stream which will be
**			used by qeu_assemble_text() to allocate space for new
**			DB_TEXT_STRING bufeers if the current buffer cannot
**			accomodate new text
**
** Output:
**	error       filled in if an error occurred
**
** Returns:
**      E_DB_{OK,ERROR,...}
**
** History:
**
**      29-jul-92 (andre)
**          written
**	04-aug-92 (andre)
**	    added new_perm, ulm, and tup_stream to the interface;
**	    this was done as a part of changes to enable callers of
**	    qeu_construct_perm_text() to request that query text be built for a
**	    permit tuple without writing it out to disk
**	01-feb-93 (andre)
**	    qeu_bld_qtxt_tuple() is being replaced with qeu_assemble_text()
**	    which accumulates test into a DB_TEXT_STRING format; as a result, we
**	    remove qef_cb, qtxt_qeu, qtxt_tuple, new_perm, and tup_stream and
**	    add result, max_size, and error
**	18-aug-93 (andre)
**	    column names will be always delimited.  Eventually we will get our 
**	    hands of IIUG function that we help us determine whether a given 
**	    normalized name NEEDS to be expressed as a delimited identifier - 
**	    until such time we will always delimit.
**
**	    And another exciting development - because we may have to generate
**	    a column list for DEFINE PERMIT, I have added support for QUEL-style
**	    (meaning single-quoted) delimited identifiers in QUEL for use in 
**	    <column list> of DEFINE PERMIT statement.  As a result, we will 
**	    use single-quoted identifiers to express columns for QUEL permits 
**	    and double-quoted identifiers to express column names for SQL
**	    permits
*/
static DB_STATUS
qeu_perm_col_list(
DB_TEXT_STRING	**result,
i4		*max_size,
DB_PROTECTION	*prot_tuple,
DMT_ATT_ENTRY	*att_array,
ULM_RCB		*ulm,
i4		*error)
{
    i4		    	i;
    DB_STATUS	    	status = E_DB_OK;
    i4		    	*map_src, *map_dest;
    i4                  attrmap[DB_COL_WORDS];
    DMT_ATT_ENTRY	*att_entry;
    i4			need_comma = FALSE;
    i4			str_len;
    u_i4		idunorm_mode;

    /*
    ** if the permit was created in SQL, we'll use SQL delimited identifirs to
    ** represent column names; otherwise we will use QUEL-style (read 
    ** single-quoted) identifiers
    */
    idunorm_mode = (prot_tuple->dbp_flags & DBP_SQL_PERM)
	? (u_i4) (CUI_ID_DLM | CUI_ID_STRIP)
	: (u_i4) (CUI_ID_QUO| CUI_ID_STRIP);

    /* 
    ** if bit 0 in the prot_tuple->dbp_domset is set, we are granting
    ** a privilege on a table excluding some column(s); otherwise we are
    ** granting a privilege on some column(s)
    */
    if (BTtest(0, (char *) prot_tuple->dbp_domset))
    {
	/* 
	** we need to append keyword EXCLUDING and build a complement
	** of the map in prot_tuple->dbp_domset since this will give us
	** a map of attributes which will be excluded from the
	** privilege
	*/

	status = qeu_assemble_text(result, max_size,
	    "excluding (", (sizeof("excluding (") - 1), ulm, error);
	if (status != E_DB_OK)
	{
	    return(status);
	}

	for (
	     i = 0, map_src = prot_tuple->dbp_domset, map_dest = attrmap;

	     i < DB_COL_WORDS;

	     i++, map_src++, map_dest++
	    )
	{
	    *map_dest = ~*map_src;
	}
    }
    else
    {
	/* 
	** privileges on some column(s) are being granted; here we
	** simply copy prot_tuple->dbp_domset to attrmap so that the
	** following code does not need to distnguish the two cases
	*/

	status = qeu_assemble_text(result, max_size, 
	    "(", (sizeof("(") - 1), ulm, error);
	if (status != E_DB_OK)
	{
	    return(status);
	}

	for (
	     i = 0, map_src = prot_tuple->dbp_domset, map_dest = attrmap;

	     i < DB_COL_WORDS;

	     i++, map_src++, map_dest++
	    )
	{
	    *map_dest = *map_src;
	}
    }

    /* 
    ** now we just walk the attribute map, and add names of
    ** attributes to the IIQRYTEXT tuple(s)
    */
    for (
	 i = 0; (i = BTnext(i, (char *) attrmap, DB_MAX_COLS +1)) != -1;
	)
    {
        u_char		delim_ident[DB_MAX_DELIMID];
        u_i4		delim_ident_len;
	DB_ERROR	err_blk;

	att_entry = att_array + i;

	/*
	** compute the length of column name
	*/
	str_len = qec_trimwhite(sizeof(att_entry->att_name),
	    att_entry->att_name.db_att_name);

	delim_ident_len = DB_MAX_DELIMID;

	status = cui_idunorm((u_char *) att_entry->att_name.db_att_name, 
	    (u_i4 *) &str_len, delim_ident, &delim_ident_len, idunorm_mode,
	    &err_blk);
	if (DB_FAILURE_MACRO(status))
	{
	    *error = err_blk.err_code;
	    return(status);
	}

	if (need_comma)
	{
	    status = qeu_assemble_text(result, max_size,
		", ", (sizeof(", ") - 1), ulm, error);
	    if (status != E_DB_OK)
	    {
		return(status);
	    }
	}
	else
	{
	    need_comma = TRUE;
	}

	status = qeu_assemble_text(result, max_size, 
	    (char *) delim_ident, (i4) delim_ident_len, ulm, error);
	if (status != E_DB_OK)
	{
	    return(status);
	}
    }

    /* 
    ** add the closing paren at the end of column list 
    */
    status = qeu_assemble_text(result, max_size,
	") ", (sizeof(") ") - 1), ulm, error);
    if (status != E_DB_OK)
    {
	return(status);
    }

    return(status);
}

/*
** Name:  qeu_0make_grant_perm_text - build query text describing a
**				      permit which was created using 
**				      GRANT statement
**
** Description:
**    Given an IIPROTECT tuple describing a permit, this function will build a
**    DB_TEXT_STRING containing query text for the permit expressed using GRANT
**    syntax
**
** Input:
**	prot_tuple	IIPROTECT tuple describing a permit
**	    dbp_popset	privilege specified by the permit
**	    dbp_domset	attributes to which a permit applies (relevant
**			only for table/view permits)
**	att_array	array of DMT_ATT_ENTRYs describing table/view
**			attributes; will be set to NULL unless the permit 
**			conveys some column-specific privilege on a table 
**			or a view
**	ulm		if (new_perm) this is an address of a preopened memory
**			stream which shoud be used by qeu_assemble_text() to
**			allocate space for new QEF_DATA's and new IIQRYTEXT
**			tuples
**
** Output:
**	*result		will point at a DB_TEXT_STRING containing permit text
**	error		filled in if an error occurred
**
** Returns:
**	E_DB_{OK,ERROR,...}
**
** Side effects
**	calls qeu_assemble_text() which will allocate memory
**
** History:
**
**	29-jul-92 (andre)
**	    written
**	04-aug-92 (andre)
**	    added new_perm, ulm, and new_qtxt to the interface;
**	    this was done as a part of changes to enable callers of
**	    qeu_construct_perm_text() to request that query text be built for a
**	    permit tuple without writing it out to disk
**	16-sep-92 (andre)
**	    privilege map is built using bitwise ops so rather than check its
**	    contents using BTnext() we will use bitwise AND
**	02-feb-93 (andre)
**	    removed new_perm, new_qtxt, qef_cb, qtxt_qeu, and qtxt_tuple from
**	    the interface because qeu_assemble_text() is replacing
**	    qeu_bld_qtxt_tuple() and it does not try to build and write query
**	    text tuples to disk or memory
**	    
**	    added result and error
**	18-aug-93 (andre)
**	    When constructing text of a permit affected by revocation of 
**	    column-specific privilege or GRANT OPTION FOR privilege, schema 
** 	    name, object name, grantee name and column names (if any)
**	    will now be expressed as delimited identifiers.  
**
**	    Some time (maybe soon), we will figure out a way to use 
**	    functionality provided by IIUGdlm_ChkdlmBEobjct() to avoid 
**	    delimiting identifiers unless absolutely necessary - for now,
**	    "delimited" is the way to go
*/
static DB_STATUS
qeu_0make_grant_perm_text(
DB_TEXT_STRING	**result,
DB_PROTECTION	*prot_tuple,
DMT_ATT_ENTRY	*att_array,
ULM_RCB		*ulm,
i4		*error)
{
    DB_STATUS		status = E_DB_OK;
    i4			priv;
    char		*str;
    i4			str_len;
    i4			max_size = 0;
    u_char		delim_ident[DB_MAX_DELIMID];
    u_i4		delim_ident_len;
    DB_ERROR		err_blk;

    /*
    ** initialize *result to NULL to indicate that no text has been added
    ** to the buffer
    */
    *result = (DB_TEXT_STRING *) NULL;
    
    /* 
    ** permit on a dbevent, sequence or a dbproc or a (possibly
    ** non-table-wide) privilege on a table/view which was created
    ** using GRANT statement; if permit specifies a non-table-wide
    ** privilege, we will make use of attribute descriptions
    ** obtained from dmt_show()
    */

    /* start assembling query text */
    status = qeu_assemble_text(result, &max_size,
	"grant ", sizeof("grant ") - 1, ulm, error);
    if (status != E_DB_OK)
    {
        return(status);
    }

    /*
    ** recall that permit may specify only one privilege (SELECT
    ** privilege on table/view is represented by three bits:
    ** DB_RETRIEVE, DB_TEST, and DB_AGGREGATE), possibly with grant option
    */

    priv = prot_tuple->dbp_popset & ~((i4) DB_GRANT_OPTION);

    if (priv & (DB_RETRIEVE | DB_TEST | DB_AGGREGATE))
    {
	str = "select ";
	str_len = sizeof("select ") - 1;
    }
    else if (priv & DB_REPLACE)
    {
	str = "update ";
	str_len = sizeof("update ") - 1;
    }
    else if (priv & DB_COPY_FROM)
    {
	str = "copy_from ";
	str_len = sizeof("copy_from ") - 1;
    }
    else if (priv & DB_COPY_INTO)
    {
	str = "copy_into ";
	str_len = sizeof("copy_into ") - 1;
    }
    else if (priv & DB_DELETE)
    {
	str = "delete ";
	str_len = sizeof("delete ") - 1;
    }
    else if (priv & DB_APPEND)
    {
	str = "insert ";
	str_len = sizeof("insert ") - 1;
    }
    else if (priv & DB_REFERENCES)
    {
	str = "references ";
	str_len = sizeof("references ") - 1;
    }
    else if (priv & DB_EXECUTE)
    {
	str = "execute on procedure ";
	str_len = sizeof("execute on procedure ") - 1;
    }
    else if (priv & DB_EVREGISTER)
    {
	str = "register on dbevent ";
	str_len = sizeof("register on dbevent ") - 1;
    }
    else if (priv & DB_EVRAISE)
    {
	str = "raise on dbevent ";
	str_len = sizeof("raise on dbevent ") - 1;
    }
    else if (priv & DB_NEXT)
    {
	str = "next on sequence ";
	str_len = sizeof("next on sequence ") - 1;
    }

    status = qeu_assemble_text(result, &max_size, str, str_len, ulm, error);
    if (status != E_DB_OK)
    {
        return(status);
    }

    /*
    ** if a permit is being granted on a table or a view, we may need to
    ** add the column list
    */
    if (   prot_tuple->dbp_obtype == DBOB_TABLE 
	|| prot_tuple->dbp_obtype == DBOB_VIEW)
    {
	/*
	** if permit tuple specifies a non-table-wide privilege, append
	** column list
	*/
	if (BTcount((char *) prot_tuple->dbp_domset,
		BITS_IN(prot_tuple->dbp_domset)) <
	    BITS_IN(prot_tuple->dbp_domset)
	   )
	{
	    /* column names will be represented by delimited identifiers */
	    status = qeu_perm_col_list(result, &max_size, prot_tuple,
		att_array, ulm, error);
	    if (status != E_DB_OK)
	    {
		return(status);
	    }
	}

	/*
	** add "on " 
	*/
	status = qeu_assemble_text(result, &max_size, "on ", sizeof("on ") - 1,
	    ulm, error);
	if (status != E_DB_OK)
	{
	    return(status);
	}
    }

    /*
    ** determine length of schema name
    */
    str_len = qec_trimwhite(sizeof(prot_tuple->dbp_obown),
	prot_tuple->dbp_obown.db_own_name);
    
    delim_ident_len = DB_MAX_DELIMID;

    status = cui_idunorm((u_char *) &prot_tuple->dbp_obown, (u_i4 *) &str_len,
	delim_ident, &delim_ident_len, (u_i4) (CUI_ID_DLM | CUI_ID_STRIP),
	&err_blk);
    if (DB_FAILURE_MACRO(status))
    {
	*error = err_blk.err_code;
	return(status);
    }

    /*
    ** insert schema name expressed as a delimited identifier;
    */
    status = qeu_assemble_text(result, &max_size,
        (char *) delim_ident, (i4) delim_ident_len, ulm, error);
    if (status != E_DB_OK)
    {
        return(status);
    }

    /* insert a period separating schema name from object name */
    status = qeu_assemble_text(result, &max_size, ".", sizeof(".") - 1,
	ulm, error);
    if (status != E_DB_OK)
    {
        return(status);
    }
	
    /*
    ** determine length of object name
    */
    str_len = qec_trimwhite(sizeof(prot_tuple->dbp_obname),
	prot_tuple->dbp_obname.db_tab_name);

    delim_ident_len = DB_MAX_DELIMID;

    status = cui_idunorm((u_char *) &prot_tuple->dbp_obname, (u_i4 *) &str_len,
	delim_ident, &delim_ident_len, (u_i4) (CUI_ID_DLM | CUI_ID_STRIP),
	&err_blk);
    if (DB_FAILURE_MACRO(status))
    {
	*error = err_blk.err_code;
	return(status);
    }

    /*
    ** insert object name expressed as a delimited identifier
    */
    status = qeu_assemble_text(result, &max_size,
        (char *) delim_ident, (i4) delim_ident_len, ulm, error);
    if (status != E_DB_OK)
    {
        return(status);
    }

    /*
    ** figure out the grantee string - if it is PUBLIC, we will simply say
    ** "to public"; otherwise it will be "to <grantee type> <grantee_name>"
    */
    if (prot_tuple->dbp_gtype == DBGR_PUBLIC)
    {
        status = qeu_assemble_text(result, &max_size,
    	    " to public", sizeof(" to public") - 1, ulm, error);
        if (status != E_DB_OK)
        {
	    return(status);
	}
    }
    else
    {
        /* figure out grantee type - must be group, role, or user */
        switch (prot_tuple->dbp_gtype)
        {
	    case DBGR_GROUP:
	        str = " to group ";
	        str_len = sizeof(" to group ") - 1;
	        break;
	    case DBGR_APLID:
	        str = " to role ";
	        str_len = sizeof(" to role ") - 1;
	        break;
	    default:
	        /*
	        ** we don't know whether TO or TO USER was originally
	        ** specified + TO USER is NOT ANSI-compatible, so we'll
	        ** skip USER
	        */
	        str = " to ";
	        str_len = sizeof(" to ") - 1;
	        break;
        }

        status = qeu_assemble_text(result, &max_size, str, str_len, ulm, error);
        if (status != E_DB_OK)
        {
	    return(status);
        }

        /*
        ** determine length of grantee name
        */
        str_len = qec_trimwhite(sizeof(prot_tuple->dbp_owner),
	    prot_tuple->dbp_owner.db_own_name);

        delim_ident_len = DB_MAX_DELIMID;
    
        status = cui_idunorm((u_char *) &prot_tuple->dbp_owner, 
	    (u_i4 *) &str_len, delim_ident, &delim_ident_len, 
	    (u_i4) (CUI_ID_DLM | CUI_ID_STRIP), &err_blk);
        if (DB_FAILURE_MACRO(status))
        {
	    *error = err_blk.err_code;
	    return(status);
        }

        /* write out grantee name */
        status = qeu_assemble_text(result, &max_size,
	    (char *) delim_ident, (i4) delim_ident_len, ulm, error);
        if (status != E_DB_OK)
        {
	    return(status);
        }
    }

    /*
    ** and finally, check whether WITH GRANT OPTION needs to be
    ** specified
    */
    if (prot_tuple->dbp_popset & DB_GRANT_OPTION)
    {
        /* append WITH GRANT OPTION */
        status = qeu_assemble_text(result, &max_size,
	    " with grant option", sizeof(" with grant option") - 1, ulm, error);
        if (status != E_DB_OK)
        {
	    return(status);
        }
    }

    return(status);
}

/*
** Name:  qeu_1make_create_permit_text - build query text describing a
**				         permit which was created using 
**				         CREATE/DEFINE PERMIT statement
**
** Description:
**    Given an IIPROTECT tuple describing a permit which was created
**    using CREATE/DEFINE PERMIT statement and the text of the old permit that
**    is getting replaced, this function construct new permit text in
**    DB_TEXT_STRING format.
**
** Input:
**	qef_cb		    QEF session CB
**	qtxt_qeu	    QEU_CB which was used to open IIQRYTEXT for
**			    writing and shall be used to read tuples from
**			    IIQRYTEXT
**	ulm		    ULM request CB describing a previously opened stream
**	prot_tuple	    IIPROTECT tuple describing a permit
**	    dbp_domset	    attributes to which a permit applies
**	    dbp_flags	    if DBP_SQL_PERM is set, we are dealing with a permit
**			    created via CREATE PERMIT; otherwise, it was created
**			    via DEFINE PERMIT
**	read_qtxt_tuple	    IIQRYTEXT tuple which will be filled in when
**			    reading IIQRYTEXT using qtxt_qeu
**	qtxt_seq	    address of the sequence field which is used
**			    to contain the fourth key (sequence) used
**			    when accessing IIQRYTEXT
**	att_array	    array of DMT_ATT_ENTRYs describing table/view
**			    attributes; will be set to NULL unless the permit 
**			    conveys some column-specific privilege on a table 
**			    or a view
**
** Output:
**	*result		    will point at permit text stored in DB_TEXT_STRING
**			    format
**	error		    filled in if an error occurred
**
** Returns:
**	E_DB_{OK,ERROR,...}
**
** Side effects
**	calls qeu_assemble_text() which allocates memory
**
** History:
**
**	29-jul-92 (andre)
**	    written
**	04-aug-92 (andre)
**	    added new_perm and new_qtxt to the interface;
**	    this was done as a part of changes to enable callers of
**	    qeu_construct_perm_text() to request that query text be built for a
**	    permit tuple without writing it out to disk
**	28-jan-93 (andre)
**	    I thought I testsed it all, but it looks like soem things have
**	    fallen through the cracks:
**		- forgot to set qtxt_qeu->qeu_getnext to QEU_REPO before staring
**		  to look up IIQRYTEXT tuples
**	31-jan-93 (andre)
**	    made changes to handle recosntruction of permits created via DEFINE
**	    PERMIT (mainly this will involve dealing with one or more RANGE
**	    statements preceeding the DEFINE PERMIT which means that we canot
**	    make assumptions about whether any given string within the permit
**	    definition may end up being split between multiple IIQRYTEXT tuples)
**	01-feb-93 (andre)
**	    removed new_perm and new_qtxt from the interface becaue this
**	    function will never be called to build "new" permits.  Furthermore,
**	    very soon we will start calling RDF to build query text tuples, at
**	    which point all the new_perm silliness will disappear anyway
**	02-feb-93 (andre)
**	    "very soon" has come faster than I expected: qeu_bld_qtxt_tuple() is
**	    giving way to qeu_assemble_text() which will accumulate text in
**	    DB_TEXT_STRING format;
**
**	    write_qtxt_tuple will be removed from the interface because this
**	    function will no longer be in business of writing IIQRYTEXT tuples
**
**	    result will be added to the interface to enable this function to
**	    return accumulated DB_TEXT_STRING to the caller
**	18-aug-93 (andre)
**	    delimited identifiers are here - be prepared to encounter them while
**	    processing schema name, table name, column names and correlation 
**	    name
*/
static DB_STATUS
qeu_1make_create_permit_text(
DB_TEXT_STRING	**result,
QEF_CB		*qef_cb,
QEU_CB		*qtxt_qeu,
ULM_RCB         *ulm,
DB_PROTECTION   *prot_tuple,
DB_IIQRYTEXT    *read_qtxt_tuple,
i4		*qtxt_seq,
DMT_ATT_ENTRY	*att_array,
i4         *error)
{
    /* 
    ** permit was created using CREATE/DEFINE PERMIT - we will reuse as much
    ** of the original query text as we can.  This means that we will
    ** reuse the text of the existing permit query text starting at 
    ** the beginning up to and including the table name and the portion
    ** starting at TO <grantee> and to the end.
    */
    DB_STATUS	    status = E_DB_OK;
    char	    *c, *c1, *c2;
    char	    *char_lim;
    QEF_DATA	    *first = (QEF_DATA *) NULL;
    QEF_DATA	    *last  = (QEF_DATA *) NULL;
    QEF_DATA	    *cur;
    DB_TEXT_STRING  *in_txt = &read_qtxt_tuple->dbq_text;
    i4		    max_size = 0;
    /* will be reset to TRUE once we skip past TO (as in "TO <grantee>" */
    bool	    reached_grantee = FALSE;

    /* set *result to NULL to indicate that no text has been accumulated yet */
    *result = (DB_TEXT_STRING *) NULL;

    /*
    ** NOTE: do NOT(!!!) reset qtxt_qeu->qeu_getnext to QEU_NOREPO until you are
    ** done fetching IIQRYTEXT tuples (for convenience' sake, I'll do it at the
    ** end of this function)
    */
    qtxt_qeu->qeu_getnext = QEU_REPO;

    /* all 4 keys will be used for retrieving IIQRYTEXT tuples */
    qtxt_qeu->qeu_klen = 4;
    *qtxt_seq = 0;

    /* get the first tuple to get things going */
    status = qeu_get(qef_cb, qtxt_qeu);
    if (status != E_DB_OK)
    {
	*error = qtxt_qeu->error.err_code;
	return(status);
    }

    c = (char *) in_txt->db_t_text;
    char_lim = c + in_txt->db_t_count;
    
    if (~prot_tuple->dbp_flags & DBP_SQL_PERM)
    {
	/*
	** this permit was created using DEFINE PERMIT - skip leading RANGE
	** statement(s)
	*/

	    /* will be reset to TRUE when we skip past ALL RANGE statements */
	bool	    done_with_rng_stmts = FALSE;
	    /*
	    ** will be set to TRUE when we are in the middle of scanning
	    ** through a RANGE statement
	    */
	bool	    rng_stmt = FALSE;
	
	do
	{
	    if (c == char_lim)
	    {
		/*
		** finished scaning current tuple - write it out, then read the
		** next one
		*/
		status = qeu_assemble_text(result, &max_size,
		    (char *) in_txt->db_t_text, (i4) in_txt->db_t_count,
		    ulm, error);
		if (status != E_DB_OK)
		{
		    return(status);
		}

		(*qtxt_seq)++;
		status = qeu_get(qef_cb, qtxt_qeu);
		if (status != E_DB_OK)
		{
		    *error = qtxt_qeu->error.err_code;
		    return(status);
		}

		c = (char *) in_txt->db_t_text;
		char_lim = c + in_txt->db_t_count;
	    }

	    if (rng_stmt)
	    {
		/*
		** skip to the next EOL or end of IIQRYTEXT tuple, whichever
		** comes first
		*/

		for (; (c < char_lim && CMcmpcase(c, "\n")); CMnext(c))
		;

		if (c < char_lim)
		{
		    /* must have reached the end of the range statement */
		    rng_stmt = FALSE;

		    /* skip past the EOL */
		    CMnext(c);
		}
	    }

	    if (!rng_stmt)
	    {
		/*
		** we have just finished scanning past a RANGE statement
		** (or we have seen none at all so far) - skip past white chars;
		** if the first non-white char is R, assume we are dealing with
		** RANGE statement; otherwise, this must be the beginning of
		** DEFINE PERMIT
		*/

		for (; (c < char_lim && CMwhite(c)); CMnext(c));

		if (c < char_lim)
		{
		    /*
		    ** found a non-white char: if 'r', it is a beginning of
		    ** another RANGE statement; otherwise we are done with them
		    */

		    if (!CMcmpcase(c, "r"))
			rng_stmt = TRUE;
		    else
		    {
			done_with_rng_stmts = TRUE;

			/*
			** if we scanned at least one char in the current
			** buffer, write out scanned chars
			*/
			if (c > (char *) in_txt->db_t_text)
			{
			    status = qeu_assemble_text(result, &max_size,
				(char *) in_txt->db_t_text,
				c - (char *) in_txt->db_t_text, ulm, error);
			    if (status != E_DB_OK)
			    {
				return(status);
			    }
			}
		    }
		}
	    }
	} while (!done_with_rng_stmts);
    }

    /* skip past the first 4 words (CREATE/DEFINE PERMIT <privilege> ON) */
    status = qeu_0skip_words(4, &c, &char_lim, qef_cb, qtxt_qeu,
	in_txt, ulm, qtxt_seq, result, &max_size, (bool) TRUE, error);
    if (DB_FAILURE_MACRO(status))
	return(status);

    /* 
    ** we have skipped past ON (in ON <object name>);
    ** next we need to skip past the object name which may be prefixed with
    ** owner name if the permit was created using SQL 
    */

    /* 
    ** find beginning of the next word;
    ** 
    ** if the permit was created using SQL, it may be a regular or delimited
    ** identifier or a string representing the name of the schema
    ** 
    ** it may also be an identifier (regular or delimited .. yes, as 
    ** a matter of fact, I may be jumping ahead of myself here)
    ** representing name of a table/view on which a privilege is being
    ** created
    */

    status = qeu_1skip_white_chars(&c, &char_lim, qef_cb, qtxt_qeu,
	in_txt, ulm, qtxt_seq, result, &max_size, (bool) TRUE, error);
    if (DB_FAILURE_MACRO(status))
	return(status);

    if (CMnmstart(c))
    {
	/* skip past a regular identifier */
	status = qeu_0skip_words(1, &c, &char_lim, qef_cb, qtxt_qeu,
	    in_txt, ulm, qtxt_seq, result, &max_size, (bool) TRUE, error);
    }
    else
    {
	char	    delim = *c;

	/* skip past a delimited identifier or a string */
	status = qeu_2skip_delimited_string(&c, &char_lim, &delim, qef_cb,
	    qtxt_qeu, in_txt, ulm, qtxt_seq, result, &max_size, (bool) TRUE,
	    error);
    }

    if (DB_FAILURE_MACRO(status))
	return(status);

    status = qeu_1skip_white_chars(&c, &char_lim, qef_cb, qtxt_qeu,
	in_txt, ulm, qtxt_seq, result, &max_size, (bool) TRUE, error);
    if (DB_FAILURE_MACRO(status))
	return(status);

    /*
    ** the next char can be a period separating schema name from the table name;
    ** if so, skip past it and past the object name
    */
    if (!CMcmpcase(c, "."))
    {
	CMnext(c);

	status = qeu_assemble_text(result, &max_size, ".", sizeof(".") - 1,
	    ulm, error);
	if (status != E_DB_OK)
	{
	    return(status);
	}

	status = qeu_1skip_white_chars(&c, &char_lim, qef_cb, qtxt_qeu,
	    in_txt, ulm, qtxt_seq, result, &max_size, (bool) TRUE, error);
	if (DB_FAILURE_MACRO(status))
	    return(status);

	/* now skip past the name of the object */
	if (CMnmstart(c))
	{
	    /* skip past a regular identifier */
	    status = qeu_0skip_words(1, &c, &char_lim, qef_cb, qtxt_qeu,
		in_txt, ulm, qtxt_seq, result, &max_size, (bool) TRUE, error);
	}
	else
	{
	    char	    delim = *c;

	    /* skip past a delimited identifier */
	    status = qeu_2skip_delimited_string(&c, &char_lim, &delim, qef_cb,
		qtxt_qeu, in_txt, ulm, qtxt_seq, result, &max_size, (bool) TRUE,
		error);
	}

	if (DB_FAILURE_MACRO(status))
	    return(status);

	status = qeu_1skip_white_chars(&c, &char_lim, qef_cb, qtxt_qeu,
	    in_txt, ulm, qtxt_seq, result, &max_size, (bool) TRUE, error);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    }

    /* 
    ** c now points at the first non-white character following the object name;
    ** next token could be a '(' indicating the beginning of the column list,
    ** keyword EXCLUDING followed by a column list, a keyword TO followed by
    ** grantee specification, or the correlation name, which could be expressed 
    ** using a regular or a delimited identifier; if correlation name was
    ** specified, we want to attach it to the IIQRYTEXT tuple we are
    ** constructing, but if a column list was specified, we will want to skip
    ** past it
    */
    
    if (CMnmstart(c))
    {
	/*
	** we will invoke qeu_0skip_words() to get the next word and then
	** compare it with TO and EXCLUDING; if it matches neither of the
	** two, it must be the correlation name, otherwise, we will "forget"
	** that we saw it
	*/

	char		*c1;
	i4		save_offset = (*result)->db_t_count;
	i4		new_offset;
	bool		corr_name = FALSE;
	
	status = qeu_0skip_words(1, &c, &char_lim, qef_cb, qtxt_qeu,
	    in_txt, ulm, qtxt_seq, result, &max_size, (bool) TRUE, error);
	if (DB_FAILURE_MACRO(status))
	    return(status);

	c1 = (char *) (*result)->db_t_text + save_offset;
	new_offset = (*result)->db_t_count;
	
	if (   !CMcmpnocase(c1, "t")
	    && (new_offset - save_offset) == sizeof("to") - 1)
	{
	    /* looks like TO, check the second char */
	    CMnext(c1);
	    
	    if (CMcmpnocase(c1, "o"))
	    {
		corr_name = TRUE;
	    }
	    else
	    {
		/*
		** OK, so we are lying a bit, but we WILL skip past the white
		** spaces following "TO" at which point we will be staring at
		** the grantee name at which point we do want to stop skipping
		** chars
		*/
		reached_grantee = TRUE;
	    }
	}
	else if (   !CMcmpnocase(c1, "e")
		 && (new_offset - save_offset) == sizeof("excluding") - 1
		)
	{
	    char	    *excl;

	    /* could be EXCLUDING, check the remaining chars */

	    /*
	    ** we already know that the first letter is "e", now
	    ** check the rest
	    */
	    excl = "xcluding";
	    for (CMnext(c1); !CMcmpnocase(c1, excl); CMnext(c1), CMnext(excl))
	    ;

	    /*
	    ** if we terminated the search before reaching end of the
	    ** token, it must not have been the keyword EXCLUDING
	    */
	    if (*excl != EOS)
		corr_name = TRUE;
	}
	else
	{
	    corr_name = TRUE;
	}

	/*
	** if this was a correlation name, copy white chars which follow it into
	** the output buffer; otherwise, reset number of chars in the
	** buffer to its value before we scanned the last word and skip the
	** white chars following the keyword WITHOUT copying them to the output
	** buffer
	*/
	if (!corr_name)
	    (*result)->db_t_count = save_offset;

	status = qeu_1skip_white_chars(&c, &char_lim, qef_cb, qtxt_qeu,
	    in_txt, ulm, qtxt_seq, result, &max_size, corr_name, error);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    }
    else if (!CMcmpcase(c, "\""))
    {
	/*
	** skip past the correlation name and white char(s) that follow it and
	** copy them to the output buffer
	*/

	status = qeu_2skip_delimited_string(&c, &char_lim, "\"", qef_cb,
	    qtxt_qeu, in_txt, ulm, qtxt_seq, result, &max_size, (bool) TRUE,
	    error);
	if (DB_FAILURE_MACRO(status))
	    return(status);

	status = qeu_1skip_white_chars(&c, &char_lim, qef_cb, qtxt_qeu,
	    in_txt, ulm, qtxt_seq, result, &max_size, (bool) TRUE, error);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    }
    
    /*
    ** At this point we may be looking at
    ** - keywords TO or EXCLUDING (if the correlation name was present),
    ** - the opening paren of the column list, or
    ** - <grantee> (reached_grantee will be set).
    **
    ** we want to skip past EXCLUDING (if found), <column list> (if found), and
    ** TO (if haven't already)
    */
	
    if (!reached_grantee)
    {
	/*
	** if original query text contained "EXCLUDING", and we have
	** not skipped it yet, skip it now
	*/
	if (!CMcmpnocase(c, "e"))
	{
	    status = qeu_0skip_words(1, &c, &char_lim, qef_cb, qtxt_qeu,
		in_txt, ulm, qtxt_seq, result, &max_size, (bool) FALSE, error);
	    if (DB_FAILURE_MACRO(status))
		return(status);

	    status = qeu_1skip_white_chars(&c, &char_lim, qef_cb, qtxt_qeu,
		in_txt, ulm, qtxt_seq, result, &max_size, (bool) FALSE, error);
	    if (DB_FAILURE_MACRO(status))
		return(status);
	}

	/* if original query text contained a <column list>, skip it now */
	if (!CMcmpcase(c, "("))
	{
	    CMnext(c);

	    status = qeu_1skip_white_chars(&c, &char_lim, qef_cb, qtxt_qeu,
		in_txt, ulm, qtxt_seq, result, &max_size, (bool) FALSE, error);
	    if (DB_FAILURE_MACRO(status))
		return(status);

	    do
	    {
		/* skip a column name */
		if (CMnmstart(c))
		{
		    status = qeu_0skip_words(1, &c, &char_lim, qef_cb, qtxt_qeu,
			in_txt, ulm, qtxt_seq, result, &max_size, (bool) FALSE,
			error);
		}
		else
		{
		    char	delim = *c;

		    status = qeu_2skip_delimited_string(&c, &char_lim, &delim,
			qef_cb, qtxt_qeu, in_txt, ulm, qtxt_seq, result,
			&max_size, (bool) FALSE, error);
		}
		
		if (DB_FAILURE_MACRO(status))
		    return(status);

		/* skip white chars */
		status = qeu_1skip_white_chars(&c, &char_lim, qef_cb, qtxt_qeu,
		    in_txt, ulm, qtxt_seq, result, &max_size, (bool) FALSE,
		    error);
		if (DB_FAILURE_MACRO(status))
		    return(status);

		if (!CMcmpcase(c, ","))
		{
		    /* skip the comma and any white chars that may follow it */
		    CMnext(c);

		    status = qeu_1skip_white_chars(&c, &char_lim, qef_cb,
			qtxt_qeu, in_txt, ulm, qtxt_seq, result, &max_size,
			(bool) FALSE, error);
		    if (DB_FAILURE_MACRO(status))
			return(status);
		}

	    } while (CMcmpcase(c, ")"));

	    /* skip the closing paren and any white chars */
	    CMnext(c);
	    
	    status = qeu_1skip_white_chars(&c, &char_lim, qef_cb, qtxt_qeu,
		in_txt, ulm, qtxt_seq, result, &max_size, (bool) FALSE,
		error);
	    if (DB_FAILURE_MACRO(status))
		return(status);
	}

	if (!CMcmpnocase(c, "t"))
	{
	    /* skip keyword TO and any white chars */
	    status = qeu_0skip_words(1, &c, &char_lim, qef_cb, qtxt_qeu,
		in_txt, ulm, qtxt_seq, result, &max_size, (bool) FALSE, error);
	    if (DB_FAILURE_MACRO(status))
		return(status);

	    status = qeu_1skip_white_chars(&c, &char_lim, qef_cb, qtxt_qeu,
		in_txt, ulm, qtxt_seq, result, &max_size, (bool) FALSE,
		error);
	    if (DB_FAILURE_MACRO(status))
		return(status);

	    /* c now points at the beginning of <grantee> */
	    reached_grantee = TRUE;
	}
    }

    /*
    ** c now points at the <grantee>.  we need to append <column list> followed
    ** by keyword "TO" to the permit text being built, then we will append the
    ** remainder (starting with c) of the original query text
    */

    /*
    ** if permit tuple specifies a non-table-wide privilege, append
    ** column list
    */
    if (BTcount((char *) prot_tuple->dbp_domset,
          BITS_IN(prot_tuple->dbp_domset)) < BITS_IN(prot_tuple->dbp_domset)
       )
    {
        status = qeu_perm_col_list(result, &max_size, prot_tuple, 
            att_array, ulm, error);
        if (DB_FAILURE_MACRO(status))
            return(status);
    }

    status = qeu_assemble_text(result, &max_size, " to ", sizeof(" to ") - 1,
	ulm, error);
    if (DB_FAILURE_MACRO(status))
	return(status);

    for (;;)
    {
        if (char_lim > c)
        {
	    status = qeu_assemble_text(result, &max_size, c, char_lim -c,
		ulm, error);
	    if (DB_FAILURE_MACRO(status))
		return(status);
        }

        /* try to get the next tuple; remember to increment qtxt_seq */
        (*qtxt_seq)++;
        status = qeu_get(qef_cb, qtxt_qeu);
        if (status != E_DB_OK)
        {
	    if (qtxt_qeu->error.err_code == E_QE0015_NO_MORE_ROWS)
	    {
	        /* 
	        ** no more tuples to read; we are done copying existing
		** query text
	        */
	        status = E_DB_OK;
	        break;
	    }
	    else
	    {
	        *error = qtxt_qeu->error.err_code;
	        return(status);
	    }
        }

        /* 
        ** now we need to set c to point at the beginning of the
        ** query text and char_lim to a char immediately following the
        ** query text
        */
        c = (char *) read_qtxt_tuple->dbq_text.db_t_text;
        char_lim = c + read_qtxt_tuple->dbq_text.db_t_count;
    }

    /*
    ** we were always fetching query text by key; now that we are done, reset
    ** qeu_getnext to avoid adverse effects on other functions using the same
    ** QEU_CB
    */
    qtxt_qeu->qeu_getnext = QEU_NOREPO;

    /* we are done */
    return(status);
}

/*
**  Name: qeu_prv_not_revoked_err - produce a warning message notifying user
**				    that some/all of the privileges specified in
**				    REVOKE statement could not be revoked
**				    because they were never granted
**
** Description:
**	Produce a warning message notifying user that some/all of the privileges
**	specified in REVOKE statement could not be revoked because they were
**	never granted
**
** Input:
**	priv			DB_PROTECTION structure that describes the
**				REVOKE statement
**	    dbp_gtype		alleged grantee type
**	    dbp_owner		alleged grantee name
**	    dbp_grantor		alleged grantor of privileges
**	    dbp_obname		object name
**	qeuq_cb			QEU qeury cb
**	    qeuq_permit_mask	information about privilege-related statements
**		QEU_REVOKE_ALL	REVOKE ALL [PRIVILEGES] was specified
**	user_lacked_privs	TRUE if the user attempting to REVOKE 
**				privilege(s) on another user's object possessed
**				no privileges on that object
**
** Output:
**	error			filled in by qef_error()
**
** Returns:
**	none
**
** Side effects:
**	none
**
** Hostory:
**
**	05-aug-92 (andre)
**	    written
**	02-nov-93 (andre)
**	    added user_lacked_privs.  If TRUE, rather than telling the user that
**	    some of the privileges were not revoked, we will issue a message
*	    saying that either the object on which he tried to revoke privileges
**	    did not exist or he lacked requried privileges on it
**	25-nov-93 (stephenb)
**	    Audit failure to revoke privs with qeu_secaudit() 
**	    when user_lacked_privs.
**	14-may-02 (inkdo01)
**	    Updated to add sequence privileges.
*/
static VOID
qeu_prv_not_revoked_err(
DB_PROTECTION	    *priv,
QEUQ_CB		    *qeuq_cb,
bool		    user_lacked_privs,
i4		    *error)
{
    char	    *grntee_type;
    i4		    grntee_tlen;
    char	    *grntee_name;
    i4		    grntee_nlen;
    i4		    grntor_len;
    i4		    obj_name_len;

    switch (priv->dbp_gtype)
    {
	case DBGR_USER:
	    grntee_type = "user";
	    grntee_tlen = sizeof("user") - 1;
	    break;
	case DBGR_GROUP:
	    grntee_type = "group";
	    grntee_tlen = sizeof("group") - 1;
	    break;
	case DBGR_APLID:
	    grntee_type = "role";
	    grntee_tlen = sizeof("role") - 1;
	    break;
	default:	/* PUBLIC */
	    grntee_type = "";
	    grntee_tlen = 0;
	    break;
    }

    if (priv->dbp_gtype == DBGR_PUBLIC)
    {
	grntee_name = "PUBLIC";
	grntee_nlen = sizeof("PUBLIC") - 1;
    }
    else
    {
	grntee_name = (char *) &priv->dbp_owner;
	grntee_nlen = qec_trimwhite(sizeof(priv->dbp_owner), grntee_name);
    }

    grntor_len = qec_trimwhite(sizeof(priv->dbp_grantor),
	(char *) &priv->dbp_grantor);

    obj_name_len = qec_trimwhite(sizeof(priv->dbp_obname),
	(char *) &priv->dbp_obname);

    /*
    ** if the user attempting to revoke privilege(s) on an object himself
    ** possessed no privileges on it, a suitably vague message saying that
    ** either the object did not exist or the user lacked required privileges
    ** on it
    */
    if (user_lacked_privs)
    {
	i4		err_number;
	SXF_EVENT	auditevent;
	i4		msg_id;
	DB_ERROR	e_error;

	if (qeuq_cb->qeuq_permit_mask & QEU_DBP_PROTECTION)
	{
	    err_number = E_US0890_2192_INACCESSIBLE_DBP;
	    auditevent = SXF_E_PROCEDURE;
	    msg_id = I_SX2019_PROT_DBP_DROP;
	}
	else if (qeuq_cb->qeuq_permit_mask & QEU_EV_PROTECTION)
	{
	    err_number = E_US088F_2191_INACCESSIBLE_EVENT;
	    auditevent = SXF_E_EVENT;
	    msg_id = I_SX2031_PROT_EV_DROP;
	}
	else if (qeuq_cb->qeuq_permit_mask & QEU_SEQ_PROTECTION)
	{
	    err_number = E_US0891_2193_INACCESSIBLE_SEQ;
	    auditevent = SXF_E_EVENT;
	    msg_id = I_SX204A_PROT_SEQ_DROP;
	}
	else
	{
	    err_number = E_US088E_2190_INACCESSIBLE_TBL;
	    auditevent = SXF_E_TABLE;
	    msg_id = I_SX2018_PROT_TAB_DROP;
	}

	(VOID) qef_error(err_number, 0L, E_DB_WARN, error, &qeuq_cb->error, 3,
	    (i4) (sizeof("REVOKE") - 1), (i4 *) "REVOKE",
	    qec_trimwhite(sizeof(priv->dbp_obown), (char *) &priv->dbp_obown),
	    (i4 *) &priv->dbp_obown,
	    (i4) obj_name_len, (i4 *) &priv->dbp_obname);
	/*
	** Audit failure to revoke privilege
	*/
	if ( Qef_s_cb->qef_state & QEF_S_C2SECURE )
	    (VOID) qeu_secaudit(FALSE, (CS_SID) 0, 
		(char *)&priv->dbp_obname, &priv->dbp_obown,
		sizeof(priv->dbp_obname), auditevent, msg_id, 
		SXF_A_FAIL | SXF_A_CONTROL, &e_error);
    }
    /*
    ** otherwise, produce a warning message indicating that no privileges were
    ** revoked because none existed and leave the loop since no
    ** privileges were revoked
    */
    else if (qeuq_cb->qeuq_permit_mask & QEU_REVOKE_ALL)
    {
	(VOID) qef_error(E_QE0253_ALL_PRIV_NONE_REVOKED, 0L, E_DB_WARN,
	    error, &qeuq_cb->error, 4,
	    (i4) grntee_tlen, (i4 *) grntee_type,
	    (i4) grntee_nlen, (i4 *) grntee_name,
	    (i4) obj_name_len, (i4 *) &priv->dbp_obname,
	    (i4) grntor_len, (i4 *) &priv->dbp_grantor);
    }
    else
    {
	(VOID) qef_error(E_QE0254_SOME_PRIV_NOT_REVOKED, 0L, E_DB_WARN,
	    error, &qeuq_cb->error, 4,
	    (i4) obj_name_len, (i4 *) &priv->dbp_obname,
	    (i4) grntee_tlen, (i4 *) grntee_type,
	    (i4) grntee_nlen, (i4 *) grntee_name,
	    (i4) grntor_len, (i4 *) &priv->dbp_grantor);
    }

    return;
}

/*
** qeu_mpt2_qual() -	determine whether an existing temp table tuple matches
**			one we have constructed (and are about to append to the
**			temp table)
**
** Description:
**	This function is passed by qeu_match_privs() inside QEU_CB to
**	qeu_get() to be used in qualifying tuples returned by dmr_get()
**	when we are reading existing QEU_MATCH_PRIV_INFO tuples looking for
**	tuple(s) that would enable us to claim that a new tuple N does not need
**	to be inserted into the temp table.  To qualify, a tuple T passed by
**	dmr_get() must have
**	       T.qeu_gtype		== DBGR_USER
**	    && T.qeu_authid		== N.qeu_authid
**	    && T.qeu_flags		== N.qeu_flags
**	    && (   T.qeu_priv == DB_EXECUTE
**		|| T.qeu_priv == DB_EVRAISE
**		|| T.qeu_priv == DB_EVREGISTER
**		|| T.qeu_priv == DB_NEXT
**		|| (   (T.qeu_priv == DB_REPLACE || T.qeu_priv == DB_REFERENCES)
**		    && T.qeu_attrmap has a non-empty intersection with
**		       N.qeu_attrmap
**		   )
**		|| (T.qeu_attrmap is a superset of N.qeu_attrmap)
**	       )
**
**	If this condition is satisfied, this function will return DMR_F_RETURN
**	to indicate to dmr_get() to give this tuple to qeu_match_privs();
**	otherwise DMR_F_NEXT will be returned and DMF will go on to the next
**	tuple
**	
**  Input:
**	new_info    address of a QEU_MATCH_PRIV_INFO tuple which will be
**		    appended to the temp table unless we find matching tuple(s)
**		    which have been previously entered into the table
**	info_tuple  ptr to the QEU_MATCH_PRIV_INFO tuple being considered
**	
**  Output:
**	None
**	
**  Returns:
**	DMR_F_NEXT if tuple does not match, DMR_F_RETURN otherwise.
**
**  History
**
**	20-aug-92 (andre)
**	    written
**	11-dec-92 (andre)
**	    for tuples specifying REFERENCE privilege, we will be satisfied if
**	    the attribute maps have a non-empty intersection (just as it is
**	    done for UPDATE)
*/
static i4
qeu_mpt2_qual(
QEU_MATCH_PRIV_INFO	*new_info,
QEU_MATCH_PRIV_INFO	*info_tuple)
{
    register i4	i;

    if (   info_tuple->qeu_flags != new_info->qeu_flags
#ifndef	TEMP_TABLES_WORK
	|| info_tuple->qeu_priv  != new_info->qeu_priv
#endif
	|| info_tuple->qeu_gtype != DBGR_USER
	|| MEcmp((PTR) &info_tuple->qeu_authid, (PTR) &new_info->qeu_authid,
		 sizeof(DB_OWN_NAME))
       )
    {
	return(DMR_F_NEXT);
    }

    /*
    ** if this tuple describes a dbproc or dbevent privilege, there is no reason
    ** to look at the attribute maps - we found our tuple
    */
    if (   info_tuple->qeu_priv == DB_EXECUTE
	|| info_tuple->qeu_priv == DB_EVRAISE
	|| info_tuple->qeu_priv == DB_EVREGISTER
	|| info_tuple->qeu_priv == DB_NEXT
       )
    {
	return(DMR_F_RETURN);
    }

    /* if we got here, we are dealing with table privileges */
    
    /*
    ** if the tuple describes UPDATE or REFERENCES privilege, we want attribute
    ** maps to have a non-empty intersection; otherwise qeu_attrmap in the tuple
    ** passed by dmr_get() must be a superset of that passed by the caller
    */
    if (   info_tuple->qeu_priv == DB_REPLACE
	|| info_tuple->qeu_priv == DB_REFERENCES)
    {
	for (i = 0; i < DB_COL_WORDS; i++)
	{
	    if (new_info->qeu_attrmap[i] & info_tuple->qeu_attrmap[i])
	    {
		/* non-empty intersection */
		return(DMR_F_RETURN);
	    }
	}
	
	/* empty attribute map intersection */
	return(DMR_F_NEXT);
    }
    else
    {
	for (i = 0; i < DB_COL_WORDS; i++)
	{
	    if (new_info->qeu_attrmap[i] & ~info_tuple->qeu_attrmap[i])
	    {
		/*
		** qeu_attrmap in dmr_get()'s tuple is NOT a superset of that
		** passed by the caller (qeu_match_privs())
		*/
		return(DMR_F_NEXT);
	    }
	}

	/*
	** qeu_attrmap in dmr_get()'s tuple is a superset of that passed by the
	** caller (qeu_match_privs())
	*/
	return(DMR_F_RETURN);
    }
}

#ifndef	    TEMP_TABLES_WORK
/*
** Name: qeu_mpt1_qual() - a function to qualify QEU_MATCH_PRIV_INFO tuples
**
** Description:
**	This function implements a temporary workaround which was made necessary
**	by the fact that BTREE temp tables are not working.  It will be passed
**	to qeu_get and will first qualify a tuple based on privilege and depth.
**	If they pass this test and qeu_depth>0, we will also compare qeu_gtype
**	and qeu_authid.
**
** Input:
**	
**	key_info	tuple passed by qeu_match_privs() which contains the
**			values by which a newly read tuple will be qualified
**	info_tuple	tuple fetched by dmr_get()
** Output:
**	none
**
** Returns:
**	DMR_F_NEXT if the row is to be skipped and DMR_F_RETURN otherwise
**
** History:
**
**	21-aug-92 (andre)
**	    written
*/
static i4
qeu_mpt1_qual(
QEU_MATCH_PRIV_INFO	*key_info,
QEU_MATCH_PRIV_INFO	*info_tuple)
{

    if (   key_info->qeu_priv != info_tuple->qeu_priv
	|| key_info->qeu_depth != info_tuple->qeu_depth
	|| (   key_info->qeu_depth > 0
	    && (   key_info->qeu_gtype != info_tuple->qeu_gtype
		|| MEcmp((PTR) &key_info->qeu_authid,
			 (PTR) &info_tuple->qeu_authid,
			 sizeof(DB_OWN_NAME))
	       )
	   )
       )
    {
	return(DMR_F_NEXT);
    }
    else
    {
	return(DMR_F_RETURN);
    }
}
#endif

/*
** Name:  qeu_depth()  - compute "depth" of a permit
**
** Description:
**	This function will recompute depth of permit(s) affected by
**	destruction/alteration of a existing permit.  It should be called if 
**	the permit that was dropped or altered specified a grantable privilege 
**	(dropped_perm->dbp_popset & DB_GRANT_OPTION).  
**
**	When a new permit is created by user U on another user's object, 
**	permit's depth is computed based on depth's of existing permits 
**	granting GRANT OPTION FOR that privilege on the object to U or 
**	to PUBLIC.  Thus we can claim that permit of depth N will not 
**	depend on a permit of depth N+k (k>=0).  However, when a permit 
**	specifying a grantable privilege is dropped/altered, depth of some of 
**	the remaining permits which depend on the newly dropped/altered permit 
**	may need to be recomputed.  Recomputation of depth of one permit can 
**	cause a chain reaction in which other permits may need to be 
**	recomputed as well (this would happen if these permits were created 
**	before the permit on which they now must depend.)
**
**	Obviously, it is to our benefit to recompute depth of as few
**	permits as possible.  Accordingly, when recomputing depth of a
**	permit, we will strive to obtain a value not exceeding the
**	original depth which will enable us to claim that there is no need to
**	even try recomputing depth of permits dependent on this permit (if new
**	depth is not greater than old depth, depth of permits dependent on this
**	permit should not have to be recomputed.)  Failing that we will
**	recompute depth of the permit, record that we have done so (this will
**	enable us to remember to check whether any of permits that depend on
**	this permit need to be recomputed)
**
**	The algorithm employed here will be as follows:
**		
** 		(1) read the next row R from the temp table; 
**		    if no more rows are left to read, we are done;
**		    otherwise, let PRIV be the privilege described by R
**		(2) if (PRIV was granted to a user) 
**		    {
**		      step (3) will apply to all permits granted by 
**		      R.qeu_grantee that may depend on PRIV
**		    }
**		    else
**                  {
**		      step (3) will apply to all permits that may depend 
**		      on PRIV, regardless of who granted them
**                  }
**		(3) Let P be a permit that could depend on PRIV; 
**		    if (depth of P is <= R.qeu_old_depth)
**		    {
**		      there exists some permit on which P depends and
**		      depth of P will not be affected by change in the
**		      depth of PRIV
**		    }
**		    else if (depth of P > R.qeu_new_depth)
**                  {
**		      even if P did depend on PRIV, depths of PRIV and P still
**		      satisfy the requirement that a permit of depth N does 
**		      not depend on a permit of depth N+k (k>=0); 
**		      we do not need to recompute depth of P
**                  }
**		    else 
**                  {
**		      if (there exist permits of depth < P.dbp_depth
**			  granted either to grantor of P or to PUBLIC which 
**			  would enable creation of P)
**		      {
**			there is no need to recompute depth of this permit
**		      }
**		      else
**                    {
**			Let N be the largest depth of a permit (other
**			than P) granted to the grantor of P or to PUBLIC
**			and specifying the same privilege as P;
**			// there must be at least one such permit since
**			// we claim that qeu_depth() will NOT be called to 
**			// recompute depth of permits that are abandoned
**			if (N >= P.dbp_depth)
**			{
**			  reset P.dbp_depth to N+1
**                        if (P specified a grantable privilege)
**			  {
**			    append a row to the temp table describing privilege **			    conveyed by P along with its old and new depths
**			  }
**			}
**                    }
**                  }
**		(4) goto (1)
**
** Input:
**	qef_cb			QEF session CB
**	qeuq_cb			QEU query CB
**	obj_id			id of the object
**	pdi_tbl_descr		description of a temp table containing 
**				descriptions of privileges whose depths have 
**				changed as a result of revocation of specified 
**				privileges
**	obj_owner		name of the object owner
**
** Output:
**	error			filled in if an error occured
**
** Returns:
**	E_DB_{OK,ERROR,...}
**
** History:
**
**	22-aug-92 (andre)
**	    written
**	10-jun-93 (andre)
**	    added obj_owner to the interface; if the name of the object owner is
**	    the same as that of the user from whom someone has revoked some
**	    privilege, it will have no effect on depths of any permits defined
**	    on the object since permits granted by the object owner have depth 0
**	    and all other permits are allowed to be created because of them
**	23-jul-93 (andre)
**	    Since qeu_depth() will now be called to recompute depths of permits
**	    only if they are not abandoned, we will no longer insist that a 
**	    depth of a given permit may be recomputed only once
**	25-jul-93 (andre)
**	    qeu_match_privs() is now responsible for storing descriptions
**	    of privileges whose depth has changed as a result of revocation of
**	    specified privileges - instead of passing in a single description 
**	    of a privilege whose depth has changed, the caller will hand us a 
**	    description of a temp table containing descriptions of all 
**	    privileges whose depths have changed
*/
static DB_STATUS
qeu_depth(
QEF_CB			*qef_cb,
QEUQ_CB			*qeuq_cb,
DB_TAB_ID		*obj_id,
QEU_PDI_TBL_DESCR	*pdi_tbl_descr,
DB_OWN_NAME		*obj_owner,
i4			*error)
{
    DB_STATUS           status = E_DB_OK, local_status;
    i4                  i, j;
    QEU_CB              *pdi_qeu = &pdi_tbl_descr->qeu_pdi_qeucb;
    QEU_CB              pr1qeu, *pr1_qeu = (QEU_CB *) NULL;
    QEU_CB		pr2qeu, *pr2_qeu = (QEU_CB *) NULL;
    QEF_DATA            pr1_qefdata;
    QEF_DATA		pr2_qefdata;
    QEU_PERM_DEPTH_INFO	*read_pdi_row = &pdi_tbl_descr->qeu_pdi_read_row;
    QEU_PERM_DEPTH_INFO *write_pdi_row = &pdi_tbl_descr->qeu_pdi_write_row;
    DB_PROTECTION       prot1, *pr1_tuple = &prot1;
    DB_PROTECTION	prot2, *pr2_tuple = &prot2;
    DB_PROTECTION	prot,  *pr_tuple  = &prot;
    DMR_ATTR_ENTRY	prot_key_array[2];
    DMR_ATTR_ENTRY      *protkey_ptr_array[2];
    i4			exit_loop;

    for (exit_loop = FALSE; !exit_loop; )
    {
	/* 
	** open IIPROTECT for reading and writing; 
	** we open it twice because once we find a permit tuple that could 
	** be affected by the changes in the depth of a permit described by 
	** a temp table row, we will try to scan other permits on the object 
	** trying to determine whether and how the depth of the permit needs 
	** to be changed
	*/
	pr1qeu.qeu_type = QEUCB_CB;
        pr1qeu.qeu_length = sizeof(QEUCB_CB);
        pr1qeu.qeu_db_id = qeuq_cb->qeuq_db_id;
        pr1qeu.qeu_lk_mode = DMT_X;
        pr1qeu.qeu_flag = DMT_U_DIRECT;
        pr1qeu.qeu_access_mode = DMT_A_WRITE;
	pr1qeu.qeu_mask = 0;
	pr1qeu.qeu_qual = pr1qeu.qeu_f_qual = 0;
	pr1qeu.qeu_qarg = pr1qeu.qeu_f_qarg = 0;

	pr1qeu.qeu_tab_id.db_tab_base  = DM_B_PROTECT_TAB_ID;
	pr1qeu.qeu_tab_id.db_tab_index = DM_I_PROTECT_TAB_ID;
	pr1qeu.qeu_lk_mode             = DMT_IX;
	STRUCT_ASSIGN_MACRO(pr1qeu, pr2qeu);

	status = qeu_open(qef_cb, &pr1qeu);
	if (status != E_DB_OK)
	{
	    *error = pr1qeu.error.err_code;
	    break;
	}
	pr1_qeu = &pr1qeu;

	status = qeu_open(qef_cb, &pr2qeu);
	if (status != E_DB_OK)
	{
	    *error = pr2qeu.error.err_code;
	    break;
	}
	pr2_qeu = &pr2qeu;

	pr1_qeu->qeu_tup_length = sizeof(DB_PROTECTION);

	pr1_qeu->qeu_output = &pr1_qefdata;
	pr1_qefdata.dt_next = 0;
	pr1_qefdata.dt_size = sizeof(DB_PROTECTION);
	pr1_qefdata.dt_data = (PTR) pr1_tuple;

	pr2_qeu->qeu_tup_length = sizeof(DB_PROTECTION);

	pr2_qeu->qeu_input = pr2_qeu->qeu_output = &pr2_qefdata;
	pr2_qefdata.dt_next = 0;
	pr2_qefdata.dt_size = sizeof(DB_PROTECTION);
	pr2_qefdata.dt_data = (PTR) pr2_tuple;

	pr1_qeu->qeu_klen = 2;
	pr1_qeu->qeu_getnext = QEU_REPO;
	pr1_qeu->qeu_key = protkey_ptr_array;

	protkey_ptr_array[0] = prot_key_array;
	protkey_ptr_array[0]->attr_number = DM_1_PROTECT_KEY;
	protkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	protkey_ptr_array[0]->attr_value = (char *) &obj_id->db_tab_base;
	
	protkey_ptr_array[1] = prot_key_array + 1;
	protkey_ptr_array[1]->attr_number = DM_2_PROTECT_KEY;
	protkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	protkey_ptr_array[1]->attr_value = (char *) &obj_id->db_tab_index;

	/* use the same key as that used in pr1_qeu */
	pr2_qeu->qeu_key = pr1_qeu->qeu_key;

	/* here we go... read the temp table until we run out of rows */
	pdi_qeu->qeu_getnext = QEU_REPO;

	for (;;)
	{
	    status = qeu_get(qef_cb, pdi_qeu);
	    if (status != E_DB_OK)
	    {
		if (pdi_qeu->error.err_code == E_QE0015_NO_MORE_ROWS)
		{
		    /* no more rows - we are done */
		    status = E_DB_OK;
		}
		else
		{
		    *error = pdi_qeu->error.err_code;
		}
		break;
	    }

	    if (pdi_qeu->qeu_getnext == QEU_REPO)
		pdi_qeu->qeu_getnext = QEU_NOREPO;

	    /* 
	    ** now look for permits that may depend on the permit
	    ** described by this temp table row
	    */

	    pr2_qeu->qeu_count = 1;
	    pr2_qeu->qeu_klen = 2;
	    pr2_qeu->qeu_getnext = QEU_REPO;

	    for (;;)
	    {
		/* 
		** use qeu_pdp2_qual() to qualify permits returned by
		** dmr_get(); we are setting it here because it will be
		** reset prior to calling qeu_replace() (qeu_replace()
		** insists that if we are replacing a current row,
		** qualification function should not be passed)
		*/
		pr2_qeu->qeu_qual = ( PTR ) qeu_pdp2_qual;
		pr2_qeu->qeu_qarg = (PTR) read_pdi_row;

		status = qeu_get(qef_cb, pr2_qeu);
		if (status != E_DB_OK)
		{
		    if (pr2_qeu->error.err_code == E_QE0015_NO_MORE_ROWS)
		    {
			/* 
			** no more permits to consider for this temp
			** table row
			*/
			status = E_DB_OK;
		    }
		    else
		    {
			*error = pr2_qeu->error.err_code;
		    }
		    break;
		}

		if (pr2_qeu->qeu_getnext == QEU_REPO)
		{
		    pr2_qeu->qeu_getnext = QEU_NOREPO;
		    pr2_qeu->qeu_klen = 0;
		}

		/* 
		** check whether there exist permits of depth less than
		** pr2_tuple->dbp_depth that would give grantor of the
		** permit described by *pr2_tuple sufficient privileges
		** to grant that permit; if so, we do not need to recompute
		** its depth - otherwise, we will have no other choice
		*/

		/* 
		** make a copy of the permit tuple since qeu_pdp1_qual()
		** may alter attribute map as it is trying to determine
		** whether there exist permit(s) of depth < pr2_tuple->dbp_depth
		** which would give grantor of the permit described by
		** *pr2_tuple sufficient privileges to grant that permit
		*/
		STRUCT_ASSIGN_MACRO((*pr2_tuple), (*pr_tuple));

                /*
                ** it makes our life easier if the privilege map does not have
                ** DB_GRANT_OPTION bit set - a tuple would not be inserted
                ** into the temp table if the permit represented by it did
                ** not specify a grantable privilege
                **
                ** also, turn off DB_TEST and DB_AGGREGATE bits if the permit
                ** specify SELECT privilege
                */
		pr_tuple->dbp_popset &= ~((i4) DB_GRANT_OPTION);
		if (pr_tuple->dbp_popset & DB_RETRIEVE)
		    pr_tuple->dbp_popset = DB_RETRIEVE;

		/*
		** qeu_pdp1_qual() will be used to qualify tuples
		** returned by dmr_get(); this function is set up so that
		** if it returns a row, it will signal to us that the
		** permit's depth does not need to be recomputed; if it
		** returns E_QE0015, we will be recomputing the permit's
		** depth (unless, of course it would become abandoned)
		*/
		pr1_qeu->qeu_qual = ( PTR ) qeu_pdp1_qual;
		pr1_qeu->qeu_qarg = (PTR) pr_tuple;

		pr1_qeu->qeu_count = 1;
		status = qeu_get(qef_cb, pr1_qeu);
		if (status == E_DB_OK)
		{
		    /* depth of this permit need not be recomputed */
		    continue;
		}
		else
		{
		    if (pr1_qeu->error.err_code == E_QE0015_NO_MORE_ROWS)
		    {
			/* 
			** next we will try to recompute depth of this
			** permit
			*/
			status = E_DB_OK;
		    }
		    else
		    {
		        *error = pr1_qeu->error.err_code;
		        break;
		    }
		}

		/* 
		** now we will scan IIPROTECT again trying to recompute
		** this permit's depth 
		**
		** we'll use qeu_pdp3_qual() to qualify permit tuples;
		** this function will look at all tuples specifying
		** grantable privilege to PUBLIC or to the grantor of this
		** permit; this function is set up so that it returns a row
		** only after we encounter permits which specified all
		** of privileges required for this permit to exist
                */
                pr1_qeu->qeu_qual = ( PTR ) qeu_pdp3_qual;
		pr1_qeu->qeu_count = 1;

		/* 
		** if the permit whose depth is likely to be recomputed
		** specified a grantable privilege on a table or a view, copy 
		** the attribute map from pr_tuple to write_pdi_row
		** since qeu_pdp3_qual() is likely to alter it
		*/
		if (   pr2_tuple->dbp_popset & DB_GRANT_OPTION
		    && (   pr2_tuple->dbp_obtype == DBOB_TABLE
		    	|| pr2_tuple->dbp_obtype == DBOB_VIEW)
		   )
		{
		    for (i = 0; i < DB_COL_WORDS; i++)
			write_pdi_row->qeu_attrmap[i] = 
			    pr_tuple->dbp_domset[i];
		}

		status = qeu_get(qef_cb, pr1_qeu);
		if (status != E_DB_OK)
		{
		    if (pr1_qeu->error.err_code == E_QE0015_NO_MORE_ROWS)
		    {
			status = E_DB_OK;

			/* 
			** if this is a permit on a table or a view, we
			** have not found permits that would grant the
			** required privilege WGO to PUBLIC or to the
			** grantor of the permit on some attribute(s)
			** mentioned in the permit; 
			** if NONE of the attributes which were in 
			** pr_tuple->dbp_domset before this latest scan 
			** of IIPROTECT were removed from it during the scan, 
			** we can safely conclude that the privilege on all 
			** those attributes will become abandoned, but we 
			** will let qeu_priv_revoke() deal with cascadign 
			** revocation issues; here we will simply not insert 
			** a new row into the temp table - no sense in 
			** recomputing depth of abandoned permit
			**
			** if this is a permit on a dbproc or a dbevent,
			** we can conclude that this permit will become
			** abandoned; we will not insert a row into the
			** temp table
			*/
			if (pr_tuple->dbp_depth == pr2_tuple->dbp_depth)
			{
			    continue;
			}
			else if (   pr2_tuple->dbp_popset & DB_GRANT_OPTION
		    		 && (   pr2_tuple->dbp_obtype == DBOB_TABLE
		    		     || pr2_tuple->dbp_obtype == DBOB_VIEW)
		   		)
			{
			    /*
			    ** write_pdi_row->qeu_attrmap contains a map
			    ** of attributes for which we were forced to
			    ** recompute permit's depth and pr_tuple->dbp_domset
			    ** contains a map of attributes for which the 
			    ** privilege specified in pr_tuple->dbp_popset
			    ** will become abandoned - here we will update
			    ** write_pdi_row->qeu_attrmap to describe only
			    ** the attributes for which (we think - since
			    ** only qeu_match_privs() can tell for sure)
			    ** the privilege will not become abandoned
			    */
			    for (i = 0; i < DB_COL_WORDS; i++)
				write_pdi_row->qeu_attrmap[i] &= 
				    ~pr_tuple->dbp_domset[i];
			}
		    }
		    else
		    {
			*error = pr1_qeu->error.err_code;
			break;
		    }
		}

		/*
		** if the permit represented grantable privilege, construct 
		** the tuple that will be inserted into the temp table
		*/
		if (pr2_tuple->dbp_popset & DB_GRANT_OPTION)
		{
		    write_pdi_row->qeu_pdi_flags = 0;
		    write_pdi_row->qeu_priv = pr_tuple->dbp_popset;
    
		    write_pdi_row->qeu_old_depth = pr2_tuple->dbp_depth;
		    write_pdi_row->qeu_new_depth = pr_tuple->dbp_depth;
    
		    write_pdi_row->qeu_gtype = pr_tuple->dbp_gtype;
		    if (write_pdi_row->qeu_gtype == DBGR_USER)
		    {
		        STRUCT_ASSIGN_MACRO(pr_tuple->dbp_owner,
					    write_pdi_row->qeu_authid);
		    }
    
		    status = qeu_append(qef_cb, pdi_qeu);
		    if (status != E_DB_OK)
		    {
		        *error = pdi_qeu->error.err_code;
		        break;
		    }
		}

		/*
		** and finally, update depth of the permit
		*/
		pr2_qeu->qeu_qual = 0;
		pr2_qeu->qeu_qarg = 0;
		pr2_tuple->dbp_depth = pr_tuple->dbp_depth;
		status = qeu_replace(qef_cb, pr2_qeu);
		if (status != E_DB_OK)
		{
		    *error = pr2_qeu->error.err_code;
		    break;
		}
	    }
	    if (status != E_DB_OK)
		break;
	}

	exit_loop = TRUE;
    }
    
    /* we get here if we are done or if something went wrong */

    if (pr1_qeu)
    {
	local_status = qeu_close(qef_cb, pr1_qeu);
	if (local_status != E_DB_OK)
        {
            if (status != E_DB_OK)
            {
                /*
                ** if we got here as a result of an error, display that error
                ** before overwriting it
                */
                (VOID) qef_error(*error, 0L, local_status, error,
                    &qeuq_cb->error, 0);
            }
            *error = pr1_qeu->error.err_code;
            if (local_status > status)
                status = local_status;
        }
    }

    if (pr2_qeu)
    {
	local_status = qeu_close(qef_cb, pr2_qeu);
	if (local_status != E_DB_OK)
        {
            if (status != E_DB_OK)
            {
                /*
                ** if we got here as a result of an error, display that error
                ** before overwriting it
                */
                (VOID) qef_error(*error, 0L, local_status, error,
                    &qeuq_cb->error, 0);
            }
            *error = pr2_qeu->error.err_code;
            if (local_status > status)
                status = local_status;
        }
    }

    return(status);
}

/*
** Name: qeu_pdp2_qual() - a function to qualify DB_PROTECTION tuples
**
** Description:
**	This function qualifies DB_PROTECTION tuples returned by dmr_get() 
**	to qeu_depth() when we are searching for permits that could be affected 
**	by change in the depth of a given permit
**
** Input:
**	
**	info_tuple	temp table tuple passed by qeu_depth() which contains 
**			the values by which a newly read tuple will be qualified
**	prot_tuple	tuple fetched by dmr_get()
** Output:
**	none
**
** Returns:
**	DMR_F_NEXT if the row is to be skipped and DMR_F_RETURN otherwise
**
** History:
**
**	21-aug-92 (andre)
**	    written
*/
static i4
qeu_pdp2_qual(
QEU_PERM_DEPTH_INFO	*info_tuple,
DB_PROTECTION		*prot_tuple)
{
    /*
    ** we are not interested in tuples representing
    ** security_alarms, access permits or pre-6.5 permits
    */
    if (   prot_tuple->dbp_popset & (i4) (DB_CAN_ACCESS | DB_ALARM)
	|| ~prot_tuple->dbp_flags & DBP_65_PLUS_PERM)
    {
	return(DMR_F_NEXT);
    }

    /*
    ** we are also very uninterested in permits granted by the object owner 
    ** (their depth gets set to 0 at creation time and is never reset)
    */
    if (!MEcmp((PTR) &prot_tuple->dbp_obown, (PTR) &prot_tuple->dbp_grantor,
	    sizeof(DB_OWN_NAME)))
    {
	return(DMR_F_NEXT);
    }

    /* 
    ** if the permit did not specify privilege which was specified by a
    ** permit whose depth was recomputed, it can hardly be affected by the
    ** recomputation
    */
    if (!(prot_tuple->dbp_popset & info_tuple->qeu_priv))
    {
	return(DMR_F_NEXT);
    }

    /*
    ** if permit whose depth was recomputed was granted to a user different 
    ** from the grantor of the permit being considered, the permit will not be 
    ** affected
    */
    if (   info_tuple->qeu_gtype == DBGR_USER
	&& MEcmp((PTR) &info_tuple->qeu_authid, (PTR) &prot_tuple->dbp_grantor,
		sizeof(DB_OWN_NAME)))
    {
	return(DMR_F_NEXT);
    }

    if (prot_tuple->dbp_depth <= info_tuple->qeu_old_depth)
    {
        /* 
        ** this permit never depended on permit whose depth
        ** was changed - it's depth cannot possibly be affected
        */
	return(DMR_F_NEXT);
    }

    if (   info_tuple->qeu_new_depth > 0
	&& info_tuple->qeu_new_depth < prot_tuple->dbp_depth)
    {
	/*
	** unless the temp table row describes a permit that was dropped,
	** even if this permit did depend on the permit whose depth was
	** recomputed, their depths would still satisfy the requirement that a 
	** permit of depth N does not depend on a permit of depth N+k (k>=0);
	** therefore, depth of this permit need not be recomputed
	*/
	return(DMR_F_NEXT);
    }

    /* 
    ** if the permit specifies a privilege on a table or a view, apply
    ** one last check: if the intersection of attribute maps of the permit
    ** whose depth was recomputed and the permit currently under
    ** consideration is empty, the permit will not be affected
    */
    if (   prot_tuple->dbp_obtype == DBOB_TABLE
	|| prot_tuple->dbp_obtype == DBOB_VIEW)
    {
	register i4	i;

	for (i = 0; i < DB_COL_WORDS; i++)
	{
	    if (prot_tuple->dbp_domset[i] & info_tuple->qeu_attrmap[i])
		break;
	}

	if (i == DB_COL_WORDS)
	{
	    /* intersection of attribute maps is empty */
	    return(DMR_F_NEXT);
	}
    }

    /* 
    ** the tuple passed all checks - it may be affected by the change in
    ** the depth of the permit described in the temp table tuple
    */
    return(DMR_F_RETURN);
}

/*
** Name: qeu_pdp1_qual() - a function to qualify DB_PROTECTION tuples
**
** Description:
**	This function qualifies DB_PROTECTION tuples returned by dmr_get() 
**	to qeu_depth() when we are searching for permits that would
**	enable us to conclude that the depth of a permit passed by qeu_depth()
**	does not need to be recomputed;
**	this function does more than just qualify rows returned by dmr_get():
**
**	    it will return DMR_F_RETURN only if we have found permit(s)
**	    that would enable us to conclude that the depth of the permit 
**	    currently under consideration by qeu_depth() does not need to be 
**	    recomputed; otherwise it will return DMR_F_NEXT;
**	    if a given permit returned by dmr_get() is of acceptable
**	    depth but specifies a grantable privilege only on a subset of 
**	    columns specified in the permit passed by qeu_depth(), that 
**	    permit's attribute map will be updated to reflect that the 
**	    privilege now is required on a smaller set of attributes
**
** Input:
**	
**	cur_prot_tuple	IIPROTECT tuple passed by qeu_depth(); this is
**                      expected to be a copy of the real tuple since this 
**		 	function may alter its attribute map
**	prot_tuple	tuple fetched by dmr_get()
** Output:
**	none
**
** Returns:
**	DMR_F_NEXT if more permits need to be examined; DMR_F_RETURN otherwise
**
** History:
**
**	24-aug-92 (andre)
**	    written
*/
static i4
qeu_pdp1_qual(
DB_PROTECTION		*cur_prot_tuple,
DB_PROTECTION		*prot_tuple)
{
    /*
    ** we are not interested in tuples representing
    ** security_alarms, access permits or pre-6.5 permits
    */
    if (   prot_tuple->dbp_popset & (i4) (DB_CAN_ACCESS | DB_ALARM)
	|| ~prot_tuple->dbp_flags & DBP_65_PLUS_PERM)
    {
	return(DMR_F_NEXT);
    }

    /* 
    ** current tuple returned by dmr_get() is of no interest to us if
    **   - its depth is not less than that of the tuple passed by qeu_depth(),
    **     or
    **   - it does not specify a grantable privilege, or
    **   - it does not specify the same privilege as the tuple passed by
    **     qeu_depth()
    */
    if (   prot_tuple->dbp_depth >= cur_prot_tuple->dbp_depth
	|| ~prot_tuple->dbp_popset & DB_GRANT_OPTION
	|| !(prot_tuple->dbp_popset & cur_prot_tuple->dbp_popset)
       )
    {
	return(DMR_F_NEXT);
    }

    /*
    ** current tuple is not of interest to us if
    **   - it was not granted to PUBLIC and
    **   - it was not granted to the grantor of the permit passed by qeu_depth()
    */
    if (   prot_tuple->dbp_gtype != DBGR_PUBLIC
	&& (   prot_tuple->dbp_gtype != DBGR_USER 
	    || MEcmp((PTR) &cur_prot_tuple->dbp_grantor, 
		     (PTR) &prot_tuple->dbp_owner, sizeof(DB_OWN_NAME))
	   )
       )
    {
	return(DMR_F_NEXT);
    }

    /* 
    ** if the permit specifies a privilege on a table or a view, apply
    ** one last check: if the set difference of the attribute map in the
    ** tuple passed by qeu_depth() and that in the tuple passed by
    ** dmr_get() is non-empty, we have to continue looking for more rows
    */
    if (   prot_tuple->dbp_obtype == DBOB_TABLE
	|| prot_tuple->dbp_obtype == DBOB_VIEW)
    {
	register i4	i;
	i4		done = TRUE;	/* so call me an optimist... */

	for (i = 0; i < DB_COL_WORDS; i++)
	{
	    if (cur_prot_tuple->dbp_domset[i] &= ~prot_tuple->dbp_domset[i])
		done = FALSE;
	}

	if (!done)
	{
	    return(DMR_F_NEXT);
	}
    }

    /* 
    ** we have determined that the depth of the permit specified by the
    ** caller will not need to be recomputed; return this row to
    ** communicate that information
    */
    return(DMR_F_RETURN);
}

/*
** Name: qeu_pdp3_qual() - a function to qualify DB_PROTECTION tuples
**
** Description:
**	This function qualifies DB_PROTECTION tuples returned by dmr_get() 
**	to qeu_depth() when we are searching for permits that would
**	enable us to recompute the depth of a permit passed by qeu_depth();
**	this function does more than just qualify rows returned by dmr_get():
**
**	    it will return DMR_F_RETURN only if we have found permit(s)
**	    that would enable us to claim that we found depth of the
**	    permit currently under consideration by qeu_depth(); otherwise 
**	    it will return DMR_F_NEXT
**	    if a given permit returned by dmr_get() specifies a grantable 
**	    privilege only on a subset of columns specified in the permit 
**	    passed by qeu_depth(), that permit's attribute map will be 
**	    updated to reflect that the privilege now is required on a 
**	    smaller set of attributes;
**
**	    depth of the permit passed by qeu_depth() will be updated
**	    based on prot_tuple->dbp_depth if the permit passed by
**	    dmr_get() is in some way applicable to our search
**
** Input:
**	
**	cur_prot_tuple	IIPROTECT tuple passed by qeu_depth(); this is
**                      expected to be a copy of the real tuple since this 
**		 	function may alter its attribute map
**	prot_tuple	tuple fetched by dmr_get()
**
** Output:
**	none
**
** Returns:
**	DMR_F_NEXT if more permits need to be examined; DMR_F_RETURN otherwise
**
** History:
**
**	24-aug-92 (andre)
**	    written
*/
static i4
qeu_pdp3_qual(
DB_PROTECTION		*cur_prot_tuple,
DB_PROTECTION		*prot_tuple)
{
    /*
    ** we are not interested in tuples representing
    ** security_alarms, access permits or pre-6.5 permits
    */
    if (   prot_tuple->dbp_popset & (i4) (DB_CAN_ACCESS | DB_ALARM)
	|| ~prot_tuple->dbp_flags & DBP_65_PLUS_PERM)
    {
	return(DMR_F_NEXT);
    }

    /* 
    ** current tuple returned by dmr_get() is of no interest to us if
    **   - it does not specify a grantable privilege, or
    **   - it does not specify the same privilege as the tuple passed by
    **     qeu_depth()
    */
    if (   ~prot_tuple->dbp_popset & DB_GRANT_OPTION
	|| !(prot_tuple->dbp_popset & cur_prot_tuple->dbp_popset)
       )
    {
	return(DMR_F_NEXT);
    }

    /*
    ** current tuple is not of interest to us if
    **   - it was not granted to PUBLIC and
    **   - it was not granted to the grantor of the permit passed by qeu_depth()
    */
    if (   prot_tuple->dbp_gtype != DBGR_PUBLIC
	&& (   prot_tuple->dbp_gtype != DBGR_USER 
	    || MEcmp((PTR) &cur_prot_tuple->dbp_grantor, 
		     (PTR) &prot_tuple->dbp_owner, sizeof(DB_OWN_NAME))
	   )
       )
    {
	return(DMR_F_NEXT);
    }

    /* 
    ** if the permit specifies a privilege on a table or a view, apply
    ** one last check: if the set difference of the attribute map in the
    ** tuple passed by qeu_depth() and that in the tuple passed by
    ** dmr_get() is non-empty, we have to continue looking for more rows
    **
    ** However, if the intersection of two attribute maps in non-empty,
    ** we may need to update cur_prot_tuple->dbp_depth if it is less than
    ** prot_tuple->dbp_depth+1
    */
    if (   prot_tuple->dbp_obtype == DBOB_TABLE
	|| prot_tuple->dbp_obtype == DBOB_VIEW)
    {
	register i4	i;
	i4		done = TRUE;
	i4             empty_intersection = TRUE;

	for (i = 0; i < DB_COL_WORDS; i++)
	{
	    if (cur_prot_tuple->dbp_domset[i] & prot_tuple->dbp_domset[i])
	        empty_intersection = FALSE;

	    if (cur_prot_tuple->dbp_domset[i] &= ~prot_tuple->dbp_domset[i])
		done = FALSE;
	}

	if (!done)
	{
	    if (   !empty_intersection 
	        && cur_prot_tuple->dbp_depth <= prot_tuple->dbp_depth)
	    {
	        cur_prot_tuple->dbp_depth = prot_tuple->dbp_depth + 1;
	    }

	    return(DMR_F_NEXT);
	}
    }

    if (cur_prot_tuple->dbp_depth <= prot_tuple->dbp_depth)
    {
	cur_prot_tuple->dbp_depth = prot_tuple->dbp_depth + 1;
    }

    return(DMR_F_RETURN);
}

/*
** Name: qeu_0skip_words - skip past a specified number of words in IIQRYTEXT
**			   tuples
**
** Description:
**	This function will skip past a specified number of regular identifiers
**	(i.e. words that start with a CMnmstart() character, consist of 0 or
**	more CMnmchar() characters, and are separated by !CMnmchar()
**	characters.)
**	Caller may request that skipped words be added to text string being
**	accumulated, in which case we will call qeu_assemble_text() to add them
**	to the string.  If it becomes necessary to cross a tuple boundary, next
**	tuple will be read.
**
** Input:
**	num_words	    number of words to skip
**	cur_char	    addr of a ptr to the next char to consider
**	last_char	    addr to the ptr to the last char in the tuple
**	qef_cb		    QEF session CB
**	qtxt_qeu	    QEU_CB which was used to open IIQRYTEXT for
**			    writing and shall be used to read, delete and append
**			    tuples from/to IIQRYTEXT
**	in_txt		    DB_TEXT_STRING describing text being processed
**	    db_t_text	    first char
**	    db_t_count	    number of chars
**	ulm		    ULM request CB describing a previously opened stream
**	qtxt_seq	    address of the sequence field which is used
**			    to contain the fourth key (sequence) used
**			    when accessing IIQRYTEXT
**	out_buf		    addr of a ptr to string being assembled
**	buf_capacity	    address of a number representing a maximum number of
**			    chars that can be accomodated by **out_buf
**	add_to_text	    if TRUE, portion of tuple(s) processed in search for
**			    specified number of words will be added to the text
**			    string pointed to by *out_buf
** Output:
**	*cur_char	    will point to the next char after the specified
**			    number of words
**	*last_char	    will point at the last char in the current tuple
**	*qtxt_seq	    will be incremented if we had to skip past a tuple
**			    boundary
**	*out_buf	    if caller requested that we write scanned chars to
**			    the buffer and it could not accommodate them all, a
**			    new buffer will be allocated and *out_buf will point
**			    at it
**	*buf_capacity	    if caller requested that we write scanned chars to
**			    the buffer and it could not accommodate them all,
**			    its size will be increased, and the new capacity
**			    will be stored into *buf_capacity
**	error		    filled in if an error occurred
**
** Returns:
**	E_DB_{OK,ERROR,...}
**
** Side effects:
**	if requested by the caller, chars scanned by the function will be
**	written out, which may result in memory being allocated
**
** History
**	01-feb-93 (andre)
**	    written
**	02-feb-93 (andre)
**	    got rid of write_qtxt_tuple, added out_buf and buf_capacity, renamed
**	    write_tuples to add_to_text
*/
static DB_STATUS
qeu_0skip_words(
i4		num_words,
char		**cur_char,
char		**last_char,
QEF_CB          *qef_cb,
QEU_CB          *qtxt_qeu,
DB_TEXT_STRING	*in_txt,
ULM_RCB         *ulm,
i4		*qtxt_seq,
DB_TEXT_STRING	**out_buf,
i4		*buf_capacity,
bool		add_to_text,
i4		*error)
{
    char	*c = *cur_char, *char_lim = *last_char, *char_start = c;
    DB_STATUS	status = E_DB_OK;
    bool	word_started = FALSE;
	
    while (num_words > 0)
    {
	if (c == char_lim)
	{
	    /*
	    ** finished scaning current tuple - if caller requested that we
	    ** write out characters that were scanned, do so now, then read the
	    ** next tuple
	    */
	    if (add_to_text && char_start < c)
	    {
		status = qeu_assemble_text(out_buf, buf_capacity,
		    char_start, c - char_start, ulm, error);
		if (status != E_DB_OK)
		{
		    return(status);
		}
	    }
	    
	    (*qtxt_seq)++;
	    status = qeu_get(qef_cb, qtxt_qeu);
	    if (status != E_DB_OK)
	    {
		*error = qtxt_qeu->error.err_code;
		return(status);
	    }

	    char_start = c = (char *) in_txt->db_t_text;
	    *last_char = char_lim = c + in_txt->db_t_count;
	}

	/*
	** if a word got split between two tuples, we may be looking at a char
	** that is not CMnmstart(), but is CMnmchar()
	*/
        if (CMnmstart(c) || (word_started && CMnmchar(c)))
        {
	    word_started = TRUE;
	    for (CMnext(c); (c < char_lim && CMnmchar(c)); CMnext(c))
	    ;
        }
	else
        {
	    /*
	    ** if this is the last word we were asked to "skip", stop at the
	    ** first char folloing that word; otherwise, skip until you
	    ** encounter a beginning of the next word
	    */

	    if (word_started)
	    {
		word_started = FALSE;
		/*
		** decrement num_words after reaching end (rather than
		** beginning of a word) in case a word gets split between two
		** tuples
		*/
		if (!--num_words)
		    continue;
	    }

	    for (; (c < char_lim && !CMnmstart(c)); CMnext(c))
	    ;
        }
    }

    /*
    ** if caller requested that we write out characters that were scanned, and
    ** at least one was scanned in the current tuple, do so now
    */
    if (add_to_text && char_start < c)
    {
	status = qeu_assemble_text(out_buf, buf_capacity,
	    char_start, c - char_start, ulm, error);
	if (status != E_DB_OK)
	{
	    return(status);
	}
    }
    
    /*
    ** reset *cur_char to point at the next char;
    ** NOTE: if the last skipped word was the last word in the current tuple,
    ** *cur_char will end up pointing immediately after the last char in the
    ** current tuple
    */
    *cur_char = c;

    return(E_DB_OK);
}

/*
** Name: qeu_1skip_white_chars - skip past 0 or more white chars
**
** Description:
**	This function will skip past 0 or more white chars starting with the
**	current one. 
**	Caller may request that skipped words be added to text string being
**	accumulated, in which case we will call qeu_assemble_text() to add them
**	to the string.  If it becomes necessary to cross a tuple boundary, next
**	tuple will be read.
**
** Input:
**	cur_char	    addr of a ptr to the next char to consider
**	last_char	    addr to the ptr to the last char in the tuple
**	qef_cb		    QEF session CB
**	qtxt_qeu	    QEU_CB which was used to open IIQRYTEXT for
**			    writing and shall be used to read, delete and append
**			    tuples from/to IIQRYTEXT
**	in_txt		    DB_TEXT_STRING describing text being processed
**	    db_t_text	    first char
**	    db_t_count	    number of chars
**	ulm		    ULM request CB describing a previously opened stream
**	qtxt_seq	    address of the sequence field which is used
**			    to contain the fourth key (sequence) used
**			    when accessing IIQRYTEXT
**	out_buf		    addr of a ptr to string being assembled
**	buf_capacity	    address of a number representing a maximum number of
**			    chars that can be accomodated by **out_buf
**	add_to_text	    if TRUE, portion of tuple(s) processed in search for
**			    the first non-white char will be added to the text
**			    string pointed to by *out_buf
** Output:
**	*cur_char	    will point to the next non-white char
**	*last_char	    will point at the last char in the current tuple
**	*qtxt_seq	    will be incremented if we had to skip past a tuple
**			    boundary
**	*out_buf	    if caller requested that we write scanned chars to
**			    the buffer and it could not accommodate them all, a
**			    new buffer will be allocated and *out_buf will point
**			    at it
**	*buf_capacity	    if caller requested that we write scanned chars to
**			    the buffer and it could not accommodate them all,
**			    its size will be increased, and the new capacity
**			    will be stored into *buf_capacity
**	error		    filled in if an error occurred
**
** Returns:
**	E_DB_{OK,ERROR,...}
**
** Side effects:
**	if requested by the caller, chars scanned by the function will be
**	written out, which may result in memory being allocated
**
** History
**	01-feb-93 (andre)
**	    written
**	02-feb-93 (andre)
**	    got rid of write_qtxt_tuple, added out_buf and buf_capacity, renamed
**	    write_tuples to add_to_text
*/
static DB_STATUS
qeu_1skip_white_chars(
char		**cur_char,
char		**last_char,
QEF_CB          *qef_cb,
QEU_CB          *qtxt_qeu,
DB_TEXT_STRING	*in_txt,
ULM_RCB         *ulm,
i4		*qtxt_seq,
DB_TEXT_STRING	**out_buf,
i4		*buf_capacity,
bool		add_to_text,
i4		*error)
{
    char	*c = *cur_char, *char_lim = *last_char, *char_start = c;
    DB_STATUS	status = E_DB_OK;

    for (;;)
    {
	for (; (c < char_lim && CMwhite(c)); CMnext(c))
	;

	if (c == char_lim)
	{
	    /*
	    ** finished scaning current tuple - if requested by the caller,
	    ** write it out, then read the next one
	    */
	    if (add_to_text && char_start < c)
	    {
		status = qeu_assemble_text(out_buf, buf_capacity,
		    char_start, c - char_start, ulm, error);
		if (status != E_DB_OK)
		{
		    return(status);
		}
	    }
	    
	    (*qtxt_seq)++;
	    status = qeu_get(qef_cb, qtxt_qeu);
	    if (status != E_DB_OK)
	    {
		*error = qtxt_qeu->error.err_code;
		return(status);
	    }

	    char_start = c = (char *) in_txt->db_t_text;
	    *last_char = char_lim = c + in_txt->db_t_count;
	}
	else
	{
	    break;
	}
    }

    /*
    ** if caller requested that we write out characters that were scanned, and
    ** at least one was scanned in the current tuple, do so now
    */
    if (add_to_text && char_start < c)
    {
	status = qeu_assemble_text(out_buf, buf_capacity,
	    char_start, c - char_start, ulm, error);
	if (status != E_DB_OK)
	{
	    return(status);
	}
    }

    *cur_char = c;

    return(E_DB_OK);
}

/*
** Name: qeu_2skip_delimited_string - skip past a string delimited with
**				      specified character
**
** Description:
**	This function will skip past a string delimited with specified
**	character.  Current char is expected to be pointing at the opening
**	delimiter.
**	Caller may request that skipped words be added to text string being
**	accumulated, in which case we will call qeu_assemble_text() to add them
**	to the string.  If it becomes necessary to cross a tuple boundary, next
**	tuple will be read.
**
** Input:
**	cur_char	    addr of a ptr to the next char to consider
**	last_char	    addr to the ptr to the last char in the tuple
**	delim		    string delimiter
**	qef_cb		    QEF session CB
**	qtxt_qeu	    QEU_CB which was used to open IIQRYTEXT for
**			    writing and shall be used to read, delete and append
**			    tuples from/to IIQRYTEXT
**	in_txt		    DB_TEXT_STRING describing text being processed
**	    db_t_text	    first char
**	    db_t_count	    number of chars
**	ulm		    ULM request CB describing a previously opened stream
**	qtxt_seq	    address of the sequence field which is used
**			    to contain the fourth key (sequence) used
**			    when accessing IIQRYTEXT
**	out_buf		    addr of a ptr to string being assembled
**	buf_capacity	    address of a number representing a maximum number of
**			    chars that can be accomodated by **out_buf
**	add_to_text	    if TRUE, portion of tuple(s) processed in search for
**			    the delimited identifier will be added to the text
**			    string pointed to by *out_buf
** Output:
**	*cur_char	    will point to the next non-white char
**	*last_char	    will point at the last char in the current tuple
**	*qtxt_seq	    will be incremented if we had to skip past a tuple
**			    boundary
**	*out_buf	    if caller requested that we write scanned chars to
**			    the buffer and it could not accommodate them all, a
**			    new buffer will be allocated and *out_buf will point
**			    at it
**	*buf_capacity	    if caller requested that we write scanned chars to
**			    the buffer and it could not accommodate them all,
**			    its size will be increased, and the new capacity
**			    will be stored into *buf_capacity
**	error		    filled in if an error occurred
**
** Returns:
**	E_DB_{OK,ERROR,...}
**
** Side effects:
**	if requested by the caller, chars scanned by the function will be
**	written out, which may result in memory being allocated
**
** History
**	01-feb-93 (andre)
**	    written
**	02-feb-93 (andre)
**	    got rid of write_qtxt_tuple, added out_buf and buf_capacity, renamed
**	    write_tuples to add_to_text
*/
static DB_STATUS
qeu_2skip_delimited_string(
char		**cur_char,
char		**last_char,
char		*delim,
QEF_CB          *qef_cb,
QEU_CB          *qtxt_qeu,
DB_TEXT_STRING	*in_txt,
ULM_RCB         *ulm,
i4		*qtxt_seq,
DB_TEXT_STRING	**out_buf,
i4		*buf_capacity,
bool		add_to_text,
i4		*error)
{
    char	*c = *cur_char, *char_lim = *last_char, *char_start = c;
    DB_STATUS	status = E_DB_OK;
    bool	done = FALSE;
    bool	saw_a_delim = FALSE;

    /* skip past the leading delimiter */
    CMnext(c);
    
    while (!done)
    {
	if (c == char_lim)
	{
	    /*
	    ** finished scaning current tuple - if requested by the caller,
	    ** write it out, then read the next one
	    */
	    if (add_to_text && char_start < c)
	    {
		status = qeu_assemble_text(out_buf, buf_capacity,
		    char_start, c - char_start, ulm, error);
		if (status != E_DB_OK)
		{
		    return(status);
		}
	    }
	    
	    (*qtxt_seq)++;
	    status = qeu_get(qef_cb, qtxt_qeu);
	    if (status != E_DB_OK)
	    {
		*error = qtxt_qeu->error.err_code;
		return(status);
	    }

	    char_start = c = (char *) in_txt->db_t_text;
	    *last_char = char_lim = c + in_txt->db_t_count;
	}

	if (saw_a_delim)
	{
	    /*
	    ** the last char in the previous tuple was a delimiter; if the
	    ** first char in this tuple is also a delimiter, skip past it
	    ** since the first one escaped the second one; otherwise, the
	    ** delimiter in the previous tuple was the trailing delimiter
	    */
	    if (CMcmpcase(c, delim))
	    {
		/* saw the trailing delimiter - we are done */
		done = TRUE;
	    }
	    else
	    {
		/*
		** the first delimiter was used to escape this delimiter - skip
		** past it
		*/ 
		CMnext(c);
	    }

	    saw_a_delim = FALSE;
	    continue;
	}
	
	/*
	** look for a delimiter not immediately followed by another delimiter;
	** if a delimiter is encountered as the last char in a tuple,
	** set saw_a_delim to remind us to check wehther the first char in the
	** next tuple is a delimiter
	*/
	for (; (c < char_lim && CMcmpcase(c, delim)); CMnext(c))
	;

	if (c < char_lim)
	{
	    /*
	    ** OK, we are looking at a delimiter; if the next char is not a
	    ** delimiter, we are done, otherwise the first delimiter was
	    ** used to escape the second one
	    */

	    CMnext(c);

	    if (c == char_lim)
	    {
		saw_a_delim = TRUE;
	    }
	    else if (CMcmpcase(c, delim))
	    {
		/* saw the trailing delimiter - we are done */
		done = TRUE;
	    }
	    else
	    {
		/*
		** the first delimiter was used to escape this delimiter -
		** skip past it
		*/
		CMnext(c);
	    }
	}
    }

    /*
    ** if caller requested that we write out characters that were scanned, and
    ** at least one was scanned in the current tuple, do so now
    */
    if (add_to_text && char_start < c)
    {
	status = qeu_assemble_text(out_buf, buf_capacity,
	    char_start, c - char_start, ulm, error);
	if (status != E_DB_OK)
	{
	    return(status);
	}
    }

    *cur_char = c;

    return(E_DB_OK);
}

/*
** Name: qeu_q_consdep - filter out IIDBDEPENDS tuples when we are looking for 
**			 objects dependent on any constraint defined on a 
**			 specific table
**
** Description:
**	As a part of destroying a base table, we will be looking for objects 
**	that depend on any of the constraints defined on the table.  Some of 
**	these objects, namely rules, inidces and constraints defined on the 
**	same table are of no interest to us since they will be destroyed as a a 
**	part of destroying all rules, indices, and integrities defined on the 
**	table.  The objects that are of interest to us are database procedures 
**	which were created to enforce constraints, rules on other tables which
**	were defined to enforce [REFERENCES] constraint on the table being
**	dropped, and constraints on other table which depend on existence of
**	some constraints defined on this table (i.e. REFERENCES constraint on
**	another table can depend on UNIQUE constraint being defined on this
**	table.) 
**
**	This function will be passed to DMF to filter out the "uninteresting" 
**	tuples
**
** Input:
**	tbl_id		id of the table being dropped
**	dep_tuple	IIDBDEPENDS tuple read by DMF
** 
** Output:
**	none
**
** Returns:
**	DMR_F_RETURN if this IIDBDEPENDS tuple describes dependence of a dbproc 
**	or of a constraint defined on a different table; DMR_F_NEXT otherwise
**
** History:
**	17-mar-93 (andre)
**	    written
**	03-sep-93 (andre)
**	    in addition to constraints on other tables which depend on some
**	    constraint on the table being dropped, we are also interested
**	    in rules on other tables which [the rules] were defined to enforce
**	    some [REFERENCES] constraint on the table being dropped
*/
static i4
qeu_q_consdep(
	DB_TAB_ID	*tbl_id,
	DB_IIDBDEPENDS	*dep_tuple)
{
    if (   dep_tuple->dbv_dtype == DB_DBP
	|| (   (   dep_tuple->dbv_dtype == DB_CONS
		|| dep_tuple->dbv_dtype == DB_RULE
	       )
	    && (   tbl_id->db_tab_base  != dep_tuple->dbv_dep.db_tab_base
		|| tbl_id->db_tab_index != dep_tuple->dbv_dep.db_tab_index
	       )
	   )
       )
    {
	return(DMR_F_RETURN);
    }
    else
    {
	return(DMR_F_NEXT);
    }
}

/*
** Name: qeu_q_consdep - filter out IIINTEGRITY tuples corresponding to a
**			 specific integrity
**
** Description:
**	If asked to destroy a specific integrity, we are only interested in
**	IIINTEGRITY tuple(s) (there will be two tuples only for CHECK
**	constraints that rnder some attribute(s) "known not nullable")
**	corresponding to the integrity being dropped.
**	
**	This function will be passed to DMF to filter out the "uninteresting" 
**	tuples
**
** Input:
**	integ_no	number of integrity being dropped
**	integ_tuple	IIINTEGRITY tuple read by DMF
** 
** Output:
**	none
**
** Returns:
**	DMR_F_RETURN if this IIINTEGRITY tuple corresponds to the specific
**	integrity being dropped; DMR_F_NEXT otherwise
**
** History:
**	22-mar-93 (andre)
**	    written
*/
static i4
qeu_q_integ(
	i4		*integ_no,
	DB_INTEGRITY	*integ_tuple)
{
    return((integ_tuple->dbi_number == *integ_no) ? DMR_F_RETURN : DMR_F_NEXT);
}

/*
** Name: qeu_nxt_prv_descr_no - determine next available privilege descriptor
**				number
**
** Description:
**	This function will determine the next available privilege descriptor
**	number for a given table.  It will find a privilege descriptor with the
**	highest number and return a number one greater
**
** Input:
**	qef_cb		    QEF sesion CB
**	qeuq_cb		    QEU query CB
**	    qeuq_db_id	    database id
**	    qeuq_rtbl	    id of the table
**	
** Output:
**	descr_no	next available descriptor number
**	error		filled with error number if an error occurs
**
** Side effects:
**	Opens and reads IIPRIV
**
** Returns:
**	E_DB_{OK,ERROR}
**
** History:
**	27-mar-93 (andre)
**	    extracted from qeu_cprot() 
*/
static DB_STATUS
qeu_nxt_prv_descr_no(
    QEF_CB	*qef_cb,
    QEUQ_CB	*qeuq_cb,
    i2		*descr_no,
    i4	*error)
{
    DB_STATUS		status, loc_status;
    QEU_CB		privqeu, *priv_qeu = (QEU_CB *) NULL;
    QEF_DATA		prqef_data;
    DB_IIPRIV		priv_tuple;
    DMR_ATTR_ENTRY	prkey_array[3];
    DMR_ATTR_ENTRY	*prkey_ptr_array[3];
    i4			i;

    privqeu.qeu_type	    = QEUCB_CB;
    privqeu.qeu_length	    = sizeof(QEUCB_CB);
    privqeu.qeu_db_id	    = qeuq_cb->qeuq_db_id;
    privqeu.qeu_lk_mode	    = DMT_IS;
    privqeu.qeu_flag	    = 0;
    privqeu.qeu_access_mode = DMT_A_READ;
    privqeu.qeu_mask	    = 0;

    privqeu.qeu_tab_id.db_tab_base  = DM_B_PRIV_TAB_ID;
    privqeu.qeu_tab_id.db_tab_index = DM_I_PRIV_TAB_ID;

    status = qeu_open(qef_cb, &privqeu);
    if (status != E_DB_OK)
    {
	*error = privqeu.error.err_code;
	return(status);
    }
	    
    priv_qeu = &privqeu;

    priv_qeu->qeu_count = 1;
    priv_qeu->qeu_tup_length = sizeof(DB_IIPRIV);
    priv_qeu->qeu_output = &prqef_data;
    prqef_data.dt_next = 0;
    prqef_data.dt_size = sizeof(DB_IIPRIV);
    prqef_data.dt_data = (PTR) &priv_tuple;
    
    priv_qeu->qeu_getnext = QEU_REPO;
    priv_qeu->qeu_klen = 3;       

    for (i = 0; i < 3; i++)
	prkey_ptr_array[i] = &prkey_array[i];

    priv_qeu->qeu_key = prkey_ptr_array;
    prkey_ptr_array[0]->attr_number = DM_1_PRIV_KEY;
    prkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
    prkey_ptr_array[0]->attr_value = 
	(char *) &qeuq_cb->qeuq_rtbl->db_tab_base;

    prkey_ptr_array[1]->attr_number = DM_2_PRIV_KEY;
    prkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
    prkey_ptr_array[1]->attr_value =
	(char *) &qeuq_cb->qeuq_rtbl->db_tab_index;

    prkey_ptr_array[2]->attr_number = DM_3_PRIV_KEY;
    prkey_ptr_array[2]->attr_operator = DMR_OP_GTE;
    prkey_ptr_array[2]->attr_value = (char *) descr_no;
    
    priv_qeu->qeu_qual = priv_qeu->qeu_f_qual = 0;
    priv_qeu->qeu_qarg = priv_qeu->qeu_f_qarg = 0;

    priv_qeu->error.err_code = 0L;

    /* 
    ** privilege descriptor number for a permit will be 
    ** numbered 1 or higher (0 is used for a descriptor of 
    ** privileges on which a view depends)
    */
    *descr_no = 1;

    for (;;)
    {
	status = qeu_get(qef_cb, priv_qeu);
	if (status != E_DB_OK)
	{
	    if (priv_qeu->error.err_code == E_QE0015_NO_MORE_ROWS)
	    {
		status = E_DB_OK;
	    }
	    else
	    {
		*error = priv_qeu->error.err_code;
	    }

	    break;
	}

	*descr_no = priv_tuple.db_descr_no + 1;
    }

    /* 
    ** close IIPRIV
    */
    loc_status = qeu_close(qef_cb, priv_qeu);
    if (loc_status != E_DB_OK)
    {
	*error = priv_qeu->error.err_code;
	if (loc_status > status)
	    status = loc_status;
    }

    return(status);
}

/*
** Name: qeu_iipriv_by_type - filter out IIPRIV tuples which do not describe
**			      privileges on which objects of specified type
**			      depend
**
** Description:
**	When asked to destroy (by calling qeu_dbdelete()) record of
**	objects/privileges on which all permits or all constraints depended, we
**	may not destroy ALL IIPRIV tuples since then we are likely to destroy
**	IIPRIV tuples describing privileges on which constraints depend in the
**	course of destroying all permits on a base table and, in the future
**	(if/when we allow constraints to be created on views) the reverse could
**	happen as well.  To avoid such eventuality, we need to pass to DMF a
**	function which will ensure that only tuples describing privileges on
**	which objects of specified type depend are deleted.
**
** Input:
**	dep_type	type of object (DB_CONS or DB_PROT) s.t. all IIPRIV
**			tuples describing privileges on which objects of that
**			type defined on a specific table/view depend must be
**			deleted
**	iipriv_tuple	IIPRIV tuple read by DMF
** 
** Output:
**	none
**
** Returns:
**	DMR_F_RETURN if this IIPRIV tuple corresponds to the specific object
**	type; DMR_F_NEXT otherwise
**
** History:
**	27-mar-93 (andre)
**	    written
*/
static i4
qeu_iipriv_by_type(
	i2		*dep_type,
	DB_IIPRIV	*iipriv_tuple)
{
    return((iipriv_tuple->db_dep_obj_type == *dep_type) ? DMR_F_RETURN
							: DMR_F_NEXT);
}

/*
** Name: qeu_crt_pdi_tbl - create a temp table which will be used to keep track
**			   of privileges whose depth has changed as a result of
**			   revocation of specified privileges
** 
** Description:
**	This function will initialize the structure which will describe the 
**	temp table used to keep track of privileges whose depth has changed 
**	as a result of revocation of specified privileges and open the temp 
**	table.
**
** Input:
**	qef_cb			QEF session CB
**	    qef_dmt_id		DMF transaction id
**	pdi_tbl_descr		address of the structure which will describe 
**				the temp table
**      db_id			DMF database id
**	obj_id			id of the object changes in depth of privileges
**				on which will be tracked using the temp table
**
**
** Output:
**	*error			filled in if an error occurs
**
** Side effects:
**	Allocates memory, opens a temp table
**
** Returns:
**	E_DB_{OK,ERROR,SEVERE}
**
** History:
**	24-jul-93 (andre)
**	    written
*/
static DB_STATUS
qeu_crt_pdi_tbl(
	QEF_CB			*qef_cb,
	QEU_PDI_TBL_DESCR	*pdi_tbl_descr,
	PTR			db_id,
	DB_TAB_ID		*obj_id,
	i4 		*error)
{
    DB_STATUS		status;
    DMT_ATTR_ENTRY	*attr_p, **attr_pp;
    DMT_CB		*dmt_cb = &pdi_tbl_descr->qeu_pdi_dmtcb;
    QEU_CB		*qeu_cb = &pdi_tbl_descr->qeu_pdi_qeucb;
    QEF_DATA		*qef_data;
    i4			i;

    pdi_tbl_descr->qeu_pdi_num_rows = 0;
    pdi_tbl_descr->qeu_pdi_flags = 0;
    pdi_tbl_descr->qeu_pdi_obj_id.db_tab_base  = obj_id->db_tab_base;
    pdi_tbl_descr->qeu_pdi_obj_id.db_tab_index = obj_id->db_tab_index;

    /* initialize array of attribute pointers for the temporary table */
    for (i = 0, 
	 attr_p = pdi_tbl_descr->qeu_pdi_attr_descr, 
	 attr_pp = pdi_tbl_descr->qeu_pdi_attr_descr_p; 
	 
	 i < QEU_PERM_DEPTH_TBL_DEGREE; 
	 
	 i++, attr_p++, attr_pp++)
    {
	*attr_pp = attr_p;
    }
    
    attr_p = pdi_tbl_descr->qeu_pdi_attr_descr;

    /* attribute 1 - flags */
    attr_p[0].attr_type = DB_INT_TYPE;
    attr_p[0].attr_size = 2;
    attr_p[0].attr_precision = 0;
    attr_p[0].attr_flags = 0;
    attr_p[0].attr_collID = -1;
    SET_CANON_DEF_ID(attr_p[0].attr_defaultID, DB_DEF_NOT_DEFAULT);
    attr_p[0].attr_default = NULL;

    /* attributes 2-4 */
    for (i = 1; i < 4; i++)
        STRUCT_ASSIGN_MACRO(attr_p[0], attr_p[i]);

    /* attribute 5 - auth id */
    attr_p[4].attr_type = DB_CHA_TYPE;
    attr_p[4].attr_size = sizeof(DB_OWN_NAME);
    attr_p[4].attr_precision = 0;
    attr_p[4].attr_flags = 0;
    attr_p[4].attr_collID = -1;
    SET_CANON_DEF_ID(attr_p[4].attr_defaultID, DB_DEF_NOT_DEFAULT);
    attr_p[4].attr_default = NULL;

    /* attribute 6 - privilege */
    attr_p[5].attr_type = DB_INT_TYPE;
    attr_p[5].attr_size = 4;
    attr_p[5].attr_precision = 0;
    attr_p[5].attr_flags = 0;
    attr_p[5].attr_collID = -1;
    SET_CANON_DEF_ID(attr_p[5].attr_defaultID, DB_DEF_NOT_DEFAULT);
    attr_p[5].attr_default = NULL;
    
    for (i = 6; i < QEU_PERM_DEPTH_TBL_DEGREE; i++)
        STRUCT_ASSIGN_MACRO(attr_p[5], attr_p[i]);

    /* first attribute - misc. flags */
    MEmove((sizeof("flags") - 1), "flags", ' ',
        DB_MAXNAME, (char *) &attr_p[0].attr_name);

    /* second attribute - aggregate privilege depth prior to recalculation */
    MEmove((sizeof("old_depth") - 1), "old_depth", ' ',
        DB_MAXNAME, (char *) &attr_p[1].attr_name);

    /* third atribute - recomputed aggregate privilege depth */
    MEmove((sizeof("new_depth") - 1), "new_depth", ' ',
        DB_MAXNAME, (char *) &attr_p[2].attr_name);

    /* fourth atribute - grantee type */
    MEmove((sizeof("gtype") - 1), "gtype", ' ',
        DB_MAXNAME, (char *) &attr_p[3].attr_name);

    /* fifth attribute - grantee of the permit */
    MEmove((sizeof("auth_id") - 1), "auth_id", ' ',
        DB_MAXNAME, (char *) &attr_p[4].attr_name);
    
    /* sixth attribute - privilege specified in the permit */
    MEmove((sizeof("priv") - 1), "priv", ' ',
        DB_MAXNAME, (char *) &attr_p[5].attr_name);
    
    /* attributes 7-16 constitute an attribute map */
    {
        char	c[sizeof("attmrapa") - 1];
        char	*p = c + (i4)(sizeof("attmrapa") - 2);

        MEmove((sizeof("attmrapa") - 1), "attrmapa", ' ',
	    (sizeof("attmrapa") - 1), c);

        (*p)--;

        for (i = 6; i < QEU_PERM_DEPTH_TBL_DEGREE; i++)
        {
	    (*p)++;
	    MEmove((sizeof("attmrapa") - 1), c, ' ',
	        DB_MAXNAME, (char *) &attr_p[i].attr_name);
        }
    }

    dmt_cb->length = sizeof(DMT_CB);
    dmt_cb->type = DMT_TABLE_CB;
    dmt_cb->dmt_flags_mask = DMT_DBMS_REQUEST;
    dmt_cb->dmt_db_id = db_id;
    dmt_cb->dmt_char_array.data_address = 0;
    dmt_cb->dmt_char_array.data_in_size = 0;
    dmt_cb->dmt_attr_array.ptr_address = 
	(PTR) pdi_tbl_descr->qeu_pdi_attr_descr_p;
    dmt_cb->dmt_attr_array.ptr_in_count = QEU_PERM_DEPTH_TBL_DEGREE;
    dmt_cb->dmt_attr_array.ptr_size = sizeof (DMT_ATTR_ENTRY);
    dmt_cb->dmt_tran_id = qef_cb->qef_dmt_id;	    
    MEmove((sizeof("$default") - 1), "$default", ' ', 
        sizeof(DB_LOC_NAME), (char *) &pdi_tbl_descr->qeu_pdi_locname);
    dmt_cb->dmt_location.data_address = (PTR) &pdi_tbl_descr->qeu_pdi_locname;
    dmt_cb->dmt_location.data_in_size = sizeof(DB_LOC_NAME);

    status = dmf_call(DMT_CREATE_TEMP, dmt_cb);
    if (status != E_DB_OK)
    {
        if (dmt_cb->error.err_code == E_DM0078_TABLE_EXISTS)
        {
	    *error = E_QE0050_TEMP_TABLE_EXISTS;
	    status = E_DB_SEVERE;
        }
        else
        {
	    *error = dmt_cb->error.err_code;
        }
        
	return(status);
    }

    /* remember that the table has been created */
    pdi_tbl_descr->qeu_pdi_flags |= QEU_PDI_TBL_CREATED;

    /* 
    ** now open the temporary table; get an exclusive lock on it to
    ** save a few page-level locks
    */
    qeu_cb->qeu_type = QEUCB_CB;
    qeu_cb->qeu_length = sizeof(QEUCB_CB);
    qeu_cb->qeu_db_id = db_id;
    qeu_cb->qeu_lk_mode = DMT_X;
    qeu_cb->qeu_flag = DMT_U_DIRECT;
    qeu_cb->qeu_access_mode = DMT_A_WRITE;
    qeu_cb->qeu_mask = 0;
    qeu_cb->qeu_qual = qeu_cb->qeu_f_qual = 0;
    qeu_cb->qeu_qarg = qeu_cb->qeu_f_qarg = 0;

    qeu_cb->qeu_tab_id.db_tab_base  = dmt_cb->dmt_id.db_tab_base;
    qeu_cb->qeu_tab_id.db_tab_index = dmt_cb->dmt_id.db_tab_index;

    status = qeu_open(qef_cb, qeu_cb);
    if (status != E_DB_OK)
    {
        *error = qeu_cb->error.err_code;
        return(status);
    }

    /* remember that the table has been opened */
    pdi_tbl_descr->qeu_pdi_flags |= QEU_PDI_TBL_OPENED;

    qeu_cb->qeu_tup_length = sizeof(QEU_PERM_DEPTH_INFO);

    qeu_cb->qeu_output = qef_data = &pdi_tbl_descr->qeu_pdi_read_qefdata;
    qef_data->dt_next = 0;
    qef_data->dt_size = sizeof(QEU_PERM_DEPTH_INFO);
    qef_data->dt_data = (PTR) &pdi_tbl_descr->qeu_pdi_read_row;

    qeu_cb->qeu_input = qef_data = &pdi_tbl_descr->qeu_pdi_write_qefdata;
    qef_data->dt_next = 0;
    qef_data->dt_size = sizeof(QEU_PERM_DEPTH_INFO);
    qef_data->dt_data = (PTR) &pdi_tbl_descr->qeu_pdi_write_row;

    /* position the temp table "for all" */
    qeu_cb->qeu_key = (DMR_ATTR_ENTRY **) NULL;
    qeu_cb->qeu_klen = 0;

    return(E_DB_OK);
}

/*
** Name: qeu_drop_pdi_tbl - close and drop a temp table which was created
**                          to keep track of privileges whose depth has 
**			    changed as a result of revocation of specified 
**			    privilege
**
** Description:
**	This function will handle closing and destroying of a temp table
**      which was created to keep track of privileges whose depth has
**	changed as a result of revocation of specified privilege.
**	If the table was opened, we will try to close it.  If that fails,
**	we will return error unless the caller has requested that we try to
**	destroy it anyway.
**
** Input:
**	qef_cb			QEF session CB
**	pdi_tbl_descr		structure containing, among other things
**				control blocks which should be used to
**				close and drop the temp table
**	force			if TRUE, try to drop the table even if
**				the attempt to close it failed
**
** Output:
**	err_blk			filled in if an error occurred trying to
**				close the temp table and the caller has 
**				instructed us to try to drop the table anyway
**	error			filled in if an error occured
**
** History:
**	25-jul-93 (andre)
**	    written
*/
static DB_STATUS
qeu_drop_pdi_tbl(
	QEF_CB			*qef_cb,
	QEU_PDI_TBL_DESCR       *pdi_tbl_descr,
	bool			force,
	DB_ERROR		*err_blk,
	i4			*error)
{
    DB_STATUS		status = E_DB_OK;
    DB_STATUS		save_err_status = E_DB_OK;

    /* 
    ** if the table was not created, some error must have happened very
    ** soon after we allocated the structure - there is nothing for us to do
    */
    if (pdi_tbl_descr->qeu_pdi_flags & QEU_PDI_TBL_CREATED)
    {
	/* if table was opened, try to close it */
	if (pdi_tbl_descr->qeu_pdi_flags & QEU_PDI_TBL_OPENED)
	{
	    status = qeu_close(qef_cb, &pdi_tbl_descr->qeu_pdi_qeucb);
	    if (status != E_DB_OK)
            {
                *error = pdi_tbl_descr->qeu_pdi_qeucb.error.err_code;

		if (force)
		{
		    save_err_status = status;
		}
		else
		{
		    return(status);
		}
            }
        }

        pdi_tbl_descr->qeu_pdi_dmtcb.dmt_flags_mask = 0;
        status = dmf_call(DMT_DELETE_TEMP, &pdi_tbl_descr->qeu_pdi_dmtcb);
        if (status != E_DB_OK)
        {
            if (save_err_status != E_DB_OK)
            {
                (VOID) qef_error(*error, 0L, save_err_status, error, 
		    err_blk, 0);
            }
            *error = pdi_tbl_descr->qeu_pdi_dmtcb.error.err_code;
            if (save_err_status > status)
                status = save_err_status;
        }
    }
    
    return(status);
}

/*
** Name: qeu_rdfcb_init - common code to initialize RDF_CB
**
** Description:
**      this function will be called to zero out RDF_CB structure and initialize
**      common elements of RDF_CB.  caller will still be responsible for
**      customizing RDF_CB to its specific needs.
**
** Input:
**      rdf_cb                          ptr to RDF_CB to be initialized
**      qef_cb                          QEF session CB
**          qef_dbid                    database id
**          qef_udbid                   unique db id
**          qef_ses_id                  Pointer to session id
**          qef_c1_distrib              Flag indicating whether we are running
**                                      in distributed session
**
** Output:
**      rdf_cb                          initialized RDF_CB
**          rdf_rb                      request block
**              rdr_db_id                   = qef_dbid
**              rdr_unique_dbid             = qef_udbid
**              rdr_session_id              = qef_ses_id
**              rdr_r1_distrib              = qef_c1_distrib
**              rdr_fcb                     = Qef_s_cb->qef_rdffcb
**              rdr_2types_mask             |= RDR2_REFRESH
**
** History
**
**      07-sep-93 (andre)
**          written
**	6-Jul-2006 (kschendel)
**	    Correct unique dbid type (is i4).
*/
VOID
qeu_rdfcb_init(
	PTR		rdfcb_ptr,
	QEF_CB		*qef_cb)
{
    RDF_CB		    *rdf_cb = (RDF_CB *) rdfcb_ptr;
    RDR_RB                  *rdf_rb = &rdf_cb->rdf_rb;
    GLOBALREF QEF_S_CB      *Qef_s_cb;

    MEfill(sizeof(RDF_CB), (u_char) 0, (PTR) rdf_cb);
    
    rdf_rb->rdr_db_id	    = qef_cb->qef_dbid;
    rdf_rb->rdr_session_id  = qef_cb->qef_ses_id;
    rdf_rb->rdr_unique_dbid = qef_cb->qef_udbid;
    rdf_rb->rdr_r1_distrib  = qef_cb->qef_c1_distrib;
    rdf_rb->rdr_fcb	    = Qef_s_cb->qef_rdffcb;

    return;
}

/*
** Name: qeu_v_ubt_id		determine id of a view's underlying base table
**
** Description:
**	This function will obtain tree of a view (by calling RDF) and copy
**	the id of the view's underlying base table (ubt) from the view tree into
**	space provided by the caller.  Before exiting, this function will unfix
**	the view description to avoid having it permanently fixed in RDF's cache
**
** Input:
**	qef_cb			QEF session CB
**	view_id			id of the view
**
** Output:
**	ubt_id			if the view tree is found and it contains the id
**				of the view's ubt, that id will be returned 
**				inside of ubt_id; otherwise it will be zeroed 
**				out
**	err_blk			filled in if an error occurs
**
** Side effects:
**	RDF cache entry describing the view will be fixed, tree description 
**	will be obtained, then the cache entry will be unfixed
**
** History:
**	07-sep-93 (andre)
**	    written
**	22-sep-93 (andre)
**	    if running UPGRADEDB, do not try to look up id of view's underlying
** 	    base table - set ubt_id->db_tab_base and ubt_id->db_tab_index to 0 
** 	    and return making it look as if the id could not be found (which is
**	    not far from truth because pre-6.5 view trees will not contain 
**	    ubt_id + RDF may be unable to read older trees)
**	16-nov-93 (andre)
**	    we will NOT try to alter timestamp of any catalog in order to force 
**	    invalidation of QEPs.  This will help us avoid situation where we
**	    try to update a catalog which has been previously opened in 
**	    exclusive mode (which is a side-effect of altering its timestamp)
**	24-nov-93 (andre)
**	    instead of relying on the caller to return the error, we will 
**	    report it here (which will enable us to report the RDF error as 
**	    well.)  This requires that instead of providing us with a place to 
**	    store an error number, we be given an address of DB_ERROR
*/
DB_STATUS
qeu_v_ubt_id(
	QEF_CB		*qef_cb,
	DB_TAB_ID	*view_id,
	DB_TAB_ID	*ubt_id,
	DB_ERROR	*err_blk)
{
    RDF_CB		rdf_cb;
    PST_PROCEDURE   	*pnode;
    PST_QTREE       	*vtree;
    DB_STATUS		status;
    DB_STATUS		unfix_status;
    i4		error;

    if (qef_cb->qef_sess_flags & QEF_RUNNING_UPGRADEDB)
    {
	ubt_id->db_tab_base = ubt_id->db_tab_index = (i4) 0;
	return(E_DB_OK);
    }

    /* 
    ** first obtain the view description - RDF will not give us the view tree
    ** unless we first obtain view description
    */
    qeu_rdfcb_init((PTR) &rdf_cb, qef_cb);

    rdf_cb.rdf_rb.rdr_tabid.db_tab_base  = view_id->db_tab_base;
    rdf_cb.rdf_rb.rdr_tabid.db_tab_index = view_id->db_tab_index;

    rdf_cb.rdf_rb.rdr_types_mask = RDR_RELATION;

    status = rdf_call(RDF_GETDESC, &rdf_cb);

    if (DB_FAILURE_MACRO(status))
    {
	/* 
	** not good - we could not look up view definition - no cleanup 
	** is needed 
	*/
	(VOID) qef_error(E_QE0143_V_UBTID_GETDESC_ERR, 
	    rdf_cb.rdf_error.err_code, status, &error, err_blk, 2,
	    (i4) sizeof(view_id->db_tab_base), 
	    (i4 *) &view_id->db_tab_base,
	    (i4) sizeof(view_id->db_tab_index), 
	    (i4 *) &view_id->db_tab_index);
	return(status);
    }

    /* now obtain the view tree */
    rdf_cb.rdf_rb.rdr_types_mask = RDR_VIEW | RDR_QTREE;
    rdf_cb.rdf_rb.rdr_qtuple_count = 1;

    status = rdf_call(RDF_GETINFO, &rdf_cb);

    if (DB_FAILURE_MACRO(status))
    {
	/* 
	** not good - we could not look up view tree - need to unfix the 
	** RDF cache entry before exiting 
	*/
	(VOID) qef_error(E_QE0141_V_UBTID_GETINFO_ERR, 
	    rdf_cb.rdf_error.err_code, status, &error, err_blk, 2,
	    (i4) sizeof(view_id->db_tab_base), 
	    (i4 *) &view_id->db_tab_base,
	    (i4) sizeof(view_id->db_tab_index), 
	    (i4 *) &view_id->db_tab_index);
    }
    else
    {
        pnode = (PST_PROCEDURE *) rdf_cb.rdf_info_blk->rdr_view->qry_root_node;
        vtree = pnode->pst_stmts->pst_specific.pst_tree;

        /* 
        ** if the view tree contain id of the view's UBT (and it is not a 
	** catalog), copy it into space provided by the caller
        */
        if (vtree->pst_basetab_id.db_tab_base > DM_SCATALOG_MAX)
        {
            ubt_id->db_tab_base  = vtree->pst_basetab_id.db_tab_base;
            ubt_id->db_tab_index = vtree->pst_basetab_id.db_tab_index;
        }
        else
        {
	    ubt_id->db_tab_base = ubt_id->db_tab_index = (i4) 0;
        }
    }

    /*
    ** finally, unfix the RDF cache entry (remembering to reset rdr_types_mask
    ** which is now set to RDR_VIEW | RDR_QTREE)
    */

    /* 
    ** rdf_unfix() would be satisfied with 0, but we are unfixing a relation 
    ** cache entry - so we tell it like it is
    */
    rdf_cb.rdf_rb.rdr_types_mask = RDR_RELATION;

    unfix_status = rdf_call(RDF_UNFIX, &rdf_cb);

    if (DB_FAILURE_MACRO(unfix_status))
    {
	/* not good - we could not unfix the view's description */
	(VOID) qef_error(E_QE0142_V_UBTID_UNFIX_ERR, 
	    rdf_cb.rdf_error.err_code, unfix_status, &error, err_blk, 2,
	    (i4) sizeof(view_id->db_tab_base), 
	    (i4 *) &view_id->db_tab_base,
	    (i4) sizeof(view_id->db_tab_index), 
	    (i4 *) &view_id->db_tab_index);
	return((status > unfix_status) ? status : unfix_status);
    }

    return(status);
}

/*
** Name: qeu_mdfy_timestamp 	alter timestamp of specified table and 
**				invalidate RDF cache entry for that table 
** 
** Description:
**	This function will alter timestamp of a specified table and invalidate
**	the RDF cache entry describing that table
**
** Input:
**	qef_cb		QEF session CB
**	tbl_id		id of the table whose timestamp needs to be altered
**
** Output:
** 	error		filled in if an error occurs
**
** History:
**	07-sep-93 (andre)
**	    written
*/
DB_STATUS
qeu_mdfy_timestamp(
		QEF_CB		*qef_cb,
		DB_TAB_ID	*tbl_id,
		i4		*error)
{
    DMT_CB              dmt_cb;
    DB_STATUS		status;
    RDF_CB		rdf_cb;

    /*
    ** call dmt_alter() to change the timestamp
    */
    dmt_cb.dmt_flags_mask = DMT_TIMESTAMP_ONLY;
    dmt_cb.dmt_db_id = qef_cb->qef_dbid;
    dmt_cb.dmt_char_array.data_in_size = 0;
    dmt_cb.dmt_char_array.data_address = (PTR) NULL;
    dmt_cb.length = sizeof(DMT_CB);
    dmt_cb.type = DMT_TABLE_CB;
    dmt_cb.dmt_id.db_tab_base  = tbl_id->db_tab_base;
    dmt_cb.dmt_id.db_tab_index = tbl_id->db_tab_index;
    dmt_cb.dmt_tran_id = qef_cb->qef_dmt_id;
    status = dmf_call(DMT_ALTER, &dmt_cb);
    if (DB_FAILURE_MACRO(status))
    {
        *error = dmt_cb.error.err_code;
	return(status);
    }

    /* 
    ** now invalidate the RDF cache entry for the table whose timestamp has 
    ** been changed
    */
    qeu_rdfcb_init((PTR) &rdf_cb, qef_cb);
    
    rdf_cb.rdf_rb.rdr_tabid.db_tab_base  = tbl_id->db_tab_base;
    rdf_cb.rdf_rb.rdr_tabid.db_tab_index = tbl_id->db_tab_index;

    status = rdf_call(RDF_INVALIDATE, &rdf_cb);
    if (DB_FAILURE_MACRO(status))
    {
        *error = rdf_cb.rdf_error.err_code;
	return(status);
    }

    return(E_DB_OK);
}

/*
** Name: qeu_force_qp_invalidation - force invalidation of QPs by altering 
**				     timestamp of a base table 
**
** Description:
**	After some DDL statements against table or view, we need to invalidate
**	QPs of repeat queries and dbprocs which depend on the table or view
**	affected by the query.  After some deliberation we decided to 
**	accomplish this by altering timestamp of the base table affected by 
**	the query or by altering timestamp of some base table used in the 
**	definition of a view affected by the query.  
**
** 	This function will obtain description of the object (table or view) 
**	affected by the query.  If it is NOT a view AND if its timestamp
**	has not already been altered, it will proceed to so ao now.  If
**	it is a view, it will invoke qeu_v_ubt_id() to determine the id of 
**	view's underlying base table and, if one is found, proceed to
** 	update its timestamp
**
** Input:
**	qef_cb				QEF session CB
**	    .qef_dbid			database id
**	    .qef_ses_id			sesion id
**	obj_id				id of the table or view which was 
**					affected by a DDL query
**	obj_timestamp_already_altered	TRUE if timestamp of the object affected
**					by a DDL query has already been altered;
**					FALSE otherwise
**
** Output:
**	qps_invalidated			may be set to NULL if the user is not 
**					interested in this information; 
**					it will be set to TRUE if we 
**					successfully altered a timestamp of 
**					some base table or if the id passed by 
**					the caller was that of a base table and
**					the caller has already altered its 
**					timestamp; otherwise it will be set to 
**					FALSE
**	err_blk				filled in if an eror occurs
**
** Returns:
**	E_DB_{OK,ERROR}
**
** History:
**	09-sep-93 (andre)
**	    written
**	04-nov-93 (andre)
**	    added qps_invalidated to the interface.
**	16-nov-93 (andre)
**	    we will NOT try to alter timestamp of any catalog in order to force 
**	    invalidation of QEPs.  This will help us avoid situation where we
**	    try to update a catalog which has been previously opened in 
**	    exclusive mode (which is a side-effect of altering its timestamp)
**	24-nov-93 (andre)
**	    qeu_force_qp_invalidation() will report its own errors, therefore
**	    caller will have to provide us with an address of DB_ERROR rather 
**	    than just a place to put error number
**
**	    qeu_v_ubt_id() will take address of DB_ERROR and will report its own
**	    errors
*/
DB_STATUS
qeu_force_qp_invalidation(
		QEF_CB		*qef_cb,
		DB_TAB_ID	*obj_id,
		bool		obj_timestamp_already_altered,
		bool		*qps_invalidated,
		DB_ERROR	*err_blk)
{
    DMT_SHW_CB          dmt_show;
    DMT_TBL_ENTRY       dmt_tbl_entry;
    DB_STATUS		status;
    DB_TAB_ID		tbl_to_alter;
    i4		error;

    /* assume the worst */
    if (qps_invalidated)
        *qps_invalidated = FALSE;

    dmt_show.type = DMT_SH_CB;
    dmt_show.length = sizeof(DMT_SHW_CB);
    dmt_show.dmt_char_array.data_in_size = 0;
    dmt_show.dmt_char_array.data_out_size  = 0;
    STRUCT_ASSIGN_MACRO((*obj_id), dmt_show.dmt_tab_id);
    dmt_show.dmt_flags_mask = DMT_M_TABLE;
    dmt_show.dmt_db_id  = qef_cb->qef_dbid;
    dmt_show.dmt_session_id = qef_cb->qef_ses_id;
    dmt_show.dmt_table.data_address = (PTR) &dmt_tbl_entry;
    dmt_show.dmt_table.data_in_size = sizeof(DMT_TBL_ENTRY);
    dmt_show.dmt_char_array.data_address = (PTR)NULL;
    dmt_show.dmt_char_array.data_in_size = 0;
    dmt_show.dmt_char_array.data_out_size  = 0;

    status = dmf_call(DMT_SHOW, &dmt_show);

    if (   DB_FAILURE_MACRO(status) 
        && dmt_show.error.err_code != E_DM0114_FILE_NOT_FOUND)
    {
	(VOID) qef_error(dmt_show.error.err_code, 0L, status, &error, 
	    err_blk, 0);
        return(status);
    }

    if (dmt_tbl_entry.tbl_status_mask & DMT_VIEW)
    {
        status = qeu_v_ubt_id(qef_cb, obj_id, &tbl_to_alter, err_blk);
        if (DB_FAILURE_MACRO(status))
	    return(status);
	
	/*
	** if the view was based on no base tables except, perhaps, for core 
	** catalogs, its ubt_id will be set to 0, in which case there is 
	** nothing left for us to do
	*/
	if (!tbl_to_alter.db_tab_base)
	    return(E_DB_OK);
    }
    else if (   obj_timestamp_already_altered 
	     || dmt_tbl_entry.tbl_status_mask & DMT_CATALOG)
    {
	/*
	** if the object is not a view (i.e. is a base table or an index) and 
	** either we have already altered its timestamp or 
	** it is a core catalog ** so that its timestamp may not be altered, 
	** we are done
	*/

	/* 
	** if the caller altered timestamp of the object whose id was given to 
	** us, then the QPs affected by change in the status of that object 
	** have already been invalidated; if the id is that of a core catalog, 
	** we cannot alter its timestamp, caller will be told  (if he wants to 
	** know) that the QPs which could be affected by the change were NOT 
	** invalidated
	*/
	if (qps_invalidated)
	    *qps_invalidated = obj_timestamp_already_altered;

	return(E_DB_OK);
    }
    else
    {
	STRUCT_ASSIGN_MACRO((*obj_id), tbl_to_alter);
    }
    
    /* finally, we are ready to change the timestamp */
    status = qeu_mdfy_timestamp(qef_cb, &tbl_to_alter, &error);
    if (DB_FAILURE_MACRO(status))
    {
	(VOID) qef_error(error, 0L, status, &error, err_blk, 0);
    }
    else if (qps_invalidated)
    {
	*qps_invalidated = TRUE;
    }

    return(status);
}

/*
** Name: qeu_p_ubt_id           determine id of a dbproc's underlying base table
**
** Description:
**      This function will obtain IIPROCEDURE tuple describing the dbproc and 
**	copy id of a base table used in its definition from db_dbp_ubt_id into
**      space provided by the caller.
**
** Input:
**      qef_cb                  QEF session CB
**	dbp_owner		owner of the dbproc
**	dbp_name		name of the dbproc
**
** Output:
**      ubt_id                  if dbproc is not abandoned and its IIPROCEDURE 
**				tuple contains id of the dbproc's ubt, that id 
**				will be returned inside of ubt_id; otherwise it
**				will be zeroed out
**      error                   filled in if an error occurs
**
** History:
**      13-sep-93 (andre)
**          written
**	22-sep-93 (andre)
**	    if running UPGRADEDB, do not try to look up id of view's underlying
** 	    base table - set ubt_id->db_tab_base and ubt_id->db_tab_index to 0 
** 	    and return making it look as if the id could not be found (which is
**	    not far from truth because pre-6.5 view trees will not contain 
**	    ubt_id + RDF may be unable to read older trees)
*/
DB_STATUS
qeu_p_ubt_id(
	QEF_CB		*qef_cb,
	DB_OWN_NAME     *dbp_owner,
	DB_DBP_NAME	*dbp_name,
	DB_TAB_ID	*dbp_ubt_id,
	i4		*error)
{
    DB_STATUS	    status, local_status;
    QEU_CB	    dbpqeu, *dbp_qeu = (QEU_CB *) NULL;
    QEF_DATA	    dbpqef_data;
    DB_PROCEDURE    dbptuple;
    DMR_ATTR_ENTRY  dbpkey_array[2];
    DMR_ATTR_ENTRY  *dbpkey_ptr_array[2];
    i4		    exit_loop;

    if (qef_cb->qef_sess_flags & QEF_RUNNING_UPGRADEDB)
    {
	dbp_ubt_id->db_tab_base = dbp_ubt_id->db_tab_index = (i4) 0;
	return(E_DB_OK);
    }

    for (exit_loop = FALSE; !exit_loop; )
    {
	/* open IIPROCEDURE catalog */
	dbpqeu.qeu_type = QEUCB_CB;
        dbpqeu.qeu_length = sizeof(QEUCB_CB);
        dbpqeu.qeu_db_id = qef_cb->qef_dbid;
        dbpqeu.qeu_lk_mode = DMT_IS;
        dbpqeu.qeu_flag = DMT_U_DIRECT;
        dbpqeu.qeu_access_mode = DMT_A_READ;
        dbpqeu.qeu_tab_id.db_tab_base = DM_B_DBP_TAB_ID; 	
        dbpqeu.qeu_tab_id.db_tab_index = DM_I_DBP_TAB_ID; 	
	dbpqeu.qeu_mask = 0;

	status = qeu_open(qef_cb, &dbpqeu);
	if (DB_FAILURE_MACRO(status))
	{
	    *error = dbpqeu.error.err_code;
	    break;
	}
	dbp_qeu = &dbpqeu;

	/*
	** Get the iiprocedure tuple that matches procedure and owner names 
	*/
	dbp_qeu->qeu_count = 1;
        dbp_qeu->qeu_tup_length = sizeof(DB_PROCEDURE);
	dbp_qeu->qeu_output = &dbpqef_data;
	dbpqef_data.dt_next = 0;
        dbpqef_data.dt_size = sizeof(DB_PROCEDURE);
        dbpqef_data.dt_data = (PTR) &dbptuple;
	dbp_qeu->qeu_getnext = QEU_REPO;
	dbp_qeu->qeu_klen = 2;       
	dbp_qeu->qeu_key = dbpkey_ptr_array;
	
	dbpkey_ptr_array[0] = &dbpkey_array[0];
	dbpkey_ptr_array[0]->attr_number = DM_1_DBP_KEY;
	dbpkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	dbpkey_ptr_array[0]->attr_value = (char *) dbp_name;

	dbpkey_ptr_array[1] = &dbpkey_array[1];
	dbpkey_ptr_array[1]->attr_number = DM_2_DBP_KEY;
	dbpkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	dbpkey_ptr_array[1]->attr_value = (char *) dbp_owner;
    
	dbp_qeu->qeu_qual = 0;
	dbp_qeu->qeu_qarg = 0;
 
	status = qeu_get(qef_cb, dbp_qeu);
	if (status != E_DB_OK)
	{
	    if (dbp_qeu->error.err_code == E_QE0015_NO_MORE_ROWS)
		*error = E_QE0117_NONEXISTENT_DBP;
	    else
		*error = dbp_qeu->error.err_code;
	}

	exit_loop = TRUE;
    }
    
    if (status == E_DB_OK)
    {
	/* 
	** we got the tuple - if the dbproc is not dormant, copy id of the 
	** dbproc's underlying base table into caller's variable; otherwise, 
	** zero it out
	*/
	if (dbptuple.db_mask[0] & DB_DBP_INDEP_LIST)
	{
	    dbp_ubt_id->db_tab_base  = dbptuple.db_dbp_ubt_id.db_tab_base;
	    dbp_ubt_id->db_tab_index = dbptuple.db_dbp_ubt_id.db_tab_index;
	}
	else
	{
	    dbp_ubt_id->db_tab_base = dbp_ubt_id->db_tab_index = 0;
	}
    }

    /* Close IIPROCEDURE */

    if (dbp_qeu)
    {
	local_status = qeu_close(qef_cb, dbp_qeu);    
	if (local_status != E_DB_OK)
	{
	    *error = dbp_qeu->error.err_code;
	    if (local_status > status)
		status = local_status;
	}
    }

    return (status);
}

/*
** Name: qeu_destr_dbp_qp       destroy QP object for a given dbproc
**
** Description:
**      This function will destroy query plan object for a dbproc whose schema 
**	and name were supplied to it by the caller.  It will use the dbproc
**	schema and name to get a handle to the object (if it exists) and, if
**	successful, will proceed to destroy the object.
**
**	NOTE that this function reports its own errors so the caller should
**	avoid reporting it again.
**
** Input:
**      qef_cb                  QEF session CB
**	dbp_schema		schema to which the dbproc belongs
**	dbp_name		name of the dbproc
**
** Output:
**      err_blk                 filled in if an error occurs
**
** History:
**      04-mar-94 (andre)
**          written
*/
static DB_STATUS
qeu_destr_dbp_qp(
	QEF_CB		*qef_cb,
	DB_OWN_NAME	*dbp_schema,
	DB_DBP_NAME	*dbp_name,
	DB_ERROR	*err_blk)
{
    QSF_RCB         dbp_qsf_rb;
    char	    dbp_fe_id[sizeof(DB_CURSOR_ID) + DB_MAXNAME + sizeof(i4)];
    DB_CURSOR_ID    *fe_idp = (DB_CURSOR_ID *) dbp_fe_id;
    DB_STATUS	    status;
    i4	    error;

    /* 
    ** build up a FE object id that can be fed to QSO_JUST_TRANS to find 
    ** a dbproc QP
    */
    fe_idp->db_cursor_id[0] = fe_idp->db_cursor_id[1] = (i4) 0;
    MEcopy((PTR) dbp_name, sizeof((*dbp_name)),
	(PTR) fe_idp->db_cur_name);
    MEcopy((PTR) dbp_schema, sizeof((*dbp_schema)),
	(PTR) (dbp_fe_id + sizeof(DB_CURSOR_ID)));
    I4ASSIGN_MACRO(qef_cb->qef_udbid, 
	*(i4 *) (dbp_fe_id + sizeof(DB_CURSOR_ID) + sizeof((*dbp_schema))));

    /* initialize QSF_RCB */
    dbp_qsf_rb.qsf_type = QSFRB_CB;
    dbp_qsf_rb.qsf_ascii_id = QSFRB_ASCII_ID;
    dbp_qsf_rb.qsf_length = sizeof(QSF_RCB);
    dbp_qsf_rb.qsf_owner = (PTR) DB_QEF_ID;
    dbp_qsf_rb.qsf_sid = qef_cb->qef_ses_id;

    dbp_qsf_rb.qsf_feobj_id.qso_type = QSO_ALIAS_OBJ;
    dbp_qsf_rb.qsf_feobj_id.qso_lname = sizeof(dbp_fe_id);

    MEcopy((PTR) dbp_fe_id, sizeof(dbp_fe_id),
	(PTR) dbp_qsf_rb.qsf_feobj_id.qso_name);
    
    /* 
    ** by asking for a lock, we ensure that we will be given a QSF object handle
    ** which will be used to destroy the object
    */
    dbp_qsf_rb.qsf_lk_state = QSO_SHLOCK;

    status = qsf_call(QSO_JUST_TRANS, &dbp_qsf_rb);
    if (DB_FAILURE_MACRO(status))
    {
	if (dbp_qsf_rb.qsf_error.err_code == E_QS0019_UNKNOWN_OBJ)
	{
	    /* query plan not found in QSF - we are done */
	    return(E_DB_OK);
	}
	else
	{
	    /* some unexpected error */
	    (VOID) qef_error(E_QE0146_QSO_JUST_TRANS_ERROR, 
	        dbp_qsf_rb.qsf_error.err_code, status, &error, err_blk, 2,
	        (i4) sizeof(*dbp_schema), (i4 *) dbp_schema,
	        (i4) sizeof(*dbp_name), (i4 *) dbp_name);
	    return(status);
	}
    }

    status = qsf_call(QSO_DESTROYALL, &dbp_qsf_rb);
    if (DB_FAILURE_MACRO(status))
    {
        /* some unexpected error */
        (VOID) qef_error(E_QE0147_QSO_DESTROY_ERROR, 
            dbp_qsf_rb.qsf_error.err_code, status, &error, err_blk, 2,
            (i4) sizeof(*dbp_schema), (i4 *) dbp_schema,
            (i4) sizeof(*dbp_name), (i4 *) dbp_name);
        return(status);
    }

    return(E_DB_OK);
}

/*
** Name: qeu_v_col_drop         determine if view includes base table column  
**				that is being dropped.
**
** Description:
**	This function will obtain tree of a view (by calling RDF) and.......
**
** Input:
**	qef_cb			QEF session CB
**	view_id			id of the view
**	depobj_type		dependent object type
**
** Output:
**	err_blk			filled in if an error occurs
**
** Side effects:
**	RDF cache entry describing the dependent will be fixed, tree 
**	description will be obtained, then the cache entry will be unfixed
**
** History:
**	22-jul-96 (ramra01)
**	    Alter table add drop column implementation. Cascade view
**	    objects dependent on dropped column.
**	27-dec-1996 (nanpr01)
**	    Bug :79854 - Dependent views are not getting dropped correctly.
**	27-jan-1997 (nanpr01)
**	    Make the routine mode generic and fix bugs 80184, 80223, 80233.
**	27-mar-1997 (shero03)
**	    Bug 81216 - dependent views are not getting packed correctly.
**      08-dec-2005 (huazh01)
**          unfix the DB_RULE object before returning to caller routine. 
**          Otherwise, it causes a RDF memory leak and eventually 
**          leads to E_RD0043 error.
**          b111832, INGSRV2714. 
*/
DB_STATUS
qeu_v_col_drop(
	ULM_RCB		*ulm,
	QEF_CB		*qef_cb,
	QEU_CB		*tqeu,
	DB_TAB_ID	*dep_id,
	DB_TAB_ID	*base_id,
	DB_TREE_ID	*tree_id,
	i2		obj_num,
	DB_NAME		*obj_name,
	DB_OWN_NAME	*obj_owner,
	DB_QMODE	depobj_type,
	i4		col_attno,
	bool 		cascade,
	DB_ERROR	*err_blk)
{
    RDF_CB		rdfcb, *rdf_cb = &rdfcb;
    RDR_RB          	*rdf_inv_rb = &rdf_cb->rdf_rb;
    RDF_QT_PACK_RCB	pack_rcb;
    QEF_DATA		*qtree_tuples = (QEF_DATA *) NULL;
    DMR_ATTR_ENTRY	*tkey_ptr_array[3];
    DMR_ATTR_ENTRY	tkey_array[3];
    DMR_ATTR_ENTRY	**save_qeu_key;
    PST_PROCEDURE   	*pnode;
    PST_QTREE       	*vtree;
    DB_IITREE  		*qtree_ptr, qtree;
    DB_TAB_ID		loc_base_id;
    DB_STATUS		status = E_DB_OK;
    DB_STATUS		unfix_status = E_DB_OK;
    DB_STATUS		loc_status = E_DB_OK;
    i4		error = 0;
    i4		i;
    i4		irngtab;
    i2			qmode;
    bool		rep_depobj;

    /* 
    ** first obtain the view description - RDF will not give us the view tree
    ** unless we first obtain view description
    */
    qeu_rdfcb_init((PTR) rdf_cb, qef_cb);

    if (depobj_type == DB_VIEW)
    {
        rdf_cb->rdf_rb.rdr_tabid.db_tab_base  = dep_id->db_tab_base;
        rdf_cb->rdf_rb.rdr_tabid.db_tab_index = dep_id->db_tab_index;
    }
    else
    {
        rdf_cb->rdf_rb.rdr_tabid.db_tab_base  = base_id->db_tab_base;
        rdf_cb->rdf_rb.rdr_tabid.db_tab_index = base_id->db_tab_index;
    }
        
    rdf_cb->rdf_rb.rdr_types_mask = RDR_RELATION;
    status = rdf_call(RDF_GETDESC, rdf_cb);
    
    if (DB_FAILURE_MACRO(status))
    {
     /* 
     ** not good - we could not look up view definition - no cleanup 
     ** is needed 
     */
       (VOID) qef_error(E_QE0143_V_UBTID_GETDESC_ERR, 
	        rdf_cb->rdf_error.err_code, status, &error, err_blk, 2,
	        (i4) sizeof(rdf_cb->rdf_rb.rdr_tabid.db_tab_base), 
	        (i4 *) &rdf_cb->rdf_rb.rdr_tabid.db_tab_base,
	        (i4) sizeof(rdf_cb->rdf_rb.rdr_tabid.db_tab_index), 
	        (i4 *) &rdf_cb->rdf_rb.rdr_tabid.db_tab_index);
      return(status);
    }

    /* now obtain the associated query_tree */
    switch (depobj_type)
    {
	case DB_VIEW : 
	    rdf_cb->rdf_rb.rdr_types_mask = RDR_VIEW | RDR_QTREE;
	    STRUCT_ASSIGN_MACRO(*dep_id, loc_base_id);
	    break;
	case DB_RULE :
	    rdf_cb->rdf_rb.rdr_types_mask = RDR_RULE | RDR_QTREE;
	    rdf_cb->rdf_rb.rdr_rec_access_id = NULL;
	    STRUCT_ASSIGN_MACRO(*tree_id, rdf_cb->rdf_rb.rdr_tree_id);
	    MEcopy((PTR)obj_name, sizeof(DB_NAME),
			(PTR)&rdf_cb->rdf_rb.rdr_name.rdr_rlname);
	    MEcopy((PTR)obj_owner, sizeof(DB_OWN_NAME), 
			(PTR)&rdf_cb->rdf_rb.rdr_owner);
	    STRUCT_ASSIGN_MACRO(*base_id, loc_base_id);
	    break;
	case DB_INTG :
	    rdf_cb->rdf_rb.rdr_types_mask = RDR_INTEGRITIES | RDR_QTREE;
	    rdf_cb->rdf_rb.rdr_rec_access_id = NULL;
	    STRUCT_ASSIGN_MACRO(*tree_id, rdf_cb->rdf_rb.rdr_tree_id);
	    rdf_cb->rdf_rb.rdr_qrymod_id = obj_num;
	    rdf_cb->rdf_rb.rdr_update_op = 0;
	    rdf_cb->rdf_rb.rdr_integrity = NULL;
	    rdf_cb->rdf_rb.rdr_sequence = obj_num;
	    STRUCT_ASSIGN_MACRO(*base_id, loc_base_id);
	    break;
	default :
	    /* FIX ME */
	    break;
    }
    rdf_cb->rdf_rb.rdr_qtuple_count = 1;

    status = rdf_call(RDF_GETINFO, rdf_cb);

    if (DB_FAILURE_MACRO(status))
    {
	/* 
	** not good - we could not look up view tree - need to unfix the 
	** RDF cache entry before exiting 
	*/
	(VOID) qef_error(E_QE0141_V_UBTID_GETINFO_ERR, 
	    rdf_cb->rdf_error.err_code, status, &error, err_blk, 2,
	    (i4) sizeof(loc_base_id.db_tab_base), 
	    (i4 *) &loc_base_id.db_tab_base,
	    (i4) sizeof(loc_base_id.db_tab_index), 
	    (i4 *) &loc_base_id.db_tab_index);
    }
    else
    {
	rep_depobj = FALSE;

        /* point to the associated query_tree */
        switch (depobj_type)
        {
	    case DB_VIEW : 
              pnode = (PST_PROCEDURE *) rdf_cb->rdf_info_blk->rdr_view->
					qry_root_node;
	      tqeu->qeu_qual = 0;
	      tqeu->qeu_qarg = 0;
	      break;
	    case DB_RULE :
              pnode = (PST_PROCEDURE *) rdf_cb->rdf_rb.rdr_rule->qry_root_node;
	      tqeu->qeu_qual = (PTR)qeu_tr_ts_cmp;
	      tqeu->qeu_qarg = (PTR)tree_id;
	      break;
	    case DB_INTG :
              pnode = (PST_PROCEDURE *) rdf_cb->rdf_rb.rdr_integrity->
					qry_root_node;
	      tqeu->qeu_qual = (PTR)qeu_tr_ts_cmp;
	      tqeu->qeu_qarg = (PTR)tree_id;
	      break;
	   default :
	      /* FIX ME */
	    break;
         }

        vtree = pnode->pst_stmts->pst_specific.pst_tree;

	for (i = 0, irngtab = -1;
	     i < vtree->pst_rngvar_count;
	     i++)
	{
	    if ( (vtree->pst_rangetab[i]->pst_rngvar.db_tab_base ==
		  base_id->db_tab_base) &&
	    	 (vtree->pst_rangetab[i]->pst_rngvar.db_tab_index ==
		  base_id->db_tab_index) )
	    {
		  irngtab = i;
		  fixtree(vtree->pst_qtree, irngtab, 
		     	col_attno, &rep_depobj);
    		  if (DB_FAILURE_MACRO(status))
		  {
			/*
			** I have no other way of invalidating the RDF cache
			** other than this. This may be quite expensive but
			** without substantial RDF changes this is the only
			** way to fix the bug 80223.
			*/
			rdf_cb->rdf_info_blk = NULL;
			rdf_inv_rb->rdr_2types_mask |= RDR2_KILLTREES;
			rdf_inv_rb->rdr_db_id = NULL;

			loc_status = rdf_call(RDF_INVALIDATE, rdf_cb);
			if (DB_FAILURE_MACRO(loc_status))
			{
			    /*
			    ** not good - we could not look up view tree - 
			    ** need to invalidate RDF cache entry before exiting
			    */
			    (VOID) qef_error(E_QE0141_V_UBTID_GETINFO_ERR,
			    rdf_cb->rdf_error.err_code, status, &error, err_blk,
			    2, (i4) sizeof(loc_base_id.db_tab_base),
			    (i4 *) &loc_base_id.db_tab_base,
			    (i4) sizeof(loc_base_id.db_tab_index),
			    (i4 *) &loc_base_id.db_tab_index);
			}
			return(status);
		  }
	    }
	}

	if (irngtab == -1)
	{
	   status = E_DB_ERROR;
	}
	else if (rep_depobj)
        {
	    rdf_cb->rdf_rb.rdr_instr |= RDF_USE_CALLER_ULM_RCB;
	    rdf_cb->rdf_rb.rdf_fac_ulmrcb = (PTR) ulm;
	    rdf_cb->rdf_rb.rdr_types_mask = 0;
	    rdf_cb->rdf_rb.rdr_qt_pack_rcb = &pack_rcb;
	    if (depobj_type == DB_VIEW)
		rdf_cb->rdf_rb.rdr_qry_root_node = (PTR) vtree;
	    else
		rdf_cb->rdf_rb.rdr_qry_root_node = (PTR) pnode;
	    pack_rcb.rdf_qt_tuples = &qtree_tuples;
	    pack_rcb.rdf_qt_tuple_count = &tqeu->qeu_count;
	    pack_rcb.rdf_qt_mode = depobj_type;
	      
	    /* If RDF can't get (QEF) memory, garbage collect and retry */
	    while ( status == E_DB_OK &&
		    (status = rdf_call(RDF_PACK_QTREE, rdf_cb)) &&
		     rdf_cb->rdf_error.err_code == E_UL0005_NOMEM )
	    {
		status = qec_mfree(ulm);
	    }

	    /* Ensure that later RDF calls don't find this on! */
	    rdf_cb->rdf_rb.rdr_instr &= ~RDF_USE_CALLER_ULM_RCB;
	    
	    if ( status )
	    {
		(VOID) qef_error(E_QE0141_V_UBTID_GETINFO_ERR, 
		    rdf_cb->rdf_error.err_code, status, &error, err_blk, 2,
		    (i4) sizeof(loc_base_id.db_tab_base), 
		    (i4 *) &loc_base_id.db_tab_base,
		    (i4) sizeof(loc_base_id.db_tab_index), 
		    (i4 *) &loc_base_id.db_tab_index);
    	    }
	    else
	    {
	         tqeu->qeu_tup_length = sizeof(DB_IITREE);
	         tqeu->qeu_input = qtree_tuples;
	         tqeu->qeu_output = (QEF_DATA *) NULL;
	         tqeu->qeu_getnext = QEU_REPO;
	         tqeu->qeu_klen = 3;
		 save_qeu_key = tqeu->qeu_key;	/* save caller's value */
	         tqeu->qeu_key = tkey_ptr_array;

	         tkey_ptr_array[0] = &tkey_array[0];
	         tkey_ptr_array[0]->attr_number = DM_1_TREE_KEY;
	         tkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
	         tkey_ptr_array[0]->attr_value = 
					(char *) &loc_base_id.db_tab_base;

	         tkey_ptr_array[1] = &tkey_array[1];
	         tkey_ptr_array[1]->attr_number = DM_2_TREE_KEY;
	         tkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
	         tkey_ptr_array[1]->attr_value = 
					(char*) &loc_base_id.db_tab_index;

	         qmode = depobj_type;
	         tkey_ptr_array[2] = &tkey_array[2];
	         tkey_ptr_array[2]->attr_number = DM_3_TREE_KEY;
	         tkey_ptr_array[2]->attr_operator = DMR_OP_EQ;
	         tkey_ptr_array[2]->attr_value = (char *) &qmode;

	         status = qeu_delete(qef_cb, tqeu);
	         if (status != E_DB_OK)
	         {
		    *err_blk = tqeu->error;
	         }
		 else
		 {
		    status = qeu_append(qef_cb, tqeu);
		    if (status != E_DB_OK)
		       *err_blk = tqeu->error;
		 }
	         tqeu->qeu_key = save_qeu_key;	/* restore caller's value */
	    }
        }
    }

    /* b111832, INGSRV2714
    ** unfix DB_RULE object, otherwise, it causes a RDF memory leak
    ** and leads to E_RD0043 error.
    */
    if (depobj_type == DB_RULE)
    {
       rdf_cb->rdf_rb.rdr_types_mask = RDR_QTREE | RDR_RULE;
       unfix_status = rdf_call(RDF_UNFIX, (PTR)rdf_cb); 
       if (DB_FAILURE_MACRO(unfix_status))
       {
          (VOID) qef_error(E_QE0142_V_UBTID_UNFIX_ERR, 
               rdf_cb->rdf_error.err_code, unfix_status, &error, err_blk, 2,
               (i4) sizeof(loc_base_id.db_tab_base), 
               (i4 *) &loc_base_id.db_tab_base,
               (i4) sizeof(loc_base_id.db_tab_index), 
               (i4 *) &loc_base_id.db_tab_index);
          return((status > unfix_status) ? status : unfix_status);
       }
    }

    rdf_cb->rdf_rb.rdr_types_mask = RDR_RELATION;

    unfix_status = rdf_call(RDF_UNFIX, rdf_cb);

    if (DB_FAILURE_MACRO(unfix_status))
    {
	/* not good - we could not unfix the view's description */
	(VOID) qef_error(E_QE0142_V_UBTID_UNFIX_ERR, 
	    rdf_cb->rdf_error.err_code, unfix_status, &error, err_blk, 2,
	    (i4) sizeof(loc_base_id.db_tab_base), 
	    (i4 *) &loc_base_id.db_tab_base,
	    (i4) sizeof(loc_base_id.db_tab_index), 
	    (i4 *) &loc_base_id.db_tab_index);
	return((status > unfix_status) ? status : unfix_status);
    }

    /* Now invalidate the tree if tree was modified */
    if (rep_depobj)
    {
        rdf_cb->rdf_rb.rdr_types_mask &= ~RDR_RELATION;
        switch (depobj_type)
        {
	    case DB_VIEW : 
                    rdf_inv_rb->rdr_2types_mask |= RDR2_KILLTREES;
		    break;
	    case DB_RULE : 
		    rdf_inv_rb->rdr_2types_mask |= RDR2_ALIAS;
		    rdf_inv_rb->rdr_types_mask  |= RDR_RULE;
		    break;
	    case DB_INTG : 
                    rdf_inv_rb->rdr_2types_mask |= RDR2_KILLTREES;
		    break;
	}
	unfix_status = rdf_call(RDF_INVALIDATE, rdf_cb);
	if (DB_FAILURE_MACRO(unfix_status))
	{
	    /* not good - we could not unfix the view's description */
	    (VOID) qef_error(E_QE0142_V_UBTID_UNFIX_ERR, 
		rdf_cb->rdf_error.err_code, unfix_status, &error, err_blk, 2,
		(i4) sizeof(loc_base_id.db_tab_base), 
		(i4 *) &loc_base_id.db_tab_base,
		(i4) sizeof(loc_base_id.db_tab_index), 
		(i4 *) &loc_base_id.db_tab_index);
	    return((status > unfix_status) ? status : unfix_status);
	}
    }

    return(status);
}
/*
** Name : qeu_v_finddependency : This routine just find the dependencies
**
** Description:
**      This function will find any dependent column in the tree.
**
** Input:
**      *qnode :        pointer to the root node of the tree.
**      irngtab :       range table number matching the table number containing
**                      dropped column.
**      col_attno :     column number containing dropped column.
**
** Output:
**      drop_depobj :   whether a dependency is present
**
** Side Effects : Changes the cached qtree entry. The QTREE cache must
**	          be invalidated if any operation fails.
**
** History:
**	27-mar-1997 (shero03)
**	    Bug 81216: Created 
**	    Broke the finddependencyandfixtree into two separate routines 
**	    That way all the dependent trees can be check for dependencies 
**	    before any tree is changed.
**	06-may-1997 (nanpr01)
**	    Bug 81725 : dependency is reported when there is no dependency.
**       6-Oct-2006 (hanal04) Bug 116791
**          Modified to deal with view on view on table dependencies.
**          If a view references a view recurse into that view to see if that
**          view is dependent upon the base table column being dropped.
**          If a view up the tree is dependent it will be dropped so we should
**          be dropped.
**          Added checks for unioned views. The existing code failed to
**          check the union trees and would miss dependencies.
**          Modified the behaviour so that we drop out at the first sign of
**          a dependency on the drop column.
*/
static DB_STATUS
qeu_v_finddependency(
        ULM_RCB         *ulm,
        QEF_CB          *qef_cb,
        QEU_CB          *tqeu,
        DB_TAB_ID       *dep_id,
        DB_TAB_ID       *base_id,
        DB_TREE_ID      *tree_id,
        i2              obj_num,
        DB_NAME         *obj_name,
        DB_OWN_NAME     *obj_owner,
        DB_QMODE        depobj_type,
        i4              col_attno,
        bool            cascade,
        bool            *drop_depobj,
        DB_ERROR        *err_blk)
{
    RDF_CB		rdfcb, *rdf_cb = &rdfcb;
    RDR_RB          	*rdf_inv_rb = &rdf_cb->rdf_rb;
    PST_PROCEDURE   	*pnode;
    PST_QTREE       	*vtree;
    DB_IITREE  		*qtree_ptr, qtree;
    DB_TAB_ID		loc_base_id;
    DB_STATUS		status = E_DB_OK;
    DB_STATUS		unfix_status = E_DB_OK;
    i4		error = 0;
    i4		i;
    i4		irngtab;
    i2			qmode;
    RDF_CB		subrdfcb, *subrdf_cb = &subrdfcb;
    RDR_RB          	*subrdf_inv_rb = &subrdf_cb->rdf_rb;
    PST_QNODE           *qtunion;
    DMT_SHW_CB          dmt_show;
    DMT_TBL_ENTRY       dmt_tbl_entry;

    /* 
    ** first obtain the view description - RDF will not give us the view tree
    ** unless we first obtain view description
    */
    qeu_rdfcb_init((PTR) rdf_cb, qef_cb);

    if (depobj_type == DB_VIEW)
    {
        rdf_cb->rdf_rb.rdr_tabid.db_tab_base  = dep_id->db_tab_base;
        rdf_cb->rdf_rb.rdr_tabid.db_tab_index = dep_id->db_tab_index;
    }
    else
    {
        rdf_cb->rdf_rb.rdr_tabid.db_tab_base  = base_id->db_tab_base;
        rdf_cb->rdf_rb.rdr_tabid.db_tab_index = base_id->db_tab_index;
    }
        
    rdf_cb->rdf_rb.rdr_types_mask = RDR_RELATION;
    status = rdf_call(RDF_GETDESC, rdf_cb);
    
    if (DB_FAILURE_MACRO(status))
    {
     /* 
     ** not good - we could not look up view definition - no cleanup 
     ** is needed 
     */
       (VOID) qef_error(E_QE0143_V_UBTID_GETDESC_ERR, 
	        rdf_cb->rdf_error.err_code, status, &error, err_blk, 2,
	        (i4) sizeof(rdf_cb->rdf_rb.rdr_tabid.db_tab_base), 
	        (i4 *) &rdf_cb->rdf_rb.rdr_tabid.db_tab_base,
	        (i4) sizeof(rdf_cb->rdf_rb.rdr_tabid.db_tab_index), 
	        (i4 *) &rdf_cb->rdf_rb.rdr_tabid.db_tab_index);
      return(status);
    }

    /* now obtain the associated query_tree */
    switch (depobj_type)
    {
	case DB_VIEW : 
	    rdf_cb->rdf_rb.rdr_types_mask = RDR_VIEW | RDR_QTREE;
	    STRUCT_ASSIGN_MACRO(*dep_id, loc_base_id);
	    break;
	case DB_RULE :
	    rdf_cb->rdf_rb.rdr_types_mask = RDR_RULE | RDR_QTREE;
	    rdf_cb->rdf_rb.rdr_rec_access_id = NULL;
	    STRUCT_ASSIGN_MACRO(*tree_id, rdf_cb->rdf_rb.rdr_tree_id);
	    MEcopy((PTR)obj_name, sizeof(DB_NAME),
			(PTR)&rdf_cb->rdf_rb.rdr_name.rdr_rlname);
	    MEcopy((PTR)obj_owner, sizeof(DB_OWN_NAME), 
			(PTR)&rdf_cb->rdf_rb.rdr_owner);
	    STRUCT_ASSIGN_MACRO(*base_id, loc_base_id);
	    break;
	case DB_INTG :
	    rdf_cb->rdf_rb.rdr_types_mask = RDR_INTEGRITIES | RDR_QTREE;
	    rdf_cb->rdf_rb.rdr_rec_access_id = NULL;
	    STRUCT_ASSIGN_MACRO(*tree_id, rdf_cb->rdf_rb.rdr_tree_id);
	    rdf_cb->rdf_rb.rdr_qrymod_id = obj_num;
	    rdf_cb->rdf_rb.rdr_update_op = 0;
	    rdf_cb->rdf_rb.rdr_integrity = NULL;
	    rdf_cb->rdf_rb.rdr_sequence = obj_num;
	    STRUCT_ASSIGN_MACRO(*base_id, loc_base_id);
	    break;
	default :
	    /* FIX ME */
	    break;
    }
    rdf_cb->rdf_rb.rdr_qtuple_count = 1;

    status = rdf_call(RDF_GETINFO, rdf_cb);

    if (DB_FAILURE_MACRO(status))
    {
	/* 
	** not good - we could not look up view tree - need to unfix the 
	** RDF cache entry before exiting 
	*/
	(VOID) qef_error(E_QE0141_V_UBTID_GETINFO_ERR, 
	    rdf_cb->rdf_error.err_code, status, &error, err_blk, 2,
	    (i4) sizeof(loc_base_id.db_tab_base), 
	    (i4 *) &loc_base_id.db_tab_base,
	    (i4) sizeof(loc_base_id.db_tab_index), 
	    (i4 *) &loc_base_id.db_tab_index);
    }
    else
    {

        /* point to the associated query_tree */
        switch (depobj_type)
        {
	    case DB_VIEW : 
              pnode = (PST_PROCEDURE *) rdf_cb->rdf_info_blk->rdr_view->
					qry_root_node;
	      tqeu->qeu_qual = 0;
	      tqeu->qeu_qarg = 0;
	      break;
	    case DB_RULE :
              pnode = (PST_PROCEDURE *) rdf_cb->rdf_rb.rdr_rule->qry_root_node;
	      tqeu->qeu_qual = (PTR)qeu_tr_ts_cmp;
	      tqeu->qeu_qarg = (PTR)tree_id;
	      break;
	    case DB_INTG :
              pnode = (PST_PROCEDURE *) rdf_cb->rdf_rb.rdr_integrity->
					qry_root_node;
	      tqeu->qeu_qual = (PTR)qeu_tr_ts_cmp;
	      tqeu->qeu_qarg = (PTR)tree_id;
	      break;
	   default :
	      /* FIX ME */
	    break;
         }

        *drop_depobj = FALSE;
        vtree = pnode->pst_stmts->pst_specific.pst_tree;
        
        /* Start with the original select */
        qtunion = vtree->pst_qtree;

	for (i = 0; i < vtree->pst_rngvar_count; i++)
	{
	    if ( (vtree->pst_rangetab[i]->pst_rngvar.db_tab_base ==
		  base_id->db_tab_base) &&
	    	 (vtree->pst_rangetab[i]->pst_rngvar.db_tab_index ==
		  base_id->db_tab_index) )
	    {

                for (; qtunion != (PST_QNODE *) NULL;)
                {
	            preorder(qtunion, i, col_attno, drop_depobj);

	            if (!cascade && *drop_depobj)
	            {
		        err_blk->err_code = E_QE016B_COLUMN_HAS_OBJECTS;
		        status = E_DB_ERROR;
                        /* Set exit condition for outer loop */
                        i = vtree->pst_rngvar_count;
		        break;
		    }

                    if (*drop_depobj)
                    {
                        /* Set exit condition for outer loop */
                        i = vtree->pst_rngvar_count;
                        break;
                    }
                
                    if(qtunion->pst_sym.pst_type == PST_ROOT)
                    {
                        qtunion = qtunion->pst_sym.pst_value.pst_s_root.pst_union.pst_next;
                    }
                    else
                    {
                        break;
                    }
                }  /* for each union */
	    }
	}  /* for each range */

        if((*drop_depobj == FALSE) && (depobj_type == DB_VIEW))
        {
            for (i = 0; i < vtree->pst_rngvar_count; i++)
            {
                if ( (vtree->pst_rangetab[i]->pst_rngvar.db_tab_base !=
                       base_id->db_tab_base) ||
                      (vtree->pst_rangetab[i]->pst_rngvar.db_tab_index !=
                       base_id->db_tab_index) )
                {
                    /* See if this rng var is a view */
                    dmt_show.type = DMT_SH_CB;
                    dmt_show.length = sizeof(DMT_SHW_CB);
                    dmt_show.dmt_char_array.data_in_size = 0;
                    dmt_show.dmt_char_array.data_out_size  = 0;
                    dmt_show.dmt_tab_id.db_tab_base =  
                             vtree->pst_rangetab[i]->pst_rngvar.db_tab_base;
                    dmt_show.dmt_tab_id.db_tab_index =  
                             vtree->pst_rangetab[i]->pst_rngvar.db_tab_index;
                    dmt_show.dmt_flags_mask = DMT_M_TABLE;
                    dmt_show.dmt_db_id  = tqeu->qeu_db_id;
                    dmt_show.dmt_session_id = tqeu->qeu_d_id;
                    dmt_show.dmt_table.data_address = (PTR) &dmt_tbl_entry;
                    dmt_show.dmt_table.data_in_size = sizeof(DMT_TBL_ENTRY);
                    dmt_show.dmt_char_array.data_address = (PTR)NULL;
                    dmt_show.dmt_char_array.data_in_size = 0;
                    dmt_show.dmt_char_array.data_out_size  = 0;                
    
                    status = dmf_call(DMT_SHOW, &dmt_show);

                    if (status != E_DB_OK)
                    {
                        err_blk->err_code = dmt_show.error.err_code; 
                        return(status);
                    }

                    if(!(dmt_tbl_entry.tbl_status_mask & DMT_VIEW))
                        continue;
      
                    status = qeu_v_finddependency(ulm, qef_cb, tqeu, 
                                         &dmt_show.dmt_tab_id, base_id, 
                                         tree_id, obj_num, obj_name, obj_owner, 
                                         DB_VIEW, col_attno, cascade, 
                                         drop_depobj, err_blk);
                    if (status != E_DB_OK)
                    { 
                        return(status);
                    } 

                    if (*drop_depobj)
                    {
                        break;
                    }
                }        
            }
        }
    }

    rdf_cb->rdf_rb.rdr_types_mask = RDR_RELATION;

    unfix_status = rdf_call(RDF_UNFIX, rdf_cb);

    if (DB_FAILURE_MACRO(unfix_status))
    {
	/* not good - we could not unfix the view's description */
	(VOID) qef_error(E_QE0142_V_UBTID_UNFIX_ERR, 
	    rdf_cb->rdf_error.err_code, unfix_status, &error, err_blk, 2,
	    (i4) sizeof(loc_base_id.db_tab_base), 
	    (i4 *) &loc_base_id.db_tab_base,
	    (i4) sizeof(loc_base_id.db_tab_index), 
	    (i4 *) &loc_base_id.db_tab_index);
	return((status > unfix_status) ? status : unfix_status);
    }

    return(status);
}

/*
** Name: qeu_d_check_conix - look for constraint using specified table/index.
**
** Description:
**      This function reads the iidbdepends rows on this table, looking
**      for a constraint associated with the specified table/index. 
**      If iidbdepends can not be accessed or it has not yet been
**      modified to btree (as in a createdb) then retuen E_DB_OK and
**      report no dependent constraints.
**
** Input:
**
** Output:
**      found_one   TRUE means a constraint was found
**                  FALSE means a constraint was not found
**
** Side effects:
**
** History:
**      29-Apr-2003 (hanal04) Bug 109682 INGSRV 2155, Bug 109564 INGSRV 2237
**         Written to detect tables/indexes which have constraint dependencies.
**      15-mar-2004 (horda03) Bug 111919/INGSRV2745
**         Reduce X lock to S lock on IIDBDEPENDS.
**      13-mar-2005 (horda03) Bug 112560/INGSRV2880
**         Ensure base table is locked prior to scanning for constraints. Check
**         to see if iidbdepends already locked by TX, and if not release any
**         locks taken on iidbdepends when checking for constraints.
**	04-Oct-2006 (jonj)
**	   Take and hold a table Control lock rather than a Table lock.
*/
DB_STATUS
qeu_d_check_conix(
QEF_CB          *qef_cb,
QEUQ_CB         *qeuq_cb,
ULM_RCB         *ulm,
DB_TAB_ID       *ixid,
bool            temp_tbl,
i4              *error,
bool            *found_one)
{
    QEU_CB          vqeu;                   /* for iidbdepends table */
    QEF_DATA        vqef_data;
    DMR_ATTR_ENTRY  vkey_array[4];
    DMR_ATTR_ENTRY  *vkey_ptr_array[4];
    DB_STATUS       status;
    DB_IIDBDEPENDS  *dtuple;
    i4              objtype = DB_CONS;
    DB_TAB_ID       bt_constant = {0, 0};
    DMT_SHW_CB          dmt_show;
    DMT_TBL_ENTRY       dmt_tbl_entry;
    DMT_CB              dmt_cb;

    /*
    ** Confirm that IIDBDEPENDs exists (this might be createdb)
    ** and that it is a BTREE
    ** If IIDBDEPENDS is not a btree, qeu_get() will fail in dmr_position()
    **
    ** Format the DMT_SHOW request block
    */
    dmt_show.type = DMT_SH_CB;
    dmt_show.length = sizeof(DMT_SHW_CB);
    dmt_show.dmt_char_array.data_in_size = 0;
    dmt_show.dmt_char_array.data_out_size  = 0;
    dmt_show.dmt_tab_id.db_tab_base = DM_B_DEPENDS_TAB_ID;
    dmt_show.dmt_tab_id.db_tab_index = DM_I_DEPENDS_TAB_ID;
    dmt_show.dmt_flags_mask = DMT_M_TABLE;
    dmt_show.dmt_db_id  = qeuq_cb->qeuq_db_id;
    dmt_show.dmt_session_id = qeuq_cb->qeuq_d_id;
    dmt_show.dmt_table.data_address = (PTR) &dmt_tbl_entry;
    dmt_show.dmt_table.data_in_size = sizeof(DMT_TBL_ENTRY);
    dmt_show.dmt_char_array.data_address = (PTR)NULL;
    dmt_show.dmt_char_array.data_in_size = 0;
    dmt_show.dmt_char_array.data_out_size  = 0;

    status = dmf_call(DMT_SHOW, &dmt_show);
    if (status != E_DB_OK)
    {
	/* Ignore E_DM0114_FILE_NOT_FOUND, this might be createdb */
	if (dmt_show.error.err_code == E_DM0054_NONEXISTENT_TABLE)
	    return (E_DB_OK);
	/* Otherwise, an unexpected error */
        *error = dmt_show.error.err_code;
        return(status);
    }

    if(dmt_tbl_entry.tbl_storage_type != DMT_BTREE_TYPE)
    {
        return(E_DB_OK);
    }

    /* horda03 - Open the table to prevent additional constraints
    ** begin added then check to see if the TX already has a lock on the
    ** IIDBDEPENDs table. If it doesn't, then at the end of the function
    ** the IIDBDEPEND locks will be released.
    **
    ** (jonj) Instead of opening the table to get a lock on it (which
    ** causes deadlock with online modify), rather get a Control lock,
    ** which is compatible with online modify.
    **
    ** Temp tables don't use control locks, but I'm not sure we should
    ** be in here anyway if this is a temp table.
    */
    if ( !temp_tbl )
    {
	dmt_cb.type         = DMT_TABLE_CB;
	dmt_cb.length       = sizeof(DMT_CB);
	dmt_cb.dmt_db_id    = qeuq_cb->qeuq_db_id;
	dmt_cb.dmt_tran_id = qef_cb->qef_dmt_id;
	dmt_cb.dmt_id.db_tab_base = ixid->db_tab_base;
	dmt_cb.dmt_id.db_tab_index = ixid->db_tab_index;
	dmt_cb.dmt_flags_mask  = 0;
	dmt_cb.dmt_lock_mode   = DMT_S;
	status = dmf_call(DMT_CONTROL_LOCK, &dmt_cb);

	if (status != E_DB_OK)
	{
	    *error = dmt_cb.error.err_code;
	    return(status);
	}
    }


    dmt_cb.type         = DMT_TABLE_CB;
    dmt_cb.length       = sizeof(DMT_CB);
    dmt_cb.dmt_flags_mask = 0;
    dmt_cb.dmt_db_id    = qeuq_cb->qeuq_db_id;
    dmt_cb.dmt_id.db_tab_base = DM_B_DEPENDS_TAB_ID;
    dmt_cb.dmt_id.db_tab_index = DM_I_DEPENDS_TAB_ID;
    dmt_cb.dmt_tran_id = qef_cb->qef_dmt_id;

    status = dmf_call(DMT_TABLE_LOCKED, &dmt_cb);

    if (status != E_DB_OK)
    {
       return(status);
    }

    /* Allocate "iidbdepends" tuple buffer, then init the QEU_CB
    ** to be used for the retrieval. */
 
    ulm->ulm_psize = sizeof(DB_IIDBDEPENDS);
    if ((status = qec_malloc(ulm)) != E_DB_OK)
    {
        ulm_closestream(ulm);
        return(status);
    }
    dtuple = (DB_IIDBDEPENDS*) ulm->ulm_pptr;
 
    vqeu.qeu_type = QEUCB_CB;
    vqeu.qeu_length = sizeof(QEUCB_CB);
    vqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
    vqeu.qeu_flag = DMT_U_DIRECT;
    vqeu.qeu_mask = 0;
    vqeu.qeu_qual = vqeu.qeu_f_qual = 0;
    vqeu.qeu_qarg = vqeu.qeu_f_qarg = 0;
 
    vqeu.qeu_lk_mode = DMT_N;
    vqeu.qeu_access_mode = DMT_A_READ;
 
    vqeu.qeu_tab_id.db_tab_base = DM_B_DEPENDS_TAB_ID;
    vqeu.qeu_tab_id.db_tab_index = DM_I_DEPENDS_TAB_ID;

    /* Flag qeu routines so that they do not call qef_error()
    ** We want to do our own error handling.
    ** During createdb iidbdepends will not exist. That's OK
    */
    status = qeu_open(qef_cb, &vqeu);   /* open iidbdepends */
    if (status != E_DB_OK)
    {
        if(vqeu.error.err_code == E_DM0054_NONEXISTENT_TABLE)
        {
            return(E_DB_OK);
        }
        else
        {
            *error = vqeu.error.err_code;
            return(status);
        }
    }
 
    vqeu.qeu_count = 1;
    vqeu.qeu_tup_length = sizeof(DB_IIDBDEPENDS);
    vqeu.qeu_output = &vqef_data;
    vqef_data.dt_next = 0;
    vqef_data.dt_size = sizeof(DB_IIDBDEPENDS);
    vqef_data.dt_data = (PTR) dtuple;
 
    vqeu.qeu_key = vkey_ptr_array;
 
    /* Set up the 3 key parameters to be used for retrieval:
    **  <constraint tableID, constraint table indexID, type code>. */
 
    vkey_ptr_array[0] = &vkey_array[0];
    vkey_ptr_array[0]->attr_number = DM_1_DEPENDS_KEY;
    vkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
    vkey_ptr_array[0]->attr_value = (char *) &ixid->db_tab_base;
 
    vkey_ptr_array[1] = &vkey_array[1];
    vkey_ptr_array[1]->attr_number = DM_2_DEPENDS_KEY;
    vkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
    vkey_ptr_array[1]->attr_value = (char *) &bt_constant.db_tab_index;
 
    vkey_ptr_array[2] = &vkey_array[2];
    vkey_ptr_array[2]->attr_number = DM_3_DEPENDS_KEY;
    vkey_ptr_array[2]->attr_operator = DMR_OP_EQ;
    vkey_ptr_array[2]->attr_value = (char *) &objtype;
    vqeu.qeu_getnext = QEU_REPO;
    vqeu.qeu_klen = 3;
 
    for (;;)
    {
        status = qeu_get(qef_cb, &vqeu);
        if (status != E_DB_OK)
        {
            if (vqeu.error.err_code == E_QE0015_NO_MORE_ROWS) 
            {
                break;
            }
            *error = vqeu.error.err_code;
            return(status);         /* otherwise, return error */
        }
	vqeu.qeu_getnext = QEU_NOREPO;  /* just get next from here */

        if (dtuple->dbv_itype != DB_CONS ||
            dtuple->dbv_dtype != DB_INDEX) continue;
                                /* must be CONS/INDEX row */
        if (dtuple->dbv_dep.db_tab_base == ixid->db_tab_base &&
            dtuple->dbv_dep.db_tab_index == ixid->db_tab_index)
        {
            /* Found one. */ 
            *found_one = TRUE;
            break;
        }
    }
    
    status = qeu_close(qef_cb, &vqeu);

    return status;
}

/*
** Name: qeu_d_check_consix - look for second constraint using same 
**			index.
**
** Description:
**	This function reads the iidbdepends rows on this table, looking
**	for more than one constraint associated with the same index. If
**	this happens, index cannot be dropped as part of "drop constraint"
**	processing.
**
** Input:
**
** Output:
**	found_another	TRUE means a second constraint was found
**			FALSE means only one constraint uses the index
**
** Side effects:
**
** History:
**	7-apr-98 (inkdo01)
**	    Written for constraint index with options feature.
*/
static DB_STATUS
qeu_d_check_consix(
QEF_CB		*qef_cb,
QEUQ_CB		*qeuq_cb,
ULM_RCB		*ulm,
DB_TAB_ID	*ixid,
i4		*error,
bool		*found_another)

{
    QEU_CB	    vqeu;		    /* for iidbdepends table */
    QEF_DATA	    vqef_data;
    DMR_ATTR_ENTRY  vkey_array[4];
    DMR_ATTR_ENTRY  *vkey_ptr_array[4];
    DB_STATUS	    status;
    DB_IIDBDEPENDS  *dtuple;
    i4		    objtype = DB_CONS;
    DB_TAB_ID	    bt_constant = {0, 0};


    /* Allocate "iidbdepends" tuple buffer, then init the QEU_CB 
    ** to be used for the retrieval. */

    ulm->ulm_psize = sizeof(DB_IIDBDEPENDS);
    if ((status = qec_malloc(ulm)) != E_DB_OK)
    {
	ulm_closestream(ulm);
	return(status);
    }
    dtuple = (DB_IIDBDEPENDS*) ulm->ulm_pptr;

    vqeu.qeu_type = QEUCB_CB;
    vqeu.qeu_length = sizeof(QEUCB_CB);
    vqeu.qeu_db_id = qeuq_cb->qeuq_db_id;
    vqeu.qeu_flag = DMT_U_DIRECT;
    vqeu.qeu_mask = 0;
    vqeu.qeu_qual = vqeu.qeu_f_qual = 0;
    vqeu.qeu_qarg = vqeu.qeu_f_qarg = 0;

    vqeu.qeu_lk_mode = DMT_N;
    vqeu.qeu_access_mode = DMT_A_READ;

    vqeu.qeu_tab_id.db_tab_base = DM_B_DEPENDS_TAB_ID;    		
    vqeu.qeu_tab_id.db_tab_index = DM_I_DEPENDS_TAB_ID;    		
    status = qeu_open(qef_cb, &vqeu);	/* open iidbdepends */
    if (status != E_DB_OK)
    {
        *error = vqeu.error.err_code;
        return(status);
    }

    vqeu.qeu_count = 1;
    vqeu.qeu_tup_length = sizeof(DB_IIDBDEPENDS);
    vqeu.qeu_output = &vqef_data;
    vqef_data.dt_next = 0;
    vqef_data.dt_size = sizeof(DB_IIDBDEPENDS);
    vqef_data.dt_data = (PTR) dtuple;

    vqeu.qeu_key = vkey_ptr_array;

    /* Set up the 3 key parameters to be used for retrieval:
    **	<constraint tableID, constraint table indexID, type code>. */

    vkey_ptr_array[0] = &vkey_array[0];
    vkey_ptr_array[0]->attr_number = DM_1_DEPENDS_KEY;
    vkey_ptr_array[0]->attr_operator = DMR_OP_EQ;
    vkey_ptr_array[0]->attr_value = (char *) &ixid->db_tab_base;

    vkey_ptr_array[1] = &vkey_array[1];
    vkey_ptr_array[1]->attr_number = DM_2_DEPENDS_KEY;
    vkey_ptr_array[1]->attr_operator = DMR_OP_EQ;
    vkey_ptr_array[1]->attr_value = (char *) &bt_constant.db_tab_index;

    vkey_ptr_array[2] = &vkey_array[2];
    vkey_ptr_array[2]->attr_number = DM_3_DEPENDS_KEY;
    vkey_ptr_array[2]->attr_operator = DMR_OP_EQ;
    vkey_ptr_array[2]->attr_value = (char *) &objtype;

    vqeu.qeu_getnext = QEU_REPO;
    vqeu.qeu_klen = 3;

    /* Simply loop over all rows, looking for more than one constraint
    ** attached to our index. */

    for (;;)
    {
	status = qeu_get(qef_cb, &vqeu);
	if (status != E_DB_OK)
	{
	if (vqeu.error.err_code == E_QE0015_NO_MORE_ROWS) break;
				/* if done, exit loop */
	*error = vqeu.error.err_code;
	return(status);		/* otherwise, return error */
	}
	vqeu.qeu_getnext = QEU_NOREPO;	/* just get next from here */
	
	/* Check for at least 2 CONS/INDEX rows which match given index. */
	if (dtuple->dbv_itype != DB_CONS || 
	    dtuple->dbv_dtype != DB_INDEX) continue;
				/* must be CONS/INDEX row */
	if (dtuple->dbv_dep.db_tab_base == ixid->db_tab_base &&
	    dtuple->dbv_dep.db_tab_index == ixid->db_tab_index)
	{
	    /* Found at least one. If this is the second, we've done.
	    ** Otherwise, indicate we've found one and keep looping. */
	    *found_another = TRUE;
	    break;
	}
    }

    return(qeu_close(qef_cb, &vqeu));

}
/*
** Name: preorder :	Recursively traverses the parse tree to find the
**			view dependencies. 
**
** Description:
**	This function returns true value if there is a dependent view or
**	the attid's are modified.
**
** Input:
**	*qnode :	pointer to the root node of the tree.
**	irngtab :	range table number matching the table number containing
**			dropped column.
**	col_attno :	column number containing dropped column.
**
** Output:
**	drop_depobj :	whether a dependency is present
**
** Side effects:
**
** History:
**	27-dec-1996 (nanpr01)
**	    Bug :79854 - Dependent views are not getting dropped correctly.
**	27-jan-1997 (nanpr01)
**	    Make the routine generic.
*/
static VOID
preorder(
PST_QNODE	    *qnode,
i4		    irngtab,
i4		    col_attno,
bool		    *drop_depobj)
{
	if (qnode == NULL)
	    return;
	else
	{
	   if (qnode->pst_sym.pst_type == PST_VAR) 
	   {
		if ((qnode->pst_sym.pst_value.pst_s_var.pst_vno == irngtab) &&
		    (qnode->pst_sym.pst_value.pst_s_var.pst_atno.db_att_id
                                                         == col_attno))
		{
		    *drop_depobj = TRUE;
		    return;
		}
	   }
	   else if (qnode->pst_sym.pst_type == PST_RULEVAR)
	   {
		if ((qnode->pst_sym.pst_value.pst_s_var.pst_vno == irngtab) &&
		    (qnode->pst_sym.pst_value.pst_s_rule.pst_rltno 
							== col_attno))
		{
		    *drop_depobj = TRUE;
		    return;
		}
	   }
	}

	if (qnode->pst_left != NULL)
	    preorder(qnode->pst_left, irngtab, col_attno, drop_depobj);
	if (qnode->pst_right != NULL)
	    preorder(qnode->pst_right, irngtab, col_attno, drop_depobj);
	return;
}

/*
** Name: Fixtree:	Recursively traverses the parse tree to fix the 
**			attribute ids on the tree. 
**
** Description:
**	This function returns true value if the attid's are modified.
**
** Input:
**	*qnode :	pointer to the root node of the tree.
**	irngtab :	range table number matching the table number containing
**			dropped column.
**	col_attno :	column number containing dropped column.
**
** Output:
**	rep_depobj :	whether attribute id's were fixed because a column
**			was dropped.
**
** Side effects:
**
** History:
**	27-dec-1996 (nanpr01)
**	    Bug :79854 - Dependent views are not getting dropped correctly.
**	27-jan-1997 (nanpr01)
**	    Make this routine generic.
*/
static VOID
fixtree(
PST_QNODE	    *qnode,
i4		    irngtab,
i4		    col_attno,
bool		    *rep_depobj)
{
	if (qnode == NULL)
	    return;
	else
	{
	   if (qnode->pst_sym.pst_type == PST_VAR) 
	   {
		if ((qnode->pst_sym.pst_value.pst_s_var.pst_vno == irngtab) &&
		    (qnode->pst_sym.pst_value.pst_s_var.pst_atno.db_att_id
				> col_attno))
		{
		    qnode->pst_sym.pst_value.pst_s_var.pst_atno.db_att_id--;
		    *rep_depobj = TRUE;
		}
	   }
	   else if (qnode->pst_sym.pst_type == PST_RULEVAR)
	   {
		if ((qnode->pst_sym.pst_value.pst_s_var.pst_vno == irngtab) &&
		    (qnode->pst_sym.pst_value.pst_s_rule.pst_rltno 
							> col_attno))
		{
		    qnode->pst_sym.pst_value.pst_s_rule.pst_rltno--;
		    *rep_depobj = TRUE;
		}
	   }
	}

	if (qnode->pst_left != NULL)
	    fixtree(qnode->pst_left, irngtab, col_attno, rep_depobj);
	if (qnode->pst_right != NULL)
	    fixtree(qnode->pst_right, irngtab, col_attno, rep_depobj);
	return;
}
