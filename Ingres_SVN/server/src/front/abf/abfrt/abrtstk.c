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
#include	<ooclass.h>
#include	<lo.h>
#include	<fdesc.h>
#include	<abfrts.h>
#include	<rtsdata.h>
#include	<erar.h>
/**
** Name:	abrtsstk.qc -	Display ABF RunTime Object Stack.
**
** Description:
**	Contains the routine that displays the ABF runtime object stack in
**	a table field.  Defines:
**
**	iiarStkLoad()	load rts object stack into table field.
**
**	NOTE:  This routine is internal to the ABF RunTime System.
**
** History:
**	Revision 5.1  86/08/14  joe
**	Extracted from "abfrts" for PC overlay work.
**	08/19/88 (dkh) - Changed static READONLY declarations to
**			 just static to regain shareability of
**			 shared libs on VMS.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
/*{
** Name:	iiarStkLoad() -	Load RTS Object Stack into Table Field.
**
** Description:
**	Loads the ABF runtime system stack into a table field to be displayed.
**
** Inputs:
**	stack	{ABRTSSTK *}  The ABF RTS stack.
**	form	{char *}  The name of the form containing the table field,
**				"frmstk," that will be loaded.
**	hidden	{bool}  If true, place the actual stack element in
**				the hidden field 'stkele'.
**
** Side Effects:
**	DEPENDS ON THE FACT THAT A POINTER CAN FIT IN AN i4.
**
** History:
**	??? (joe)
**	27-sep-1985 (joe)
**		Added the hidden option.
*/
VOID
iiarStkLoad ( stack, form, hidden )
ABRTSSTK	*stack;
  char *form;
bool		hidden;
  {
	static
    char tblfld[]= ERx("frmstk");
	register ABRTSSTK	*stk;
	if ( hidden )
/* # line 76 "abrtstk.qsc" */	/* inittable */
    {
      if (IItbinit(form,tblfld,(char *)"r") != 0) {
        IIthidecol((char *)"stkele",(char *)"i4");
        IItfill();
      } /* IItbinit */
    }
/* # line 77 "abrtstk.qsc" */	/* host code */
	else
/* # line 78 "abrtstk.qsc" */	/* inittable */
    {
      if (IItbinit(form,tblfld,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 80 "abrtstk.qsc" */	/* host code */
	for ( stk = stack ; stk != NULL ; stk = stk->abrfsnext )
	{
    char *name;
		i4	usage;	/* message ID */
		name = stk->abrfsname;
		switch (stk->abrfsusage)
		{
		  case OC_OSLFRAME:
		  case OC_OLDOSLFRAME:
			usage = S_AR004A_USER;
			break;
		  case OC_QBFFRAME:
			usage = S_AR0040_QBF;
			break;
		  case OC_RWFRAME:
			usage = S_AR0041_REPORT;
			break;
		  case OC_GRFRAME:
			usage = S_AR004B_GRAPHICS;
			break;
		  case OC_GBFFRAME:
			usage = S_AR0042_GBF;
			break;
		  case OC_OSLPROC:
		  case OC_HLPROC:
			usage = S_AR004C_PROC;
			break;
		  case OC_OSLLOCALPROC:
			usage = S_AR0049_LOCALPROC;
			break;
		  default:
			usage = S_AR004D_UNDEF;
			break;
		}
		/* Load into Table Field */
		if ( hidden)
    {
      i4 ele = (i4) stk;
/* # line 120 "abrtstk.qsc" */	/* loadtable */
      {
        if (IItbact(form,tblfld,1) != 0) {
          IItcoputio((char *)"frame",(short *)0,1,32,0,name);
          IItcoputio((char *)"usage",(short *)0,1,32,0,ERget(usage));
          IItcoputio((char *)"stkele",(short *)0,1,30,4,&ele);
          IITBceColEnd();
        } /* IItbact */
      }
    }
/* # line 125 "abrtstk.qsc" */	/* host code */
		else
		{
/* # line 127 "abrtstk.qsc" */	/* loadtable */
    {
      if (IItbact(form,tblfld,1) != 0) {
        IItcoputio((char *)"frame",(short *)0,1,32,0,name);
        IItcoputio((char *)"usage",(short *)0,1,32,0,ERget(usage));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 130 "abrtstk.qsc" */	/* host code */
		}
	} /* end for */
  }
