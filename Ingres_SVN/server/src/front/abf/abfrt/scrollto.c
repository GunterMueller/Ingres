# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<st.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
/**
** Name:	scrollto.qc	-	ABF Run-Time System Scroll Table Field Routine.
**
** Description:
**	Contains a routine used to scroll a table field to a particular row.
**	This routine can be called by users from OSL.  Defines:
**
**	IIscrollto()	scroll to row of table field.
**
** History:
**	Revision 6.1  87/11/20  wong
**	Initial revision.
*/
/*{
** Name:	IIscrollto() -	Scroll to Row of Table Field.
**
** Description:
**	Scrolls the input table field of the input form (or the current of
**	either if NULL or empty) to the input row.  Rows are positive except
**	for the last row, which can be specified by -1.
**
** Input:
**	form	{char *}  Name of form (possibly NULL or empty.)
**	table	{char *}  Name of table field (possibly NULL or empty.)
**	row	{nat}  Row number to which to scroll the table field.
**
** Side Effects:
**	Will scroll the current or input table field.
**
** History:
**	11/87 (jhw) -- Written.
*/
VOID
  IIscrollto(form, table, row)
  char *form;
  char *table;
  int row;
  {
    char formname[FE_MAXNAME+1];
    char tablename[FE_MAXNAME+1];
	/* Get current form if not specified */
	if (form == NULL || *form == EOS || STtrmwhite(form) <= 0)
	{
/* # line 58 "scrollto.qsc" */	/* inquire_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,formname,19,(char *)0,0);
      } /* IIiqset */
    }
/* # line 59 "scrollto.qsc" */	/* host code */
		form = formname;
		if (*form == EOS)
			return;	/* no form */
	}
	/* Get current table field if not specified */
	if (table == NULL || *table == EOS || STtrmwhite(table) <= 0)
    {
      int is_table;
/* # line 69 "scrollto.qsc" */	/* inquire_frs */
      {
        if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,tablename,22,(char *)0,0);
        } /* IIiqset */
      }
/* # line 70 "scrollto.qsc" */	/* host code */
		table = tablename;
		if (*table == EOS)
			return;	/* no table field */
/* # line 73 "scrollto.qsc" */	/* inquire_frs */
      {
        if (IIiqset(2,0,form,(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&is_table,28,tablename,0);
        } /* IIiqset */
      }
/* # line 74 "scrollto.qsc" */	/* host code */
		if (is_table != 1)
			return;	/* no table field */
    }
/* # line 78 "scrollto.qsc" */	/* scroll */
    {
      if (IItbsetio(1,form,table,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,row) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
  }
