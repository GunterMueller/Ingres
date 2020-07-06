# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 1989, 2005 Ingres Corporation
**
*/
# include	<compat.h>
# include	<nm.h>		/* 6-x_PC_80x86 */
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<ut.h>		/* 6-x_PC_80x86 */
# include       <cm.h>
# include       <er.h>
# include       <me.h>
# include       <si.h>
# include       <lo.h>
# include       <gl.h>
# include       <sl.h>
# include       <iicommon.h>
# include	<fe.h>
# include       <fmt.h>
# include       <adf.h>
# include       <afe.h>
# include       <ft.h>
# include       "eruf.h"
/**
** Name:	tfedit.qsc - Edit in a tablefield
**
** Description:
**	Routines used to edit text in a tablefield.  
**	These routines can be called from a simple display loop to provide
**	the same interface used by fstm.
**	This file defines:
**
**	IIUFtfeTblFldEdit	Call the system editor
**	IIUFtflTblFldLoad	Load a file into the tablefield(interactive)
**	IIUFtfnTblFldNiLoad	Load a file into the tablefield(non-interactive)
**	IIUFtfuTblFldUnload	Unload the tablefield into a file
**	IIUFtfiTblFldInsert	Insert a blank line into the tablefield
**	IIUFtfdTblFldDelete	Delete a line from the tablefield
**	IIUFtfsTblFldString	Load a string into the tablefield
**	IIUFtfrTblFldRegister	Register a diagnostic function
**	IIUFtfbTblFldBreak	Break a tablefield row
**	IIUFtfjTblFldJoin	Join two tablefield rows
**	IIUFsifSetIntFmt	Use special internal format
**	IIUFsnfSetNormFmt	Use normal format
**
** History:
**	8/7/89 (Mike S)	Abstracted from fstm/fsedit.qsc and fstm/fsload.qsc
**	8/28/89 (Mike S) Added split and join routines
**	10/12/89 (Mike S) Make diagnostic function global
**    10-sep-93 (dkhor)
**            On axp_osf, if (NULL == _format) won't compile.  Use 
**            if (_format == NULL)
**      25-sep-96 (mcgem01)
**              Global data moved to ufdata.c
**	4-dec-96(angusm) Change operation of load_tf (bug 79222/76745)
**	15-jan-1996 (angusm) Redo fix for 79222 (bug 80037)
**      03-may-1999 (hanch04)
**          SIputrec returns EOF on error.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	07-sep-2005 (abbjo03)
**	    Replace xxx_FILE constants by SI_xxx.
**/
/* # define's */
# define BFRSIZE	254
# define DELETED	4
/* Something to load into a tablefield: a file or a string */
typedef struct
{
	i4 tag;	/* Kind of input */
# define TFIN_FILE 	0
# define TFIN_STRING	1
	union
	{
		FILE *fp;	/* File input */
		char *string;	/* String input */
	} var;
}  TFINPUT;
/* GLOBALREF's */
GLOBALREF VOID 	(*iiuftfdDiagFunc)();	/* Diagnostic tracing function */
/* extern's */
FUNC_EXTERN	ADF_CB	*FEadfcb();
FUNC_EXTERN	PTR	FEreqmem();
FUNC_EXTERN	VOID	IIUFsnfSetNormFmt();
FUNC_EXTERN	VOID	IIUGmsg();
FUNC_EXTERN	char	*STskipblank();
/* static's */
static ADF_CB 	*cb = NULL;		/* ADF control block for load_tf */
static char *_format = NULL;		/* Wrap format for loading tablefield */
static READONLY char _nl[] = ERx("\n");
static READONLY 
  char _blank[]= ERx(" ");
static	STATUS load_tf();
static	STATUS str_getrec();
/*{
** Name:	IIUFtfeTblFldEdit  -  Call system editor
**
** Description:
**    The current contents of the tablefield will be copied
**    to a temporary file, after which a editor will be invoked
**    to allow the user to modify it.
**
**    The changed dataset will be used to reload the input form; any
**    excess records will be ignored.
**
**
** Inputs:
**	formname	char *	Name of form being edited
**	tblfldname	char *	name of tablefield being edited
**	fldname		char *  Column in tablefield being edited
**
** Outputs:
**	msgbuf		char **	Message buffer explaining error.  Only contains
**				a valid message if the return is FAIL.
**
**	Returns:
**		STATUS	OK	if all is well
**			FAIL	if there are problems opening or closing the
**				temp file.
**
**	Exceptions:
**		none
**
** Side Effects:
**	none
**
** History:
**	8/7/89 (Mike S)	abstracted from fstm/fsedit.qsc
**	1-jun-90 (teresal)
**		Changed LOdelete to LOpurge so all versions of the temp edit
**		file will be deleted.  Problem was evident on VMS but not on 
**		Unix.
*/
STATUS
  IIUFtfeTblFldEdit(formname, tblfldname, fldname, msgbuf)
  char *formname;
  char *tblfldname;
  char *fldname;
char	**msgbuf;
  {
        i4         rc;
	char            msg[80];
        LOCATION        loc;
        FILE            *fp;
        char            filename[MAX_LOC];
    char bfr[BFRSIZE+2];
    i4 r_state;
	TFINPUT		tfinput;
	*msgbuf = msg;
        /*
        **    Open a temp file for output
        */
        if ((rc = NMt_open(ERx("w"), ERx("tf"), ERx("edt"), &loc, &fp)) != OK)
        {
                _VOID_ IIUGfmt(msg, 80, ERget(E_UF0009_Unable_open_temp_edit), 
				1, (PTR)&rc);
                return (FAIL);
	}
        LOcopy(&loc, filename, &loc);
        /*
        **    Copy each line from the input form into the temporary file
        */
/* # line 181 "tfedit.qsc" */	/* unloadtable */
    {
      if (IItbact(formname,tblfldname,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,BFRSIZE+2-1,bfr,fldname);
        IItcogetio((short *)0,1,30,4,&r_state,(char *)"_STATE");
        IITBceColEnd();
        {
/* # line 183 "tfedit.qsc" */	/* host code */
                if (r_state != DELETED)
                {
                        if (bfr[0])
                                SIputrec(bfr, fp);
                        else
                                SIputc(' ', fp);
                        SIputc('\n', fp);
                }
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 193 "tfedit.qsc" */	/* host code */
        /*
        **    Close the temp file.
        */
	if (iiuftfdDiagFunc != NULL) 
		(*iiuftfdDiagFunc)(ERx("IIUFtfeTblFldEdit: closing '%s'\n"),
				   filename);	
	if ( (rc=SIclose(fp)) != OK)
        {
                _VOID_ IIUGfmt(msg, 80, ERget(E_UF000A_Unable_to_close_file),
                        	2, (PTR)filename, (PTR)&rc);
		return (FAIL);
	}
        /*
        **    Invoke the default editor on the temp file
        */
        FTclear();
        FTrestore(FT_NORMAL);
        UTedit(NULL, &loc);
        FTrestore(FT_FORMS);
        FTclear();
        /*
        **    Make sure the file is still there
        */
        if (LOexist(&loc) != OK)
        {
                FEmsg(ERget(S_UF000B_File_empty_table_unch),
                        TRUE, filename);
                return(OK);
        }
        /*
        **    Open the temp file for input
        */
	if (iiuftfdDiagFunc != NULL) 
		(*iiuftfdDiagFunc)
			(ERx("IIUFtfeTblFldEdit: opening '%s' for input\n"),
			     filename);
        if ((rc=SIfopen(&loc, ERx("r"), SI_TXT, 512, &fp)) != OK)
        {
                _VOID_ IIUGerr(E_UF000C_cant_open_f_tbl_unchg, 0, 2, 
				(PTR)filename, (PTR)&rc);
                return(OK);
        }
        /*
        **    Read in the file
        */
/* # line 242 "tfedit.qsc" */	/* clear */
    {
      IIfldclear(tblfldname);
    }
/* # line 243 "tfedit.qsc" */	/* host code */
	tfinput.tag = TFIN_FILE;
	tfinput.var.fp = fp;
        rc = load_tf(&tfinput, formname, tblfldname, fldname);
        if (rc != OK)
        {
                _VOID_ IIUGerr(E_UF000D_cant_read_temp_file, 0, 1, 
				(PTR)filename);
        }
        /*
	**    Close and delete the temp file
        */
	if (iiuftfdDiagFunc != NULL) 
		(*iiuftfdDiagFunc)
			(ERx("IIUFtfeTblFldEdit: closing '%s'\n"),filename);
        if ( (rc=SIclose(fp)) != OK)
        {
                _VOID_ IIUGfmt(msg, 80, ERget(E_UF000A_Unable_to_close_file), 2,
                        	(PTR) filename, (PTR)rc);
		return (FAIL);
 	}
        LOpurge(&loc,0);
        return(OK);
  }
/*{
** Name: IIUFtfrTblFldRegister	- Register a diagnostic function
**
** Description:
**	Register a diagnostic function, which can print a trace of the
**	tablefield editing session.  Provided for use by fstm, which
**	registers FSdiag for this purpose.
**
**	The function will be called via:
**		(*iiuftfdDiagFunc)(fmt, par1, par2, ...)
**	with parameters suitable for passing to SIprintf or FTdiag.
**		
**
** Inputs:
**	func		VOID (*)()	Function to register
**
** Outputs:
**	none
**
**	Returns:
**		none
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	8/7/89 (Mike S)	Initial version
*/
VOID
IIUFtfrTblFldRegister(func)
VOID (*func)();
{
	iiuftfdDiagFunc = func;		/* Register it */
}
/*{
** Name:	IIUFtfiTblFldInsert  -	Insert a blank line into the tablefield
**
** Description:
**	Insert a blank line into the tablefield
**
** Inputs:
**	form	char *		name of form
**	tf	char *		name of tablefield
**
** Outputs:
**	none
**
**	Returns:
**		none
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	8/7/89 (Mike S)	Initial version
*/
VOID
  IIUFtfiTblFldInsert(form, tf)
  char *form;
  char *tf;
  {
    i4 currow;
/* # line 339 "tfedit.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,(char *)"",(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&currow,29,(char *)0,0);
      } /* IIiqset */
    }
/* # line 340 "tfedit.qsc" */	/* host code */
	currow = currow - 1;
/* # line 341 "tfedit.qsc" */	/* insertrow */
    {
      if (IItbsetio(5,form,tf,currow) != 0) {
        if (IItinsert() != 0) {
          IITBceColEnd();
        } /* IItinsert */
      } /* IItbsetio */
    }
  }
/* # line 344 "tfedit.qsc" */	/* host code */
/*{
** Name:	IIUFtfiTblFldDelete  -	Delete a blank line from the tablefield
**
** Description:
**	Delete a line from the tablefield
**
** Inputs:
**	dform	char *		name of form
**	dtf	char *		name of tablefield
**
** Outputs:
**	none
**
**	Returns:
**		none
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	8/7/89 	(Mike S)	Initial version
**	10/15/89 (Mike S)	Delete last line
*/
VOID
  IIUFtfdTblFldDelete(form, tf)
  char *form;
  char *tf;
  {
/* # line 374 "tfedit.qsc" */	/* deleterow */
    {
      if (IItbsetio(4,form,tf,-2) != 0) {
        if (IItdelrow(0) != 0) {
        } /* IItdelrow */
      } /* IItbsetio */
    }
  }
/* # line 378 "tfedit.qsc" */	/* host code */
/*{
** Name:	IIUFtflTblFldLoad - Load a file into the tablefield
**
** Description:
**	Prompt the user for a file name, and load that file into the tablefield.
**	File records will be wrapped at whitespace, if possible.
**
** Inputs:
**	formname	char *	name of form
**	tblfldname	char *	name of tablefield
**	fldname		char *	name of column
**
** Outputs:
**	msgbuf		char **	Pointer to message string.
**				Contains valid data only if status is FAIL.
**
**	Returns:
**		STATUS		OK if all is well
**				FAIL if there's an error closing the file
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	8/8/89 (Mike S)	Abstracted from fstm/fsload.qsc
**	10/12/89 (teresal) Brought over kenl's DG fix (10/6) from 
**			   fstm/fsload.qsc.  Fix: broke apart IF statement 
**			   for SIfopen so DG can refresh screen after open.
*/
STATUS
IIUFtflTblFldLoad(formname, tblfldname, fldname, msgbuf)
char *formname;
char *tblfldname;
char *fldname;
char **msgbuf;
{
        i4         rc;
        LOCATION        loc;
        FILE            *fp;
	char            msg[80];
  char filename[MAX_LOC];
	TFINPUT		tfinput;
	*msgbuf = msg;
        if (iiuftfdDiagFunc != NULL)
                (*iiuftfdDiagFunc)(ERx("IIUFtflTblFldLoad: entered\n"));
/* # line 427 "tfedit.qsc" */	/* prompt */
  {
    IIprmptio(0,ERget(S_UF000E_Enter_filenam_to_read),(short *)0,1,32,MAX_LOC
    -1,filename);
  }
/* # line 429 "tfedit.qsc" */	/* host code */
        if (!filename[0])
                return(OK);
# ifdef CMS
        TOwscnme(filename);
# endif
        /*
        **    Open the file for input
        */
        LOfroms(PATH&FILENAME, filename, &loc);
        if (LOexist(&loc) != OK)
        {
                _VOID_ IIUGerr(E_UF000F_File_does_not_exist, 0, 1, 
				(PTR)filename);
                return(OK);
        }
	if (iiuftfdDiagFunc != NULL)
		(*iiuftfdDiagFunc)
			(ERx("IIUFtflTblFldLoad: opening '%s' for input\n"),
			     filename);
	rc = SIfopen(&loc, ERx("r"), SI_TXT, 512, &fp);
# ifdef DGC_AOS
/* # line 453 "tfedit.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 454 "tfedit.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 455 "tfedit.qsc" */	/* host code */
# endif
	if (rc != OK)
        {
                IIUGerr(E_UF0010_cant_open_file_input, 0, 2, (PTR)filename, 
			(PTR)&rc);
                return(OK);
        }
        /*
        **    Read in the file
        */
	tfinput.tag = TFIN_FILE;
	tfinput.var.fp = fp;
        rc = load_tf(&tfinput, formname, tblfldname, fldname);
        if (rc != OK)
        {
                _VOID_ IIUGerr(E_UF0011_Unable_to_read_file, 0, 1, 
				(PTR)filename);
        }
        /*
        **    Close the file
        */
        if (iiuftfdDiagFunc != NULL)
                (*iiuftfdDiagFunc)(ERx("IIUFtflTblFldLoad: closing '%s'\n"), 
					filename);
        if ( (rc=SIclose(fp)) != OK)
        {
                _VOID_ IIUGfmt(msg, 80, ERget(E_UF000A_Unable_to_close_file), 2,
                        (PTR)filename, (PTR)&rc);
                return(FAIL);
        }
        return (OK);
}
/*{
** Name:	IIUFtfnTblFldNiLoad - Load a file into the tablefield
**
** Description:
**	Load the supplied file into the tablefield.
**	File records will be wrapped at whitespace, if possible.
**
** Inputs:
**	file		LOCATION * file to load
**	formname	char *	name of form
**	tblfldname	char *	name of tablefield
**	fldname		char *	name of column
**
** Outputs:
**	none
**
**	Returns:
**		STATUS		OK if all is well
**				FAIL if there's an error processing the file
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	11/8/89 (Mike S)	Initial version
*/
STATUS
IIUFtfnTblFldNiLoad(file, formname, tblfldname, fldname)
LOCATION *file;
char *formname;
char *tblfldname;
char *fldname;
{
        STATUS         rc;
        FILE            *fp;
	TFINPUT		tfinput;
        /*
        **    Open the file for input
        */
	rc = SIfopen(file, ERx("r"), SI_TXT, 512, &fp);
	if (rc != OK) return rc;
        /*
        **    Read in the file
        */
	tfinput.tag = TFIN_FILE;
	tfinput.var.fp = fp;
        rc = load_tf(&tfinput, formname, tblfldname, fldname);
        if (rc != OK) return (rc);
        /*
        **    Close the file
        */
        if ( (rc=SIclose(fp)) != OK) return (rc);
        return (OK);
}
/*{
** Name:	IIUFtfuTblFldUnload - Unload the tablefield into a file
**
** Description:
**	The user will supply a file name into which the tablefield
**	contents will be written.
**
**	The posible results from calling this function:
**
**					return	written	msgbuf		
**	The file was created		OK	TRUE	garbage
**	and written successfully.
**
**	The file couldn't be created. 	OK	FALSE	garbage
**	file name.
**
**	The file was created, but an 	FAIL	FALSE	a message
**	error occured writing or closing it.
**	
**
** Inputs:
**	fname		char *	form name
**	tblname		char *	tablefield name
**	fldnm		char *	tablefield column name
**
** Outputs:
**      msgbuf          char ** Pointer to message string.
**                              Contains valid data only if status is FAIL.
**	written		bool *	TRUE if file was written; FALSE otherwise.
**
**      Returns:
**              STATUS          OK if all is well
**                              FAIL if there's an error in writing or 
**				     closing the file
**
**      Exceptions:
**              none
**
** Side Effects:
**
** History:
**      8/8/89 (Mike S) Abstracted from fstm/fsload.qsc
**	10/12/89 (teresal) Brought over kenl's DG fix (10/6) from 
**			   fstm/fsload.qsc.  Fix: broke apart IF statement 
**			   for SIfopen so DG can refresh screen after open.
**	2/12/90	 (Mike S)	Added "written" argument.
*/
STATUS
  IIUFtfuTblFldUnload(fname, tblname, fldnm, msgbuf, written)
  char *fname;
  char *tblname;
  char *fldnm;
char 	**msgbuf;
bool	*written;
  {
        i4         rc;
        LOCATION        loc;
        FILE            *fp;
        char            msg[80];
    char filenm[MAX_LOC];
    char bufr[BFRSIZE];
    i4 r_state;
	*written = FALSE;
        *msgbuf = msg; 
	if (iiuftfdDiagFunc != NULL)
		(*iiuftfdDiagFunc)(ERx("IIUFtfuTblFldUnload: entered\n"));
/* # line 622 "tfedit.qsc" */	/* prompt */
    {
      IIprmptio(0,ERget(S_UF0012_Enter_filename_write),(short *)0,1,32,MAX_LOC
      -1,filenm);
    }
/* # line 624 "tfedit.qsc" */	/* host code */
        if (!filenm[0])
                return(OK);
# ifdef CMS
        TOwscnme(filenm);
# endif
        /*
        **  Open the file for input
        */
        LOfroms(PATH&FILENAME, filenm, &loc);
        if (iiuftfdDiagFunc != NULL) 
                (*iiuftfdDiagFunc)
		    (ERx("IIUFtfuTblFldUnload: opening '%s' for input\n"), 
		    filenm);
	rc = SIfopen(&loc, ERx("w"), SI_TXT, 512, &fp);
# ifdef DGC_AOS
/* # line 642 "tfedit.qsc" */	/* clear */
    {
      IIclrscr();
    }
/* # line 643 "tfedit.qsc" */	/* redisplay */
    {
      IIredisp();
    }
/* # line 644 "tfedit.qsc" */	/* host code */
# endif
	if (rc != OK)
        {
                _VOID_ IIUGerr(E_UF0013_cant_open_file_output, 0, 2, 
				(PTR)filenm, (PTR)&rc);
                return(OK);
        }
        /*
        **  Read in the file and load rows back into the input form.
        */
/* # line 655 "tfedit.qsc" */	/* unloadtable */
    {
      if (IItbact(fname,tblname,0) == 0) goto IItbE2;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,BFRSIZE-1,bufr,fldnm);
        IItcogetio((short *)0,1,30,4,&r_state,(char *)"_STATE");
        IITBceColEnd();
        {
/* # line 657 "tfedit.qsc" */	/* host code */
                if (r_state != DELETED)
                {
                        STcat(bufr, ERx("\n"));
                        if (rc = (SIputrec(bufr, fp)) == EOF)
                        {
                                _VOID_ IIUGfmt(msg, 80, 
                                        ERget(E_UF0014_Unable_to_write_file),
                                        2, (PTR)filenm, (PTR)&rc);
                                return(FAIL);
                                break;
                        }
                }
        }
      } /* IItunload */
IItbE2:
      IItunend();
    }
/* # line 671 "tfedit.qsc" */	/* host code */
        /*
        **  Close the file
        */
        if (iiuftfdDiagFunc != NULL)
                (*iiuftfdDiagFunc)(ERx("IIUFtfuTblFldUnload: closing '%s'\n"), 
				   filenm);
        if ( (rc = SIclose(fp)) != OK)
        {
                _VOID_ IIUGfmt(msg, 80, ERget(E_UF000A_Unable_to_close_file),
                        	2, (PTR)filenm, (PTR)&rc);
		return(FAIL);        
	}
	*written = TRUE;
        return (OK);
  }
/*{
** Name:	IIUFtfsTblFldString	Load a string into the tablefield
**
** Description:
**	Load a string into the tablefield. Split the string into rows at
**	newlines.  If the string has to be split between newlines, try to
**	split it at whitespace.
**
** Inputs:
**	string		char *	string to load
**      fname           char *  form name
**      tblname         char *  tablefield name
**      fldnm           char *  tablefield column name
**
** Outputs:
**      none
**                              Contains valid data only if status is FAIL.
**
**      Returns:
**              STATUS          OK if all is well
**                              not OK if there's a fromatting error
**
**      Exceptions:
**              none
**
** Side Effects:
**
** History:
**	8/8/89 (Mike S)	Initial version
*/
STATUS
IIUFtfsTblFldString(string, fname, tblname, fldnm)
char	*string;	/* Input string */
char	*fname;		/* Form name */
char	*tblname;	/* tablefield name */
char	*fldnm;		/* Tablefield column name */
{
	TFINPUT	tfinput;
	/* Set up string descriptor */
	tfinput.tag = TFIN_STRING;
	tfinput.var.string = string;
	/* Load the tablefield */
	return(load_tf(&tfinput, fname, tblname, fldnm));
}
/*{
** Name:	str_getrec - Get a "record" from a string
**
** Description:
**	Like SIgetrec -- copy characters from a string, up to a given length,
**	until we see either a newline or an EOS. Like SIgetrec, if a 
**	"record" is too big to fit in the buffer, we split it.  Since our
**	record size in this file is BFRSIZE (254), this should be moot.
**
** Inputs:
**	maxbytes	i4	Maximum bytes to copy
**	string		char **	Where to copy from
**
** Outputs:
**	buffer		char *	Buffer to copy into
**	string		char ** Next byte in string
**
**	Returns:
**		STATUS		OK	if there are more chars in the string
**				FAIL	if we're at EOS
**
**	Exceptions:
**		none
**
** Side Effects:
**		none
**
** History:
**	8/8/89 (Mike S) Initial version
*/
static STATUS
str_getrec(buffer, maxbytes, string)
register char	*buffer;
i4	maxbytes;
char	**string;
{
	register char *ptr = *string;
	register i4  csize;
	if (*ptr == EOS)
		return(FAIL);		/* Empty string */
	maxbytes--;	/* Leave space for null terminator */
	for ( ; ; )
	{
		if (CMcmpcase(ptr, _nl) == 0)
		{
			/* We've found a newline -- skip it and break */
			CMnext(ptr);
			break;
		}
		else if (*ptr == EOS)
		{
			/* Stay at EOS */
			break;
		}
		else if ((csize = CMbytecnt(ptr)) >= maxbytes) 
		{
			/* If the next character doesn't fit, break */
			break;
		}
		else
		{
			/* copy the character */
			CMcpyinc(ptr, buffer);
			maxbytes -= csize;
		}
	}
	/* We've reached the end of the "record" */
	*buffer = EOS;
	*string = ptr;
	return (OK);
}
/*{
** Name:	load_tf		- Load a tablefield
**
** Description:
**	load_tf will load a tablefield from either a sequential file or a 
**	string.  A file is loaded by records, a string by lines ending in
**	'\n'.  If a line is greater than the width of the table field,
**	we try to wrap at spaces or tabs.
**
** Inputs:
**	tfinput		TFINPUT *	Input descriptor
**      fname           char *  form name
**      tblname         char *  tablefield name
**      fldnm           char *  tablefield column name
**
** Outputs:
**	none
**
**      Returns:
**              i4              OK	If all is well
**				FAIL	If memory can't be allocated
**				other   If a formatting error is returned
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	8/8/89 (Mike S)	Abstracted from fstm/fsload.qsc
**	8/14/89 (Mike S) Keep blank lines, except for leading blanks
**	1/8/90 (Mike S)  Load lines above current row
**      3/21/91 (elein)         (b35574) Add FALSE parameter to call to
**                              fmt_multi. TRUE is used internally for boxed
**                              messages only.  They need to have control
**                              characters suppressed.
**	4-dec-1996 (angusm)	various necessary changes to operation
**				of F_CU format have caused text to
**				have a leading blank inserted on return
**				from editor. Redo text buffer to avoid.
**				(bug 79222/76745)
**	15-jan-1997 (angusm)	Redo above fix - too simplistic. Now check
**				leading blanks before/after fmt_multi()
**				(bug 80037)
*/
static STATUS
  load_tf(tfinput, formname, tblfldname, fldname)
TFINPUT	*tfinput;
  char *formname;
  char *tblfldname;
  char *fldname;
  {
	/* Input record descriptor */
        AFE_DCL_TXT_MACRO(BFRSIZE+1)    dbv_textbuf;
        DB_TEXT_STRING  *dbv_linebuf = (DB_TEXT_STRING *) &dbv_textbuf;
	DB_DATA_VALUE	dbv;
        char            *dbv_ptr;		/* Text pointer */
	/* Tablefield row desciptor */
        AFE_DCL_TXT_MACRO(BFRSIZE+1)    textbuf;
	DB_TEXT_STRING  *linebuf = (DB_TEXT_STRING *) &textbuf;
	DB_DATA_VALUE	display;
    i4 cwidth;
	PTR             area;			/* Formatting area */
        PTR             worksp;			/* Formatting workspace */
        char            fmtstr[30];		/* Format string */
        FMT             *fmt;			/* Format structure */
        i4              asize, wsize;
        i4              dumlen;
        STATUS          rval;
	i4		lines;			/* Number of lines loaded */
	i4		splitlines;		/* Number of lines split */
	i4		leadblanks[2];
    i4 cur_rec;
    i4 row_state;
    i4 cur_row;
    i4 tot_recs;
        /* use cf0.<width> or cu0.<width> format */
/* # line 895 "tfedit.qsc" */	/* inquire_frs */
    {
      if (IIiqset(4,0,formname,tblfldname,(char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&cwidth,24,fldname,0);
      } /* IIiqset */
    }
/* # line 897 "tfedit.qsc" */	/* host code */
        if (_format == NULL)
		IIUFsnfSetNormFmt();
        STprintf(fmtstr, _format, cwidth);
	/* Init adf */
	if (cb == NULL)
		cb = FEadfcb();
	/* Allocate formatting area */
        if ((rval = fmt_areasize(cb, fmtstr, &asize)) != OK)
        	return rval;
        if ((area = FEreqmem((u_i4)0, (u_i4)asize, 
			     (bool)TRUE, (STATUS *)NULL)) 
		== NULL)
		return (FAIL);
	/* Make format string */
	if ((rval = fmt_setfmt(cb, fmtstr, area, &fmt, &dumlen)) != OK)
		return rval;
	/* Init input descriptor */
        dbv.db_datatype = DB_TXT_TYPE;
        dbv.db_length = sizeof(dbv_textbuf);
        dbv.db_data = (PTR)dbv_linebuf;
        dbv_ptr = (char *)dbv_linebuf->db_t_text;
	/* Allocate workspace */	
        if ((rval = fmt_workspace(cb, fmt, &dbv, &wsize)) != OK)
        	return rval;
        if ((worksp = FEreqmem((u_i4)0, (u_i4)wsize, 
			       (bool)TRUE, (STATUS *)NULL))
		== NULL)
		return (FAIL);
	/* Init tablefield column desriptor */ 
        display.db_datatype = DB_LTXT_TYPE;
        display.db_length = sizeof(textbuf);
        display.db_data = (PTR)linebuf;
        linebuf->db_t_count = cwidth;
	/* Get our current tablefield record */
/* # line 938 "tfedit.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,formname,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&tot_recs,32,tblfldname,0);
      } /* IIiqset */
    }
/* # line 939 "tfedit.qsc" */	/* host code */
	if (tot_recs <= 0)
	{
		cur_rec = 0;
	}
	else
	{
    char statebuf[30];
/* # line 947 "tfedit.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,formname,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&cur_row,29,tblfldname,0);
      } /* IIiqset */
    }
/* # line 948 "tfedit.qsc" */	/* getrow */
    {
      if (IItbsetio(2,formname,tblfldname,cur_row) != 0) {
        IItcogetio((short *)0,1,30,4,&cur_rec,(char *)"_RECORD");
        IItcogetio((short *)0,1,30,4,&row_state,(char *)"_STATE");
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 950 "tfedit.qsc" */	/* host code */
		/* 
		** If the state is 0, and there's only one record, it's a
		** fake record in an empty tablefield.  Ignore it.
		*/
		if ((0 == row_state) && (1 == tot_recs))
			cur_rec = 0;
	}
	/* Loop through input */
	for (lines = 0; ; )
	{
    char *ptr;
		char	*tptr;
		i4	i;
		bool 	more;
		STATUS 	status; 
		/* Get input line */
		if (tfinput->tag == TFIN_FILE)
			status = SIgetrec(dbv_ptr, BFRSIZE, tfinput->var.fp);
		else
			status = str_getrec(dbv_ptr, BFRSIZE, 
					    &tfinput->var.string);
		if (status != OK) break;
		/* Prepare to split input line into tablefield rows */
		STtrmwhite(dbv_ptr);
		dbv_linebuf->db_t_count = STlength(dbv_ptr);
		tptr = dbv_ptr;
		leadblanks[0]=0;
		while (CMwhite(tptr))
		{
			if (*tptr == '\t')
				leadblanks[0]+=8;
			else
				leadblanks[0]++;
			tptr++;
		}
		rval = IIfmt_init(cb, fmt, &dbv, worksp);
		if (rval != OK)
			return rval;
		/* Load each row into the tablefield */
		for (splitlines = 0;;)
		{
			rval = fmt_multi(cb, fmt, &dbv, worksp, 
				&display, &more, FALSE, FALSE);
			if (rval != OK)
				return rval;
			if (!more)
			    break;	/* No more lines */
/*
** bug 79222/76745/80037
*/
			tptr = (char *)linebuf->db_t_text;
			leadblanks[1]=0;
			while (CMwhite(tptr))
			{
				if (*tptr == '\t')
					leadblanks[1]+=8;
				else
					leadblanks[1]++;
				tptr++;
			}
			tptr = (char *)linebuf->db_t_text;
			if ( leadblanks[1] == (leadblanks[0] +1) )
			{
				for (i=0; i<linebuf->db_t_count-1; i++, tptr++)
					*tptr = *(tptr+1);
				*tptr = ' ';
				linebuf->db_t_count--;
			}
/*
** bug 79222/76745/80037
*/
			ptr = (char *)linebuf->db_t_text;
			if (cur_rec <= 0)
			{
/* # line 1030 "tfedit.qsc" */	/* loadtable */
    {
      if (IItbact(formname,tblfldname,1) != 0) {
        IItcoputio(fldname,(short *)0,1,32,0,ptr);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1031 "tfedit.qsc" */	/* host code */
			}
			else
			{
/* # line 1034 "tfedit.qsc" */	/* scroll */
    {
      if (IItbsetio(1,formname,tblfldname,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,cur_rec) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 1035 "tfedit.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,formname,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&cur_row,29,tblfldname,0);
      } /* IIiqset */
    }
/* # line 1037 "tfedit.qsc" */	/* host code */
				cur_row--;
/* # line 1038 "tfedit.qsc" */	/* insertrow */
    {
      if (IItbsetio(5,formname,tblfldname,cur_row) != 0) {
        if (IItinsert() != 0) {
          IItcoputio(fldname,(short *)0,1,32,0,ptr);
          IITBceColEnd();
        } /* IItinsert */
      } /* IItbsetio */
    }
/* # line 1040 "tfedit.qsc" */	/* host code */
				cur_rec++;
			}
			lines++;
			splitlines++;
		}
		/* 
		** Ignore a blank line, if it's a leading blank line in a
		** file.
		*/
		if ((splitlines == 0) && 
		    (lines > 0 || TFIN_STRING == tfinput->tag))
		{
			if (cur_rec <= 0)
			{
/* # line 1055 "tfedit.qsc" */	/* loadtable */
    {
      if (IItbact(formname,tblfldname,1) != 0) {
        IItcoputio(fldname,(short *)0,1,32,0,_blank);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1056 "tfedit.qsc" */	/* host code */
			}
			else
			{
/* # line 1059 "tfedit.qsc" */	/* scroll */
    {
      if (IItbsetio(1,formname,tblfldname,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,cur_rec) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 1060 "tfedit.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,formname,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&cur_row,29,tblfldname,0);
      } /* IIiqset */
    }
/* # line 1062 "tfedit.qsc" */	/* host code */
				cur_row--;
/* # line 1063 "tfedit.qsc" */	/* insertrow */
    {
      if (IItbsetio(5,formname,tblfldname,cur_row) != 0) {
        if (IItinsert() != 0) {
          IItcoputio(fldname,(short *)0,1,32,0,_blank);
          IITBceColEnd();
        } /* IItinsert */
      } /* IItbsetio */
    }
/* # line 1065 "tfedit.qsc" */	/* host code */
				cur_rec++;
			}
			lines++;
		}
	}
	/* Scroll to the record we began at. (1 if the tablefield was empty) */
	cur_rec = max(cur_rec, 1);
/* # line 1072 "tfedit.qsc" */	/* scroll */
    {
      if (IItbsetio(1,formname,tblfldname,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,cur_rec) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 1074 "tfedit.qsc" */	/* host code */
	return OK;
  }
/*{
** Name:	IIUFtfbTblFldBreak - Break a tablefield row into two
**
** Description:
**	Break a tablefield row at the current cursor position.
**
** Inputs:
**	formname	char *	form name
**	tfname		char *	tablefield name
**	fldname		char *	column name
**
** Outputs:
**	none
**
**	Returns:
**		none
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	8/28/89 (Mike S)	Initial version
**	10/2/91 (jillb--DGC)
**            Changed fmt_init to IIfmt_init since it is a fairly global
**            routine.  The old name, fmt_init, conflicts with a Green Hills
**            Fortran function name.
*/
VOID
  IIUFtfbTblFldBreak(formname, tfname, fldname)
  char *formname;
  char *tfname;
  char *fldname;
  {
    i4 curcol;
    i4 colstart;
    char buffer[BFRSIZE];
    char *second;
	char temp[5];		/* Temporary character */
	i4  length;		/* Length of first new line */
	/* 
	** Get current column position -- we assume it's the start 
	** of a character
	*/
/* # line 1124 "tfedit.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&curcol,49,(char *)0,0);
      } /* IIiqset */
    }
/* # line 1125 "tfedit.qsc" */	/* inquire_frs */
    {
      if (IIiqset(5,0,formname,tfname,(char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&colstart,47,(char *)0,0);
      } /* IIiqset */
    }
/* # line 1126 "tfedit.qsc" */	/* host code */
	length = curcol - colstart;
	/* Get line from tablefield */
/* # line 1129 "tfedit.qsc" */	/* getrow */
    {
      if (IItbsetio(2,formname,tfname,-2) != 0) {
        IItcogetio((short *)0,1,32,BFRSIZE-1,buffer,fldname);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1130 "tfedit.qsc" */	/* host code */
	second = buffer + length;
	CMcpychar(second, temp);
	*second = EOS;
	/* Change first line */
/* # line 1135 "tfedit.qsc" */	/* putrow */
    {
      if (IItbsetio(3,formname,tfname,-2) != 0) {
        IItcoputio(fldname,(short *)0,1,32,0,buffer);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1137 "tfedit.qsc" */	/* host code */
	/* Insert second line */
	CMcpychar(temp, second);
/* # line 1139 "tfedit.qsc" */	/* insertrow */
    {
      if (IItbsetio(5,formname,tfname,-2) != 0) {
        if (IItinsert() != 0) {
          IItcoputio(fldname,(short *)0,1,32,0,second);
          IITBceColEnd();
        } /* IItinsert */
      } /* IItbsetio */
    }
  }
/* # line 1142 "tfedit.qsc" */	/* host code */
/*{
** Name:	IIUFtfjTblFldJoin - Join two tablefield rows into one
**
** Description:
**	Join two tablefield rows into one.  Delete trailing whitespace
**	from both and leading whitespace from the second.
**
** Inputs:
**	formname	char *	form name
**	tfname		char *	tablefield name
**	fldname		char *	column name
**
** Outputs:
**	none
**
**	Returns:
**		none
**
**	Exceptions:
**		none
**
**		none
**
** Side Effects:
**
** History:
**	8/28/89 (Mike S)	Initial version
*/
VOID
  IIUFtfjTblFldJoin(formname, tfname, fldname)
  char *formname;
  char *tfname;
  char *fldname;
  {
    char buffer1[BFRSIZE];
    char buffer2[BFRSIZE];
    char *start2;
    i4 maxsize;
    i4 datarecs;
    i4 currec;
    i4 currow;
    i4 nextrec;
    i4 nextrow;
    i4 disprows;
	bool	scroll;			/* Do we have to scroll ? */
	bool	join;			/* Is there a second row to join ? */
	/* Get tablefield data */
/* # line 1191 "tfedit.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,formname,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&disprows,30,(char *)0,0);
        IIiqfsio((short *)0,1,30,4,&datarecs,32,(char *)0,0);
        IIiqfsio((short *)0,1,30,4,&currow,29,(char *)0,0);
      } /* IIiqset */
    }
/* # line 1194 "tfedit.qsc" */	/* host code */
	/* Get the first rows */
/* # line 1195 "tfedit.qsc" */	/* getrow */
    {
      if (IItbsetio(2,formname,tfname,-2) != 0) {
        IItcogetio((short *)0,1,32,BFRSIZE-1,buffer1,fldname);
        IItcogetio((short *)0,1,30,4,&currec,(char *)"_RECORD");
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1196 "tfedit.qsc" */	/* host code */
	if (currec == datarecs)
		return;		/* No second row to join */
	nextrec = currec + 1;
	/* 
	** Get the second row. We will have to scroll if we're at the bottom
	** line.
	*/
	scroll = (currow == disprows);
	if (scroll)
	{
/* # line 1207 "tfedit.qsc" */	/* scroll */
    {
      if (IItbsetio(1,formname,tfname,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,nextrec) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 1208 "tfedit.qsc" */	/* getrow */
    {
      if (IItbsetio(2,formname,tfname,-2) != 0) {
        IItcogetio((short *)0,1,32,BFRSIZE-1,buffer2,fldname);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1209 "tfedit.qsc" */	/* host code */
	}
	else
	{
		nextrow = currow + 1;
/* # line 1213 "tfedit.qsc" */	/* getrow */
    {
      if (IItbsetio(2,formname,tfname,nextrow) != 0) {
        IItcogetio((short *)0,1,32,BFRSIZE-1,buffer2,fldname);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1214 "tfedit.qsc" */	/* host code */
	}
	/* Trim whitespace */
	STtrmwhite(buffer1);
	STtrmwhite(buffer2);
	start2 = STskipblank(buffer2, STlength(buffer2));
	/* If the second row isn't blank, join the two together */
	join = (buffer2 != NULL && *buffer2 != EOS);
	if (join)
	{
/* # line 1225 "tfedit.qsc" */	/* inquire_frs */
    {
      if (IIiqset(4,0,formname,tfname,(char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&maxsize,24,fldname,0);
      } /* IIiqset */
    }
/* # line 1226 "tfedit.qsc" */	/* host code */
		if (STlength(buffer1) + STlength(_blank) + STlength(start2) 
			> maxsize)
		{
			IIUGmsg(ERget(E_UF0015_Line_too_long), TRUE, 0); 
			return;
		}
		STpolycat(2, _blank, start2, buffer1 + STlength(buffer1));
	}
	/* 
	** Change the first line, if we joined.  Always delete the second
	** line.
	*/
	if (scroll)
	{
/* # line 1241 "tfedit.qsc" */	/* deleterow */
    {
      if (IItbsetio(4,formname,tfname,-2) != 0) {
        if (IItdelrow(0) != 0) {
        } /* IItdelrow */
      } /* IItbsetio */
    }
/* # line 1242 "tfedit.qsc" */	/* scroll */
    {
      if (IItbsetio(1,formname,tfname,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,currec) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 1243 "tfedit.qsc" */	/* host code */
	}
	else
	{
/* # line 1246 "tfedit.qsc" */	/* deleterow */
    {
      if (IItbsetio(4,formname,tfname,nextrow) != 0) {
        if (IItdelrow(0) != 0) {
        } /* IItdelrow */
      } /* IItbsetio */
    }
/* # line 1247 "tfedit.qsc" */	/* host code */
	}
	if (join)
/* # line 1249 "tfedit.qsc" */	/* putrow */
    {
      if (IItbsetio(3,formname,tfname,-2) != 0) {
        IItcoputio(fldname,(short *)0,1,32,0,buffer1);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1250 "tfedit.qsc" */	/* host code */
	return;
  }
/*
**	We use one of two 'wrap' formats to load the tablefield.  The
**	usual one, "cf0.<width>", wraps at whitespace.  A special internal
**	format, "cu0.<width>", recognizes a single or double-quoted string aa
**	an entity, and tries to keep it one one line.  It should only be used
**	when the input is expected to be SQL, QUEL, or 4GL code.
*/
VOID
IIUFsifSetIntFmt()
{
	_format = ERx("cu0.%d");
}
VOID
IIUFsnfSetNormFmt()
{
	_format = ERx("cf0.%d");
}
