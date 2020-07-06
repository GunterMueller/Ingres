/*
** Copyright (c) 2004, Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <st.h>
#include    <pc.h>
#include    <lk.h>
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
#include    <scf.h>
#include    <qefrcb.h>
#include    <qefcb.h>
#include    <qefnode.h>
#include    <psfparse.h>
#include    <qefact.h>
#include    <qefqp.h>
#include    <qefdsh.h>
#include    <qefscb.h>

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
**  Name: QEQPART.C - functions for table partitioning support.
**
**  Description:
**	The functions in this file provide support for QEF management 
**	of table partitions.
**
**	qeq_part_open - allocates/formats/opens DMR_CB/DMT_CB for 
**	a table partition
**
**
**  History:
**	22-jan-04 (inkdo01)
**	    Written for table partitioning.
**	23-feb-04 (inkdo01)
**	    Changed qef_rcb->error to dsh->dsh_error for || thread safety.
**/

    GLOBALREF QEF_S_CB *Qef_s_cb;

/*{
** Name: QEQ_PART_OPEN	- Open a partition of a partitioned table
**
** Description:
**      Allocate/init DMR_CB and DMT_CB, then open DMT_CB for table
**	partition.  If the partition is already open, we do nothing
**	and return success.
**
** Inputs:
**	qef_rcb - rcb for the current query
**	action - current action being executed
**	dsh - dsh for the current query
**
** Outputs:
**
**	Returns:
**	    success or error status.
**	Exceptions:
**	    none
**
** Side Effects:
**	    The partition gets opened.
**
** History:
**      22-jan-04 (inkdo01)
**	    Written for partitioned table support.
**	10-mar-04 (inkdo01)
**	    Copy dmr_attr_desc from master DMR_CB to partition DMR_CB.
**	26-mar-04 (inkdo01)
**	    Use dsh_ahdvalid for locating tables - not all action hdrs
**	    have a valid chain.
**	6-may-04 (inkdo01)
**	    New code to avoid re-allocating DMT/DMR_CBs for repeat qs
**	    with DSHs containing already alloc'ed cbs.
**	19-Jul-2004 (schka24)
**	    Do vl lookup after we decide that open is needed.
**	28-Jul-2004 (jenjo02)
**	    Tweaked the function to also open an instance of a
**	    non-partitioned table on behalf of a Child thread.
**	30-Jul-2004 (jenjo02)
**	    Use dsh_dmt_id rather than qef_dmt_id transaction context.
*/
DB_STATUS
qeq_part_open(
	QEF_RCB	    *qef_rcb, 
	QEF_AHD	    *action, 
	QEE_DSH	    *dsh, 
	PTR	    rowptr,
	i4	    rowsize,
	i4	    dmrix,
	i4	    dmtix,
	u_i2	    partno)
{
    PTR		*cbs = dsh->dsh_cbs;
    DMT_CB	*dmt_cb;
    DMR_CB	*dmr_cb, *masterdmr_cb;
    QEF_VALID	*vl;
    DB_STATUS	status = E_DB_OK;
    QEF_QP_CB	*qp = dsh->dsh_qp_ptr;
    DMT_CHAR_ENTRY  dmt_char_entry[2];
    ULM_RCB	ulm;

    /*
    ** If dmtix == -1, this is a request to open an
    ** instance of a table for a Child thread (called
    ** from qen_orig() when dmr_cb->dmr_access_id has
    ** been set to NULL by qen_dsh().
    **
    ** In this case, look up the passed dmrix in the
    ** query's list of validate tables, then open
    ** an instance of the table, supplying dmr_access_id
    ** and linking the table into the DSH's list of
    ** tables opened by this Child thread.
    **
    ** It's presumed that since the table must have already
    ** been opened earlier, the corresponding DMT_CB has
    ** be initialized to all the proper locking, blah-blah,
    ** values, so we just open it.
    */
    if ( dmtix == -1 )
    {
	for ( vl = dsh->dsh_ahdvalid; vl; vl = vl->vl_next )
	{
	    /* Find this DMR_CB's index, extract DMT_CB */
	    if ( vl->vl_dmr_cb == dmrix )
	    {
		dmr_cb = (DMR_CB*)cbs[vl->vl_dmr_cb];
		dmt_cb = (DMT_CB*)cbs[vl->vl_dmf_cb];
		break;
	    }
	}
	if (vl == (QEF_VALID *) NULL)
	{
	    /* Gotta return some sort of error code */
	    dsh->dsh_qp_status |= DSH_QP_OBSOLETE;
	    dsh->dsh_error.err_code = E_QE0023_INVALID_QUERY;
	    return(E_DB_ERROR);	/* big trouble if no QEF_VALID */
	}
	/* Fall through to open the table... */
    }
    else
    {
	/* Regular "open a partition" call */
	if ( (dmt_cb = (DMT_CB *)cbs[dmtix + partno + 1]) != (DMT_CB *) NULL)
	{
	    if (dmt_cb->dmt_record_access_id)
		return(E_DB_OK);	/* already open - nothing to do */
	    dmr_cb = (DMR_CB *)dsh->dsh_cbs[dmrix + partno + 1];
	}
	else
	{
	    /* Not already allocated, set up for memory allocation. */
	    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
	    ulm.ulm_streamid_p = &dsh->dsh_streamid;
	    
	    ulm.ulm_psize = sizeof(DMT_CB) + sizeof(DMR_CB) + sizeof(DB_LOC_NAME);
	    status = qec_malloc(&ulm);
	    if (status != E_DB_OK)
	    {
		dsh->dsh_error.err_code = ulm.ulm_error.err_code;
		return (status);
	    }
	    dmt_cb 	= (DMT_CB*) ((char *) ulm.ulm_pptr); 
	    dsh->dsh_cbs[dmtix+partno+1] = (PTR) dmt_cb;
	    dmr_cb = (DMR_CB *) &ulm.ulm_pptr[sizeof(DMT_CB) + sizeof(DB_LOC_NAME)];
	    dsh->dsh_cbs[dmrix+partno+1] = (PTR) dmr_cb;

	    /* Address the single location entry. */
	    dmt_cb->dmt_location.data_address = &ulm.ulm_pptr[sizeof(DMT_CB)];
	}

	masterdmr_cb = (DMR_CB *) dsh->dsh_cbs[dmrix-1];
			    /* load master from stowed address */

	/* Locate QEF_VALID structure for this table (for partition 
	** db_tab_id values and later DMT_CB manipulation). */
	for (vl = dsh->dsh_ahdvalid; vl; vl = vl->vl_next)
	    if (vl->vl_dmr_cb == dmrix && vl->vl_dmf_cb == dmtix)
		break;

	if (vl == (QEF_VALID *) NULL)
	{
	    dsh->dsh_qp_status |= DSH_QP_OBSOLETE;
	    dsh->dsh_error.err_code = E_QE0023_INVALID_QUERY;
	    return(E_DB_ERROR);	/* big trouble if no QEF_VALID */
	}

	/* Initialize and open. */
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
	STRUCT_ASSIGN_MACRO(vl->vl_part_tab_id[partno], dmt_cb->dmt_id);
	dmt_cb->dmt_flags_mask = 0;
	MEfill(sizeof(DB_OWN_NAME), (u_char)' ',
	    (PTR)&dmt_cb->dmt_owner);
	
	dmt_cb->dmt_char_array.data_address = 0;
	dmt_cb->dmt_char_array.data_in_size = 0;
	dmt_cb->dmt_mustlock = vl->vl_mustlock;
	if (action->ahd_flags & QEA_DIR)
	    dmt_cb->dmt_update_mode = DMT_U_DIRECT;
	else
	    dmt_cb->dmt_update_mode = DMT_U_DEFERRED;

	/* Initialize the DMR_CB */
	dmr_cb->q_next = (PTR)NULL;
	dmr_cb->q_prev = (PTR)NULL;
	dmr_cb->length = sizeof (DMR_CB);
	dmr_cb->type = (i2)DMR_RECORD_CB;
	dmr_cb->owner = (PTR)DB_QEF_ID;
	dmr_cb->ascii_id = DMR_ASCII_ID;
	dmr_cb->dmr_access_id = (PTR)NULL;

	/* Copy key info from master. */
	STRUCT_ASSIGN_MACRO(masterdmr_cb->dmr_attr_desc, 
				    dmr_cb->dmr_attr_desc);

	/* Now set up/open the DMT_CB. */
	dmt_cb->dmt_db_id = qef_rcb->qef_db_id;
	dmt_cb->dmt_sequence = dsh->dsh_stmt_no;
	dmt_cb->dmt_mustlock = vl->vl_mustlock;

	if (qef_rcb->qef_qacc == QEF_READONLY)
	    dmt_cb->dmt_access_mode = DMT_A_READ;
	else
	    dmt_cb->dmt_access_mode = vl->vl_rw;

	if ((qp->qp_status & QEQP_DEFERRED) != 0)
	    dmt_cb->dmt_update_mode = DMT_U_DEFERRED;
	else
	    dmt_cb->dmt_update_mode = DMT_U_DIRECT;

	/* default for lock granularity setting purposes */
	switch (dmt_cb->dmt_access_mode)
	{
	    case DMT_A_READ:
	    case DMT_A_RONLY:
		dmt_cb->dmt_lock_mode = DMT_IS;
		break;

	    case DMT_A_WRITE:
		dmt_cb->dmt_lock_mode = DMT_IX;
		break;
	}

	/*
	** If executing a procedure that supports an integrity constraint, 
	** set DMT_CONSTRAINTS flag in the DMT_CB.
	*/
	if (qef_rcb->qef_intstate & QEF_CONSTR_PROC)
	    dmt_cb->dmt_flags_mask |= DMT_CONSTRAINT;

	if (vl->vl_flags & QEF_SESS_TEMP)
	    dmt_cb->dmt_flags_mask |= DMT_SESSION_TEMP;
    }

    /* This thread's transaction context */
    dmt_cb->dmt_tran_id	= dsh->dsh_dmt_id;

    dmt_char_entry[0].char_id = DMT_C_EST_PAGES;
    dmt_char_entry[0].char_value = vl->vl_est_pages;
    dmt_char_entry[1].char_id = DMT_C_TOTAL_PAGES;
    dmt_char_entry[1].char_value = vl->vl_total_pages;
    dmt_cb->dmt_char_array.data_in_size = 2 * sizeof(DMR_CHAR_ENTRY);
    dmt_cb->dmt_char_array.data_address = (PTR)dmt_char_entry;

    /* clear the qualification function */	
    dmr_cb->dmr_q_fcn = 0;
    dmr_cb->dmr_q_arg = 0;

    /* assign the output row and its size */
    dmr_cb->dmr_data.data_address = rowptr;
    dmr_cb->dmr_data.data_in_size = rowsize;

    /* open and check time stamp for each non-temp table */

    status = dmf_call(DMT_OPEN, dmt_cb);
    if (status && dmt_cb->error.err_code == E_DM004D_LOCK_TIMER_EXPIRED)
    {
	/* Try to do validate with lockmode DMT_N (B103150) */
	dmt_cb->dmt_lock_mode = DMT_N;
	status = dmf_call(DMT_OPEN, dmt_cb);
    }

    /* Reset dmt_char_array for future calls */
    dmt_cb->dmt_char_array.data_address = 0;
    dmt_cb->dmt_char_array.data_in_size = 0;

    /*
    ** Turn off row update bit, since later calls may check for
    ** (in)valid flags_mask
    */
    if (status != E_DB_OK)
    {
	if (dmt_cb->error.err_code == E_DM0054_NONEXISTENT_TABLE ||
	    dmt_cb->error.err_code == E_DM0114_FILE_NOT_FOUND)
	{
	    dsh->dsh_qp_status |= DSH_QP_OBSOLETE;
	    dsh->dsh_error.err_code = E_QE0023_INVALID_QUERY;
	    status = E_DB_WARN;
	}
	else if (dmt_cb->error.err_code 
	      != E_DM0129_SECURITY_CANT_UPDATE)
	    dsh->dsh_error.err_code = dmt_cb->error.err_code;
	return(status);
    }

    /* Prepend the DMT_CB into a list of the opened tables in the
    ** data segment header (DSH)
    */
    dmt_cb->q_prev = (PTR)NULL;
    if (dsh->dsh_open_dmtcbs == (DMT_CB *)NULL)
	dmt_cb->q_next = (PTR)NULL;

    else
    {
	dsh->dsh_open_dmtcbs->q_prev = (PTR)dmt_cb;
	dmt_cb->q_next = (PTR)(dsh->dsh_open_dmtcbs);
    }
    dsh->dsh_open_dmtcbs = dmt_cb;

    /* Consistency check due to common AV */
    if (cbs[vl->vl_dmr_cb] == NULL)
    {
	i4		err;
	char		reason[100];

	status = E_DB_ERROR;
	STprintf(reason, "vl_dmr_cb = %d, cbs[%d] = NULL, ahd_atype = %d",
	     vl->vl_dmr_cb, vl->vl_dmr_cb, action->ahd_atype);
	qef_error(E_QE0105_CORRUPTED_CB, 0L, status, &err,
	      &dsh->dsh_error, 2,
	      sizeof("qeq_validate")-1, "qeq_validate",
	      STlength(reason), reason);
	dsh->dsh_error.err_code = E_QE0025_USER_ERROR;
	return(status);
    }
    /* Set the table record access id into the DMR_CB. */
    dmr_cb->dmr_access_id = dmt_cb->dmt_record_access_id;

    return(status);
}
