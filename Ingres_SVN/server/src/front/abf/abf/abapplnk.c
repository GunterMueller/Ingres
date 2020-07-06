# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<er.h>
# include	<ex.h>
# include	<lo.h>
# include	<si.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
#include	<fdesc.h>
# include       <abfrts.h>
# include <abclass.h>
#include	<abfcompl.h>
#include	"abfgolnk.h"
# include	"ablink.h"
# include	"erab.h"
/**
** Name:	abapplnk.qc -	Link an application.
**
** Description:
**	This file defines:
**
**	IIABlkaLinKApplication - Perform and display an application link
** 	IIABdcmDispCompMsg - 	Display a compilation message
** 	IIABdcfDispCompFile - 	Display a file during compilation
** 	IIABrsRestoreScreen - 	Restore the screen
**
** History:
**	3/30/90 (Mike S) Initial Version
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
GLOBALDEF bool	IIabfmFormsMode;	/* Is the forms system active */
static READONLY
  char _form[]= 	ERx("ablnkdsp"), _tf[]= 	ERx("ablnkdsp"), _tfcol[]= 	ERx("ablnkdsp")
  , _title[]= 	ERx("title"), _prompt[]= 	ERx("prompt"), _blank[]= 	ERx("");
static i4   	in_link = FALSE;
static bool	first_time = TRUE;
static STATUS do_link();
static STATUS postlink();
/*{
** Name:	IIABlkaLinKApplication - Perform and display an application link
**
** Description:
**	This routine performs all application linking, both for testing and
**	image-building.  IIABtestLink or iiabImgLink, respectively,  is called 
**	to do the link.
**	The value of IIabfmFormsMode determines whether to display output in a 
**	tablefield, or just let it go to stdout.
**
** Inputs:
**	link_args	ABLNKARGS *	link arguments
**
** Returns:
**	{STATUS}		Link status
**
** History:
**	3/30/90 (Mike S)	Initial version
*/
STATUS
IIABlkaLinKApplication(link_args)
ABLNKARGS *link_args;
{
	STATUS	status;		/* link status */
	/* Initialize the display form */
	if (first_time && IIabfmFormsMode)
	{
		first_time = FALSE;
		IIARiaForm(_form);
/* # line 90 "abapplnk.qsc" */	/* inittable */
  {
    if (IItbinit(_form,_tf,(char *)"r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 91 "abapplnk.qsc" */	/* host code */
	}
	/* If we're not in forms mode, call the link routines directly */
	if (!IIabfmFormsMode)
	{
		return do_link(link_args);
	}
	/* 
	** We are in forms mode.  We have to bring up the compilation
	** display form and then call the link routines.  They will
	** output into the tablefield, through the wonders of the forms
	** system versions of the callback routines.
	*/
/* # line 105 "abapplnk.qsc" */	/* display */
  {
    if (IIdispfrm(_form,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
      char title[80];
/* # line 110 "abapplnk.qsc" */	/* host code */
		/* Preload tablefield with a blank line */
/* # line 111 "abapplnk.qsc" */	/* loadtable */
      {
        if (IItbact(_form,_tf,1) != 0) {
          IItcoputio(_tfcol,(short *)0,1,32,0,_blank);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 113 "abapplnk.qsc" */	/* host code */
		IIUGfmt(title, sizeof(title), ERget(F_AB0100_BuildAppl), 1,
			link_args->app->name);
/* # line 115 "abapplnk.qsc" */	/* putform */
      {
        if (IIfsetio(_form) != 0) {
          IIputfldio(_title,(short *)0,1,32,0,title);
          IIputfldio(_prompt,(short *)0,1,32,0,_blank);
        } /* IIfsetio */
      }
/* # line 116 "abapplnk.qsc" */	/* message */
      {
        IImessage(_blank);
      }
/* # line 117 "abapplnk.qsc" */	/* redisplay */
      {
        IIredisp();
      }
/* # line 119 "abapplnk.qsc" */	/* host code */
		status = do_link(link_args);
		if (EXDECLARE == status)
		{
			/* We were interrupted -- let's get out */
/* # line 123 "abapplnk.qsc" */	/* breakdisplay */
      {
        if (1) goto IIfdE1;
      }
/* # line 124 "abapplnk.qsc" */	/* host code */
		}
		/* 
		** What the user can do know depends on the circumstances,
		** so we use a submenu for flexibilty.
		*/
		status = postlink(link_args, status);
/* # line 131 "abapplnk.qsc" */	/* clear */
      {
        IIfldclear(_tf);
      }
/* # line 132 "abapplnk.qsc" */	/* breakdisplay */
      {
        if (1) goto IIfdE1;
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 137 "abapplnk.qsc" */	/* host code */
		/* A dummy activation; we never get here */
/* # line 138 "abapplnk.qsc" */	/* breakdisplay */
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
    goto IIfdE1;
IImuI1:
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_End),(char *)0,2,2,1) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 141 "abapplnk.qsc" */	/* host code */
	return (status);
}
/*
**	After the link, allow the user to do whatever makes sense:
**	1.	If it was a test build, and everything worked, we run the 
**		interpreter.
**	2.	If there were compilation errors, allow their display.
**	3.	If the interpreter is runnable after compilation errors,
**		allow that.
*/
static STATUS
postlink(link_args, status)
ABLNKARGS *link_args;
STATUS	status;		/* Status from do_link */
{
  char *erredit = _blank;
  char *go = _blank;
  char prompt_str[80];
	/* Format default prompt string */
	IIUGfmt(prompt_str, sizeof(prompt_str), 
		ERget(S_AB001C_EndtoExit), 1, ERget(FE_End));
	/* Determine which menuitems are appropriate */
	if (ABLNK_TEST == link_args->link_type)
	{
		/* A test build */
		if (OK == status)
		{
			if (!link_args->tstimg->failed_comps)
			{
				/* No failure of any kind. Run interpreter */
				return OK;
			}
			else
			{
				go = ERget(FE_Go);
				erredit = ERget(F_AB02FB_Errors);
				/* Tell user he might be able to proceeed */
				IIUGerr(S_AB0278_Failed, 0, 0);
				/* Override default prompt string */
				IIUGfmt(prompt_str, sizeof(prompt_str), 
					ERget(S_AB0030_GoToTest), 1, 
					ERget(FE_Go));
			}
		}
		else
		{
			/* Unrecoverable compilation errors */
			erredit = ERget(F_AB02FB_Errors);
			/* Tell user he can't proceeed */
			IIUGerr(S_AB0021_SevereFailed, 0, 0);
		}
	}
	else
	{ /* An image build */
		if (status != OK)
			erredit = ERget(F_AB02FB_Errors);
	}
	/* Put up prompt string */
/* # line 207 "abapplnk.qsc" */	/* putform */
  {
    if (IIfsetio(_form) != 0) {
      IIputfldio(_prompt,(short *)0,1,32,0,prompt_str);
    } /* IIfsetio */
  }
/* # line 208 "abapplnk.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 210 "abapplnk.qsc" */	/* display */
  {
    if (IInestmu() == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
IIfdB2:
    while (IIrunnest() != 0) {
      if (IIretval() == 1) {
        {
/* # line 214 "abapplnk.qsc" */	/* host code */
		/* Test the application, by returning a status of OK */
		status = OK;
/* # line 216 "abapplnk.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 221 "abapplnk.qsc" */	/* host code */
		/* Edit the current errors */
		_VOID_ IIVQdceDisplayCompErrs(link_args->app, (OO_OBJECT *)NULL,
					      FALSE);
		status = FAIL;
/* # line 225 "abapplnk.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 3) {
        {
          char *msg;
/* # line 231 "abapplnk.qsc" */	/* host code */
            bool dummy;
                if (IIUFtfuTblFldUnload(_form, _tf, _tfcol, &msg, &dummy) != OK)                {
/* # line 234 "abapplnk.qsc" */	/* message */
          {
            IImessage(msg);
          }
/* # line 235 "abapplnk.qsc" */	/* sleep */
          {
            IIsleep(2);
          }
/* # line 236 "abapplnk.qsc" */	/* host code */
                }
        }
      } else if (IIretval() == 4) {
        {
/* # line 242 "abapplnk.qsc" */	/* host code */
                FEhelp(ERx("ablnkapp.hlp"), ERget(S_AB0011_Link_Banner));
        }
      } else if (IIretval() == 5) {
        {
/* # line 248 "abapplnk.qsc" */	/* host code */
		status = FAIL;		/* Don't run the interpreter */
/* # line 249 "abapplnk.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 255 "abapplnk.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_tf,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 7) {
        {
/* # line 261 "abapplnk.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_tf,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,-1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 8) {
        {
/* # line 267 "abapplnk.qsc" */	/* host code */
                FEtabfnd(_form, _tf);
        }
      } else {
        if (1) goto IIfdE2;
      } /* IIretval */
    } /* IIrunnest */
IIfdF2:
    if (IIchkfrm() == 0) goto IIfdB2;
    goto IIfdE2;
IImuI2:
    if (IIinitmu() == 0) goto IIfdE2;
    if (IInmuact(go,ERget(F_AB000B_Go_expl),2,2,1) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE2;
    if (IInmuact(erredit,ERget(F_AB02FC_ErrorsExpl),2,2,2) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_WriteFile),ERget(F_FE010E_WriteExpl),2,2,3) == 0) 
    goto IIfdE2;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),2,2,4) == 0) 
    goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,4) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,5) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,5) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,6) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,7) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,8) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendnest();
  }
/* # line 270 "abapplnk.qsc" */	/* host code */
	return(status);
}
/*{
** Name:	IIABrsRestoreScreen -- Restore the screen
**
** Description:
**	Restore the screen after a third-party program has scribbled
**	over it.  If we're in a non-forms-system link, this does nothing.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
**	Returns:
**		none
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	3/30/90 (Mike S) Initial Version
*/
VOID
IIABrsRestoreScreen()
{
	if (in_link && IIabfmFormsMode)
	{
		/* Clear and repaint the screen */
/* # line 302 "abapplnk.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 303 "abapplnk.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 304 "abapplnk.qsc" */	/* host code */
	}
}
/*{
** Name:	IIABdcfDispCompFile - Display a file during compilation
**
** Description:  
**	Display a file during compilation of linking.  If we're
**	doing a forms system link, this goes into the tablefield.  If we're
**	doing a non-forms system link, it goes to stdout.
**	
** Inputs:
**	file	{LOCATION *}  Location of file to be displayed.
**
** History:
**	3/30/90 (Mike S) Initial Version
*/
VOID
IIABdcfDispCompFile(file)
LOCATION *file;         /* File to display */
{
	if (!in_link)
		return;
	if (IIabfmFormsMode)
	{
		/* Put the file above the trailing blank line */
/* # line 331 "abapplnk.qsc" */	/* scroll */
  {
    if (IItbsetio(1,_form,_tf,-3) != 0) {
      IItbsmode((char *)"to");
      if (IItscroll(0,-1) != 0) {
      } /* IItscroll */
    } /* IItbsetio */
  }
/* # line 332 "abapplnk.qsc" */	/* host code */
		_VOID_ IIUFtfnTblFldNiLoad(file, _form, _tf, _tfcol);
		/* Show it */
/* # line 335 "abapplnk.qsc" */	/* scroll */
  {
    if (IItbsetio(1,_form,_tf,-3) != 0) {
      IItbsmode((char *)"to");
      if (IItscroll(0,-1) != 0) {
      } /* IItscroll */
    } /* IItbsetio */
  }
/* # line 336 "abapplnk.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 337 "abapplnk.qsc" */	/* host code */
	}
	else
	{
		/* Send it to stdout */
		_VOID_ SIcat(file, stdout);
	}
}
/*{
** Name:	IIABdcmDispCompMsg - Display a compilation message
**
** Description:
**	Display a message during compilation or linking.  If we're in a forms
**	system link, append it to the tablefield.  If we're in a non-forms 
**	system link, display it with IIUGmsg.  If we're not in a link at all 
**	(e.g. in a single-frame compilation), display it with IIUGmsg if
**	the "always" flag is set.
**
** Inputs:
**	msg		ER_MSGID	message id
**	always		bool		Display for a single-frame compilation?
**	parcount	i4		number of message paramters
**	a0, a1, ...	PTR		message parameters 
**
** History:
**	3/30/90 (Mike S) Initial Version
*/
VOID
IIABdcmDispCompMsg(msg, always, parcount, a1, a2, a3, a4, a5)
ER_MSGID msg;
bool always;
i4  parcount;
PTR	a1;
PTR	a2;
PTR	a3;
PTR	a4;
PTR	a5;
{
	char buff[256];
	if (IIabfmFormsMode && in_link)
	{
		/* Put the message above the trailing blank line */
/* # line 380 "abapplnk.qsc" */	/* scroll */
  {
    if (IItbsetio(1,_form,_tf,-3) != 0) {
      IItbsmode((char *)"to");
      if (IItscroll(0,-1) != 0) {
      } /* IItscroll */
    } /* IItbsetio */
  }
/* # line 381 "abapplnk.qsc" */	/* host code */
		_VOID_ IIUGfmt(buff, sizeof(buff), ERget(msg), parcount, 
			       a1, a2, a3, a4, a5);
		_VOID_ IIUFtfsTblFldString(buff, _form, _tf, _tfcol);
		/* Show it */
/* # line 386 "abapplnk.qsc" */	/* scroll */
  {
    if (IItbsetio(1,_form,_tf,-3) != 0) {
      IItbsmode((char *)"to");
      if (IItscroll(0,-1) != 0) {
      } /* IItscroll */
    } /* IItbsetio */
  }
/* # line 387 "abapplnk.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 388 "abapplnk.qsc" */	/* host code */
	}
	else if (in_link || always)
	{
		/* Use IIUGmsg */
		IIUGmsg(ERget(msg), FALSE, parcount, a1, a2, a3, a4, a5);
	}
}
/*
**	Do the link
*/
static STATUS
do_link(link_args)
ABLNKARGS *link_args;
{
	STATUS status;
	ER_MSGID msg = 0;
	EX_CONTEXT context;
	FUNC_EXTERN i4 abexcintr();
	if ((status = EXdeclare(abexcintr, &context)) != OK)
	{
		EXdelete();
		return status;
	}
	/* Call the proper link routine, based on link type */
	in_link = TRUE;
 	if (ABLNK_TEST == link_args->link_type)
	{
		status =  IIABtestLink(link_args->app, link_args->tstimg);
	}
	else
	{
		status =  iiabImgLink(link_args->app, link_args->executable,
				  link_args->options);
		msg = (OK == status)  ?
				S_AB0018_ImageWorked : S_AB0019_ImageFailed;
	}
	/* 
	** If we're in forms mode, write a success/error message into the 
	** tablefield.
	*/
	if (IIabfmFormsMode && (msg != 0))
	{
		/* Insert a blank line first */
/* # line 435 "abapplnk.qsc" */	/* loadtable */
  {
    if (IItbact(_form,_tf,1) != 0) {
      IItcoputio(_tfcol,(short *)0,1,32,0,_blank);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 436 "abapplnk.qsc" */	/* host code */
		IIABdcmDispCompMsg(msg, FALSE, 0);
	}
	in_link = FALSE;
	EXdelete();
	return status;
}
