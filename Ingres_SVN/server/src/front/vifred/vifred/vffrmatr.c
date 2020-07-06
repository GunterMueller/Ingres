# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
*/
# include       <compat.h>
# include	<nm.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include       <fe.h>
# include       "decls.h"
# include       <me.h>
# include       <st.h>
# include       <si.h>
# include       <ug.h>
# include       <er.h>
# include       <vt.h>
# include       <cv.h>
# include       <ds.h>
# include       <feds.h>
# include       <frsctblk.h>
# include	<lo.h>
# include       "ervf.h"
# include       "vfhlp.h"
# include       "uf.h"
/**
** Name:        vffrmatr.qsc -  vifred form attribute changing 
**
** Description:
**      functions to process changing form attributes such as popup
**      style forms etc.
**
** Defines:
**      vfWhersCurs     - report screen cursor position to the user
**      vfformattr      - allow user to change form attributes
**      vfChkPopup      - if frame dimensions do not allow popup.. then
**				issue error, and convert form to fullscreen.
**
** History:
**      04/07/88  (tom) - created for Venus project.
**	07/21/88 (dkh) - Added new hooks to make "help" work.
**	10/28/88 (dkh) - Performance changes.
**	30-mar-89 (bruceb)	Fix for bug 5197.
**		Call clearLine() before (rather than after) the resizing
**		work.  Vifred relies on each modification routine to
**		clear the position masks before use.
**	28-apr-89 (bruceb)
**		Changed to make use of invisible fields rather than modifying
**		the form's mode.
**	08/07/89 (dkh) - Added support for 80/132 runtime switching.
**	09/23/89 (dkh) - Porting changes integration.
**	10/01/89 (dkh) - Code cleanup.
**	10-oct-89 (mgw)
**		Backed out 9/23 change. See comment in types.h for details.
**	12/05/89 (dkh) - Changed interface to VTcursor() so VTLIB could
**			 be moved into ii_framelib shared lib.
**	08-dec-89 (bruceb)
**		Only change 'screen width' field to invisible if successful
**		in switch to popup form.
**	12/15/89 (dkh) - VMS shared lib changes.
**	01/12/90 (dkh) - Put in some changes to not set noChanges if
**			 user didn't really do anything to the form attributes.
**	17-jan-90 (bruceb)
**		opostype variable wasn't always initialized.  Fix that.
**	07-mar-90 (bruceb)
**		Lint:  Add 5th param to FTaddfrs() calls.
**	30-mar-90 (bruceb)
**		Force Vifred menu display back to first menuitem.
**	05-apr-90 (bruceb)
**		Call VTgetScrCurs() with endxFrm so that the column returned
**		accounts for getting shifted by vfcursor() off the End-of-Form
**		trim.
**	09-may-90 (bruceb)
**		Call VTcursor() with new arg indicating whether there is
**		'not' a menu associated with this call.
**      06/12/90 (esd) - Tighten up the meaning of the global variable
**                       endxFrm.  In the past, it was usually (but
**                       not always) assumed that in a 3270 environment,
**                       the form included a blank column on the right,
**                       so that endxFrm was the 0-relative column
**                       number of that blank column at the right edge.
**                       I am removing that assumption:  endxFrm will
**                       now typically be the 0-relative column number
**                       of the rightmost column containing a feature.
**      06/12/90 (esd) - When calculating how much space is required
**                       for border, take into account attribute bytes
**                       between vertical sides of enclosing box and
**                       the form proper.
**      06/15/90 (esd) - Allow user to change whether or not a form
**                       needs space for attribute bytes.
**      06/22/90 (esd) - Fix bug: When style or postype or border
**                       were activated, even without being changed,
**                       changes to other fields were being discarded.
**	07/05/90 (dkh) - Fixed up popup displays on top of layout frame to
**			 look and behave like normal popups.
**	05-sep-90 (bruceb)
**		After calling IIFTsffaStretchFormForAttrs(), check that the
**		form is either fullscreen, or still small enough to be pop-up.
**      09/12/90 (esd) - Make attrbytes field invisible unless
**                       logical symbol II_ATTRBYTES_VISIBLE is 'yes'.
**	06/27/92 (dkh) - Added support for rulers.
**	09/24/92 (dkh) - Removed attribute byte stuff since 3270 support
**			 is no longer needed.
**	02/24/93 (dkh) - Fixed bug 49854.  Fixed code to turn off undo
**			 mechanism when a resize is performed.  This is
**			 the same behavior as in 6.4.
**	11/10/93 (dkh) - Fixed bug 55571.  Set IIVFxh_disp to FALSE when
**			 editing form attributes since display of straight
**			 edges are temporarily disabled while the form
**			 attributes frame is active.  This keeps everything
**			 in sync so that other parts of vifred don't try
**			 to manipulate non-existent straight edges when the
**			 form has been resized by the user.
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**      15-jan-1996 (toumi01; from 1.1 axp_osf port)
**              Added kchin's changes (from 6.4) for axp_osf)
**              10/15/93 (kchin)
**              Cast argument to PTR when calling FEmsg(), this
**              is due to the change of datatype of variable parameters
**              to PTR in FEmsg() routine.  The change here is made in
**              vfWhersCurs().
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
/* # define's */
/* GLOBALDEF's */
/* extern's */
GLOBALREF       FILE   		*vfdfile;	
FUNC_EXTERN	VOID	FTfrminvis();
FUNC_EXTERN	VOID	FTfrmvis();
FUNC_EXTERN	VOID	IIVTlf();
FUNC_EXTERN	i4	VFfeature();
GLOBALREF	FT_TERMINFO	IIVFterminfo;
/* static's */
static  bool    ispopup;        
static  bool    isfixed;
static  bool    isborder;
  static char *fullscreentxt;
  static char *popuptxt;
  static char *fixedtxt;
  static char *floatingtxt;
  static i4 nrows;
  static i4 ncols;
static	bool	frmattr_alloc = FALSE;
static  bool    attrbytes_visible = FALSE;
static  bool    attrbytes_visible_set = FALSE;
static	bool	fattr_chg = FALSE;
static  i4      vfChkFrmFit();
static  bool    vfMakePopup();
static  bool    vffatr_sub();
static	VOID	vfBotResize();
static	bool	space_for_attr;
/*{
** Name:	vfChkPopup -  check if form can remain a popup
**
** Description:
**		Whenever a user action causes the form to expand, 
**		this function is called to see if it can still be 
**		a popup (assuming it is one). If we cannot make
**		it a popup then we give an error message and 
**		convert the form to fullscreen.
**
**		Note that this should never occur during execution of the
**		code in this file (visual adjust) because we are
**		always limiting to the screen.  NO LONGER true.  Now also
**		called when stretching the form for attribute bytes.
**		
**
** Inputs:
**
** Outputs:
**	none
**
**	Returns:
**
**	Exceptions:
**
** Side Effects:
**		Can convert the edited form back to full screen mode.
**
** History:
**	06/22/88 (tom) - written
*/
vfChkPopup()
{
	i4     row;
	i4     col;
	i4     bord;
	if ((frame->frmflags & fdISPOPUP) == FALSE)
	{
		return;	
	}
	/* convert flag into # of lines consumed */
	bord = (frame->frmflags & fdBOXFR) ? 2 : 0;
	if (frame->frposy)	/* is it fixed position? */
	{
		row = frame->frposy - 1; 
		col = frame->frposx - 1; 
	}
	else	/* if not assume the most lenient position.. 0,0 */
	{
		row = col = 0;
	}
	if (    (row + endFrame + bord > IIVFterminfo.lines - 1)
	   ||   (col + (endxFrm + 1) + bord * (1 + IIVFsfaSpaceForAttr)
		    > IIVFterminfo.cols )
	   )
	{
		IIUGerr(E_VF0135_Back_to_fullscreen, UG_ERR_ERROR, 0);
		frame->frmflags &= ~(fdBOXFR | fdISPOPUP);
		frame->frposy = frame->frposx = 0;
	}
}
/*{
** Name:        vfWhersCurs - Report cursor position
** 
** Description: 
**	Report cursor position to the user.
**
** Inputs:
**      none
**
** Outputs:
**
**      Returns:
**              none
**
**      Exceptions:
**
** Side Effects:
**
** History:
**      04/08/88  (tom) - created.
*/
vfWhersCurs()
{
	i4	row;
	i4	col;
	VTgetScrCurs(endxFrm, &row, &col);
	VTflashplus(frame, globy, globx, 1);
	FEmsg(ERget(S_VF012d_CurPosStr), TRUE, (PTR)(row + 1), (PTR)(col + 1));
	VTflashplus(frame, 0, 0, 0);
}
/*{
** Name:        vfformattr - user has selected FormAttr from cursor menu.
** 
** Description: 
**      User has invoked form attribute option on the cursor menu.  
**      This function supervises the popup form which allows setting
**      basic form style, position and border attributes.
**      The function vffatr_sub runs the display loop to process
**      the form for setting attributes. This is called as long as
**      it keeps returning TRUE.  This is done because it is
**      necessary to break the display loop in order to update
**      the vifred edit window if visual adjustment is selected
**      and if we adjust the window size in order to make 
**      a fullscreen form into a popup. 
**
** Inputs:
**      none
**
** Outputs:
**
**      Returns:
**              none
**      Exceptions:
**		none
** Side Effects:
**
** History:
**      04/08/88  (tom) - created.
*/
vfformattr()
{
	bool    callback;
	if (!frmattr_alloc)
	{
		allocFrmAttr();
		frmattr_alloc = TRUE;
	}
	fattr_chg = FALSE;
	space_for_attr = IIVFsfaSpaceForAttr;
	for (callback = 1; callback;)
	{
		IIVTlf(frame, TRUE, FALSE);
		callback = vffatr_sub();
		if (globy > endFrame)
			globy = endFrame;
		if (globx > endxFrm + 1 + IIVFsfaSpaceForAttr)
			globx = endxFrm + 1 + IIVFsfaSpaceForAttr;
/* # line 325 "vffrmatr.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 326 "vffrmatr.qsc" */	/* host code */
		IIVTlf(frame, FALSE, FALSE);
		vfTrigUpd();
		vfUpdate(globy, globx, TRUE);
	}
	/*
	** forget previously pending undo cause too many things might
	** have changed.. such that the undo may no longer be relevent
	*/
	if (fattr_chg)
	{
		setGlobUn(unNONE, (POS*) NULL, (i4*) NULL);
	}
}
/*{
** Name:        vffatr_sub - worker subroutine for vfformattr
** 
** Description: 
**      User has invoked form attribute option on the cursor menu.  
**      This function supervises the popup form which allows setting
**      basic form style, position and border attributes.
**
** Inputs:
**      none
**
** Outputs:
**
**      Returns:
**              bool    - TRUE means to call me back right away 
**                        FALSE  means no need to call me back i'm done
**
**      Exceptions:
**
** Side Effects:
**
** History:
**      04/08/88  (tom) - created.
**      06/15/90  (esd) - add activation logic for field 'attrbytes'.
*/
static bool
vffatr_sub()
{
	i4     i;
	i4     retval = FALSE;
	char    *frname;
  char *narrow;
  char *wide;
  char *cur;
  char *iptr;
  char buf[100];
  i4 pos;
	char	*ostyle;
	char	*opostype;
	i4	oisborder;
	i4	oposy = 0;
	i4	oposx = 0;
	char	*oscrsz;
	bool	xh_disp_local = FALSE;
/* # line 388 "vffrmatr.qsc" */	/* display */
  {
    if (IIdispfrm((char *)"vffrmatr",(char *)"u") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 391 "vffrmatr.qsc" */	/* host code */
		/*
		** get the text strings corresponding to the possible
		** field entry values
		*/                   
		fullscreentxt = ERget(F_VF004f_fullscreen);
		popuptxt = ERget(F_VF0050_popup);
		fixedtxt = ERget(F_VF0051_fixed);
		floatingtxt = ERget(F_VF0052_floating);
		narrow = ERget(F_VF0072_Narrow);
		wide = ERget(F_VF0073_Wide);
		cur = ERget(F_VF0074_Current);
		oisborder = isborder = (frame->frmflags & fdBOXFR) != 0;
		ispopup = (frame->frmflags & fdISPOPUP) != 0;
		isfixed = (ispopup == TRUE) ?  frame->frposy != 0 :  FALSE;
		nrows = endFrame;
		ncols = endxFrm + 1;
		frname = (frame->frname[0] == EOS)
			? ERget(F_VF0045_blank)
			: frame->frname;
		STprintf(buf, ERget(F_VF004e_Frmatr_title), frname);
/* # line 414 "vffrmatr.qsc" */	/* putform */
      {
        if (IIfsetio((char *)"vffrmatr") != 0) {
          IIputfldio((char *)"title",(short *)0,1,32,0,buf);
        } /* IIfsetio */
      }
/* # line 415 "vffrmatr.qsc" */	/* host code */
		if (! attrbytes_visible_set)
		{
		    NMgtAt(ERx("II_ATTRBYTES_VISIBLE"), &iptr);
		    attrbytes_visible = IIUGyn(iptr,(STATUS *)NULL);
		    attrbytes_visible_set = TRUE;
		}
		if (attrbytes_visible)
		{
		    iptr = (space_for_attr)
			? ERget(F_UG0001_Yes1)
			: ERget(F_UG0006_No1);
/* # line 426 "vffrmatr.qsc" */	/* putform */
      {
        if (IIfsetio((char *)"vffrmatr") != 0) {
          IIputfldio((char *)"attrbytes",(short *)0,1,32,0,iptr);
        } /* IIfsetio */
      }
/* # line 427 "vffrmatr.qsc" */	/* host code */
		}
		else
		{
/* # line 430 "vffrmatr.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
          IIstfsio(54,(char *)"attrbytes",0,(short *)0,1,30,sizeof(1),
          (void *)IILQint(1));
        } /* IIiqset */
      }
/* # line 431 "vffrmatr.qsc" */	/* host code */
		}
		vfPutFrmAtr();          
		/*
		**  If cross hairs are displayed we will just
		**  drop them here so they don't get in the way
		**  if the user wants to turn a fullscreen into
		**  a popup or if user wants to resize a popup.
		**
		**  They will be put back before we exit.
		*/
		if (IIVFxh_disp)
		{
			vfersPos(IIVFhcross);
			vfersPos(IIVFvcross);
			unLinkPos(IIVFhcross);
			unLinkPos(IIVFvcross);
			/*
			**  Note that display of straight edges
			**  was actually on.
			*/
			xh_disp_local = TRUE;
			/*
			**  Reset control variable since we disabled
			**  the straight edges above.
			*/
			IIVFxh_disp = FALSE;
		}
		if (ispopup)
		{
			oscrsz = ERx("");
/* # line 465 "vffrmatr.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
          IIstfsio(54,(char *)"screen_size",0,(short *)0,1,30,sizeof(1),
          (void *)IILQint(1));
        } /* IIiqset */
      }
/* # line 466 "vffrmatr.qsc" */	/* host code */
			oposy = frame->frposy;
			oposx = frame->frposx;
			ostyle = popuptxt;
			if (isfixed)
			{
				opostype = fixedtxt;
			}
			else
			{
				opostype = floatingtxt;
			}
		}
		else
		{
			opostype = ERx("");
			ostyle = fullscreentxt;
/* # line 482 "vffrmatr.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
          IIstfsio(54,(char *)"screen_size",0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
        } /* IIiqset */
      }
/* # line 483 "vffrmatr.qsc" */	/* host code */
			if (frame->frmflags & fdNSCRWID)
			{
				oscrsz = narrow;
				STcopy(narrow, buf);
			}
			else if (frame->frmflags & fdWSCRWID)
			{
				oscrsz = wide;
				STcopy(wide, buf);
			}
			else
			{
				oscrsz = cur;
				STcopy(cur, buf);
			}
/* # line 498 "vffrmatr.qsc" */	/* putform */
      {
        if (IIfsetio((char *)0) != 0) {
          IIputfldio((char *)"screen_size",(short *)0,1,32,0,buf);
        } /* IIfsetio */
      }
/* # line 499 "vffrmatr.qsc" */	/* host code */
		}
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 503 "vffrmatr.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"style") == 0) {
              goto IIfdB1;
            } /* IIvalfld */
          }
/* # line 504 "vffrmatr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,99,buf,(char *)"style");
            } /* IIfsetio */
          }
/* # line 505 "vffrmatr.qsc" */	/* host code */
		i = MEcmp(buf, popuptxt, STlength(buf)) == 0;
		CVlower(buf);
		if (STcompare(buf, ostyle) != 0)
		{
			fattr_chg = TRUE;
		}
		if (ispopup != i)
		{
		    /* transition from fullscreen to popup */
		    if (i)
		    {
			if ((vfChkFrmFit(1, 1, TRUE) != 0)
			   && (vfMakePopup() == FALSE))
			{ 
			    IIUGerr(E_VF0128_CantMakePopup, UG_ERR_ERROR, 0);
			}
			else
			{
			    vfSetPopFlg(TRUE);
			    vfSetBordFlg(TRUE);
			    /*
			    **  Turn off screen size field since
			    **  we are now a popup.
			    */
/* # line 532 "vffrmatr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"screen_size",0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 533 "vffrmatr.qsc" */	/* host code */
			    frame->frmflags &= ~(fdNSCRWID | fdWSCRWID);
			} 
		    }
		    else	/* transition from popup to fullscreen */
		    {
			vfSetPopFlg(FALSE);
			vfSetBordFlg(FALSE);
			isfixed = 0;
			frame->frposy = frame->frposx = 0;
			/*
			**  Turn on screen size field since
			**  we are now a fullscreen.
			*/
/* # line 547 "vffrmatr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIputfldio((char *)"screen_size",(short *)0,1,32,0,cur);
            } /* IIfsetio */
          }
/* # line 548 "vffrmatr.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
              IIstfsio(54,(char *)"screen_size",0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 549 "vffrmatr.qsc" */	/* host code */
		    }
		    vfPutFrmAtr();
		}
		else if (ispopup)
		{
/* # line 555 "vffrmatr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vffrmatr") != 0) {
              IIputfldio((char *)"style",(short *)0,1,32,0,popuptxt);
            } /* IIfsetio */
          }
/* # line 556 "vffrmatr.qsc" */	/* host code */
		}
		else
		{
/* # line 559 "vffrmatr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vffrmatr") != 0) {
              IIputfldio((char *)"style",(short *)0,1,32,0,fullscreentxt);
            } /* IIfsetio */
          }
/* # line 560 "vffrmatr.qsc" */	/* host code */
		}
/* # line 561 "vffrmatr.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 565 "vffrmatr.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"screen_size") == 0) {
              goto IIfdB1;
            } /* IIvalfld */
          }
/* # line 566 "vffrmatr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,99,buf,(char *)"screen_size");
            } /* IIfsetio */
          }
/* # line 568 "vffrmatr.qsc" */	/* host code */
		CVlower(buf);
		if (STcompare(buf, oscrsz) != 0)
		{
			fattr_chg = TRUE;
		}
		/*
		**  Find out what the user specified.
		*/
		if (MEcmp(buf, narrow, STlength(buf)) == 0)
		{
/* # line 579 "vffrmatr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIputfldio((char *)"screen_size",(short *)0,1,32,0,narrow);
            } /* IIfsetio */
          }
/* # line 580 "vffrmatr.qsc" */	/* host code */
			frame->frmflags &= ~fdWSCRWID;
			frame->frmflags |= fdNSCRWID;
		}
		else if (MEcmp(buf, wide, STlength(buf)) == 0)
		{
/* # line 585 "vffrmatr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIputfldio((char *)"screen_size",(short *)0,1,32,0,wide);
            } /* IIfsetio */
          }
/* # line 586 "vffrmatr.qsc" */	/* host code */
			frame->frmflags &= ~fdNSCRWID;
			frame->frmflags |= fdWSCRWID;
		}
		else
		{
			/*
			**  Must be current.
			*/
/* # line 594 "vffrmatr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIputfldio((char *)"screen_size",(short *)0,1,32,0,cur);
            } /* IIfsetio */
          }
/* # line 595 "vffrmatr.qsc" */	/* host code */
			frame->frmflags &= ~(fdNSCRWID | fdWSCRWID);
		}
/* # line 597 "vffrmatr.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 601 "vffrmatr.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"postype") == 0) {
              goto IIfdB1;
            } /* IIvalfld */
          }
/* # line 602 "vffrmatr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,99,buf,(char *)"postype");
            } /* IIfsetio */
          }
/* # line 604 "vffrmatr.qsc" */	/* host code */
		CVlower(buf);
		if (STcompare(buf, opostype) != 0)
		{
			fattr_chg = TRUE;
		}
		i = MEcmp(buf, fixedtxt, STlength(buf)) == 0;
		if (isfixed != i)
		{
		    isfixed = frame->frposy = frame->frposx = i;
		    vfPutStartRowCol();
		}
		if (isfixed)
		{
/* # line 621 "vffrmatr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vffrmatr") != 0) {
              IIputfldio((char *)"postype",(short *)0,1,32,0,fixedtxt);
            } /* IIfsetio */
          }
/* # line 622 "vffrmatr.qsc" */	/* host code */
		}
		else
		{
/* # line 625 "vffrmatr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vffrmatr") != 0) {
              IIputfldio((char *)"postype",(short *)0,1,32,0,floatingtxt);
            } /* IIfsetio */
          }
/* # line 626 "vffrmatr.qsc" */	/* host code */
		}
/* # line 627 "vffrmatr.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 631 "vffrmatr.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"border") == 0) {
              goto IIfdB1;
            } /* IIvalfld */
          }
/* # line 632 "vffrmatr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vffrmatr") != 0) {
              IIgetfldio((short *)0,1,32,99,buf,(char *)"border");
            } /* IIfsetio */
          }
/* # line 633 "vffrmatr.qsc" */	/* host code */
		i = (IIVF_yes1[0] == buf[0]);
		if (vfChkFrmFit(frame->frposy, frame->frposx, i) != 0)
		{
			IIUGerr(E_VF0127_BorderOvFlo, UG_ERR_ERROR, 0);
		}
		else
		{
			if (i != oisborder)
			{
				fattr_chg = TRUE;
			}
			vfSetBordFlg(i);
		}
		vfPutNumRowsCols();
/* # line 648 "vffrmatr.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 652 "vffrmatr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vffrmatr") != 0) {
              IIgetfldio((short *)0,1,30,4,&pos,(char *)"startrow");
            } /* IIfsetio */
          }
/* # line 653 "vffrmatr.qsc" */	/* host code */
		if (pos < 1)
		{
			IIUGerr(E_VF0125_StartRowColErr, UG_ERR_ERROR, 0);
/* # line 656 "vffrmatr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vffrmatr") != 0) {
              IIputfldio((char *)"startrow",(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
            } /* IIfsetio */
          }
/* # line 657 "vffrmatr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 658 "vffrmatr.qsc" */	/* host code */
		}
		if (vfChkFrmFit(pos, frame->frposx, isborder) != 0) 
		{ 
			IIUGerr(E_VF0126_StartRCOvFlo, UG_ERR_ERROR, 1, 
				ERget(F_VF004c_Rowtitle));
/* # line 663 "vffrmatr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vffrmatr") != 0) {
              IIputfldio((char *)"startrow",(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
            } /* IIfsetio */
          }
/* # line 664 "vffrmatr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 665 "vffrmatr.qsc" */	/* host code */
		}
		if (pos != oposy)
		{
			fattr_chg = TRUE;
		}
		frame->frposy = pos;
/* # line 671 "vffrmatr.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 675 "vffrmatr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vffrmatr") != 0) {
              IIgetfldio((short *)0,1,30,4,&pos,(char *)"startcol");
            } /* IIfsetio */
          }
/* # line 676 "vffrmatr.qsc" */	/* host code */
		if (pos < 1)
		{
			IIUGerr(E_VF0125_StartRowColErr, UG_ERR_ERROR, 0);
/* # line 679 "vffrmatr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vffrmatr") != 0) {
              IIputfldio((char *)"startcol",(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
            } /* IIfsetio */
          }
/* # line 680 "vffrmatr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 681 "vffrmatr.qsc" */	/* host code */
		}
		if (vfChkFrmFit(frame->frposy, pos, isborder) != 0) 
		{
			IIUGerr(E_VF0126_StartRCOvFlo, UG_ERR_ERROR, 1,
				ERget(F_VF004d_Coltitle));
/* # line 686 "vffrmatr.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"vffrmatr") != 0) {
              IIputfldio((char *)"startcol",(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
            } /* IIfsetio */
          }
/* # line 687 "vffrmatr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 688 "vffrmatr.qsc" */	/* host code */
		}
		if (pos != oposx)
		{
			fattr_chg = TRUE;
		}
		frame->frposx = pos;
/* # line 694 "vffrmatr.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 7) {
        {
/* # line 698 "vffrmatr.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"attrbytes") == 0) {
              goto IIfdB1;
            } /* IIvalfld */
          }
/* # line 699 "vffrmatr.qsc" */	/* getform */
          {
            if (IIfsetio((char *)"vffrmatr") != 0) {
              IIgetfldio((short *)0,1,32,99,buf,(char *)"attrbytes");
            } /* IIfsetio */
          }
/* # line 700 "vffrmatr.qsc" */	/* host code */
		space_for_attr = (IIVF_yes1[0] == buf[0]);
/* # line 701 "vffrmatr.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 8) {
        {
/* # line 706 "vffrmatr.qsc" */	/* host code */
		if (ispopup == FALSE)
		{
			IIUGerr(E_VF0129_NoVisAdjFullScr, UG_ERR_ERROR, 0); 
/* # line 709 "vffrmatr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 710 "vffrmatr.qsc" */	/* host code */
		}
		fattr_chg = TRUE;
		FTfrminvis();
		vfVisualAdj();
		retval = TRUE;
		FTfrmvis();
/* # line 716 "vffrmatr.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 9) {
        {
/* # line 722 "vffrmatr.qsc" */	/* host code */
		IIUFfieldHelp();
        }
      } else if (IIretval() == 10) {
        {
/* # line 727 "vffrmatr.qsc" */	/* host code */
		vfhelpsub(VFH_FRMATR, ERget(S_VF0130_Form_Attr), NULL);
        }
      } else if (IIretval() == 11) {
        {
/* # line 732 "vffrmatr.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else {
        if (1) goto IIfdE1;
      } /* IIretval */
    } /* IIrunform */
IIfdF1:
    if (IIchkfrm() == 0) goto IIfdB1;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    goto IIfdE1;
IImuI1:
    if (IIFRafActFld((char *)"style",0,1) == 0) goto IIfdE1;
    if (IIFRafActFld((char *)"screen_size",0,2) == 0) goto IIfdE1;
    if (IIFRafActFld((char *)"postype",0,3) == 0) goto IIfdE1;
    if (IIFRafActFld((char *)"border",0,4) == 0) goto IIfdE1;
    if (IIFRafActFld((char *)"startrow",0,5) == 0) goto IIfdE1;
    if (IIFRafActFld((char *)"startcol",0,6) == 0) goto IIfdE1;
    if (IIFRafActFld((char *)"attrbytes",0,7) == 0) goto IIfdE1;
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(F_VF0049_VisuallyAdjust),(char *)0,1,1,8) == 0) goto 
    IIfdE1;
    if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,9) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(10,(char *)0,0,0,9) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),(char *)0,0,0,10) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,0,10) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_End),(char *)0,1,1,11) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,1,1,11) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 737 "vffrmatr.qsc" */	/* host code */
	/*
	**  If cross hairs were dropped, lets put them back.
	*/
	if (xh_disp_local)
	{
		IIVFhcross->ps_begy = IIVFhcross->ps_endy = endFrame - 1;
		IIVFhcross->ps_endx = endxFrm;
		IIVFvcross->ps_endy = endFrame - 1;
		IIVFvcross->ps_begx = IIVFvcross->ps_endx = endxFrm;
		insPos(IIVFhcross, FALSE);
		insPos(IIVFvcross, FALSE);
		/*
		**  Enable display of straight edges.
		*/
		IIVFxh_disp = TRUE;
	}
	return retval;
}
/*{
** Name:        vfPutNumRowsCols - put number or rows and columns
**                                 onto the vffrmatr form
**
** Description:
**      Update the #nrows and #ncols fields of the vffrmatr form
**      (called whenever isborder might have changed).
**
** Inputs:
**      none
**
** Outputs:
**
**      Returns:
**              VOID
**
**      Exceptions:
**
** Side Effects:
**
**
** History:
**      06/22/90 (esd) - created (moved from vfPutFrmAtr)
*/
vfPutNumRowsCols()
{
  i4 n;
	n = nrows + (isborder ? 2 : 0);
/* # line 789 "vffrmatr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vffrmatr") != 0) {
      IIputfldio((char *)"nrows",(short *)0,1,30,4,&n);
    } /* IIfsetio */
  }
/* # line 790 "vffrmatr.qsc" */	/* host code */
	n = ncols + (isborder ? 2 * (1 + IIVFsfaSpaceForAttr) : 0);
/* # line 791 "vffrmatr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vffrmatr") != 0) {
      IIputfldio((char *)"ncols",(short *)0,1,30,4,&n);
    } /* IIfsetio */
  }
/* # line 792 "vffrmatr.qsc" */	/* host code */
}
/*{
** Name:        vfPutStartRowCol - put starting row and column
**                                 onto the vffrmatr form
**
** Description:
**      Update the startung row and fields of the vffrmatr form
**      (called whenever isfixed might have changed).
**
** Inputs:
**      none
**
** Outputs:
**
**      Returns:
**              VOID
**
**      Exceptions:
**
** Side Effects:
**
**
** History:
**      06/22/90 (esd) - created (moved from vfPutFrmAtr)
*/
vfPutStartRowCol()
{
  i4 sz;
	if (isfixed)
	{
/* # line 824 "vffrmatr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vffrmatr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"startrow",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 825 "vffrmatr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vffrmatr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"startcol",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 826 "vffrmatr.qsc" */	/* host code */
	    sz = frame->frposy;
/* # line 827 "vffrmatr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vffrmatr") != 0) {
      IIputfldio((char *)"startrow",(short *)0,1,30,4,&sz);
    } /* IIfsetio */
  }
/* # line 828 "vffrmatr.qsc" */	/* host code */
	    sz = frame->frposx;
/* # line 829 "vffrmatr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vffrmatr") != 0) {
      IIputfldio((char *)"startcol",(short *)0,1,30,4,&sz);
    } /* IIfsetio */
  }
/* # line 830 "vffrmatr.qsc" */	/* host code */
	}
	else    /* floating */
	{
/* # line 833 "vffrmatr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vffrmatr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"startrow",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 834 "vffrmatr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vffrmatr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"startcol",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 835 "vffrmatr.qsc" */	/* host code */
	}
}
/*{
** Name:        vfPutFrmAtr     - put fields onto the vffrmatr form 
**
** Description: 
**      Update the fields of the form an adjust their display state
**
** Inputs:
**      none
**
** Outputs:
**
**      Returns:
**
**      Exceptions:
**
** Side Effects:
**
**
** History:
**      05/23/88 (tom)  - written
**      06/22/90 (esd)  - break out initialization of numbers of rows
**                        and columns into a separate functions
*/
vfPutFrmAtr()
{
  char *ptr;
	vfPutNumRowsCols();
	if (ispopup)
	{
/* # line 869 "vffrmatr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vffrmatr") != 0) {
      IIputfldio((char *)"style",(short *)0,1,32,0,popuptxt);
    } /* IIfsetio */
  }
/* # line 871 "vffrmatr.qsc" */	/* host code */
		ptr = (isborder) ? ERget(F_UG0001_Yes1) : ERget(F_UG0006_No1);
/* # line 872 "vffrmatr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vffrmatr") != 0) {
      IIputfldio((char *)"border",(short *)0,1,32,0,ptr);
    } /* IIfsetio */
  }
/* # line 873 "vffrmatr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vffrmatr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"postype",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 874 "vffrmatr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vffrmatr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"border",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 875 "vffrmatr.qsc" */	/* host code */
	}
	else    /* fullscreen */
	{
/* # line 878 "vffrmatr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vffrmatr") != 0) {
      IIputfldio((char *)"style",(short *)0,1,32,0,fullscreentxt);
    } /* IIfsetio */
  }
/* # line 880 "vffrmatr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vffrmatr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"postype",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 881 "vffrmatr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"vffrmatr",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"border",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 882 "vffrmatr.qsc" */	/* host code */
	}
	if (isfixed)
	{
/* # line 886 "vffrmatr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vffrmatr") != 0) {
      IIputfldio((char *)"postype",(short *)0,1,32,0,fixedtxt);
    } /* IIfsetio */
  }
/* # line 887 "vffrmatr.qsc" */	/* host code */
	}
	else    /* floating */
	{
		if (ispopup == TRUE)
/* # line 891 "vffrmatr.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"vffrmatr") != 0) {
      IIputfldio((char *)"postype",(short *)0,1,32,0,floatingtxt);
    } /* IIfsetio */
  }
/* # line 892 "vffrmatr.qsc" */	/* host code */
	}
	vfPutStartRowCol();
}
/*{
** Name:        vfChkFrmFit     - check if form fits on the screen
**
** Description:
**              Checks proposed startrow, startcol, and borderflag
**              to see, if they were used, if the form would fit
**              on the screen. If they would not bit flags are
**		returned which one (or both) line and col tests failed.
**
** Inputs:
**      i4      row;    - proposed start row for form
**      i4      col;    - proposed start col for form
**      i4      bord;   - proposed border flag
**
** Outputs:
**      Returns:
**              i4      bitflag 0 - says line doesn't fit
**			bitflag 1 - says column didn't fit
**
**      Exceptions:
**
** Side Effects:
**
** History:
**      05/25/88  (tom) - written
*/
static i4  
vfChkFrmFit(row, col, bord)
i4      row;
i4      col;
i4      bord;
{
	i4	ret = 0;
	if (bord)       /* convert flag into # of lines consumed */
	{
		bord = 2;
	}
	/* if form is 'floating' use 1,1 which is the worst case size */
	if (isfixed == FALSE)
	{
		row = col = 1;
	}
	if (row + nrows + bord > IIVFterminfo.lines)
	{
		ret |=  1;
	}
	if ((col + ncols + bord * (1 + IIVFsfaSpaceForAttr))
		> (IIVFterminfo.cols + 1))
	{
		ret |= 2;
	}
	return (ret);
}
/*{
** Name:	vfBotResize - Resize bottom of form.
**
** Description:
**	Move bottom of form up or down.  This is similar to vfFrmBotResize()
**	but not the same.  Only difference is that the changed boundary is NOT
**	displayed.  This differentiation is necessary when this routine
**	is executed in context of changing a form from fullscreen to
**	popup in the FormAttr() popup form.  We don't want multiple
**	boundaries to show up at that point.
**
** Inputs:
**	n	Number of lines to move the bottom of the form.
**	bound	Boundary of the bottom of the form.
**
** Outputs:
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
**	09/14/89 (dkh) - Initial version duplicated from vfFrmBotResize().
*/
static VOID
vfBotResize(n, bound)
i4	n;
i4	bound;
{
	if (nrows + n - 1 < bound)
	{
		n = bound - nrows + 1;
	}
	nrows += n;
	if (n > 0)
	{
		while (n--)
		{
			clearLine();
			doInsLine(endFrame);
		}
	}
	else if (n < 0)
	{
		while (n++)
		{
			clearLine();
			doDelLine(endFrame - 1);
		}
	}
}
/*{
** Name:	vfMakePopup	- Convert a form into a popup
**
** Description:
**		This function is called when a form is being 
**		converted from a fullscreen form to a popup.
**		It is only called if the form is too big to 
**		fit on the screen as a popup. We assume here that
**		the new popup will have a border and will be 
**		positioned at 1,1.
**		
**		Actions taken:
**
**		- Check if form's features are positioned so that
**		  we can shrink it to fit on the screen.
**		  If not return FALSE.
**		- Shrink the form by moveing the right side and
**		  bottom so that the popup will just fit on the
**		  screen
**
** Inputs:
**	none
**
** Outputs:
**	none
**
**	Returns:
**		bool	- TRUE  =  all ok.
**			- FALSE =  the position of features did
**					not allow the form to shrunk.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	06/10/88  (tom) - written
*/
static bool
vfMakePopup()
{
	POS	ps;
	i4	rowdif;
	i4	coldif;
	/* get the extent of form features */
	vfFormExtent(&ps);
	/* test if it is possible to shrink */
	if (  ps.ps_endy >= IIVFterminfo.lines - 3
	   || ps.ps_endx >= IIVFterminfo.cols - 2
	   )
	{
		return (FALSE);
	}
	rowdif = IIVFterminfo.lines - (nrows + 3);
	coldif = IIVFterminfo.cols - (ncols + 2);
	if (rowdif < 0)
	{
		vfBotResize(rowdif, ps.ps_endy);
	}
	if (coldif < 0)
	{
		vfFrmRightResize(coldif, ps.ps_endx);
	}
	return (TRUE);
}
/*{
** Name:        allocFrmAttr- initialize the form attribute form 
**
** Description:
**              Initialize the form attribute form from the form index file
**
** Inputs:
**      none
**
** Outputs:
**
**      Returns:
**              none
**
**      Exceptions:
**
** Side Effects:
**              The form is readied for display 
**
** History:
**      04/08/88  (tom) - created.
*/
allocFrmAttr()
{
	FDsetparse(FALSE);
	if (IIUFgtfGetForm(IIUFlcfLocateForm(), ERx("vffrmatr")) != OK)
	{
		syserr(ERget(S_VF0001_Can_not_access_form), ERx("vffrmatr"));
	}
	FDsetparse(TRUE);
}
/*{
** Name:        vfVisualAdj     - Visually adjust popup form position and size
**
** Description:
**              Support visual adjustment of popup form. Supported
**              adjustments include   Movement, Resize and display
**              of background form.
**
** Inputs:
**      none
**
** Outputs:
**      Returns:
**
**      Exceptions:
**
** Side Effects:
**
** History:
**      05/25/88  (tom) - created.
*/
vfVisualAdj()
{
	i4	done;
	i4	r;
	i4	c;
  char movetxt[20];
	FRAME   vtfrm;  /* fake frame which has a window which
			   is screen sized so that vtcursor will be
			   limited to the screen. */
	/* fixed popups allow movement.. but floating ones don't */
	if (isfixed)
	{
		STcopy(ERget(FE_Move), movetxt);
	}
	else
	{
		movetxt[0] = EOS;
	}
	for (done = FALSE; done == FALSE;)
	{
		r = nrows + (isborder ? 2 : 0);
		c = ncols + (isborder ?
		    2 * (1 + IIVFsfaSpaceForAttr) : 0);
		if (isfixed)
		{
			vtfrm.frposy = frame->frposy;
			vtfrm.frposx = frame->frposx;
		}
		else
		{
			/* floating popups are centered on the screen */
			vtfrm.frposy = (IIVFterminfo.lines - 1 - r) / 2 + 1;
			vtfrm.frposx = (IIVFterminfo.cols - c) / 2 + 1;
		}
		vtfrm.frmaxy = r;
		vtfrm.frmaxx = c;
		vtfrm.frmflags = (isborder ? fdBOXFR: 0);
		VTFormPreview(&vtfrm, frame);
/* # line 1190 "vffrmatr.qsc" */	/* submenu */
  {
    goto IImuI2;
IIfdB2:
    IIrunmu(0);
    if (IIretval() == 1) {
      {
/* # line 1193 "vffrmatr.qsc" */	/* host code */
			vfFormResize(&vtfrm);
      }
    } else if (IIretval() == 2) {
      {
/* # line 1197 "vffrmatr.qsc" */	/* host code */
			/* update preview window with hilighted corners */
			VTUpdPreview(&vtfrm, TRUE);
			vfFormMove(&vtfrm);
      }
    } else if (IIretval() == 3) {
      {
/* # line 1203 "vffrmatr.qsc" */	/* host code */
			vfhelpsub(VFH_VISADJ, ERget(S_VF0134_Vis_Adj), NULL);
			VTUpdPreview(&vtfrm, FALSE);
      }
    } else if (IIretval() == 4) {
      {
/* # line 1208 "vffrmatr.qsc" */	/* host code */
			done = TRUE;
      }
    } else {
      if (1) goto IIfdE2;
    } /* IIretval */
    goto IIfdE2;
IImuI2:
    IImuonly();
    if (IIinitmu() == 0) goto IIfdE2;
    if (IInmuact(ERget(F_VF0046_Resize),(char *)0,2,2,1) == 0) goto IIfdE2;
    if (IInmuact(movetxt,(char *)0,2,2,2) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Help),(char *)0,2,2,3) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_End),(char *)0,2,2,4) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdB2;
IIfdE2:;
  }
/* # line 1210 "vffrmatr.qsc" */	/* host code */
		VTKillPreview(&vtfrm);
	}
/* # line 1212 "vffrmatr.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 1213 "vffrmatr.qsc" */	/* host code */
}
/*{
** Name:        vfFormMove      - let user move the form 
**
** Description:
**      Supervise moving a form.
**
** Inputs:
**      FRAME   *vtfrm;         - frame to use for vtcursor
**
** Outputs:
**
**      Returns:
**              none
**
**      Exceptions:
**              none
**
** Side Effects:
**
** History:
**      05/25/88  (tom) - created
*/
vfFormMove(vtfrm)
FRAME   *vtfrm;
{
	i4             poscount = 0;
	FLD_POS         *posarray = NULL;
	FRS_EVCB        evcb;
	i4		menu_val;
	i4             y_anchor, x_anchor;
	i4             y_rover, x_rover;
	/* Force the next FTputmenu() call to re-construct the menu. */
	selbox_m->mu_flags |= MU_NEW;
	for (;;)
	{
		y_anchor = y_rover = frame->frposy - 1;         
		x_anchor = x_rover = frame->frposx - 1;
		MEfill(sizeof(FRS_EVCB), '\0', &evcb);
		evcb.event = fdNOCODE;
		FTclrfrsk();
		FTaddfrs(1, HELP, NULL, 0, 0);
		FTaddfrs(3, UNDO, NULL, 0, 0);
		FTputmenu(selbox_m);
	    again:
		VTcursor(vtfrm, &y_rover, &x_rover, 
			IIVFterminfo.lines - 1, IIVFterminfo.cols - 1,
			poscount, posarray, &evcb, VFfeature,
			RBF, FALSE, TRUE, FALSE, FALSE);
		VTdumpset(vfdfile);
		if (evcb.event == fdopFRSK)
		{
			menu_val = evcb.intr_val;
		}
		else if (evcb.event == fdopMUITEM)
		{
			if (IImumatch(selbox_m, &evcb, &menu_val) != OK)
			{
				continue;
			}
		} 
		else 
		{
			menu_val = FTgetmenu(selbox_m, &evcb);
			if (evcb.event == fdopFRSK)
			{
				menu_val = evcb.intr_val;
			}
		}
		VTdumpset(NULL);
		switch (menu_val)
		{
		case HELP:
			vfhelpsub(VFH_FRMMOV, 
				ERget(S_VF0131_MOVE_Frm_Command),
				selbox_m);
			VTUpdPreview(vtfrm, TRUE);
			continue;
		case PLACE:
			switch (vfChkFrmFit(
					frame->frposy += (y_rover - y_anchor), 
					frame->frposx += (x_rover - x_anchor), 
					isborder))
			{
			case 3:	/* both row and column are out of bounds */
				frame->frposx = IIVFterminfo.cols + 1  - ncols 
				    - (isborder ? 2 * (1 + IIVFsfaSpaceForAttr)
				    : 0); 
				/* fall thru */
			case 1:	/* row is out of bounds */
				frame->frposy = IIVFterminfo.lines - nrows 
					- (isborder ? 2 : 0); 
				break;
			case 2: /* column is out of bounds */
				frame->frposx = IIVFterminfo.cols + 1  - ncols 
				    - (isborder ? 2 * (1 + IIVFsfaSpaceForAttr)
				    : 0); 
				break;
			case 0:
				break;
			}
		case UNDO:
			break;
		default:
			goto again;
		}
		break;
	}
}
/*{
** Name:        vfFormResize    - let user resize the form 
**
** Description:
**      Supervise resizing a form. First the user is allowed 
**      to move the lower right corner.. upon the tab key
**      we rotate the moveable corner around the form clockwise.
**
** Inputs:
**      FRAME   *vtfrm;         - frame to use for vtcursor
**
** Outputs:
**
**      Returns:
**              none
**
**      Exceptions:
**              none
**
** Side Effects:
**
** History:
**      05/25/88  (tom) - created
*/
vfFormResize(vtfrm)
FRAME   *vtfrm;
{
	i4             poscount = 0;
	FLD_POS         *posarray = NULL;
	FRS_EVCB        evcb;
	i4             bord;
	i4             y_anchor, x_anchor;
	i4             y_rover, x_rover;
	i4             y_top, x_left;
	i4             y_bot, x_right;
	/* the lower right corner...numbered clockwise starting here */
	i4             movecorner = 0;
	/* filled in with maximum extent of features of the form */
	POS             ps;
	vfFormExtent(&ps);
	for (;;)
	{
		bord = (isborder ? 2 : 0);
		/*
		** setup for rover on corner 0, lower right corner, 
		** anchor on upper left corner
		*/
		y_anchor = y_top = vtfrm->frposy - 1;         
		y_rover =  y_bot = y_top + nrows + bord - 1; 
		x_anchor = x_left = vtfrm->frposx - 1;
		x_rover =  x_right = x_left + ncols
		    + bord * (1 + IIVFsfaSpaceForAttr) - 1;
		/* if we were not on corner 0...then exchange appropriately */
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
		VTflashplus(vtfrm, y_anchor, x_anchor, 1);
	    again:
		switch (VTcursor(vtfrm, &y_rover, &x_rover, 
			IIVFterminfo.lines - 1, IIVFterminfo.cols - 1,
			poscount, posarray, &evcb, VFfeature,
			RBF, FALSE, TRUE, TRUE, TRUE))
		{
		case fdopMENU:                  /* return or menu key.. done */
			VTflashplus(vtfrm, 0, 0, 0);
			break;
		case fdopNEXT:                  /* tab key was struct */
			++movecorner;
			VTflashplus(vtfrm, 0, 0, 0);
			continue;
		default:
			goto again;
		}
		/* depending upon which corner was being moved... */
		switch (movecorner % 4)
		{
		case 0:
			vfFrmBotResize(y_rover - y_bot, ps.ps_endy);
			vfFrmRightResize(x_rover - x_right, ps.ps_endx);
			break;
		case 1:
			vfFrmBotResize(y_rover - y_bot, ps.ps_endy);
			vfFrmLeftResize(x_rover - x_left, ps.ps_begx);
			break;
		case 2:
			vfFrmTopResize(y_rover - y_top, ps.ps_begy);
			vfFrmLeftResize(x_rover - x_left, ps.ps_begx);
			break;
		case 3:
			vfFrmTopResize(y_rover - y_top, ps.ps_begy);
			vfFrmRightResize(x_rover - x_right, ps.ps_endx);
			break;
		}
		VTerase(frame);
		vfDispAll(endFrame);
		break;
	}
}
/*{
** Name:        vfFrmTopResize - Form top resize by n rows.
**
** Description:
**              In effect.. move the top of the form up or down
**                      depending upon n. 0 is alright and
**                      means do nothing. Negative values mean up.
**                      Positive values mean down.
**
** Inputs:
**      i4      n;      - number of lines to move the top of the form.
**      i4      bound;  - boundary of the top of the form.
**
**
** History:
**      05/30/88 (tom) - written
*/
vfFrmTopResize(n, bound)
reg     i4      n;
	i4     bound;
{
	if (n > bound)
	{
		n = bound;
	}
	if (isfixed)
		frame->frposy += n;
	nrows -= n;
	if (n > 0)
	{
		while (n--)
		{
			clearLine();
			doDelLine(0);
			vfDispAll(endFrame);
		}
	}
	else if (n < 0)
	{
		while (n++)
		{
			clearLine();
			doInsLine(0);
			vfDispAll(endFrame);
		}
	}
}
/*{
** Name:        vfFrmBotResize - Form bottom resize by n rows.
**
** Description:
**              In effect.. move the bottom of the form up or down
**                      depending upon n. 0 is alright and
**                      means do nothing. Negative values mean up.
**                      Positive values mean down.
**
** Inputs:
**      i4      n;      - number of lines to move the bottom of the form.
**      i4      bound;  - boundary of the bottom of the form.
**
**
** History:
**      05/30/88 (tom) - written
*/
vfFrmBotResize(n, bound)
reg     i4      n;
	i4     bound;
{
	if (nrows + n - 1 < bound)
	{
		n = bound - nrows + 1;
	}
	nrows += n;
	if (n > 0)
	{
		while (n--)
		{
			clearLine();
			doInsLine(endFrame);
			vfDispAll(endFrame);
		}
	}
	else if (n < 0)
	{
		while (n++)
		{
			clearLine();
			doDelLine(endFrame - 1);
			vfDispAll(endFrame);
		}
	}
}
/*{
** Name:        vfFrmLeftResize - Form left side resize by n rows.
**
** Description:
**              In effect.. move the left side of the form right or left
**                      depending upon n. 0 is alright and
**                      means do nothing. Negative values mean left.
**                      Positive values mean right.
**
** Inputs:
**      i4      n;      - number of lines to move the right of the form.
**      i4      bound;  - boundary of the right side of the form.
**
**
** History:
**      05/30/88 (tom) - written
*/
vfFrmLeftResize(n, bound)
reg     i4      n;
	i4     bound;
{
	if (n > bound)
	{
		n = bound;
	}
	if (isfixed)
		frame->frposx += n;
	ncols -= n;
	if (n > 0)
	{
		vf_wchg(endxFrm - n);
	}
	else
	{
		vfwider(frame, -n);
	}
	if (n)
	{
		vfFormHorzAdjust(-n);
	}
}
/*{
** Name:        vfFrmRightResize - Form right side resize by n rows.
**
** Description:
**              In effect.. move the right side of the form right or left
**                      depending upon n. 0 is alright and
**                      means do nothing. Negative values mean left.
**                      Positive values mean right.
**
** Inputs:
**      i4      n;      - number of lines to move the right of the form.
**      i4      bound;  - boundary of the right side of the form.
**
**
** History:
**      05/30/88 (tom) - written
*/
vfFrmRightResize(n, bound)
reg     i4      n;
	i4     bound;
{
	if (ncols + n - 1 < bound)
	{
		n = bound - ncols + 1;
	}
	ncols += n;
	if (n > 0)
	{
		vfwider(frame, n);
	}
	else
	{
		vf_wchg(endxFrm + n);
	}
}
/*{ 
** Name:        vfFormExtent - get the extent of form's features
**
** Description:
**              Fill in the POS structure with the start 
**              and end x and y coordinates of the form's features.
**
** Inputs:
**      POS     *ps;    - position struct to be filled in 
**
**
**
** History:
**      05/26/88 (tom) - written
*/
vfFormExtent(ps)
reg     POS     *ps;
{
	reg     POS     *cur;
	reg     i4      i;
	reg     VFNODE  *node;
	reg     VFNODE  **nodep;
	/* initialize the extent with numbers such that our limit
	   test below will get us the min/max values. */
	ps->ps_begy = nrows - 1;
	ps->ps_begx = ncols - 1;
	ps->ps_endy =  ps->ps_endx =  0;
	/* for each line in the line table */
	for (nodep = line, i = 0; i < endFrame ; i++, nodep++)
	{
		/* for each position on the line */
		for (node = *nodep; node != NNIL; node = vflnNext(node, i))
		{
			cur = node->nd_pos;
			/* if it is the first line of the feature */ 
			if (i == cur->ps_begy)
			{
				if (cur->ps_begy < ps->ps_begy)
					ps->ps_begy = cur->ps_begy;
				if (cur->ps_begx < ps->ps_begx)
					ps->ps_begx = cur->ps_begx;
				if (cur->ps_endy > ps->ps_endy)
					ps->ps_endy = cur->ps_endy;
				if (cur->ps_endx > ps->ps_endx)
					ps->ps_endx = cur->ps_endx;
			}
		}
	}
}
/*{
** Name:        vfFormHorzAdjust - adjust horizontal position of form features
**
** Description:
**              Adjust all pos structures by the number of columns
**		indicated. Negative values mean left, positive values
**		mean right.
**
** Inputs:
**      i4	cols;  - the number of columns to offset the ps_begx
**			and ps_endx elements of all pos structs.
** History:
**      05/26/88 (tom) - written
*/
vfFormHorzAdjust(cols)
reg     i4	cols;
{
	reg     POS     *cur;
	reg     i4      i;
	reg     VFNODE  *node;
	reg     VFNODE  **nodep;
	clearLine();
	for (nodep = line, i = 0; i < endFrame ; i++, nodep++)
	{
		for (node = *nodep; node != NNIL; node = vflnNext(node, i))
		{
			cur = node->nd_pos;
			if (cur->ps_begy == i)
			{
				cur->ps_mask = PMOVE;
				cur->ps_begx += cols; 
				cur->ps_endx += cols;
			}
		}
	}
	vfDispAll(endFrame);
}
/*{
** Name:        vfSetPopFlg - sets popup state
**
** Description:
**		Sets local var, and frame member to the given state.
**
** Inputs:
**	bool state;  	- state to set popup condition
**	
** History:
**      07/06/88 (tom) - written
*/
vfSetPopFlg(state)
reg	bool	state;
{
	if (state)
	{
		frame->frmflags |= fdISPOPUP;
		ispopup = TRUE;
	}
	else
	{	
		frame->frmflags &= ~fdISPOPUP;
		ispopup = FALSE; 
	} 
}
/*{
** Name:        vfSetBordFlg - sets border state
**
** Description:
**		Sets local var, and frame member to the given state.
**
** Inputs:
**	bool state;  	- state to set border condition
**	
** History:
**      07/06/88 (tom) - written
*/
vfSetBordFlg(state)
reg	bool	state;
{
	if (state)
	{
		frame->frmflags |= fdBOXFR;
		isborder = TRUE;
	}
	else
	{	
		frame->frmflags &= ~fdBOXFR;
		isborder = FALSE; 
	} 
}
