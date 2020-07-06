/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <ex.h>
#include    <me.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ulf.h>
#include    <adf.h>
#include    <scf.h>
#include    <dmf.h>
#include    <dmrcb.h>
#include    <dmtcb.h>
#include    <ulm.h>
#include    <ulx.h>
#include    <gwf.h>
#include    <gwfint.h>

/**
** Name: GWFCALL.C	- The main GWF entry point
**
** Description:
**	This file contains gwf_call(), which is the main entry point to
**	GWF and the means by which all other facilities invoke GWF
**	services.
**
**	Routines:
**
**	gwf_call	    - Primary GWF entry point.
**
** History:
**	6-apr-90 (bryanp)
**	    Created.
**	4-feb-91 (linda)
**	    Code cleanup -- in particular, update structure member names.
**      18-may-92 (schang)
**          GW merge
**	8-oct-92 (daveb)
**	    include <gwfint.h> for prototypes of called functions.
**      16-nov-92 (schang)
**          prototype
**	18-dec-92 (robf)
**	    Add call to verify gateway id/name (GWU_VALIDGW).
**	    Improve exception handling to new standard by calling
**	    ulx_exception().
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**      26-jul-93 (schang)
**          reset gwf interrupt handler when enter GWF
**	27-jul-93 (ralph)
**	    DELIM_IDENT:
**	    Add support for GWS_ALTER opcode to alter the session's
**	    case translation semantics.
**	28-Jul-1993 (daveb)
**	    Work around segv on init from checking interrupt handling
**	    when there's no session.
**	26-Oct-1993 (daveb) 55487
**	    Fixed interrupt handling problems.   If interrupt is set in
**	    one query, it is not reset, and entry to second query can
**	    return interrupted.  (For instance, 'help' followed by a
**	    select from a GWF table.)  We need to reset the interrupt
**	    field on entry in certain operations, and check them later.
*/

/*
** Definition of static variables and forward static functions.
*/

static	STATUS	gwf_handler();	/* GWF catch-all exception handler. */

/*{
** Name: gwf_call	- Primary GWF entry point.
**
** Description:
**	This routine checks that the arguments to gwf_call look reasonable
**	and that the appropriate control block type and size is passed.
**	Then it calls the appropriate GWF function to execute the desired
**	operation and the operation completion status is returned to the
**	caller.
**
** Inputs:
**	operation	    - The GWF operation code
**	control_block	    - The GWF control block (currently GW_RCB always)
**
** Outputs:
**	None
**
** Returns:
**	DB_STATUS
**
** Exceptions & Side Effects:
**	None
**
** History:
**	6-apr-90 (bryanp)
**	    Created.
**	8-oct-92 (daveb)
**	    removed dead variable "err_code"
**	18-dec-92 (robf)
**	    Add operation gwu_validgw.
**      06-jul-93 (schang)
**          reset gwf interrupt handler when enter GWF
**	27-jul-93 (ralph)
**	    DELIM_IDENT:
**	    Add support for GWS_ALTER opcode to alter the session's
**	    case translation semantics.
**	28-Jul-1993 (daveb)
**	    Work around segv on init from checking interrupt handling
**	    when there's no session.
**	26-Oct-1993 (daveb) 55487
**	    Fixed interrupt handling problems correctly, solving
**	    riddled left by the 06-jul-92 and 28-jul-92 change and
**	    backout.  If interrupt is set in one query, it is not
**	    reset, and entry to second query can return interrupted.
**	    (For instance, 'help' followed by a select from a GWF
**	    table.)  We need to reset the interrupt field on entry in
**	    certain operations, and check them later.  That's why we
**	    tried to call gws_check_interupt on entry.  But this is
**	    only correct when there is a session already; so call
**	    check in the likely places inside the switch instead of
**	    once for everyone.
*/
DB_STATUS
gwf_call
(
    GWF_OPERATION	operation,
    GW_RCB		*control_block
)
{
    EX_CONTEXT	context;
    DB_STATUS	status;

    /*
    ** Make some simple checks for bad parameters and incorrect operations.
    */

    if (control_block == (GW_RCB *)0)
    {
	/* not much we can do to log an error... */
	return (E_DB_ERROR);
    }

    if (operation < GWF_MIN_OPERATION || operation > GWF_MAX_OPERATION)
    {
	control_block->gwr_error.err_code = E_GW0660_BAD_OPERATION_CODE;
	return (E_DB_ERROR);
    }

    if (control_block->gwr_type != GWR_CB_TYPE)
    {
	control_block->gwr_error.err_code = E_GW0661_BAD_CB_TYPE;
	return (E_DB_ERROR);
    }

    if (control_block->gwr_length != sizeof(GW_RCB))
    {
	control_block->gwr_error.err_code = E_GW0662_BAD_CB_LENGTH;
	return (E_DB_ERROR);
    }

    /*
    ** Plant an exception handler so that if any lower-level GWF routine
    ** aborts, we can attempt to trap the error and "do the right thing".
    */
    if (EXdeclare( gwf_handler, &context) != OK)
    {
	/*
	** We get here if an exception occurs and the handler longjumps
	** back to us (returns EXDECLARE). In this case, something HORRID
	** has occurred, so let's complain loudly to our caller:
	*/
	EXdelete();
	control_block->gwr_error.err_code = E_GW0663_INTERNAL_ERROR;
	return (E_DB_FATAL);
    }

    /*
    ** Now call the appropriate GWF routine based on the operation code.
    ** Return the status from the GWF routine to the caller.
    */
    switch( operation )
    {
	case GWF_INIT:
	    status = gwf_init( control_block );
	    break;

	case GWF_TERM:
	    status = gwf_term( control_block );
	    break;

	case GWF_SVR_INFO:
	    status = gwf_svr_info( control_block );
	    break;

	case GWS_INIT:
	    status = gws_init( control_block ); break;

	case GWS_TERM:
	    (void) gws_check_interrupt(
			(GW_SESSION *)control_block->gwr_session_id);
	    status = gws_term( control_block );
	    break;

	case GWS_ALTER:
	    (void) gws_check_interrupt(
			(GW_SESSION *)control_block->gwr_session_id);
	    status = gws_alter( control_block );
	    break;

	case GWT_REGISTER:
	    (void) gws_check_interrupt(
			(GW_SESSION *)control_block->gwr_session_id);
	    status = gwt_register( control_block );
	    break;

	case GWT_REMOVE:
	    (void) gws_check_interrupt(
			(GW_SESSION *)control_block->gwr_session_id);
	    status = gwt_remove( control_block );
	    break;

	case GWT_OPEN:
	    (void) gws_check_interrupt(
			(GW_SESSION *)control_block->gwr_session_id);
	    status = gwt_open( control_block );
	    break;

	case GWT_CLOSE:
	    (void) gws_check_interrupt(
			(GW_SESSION *)control_block->gwr_session_id);
	    status = gwt_close( control_block );
	    break;

	case GWI_REGISTER:
	    (void) gws_check_interrupt(
			(GW_SESSION *)control_block->gwr_session_id);
	    status = gwi_register( control_block );
	    break;

	case GWI_REMOVE:
	    (void) gws_check_interrupt(
			(GW_SESSION *)control_block->gwr_session_id);
	    status = gwi_remove( control_block );
	    break;

	case GWR_POSITION:
	    (void) gws_check_interrupt(
			(GW_SESSION *)control_block->gwr_session_id);
	    status = gwr_position( control_block );
	    break;

	case GWR_GET:
	    (void) gws_check_interrupt(
			(GW_SESSION *)control_block->gwr_session_id);
	    status = gwr_get( control_block );
	    break;

	case GWR_PUT:
	    (void) gws_check_interrupt(
			(GW_SESSION *)control_block->gwr_session_id);
	    status = gwr_put( control_block );
	    break;

	case GWR_REPLACE:
	    (void) gws_check_interrupt(
			(GW_SESSION *)control_block->gwr_session_id);
	    status = gwr_replace( control_block );
	    break;

	case GWR_DELETE:
	    (void) gws_check_interrupt(
			(GW_SESSION *)control_block->gwr_session_id);
	    status = gwr_delete( control_block );
	    break;

	case GWX_BEGIN:
	    (void) gws_check_interrupt(
			(GW_SESSION *)control_block->gwr_session_id);
	    status = gwx_begin( control_block );
	    break;

	case GWX_ABORT:
	    (void) gws_check_interrupt(
			(GW_SESSION *)control_block->gwr_session_id);
	    status = gwx_abort( control_block );
	    break;

	case GWX_COMMIT:
	    (void) gws_check_interrupt(
			(GW_SESSION *)control_block->gwr_session_id);
	    status = gwx_commit( control_block );
	    break;

	case GWU_INFO:
	    status = gwu_info( control_block );
	    break;
	
	case GWU_VALIDGW:
	    status = gwu_validgw( control_block );
	    break;

	default:
	    /*
	    ** Falling through means that our case statement is out of sync with
	    ** the known GWF operations and indicates a severe coding error.
	    */
	    control_block->gwr_error.err_code = E_GW0664_UNKNOWN_OPERATION;
	    status = E_DB_ERROR;
	    break;
    }

    EXdelete();
    return (status);
}

/*{
** Name: gwf_handler	    - GWF exception handler
**
** Description:
**	This routine will catch all GWF exceptions. Currently, any exception
**	caught by GWF is considered a severe problem.
**
** Inputs:
**	ex_args		    - Exception arguments, as called by EX
**
** Outputs:
**	None
**
** Returns:
**	EXDECLARE
**
** Exceptions and Side Effects:
**	The EXDECLARE return causes an abrupt return to gwf_call()
**
** History:
**	6-apr-90 (bryanp)
**	    Created.
**	10-jul-91 (rickh)
**	    Corrected bug in buffer length found by the ICL port.
**	18-dec-92 (robf)
**	    Upgrade to new handling by calling ulx_exception.
*/

static STATUS
gwf_handler
(
    EX_ARGS	    *ex_args
)
{
    /*
    **	Report the exception
    */
    ulx_exception (ex_args, DB_GWF_ID, E_GW0668_UNEXPECTED_EXCEPTION, TRUE);

    return (EXDECLARE);
}
