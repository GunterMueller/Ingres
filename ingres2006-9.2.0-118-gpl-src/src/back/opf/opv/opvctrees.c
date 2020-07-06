/*
**Copyright (c) 2004 Ingres Corporation
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
#define        OPV_CTREES      TRUE
#include    <opxlint.h>

/**
**
**  Name: OPV_CTREES.C - compare two query trees for equality
**
**  Description:
**      Compare two query trees for equality
**
**  History:
**      28-jun-86 (seputis)    
**          initial creation
**	17-apr-89 (jrb)
**	    Properly initialized prec field(s).
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**      16-sep-93 (smc)
**          Moved <cs.h> for CS_SID.
**      24-Jun-99 (hanal04)
**          b97219. Adjust the logic in opv_ctress() to address the
**          nullability introduced by outer-joins.
[@history_line@]...
**/

/*{
** Name: opv_cvar	- compare 2 variables with same table ID
**
** Description:
**      Make a comparison of 2 variables with the same table ID
**	by ensuring that all references to that table ID are used 
**      consistently in other comparisions.  One tree is considered
**	to be the "master" and the other the slave, in that the master's
**	table ID's will not change but eventually the slave's table ID's
**	will change.
**
** Inputs:
**      node1                           root of the master query subtree to be compared
**      node2                           root of the slave query subtree to be compared
**      global                          ptr to global state variable
**                                      for error reporting
**
** Outputs:
**
**	Returns:
**	    TRUE - is var nodes are logically equal
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      9-nov-90 (seputis)
**          initial creation
**	27-mar-92 (seputis)
**	    -b43339 compare IDs only, do not translate when looking at
**	    target list
[@history_template@]...
*/
static bool
opv_cvar(
	OPS_STATE          *global,
	PST_QNODE          *node1,
	PST_QNODE          *node2)
{
    OPV_GRV	    *grvp;
    bool	    ret_val;
    OPV_GRT	    *gbase;

    gbase = global->ops_rangetab.opv_base;
    grvp = gbase->opv_grv[node2->pst_sym.pst_value.pst_s_var.pst_vno];
    if ((grvp->opv_compare == OPV_NOGVAR)
	&&
	!(global->ops_gmask & OPS_CIDONLY))
    {	/* new mapping found for slave range variable */
	OPV_GRV	    **grvpp;

	/* check that no other variable has this mapping */
	for (grvpp = &gbase->opv_grv[0]; grvpp == &gbase->opv_grv[OPV_MAXVAR]; grvpp++)
	    if ((*grvpp)
		&&
		((*grvpp)->opv_compare == node1->pst_sym.pst_value.pst_s_var.pst_vno)
		)
		return(FALSE);	/* 2 table ID's cannot map to the same table */

	grvp->opv_compare = node1->pst_sym.pst_value.pst_s_var.pst_vno;
	ret_val = TRUE;
    }
    else 
	ret_val = (node1->pst_sym.pst_value.pst_s_var.pst_vno == grvp->opv_compare);
    return(ret_val);
}

/*{
** Name: opv_ctrees	- compare two subtrees for equality
**
** Description:
**      Check if two subtrees are identical (with respect to aggregate
**      processing).  The query tree comparison is a structural comparison
**      so that order is important if two aggregates are to be run together.
**      Comparison of constants within the query tree is done using ADF
**      and care is made to ensure that "pattern-matching" equality is
**      ignored.
**
**      The 4.0 version of this routine did MEcmp, but this will not
**      work on machines which align components of structures.  Some machines
**      may not require aligned components but compilers will align anyways
**      in order to improve performance.  This routine
**      compares the components directly, and is more complicated, but more
**      accurate.  This other alternative is to ensure that every node is
**      zeroed prior to being used in the query tree.  This is not done.
**
** Inputs:
**      node1                           root of the query subtree to be compared
**      node2                           root of the query subtree to be compared
**      global                          ptr to global state variable
**                                      for error reporting
**
** Outputs:
**	Returns:
**          TRUE - if subtrees are equal
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	10-apr-86 (seputis)
**          initial creation from sameafcn in aggregate.c
**	30-apr-90 (seputis)
**	    fix b21461, did not recognize subselect node
**	23-may-91 (seputis)
**	    compare constants of different types correctly
**      24-Jun-99 (hanal04)
**          b97219. When we check to see whether two nodes are of the same
**          DB_DATA_VALUE we must account for nullability introduced by
**          an outer-join as follows;
**             i) ABSOLUTE types should be compared.
**            ii) ADJUSTED lengths should be compared. Adjustments as follows;
**                  N1 non-nullable, N2 non-nullable => no adjustment
**                  N1 nullable, N2 non-nullable => add 1 to N2's length
**                  N1 non-nullable, N2 nullable => add -1 to N2's length
**                  N1 nullable, N2 nullable => no adjustment
**    22-oct-99 (inkdo01)
**        Support for new case expression node types.
**	11-mar-02 (inkdo01)
**	    Support for sequence operator node type.
**	5-dec-03 (inkdo01)
**	    Add support for PST_MOP, PST_OPERAND (SUBSTRING) nodes.
**	6-apr-06 (dougi)
**	    Add support for PST_GSET/GBCR/GCL nodes (for GROUP BY support).
[@history_line@]...
*/
bool
opv_ctrees(
	OPS_STATE          *global,
	PST_QNODE          *node1,
	PST_QNODE          *node2)
{
    i4  length_adjust = 0;

    if (!(node1 && node2))
	return(!(node1 || node2));	    /* TRUE if both are NULL */

    if (node1->pst_sym.pst_type != node2->pst_sym.pst_type)
	return( FALSE );                    /* symbol types do not match */

    switch (node1->pst_sym.pst_type)
    {

    case PST_ROOT:		    /* bitmaps may not be up to date */
    case PST_AGHEAD:		    /* bitmaps may not be up to date */
    case PST_SUBSEL:		    /* bitmaps may not be up to date */
    case PST_BYHEAD:
    case PST_OPERAND:
    case PST_CASEOP:
    case PST_WHLIST:
    case PST_WHOP:
    case PST_AND:
    case PST_OR:
    case PST_GSET:
    case PST_GCL:
    {
	return( opv_ctrees( global, node1->pst_left, node2->pst_left )
		&&
		opv_ctrees( global, node1->pst_right, node2->pst_right )
	      );
    }
    case PST_NOT:
	return (opv_ctrees( global, node1->pst_left, node2->pst_left ));
	    /*FIXME - is left or right branch used for NOT */
    case PST_SEQOP:
    case PST_QLEND:
    case PST_TREE:
	return( TRUE );

    case PST_GBCR:
	return(node1->pst_sym.pst_value.pst_s_group.pst_groupmask ==
	    node2->pst_sym.pst_value.pst_s_group.pst_groupmask);

    case PST_UOP:
    case PST_BOP:
    case PST_AOP:
    case PST_COP:
    case PST_MOP:
    case PST_CONST:
    case PST_VAR:
    case PST_RESDOM:
    {
        /* b97219 - Assess the length adjustment if any */
        if((node1->pst_sym.pst_dataval.db_datatype < 0) &&
           (node2->pst_sym.pst_dataval.db_datatype > 0))
            length_adjust = 1;
        if((node1->pst_sym.pst_dataval.db_datatype > 0) &&
            (node2->pst_sym.pst_dataval.db_datatype < 0))
           length_adjust = -1;

	/* check that the DB_DATA_VALUE matches */
        /* b97219 - Compare absolute data types and adjusted lengths */
	if (    
		(abs(node1->pst_sym.pst_dataval.db_datatype) 
		!= 
		abs(node2->pst_sym.pst_dataval.db_datatype))
	    ||
		(node1->pst_sym.pst_dataval.db_prec
		!= 
		node2->pst_sym.pst_dataval.db_prec)
	    ||
		(node1->pst_sym.pst_dataval.db_length
		!= 
		(node2->pst_sym.pst_dataval.db_length+length_adjust))
	    )
	    return (FALSE);
	break;
    }
    case PST_SORT:
    {
	return (    (node1->pst_sym.pst_value.pst_s_sort.pst_srvno
		    ==
		    node2->pst_sym.pst_value.pst_s_sort.pst_srvno)
		&&
		    (node1->pst_sym.pst_value.pst_s_sort.pst_srasc
		    ==
		    node2->pst_sym.pst_value.pst_s_sort.pst_srasc)
		&&
		opv_ctrees( global, node1->pst_left, node2->pst_left )
		&&
		opv_ctrees( global, node1->pst_right, node2->pst_right )
	        );
    }
    case PST_CURVAL:
# ifdef  E_OP0682_UNEXPECTED_NODE
	opx_error( E_OP0682_UNEXPECTED_NODE);
# endif
    default:
	{
# ifdef E_OP0681_UNKNOWN_NODE
	    opx_error( E_OP0681_UNKNOWN_NODE);
# endif
	}
    }

    switch (node1->pst_sym.pst_type)
    {
    case PST_RESDOM:
    {
	return (    (node1->pst_sym.pst_value.pst_s_rsdm.pst_rsno
		    ==
		    node2->pst_sym.pst_value.pst_s_rsdm.pst_rsno)
		&&
		opv_ctrees( global, node1->pst_left, node2->pst_left )
		&&
		opv_ctrees( global, node1->pst_right, node2->pst_right )
	        );
    }
    case PST_UOP:
    case PST_BOP:
    case PST_AOP:
    case PST_COP:
    case PST_MOP:
	return (    (node1->pst_sym.pst_value.pst_s_op.pst_opno
		    ==
		    node2->pst_sym.pst_value.pst_s_op.pst_opno)
		&&
		    (node1->pst_sym.pst_value.pst_s_op.pst_opinst
		    ==
		    node2->pst_sym.pst_value.pst_s_op.pst_opinst)
		&&
		opv_ctrees( global, node1->pst_left, node2->pst_left )
		&&
		opv_ctrees( global, node1->pst_right, node2->pst_right )
	        );

    case PST_CONST:
	{
	    OPS_PNUM		parm1;
	    OPS_PNUM		parm2;
	    parm1 = node1->pst_sym.pst_value.pst_s_cnst.pst_parm_no;
	    parm2 = node2->pst_sym.pst_value.pst_s_cnst.pst_parm_no;
	    if (parm1 != parm2)
	    {
		if ((parm1 <= global->ops_qheader->pst_numparm)
		    ||
		    (parm2 <= global->ops_qheader->pst_numparm)
		    ||
		    (parm1 > global->ops_gdist.opd_user_parameters)
		    ||
		    (parm2 > global->ops_gdist.opd_user_parameters)
		    ||
		    (   node1->pst_sym.pst_value.pst_s_cnst.pst_pmspec
			!=
			node2->pst_sym.pst_value.pst_s_cnst.pst_pmspec
		    )
		    )
		    return(FALSE);	    /* check for non-repeat parameters
					    ** and non-simple aggregate constants
					    ** i.e. user parameters which are
					    ** constant for all invocations, these
					    ** should be treated as equal
					    ** also pattern matching should match */
	    }
	    else if (parm1)
		return (TRUE);		    /* parameter constants with the same
                                            ** parameter number are equal */
		 
	    /* only compare non-parameter constants for equality */
	    return(
		    !opu_dcompare( global, &node1->pst_sym.pst_dataval,
			    &node2->pst_sym.pst_dataval) /* opu_dcompare will
                                            ** return 0 if datavalues are
                                            ** equal */
		);
	}
    case PST_VAR:
	{
	    if (node1->pst_sym.pst_value.pst_s_var.pst_atno.db_att_id
		!=
		node2->pst_sym.pst_value.pst_s_var.pst_atno.db_att_id)
		return(FALSE);		    /* if attributes are not equal
					    ** then var nodes cannot be equal
					    ** even if table IDs are not 
					    ** equal */
	    if (node1->pst_sym.pst_value.pst_s_var.pst_vno
		==
		node2->pst_sym.pst_value.pst_s_var.pst_vno)
		return(TRUE);
	    else if ((global->ops_gmask & OPS_IDSAME) 
		&&
		(global->ops_gmask & OPS_IDCOMPARE)
		)
	    {
		/* check that variable mapping can be used to make 2 range variables with
		** different table ID's equal */
		if ((	global->ops_rangetab.opv_base->opv_grv
			    [node1->pst_sym.pst_value.pst_s_var.pst_vno]->opv_same
			!=
			OPV_NOGVAR
		    )
		    &&
		    (	global->ops_rangetab.opv_base->opv_grv
			    [node1->pst_sym.pst_value.pst_s_var.pst_vno]->opv_same
			==
			global->ops_rangetab.opv_base->opv_grv
			    [node2->pst_sym.pst_value.pst_s_var.pst_vno]->opv_same
		    )
		    )
		    return(opv_cvar(global,node1,node2)); /* since the table ID's
					    ** are the same check if a valid mapping
					    ** can be found */
	    }
	    return(FALSE);
	}
    default:
	{
#ifdef    E_OP0681_UNKNOWN_NODE
	    opx_error( E_OP0681_UNKNOWN_NODE);
#endif
	}
    }
    return (FALSE);
}
