# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	Alls rights reserved.
*/
#include	<compat.h>
#include	<st.h> 
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
/**
** Name:	FEtbsins.qc	- routine for insert row in sorted table
**
** Description:
**	This file defines:
**
**	FEtbsins()	find insert row for a string in a sorted table
**
** History:
**	Revision 4.0  85/01  bobm
**	Initial revision.
**	03/28/91 (dkh) - Replaced old inquire_frs syntax with new.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
/*{
** Name:	FEtbsins - find row for inserting string into sorted table.
**
** Description:
**
**	Searches through a table assumed to be sorted, and finds 
**	row for insert.
**
** Inputs:
**	frmnam		- name of form.
**	tabnam		- name of table field.
**	name		- string to be inserted
**	col		- sort column
**	uniq		- if TRUE, delete row with identical key if found.
**			  if FALSE, returned row inserts ahead of duplicate.
**
** Outputs:
**
**	Returns:  row number to insert
**
** Side Effects:
**	Positions cursor in table field for insert.
**
** History:
**	1/85 (rlm)	written.
*/
i4
FEtbsins(frmnam, tabnam, name, col, uniq)
  char *frmnam;
  char *tabnam;
  char *name;
  char *col;
bool uniq;
  {
    i4 insrow;
    i4 trow;
    i4 drow;
    char buff[80];
    i4 state;
	i4 comp;
	comp = -1;
	insrow = 0;
/* # line 75 "fetbsins.qsc" */	/* unloadtable */
    {
      if (IItbact(frmnam,tabnam,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,79,buff,col);
        IItcogetio((short *)0,1,30,4,&trow,(char *)"_RECORD");
        IItcogetio((short *)0,1,30,4,&state,(char *)"_STATE");
        IITBceColEnd();
        {
/* # line 78 "fetbsins.qsc" */	/* host code */
		if (state != 0 && state != 4 && trow > 0)
		{
			insrow = trow;
			STtrmwhite(buff);
			if ((comp = STcompare(buff,name)) >= 0)
			{
				--insrow;
/* # line 85 "fetbsins.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 86 "fetbsins.qsc" */	/* host code */
			}
		}
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 90 "fetbsins.qsc" */	/* host code */
	if (uniq && comp == 0)
	{
/* # line 92 "fetbsins.qsc" */	/* scroll */
    {
      if (IItbsetio(1,frmnam,tabnam,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,trow) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 93 "fetbsins.qsc" */	/* deleterow */
    {
      if (IItbsetio(4,frmnam,tabnam,-2) != 0) {
        if (IItdelrow(0) != 0) {
        } /* IItdelrow */
      } /* IItbsetio */
    }
/* # line 94 "fetbsins.qsc" */	/* host code */
	}
	if (insrow == 0)
	{
/* # line 98 "fetbsins.qsc" */	/* scroll */
    {
      if (IItbsetio(1,frmnam,tabnam,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,1) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 99 "fetbsins.qsc" */	/* host code */
		return (0);
	}
/* # line 102 "fetbsins.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,frmnam,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&trow,32,tabnam,0);
      } /* IIiqset */
    }
/* # line 103 "fetbsins.qsc" */	/* host code */
	if (trow > (insrow+1))
	{
		insrow += 2;
/* # line 106 "fetbsins.qsc" */	/* scroll */
    {
      if (IItbsetio(1,frmnam,tabnam,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,insrow) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 107 "fetbsins.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,frmnam,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&drow,29,tabnam,0);
      } /* IIiqset */
    }
/* # line 108 "fetbsins.qsc" */	/* host code */
		drow -= 2;
		if (drow > 0)
			return (drow);
	}
/* # line 112 "fetbsins.qsc" */	/* scroll */
    {
      if (IItbsetio(1,frmnam,tabnam,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,insrow) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 113 "fetbsins.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,frmnam,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&drow,29,tabnam,0);
      } /* IIiqset */
    }
/* # line 114 "fetbsins.qsc" */	/* host code */
	return (drow);
  }
