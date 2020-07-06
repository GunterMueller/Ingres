# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<si.h>
#include	<lo.h>
#include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
# include       <ut.h>
# include       <ft.h>
# include <abclass.h>
/*
#include	<abclass.h>
*/
#include	<fdesc.h>
#include	<abfrts.h>
#include	<abfcnsts.h>
#include	<abfcompl.h>
#include	"erab.h"
/*
** Name:	abgconsf.qsc -	ABF Global Constant File Specification screen.
**
** Description:
**	Contains the frame for the link image frame of ABF.  Defines:
**
**	IIABgcfGlobalConstantFile()	Global Constant File spec. frame.
**	IIABscfSetConstantFile()	Set Global Constant File specification
**	IIABqcfQueryConstantFile()	Query Global Constant File specification
**	IIABccfCreateConstantFile()	Create and load Global Constant File.
**	IIABgceGlobalConstantEdit()	Edit Global Constant File.
**
** History:
**	Revision 6.5
**	13-oct-92 (davel)
**		First written.
**	19-oct-92 (davel)
**		Added IIABscfSetConstantFile() and IIABqcfQueryConstantFile().
**	20-oct-92 (essi)
**		Added IIABccfCreateConstantFile() and 
**	       	IIABgceGlobalConstantEdit().
**	12-nov-92 (davel)
**		Added <si.h> for VMS.
**	14-dec-92 (essi)
**		Cleaned up code and initialized the form with the
**		global constant of previous 'form entry' or blank.
**	16-dec-92 (essi)
**		Fixed minor bug and cleaned up the code.
**	12-jan-93 (essi)
**		Fixed Cancel, End and Help options.
**	22-jan-93 (davel)
**		add terminating \n in IIABccfCreateConstantFile.
**	25-jan-93 (davel)
**		initialize filename value from IIABqcfQueryConstantFile.
**	02-feb-93 (essi)
**		Changed IIABvfValidFile	to IIUGvfVerifyFile. Made 
**		IIUGvfVerifyFile a generic utility and placed under ug
**		in ugvf.c.
**	10-mar-93 (essi)
**		Added this history line for my previous fix to bug 49718.
**	18-oct-93 (daver)
**		Made fe.h appear before ug.h to avoid prototype related
**		compiler failures in ug.h.
**      03-may-1999 (hanch04)
**          SIputrec returns EOF on error.
*/
GLOBALREF	char *IIabExename;
#define _CreateExpl	F_AB00E3_Create_ConsFile_expl
#define _EditExpl	F_AB00E4_Edit_ConsFile_expl
#define _CancelExpl	F_FE0103_ExplCancel
#define _EndExpl	F_AB00E5_End_ConsFile_expl
#define _HelpExpl	F_FE0100_ExplFrameHelp
static READONLY
  char _GlobForm[]= ERx("globconsfile");
  char filebuf[MAX_LOC+1];
  i2 null_able;
VOID 	IIABscfSetConstantFile();
char	*IIABqcfQueryConstantFile();
static void init_form(char *fname);
/*{
** Name:	IIABgcfGlobalConstantFile() -	Global Constant File frame.
**
** Description:
**	Allow a user to build an executable image for an application.
**	The resulting executable is a stand alone program that the user
**	can run outside of ABF.
**
** Input:
**	app	{APPL *}  The application object to be linked; may not
**				be instantiated (i.e., ID is OC_UNDEFINED.)
**	appname	{char *} The application to be linked.
**	id	{OOID}  The application object ID.
**
** History:
**	13-oct-92 (davel)
**		First written.
*/
VOID
IIABgcfGlobalConstantFile(gcapp, gcappname)
  APPL *gcapp;
  char *gcappname;
{
	char    *filename;
  char appnam[FE_MAXNAME+1];
	init_form(_GlobForm);
/* # line 119 "abgconsf.qsc" */	/* display */
  {
    if (IIdispfrm(_GlobForm,(char *)"f") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
      char titlebuf[80];
/* # line 125 "abgconsf.qsc" */	/* host code */
		IIUGfmt(titlebuf, sizeof(titlebuf), 
			ERget(F_AB00E0_ConsFile_title), 
			1, IIabExename);
		STcopy(gcappname,appnam);
		filename = IIABqcfQueryConstantFile();
		if (filename != NULL)
		    STcopy(filename, filebuf);
		else
		    filebuf[0] = EOS;
/* # line 136 "abgconsf.qsc" */	/* putform */
      {
        if (IIfsetio(_GlobForm) != 0) {
          IIputfldio((char *)"title",(short *)0,1,32,0,titlebuf);
          IIputfldio((char *)"app_name",(short *)0,1,32,0,appnam);
          IIputfldio((char *)"gc_fname",(short *)0,1,32,0,filebuf);
        } /* IIfsetio */
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 142 "abgconsf.qsc" */	/* host code */
		if (IIABccfCreateConstantFile() != OK)
/* # line 143 "abgconsf.qsc" */	/* resume */
          {
            IIresfld((char *)"gc_fname");
            if (1) goto IIfdB1;
          }
/* # line 144 "abgconsf.qsc" */	/* host code */
/* # line 151 "abgconsf.sc" */	/* message */
  {
    IImessage((char *)"File successfully written.");
  }
/* # line 152 "abgconsf.sc" */	/* sleep */
  {
    IIsleep(1);
  }
/* # line 153 "abgconsf.sc" */	/* host code */
/* # line 146 "abgconsf.qsc" */	/* resume */
          {
            IIresfld((char *)"gc_fname");
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 151 "abgconsf.qsc" */	/* getform */
          {
            if (IIfsetio(_GlobForm) != 0) {
              IIgetfldio(&null_able,1,32,MAX_LOC+1-1,filebuf,(char *)
"gc_fname");
            } /* IIfsetio */
          }
/* # line 152 "abgconsf.qsc" */	/* host code */
                if ( null_able == DB_EMB_NULL || STlength(filebuf) == 0)
		{
			IIABscfSetConstantFile( (char *)NULL );
			filebuf[0] = EOS;
		}
		if ( IIUGvfVerifyFile ( filebuf, UG_IsExistingFile, FALSE, 
			TRUE, UG_ERR_ERROR ) == OK )
		{
			IIABgceGlobalConstantEdit( filebuf );
		}
/* # line 162 "abgconsf.qsc" */	/* resume */
          {
            IIresfld((char *)"gc_fname");
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 169 "abgconsf.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 174 "abgconsf.qsc" */	/* host code */
		/* validate the file entry, and then save/clear using the
		** "specified global files" interface.
		*/
		LOCATION	loc;
/* # line 178 "abgconsf.qsc" */	/* getform */
          {
            if (IIfsetio(_GlobForm) != 0) {
              IIgetfldio(&null_able,1,32,MAX_LOC+1-1,filebuf,(char *)
"gc_fname");
            } /* IIfsetio */
          }
/* # line 179 "abgconsf.qsc" */	/* host code */
                if ( null_able == DB_EMB_NULL || STlength(filebuf) == 0)
		{
			IIABscfSetConstantFile( (char *)NULL );
			filebuf[0] = EOS;
/* # line 183 "abgconsf.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
/* # line 184 "abgconsf.qsc" */	/* host code */
		}
		if ( IIUGvfVerifyFile ( filebuf, UG_IsExistingFile, FALSE, 
			TRUE, UG_ERR_ERROR ) == OK )
		{
			IIABscfSetConstantFile( filebuf );
/* # line 189 "abgconsf.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
/* # line 190 "abgconsf.qsc" */	/* host code */
		}
/* # line 191 "abgconsf.qsc" */	/* resume */
          {
            IIresfld((char *)"gc_fname");
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 197 "abgconsf.qsc" */	/* host code */
		FEhelp(ERx("abgconsf.hlp"), ERget(F_AB002F_ConstFileHlp));
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
    if (IInmuact(ERget(FE_Create),ERget(_CreateExpl),2,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Edit),ERget(_EditExpl),2,2,2) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Cancel),ERget(_CancelExpl),0,2,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,0,2,3) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_End),ERget(_EndExpl),2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,5) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,2,5) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 200 "abgconsf.qsc" */	/* host code */
}
/*
** Cache initialization of this form.
*/
static bool form_is_init = FALSE;
static void
init_form(fname)
char *fname;
{
	if (!form_is_init)
	{
		form_is_init = TRUE;
		IIARiaForm(fname);
	}
}
/*{
** Name:	IIABscfSetConstantFile() & IIABqcfQueryConstantFile()
**
** Description:
**	Store and retrieve the setting of the Global Constant File, which
**	is specified to be used as part of "image" or "go" operation.  The
**	value is set within the IIABgcfGlobalConstantFile() frame (in this 
**	source file), and also cleared from the top Application catalog screen.
**	This file specification is queried upon in abftab.c as part of the 
**	"image" and "go" operations.
**
** Inputs: (for IIABscfSetConstantFile)
**	cons_file {char *}	Specified constant file.
**
** Returns: (for IIABqcfQueryConstantFile)
**	{char *}	the specified constant file; NULL is none specified.
**
** History:
**	19-oct-92 (davel)
**		First written.
*/
static char     saved_cons_file[MAX_LOC + 1] = {EOS};
VOID
IIABscfSetConstantFile(char *cons_file)
{
	if (cons_file == NULL || *cons_file == EOS)
	{
		/* clear the constant file spec. */
		saved_cons_file[0] = EOS;
	}
	else
	{
		(void) STlcopy(cons_file, saved_cons_file, MAX_LOC);
	}
}
char *
IIABqcfQueryConstantFile()
{
	return (saved_cons_file[0] == EOS ? NULL : saved_cons_file);
}
/*{
** Name:	IIABccfGlobalConstantFile() -Global Constant File load module.
**
** Description:
**	Create a file of global constants for the application. The names
**	and values are extracted from catalogs and placed in a file. The 
**	file is neatly formatted as follows:
**
**	<name 32 pos>='<value 28 pos>' # comments start at pos 65
**
**	If the value is longer that 28 position, it is broken into pieces in
**	consecutive lines.
**
** Input:
**	loc	{LOCATION}	User defined filename.
**
** History:
**	19-oct-92 (essi)
**		First written.
**	26-jul-93 (blaise)
**		Preparing for internationalization: replaced string literals
**		written to global constants file with messages.
*/
STATUS
IIABccfCreateConstantFile()
{
	i4		flagword,i;
	LOINFORMATION	loinf;
	LOCATION	loc;
	FILE		*ccfp,*tmpfp;
	char		ccfbuf[180+FE_MAXNAME+1]; /*180 is sizeof abf_source*/
	char		langbrk[FE_MAXNAME+1];
	bool		firstrecord;
	char 		*vp;
	STATUS		status;
  char ccname[FE_MAXNAME+1];
  char ccvalue[180+1];
  char aname[FE_MAXNAME+1];
  char cclang[FE_MAXNAME+1];
  char cctype[FE_MAXNAME+1];
  OOID apid;
/* # line 305 "abgconsf.qsc" */	/* getform */
  {
    if (IIfsetio(_GlobForm) != 0) {
      IIgetfldio(&null_able,1,32,MAX_LOC+1-1,filebuf,(char *)"gc_fname");
    } /* IIfsetio */
  }
/* # line 306 "abgconsf.qsc" */	/* host code */
        if ( null_able == DB_EMB_NULL || STlength(filebuf) == 0 )
	{
		filebuf[0] = EOS;
	}
        status = IIUGvfVerifyFile ( filebuf, UG_IsNonExistingFile, FALSE, 
		TRUE, UG_ERR_ERROR );
	if ( status != OK )
		return ( FAIL );
/* # line 315 "abgconsf.qsc" */	/* getform */
  {
    if (IIfsetio(_GlobForm) != 0) {
      IIgetfldio(&null_able,1,32,FE_MAXNAME+1-1,aname,(char *)"app_nam\
e");
    } /* IIfsetio */
  }
/* # line 316 "abgconsf.qsc" */	/* host code */
        if ( null_able == DB_EMB_NULL )
	{
		IIUGerr( E_AB0309_BadAppName, 0, 0 );
		return ( FAIL );
	}
	LOfroms( PATH&FILENAME, filebuf, &loc ); 
	if ( SIfopen( &loc,"w",SI_TXT,0,&ccfp ) != OK )
	{
		IIUGerr( E_AB0307_CantOpenForWrite, 0, 0 );
		return ( FAIL );
	}
	STcopy( ERget(F_AB00EB_ConsFile_Str1), ccfbuf );
	if ( SIputrec( ccfbuf,ccfp ) == EOF )
	{
		IIUGerr( E_AB0308_CantWrite, 0, 0 );
		return ( FAIL );
	}
	if ( SIputrec( "#\n",ccfp ) == EOF )
	{
		IIUGerr( E_AB0308_CantWrite, 0, 0 );
		return ( FAIL );
	}
	STprintf( ccfbuf,"%s %-s\n#\n", ERget(F_AB00EC_ConsFile_Str2), aname );
	if ( SIputrec( ccfbuf,ccfp ) == EOF )
	{
		IIUGerr( E_AB0308_CantWrite, 0, 0 );
		return ( FAIL );
	}
	/*
	** The next 'select' must be a singleton since application names
	** are unique per database. An error here may mean catalog 
	** corruption. 
	*/
/* # line 419 "abgconsf.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"abgconsf1",6929,11512);
      IIputdomio((short *)0,1,32,0,aname);
      IIputdomio((short *)0,1,30,sizeof(OC_APPL),(void *)IILQint(OC_APPL));
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"abgconsf1",6929,11512);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select object_id from ii_objects where object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,aname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_class=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,sizeof(OC_APPL),(void *)IILQint(OC_APPL));
        IIexDefine(0,(char *)"abgconsf1",6929,11512);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&apid);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 423 "abgconsf.sc" */	/* host code */
	if ( FEinqerr() != OK )
	{
		IIUGerr( E_AB0310_BadObjectCat, 0, 0 );
		return ( FAIL );
	}
	langbrk[0] = EOS;
	firstrecord = TRUE;
/* # line 431 "abgconsf.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"abgconsf2",6930,10993);
      IIputdomio((short *)0,1,30,sizeof(OC_CONST),(void *)IILQint(OC_CONST));
      IIputdomio((short *)0,1,30,4,&apid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"abgconsf2",6930,10993);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select object_name, abf_source, abf_arg1, rettype from ii_objects, ii\
_abfobjects where ii_objects.object_class=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,sizeof(OC_CONST),(void *)IILQint(OC_CONST));
        IIwritio(0,(short *)0,1,32,0,(char *)
"and ii_objects.object_id=ii_abfobjects.object_id and ii_abfobjects.ap\
plid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&apid);
        IIwritio(0,(short *)0,1,32,0,(char *)
"order by ii_abfobjects.abf_arg1, object_name");
        IIexDefine(0,(char *)"abgconsf2",6930,10993);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,ccname);
      IIgetdomio((short *)0,1,32,180+1-1,ccvalue);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,cclang);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,cctype);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 440 "abgconsf.sc" */	/* host code */
	{
		/* 
		** Break the value into 28 character pieces if needed.
		*/
		if ( STcompare(langbrk,cclang) != 0 )
		{
			if (firstrecord)
			{
				firstrecord = FALSE;
			}
			else	
			{
				STprintf( ccfbuf,"\n%s %s\n",
					ERget(F_AB00EF_ConsFile_End), langbrk);
				SIputrec( ccfbuf,ccfp );
			}
			STprintf( ccfbuf,"\n%s %s",
				ERget(F_AB00EE_ConsFile_Begin), cclang);
			SIputrec( ccfbuf,ccfp );
		}
		STcopy(cclang, langbrk);
		STprintf( ccfbuf,"\n%-32s=",ccname );
		SIputrec( ccfbuf,ccfp );
		vp = ccvalue;
		i = -1;
		while ( *vp != EOS )
		{
			i++;
			/* handle the case where the ' is at the start */
			if ( *vp == '\'' )
			{
				if ( i == 0 )
				{
					SIputc( '\'',ccfp );
					SIputc( '\\',ccfp );
					i++;
				}
				else
				{
					SIputc( '\\',ccfp );
					i++;
				}
			}
			if ( i == 0 ) 
			{
				SIputc( '\'',ccfp );
				SIputc( *vp,ccfp );
			}
			else if ( i == 27 )
			{
				i = 0;
				SIputrec( "'\n                                 '",ccfp );
				SIputc( *vp,ccfp );
			}
			else
				SIputc( *vp,ccfp );
			*++vp;
		}
		SIputc( '\'',ccfp );
		/* Advance to position 65 to place constants' type name */
		i = 28 - i;
		while ( i > 0 )
		{
			SIputc( ' ',ccfp );
			i--;
		}
		STprintf( ccfbuf,"# %s",cctype );
		SIputrec( ccfbuf,ccfp );
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if ( FEinqerr() != OK )
	{
		IIUGerr( E_AB0311_AbfObjJoinFail, 0, 0 );
	  	return ( FAIL );
	}
	STprintf( ccfbuf,"\n%s %s\n", ERget(F_AB00EF_ConsFile_End), langbrk);
	SIputrec( ccfbuf,ccfp );
	STprintf( ccfbuf,"\n%s %d\n", ERget(F_AB00ED_ConsFile_Str3),
			FEinqrows() );
	SIputrec( ccfbuf,ccfp );
	if ( SIclose( ccfp ) != OK )
		return ( FAIL );
	return ( OK );
}
STATUS
IIABgceGlobalConstantEdit( char *filename )
{
	LOCATION	loc;
	LOfroms( PATH&FILENAME, filename, &loc );
        FTclear();
	FTrestore(FT_NORMAL);
	UTedit(NULL, &loc);
	FTrestore(FT_FORMS);
	FTclear();
}
