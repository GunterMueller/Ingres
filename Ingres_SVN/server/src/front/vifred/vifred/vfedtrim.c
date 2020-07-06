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
# include	<er.h>
# include	"decls.h"
# include	<ug.h>
# include	<frsctblk.h>
# include	<st.h>
# include	<me.h>
# include	<vt.h>
# include       <lo.h>
# include	"vfhlp.h"
# include	"ervf.h"
# include	"uf.h"
/**
** Name:	vfedtrim.qsc -	trim edit code
**
** Description:
**	This file contains functions to control a user's editing of a piece
**	of trim.  Users can edit the text of the trim, or specify attributes
**	for that trim.
**
**	IIVFteTrimEdit	allow modification of the trim or its attributes
**	IIVFtaTrimAttr	change the attributes of a piece of trim
**
** History:
**	06-sep-89 (bruceb)	Written.
**		Written.  (Much swiped from vfedbox.qsc.)
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**	29-may-1997 (canor01)
**	    Clean up compiler warnings from CL prototypes.
**      02-Jul-1999 (hweho01)
**              Included uf.h header file, it contains the function     
**              prototype of IIUFlcfLocateForm(). Without the explicit
**              declaration, the default int return value for a function 
**              will truncate a 64-bit address on ris_u64 platform. 
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
# define	MAX_TRIM_ATTR	4
static	char	*trimattrnames[MAX_TRIM_ATTR];
static	i4	trimattrflgs[] =
{
	fdRVVID,
	fdBLINK,
	fdUNLN,
	fdCHGINT
};
/* field header so we can change the color of the color field on the fly */
static	FLDHDR	*colorhdr;
static	i4	colorseq;		/* field sequence number */
FUNC_EXTERN	char	*flgToChar();
FUNC_EXTERN	VOID	charToFlag();
FUNC_EXTERN	FRAME	*IIfindfrm();
FUNC_EXTERN	FIELD	*FDfind();
FUNC_EXTERN	FLDHDR	*IIFDgfhGetFldHdr();
VOID		IIVFtaTrimAttr();
static	VOID	allocTrimAttr();
static	bool	trimfrm_alloc = FALSE;
static	FRAME	*vftrmatr = NULL;
/* Name of both the edit trim attributes form, and the form's table field. */
  static char *name = ERx("vftrmatr");
/*{
** Name:	IIVFteTrimEdit	- edit a piece of trim
**
** Description:
**	The user has just selected edit from the main edit menu while
**	positioned on a piece of textual trim.  Allow editing of either
**	the text or the attributes.  Ending from either of those choices
**	exits back to the main menu.
**
** Inputs:
**	ps	- position structure of the trim
**
** Outputs:
**
**	Returns:
**
**	Exceptions:
**		None
**
** Side Effects:
**
** History:
**	06-sep-89 (bruceb)	Written
*/
VOID
IIVFteTrimEdit(ps)
POS	*ps;	
{
    char	hlpbuf[100];
    if (!trimfrm_alloc)
    {
	allocTrimAttr();
	trimfrm_alloc = TRUE;
    }
    setGlobUn(edTRIM, ps, NULL);
    for (;;)
    {
/* # line 125 "vfedtrim.qsc" */	/* submenu */
  {
    goto IImuI1;
IIfdB1:
    IIrunmu(0);
    if (IIretval() == 1) {
      {
/* # line 128 "vfedtrim.qsc" */	/* host code */
	    scrmsg(ERget(F_VF001B_Edit_trim));
	    editTrim(ps);
	    break;
      }
    } else if (IIretval() == 2) {
      {
/* # line 134 "vfedtrim.qsc" */	/* host code */
	    IIVFtaTrimAttr(ps);
	    break;
      }
    } else if (IIretval() == 3) {
      {
/* # line 139 "vfedtrim.qsc" */	/* host code */
	    STcopy(ERget(S_VF0144_Edit_Trim), hlpbuf);
	    vfhelp(VFH_EDTRIM, hlpbuf);
      }
    } else if (IIretval() == 4) {
      {
/* # line 144 "vfedtrim.qsc" */	/* host code */
	    break;
      }
    } else {
      if (1) goto IIfdE1;
    } /* IIretval */
    goto IIfdE1;
IImuI1:
    IImuonly();
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Trim),(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget(F_VF0038_Attr),(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),(char *)0,2,2,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact(ERget(F_VF0018_End),(char *)0,2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdB1;
IIfdE1:;
  }
/* # line 146 "vfedtrim.qsc" */	/* host code */
    }
    setGlMove(ps);
    vfTrigUpd();
    vfUpdate(globy, globx, TRUE);
} 
/*{
** Name:	IIVFtaTrimAttr	- change the attributes of a piece of trim
**
** Description:
**	Display a form allowing the user to change the color and 
**	attributes of a piece of trim.
**
** Inputs:
**	ps;	- position structure of the trim
**
** Outputs:
**
**	Returns:
**
**	Exceptions:
**		None
**
** Side Effects:
**
** History:
**	06-sep-89 (bruceb)	Written
*/
VOID
IIVFtaTrimAttr(ps)
POS	*ps;
{
  char flgbuf[5];
  char *p;
  i4 curcolor;
  i4 i;
  i4 oldcolor;
  i4 flags;
  char uflag[6];
    char	hlpbuf[100];
    TRIM	*tr;
/* # line 189 "vfedtrim.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 191 "vfedtrim.qsc" */	/* display */
  {
    if (IIdispfrm(name,(char *)"u") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    {
/* # line 194 "vfedtrim.qsc" */	/* inittable */
      {
        if (IItbinit(name,name,(char *)"u") != 0) {
          IItfill();
        } /* IItbinit */
      }
/* # line 196 "vfedtrim.qsc" */	/* host code */
	tr = (TRIM *)ps->ps_feat;
	flags = tr->trmflags;
	for (i = 0; i < MAX_TRIM_ATTR; i++) 
	{ 
	    STcopy(flgToChar(flags, trimattrflgs[i]), flgbuf);
	    p = trimattrnames[i];
/* # line 203 "vfedtrim.qsc" */	/* loadtable */
      {
        if (IItbact(name,name,1) != 0) {
          IItcoputio((char *)"attr",(short *)0,1,32,0,p);
          IItcoputio((char *)"sett",(short *)0,1,32,0,flgbuf);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 204 "vfedtrim.qsc" */	/* host code */
	}
	oldcolor = curcolor = vfgetcolor(flags); 
	vfsetcolor(&colorhdr->fhdflags, curcolor);
/* # line 207 "vfedtrim.qsc" */	/* putform */
      {
        if (IIfsetio(name) != 0) {
          IIputfldio((char *)"color",(short *)0,1,30,4,&curcolor);
        } /* IIfsetio */
      }
/* # line 208 "vfedtrim.qsc" */	/* putform */
      {
        if (IIfsetio(name) != 0) {
          IIputfldio((char *)"colortitle",(short *)0,1,32,0,
          ERget(F_VF0010_Color));
        } /* IIfsetio */
      }
/* # line 209 "vfedtrim.qsc" */	/* host code */
	FTsetda(vftrmatr, colorseq, FT_UPDATE, FT_REGFLD, 0, 0,
	    colorhdr->fhdflags);
    }
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 215 "vfedtrim.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 219 "vfedtrim.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,30,4,&curcolor,(char *)"color");
            } /* IIfsetio */
          }
/* # line 220 "vfedtrim.qsc" */	/* host code */
	if (curcolor != oldcolor)
	{
	    vfsetcolor(&colorhdr->fhdflags, curcolor);
	    FTsetda(vftrmatr, colorseq, FT_UPDATE, FT_REGFLD, 0, 0,
		colorhdr->fhdflags);
	    oldcolor = curcolor;
	}
	else
	{
/* # line 229 "vfedtrim.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB2;
          }
/* # line 230 "vfedtrim.qsc" */	/* host code */
	}
        }
      } else if (IIretval() == 3) {
        {
/* # line 236 "vfedtrim.qsc" */	/* host code */
	IIUFfieldHelp();
        }
      } else if (IIretval() == 4) {
        {
/* # line 240 "vfedtrim.qsc" */	/* host code */
	STcopy(ERget(S_VF0143_Trim_Attributes), hlpbuf);
	FEhelp(VFH_TRMATTR, hlpbuf);
        }
      } else if (IIretval() == 5) {
        {
/* # line 245 "vfedtrim.qsc" */	/* validate */
          {
            if (IIvalfld(name) == 0) {
              goto IIfdB2;
            } /* IIvalfld */
          }
/* # line 246 "vfedtrim.qsc" */	/* host code */
	flags = i = 0;
/* # line 247 "vfedtrim.qsc" */	/* unloadtable */
          {
            if (IItbact(name,name,0) == 0) goto IItbE1;
            while (IItunload() != 0) {
              IItcogetio((short *)0,1,32,5,uflag,(char *)"sett");
              IITBceColEnd();
              {
/* # line 249 "vfedtrim.qsc" */	/* host code */
	    if (i < MAX_TRIM_ATTR)
	    {
		charToFlag(&flags, uflag, trimattrflgs[i]);
		i++;
	    }
	    else
	    {
/* # line 256 "vfedtrim.qsc" */	/* endloop */
                {
                  if (1) goto IItbE1;
                }
/* # line 257 "vfedtrim.qsc" */	/* host code */
	    }
              }
            } /* IItunload */
IItbE1:
            IItunend();
          }
/* # line 259 "vfedtrim.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,30,4,&curcolor,(char *)"color");
            } /* IIfsetio */
          }
/* # line 260 "vfedtrim.qsc" */	/* host code */
	vfsetcolor(&flags, curcolor);
	tr->trmflags = flags;
/* # line 262 "vfedtrim.qsc" */	/* enddisplay */
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
    if (IIactscrl(name,0,1) == 0) goto IIfdE2;
    if (IIactscrl(name,1,1) == 0) goto IIfdE2;
    if (IIFRafActFld(ERx("color"),0,2) == 0) goto IIfdE2;
    if (IIinitmu() == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,3) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(10,(char *)0,0,0,3) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Help),(char *)0,2,2,4) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,2,4) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_End),(char *)0,2,2,5) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,5) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 264 "vfedtrim.qsc" */	/* host code */
    FTclear();
}
static VOID
allocTrimAttr()
{
	REGFLD	*dummy;
	FIELD	*fld;
	FRAME	*tframe;
	FDsetparse(FALSE);
	if (IIUFgtfGetForm(IIUFlcfLocateForm(), name) != OK)
	{
		syserr(ERget(S_VF0001_Can_not_access_form), name);
	}
	FDsetparse(TRUE);
	/*
	**  Workaround to get things running.  Not sure why this
	**  is needed.
	*/
	tframe = IIfindfrm(name);
	fld = FDfind((i4 **)tframe->frfld, ERx("colortitle"),
		tframe->frfldno, IIFDgfhGetFldHdr);
	colorseq = FDgtfldno(FT_UPDATE, tframe, fld, &dummy);
	colorhdr = FDgethdr(fld);
	vftrmatr = tframe;
	trimattrnames[0] = ERget(F_VF0004_Reverse_Video);
	trimattrnames[1] = ERget(F_VF0005_Blinking);
	trimattrnames[2] = ERget(F_VF0006_Underline);
	trimattrnames[3] = ERget(F_VF0007_Brightness_Change);
}
