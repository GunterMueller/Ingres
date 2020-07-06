/*
**	Copyright (c) 2004 Ingres Corporation
*/

# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ft.h>
# include	<fmt.h>
# include	<adf.h>
# include	<frame.h>
# include	<runtime.h>
# include	<lqgdata.h>
# include	<frserrno.h>
# include	<rtvars.h>
# include       <er.h>


/**
** Name:	iitoact.c -	Routines to handle timeout feature
**
** Description:
**	Routines defined are:
**		IIFRtoact		Define a timeout activation value
**		IIFRgtoGetTOact		Retrieve a timeout activation value
**		IIFRitIsTimeout		Indicate if timeout has occurred
**
** History:
**	10-nov-88 (bruceb)	Written
**	11/02/90 (dkh) - Replaced IILIBQgdata with IILQgdata().
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/

/*
** Stores the timeout return value (0, or positive value if activate timeout
** was defined) for the current ##submenu.
*/
GLOBALREF	i4	IIFRtovTmOutVal;


/*{
** Name:	IIFRtoact	- Define a timeout activation value
**
** Description:
**	Routine called as a result of an EQUEL "## ACTIVATE TIMEOUT"
**	statement.  Define a timeout activation value.  If timeout
**	activation value is already set (signifying that a prior
**	activate timeout block was specified for this context), generate
**	an error message an ignore this call.
**
**	This routine is part of RUNTIME's external interface.
**
** Inputs:
**	timeout		Number of seconds until timeout.
**			(Currently ignored.)
**	intr_val	Activation code
**
** Outputs:
**
**	Returns:
**		TRUE	This activation was successful (or ignored if
**			more than one per context.)
**
**	Exceptions:
**		None
**
** Side Effects:
**
** History:
**	10-nov-88 (bab)	Written.
*/

i4
IIFRtoact(timeout, intr_val)
i4	timeout;
i4	intr_val;
{
    i4	*tovptr;

    if (IIissubmu())	/* ## submenu */
    {
	tovptr = &IIFRtovTmOutVal;
    }
    else	/* ## display or ## display submenu */
    {
	tovptr = &(IIstkfrm->tmout_val);
    }

    if (*tovptr == 0)
    {
	/* Set the activate timeout value. */
	*tovptr = intr_val;
    }
    else
    {
	/* Already an activate timeout block for this context. */
	IIFDerror(RTDBTACT, 0, NULL);
    }

    return(TRUE);
}

/*{
** Name:	IIFRgtoGetTOact	- Retrieve a timeout activation value
**
** Description:
**	Called to obtain the activation value for a given display loop
**	or submenu.  This will be either that value generated by an
**	equel ##activate timeout (see IIFRtoact) or 0 if no activate
**	timeout block was defined.
**
** Inputs:
**
** Outputs:
**
**	Returns:
**		activation value for this display loop or submenu.
**
**	Exceptions:
**		None
**
** Side Effects:
**
** History:
**	11-nov-88 (bab)	Written.
*/

i4
IIFRgtoGetTOact()
{
    i4	intr_val;

    /*
    ** tmout_val will contain either the appropriate
    ** retval generated by equel/esql or 0 if no activate
    ** timeout was specified.
    */
    if (IIissubmu())
    {
	/* activate occurred in a submenu */
	intr_val = IIFRtovTmOutVal;
    }
    else
    {
	/* activate occurred in a display loop or display submenu */
	intr_val = IIstkfrm->tmout_val;
    }

    return(intr_val);
}


/*{
** Name:	IIFRitIsTimeout	- Indicate if timeout has occurred
**
** Description:
**	Indicate if timeout has occurred.  If the FRS is not on, than
**	timeout can not have occurred (and realistically, this routine
**	won't have been called.)  If the FRS is on than return the
**	answer as indicated by the global event control block's event
**	member.
**
**	Equel generates a call on this routine if the display loop
**	contains no activation blocks.  If that is true, than this routine
**	is called before IIchkfrm, with IIchkfrm being called only if
**	timeout has not occurred.  Code will look something like this:
**
**	    while (IIrunform() != 0) {
**	    }
**	    if (IIFRitIsTimeout() != 0) goto IIfdE1;
**	IIfdF1:
**	    if (IIchkfrm() == 0) goto IIfdB1;
**
** Inputs:
**
** Outputs:
**
**	Returns:
**		TRUE	Timeout has occurred.
**		FALSE	Timeout has not occurred.
**
**	Exceptions:
**		None
**
** Side Effects:
**	None
**
** History:
**	30-nov-88 (bab)	Written.
*/
i4
IIFRitIsTimeout()
{
    if (IILQgdata()->form_on && IIfrscb->frs_event->event == fdopTMOUT)
	return(TRUE);
    else
	return(FALSE);
}
