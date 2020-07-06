# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<pc.h>		 
# include	<st.h>
# include	<si.h>
# include	<cv.h>
# include	<lo.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ug.h>
# include <ooclass.h>
# include	<cf.h>
# include	"ercf.h"
/**
** Name:	cfdmpobj.qsc - routine to write the definition of a
**				form-related object to a file.
**
** Description:
**	This file defines:
**
**	cf_dmpobj	write definition of form-related object to file.
**
** History:
**	see entries for procedure cf_dmpobj below.
**	12/12/89 (dkh) - Put in changes for owner.table support.
**	15-mar-90 (griffin)
**		Turned off optimization for hp3_us5 port.
**	25-jul-91 (johnr)
**		hp3_us5 no longer needs NO_OPTIM
**	19-feb-92 (leighb) DeskTop Porting Change:
**		FEchkname has only 1 argument; deleted bogus ones.
**	30-Nov-92 (fredb)
**		Integrated porting change from 6.2 codeline. See below.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/* # define's */
/* GLOBALDEF's */
/* extern's */
/* static's */
/*{
** Name:	cf_dmpobj - write definition of form-related object(s) to
**				intermediate file.
**
** Description:
**	Write definition of qbfnames, joindefs and/or forms to an intermediate
**	file.  The type of the objects passed to the procedure to be written
**	to the file may be only one of qbfnames, joindefs and forms.  If the
**	passed objects are qbfnames, then the definition of associated
**	joindefs and forms will also be written to the file.  If the passed
**	objects are joindefs or forms, then only those objects will be written.
**
**	The intermediate file has the following format:
**		COPYFORM: Version date
**		QBFNAME: name short_remark long_remark
**			ii_qbfnames record
**		JOINDEF: name short_remark long_remark
**			ii_joindefs records
**		FORM: name short_remark long_remark
**			ii_forms record
**		FIELD:
**			ii_fields records
**		TRIM:
**			ii_trim records
**
**	The QBFNAME section appears first, one block per qbfname.
**	The JOINDEF section appears next, one block per joindef.
**	The FORM, FIELD, TRIM section appears next, one block per form.
**
** Input params:
**	inmode		indicates what the inlist contains
**				CF_QBFNAME
**				CF_JOINDEF
**				CF_FORM
**	inlist		array of ptrs to object names.
**	username	user name for the objects.
**	dba		dba of the open database.
**	filename	name of the file (for display msgs).
**
** Output params:
**	none
**
** Returns:
**	OK		all went well.	Form written out.
**	FAIL		error writing to file.
**
** Exceptions:
**	none
**
** Side Effects:
**
** History:
**	7/31/83 (ps)	written.
**	10/5/84 (agh)	Eliminated references to fldummy field in
**			  fdfields catalog.  Extended size of
**			  flvalchk to 1001.
**	06/12/85 (jrc)	Added code for new QBFMAP catalogs so that
**			old copyform files still work.	This means
**			versions (Version from GV.h) are now placedi
**			in the file.
**	10/21/85 (prs)	Fix problem with default value not being
**			stripped of blanks.
**	01/13/85 (dkh)	Fix for BUG 7819.  Placed a newline at end
**			of each iiqbfino row outputted since
**			a newline is expected when copying IN.
**	10-oct-86 (marian)	bug # 7774
**			If non-dba running copyform and form is owned by dba
**			still copy it out.
**	10/21/86 (sandyd)  Had to add a funny "where" clause to the
**			retrieve of "today", because of limitations
**			in SATURN backend.
**	27-oct-86 (bab) for bug 10667
**			Call STtrmwhite(fltitcol) to avoid problems when
**			doing the copyform back in.
**	7/Apr/1987 (rdesmond) Using new FE catalogs (ii_..), adding
**			record to ii_objects [and ii_longremarks] when
**			creating form, allowing use of 5.0 intermediate files.
**	03-nov-87  (danielt) changed all QUEL database statements to vanilla SQL
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	12/12/89 (dkh) - Added support for owner.table.
**	30-Nov-92 (fredb)
**		Integrated porting change from 6.2 codeline:
**      		25-Feb-91 (fredb)
**              		Changed use of SIopen to SIfopen for MPE/XL
**				(hp9_mpe) because we need the extra control
**				over file attributes.
*/
STATUS
cf_dmpobj(inmode, inlist, username, dba, filename)
i4		inmode;
char	*inlist[];
char	*username;
char	*dba;
char	*filename;
{
	/* internal declarations */
	char		*n_joindefs[MAX_OBJNAMES];
	char		*n_forms[MAX_OBJNAMES];
	char		**jdef_ptr;
	char		**form_ptr;
  i4 qbftype;
  char qbftype_ascii[3];
  char form[FE_MAXNAME+1];
  char joindef[FE_MAXNAME+1];
  char relowner[FE_MAXNAME + 1];
  i2 relown_null;
  char today[DATELEN];
	i4		count;			/* number of bytes written */
	i4		i;
	i4		j;
	i4		k;
	char		linebuf[1024];		/* output line buffer */
	LOCATION	l;
	LOCATION	*loc = &l;
	FILE		*fd;			/* File descriptor for output */
  i4 id;
  i2 ifrmaxcol;
  i2 ifrmaxlin;
  i2 ifrposx;
  i2 ifrposy;
  i2 ifrfldno;
  i2 ifrnsno;
  i2 ifrtrimno;
  i2 ifrversion;
  i2 ifrscrtype;
  i2 ifrscrmaxx;
  i2 ifrscrmaxy;
  i2 ifrscrdpix;
  i2 ifrscrdpiy;
  i4 ifrflags;
  i4 ifr2flags;
  i4 ifrtotflds;
  char frmaxcol[i2ascii_len];
  char frmaxlin[i2ascii_len];
  char frposx[i2ascii_len];
  char frposy[i2ascii_len];
  char frfldno[i2ascii_len];
  char frnsno[i2ascii_len];
  char frtrimno[i2ascii_len];
  char frversion[i2ascii_len];
  char frscrtype[i2ascii_len];
  char frscrmaxx[i2ascii_len];
  char frscrmaxy[i2ascii_len];
  char frscrdpix[i2ascii_len];
  char frscrdpiy[i2ascii_len];
  char frflags[i4ascii_len];
  char fr2flags[i4ascii_len];
  char frtotflds[i4ascii_len];
  char flseq[i2ascii_len];
  i2 iflseq;
  char fldname[33];
  char fldatatype[i2ascii_len];
  i2 ifldatatype;
  char fllength[i2ascii_len];
  i2 ifllength;
  char flprec[i2ascii_len];
  i2 iflprec;
  char flwidth[i2ascii_len];
  i2 iflwidth;
  char flmaxlin[i2ascii_len];
  i2 iflmaxlin;
  char flmaxcol[i2ascii_len];
  i2 iflmaxcol;
  char flposy[i2ascii_len];
  i2 iflposy;
  char flposx[i2ascii_len];
  i2 iflposx;
  char fldatawidth[i2ascii_len];
  i2 ifldatawidth;
  char fldatalin[i2ascii_len];
  i2 ifldatalin;
  char fldatacol[i2ascii_len];
  i2 ifldatacol;
  char fltitle[51];
  char fltitcol[i2ascii_len];
  i2 ifltitcol;
  char fltitlin[i2ascii_len];
  i2 ifltitlin;
  char flintrp[i2ascii_len];
  i2 iflintrp;
  char fldflags[i4ascii_len];
  i4 ifldflags;
  char fld2flags[i4ascii_len];
  i4 ifld2flags;
  char fldfont[i2ascii_len];
  i2 ifldfont;
  char fldptsz[i2ascii_len];
  i2 ifldptsz;
  char fldefault[51];
  char flformat[51];
  char flvalmsg[101];
  char flvalchk[256];
  char fltype[i2ascii_len];
  i2 ifltype;
  char flsubseq[i2ascii_len];
  i2 iflsubseq;
  char trimcol[i2ascii_len];
  i2 itrimcol;
  char trimlin[i2ascii_len];
  i2 itrimlin;
  char trimstring[151];
  char trimflags[i4ascii_len];
  i4 itrimflags;
  char trim2flags[i4ascii_len];
  i4 itrim2flags;
  char trimfont[i2ascii_len];
  i2 itrimfont;
  char trimptsz[i2ascii_len];
  i2 itrimptsz;
  char qtype[i4ascii_len];
  i4 iqtype;
  char qinfo1[FE_MAXNAME+1];
  char qinfo2[FE_MAXNAME+1];
  char qinfo3[FE_MAXNAME+1];
  char qinfo4[FE_MAXNAME+1];
  char qinfo5[FE_MAXNAME + 1];
  i2 qinf5_null;
	/* open intermediate file */
	LOfroms(PATH & FILENAME, filename, loc);
# ifdef hp9_mpe
	if (SIfopen(loc, ERx("w"), SI_TXT, 252, &fd) != OK)
# else
	if (SIopen(loc, ERx("w"), &fd) != OK)
# endif
	{
	    IIUGerr(E_CF0000_CANNOT_OPEN_FILE, 0, 1, filename);
	    FEing_exit();
	    PCexit(-1);
	}
    STcopy(UGcat_now(), today);
    /* Write file header */
    STcopy(ERx("COPYFORM:"), linebuf);
    STcat(linebuf, ERx("\t"));
    STcat(linebuf, Cfversion);
    STcat(linebuf, ERx("\t"));
    STcat(linebuf, today);
    STcat(linebuf, ERx("\n"));
    if (SIwrite(STlength(linebuf), linebuf, &count, fd) != OK)
    {
	IIUGerr(E_CF0001_CANNOT_WRITE_TO_FILE, 0, 1, filename);
	return (FAIL);
    }
	if(inmode == CF_QBFNAME)
	{
	    for (i = 0, j = 0, k = 0; inlist[i] != NULL; i++)
	    {
		/* no wildcard chars are allowed in qbfname */
		if (FEchkname(inlist[i]) != OK)		 
		{
		    IIUGerr(E_CF0016_ILLEGAL_QBFNAME, 0, 1, inlist[i]);
		}
		/* check if name object already written */
		else if (cf_inarray(inlist, i) == FALSE)
		{
		    /* Convert to lowercase for retrieval */
		    CVlower(inlist[i]);
		    if (cf_writeobj(OC_QBFNAME, inlist[i], username, dba, fd, 
		      &id) == FAIL)
		    {
			IIUGerr(E_CF0001_CANNOT_WRITE_TO_FILE, 0, 1, filename);
			return (FAIL);
		    }
		    if (id != 0)
		    {
/* # line 326 "cfdmpobj.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cfdmpobj1",6470,23991);
      IIputdomio((short *)0,1,30,4,&id);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"cfdmpobj1",6470,23991);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select relname, frname, qbftype, relowner from ii_qbfnames where obje\
ct_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"cfdmpobj1",6470,23991);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,joindef);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,form);
      IIgetdomio((short *)0,1,30,4,&qbftype);
      IIgetdomio(&relown_null,1,32,FE_MAXNAME + 1-1,relowner);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 332 "cfdmpobj.sc" */	/* host code */
			STtrmwhite(joindef);
			STtrmwhite(form);
			if (relown_null != -1)
			{
				STtrmwhite(relowner);			
			}
			else
			{
				*relowner = EOS;
			}
			STcopy(ERx("\t"), linebuf);
			STcat(linebuf, joindef);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, form);
			STcat(linebuf, ERx("\t"));
			CVla(qbftype, qbftype_ascii);
			STcat(linebuf, qbftype_ascii);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, relowner);
			STcat(linebuf, ERx("\n"));
			if (SIwrite(STlength(linebuf), linebuf, &count, fd) != OK)
			{
			    IIUGerr(E_CF0001_CANNOT_WRITE_TO_FILE, 0, 1, filename);
			    return (FAIL);
			}
			   /*
			   ** if this qbfname refers to a relation, then
			   ** set joindef name in array of joindefs
		           */
			if (qbftype == 1)
				n_joindefs[j++] = STalloc(joindef);
			/* set form name in array of qbfnames */
			n_forms[k++] = STalloc(form);
			if (!Cf_silent)
				FEmsg(ERget(S_CF0002_WROTE_QBFNAME), (bool)FALSE, 
					inlist[i], filename);
		}
		else
		{
		    IIUGerr(E_CF0005_CANNOT_FIND_QBFNAME, 0, 1, inlist[i]);
		}
	    }
	}
	n_joindefs[j] = NULL;
	n_forms[k] = NULL;
	jdef_ptr = n_joindefs;
	form_ptr = n_forms;
    }
    else
    {
	/* inlist refers to either forms or joindefs */
	jdef_ptr = inlist;
	form_ptr = inlist;
    }
    if(inmode != CF_FORM)
    {
	/* Copy joindefs to file */
	for (i = 0; jdef_ptr[i] != NULL; i++)
	{
	    /* Convert to lowercase for retrieval */
	    CVlower(jdef_ptr[i]);
	    /* no wildcard chars are allowed in name */
	    if (FEchkname(jdef_ptr[i]) != OK)		 
	    {
		IIUGerr(E_CF0018_ILLEGAL_JDEFNAME, 0, 1, jdef_ptr[i]);
	    }
	    /* check if name object already written */
	    else if (cf_inarray(jdef_ptr, i) == FALSE)
	    {
		/* check if exists in db and write hdr rec if so */
		if (cf_writeobj(OC_JOINDEF, jdef_ptr[i], username, dba, fd, 
		  &id) == FAIL)
		{
		    IIUGerr(E_CF0001_CANNOT_WRITE_TO_FILE, 0, 1, filename);
		    return (FAIL);
		}
		if (id != 0)
		{
/* # line 410 "cfdmpobj.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cfdmpobj2",6471,21244);
      IIputdomio((short *)0,1,30,4,&id);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cfdmpobj2",6471,21244);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select qtype, qinfo1, qinfo2, qinfo3, qinfo4, qinfo5 from ii_joindefs\
 where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"cfdmpobj2",6471,21244);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&iqtype);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,qinfo1);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,qinfo2);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,qinfo3);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,qinfo4);
      IIgetdomio(&qinf5_null,1,32,FE_MAXNAME + 1-1,qinfo5);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 417 "cfdmpobj.sc" */	/* host code */
			IIUGitoa(iqtype, qtype, 2);
			STtrmwhite(qtype);
			STtrmwhite(qinfo1);
			STtrmwhite(qinfo2);
			STtrmwhite(qinfo3);
			STtrmwhite(qinfo4);
			if (qinf5_null != -1)
			{
				STtrmwhite(qinfo5);
			}
			else
			{
				*qinfo5 = EOS;
			}
			STcopy(ERx("\t"), linebuf);
			STcat(linebuf, qtype);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, qinfo1);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, qinfo2);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, qinfo3);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, qinfo4);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, qinfo5);
			STcat(linebuf, ERx("\n"));
			if (SIwrite(STlength(linebuf), linebuf, &count, fd)
			  != OK)
			{
			    IIUGerr(E_CF0001_CANNOT_WRITE_TO_FILE, 0, 1, 
			      filename);
			    return (FAIL);
			}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
		    if (!Cf_silent)
			FEmsg(ERget(S_CF0003_WROTE_JOINDEF), (bool)FALSE,
			  jdef_ptr[i], filename);
		}
		else
		{
		    IIUGerr(E_CF0006_CANNOT_FIND_JOINDEF, 0, 1, jdef_ptr[i]);
		}
	    }
	}
    }
    if(inmode != CF_JOINDEF)
    {
	/* Copy forms to file */
	for (i = 0; form_ptr[i] != NULL; i++)
	{
	    CVlower(form_ptr[i]);
	    /* no wildcard chars are allowed in name */
	    if (FEchkname(form_ptr[i]) != OK)		 
	    {
		IIUGerr(E_CF0017_ILLEGAL_FORMNAME, 0, 1, form_ptr[i]);
	    }
	    /* check if name object already written */
	    else if (cf_inarray(form_ptr, i) == FALSE)
	    {
		/* write form header record, determine if form exists */
		if (cf_writeobj(OC_FORM, form_ptr[i], username, dba, fd, 
		  &id) == FAIL)
		{
		    IIUGerr(E_CF0001_CANNOT_WRITE_TO_FILE, 0, 1, filename);
		    return (FAIL);
		}
		if (id != 0)
		{
/* # line 486 "cfdmpobj.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cfdmpobj3",6472,30455);
      IIputdomio((short *)0,1,30,4,&id);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"cfdmpobj3",6472,30455);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select frmaxcol, frmaxlin, frposx, frposy, frfldno, frnsno, frtrimno,\
 frversion, frscrtype, frscrmaxx, frscrmaxy, frscrdpix, frscrdpiy, frf\
lags, fr2flags, frtotflds from ii_forms where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"cfdmpobj3",6472,30455);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&ifrmaxcol);
      IIgetdomio((short *)0,1,30,2,&ifrmaxlin);
      IIgetdomio((short *)0,1,30,2,&ifrposx);
      IIgetdomio((short *)0,1,30,2,&ifrposy);
      IIgetdomio((short *)0,1,30,2,&ifrfldno);
      IIgetdomio((short *)0,1,30,2,&ifrnsno);
      IIgetdomio((short *)0,1,30,2,&ifrtrimno);
      IIgetdomio((short *)0,1,30,2,&ifrversion);
      IIgetdomio((short *)0,1,30,2,&ifrscrtype);
      IIgetdomio((short *)0,1,30,2,&ifrscrmaxx);
      IIgetdomio((short *)0,1,30,2,&ifrscrmaxy);
      IIgetdomio((short *)0,1,30,2,&ifrscrdpix);
      IIgetdomio((short *)0,1,30,2,&ifrscrdpiy);
      IIgetdomio((short *)0,1,30,4,&ifrflags);
      IIgetdomio((short *)0,1,30,4,&ifr2flags);
      IIgetdomio((short *)0,1,30,4,&ifrtotflds);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 520 "cfdmpobj.sc" */	/* host code */
		    /* II_FORMS detail line. */
		    IIUGitoa(ifrmaxcol,frmaxcol,2); 
		    IIUGitoa(ifrmaxlin,frmaxlin,2); 
		    IIUGitoa(ifrposx,frposx, 2);
		    IIUGitoa(ifrposy,frposy, 2);
		    IIUGitoa(ifrfldno,frfldno,2);
		    IIUGitoa(ifrnsno,frnsno, 2);
		    IIUGitoa(ifrtrimno,frtrimno, 2);
		    IIUGitoa(ifrversion,frversion, 2);
		    IIUGitoa(ifrscrtype,frscrtype,2);
		    IIUGitoa(ifrscrmaxx,frscrmaxx, 2);
		    IIUGitoa(ifrscrmaxy,frscrmaxy,2);
		    IIUGitoa(ifrscrdpix,frscrdpix, 2);
		    IIUGitoa(ifrscrdpiy,frscrdpiy,2);
		    IIUGitoa(ifrflags,frflags ,4);
		    IIUGitoa(ifr2flags,fr2flags ,4);
		    IIUGitoa(ifrtotflds, frtotflds,4);
		    STtrmwhite(frmaxcol); 
		    STtrmwhite(frmaxlin); 
		    STtrmwhite(frposx); 
		    STtrmwhite(frposy); 
		    STtrmwhite(frfldno);
		    STtrmwhite(frnsno); 
		    STtrmwhite(frtrimno); 
		    STtrmwhite(frversion); 
		    STtrmwhite(frscrtype);
		    STtrmwhite(frscrmaxx); 
		    STtrmwhite(frscrmaxy); 
		    STtrmwhite(frscrdpix); 
		    STtrmwhite(frscrdpiy);
		    STtrmwhite(frflags); 
		    STtrmwhite(fr2flags); 
		    STtrmwhite(frtotflds);
		    STcopy(ERx("\t"), linebuf);
		    STcat(linebuf, frmaxcol);
		    STcat(linebuf, ERx("\t"));
		    STcat(linebuf, frmaxlin);
		    STcat(linebuf, ERx("\t"));
		    STcat(linebuf, frposx);
		    STcat(linebuf, ERx("\t"));
		    STcat(linebuf, frposy);
		    STcat(linebuf, ERx("\t"));
		    STcat(linebuf, frfldno);
		    STcat(linebuf, ERx("\t"));
		    STcat(linebuf, frnsno);
		    STcat(linebuf, ERx("\t"));
		    STcat(linebuf, frtrimno);
		    STcat(linebuf, ERx("\t"));
		    STcat(linebuf, frversion);
		    STcat(linebuf, ERx("\t"));
		    STcat(linebuf, frscrtype);
		    STcat(linebuf, ERx("\t"));
		    STcat(linebuf, frscrmaxx);
		    STcat(linebuf, ERx("\t"));
		    STcat(linebuf, frscrmaxy);
		    STcat(linebuf, ERx("\t"));
		    STcat(linebuf, frscrdpix);
		    STcat(linebuf, ERx("\t"));
		    STcat(linebuf, frscrdpiy);
		    STcat(linebuf, ERx("\t"));
		    STcat(linebuf, frflags);
		    STcat(linebuf, ERx("\t"));
		    STcat(linebuf, fr2flags);
		    STcat(linebuf, ERx("\t"));
		    STcat(linebuf, frtotflds);
		    STcat(linebuf, ERx("\n"));
		    if (SIwrite(STlength(linebuf), linebuf, &count, fd) != OK)
		    {
			IIUGerr(E_CF0001_CANNOT_WRITE_TO_FILE, 0, 1, filename);
			return (FAIL);
		    }
		    /* II_FIELDS header line. */
		    STcopy(ERx("FIELD:\n"), linebuf);
		    if (SIwrite(STlength(linebuf), linebuf, &count, fd) != OK)
		    {
			IIUGerr(E_CF0001_CANNOT_WRITE_TO_FILE, 0, 1, filename);
			return (FAIL);
		    }
		    /* II_FIELDS detail lines. */
/* # line 599 "cfdmpobj.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cfdmpobj4",6473,16561);
      IIputdomio((short *)0,1,30,4,&id);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cfdmpobj4",6473,16561);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select flseq, fldname, fldatatype, fllength, flprec, flwidth, flmaxli\
n, flmaxcol, flposy, flposx, fldatawidth, fldatalin, fldatacol, fltitl\
e, fltitcol, fltitlin, flintrp, fldflags, fld2flags, fldfont, fldptsz,\
 fldefault, flformat, flvalmsg, flvalchk, ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"fltype, flsubseq from ii_fields where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"cfdmpobj4",6473,16561);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&iflseq);
      IIgetdomio((short *)0,1,32,32,fldname);
      IIgetdomio((short *)0,1,30,2,&ifldatatype);
      IIgetdomio((short *)0,1,30,2,&ifllength);
      IIgetdomio((short *)0,1,30,2,&iflprec);
      IIgetdomio((short *)0,1,30,2,&iflwidth);
      IIgetdomio((short *)0,1,30,2,&iflmaxlin);
      IIgetdomio((short *)0,1,30,2,&iflmaxcol);
      IIgetdomio((short *)0,1,30,2,&iflposy);
      IIgetdomio((short *)0,1,30,2,&iflposx);
      IIgetdomio((short *)0,1,30,2,&ifldatawidth);
      IIgetdomio((short *)0,1,30,2,&ifldatalin);
      IIgetdomio((short *)0,1,30,2,&ifldatacol);
      IIgetdomio((short *)0,1,32,50,fltitle);
      IIgetdomio((short *)0,1,30,2,&ifltitcol);
      IIgetdomio((short *)0,1,30,2,&ifltitlin);
      IIgetdomio((short *)0,1,30,2,&iflintrp);
      IIgetdomio((short *)0,1,30,4,&ifldflags);
      IIgetdomio((short *)0,1,30,4,&ifld2flags);
      IIgetdomio((short *)0,1,30,2,&ifldfont);
      IIgetdomio((short *)0,1,30,2,&ifldptsz);
      IIgetdomio((short *)0,1,32,50,fldefault);
      IIgetdomio((short *)0,1,32,50,flformat);
      IIgetdomio((short *)0,1,32,100,flvalmsg);
      IIgetdomio((short *)0,1,32,255,flvalchk);
      IIgetdomio((short *)0,1,30,2,&ifltype);
      IIgetdomio((short *)0,1,30,2,&iflsubseq);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 658 "cfdmpobj.sc" */	/* host code */
			IIUGitoa(iflseq,flseq,2);
  			IIUGitoa(ifldatatype,fldatatype,2);
  			IIUGitoa(ifllength, fllength,2);
  			IIUGitoa(iflprec, flprec,2);
  			IIUGitoa(iflwidth, flwidth,2);
  			IIUGitoa(iflmaxlin, flmaxlin,2);
  			IIUGitoa(iflmaxcol, flmaxcol,2);
  			IIUGitoa(iflposy, flposy,2);
  			IIUGitoa(iflposx, flposx,2);
  			IIUGitoa(ifldatawidth, fldatawidth,2);
  			IIUGitoa(ifldatalin, fldatalin,2);
  			IIUGitoa(ifldatacol, fldatacol,2);
  			IIUGitoa(ifltitcol, fltitcol,2);
  			IIUGitoa(ifltitlin, fltitlin,2);
  			IIUGitoa(iflintrp, flintrp,2);
  			IIUGitoa(ifldflags, fldflags,4);
  			IIUGitoa(ifld2flags, fld2flags,4);
  			IIUGitoa(ifldfont, fldfont,2);
  			IIUGitoa(ifldptsz, fldptsz,2);
  			IIUGitoa(ifltype, fltype,2);
  			IIUGitoa(iflsubseq , flsubseq,2);
			STtrmwhite(flseq);
  			STtrmwhite(fldname); 
  			STtrmwhite(fldatatype);
  			STtrmwhite(fllength); 
  			STtrmwhite(flprec); 
  			STtrmwhite(flwidth); 
  			STtrmwhite(flmaxlin); 
  			STtrmwhite(flmaxcol); 
  			STtrmwhite(flposy); 
  			STtrmwhite(flposx); 
  			STtrmwhite(fldatawidth); 
  			STtrmwhite(fldatalin); 
  			STtrmwhite(fldatacol); 
  			STtrmwhite(fltitle); 
  			STtrmwhite(fltitcol); 
  			STtrmwhite(fltitlin); 
  			STtrmwhite(flintrp); 
  			STtrmwhite(fldflags); 
  			STtrmwhite(fld2flags); 
  			STtrmwhite(fldfont); 
  			STtrmwhite(fldptsz); 
  			STtrmwhite(fldefault); 
  			STtrmwhite(flformat); 
  			STtrmwhite(flvalmsg); 
  			STtrmwhite(flvalchk); 
  			STtrmwhite(fltype); 
  			STtrmwhite(flsubseq);
			STcopy(ERx("\t"), linebuf);
			STcat(linebuf, flseq);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, fldname);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, fldatatype);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, fllength);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, flprec);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, flwidth);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, flmaxlin);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, flmaxcol);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, flposy);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, flposx);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, fldatawidth);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, fldatalin);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, fldatacol);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, fltitle);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, fltitcol);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, fltitlin);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, flintrp);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, fldflags);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, fld2flags);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, fldfont);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, fldptsz);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, fldefault);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, flformat);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, flvalmsg);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, flvalchk);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, fltype);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, flsubseq);
			STcat(linebuf, ERx("\n"));
			if (SIwrite(STlength(linebuf), linebuf, &count, fd)
			  != OK)
			{
			    IIUGerr(E_CF0001_CANNOT_WRITE_TO_FILE, 0, 1, 
			      filename);
			    return (FAIL);
			}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
		    /* II_TRIM header line. */
		    STcopy(ERx("TRIM:\n"), linebuf);
		    if (SIwrite(STlength(linebuf), linebuf, &count, fd) != OK)
		    {
			IIUGerr(E_CF0001_CANNOT_WRITE_TO_FILE, 0, 1, filename);
			return (FAIL);
		    }
		    /* II_TRIM detail lines. */
/* # line 777 "cfdmpobj.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cfdmpobj5",6474,20901);
      IIputdomio((short *)0,1,30,4,&id);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cfdmpobj5",6474,20901);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select trim_col, trim_lin, trim_trim, trim_flags, trim2_flags, trim_f\
ont, trim_ptsz from ii_trim where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"cfdmpobj5",6474,20901);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&itrimcol);
      IIgetdomio((short *)0,1,30,2,&itrimlin);
      IIgetdomio((short *)0,1,32,150,trimstring);
      IIgetdomio((short *)0,1,30,4,&itrimflags);
      IIgetdomio((short *)0,1,30,4,&itrim2flags);
      IIgetdomio((short *)0,1,30,2,&itrimfont);
      IIgetdomio((short *)0,1,30,2,&itrimptsz);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 784 "cfdmpobj.sc" */	/* host code */
			IIUGitoa(itrimcol,trimcol,2);
			IIUGitoa(itrimlin,trimlin,2);
			IIUGitoa(itrimflags,trimflags,4);
			IIUGitoa(itrim2flags,trim2flags,4);
			IIUGitoa(itrimfont,trimfont,2);
			IIUGitoa(itrimptsz,trimptsz,2);
			STtrmwhite(trimcol);
			STtrmwhite(trimlin);
			STtrmwhite(trimstring);
			STtrmwhite(trimflags);
			STtrmwhite(trim2flags);
			STtrmwhite(trimfont);
			STtrmwhite(trimptsz);
			STcopy(ERx("\t"), linebuf);
			STcat(linebuf, trimcol);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, trimlin);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, trimstring);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, trimflags);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, trim2flags);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, trimfont);
			STcat(linebuf, ERx("\t"));
			STcat(linebuf, trimptsz);
			STcat(linebuf, ERx("\n"));
			if (SIwrite(STlength(linebuf), linebuf, &count, fd)
			  != OK)
			{
			    IIUGerr(E_CF0001_CANNOT_WRITE_TO_FILE, 0, 1, 
			      filename);
			    return (FAIL);
			}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
		    if (!Cf_silent)
			FEmsg(ERget(S_CF0004_WROTE_FORM), (bool)FALSE, 
			  form_ptr[i], filename);
		}
		else
		{
		    IIUGerr(E_CF0007_CANNOT_FIND_FORM, 0, 1, form_ptr[i]);
		}
	    }
	}
    }
    SIclose(fd);
    return (OK);
}
