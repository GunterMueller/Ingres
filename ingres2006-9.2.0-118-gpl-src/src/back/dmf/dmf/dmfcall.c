/*
** Copyright (c) 1985, 2005 Ingres Corporation
**
**
NO_OPTIM=dr6_us5
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <ex.h>
#include    <me.h>
#include    <pc.h>
#include    <sl.h>
#include    <cx.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <lk.h>
#include    <lg.h>
#include    <tm.h>
#include    <ulf.h>
#include    <ulx.h>
#include    <dmf.h>
#include    <dm.h>
#include    <dmccb.h>
#include    <dmdcb.h>
#include    <dmrcb.h>
#include    <dmscb.h>
#include    <dmtcb.h>
#include    <dmxcb.h>
#include    <dmucb.h>
#include    <dmmcb.h>
#include    <dmp.h>
#include    <dml.h>
#include    <dmu.h>
#include    <dmm.h>
#include    <dmpp.h>
#include    <sxf.h>
#include    <dma.h>
#include    <dm1b.h>
#include    <dm0l.h>
#include    <dm0p.h>
#include    <dmd.h>
#include    <dmftrace.h>
#include    <csp.h>
#include    <dmfcsp.h>


/**
**
**  Name: DMF_CALL.C - The main DMF entry point.
**
**  Description:
**	This file contains the routine that implements the DMF logical
**	interface.
**
**          dmf_call - The primary DMF entry point.
**
**
**  History:
**      14-nov-1985 (derek)    
**          Created new for jupiter.
**	15-Aug-1988 (teg)
**	    added call for DMU_GET_TABID
**	31-Aug-1988 (rogerk)
**	    added call for DMC_WRITE_BEHIND
**	25-mar-1988 (Derek)
**	    Added new maintanence operations.
**	14-Apr-1989 (teg)
**	    Added new maintenance operations (DMM_LISTFILE, DMM_DROPFILE)
**      18-Dec-1989 (anton)
**	    Bug 6143 - UNIX quantum - added CSswitch
**	12-feb-1990 (rogerk)
**	    Added DMR_ALTER entry point.
**	05-mar-1990 (linda,bryanp)
**	    Changed cb_type and cb_size fields to match types in the control
**	    blocks being checked.  sizeof(DMU_CB) was >256, cb_size type was
**	    u_char; so wrong size was not detected.
**	08-aug-1990 (ralph)
**	    Add support for DMA_SHOW_STATE 
**	24-sep-1990 (rogerk)
**	    Merged 6.3 changes into 6.5.
**	07-jan-91 (teresa)
**	    Add support for DMM_FINDDBS
**	14-jul-1992 (ananth)
**	    Prototyping DMF.
**	22-jul-1992 (bryanp)
**	    Add support for new special threads: DMC_RECOVERY_THREAD,
**	    DMC_CHECK_DEAD, DMC_FORCE_ABORT, DMC_GROUP_COMMIT,
**	    DMC_LOGWRITER.
**	30-October-1992 (rmuth)
**	    Add support for DMU_CONVERT_TABLE.
**	14-dec-1992 (rogerk)
**	    Include dm1b.h before dm0l.h for prototype definitions.
**	17-nov-1992 (robf)
**	    DELETE support for DMA_WRITE_RECORD, DMA_SET_STATE and
**	    DMA_SHOW_STATE. These are now handled by the SXF facility.
**	18-jan-1993 (bryanp)
**	    Add support for DMC_CP_TIMER special thread.
**	24-may-1993 (bryanp)
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**	24-may-1993 (robf)
**	    Add support for DMU_GET_SECID call.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	26-jul-1993 (bryanp)
**	    Prototyping code for 6.5. Protoyping this routine doesn't add much
**	    value, and it forces the touching of much non-DMF code, so we
**	    don't prototype this routine. However, I did change it slightly
**	    to treat the control_block argument as a PTR, which is the actual
**	    type of the passed argument.
**	26-jul-1993 (rog)
**	    Generic exception handler cleanup.
**	31-jan-1994 (mikem)
**          Sir #58499
**	    Made calls to dmd_return() and dmd_call() conditional on the dm1205
**	    tracepoint rather than xDEBUG.  This will make debugging some 
**	    problems in the field a lot easier at the cost of 2 extra 
**	    conditional branches per dmf call.
**	24-apr-1995 (cohmi01)
**	    Add calls for write-along/read-ahead threads for IOMASTER server.
**	22-may-1995 (cohmi01)
**	    Add DMR_COUNT request for aggregate optimization project.
**	21-aug-1995 (cohmi01)
**	    DMR_COUNT changed to the generalized DMR_AGGREGATE request.
**      01-jun-1996 (ramra01 for bryanp)
**          Add case for DMU_ALTER_TABLE; call dmu_atable to process the work.
**	4-jun-96 (stephenb)
**	    add entry to cb_check struct for replication queue management
**	    thread, and new case to dmf_call for DMC_REP_QMAN (the thread's
**	    main entry point).
**      01-aug-1996 (nanpr01 for keving)
**          dmf_call(DMC_LK_CALLBACK) will make this thread the LK Blocking
**          Callback Thread.
**	15-Nov-1996 (jenjo02)
**	    dmf_call(DMC_DEADLOCK_THREAD) will start the recovery server 
**	    Deadlock Detection Thread.
**	5-dec-96 (stephenb)
**	    Add performance profiling code
**	21-may-1997 (shero03)
**	    Added facility tracing diagnostic
**	18-feb-98 (inkdo01)
**	    Added DMC_SORT_THREAD for parallel sort threads project.
**	13-apr-98 (inkdo01)
**	    Dropped DMC_SORT_THREAD in favour of factotum thread strategy.
**	14-May-1998 (jenjo02)
**	    Replaced DMC_WRITE_BEHIND with DMC_START_WB_THREADS.
**	    Call dm0p_start_wb_threads() instead of dmc_write_behind().
**	19-may-1998 (nanpr01)
**	    Parallel Index Creation.
**       1-oct-1999 (mosjo01) SIR 97564
**          Reposition cs.h before ex.h avoids compiler errors with __jmpbuf
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	11-may-2001 (devjo01)
**	    Add support for DMC_CSP_x calls. (recycling slots 4, 5, & 6)
**	    s103715.
**	05-Mar-2002 (jenjo02)
**	    Added DMS_OPEN_SEQ, DMS_CLOSE_SEQ, DMS_NEXT_SEQ
**	    Sequence Generator operations.
**      13-mar-2002 (stial01)
**          Added call to dmd_call if error.
**      22-oct-2002 (stial01)
**          Added DMX_XA_ABORT
**      08-may-2003 (stial01)
**         Fixed diagnostics for SIR 107325 (E_DM002A_BAD_PARAMETER)
**		29-Apr-2004 (gorvi01)
**			Added entry in struct for DMM_DEL_LOC used for unextenddb.
**		29-Apr-2004 (gorvi01)
**			Added case statement for DMM_DEL_LOC. This will call the 
**			dmm_del_location procedure.
**      01-sep-2004 (stial01)
**          Support cluster online checkpoint
**	09-may-2005 (devjo01)
**	    Add DMC_DIST_DEADLOCK_THR in support of VMS clusters.
**	21-jun-2005 (devjo01)
**	    Add DMC_GLC_SUPPORT_THR in support of VMS clusters.
**      13-feb-2006 horda03) Bug 112560/INGSRV2880
**         New DMT interfaces DMT_TABLE_LOCKED and
**         DMT_RELEASE_TABLE_LOCKS.
**	21-Jun-2006 (jonj)
**	    Add DMX_XA_START, DMX_XA_END, DMX_XA_PREPARE, DMX_XA_COMMIT,
**	    DMX_XA_ROLLBACK for XA integration, deprecated DMX_XA_ABORT.
**	19-Sep-2006 (jonj)
**	    Add DMT_INVALIDATE_TCB.
**	04-Oct-2006 (jonj)
**	    Add DMT_CONTROL_LOCK.
*/

/*
**  Definition of static variables and forward static functions.
*/

static    STATUS  ex_handler(
	EX_ARGS	*ex_args);	    /*	DMF catch-all exception handler. */
static    STATUS  ex_2handler(
        EX_ARGS *ex_args);          /*  DMF catch-all exception handler. */


/*
**  Using operation code as an index this gives that type of control 
**  block required and the minimum size of the control block.
*/

static READONLY struct
    {
	i2		cb_type;
	i4		cb_size;
    }		    cb_check[DM_NEXT_OP] =
    {
	{ 0, 0 },			    /* NOT USED */
	{ 0, 0 },			    /* NOT USED */
	{ 0, 0 },			    /* NOT USED */
	{ 0, 0 },			    /* NOT USED */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_CSP_MAIN */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_CSP_MSGMON */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_CSP_MSGTHR */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_RESET_EFF_USER_ID */
        { DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_START_WB_THREADS */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_FAST_COMMIT */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_ALTER */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_SHOW */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_START_SERVER */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_STOP_SERVER */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_BEGIN_SESSION */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_END_SESSION */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_OPEN_DB */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_CLOSE_DB */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_ADD_DB */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_DEL_DB */
	{ DMD_DEBUG_CB, sizeof(DMD_CB) },   /* DMD_ALTER_CB */
	{ DMD_DEBUG_CB, sizeof(DMD_CB) },   /* DMD_CANCEL_TRACE */
	{ DMD_DEBUG_CB, sizeof(DMD_CB) },   /* DMD_DISPLAY_TRACE */
	{ DMD_DEBUG_CB, sizeof(DMD_CB) },   /* DMD_GET_CB */
	{ DMD_DEBUG_CB, sizeof(DMD_CB) },   /* DMD_RELEASE_CB */
	{ DMD_DEBUG_CB, sizeof(DMD_CB) },   /* DMD_SET_TRACE */
	{ DMD_DEBUG_CB, sizeof(DMD_CB) },   /* DMD_SHOW_CB */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_REP_QMAN */
        { DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_DIST_DEADLOCK_THR */
        { DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_GLC_SUPPORT_THR */
	{ DMR_RECORD_CB, sizeof(DMR_CB) },  /* DMR_DELETE */
	{ DMR_RECORD_CB, sizeof(DMR_CB) },  /* DMR_GET */
	{ DMR_RECORD_CB, sizeof(DMR_CB) },  /* DMR_POSITION */
	{ DMR_RECORD_CB, sizeof(DMR_CB) },  /* DMR_PUT */
	{ DMR_RECORD_CB, sizeof(DMR_CB) },  /* DMR_REPLACE */
	{ DMR_RECORD_CB, sizeof(DMR_CB) },  /* DMR_LOAD */
	{ DMR_RECORD_CB, sizeof(DMR_CB) },  /* DMR_DUMP_DATA */
	{ DMR_RECORD_CB, sizeof(DMR_CB) },  /* DMR_ALTER */
	{ DMR_RECORD_CB, sizeof(DMR_CB) },  /* DMR_AGGREGATE */
	{ DMR_RECORD_CB, sizeof(DMR_CB) },  /* DMR_UNFIX */
	{ 0, 0 },			    /* NOT USED*/
	{ 0, 0 },			    /* NOT USED*/
	{ 0, 0 },			    /* NOT USED*/
	{ 0, 0 },			    /* NOT USED*/
	{ 0, 0 },			    /* NOT USED*/
	{ 0, 0 },			    /* NOT USED*/
	{ 0, 0 },			    /* NOT USED*/
	{ 0, 0 },			    /* NOT USED */
	{ 0, 0 },			    /* NOT USED */
	{ 0, 0 },			    /* NOT USED */
	{ DMT_TABLE_CB, sizeof(DMT_CB) },   /* DMT_ALTER */
	{ DMT_TABLE_CB, sizeof(DMT_CB) },   /* DMT_CLOSE */
	{ DMT_TABLE_CB, sizeof(DMT_CB) },   /* DMT_CREATE_TEMP */
	{ DMT_TABLE_CB, sizeof(DMT_CB) },   /* DMT_DELETE_TEMP */
	{ DMT_TABLE_CB, sizeof(DMT_CB) },   /* DMT_OPEN */
	{ DMT_SH_CB, sizeof(DMT_SHW_CB) },  /* DMT_SHOW */
	{ DMT_TABLE_CB, sizeof(DMT_CB) },   /* DMT_SORT_COST */
	{ DMT_TABLE_CB, sizeof(DMT_CB) },   /* DMT_INVALIDATE_TCB */
	{ DMT_TABLE_CB, sizeof(DMT_CB) },   /* DMT_TABLE_LOCKED */
	{ DMT_TABLE_CB, sizeof(DMT_CB) },   /* DMT_RELEASE_TABLE_LOCKS */
	{ DMU_UTILITY_CB, sizeof(DMU_CB) }, /* DMU_CREATE_TABLE */
	{ DMU_UTILITY_CB, sizeof(DMU_CB) }, /* DMU_DESTROY_TABLE */
	{ DMU_UTILITY_CB, sizeof(DMU_CB) }, /* DMU_INDEX_TABLE */
	{ DMU_UTILITY_CB, sizeof(DMU_CB) }, /* DMU_MODIFY_TABLE */
	{ DMU_UTILITY_CB, sizeof(DMU_CB) }, /* DMU_ALTER  */
	{ DMU_UTILITY_CB, sizeof(DMU_CB) }, /* DMU_SHOW */
	{ 0, 0 },			    /* NOT USED */
	{ DMU_UTILITY_CB, sizeof(DMU_CB) }, /* DMU_GET_TABID */
	{ DMU_UTILITY_CB, sizeof(DMU_CB) }, /* DMU_CONVERT_TABLE */
	{ 0, 0 },			    /* NOT USED */
	{ DMX_TRANSACTION_CB, sizeof(DMX_CB) }, /* DMX_ABORT */
	{ DMX_TRANSACTION_CB, sizeof(DMX_CB) }, /* DMX_BEGIN */
	{ DMX_TRANSACTION_CB, sizeof(DMX_CB) }, /* DMX_COMMIT */
	{ DMX_TRANSACTION_CB, sizeof(DMX_CB) }, /* DMX_SECURE */
	{ DMX_TRANSACTION_CB, sizeof(DMX_CB) }, /* DMX_SAVEPOINT  */
	{ DMX_TRANSACTION_CB, sizeof(DMX_CB) }, /* DMX_RESUME */
	{ DMM_MAINT_CB, sizeof(DMM_CB) },   /* DMM_CREATE_DB */
	{ DMM_MAINT_CB, sizeof(DMM_CB) },   /* DMM_DESTROY_DB */
	{ DMM_MAINT_CB, sizeof(DMM_CB) },   /* DMM_ALTER_DB */
	{ 0, 0 },   			    /* NOT USED */
	{ DMM_MAINT_CB, sizeof(DMM_CB) },   /* DMM_LISTFILE */
	{ DMM_MAINT_CB, sizeof(DMM_CB) },   /* DMM_DROPFILE */
	{ DMM_MAINT_CB, sizeof(DMM_CB) },   /* DMM_ADD_LOC */
	{ 0, 0 },   			    /* NOT USED */
	{ 0, 0 },   			    /* NOT USED */
	{ DMM_MAINT_CB, sizeof(DMM_CB) },   /* DMM_FINDDBS */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_RECOVERY_THREAD */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_CHECK_DEAD */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_GROUP_COMMIT */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_FORCE_ABORT */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_LOGWRITER */
	{ DMM_MAINT_CB, sizeof(DMM_CB) },   /* DMM_CONFIG */
	{ DMM_MAINT_CB, sizeof(DMM_CB) },   /* DMM_DEL_DUMP_CONFIG */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_CP_TIMER */
	{ DMU_UTILITY_CB, sizeof(DMU_CB) }, /* DMU_GET_SECID */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_WRITE_ALONG  */
	{ DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_READ_AHEAD   */
	{ DMU_UTILITY_CB, sizeof(DMU_CB) }, /* DMU_SETPRODUCTION */
	{ DMU_UTILITY_CB, sizeof(DMU_CB) }, /* DMU_ALTER_TABLE  */
        { DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_LK_CALLBACK */
        { DMC_CONTROL_CB, sizeof(DMC_CB) }, /* DMC_DEADLOCK_THREAD */
	{ DMU_UTILITY_CB, sizeof(DMU_CB) }, /* DMU_PINDEX_TABLE */
	{ DMS_SEQ_CB, sizeof(DMS_CB) },     /* DMS_OPEN_SEQ */
	{ DMS_SEQ_CB, sizeof(DMS_CB) },     /* DMS_CLOSE_SEQ */
	{ DMS_SEQ_CB, sizeof(DMS_CB) },     /* DMS_NEXT_SEQ */
	{ DMM_MAINT_CB, sizeof(DMM_CB) },   /* DMM_DEL_LOC */
	{ DMX_TRANSACTION_CB, sizeof(DMX_CB) }, /* DMX_XA_START */
	{ DMX_TRANSACTION_CB, sizeof(DMX_CB) }, /* DMX_XA_END */
	{ DMX_TRANSACTION_CB, sizeof(DMX_CB) }, /* DMX_XA_PREPARE */
	{ DMX_TRANSACTION_CB, sizeof(DMX_CB) }, /* DMX_XA_COMMIT */
	{ DMX_TRANSACTION_CB, sizeof(DMX_CB) }, /* DMX_XA_ROLLBACK */
	{ DMT_TABLE_CB, sizeof(DMT_CB) },   /* DMT_CONTROL_LOCK */
    };

/*{
** Name: dmf_call	- The main dmf entry point.
**
** Description:
**      The routine checks that the arguments to dmf_call look reasonable
**	and the appropraite control block type and size is passed.  The
**	implementing function is then called and operation completion
**	status is returned to the caller.
**
** Inputs:
**      operation                       The DMF operation code.
**      control_block                   The DMF control block for the operation.
**
** Outputs:
**	Returns:
**	    DB_STATUS
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	14-nov-1985 (derek)
**          Created new for jupiter.
**	15-Aug-1988 (teg)
**	    added call for DMU_GET_TABID
**	31-Aug-1988 (rogerk)
**	    added call for DMC_WRITE_BEHIND
**	14-Apr-1989 (teg)
**	    Added new maintenance operations (DMM_LISTFILE, DMM_DROPFILE)
**	18-Dec-1989 (anton)
**	    Bug 6143 - UNIX Quantum - adding call to CSswitch
**	12-feb-1990 (rogerk)
**	    Added DMR_ALTER entry point.
**	08-aug-1990 (ralph)
**	    Add support for DMA_SHOW_STATE 
**	07-jan-91 (teresa)
**	    Add support for DMM_FINDDBS
**	23-may-91 (andre)
**	    added support for DMC_RESET_EFF_USER_ID.
**	22-jul-1992 (bryanp)
**	    Add support for DMC_RECOVERY_THREAD
**	05-oct-92 (teresa)
**	    add support for DMM_CONDIG and DMM_DEL_DUMP_CONFIG
**	30-October-1992 (rmuth)
**	    add support for DMU_CONVERT_TABLE.
**	18-jan-1993 (bryanp)
**	    Add support for DMC_CP_TIMER special thread.
**	20-may-1993 (robf)
**	    DMC_SHOW now calls dmc_show()
**	26-jul-1993 (bryanp)
**	    Prototyping code for 6.5. Protoyping this routine doesn't add much
**	    value, and it forces the touching of much non-DMF code, so we
**	    don't prototype this routine. However, I did change it slightly
**	    to treat the control_block argument as a PTR, which is the actual
**	    type of the passed argument.
**	02-jan-94 (andre)
**	    DMC_SHOW is no longer a NOOP.  For the time being it will only 
** 	    support requests for journaling status of a database, but this will
** 	    probably change in the future
**	31-jan-1994 (mikem)
**          Sir #58499
**	    Made calls to dmd_return() and dmd_call() conditional on the dm1205
**	    tracepoint rather than xDEBUG.  This will make debugging some 
**	    problems in the field a lot easier at the cost of 2 extra 
**	    conditional branches per dmf call.
**      01-jun-1996 (ramra01 for bryanp)
**          Add case for DMU_ALTER_TABLE; call dmu_atable to process the work.
**	4-jun-96 (stephenb)
**	    Add case DMC_REP_QMAN (replicator queue management thread main
**	    entry point).
**      01-aug-1996 (nanpr01 for keving)
**          Add support for DMC_LK_CALLBACK special thread.
**	15-Nov-1996 (jenjo02)
**	    dmf_call(DMC_DEADLOCK_THREAD) will start the recovery server 
**	    Deadlock Detection Thread.
**	18-feb-98 (inkdo01)
**	    Added DMC_SORT_THREAD for parallel sort threads project.
**	13-apr-98 (inkdo01)
**	    Dropped DMC_SORT_THREAD in favour of factotum thread strategy.
**	14-May-1998 (jenjo02)
**	    Replaced DMC_WRITE_BEHIND with DMC_START_WB_THREADS.
**	    Call dm0p_start_wb_threads() instead of dmc_write_behind().
**	11-may-2001 (devjo01)
**	    Add support for DMC_CSP_x calls.
**	09-Apr-2004 (jenjo02)
**	    Added DMR_UNFIX.
**	15-Jul-2004 (jenjo02)
**	    For || queries: until QEF can tell us otherwise,
**	    set dmt_tran_id from DMF's SCB rather than relying
**	    on its contents, which for || query threads will
**	    be (incorrectly) the main query session's tranid.
**	30-Jul-2004 (jenjo02)
**	    QEF fixed to pass correct transaction context in
**	    dmt_tran_id, removed temp 15-Jul change.
**	03-Nov-2004 (jenjo02)
**	    Relocated CSswitch to dmr_get, where we can more easily
**	    bypass the call for Factotum threads.
**	09-may-2005 (devjo01)
**	    Add DMC_DIST_DEADLOCK_THR in support of VMS clusters.
*/
DB_STATUS
dmf_call(operation, control_block)
DM_OPERATION        operation;
PTR		    control_block;
{
    DMC_CB	    *cb = (DMC_CB *)control_block;
    i4	    err_code;
    DB_STATUS	    local_status;
    i4		    local_error;
    DB_STATUS	    status;
    EX_CONTEXT	    context;
	
    /*	Make a cursory check for bad parameters. */
#ifdef PERF_TEST
    CSfac_stats(TRUE, CS_DMF_ID);
#endif
#ifdef xDEV_TRACE
    CSfac_trace(TRUE, CS_DMF_ID, operation, control_block);
#endif

    cb->error.err_code = 0;
    cb->error.err_data = 0;
    if (operation >= DM_NEXT_OP ||
	cb_check[operation].cb_type != cb->type ||
	cb_check[operation].cb_size > cb->length)
    {
	/*  Figure out the error in more detail. */

	if (operation >= DM_NEXT_OP || cb_check[operation].cb_type == 0)
	    cb->error.err_code = E_DM006D_BAD_OPERATION_CODE;
	else if (cb_check[operation].cb_type != cb->type)
	    cb->error.err_code = E_DM000C_BAD_CB_TYPE;
	else
	    cb->error.err_code = E_DM000B_BAD_CB_LENGTH;
	return (E_DB_ERROR);
    }

    if ((dmf_svcb !=0) && DMZ_CLL_MACRO(5))
	dmd_call(operation, (PTR)control_block, cb->error.err_code);

    if (EXdeclare(ex_handler, &context) == OK &&
	(dmf_svcb == 0 || (dmf_svcb->svcb_status & SVCB_CHECK) == 0))
    {
	switch (operation)
	{
    
	/* RECORD operations. */

	case DMR_DELETE:
	    status = dmr_delete((DMR_CB *)cb);
	    break;

	case DMR_GET:
	    status = dmr_get((DMR_CB *)cb);
	    break;

	case DMR_POSITION:
	    status = dmr_position((DMR_CB *)cb);
	    break;

	case DMR_PUT:
	    status = dmr_put((DMR_CB *)cb);
	    break;

	case DMR_REPLACE:
	    status = dmr_replace((DMR_CB *)cb);
	    break;

	case DMR_LOAD:
	    status = dmr_load((DMR_CB *)cb);
	    break;
	
	case DMR_DUMP_DATA:
	    status = dmr_dump_data((DMR_CB *)cb);
	    break;

	case DMR_ALTER:
	    status = dmr_alter((DMR_CB *)cb);
	    break;

	case DMR_AGGREGATE:
	    status = dmr_aggregate((DMR_CB *)cb);
	    break;

	case DMR_UNFIX:
	    status = dmr_unfix((DMR_CB *)cb);
	    break;
    
	/* TABLE operations. */

	case DMT_CLOSE:
	    status = dmt_close((DMT_CB *)cb);
	    break;

	case DMT_OPEN:
	    status = dmt_open((DMT_CB *)cb);
	    break;

	case DMT_SHOW:
	    status = dmt_show((DMT_SHW_CB *)cb);
	    break;

	case DMT_ALTER:
	    status = dmt_alter((DMT_CB *)cb);
	    break;

	case DMT_CREATE_TEMP:
	    status = dmt_create_temp((DMT_CB *)cb);
	    break;

	case DMT_DELETE_TEMP:
	    status = dmt_delete_temp((DMT_CB *)cb);
	    break;

	case DMT_SORT_COST:
	    status = dmt_sort_cost((DMT_CB *)cb);
	    break;

	case DMT_INVALIDATE_TCB:
	    status = dm2tInvalidateTCB((DMT_CB *)cb);
	    break;

        case DMT_TABLE_LOCKED:
            status = dmt_table_locked((DMT_CB *)cb);
            break;

        case DMT_RELEASE_TABLE_LOCKS:
            status = dmt_release_table_locks((DMT_CB *)cb);
            break;

        case DMT_CONTROL_LOCK:
            status = dmt_control_lock((DMT_CB *)cb);
            break;
    
	/*	TRANSACTION operations. */

	case DMX_BEGIN:
	    status = dmx_begin((DMX_CB *)cb);
	    break;

	case DMX_COMMIT:
	    status = dmx_commit((DMX_CB *)cb);
	    break;

	case DMX_ABORT:
	    status = dmx_abort((DMX_CB *)cb);
	    break;

	case DMX_SAVEPOINT:
	    status = dmx_savepoint((DMX_CB *)cb);
	    break;

	case DMX_SECURE:
	    status = dmx_secure((DMX_CB *)cb);
	    break;

	case DMX_RESUME:
	    status = dmx_resume((DMX_CB *)cb);
	    break;
    
	/*	UTILITY operations. */

	case DMU_CREATE_TABLE:
	    status = dmu_create((DMU_CB *)cb);
	    break;

	case DMU_DESTROY_TABLE:
	    status = dmu_destroy((DMU_CB *)cb);
	    break;

	case DMU_RELOCATE_TABLE:
	    status = dmu_relocate((DMU_CB *)cb);
	    break;

	case DMU_INDEX_TABLE:
	    status = dmu_index((DMU_CB *)cb);
	    break;

	case DMU_PINDEX_TABLE:
	    status = dmu_pindex((DMU_CB *)cb);
	    break;

	case DMU_MODIFY_TABLE:
	    status = dmu_modify((DMU_CB *)cb);
	    break;

	case DMU_ALTER:
	case DMU_SHOW:
	    status = E_DB_OK;
	    break;

	case DMU_GET_TABID:
	    status = dmu_tabid((DMU_CB *)cb);
	    break;

	case DMU_CONVERT_TABLE:
	    status = dmu_convert((DMU_CB *)cb);
	    break;

	case DMU_ALTER_TABLE:			
	    status = dmu_atable((DMU_CB *)cb);
	    break;

    
	/* CONTROL operations. */

	case DMC_START_SERVER:
	    status = dmc_start_server((DMC_CB *)cb);
	    break;

	case DMC_STOP_SERVER:
	    status = dmc_stop_server((DMC_CB *)cb);
	    break;

	case DMC_WRITE_ALONG:  
	    status = dmc_write_along((DMC_CB *)cb);
	    break;

	case DMC_READ_AHEAD:  
	    status = dmc_read_ahead((DMC_CB *)cb);
	    break;

	case DMC_FAST_COMMIT:
	    status = dmc_fast_commit((DMC_CB *)cb);
	    break;

	case DMC_RECOVERY_THREAD:
	    status = dmc_recovery((DMC_CB *)cb);
	    break;

	case DMC_CHECK_DEAD:
	    status = dmc_check_dead((DMC_CB *)cb);
	    break;

	case DMC_GROUP_COMMIT:
	    status = dmc_group_commit((DMC_CB *)cb);
	    break;

	case DMC_FORCE_ABORT:
	    status = dmc_force_abort((DMC_CB *)cb);
	    break;

	case DMC_LOGWRITER:
	    status = dmc_logwriter((DMC_CB *)cb);
	    break;

	case DMC_CP_TIMER:
	    status = dmc_cp_timer((DMC_CB *)cb);
	    break;

	case DMC_ADD_DB:
	    status = dmc_add_db((DMC_CB *)cb);
	    break;

	case DMC_DEL_DB:
	    status = dmc_del_db((DMC_CB *)cb);
	    break;

	case DMC_BEGIN_SESSION:
	    status = dmc_begin_session((DMC_CB *)cb);
	    break;

	case DMC_END_SESSION:
	    status = dmc_end_session((DMC_CB *)cb);
	    break;

	case DMC_OPEN_DB:
	    status = dmc_open_db((DMC_CB *)cb);
	    break;

	case DMC_CLOSE_DB:
	    status = dmc_close_db((DMC_CB *)cb);
	    break;

	case DMC_ALTER:
	    status = dmc_alter((DMC_CB *)cb);
	    break;

	case DMC_SHOW:
	    status = dmc_show((DMC_CB *) cb);
	    break;

	case DMC_RESET_EFF_USER_ID:
	    status = dmc_reset_eff_user_id(cb);
	    break;

	case DMC_REP_QMAN:
	    status = dmc_repq((DMC_CB *) cb);
	    break;

        case DMC_LK_CALLBACK:
            status = lk_callback((DMC_CB *)cb);
            break;

        case DMC_DEADLOCK_THREAD:
            status = LKdeadlock_thread((DMC_CB *)cb);
            break;

	case DMC_START_WB_THREADS:
	    status = dm0p_start_wb_threads();
	    break;

	case DMC_CSP_MAIN:
	    status = dmfcsp((DMC_CB *)cb);
	    break;

	case DMC_CSP_MSGMON:
	    status = dmfcsp_msg_monitor((DMC_CB *)cb);
	    break;

	case DMC_CSP_MSGTHR:
	    status = dmfcsp_msg_thread((DMC_CB *)cb);
	    break;

	case DMC_DIST_DEADLOCK_THR:
	    status = LKdist_deadlock_thread((DMC_CB *)cb);
	    break;

	case DMC_GLC_SUPPORT_THR:
	    status = CXdlm_glc_support();
	    break;

    
	/*	MAINTANENCE operations. */

	case DMM_CREATE_DB:
	    status = dmm_create((DMM_CB *)cb);
	    break;

	case DMM_DESTROY_DB:
	    status = dmm_destroy((DMM_CB *)cb);
	    break;

	case DMM_ALTER_DB:
	    status = dmm_alter((DMM_CB *)cb);
	    break;

	case DMM_ADD_LOC:
	    status = dmm_add_location((DMM_CB *)cb);
	    break;

	case DMM_DEL_LOC:
	    status = dmm_del_location((DMM_CB *)cb);
	    break;

	case DMM_LISTFILE:
	    status = dmm_list((DMM_CB *)cb);
	    break;

	case DMM_DROPFILE:
	    status = dmm_delete((DMM_CB *)cb);
	    break;

	case DMM_CONFIG:
	    status = dmm_config((DMM_CB *)cb);
	    break;

	case DMM_DEL_DUMP_CONFIG:
	    status = dmm_del_config((DMM_CB *)cb);
	    break;

	case DMM_FINDDBS:
	    status = dmm_finddbs((DMM_CB *)cb);
	    break;

    
	/*	DEBUG operations. */

	case DMD_ALTER_CB:
	case DMD_CANCEL_TRACE:
	case DMD_DISPLAY_TRACE:
	case DMD_GET_CB:
	case DMD_RELEASE_CB:
	case DMD_SET_TRACE:
	case DMD_SHOW_CB:
	    status = E_DB_OK;
	    break;


	/* Sequence generator operations */
	case DMS_OPEN_SEQ:
	    status = dms_open_seq((PTR)cb);
	    break;
	case DMS_CLOSE_SEQ:
	    status = dms_close_seq((PTR)cb);
	    break;
	case DMS_NEXT_SEQ:
	    status = dms_next_seq((PTR)cb);
	    break;
	    
	/* XA transaction operations */

	case DMX_XA_START:
	    status = dmx_xa_start((DMX_CB *)cb);
	    break;

	case DMX_XA_END:
	    status = dmx_xa_end((DMX_CB *)cb);
	    break;

	case DMX_XA_PREPARE:
	    status = dmx_xa_prepare((DMX_CB *)cb);
	    break;

	case DMX_XA_COMMIT:
	    status = dmx_xa_commit((DMX_CB *)cb);
	    break;

	case DMX_XA_ROLLBACK:
	    status = dmx_xa_rollback((DMX_CB *)cb);
	    break;

	default:
	    cb->error.err_code = E_DM002A_BAD_PARAMETER;
	    status = E_DB_ERROR;
	    break;
	}

	if ((dmf_svcb !=0) && DMZ_CLL_MACRO(5))
	    dmd_return(operation, (PTR)control_block, status);

	EXdelete();
#ifdef PERF_TEST
	CSfac_stats(FALSE, CS_DMF_ID);
#endif
#ifdef xDEV_TRACE
        CSfac_trace(FALSE, CS_DMF_ID, operation, control_block);
#endif

	if (dmf_svcb !=0 && status == E_DB_ERROR 
		&& cb->error.err_code == E_DM002A_BAD_PARAMETER)
	{
	    ule_format(E_DM002A_BAD_PARAMETER, (CL_SYS_ERR *)0,
		    ULE_LOG, NULL, (char * )0, (i4)0, (i4 *)0, &local_error, 0);
	    dmd_call(operation, (PTR)control_block, cb->error.err_code);

	    /* Now spin, so dba can do something useful, like a pstack */
	    if (DMZ_CLL_MACRO(5) )
	    {
		TRdisplay("%@ E_DM002A_BAD_PARAMETER, spinning!\n"); for(;;);
	    }
	}

        return (status);
    }

    /*
    **	If exception handler declares or DMF is inconsistent already
    **	just return a severe error.
    */

    EXdelete();
    cb->error.err_code = E_DM004A_INTERNAL_ERROR;
    return (E_DB_FATAL);
}

/*{
** Name: ex_handler	- DMF exception handler.
**
** Description:
**      This routine will catch all DMF exceptions.  Any exception caught
**	by DMF is considered SEVERE.  DMf is marked as inconsistent and
**	all future callers will be return an error of E_DM004A_INTERNAL_ERROR.
**
** Inputs:
**      ex_args                         Exception arguments.
**
** Outputs:
**	Returns:
**	    EXDECLARE
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      05-feb-1987 (Derek)
**          Added.
**	19-sep-1987 (rogerk)
**	    Check for EX_DMF_FATAL exception.  Don't dump dmf control blocks
**	    on this exception since we already dumped them.
**	25-sep-1991 (mikem) integrated following change: 16-aug-1991 (rog)
**	    Set buffer to be EX_MAX_SYS_REP+15 instead of 256.
**	07-jul-1993 (rog)
**	    Removed error reporting code and called ulx_exception() instead.
**	09-Jan-2007 (kiria01) b117452
**	    Added missing EXdelete() to clean off nested handler from stack.
[@history_template@]...
*/
static STATUS
ex_handler(
EX_ARGS		    *ex_args)
{
    i4	    err_code;
    i4	    status;
    EX_CONTEXT	context;

if (EXdeclare(ex_2handler, &context) != OK)
    {
        EXdelete();
        return (EXDECLARE);
    }


#ifdef VMS
    status = sys$setast(0);
#endif
    
    if (ex_args->exarg_num == EX_DMF_FATAL)
    {
	err_code = E_DM904A_FATAL_EXCEPTION;
    }
    else
    {
	err_code = E_DM9049_UNKNOWN_EXCEPTION;
    }

    /*
    ** Report the exception.
    ** Note: Usually ulx_exception is called with the fourth argument as
    ** TRUE, which tells the function to call scs_avformat() to dump the
    ** session into to the errlog.  However, in DMF's case, the dmd_check()
    ** code already calls scs_avformat(), so we don't need to do so here.
    */
    ulx_exception( ex_args, DB_DMF_ID, err_code, FALSE );

    /* If we have already done a dmd_check, no need to redump dmf objects */
    if (ex_args->exarg_num != EX_DMF_FATAL)
	dmd_dmp_pool(1);

    dmf_svcb->svcb_status |= SVCB_CHECK;
#ifdef VMS
    if (status == 9)
	sys$setast(1);
#endif
    EXdelete();
    return (EXDECLARE);
}

static STATUS
ex_2handler(
        EX_ARGS            *ex_args)
{
    return (EXDECLARE);
}
