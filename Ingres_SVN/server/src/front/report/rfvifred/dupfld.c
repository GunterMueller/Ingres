# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
/* # include's */
# include	<compat.h>
# include	<me.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	"decls.h"
# include	"vfunique.h"
# include	"seq.h"
# include	<lo.h>
# include	<st.h>
# include	<ug.h>
# include	<er.h>
# include	"ervf.h"
# include <ooclass.h>
# include <uigdata.h>
# include	<flddesc.h>
/**
** Name:	dupfld.qsc - Duplicate Field Support File.
**
** Description:
**	This file contains routines to implement the DuplicateField
**	feature in Vifred.
**
**	IIVFlfdListFld	List the set of fields defined in a form.
**	IIVFlfListFOrms	List set of forms user can access in db.
**	IIFVdfDupFld	Top level entry point for DuplicateField feature.
**
** History:
**	08/09/89 (dkh) - Initial version.
**	09/12/89 (dkh) - Changed `vflsforms.hlp' to `vflsfrms.hlp'.
**	10/01/89 (dkh) - Code cleanup.
**	10/02/89 (dkh) - Changed IIVFdfDupFld() to IIVFfdFldDup().
**	11/14/89 (dkh) - Fixed problem with using same counter in
**			 nested loops.
**	11/27/89 (dkh) - Changed menuitem "End" to "Cancel" in top level
**			 menu in IIVFfdFldDup().
**	27-nov-89 (bruceb)
**		Find no longer a visible menuitem.
**		Added Top/Bottom frskey activations.
**	12/04/89 (dkh) - Fixed a few bugs.
**	01/10/90 (dkh) - Added a ## redisplay statement to the init
**			 blocks in IIVFlfdListFld() and IIVFlfListForms().
**			 This cleans up the display in case we are displaying
**			 fewer rows than the previous display loop.
**	30-jan-90 (bruceb)
**		Changed Cancel's frskey mapping to frskey9 as per FRC ruling.
**	17-apr-90 (bruceb)
**		Lint changes; removed args in call on FDwtblalloc.
**	25-apr-90 (bruceb)
**		Converted to use listpick.
**	07/04/90 (dkh) - Put in changes so layout does not change for
**			 RBF when popups are displayed.
**	11-sep-90 (bruceb)
**		Remove calls on FTspace() as endxFrm will be up-to-date
**		for use with attribute bytes.
**	12/26/90 (dkh) - Fixed bug 35056.  The 7th parameter was missing
**			 in call to palloc().
**	25-Aug-1993 (fredv)
**		Removed the declaration of MEreqmem(). It is declared in
**			<me.h> already.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/* # define's */
# define	DUPMAXDISP	10	/* max rows to display for dup flds */
/* extern's */
FUNC_EXTERN	LOCATION	*IIUFlcfLocateForm();
FUNC_EXTERN	FIELD		*FDnewfld();
FUNC_EXTERN	char		*FEsalloc();
FUNC_EXTERN	ADF_CB		*FEadfcb();
FUNC_EXTERN	PTR		FEreqmem();
FUNC_EXTERN	FRAME		*IIfindfrm();
FUNC_EXTERN	FIELD		*FDfndfld();
FUNC_EXTERN	i4		IIFRclpCallListPick();
FUNC_EXTERN	i4		IIFRrlpRunListPick();
/* static's */
static	bool	init_dupform = TRUE;	/* boolean for initializing form */
static	i4	maxy_dupfrm = 0;
static	i4	maxy_tbl = 0;
static	FRAME	*dupfrm = NULL;
static	TBLFLD	*tbl_dupfrm = NULL;
static	FLDHDR	*tbl_hdr = NULL;
static	FIELD	*new_fld = NULL;
static	bool	created = FALSE;
static	POS	*fld_pos = NULL;
  static char Form[]= ERx("dupfldform");
  static char TFld[]= ERx("list");
# define MU_LISTFORMS	2
static	LISTMENU	lstflds_menu[] =
{
    { FE_Select, F_FE0108_ExplSelect,  LPB_CRSELECT, 4 },
    { FE_Cancel, F_FE0103_ExplCancel,  LPB_QUIT,     9 },
    { F_VF0076_Listforms,
	     F_VF0092_ExplListForms,   LPB_SELECT,   0 },
    { FE_Empty,  F_FE0104_ExplFind,    LPB_FIND,     7 },
    { FE_Empty,  F_FE0105_ExplTop,     LPB_TOP,      5 },
    { FE_Empty,  F_FE0106_ExplBottom,  LPB_BOTTOM,   6 },
    { FE_Help,   F_FE0101_ExplKeyHelp, LPB_HELP,     1 }
};
/*{
** Name:	IIVFlfdListFld - List fields in a form.
**
** Description:
**	Display a list of fields that user can select from.
**
** Inputs:
**	id	Object id of form to obtain list of fields from.
**
** Outputs:
**	None.
**	Returns:
**		None.
**	Exceptions:
**		None.
**
** Side Effects:
**	New field may be added to form being edited.
**
** History:
**	08/09/89 (dkh) - Initial version.
*/
VOID
IIVFlfdListFld(id, nm)
  i4 id;
  char *nm;
{
	i4	fldcount;
  char fname[FE_MAXNAME + 1];
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
  char title[fdTILEN + 1];
  i4 titcol;
  i4 titlin;
  i4 intrp;
  char defval[fdDEFLEN + 1];
  char format[fdFMTLEN + 1];
  char valmsg[fdVMSGLEN + 1];
  char valchk[fdVSTRLEN + 1];
  i4 type;
  i4 subseq;
  i4 fld2flg;
  i4 font;
  i4 ptsz;
  i4 prec;
  i4 seq1;
  i4 seq2;
  char msgbuf[200];
	i4	error;
	FIELD	*fld;
	FLDHDR	*hdr;
	FLDTYPE	*ftype;
	FLDVAL	*fval;
	FLDCOL	**colarr;
	FLDCOL	*acol;
	TBLFLD	*tbl;
	FLDHDR	*ahdr;
	FLDTYPE	*atype;
	FLDVAL	*aval;
	PTR	blk;
	i4	fmtsize;
	i4	fmtlen;
	POS	*ps;
	i4	pstype;
	i4	untype;
  char nmlist3[DUPMAXDISP][FE_MAXNAME + 1];
  i4 subseqlist[DUPMAXDISP];
  i4 typelist[DUPMAXDISP];
  i4 colslist[DUPMAXDISP];
	i4	listcount = 0;
  char *aname;
	i4	need_toinit = TRUE;
	i4	i;
	i4	overflow;
	char	savename[FE_MAXNAME + 1];
  i4 sel_fld;
	dupfrm->frmaxy = maxy_dupfrm;
	tbl_hdr->fhmaxy = maxy_tbl;
	tbl_dupfrm->tfrows = DUPMAXDISP;
	fldcount = 0;
	IIUIcnb_CatNolockBegin(ERx("ii_fields"));
/* # line 206 "dupfld.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"dupfld1",7351,6235);
      IIputdomio((short *)0,1,30,4,&id);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"dupfld1",7351,6235);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select fldname, flsubseq, fltype, flwidth from ii_fields where object\
_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by fldname");
        IIexDefine(0,(char *)"dupfld1",7351,6235);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,fname);
      IIgetdomio((short *)0,1,30,4,&subseq);
      IIgetdomio((short *)0,1,30,4,&type);
      IIgetdomio((short *)0,1,30,4,&width);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 211 "dupfld.sc" */	/* host code */
		if (type == FREGULAR || type == FTABLE)
		{
			if (listcount < DUPMAXDISP)
			{
				STcopy(fname, nmlist3[listcount]);
				subseqlist[listcount] = subseq;
				typelist[listcount] = type;
				colslist[listcount] = width;
			}
			else
			{
				if (need_toinit)
				{
/* # line 243 "dupfld.qsc" */	/* inittable */
  {
    if (IItbinit(Form,TFld,(char *)"r") != 0) {
      IIthidecol((char *)"hsubseq",(char *)"i4");
      IIthidecol((char *)"htype",(char *)"i4");
      IIthidecol((char *)"hcols",(char *)"i4");
      IItfill();
    } /* IItbinit */
  }
/* # line 246 "dupfld.qsc" */	/* host code */
					need_toinit = FALSE;
					for (i = 0; i < DUPMAXDISP; i++)
					{
/* # line 249 "dupfld.qsc" */	/* loadtable */
  {
    if (IItbact(Form,TFld,1) != 0) {
      IItcoputio((char *)"name",(short *)0,1,32,0,nmlist3[i]);
      IItcoputio((char *)"hsubseq",(short *)0,1,30,4,&subseqlist[i]);
      IItcoputio((char *)"htype",(short *)0,1,30,4,&typelist[i]);
      IItcoputio((char *)"hcols",(short *)0,1,30,4,&colslist[i]);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 254 "dupfld.qsc" */	/* host code */
					}
				}
/* # line 256 "dupfld.qsc" */	/* loadtable */
  {
    if (IItbact(Form,TFld,1) != 0) {
      IItcoputio((char *)"name",(short *)0,1,32,0,fname);
      IItcoputio((char *)"hsubseq",(short *)0,1,30,4,&subseq);
      IItcoputio((char *)"htype",(short *)0,1,30,4,&type);
      IItcoputio((char *)"hcols",(short *)0,1,30,4,&width);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 259 "dupfld.qsc" */	/* host code */
			}
			fldcount++;
			listcount++;
		}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	IIUIcne_CatNolockEnd(ERx("ii_fields"));
	/*
	**  If form has no fields, display error message and return.
	*/
	if (!fldcount)
	{
		STprintf(msgbuf, ERget(S_VF013C_NoFldInForm), nm);
/* # line 272 "dupfld.qsc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(msgbuf);
  }
/* # line 273 "dupfld.qsc" */	/* host code */
		return;
	}
	if (need_toinit)
	{
		dupfrm->frmaxy -= (DUPMAXDISP - listcount);
		tbl_hdr->fhmaxy -= (DUPMAXDISP - listcount);
		tbl_dupfrm->tfrows = listcount;
/* # line 281 "dupfld.qsc" */	/* inittable */
  {
    if (IItbinit(Form,TFld,(char *)"r") != 0) {
      IIthidecol((char *)"hsubseq",(char *)"i4");
      IIthidecol((char *)"htype",(char *)"i4");
      IIthidecol((char *)"hcols",(char *)"i4");
      IItfill();
    } /* IItbinit */
  }
/* # line 283 "dupfld.qsc" */	/* host code */
		for (i = 0; i < listcount; i++)
		{
			aname = nmlist3[i];
			subseq = subseqlist[i];
			type = typelist[i];
			width = colslist[i];
/* # line 289 "dupfld.qsc" */	/* loadtable */
  {
    if (IItbact(Form,TFld,1) != 0) {
      IItcoputio((char *)"name",(short *)0,1,32,0,aname);
      IItcoputio((char *)"hsubseq",(short *)0,1,30,4,&subseq);
      IItcoputio((char *)"htype",(short *)0,1,30,4,&type);
      IItcoputio((char *)"hcols",(short *)0,1,30,4,&width);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 291 "dupfld.qsc" */	/* host code */
		}
	}
/* # line 294 "dupfld.qsc" */	/* putform */
  {
    if (IIfsetio(Form) != 0) {
      IIputfldio((char *)"title1",(short *)0,1,32,0,ERget(F_VF0078_FldsIn));
      IIputfldio((char *)"title2",(short *)0,1,32,0,nm);
    } /* IIfsetio */
  }
/* # line 295 "dupfld.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 297 "dupfld.qsc" */	/* host code */
	sel_fld = IIFRrlpRunListPick(Form, TFld, LPGR_CENTER, LPGR_CENTER,
	    ERget(S_VF013D_Dupfromdb), ERx("vflsflds.hlp"), NULL, NULL);
	if (++sel_fld > 0)
	{
/* # line 302 "dupfld.qsc" */	/* scroll */
  {
    if (IItbsetio(1,Form,TFld,-3) != 0) {
      IItbsmode((char *)"to");
      if (IItscroll(0,sel_fld) != 0) {
      } /* IItscroll */
    } /* IItbsetio */
  }
/* # line 303 "dupfld.qsc" */	/* getrow */
  {
    if (IItbsetio(2,Form,TFld,-2) != 0) {
      IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,fname,(char *)"name");
      IItcogetio((short *)0,1,30,4,&subseq,(char *)"hsubseq");
      IItcogetio((short *)0,1,30,4,&type,(char *)"htype");
      IItcogetio((short *)0,1,30,4,&width,(char *)"hcols");
      IITBceColEnd();
    } /* IItbsetio */
  }
/* # line 305 "dupfld.qsc" */	/* host code */
	    /*
	    **  Save away field name for error messages.
	    */
	    STcopy(fname, savename);
/* # line 310 "dupfld.qsc" */	/* message */
  {
    IImessage(ERget(F_VF0079_Crfld));
  }
/* # line 311 "dupfld.qsc" */	/* host code */
	    /*
	    **  Now get field out of DB and create it.
	    */
	    fldcount = 0;
	    error = FALSE;
	    fld = FDnewfld(fname, 0, type);
	    if (type == FTABLE)
	    {
		    tbl = fld->fld_var.fltblfld;
		    if ((colarr = (FLDCOL **) FEreqmem(0,
			    (width * sizeof(FLDCOL *)), TRUE,
			    (STATUS *) NULL)) == NULL)
		    {
			   IIUGbmaBadMemoryAllocation(ERx("IIVFlfd"));
		    }
		    tbl->tfflds = colarr;
		    /*
		    **  Calculate seq1 and seq2
		    */
		    seq1 = subseq;
		    seq2 = subseq + width;
		    hdr = &(tbl->tfhdr);
		    ftype = NULL;
		    fval = NULL;
	    }
	    else
	    {
		    seq1 = seq2 = subseq;
		    hdr = &(fld->fld_var.flregfld->flhdr);
		    ftype = &(fld->fld_var.flregfld->fltype);
		    fval = &(fld->fld_var.flregfld->flval);
	    }
	    fldcount = 0;
/* # line 394 "dupfld.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"dupfld2",7352,10358);
      IIputdomio((short *)0,1,30,4,&id);
      IIputdomio((short *)0,1,30,4,&seq1);
      IIputdomio((short *)0,1,30,4,&seq2);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"dupfld2",7352,10358);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select flseq, fldname, fldatatype, fllength, flwidth, flmaxlin, flmax\
col, flposy, flposx, fldatawidth, fldatalin, fldatacol, fltitle, fltit\
col, fltitlin, flintrp, fldflags, fldefault, flformat, flvalmsg, flval\
chk, fltype, fld2flags, fldfont, fldptsz, ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"flprec, flsubseq from ii_fields where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIwritio(0,(short *)0,1,32,0,(char *)"and flsubseq>=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&seq1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and flsubseq<=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&seq2);
        IIexDefine(0,(char *)"dupfld2",7352,10358);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&seq);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,fname);
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
      IIgetdomio((short *)0,1,32,fdTILEN + 1-1,title);
      IIgetdomio((short *)0,1,30,4,&titcol);
      IIgetdomio((short *)0,1,30,4,&titlin);
      IIgetdomio((short *)0,1,30,4,&intrp);
      IIgetdomio((short *)0,1,30,4,&flags);
      IIgetdomio((short *)0,1,32,fdDEFLEN + 1-1,defval);
      IIgetdomio((short *)0,1,32,fdFMTLEN + 1-1,format);
      IIgetdomio((short *)0,1,32,fdVMSGLEN + 1-1,valmsg);
      IIgetdomio((short *)0,1,32,fdVSTRLEN + 1-1,valchk);
      IIgetdomio((short *)0,1,30,4,&type);
      IIgetdomio((short *)0,1,30,4,&fld2flg);
      IIgetdomio((short *)0,1,30,4,&font);
      IIgetdomio((short *)0,1,30,4,&ptsz);
      IIgetdomio((short *)0,1,30,4,&prec);
      IIgetdomio((short *)0,1,30,4,&subseq);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 411 "dupfld.sc" */	/* host code */
		    if (fldcount && seq1 == seq2)
		    {
			    /*
			    **  Too many rows for a simple field
			    **  retrieval.
			    */
			    IIUGerr(E_VF013E_TooManyRows, UG_ERR_ERROR,
				    2, savename, nm);
			    error = TRUE;
/* # line 420 "dupfld.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE2;
      }
/* # line 421 "dupfld.sc" */	/* host code */
		    }
		    /*
		    **  If this is the row defining the field
		    **  then set up the fields.
		    */
		    if (type != FCOLUMN)
		    {
			    ahdr = hdr;
			    atype = ftype;	/* meaningless for tf */
			    aval = fval;	/* meaningless for tf */
		    }
		    else
		    {
			    acol = FDnewcol(fname, seq);
			    colarr[seq] = acol;
			    ahdr = &(acol->flhdr);
			    atype = &(acol->fltype);
		    }
		    ahdr->fhdflags = flags;
		    ahdr->fhintrp = intrp;
		    ahdr->fhd2flags = (fld2flg & ~fdVQLOCK);
		    ahdr->fhdfont = font;
		    ahdr->fhdptsz = ptsz;
		    ahdr->fhseq = seq;
		    ahdr->fhmaxx = maxcol;
		    ahdr->fhmaxy = maxlin;
		    ahdr->fhposx = posx;
		    ahdr->fhposy = posy;
		    ahdr->fhtitle = FEsalloc(title);
		    ahdr->fhtitx = titcol;
		    ahdr->fhtity = titlin;
		    if (type == FTABLE)
		    {
			    tbl->tfrows = length;
			    tbl->tfcols = width;
			    tbl->tfwidth = datawidth;
			    tbl->tfstart = datalin;
		    }
		    else
		    {
			    atype->ftdatatype = datatype;
			    atype->ftwidth = width;
			    atype->ftlength = length;
			    atype->ftprec = prec;
			    atype->ftdatax = datacol;
			    atype->ftdataln = datawidth;
			    atype->ftvalmsg = FEsalloc(valmsg);
			    atype->ftdefault = FEsalloc(defval);
			    atype->ftvalstr = FEsalloc(valchk);
			    if (type == FREGULAR)
			    {
				    aval->fvdatay = datalin;
			    }
			    atype->ftfmtstr = FEsalloc(format);
			    /*
			    **  Now set the FMT structure.  Should
			    **  have no errors.
			    */
			    (VOID) fmt_areasize(FEadfcb(), format,&fmtsize);
			    if ((blk = MEreqmem(0, fmtsize, TRUE,
				    (STATUS *)NULL)) == NULL)
			    {
				    IIUGbmaBadMemoryAllocation(ERx("ld"));
			    }
			    (VOID) fmt_setfmt(FEadfcb(), format, blk,
				    &(atype->ftfmt), &fmtlen);
		    }
		    fldcount++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
	    if (error)
	    {
		    /*
		    **  Retrieval error already outputted to user.
		    **  So just return.
		    */
		    return;
	    }
	    /*
	    **  Couldn't find all the pieces of a
	    **  table field.
	    */
	    if (fldcount != seq2 - seq1 + 1)
	    {
		    IIUGerr(E_VF013F_TFRetErr, UG_ERR_ERROR, 2,
			    savename, nm);
		    return;
	    }
	    /*
	    **  Now complete the rest of the table field.
	    */
	    if (fld->fltag == FTABLE)
	    {
		    if ((tbl->tfwins  = (FLDVAL *)FEreqmem(0,
			    (sizeof(FLDVAL) * tbl->tfcols * tbl->tfrows),
			    TRUE, (STATUS *) NULL)) == NULL)
		    {
			    IIUGbmaBadMemoryAllocation(ERx("IIVFldf2"));
		    }
		    if ((tbl->tffflags = (i4 *)FEreqmem(0,
			    (sizeof(i4) * tbl->tfcols * tbl->tfrows),
			    TRUE, (STATUS *) NULL)) == NULL)
		    {
			    IIUGbmaBadMemoryAllocation(ERx("IIVFldf2"));
		    }
	    }
	    /*
	    **  Check for name uniqueness.
	    */
	    if (vfnmunique(hdr->fhdname, TRUE, AFIELD) == FOUND)
	    {
		    for (;;)
		    {
/* # line 491 "dupfld.qsc" */	/* prompt */
  {
    IIprmptio(0,ERget(F_VF0077_NewFldName),(short *)0,1,32,199,msgbuf);
  }
/* # line 492 "dupfld.qsc" */	/* host code */
			    if (!vffnmchk(msgbuf, ERget(F_VF0011_field)))
			    {
				    continue;
			    }
			    if (vfnmunique(msgbuf, TRUE, AFIELD) == FOUND)
			    {
				    IIUGerr(E_VF0052_name, UG_ERR_ERROR,
					    1, msgbuf);
			    }
			    else
			    {
				    STcopy(msgbuf, hdr->fhdname);
				    break;
			    }
		    }
	    }
	    /*
	    **  Put new field into Vifred structures.
	    **  We are of course assuming that what we
	    **  get out of the DB is correct.
	    */
	    hdr->fhposy = globy;
	    hdr->fhposx = globx;
	    if ((overflow = globx + hdr->fhmaxx - 1 - endxFrm) > 0)
	    {
		    vfwider(frame, overflow);
	    }
	    if (fld->fltag == FREGULAR)
	    {
		    pstype = PFIELD;
		    untype = enFIELD;
	    }
	    else
	    {
		    pstype = PTABLE;
		    untype = enTABLE;
	    }
	    hdr->fhseq = NEWFLDSEQ;
	    fld_pos = ps = palloc(pstype, globy, globx, hdr->fhmaxy,
		    hdr->fhmaxx, (i4 *)fld, IIVFsfaSpaceForAttr);
	    setGlobUn(untype, (POS *)NULL, NULL);
	    fitPos(ps, TRUE);
	    insPos(ps, TRUE);
	    created = TRUE;
	    new_fld = fld;
	}
}
/*{
** Name:	IIVFlfListForms - List forms from DB.
**
** Description:
**	List forms that user has accessed to in the DB.
**
** Inputs:
**	None.
**
** Outputs:
**	None.
**	Returns:
**		None.
**	Exceptions:
**		None.
**
** Side Effects:
**	New field may be added to form being edited.
**
** History:
**	08/09/89 (dkh) - Initial version.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
*/
VOID
IIVFlfListForms()
{
  char *formname;
  i4 formid;
  i4 objclass;
  char owner[FE_MAXNAME + 1];
  UIDBDATA *uidbdata;
  char *loadname;
  i4 loadid;
	char	*names[2];
	char	*nameptr;
	char	buf1[FE_MAXNAME + 1];
	char	buf2[FE_MAXNAME + 1];
	i4	id1;
	i4	formcount;
	i4	userfirst;
	i4	state;
  char nmlist2[DUPMAXDISP][FE_MAXNAME + 1];
  i4 idlist2[DUPMAXDISP];
	i4	listcount = 0;
	i4	need_toinit = TRUE;
	i4	i;
  i4 sel_frm;
	dupfrm->frmaxy = maxy_dupfrm;
	tbl_hdr->fhmaxy = maxy_tbl;
	tbl_dupfrm->tfrows = DUPMAXDISP;
	objclass = OC_FORM;
	names[0] = buf1;
	names[1] = buf2;
	formcount = 0;
	uidbdata = IIUIdbdata();
	userfirst = (STcompare(uidbdata->user, uidbdata->dba) <= 0 ? 1 : 0);
	state = 0;
	formname = names[0];
	IIUIcnb_CatNolockBegin(ERx("ii_objects"));
/* # line 648 "dupfld.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"dupfld3",7353,19791);
      IIputdomio((short *)0,1,30,4,&objclass);
      IIputdomio((short *)0,1,32,0,uidbdata->user);
      IIputdomio((short *)0,1,32,0,uidbdata->dba);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"dupfld3",7353,19791);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select object_id, object_name, object_owner from ii_objects where obj\
ect_class=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&objclass);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,uidbdata->user);
        IIwritio(0,(short *)0,1,32,0,(char *)"or object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,uidbdata->dba);
        IIwritio(0,(short *)0,1,32,0,(char *)
")order by object_name, object_owner");
        IIexDefine(0,(char *)"dupfld3",7353,19791);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&formid);
      IIgetdomio((short *)0,1,32,0,formname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,owner);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 655 "dupfld.sc" */	/* host code */
		if (state == 0)
		{
			formname = names[1];
			id1 = formid;
			state++;
		}
		else
		{
			if (STequal(names[0], names[1]))
			{
				if (userfirst)
				{
					loadname = names[0];
					loadid = id1;
				}
				else
				{
					loadname = names[1];
					loadid = formid;
				}
				formname = names[0];
				state = 0;
			}
			else
			{
				loadname = nameptr = formname = names[0];
				loadid = id1;
				names[0] = names[1];
				names[1] = nameptr;
				id1 = formid;
			}
			if (listcount < DUPMAXDISP)
			{
				STcopy(loadname, nmlist2[listcount]);
				idlist2[listcount] = loadid;
			}
			else
			{
				if (need_toinit)
				{
/* # line 658 "dupfld.qsc" */	/* inittable */
  {
    if (IItbinit(Form,TFld,(char *)"r") != 0) {
      IIthidecol((char *)"id",(char *)"i4");
      IItfill();
    } /* IItbinit */
  }
/* # line 659 "dupfld.qsc" */	/* host code */
					need_toinit = FALSE;
					for (i = 0; i < DUPMAXDISP; i++)
					{
/* # line 662 "dupfld.qsc" */	/* loadtable */
  {
    if (IItbact(Form,TFld,1) != 0) {
      IItcoputio((char *)"name",(short *)0,1,32,0,nmlist2[i]);
      IItcoputio((char *)"id",(short *)0,1,30,4,&idlist2[i]);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 665 "dupfld.qsc" */	/* host code */
					}
				}
/* # line 667 "dupfld.qsc" */	/* loadtable */
  {
    if (IItbact(Form,TFld,1) != 0) {
      IItcoputio((char *)"name",(short *)0,1,32,0,loadname);
      IItcoputio((char *)"id",(short *)0,1,30,4,&loadid);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 669 "dupfld.qsc" */	/* host code */
			}
			formcount++;
			listcount++;
		}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
	IIUIcne_CatNolockEnd(ERx("ii_objects"));
	if (state)
	{
		loadname = names[0];
		loadid = id1;
		if (listcount < DUPMAXDISP)
		{
			STcopy(loadname, nmlist2[listcount]);
			idlist2[listcount] = loadid;
		}
		else
		{
			if (need_toinit)
			{
/* # line 688 "dupfld.qsc" */	/* inittable */
  {
    if (IItbinit(Form,TFld,(char *)"r") != 0) {
      IIthidecol((char *)"id",(char *)"i4");
      IItfill();
    } /* IItbinit */
  }
/* # line 689 "dupfld.qsc" */	/* host code */
				need_toinit = FALSE;
				for (i = 0; i < DUPMAXDISP; i++)
				{
/* # line 692 "dupfld.qsc" */	/* loadtable */
  {
    if (IItbact(Form,TFld,1) != 0) {
      IItcoputio((char *)"name",(short *)0,1,32,0,nmlist2[i]);
      IItcoputio((char *)"id",(short *)0,1,30,4,&idlist2[i]);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 695 "dupfld.qsc" */	/* host code */
				}
			}
/* # line 697 "dupfld.qsc" */	/* loadtable */
  {
    if (IItbact(Form,TFld,1) != 0) {
      IItcoputio((char *)"name",(short *)0,1,32,0,loadname);
      IItcoputio((char *)"id",(short *)0,1,30,4,&loadid);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 698 "dupfld.qsc" */	/* host code */
		}
		listcount++;
		formcount++;
	}
	if (need_toinit)
	{
		dupfrm->frmaxy -= (DUPMAXDISP - listcount);
		tbl_hdr->fhmaxy -= (DUPMAXDISP - listcount);
		tbl_dupfrm->tfrows = listcount;
/* # line 708 "dupfld.qsc" */	/* inittable */
  {
    if (IItbinit(Form,TFld,(char *)"r") != 0) {
      IIthidecol((char *)"id",(char *)"i4");
      IItfill();
    } /* IItbinit */
  }
/* # line 709 "dupfld.qsc" */	/* host code */
		for (i = 0; i < listcount; i++)
		{
			loadname = nmlist2[i];
			loadid = idlist2[i];
/* # line 713 "dupfld.qsc" */	/* loadtable */
  {
    if (IItbact(Form,TFld,1) != 0) {
      IItcoputio((char *)"name",(short *)0,1,32,0,loadname);
      IItcoputio((char *)"id",(short *)0,1,30,4,&loadid);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 714 "dupfld.qsc" */	/* host code */
		}
	}
	/*
	**  If user has no forms that he can access, display error
	**  message and return.
	*/
	if (!formcount)
	{
		IIUGerr(E_VF0140_NoForms, UG_ERR_ERROR, 0);
		return;
	}
/* # line 727 "dupfld.qsc" */	/* putform */
  {
    if (IIfsetio(Form) != 0) {
      IIputfldio((char *)"title1",(short *)0,1,32,0,ERget(F_VF007A_FormsIn));
      IIputfldio((char *)"title2",(short *)0,1,32,0,ERget(F_VF007B_DB));
    } /* IIfsetio */
  }
/* # line 729 "dupfld.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 731 "dupfld.qsc" */	/* host code */
	sel_frm = IIFRrlpRunListPick(Form, TFld, LPGR_CENTER, LPGR_CENTER,
	    ERget(S_VF0141_LsForms), ERx("vflsfrms.hlp"), NULL, NULL);
	if (++sel_frm > 0)
	{
/* # line 736 "dupfld.qsc" */	/* scroll */
  {
    if (IItbsetio(1,Form,TFld,-3) != 0) {
      IItbsmode((char *)"to");
      if (IItscroll(0,sel_frm) != 0) {
      } /* IItscroll */
    } /* IItbsetio */
  }
/* # line 737 "dupfld.qsc" */	/* getrow */
  {
    if (IItbsetio(2,Form,TFld,-2) != 0) {
      IItcogetio((short *)0,1,32,0,loadname,(char *)"name");
      IItcogetio((short *)0,1,30,4,&loadid,(char *)"id");
      IITBceColEnd();
    } /* IItbsetio */
  }
/* # line 738 "dupfld.qsc" */	/* message */
  {
    IImessage(ERget(F_VF007C_RetFlds));
  }
/* # line 739 "dupfld.qsc" */	/* host code */
		IIVFlfdListFld(loadid, loadname);
	}
}
/*{
** Name:	IIVFfdFldDup - Entry point for DuplicateField feature.
**
** Description:
**	This routine is the top level entry point routine for
**	supporting the DuplicateField feature of Vifred.
**
** Inputs:
**	None.
**
** Outputs:
**	None.
**
**	Returns:
**		None.
**	Exceptions:
**		None.
**
** Side Effects:
**	A new field may actually be created.
**
** History:
**	08/09/89 (dkh) - Initial version.
*/
VOID
IIVFfdFldDup()
{
	i4	i;
	i4	j;
	VFNODE	**lp;
	VFNODE	*lt;
	POS	*ps;
	FLDHDR	*hdr;
	FLDTYPE	*type;
	FLDVAL	*val;
	FLDHDR	*nhdr;
	FLDTYPE	*ntype;
	FLDVAL	*nval;
	TBLFLD	*tbl;
	TBLFLD	*ntbl;
	FLDCOL	*col;
	FLDCOL	*ncol;
	FLDHDR	*chdr;
	FLDHDR	*nchdr;
	FIELD	*fd;
	FIELD	*nfd;
  i4 fldptr;
  char *fdname;
  char buf[200];
  i4 sel_row;
	i4	mitem;
	PTR	blk;
	i4	fmtsize;
	i4	fmtlen;
	i4	pstype;
	i4	untype;
  char nmlist1[DUPMAXDISP][FE_MAXNAME + 1];
  i4 idlist1[DUPMAXDISP];
	i4	listcount = 0;
	i4	need_toinit = TRUE;
	i4	overflow;
	bool	dummy;
	LISTPICK	lpck;
	/*
	**  Initialize form if it has not already been done.
	*/
	if (init_dupform)
	{
		FDsetparse(FALSE);
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), Form))
		{
			/*
			**  never returns.
			*/
			syserr(ERget(S_VF0001_Can_not_access_form), Form);
		}
		FDsetparse(TRUE);
		init_dupform = FALSE;
		dupfrm = IIfindfrm(Form);
		maxy_dupfrm = dupfrm->frmaxy;
		fd = FDfndfld(dupfrm, TFld, &dummy);
		tbl_dupfrm = fd->fld_var.fltblfld;
		tbl_hdr = &(tbl_dupfrm->tfhdr);
		maxy_tbl = tbl_hdr->fhmaxy;
	}
	else
	{
		dupfrm->frmaxy = maxy_dupfrm;
		tbl_hdr->fhmaxy = maxy_tbl;
		tbl_dupfrm->tfrows = DUPMAXDISP;
	}
	IIVTlf(frame, TRUE, FALSE);
	created = FALSE;
	new_fld = NULL;
	/*
	**  Find list of fields in current form, if any.
	*/
	if (frame->frfldno > 0)
	{
		lp = line;
		for (i = 0; i < endFrame; i++, lp++)
		{
			for (lt = *lp; lt != NNIL; lt = vflnNext(lt, i))
			{
				if (lt->nd_pos->ps_begy != i)
				{
					continue;
				}
				ps = lt->nd_pos;
				switch (ps->ps_name)
				{
				    case PFIELD:
					fd = (FIELD *) ps->ps_feat;
					hdr = FDgethdr(fd);
					break;
				    case PTABLE:
					fd = (FIELD *) ps->ps_feat;
					hdr = &(fd->fld_var.fltblfld->tfhdr);
					break;
				    default:
					continue;
				}
				if (listcount < DUPMAXDISP)
				{
					STcopy(hdr->fhdname,
						nmlist1[listcount]);
					idlist1[listcount] = (i4) fd;
				}
				else
				{
					if (need_toinit)
					{
/* # line 884 "dupfld.qsc" */	/* inittable */
  {
    if (IItbinit(Form,TFld,(char *)"r") != 0) {
      IIthidecol((char *)"id",(char *)"i4");
      IItfill();
    } /* IItbinit */
  }
/* # line 886 "dupfld.qsc" */	/* host code */
						need_toinit = FALSE;
						for (j = 0; j < DUPMAXDISP; j++)
						{
/* # line 889 "dupfld.qsc" */	/* loadtable */
  {
    if (IItbact(Form,TFld,1) != 0) {
      IItcoputio((char *)"name",(short *)0,1,32,0,nmlist1[j]);
      IItcoputio((char *)"id",(short *)0,1,30,4,&idlist1[j]);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 892 "dupfld.qsc" */	/* host code */
						}
					}
					fldptr = (i4) fd;
					fdname = hdr->fhdname;
/* # line 896 "dupfld.qsc" */	/* loadtable */
  {
    if (IItbact(Form,TFld,1) != 0) {
      IItcoputio((char *)"name",(short *)0,1,32,0,fdname);
      IItcoputio((char *)"id",(short *)0,1,30,4,&fldptr);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 898 "dupfld.qsc" */	/* host code */
				}
				listcount++;
			}
		}
		if (need_toinit)
		{
			dupfrm->frmaxy -= (DUPMAXDISP - listcount);
			tbl_hdr->fhmaxy -= (DUPMAXDISP - listcount);
			tbl_dupfrm->tfrows = listcount;
/* # line 908 "dupfld.qsc" */	/* inittable */
  {
    if (IItbinit(Form,TFld,(char *)"r") != 0) {
      IIthidecol((char *)"id",(char *)"i4");
      IItfill();
    } /* IItbinit */
  }
/* # line 909 "dupfld.qsc" */	/* host code */
			for (i = 0; i < listcount; i++)
			{
				fldptr = idlist1[i];
				fdname = nmlist1[i];
/* # line 913 "dupfld.qsc" */	/* loadtable */
  {
    if (IItbact(Form,TFld,1) != 0) {
      IItcoputio((char *)"name",(short *)0,1,32,0,fdname);
      IItcoputio((char *)"id",(short *)0,1,30,4,&fldptr);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 915 "dupfld.qsc" */	/* host code */
			}
		}
		lpck.form = Form;
		lpck.tf = TFld;
		lpck.srow = LPGR_CENTER;
		lpck.scol = LPGR_CENTER;
		lpck.lineno = 0;
		lpck.hfile = ERx("vfdupfld.hlp");
		lpck.htitle = ERget(S_VF013B_DupHelp);
		lpck.nomenitems = (sizeof(lstflds_menu)/sizeof(LISTMENU));
		lpck.menuitems = lstflds_menu;
		lpck.handler = NULL;
		lpck.data = NULL;
		lpck.flags = 0;
/* # line 931 "dupfld.qsc" */	/* putform */
  {
    if (IIfsetio(Form) != 0) {
      IIputfldio((char *)"title1",(short *)0,1,32,0,ERget(F_VF0078_FldsIn));
      IIputfldio((char *)"title2",(short *)0,1,32,0,ERget(F_VF007D_CurForm));
    } /* IIfsetio */
  }
/* # line 934 "dupfld.qsc" */	/* host code */
		sel_row = IIFRclpCallListPick(&lpck, &mitem);
		if (mitem == MU_LISTFORMS)
		{
		    /*
		    **  Grab list of forms from db.
		    */
/* # line 941 "dupfld.qsc" */	/* message */
  {
    IImessage(ERget(F_VF007E_RetForms));
  }
/* # line 942 "dupfld.qsc" */	/* host code */
		    IIVFlfListForms();
		}
		else if (++sel_row > 0)
		{
/* # line 946 "dupfld.qsc" */	/* scroll */
  {
    if (IItbsetio(1,Form,TFld,-3) != 0) {
      IItbsmode((char *)"to");
      if (IItscroll(0,sel_row) != 0) {
      } /* IItscroll */
    } /* IItbsetio */
  }
/* # line 947 "dupfld.qsc" */	/* getrow */
  {
    if (IItbsetio(2,Form,TFld,-2) != 0) {
      IItcogetio((short *)0,1,32,199,buf,(char *)"name");
      IItcogetio((short *)0,1,30,4,&fldptr,(char *)"id");
      IITBceColEnd();
    } /* IItbsetio */
  }
/* # line 948 "dupfld.qsc" */	/* host code */
		    fdname = buf;
		    fd = (FIELD *) fldptr;
		    for (;;)
		    {
/* # line 952 "dupfld.qsc" */	/* prompt */
  {
    IIprmptio(0,ERget(F_VF0077_NewFldName),(short *)0,1,32,199,buf);
  }
/* # line 953 "dupfld.qsc" */	/* host code */
			    /*
			    **  Check new name to make sure its
			    **  unique.
			    */
			    if (!vffnmchk(buf, ERget(F_VF0011_field)))
			    {
				    continue;
			    }
			    if (vfnmunique(buf, TRUE, AFIELD) == FOUND)
			    {
				    IIUGerr(E_VF0052_name,
					    UG_ERR_ERROR, 1, buf);
				    continue;
			    }
			    break;
		    }
		    /*
		    **  Now create new field and put it into
		    **  the node structure, etc.
		    */
		    nfd = FDnewfld(buf, 0, fd->fltag);
		    if (nfd->fltag == FREGULAR)
		    {
			    hdr = &(fd->fld_var.flregfld->flhdr);
			    type = &(fd->fld_var.flregfld->fltype);
			    val = &(fd->fld_var.flregfld->flval);
			    nhdr = &(nfd->fld_var.flregfld->flhdr);
			    ntype = &(nfd->fld_var.flregfld->fltype);
			    nval = &(nfd->fld_var.flregfld->flval);
			    MEcopy((PTR)hdr, sizeof(FLDHDR), (PTR)nhdr);
			    MEcopy((PTR)type, sizeof(FLDTYPE), (PTR)ntype);
			    MEcopy((PTR)val, sizeof(FLDVAL), (PTR)nval);
			    nhdr->fhd2flags &= ~fdVQLOCK;
			    nhdr->fhtitle = FEsalloc(hdr->fhtitle);
			    ntype->ftvalmsg = FEsalloc(type->ftvalmsg);
			    ntype->ftdefault = FEsalloc(type->ftdefault);
			    if (type->ftvalstr)
			    {
				ntype->ftvalstr = FEsalloc(type->ftvalstr);
			    }
			    ntype->ftfmtstr = FEsalloc(type->ftfmtstr);
			    /*
			    **  Now set the fmt structures.  Should have
			    **  no errors.
			    */
			    (VOID) fmt_areasize(FEadfcb(), ntype->ftfmtstr,
				    &fmtsize);
			    if ((blk = MEreqmem(0, fmtsize, TRUE,
				    (STATUS *)NULL)) == NULL)
			    {
				    IIUGbmaBadMemoryAllocation(ERx("lf"));
			    }
			    (VOID) fmt_setfmt(FEadfcb(), ntype->ftfmtstr,
				    blk, &(ntype->ftfmt), &fmtlen);
		    }
		    else
		    {
			    tbl = fd->fld_var.fltblfld;
			    ntbl = nfd->fld_var.fltblfld;
			    hdr = &(fd->fld_var.fltblfld->tfhdr);
			    nhdr = &(nfd->fld_var.fltblfld->tfhdr);
			    MEcopy((PTR)tbl, sizeof(TBLFLD), (PTR)ntbl);
			    nhdr->fhd2flags &= ~fdVQLOCK;
			    if (!FDwtblalloc(ntbl))
			    {
				    IIUGbmaBadMemoryAllocation(ERx("lf"));
			    }
			    for (i = 0; i < ntbl->tfcols; i++)
			    {
				    col = tbl->tfflds[i];
				    chdr = &(col->flhdr);
				    type = &(col->fltype);
				    ncol = FDnewcol(chdr->fhdname,
					    chdr->fhseq);
				    ntbl->tfflds[i] = ncol;
				    nchdr = &(ncol->flhdr);
				    ntype = &(ncol->fltype);
				    MEcopy((PTR)col, sizeof(FLDCOL),
					    (PTR)ncol);
				    nchdr->fhd2flags &= ~fdVQLOCK;
				    nchdr->fhtitle = FEsalloc(chdr->fhtitle);
				    ntype->ftvalmsg = FEsalloc(type->ftvalmsg);
				    ntype->ftdefault = FEsalloc(type->ftdefault);
				    if (type->ftvalstr)
				    {
					    ntype->ftvalstr =
						FEsalloc(type->ftvalstr);
				    }
				    ntype->ftfmtstr = FEsalloc(type->ftfmtstr);
				    (VOID) fmt_areasize(FEadfcb(),
					    ntype->ftfmtstr, &fmtsize);
				    if ((blk = MEreqmem(0, fmtsize, TRUE,
					    (STATUS *)NULL)) == NULL)
				    {
					    IIUGbmaBadMemoryAllocation(ERx("lf"));
				    }
				    (VOID) fmt_setfmt(FEadfcb(),
					    ntype->ftfmtstr, blk,
					    &(ntype->ftfmt), &fmtlen);
			    }
		    }
		    STcopy(buf, nhdr->fhdname);
		    nhdr->fhseq = NEWFLDSEQ;
		    nhdr->fhposy = globy;
		    nhdr->fhposx = globx;
		    if ((overflow = globx + hdr->fhmaxx - 1 - endxFrm) > 0)
		    {
			    vfwider(frame, overflow);
		    }
		    if (nfd->fltag == FREGULAR)
		    {
			    pstype = PFIELD;
			    untype = enFIELD;
		    }
		    else
		    {
			    pstype = PTABLE;
			    untype = enTABLE;
		    }
		    fld_pos = ps = palloc(pstype, globy, globx, hdr->fhmaxy,
			    hdr->fhmaxx, (i4 *)nfd, IIVFsfaSpaceForAttr);
		    setGlobUn(untype, (POS *)NULL, NULL);
		    fitPos(ps, TRUE);
		    insPos(ps, TRUE);
		    created = TRUE;
		    new_fld = nfd;
		}
	}
	else
	{
/* # line 1085 "dupfld.qsc" */	/* message */
  {
    IImessage(ERget(F_VF007F_NoCurFld));
  }
/* # line 1087 "dupfld.qsc" */	/* host code */
		/*
		**  Grab list of forms from db.
		*/
		IIVFlfListForms();
	}
	/*
	**  Put vifred edit screen back to what it was before
	**  DuplicateField was called.
	*/
/* # line 1097 "dupfld.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 1098 "dupfld.qsc" */	/* host code */
	IIVTlf(frame, FALSE, FALSE);
	if (created)
	{
		if (new_fld->fltag == FREGULAR)
		{
			vfFlddisp(new_fld);
		}
		else
		{
			vfTabdisp(new_fld->fld_var.fltblfld, FALSE);
		}
		vffrmcount(fld_pos, 1);
		vfTrigUpd();
		vfUpdate(globy, globx, TRUE);
		vfTestDump();
	}
}