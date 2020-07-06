/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <st.h>
#include    <me.h>
#include    <iicommon.h>
#include    <dbdbms.h>
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
**  Name: QENHJOIN.C - hash table(s) used to join matching rows
**
**  Description:
**      The join keys of the outer source are used to build hash table(s)
**	probed by rows of inner source to determine matching rows.
**
**          qen_hjoin - hash join
**
**
**  History:    
**      1-mar-99 (inkdo01)
**	    Cloned from qenkjoin.
**	23-feb-04 (inkdo01)
**	    Changed qef_rcb->error to dsh->dsh_error for || thread safety.
**/

GLOBALREF QEF_S_CB  *Qef_s_cb;

/* Static variables, functions, etc. */

static VOID 
hjoin_stat_dump(QEF_RCB	*rcb,
	QEE_DSH		*dsh,
	QEN_NODE	*node,
	QEN_STATUS	*qen_status,
	QEN_HASH_BASE	*hbase);

static DB_STATUS
qen_hjoin_reset(QEF_RCB	*rcb,
	QEE_DSH		*dsh,
	QEN_HASH_BASE	*hbase,
	bool		clearbufptrs);

static void qen_hjoin_cleanup(
	QEF_RCB		*rcb,
	QEE_DSH		*dsh,
	QEN_HASH_BASE	*hbase);


/*{
** Name: QEN_HJOIN	- hash join
**
** Description:
**      The join keys of the outer source are used to build hash table(s)
**	probed by rows of inner source to determine matching rows.
**
**	Hash join proceeds in several phases:
**
**	Hash build:  read the build (outer) source and partition the
**	rows by hash key.  After the outer is entirely read, we attempt
**	to build a hash table in memory.  This may succeed, partially
**	succeed (some of the build spilled), or fail (all of the build
**	side spilled with no room to put any one partition).
**
**	Probe1:  the probe (inner) side is read and we decide whether
**	the partition it hashes to is in the hash table or not.  If not,
**	the row is buffered and possibly spilled.  If yes, we match the
**	row against the hash table to see if it joins.
**
**	P1P2: this interphase handles EOF on the probe side.  Left-join
**	rows are drained from the hash table (if any).  If any rows from
**	either side were spilled to disk, the previous hash table is
**	flushed and a new one set up.
**
**	Probe2:  this phase reads rows from the spilled "probe" partition(s)
**	corresponding to the current hash table, and joins them against
**	the "build" rows in the hash table.  (I quote "probe" and "build"
**	here, because during phase 2 role-reversal can occur such that
**	the "build" rows actually came from the inner, and "probe" rows
**	came from the outer.  Care is needed.)  After reading all the
**	relevant rows, if there are more spilled partitions around, we
**	flush the old hash table, make a new one, and repeat probe2
**	until the join is complete.
**
**	Outer joins are handled by tracking an indicator for each row
**	that shows whether the row ever joined.  (Obvious cases, such as
**	a probe that doesn't match any build, can be handled without the
**	indicator).  The indicator byte is at the end of the row.
**	When the build side is outer, after we're done with a hash table
**	we run through the rows looking for zero indicators;  these are
**	OJ rows.
**
**	When the probe side is outer, it's a bit more complicated.  In the
**	normal case there's no need to track an indicator byte, because
**	as soon as we're done probing with a row (and have no more matches)
**	we can use a simple NOJOIN flag to determine whether the probe row
**	is an OJ row.   We still maintain the indicator byte for the probe
**	side, though, because if we have to cart-prod a partition the
**	cart-prod logic runs the nested loops with the build side as the
**	outer-loop, and probe rows get re-read.
**
**	Note that the indicator byte is allocated for both probe and build
**	rows, because of the possibility of a cart-prod, and because you
**	never know when we might decide to do role reversal.
**
** Inputs:
**      node                            the hash join node data structure
**	rcb				request block
**	    .qef_cb.qef_dsh		the data segment header
**
** Outputs:
**      rcb
**	    .error.err_code		one of the following
**					E_QE0000_OK
**					E_QE0015_NO_MORE_ROWS
**					E_QE00A5_END_OF_PARTITION
**					E_QE0019_NON_INTERNAL_FAIL
**					E_QE0002_INTERNAL_ERROR
**					
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	All values in the appropriate row buffers have been set to that
**	which will produce the joined row.
**
** History:
**	1-mar-99 (inkdo01)
**	    Cloned from qenkjoin.
**	jan-00 (inkdo01)
**	    Changes for outer joins, cart prods.
**	1-may-01 (inkdo01)
**	    Removed MEfills that temporarily cleaned hash key areas in
**	   preparation for moving var-length strings.
**	7-sep-01 (inkdo01)
**	    Changed NOMATCH to NOJOIN to better reflect its purpose and
**	    added better OJ logic for right joins in probe1 phase.
**	29-oct-01 (inkdo01)
**	    Fix hsh_jcount computation.
**	14-mar-02 (inkdo01)
**	    Tidy up hash structures when join is re-opened (e.g. because
**	    it's under a QP node).
**	9-aug-02 (inkdo01)
**	    Handle OJ candidates that arise from phase II when probe row
**	    matches, but is rejected by OQUAL code (fix bug 108229).
**	2-jan-03 (inkdo01)
**	    Removed noadf_mask logic in favour of QEN_ADF * tests to fix
**	    concurrency bug.
**	22-july-03 (inkdo01)
**	    Init dmh_tran_id on first (or reset) call. This used to be done
**	    in qee.c, but that is too soon (tran ID may change before query
**	    execution). Fixes bug 110603.
**	20-nov-03 (inkdo01)
**	    Add support for "open" call to allow parallel query initialization.
**	29-dec-03 (inkdo01)
**	    DSH is now parameter, "function" replaces "reset".
**	2-jan-04 (inkdo01)
**	    Added end-of-partition-grouping logic.
**	27-feb-04 (inkdo01)
**	    Changed dsh_hash from array of QEN_HASH_BASE to array of ptrs.
**	17-mar-04 (toumi01)
**	    Add support for "close" call to allow parallel query termination.
**	8-apr-04 (inkdo01)
**	    Added dsh parm to QEF_CHECK_FOR_INTERRUPT macro.
**	19-apr-04 (inkdo01)
**	    Minor fix to partition group handling.
**	28-apr-04 (inkdo01)
**	    Add support for "end of partition group" call.
**	9-june-04 (inkdo01)
**	    Heuristic to read one inner row first, so we don't read
**	    all outer rows only to determine there are no inners to
**	    join to.
**	30-Jul-2004 (jenjo02)
**	    Use dsh_dmt_id rather than qef_dmt_id transaction context.
**	16-aug-04 (toumi01)
**	    free hash memory if we are done with it to ease pressure on qef
**	    memory, especially if we are running queries in parallel
**	28-aug-04 (inkdo01)
**	    Changes to use global base array to avoid excessive moving
**	    of build and probe rows.
**	20-Sep-2004 (jenjo02)
**	    Reset hsh_dmhcb on OPEN; with child threads, dsh_cbs
**	    aren't what they used to be when qee initialized them.
**	14-oct-04 (inkdo01)
**	    Tidy up some build/probe row logic - added more fields to xDEBUG
**	    TRdisplay.
**	02-Nov-2004 (jenjo02)
**	    After freeing hash stream, clear hsh_size lest we
**	    keep accumulating it and later think we've blown through
**	    qef_h_max.
**	04-Nov-2004 (jenjo02)
**	    Only CSswitch on root DSH or if not OS-threaded.
**	21-feb-05 (inkdo01)
**	    Fix assignment of build/probe row indicators for outer join
**	    detection, broken by earlier base array changes (fixes 113918).
**	6-May-2005 (schka24)
**	    Make hash values unsigned.  Fix a few improper (i2) casts on
**	    MEcopy sizes (a relic of yore, no longer correct).
**	17-Aug-2005 (schka24)
**	    Teach join to stop if PC-joining and one side gets a real EOF,
**	    and it's not an outer join.  (since orig now goes to substantial
**	    lengths to return a true EOF indication with partition qualified
**	    PC-joins, we might as well take advantage!)
**	    Mess with debugging displays a little to help me out with
**	    PC-joining.
**	10-Nov-2005 (schka24)
**	    Rename partitioning filter flag for clarity.
**	    Don't copy bogus data around in probe1 phase unless probe1
**	    actually got something.
**	    Fix wrong-answer bug with OJ and role reversal by eliminating
**	    the use of the indicator byte on the probe side, and repurposing
**	    the NOJOIN flag to mean "probe row hasn't joined yet".
**	    The prevpr fix from 6-Apr-05 was getting confused during role
**	    reversal, and this way is easier anyway.
**	    Simplify the probe-outer logic a little by moving some code
**	    around.
**	14-Dec-2005 (kschendel)
**	    Eliminate rcb from most utility routine calls (DSH suffices).
**	12-Jan-2006 (kschendel)
**	    Update qen-execute-cx calls.
**	    Smack around build, probe loops a bit more, mainly for cosmetics.
**	16-Feb-2006 (kschendel)
**	    Return after close, don't keep going.
**	10-oct-2006 (dougi)
**	    Assure that buildptr, probeptr start off right.
**	13-Feb-2007 (kschendel)
**	    Replace CSswitch with better cancel check.
*/

DB_STATUS
qen_hjoin(
QEN_NODE           *node,
QEF_RCB            *rcb,
QEE_DSH		   *dsh,
i4		    function )
{
    ADE_EXCB	    *ade_excb;
    QEF_CB	    *qef_cb = dsh->dsh_qefcb;
    QEE_XADDRS	    *node_xaddrs = dsh->dsh_xaddrs[node->qen_num];
    QEN_STATUS	    *qen_status = node_xaddrs->qex_status;
    QEN_NODE	    *out_node = node->node_qen.qen_hjoin.hjn_out;
    QEN_NODE	    *in_node = node->node_qen.qen_hjoin.hjn_inner;
    QEN_HASH_BASE   *hashptr = dsh->dsh_hash[node->
					node_qen.qen_hjoin.hjn_hash];
    char	    *buildptr = hashptr->hsh_brptr;
    char	    *probeptr = hashptr->hsh_prptr;
    char	    *bindptr, *pindptr;
    char	    toss_ind;		/* Dummy */
    DB_STATUS	    status = E_DB_OK;
    DB_STATUS	    locstat;
    i4	    	    offset;
    i4	    	    val1 = 0;
    i4	    	    val2 = 0;
    i4		    browsz = hashptr->hsh_browsz;
    i4		    prowsz = hashptr->hsh_prowsz;
    QEN_OJMASK	    ojmask = 0;
    bool	    reset = ((function & FUNC_RESET) != 0);
    bool	    out_reset = reset;
    bool	    in_reset = reset;
    bool	    load_outer = FALSE;
    bool	    filterflag;
    bool	    ojoin = FALSE;
    bool	    readinner;
    TIMERSTAT	    timerstat;


#ifdef xDEBUG
/*     (VOID) qe2_chk_qp(dsh); */
#endif

    /* Do open processing, if required. Only if this is the root node
    ** of the query tree do we continue executing the function. */
    if ((function & TOP_OPEN || function & MID_OPEN) && 
	!(qen_status->node_status_flags & QEN1_NODE_OPEN))
    {
	/* Ensure correct DMH_CB */
	hashptr->hsh_dmhcb = dsh->dsh_cbs[node->node_qen.qen_hjoin.hjn_dmhcb];
	buildptr = hashptr->hsh_brptr = 
		dsh->dsh_row[node->node_qen.qen_hjoin.hjn_brow];
	probeptr = hashptr->hsh_prptr = 
		dsh->dsh_row[node->node_qen.qen_hjoin.hjn_prow];
#ifdef xDEBUG
	TRdisplay("%@ HJOIN open node %d thread %d hbase %p pcount %d msize %u csize %u bvsize %d browsz %d prowsz %d\n",
		node->qen_num, dsh->dsh_threadno,
		hashptr, hashptr->hsh_pcount, hashptr->hsh_msize,
		hashptr->hsh_csize, hashptr->hsh_bvsize,
		hashptr->hsh_browsz, hashptr->hsh_prowsz);
#endif
	status = (*out_node->qen_func)(out_node, rcb, dsh, MID_OPEN);
	status = (*in_node->qen_func)(in_node, rcb, dsh, MID_OPEN);
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
	    status = (*out_node->qen_func)(out_node, rcb, dsh, FUNC_CLOSE);
	    status = (*in_node->qen_func)(in_node, rcb, dsh, FUNC_CLOSE);
	    qen_hjoin_cleanup(rcb, dsh, hashptr);
	    qen_status->node_status_flags = 
		(qen_status->node_status_flags & ~QEN1_NODE_OPEN) | QEN8_NODE_CLOSED;
	}
	return(E_DB_OK);
    }
    /* End of partition group call just gets passed down. */
    if (function & FUNC_EOGROUP)
    {
	status = (*out_node->qen_func)(out_node, rcb, dsh, FUNC_EOGROUP);
	status = (*in_node->qen_func)(in_node, rcb, dsh, FUNC_EOGROUP);
	return(E_DB_OK);
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

    dsh->dsh_error.err_code = E_QE0000_OK;
    if (reset || qen_status->node_status == QEN0_INITIAL)
	readinner = FALSE;
    else readinner = TRUE;

    if (node->node_qen.qen_hjoin.hjn_oj)
    {
	ojoin = TRUE;
	if (hashptr->hsh_flags & HASH_WANT_LEFT)
	    readinner = TRUE;	/* turn off inner read heuristic
				** for left joins (for now) */
    }

    /* Reset buildptr, probeptr to those of previous call if we're
    ** using global base arrays. */
    if (dsh->dsh_qp_ptr->qp_status & QEQP_GLOBAL_BASEARRAY)
    {
	buildptr = dsh->dsh_row[node->node_qen.qen_hjoin.hjn_brow];
	probeptr = dsh->dsh_row[node->node_qen.qen_hjoin.hjn_prow];
    }

    /*
    ** If forced reset, reset the node status.  This makes it go through
    ** the "first time" routine. Must also reset lots of stuff in the 
    ** hash structures. This can happen if the hash join is opened more
    ** than once in a plan execution - as when it sits under a QP node
    ** or for each grouping of a partition compatible join.
    */
loop_reset:
    if (reset)
    {
	qen_status->node_status = QEN0_INITIAL;
	status = qen_hjoin_reset(rcb, dsh, hashptr, FALSE);
	probeptr = hashptr->hsh_prptr;
	if (status != E_DB_OK)
	    goto errexit;
    }

    if(qen_status->node_status == QEN0_INITIAL)
    {
	DMH_CB	*dmhcb = (DMH_CB *)hashptr->hsh_dmhcb;

	dmhcb->dmh_tran_id = dsh->dsh_dmt_id;
	qen_status->node_access = 0;
	buildptr = hashptr->hsh_brptr;
	bindptr = &buildptr[browsz-1];
				/* make sure it is correct */
	load_outer = TRUE; 
	/* new_outer remains FALSE */
    }

    status = E_DB_OK;

new1stinner:
    if (!readinner && qen_status->node_status == QEN0_INITIAL)
    {
	/* Heuristic that checks inner for any rows before we read
	** from outer. This enables us to avoid reading the outer
	** source and loading hash table/spooling overflow to disk
	** when there are no inner rows to process anyway. 
	**
	** It only handles non-left joins for now, but left join
	** support will be added in the future. */

	status = (*in_node->qen_func)(in_node, rcb, dsh,
			(in_reset) ? FUNC_RESET : NO_FUNC);
	in_reset = FALSE;

	if (status != E_DB_OK)
	{
	    if (dsh->dsh_error.err_code == E_QE0015_NO_MORE_ROWS ||
		(dsh->dsh_error.err_code == E_QE00A5_END_OF_PARTITION &&
		 node->qen_flags & QEN_PART_SEPARATE))
	    {
#ifdef xDEBUG
    TRdisplay("hjoin: probe-trial node: %d error: %s by thread: %d rcount: %d brcount: %d prcount: %d jcount: %d\n",
	node->qen_num, (dsh->dsh_error.err_code == E_QE0015_NO_MORE_ROWS) ?
	"15" : "A5", dsh->dsh_threadno, qen_status->node_rcount,
	hashptr->hsh_brcount, hashptr->hsh_prcount, hashptr->hsh_jcount);
#endif
		/* If partition compatible join, set flags, force end of
		** group on build source and go to error exit to loop
		** on next group. */
		if (dsh->dsh_error.err_code == E_QE0015_NO_MORE_ROWS)
		    qen_status->node_status_flags |= QEN512_IPART_EOF;
		else
		{
		    qen_status->node_status_flags |= QEN4_IPART_END;
		    status = (*out_node->qen_func)(out_node, rcb, dsh, 
					FUNC_EOGROUP);
		    if (status != E_DB_OK)
		    {
			if (dsh->dsh_error.err_code == E_QE00A5_END_OF_PARTITION)
			    qen_status->node_status_flags |= QEN2_OPART_END;
			else if (dsh->dsh_error.err_code == E_QE0015_NO_MORE_ROWS)
			    qen_status->node_status_flags |= QEN256_OPART_EOF;
		    }
		}

		qen_status->node_u.node_join.node_inner_status = 
							QEN11_INNER_ENDS;
		/* If QE0015, this exits whole join. */
		goto errexit;
	    }
	    else if (dsh->dsh_error.err_code == E_QE00A5_END_OF_PARTITION)
	    {
		/* Not sure if this happens - but if non PC join, keep
		** looping back to read successive groups (I think we
		** won't see these. in_node should keep going on all
		** partitions). */
		in_reset = TRUE;
		goto new1stinner;
	    }
	    goto errexit;  /* else return error */ 
	}
    }

    if (load_outer)
    {
	ADE_EXCB	*bmat_excb;

	/* The build side is "outer" in the xaddrs struct */
	bmat_excb = node_xaddrs->qex_otmat;
	for ( ; ; )	/* big loop to drive load of build source */
	{
	    qen_status->node_status = QEN1_EXECUTED;
	    load_outer = FALSE;
    newouter:
	    status = (*out_node->qen_func)(out_node, rcb, dsh,
			(out_reset) ? FUNC_RESET : NO_FUNC);
	    out_reset = FALSE;
	    if (status != E_DB_OK)
		break;			/* Go figure it out */

	    /* Materialize the row into buffer (c/w hash key). */
	    *((u_i4 *)buildptr) = 0;	/* first, init hashno to 0 */

	    /* Used to MEfill key buffer prior to materialization. Trim 
	    ** function in ADF code obviates need to do this for varchar
	    ** key columns. And null check obviates need to do this for
	    ** nullable key columns (if it's null, the old buffer contents
	    ** aren't overlaid). */
	    status = qen_execute_cx(dsh, bmat_excb);
	    if (status != E_DB_OK)
		goto errexit;	/* if ADF error, we're dead */

	    *bindptr = 0x0;		/* init oj join indicator */

	    /* Got row from outer source - send it to hash table build. */
	    hashptr->hsh_brcount++;		/* incr build row count */
	    filterflag = FALSE;
	    status = qen_hash_partition(dsh, hashptr, buildptr, &filterflag);
	    if (status != E_DB_OK) goto errexit;
	}

	/* Analyze error exit out of above loop */
	if (dsh->dsh_error.err_code == E_QE0015_NO_MORE_ROWS ||
	    (dsh->dsh_error.err_code == E_QE00A5_END_OF_PARTITION &&
	     node->qen_flags & QEN_PART_SEPARATE))
	{
#ifdef xDEBUG
    TRdisplay("hjoin: build node: %d error: %s by thread: %d rcount: %d brcount: %d prcount: %d jcount: %d\n",
	node->qen_num, (dsh->dsh_error.err_code == E_QE0015_NO_MORE_ROWS) ?
	"15" : "A5", dsh->dsh_threadno, qen_status->node_rcount,
	hashptr->hsh_brcount, hashptr->hsh_prcount, hashptr->hsh_jcount);
#endif
	    /* EOF or EOG is the expected "error" */
	    if (dsh->dsh_error.err_code == E_QE00A5_END_OF_PARTITION)
		qen_status->node_status_flags |= QEN2_OPART_END;
	    else
		qen_status->node_status_flags |= QEN256_OPART_EOF;

	    qen_status->node_u.node_join.node_outer_status = QEN8_OUTER_EOF;
	    qen_status->node_u.node_join.node_inner_status = QEN13_PROBE_P1;
	    if (hashptr->hsh_brcount == 0)
	    {
		if(!(hashptr->hsh_flags & HASH_WANT_RIGHT))
		{
		    /* No build rows and no right/full join.
		    ** Send EOG to probe source if PC join, then quit.
		    */
		    if (node->qen_flags & QEN_PART_SEPARATE)
		    {
			status = (*in_node->qen_func)(in_node, rcb, dsh, 
				    FUNC_EOGROUP);
			if (status != E_DB_OK)
			{
			    if (dsh->dsh_error.err_code == E_QE00A5_END_OF_PARTITION)
				qen_status->node_status_flags |= QEN4_IPART_END;
			    else if (dsh->dsh_error.err_code == E_QE0015_NO_MORE_ROWS)
				qen_status->node_status_flags |= QEN512_IPART_EOF;
			}
		    }
		    goto errexit;
		}	/* no build rows & no right/full join */
		else 
		{
		    /* Have to process probes if right/full join */
		    /* Fall out of if's to process probes */
		    hashptr->hsh_flags |= HASH_NO_BUILDROWS;
		}
	    }
	    else
	    {
		/* Got build rows, this is the normal continuation.
		** Reset QEF's error code before doing hash build, then
		** build first (or only) hash table.
		*/
		dsh->dsh_error.err_code = E_QE0000_OK;
		status = qen_hash_build(dsh, hashptr);
		/* If we couldn't build a hash table, indicate that we
		** are partitioning probes in Phase1
		*/
		if (hashptr->hsh_flags & HASH_NOHASHT)
		    hashptr->hsh_flags |= HASH_ROLE_REVERSE;
		if (status != E_DB_OK) goto errexit;
	    }
	}
	else if (dsh->dsh_error.err_code == E_QE00A5_END_OF_PARTITION)
	{
	    /* No more rows in partitioning group - read from 
	    ** next group.  bmat_excb still set up.
	    */
	    out_reset = TRUE;
	    goto newouter;
	}
	else
	{
	    goto errexit;  /* else return error */ 
	}
    } /* if load-outer */



    for ( ; ; )		/* Biiiig loop to cover whole join process */
    {

	/* Build (outer) source has been passed and turned into one or more
	** partitions. One of these is in a memory resident hash table. 
	** Phase 1 of the probe operation reads the rows of the probe (inner)
	** source. If the row targets the memory resident (and hashed) 
	** partition, an attempt is made to join it now. Otherwise, it is 
	** written to a probe partition on disk matching the build partition 
	** it hashes to. */

	if (qen_status->node_u.node_join.node_inner_status == QEN13_PROBE_P1)
	{
	    ADE_EXCB	*pmat_excb;
	    bool	gotone;

	    /* The probe materialize CX is "inner" */
	    pmat_excb = node_xaddrs->qex_itmat;
	    for ( ; ; )	/* big loop until a match is made, or eof/eog */
	    {

		gotone = FALSE;
		if (hashptr->hsh_flags & HASH_PROBE_MATCH)
		    pindptr = &probeptr[prowsz-1];	/* Ensure it's set */
		else
		{
		    if (readinner)
		    {
			/* !readinner means the 1st probe row has been read and 
			** is in buffers waiting for processing. */
	newinner:
			status = (*in_node->qen_func)(in_node, rcb, dsh,
				(in_reset) ? FUNC_RESET : NO_FUNC);
			in_reset = FALSE;
			if (status != E_DB_OK)
			    break;		/* Go figure it out */
		    }

		    readinner = TRUE;	/* read all the rest */
		    /* Materialize the row into buffer (c/w hash key). */
		    *((u_i4 *)probeptr) = 0;	/* first, init hashno to 0 */
		    pindptr = &probeptr[prowsz-1];
		    status = qen_execute_cx(dsh, pmat_excb);
		    if (status != E_DB_OK)
			goto errexit;	/* if ADF error, we're dead */

		    hashptr->hsh_flags |= HASH_PROBE_NOJOIN;
		    *pindptr = 0x0;	/* init oj join indicator */

		    /* This is not executed because it depends on the setting
		    ** of the null indicator for the hash result, which doesn't
		    ** currently happen (but it'd be nice if it did!).
		    ** if (probeptr[sizeof(i4)] != 0x0 && hashptr->hsh_flags & HASH_WANT_RIGHT)
		    ** {
			** Hash key is null - automatic rjoin candidate. **
			** ojmask |= OJ_HAVE_RIGHT;
			** gotone = TRUE;
			** break;
		    ** } */

		    hashptr->hsh_prcount++;		/* incr probe row count */
		}

		if (hashptr->hsh_flags & HASH_NO_BUILDROWS) 
		{
		    /* no build rows - and right/full join. */
		    ojmask |= OJ_HAVE_RIGHT;
		    gotone = TRUE;
		    break;
		}
		else 
		{
		    /* Use probe (inner) row to probe build. */
		    status = qen_hash_probe1(dsh, hashptr, &buildptr, 
				probeptr, &ojmask, &gotone);
		    if (status != E_DB_OK) goto errexit;
		    if (gotone) 
		    {
			hashptr->hsh_jcount++;
			/* buildptr might be bogus if right-OJ (the only kind
			** that we can see during P1)
			*/
			if (ojmask != 0)
			{
			    buildptr = NULL;
			    bindptr = &toss_ind;	/* Dummy */
			}
			else
			{
			    if (dsh->dsh_qp_ptr->qp_status & QEQP_GLOBAL_BASEARRAY)
			    {
				/* With global base array, just update row ptr array. */
				dsh->dsh_row[node->node_qen.qen_hjoin.hjn_brow] = buildptr;
			    }
			    else
			    {
				/* Without global base array, copy to row buffer. */
				MEcopy(buildptr, browsz, hashptr->hsh_brptr);	
			    }
			    /* OJ build row match indicator addr. */
			    bindptr = &buildptr[browsz-1];
			}
			break;	/* Proceed with join */
		    }
		    /* !gotone, no match, try another */
		}
	    } /* P1 loop */

	    if (status != E_DB_OK)
	    {
		/* If we fell out with nonzero error status, must have
		** been EOF, EOG, or bad problem, see which
		*/
		if (dsh->dsh_error.err_code == E_QE0015_NO_MORE_ROWS ||
		    (dsh->dsh_error.err_code == E_QE00A5_END_OF_PARTITION &&
		     node->qen_flags & QEN_PART_SEPARATE))
		{
#ifdef xDEBUG
    TRdisplay("hjoin: probe node: %d error: %s by thread: %d rcount: %d brcount: %d prcount: %d jcount: %d\n",
	node->qen_num, (dsh->dsh_error.err_code == E_QE0015_NO_MORE_ROWS) ?
	"15" : "A5", dsh->dsh_threadno, qen_status->node_rcount,
	hashptr->hsh_brcount, hashptr->hsh_prcount, hashptr->hsh_jcount);
#endif
		    if (dsh->dsh_error.err_code == E_QE00A5_END_OF_PARTITION)
			qen_status->node_status_flags |= QEN4_IPART_END;
		    else
			qen_status->node_status_flags |= QEN512_IPART_EOF;

		    qen_status->node_u.node_join.node_inner_status = 
				QEN14_PROBE_P2;
		    /* "End" any in-hash or useless partitions.
		    ** Indicate that all probes are read, and
		    ** flush them out.
		    */
		    hashptr->hsh_flags |= HASH_BETWEEN_P1P2;
		    if (hashptr->hsh_prcount == 0)
			if (hashptr->hsh_brcount == 0 || 
			    !(hashptr->hsh_flags & HASH_WANT_LEFT)) 
							goto errexit;	
				/* no inner rows & no outers or
				** no left/full join => no join */
			else
			    hashptr->hsh_flags |= HASH_NO_PROBEROWS;
		    if (hashptr->hsh_flags & HASH_NO_BUILDROWS) 
			goto errexit;	/* right joins are done in
					** p1 calls for NO_BUILD */
		    if (hashptr->hsh_flags & HASH_WANT_LEFT && 
			!(hashptr->hsh_flags & HASH_NOHASHT))
		    {
			/* rescan hash table for unjoined outers. */
			hashptr->hsh_currbucket = -1;
			hashptr->hsh_flags |= HASH_BUILD_OJS;
		    }
		    else hashptr->hsh_flags |= HASH_NOHASHT;
				/* tell P2 to start another hasht */
		    dsh->dsh_error.err_code = 0;
				/* reset error for P2 */
		}
		else if (dsh->dsh_error.err_code == E_QE00A5_END_OF_PARTITION)
		{
		    /* No more rows in partitioning group - read from 
		    ** next group. pmat_excb still OK here */
		    in_reset = TRUE;
		    goto newinner;
		}
		else
		    goto errexit;  /* else return error */ 
	    } /* if status != ok */

	}   /* End of if probe phase 1 */

	/* Phase 2 probe occurs if the build source required partitioning
	** onto disk because it was too large to fit into a single 
	** memory resident hash table. This results in the probe source
	** also being partitioned onto disk. Phase 1 does the partitioning,
	** as well as the matching of probe rows which have joins in the
	** initial (partition 1, if you want) build source hash table.
	** Phase 2 reloads the remaining build source partitions into
	** a hash table, and hashes the rows of the corresponding probe
	** partitions into them, returning any joins found in the process. 
	** Phase 2 repeats building hash tables until everything has been
	** processed.
	**
	** Phase 2 also returns any right join candidates (probe rows
	** which didn't find a match) and left join candidates (leftover
	** build rows from current hash table), as required by the join 
	** type. */

	if (qen_status->node_u.node_join.node_inner_status == QEN14_PROBE_P2)
	{
	    /* If we're in P2 now, continue with previously matched row
	    ** (if PROBE_MATCH set), or fetch a fresh probe and match it up.
	    ** no for-loop required - each call should return a row.
	    */
	    status = qen_hash_probe2(dsh, hashptr, &buildptr, &probeptr, 
					&ojmask);
	    if (status != E_DB_OK) goto errexit;
	    /* Row bits are who knows where, so move to proper place */
	    if (dsh->dsh_qp_ptr->qp_status & QEQP_GLOBAL_BASEARRAY)
	    {
		/* With global base array, just update row ptr array. */
		dsh->dsh_row[node->node_qen.qen_hjoin.hjn_brow] = buildptr;
		dsh->dsh_row[node->node_qen.qen_hjoin.hjn_prow] = probeptr;
	    }
	    else
	    {
		/* Without global base array, copy to result buffers. */
		MEcopy(buildptr, browsz, hashptr->hsh_brptr);	
		MEcopy(probeptr, prowsz, hashptr->hsh_prptr);	
	    }
	    /* OJ build/probe row match indicator addrs. */
	    bindptr = &buildptr[browsz-1];
	    pindptr = &probeptr[prowsz-1];
	    if (ojmask == 0)
		hashptr->hsh_jcount++;
	}

	/* We really ought to have a row now. Do the rest of the join
	** stuff (ADF exprs, etc.) and either stay in big loop, or break
	** to return to caller. */

	/* Check for outer join rows returned. If left join, project
	** right source nulls. If right join, project left source
	** nulls. Otherwise execute remaining "on" clause. */
	if (hashptr->hsh_flags & (HASH_WANT_LEFT | HASH_WANT_RIGHT))
	{
	    if (ojmask == 0)
	    {
		/* Don't know about row yet, run the oqual.
		** (if no oqual, it's equivalent to TRUE, ie inner join).
		*/
 
		ade_excb = node_xaddrs->qex_onqual;
		if (status = qen_execute_cx(dsh, ade_excb))
		    goto errexit;
		if (ade_excb != NULL && ade_excb->excb_value != ADE_TRUE) 
		{
		    /* If row might have more matches in hash table,
		    ** just loop since it might join one of them.
		    ** (and if it doesn't after all, the prober will
		    ** announce outer-join if appropriate.)
		    */
		    if (hashptr->hsh_flags & HASH_PROBE_MATCH)
			continue;

		    /* Failed oqual and no more chances, if probe never
		    ** joined, then we can pronounce outer join.
		    */
		    if ((hashptr->hsh_flags & (HASH_ROLE_REVERSE | HASH_WANT_RIGHT | HASH_PROBE_NOJOIN))
			    == (HASH_WANT_RIGHT | HASH_PROBE_NOJOIN) )
			ojmask |= OJ_HAVE_RIGHT;
		    else if ((hashptr->hsh_flags & (HASH_ROLE_REVERSE | HASH_WANT_LEFT | HASH_PROBE_NOJOIN))
			    == (HASH_ROLE_REVERSE | HASH_WANT_LEFT | HASH_PROBE_NOJOIN) )
			ojmask |= OJ_HAVE_LEFT;
		    else
			continue;
		}
	    }
	    if (ojmask & OJ_HAVE_LEFT)
	    {
		if (status = qen_execute_cx(dsh, node_xaddrs->qex_lnull))
		    goto errexit;
	    }
	    else if (ojmask & OJ_HAVE_RIGHT)
	    {
		if (status = qen_execute_cx(dsh, node_xaddrs->qex_rnull))
		    goto errexit;
	    }
	    else 
	    {
		/* build and probe inner-joined */
		if (status = qen_execute_cx(dsh, node_xaddrs->qex_eqmat))
		    goto errexit;
		/* Set OJ indicators in hash table (to eliminate these guys as 
		** outer join candidates). */
		*bindptr = 0x1;
		*pindptr = 0x1;		/* In case cart-proding */
		hashptr->hsh_flags &= ~HASH_PROBE_NOJOIN;
	    }
	} /* if outer join */

	/* Post-qualify the row (e.g. for inequalities), toss the joined
	** row and keep looking if post-qual fails.
	*/
	ade_excb = node_xaddrs->qex_jqual;
	if (ade_excb == NULL)
	    break;
	if (status = qen_execute_cx(dsh, ade_excb))
	    goto errexit;
	if (ade_excb->excb_value == ADE_TRUE) break;	/* got a match */

    }	/* end of loop which finds joins */

    /* We get here with a successful join. Final tidying before it's
    ** returned to caller. First though, if existence only join, force
    ** read of next probe row. */
    if (node->node_qen.qen_hjoin.hjn_kuniq)
	hashptr->hsh_flags &= ~HASH_PROBE_MATCH;

qualified:
    status = qen_execute_cx(dsh, node_xaddrs->qex_fatts);
    if (status != E_DB_OK)
    {
#ifdef xDEBUG
	/* (VOID) qe2_chk_qp(dsh); */
#endif
	goto errexit;       
    }

#ifdef xDEBUG
/*    (VOID) qe2_chk_qp(dsh); */
#endif

errexit:

    if (status != E_DB_OK)
    {
	locstat = qen_hash_closeall(hashptr);
	if (ult_check_macro(&qef_cb->qef_trace, 93, &val1, &val2))
	    hjoin_stat_dump(rcb, dsh, node, qen_status, hashptr);
    }

    if ((dsh->dsh_error.err_code == E_QE0015_NO_MORE_ROWS ||
	dsh->dsh_error.err_code == E_QE00A5_END_OF_PARTITION) &&
	(qen_status->node_status_flags & 
			(QEN2_OPART_END | QEN4_IPART_END)))
    {
	/* One side got end-of-group, but before moving on, see if
	** we reached true EOF on some input (and don't need outer
	** join on the other side).  If that's the case, the
	** join is over.
	** "if (! (join over)) ... "
	*/
	if (! ( ((qen_status->node_status_flags & QEN256_OPART_EOF)
		&& ((hashptr->hsh_flags & HASH_WANT_RIGHT) == 0))
	      || ((qen_status->node_status_flags & QEN512_IPART_EOF)
		&& ((hashptr->hsh_flags & HASH_WANT_LEFT) == 0)) ) )
	{

#ifdef xDEBUG
	    TRdisplay("hjoin: recycle for next group node: %d by thread: %d rcount: %d brcount: %d prcount: %d jcount: %d\n",
	    node->qen_num, dsh->dsh_threadno, qen_status->node_rcount,
	    hashptr->hsh_brcount, hashptr->hsh_prcount, hashptr->hsh_jcount);
#endif

	    /* Restart using next partitioning group. */
	    load_outer = out_reset = in_reset = reset = TRUE;
	    qen_status->node_status_flags &=
			~(QEN2_OPART_END | QEN4_IPART_END);
	    if (hashptr->hsh_flags & HASH_WANT_LEFT)
		readinner = TRUE;
	    else readinner = FALSE;		/* reset inner read heuristic */
	    goto loop_reset;
	}
	/* We want to stop the join, set things for cleanup below */
	status = E_DB_WARN;
	dsh->dsh_error.err_code = E_QE0015_NO_MORE_ROWS;
    }

    /*
    ** free hash memory if we are done with it to ease pressure on qef
    ** memory, especially if we are running queries in parallel
    */
    if (status == E_DB_WARN && dsh->dsh_error.err_code == E_QE0015_NO_MORE_ROWS)
    {
	qen_hjoin_cleanup(rcb, dsh, hashptr);
    }

    if (ult_check_macro(&qef_cb->qef_trace, 90, &val1, &val2))
    {
	DB_STATUS   cost_status;

	cost_status = qen_ecost_end(dsh, &timerstat, qen_status);
	if (DB_FAILURE_MACRO(cost_status))
	{
	    return(cost_status);
	}
    }

    /* Increment the count of rows that this node has returned */
    if (status == E_DB_OK)
    {
	qen_status->node_rcount++;
    }

    /* print tracing information DO NOT xDEBUG THIS */
    if (status == E_DB_OK &&
	    node->qen_prow != NULL &&
	    (ult_check_macro(&qef_cb->qef_trace, 100+node->qen_num,
				&val1, &val2) ||
		ult_check_macro(&qef_cb->qef_trace, 99,
				&val1, &val2)
	    )
	)
    {
	if (status == E_DB_OK)
	{
	    status = qen_print_row(node, rcb, dsh);
	}
    }

    return (status);
}


/*{
** Name: HJOIN_STAT_DUMP	- dumps some hash join stats per qe93
**
** Description:
**      Implements tracepoint qe93 by dumping selected information
**	concerning a hash join. 
**
** Inputs:
**      node                            the hash join node data structure
**	rcb				request block
**	    .qef_cb.qef_dsh		the data segment header
**	hbase				the hash join control structure
**
** Outputs:
**      none
**					
**	Returns:
**	    nothing
**	Exceptions:
**	    none
**
** History:
**	jan-00 (inkdo01)
**	    Written.
**	25-oct-01 (inkdo01)
**	    Added "known" row count to display.
**	22-Jun-2005 (schka24)
**	    Add more stuff to stats output.
*/
static VOID 
hjoin_stat_dump(QEF_RCB	*rcb,
	QEE_DSH		*dsh,
	QEN_NODE	*node,
	QEN_STATUS	*qen_status,
	QEN_HASH_BASE	*hbase)

{
    QEF_CB	    *qef_cb = dsh->dsh_qefcb;
    char	    *cbuf = qef_cb->qef_trfmt;
    i4		    cbufsize = qef_cb->qef_trsize;


    STprintf(cbuf, "Hash join summary for node %d thread %d\n",
		node->qen_num, dsh->dsh_threadno);
    qec_tprintf(rcb, cbufsize, cbuf);
    STprintf(cbuf, "Mem guess: %d, actual: %d;  buffer size: %d, # partitions: %d\n",
	hbase->hsh_msize, hbase->hsh_size, hbase->hsh_csize, hbase->hsh_pcount);
    qec_tprintf(rcb, cbufsize, cbuf);
    STprintf(cbuf, "Build row count: %d, probe row count: %d, result row count: %d\n",
	hbase->hsh_brcount, hbase->hsh_prcount, qen_status->node_rcount);
    qec_tprintf(rcb, cbufsize, cbuf);
    STprintf(cbuf, "Joined row count: %d, file count: %d, cartprod count: %d\n",
	hbase->hsh_jcount, hbase->hsh_fcount, hbase->hsh_ccount);
    qec_tprintf(rcb, cbufsize, cbuf);
    STprintf(cbuf, "Max. recursion level: %d, filter discards: %d\n",
	hbase->hsh_maxreclvl, hbase->hsh_bvdcnt);
    qec_tprintf(rcb, cbufsize, cbuf);
    STprintf(cbuf, "Build kb spilled: %d, probe kb spilled: %d\n",
	hbase->hsh_bwritek, hbase->hsh_pwritek);
    qec_tprintf(rcb, cbufsize, cbuf);
    STprintf(cbuf, "Repartitions: %d, unspilled ht-build partitions: %d\n\n",
	hbase->hsh_rpcount, hbase->hsh_memparts);
    qec_tprintf(rcb, cbufsize, cbuf);

}


/*{
** Name: QEN_HJOIN_RESET	- reset hash structures upon recall
**
** Description:
**      When hash join is opened multiple times in a plan execution
**	(as when it is under a QP node), must reset various ptrs, counters,
**	flags for it to execute properly. We avoid reallocation of memory, 
**	though.
**
** Inputs:
**	rcb				request block
**	    .qef_cb.qef_dsh		the data segment header
**	hbase				the hash join control structure
**	clearbufptrs			TRUE if hash stream will be released,
**					clear out pointers.
**
** Outputs:
**      none
**					
**	Returns:
**	    nothing
**	Exceptions:
**	    none
**
** History:
**	14-mar-02 (inkdo01)
**	    Added "known" row count to display.
**	30-Jul-2004 (jenjo02)
**	    Use dsh_dmt_id rather than qef_dmt_id transaction context.
**	30-aug-04 (inkdo01)
**	    Add flag to clear ptrs after closestream.
**	6-apr-05 (inkdo01)
**	    Init. hsh_prevpr.
**	13-Jun-2005 (schka24)
**	    Got rid of first-build flag.
**	14-Nov-2005 (schka24)
**	    Remove prevpr, fixed differently.
*/
static DB_STATUS
qen_hjoin_reset(QEF_RCB	*rcb,
	QEE_DSH		*dsh,
	QEN_HASH_BASE	*hbase,
	bool		clearbufptrs)

{
    QEF_CB	    *qef_cb = dsh->dsh_qefcb;
    DMH_CB	    *dmhcb;
    QEN_HASH_LINK   *hlnk;
    QEN_HASH_PART   *hpart;

    DB_STATUS	    status = E_DB_OK;
    i4		    j;


    /* Tidy up the hash structure. */
    if (clearbufptrs)
	hbase->hsh_htable = NULL;
    /* (don't clean file pointers, closeall will) */
    dsh->dsh_row[hbase->hsh_nodep->hjn_brow] = hbase->hsh_brptr;
				/* reset default buffer ptrs */
    dsh->dsh_row[hbase->hsh_nodep->hjn_prow] = hbase->hsh_prptr;
    hbase->hsh_cartprod = NULL;
    hbase->hsh_firstbr = NULL;
    hbase->hsh_nextbr = NULL;
    hbase->hsh_jcount = 0;
    hbase->hsh_ccount = 0;
    hbase->hsh_fcount = 0;
    hbase->hsh_maxreclvl = 0;
    hbase->hsh_bvdcnt = 0;
    hbase->hsh_bwritek = 0;
    hbase->hsh_pwritek = 0;
    hbase->hsh_memparts = 0;
    hbase->hsh_rpcount = 0;
    hbase->hsh_lastloc = -1;
    hbase->hsh_currpart = -1;
    hbase->hsh_brcount = 0;
    hbase->hsh_prcount = 0;
    hbase->hsh_flags &= (HASH_WANT_LEFT | HASH_WANT_RIGHT
			| HASH_CHECKNULL); 
				/* drop all but OJ & null check flags */
    hbase->hsh_currows_left = 0;
    hbase->hsh_currbucket = 0;
    hbase->hsh_count1 = 0;
    hbase->hsh_count2 = 0;
    hbase->hsh_count3 = 0;
    hbase->hsh_count4 = 0;
    dmhcb = (DMH_CB *)hbase->hsh_dmhcb;	/* load DMH_CB for later */

    /* Loop to init LINK structures. */
    for (hlnk = hbase->hsh_toplink; hlnk; hlnk = hlnk->hsl_next)
    {
	hlnk->hsl_brcount = 0;
	hlnk->hsl_prcount = 0;
	hlnk->hsl_pno = -1;
	hlnk->hsl_flags = 0;

	/* And re-init the partition descriptors. */
	for (j = 0; j < hbase->hsh_pcount; j++)
	{
	    hpart = &hlnk->hsl_partition[j];

	    hpart->hsp_bfile = (QEN_HASH_FILE *) NULL;
	    hpart->hsp_pfile = (QEN_HASH_FILE *) NULL;
	    hpart->hsp_offset = 0;
	    hpart->hsp_brcount = 0;
	    hpart->hsp_prcount = 0;
	    hpart->hsp_currbn = 0;
	    hpart->hsp_reclevel = 0;
	    hpart->hsp_flags  = (HSP_BALLSAME | HSP_PALLSAME);
	    if (clearbufptrs)
	    {
		hpart->hsp_bptr = NULL;
		hpart->hsp_bvptr = NULL;
	    }
	}
	/* No point in cleaning lower levels if releasing the memory
	** stream, they'll all just disappear
	*/
	if (clearbufptrs)
	    break;
    }
    /* The topmost one sticks around, clean it if releasing memory */
    if (clearbufptrs && hbase->hsh_toplink != NULL)
	hbase->hsh_toplink->hsl_next = NULL;

    /* Cleanup the files (cleans hbase file lists too). */
    if (dmhcb && dmhcb->dmh_locidxp)
    {
	status = qen_hash_closeall(hbase);
	if (clearbufptrs)
	    hbase->hsh_availfiles = NULL;
    }
    if (status == E_DB_OK)
    {
	/* Tidy up some stuff in the hash join's DMH_CB. */
	dmhcb->dmh_file_context = (PTR) NULL;
	dmhcb->dmh_buffer = (PTR) NULL;
	dmhcb->dmh_locidxp = (i4 *) NULL;
	dmhcb->dmh_numlocs = 0;
	dmhcb->dmh_rbn = 0;
	dmhcb->dmh_func = 0;
	dmhcb->dmh_flags_mask = 0;
	dmhcb->dmh_db_id = rcb->qef_db_id;
	dmhcb->dmh_tran_id = dsh->dsh_dmt_id;
    }
    return(status);

}

/* Name: qen_hjoin_cleanup - clean up completed hash join
**
** Description:
**
**	After a hash join is entirely completed, and won't be needed
**	any more, this routine is called to clean it up.  We'll reset
**	the hash join (which deletes all the temp files as well as
**	cleaning up data structures), and free the hash memory stream.
**
** Inputs:
**	qef_rcb			QEF request control block
**	dsh			dsh for this (possibly child) execution
**	hbase			Hash join control block
**
** Outputs:
**	None
**
** History:
**	27-May-2005, schka24
**	    Written.
*/

static void
qen_hjoin_cleanup(QEF_RCB *qef_rcb,
	QEE_DSH *dsh, QEN_HASH_BASE *hbase)

{
    DB_STATUS	status;
    i4		err;
    QEF_CB	*qefcb;
    ULM_RCB	ulm;

    qefcb = dsh->dsh_qefcb;
    status = qen_hjoin_reset(qef_rcb, dsh, hbase, TRUE);
    /* Don't care if it fails */
    if (hbase->hsh_streamid != (PTR)NULL)
    {

	STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_s_ulmcb, ulm);
	ulm.ulm_streamid_p = &hbase->hsh_streamid;
	if (ulm_closestream(&ulm) == E_DB_OK)
	{
	    /* ULM has nullified hsh_streamid */
	    /* Adjust hash used for the session. */
	    CSadjust_counter(&qefcb->qef_h_used, -hbase->hsh_size);
	    hbase->hsh_size = 0;
	}
    }
} /* qen_hjoin_cleanup */
