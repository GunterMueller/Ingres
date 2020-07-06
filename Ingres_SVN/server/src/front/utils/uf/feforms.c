# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<uigdata.h>
/**
** Name:	feforms.qc -	Front-End Forms System Start-up Module.
**
** Description:
**	This module starts up and ends the Forms System.  Contains:
**
**	FEforms()	forms system start-up utility.
**	FEendforms()	forms system end utility.
**
** History:
**	Revision 6.4  89/08  wong
**	Enable the Shell key.
**
**	Revision 6.2  88/08  wong
**	Turn on all activations and validations.
**
**	Revision 6.0  87/04  peter
**	Take out msg routine setting, as ##forms does it now.
**
**	Revision 4.2  86/04/18  09:13:38  sandyd
**	Turned on field validation on "previousfield".
**
**	Revision 4.1  86/02/13  11:54:10  wong
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*
** Name:	forms_cleanup() -	Clean-Up Forms System on Exit.
*/
static i4
forms_cleanup ()
{
/* # line 52 "feforms.qsc" */	/* endforms */
  {
    IIendforms();
  }
/* # line 53 "feforms.qsc" */	/* host code */
}
/*{
** Name:	FEforms() -	Front-End Forms System Start-up Utility.
**
** Description:
**	This routine starts up the Forms System and sets the FE forms message
**	routine pointer to 'FTmessage()'.  It also sets the exit clean-up
**	routine for the Forms System.
**
** Returns:
**	{STATUS}  OK if forms startup without errors.
**		  FAIL if any errors occurred.
**
** History:
**	02/86 (jhw) -- Written.
**	18-apr-1986 (sandyd)
**		Turned on field validation for "previousfield" exits.
**		The FRS default of allowing ^P to bypass validation
**		was considered dangerous, and not a desirable default
**		for our front ends.
**	22-dec-1986 (peter)	Added return status code from
**		FEforms to return FAIL if any forms errors
**		occur.
**	04-apr-1987 (peter)
**		Moved setting of message routines to ##forms.
**	07/27/87 (dkh) - Fixed jup bug 625.
**	08/87 (jhw) -- Turn on all validations and activations.
**	08/89 (jhw) -- Enable the Shell key for all Front-Ends.
**	03/90 (jhw) -- Don't turn on all validations and activations
**		if they're already on.
**	28-aug-1990 (Joe)
**	    Changed IIUIgdata to a function.
**	28-aug-1990 (Joe)
**	    Changed name of IIUIgdata to IIUIfedata.
*/
STATUS
FEforms()
{
  i4 errnum = 0;
	VOID	IIUFprompt();
	VOID	FEset_exit();
/* # line 98 "feforms.qsc" */	/* forms */
  {
    IIforms(0);
  }
/* # line 100 "feforms.qsc" */	/* host code */
	/*
	**  Using IItest_err since forms system may not
	**  have come up properly.  Bad terminal name, etc.
	*/
/* # line 104 "feforms.qsc" */	/* inquire_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,30,4,&errnum,1,(char *)0,0);
    } /* IIiqset */
  }
/* # line 105 "feforms.qsc" */	/* host code */
	if ( errnum != OK )
	{
		return errnum;
	}
	FEset_exit(forms_cleanup);
	/* Set all activations and validations.  Note:  Do not set "menu"
	** since it cannot be overriden; "menuitem" covers all menuitems
	** and can be overriden on an individual basis.
	**
	** Note: "next" is always on already.
	*/
/* # line 118 "feforms.qsc" */	/* inquire_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,30,4,&errnum,4,(char *)0,0);
    } /* IIiqset */
  }
/* # line 119 "feforms.qsc" */	/* host code */
	if ( errnum == 1 )
	{ /* all already on (II_FRS_ACTIVATE = 60) */
/* # line 121 "feforms.qsc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(4,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      IIstfsio(55,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 125 "feforms.qsc" */	/* host code */
	}
	else
	{ /* none were on */
/* # line 128 "feforms.qsc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(3,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      IIstfsio(6,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      IIstfsio(5,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      IIstfsio(37,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      IIstfsio(40,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      IIstfsio(41,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      IIstfsio(55,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 137 "feforms.qsc" */	/* host code */
	}
	IIUIfedata()->prompt_func = IIUFprompt;
	return FEinqferr();
}
/*{
** Name:	FEendforms() -	Front-End Forms System End Utility.
**
** Description:
**	This routine ends the Forms System and clears the FE forms message
**	routine pointers.
**
** Side Effects:
**	Calls iiugfrs_setting to turn off messages, and
**	and clears the exit clean-up routine.
**
** History:
**	02/86 (jhw) -- Written.
*/
VOID
FEendforms()
{
	VOID	FEclr_exit();
/* # line 164 "feforms.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 166 "feforms.qsc" */	/* endforms */
  {
    IIendforms();
  }
/* # line 168 "feforms.qsc" */	/* host code */
	FEclr_exit(forms_cleanup);
}
