/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <st.h>
#include    <qu.h>
#include    <bt.h>
#include    <me.h>
#include    <iicommon.h>
#include    <adf.h>
#include    <adfops.h>
#include    <gca.h>
#include    <dbdbms.h>
#include    <ddb.h>
#include    <dmf.h>
#include    <dmtcb.h>
#include    <dmrcb.h>
#include    <dmucb.h>
#include    <ulf.h>
#include    <ulm.h>
#include    <qsf.h>
#include    <scf.h>
#include    <qefrcb.h>
#include    <qefqeu.h>
#include    <qeuqcb.h>
#include    <rdf.h>
#include    <psfparse.h>
#include    <psfindep.h>
#include    <pshparse.h>
#include    <psftrmwh.h>

/*
**
**  Name: PSTDSQL.C - Semantic Routines for Dynamic SQL.
**
**  Description:
**      This module contains semantic routines for dynamic SQL statements.
**	These routines are: 
**
**          pst_prepare - for a PREPARE statement.
**          pst_execute - for an EXECUTE statement.
**          pst_describe - for a DESCRIBE statement.
**          pst_prmsub - for dynamic SQL parameter substitution.
**          pst_commit_dsql - for destroying statement prototypes.
**
**  History:
**      11-apr-87 (puree)
**          initial version.
**	28-may-87 (puree)
**	    modified pst_prepare and pst_execute for grant statement.
**	06-nov-87 (puree)
**	    modified pst_execute to add "savepoint" to dynamic SQL repertoire.
**	10-nov-87 (puree)
**	    applied qrymod to DSQL.
**	17-nov-87 (puree)
**	    remove range table from query prototype.
**	19-nov-87 (puree)
**	    modify pst_execute to return query tree pointer.
**	21-jul-1988 (stec)
**	    Make changes related to the new query tree structure.
**	07-sep-88 (stec)
**	    For EXISTS pst_resolve must not be called, follow code in pstnode.c
**	22-SEP-88 (stec)
**	    Save `nonupdt' flag in PST_PROTE header when preparing a statement;
**	    restore it on execution.
**	31-oct-88 (stec)
**	    Change interface to pst_prepare, pst_execute.
**	10-mar-89 (neil)
**	    Added support for preparing and describing rule statements.
**	18-apr-89 (jrb)
**	    Initialized db_prec to zero for decimal project.
**	21-apr-89 (neil)
**	    Changed interface to pst_ruledup for cursor support.
**	06-may-89 (ralph)
**	    Added support for preparing and executing 
**	    CREATE/ALTER/DROP ROLE/GROUP and
**	    GRANT/REVOKE for database privileges.
**	11-may-89 (neil)
**	    Added support for Dynamic SQL cursor statements.
**	11-sep-89 (ralph)
**	    Added support for preparing and executing 
**	    CREATE/ALTER/DROP USER/LOCATION,
**	    CREATE/DROP SECURITY_ALARM,
**	    ENABLE/DISABLE SECURITY_AUDIT.
**	10-nov-89 (neil)
**	    Alerters: Support for EVENT DDL and EVENT excutable statements.
**	16-apr-90 (nancy)
**	    Fixed bug 21097, byte alignment problem in pst_sqlatts(). See
**	    comment below.
**	08-aug-90 (ralph)
**	    Add support for PSQ_[GR]OKPRIV.
**	13-sep-90 (teresa)
**	    the following are now bitflags instead of booleans:
**	     PSQ_CLSALL, PSQ_ALLDELUPD
**	30-may-91 (andre)
**	    Made changes to pst_execute() to handle SET USER/GROUP/ROLE
**	20-sep-91 (rog)
**	    Fixed bug 39530, datatype resolution problem in pst_prmsub().
**	    See comment below.
**	07-oct-91 (rog)
**	    Fixed bug 40131, pst_execute() was trying to resolve a non-existent
**	    tree.
**	4-may-1992 (bryanp)
**	    Added support for new query modes: PSQ_DGTT and PSQ_DGTT_AS_SELECT.
**      20-may-92 (schang)
**          GW integration.  Add PSQ_REG_IMPORT, _REG_INDEX, _REG_REMOVE flags.
**	30-jun-1993 (shailaja)
**	    Changed cast of pst_qeuqcb from PTR to QEUQ_CB *.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	13-aug-93 (andre)
**	    fixed causes of compiler warnings
**	 7-jan-94 (swm)
**	    Bug #58635
**	    Added PTR cast for qsf_owner which has changed type to PTR.
**      15-nov-94 (rudtr01)
**          Bug #47329
**          Save ldbdesc for (STAR) COPY TABLE (pst_prepare).
**          Implement execute of prepared copy table (STAR). Someone forgot!
**      28-nov-94 (rudtr01)
**          Bug #47329
**	    Previous fix broke local copy table while fixing star 
**	    copy table (Mea Culpa!)  Made fix more specific.
**	    Also eliminated compile warnings.
**	29-oct-1996 (prida01)
**	    Ensure we initialise the pss_viewrng max count on each prepare. We
**	    do this in order to rebuild this range table after each prepare 
**	    statement.
**	08-apr-1997 (i4jo01)
**	    When a parameter list is given during open cursor and decimal
**	    values exist in child nodes, the precision value should be
**	    propagated to the higher nodes. 
**	11-Aug-1997 (jenjo02)
**	    Changed ulm_streamid from (PTR) to (PTR*) so that ulm
**	    can destroy those handles when the memory is freed.
**	15-Aug-1997 (jenjo02)
** 	    New field, ulm_flags, added to ULM_RCB in which to specify the
**	    type of stream to be opened, shared or private.
**      4-sept-97 (davri06)
**          b84163, b85101 - prida01's change to pst_prepare (Oct 96)
**          assumes an pst_execute is preceded by a pst_prepare. If
**          this is not the case initialise pss_viewrng.pss_maxrng.
**      05-Nov-98 (hanal04)
**          Modification to the above fix to ensure that the value of
**          pss_viewrng.pss_maxrng is set to the actual value
**          associated with a prepared statment rather than just
**          initialising it. b90168.
**	16-Jun-1999 (johco01)
**	    Cross integrate change 439089 (gygro01)
**	    Took out previous change 438631 because it introduced bug 94175.
**	    Change 437081 solves bugs 90168 and 94175.
**	18-Jun-1999 (schte01)
**       Added char cast to avoid unaligned access in axp_osf.   
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	14-nov-2000 (abbjo03) bug 89924
**	    Add missing case for PSQ_ATBL_DROP_COLUMN in pst_execute.
**	10-Jan-2001 (jenjo02)
**	    Supply session's SID to QSF in qsf_sid.
**	    Added *PSS_SELBLK parm to psf_mopen(), psf_mlock(), psf_munlock(),
**	    psf_malloc(), psf_mclose(), psf_mroot(), psf_mchtyp(),
**	    psl_rptqry_tblids(), psl_cons_text(), psl_gen_alter_text(),
**	    psq_tmulti_out(), psq_1rptqry_out(), psq_tout().
**      11-oct-2001 (stial01)
**          pst_execute() added rdf_unfix after pst_showtab (b106005)
**	06-nov-2001 (devjo01)
**	    Perform GCA_COL_ATT packing in pst_describe after calling
**	    pst_sqlatt.   This allows pst_sqlatt to work with
**	    aligned GCA_COL_ATT structs, addressing a problem in
**	    which the Tru64 optimizer outsmarts itself, and replaces
**	    the memcpy's used to prevent alignment access problems
**	    with aligned load and stores.
**	21-Dec-2001 (gordy)
**	    GCA_COL_ATT no longer defined with DB_DATA_VALUE.
**      06-mar-2002 (horda03)
**          There is no need to store the PSQ_ALLDELUPD flag in the
**          prototype during a PREPARE statement, as this will cause
**          a SQLWARN4 to be generated for the EXECUTE of the prepared
**          statement. Bug 106509
**	24-jun-2002 (somsa01)
**	    Additional change for bug 106509; we need to at least
**	    initialize pst_alldelupd to FALSE.
**      17-Feb-2003 (hanal04) Bug 109572 INGSRV 2089
**          Modified update of prototype range table entries in pst_execute()
**          to prevent SIGSEGV in pst_showtab().
**      24-jul-2003 (stial01)
**          changes for prepared inserts with blobs (b110620)
**      15-sep-2003 (stial01)
**          Blob optimization for prepared updates (b110923)
**      07-jun-2004 (stial01)
**          Blob optimization for updates only if current of cursor (b112427)
**      01-dec-2004 (stial01)
**	    Blob optimization for updates only if PSQ_REPCURS (b112427, b113501)
**	06-Jul-06 (kiria01) b116230
**	    psl_print becomes PSL_RS_PRINT bit in new field psl_rsflags
**	    to allow for extra flags in PST_RSDM_NODE
*/


/*
** Definition of all global variables referenced by this file.
*/

GLOBALREF PSF_SERVBLK     *Psf_srvblk;  /* PSF server control block ptr */
/*
**  Forward function references.
*/

#define	GCA_COL_ATT_SIZE \
    (((((GCA_COL_ATT*)0)->gca_attname - (char*)0) + DB_MAXNAME + \
      sizeof(ALIGN_RESTRICT) - 1) & ~(sizeof(ALIGN_RESTRICT) - 1))


static GCA_COL_ATT *
pst_sqlatt(
	PST_QNODE	*resdom,
	GCA_COL_ATT 	*sqlatts,
	i4		*tup_size);
static DB_STATUS
pst_dstobj(
	PSQ_CB		    *psq_cb,
	PSS_SESBLK	    *sess_cb,
	PST_PROTO	    *proto);

static DB_STATUS pst_descinput_walk(
	PSS_SESBLK *sess_cb,
	PSQ_CB *psq_cb,
	PST_PROTO *proto,
	GCA_COL_ATT *desc_base,
	PST_QNODE *root,
	i4 context_type,
	i4 context_length,
	bool *have_parm);

/*{
** Name: pst_prepare	- Semantic routine for SQL's PREPARE.
**
**      This routine is called when the grammar recognizes the complete 
**      PREPARE statement.  Its main function is to build a header for 
**      the just-defined prototype statement.  The header contains 
**      information needed to recreate control blocks/query tree (query
**	objects) for execution of the statment later on.  The prototype
**	headers are kept as a singly linked list with the head of the
**	list stored in pss_proto in the PSF session control block.
**
**	The query object itself is built by regular productions of the
**	grammar and is stored in QSF memory space.  Since prototypes
**	are not shared among sessions, we will keep them locked until
**	commit/abort time.
**
** Inputs:
**      psq_cb                          ptr to the parser control block.
**      sess_cb                         ptr to PSF session control block.
**      sname                           ptr to a null terminated statement
**					name (front-end name).
**	nonupdt				boolean indicator showing whether
**					tree is updateable based on syntax
**					checks.
**	updcollst			ptr to subtree representing
**					updateable columns.
**
** Outputs:
**      none
**	Returns:
**	    DB_STATUS
**	Exceptions:
**	    none
**
** Side Effects:
**	    Prototype header is built in PSF memory pool and linked to
**	    pss_proto list.
**
**  History:
**      11-apr-87 (puree)
**          initial version.
**	28-may-87 (puree)
**	    save end-of-queue values for table, user, and column queue
**	    in preparing a grant statement.
**	10-nov-87 (puree)
**	    save aux. range table in the prototype.  This table is needed
**	    by qrymod during DSQL execution phase.
**	17-nov-87 (puree)
**	    remove the range table. It is not needed. QRYMOD is executed
**	    during query parsing.
**	22-sep-88 (stec)
**	    Save `nonupdt' flag in PST_PROTE header.
**	31-oct-88 (stec)
**	    Change interface - add updateable column list.
**	06-may-89 (ralph)
**	    Added support for preparing and executing 
**	    CREATE/ALTER/DROP ROLE/GROUP and
**	    GRANT/REVOKE for database privileges.
**	11-may-89 (neil)
**	    Added support for saving cursor ids for Dynamic SQL cursor support.
**	11-sep-89 (ralph)
**	    Added support for preparing and executing 
**	    CREATE/ALTER/DROP USER/LOCATION,
**	    CREATE/DROP SECURITY_ALARM,
**	    ENABLE/DISABLE SECURITY_AUDIT.
**	08-aug-90 (ralph)
**	    Add support for PSQ_[GR]OKPRIV.
**	17-apr-91 (andre)
**	    fix bug 33598: if a prototype block is being reused, buffer area
**	    for pointers to DB_DATA_VALUEs will be allocated only if the new
**	    query involves more parameters than the old one.  Otherwise we will
**	    reuse the previously allocated buffer.  Always allocating the
**	    pointer buffer resulted in the phenomenon where we were freeing up
**	    the memory occupied by the query tree, but the pointer buffer area
**	    was not reused until we finally ran out of QSF memory.
**	30-may-91 (andre)
**	    save the return flag and the effective user, group, and role ids in
**	    the prototype header.
**	14-jan-91 (andre)
**	    if the user is reusing a statement name and the new statement
**	    involves no parameters, do NOT reset pst_plist to NULL;
**	    this also fixes bug 41833 since I was setting pst_plist to NULL
**	    without resetting pst_num_parmptrs to 0.
**	08-sep-92 (andre)
**	    save addresses of user, table, and column queues for REVOKE
**	02-dec-92 (andre)
**	    we are desupporting SET GROUP/ROLE - as a result we no longer need
**	    to save the effective group and role ids in the prototype header.
**	11-mar-93 (andre)
**	    save a copy of sess_cb->pss_stmt_flags in proto->pst_stmt_flags.  
**	15-nov-93 (andre)
**	    save a copy of pss_flattening_flags in proto->pst_flattening_flags
**
**	    PSS_SINGLETON_SUBSELECT got moved from pss_stmt_flags to
**	    pss_flattening_flags
**      15-nov-94 (rudtr01)
**          Bug #47329
**          Save ldbdesc for COPY TABLE (pst_prepare).
**      28-nov-94 (rudtr01)
**          Bug #47329
**	    Previous fix broke local copy table while fixing star 
**	    copy table (Mea Culpa!)  Made fix more specific.
**	29-oct-1996 (prida01)
**	    Initialise the pss_viewrng max count on each prepare.
**      06-Aug-1998 (hanal04)
**          Initialise the new proto element pst_view_maxrng when
**          a new prototype is created. b90168.
**      16-Jun-1999 (johco01)
**          Cross integrate change (gygro01)
**          Took out previous change because it introduced bug 94175.
**          Change 437081 solves bugs 90168 and 94175.
*/
DB_STATUS
pst_prepare(
	PSQ_CB             *psq_cb,
	PSS_SESBLK         *sess_cb,
	char		   *sname,
	bool		   nonupdt,
	PST_QNODE	   *updcollst)
{
    PST_PROTO		*proto;
    DB_STATUS		status;
    ULM_RCB		ulm_rcb;
    i4		err_code;

    /* Format ulm_rcb */

    sess_cb->pss_viewrng.pss_maxrng = -1;
    ulm_rcb.ulm_facility = DB_PSF_ID;
    ulm_rcb.ulm_poolid = Psf_srvblk->psf_poolid;
    ulm_rcb.ulm_blocksize = sizeof(PST_PROTO);
    ulm_rcb.ulm_memleft = &sess_cb->pss_memleft;
    /* Open a private, thread-safe stream */
    ulm_rcb.ulm_flags = ULM_PRIVATE_STREAM;

    /* Search the prototype header list for duplicated names */

    proto = sess_cb->pss_proto;	    /* get head of chain from session cb */

    if (proto == (PST_PROTO *) NULL)	/* we don't have a list yet */
    {
	/* Open ULM memory stream for the prototype header */

	/* Point ULM to streamid pointer */
	ulm_rcb.ulm_streamid_p = &sess_cb->pss_pstream;

	if ((status = ulm_openstream(&ulm_rcb)) != E_DB_OK)
	{
	    if (ulm_rcb.ulm_error.err_code == E_UL0004_CORRUPT)
	    {
		status = E_DB_FATAL;
	    }

	    if (ulm_rcb.ulm_error.err_code == E_UL0005_NOMEM)
	    {
		psf_error(E_PS0F02_MEMORY_FULL, 0L, PSF_CALLERR, 
		    &err_code, &psq_cb->psq_error, 0);
	    }
	    else
		(VOID) psf_error(E_PS0A05_BADMEMREQ, ulm_rcb.ulm_error.err_data,
			PSF_INTERR, &err_code, &psq_cb->psq_error, 0);

	    return (status);
	}
    }

    while (proto != (PST_PROTO *) NULL)
    {
	if (STcompare(sname, proto->pst_sname) == 0)
	    break;
	proto = proto->pst_next;
    }
    /*
    ** If found, delete the associated object(s) from QSF, reuse the
    ** prototype header block
    */

    if (proto != (PST_PROTO *)NULL)
    {
	if ((status = pst_dstobj(psq_cb, sess_cb, proto)) != E_DB_OK)
	    return (status);
    }
    else				/* no protype header found */
    {
	/* Allocate memory for the prototype header */

	ulm_rcb.ulm_streamid_p = &sess_cb->pss_pstream;
	ulm_rcb.ulm_psize = sizeof(PST_PROTO);
	if ((status = ulm_palloc(&ulm_rcb)) != E_DB_OK)
	{
	    if (ulm_rcb.ulm_error.err_code == E_UL0004_CORRUPT)
	    {
		status = E_DB_FATAL;
	    }

	    if (ulm_rcb.ulm_error.err_code == E_UL0005_NOMEM)
	    {
		psf_error(E_PS0F02_MEMORY_FULL, 0L, PSF_CALLERR, 
		    &err_code, &psq_cb->psq_error, 0);
	    }
	    else
		(VOID) psf_error(E_PS0A02_BADALLOC, ulm_rcb.ulm_error.err_code,
			PSF_INTERR, &err_code, &psq_cb->psq_error, 0);

	    return (status);
	}
	proto = (PST_PROTO *) ulm_rcb.ulm_pptr;

	/* prepend it to the head of the list */

	proto->pst_next = sess_cb->pss_proto;
	sess_cb->pss_proto = proto;

	/*
	** set pst_num_parmptrs to 0 to indicate that the prototype block does
	** not have a parameter pointer buffer area associated with it
	*/
	proto->pst_num_parmptrs = 0;
    }

    proto->pst_maxparm = sess_cb->pss_highparm;

    /*
    ** If the dynamic statement contains parameter markers
    **	    we may have to allocate a buffer area for storing pointers to
    **	    DB_DATA_VALUE's
    ** otherwise we just leave the existing buffer area, if any, alone
    */
    if (sess_cb->pss_highparm >= 0)
    {
	/*
	**  if the prototype block's pst_plist does not point to an array big
	**  enough to accomodate this query,
	**	allocate a buffer area for storing pointers to DB_DATA_VALUE's
	*/
	if (sess_cb->pss_highparm >= proto->pst_num_parmptrs)
	{
	    ulm_rcb.ulm_streamid_p = &sess_cb->pss_pstream;
	    proto->pst_num_parmptrs = sess_cb->pss_highparm + 1;
	    ulm_rcb.ulm_psize =  sizeof(PTR) * proto->pst_num_parmptrs;

	    if ((status = ulm_palloc(&ulm_rcb)) != E_DB_OK)
	    {
		if (ulm_rcb.ulm_error.err_code == E_UL0004_CORRUPT)
		{
		    status = E_DB_FATAL;
		}

		if (ulm_rcb.ulm_error.err_code == E_UL0005_NOMEM)
		{
		    psf_error(E_PS0F02_MEMORY_FULL, 0L, PSF_CALLERR, 
			&err_code, &psq_cb->psq_error, 0);
		}
		else
		    (VOID) psf_error(E_PS0A02_BADALLOC, ulm_rcb.ulm_error.err_code,
			    PSF_INTERR, &err_code, &psq_cb->psq_error, 0);

		return (status);
	    }

	    proto->pst_plist = (DB_DATA_VALUE **) ulm_rcb.ulm_pptr;
	}
    }

    /* Save session and query information in the prototype header */

    /* PSQ_CB information */

    (VOID) STcopy(sname, proto->pst_sname);
    proto->pst_mode = psq_cb->psq_mode;
    proto->pst_alldelupd = FALSE;
    proto->pst_clsall = (psq_cb->psq_flag & PSQ_CLSALL);
    proto->pst_ret_flag = psq_cb->psq_ret_flag;

    proto->pst_stmt_info.psq_stmt_blob_cnt = 0;
    proto->pst_stmt_info.psq_stmt_blob_colno = 0;
    proto->pst_stmt_info.psq_stmt_tabname[0] = '\0';

    /* can only do blob optimization if insert or update current of cursor */

    if (proto->pst_mode == PSQ_APPEND || proto->pst_mode == PSQ_REPCURS)
    {
	u_char		*stmt = sess_cb->pss_bgnstmt; /* or pss_qbuf */
	i4		i;
	PSS_RNGTAB	*rng = sess_cb->pss_resrng;
	DMT_ATT_ENTRY	**attribute;
	PSQ_STMT_INFO	*s = &proto->pst_stmt_info;

	/* find the table name in the insert/update statement */
	MEcopy(rng->pss_tabname.db_tab_name, DB_MAXNAME, s->psq_stmt_tabname);
	s->psq_stmt_tabname[DB_MAXNAME] = '\0';
	MEcopy(rng->pss_ownname.db_own_name, DB_MAXNAME, s->psq_stmt_ownname);
	s->psq_stmt_ownname[DB_MAXNAME] = '\0';

	s->psq_stmt_blob_cnt = 0;
	s->psq_stmt_blob_colno = 0;
	if (rng->pss_tabdesc->tbl_2_status_mask & DMT_HAS_EXTENSIONS)
	{
#ifdef xDEBUG
	    TRdisplay("PREPARE table having blob cols\n");
#endif
	    for (i = 0, attribute = rng->pss_attdesc + 1;
		 i < rng->pss_tabdesc->tbl_attr_count;
		 i++, attribute++)
	    {
		/* Count DMT_F_PERIPHERAL (blob) columns */
		if ((*attribute)->att_flags & DMT_F_PERIPHERAL)
		{
		    s->psq_stmt_blob_cnt++;
		    s->psq_stmt_blob_colno = (*attribute)->att_number;
		}
	    }
	}
#ifdef xDEBUG
	TRdisplay("PREPARE '%s' %x\n", stmt, s);
	TRdisplay("PREPARE resrng %32.32s %32.32s\n",
		rng->pss_ownname.db_own_name, rng->pss_tabname.db_tab_name);
	TRdisplay("PREPARE blob col %d cnt %d\n",
	    s->psq_stmt_blob_colno, s->psq_stmt_blob_cnt);
#endif
	/*
	** If a inserting a blob column, check for resdom = constant
	** The blob insert optimization cannot be done unless we are
	** preparing a constant for insertion e.g.,
	**  insert into t (col) values (:hostvar + 'x')  -> can't optimize
	**  insert into t (col) values (lower(:hostvar)) -> can't optimize
	**  insert into t (col) values (:hostvar)        -> CAN optimized
	*/
	if (s->psq_stmt_blob_colno)
	{
	    PST_QNODE		*pnode;
	    bool		optimize = FALSE;

	    for (pnode = sess_cb->pss_tlist;
		 pnode && pnode->pst_sym.pst_type == PST_RESDOM;
		 pnode = pnode->pst_left)
	    {
		PST_RSDM_NODE	*rsdm;
		rsdm = &pnode->pst_sym.pst_value.pst_s_rsdm;

		if (rsdm->pst_ttargtype == PST_ATTNO && 
		    rsdm->pst_rsno == s->psq_stmt_blob_colno)
		{
		    if (pnode->pst_right &&
			pnode->pst_right->pst_sym.pst_type == PST_CONST)
			optimize = TRUE;
		    break;
		}
	    }
#ifdef xDEBUG
	    if (!optimize)
	    {
		s->psq_stmt_blob_colno = 0;
		s->psq_stmt_blob_cnt = 0;
		TRdisplay("Can't optimize non-constant blob resdom\n");
	    }
	    else
		TRdisplay("Can optimize constant blob resdom\n");
#endif

	}
    }
    
    if (psq_cb->psq_mode == PSQ_SET_SESS_AUTH_ID)
	STRUCT_ASSIGN_MACRO(psq_cb->psq_user, proto->pst_user);
    
    STRUCT_ASSIGN_MACRO(psq_cb->psq_table, proto->pst_table);
    STRUCT_ASSIGN_MACRO(psq_cb->psq_result, proto->pst_result);
    STRUCT_ASSIGN_MACRO(psq_cb->psq_cursid, proto->pst_cursid);

    /* Save ldbdesc info for STAR COPY */
    if (psq_cb->psq_mode == PSQ_DDCOPY)
	STRUCT_ASSIGN_MACRO(*psq_cb->psq_ldbdesc, proto->pst_ldbdesc);

    /* PSS_SESBLK information */

    STRUCT_ASSIGN_MACRO(sess_cb->pss_ostream, proto->pst_ostream);
    STRUCT_ASSIGN_MACRO(sess_cb->pss_cbstream, proto->pst_cbstream);
    STRUCT_ASSIGN_MACRO(sess_cb->pss_tstream, proto->pst_tstream);

    /* copy the effective user identifier from the sess_cb */
    STRUCT_ASSIGN_MACRO(sess_cb->pss_user, proto->pst_eff_user);

    /* prevent psq_parseqry from unlocking the object */

    sess_cb->pss_ostream.psf_mstream.qso_handle = (PTR) NULL;
    sess_cb->pss_cbstream.psf_mstream.qso_handle = (PTR) NULL;
    sess_cb->pss_tstream.psf_mstream.qso_handle = (PTR) NULL;

    /*
    ** if the text stream was created, it was unlocked by psq_tout.  Need
    ** to lock it now.
    */

    if (proto->pst_tstream.psf_mstream.qso_handle != (PTR) NULL)
    {
	if ((status = psf_mlock(sess_cb, &proto->pst_tstream,
			    &psq_cb->psq_error)) != E_DB_OK)
		return (status);
    }

    /* Save updatibility flag */
    proto->pst_nupdt = nonupdt;

    /* Save update column tree */
    proto->pst_updcollst = updcollst;

    /*
    ** Some statement flags are of interest as we try to execute the statement;
    ** such flags will be saved in pst_stmt_flags and restored later in
    ** pst_execute();  the following is a complete list of "interesting"
    ** statement flags:
    **	    PSS_IMPL_COL_LIST_IN_DECL_CURS
    ** 'Why not copy all the flags?' you may ask.  Consider PSS_TXTEMIT flag for
    ** instance.  If you were to copy it here and restore it in pst_execute(), a
    ** final call to psl_sscan() would result in the scanner atempting to add
    ** the trailing blank to the text chain that was never opened with fairly
    ** dreadful consequences.  Rather than copying all the flags except for
    ** those that I think will cause trouble, I chose to copy the flag which I
    ** definitely need and which (I claim) will not cause trouble.
    */
    proto->pst_stmt_flags = sess_cb->pss_stmt_flags &
	(
	   PSS_IMPL_COL_LIST_IN_DECL_CURS
        );

    proto->pst_flattening_flags = sess_cb->pss_flattening_flags;

    psq_cb->psq_error.err_code = E_PS0000_OK;
    psq_cb->psq_error.err_data = E_PS0000_OK;
    return (status);
}

/*{
** Name: pst_execute	- Semantic routine for SQL's EXECUTE.
**
** Description:
**      This routine is called when the grammar recognizes the first part
**	of the EXECUTE statement.  The routine looks up the prototype
**	header from pss_proto list.  It then accesses the prototype
**	object from the saved root.  The query object is then reproduced
**	according to the saved 'qmode'.  Certain fields in PSF session
**	control block and parser control block are also restored.
**
** Inputs:
**      psq_cb                          ptr to the parser control block.
**      sess_cb                         ptr to PSF session control block.
**      sname                           ptr to a null terminated statement
**					name (front-end name).
**	nonupdt				address of boolean indicator showing
**					whether tree updateable.
**	updcollst			pointer to ptr to root of updateable
**					columns subtree
**
** Outputs:
**	*pnode				set to the address of the procedure node
**					of a query tree, if applicable.
**      *maxparm                        number of parameters specified in
**					the prototype.
**      *plist                          address of area for DB_DATA_VALUE
**					pointers for the parameters.
**	nonupdt				boolean indicator showing if
**					tree updateable.
**	updcollst			ptr to root of updateable
**					columns subtree
**
**	Returns:
**	    DB_STATUS
**	Exceptions:
**	    none
**
** Side Effects:
**	    A duplicate query tree/control block is created in QSF storage.
**
**  History:
**      11-apr-87 (puree)
**          initial version.
**	28-may-87 (puree)
**	    modify end-of-queue values for table, user, and column queue
**	    when executing a grant statement.
**	06-nov-87 (puree)
**	    add "savepoint" to dynamic SQL repertoire.
**	19-nov-87 (puree)
**	    let pst_execute locate the query header based on the
**	    query mode.  Return the query header to the caller.
**	21-jul-1988 (stec)
**	    Make changes related to the new query tree structure.
**	22-sep-88 (stec)
**	    Restore value of updatability flag.
**	31-oct-88 (stec)
**	    Change interface - add updateable column list.
**	    Restore parser range table (pss_auxrng).
**	09-mar-89 (neil)
**	    Accept new rule query modes for CREATE and DROP RULE.
**	    Restore the rule statement lists attached to DML statements.
**	    Fixed a grant bug.
**	21-apr-89 (neil)
**	    Changed interface to pst_ruledup for cursor support.
**	06-may-89 (ralph)
**	    Added support for preparing and executing 
**	    CREATE/ALTER/DROP ROLE/GROUP and
**	    GRANT/REVOKE for database privileges.
**	11-may-89 (neil)
**	    Restore the cursor id for DELETE/UPDATE cursor.
**	    Integrated extra bug fix for not checking psy_grant.
**	13-sep-89 (andre)
**	    interface with pst_treedup() has changed.
**	11-sep-89 (ralph)
**	    Added support for preparing and executing 
**	    CREATE/ALTER/DROP USER/LOCATION,
**	    CREATE/DROP SECURITY_ALARM,
**	    ENABLE/DISABLE SECURITY_AUDIT.
**	10-nov-89 (neil)
**	    Alerters: Support for CREATE & DROP EVENT (DDL) and REGISTER,
**	    REMOVE & RAISE EVENT (DML).
**	    Bug #1: Fix so that you cannot OPEN (PSQ_DEFCURS) non-SELECT query.
**	    Bug #2: Fix so that queues of objects are copied for all GRANT's.
**	08-aug-90 (ralph)
**	    Add support for PSQ_[GR]OKPRIV.
**	30-may-91 (andre)
**	    Made changes to handle SET USER/GROUP/ROLE
**	30-may-91 (andre)
**	    user will be prevented from executing statements if the effective
**	    user, group, or role identifier which was in effect at the time the
**	    query was prepared is different from a corresponding identifier
**	    currently in effect.
**	06-jun-91 (andre)
**	    add support for dynamically prepared COMMENT and CREATE SYNONYM
**	07-oct-91 (rog)
**	    Only try to resolve a tree if there is one.  Bug 40131.
**	    See comment below.
**	4-may-1992 (bryanp)
**	    Added support for new query modes: PSQ_DGTT and PSQ_DGTT_AS_SELECT.
**	4-sep-92 (andre)
**	    if processing SET USER/GROUP/ROLE, remember to set status to E_DB_OK
**	    because if everything goes well, its value will not change
**	08-sep-92 (andre)
**	    add handling for REVOKE
**	02-dec-92 (andre)
**	    since we are desupporting SET GROUP/ROLE, we will no longer compare
**	    effective group/role identifiers to deetrmine whether a query can be
**	    executed in the current context
**
**	    if processing a prepared SET SESSION AUTHORIZATION, copy
**	    proto->pst_user into psq_cb->psq_user (this was overlooked during
**	    the SET USER/GROUP/ROLE project - shame on me)
**	06-dec-92 (andre)
**	    added code to restore psy_u_colq and psy_r_colq
**	22-dec-92 (andre)
**	    CREATE VIEW will be represented by a PST_CREATE_VIEW statement
**	23-feb-93 (andre)
**	    add support for executing a prepared DROP SCHEMA statement
**	11-mar-93 (Andre)
**	    restore sess_cb->pss_stmt_flags from proto->pst_stmt_flags
**	13-mar-93 (andre)
**	    ALTER TABLE DROP CONSTRAINT will be represented by a
**	    PST_DROP_INTEGRITY statement
**	25-mar-93 (andre)
**	    Local and STAR CREATE TABLE are represented differently in PSF:
**	    
**	    Local				STAR
**	    -----				----
**
**	    Represented by a PST_PROCEDURE	Represented by a QEU_CB
**	    tree consisting of one
**	    PST_CREATE_TABLE node followed
**	    by 0 or more PST_CREATE_INTEGRITY
**	    and/or PST_EXEC_IMM (for self-
**	    referencing REFERENCES constraint)
**
**	    Query mode set to PSQ_CREATE	Query mode set to PSQ_DISTCREATE
**
**	    Processing for PSQ_DISTCREATE will be that which was done for
**	    PSQ_CREATE until now.  For Local CREATE TABLE, we will simply copy
**	    the PST_PROCEDURE node
**	08-apr-93 (andre)
**	    call pst_ruledup() to copy statement-level rules as well as
**	    row-level rules
**	10-jul-93 (andre)
**	    removed (PTR) cast to get rid of "assignment type mismatch" msg from
**	    ACC
**
**	    cast types of args to MEcopy() and QUinit() to agree with the
**	    prototype declaration
**	15-jul-93 (andre)
**	    act_crt_view was being used without being initialized
**	15-nov-93 (andre)
**	    restore pss_flattening_flags from proto->pst_flattening_flags
**	10-dec-93 (robf)
**          Add PSQ_C/A/DPROFILE for user profiles
**      15-nov-94 (rudtr01)
**          Bug #47329
**          Implement execute of prepared copy table (STAR). Someone forgot!
**      28-nov-94 (rudtr01)
**	    Also eliminated compile warnings.
**      4-sept-97 (davri06)
**          b84163, b85101 - prida01's change to pst_prepare (Oct 96)
**          assumes an pst_execute is preceded by a pst_prepare. If
**          this is not the case initialise pss_viewrng.pss_maxrng.
**	6-aug-98 (inkdo01)
**	    Fix to copy logic for prototype, to copy entire range table
**	    rather than just the pointer to its pointer array.
**	26-aug-1998 (nanpr01)
**	    When retrying the query, refresh the rdf cache calling 
**	    pst_showtab and updating the range table entry timestamp.
**      05-Nov-98 (hanal04)
**          Set the sess_cb->pss_viewrng.pss_maxrng according to the
**          value associated with the statement rather than just
**          initialising it. b90168.
**      16-Jun-1999 (johco01)
**          Cross integrate change (gygro01)
**          Took out previous change because it introduced bug 94175.
**          Change 437081 solves bugs 90168 and 94175.
**	    Change 437081 by inkdo01 fixes the logic of copying prototypes
** 	    It copies the entire range table, rather than just the 
**	    pointer to its pointer array.
**	26-aug-1998 (nanpr01)
**	    When retrying the query, refresh the rdf cache calling 
**	    pst_showtab and updating the range table entry timestamp.
**	15-nov-99 (inkdo01)
**	    Fix to nanpr01 fix, above, to call pst_showtab instead of 
**	    pst_rgent which has side effect of adding unwanted range table
**          entry.
**	18-feb-2000 (hayke02)
**	    Modified the range table copying code so that we now check
**	    for a NULL range table entry and set the corresponding action
**	    header range table entry to NULL if this is the case. This
**	    prevents a SEGV when we try to MEcopy() a NULL range table
**	    entry. This can occur when we execute queries of the form
**	    'insert into ... select ...' and 'create table ... as
**	    select ...'. This change fixes bug 100411.
**	15-nov-99 (inkdo01)
**	    Fix to nanpr01 fix, above, to call pst_showtab instead of 
**	    pst_rgent which has side effect of adding unwanted range table entry.
**	14-nov-2000 (abbjo03)
**	    Add missing case for PSQ_ATBL_DROP_COLUMN.
**      17-Feb-2003 (hanal04) Bug 109572 INGSRV 2089
**          Modified update of prototype range table entries to
**          skip NULL entries. This avoids a SIGSEGV in pst_showtab().
*/
#define	RT_NODE_SIZE \
	    sizeof(PST_QNODE) - sizeof(PST_SYMVALUE) + sizeof(PST_RT_NODE)

DB_STATUS
pst_execute(
	PSQ_CB             *psq_cb,
	PSS_SESBLK         *sess_cb,
	char		   *sname,
	PST_PROCEDURE	   **pnode,
	i4		   *maxparm,
	PTR		   *parmlist,
	bool		   *nonupdt,
	PST_QNODE	   **updcollst)
{
    PST_PROTO		*proto;
    PST_PROCEDURE	*prt_pnode, *act_pnode;
    PST_QNODE		*prt_root, *act_root;
    PST_QTREE		*prt_header, *act_header;
    PST_STATEMENT	*prt_stmt, *act_stmt;
    char		*prt_text, *act_text;
    i4			text_length;
    PSC_CURBLK		*csr;
    DB_STATUS		status;
    i4		err_code;
    QSF_RCB		qsf_rb;

    /* b84163, b85101 */
    sess_cb->pss_viewrng.pss_maxrng = -1;

    *pnode = (PST_PROCEDURE *) NULL;
    *updcollst = (PST_QNODE *) NULL;

    /* Search the prototype header list for the requested statement */

    proto = sess_cb->pss_proto;	/* get head of chain from session cb */

    while (proto != (PST_PROTO *) NULL)
    {
	if (STcompare(sname, proto->pst_sname) == 0)
	    break;
	proto = proto->pst_next;
    }

    /* Error, if not found */
    if (proto == (PST_PROTO *) NULL)
    {
	(VOID) psf_error(2304L, 0L, PSF_USERERR,
	    &err_code, &psq_cb->psq_error, 2, sizeof(sess_cb->pss_lineno),
	    &sess_cb->pss_lineno, STtrmwhite(sname), sname);
	return (E_DB_ERROR);
    }
    else if (MEcmp((PTR) &sess_cb->pss_user, (PTR) &proto->pst_eff_user,
	           sizeof(DB_OWN_NAME)))
    {
	DB_OWN_NAME	    *cur_id = &sess_cb->pss_user,
			    *old_id = &proto->pst_eff_user;

	/*
	** the current effective user identifier is different from that in
	** effect at the time the query was prepared - user will be prevented
	** from executing this statement
	*/
	(VOID) psf_error(E_PS0355_DIFF_EFF_USER, 0L, PSF_USERERR, &err_code,
	    &psq_cb->psq_error, 3, 
	    STlength(sname), sname,
	    psf_trmwhite(sizeof(DB_OWN_NAME), (char *) cur_id), cur_id,
	    psf_trmwhite(sizeof(DB_OWN_NAME), (char *) old_id), old_id);
	return (E_DB_ERROR);
    }
	

    /*
    ** The prototype header is found.  Obtain object pointer from QSF unless the
    ** prepared query was SET SESSION AUTHORIZATION, since no QSF object gets
    ** created for that statement
    */
    if (proto->pst_mode != PSQ_SET_SESS_AUTH_ID)
    {
	/* set up QSF control block */

	qsf_rb.qsf_type = QSFRB_CB;
	qsf_rb.qsf_ascii_id = QSFRB_ASCII_ID;
	qsf_rb.qsf_length = sizeof(qsf_rb);
	qsf_rb.qsf_owner = (PTR)DB_PSF_ID;
	qsf_rb.qsf_sid = sess_cb->pss_sessid;
	qsf_rb.qsf_obj_id.qso_lname = 0;

	qsf_rb.qsf_obj_id.qso_handle = 
			proto->pst_ostream.psf_mstream.qso_handle;
	status = qsf_call(QSO_INFO, &qsf_rb);

	if ((status != E_DB_OK) || (qsf_rb.qsf_root == NULL))
	{
	    (VOID) psf_error(E_PS0D19_QSF_INFO, qsf_rb.qsf_error.err_code,
		PSF_INTERR, &err_code, &psq_cb->psq_error, 0);
	    return (E_DB_ERROR);
	}
    }
    else
    {
        status = E_DB_OK;
    }

    /* Return no. of parameters and an area to store pointers to the
    ** DB_DATA_VALUE's to the caller.
    */
    *maxparm = proto->pst_maxparm;
    *parmlist = (PTR) proto->pst_plist;
    /*
    ** Determine the type of the object to be reproduced from the
    ** query mode saved in the prototype header.
    */

    /*
    ** Bug fix (neil):
    ** SELECT statement may not be EXECUTE'd, & non-SELECT may not be OPEN'd
    */
    if (   (proto->pst_mode == PSQ_RETRIEVE && psq_cb->psq_mode == PSQ_EXECQRY)
	|| (proto->pst_mode != PSQ_RETRIEVE && psq_cb->psq_mode == PSQ_DEFCURS)
       )
    {
	(VOID) psf_error(2302L, 0L, PSF_USERERR,
	    &err_code, &psq_cb->psq_error, 2, sizeof(sess_cb->pss_lineno),
	    &sess_cb->pss_lineno, STtrmwhite(sname), sname);

	return (E_DB_ERROR);
    }

    switch (proto->pst_mode)		/* original query mode */
    {
        case PSQ_DDCOPY:        /* COPY TABLE in STAR */
	{
	    /*
		We will now make a copy (sort of) of the query text for
		the xlated copy table stmt.  What we are really going to
		do is copy the first and only DD_PACKET which contains
		a pointer to the query text.   This works only because the
		copy table stmt always fits in one packet.
	    */
	    DD_PACKET *ppkt;    /* ptr to output of PREPARE */
	    DD_PACKET *epkt;    /* ptr to new packets for EXECUTE */

	    /* point to the PREPAREd packet */
	    ppkt = (DD_PACKET *)qsf_rb.qsf_root;

	    /* Open and allocate QSF memory for the actual control block */
	    if ((status = psf_mopen(sess_cb, QSO_QTEXT_OBJ, &sess_cb->pss_ostream,
				&psq_cb->psq_error)) != E_DB_OK)
		return (status);

	    if ((status = psf_malloc(sess_cb, &sess_cb->pss_ostream,
			(u_i2)sizeof(DD_PACKET),  &sess_cb->pss_object,
			&psq_cb->psq_error)) != E_DB_OK)
		return (status);

	    epkt = (DD_PACKET *) sess_cb->pss_object;

	    /* Fix the root in QSF */
	    if ((status = psf_mroot(sess_cb, &sess_cb->pss_ostream, sess_cb->pss_object,
			&psq_cb->psq_error)) != E_DB_OK)
		return (status);

	    /* Copy data from the prototype control block */
	    (VOID) MEcopy((PTR) ppkt, sizeof(DD_PACKET), (PTR) epkt);

	    /* reset the ldbdesc to its correct value */
	    STRUCT_ASSIGN_MACRO(proto->pst_ldbdesc, *psq_cb->psq_ldbdesc);

	    break;
	}

	case PSQ_SVEPOINT:	/* declare a savepoint */
	case PSQ_COMMIT:	/* commit work */
	case PSQ_COPY:		/* copy table */
	case PSQ_ROLLBACK:	/* rollback */
	case PSQ_RLSAVE:	/* rollback to savepoint */
	/*
	**  There is one prototype object with multiple blocks.
	**  The root of the object is the QEF_RCB block.  We will
	**  create a new QSF object for this block and use the prototype
	**  copy for other blocks.  This way, we simply copy pointers
	**  to the new QEF_RCB block.  When query execution is completed,
	**  SCF will destroy the new QEF_RCB, but not the prototype.
	*/
	{
	    QEF_RCB	    *prt_cb, *act_cb;


	    /* Set up pointer to the prototype control block */

	    prt_cb = (QEF_RCB *) qsf_rb.qsf_root;

	    /* Open and allocate QSF memory for the actual control block */

	    if ((status = psf_mopen(sess_cb, QSO_QP_OBJ, &sess_cb->pss_ostream,
			&psq_cb->psq_error)) != E_DB_OK)
		return (status);

	    if ((status = psf_malloc(sess_cb, &sess_cb->pss_ostream, sizeof(QEF_RCB),
			&sess_cb->pss_object, &psq_cb->psq_error)) != E_DB_OK)
		return (status);

	    act_cb = (QEF_RCB *) sess_cb->pss_object;

	    /* Fix the root in QSF */

	    if ((status = psf_mroot(sess_cb, &sess_cb->pss_ostream, sess_cb->pss_object,
			&psq_cb->psq_error)) != E_DB_OK)
		return (status);
	    
	    /* Copy data from the prototype control block */

	    (VOID) MEcopy((PTR) prt_cb, sizeof(QEF_RCB), (PTR) act_cb);

	    break;
	}

	case PSQ_DESTROY:	/* drop table, drop view, drop object */
	case PSQ_REG_REMOVE:    /* remove registered table/index */
	case PSQ_DSTINTEG:	/* drop integrity on */
	case PSQ_DSTPERM:	/* drop permit on */
	case PSQ_DSTRULE:	/* drop rule */
	case PSQ_KALARM:	/* drop security_alarm */
	case PSQ_ENAUDIT:	/* enable security_audit */
	case PSQ_DISAUDIT:	/* disable security_audit */
	case PSQ_EVDROP:	/* drop event */
	case PSQ_COMMENT:	/* comment on */
	case PSQ_CSYNONYM:	/* create synonym */
	case PSQ_DSYNONYM:	/* drop synonym */
	case PSQ_DROP_SCHEMA:	/* drop schema */
	case PSQ_ATBL_DROP_COLUMN:	/* alter table drop column */
	{
	/*
	**  The PSY_CB is created in QSF and duplicated from the
	**  prototype.
	*/
	    PSY_CB	    *prt_cb, *act_cb;

	    /* Set up pointer to the prototype control block */

	    prt_cb = (PSY_CB *) qsf_rb.qsf_root;

	    /* Open and allocate QSF memory for the actual control block */

	    if ((status = psf_mopen(sess_cb, QSO_QP_OBJ, &sess_cb->pss_ostream,
			&psq_cb->psq_error)) != E_DB_OK)
		return (status);

	    if ((status = psf_malloc(sess_cb, &sess_cb->pss_ostream, sizeof(PSY_CB),
			&sess_cb->pss_object, &psq_cb->psq_error)) != E_DB_OK)
		return (status);

	    act_cb = (PSY_CB *) sess_cb->pss_object;

	    /* Fix the root in QSF */

	    if ((status = psf_mroot(sess_cb, &sess_cb->pss_ostream, sess_cb->pss_object,
			&psq_cb->psq_error)) != E_DB_OK)
		return (status);
	    
	    /* Copy data from the prototype control block */

	    (VOID) MEcopy((PTR) prt_cb, sizeof(PSY_CB), (PTR) act_cb);


	    break;
	}

	case PSQ_APPEND:	/* insert */
	case PSQ_DELETE:	/* delete */
	case PSQ_REPLACE:	/* update */
	case PSQ_RETRIEVE:	/* select */
	case PSQ_RETINTO:	/* create table .. as select ... */
	case PSQ_DGTT_AS_SELECT:
				/* declare global temporary table as select */
	case PSQ_DELCURS:	/* delete ... where current of */
	case PSQ_REPCURS:	/* update ... where current of */
	{
	    PSS_DUPRB		dup_rb;

	    /* initialize fields inside the dup_rb */
	    dup_rb.pss_op_mask = 0;
	    dup_rb.pss_num_joins = PST_NOJOIN;
	    dup_rb.pss_tree_info = (i4 *) NULL;
	    dup_rb.pss_mstream = &sess_cb->pss_ostream;
	    dup_rb.pss_err_blk = &psq_cb->psq_error;

	/*
	**  Create a new QSF object for the query header and tree.
	**  Duplicate them from the prototype.  At the completion
	**  of execution, this new object is destroyed by SCF.
	*/
	    
	    /* Set up pointers to the prototype tree */

	    prt_pnode = (PST_PROCEDURE *) qsf_rb.qsf_root;
	    prt_stmt = prt_pnode->pst_stmts;
	    prt_header = prt_stmt->pst_specific.pst_tree;
	    prt_root = prt_header->pst_qtree;

	    /* Open and allocate QSF memory for the actual tree */
	    status = psf_mopen(sess_cb, QSO_QTREE_OBJ, &sess_cb->pss_ostream,
		&psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    /* Allocate and copy tree header */
	    status = psf_malloc(sess_cb, &sess_cb->pss_ostream, sizeof(PST_QTREE),
		(PTR *) &act_header, &psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    (VOID) MEcopy((PTR) prt_header, sizeof(PST_QTREE),
		(PTR) act_header);

	    /*
	    ** Allocate and initialize an array of pointers to range entry 
	    ** structure. Then allocate and copy the PST_RANGETAB's themselves.
	    */
	    if (act_header->pst_rngvar_count > 0)
	    {
		i4	i;
		PST_RNGENTRY	*rngent;
		RDF_CB           rdf_cb;

		if (sess_cb->pss_retry & PSS_REFRESH_CACHE)
		{
		    PSS_RNGTAB      rngvar;
		    DB_TAB_TIMESTAMP    *timestamp;

		    /* Update the prototype range table entries. */
		    for (i = prt_header->pst_rngvar_count - 1; i >= 0; i--)
		    {
			if(prt_header->pst_rangetab[i] == NULL)
			{
			    continue;
			}

			MEfill(sizeof(PSS_RNGTAB), (u_char)0,
					(PTR)&rngvar);
			status = pst_showtab (sess_cb, PST_SHWID,
				(DB_TAB_NAME *) NULL, 
				(DB_TAB_OWN *) NULL,
				&prt_header->pst_rangetab[i]->pst_rngvar, 
				FALSE, &rngvar, proto->pst_mode, 
				&psq_cb->psq_error);
			if (DB_FAILURE_MACRO(status))
			{
			    return(status);
			}
			timestamp = &rngvar.pss_tabdesc->tbl_date_modified;
			STRUCT_ASSIGN_MACRO(*timestamp, 
				prt_header->pst_rangetab[i]->pst_timestamp);
			pst_rdfcb_init(&rdf_cb, sess_cb);
			rdf_cb.rdf_info_blk = rngvar.pss_rdrinfo;
			status = rdf_call(RDF_UNFIX, (PTR) &rdf_cb);
			if (status != E_DB_OK)
			{
			    (VOID) psf_rdf_error(RDF_UNFIX, &rdf_cb.rdf_error,
				&psq_cb->psq_error);
			    return (status);
			}
		    }
		}

		i = act_header->pst_rngvar_count;
		status = psf_malloc(sess_cb, &sess_cb->pss_ostream, 
				i * sizeof(PST_RNGENTRY *),
				(PTR *) &act_header->pst_rangetab, 
				&psq_cb->psq_error);
		if (DB_FAILURE_MACRO(status))
		    return (status);

		for (i = act_header->pst_rngvar_count - 1; i >= 0; i--)
		{
		        if (prt_header->pst_rangetab[i])
		        {
		        status = psf_malloc(sess_cb, &sess_cb->pss_ostream, 
					    sizeof(PST_RNGENTRY),
					    (PTR *)&act_header->pst_rangetab[i], 
					    &psq_cb->psq_error);
		        if (DB_FAILURE_MACRO(status))
			    return (status);
			rngent = act_header->pst_rangetab[i];
		        MEcopy((PTR)prt_header->pst_rangetab[i], 
				sizeof(PST_RNGENTRY), (PTR)rngent);
		        }
		        else
			    act_header->pst_rangetab[i] = (PST_RNGENTRY *) NULL;
		}
	    }

	    /* Allocate and copy statement node */
	    status = psf_malloc(sess_cb, &sess_cb->pss_ostream, sizeof(PST_STATEMENT),
		(PTR *) &act_stmt, &psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    MEcopy((PTR) prt_stmt, sizeof(PST_STATEMENT), (PTR) act_stmt);

	    /* Allocate and copy procedure node */
	    status = psf_malloc(sess_cb, &sess_cb->pss_ostream, sizeof(PST_PROCEDURE),
		(PTR *) &act_pnode, &psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    MEcopy((PTR) prt_pnode, sizeof(PST_PROCEDURE), (PTR) act_pnode);

	    /* Duplicate the rest of the tree */
	    dup_rb.pss_tree = prt_root;
	    dup_rb.pss_dup  = &act_root;
	    status = pst_treedup(sess_cb, &dup_rb);

	    if (DB_FAILURE_MACRO(status))
		return (status);

	    *pnode = act_pnode;
	    sess_cb->pss_object = (PTR) *pnode;

	    /* Link up the allocated nodes. */
	    act_header->pst_qtree = act_root;
	    act_stmt->pst_specific.pst_tree = act_header;
	    act_pnode->pst_stmts = act_stmt;
	    /*
	    ** Restore rule statement lists too.  This requires patching
	    ** pointers associated with rule statements (IF and CALLPROC).
	    */
	    status = pst_ruledup(sess_cb, &sess_cb->pss_ostream,
		(i4) 0, (char *) NULL, prt_stmt->pst_after_stmt,
		&act_stmt->pst_after_stmt, &psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    status = pst_ruledup(sess_cb, &sess_cb->pss_ostream,
		(i4) 0, (char *) NULL, prt_stmt->pst_statementEndRules,
		&act_stmt->pst_statementEndRules, &psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    /*
	    ** finally, coalesce row-level and statement-level rules; this will
	    ** enable OPF to walk ALL rules without worrying about switching
	    ** between rule lists; OPF would still be able to distinguish
	    ** statement-level rules from row-level rules because the former
	    ** will also belong to the list pointed to by pst_statementEndRules
	    **
	    ** In most cases this is done in pst_header() but we had to make an
	    ** exception for PREPARE because the rules get copied here and
	    ** coalescing the two lists before copying them would result in
	    ** incorrect rule lists being created.
	    */
	    psy_rl_coalesce(&act_stmt->pst_after_stmt,
		act_stmt->pst_statementEndRules);

	    /* Fix the root in QSF */
	    status = psf_mroot(sess_cb, &sess_cb->pss_ostream, (PTR) act_pnode,
		&psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    /* Duplicate the subtree of updateable columns */
	    dup_rb.pss_tree = proto->pst_updcollst;
	    dup_rb.pss_dup  = updcollst;
	    status = pst_treedup(sess_cb, &dup_rb);

	    if (DB_FAILURE_MACRO(status))
		return (status);

	    if (   proto->pst_mode == PSQ_DELCURS
		|| proto->pst_mode == PSQ_REPCURS)
	    {
		/*
		** Look up the cursor based on the full internal cursor id.  If
		** we just reused the dynamic name we may end up with a newly
		** opened cursor with the same user name. Return error if not
		** found.
		*/
		status = psq_crfind(sess_cb, &proto->pst_cursid, &csr,
				    &psq_cb->psq_error);
		if (status != E_DB_OK)
		    return(status);
		if (csr == NULL || csr->psc_open == FALSE)
		{
		    (VOID) psf_error(2310L, 0L, PSF_USERERR,
			&err_code, &psq_cb->psq_error, 2,
			STtrmwhite(sname), sname,
			psf_trmwhite(DB_MAXNAME, proto->pst_cursid.db_cur_name),
			proto->pst_cursid.db_cur_name);
		    return (E_DB_ERROR);
		}
		STRUCT_ASSIGN_MACRO(proto->pst_cursid, psq_cb->psq_cursid);
	    }

	    break;
	}

	case PSQ_EVREGISTER:	/* register event */
	case PSQ_EVDEREG:	/* remove event */
	case PSQ_EVRAISE:	/* raise event */
	{
	    PSS_DUPRB		dup_rb;

	    /*
	    **  Create a new QSF object for the query header and tree.
	    **  Duplicate them from the prototype.  At the completion
	    **  of execution, this new object is destroyed by SCF.
	    */
	    /* Set up pointers to the prototype tree */
	    prt_pnode = (PST_PROCEDURE *)qsf_rb.qsf_root;
	    prt_stmt = prt_pnode->pst_stmts;
	    prt_root = prt_stmt->pst_specific.pst_eventstmt.pst_evvalue;

	    /* Open and allocate QSF memory for the procedure */
	    status = psf_mopen(sess_cb, QSO_QTREE_OBJ, &sess_cb->pss_ostream,
			       &psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    /* Allocate and copy statement node (includes event header) */
	    status = psf_malloc(sess_cb, &sess_cb->pss_ostream, sizeof(PST_STATEMENT),
				(PTR *) &act_stmt, &psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);
	    MEcopy((PTR) prt_stmt, sizeof(PST_STATEMENT), (PTR) act_stmt);

	    /* Allocate and copy procedure node */
	    status = psf_malloc(sess_cb, &sess_cb->pss_ostream, sizeof(PST_PROCEDURE),
				(PTR *) &act_pnode, &psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);
	    MEcopy((PTR) prt_pnode, sizeof(PST_PROCEDURE), (PTR) act_pnode);

	    /* initialize fields inside the dup_rb */
	    dup_rb.pss_op_mask = 0;
	    dup_rb.pss_num_joins = PST_NOJOIN;
	    dup_rb.pss_tree_info = (i4 *) NULL;
	    dup_rb.pss_mstream = &sess_cb->pss_ostream;
	    dup_rb.pss_err_blk = &psq_cb->psq_error;

	    /* Duplicate the event value clause (if not NULL) */
	    dup_rb.pss_tree = prt_root;
	    dup_rb.pss_dup  = &act_root;
	    status = pst_treedup(sess_cb, &dup_rb);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    /* Link up the allocated nodes */
	    act_stmt->pst_specific.pst_eventstmt.pst_evvalue = act_root;
	    act_pnode->pst_stmts = act_stmt;
	    sess_cb->pss_object = (PTR)act_pnode;	/* Available outside */
	    /* Fix the root in QSF */
	    status = psf_mroot(sess_cb, &sess_cb->pss_ostream, (PTR) act_pnode,
			       &psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);
	    break;
	}

	case PSQ_DISTCREATE:	/* STAR create table */
	case PSQ_DGTT:		/* declare global temporary table */
	case PSQ_REG_IMPORT:    /* register table */
	case PSQ_INDEX:		/* create index */
	case PSQ_REG_INDEX:     /* register index */
	case PSQ_MODIFY:	/* modify table */
	case PSQ_RELOCATE:	/* relocate table */
	case PSQ_SAVE:		/* save */
	/*
	**  Only the QEU_CB is recreated here.  The original DMU_CB and its
	**  various data blocks are linked to the new QEU_CB.  At the 
	**  completion of query execution, the QEU_CB is destroyed,  the
	**  DMU_CB and its data blocks are not because they are on the 
	**  prototype memory stream.
	*/
	{
	    QEU_CB	    *prt_cb, *act_cb;

	    /* Set up pointer to the prototype control block */

	    prt_cb = (QEU_CB *) qsf_rb.qsf_root;

	    /* Open and allocate QSF memory for the actual control block */

	    if ((status = psf_mopen(sess_cb, QSO_QP_OBJ, &sess_cb->pss_ostream,
			&psq_cb->psq_error)) != E_DB_OK)
		return (status);

	    if ((status = psf_malloc(sess_cb, &sess_cb->pss_ostream, sizeof(QEU_CB),
			&sess_cb->pss_object, &psq_cb->psq_error)) != E_DB_OK)
		return (status);

	    act_cb = (QEU_CB *) sess_cb->pss_object;

	    /* Fix the root in QSF */

	    if ((status = psf_mroot(sess_cb, &sess_cb->pss_ostream, sess_cb->pss_object,
			&psq_cb->psq_error)) != E_DB_OK)
		return (status);
	    
	    /* Copy data from the prototype control block */

	    MEcopy((PTR) prt_cb, sizeof(QEU_CB), (PTR) act_cb);

	    break;
	}

	case PSQ_VIEW:
	{
	    /*
	    ** CREATE VIEW is represented by a PST_CREATE_VIEW statement node
	    */
	    PST_CREATE_VIEW	    *prt_crt_view, *act_crt_view;
	    QEUQ_CB		    *prt_qeuq_cb, *act_qeuq_cb;
	    DMU_CB		    *prt_dmu_cb, *act_dmu_cb;
	    DB_TEXT_STRING	    *prt_text, *act_text;
	    PSS_DUPRB		    dup_rb;

	    /* initialize fields inside the dup_rb */
	    dup_rb.pss_op_mask = 0;
	    dup_rb.pss_num_joins = PST_NOJOIN;
	    dup_rb.pss_tree_info = (i4 *) NULL;
	    dup_rb.pss_mstream = &sess_cb->pss_ostream;
	    dup_rb.pss_err_blk = &psq_cb->psq_error;
	    /*
	    ** Create a new QSF object for PST_PROCEDURE, PST_STATEMENT, the
	    ** query header and tree.  Duplicate them from the prototype.  At
	    ** the completion of execution, this new object is destroyed by SCF.
	    */
	    
	    /* Set up pointers to the prototype tree */

	    prt_pnode = (PST_PROCEDURE *) qsf_rb.qsf_root;
	    prt_stmt = prt_pnode->pst_stmts;
	    prt_crt_view = &prt_stmt->pst_specific.pst_create_view;
	    prt_qeuq_cb = (QEUQ_CB *) prt_crt_view->pst_qeuqcb;
	    prt_dmu_cb = (DMU_CB *) prt_qeuq_cb->qeuq_dmf_cb;
	    prt_text = prt_crt_view->pst_view_text;
	    prt_header = prt_crt_view->pst_view_tree;
	    prt_root = prt_header->pst_qtree;

	    /* Open and allocate QSF memory for the actual tree */
	    status = psf_mopen(sess_cb, QSO_QTREE_OBJ, &sess_cb->pss_ostream,
		&psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    /* Allocate and copy tree header */
	    status = psf_malloc(sess_cb, &sess_cb->pss_ostream, sizeof(PST_QTREE),
		(PTR *) &act_header, &psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    STRUCT_ASSIGN_MACRO((*prt_header), (*act_header));

	    /* allocate space for query text and copy it */
	    status = psf_malloc(sess_cb, &sess_cb->pss_ostream,
		sizeof(DB_TEXT_STRING) - sizeof(prt_text->db_t_text) +
		    prt_text->db_t_count,
		(PTR *) &act_text, &psq_cb->psq_error);

	    if (DB_FAILURE_MACRO(status))
		return (status);

	    act_text->db_t_count = prt_text->db_t_count;
	    MEcopy((PTR) prt_text->db_t_text,
		prt_text->db_t_count,
		(PTR) act_text->db_t_text);

	    /* Allocate and copy statement node */
	    status = psf_malloc(sess_cb, &sess_cb->pss_ostream, sizeof(PST_STATEMENT),
		(PTR *) &act_stmt, &psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    STRUCT_ASSIGN_MACRO((*prt_stmt), (*act_stmt));

	    /*
	    ** allocate and copy QEUQ_CB which is a part of
	    ** PST_CREATE_VIEW statement; also copy DMU_CB address of which is
	    ** contained in QEUQ_CB
	    */
	    status = psf_malloc(sess_cb, &sess_cb->pss_ostream, sizeof(QEUQ_CB),
		(PTR *) &act_qeuq_cb, &psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    STRUCT_ASSIGN_MACRO((*prt_qeuq_cb), (*act_qeuq_cb));

	    status = psf_malloc(sess_cb, &sess_cb->pss_ostream, sizeof(DMU_CB),
		(PTR *) &act_dmu_cb, &psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    STRUCT_ASSIGN_MACRO((*prt_dmu_cb), (*act_dmu_cb));
	    
	    /* Allocate and copy procedure node */
	    status = psf_malloc(sess_cb, &sess_cb->pss_ostream, sizeof(PST_PROCEDURE),
		(PTR *) &act_pnode, &psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    STRUCT_ASSIGN_MACRO((*prt_pnode), (*act_pnode));

	    /* Duplicate the rest of the tree */
	    dup_rb.pss_tree = prt_root;
	    dup_rb.pss_dup  = &act_root;
	    status = pst_treedup(sess_cb, &dup_rb);

	    if (DB_FAILURE_MACRO(status))
		return (status);

	    *pnode = act_pnode;
	    sess_cb->pss_object = (PTR) *pnode;

	    /* Link up the allocated nodes. */
	    act_pnode->pst_stmts = act_stmt;

	    act_crt_view = &act_stmt->pst_specific.pst_create_view;
	    act_crt_view->pst_qeuqcb = act_qeuq_cb;
	    act_qeuq_cb->qeuq_dmf_cb = (PTR) act_dmu_cb;
	    act_crt_view->pst_view_text = act_text;
	    act_crt_view->pst_view_tree = act_header;

	    act_header->pst_qtree = act_root;

	    /* Fix the root in QSF */
	    status = psf_mroot(sess_cb, &sess_cb->pss_ostream, (PTR) act_pnode,
		&psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    break;
	}
	
	case PSQ_GRANT:		/* grant */
	case PSQ_REVOKE:	/* revoke on [table]/procedure/dbevent */
	case PSQ_INTEG:		/* create integrity */
	case PSQ_PROT:		/* create permit/grant */
	case PSQ_RULE:		/* create rule */
	case PSQ_GDBPRIV:	/* grant database privileges */
	case PSQ_RDBPRIV:	/* revoke database privileges */
	case PSQ_GOKPRIV:	/* grant  unrestricted database privileges */
	case PSQ_ROKPRIV:	/* revoke unrestricted database privileges */
	case PSQ_CGROUP:	/* create group */
	case PSQ_AGROUP:	/* alter group add */
	case PSQ_DGROUP:	/* alter group drop */
	case PSQ_KGROUP:	/* drop group */
	case PSQ_CAPLID:	/* create role */
	case PSQ_AAPLID:	/* alter role */
	case PSQ_KAPLID:	/* drop role */
	case PSQ_CUSER:		/* create user */
	case PSQ_AUSER:		/* alter user */
	case PSQ_KUSER:		/* drop user */
	case PSQ_CLOC:		/* create location */
	case PSQ_ALOC:		/* alter location */
	case PSQ_KLOC:		/* drop location */
	case PSQ_CALARM:	/* create security_alarm */
	case PSQ_EVENT:		/* create event */
	case PSQ_CPROFILE:	/* create profile */
	case PSQ_DPROFILE:	/* alter profile */
	case PSQ_APROFILE:	/* drop profile */
	/*
	**  The QSF stream for PSY_CB is in proto->pst_ostream.  The
	**  stream for the query header and tree for the subselect
	**  clause is in psy_cb->psy_intree.  Both objects must be
	**  reproduced.
	*/
	{
	    PSY_CB	    *proto_cb, *actual_cb;

	    /*
	    **  Reproduce the PSY_CB.
	    **	Set up pointer to the prototype control block.
	    */

	    proto_cb = (PSY_CB *) qsf_rb.qsf_root;

	    /* Open and allocate QSF memory for the actual control block */

	    if ((status = psf_mopen(sess_cb, QSO_QP_OBJ, &sess_cb->pss_ostream,
			&psq_cb->psq_error)) != E_DB_OK)
		return (status);

	    if ((status = psf_malloc(sess_cb, &sess_cb->pss_ostream, sizeof(PSY_CB),
			&sess_cb->pss_object, &psq_cb->psq_error)) != E_DB_OK)
		return (status);

	    actual_cb = (PSY_CB *) sess_cb->pss_object;

	    /* Fix the root in QSF */

	    if ((status = psf_mroot(sess_cb, &sess_cb->pss_ostream, sess_cb->pss_object,
			&psq_cb->psq_error)) != E_DB_OK)
		return (status);
	    
	    /* Copy data from the prototype control block */

	    MEcopy((PTR) proto_cb, sizeof(PSY_CB), (PTR) actual_cb);

	    /*
	    ** For a grant, revoke, and create/alter/drop role/group
	    ** modify end-of-list pointers for user, table, and column queues.
	    ** Bugfix (neil): These queues can exist for all GRANT's.
	    */
	    if ((proto->pst_mode == PSQ_GRANT 	||
	         proto->pst_mode == PSQ_REVOKE	||
		 proto->pst_mode == PSQ_PROT	||
		 proto->pst_mode == PSQ_GDBPRIV ||
		 proto->pst_mode == PSQ_RDBPRIV ||
		 proto->pst_mode == PSQ_GOKPRIV ||
		 proto->pst_mode == PSQ_ROKPRIV ||
		 proto->pst_mode == PSQ_CGROUP  ||
		 proto->pst_mode == PSQ_AGROUP  ||
		 proto->pst_mode == PSQ_DGROUP  ||
		 proto->pst_mode == PSQ_KGROUP  ||
		 proto->pst_mode == PSQ_CAPLID	||
		 proto->pst_mode == PSQ_AAPLID	||
		 proto->pst_mode == PSQ_KAPLID	||
		 proto->pst_mode == PSQ_CUSER	||
		 proto->pst_mode == PSQ_AUSER	||
		 proto->pst_mode == PSQ_KUSER	||
		 proto->pst_mode == PSQ_CLOC	||
		 proto->pst_mode == PSQ_ALOC	||
		 proto->pst_mode == PSQ_KLOC	||
		 proto->pst_mode == PSQ_CPROFILE||
		 proto->pst_mode == PSQ_APROFILE||
		 proto->pst_mode == PSQ_DPROFILE||
		 proto->pst_mode == PSQ_CALARM
	        )
	       )
	    {
		if (proto_cb->psy_usrq.q_next == &proto_cb->psy_usrq)
		{
		    (VOID) QUinit((QUEUE *) &actual_cb->psy_usrq);
		}
		else
		{
		    actual_cb->psy_usrq.q_next->q_prev = 
			actual_cb->psy_usrq.q_prev->q_next =
			    &actual_cb->psy_usrq;
		}

		if (proto_cb->psy_tblq.q_next == &proto_cb->psy_tblq)
		{
		    (VOID) QUinit((QUEUE *) &actual_cb->psy_tblq);
		}
		else
		{
		    actual_cb->psy_tblq.q_next->q_prev = 
			actual_cb->psy_tblq.q_prev->q_next =
			    &actual_cb->psy_tblq;
		}

		if (proto_cb->psy_colq.q_next == &proto_cb->psy_colq)
		{
		    (VOID) QUinit((QUEUE *) &actual_cb->psy_colq);
		}
		else
		{
		    actual_cb->psy_colq.q_next->q_prev = 
			actual_cb->psy_colq.q_prev->q_next =
			    &actual_cb->psy_colq;
		}

		if (proto_cb->psy_u_colq.q_next == &proto_cb->psy_u_colq)
		{
		    (VOID) QUinit((QUEUE *) &actual_cb->psy_u_colq);
		}
		else
		{
		    actual_cb->psy_u_colq.q_next->q_prev = 
			actual_cb->psy_u_colq.q_prev->q_next =
			    &actual_cb->psy_u_colq;
		}

		if (proto_cb->psy_r_colq.q_next == &proto_cb->psy_r_colq)
		{
		    (VOID) QUinit((QUEUE *) &actual_cb->psy_r_colq);
		}
		else
		{
		    actual_cb->psy_r_colq.q_next->q_prev = 
			actual_cb->psy_r_colq.q_prev->q_next =
			    &actual_cb->psy_r_colq;
		}
	    }

	    /*
	    **  Reproduce the query tree and header.
	    **  Obtain the root and pointer to the prototype tree
	    **	from QSF.  Rules do not necessarily have trees (like PSQ_PROT).
	    **  Bug 40131: Only run the following code if there is a tree:
	    **  integrities always have trees; so, istree isn't set.
	    **  Rules and permits may or may not have trees; so, we need to
	    **  test istree.
	    **  NOTE: if you add a new statement that may have a tree, add it
	    **  here so that the tree gets processed.
	    */
	    if (   proto->pst_mode == PSQ_INTEG
		|| (
		    (proto->pst_mode == PSQ_RULE || proto->pst_mode == PSQ_PROT)
		    && proto_cb->psy_istree
		   )
	       )
	    {
		PSS_DUPRB		dup_rb;

		/* initialize fields inside the dup_rb */
		dup_rb.pss_op_mask = 0;
		dup_rb.pss_num_joins = PST_NOJOIN;
		dup_rb.pss_tree_info = (i4 *) NULL;
		dup_rb.pss_mstream = &sess_cb->pss_cbstream;
		dup_rb.pss_err_blk = &psq_cb->psq_error;

		qsf_rb.qsf_obj_id.qso_handle = proto_cb->psy_intree.qso_handle;
		status = qsf_call(QSO_INFO, &qsf_rb);
		if ((status != E_DB_OK) || (qsf_rb.qsf_root == NULL))
		{
		    (VOID) psf_error(E_PS0D19_QSF_INFO,
			qsf_rb.qsf_error.err_code, PSF_INTERR,
			&err_code, &psq_cb->psq_error, 0);
		    return (E_DB_ERROR);
		}

		/* Set up pointers to the prototype tree */

		prt_pnode = (PST_PROCEDURE *) qsf_rb.qsf_root;
		prt_stmt = prt_pnode->pst_stmts;
		prt_header = prt_stmt->pst_specific.pst_tree;
		prt_root = prt_header->pst_qtree;

		/* Open and allocate QSF memory for the actual tree */
		status = psf_mopen(sess_cb, QSO_QTREE_OBJ, &sess_cb->pss_cbstream,
		    &psq_cb->psq_error);
		if (DB_FAILURE_MACRO(status))
		    return (status);

		/* Allocate and copy tree header */
		status = psf_malloc(sess_cb, &sess_cb->pss_cbstream, sizeof(PST_QTREE),
		    (PTR *) &act_header, &psq_cb->psq_error);
		if (DB_FAILURE_MACRO(status))
		    return (status);

		MEcopy((PTR) prt_header, sizeof(PST_QTREE), (PTR) act_header);

		/* Allocate and copy statement node */
		status = psf_malloc(sess_cb, &sess_cb->pss_cbstream, 
		    sizeof(PST_STATEMENT), (PTR *) &act_stmt, 
		    &psq_cb->psq_error);
		if (DB_FAILURE_MACRO(status))
		    return (status);

		MEcopy((PTR) prt_stmt, sizeof(PST_STATEMENT), (PTR) act_stmt);

		/* Allocate and copy procedure node */
		status = psf_malloc(sess_cb, &sess_cb->pss_cbstream, 
		    sizeof(PST_PROCEDURE), (PTR *) &act_pnode, 
		    &psq_cb->psq_error);
		if (DB_FAILURE_MACRO(status))
		    return (status);

		MEcopy((PTR) prt_pnode, sizeof(PST_PROCEDURE), (PTR) act_pnode);

		/* Duplicate the rest of the tree */
		dup_rb.pss_tree = prt_root;
		dup_rb.pss_dup  = &act_root;
		status = pst_treedup(sess_cb, &dup_rb);

		if (DB_FAILURE_MACRO(status))
		    return (status);

		*pnode = act_pnode;

		/* Link up the allocated nodes. */
		act_header->pst_qtree = act_root;
		act_stmt->pst_specific.pst_tree = act_header;
		act_pnode->pst_stmts = act_stmt;

		/* Fix the root in QSF */
		status = psf_mroot(sess_cb, &sess_cb->pss_cbstream, (PTR) act_pnode,
		    &psq_cb->psq_error);
		if (DB_FAILURE_MACRO(status))
		    return (status);

		/* store tree stream in PSY_CB */
		STRUCT_ASSIGN_MACRO(sess_cb->pss_cbstream.psf_mstream,
			    actual_cb->psy_intree);
	    }
	    else
	    {
		actual_cb->psy_intree.qso_handle = (PTR) NULL;
	    }
	    /*
	    **  Reproduce QSF object for text
	    */
	    if ((qsf_rb.qsf_obj_id.qso_handle = 
			proto_cb->psy_qrytext.qso_handle) != (PTR) NULL)
	    {
		
		status = qsf_call(QSO_INFO, &qsf_rb);
		if ((status != E_DB_OK) || (qsf_rb.qsf_root == NULL))
		{
		    (VOID) psf_error(E_PS0D19_QSF_INFO,
			qsf_rb.qsf_error.err_code, PSF_INTERR,
			&err_code, &psq_cb->psq_error, 0);
		    return (E_DB_ERROR);
		}

		prt_text = (char *) qsf_rb.qsf_root;
		MEcopy((PTR)prt_text, sizeof(i4), (PTR)&text_length);
		text_length += sizeof(i4);

		/* Open and allocate QSF memory for the actual text object */

		if ((status = psf_mopen(sess_cb, QSO_QTEXT_OBJ, &sess_cb->pss_tstream, 
			    &psq_cb->psq_error)) != E_DB_OK)
		    return (status);
		
		if ((status = psf_malloc(sess_cb, &sess_cb->pss_tstream, text_length,
			    (PTR *) &act_text, &psq_cb->psq_error)) != E_DB_OK)
		    return (status);

		/* Fix the root in QSF */

		if ((status = psf_mroot(sess_cb, &sess_cb->pss_tstream, act_text,
			    &psq_cb->psq_error)) != E_DB_OK)
		    return (status);

		/* store text stream in PSY_CB */

		STRUCT_ASSIGN_MACRO(sess_cb->pss_tstream.psf_mstream,
				actual_cb->psy_qrytext);

		/* Copy data from the prototype query text object */

		MEcopy(prt_text, text_length, act_text);

		/* Unlock the text object */

		STRUCT_ASSIGN_MACRO(sess_cb->pss_tstream.psf_mstream, 
				qsf_rb.qsf_obj_id);
		qsf_rb.qsf_lk_id = sess_cb->pss_tstream.psf_mlock;
		status = qsf_call(QSO_UNLOCK, &qsf_rb);
		if (status != E_DB_OK)
		{
		    (VOID) psf_error(E_PS0375_UNLOCK_QSF_TEXT,
			qsf_rb.qsf_error.err_code, PSF_INTERR,
			&err_code, &psq_cb->psq_error, 0);
		    return (status);
		}
	    }
	    
	    break;
	}

	case PSQ_SET_SESS_AUTH_ID:
	{
	    /* copy new effective user name (if one was specified) */
	    STRUCT_ASSIGN_MACRO(proto->pst_user, psq_cb->psq_user);

	    break;
	}
	    
	case PSQ_CREATE:
	/*
	** Local (as opposed to STAR) CREATE TABLE statement is represented
	** by a PST_PROCEDURE tree consisting of one PST_CREATE_TABLE
	** statement and 0 or more PST_CREATE_INTEGRITY and/or PST_EXEC_IMM
	** statement
	** I will simply copy the top PST_PROCEDURE node - recepient of this
	** tree (OPF) will make no changes to it, so there is hardly a reason to
	** go through the painful exercise of copying every statement node and
	** all the structures hanging off of it
	*/
	    
	case PSQ_ALTERTABLE:
	/*
	** ALTER TABLE ADD/DROP CONSTRAINT is represented by a PST_PROCEDURE
	** node with at least one PST_STATEMENT node attached.
	** Unless there is a reason to do something differently for ADD
	** CONSTRAINT, I will simply copy the PST_PROCEDURE node here
	*/
	{
	    if ((status = psf_mopen(sess_cb, QSO_QP_OBJ, &sess_cb->pss_ostream,
		    &psq_cb->psq_error)) != E_DB_OK)
	    return (status);

	    prt_pnode = (PST_PROCEDURE *) qsf_rb.qsf_root;
	    status = psf_malloc(sess_cb, &sess_cb->pss_ostream, sizeof(PST_PROCEDURE),
		(PTR *) &act_pnode, &psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    STRUCT_ASSIGN_MACRO((*prt_pnode), (*act_pnode));

	    /* Fix the root in QSF */
	    status = psf_mroot(sess_cb, &sess_cb->pss_ostream, (PTR) act_pnode,
		&psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);
		
	    break;
	}
	    
	default:
	{
	    (VOID) psf_error(2302L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 2, sizeof(sess_cb->pss_lineno),
		&sess_cb->pss_lineno, STtrmwhite(sname), sname);

	    return (E_DB_ERROR);
	}
    }

    /* Restore session and query information from the prototype header */

    psq_cb->psq_mode = proto->pst_mode;		/* query mode */
    if (proto->pst_alldelupd)
	psq_cb->psq_flag |= PSQ_ALLDELUPD;
    else
	psq_cb->psq_flag &= ~PSQ_ALLDELUPD;

    if (proto->pst_clsall)
	psq_cb->psq_flag |= PSQ_CLSALL;
    else
	psq_cb->psq_flag &= ~PSQ_CLSALL;

    /* Restore the updatability flag. */
    *nonupdt = proto->pst_nupdt;

    /* restore the statement flag field */
    sess_cb->pss_stmt_flags |= proto->pst_stmt_flags;

    /* 
    ** restore the flag field containing info used to determine whether the 
    ** query will be flattened 
    */
    sess_cb->pss_flattening_flags = proto->pst_flattening_flags;

    psq_cb->psq_ret_flag = proto->pst_ret_flag;

    psq_cb->psq_error.err_code = E_PS0000_OK;
    psq_cb->psq_error.err_data = E_PS0000_OK;
    return (status);
}


/*{
** Name: pst_prmsub	- Routine to substitue constant nodes with actual
**			  parameters.
**
** Description:
**      This routine traverses a query tree, beginning from the current
**	node, toward the right and left descendents.  All parameter nodes
**	found are substituted with actual values from the input parameter
**      list.  If the current node is an operator node, the data type is
**	also resolved after its descendent nodes are substituted.
**
** Inputs:
**      psq_cb                          ptr to the parser control block.
**      sess_cb                         ptr to PSF session control block.
**      node                            ptr to the current node.
**      plist                           ptr to array of pointers to 
**					DB_DATA_VALUE's for the parameters
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
**  History:
**      11-apr-87 (puree)
**          initial version.
**	07-sep-88 (stec)
**	    For EXISTS pst_resolve must not be called, follow code in pstnode.c
**	24-feb-89 (andre)
**	    While parsing parameterized prepare, pst_resolve() is not called.
**	    As a result, datatype of nodes would not be known.  Since during
**	    parsing OPEN CURSOR we call pst_resolve(), datatype info for
**	    children node becomes available and must be propagated to their
**	    parents.
**	20-sep-91 (rog)
**	    Finish the previous fix: if the child of the node we're trying
**	    to resolve is a BYHEAD, we have to go to the BYHEAD's right
**	    child to get the datatype.  (bug 39530)
**	24-mar-92 (andre)
**	    fix bug 43239.  If processing a PST_ROOT node, check pst_next in
**	    case the query involved UNION.
**	10-jul-93 (andre)
**	    cast 1st and 3rd args to MEcopy() to agree with the prototype
**	    declaration
**      08-apr-1997 (i4jo01)
**	    The precision value for decimal datatypes was not being      
**	    propagated to higher level nodes, causing problems during 
**	    resolution of resdoms later on.
*/
DB_STATUS
pst_prmsub(
	PSQ_CB		    *psq_cb,
	PSS_SESBLK	    *sess_cb,
	PST_QNODE	    *node,
	DB_DATA_VALUE	    *plist[])
{
    DB_STATUS		status;
    i4			parm_no;
    ADI_DT_BITMASK	typeset;
    DB_DT_ID		db_right;
    DB_DT_ID		adb_right;
    DB_DT_ID		adatatype;
    DB_ERROR		err_code;

    /*
    ** If the current node is a constant node and its param number
    ** is non-zero,  put the corresponding value from the input
    ** parameter list in pst_dataval and zero out the param number.
    */
    if (node->pst_sym.pst_type == PST_CONST &&
	(parm_no = node->pst_sym.pst_value.pst_s_cnst.pst_parm_no) > 0)
    {
	--parm_no;
	MEcopy((PTR) plist[parm_no], sizeof(DB_DATA_VALUE),
		(PTR) &node->pst_sym.pst_dataval);

	if ((status = psf_malloc(sess_cb, &sess_cb->pss_ostream,
		    (i4)(plist[parm_no]->db_length),
		    &node->pst_sym.pst_dataval.db_data,
		    &psq_cb->psq_error)) != E_DB_OK)
	    return (status);

	MEcopy(plist[parm_no]->db_data, plist[parm_no]->db_length,
		node->pst_sym.pst_dataval.db_data);

	node->pst_sym.pst_value.pst_s_cnst.pst_parm_no = 0;
    }
    /*
    ** If there is the right child, recursively process it.
    */

    if (node->pst_right)
    {
	if ((status = pst_prmsub(psq_cb, sess_cb, node->pst_right, plist))
			    != E_DB_OK)
	    return (status);
    }

    /*
    ** If there is the left child, recursively process it.
    */

    if (node->pst_left)
    {
	if ((status = pst_prmsub(psq_cb, sess_cb, node->pst_left, plist))
			    != E_DB_OK)
	    return (status);
    }

    /*
    ** If the node is AGHEAD, and we are parsing OPEN CURSOR for parameterized
    ** PREPARE, AGHEAD's type would not be set because during the parsing of
    ** PREPARE pst_resolve() would not be invoked on the AOP, so the type
    ** would stay 0.  At this point, however, pst_resolve() has been invoked,
    ** so we may propagate type and length of the result data value.
    **     Similarly, if the node is SUBSEL, and we are parsing OPEN CURSOR
    ** for parameterized PREPARE, SUBSEL's type would not have been set
    ** properly if its left child is not a simple RESDOM (e.g. AGHEAD).
    ** By this time, the type of SUBSEL's left child would be known, so copy
    ** it into SUBSEL.
    ** Bug 39530: If the left child is a BYHEAD, we have to go farther down
    ** the tree to the BYHEAD's right child to get the type and length.
    */
    
    switch (node->pst_sym.pst_type)
    {
	case PST_AGHEAD:
	case PST_SUBSEL:
	{
	    if (node->pst_sym.pst_dataval.db_datatype == 0)
	    {
		DB_DATA_VALUE	*src_dbval;

		if (node->pst_left->pst_sym.pst_dataval.db_datatype != 0)
		{
		    src_dbval = &node->pst_left->pst_sym.pst_dataval;
		}
		else
		{
		    src_dbval = &node->pst_left->pst_right->pst_sym.pst_dataval;
		}

		node->pst_sym.pst_dataval.db_datatype = src_dbval->db_datatype;
		node->pst_sym.pst_dataval.db_length = src_dbval->db_length;
		node->pst_sym.pst_dataval.db_prec = src_dbval->db_prec;
	    }
	    
	    break;
	}
	case PST_ROOT:
	{
	    if (node->pst_sym.pst_value.pst_s_root.pst_union.pst_next)
	    {
		if ((status = pst_prmsub(psq_cb, sess_cb,
			node->pst_sym.pst_value.pst_s_root.pst_union.pst_next,
			plist)) != E_DB_OK)
		    return (status);
	    }

	    break;
	}
	case PST_UOP:
	{
	    /* EXISTS needs to be handled in a special way. */
	    if (node->pst_sym.pst_value.pst_s_op.pst_opno == ADI_EXIST_OP)
	    {
		node->pst_sym.pst_dataval.db_datatype = DB_BOO_TYPE;
		node->pst_sym.pst_dataval.db_prec = 0;
		node->pst_sym.pst_dataval.db_length = sizeof(bool);
		node->pst_sym.pst_value.pst_s_op.pst_opinst =
		    (ADI_FI_ID) ADI_NOFI;
		node->pst_sym.pst_value.pst_s_op.pst_fdesc =
		    (ADI_FI_DESC *) NULL;

		break;
	    }
	    /* otherwise fall through to code below */
	}
	    
	case PST_AOP:
	case PST_BOP:
	case PST_COP:
	case PST_MOP:
	{
	    /*
	    ** If the current node is an operator node, resolve data type
	    */

	    status = pst_resolve(sess_cb, (ADF_CB *) sess_cb->pss_adfcb, node,
			sess_cb->pss_lang, &psq_cb->psq_error);

	    if (DB_FAILURE_MACRO(status))
	    {
		return (pst_rserror(sess_cb->pss_lineno, node,
		    &psq_cb->psq_error));
	    }

	    break;
	}

	case PST_RESDOM:
	{
	    if (node->pst_right != (PST_QNODE *) NULL)
	    {
		/*
		** If the current node is a resdom node, check data type
		** conversion.
		*/

		/*
		** If RESDOM's datatype was not determinable during the parsing
		** of the PREPARE, now is a good time to set RESDOM's datatype
		** and datalen to those of its right child
		*/
		if (node->pst_sym.pst_dataval.db_datatype == 0)
		{
		    node->pst_sym.pst_dataval.db_length =
			node->pst_right->pst_sym.pst_dataval.db_length;
		    node->pst_sym.pst_dataval.db_datatype =
			node->pst_right->pst_sym.pst_dataval.db_datatype;
		    node->pst_sym.pst_dataval.db_prec = 
			node->pst_right->pst_sym.pst_dataval.db_prec;
		}
		else
		{
		    db_right = node->pst_right->pst_sym.pst_dataval.db_datatype;
		    adb_right = abs(db_right);
		    adatatype = abs(node->pst_sym.pst_dataval.db_datatype);
		    if (adatatype != adb_right
		        &&
			(   (status = adi_tycoerce(sess_cb->pss_adfcb,
				          db_right, &typeset))
			 || !BTtest((i4)ADI_DT_MAP_MACRO(adatatype),
			        (char*) &typeset)
			)
		       )
		    {
			/* error 2913 - incompatible type coercion */
			ADI_DT_NAME	    right_name;
			ADI_DT_NAME	    res_name;
		    
			/*
			** Get the names of the left & right operands, if
			** any
			*/ 
			STmove("<none>", ' ', sizeof (ADI_DT_NAME),
			     (char *) &right_name);
			STmove("<none>", ' ', sizeof (ADI_DT_NAME), 
			    (char *) &res_name);

			status = adi_tyname(sess_cb->pss_adfcb, db_right,
			    &right_name);
			status = adi_tyname(sess_cb->pss_adfcb,
			    node->pst_sym.pst_dataval.db_datatype,
			    &res_name);

			/* Now report the error */
			(VOID) psf_error(2913, 0L, PSF_USERERR,
			    &err_code, &psq_cb->psq_error, 3,
			    sizeof(sess_cb->pss_lineno), &sess_cb->pss_lineno, 
			    (i4) psf_trmwhite(sizeof (ADI_DT_NAME),
				(char *) &right_name), 
			    &right_name, 
			    (i4) psf_trmwhite(sizeof (ADI_DT_NAME),
				(char *) &res_name),
			    &res_name);
			return (E_DB_ERROR);
		    }
		}
	    }
	    break;
	}
    }

    return (E_DB_OK);
}

/*{
** Name: pst_describe	- Semantic routine for SQL's DESCRIBE.
**
** Description:
**	This routine determines result tuple description from the prototype
**	tree and format it into sqlda which is subsequently returned in
**	pss_object.
**
** Inputs:
**      psq_cb                          ptr to the parser control block.
**      sess_cb                         ptr to PSF session control block.
**      sname                           ptr to a null terminated statement
**					name (front-end name).
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
**  History:
**      11-apr-87 (puree)
**          initial version.
**	21-jul-1988 (stec)
**	    Make changes related to the new query tree structure.
**	06-nov-2001 (devjo01)
**	    Perform GCA_COL_ATT packing in one pass after calling
**	    pst_sqlatt.   This allows pst_sqlatt to work with
**	    aligned GCA_COL_ATT structs, addressing a problem in
**	    which the Tru64 optimizer outsmarts itself, and replaces
**	    the mem copies used to prevent alignment access problems
**	    with aligned load and stores.
**	    
*/
DB_STATUS
pst_describe(
	PSQ_CB		    *psq_cb,
	PSS_SESBLK	    *sess_cb,
	char		    *sname)
{

    register PST_PROTO    *proto;
    DB_STATUS		  status;
    PST_PROCEDURE	  *pnode;
    PST_QNODE		  *root;
    PST_QNODE		  *resdom;
    GCA_TD_DATA		  *sqlda;
    i4		  err_code;
    i4			  da_size;
    i4			  da_num;
    QSF_RCB		  qsf_rb;

    /* set up QSF control block */

    qsf_rb.qsf_type = QSFRB_CB;
    qsf_rb.qsf_ascii_id = QSFRB_ASCII_ID;
    qsf_rb.qsf_length = sizeof(qsf_rb);
    qsf_rb.qsf_owner = (PTR)DB_PSF_ID;
    qsf_rb.qsf_sid = sess_cb->pss_sessid;
    qsf_rb.qsf_obj_id.qso_lname = 0;

    /* Search the prototype header list for the requested statement */

    proto = sess_cb->pss_proto;	/* get head of chain from session cb */

    while (proto != (PST_PROTO *) NULL)
    {
	if (STcompare(sname, proto->pst_sname) == 0)
	    break;
	proto = proto->pst_next;
    }

    /* Error, if not found */

    if (proto == (PST_PROTO *) NULL)
    {
	(VOID) psf_error(2304L, 0L, PSF_USERERR,
	    &err_code, &psq_cb->psq_error, 2, sizeof(sess_cb->pss_lineno),
	    &sess_cb->pss_lineno, STtrmwhite(sname), sname);
	return (E_DB_ERROR);
    }

    /* Obtain the root of the object from QSF. */

    qsf_rb.qsf_obj_id.qso_handle = 
		    proto->pst_ostream.psf_mstream.qso_handle;

    status = qsf_call(QSO_INFO, &qsf_rb);
    if ((status != E_DB_OK) || (qsf_rb.qsf_root == NULL))
    {
	(VOID) psf_error(E_PS0D19_QSF_INFO, qsf_rb.qsf_error.err_code,
	    PSF_INTERR, &err_code, &psq_cb->psq_error, 0);
	return (E_DB_ERROR);
    }
    /*
    ** Compute the size and allocate QSF memory for GCA_TD_DATA.
    */
    da_num = 0;

    if (proto->pst_mode == PSQ_RETRIEVE)
    {
	/* Set up pointers to the prototype tree */

	pnode = (PST_PROCEDURE *) qsf_rb.qsf_root;
	root = pnode->pst_stmts->pst_specific.pst_tree->pst_qtree;
    
	for (resdom = root->pst_left; resdom->pst_sym.pst_type == PST_RESDOM; 
		resdom = resdom->pst_left)
	{
	    if (resdom->pst_sym.pst_value.pst_s_rsdm.pst_rsflags&PST_RS_PRINT)
		da_num += 1;
	}
    }

    da_size = sizeof(GCA_TD_DATA) + (da_num * GCA_COL_ATT_SIZE);

    if ((status = psf_mopen(sess_cb, QSO_SQLDA_OBJ, &sess_cb->pss_ostream,
		&psq_cb->psq_error)) != E_DB_OK)
	return (status);

    if ((status = psf_malloc(sess_cb, &sess_cb->pss_ostream, da_size, 
		&sess_cb->pss_object, &psq_cb->psq_error)) != E_DB_OK)
	return (status);

    sqlda = (GCA_TD_DATA *) sess_cb->pss_object;

    if ((status = psf_mroot(sess_cb, &sess_cb->pss_ostream, sess_cb->pss_object,
		&psq_cb->psq_error)) != E_DB_OK)
	return (status);

    sqlda->gca_tsize = 0;
    sqlda->gca_result_modifier = GCA_NAMES_MASK;
    sqlda->gca_l_col_desc = da_num;

    if (proto->pst_mode == PSQ_RETRIEVE)
    {
	char	*sp, *dp;
	i4	cpyamt;

	pst_sqlatt(root->pst_left, sqlda->gca_col_desc,
					&sqlda->gca_tsize);
	
	/*
	** Pack GCA_COL_ATT array so that each structure lies
	** directly after the actual characters used for the
	** gca_attname field of the structure before, ignoring
	** any alignment considerations.  (Yuck).
	*/
	for ( sp = dp = (char*)sqlda->gca_col_desc;
	      da_num-- > 0;
	      sp += GCA_COL_ATT_SIZE )
	{
	    cpyamt = (((GCA_COL_ATT*)0)->gca_attname - (char*)0) +
	      ((GCA_COL_ATT*)sp)->gca_l_attname;
	    if ( sp != dp )
		MEcopy(sp, cpyamt, dp);
	    dp += cpyamt;
	}
    }

    psq_cb->psq_error.err_code = E_PS0000_OK;
    psq_cb->psq_error.err_data = E_PS0000_OK;
    return (status);
}


/*{
** Name: pst_commit_dsql    - Destroy all prototypes at commit time.
**
** Description:
**	This routine is called at commit time.  Its primary function
**	is to clean up all statement prototypes and their headers.
**	All memory allocated are returned.
**
** Inputs:
**      psq_cb                          ptr to the parser control block.
**      sess_cb                         ptr to PSF session control block.
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
**  History:
**      11-apr-87 (puree)
**          initial version.
*/
DB_STATUS
pst_commit_dsql(
	PSQ_CB		    *psq_cb,
	PSS_SESBLK	    *sess_cb)
{
    register PST_PROTO    *proto;
    DB_STATUS		  status;
    ULM_RCB		  ulm_rcb;
    i4		  err_code;


    /* Scan the prototype header list */

    proto = sess_cb->pss_proto;	    /* get head of chain from session cb */

    if (proto == (PST_PROTO *) NULL)	/* we don't have a list yet */
    {
	return (E_DB_OK);
    }
    /*
    ** Delete the associated object(s) from QSF.
    */
    while (proto != (PST_PROTO *) NULL)
    {
	if ((status = pst_dstobj(psq_cb, sess_cb, proto)) != E_DB_OK)
	    return (status);
    
	proto = proto->pst_next;
    }

    /* Close memory stream for the prototype header */

    ulm_rcb.ulm_facility = DB_PSF_ID;
    ulm_rcb.ulm_poolid = Psf_srvblk->psf_poolid;
    ulm_rcb.ulm_blocksize = sizeof(PST_PROTO);
    ulm_rcb.ulm_memleft = &sess_cb->pss_memleft;
    ulm_rcb.ulm_streamid_p = &sess_cb->pss_pstream;
    if ((status = ulm_closestream(&ulm_rcb)) != E_DB_OK)
    {
	if (ulm_rcb.ulm_error.err_code == E_UL0004_CORRUPT)
	{
	    status = E_DB_FATAL;
	}

	(VOID) psf_error(E_PS0A02_BADALLOC, ulm_rcb.ulm_error.err_code,
		PSF_INTERR, &err_code, &psq_cb->psq_error, 0);

	return (status);
    }

    sess_cb->pss_proto = (PST_PROTO *) NULL;
    /* ULM has nullified pss_pstream */
    return (status);
}



/*{
** Name: PST_SQLATT	- Fill in an GCA_COL_ATT array for a GCA_TD_DATA
**
** Description:
**      This internal routine recursively traverses the result domain
**	nodes of a query tree and extracts information to build the 
**	sqlda.
**
** Inputs:
**      resdom                          ptr to the current resdom node
**      sqlatts                         ptr to the current GCA_COL_ATT
**      tup_size                        ptr to tuple size accumulator
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
**      3-apr-87 (eric)
**          written
**	6-apr-87 (puree)
**	    modified for PSF.
**	02-oct-1987 (fred)
**	    modified for GCF.
**	16-apr-1990 (nancy)
**	    fix bug 21097.  Replace STRUCT_ASSIGN_MACRO with MEcopy for
**	    dataval, because might not be aligned.
**	06-nov-2001 (devjo01)
**	    Cannot rely on MEcopy to address alignment issues, since
**	    on Tru64, compiler is now "smart" enough to inline memcpy
**	    to simple load and stores, if it thinks source and target
**	    addresses are aligned.   Simplify this routine to act on
**	    aligned GCA_COL_ATT structures, making caller responsible
**	    for packing array in one pass.
**	21-Dec-2001 (gordy)
**	    GCA_COL_ATT no longer defined with DB_DATA_VALUE: use DB_COPY macro.
*/
static GCA_COL_ATT *
pst_sqlatt(
	PST_QNODE	*resdom,
	GCA_COL_ATT 	*sqlatts,
	i4		*tup_size)
{
    if (resdom->pst_left != NULL &&
	    resdom->pst_left->pst_sym.pst_type == PST_RESDOM
	)
    {
	sqlatts = pst_sqlatt(resdom->pst_left, sqlatts, tup_size);
    }
    else
    {
	*tup_size = 0;
    }

    /* fill in its DB_DATA_VALUE; */

    if (resdom->pst_sym.pst_value.pst_s_rsdm.pst_rsflags&PST_RS_PRINT)
    {
	DB_COPY_DV_TO_ATT( &resdom->pst_sym.pst_dataval, sqlatts );

	MEcopy(resdom->pst_sym.pst_value.pst_s_rsdm.pst_rsname, 
				DB_MAXNAME, sqlatts->gca_attname);
	sqlatts->gca_l_attname = STtrmnwhite(sqlatts->gca_attname, DB_MAXNAME);

	sqlatts = (GCA_COL_ATT*)(((char *) sqlatts) + GCA_COL_ATT_SIZE);

	/* add its size to the tuple size; */

	*tup_size += resdom->pst_sym.pst_dataval.db_length;
    }

    return (sqlatts);
}



/*{
** Name: PST_DSTOBJ	- Destroy prototype QSF objects.
**
** Description:
**      This internal routine destroys all QSF objects associated with
**	a dynamic SQL prototype.  
**
** Inputs:
**      psq_cb                          ptr to the parser control block.
**      sess_cb                         ptr to PSF session control block.
**      proto				ptr to the prototype header
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
**	6-apr-87 (puree)
**	    initial version.
**	10-jan-97 (pchang)
**	    Fixed typo - set pst_tstream rather than pst_cbstream qso_handle
**	    to NULL in the last case.
*/
static DB_STATUS
pst_dstobj(
	PSQ_CB		    *psq_cb,
	PSS_SESBLK	    *sess_cb,
	PST_PROTO	    *proto)
{
    DB_STATUS		  status;

    
    if (proto->pst_ostream.psf_mstream.qso_handle != (PTR) NULL)
    {
	if ((status = psf_mclose(sess_cb, &proto->pst_ostream,
			    &psq_cb->psq_error)) != E_DB_OK)
		return (status);
	proto->pst_ostream.psf_mstream.qso_handle = (PTR) NULL;
    }

    if (proto->pst_cbstream.psf_mstream.qso_handle != (PTR) NULL)
    {
	if ((status = psf_mclose(sess_cb, &proto->pst_cbstream,
			    &psq_cb->psq_error)) != E_DB_OK)
		return (status);
	proto->pst_cbstream.psf_mstream.qso_handle = (PTR) NULL;
    }

    if (proto->pst_tstream.psf_mstream.qso_handle != (PTR) NULL)
    {
	if ((status = psf_mclose(sess_cb, &proto->pst_tstream,
			    &psq_cb->psq_error)) != E_DB_OK)
		return (status);

	proto->pst_tstream.psf_mstream.qso_handle = (PTR) NULL;
    }

    return (E_DB_OK);
}



/*{
** Name: PST_GET_STMT_MODE	- Return information about prepared insert stmt
**
** Description:
**      Return information about prepared insert statement
**
** Inputs:
**      sess_cb                         ptr to PSF session control block.
**      sname				statement name
**
** Outputs:
**      stmt_info			Return ptr to PSQ_STMT_INFO in PST_PROTO
**	Returns:
**	    statement mode PSQ_APPEND or zero
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**     24-jul-2003 (stial01)
**          written for b110620
*/
i4
pst_get_stmt_mode(
	PSS_SESBLK         *sess_cb,
	char		   *sname,
	PSQ_STMT_INFO	   **stmt_info)
{
    PST_PROTO		*proto;
    i4			pst_mode = 0;

    *stmt_info = NULL;
    proto = sess_cb->pss_proto;	    /* get head of chain from session cb */

    if (proto == (PST_PROTO *) NULL)	/* we don't have a list yet */
    {
	return (0);
    }

    while (proto != (PST_PROTO *) NULL)
    {
	if (sname)
	{
	    if (STcompare(sname, proto->pst_sname) == 0)
	    {
		pst_mode = proto->pst_mode;
		*stmt_info = &proto->pst_stmt_info;
#ifdef xDEBUG
		TRdisplay("pst_get_stmt_mode %x %s mode %d blob_cnt %d colno %d\n", 
			*stmt_info, sname, proto->pst_mode, 
			proto->pst_stmt_info.psq_stmt_blob_cnt, 
			proto->pst_stmt_info.psq_stmt_blob_colno);
#endif
		break;
	    }
	}
	else
	{
	    /* just TRdisplay statement names */
	    if (proto->pst_mode == PSQ_APPEND || proto->pst_mode == PSQ_REPCURS)
		TRdisplay("Statement %s: mode %d %s %s\n", 
			proto->pst_sname, proto->pst_mode, 
			proto->pst_stmt_info.psq_stmt_tabname, 
			proto->pst_stmt_info.psq_stmt_ownname);
	    else
		TRdisplay("Statement %s: mode %d\n",
			proto->pst_sname, proto->pst_mode);
	}
	proto = proto->pst_next;
    }

    return (pst_mode);
}

/*
** NAME: pst_descinput -- DESCRIBE INPUT statement
**
** Description:
**
**	The DESCRIBE INPUT statement describes the input parameters
**	of a prepared SQL statement.
**
**	That description sounds plausible enough, except that massive
**	ambiguity is hidden therein.  The problem of course is that SQL
**	is not a strongly typed language, and input parameters may
**	typically be of a variety of types.  At execution time, when
**	the query tree is type-resolved, coercions may be applied to
**	the user parameter values to produce a valid query.  What then
**	should we return as the input parameter types?
**
**	The idea that we'll use is that we will attempt to return
**	the current type "context" -- a nebulous enough idea, to be
**	sure.  There really isn't any such thing, but we'll try to
**	match the type of whatever the parameter CONST node is "close"
**	to.  So, if e.g. it's under an ATT RESDOM, this is probably an
**	insert or update, and we can take the table column type from
**	the resdom.  If it's a DB procedure call parameter, we can look
**	at the type of that DB procedure formal.  Beyond that, things
**	get sticky.  If there is some useful type information on the
**	other side of an operator (e.g. col = ?), we can use that.
**	In the worst case, such as INT4(?), the parameter could be just
**	about anything, and we'll return a zero type code.
**
**	The parameter type information is returned in SQLDA form.
**	This allows us to use all the same SQLDA machinery as the
**	DESCRIBE [output] statement uses already.  The SQLDA is built
**	in QSF memory, where the sequencer can get at it and spit it
**	back to the client as a tuple descriptor message.
**
**	Note that the only prepared statements that allow parameter
**	markers are the usual DML statements, plus create-as-select,
**	dgtt-as-select, create view, and set session authorization.
**	All but the last have ordinary qtrees that hold the parameter
**	markers and we depend on that in the code.
**
** Inputs:
**	psq_cb			PSQ_CB * query parse control block
**	sess_cb			PSS_SESBLK * parser session control block
**	sname			Name of the prepared statement
**
** Outputs:
**	sess_cb.pss_object	Holds the QSF handle of the new SQLDA
**	Returns E_DB_OK or error
**
** History:
**	24-May-2006 (kschendel)
**	    Written.
**	28-dec-2006 (dougi)
**	    Add support for with clause elements.
*/

DB_STATUS
pst_descinput(PSQ_CB *psq_cb, PSS_SESBLK *sess_cb, char *sname)
{

    bool have_parm;			/* See tree walker */
    DB_STATUS status;
    GCA_COL_ATT *desc_base;		/* First param descriptor */
    GCA_TD_DATA *sqlda;			/* Start of sqlda area in QSF */
    i4 da_size;				/* Size of sqlda in bytes */
    i4 da_num;				/* Number of parameter markers */
    i4 err_code;
    i4 i;
    PST_PROTO *proto;			/* Prepared statement prototype hdr */
    PST_PROCEDURE *prt_pnode;
    PST_QNODE *prt_root;
    PST_QTREE *prt_qtree;
    PST_RNGENTRY **prt_rangetab;	/* Pointer to range table ptr array */
    PST_STATEMENT *prt_stmt;
    QSF_RCB qsf_rb;

    /* set up common stuff in QSF control block */

    qsf_rb.qsf_type = QSFRB_CB;
    qsf_rb.qsf_ascii_id = QSFRB_ASCII_ID;
    qsf_rb.qsf_length = sizeof(qsf_rb);
    qsf_rb.qsf_owner = (PTR)DB_PSF_ID;
    qsf_rb.qsf_sid = sess_cb->pss_sessid;
    qsf_rb.qsf_obj_id.qso_lname = 0;

    /* Search the prototype header list for the requested statement */

    proto = sess_cb->pss_proto;	/* get head of chain from session cb */

    while (proto != (PST_PROTO *) NULL)
    {
	if (STcompare(sname, proto->pst_sname) == 0)
	    break;
	proto = proto->pst_next;
    }

    /* Error, if not found */

    if (proto == (PST_PROTO *) NULL)
    {
	(VOID) psf_error(2304L, 0L, PSF_USERERR,
	    &err_code, &psq_cb->psq_error, 2, sizeof(sess_cb->pss_lineno),
	    &sess_cb->pss_lineno, STtrmwhite(sname), sname);
	return (E_DB_ERROR);
    }

    /*
    ** Compute the size and allocate QSF memory for GCA_TD_DATA.
    ** One SQLDA entry for each user parameter.  "maxparm" is max
    ** index, not number of parameter markers.
    ** (Note: this is bigger than needed, there's a stub GCA_COL_ATT in the
    ** header, but it's inconvenient to subtract that out.)
    */
    da_num = proto->pst_maxparm + 1;
    da_size = sizeof(GCA_TD_DATA) + da_num * GCA_COL_ATT_SIZE;

    if ((status = psf_mopen(sess_cb, QSO_SQLDA_OBJ, &sess_cb->pss_ostream,
		&psq_cb->psq_error)) != E_DB_OK)
	return (status);

    if ((status = psf_malloc(sess_cb, &sess_cb->pss_ostream, da_size, 
		&sess_cb->pss_object, &psq_cb->psq_error)) != E_DB_OK)
	goto error_exit;

    sqlda = (GCA_TD_DATA *) sess_cb->pss_object;

    /* Make the sqlda object its own QSF object root. */
    if ((status = psf_mroot(sess_cb, &sess_cb->pss_ostream, sess_cb->pss_object,
		&psq_cb->psq_error)) != E_DB_OK)
	goto error_exit;

    MEfill(da_size, 0, (PTR) sqlda);	/* Zero the whole thing */
    sqlda->gca_l_col_desc = da_num;
    psq_cb->psq_error.err_code = E_PS0000_OK;
    psq_cb->psq_error.err_data = E_PS0000_OK;

    /* If nothing there, just return the empty header. */
    if (da_num == 0)
	return (E_DB_OK);

    desc_base = &sqlda->gca_col_desc[0];

    /* Before we get involved in digging around in the prepared statement's
    ** parse tree, check that there actually is one.
    ** For whatever reason, SET SESSION AUTHORIZATION doesn't create one
    ** (the target username is just stuffed into the psq_cb normally.)
    */

    if (proto->pst_mode == PSQ_SET_SESS_AUTH_ID)
    {
	/* In this case there can only be one parameter, and it's varchar */

	desc_base->gca_attdbv.db_length = DB_MAXNAME + 2; /* plus count */
	desc_base->gca_attdbv.db_datatype = DB_VCH_TYPE;
    }
    else
    {
	/* Normal case, churn around in the parse tree.
	** Obtain the QTREE for the statement from QSF.
	*/

	qsf_rb.qsf_obj_id.qso_handle = 
			proto->pst_ostream.psf_mstream.qso_handle;
	status = qsf_call(QSO_INFO, &qsf_rb);

	if (status != E_DB_OK || qsf_rb.qsf_root == NULL)
	{
	    (VOID) psf_error(E_PS0D19_QSF_INFO, qsf_rb.qsf_error.err_code,
		PSF_INTERR, &err_code, &psq_cb->psq_error, 0);
	    goto error_exit;
	}

	/* Unearth the pointer to the query tree proper.  Getting
	** to the qtree root for CREATE VIEW is a little different
	** from the others.
	*/

	prt_pnode = (PST_PROCEDURE *) qsf_rb.qsf_root;
	prt_stmt = prt_pnode->pst_stmts;
	if (proto->pst_mode == PSQ_VIEW)
	    prt_qtree = prt_stmt->pst_specific.pst_create_view.pst_view_tree;
	else
	    prt_qtree = prt_stmt->pst_specific.pst_tree;
	prt_root = prt_qtree->pst_qtree;

	have_parm = FALSE;
	status = pst_descinput_walk(sess_cb, psq_cb,
			proto, desc_base, prt_root, 0, 0, &have_parm);
	if (status != E_DB_OK)
	    goto error_exit;
	/* Walk qtree's range table entries looking for "derived tables",
	** i.e. inline views that might have parameter markers.
	*/
	prt_rangetab = prt_qtree->pst_rangetab;
	for (i = prt_qtree->pst_rngvar_count; i > 0; --i)
	{
	    if ((*prt_rangetab)->pst_rgtype == PST_DRTREE ||
		(*prt_rangetab)->pst_rgtype == PST_WETREE)
	    {
		have_parm = FALSE;
		status = pst_descinput_walk(sess_cb, psq_cb, proto, desc_base,
			(*prt_rangetab)->pst_rgroot, 0, 0, &have_parm);
		if (status != E_DB_OK)
		    goto error_exit;
	    }
	    ++ prt_rangetab;
	}
    }
    /*
    ** Pack GCA_COL_ATT array so that each structure lies
    ** directly after the actual characters used for the
    ** gca_attname field of the structure before, ignoring
    ** any alignment considerations.  (Yuck).
    */
    {
	char	*sp, *dp;
	i4	cpyamt;

	for ( sp = dp = (char*)sqlda->gca_col_desc;
	      --da_num >= 0;
	      sp += GCA_COL_ATT_SIZE )
	{
	    cpyamt = (((GCA_COL_ATT*)0)->gca_attname - (char*)0) +
	      ((GCA_COL_ATT*)sp)->gca_l_attname;
	    if ( sp != dp )
		MEcopy(sp, cpyamt, dp);
	    dp += cpyamt;
	}
    }

    return (E_DB_OK);

error_exit:
    /* Delete the SQLDA to prevent a QSF memory leak */
    qsf_rb.qsf_obj_id.qso_handle = sess_cb->pss_ostream.psf_mstream.qso_handle;
    (void) qsf_call(QSO_DESTROY, &qsf_rb);
    return (E_DB_ERROR);

} /* pst_descinput */

/* Name: pst_descinput_walk - Walk qtree for DESCRIBE INPUT
**
** Description:
**
**	This routine does the hard work for DESCRIBE INPUT after
**	the caller has set things up.  We walk the query tree, looking
**	for parameter markers (they're in CONST nodes).  If one is found,
**	and if there's some sort of type context set higher up in the
**	tree, that type is returned in the parameter marker's SQLDA
**	entry.  If we have a marker with no type context, a flag
**	(have_parm) is returned to the caller, so that we can guess at
**	a type for things like "expr OP ?".
**
**	The most reliable way to supply type context is from a RESDOM
**	node;  this takes care of the most obvious cases, like
**	INSERT VALUES lists, or UPDATE SET col=? clauses.  The other
**	way we get type context is from a binary operator or comparison
**	operator, when one side is ? and the other side has some type.
**	Note that we DO NOT attempt to go nuts and attempt a nearest-type
**	FI resolution on arbitrary function-call operator nodes;
**	there's just too much room for error.  (cast(? as varchar), anyone?)
**	Likewise, we don't attempt to propagate type guesses, so
**	a construct like "int + ? + ?" will end up failing to return
**	a type on the second ? marker.  Here again the idea is that
**	it's just too easy to propagate nonsense, no info is better than
**	wrong info.
**
**	Because the parser likes to copy query tree bits around,
**	we might hit the same parameter marker more than once.
**	After the first time, though, we just ignore it.  The
**	SQLDA types all start out zero so that we can tell.
**
**
** Inputs:
**	sess_cb			PSS_SESBLK * session control block
**	psq_cb			PSQ_CB * query parse info block
**	proto			PST_PROTO * pointer to prototype header
**	desc_base		Pointer to &sqlda.sqlvar[0]
**	node			PST_QNODE * current query tree node
**	context_type		Datatype context for this node
**	context_length		Datatype length for this node
**	have_parm		bool * (an output)
**
** Outputs:
**	*have_parm		Returned TRUE if node is a parameter marker
**				and we don't have any current type available
**	Fills in type info in sqlda
**	Returns E_DB_OK or error
**
** History:
**	1-Jun-2006 (kschendel)
**	    New for DESCRIBE INPUT.
*/

static DB_STATUS
pst_descinput_walk(PSS_SESBLK *sess_cb, PSQ_CB *psq_cb,
	PST_PROTO *proto, GCA_COL_ATT *desc_base,
	PST_QNODE *node, i4 context_type, i4 context_length,
	bool *have_parm)
{


    ADI_OPINFO opinfo;		/* Operator info for op node */
    DB_STATUS op_status;	/* Operator info status */
    DB_STATUS status;		/* Usual status thing */
    GCA_COL_ATT *desc;		/* SQLDA entry for parameter */
    i4 err_code;		/* Junk */
    i4 parm_no;			/* Parameter marker number */
    i4 node_type;		/* The node type */

    /* See if we have reached a parameter node with some sort of
    ** useful type context, if so, note that type in our SQLDA.
    */
    node_type = node->pst_sym.pst_type;
    if (node_type == PST_CONST
      && (parm_no = node->pst_sym.pst_value.pst_s_cnst.pst_parm_no) > 0)
    {
	/* Change param number from 1-origin to 0-origin index */
	-- parm_no;
	if (parm_no > proto->pst_maxparm)
	{
	    TRdisplay("%@ psf_descinput_walk: param marker ix %d higher than max %d at node %p\n",
			parm_no, proto->pst_maxparm, node);
	    (void) psf_error(E_PS0002_INTERNAL_ERROR, 0,
			PSF_INTERR, &err_code, &psq_cb->psq_error, 0);
	    return (E_DB_ERROR);
	}

	/* Address that parm in the sqlda, see if we've figured a type
	** for it already.
	** Caution here, NOT &desc_base[parm_no]!  because of the
	** screwy way that gca-col-att struct is defined.
	*/

	desc = (GCA_COL_ATT *) ((char *) desc_base + (parm_no * GCA_COL_ATT_SIZE));
	if (desc->gca_attdbv.db_datatype != 0)
	    return (E_DB_OK);

	/* If we have some sort of type available from above, record it.
	** If not, inform the caller that some help is needed.
	*/

	if (context_type != 0)
	{
	    desc->gca_attdbv.db_datatype = context_type;
	    desc->gca_attdbv.db_length = context_length;
	}
	else
	{
	    *have_parm = TRUE;
	}
    }

    /* If we're an ATT resdom, take the type "context" from the resdom.
    ** This will help out the simple INSERT and UPDATE cases.
    ** If we're an ADI_OPERATOR op, leave the type context alone;
    ** these are things like + and - which are in general not type-
    ** changing operations.  For anything else, who knows what can
    ** happen, so don't pass any context down.
    ** Leave type context alone for CONST nodes;  the only way they
    ** have children is for IN-lists, and the same context should apply
    ** to all the IN-elements.
    */
    if (node_type == PST_RESDOM)
    {
	i4 targtype = node->pst_sym.pst_value.pst_s_rsdm.pst_ttargtype;

	if (targtype == PST_ATTNO || targtype == PST_LOCALVARNO
	  || targtype == PST_DBPARAMNO | targtype == PST_BYREF_DBPARAMNO
	  || targtype == PST_RESROW_COL)
	{
	    context_type = node->pst_sym.pst_dataval.db_datatype;
	    context_length = node->pst_sym.pst_dataval.db_length;
	} 
	else
	{
	    context_type = 0; context_length = 0;
	}
    }
    else if (node_type == PST_UOP || node_type == PST_BOP
      || node_type == PST_MOP || node_type == PST_COP)
    {
	op_status = adi_op_info(sess_cb->pss_adfcb,
			node->pst_sym.pst_value.pst_s_op.pst_opno, &opinfo);
	if (op_status != E_DB_OK || opinfo.adi_optype != ADI_OPERATOR)
	{
	    context_type = 0; context_length = 0;
	}
	/* else leave them alone for operators */
    }
    else if (node_type != PST_CONST)
    {
	context_type = 0; context_length = 0;
    }

    /* Try left, right subtrees */
    if (node->pst_left != NULL)
    {
	*have_parm = FALSE;
	status = pst_descinput_walk(sess_cb, psq_cb, proto, desc_base,
			node->pst_left, context_type, context_length,
			have_parm);
	if (status != E_DB_OK)
	    return (status);
	if (*have_parm && node_type == PST_BOP && node->pst_right != NULL
	  && op_status == E_DB_OK
	  && (opinfo.adi_optype == ADI_COMPARISON || opinfo.adi_optype == ADI_OPERATOR)
	  && node->pst_right->pst_sym.pst_dataval.db_datatype != 0)
	{
	    /* We have "? binop expr", take a swing at using expr's type */
	    status = pst_descinput_walk(sess_cb, psq_cb, proto, desc_base,
			node->pst_left,
			node->pst_right->pst_sym.pst_dataval.db_datatype,
			node->pst_right->pst_sym.pst_dataval.db_length,
			have_parm);
	    if (status != E_DB_OK)
		return (status);
	}
    }
    if (node->pst_right != NULL)
    {
	*have_parm = FALSE;
	status = pst_descinput_walk(sess_cb, psq_cb, proto, desc_base,
			node->pst_right, context_type, context_length,
			have_parm);
	if (status != E_DB_OK)
	    return (status);
	if (*have_parm && node_type == PST_BOP && node->pst_left != NULL
	  && op_status == E_DB_OK
	  && (opinfo.adi_optype == ADI_COMPARISON || opinfo.adi_optype == ADI_OPERATOR)
	  && node->pst_left->pst_sym.pst_dataval.db_datatype != 0)
	{
	    /* We have "expr binop ?", take a swing at using expr's type */
	    status = pst_descinput_walk(sess_cb, psq_cb, proto, desc_base,
			node->pst_right,
			node->pst_left->pst_sym.pst_dataval.db_datatype,
			node->pst_left->pst_sym.pst_dataval.db_length,
			have_parm);
	    if (status != E_DB_OK)
		return (status);
	}
    }

    /* Special case additional walking for special case tree nodes */
    if (node_type == PST_ROOT
      && node->pst_sym.pst_value.pst_s_root.pst_union.pst_next != NULL)
    {
	status = pst_descinput_walk(sess_cb, psq_cb, proto, desc_base,
			node->pst_sym.pst_value.pst_s_root.pst_union.pst_next,
			0, 0, have_parm);
	if (status != E_DB_OK)
	    return (status);
	*have_parm = FALSE;
    }
    return (E_DB_OK);

} /* pst_descinput_walk */
