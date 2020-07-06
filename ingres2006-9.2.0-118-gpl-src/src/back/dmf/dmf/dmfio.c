/*
** Copyright (c) 1995, 2005 Ingres Corporation
**
**
*/

# include <compat.h>
# include <gl.h>
# include <er.h>
# include <si.h>
# include <sl.h>
# include <cs.h>
# include <di.h>
# include <pc.h>
# include <tr.h>
# include <iicommon.h>
# include  <ulf.h>
# include  <dbdbms.h>
# include  <dmf.h>
# include  <dm.h>
# include  <lg.h>
# include  <lk.h>
# include  <dmp.h>
# include  <dm0c.h>
# include  <dmfjsp.h>

/*
**  dmfio.c - DMF I/O module.
**
**  Description:
**      This file contains the function calls for output to stdout and stderr.
**      These functions have been moved from dmfjsp.
**  History:
**      24-Oct-95 (fanra01)
**          Created.
**	23-sep-1996 (canor01)
**	    Updated.
**	26-apr-1999 (hanch04)
**	    Added casts to quite compiler warnings
**      04-may-1999 (hanch04)
**          Change TRformat's print function to pass a PTR not an i4.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	14-oct-2005 (mutma03/devjo01)
**	    Add dmf_write_msg_with_args
*/

static void write_msg_common( DB_STATUS status, DB_STATUS err_code,
                              char *message, i4 l_message );
/*{
** Name: dmf_write_msg	- Write message to error output.
**
** Description:
**      This routine will lookup a standard message and write the test of the
**	message to the error output.
**
**	It will also log the error message in the error log.
**
**	We also call TRdisplay to echo the message to the trace file. If
**	TRset_file was not called, this TRdisplay is a no-op, otherwise it
**	sends the message to the appropriate trace log opened by TRset_file.
**
** Inputs:
**      err_code                        Code for message to display.
**
** Outputs:
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      01-dec-1986 (Derek)
**          Created for Jupiter.
**	17-oct-1991 (rogerk)
**	    Great Merge of DMF : Move 6.4 changes into 6.5 line.
**
**	    30-apr-1991 (bryanp)
**		Change ULE_LOOKUP to ULE_LOG; messages written through this
**		subroutine will now be logged to the error log as well, as they
**		are generally error messages which we want to keep a record of.
**		Call TRdisplay so that utility trace logs are more complete.
**      16-nov-2005 (devjo01)
**	    Split portion of code into write_msg_common.
[@history_template@]...
*/
VOID
dmf_write_msg(
DB_STATUS	    err_code)
{
    char		message[ER_MAX_LEN];
    i4			l_message;
    DB_STATUS		local_err_code;
    DB_STATUS		status;

    status = ule_format(err_code, 0, ULE_LOG, NULL, message, sizeof(message),
	&l_message, &local_err_code, 0);
    write_msg_common( status, err_code, message, l_message );
}


/*{
** Name: dmf_write_msg_with_args	- Write message to error output.
**
** Description:
**      This routine will lookup a standard message and write the test of the
**	message to the error output, allowing the user to provide up to
**	NUM_ER_ARGS parameters.
**
**	It will also log the error message in the error log.
**
**	We also call TRdisplay to echo the message to the trace file. If
**	TRset_file was not called, this TRdisplay is a no-op, otherwise it
**	sends the message to the appropriate trace log opened by TRset_file.
**
** Inputs:
**      err_code                        Code for message to display.
**	numparams			# of argument pairs passed.
**	[ arg0_size, arg0, ... ]	Up to four pairs of arguments.
**					1st element of pair is an i4
**					size (normally 0 for integer
**					arguments, and '\0' terminated
**					strings.  2nd element of pair
**					is the actual argument cast as
**					a pointer.
**
** Outputs:
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      14-oct-2005 (mutma03/devjo01)
**	    Init version created from dmf_write_msg().
[@history_template@]...
*/
VOID
dmf_write_msg_with_args( DB_STATUS err_code, i4 numparams, ... )
{
#   define NUM_ER_ARGS	4
    char		message[ER_MAX_LEN+1];
    i4			l_message;
    DB_STATUS		local_err_code;
    DB_STATUS		status;
    va_list		ap;
    ER_ARGUMENT         er_args[NUM_ER_ARGS];
    i4			count;

    va_start( ap, numparams );
    for ( count = 0; count < numparams; count++ )
    {
	er_args[count].er_size = (int) va_arg( ap, int );
	er_args[count].er_value = (PTR) va_arg( ap, PTR );
    }

    status = ule_format(err_code, 0, ULE_LOG, NULL, message, ER_MAX_LEN,
	&l_message, &local_err_code, numparams,
	er_args[0].er_size, er_args[0].er_value,
	er_args[1].er_size, er_args[1].er_value,
	er_args[2].er_size, er_args[2].er_value,
	er_args[3].er_size, er_args[3].er_value );
    write_msg_common( status, err_code, message, l_message );
}

/*
** Common utility routine for dmf_write_msg[_with_args]
*/
static void
write_msg_common( DB_STATUS status, DB_STATUS err_code, 
                  char *message, i4 l_message )
{
    i4		count;

    if (status == E_DB_OK)
    {
	SIwrite(l_message, message, &count, stderr);
	SIwrite(1, "\n", &count, stderr);

	TRdisplay("%t\n", l_message, message);
    }
    else
	SIprintf("Can't find message text for error: %x\n", err_code);
    SIflush(stderr);
}

/*{
** Name: dmf_put_line	- Write message to standard output.
**
** Description:
**	Write line to standard output
**
**	We also call TRdisplay to echo the message to the trace file. If
**	TRset_file was not called, this TRdisplay is a no-op, otherwise it
**	sends the message to the appropriate trace log opened by TRset_file.
**
** Inputs:
**      err_code                        Code for message to display.
**
** Outputs:
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      01-dec-1986 (Derek)
**          Created for Jupiter.
**	17-oct-1991 (rogerk)
**	    Great Merge of DMF : Move 6.4 changes into 6.5 line.
**
**	    30-apr-1991 (bryanp)
**		Call TRdisplay so that utility trace logs are more complete.
[@history_template@]...
*/
i4
dmf_put_line(
PTR		arg,
i4		length,
char		*buffer)
{
    i4	count;

	SIwrite(length, buffer, &count, stdout);
	SIwrite(1, "\n", &count, stdout);
	SIflush(stdout);

	TRdisplay("%t\n", length, buffer);
}
