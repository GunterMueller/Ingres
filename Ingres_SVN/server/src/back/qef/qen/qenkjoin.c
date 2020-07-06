/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <bt.h>
#include    <gl.h>
#include    <cs.h>
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
**  Name: QENKJOIN.C - key lookup on inner join
**
**  Description:
**      The attributes from the outer node can
**  be used to key/access the inner relation. 
**
**          qen_kjoin - key join
**
**
**  History:    $Log-for RCS$
**      8-apr-86 (daved)    
**          written
**	30-jul-86 (daved)
**	    updated once more information was known about how the system
**	    would work.
**	24-jul-87 (daved)
**	    added qen_pjoin as a special case of kjoin. pjoin has no keys,
**	    a scan is used instead.
**	02-feb-88 (puree)
**	    add reset flag to qen_kjoin function arguments.
**	03-aug-88 (puree)
**	    fix error in ult_check_macro call.
**	28-sep-89 (eric)
**	    Incremented qen_status->node_rcount to keep track of how many
**	    rows this node returns. This will be used to fix the Sybase bug.
**	29-nov-89 (davebf)
**	    modified for outer join support
**	11-mar-92 (jhahn)
**	    Added support for QEF interrupts.
**	    Added call to CSswitch.
**	01-apr-92 (rickh)
**	    De-supported right joins inside kjoins.  We will support this
**	    only when DMF supports a "physical scan" of a table that returns
**	    tuples in TID order.
**	08-dec-92 (anitap)
**	    Included <qsf.h> and <psfparse.h> for CREATE SCHEMA.
**	02-apr-93 (rickh)
**	    When an inner join condition occurs, remember it.
**      14-sep-93 (smc)
**          Moved <cs.h> for CS_SID.
**	31-aug-93 (rickh)
**	    If an outer tuple inner joined with an inner tuple but failed
**	    the ON clause for the next inner tuple, we were emitting the
**	    outer tuple as both an inner and outer join.
**	18-oct-93 (rickh)
**	    Converted node_access to a flag array.  Added new flag to indicate
**	    when the last outer tuple successfully keyed into the inner
**	    relation.  This allows us to fix a bug:  we used to get an
**	    "unpositioned stream" error from DMF when trying to reposition a
**	    BTREE on a previous outer value which failed to probe in the first
**	    place.
**
**	    Also, moved the inner tuple materializer higher up so that the
**	    inner tuple would be available for the KQUAL and OQUAL.
**      01-nov-95 (ramra01)
**          Prior to calling qen_u21_execute_cx confirm the presence of
**          a adf control block to execute
**	7-nov-95 (inkdo01)
**	    Changes to replace QEN_ADF and QEN_OJINFO structure instances by
**	    pointers in QEN_KJOIN.
**	12-jun-96 (inkdo01)
**	    Support null detection in Rtree spatial key joins.
**	05-feb-97 (kch)
**	    In qen_kjoin(), we now increment the counter of the rows that the
**	    node has returned. This change fixes bug 78836.
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          Set DMR_SORT flag if getting tuples to append them to a sorter.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	23-feb-04 (inkdo01)
**	    Changed qef_rcb->error to dsh->dsh_error for || thread safety.
**	14-Jul-2004 (schka24)
**	    First cut at implementing partitioned inners.  For now,
**	    there's no support for using the struct-compat flag in the
**	    partdef to narrow the partition search - we just look at all
**	    partitions.
**	28-Dec-2005 (kschendel)
**	    Use DSH references whenever possible instead of QEF RCB
**	    references.  (This simplifies the code, eliminating many
**	    RCB references, and reflects the new primacy of the DSH
**	    thanks to the parallel query changes.)
**	13-Jan-2006 (kschendel)
**	    Revise CX execution calls.
**/


GLOBALREF QEF_S_CB	*Qef_s_cb;


/*	static functions	*/

static DB_STATUS kjoin_inner(
	QEF_RCB *rcb,
	QEE_DSH *dsh,
	QEN_NODE *node,
	QEN_STATUS *qen_status,
	bool read_outer);


/*{
** Name: QEN_KJOIN	- key join
**
** Description:
**      The attributes from the outer node is used to key the inner relation. 
**
** Inputs:
**      node                            the keyed join node data structure
**	rcb				request block
**	dsh				Query fragment data segment header
**	func				Function (open, close, etc)
**
** Outputs:
**      dsh
**	    .dsh_error.err_code		one of the following
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
**	8-apr-86 (daved)
**          written
**	02-feb-88 (puree)
**	    add reset flag to the function arguments.
**	03-aug-88 (puree)
**	    fix error in ult_check_macro call.
**	28-sep-89 (eric)
**	    Incremented qen_status->node_rcount to keep track of how many
**	    rows this node returns. This will be used to fix the Sybase bug.
**	29-nov-89 (davebf)
**	    modified for outer join support
**	11-mar-92 (jhahn)
**	    Added support for QEF interrupts.
**	    Added call to CSswitch.
**	01-apr-92 (rickh)
**	    De-supported right joins inside kjoins.  We will support this
**	    only when DMF supports a "physical scan" of a table that returns
**	    tuples in TID order.
**	02-apr-93 (rickh)
**	    When an inner join condition occurs, remember it.
**	14-apr-93 (rickh)
**	    An assumption was made that if the last outer tuple created a
**	    left join condition, then its successor will too.  But this
**	    is not true if there are non-key clauses in the OQUAL.  Bogus
**	    assumption removed.  Also removed the test for new_outer, which
**	    looked really wrong.
**      08-apr-93 (smc)
**          Moved declaration of qen_kjoin() to clear compiler warnings.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	18-oct-93 (rickh)
**	    Converted node_access to a flag array.  Added new flag to indicate
**	    when the last outer tuple successfully keyed into the inner
**	    relation.  This allows us to fix a bug:  we used to get an
**	    "unpositioned stream" error from DMF when trying to reposition a
**	    BTREE on a previous outer value which failed to probe in the first
**	    place.
**
**	    Also, moved the inner tuple materializer higher up so that the
**	    inner tuple would be available for the KQUAL and OQUAL.
**
**	    Also, go get another inner if the OQUAL fails.  You can't emit
**	    the outer tuple as a LEFT JOIN until you know that it doesn't
**	    inner join with ANY inner tuple.
**      01-nov-95 (ramra01)
**          Prior to calling qen_u21_execute_cx confirm the presence of
**          a adf control block to execute
**	7-nov-95 (inkdo01)
**	    Changes to replace QEN_ADF and QEN_OJINFO structure instances by
**	    pointers in QEN_KJOIN.
**	12-jun-96 (inkdo01)
**	    Support null detection in Rtree spatial key joins.
**	05-feb-97 (kch)
**	    We now increment the counter of the rows that the node has
**	    returned, qen_status->node_rcount. We ignore the first call to
**	    this function in oder to give the exact row count. This change
**	    fixes bug 78836.
**      14-may-97 (dilma04)
**          Cursor Stability Project:
**          Set DMR_SORT flag if getting tuples to append them to a sorter.
**	28-may-97 (inkdo01)
**	    Fix logic for handling TID qualifications. If kjoin_tqual, 
**	    execute the kjoin_iqual OUTSIDE of DMF.
**	12-nov-98 (inkdo01)
**	    Added support of base table index only access (Btree).
**	20-may-99 (hayke02)
**	    We now do not call qen_u21_execute_cx(QEN_22KJN_KQUAL,&xcxcb)
**	    and only set xcxcb.excb_value TRUE if KJN22_KQUAL is set in
**	    qen_kjoin.noadf_mask. KJN22_KQUAL will be set in qeq_validate()
**	    if a NULL kjoin_kqual is found - this will now be the case
**	    for non-outer and non-spatial key joins (see opc_ijkjoin()).
**	    This change fixes bug 96840.
**      12-Oct-2001 (hanal04) Bug 105231 INGSRV 1495.
**          If DMR_GET fails with E_QE0025_USER_ERROR do not overwrite
**          dsh->dsh_error.err_code with the dmr_cb->error.err_code. The
**          will already have been handled in qef_adf_error() and
**          qef_error() will report 'alarming' E_DM008A and E_QE007C
**          errors when the underlying error is a USER error.
**	2-jan-03 (inkdo01)
**	    Removed noadf_mask logic in favour of QEN_ADF * tests to fix
**	    concurrency bug.
**	20-nov-03 (inkdo01)
**	    Add support for "open" call to allow parallel query initialization.
**	29-dec-03 (inkdo01)
**	    DSH is now parameter, "function" replaces "reset".
**	2-jan-04 (inkdo01)
**	    Added end-of-partition-grouping logic.
**	3-feb-04 (inkdo01)
**	    Changed TIDs to TID8s.
**	27-feb-04 (inkdo01)
**	    Changed dsh ptr to array of QEN_STATUS structs to ptr to array
**	    of ptrs.
**	12-mar-04 (my daughter's birthday) (inkdo01)
**	    Fill in dmr_q_arg with this thread's dsh addr.
**	17-mar-04 (toumi01)
**	    Add support for "close" call to allow parallel query termination.
**	19-mar-04 (inkdo01)
**	    Fix TID manipulation for byte swapped machines.
**	8-apr-04 (inkdo01)
**	    Added dsh parm to QEF_CHECK_FOR_INTERRUPT macro.
**	13-Jul-2004 (schka24)
**	    Added partitioned-inner support (first cut).
**	    Cleaned out remnants of never-implemented right join code,
**	    right kjoin is just impractical IMHO.  Cleaned out other
**	    dead code (new_outer set but never tested, etc).
**	    Test/set the "this key won't probe" flag from the same place
**	    at all times (!) (node-access, not node-status).
**	10-Aug-2004 (jenjo02)
**	    KJOIN under a Child thread needs its tables opened now.
**	04-Nov-2004 (jenjo02)
**	    Only CSswitch on root DSH or if not OS-threaded.
**	13-Dec-2005 (kschendel)
**	    Double-check that there's an iqual before asking DMF to
**	    qualify against it.  (probably always an iqual, except maybe
**	    for spatial.)
**	25-Feb-2006 (kschendel)
**	    Always return after doing a FUNC_CLOSE.
**	13-Feb-2007 (kschendel)
**	    Replace CSswitch with better cancel check.
**    25-march-2008 (dougi) bug 120154
**        Change nullind_ptr to u_char* from bool*.
*/

DB_STATUS
qen_kjoin(
QEN_NODE           *node,
QEF_RCB            *rcb,
QEE_DSH		   *dsh,
i4		    function )
{
    ADE_EXCB	    *ade_excb;
    ADE_EXCB	    *jqual_excb;
    QEF_CB	    *qef_cb = dsh->dsh_qefcb;
    PTR		    *cbs = dsh->dsh_cbs;
    QEE_XADDRS	    *node_xaddrs = dsh->dsh_xaddrs[node->qen_num];
    QEN_STATUS	    *qen_status = node_xaddrs->qex_status;
    DMR_CB	    *dmr_cb;
    QEN_NODE	    *out_node = node->node_qen.qen_kjoin.kjoin_out;
    QEN_OJINFO	    *ojoin = node->node_qen.qen_kjoin.kjoin_oj;
    QEN_PART_INFO   *partp;		/* Partitioning info if any */
    DB_STATUS	    status = E_DB_OK;
    STATUS	    st;
    bool	    reset = ((function & FUNC_RESET) != 0);
    i4		    out_func = NO_FUNC;
    i4	    val1 = 0;
    i4	    val2 = 0;
    i4		    cmp_value;
    bool	    read_outer;		/* Need to / have just read an outer */
    TIMERSTAT	    timerstat;
    u_char	    *nullind_ptr;


#ifdef xDEBUG
    (VOID) qe2_chk_qp(dsh);
#endif

    /* Do open processing, if required. Only if this is the root node
    ** of the query tree do we continue executing the function. */
    if ((function & TOP_OPEN || function & MID_OPEN) && 
	!(qen_status->node_status_flags & QEN1_NODE_OPEN))
    {
	/* "Open" the kjoin node.
	** For partitioned inner, allocate memory for a key nomatch
	** bitmap.  (This memory could be allocated for the node in qee,
	** but there's no real convenient place to point the pointer except
	** in the node status entry;  and qee bulk-clears all node statuses
	** at query start, which wouldn't fly.  It's easier to allocate here.)
	** For a nonpartitioned inner, set the DMR_GET qualification
	** function since we won't be farting with partition dmrcb's.
	*/

	partp = node->node_qen.qen_kjoin.kjoin_part;
	if (partp != NULL)
	{
	    ULM_RCB ulm;

	    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_d_ulmcb, ulm);
	    ulm.ulm_streamid = dsh->dsh_qstreamid;
	    ulm.ulm_psize = ((u_i4)partp->part_totparts + BITSPERBYTE-1) / BITSPERBYTE;
	    if (ulm.ulm_streamid == NULL)
	    {
		/* Open the stream if we're the first */
		ulm.ulm_flags = ULM_OPEN_AND_PALLOC;
		status = qec_mopen(&ulm);
		if (status == E_DB_OK)
		    dsh->dsh_qstreamid = ulm.ulm_streamid;
	    }
	    else
	    {
		status = qec_malloc(&ulm);
	    }
	    if (status != E_DB_OK)
	    {
		dsh->dsh_error.err_code = ulm.ulm_error.err_code;
		return (status);
	    }
	    
	    qen_status->node_u.node_kjoin.node_nomatch_bits = (PTR) ulm.ulm_pptr;
	}
	else
	{
	    qen_status->node_u.node_kjoin.node_nomatch_bits = NULL;

	    /* Prepare unpartitioned dmr_cb with proper DSH parameter. */

	    dmr_cb = (DMR_CB *) cbs[node->node_qen.qen_kjoin.kjoin_get];

	    if ( dmr_cb->dmr_access_id == NULL && dsh != dsh->dsh_root )
	    {
		/*
		** Then this is a Child thread that needs
		** its table opened.
		*/
		status = qeq_part_open (rcb, dsh->dsh_act_ptr, dsh,
			    dsh->dsh_row[node->qen_row],
			    dsh->dsh_qp_ptr->qp_row_len[node->qen_row],
				node->node_qen.qen_kjoin.kjoin_get,
				(i4)-1, (u_i2)0);
		if (status != E_DB_OK)
		    return (status);
	    }

	    /* Run inner qual (if any) from DMF unless there's tid poop */
	    if (node->node_qen.qen_kjoin.kjoin_iqual != NULL
	      && ! node->node_qen.qen_kjoin.kjoin_tqual)
	    {
		dmr_cb->dmr_q_fcn = (PTR) qen_dmr_cx;
		dmr_cb->dmr_q_arg = (PTR)dsh;
	    }
	    else
	    {
		dmr_cb->dmr_q_fcn = (PTR) NULL;
		dmr_cb->dmr_q_arg = (PTR) NULL;
	    }
	}

	/* Pass open request to our children */
	status = (*out_node->qen_func)(out_node, rcb, dsh, MID_OPEN);
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
	    /* Pass close to (left) child node */
	    status = (*out_node->qen_func)(out_node, rcb, dsh, FUNC_CLOSE);
	    qen_status->node_status_flags = 
		(qen_status->node_status_flags & ~QEN1_NODE_OPEN) | QEN8_NODE_CLOSED;
	}
	return(E_DB_OK);
    }

    if (ojoin != NULL)
    {
	/* Only left joins supported */
    	if (ojoin->oj_jntype != DB_LEFT_JOIN)
	{
	    dsh->dsh_error.err_code = E_QE0002_INTERNAL_ERROR;
	    return (E_DB_ERROR);
	}
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
    jqual_excb = node_xaddrs->qex_jqual;

    /*
    ** If forced reset, reset the node status.  This makes it go through
    ** the "first time" routine.
    */
    if (reset)
    {
	qen_status->node_status = QEN0_INITIAL;
	out_func = FUNC_RESET;
    }

    /* Need a fresh outer row if first time, or if join-state bits say so */
    read_outer = FALSE;
    if(qen_status->node_status == QEN0_INITIAL)
    {
	qen_status->node_access = 0;
	read_outer = TRUE; 
	/* Leave initial status for below */
    }
    else if ((qen_status->node_access & (QEN31_IJ_OUTER | QEN32_OJ_OUTER)) != 0)
    {
	read_outer = TRUE;
    }

    status = E_DB_OK;

    /* Locate null indicator copied from nullable outers in spatial
    ** key joins. */
    if (node->node_qen.qen_kjoin.kjoin_nloff >= 0)
	nullind_ptr = dsh->dsh_row[node->node_qen.qen_kjoin.kjoin_key->
	    qen_output] + node->node_qen.qen_kjoin.kjoin_nloff;
    else nullind_ptr = NULL;

    if (read_outer)
    {
newouter:
	/* Get an outer, looping if we ran off the end of an outer
	** partition group.
	** (currently notimp)
	*/
	status = (*out_node->qen_func)(out_node, rcb, dsh, out_func);
	out_func = NO_FUNC;
	if (status != E_DB_OK)
	{
	    if (dsh->dsh_error.err_code == E_QE0015_NO_MORE_ROWS)
	    {
		status = E_DB_WARN;
		goto errexit;
	    }
	    if (dsh->dsh_error.err_code == E_QE00A5_END_OF_PARTITION &&
		!(node->qen_flags & QEN_PART_SEPARATE))
	    {
		/* End of partitioning group - immediately start reading
		** the next or pass 00A5 up the plan tree.
		** *Note* as of Jul '04 we don't use partitioning groups
		** for kjoins, so you don't really get here.
		*/
		out_func = FUNC_RESET;
		goto newouter;
	    }
	    goto errexit;  /* else return error */ 
	}
	read_outer = TRUE;		/* Just read a new outer */

	/* if the first time through the loop (or forced reset) */
	if (qen_status->node_status == QEN0_INITIAL)
	{
	    cmp_value = ADE_NOT_TRUE;
	    qen_status->node_status = QEN1_EXECUTED; /* show not first*/
	}
	else
	{
	    /* compare to previous key value */
	    cmp_value = ADE_TRUE;
	    ade_excb = node_xaddrs->qex_kcompare;
	    if (ade_excb != NULL)
	    {
		status = qen_execute_cx(dsh, ade_excb);
		if (status != E_DB_OK)
		    goto errexit;
		cmp_value = ade_excb->excb_value;
	    }
	}
	if (cmp_value != ADE_TRUE)  /* not equal */
	{
	    /*
	    ** New key.  (or first time/reset.)
	    ** We can now forget whether the last tuple keyed
	    ** into the inner relation.
	    */
	    qen_status->node_access &= ~QEN_THIS_KEY_WONT_PROBE;

	    /* now materialize and prepare the new outer key to be used
	    ** in DMR_POSITION */

	    status = qen_execute_cx(dsh, node_xaddrs->qex_keyprep);
	    if (status != E_DB_OK)
		goto errexit;   /* if ade error, return error */
    
	    /* check if this was a null outer column. 
	    */
	    if (nullind_ptr && *nullind_ptr)
		qen_status->node_access |= QEN_THIS_KEY_WONT_PROBE;

	    qen_status->node_access |= QEN_OUTER_NEW_KEY;
	}
	else
	{
	    /* 
	    ** We come here if the new outer key is the same
	    ** as the old one.
	    */

	    qen_status->node_access &= ~QEN_OUTER_NEW_KEY;
	}

	/* indicate a new outer has been read  with no LJ or IJ
	** returned yet */
	QEN_SET_JOIN_STATE( qen_status->node_access, 0 );

    }   /* end of if read_outer */


    for (;;)	/* to break/continue in */
    {
	/* Fetch (next) inner for this key */

	status = kjoin_inner(rcb, dsh, node, qen_status, read_outer);
	read_outer = FALSE;
	if (status != E_DB_OK)
	{
	    /* if end of kjoin true */
	    if (dsh->dsh_error.err_code != E_QE0015_NO_MORE_ROWS)
	    {
		/* Badness, exit */
		goto errexit;
	    }
	    /* No (more) inners for this outer */
	    if(ojoin != NULL		/* if left join requested and */
		&& ( ( qen_status->node_access & QEN_JOIN_STATE_MASK ) == 0) )
					    /* no IJ or LJ returned */
	    {
		/* execute oj_lnull, fill in nulls */
		if(status = qen_execute_cx(dsh, node_xaddrs->qex_lnull))
		    goto errexit;   /* if ade error, return error */

		/* Qualify (outer)-joined row, get new outer if fails */
		status = qen_execute_cx(dsh, jqual_excb);
		if (status != E_DB_OK) goto errexit;
		if (jqual_excb == NULL || jqual_excb->excb_value == ADE_TRUE)
		{
		    /* We'll return an (outer-joined) row.
		    ** Set state so that we get a new outer next time in.
		    */

		    QEN_SET_JOIN_STATE( qen_status->node_access,
				    QEN32_OJ_OUTER );
		    break;	/* to return left join */
		}
	    } 
	    
	    /* No oj, or jqual rejected the oj'ed row.
	    ** Time for a new outer.
	    */
	    goto newouter;
	}
	/* we have an inner tuple */

	/* If there's a TID qualifier, kjoin_iqual is executed out here,
	** not in DMF. */
	if (node->node_qen.qen_kjoin.kjoin_tqual &&
	    (ade_excb = node_xaddrs->qex_origqual) != NULL)
	{
	    if ((status = qen_execute_cx(dsh, ade_excb)))
		goto errexit;   /* if ade error, return error */
	    if (ade_excb->excb_value != ADE_TRUE)
		continue;		/* to get new inner */
	}

	/* Materialize the joined tuple (if OJ) for join, ON-clause tests */
	status = qen_execute_cx(dsh, node_xaddrs->qex_eqmat);
	if (status != E_DB_OK)
	    goto errexit;   /* if ade error, return error */

	/* execute KQUAL  if it exists */
	ade_excb = node_xaddrs->qex_joinkey;
	if (ade_excb != NULL)
	{
	    if (status = qen_execute_cx(dsh, ade_excb) )
		goto errexit;   /* if ade error, return error */
	    if (ade_excb->excb_value != ADE_TRUE)
		continue;	/* to get new inner */
	}

	/* execute OQUAL if outer join (the ON clause) */
	ade_excb = node_xaddrs->qex_onqual;
	if (ade_excb != NULL)
	{
	    status = qen_execute_cx(dsh, ade_excb);
	    if (status != E_DB_OK)
		goto errexit;   /* if ade error, return error */
	}
	if (ade_excb == NULL || ade_excb->excb_value == ADE_TRUE)
	{
	    /* No OQUAL, or OQUAL succeeds, so these rows inner-join. */

	    /* If inner is keyed unique, there's no need to look
	    ** for more inners, and we can get another outer
	    ** when we return.
	    */
	    if(node->node_qen.qen_kjoin.kjoin_kuniq)
	    {
		/* make next entry read new outer */
		QEN_SET_JOIN_STATE( qen_status->node_access,
					QEN31_IJ_OUTER );
	    }
	    else
	    {
		/* need to read more inners when we return */
		QEN_SET_JOIN_STATE( qen_status->node_access,
					QEN33_IJ_INNER );
	    }
	    /* See if joined result passes JQUAL */
	    if (jqual_excb != NULL)
	    {
		status = qen_execute_cx(dsh, jqual_excb);
		if (status != E_DB_OK)
		    goto errexit;   /* if ade error, return error */
		if (jqual_excb->excb_value != ADE_TRUE)
		{
		    /* Don't want this join.  If inner is unique we can skip
		    ** looking for more and get a new outer.  Otherwise keep
		    ** looking for joining inners.
		    */
		    if (node->node_qen.qen_kjoin.kjoin_kuniq)
			goto newouter;
		    continue;	/* to get a new inner */
		}
	    }
	    break;		/* to return inner join */
	}  /* end of inner-join */

	/*
	** If OQUAL was false, this must be an outer join, and this
	** inner and outer didn't join.
	** Go get another inner tuple.  We can't emit this outer
	** tuple as a LEFT JOIN until we know that it doesn't
	** inner join with any inner tuple.
	*/

    }   /* end inner loop */

    /* Return an inner or outer joined row. */

    status = qen_execute_cx(dsh, node_xaddrs->qex_fatts);
    if (status != E_DB_OK)
	goto errexit;       

#ifdef xDEBUG
    (VOID) qe2_chk_qp(dsh);
#endif

errexit:

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
    if (!reset)
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

/* Name: kjoin_inner - Read inner row via key
**
** Description:
**	This routine reads an inner row.  If we're working on a
**	fresh outer, we position first;  otherwise we simply read
**	the next inner (there might be several inners that match
**	the given key).
**
**	For an unpartitioned inner, doing the above is simple enough.
**	However for a partitioned inner, we need to repeat the
**	position+read for each partition.  The calling routine doesn't
**	know that the inner table is partitioned, and we will hide
**	that fact from it.
**
**	For partitioned inners, there are two cases.  The most
**	general case is that the partitioning scheme has no relation
**	to the storage structure key.  In this case, we have no
**	choice but to examine each partition in turn, doing a key
**	position in each one, and reading any rows we find.  Only
**	after the final partition is examined do we return a "no next"
**	to the caller.
**
**	When the partitioning scheme is compatible with the storage
**	structure, it's possible to do better.  In this case, we can
**	present the key (or partial key) to the which-partition function
**	and get a partition (or range of partitions, for non-equality
**	joins or partial key matches).  Thus we can restrict the
**	position/get loop to just those partition(s).
**
**	FIXME initially we don't do the struct-compat stuff, partly
**	because it needs more support from OPF (we need the entire
**	db-part-def in the qen-part-info, for instance).  Range
**	searches or partial keys make it even more exciting.  for later.
**
** Inputs:
**	rcb			The query's QEF_RCB request block
**	dsh			This query fragment's data segment header
**	node			Pointer to the kjoin node
**	qen_status		Pointer to this kjoin node's runtime status
**	read_outer		TRUE if we just read a new outer;
**				otherwise we've positioned and should just
**				try to read more inners
**
** Outputs:
**	dsh.dsh_error.err_code	Set to E_QE0015_NO_MORE_ROWS if no (more)
**				matching inners;  or to other error code
**				if something bad happened.
**	Returns E_DB_xxx status;  no-more-rows returns E_DB_WARN
**
** Side effects:
**	If the inner tid is needed (for a qualification, say), it will
**	be computed into the inner's row buffer at the specified offset.
**
** History:
**	15-Jul-2004 (schka24)
**	    First pass at doing partitioned inners.
**	10-Aug-2004 (jenjo02)
**	    Open Child thread's table.
**	18-Jul-2005 (schka24)
**	    node-ppart-num moved, adjust here.
*/

static DB_STATUS
kjoin_inner(QEF_RCB *rcb, QEE_DSH *dsh,
	QEN_NODE *node, QEN_STATUS *qen_status,
	bool read_outer)
{
    bool do_position;			/* TRUE if inner needs to position */
    bool key_notin_partition;		/* TRUE if nomatch in cur partition */
    char *output;			/* Where to put the inner tid */
    DB_STATUS status;			/* The usual status thing */
    DB_TID8 bigtid;			/* Inner tid with partition */
    DMR_CB *dmr;			/* Inner's DMF record request CB */
    DMT_CB *dmt;			/* Inner's DMF table request CB */
    i4 offset;				/* Tid offset in output row */
    PTR *cbs;				/* CB pointer base for this DSH */
    QEN_PART_INFO *partp;		/* Partitioning info if any */
    u_i2 partno;			/* Partition number */

    cbs = dsh->dsh_cbs;
    partp = node->node_qen.qen_kjoin.kjoin_part;
    do_position = FALSE;

    if (partp == NULL)
    {
	/* Not partitioned, set up dmrcb ptr just once here */
	partno = 0;
	dmr = (DMR_CB *) cbs[node->node_qen.qen_kjoin.kjoin_get];

	if ( dmr->dmr_access_id == NULL && dsh != dsh->dsh_root )
	{
	    /*
	    ** Then this is a Child thread that needs
	    ** its table opened.
	    */
	    status = qeq_part_open (rcb, dsh->dsh_act_ptr, dsh,
			dsh->dsh_row[node->qen_row],
			dsh->dsh_qp_ptr->qp_row_len[node->qen_row],
			    node->node_qen.qen_kjoin.kjoin_get,
			    (i4)-1, (u_i2)0);
	    if (status != E_DB_OK)
		return (status);
	}
    }

    /* If new outer, we need to start over with the partitions, and
    ** decide which ones we're interested in.
    */
    if (read_outer)
    {
	if (partp != NULL)
	{
	    if ((qen_status->node_access & QEN_OUTER_NEW_KEY) == 0)
	    {
		/* Outer is same key as last time.   Reset to the first
		** qualifying partition from last time.
		*/
		/* FIXME for compat, set to saved partno for exact-partition
		** keying, or pick up first partno of range.
		*/
		qen_status->node_ppart_num = 0;
	    }
	    else
	    {
		/* Outer has new key.  Decide which partitions we need
		** to look at.
		*/
		/* FIXME general case when partdef null or struct-compat
		** flag off.  exact-partition case when doing equijoin
		** with full key and partitioning columns are a subset
		** (or equal) to key columns.  (might be useful to have
		** rdf set that as a flag as well.)  multi-partition
		** case otherwise, need to compute range of partitions
		** (can only happen with tree structures).
		*/
		qen_status->node_ppart_num = 0;

		/* Unless this key is blatantly hopeless (null),
		** we don't know what partitions we'll find it in, so
		** reset all the not-found bits.
		*/
		if ((qen_status->node_access & QEN_THIS_KEY_WONT_PROBE) == 0)
		    MEfill( ((u_i4)partp->part_totparts + BITSPERBYTE-1)/BITSPERBYTE,
			0, qen_status->node_u.node_kjoin.node_nomatch_bits);
		qen_status->node_access &= ~QEN_KEY_IN_SOME_PART;
	    }
	} /* if partp != NULL */

	/* We need a position because we're looking at a new outer */
	do_position = TRUE;
    } /* if read-outer */

    /* Now position/read an inner */
    for (;;)
    {
	if (partp != NULL)
	{
	    /* Set up dmrcb pointer for our current partition */
	    partno = qen_status->node_ppart_num;
	    dmr = (DMR_CB *) cbs[partp->part_dmrix + partno + 1];
	}

	status = E_DB_OK;
	if (do_position)
	{
	    /* We're positioning the inner first. */
	    /* If it's partitioned, we have likely moved from one partition
	    ** to a different one, so do some checking and initialization.
	    */
	    key_notin_partition = FALSE;
	    if (partp != NULL)
	    {
		dmt = (DMT_CB *) cbs[partp->part_dmtix + partno + 1];
		if (dmr == NULL || dmt == NULL || dmt->dmt_record_access_id == NULL)
		{
		    /* Partition ain't open yet, open it. */
		    status = qeq_part_open (rcb, dsh->dsh_act_ptr, dsh,
				dsh->dsh_row[node->qen_row],
				dsh->dsh_qp_ptr->qp_row_len[node->qen_row],
				partp->part_dmrix, partp->part_dmtix,
				partno);
		    if (status != E_DB_OK)
			return (status);
		    ++ qen_status->node_pcount;	/* Update stats */
		    dmr = (DMR_CB *) cbs[partp->part_dmrix + partno + 1];

		    /* Set qual function for this DMR_CB.  Let DMF do
		    ** inner qualification unless tid is involved.
		    */

		    if (node->node_qen.qen_kjoin.kjoin_iqual != NULL
		      && ! node->node_qen.qen_kjoin.kjoin_tqual)
		    {
			dmr->dmr_q_fcn = (PTR) qen_dmr_cx;
			dmr->dmr_q_arg = (PTR)dsh;
		    }
		    else
		    {
			dmr->dmr_q_fcn = (PTR) NULL;
			dmr->dmr_q_arg = (PTR) NULL;
		    }
		}
		/* Perhaps we know globally that the key is a nomatch,
		** or see if perhaps we know it's not in this partition.
		** Don't look at the partition bits if this is the first
		** time for the key, they aren't set yet and it's a waste.
		** ***** ?? doubtful if this a win if we could in-line bttest?
		*/
		if (qen_status->node_access & QEN_THIS_KEY_WONT_PROBE
		  || ((qen_status->node_access & QEN_OUTER_NEW_KEY) == 0
		      && BTtest(partno, qen_status->node_u.node_kjoin.node_nomatch_bits)) )
		    key_notin_partition = TRUE;
	    }

	    /* Ok, we can position now.  Decide how:
	    ** If this outer was a fresh key, use QUAL key positioning.
	    ** If this outer has the same key as the last outer, just
	    ** reposition to reread the same inners.
	    */
	    dmr->dmr_position_type = DMR_QUAL;
	    if ((qen_status->node_access & QEN_OUTER_NEW_KEY) == 0)
		dmr->dmr_position_type = DMR_REPOSITION;

	    /* Perhaps we know that this key is a nomatch, either
	    ** because it's null, or because this isn't a new key and
	    ** it was a nomatch last time.  Simulate no-rows if we
	    ** know;  otherwise do an actual position.
	    */

	    if ((qen_status->node_access & QEN_THIS_KEY_WONT_PROBE)
	      || key_notin_partition)
	    {
		/* This dog don't hunt */
		status = E_DB_WARN;
		dmr->error.err_code = E_DM0055_NONEXT;
	    }
	    else
	    {
		dmr->dmr_flags_mask = 0;
		status = dmf_call(DMR_POSITION, dmr);
	    }
	    /* Don't turn off do-position until we see if it worked */
	}
	if (status == E_DB_OK)
	{
	    /* No position, or the position worked, try a get */

	    do_position = FALSE;

	    /* Set various DMF flags.  If the kjoin result will end up
	    ** in a sort, tell dmf so that it can fool with its locking
	    ** protocols.  If we can satisfy the request from a tree
	    ** index alone, light the primary-key only flag.
	    */

	    dmr->dmr_flags_mask = DMR_NEXT;
	    if (dsh->dsh_qp_status & DSH_SORT)
		dmr->dmr_flags_mask |= DMR_SORT;
	    if (node->node_qen.qen_kjoin.kjoin_ixonly)
		dmr->dmr_flags_mask |= DMR_PKEY_PROJECTION;
	    dsh->dsh_qe_adf = node->node_qen.qen_kjoin.kjoin_iqual;  
	    status = dmf_call(DMR_GET, dmr);
	}
	if (status == E_DB_OK)
	{
	    /* We have a row.  See if the tid needs to be calculated. */
	    if ( (offset = node->node_qen.qen_kjoin.kjoin_tid) > -1)
	    {
		bigtid.tid_i4.tpf = 0;
		bigtid.tid_i4.tid = dmr->dmr_tid;
		bigtid.tid.tid_partno = partno;
		output = (char *) dmr->dmr_data.data_address + offset;
		I8ASSIGN_MACRO(bigtid, *output);
	    } 
	    qen_status->node_access |= QEN_KEY_IN_SOME_PART;
	    return (status);	/* Normal OK return with row */
	}

	if (dmr->error.err_code != E_DM0055_NONEXT)
	{
	    dsh->dsh_error.err_code = dmr->error.err_code;
	    return (status);	/* Badness */
	}

	/* No-more-rows, either on the position, or on the get.
	** If it was on the position, note that we have a nomatch key
	** (either globally or for this partition) in case the same key
	** happens in the next outer.
	** Then either move to the next partition, or pass no-more
	** back to the caller.
	*/
	if (do_position)
	{
	    /* Nomatch on the position */
	    if (partp != NULL)
	    {
		/* Set nomatch for this partition.  Tracking partitions
		** individually ought to be a win for lots of partitions,
		** and of little moment for few partitions.
		*/
		BTset(partno, qen_status->node_u.node_kjoin.node_nomatch_bits);
	    }
	    else
	    {
		qen_status->node_access |= QEN_THIS_KEY_WONT_PROBE;
	    }
	}
	if (partp == NULL)
	{
	    dsh->dsh_error.err_code = E_QE0015_NO_MORE_ROWS;
	    return (E_DB_WARN);
	}

	/* Move to the next partition if there is one */
	/* FIXME will get fancier with struct compat stuff */

	if( ++qen_status->node_ppart_num >= partp->part_totparts)
	{
	    /* They ain't no mo partitions.
	    ** If we didn't see any successful gets, light the global
	    ** key nomatch flag.  Then, return no more rows.
	    */
	    if ((qen_status->node_access & QEN_KEY_IN_SOME_PART) == 0)
		qen_status->node_access |= QEN_THIS_KEY_WONT_PROBE;
	    dsh->dsh_error.err_code = E_QE0015_NO_MORE_ROWS;
	    return (E_DB_WARN);
	}
	/* else loop around to position and read this partition */
	do_position = TRUE;
    } /* for */

    /*NOTREACHED*/

} /* kjoin_inner */
