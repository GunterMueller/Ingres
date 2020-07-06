# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  Copyright (c) 2004 Ingres Corporation
*/
/*
** ATTR.QSC
** code to get values from the attribute form
**
** History:
**	01/16/85 (drh)	Modified to eliminate MEfree of fld1list and pslist1,
**			since these are now allocated and freed by tag.
**	02/04/85 (drh)	Added Dave H.'s bug fixes from VMS.
**	06/27/85 (gac)	Replaced sleeped messages by FEmsg.
**	07/18/85 (dkh)	Added changes for international support.  Checking
**			to make sure internal field names are 7 bit ascii.
**	08/19/85 (dkh)	Added support for new field attributes.
**	01/20/86 (prs)	Change "color" title to "Color"
**	03/23/87 (dkh) -Added support for ADTs.
**	03/27/87 (dkh) -Added datatype editing support.
**	10/20/86 (KY)  -- Changed CH.h to CM.h.
**	01/13/87 (wolf) Drop CMS support of NoAutoTab attribute(part of
**			fix for bug #10816)
**	05/06/87 (dkh) - Take away include of afe.h, done by decls.h.
**	08-may-87 (bruceb)
**		Rather than clear fhdflags to 0, set it to the previous
**		value of the fdREVDIR bit.  This has the same effect as
**		clearing to 0, but without the additional result of losing
**		any other bits set in fhdflags prior to editing the
**		attributes.
**	06/30/87 (dkh) - Fix for jup bug 377.
**	13-jul-87 (bruceb) Changed calls on allocArr() to return a PTR (for DG.)
**	08/04/87 (dkh) - Changed to use compiled forms in a file.
**	08/08/87 (dkh) - Usability changes for editing datatypes/formats.
**	08/14/87 (dkh) - ER changes.
**	08/28/87 (dkh) - Fixed buffer size to use FE_MAXNAME.
**	11-sep-87 (bruceb)
**		Changed cast in call on FDfind from char** to nat**.
**	10/02/87 (dkh) - Help file changes.
**	09-nov-87 (bruceb)
**		Code added to allow internal version of scrolling
**		field interface.  This includes IIVFsflScrollFld,
**		internal routine dw_isok, and code dealing with
**		the proper display of the 'Scrolling Data Width'
**		field.
**	10/25/87 (dkh) - Error message cleanup.
**	12/16/87 (dkh) - Fixed jup bug 1609.
**	11-jan-88 (sylviap)
**		Created the .qsc file.
**	09-feb-88 (bruceb)
**		fdSCRLFD no longer referenced through fhdflags; use of
**		the most significant bit of fhdflags caused problems.
**	04/09/88 (dkh) - Integrated a 5.0 bug fix.
**	05/18/88 (dkh) - Added user support for scrollable fields.
**	05/26/88 (dkh) - Cleaned up code.
**	05/26/88 (dkh) - Added additional code to disallow scrolling fields
**			 if attributes DISPLAY ONLY or NO ECHO are also set.
**	06/22/88 (dkh) - Fixed venus bug 2810.
**	08/30/88 (dkh) - Fixed venus bug 3287 & 3288.
**	16-dec-88 (bruceb)
**		Added fdREADONLY and fdINVISIBLE attributes.  Additionally,
**		code modified to expect that no non-sequence fields or
**		fdtfCOLREAD attributes exist (both entities will be folded
**		on top of fdREADONLY at VIFRED startup).  It follows that
**		any form saved in VIFRED will have 'no' non-sequence fields.
**	12/31/88 (dkh) - Perf changes.
**	01/30/89 (dkh) - Integrated CMS changes.
**	02/18/89 (dkh) - Yet more changes for performance.
**	21-mar-89 (bruceb)
**		In attrinit(), use hdr->fhdflags for setting the color,
**		rather than the potentially uninitialized 'flags'.
**	05-apr-89 (bruceb)
**		Use invisible fields rather than query/fill modes.  Made
**		scrollsz field a numeric rather than char field.  Made
**		corresponding changes to the vifattr form.  Only visual
**		change is that scrollsz field will display a 0 rather than
**		blank space as the default.
**		Also, removed IIVFirtInternalRTI and IIVFsflScrollFld();
**		no longer used.
**		Also, no longer modify activate(previousfield); set
**		properly in FEforms().
**	07-apr-89 (bruceb)
**		Retain fdVQLOCK setting in fhd2flags.  If so locked, make
**		datatype and name fields displayonly.
**	20-jun-89 (bruceb)
**		Modified to use two forms for editing attributes:  vifattr
**		and vifvalder (validation string/derivation formula.)
**		Valstr and valmsg made scrollable with fully visible versions
**		available on the vifvalder form.
**	06/23/89 (dkh) - Fixed bug 6745.
**	08/03/89 (dkh) - Updated with interface change to adc_lenchk().
**	11-sep-89 (bruceb)
**		Added invisible table field columns.  If all columns for a
**		table field are made invisible, make the table field invisible.
**	05-dec-89 (bruceb)
**		Derivation formula/validation string field now is modified
**		when the derived (y/n) response is changed.  No longer
**		continue to display the prior contents on the off chance of
**		a user error.
**	01/24/90 (dkh) - Moved include of er.h ahead of uf.h due to changes
**			 in uf.h.
**	01/30/90 (dkh) - Added fix to get around cpp problem with comments.
**	04/18/90 (dkh) - Integrated IBM porting changes.
**	18-apr-90 (bruceb)
**		Lint cleanup; removed 'hdr' arg from vfputdtype.
**      06/09/90 (esd) - Check IIVFsfaSpaceForAttr instead of
**                       calling FTspace, so that whether or not
**                       to leave room for attribute bytes can be
**                       controlled on a form-by-form basis.
**      06/12/90 (esd) - Tighten up the meaning of the global variable
**                       endxFrm.  In the past, it was usually (but
**                       not always) assumed that in a 3270 environment,
**                       the form included a blank column on the right,
**                       so that endxFrm was the 0-relative column
**                       number of that blank column at the right edge.
**                       I am removing that assumption:  endxFrm will
**                       now typically be the 0-relative column number
**                       of the rightmost column containing a feature.
**      06/12/90 (esd) - When boxing a field on a 3270, leave space
**                       for an attribute between the box and title/data
**                       on the left as well as the right (bug 21618).
**      06/12/90 (esd) - When boxing or unboxing a field, set new member
**                       ps_attr of POS structure appropriately.
**	09/07/90 (dkh) - Fixed nattrCom() so that if the form started with
**			 an empty name or datatype (or both), then one can
**			 still exit with the "End" menuitem/frskey.
**	10/26/90 (dkh) - Put in fix to make sure attributes for derived
**			 fields aren't lost when the field is edited.
**	12/05/90 (dkh) - Fixed typo to do bit manipulation correctly.
**	04/06/91 (dkh) - Fixed double byte handling in error handler.
**	08/02/91 (dkh) - Fixed bug 38378.  The code now checks to make
**			 sure the Query Only attribute is disallowed for
**			 derived fields.
**	06/17/92 (dkh) - Added support for decimal datatype for 6.5.
**	12/12/92 (dkh) - Added ability to toggle use of input masks.
**	04/15/93 (dkh) - Fixed bug 50604.  Screened out unsupported
**			 datatypes by calling IIAFfedatatype().
**      07/06/99 (hweho01) 
**              Added function prototype of allocArr().
**              Without the explicit declaration, the default int return
**              value for a function will truncate a 64-bit address 
**              on ris_u64 platform.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	15-Oct-2007 (kibro01) b119305
**	    Add in list choices for data type
*/
# include	<compat.h>
# include	<cv.h>		/* 6-x_PC_80x86 */
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	"decls.h"
# include	"vfunique.h"
# include	<cm.h>
# include	<lo.h>
# include	<si.h>
# include	<er.h>
# include	<fedml.h>
# include	<uf.h>
# include	<ug.h>
# include	"ervf.h"
# include	"vfhlp.h"
# define	RMAXATTR	15
# define	TFMAXATTR	13
/*
**  These two defines should be in a header file
*/
# define	NEXT		0
# define	PREVIOUS	1
typedef struct attribs
{
	i4	attribute;
	char	*name;
	bool	use_fhd2flags;
} ATTRIBS;
static	char	curname[FE_MAXNAME + 1] = {0};
static	FRAME		*vfattr = NULL;
static	DB_USER_TYPE	attrdtype = {0};
static	i4		vfdml = FEDMLQUEL;
static ATTRIBS	attrflag[RMAXATTR] =
{
	{fdBOXFLD,	(char *)NULL,	(bool)0},
	{fdKPREV,	(char *)NULL,	(bool)0},
	{fdMAND,	(char *)NULL,	(bool)0},
	{fdRVVID,	(char *)NULL,	(bool)0},
	{fdBLINK,	(char *)NULL,	(bool)0},
	{fdUNLN,	(char *)NULL,	(bool)0},
	{fdCHGINT,	(char *)NULL,	(bool)0},
	{fdQUERYONLY,	(char *)NULL,	(bool)0},
	{fdLOWCASE,	(char *)NULL,	(bool)0},
	{fdUPCASE,	(char *)NULL,	(bool)0},
	{fdNOAUTOTAB,	(char *)NULL,	(bool)0},
	{fdNOECHO,	(char *)NULL,	(bool)0},
	{fdREADONLY,	(char *)NULL,	(bool)1},
	{fdINVISIBLE,	(char *)NULL,	(bool)0},
	{fdEMASK,	(char *)NULL,	(bool)1}
};
static ATTRIBS	tfattrflag[TFMAXATTR] =
{
	{fdMAND,	(char *)NULL,	(bool)0},
	{fdRVVID,	(char *)NULL,	(bool)0},
	{fdBLINK,	(char *)NULL,	(bool)0},
	{fdUNLN,	(char *)NULL,	(bool)0},
	{fdCHGINT,	(char *)NULL,	(bool)0},
	{fdQUERYONLY,	(char *)NULL,	(bool)0},
	{fdLOWCASE,	(char *)NULL,	(bool)0},
	{fdUPCASE,	(char *)NULL,	(bool)0},
	{fdNOAUTOTAB,	(char *)NULL,	(bool)0},
	{fdNOECHO,	(char *)NULL,	(bool)0},
	{fdREADONLY,	(char *)NULL,	(bool)1},
	{fdINVISIBLE,	(char *)NULL,	(bool)0},
	{fdEMASK,	(char *)NULL,	(bool)1}
};
static	FIELD	**fld1list = NULL;
static	FIELD	**fld2list = NULL;
static	FIELD	**fld3list = NULL;
static	POS	**pslist1 = NULL;
static	POS	**pslist2 = NULL;
static	POS	**pslist3 = NULL;
static	i4	atcurfld = 0;
static	i4	attotflds = 0;
static	FLDHDR	*colorhdr = NULL;
static	i4	colorseq = 0;
static	bool	scrlfld = FALSE;	/* is current field scrollable */
static	bool	form_alloced = FALSE;
static	bool	can_scroll = FALSE;	/* field is potentially scrollable */
static	i4	scrsize = 0;		/* Real scroll length of field */
static	bool	was_chartype = FALSE;	/* was previous datatype a string */
static	i4	fmt_length = 0;		/* Length of format */
static	bool	der_fld = FALSE;
FUNC_EXTERN	i4	FTspace();
FUNC_EXTERN	void	vfdelnm();
FUNC_EXTERN	i4	vfnmunique();
FUNC_EXTERN	i4	nxtfattr();
FUNC_EXTERN	FIELD	*FDfind();
FUNC_EXTERN	FLDHDR	*IIFDgfhGetFldHdr();
FUNC_EXTERN	ADF_CB	*FEadfcb();
FUNC_EXTERN	STATUS	afe_vtyoutput();
FUNC_EXTERN	STATUS	afe_vtychk();
FUNC_EXTERN	STATUS	fmt_vfvalid();
FUNC_EXTERN	STATUS	fmt_ftot();
FUNC_EXTERN	char	*saveStr();
FUNC_EXTERN	FRAME	*IIfindfrm();
FUNC_EXTERN	VOID	IIAFddcDetermineDatatypeClass();
FUNC_EXTERN	bool	fmt_justify();
FUNC_EXTERN     PTR     allocArr();
char		*flgToChar();
VOID		charToFlag();
static	bool	scr_check();
static	VOID	valderhdlr();
# ifndef FORRBF
VOID
allocAttr()
{
    REGFLD	*dummy;
    FIELD	*fld;
    FRAME	*tframe;
    i4		lang;
    if (form_alloced)
    {
	return;
    }
    FDsetparse(FALSE);
    if (IIUFgtfGetForm(IIUFlcfLocateForm(), ERx("vifattr")) != OK)
    {
	syserr(ERget(S_VF0001_Can_not_access_form), ERx("vifattr"));
    }
    if (IIUFgtfGetForm(IIUFlcfLocateForm(), ERx("vifvalder")) != OK)
    {
	syserr(ERget(S_VF0001_Can_not_access_form), ERx("vifvalder"));
    }
    FDsetparse(TRUE);
    /*
    **  Workaround to get things running.  Not sure why this
    **  is needed.
    */
    tframe = IIfindfrm(ERx("vifattr"));
    fld = FDfind((i4 **)tframe->frfld, ERx("colortitle"),
	tframe->frfldno, IIFDgfhGetFldHdr);
    colorseq = FDgtfldno(FT_UPDATE, tframe, fld, &dummy);
    colorhdr = FDgethdr(fld);
    vfattr = tframe;
    attrflag[0].name = ERget(F_VF0001_Box_Field);
    attrflag[1].name = ERget(F_VF0002_Keep_Previous_Value);
    attrflag[2].name = tfattrflag[0].name = ERget(F_VF0003_Mandatory_Field);
    attrflag[3].name = tfattrflag[1].name = ERget(F_VF0004_Reverse_Video);
    attrflag[4].name = tfattrflag[2].name = ERget(F_VF0005_Blinking);
    attrflag[5].name = tfattrflag[3].name = ERget(F_VF0006_Underline);
    attrflag[6].name = tfattrflag[4].name = ERget(F_VF0007_Brightness_Change);
    attrflag[7].name = tfattrflag[5].name = ERget(F_VF0008_Query_Only);
    attrflag[8].name = tfattrflag[6].name = ERget(F_VF0009_Force_Lower_Case);
    attrflag[9].name = tfattrflag[7].name = ERget(F_VF000A_Force_Upper_Case);
    attrflag[10].name = tfattrflag[8].name = ERget(F_VF000B_No_Auto_Tab);
    attrflag[11].name = tfattrflag[9].name = ERget(F_VF000C_No_Echo);
    attrflag[12].name = tfattrflag[10].name = ERget(F_VF000D_Display_Only);
    attrflag[13].name = tfattrflag[11].name = ERget(F_VF0068_Invisible);
    attrflag[14].name = tfattrflag[12].name = ERget(F_VF00BF_Input_mask);
    /*
    **  Set up default dm.
    */
    _VOID_ FEdml(&vfdml, &lang);
    form_alloced = TRUE;
}
/*
** tfattr:
**	Routine to handle table field attributes.
*/
tfattr(col, writeout, tfseq)
FLDCOL	*col;
bool	writeout;
i4	tfseq;
{
	FLDHDR	*hdr;
	FLDTYPE *type;
	allocAttr();
	hdr = &col->flhdr;
	type = &col->fltype;
/* # line 351 "attr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"defval",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 352 "attr.qsc" */	/* host code */
	attrhandler(hdr, type, writeout, tfseq);
}
/*
** writeout is true if frame is being written out
** only allow changes to validation string
*/
attrCom(fd, writeout)
reg	FIELD	*fd;
	bool	writeout;
{
	reg	i4	i;
	reg	VFNODE	**lp;
	reg	VFNODE	*lt;
		FLDHDR	*hdr;
		FLDTYPE *type;
		i4	rfldcount = 0;
	allocAttr();
	attotflds = frame->frfldno;
	fld1list = fld2list = (FIELD **)allocArr(attotflds);
	pslist1 = pslist2 = (POS **)allocArr(attotflds);
	lp = line;
	for (i = 0; i < endFrame; i++, lp++)
	{
		for (lt = *lp; lt != NNIL; lt = vflnNext(lt, i))
		{
			if (lt->nd_pos->ps_begy == i)
			{
				if (lt->nd_pos->ps_name == PFIELD)
				{
				    *fld2list++ = (FIELD *)lt->nd_pos->ps_feat;
				    *pslist2++ = lt->nd_pos;
				    rfldcount++;
				}
			}
		}
	}
	fld3list = --fld2list;
	pslist3 = --pslist2;
	atcurfld = -1;
	fld2list = fld1list;
	pslist2 = pslist1;
	attotflds = rfldcount;
	for (i = 0; i < attotflds; i++)
	{
		if (fd == *fld2list)
		{
			atcurfld = i;
			atcurfld++;
			break;
		}
		else
		{
			fld2list++;
			pslist2++;
		}
	}
	if (atcurfld == -1)
	{
		IIUGerr(E_VF0002_Cant_find_field, UG_ERR_ERROR, 0);
		return;
	}
	hdr = FDgethdr(fd);
	type = FDgettype(fd);
	vfdmpoff();
/* # line 420 "attr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"defval",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 421 "attr.qsc" */	/* host code */
	attrhandler(hdr, type, writeout, (i4) -1);
}
attrinit(hdr, type, fromtf, colorval, der_buf, val_buf)
FLDHDR	*hdr;
FLDTYPE *type;
bool	fromtf;
i4	*colorval;
char	*der_buf;
char	*val_buf;
{
	i4		flags;
	i4		i;
  char *cp;
  char *cp2;
  char *cp3;
  char *iattrptr;
  char iuflag[5];
  i4 attrcolor;
	i4		maxattr;
	ATTRIBS		*attr;
  char *lyes1;
  char *lno1;
  i4 scroll_width;
	i4		class;
	i4		x;
	i4		y;
	i4		justify;
	DB_DATA_VALUE	dbv;
	DB_DATA_VALUE	sdbv;
	lyes1 = IIVF_yes1;
	lno1 = IIVF_no1;
	STcopy(hdr->fhdname, curname);
	if (fromtf)
	{
		maxattr = TFMAXATTR;
		attr = tfattrflag;
	}
	else
	{
		maxattr = RMAXATTR;
		attr = attrflag;
	}
/* # line 467 "attr.qsc" */	/* inittable */
  {
    if (IItbinit((char *)"vifattr",(char *)"vifattr",(char *)"u") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 468 "attr.qsc" */	/* host code */
	for (i = 0; i < maxattr; i++, attr++)
	{
		if (attr->use_fhd2flags)
			flags = hdr->fhd2flags;
		else
			flags = hdr->fhdflags;
		/* If derived field, don't show the (internal) disponly flag. */
		if (attr->attribute & fdREADONLY && hdr->fhd2flags & fdDERIVED)
			STcopy(flgToChar(flags, (i4)0), iuflag);
		else
			STcopy(flgToChar(flags, attr->attribute), iuflag);
		iattrptr = attr->name;
/* # line 482 "attr.qsc" */	/* loadtable */
  {
    if (IItbact((char *)"vifattr",(char *)"vifattr",1) != 0) {
      IItcoputio((char *)"attr",(short *)0,1,32,0,iattrptr);
      IItcoputio((char *)"sett",(short *)0,1,32,0,iuflag);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 483 "attr.qsc" */	/* host code */
	}
	if (hdr->fhd2flags & fdVQLOCK)
	{
/* # line 487 "attr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
      IIstfsio(53,(char *)"datatype",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(53,(char *)"name",0,(short *)0,1,30,sizeof(1),(void *)IILQint(1
      ));
    } /* IIiqset */
  }
/* # line 489 "attr.qsc" */	/* host code */
	}
	else
	{
/* # line 492 "attr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
      IIstfsio(53,(char *)"datatype",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(53,(char *)"name",0,(short *)0,1,30,sizeof(0),(void *)IILQint(0
      ));
    } /* IIiqset */
  }
/* # line 494 "attr.qsc" */	/* host code */
	}
	/*
	** Check whether this is the proper sort of field for the
	** scrolling attribute (basically, a character field).  This
	** type of check means that on creation of a new regular field,
	** since the datatype is initially unknown, this section will
	** not be triggered.
	*/
	IIAFddcDetermineDatatypeClass(type->ftdatatype, &class);
	if (type->ftfmt)
	{
		_VOID_ fmt_size(FEadfcb(), type->ftfmt, NULL, &y, &x);
		justify = fmt_justify(FEadfcb(), type->ftfmt);
		fmt_length = x;
	}
	else
	{
		y = 2;
		justify = TRUE;
		fmt_length = 0;
	}
	can_scroll = FALSE;
	was_chartype = FALSE;
	if (class == CHAR_DTYPE && y == 1 && justify == FALSE)
	{
		can_scroll = TRUE;
		was_chartype = TRUE;
	}
	if (can_scroll)
	{
		/*
		**  If field is already set to be scrollable, set
		**  up special fields for display.
		*/
		if (hdr->fhd2flags & fdSCRLFD)
		{
			dbv.db_datatype = type->ftdatatype;
			dbv.db_length = type->ftlength;
			dbv.db_prec = type->ftprec;
			if (adc_lenchk(FEadfcb(), FALSE, &dbv, &sdbv)
				!= E_DB_OK)
			{
				scroll_width = 0;
			}
			else
			{
				scroll_width = sdbv.db_length;
			}
			scrsize = scroll_width;
/* # line 544 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"scrollable",(short *)0,1,32,0,lyes1);
      IIputfldio((char *)"scrollsz",(short *)0,1,30,4,&scroll_width);
    } /* IIfsetio */
  }
/* # line 546 "attr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"scrollable",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"scrollsz",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 548 "attr.qsc" */	/* host code */
			scrlfld = TRUE;		/* this is a scrollable field */
		}
		else
		{
/* # line 552 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"scrollable",(short *)0,1,32,0,lno1);
      IIputfldio((char *)"scrollsz",(short *)0,1,30,sizeof(0),(void *)IILQint(
      0));
    } /* IIfsetio */
  }
/* # line 553 "attr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"scrollable",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"scrollsz",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 555 "attr.qsc" */	/* host code */
			scrlfld = FALSE;
		}
		/*
		** Don't allow scrollable, derived fields; turn off the
		** scrollable flag (since this is really an error condition).
		*/
		if (hdr->fhd2flags & fdDERIVED)
		{
			hdr->fhd2flags &= ~fdSCRLFD;
			scrlfld = FALSE;
/* # line 565 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"scrollable",(short *)0,1,32,0,lno1);
      IIputfldio((char *)"scrollsz",(short *)0,1,30,sizeof(0),(void *)IILQint(
      0));
    } /* IIfsetio */
  }
/* # line 566 "attr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"scrollable",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"scrollsz",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 568 "attr.qsc" */	/* host code */
		}
	}
	else
	{
		scrlfld = FALSE;	/* this is not a scrollable field */
/* # line 573 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"scrollable",(short *)0,1,32,0,lno1);
      IIputfldio((char *)"scrollsz",(short *)0,1,30,sizeof(0),(void *)IILQint(
      0));
    } /* IIfsetio */
  }
/* # line 574 "attr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"scrollable",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"scrollsz",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 577 "attr.qsc" */	/* host code */
		/*
		**  If field is scrollable, things are out of sync.
		**  This can happen if user is editing a table field,
		**  sets scrollability and then later on changes the
		**  format for the column to make it ineligible for
		**  scrollability.  Unset it and put out error message.
		*/
		if (hdr->fhd2flags & fdSCRLFD)
		{
			hdr->fhd2flags &= ~fdSCRLFD;
			IIUGerr(E_VF011B_scroll_sync, UG_ERR_ERROR, 1,
				hdr->fhdname);
		}
	}
	*der_buf = EOS;
	*val_buf = EOS;
	if (hdr->fhd2flags & fdDERIVED)
	{
		der_fld = TRUE;
		cp = IIVF_yes1;
		cp2 = ERget(F_VF0069_Der_Form);
		cp3 = ERget(F_VF006D_ValNotAllowed);
/* # line 600 "attr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"valmsg",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 601 "attr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vifvalder",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"valmsg",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 602 "attr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"defval",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 603 "attr.qsc" */	/* host code */
	}
	else
	{
		der_fld = FALSE;
		cp = IIVF_no1;
		cp2 = ERget(F_VF006A_Val_Chk);
		cp3 = ERget(F_VF006C_ValAllowed);
/* # line 610 "attr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"valmsg",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 611 "attr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vifvalder",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"valmsg",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 612 "attr.qsc" */	/* host code */
		if (!fromtf)
/* # line 613 "attr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"defval",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 614 "attr.qsc" */	/* host code */
	}
/* # line 615 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"derived",(short *)0,1,32,0,cp);
    } /* IIfsetio */
  }
/* # line 616 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"valstrtitle",(short *)0,1,32,0,cp2);
    } /* IIfsetio */
  }
/* # line 617 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifvalder") != 0) {
      IIputfldio((char *)"valstrtitle",(short *)0,1,32,0,cp2);
    } /* IIfsetio */
  }
/* # line 618 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"expl_text",(short *)0,1,32,0,cp3);
    } /* IIfsetio */
  }
/* # line 620 "attr.qsc" */	/* host code */
	cp = type->ftvalstr;
/* # line 621 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"valstr",(short *)0,1,32,0,cp);
    } /* IIfsetio */
  }
/* # line 623 "attr.qsc" */	/* host code */
	cp = type->ftvalmsg;
/* # line 624 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"valmsg",(short *)0,1,32,0,cp);
    } /* IIfsetio */
  }
/* # line 626 "attr.qsc" */	/* host code */
	cp = type->ftdefault;
/* # line 627 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"defval",(short *)0,1,32,0,cp);
    } /* IIfsetio */
  }
/* # line 629 "attr.qsc" */	/* host code */
	cp = hdr->fhdname;
/* # line 630 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"name",(short *)0,1,32,0,cp);
    } /* IIfsetio */
  }
/* # line 632 "attr.qsc" */	/* host code */
	attrcolor = vfgetcolor(hdr->fhdflags);
	colorhdr->fhdflags &= ~fdCOLOR;
	colorhdr->fhdflags |= hdr->fhdflags & fdCOLOR;
	*colorval = attrcolor;
/* # line 637 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"color",(short *)0,1,30,4,&attrcolor);
    } /* IIfsetio */
  }
/* # line 638 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"colortitle",(short *)0,1,32,0,ERget(F_VF0010_Color));
    } /* IIfsetio */
  }
/* # line 640 "attr.qsc" */	/* host code */
	FTsetda(vfattr, colorseq, FT_UPDATE, FT_REGFLD, 0, 0,
		colorhdr->fhdflags);
}
vfputdtype(type)
FLDTYPE *type;
{
	DB_DATA_VALUE	dbv;
  char *isnull;
  char *dtype;
	/*
	**  Decode datatype information and put it into the form.
	**  Also save this in attrdtype so we can put it back
	**  if there is a conflict with the datatype entered by
	**  the user.
	*/
	dtype = attrdtype.dbut_name;
	attrdtype.dbut_kind = DB_UT_NOTNULL;
	if (type->ftdatatype == DB_NODT)
	{
		*dtype = '\0';
	}
	else
	{
		dbv.db_datatype = type->ftdatatype;
		dbv.db_length = type->ftlength;
		dbv.db_prec = type->ftprec;
		if (afe_vtyoutput(FEadfcb(), &dbv, &attrdtype) != OK)
		{
			IIUGerr(E_VF0003_Could_not_decode_data,
				UG_ERR_ERROR, 0);
		}
	}
	/*
	**  Only set the nullable field to "y" if dbut_kind is
	**  set to DB_UT_NULL.	Any other value is treated as
	**  non NULL.
	*/
	if (attrdtype.dbut_kind == DB_UT_NULL)
	{
		isnull = IIVF_yes1;
	}
	else
	{
		isnull = IIVF_no1;
	}
/* # line 689 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"datatype",(short *)0,1,32,0,dtype);
    } /* IIfsetio */
  }
/* # line 690 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"nullable",(short *)0,1,32,0,isnull);
    } /* IIfsetio */
  }
/* # line 691 "attr.qsc" */	/* host code */
}
/*
** vfretattr:
**	Retrieve attribute information from the form
**	before exiting or going to the next field.
*/
STATUS
vfretattr(nhdr, ntype, oldname, writeout, fromtf)
FLDHDR	*nhdr;
FLDTYPE *ntype;
char	*oldname;
bool	writeout;
bool	fromtf;
  {
    char uattr[30];
    char uflag[6];
    i4 acolor;
	i4		i;
    char *cptr;
    char *cp;
    char *no_1;
	char		buf[BUFSIZ];
	SMLFLD		smallfd;
	SMLFLD		*sfd;
	POS		*ps;
	i4		maxattr;
	ATTRIBS		*attr;
	i4		nametype;
	i4		spacesize;
	bool		badattr = FALSE;
	DB_USER_TYPE	usertype;
	DB_DATA_VALUE	dbv;
	bool		valid;
	bool		is_str;
	i4		*flags;		
	spacesize = IIVFsfaSpaceForAttr;
	no_1 = IIVF_no1;
	if (!writeout)
	{
		if (fromtf)
		{
			maxattr = TFMAXATTR;
			attr = tfattrflag;
			nametype = ATBLFD;
		}
		else
		{
			maxattr = RMAXATTR;
			attr = attrflag;
			nametype = AFIELD;
		}
		/*
		** clear all field attributes except those that can
		** obtain values outside of the edit attribute form.
		*/
		nhdr->fhdflags &= fdREVDIR;
		nhdr->fhd2flags &= (fdSCRLFD | fdVQLOCK);
		i = 0;
/* # line 755 "attr.qsc" */	/* unloadtable */
    {
      if (IItbact((char *)"vifattr",(char *)"vifattr",0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,29,uattr,(char *)"attr");
        IItcogetio((short *)0,1,32,5,uflag,(char *)"sett");
        IITBceColEnd();
        {
/* # line 757 "attr.qsc" */	/* host code */
			if (i < maxattr)
			{
				if (attr->use_fhd2flags)
					flags = &nhdr->fhd2flags;
				else
					flags = &nhdr->fhdflags;
				charToFlag(flags, uflag, attr->attribute);
				attr++;
				i++;
			}
			else
			{
/* # line 770 "attr.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 771 "attr.qsc" */	/* host code */
			}
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 774 "attr.qsc" */	/* host code */
		if ((nhdr->fhd2flags & fdREADONLY)
		    && (nhdr->fhdflags & fdQUERYONLY))
		{
			IIUGerr(E_VF0004_Cant_set_both, UG_ERR_ERROR, 0);
			badattr = TRUE;
		}
		if ((nhdr->fhdflags & fdLOWCASE) && (nhdr->fhdflags & fdUPCASE))
		{
			IIUGerr(E_VF0005_Cant_set_low_up, UG_ERR_ERROR, 0);
			badattr = TRUE;
		}
		if ((nhdr->fhd2flags & fdSCRLFD) && (nhdr->fhdflags & fdNOECHO))
		{
			IIUGerr(E_VF0120_scroll_badattr, UG_ERR_ERROR, 0);
			badattr = TRUE;
		}
		if (badattr)
		{
			return(FAIL);
		}
	}
	cptr = buf;
/* # line 797 "attr.qsc" */	/* getform */
    {
      if (IIfsetio((char *)"vifattr") != 0) {
        IIgetfldio((short *)0,1,32,0,cptr,(char *)"valstr");
      } /* IIfsetio */
    }
/* # line 798 "attr.qsc" */	/* host code */
	ntype->ftvalstr = saveStr(cptr);
/* # line 800 "attr.qsc" */	/* getform */
    {
      if (IIfsetio((char *)"vifattr") != 0) {
        IIgetfldio((short *)0,1,32,5,uflag,(char *)"derived");
      } /* IIfsetio */
    }
/* # line 801 "attr.qsc" */	/* host code */
	charToFlag(&nhdr->fhd2flags, uflag, fdDERIVED);
	if (nhdr->fhd2flags & fdDERIVED)
	{
		if (ntype->ftvalstr[0] == EOS)
		{
			nhdr->fhd2flags &= ~fdDERIVED;
		}
		else if ((nhdr->fhdflags & fdMAND)
			|| (nhdr->fhdflags & fdLOWCASE)
			|| (nhdr->fhdflags & fdUPCASE)
			|| (nhdr->fhdflags & fdQUERYONLY)
			|| (nhdr->fhd2flags & fdREADONLY))	/* User set. */
		{
			IIUGerr(E_VF012C_Cant_set_derived_and, UG_ERR_ERROR, 0);
			/* Turn off the offending attributes. */
			nhdr->fhdflags &= ~(fdMAND | fdLOWCASE | fdUPCASE | fdQUERYONLY);
			/* Turn on the (internal) disponly flag if derived. */
			nhdr->fhd2flags |= fdREADONLY;	/* Internally set. */
		}
		else
		{
			/* Turn on the (internal) disponly flag if derived. */
			nhdr->fhd2flags |= fdREADONLY;	/* Internally set. */
		}
	}
/* # line 830 "attr.qsc" */	/* getform */
    {
      if (IIfsetio((char *)"vifattr") != 0) {
        IIgetfldio((short *)0,1,32,0,cptr,(char *)"defval");
      } /* IIfsetio */
    }
/* # line 831 "attr.qsc" */	/* host code */
	if (nhdr->fhd2flags & fdDERIVED)
	{
		/* No default values for derived fields. */
		cptr[0] = EOS;
	}
	ntype->ftdefault = saveStr(cptr);
/* # line 838 "attr.qsc" */	/* getform */
    {
      if (IIfsetio((char *)"vifattr") != 0) {
        IIgetfldio((short *)0,1,30,4,&acolor,(char *)"color");
      } /* IIfsetio */
    }
/* # line 839 "attr.qsc" */	/* host code */
	vfsetcolor(&nhdr->fhdflags, acolor);
	/*
	**  Clear out validation tree pointer in case user
	**  changed a field from writeable to display-only.
	**  Trees will be recreated, if necessary, when form
	**  is written to the database.
	*/
	ntype->ftvalchk = NULL;
/* # line 849 "attr.qsc" */	/* getform */
    {
      if (IIfsetio((char *)"vifattr") != 0) {
        IIgetfldio((short *)0,1,32,0,cptr,(char *)"valmsg");
      } /* IIfsetio */
    }
/* # line 850 "attr.qsc" */	/* host code */
	if (nhdr->fhd2flags & fdDERIVED)
	{
		/* No validation error message for derived fields. */
		cptr[0] = EOS;
	}
	ntype->ftvalmsg = saveStr(cptr);
	/*
	**  Get and check datatype/nullability from form.
	*/
/* # line 860 "attr.qsc" */	/* getform */
    {
      if (IIfsetio((char *)"vifattr") != 0) {
        IIgetfldio((short *)0,1,32,0,cptr,(char *)"datatype");
      } /* IIfsetio */
    }
/* # line 861 "attr.qsc" */	/* host code */
	STcopy(cptr, usertype.dbut_name);
	/*
	**  If from a table field, and the datatype type is
	**  blank and we started with blank, then let user
	**  get out.
	*/
	if (fromtf && *cptr == '\0' && attrdtype.dbut_name[0] == '\0')
	{
		ntype->ftdatatype = DB_NODT;
		ntype->ftlength = 0;
		ntype->ftprec = 0;
		nhdr->fhd2flags &= ~fdSCRLFD;
	}
	else
	{
/* # line 877 "attr.qsc" */	/* getform */
    {
      if (IIfsetio((char *)"vifattr") != 0) {
        IIgetfldio((short *)0,1,32,0,cptr,(char *)"nullable");
      } /* IIfsetio */
    }
/* # line 878 "attr.qsc" */	/* host code */
		CVlower(cptr);
		if (*cptr == *IIVF_yes1)
		{
			usertype.dbut_kind = DB_UT_NULL;
		}
		else
		{
			usertype.dbut_kind = DB_UT_NOTNULL;
		}
		if (afe_vtychk(FEadfcb(), &usertype, &dbv) != OK)
		{
			/*
			**  This should not happen since this check
			**  should have already been done before
			**  this is called.  This call is made
			**  here to get the dbv set up.
			*/
			IIUGerr(E_VF0007_not_valid_datatype, UG_ERR_ERROR,
				1, usertype.dbut_name);
			cp = attrdtype.dbut_name;
			if (*cp != '\0')
			{
/* # line 900 "attr.qsc" */	/* putform */
    {
      if (IIfsetio((char *)"vifattr") != 0) {
        IIputfldio((char *)"datatype",(short *)0,1,32,0,cp);
      } /* IIfsetio */
    }
/* # line 901 "attr.qsc" */	/* host code */
			}
			return(FAIL);
		}
		/*
		**  Check for compatiblity with the format if we are not
		**  called from table field creation.
		**  Otherwise, assign datatype information for
		**  table field column.
		*/
		if (!fromtf)
		{
			if (fmt_vfvalid(FEadfcb(), ntype->ftfmt, &dbv,
				&valid, &is_str) != OK || !valid)
			{
				IIUGerr(E_VF0008_datatype_not_compat,
					UG_ERR_ERROR, 2,
					usertype.dbut_name, ntype->ftfmtstr);
				cp = attrdtype.dbut_name;
/* # line 920 "attr.qsc" */	/* putform */
    {
      if (IIfsetio((char *)"vifattr") != 0) {
        IIputfldio((char *)"datatype",(short *)0,1,32,0,cp);
      } /* IIfsetio */
    }
/* # line 922 "attr.qsc" */	/* host code */
				/*
				**  Setup scrollability again if we
				**  came in with scrollability.  All
				**  we need to check is "can_scroll"
				**  since it is set from the datatype
				**  that we are restoring.
				*/
				if (can_scroll && !was_chartype)
				{
/* # line 931 "attr.qsc" */	/* putform */
    {
      if (IIfsetio((char *)"vifattr") != 0) {
        IIputfldio((char *)"scrollable",(short *)0,1,32,0,no_1);
        IIputfldio((char *)"scrollsz",(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIfsetio */
    }
/* # line 933 "attr.qsc" */	/* host code */
				    was_chartype = TRUE;
				    if (!(nhdr->fhd2flags & fdDERIVED))
				    {
/* # line 936 "attr.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
        IIstfsio(54,(char *)"scrollable",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 938 "attr.qsc" */	/* host code */
				    }
				}
				return(FAIL);
			}
			/*
			**  If the datatype is decimal, make sure we
			**  pick up the latest precision/scale values.
			**
			**  This is not needed since it is done below.
			**
			if (dbv.db_datatype == DB_DEC_TYPE)
			{
				ntype->ftlength = dbv.db_length;
				ntype->ftprec = dbv.db_prec;
			}
			*/
		}
		ntype->ftdatatype = dbv.db_datatype;
		ntype->ftlength = dbv.db_length;
		ntype->ftprec = dbv.db_prec;
		/*
		**  If called from a table field editing phase,
		**  then need to check if the data length is zero
		**  (set by call to afe_vtychk).  If so, put in
		**  a fake length so that things will still work
		**  if user cycles around to this column again.
		**  This is OK, since data length can only be zero
		**  if the datatype is one of the character ones.
		**  The character data length will be filled in
		**  at table field creation time.
		*/
		if (fromtf && ntype->ftlength == 0)
		{
			if (abs(ntype->ftdatatype) == DB_DEC_TYPE)
			{
				DB_DATA_VALUE	dbv1;
				DB_DATA_VALUE	dbv2;
				dbv1.db_datatype = ntype->ftdatatype;
				dbv1.db_length = DB_PREC_TO_LEN_MACRO(10);
				dbv1.db_prec = DB_PS_ENCODE_MACRO(10, 0);
				(void)adc_lenchk(FEadfcb(), TRUE, &dbv1, &dbv2);
				ntype->ftlength = dbv2.db_length;
				ntype->ftprec = dbv1.db_prec;
			}
			else
			{
				ntype->ftlength = 10;
			}
		}
	}
	cptr = nhdr->fhdname;
/* # line 994 "attr.qsc" */	/* getform */
    {
      if (IIfsetio((char *)"vifattr") != 0) {
        IIgetfldio((short *)0,1,32,0,cptr,(char *)"name");
      } /* IIfsetio */
    }
/* # line 995 "attr.qsc" */	/* host code */
	if (STcompare(oldname, cptr) != 0 &&
		!vffnmchk(cptr, ERget(F_VF0011_field)))
	{
		return(FAIL);
	}
	CVlower(nhdr->fhdname);
	/*
	**  Fix for jup bug 377. (dkh)
	*/
	if (STbcompare(nhdr->fhdname, 0, curname, 0, TRUE) != 0)
	{
		if (vfnmunique(nhdr->fhdname, TRUE, nametype) != FOUND)
		{
			vfdelnm(curname, nametype);
		}
		else
		{
			IIUGerr(E_VF000A_internal_name_exists, UG_ERR_ERROR,
				1, nhdr->fhdname);
			return(FAIL);
		}
	}
	if (!fromtf)
	{
		/*
		**  Everything OK, set new datatype/nullability information
		**  only if not from a table field.
		*/
		ntype->ftdatatype = dbv.db_datatype;
		ntype->ftprec = dbv.db_prec;
		/*
		**  Finally, set scroll length, which is the internal
		**  adf length for the datatype if the field is
		**  still qualified to be scrollable.  This value was set up
		**  by a previous call to scr_check().
		*/
		if (nhdr->fhd2flags & fdSCRLFD)
		{
			ntype->ftlength = scrsize;
		}
		else
		{
			ntype->ftlength = dbv.db_length;
		}
		FDToSmlfd(&smallfd, *pslist2);
		wrOver();
		unLinkPos(*pslist2);
		vfersPos(*pslist2);
		savePos(*pslist2);
		ps = *pslist2;
		sfd = &smallfd;
		ps->ps_begx = min(sfd->tx, sfd->dx);
		ps->ps_begy = min(sfd->ty, sfd->dy);
		ps->ps_endx = max(sfd->tex, sfd->dex);
		ps->ps_endy = max(sfd->tey, sfd->dey);
		if (nhdr->fhdflags & fdBOXFLD)
		{
			/*
			**  spacesize used to account for attribute byte space
			**  needed by some terminals, most notably the 3270.
			**  (dkh)
			*/
			if (ps->ps_begy == 0 || ps->ps_begx <= spacesize ||
				ps->ps_endx >= endxFrm - spacesize)
			{
				IIUGerr(E_VF000B_Cant_box_this_field, 
					UG_ERR_ERROR, 0);
				nhdr->fhdflags &= (~fdBOXFLD);
			}
			else
			{
				ps->ps_begx -= 1 + spacesize;
				ps->ps_endx += 1 + spacesize;
				ps->ps_begy--;
				ps->ps_endy++;
			}
		}
		fitPos(*pslist2, TRUE);
		insPos(*pslist2, TRUE);
		moveComp(*pslist2, &smallfd);
	}
	else
	{
		if (nhdr->fhd2flags & fdSCRLFD)
		{
			ntype->ftlength = scrsize;
		}
	}
	return(OK);
  }
/*
** nxregattr:
**	Find the next regular field for setting
**	attributes.
*/
nxregattr(type, hdrptr, typeptr)
i4	type;
FLDHDR	**hdrptr;
FLDTYPE **typeptr;
{
	FIELD	*fld;
	if (type == NEXT)
	{
		if (atcurfld == attotflds)
		{
			atcurfld = 1;
			fld = *fld1list;
			fld2list = fld1list;
			pslist2 = pslist1;
		}
		else
		{
			atcurfld++;
			fld = *++fld2list;
			pslist2++;
		}
	}
	else
	{
		if (atcurfld == 1)
		{
			atcurfld = attotflds;
			fld = *fld3list;
			fld2list = fld3list;
			pslist2 = pslist3;
		}
		else
		{
			atcurfld--;
			fld = *--fld2list;
			pslist2--;
		}
	}
	*hdrptr = FDgethdr(fld);
	*typeptr = FDgettype(fld);
	vfDispAll(endFrame);
	setGlobUn((i4) edATTR, *pslist2, fld);
}
/*
** nomoreattr:
**	Routine to print out message saying there are no
**	more attributes to edit.  This only happens if
**	there is only one column in a table field or
**	there is only one regular field in the form.
*/
nomoreattr()
{
	IIUGerr(E_VF000C_There_are_no_other_at, UG_ERR_ERROR, 0);
}
/*
** attrhandler:
**	Code to control the attribute form
*/
attrhandler(hdr, type, writeout, tfseq)
FLDHDR	*hdr;
FLDTYPE *type;
bool	writeout;
i4	tfseq;
{
# ifndef FORRBF
	i4	curfield;
# endif
	FLDHDR	*nhdr;
	FLDTYPE *ntype;
  i4 afldcolor;
	i4	colorval;
	char	oldnm[FE_MAXNAME + 1];
	bool	fromtf = FALSE;
	bool	nomore = FALSE;
	char	hlpbuf[100];
  char isscrollable[2];
  char dttype[40];
	i4	class;
  char *no1;
  char dval[2];
  char formula[fdVSTRLEN + 1];
  char msg[fdVMSGLEN + 1];
  char *chp;
  char *chp2;
	DB_DATA_VALUE	dbv;
	DB_USER_TYPE	usertype;
  char der_buf[fdVSTRLEN + 1];
  char val_buf[fdVSTRLEN + 1];
	no1 = IIVF_no1;
	/*
	** first fill in the attributes from the field
	** structure and then drive the form
	** when control comes back fill in the field
	*/
	nhdr = hdr;
	ntype = type;
	FTclear();
	/*
	** now drive the form
	** when we get back fill in
	*/
	if (tfseq != -1)
	{
# ifndef FORRBF
		curfield = tfseq;
# endif
		fromtf = TRUE;
	}
/* # line 1219 "attr.qsc" */	/* display */
  {
    if (IIdispfrm(ERx("vifattr"),(char *)"u") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 1222 "attr.qsc" */	/* host code */
		STcopy(hdr->fhdname, oldnm);
		attrinit(hdr, type, fromtf, &colorval, der_buf, val_buf);
		vfputdtype(type);
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 1228 "attr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 1232 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,30,4,&afldcolor,(char *)"color");
            } /* IIfsetio */
          }
/* # line 1233 "attr.qsc" */	/* host code */
		if (colorval != afldcolor)
		{
			vfsetcolor(&colorhdr->fhdflags, afldcolor);
			colorval = afldcolor;
			FTsetda(vfattr, colorseq, FT_UPDATE, FT_REGFLD, 0, 0,
				colorhdr->fhdflags);
		}
		else
		{
/* # line 1242 "attr.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
/* # line 1243 "attr.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 3) {
        {
/* # line 1247 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIgetfldio((short *)0,1,32,39,dttype,(char *)"datatype");
            } /* IIfsetio */
          }
/* # line 1248 "attr.qsc" */	/* host code */
		STcopy(dttype, usertype.dbut_name);
		usertype.dbut_kind = DB_UT_NOTNULL;
		if (afe_vtychk(FEadfcb(), &usertype, &dbv) != OK)
		{
			IIUGerr(E_VF0007_not_valid_datatype,
				UG_ERR_ERROR, 1, usertype.dbut_name);
/* # line 1254 "attr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 1255 "attr.qsc" */	/* host code */
		}
		/*
		**  Need to check if datatype is not one of the
		**  string types.  If not a string type anymore,
		**  we need to remove the scrollable fields
		**  We probably want to save info in the scrollable
		**  fields to display them again if datatype is
		**  changed back to a string type.
		*/
		if (can_scroll)
		{
			IIAFddcDetermineDatatypeClass(dbv.db_datatype, &class);
			/*
			**  Display scrollable field if string datatype.
			*/
			if (class == CHAR_DTYPE)
			{
				if (!was_chartype)
				{
/* # line 1275 "attr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIputfldio((char *)"scrollable",(short *)0,1,32,0,no1);
              IIputfldio((char *)"scrollsz",(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIfsetio */
          }
/* # line 1277 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,1,dval,(char *)"derived");
            } /* IIfsetio */
          }
/* # line 1278 "attr.qsc" */	/* host code */
					if (*dval == *IIVF_no1)
					{
/* # line 1280 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"scrollable",0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 1282 "attr.qsc" */	/* host code */
					}
				}
				was_chartype = TRUE;
			}
			else
			{
				was_chartype = FALSE;
				/*
				**  Otherwise, make scroll fields disappear.
				*/
				scrlfld = FALSE;
/* # line 1293 "attr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIputfldio((char *)"scrollable",(short *)0,1,32,0,no1);
              IIputfldio((char *)"scrollsz",(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIfsetio */
          }
/* # line 1294 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"scrollable",0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
              IIstfsio(54,(char *)"scrollsz",0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 1297 "attr.qsc" */	/* host code */
			}
		}
/* # line 1299 "attr.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 1303 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,1,isscrollable,(char *)
"scrollable");
            } /* IIfsetio */
          }
/* # line 1304 "attr.qsc" */	/* host code */
		if (*isscrollable == *IIVF_yes1)
		{
			/*
			**  If not scrollable, display other
			**  field.  Otherwise, do nothing.
			*/
			if (!scrlfld)
			{
/* # line 1312 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"scrollsz",0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 1313 "attr.qsc" */	/* host code */
				scrlfld = TRUE;
			}
		}
		else if (*isscrollable == *IIVF_no1)
		{
			/*
			**  If scrollable, remove other field.
			**  Otherwise, do nothing.
			*/
			if (scrlfld)
			{
				/*
				**  Retain value even though now invisible.
				*/
/* # line 1327 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"scrollsz",0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 1328 "attr.qsc" */	/* host code */
				scrlfld = FALSE;
			}
		}
		else
		{
			IIUGerr(E_VF011C_bad_scroll_val, UG_ERR_ERROR, 0);
/* # line 1334 "attr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 1335 "attr.qsc" */	/* host code */
		}
/* # line 1336 "attr.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 1340 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,1,dval,(char *)"derived");
            } /* IIfsetio */
          }
/* # line 1341 "attr.qsc" */	/* host code */
	    if (*dval == *IIVF_yes1)
	    {
		if (der_fld == FALSE)
		{
		    der_fld = TRUE;
/* # line 1346 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,fdVSTRLEN + 1-1,val_buf,(char *)
"valstr");
            } /* IIfsetio */
          }
/* # line 1347 "attr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIputfldio((char *)"valstr",(short *)0,1,32,0,der_buf);
            } /* IIfsetio */
          }
/* # line 1348 "attr.qsc" */	/* host code */
		}
		chp = ERget(F_VF0069_Der_Form);
		chp2 = ERget(F_VF006D_ValNotAllowed);
/* # line 1351 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"valmsg",0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 1352 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifvalder",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"valmsg",0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 1353 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"defval",0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 1354 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"scrollable",0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
              IIstfsio(54,(char *)"scrollsz",0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 1356 "attr.qsc" */	/* host code */
	    }
	    else	/* *dval == *IIVF_no1 */
	    {
		if (der_fld == TRUE)
		{
		    der_fld = FALSE;
/* # line 1362 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,fdVSTRLEN + 1-1,der_buf,(char *)
"valstr");
            } /* IIfsetio */
          }
/* # line 1363 "attr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIputfldio((char *)"valstr",(short *)0,1,32,0,val_buf);
            } /* IIfsetio */
          }
/* # line 1364 "attr.qsc" */	/* host code */
		}
		chp = ERget(F_VF006A_Val_Chk);
		chp2 = ERget(F_VF006C_ValAllowed);
/* # line 1367 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"valmsg",0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 1368 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifvalder",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"valmsg",0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 1369 "attr.qsc" */	/* host code */
		if (!fromtf)
/* # line 1370 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"defval",0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 1371 "attr.qsc" */	/* host code */
		if (can_scroll)
/* # line 1372 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"scrollable",0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 1373 "attr.qsc" */	/* host code */
		if (scrlfld)
/* # line 1374 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"scrollsz",0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 1375 "attr.qsc" */	/* host code */
	    }
/* # line 1376 "attr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIputfldio((char *)"valstrtitle",(short *)0,1,32,0,chp);
            } /* IIfsetio */
          }
/* # line 1377 "attr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vifvalder") != 0) {
              IIputfldio((char *)"valstrtitle",(short *)0,1,32,0,chp);
            } /* IIfsetio */
          }
/* # line 1378 "attr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIputfldio((char *)"expl_text",(short *)0,1,32,0,chp2);
            } /* IIfsetio */
          }
/* # line 1379 "attr.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 1383 "attr.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"vifattr") == 0) {
              goto IIfdB1;
            } /* IIvalfld */
          }
/* # line 1384 "attr.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"nullable") == 0) {
              goto IIfdB1;
            } /* IIvalfld */
          }
/* # line 1385 "attr.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"derived") == 0) {
              goto IIfdB1;
            } /* IIvalfld */
          }
/* # line 1386 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIgetfldio((short *)0,1,32,39,dttype,(char *)"datatype");
            } /* IIfsetio */
          }
/* # line 1387 "attr.qsc" */	/* host code */
		STcopy(dttype, usertype.dbut_name);
		usertype.dbut_kind = DB_UT_NOTNULL;
		if (afe_vtychk(FEadfcb(), &usertype, &dbv) != OK)
		{
			IIUGerr(E_VF0007_not_valid_datatype,
				UG_ERR_ERROR, 1, usertype.dbut_name);
/* # line 1393 "attr.qsc" */	/* resume */
          {
            IIresfld((char *)"datatype");
            if (1) goto IIfdB1;
          }
/* # line 1394 "attr.qsc" */	/* host code */
		}
		if (!scr_check(nhdr, ntype))
		{
/* # line 1397 "attr.qsc" */	/* resume */
          {
            IIresfld((char *)"scrollsz");
            if (1) goto IIfdB1;
          }
/* # line 1398 "attr.qsc" */	/* host code */
		}
		if (vfretattr(nhdr, ntype, oldnm, writeout, fromtf) == FAIL)
		{
/* # line 1401 "attr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 1402 "attr.qsc" */	/* host code */
		}
# ifndef FORRBF
		if (fromtf)
		{
			curfield = nxtfattr((i4) NEXT, &nhdr, &ntype, curfield);
			if (curfield == -1)
			{
				nomoreattr();
				nomore = TRUE;
/* # line 1411 "attr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 1412 "attr.qsc" */	/* host code */
			}
		}
		else
# endif
		{
			if (attotflds == 1)
			{
				nomoreattr();
				nomore = TRUE;
/* # line 1421 "attr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 1422 "attr.qsc" */	/* host code */
			}
			nxregattr((i4) NEXT, &nhdr, &ntype);
		}
		STcopy(nhdr->fhdname, oldnm);
		attrinit(nhdr, ntype, fromtf, &colorval, der_buf, val_buf);
		vfputdtype(ntype);
/* # line 1428 "attr.qsc" */	/* resume */
          {
            IIresfld((char *)"vifattr");
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 7) {
        {
/* # line 1432 "attr.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"vifattr") == 0) {
              goto IIfdB1;
            } /* IIvalfld */
          }
/* # line 1433 "attr.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"nullable") == 0) {
              goto IIfdB1;
            } /* IIvalfld */
          }
/* # line 1434 "attr.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"derived") == 0) {
              goto IIfdB1;
            } /* IIvalfld */
          }
/* # line 1435 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIgetfldio((short *)0,1,32,39,dttype,(char *)"datatype");
            } /* IIfsetio */
          }
/* # line 1436 "attr.qsc" */	/* host code */
		STcopy(dttype, usertype.dbut_name);
		usertype.dbut_kind = DB_UT_NOTNULL;
		if (afe_vtychk(FEadfcb(), &usertype, &dbv) != OK)
		{
			IIUGerr(E_VF0007_not_valid_datatype,
				UG_ERR_ERROR, 1, usertype.dbut_name);
/* # line 1442 "attr.qsc" */	/* resume */
          {
            IIresfld((char *)"datatype");
            if (1) goto IIfdB1;
          }
/* # line 1443 "attr.qsc" */	/* host code */
		}
		if (!scr_check(nhdr, ntype))
		{
/* # line 1446 "attr.qsc" */	/* resume */
          {
            IIresfld((char *)"scrollsz");
            if (1) goto IIfdB1;
          }
/* # line 1447 "attr.qsc" */	/* host code */
		}
		if (vfretattr(nhdr, ntype, oldnm, writeout, fromtf) == FAIL)
		{
/* # line 1450 "attr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 1451 "attr.qsc" */	/* host code */
		}
# ifndef FORRBF
		if (fromtf)
		{
			curfield = nxtfattr((i4) PREVIOUS, &nhdr, &ntype,
				curfield);
			if (curfield == -1)
			{
				nomoreattr();
				nomore = TRUE;
/* # line 1461 "attr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 1462 "attr.qsc" */	/* host code */
			}
		}
		else
# endif
		{
			if (attotflds == 1)
			{
				nomoreattr();
				nomore = TRUE;
/* # line 1471 "attr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 1472 "attr.qsc" */	/* host code */
			}
			nxregattr((i4) PREVIOUS, &nhdr, &ntype);
		}
		STcopy(nhdr->fhdname, oldnm);
		attrinit(nhdr, ntype, fromtf, &colorval, der_buf, val_buf);
		vfputdtype(ntype);
/* # line 1478 "attr.qsc" */	/* resume */
          {
            IIresfld((char *)"vifattr");
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 8) {
        {
/* # line 1482 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIgetfldio((short *)0,1,32,fdVSTRLEN + 1-1,formula,(char *)
"valstr");
              IIgetfldio((short *)0,1,32,fdVMSGLEN + 1-1,msg,(char *)
"valmsg");
            } /* IIfsetio */
          }
/* # line 1483 "attr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vifvalder") != 0) {
              IIputfldio((char *)"valstr",(short *)0,1,32,0,formula);
              IIputfldio((char *)"valmsg",(short *)0,1,32,0,msg);
            } /* IIfsetio */
          }
/* # line 1484 "attr.qsc" */	/* host code */
		valderhdlr();
/* # line 1485 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifvalder") != 0) {
              IIgetfldio((short *)0,1,32,fdVSTRLEN + 1-1,formula,(char *)
"valstr");
              IIgetfldio((short *)0,1,32,fdVMSGLEN + 1-1,msg,(char *)
"valmsg");
            } /* IIfsetio */
          }
/* # line 1486 "attr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIputfldio((char *)"valstr",(short *)0,1,32,0,formula);
              IIputfldio((char *)"valmsg",(short *)0,1,32,0,msg);
            } /* IIfsetio */
          }
        }
      } else if (IIretval() == 9) {
        {
/* # line 1492 "attr.qsc" */	/* host code */
		IIUFfieldHelp();
        }
      } else if (IIretval() == 10) {
        {
/* # line 1497 "attr.qsc" */	/* host code */
		STcopy(ERget(S_VF000D_Field_Attributes), hlpbuf);
		FEhelp(VFH_ATTRIBUTE, hlpbuf);
        }
      } else if (IIretval() == 11) {
        {
/* # line 1502 "attr.qsc" */	/* host code */
		if (!nomore)
		{
/* # line 1504 "attr.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"vifattr") == 0) {
              goto IIfdB1;
            } /* IIvalfld */
          }
/* # line 1505 "attr.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"nullable") == 0) {
              goto IIfdB1;
            } /* IIvalfld */
          }
/* # line 1506 "attr.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"derived") == 0) {
              goto IIfdB1;
            } /* IIvalfld */
          }
/* # line 1507 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIgetfldio((short *)0,1,32,39,dttype,(char *)"datatype");
            } /* IIfsetio */
          }
/* # line 1508 "attr.qsc" */	/* host code */
			STcopy(dttype, usertype.dbut_name);
			usertype.dbut_kind = DB_UT_NOTNULL;
			if (afe_vtychk(FEadfcb(), &usertype, &dbv) != OK)
			{
				IIUGerr(E_VF0007_not_valid_datatype,
					UG_ERR_ERROR, 1, usertype.dbut_name);
/* # line 1514 "attr.qsc" */	/* resume */
          {
            IIresfld((char *)"datatype");
            if (1) goto IIfdB1;
          }
/* # line 1515 "attr.qsc" */	/* host code */
			}
			if (!scr_check(nhdr, ntype))
			{
/* # line 1518 "attr.qsc" */	/* resume */
          {
            IIresfld((char *)"scrollsz");
            if (1) goto IIfdB1;
          }
/* # line 1519 "attr.qsc" */	/* host code */
			}
			if (vfretattr(nhdr, ntype, oldnm, writeout,
				fromtf) == FAIL)
			{
/* # line 1523 "attr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 1524 "attr.qsc" */	/* host code */
			}
		}
/* # line 1526 "attr.qsc" */	/* enddisplay */
          {
            if (1) goto IIfdF1;
          }
        }
      } else {
        if (1) goto IIfdE1;
      } /* IIretval */
    } /* IIrunform */
IIfdF1:
    if (IIchkfrm() == 0) goto IIfdB1;
    goto IIfdE1;
IImuI1:
    if (IIactscrl((char *)"vifattr",0,1) == 0) goto IIfdE1;
    if (IIactscrl((char *)"vifattr",1,1) == 0) goto IIfdE1;
    if (IIFRafActFld(ERx("color"),0,2) == 0) goto IIfdE1;
    if (IIFRafActFld(ERx("datatype"),0,3) == 0) goto IIfdE1;
    if (IIFRafActFld(ERx("scrollable"),0,4) == 0) goto IIfdE1;
    if (IIFRafActFld(ERx("derived"),0,5) == 0) goto IIfdE1;
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Next),(char *)0,2,2,6) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Previous),(char *)0,2,2,7) == 0) goto IIfdE1;
    if (IInmuact(ERget(F_VF006B_MoreEdit),(char *)0,2,2,8) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,9) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(10,(char *)0,0,0,9) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),(char *)0,0,0,10) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,0,10) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_End),(char *)0,2,2,11) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,11) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 1528 "attr.qsc" */	/* host code */
	if (!fromtf)
	{
		globy = (*pslist2)->ps_begy;
		globx = (*pslist2)->ps_begx;
	}
	FTclear();
}
/*{
** Name:	vfndeftype - Get default data type.
**
** Description:
**	Get the default datatype type information for a field
**	that is being created.	The format string is used to
**	derived the default datatype.  If the string is empty
**	or datatype information is already present, do nothing.
**
**	The derived datatype is placed into form "vifattr".
**
** Inputs:
**	hdr	Pointer to a FLDHDR structure for the new field.
**	type	Pointer to a FLDTYPE structure for the new field.
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
**	None.
**
** History:
**	03/27/87 (dkh) -Initial version written.
*/
VOID
vfndeftype(hdr, type)
FLDHDR	*hdr;
FLDTYPE *type;
{
  char *dptr;
	DB_USER_TYPE	usertype;
	DB_DATA_VALUE	dbv;
  char *llyes1;
  char *llno1;
	llyes1 = IIVF_yes1;
	llno1 = IIVF_no1;
	if (vfdml == FEDMLSQL)
	{
/* # line 1582 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"nullable",(short *)0,1,32,0,llyes1);
    } /* IIfsetio */
  }
/* # line 1583 "attr.qsc" */	/* host code */
		attrdtype.dbut_kind = DB_UT_NULL;
	}
	else
	{
/* # line 1587 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"nullable",(short *)0,1,32,0,llno1);
    } /* IIfsetio */
  }
/* # line 1588 "attr.qsc" */	/* host code */
		attrdtype.dbut_kind = DB_UT_NOTNULL;
	}
/* # line 1590 "attr.qsc" */	/* clear */
  {
    IIfldclear((char *)"datatype");
  }
/* # line 1591 "attr.qsc" */	/* host code */
	attrdtype.dbut_name[0] = EOS;
	if (type->ftdatatype == DB_NODT && type->ftfmt == NULL)
	{
		return;
	}
	if (type->ftdatatype != DB_NODT)
	{
		dbv.db_datatype = type->ftdatatype;
		dbv.db_length = type->ftlength;
		dbv.db_prec = type->ftprec;
	}
	else
	{
		if (fmt_ftot(FEadfcb(), type->ftfmt, &dbv) != OK)
		{
			IIUGerr(E_VF000E_Error_in_getting_defa,
				UG_ERR_ERROR, 0);
			return;
		}
	}
	if (afe_vtyoutput(FEadfcb(), &dbv, &usertype) != OK)
	{
		IIUGerr(E_VF000F_Error_in_converting_d, UG_ERR_ERROR, 0);
		return;
	}
	dptr = usertype.dbut_name;
	STcopy(dptr, attrdtype.dbut_name);
/* # line 1618 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"datatype",(short *)0,1,32,0,dptr);
    } /* IIfsetio */
  }
/* # line 1619 "attr.qsc" */	/* host code */
	if (usertype.dbut_kind == DB_UT_NULL)
	{
		dptr = IIVF_yes1;
	}
	else
	{
		dptr = IIVF_no1;
	}
/* # line 1627 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"nullable",(short *)0,1,32,0,dptr);
    } /* IIfsetio */
  }
/* # line 1628 "attr.qsc" */	/* host code */
	attrdtype.dbut_kind = usertype.dbut_kind;
}
nattrCom(fd, writeout)
FIELD	*fd;
bool	writeout;
{
	FLDHDR		*hdr;
	FLDTYPE		*type;
	i4		i;
	i4		colorval;
  i4 bfldcolor;
  i4 bcolor;
	ATTRIBS		*attr;
  char *ptr;
  i4 browno;
	char		buf[BUFSIZ];
  char i4tr[30];
  char bbuf[40];
  char nflag[5];
	DB_DATA_VALUE	dbv;
	DB_USER_TYPE	usertype;
  char dbuf[50];
	i4		*flags;
  char *local_no1 = IIVF_no1;
  char derval[2];
  char dvstr[fdVSTRLEN + 1];
  char vmsg[fdVMSGLEN + 1];
  char *chr;
  char *chr2;
  char der_str[fdVSTRLEN + 1];
  char val_str[fdVSTRLEN + 1];
	char		old_name[FE_MAXNAME + 1];
	allocAttr();
	hdr = FDgethdr(fd);
	type = FDgettype(fd);
	FTclear();
	vfdmpoff();
/* # line 1671 "attr.qsc" */	/* display */
  {
    if (IIdispfrm(ERx("vifattr"),(char *)"u") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    {
/* # line 1674 "attr.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
          IIstfsio(54,(char *)"defval",0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
        } /* IIiqset */
      }
/* # line 1675 "attr.qsc" */	/* host code */
		attrinit(hdr, type, FALSE, &colorval, der_str, val_str);
		vfndeftype(hdr, type);
		can_scroll = FALSE;
		scrlfld = FALSE;
/* # line 1679 "attr.qsc" */	/* putform */
      {
        if (IIfsetio((char *)0) != 0) {
          IIputfldio((char *)"scrollable",(short *)0,1,32,0,local_no1);
          IIputfldio((char *)"scrollsz",(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
        } /* IIfsetio */
      }
/* # line 1680 "attr.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
          IIstfsio(54,(char *)"scrollable",0,(short *)0,1,30,sizeof(1),
          (void *)IILQint(1));
          IIstfsio(54,(char *)"scrollsz",0,(short *)0,1,30,sizeof(1),
          (void *)IILQint(1));
        } /* IIiqset */
      }
/* # line 1682 "attr.qsc" */	/* host code */
		old_name[0] = EOS;
		if (hdr->fhdname != NULL)
		{
			STcopy(hdr->fhdname, old_name);
		}
    }
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 1690 "attr.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 1694 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,30,4,&bfldcolor,(char *)"color");
            } /* IIfsetio */
          }
/* # line 1695 "attr.qsc" */	/* host code */
		if (colorval != bfldcolor)
		{
			vfsetcolor(&colorhdr->fhdflags, bfldcolor);
			colorval = bfldcolor;
			FTsetda(vfattr, colorseq, FT_UPDATE, FT_REGFLD,
				0, 0, colorhdr->fhdflags);
		}
		else
		{
/* # line 1704 "attr.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB2;
          }
/* # line 1705 "attr.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 3) {
        {
/* # line 1709 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIgetfldio((short *)0,1,32,39,bbuf,(char *)"datatype");
            } /* IIfsetio */
          }
/* # line 1710 "attr.qsc" */	/* host code */
		STcopy(bbuf, usertype.dbut_name);
		usertype.dbut_kind = DB_UT_NOTNULL;
		if (afe_vtychk(FEadfcb(), &usertype, &dbv) != OK)
		{
			IIUGerr(E_VF0007_not_valid_datatype, UG_ERR_ERROR,
				1, usertype.dbut_name);
/* # line 1716 "attr.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
/* # line 1717 "attr.qsc" */	/* host code */
		}
/* # line 1718 "attr.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB2;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 1722 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,1,derval,(char *)"derived");
            } /* IIfsetio */
          }
/* # line 1723 "attr.qsc" */	/* host code */
	    if (*derval == *IIVF_yes1)
	    {
		if (der_fld == FALSE)
		{
		    der_fld = TRUE;
/* # line 1728 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,fdVSTRLEN + 1-1,val_str,(char *)
"valstr");
            } /* IIfsetio */
          }
/* # line 1729 "attr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIputfldio((char *)"valstr",(short *)0,1,32,0,der_str);
            } /* IIfsetio */
          }
/* # line 1730 "attr.qsc" */	/* host code */
		}
		chr = ERget(F_VF0069_Der_Form);
		chr2 = ERget(F_VF006D_ValNotAllowed);
/* # line 1733 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"valmsg",0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 1734 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifvalder",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"valmsg",0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 1735 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"defval",0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 1736 "attr.qsc" */	/* host code */
	    }
	    else	/* *derval == *IIVF_no1 */
	    {
		if (der_fld == TRUE)
		{
		    der_fld = FALSE;
/* # line 1742 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,fdVSTRLEN + 1-1,der_str,(char *)
"valstr");
            } /* IIfsetio */
          }
/* # line 1743 "attr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIputfldio((char *)"valstr",(short *)0,1,32,0,val_str);
            } /* IIfsetio */
          }
/* # line 1744 "attr.qsc" */	/* host code */
		}
		chr = ERget(F_VF006A_Val_Chk);
		chr2 = ERget(F_VF006C_ValAllowed);
/* # line 1747 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"valmsg",0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 1748 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifvalder",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"valmsg",0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 1749 "attr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"defval",0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 1750 "attr.qsc" */	/* host code */
	    }
/* # line 1751 "attr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIputfldio((char *)"valstrtitle",(short *)0,1,32,0,chr);
            } /* IIfsetio */
          }
/* # line 1752 "attr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vifvalder") != 0) {
              IIputfldio((char *)"valstrtitle",(short *)0,1,32,0,chr);
            } /* IIfsetio */
          }
/* # line 1753 "attr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIputfldio((char *)"expl_text",(short *)0,1,32,0,chr2);
            } /* IIfsetio */
          }
/* # line 1754 "attr.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB2;
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 1758 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIgetfldio((short *)0,1,32,fdVSTRLEN + 1-1,dvstr,(char *)
"valstr");
              IIgetfldio((short *)0,1,32,fdVMSGLEN + 1-1,vmsg,(char *)
"valmsg");
            } /* IIfsetio */
          }
/* # line 1759 "attr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vifvalder") != 0) {
              IIputfldio((char *)"valstr",(short *)0,1,32,0,dvstr);
              IIputfldio((char *)"valmsg",(short *)0,1,32,0,vmsg);
            } /* IIfsetio */
          }
/* # line 1760 "attr.qsc" */	/* host code */
		valderhdlr();
/* # line 1761 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifvalder") != 0) {
              IIgetfldio((short *)0,1,32,fdVSTRLEN + 1-1,dvstr,(char *)
"valstr");
              IIgetfldio((short *)0,1,32,fdVMSGLEN + 1-1,vmsg,(char *)
"valmsg");
            } /* IIfsetio */
          }
/* # line 1762 "attr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIputfldio((char *)"valstr",(short *)0,1,32,0,dvstr);
              IIputfldio((char *)"valmsg",(short *)0,1,32,0,vmsg);
            } /* IIfsetio */
          }
        }
      } else if (IIretval() == 6) {
        {
          char locfld[FE_MAXNAME+1];
/* # line 1769 "attr.qsc" */	/* host code */
		FLD_VALUE	fv;
		fv._form = ERx("vifattr");
		fv._field = locfld;
		fv._column = NULL;
/* If this is field "datatype", offer a list of data types (kibro01) b119305 */
/* # line 1776 "attr.qsc" */	/* inquire_frs */
          {
            if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,locfld,22,(char *)0,0);
            } /* IIiqset */
          }
/* # line 1777 "attr.qsc" */	/* host code */
		if ( STequal(locfld, ERx("datatype")) )
		{
			IIUFhlpType( NULL, 0, FALSE, &fv);
		} else
		{
			IIUFfieldHelp();
		}
        }
      } else if (IIretval() == 7) {
        {
/* # line 1788 "attr.qsc" */	/* host code */
		FEhelp(VFH_ATTRIBUTE, ERget(S_VF000D_Field_Attributes));
        }
      } else if (IIretval() == 8) {
        {
/* # line 1792 "attr.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"vifattr") == 0) {
              goto IIfdB2;
            } /* IIvalfld */
          }
/* # line 1793 "attr.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"nullable") == 0) {
              goto IIfdB2;
            } /* IIvalfld */
          }
/* # line 1794 "attr.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"derived") == 0) {
              goto IIfdB2;
            } /* IIvalfld */
          }
/* # line 1795 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIgetfldio((short *)0,1,32,39,bbuf,(char *)"datatype");
            } /* IIfsetio */
          }
/* # line 1796 "attr.qsc" */	/* host code */
		STcopy(bbuf, usertype.dbut_name);
		usertype.dbut_kind = DB_UT_NOTNULL;
		/*
		**  If we started out with no datatype, then allow
		**  no datatype.
		*/
		if (!(bbuf[0] == EOS && attrdtype.dbut_name[0] == EOS))
		{
			if (afe_vtychk(FEadfcb(), &usertype, &dbv) != OK)
			{
				IIUGerr(E_VF0007_not_valid_datatype,
					UG_ERR_ERROR, 1, usertype.dbut_name);
/* # line 1808 "attr.qsc" */	/* resume */
          {
            IIresfld((char *)"datatype");
            if (1) goto IIfdB2;
          }
/* # line 1809 "attr.qsc" */	/* host code */
			}
		}
		/*
		** clear all field attributes except those that can
		** obtain values outside of the edit attribute form.
		*/
		hdr->fhdflags &= fdREVDIR;
		hdr->fhd2flags &= (fdSCRLFD | fdVQLOCK);
		i = 0;
		attr = attrflag;
/* # line 1819 "attr.qsc" */	/* unloadtable */
          {
            if (IItbact((char *)"vifattr",(char *)"vifattr",0) == 0) goto 
            IItbE2;
            while (IItunload() != 0) {
              IItcogetio((short *)0,1,32,4,nflag,(char *)"sett");
              IITBceColEnd();
              {
/* # line 1821 "attr.qsc" */	/* host code */
			if (i < RMAXATTR)
			{
				if (attr->use_fhd2flags)
					flags = &hdr->fhd2flags;
				else
					flags = &hdr->fhdflags;
				charToFlag(flags, nflag, attr->attribute);
				attr++;
				i++;
			}
			else
			{
/* # line 1833 "attr.qsc" */	/* endloop */
                {
                  if (1) goto IItbE2;
                }
/* # line 1834 "attr.qsc" */	/* host code */
			}
              }
            } /* IItunload */
IItbE2:
            IItunend();
          }
/* # line 1837 "attr.qsc" */	/* host code */
		if ((hdr->fhd2flags & fdREADONLY)
		    && (hdr->fhdflags & fdQUERYONLY))
		{
			IIUGerr(E_VF0004_Cant_set_both, UG_ERR_ERROR, 0);
/* # line 1841 "attr.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
/* # line 1842 "attr.qsc" */	/* host code */
		}
		if ((hdr->fhdflags & fdLOWCASE) && (hdr->fhdflags & fdUPCASE))
		{
			IIUGerr(E_VF0005_Cant_set_low_up, UG_ERR_ERROR, 0);
/* # line 1846 "attr.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
/* # line 1847 "attr.qsc" */	/* host code */
		}
		ptr = buf;
/* # line 1851 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIgetfldio((short *)0,1,32,0,ptr,(char *)"valstr");
            } /* IIfsetio */
          }
/* # line 1852 "attr.qsc" */	/* host code */
		type->ftvalstr = saveStr(ptr);
/* # line 1854 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIgetfldio((short *)0,1,32,4,nflag,(char *)"derived");
            } /* IIfsetio */
          }
/* # line 1855 "attr.qsc" */	/* host code */
		charToFlag(&hdr->fhd2flags, nflag, fdDERIVED);
		if (hdr->fhd2flags & fdDERIVED)
		{
			if (type->ftvalstr[0] == EOS)
			{
				hdr->fhd2flags &= ~fdDERIVED;
			}
			else if ((hdr->fhdflags & fdMAND)
				|| (hdr->fhdflags & fdLOWCASE)
				|| (hdr->fhdflags & fdUPCASE)
				|| (hdr->fhdflags & fdQUERYONLY)
				|| (hdr->fhd2flags & fdREADONLY))
			{
				IIUGerr(E_VF012C_Cant_set_derived_and,
					UG_ERR_ERROR, 0);
				/* Turn off the offending attributes. */
				hdr->fhdflags
					&= ~(fdMAND | fdLOWCASE | fdUPCASE | fdQUERYONLY);
				/*
				** Turn on the (internal) disponly flag if
				** derived.  Used by FRS code to prevent
				** user access to this field.
				*/
				hdr->fhd2flags |= fdREADONLY;
			}
			else
			{
				/*
				** Turn on the (internal) disponly flag if
				** derived.
				*/
				hdr->fhd2flags |= fdREADONLY;
			}
		}
/* # line 1893 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIgetfldio((short *)0,1,32,0,ptr,(char *)"defval");
            } /* IIfsetio */
          }
/* # line 1894 "attr.qsc" */	/* host code */
		if (hdr->fhd2flags & fdDERIVED)
		{
			/* No default values for derived fields. */
			ptr[0] = EOS;
		}
		type->ftdefault = saveStr(ptr);
/* # line 1901 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIgetfldio((short *)0,1,32,0,ptr,(char *)"valmsg");
            } /* IIfsetio */
          }
/* # line 1902 "attr.qsc" */	/* host code */
		if (hdr->fhd2flags & fdDERIVED)
		{
			/* No validation error message for derived fields. */
			ptr[0] = EOS;
		}
		type->ftvalmsg = saveStr(ptr);
		ptr = hdr->fhdname;
/* # line 1910 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIgetfldio((short *)0,1,32,0,ptr,(char *)"name");
            } /* IIfsetio */
          }
/* # line 1912 "attr.qsc" */	/* host code */
		/*
		**  Allow empty name if we started with empty name.
		*/
		if (!(old_name[0] == EOS && *ptr == EOS))
		{
			if (!vffnmchk(ptr, ERget(F_VF0011_field)))
			{
/* # line 1919 "attr.qsc" */	/* resume */
          {
            IIresfld((char *)"name");
            if (1) goto IIfdB2;
          }
/* # line 1920 "attr.qsc" */	/* host code */
			}
		}
		CVlower(hdr->fhdname);
/* # line 1925 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIgetfldio((short *)0,1,30,4,&bcolor,(char *)"color");
            } /* IIfsetio */
          }
/* # line 1926 "attr.qsc" */	/* host code */
		vfsetcolor(&hdr->fhdflags, bcolor);
		/*
		**  Get and check datatype/nullability from form.
		*/
/* # line 1931 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIgetfldio((short *)0,1,32,49,dbuf,(char *)"datatype");
            } /* IIfsetio */
          }
/* # line 1933 "attr.qsc" */	/* host code */
		/*
		**  Let datatype be blank if we started with
		**  blank as well.
		*/
		if (dbuf[0] == '\0' && attrdtype.dbut_name[0] == '\0')
		{
			type->ftdatatype = DB_NODT;
		}
		else
		{
			STcopy(dbuf, usertype.dbut_name);
/* # line 1944 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIgetfldio((short *)0,1,32,49,dbuf,(char *)"nullable");
            } /* IIfsetio */
          }
/* # line 1945 "attr.qsc" */	/* host code */
			CVlower(dbuf);
			if (*dbuf == *IIVF_yes1)
			{
				usertype.dbut_kind = DB_UT_NULL;
			}
			else
			{
				usertype.dbut_kind = DB_UT_NOTNULL;
			}
			if (afe_vtychk(FEadfcb(), &usertype, &dbv) != OK)
			{
				IIUGerr(E_VF0007_not_valid_datatype,
					UG_ERR_ERROR, 1, usertype.dbut_name);
				ptr = attrdtype.dbut_name;
				if (*ptr != '\0')
				{
/* # line 1961 "attr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIputfldio((char *)"datatype",(short *)0,1,32,0,ptr);
            } /* IIfsetio */
          }
/* # line 1962 "attr.qsc" */	/* host code */
				}
/* # line 1963 "attr.qsc" */	/* resume */
          {
            IIresfld((char *)"datatype");
            if (1) goto IIfdB2;
          }
/* # line 1964 "attr.qsc" */	/* host code */
			}
			/*
			**  Put in a fake length if db_length
			**  is zero (i.e., one of the character
			**  datatypes.
			*/
			if (dbv.db_length == 0)
			{
				dbv.db_length = 10;
				/*
				**  If we have a decimal datatype, we also want
				**  to fake up a precision/scale combo value
				**  as well.
				*/
				if (abs(dbv.db_datatype) == DB_DEC_TYPE)
				{
					DB_DATA_VALUE	dbv1;
					dbv.db_length =
						DB_PREC_TO_LEN_MACRO(10);
					dbv.db_prec =
						DB_PS_ENCODE_MACRO(10, 0);
					(void)adc_lenchk(FEadfcb(), TRUE,
						&dbv, &dbv1);
					dbv.db_length = dbv1.db_length;
				}
			}
			type->ftdatatype = dbv.db_datatype;
			type->ftlength = dbv.db_length;
			type->ftprec = dbv.db_prec;
		}
/* # line 1998 "attr.qsc" */	/* enddisplay */
          {
            if (1) goto IIfdF2;
          }
        }
      } else {
        if (1) goto IIfdE2;
      } /* IIretval */
    } /* IIrunform */
IIfdF2:
    if (IIchkfrm() == 0) goto IIfdB2;
    goto IIfdE2;
IImuI2:
    if (IIactscrl((char *)"vifattr",0,1) == 0) goto IIfdE2;
    if (IIactscrl((char *)"vifattr",1,1) == 0) goto IIfdE2;
    if (IIFRafActFld(ERx("color"),0,2) == 0) goto IIfdE2;
    if (IIFRafActFld(ERx("datatype"),0,3) == 0) goto IIfdE2;
    if (IIFRafActFld(ERx("derived"),0,4) == 0) goto IIfdE2;
    if (IIinitmu() == 0) goto IIfdE2;
    if (IInmuact(ERget(F_VF006B_MoreEdit),(char *)0,2,2,5) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,6) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(10,(char *)0,0,0,6) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Help),(char *)0,0,0,7) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,0,7) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_End),(char *)0,2,2,8) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,8) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 2000 "attr.qsc" */	/* host code */
	FTclear();
}
# endif /* FORRBF */
char *
flgToChar(flag, val)
i4	flag;
i4	val;
{
	if (flag & val)
		return(IIVF_yes1);
	else
		return(IIVF_no1);
}
VOID
charToFlag(flag, ch, val)
i4	*flag;
char	*ch;
i4	val;
{
	if (STcompare(ch, IIVF_yes1) == 0)
		*flag |= val;
}
# ifndef FORRBF
vffnmchk(name, fldtype)
char	*name;
char	*fldtype;
{
	i4	retval = TRUE;
	char	*namestart;
	char	badchar[5];
	namestart = name;
	if (*name == EOS)
	{
		IIUGerr(E_VF0051_empty, UG_ERR_ERROR, 2, fldtype, fldtype);
		return(FALSE);
	}
	if (!CMnmstart(name))
	{
		IIUGerr(E_VF0010_First_character_of_fi, UG_ERR_ERROR, 
			2, fldtype, namestart);
		return(FALSE);
	}
	while(*name)
	{
		if (!CMnmchar(name))
		{
			CMcpychar(name,badchar);
			IIUGerr(E_VF0011_Character_not_ok, UG_ERR_ERROR,
				3, badchar, fldtype, namestart);
			retval = FALSE;
			break;
		}
		CMnext(name);
	}
	return(retval);
}
# endif /* FORRBF */
/*{
** Name:	vfgetcolor	- get color number from flags 
**
** Description:
**	This function translates a form system (frame.h) fdCOLOR 
**	flag bit definition into a form color number (0-7) which
**	is used on forms to represent the color number.
**
** Inputs:
**	i4	*flags; - pointer to form system flags as in frame.h
**	i4	color;	- form derived color number  (0-7)
**
** Outputs:
**
**	Returns:
**		i4	- color number to be displayed on a form
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	05/10/88  (tom) - comment written
*/
i4
vfgetcolor(flags)
i4	flags;
{
	if (flags & fd1COLOR)
		return 1;
	else if (flags & fd2COLOR)
		return 2;
	else if (flags & fd3COLOR)
		return 3;
	else if (flags & fd4COLOR)
		return 4;
	else if (flags & fd5COLOR)
		return 5;
	else if (flags & fd6COLOR)
		return 6;
	else if (flags & fd7COLOR)
		return 7;
	else 
		return 0;
}
/*{
** Name:	vfsetcolor	- set flags from color number (0-7)
**
** Description:
**	This function translates the vifred form color number (0-7)
**	into the approriate fdCOLOR bit flags and posts the flag
**	a pointer to which is passed in as an argument.
**
** Inputs:
**	i4	*flags; - pointer to form system flags as in frame.h
**	i4	color;	- form derived color number  (0-7)
**
** Outputs:
**
**	Returns:
**		none 
**
**	Exceptions:
**
** Side Effects:
**	*flags is posted
**
** History:
**	05/10/88  (tom) - comment written
*/
vfsetcolor(flags, color)
i4	*flags;
i4	color;
{
	static i4  mask[8] = 
	{ 	0, fd1COLOR, fd2COLOR, fd3COLOR, fd4COLOR, 
		fd5COLOR, fd6COLOR, fd7COLOR 
	};
	if (color < 0 || color > 7)
		return;
	*flags &= ~fdCOLOR;
	*flags |= mask[color];
}
# ifndef FORRBF
/*{
** Name:	scr_check - Scrollability Check.
**
** Description:
**	Do sanity check on scrollability for a field/column.
**	If field is scrollable, then check datatype, scroll
**	size for validity.
**
**	This routine is being called as part of exiting the Edit Attributes
**	frame, so reset scrollable and scrollsz settings for derived fields.
**
** Inputs:
**	hdr	FLDHDR structure for field/column.
**	type	FLDTYPE structure for field/column.
**
** Outputs:
**
**	Returns:
**		TRUE	If sanity check is OK.
**		FALSE	Sanity check failed.
**
**	Exceptions:
**		None.
**
** Side Effects:
**	Field may be flagged as a scrollable field and data length may
**	also be set to the scroll length.
**
** History:
**	05/21/88 (dkh) - Initial version.
*/
static bool
scr_check(hdr, type)
FLDHDR	*hdr;
FLDTYPE	*type;
{
  char tbuf[40];
  char nbuf[2];
  char derbuf[2];
  i4 scrlength;
	DB_USER_TYPE	usertype;
	DB_DATA_VALUE	dbv;
	DB_DATA_VALUE	sdbv;
	i4		class;
	i4		intern_length;
  char *loc_no1;
	i4		maxlen;
	if (!can_scroll)
	{
		return(TRUE);
	}
	loc_no1 = IIVF_no1;
	/*
	**  Get datatype so we can see if we have to turn off scrollability.
	**  Note that the "nullable" field has already been checked
	**  before this routine is called.
	*/
/* # line 2219 "attr.qsc" */	/* getform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIgetfldio((short *)0,1,32,39,tbuf,(char *)"datatype");
      IIgetfldio((short *)0,1,32,1,nbuf,(char *)"nullable");
      IIgetfldio((short *)0,1,32,1,derbuf,(char *)"derived");
    } /* IIfsetio */
  }
/* # line 2220 "attr.qsc" */	/* host code */
	CVlower(nbuf);
	/* If derived, simply zap scrollability. */
	if (*derbuf == *IIVF_yes1)
	{
/* # line 2225 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"scrollable",(short *)0,1,32,0,loc_no1);
      IIputfldio((char *)"scrollsz",(short *)0,1,30,sizeof(0),(void *)IILQint(
      0));
    } /* IIfsetio */
  }
/* # line 2226 "attr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"scrollable",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"scrollsz",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 2228 "attr.qsc" */	/* host code */
	    scrlfld = FALSE;
	    hdr->fhd2flags &= ~fdSCRLFD;
	    return(TRUE);
	}
	STcopy(tbuf, usertype.dbut_name);
	if (*nbuf == *IIVF_yes1)
	{
		usertype.dbut_kind = DB_UT_NULL;
	}
	else
	{
		usertype.dbut_kind = DB_UT_NOTNULL;
	}
	if ((afe_tychk(FEadfcb(), &usertype, &dbv) != OK) ||
		(!IIAFfedatatype(&dbv)))
	{
		IIUGerr(E_VF0007_not_valid_datatype, UG_ERR_ERROR, 1,
			usertype.dbut_name);
		return(FALSE);
	}
	IIAFddcDetermineDatatypeClass(dbv.db_datatype, &class);
	/*
	**  If datatype is no longer a string, reset world and
	**  return good status.
	*/
	if (class != CHAR_DTYPE)
	{
/* # line 2257 "attr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIputfldio((char *)"scrollable",(short *)0,1,32,0,loc_no1);
      IIputfldio((char *)"scrollsz",(short *)0,1,30,sizeof(0),(void *)IILQint(
      0));
    } /* IIfsetio */
  }
/* # line 2258 "attr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vifattr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"scrollable",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"scrollsz",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 2260 "attr.qsc" */	/* host code */
		scrlfld = FALSE;
		hdr->fhd2flags &= ~fdSCRLFD;
		return(TRUE);
	}
	/*
	**  Check if scrollability has been turned off.  If so, return.
	*/
/* # line 2268 "attr.qsc" */	/* getform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIgetfldio((short *)0,1,32,39,tbuf,(char *)"scrollable");
    } /* IIfsetio */
  }
/* # line 2269 "attr.qsc" */	/* host code */
	if (*tbuf == *IIVF_no1)
	{
		scrlfld = FALSE;
		hdr->fhd2flags &= ~fdSCRLFD;
		return(TRUE);
	}
	else if (*tbuf != *IIVF_yes1)
	{
		/* Not a 'y' or 'n'.  This is an error.  */
		IIUGerr(E_VF011C_bad_scroll_val, UG_ERR_ERROR, 0);
		return(FALSE);
	}
	/*
	**  Datatype has already been validated as a string.  Need to check it
	**  against the scroll length.
	*/
/* # line 2286 "attr.qsc" */	/* getform */
  {
    if (IIfsetio((char *)"vifattr") != 0) {
      IIgetfldio((short *)0,1,30,4,&scrlength,(char *)"scrollsz");
    } /* IIfsetio */
  }
/* # line 2288 "attr.qsc" */	/* host code */
	/*
	**  Now check length for datatype to make sure that's OK too.
	*/
	maxlen = DB_GW4_MAXSTRING;
	dbv.db_length = scrlength;
	dbv.db_prec = 0;	/* Character datatype. */
	if (adc_lenchk(FEadfcb(), TRUE, &dbv, &sdbv) != E_DB_OK)
	{
		/* Invalid number for datatype.  */
		IIUGerr(E_VF0164_bad_scroll_len, UG_ERR_ERROR, 2,
		    &fmt_length, &maxlen);
		return(FALSE);
	}
	intern_length = sdbv.db_length;
	/*
	**  Finally, check length against the format length.
	**  If it is too small give an error.
	*/
	if (fmt_length > scrlength)
	{
		IIUGerr(E_VF0164_bad_scroll_len, UG_ERR_ERROR, 2,
		    &fmt_length, &maxlen);
		return(FALSE);
	}
	/*
	**  Everything appears to be OK, set scroll flag and length.
	*/
	scrsize = intern_length;
	hdr->fhd2flags |= fdSCRLFD;
	return(TRUE);
}
/*{
** Name:	valderhdlr	- Display loop for vifvalder frame.
**
** Description:
**	Run the display of the vifvalder frame.  This frame allows the
**	user to set the validation string or derivation formula for a field.
**
** Inputs:
**
** Outputs:
**
**	Returns:
**		VOID
**
**	Exceptions:
**		None
**
** Side Effects:
**	None
**
** History:
**	21-jun-89 (bruceb)	Written.
*/
static VOID
valderhdlr()
{
  char drived[2];
    /* Display form in update mode to save values set by calling routines. */
/* # line 2352 "attr.qsc" */	/* display */
  {
    if (IIdispfrm((char *)"vifvalder",(char *)"u") == 0) goto IIfdE3;
    goto IImuI3;
IIfdI3:;
IIfdB3:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 2355 "attr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vifattr") != 0) {
              IIgetfldio((short *)0,1,32,1,drived,(char *)"derived");
            } /* IIfsetio */
          }
/* # line 2356 "attr.qsc" */	/* host code */
	if (*drived == *IIVF_yes1)
	    FEhelp(VFH_DERIVE, ERget(S_VF013A_Field_Derivations));
	else
	    FEhelp(VFH_VALID, ERget(S_VF0137_Field_Validations));
        }
      } else if (IIretval() == 2) {
        {
/* # line 2363 "attr.qsc" */	/* enddisplay */
          {
            if (1) goto IIfdF3;
          }
        }
      } else {
        if (1) goto IIfdE3;
      } /* IIretval */
    } /* IIrunform */
IIfdF3:
    if (IIchkfrm() == 0) goto IIfdB3;
    goto IIfdE3;
IImuI3:
    if (IIinitmu() == 0) goto IIfdE3;
    if (IInmuact(ERget(FE_Help),(char *)0,2,2,1) == 0) goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,1) == 0) goto IIfdE3;
    if (IInmuact(ERget(FE_End),(char *)0,2,2,2) == 0) goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,2) == 0) goto IIfdE3;
    if (IIendmu() == 0) goto IIfdE3;
    goto IIfdI3;
IIfdE3:;
    IIendfrm();
  }
/* # line 2365 "attr.qsc" */	/* host code */
}
# endif /* FORRBF */
