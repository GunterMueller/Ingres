/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <er.h>
#include    <ex.h>
#include    <cs.h>
#include    <cv.h>
#include    <tm.h>
#include    <tr.h>
#include    <pc.h>
#include    <st.h>
#include    <sl.h>
#include    <lk.h>
#include    <iicommon.h>
#include    <dbdbms.h>

#include    <ddb.h>
#include    <ulf.h>
#include    <ulx.h>
#include    <scf.h>
#include    <qsf.h>	/* for scs control block */
#include    <adf.h>	/* for scs cb */
#include    <gca.h>
#include    <generr.h>
#include    <sqlstate.h>

/* added for scs.h prototypes, ugh! */
#include <dudbms.h>
#include <ulm.h>
#include <dmf.h>
#include <dmccb.h>
#include <dmrcb.h>
#include <copy.h>
#include <qefrcb.h>
#include <qefqeu.h>
#include <qefcopy.h>

#include    <sc.h>
#include    <scc.h>
#include    <scs.h>
#include    <scd.h>
#include    <sc0e.h>
#include    <sc0m.h>
#include    <sce.h>
#include    <scfcontrol.h>

/**
**
**  Name: SC0E.C - Log error messages from SCF
**
**  Description:
**      This routine writes (via ule_format()) error messages to the Ingres 
**      error log.  It is provided as a common interface so that errors may 
**      be related to one another.
**
**          sc0e_put - write an error message.
**	    sc0e_uput - write an error message to the user
**
**
**  History:
**      30-jun-1986 (fred)    
**          Created on Jupiter.
**	21-may-89 (jrb)
**	    New ule_format interface changes, and passes generic error code to
**	    scc_error.
**	13-jul-92 (rog)
**	    Changed sc0e_tput so that it can send its output to either
**	    SCC_TRACE or the errlog depending on a mask passed in.
**	12-Mar-1993 (daveb)
**	    Add include <st.h>
**	24-may-1993 (bryanp)
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**	29-Jun-1993 (daveb)
**	    Include <tr.h>, correctly cast arg to CSget_scb.
**	30-jun-1993 (shailaja)
**	    Fixed prototype incompatibilities.
**	2-Jul-1993 (daveb)
**	    prototyped, add sc0e_0_pyt and sc0e_0_uput.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	13-sep-93 (swm)
**	    Include cv.h to pickup CV_HEX_PTR_SIZE define which is now
**	    needed by scs.h.
**      28-Oct-1996 (hanch04)
**          Changed SS50000_MISC_ING_ERRORS to SS50000_MISC_ERRORS
**	22-Oct-1998 (jenjo02)
**	    Corrected miscasts of CS_SCB to SCF_SESSION. Caused BUS
**	    errors on HP-11.
**	26-apr-1999 (hanch04)
**	    Added casts to quite compiler warnings
**      04-may-1999 (hanch04)
**          Change TRformat's print function to pass a PTR not an i4.
**	30-jun-1999 (hanch04)
**	    Corrected casts removed by cleanup change.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	04-Jan-2001 (jenjo02)
**	    Pass CS_SCB* in scf_session to scf_call() rather than
**	    DB_NOSESSION.
**      23-Apr-2001 (hweho01)
**          Changed sc0e_put() to a varargs function, avoid stack
**          corruptions.
**	10-Jun-2004 (schka24)
**	    Don't use ule-format macro for user message lookups, leaves
**	    little TRdisplay droppings in the DBMS log file.
**	    FIXME there are lots more of these, but I don't have time
**	    to fix them all -- just getting the most annoying one.
**/

/*
**  Forward and/or External function references.
*/

GLOBALREF SC_MAIN_CB *Sc_main_cb;        /* core struct of scf */    

/*{
** Name: sc0e_put	- write an error message to the log
**
** Description:
**      This routine writes an error message to the Ingres error log. 
**      The message, along with its parameters, is sent to ule_format() 
**      augmented with the server id, the session id, and the error sequence 
**      number for the server.
**
** Inputs:
**      scf_error                       the scf error to be logged.
**      os_error                        the os error if appropriate
**	num_params			the number of param pairs to follow
**      param_1_length                  length of ...
**      param_1                         the first parameter
**      ...                             more param pairs
**
** Outputs:
**      none
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	30-Jun-1986 (fred)
**          Created on Jupiter
**	28-jan-1989 (jrb)
**	    Added num_params to sc0e_put.
**	09-mar-1989 (jrb)
**	    Now passes num_params arg thru to ule_format.
**	21-may-89 (jrb)
**	    Changed calls to ule_format to pass NULL as 4th parm.
**	13-jul-92 (rog)
**	    Result of call to CSget_sid was never used.  Call and otherwise
**	    unused local variable eliminated.  (Integrating seg's change from
**	    6.4.)
**	25-oct-92 (andre)
**	    interface of ule_format() has changed: instead of generic_error
**	    (i4 *), it expects sqlstate (DB_SQLSTATE *)
**      06-apr-1993 (smc)
**          Cast parameters to match prototypes.
**	29-Jun-1993 (daveb)
**	    correctly cast arg to CSget_scb.
**	2-Jul-1993 (daveb)
**	    prototyped.  os_error is properly a CL_ERR_DESC *, not a i4.
[@history_template@]...
*/
VOID
sc0e_put(int scf_error,
         CL_ERR_DESC *os_error,
         int num_params,
         ... )
{
    DB_STATUS           status;
    i4             error;
    int param_size[6];
    PTR param_value[6];
    int i; 

    va_list ap;
    va_start( ap , num_params );

    for( i = 0;  i < num_params ; i++ )
       {
        param_size[i] = (int) va_arg( ap, int );
        param_value[i] = ( PTR ) va_arg( ap, PTR );
       }
    va_end( ap );

    status = ule_format(scf_error, os_error, ULE_LOG, (DB_SQLSTATE *) NULL, 0,
	0, 0, &error, num_params, param_size[0], param_value[0], 
        param_size[1], param_value[1], param_size[2], param_value[2],
        param_size[3], param_value[3], param_size[4], param_value[4],
        param_size[5], param_value[5]);

    if (Sc_main_cb)
    {
	if (DB_FAILURE_MACRO(status))
	{
	    status = ule_format(E_SC012A_BAD_MESSAGE_FOUND, 0, ULE_LOG,
		(DB_SQLSTATE *) NULL, 0, 0, 0, &error, 1,
		(scf_error ? sizeof(scf_error) : sizeof(os_error)),
		(scf_error ? (PTR)&scf_error : (PTR)&os_error));
	    if (DB_FAILURE_MACRO(status))
	    {
		ule_format(0, 0, ULE_MESSAGE, (DB_SQLSTATE *) NULL,
		    "E_????_ERROR_LOGGING_&_FORMATTING_MESSAGE",
		     41, 0, &error, 0);
	    }
	}
	if (ult_check_macro(&Sc_main_cb->sc_trace_vector, SCT_SCRLOG, 0, 0))
	{
	    SCF_CB			*scf_cb;
	    char			msg_area[DB_ERR_SIZE];    
	    SCD_SCB		*scb;

	    CSget_scb((CS_SCB **)&scb);
	
	    if ((scb) && (scb->scb_sscb.sscb_scccb))
	    {
		scf_cb = (SCF_CB *)scb->scb_sscb.sscb_scccb;
		if (scf_cb)
		{
		    scf_cb->scf_facility = DB_SCF_ID;
		    scf_cb->scf_session = (SCF_SESSION)scb;
		    scf_cb->scf_length = sizeof(SCF_CB);
		    scf_cb->scf_type = SCF_CB_TYPE;
		    status = ule_doformat(scf_error, os_error, ULE_LOOKUP,
			(DB_SQLSTATE *) NULL, msg_area, sizeof(msg_area),
			&scf_cb->scf_len_union.scf_blength, 
			&error, num_params, param_size[0],
                        param_value[0], param_size[1], param_value[1], 
                        param_size[2], param_value[2], param_size[3],
                        param_value[3], param_size[4], param_value[4],
                        param_size[5], param_value[5]);
		    if (status == E_DB_OK)
		    {
			scf_cb->scf_ptr_union.scf_buffer = msg_area;
			scf_cb->scf_nbr_union.scf_local_error = scf_error;
			scf_call(SCC_TRACE, scf_cb);
		    }
		}
	    }
	}
    }
}


VOID
sc0e_0_put(i4 scf_error, CL_ERR_DESC *os_error )
{
    sc0e_put( scf_error, os_error, 0,
	     0, (PTR)0,
	     0, (PTR)0,
	     0, (PTR)0,
	     0, (PTR)0,
	     0, (PTR)0,
	     0, (PTR)0 );
}


/*{
** Name: sc0e_uput	- write an error message to the user
**
** Description:
**      This routine writes an error message to the Ingres user.
**      The message, along with its parameters, is sent to ule_format() 
**      augmented with the server id, the session id, and the error sequence 
**      number for the server.
**
** Inputs:
**      scf_error                       the scf error to be logged.
**      os_error                        the os error if appropriate
**	num_params			the number of param pairs to follow
**      param_1_length                  length of ...
**      param_1                         the first parameter
**      ...                             more param pairs
**
** Outputs:
**      none
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	24-Jul-1986 (fred)
**          Created on Jupiter
**	28-jan-1989 (jrb)
**	    Added num_params to sc0e_uput.
**	09-mar-1989 (jrb)
**	    Now passes num_params arg thru to ule_format.
**	21-may-89 (jrb)
**	    Now passes generic error code to scc_error after looking up thru
**	    ule_format.
**	13-jul-92 (rog)
**	    Added an (unsigned) to the 0xffff0000 mask.
**	25-oct-92 (andre)
**	    interface of ule_format() has changed: instead of generic_error
**	    (i4 *), it expects sqlstate (DB_SQLSTATE *) +
**	    in SCF_CB, scf_generic_error has been replaced with scf_sqlstate
**	29-Jun-1993 (daveb)
**	    correctly cast arg to CSget_scb.
**	2-Jul-1993 (daveb)
**	    Prototyped.  Pass correct type/number of args to sc0e_put.
**	    os_error is CL_ERR_DESC *, not i4.
[@history_template@]...
*/
VOID
sc0e_uput(i4 scf_error,
	  CL_ERR_DESC *os_error,
	  i4  num_params,
	  i4  p1l,  PTR p1,
	  i4  p2l,  PTR p2,
	  i4  p3l,  PTR p3,
	  i4  p4l,  PTR p4,
	  i4  p5l,  PTR p5,
	  i4  p6l,  PTR p6 )
{
    DB_STATUS           status;
    i4             error;
    i4			msg_owner;
    SCF_CB		*scf_cb;
    char		msg_area[DB_ERR_SIZE];    
    SCD_SCB		*scb;

    CSget_scb((CS_SCB **)&scb);
    
    if ((scb) && (scb->scb_sscb.sscb_scccb))
    {
	scf_cb = (SCF_CB *)scb->scb_sscb.sscb_scccb;
	scf_cb->scf_facility = DB_SCF_ID;
	scf_cb->scf_session = (SCF_SESSION)scb;
	scf_cb->scf_length = sizeof(SCF_CB);
	scf_cb->scf_type = SCF_CB_TYPE;
	msg_owner = (scf_error & (unsigned)0xffff0000) >> 16;
	status = ule_doformat(scf_error, os_error, ULE_LOOKUP,
		&scf_cb->scf_aux_union.scf_sqlstate, msg_area,
		sizeof(msg_area),
		&scf_cb->scf_len_union.scf_blength, 
		&error, num_params,
		p1l, p1, p2l, p2, p3l, p3, p4l, p4, p5l, p5, p6l, p6);
	if (DB_SUCCESS_MACRO(status))
	{
	    scf_cb->scf_ptr_union.scf_buffer = msg_area;
	    scf_cb->scf_nbr_union.scf_local_error = scf_error;
	    status = scf_call(SCC_ERROR, scf_cb);
	    if (status != E_DB_OK)
	    {
		sc0e_0_put(scf_cb->scf_error.err_code, 0 );
	    }
	}
	else
	{
	    char	*sqlstate_str = SS50000_MISC_ERRORS;
	    i4		i;
	    
	    scf_cb->scf_ptr_union.scf_buffer = "Text unavailable";
	    scf_cb->scf_len_union.scf_blength =
		STlength(scf_cb->scf_ptr_union.scf_buffer);
	    scf_cb->scf_nbr_union.scf_local_error = scf_error;
	    for (i = 0; i < DB_SQLSTATE_STRING_LEN; i++)
		scf_cb->scf_aux_union.scf_sqlstate.db_sqlstate[i] =
		    sqlstate_str[i];
	    status = scf_call(SCC_ERROR, scf_cb);
	    if (status != E_DB_OK)
	    {
		sc0e_0_put(scf_cb->scf_error.err_code, 0 );
	    }
	    sc0e_put(E_SC012A_BAD_MESSAGE_FOUND, 0, 1,
		     (scf_error ? sizeof(scf_error) : sizeof(os_error)),
		     (scf_error ? (PTR)&scf_error : (PTR)&os_error),
		     0, (PTR)0,
		     0, (PTR)0,
		     0, (PTR)0,
		     0, (PTR)0,
		     0, (PTR)0 );
	}
    }
}

VOID
sc0e_0_uput(i4 scf_error, CL_ERR_DESC *os_error )
{
    sc0e_uput( scf_error, os_error, 0,
	      0, (PTR)0,
	      0, (PTR)0,
	      0, (PTR)0,
	      0, (PTR)0,
	      0, (PTR)0,
	      0, (PTR)0 );
}


/*{
** Name: sc0e_trace	- Send trace statements to the FE
**
** Description:
**      This routine is called similar to TRdisplay(), but 
**      sends the string to the FE.
**
** Inputs:
**	scb				Scb to trace
**      fmt                             Format string (ala TRdisplay())
**      p1, p2, ...                     Parameters
**
** Outputs:
**      None.
**	Returns:
**	    Void
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      20-Jan-88 (fred)
**          Created.
**	13-jul-92 (rog)
**	    Changed sc0e_tput so that it can send its output to either
**	    SCC_TRACE or the errlog depending on a mask passed in.
**	03-jun-1993 (rog)
**	    If sc0e_tput() is sending output to the errlog, it first calls
**	    ulx_format_query() to break up the text in <80 character lines
**	    that are split at reasonable places.
**	29-Jun-1993 (daveb)
**	    correctly cast arg to CSget_scb.
**	2-Jul-1993 (daveb)
**	    Tried to prototype, but callers's are type incorrect.
**	    Easier for now to leave it old-style.
**	13-jul-1993 (rog)
**	    print_mask must be initialized to the correct flags or the trace
**	    output from sc0e_trace() will be silently discarded.  (bug 53118)
**	11-oct-93 (swm)
**	    Bug #56448
**	    Altered sc0e_trace calls to conform to new portable interface.
**	    It is no longer a "pseudo-varargs" function. It cannot become a
**	    varargs function as this practice is outlawed in main line code.
**	    Instead it takes a formatted buffer as its only parameter.
**	    Each call which previously had additional arguments has an
**	    STprintf embedded to format the buffer to pass to sc0e_trace.
**	    This works because STprintf is a varargs function in the CL.
**      27-Jun-2007 (kiria01) b118501
**          The previous change did not consider calls to this function where
**          it was used as a parameter function such as with calls from
**          adu_2prvalue - not all of these explicitly pass through STprintf.
**          Specificly it is possible for a raw databuffer to be presented which
**          might have character sequences that could be mistaken for format
**          specifiers. To make sure this is handled, we now pass via "%s".
[@history_template@]...
*/
VOID
sc0e_trace( char *fmt )
{
    char		buffer[SC0E_FMTSIZE]; /* last char for `\n' */
    i4		print_mask = SCE_FORMAT_NEEDNUL | SCE_FORMAT_GCA;

    /* TRformat removes `\n' chars, so to ensure that psf_scctrace()
    ** outputs a logical line (which it is supposed to do), we allocate
    ** a buffer with one extra char for NL and will hide it from TRformat
    ** by specifying length of 1 byte less. The NL char will be inserted
    ** at the end of the message by scs_tput().
    */
    TRformat(sc0e_tput, &print_mask, buffer, sizeof(buffer) - 1, "%s", fmt);
}

/*
** History:
**	2-Jul-1993 (daveb)
**	    prototyped.
*/
i4
sc0e_tput( PTR arg1, i4 msg_length, char *msg_buffer )
{
    SCF_CB       *scf_cb;
    SCD_SCB	 *scb;
    DB_STATUS    status = E_DB_OK;
    i4	 mask;

    if (msg_length == 0)
	return;

    if (arg1 == (PTR)0)
	mask = SCE_FORMAT_NEEDNUL | SCE_FORMAT_GCA;
    else if (arg1 == (PTR)1)
	mask = SCE_FORMAT_GCA;
    else
	mask = *((i4 *) arg1);

    if (mask & SCE_FORMAT_GCA)
    {
	if (mask & SCE_FORMAT_NEEDNUL)
	{
	    /* Add a NL character */
	    *((char *)(msg_buffer + msg_length)) = '\n';
	    msg_length++;
	}    
    
	CSget_scb((CS_SCB **)&scb);

	scf_cb = (SCF_CB *) scb->scb_sscb.sscb_scccb;
	scf_cb->scf_length = sizeof(*scf_cb);
	scf_cb->scf_type = SCF_CB_TYPE;
	scf_cb->scf_facility = DB_PSF_ID;
	scf_cb->scf_len_union.scf_blength = (u_i4)msg_length;
	scf_cb->scf_ptr_union.scf_buffer = msg_buffer;
	scf_cb->scf_session = (SCF_SESSION)scb;	/* print to current session */

	status = scf_call(SCC_TRACE, scf_cb);
    }
    else if (mask & SCE_FORMAT_ERRLOG)
    {
	char		out_buffer[2048];
	i4		err_code;
    
	if (mask & SCE_FORMAT_NEEDNUL)
	{
	    /* Add a NL character */
	    *((char *)(msg_buffer + msg_length)) = '\n';
	    msg_length++;
	}    

	ulx_format_query(msg_buffer, msg_length, out_buffer,
			 sizeof(out_buffer));

	msg_length = STlength(out_buffer);

	status = ule_format(0, 0, ULE_MESSAGE, (DB_SQLSTATE *) NULL, out_buffer,
	    msg_length, 0, &err_code, 0);
    }

    if (status != E_DB_OK)
    {
	TRdisplay("SCS_TPUT: SCF error %d displaying query text to user\n",
	    scf_cb->scf_error.err_code);
    }
}
