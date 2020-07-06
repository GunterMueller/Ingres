# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<er.h>
#include	"eruf.h"
/**
** Name:	ufchkend.qc -	Front-End Utility Check Form for Change Module.
**
** Description:
**	Contains routines used to check whether changes have been made to the
**	current form and to confirm ending without saving them.  Defines:
**
**	IIUFcheckEnd()	check form for changes and confirm.
**
** History:
**	Revision 6.2  89/01  wong
**	Moved from "oo!oodetails.qsc" as 'OOcheckEnd()' and renamed.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*{
** Name:	IIUFcheckEnd() - Check Current Form for Changes and Confirm.
**
** Description:
**	Checks the current form for any changes and if there were, asks the
**	user to confirm that the changes are not to be saved.  Returns whether
**	to end or not.
**
** Returns:
**	{bool}  TRUE	end without saving changes.
**		FALSE	do not end.
**
** History:
**	5/7/87 (peterk) -- Written.
*/
bool
IIUFcheckEnd ()
  {
    i4 formchange;
/* # line 50 "ufchkend.qsc" */	/* inquire_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&formchange,20,(char *)0,0);
      } /* IIiqset */
    }
/* # line 52 "ufchkend.qsc" */	/* host code */
	return (bool)( !formchange ||
			IIUFver(ERget(S_UF0008_EndNoSavePrompt), 0)
	);
  }
