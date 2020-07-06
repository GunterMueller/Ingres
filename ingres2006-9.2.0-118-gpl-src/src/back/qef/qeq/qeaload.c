/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ddb.h>
#include    <ulf.h>
#include    <ulm.h>
#include    <adf.h>
#include    <ade.h>
#include    <dmf.h>
#include    <dmrcb.h>
#include    <dmtcb.h>
#include    <qefmain.h>
#include    <qsf.h>
#include    <qefrcb.h>
#include    <qefcb.h>
#include    <qefnode.h>
#include    <psfparse.h>
#include    <qefact.h>
#include    <qefqp.h>
#include    <qefdsh.h>

#include    <dmmcb.h>
#include    <dmucb.h>
#include    <ex.h>
#include    <tm.h>

#include    <dudbms.h>
#include    <qefqeu.h>
#include    <qeuqcb.h>
#include    <rqf.h>
#include    <tpf.h>
#include    <qefkon.h>
#include    <qefqry.h>
#include    <qefcat.h>
#include    <sxf.h>
#include    <qefprotos.h>

/**
**
**  Name: QEALOAD.C - provides tuple load support
**
**  Description:
**      The routines in this file append records to an open empty relation
**
**          qea_load - append one or more records to the open, empty relation.
**
**
**  History:    $Log-for RCS$
**      17-sep-87 (rogerk)
**          written
**	02-feb-88 (puree)
**	    add reset flag to qea_load function arguments.
**	02-jul-91 (nancy)
**	    bug 36834, added resource control check for load operations so
**	    that resource control can be applied to create table as select
**	    statements.
**	11-may-92 (nancy)
**	    Fix bug 44076.  Add support here to fix "SYBASE" bug occuring when
**	    a table is empty and we need to retry without flattening.
**          This is done by returning a zerotup check flag when invalidating 
**          a query plan because it's QEN_NODE returns zero rows.
**	08-dec-92 (anitap)
**	    Include <qsf.h> and <psfparse.h> for CREATE SCHEMA.
**      13-sep-93 (smc)
**          Added <cs.h> for CS_SID.
**	7-nov-95 (inkdo01)
**	    Changes to replace QEN_ADF structure instances by pointers in
**	    QEF_AHD structures.
**	06-jul-1998 (somsa01)
**	    If the zero tuple check is on, and the query is a declaration of
**	    a global temporary table as a select, then we need to destroy
**	    the temp table before allowing the query to be re-run.
**	    (Bug #91827)
**	23-feb-04 (inkdo01)
**	    Changed qef_rcb->error to dsh->dsh_error for || thread safety
**	    (and others).
**	28-Dec-2005 (kschendel)
**	    Use DSH references whenever possible instead of QEF RCB
**	    references.  (This simplifies the code, eliminating many
**	    RCB references, and reflects the new primacy of the DSH
**	    thanks to the parallel query changes.)
[@history_template@]...
**/


/*{
** Name: QEA_LOAD	- append rows to table
**
** Description:
**      If a QEP exists, then rows are retrieved from the QEP
**  and used to load the result relation. Otherwise, just
**  materialize the output tuple and load it. 
**
** Inputs:
**      action                          append action item
**
** Outputs:
**      qef_rcb
**	    .qef_rowcount		number of output tuples
**	    .qef_targcount		number of attempted puts
**	    .error.err_code		one of the following
**					    E_QE0000_OK
**					    E_QE0017_BAD_CB
**					    E_QE0018_BAD_PARAM_IN_CB
**					    E_QE0019_NON_INTERNAL_FAIL
**					    E_QE0002_INTERNAL_ERROR
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
**      17-sep-87 (rogerk)
**          written
**	30-sep-87 (rogerk)
**	    Get correct count of tuples loaded.  Use count returned from
**	    ENDLOAD call to dmr_load.  This will not include duplicates and
**	    duplicate keys that may have been removed.
**	02-nov-87 (puree)
**	    set dmr_s_estimated_records from qen_est_tuples.
**	02-feb-88 (puree)
**	    add reset flag to the function arguments.
**	02-jul-91 (nancy)
**	    bug 36834, added resource control check for load operations so
**	    that resource control can be applied to create table as select
**	    statements.
**	11-may-92 (nancy)
**	    Fix bug 44076.  Add support here to fix "SYBASE" bug occuring when
**	    a table is empty and we need to retry without flattening.
**          This is done by returning a zerotup check flag when invalidating 
**          a query plan because it's QEN_NODE returns zero rows.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	7-nov-95 (inkdo01)
**	    Changes to replace QEN_ADF structure instances by pointers in
**	    QEF_AHD structures.
**	06-jul-1998 (somsa01)
**	    If the zero tuple check is on, and the query is a declaration of
**	    a global temporary table as a select, then we need to destroy
**	    the temp table before allowing the query to be re-run.
**	    (Bug #91827)
**	3-dec-01 (inkdo01)
**	    Add support for "select first n" in declare global temp tab as ... and 
**	    create table as ... 
**	26-mar-03 (inkdo01)
**	    Add support for "nextval" expressions (in DGTT statements).
**	21-nov-03 (inkdo01)
**	    Add open_flag parm to qen_func calls for parallel query support.
**	29-dec-03 (inkdo01)
**	    DSH is now parameter, "function" replaces "reset".
**	22-jan-04 (inkdo01)
**	    Added code to process rows in partitioned tables.
**	30-jan-04 (inkdo01)
**	    Fixed above - neglected to load dmr_cb addr for non-partitioned.
**	27-feb-04 (inkdo01)
**	    Changed dsh_qen_status from ptr to array of QEN_STATUSs to 
**	    ptr to arrays of ptrs.
**	28-feb-04 (inkdo01)
**	    Change qef_rcb parm to dsh in qeq_chk_resources call.
**	12-may-04 (inkdo01)
**	    Check more carefully for pre-opened DMR_CB.
**	16-Jan-2006 (kschendel)
**	    Access qen-status thru xaddrs.
[@history_template@]...
*/
DB_STATUS
qea_load(
QEF_AHD		*action,
QEF_RCB		*qef_rcb,
QEE_DSH		*dsh,
i4		function )
{
    QEN_NODE	*node = action->qhd_obj.qhd_qep.ahd_qep;
    bool	reset = ((function & FUNC_RESET) != 0);
    bool	node_reset = FALSE;
    QEF_CB	*qef_cb = dsh->dsh_qefcb;
    PTR		*cbs	= dsh->dsh_cbs;
    DMR_CB	*dmr_cb;
    DMT_CB	*dmt_cb;
    ADE_EXCB	*ade_excb;
    DB_STATUS	status;
    DM_MDATA	dm_mdata;
    i4	i, j;
    i4		open_flag = (reset) ? (TOP_OPEN | function) : function;
    i4		first_n = action->qhd_obj.qhd_qep.ahd_firstn;
    i4		rowno   = action->qhd_obj.qhd_qep.ahd_current->qen_output;
    u_i2	partno;
    char	*output = dsh->dsh_row[rowno];
    bool	firstrow = TRUE, newpart = FALSE;

    /* Check if resource limits are exceeded by this query */
    if (action->qhd_obj.qhd_qep.ahd_estimates && (qef_cb->qef_fl_dbpriv &
        (DBPR_QROW_LIMIT | DBPR_QDIO_LIMIT | DBPR_QCPU_LIMIT |
        DBPR_QPAGE_LIMIT | DBPR_QCOST_LIMIT))
        )
    {
        if (qeq_chk_resource(dsh, action) != E_DB_OK)
	    return (E_DB_ERROR);
    }

    /* materialize row into update buffer */
    ade_excb = (ADE_EXCB*) cbs[action->qhd_obj.qhd_qep.ahd_current->qen_pos];	
    dsh->dsh_qef_rowcount = 0;
    dsh->dsh_qef_targcount = 0;
    dmr_cb = (DMR_CB *)cbs[action->qhd_obj.qhd_qep.ahd_odmr_cb];

    for (i = 0, j = 0; (node) || (i == 0) ; i++)
    {
	if (node)
	{
	    /* fetch rows */
	    status = (*node->qen_func)(node, qef_rcb, dsh, open_flag);
	    node_reset = FALSE;
	    open_flag = function &= ~(TOP_OPEN | FUNC_RESET);
	    if (status != E_DB_OK)
	    {
		if (dsh->dsh_error.err_code == E_QE0015_NO_MORE_ROWS)
		{
		    /* If the qen_node returned no rows and we are supposed to
		    ** invalidate this query plan if this action doesn't get
		    ** any rows, then invalidate the plan.
		    */
		    if (dsh->dsh_xaddrs[node->qen_num]->qex_status->node_rcount == 0 &&
		        (action->ahd_flags & QEA_ZEROTUP_CHK) != 0)
		    {
			/* If this is a global temporary table being created
			** as a select, then this temp table must be destroyed
			** before retrying the query.
			*/
			if (qef_rcb->qef_stmt_type & QEF_DGTT_AS_SELECT)
			{
			    DMT_CB	*dmt_cb;
			    i4	error;

			    dmt_cb = (DMT_CB *)cbs[dsh->dsh_dgtt_cb_no];

			    /* Make sure that the table is closed. */
			    status = dmf_call(DMT_CLOSE, dmt_cb);
			    if (status)
			    {
				qef_error(dmt_cb->error.err_code, 0L, status,
					  &error, &dsh->dsh_error, 0);
			    }
			    dmt_cb->dmt_record_access_id = (PTR)NULL;

			    /* Now, delete the table. */
			    dmt_cb->dmt_flags_mask = 0;
			    status = dmf_call(DMT_DELETE_TEMP, dmt_cb);
			    if (status)
			    {
				qef_error(dmt_cb->error.err_code, 0L, status,
					  &error, &dsh->dsh_error, 0);
			    }
			}

			dsh->dsh_qp_status |= DSH_QP_OBSOLETE;
			dsh->dsh_error.err_code = E_QE0023_INVALID_QUERY;
			qef_rcb->qef_intstate |= QEF_ZEROTUP_CHK;
			if (dsh->dsh_qp_ptr->qp_ahd != action)
			{
			    status = E_DB_ERROR;
			}
			else
			{
			    status = E_DB_WARN;
			}
		    }
		    else
		    {
		        status = E_DB_OK;
			dsh->dsh_error.err_code = E_QE0000_OK;
			break;
		    }
		}
		return (status);
	    }
	}

	/* Before running current CX, check for sequences to update. */
	if (action->qhd_obj.qhd_qep.ahd_seq != NULL)
	{
	    status = qea_seqop(action, dsh);
	    if (status != E_DB_OK)
		    return(status);
	}

	/* process the constant expression (MAIN segment) */
	status = qen_execute_cx(dsh, ade_excb);
	if (status != E_DB_OK)
	    return (status);

	/* Check for partitioning and set up DMR_CB. */
	if (action->qhd_obj.qhd_qep.ahd_part_def)
	{
	    status = adt_whichpart_no(dsh->dsh_adf_cb,
			action->qhd_obj.qhd_qep.ahd_part_def, 
			output, &partno);
	    if (status != E_DB_OK)
		    return(status);
		
	    if ((dmr_cb = (DMR_CB *)cbs[action->
			qhd_obj.qhd_qep.ahd_odmr_cb+partno+1]) == NULL ||
		(dmt_cb = (DMT_CB *)cbs[action->
			qhd_obj.qhd_qep.ahd_dmtix+partno+1]) == NULL ||
		dmt_cb->dmt_record_access_id == NULL)
	    {
		status = qeq_part_open(qef_rcb, action, dsh, output,
			dsh->dsh_qp_ptr->qp_row_len[rowno], 
			action->qhd_obj.qhd_qep.ahd_odmr_cb,
			action->qhd_obj.qhd_qep.ahd_dmtix, partno);
			
		if (status != E_DB_OK)
			return(status);

		dmr_cb = (DMR_CB *) cbs[action->
		    qhd_obj.qhd_qep.ahd_odmr_cb+partno+1];
		newpart = TRUE;
	    }
	    else newpart = FALSE;
	}
	if (firstrow || newpart)
	{
	    /* Set up the DMR_CB. */
	    dmr_cb->dmr_flags_mask	    = DMR_TABLE_EXISTS | DMR_EMPTY;
	    dmr_cb->dmr_tid		    = 0;	/* not used */
	    if (node)
		dmr_cb->dmr_s_estimated_records = node->qen_est_tuples;
	    else
		dmr_cb->dmr_s_estimated_records = 0;
	    dmr_cb->dmr_count = 1;
	    dmr_cb->dmr_mdata = &dm_mdata;
	    dmr_cb->dmr_mdata->data_address   = output;
	    dmr_cb->dmr_mdata->data_size   = dsh->dsh_qp_ptr->
					qp_row_len[rowno];
	    dmr_cb->dmr_mdata->data_next = (DM_MDATA *) 0;
	    firstrow = FALSE;
	}
		
	/* perform the append */
	status = dmf_call(DMR_LOAD, dmr_cb);
	if (status != E_DB_OK)
	{
	    /* QEF is not doing anything tricky in this code. 
	    ** Let the DMF error message pass through. 
	    ** Will expand this later if necessary.
	    */
	    dsh->dsh_error.err_code = dmr_cb->error.err_code;
	    return (status);
	}
	j++;

	/* Check "first n" and pretend to be end-of-scan, if warranted. */
	if (first_n > 0 && j >= first_n)
	{
	    status = E_DB_OK;
	    dsh->dsh_error.err_code = E_QE0000_OK;
	    break;
	}
    }
    dsh->dsh_qef_targcount = i;

    /* If this is partitioned table, loop over cbs looking for open 
    ** partitions to finish. */
    if (action->qhd_obj.qhd_qep.ahd_part_def != (DB_PART_DEF *) NULL)
    {
	for (i = action->qhd_obj.qhd_qep.ahd_odmr_cb + 
			action->qhd_obj.qhd_qep.ahd_part_def->nphys_parts;
		i > action->qhd_obj.qhd_qep.ahd_odmr_cb; i--)
	 if ((dmr_cb = (DMR_CB *)cbs[i]) != (DMR_CB *) NULL)
	{
	    dmr_cb->dmr_flags_mask = DMR_TABLE_EXISTS | DMR_ENDLOAD;
	    dmr_cb->dmr_count = 0;
	    status = dmf_call(DMR_LOAD, dmr_cb);
	    if (status != E_DB_OK)
		break;
	}
    }
    else
    {
	dmr_cb->dmr_flags_mask = DMR_TABLE_EXISTS | DMR_ENDLOAD;
	dmr_cb->dmr_count = 0;
	status = dmf_call(DMR_LOAD, dmr_cb);
    }

    if (status != E_DB_OK)
    {
	dsh->dsh_error.err_code = dmr_cb->error.err_code;
	return (status);
    }

    dsh->dsh_qef_rowcount = j;
    return (status);	
}
/*[@function_definition@]...*/
