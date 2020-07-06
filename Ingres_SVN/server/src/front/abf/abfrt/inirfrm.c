# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  Copyright (c) 1982, 2004 Ingres Corporation
**  All rights reserved.
*/
#include	<compat.h>
#include	<lo.h>
#include	<nm.h>
#include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<uf.h>
#include	<abfcnsts.h>
#include	<lqgdata.h>
#include	<iisqlca.h>
#include	<libq.h>
/**
** Name:    inirfrm.qc - ABF Run-Time System Form System Initialization Module.
**
** Description:
**	Contains the routine that initializes the Form System and
**	the frames used by the ABF runtime system.  Defines:
**
**	IIARfrsInit()	initialize ABF runtime FRS and frames.
**
** History:
**	Revision 2.0  82/07  joe
**	Initial revision.
**
**	11/02/90 (dkh) - Replaced IILIBQgdata with IILQgdata().
**      10/26/99 (hweho01)
**            Added IILQint() function prototype. Without the explicit 
**            declaration, the default int return value for a function 
**            will truncate a 64-bit address on ris_u64 platform.
**	28-Jan-2004 (fanra01)
**	    Add header file for IILQint function prototype and removed extern 
**	    reference.
**       2-Mar-2004 (hanal04) Bug 111888
**            IILQint() function prototype must match that found in
**            front/frontcl/libqsys/iiaplibq.h
**	20-may-2004 (abbjo03)
**	    Change header file for IILQint from iilibq.h (in front/embed/hdr)
**	    to new libq.h (in front/hdr/hdr).
*/
GLOBALREF bool	IIarUserApp;
/*{
** Name:	IIARfrsInit() -	Initialize FRS for ABF.
**
** Description:
**	Initialize the FRS for ABF or an ABF application.
**
** Input:
**	application	{bool}  Set if not ABF.
**
** Called by:
**	'IIARmain()' and 'IIABiniFrm()'.
**
** Side Effects:
**	Allocates the ABF RTS frames.
**
** History:
**	07/82 Written (jrc)
**	09/88 (jhw)  Changed to restore validate/activate states for
**		backwards compatibility for applications.
**	03/90 (jhw)  Check for II_FRS_ACTIVATE to set-up validations,
**			activations.  JupBug #20534.
**	29-oct-1993 (mgw) Bug #49551
**		Set abfrtlib Global, IIarUserApp, according to application
**		setting below for later testing in iiarFormInit() (and
**		elsewhere as needed).
*/
VOID
IIARfrsInit ( application )
bool	application;
{
	register LOCATION	*loc;
	if ( !IILQgdata()->form_on && FEforms() != OK )
	{
		abexcexit(FAIL);
		/*NOTREACHED*/
	}
	/* Set up FRS state */
	if ( application )
	{
		char	*cp;
		IIarUserApp = application;
		/* Note:  'FEforms()' turns on everything but "menu"
		** and enables the shell key.
		*/
		NMgtAt(ERx("II_FRS_ACTIVATE"), &cp);
		if ( cp != NULL && STequal(cp, ERx("60")) )
		{ /* turn on menu */
/* # line 104 "inirfrm.qc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(4,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 105 "inirfrm.qc" */	/* host code */
		}
		else
		{ /* Set up for backwards compatibility */
/* # line 108 "inirfrm.qc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(3,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      IIstfsio(6,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      IIstfsio(4,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      IIstfsio(5,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      IIstfsio(37,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      IIstfsio(40,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      IIstfsio(39,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      IIstfsio(41,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 117 "inirfrm.qc" */	/* host code */
		}
/* # line 118 "inirfrm.qc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(55,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 119 "inirfrm.qc" */	/* host code */
	}
}
/*{
** Name:	IIARiaForm() -	Initialize a Form.  
**
** Input:
**	name	-- name of the form
**
** History:
**	11/88 Written (billc)
*/
static	LOCATION	*Loc = (LOCATION *) NULL;
IIARiaForm ( name )
char *name;
{
	/* cache the location. */
	if (Loc == NULL)
		Loc = IIUFlcfLocateForm();
	if (Loc == NULL || IIUFgtfGetForm(Loc, name) != OK)
	{
		abproerr( ERx("IIARiaForm"), ALLOCFRM, name, (char *)NULL );
		/*NOTREACHED*/
	}
}
/*{
** Name:	iiar_blankform() - cache the blank form.
**
** Called from:
**	abrtvig.qsc, abrtqbf.qsc.
**
** Returns:
** 	name of the blank file.
**
** History:
**	11/88 Written (billc)
*/
static	char *_BlankName = NULL;
char *
iiar_blankform ()
{
	if (_BlankName == NULL)
	{
		_BlankName = "abfrblank";
		IIARiaForm( _BlankName );
	}
	return _BlankName;
}
