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
# include	<lo.h>
# include	"vfhlp.h"
# include	"ervf.h"
# include	"uf.h"
/**
** Name:	vfedbox.qsc -	box edit code
**
** Description:
**	This file contains functions to supervise the user's editing
**	of a box. This involves allowing the user to resize the box
**	via a visual sequence. And also we allow the attributes/color
**	of the box to be set.
**
**	vfBoxEdit	display all features of the form up to given line #
**	vfBoxResize	change the size of a box
**	vfBoxAttr	change the attributes of a box
**
** History:
**	04/19/88  (tom) - created.
**	08/22/88 (dkh) - Fixed venus bug found by trashathon.
**	10/03/88 (tom) - Fixed bug involving not being able to tab
**			out of the color field.
**	10/28/88 (dk) - Performance changes.
**	12/31/88 (dkh) - Perf changes.
**	24-apr-89 (bruceb)
**		No longer any nonseq fields; search on the regular field list.
**	05/17/89 (dkh) - Fixed bug 3757.
**	21-jun-89 (bruceb)
**		Zapped the 'End of Attributes' line from the table field.
**	19-sep-89 (bruceb)
**		User no longer returned to the submenu when editing box
**		trim after resizing or changing attributes (as per FRC.)
**	12/05/89 (dkh) - Changed interface to VTcursor() so VTLIB could
**			 be moved into ii_framelib shared lib.
**	09-may-90 (bruceb)
**		Call VTcursor() with new arg indicating whether there is
**		'not' a menu associated with this call.
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**	29-may-1997 (canor01)
**	    Cleaned up compiler warnings from CL prototypes.
**      02-Jul-1999 (hweho01)
**          Included uf.h header file, it contains the function     
**          prototype of IIUFlcfLocateForm(). Without the explicit
**          declaration, the default int return value for a  
**          function will truncate a 64-bit address on ris_u64 
**          platform.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
# define MAXBOXATR	3 
static char	*boxatrnames[3];
static i4	boxatrflgs[] =
{
	fdRVVID,
	fdBLINK,
	fdCHGINT
};
static FLDHDR *colorhdr;		/* field header so we can change 
				the color of the color field on the fly */
static i4  colorseq;		/* field sequence number */
static FRAME	*vfbatr = NULL;
FUNC_EXTERN	char	*flgToChar();
FUNC_EXTERN	VOID	charToFlag();
FUNC_EXTERN	FRAME	*IIfindfrm();
FUNC_EXTERN	FIELD	*FDfind();
FUNC_EXTERN	FLDHDR	*IIFDgfhGetFldHdr();
FUNC_EXTERN	VOID	allocBoxAttr();
FUNC_EXTERN	i4	VFfeature();
bool	vfBoxResize();
static	bool	boxfrm_alloc = FALSE;
/*{
** Name:	vfBoxEdit	- edit a box
**
** Description:
**	The user has just selected edit from the main edit 
**	menu, while positioned on a box.
**
** Inputs:
**	POS *ps;	- position structure of the box
**
** Outputs:
**
**	Returns:
**		nothing
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	03/26/87  (tom) - created
*/
VOID
vfBoxEdit(ps)
POS	*ps;	
{
	char	hlpbuf[200];
	if (!boxfrm_alloc)
	{
		allocBoxAttr();
		boxfrm_alloc = TRUE;
	}
	setGlobUn(edBOX, ps, 0);
	for (;;)
	{
		/* display box as hilighted */
		vfdisplay(ps, PBOX, TRUE);
		VTxydraw(frame, globy, globx);
/* # line 142 "vfedbox.qsc" */	/* submenu */
  {
    goto IImuI1;
IIfdB1:
    IIrunmu(0);
    if (IIretval() == 1) {
      {
/* # line 145 "vfedbox.qsc" */	/* host code */
			/* display box as unhilighted for resize,
			   the thought is that there are too many 
			   cursorlike blinking spots..*/
			vfdisplay(ps, PBOX, FALSE);
			VTxydraw(frame, globy, globx);
			/* did we resize the box out of existance */
			if (vfBoxResize(ps) == FALSE)
			{
				vfTrigUpd();
				return;
			}
			break;
      }
    } else if (IIretval() == 2) {
      {
/* # line 161 "vfedbox.qsc" */	/* host code */
			vfBoxAttr(ps);
			break;
      }
    } else if (IIretval() == 3) {
      {
/* # line 166 "vfedbox.qsc" */	/* host code */
			STcopy(ERget(S_VF0136_EDITING_BOX_LINE), hlpbuf);
			vfhelpsub(VFH_EDBOX, hlpbuf, NULL);
      }
    } else if (IIretval() == 4) {
      {
/* # line 171 "vfedbox.qsc" */	/* host code */
			break;
      }
    } else {
      if (1) goto IIfdE1;
    } /* IIretval */
    goto IIfdE1;
IImuI1:
    IImuonly();
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(F_VF0046_Resize),(char *)0,2,2,1) == 0) goto IIfdE1;
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
/* # line 173 "vfedbox.qsc" */	/* host code */
	}
	setGlMove(ps);
	vfTrigUpd();
	vfUpdate(globy, globx, TRUE);
} 
/*{
** Name:	vfBoxResize	- let user resize a box 
**
** Description:
**	Supervise resizing a box. First the user is allowed 
**	to move the lower right corner.. upon the tab key
**	we rotate the moveable corner around the box clockwise.
**
** Inputs:
**	POS	*ps;	- pos struct of the box we are resizing 
**
** Outputs:
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
**	04/21/88  (tom) - created
*/
bool
vfBoxResize(ps)
POS	*ps;
{
	i4		poscount;
	FLD_POS		*posarray = NULL;
	FRS_EVCB	evcb;
	bool		ret = FALSE;
	i4		y_anchor, x_anchor;
	i4		y_rover, x_rover;
	i4		movecorner = 0; /* the lower right corner.. 
				corners are numbered clockwise starting here */
	vfposarray(&poscount, &posarray);
	for (;;)
	{
		/* setup for corner 0, lower right corner */
		y_anchor = ps->ps_begy;	 
		y_rover = ps->ps_endy;
		x_anchor = ps->ps_begx;
		x_rover = ps->ps_endx;
		/* now if we were not on corner 0.. 
 				then exchange appropriately */
		switch (movecorner % 4)
		{
		case 1:
			exChange(&x_anchor, &x_rover);
			break;
		case 2:
			exChange(&x_anchor, &x_rover);
			/* fall thru */
		case 3:
			exChange(&y_anchor, &y_rover);
			break;
		}
		scrmsg(ERget(S_VF0124_Mv_to_new_corner));
		MEfill(sizeof(FRS_EVCB), '\0', &evcb);
		evcb.event = fdNOCODE;
		VTflashplus(frame, y_anchor, x_anchor, 1);
	    AGAIN:
		switch (VTcursor(frame, &y_rover, &x_rover, 
			endFrame - 1, endxFrm,
			poscount, posarray, &evcb, VFfeature,
			RBF, FALSE, TRUE, FALSE, TRUE))
		{
		case fdopMENU:
			VTflashplus(frame, 0, 0, 0);
			break;
		case fdopNEXT:
			++movecorner;
			VTflashplus(frame, 0, 0, 0);
			continue;
		default:
			goto AGAIN;
		}
		/* first we get rid of the old box */
		vfersPos(ps);	/* erase the old box from the screen */
		unLinkPos(ps);	/* remove position struct from line table */
		/* if rover is not on the anchor point..  */
		if (y_anchor != y_rover || x_anchor != x_rover) 
		{
			if (y_rover < y_anchor)	/* adjust orientation */
				exChange(&y_anchor, &y_rover);
			if (x_rover < x_anchor)
				exChange(&x_anchor, &x_rover);
			ps->ps_begy = y_anchor;
			ps->ps_begx = x_anchor;
			ps->ps_endy = y_rover;
			ps->ps_endx = x_rover;
			insPos(ps, FALSE); 
			vfTrigUpd();
			ret = TRUE;
		} 
		else	/* rover is on anchor.. pretend it's deleted */
		{
			setGlobUn(dlBOX, ps, 0);
			setGlMove(ps);
		}
		break;
	}
	/* call for update.. but pass updflg == FALSE because
	   our caller would like to display the current field
	   hilighted and we don't want to update user's screen until then */
	vfUpdate(globy = y_anchor, globx = x_anchor, FALSE);
	MEfree(posarray);
	return (ret);
}
/*{
** Name:	vfBoxAttr	- let the user change the attributes
**				  of a box.
**
** Description:
**	Display a form allowing the user to change the color and 
**	attributes of a box.
**
** Inputs:
**	POS	*ps;	- pos struct of the box we are attributing
**
** Outputs:
**	Returns:
**		none
**	Exceptions:
**		none
** Side Effects:
**
** History:
**	04/21/88  (tom) - created
*/
vfBoxAttr(ps)
POS	*ps;
{
  char flgbuf[5], *p;
  i4 curcolor, i;
  i4 oldcolor;
  i4 flags;
  char uflag[6];
	char	hlpbuf[100];
/* # line 334 "vfedbox.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 336 "vfedbox.qsc" */	/* display */
  {
    if (IIdispfrm(ERx("vfboxatr"),(char *)"u") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    {
/* # line 339 "vfedbox.qsc" */	/* inittable */
      {
        if (IItbinit((char *)"vfboxatr",(char *)"vfboxatr",(char *)"u") != 0) 
        {
          IItfill();
        } /* IItbinit */
      }
/* # line 341 "vfedbox.qsc" */	/* host code */
		flags = *(i4*)ps->ps_feat;
		for (i = 0; i < MAXBOXATR; i++) 
		{ 
			STcopy(flgToChar(flags, boxatrflgs[i]), flgbuf);
			p = boxatrnames[i];
/* # line 347 "vfedbox.qsc" */	/* loadtable */
      {
        if (IItbact((char *)"vfboxatr",(char *)"vfboxatr",1) != 0) {
          IItcoputio((char *)"attr",(short *)0,1,32,0,p);
          IItcoputio((char *)"sett",(short *)0,1,32,0,flgbuf);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 348 "vfedbox.qsc" */	/* host code */
		}
		oldcolor = curcolor = vfgetcolor(flags); 
		vfsetcolor(&colorhdr->fhdflags, curcolor);
/* # line 351 "vfedbox.qsc" */	/* putform */
      {
        if (IIfsetio(ERx("vfboxatr")) != 0) {
          IIputfldio((char *)"color",(short *)0,1,30,4,&curcolor);
        } /* IIfsetio */
      }
/* # line 352 "vfedbox.qsc" */	/* putform */
      {
        if (IIfsetio(ERx("vfboxatr")) != 0) {
          IIputfldio((char *)"colortitle",(short *)0,1,32,0,
          ERget(F_VF0010_Color));
        } /* IIfsetio */
      }
/* # line 353 "vfedbox.qsc" */	/* host code */
		FTsetda(vfbatr, colorseq, FT_UPDATE, FT_REGFLD, 0, 0,
			colorhdr->fhdflags);
    }
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 359 "vfedbox.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 363 "vfedbox.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,30,4,&curcolor,(char *)"color");
            } /* IIfsetio */
          }
/* # line 364 "vfedbox.qsc" */	/* host code */
		if (curcolor != oldcolor)
		{
			vfsetcolor(&colorhdr->fhdflags, curcolor);
			FTsetda(vfbatr, colorseq, FT_UPDATE, FT_REGFLD, 0, 0,
				colorhdr->fhdflags);
			oldcolor = curcolor;
		}
		else
		{
/* # line 373 "vfedbox.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB2;
          }
/* # line 374 "vfedbox.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 3) {
        {
/* # line 380 "vfedbox.qsc" */	/* host code */
		IIUFfieldHelp();
        }
      } else if (IIretval() == 4) {
        {
/* # line 384 "vfedbox.qsc" */	/* host code */
		STcopy(ERget(S_VF0132_Box_Attributes), hlpbuf);
		FEhelp(VFH_BOXATTR, hlpbuf);
        }
      } else if (IIretval() == 5) {
        {
/* # line 389 "vfedbox.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"vfboxatr") == 0) {
              goto IIfdB2;
            } /* IIvalfld */
          }
/* # line 390 "vfedbox.qsc" */	/* host code */
		flags = i = 0;
/* # line 391 "vfedbox.qsc" */	/* unloadtable */
          {
            if (IItbact((char *)"vfboxatr",(char *)"vfboxatr",0) == 0) goto 
            IItbE1;
            while (IItunload() != 0) {
              IItcogetio((short *)0,1,32,5,uflag,(char *)"sett");
              IITBceColEnd();
              {
/* # line 393 "vfedbox.qsc" */	/* host code */
			if (i < MAXBOXATR)
			{
				charToFlag(&flags, uflag, boxatrflgs[i]);
				i++;
			}
			else
			{
/* # line 400 "vfedbox.qsc" */	/* endloop */
                {
                  if (1) goto IItbE1;
                }
/* # line 401 "vfedbox.qsc" */	/* host code */
			}
              }
            } /* IItunload */
IItbE1:
            IItunend();
          }
/* # line 403 "vfedbox.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,30,4,&curcolor,(char *)"color");
            } /* IIfsetio */
          }
/* # line 404 "vfedbox.qsc" */	/* host code */
		vfsetcolor(&flags, curcolor);
		*(i4*)ps->ps_feat = flags;
/* # line 406 "vfedbox.qsc" */	/* enddisplay */
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
    if (IIactscrl((char *)"vfboxatr",0,1) == 0) goto IIfdE2;
    if (IIactscrl((char *)"vfboxatr",1,1) == 0) goto IIfdE2;
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
/* # line 408 "vfedbox.qsc" */	/* host code */
	FTclear();
}
VOID
allocBoxAttr()
{
	REGFLD	*dummy;
	FIELD	*fld;
	FRAME	*tframe;
	FDsetparse(FALSE);
	if (IIUFgtfGetForm(IIUFlcfLocateForm(), ERx("vfboxatr")) != OK)
	{
		syserr(ERget(S_VF0001_Can_not_access_form), ERx("vfboxatr"));
	}
	FDsetparse(TRUE);
	/*
	**  Workaround to get things running.  Not sure why this
	**  is needed.
	*/
	tframe = IIfindfrm(ERx("vfboxatr"));
	fld = FDfind((i4 **)tframe->frfld, ERx("colortitle"),
		tframe->frfldno, IIFDgfhGetFldHdr);
	colorseq = FDgtfldno(FT_UPDATE, tframe, fld, &dummy);
	colorhdr = FDgethdr(fld);
	vfbatr = tframe;
	boxatrnames[0] = ERget(F_VF0004_Reverse_Video);
	boxatrnames[1] = ERget(F_VF0005_Blinking);
	boxatrnames[2] = ERget(F_VF0007_Brightness_Change);
}
