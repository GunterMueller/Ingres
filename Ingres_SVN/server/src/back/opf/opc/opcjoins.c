/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <pc.h>
#include    <cs.h>
#include    <lk.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ddb.h>
#include    <ulm.h>
#include    <ulf.h>
#include    <adf.h>
#include    <ade.h>
#include    <adfops.h>
#include    <dmf.h>
#include    <dmhcb.h>
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

/* external routine declarations definitions */
#define             OPC_JOINS           TRUE
#include    <bt.h>
#include    <me.h>
#include    <opxlint.h>
#include    <opclint.h>

/**
**
**  Name: OPCJOINS.C - Routine to build all types of join nodes.
**
**  Description:
**      This file  
[@comment_line@]...
**
**          opc_tjoin_build() - Compile a QEN_TJOIN node.
**          opc_kjoin_build() - Compile a QEN_KJOIN node.
**          opc_hjoin_build() - Compile a QEN_HJOIN node.
**	    opc_fsmjoin_build() - Compile a QEN_FSMJOIN node.
**          opc_cpjoin_build() - Compile a QEN_CPJOIN node.
**          opc_isjoin_build() - Compile a QEN_ISJOIN node.
[@func_list@]...
**
**
**  History:    
**      17-oct-86 (eric)
**          created
**	23-oct-89 (stec)
**	    In opc_tjoin_build commented out the call to opc_jnotret procedure
**	    to fix the problem of "eqc not available" when a function
**	    attribute is involved.
**	30-oct-89 (stec)
**	    Implement new QEN_NODE types and outer joins.
**	24-may-90 (stec)
**	    Added checks to use OPC_EQ, OPC_ATT contents only when filled in.
**	13-jul-90 (stec)
**	    Modified opc_tjoin_build() and opc_kjoin_build() to prevent integer
**	    overflow when calling opc_valid().
**	08-jan-91 (stec)
**	    Modified opc_join_build() to fix bug 34959. Also modified
**	    opc_sijoin_build() to prevent similiar problem.
**	29-jan-91 (stec)
**	    Changed opc_join_build(), opc_sijoin_build().
**	16-apr-91 (davebf)
**	    Added parm to opc_valid() call in order to enable vl_size_sensitive
**	25-mar-92 (rickh)
**	    Right joins now store tids in a real hold file.  Some related
**	    cleanup:  removed references to jn->sjn_hget and jn->sjn_hcreate,
**	    which weren't used anywhere in QEF.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**      17-aug-93 (anitap)
**          Typecast the values in the conditional tests involving dissimilar
**          types like float and MAXI4. The lack of these casts caused by
**          floating point exceptions on a certain machine in opc_tjoin_build()
**	    and opc_kjoin_build(). 
**      14-sep-93 (smc)
**          Moved <cs.h> for CS_SID.
**	5-jan-94 (rickh)
**	    The WHERE clause is executed AFTER the ON clause and outer join null
**	    fabricators.  Therefore, it's clearer if we compile the WHERE
**	    clause before the outer join CXs.  One happy advantage of this
**	    approach is that it forces the WHERE clause to refer to the
**	    same locations as the outer join CXs.
**	17-mar-1999 (somsa01)
**	    Added include of pc.h and lk.h to resolve references to locking
**	    ids.
**      08-may-2002 (huazh01)
**          Apply the fix for bug 98395 to opc_isjoin_build(). This fixes 
**          bug 107046. 
**	31-dec-04 (inkdo01)
**	    Added parm to opc_materialize() calls.
[@history_template@]...
**/

/*
**  Forward and/or External function references.
*/

/*{
** Name: OPC_TJOIN_BUILD	- Compile a CO node into a QEN_TJOIN node.
**
** Description:
{@comment_line@}...
**
** Inputs:
[@PARAM_DESCR@]...
**
** Outputs:
[@PARAM_DESCR@]...
**	Returns:
**	    {@return_description@}
**	Exceptions:
**	    [@description_or_none@]
**
** Side Effects:
**	    [@description_or_none@]
**
** History:
**      17-oct-86 (eric)
**          written
**	23-oct-89 (stec)
**	    Commented out the call to opc_jnotret procedure
**	    to fix the problem of "eqc not available" when
**	    a function attribute is involved.
**	24-may-90 (stec)
**	    Added checks to use OPC_EQ, OPC_ATT contents only when filled in.
**	13-jul-90 (stec)
**	    Modified call to opc_valid() to prevent integer overflow.
**	16-apr-91 (davebf)
**	    Added parm to opc_valid() call in order to enable vl_size_sensitive
**      21-may-93 (ed)
**          - changed opo_var to be a union structure so that
**          the outer join ID can be stored in interior CO nodes
**      17-aug-93 (anitap)
**          Typecast the values in the conditional tests involving dissimilar
**          types like float and MAXI4. The lack of these casts caused by
**          floating point exceptions on a certain machine. 
**	5-jan-94 (rickh)
**	    The WHERE clause is executed AFTER the ON clause and outer join null
**	    fabricators.  Therefore, it's clearer if we compile the WHERE
**	    clause before the outer join CXs.  One happy advantage of this
**	    approach is that it forces the WHERE clause to refer to the
**	    same locations as the outer join CXs.
**	2-feb-04 (inkdo01)
**	    Init tjoin_part for table partitioning.
**	3-feb-04 (inkdo01)
**	    Change buffer to hold TID8 for table partitioning.
**	13-apr-04 (inkdo01)
**	    Changes to QEN_PART_INFO inits.
**	3-may-05 (inkdo01)
**	    Init tjoin_flag and set TJOIN_4BYTE_TIDP to fix bug in post
**	    partitioned table Tjoins.
**	24-Jun-2005 (schka24)
**	    Extend above fix to properly handle OJ T-joins where the tidp
**	    is nullable.  (The size is then 5, not 4.)
**	22-Jul-2005 (schka24)
**	    Init new work-bitmap in partinfo.
**	16-Jun-2006 (kschendel)
**	    part-info qual stuff moved around, fix here.
**	 4-sep-06 (hayke02)
**	    Remove non-inner outer join equivalence classes from those used to
**	    compile tjoin_jqual ADE_COMPARE instructions. This prevents OJ
**	    outer rows, with ON clause attribute NULLs, being removed when we
**	    execute these erroneous base table/secondary index ADE_COMPAREs and
**	    return from ade_execute_cx() with an excb_value of ADE_BOTHNULL.
**	    This change fixes bug 116570.
[@history_template@]...
*/
VOID
opc_tjoin_build(
		OPS_STATE   *global,
		OPC_NODE    *cnode)
{
    OPS_SUBQUERY	*subqry = global->ops_cstate.opc_subqry;
    OPO_CO		*co = cnode->opc_cco;
    OPC_EQ		*ceq = cnode->opc_ceq;
    OPC_EQ		*oceq;
    OPC_EQ		*iceq;
    QEN_NODE		*qn = cnode->opc_qennode;
    QEN_TJOIN		*jn = &cnode->opc_qennode->node_qen.qen_tjoin;
    RDR_INFO		*rel;
    OPV_VARS		*opv;
    QEF_VALID		*valid;
    QEN_PART_INFO	*partp;
    i4			tidatt;
    i4			ev_size;
    i4			notused;
    i4			specialrow;
    i4			tidlength;

    /* compile the outer node, and fill in the sorted info */
    opc_jinouter(global, cnode, co->opo_outer, OPC_COOUTER, 
	&jn->tjoin_out, &notused, &oceq);

    /* set up the inner relation to be read */
    opv = subqry->ops_vars.opv_base->opv_rt[co->opo_inner->opo_union.opo_orig];
    rel = opv->opv_grv->opv_relation;

    {
	i4	pages_touched;

	/* Prevent integer overflow */
	pages_touched =
	    (co->opo_cost.opo_pagestouched < (OPO_BLOCKS)MAXI4) ?
	    (i4)co->opo_cost.opo_pagestouched : MAXI4;
	valid = opc_valid(global, opv->opv_grv, DMT_S, 
			  pages_touched, (bool)TRUE);
    }

    jn->tjoin_get = valid->vl_dmr_cb;

    if (rel->rdr_parts)
    {
	/* Allocate/init QEN_PART_INFO for partitioned table. */
	partp = (QEN_PART_INFO *)opu_qsfmem(global, sizeof(QEN_PART_INFO));
	jn->tjoin_part = partp;
	partp->part_gcount = 0;
	partp->part_qual.part_qpart = NULL;
	partp->part_qual.part_qnext = NULL;
	partp->part_qual.part_qdim = NULL;
	partp->part_pcdim = NULL;
	partp->part_qpix = -1;
	partp->part_qpwlix = -1;
	partp->part_qpwpix = -1;
	partp->part_dmrix = valid->vl_dmr_cb;
	partp->part_dmtix = valid->vl_dmf_cb;
	partp->part_totparts = valid->vl_partition_cnt;
	partp->part_threads = 1;	/* for now, Tjoin executes under
					** a single thread */
    }
    else jn->tjoin_part = (QEN_PART_INFO *) NULL;

    if (oceq[co->opo_sjeqc].opc_eqavailable == FALSE)
    {
	opx_error(E_OP0889_EQ_NOT_HERE);
    }
    if (oceq[co->opo_sjeqc].opc_attavailable == FALSE)
    {
	opx_error(E_OP0888_ATT_NOT_HERE);
    }
    jn->tjoin_flag = 0;

    /* record where the uncoerced outer tid will be */
    jn->tjoin_orow = oceq[co->opo_sjeqc].opc_dshrow;
    jn->tjoin_ooffset = oceq[co->opo_sjeqc].opc_dshoffset;
    tidlength = oceq[co->opo_sjeqc].opc_eqcdv.db_length;
    if (oceq[co->opo_sjeqc].opc_eqcdv.db_datatype < 0)
	-- tidlength;			/* Nullable, don't count null byte */
    if (tidlength == 4)
	jn->tjoin_flag |= TJOIN_4BYTE_TIDP;

    /*
    ** Find a spot for the inner row, and fill in its eq's and att's.
    ** Remember that the TID needs to be materialized, so row width
    ** needs to be increased.
    */
    {
	i4 align;

	qn->qen_rsz = (i4) rel->rdr_rel->tbl_width;

	/* Make sure that we can store TID at an aligned location. */ 
	align = qn->qen_rsz % sizeof (ALIGN_RESTRICT);
	if (align != 0)
	    align = sizeof (ALIGN_RESTRICT) - align;
	qn->qen_rsz += (align + DB_TID8_LENGTH);

	opc_ptrow(global, &qn->qen_row, qn->qen_rsz);
    }

    /* Figure out what is coming out of the inner relation */
    ev_size = subqry->ops_eclass.ope_ev * sizeof (OPC_EQ);
    iceq = (OPC_EQ *) opu_Gsmemory_get(global, ev_size);
    MEfill(ev_size, (u_char)0, (PTR)iceq);

    opc_ratt(global, iceq, co->opo_inner, qn->qen_row, qn->qen_rsz);

    /* record where the inner tid will be */
    jn->tjoin_irow = iceq[co->opo_sjeqc].opc_dshrow;
    jn->tjoin_ioffset = iceq[co->opo_sjeqc].opc_dshoffset;

    /* Generate the CX to determine if true TID value is null. */
    opc_tisnull(global, cnode, oceq, co->opo_inner->opo_union.opo_orig,
	&jn->tjoin_isnull);

    /* merge iceq and oceq into ceq */
    opc_jmerge(global, cnode, iceq, oceq, ceq);
    cnode->opc_ceq = ceq;

    /* Initialize the opc info for special OJ eqcs. */
    opc_ojspfatts(global, cnode, &specialrow);

    /*
    ** Initialize the outer join info. Do this before we build the WHERE
    ** clause so that the WHERE clause can refer to variables filled in
    ** by the LNULL, RNULL, and OJEQUAL compiled expressions.
    */
    opc_ojinfo_build(global, cnode, iceq, oceq);
    if (jn->tjoin_oj != (QEN_OJINFO *) NULL)
	jn->tjoin_oj->oj_specialEQCrow = specialrow;

    /* Qualify the joined tuple. Remember to compare only non-key eqcs. */
    {
	OPE_BMEQCLS	keqcmp;
	OPE_IEQCLS	eqcno;
	OPE_IEQCLS	maxeqcls = subqry->ops_eclass.ope_ev;

	/* Figure out key eqcs at this node */
	MEcopy((PTR)&cnode->opc_cco->opo_inner->opo_maps->opo_eqcmap,
	    sizeof(keqcmp), (PTR)&keqcmp);
	BTand((i4)maxeqcls,
	    (char *)&cnode->opc_cco->opo_outer->opo_maps->opo_eqcmap,
							    (char *)&keqcmp);

	for (eqcno = -1; (eqcno = BTnext(eqcno,
	     (char *)&keqcmp, subqry->ops_eclass.ope_ev)) > -1; )
	{
	    bool	    noninneroj = FALSE;
	    OPL_IOUTER	    ojid;

	    for (ojid = -1; (ojid = BTnext((i4)ojid, (char *)&subqry->
		ops_eclass.ope_base->ope_eqclist[eqcno]->ope_ojs,
		(i4)subqry->ops_oj.opl_lv)) >= 0;)
	    {
		if ((subqry->ops_oj.opl_base->opl_ojt[ojid]->opl_type !=
								OPL_INNERJOIN)
		    &&
		    (subqry->ops_oj.opl_base->opl_ojt[ojid]->opl_type !=
								OPL_FOLDEDJOIN))
		{
		    noninneroj = TRUE;
		    break;
		}
	    }
	    if ((subqry->ops_eclass.ope_base->ope_eqclist[eqcno]->ope_eqctype
		== OPE_NONTID)
		&&
		(noninneroj == FALSE))
	    {
		BTclear((i4)eqcno, (char *)&keqcmp);
	    }
	}

	opc_jqual(global, cnode, (OPE_BMEQCLS *)&keqcmp, iceq,
	    oceq, &jn->tjoin_jqual, &tidatt);
    }

    /* eliminate the eqcs and atts from ceq that won't be returned from
    **	    this node;
    **
    **	EJLFIX: We can't call opc_jnotret() right now because if there is
    **	    a func att that the inner orig node need to create then we
    **	    can't eliminate the attributes that are needed to create the
    **	    func att. Find a better way around this (like get 
    **	    opc_qentree_build() to call opc_jnotret for all (or most) nodes.
    **
    ** opc_jnotret(global, co, ceq);
    */
}

/*{
** Name: OPC_KJOIN_BUILD	- Compile an QEN_KJOIN node.
**
** Description:
**      This routine builds a QEN_KJOIN node from a co node. This is done 
**      by first compiling the outer node. Then we pick a DSH row to hold 
**      the inner row, and decide which of the inner attributes will be 
**      needed. Next we figure out which of the inner and outer attributes 
**      the calling procedure needs to see. Then we fill in the keying
**	information for the inner relation (kjoin_key, kjoin_kcompare, and
**	kjoin_kys). Finally, we compile the qualification to be applied 
**	at this node. 
[@comment_line@]...
**
** Inputs:
**	global -
**	    This is the query wide state variable
**	cnode -
**	    This describes various info about the QEN node that we are
**	    compiling.
**
** Outputs:
**	cnode->opc_qennode -
**	    This will be completely filled in.
**	cnode->opc_ceq -
**	    These will tell what eqcs and attrs that this node returns.
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
**      25-aug-86 (eric)
**          written
**	13-jul-90 (stec)
**	    Modified call to opc_valid() to prevent integer overflow.
**	16-apr-91 (davebf)
**	    Added parm to opc_valid() call in order to enable vl_size_sensitive
**      17-aug-93 (anitap)
**          Typecast the values in the conditional tests involving dissimilar
**          types like float and MAXI4. The lack of these casts caused by
**          floating point exceptions on a certain machine. 
**	5-jan-94 (rickh)
**	    The WHERE clause is executed AFTER the ON clause and outer join null
**	    fabricators.  Therefore, it's clearer if we compile the WHERE
**	    clause before the outer join CXs.  One happy advantage of this
**	    approach is that it forces the WHERE clause to refer to the
**	    same locations as the outer join CXs.
**	6-jun-96 (inkdo01)
**	    Added support of spatial key joins.
**	28-may-97 (inkdo01)
**	    Fix TID qualification stuff for key joins.
**	12-nov-98 (inkdo01)
**	    Set kjoin_ixonly flag for Btree base table access which doesn't
**	    need access to data pages.
**	14-may-99 (inkdo01)
**	    Changes to speed up non-outer kjoins. Call to opc_ijkqual() (if
**	    this is not a spatial key join nor an outer join) which builds
**	    a single qualification expression from all non-outer key join
**	    predicates. This helps non-outer key joins to run much faster.
**	    This change fixes bug 96840.
**      25-jun-99 (inkdo01)
**          Identify key columns in join predicates to remove them from
**          key join kcompare code (since compare is already done in DMF).
**      15-oct-2003 (chash01)
**          Add rel->rdr_rel->tbl_relgwid == DMGW_RMS as last arg of 
**          opc_ijkqual() to fix rms gateway bug 110758 
**	2-feb-04 (inkdo01)
**	    Init kjoin_part for table partitioning.
**	3-feb-04 (inkdo01)
**	    Change buffer to hold TID8 for table partitioning.
**	13-apr-04 (inkdo01)
**	    Fill out the QEN_PART_INFO init.
**	22-Jul-2005 (schka24)
**	    Init new work-bitmap in partinfo.
**	16-Jun-2006 (kschendel)
**	    part-info qual stuff moved around, fix here.
[@history_template@]...
*/
VOID
opc_kjoin_build(
		OPS_STATE   *global,
		OPC_NODE    *cnode)
{
    OPS_SUBQUERY	*subqry = global->ops_cstate.opc_subqry;
    OPO_CO		*co = cnode->opc_cco;
    OPC_EQ		*ceq = cnode->opc_ceq;
    OPC_EQ		*oceq;
    OPC_EQ		*iceq;
    QEN_NODE		*qn = cnode->opc_qennode;
    QEN_KJOIN		*jn = &cnode->opc_qennode->node_qen.qen_kjoin;
    RDR_INFO		*rel;
    QEN_PART_INFO	*partp;
    OPV_VARS		*opv;
    i4			dummy;
    QEF_VALID		*valid;
    i4			align;
    i4			ev_size;
    i4			tidatt;
    i4			specialrow;
    OPE_IEQCLS		sjeqc;
    OPE_BMEQCLS		keymap;
    bool		spjoin = FALSE;

    /* compile the outer node, and fill in the sorted info */
    opc_jinouter(global, cnode, co->opo_outer, OPC_COOUTER, 
	&jn->kjoin_out, &dummy, &oceq);

    /* set up the inner relation to be read */
    opv = subqry->ops_vars.opv_base->opv_rt[co->opo_inner->opo_union.opo_orig];
    rel = opv->opv_grv->opv_relation;

    {
	i4	pages_touched;

	/* Prevent integer overflow */
	pages_touched =
	    (co->opo_cost.opo_pagestouched < (OPO_BLOCKS)MAXI4) ?
	    (i4)co->opo_cost.opo_pagestouched : MAXI4;
	valid = opc_valid(global, opv->opv_grv, DMT_S, 
			  pages_touched, (bool)TRUE);
    }

    jn->kjoin_get = valid->vl_dmr_cb;

    if (rel->rdr_parts)
    {
	/* Allocate/init QEN_PART_INFO for partitioned table. */
	partp = (QEN_PART_INFO *)opu_qsfmem(global, sizeof(QEN_PART_INFO));
	jn->kjoin_part = partp;
	partp->part_gcount = 0;
	partp->part_qual.part_qpart = NULL;
	partp->part_qual.part_qnext = NULL;
	partp->part_qual.part_qdim = NULL;
	partp->part_pcdim = NULL;
	partp->part_qpix = -1;
	partp->part_qpwlix = -1;
	partp->part_qpwpix = -1;
	partp->part_dmrix = valid->vl_dmr_cb;
	partp->part_dmtix = valid->vl_dmf_cb;
	partp->part_totparts = valid->vl_partition_cnt;
	partp->part_threads = 1;	/* for now, Kjoin executes under
					** a single thread */
    }
    else jn->kjoin_part = (QEN_PART_INFO *) NULL;
    jn->kjoin_nloff = -1;
    jn->kjoin_tqual = FALSE;
    jn->kjoin_ixonly = (opv->opv_mask & OPV_BTSSUB);
			/* set "index access only" flag */
    jn->kjoin_pcompat = FALSE;

    /* Find a spot for the inner row */
    qn->qen_rsz = (i4) rel->rdr_rel->tbl_width;
    if (opv->opv_primary.opv_tid == OPE_NOEQCLS)
    {
	jn->kjoin_tid = -1;
    }
    else
    {
	align = qn->qen_rsz % sizeof (ALIGN_RESTRICT);
	if (align != 0)
	    align = sizeof (ALIGN_RESTRICT) - align;
	qn->qen_rsz += (align + DB_TID8_LENGTH);

	jn->kjoin_tid = qn->qen_rsz - DB_TID8_LENGTH;
    }

    opc_ptrow(global, &qn->qen_row, qn->qen_rsz);

    /* fill in the inner rows eqc's */
    ev_size = subqry->ops_eclass.ope_ev * sizeof (OPC_EQ);
    iceq = (OPC_EQ *) opu_Gsmemory_get(global, ev_size);
    MEfill(ev_size, (u_char)0, (PTR)iceq);

    opc_ratt(global, iceq, co->opo_inner, qn->qen_row, qn->qen_rsz);

    /* Now lets merge iceq and oceq into ceq */
    opc_jmerge(global, cnode, iceq, oceq, ceq);
    cnode->opc_ceq = ceq;

    /* fill in kjoin_key, kjoin_kcompare, and kjoin_kys */
    /* EJLFIX: use a smaller portion of the tree than subqry->ops_root */
    /* First step is to distinguish spatial join from regular key join,
    ** and split the logic accordingly. */

    MEfill(sizeof(keymap), (u_char)0, (PTR)&keymap);

    if (rel->rdr_rel->tbl_storage_type == DB_RTRE_STORE &&
	(sjeqc = co->opo_sjeqc) >= 0 &&
	subqry->ops_eclass.ope_base->ope_eqclist[sjeqc]->ope_mask &
	OPE_SPATJ)
    {
	if (opc_spjoin(global, cnode, co, oceq, ceq)) spjoin = TRUE;
	else opx_error(E_OP08AD_SPATIAL_ERROR);
    }
    else opc_kjkeyinfo(global, cnode, rel, subqry->ops_root->pst_right, 
	               oceq, iceq, &keymap);

    /* If this is a key join that has no keys, then change the type to a
    ** physical join. This is done, as a mild hack, for QEF since keyed
    ** accesses to relations and full scans are very different.
    */
    if (jn->kjoin_kys == NULL)
    {
	/*FIXME*/
	/*qn->qen_type = QE_PJOIN;*/

	/* physical joins are not supported any longer */
	opx_error(E_OP089B_PJOIN_FOUND);
    }

    /* For non-outer joins, combine all the qualification code into 
    ** kjoin_iqual and set the rest to NULL. This makes kjoins 
    ** considerably faster. */

    if (cnode->opc_cco->opo_variant.opo_local->opo_ojid == OPL_NOOUTER
	&& !spjoin)
    {
	jn->kjoin_kqual = NULL;
	jn->kjoin_jqual = NULL;
	jn->kjoin_oj = NULL;
	opc_ijkqual(global, cnode, iceq, oceq, &keymap, &jn->kjoin_iqual,
                    &jn->kjoin_tqual, rel->rdr_rel->tbl_relgwid == DMGW_RMS);
	return;
    }

    /* qualify the retrieved tuple; */
    if (!spjoin) opc_iqual(global, cnode, iceq, &jn->kjoin_iqual,
							&jn->kjoin_tqual);
    else jn->kjoin_iqual = NULL;

    /* Apply full key qualification to the joined tuple. */
    if (!spjoin) opc_kqual(global, cnode, iceq, oceq, &jn->kjoin_kqual);
    else jn->kjoin_kqual = NULL;

    /* Initialize the opc info for special OJ eqcs. */
    opc_ojspfatts(global, cnode, &specialrow);

    /*
    ** Initialize the outer join info. Do this before we build the WHERE
    ** clause so that the WHERE clause can refer to variables filled in
    ** by the LNULL, RNULL, and OJEQUAL compiled expressions.
    */
    opc_ojinfo_build(global, cnode, iceq, oceq);
    if (jn->kjoin_oj != (QEN_OJINFO *) NULL)
	jn->kjoin_oj->oj_specialEQCrow = specialrow;

    if (!spjoin) opc_jqual(global, cnode, (OPE_BMEQCLS *)NULL, iceq,
	oceq, &jn->kjoin_jqual, &tidatt);
    else jn->kjoin_jqual = NULL;

    /* eliminate the eqcs and atts from ceq that won't be returned from
    **	    this node;
    **
    **	EJLFIX: We can't call opc_jnotret() right now because if there is
    **	    a func att that the inner orig node need to create then we
    **	    can't eliminate the attributes that are needed to create the
    **	    func att. Find a better way around this (like get 
    **	    opc_qentree_build() to call opc_jnotret for all (or most) nodes.
    **
    ** opc_jnotret(global, co, ceq);
    */
}

/*{
** Name: OPC_HJOIN_BUILD	- Compile a QEN_HJOIN node.
**
** Description:
{@comment_line@}...
**
** Inputs:
[@PARAM_DESCR@]...
**
** Outputs:
[@PARAM_DESCR@]...
**	Returns:
**	    {@return_description@}
**	Exceptions:
**	    [@description_or_none@]
**
** Side Effects:
**	    [@description_or_none@]
**
** History:
**      26-feb-99 (inkdo01)
**	    Cloned from opc_fsmjoin_build for hash joins.
**	8-feb-00 (inkdo01)
**	    Add support for build source duplicates removal.
**	26-june-01 (inkdo01)
**	    Added code to augment join node EQCs for outer joins in case not
**	    null col from inner is in "is null" pred in where clause (copied 
**	    from fsmjoin_build).
**	29-oct-01 (inkdo01)
**	    Silly me - I had left and right joins reversed in flag sent to 
**	    hash_materialize. This screwed up hash buffer oj indicators and 
**	    made some non-null columns look null.
**	19-apr-04 (inkdo01)
**	    Partition compatible join support.
**	7-Dec-2005 (kschendel)
**	    Build a compare-list instead of an attribute-list for keys.
[@history_template@]...
*/
VOID
opc_hjoin_build(
		OPS_STATE   *global,
		OPC_NODE    *cnode)
{
    OPO_CO		*co = cnode->opc_cco;
    OPC_EQ		*ceq = cnode->opc_ceq;
    OPC_EQ		*oceq;
    OPC_EQ		*iceq;
    QEN_NODE		*qn = cnode->opc_qennode;
    QEN_HJOIN		*jn = &cnode->opc_qennode->node_qen.qen_hjoin;
    OPE_BMEQCLS		keqcmp;
    OPC_EQ		*okceq;
    i4			dummy;
    i4			njeqcs;
    i4			tidatt;
    i4			notused;
    i4			brows;
    i4			specialrow;
    bool		ljoin, rjoin;

    /* Compile the outer node, and fill in the sorted info */
    opc_jinouter(global, cnode, co->opo_outer, OPC_COOUTER, 
	&jn->hjn_out, &notused, &oceq);

    /* Compile the inner node and fill in the sorted info */
    opc_jinouter(global, cnode, co->opo_inner, OPC_COOUTER,
	&jn->hjn_inner, &notused, &iceq);

    /* Fill in the hash structure number */
    opc_pthash(global, &jn->hjn_hash);

    /* CB number for hash partition file CB */
    opc_ptcb(global, &jn->hjn_dmhcb, sizeof(DMH_CB));

    /* Initialize keqcmp, okceq. */
    if (co->opo_sjeqc == OPE_NOEQCLS)
	njeqcs = 0;
    else
	njeqcs = 1;

    /* If outer join, make sure node EQCs include all the EQCs of the inner 
    ** side of the outer join. This assures that "not null" columns from the
    ** inner whose only reference is in the where clause (in a "is null" pred)
    ** get null indicator space allocated. */
    if (co->opo_variant.opo_local->opo_ojid != OPL_NOOUTER)
     switch (co->opo_variant.opo_local->opo_type) {
      case OPL_RIGHTJOIN:
	BTor( (i4) BITS_IN( OPE_BMEQCLS ),
	    (char *)&co->opo_outer->opo_maps->opo_eqcmap,
	    (char *)&co->opo_maps->opo_eqcmap);
	break;
      case OPL_LEFTJOIN:
	BTor( (i4) BITS_IN( OPE_BMEQCLS ),
	    (char *)&co->opo_inner->opo_maps->opo_eqcmap,
	    (char *)&co->opo_maps->opo_eqcmap);
	break;
      case OPL_FULLJOIN:
	BTor( (i4) BITS_IN( OPE_BMEQCLS ),
	    (char *)&co->opo_outer->opo_maps->opo_eqcmap,
	    (char *)&co->opo_maps->opo_eqcmap);
	BTor( (i4) BITS_IN( OPE_BMEQCLS ),
	    (char *)&co->opo_inner->opo_maps->opo_eqcmap,
	    (char *)&co->opo_maps->opo_eqcmap);
	break;
    }

    opc_jkinit(global, &co->opo_sjeqc, njeqcs, oceq, 
	&keqcmp, &okceq);

    /* build compare-list array to describe hash keys */
    jn->hjn_nullablekey = opc_hashatts(global, &keqcmp, oceq, iceq,
				 &jn->hjn_cmplist, &jn->hjn_hkcount);

    /* Set oj flags for build/probe tuple materialization. */
    ljoin = rjoin = FALSE;
    if (co->opo_variant.opo_local->opo_ojid != OPL_NOOUTER)
     switch (co->opo_variant.opo_local->opo_type) {
      case OPL_LEFTJOIN:
	ljoin = TRUE;
	break;
      case OPL_RIGHTJOIN:
	rjoin = TRUE;
	break;
      case OPL_FULLJOIN:
	ljoin = rjoin = TRUE;
	break;
    }
    /* Fill in the CO flag */
    jn->hjn_kuniq = (co->opo_existence != 0);

    /* materialize the outer tuple; */
    opc_hash_materialize(global, cnode, &jn->hjn_btmat, &co->opo_outer->opo_maps->opo_eqcmap, 
	&keqcmp, oceq, jn->hjn_cmplist, &jn->hjn_brow, &qn->qen_rsz, (ljoin | rjoin));
    /*	(rjoin) ? 1 : 0); */

    /* Compute size of build source as row count times row size, allowing 
    ** for build size > MAXI4. */
    brows = (co->opo_outer->opo_cost.opo_tups < (OPO_TUPLES)MAXI4) ? 
			(i4)co->opo_outer->opo_cost.opo_tups : MAXI4;
    jn->hjn_bmem = (MAXI4 / qn->qen_rsz > brows) ? brows * qn->qen_rsz : MAXI4;

    /* materialize the inner tuple; */
    opc_hash_materialize(global, cnode, &jn->hjn_ptmat, &co->opo_inner->opo_maps->opo_eqcmap, 
	&keqcmp, iceq, jn->hjn_cmplist, &jn->hjn_prow, &qn->qen_rsz, (ljoin | rjoin));
    /*	(ljoin) ? 1 : 0); */
    qn->qen_row = jn->hjn_prow;		/* stick something there */

    /* Merge iceq and oceq into ceq */
    opc_jmerge(global, cnode, iceq, oceq, ceq);
    cnode->opc_ceq = ceq;

    /* Initialize the opc info for special OJ eqcs. */
    opc_ojspfatts(global, cnode, &specialrow);

    /*
    ** Initialize the outer join info. Do this before we build the WHERE
    ** clause so that the WHERE clause can refer to variables filled in
    ** by the LNULL, RNULL, and OJEQUAL compiled expressions.
    */
    opc_ojinfo_build(global, cnode, iceq, oceq);
    if (jn->hjn_oj != (QEN_OJINFO *) NULL)
	jn->hjn_oj->oj_specialEQCrow = specialrow;

    /* qualify the joined tuple; */
    opc_jqual(global, cnode, &keqcmp, iceq, oceq, 
	&jn->hjn_jqual, &tidatt);

    /* If CO node has opo_local->opo_pgcount is non-0, set partition
    ** group processing flag - this is partition compatible join. */
    if (co->opo_variant.opo_local->opo_pgcount)
    {
	qn->qen_flags |= QEN_PART_SEPARATE;
	if (jn->hjn_out->qen_type == QE_TSORT)
	    jn->hjn_out->qen_flags |= QEN_PART_SEPARATE;
	if (jn->hjn_inner->qen_type == QE_SORT)
	    jn->hjn_inner->qen_flags |= QEN_PART_SEPARATE;
    }

    /* 
    ** Eliminate the eqcs from ceq that
    ** won't be returned from this node. However, because of function attribute
    ** processing we must not do this. The call is commented out to fix
    ** bug 34959.
    */
    /* opc_jnotret(global, co, ceq ); */

    /* Finally, if no keys are nullable, reduce the join key to a single
    ** BYTE(n) key of the proper length.  We can get away with this
    ** because hash-materialize has to preprocess the keys for hashing,
    ** so a straight bit-comparison on the full join key works.
    ** (Do this even on a single key, since BYTE is always easy to compare.)
    */
    if (! jn->hjn_nullablekey)
    {
	DB_CMP_LIST *cmp = &jn->hjn_cmplist[0];

	cmp->cmp_type = DB_BYTE_TYPE;
	cmp->cmp_length = jn->hjn_cmplist[jn->hjn_hkcount-1].cmp_offset
		+ jn->hjn_cmplist[jn->hjn_hkcount-1].cmp_length;
	cmp->cmp_precision = 0;
	cmp->cmp_collID = -1;
	jn->hjn_hkcount = 1;
    }
}

/*{
** Name: OPC_FSMJOIN_BUILD	- Compile an QEN_JOIN node.
**
** Description:
{@comment_line@}...
**
** Inputs:
[@PARAM_DESCR@]...
**
** Outputs:
[@PARAM_DESCR@]...
**	Returns:
**	    {@return_description@}
**	Exceptions:
**	    [@description_or_none@]
**
** Side Effects:
**	    [@description_or_none@]
**
** History:
**      31-oct-89 (stec)
**          written
**	08-jan-91 (stec)
**	    Call to opc_jnotret() has been commented out to fix
**	    bug 34959.
**	29-jan-91 (stec)
**	    Changed code calling opc_materialize() to reflect interface change.
**	25-mar-92 (rickh)
**	    Right joins now store tids in a real hold file.  Some related
**	    cleanup:  removed references to jn->sjn_hget and jn->sjn_hcreate,
**	    which weren't used anywhere in QEF.
**	5-jan-94 (rickh)
**	    The WHERE clause is executed AFTER the ON clause and outer join null
**	    fabricators.  Therefore, it's clearer if we compile the WHERE
**	    clause before the outer join CXs.  One happy advantage of this
**	    approach is that it forces the WHERE clause to refer to the
**	    same locations as the outer join CXs.
**	13-aug-99 (inkdo01)
**	    Added code to augment join node EQCs for outer joins in case not
**	    null col from inner is in "is null" pred in where clause.
[@history_template@]...
*/
VOID
opc_fsmjoin_build(
		OPS_STATE   *global,
		OPC_NODE    *cnode)
{
    OPO_CO		*co = cnode->opc_cco;
    OPC_EQ		*ceq = cnode->opc_ceq;
    OPC_EQ		*oceq;
    OPC_EQ		*iceq;
    QEN_NODE		*qn = cnode->opc_qennode;
    QEN_SJOIN		*jn = &cnode->opc_qennode->node_qen.qen_sjoin;
    OPE_BMEQCLS		keqcmp;
    OPC_EQ		*okceq;
    i4			dummy;
    i4			njeqcs;
    i4			tidatt;
    i4			notused;
    i4			specialrow;

    /* Compile the outer node, and fill in the sorted info */
    opc_jinouter(global, cnode, co->opo_outer, OPC_COOUTER, 
	&jn->sjn_out, &notused, &oceq);

    /* Compile the inner node and fill in the sorted info */
    opc_jinouter(global, cnode, co->opo_inner, OPC_COINNER,
	&jn->sjn_inner, &notused, &iceq);

    /* Fill in the hold file info */
    opc_pthold(global, &jn->sjn_hfile);

    /* Fill in the uniqueness info */
    jn->sjn_kuniq = (i4) co->opo_existence;

    /* Initialize keqcmp, okceq. */
    if (co->opo_sjeqc == OPE_NOEQCLS)
	njeqcs = 0;
    else
	njeqcs = 1;

    /* If outer join, make sure node EQCs include all the EQCs of the inner 
    ** side of the outer join. This assures that "not null" columns from the
    ** inner whose only reference is in the where clause (in a "is null" pred)
    ** get null indicator space allocated. */
    if (co->opo_variant.opo_local->opo_ojid != OPL_NOOUTER)
     switch (co->opo_variant.opo_local->opo_type) {
      case OPL_RIGHTJOIN:
	BTor( (i4) BITS_IN( OPE_BMEQCLS ),
	    (char *)&co->opo_outer->opo_maps->opo_eqcmap,
	    (char *)&co->opo_maps->opo_eqcmap);
	break;
      case OPL_LEFTJOIN:
	BTor( (i4) BITS_IN( OPE_BMEQCLS ),
	    (char *)&co->opo_inner->opo_maps->opo_eqcmap,
	    (char *)&co->opo_maps->opo_eqcmap);
	break;
      case OPL_FULLJOIN:
	BTor( (i4) BITS_IN( OPE_BMEQCLS ),
	    (char *)&co->opo_outer->opo_maps->opo_eqcmap,
	    (char *)&co->opo_maps->opo_eqcmap);
	BTor( (i4) BITS_IN( OPE_BMEQCLS ),
	    (char *)&co->opo_inner->opo_maps->opo_eqcmap,
	    (char *)&co->opo_maps->opo_eqcmap);
	break;
    }

    opc_jkinit(global, &co->opo_sjeqc, njeqcs, oceq, 
	&keqcmp, &okceq);

    /* materialize the inner tuple; */
    opc_materialize(global, cnode, &jn->sjn_itmat, &co->opo_inner->opo_maps->opo_eqcmap, 
	iceq, &qn->qen_row, &qn->qen_rsz, (i4)TRUE, (bool)TRUE, (bool)FALSE);

    /* materialize the outer key; */
    opc_materialize(global, cnode, &jn->sjn_okmat, &keqcmp, okceq, 
	&jn->sjn_krow, &dummy, (i4)TRUE, (bool)TRUE, (bool)FALSE);

    /* compare the materialized outer key to the current outer key; */
    opc_kcompare(global, cnode, &jn->sjn_okcompare, co->opo_sjeqc, 
	okceq, oceq);

    /* compare the inner and outer tuples on the join key; */
    opc_kcompare(global, cnode, &jn->sjn_joinkey, co->opo_sjeqc, 
	okceq, iceq);

    /* Merge iceq and oceq into ceq */
    opc_jmerge(global, cnode, iceq, oceq, ceq);
    cnode->opc_ceq = ceq;

    /* Initialize the opc info for special OJ eqcs. */
    opc_ojspfatts(global, cnode, &specialrow);

    /*
    ** Initialize the outer join info. Do this before we build the WHERE
    ** clause so that the WHERE clause can refer to variables filled in
    ** by the LNULL, RNULL, and OJEQUAL compiled expressions.
    */
    opc_ojinfo_build(global, cnode, iceq, oceq);
    if (jn->sjn_oj != (QEN_OJINFO *) NULL)
	jn->sjn_oj->oj_specialEQCrow = specialrow;

    /* qualify the joined tuple; */
    opc_jqual(global, cnode, &keqcmp, iceq, oceq, 
	&jn->sjn_jqual, &tidatt);

    /* If CO node has opo_local->opo_pgcount is non-0, set partition
    ** group processing flag - this is partition compatible join. */
    if (co->opo_variant.opo_local->opo_pgcount)
    {
	qn->qen_flags |= QEN_PART_SEPARATE;
	if (jn->sjn_out->qen_type == QE_TSORT)
	    jn->sjn_out->qen_flags |= QEN_PART_SEPARATE;
	if (jn->sjn_inner->qen_type == QE_SORT)
	    jn->sjn_inner->qen_flags |= QEN_PART_SEPARATE;
    }

    /* 
    ** Eliminate the eqcs from ceq that
    ** won't be returned from this node. However, because of function attribute
    ** processing we must not do this. The call is commented out to fix
    ** bug 34959.
    */
    /* opc_jnotret(global, co, ceq ); */
}

/*{
** Name: OPC_CPJOIN_BUILD	- Compile an QEN_CPJOIN node.
**
** Description:
{@comment_line@}...
**
** Inputs:
[@PARAM_DESCR@]...
**
** Outputs:
[@PARAM_DESCR@]...
**	Returns:
**	    {@return_description@}
**	Exceptions:
**	    [@description_or_none@]
**
** Side Effects:
**	    [@description_or_none@]
**
** History:
**      25-aug-86 (eric)
**          written
**	08-jan-91 (stec)
**	    Call to opc_jnotret() has been commented out to fix
**	    bug 34959.
**	29-jan-91 (stec)
**	    Changed code calling opc_materialize() to reflect interface change.
**	25-mar-92 (rickh)
**	    Right joins now store tids in a real hold file.  Some related
**	    cleanup:  removed references to jn->sjn_hget and jn->sjn_hcreate,
**	    which weren't used anywhere in QEF.
**	5-jan-94 (rickh)
**	    The WHERE clause is executed AFTER the ON clause and outer join null
**	    fabricators.  Therefore, it's clearer if we compile the WHERE
**	    clause before the outer join CXs.  One happy advantage of this
**	    approach is that it forces the WHERE clause to refer to the
**	    same locations as the outer join CXs.
[@history_template@]...
*/
VOID
opc_cpjoin_build(
		OPS_STATE   *global,
		OPC_NODE    *cnode)
{
    OPO_CO		*co = cnode->opc_cco;
    OPC_EQ		*ceq = cnode->opc_ceq;
    OPC_EQ		*oceq;
    OPC_EQ		*iceq;
    QEN_NODE		*qn = cnode->opc_qennode;
    QEN_SJOIN		*jn = &cnode->opc_qennode->node_qen.qen_sjoin;
    OPE_BMEQCLS		keqcmp;
    OPC_EQ		*okceq;
    i4			dummy;
    i4			njeqcs;
    i4			tidatt;
    i4			notused;
    i4			specialrow;

    /* Compile the outer node, and fill in the sorted info */
    opc_jinouter(global, cnode, co->opo_outer, OPC_COOUTER, 
	&jn->sjn_out, &notused, &oceq);

    /* Compile the inner node and fill in the sorted info */
    opc_jinouter(global, cnode, co->opo_inner, OPC_COINNER,
	&jn->sjn_inner, &notused, &iceq);

    /* Fill in the hold file info */
    opc_pthold(global, &jn->sjn_hfile);

    /* Fill in the uniqueness info */
    jn->sjn_kuniq = (i4) co->opo_existence;

    /* Initialize keqcmp and okceq. */
    if (co->opo_sjeqc == OPE_NOEQCLS)
	njeqcs = 0;
    else
	njeqcs = 1;

    opc_jkinit(global, &co->opo_sjeqc, njeqcs, oceq, 
	&keqcmp, &okceq);

    /* materialize the inner tuple; */
    opc_materialize(global, cnode, &jn->sjn_itmat, &co->opo_inner->opo_maps->opo_eqcmap, 
	iceq, &qn->qen_row, &qn->qen_rsz, (i4)TRUE, (bool)TRUE, (bool)FALSE);

    /* materialize the outer key; */
    opc_materialize(global, cnode, &jn->sjn_okmat, &keqcmp, okceq, 
	&jn->sjn_krow, &dummy, (i4)TRUE, (bool)TRUE, (bool)FALSE);

    /* compare the materialized outer key to the current outer key; */
    opc_kcompare(global, cnode, &jn->sjn_okcompare, co->opo_sjeqc, 
	okceq, oceq);

    /* compare the inner and outer tuples on the join key; */
    opc_kcompare(global, cnode, &jn->sjn_joinkey, co->opo_sjeqc, 
	okceq, iceq);

    /* Merge iceq and oceq into ceq */
    opc_jmerge(global, cnode, iceq, oceq, ceq);
    cnode->opc_ceq = ceq;

    /* Initialize the opc info for special OJ eqcs. */
    opc_ojspfatts(global, cnode, &specialrow);

    /*
    ** Initialize the outer join info. Do this before we build the WHERE
    ** clause so that the WHERE clause can refer to variables filled in
    ** by the LNULL, RNULL, and OJEQUAL compiled expressions.
    */
    opc_ojinfo_build(global, cnode, iceq, oceq);
    if (jn->sjn_oj != (QEN_OJINFO *) NULL)
	jn->sjn_oj->oj_specialEQCrow = specialrow;

    /* qualify the joined tuple; */
    opc_jqual(global, cnode, (OPE_BMEQCLS *)NULL, iceq, oceq,
	&jn->sjn_jqual, &tidatt);

    /* 
    ** Eliminate the eqcs from ceq that
    ** won't be returned from this node. However, because of function attribute
    ** processing we must not do this. The call is commented out to fix
    ** bug 34959.
    */
    /* opc_jnotret(global, co, ceq); */
}

/*{
** Name: OPC_ISJOIN_BUILD	- Compile an QEN_ISJOIN node.
**
** Description:
{@comment_line@}...
**
** Inputs:
[@PARAM_DESCR@]...
**
** Outputs:
[@PARAM_DESCR@]...
**	Returns:
**	    {@return_description@}
**	Exceptions:
**	    [@description_or_none@]
**
** Side Effects:
**	    [@description_or_none@]
**
** History:
**      25-aug-86 (eric)
**          written
**	08-jan-91 (stec)
**	    Call to opc_jnotret() has been commented out to fix
**	    bug 34959.
**	29-jan-91 (stec)
**	    Changed code calling opc_materialize() to reflect interface change.
**	25-mar-92 (rickh)
**	    Right joins now store tids in a real hold file.  Some related
**	    cleanup:  removed references to jn->sjn_hget and jn->sjn_hcreate,
**	    which weren't used anywhere in QEF.
**	5-jan-94 (rickh)
**	    The WHERE clause is executed AFTER the ON clause and outer join null
**	    fabricators.  Therefore, it's clearer if we compile the WHERE
**	    clause before the outer join CXs.  One happy advantage of this
**	    approach is that it forces the WHERE clause to refer to the
**	    same locations as the outer join CXs.
**      08-may-2002 (huazh01)
**          Apply the fix for bug 98395 to opc_isjoin_build(). This fixes
**          bug 107046.
[@history_template@]...
*/
VOID
opc_isjoin_build(
		OPS_STATE   *global,
		OPC_NODE    *cnode)
{
    OPO_CO		*co = cnode->opc_cco;
    OPC_EQ		*ceq = cnode->opc_ceq;
    OPC_EQ		*oceq;
    OPC_EQ		*iceq;
    QEN_SJOIN		*jn = &cnode->opc_qennode->node_qen.qen_sjoin;
    QEN_NODE		*qn = cnode->opc_qennode;
    OPE_BMEQCLS		keqcmp;
    OPC_EQ		*okceq;
    i4			notused;
    i4			njeqcs;
    i4			tidatt;
    i4			specialrow;

    /* compile the outer node, and fill in the sorted info */
    opc_jinouter(global, cnode, co->opo_outer, OPC_COOUTER, 
	&jn->sjn_out, &notused, &oceq);

    /* now lets compile the inner node and fill in the sorted info */
    opc_jinouter(global, cnode, co->opo_inner, OPC_COINNER,
	&jn->sjn_inner, &notused, &iceq);

    /* Fill in the hold file info */
    opc_pthold(global, &jn->sjn_hfile);

    /* Fill in the uniqueness info */
    jn->sjn_kuniq = (i4) co->opo_existence;

    /* Initialize keqcmp, okceq. */
    if (co->opo_sjeqc == OPE_NOEQCLS)
	njeqcs = 0;
    else
	njeqcs = 1;
 
    /* If outer join, make sure node EQCs include all the EQCs of the inner
     ** side of the outer join. This assures that "not null" columns from the
     ** inner whose only reference is in the where clause (in a "is null" pred)
     ** get null indicator space allocated. */ 
    if (co->opo_variant.opo_local->opo_ojid != OPL_NOOUTER)
     switch (co->opo_variant.opo_local->opo_type) {
      case OPL_RIGHTJOIN:
        BTor( (i4) BITS_IN( OPE_BMEQCLS ),
            (char *)&co->opo_outer->opo_maps->opo_eqcmap,
            (char *)&co->opo_maps->opo_eqcmap);
        break;
      case OPL_LEFTJOIN:
        BTor( (i4) BITS_IN( OPE_BMEQCLS ),
            (char *)&co->opo_inner->opo_maps->opo_eqcmap,
            (char *)&co->opo_maps->opo_eqcmap);
        break;
      case OPL_FULLJOIN:
        BTor( (i4) BITS_IN( OPE_BMEQCLS ),
            (char *)&co->opo_outer->opo_maps->opo_eqcmap,
            (char *)&co->opo_maps->opo_eqcmap);
        BTor( (i4) BITS_IN( OPE_BMEQCLS ),
            (char *)&co->opo_inner->opo_maps->opo_eqcmap,
            (char *)&co->opo_maps->opo_eqcmap);
        break;
    } 

    opc_jkinit(global, &co->opo_sjeqc, njeqcs, oceq,
	&keqcmp, &okceq);

    /* materialize the inner tuple; */
    opc_materialize(global, cnode, &jn->sjn_itmat, &co->opo_inner->opo_maps->opo_eqcmap, 
	iceq, &qn->qen_row, &qn->qen_rsz, (i4)TRUE, (bool)TRUE, (bool)FALSE);

    /* materialize the outer key; */
    opc_materialize(global, cnode, &jn->sjn_okmat, &keqcmp, okceq, 
	&jn->sjn_krow, &notused, (i4)TRUE, (bool)TRUE, (bool)FALSE);

    /* compare the materialized outer key to the current outer key; */
    opc_kcompare(global, cnode, &jn->sjn_okcompare, co->opo_sjeqc, 
	okceq, oceq);

    /* compare the inner and outer tuples on the join key; */
    opc_kcompare(global, cnode, &jn->sjn_joinkey, co->opo_sjeqc, 
	okceq, iceq);

    /* Merge iceq and oceq into ceq */
    opc_jmerge(global, cnode, iceq, oceq, ceq);
    cnode->opc_ceq = ceq;

    /* Initialize the opc info for special OJ eqcs. */
    opc_ojspfatts(global, cnode, &specialrow);

    /*
    ** Initialize the outer join info. Do this before we build the WHERE
    ** clause so that the WHERE clause can refer to variables filled in
    ** by the LNULL, RNULL, and OJEQUAL compiled expressions.
    */
    opc_ojinfo_build(global, cnode, iceq, oceq);
    if (jn->sjn_oj != (QEN_OJINFO *) NULL)
	jn->sjn_oj->oj_specialEQCrow = specialrow;

    /* qualify the joined tuple; */
    opc_jqual(global, cnode, &keqcmp, iceq, oceq, 
	&jn->sjn_jqual, &tidatt);

    /* 
    ** Eliminate the eqcs from ceq that
    ** won't be returned from this node;
    ** won't be returned from this node. However, because of function attribute
    ** processing we must not do this. The call is commented out to fix
    ** bug 34959.
    */
    /* opc_jnotret(global, co, ceq); */
}
