/*
** Copyright (c) 2004, Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ddb.h>
#include    <ulm.h>
#include    <ulf.h>
#include    <adf.h>
#include    <dmf.h>
#include    <dmtcb.h>
#include    <scf.h>
#include    <qsf.h>
#include    <qefrcb.h>
#include    <rdf.h>
#include    <psfparse.h>
#include    <qefnode.h>
#include    <qefact.h>
#include    <qefqp.h>
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

/* external routine declarations definitions */
#include    <me.h>
#include    <opxlint.h> /* EXTERNAL DEFINITION */ 


/**
**
**  Name: OPBPQBF.C - find matching boolean factors for partitioned tables
**
**  Description:
**      These routines will build the OPB_PQBF structures for each 
**	partitioned table in the query. They describe boolean factors 
**	that can be used to reduce the number of partitions to process.
**
**  History:    
**      15-jan-04 (inkdo01)
**	    Written using opbmbf/pmbf/jkeylist as guidelines.
**/


/*{
** Name: opb_pqbf	- find boolean factors for qualifying table
**		partitions
**
** Description:
**      This routine will build the OPB_PQBF structures for each
**	partitioned table in the query. There is one for each partition
**	dimension of a partitioned table (non-automatic) and they are
**	chained from the OPV_VARS structure for the corresponding table.
**	A OPB_KA structure addressed from each OPB_PQBF identifies the 
**	applicable boolean factors to each partitioning column.
**
** Inputs:
**      subquery                        ptr to subquery being analyzed
**
** Outputs:
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    All partitioned primary tables in the subquery range table
**	    will have OPB_PQBF structures built and attached to the
**	    OPV_VARS structures.
**
** History:
**	15-jan-04 (inkdo01)
**	    Written.
**	13-may-04 (inkdo01)
**	    Add support for between predicates.
**	28-Feb-2005 (schka24)
**	    Add restrictions because the partition qualification in OPC and
**	    QEF is still pretty limited.  a) all columns in a dim must
**	    be there;  b) no LIKE's or similar range-ish operators;
**	    c) no range operators if multiple columns in a dim, unless
**	    the oper is for the last column.
[@history_line@]...
*/
VOID
opb_pqbf(
	OPS_SUBQUERY       *subquery)
{
    OPV_IVARS		pvarno;		/* joinop range table number of base
                                        ** relation being analyzed */
    OPV_RT              *vbase;         /* ptr to base of array of ptrs to
                                        ** joinop range table elements */
    OPZ_AT		*abase;		/* base of joinop attributes array */
    OPB_BFT		*bfbase;	/* base of boolean factor array */
    OPE_ET		*ebase;		/* base of equivalence class array */
    OPV_VARS		*varp;		/* ptr to range var being analyzed */
    OPB_PQBF		*pqbfp;
    OPB_PQBF		**pqbfpp;
    OPB_KA		*kap;
    OPG_CB		*ops_server;
    DB_PART_DEF		*partp;
    PST_QNODE		*nodep;
    i4			i, j, ndims;
    i2			rule;
    bool		allcols, anycols;
    i4			keyi;		/* offset in opb_keyorder array of
                                        ** of attribute being analyzed
                                        */


    vbase = subquery->ops_vars.opv_base;
    abase = subquery->ops_attrs.opz_base; 
    bfbase = subquery->ops_bfs.opb_base;
    ebase = subquery->ops_eclass.ope_base;
    ops_server = subquery->ops_global->ops_cb->ops_server;

    for ( pvarno = 0; pvarno < subquery->ops_vars.opv_prv; pvarno++)
    {
	/* Loop over base tables looking for those that are partitioned. */
	varp = vbase->opv_rt[pvarno];
	if (varp->opv_grv->opv_relation)
	    partp = varp->opv_grv->opv_relation->rdr_parts;
	else continue;
	if (partp == (DB_PART_DEF *) NULL || (ndims = partp->ndims) == 0)
	    continue;			/* skip if not partitionined */

	for (i = 0, pqbfpp = &varp->opv_pqbf; i < ndims; i++)
	{
	    /* Loop over dimensions of table partitioning scheme. */
	    DB_PART_DIM	*dimp = &partp->dimension[i];

	    if ((rule = dimp->distrule) == DBDS_RULE_AUTOMATIC)
		continue;		/* skip the "automatic" ones */

	    /* Allocate memory for the OPB_PQBF/OPB_KA structures and 
	    ** hook them together with the OPV_VARS. */
	    *pqbfpp = pqbfp = (OPB_PQBF *)opu_memory(subquery->ops_global,
			sizeof(OPB_PQBF));
	    pqbfpp = &pqbfp->opb_pqbfnext;  /* next link word */
	    pqbfp->opb_pqbfnext = (OPB_PQBF *) NULL;
	    pqbfp->opb_pdim = i;
	    pqbfp->opb_pqcount = 1;
	    pqbfp->opb_pccount = dimp->ncols;
	    pqbfp->opb_pbfcount = 0;
	    pqbfp->opb_pqopno = ops_server->opg_eq; /* assume all "=" */

	    kap = (OPB_KA *)opu_memory(subquery->ops_global, sizeof(OPB_KA));
	    pqbfp->opb_pcbase = kap;

	    /* Fill in the partitioning column array in opb_pcbase. */
	    allcols = TRUE;
	    anycols = FALSE;
	    for (j = 0; j < dimp->ncols; j++)
	    {
		kap->opb_keyorder[j].opb_attno = opz_findatt(subquery, 
			pvarno, dimp->part_list[j].att_number);
		if (kap->opb_keyorder[j].opb_attno != OPZ_NOATTR)
		    anycols = TRUE;	/* at least one p col in query */
		else allcols = FALSE;
		kap->opb_keyorder[j].opb_gbf = OPB_NOBF;
	    }

	    /* Eventually we want to allow partial qualification for
	    ** LIST and RANGE.  However, at present, the runtime does
	    ** not support it.  (There are no whichpart functions that
	    ** say "give me the first and last partition break table
	    ** entries that match this partial partition value.")
	    ** So, for now, we insist upon allcols.
	    ** Note that some of the code in the OPB_KA building loop
	    ** below is prepared to deal with partial qualification.
	    */
	    if ( !allcols )
	    {
		/* Must be at least 1 column (or all columns for hash)
		** to be worth pursuing further. */
		pqbfp->opb_pbfcount = 0;
		pqbfp->opb_pcbase = NULL;
		continue;		/* nothing more for this dimension */
	    }

	    /* Fill in OPB_KA structure with bf information. This code
	    ** does essentially the same as opb_mbf, but for partitioning
	    ** columns. */
	    for (j = 0; j < dimp->ncols; j++)
	    {
		OPB_ATBFLST		*coldesc;
		OPZ_ATTS		*ap;	
		OPE_IEQCLS		eqcls;	/* eqc of part. col. */
		i4			range = -1;
		i2			opno, other_opno;

		if ((coldesc = &kap->opb_keyorder[j])->opb_attno == OPZ_NOATTR)
		    continue;

		ap = abase->opz_attnums[coldesc->opb_attno];
		if ( (eqcls = ap->opz_equcls) >= 0) 
		{
		    /* if equivalence class exists then check if a list 
		    ** of limiting predicates is associated with the eqc. */
		    OPB_IBF            bfi; /* bf no which contains
                                        ** OPB_BFGLOBAL information */

		    bfi = ebase->ope_eqclist[eqcls]->ope_nbf; 
					/* get boolean factor which contains 
					** summary of information for eqc */
                    if (bfi != OPB_NOBF)  /* check if any boolean factors
                                        ** exist for this equivalence class */
		    {
			OPB_BOOLFACT    *bfp;  /* ptr to bf which contains
                                        ** OPB_BFGLOBAL inf for eqc */
			OPB_BFKEYINFO   *bfinfop; /* ptr to bfkeyinfo for this
					** joinop attribute (type,length) */

			bfp = bfbase->opb_boolfact[bfi];

			bfinfop = opb_bfkget( subquery, bfp, eqcls, 
			    &ap->opz_dataval, &ap->opz_histogram.oph_dataval, 
				TRUE);
					/* get the keyinfo ptr
                                        ** for the type and length
                                        ** of the attribute, which
                                        ** contains the OPB_BFGLOBAL
                                        ** information for the
                                        ** attribute */

			/* Now copy the most useful BF to the OPB_KA structure
			** (just as opb_bestbf does in opbmbf.c). */
			if (bfinfop->opb_global->opb_single != OPB_NOBF)
			    coldesc->opb_gbf = bfinfop->opb_global->opb_single;
			else if (bfinfop->opb_global->opb_multiple != OPB_NOBF)
			{
			    coldesc->opb_gbf = bfinfop->opb_global->opb_multiple;
			    if ((nodep = bfbase->opb_boolfact[bfi]->opb_qnode)
				->pst_sym.pst_type == PST_BOP &&
				(nodep = nodep->pst_right)->pst_sym.pst_type
					== PST_CONST &&
				nodep->pst_left && pqbfp->opb_pqcount == 1)
			     for (nodep = nodep->pst_left; nodep;
					nodep = nodep->pst_left)
				pqbfp->opb_pqcount++;
					/* count entries in 1st IN-list */
			}
			else if (bfinfop->opb_global->opb_range != OPB_NOBF &&
			    (nodep = bfbase->opb_boolfact[bfinfop->
				opb_global->opb_range]->opb_qnode)->
				pst_sym.pst_type != PST_OR)
			{
			    opno = nodep->pst_sym.pst_value.pst_s_op.pst_opno;
			    coldesc->opb_gbf = bfinfop->opb_global->opb_range;
			    if (range < 0)
			    {
				range = j;
				pqbfp->opb_pqopno = opno;
					/* copy range comp operator */
			    }
			    if (dimp->ncols == 1 && 
				bfi != bfinfop->opb_global->opb_range)
			    {
				/* Small heuristic that detects between
				** restriction on partitioning column. */
				nodep = bfbase->opb_boolfact[bfi]->opb_qnode;
				if (nodep != NULL &&
				    nodep->pst_sym.pst_type == PST_BOP)
				{
				    other_opno = nodep->
					pst_sym.pst_value.pst_s_op.pst_opno;
				    if ((other_opno == ops_server->opg_lt
				      || other_opno == ops_server->opg_le)
				      && (opno == ops_server->opg_ge
					|| opno == ops_server->opg_gt) )
				    {
					coldesc->opb_gbf = 
					    bfinfop->opb_global->opb_range;
					kap->opb_keyorder[1].opb_attno =
					    coldesc->opb_attno;
					kap->opb_keyorder[1].opb_gbf = bfi;
					pqbfp->opb_pqopno = OPB_BETWEEN_OP;
					j++;
				    }
				    else if ((other_opno == ops_server->opg_gt
				      || other_opno == ops_server->opg_ge)
				      && (opno == ops_server->opg_lt
					|| opno == ops_server->opg_le) )
				    {
					coldesc->opb_gbf = bfi;
					kap->opb_keyorder[1].opb_attno =
					    coldesc->opb_attno;
					kap->opb_keyorder[1].opb_gbf = 
					    bfinfop->opb_global->opb_range;
					pqbfp->opb_pqopno = OPB_BETWEEN_OP;
					j++;
				    }
				}
			    }	/* end of "between" heuristic */
			}
		    }
		}

		if (range >= 0)
		{
		    /* Yet another runtime limitation here:  at present
		    ** it doesn't handle LIKE.  Anything that causes a
		    ** range that isn't < <= > >= or BETWEEN, we reject
		    ** qualifying this dimension.
		    */
		    opno = pqbfp->opb_pqopno;
		    if (opno != ops_server->opg_lt
		      && opno != ops_server->opg_le
		      && opno != ops_server->opg_gt
		      && opno != ops_server->opg_ge
		      && opno != OPB_BETWEEN_OP)
		    {
			pqbfp->opb_pbfcount = pqbfp->opb_pccount = 0;
			break;
		    }
		}

		if (range >= 0 || coldesc->opb_gbf == OPB_NOBF)
		{
		    if (rule == DBDS_RULE_HASH)
			pqbfp->opb_pbfcount = pqbfp->opb_pccount = 0;
					/* hash must have "=" for all cols */
		    else if (rule == DBDS_RULE_RANGE &&  range >= 0)
			pqbfp->opb_pbfcount = j+1;
					/* range stops at first non-"=" */
		    /* Stop when we see a range, or have no usable BF.
		    ** Ideally for LIST we could keep going, but not yet.
		    ** If the non-"=" isn't the last column, we'll not
		    ** try to use this dimension.
		    */
		    break;
		}
		if (coldesc->opb_gbf != OPB_NOBF)
		    pqbfp->opb_pbfcount++;

	    }	/* end of columns in dimension loop */

	}   /* end of partition dimensions loop */

	/* For each variable, drop dimensions that can't be used. */
	/* For now, added test that we must have usable BF's for all
	** partitioning columns.  (See above.)
	*/
	for (pqbfpp = &varp->opv_pqbf; (pqbfp = (*pqbfpp)) != NULL; )
	    if (pqbfp->opb_pbfcount == 0
	      || pqbfp->opb_pbfcount < pqbfp->opb_pccount)
		(*pqbfpp) = pqbfp->opb_pqbfnext;	/* drop this one */
	    else
		pqbfpp = &pqbfp->opb_pqbfnext;	/* else on to next */

    }	/* end of primary variables loop */

}
