/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <pc.h>
#include    <cs.h>
#include    <lk.h>
#include    <st.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ddb.h>
#include    <ulm.h>
#include    <ulf.h>
#include    <adf.h>
#include    <ade.h>
#include    <adfops.h>
#include    <dmf.h>
#include    <dmtcb.h>
#include    <dmrcb.h>
#include    <dmucb.h>
#include    <scf.h>
#include    <qsf.h>
#include    <qefrcb.h>
#include    <rdf.h>
#include    <psfparse.h>
#include    <qefnode.h>
#include    <qefact.h>
#include    <qefqp.h>
#include    <qefqeu.h>
#include    <qefmain.h>
#include    <me.h>
#include    <bt.h>
#include    <tr.h>
/* beginning of optimizer header files */
#include    <opglobal.h>
#include    <opdstrib.h>
#include    <opfcb.h>
#include    <opgcb.h>
#include    <opscb.h>
#include    <ophisto.h>
#include    <opboolfact.h>
#include    <oplouter.h>
#include    <opeqclass.h>
#include    <opcotree.h>
#include    <opvariable.h>
#include    <opattr.h>
#include    <openum.h>
#include    <opagg.h>
#include    <opmflat.h>
#include    <opcsubqry.h>
#include    <opsubquery.h>
#include    <opcstate.h>
#include    <opstate.h>
#include    <opckey.h>
#include    <opcd.h>
#define        OPC_ORIG		TRUE
#include    <opxlint.h>
#include    <opclint.h>


/**
**
**  Name: OPCORIG.C - Routines that fill in orig QEN_NODE's.
**
**  Description:
**      This file contains the main entry point for building orig QEN_NODEs,
**      and supporting routines. For other supporting routines, please refer 
**      to OPCQTREE.C and OPCKEY.C. 
[@comment_line@]...
**
**
**	Externally visable routines:
**	    opc_orig_build() - Entry point to build an orig QEN_NODE.
**          opc_ratt() - Mark the returning eqcs in a cnode.
**          opc_valid() - Allocate and init a QEN_VALID struct.
**          opc_bgmkey() - Begin the AHD_MKEY CX
[@func_list@]...
**
**	Internal only routines (to this file):
**          opc_key() - Build a QEF_KEY struct for an orig node
**          opc_qekey() - Build a QEF_KEY struct from an OPC_KEY struct.
**          opc_qeor() - Build a QEF_KOR struct from an OPC_KOR struct.
**          opc_qeoneand() - Build a QEF_AND struct for an exact match.
**          opc_qerangeand - Build a QEF_AND struct for a range match.
**          opc_parmap() - Build a parameter map from a QTREE.
[@func_list@]...
**
**  History:
**      5-aug-86 (eric)
**	    created
**	02-sep-88 (puree)
**	    Remove qp_cb_cnt.  
**	    Rename the old qp_cb_num to qp_cb_cnt.
**	    Rename qen_adf to qen_ade_cx.
**	17-apr-89 (jrb)
**	    Properly initialized prec field(s).
**      15-aug-89 (eric)
**          Changed the way that repeat query parameters/dbp vars/expressions
**          are used in keying.
**	11-oct-89 (stec)
**	    Added boolean factor bitmap ptr to the list of args
**	    passed to opc_gtqual in opc_orig_build.
**	26-mar-90 (stec)
**	    Removed unused forward references to opc_kmap, opc_kequal.
**	13-apr-90 (nancy)
**	    Fixed bug 20408 in opc_key().  See comments below.
**	13-jul-90 (stec)
**	    Modified opc_valid() to change npage_est arg from i4  to i4.
**	    Changed opc_orig_build() to prevent integer overflow.
**	31-oct-90 (stec)
**	    Fixed bug 32619 in opc_qerangeand().
**	26-nov-90 (stec)
**	    Modified opc_key().
**	18-dec-90 (kwatts)
**	    Smart Disk integration: Added call of opc_sd_qual() to check for
**	    possible Smart Disk access in opc_orig_build().
**	28-dec-90 (stec)
**	    Modified opc_key().
**	18-jan-91 (stec)
**	    Added missing precision initialization to opc_qeoneand(),
**	    opc_qerangeand(), opc_bgmkey().
**	06-feb-91 (stec)
**	    Cleaned opc_qeoneand() and opc_qerangeand() procs.
**	21-feb-91 (stec) (merged by schang 04/20/92)
**	    Modified opc_orig_build() to fix bug 36014.
**	06-mar-91 (stec)
**	    Modified opc_orig_build().
**	16-apr-91 (davebf)
**	    Added parm to opc_valid() call in order to enable vl_size_sensitive
**      18-aug-91 (eric)
**          Added minor supporting changes for bugs 37437 and 36518. See
**          opckey.c for comments about the big picture.
**      26-feb-92 (anitap)
**          Added changes for bug 42291/36295. See opc_bgmkey() for comments.
**      18-mar-92 (anitap)
**          Added changes for bug 42363. See opc_qeoneand() and opc_qerangeand()
**          for comments.
**	13-may-92 (rickh)
**	    Added cnode as an argument to opc_gtqual and opc_gtiqual.  This
**	    enables these routines to tell which outer join boolean factors
**	    they shouldn't touch.
**      23-apr-92 (schang)
**          fix gateway bug #42823. see opc_orig_build() for comments.
**      25-jun-92 (anitap)
**          Added changes for a bug. See opc_bgmkey() for comments.
**      04-aug-92 (anitap)
**          Commented out DEV_TEST after #endif in opc_orig_build() to shut up
**          HP compiler.
**	12-feb-93 (jhahn)
**	    Added support for statement level rules. (FIPS)
**	02-apr-93 (jhahn)
**	    Bug fix for support of statement level rules. (FIPS)
**	14-may-93 (rickh)
**	    Fill in the datatype of function attributes anyway when
**	    marking the returning eqcs of a cnode.
**      21-may-93 (ed)
**          - changed opo_var to be a union structure so that
**          the outer join ID can be stored in interior CO nodes
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	30-jun-93 (rickh)
**	    Added TR.H.
**	20-jul-1993 (pearl)
**          Bug 45912.  In opc_qeor((), set the (new) kor_id field of the
**          QEF_KOR structure; increment the (new) qp_kor_cnt field of the
**          QEF_QP_CB.
**     17-aug-93 (anitap)
**          Typecast the values in the conditional test involving dissimilar
**          types like float and MAXI4. The lack of these casts caused by
**          floating point exceptions on a certain machine in opc_orig_build().
**      14-sep-93 (smc)
**          Moved <cs.h> for CS_SID. Added cast to rdf_call.
**	5-oct-94 (eric)
**          Added support for resource lists in qps.
**	17-jan-94 (rickh)
**	    Fixed a cast.
**	03-feb-94 (rickh)
**	    Replaced 9999 diagnostics with real error messages.
**    19-dec-94 (sarjo01) from 18-feb-94 (markm)
**          If the query is an update/delete make sure that we don't do "dirty"
**          reads on the secondary by setting the "mustlock" flag (58681).
**	11-apr-95 (pchang)
**	    In opc_valid, added check to ensure that the relation being
**	    validated is a secondary index of the result table before setting
**	    the mustlock flag where necessary (64871).
**      10-nov-95 (inkdo01)
**	    Changes to support replacement of QEN_ADF instances by pointers.
**      01-feb-96 (wilri01)
**	    Eliminate orig_qual if all eq on high-order key fields.
**	21-Apr-1997 (shero03)
**	    Support 3D NBR.  Call adi_dt_rtree for rtree index information
**	12-nov-1998 (somsa01)
**	    In opc_valid(), set QEF_SESS_TEMP if this is a Global Temporary
**	    Table.  (Bug 94059)
**	17-mar-1999 (somsa01)
**	    Added include of pc.h and lk.h to resolve references to locking
**	    ids.
**	21-may-1999 (hanch04)
**	    Replace STbcompare with STcasecmp,STncasecmp,STcmp,STncmp
**      15-may-2000 (stial01)
**          Remove Smart Disk code
**      16-Nov-2000 (devjo01)
**          Check for missing hash key components before enabling a node
**          for keyed retreival.  If node uses a multi-part hash key,
**          and a query qualified it with multiple OR clauses, SOME of
**          which did not specify all the hash key components, we would
**          generate keying information in which some OPC_KAND fields
**          would be left uninitialized, leading to a SEGV in qeq_ksort.
**          (b103164).  This replaces insufficent fix for 35506.
**	11-Dec-2000 (devjo01)
**	    Correct error in b103164 fix.  Fix was not correctly
**	    distinguishing between uninitialized and unresolved KAND
**	    structures, which led to database procedure queries being
**	    prevented from using a perfectly legal hash access.
**	    The new QEP used a table scan which in addition to being
**	    inefficient, also led to deadlocks.
**	3-feb-04 (inkdo01)
**	    Changed DB_TID to DB_TID8 for table partitioning.
**      6-jul-06 (kibro01)
**          Added support for LOCALVARNO being used in partition opc_pqmat
**          to fix bug 116306.
[@history_template@]...
**/

/*
**  Forward and/or External function references.
*/
static bool 
opc_rkey(OPS_STATE	*global,
	OPC_NODE	*cnode,
	PST_QNODE	*orig_qual,
	OPO_CO		*tco);

static VOID
opc_key(
	OPS_STATE   *global,
	OPC_NODE    *cnode,
	PST_QNODE   *orig_qual,
	i4	    tids_in_qual );

static QEF_KEY *
opc_qekey(
	OPS_STATE   *global,
	OPC_NODE    *cnode,
	OPC_QUAL    *cqual,
	i4	    *min_key_att );

static QEF_KOR *
opc_qeor(
	OPS_STATE   *global,
	OPC_NODE    *cnode,
	OPC_QUAL    *cqual,
	OPC_KOR	    *opkor,
	i4	    *nkey_att );

static VOID
opc_qeoneand(
	OPS_STATE	*global,
	OPC_NODE	*cnode,
	OPC_QUAL	*cqual,
	DMT_ATT_ENTRY	*att,
	OPC_KAND	*opkand,
	i4		keyno,
	QEF_KOR		*qekor,
	QEF_KAND	**end_qekand );

static VOID
opc_qerangeand(
	OPS_STATE	*global,
	OPC_NODE	*cnode,
	OPC_QUAL	*cqual,
	DMT_ATT_ENTRY	*att,
	OPC_KAND	*opkand,
	i4		keyno,
	QEF_KOR		*qekor,
	QEF_KAND	**end_qekand );

static VOID
opc_parmap(
	OPS_STATE   *global,
	PST_QNODE   *root,
	u_i1	    *pmap );

static QEF_MEM_CONSTTAB *
opc_make_mcnsttab(
	OPS_STATE   *global, 
	OPV_GRV	    *grvp);

static VOID
opc_pqmat(
	OPS_STATE	*global,
	QEN_ORIG	*orig,
	OPC_NODE	*cnode,
	OPV_VARS	*varp,
	OPV_IVARS	varno);

static VOID
opc_pqdim(
	OPS_STATE	*global,
	QEN_ORIG	*orig,
	OPC_NODE	*cnode,
	OPB_PQBF	*pqbfp,
	DB_PART_DEF	*pdefp);

static bool
opc_pqbf_verify(
	OPS_STATE	*global,
	PST_QNODE	*nodep,
	OPZ_IATTS	attno,
	OPV_IVARS	varno);

/*
**  Defines of constants used in this file
*/
#define    OPC_UNKNOWN_LEN    0
#define    OPC_UNKNOWN_PREC   0

/*{
** Name: OPC_ORIG_BUILD	- Fill in orig node specific information into a QEN_NODE
**
** Description:
**      This routine is responsible for building a QEN_NODE to originate a 
**      relation. The main tasks to be done fall into two catagories, 1) fill
**	in the QEN_NODE, and 2) fill in the OPC_NODE. Filling in the QEN_NODE
**	involves: A) deciding how to key into
**      the relation, and B) setting up the qualification information. 
**      Filling in the OPC_NODE involves setting what eqcs and joinop
**      attributes are available from the relation to other nodes. 
[@comment_line@]...
**
** Inputs:
**  global -
**	This is the state variable that hold query specific information. It
**	is used to locate the current subquery struct, the local and global
**	range tables, as well as being a parameter to all other procedures.
**  cnode -
**	This is the OPC specific information about the node being compiled.
**	It is used to location the QEN_NODE that is being built, the arrays
**	that will describe what eqcs and atts this orig node returns, what
**	OPO_CO node to use for the translation to the QEN_NODE.
**
** Outputs:
**  cnode->opc_qennode->node_qen.qen_orig -
**	This will be completely filled in to originate the relation.
**  cnode->opc_ceq -
**	This will describe the eqcs and joinop attributes that are available
**	to the other nodes from this relation.
**
**	Returns:
**	    nothing
**	Exceptions:
**	    only though opx_error() or opx_verror().
**
** Side Effects:
**	    none.
**
** History:
**      5-aug-86 (eric)
**          written
**      8-mar-89 (eric)
**          fixed a bug where clauses that included fatts that could be
**          evaluated at this node weren't being included in the qual.
**          This meant that the orig node was returning tuples that it
**          shouldn't have been.
**	11-oct-89 (stec)
**	    Added boolean factor bitmap ptr to the list of args
**	    passed to opc_gtqual.
**	13-jul-90 (stec)
**	    Changed call to opc_valid() to prevent integer overflow.
**	18-dec-90 (kwatts)
**	    Smart Disk integration: Added call of opc_sd_qual() to check for
**	    possible Smart Disk access.
**	06-mar-91 (stec)
**	    Add consistency check for pages_touched value.
**	16-apr-91 (davebf)
**	    Added parm to opc_valid() call in order to enable vl_size_sensitive
**      20-apr-92 (schang)
**          GW merge and bug fixes.  NOTE: rel->rdr_rel->tbl_relgwid
**          is not 0 even for ingres table (a negative number). we must test for
**          each GW's to avoid descriptor cache full problem,bug#42823.
**	    14-feb-91 (linda)
**	        If this is a base table, or any non-gateway (i4ive Ingres) table
**	        or index, just get the width directly.  If it's a gateway secondary
**	        index, get information from RDF on the parent table and use its
**	        width instead.
**	13-may-92 (rickh)
**	    Added cnode as an argument to opc_gtqual and opc_gtiqual.  This
**	    enables these routines to tell which outer join boolean factors
**	    they shouldn't touch.
**     04-aug-92 (anitap)
**         Commented out xDEV_TEST in #endif so that HP compiler does not
**         complain.
**     17-aug-93 (anitap)
**          Typecast the values in the conditional test involving dissimilar
**          types like float and MAXI4. The lack of these casts caused by
**          floating point exceptions on a certain machine.
**	03-feb-94 (rickh)
**	    Replaced 9999 diagnostics with real error messages.
**      19-jun-95 (inkdo01)
**          Changed i4  orig_tkey to 1 bit flag setting in orig_flag
**	01-feb-96 (wilri01)
**	    Eliminate orig_qual if all eq on high-order key fields.
**	24-apr-96 (inkdo01)
**	    Changes to support Rtree index probes.
**	11-feb-97 (inkdo01)
**	    Support MS Access OR transformed in-memory constant tables.
**	10-nov-98 (inkdo01)
**	    Added support of ORIG_IXONLY flag for base table access which
**	    only needs to look at index pages.
**	18-dec-98 (inkdo01)
**	    Fix to wilri change so we don't drop ADF code for Quel comparisons
**	    which could involve pattern matching semantics.
**	15-jan-99 (hayke02)
**	    Remove wilri01 change (424119) which drops ADF code for eq on
**	    high-order key fields by assigning orig_qual to NULL. This was
**	    occurring erroneously for char/varchar equals predicates where
**	    the constant string is longer than the attribute width (say, n)
**	    and the first n chars of the constant qualifier matches all n
**	    chars in the char/vchar attribute. This change fixes bug 94859.
**	3-nov-99 (inkdo01)
**	    Compute orig_qualrows to aid group read determination.
**	19-dec-03 (inkdo01)
**	    Add support for orig_parray, orig_parrcnt, orig_pcount for
**	    table partitioning.
**	14-Jan-2004 (jenjo02)
**	    Check db_tab_index > 0 for index, not simply != 0.
**	13-apr-04 (inkdo01)
**	    Dropped part_parray/parrcnt for part_gcount/tgcount. Update 
**	    to support partition compatible joins.
**	27-Jul-2004 (schka24)
**	    In the unusual case of calling rdf for base table info here,
**	    make sure the RDF info is unfixed once we get what little we need.
**	28-Feb-2005 (schka24)
**	    Set 4byte tidp flag regardless of query type, we might need to
**	    carry it over to a cursor update or delete.
**	14-Mar-2005 (schka24)
**	    Don't set 4byte tidp flag here at all.  Set it in opcahd where
**	    action header tid info is set.
**	22-Jul-2005 (schka24)
**	    Init work bit-map index in partition info.
[@history_template@]...
*/
VOID
opc_orig_build(
		OPS_STATE   *global,
		OPC_NODE    *cnode)
{
    OPS_SUBQUERY	*subqry = global->ops_cstate.opc_subqry;
    QEN_NODE		*orig = cnode->opc_qennode;
    RDR_INFO		*rel;
    OPO_CO		*co;
    OPO_CO		*tco;
    OPV_VARS		*opv;
    OPV_IVARS		vno;
    QEF_VALID		*valid;
    QEN_PART_INFO	*partp = (QEN_PART_INFO *) NULL;
    i4			align;
    OPE_BMEQCLS		avail_eqcmp;
    OPE_BMEQCLS		qual_eqcmp;
    PST_QNODE		*orig_qual;
    i4			tid_qual;
    i4		lockmode;
    i4	    atype;
    bool		gwmain;



    /* Lets figure out where the real orig node is and point opc_cco at
    ** it. This is because this code is almost never interested in the
    ** PR node.
    */
    tco = cnode->opc_cco;
    if (tco->opo_outer != NULL)
    {
	co = tco->opo_outer;
    }
    else if (tco->opo_inner != NULL)
    {
	co = tco->opo_inner;
    }
    else
    {
	co = tco;
    }
    cnode->opc_cco = co;

    /*
    ** Make sure that ORIG nodes do 
    ** not have an empty eqc map.
    */
    if (BTcount((char *)&cnode->opc_cco->opo_maps->opo_eqcmap,
	(i4)subqry->ops_eclass.ope_ev) == 0
       )
    {
	opx_error( E_OP08AA_EMPTY_NODE );
	return;
    }

    vno = co->opo_union.opo_orig;
    opv = subqry->ops_vars.opv_base->opv_rt[vno];
    rel = opv->opv_grv->opv_relation;

    /* If key column qualified retrieval and based on real histograms, 
    ** estimate rows to be read from table to help group read determination. */
    if (opv->opv_mask & OPV_HISTKEYEST) 
	orig->node_qen.qen_orig.orig_qualrows = 
		opv->opv_keysel * rel->rdr_rel->tbl_record_count;
    else orig->node_qen.qen_orig.orig_qualrows = -1;

    /* As the first real action, lets set up the row number and size */
    orig->qen_rsz = rel->rdr_rel->tbl_width;
    orig->node_qen.qen_orig.orig_flag = 0;   /* clear the flag word */

    if (rel->rdr_parts)
    {
	/* Allocate/init QEN_PART_INFO for partitioned table. */
	OPV_PCJDESC	*wpcjdp;
	i4		dimno;

	partp = (QEN_PART_INFO *)opu_qsfmem(global, sizeof(QEN_PART_INFO));
	orig->node_qen.qen_orig.orig_part = partp;
	partp->part_gcount = 0;
	if ((wpcjdp = co->opo_variant.opo_local->opo_pcjdp))
	{
	    if (vno == wpcjdp->jn_var1)
		dimno = wpcjdp->jn_dim1;
	    else dimno = wpcjdp->jn_dim2;
	    opc_partdim_copy(global, rel->rdr_parts, dimno, 
						&partp->part_pcdim);
	    partp->part_gcount = partp->part_pcdim->nparts /
			co->opo_variant.opo_local->opo_pgcount;
	}
	else partp->part_pcdim = NULL;
	partp->part_qual.part_qpart = NULL;
	partp->part_qual.part_qdim = NULL;
	partp->part_qual.part_qnext = NULL;
	partp->part_qpix = -1;
	partp->part_qpwlix = -1;
	partp->part_qpwpix = -1;
	partp->part_dmrix = -1;
	partp->part_dmtix = -1;
	partp->part_threads = 1;
    }
    else orig->node_qen.qen_orig.orig_part = (QEN_PART_INFO *) NULL;

    if (opv->opv_pqbf != (OPB_PQBF *) NULL)
	opc_pqmat(global, &orig->node_qen.qen_orig, cnode, opv, vno);
				/* build CX to determine qualified p's */

    /*
    ** If this is a base table, or it's a non-gateway table or index, just get
    ** the width directly.  If it's a secondary index, get information from RDF
    ** on the parent table and use its width instead.  14-feb-91 (linda)
    ** schang 04/20/92 merge and bug fixes.  NOTE: rel->rdr_rel->tbl_relgwid
    ** is not 0 even for ingres table (a negative number). we must test for
    ** each GW's to avoid descriptor cache full problem,bug#42823.  There are
    ** other place relies on the value of rel->rdr_rel->tbl_relgwid. Since
    ** there is no associated bug or error, I am hesitant in changing them.
    */

    if ((rel->rdr_rel->tbl_relgwid != DMGW_RMS) ||
        (rel->rdr_rel->tbl_id.db_tab_index <= 0))
	orig->qen_rsz = rel->rdr_rel->tbl_width + 4;
    else
    {
	DB_STATUS   status;
	DB_TAB_ID   basetab;
	RDR_INFO    *tmprel;

	basetab.db_tab_base = rel->rdr_rel->tbl_id.db_tab_base;
	basetab.db_tab_index = 0;
	STRUCT_ASSIGN_MACRO(basetab,
		global->ops_rangetab.opv_rdfcb.rdf_rb.rdr_tabid);
	global->ops_rangetab.opv_rdfcb.rdf_rb.rdr_types_mask = RDR_RELATION;
	global->ops_rangetab.opv_rdfcb.rdf_info_blk = NULL;
	status = rdf_call(RDF_GETDESC, (PTR)&global->ops_rangetab.opv_rdfcb);
	if (DB_FAILURE_MACRO(status) == TRUE)
	{
	    opx_error(E_OP0004_RDF_GETDESC);
	}
	tmprel = global->ops_rangetab.opv_rdfcb.rdf_info_blk;
	orig->qen_rsz = tmprel->rdr_rel->tbl_width;
	/* Release the base table RDF info */
	global->ops_rangetab.opv_rdfcb.rdf_rb.rdr_types_mask = 0;
	(void) rdf_call(RDF_UNFIX, &global->ops_rangetab.opv_rdfcb);
	global->ops_rangetab.opv_rdfcb.rdf_info_blk = NULL;
    }

    if (opv->opv_mask & OPV_BTSSUB &&
	!(global->ops_cstate.opc_qp->qp_status & QEQP_UPDATE)) 
	orig->node_qen.qen_orig.orig_flag |= ORIG_IXONLY;
					/* only need to access bt ix pages */

    if (opv->opv_primary.opv_tid == OPE_NOEQCLS)
    {
	orig->node_qen.qen_orig.orig_tid = -1;
    }
    else
    {
	align = orig->qen_rsz % sizeof (ALIGN_RESTRICT);
	if (align != 0)
	    align = sizeof (ALIGN_RESTRICT) - align;
	orig->qen_rsz += (align + DB_TID8_LENGTH);
	orig->node_qen.qen_orig.orig_tid = orig->qen_rsz - DB_TID8_LENGTH;
    }
    opc_ptrow(global, &orig->qen_row, orig->qen_rsz);

    /* set the eqc info in the cnode for what this orig node returns. */
    opc_ratt(global, cnode->opc_ceq, co, orig->qen_row, orig->qen_rsz);


    /* Now that the available eqcs have been filled in, lets get the 
    ** qualification that we need to apply at this node.
    **
    ** IMPORTANT:
    ** The boolean factor bit map passed comes from the PR node (parent)
    ** rather than the ORIG node, where does not have one.
    */
    opc_gtqual(global, cnode, orig->qen_type, &co->opo_maps->opo_eqcmap,
	(OPE_BMEQCLS *)NULL,
	tco->opo_variant.opo_local->opo_bmbf,
	&orig_qual, &qual_eqcmp);

    /* Now that we know what eqcs are used in the qualification that is
    ** to be applied at this node, lets see if the tid attribute is
    ** used in the qual.
    */
    tid_qual = (opv->opv_primary.opv_tid != OPE_NOEQCLS &&
	    BTtest((i4)opv->opv_primary.opv_tid, (char *)&qual_eqcmp) == TRUE
	);

    if (tid_qual) orig->node_qen.qen_orig.orig_flag |= ORIG_TKEY;

    /* schang : 04/20/92 use switch to get correct value for gwmain */
    switch (rel->rdr_rel->tbl_relgwid)
    {
        case DMGW_RMS :
            gwmain = subqry->ops_sqtype == OPS_MAIN;
            break;
        default :
            gwmain = TRUE;	    /* fix for bug 36014 */
    }

    /* Fill in a QEF_VALID struct */
    if (gwmain &&
        subqry->ops_result != OPV_NOGVAR &&
	opv->opv_grv == 
		global->ops_rangetab.opv_base->opv_grv[subqry->ops_result]
	)
    {
	/*
	** This is the result relation that we are originating, so lets
	** give it an exclusive lock. However, this needs to happen only
	** in the main subquery, in other context this orig node is to be
	** treated as read-only.
	*/
	lockmode = DMT_X;
    }
    else
    {
	lockmode = DMT_S;
    }

    {
	i4	    pages_touched;

	/* Prevent integer overflow */
	pages_touched =
	    (tco->opo_cost.opo_pagestouched < (OPO_BLOCKS)MAXI4) ?
	    (i4)tco->opo_cost.opo_pagestouched : MAXI4;

#ifdef xDEV_TEST
	if (pages_touched == MAXI4)
	{
	    opx_error(E_OP088A_EST_TUPLES);
	}
#endif /* xDEV_TEST */

	valid = opc_valid(global, opv->opv_grv, 
			  lockmode, pages_touched, (bool)TRUE);
    }

    orig->node_qen.qen_orig.orig_get = valid->vl_dmr_cb;
    if (partp)
    {
	/* Set dsh_cbs indexes for partition DMR/DMT_CBs. */
	partp->part_dmrix = valid->vl_dmr_cb;
	partp->part_dmtix = valid->vl_dmf_cb;
	partp->part_totparts = valid->vl_partition_cnt;
    }

    if (cnode->opc_cco->opo_variant.opo_local->opo_mask & OPO_READBACK)
    {
	i4	ordcount;

	orig->node_qen.qen_orig.orig_flag |= ORIG_READBACK;
				/* flag for read backwards (descending sort) */
	if (subqry->ops_msort.opo_tsortp)
	 for (ordcount = 0; 
	    	subqry->ops_msort.opo_tsortp->opo_eqorder[ordcount] >= 0 
		&& ordcount < DB_MAXKEYS; ordcount++);
	else ordcount = -1;
	orig->node_qen.qen_orig.orig_ordcnt = ordcount;
				/* count of descending "order by" cols */
    }
    else orig->node_qen.qen_orig.orig_ordcnt = -1;
    if (opv->opv_grv->opv_gmask & OPV_MCONSTTAB)
	orig->node_qen.qen_orig.orig_flag |= ORIG_MCNSTTAB;
				/* flag funny in-memory tables */

    /* Fill in an QEF_KEY struct giving the keying information; 
    ** Along the way, we compile the code to make the keys and
    ** link the QEF_KEY struct into the action header.
    */
    if (opv->opv_mask & OPV_SPATF && orig_qual)
    {
	if (!opc_rkey(global, cnode, orig_qual, tco))
				/* special Rtree keybld code */
	    opx_error(E_OP08AD_SPATIAL_ERROR);
    }
    else opc_key(global, cnode, orig_qual, tid_qual);
    
    /* Adjust keywidth for certain descending sort queries. */
    if (orig->node_qen.qen_orig.orig_ordcnt > 0 &&
	orig->node_qen.qen_orig.orig_pkeys)
     orig->node_qen.qen_orig.orig_pkeys->key_width += 
	orig->node_qen.qen_orig.orig_ordcnt;

    /* Compile the ADF code to qualify the tuple in the orig node.
    */
    if (!(opv->opv_mask & OPV_SPATF))	/* Rtree 2ary's don't have qual code */
	 opc_ccqtree(global, cnode, orig_qual, &orig->node_qen.qen_orig.
	  orig_qual, (tid_qual == FALSE ? orig->qen_row : -1));

    /* Since we played around with the setting of opc_cco at the beginning
    ** of this routine, we must restore it to its original setting before
    ** we return.
    */
    cnode->opc_cco = tco;
}

/*{
** Name: OPC_RKEY	- build code to construct Rtree key structure
**
** Description:
**      This routine builds code to project spatial constant for use in Rtree
**	probe, then builds QEF key structure to define the probe. The code
**	places the constant into the ADF constant buffer, then performs a
**	coercion (if needed) in the virgin code. Finally, the "nbr" function
**	is executed to project the NBR into the key buffer where it will
**	be picked up by QEF. 
[@comment_line@]...
**
** Outputs:
**      None
**	Returns:
**	    none
**	Exceptions:
**	    Error E_OP08AD_SPATIAL_ERROR has been defined, if we have need
**	    for a general purpose exception code. 
**
** Side Effects:
**	    none
**
** History:
**      22-apr-96 (inkdo01)
**          written
**	21-Apr-1997 (shero03)
**	    Support 3D NBR.  Call adi_dt_rtree for rtree index information
**	25-aug-98 (inkdo01)
**	    Fixed code at end which removes non-TID eqclass. In queries where same 
**	    spatial column is referenced in several predicates, there may be more
**	    than 1 to remove, and this code does it.
[@history_template@]...
*/
static bool opc_rkey(OPS_STATE	*global,
		OPC_NODE	*cnode,
		PST_QNODE	*orig_qual,
		OPO_CO		*tco)

{
    PST_QNODE	range; 		/* range constant for NBR call */
    PST_QNODE	*spatcon;
    PST_QNODE	*spatbop;
    
    OPS_SUBQUERY *subqry = global->ops_cstate.opc_subqry;
    QEF_QP_CB	*qp = global->ops_cstate.opc_qp;
    QEF_AHD	*ahd = subqry->ops_compile.opc_ahd;
				/* current action header */
    OPC_ADF	*cadf = &subqry->ops_compile.opc_mkey;
				/* where the key building code'll go */
    QEN_NODE	*orig = cnode->opc_qennode;
				/* our ORIG node */
    OPV_VARS	*varp = subqry->ops_vars.opv_base->opv_rt[cnode->opc_cco->
	opo_union.opo_orig];	/* var ptr for the Rtree */
    
    QEF_KEY	*qekey;		/* key structure ptrs */
    QEF_KOR	*qekor;
    QEF_KAND	*qekand;
    QEF_KATT	*qekatt;

    i4		dummy, align;
    ADE_OPERAND	ops[3];
    ADI_OP_ID	inside_op, overlaps_op, intersects_op;
    ADI_DT_RTREE rtree_blk;	/* Information given the datatype of col indexed*/

    /* Lookup various spatial codes (not intrinsics, so we have to ask
    ** for them). */
    if (adi_opid(global->ops_adfcb, "inside", &inside_op) ||
    	adi_opid(global->ops_adfcb, "intersects", &intersects_op) ||
    	adi_opid(global->ops_adfcb, "overlaps", &overlaps_op))
				/* first, load relevant "methods" operators */
    	return(FALSE);		/*  If any fail, return ** error */

    /* Find spatial operator, first locating last of the AND's (code in
    ** opc_gtqual ensures that it's there), then determining
    ** whether function or infix notation was used. */

    for (;orig_qual->pst_sym.pst_type == PST_AND && orig_qual->pst_right;
	orig_qual = orig_qual->pst_right);
					/* loop to find last AND */

    if (orig_qual->pst_sym.pst_type != PST_AND) return(FALSE);
     else spatbop = orig_qual->pst_left;

    if (!(spatbop->pst_sym.pst_value.pst_s_op.pst_opno == inside_op ||
	spatbop->pst_sym.pst_value.pst_s_op.pst_opno == intersects_op ||
	spatbop->pst_sym.pst_value.pst_s_op.pst_opno == overlaps_op))
     spatbop = spatbop->pst_left;	/* func notation - spatial op is
					** one more level down */

    /* Find spatial constant expression to NBRize */
    if (spatbop->pst_left->pst_sym.pst_type == PST_VAR)
	spatcon = spatbop->pst_right;
    else spatcon = spatbop->pst_left;

    /* Make sure there's a ahd_mkey to stick this stuff into */
    if (ahd->ahd_mkey == NULL) opc_bgmkey(global, ahd, cadf);

    /* Build code to project the NBR of the spatial constant/parm into 
    ** the key buffer */
    ops[1].opr_dt = spatcon->pst_sym.pst_dataval.db_datatype;
    ops[1].opr_len = OPC_UNKNOWN_LEN;
    ops[1].opr_prec = OPC_UNKNOWN_PREC;
    opc_cqual(global, cnode, spatcon, cadf, ADE_SVIRGIN, &ops[1], &dummy);
 
    if (adi_dt_rtree(global->ops_adfcb, 
    			spatcon->pst_sym.pst_dataval.db_datatype, 
			&rtree_blk))
    	return(FALSE);

    /* Now stick the "default range" parm to the NBR call into the
    ** constant buffer */
    range.pst_left = (PST_QNODE *)NULL;
    range.pst_right = (PST_QNODE *)NULL;
    range.pst_sym.pst_type = PST_CONST;
    range.pst_sym.pst_dataval.db_datatype = varp->opv_grv->opv_relation->
		rdr_rel->tbl_rangedt;
    range.pst_sym.pst_dataval.db_data = (char *)varp->opv_grv->opv_relation->
		rdr_rel->tbl_range;	/* range value from iirange */
    range.pst_sym.pst_dataval.db_length = varp->opv_grv->opv_relation->
		rdr_rel->tbl_rangesize;
    range.pst_sym.pst_dataval.db_prec = 0;
    MEfill(sizeof(range.pst_sym.pst_value.pst_s_cnst), (u_char)0,
	&range.pst_sym.pst_value.pst_s_cnst);
    range.pst_sym.pst_value.pst_s_cnst.pst_tparmtype = PST_USER;
    range.pst_sym.pst_value.pst_s_cnst.pst_cqlang = DB_SQL;
    ops[2].opr_dt = rtree_blk.adi_range_dtid;
    ops[2].opr_len = OPC_UNKNOWN_LEN;
    ops[2].opr_prec = OPC_UNKNOWN_PREC;
    opc_cqual(global, cnode, &range, cadf, ADE_SVIRGIN, &ops[2], &dummy);

    /* Set up NBR output operand, and load up the NBR instruction.
    ** This NBRizes the constant/parm into the key buffer for QEF. */
    ops[0].opr_dt = rtree_blk.adi_nbr_dtid;
    ops[0].opr_len = rtree_blk.adi_hilbertsize * 2;
    ops[0].opr_prec = 0;
    ops[0].opr_base = cadf->opc_key_base;
    ops[0].opr_offset = qp->qp_key_sz;
    opc_adinstr(global, cadf, rtree_blk.adi_nbr_fiid, ADE_SMAIN, 3, ops, 1);

    /* All that's left (?) is to allocate and format the QEF key 
    ** structure to define the Rtree probe */
    qekey = (QEF_KEY *)opu_qsfmem(global, (i4)(sizeof(QEF_KEY) +
	sizeof(QEF_KOR) + sizeof(QEF_KAND) + sizeof(QEF_KATT)));
					/* alloc, then chop into pieces */
    qekor = (QEF_KOR *)((char *)qekey + (i4)sizeof(QEF_KEY));
    qekand = (QEF_KAND *)((char *)qekor + (i4)sizeof(QEF_KOR));
    qekatt = (QEF_KATT *)((char *)qekand + (i4)sizeof(QEF_KAND));

    qekey->key_chain = ahd->ahd_keys;	/* format the buggers */
    ahd->ahd_keys = qekey;		/* link into "key chain" */
    qekey->key_kor = qekor;		/* qekey->qekor */
    qekey->key_width = 1;		/* always "1" */

    qekor->kor_next = (QEF_KOR *)NULL;
    qekor->kor_keys = qekand;		/* qekor->qekand */
    qekor->kor_id = qp->qp_kor_cnt++;
					/* update QP key count */
    qekor->kor_numatt = 1;

    qekand->kand_type = QEK_EQ;
    qekand->kand_next = (QEF_KAND *)NULL;
    qekand->kand_attno = 1;		/* MBR is col 1 of Rtree */
    qekand->kand_keys = qekatt;		/* qekand->qekatt */

    qekatt->attr_attno = 1;
    qekatt->attr_type = rtree_blk.adi_nbr_dtid;
    qekatt->attr_length = rtree_blk.adi_hilbertsize * 2;
    qekatt->attr_prec = 0;
    qekatt->attr_value = qp->qp_key_sz;
    qekatt->attr_next = (QEF_KATT *)NULL;

    /* Turn the spatial function opcode into DMF Rtree probe code */
    if (spatbop->pst_sym.pst_value.pst_s_op.pst_opno == intersects_op)
	qekatt->attr_operator = DMR_OP_INTERSECTS;
    else if (spatbop->pst_sym.pst_value.pst_s_op.pst_opno ==
	overlaps_op) qekatt->attr_operator = DMR_OP_OVERLAY;
    else if (spatbop->pst_sym.pst_value.pst_s_op.pst_opno ==
	inside_op)
    {
	/* "inside" is executed as INSIDE if the column is the left
	** operand and CONTAINS if the column is the right operand */
	if (spatbop->pst_left->pst_sym.pst_type == PST_VAR)
	    qekatt->attr_operator = DMR_OP_INSIDE;
	else qekatt->attr_operator = DMR_OP_CONTAINS;
    }

    orig->node_qen.qen_orig.orig_pkeys = qekey;	
					/* add key struct to our ORIG */
    orig->node_qen.qen_orig.orig_keys = qp->qp_cb_cnt;
					/* CB slot for QEN_TKEY struct */
    orig->node_qen.qen_orig.orig_flag |= ORIG_RTREE;
					/* a little help for others */
    qp->qp_cb_cnt++;
    align = (rtree_blk.adi_hilbertsize * 2) % sizeof (ALIGN_RESTRICT);
    if (align != 0)
	align = sizeof (ALIGN_RESTRICT) - align;
    qp->qp_key_sz += (i4)(rtree_blk.adi_hilbertsize * 2) + align;	/* update key offset */
    /* Last task - remove the data column from PR eqcmap, to prevent non-
    ** existent column from occupying buffer space above. It must be one of
    ** only two possible eqclasses in this node - one's the TID which we
    ** keep, the other's the one we want. */
    for (dummy = -1; (dummy = BTnext(dummy, (char *)&tco->opo_maps->opo_eqcmap,
		(i4)subqry->ops_eclass.ope_ev)) >= 0; )
     if (subqry->ops_eclass.ope_base->ope_eqclist[dummy]->ope_eqctype != OPE_TID)
      BTclear(dummy, (char *)&tco->opo_maps->opo_eqcmap);	/* clear it */

    return(TRUE);
}		/* end of opc_rkey */

/*{
** Name: OPC_RATT	- Mark the returning eqc's in the cnode
**
** Description:
**      This routine fills in an eqc map, an OPC_EQ array with the eqcs that 
**      the current OPO_CO  node (not necessarily an orig node) returns. 
[@comment_line@]...
**
** Inputs:
**  global -
**	This is the query-wide state variable. We will use it to locate the
**	current subquery struct.
**  ceq -
**	These are arrays of eqc and joinop attribute information of sufficient
**	size. They should be filled with zeros upon entry.
**  co -
**	This is the co node the is being used to translate into the QEN_NODE.
**  rowno -
**	This is the DSH row number that where the tuple for the current node
**	will be held.
**  rowsz -
**	This is the size of rowno.
**
** Outputs:
**  ceq -
**	Those eqcs and atts that will be returned from this node will have
**	their respective array entries filled.
**
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      3-sept-86 (eric)
**          written
**	14-may-93 (rickh)
**	    Fill in the datatype of function attributes anyway when
**	    marking the returning eqcs of a cnode.
**	30-mar-04 (inkdo01)
**	    Set TID lengths properly.
[@history_template@]...
*/
VOID
opc_ratt(
		OPS_STATE   *global,
		OPC_EQ	    *ceq,
		OPO_CO	    *co,
		i4	    rowno,
		i4	    rowsz)
{
    OPS_SUBQUERY	*subqry = global->ops_cstate.opc_subqry;
    RDR_INFO		*rel;
    OPE_IEQCLS		eqcno;
    OPZ_IATTS		jattno;
    i4			iattno;
    OPV_IVARS		reli;
    OPZ_ATTS		*jatt;
    OPZ_FATTS		*fatt;
    ADE_OPERAND		ade_operand;
    DB_DATA_VALUE	*dataValue;

    /*
    ** Set up some some frequently used variables 
    ** that help keep the line length down.
    */
    reli = co->opo_union.opo_orig;
    rel = subqry->ops_vars.opv_base->opv_rt[reli]->opv_grv->opv_relation;

    for (eqcno = -1;
	 (eqcno = BTnext((i4) eqcno, (char *)&co->opo_maps->opo_eqcmap,
		(i4)subqry->ops_eclass.ope_ev)) > -1;)
    {
	OPE_EQCLIST	*eqc;

	/* If we already have both the eq and att
	** information, then don't overwrite it.
	*/
	if (ceq[eqcno].opc_eqavailable == TRUE &&
	    ceq[eqcno].opc_attavailable == TRUE
	    )
	{
	    continue;
	}

	eqc = subqry->ops_eclass.ope_base->ope_eqclist[eqcno];

	for (jattno = -1;
	     (jattno = BTnext((i4)jattno, (char *)&eqc->ope_attrmap,
		(i4)subqry->ops_attrs.opz_av)) > -1;)
	{
	    jatt = subqry->ops_attrs.opz_base->opz_attnums[jattno];
	    iattno = jatt->opz_attnm.db_att_id;

	    /*
	    ** Look only at attributes that belong
	    ** to the interesting relation.
	    */
	    if (jatt->opz_varnm == reli)
	    {
		/* fill in the eqc and att info; */

		if (iattno == OPZ_FANUM)
		{
		    /* 
		    ** Filter out function attributes representing special
		    ** equivalence classes for outer joins. These can only
		    ** be materialized and evaluated at join nodes, so they
		    ** are of no interest to us here.
		    */
		    fatt = subqry->ops_funcs.opz_fbase->
			opz_fatts[jatt->opz_func_att];
		    if (!(fatt->opz_mask & OPZ_OJFA))
		    {
			ceq[eqcno].opc_eqavailable = TRUE;
			ceq[eqcno].opc_attno = jattno;
			ceq[eqcno].opc_attavailable = FALSE;

			/* fill in the datatype */

			opc_getFattDataType( global, ( i4  ) jattno,
				&ade_operand );

			dataValue = &ceq[eqcno].opc_eqcdv;
			dataValue->db_data = ( PTR ) NULL;
			dataValue->db_length = ade_operand.opr_len;
			dataValue->db_datatype = ade_operand.opr_dt;
			dataValue->db_prec = ade_operand.opr_prec;
		    }
		    continue;
		}

		ceq[eqcno].opc_eqavailable = TRUE;
		ceq[eqcno].opc_attno = jattno;
		ceq[eqcno].opc_attavailable = TRUE;
		ceq[eqcno].opc_dshrow = rowno;
		STRUCT_ASSIGN_MACRO(jatt->opz_dataval, ceq[eqcno].opc_eqcdv);

		if (iattno == DB_IMTID)
		{
		    ceq[eqcno].opc_dshoffset = rowsz - DB_TID8_LENGTH;
		    ceq[eqcno].opc_eqcdv.db_length = DB_TID8_LENGTH;
		}
		else
		{
		    ceq[eqcno].opc_dshoffset = 
			rel->rdr_attr[iattno]->att_offset;
		}
	    }
	}
    }
}

/*{
** Name: OPC_KEY	- Build a QEF_KEY struct for an orig node.
**
** Description:
**      This routine does the overall coordination for deciding what sort of 
**      keying, and hence building the QEF_KEY structures, for orig nodes. 
**      Being a coordinator, it doesn't do much except call other keying 
**      routines. The job does fall into two distinct phases: 1) make the
**	actual decisions on how to key into the relation, and 2) convert
**	these decisions into the data structures that QEF expects.
**	While making the actual decisions, the interesting routines, 
**	opc_keyqual(), opc_range(), opc_meqrange(), and opc_srange(), are 
**	located in OPCQTREE.C and OPCKEY.C. Converting the decisions is done
**	by opc_qekey() and is located below in this file.
[@comment_line@]...
**
** Inputs:
**  global -
**	This is the query-wide state information. It is used to find the qp
**	that is currently being built, the current subquery, as well as a
**	parameter to all other OPC routines.
**  cnode -
**	This is used to find the current CO and QEN nodes, as well as a
**	parameter to other routines who will very likely use the eqc and
**	att information.
**  orig_qual -
**	This is a qualification that must applied at the current orig node.
**	This will be used when we look for clauses that are useful for keying.
**  tids_in_qual -
**	This will be TRUE if there the TID attribute is used in orig_qual. It
**	is FALSE otherwise.
**  
**
** Outputs:
**  cnode->opc_qennode->node_qen.qen_orig.orig_pkeys -
**  cnode->opc_qennode->node_qen.qen_orig.orig_keys -
**  cnode->opc_qennode->node_qen.qen_orig.orig_flag -
**	These fields will be filled in, which describe how to QEF should key
**	into the relation.
**
**	Returns:
**	    nothing
**	Exceptions:
**	    none, except for opx_error() or opx_verror()
**
** Side Effects:
**	    none
**
** History:
**      7-aug-86 (eric)
**          written
**	13-apr-90 (nancy) -- Bug 20408. Change condition under which 
**	    ukey is set.  If it is a tidkey, it must also be an equality and
**	    not have any "or" clauses.  Note:  This necessitate making changes
**	    to qef to not depend on ukey for tid positioning. (that is, this
**	    might be a tid qual, but NOT necessarily a unique key).
**	26-nov-90 (stec)
**	    Added initialization of print buffer for trace code.
**	28-dec-90 (stec)
**	    Changed code to reflect fact that print buffer is in OPC_STATE
**	    struct.
**      19-jun-95 (inkdo01)
**          Changed i4  orig_ukey to 1 bit flag setting in orig_flag
**	16-nov-2000 (devjo01)
**	    Fix for b103164.  (Incomplete hash key specifications.)
**	11-Dec-2000 (devjo01)
**	    Correct error in b103164 fix.  Fix was not correctly
**	    distinguishing between uninitialized and unresolved KAND
**	    structures, which led to database procedure queries being
**	    prevented from using a perfectly legal hash access.
[@history_template@]...
*/
static VOID
opc_key(
	OPS_STATE   *global,
	OPC_NODE    *cnode,
	PST_QNODE   *orig_qual,
	i4	    tids_in_qual )
{
    QEF_KEY		*qekey;
    OPS_SUBQUERY	*subqry = global->ops_cstate.opc_subqry;
    QEF_QP_CB		*qp = global->ops_cstate.opc_qp;
    OPO_CO		*co = cnode->opc_cco;
    OPV_IVARS		reli;
    RDR_INFO		*rel;
    QEN_NODE		*orig = cnode->opc_qennode;
    OPC_KOR		*kor;
    i4			and_size;
    OPV_VARS		*opv;
    i4			min_key_att = -1;
    OPC_QUAL		*cqual;
    i4			kandi;

    /* Initialize the values that we want to set */
    orig->node_qen.qen_orig.orig_pkeys = NULL;
    orig->node_qen.qen_orig.orig_keys = 0;
    orig->node_qen.qen_orig.orig_flag &= (0xffffffff-ORIG_UKEY);

    /* Init some vars that will keep line length down. */
    reli = cnode->opc_cco->opo_union.opo_orig;
    opv = subqry->ops_vars.opv_base->opv_rt[reli];
    rel = opv->opv_grv->opv_relation;

    if (tids_in_qual == TRUE && opv->opv_primary.opv_tid != OPE_NOEQCLS)
    {
	/* copy the qualification, eliminating CNF clauses that are not needed, 
	** ie. clauses that refer to non-orig relations or non-key attributes.
	*/
	cqual = opc_keyqual(global, cnode, orig_qual, co->opo_union.opo_orig, TRUE);
    }
    else
    {
	cqual = NULL;
    }

    /*
    ** If we couldn't find a tid key, then lets look for a key using the
    ** storage structure
    */
    if (cqual == NULL || cqual->opc_cnf_qual == NULL)
    {
	if (rel->rdr_rel->tbl_storage_type == DMT_HEAP_TYPE)
	{
	    return;
	}

	/* copy the qualification, eliminating CNF clauses that are not needed,
	** ie. clauses that refer to non-orig relations or non-key attributes.
	*/
	cqual = opc_keyqual(global, cnode, orig_qual, co->opo_union.opo_orig, FALSE);
    }

    if (cqual != NULL && cqual->opc_cnf_qual != NULL)
    {
	if (rel->rdr_rel->tbl_storage_type == DMT_HASH_TYPE &&
	    cqual->opc_qsinfo.opc_inequality == TRUE)
	{
	    return;
	}

	/* use the CNF key qual to build an OPC_KEY struct; */
	kor = (OPC_KOR *) opu_Gsmemory_get(global, sizeof (OPC_KOR));
	MEfill(sizeof (OPC_KOR), (u_char)0, (PTR)kor);

	and_size = sizeof (OPC_KAND) * cqual->opc_qsinfo.opc_nkeys;
	kor->opc_ands = (OPC_KAND *) opu_Gsmemory_get(global, and_size);
	MEfill(and_size, (u_char)0, (PTR)kor->opc_ands);
	for (kandi = 0; kandi < cqual->opc_qsinfo.opc_nkeys; kandi += 1)
	{
	    kor->opc_ands[kandi].opc_krange_type = ADE_1RANGE_DONTCARE;
	}

	if (cqual->opc_qsinfo.opc_nparms > 0)
	{
	    opc_meqrange(global, cqual, kor);

	    if (cqual->opc_dnf_qual == NULL)
	    {
		opc_srange(global, cqual, kor);
	    }
	}
	else
	{
	    opc_range(global, cqual, kor);
	}

#ifdef OPT_F027_FUSED_OPKEY
	if (opt_strace(global->ops_cb, OPT_F027_FUSED_OPKEY) == TRUE)
	{
	    char	temp[OPT_PBLEN + 1];
	    bool	init = 0;

	    if (global->ops_cstate.opc_prbuf == NULL)
	    {
		global->ops_cstate.opc_prbuf = temp;
		init++;
	    }

	    TRdisplay("Fused OPC_KEY:\n");
	    opt_opkeys(global, cqual);
	    
	    if (init)
	    {
		global->ops_cstate.opc_prbuf = NULL;
	    }
	}
#endif
        /*
        ** If this we are using hash keying, use the DNF
        ** version of the keying information to confirm that
        ** all key component were specified in each clause.  
        **
        ** If not, then we cannot make use of the hash key.
        */
        if (rel->rdr_rel->tbl_storage_type == DMT_HASH_TYPE)
        {
            /* For each OR group. */
            for (kor = cqual->opc_dnf_qual; kor; kor = kor->opc_ornext)
            {   
                /* For each AND */
                for (kandi = 0; kandi < cqual->opc_qsinfo.opc_nkeys; kandi += 1)
                {
                    if ( (kor->opc_ands[kandi].opc_khi == NULL) &&
			 (kor->opc_ands[kandi].opc_kphi == NULL) &&
			 (kor->opc_ands[kandi].opc_krange_type ==
			  ADE_1RANGE_DONTCARE) )
                    {
                        /* At least one key component not specified,
                        ** leave routine without setting 'orig_pkeys',
                        ** and without creating the QEF_KEY struct.
                        */
                        return;
                    } 
                }
            }
        }

	/* use the OPC_KEY struct to build an QEF_KEY struct; */
	qekey = opc_qekey(global, cnode, cqual, &min_key_att);

	/* Choose a CB slot for QEF's current key info.
	*/
	if (qekey != NULL)
	{
	    orig->node_qen.qen_orig.orig_pkeys = qekey;
	    orig->node_qen.qen_orig.orig_keys = qp->qp_cb_cnt;
	    qp->qp_cb_cnt++;

	    /* Set unique key info; */
	    if (cqual->opc_qsinfo.opc_tidkey == TRUE &&
		cqual->opc_qsinfo.opc_inequality == FALSE &&
		qekey->key_kor != NULL &&
		qekey->key_kor->kor_next == NULL)
	    {
		orig->node_qen.qen_orig.orig_flag |= ORIG_UKEY;
	    }
	    else if ((rel->rdr_rel->tbl_status_mask & DMT_UNIQUEKEYS) &&
			cqual->opc_qsinfo.opc_inequality == FALSE &&
			qekey->key_kor != NULL &&
			qekey->key_kor->kor_next == NULL &&
			rel->rdr_keys->key_count == min_key_att
		    )
	    {
		/* If we are keying into a relation with a unique storage structure
		** with a single fully specified equality key, then we should tell
		** QEF to look for only one tuple from this relation.
		*/
		orig->node_qen.qen_orig.orig_flag |= ORIG_UKEY;
	    }
	}
    }
}

/*{
** Name: OPC_QEKEY	- Build a QEF_KEY struct from an OPC_KEY struct.
**
** Description:
**      This routine performs the fairly straight forward translation from
**	an OPC_KEY struct into a QEF_KEY struct. This translation is required
**      because opc_range() would find it too difficult to generate QEF_KEY 
**      structs directly. Instead, opc_range() produces OPC_KEY structs, which 
**      have been designed to make its job easier. Both OPC_KEY and QEN_KEY
**      structs are, if you will remember, are the top level structures that 
**      describe how to key into a relation. 
[@comment_line@]...
**
** Inputs:
**  global -
**	This is the query wide state variable. It is used for finding the
**	current subquery struct, and as a parameter to all other OPC routines.
**  cnode -
**	This is used as a parameter to other routines, primarily for its
**	ability to translate eqcs to joinop atts.
**  cqual -
**	This holds OPC keying information in various forms. The main one
**	of interest is cqual->opc_dnf_qual which is a pointer to an OPC_KEY
**	struct. This will be the source of the translation.
**  min_key_att -
**	This is a pointer to an uninitialized i4
**
** Outputs:
**  min_key_att -
**	This returns the smallest number of key attributes that that any
**	key range uses. This is used primarily to determine if less than
**	a full key specification is used.
**
**	Returns:
**	    A completed QEF_KEY struct.
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      21-aug-86 (eric)
**          written
[@history_template@]...
*/
static QEF_KEY *
opc_qekey(
	OPS_STATE   *global,
	OPC_NODE    *cnode,
	OPC_QUAL    *cqual,
	i4	    *min_key_att )
{
    OPS_SUBQUERY	*subqry = global->ops_cstate.opc_subqry;
    QEF_AHD		*ahd = subqry->ops_compile.opc_ahd;
    i4		max_width;
    i4			nkey_att;
    QEF_KEY		*qekey;
    QEF_KOR		*qekor;
    OPC_KOR		*opkor;
    OPC_KAND		*opkand;

    /* if this is a full scan of the relation, then return NULL */
    if (cqual == NULL || 
	    cqual->opc_dnf_qual == NULL ||
	    cqual->opc_dnf_qual->opc_ands == NULL
	)
    {
	return(NULL);
    }

    opkand = cqual->opc_dnf_qual->opc_ands;
    if (opkand[0].opc_khi == NULL &&
	    (opkand[0].opc_kphi == NULL || BTnext(-1, opkand[0].opc_kphi, 
			    cqual->opc_qsinfo.opc_hiparm) == -1
	    ) &&
	    opkand[0].opc_klo == NULL &&
	    (opkand[0].opc_kplo == NULL || BTnext(-1, opkand[0].opc_kplo, 
			    cqual->opc_qsinfo.opc_hiparm) == -1
	    )
	)
    {
	return(NULL);
    }

    if (!ahd->ahd_mkey || ahd->ahd_mkey->qen_ade_cx == NULL)
    {
	opc_bgmkey(global, ahd, &subqry->ops_compile.opc_mkey);
    }

    qekey = (QEF_KEY *) opu_qsfmem(global, sizeof (QEF_KEY));
    qekey->key_chain = ahd->ahd_keys;
    qekey->key_width = 0;
    qekey->key_kor = NULL;

    /* link this qekey into the action header */
    global->ops_cstate.opc_subqry->ops_compile.opc_ahd->ahd_keys = qekey;

    /* for (each opkor) */
    *min_key_att = -1;
    max_width = 0;
    for (opkor = cqual->opc_dnf_qual; opkor != NULL; opkor = opkor->opc_ornext)
    {
	/* make an qekor; */
	qekor = opc_qeor(global, cnode, cqual, opkor, &nkey_att);

	/* add it to the list; */
	qekor->kor_next = qekey->key_kor;
	qekey->key_kor = qekor;

	/* if (the width of this qekor is the max) */
	if (qekor->kor_numatt > max_width)
	    max_width = qekor->kor_numatt;

	if (nkey_att < *min_key_att || *min_key_att == -1)
	    *min_key_att = nkey_att;

    }

    qekey->key_width = max_width;
    return(qekey);
}

/*{
** Name: OPC_QEOR	- Build an QEF_KOR struct from an OPC_KOR struct.
**
** Description:
**      This routine performs that relative simple translation from an OPC_KOR 
**      struct to a QEF_KOR struct. Both structs represent a single span, ie.
**      a place to start and stop, in the relation to search. The span can
**      be represented by either an exact match or a range search. With the
**      exact match, the min and max values to search are the same. With a 
**      range search, the min and max values to seach are different. 
[@comment_line@]...
**
** Inputs:
**  global -
**	This is the query wide state variable. It is used to find the RDR_INFO
**	descriptor for the current relation, and as a parameter to all other
**	OPC routines.
**  cnode -
**	This is used to find joinop's local range variable number, as well as
**	a parameter to other routines.
**  cqual -
**	This holds all the OPC structures that are related to keying. This is
**	used to find out how many key attributes we're dealing with here, and
**	if we're building a key for a tid attribute.
**  opkor -
**	This is the OPC structure that describes the span (range) that we need
**	to search.
**  nkey_att -
**	This is a pointer to an uninitialized nat.
**
** Outputs:
**  nkey_att -
**	This tells the number of key attributes that this KOR uses.
**
**	Returns:
**	    The completed QEF_KOR.
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      21-AUG-86 (eric)
**          written
**	20-jul-1993 (pearl)
**          Bug 45912.  In opc_qeor((), set the (new) kor_id field of the
**          QEF_KOR structure; increment the (new) qp_kor_cnt field of the
**          QEF_QP_CB.
[@history_template@]...
*/
static QEF_KOR *
opc_qeor(
	OPS_STATE   *global,
	OPC_NODE    *cnode,
	OPC_QUAL    *cqual,
	OPC_KOR	    *opkor,
	i4	    *nkey_att )
{
    RDR_INFO		*rel;
    QEF_KOR		*qekor;
    i4			kandi;
    OPC_KAND		*kand;
    QEF_KAND		*end_qekand;
    DMT_ATT_ENTRY	*att;
    DMT_ATT_ENTRY	tidatt;
    OPC_QCONST		*khi;
    OPC_QCONST		*klo;
    OPC_QCONST		*kphi;
    OPC_QCONST		*kplo;
    QEF_QP_CB           *qp = global->ops_cstate.opc_qp;
    i4			lo_done = FALSE;
    i4			hi_done = FALSE;



    rel = global->ops_cstate.opc_subqry->ops_vars.opv_base->
		    opv_rt[cnode->opc_cco->opo_union.opo_orig]->opv_grv->opv_relation;

    /* allocate an qekor; */
    qekor = (QEF_KOR *) opu_qsfmem(global, sizeof (QEF_KOR));
    qekor->kor_next = NULL;
    qekor->kor_keys = NULL;
    qekor->kor_numatt = 0;
    /* bug 45912 */
	qekor->kor_id = qp->qp_kor_cnt++;

    /* for (each opkand) */
    end_qekand = NULL;
    for (kandi = 0; kandi < cqual->opc_qsinfo.opc_nkeys; kandi += 1)
    {
	kand = &opkor->opc_ands[kandi];
	khi = kand->opc_khi;
	klo = kand->opc_klo;
	kphi = (OPC_QCONST *)kand->opc_kphi;
	kplo = (OPC_QCONST *)kand->opc_kplo;

	if (cqual->opc_qsinfo.opc_tidkey)
	{
	    tidatt.att_number = DB_IMTID;
	    tidatt.att_type = DB_TID8_TYPE;
	    tidatt.att_width = DB_TID8_LENGTH;
	    tidatt.att_prec = 0;
	    tidatt.att_flags = 0;
	    tidatt.att_key_seq_number = 1;

	    att = &tidatt;
	}
	else
	{
	    att = rel->rdr_attr[rel->rdr_keys->key_array[kandi]];
	}

	if (khi == NULL && kphi == NULL)
	{
	    hi_done = TRUE;
	}
	if (klo == NULL && kplo == NULL)
	{
	    lo_done = TRUE;
	}

	if (hi_done == TRUE && lo_done == TRUE)
	{
	    break;
	}

	/* make an qekand for this opkand; */
	if (hi_done == lo_done && khi == klo &&
				    kand->opc_krange_type != ADE_2RANGE_YES
	    )
	{
	    opc_qeoneand(global, cnode, cqual, att, 
					    kand, kandi, qekor, &end_qekand);
	}
	else
	{
	    opc_qerangeand(global, cnode, cqual, att, 
					    kand, kandi, qekor, &end_qekand);
	}
    }

    *nkey_att = kandi;
    return(qekor);
}

/*{
** Name: OPC_QEONEAND	- Build an QEF_AND struct for an exact match.
**
** Description:
**      This routine translates an OPC_KAND struct into a QEF_AND struct.
**      A QEF_AND struct describes one or more key values, one of which
**	will be used for a key attribute. In this routine, we build
**      a QEF_AND struct for an exact match type of key. 
[@comment_line@]...
**
** Inputs:
**  global -
**	This is the query wide state variable for OPF/OPC. This will be used
**	to locate the current subqry, and qp structs, as well as a parameter
**	to all OPC routines.
**  cnode -
**	This is used as a parameter to other routines, primarily for its
**	eqc and att information.
**  att -
**	This is a description of the key attribute that the QEF_AND is being
**	built around.
**  opkand -
**	This is the OPC_KAND that will serve as the primary source of
**	information for the QEF_AND.
**  keyno -
**	The key number for the range that is being built.
**  qekor -
**	This is the QEF_KOR that is being currently built.
**
** Outputs:
**  qekor->kor_keys -
**	The QEF_AND struct that is build will be attached to this linked list.
**
**	Returns:
**	    nothing
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      22-aug-86 (eric)
**          written
**	17-jul-89 (jrb)
**	    Initialize precision fields properly for decimal project.
**	06-feb-91 (stec)
**	    Cleaned up code.
**      18-mar-92 (anitap)
**          Added initialization for opr_prec and opr_len to fix bug 42363.
**          For details see opc_cqual() in OPCQUAL.C.
**	26-mar-96 (inkdo01)
**	    Replaces ADE_KEYBLD by much faster ADE_MECOPY for some types
**	    of simple key building operations.
**	17-jul-2006 (gupsh01)
**	    Added support for new ANSI datetime datatypes in opc_qeoneand.
[@history_template@]...
*/
static VOID
opc_qeoneand(
	OPS_STATE	*global,
	OPC_NODE	*cnode,
	OPC_QUAL	*cqual,
	DMT_ATT_ENTRY	*att,
	OPC_KAND	*opkand,
	i4		keyno,
	QEF_KOR		*qekor,
	QEF_KAND	**end_qekand )
{
    OPS_SUBQUERY	*subqry = global->ops_cstate.opc_subqry;
    QEF_QP_CB		*qp = global->ops_cstate.opc_qp;
    QEF_KAND		*qekand;
    QEF_KATT	        *qekatt;
    i4			align;
    ADE_OPERAND		adeops[5];
    OPC_ADF		*cadf = &subqry->ops_compile.opc_mkey;
    DB_DATA_VALUE	kdv;
    OPC_QCONST		*qconst;
    i4			parmno;

    /* Allocate and initialize a qekand */
    qekand = (QEF_KAND *) opu_qsfmem(global, sizeof (QEF_KAND));
    qekand->kand_type = QEK_EQ;
    qekand->kand_next = NULL;
    qekand->kand_attno = att->att_number;
    qekand->kand_keys = NULL;

    /* Link qekand into qekor */
    if (*end_qekand == NULL)
    {
	qekor->kor_keys = qekand;
    }
    else
    {
	(*end_qekand)->kand_next = qekand;
    }
    *end_qekand = qekand;
    qekor->kor_numatt++;

    /* Allocate and initialize a qef_katt struct */
    qekatt = (QEF_KATT *) opu_qsfmem(global, sizeof (QEF_KATT));
    qekatt->attr_attno = att->att_number;
    qekatt->attr_type = att->att_type;
    qekatt->attr_length = att->att_width;
    qekatt->attr_prec = att->att_prec;
    qekatt->attr_next = NULL;
    qekatt->attr_operator = DMR_OP_EQ;

    qekand->kand_keys = qekatt;

    /* Locate the interesting OPC_QCONST. */
    if (opkand->opc_khi != NULL)
    {
	qconst = opkand->opc_khi;
    }
    else
    {
	parmno = BTnext(-1, opkand->opc_kphi, cqual->opc_qsinfo.opc_hiparm);
	if (global->ops_procedure->pst_isdbp == TRUE)
	{
	    qconst = cqual->opc_kconsts[keyno].opc_dbpvars[parmno];
	}
	else
	{
	    qconst = cqual->opc_kconsts[keyno].opc_qparms[parmno];
	}
    }

    /* Initialize key info for QEF. */
    if (qconst->opc_dshlocation >= 0)
    {
	/*
	** When this condition is true, then we have already processed this
	** constant. This is a minor optimization for the exact match case 
	** when low and high keys point at the same constant.
	*/
	qekatt->attr_value = qconst->opc_dshlocation;
    }
    else
    {
	/*
	** This constant needs to be processed here; we need to ensure
	** that OPC_QCONST constants which represent dates get processed
	** in the same way as repeat query parms, i.e., at runtime.
	*/

	qekatt->attr_value = qp->qp_key_sz;
	qconst->opc_dshlocation = qp->qp_key_sz;

	if (qconst->opc_tconst_type == PST_USER)
	{
	    STRUCT_ASSIGN_MACRO(qconst->opc_keydata.opc_data, kdv);

	    /* Compile the constant in. */
	    opc_adconst(global, &subqry->ops_compile.opc_mkey, &kdv,
		&adeops[1], qconst->opc_qlang, ADE_SMAIN);
	    adeops[0].opr_dt = att->att_type;
	    adeops[0].opr_len = att->att_width;
	    adeops[0].opr_prec = att->att_prec;
	    adeops[0].opr_base = cadf->opc_key_base;
	    adeops[0].opr_offset = qp->qp_key_sz;

	    /* Move the constant into the DSH key buffer. */
	    opc_adinstr(global, &subqry->ops_compile.opc_mkey, 
		ADE_MECOPY, ADE_SMAIN, 2, adeops, 1);
	}
	else
	{
	    /*
	    ** All constants handled in this block of the code
	    ** are to be handled at runtime, therefore ADE_KEYBLD
	    ** instruction is to be compiled in.
	    */

	    i4		dummy;
	    PST_QNODE	*root;

	    root = qconst->opc_keydata.opc_pstnode;

	    adeops[3].opr_dt = root->pst_sym.pst_dataval.db_datatype;
	    adeops[3].opr_len = OPC_UNKNOWN_LEN;
            adeops[3].opr_prec = OPC_UNKNOWN_PREC;
	    opc_cqual(global, cnode, root, &subqry->ops_compile.opc_mkey, 
		ADE_SMAIN, &adeops[3], &dummy);

	    /* First, provide a place for the type of key */
	    adeops[0].opr_dt = DB_INT_TYPE;
	    adeops[0].opr_prec = 0;
	    adeops[0].opr_len = 2;
	    adeops[0].opr_prec = 0;
	    adeops[0].opr_base = cadf->opc_key_base;
	    adeops[0].opr_offset = 0;

	    /* Next, the low or high key */
	    adeops[1].opr_dt     = adeops[2].opr_dt = att->att_type;
	    adeops[1].opr_len    = adeops[2].opr_len = att->att_width;
	    adeops[1].opr_prec   = adeops[2].opr_prec = att->att_prec;
	    /* In the midst of this, see if MECOPY can still be used */
	    if (adeops[3].opr_dt == adeops[2].opr_dt && 
		adeops[3].opr_len == adeops[2].opr_len &&
		adeops[3].opr_prec == adeops[2].opr_prec &&
		adeops[3].opr_dt != DB_DTE_TYPE &&
		adeops[3].opr_dt != DB_ADTE_TYPE &&
		adeops[3].opr_dt != DB_TMWO_TYPE &&
		adeops[3].opr_dt != DB_TMW_TYPE &&
		adeops[3].opr_dt != DB_TME_TYPE &&
		adeops[3].opr_dt != DB_TSWO_TYPE &&
		adeops[3].opr_dt != DB_TSW_TYPE &&
		adeops[3].opr_dt != DB_TSTMP_TYPE &&
		adeops[3].opr_dt != DB_INYM_TYPE &&
		adeops[3].opr_dt != DB_INDS_TYPE &&
		-adeops[3].opr_dt != DB_DTE_TYPE
		-adeops[3].opr_dt != DB_ADTE_TYPE &&
		-adeops[3].opr_dt != DB_TMWO_TYPE &&
		-adeops[3].opr_dt != DB_TMW_TYPE &&
		-adeops[3].opr_dt != DB_TME_TYPE &&
		-adeops[3].opr_dt != DB_TSWO_TYPE &&
		-adeops[3].opr_dt != DB_TSW_TYPE &&
		-adeops[3].opr_dt != DB_TSTMP_TYPE &&
		-adeops[3].opr_dt != DB_INYM_TYPE &&
		-adeops[3].opr_dt != DB_INDS_TYPE)
	    {
	    	adeops[2].opr_base   = cadf->opc_key_base;
	    	adeops[2].opr_offset = qp->qp_key_sz;
	    	/* Move the parameter into the DSH key buffer. */
	    	opc_adinstr(global, &subqry->ops_compile.opc_mkey, 
		    ADE_MECOPY, ADE_SMAIN, 2, &adeops[2], 1);
		goto quickexit;  /* temporary */
	    }

	    adeops[1].opr_base   = cadf->opc_key_base;
	    adeops[1].opr_offset = qp->qp_key_sz;
	    adeops[2].opr_base   = ADE_NULLBASE;
	    adeops[2].opr_offset = 0;

	    /* Finally, set the overloaded operator */
	    adeops[4].opr_len    = opkand->opc_krange_type;
	    adeops[4].opr_prec   = 0;
	    adeops[4].opr_base   = 0;
	    adeops[4].opr_offset = 0;
	    adeops[4].opr_dt = 
		    qconst->opc_qop->pst_sym.pst_value.pst_s_op.pst_opno;

	    /*
	    ** If necessary compile instructions indicating 
	    ** presence of an escape char.
	    */
	    if (qconst->opc_qop->pst_sym.pst_value.pst_s_op.pst_isescape == 
								PST_HAS_ESCAPE)
	    {
		ADE_OPERAND	escape_ops[1];
		DB_DATA_VALUE   escape_dv;

		escape_dv.db_datatype = DB_CHA_TYPE;
		escape_dv.db_length = 1;
		escape_dv.db_prec = 0;
		escape_dv.db_data = 
		    &qconst->opc_qop->pst_sym.pst_value.pst_s_op.pst_escape;

		opc_adconst(global, cadf, &escape_dv, &escape_ops[0],
		    DB_SQL, ADE_SMAIN);
		opc_adinstr(global, &subqry->ops_compile.opc_mkey, 
		    ADE_ESC_CHAR, ADE_SMAIN, 1, escape_ops, 0);
	    }

	    /* Now lets generate the KEYBLD instruction */
	    opc_adinstr(global, &subqry->ops_compile.opc_mkey, 
		ADE_KEYBLD, ADE_SMAIN, 5, adeops, 2);
	}
quickexit:

	/*
	** Increment qp_key_sz so that the next key value required
	** won't stomp on the current value and will fall on a good 
	** alignment boundary for this machine.
	*/
	align = att->att_width % sizeof (ALIGN_RESTRICT);
	if (align != 0)
	    align = sizeof (ALIGN_RESTRICT) - align;
	qp->qp_key_sz += (i4) att->att_width + align;
    }

    return;
}

/*{
** Name: OPC_QERANGEAND	- Build an QEF_AND struct for a range match.
**
** Description:
**      This routine translates an OPC_KAND struct into a QEF_AND struct. 
**      This is exactly the same as opc_qeoneand(), except that this will 
**      build a QEF_AND struct for non-exact matches. We could get by with 
**      only opc_qerangeand() and not have opc_qeoneand(), except that 
**      performance would suffer. 
[@comment_line@]...
**
** Inputs:
**  global -
**	This is the query wide state variable for OPF/OPC. It is used to find 
**	the current subquery and qp structs, and as a parameter to all other
**	OPC routines.
**  cnode -
**	This is used as a parameter to other routines mainly for it's eqc and
**	att information.
**  att -
**	This is a description of the key attribute that we are building the
**	QEN_AND struct for.
**  opkand -
**	This is the main source of information when we build the QEN_AND
**	struct.
**  keyno -
**	The number of the key attribute that is being built.
**  qekor -
**	This is the QEN_KOR struct that we are in the middle of building.
**
** Outputs:
**  qekor->kor_keys -
**	The QEF_AND struct that is build will be attached to this linked list.
**
**	Returns:
**	    nothing
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      22-aug-86 (eric)
**          written
**	17-jul-89 (jrb)
**	    Initialize precision fields correctly for decimal.
**	31-oct-90 (stec)
**	    Fixed bug 32619 by changing the language argument in the call
**	    to opc_adconst(). Old code assumed that qconst->opc_qop pointed
**	    to a constant node (which was not true) and the language value
**	    was obtained from there. The fix is to obtain language value from
**	    qconst->opc_qlang.
**	06-feb-91 (stec)
**	    Cleaned up code.
**      18-mar-92 (anitap)
**          Added initializations for opr_prec and opr_len to fix bug 42363.
**          See opc_cqual() in OPCQUAL.C for details.
[@history_template@]...
*/
static VOID
opc_qerangeand(
	OPS_STATE	*global,
	OPC_NODE	*cnode,
	OPC_QUAL	*cqual,
	DMT_ATT_ENTRY	*att,
	OPC_KAND	*opkand,
	i4		keyno,
	QEF_KOR		*qekor,
	QEF_KAND	**end_qekand )
{
    OPS_SUBQUERY	*subqry = global->ops_cstate.opc_subqry;
    QEF_QP_CB		*qp = global->ops_cstate.opc_qp;
    QEF_KAND		*hikand;
    QEF_KAND		*lokand;
    QEF_KATT	        *qekatt;
    i4			align;
    ADE_OPERAND		adeops[5];
    OPC_ADF		*cadf = &subqry->ops_compile.opc_mkey;
    DB_DATA_VALUE	kdv;
    OPC_QCONST		*qconst;
    OPC_KCONST		*kconst;
    i4			dummy;
    PTR			kpmap;	    /* map of both opc_kphi and opc_kplo */
    i4			szkparm_byte =
			    (cqual->opc_qsinfo.opc_hiparm / BITSPERBYTE) + 1;
    i4			parmno;

    /* Allocate and initialize a qekand corresponding to low key */
    if (opkand->opc_klo != NULL || opkand->opc_kplo != NULL)
    {
	lokand = (QEF_KAND *) opu_qsfmem(global, sizeof (QEF_KAND));
	lokand->kand_type = QEK_MIN;
	lokand->kand_next = NULL;
	lokand->kand_attno = att->att_number;
        lokand->kand_keys = NULL;

	/* Link qekand into qekor */
	if (*end_qekand == NULL)
	{
	    qekor->kor_keys = lokand;
	}
	else
	{
	    (*end_qekand)->kand_next = lokand;
	}
	*end_qekand = lokand;
	qekor->kor_numatt++;
    }

    /* Allocate and initialize a qekand corresponding to high key */
    if (opkand->opc_khi != NULL || opkand->opc_kphi != NULL)
    {
	hikand = (QEF_KAND *) opu_qsfmem(global, sizeof (QEF_KAND));
	hikand->kand_type = QEK_MAX;
	hikand->kand_next = NULL;
	hikand->kand_attno = att->att_number;
        hikand->kand_keys = NULL;

	/* link qekand into qekor */
	if (*end_qekand == NULL)
	{
	    qekor->kor_keys = hikand;
	}
	else
	{
	    (*end_qekand)->kand_next = hikand;
	}
	*end_qekand = hikand;
	qekor->kor_numatt++;
    }

    /*
    ** In the code below we will first deal with non_repeat keys
    ** and then with repeat type keys. The difference is that the 
    ** second type of keys has to be built at runtime via execution
    ** of the ADE_KEYBLD instruction.
    */

    /* Allocate and initialize a qef_katt struct for the non-repeat low key */
    if (opkand->opc_klo != NULL)
    {
	qekatt = (QEF_KATT *) opu_qsfmem(global, sizeof (QEF_KATT));
	qekatt->attr_attno = att->att_number;
	qekatt->attr_type = att->att_type;
	qekatt->attr_length = att->att_width;
	qekatt->attr_prec = att->att_prec;
	qekatt->attr_next = lokand->kand_keys;
	qekatt->attr_operator = DMR_OP_GTE;

	lokand->kand_keys = qekatt;

	if (opkand->opc_klo->opc_dshlocation >= 0)
	{
	    /*
	    ** When this condition is true, then we have already processed
	    ** this constant. This is a minor optimization for the exact 
	    ** match case when low and high keys point to the same constant.
	    */
	    qekatt->attr_value = opkand->opc_klo->opc_dshlocation;
	}
	else
	{
	    /* This constant needs to be processed here. */

	    opkand->opc_klo->opc_dshlocation = qp->qp_key_sz; 
	    qekatt->attr_value = qp->qp_key_sz; 

	    STRUCT_ASSIGN_MACRO(opkand->opc_klo->opc_keydata.opc_data, kdv);

	    /* Compile the constant in. */
	    opc_adconst(global, &subqry->ops_compile.opc_mkey, 
		&kdv, &adeops[1], opkand->opc_klo->opc_qlang, ADE_SMAIN);

	    adeops[0].opr_dt = att->att_type;
	    adeops[0].opr_prec = att->att_prec;
	    adeops[0].opr_len = att->att_width;
	    adeops[0].opr_prec = att->att_prec;
	    adeops[0].opr_base = cadf->opc_key_base;
	    adeops[0].opr_offset = qp->qp_key_sz;

	    /* Move the constant into the DSH key buffer. */
	    opc_adinstr(global, &subqry->ops_compile.opc_mkey, 
		ADE_MECOPY, ADE_SMAIN, 2, adeops, 1);

	    /*
	    ** Increment qp_key_sz so that the next key value required
	    ** won't stomp on the current value and will fall on a 
	    ** good alignment boundary for this machine.
	    */
	    align = att->att_width % sizeof (ALIGN_RESTRICT);
	    if (align != 0)
		align = sizeof (ALIGN_RESTRICT) - align;
	    qp->qp_key_sz += (i4) att->att_width + align;
	}
    }

    /* Allocate and initialize a qef_katt struct for the non-repeat high key */
    if (opkand->opc_khi != NULL)
    {
	qekatt = (QEF_KATT *) opu_qsfmem(global, sizeof (QEF_KATT));
	qekatt->attr_attno  = att->att_number;
	qekatt->attr_type   = att->att_type;
	qekatt->attr_length = att->att_width;
	qekatt->attr_prec   = att->att_prec;
	qekatt->attr_next   = hikand->kand_keys;
	qekatt->attr_operator = DMR_OP_LTE;

	hikand->kand_keys = qekatt;

	if (opkand->opc_khi->opc_dshlocation >= 0)
	{
	    /*
	    ** When this condition is true, then we have already processed
	    ** this constant. This is a minor optimization for the exact 
	    ** match case when low and high keys point to the same constant.
	    */
	    qekatt->attr_value = opkand->opc_khi->opc_dshlocation;
	}
	else
	{
	    /* This constant needs to be processed here. */

	    opkand->opc_khi->opc_dshlocation = qp->qp_key_sz; 
	    qekatt->attr_value = qp->qp_key_sz; 

	    STRUCT_ASSIGN_MACRO(opkand->opc_khi->opc_keydata.opc_data, kdv);

	    /* Compile the constant in. */
	    opc_adconst(global, &subqry->ops_compile.opc_mkey, 
		&kdv, &adeops[1], opkand->opc_khi->opc_qlang, ADE_SMAIN);

	    adeops[0].opr_dt = att->att_type;
	    adeops[0].opr_prec = att->att_prec;
	    adeops[0].opr_len = att->att_width;
	    adeops[0].opr_prec = att->att_prec;
	    adeops[0].opr_base = cadf->opc_key_base;
	    adeops[0].opr_offset = qp->qp_key_sz;

	    /* Move the constant into the DSH key buffer. */
	    opc_adinstr(global, &subqry->ops_compile.opc_mkey, 
		ADE_MECOPY, ADE_SMAIN, 2, adeops, 1);

	    /*
	    ** Increment qp_key_sz so that the next key value required
	    ** won't stomp on the current value and will fall on a 
	    ** good alignment boundary for this machine.
	    */
	    align = att->att_width % sizeof (ALIGN_RESTRICT);
	    if (align != 0)
		align = sizeof (ALIGN_RESTRICT) - align;
	    qp->qp_key_sz += (i4) att->att_width + align;
	}
    }


    /*
    ** Now that we have the non-repeat constants out of the way, let's
    ** build the key for the repeat query parameters. The first step is
    ** to find the list of parameters.
    */

    kconst = &cqual->opc_kconsts[keyno];

    if (opkand->opc_kphi != NULL || opkand->opc_kplo != NULL)
    {
	/*
	** We have determined that at least one of low key, high key
	** bitmaps is non-zero, which means that there are parms that
	** we should deal with here.
	*/

	/* Describe location for the type of key */
	adeops[0].opr_dt   = DB_INT_TYPE;
	adeops[0].opr_len  = 2;
	adeops[0].opr_prec = 0;
	adeops[0].opr_base = cadf->opc_key_base;
	adeops[0].opr_offset = 0;

	/* Describe datatype for the low and high key */
	adeops[1].opr_dt   = adeops[2].opr_dt   = att->att_type;
	adeops[1].opr_len  = adeops[2].opr_len  = att->att_width;
	adeops[1].opr_prec = adeops[2].opr_prec = att->att_prec;

	/* Set the overloaded operator */
	adeops[4].opr_base = 0;
	adeops[4].opr_prec = 0;
	adeops[4].opr_offset = 0;

	/*
	** Let's figure out what parameters will be 
	** used for the hi and low range of this key.
	*/
	kpmap = opu_Gsmemory_get(global, szkparm_byte);
	if (opkand->opc_kphi != NULL)
	{
	    MEcopy(opkand->opc_kphi, szkparm_byte, kpmap);

	    if (opkand->opc_kplo != NULL)
	    {
		BTor(cqual->opc_qsinfo.opc_hiparm, opkand->opc_kplo, kpmap);
	    }
	}
	else
	{
	    MEcopy(opkand->opc_kplo, szkparm_byte, kpmap);
	}

	/*
	** kpmap represents now a bitmap of low
	** and high key bitmaps OR'ed together.
	*/
	for (parmno = -1; 
	     (parmno = BTnext(parmno, kpmap, cqual->opc_qsinfo.opc_hiparm))
									!= -1;
	    )
	{	
	    /* Identify the interesting OPC_QCONST. */
	    if (global->ops_procedure->pst_isdbp == TRUE)
	    {
		qconst = kconst->opc_dbpvars[parmno];
	    }
	    else
	    {
		qconst = kconst->opc_qparms[parmno];
	    }

	    /*
	    ** If we are building a high key, and this parm is set
	    ** in the high key bitmap, then describe it to QEF.
	    */
	    if (opkand->opc_kphi != NULL && 
		BTtest(parmno, opkand->opc_kphi) == TRUE)
	    {
		qekatt = (QEF_KATT *) opu_qsfmem(global, sizeof (QEF_KATT));
		qekatt->attr_attno = att->att_number;
		qekatt->attr_type = att->att_type;
		qekatt->attr_length = att->att_width;
		qekatt->attr_prec  = att->att_prec;
		qekatt->attr_value = qp->qp_key_sz;
		qekatt->attr_next = hikand->kand_keys;
		qekatt->attr_operator = DMR_OP_LTE;

		hikand->kand_keys = qekatt;

		/* Describe location for the high key */
		adeops[2].opr_base = cadf->opc_key_base;
		adeops[2].opr_offset = qp->qp_key_sz;

		/*
		** Increment qp_key_sz so that the next key value required
		** won't stomp on the current value and will fall on a good
		** alignment boundary for this machine.
		*/
		align = att->att_width % sizeof (ALIGN_RESTRICT);
		if (align != 0)
		    align = sizeof (ALIGN_RESTRICT) - align;
		qp->qp_key_sz += (i4) att->att_width + align;
	    }
	    else
	    {
		adeops[2].opr_base = ADE_NULLBASE;
		adeops[2].opr_offset = 0;
	    }

	    /*
	    ** If we are building a low key, and this parm is set
	    ** in the low key bitmap, then describe it to QEF.
	    */
	    if (opkand->opc_kplo != NULL && 
		BTtest(parmno, opkand->opc_kplo) == TRUE)
	    {
		qekatt = (QEF_KATT *) opu_qsfmem(global, sizeof (QEF_KATT));
		qekatt->attr_attno = att->att_number;
		qekatt->attr_type = att->att_type;
		qekatt->attr_length = att->att_width;
		qekatt->attr_prec = att->att_prec;
		qekatt->attr_value = qp->qp_key_sz;
		qekatt->attr_next = lokand->kand_keys;
		qekatt->attr_operator = DMR_OP_GTE;

		lokand->kand_keys = qekatt;

		/* Describe location for the low key */
		adeops[1].opr_base = cadf->opc_key_base;
		adeops[1].opr_offset = qp->qp_key_sz;

		/*
		** Increment qp_key_sz so that the next key value required
		** won't stomp on the current value and will fall on a good
		** alignment boundary for this machine.
		*/
		align = att->att_width % sizeof (ALIGN_RESTRICT);
		if (align != 0)
		    align = sizeof (ALIGN_RESTRICT) - align;
		qp->qp_key_sz += (i4) att->att_width + align;
	    }
	    else
	    {
		adeops[1].opr_base = ADE_NULLBASE;
		adeops[1].opr_offset = 0;
	    }

	    /*
	    ** If this operand won't do anything,
	    ** then don't bother compiling it.
	    */
	    if (adeops[1].opr_base == ADE_NULLBASE && 
		adeops[2].opr_base == ADE_NULLBASE)
	    {
		continue;
	    }

	    /* Describe the value for which the key is to be built. */
	    adeops[3].opr_dt = qconst->opc_keydata.opc_pstnode->
		pst_sym.pst_dataval.db_datatype;
	    adeops[3].opr_len = OPC_UNKNOWN_LEN;
            adeops[3].opr_prec = OPC_UNKNOWN_PREC;

	    opc_cqual(global, cnode, qconst->opc_keydata.opc_pstnode,
		&subqry->ops_compile.opc_mkey, ADE_SMAIN, &adeops[3], &dummy);

	    /* Set the overloaded operator */
	    if (qconst->opc_key_type == ADC_KEXACTKEY ||
		qconst->opc_key_type == ADC_KNOMATCH)
	    {
		adeops[4].opr_len = ADE_3RANGE_NO;
	    }
	    else if (qconst->opc_key_type == ADC_KNOKEY)
	    {
		adeops[4].opr_len = ADE_1RANGE_DONTCARE;
	    }
	    else
	    {
		adeops[4].opr_len = ADE_2RANGE_YES;
	    }
		    
	    adeops[4].opr_dt = qconst->opc_qop->
		pst_sym.pst_value.pst_s_op.pst_opno;

	    /*
	    ** If necessary compile instructions indicating 
	    ** presence of an escape char.
	    */
	    if (qconst->opc_qop->
		pst_sym.pst_value.pst_s_op.pst_isescape == PST_HAS_ESCAPE)
	    {
		ADE_OPERAND	escape_ops[1];
		DB_DATA_VALUE   escape_dv;

		escape_dv.db_datatype = DB_CHA_TYPE;
		escape_dv.db_length = 1;
		escape_dv.db_prec = 0;
		escape_dv.db_data = 
		    &qconst->opc_qop->pst_sym.pst_value.pst_s_op.pst_escape;
		opc_adconst(global, cadf, &escape_dv, &escape_ops[0],
		    qconst->opc_qlang, ADE_SMAIN);  /* Fix for bug 32619 */
		opc_adinstr(global, &subqry->ops_compile.opc_mkey, 
		    ADE_ESC_CHAR, ADE_SMAIN, 1, escape_ops, 0);
	    }

	    /* Compile KEYBLD instruction */
	    opc_adinstr(global, &subqry->ops_compile.opc_mkey, 
		ADE_KEYBLD, ADE_SMAIN, 5, adeops, 2);
	}
    }

    return;
}

/*{
** Name: OPC_VALID	- Allocate and init a QEN_VALID struct
**
** Description:
**       This routine allocates and initializes a QEN_VALID struct for a 
**      given relation, and if necessary adds a resource element to the
**	resource list on the qp header. The QEN_VALID struct is placed on a 
**	linked list of valid structs off of the top most action header that
**	uses it. In other words, valid structs for actions that are below
**	QEN_QP nodes are put on its top most parent action. Ignoring 
**	sub-actions, this means that the valid list holds all the table 
**	references (ie. correlation variables) that are necessary for that 
**	action. 
**	The resource list hangs off of the QEF_QP_CB header and holds a
**	list of tables (and other objects) that are used by the query plan.
**	The differences between the resource and valid lists are:
**	    - There is only one resource list per qp, where as there is
**	      one valid list per qp.
**	    - A table is listed only once on the resource list, where as
**	      a table can be listed many times on a valid list (depending
**	      on the number of correlation variables that refer to that table).
**	The resource list is used by QEF to verify that the query plan is
**	still legal to run. The valid list is used to insure that the
**	needed open tables are available for an action. Given this, valid
**	lists are no longer the best name for their function. The name
**	hasn't been changed due to the lateness of the cycle and the number of
**	files changed.
**
**	In the past, the valid list served both functions (query plan
**	correctness, and opening tables). In this scheme, which action header 
**	was used depended on whether a relation was temporary or not. 
**      If it was temporary, ie. it is created during the execution of this 
**      qp,  then the valid struct was placed on the top most of the current
**      action. If, on the other hand, we were dealing with a user relation 
**      then we put the QEN_VALID struct on the first subquery struct that 
**	was to be executed. This insured early invalidation of query plans. 
**
**	The current scheme has the following advantages:
**	    - A minimum number of tables are opened to verify query plans
**	    - Opening tables for actions is simplified, esp in regard
**	      to maintain the statement number in the dmt_cb.
**	    - Sharing open dmt_cb's is facilitated.
**
** Inputs:
**	global -
**	    This is the query wide state variable for OPF/OPC. It is used
**	    to locate the subquery structs that we are interested in, and as
**	    a parameter to other OPC routines.
**	grv -
**	    This describes the relation that we are building the valid struct
**	    for. It also helps us locate the other valid structs that have
**	    built for this range variable.
**	lock_mode -
**	    The tells whether the relation should be have a shared or exclusive
**	    lock on it. This does not tell whether page or table level locking
**	    should be used.
**	npage_est -
**	    This is an estimate on the number of pages in the relation that
**	    will be touched.
**
** Outputs:
**  global->ops_cstate.opc_topahd -
**  global->ops_cstate.opc_firstahd -
**	As I described above, the QEN_VALID struct that is built will be put
**	on the linked list of one of the two subqry structs list above.
**
**	Returns:
**	    The valid struct that is built.
**	Exceptions:
**	    none except for those raised by opx_error and opx_verror.
**
** Side Effects:
**	    none
**
** History:
**      7-sept-86 (eric)
**          written
**	13-jul-90 (stec)
**	    Changed npage_est argument from i4  to i4.
**	16-apr-91 (davebf)
**	    Added parm to opc_valid() call in order to enable vl_size_sensitive
**      20-apr-92 (schang)
**          GW 6.5 merge.
**	    20-may-91 (rickh)
**		    Merged in a fix by Linda.  For non-relational gateways (e.g. and
**		    i.e., RMSGW), only the last record stream should be opened for
**		    write.  This prevents self deadlocks when RMS is asked to scan
**		    the file many ways simultaneously during a query.  This situation
**		    occurs when using read/write cursors.
**	12-feb-93 (jhahn)
**	    Added support for statement level rules. (FIPS)
**	02-apr-93 (jhahn)
**	    Bug fix for support of statement level rules. (FIPS)
**      5-oct-95 (eric)
**          Added support for resource lists in qps. The resource
**      19-dec-94 (sarjo01) from 18-feb-94 (markm)
**          If the query is an update/delete make sure that we don't do "dirty"
**          reads on the secondary by setting the "mustlock" flag (58681).
**	11-apr-95 (pchang)
**	    The above fix would incur unnecessary IS locks on the source table
**	    of a "UPDATE <target> FROM <source>" query, if access to the source
**	    table was via a secondary index (64871).
**	    To avoid that we need to check that the relation being validated is
**	    a secondary index of the target table before enforcing readlock.
**	11-feb-97 (inkdo01)
**	    Changes for MS Access OR transformations.
**	12-nov-1998 (somsa01)
**	    Set QEF_SESS_TEMP if this is a Global Temporary Table.
**	    (Bug #94059)
**	3-dec-03 (inkdo01)
**	    Changes for table partitioning.
**	19-jan-04 (inkdo01)
**	    Copy partition DB_TAB_IDs into QEF_VALID.
**	11-may-04 (inkdo01)
**	    Add one more DMR_CB slot in cb array for partitioned masters.
[@history_line@]...
[@history_template@]...
*/
QEF_VALID *
opc_valid(
		OPS_STATE	*global,
		OPV_GRV		*grv,
		i4		lock_mode,
		i4		npage_est,
		bool		size_sensitive)
{
    QEF_QP_CB		*qp = global->ops_cstate.opc_qp;
    QEF_VALID		*top_vl;
    QEF_VALID		*ret_vl;
    QEF_RESOURCE	*resource;
    QEF_TBL_RESOURCE	*qr_tbl;
    i4		new_rw;
    RDR_INFO		*rel = grv->opv_relation;
    bool		gateway;
    i4			tbl_type;
    PTR			temp_id;
    i4			i;
    bool                mustlock = FALSE;
    QEF_AHD             *ahd;
    QEF_MEM_CONSTTAB	*cnsttab_p;
    DMT_PHYS_PART	*dmt_ppp;

    /* Look to see if our table is already on the qp resource list */
    resource = opc_get_resource(global, grv, &tbl_type, &temp_id, &cnsttab_p);
    qr_tbl = &resource->qr_resource.qr_tbl;

    /* If we didn't find a resource for the table then add one */
    if (resource == NULL)
    {
	resource = (QEF_RESOURCE *) opu_qsfmem(global, sizeof (QEF_RESOURCE));
	qr_tbl = &resource->qr_resource.qr_tbl;

	resource->qr_next = qp->qp_resources;
	resource->qr_type = QEQR_TABLE;
	resource->qr_id_resource = qp->qp_cnt_resources;
	qr_tbl->qr_valid = NULL;
	qr_tbl->qr_lastvalid = NULL;
	qr_tbl->qr_tbl_type = tbl_type;
	qr_tbl->qr_temp_id = temp_id;
	qr_tbl->qr_cnsttab_p = cnsttab_p;

	qp->qp_resources = resource;
	qp->qp_cnt_resources += 1;
    }

    /* schang : set gateway flag properly */
    switch (rel->rdr_rel->tbl_relgwid) 
    {
        case DMGW_RMS :
            gateway = TRUE;
            break;
        default :
            gateway = FALSE;
    }

    /* First, lets figure out what the new lock mode and read-write status
    ** should be based on the the estimated pages touched and the type
    ** of lock desired.
    */
    if (lock_mode == DMT_S || lock_mode == DMT_IS)
    {
	new_rw = DMT_A_READ;
    }
    else
    {
	new_rw = DMT_A_WRITE;
    }

    if (grv->opv_topvalid != NULL)
    {
	top_vl = grv->opv_topvalid;

	if (!gateway && global->ops_qheader->pst_updtmode != PST_UNSPECIFIED)
	{
	    /* If we're going to update this relation, then all read
	    ** accesses should use exclusive locks to reduce deadlocks
	    ** unless the cursor was opened without a for update clause
	    ** (or this is a gateway table).
	    */
	    if (top_vl->vl_rw == DMT_A_WRITE && new_rw == DMT_A_READ)
	    {
		new_rw = DMT_A_WRITE;
	    }
        }
	if (npage_est < top_vl->vl_est_pages)
	{
	    npage_est = top_vl->vl_est_pages;
	}

	for ( ; 
		top_vl != NULL &&
		    global->ops_qheader->pst_updtmode != PST_UNSPECIFIED;
		top_vl = top_vl->vl_alt
	    )
	{
	    if (!gateway)
	    {
		top_vl->vl_rw = new_rw;
	    }
	    else
	    {
		/*
		** For gateways (really, for now, RMSGW), we only want the last
		** instance to get write mode.  If we have a write mode coming
		** in, update all other instances of this range variable to be
		** read mode.  This is exactly opposite to what Ingres does for
		** its native tables:  they all get the maximum lockmode if any
		** instance is open for write.  The trick for us is to determine
		** which instance should be left open for write; the assumption
		** here is that only the last instance to come in with write
		** mode is the one to leave in that mode.
		*/
		if (new_rw == DMT_A_WRITE)
		    top_vl->vl_rw = DMT_A_READ;
	    }	/* end if !gateway */

	    top_vl->vl_est_pages = npage_est;
	}
    }
    if (rel->rdr_rel->tbl_id.db_tab_base == 0 &&
            rel->rdr_rel->tbl_id.db_tab_index == 0
        )
    {
        /* if this is a temporary relation then we want to put the new
        ** valid list entry on the top most action header of the current
        ** action header. This ensures that the relation gets opened
        ** after it gets created. This also avoids having valid list
        ** entries on action headers that are below QEN_QP nodes, which
        ** QEF doesn't like.
        */
        ahd = global->ops_cstate.opc_topahd;
    }
    else
    {
        /* If this is a user relation, then lets put the new valid list
        ** entry on the first action header. This ensures that the relation
        ** is validated by time stamp and, hence, the query plan can be
        ** kicked out as early as possible.
        */
        ahd = global->ops_cstate.opc_firstahd;

	/* If the query is an Update/Delete and Read access via a Secondary
	** Index of the Target table is required, set the "mustlock" flag
	** to prevent dirty reads on the target table's indexes in case
	** readlock=nolock has been specified on the session or the target
	** relation  (58681 & 64871).
	*/

	if ( (rel->rdr_rel->tbl_id.db_tab_index > 0) &&
	     (ahd->ahd_atype == QEA_UPD || ahd->ahd_atype == QEA_DEL) &&
	     (new_rw == DMT_A_READ) )
	{
	    PST_QTREE  *qtree;
	    DB_TAB_ID  restab_id;

	    qtree = global->ops_statement->pst_specific.pst_tree;
	    restab_id = qtree->pst_restab.pst_restabid;

	    if (rel->rdr_rel->tbl_id.db_tab_base == restab_id.db_tab_base)
		mustlock = TRUE;
	}
    }

    /* Allocate and fill in the valid list entry. */
    ret_vl = (QEF_VALID *) opu_qsfmem(global, sizeof (QEF_VALID));

    ret_vl->vl_next = global->ops_cstate.opc_topahd->ahd_valid;
    ret_vl->vl_rw = new_rw;
    ret_vl->vl_est_pages = npage_est;
    ret_vl->vl_total_pages = rel->rdr_rel->tbl_page_count;
    ret_vl->vl_alt = NULL;
    ret_vl->vl_size_sensitive = size_sensitive;
    ret_vl->vl_stmt = global->ops_cstate.opc_stmtno;
    ret_vl->vl_debug = (PTR) rel;
    ret_vl->vl_resource = resource;
    ret_vl->vl_mustlock = mustlock;
    if (rel->rdr_parts != NULL)
    {
	ret_vl->vl_flags |= QEF_PART_TABLE;
	ret_vl->vl_part_tab_id = (DB_TAB_ID *) opu_qsfmem(global,
			rel->rdr_parts->nphys_parts * sizeof(DB_TAB_ID));
	ret_vl->vl_partition_cnt = rel->rdr_parts->nphys_parts;
	for (i = 0, dmt_ppp = rel->rdr_pp_array; 
		i < ret_vl->vl_partition_cnt; i++, dmt_ppp++)
	    STRUCT_ASSIGN_MACRO(dmt_ppp->pp_tabid,
				ret_vl->vl_part_tab_id[i]);
    }
    else
    {
	ret_vl->vl_partition_cnt = 0;
	ret_vl->vl_part_tab_id = (DB_TAB_ID *) NULL;
    }
	
    if (grv->opv_gmask & OPV_SETINPUT)
    {
	if (global->ops_cstate.opc_qp->qp_setInput == NULL)
	{
	    global->ops_cstate.opc_qp->qp_setInput = ret_vl;
	    opc_ptrow(global, &ret_vl->vl_tab_id_index, sizeof(DB_TAB_ID));
	}
	else
	{
	    ret_vl->vl_tab_id_index =
		global->ops_cstate.opc_qp->qp_setInput->vl_tab_id_index;
	    global->ops_cstate.opc_qp->qp_setInput->vl_size_sensitive |=
		size_sensitive;
	}
	ret_vl->vl_flags = QEF_SET_INPUT;
    }
    else
    {
	STRUCT_ASSIGN_MACRO(rel->rdr_rel->tbl_id,
			    ret_vl->vl_tab_id);
	STRUCT_ASSIGN_MACRO(rel->rdr_rel->tbl_date_modified,
			    ret_vl->vl_timestamp);
    }

    /*
    ** If this is a Global Session Temporary Table, turn on QEF_SESS_TEMP.
    */
    if (((i4)rel->rdr_rel->tbl_id.db_tab_base <= 0) &&
	(!STncmp("$Sess", rel->rdr_rel->tbl_owner.db_own_name, 5 )))
	ret_vl->vl_flags |= QEF_SESS_TEMP;

    if (!(grv->opv_gmask & OPV_MCONSTTAB))
    {
	opc_ptcb(global, &ret_vl->vl_dmf_cb, sizeof (DMT_CB));
	global->ops_cstate.opc_qp->qp_cb_cnt += ret_vl->vl_partition_cnt;
				/* add room for partition DMT_CBs */
    }
    else 
    {
	ret_vl->vl_dmf_cb = -1;
	ret_vl->vl_flags |= QEF_MCNSTTBL;
    }
    opc_ptcb(global, &ret_vl->vl_dmr_cb, sizeof (DMR_CB));
    global->ops_cstate.opc_qp->qp_cb_cnt += ret_vl->vl_partition_cnt;
				/* add room for partition DMR_CBs */
    if (ret_vl->vl_partition_cnt > 0)
    {
	/* Add one more slot to save master DMR_CB in rpt queries. */
	global->ops_cstate.opc_qp->qp_cb_cnt++;
	ret_vl->vl_dmr_cb++;
    }

    /* Put the new valid structure on the resource structs list of actions */
    if (qr_tbl->qr_valid == NULL)
    {
	qr_tbl->qr_valid = qr_tbl->qr_lastvalid = ret_vl;
    }
    else
    {
	qr_tbl->qr_lastvalid->vl_alt = ret_vl;
	qr_tbl->qr_lastvalid = ret_vl;
    }	

    /* If a DMU action needs to point to the valid list (to update the
    ** table id)
    */
    if (grv->opv_ptrvalid != NULL)
    {
	*grv->opv_ptrvalid = ret_vl;
	grv->opv_ptrvalid = NULL;
    }

    /* Remember the first valid struct for determining the read/write 
    ** stuff above
    */
    if (grv->opv_topvalid == NULL)
    {
	grv->opv_topvalid = ret_vl;
    }

    /* put the valid list entry on the action header list.
    */
    global->ops_cstate.opc_topahd->ahd_valid = ret_vl;

    /* All's well, lets give it to the caller. */
    return(ret_vl);
}

/*{
** Name: OPC_GET_RESOURCE	- Find the QEF_RESOURCE for a table if one 
**				    exists
**
** Description:
**      This searches through the list of resources for the given table 
**      and returns it if found (otherwise it returns NULL). 
[@comment_line@]...
**
** Inputs:
**  global -
**	The query-wide state information.
**  grv -
**	The global range variable of the table to find
**  ptbl_type -
**  ptemp_id -
**	Pointers to uninitialized nat/PTRs if the table type/temp id is
**	desired. If it is not needed, then pass NULL pointers.
**
** Outputs:
**  ptbl_type -
**	Whether this is a temp table, set input table, or user table.
**  ptemp_id -
**	If this is a temp table, this is an identifier to use when refering
**	to this table (since the table id isn't known yet).
**
**	Returns:
**	    The resource for the table if found, or NULL.
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      5-oct-93 (eric)
**          created.
**	11-feb-97 (inkdo01)
**	    Changes for MS Access OR transformations.
[@history_template@]...
*/
QEF_RESOURCE *
opc_get_resource(
	OPS_STATE   *global, 
	OPV_GRV	    *grv, 
	i4	    *ptbl_type, 
	PTR	    *ptemp_id,
	QEF_MEM_CONSTTAB **cnsttab_p)
{
    QEF_QP_CB		*qp = global->ops_cstate.opc_qp;
    i4			tbl_type;
    PTR			temp_id;
    RDR_INFO		*rel = grv->opv_relation;
    QEF_RESOURCE	*resource;
    QEF_TBL_RESOURCE	*qr_tbl;
    i4		tab_base;
    i4		tab_index;

    /* figure out the table id */
    if (rel == NULL)
    {
	tab_base = 0;
	tab_index = 0;
    }
    else
    {
	tab_base = rel->rdr_rel->tbl_id.db_tab_base;
	tab_index = rel->rdr_rel->tbl_id.db_tab_index;
    }

    /* set the tbl_type and temp_id */
    if (grv->opv_gmask & OPV_SETINPUT)
    {
	tbl_type = QEQR_SETINTBL;
	temp_id = NULL;
	cnsttab_p = NULL;
    }
    else if (grv->opv_gmask & OPV_MCONSTTAB)
    {
	tbl_type = QEQR_MCNSTTBL;
	temp_id = NULL;
	(*cnsttab_p) = opc_make_mcnsttab(global, grv);
    }
    else if (tab_base == 0 && tab_index == 0)
    {
	tbl_type = QEQR_TMPTBL;
	temp_id = (PTR) grv;
	cnsttab_p = NULL;
    }
    else
    {
	tbl_type = QEQR_REGTBL;
	temp_id = NULL;
	cnsttab_p = NULL;
    }

    /* find the resource block for this table */
    for (resource = qp->qp_resources; 
	    resource != NULL; 
	    resource = resource->qr_next
	)
    {
	/* if we found a resource with the same table id, table type, and
	** temp id, then we've found what were looking for
	*/
	qr_tbl = &resource->qr_resource.qr_tbl;
	if (qr_tbl->qr_valid->vl_tab_id.db_tab_base == tab_base &&
	     qr_tbl->qr_valid->vl_tab_id.db_tab_index == tab_index &&
	     tbl_type == qr_tbl->qr_tbl_type &&
	     temp_id == qr_tbl->qr_temp_id
	    )
	{
	    break;
	}
    }

    if (ptbl_type != NULL)
	*ptbl_type = tbl_type;
    if (ptemp_id != NULL)
	*ptemp_id = temp_id;

    return(resource);
}

/*{
** Name: OPC_BGMKEY	- Begin the AHD_MKEY CX
**
** Description:
**      This routine begins the compilation of the AHD_MKEY CX. QEF uses 
**      the AHD_MKEY CX to build the key values that are required to key 
**      into the relations that an action uses. Because this CX is executed
**	once at the beginning of the execution of an action, several other
**	things are compiled in. The first additional item is PMENCODE 
**	instructions for repeat query parmeters. The next item is filling
**	DBP local variables in with default values. The compilation for this 
**	CX is begun here, and then it is continued in opc_orig_build(),
**      and opc_kjoin_build(). The compilation is terminated in 
**	opc_fqp_finish().
**        
**      The steps taken are: First we estimate the size of the CX. This is 
**      not as simple a task as it is for other CXs. The problem is that  
**      we don't yet know how we will key into the various relations (this 
**      is determined during the building of the nodes). Because of this, 
**      we assume that each relation will have an upper and lower bound 
**      for each key attribute. If this turns out to be wrong, then the 
**      CX will automatically expanded. Because the AHD_MKEY CX is used for 
**      more than just building keys, we also include size estimates for 
**      doing the PMENCODE instructions on repeat query parameters, and 
**	filling the DBP local variables in with default values. Once we
**      have the CX size estimate, we begin the CX and add the qen bases
**      that are required. 
**        
**      Once we have begun the CX, we go ahead and compile the PMENCODE 
**      instructions for the repeat query parameters, and the instructions 
**      to fill the DBP local vars in with default values. 
**
** Inputs:
**	global -
**	    The query wide state variable for OPF/OPC
**	ahd -
**	    The action header that is currently being built. This holds the
**	    ahd_mkey field that we will begin the compilation for.
**	cadf -
**	    This is an uninitialized OPC_ADF struct.
**
** Outputs:
**	ahd -
**	    The ahd_mkey field will hold initial values for compilation,
**	    plus any addition values require because the ENCODEs and the
**	    default values have been compiled into DBP vars.
**	cadf -
**	    Like ahd_mkey, but the OPC version.
**
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    none, except those that opx_error returns.
**
** History:
**      18-june-87 (eric)
**          written
**	17-jul-89 (jrb)
**	    Set precision correctly for decimal project.
**	26-feb-92 (anitap)
**          Added changes to fix bug 42291/36295. To pass the correct tree to
**          opc_parmap().
**      25-jun-92 (anitap)
**          Added changes to fix a bug. The ADE_PMENCODE instruction to
**          translate pattern matching characters into internal QUEL pattern
**          matching chars was being compiled in the MAIN segment of ahd_mkey
**          CX. This led to queries having functions on pattern matching chars
**          to return incorrect results. This was because the function was 
**	    being compiled in the VIRGIN segment and the ADE_PMENCODE in the
**          MAIN segment. This led to them using different bases. Moved the
**          ADE_PMENCODE insr to VIRGIN segment.
**      10-nov-95 (inkdo01)
**	    Changes to support replacement of QEN_ADF instances by pointers.
**	12-Dec-2005 (kschendel)
**	    Use new opcadf utility for allocating QEN_ADF in opf memory.
**	31-May-07 (kibro01) b118384
**	    If we have functions within functions of parameters, allocating
**	    a single base per parameter may not be sufficient.  If there are
**	    parameters use cxest to estimate better how many we'll need.
[@history_template@]...
*/
VOID
opc_bgmkey(
		OPS_STATE	*global,
		QEF_AHD		*ahd,
		OPC_ADF		*cadf)
{
	/* standard variables to keep line length down */
    OPS_SUBQUERY    *sq = global->ops_cstate.opc_subqry;
    OPC_BASE	    *parms = global->ops_cstate.opc_rparms;

	/* Adeops holds the operands for the encode and mecopy instructions */
    ADE_OPERAND	    adeops[2];

	/* varno holds the range variable number for each relation that
	** will be keyed into. Rel points to the relation description for
	** that relation. Lvarno points
	*/
    OPV_IVARS	    varno;
    RDR_INFO	    *rel;

	/* Keyno holds the key number that we are currently estimating the
	** size of. Attno holds the corresponding attribute number.
	*/
    i4		    keyno;
    i4		    attno;

	/* Ninstr, nops, nconst, szconst, and max_base hold the size
	** estimate of the CX (or at least the size estimate that is
	** given to opc_adbegin().
	*/
    i4		    ninstr, nops, nconst;
    i4	    szconst;
    i4		    max_base;

	/* Extra copies of these to search to query tree with */
    i4		    cx_ninstr, cx_nops, cx_nconst, cx_szconst;

	/* parmno is the repeat query parameter number that we are currently
	** estimating or compiling.
	*/
    i4		    parmno;

	/* pmap_size and param_map hold the info about which repeat query
	** parameters need PMENCODEs.
	*/
    i4		    pmap_size;
    u_i1	    *param_map;

	/* First_stmt points to the first statement in the DBP. This is used
	** to find the description of the local vars. Lvarno holds each of
	** the local variable numbers that are used in the DBP. OPC_PST
	** is the structure that we use to hold info about the dbp local vars.
	** This structure is stored in the pst_opf field of the PST_STATEMENT
	** structure for local vars. Decvar is the pointer to the decvar struct
	** for the local vars. Finally, first_rowno is the first DSH row
	** number that we added for holding local vars. Each local var gets
	** it's own DSH row, and the rows are allocate sequentially.
	*/
    PST_STATEMENT   *first_stmt;
    i4		    lvarno;
    OPC_PST_STATEMENT	*opc_pst;
    PST_DECVAR	    *decvar;
    i4		    first_rowno;

	/* Dv is used to hold the default value for the dbp local variable. */
    DB_DATA_VALUE   dv;

	/* Mem_type will tell opc_adbegin to allocate memory from the
	** stack ULM stream, or from the query permenent stream.
	*/
    i4		    mem_type;

        /* Added for bug 42291/36295 */
    i4              num_boolfact;
    OPB_BOOLFACT    *boolfact;

    /* If ahd_mkey has already been allocated and inited then don't do it
    ** again.
    */
    if (ahd->ahd_mkey)		/* better not be one, yet */
    {
	return;
    }

    /*
    ** Init the ADF CX to make the keys. The order of bases is: key array,
    ** each repeat query parameter (in order), and then the tuples for
    ** each range var if needed.
    */

    /* Estimate the size of the CX for key making */
    nconst = 0;
    szconst = 0;
    nops = 0;
    ninstr = 0;
    max_base = 1;

    cx_ninstr = cx_nops = cx_nconst = cx_szconst = 0;

    if (global->ops_statement->pst_type == PST_QT_TYPE)
    {
	max_base += sq->ops_vars.opv_rv;

	if (global->ops_procedure != NULL &&
	    global->ops_procedure->pst_isdbp == FALSE)
	{
	    max_base += global->ops_parmtotal;
	}

	/* for (each variable) */
	for (varno = 0; varno < sq->ops_vars.opv_rv; varno++)
	{
	    rel = sq->ops_vars.opv_base->opv_rt[varno]->opv_grv->opv_relation;

	    if (rel != NULL && rel->rdr_keys != NULL)
	    {
		/*
		** Estimating the CX size is a total guess, since we can't find
		** out how many keys there are until we finish the query plan.
		** Because of this, we assume that each relation will be keyed
		** by a single range (upper and lower bounds) on all key atts.
		** For each key attribute, we will have: 2 instructions (one for
		** lower, one for upper), 10 operands (5 operands for each
		** ADE_KEYBLD instruction), 2 constants (one for each upper and
		** lower).
		*/
		ninstr += (2 * rel->rdr_keys->key_count);
		nops += (10 * rel->rdr_keys->key_count);
		nconst += (2 * rel->rdr_keys->key_count);

		/* for (each key in the variable) */
		for (keyno = 0; keyno < rel->rdr_keys->key_count; keyno++)
		{
		    attno = rel->rdr_keys->key_array[keyno];
		    szconst += (2 * rel->rdr_attr[attno]->att_width);
		}
	    }
	}
	/* If we've got keys to build (and parameters), we'd better check to 
	** see we don't need more space to build them in (which would mean that
	** there are functions-within-functions or additional data type
	** coercions required).  (kibro01) b118384
	*/
	if (ninstr > 0 && global->ops_qheader->pst_numparm > 0)
	{
	    opc_cxest(global, NULL,
		global->ops_cstate.opc_subqry->ops_root->pst_left,
		&cx_ninstr, &cx_nops, &cx_nconst, &cx_szconst);
	    if (cx_nops > ninstr)
	    {
		/* If our new estimate found more operations, then we
		** need to ensure adequate bases are allocated, so flag that
		** by increasing the estimate for max_base.
		** (kibro01) b118384
		*/
		max_base += cx_nops - ninstr;
	    }
	}

	/* for (each user repeat query parameter) */
	if (global->ops_qheader->pst_qtree->
	    pst_sym.pst_value.pst_s_root.pst_qlang == DB_QUEL)
	{
	    for (parmno = 1;
		 parmno <= global->ops_qheader->pst_numparm; 
		 parmno++
		)
	    {
		ninstr++;
		nops++;
	    }
	}
    }

    /* make sure that there is space for any local vars to be filled */
    first_stmt = global->ops_procedure->pst_stmts;
    if (global->ops_procedure->pst_isdbp == TRUE &&
	(global->ops_cstate.opc_flags & OPC_LVARS_FILLED) == 0 &&
	first_stmt->pst_type == PST_DV_TYPE
	)
    {
	/* add enough bases for the local vars */
	max_base += first_stmt->pst_specific.pst_dbpvar->pst_nvars;

	decvar = first_stmt->pst_specific.pst_dbpvar;
	for (lvarno = 0;
	     lvarno < first_stmt->pst_specific.pst_dbpvar->pst_nvars;
	     lvarno++
	    )
	{
	    ninstr++;
	    nops += 2;
	    nconst++;
	    szconst += decvar->pst_vardef[lvarno].db_length;
	}
    }

    /* Begin the CX */
    if (global->ops_statement->pst_type == PST_QT_TYPE)
    {
	mem_type = OPC_REG_MEM;
    }
    else
    {
	mem_type = OPC_STACK_MEM;
    }
    /* Don't know if anything will be generated, so ask for QEN_ADF
    ** in "trial" memory.
    */
    opc_adalloct_begin(global, cadf, &ahd->ahd_mkey, 
		ninstr, nops, nconst, szconst, max_base, mem_type);

    if (global->ops_statement->pst_type == PST_QT_TYPE)
    {
	/* Add the bases that we know that we'll need */
	opc_adbase(global, cadf, QEN_KEY, 0, 0, 0);
	if (global->ops_procedure != NULL &&
	    global->ops_procedure->pst_isdbp == FALSE)
	{
	    /*
	    ** Add the repeat query parameters (if this is a database
	    ** procedure then ops_parmtotal refers to the number of dbp
	    ** vars, not the number of repeat query parameters).
	    */
	    for (parmno = 1; parmno <= global->ops_parmtotal; parmno++)
	    {
		if (parms[parmno].opc_bvalid == TRUE)
		{
		    /*
		    ** Add all parameters that might be involved in making
		    ** a key for this action. Note that if the parameter has
		    ** not been calculated yet, because of simple aggregates,
		    ** then it cannot possibly be a part of a key.
		    */
		    opc_adbase(global, cadf, QEN_PARM, parmno, 0, 0);
		}
	    }
	}

	if (global->ops_qheader->pst_qtree->
	    pst_sym.pst_value.pst_s_root.pst_qlang == DB_QUEL)
	{
	    pmap_size = 
		    ((global->ops_qheader->pst_numparm + 1) / BITSPERBYTE) + 1;
	    param_map = (u_i1 *) opu_Gsmemory_get(global, pmap_size);
	    MEfill(pmap_size, (u_char)0, (PTR)param_map);

            /* fix for bug 42291 */
            if (global->ops_statement->pst_type == PST_QT_TYPE &&
                global->ops_parmtotal > 0 &&
                global->ops_procedure->pst_isdbp == FALSE)
            {
               for (num_boolfact = 0;
                        num_boolfact < global->ops_subquery->ops_bfs.opb_bv;
                         num_boolfact++)               
               {    
		   boolfact =
             global->ops_subquery->ops_bfs.opb_base->opb_boolfact[num_boolfact];
	    	   opc_parmap(global, 
			      boolfact->opb_qnode, param_map);
	       }
	    }


	    /* For (each user repeat query parameter that is in the qual) */
	    for (parmno = -1;
		 (parmno = BTnext((i4)parmno, (char *)param_map, 
			(i4)(global->ops_qheader->pst_numparm + 1))) != -1;
		)
	    {
		/* compile a 'pmencode' instruction into the mkey CX; */
		adeops[0].opr_dt = parms[parmno].opc_dv.db_datatype;
		adeops[0].opr_prec = parms[parmno].opc_dv.db_prec;
		adeops[0].opr_len = parms[parmno].opc_dv.db_length;
		adeops[0].opr_prec = parms[parmno].opc_dv.db_prec;
		adeops[0].opr_offset = parms[parmno].opc_offset;
		adeops[0].opr_base = cadf->opc_repeat_base[parmno];

		/* As a fix for a QUEL repeat query bug, changed the 
		** compilation of the ADE_PMENCODE instr to VIRGIN 
	 	** segment from MAIN segment.
		*/
		opc_adparm(global, cadf, parmno, 
		    global->ops_qheader->pst_qtree->
		    pst_sym.pst_value.pst_s_root.pst_qlang, 
		    ADE_SVIRGIN);
		opc_adinstr(global, cadf, ADE_PMENCODE, ADE_SVIRGIN,
		    1, adeops, 0);
	    }
	}
    }

    /* If this is a database procedure that has local vars and they haven't
    ** already been filled with default values, then compile code to fill 
    ** them.
    */
    first_stmt = global->ops_procedure->pst_stmts;
    if (global->ops_procedure->pst_isdbp == TRUE &&
	(global->ops_cstate.opc_flags & OPC_LVARS_FILLED) == 0 &&
	first_stmt->pst_type == PST_DV_TYPE
	)
    {
	/* set the local vars filled bit so that we don't do this again */
	global->ops_cstate.opc_flags |= OPC_LVARS_FILLED;

	/* initialize vars to be used in the loop */
	decvar = first_stmt->pst_specific.pst_dbpvar;
	opc_pst = (OPC_PST_STATEMENT *) first_stmt->pst_opf;
	first_rowno = opc_pst->opc_lvrow;
	dv.db_data = NULL;
	dv.db_length = 0;
	dv.db_datatype = 0;
	dv.db_prec = 0;

	/* Now fill each local var */
	for (lvarno = 0;
	     lvarno < first_stmt->pst_specific.pst_dbpvar->pst_nvars;
	     lvarno++
	    )
	{
	    /* Figure out where to put the empty value */
	    adeops[0].opr_dt = decvar->pst_vardef[lvarno].db_datatype;
	    adeops[0].opr_prec = decvar->pst_vardef[lvarno].db_prec;
	    adeops[0].opr_len = decvar->pst_vardef[lvarno].db_length;
	    adeops[0].opr_prec = decvar->pst_vardef[lvarno].db_prec;
	    adeops[0].opr_offset = 0;
	    adeops[0].opr_base = cadf->opc_row_base[lvarno + first_rowno];

	    if (adeops[0].opr_base < ADE_ZBASE || 
		adeops[0].opr_base >= ADE_ZBASE + cadf->opc_qeadf->qen_sz_base)
	    {
		opc_adbase(global, cadf, QEN_ROW, 
					    lvarno + first_rowno, 0, 0);
		adeops[0].opr_base = 
			cadf->opc_row_base[lvarno + first_rowno];
	    }

	    /*
	    ** Get an empty value, add it to the
	    ** CX and copy it to the local var.
	    */
	    opc_adempty(global, &dv,
		adeops[0].opr_dt,
		adeops[0].opr_prec,
		adeops[0].opr_len);

	    opc_adconst(global, cadf, &dv, &adeops[1], DB_SQL, ADE_SMAIN);
	    opc_adinstr(global, cadf, ADE_MECOPY, ADE_SMAIN, 2, adeops, 1);
	}
    }
}

/*{
** Name: OPC_PARMAP	- Provide a map of parameters that are used in a qtree
**
** Description:
**      This routine fills in a bit map with the user repeat query parameters
**      that are used in a query tree. 
[@comment_line@]...
**
** Inputs:
**  global - 
**	The OPF state variable for this query.
**  global->ops_qheader->pst_numparm
**	The number of user repeat query parameters
**  root
**	The query tree to be mapped
**  pmap -
**	A pointer to memory that has one bit for each param plus one extra.
**	Pmap should be initialized by the caller with what ever they want,
**	typically zeros.
**
** Outputs:
**  pamap -
**	The bit in pmap for each parameter that is present in qtree pointed
**	to by root will be turned on.
**
**	Returns:
**	    nothing
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      19-june-87 (eric)
**          written
[@history_template@]...
*/
static VOID
opc_parmap(
	OPS_STATE   *global,
	PST_QNODE   *root,
	u_i1	    *pmap )
{
    i4		parm_no;

    if (root->pst_sym.pst_type == PST_CONST)
    {
	parm_no = root->pst_sym.pst_value.pst_s_cnst.pst_parm_no;
   
	if (root->pst_sym.pst_value.pst_s_cnst.pst_tparmtype == PST_RQPARAMNO 
		&& parm_no <= global->ops_qheader->pst_numparm
	    )
	{
	    BTset((i4)parm_no, (char *)pmap);
	}
    }

    if (root->pst_right != NULL)
    {
	opc_parmap(global, root->pst_right, pmap);
    }

    if (root->pst_left != NULL)
    {
	opc_parmap(global, root->pst_left, pmap);
    }
}

/*{
** Name: OPC_MAKE_MCNSTTAB - builds structure to define a MS Access OR 
**	transformed in-memory constants table
**
** Description:
**
** Inputs:
**	global	    - the global control block for compilation
**
**	grvp	    - address of global range var which defines table
**
** Outputs:
**	Returns:
**
**	cnsttab_p   - address of QEF_MEM_CONSTTAB structure.
**
** History:
**	11-feb-97 (inkdo01)
**	    initial creation
**	24-mar-97 (inkdo01)
**	    Fix to handle mixed length ints and floats.
[@history_template@]...
*/
static QEF_MEM_CONSTTAB *
opc_make_mcnsttab(
    OPS_STATE	*global,
    OPV_GRV	*grvp)

{
    QEF_MEM_CONSTTAB *cnsttab_p;
    PST_QNODE	*cnst;
    DMT_ATT_ENTRY *attrp;
    char	*currow;
    i4		i, j, rowsize, align;
    f4		f4temp;
    i2		i2temp;

    /* First compute aligned rowsize. */
    align = grvp->opv_relation->rdr_rel->tbl_width % sizeof (ALIGN_RESTRICT);
    if (align != 0)
        align = sizeof (ALIGN_RESTRICT) - align;
    rowsize = align + grvp->opv_relation->rdr_rel->tbl_width;

    /* Allocate the CONSTTAB structure and initialize it. */
    cnsttab_p = (QEF_MEM_CONSTTAB *) opu_qsfmem(global, sizeof(QEF_MEM_CONSTTAB));
    cnsttab_p->qr_rowsize = rowsize;
    cnsttab_p->qr_rowcount = grvp->opv_relation->rdr_rel->tbl_record_count;

    /* Allocate memory for the table itself, then copy in the CONSTs. */
    cnsttab_p->qr_tab_p = (PTR) opu_qsfmem(global, cnsttab_p->qr_rowsize *
						cnsttab_p->qr_rowcount);
    currow = cnsttab_p->qr_tab_p;
    MEfill(cnsttab_p->qr_rowsize * cnsttab_p->qr_rowcount, (u_char)0,
	cnsttab_p->qr_tab_p);		/* clear the whole thing */
    for (i = 0; i < cnsttab_p->qr_rowcount; i++, currow += cnsttab_p->qr_rowsize)
     for (cnst = grvp->opv_orrows[i], j = 1;
	 	cnst != NULL && (attrp = grvp->opv_relation->rdr_attr[j]);
		cnst = cnst->pst_right, j++)
				/* loop down rows, then across values */
	if (cnst->pst_sym.pst_dataval.db_length != attrp->att_width &&
		(attrp->att_type == DB_INT_TYPE ||
		attrp->att_type == DB_FLT_TYPE))
	{
	    /* Check for float or integer constants in which we have mixed
	    ** lengths (can be caused by the manner in which PSF's lexical 
	    ** analyzer builds constants). Otherwise, just MEcopy the vals. */
	    if (attrp->att_type == DB_INT_TYPE)
	    {
		PTR	intptr = (PTR)(currow + attrp->att_offset);
		i4	i4temp;
		switch (cnst->pst_sym.pst_dataval.db_length) {
		 case 1:
		    i4temp = I1_CHECK_MACRO(*(i1 *)cnst->pst_sym.pst_dataval.db_data);
		    break;
		 case 2:
		    i4temp = *(i2 *)cnst->pst_sym.pst_dataval.db_data;
		    break;
		 case 4:
		    i4temp = *(i4 *)cnst->pst_sym.pst_dataval.db_data;
		 }
		switch (attrp->att_width) {
		 case 2:
		    *(i2 *)intptr = i4temp;
		    break;
		 case 4:
		    *(i4 *)intptr = i4temp;
		 }
	    }
	    else if (attrp->att_type == DB_FLT_TYPE)
	    {
		PTR	fltptr = (PTR) (currow + attrp->att_offset);
		f8	f8temp;
		switch (cnst->pst_sym.pst_dataval.db_length) {
		 case 4:
		    f8temp = *(f4 *)cnst->pst_sym.pst_dataval.db_data;
		    break;
		 case 8:
		    f8temp = *(f8 *)cnst->pst_sym.pst_dataval.db_data;
		}
		*(f8 *)fltptr = f8temp;
	    }
	}
	else MEcopy((PTR)cnst->pst_sym.pst_dataval.db_data, 
	    cnst->pst_sym.pst_dataval.db_length,
	    (PTR)(currow + grvp->opv_relation->rdr_attr[j]->att_offset));

    return(cnsttab_p);			/* pass back the QEF_MEM_CONSTTAB */

}	/* end of opc_make_mcnsttab */

/*{
** Name: OPC_PQMAT	- generate CX that materializes partition
**	qualification constants for partitioned table access
**
** Description:
**	Generate partition qualification info for an ORIG node.
**
**	Partition qualification boolean factors have already been
**	analyzed.  All we need to do here is run through the list
**	for this table and call pqdim to generate the runtime
**	glop needed for each dimension that has usable qualification.
**
** Inputs:
**	global	    - the global control block for compilation
**	cnode
**	orig	    - ptr to node structure into which CX is built
**	varp	    - ptr to local range variable for table
**
** Outputs:
**	Returns:
**
**
** History:
**	28-jan-04 (inkdo01)
**	    Written for table partitioning.
**	16-apr-04 (inkdo01)
**	    Allow constant expression comparands (e.g. with date funcs).
**	29-apr-04 (inkdo01)
**	    Fix constant expression comparands and add validation call to
**	    assure legitimacy of expressions.
**	13-may-04 (inkdo01)
**	    Changes to support partition qualification using "between"
**	    predicates (only if a single partitioning column, though).
**	28-Feb-2005 (schka24)
**	    Require that all dimension columns be qualified, runtime
**	    can't handle partial qual yet.  Fix up a row counting glitch
**	    with multiple column dims, and make multiple IN-lists work.
**	22-Jul-2005 (schka24)
**	    Allocate a work bit-map when doing qualification in a multi-
**	    dimensional partitioning scheme, and make qual bit-map length
**	    big enough for physical partitions (rather than logical).
**	3-Jan-2006 (kschendel)
**	    Oops, calculations for best dimension were off for multi
**	    dimensional schemes;  want to take fewest physical partitions.
**	16-Jun-2006 (kschendel)
**	    Break out CX and partinfo generation for multidim qualification.
[@history_template@]...
*/

static VOID
opc_pqmat(
	OPS_STATE	*global,
	QEN_ORIG	*orig,
	OPC_NODE	*cnode,
	OPV_VARS	*varp,
	OPV_IVARS	varno)

{
    DB_PART_DEF	*pdefp = varp->opv_grv->opv_relation->rdr_parts;
    DB_PART_DIM	*pdimp;
    QEN_PART_INFO *partp = orig->orig_part;
    OPB_PQBF	*pqbfp;
    OPB_BOOLFACT *bfp;
    OPB_ATBFLST	*keyp;
    i4		totparts = pdefp->nphys_parts;
    i4		i;
    bool	ok;

    /* Loop over partition qual BF's by dimension, generate qual
    ** stuff for the ones that look usable
    */
    for (pqbfp = varp->opv_pqbf;
		pqbfp != (OPB_PQBF *) NULL; pqbfp = pqbfp->opb_pqbfnext)
    {
	pdimp = &pdefp->dimension[pqbfp->opb_pdim];
	/* Verify the qualification expressions are legitimate. */
	ok = TRUE;
	for (i = 0; i < pqbfp->opb_pbfcount; i++)
	{
	    keyp = &pqbfp->opb_pcbase->opb_keyorder[i];
	    if (keyp->opb_gbf == OPB_NOBF)
		break;		/* no more covered columns */
	    bfp = global->ops_cstate.opc_subqry->ops_bfs.opb_base->
			opb_boolfact[keyp->opb_gbf];
	    ok = opc_pqbf_verify(global, bfp->opb_qnode, 
			keyp->opb_attno, varno);
	    if (!ok)
		break;
	}
	if (ok)
	    opc_pqdim(global, orig, cnode, pqbfp, pdefp);
    }

    if (partp->part_qual.part_qdim == NULL)
	return;

    /* Must have generated one or more quals, allocate bit maps for QEF */

    opc_ptrow(global, &partp->part_qpix, (totparts + 7) / 8);
				/* row array slot for partition bit map */

    /* Allocate work bit maps that are the same size as the partition
    ** bit map.  This is quicker and easier than making qef allocate
    ** them by hand.
    */
    opc_ptrow(global, &partp->part_qpwlix, (totparts + 7) / 8);
    opc_ptrow(global, &partp->part_qpwpix, (totparts + 7) / 8);

} /* opc_pqmat */

/*{
** Name: OPC_PQDIM	- generate CX that materializes partition
**	qualification constants for partitioned table access
**
** Description:
**	Generate a CX to materialize the constant values needed for
**	simple partition qualification for one dimension.
**
**	At present, only basic testing operators are supported: =, < <=,
**	> >=, and BETWEEN.  LIKE is not supported;  partial qualification
**	where some column is omitted is not supported;  and multiple
**	non-equalities are not supported.  I think that ultimately what
**	will be needed is code to do a KEYBLD operation for the fancier
**	stuff, such as LIKE, so that the low and high limits for qualification
**	can be generated.  "whichpart" will also have to be enhanced if
**	we're to support partial qualification.
**	
**
** Inputs:
**	global	    - the global control block for compilation
**	orig	    - ptr to node structure into which CX is built
**	cnode
**	pqbfp	    - ptr to qualification boolfacts for this dimension
**	pdefp	    - ptr to partition definition info (in RDF)
**
** Outputs:
**	Returns:
**
** History:
**	16-Jun-2006 (kschendel)
**	    Split up original opc_pqmat for multi-dimensional qualification.
**      6-jul-06 (kibro01)
**          Added support for LOCALVARNO being used in partition opc_pqmat
**          to fix bug 116306.
**	27-apr-2007 (dougi)
**	    Add support for UTF8-enabled server.
*/

static VOID
opc_pqdim(
	OPS_STATE	*global,
	QEN_ORIG	*orig,
	OPC_NODE	*cnode,
	OPB_PQBF	*pqbfp,
	DB_PART_DEF	*pdefp)

{
    DB_PART_DIM	*pdimp;
    DB_PART_LIST *pcolp;
    QEN_PART_INFO *partp = orig->orig_part;
    QEN_PART_QUAL *pqualp;
    OPB_BOOLFACT *bfp;
    OPB_ATBFLST	*keyp;
    PST_QNODE	*nodep, *colp, *comparandp;
    PST_QNODE	*inlistp[DB_MAXKEYS];
    PST_QNODE	*incolp[DB_MAXKEYS];
    OPC_BASE	*parms;
    OPG_CB	*ops_server;
    OPS_SUBQUERY *subquery = global->ops_cstate.opc_subqry;
    ADE_OPERAND ops[2];
    ADI_OP_ID	opno;
    OPC_ADF	cadf;
    i4		i, pc, nops, ninstrs, nconsts, szconsts, dummy;
    i4		rowsz, rowno, rowcount, parmno, bufsz, baseno;
    i4		inoff[DB_MAXKEYS];
    i4		in_index, in_cols;
    i4		in_row, rows_to_dup;
    i4		align;
    bool	unorm, ok;
    bool	isconst;
    bool	between;

    ops_server = global->ops_cb->ops_server;

    /* Fill in embedded PART_QUAL first, then allocate a new one */

    if (partp->part_qual.part_qdim == NULL)
	pqualp = &partp->part_qual;
    else
    {
	pqualp = (QEN_PART_QUAL *) opu_qsfmem(global, sizeof(QEN_PART_QUAL));
	pqualp->part_qnext = partp->part_qual.part_qnext;
	partp->part_qual.part_qnext = pqualp;
    }
    pqualp->part_qflags = 0;

    /* Copy the chosen dimension's definition to orig node. */
    opc_partdim_copy(global, pdefp, pqbfp->opb_pdim, &pqualp->part_qdim);
    pdimp = pqualp->part_qdim;

    /* Go over bf's defined by pqbfp, generating CX and editing 
    ** dimension descriptor to match the materialized "constant rows". */
    nops = ninstrs = nconsts = szconsts = 0;	/* init for CX ests */
    for (i = 0; i < pqbfp->opb_pbfcount; i++)
    {
	keyp = &pqbfp->opb_pcbase->opb_keyorder[i];
	if (keyp->opb_gbf == OPB_NOBF)
	    break;		/* end of covered partitioning cols */
	bfp = subquery->ops_bfs.opb_base->opb_boolfact[keyp->opb_gbf];
	opc_cxest(global, cnode, bfp->opb_qnode, &ninstrs, &nops, 
				&nconsts, &szconsts);
    }
    if (i < pdimp->ncols)
    {
	/* Here, we would normally reset pdimp->ncols = i, but the
	** runtime can't handle the partial qualification thing yet.
	** So we should never get here.
	*/
	opx_1perror(E_OP009D_MISC_INTERNAL_ERROR,"didn't get allcols in opc_pqmat");
    }

    opc_adalloc_begin(global, &cadf, &pqualp->part_qpart, ninstrs,
	nops, nconsts, szconsts, cnode->opc_below_rows, OPC_STACK_MEM);
    rowsz = bufsz = rowcount = 0;
    rowno = global->ops_cstate.opc_qp->qp_row_cnt;
    opc_adbase(global, &cadf, QEN_ROW, rowno, 0, 0);
    baseno = cadf.opc_row_base[rowno];
				/* set result buffer number */
    between = (pqbfp->opb_pqopno == OPB_BETWEEN_OP);

    /* One pass over the partition qualification factors builds the 
    ** first row. If one of the bf's is an IN-list, it's comparison
    ** values will drive the creation of other rows in which all other 
    ** "columns" are the same as in the first. */

    ops[0].opr_base = baseno;
 
    /* We can have a regular set of values, or a BETWEEN, or one or
    ** more IN-lists.  i is set to -1 to stop the looping for a non-between.
    ** If there are IN-lists, the loop runs with i = -1 until we run out
    ** of IN-values.
    ** i runs past opb_pbfcount to stop the loop for a between.
    */
    in_index = 0;
    in_cols = 0;
    inlistp[0] = (PST_QNODE *) NULL;
    rows_to_dup = 1;
    in_row = 0;
    for (i = 0; i < pqbfp->opb_pbfcount &&
		(i >= 0 || (in_index < in_cols && inlistp[in_index] != NULL)); )
    {
	if (i >= 0 && (i < pdimp->ncols || between))
	{
	    /* We're either building the first row, or either of the
	    ** two rows of a between.
	    */
	    keyp = &pqbfp->opb_pcbase->opb_keyorder[i];
	    /* Note that this doesn't happen at present: */
	    if (keyp->opb_gbf == OPB_NOBF)
		break;		/* first NOBF terminates loop */
	    nodep = subquery->ops_bfs.opb_base->opb_boolfact[
				keyp->opb_gbf]->opb_qnode;
	    if (nodep->pst_sym.pst_type != PST_BOP ||
		(colp = nodep->pst_left)->pst_sym.pst_type != PST_VAR)
	    {
		/* Error - unexpected expression format. */
		opx_error(E_OP0682_UNEXPECTED_NODE);
	    }
	    comparandp = nodep->pst_right;
	    isconst = (comparandp->pst_sym.pst_type == PST_CONST);
	    if (isconst && inlistp[in_cols] == (PST_QNODE *) NULL &&
		comparandp->pst_left != (PST_QNODE *) NULL)
	    {
		/* IN-list - remember next value in list and row offset,
		** and count up the number of IN-lists we have.
		*/
		inlistp[in_cols] = comparandp->pst_left;
		incolp[in_cols] = colp;
		inoff[in_cols] = rowsz;
		++ in_cols;
		inlistp[in_cols] = NULL;
	    }
	    /* Prepare operands for movement to buffer. */
	    if (!between || i == 0)
	    {
		pcolp = &pdimp->part_list[i];
		pcolp->row_offset = rowsz;
				/* fix column to match buffer */
	    }
	    ops[0].opr_offset = rowsz;
	}
	else
	{
	    /* This is the 2nd or later row due to the presence of
	    ** an IN-list predicate. If the "rows" have more than one
	    ** column, the row is the same except for the IN-list value
	    ** so the first row is copied to the next offset and the
	    ** IN-list value is projected over top. If the IN-list
	    ** column is the only one, it is simply projected to the 
	    ** next row offset. */
	    if (pdimp->ncols > 1)
	    {
		ops[0].opr_offset = bufsz;
		ops[0].opr_dt = DB_BYTE_TYPE;
		ops[0].opr_len = rowsz;
		ops[0].opr_prec = 0;

		ops[1].opr_base = ops[0].opr_base;
		ops[1].opr_offset = rowsz * in_row;
		ops[1].opr_dt = DB_BYTE_TYPE;
		ops[1].opr_len = rowsz;
		ops[1].opr_prec = 0;

		opc_adinstr(global, &cadf, ADE_MECOPY, ADE_SVIRGIN,
					2, ops, 1);
				/* copy row to next slot */
	    }
	    ops[0].opr_offset = bufsz + inoff[in_index];
				/* offset of next inlist value */
	    colp = incolp[in_index];
	    comparandp = inlistp[in_index];
	    /* Advance only after cycling through all of the prior
	    ** IN-lists' possibilities.  Example:
	    ** c1 IN ('a','b','c') AND c2 IN (1,2).  The first row is
	    ** a 1.  We copy that for b 1 and c 1.  Then we move on to
	    ** c2's IN-list, and we have to copy a 1, b 1, and c 1 and
	    ** overlay 2 on top of each giving a 2, b 2, c 2.
	    ** rows_to_dup is 1 for the c1 IN-list, and 3 for the c2 list.
	    ** FIXME generates the 2 const over and over, make it stop...
	    */
	    if (++ in_row == rows_to_dup)
	    {
		inlistp[in_index] = inlistp[in_index]->pst_left;
		in_row = 0;
		if (inlistp[in_index] == NULL)
		{
		    ++ in_index;
		    rows_to_dup = rowcount + 1;	/* Haven't counted this one yet */
		}
	    }
	}
	
	/* The remaining code in the loop is common between the
	** first row and n'th row case. */
	ops[0].opr_dt = colp->pst_sym.pst_dataval.db_datatype;
	ops[0].opr_len = colp->pst_sym.pst_dataval.db_length;
	ops[0].opr_prec = colp->pst_sym.pst_dataval.db_prec;
	ops[0].opr_collID = colp->pst_sym.pst_dataval.db_collID;
	if (isconst && comparandp->pst_sym.pst_value.pst_s_cnst.
			pst_tparmtype == PST_USER)
	{
	    /* Literal constant. */
	    opc_adconst(global, &cadf, &comparandp->pst_sym.pst_dataval,
		&ops[1], comparandp->pst_sym.pst_value.pst_s_cnst.pst_cqlang,
		ADE_SVIRGIN);
	}
	else if (isconst && comparandp->pst_sym.pst_value.pst_s_cnst.
			pst_tparmtype == PST_RQPARAMNO)
	{
	    /* Repeat query parameter. */
	    parms = global->ops_cstate.opc_rparms;
	    parmno = comparandp->pst_sym.pst_value.pst_s_cnst.pst_parm_no;
	    ops[1].opr_base = cadf.opc_repeat_base[parmno];
	    ops[1].opr_offset = parms[parmno].opc_offset;
	    ops[1].opr_dt = parms[parmno].opc_dv.db_datatype;
	    ops[1].opr_len = parms[parmno].opc_dv.db_length;
	    ops[1].opr_prec = parms[parmno].opc_dv.db_prec;
	    ops[1].opr_collID = parms[parmno].opc_dv.db_collID;
	    opc_adparm(global, &cadf, parmno, 
		comparandp->pst_sym.pst_value.pst_s_cnst.pst_cqlang,
		ADE_SVIRGIN);
	    if (ops[1].opr_base < ADE_ZBASE ||
		ops[1].opr_base >= ADE_ZBASE + cadf.opc_qeadf->qen_sz_base)
	    {
		/* Add a base for the new parm. */
		opc_adbase(global, &cadf, QEN_PARM, parmno, 0, 0);
		ops[1].opr_base = cadf.opc_repeat_base[parmno];
	    }
	    opc_adseglang(global, &cadf, 
		comparandp->pst_sym.pst_value.pst_s_cnst.pst_cqlang,
		ADE_SVIRGIN);
	}
	else if (isconst && comparandp->pst_sym.pst_value.pst_s_cnst.
			pst_tparmtype == PST_LOCALVARNO)
	{
	    /* Local variable. */
	    parmno = comparandp->pst_sym.pst_value.pst_s_cnst.pst_parm_no;
	    opc_lvar_row(global, parmno, &cadf, &ops[1]);
	    opc_adseglang(global, &cadf, 
		comparandp->pst_sym.pst_value.pst_s_cnst.pst_cqlang,
		ADE_SVIRGIN);
	}
	else if (!isconst)
	{
	    /* Constant expression - ... */
	    ops[1].opr_dt = ops[0].opr_dt;
	    ops[1].opr_prec = ops[0].opr_prec;
	    ops[1].opr_len = ops[0].opr_len;
	    ops[1].opr_collID = ops[0].opr_collID;
	    ops[1].opr_base = cadf.opc_row_base[OPC_CTEMP];
	    ops[1].opr_offset = global->ops_cstate.opc_temprow->opc_olen;

	    /* increase the temp results buffer to hold our new value. */
	    if (ops[1].opr_len > 0)
	    {
		align = DB_ALIGN_MACRO(ops[1].opr_len);
		global->ops_cstate.opc_temprow->opc_olen += align;
	    }
	    opc_cqual(global, cnode, comparandp, &cadf, ADE_SVIRGIN,
				&ops[1], &dummy);
	}
	else opx_error(E_OP0897_BAD_CONST_RESDOM);

	/* Operands are (more or less) ready - do Unicode check then
	** copy to destination. */

	/* Note if result is Unicode - if so, source value is
	** normalized into result location. */
	if (abs(ops[0].opr_dt) == DB_NCHR_TYPE ||
		abs(ops[0].opr_dt) == DB_NVCHR_TYPE ||
	    ((global->ops_adfcb->adf_utf8_flag & AD_UTF8_ENABLED) &&
	    (abs(ops[0].opr_dt) == DB_VCH_TYPE ||
	     abs(ops[0].opr_dt) == DB_CHA_TYPE ||
	     abs(ops[0].opr_dt) == DB_TXT_TYPE ||
	     abs(ops[0].opr_dt) == DB_CHR_TYPE)))
	    unorm = TRUE;
	else unorm = FALSE;

	/* if the actual and intended result datatype/length/precisions
	** are the same, ignoring nullability (which is why we have the
	** second comparison with the negation), then no coercion is
	** needed.
	*/
	if ((ops[0].opr_dt == ops[1].opr_dt ||
		 ops[0].opr_dt == -ops[1].opr_dt) &&
	        (ops[0].opr_len == OPC_UNKNOWN_LEN ||
		 abs(ops[0].opr_dt) == DB_NCHR_TYPE ||
		 abs(ops[0].opr_dt) == DB_NVCHR_TYPE ||
		 (ops[0].opr_len == ops[1].opr_len &&
		  (ops[0].opr_prec == ops[1].opr_prec ||
		   abs(ops[1].opr_dt) != DB_DEC_TYPE) &&
		  ops[0].opr_dt == ops[1].opr_dt)))
	{
	    /* Emit MECOPY to copy VAR value to result location. */
	    opc_adinstr(global, &cadf, (unorm) ? ADE_UNORM: ADE_MECOPY, 
			ADE_SVIRGIN, 2, ops, 1);
	}
	else
	{
	    opc_adtransform(global, &cadf, &ops[1], &ops[0],
			ADE_SVIRGIN, (bool) FALSE);
	}
	if (i >= 0)
	{
	    rowsz += ops[0].opr_len;
	    i++;
	    if (i >= pdimp->ncols)
	    {
		/* We finished the first row.  If this is between, just
		** keep going for the next row.  Otherwise, if we have
		** any IN-lists, set up to iterate on the IN-values,
		** running with i set to -1 until the IN-values run out.
		*/
		++ rowcount;
		if (!between || i > 1)
		    i = -1;	/* end of "all columns" loop */
		align = rowsz % sizeof(ALIGN_RESTRICT);
		bufsz = rowsz += (sizeof(ALIGN_RESTRICT) - align);
				/* align single row buffer */
		opno = nodep->pst_sym.pst_value.pst_s_op.pst_opno;
		/* Flag if range test. */
		if (opno == ops_server->opg_lt ||
		    opno == ops_server->opg_le)
		    pqualp->part_qflags |= PART_QUAL_LE;
		else if (opno == ops_server->opg_gt ||
		    opno == ops_server->opg_ge)
		    pqualp->part_qflags |= PART_QUAL_GE;
	    }
	}
	else 
	{
	    bufsz += rowsz;
	    ++ rowcount;
	}
    }

    pqualp->part_prowcnt = rowcount;
    opc_ptrow(global, &rowno, bufsz);
    cadf.opc_qeadf->qen_output = pqualp->part_qpcix = rowno;

    opc_adend(global, &cadf);
    
} /* opc_pqdim */

/*{
** Name: OPC_PQBF_VERIFY	- confirms that comparand expressions
**	used to generate qualification bit maps for partitioned table
**	access are ok.
**
** Description:
**
** Inputs:
**	global	- the global control block for compilation
**	bfp	- ptr to expression to validate
**	attno	- joinop attr no of column
**	varno	- range table no for table
**
** Outputs:
**	Returns:
**
**
** History:
**	30-apr-04 (inkdo01)
**	    Written for table partitioning.
**	28-oct-05 (inkdo01)
**	    Permit <attno, varno> if in same EQC as PST_VAR nodes - doesn't
**	    have to be exact column.
[@history_template@]...
*/

static bool
opc_pqbf_verify(
	OPS_STATE	*global,
	PST_QNODE	*nodep,
	OPZ_IATTS	attno,
	OPV_IVARS	varno)

{
    OPS_SUBQUERY	*subquery = global->ops_cstate.opc_subqry;
    OPE_IEQCLS		eqcno;


    /* Descend theparse tree looking for invalid nodes. */
    switch (nodep->pst_sym.pst_type) {

      case PST_CONST:
	if (nodep->pst_left)
	    return(opc_pqbf_verify(global, nodep->pst_left, attno, varno));
	return(TRUE);

      case PST_BOP:
	if (opc_pqbf_verify(global, nodep->pst_left, attno, varno))
	    return(opc_pqbf_verify(global, nodep->pst_right, attno, varno));
	else return(FALSE);

      case PST_UOP:
	return(opc_pqbf_verify(global, nodep->pst_left, attno, varno));

      case PST_VAR:
	if (nodep->pst_sym.pst_value.pst_s_var.pst_vno == varno &&
	    nodep->pst_sym.pst_value.pst_s_var.pst_atno.db_att_id == attno)
	    return(TRUE);

	/* Might be same EQC as <varno, attno> - that's all we really need. */
	eqcno = subquery->ops_attrs.opz_base->opz_attnums[attno]->opz_equcls;
	if (eqcno != OPE_NOEQCLS &&
	    subquery->ops_attrs.opz_base->opz_attnums[nodep->pst_sym.pst_value.
			pst_s_var.pst_atno.db_att_id]->opz_equcls == eqcno)
	    return(TRUE);

	return(FALSE);

      default:
	return(FALSE);
	    }

}
