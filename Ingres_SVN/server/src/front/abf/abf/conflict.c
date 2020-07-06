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
# include <ooclass.h>
# include <oocat.h>
/**
** Name:	conflict.qc -	Report Conflicts with System Function Names.
**
** Description:
**	Display procedure names that conflict with system function names.
**	Defines:
**
**	iiabReportConflicts()	report conflicts with system function names.
**
** History:
**	Revision 6.2  89/02  wong
**	Renamed from 'absysconflict()' and modified to read names from a
**	table field and to check them directly.
**
**	Revision 6.0  87/04  bobm
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
/*{
** Name:	iiabReportConflicts() -	Report Conflicts with System Names.
**
** Description:
**	Display procedure names that conflict with system function names.  The
**	names are read from the input table field, checked for conflict with
**	the system names, and then displayed on the "abfconflict" form in a
**	table field.
**
** Input:
**	form	{char *}  The form name containing the names table field.
**	tblfld	{char *}  The table field containing the names to be checked.
**
** History:
**	1987 (bobm)  Written.
**	02/89 (jhw)  Rewritten to read names from the table field.
*/
static READONLY
  char _Form[]= ERx("abfconflict"), _NamesFld[]= ERx("names");
  static i4 _maxRows = 0;
#define _EndExpl	F_FE0102_ExplEnd
VOID
iiabReportConflicts ( form, tblfld )
  char *form;
  char *tblfld;
{
	if ( _maxRows == 0 )
	{
		IIARiaForm(_Form);
/* # line 72 "conflict.qsc" */	/* inittable */
  {
    if (IItbinit(_Form,_NamesFld,(char *)"f") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 73 "conflict.qsc" */	/* inquire_frs */
  {
    if (IIiqset(3,0,_Form,(char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,30,4,&_maxRows,30,_NamesFld,0);
    } /* IIiqset */
  }
/* # line 74 "conflict.qsc" */	/* host code */
	}
/* # line 76 "conflict.qsc" */	/* display */
  {
    if (IIdispfrm(_Form,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 79 "conflict.qsc" */	/* host code */
		register i4	rows = 0;
      char name[FE_MAXNAME+1];
/* # line 82 "conflict.qsc" */	/* clear */
      {
        IIfldclear(_NamesFld);
      }
/* # line 83 "conflict.qsc" */	/* redisplay */
      {
        IIredisp();
      }
/* # line 84 "conflict.qsc" */	/* unloadtable */
      {
        if (IItbact(form,tblfld,0) == 0) goto IItbE1;
        while (IItunload() != 0) {
          IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,name,_name);
          IITBceColEnd();
          {
/* # line 86 "conflict.qsc" */	/* host code */
			if ( IIAFckFuncName(name) )
			{
/* # line 88 "conflict.qsc" */	/* loadtable */
            {
              if (IItbact(_Form,_NamesFld,1) != 0) {
                IItcoputio((char *)"procname",(short *)0,1,32,0,name);
                IITBceColEnd();
              } /* IItbact */
            }
/* # line 92 "conflict.qsc" */	/* host code */
				if ( ++rows == _maxRows )
/* # line 93 "conflict.qsc" */	/* redisplay */
            {
              IIredisp();
            }
/* # line 94 "conflict.qsc" */	/* host code */
			}
          }
        } /* IItunload */
IItbE1:
        IItunend();
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 100 "conflict.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else {
        if (1) goto IIfdE1;
      } /* IIretval */
    } /* IIrunform */
IIfdF1:
    if (IIchkfrm() == 0) goto IIfdB1;
    goto IIfdE1;
IImuI1:
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_End),ERget(_EndExpl),2,2,1) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 102 "conflict.qsc" */	/* host code */
}
