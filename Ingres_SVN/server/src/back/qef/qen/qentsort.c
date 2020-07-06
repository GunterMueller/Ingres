/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <me.h>
#include    <pc.h>
#include    <st.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ddb.h>
#include    <adf.h>
#include    <ade.h>
#include    <cs.h>
#include    <lk.h>
#include    <scf.h>
#include    <ulf.h>
#include    <ulm.h>
#include    <dmf.h>
#include    <dmtcb.h>
#include    <dmrcb.h>
#include    <qefmain.h>
#include    <qsf.h>
#include    <qefrcb.h>
#include    <qefcb.h>
#include    <qefnode.h>
#include    <psfparse.h>
#include    <qefact.h>
#include    <qefqp.h>
#include    <qefdsh.h>
#include    <qefscb.h>
#include    <tm.h>
#include    <ex.h>
#include    <dmmcb.h>
#include    <dmucb.h>

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
**  Name: QENTSORT.C - top sort node
**
**  Description:
**      The routine in this file loads temporary sort tables 
**
**          qen_tsort	 - load temporary sort table
**	    qen_ts_dump	 - dump the internal sort buffer to DMF sorter.
**
**
**  History:    $Log-for RCS$
**      2-sep-86 (daved)
**          written
**	16-mar-87 (rogerk) Changed call to DMR_LOAD to use dmr_mdata instead
**	    of dmr_data for new multi-row interface.
**	02-nov-87 (puree)
**	    set dmr_s_estimated_records from qen_est_tuples.
**	02-feb-88 (puree)
**	    Added reset flag to qen_tsort function arguments.
**	11-mar-88 (puree)
**	    Remove DSH_QUEUE structure. DMT_CB's can be linked together
**	    using the standard jupiter control block header.
**	29-mar-88 (puree)
**	    Modified QENTSORT, reset dmr_cb before loading the sort file.
**	03-aug-88 (puree)
**	    Fixed error in ult_check_macro.
**	26-sep-88 (puree)
**	    Modified qen_tsort for QEF in-memory sorter.  Implemented 
**	    qen_ts_dump.
**	10-nov-88 (puree)
**	    Fix bug handling error from the outer node.
**	07-jan-91 (davebf)
**	    Modified to support insert in_memory sorter which eliminates
**	    duplicates on insert.
**	11-mar-92 (jhahn)
**	    Added support for QEF interrupts.
**	    Added call to CSswitch.
**	04-may-1992 (bryanp)
**	    Changed dmt_location from a DB_LOC_NAME to a DM_DATA.
**	08-dec-92 (anitap)
**	    Included <qsf.h> and <psfparse.h> for CREATE SCHEMA.
**      23-aug-93 (andys)
**          If there is a commit in the middle of a database procedure,
**          then the holdfile may have been closed. If we then do a
**          dmr_dump_data on the closed file then we fall over. When
**          the commit happens (in qea_commit) then
**          dmt_record_access_id is set no NULL for the DMT_CB.  So,
**          check if this field is NULL, if it is then set status to
**          indicate that hold file should be initialised. [bug 44556]
**          [was 04-aug-92 (andys) in 6.4].
**      19-may-95 (ramra01)
**	    New implementation of heap sort instead of insertion sort for
**          performance gains.
**	7-nov-95 (inkdo01)
**	    Changes to replace QEN_ADF structure instance by pointer in 
**	    QEN_TSORT.
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          In qen_tsort(), set DSH_SORT flag before calling an input function, 
**          and reset it before returning to the caller.
**	17-mar-1999 (somsa01)
**	    Added include of pc.h and lk.h to resolve references to locking
**	    ids.
**  19-feb-99 (kitch01)
**     Bugs 94409 and 95292. In qen_tsort() amend call to qen_ts_dump() to 
**     reorder the heap_sort and rowno parms to correctly match the function
**     prototype.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      31-Jul-2002 (hanal04) Bug 107132 INGSRV 1692
**          Resolve SIGSEGV in dmr_get() when qen_tsort() performs
**          a DMF (DMSE) sort.
**	23-feb-04 (inkdo01)
**	    Changed qef_rcb->error to dsh->dsh_error for || thread safety.
**      28-apr-04 (stial01)
**          qen_ts_dump() Specify duplicate rows allowed 
**	28-Dec-2005 (kschendel)
**	    Use DSH references whenever possible instead of QEF RCB
**	    references.  (This simplifies the code, eliminating many
**	    RCB references, and reflects the new primacy of the DSH
**	    thanks to the parallel query changes.)
**/

/*	static functions	*/

static	DB_STATUS
qen_ts_dump(
QEN_SHD		   *shd,
QEE_DSH		   *dsh,
QEN_NODE           *node,
i4		   rowno,
bool		   heap_sort,
bool		   no_qef );




/*{
** Name: QEN_TSORT	- load temporary sort table
**
** Description:
**      This routine is used to read tuples from the outer node and
**	load them into the sort table. Once the tuples are sorted, this 
**	routine returns them in sorted order.
**
**	The first time this routine is called, the following events take place.
**	    1) read tuples from below
**	    2) project tuples on the sort attributes
**	    3) append tuples to sort relation 
**	    4) the first tuple is read.
**
**  On subsequent calls, a tuple is read.
**  
**
** Inputs:
**      node                            the sort node data structure
**	qef_rcb				request block
**	    .qef_cb.dsh		the data segment header
**
** Outputs:
**      qef_rcb
**	    .error.err_code		one of the following
**					E_QE0000_OK
**					E_QE0019_NON_INTERNAL_FAIL
**					E_QE0002_INTERNAL_ERROR
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
**      2-sep-86 (daved)
**          written
**	16-mar-87 (rogerk) Changed call to DMR_LOAD to use dmr_mdata instead
**	    of dmr_data for new multi-row interface.
**	28-sep-87 (rogerk)
**	    Zero dmr_s_estimated_records before call to dmr_load.
**	02-nov-87 (puree)
**	    set dmr_s_estimated_records from qen_est_tuples.
**	02-feb-88 (puree)
**	    Added reset flag to the function arguments.
**	11-mar-88 (puree)
**	    Removed DSH_QUEUE structure. DMT_CB's can be linked together
**	    using the standard jupiter control block header.
**	29-mar-88 (puree)
**	    Reset dmr_load before loading the sort file whether it is the
**	    initial call or a resetting call.
**	03-aug-88 (puree)
**	    Fixed error in ult_check_macro.
**	26-sep-88 (puree)
**	    Implement QEF in-memory sorter.
**	10-nov-88 (puree)
**	    Fix bug handling error from the outer node.
**	28-sept-89 (eric)
**	    Incremented qen_status->node_rcount to keep track of how many
**	    rows this node returns. This will be used to fix the Sybase bug.
**	07-jan-91 (davebf)
**	    Modified to support insert in_memory sorter which eliminates
**	    duplicates on insert.
**	11-mar-92 (jhahn)
**	    Added support for QEF interrupts.
**	    Added call to CSswitch.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**      23-aug-93 (andys)
**          If there is a commit in the middle of a database procedure,
**          then the holdfile may have been closed. If we then do a
**          dmr_dump_data on the closed file then we fall over. When
**          the commit happens (in qea_commit) then
**          dmt_record_access_id is set no NULL for the DMT_CB.  So,
**          check if this field is NULL, if it is then set status to
**          indicate that hold file should be initialised. [bug 44556]
**          [was 04-aug-92 (andys) in 6.4].
**	14-dec-93 (rickh)
**	    Call qen_u33_resetSortFile when releasing SHD memory.
**	7-nov-95 (inkdo01)
**	    Changes to replace QEN_ADF structure instance by pointer in 
**	    QEN_TSORT.
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          Set DSH_SORT flag before calling an input function, and reset it
**          before returning to the caller of this routine.
**      18-jul-97 (inkdo01)
**          Add support for tracepoint QE92 which uses the old insert sort
**          for duplicates removing sorts. A very small number of such sorts
**          (those with very large numbers of dups) may perform better with
**          insert than heap).
**	10-dec-97 (inkdo01)
**	    Changes to support overhaul of QEF heap sort. Now uses qes_putheap/
**	    qes_getheap to access sort.
**	19-dec-97 (inkdo01)
**	    Changes to avoid the copying of sorted rows to temp table, prior 
**	    to materializing back to tsort.
**  19-feb-99 (kitch01)
**     Bugs 94409 and 95292.
**     Amend call to qen_ts_dump() to reorder the heap_sort and rowno parms
**     to correctly match the function prototype.
**      31-Jul-2002 (hanal04) Bug 107132 INGSRV 1692
**          If dmse_end() has been called in dmr_get() we will SIGSEGV
**          unless we have started a new DMSE sort. To do this we
**          revert back to a QEF sort which will initiate a DMF (DMSE)
**          sort if it runs out of qef_sort_mem.
**          This revealed a further problem. dmr_dump_data() does not
**          dump data from a DMSE sort and the wrong number of rows may
**          be returned to the caller. To resolve this problem all
**          'reset' calls that follow a DMF sort now end the DMSE sort
**          unless the previous get loop hit eof (in which case dmr_get()
**          would have ended the DMSE sort).
**          This lead to a SIGSEGV in dmt_close() because qen_ts_dump()
**          adds the same dmt_cb to the dsh_open_dmtcbs list and creates
**          a loop in the q_prev chain. This was resolved by closing the
**          dmt_cb and removing it from the dsh_open_dmtcbs list with
**          each 'reset' call that followed a DMF sort.
**	20-nov-03 (inkdo01)
**	    Add support for "open" call to allow parallel query initialization.
**	29-dec-03 (inkdo01)
**	    DSH is now parameter, "function" replaces "reset".
**	2-jan-04 (inkdo01)
**	    Added end-of-partition-grouping logic.
**	27-feb-04 (inkdo01)
**	    Changed dsh ptr to arrays of structs (SHD, STATUS) to
**	    ptrs to arrays of ptrs.
**	17-mar-04 (toumi01)
**	    Add support for "close" call to allow parallel query termination.
**	8-apr-04 (inkdo01)
**	    Added dsh parm to QEF_CHECK_FOR_INTERRUPT macro.
**	28-apr-04 (inkdo01)
**	    Add support for "end of partition group" call.
**	27-Jul-2004 (schka24)
**	    Don't need to pass eof flag to DMF when resetting sort file, DMF
**	    will decide whether it needs to end an in-progress sort itself.
**	04-Nov-2004 (jenjo02)
**	    Only CSswitch on root DSH or if not OS-threaded.
**	6-Dec-2005 (kschendel)
**	    Let opc build the sort compare list.
**	13-Jan-2006 (kschendel)
**	    Revise CX evaluation calls.
**	16-Feb-2006 (kschendel)
**	    Return after close, don't keep going.
**	13-Feb-2007 (kschendel)
**	    Replace CSswitch with better cancel check.
**	1-feb-2008 (dougi)
**	    Add qe94 to disable QEF sort altogether.
*/

DB_STATUS
qen_tsort(
QEN_NODE           *node,
QEF_RCB            *qef_rcb,
QEE_DSH		   *dsh,
i4		    function )
{
    GLOBALREF QEF_S_CB	*Qef_s_cb;
    QEF_CB	*qef_cb = dsh->dsh_qefcb;
    PTR		*cbs = dsh->dsh_cbs;
    ADE_EXCB	*ade_excb;
    DMR_CB	*dmr_load = (DMR_CB*)cbs[node->node_qen.qen_tsort.tsort_load];
    DMR_CB	*dmr_get = (DMR_CB*) cbs[node->node_qen.qen_tsort.tsort_get];
    QEN_NODE	*out_node = node->node_qen.qen_tsort.tsort_out;
    QEE_XADDRS	*node_xaddrs = dsh->dsh_xaddrs[node->qen_num];
    QEN_STATUS	*qen_status = node_xaddrs->qex_status;
    QEN_SHD	*shd = dsh->dsh_shd[node->node_qen.qen_tsort.tsort_shd];
    DB_STATUS	status;
    bool	reset = ((function & FUNC_RESET) != 0);
    bool	out_reset = reset;
    bool	heap_sort = TRUE, no_qef = FALSE;
    i4		rowno;
    DM_MDATA	dm_mdata;
    ULM_RCB	ulm_rcb;    
    i4	val1 = 0;
    i4	val2 = 0;
    TIMERSTAT	timerstat;
    DMT_CB	*dmt_cb;

    /* Do open processing, if required. Only if this is the root node
    ** of the query tree do we continue executing the function. */
    if ((function & TOP_OPEN || function & MID_OPEN) && 
	!(qen_status->node_status_flags & QEN1_NODE_OPEN))
    {
	status = (*out_node->qen_func)(out_node, qef_rcb, dsh, MID_OPEN);
	qen_status->node_status_flags |= QEN1_NODE_OPEN;
	if (function & MID_OPEN)
	    return(E_DB_OK);
	function &= ~TOP_OPEN;
    }
    /* Do close processing, if required. */
    if (function & FUNC_CLOSE)
    {
	if (!(qen_status->node_status_flags & QEN8_NODE_CLOSED))
	{
	    status = (*out_node->qen_func)(out_node, qef_rcb, dsh, FUNC_CLOSE);
	    qen_status->node_status_flags = 
		(qen_status->node_status_flags & ~QEN1_NODE_OPEN) | QEN8_NODE_CLOSED;
	}
	return(E_DB_OK);
    }
    /* End of partition group call just gets passed down. */
    if (function & FUNC_EOGROUP)
    {
	status = (*out_node->qen_func)(out_node, qef_rcb, dsh, FUNC_EOGROUP);
	return(E_DB_OK);
    }


    /* Check for cancel, context switch if not MT */
    CScancelCheck(dsh->dsh_sid);
    if (QEF_CHECK_FOR_INTERRUPT(qef_cb, dsh) == E_DB_ERROR)
	return (E_DB_ERROR);

    /* If the trace point qe90 is turned on then gather cpu and dio stats */
    if (ult_check_macro(&qef_cb->qef_trace, 90, &val1, &val2))
    {
	status = qen_bcost_begin(dsh, &timerstat, qen_status);
	if (DB_FAILURE_MACRO(status))
	{
	    return(status);
	}
    }

    if (node->node_qen.qen_tsort.tsort_dups == DMR_NODUPLICATES &&
        ult_check_macro(&qef_cb->qef_trace, 92, &val1, &val2))
            heap_sort = FALSE;

    if (ult_check_macro(&qef_cb->qef_trace, 94, &val1, &val2))
    {
	no_qef = TRUE;
	qen_status->node_u.node_sort.node_sort_status = QEN9_DMF_SORT;
    }

#ifdef xDEBUG
    (VOID) qe2_chk_qp(dsh);
#endif

    for (;;)	    /* to break off in case of error */
    {
loop_reset:
	if (reset && qen_status->node_status != QEN0_INITIAL)
	{
	    /* Dump old data in the sort file:
	    ** For QEF in-memory sort, Close the sort buffer memory stream */
	    if (qen_status->node_u.node_sort.node_sort_status == QEN0_QEF_SORT)
	    {
		status = qen_u33_resetSortFile( shd, dsh );
		if (status != E_DB_OK)	{ break; }

		qen_status->node_status = QEN0_INITIAL;
	    }	

	    /* For DMF sort, call DMF to dump data in the sorter. */
	    else 
	    {
		dmt_cb = (DMT_CB *)cbs[node->node_qen.qen_tsort.tsort_create];
		if (dmt_cb->dmt_record_access_id == (PTR)NULL)
		{
		    /*
		    ** A commit has closed the sort hold file. It should
		    ** be initialised.
		    */
		    status = E_DB_OK ;
	            qen_status->node_status = QEN0_INITIAL;
		}
		else
		{
		    /* Pitch out contents of sort temp */
		    dmr_load->dmr_flags_mask = 0;
		    status = dmf_call(DMR_DUMP_DATA, dmr_load);
		    if (status != E_DB_OK)
		    {
		        dsh->dsh_error.err_code = dmr_load->error.err_code;
		        break;
		    }
		    dmr_load->dmr_flags_mask = (node->node_qen.qen_tsort.tsort_dups |
							DMR_SORT_NOCOPY);
		    dmr_load->dmr_count = 1;  /* reset for subsequent loads */

		    /* Close the table and remove it from the
		    ** dsh_open_dmtcbs list in order to avoid duplicate dmt_cb
		    ** entries in the dsh_open_dmtcbs list when we
		    ** next call qen_ts_dump(). This prevents a SIGSEGV
		    ** in dmt_close() during qeq_cleanup. Poached this code
		    ** from fix for bug 103845.
		    */
		    dmt_cb->dmt_flags_mask = 0;
		    status = dmf_call(DMT_CLOSE, dmt_cb);
		    if (status)
		    {
			dsh->dsh_error.err_code = dmt_cb->error.err_code;
		        return(status);
                    }  

		    status = dmf_call(DMT_DELETE_TEMP, dmt_cb);
		    if (status)
		    {
			dsh->dsh_error.err_code = dmt_cb->error.err_code;
			return(status);
                    }

                    /*
                    ** if the dmtcb is the only one on the dsh queue,
                    ** then set queue to null.
                    ** if the dmtcb is currently the one pointed to by
                    ** the dsh->dsh_open_dmtcbs then set next dmtcb as
                    ** the dsh->dsh_open_dmtcbs.
                    ** In any case, need to remove the dmtcb from the
                    ** dmtcb queue.
                    */
                    if (dsh->dsh_open_dmtcbs == dmt_cb)
                        dsh->dsh_open_dmtcbs = (DMT_CB *)dmt_cb->q_next;
 
                    /* remove dmtcb from queue */
                    if (dmt_cb->q_next)
                        ((DMT_CB *)(dmt_cb->q_next))->q_prev = dmt_cb->q_prev;
                    else
                        ((DMT_CB *)(dmt_cb->q_prev))->q_next = (PTR)NULL;
 
                    if (dmt_cb->q_prev)
                        ((DMT_CB *)(dmt_cb->q_prev))->q_next = dmt_cb->q_next;
                    else
                        ((DMT_CB *)(dmt_cb->q_next))->q_prev = NULL;
                    dmt_cb->q_next = dmt_cb->q_prev = (PTR)NULL;
 
		    /* Having ended the dmse_sort the existing code
		    ** for starting a new one on a reset assumes we
		    ** would be in a QEF sort that ran out of memory.
		    ** Switch back to an inital state with QEF sort.
		    ** some of the following sorts actually run to
		    ** completion under QEF sort anyway.
		    */
                    qen_status->node_status = QEN0_INITIAL;
                    qen_status->node_u.node_sort.node_sort_status = QEN0_QEF_SORT;
		    dmt_cb->dmt_record_access_id = (PTR)NULL;
		}
	    }
	}

	/* If NO MORE ROWS from this node, just return */
	if (qen_status->node_status == QEN4_NO_MORE_ROWS)
	{
	    dsh->dsh_error.err_code = E_QE0015_NO_MORE_ROWS;
	    status = E_DB_WARN;
	    break;
	}

	rowno = node->node_qen.qen_tsort.tsort_mat->qen_output;
	ade_excb = node_xaddrs->qex_otmat;

	/* If this is the first time execution, or if the node is reset, 
	** initialize the sorter. If this is not, just go get a tuple.
	*/
	if (qen_status->node_status == QEN0_INITIAL || 
	    qen_status->node_status == QEN1_EXECUTED)
	{
	    if (qen_status->node_status == QEN0_INITIAL)
	    {
		if (qen_status->node_u.node_sort.node_sort_status == QEN0_QEF_SORT)
		{
		    status = qes_init(dsh, shd, node, rowno,
				node->node_qen.qen_tsort.tsort_dups);
		    if (status != E_DB_OK)
		    {
			if (dsh->dsh_error.err_code == E_QE000D_NO_MEMORY_LEFT)
			{
			    /* out of space, convert it to DMF sort */
			    qen_status->node_u.node_sort.node_sort_status = QEN9_DMF_SORT;
			    status = E_DB_OK;
			}
			else
			{
			    break;
			}
		    }
		}

		if (qen_status->node_u.node_sort.node_sort_status == QEN9_DMF_SORT)
		{
		    status = qen_ts_dump(shd, dsh, node, 
					rowno, heap_sort, no_qef);
		    if (status != E_DB_OK)
			break;
		}
		qen_status->node_status = QEN1_EXECUTED;
	    }

	    /* Common code for QEN0_INITIAL and QEN1_EXECUTED */

	    /* Get dm_mdata ready for DMF loading  */
	    dm_mdata.data_address = dsh->dsh_row[rowno];
	    dm_mdata.data_size = dsh->dsh_qp_ptr->qp_row_len[rowno];
	    dmr_load->dmr_mdata = &dm_mdata;

            dsh->dsh_qp_status |= DSH_SORT;

	    /* Get rows from the underneath node and append them to the 
	    ** sorter */
	    for (;;)
	    {
		/* fetch rows */
		status = (*out_node->qen_func)(out_node, qef_rcb, dsh,
				(out_reset) ? FUNC_RESET : NO_FUNC);
		out_reset = FALSE;
		if (status != E_DB_OK)
		{
		    /* the error.err_code field in qef_rcb should already be
		    ** filled in.
		    */
		    if (dsh->dsh_error.err_code == E_QE0015_NO_MORE_ROWS)
		    {
			status = E_DB_OK;
		    }
		    else if (dsh->dsh_error.err_code == E_QE00A5_END_OF_PARTITION &&
			(node->qen_flags & QEN_PART_SEPARATE))
		    {
			/* End of rows from partitioning group. Flag the
			** fact and make it look like regular "eof". */
			qen_status->node_status_flags |= QEN2_OPART_END;
			dsh->dsh_error.err_code = E_QE0015_NO_MORE_ROWS;
			status = E_DB_OK;
		    }

		    break;
		}
		
		/* project the attributes into sort tuple row */
		status = qen_execute_cx(dsh, ade_excb);
		if (status != E_DB_OK)
		    break;

                /* append the sort tuple into the sorter - note periodic
                ** differences between heap sort and trace point mandated
                ** insert sort */
		if (qen_status->node_u.node_sort.node_sort_status == QEN0_QEF_SORT)
		{
		    if (heap_sort) status = qes_putheap(dsh, shd, 
                         node->node_qen.qen_tsort.tsort_cmplist,
                         node->node_qen.qen_tsort.tsort_scount); 
                     else status = qes_insert(dsh, shd,
                         node->node_qen.qen_tsort.tsort_cmplist,
                         node->node_qen.qen_tsort.tsort_scount);

		    if (status != E_DB_OK && 
			dsh->dsh_error.err_code == E_QE000D_NO_MEMORY_LEFT)
		    {
			/* out of space, convert it to DMF sort */
			qen_status->node_u.node_sort.node_sort_status = QEN9_DMF_SORT;
			status = qen_ts_dump(shd, dsh, 
					node, rowno, heap_sort, no_qef);
		    }
		    if (status != E_DB_OK)
			break;
		}

		if(qen_status->node_u.node_sort.node_sort_status == QEN9_DMF_SORT)  
		{
		    status = dmf_call(DMR_LOAD, dmr_load);
		    if (status != E_DB_OK)
		    {
			dsh->dsh_error.err_code = dmr_load->error.err_code; 
			break;
		    }
		}
	    }
	    if (status != E_DB_OK)
		break;

	    /* End of loading loop */
	    if(qen_status->node_u.node_sort.node_sort_status == QEN0_QEF_SORT)
	    {
		if (!heap_sort) qes_endsort(dsh, shd);
						/* prep return of tups */

	    }
	    else   /* DMF */
	    {
		/* Tell DMF that there are no more rows */
		dmr_load->dmr_flags_mask = (DMR_ENDLOAD | DMR_SORT_NOCOPY);
		status = dmf_call(DMR_LOAD, dmr_load);
		if (status != E_DB_OK)
		{
		    dsh->dsh_error.err_code = dmr_load->error.err_code;
		    break;
		}

		/* position the table for reading sorted tuples */
		dmr_get->dmr_flags_mask = DMR_SORTGET;
		dmr_get->dmr_position_type = DMR_ALL;
		status = dmf_call(DMR_POSITION, dmr_get);
		if (status != E_DB_OK)
		{
		    dsh->dsh_error.err_code = dmr_get->error.err_code;
		    break;
		}
	    }

	    /* Mark the node is ready to return tuples */
	    qen_status->node_status = QEN3_GET_NEXT_INNER;
	}


	/* 
	** Return a tuple from the sorter to the caller.
	*/
	if (qen_status->node_u.node_sort.node_sort_status == QEN0_QEF_SORT)
	{
	    if (heap_sort) 
	    {
		status = qes_getheap(dsh, shd, 
                         node->node_qen.qen_tsort.tsort_cmplist,
                         node->node_qen.qen_tsort.tsort_scount); 
		if (status != E_DB_OK) 
		{
		    if (dsh->dsh_error.err_code == E_QE0015_NO_MORE_ROWS)
		    {
			qen_status->node_status = QEN4_NO_MORE_ROWS;
			status = E_DB_WARN;
		    }
		    break;
		}
	    }
	    else
	    {		/* rows come straight from pointer array */
	 	/* Check for the end of buffer */
		if (shd->shd_next_tup == shd->shd_tup_cnt)
		{
		    qen_status->node_status = QEN4_NO_MORE_ROWS;
		    dsh->dsh_error.err_code = E_QE0015_NO_MORE_ROWS;
		    status = E_DB_WARN;
		    break;
		}

		/* Copy tuple to the row buffer */
		MEcopy((PTR)(shd->shd_vector[shd->shd_next_tup]), 
		    shd->shd_width, (PTR)shd->shd_row);
		++shd->shd_next_tup;
		status = E_DB_OK;
	    }
	}
	else
	{
	    dmr_get->dmr_flags_mask = (DMR_NEXT | DMR_SORTGET);
	    status = dmf_call(DMR_GET, dmr_get);
	    if (status != E_DB_OK)
	    {
		if (dmr_get->error.err_code == E_DM0055_NONEXT)
		{
		    qen_status->node_status = QEN4_NO_MORE_ROWS;
		    dsh->dsh_error.err_code = E_QE0015_NO_MORE_ROWS;
		    status = E_DB_WARN;
		}
		else
		{
		    dsh->dsh_error.err_code = dmr_get->error.err_code;
		}
		break;
	    }
	}


	/* Increment the count of rows that this node has returned */
	qen_status->node_rcount++;
	dsh->dsh_error.err_code = 0;

	/* print tracing information DO NOT xDEBUG THIS */
	if (node->qen_prow &&
		(ult_check_macro(&qef_cb->qef_trace, 100+node->qen_num,
				    &val1, &val2) ||
		    ult_check_macro(&qef_cb->qef_trace, 99,
				    &val1, &val2)
		)
	    )
	{
	    if (status == E_DB_OK)
	    {
		status = qen_print_row(node, qef_rcb, dsh);
		if (status != E_DB_OK)
		{
		    return(status);
		}
	    }
	}

	break;
    }	    /* end of error-break loop */

#ifdef xDEBUG
    (VOID) qe2_chk_qp(dsh);
#endif

    if (ult_check_macro(&qef_cb->qef_trace, 90, &val1, &val2))
    {
	DB_STATUS   cost_status;

	cost_status = qen_ecost_end(dsh, &timerstat, qen_status);
	if (DB_FAILURE_MACRO(cost_status))
	{
	    return(cost_status);
	}
    }

    dsh->dsh_qp_status &= ~DSH_SORT;

    if (dsh->dsh_error.err_code == E_QE0015_NO_MORE_ROWS &&
	(qen_status->node_status_flags & QEN2_OPART_END))
    {
	/* If this was just the end of a partition, reset error code
	** to notify the caller. */
	qen_status->node_status_flags &= ~QEN2_OPART_END;
	dsh->dsh_error.err_code = E_QE00A5_END_OF_PARTITION;
	status = E_DB_WARN;
    }
    return (status);
}

/*
**	04-may-1992 (bryanp)
**	    Changed dmt_location from a DB_LOC_NAME to a DM_DATA.
**	06-mar-1996 (nanpr01)
**	    Get the pagesize from the qen_tsort.tsort_page_size 
**	30-Jul-2004 (jenjo02)
**	    Use dsh_dmt_id rather than qef_dmt_id transaction context.
**	1-feb-2008 (dougi)
**	    Add tp qe94 to disable QEF sort.
*/

static	DB_STATUS
qen_ts_dump(
QEN_SHD		   *shd,
QEE_DSH		   *dsh,
QEN_NODE           *node,
i4		   rowno,
bool		   heap_sort,
bool		   no_qef )
{
    PTR		*cbs = dsh->dsh_cbs;
    DMT_CB	*dmt = (DMT_CB *)cbs[node->node_qen.qen_tsort.tsort_create];
    DMR_CB	*dmr_load = (DMR_CB*)cbs[node->node_qen.qen_tsort.tsort_load];
    DMR_CB	*dmr_get = (DMR_CB*) cbs[node->node_qen.qen_tsort.tsort_get];
    DMT_CHAR_ENTRY	char_array[2];
    DB_STATUS	status;


    for (;;)	    /* to break off in case of error */
    {
	/* Initialize the DMT_CB for the sorter table */
	dmt->dmt_flags_mask = DMT_LOAD | DMT_DBMS_REQUEST;
	dmt->dmt_db_id = dsh->dsh_qefcb->qef_rcb->qef_db_id;
	dmt->dmt_tran_id = dsh->dsh_dmt_id;
	MEmove(8, (PTR) "$default", (char) ' ', 
	    sizeof(DB_LOC_NAME),
	    (PTR) dmt->dmt_location.data_address);
	dmt->dmt_location.data_in_size = sizeof(DB_LOC_NAME);


	/* Initialize table attribute descriptors */
	dmt->dmt_attr_array.ptr_address = 
		    (PTR) node->node_qen.qen_tsort.tsort_atts;
	dmt->dmt_attr_array.ptr_in_count = 
		    node->node_qen.qen_tsort.tsort_acount;
	dmt->dmt_attr_array.ptr_size = sizeof (DMT_ATTR_ENTRY);

	/* Initialize the the sort key descriptors */
	dmt->dmt_key_array.ptr_address = 
		(PTR) node->node_qen.qen_tsort.tsort_satts;
	dmt->dmt_key_array.ptr_in_count = 
		node->node_qen.qen_tsort.tsort_scount;
	dmt->dmt_key_array.ptr_size = sizeof (DMT_KEY_ENTRY);

	/*  Pass the page size */
	char_array[0].char_id = DMT_C_PAGE_SIZE;
	char_array[0].char_value = node->node_qen.qen_tsort.tsort_pagesize;
	char_array[1].char_id = DMT_C_DUPLICATES;
	char_array[1].char_value = DMT_C_ON; /* duplicate rows allowed */
	dmt->dmt_char_array.data_address = (PTR) &char_array;
	dmt->dmt_char_array.data_in_size = 2 * sizeof(DMT_CHAR_ENTRY);

	/* Create the sorter table */
	status = dmf_call(DMT_CREATE_TEMP, dmt);
	if (status != E_DB_OK)
	{
	    if (dmt->error.err_code == E_DM0078_TABLE_EXISTS)
	    {
		dsh->dsh_error.err_code = E_QE0050_TEMP_TABLE_EXISTS;
		status = E_DB_ERROR;
	    }
	    else
	    {
		dsh->dsh_error.err_code = dmt->error.err_code;
	    }
	    break;
	}

	/* Open the sorter table */
	dmt->dmt_flags_mask = 0;
	dmt->dmt_sequence = dsh->dsh_stmt_no;
	dmt->dmt_access_mode = DMT_A_WRITE;
	dmt->dmt_lock_mode = DMT_X;
	dmt->dmt_update_mode = DMT_U_DIRECT;
	dmt->dmt_char_array.data_address = 0;
	dmt->dmt_char_array.data_in_size = 0;
	
	status = dmf_call(DMT_OPEN, dmt);
	if (status != E_DB_OK)
	{
	    dsh->dsh_error.err_code = dmt->error.err_code;
	    break;
	}
	/* move the control block into a list of open table in the
	** data segment header (DSH)
	*/
	if (dsh->dsh_open_dmtcbs == (DMT_CB *)NULL)
	{
	    dmt->q_next = dmt->q_prev = (PTR)NULL;
	    dsh->dsh_open_dmtcbs = dmt;	    
	}
	else
	{
	    dsh->dsh_open_dmtcbs->q_prev = (PTR)dmt;
	    dmt->q_next = (PTR)(dsh->dsh_open_dmtcbs);
	    dmt->q_prev = (PTR)NULL;
	    dsh->dsh_open_dmtcbs = dmt;
	}

	/* Initialize the DMR_CB for loading the sorter */
	dmr_load->dmr_access_id = dmt->dmt_record_access_id;
	dmr_load->dmr_count = 1;
	dmr_load->dmr_flags_mask = (node->node_qen.qen_tsort.tsort_dups |
					DMR_SORT_NOCOPY);
	dmr_load->dmr_s_estimated_records = node->qen_est_tuples;
	dmr_load->dmr_tid = 0;

	/* Initialize the DMR_CB for reading the sorter */
	dmr_get->dmr_access_id = dmt->dmt_record_access_id;
	dmr_get->dmr_data.data_address = dsh->dsh_row[rowno];
	dmr_get->dmr_data.data_in_size =  dsh->dsh_qp_ptr->qp_row_len[rowno];
	dmr_get->dmr_tid = 0;

	if (no_qef)
	    break;				/* leave, if nothing there */

	/* If we have tuples in the in-memory sort buffer, load them into
	** DMF sorter table now and write current tuple. */
        if (heap_sort) status = qes_dump(dsh, shd, dmr_load);
	 else status = qes_dumploop(dsh, shd, dmr_load);
	break;
    }	/* end of error-break loop */
    return (status);
}
