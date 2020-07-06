/* 
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <st.h>
#include    <tr.h>
#include    <qu.h>
#include    <bt.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <adf.h>
#include    <adfops.h>
#include    <dbdbms.h>
#include    <ddb.h>
#include    <dmf.h>
#include    <dmtcb.h>
#include    <ulf.h>
#include    <qsf.h>
#include    <scf.h>
#include    <qefrcb.h>
#include    <rdf.h>
#include    <usererror.h>
#include    <psfparse.h>
#include    <psfindep.h>
#include    <pshparse.h>
#include    <psftrmwh.h>

/**
**
**  Name: PSTRSLV.C - Functions used to resolve a query tree.
**
**  Description:
**      This file contains the functions necessary to resolve a query tree
**	operator node.  This means setting the conversions and the result
**	types and lengths for the node, as well as figuring out which ADF
**	function should be called.
**
**          pst_resolve - Resolve a query tree operator node
**	    pst_union_resolve - Resolve unions
**	    pst_caserslt_resolve - Resolve result expressions of case funcs
**	    pst_casesrce_resolve - Resolve source/comparand exprs of case funcs
**	    pst_parm_resolve - Resolve local variables/parms
**	    pst_convlit - Convert literals to compatible type if possible
**
**
**  History:
**      03-oct-85 (jeff)    
**          written
**      13-sep_86 (seputis)
**          added initialization of fidesc ptr
**	4-mar-87 (daved)
**	    added resolution for union nodes
**	23-oct-87 (stec)
**	    General cleanup and fixes to pst_union_resolve.
**	1-feb-88 (stec)
**	    Gene changed data type precedence hierarchy.
**	18-feb-88 (thurston)
**	    Replaced the ty_better() static routine and its uses in pst_opcomp()
**	    and pst_uncomp() with the ty_rank() static routine.  This solves a
**	    bug, and is simpler code.
**	08-jun-88 (stec)
**	    Added pst_parm_resolve procedure.
**	03-nov-88 (stec)
**	    Correct bug found by lint.
**	18-jun-89 (jrb)
**	    Major changes.  Algorithm for doing resolution is now in ADF.  This
**	    routine no longer takes any knowledge about what a datatype looks
**	    like internally nor does it make any assumptions about how a
**	    datatype is resolved.  Removed pst_opcomp, pst_uncomp, and ty_rank
**	    functions.  This file is about half of its original size.
**	12-jul-89 (jrb)
**	    Made E_AD2061_BAD_OP_COUNT map to a user error rather than letting
**	    PSF treat it as an internal error.
**	15-dec-89 (jrb)
**	    Added checks for DB_ALL_TYPE in function instance descriptors (for
**	    outer join project).
**	24-oct-90 (andre)
**	    remove declarations for ty_rank(), pst_opcomp(), pst_uncomp() which
**	    have been previously removed.  Apparently, some compilers do not dig
**	    static declarations for non-existent functions.
**	14-jan-92 (barbara)
**	    Included ddb.h for Star.
**	18-feb-93 (walt)
**		Removed (the now incorrect) prototypes for adi_fitab, adi_tycoerce,
**		and adi_tyname now that ADF is prototyped in <adf.h>.
**	23-mar-93 (smc)
**	    Cast parameter of BTtest() to match prototype.
**	29-jun-93 (andre)
**	    #included TR.H
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	14-mar-94 (andre)
**	    extracted code from pst_union_resolve() into
**	    pst_get_union_resdom_type()
**      1-feb-95 (inkdo01)
**          special case code to allow long byte/varchar union compatibility
**	6-feb-95 (hanch04)
**	    fixed syntax error caused by comment
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      07-Jan-2003 (hanch04)
**          Back out last change buy zhahu02.  Not compatible with new code.
**	17-jul-2003 (toumi01)
**	    Add to PSF some flattening decisions.
**	31-aug-2004 (sheco02)
**	    X-integrate change 466442 to main.
**	31-aug-2004 (sheco02)
**	    Fixed previous cross change mistake.
**	02-Sep-2004 (bonro01)
**	    Fixed x-integration of change 466442
**/


GLOBALREF PSF_SERVBLK     *Psf_srvblk;  /* PSF server control block ptr */

static DB_STATUS
pst_caserslt_resolve(
	PSS_SESBLK  *sess_cb,
	PST_QNODE   *casep,
	DB_ERROR    *error);

static DB_STATUS
pst_casesrce_resolve(
	PSS_SESBLK  *sess_cb,
	PST_QNODE   *casep,
	DB_ERROR    *error);

static DB_STATUS
pst_get_union_resdom_type(
			  DB_DT_ID	dt,
			  DB_DATA_VALUE *resval,
			  ADF_CB	*adf_scb,
			  bool		nullable,
			  DB_ERROR	*error);

/*{
** Name: pst_resolve	- Resolve a query tree operator node
**
** Description:
**      The pst_resolve function resolves a query tree operator node.  This
**	means that it sets result type and length for the node.  It also
**	figures out which ADF function should be called for the node.  This
**	operation should be called whenever a new operator node	is constructed. 
**	It will not be necessary to re-resolve an operator node should it get
**	new children; however, one should be sure that the function that the
**	operator represents can take the new children as arguments (it might
**	be impossible to convert the children to the desired types before
**	performing the function).  Any query tree that comes from the parser
**	is guaranteed to have already been resolved.
**
**	This function will work on the following node types.  They MUST have the
**	children required for the particular node type:
**
**	    Node Type	    Left Child	    Right Child
**
**	    PST_UOP	    Yes		    No
**	    PST_BOP	    Yes		    Yes
**	    PST_AOP	    Yes		    No
**	    PST_COP	    No		    No
**	    PST_MOP         Yes             No
**
**	NOTE: This function does not set the conversion ids for an operator
**	node.  That it because the operator node could receive new children
**	of different types.  The conversions must be set before the expression
**	is used.
**
**	NOTE: Some operators (cast, for example) set the result type by their
**	very definition in the parser. If the result type determined here is
**	the same as the datatype in the operator node, it is assumed that the 
**	parser already set the data type for the operator and it is retained.
**
** Inputs:
**	sess_cb				session control block. MAY BE NULL.
**	adf_scb				adf session control block
**      opnode				The operator node to resolve
**	lang				query language
**      error                           Error block
**
** Outputs:
**      opnode				The resolved operator node
**	error				Standard error block
**	    .err_code			    What error occurred
**		E_PS0000_OK			Success
**		E_PS0002_INTERNAL_ERROR		Internal inconsistency in PSF
**		2908L				No function found by this name
**						that can take these arguments.
**		2910L				Ambiguous function or operator
**		E_PS0C02_NULL_PTR		opnode = NULL
**		E_PS0C03_BAD_NODE_TYPE		opnode is not an operator node
**		E_PS0C04_BAD_TREE		Something is wrong with the
**						query tree.
**	Returns:
**	    E_DB_OK			Function completed normally.
**	    E_DB_WARN			Function completed with warning(s)
**	    E_DB_ERROR			Function failed; non-catastrophic error
**	    E_DB_FATAL			Function failed; catastrophic error
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	03-oct-85 (jeff)
**          written
**      13-sep-86 (seputis)
**          added ptr to ADI_FI_DESC to node to be resolved
**	14-may-87 (stec)
**	    added code to handle COUNT(*) case in SQL (no children).
**	09-jul-87 (stec)
**	    fixed bug resulting in computation of bad result length.
**	12-aug-87 (thurston)
**	    fixed bug that didn't calculate the result length properly for
**	    aggregates that have the nullability of the result different than
**	    that of the input, AND did not need to coerce the input.
**	18-jun-89 (jrb)
**	    Changed this routine to use ADF to do datatype resolution.  Also,
**	    fixed a bug where the function instance id's for the coercions in an
**	    operator node where being set incorrectly (they're not currently
**	    being used anyway).
**	12-jul-89 (jrb)
**	    Made E_AD2061_BAD_OP_COUNT map to a user error rather than letting
**	    PSF treat it as an internal error.
**	18-jul-89 (jrb)
**	    Set precision field for all db_data_values contained in here.
**	25-jul-89 (jrb)
**	    Made sure db_data field was filled in for db_data_value's used in
**	    calclen calls.
**	15-dec-89 (jrb)
**	    Added checks for DB_ALL_TYPE in function instance descriptors.
**	10-aug-93 (Andre)
**	    removed declaration of scf_call()
**	09-Mar-1999 (shero03)
**	    support MOP nodes.
**	7-sep-99 (inkdo01)
**	    Added calls to pst_caserslt_resolve/pst_casesrce_resolve for case 
**	    function resolution.
**	4-july-01 (inkdo01)
**	    Fix error message for wrong number of function parms.
**      09-dec-2005 (huazh01)
**          if the aggregate is under a PST_BOP node, do not 
**          flatten such query too. 
**          b115593, INGSRV3530.
**	16-jul-03 (hayke02)
**	    Call adi_resolve() with varchar_precedence TRUE if either ps202
**	    (PSS_VARCHAR_PRECEDENCE) is set or psf_vch_prec is ON. This change
**	    fixes bug 109132.
**	22-dec-04 (inkdo01)
**	    Add logic to resolve binop's involving explicit collations.
**	2-feb-06 (dougi)
**	    Make cast functions in select list work.
**	8-mar-06 (dougi)
**	    Slight adjustment to avoid problems from ii_iftrue().
**      17-apr-2006 (huazh01)
**          some nodes in a dynamic SQL query might not be resolved in
**          pst_node(), which will leave the node's dbtype as 0. If
**          the dbtype of a child node does not seem to be valid,
**          try to resolve such child node before we continue our work
**          with the invalid dbtype.
**          bug 115963.
**	25-Apr-2006 (kschendel)
**	    Fix to not check args to varargs fns that are in the optional
**	    part of the param list (>= fi's numargs).
**	    Fix old bug in MOP checking, followed wrong link.
**	05-sep-2006 (gupsh01)
**	    Added call to adi_dtfamily_resolve(), if function instance 
**	    selected is generic to the datatype family of the operands.
**	25-oct-2006 (dougi)
**	    Check for null sess_cb before dereferencing it.
*/
DB_STATUS
pst_resolve(
	PSS_SESBLK	    *sess_cb,
	ADF_CB		    *adf_scb,
	register PST_QNODE  *opnode,
	DB_LANG 	    lang,
	DB_ERROR	    *error)
{

    i4			children;
    i4			i;
    DB_STATUS		status;
    DB_DATA_VALUE	ops[ADI_MAX_OPERANDS];
    DB_DATA_VALUE	*pop[ADI_MAX_OPERANDS];
    DB_DATA_VALUE	tempop, tempop1;
    ADI_FI_DESC		*fi_desc;
    ADI_FI_DESC		*best_fidesc;
    ADI_FI_DESC		*new_fidesc = NULL;
    ADI_RSLV_BLK	adi_rslv_blk;
    PST_QNODE  		*lopnode;
    DB_DATA_VALUE	*opptrs[ADI_MAX_OPERANDS];
    PST_QNODE	        *lqnode;
    PST_QNODE           *resqnode;
    i1		        leftvisited = FALSE;
    i4		val1;
    i4		val2;
#ifdef    xDEBUG
    SCF_CB		scf_cb;
    SCF_SCI		sci_list[1];
    PTR			scf_scb;
#endif

    /* Make sure there is a node to resolve */
    if (opnode == (PST_QNODE *) NULL)
    {
	error->err_code = E_PS0C02_NULL_PTR;
	return (E_DB_SEVERE);
    }

    /* If it's a "case" function, call the special case resolution functions. */
    if (opnode->pst_sym.pst_type == PST_CASEOP)
    {
	/* First call result expression resolution routine. */
	status = pst_caserslt_resolve(sess_cb, opnode, error);
	/* if (status != E_DB_OK || opnode->pst_right == NULL) */ return(status);
	
	/* If we get here, it's a "simple" case with source expressions to 
	** resolve, too. */
	/* return(pst_casesrce_resolve(sess_cb, opnode, error)); */
    }

    /* Make sure it's an operator node */
    if (opnode->pst_sym.pst_type != PST_UOP &&
        opnode->pst_sym.pst_type != PST_BOP &&
        opnode->pst_sym.pst_type != PST_COP &&
	opnode->pst_sym.pst_type != PST_AOP &&
	opnode->pst_sym.pst_type != PST_MOP)
    {
	error->err_code = E_PS0C03_BAD_NODE_TYPE;
	return (E_DB_SEVERE);
    }

    for (i = 0; i < ADI_MAX_OPERANDS; i++)
    {	
    	pop[i] = (DB_DATA_VALUE *)NULL;
	adi_rslv_blk.adi_dt[i] = DB_NODT;
    }

    /* Figure out the number of children */
    if ((opnode->pst_sym.pst_type == PST_COP) ||
	((opnode->pst_sym.pst_type == PST_AOP) &&
	 (opnode->pst_sym.pst_value.pst_s_op.pst_opno == ADI_CNTAL_OP))
       )
	children = 0;
    else if (opnode->pst_sym.pst_type == PST_BOP)
	children = 2;
    else
	children = 1;

    if (opnode->pst_sym.pst_type == PST_AOP &&
	opnode->pst_right != (PST_QNODE *) NULL) children = 2;

    if (opnode->pst_sym.pst_type == PST_MOP)
    {
	lopnode = opnode;
	children = 0;
	while (lopnode)
	{	
	   if (lopnode->pst_right)     
	   {	
              adi_rslv_blk.adi_dt[children] =
	    	    lopnode->pst_right->pst_sym.pst_dataval.db_datatype;
    	      children++;
	   }
	   lopnode = lopnode->pst_left;
	}
    }		
    else if (children >= 1)
    {
	if (opnode->pst_left == (PST_QNODE *) NULL)
	{
             error->err_code = E_PS0C04_BAD_TREE;
	     return (E_DB_SEVERE);
	}
        adi_rslv_blk.adi_dt[0] = 
		opnode->pst_left->pst_sym.pst_dataval.db_datatype;

        /* b115963 */
        if (adi_rslv_blk.adi_dt[0] == 0) 
        {
           status = pst_resolve(sess_cb, adf_scb, opnode->pst_left, lang, error);

           if (status != E_DB_OK) return (E_DB_SEVERE);

           adi_rslv_blk.adi_dt[0] =
                opnode->pst_left->pst_sym.pst_dataval.db_datatype;
        }

	if (children == 2)
	{
	    if (opnode->pst_right == (PST_QNODE *) NULL)
	    {
        	error->err_code = E_PS0C04_BAD_TREE;
		return (E_DB_SEVERE);
	    }
            adi_rslv_blk.adi_dt[1] = 
		opnode->pst_right->pst_sym.pst_dataval.db_datatype;

            /* b115963 */
            if (adi_rslv_blk.adi_dt[1] == 0)
            {
               status = pst_resolve(sess_cb, adf_scb, opnode->pst_right,
                                      lang, error);

               if (status != E_DB_OK) return (E_DB_SEVERE);

               adi_rslv_blk.adi_dt[1] =
                   opnode->pst_right->pst_sym.pst_dataval.db_datatype;
            }

	}
    }

#ifdef	xDEBUG

    /* Don't do tracing if couldn't get session control block */
    if (sess_cb != (PSS_SESBLK *) NULL)
    {
	if (ult_check_macro(&sess_cb->pss_trace, 21, &val1, &val2))
	{
	    TRdisplay("Resolving operator with %d children\n\n", children);
	}
    }
#endif

    /* set up to call ADF to resolve this node */
    adi_rslv_blk.adi_op_id   = opnode->pst_sym.pst_value.pst_s_op.pst_opno;
    adi_rslv_blk.adi_num_dts = children;
    STRUCT_ASSIGN_MACRO(opnode->pst_sym.pst_dataval, tempop1);

    if ((Psf_srvblk->psf_vch_prec == TRUE) ||
	(sess_cb && ult_check_macro(&sess_cb->pss_trace,
	PSS_VARCHAR_PRECEDENCE, &val1, &val2)))
	status = adi_resolve(adf_scb, &adi_rslv_blk, TRUE);
    else
	status = adi_resolve(adf_scb, &adi_rslv_blk, FALSE);

    /*
    ** for flattening we care if we have one or multiple ifnulls with
    ** aggregates, so use two flags as a two-bit counter indicating that
    ** we have none, one, or more than one
    */
    if ( sess_cb && adi_rslv_blk.adi_op_id == ADI_IFNUL_OP )
    {
	if ( (opnode->pst_left->pst_sym.pst_type == PST_AGHEAD )
             ||
             /* b115593, INGSRV3530 
             ** do not flatten the query if the PST_AGHEAD is under
             ** a PST_BOP node.
             */
             (
              (opnode->pst_left->pst_sym.pst_type == PST_BOP) &&
              (
                 (opnode->pst_left->pst_left->pst_sym.pst_type ==
                   PST_AGHEAD) 
                 ||
                 (opnode->pst_left->pst_right->pst_sym.pst_type ==
                   PST_AGHEAD)
              )
             )
           )
	{
	    if (sess_cb->pss_flattening_flags & PSS_IFNULL_AGHEAD)
                sess_cb->pss_flattening_flags |= PSS_IFNULL_AGHEAD_MULTI;
	    sess_cb->pss_flattening_flags |= PSS_IFNULL_AGHEAD;
	}
    }

    if (status != E_DB_OK)
    {
	/* Complain if no applicable function found */
	if (	adf_scb->adf_errcb.ad_errcode == E_AD2062_NO_FUNC_FOUND)
	{
	    if (children == 1)
		error->err_code = 2907L;
	    else
		error->err_code = 2908L;
	    return (E_DB_ERROR);
	}

	/* Complain if parameter count is wrong. */
	if (  	adf_scb->adf_errcb.ad_errcode == E_AD2061_BAD_OP_COUNT)
	{
	    error->err_code = 2903L;
	    return(E_DB_ERROR);
	}

	/* Complain if ambiguous function found */
	if (adf_scb->adf_errcb.ad_errcode == E_AD2063_FUNC_AMBIGUOUS)
	{
	    if (children == 1)
		error->err_code = 2909L;
	    else
		error->err_code = 2910L;
	    return (E_DB_ERROR);
	}

	error->err_code = E_PS0C05_BAD_ADF_STATUS;
	return (E_DB_ERROR);
    }

    /* we now have the "best" fi descriptor */
    best_fidesc = adi_rslv_blk.adi_fidesc;
    
    if (best_fidesc)
    {
	i2	need_fmly_resolve = 0;

	status = adi_need_dtfamily_resolve( adf_scb, best_fidesc, 
					    &adi_rslv_blk, 
					    &need_fmly_resolve);
	if (status)
	{
    	  error->err_code = E_PS0C05_BAD_ADF_STATUS;
	  return (E_DB_SEVERE);
	}

	if (sess_cb && need_fmly_resolve == 1) 
	{
	    status = psf_malloc(sess_cb, &sess_cb->pss_ostream,
		sizeof(ADI_FI_DESC), &new_fidesc, error);
            if (status != E_DB_OK)
            {
              return (status);
            }
 	    if  (new_fidesc == NULL)
	    {
    	      error->err_code = E_PS0C05_BAD_ADF_STATUS;
	      return (E_DB_SEVERE);
	    }

	    status = adi_dtfamily_resolve(adf_scb, best_fidesc, 
					  new_fidesc, &adi_rslv_blk);
	    if (status != E_DB_OK)
	    {
		return (status);
	    }

	    best_fidesc = new_fidesc; 
	}
    }

    status = adi_res_type(adf_scb, best_fidesc,
    		adi_rslv_blk.adi_num_dts, adi_rslv_blk.adi_dt,
		&opnode->pst_sym.pst_dataval.db_datatype);
		
    if (status != E_DB_OK)
    {
    	error->err_code = E_PS0C05_BAD_ADF_STATUS;
	return (E_DB_SEVERE);
    }
    opnode->pst_sym.pst_value.pst_s_op.pst_opinst = best_fidesc->adi_finstid;
    opnode->pst_sym.pst_value.pst_s_op.pst_fdesc = best_fidesc;
    if (opnode->pst_sym.pst_dataval.db_datatype < 0 &&
	tempop1.db_datatype > 0)
    {
	tempop1.db_length++;
	tempop1.db_datatype = -tempop1.db_datatype;
    }

    /*
    ** Figure out the result length.  First we must get the lengths of the
    ** operands after conversion, then we must use these lengths to figure
    ** out the length of the result.
    */

    i = 0;
    if (opnode->pst_sym.pst_type == PST_MOP)
    {
	/* MOP operands are chained thru the left link, but the first
	** operand is the mop's right link.
	*/
    	resqnode = opnode;
	lqnode = resqnode->pst_right;
    }
    else
    {
    	lqnode = opnode->pst_left;
	if (lqnode == NULL)
	{
	   lqnode = opnode->pst_right;
	   leftvisited = TRUE;
	}
    }
    
    while (lqnode)
    {		   					       
       /* Assume no coercion for now */
       STRUCT_ASSIGN_MACRO(lqnode->pst_sym.pst_dataval, tempop);
       STRUCT_ASSIGN_MACRO(tempop, ops[i]);

       /* Get coercion id for this child, if needed */
       if (i < best_fidesc->adi_numargs
	  && best_fidesc->adi_dt[i] != 
	    abs(lqnode->pst_sym.pst_dataval.db_datatype)
	  && best_fidesc->adi_dt[i] != DB_ALL_TYPE)
       {
          status = adi_ficoerce(adf_scb, 
	   	lqnode->pst_sym.pst_dataval.db_datatype,
		best_fidesc->adi_dt[i],
		&opnode->pst_sym.pst_value.pst_s_op.pst_oplcnvrtid);
	  if (status != E_DB_OK)
	  {
	     error->err_code = E_PS0C05_BAD_ADF_STATUS;
	     return (E_DB_SEVERE);
	  }

	  /* Look up coercion in function instance table */
	  status = adi_fidesc(adf_scb,
		opnode->pst_sym.pst_value.pst_s_op.pst_oplcnvrtid,
		&fi_desc);
	  if (status != E_DB_OK)
	  {
	     error->err_code = E_PS0C05_BAD_ADF_STATUS;
	     return (E_DB_SEVERE);
	  }

	  /* Get length after coercion */
	  opptrs[0] = &ops[i];
	  status = adi_0calclen(adf_scb, &fi_desc->adi_lenspec, 1,
				opptrs, &tempop);
	  if (status != E_DB_OK 
		&& adf_scb->adf_errcb.ad_errcode != E_AD2022_UNKNOWN_LEN)
	  {
		error->err_code = E_PS0C05_BAD_ADF_STATUS;
		return (E_DB_SEVERE);
	  }

          STRUCT_ASSIGN_MACRO(tempop, ops[i]);
       }
       pop[i] = &ops[i];

       i++;
       if (opnode->pst_sym.pst_type == PST_MOP)
       {
	    /* Chain on the left, actual args on the right */
	    resqnode = resqnode->pst_left;
	    if (resqnode)
		lqnode = resqnode->pst_right;
	    else
		break;
       }		
       else
       {
       	  if (leftvisited)
	     break;
	  leftvisited = TRUE;
	  lqnode = opnode->pst_right;    	    
       }
    }

    /* Now figure out the result length of the operator being resolved */
    status = adi_0calclen(adf_scb, &best_fidesc->adi_lenspec, children, pop,
			&opnode->pst_sym.pst_dataval);
	
    /* Determine result db_collID. Chooses left operand collation ID
    ** unless right collID >= 0 and left was -1 (not sure about this -
    ** it should actually be an error if they are both explicitly 
    ** declared, >0 and not equal). */
    if (tempop1.db_datatype == best_fidesc->adi_dtresult ||
	tempop1.db_datatype == -best_fidesc->adi_dtresult)
	STRUCT_ASSIGN_MACRO(tempop1, opnode->pst_sym.pst_dataval);
    else if (opnode->pst_left)
    {
	opnode->pst_sym.pst_dataval.db_collID = 
			opnode->pst_left->pst_sym.pst_dataval.db_collID;
	if (opnode->pst_right)
	{
	    if (opnode->pst_right->pst_sym.pst_dataval.db_collID >= 0 &&
		opnode->pst_sym.pst_dataval.db_collID == -1)
		opnode->pst_sym.pst_dataval.db_collID =
			opnode->pst_right->pst_sym.pst_dataval.db_collID;
	}
    }

    if (status != E_DB_OK	
	&& adf_scb->adf_errcb.ad_errcode != E_AD2022_UNKNOWN_LEN)
    {
	error->err_code = E_PS0C05_BAD_ADF_STATUS;
	return (E_DB_SEVERE);
    }

    return    (E_DB_OK);
}


/*{
** Name: pst_union_resolve	- Resolve a query tree union
**
** Description:
**      The pst_union_resolve function resolves a query tree union.  This
**	means that it sets result type and length for the nodes.  It also
**	figures out which ADF function should be called for the nodes.  This
**	operation should be called whenever a union is added to the select. 
**	Any query tree that comes from the parser is guaranteed to have already
**	been resolved.
**
**	NOTE: This function does not set the conversion ids for an operator
**	node.  That it because the operator node could receive new children
**	of different types.  The conversions must be set before the expression
**	is used.
**
** Inputs:
**	sess_cb				session control block
**      rootnode			The root node pointing to the union
**      error                           Error block
**
** Outputs:
**      opnode				The resolved operator node
**	error				Standard error block
**	    .err_code			    What error occurred
**		E_PS0000_OK			Success
**		E_PS0002_INTERNAL_ERROR		Internal inconsistency in PSF
**		E_PS0C02_NULL_PTR		rootnode = NULL or rootnode
**						-->pst_union == NULL
**		E_PS0C03_BAD_NODE_TYPE		rootnode is not a root node
**		E_PS0C04_BAD_TREE		Something is wrong with the
**						query tree.
**		2917				Unequal number of resdoms
**						in union.
**		2918				resdom types not compatible
**	Returns:
**	    E_DB_OK			Function completed normally.
**	    E_DB_WARN			Function completed with warning(s)
**	    E_DB_ERROR			Function failed; non-catastrophic error
**	    E_DB_FATAL			Function failed; catastrophic error
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	4/3/87 (daved)
**	    written
**	23-oct-87 (stec)
**	    Cleanup, do resolution for n subselects, not just two.
**	18-jun-89 (jrb)
**	    This routine now calls ADF to do datatype resolution instead of
**	    taking knowledge of how it should be done.
**	12-jul-89 (jrb)
**	    Made E_AD2061_BAD_OP_COUNT map to a user error rather than letting
**	    PSF treat it as an internal error.
**	01-aug-89 (jrb)
**	    Set precision field for all db_data_values contained in here.  Also,
**	    added rules for doing UNIONs on decimal resdoms.
**	18-aug-89 (jrb)
**	    Fixed problem where error printing code was AVing because it was
**	    using the wrong fi pointer to get the dt name from ADF.
**	30-oct-89 (jrb)
**	    Can't initialize structs on the stack on Unix although it works on
**	    VMS.  Fixed this.
**	15-dec-89 (jrb)
**	    Added checks for DB_ALL_TYPE in function instance descriptors.
**	14-mar-94 (andre)
**	    moved code propagating type information of RESDOM nodes in the first
**	    element of UNION into RESDOM nodes of the subsequent elements
**	15-apr-05 (inkdo01)
**	    Add collation support.
**      09-sep-05 (zhahu02)
**          Updated to be compatible with long nvarchar (INGSRV3411/b115179).
*/
DB_STATUS
pst_union_resolve(
	PSS_SESBLK  *sess_cb,
	PST_QNODE   *rootnode,
	DB_ERROR    *error)
{
    ADF_CB		*adf_scb = (ADF_CB *)sess_cb->pss_adfcb;
    DB_STATUS		status;
    register PST_QNODE	*res1;
    register PST_QNODE	*res2;
    register PST_QNODE	*ss;
    ADI_RSLV_BLK	adi_rslv_blk;
    ADI_FI_DESC		*best_fidesc;
    DB_DT_ID		temp_dt;
    i4		err_code;
    ADI_DT_NAME		leftparm;
    ADI_DT_NAME		rightparm;
    bool		nullable;
#ifdef    xDEBUG
    i4		val1;
    i4		val2;
#endif

    /* Make sure there is a node to resolve */
    if (rootnode == (PST_QNODE *) NULL || 
	rootnode->pst_sym.pst_value.pst_s_root.pst_union.pst_next
						     == (PST_QNODE*) NULL)
    {
	error->err_code = E_PS0C02_NULL_PTR;
	return (E_DB_SEVERE);
    }

    /* Make sure it's a root node */
    if (rootnode->pst_sym.pst_type != PST_ROOT)
    {
	error->err_code = E_PS0C03_BAD_NODE_TYPE;
	return (E_DB_SEVERE);
    }

    /*
    ** In general case, a query expression may have n subselects. We need to
    ** look at data types of the resdoms across subselects (i.e., resdom no. =
    ** const), and determine if they can be coerced into a common datatype, and
    ** if there is more than one, choose the best one. Rules for choosing the
    ** best one are same as in case of other data type coercions. Upon return
    ** the target list of the first subselect shall contain the appropriate
    ** datatype info.  The algorithm is to process two target lists at a time.
    ** In the first run it will be done for the target list of the first and
    ** second subselects, then first and nth subselect, where n = 3, 4, ...
    */

    /* traverse each subselect */
    for (ss = rootnode->pst_sym.pst_value.pst_s_root.pst_union.pst_next; ss;
	 ss = ss->pst_sym.pst_value.pst_s_root.pst_union.pst_next
	)
    {
	/* walk down list of resdoms for 1st and nth subselect */
	for (res1 = rootnode->pst_left, res2 = ss->pst_left;
	     res1 || res2;
	     res1 = res1->pst_left, res2 = res2->pst_left
	    )
	{
	    /* if not both resdoms or tree ends, then error */
	    if (!res1 || !res2 || 
		(res1->pst_sym.pst_type != res2->pst_sym.pst_type)
	       )
	    {
		(VOID) psf_error(2917L, 0L, PSF_USERERR, &err_code, error, 1,
		    sizeof (sess_cb->pss_lineno), &sess_cb->pss_lineno);
		return (E_DB_ERROR);
	    }

	    if (res1->pst_sym.pst_type == PST_TREE)
		break;

	    /* Make sure it's a resdom node */
	    if (res1->pst_sym.pst_type != PST_RESDOM)
	    {
		error->err_code = E_PS0C03_BAD_NODE_TYPE;
		return (E_DB_SEVERE);
	    }

	    /* Coalesce the collation IDs (if any). If nth is > 0,
	    ** copy to 1st. Later, 1st will be propagated to the rest.
	    ** NOTE: 2 collations > 0 but different should probably be 
	    ** an error, but not just yet since they're probably unicode
	    ** and unicode_case_insensitive which we tolerate. */
	    if (res2->pst_sym.pst_dataval.db_collID > 0 &&
		res1->pst_sym.pst_dataval.db_collID != 2)
		res1->pst_sym.pst_dataval.db_collID = 
		    res2->pst_sym.pst_dataval.db_collID;

	    /* ask ADF to find a common datatype for the current resdoms */
	    adi_rslv_blk.adi_op_id   = ADI_FIND_COMMON;
	    adi_rslv_blk.adi_num_dts = 2;
	    adi_rslv_blk.adi_dt[0]     = res1->pst_sym.pst_dataval.db_datatype;
	    adi_rslv_blk.adi_dt[1]     = res2->pst_sym.pst_dataval.db_datatype;

	    status = adi_resolve(adf_scb, &adi_rslv_blk, FALSE);

	    if (status != E_DB_OK)
	    {
		/* Complain if no applicable function found */
		if (	adf_scb->adf_errcb.ad_errcode == E_AD2062_NO_FUNC_FOUND
		    ||	adf_scb->adf_errcb.ad_errcode == E_AD2061_BAD_OP_COUNT
		    ||	adf_scb->adf_errcb.ad_errcode == E_AD2009_NOCOERCION)  
		{
		    status = adi_tyname(adf_scb,
					    adi_rslv_blk.adi_dt[0],
					    &leftparm);
		    if (status != E_DB_OK)
		    {
			if (adf_scb->adf_errcb.ad_errclass == ADF_USER_ERROR)
			{
			    (VOID) psf_error(E_PS0C04_BAD_TREE,
				adf_scb->adf_errcb.ad_usererr, PSF_INTERR,
				&err_code, error, 0);
			}
			else
			{
			    (VOID) psf_error(E_PS0C04_BAD_TREE,
				adf_scb->adf_errcb.ad_errcode, PSF_INTERR,
				&err_code, error, 0);
			}
			return (E_DB_SEVERE);
		    }
		    status = adi_tyname(adf_scb,
					    adi_rslv_blk.adi_dt[1],
					    &rightparm);
		    if (status != E_DB_OK)
		    {
			if (adf_scb->adf_errcb.ad_errclass == ADF_USER_ERROR)
			{
			    (VOID) psf_error(E_PS0C04_BAD_TREE,
				adf_scb->adf_errcb.ad_usererr, PSF_INTERR,
				&err_code, error, 0);
			}
			else
			{
			    (VOID) psf_error(E_PS0C04_BAD_TREE,
				adf_scb->adf_errcb.ad_errcode, PSF_INTERR,
				&err_code, error, 0);
			}
			return (E_DB_SEVERE);
		    }
		    (VOID) psf_error(2918L, 0L, PSF_USERERR, 
			&err_code, error, 3, 
			sizeof (sess_cb->pss_lineno), &sess_cb->pss_lineno,
		        psf_trmwhite(sizeof (ADI_DT_NAME), (char *) &leftparm), 
			&leftparm, 
		        psf_trmwhite(sizeof (ADI_DT_NAME), (char *) &rightparm),
			&rightparm);

		    return (E_DB_ERROR);
		}

		/* Complain if ambiguous function found */
		if (adf_scb->adf_errcb.ad_errcode == E_AD2063_FUNC_AMBIGUOUS)
		{
		    status = adi_tyname(adf_scb,
					    adi_rslv_blk.adi_dt[0],
					    &leftparm);
		    if (status != E_DB_OK)
		    {
			if (adf_scb->adf_errcb.ad_errclass == ADF_USER_ERROR)
			{
			    (VOID) psf_error(E_PS0C04_BAD_TREE,
				adf_scb->adf_errcb.ad_usererr, PSF_INTERR,
				&err_code, error, 0);
			}
			else
			{
			    (VOID) psf_error(E_PS0C04_BAD_TREE,
				adf_scb->adf_errcb.ad_errcode, PSF_INTERR,
				&err_code, error, 0);
			}
			return (E_DB_SEVERE);
		    }
		    status = adi_tyname(adf_scb,
					    adi_rslv_blk.adi_dt[1],
					    &rightparm);
		    if (status != E_DB_OK)
		    {
			if (adf_scb->adf_errcb.ad_errclass == ADF_USER_ERROR)
			{
			    (VOID) psf_error(E_PS0C04_BAD_TREE,
				adf_scb->adf_errcb.ad_usererr, PSF_INTERR,
				&err_code, error, 0);
			}
			else
			{
			    (VOID) psf_error(E_PS0C04_BAD_TREE,
				adf_scb->adf_errcb.ad_errcode, PSF_INTERR,
				&err_code, error, 0);
			}
			return (E_DB_SEVERE);
		    }
		    (VOID) psf_error(2919L, 0L, PSF_USERERR, 
			&err_code, error, 3, 
			sizeof (sess_cb->pss_lineno), &sess_cb->pss_lineno,
		        psf_trmwhite(sizeof (ADI_DT_NAME), (char *) &leftparm), 
			&leftparm, 
		        psf_trmwhite(sizeof (ADI_DT_NAME), (char *) &rightparm),
			&rightparm);

		    return (E_DB_ERROR);
		}

		error->err_code = E_PS0C05_BAD_ADF_STATUS;
		return (E_DB_ERROR);
	    }

	    /* we now have the "best" fi descriptor */
	    best_fidesc = adi_rslv_blk.adi_fidesc;

	    /*
	    ** In some cases coercion per se may not be needed, but
	    ** data type may have to be changed to nullable.
	    */

	    status = adi_res_type(adf_scb, best_fidesc,
    			adi_rslv_blk.adi_num_dts, adi_rslv_blk.adi_dt,
			&temp_dt);
			
	    nullable = (bool) (temp_dt < 0);

	    /* update type info of left node, if needed */
	    status = pst_get_union_resdom_type(best_fidesc->adi_dt[0],
		&res1->pst_sym.pst_dataval, adf_scb, nullable, error);
	    if (DB_FAILURE_MACRO(status))
	        return(status);
	    
            /* if either type is long byte/varchar, must short circuit the
            ** resolution process here. Since long's have no "=" ops,
            ** adi_resolve (way back there) produced the actual coercion 
            ** fi. Since there's only one operand for coercions, the
            ** rest of this stuff needs to be bypassed */
 
            if (best_fidesc->adi_dt[0] != DB_LVCH_TYPE && 
                  best_fidesc->adi_dt[0] != DB_LBYTE_TYPE &&
                best_fidesc->adi_dt[0] != DB_LNVCHR_TYPE)
            {                             /* not "long" - keep going */
                
	        /* update type info of right node, if needed */
	        status = pst_get_union_resdom_type(best_fidesc->adi_dt[1],
		    &res2->pst_sym.pst_dataval, adf_scb, nullable, error);
	        if (DB_FAILURE_MACRO(status))
	            return(status);

	       /*
	        ** For decimal datatype we use the ADI_DECBLEND lenspec to get the
	        ** result precision, scale, and length; for other types, just
	        ** take the maximum length.  The resulting precision/scale and
	        ** length will be placed in res1 which will end up with the values
	        ** that everything else will need to be coerced into.
	        */
	        if (abs(res1->pst_sym.pst_dataval.db_datatype) == DB_DEC_TYPE)
	        {
		    DB_DATA_VALUE	    *dv[2];
		    DB_DATA_VALUE	    tmp;
		    ADI_LENSPEC	    lenspec;

		    tmp.db_datatype = DB_DEC_TYPE;
		    tmp.db_data = NULL;

		    lenspec.adi_lncompute	= ADI_DECBLEND;
		    lenspec.adi_fixedsize	= 0;
		    lenspec.adi_psfixed	= 0;
		    dv[0] = &res1->pst_sym.pst_dataval;
		    dv[1] = &res2->pst_sym.pst_dataval;
		
		    status = adi_0calclen(adf_scb, &lenspec, 2, dv, &tmp);
		    if (status != 0)
		    {
		        error->err_code = E_PS0C05_BAD_ADF_STATUS;
		        return (E_DB_SEVERE);
		    }

		    dv[0]->db_prec = tmp.db_prec;
		    dv[0]->db_length = tmp.db_length;

		    if (nullable)
		        dv[0]->db_length++;
	        }
	        else
	        {
		    if (res1->pst_sym.pst_dataval.db_length <
		        res2->pst_sym.pst_dataval.db_length)
		    {
		        res1->pst_sym.pst_dataval.db_length =
			    res2->pst_sym.pst_dataval.db_length;
		    }
	        }
	        
	    } /* for each resdom in both analyzed target lists */
        } /* for each subselect in union loop */
    }   /* end of ! "long" types */

    /* Propagate the resdom data type to the subselect being added. */
    for (ss = rootnode->pst_sym.pst_value.pst_s_root.pst_union.pst_next;
         ss;
	 ss = ss->pst_sym.pst_value.pst_s_root.pst_union.pst_next)
    {
	for (res1 = rootnode->pst_left, res2 = ss->pst_left; 
	    res1->pst_sym.pst_type == PST_RESDOM;
	    res1 = res1->pst_left, res2 = res2->pst_left)
	{
	    res2->pst_sym.pst_dataval.db_datatype 
		= res1->pst_sym.pst_dataval.db_datatype;
	    res2->pst_sym.pst_dataval.db_prec
		= res1->pst_sym.pst_dataval.db_prec;
	    res2->pst_sym.pst_dataval.db_length
		= res1->pst_sym.pst_dataval.db_length;
	    res2->pst_sym.pst_dataval.db_collID
		= res1->pst_sym.pst_dataval.db_collID;
	}
    }

    return    (E_DB_OK);
}

/*{
** Name: pst_caserslt_resolve	- Resolve the set of result expressions
**				in a case function
**
** Description:
**      The pst_caserslt_resolve function resolves the result expressions of
**	a case function. That is, it determines the common data type for
**	all result expressions and sets it into their respective subtrees
**	(and the case node itself).
**
** Inputs:
**	sess_cb				session control block
**      casep				The case function root node
**      error                           Error block
**
** Outputs:
**      opnode				The resolved operator node
**	error				Standard error block
**	    .err_code			    What error occurred
**		E_PS0000_OK			Success
**		E_PS0002_INTERNAL_ERROR		Internal inconsistency in PSF
**		E_PS0C02_NULL_PTR		rootnode = NULL or rootnode
**						-->pst_union == NULL
**		E_PS0C03_BAD_NODE_TYPE		rootnode is not a root node
**		E_PS0C04_BAD_TREE		Something is wrong with the
**						query tree.
**		2933				Result expressions not coercible
**	Returns:
**	    E_DB_OK			Function completed normally.
**	    E_DB_WARN			Function completed with warning(s)
**	    E_DB_ERROR			Function failed; non-catastrophic error
**	    E_DB_FATAL			Function failed; catastrophic error
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	3-sep-99 (inkdo01)
**	    Cloned from pst_union_resolve for case function implementation.
**      23-May-2002 (zhahu02)
**          Modified with the decimal type for case function implementation
**          (b107866/INGSRV1786).
**	07-Jan-2003 (hanch04)
**	    Back out last change.  Not compatible with new code.
**	8-nov-2006 (dougi)
**	    Assign result type to "then" results, too, to assure correct
**	    coercions.
**	13-nov-2006 (dougi)
**	    Oops - that one broke more than it fixed - now removed.
**	22-Mar-07 (wanfr01)
**	    Bug 117979
**	    Need third parameter for adi_resolve
**	3-apr-2007 (dougi)
**	    Permit NULL result from THEN or ELSE.
*/
static DB_STATUS
pst_caserslt_resolve(
	PSS_SESBLK  *sess_cb,
	PST_QNODE   *casep,
	DB_ERROR    *error)
{
    ADF_CB		*adf_scb = (ADF_CB *)sess_cb->pss_adfcb;
    DB_STATUS		status;
    register PST_QNODE	*res1;
    register PST_QNODE	*res2;
    PST_QNODE		*whlistp, *whop1p, *whop2p;
    ADI_RSLV_BLK	adi_rslv_blk;
    ADI_FI_DESC		*best_fidesc;
    DB_DT_ID		temp_dt;
    i4			err_code;
    ADI_DT_NAME		leftparm;
    ADI_DT_NAME		rightparm;
    bool		nullable;

    /* Make sure there is a node to resolve */
    if (casep == (PST_QNODE *) NULL ||
	(whlistp = casep->pst_left) == (PST_QNODE *)NULL ||
	(whop1p = whlistp->pst_right) == (PST_QNODE *)NULL)
    {
	error->err_code = E_PS0C02_NULL_PTR;
	return (E_DB_SEVERE);
    }

    /* Make sure it's a case function. */
    if (casep->pst_sym.pst_type != PST_CASEOP ||
	whlistp->pst_sym.pst_type != PST_WHLIST ||
	whop1p->pst_sym.pst_type != PST_WHOP)
    {
	error->err_code = E_PS0C03_BAD_NODE_TYPE;
	return (E_DB_SEVERE);
    }

    /* Start by copying first result expression type to "case" node. */
    res1 = whop1p->pst_right;
    casep->pst_sym.pst_dataval.db_datatype = 
	    res1->pst_sym.pst_dataval.db_datatype;
    casep->pst_sym.pst_dataval.db_prec =
	    res1->pst_sym.pst_dataval.db_prec;
    casep->pst_sym.pst_dataval.db_length =
	    res1->pst_sym.pst_dataval.db_length;

    /* Check for stupid "case" with one "when" and no "else". */
    if (whlistp->pst_left == (PST_QNODE *)NULL)
    {
	return(E_DB_OK);
    }

    /*
    ** The algorithm is simply to resolve the first result expression
    ** (copied to the case node) in turn with each remaining result
    ** expression. This will produce the final result type in the case node.
    */

    {
	/* Walk down when-list to process remaining result exprs. */
	for (res1 = casep, whlistp = whlistp->pst_left;
	    whlistp && whlistp->pst_sym.pst_type == PST_WHLIST &&
	    (whop2p = whlistp->pst_right) && (res2 = whop2p->pst_right);
	    whlistp = whlistp->pst_left) 
	{
	    if (res2->pst_sym.pst_dataval.db_datatype == -DB_LTXT_TYPE)
	    {
		nullable = TRUE;
		continue;		/* skip NULL */
	    }

	    /* ask ADF to find a common datatype for the current result exprs. */
	    adi_rslv_blk.adi_op_id   = ADI_FIND_COMMON;
	    adi_rslv_blk.adi_num_dts = 2;
	    adi_rslv_blk.adi_dt[0]   = res1->pst_sym.pst_dataval.db_datatype;
	    adi_rslv_blk.adi_dt[1]   = res2->pst_sym.pst_dataval.db_datatype;

	    status = adi_resolve(adf_scb, &adi_rslv_blk, FALSE);

	    if (status != E_DB_OK)
	    {
		/* Complain if essentially non-coercible. */
		if (	adf_scb->adf_errcb.ad_errcode == E_AD2062_NO_FUNC_FOUND
		    ||	adf_scb->adf_errcb.ad_errcode == E_AD2061_BAD_OP_COUNT
		    ||	adf_scb->adf_errcb.ad_errcode == E_AD2009_NOCOERCION
		    ||  adf_scb->adf_errcb.ad_errcode == E_AD2063_FUNC_AMBIGUOUS)
		{
		    (VOID) psf_error(2933L, 0L, PSF_USERERR, 
			&err_code, error, 2, 
			sizeof (sess_cb->pss_lineno), &sess_cb->pss_lineno,
		        (i4)6, "result");
		    return (E_DB_ERROR);
		}

		error->err_code = E_PS0C05_BAD_ADF_STATUS;
		return (E_DB_ERROR);
	    }

	    /* we now have the "best" fi descriptor */
	    best_fidesc = adi_rslv_blk.adi_fidesc;

	    /*
	    ** In some cases coercion per se may not be needed, but
	    ** data type may have to be changed to nullable.
	    */

	    status = adi_res_type(adf_scb, best_fidesc,
			adi_rslv_blk.adi_num_dts, adi_rslv_blk.adi_dt,
			&temp_dt);
			
	    /* Nullable flag set from resolved type, or if this is the
	    ** last "when" of an "else-less" case function. */
	    nullable = (bool) (temp_dt < 0 ||
		(whlistp->pst_left == (PST_QNODE *)NULL &&
		 whop2p->pst_left != (PST_QNODE *)NULL));

	    /* update type info of first result node, if needed */
	    status = pst_get_union_resdom_type(best_fidesc->adi_dt[0],
		&res1->pst_sym.pst_dataval, adf_scb, nullable, error);
	    if (DB_FAILURE_MACRO(status))
	        return(status);
	    
            /* if either type is long byte/varchar, must short circuit the
            ** resolution process here. Since long's have no "=" ops,
            ** adi_resolve (way back there) produced the actual coercion 
            ** fi. Since there's only one operand for coercions, the
            ** rest of this stuff needs to be bypassed */
 
            if (best_fidesc->adi_dt[0] != DB_LVCH_TYPE && 
                  best_fidesc->adi_dt[0] != DB_LBYTE_TYPE)
            {                             /* not "long" - keep going */
		DB_DATA_VALUE	resval2;
                
	        /* update type info of current result node, if needed */
		STRUCT_ASSIGN_MACRO(res2->pst_sym.pst_dataval, resval2);
	        status = pst_get_union_resdom_type(best_fidesc->adi_dt[1],
		    &resval2, adf_scb, nullable, error);
	        if (DB_FAILURE_MACRO(status))
	            return(status);

	       /*
	        ** For decimal datatype we use the ADI_DECBLEND lenspec to get the
	        ** result precision, scale, and length; for other types, just
	        ** take the maximum length.  The resulting precision/scale and
	        ** length will be placed in res1 which will end up with the values
	        ** that everything else will need to be coerced into.
	        */
	        if (abs(res1->pst_sym.pst_dataval.db_datatype) == DB_DEC_TYPE)
	        {
		    DB_DATA_VALUE	    *dv[2];
		    DB_DATA_VALUE	    tmp;
		    ADI_LENSPEC	    lenspec;

		    dv[0] = &res1->pst_sym.pst_dataval;
		    dv[1] = &resval2;

		    tmp.db_datatype = DB_DEC_TYPE;
		    tmp.db_data = NULL;

		    lenspec.adi_lncompute	= ADI_DECBLEND;
		    lenspec.adi_fixedsize	= 0;
		    lenspec.adi_psfixed	= 0;
		
		    status = adi_0calclen(adf_scb, &lenspec, 2, dv, &tmp);

		    if (status != 0)
		    {
		        error->err_code = E_PS0C05_BAD_ADF_STATUS;
		        return (E_DB_SEVERE);
		    }

		    dv[0]->db_prec = tmp.db_prec;
		    dv[0]->db_length = tmp.db_length;

		    if (nullable)
		        dv[0]->db_length++;
	        }
	        else
	        {
		    if (res1->pst_sym.pst_dataval.db_length < resval2.db_length)
		    {
		        res1->pst_sym.pst_dataval.db_length = resval2.db_length;
		    }
	        }
	        
	    } /* end of ! "long" types */
        } /* for each "when" in case function */
    }

    /* It is possible that the only nullable result came from an empty
    ** ELSE. In this case, we still need to make the case nullable. */
    if (nullable && casep->pst_sym.pst_dataval.db_datatype > 0)
    {
	casep->pst_sym.pst_dataval.db_datatype = 
			-casep->pst_sym.pst_dataval.db_datatype;
	casep->pst_sym.pst_dataval.db_length++;
    }

    return    (E_DB_OK);
}

/*{
** Name: pst_casesrce_resolve	- Confirm the source expression is comparable
**				with all comparand expressions in a simple case.
**
** Description:
**      The pst_casesrce_resolve function checks the source and comparand 
**	expressions of the simple form of case function (CASE expr WHEN comp
**	expr THEN ...). That is, it assures the source is comparable to
**	each comparand expression.
**
** Inputs:
**	sess_cb				session control block
**      casep				The case function root node
**      error                           Error block
**
** Outputs:
**      opnode				The resolved operator node
**	error				Standard error block
**	    .err_code			    What error occurred
**		E_PS0000_OK			Success
**		E_PS0002_INTERNAL_ERROR		Internal inconsistency in PSF
**		E_PS0C02_NULL_PTR		rootnode = NULL or rootnode
**						-->pst_union == NULL
**		E_PS0C03_BAD_NODE_TYPE		rootnode is not a root node
**		E_PS0C04_BAD_TREE		Something is wrong with the
**						query tree.
**		2933				Source/comparand expressions not 
**						comparable.
**	Returns:
**	    E_DB_OK			Function completed normally.
**	    E_DB_WARN			Function completed with warning(s)
**	    E_DB_ERROR			Function failed; non-catastrophic error
**	    E_DB_FATAL			Function failed; catastrophic error
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	3-sep-99 (inkdo01)
**	    Cloned from pst_union_resolve for case function implementation.
**	10-feb-00 (inkdo01)
**	    Replaced pst_srceval with individual type, length, precision
**	    fields for 64-bit support.
**	22-Mar-07 (wanfr01)
**	    Bug 117979
**	    Need third parameter for adi_resolve
*/
static DB_STATUS
pst_casesrce_resolve(
	PSS_SESBLK  *sess_cb,
	PST_QNODE   *casep,
	DB_ERROR    *error)
{
    ADF_CB		*adf_scb = (ADF_CB *)sess_cb->pss_adfcb;
    DB_STATUS		status;
    register PST_QNODE	*srcep;
    register PST_QNODE	*compp;
    PST_QNODE		*whlistp, *whop1p, *whop2p;
    ADI_RSLV_BLK	adi_rslv_blk;
    ADI_FI_DESC		*best_fidesc;
    DB_DATA_VALUE	*srcevalp, srceval;
    DB_DT_ID		temp_dt;
    i4			err_code;
    i4			count;
    ADI_DT_NAME		leftparm;
    ADI_DT_NAME		rightparm;
    bool		nullable;

    /* Make sure there is a node to resolve */
    if (casep == (PST_QNODE *) NULL ||
	(srcep = casep->pst_right) == (PST_QNODE *)NULL ||
	(whlistp = casep->pst_left) == (PST_QNODE *)NULL ||
	(whop1p = whlistp->pst_right) == (PST_QNODE *)NULL)
    {
	error->err_code = E_PS0C02_NULL_PTR;
	return (E_DB_SEVERE);
    }

    /* Make sure it's a case function. */
    if (casep->pst_sym.pst_type != PST_CASEOP ||
	whlistp->pst_sym.pst_type != PST_WHLIST ||
	whop1p->pst_sym.pst_type != PST_WHOP)
    {
	error->err_code = E_PS0C03_BAD_NODE_TYPE;
	return (E_DB_SEVERE);
    }

    /*
    ** The algorithm is simply to resolve the source expression in turn 
    ** with the comparand expressions of each "when" clause. This will
    ** produce the final type for the source expression, which is then
    ** propagated down to all comparands.
    ** 
    ** But first, init the common source comparison type to that of the 
    ** source expression and remember its address.
    */

    srceval.db_data = (PTR) NULL;
    srceval.db_length = casep->pst_sym.pst_value.pst_s_case.pst_caselen;
    srceval.db_datatype = casep->pst_sym.pst_value.pst_s_case.pst_casedt;
    srceval.db_prec = casep->pst_sym.pst_value.pst_s_case.pst_caseprec;
    srcevalp = &srceval;

    {
	/* Walk down when-list to process each comparand expr. */
	for (count = 0; whlistp && whlistp->pst_sym.pst_type == PST_WHLIST &&
	    (whop2p = whlistp->pst_right) && (compp = whop2p->pst_left);
	    count++, whlistp = whlistp->pst_left) 
	{
	    /* ask ADF to find a common datatype for the current expression pair. */
	    adi_rslv_blk.adi_op_id   = ADI_FIND_COMMON;
	    adi_rslv_blk.adi_num_dts = 2;
	    adi_rslv_blk.adi_dt[0]   = srcevalp->db_datatype;
	    adi_rslv_blk.adi_dt[1]   = compp->pst_sym.pst_dataval.db_datatype;

	    status = adi_resolve(adf_scb, &adi_rslv_blk, FALSE);

	    if (status != E_DB_OK)
	    {
		/* Complain if essentially non-coercible. */
		if (	adf_scb->adf_errcb.ad_errcode == E_AD2062_NO_FUNC_FOUND
		    ||	adf_scb->adf_errcb.ad_errcode == E_AD2061_BAD_OP_COUNT
		    ||	adf_scb->adf_errcb.ad_errcode == E_AD2009_NOCOERCION
		    ||  adf_scb->adf_errcb.ad_errcode == E_AD2063_FUNC_AMBIGUOUS)
		{
		    (VOID) psf_error(2933L, 0L, PSF_USERERR, 
			&err_code, error, 2, 
			sizeof (sess_cb->pss_lineno), &sess_cb->pss_lineno,
		        (i4)16, "source/comparand");
		    return (E_DB_ERROR);
		}

		error->err_code = E_PS0C05_BAD_ADF_STATUS;
		return (E_DB_ERROR);
	    }

	    /* we now have the "best" fi descriptor */
	    best_fidesc = adi_rslv_blk.adi_fidesc;

	    /*
	    ** In some cases coercion per se may not be needed, but
	    ** data type may have to be changed to nullable.
	    */

	    status = adi_res_type(adf_scb, best_fidesc,
			srcevalp->db_datatype,
			compp->pst_sym.pst_dataval.db_datatype,
			&temp_dt);
			
	    nullable = (bool) (temp_dt < 0);

	    /* update type info of source node, if needed */
	    status = pst_get_union_resdom_type(best_fidesc->adi_dt[0],
		srcevalp, adf_scb, nullable, error);
	    if (DB_FAILURE_MACRO(status))
	        return(status);
	    
            /* if either type is long byte/varchar, must short circuit the
            ** resolution process here. Since long's have no "=" ops,
            ** adi_resolve (way back there) produced the actual coercion 
            ** fi. Since there's only one operand for coercions, the
            ** rest of this stuff needs to be bypassed */
 
            if (best_fidesc->adi_dt[0] != DB_LVCH_TYPE && 
                  best_fidesc->adi_dt[0] != DB_LBYTE_TYPE)
            {                             /* not "long" - keep going */
		DB_DATA_VALUE	compval;
                
	        /* update type info of comparand node, if needed */
		STRUCT_ASSIGN_MACRO(compp->pst_sym.pst_dataval, compval);
	        status = pst_get_union_resdom_type(best_fidesc->adi_dt[1],
		    &compval, adf_scb, nullable, error);
	        if (DB_FAILURE_MACRO(status))
	            return(status);

	       /*
	        ** For decimal datatype we use the ADI_DECBLEND lenspec to get the
	        ** result precision, scale, and length; for other types, just
	        ** take the maximum length.  The resulting precision/scale and
	        ** length will be placed in srcep which will end up with the values
	        ** that everything else will need to be coerced into.
	        */
	        if (abs(srcevalp->db_datatype) == DB_DEC_TYPE)
	        {
		    DB_DATA_VALUE	    *dv[2];
		    DB_DATA_VALUE	    tmp;
		    ADI_LENSPEC	    lenspec;

		    dv[0] = srcevalp;
		    dv[1] = &compval;

		    tmp.db_datatype = DB_DEC_TYPE;
		    tmp.db_data = NULL;

		    lenspec.adi_lncompute	= ADI_DECBLEND;
		    lenspec.adi_fixedsize	= 0;
		    lenspec.adi_psfixed	= 0;
		
		    status = adi_0calclen(adf_scb, &lenspec, 2, dv, &tmp);
		    if (status != 0)
		    {
		        error->err_code = E_PS0C05_BAD_ADF_STATUS;
		        return (E_DB_SEVERE);
		    }

		    dv[0]->db_prec = tmp.db_prec;
		    dv[0]->db_length = tmp.db_length;

		    if (nullable)
		        dv[0]->db_length++;
	        }
	        else
	        {
		    if (srcevalp->db_length < compval.db_length)
		    {
		        srcevalp->db_length = compval.db_length;
		    }
	        }
	        
	    } /* end of ! "long" types */
        } /* for each "when" in case function */
    }

    /* Re-assign resolved type info. */
    casep->pst_sym.pst_value.pst_s_case.pst_caselen = srceval.db_length;
    casep->pst_sym.pst_value.pst_s_case.pst_casedt = srceval.db_datatype;
    casep->pst_sym.pst_value.pst_s_case.pst_caseprec = srceval.db_prec;

    return    (E_DB_OK);
}

/*
** Name:    pst_get_union_resdom_type - determine type info of a resdom node
**
** Description:
**	Using result of adi_resolve() and adi_res_type(), determine type info of
**	a resdom node of a UNION select
**
** Input:
**	dt		result of resolving datatypes of left and right RESDOM
**			nodes
**	resval		ptr to DB_DATA_VALUE of RESDOM node whose type info needs
**			to be computed
**	adf_scb		ADF session CB
**	nullable	indicates whether the common datatype should be nullable
**
** Output:
**	error
**	    err_code	filled in if an error occurred
**
** Returns:
**	E_DB_{OK,STATUS}
**
** History:
**	14-mar-94 (andre)
**	    extracted from the body of pst_union_resolve()
**	8-sep-99 (inkdo01)
**	    Changed 2nd parm to ptr to pst_dataval from ptr to node (to make 
**	    case funcs a bit easier to code)
*/
static DB_STATUS
pst_get_union_resdom_type(
			  DB_DT_ID	dt,
			  DB_DATA_VALUE	*resval,
			  ADF_CB	*adf_scb,
			  bool		nullable,
			  DB_ERROR	*error)
{
    DB_STATUS	    status;
    ADI_FI_ID	    convid;
    ADI_FI_DESC	    *fi_desc;
    DB_DATA_VALUE   resop;
    DB_DATA_VALUE   *opptrs[ADI_MAX_OPERANDS];

    /* Get coercion id for the resdom, if needed */
    if (dt != abs(resval->db_datatype) && dt != DB_ALL_TYPE)
    {
	status = adi_ficoerce(adf_scb, resval->db_datatype,
	    dt, &convid);
	if (status != E_DB_OK)
	{
	    error->err_code = E_PS0C05_BAD_ADF_STATUS;
	    return(E_DB_SEVERE);
	}

	/* Look up coercion in function instance table */
	status = adi_fidesc(adf_scb, convid, &fi_desc);
	if (status != E_DB_OK)
	{
	    error->err_code = E_PS0C05_BAD_ADF_STATUS;
	    return (E_DB_SEVERE);
	}

	resop.db_datatype = (nullable) ? -dt : dt;

	opptrs[0] = resval;
	/* Get length after coercion */
	status = adi_0calclen(adf_scb, &fi_desc->adi_lenspec, 1,
	    opptrs, &resop);
	if (   status != E_DB_OK
	    && adf_scb->adf_errcb.ad_errcode != E_AD2022_UNKNOWN_LEN)
	{
	    error->err_code = E_PS0C05_BAD_ADF_STATUS;
	    return(E_DB_SEVERE);
	}

	/* Remember the result length of the coercion */
	resval->db_prec = resop.db_prec;
	resval->db_length = resop.db_length;
	resval->db_datatype = resop.db_datatype;
    }
    /* Convert from non-nullable to nullable */
    else if (resval->db_datatype > 0 && nullable)
    {
	resval->db_datatype = -resval->db_datatype;
	resval->db_length++;
    }

    return(E_DB_OK);
}

/*{
** Name: pst_parm_resolve   - Resolve a resdom node representing
**			      local var/parm.
**
** Description:
**      The function resolves datatypes for a resdom node representing
**	a local variable or a parameter. The datatype of the resdom node
**	must not actually change, it's the right child that needs adjustment.
**
** Inputs:
**
** Outputs:
**	Returns:
**	    E_DB_OK			Function completed normally.
**	    E_DB_WARN			Function completed with warning(s)
**	    E_DB_ERROR			Function failed; non-catastrophic error
**	    E_DB_FATAL			Function failed; catastrophic error
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	08-jun-88 (stec)
**	    written
**	03-nov-88 (stec)
**	    Correct bug found by lint.
*/
DB_STATUS
pst_parm_resolve(
	PSS_SESBLK  *sess_cb,
	PSQ_CB	    *psq_cb,
	PST_QNODE   *resdom)
{
    ADF_CB	    *adf_scb = (ADF_CB *)sess_cb->pss_adfcb;
    ADI_DT_BITMASK  mask;
    DB_STATUS	    status;
    i4	    err_code;
    DB_DT_ID	    rdt, cdt;
    ADI_DT_NAME	    rdtname, cdtname;
    ADI_FI_ID	    convid;
    

    /* Make sure there is a node to resolve */
    if (resdom == (PST_QNODE *) NULL || 
	resdom->pst_right == (PST_QNODE*) NULL
       )
    {
	return (E_DB_OK);
    }

    /* Make sure it's a resdom node */
    if (resdom->pst_sym.pst_type != PST_RESDOM)
    {
	(VOID) psf_error(E_PS0C03_BAD_NODE_TYPE, 0L,
	    PSF_INTERR, &err_code, &psq_cb->psq_error, 0);
	return (E_DB_SEVERE);
    }

    rdt = resdom->pst_sym.pst_dataval.db_datatype;
    cdt = resdom->pst_right->pst_sym.pst_dataval.db_datatype;

    /* See what the right child is coercible into. */
    status = adi_tycoerce(adf_scb, abs(cdt), &mask);

    if (DB_FAILURE_MACRO(status))
    {
	if (adf_scb->adf_errcb.ad_errclass == ADF_USER_ERROR)
	{
	    (VOID) psf_error(E_PS0C05_BAD_ADF_STATUS,
		adf_scb->adf_errcb.ad_usererr, PSF_INTERR,
		&err_code, &psq_cb->psq_error, 0);
	}
	else
	{
	    (VOID) psf_error(E_PS0C05_BAD_ADF_STATUS,
		adf_scb->adf_errcb.ad_errcode, PSF_INTERR,
		&err_code, &psq_cb->psq_error, 0);
	}
	return(status);
    }

    /* See what the resdom data type is one of the options. */
    if (BTtest(ADI_DT_MAP_MACRO(abs(rdt)), (char *)&mask) == FALSE)
    {
	/* Conversion impossible. */

	status = adi_tyname(adf_scb, rdt, &rdtname);
	if (DB_FAILURE_MACRO(status))
	{
	    if (adf_scb->adf_errcb.ad_errclass == ADF_USER_ERROR)
	    {
		(VOID) psf_error(E_PS0C04_BAD_TREE,
		    adf_scb->adf_errcb.ad_usererr, PSF_INTERR,
		    &err_code, &psq_cb->psq_error, 0);
	    }
	    else
	    {
		(VOID) psf_error(E_PS0C04_BAD_TREE,
		    adf_scb->adf_errcb.ad_errcode, PSF_INTERR,
		    &err_code, &psq_cb->psq_error, 0);
	    }
	    return (E_DB_SEVERE);
	}

	status = adi_tyname(adf_scb, cdt, &cdtname);
	if (DB_FAILURE_MACRO(status))
	{
	    if (adf_scb->adf_errcb.ad_errclass == ADF_USER_ERROR)
	    {
		(VOID) psf_error(E_PS0C04_BAD_TREE,
		    adf_scb->adf_errcb.ad_usererr, PSF_INTERR,
		    &err_code, &psq_cb->psq_error, 0);
	    }
	    else
	    {
		(VOID) psf_error(E_PS0C04_BAD_TREE,
		    adf_scb->adf_errcb.ad_errcode, PSF_INTERR,
		    &err_code, &psq_cb->psq_error, 0);
	    }
	    return (E_DB_SEVERE);
	}

	(VOID) psf_error(2417L, 0L, PSF_USERERR, &err_code, 
	    &psq_cb->psq_error, 2,
	    psf_trmwhite(sizeof(ADI_DT_NAME), (char *) &cdtname), &cdtname,
	    psf_trmwhite(sizeof(ADI_DT_NAME), (char *) &rdtname), &rdtname); 

	return (E_DB_ERROR);
    }

    return    (E_DB_OK);
}

/*{
** Name: pst_convlit	- Convert constant literal between string & 
**				numeric
**
** Description:
**      In INSERT/UPDATE statements and binary expressions, if context
**	dictates that type mismatch can be overcome by converting a 
**	constant literal to the target type, we'll give it a whack.
**	This is integral to the Rpath request to support integer literals
**	in character columns and vice versa.
**
** Inputs:
**	sess_cb		- ptr to parser control block
**	stream		- ptr to memory stream for constant allocation
**	targdv		- ptr to target type description
**	srcenode	- ptr to source node
**
** Outputs:
**	Returns:
**	    FALSE			Conversion couldn't be done
**	    TRUE			Conversion was successful
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	13-sep-2006 (dougi)
**	    Written.
*/
bool
pst_convlit(
	PSS_SESBLK  *sess_cb,
	PSF_MSTREAM *stream,
	DB_DATA_VALUE *targdv,
	PST_QNODE   *srcenode)

{
    ADF_CB	*adf_cb = sess_cb->pss_adfcb;
    DB_STATUS	status;
    DB_ERROR	errblk;
    DB_DATA_VALUE convdv;
    char	*convptr;
    DB_DT_ID	targtype, srcetype;


    /* Verify that source node is a PST_CONST literal. */
    if (srcenode->pst_sym.pst_type != PST_CONST ||
	srcenode->pst_sym.pst_value.pst_s_cnst.pst_tparmtype != PST_USER)
	return(FALSE);

    /* We have a literal - now see if it is coercible. Verify the 
    ** permissable combinations. */
    srcetype = abs(srcenode->pst_sym.pst_dataval.db_datatype);
    targtype = abs(targdv->db_datatype);

    switch (targtype) {
      case DB_INT_TYPE:
      case DB_FLT_TYPE:
      case DB_DEC_TYPE:
	if (srcetype != DB_CHA_TYPE && srcetype != DB_VCH_TYPE)
	    return(FALSE);
	break;

      case DB_CHA_TYPE:
      case DB_VCH_TYPE:
	if (srcetype != DB_INT_TYPE && srcetype != DB_FLT_TYPE &&
		srcetype != DB_DEC_TYPE)
	    return(FALSE);
	break;

      default:	/* All the rest won't work. */
	return(FALSE);
    }

    /* Passed the easy tests - allocate space for the converted literal. */
    status = psf_malloc(sess_cb, stream, targdv->db_length, 
	&convptr, &errblk);
    if (status != E_DB_OK)
	return(FALSE);

    STRUCT_ASSIGN_MACRO(*targdv, convdv);
    if (convdv.db_datatype < 0)
    {
	/* Convert to not null so's not to offend the coercion functions. */
	convdv.db_datatype = -convdv.db_datatype;
	convdv.db_length--;
    }

    convdv.db_data = convptr;

    /* Use switch to direct coercion to correct function. */
    switch(targtype) {
      case DB_INT_TYPE:
	status = adu_1int_coerce(adf_cb, &srcenode->pst_sym.pst_dataval,
						&convdv);
	break;

      case DB_FLT_TYPE:
	status = adu_1flt_coerce(adf_cb, &srcenode->pst_sym.pst_dataval,
						&convdv);
	break;

      case DB_DEC_TYPE:
	status = adu_1dec_coerce(adf_cb, &srcenode->pst_sym.pst_dataval,
						&convdv);
	break;

      case DB_CHA_TYPE:
      case DB_VCH_TYPE:
	status = adu_ascii(adf_cb, &srcenode->pst_sym.pst_dataval,
						&convdv);
	break;

      default:	/* Shouldn't get here - but just in case */
	return(FALSE);
    }

    /* If coercion failed, just return FALSE. */
    if (status != E_DB_OK)
	return(FALSE);

    /* Copy the resulting DB_DATA_VALUE and return with success. */
    STRUCT_ASSIGN_MACRO(convdv, srcenode->pst_sym.pst_dataval);
    return(TRUE);		/* Ta da! */

}
