# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
/**
** Name:	FEtfscrl.qc -- scroll a table field row to 1/4 down.
**
** Description:
**	This file defines:
**
**	FEtfscrl(frm, tf, col, rownum) -- position row rownum of
**		tablefield tf (column col) of form frm to an
**		aesthetically pleasing location, 1/4 of the way
**		down the tablefield if possible.
**
** History:
**	3/10/87 (peterk) - created from qbf/mqbfhelp.qc/tabscroll()
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
  FEtfscrl(frm, tabfld, col, rownumber)
  char *frm, *tabfld, *col;
   i4	rownumber;
  {
    i4 currrec;
    i4 numrows;
    i4 numrecs;
	if (rownumber > 0)
	{
/* # line 38 "fetfscrl.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,frm,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&numrecs,32,tabfld,0);
        IIiqfsio((short *)0,1,30,4,&numrows,31,tabfld,0);
      } /* IIiqset */
    }
/* # line 42 "fetfscrl.qsc" */	/* host code */
		currrec = min(rownumber+(numrows*3/4), numrecs);
/* # line 43 "fetfscrl.qsc" */	/* scroll */
    {
      if (IItbsetio(1,frm,tabfld,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,currrec) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 44 "fetfscrl.qsc" */	/* getrow */
    {
      if (IItbsetio(2,frm,tabfld,numrows) != 0) {
        IItcogetio((short *)0,1,30,4,&currrec,(char *)"_RECORD");
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 45 "fetfscrl.qsc" */	/* host code */
		IIrowset(frm, tabfld,
			 numrows - (currrec-rownumber), col);
	}
  }
