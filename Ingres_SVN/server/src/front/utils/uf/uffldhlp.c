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
/**
** Name:	uffldhlp.qc -	Help on Current Field or Column Routine.
**
** Description:
**	Contains the routine that provides help on the current field or
**	column on the form.  Defines:
**
**	IIUFfieldHlep()	provide help on the current field or column.
**
** History:
**	Revision 6.4  89/11  wong
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*{
** Name:	IIUFfieldHelp() -	Provide Help on the Current Field
**						or Column.
** Description:
**	Calls the (FRS) RunTime help facility for fields, which either provides
**	a selection list or displays information on the field or column.  The
**	current form, field and column are determined and passed to the help
**	facility.
**
** History:
**	11/89 (jhw) -- Written.
*/
VOID
IIUFfieldHelp()
  {
    i4 istable;
    char formname[FE_MAXNAME+1];
    char fldname[FE_MAXNAME+1];
    char colname[FE_MAXNAME+1];
/* # line 51 "uffldhlp.qc" */	/* inquire_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,formname,19,(char *)0,0);
        IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,fldname,22,(char *)0,0);
      } /* IIiqset */
    }
/* # line 53 "uffldhlp.qc" */	/* host code */
	if ( fldname[0] == EOS || formname[0] == EOS )
		return;
/* # line 56 "uffldhlp.qc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&istable,28,fldname,0);
      } /* IIiqset */
    }
/* # line 57 "uffldhlp.qc" */	/* host code */
	if ( istable != 1 )
	{
		colname[0] = EOS;
	}
	else
	{ /* table field */
/* # line 63 "uffldhlp.qc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,formname,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,colname,34,fldname,0);
      } /* IIiqset */
    }
/* # line 64 "uffldhlp.qc" */	/* host code */
	}
	FEhvals(formname, fldname, colname);
  }
