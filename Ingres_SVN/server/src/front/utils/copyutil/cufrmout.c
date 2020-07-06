# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include <ooclass.h>
# include	<cu.h>
# include	<er.h>
# include	"ercu.h"
# include	<si.h>
/**
** Name:	cufrmout.c -    Copy a form out.
**
** Description:
**	Copies a form to a copyutil file.
**
**
** History:
**	30-Jul-1987 (Joe)
**		First Written
**	10/09/87 (dkh) - Fixed jup bug 1197.
**	2-feb-1988 (Joe)
**		Took out ranges, and made queries repeats.
**	05-sep-88 (kenl)
**		Changed QUEL to SQL.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	15-Jan-2004 (kodse01)
**	    replace %ld with %d for old long nat variables.
**/
/* # define's */
/* GLOBALDEF's */
/* extern's */
/* static's */
/*{
** Name:	IICUcfoCopyFormOut	-  Copy a form to a file.
**
** Description:
**	Given a form name this copies the form's
**	definition to a copyutil file.  The layout for the fields
**	of the different form tables is given in the CURECORD
**	for the form objects.
**
** Inputs:
**	name		The name of the form to copy out.
**
**	class		The class of the form to copy out.
**			This is not used in this routine.  We copy
**			out any form we find.
**
**	notthereok	If this is TRUE, then the form not being
**			there is not an error.  Otherwise it is an
**			error.
**
**	fp		The file the output to.
**
** Outputs:
**	Returns:
**		OK if succeeded.
**		If notthereok is false and the form is not present
**		returns a failure status.
**
** History:
**	30-jul-1987 (Joe)
**		First Written
**	10/09/87 (dkh) - Fixed jup bug 1197.
**	2-feb-1988 (Joe)
**		Took out ranges, and made queries repeats.
*/
STATUS
IICUcfoCopyFormOut(name, class, notthereok, fp)
char	*name;
i4	class;
bool	notthereok;
FILE	*fp;
{
  i4 frmaxcol;
  i4 frmaxlin;
  i4 frposx;
  i4 frposy;
  i4 frfldno;
  i4 frnsno;
  i4 frtrimno;
  i4 frversion;
  i4 frscrtype;
  i4 frscrmaxx;
  i4 frscrmaxy;
  i4 frscrdpix;
  i4 frscrdpiy;
  i4 frflags;
  i4 fr2flags;
  i4 frtotflds;
  i4 flseq;
  char fldname[FE_MAXNAME+1];
  i4 fldatatype;
  i4 fllength;
  i4 flprec;
  i4 flwidth;
  i4 flmaxlin;
  i4 flmaxcol;
  i4 flposy;
  i4 flposx;
  i4 fldatawidth;
  i4 fldatalin;
  i4 fldatacol;
  char fltitle[51];
  i4 fltitcol;
  i4 fltitlin;
  i4 flintrp;
  i4 fldflags;
  i4 fld2flags;
  i4 fldfont;
  i4 fldptsz;
  char fldefault[51];
  char flformat[51];
  char flvalmsg[101];
  char flvalchk[256];
  i4 fltype;
  i4 flsubseq;
  i4 trimcol;
  i4 trimlin;
  char trimstring[151];
  i4 trimflags;
  i4 trim2flags;
  i4 trimfont;
  i4 trimptsz;
  i4 formid;
	STATUS	rval;
	/*
	** Write header records for object.
	*/
	if ((rval = cu_writeobj(OC_FORM, 0, name, fp, &formid)) != OK)
	{
	    return (notthereok ? OK : rval);
	}
	/*
	** Write out the detail lines for II_FORMS
	*/
/* # line 154 "cufrmout.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cufrmout1",6485,12361);
      IIputdomio((short *)0,1,30,4,&formid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cufrmout1",6485,12361);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select frmaxcol, frmaxlin, frposx, frposy, frfldno, frnsno, frtrimno,\
 frversion, frscrtype, frscrmaxx, frscrmaxy, frscrdpix, frscrdpiy, frf\
lags, fr2flags, frtotflds from ii_forms where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&formid);
        IIexDefine(0,(char *)"cufrmout1",6485,12361);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&frmaxcol);
      IIgetdomio((short *)0,1,30,4,&frmaxlin);
      IIgetdomio((short *)0,1,30,4,&frposx);
      IIgetdomio((short *)0,1,30,4,&frposy);
      IIgetdomio((short *)0,1,30,4,&frfldno);
      IIgetdomio((short *)0,1,30,4,&frnsno);
      IIgetdomio((short *)0,1,30,4,&frtrimno);
      IIgetdomio((short *)0,1,30,4,&frversion);
      IIgetdomio((short *)0,1,30,4,&frscrtype);
      IIgetdomio((short *)0,1,30,4,&frscrmaxx);
      IIgetdomio((short *)0,1,30,4,&frscrmaxy);
      IIgetdomio((short *)0,1,30,4,&frscrdpix);
      IIgetdomio((short *)0,1,30,4,&frscrdpiy);
      IIgetdomio((short *)0,1,30,4,&frflags);
      IIgetdomio((short *)0,1,30,4,&fr2flags);
      IIgetdomio((short *)0,1,30,4,&frtotflds);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 165 "cufrmout.sc" */	/* host code */
	    /* II_FORMS detail line. */
	    SIfprintf(fp, ERx("\t%d\t%d\t%d\t%d\t%d\t%d\t%d"),
		frmaxcol,
		frmaxlin,
		frposx,
		frposy,
		frfldno,
		frnsno,
		frtrimno);
	    SIfprintf(fp, ERx("\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n"),
		frversion,
		frscrtype,
		frscrmaxx,
		frscrmaxy,
		frscrdpix,
		frscrdpiy,
		frflags,
		fr2flags,
		frtotflds);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if (FEinqerr() != 0)
	    return FEinqerr();
	/*
	** Write out the detail lines for II_FIELDS
	*/
	cu_wrcomp(CUC_FIELD, fp);
/* # line 191 "cufrmout.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cufrmout2",6486,12760);
      IIputdomio((short *)0,1,30,4,&formid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cufrmout2",6486,12760);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select flseq, fldname, fldatatype, fllength, flprec, flwidth, flmaxli\
n, flmaxcol, flposy, flposx, fldatawidth, fldatalin, fldatacol, fltitl\
e, fltitcol, fltitlin, flintrp, fldflags, fld2flags, fldfont, fldptsz,\
 fldefault, flformat, flvalmsg, flvalchk, ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"fltype, flsubseq from ii_fields where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&formid);
        IIexDefine(0,(char *)"cufrmout2",6486,12760);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&flseq);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,fldname);
      IIgetdomio((short *)0,1,30,4,&fldatatype);
      IIgetdomio((short *)0,1,30,4,&fllength);
      IIgetdomio((short *)0,1,30,4,&flprec);
      IIgetdomio((short *)0,1,30,4,&flwidth);
      IIgetdomio((short *)0,1,30,4,&flmaxlin);
      IIgetdomio((short *)0,1,30,4,&flmaxcol);
      IIgetdomio((short *)0,1,30,4,&flposy);
      IIgetdomio((short *)0,1,30,4,&flposx);
      IIgetdomio((short *)0,1,30,4,&fldatawidth);
      IIgetdomio((short *)0,1,30,4,&fldatalin);
      IIgetdomio((short *)0,1,30,4,&fldatacol);
      IIgetdomio((short *)0,1,32,50,fltitle);
      IIgetdomio((short *)0,1,30,4,&fltitcol);
      IIgetdomio((short *)0,1,30,4,&fltitlin);
      IIgetdomio((short *)0,1,30,4,&flintrp);
      IIgetdomio((short *)0,1,30,4,&fldflags);
      IIgetdomio((short *)0,1,30,4,&fld2flags);
      IIgetdomio((short *)0,1,30,4,&fldfont);
      IIgetdomio((short *)0,1,30,4,&fldptsz);
      IIgetdomio((short *)0,1,32,50,fldefault);
      IIgetdomio((short *)0,1,32,50,flformat);
      IIgetdomio((short *)0,1,32,100,flvalmsg);
      IIgetdomio((short *)0,1,32,255,flvalchk);
      IIgetdomio((short *)0,1,30,4,&fltype);
      IIgetdomio((short *)0,1,30,4,&flsubseq);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 208 "cufrmout.sc" */	/* host code */
	    SIfprintf(fp, ERx("\t%d\t%s\t%d\t%d\t%d"),
		flseq,
		fldname,
		fldatatype,
		fllength,
		flprec);
	    SIfprintf(fp, ERx("\t%d\t%d\t%d\t%d\t%d"),
		flwidth,
		flmaxlin,
		flmaxcol,
		flposy,
		flposx);
	    SIfprintf(fp, ERx("\t%d\t%d\t%d\t%s\t%d\t%d"),
		fldatawidth,
		fldatalin,
		fldatacol,
		fltitle,
		fltitcol,
		fltitlin);
	    SIfprintf(fp, ERx("\t%d\t%d\t%d\t%d\t%d"),
		flintrp,
		fldflags,
		fld2flags,
		fldfont,
		fldptsz);
	    SIfprintf(fp, ERx("\t%s\t%s\t%s\t%s\t%d\t%d\n"),
		fldefault,
		flformat,
		flvalmsg,
		flvalchk,
		fltype,
		flsubseq);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
	if (FEinqerr() != 0)
	    return FEinqerr();
	/*
	** Write out the detail lines for II_TRIM
	*/
	cu_wrcomp(CUC_TRIM, fp);
/* # line 247 "cufrmout.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cufrmout3",6487,5447);
      IIputdomio((short *)0,1,30,4,&formid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cufrmout3",6487,5447);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select trim_col, trim_lin, trim_trim, trim_flags, trim2_flags, trim_f\
ont, trim_ptsz from ii_trim where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&formid);
        IIexDefine(0,(char *)"cufrmout3",6487,5447);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&trimcol);
      IIgetdomio((short *)0,1,30,4,&trimlin);
      IIgetdomio((short *)0,1,32,150,trimstring);
      IIgetdomio((short *)0,1,30,4,&trimflags);
      IIgetdomio((short *)0,1,30,4,&trim2flags);
      IIgetdomio((short *)0,1,30,4,&trimfont);
      IIgetdomio((short *)0,1,30,4,&trimptsz);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 256 "cufrmout.sc" */	/* host code */
	    SIfprintf(fp, ERx("\t%d\t%d\t%s\t%d\t%d\t%d\t%d\n"),
		trimcol,
		trimlin,
		trimstring,
		trimflags,
		trim2flags,
		trimfont,
		trimptsz);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
	return FEinqerr();
}
