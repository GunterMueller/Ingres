# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  txtfldcre.sc
**
**  Copyright (c) 2004 Ingres Corporation
*/
/*
** txtfldcre.sc
**
**	Create the fields for the table frame driver.
**
**	The fields are entered with a sequence number.  The sequence
**	number makes it easier to allocate table fields by ordering
**	the columns after the table field that the columns belong
**	to.  This routine reads the field entries in database order
**	allocating what can be allocated, and adding the structure to
**	an array which is indexed by sequence number.  After the initial
**	retrieve the structures are in sequence order in the array and
**	a second pass is made to allocate those things which depend on
**	order.
**
** History:
**  WRITTEN
**	10/21/82 (jrc)
**  RESURRECTED
**	03/22/85 (dkh)
**
**	02/20/87 (dkh) - Added support for ADTs.
**	19-jun-87 (bruceb)	Code cleanup.
**	13-jul-87 (bruceb)	Changed memory allocation to use [FM]Ereqmem.
**	08/14/87 (dkh) - ER changes.
**	08/26/87 (dkh) - Changes for 8000 series error numbers.
**	21-mar-88 (sylviap)
**		Created .qsc.
**	13-oct-88 (sylviap)
**		Code clean-up.  Deleted buffer locv.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	17-apr-90 (bruceb)
**		Lint changes; removed parameters in calls on FDwflalloc,
**		FDwtblalloc and FDwcolalloc.
**	19-feb-92 (leighb) DeskTop Porting Change:
**		IIbreak() has no arguments; deleted bogus ones.
**	12/12/92 (dkh) - Added support for edit masks.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# include	<compat.h>
# include	<st.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ft.h>
# include	<fmt.h>
# include	<adf.h>
# include	<frame.h>
# include	<frserrno.h>
# include	"fdfuncs.h"
# include	<me.h>
# include	<er.h>
FUNC_EXTERN	FLDCOL	*FDnewcol();
FUNC_EXTERN	char	*FEsalloc();
FUNC_EXTERN	ADF_CB	*FEadfcb();
FUNC_EXTERN	i4	IIFDemask();
typedef struct {
	i4	type;
	union {
		FIELD	*vfd;
		FLDCOL	*vcol;
	} var;
} FLDARR;
# define	fdarrfld	var.vfd
# define	fdarrcol	var.vcol
FDtxtfldcre(frmid, ooid, frm, num, wins)		/* FDFRCREATE: */
  char *frmid;
  i4 ooid;
FRAME	*frm;
i4	num;				/* number of field entries */
bool	wins;
{
  char name[FE_MAXNAME + 1];
  i4 maxcol;
  i4 maxlin;
  i4 posy;
  i4 posx;
  i4 flags;
  i2 seq;
  i4 datatype;
  i4 length;
  i4 width;
  i4 datawidth;
  i4 datalin;
  i4 datacol;
  char title[51];
  i4 titcol;
  i4 titlin;
  i4 intrp;
  char defval[51];
  char format[51];
  char valmsg[101];
  char valchk[401];
  i4 type;
  i4 subseq;
  i4 fld2flg;
  i4 font;
  i4 ptsz;
  i4 prec;
	char	*IICFLDS = ERx("ii_fields");
	i4	errcnt;
	char	*fmtstr;
	i4	fmtsize;
	i4	fmtlen;
	PTR	blk;
	FIELD	*tf;			/* temporary pointer to a field */
	i4	i;			/* index into several arrays */
	i4	frnsno;			/* index into non-seq fields */
	i4	frfldno;		/* index into field array */
	bool	fatal;			/* flag: any fatal errors? */
	bool    inTable = FALSE;	/* has a table field hdr been read? */
	bool	old = FALSE;
	FLDHDR	*hdr;
	FLDTYPE	*flty;
	FLDVAL	*val;
	FLDCOL	*col;
	TBLFLD	*tbl;
	i4	numcols = 0;
	FLDHDR	*thdr = NULL;
	FLDARR	*arr,
		*ap;
	/* old style */
	if (num == 0 && (frm->frnsno != 0 || frm->frfldno != 0))
	{
		num = frm->frnsno + frm->frfldno;
		old = TRUE;
	}
	else if (num == 0)	/* for bug 1498 (no fields) */
	{
		return (TRUE);
	}
	if ((arr = (FLDARR *)FEreqmem((u_i4)0, (u_i4)(num * sizeof(FLDARR)),
	    TRUE, (STATUS *)NULL)) == NULL)
	{
		return (FALSE);
	}
	i = 0;				/* initialize field counter */
	frnsno = 0;			/* initialize the non-seq counter */
	frfldno = 0;
	IIUIcnb_CatNolockBegin(IICFLDS);
/* # line 158 "txtfdcre.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select flseq, fldname, fldatatype, fllength, flwidth, flmaxlin, flmax\
col, flposy, flposx, fldatawidth, fldatalin, fldatacol, fltitle, fltit\
col, fltitlin, flintrp, fldflags, fldefault, flformat, flvalmsg, flval\
chk, fltype, fld2flags, fldfont, fldptsz, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"flprec, flsubseq from ii_fields where object_id=");
    IIputdomio((short *)0,1,30,4,&ooid);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&seq);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,name);
      IIgetdomio((short *)0,1,30,4,&datatype);
      IIgetdomio((short *)0,1,30,4,&length);
      IIgetdomio((short *)0,1,30,4,&width);
      IIgetdomio((short *)0,1,30,4,&maxlin);
      IIgetdomio((short *)0,1,30,4,&maxcol);
      IIgetdomio((short *)0,1,30,4,&posy);
      IIgetdomio((short *)0,1,30,4,&posx);
      IIgetdomio((short *)0,1,30,4,&datawidth);
      IIgetdomio((short *)0,1,30,4,&datalin);
      IIgetdomio((short *)0,1,30,4,&datacol);
      IIgetdomio((short *)0,1,32,50,title);
      IIgetdomio((short *)0,1,30,4,&titcol);
      IIgetdomio((short *)0,1,30,4,&titlin);
      IIgetdomio((short *)0,1,30,4,&intrp);
      IIgetdomio((short *)0,1,30,4,&flags);
      IIgetdomio((short *)0,1,32,50,defval);
      IIgetdomio((short *)0,1,32,50,format);
      IIgetdomio((short *)0,1,32,100,valmsg);
      IIgetdomio((short *)0,1,32,400,valchk);
      IIgetdomio((short *)0,1,30,4,&type);
      IIgetdomio((short *)0,1,30,4,&fld2flg);
      IIgetdomio((short *)0,1,30,4,&font);
      IIgetdomio((short *)0,1,30,4,&ptsz);
      IIgetdomio((short *)0,1,30,4,&prec);
      IIgetdomio((short *)0,1,30,4,&subseq);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 217 "txtfdcre.sc" */	/* host code */
		i++;
		if (old)
			ap = &arr[i];
		else
			ap = &arr[subseq];
		STtrmwhite(name);
		STtrmwhite(title);
		STtrmwhite(defval);
		STtrmwhite(format);
		STtrmwhite(valmsg);
		STtrmwhite(valchk);
		ap->type = type;
		if (type != FCOLUMN)
		{
			/* if sequence number > number of fields */
			if (seq >= frm->frfldno)
			{
			    IIFDerror(CFFLSQ, (i4)2, frmid, name);
			    IIbreak();
			    IIUIcne_CatNolockEnd(IICFLDS);
			    return (FALSE);
			}
			/* if negative sequence number */
			/*   then field is a non-seq field */
			if (seq < 0)
			{
			    /* test for non-seq overflow */
			    if (frnsno >= frm->frnsno)
			    {
				IIFDerror(CFFLNS, 2, frmid, name);
				IIbreak();
				IIUIcne_CatNolockEnd(IICFLDS);
				return(FALSE);
			    }
			    tf = frm->frnsfld[frnsno++] = FDnewfld(name,
				(i4)seq, type);
			}
			/* else valid sequence number */
			else
			{
			    frfldno++;
			    /* allocate a new frame */
			    tf = frm->frfld[seq] = FDnewfld(name, (i4)seq,
				type);
			}
			ap->fdarrfld = tf;
    		        if (type == FREGULAR)
		        {
		 	    hdr = &tf->fld_var.flregfld->flhdr;
			    flty = &tf->fld_var.flregfld->fltype;
			    val = &tf->fld_var.flregfld->flval;
		        }
		        else
		        {
		 	    tbl = tf->fld_var.fltblfld;
			    hdr = &tbl->tfhdr;
		        }
			hdr->fhdflags = flags;
			hdr->fhintrp = intrp;
			hdr->fhd2flags = fld2flg;
			hdr->fhdfont = font;
			hdr->fhdptsz = ptsz;
		}
		else
		{
			col = FDnewcol(name, (i4)seq);
			ap->fdarrcol = col;
		  	hdr = &col->flhdr;
			flty = &col->fltype;
			hdr->fhdflags = flags;
			hdr->fhd2flags = fld2flg;
			hdr->fhdfont = font;
			hdr->fhdptsz = ptsz;
		}
		hdr->fhseq = seq;	/* set field information */
		hdr->fhmaxx = maxcol;
		hdr->fhmaxy = maxlin;
		hdr->fhposx = posx;
		hdr->fhposy = posy;
		hdr->fhtitle = FEsalloc(title);
		hdr->fhtitx = titcol;
		hdr->fhtity = titlin;
		if (type == FTABLE)
		{
			tbl->tfrows = length;
			tbl->tfcols = width;
			tbl->tfwidth = datawidth;
			tbl->tfstart = datalin;
			if (FDwtblalloc(tbl) == FALSE)
			{
				IIFDerror(CFTBAL, 2, frmid, name);
				IIbreak();
				IIUIcne_CatNolockEnd(IICFLDS);
				return (FALSE);
			}
		}
		else
		{
			flty->ftdatatype = datatype;
			flty->ftwidth = width;
			flty->ftlength = length;
			flty->ftprec = prec;
			flty->ftdatax = datacol;
			flty->ftdataln = datawidth;
			flty->ftvalmsg = FEsalloc(valmsg);
			flty->ftdefault = FEsalloc(defval);
		/*
		**  Code changed for bug fix 4424.  Code now also
		**  stores away validation string even for a display
		**  only field. (dkh)
		**
		**	if (STcompare(valchk, "") == 0 || seq < 0)
		*/
			if (STcompare(valchk, ERx("")) == 0)
				flty->ftvalstr = NULL;
			else
				flty->ftvalstr = FEsalloc(valchk);
			if (type == FREGULAR)
			{
				val->fvdatay = datalin;
				/* allocate structs and window for the field */
				if(FDwflalloc(tf, wins) == FALSE)
				{
					IIFDerror(CFFLAL, 2, frmid, name);
					IIbreak();
					IIUIcne_CatNolockEnd(IICFLDS);
					return (FALSE);
				}
			}
			if (STcompare(format, ERx("")) == 0)
			{
				IIFDerror(CFFLAL, 2, frmid, name);
				IIbreak();
				IIUIcne_CatNolockEnd(IICFLDS);
				return(FALSE);
			}
			else
			{
				/*
				**  We no longer need to adjust
				**  the format with a leading '-'
				**  since the converted forms
				**  should be correct.
				*/
				fmtstr = format;
				flty->ftfmtstr = FEsalloc(fmtstr);
				fmtstr = flty->ftfmtstr;
				if ((fmt_areasize(FEadfcb(), fmtstr,
					&fmtsize) == OK) &&
					((blk = MEreqmem((u_i4)0,
					    (u_i4)fmtsize, TRUE,
					    (STATUS *)NULL)) != NULL) &&
					(fmt_setfmt(FEadfcb(), fmtstr, blk,
						&(flty->ftfmt), &fmtlen) == OK))
				{
					;
				}
				else
				{
					IIFDerror(CFFLFMT, 2, frmid, name);
					IIUIcne_CatNolockEnd(IICFLDS);
					return (FALSE);
				}
				if (wins)
				{
					(void) IIFDemask(hdr, flty);
				}
			}
		}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	IIUIcne_CatNolockEnd(IICFLDS);
	if(IItest_err())
	{
		return(FALSE);
	}
    	if (frfldno < frm->frfldno)
    	{
	    errcnt = frfldno;
	    IIFDerror(CFFLTF, 2, frmid, &errcnt);
	    return(FALSE);
    	}
    	if (frnsno < frm->frnsno)
    	{
	    errcnt = frnsno;
	    IIFDerror(CFNSTF, 2, frmid, &errcnt);
	    return(FALSE);
    	}
	/*
	** Time to make second pass.
	** This simply allocates the columns and makes sure all are present.
	*/
	for (i = 0, ap = arr; i < num; i++, ap++)
	{
		switch (ap->type)
		{
		  case FREGULAR:
		  case FTABLE:
			if (inTable)
			{
				if (tbl->tfcols != numcols)
				{
					IIFDerror(CFCOTF, 2, frmid,
						thdr->fhdname);
					return (FALSE);
				}
				inTable = FALSE;
			}
			if (ap->type == FTABLE)
			{
			 	tf = ap->fdarrfld;
    		 	 	tbl = tf->fld_var.fltblfld;
			 	thdr = &(tbl->tfhdr);
				inTable = TRUE;
				numcols = 0;
			}
			break;
		  case FCOLUMN:
			col = ap->fdarrcol;
			hdr = &(col->flhdr);
			if (!inTable)
			{
				IIFDerror(CFTBOC, 2, frmid, hdr->fhdname);
				return (FALSE);
			}
			if (!(0 <= hdr->fhseq && hdr->fhseq <= tbl->tfcols))
			{
				IIFDerror(CFCOOS, 3, frmid, thdr->fhdname,
					hdr->fhdname);
				return (FALSE);
			}
			tbl->tfflds[hdr->fhseq] = col;
			if (FDwcolalloc(tbl, col, wins) == FALSE)
			{
				IIFDerror(CFCLAL, 1, frmid);
				return (FALSE);
			}
			numcols++;
		}
	}
	/* no fatal parsing errors yet */
	fatal = FDparse(frm);
	/* if any fatal parse errors, abort */
	if (fatal)
	{
		return(FALSE);
	}
	return (TRUE);
}
