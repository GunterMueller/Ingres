# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<abclass.h>
#include	"erab.h"
/**
** Name:	aberrlst.c -	ABF Source Compilation Error Listing.
**
** Description:
**	Contains the ABF source compilation error listing routine.  Defines:
**
**	iiabErrList()	display the error listing for a compilation.
**
** History:
**	Revision 6.2  88/02  wong
**	Initial revision.
*/
/*{
** Name:	iiabErrList() -	Display the Error Listing for a Compilation.
**
** Description:
**	Calls the VQ error display facility for the component in error.
**
** Inputs:
**	obj	{OO_OBJECT *}  The object for which errors are to be displayed.
**
** History:
**	02/89 (jhw)  Written.
**	11/89 (jhw)  Added missing TRUE parameter to 'FEhframe()' call.
**			JupBug #8535.
**	11/89 (jhw)  Re-wrote to get component as argument and to call
**		'IIVQdceDisplayCompErrs()'.
**	2/90 (Mike S) New calling sequence for IIVQdceDisplayCompErrs
*/
VOID
iiabErrorList ( obj )
OO_OBJECT	*obj;
{
	APPL		*appl;
	OO_OBJECT	*frame;
	if ( obj->class == OC_APPL )
	{
		appl = (APPL *)obj;
		frame = (OO_OBJECT *)NULL;
	}
	else
	{
		appl = ((APPL_COMP *)obj)->appl;
		frame = obj;
	}
	_VOID_ IIVQdceDisplayCompErrs(appl, frame, FALSE);
}
