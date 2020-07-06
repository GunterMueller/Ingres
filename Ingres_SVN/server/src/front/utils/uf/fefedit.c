# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 1987, 2005 Ingres Corporation
**
*/
#include	<compat.h>
#include	<st.h>
#include	<ut.h>
#include	<si.h>
#include	<lo.h>
#include	<nm.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ft.h>
#include	"eruf.h"
/**
** Name:    fefedit.qc -	Front-End Utility Field/Column Edit Routine.
**
** Description:
**	Contains the routine used that calls up the system editor on the
**	contents of a field or column.  Defines:
**
**	FEfedit()	edit contents of field/column.
**
** History:
**	Revision 6.0  87/03  peterk
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	07-sep-2005 (abbjo03)
**	    Replace xxx_FILE constants by SI_xxx.
**/
# define	BFRSIZE 256
/*{
** Name:    FEfedit() -	Edit Contents of Field/Column.
**
** Description:
**    The contents of the current field or table-field column will
**    be copied to a temporary file, after which an editor will be
**    invoked to allow the user to modify it.
**
**    The changed dataset will be used to reload the input form; any
**    excess records will be ignored.
**
** Returns:
**	{STATUS}  OK if no errors;
**		  FAIL otherwise.
**
** History:
**	3/18/87 (peterk) - code taken from fstm/fsedit.qc to create
**		generalized fefedit.qc.
**	3/20/87 (peterk) - generalized to work on current cursor-
**		located field, simple or table.
*/
STATUS
FEfedit()
{
  char formname[FE_MAXNAME+1];
  char fld[FE_MAXNAME+1];
  char col[FE_MAXNAME+1];
    STATUS	status;
    FILE	*fp;
    LOCATION	loc;
    char	filename[MAX_LOC];
  char bfr[BFRSIZE+2];
  i4 istable;
/* # line 77 "fefedit.qsc" */	/* inquire_frs */
  {
    if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,formname,19,(char *)0,0);
    } /* IIiqset */
  }
/* # line 78 "fefedit.qsc" */	/* inquire_frs */
  {
    if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,fld,19,(char *)0,0);
      IIiqfsio((short *)0,1,30,4,&istable,28,(char *)0,0);
    } /* IIiqset */
  }
/* # line 79 "fefedit.qsc" */	/* host code */
    if (istable == 1)
/* # line 80 "fefedit.qsc" */	/* inquire_frs */
  {
    if (IIiqset(3,0,formname,(char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,col,34,(char *)0,0);
    } /* IIiqset */
  }
/* # line 82 "fefedit.qsc" */	/* host code */
    FEmsg(_EnterEditor, FALSE, fld);
    /*
    **    Open a temp file for output
    */
    if ((status = NMt_open(ERx("w"), ERx("fs"), ERx("edt"), &loc, &fp)) != OK)
    {
	IIUGerr(E_UF0003_Bad_Create, 0, 1, &status);
	return FAIL;
    }
    LOcopy(&loc, filename, &loc);
    /*
    **    Copy each line from the input form into the temporary file
    */
    if (!istable)
    {
/* # line 99 "fefedit.qsc" */	/* getform */
  {
    if (IIfsetio(formname) != 0) {
      IIgetfldio((short *)0,1,32,BFRSIZE+2-1,bfr,fld);
    } /* IIfsetio */
  }
/* # line 100 "fefedit.qsc" */	/* host code */
	if (bfr[0] != EOS)
	    SIputrec(bfr, fp);
	else
	    SIputc(' ', fp);
	SIputc('\n', fp);
    }
    else
    {
/* # line 108 "fefedit.qsc" */	/* unloadtable */
  {
    if (IItbact(formname,fld,0) == 0) goto IItbE1;
    while (IItunload() != 0) {
      IItcogetio((short *)0,1,32,BFRSIZE+2-1,bfr,col);
      IITBceColEnd();
      {
/* # line 110 "fefedit.qsc" */	/* host code */
	    if (bfr[0] != EOS)
		SIputrec(bfr, fp);
	    else
		SIputc(' ', fp);
	    SIputc('\n', fp);
      }
    } /* IItunload */
IItbE1:
    IItunend();
  }
/* # line 116 "fefedit.qsc" */	/* host code */
    }
    /*
    **    Close the temp file.
    */
    if ( (status = SIclose(fp) ) != OK)
    {
	IIUGerr(E_UF0004_Bad_Close, 0, 2, filename, &status);
	return FAIL;
    }
    /*
    **    Invoke the default editor on the temp file
    */
    FTclear();
    FTrestore(FT_NORMAL);
    UTedit((char *)NULL, &loc);
    FTrestore(FT_FORMS);
    FTclear();
    /*
    **    Make sure the file is still there
    */
    if (LOexist(&loc) != OK)
    {
	FEmsg(_EmptyFile, TRUE, filename);
	return OK;
    }
    /*
    **    Open the temp file for input
    */
    if ((status = SIfopen(&loc, ERx("r"), SI_TXT, 512, &fp)) != OK)
    {
	IIUGerr(E_UF0005_Bad_Open, 0, 2, filename, &status);
	return FAIL;
    }
    /*
    **    Read in the file and load value or column rows back onto the form.
    */
/* # line 156 "fefedit.qsc" */	/* clear */
  {
    IIfldclear(fld);
  }
/* # line 158 "fefedit.qsc" */	/* host code */
    bfr[BFRSIZE+1] = EOS;
    while (SIgetrec(bfr, BFRSIZE, fp) == OK)
    {
	STtrmwhite(bfr);
	if (istable)
/* # line 163 "fefedit.qsc" */	/* loadtable */
  {
    if (IItbact(formname,(char *)"tblfld",1) != 0) {
      IItcoputio(col,(short *)0,1,32,0,bfr);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 164 "fefedit.qsc" */	/* host code */
	else
	{
/* # line 166 "fefedit.qsc" */	/* putform */
  {
    if (IIfsetio(formname) != 0) {
      IIputfldio(fld,(short *)0,1,32,0,bfr);
    } /* IIfsetio */
  }
/* # line 167 "fefedit.qsc" */	/* host code */
	    break;
	}
    }
    /*
    **    Close and delete the temp file
    */
    if ((status = SIclose(fp)) != OK || (status = LOdelete(&loc)) != OK)
    {
	IIUGerr(E_UF0006_Bad_Delete, 0, 2, filename, &status);
	/* Field/Column value was already changed; this is a warning only. */
    }
    return OK;
}
