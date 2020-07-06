/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <tm.h>
#include    <cs.h>
#include    <me.h>
#include    <pc.h>
#include    <tr.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ulf.h>
#include    <lg.h>
#include    <lk.h>
#include    <dmf.h>
#include    <dmccb.h>
#include    <dmrcb.h>
#include    <dmxcb.h>
#include    <dmtcb.h>
#include    <dm.h>
#include    <dmp.h>
#include    <dmpp.h>
#include    <dm1b.h>
#include    <dm0l.h>
#include    <dm0p.h>
#include    <dmftrace.h>
#include    <dm0llctx.h>

/**
** Name: DMCFAST.C  - Consistency Point thread.
**
** Description:
**    
**      dmc_fast_commit()  -  Consistency Point Thread proceedure.
**
** History:
**      15-jun-1988 (rogerk)
**          Created for Jupiter.
**	20-sep-1988 (rogerk)
**	    Added statistics dumping.
**	20-Jan-1989 (ac)
**	    Added arguments to LGbegin().
**	 6-feb-1989 (rogerk)
**	    Added in dm0p_cpdone, dm0p_count_connections calls for shared
**	    buffer manager support.
**	    Changed to use same transaction for all CP's instead of starting
**	    a new one for each consistency point.
**	15-may-1989 (rogerk)
**	    Return resource errors if resource limit is exceeded.
**      17-oct-1991 (mikem)
**          Eliminated unix compile warnings ("warning: statement not reached")
**          changing the way "while (xxx == E_DB_OK)" loops were coded.
**	8-jul-1992 (walt)
**	    Prototyping DMF.
**	3-oct-1992 (bryanp)
**	    Re-arrange includes to share transaction ID type definition.
**	05-oct-1992 (rogerk)
**	    Reduced Logging Project: Cache and recovery protocols changed
**	    to require all servers to flush their caches at consistency
**	    points rather than just fast commit threads.  Accordingly,
**	    this routine changed to implement the "Consistency Point Thread"
**	    rather than the "Fast Commit Thread".
**	14-dec-1992 (rogerk)
**	    Include dm1b.h before dm0l.h for prototype definitions.
**	26-apr-1993 (bryanp)
**	    Correct parameter passing errors revealed by prototyping LK.
**	24-may-1993 (bryanp)
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	31-jan-1994 (bryanp) B58036, B58037, B58038, B58039
**	    Log LG/LK status if error occurs.
**	    Check return code from CSstatistics.
**	    If LG call fails, set error_code before breaking.
**	    Set error code if error occurs during cleanup.
**      22-Feb-1999 (bonro01)
**          Replace references to svcb_log_id with
**          dmf_svcb->svcb_lctx_ptr->lctx_lgid
**	06-May-1999 (jenjo02)
**	    TRdisplay thread stats for each CP by default instead of
**	    thru a trace point. Knowing when a CP is running is often
**	    quite useful.
**	30-Nov-1999 (jenjo02)
**	    Added DB_DIS_TRAN_ID parm to LKcreate_list() prototype.
**	15-Dec-1999 (jenjo02)
**	    Added DB_DIS_TRAN_ID parm to LGbegin() prototype.
**	    Removed DB_DIS_TRAN_ID parm from LKcreate_list() prototype.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      18-feb-2004 (stial01)
**          Fixed incorrect casting of length arguments.
**/

/*
**  Definition of static variables and forward static functions.
*/


/*{
**
** Name: dmc_fast_commit  -  runs server protocols necessary for fast commit
**
** EXTERNAL call format:	status = dmf_call(DMC_FAST_COMMIT, &dmc_cb);
**
** Description:
**     The dmc_fast_commit routine is used for implementing the server side
**     of the protocols necessary to run Fast Commit.  Its main task is to
**     flush dirty pages out of the Buffer Manager at Consistency Points.
**
**     The dmc_fast_commit routine should only be called within a special
**     session that is dedicated for this purpose.  This routine will not
**     return under normal circumstances until server shutdown time.
**
**     If dmc_fast_commit does return abnormally, it signifies a fatal
**     error condition in DMF.
**
**     The DMC_FAST_COMMIT routine responds to the following LG events:
**
** 	LG_E_CPFLUSH	    - Flush all dirty pages out of the buffer
** 			      as part of consistency point protocol.
** 	LG_E_FCRECOVER	    - The RCP is going to recover databases.
** 			      Flush dirty pages so the RCP can access them.
** 	LG_E_FCSHUTDOWN	    - Server is shutting down, return to caller so
**			      this session can be terminated.
**
** Inputs:
**     dmc_cb
** 	.type		    Must be set to DMC_CONTROL_CB.
** 	.length		    Must be at least sizeof(DMC_CB).
**
** Outputs:
**     dmc_cb
** 	.error.err_code	    One of the following error numbers.
**			    E_DB_OK
**			    E_DM004A_INTERNAL_ERROR
**			    E_DM004B_LOCK_QUOTA_EXCEED
**			    E_DM0062_TRAN_QUOTA_EXCEED
**			    E_DM0116_FAST_COMMIT
**
** Returns:
**     E_DB_OK
**     E_DB_FATAL
**
** History:
**	 6-feb-1989 (rogerk)
**	    Added in dm0p_cpdone, dm0p_count_connections calls for shared
**	    buffer manager support.
**	    Changed to use same transaction for all CP's instead of starting
**	    a new one for each consistency point.
**	15-may-1989 (rogerk)
**	    Return resource errors if resource limit is exceeded.
**      17-oct-1991 (mikem)
**          Eliminated unix compile warnings ("warning: statement not reached")
**          changing the way "while (xxx == E_DB_OK)" loops were coded.
**      2-oct-1992 (ed)
**          Use DB_MAXNAME to replace hard coded numbers
**          - also created defines to replace hard coded character strings
**          dependent on DB_MAXNAME
**	05-oct-1992 (rogerk)
**	    Reduced Logging Project: Cache and recovery protocols changed
**	    to require all servers to flush their caches at consistency
**	    points rather than just fast commit threads.  Accordingly,
**	    this routine changed to implement the "Consistency Point Thread"
**	    rather than the "Fast Commit Thread".  The logging system
**	    now requires that an LGalter (LG_A_CPAGENT) call be made to
**	    identify the server as having a Consistency Point Thread.
**	10-oct-93 (swm)
**	    Bug #56438
**	    Put LG_DBID into automatic variable lg_dbid rather than overloading
**	    dmc_cb->dmc_db_id.
**	31-jan-1994 (bryanp) B58036, B58037, B58038, B58039
**	    Log LG/LK status if error occurs.
**	    Check return code from CSstatistics.
**	    If LG call fails, set error_code before breaking.
**	    Set error code if error occurs during cleanup.
**	06-May-1999 (jenjo02)
**	    TRdisplay thread stats for each CP by default instead of
**	    thru a trace point. Knowing when a CP is running is often
**	    quite useful.
*/
DB_STATUS
dmc_fast_commit(
DMC_CB	    *dmc_cb)
{
    DM_SVCB	    *svcb = dmf_svcb;
    DB_TRAN_ID	    tran_id;
    LG_LXID	    lx_id;
    DM0L_ADDDB	    add_info;
    i4	    lock_list;
    i4		    len_add_info;
    i4		    event_mask;
    i4		    events, wakeup_event;
    i4		    have_locklist = FALSE;
    i4		    have_transaction = FALSE;
    i4	    cpcount = 0;
    i4	    error_code = E_DM0116_FAST_COMMIT;
    DB_STATUS	    status = E_DB_OK;
    STATUS	    stat;
    i4	    error;
    CL_SYS_ERR	    sys_err;
    TIMERSTAT	    stat_block;
    DB_OWN_NAME	    user_name;
    LG_DBID	    lg_dbid;
    i4 	    	    last_dio = 0;
    i4		    last_cpu = 0;
    i4		    enable_stats = TRUE;

#ifdef xDEBUG
    TRdisplay("Starting server Consistency Point Thread for server id 0x%x\n",
	svcb->svcb_id);
#endif

    MEcopy((PTR)DB_CPTHREAD_NAME, sizeof(add_info.ad_dbname),
	(PTR) &add_info.ad_dbname);
    MEcopy((PTR)DB_INGRES_NAME, sizeof(add_info.ad_dbowner),
	(PTR) &add_info.ad_dbowner);
    MEcopy((PTR)"None", 4, (PTR) &add_info.ad_root);
    add_info.ad_dbid = 0;
    add_info.ad_l_root = 4;
    len_add_info = sizeof(add_info) - sizeof(add_info.ad_root) + 4;

    stat = LGadd(dmf_svcb->svcb_lctx_ptr->lctx_lgid, LG_NOTDB, (char *)&add_info, 
	len_add_info, &lg_dbid, &sys_err);
    if (stat != OK)
    {
	ule_format(stat, (CL_ERR_DESC *)&sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, &error, 0);
	(VOID) ule_format(E_DM900A_BAD_LOG_DBADD, &sys_err, ULE_LOG, NULL,
	    (char *)0, 0L, (i4 *)0, &error, 4, 0,
	    dmf_svcb->svcb_lctx_ptr->lctx_lgid,
	    sizeof(add_info.ad_dbname), (PTR) &add_info.ad_dbname,
	    sizeof(add_info.ad_dbowner), (PTR) &add_info.ad_dbowner,
	    4, (PTR) &add_info.ad_root);
	if (stat == LG_EXCEED_LIMIT)
	    dmc_cb->error.err_code = E_DM0062_TRAN_QUOTA_EXCEEDED;
	else
	    dmc_cb->error.err_code = E_DM0116_FAST_COMMIT;
	return (E_DB_ERROR);
    }

    for (;;)
    {
    	if (stat != OK)
	    break;

	MEcopy((PTR)DB_CPTHREAD_OWNER, sizeof(DB_OWN_NAME), (PTR)&user_name);
	stat = LGbegin(LG_NOPROTECT, lg_dbid, &tran_id, &lx_id,
	    sizeof(DB_OWN_NAME), user_name.db_own_name, 
	    (DB_DIS_TRAN_ID*)NULL, &sys_err);
	if (stat != OK)
	{
	    ule_format(stat, (CL_ERR_DESC *)&sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &error, 0);
	    ule_format(E_DM900C_BAD_LOG_BEGIN, &sys_err, ULE_LOG, NULL, 
		(char *)0, (i4)0, (i4 *)0, &error, 1, 0, lg_dbid);
	    if (stat == LG_EXCEED_LIMIT)
		error_code = E_DM0062_TRAN_QUOTA_EXCEEDED;
	    status = E_DB_ERROR;
	    break;
	}
	have_transaction = TRUE;

	stat = LKcreate_list(LK_NONPROTECT, (i4) 0,
	    (LK_UNIQUE *)&tran_id, (LK_LLID *)&lock_list, (i4)0, 
	    &sys_err);
	if (stat != OK)
	{
	    ule_format(stat, (CL_ERR_DESC *)&sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &error, 0);
	    ule_format(E_DM901A_BAD_LOCK_CREATE, &sys_err, ULE_LOG, NULL, 
		(char *)0, (i4)0, (i4 *)0, &error, 0);
	    if (stat == LK_NOLOCKS)
		error_code = E_DM004B_LOCK_QUOTA_EXCEEDED;
	    status = E_DB_ERROR;
	    break;
	}
	have_locklist = TRUE;

	/*
	** Inform the Logging System that this process has a Consistency
	** Point thread.  After this the logging system will inform us of
	** all CP events and wait for us to respond before marking the CP
	** event complete.
	*/
	stat = LGalter(LG_A_CPAGENT, (PTR)&svcb->svcb_lctx_ptr->lctx_lgid,
	    sizeof(svcb->svcb_lctx_ptr->lctx_lgid), &sys_err);
	if (stat != OK)
	{
	    ule_format(stat, (CL_ERR_DESC *)&sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &error, 0);
	    ule_format(E_DM900B_BAD_LOG_ALTER, &sys_err, ULE_LOG, NULL, 
		(char *)0, (i4)0, (i4 *)0, &error, 1, 
		0, LG_A_CPAGENT);
	    status = E_DB_ERROR;
	    break;
	}
	break;
    }

    /* turn on session stats */
    CSaltr_session((CS_SID)0, CS_AS_CPUSTATS, (PTR)&enable_stats);

    while (stat == OK)
    {
	event_mask = LG_E_CPFLUSH;

	stat = LGevent(LG_INTRPTABLE, lx_id, event_mask, &events, &sys_err);
	if (stat != OK)
	{
	    /*
	    ** Check for shutdown interrupt event.  When server is being
	    ** terminated, this thread will be interrupted.
	    */
	    if (stat == LG_INTERRUPT)
	    {
		status = E_DB_OK;
		break;
	    }

	    ule_format(stat, (CL_ERR_DESC *)&sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &error, 0);
	    ule_format(E_DM900F_BAD_LOG_EVENT, &sys_err, ULE_LOG, NULL, (char *)0,
		(i4)0, (i4 *)0, &error, 1, 0, event_mask);
	    status = E_DB_ERROR;
	    break;
	}

	TRdisplay("%@ Consistency Point Thread for server %d just got event 0x%x\n", 
		    svcb->svcb_id, events);

	/*
	** Consistency Point Protocol - We have been signaled to flush
	** dirty pages as part of a consistency point.
	**
	** When we have finished flushing, call LG to indicate that we are
	** done and wait for LG to tell us to continue.  We have to wait
	** for all the other Fast Commit threads to complete as well so
	** LG can turn off the CPFLUSH event.
	*/
	if (events & LG_E_CPFLUSH)
	{
	    /* Count Consistency Points for statistics printing */
	    cpcount++;

	    /*
	    ** Call buffer manager to verify the number of connections
	    ** to it.  This is done so that the consistency point
	    ** code can verify that all the buffers have been flushed
	    ** when the last Fast Commit thread is done with the CP.
	    */
	    dm0p_count_connections();

	    /*
	    ** Flush all dirty pages out of the Buffer Manager.
	    */
	    status = dm0p_cp_flush(lock_list, (i4)lx_id, &error);
	    if (status != E_DB_OK)
	    {
		error_code = error;
		if (error > E_DM_INTERNAL)
		{
		    ule_format(error, 0, ULE_LOG, NULL, (char *)0, (i4)0,
			(i4 *)0, &error, 0);
		    error_code = E_DM0116_FAST_COMMIT;
		}
		break;
	    }

	    /*
	    ** Tell Logging System that we have finished flushing.
	    */
	    stat = LGalter(LG_A_CPFDONE, (PTR)&svcb->svcb_lctx_ptr->lctx_lgid,
		sizeof(svcb->svcb_lctx_ptr->lctx_lgid), &sys_err);
	    if (stat != OK)
	    {
		ule_format(stat, (CL_ERR_DESC *)&sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, &error, 0);
		ule_format(E_DM900B_BAD_LOG_ALTER, &sys_err, ULE_LOG, NULL, (char *)0,
		    (i4)0, (i4 *)0, &error, 1, 0, LG_A_CPFDONE);
		status = E_DB_ERROR;
		break;
	    }

	    /*
	    ** Wait till CPFLUSH event is turned off.
	    */
	    stat = LGevent(LG_INTRPTABLE, lx_id, LG_E_CPWAKEUP, &wakeup_event,
		&sys_err);
	    if (stat != OK)
	    {
		/*
		** Check for shutdown interrupt event.  When server is being
		** terminated, this thread will be interrupted.
		*/
		if (stat == LG_INTERRUPT)
		{
		    status = E_DB_OK;
		    break;
		}

		ule_format(stat, (CL_ERR_DESC *)&sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, &error, 0);
		ule_format(E_DM900F_BAD_LOG_EVENT, &sys_err, ULE_LOG, NULL, (char *)0,
		    (i4)0, (i4 *)0, &error, 1, 0, LG_E_CPWAKEUP);
		status = E_DB_ERROR;
		break;
	    }

	    /*
	    ** Call buffer manager to signal that the consistency point is
	    ** finished and to set up for the next CP.
	    */
	    dm0p_cpdone();

	    /*
	    ** Tell Logging Sytem that we are ready for the next event.
	    */
	    stat = LGalter(LG_A_CPWAITDONE, (PTR)&svcb->svcb_lctx_ptr->lctx_lgid,
		sizeof(svcb->svcb_lctx_ptr->lctx_lgid), &sys_err);
	    if (stat != OK)
	    {
		ule_format(stat, (CL_ERR_DESC *)&sys_err, ULE_LOG, NULL,
			(char *)0, (i4)0, (i4 *)0, &error, 0);
		ule_format(E_DM900B_BAD_LOG_ALTER, &sys_err, ULE_LOG, NULL, (char *)0,
		    (i4)0, (i4 *)0, &error, 1, 0, LG_A_CPWAITDONE);
		status = E_DB_ERROR;
		break;
	    }

	    /* Write stats about this Consistency Point */
	    stat = CSstatistics(&stat_block, 0);
	    if (stat)
		ule_format(stat, (CL_ERR_DESC *)0, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &error, 0);
	    TRdisplay("%@ Consistency Point Thread statistics for server %d %19*-\n",
			svcb->svcb_id);
	    TRdisplay("%@ Consistency Point : %d    Cpu : %d    Dio : %d\n",
			cpcount, stat_block.stat_cpu - last_cpu, 
			stat_block.stat_dio - last_dio);
	    TRdisplay("%79*-\n");
	    
	    last_cpu = stat_block.stat_cpu;
	    last_dio = stat_block.stat_dio;
	}

	/*
	** Future event - might be asked to flush pages for a particular
	** transaction so the RCP can recover it.  This will be needed
	** when we have Multi-server Fast Commit.  The RCP may ask us
	** to flush pages because another server failed.
	*/
    }

    /*
    ** Clean up transaction or lock list left hanging around. If an error
    ** occurs, log the error. If we haven't yet encountered an error (that is,
    ** if this error is the first error we've hit so far in this routine), then
    ** make sure that we set a return status and return error_code for our
    ** caller.
    */
    if (have_transaction)
    {
	stat = LGend(lx_id, 0, &sys_err);
	if (stat != OK)
	{
	    ule_format(stat, (CL_ERR_DESC *)&sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &error, 0);
	    ule_format(E_DM900E_BAD_LOG_END, &sys_err, ULE_LOG, NULL, (char *)0,
		(i4)0, (i4 *)0, &error, 1, 0, lx_id);
	    if (status == E_DB_OK)
	    {
		error_code = E_DM0116_FAST_COMMIT;
		status = E_DB_ERROR;
	    }
	}
    }
    if (have_locklist)
    {
	stat = LKrelease(LK_ALL, lock_list, (LK_LKID *)0, (LK_LOCK_KEY *)0,
	    (LK_VALUE *)0, &sys_err);
	if (stat != OK)
	{
	    ule_format(stat, (CL_ERR_DESC *)&sys_err, ULE_LOG, NULL,
		    (char *)0, (i4)0, (i4 *)0, &error, 0);
	    ule_format(E_DM901B_BAD_LOCK_RELEASE, &sys_err, ULE_LOG, NULL, (char *)0,
		(i4)0, (i4 *)0, &error, 1, 0, lock_list);
	    if (status == E_DB_OK)
	    {
		error_code = E_DM0116_FAST_COMMIT;
		status = E_DB_ERROR;
	    }
	}
    }

    stat = LGremove(lg_dbid, &sys_err);
    if (stat != OK)
    {
	ule_format(stat, (CL_ERR_DESC *)&sys_err, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, &error, 0);
	ule_format(E_DM9016_BAD_LOG_REMOVE, &sys_err, ULE_LOG, NULL, (char *)0,
		(i4)0, (i4 *)0, &error, 1, 0, lg_dbid);
	if (status == E_DB_OK)
	{
	    error_code = E_DM0116_FAST_COMMIT;
	    status = E_DB_ERROR;
	}
    }

    /*
    ** Write cumulative Fast Commit thread statistics.  These will be written to the
    ** server log file if there is one defined.
    */
    stat = CSstatistics(&stat_block, 0);
    if (stat)
	ule_format(stat, (CL_ERR_DESC *)0, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, &error, 0);
    TRdisplay("\n%19*- Cumulative Consistency Point Thread statistics for server %d %8*-\n",
		    svcb->svcb_id);
    TRdisplay("    Consistency Points : %d    Cpu : %d    Dio : %d\n",
	cpcount, stat_block.stat_cpu, stat_block.stat_dio);
    TRdisplay("%79*-\n");

    if (status != E_DB_OK)
	dmc_cb->error.err_code = error_code;
    return (status);
}
