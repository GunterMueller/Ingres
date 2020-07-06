/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <me.h>
#include    <qu.h>
#include    <st.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ddb.h>
#include    <dmf.h>
#include    <dmtcb.h>
#include    <adf.h>
#include    <adfops.h>
#include    <ade.h>
#include    <ulf.h>
#include    <qsf.h>
#include    <scf.h>
#include    <qefrcb.h>
#include    <rdf.h>
#include    <psfparse.h>
#include    <psfindep.h>
#include    <pshparse.h>

/**
**
**  Name: PSTNORML.C - Functions for normalizing qualifications.
**
**  Description:
**      This file used to contain the functions for putting query tree
**	qualifications into conjunctive normal form.  That is now done
**	in OPF, and all that's left here are a couple utility routines.
**
**	    pst_node_size - Return the size of a tree node
**	    pst_treedup - Duplicate a sub-tree
**
**
**  History:
**      03-apr-86 (jeff)    
**          Adapted from the normalization routines in 3.0
**	18-feb-88 (stec)
**	    All normalization code has been `IFDEFed' out.
**	    Normalization is now done in OPF.
**	02-mar-88 (stec)
**	    pst_treedup needs to copy exact size of the node.
**	02-may-88 (stec)
**	    Also copy PST_CURVAL node.
**	10-jun-88 (stec)
**	    Modified for DB procedures.
**	08-mar-89 (neil)
**	    Support for new rule nodes.
**	02-jun-89 (andre)
**	    wrote pst_dup_viewtree()
**	12-sep-89 (andre)
**	    Collapsed pst_dup_viewtree into an all new and improved
**	    pst_treedup().
**	11-nov-91 (rblumer)
**	  merged from 6.4:  19-aug-91 (andre)
**	    changed casting from (PTR) to (char *) so that pointer arithmetic 
**	    works on all platforms.
**	14-jan-92 (barbara)
**	    Included ddb.h for Star.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**      16-sep-93 (smc)
**          Added/moved <cs.h> for CS_SID. Added history_template so we
**          can automate this next time.
**	10-Jan-2001 (jenjo02)
**	    Added *PSS_SELBLK parm to psf_mopen(), psf_mlock(), psf_munlock(),
**	    psf_malloc(), psf_mclose(), psf_mroot(), psf_mchtyp(),
**	    psl_rptqry_tblids(), psl_cons_text(), psl_gen_alter_text(),
**	    psq_tmulti_out(), psq_1rptqry_out(), psq_tout(), pst_treedup().
**      16-apr-2001 (stial01)
**          pst_node_size() Added PST_MOP and PST_OPERAND cases
**	22-Jul-2004 (schka24)
**	    Delete old-code, been ifdef'ed out for 16 years.
[@history_template@]...
**/

/*{
** Name: pst_node_size 	- determine exact size of the node.
**
** Description:
**      Given a node, this function willl determine length of symbol and data
**	portions thereof.
**
** Inputs:
**      sym				ptr to the symbol representing the node
**
** Outputs:
**      symsize				length of the symbol part of the node
**	datasize			length of the data part of the node
**	err_blk				Filled in if an error happened
**	Returns:
**	    E_DB_SEVERE if unknown node type is encountered;
**	    E_DB_OK otherwise.
** Side Effects:
**	    None
**
** History:
**	14-nov-89 (andre) 
**	    fix bug 8624.  if pst_len != symsize+datasize, first check if it
**	    could be a node created for a repeat query TEXT or VCHR parameter,
**	    in which case db_length would be purposely set to -1.
**	    This fix was made before the function was split of from the body of
**	    pst_treedup().
**	27-dec-89 (andre)
**	    Borrowed from pst_treedup().  This function will be used in
**	    pst_treedup() and pst_trdup(), thus assuring that changes, if any,
**	    will have to be made in exactly one spot.
**	3-sep-99 (inkdo01)
**	    Added new nodes for case function.
**	12-feb-03 (inkdo01)
**	    Belatedly added sequence operation node.
**	3-apr-06 (dougi)
**	    Added PST_GSET, PST_GBCR, PST_GCL nodes for group by enhancements.
*/
DB_STATUS
pst_node_size(
	PST_SYMBOL	    *sym,
	i4		    *symsize,
	i4		    *datasize,
	DB_ERROR	    *err_blk)
{
    switch (sym->pst_type)
    {
	case PST_AND:
	case PST_OR:
	case PST_UOP:
	case PST_BOP:
	case PST_AOP:
	case PST_COP:
	case PST_MOP:
	    *symsize = sizeof(PST_OP_NODE);
	    break;
	case PST_CONST:
	    *symsize = sizeof(PST_CNST_NODE);

	    /*
	    ** Because of special treatment accorded to repeat query parameters
	    ** of type VCHR or TEXT (widely known as VLUPs), datasize of
	    ** PST_CONST node will be determined here.
	    ** There is a little trick here if we are dealing with a VLUP:
	    **	    in pst_2adparm(), db_length was reset to ADE_LEN_UNKNOWN,
	    **	    but we can still deduce it since
	    **	    pst_len = symsize + db_length.
	    */
	    if (sym->pst_dataval.db_data != (PTR) NULL)
	    {
		*datasize =
		    (sym->pst_dataval.db_length == ADE_LEN_UNKNOWN &&
		     sym->pst_value.pst_s_cnst.pst_tparmtype == PST_RQPARAMNO)
			? sym->pst_len - *symsize
			: sym->pst_dataval.db_length;
	    }
	    else
	    {
		*datasize = 0;
	    }

	    break;
	case PST_RESDOM:
	case PST_BYHEAD:
	    *symsize = sizeof(PST_RSDM_NODE);
	    break;
	case PST_VAR:
	    *symsize = sizeof(PST_VAR_NODE);
	    break;
	case PST_AGHEAD:
	case PST_ROOT:
	case PST_SUBSEL:
	    *symsize = sizeof(PST_RT_NODE);
	    break;
	case PST_QLEND:
	case PST_TREE:
	case PST_NOT:
	case PST_WHLIST:
	case PST_WHOP:
	case PST_OPERAND:
	case PST_GSET:
	case PST_GCL:
	    *symsize = 0;
	    break;
	case PST_CASEOP:
	    *symsize = sizeof(PST_CASE_NODE);
	    break;
	case PST_SORT:
	    *symsize = sizeof(PST_SRT_NODE);
	    break;
	case PST_CURVAL:
	    *symsize = sizeof(PST_CRVAL_NODE);
	    break;
	case PST_RULEVAR:
	    *symsize = sizeof(PST_RL_NODE);
	    break;
	case PST_SEQOP:
	    *symsize = sizeof(PST_SEQOP_NODE);
	    break;
	case PST_GBCR:
	    *symsize = sizeof(PST_GROUP_NODE);
	    break;
	default:
	{
	    i4	err_code;
	    
	    (VOID) psf_error(E_PS0C03_BAD_NODE_TYPE, 0L, PSF_INTERR,
		&err_code, err_blk, 0);
	    return (E_DB_SEVERE);
	}
    }

    /*
    ** Now find the data size.  Datasize for PST_CONST node has already been
    ** determined
    */
    if (sym->pst_type != PST_CONST)
    {
	if (sym->pst_dataval.db_data != (PTR) NULL)
	{
	    *datasize = sym->pst_dataval.db_length;
	}
	else
	{
	    *datasize = 0;
	}
    }

    return(E_DB_OK);
}

/*{
** Name: pst_treedup	- Duplicate a view tree
**
** Description:
**	This function is a more sophisticated version of the original
**	pst_treedup.  In addition to copying the tree passed to it, it may
**	perform requested services for the caller.  For the time being it
**	"understands" 2 opcodes: PSS_1DUP_SEEK_AGHEAD and PST_2DUP_RESET_JOINID.
**	Meanings of the masks are as follows:
**	PSS_1DUP_SEEK_AGHEAD --  while duplicating a tree, search for
**				 occurrences of AGHEAD with BYHEAD as its left
**				 child. If found, set an apprpriate bit in
**				 dup_cb->pss_tree_info.  Whenever ROOT or SUBSEL
**				 is duplicated reset tree_info to point at
**				 pst_mask1 in the newly duplicated node, as we
**				 want this information stored in the node.  In
**				 addition to that, if such occurrence is found
**				 in the outermost SUBSELECT (i.e. no SUBSEL
**				 nodes between the node where this occurrence
**				 was found and PST_ROOT), we want to
**				 communicate this knowledge to the caller,
**				 hence, if the bit was set in PST_ROOT's
**				 pst_mask1, it will be set in the mask passed
**				 by the caller inside dup_cb.  Doing so will
**				 allow us to determine if one or more members of
**				 the union had a AGHEAD/BYHEAD combination in
**				 their outer SUBSELECTs as well as pass the info
**				 to the caller of pst_treedup().
**				 This will not be done for the case when the
**				 bit is set inside SUBSEL's pst_mask1, as this
**				 in this case, information is only of interest
**				 while processing the corresponding SUBSELECT.
**	PSS_2DUP_RESET_JOINID -- while duplicating a tree, search for
**				 occurrences of operator nodes (PST_AND, PST_OR,
**				 PST_AOP, PST_BOP, PST_UOP, PST_COP), and if
**				 pst_joinid is != PST_NOJOIN, increment it by
**				 dup_cb->pst_numjoins.
**
**	To produce a copy of the original tree, it traverses the tree,
**	allocating space for each node based on the length in the node's
**	header, and copies the contents.
**
** Inputs:
**	dup_rb		    ptr to dup. request block
**	    pss_op_mask		    requested_services mask
**	    pss_numjoins	    number of joins found in the query so far
**	    pss_tree_info	    ptr to a mask used to communicate all sorts
**				    of useful info to the caller
**	    pss_mstream		    Memory stream
**	    pss_tree		    Pointer to tree to be duplicated
**	    pss_dup		    Place to put pointer to duplicate
**	    pss_err_blk		    Filled in if an error happens
**
** Outputs:
**	dup_rb		    ptr to dup. request block
**      pss_dup			    Filled in with pointer to copy
**	*pss_tree_info		    may be set if a condition for which we
**				    are looking has occured
**	    PSS_GROUP_BY_IN_TREE    found an AGHEAD whose left child is a BYHEAD
**	pss_err_blk		    Filled in if an error happened
**	Returns:
**	    E_DB_OK			Success
**	    E_DB_ERROR			Non-catastrophic error
**	    E_DB_FATAL			Catastrophic error
**	Exceptions:
**	    none
**
** Side Effects:
**	    Allocates memory
**
** History:
**	06-may-86 (jeff)
**          Adapted from treedup() in 3.0
**	02-mar-88 (stec)
**	    pst_treedup needs to copy exact size of the node.
**	02-may-88 (stec)
**	    Also copy PST_CURVAL node.
**	10-jun-88 (stec)
**	    Modified for DB procedures. Make adjustments for
**	    the new resdom and constant node sizes, i.e., make
**	    more robust for the case when node sizes change and
**	    we're retrieving old trees from catalogs.
**	08-mar-89 (neil)
**	    Support for new rule nodes.
**	11-sep-89 (andre)
**	    make changes to handle group IDs in the operator nodes.  To this end
**	    we will accept a number_of_joins_in_the_tree and increment
**	    meaningful (not PST_NOJOIN) group ids by the number of group ids
**	    in the rest of the query.
**	    Also added changes to allow searching for specific conditions (e.g.
**	    AGHEAD/BYHEAD combinations.)
**	1-oct-89 (andre)
**	    Propagate fix made in 7.0 line.  2 new masks will be "understood" by
**	    pst_treedup():
**	    PSS_3DUP_RENUMBER_BYLIST	--  renumber elements of BY list;
**	    PSS_4DUP_COUNT_RSDMS	--  after a tree rooted in RESDOM has
**					    been copied, increment
**					    dup_rb->pss_rsdm_no and use the
**					    result as a new resdom number.
**	22-jan-90 (andre)
**	    If there is a datavalue to copy, make sure that the destination
**	    db_data points at the newly allocated memory.  Until now, it would
**	    contain the value of the source db_data pointer, and so the source
**	    datavalue would be copied unto itself.
**	23-jan-90 (andre)
**	    make use of the newly defined pst_node_size().
**	27-mar-90 (andre)
**	    propagate fix for bug 20499.  1 new mask will be recognized by
**	    pst_treedup():
**	    PSS_5DUP_ATTACH_AGHEAD	-- attach all newly copied PST_AGHEAD
**					   nodes to the list of agheads whose
**					   address has been passed by the caller
**	10-jul-90 (andre)
**	    If PSS_3DUP_RENUMBER_BYLIST is set, renumber resdom nodes in the
**	    BY-list AFTER the BYHEAD node and its subtrees are processed.
**	    Previously, we would set PSS_4DUP_COUNT_RSDMS and use pss_num_rsdms
**	    to keep track of resdom numbers as they are being renumbered.  This
**	    approach fails since it relies on an assumption that there may be no
**	    BYHEAD nodes having another BYHEAD as a descendant, which is easily
**	    violated (e.g. retrieve (a=sum((t1.i) by avg((t1.i) by t1.i))).)
**	30-sep-92 (andre)
**	    pss_tree_info will no longer point at PST_RT_NODE.pst_mask1, new
**	    masks have been defined over it
**	01-oct-92 (andre)
**	    remember whether a tree being copied involved singleton subselects
**	    only when instructed to do so by the caller
**	    (dup_rb->pss_op_mask & PSS_4SEEK_SINGLETON_SUBSEL)
**	31-dec-92 (andre)
**	    pst_treedup() will no longer be asked to look for singleton
**	    subselects inside the view tree - this information will be stored
**	    inside the view tree header
**	04-jan-93 (andre)
**	    handle new opcode:
**		PSS_MAP_VAR_TO_RULEVAR - when set, every for every occurrence of
**		a PST_VAR node we will create a copy of a PST_RULEVAR node
**		pointed to by pss_1ptr and overwrite PST_RL_NODE.pst_rltno with
**		PST_VAR_NODE.pst_atno.db_att_id
*/
DB_STATUS
pst_treedup(
	PSS_SESBLK	   *sess_cb,
	PSS_DUPRB	   *dup_rb)
{
    DB_STATUS           status;
    PST_QNODE		*newtree;
    PST_QNODE		*origtree = dup_rb->pss_tree;
    PST_QNODE		**duptree = dup_rb->pss_dup;
    i4			size, symsize;
    i4			datasize;
    i4			*savemask;
    i4			tree_info = 0;
    i4		err_code;
    PST_J_ID		new_j_id = PST_NOJOIN;

    if (origtree == (PST_QNODE *) NULL)
    {
	*duptree = (PST_QNODE *) NULL;
	return (E_DB_OK);
    }

    if (   origtree->pst_sym.pst_type == PST_VAR
	&& dup_rb->pss_op_mask & PSS_MAP_VAR_TO_RULEVAR)
    {
	/*
	** we were asked to map VAR node into RULEVAR - temporarily set origtree
	** to the RULEVAR node which will be replicated
	*/
	origtree = (PST_QNODE *) dup_rb->pss_1ptr;
    }
    
    /*
    ** Determine the size of the variant part of the node and the size of data
    */
    status = pst_node_size(&origtree->pst_sym, &symsize, &datasize,
			   dup_rb->pss_err_blk);
    if (DB_FAILURE_MACRO(status))
    {
	return(status);
    }
    
    switch (origtree->pst_sym.pst_type)
    {
	case PST_BOP:
	case PST_AND:
	case PST_OR:
	case PST_UOP:
	case PST_AOP:
	case PST_COP:
	case PST_MOP:
	{
	    /*
	    ** If we were requested to update join (group) id, AND there are
	    ** joins in the rest of the query, AND this op_node is a
	    ** part of a join_search condition, save the new group_id to be
	    ** assigned to the copy of the node.  If there are no joins in the
	    ** rest of the tree, the old group_id will be used; if node was not
	    ** a part of join_search condition, then there is no reason to reset
	    ** its group_id.
	    */
	    if (dup_rb->pss_op_mask & PSS_2DUP_RESET_JOINID &&
	        dup_rb->pss_num_joins != PST_NOJOIN	    &&
	        (new_j_id = origtree->pst_sym.pst_value.pst_s_op.pst_joinid) !=
								     PST_NOJOIN)
	    {
		new_j_id += dup_rb->pss_num_joins;
	    }
	    break;
	}

	/*
	** Note that pst_node_size will report error if invalid node type is
	** detected
	*/
    }

    /*
    ** allocate enough space (data value included), i.e. space for the constant
    ** part of the node (common to all node types), variable part, and data
    ** if any.
    */
    size = sizeof(PST_QNODE) - sizeof(PST_SYMVALUE) + symsize + datasize;

    /*
    ** Perform consistency check
    */
    if (origtree->pst_sym.pst_len != symsize + datasize)
    {
	/* RDF returned incorrect pst_len value. */
	(VOID) psf_error(E_PS0D32_RDF_INV_NODE, 0L, PSF_INTERR,
	    &err_code, dup_rb->pss_err_blk, 0);
	return(E_DB_ERROR);
    }

    status = psf_malloc(sess_cb, dup_rb->pss_mstream, size, (PTR *) duptree,
			dup_rb->pss_err_blk);
    if (status != E_DB_OK)
	return (status);

    newtree = *duptree;
    
    /* Copy the old node to the new node */

    size = sizeof(PST_QNODE) - sizeof(PST_SYMVALUE) + symsize;
    (VOID) MEcopy((PTR) origtree, size, (PTR) newtree);

    if (datasize != 0)
    {
	/*
	** note that of the allocated memory,
	** (sizeof(PST_QNODE) - sizeof(PST_SYMVALUE) + symsize) bytes
	** have been used to copy the node, the remaining datasize bytes
	** will be used to contain the datavalue
	*/
	newtree->pst_sym.pst_dataval.db_data = (char *) newtree + size;
	
	MEcopy((PTR) origtree->pst_sym.pst_dataval.db_data,
	    datasize,
	    (PTR) newtree->pst_sym.pst_dataval.db_data);
    }

    /*
    ** we are done copying node pointed at by ORIGTREE; if we were asked to map
    ** VAR nodes into RULEVAR nodes, here's a good spot to reset origtree to
    ** point at the VAR node passed in by the caller + we need to overwrite
    ** attribute number in the rule node with the attribute number from the VAR
    ** node
    */
    if (   dup_rb->pss_tree->pst_sym.pst_type == PST_VAR
	&& dup_rb->pss_op_mask & PSS_MAP_VAR_TO_RULEVAR)
    {
	origtree = dup_rb->pss_tree;
	newtree->pst_sym.pst_value.pst_s_rule.pst_rltno =
	    origtree->pst_sym.pst_value.pst_s_var.pst_atno.db_att_id;
    }

    /*
    ** If this is an operator node, and its group_id needs to be reset, do it
    ** here
    */
    if (new_j_id != PST_NOJOIN)
    {
	newtree->pst_sym.pst_value.pst_s_op.pst_joinid = new_j_id;
    }

    /* we were asked to search for AGHEAD/BYHEAD combinations */
    if (dup_rb->pss_op_mask & PSS_1DUP_SEEK_AGHEAD)
    {
	/*
	** If node is ROOT or SUBSEL, we need to store indicator of whether
	** AGHEAD/BYHEAD combinations in pst_mask1 of the ROOT or SUBSEL node;
	** if, on the other hand, node is an AGHEAD with a BYHEAD for a left
	** child, set a bit in *pss_tree_info accordingly
	*/
	if (newtree->pst_sym.pst_type == PST_ROOT ||
	    newtree->pst_sym.pst_type == PST_SUBSEL)
	{
	    /* First save the tree_info mask passed by the caller */
	    savemask = dup_rb->pss_tree_info;

	    dup_rb->pss_tree_info = &tree_info;
	}
	else if (newtree->pst_sym.pst_type == PST_AGHEAD &&
		 newtree->pst_left->pst_sym.pst_type == PST_BYHEAD)
	{
	    *dup_rb->pss_tree_info |= PSS_GROUP_BY_IN_TREE;
	}
    }

    /*
    ** Check if the newly copied node is a PST_AGHEAD and the caller requested
    ** that AGHEAD's be inserted into the list of AGHEAD's
    */
    if (dup_rb->pss_op_mask & PSS_5DUP_ATTACH_AGHEAD &&
	newtree->pst_sym.pst_type == PST_AGHEAD)
    {
	YYAGG_NODE_PTR         *new_agg_node;

	/* save location of new agg head */
	status = psf_malloc(sess_cb, dup_rb->pss_mstream, (i4) sizeof(YYAGG_NODE_PTR), 
	    (PTR *) &new_agg_node, dup_rb->pss_err_blk);
	if (status != E_DB_OK)
	    return (status);

	new_agg_node->agg_next = *((YYAGG_NODE_PTR **) dup_rb->pss_1ptr);
	new_agg_node->agg_node = newtree;
	*((YYAGG_NODE_PTR **) dup_rb->pss_1ptr) = new_agg_node;
    }

    /*
    ** Now duplicate the left and right sub-trees.  First we need to reset some
    ** fields in the dup_rb
    */

    dup_rb->pss_tree = origtree->pst_left;
    dup_rb->pss_dup  = &newtree->pst_left;
    status = pst_treedup(sess_cb, dup_rb);
    
    if (status != E_DB_OK)
	return (status);

    dup_rb->pss_tree = origtree->pst_right;
    dup_rb->pss_dup  = &newtree->pst_right;
    status = pst_treedup(sess_cb, dup_rb);

    if (status != E_DB_OK)
	return (status);

    switch (origtree->pst_sym.pst_type)
    {
	case PST_ROOT:
	case PST_SUBSEL:
	{
	    /* duplicate a union */
	    
	    i4		u_mask = 0;
	    
	    dup_rb->pss_tree =
		      origtree->pst_sym.pst_value.pst_s_root.pst_union.pst_next;
	    dup_rb->pss_dup  =
		      &newtree->pst_sym.pst_value.pst_s_root.pst_union.pst_next;
	    if (dup_rb->pss_op_mask & PSS_1DUP_SEEK_AGHEAD)
	    {
		dup_rb->pss_tree_info = &u_mask;
	    }
	    
	    status = pst_treedup(sess_cb, dup_rb);

	    if (status != E_DB_OK)
		return (status);

	    /*
	    ** If we were asked to look for AGGHEAD/BYHEAD combination, before
	    ** we have reset dup_rb->pss_tree_info to &u_mask it contained
	    ** &tree_info.  Bits that may be set in tree_info and u_mask and
	    ** their meaning is as follows: 
	    **
	    ** PSS_GROUP_BY_IN_TREE	-   tree rooted in this node contained
	    **				    an AGGHED with a BYHEAD for a left
	    **				    child with no intervening SUBSELs -
	    **				    set PST_3GROUP_BY in
	    **				    pst_s_root.pst_mask1 but do NOT
	    **				    pass this info to the caller unless
	    **				    we are processing a ROOT node
	    */

	    if (dup_rb->pss_op_mask & PSS_1DUP_SEEK_AGHEAD)
	    {
		if (tree_info & PSS_GROUP_BY_IN_TREE)
		{
		    newtree->pst_sym.pst_value.pst_s_root.pst_mask1 |=
		        PST_3GROUP_BY;
		}
		else
		{
		    newtree->pst_sym.pst_value.pst_s_root.pst_mask1 &=
		        ~PST_3GROUP_BY;
		}
		
		/*
		** propagate info about AGGHEAD/BYHEAD combinations to the
		** original caller (i.e. we will not propagate it "up the tree")
		** or to other members of the union
		*/
		if (origtree->pst_sym.pst_type == PST_ROOT)
		{
		    *savemask |= (u_mask | tree_info) & PSS_GROUP_BY_IN_TREE;
		}

		/*
		** restore the mask ptr that would be reset for ROOT and SUBSEL
		*/
		dup_rb->pss_tree_info = savemask;
	    }

	    break;
	}
	case PST_BYHEAD:
	{
	    /*
	    ** Done processing subtree rooted in BYHEAD.  If caller requested
	    ** that we renumber elements of the BY-lists, do it now.
	    */
	    if (dup_rb->pss_op_mask & PSS_3DUP_RENUMBER_BYLIST)
	    {
		register i4	    rsdmno;
		register PST_QNODE  *bylist_elem;

		/* first, determine number of RESDOM nodes in the BY-list */
		for (bylist_elem = newtree->pst_left, rsdmno = 0;
		     bylist_elem->pst_sym.pst_type == PST_RESDOM;
		     bylist_elem = bylist_elem->pst_left, rsdmno++)
		;

		/* now renumber them */
		for (bylist_elem = newtree->pst_left;
		     rsdmno > 0;
		     bylist_elem = bylist_elem->pst_left, rsdmno--)
		{
		    bylist_elem->pst_sym.pst_value.pst_s_rsdm.pst_rsno = rsdmno;
		}
	    }
	    break;
	}
    }

    /*
    ** restore ptr to the original tree and addr of the ptr to the copy to
    ** values passed by the caller
    */
    dup_rb->pss_tree      = origtree;
    dup_rb->pss_dup	  = duptree;
    return (E_DB_OK);
}
