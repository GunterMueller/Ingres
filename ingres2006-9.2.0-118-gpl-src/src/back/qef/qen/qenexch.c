/*
** Copyright (c) 2004 Ingres Corporation
** 
**
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <st.h>
#include    <bt.h>
#include    <me.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <gca.h>
#include    <cut.h>
#include    <ddb.h>
#include    <adf.h>
#include    <ade.h>
#include    <scf.h>
#include    <ulf.h>
#include    <ulm.h>
#include    <dmf.h>
#include    <dmtcb.h>
#include    <dmrcb.h>
#include    <dmhcb.h>
#include    <qefmain.h>
#include    <qsf.h>
#include    <qefrcb.h>
#include    <qefcb.h>
#include    <qefnode.h>
#include    <psfparse.h>
#include    <qefact.h>
#include    <qefqp.h>
#include    <qefdsh.h>
#include    <qefexch.h>
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

GLOBALREF QEF_S_CB	*Qef_s_cb;

static char DbgParent [ ] = "EXCH: Parent:";
static char DbgChild  [ ] = "EXCH: Child";
static char MsgParent [ ] = "EXCH ERROR: Parent:";
static char MsgChild  [ ] = "EXCH ERROR: Child";

static char *NodeType[] =
{
"TJOIN","KJOIN","ORIG","FSMJOIN","ISJOIN","CPJOIN","SORT",
"TSORT","QP","SEJOIN","ORIGAGG","HJOIN","EXCHANGE"
};

#define EXCH_NOCOMMAND	0 /* no command */
#define EXCH_TERMINATE	1 /* termintate */
#define EXCH_ITERATE	2 /* iterate query for e.g. func reset */

DB_STATUS qen_exchange_child(SCF_FTX *);

static DB_STATUS qen_dsh(
		i4	threadno,
		EXCH_CB *exch_cb,
		QEE_DSH **newdsh,
		DB_ERROR *error);

/*{
** Name: QEN_EXCHANGE	- exchange node processing
**
** Description:
**	This is the main entry point for exchange node processing.
**
** Inputs:
**
** Outputs:
**					
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**
** History:
**	8-jan-04 (toumi01)
**	    Created.
**	    Created simple passthru version for initial testing.
**	27-feb-04 (inkdo01)
**	    Changed dsh ptr to array of QEN_STATUS structs to ptr to array 
**	    of ptrs.
**	12-mar-04 (toumi01)
**	    First cut at CUT exchange processing.
**	17-mar-04 (toumi01)
**	    Add support for "close" call to allow parallel query termination.
**	8-apr-04 (inkdo01)
**	    Added dsh parm to QEF_CHECK_FOR_INTERRUPT macro.
**	8-apr-04 (toumi01)
**	    Get parallel query with dsh copies working.
**	26-apr-04 (toumi01)
**	    Flush CUT buffers as needed on write for Karl's CUT changes.
**	27-apr-04 (toumi01)
**	    Return err_code and threadno via CUT.
**	28-apr-04 (toumi01)
**	    Support 1:n parallelism.
**	18-may-04 (toumi01)
**	    Enhance trace messages with child number to aid debugging.
**	19-may-04 (toumi01)
**	    Replace the xDEBUG trace compile time option with QE95. This
**	    turns on detailed II_DBMS_LOG logging of the parent<->child
**	    interaction during parallel query processing.
**	15-Jul-2004 (jenjo02)
**	    Moved DSH copy to Kids, attach the Kids then let them
**	    run; we'll synch up to them on the next "function".
**	19-Jul-2004 (schka24)
**	    Use DSH query-runtime stream for exch execution time memory.
**	28-Jul-2004 (jenjo02)
**	    When node is CLOSEd, clear exch_cb->parent_buffer as its
**	    memory stream will be closed, repeat query or not.
**	6-aug-04 (inkdo01)
**	    Add qe99 support for parent thread.
**	10-Aug-2004 (jenjo02)
**	    Cleaned up error handling, TRdisplay's.
**	18-Aug-2004 (jenjo02)
**	    Use DIRECT mode read for CUT data buffer rather than
**	    MOVE mode.
**	    Added trace point QE96 to splat out CUT data buffer
**	    stats (also writ if QE95).
**	09-Sep-2004 (jenjo02)
**	    Added DIRECT mode CUT writes for the kids,
**	    trace point QE97 to turn on CUT tracing, support
**	    GLOBAL base arrays.
**	16-Sep-2004 (schka24)
**	    Set parentage on CUT buffers, for cleanup.
**	16-Sep-2004 (jenjo02)
**	    In some situations, the root DSH proves not to
**	    be thread-safe while the Kids are making copies of it.
**	    Now the Parent makes a copy of the root DSH for 
**	    the Kids to work with.
**	    Tweaked CUT DATA buffer size up to .5mb.
**	04-Nov-2004 (jenjo02)
**	    Only CSswitch on root DSH or if not OS-threaded.
**	07-Jun-2005 (jenjo02)
**	    Tolerate E_DB_WARN caught status from CUT which are,
**	    after all, just the threads talking to each other,
**	    and don't warrant a QE0002.
**	16-Feb-2006 (kschendel)
**	    Return after close, don't keep going.
**	04-Apr-2006 (jenjo02)
**	    Rewrote to use a Data buffer per Child instead
**	    of one Data buffer shared by all the kids, to
**	    eliminate internecine conflicts.
**	27-Jun-2006 (kschendel)
**	    Signal ERROR instead of WARN when something goes wrong.
**	    DB_FAILURE_MACRO does not trap warn, and anyway CUT uses
**	    warn for a bunch of informational returns.
**	13-Feb-2007 (kschendel)
**	    Replace CSswitch with better cancel check.
*/

DB_STATUS
qen_exchange(
QEN_NODE           *node,
QEF_RCB            *rcb,
QEE_DSH		   *dsh,
i4		    function )
{
    QEN_EXCH	*exch = &node->node_qen.qen_exch;
    ADE_EXCB	*ade_excb;
    QEF_CB	*qef_cb = dsh->dsh_qefcb;
    PTR		*cbs = dsh->dsh_cbs;
    QEN_STATUS	*qen_status = dsh->dsh_xaddrs[node->qen_num]->qex_status;
    QEN_NODE	*out_node = exch->exch_out;
    QEF_QP_CB	*qp = dsh->dsh_qp_ptr;
    i4		val1 = 0;
    i4		val2 = 0;
    TIMERSTAT	timerstat;
    EXCH_CB	*exch_cb = (EXCH_CB *) cbs[exch->exch_exchcb];
    BUFF_HDR	*BuffHdr;
    DB_STATUS	status = E_DB_OK;
    DB_ERROR	error;
    SCF_CB	scfcb;
    SCF_FTC	ftc;
    ULM_RCB	ulm_rcb;
    i4		i;
    i4		num_io_cells;
    i4		data_cell_size;
    i4		tuples_per_child;
    i4		cells_per_buffer;
    i4		exch_command;
    PTR		ExchCommand = (PTR)&exch_command;
    i4		NodeNum = node->qen_num;
    char	*OutNode = NodeType[out_node->qen_type];
    CUT_RCB	*CommRcb = &exch_cb->comm_rcb;
    CUT_RCB	*DataRcb;
    CHILD_DATA	*CurChild;
    i4		cut_flags;
    i4		KidsWithNoCells;

/* This is somewhat arbitrary and maybe should be configurable */

/* Max Data buffer size per Child */
#define	MAX_CUT_BUFFER	(SIZE_TYPE)(1024*512)


    /* Do open processing, if required. Only if this is the root node
    ** of the query tree do we continue executing the function. */
    if ((function & TOP_OPEN || function & MID_OPEN) && 
	!(qen_status->node_status_flags & QEN1_NODE_OPEN))
    {
	/*
	 * set up the persistent context block for this exchange
	 */
	CSget_sid(&exch_cb->parent_sid);
	exch_cb->node = node;
	exch_cb->rcb = rcb;
	exch_cb->high_status = E_DB_OK;
	exch_cb->high_err_code = 0;
	exch_cb->cchild_running = 0;
	exch_cb->cut_attached = 0;
	if (ult_check_macro(&qef_cb->qef_trace, 95, &val1, &val2))
	    exch_cb->trace = TRUE;
	else
	    exch_cb->trace = FALSE;
	if (ult_check_macro(&qef_cb->qef_trace, 97, &val1, &val2))
	    exch_cb->cut_trace = TRUE;
	else
	    exch_cb->cut_trace = FALSE;

	/*
	** Allocate an array of thread-safe CHILD_DATA structures
	** and space for a copy of the root DSH,
	** but only if we haven't already allocated it. 
	**
	** If a REPEAT query on the root DSH, child_data will 
	** have been deallocated when dsh_qstreamid is closed.
	** On a non-root (child's) DSH, that stream goes away
	** when the Kid terminates, so we have to reallocate the
	** buffer on each OPEN.
	*/

	if ( dsh != dsh->dsh_root || exch_cb->child_data == NULL )
	{
	    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm_rcb);
	    ulm_rcb.ulm_psize = sizeof(QEE_DSH) +
			    (exch->exch_ccount * sizeof(CHILD_DATA));
	    ulm_rcb.ulm_streamid = dsh->dsh_qstreamid;
	    if (ulm_rcb.ulm_streamid == NULL)
	    {
		/* Open the work stream if we're the first */
		ulm_rcb.ulm_flags = ULM_OPEN_AND_PALLOC;
		status = qec_mopen(&ulm_rcb);	/* Open a PRIVATE stream */
		if (status == E_DB_OK)
		    dsh->dsh_qstreamid = ulm_rcb.ulm_streamid;
	    }
	    else
	    {
		status = qec_malloc(&ulm_rcb);
	    }
	    if (status != E_DB_OK)
	    {
		dsh->dsh_error.err_code = ulm_rcb.ulm_error.err_code;
		return(E_DB_ERROR);
	    }
	    exch_cb->child_data = (CHILD_DATA*)ulm_rcb.ulm_pptr;
	}

	/*
	** Because the root DSH is not thread-safe once we return
	** from this OPEN (in particular, "dsh_act_ptr" has been
	** seen to be zeroed), take a snapshot of it and pass the
	** copy to the Kids, from which they'll make their
	** DSH copy(s).
	*/
	exch_cb->dsh = (char*)exch_cb->child_data + 
			(exch->exch_ccount * sizeof(CHILD_DATA));
	/* Snapshot the root DSH for the Kids */
	MEcopy((PTR)dsh->dsh_root, sizeof(QEE_DSH), exch_cb->dsh);

	if (exch_cb->trace)
	    TRdisplay("%@ %p %2d %s %s Starting %d children for function %v\n",
		    exch_cb->parent_sid, NodeNum, OutNode, DbgParent,
		    exch->exch_ccount,
		"MID_OPEN,TOP_OPEN,RESET,ALLOC,NEXTROW,CLOSE,EOGROUP", function);

	/*
	 * if we just did an open, eat the next function if it's a
	 * FUNC_RESET, and only send EXCH_ITERATE to the child to force
	 * a FUNC_RESET call if we later on get this function
	 */
	exch_cb->eat_reset = TRUE;
	
	/*
	 * initialize cut buffer fields
	 */
	data_cell_size = sizeof(BUFF_HDR) +
			 DB_ALIGN_MACRO(qp->qp_row_len[node->qen_row]);

	/*
	** Try for CUT data buffers large enough to contain
	** an evenly distributed number of estimated
	** tuples per child thread, but don't let it  
	** exceed MAX_CUT_BUFFER.
	**
	** Note that there's an implied reliance on "good"
	** tuple estimates, though in practice they're
	** often below actual row production and cause
	** read/write waits on the CUT buffer.
	**
	** Note that CUT sets "cut_num_cells" to the
	** actual number it was able to allocate, which may
	** be less that what we wanted if CUT had memory
	** shortage problems.
	*/
	if ( (tuples_per_child = node->qen_est_tuples /
				    exch->exch_ccount) < 16 )
	    tuples_per_child = 16;

	if ( (SIZE_TYPE)(tuples_per_child * data_cell_size)
				> MAX_CUT_BUFFER )
	{
	    cells_per_buffer = MAX_CUT_BUFFER / data_cell_size;
	}
	else
	{
	    /*
	    ** They (may) all fit. We add one more in 
	    ** which the Child will place its "no more rows"
	    ** final status.
	    */
	    cells_per_buffer = tuples_per_child + 1;
	}

	/* One COMM cell for all Child threads */
	STcopy("CUT exchange COMM buffer", CommRcb->cut_buf_name);
	CommRcb->cut_cell_size = sizeof(exch_command);
	CommRcb->cut_num_cells = 1;
	CommRcb->cut_buf_use = CUT_BUF_WRITE | CUT_BUF_PARENT;

	/*
	 * create Comm CUT buffer
	 */
	status = cut_create_buf(1, &CommRcb, &error);
	if (status != E_DB_OK)
	{
	    /* Error already reported by CUT */
	    dsh->dsh_error.err_code = error.err_code;
	    if ( DB_FAILURE_MACRO(status) )
		dsh->dsh_error.err_code = E_QE0002_INTERNAL_ERROR;
	    return(status);
	}

	/* Note that we're involved with CUT now */
	exch_cb->cut_attached = 1;

	/*
	 * set up for factotum child launch
	 */
	scfcb.scf_type = SCF_CB_TYPE;	/* init SCF control blocks */
	scfcb.scf_length = sizeof(SCF_CB);
	scfcb.scf_session = DB_NOSESSION;
	scfcb.scf_facility = DB_QEF_ID;
	scfcb.scf_ptr_union.scf_ftc = &ftc;

	/* Private ADF, DMF, children use parent QEF CB's */
	ftc.ftc_facilities = (1<<DB_ADF_ID) | (1<<DB_DMF_ID) | (1<<DB_SXF_ID);
	ftc.ftc_data_length = 0;	/* i.e. don't copy data */
	ftc.ftc_thread_name = DB_EXCH_THREAD_NAME;
	ftc.ftc_priority = SCF_CURPRIORITY;
	ftc.ftc_thread_entry = qen_exchange_child;
	ftc.ftc_thread_exit = NULL;

	/*
	 * launch the exchange children
	 */

	CurChild = exch_cb->child_data; 

	for ( i = 0 ; i < exch->exch_ccount; i++ )
	{
	    /*
	    ** Each child is passed a thread-safe copy
	    ** of its "thread number".
	    ** This lets us attach each
	    ** kid without waiting for some kind of 
	    ** verification that the kid has started and
	    ** safely extracted its thread number before
	    ** incrementing that value and moving on to
	    ** the next attach.
	    ** 
	    ** Additionally, the kid is handed a pointer
	    ** to the EXCH_CB and its parent-allocated
	    ** CUT data buffer RCB, which it makes a copy
	    ** of and attaches to as a writer.
	    **
	    ** The remaining elements of the child data
	    ** structure are used by the parent and ignored
	    ** by the child.
	    */
	    CurChild->exch_cb = exch_cb;
	    CurChild->threadno = ++exch_cb->cchild_running;
	    CurChild->status = E_DB_OK;
	    CurChild->row_count = 0;
	    CurChild->cells_remaining = 0;
	    CurChild->next_cell = NULL;

	    DataRcb = &CurChild->data_rcb;
	    MEfill(sizeof(CUT_RCB), 0, (PTR)DataRcb);

	    STprintf(DataRcb->cut_buf_name,
			"CUT Data buffer %d",
			CurChild->threadno);
	    DataRcb->cut_cell_size = data_cell_size;
	    DataRcb->cut_num_cells = cells_per_buffer;
	    DataRcb->cut_buf_use = (CUT_BUF_READ | CUT_BUF_PARENT);

	    /* If CUT tracing, tell CUT */
	    if ( exch_cb->cut_trace )
		DataRcb->cut_buf_use |= CUT_BUF_TRACE;

	    /* Create the data buffer for the Child */
	    if ( status = cut_create_buf(1, &DataRcb, &error) )
	    {
		/* Error already reported by CUT */
		exch_cb->cchild_running--;
		dsh->dsh_error.err_code = error.err_code;
		if ( DB_FAILURE_MACRO(status) )
		    dsh->dsh_error.err_code = E_QE0002_INTERNAL_ERROR;
		return(status);
	    }

	    ftc.ftc_data = (PTR)CurChild;

	    if (exch_cb->trace)
	        TRdisplay("%@ %p %2d %s %s attaching child %d\n",
			exch_cb->parent_sid, NodeNum, OutNode, DbgParent, 
			CurChild->threadno);

	    status = scf_call(SCS_ATFACTOT, &scfcb);
	    if (status != E_DB_OK)
	    {
		exch_cb->cchild_running--;
		ule_format(scfcb.scf_error.err_code,
			   (CL_ERR_DESC*)0, ULE_LOG, NULL,
			   (char*)0, 0L, (i4*)0, &error.err_code, 0);
		dsh->dsh_error.err_code = E_QE0002_INTERNAL_ERROR;
		return(E_DB_ERROR);
	    }
	    /* One more thread we need to know about */
	    exch_cb->cut_attached++;
	    /* Next CHILD_DATA */
	    CurChild++;
	}

	/*
	 * flag that we have opened this node
	 */
	qen_status->node_status_flags |= QEN1_NODE_OPEN;

	/*
	 * now we go back so that other query processing can proceed -
	 * the next time we come here we'll not be doing an OPEN and will
	 * be expecting the child to have produced some data
	 */
	if (status || function & MID_OPEN)
	{
	    if (exch_cb->trace)
	    	TRdisplay("%@ %p %2d %s %s MID_OPEN completed, status = %d\n", 
				exch_cb->parent_sid, NodeNum, OutNode, DbgParent, status);
	    return(status);
	}
	if (exch_cb->trace)
	    TRdisplay("%@ %p %2d %s %s TOP_OPEN completed\n", 
			exch_cb->parent_sid, NodeNum, OutNode, DbgParent);
	function &= ~TOP_OPEN;

    }

    /* Do close processing, if required. */
    if (function & FUNC_CLOSE)
    {
	i4	stats = FALSE;
	u_i4	tot_cells, cell_size;
	u_i4	cell_writes, write_waits, cell_reads, read_waits;

	if (qen_status->node_status_flags & QEN8_NODE_CLOSED)
	    return (E_DB_OK);
	if (exch_cb->trace)
	    TRdisplay("%@ %p %2d %s %s called with FUNC_CLOSE and node status = %d\n",
		    exch_cb->parent_sid, NodeNum, OutNode, DbgParent, 
		    qen_status->node_status_flags);
	/*
	** The kids will do their own FUNC_CLOSE for their 
	** portion of the query tree as much of what needs to
	** be done CUT-wise must be done under the Kids'
	** thread, not the main query thread.
	*/

	/* Only if connected to CUT... */
	if ( exch_cb->cut_attached > 0 )
	{
	    /*
	    ** If there are still kids running, they may be waiting
	    ** on a CUT buffer, e.g., trying to write to a full
	    ** DATA buffer, or may not yet have attached.
	    **
	    ** Note that "cchild_running" is decremented only when
	    ** a Kid sends a non-zero status via the CUT DATA
	    ** buffer and the Parent reads it, so its value
	    ** does not necessarily reflect the number of Child
	    ** threads that are still alive or have yet attached
	    ** to CUT..
	    **
	    ** The query's being closed, so they'll have to abandon
	    ** what they're doing and terminate.
	    **
	    ** Signal E_DB_ERROR to the kids.
	    */
	    if ( exch_cb->cchild_running > 0 )
	    {
		/*
		** First check that everyone has at least attached
		** to CUT, waiting if there are laggards.
		*/
		if (exch_cb->trace)
		    TRdisplay("%@ %p %2d %s %s checking %d CUT attaches\n", 
				exch_cb->parent_sid, NodeNum, OutNode, DbgParent,
				exch_cb->cut_attached);
		do
		{
		    status = cut_event_wait(CommRcb, CUT_HAVE_ATTACHED, 
				(PTR)&exch_cb->cut_attached, &error);
		} while ( status == E_DB_ERROR && error.err_code == E_CU0204_ASYNC_STATUS );

		if (exch_cb->trace)
		    TRdisplay("%@ %p %2d %s %s signalling E_DB_ERROR status\n", 
				exch_cb->parent_sid, NodeNum, OutNode, DbgParent);
		(VOID)cut_signal_status(CommRcb, E_DB_ERROR, &error);
	    }

	    /* 
	    ** If iterable query, then the kids may be waiting on
	    ** a CUT read for the Parent to tell it what to do,
	    ** ITERATE or TERMINATE.
	    ** Since we're closing, tell the kids to TERMINATE.
	    */
	    if ( node->qen_flags & QEN_PQ_RESET )
	    {
		if (exch_cb->trace)
		    TRdisplay("%@ %p %2d %s %s writing EXCH_TERMINATE command\n", 
				exch_cb->parent_sid, NodeNum, OutNode, DbgParent);
		exch_command = EXCH_TERMINATE;
		num_io_cells = 1;
		status = cut_write_buf(&num_io_cells, CommRcb,
		    &ExchCommand, CUT_RW_WAIT|CUT_RW_FLUSH, &error);

		if (status != E_DB_OK)
		    (VOID)cut_signal_status(CommRcb, status, &error);
	    }

	    /* Now wait until we're the last thread attached to CUT */
	    if (exch_cb->trace)
		TRdisplay("%@ %p %2d %s %s cut_event_wait for children to FUNC_CLOSE\n",
			    exch_cb->parent_sid, NodeNum, OutNode, DbgParent);

	    do
	    {
		i = 1;	/* Wait til we're the last one */
		status = cut_event_wait(CommRcb, CUT_DETACH, 
					(PTR)&i, &error);
	    } while ( status == E_DB_ERROR && error.err_code == E_CU0204_ASYNC_STATUS );

	    /* 
	    ** Detach from each Child's Data buffer.
	    */

	    /* Wind up CUT data buffers stats if qe95/qe96/qe97 */
	    if ( exch_cb->trace || exch_cb->cut_trace ||
		ult_check_macro(&qef_cb->qef_trace, 96, &val1, &val2) )
	    {
		stats = TRUE;
		/* Clear the counters */
		tot_cells = 0;
		cell_size = 0;
		cell_writes = 0;
		write_waits = 0;
		cell_reads = 0;
		read_waits = 0;
	    }

	    CurChild = exch_cb->child_data; 

	    for ( i = 0; i < exch->exch_ccount; i++ )
	    {
		DataRcb = &CurChild->data_rcb;
	    
		if ( stats )
		{
		    CUT_BUF_INFO	info;

		    /*
		    ** Issue read_complete to CUT to update our read
		    ** stats.
		    */
		    (void) cut_read_complete(DataRcb, &error);

		    status = cut_info(CUT_INFO_BUF, DataRcb,
					(PTR)&info, &error);
		    if ( status == E_DB_OK )
		    {
			cell_size = DataRcb->cut_cell_size;
			tot_cells += DataRcb->cut_num_cells;
			cell_writes += info.cut_cell_writes;
			write_waits += info.cut_write_waits;
			cell_reads += info.cut_cell_reads;
			read_waits += info.cut_read_waits;
		    }
		}

		/* Detach from Data buffer */

		/* This resets CUT status so the detach's will work */
		(void) cut_signal_status(NULL, E_DB_OK, &error);

		/*
		** NB: We can't simply issue a "cut_thread_term" call 
		**     to unhook from CUT; we may be here as a Parent
		**	   on this EXCH node due to an invocation by
		**	   a Child EXCH, and a "thread_term" would remove
		**	   the Child from CUT as well.
		**
		**	   So we must instead explicitly detach from
		**	   the buffers this Parent created.
		*/
		(void) cut_detach_buf(DataRcb, TRUE, &error);

		CurChild++;
	    }

	    if ( stats )
		TRdisplay("%@ %p %2d %s CUT DATA stats: cells=%dx%d thr=%d Ww=(%d,%d) Rw=(%d,%d) Est=%d\n", 
		    exch_cb->parent_sid, NodeNum, OutNode,
		    tot_cells, cell_size,
		    exch->exch_ccount,
		    cell_writes, write_waits,
		    cell_reads, read_waits,
		    node->qen_est_tuples);

	    /* Lastly, detach from Communications buffer */
	    (void) cut_detach_buf(CommRcb, TRUE, &error);

	    /* EXCH is no longer CUT-involved */
	    exch_cb->cut_attached = 0;
	}
	else
	    /* No CUT, assume no Kids either, just close down */
	    status = (*out_node->qen_func)(out_node, rcb, dsh, FUNC_CLOSE);

	/*
	** If a repeat query, qee will close the memory stream
	** the "child_data" was allocated from; if not a
	** repeat query, dsh_qstreamid == dsh_streamid, so
	** in either case, "child_data" will be deallocated,
	** so if we're OPENed again (repeat), make sure we
	** get a new child_data.
	*/
	exch_cb->child_data = NULL;

	qen_status->node_status_flags = 
		(qen_status->node_status_flags & ~QEN1_NODE_OPEN) | QEN8_NODE_CLOSED;

	if (exch_cb->trace)
	    TRdisplay("%@ %p %2d %s %s FUNC_CLOSE completed\n", 
			exch_cb->parent_sid, NodeNum, OutNode, DbgParent);

	return(E_DB_OK);
    }

    /* if we are called for "normal" node processing but are not opened
     * we _could_ do an implicit open, but should probably throw out the
     * query on the grounds that the call is illogical
     */
    if (!(qen_status->node_status_flags & QEN1_NODE_OPEN) ||
          qen_status->node_status_flags & QEN8_NODE_CLOSED )
    {
	dsh->dsh_error.err_code = E_QE0002_INTERNAL_ERROR;
	return(E_DB_ERROR);
    }

    /*
     * if this is the first call after open and is a FUNC_RESET, just ignore
     * it, because the child already took care of that call when we started
     * up the exchange processing
     */
    if (exch_cb->eat_reset && (function & FUNC_RESET))
    {
	/* NO_FUNC really means "get the next row" */
	function = NO_FUNC;

	if (exch_cb->trace)
	    TRdisplay("%@ %p %2d %s %s ate FUNC_RESET so now processing NO_FUNC\n",
		exch_cb->parent_sid, NodeNum, OutNode, DbgParent);
    }
    exch_cb->eat_reset = FALSE;

    /* if we have been called with FUNC_RESET we need to wake up the child
     * threads and tell them to keep calling down the tree
     */
    if (function & FUNC_RESET)
    {
	if (exch_cb->trace)
	{
	    TRdisplay("%@ %p %2d %s %s called with FUNC_RESET and node status = %d\n",
		    exch_cb->parent_sid, NodeNum, OutNode, DbgParent, 
		    qen_status->node_status_flags);
	    TRdisplay("%@ %p %2d %s %s writing EXCH_ITERATE command\n", 
			exch_cb->parent_sid, NodeNum, OutNode, DbgParent);
	}

	/* Reset each kid's read information for another iteration */
	CurChild = exch_cb->child_data; 
	for ( i = 0; i < exch->exch_ccount; i++ )
	{
	    CurChild->status = E_DB_OK;
	    CurChild->row_count = 0;
	    CurChild->cells_remaining = 0;
	    CurChild->next_cell = NULL;

	    CurChild++;
	}
	
	exch_command = EXCH_ITERATE;
	num_io_cells = 1;
	status = cut_write_buf(&num_io_cells, CommRcb,
			&ExchCommand, CUT_RW_WAIT|CUT_RW_FLUSH, &error);
	if (status != E_DB_OK)
	{
	    if ( exch_cb->trace )
	    {
		TRdisplay("%@ %p %2d %s %s: cut_signal_status status %d %x :%d\n", 
			    exch_cb->parent_sid, NodeNum, OutNode, DbgParent,
			    status, error.err_code, __LINE__);
	    }
	    (VOID)cut_signal_status(CommRcb, status, &error);
	    /* Error already reported by CUT */
	    dsh->dsh_error.err_code = E_QE0002_INTERNAL_ERROR;
	    return(E_DB_ERROR);
	}
	/* we again need to wait for each of the children to return
	 * a non-zero status
	 */
	exch_cb->cchild_running = exch->exch_ccount;
    }

    /* If the trace point qe90 is turned on then gather cpu and dio stats */
    if (ult_check_macro(&qef_cb->qef_trace, 90, &val1, &val2))
    {
	status = qen_bcost_begin(dsh, &timerstat, qen_status);
	if (DB_FAILURE_MACRO(status))
	{
	    return(status);
	}
    }

    /* Check for cancel, context switch if not MT */
    CScancelCheck(dsh->dsh_sid);
    if (QEF_CHECK_FOR_INTERRUPT(qef_cb, dsh) == E_DB_ERROR)
	return (E_DB_ERROR);

    /*
    ** Row production:
    **
    ** For each (still active) Child thread
    **
    **    o issue direct, nowait cut read of all readable cells
    **	    from its data buffer.
    **	  o if no cells are available, try next Child
    **    o if no Children have readable cells, issue cut
    **	    read with wait. CUT will wake us up when any
    **	    Child produces a row for reading.
    **	  o process (return to QEF) every row in the Child's
    **	    block of rows, one at a time.
    **	  o when those are gone, read from next Child.
    **
    ** When a Child returns a status in a cut cell (usually
    ** "no more rows") it becomes incapable of additional
    ** cell production and is ignored.
    **
    ** When all Children have entered this state, we're done.
    */
    if ( exch_cb->trace && qen_status->node_rcount == 0 )
	TRdisplay("%@ %p %2d %s %s starting NEXTROW and node status = %d\n",
		exch_cb->parent_sid, NodeNum, OutNode, DbgParent, 
		qen_status->node_status_flags);

    cut_flags = CUT_RW_DIRECT;
    KidsWithNoCells = 0;

    for (;;)
    {
	CurChild = &exch_cb->child_data[exch_cb->curr_child];
	DataRcb = &CurChild->data_rcb;

	/*
	** Discard Children who have sent a non-zero status;
	** they're no longer players.
	**
	** Otherwise, "CurChild" is that one which just 
	** returned a row to QEF.
	** 
	** If we've drained its cell block, move on to the
	** next still-writing Child to cut-read and   
	** process its next block of cells.
	*/
	/* If we just processed current Child's last cell... */
	if ( CurChild->status || --CurChild->cells_remaining == 0 )
	{
	    /*
	    ** If Child's last row from cells and we're
	    ** using GLOBAL_BASEARRAY, we must issue
	    ** cut_read_complete to release this block of
	    ** cells for re-use by the writer. We had
	    ** to wait until now to do this because 
	    ** the cells are not reusable until the upper
	    ** layers of QEF have moved the rows from the
	    ** cells.
	    */
	    if ( CurChild->status == E_DB_OK &&
	         CurChild->cells_remaining == 0 &&
	         qp->qp_status & QEQP_GLOBAL_BASEARRAY )
	    {
		(void) cut_read_complete(DataRcb, &error);
	    }
	    CurChild->cells_remaining = 0;

	    /* Move on to next Child */
	    if ( (++exch_cb->curr_child) == exch->exch_ccount )
		exch_cb->curr_child = 0;
	    continue;
	}

	/*
	** The kid may not have attached yet, or has attached
	** but hasn't produced a row yet, or has attached,
	** produced rows, but has none for us now (we've
	** caught up).
	**
	** If any of the kids crash, they'll signal
	** the error and the read will return ASYNC status, so
	** there should be no way we can stall here indefinitely.
	*/

	/*
	** If there are no CUT cells left, or there's been
	** an async signal...
	*/
	if ( CurChild->cells_remaining <= 0 || CommRcb->cut_async_status )
	{
	    /*
	    ** "read" from CUT using DIRECT mode. 
	    **
	    ** CUT will return a pointer to the first readable
	    ** cell, and the number of cells in "cells_remaining".
	    */

	    status = cut_read_buf(&CurChild->cells_remaining,
				    DataRcb,
				    &CurChild->next_cell,
				    cut_flags,
				    &error);

	    /* 
	    ** Bad status could be because of a CUT failure (bad)
	    ** or the receipt of a async status from one of the
	    ** kids because it encountered some unexpected problem.
	    ** In either case, this query is toast.
	    */
	    if (DB_FAILURE_MACRO(status))
	    {
		if ( exch_cb->trace )
		    TRdisplay("%@ %p %2d %s %s cut_read_buf status %d %x\n",
			exch_cb->parent_sid, NodeNum, OutNode, MsgParent, 
			status, error.err_code);

		dsh->dsh_error.err_code = error.err_code;

		/* If not from a kid, cut_read_buf failed */
		if ( error.err_code != E_CU0204_ASYNC_STATUS )
		{
		    /* Error already logged by CUT */
		    
		    dsh->dsh_error.err_code = E_QE0002_INTERNAL_ERROR;
		    
		    if ( exch_cb->trace )
		    {
			TRdisplay("%@ %p %2d %s %s: cut_signal_status status = %d :d\n", 
				    exch_cb->parent_sid, NodeNum, OutNode, DbgParent,
				    status, __LINE__);
		    }
		    (VOID)cut_signal_status(CommRcb, status, &error);
		}
		/*
		** Reset our CUT thread status so we can continue to
		** use other CUT buffers if we're running under a Child.
		*/
		(void) cut_signal_status(NULL, E_DB_OK, &error);
		
		return(status);
	    }
	    else if ( CurChild->cells_remaining == 0 )
	    {
		/* No readable cells from this Child */
		if ( ++KidsWithNoCells == exch_cb->cchild_running )
		{
		    /*
		    ** We've caught up with all of the (remaining) Kids.
		    ** Move to next Child, this time waiting
		    ** until one of them produces something.
		    */
		    cut_flags |= CUT_RW_WAIT;
		    KidsWithNoCells = 0;
		}

		/* Try next Kid */
		if ( (++exch_cb->curr_child) == exch->exch_ccount )
		    exch_cb->curr_child = 0;
		continue;
	    }

	    /* Producing cells, no need to wait */
	    cut_flags &= ~CUT_RW_WAIT;
	    KidsWithNoCells = 0;

	    if ( exch_cb->cut_trace )
		TRdisplay("%@ %p %2d %s %s read %d cells from %d at %x\n",
		    exch_cb->parent_sid, NodeNum, OutNode, DbgParent, 
		    CurChild->threadno,
		    CurChild->cells_remaining,
		    CurChild->next_cell);
	}
	else
	    CurChild->next_cell += DataRcb->cut_cell_size;

	/* Each cell contains a BUFF_HDR + data */
	BuffHdr = (BUFF_HDR*)CurChild->next_cell;

	CurChild->status = BuffHdr->status;
	dsh->dsh_error.err_code = BuffHdr->err_code;

	/*
	* if the status was okay then return the data in the row buffer
	*/
	if ( CurChild->status == E_DB_OK )
	{
	    /*
	    ** If using a GLOBAL base array, we need only
	    ** to plunk a pointer to the row within the
	    ** cell, otherwise we'll need to copy the
	    ** row from the cell to the row buffer.
	    **
	    ** Note that in the GLOBAL case, we can't release
	    ** the cells, because QEF hasn't yet extracted
	    ** the cell data. We do the release at the
	    ** top of this big loop.
	    */
	    if ( qp->qp_status & QEQP_GLOBAL_BASEARRAY )
		dsh->dsh_row[node->qen_row] = 
			(PTR)BuffHdr + sizeof(BUFF_HDR);
	    else
	    {
		MEcopy((PTR)BuffHdr + sizeof(BUFF_HDR),
		    qp->qp_row_len[node->qen_row],
		    dsh->dsh_row[node->qen_row]);

		/*
		** If last cell, release the cells 
		** for re-use by the writer..
		*/
		if ( CurChild->cells_remaining == 1 )
		    (void) cut_read_complete(DataRcb, &error);
	    }

	    /* Count another row for the kid and query */
	    CurChild->row_count++;
	    qen_status->node_rcount++;

	    /* print tracing information DO NOT xDEBUG THIS */
	    if ( node->qen_prow != NULL &&
		(ult_check_macro(&qef_cb->qef_trace, 100+node->qen_num,
				&val1, &val2) ||
		ult_check_macro(&qef_cb->qef_trace, 99,
				&val1, &val2)) )
	    {
		(void)qen_print_row(node, rcb, dsh);
	    }
	    /* Break to return row to QEF */
	    break;
	}
	else
	{
	    /*
	    ** Non-zero BuffHdr->status ends this Child's
	    ** participation.
	    */
	    if ( exch_cb->trace )
		TRdisplay("%@ %p %2d %s %s child%d status=%d err=%x rows %d\n",
		    exch_cb->parent_sid, NodeNum, OutNode, DbgParent, 
		    CurChild->threadno, CurChild->status,
		    dsh->dsh_error.err_code, CurChild->row_count);

	    /* Set the worst status thus far */
	    if (CurChild->status > exch_cb->high_status)
	    {
		exch_cb->high_status = BuffHdr->status;
		exch_cb->high_err_code = BuffHdr->err_code;
	    }
	    
	    /* If last child, return the worst status */
	    if ( --exch_cb->cchild_running == 0 )
	    {
		if (exch_cb->trace)
		    TRdisplay("%@ %p %2d %s %s LAST child: status = %d err_code = %x rows %d\n",
			exch_cb->parent_sid, NodeNum, OutNode, DbgParent,
			exch_cb->high_status, exch_cb->high_err_code,
			qen_status->node_rcount);

		dsh->dsh_error.err_code = exch_cb->high_err_code;
		return(exch_cb->high_status);
	    }
	    /* Done with this Child, loop back to pick another */
	    KidsWithNoCells = 0;
	    continue;
	}
    }

    /* If the trace point qe90 is turned on capture stats snapshot */
    if (ult_check_macro(&qef_cb->qef_trace, 90, &val1, &val2))
    {
	status = qen_ecost_end(dsh, &timerstat, qen_status);
	if (DB_FAILURE_MACRO(status))
	{
	    return(status);
	}
    }

    /*
     * the child status tells us how the query is going and exchange should
     * be transparent to the rest of the query process, so just pass back
     * whatever we got without editorial comment
     */
    return(CurChild->status);

}

/**
**
**  Name: QEN_EXCHANGE_CHILD - parallel query execution child processing
**
**  Description:
**	Execute the tree below the exchange node, storing data in a
**	CUT buffer for asynchronous processing by the parent half of
**	the exchange node.
**
**  History:    
**	8-jan-04 (toumi01)
**	    Created.
**	12-mar-04 (toumi01)
**	    First cut at CUT exchange processing.
**	8-apr-04 (toumi01)
**	    Support dsh copy, FUNC_RESET, etc.
**	15-Jul-2004 (jenjo02)
**	    Kids are now responsible for DSH copy and cleanup,
**	    added EXdeclare to handle the truly awful.
**	    Added shared transaction context.
**	    When the Kid absolutely has nothing left to do,
**	    free up resources and terminate the thread rather
**	    than waiting for a TERMINATE from the Parent.
**	21-Jul-2004 (jenjo02)
**	    qen_dsh() now allocates and returns BUFF_HDR memory,
**	    the materialization buffer is set to the space just
**	    after BUFF_HDR, eliminating the need to copy the
**	    materialized row from ADF to BUFF_HDR.
**	28-Jul-2004 (jenjo02)
**	    In a 1:1 plan, the same QEN_STATUS is shared by
**	    both the parent and kid, so the kid must treat it
**	    as read-only.
**	    If EXdeclare is triggered, don't FUNC_CLOSE down
**	    the tree as the exception may have occurred somewhere
**	    else and we'd like to avoid repeating the same
**	    error over and over.
**	10-Aug-2004 (jenjo02)
**	    Tidied up error handling, TRdisplays.
**	18-Aug-2004 (jenjo02)
**	    Modified to use batched CUT writes rather than
**	    singletons.
**	09-Sep-2004 (jenjo02)
**	    The next logical CUT change, DIRECT mode
**	    writes. Added GLOBAL base array support.
**	20-Sep-2004 (jenjo02)
**	    HASH and HASH AGGREGATE memory is now allocated
**	    from sort memory, not the DSH, and must be freed
**	    when we quit.
**	    Roll QEN_STATUS stats into root DSH after we
**	    close the subtree; our QEN_STATUSs are about to
**	    be deallocated and any QE90 stats will be lost.
**	09-Dec-2004 (jenjo02)
**	    If error occurs outside of CUT buffer (parent
**	    isn't notified), make sure all others are
**	    signalled.
**	14-Feb-2005 (jenjo02)
**	    Attach to DATA buffer -after- COMM buffer; that's
**	    the one the Parent is waiting on.
**	    Added debug tracing stuff when doing cut_signal's.
**	27-May-2005 (schka24)
**	    Hash structures adjust h_used, not s_used.  Don't subtract
**	    hash usage from s_used at cleanup.
**	07-Jun-2005 (jenjo02)
**	    Defer locking up CUT cells until after the first
**	    row is produced, and release them to the parent as
**	    soon as they're all used up.
**	14-Dec-2005 (kschendel)
**	    Use ADF CB pointer in DSH.
**	04-Apr-2006 (jenjo02)
**	    Rewrote to use a Data buffer per Child instead
**	    of one Data buffer shared by all the kids, to
**	    eliminate internecine conflicts.
**/
DB_STATUS
qen_exchange_child(SCF_FTX *ftxcb)
{
    CHILD_DATA	*Child = (CHILD_DATA*)ftxcb->ftx_data;
    i4		my_threadno = Child->threadno;
    EXCH_CB	*exch_cb = Child->exch_cb;
    QEN_NODE	*node = exch_cb->node;
    QEN_EXCH	*exch = &node->node_qen.qen_exch;
    QEF_RCB	*rcb = exch_cb->rcb;
    QEE_DSH	*RootDSH = exch_cb->dsh;/* The Root DSH */
    QEE_DSH	*dsh = NULL;		/* Our scrambled copy of it */
    QEN_STATUS	*QenStatus, *RootStatus;
    QEF_CB	*qef_cb = RootDSH->dsh_qefcb;
    QEF_QP_CB	*qp = RootDSH->dsh_qp_ptr;
    ADE_EXCB	*ade_excb;
    QEN_NODE	*out_node = exch->exch_out;
    ULM_RCB	ulm;
    TIMERSTAT	timerstat;
    DB_STATUS	status;
    DB_STATUS	call_status;
    DB_ERROR	error;
    i4		function;
    i4		num_io_cells;
    i4		exch_command;		/* Next command from Parent */
    PTR		ExchCommand = (PTR)&exch_command;
    i4		val1 = 0;
    i4		val2 = 0;
    CUT_RCB	cut_rcb[2];
    CUT_RCB	*CommRcb = &cut_rcb[0];
    CUT_RCB	*DataRcb = &cut_rcb[1];
    i4		i;
    PTR		tran_id = NULL;		/* Thread's txn context */
    DMT_CB	*dmt_cb;
    EX_CONTEXT	ex_context;
    i4		EXcaught = FALSE;
    CS_SID	my_sid;
    char	*OutNode = NodeType[out_node->qen_type];
    i4		NodeNum = node->qen_num;
    BUFF_HDR	*BuffHdr;
    PTR		*MatRow;
    u_i4	MatRows;

    CSget_sid(&my_sid);

    if (exch_cb->trace)
	TRdisplay("%@ %p %2d %s %s%d: has been started by %p\n", 
		    my_sid, NodeNum, OutNode, DbgChild, my_threadno,
		    ftxcb->ftx_thread_id);

    /* DSH memory is used */
    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);

    if ( EXdeclare(qef_handler, &ex_context) )
    {
	EXcaught = TRUE;
	status = E_DB_ERROR;
	error.err_code = E_QE0002_INTERNAL_ERROR;
    }
    else do
    {
	/* Make thread-safe copies of the CUT RCBs */
	MEcopy((PTR)&exch_cb->comm_rcb,
		    sizeof(CUT_RCB), (PTR)CommRcb);
	MEcopy((PTR)&Child->data_rcb,
		    sizeof(CUT_RCB), (PTR)DataRcb);

	/*
	** Attach to the CUT buffers switching READ for WRITE 
	** and vice versa
	**
	** Note that calling cut_thread_init() is superfluous
	** as cut_attach_buf() will do that under the covers
	** anyway.
	**
	** The attach produces CUT_RCB handles for the thread+buffer
	** connection..
	*/
	DataRcb->cut_buf_use = CUT_BUF_WRITE;
	CommRcb->cut_buf_use = CUT_BUF_READ;

	/* Attach to COMM first, then DATA */
	if ( cut_attach_buf(CommRcb, &error) ||
	     cut_attach_buf(DataRcb, &error) )
	{
	    /* Error already logged by CUT */
	    status = E_DB_ERROR;
	    if ( error.err_code != E_CU0204_ASYNC_STATUS )
		error.err_code = 0;
	    break;
	}

	/* This thread needs to share the transaction context, because
	** LG and LK are going to do things like fetch the current
	** session SCB.  We're us, but the transaction was started by
	** the parent.  One symptom of not doing this right is that we
	** wouldn't see force-abort, or the appearance of lock/log
	** errors.
	**
	** Plus, a unique context is required for any tables/sorts/etc
	** opened by this thread.
	*/
	if ( status = qet_connect(rcb, &tran_id, &error) )
	    break;

	/* Make a thread-copy of the DSH */
	if ( status = qen_dsh(my_threadno, exch_cb, &dsh, &error) )
	{
	    break;
	}

	/* Set transaction context in DSH */
	dsh->dsh_dmt_id = tran_id;

	/* There's always a materializer CX */
	ade_excb = (ADE_EXCB*) dsh->dsh_cbs[exch->exch_mat->qen_pos];
	ade_excb->excb_seg = ADE_SMAIN;

	/*
	** Set the address of the slot where ADE expects the
	** materialization row pointer.
	*/
	if ( qp->qp_status & QEQP_GLOBAL_BASEARRAY )
	{
	    ade_excb->excb_bases[ADE_GLOBALBASE] = (PTR)dsh->dsh_row;
	    MatRow = &dsh->dsh_row[node->qen_row];
	}
	else
	{
	    ade_excb->excb_bases[ADE_GLOBALBASE] = NULL;
	    MatRow = &ade_excb->excb_bases[9];
	}

	/*
	 * do open processing for the rest of the query tree
	 */
	if (exch_cb->trace)
	    TRdisplay("%@ %p %2d %s %s%d: doing MID_OPEN\n",
		    my_sid, NodeNum, OutNode, DbgChild, my_threadno);

	status = (*out_node->qen_func)(out_node, rcb, dsh, MID_OPEN);

	if (status != E_DB_OK)
	{
	    error.err_code = dsh->dsh_error.err_code;
	    /* Force CUT signal status notification */
	    dsh->dsh_error.err_code = 0;
	    break;
	}
	if (exch_cb->trace)
	    TRdisplay("%@ %p %2d %s %s%d: MID_OPEN completed\n",
		    my_sid, NodeNum, OutNode, DbgChild, my_threadno);

	/*
	** At the Child EXCH level, we don't care about CPU and DIO
	** (QE90) stats. Our QEN_STATUS will get tossed when the
	** Child exits, hence will never appear in stats; the
	** Parent however, accumulates stats on its QEN_STATUS,
	** which persists and will get reported.
	** As for the nodes underneath the Child, we'll roll their
	** stats into the root DSH's QEN_STATUS's where they'll be
	** visible for reporting.
	*/

	/* After the MID_OPEN, we do a FUNC_RESET */
	function = FUNC_RESET;
	MatRows = 0;

	for (;;)
	{
	    /*
	    ** Materialize rows directly into the CUT cells.
	    ** Monitor cut_async_status while in the loop
	    ** to be on guard for interrupts.
	    */
	    call_status = E_DB_OK;
	    
	    while ( call_status == E_DB_OK && status == E_DB_OK )
	    {
		/*
		**    Throw single cells at the Parent until EOF
		**    or until we catch up to it:
		**
		**    o Get a single cell from CUT
		**    o Produce a row
		**      If more rows, materialize the row
		**	into the CUT buffer, loop.
		**
		**    If any "error' from the row 
		**    production (including "no more rows")
		**    release the CUT buffer containing the
		**    status to the reader.
		**
		**    Note that each call to cut_write_buf
		**    in DIRECT mode releases the previously
		**    written cell for reading.
		*/
		num_io_cells = 1;

		/* Get a cell for writing, wait if caught up */
		if ( (status = cut_write_buf(&num_io_cells,
			DataRcb, (PTR*)&BuffHdr,
			(CUT_RW_WAIT | CUT_RW_DIRECT), &error)) 
				== E_DB_OK )
		{
		    /* Produce a(nother) row */
		    call_status = (*out_node->qen_func)(out_node, rcb, dsh, function);

		    /* NO_FUNC really means "get the next row" */
		    function = NO_FUNC;
		    
		    /* Prime BuffHdr */

		    BuffHdr->err_code = dsh->dsh_error.err_code;

		    if ( (BuffHdr->status = call_status) == E_DB_OK )
		    {
			/*
			 * materialize the row into the space just after
			 * "BuffHdr".
			 *
			 * Where we stick this address for ADE depends
			 * on whether a global base array is being used.
			 */
			*MatRow = (char*)BuffHdr + sizeof(BUFF_HDR);

			if ( status = ade_execute_cx(dsh->dsh_adf_cb, ade_excb) )
			    error.err_code = dsh->dsh_adf_cb->adf_errcb.ad_errcode;
			else if ( status = CommRcb->cut_async_status )
			    error.err_code = E_CU0204_ASYNC_STATUS;
			else
			{
			    MatRows++;

			    if ( exch_cb->cut_trace )
			    {
				TRdisplay("%@ %p %2d %s %s%d: NC %x %d\n",
				    my_sid, NodeNum, OutNode, DbgChild, my_threadno, 
				    BuffHdr, MatRows);
			    }
			}
		    }
		}
	    }

	    /* cut_write_buf, ade_execute_cx failure or interrupt? */
	    if ( status )
		break;

	    /* 
	    ** No more rows coming (call_status != E_DB_OK).
	    ** Release the last cell to CUT for reading by
	    ** the Parent.
	    */

	    status = cut_write_complete(DataRcb, &error);

	    if (status != E_DB_OK)
	    {
		if ( exch_cb->trace )
		{
		    TRdisplay("%@ %p %s%d: cut_write_complete status = %d err = %x\n",
			my_sid, MsgChild, my_threadno, status, error.err_code);
		}
		/* Already reported by CUT */
		if ( error.err_code != E_CU0204_ASYNC_STATUS )
		     error.err_code = 0;
		break;
	    }

	    /* 
	    ** The only "good" bad status we expect is
	    ** "no more rows". If we got anything other than
	    ** that, there's no point in waiting for the 
	    ** Parent to tell us to quit, which is all it
	    ** can or will do, so we can terminate now.
	    **
	    ** If we got "no more rows" and the query is
	    ** not iterable, then terminate the thread now
	    ** rather than waiting for the Parent to tell
	    ** us do do so.
	    */

	    if (exch_cb->trace)
		TRdisplay("%@ %p %2d %s %s%d: status=%d err=%x rows %d\n", 
			    my_sid, NodeNum, OutNode, DbgChild, my_threadno, 
			    call_status,
			    dsh->dsh_error.err_code, MatRows);

	    if ( node->qen_flags & QEN_PQ_RESET &&
		 dsh->dsh_error.err_code == E_QE0015_NO_MORE_ROWS )
	    {
		/*
		 * wait for a command from the parent thread
		 */
		if (exch_cb->trace)
		    TRdisplay("%@ %p %2d %s %s%d: waiting for command from the parent\n",
			    my_sid, NodeNum, OutNode, DbgChild, my_threadno);
		num_io_cells = 1;
		status = cut_read_buf(&num_io_cells, CommRcb, 
					    &ExchCommand,
					    TRUE, &error);
		if (status != E_DB_OK)
		{
		    if ( exch_cb->trace )
		    {
			TRdisplay("%@ %p %s%d: cut_read_buf status = %d\n", 
				    my_sid, MsgChild, my_threadno, status);
		    }
		    /* Already reported by CUT */
		    if ( error.err_code != E_CU0204_ASYNC_STATUS )
			 error.err_code = 0;
		    break;
		}
		/*
		 * the usual flow is that the child will come to a NO_MORE_ROWS
		 * condition and the query processing is essentially done; but
		 * for some queries (e.g. cartesian product) we might give it
		 * another go, so the child might be called to continue query
		 * processing
		 */
		if (exch_command == EXCH_ITERATE)
		{
		    if (exch_cb->trace)
			TRdisplay("%@ %p %2d %s %s%d: received EXCH_ITERATE command\n",
			    my_sid, NodeNum, OutNode, DbgChild, my_threadno);

		    /* Pass a RESET down the tree */
		    function = FUNC_RESET;
		    MatRows = 0;
		    continue;
		}

		if ( exch_command == EXCH_TERMINATE )
		{
		    if (exch_cb->trace)
			TRdisplay("%@ %p %2d %s %s%d: received EXCH_TERMINATE command\n",
			    my_sid, NodeNum, OutNode, DbgChild, my_threadno);
		}
	    }

	    /* Copy the DSH error code, terminate the thread */
	    error.err_code = dsh->dsh_error.err_code;
	    break;
	}
    } while (0);


    /* Thread cleanup */

    /*
    ** Only signal the others if any bad status occurred
    ** in this kid, not elsewhere in the plan (dsh_error).
    ** DSH errors have been written to the cut buffer for the
    ** Parent to digest.
    */
    if ( DB_FAILURE_MACRO(status) )
    {
	i4	local_error;
	/* Tell all about our failure */

	if ( error.err_code && error.err_code != E_CU0204_ASYNC_STATUS )
	    ule_format(error.err_code,
		       (CL_ERR_DESC*)0, ULE_LOG, NULL,
		       (char*)0, 0L, (i4*)0, &local_error, 0);
	if ( !dsh || error.err_code != dsh->dsh_error.err_code )
	{
	    if ( exch_cb->trace )
	    {
		TRdisplay("%@ %p %s%d: cut_signal_status status = %d :d\n", 
			    my_sid, MsgChild, my_threadno, status,
			    __LINE__);
	    }
	    (VOID)cut_signal_status(CommRcb, status, &error);
	}
    }

    /* After this, we don't care about returned "status" */

    if ( dsh )
    {
	/* Do this once only, in case of caught EXdeclare */
	if ( !EXcaught )
	{
	    /* Close our piece of the query tree */
	    (void) (*out_node->qen_func)(out_node, rcb, dsh, FUNC_CLOSE);

	    /*
	    ** If trace point QE90 is turned on, roll QEN_STATUS
	    ** stats into the root DSH's QEN_STATUS.
	    **
	    ** Ignore our EXCH node's QEN_STATUS, and those
	    ** which are rooted in the root DSH.
	    **
	    ** Note that in a 1:1 plan, all the QEN_STATUS's
	    ** belong to the root DSH.
	    **
	    ** Accumulating wall-clock time makes no sense
	    ** (e.g., 8 kids each take 10 seconds = 80 seconds?)
	    ** but we'll do it anyway. Anyone interpreting 
	    ** the QE90 "et" stats under an EXCH node will just
	    ** have to be aware of the anomaly.
	    **
	    ** Note also that these stats are accumulated
	    ** possibly concurrently by multiple kids with
	    ** no sort of serialization, so they may at times
	    ** look "off".
	    */
	    if ( exch->exch_ccount != 1 &&
	        ult_check_macro(&qef_cb->qef_trace, 90, &val1, &val2) )
	    {
		for ( i = 0; i < qp->qp_stat_cnt; i++ )
		{
		    if ( i != node->qen_num &&
		         dsh->dsh_xaddrs[i] &&
		         RootDSH->dsh_xaddrs[i] )
		    {
			QenStatus = dsh->dsh_xaddrs[i]->qex_status;
			RootStatus = RootDSH->dsh_xaddrs[i]->qex_status;
			if (QenStatus != RootStatus)
			{
			    RootStatus->node_nbcost += QenStatus->node_nbcost;
			    RootStatus->node_necost += QenStatus->node_necost;
			    RootStatus->node_cpu += QenStatus->node_cpu;
			    RootStatus->node_dio += QenStatus->node_dio;
			    RootStatus->node_wc += QenStatus->node_wc ;
			    RootStatus->node_rcount += QenStatus->node_rcount;
			    RootStatus->node_pcount += QenStatus->node_pcount;
			}
		    }
		}
	    }
	}

	/*
	** Close all tables opened by this thread, destroy
	** all temp tables created by this thread.
	*/
	while ( dmt_cb = dsh->dsh_open_dmtcbs )
	{
	    dsh->dsh_open_dmtcbs = (DMT_CB*)(dmt_cb->q_next);

	    /* If opened, close it */
	    if ( dmt_cb->dmt_record_access_id )
	    {
		/* close the table */
		(void) dmf_call(DMT_CLOSE, dmt_cb);

		/* destroy if it's a temporary table */
		if (TEMP_TABLE_MACRO(dmt_cb))
		{
		    dmt_cb->dmt_flags_mask = 0;
		    (void) dmf_call(DMT_DELETE_TEMP, dmt_cb);
		}

		/* mark the table closed */
		dmt_cb->dmt_record_access_id = (PTR)NULL;
	    }
	}

	/*
	** Don't free memory and streams if this is a 1:1
	** plan. 
	**
	** qen_dsh() doesn't make thread versions of most
	** things when there's but a single Child, so the
	** root DSH is really the "owner" of things like
	** HOLDs and SHDs and their associated memory
	** streams.
	*/
	if ( exch->exch_ccount != 1 )
	{
	    /*
	    ** Free up odd bits of memory that may have been
	    ** allocated.
	    ** Each HOLD may have MEreqmem'd an "unget_buffer"
	    ** and this is our only chance to free that memory.
	    ** It's presumed that all other memory (other than
	    ** SHDs,HASHs,HAGGs) has been acquired using this DSH's
	    ** ULM streamid (dsh_streamid) and will evaporate when 
	    ** we close the DSH stream, below.
	    */
	    if ( dsh->dsh_hold )
	    {
		for ( i = 0;
		      i < qp->qp_hld_cnt;
		      i++ )
		{
		    if ( dsh->dsh_hold[i] && dsh->dsh_hold[i]->unget_buffer )
		    {
			MEfree(dsh->dsh_hold[i]->unget_buffer);
			dsh->dsh_hold[i]->unget_buffer = NULL;
		    }
		}
	    }

	    /*
	    ** Sorts force themselves on the Sort pool and
	    ** never use our DSH stream.
	    **
	    ** Release any remaining sort hold memory.
	    */
	    if ( dsh->dsh_shd )
	    {
		for ( i = 0;
		      i < qp->qp_sort_cnt + 
			  qp->qp_hld_cnt;
		      i++ )
		{
		    if ( dsh->dsh_shd[i] && dsh->dsh_shd[i]->shd_streamid )
		    {
			/* This bit never seems to be set */
			if ( dsh->dsh_shd[i]->shd_options & SHD1_DSH_POOL )
			{
			    /* stream comes from dsh pool */
			    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
			    ulm.ulm_streamid_p = &dsh->dsh_shd[i]->shd_streamid;
			    (void) ulm_closestream(&ulm);
			}
			else
			{
			    /* stream comes from sort pool */
			    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_s_ulmcb, ulm);
			    ulm.ulm_streamid_p = &dsh->dsh_shd[i]->shd_streamid;
			    (void) ulm_closestream(&ulm);
			    /* Hmm, this isn't thread-safe... */
			    qef_cb->qef_s_used -= dsh->dsh_shd[i]->shd_size;
			}
		    }
		}
	    }

	    /* Do likewise for HASH if not dsh_streamid ... */
	    if ( dsh->dsh_hash )
	    {
		STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_s_ulmcb, ulm);

		for (i = 0; i < qp->qp_hash_cnt; i++)
		{
		    if ( dsh->dsh_hash[i] && 
			 dsh->dsh_hash[i]->hsh_streamid &&
			 dsh->dsh_hash[i]->hsh_streamid != dsh->dsh_streamid )
		    {
			ulm.ulm_streamid_p = &dsh->dsh_hash[i]->hsh_streamid;
			qef_cb->qef_h_used -= dsh->dsh_hash[i]->hsh_size;
			(void) ulm_closestream(&ulm);
		    }
		}
	    }
	    /* ...and HASH AGGREGATEs */
	    if ( dsh->dsh_hashagg )
	    {
		STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_s_ulmcb, ulm);

		for (i = 0; i < qp->qp_hashagg_cnt; i++)
		{
		    if ( dsh->dsh_hashagg[i] &&
			 dsh->dsh_hashagg[i]->hshag_streamid &&
			 dsh->dsh_hashagg[i]->hshag_streamid != dsh->dsh_streamid )
		    {
			ulm.ulm_streamid_p = &dsh->dsh_hashagg[i]->hshag_streamid;
			qef_cb->qef_h_used -= dsh->dsh_hashagg[i]->hshag_halloc;
			(void) ulm_closestream(&ulm);
		    }
		}
	    }
	    /* Restore ulm_rcb to ref DSH */
	    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
	}

	/* Finally, destroy this thread's DSH's memory stream.
	** (Same thing as qstreamid.)
	*/
	ulm.ulm_streamid_p = &dsh->dsh_streamid;
	dsh = NULL;
	(void) ulm_closestream(&ulm);
    }

    /* Disconnect the transaction.  Since this is a shared transaction
    ** we'll merely detach from it.
    */
    if ( tran_id )
	(void) qet_disconnect(rcb, &tran_id, &error);

    if (exch_cb->trace)
	TRdisplay("%@ %p %2d %s %s%d: has left the building\n", 
			my_sid, NodeNum, OutNode, DbgChild, my_threadno);
    
    /*
    ** Lastly, detach from CUT. If the parent is waiting for us to
    ** finish, the detach will signal the parent.
    */
    (void) cut_signal_status(NULL, E_DB_OK, &error);
    (void) cut_thread_term(TRUE, &error);

    EXdelete();

    /* Return to SCF to eradicate the thread */
    return(status);
}

/*{
** Name: QEN_DSH    - allocate, format DSH block for child
**	thread of exchange node
**
** Description:
**      For thread safety in || queries we must allocate distinct
**	DSH blocks for each thread executing a query plan fragment.
**	This function creates a new memory stream and allocates 
**	a child DSH. Most of the parent DSH is copied
**	to it and various fields are rebuilt to assure thread safety.
**
**	The Parent's DSH must be handled in a read-only fashion
**	as multiple Kids may be making their copies concurrently.
**
** Inputs:
**	threadno			This kid's sequential
**					thread number, assigned
**					by the Parent.
**	rcb				ptr to request block
**	dsh				ptr to read-only parent DSH
**	node				ptr to exchange node
**
** Outputs:
**	newdshp				ptr to addr of Kid's DSH
**					
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**
** History:
**	26-feb-04 (inkdo01)
**	    Written for parallel query processing.
**	26-apr-04 (toumi01)
**	    1:n child dsh copy cleanup.
**	13-may-04 (toumi01)
**	    Fix DSH bit map copy logic.
**	18-may-04 (inkdo01)
**	    When filling excb_bases, add the ADE_ZBASE origin.
**	    Various qen_dsh fixups.
**	19-may-04 (inkdo01)
**	    Copy intermediate results buffer - might have coerced constants.
**	    And copy QEN_TKEY structures from dsh_cbs[].
**	11-june-04 (inkdo01)
**	    (Temporarily) force even 1st thread to allocate all entities.
**	22-june-04 (inkdo01)
**	    Dropped QEN_TKEY - it's more difficult and needed separate 
**	    attention.
**	15-Jul-2004 (jenjo02)
**	    Undid ink's 11-june-04 change. With it, lots of queries
**	    return no rows (?).
**	    DSH copy is now the responsibility of the Kids, so this
**	    function makes one copy when called from the Kid.
**	19-Jul-2004 (schka24)
**	    Make runtime stream same as DSH stream for kids.
**	21-Jul-2004 (jenjo02)
**	    Consolidate a bunch of qec_mallocs, allocate, prepare,
**	    and return pointer to BUFF_HDR structure, modify the
**	    materialization buffer to be just after BUFF_HDR.
**	27-Jul-2004 (jenjo02)
**	    Replaced EXCH bitmaps with entity counts, index arrays,
**	    consolidate as much as possible with DSH chunk.
**	30-Jul-2004 (jenjo02)
**	    Allocate NULL space for dsh_kor_nor pointers, just
**	    zero QEN_STATUS structure space rather than copying it.
**	30-Jul-2004 (jenjo02)
**	    Whoops - that last change dropped setting QEN_STATUS
**	    pointers, all better now.
**	05-Aug-2004 (jenjo02)
**	    Ensure that all row buffers are ALIGN_RESTRICTed
**	    so that DMF doesn't waste time copying rows to 
**	    align them properly.
**	10-Aug-2004 (jenjo02)
**	    Must always use the Root DSH as the source of stuff
**	    to allocate and copy; Kids' DSHs may be too sparse.
**	18-Aug-2004 (jenjo02)
**	    Allocate blocked CUT buffer for Child.
**	09-Sep-2004 (jenjo02)
**	    CUT buffer allocation no longer needed; Kids use
**	    DIRECT writes.
**	    Added support for GLOBAL base arrays.
**	20-Sep-2004 (jenjo02)
**	    HASH and HASH AGGREGATE streams now come from
**	    sort memory, not the DSH stream.
**	30-Jun-2005 (jenjo02)
**	    Even for 1:1 plans, regular tables' DMR_CB and DMT_CBs
**	    must be uniquified for the child to avoid lock/log_id
**	    and transaction conflicts with the main QP which may
**	    have already opened the tables in another transaction
**	    context.
**	16-Jan-2006 (kschendel)
**	    Need to resolve EXCB addresses in child's XADDR structures
**	    after we allocate everything.
**	    Apply maximum paranoia to alignment of things.  Just in case.
**	05-Sep-2006 (smeke01)
**	    Bug b116479. Copy across the array of QEN_NOR pointers
**	    to the ChildDSH. Previously the pointers were left
**	    NULL in the erroneous belief that qen_keyprep() would
**	    set them up, leading to bug b116479. Note that it is
**	    thread-safe to use the structures set up in the root
**	    DSH rather than create separate ones for the children.
**	    This is because although each ChildDSH gets a copy of
**	    the whole array, the index into the array is unique
**	    to each child thread via the index value stored on 
**	    the query-node in each child-thread/query-node 
**	    pairing.
*/

DB_STATUS
qen_dsh(
i4		threadno,
EXCH_CB		*exch_cb,
QEE_DSH		**newdshp,
DB_ERROR	*error)

{
    QEF_QP_CB	*qp;
    QEE_DSH	*RootDSH, *ChildDSH;
    QEF_RCB	*rcb = exch_cb->rcb;
    QEN_NODE	*node = exch_cb->node;
    ULM_RCB	ulm;
    ADE_EXCB	*excbp, *Dexcbp;
    QEN_ADF	*qen_adf;
    QEN_HASH_BASE *hptr, *Dhptr;
    QEN_HASH_AGGREGATE *haptr, *Dhaptr;
    QEN_STATUS	*qen_status;
    QEN_EXCH	*exch = &node->node_qen.qen_exch;
    struct _EXCH_IX *exix;
    i4		rowsz;
    i4		i, ix, j, ax, hold_ax, h;
    DB_STATUS	status = E_DB_OK;
    QEE_TEMP_TABLE *tt, *Dtt;
    DMR_CB	*dmr;
    DMT_CB	*dmt;
    DMH_CB	*dmh;
    QEN_HOLD	*hold, *Dhold;
    QEN_SHD	*shd;
    QEF_VALID	*vl;
    i4		np;
    i4		shdCnt;

    /* Extract Root DSH from EXCH_CB */
    RootDSH = exch_cb->dsh;
    qp = RootDSH->dsh_qp_ptr;


    /* Make a copy of the Root DSH for this Kid. Root contents are copied
    ** to the child. For children numbered > 1, various fields (ptrs and
    ** structures) are re-allocated to give the child distinct copies for
    ** the sake of thread safety. If there is only one child (a 1:1 exchange)
    ** this isn't necessary since plan initialization performed the needed
    ** allocations. */

    /* Nothing here, yet */
    *newdshp = NULL;

    /* Use DSH memory */
    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
					/* prepare ulm for memory calls */

    /* Set up new stream for DSH, then alloc. DSH in it. */

    /*
    ** Get memory for the DSH, row_array,pointer arrays, 
    ** and fixed structures in one piece, then carve it up.
    **
    **    -----------------------------------------------
    **	  | DSH						|
    **    -----------------------------------------------
    **	  | dsh_row pointer array              		|
    **    -----------------------------------------------
    **	  | dsh_cbs pointer array                     	|
    **    -----------------------------------------------
    **
    **    	For 1:1 plans add...
    **
    **    -----------------------------------------------
    **	  | DMR_CBs                            		|
    **	  | DMT_CBs                            		|
    **    -----------------------------------------------
    **
    **		If 1:n plan, add...
    **
    **	  | space for additional pointer arrays:	|
    **	  |       dsh_hash                      	|
    **	  |       dsh_hashagg                      	|
    **	  |       dsh_xaddrs                      	|
    **	  |       dsh_tempTables                      	|
    **	  |	  dsh_kor_nor				|
    **	  |       dsh_hold                      	|
    **	  |       dsh_shd	                      	|
    **    -----------------------------------------------
    **	  | space for predictable, fixed structures:	|
    **	  | QEN_STATUS                             	|
    **	  | QEE_XADDRS                             	|
    **	  | QEN_TEMP_TABLE                             	|
    **	  | QEN_HOLD                             	|
    **	  | QEN_SHD                             	|
    **	  | DMR_CB                             		|
    **	  | DMT_CB                             		|
    **	  | DMH_CB                             		|
    **    -----------------------------------------------
    */
    ulm.ulm_flags = ULM_PRIVATE_STREAM | ULM_OPEN_AND_PALLOC;
    ulm.ulm_psize = ulm.ulm_blocksize = DB_ALIGN_MACRO(sizeof(QEE_DSH));

    /* For || union, must address array specific to each select. */
    if ( exch->exch_flag & EXCH_UNION )
	exix = &exch->exch_ixes[threadno-1];
    else
	exix = &exch->exch_ixes[0];

    /* Pointers:  row buffers, cb's */
    np = qp->qp_row_cnt + qp->qp_cb_cnt;

    /* For a 1:1 plan we need the row array, dsh_cbs, DMR_CB+DMT_CB */
    if ( exch->exch_ccount == 1 )
    {
	/* The size of all DMR_CB plus DMT_CB */
	i = DB_ALIGN_MACRO(exix->exch_dmr_cnt * sizeof(DMR_CB));
	i += DB_ALIGN_MACRO(exix->exch_dmt_cnt * sizeof(DMT_CB));
	ulm.ulm_psize += i;
    }
    else
    {
	/* Number of QEN_SHD pointers */
	shdCnt = 0;
	if ( exix->exch_hld_cnt + exix->exch_shd_cnt > 0 )
	    shdCnt = qp->qp_sort_cnt + qp->qp_hld_cnt;

	/* The size of additional pointer arrays.
	** Indexes into these arrays were computed for the entire QP,
	** not just this exch subtree, so the pointer arrays have to be
	** full size.
	*/
	np += shdCnt + qp->qp_kor_cnt;
	if (exix->exch_hsh_cnt > 0) np += qp->qp_hash_cnt;
	if (exix->exch_hsha_cnt > 0) np += qp->qp_hashagg_cnt;
	if (exix->exch_stat_cnt > 0) np += qp->qp_stat_cnt;
	if (exix->exch_ttab_cnt > 0) np += qp->qp_tempTableCount;
	if (exix->exch_hld_cnt > 0) np += qp->qp_hld_cnt;

	/* The size of all fixed-size structures */
	i = DB_ALIGN_MACRO(exix->exch_stat_cnt * sizeof(QEN_STATUS));
	i += DB_ALIGN_MACRO(exix->exch_stat_cnt * sizeof(QEE_XADDRS));
	i += DB_ALIGN_MACRO(exix->exch_ttab_cnt * sizeof(QEE_TEMP_TABLE));
	i += DB_ALIGN_MACRO(exix->exch_hld_cnt * sizeof(QEN_HOLD));
	i += DB_ALIGN_MACRO((exix->exch_shd_cnt + exix->exch_hld_cnt)
					* sizeof(QEN_SHD));
	i += DB_ALIGN_MACRO((exix->exch_dmr_cnt + exix->exch_ttab_cnt
		+ exix->exch_hld_cnt) * sizeof(DMR_CB));
	i += DB_ALIGN_MACRO((exix->exch_dmt_cnt + exix->exch_ttab_cnt
		+ exix->exch_hld_cnt) * sizeof(DMT_CB));
	i += DB_ALIGN_MACRO(exix->exch_dmh_cnt * sizeof(DMH_CB));
	ulm.ulm_psize += i;
    }
    ulm.ulm_psize += DB_ALIGN_MACRO(np * sizeof(PTR));
    
    status = qec_mopen(&ulm);
    if (status != E_DB_OK)
    {
	error->err_code = ulm.ulm_error.err_code;
	return(status);
    }

    do
    {
	PTR ptrs, Mem;		/* Just like qee */

	/* Copy contents of root DSH, then overlay as needed. */
	ChildDSH = (QEE_DSH *)ulm.ulm_pptr;
	MEcopy((PTR)RootDSH, sizeof(QEE_DSH), (PTR) ChildDSH);
	ChildDSH->dsh_streamid = ulm.ulm_streamid;
	/* Set dsh_handle to dsh_streamid */
	ChildDSH->dsh_handle = ChildDSH->dsh_streamid;

	/* Set up query-time stream ID.  Since this is a dynamically
	** created DSH that will go away at end-of-query, we can use
	** the same stream as the DSH came from.
	*/
	ChildDSH->dsh_qstreamid = ChildDSH->dsh_streamid;

	ChildDSH->dsh_threadno = threadno;
	ChildDSH->dsh_qe_adf = (QEN_ADF *) NULL;

	/* No tables opened by this thread, yet */
	ChildDSH->dsh_open_dmtcbs = NULL;

	/* Child DSH doesn't need cancel checking */
	ChildDSH->dsh_sid = (CS_SID) 0;

	/* FIXME We use the same ADF CB in dsh_adf_cb, probably shouldn't */

	/* After the DSH comes the row pointer array.  Arrange to have
	** all pointer arrays together, actual struct things after that.
	*/
	ptrs = (PTR) ME_ALIGN_MACRO( (ChildDSH+1) , DB_ALIGN_SZ);
	MEfill(np*sizeof(PTR), 0, ptrs);
	Mem = ME_ALIGN_MACRO( (ptrs + (np*sizeof(PTR))), DB_ALIGN_SZ);

	ChildDSH->dsh_row = (PTR*) ptrs;
	ptrs = ptrs + qp->qp_row_cnt * sizeof(PTR);
	MEcopy((PTR)RootDSH->dsh_row, qp->qp_row_cnt * sizeof(PTR),
			(PTR)ChildDSH->dsh_row);

	/* The Child will set the pointer to the Mat buffer */
	ChildDSH->dsh_row[node->qen_row] = (PTR)NULL;

	/* After the dsh_row pointer array comes the dsh_cbs */
	ChildDSH->dsh_cbs = (PTR *) ptrs;
	ptrs = ptrs + qp->qp_cb_cnt * sizeof(PTR);
	MEcopy((PTR)RootDSH->dsh_cbs, qp->qp_cb_cnt * sizeof(PTR), (PTR)ChildDSH->dsh_cbs);
	
	/* 1:1 plan ? */
	if ( exch->exch_ccount == 1 )
	{
	    /* For 1:1 child thread, almost everything is prepared 
	    ** by master plan initialization. The one exception is the
	    ** child thread materialization buffer, which will be
	    ** directly written into a CUT cell.
	    **
	    ** Other exceptions (ahem) are a copy of the dsh_cbs
	    ** (which we will twiddle with) and the appropriate
	    ** DMR_CB/DMT_CB structures.
	    */

	    /* And the last (?) exception is the storage of the child
	    ** buffer ptr array address into the global base array
	    ** slot of the local base arrays of all ADE_EXCBs. This
	    ** must be done when global base arrays are in effect. */


	    /* Loop over ADE_EXCB structure entries in array (if
	    ** global base arrays are in effect). */
	    if (qp->qp_status & QEQP_GLOBAL_BASEARRAY)
	    {
		ax = 0;	/* offset in 2nd index array of ADE_EXCB ixes */

	        for ( i = 0; i < exix->exch_cx_cnt; i++, ax++ )
	        {
		    j = exix->exch_array2[ax];

		    Dexcbp = (ADE_EXCB *)RootDSH->dsh_cbs[j];
		    Dexcbp->excb_bases[ADE_GLOBALBASE] = (PTR)ChildDSH->dsh_row;
	        }
	    }
	    else
		ax = exix->exch_cx_cnt;

	    /* DMR_CB structures. */
	    dmr = (DMR_CB*) Mem;
	    Mem += DB_ALIGN_MACRO(exix->exch_dmr_cnt * sizeof(DMR_CB));

	    for ( i = 0; i < exix->exch_dmr_cnt; i++, ax++, dmr++ )
	    {
		j = exix->exch_array2[ax];
		ChildDSH->dsh_cbs[j] = (PTR)dmr;
		MEcopy((PTR)RootDSH->dsh_cbs[j],
			    sizeof(DMR_CB), (PTR)dmr);
		/*
		** If already opened and a regular table,
		** NULL access_id to trigger a new instantiation
		** for this thread.
		*/
		if ( dmr->dmr_access_id )
		{
		    for ( vl = ChildDSH->dsh_ahdvalid; vl; vl = vl->vl_next )
		    {
			if ( vl->vl_dmr_cb == j && 
			    vl->vl_resource->qr_resource.
			    qr_tbl.qr_tbl_type == QEQR_REGTBL )
			{
			    dmr->dmr_access_id = NULL;
			    break;
			}
		    }
		}
	    }

	    /* DMT_CB structures. */
	    dmt = (DMT_CB*) Mem;
	    Mem += DB_ALIGN_MACRO(exix->exch_dmt_cnt * sizeof(DMT_CB));

	    for ( i = 0; i < exix->exch_dmt_cnt; i++, ax++, dmt++ )
	    {
		j = exix->exch_array2[ax];
		ChildDSH->dsh_cbs[j] = (PTR)dmt;
		MEcopy((PTR)RootDSH->dsh_cbs[j],
			    sizeof(DMT_CB), (PTR)dmt);
		/*
		** If already opened and a regular table,
		** NULL access_id to trigger a new instantiation
		** for this thread.
		*/
		if ( dmt->dmt_record_access_id )
		{
		    for ( vl = ChildDSH->dsh_ahdvalid; vl; vl = vl->vl_next )
		    {
			if ( vl->vl_dmf_cb == j && 
			    vl->vl_resource->qr_resource.
			    qr_tbl.qr_tbl_type == QEQR_REGTBL )
			{
			    dmt->dmt_record_access_id = NULL;
			    break;
			}
		    }
		}
	    }

	    /* The Child code will set the mat addr to the CUT cell */


	    /* That's it for 1:1 */
	    break;
	}

	/* The DSH contains numerous pointer arrays each of which
	** addresses instances of various entities required for query
	** execution (buffers, control structurees, etc.). Distinct copies
	** of most of these entities are required for each thread executing
	** a plan fragment. */


	/* "ax" indexes exch_array1 */
	ax = 0;


	/* Row buffers. */
	for ( i = 0; i < exix->exch_row_cnt; i++, ax++ )
	{
	    j = exix->exch_array1[ax];

	    /*
	    ** Don't need to allocate materialization buffer
	    ** or special bases (qp_row_len == -1)
	    */
	    if ( j != node->qen_row && qp->qp_row_len[j] > 0 )
	    {
		ulm.ulm_psize = DB_ALIGN_MACRO(qp->qp_row_len[j]);
		if ( status = qec_malloc(&ulm) )
		    break;
		ChildDSH->dsh_row[j] = ulm.ulm_pptr;

		/* If this is the temp buffer, copy from root, else
		** just fill with 0's. */
		if (j == ADE_IBASE)
		    MEcopy(RootDSH->dsh_row[j], qp->qp_row_len[j],
				ChildDSH->dsh_row[j]);
		else 
		    MEfill(qp->qp_row_len[j], 0, ChildDSH->dsh_row[j]);
	    }

	}	/* end of row buffer allocations */
	if ( status )
	    break;

	/* Hash join structures. */
	if ( exix->exch_hsh_cnt )
	{
	    ChildDSH->dsh_hash = (QEN_HASH_BASE**)ptrs;
	    ptrs += qp->qp_hash_cnt * sizeof(PTR);

	    for ( i = 0; i < exix->exch_hsh_cnt; i++, ax++ )
	    {
		j = exix->exch_array1[ax];

		/* The hash we'll copy */
		Dhptr = RootDSH->dsh_hash[j];
		
		ulm.ulm_psize = sizeof (QEN_HASH_BASE);
		
		rowsz = (Dhptr->hsh_browsz > Dhptr->hsh_prowsz) ?
				Dhptr->hsh_browsz : Dhptr->hsh_prowsz;
		rowsz = DB_ALIGN_MACRO(rowsz);

		ulm.ulm_psize += Dhptr->hsh_pcount * sizeof(QEN_HASH_PART) +
			sizeof(QEN_HASH_LINK) + rowsz + rowsz;

		if ( status = qec_malloc(&ulm) )
		    break;

		/* Our copy of the hash */
		hptr = ChildDSH->dsh_hash[j] = (QEN_HASH_BASE *) ulm.ulm_pptr;

		MEcopy((PTR)Dhptr, sizeof(QEN_HASH_BASE), (PTR)hptr);
		/* HASH streams always come from sort pool, not DSH */
		hptr->hsh_streamid = NULL;
		/* Format structures unique to this QEN_HASH_BASE. */
		hptr->hsh_workrow = (PTR)hptr + sizeof(QEN_HASH_BASE);
		hptr->hsh_workrow2 = (PTR)&((char *)hptr->hsh_workrow)[rowsz];
		hptr->hsh_currlink = hptr->hsh_toplink = 
		    (QEN_HASH_LINK *)&((char *)hptr->hsh_workrow)[2*rowsz];
		MEcopy((PTR)Dhptr->hsh_toplink, 
			sizeof(QEN_HASH_LINK), 
			(PTR)hptr->hsh_toplink);
		hptr->hsh_currlink->hsl_partition = 
	(QEN_HASH_PART *)&((char *)hptr->hsh_currlink)[sizeof(QEN_HASH_LINK)];
		MEcopy((PTR)Dhptr->hsh_toplink->hsl_partition,
		    hptr->hsh_pcount * sizeof(QEN_HASH_PART),
		    (PTR)hptr->hsh_toplink->hsl_partition);
					/* copy partition array */

	    }	/* end of hash structure allocations */
	}
	else 
	    ChildDSH->dsh_hash = (QEN_HASH_BASE **) NULL;

	/* Hash aggregate structures. */
	
	if ( exix->exch_hsha_cnt && status == E_DB_OK )
	{
	    ChildDSH->dsh_hashagg = (QEN_HASH_AGGREGATE**)ptrs;
	    ptrs += qp->qp_hashagg_cnt * sizeof(PTR);

	    /* Loop over hash aggregate structure entries */
	    for ( i = 0; i < exix->exch_hsha_cnt; i++, ax++ )
	    {
		j = exix->exch_array1[ax];

		/* The hashagg we'll copy */
		Dhaptr = RootDSH->dsh_hashagg[j];

		ulm.ulm_psize = sizeof (QEN_HASH_AGGREGATE);
		rowsz = Dhaptr->hshag_obufsz;
		rowsz = DB_ALIGN_MACRO(rowsz);
		ulm.ulm_psize += Dhaptr->hshag_pcount * sizeof(QEN_HASH_PART) +
				    sizeof(QEN_HASH_LINK) + rowsz;
		if ( status = qec_malloc(&ulm) )
		    break;

		haptr = ChildDSH->dsh_hashagg[j] = (QEN_HASH_AGGREGATE *) ulm.ulm_pptr;
		MEcopy((PTR)Dhaptr, sizeof(QEN_HASH_AGGREGATE), (PTR)haptr);
		/* HAGG streams always come from sort pool, not DSH */
		haptr->hshag_streamid = NULL;
		/* Format structures unique to this QEN_HASH_AGGREGATE. */
		haptr->hshag_workrow = (PTR)haptr + sizeof(QEN_HASH_AGGREGATE);
		haptr->hshag_currlink = haptr->hshag_toplink = 
		    (QEN_HASH_LINK *)&((char *)haptr->hshag_workrow)[rowsz];
		haptr->hshag_currlink->hsl_partition = 
	(QEN_HASH_PART *)&((char *)haptr->hshag_currlink)[sizeof(QEN_HASH_LINK)];
		MEcopy((PTR)Dhaptr->hshag_toplink->hsl_partition,
		    haptr->hshag_pcount * sizeof(QEN_HASH_PART),
		    (PTR)haptr->hshag_toplink->hsl_partition);
					/* copy partition array */
	    }
	}	/* end of hash aggregate structure allocations */
	else 
	    ChildDSH->dsh_hashagg = (QEN_HASH_AGGREGATE **) NULL;

	/* Node address resolution and status structures. */

	if ( exix->exch_stat_cnt && status == E_DB_OK )
	{
	    QEE_XADDRS *xaddrs;

	    ChildDSH->dsh_xaddrs = (QEE_XADDRS **)ptrs;
	    ptrs += qp->qp_stat_cnt * sizeof(PTR);
	    qen_status = (QEN_STATUS*) Mem;
	    Mem += DB_ALIGN_MACRO(exix->exch_stat_cnt * sizeof(QEN_STATUS));
	    xaddrs = (QEE_XADDRS *) Mem;
	    Mem += DB_ALIGN_MACRO(exix->exch_stat_cnt * sizeof(QEE_XADDRS));
	    /*
	    ** Rather than copying the QEN_STATUSs, just
	    ** zero-fill them to start with a clean slate.
	    ** This "sets" node_status to QEN0_INITIAL.
	    ** However, we do copy parent's xaddrs ptr array so 
	    ** all qen_status ptrs are filled in (fixes SEGV
	    ** in qeq_validate in || unions);
	    */
	    MEcopy((PTR)RootDSH->dsh_xaddrs, qp->qp_stat_cnt * sizeof(PTR),
		   (PTR)ChildDSH->dsh_xaddrs);
	    MEfill(exix->exch_stat_cnt * sizeof(QEN_STATUS), 0, qen_status);
	    MEfill(exix->exch_stat_cnt * sizeof(QEE_XADDRS), 0, xaddrs);

	    /* Set pointers to status structures */
	    for ( i = 0; i < exix->exch_stat_cnt; i++, ax++ )
	    {
		j = exix->exch_array1[ax];
		ChildDSH->dsh_xaddrs[j] = xaddrs;
		xaddrs->qex_status = qen_status;
		xaddrs++;
		qen_status++;
	    }
	}
	else 
	    ChildDSH->dsh_xaddrs = (QEE_XADDRS **) NULL;

	/* Place all DMR_CB's next, followed by all DMT_CB's.  This
	** extreme paranoia ensures proper alignment.
	*/
	dmr = (DMR_CB *) Mem;
	Mem += DB_ALIGN_MACRO((exix->exch_dmr_cnt + exix->exch_ttab_cnt
		+ exix->exch_hld_cnt) * sizeof(DMR_CB));
	dmt = (DMT_CB *) Mem;
	Mem += DB_ALIGN_MACRO((exix->exch_dmt_cnt + exix->exch_ttab_cnt
		+ exix->exch_hld_cnt) * sizeof(DMT_CB));

	/* Temp table structures (in OJ processing). */

	if ( exix->exch_ttab_cnt && status == E_DB_OK )
	{
	    ChildDSH->dsh_tempTables = (QEE_TEMP_TABLE**)ptrs;
	    ptrs += qp->qp_tempTableCount * sizeof(PTR);
	    tt = (QEE_TEMP_TABLE*) Mem;

	    /* Each QEE_TEMP_TABLE addresses a DMR_CB and a DMT_CB.
	    ** they must be allocated and copied */
	    Mem += DB_ALIGN_MACRO(exix->exch_ttab_cnt * sizeof(QEE_TEMP_TABLE));

	    /* Loop over temp table structure entries in array. */
	    for ( i = 0; i < exix->exch_ttab_cnt; i++, ax++ )
	    {
		j = exix->exch_array1[ax];

		ChildDSH->dsh_tempTables[j] = tt;

		Dtt = RootDSH->dsh_tempTables[j];

		MEcopy((PTR)Dtt, sizeof(QEE_TEMP_TABLE), (PTR)tt);

		MEcopy((PTR)Dtt->tt_dmrcb, sizeof(DMR_CB), (PTR)dmr);
		tt->tt_dmrcb = dmr;
		dmr->dmr_access_id = NULL;
		++ dmr;

		MEcopy((PTR)Dtt->tt_dmtcb, sizeof(DMT_CB), (PTR)dmt);
		tt->tt_dmtcb = dmt; 
		dmt->dmt_record_access_id = NULL;
		++ dmt;

		++ tt;
	    }

	}	/* end of temp table structure allocations */
	else 
	    ChildDSH->dsh_tempTables = (QEE_TEMP_TABLE **) NULL;

	/* Hold structures. */

	if ( exix->exch_hld_cnt && status == E_DB_OK )
	{
	    ChildDSH->dsh_hold = (QEN_HOLD**)ptrs;
	    ptrs += qp->qp_hld_cnt * sizeof(PTR);
	    hold = (QEN_HOLD*) Mem;

	    /* Each QEN_HOLD addresses a DMR_CB and a DMT_CB.
	    ** They must be re-allocated and copied. */
	    Mem += DB_ALIGN_MACRO(exix->exch_hld_cnt * sizeof(QEN_HOLD));

	    /* Remember HOLD array index for SHD, below */
	    hold_ax = ax;

	    /* Loop over hold structure entries in array. */
	    for ( i = 0; i < exix->exch_hld_cnt; i++, ax++ )
	    {
		j = exix->exch_array1[ax];

		Dhold = RootDSH->dsh_hold[j];
		
		ChildDSH->dsh_hold[j] = hold;
		MEcopy((PTR)Dhold, sizeof(QEN_HOLD), (PTR)hold);

		MEcopy((PTR)Dhold->hold_dmr_cb, sizeof(DMR_CB), (PTR)dmr);
		hold->hold_dmr_cb = dmr;
		dmr->dmr_access_id = NULL;
		++ dmr;

		MEcopy((PTR)Dhold->hold_dmt_cb, sizeof(DMT_CB), (PTR)dmt);
		hold->hold_dmt_cb = dmt;
		dmt->dmt_record_access_id = NULL;
		++ dmt;

		++ hold;
	    }

	}	/* end of hold structure allocations */
	else 
	    ChildDSH->dsh_hold = (QEN_HOLD **) NULL;

	/* Sort hold structures. */
	/* Note that there is an additional SHD for each HOLD, above */
	
	if ( shdCnt && status == E_DB_OK )
	{
	    ChildDSH->dsh_shd = (QEN_SHD**)ptrs;
	    ptrs += shdCnt * sizeof(PTR);

	    shd = (QEN_SHD*) Mem;
	    Mem += DB_ALIGN_MACRO((exix->exch_shd_cnt + exix->exch_hld_cnt)
			    * sizeof(QEN_SHD));

	    /* Loop over sort hold structure entries in array. */

	    for ( i = 0; i < exix->exch_shd_cnt; i++, ax++, shd++ )
	    {
		j = exix->exch_array1[ax];
		ChildDSH->dsh_shd[j] = shd;

		MEcopy((PTR)RootDSH->dsh_shd[j],
			sizeof(QEN_SHD), (PTR)shd);
		/* SHD streams always come from sort pool, not DSH */
		shd->shd_streamid = NULL;
	    }

	    /* Next come SHDs for HOLDs, if any */

	    /* Revert to HOLD array (hold_ax) */
	    
	    for ( i = 0; i < exix->exch_hld_cnt; i++, hold_ax++, shd++ )
	    {
		/* 
		** Per Doug, HOLD SHDs slots are offset by
		** qp->qp_sort_cnt.
		*/
		j = exix->exch_array1[hold_ax] + qp->qp_sort_cnt;

		ChildDSH->dsh_shd[j] = shd;
		MEcopy((PTR)RootDSH->dsh_shd[j],
			sizeof (QEN_SHD), (PTR)shd);
		/* SHD streams always come from sort pool, not DSH */
		shd->shd_streamid = NULL;
	    }
	}	/* end of sort hold structure allocations */
	else 
	    ChildDSH->dsh_shd = (QEN_SHD **) NULL;

	/* Pointers to QEN_NORs */
	if ( qp->qp_kor_cnt > 0 )
	{
	    ChildDSH->dsh_kor_nor = (QEN_NOR**)ptrs;

	    /* 
	    ** Bug b116479.  
	    ** Copy across the array of QEN_NOR pointers to the ChildDSH. 
	    */
	    MEcopy((PTR)RootDSH->dsh_kor_nor, qp->qp_kor_cnt * sizeof(PTR), (PTR)ChildDSH->dsh_kor_nor);

	    ptrs += qp->qp_kor_cnt * sizeof(PTR);
	}


	/* The following are all structures whose addresses are stored
	** in the dsh_cbs pointer array. They are processed separately
	** because of varying memory and initialization requirements. */

	if ( qp->qp_cb_cnt > 0 && status == E_DB_OK )
	{
	    /* These all come from exch_array2 */
	    ax = 0;

	    /* ADE_EXCB structures. */

	    /* Loop over ADE_EXCB structure entries in array. */
	    for ( i = 0; i < exix->exch_cx_cnt; i++, ax++ )
	    {
		j = exix->exch_array2[ax];

		Dexcbp = (ADE_EXCB *)RootDSH->dsh_cbs[j];
		qen_adf = (QEN_ADF *)Dexcbp->excb_bases[ADE_QENADF];
		/* Allocate ADE_EXCB structure + CX base array. */
		ulm.ulm_psize = sizeof (ADE_EXCB) + (qen_adf->qen_sz_base +
			    ADE_ZBASE) * sizeof (PTR);
		if ( status = qec_malloc(&ulm) )
		    break;

		ChildDSH->dsh_cbs[j] = ulm.ulm_pptr;
		excbp = (ADE_EXCB *) ulm.ulm_pptr;
		MEcopy((PTR)Dexcbp,
		    ulm.ulm_psize, (PTR)excbp);

		/* If used, set pointer to global base array. */
		if ( qp->qp_status & QEQP_GLOBAL_BASEARRAY )
		    excbp->excb_bases[ADE_GLOBALBASE] = (PTR)ChildDSH->dsh_row;
		else
		    excbp->excb_bases[ADE_GLOBALBASE] = NULL;

		/* Copy QEN_ROW base entries from updated dsh_row array. */
		for (ix = 0; ix < qen_adf->qen_sz_base; ix++)
		  if (qen_adf->qen_base[ix].qen_array == QEN_ROW)
		    excbp->excb_bases[ADE_ZBASE+ix] = ChildDSH->dsh_row[qen_adf->
				    qen_base[ix].qen_index];
	    }	/* end of ADE_EXCB structure allocations */

	    if ( status == E_DB_OK )
	    {

		/* Remaining DMR_CB structures. */
		for ( i = 0; i < exix->exch_dmr_cnt; i++, ax++, dmr++ )
		{
		    j = exix->exch_array2[ax];
		    ChildDSH->dsh_cbs[j] = (PTR)dmr;
		    MEcopy((PTR)RootDSH->dsh_cbs[j],
				sizeof(DMR_CB), (PTR)dmr);
		    dmr->dmr_access_id = NULL;
		}

		/* Remaining DMT_CB structures. */

		for ( i = 0; i < exix->exch_dmt_cnt; i++, ax++, dmt++ )
		{
		    j = exix->exch_array2[ax];
		    ChildDSH->dsh_cbs[j] = (PTR)dmt;
		    MEcopy((PTR)RootDSH->dsh_cbs[j],
				sizeof(DMT_CB), (PTR)dmt);
		    dmt->dmt_record_access_id = NULL;
		}

		/* DMH_CB structures at the very end. */
		dmh = (DMH_CB*) Mem;

		for ( i = 0; i < exix->exch_dmh_cnt; i++, ax++, dmh++ )
		{
		    j = exix->exch_array2[ax];
		    ChildDSH->dsh_cbs[j] = (PTR)dmh;
		    MEcopy((PTR)RootDSH->dsh_cbs[j],
				sizeof(DMH_CB), (PTR)dmh);
		}

		/* Fill in ADE_EXCB addresses in XADDR structures,
		** starting with the exch node itself.  Resolve through
		** any QP nodes we might find.
		*/
		qee_resolve_xaddrs(ChildDSH, node, TRUE);
	    }
	}
	else
	    ChildDSH->dsh_cbs = (PTR*)NULL;

    } while (0);

    if ( status )
    {
	/* The only thing done is ulm calls... */
	error->err_code = ulm.ulm_error.err_code;

	/* Toss the DSH stream */
	(void)ulm_closestream(&ulm);
	*newdshp = NULL;
    }
    else
	*newdshp = ChildDSH;		/* return new DSH */

    return(status);
}
