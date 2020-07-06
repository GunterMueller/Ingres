# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<er.h>
# include	<st.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include <abclass.h>
# include <metafrm.h>
#include      	"vqaclist.h"
#include        <erug.h>
#include        "ervq.h"
/**
** Name:	vqrecapp.qsc - Reconcile Visual Queries with database
**
** Description:
**	This allows the visual queries in an application to be reconciled
**	with the current database contents.
**
**	IIVQraReconcileApp	Reconcile visual queries with database.
**
** History:
**	12/12/89 (Mike S)	Initial Version
**	19-oct-1990 (mgw)
**		Fixed #include of local vqaclist.h to use "" instead of <>
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/* GLOBALDEF's */
/* extern's */
FUNC_EXTERN STATUS	IIABcelCheckEditLock();
FUNC_EXTERN STATUS	IIABrlRemLock();
FUNC_EXTERN STATUS	IIAMufqUserFrameQuick();
FUNC_EXTERN VOID	IIUGerr();
FUNC_EXTERN char	*IIUGfmt();
FUNC_EXTERN VOID	IIUGmsg();
FUNC_EXTERN STATUS	IIVQcacChooseApplComp();
FUNC_EXTERN VOID	IIVQgnmGetNewMF();
FUNC_EXTERN VOID	IIVQgomGetOldMF();
FUNC_EXTERN STATUS	IIVQradReconcileAppDiscreps();
FUNC_EXTERN bool	IIVQrfdReconcileFrameDiscreps();
FUNC_EXTERN bool	IIVQsvfSelVQFrames();
FUNC_EXTERN VOID	IIVQvqsVqShow();
FUNC_EXTERN char	*IIVQntlNowToLocal();
/* static's */
static bool		init_done = FALSE;
static APPL		*cur_app;	/* Current application */
static USER_FRAME	*cur_frame;	/* Current frame */
static i4		cur_index;	/* Current frame's index */
static AC_LIST		*ac_frames;	/* Frames in application */
static i4		no_ac_frames;	/* Number of frames */
static 
  i4 tf_rows;
static READONLY
  char _form[]= 	ERx("vqrecapp"), _appname[]= 	ERx("appname"), _prompt[]= 	ERx("prompt") 
  , _tf[]=		ERx("report"), _tfcol[]= 	ERx("report"), _blank[]= 	ERx("");
static bool	do_next_frame();
static VOID	view_final_report();
/*{
** Name:  IIVQraReconcileApp      Reconcile visual queries with database
**
** Description:
**	This is the main routine for reconciling the visual queries in an 
**	application with the database.
**
** Inputs:
**	app	APPL *	The current application.  The application components
**			have been loaded, including the metaframes, but not
**			the VQs.
**
**	framecat bool	Whether we were called from frame catalog.  If not, 
**			we were called from the application catalog.  In
**			the former case, we've already got a lock on 
**			the application.
** Outputs:
**	none
**
**	Returns:
**		STATUS
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	12/12/89 (Mike S)	Initial version
*/
STATUS
IIVQraReconcileApp(app, framecat)
  APPL *app;
bool framecat;
{
	bool any;
	/* Get a list of frames to reconcile */
	_VOID_ IIVQcacChooseApplComp(app, IIVQradReconcileAppDiscreps, 
		IIVQsvfSelVQFrames, TRUE, ERget(F_VQ012F_MarkFrames), 
		"vqmarkfm.hlp", ERget(F_VQ012F_MarkFrames), (PTR)&framecat,
		&any);
	/* Give up if there are none */
	if (!any)
	{
		IIUGerr(E_VQ009E_No_VQ_Frames, 0, 1, app->name);
		return;
	}
}
/*{
** Name:	IIVQradReconcileAppDiscreps - Reconcile desired frames 
**
** Description:
**	The user has chosen the frames he wants to reconcile.  We 
**	lock them, reconcile them, let the user examine them, and let
**	him decide whether to accept the results.
**
** Inputs:
**	app		APPL *		Aplication
**	frames  	AC_LIST *	list of frames
**	no_frames	i4		size of list
**	data		PTR		Whether we were called from frame
**					catalog
**
** Outputs:
**	none
**
**	Returns:
**		OK
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	1/7/90 (Mike S)	Initial version	
*/
STATUS	IIVQradReconcileAppDiscreps(app, frames, no_frames, data)
APPL *app;
AC_LIST *frames;
i4  	no_frames;
PTR	data;
{
	i4 count;	/* Number of frames actally used */
	bool *framecat = (bool *)data;	
			/* Were we called from the frame catalog */
	i4 i;
	/* Lock the application, if need be, and all the frames chosen */
	if (!*framecat)
		if (IIABcelCheckEditLock((APPL_COMP *)app) != OK)
			return (FAIL);
	/*
	** Make the utility pointer NULL if we're not going to do
	** this one due to locking 
	*/
	for (i = 0, count = 0; i < no_frames; i++)
	{
		if (frames[i].used)
		{
			if (IIABcelCheckEditLock(frames[i].comp) != OK)
			{
				frames[i].utility = (PTR)NULL;
			}
			else
			{
				frames[i].utility = (PTR)(frames + i);
				count++;
			}
		}
	}
	/* Give up if there are no frames left */
	if (count == 0)
	{      
		IIUGerr(E_VQ00A0_No_Frames, 0, 1, app->name);
		return (OK);
	}
	/* Reconcile each frame.  First display the form */
	if (!init_done)
	{
		IIARiaForm(_form);
		init_done = TRUE;
	}
/* # line 209 "vqrecapp.qsc" */	/* display */
  {
    if (IIdispfrm(_form,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
      char title_buf[80];
/* # line 214 "vqrecapp.qsc" */	/* inittable */
      {
        if (IItbinit(_form,_tf,(char *)"r") != 0) {
          IItfill();
        } /* IItbinit */
      }
/* # line 215 "vqrecapp.qsc" */	/* host code */
		_VOID_ IIUGfmt(title_buf, sizeof(title_buf), 
				ERget(S_VQ00A6_Reconcile_Report), 1, app->name);
/* # line 217 "vqrecapp.qsc" */	/* loadtable */
      {
        if (IItbact(_form,_tf,1) != 0) {
          IItcoputio(_tfcol,(short *)0,1,32,0,title_buf);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 218 "vqrecapp.qsc" */	/* host code */
		_VOID_ IIUGfmt(title_buf, sizeof(title_buf), 
				ERget(S_VQ00A7_Reconcile_Begun), 1, 
				IIVQntlNowToLocal());
/* # line 221 "vqrecapp.qsc" */	/* loadtable */
      {
        if (IItbact(_form,_tf,1) != 0) {
          IItcoputio(_tfcol,(short *)0,1,32,0,title_buf);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 222 "vqrecapp.qsc" */	/* loadtable */
      {
        if (IItbact(_form,_tf,1) != 0) {
          IItcoputio(_tfcol,(short *)0,1,32,0,_blank);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 224 "vqrecapp.qsc" */	/* host code */
		/* Set up application name */
/* # line 225 "vqrecapp.qsc" */	/* putform */
      {
        if (IIfsetio(_form) != 0) {
          IIputfldio(_appname,(short *)0,1,32,0,app->name);
        } /* IIfsetio */
      }
/* # line 226 "vqrecapp.qsc" */	/* putform */
      {
        if (IIfsetio(_form) != 0) {
          IIputfldio(_prompt,(short *)0,1,32,0,_blank);
        } /* IIfsetio */
      }
/* # line 228 "vqrecapp.qsc" */	/* redisplay */
      {
        IIredisp();
      }
/* # line 230 "vqrecapp.qsc" */	/* host code */
		/* Get number of rows in report tablefield */
/* # line 231 "vqrecapp.qsc" */	/* inquire_frs */
      {
        if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&tf_rows,30,_tf,0);
        } /* IIiqset */
      }
/* # line 233 "vqrecapp.qsc" */	/* host code */
		/* Reconcile frames util we need user input */
		cur_index = -1;
		cur_app = app;
		ac_frames = frames;
		no_ac_frames = no_frames;
		if (!do_next_frame())
		{
			view_final_report();
/* # line 241 "vqrecapp.qsc" */	/* breakdisplay */
      {
        if (1) goto IIfdE1;
      }
/* # line 242 "vqrecapp.qsc" */	/* host code */
		}
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
          char accept[80];
/* # line 250 "vqrecapp.qsc" */	/* host code */
		/* Use the reconciled metaframe */
		IIVQgnmGetNewMF(cur_frame->mf);
		/* Write changes to database */	
		if ((cur_frame->mf->updmask & MF_P_ESC) != 0)
			IIVQsecSaveEscCode(cur_frame->mf, TRUE);
		IIAMmuMetaUpdate(cur_frame->mf, cur_frame->mf->updmask);
		IIAMmfMetaFree(cur_frame->mf, cur_frame->mf->popmask);
		IIVQfvtFreeVqTag();
		cur_frame->mf->state |= (MFST_DOGEN|MFST_DOFORM);
		IIAMufqUserFrameQuick((OO_OBJECT *)cur_frame);
		/* Put notice in report */
		_VOID_ IIUGfmt(accept, sizeof(accept), 
			ERget(S_VQ00C0_Changes_Accepted), 1, cur_frame->name);
/* # line 265 "vqrecapp.qsc" */	/* loadtable */
          {
            if (IItbact(_form,_tf,1) != 0) {
              IItcoputio(_tfcol,(short *)0,1,32,0,accept);
              IITBceColEnd();
            } /* IItbact */
          }
/* # line 266 "vqrecapp.qsc" */	/* loadtable */
          {
            if (IItbact(_form,_tf,1) != 0) {
              IItcoputio(_tfcol,(short *)0,1,32,0,_blank);
              IITBceColEnd();
            } /* IItbact */
          }
/* # line 268 "vqrecapp.qsc" */	/* host code */
		/* Reconcile frames util we need user input */
		if (!do_next_frame())
		{
			view_final_report();
/* # line 272 "vqrecapp.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
/* # line 273 "vqrecapp.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 2) {
        {
          char reject[80];
/* # line 281 "vqrecapp.qsc" */	/* host code */
		/* Free VQ from memory */
		IIAMmfMetaFree(cur_frame->mf, cur_frame->mf->popmask);
		IIVQfvtFreeVqTag();
		/* Put notice in report */
		_VOID_ IIUGfmt(reject, sizeof(reject), 
			ERget(S_VQ00C1_Changes_Rejected), 1, cur_frame->name);
/* # line 288 "vqrecapp.qsc" */	/* loadtable */
          {
            if (IItbact(_form,_tf,1) != 0) {
              IItcoputio(_tfcol,(short *)0,1,32,0,reject);
              IITBceColEnd();
            } /* IItbact */
          }
/* # line 289 "vqrecapp.qsc" */	/* loadtable */
          {
            if (IItbact(_form,_tf,1) != 0) {
              IItcoputio(_tfcol,(short *)0,1,32,0,_blank);
              IITBceColEnd();
            } /* IItbact */
          }
/* # line 291 "vqrecapp.qsc" */	/* host code */
		/* Reconcile frames util we need user input */
		if (!do_next_frame())
		{
			view_final_report();
/* # line 295 "vqrecapp.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
/* # line 296 "vqrecapp.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 3) {
        {
/* # line 302 "vqrecapp.qsc" */	/* host code */
		/* Use the old metaframe */
		IIVQgomGetOldMF(cur_frame->mf);
		/* Examine VQ */
		IIVQvqsVqShow(cur_frame->mf);
        }
      } else if (IIretval() == 4) {
        {
/* # line 312 "vqrecapp.qsc" */	/* host code */
		/* Use the new metaframe */
		IIVQgnmGetNewMF(cur_frame->mf);
		/* Examine VQ */
		IIVQvqsVqShow(cur_frame->mf);
        }
      } else if (IIretval() == 5) {
        {
          char *msg;
/* # line 322 "vqrecapp.qsc" */	/* host code */
	    bool dummy;
		if (IIUFtfuTblFldUnload(_form, _tf, _tfcol, &msg, &dummy) != OK)
		{
/* # line 326 "vqrecapp.qsc" */	/* message */
          {
            IImessage(msg);
          }
/* # line 327 "vqrecapp.qsc" */	/* sleep */
          {
            IIsleep(2);
          }
/* # line 328 "vqrecapp.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 6) {
        {
/* # line 335 "vqrecapp.qsc" */	/* host code */
		FEhelp(ERx("vqrecapp.hlp"), ERget(S_VQ009A_Reconcile_Banner));
        }
      } else if (IIretval() == 7) {
        {
/* # line 341 "vqrecapp.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_tf,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 8) {
        {
/* # line 347 "vqrecapp.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_tf,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,-1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 9) {
        {
/* # line 353 "vqrecapp.qsc" */	/* host code */
                FEtabfnd(_form, _tf);
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
    if (IInmuact(ERget(FE_OK),ERget(F_VQ0132_Reconcile_OK),2,2,1) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Cancel),ERget(F_VQ0133_Reconcile_Cancel),2,2,2) == 0
    ) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact(ERget(F_VQ0130_ExamineOld),ERget(F_VQ0135_Examine_OldVQ),2,2,
    3) == 0) goto IIfdE1;
    if (IInmuact(ERget(F_VQ0131_ExamineNew),ERget(F_VQ0134_Examine_NewVQ),2,2,
    4) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_WriteFile),ERget(F_FE010E_WriteExpl),2,2,5) == 0) 
    goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),2,2,6) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,6) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,7) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,8) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,9) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 356 "vqrecapp.qsc" */	/* host code */
	/* Release all our locks.  This is easy if we were called from the 
	** application catalog; we can release all of them.  If we were called 
	** from the frame catalog, we have to keep the application lock, and 
	** release all the frame locks separately. 
	*/
	if (!*framecat)
	{
		_VOID_ IIABrlRemLock(OC_UNDEFINED);
	}
	else
	{
		for (i = 0; i < no_frames; i++)
			if (frames[i].used && 
			    (frames[i].utility != (PTR)NULL))
				_VOID_ IIABrlRemLock(frames[i].comp->ooid);
	}
	return OK;
}
/*
**	do_next_frame()
**
**	Reconcile  frames until we need user input, i.e. until a change occurs.
**
**	Returns 	TRUE if there is a next frame
**			FALSE if we're done.
*/
static bool
do_next_frame()
{
	bool	changes;	/* Were changes required */
  i4 toprow;
  char out_buf[80];
	bool	locked;		/* Was frame locked */
	/* Erase prompt */
/* # line 393 "vqrecapp.qsc" */	/* putform */
  {
    if (IIfsetio(_form) != 0) {
      IIputfldio(_prompt,(short *)0,1,32,0,_blank);
    } /* IIfsetio */
  }
/* # line 394 "vqrecapp.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 396 "vqrecapp.qsc" */	/* host code */
	for (cur_index++; cur_index < no_ac_frames; cur_index++)
	{
		if (ac_frames[cur_index].used)
		{
			cur_frame = (USER_FRAME *)ac_frames[cur_index].comp;
			IIUGmsg(ERget(S_VQ009B_Reconciling), FALSE, 1, 
				cur_frame->name);
/* # line 403 "vqrecapp.qsc" */	/* inquire_frs */
  {
    if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,30,4,&toprow,32,_tf,0);
    } /* IIiqset */
  }
/* # line 404 "vqrecapp.qsc" */	/* host code */
			toprow++;
			locked = ((PTR)NULL == ac_frames[cur_index].utility);
			changes = IIVQrfdReconcileFrameDiscreps(
					cur_frame, locked);
			/* Scroll to this frame's report */
/* # line 410 "vqrecapp.qsc" */	/* scroll */
  {
    if (IItbsetio(1,_form,_tf,-3) != 0) {
      IItbsmode((char *)"to");
      if (IItscroll(0,-1) != 0) {
      } /* IItscroll */
    } /* IItbsetio */
  }
/* # line 411 "vqrecapp.qsc" */	/* scroll */
  {
    if (IItbsetio(1,_form,_tf,-3) != 0) {
      IItbsmode((char *)"to");
      if (IItscroll(0,toprow) != 0) {
      } /* IItscroll */
    } /* IItbsetio */
  }
/* # line 413 "vqrecapp.qsc" */	/* host code */
			if (changes)
			{
				/* Output the prompt string */
				FTbell();
				_VOID_ IIUGfmt(out_buf, sizeof(out_buf), 
					ERget(S_VQ00C5_Accept_Changes),
					2, ERget(FE_OK), (PTR)cur_frame->name);
/* # line 420 "vqrecapp.qsc" */	/* putform */
  {
    if (IIfsetio(_form) != 0) {
      IIputfldio(_prompt,(short *)0,1,32,0,out_buf);
    } /* IIfsetio */
  }
/* # line 421 "vqrecapp.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 422 "vqrecapp.qsc" */	/* host code */
				return TRUE;
			}
			else
			{
/* # line 426 "vqrecapp.qsc" */	/* scroll */
  {
    if (IItbsetio(1,_form,_tf,-3) != 0) {
      IItbsmode((char *)"to");
      if (IItscroll(0,toprow) != 0) {
      } /* IItscroll */
    } /* IItbsetio */
  }
/* # line 427 "vqrecapp.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 428 "vqrecapp.qsc" */	/* host code */
			}
		}	
	}
	/* No frames left */
	IIUGmsg(ERget(S_VQ009C_Reconcile_Done), TRUE, 0);
	/* Print trailing message */
/* # line 436 "vqrecapp.qsc" */	/* loadtable */
  {
    if (IItbact(_form,_tf,1) != 0) {
      IItcoputio(_tfcol,(short *)0,1,32,0,_blank);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 437 "vqrecapp.qsc" */	/* host code */
	_VOID_ IIUGfmt(out_buf, sizeof(out_buf), 
			ERget(S_VQ00A8_Reconcile_Ended), 1, 
			IIVQntlNowToLocal());
/* # line 440 "vqrecapp.qsc" */	/* loadtable */
  {
    if (IItbact(_form,_tf,1) != 0) {
      IItcoputio(_tfcol,(short *)0,1,32,0,out_buf);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 441 "vqrecapp.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 443 "vqrecapp.qsc" */	/* host code */
	return (FALSE);
}
/*
**	Give the user one last chance to write out the report
*/
static VOID
view_final_report()
{
  char pr_buf[80];
	/* Initialize -- scroll report to top, and change prompt */
/* # line 456 "vqrecapp.qsc" */	/* scroll */
  {
    if (IItbsetio(1,_form,_tf,-3) != 0) {
      IItbsmode((char *)"to");
      if (IItscroll(0,1) != 0) {
      } /* IItscroll */
    } /* IItbsetio */
  }
/* # line 457 "vqrecapp.qsc" */	/* host code */
	_VOID_ IIUGfmt(pr_buf, sizeof(pr_buf), 
			       ERget(S_VQ00C6_End_Reconcile),
			       1, ERget(FE_End));
/* # line 460 "vqrecapp.qsc" */	/* putform */
  {
    if (IIfsetio(_form) != 0) {
      IIputfldio(_prompt,(short *)0,1,32,0,pr_buf);
    } /* IIfsetio */
  }
/* # line 461 "vqrecapp.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 463 "vqrecapp.qsc" */	/* host code */
	/* A simple submenu -- let the user:
	**	Write the report to disk
	**	Get help
	**	End
	**	Move around the report
	*/
/* # line 470 "vqrecapp.qsc" */	/* display */
  {
    if (IInestmu() == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
IIfdB2:
    while (IIrunnest() != 0) {
      if (IIretval() == 1) {
        {
          char *msg;
/* # line 476 "vqrecapp.qsc" */	/* host code */
		bool dummy;
       	       	if (IIUFtfuTblFldUnload(_form, _tf, _tfcol, &msg, &dummy) != OK)
       	       	{
/* # line 480 "vqrecapp.qsc" */	/* message */
          {
            IImessage(msg);
          }
/* # line 481 "vqrecapp.qsc" */	/* sleep */
          {
            IIsleep(2);
          }
/* # line 482 "vqrecapp.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 2) {
        {
/* # line 489 "vqrecapp.qsc" */	/* host code */
               	FEhelp(ERx("vqrecapd.hlp"), ERget(S_VQ009A_Reconcile_Banner));
        }
      } else if (IIretval() == 3) {
        {
/* # line 496 "vqrecapp.qsc" */	/* clear */
          {
            IIfldclear(_tf);
          }
/* # line 497 "vqrecapp.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 503 "vqrecapp.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_tf,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 509 "vqrecapp.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_tf,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,-1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 515 "vqrecapp.qsc" */	/* host code */
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
    if (IInmuact(ERget(FE_WriteFile),ERget(F_FE010E_WriteExpl),2,2,1) == 0) 
    goto IIfdE2;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),2,2,2) == 0) 
    goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,2) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,3) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,4) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,5) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,6) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendnest();
  }
/* # line 517 "vqrecapp.qsc" */	/* host code */
}
