# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<er.h>
# include	<tm.h>
# include	<me.h>
# include	<lo.h>
# include	<st.h>
# include	<cm.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include       <ooclass.h>
# include <metafrm.h>
# include <abclass.h>
# include       <abfcnsts.h>
# include 	<erug.h>
# include 	<erfg.h>
# include 	"vqsecnam.h"
# include 	"ervq.h"
/**
** Name:	vqcmperr.qsc - Display compilation errors
**
** Description:
**	This file defines:
**
**	IIVQdceDisplayCompErrs - Display compilation errors
**
** History:
**	8/22/89 (Mike S)	Initial version
**	2/28/90	(Mike S)	Change calling sequence for 
**				IIVQdceDisplayCompErrs
**	31-jul-90 (mgw)
**		renamed _active to _isactive since _active is a reserved
**		word for Sequent's ptx C compiler.
**	12-dec-90 (blaise)
**		Added decode_location(), to decode encoded "\", "/" and " "
**		in menuitems. Bug numbers 34458, 34869.
**	06-apr-92 (rocky, dchan)
**              Put IIVQoefOpenErrorFile() in front of get_listfile_time()
**              in load_error_list to prevent stack get overwrite.  Stolen
**              from Bull workaround.
**	26-aug-92 (blaise)
**		Added error processing for local procedures.
**	22-mar-93 (rudyw)
**              Back out 06-apr change which resulted in the wrong error file
**		being associated with a given flagged error.(Bugs 46967/49149)
**	01-dec-95 (kch)
**		In the function find_error_frames(), the error message buffer,
**		msgbuf, has been increased in size from 80 to 100 characters
**		to accommodate application names of up to 32 characters. This
**		change fixes bug 71043.
**      04-dec-95 (chimi03)
**              Removed optimization for hp8_us5 due to internal error
**              from the compiler. The compiler error was "internal error 6590".
**	19-jan-1999 (muhpa01)
**		Removed NO_OPTIM for hp8_us5.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/* # define's */
/* relevant tablefield states */
# define TS_UNCHANGED	2
# define TS_CHANGED	3
# define TS_DELETED	4
/* Define some frame superclasses */
# define FSC_VQ		1		/* VQ-type frames */
# define FSC_UF	 	2		/* Other User Frames */
# define FSC_4P 	3		/* 4GL procedures */
# define FSC_3P 	4		/* 3GL procedures */
# define FSC_OTHER 	-1		/* Source-codeless frames */
# define MAX_FRMERR	20		/* Errors we'll display per frame */
/* Describe text tablefield.
*/
# define TOP_CONTEXT	1		/* Display 1 lines above error line */
# define BOTTOM_CONTEXT	4		/* Display 4 lines below error line */
/* Define type for frame list */
  typedef struct {
	APPL_COMP *frame;	/* Frame structure */
	i4	  sc;		/* Frame superclass */
	i4	  no_errs;	/* Number of error rows for frame */
	i4	  warnings;	/* Number of warnings for frame */
	SYSTIME   listfile_time; /* Modification date for listing file */
    char summary[80];
	bool	  badVQ;	/* Frame has a bad Visual Query */
	bool	  listexists;	/* Does compilation listing exist */
	bool	  user_edit;	/* Has the user edited Emerald-produced code */
	bool	  placeholder;	/* Do we have a placeholder error row */
  } ERR_FRAME;
/* GLOBALDEF's */
/* extern's */
FUNC_EXTERN STATUS	iiabFileEdit();
FUNC_EXTERN APPL_COMP   *iiabGetComp();
FUNC_EXTERN LOCATION	*iiabMkLoc();
FUNC_EXTERN VOID	FEhelp();
FUNC_EXTERN STATUS	IIABfrcCompile();
FUNC_EXTERN STATUS	IIABfdirFile();
FUNC_EXTERN VOID	IIABgcvGoCompVision();
FUNC_EXTERN STATUS	IIABmefMakeErrFile();
FUNC_EXTERN STATUS	IIAMufqUserFrameQuick();
FUNC_EXTERN STATUS	IIUFtfnTblFldNiLoad();
FUNC_EXTERN bool	IIUFver();
FUNC_EXTERN VOID	IIUGbmaBadMemoryAllocation();
FUNC_EXTERN VOID	IIUGerr();
FUNC_EXTERN VOID	IIUGdtsDateToSys();
FUNC_EXTERN char	*IIUGfmt();
FUNC_EXTERN VOID	IIUGmsg();
FUNC_EXTERN VOID	IIUGqsort();
FUNC_EXTERN bool	IIUGyn();
FUNC_EXTERN STATUS	IIVQcefCloseErrorFile();
FUNC_EXTERN VOID	IIVQdlfDisplayListFile();
FUNC_EXTERN VOID	IIVQeqpEditQueryParts();
FUNC_EXTERN VOID 	IIVQeseEditSpecificEscape();
FUNC_EXTERN STATUS	IIVQgneGetNextError();
FUNC_EXTERN STATUS	IIVQoefOpenErrorFile();
FUNC_EXTERN STATUS	IIVQparams();
FUNC_EXTERN VOID	IIVQpsnParseSectionName();
FUNC_EXTERN STATUS	IIVQuefUpdateErrorFile();
FUNC_EXTERN VOID	IIVQveVqError();
GLOBALREF bool  IIabVision;
GLOBALREF PTR IIVQcontext;
/* static's */
static READONLY char routine[] = ERx("IIVQdceDisplayCompErrs");
/* Form name -- vqcmperr for Vision, vqcmper2 otherwise */
static
  char *_form;
/*
**	Note: The "summary" field is used in vqlisfil.qsc.  Don't change
**	its usage or contents without checking there.
*/
static READONLY
  char _title[]= 	ERx("title"), _errtf[]= 	ERx("errors"), _namecol[]= 	ERx("framename")
  , _loccol[]= 	ERx("location"), _summary[]= 	ERx("summary"), _fixedcol[]= 	ERx("fixed")
  , _texttf[]= 	ERx("current_error"), _textcol[]= 	ERx("text"), _empty[]= 	ERx("")
  , _loc_hcol[]= 	ERx("hidden_location"), _rowno_hcol[]= 	ERx("rowno"), 
  _recordno_hcol[]= 	ERx("recordno"), _frameno_hcol[]= 	ERx("frameno"), 
  _wasfixed_hcol[]= 	ERx("wasfixed"), _isfixed_hcol[]= 	ERx("isfixed"), 
  _err_no_hcol[]= 	ERx("err_no"), _secline_hcol[]= 	ERx("secline");
/* Saved "yes" and "no" strings */
static
  char *_yes, *_no;
static bool	init_vqcmperr = FALSE;
static 
  ERR_FRAME *err_frames;
static i4	no_err_frames;		/* Number of frames with errors */
static 
  ERR_FRAME *cur_err_frame;
static i4	 cur_err_no;		/* current error number */
static i4	 cur_err_rec;		/* current record in error tablefield */
static
  i4 cur_err_row;
static i4	 cur_txt_rec;		/* record in text tablefield 
					   for current error */
/* This flag is used to prevent recursive calls to this routine */
static _isactive = FALSE;
/* If we're displaying primarily for a certain frame, this is it */
static APPL_COMP *first_frame;
/* current listing file name */
static LOCATION lstloc;
static char	lstfilbuf[MAX_LOC+1];
static char 	*lstfil;
static VOID	add_error_row();
static bool 	check_listfile_time();
static i4	cmp_frame();
static VOID	decode_loc();
static VOID	display_details();
static bool	find_error_frames();
static char 	*frame_or_proc();
static STATUS	get_listfile_time();
static i4	get_superclass();
static bool	has_errors();
static VOID	load_errors();
static i4	load_error_list();
static bool	load_listing_file();
static VOID	make_frame_list();
static VOID	make_summary();
static VOID 	no_errors_left();
static VOID	output_title();
static VOID	reload_errors();
static VOID	scroll_text_to();
static VOID	update_fixed_flags();
static VOID	update_frame();
static bool	user_edited();
/*{
** Name:	IIVQdceDisplayCompErrs - Display compilation errors
**
** Description:
**	Display the compilation errors for an application.  We show differing 
**	amounts of information for different frame types:
**
**	    For VQ frames, we show the error's location, an error summary,
**	    and the error itself.
**
**	    For other User Frames and 4GL procedures, we show the error
**	    summary and the listing file.
**	
**	    For 3GL procedures, we just show the fact of the error.
**
**	If a frame is specified, it's emphasized by putting its errors first
**	in the list.  Other frames are listed in alphabetical order.
**
**	If we were called explicitly, or as the result of an "Image" failure,
**	we'll just display the errors.  If we were called as the result of
**	an interpreter-compile failure, we'll ask first.
**
**      We use the listing file as a clue that recompilation has taken place.
**      If its date has changed, we refresh frame data from the catalog.
**      This will never tell us that another frame has compilation errors,
**      but it might tell us that a frame has different ones, or that it
**      has been fixed by a concurrent user.
**
** Inputs:
**	appl		APPL *		application to disply errors for
**	frame		OO_OBJECT *	frame to emphasize. (may be NULL)
**	ask		bool		Should we ask whether to display errors?
**
** Outputs:
**	none
**
**	Returns:
**		bool		TRUE if the error display was entered
**				FALSE if the user choose not to
**
**	Exceptions:
**		none
**
** Side Effects:
**		New compilations may be performed.  Source code may be edited
**		or regenerated.
**
** History:
**	9/22/89 (Mike S)	Initial version
*/
bool
IIVQdceDisplayCompErrs(appl, frame, ask)
APPL		*appl;
OO_OBJECT	*frame;
bool		ask;
{
	char		*cur_field;	/* Current form field */
	/* 
	** If we're already active, just return.  This will happen if a
	** compile started from this frame fails.
	*/
	if (_isactive) return FALSE;
	/*
	** If we were called because an interpreter-compile failed, ask
	** whether to display the errors.
	*/
	if (ask)
	{
/* # line 293 "vqcmperr.qsc" */	/* message */
  {
    IImessage(ERx(" "));
  }
/* # line 295 "vqcmperr.qsc" */	/* host code */
		if (!IIUFver(ERget(S_VQ0020_EditErrors), 0))
			return FALSE;
	}
	/* 
	** Make a list of frames which currently have errors.  
	*/
	first_frame = (APPL_COMP *)frame;
	if (!find_error_frames(appl))
		return FALSE;	/* No errors to display */
	/* Display the error list form */
	if (!init_vqcmperr)
	{
		/* The non-Vision form doesn't include the Location column. */
		if (IIabVision)
			_form = ERx("vqcmperr");
		else
			_form = ERx("vqcmper2");
		IIARiaForm(_form);
		init_vqcmperr = TRUE;
		/* Get "yes" and "no" strings */
		_yes = ERget(F_UG0002_Yes2);
		_no = ERget(F_UG0007_No2);
	}
/* # line 322 "vqcmperr.qsc" */	/* display */
  {
    if (IIdispfrm(_form,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 325 "vqcmperr.qsc" */	/* host code */
		/* Hidden columns:
		**	rowno - tablefield row in which error is displayed
		**	recordno - file record in which error message occurs
		**	frameno - frame in which error occured
		**	wasfixed - was error originally marked "fixed"
		**	isfixed - is error currently marked "fixed"
		**	err_no - zero-based index of this error within 
		**		 frame's errors
		**	hidden_location - non-wrapped version of error
		**		location string
		**	secline - line number of error within error section
		**
		**	If IIabVision is FALSE, "location" is a hidden 
		**    	column as well., 
		*/
		if (IIabVision)
		{
/* # line 342 "vqcmperr.qsc" */	/* inittable */
      {
        if (IItbinit(_form,_errtf,(char *)"r") != 0) {
          IIthidecol(_rowno_hcol,(char *)"i4");
          IIthidecol(_recordno_hcol,(char *)"i4");
          IIthidecol(_frameno_hcol,(char *)"i2");
          IIthidecol(_wasfixed_hcol,(char *)"i2");
          IIthidecol(_isfixed_hcol,(char *)"i2");
          IIthidecol(_err_no_hcol,(char *)"i2");
          IIthidecol(_loc_hcol,(char *)"varchar(80)");
          IIthidecol(_secline_hcol,(char *)"i2");
          IItfill();
        } /* IItbinit */
      }
/* # line 348 "vqcmperr.qsc" */	/* host code */
		}
		else
		{
/* # line 351 "vqcmperr.qsc" */	/* inittable */
      {
        if (IItbinit(_form,_errtf,(char *)"r") != 0) {
          IIthidecol(_rowno_hcol,(char *)"i4");
          IIthidecol(_recordno_hcol,(char *)"i4");
          IIthidecol(_frameno_hcol,(char *)"i2");
          IIthidecol(_wasfixed_hcol,(char *)"i2");
          IIthidecol(_isfixed_hcol,(char *)"i2");
          IIthidecol(_err_no_hcol,(char *)"i2");
          IIthidecol(_loc_hcol,(char *)"varchar(80)");
          IIthidecol(_secline_hcol,(char *)"i2");
          IIthidecol(_loccol,(char *)"varchar(80)");
          IItfill();
        } /* IItbinit */
      }
/* # line 357 "vqcmperr.qsc" */	/* host code */
		}
/* # line 358 "vqcmperr.qsc" */	/* inittable */
      {
        if (IItbinit(_form,_texttf,(char *)"r") != 0) {
          IItfill();
        } /* IItbinit */
      }
/* # line 360 "vqcmperr.qsc" */	/* host code */
		/* Initialize and load the error display tablefield */
		load_errors(err_frames, no_err_frames, -1);
		output_title(appl);
		cur_err_rec = -1;
		cur_err_frame = NULL;
		display_details((i4)1);
		cur_field = _errtf;
		_isactive = TRUE;
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
          i4 rowsleft;
/* # line 376 "vqcmperr.qsc" */	/* host code */
		/* 
		** If we've just come from the text tablefield, force the
		** previous error to be redisplayed.  If we have stayed on 
		** the error tablefield, load the current row's error.
		*/
		if (cur_field != _errtf)
		{
			i4 	rec;
			rec = cur_err_rec;
			cur_err_rec = -1;
			display_details(rec);
		}
		else
		{
			display_details((i4)-1);
		}
/* # line 394 "vqcmperr.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&rowsleft,32,_errtf,0);
            } /* IIiqset */
          }
/* # line 395 "vqcmperr.qsc" */	/* host code */
		if (rowsleft == 0)
		{
			no_errors_left(appl);
/* # line 398 "vqcmperr.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
/* # line 399 "vqcmperr.qsc" */	/* host code */
		}
		/* Set the current field */
		cur_field = _errtf;
        }
      } else if (IIretval() == 2) {
        {
          i4 rowsleft;
/* # line 409 "vqcmperr.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&rowsleft,32,_errtf,0);
            } /* IIiqset */
          }
/* # line 410 "vqcmperr.qsc" */	/* host code */
		if (rowsleft == 0)
		{
			no_errors_left(appl);
/* # line 413 "vqcmperr.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
/* # line 414 "vqcmperr.qsc" */	/* host code */
		}
		/* Set the current field */
		cur_field = _texttf;
        }
      } else if (IIretval() == 3) {
        {
/* # line 422 "vqcmperr.qsc" */	/* host code */
		/* Avoid out-of-data message */
          i4 tfrec;
          char tblname[32];
/* # line 426 "vqcmperr.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,32,31,tblname,19,(char *)0,0);
            } /* IIiqset */
          }
/* # line 427 "vqcmperr.qsc" */	/* getrow */
          {
            if (IItbsetio(2,_form,tblname,-2) != 0) {
              IItcogetio((short *)0,1,30,4,&tfrec,(char *)"_RECORD");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 429 "vqcmperr.qsc" */	/* host code */
		if (1 != tfrec)
		{
/* # line 431 "vqcmperr.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,tblname,-3) != 0) {
              IItbsmode((char *)"down");
              if (IItscroll(0,-3) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 432 "vqcmperr.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 4) {
        {
/* # line 437 "vqcmperr.qsc" */	/* host code */
		/* Avoid out-of-data message */
          i4 tfrec, tfrows;
          char tblname[32];
/* # line 441 "vqcmperr.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,32,31,tblname,19,(char *)0,0);
              IIiqfsio((short *)0,1,30,4,&tfrows,32,(char *)0,0);
            } /* IIiqset */
          }
/* # line 442 "vqcmperr.qsc" */	/* getrow */
          {
            if (IItbsetio(2,_form,tblname,-2) != 0) {
              IItcogetio((short *)0,1,30,4,&tfrec,(char *)"_RECORD");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 444 "vqcmperr.qsc" */	/* host code */
		if (tfrows != tfrec)
		{
/* # line 446 "vqcmperr.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,tblname,-3) != 0) {
              IItbsmode((char *)"up");
              if (IItscroll(0,-3) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 447 "vqcmperr.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 5) {
        {
/* # line 452 "vqcmperr.qsc" */	/* host code */
		/* Mark it fixed, if we can */
		if (cur_err_frame->sc != FSC_3P && !cur_err_frame->listexists)
		{
			IIUGerr(E_VQ0083_NoListFile, 0, 1, 
				cur_err_frame->frame->name);
		}
		else
		{
/* # line 460 "vqcmperr.qsc" */	/* putrow */
          {
            if (IItbsetio(3,_form,_errtf,cur_err_row) != 0) {
              IItcoputio(_isfixed_hcol,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
              IItcoputio(_fixedcol,(short *)0,1,32,0,_yes);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 462 "vqcmperr.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 6) {
        {
/* # line 468 "vqcmperr.qsc" */	/* host code */
		/* Mark it unfixed, if we can */
		if (cur_err_frame->sc != FSC_3P && !cur_err_frame->listexists)
		{
			IIUGerr(E_VQ0083_NoListFile, 0, 1, 
				cur_err_frame->frame->name);
		}
		else
		{
/* # line 476 "vqcmperr.qsc" */	/* putrow */
          {
            if (IItbsetio(3,_form,_errtf,cur_err_row) != 0) {
              IItcoputio(_isfixed_hcol,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
              IItcoputio(_fixedcol,(short *)0,1,32,0,_no);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 478 "vqcmperr.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 7) {
        {
          i2 frameno;
          char section[80];
          i4 rowno;
          i4 secline;
/* # line 489 "vqcmperr.qsc" */	/* host code */
		ERR_FRAME *erframe;
		APPL_COMP *cur_frame;
		bool success;
		i4 i;
		i4 major, minor;	/* Section types */
		char name[80];		/* Section name */
		METAFRAME *metaframe;	/* Metaframe pointer */
		char *source;
		/* Get error information */
/* # line 499 "vqcmperr.qsc" */	/* getrow */
          {
            if (IItbsetio(2,_form,_errtf,cur_err_row) != 0) {
              IItcogetio((short *)0,1,30,2,&frameno,_frameno_hcol);
              IItcogetio((short *)0,1,32,79,section,_loc_hcol);
              IItcogetio((short *)0,1,30,4,&rowno,_rowno_hcol);
              IItcogetio((short *)0,1,30,4,&secline,_secline_hcol);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 503 "vqcmperr.qsc" */	/* host code */
		erframe = err_frames + frameno;
		cur_frame = erframe->frame;
		/* If it's a VQ error, we can't fix it */
		if (erframe->badVQ)
		{
			IIUGerr(E_VQ0087_CannotFixVQError, 0, 1, 
				cur_frame->name);
/* # line 511 "vqcmperr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 512 "vqcmperr.qsc" */	/* host code */
		}
		/* See if we have a new listing file */
		if (!check_listfile_time(erframe))
		{
			i4	start;
			/* Different situation.  Make him choose again */
			_VOID_ iiabGetComp(cur_frame->appl, cur_frame->ooid);
			reload_errors(erframe, E_VQ00E1_ErrorsNoMore, &start);
			cur_err_rec = -1;
			cur_err_frame = NULL;
			if (has_errors(cur_frame))
			{
				/* Display new errors for this frame */
				display_details((i4)start);
			}
			else
			{
				/* Display first error */
				display_details((i4)1);
			}
/* # line 534 "vqcmperr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 535 "vqcmperr.qsc" */	/* host code */
		}
		switch (erframe->sc)
		{
		    case FSC_VQ:
			metaframe = ((USER_FRAME *)cur_frame)->mf;
			/* If we don't have the listing file, give up */
			if (!erframe->listexists)
			{
				IIUGerr(E_VQ0083_NoListFile, 0, 1, 
					cur_frame->name);
				success = FALSE;
				break;
			}
			/* parse location and call proper routine */
			IIVQpsnParseSectionName(section, &major, &minor, name);
			switch (major)
			{
			    case EST_ESCAPE:
				IIVQeseEditSpecificEscape(metaframe,
					minor, name, secline);
				success = TRUE;
				break;
			    case EST_HIDDEN:
				_VOID_ IIVQlocals((OO_OBJECT *)cur_frame);
				success = TRUE;
				break;
			    case EST_LOCPROC:
				IIVQlprocs((OO_OBJECT *)cur_frame);
				success = TRUE;
				break;
			    case EST_CALL:
				/* Find the associated menuitem's index */
				for (i = 0; i < metaframe->nummenus; i++)
				{
					if (STcompare(name, 
					     metaframe->menus[i]->text) == 0)
					{
						IIVQparams(metaframe, i);
						success = TRUE;
						break;
					}
				}
				if (i >= metaframe->nummenus)
				{
					_VOID_ IIUGerr(
						E_VQ0066_NoSuchMenuitem,
						0, 2, cur_frame->name, name);
					success = FALSE;
				}
				break;
			    case EST_DEFAULT:
				/* Defaults only work for master section */
				minor = EST_QMASTER;
				/* Fall through */
			    case EST_SELECT:
			    case EST_INSERT:
				IIVQeqpEditQueryParts((USER_FRAME *)cur_frame,
						      major, minor);
				success = TRUE;
				break;
			    case EST_NONE:
				/* Error occured before first section */
				_VOID_ IIUGerr(
					  E_VQ0067_ErrorOutsideSection, 
					  0, 0);
				success = FALSE;
				break;
			    case EST_SOURCE:
				/* User edited the source */
				success = iiabFileEdit( cur_frame->appl,
					cur_frame,
					((USER_FRAME *)cur_frame)->source)
				      == OK;
				break;
			    case EST_UNKNOWN:
			    default:
				/* Unknown error */
				_VOID_ IIUGerr(
					 E_VQ0068_UnknownErrorLocation, 
					 TRUE, 1, section);
				success = FALSE;
			}
			break;
                   case FSC_UF:
                       source = ((USER_FRAME *)cur_frame)->source;
                       goto edit_source;       
                   case FSC_4P:
                       source = ((_4GLPROC *)cur_frame)->source;
                       goto edit_source;       
                   case FSC_3P:
                       source = ((HLPROC *)cur_frame)->source;
                       goto edit_source;       
                   edit_source:
                       success = iiabFileEdit(
                                       cur_frame->appl, cur_frame, source)
				      == OK;
/* # line 644 "vqcmperr.qsc" */	/* redisplay */
          {
            IIredisp();
          }
/* # line 645 "vqcmperr.qsc" */	/* host code */
			break;
		}
		/* If we have success, mark the thing fixed */
		if (success)
		{
/* # line 651 "vqcmperr.qsc" */	/* putrow */
          {
            if (IItbsetio(3,_form,_errtf,cur_err_row) != 0) {
              IItcoputio(_isfixed_hcol,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
              IItcoputio(_fixedcol,(short *)0,1,32,0,_yes);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 653 "vqcmperr.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 8) {
        {
/* # line 659 "vqcmperr.qsc" */	/* host code */
		ERR_FRAME *erframe;	/* Current frame */
		APPL_COMP *cur_frame;	/* Current frame */
          i2 frameno;
		i4 start;		/* Record for frame's first error */
		/* 
		** Get error info for curent error.
		*/
/* # line 667 "vqcmperr.qsc" */	/* getrow */
          {
            if (IItbsetio(2,_form,_errtf,cur_err_row) != 0) {
              IItcogetio((short *)0,1,30,2,&frameno,_frameno_hcol);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 669 "vqcmperr.qsc" */	/* host code */
		/* Set up to compile and go do it */
		erframe = err_frames + frameno;
		cur_frame = erframe->frame;
		_VOID_ IIABfrcCompile( cur_frame->appl, cur_frame );
		/* Reload the tablefield with updated error info */
		reload_errors(erframe, E_VQ0086_GoodCompile, &start);
		/* Force redisplay of error and listing file */
		cur_err_rec = -1;
		cur_err_frame = NULL;
		if (has_errors(cur_frame))
		{
			/* Display new errors for this frame */
			display_details((i4)start);
		}
		else
		{
			/* Display first error */
			display_details((i4)1);
		}
		/* Resume on the error tablefield, on the current error */
/* # line 693 "vqcmperr.qsc" */	/* resume */
          {
            IIrescol(_errtf,_namecol);
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 9) {
        {
          i4 textrow;
          i4 textrec;
/* # line 702 "vqcmperr.qsc" */	/* host code */
		/* Display the error file using the full screen */
		if (!cur_err_frame->listexists)
		{
			IIUGerr(E_VQ0083_NoListFile, 0, 1, 
				cur_err_frame->frame->name);
		}
		else
		{
			/* Pass the current record number */
/* # line 711 "vqcmperr.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&textrow,29,_texttf,0);
            } /* IIiqset */
          }
/* # line 712 "vqcmperr.qsc" */	/* getrow */
          {
            if (IItbsetio(2,_form,_texttf,textrow) != 0) {
              IItcogetio((short *)0,1,30,4,&textrec,(char *)"_RECORD");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 713 "vqcmperr.qsc" */	/* host code */
			IIVQdlfDisplayListFile(_form, lstfil, textrec);
		}
        }
      } else if (IIretval() == 10) {
        {
/* # line 721 "vqcmperr.qsc" */	/* host code */
		char *hlpfile = IIabVision ? 
					ERx("vqcmperr.hlp") : 
					ERx("vqcmper2.hlp");
		FEhelp(hlpfile, ERget(S_VQ0026_EditCompileErrs));
        }
      } else if (IIretval() == 11) {
        {
/* # line 732 "vqcmperr.qsc" */	/* host code */
		update_fixed_flags();
/* # line 733 "vqcmperr.qsc" */	/* clear */
          {
            IIfldclear(_errtf);
            IIfldclear(_texttf);
          }
/* # line 734 "vqcmperr.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 12) {
        {
/* # line 739 "vqcmperr.qsc" */	/* host code */
                /* top */
          char fldname[33];
/* # line 742 "vqcmperr.qsc" */	/* inquire_frs */
          {
            if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,32,32,fldname,22,_form,0);
            } /* IIiqset */
          }
/* # line 743 "vqcmperr.qsc" */	/* host code */
                if (STequal(fldname, _errtf))
			display_details(1);
                else
/* # line 746 "vqcmperr.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_texttf,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 13) {
        {
/* # line 751 "vqcmperr.qsc" */	/* host code */
                /* bottom */
          i4 no_rows;
          char fldname[33];
/* # line 755 "vqcmperr.qsc" */	/* inquire_frs */
          {
            if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,32,32,fldname,22,_form,0);
            } /* IIiqset */
          }
/* # line 756 "vqcmperr.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&no_rows,32,fldname,0);
            } /* IIiqset */
          }
/* # line 758 "vqcmperr.qsc" */	/* host code */
                if (STequal(fldname, _errtf))
			display_details(no_rows);
                else
/* # line 761 "vqcmperr.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_texttf,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,no_rows) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 14) {
        {
/* # line 766 "vqcmperr.qsc" */	/* host code */
                /* find */
          char fldname[33];
/* # line 769 "vqcmperr.qsc" */	/* inquire_frs */
          {
            if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,32,32,fldname,22,_form,0);
            } /* IIiqset */
          }
/* # line 770 "vqcmperr.qsc" */	/* host code */
                FEtabfnd(_form, fldname);
        }
      } else {
        if (1) goto IIfdE1;
      } /* IIretval */
    } /* IIrunform */
IIfdF1:
    if (IIchkfrm() == 0) goto IIfdB1;
    goto IIfdE1;
IImuI1:
    if (IITBcaClmAct(_errtf,(char *)"all",1,1) == 0) goto IIfdE1;
    if (IITBcaClmAct(_texttf,(char *)"all",1,2) == 0) goto IIfdE1;
    if (IIactscrl(_errtf,1,3) == 0) goto IIfdE1;
    if (IIactscrl(_texttf,1,3) == 0) goto IIfdE1;
    if (IIactscrl(_errtf,0,4) == 0) goto IIfdE1;
    if (IIactscrl(_texttf,0,4) == 0) goto IIfdE1;
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(F_VQ003A_MarkFixed),ERget(F_VQ007A_MarkFixedExpl),2,2,5)
     == 0) goto IIfdE1;
    if (IInmuact(ERget(F_VQ003B_MarkUnfixed),ERget(F_VQ007B_MarkNotFixedExpl	)
    ,2,2,6) == 0) goto IIfdE1;
    if (IInmuact(ERget(F_VQ0037_FixError),ERget(F_VQ0077_FixErrorExpl),2,2,7) 
    == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Compile),ERget(F_VQ0074_CompileOneExpl),2,2,8) == 0
    ) goto IIfdE1;
    if (IInmuact(ERget(F_VQ0039_ListingFile),ERget(F_VQ0079_ListingFileExpl),2
    ,2,9) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0101_ExplKeyHelp),2,2,10) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,10) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,11) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,11) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,12) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,13) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,14) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 774 "vqcmperr.qsc" */	/* host code */
	/* Free dynamic memory */
	if (err_frames != NULL)
		MEfree(err_frames);
	/* Clean up and exit */
	_isactive = FALSE;
	return TRUE;
}
/*
**	Find out which frames have errors.
**
**	returns TRUE if any have errors.
*/
static bool
find_error_frames(appl)
APPL	*appl;         /* Application to check */
{
	/* Make a list of the frames with errors */
        make_frame_list(appl);
        if (no_err_frames == 0)
        {
  char msgbuf[100];
		IIUGfmt(msgbuf, sizeof(msgbuf), ERget(E_VQ0080_NoApplErrors),
			1, appl->name);
/* # line 800 "vqcmperr.qsc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(msgbuf);
  }
/* # line 802 "vqcmperr.qsc" */	/* host code */
                return (FALSE);
        }
	/* We found some errors */
	return (TRUE);
}
/*
**	Make a list of the frames in this application which currently 
**	have errors
*/
static VOID
make_frame_list(appl)
APPL	*appl;		/* Application to check */
{
	i4		pass;		/* Pass through frame loop */
	APPL_COMP 	*comp;		/* Current component */
	i4		sc;		/* Superclas for this component */
	i4		i;
	/* First count them, then make the list */
	for (pass = 0; pass < 2; pass ++)
	{
		if (pass == 0)
			no_err_frames = 0;
		else
			i = 0;
		comp = (APPL_COMP *)appl->comps;
		while(comp != NULL)
		{
			bool	in_error;
			METAFRAME *mf;
			sc = get_superclass(comp);
			/* There are three indications of error:
			** 1.	The VQ is bad (bit MFST_VQEERR)
			** 2.   compdate is non-blank
			** 3.  	The warning bit is set.
			*/
			in_error = FALSE;
			switch (sc)
			{
			    case FSC_VQ:
				mf = ((USER_FRAME *)comp)->mf;
				if ((mf->state & MFST_VQEERR) != 0)
				{
					in_error = TRUE;
					break;
				}
				/* 
				** If it appears to be a custom frame, but the
				** CUSTOM bit isn't set, be sure we have the 
				** latest generation date.
				*/
				if (pass == 0 && has_errors(comp) &&
				    user_edited((USER_FRAME *)comp) &&
				    (comp->flags & CUSTOM_EDIT) == 0
				   )
				{
					_VOID_ iiabGetComp(appl, comp->ooid);
				}
				/* Fall through */
			    case FSC_UF:
			    case FSC_4P:
			    case FSC_3P:
				in_error =  (has_errors(comp));
				if (in_error && pass == 0)
				{
					SYSTIME *dummy;
					/*
					** Check for listing file.  If there 
					** isn't one, reread the component
					** from the database.
					*/
					if (get_listfile_time(comp, &dummy)
					    != OK)
					{
						if (iiabGetComp(appl, 
								comp->ooid)
							!= NULL)
						{
							/* We reread it */
							in_error = 
							    has_errors(comp);
						}
					}
				}
			}
			if (in_error)
			{
				if (pass == 0) 
				{
					no_err_frames++;
				}
				else
				{
					err_frames[i].frame = comp;
					err_frames[i].sc = sc;
					err_frames[i].user_edit =  
					     ((sc == FSC_VQ) && 
					       user_edited((USER_FRAME *)comp));
					i++;
				}
			}
			comp = comp->_next;
		} 
		if (pass == 1) break;
		/* Allocate the "frames with errors" array */
		if (no_err_frames > 0)
		{
			err_frames = (ERR_FRAME *) MEreqmem(
					(u_i4)0, 
					no_err_frames * sizeof(ERR_FRAME),
					FALSE,
					(STATUS*)NULL);
			if (err_frames == NULL)
				IIUGbmaBadMemoryAllocation(routine);
		}
		else
		{
			return;
		}
	}
	/* 
	** Sort the list by these criteria:
	**	1. If "first_frame" is non-NULL, it goes first.
	** 	2. Otherwise, all frames and procedures are sorted 
	**	   alphabetically.
	*/
	IIUGqsort((char *)err_frames, no_err_frames, 
		  sizeof(ERR_FRAME), cmp_frame);
}
/*
**	Compare two frames by the criteria shown above 
*/
static i4
cmp_frame(frame1, frame2)
ERR_FRAME *frame1;
ERR_FRAME *frame2;
{
	if (frame1->frame == first_frame)
		return -1;
	else if (frame2->frame == first_frame)
		return 1;
	else
		return (STcompare(frame1->frame->name, frame2->frame->name));
}
/*
**	Remove any exisitng errors and load the new errors, if any, into the
**	tablefield.
*/
static VOID
reload_errors(erframe, no_errs_msg, start_ptr)
ERR_FRAME	*erframe;	/* Current frame */
ER_MSGID	no_errs_msg;	/* Message if frame now has no errors */
i4		*start_ptr;	/* First record number for this frame */
  {
	APPL_COMP *cur_frame = erframe->frame;
				/* Current frame */
    i2 err_no;
    i4 currow;
    i4 rowsleft;
    i4 before_rec;
    i4 start;
    i4 i;
	/* Get the current error number */
/* # line 978 "vqcmperr.qsc" */	/* getrow */
    {
      if (IItbsetio(2,_form,_errtf,cur_err_row) != 0) {
        IItcogetio((short *)0,1,30,2,&err_no,_err_no_hcol);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 980 "vqcmperr.qsc" */	/* host code */
	/* Delete all the tablefield entries for this frame */
	start = cur_err_rec - err_no;
	for (i = start; i < start + erframe->no_errs; i++)
	{
/* # line 984 "vqcmperr.qsc" */	/* scroll */
    {
      if (IItbsetio(1,_form,_errtf,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,start) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 985 "vqcmperr.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&currow,29,_errtf,0);
      } /* IIiqset */
    }
/* # line 986 "vqcmperr.qsc" */	/* deleterow */
    {
      if (IItbsetio(4,_form,_errtf,currow) != 0) {
        if (IItdelrow(0) != 0) {
        } /* IItdelrow */
      } /* IItbsetio */
    }
/* # line 987 "vqcmperr.qsc" */	/* host code */
	}
/* # line 988 "vqcmperr.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&rowsleft,32,_errtf,0);
      } /* IIiqset */
    }
/* # line 990 "vqcmperr.qsc" */	/* host code */
	if (has_errors(cur_frame))
	{
		/* If it failed, add new errors and/or warnings */
		before_rec = (start > rowsleft) ? -1 : start;
		load_errors(erframe, 1, before_rec);
	}
	else
	{	
    char msgbuf[160];
		IIUGfmt(msgbuf, sizeof(msgbuf), ERget(no_errs_msg), 2, 
			cur_frame->name, frame_or_proc(erframe));
/* # line 1003 "vqcmperr.qsc" */	/* message */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      IImessage(msgbuf);
    }
/* # line 1005 "vqcmperr.qsc" */	/* host code */
		erframe->no_errs = 0;
		erframe->warnings = 0;
	}
	*start_ptr = start;
	/* Update error totals */
	output_title(erframe->frame->appl);
  }
/*
**	Load the errors into the tablefield
*/
static VOID
load_errors(frame_list, no_frames, before_rec)
ERR_FRAME *frame_list;	/* List of frames with errors */
i4	no_frames;	/* Size of list */
i4	before_rec;	/* Record to load error tablefield before */
{
	register APPL_COMP *cur_frame;	/* Frame being loaded */
	ERR_FRAME	*frame;		/* Frame being loaded */
	i2	frameno;	/* frame number */
	char	*unavail;	/* "Unavailable" */
	bool 	was_fixed;	/* Was error fixed */
	char	buffer[80];	/* Message buffer */
	char	*invq;		/* "Error in VQ" pointer */
	i4	no_errs;	/* Errors in list file */
	i4	i;
	METAFRAME *mf;
	SYSTIME	*listfile_time;
	for (i = 0; i < no_frames; i++)
	{
		frame = frame_list + i;
		cur_frame = frame->frame;
		frameno = frame - err_frames;
		frame->warnings = 0;
		frame->summary[0] = EOS;
		frame->placeholder = FALSE;
		frame->listexists = TRUE;
		frame->badVQ = FALSE;
		switch(frame->sc)
		{
		    case FSC_VQ:
			/* If the VQ is in error, load that fact */
			mf = ((USER_FRAME *)cur_frame)->mf;
			if ((mf->state & MFST_VQEERR) != 0)
			{
				/* Get error description */
				IIVQveVqError(mf, buffer, sizeof(buffer) - 1);
				invq = ERget(F_VQ0126_ErrorInVq);
				add_error_row(before_rec, (i2)frameno, invq,
					(i2)FALSE, buffer, (i4)-1, (i4)-1,
					(i2)0, (i2)-1);
				frame->listexists = FALSE;
				frame->no_errs = 1;
				frame->badVQ = TRUE;
				break;
			}
			/* else fall through */
		    case FSC_UF:
		    case FSC_4P:
			/* Load errors from file */
			no_errs = load_error_list(frame, before_rec);
			/* 
			** If we got no errors from the list file, reread the
			** component.
			*/
			if (no_errs == 0)
			{
				if (iiabGetComp(cur_frame->appl, 
				    	        cur_frame->ooid) != NULL &&
				    !has_errors(cur_frame))
				{
					frame->no_errs = frame->warnings = 0;
				}
				else
				{
					frame->placeholder = TRUE;
					frame->no_errs = 1;
					frame->warnings = 0;
					unavail = ERget(F_VQ00FB_Unavailable);
					add_error_row(before_rec, (i2)frameno, 
						_empty, (i2)FALSE, unavail, 
						(i4)-1, (i4)-1, (i2)0, (i2)-1);
				}
			}
			else
			{
				frame->no_errs = no_errs;
			}
			break;
		    case FSC_3P:
                        if (get_listfile_time(frame->frame, &listfile_time)
                            == OK)
                        {
                                STRUCT_ASSIGN_MACRO(*listfile_time,
                                                    frame->listfile_time);
                        }
                        else
                        {
                                frame->listexists = FALSE;
                        }
			frame->no_errs = 1;
			frame->warnings = 0;
			frame->placeholder = TRUE;
			was_fixed = (cur_frame->flags & APC_UFERRS) == 0;
			add_error_row(before_rec, (i2)frameno, _empty,
				(i2)was_fixed, _empty, (i4)1, (i4)1,
				(i2)0, (i2)-1);
			break;
		}
	}
}
/*
** Load errors from list file.  
*/
static i4
load_error_list(frame, before_rec)
ERR_FRAME *frame;
i4	before_rec;		/* Record to insert errors before */
{
	char summary[160];	/* Error summary */
 	char section[100];	/* Error section */
	bool sectlist;		/* Does section give listing ? */
	i4 error_row;	/* Tablefield row for error */
	i4 error_record;	/* File record for error */
	i4	secline;	/* Error line within section */
	bool	wasfixed;	/* Was error already fixed */
	i4	errors = 0;	/* Errors loaded */
	SYSTIME	*lisfiletime;
	bool	warning;
	/*
	** Get date of listing file
	** The order of the routines in the conditional part are significant
	** since get_listfile_time() establishes the current file name info
	** in structure lstloc which IIVQoefOpenErrorFile() then uses.
	*/
	if ( get_listfile_time(frame->frame,&lisfiletime) != OK ||
                IIVQoefOpenErrorFile(&lstloc) != OK )
	{
		frame->listexists = FALSE;
		return (errors);	/* Couldn't open file */
	}
	STRUCT_ASSIGN_MACRO(*lisfiletime, frame->listfile_time);
	for ( ; errors < MAX_FRMERR; errors++ )
	/* Retrieve errors */
	{
		/* Get next error */
		if (IIVQgneGetNextError(160, 80, &sectlist, summary, section, 
			&error_row, &error_record, &secline, &wasfixed, 
			&warning) 
		    != OK)
			return (errors);
		if (warning)
			++(frame->warnings);
		/* 
		** If this is a user-edited VQ frame, all errors are in
		** the "Source code"
		*/
		if (frame->user_edit)
		{
			STcopy(ERget(F_FG002B_Source_Code), section);
			secline = -1;
		}
		else if (!sectlist)
		{
			if (frame->sc == FSC_VQ)
			{
				/* Give up if we see a "-l"-type error 
				** for a VQ frame 
				*/
				return (errors);
			}
			else
			{
				/* Make dummy values for section info */
				*section = EOS;
				secline = -1;
			}
		}
		/* Load tablefield */
		add_error_row(before_rec, (i2)(frame - err_frames), section,
			      (i2)wasfixed, summary, (i4)error_row, 
			      (i4)error_record, (i2)errors, (i2)secline);
		if (before_rec >= 0)
			before_rec++;
	}
	/* Close file and return */
	IIVQcefCloseErrorFile();
	return (errors);
}
/*
**	Put a row into the error tablefield.  This will be an inserttable, if
**	the row to insert it before is positive, or a loadtable otherwise.
*/
static VOID
add_error_row(before_rec, frame_index, location, isitfixed, summcol,
		rowno_hcol, recordno_hcol, err_no_hcol, secline_hcol)
  i4 before_rec;
  i2 frame_index;
  char *location;
  i2 isitfixed;
  char *summcol;
  i4 rowno_hcol;
  i4 recordno_hcol;
  i2 err_no_hcol;
  i2 secline_hcol;
  {
    char *framename = err_frames[frame_index].frame->name;
    char dlocation[80];
    char *divider;
    char *fixed;
    i4 cur_row;
    char decoded_loc[80];
	/* Make "fixed" flag */
	fixed = isitfixed ? _yes : _no;
	/* Decode location (encoded before compilation) */
	decode_loc(location, decoded_loc);
	/* Break location into pieces, so it can word_wrap */
	STcopy(decoded_loc, dlocation);
	divider = STindex(dlocation, ERx("\\"), 0);
	if (divider != NULL) *divider = ' ';
	if (before_rec <= 0)
	{
		/* If record number is non-positive, do a loadtable */
/* # line 1248 "vqcmperr.qsc" */	/* loadtable */
    {
      if (IItbact(_form,_errtf,1) != 0) {
        IItcoputio(_namecol,(short *)0,1,32,0,framename);
        IItcoputio(_loccol,(short *)0,1,32,0,dlocation);
        IItcoputio(_summary,(short *)0,1,32,0,summcol);
        IItcoputio(_fixedcol,(short *)0,1,32,0,fixed);
        IItcoputio(_rowno_hcol,(short *)0,1,30,4,&rowno_hcol);
        IItcoputio(_recordno_hcol,(short *)0,1,30,4,&recordno_hcol);
        IItcoputio(_frameno_hcol,(short *)0,1,30,2,&frame_index);
        IItcoputio(_err_no_hcol,(short *)0,1,30,2,&err_no_hcol);
        IItcoputio(_wasfixed_hcol,(short *)0,1,30,2,&isitfixed);
        IItcoputio(_isfixed_hcol,(short *)0,1,30,2,&isitfixed);
        IItcoputio(_loc_hcol,(short *)0,1,32,0,decoded_loc);
        IItcoputio(_secline_hcol,(short *)0,1,30,2,&secline_hcol);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1257 "vqcmperr.qsc" */	/* host code */
	}
	else
	{
		/* Otherwise insert it before the desired record */
/* # line 1261 "vqcmperr.qsc" */	/* scroll */
    {
      if (IItbsetio(1,_form,_errtf,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,before_rec) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 1262 "vqcmperr.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&cur_row,29,_errtf,0);
      } /* IIiqset */
    }
/* # line 1263 "vqcmperr.qsc" */	/* host code */
		cur_row--;
/* # line 1264 "vqcmperr.qsc" */	/* insertrow */
    {
      if (IItbsetio(5,_form,_errtf,cur_row) != 0) {
        if (IItinsert() != 0) {
          IItcoputio(_namecol,(short *)0,1,32,0,framename);
          IItcoputio(_loccol,(short *)0,1,32,0,dlocation);
          IItcoputio(_summary,(short *)0,1,32,0,summcol);
          IItcoputio(_fixedcol,(short *)0,1,32,0,fixed);
          IItcoputio(_rowno_hcol,(short *)0,1,30,4,&rowno_hcol);
          IItcoputio(_recordno_hcol,(short *)0,1,30,4,&recordno_hcol);
          IItcoputio(_frameno_hcol,(short *)0,1,30,2,&frame_index);
          IItcoputio(_err_no_hcol,(short *)0,1,30,2,&err_no_hcol);
          IItcoputio(_wasfixed_hcol,(short *)0,1,30,2,&isitfixed);
          IItcoputio(_isfixed_hcol,(short *)0,1,30,2,&isitfixed);
          IItcoputio(_loc_hcol,(short *)0,1,32,0,decoded_loc);
          IItcoputio(_secline_hcol,(short *)0,1,30,2,&secline_hcol);
          IITBceColEnd();
        } /* IItinsert */
      } /* IItbsetio */
    }
/* # line 1273 "vqcmperr.qsc" */	/* host code */
	}
  }
/*
**	Display error details for a given error tablefield row 
*/
static VOID
display_details(rec)
  i4 rec;
  {
    i4 lineno;
    i2 frameno;
    i2 err_no;
    i4 rowsleft;
	i4 start;		/* Start of errors if they're reloaded */	
start_over:
	/* Just return if there are no rows left */
/* # line 1292 "vqcmperr.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&rowsleft,32,_errtf,0);
      } /* IIiqset */
    }
/* # line 1293 "vqcmperr.qsc" */	/* host code */
	if (rowsleft == 0)
		return;
	if (rec == cur_err_rec)
	{
		/* 
		** We're already at the right error.  Just scroll the listing
		** file to the right place.
		*/
		lineno = cur_txt_rec;
	}
	else
	{
		/* Get error tablefield row */
		if (rec > 0)
		{
			/* scroll to the error */
/* # line 1311 "vqcmperr.qsc" */	/* scroll */
    {
      if (IItbsetio(1,_form,_errtf,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,rec) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 1312 "vqcmperr.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&cur_err_row,29,_errtf,0);
      } /* IIiqset */
    }
/* # line 1313 "vqcmperr.qsc" */	/* host code */
		}
		else
		{
			/* Get current record number */
/* # line 1317 "vqcmperr.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&cur_err_row,29,_errtf,0);
      } /* IIiqset */
    }
/* # line 1318 "vqcmperr.qsc" */	/* getrow */
    {
      if (IItbsetio(2,_form,_errtf,cur_err_row) != 0) {
        IItcogetio((short *)0,1,30,4,&rec,(char *)"_RECORD");
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1319 "vqcmperr.qsc" */	/* host code */
		}
/* # line 1321 "vqcmperr.qsc" */	/* getrow */
    {
      if (IItbsetio(2,_form,_errtf,cur_err_row) != 0) {
        IItcogetio((short *)0,1,30,2,&frameno,_frameno_hcol);
        IItcogetio((short *)0,1,30,4,&lineno,_rowno_hcol);
        IItcogetio((short *)0,1,30,2,&err_no,_err_no_hcol);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1325 "vqcmperr.qsc" */	/* host code */
		/* Load the correct listing file */
		cur_err_rec = rec;
		if (!load_listing_file(frameno, &start))
		{
			ERR_FRAME* erframe;	/* Current frame */
			/* 
			** We loaded a different listing file than expected.
			** We've already told the user about this, and reloaded
			** the component from the database.  Now we'll scroll to
			** the first error for the component, if it has any, and
			** the top row otherwise.
			*/
/* # line 1339 "vqcmperr.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&rowsleft,32,_errtf,0);
      } /* IIiqset */
    }
/* # line 1340 "vqcmperr.qsc" */	/* host code */
			if (rowsleft == 0)
				return;	/* No errors left */
			erframe = err_frames + frameno;
			if (has_errors(erframe->frame))
			{
				rec = start;
			}
			else
			{
				/*
				** The frame selected no longer has errors.
				** Go to the top, and try again.
				*/
				cur_err_frame = NULL;	
						/* Force listing file reload */
/* # line 1356 "vqcmperr.qsc" */	/* scroll */
    {
      if (IItbsetio(1,_form,_errtf,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,1) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 1357 "vqcmperr.qsc" */	/* host code */
				rec = -1;
				goto start_over;
			}
/* # line 1361 "vqcmperr.qsc" */	/* scroll */
    {
      if (IItbsetio(1,_form,_errtf,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,rec) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 1362 "vqcmperr.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&cur_err_row,29,_errtf,0);
      } /* IIiqset */
    }
/* # line 1363 "vqcmperr.qsc" */	/* host code */
		}
		/* Setup new "current" statics */
		cur_err_frame = err_frames + frameno;
		cur_err_no = err_no;
		cur_err_rec = rec;
		/* Put up the summary line for this frame */
		if (EOS == cur_err_frame->summary[0])
			make_summary(cur_err_frame);
/* # line 1373 "vqcmperr.qsc" */	/* putform */
    {
      if (IIfsetio(_form) != 0) {
        IIputfldio(_summary,(short *)0,1,32,0,cur_err_frame->summary);
      } /* IIfsetio */
    }
/* # line 1374 "vqcmperr.qsc" */	/* host code */
	}
	/* Scroll to the correct listing line */
	scroll_text_to(lineno);
  }
/*
**	Load a new listing file into the text tablefield.
**	We use the current row of the error tablefield to tell us
**	what to load.
**
**	Inputs:		frameno		The index of the current frame
**	Outputs:	start		The record number of the first row
**					for this frame.  This is set
**					only if FALSE is returned.
**	Returns:	TRUE if the old list of errors is still there
**			FALSE if we reloaded new ones
*/
static bool
load_listing_file(frameno, start)
i4	frameno;
i4	*start;
{
	ERR_FRAME	*frame;		/* Current frame */
	bool		status;		/* Was the expected file loaded */
	status = TRUE;
	/* See if it's the current frame */
	frame = err_frames + frameno;
	if (frame == cur_err_frame)
		return TRUE;		/* It's already loaded */
	/* clear tablefield */
/* # line 1408 "vqcmperr.qsc" */	/* clear */
  {
    IIfldclear(_texttf);
  }
/* # line 1410 "vqcmperr.qsc" */	/* host code */
	if (!check_listfile_time(frame))
	{
		/* 
		** It changed.  Reread the component, and reload the 
		** listing file.
		*/
		status = FALSE;
		_VOID_ iiabGetComp(frame->frame->appl, frame->frame->ooid);
		reload_errors(frame, E_VQ00E1_ErrorsNoMore, start);
		if (!has_errors(frame->frame))
			return FALSE;		/* No errors for this frame */
	}
	if (frame->listexists)
	{
		LOtos(&lstloc, &lstfil);
		/* load the file into the tablefield */
		status = TRUE;
		if (IIUFtfnTblFldNiLoad(&lstloc, _form, _texttf, _textcol) 
			!= OK)
		{
/* # line 1432 "vqcmperr.qsc" */	/* clear */
  {
    IIfldclear(_texttf);
  }
/* # line 1433 "vqcmperr.qsc" */	/* host code */
			frame->listexists = FALSE;
			lstfil = ERget(F_VQ00FB_Unavailable);
		}
	}
	else
	{
		lstfil = ERget(F_VQ00FB_Unavailable);
	}
	return status;
}
/*
**	Scroll text tablefield to a certain line.
*/
static VOID
scroll_text_to(lineno)
i4 lineno;
  {
    i4 records;
    i4 toprecord;
    i4 scrollrow;
	i4 topline = lineno - TOP_CONTEXT;	
			/* The record which should appear in row 1 */
	/* Do nothing for an invalid request */
	if (lineno <= 0) return;
	/* See if the file is that long */
/* # line 1462 "vqcmperr.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&records,32,_texttf,0);
      } /* IIiqset */
    }
/* # line 1464 "vqcmperr.qsc" */	/* host code */
	if (records == 0)
	{
		/* Tablefield is empty.  We've already complained about this */
		cur_txt_rec = -1;
	}
	else
	{
		/* The display should look like:
		**
		**	1. (top context)
		**	2. Error line   --- "lineno"
		**	3. caret line
		**	4. blank/FIXED
		**	5. <<* Error, ...
		** 	6. E_OSxxxx
		**
		**	The current scrolling behavior is to move as little
		**	as possible.  So if we're above the desired position
		**	we scroll to line 6; if below it, we scroll to line 1.
		*/
/* # line 1484 "vqcmperr.qsc" */	/* getrow */
    {
      if (IItbsetio(2,_form,_texttf,1) != 0) {
        IItcogetio((short *)0,1,30,4,&toprecord,(char *)"_RECORD");
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1485 "vqcmperr.qsc" */	/* host code */
		if (toprecord < topline)
			scrollrow = lineno + BOTTOM_CONTEXT;
		else if (toprecord > topline)
			scrollrow = topline;
		else
			return;
		scrollrow = max(min(scrollrow, records), 1);
/* # line 1493 "vqcmperr.qsc" */	/* scroll */
    {
      if (IItbsetio(1,_form,_texttf,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,scrollrow) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 1494 "vqcmperr.qsc" */	/* host code */
		cur_txt_rec = lineno;
	}
  }
/*
** Make summary line for a frame
*/
static VOID
make_summary(frame)
ERR_FRAME *frame;
{
	char *comp_type;
	i4 errors;
	i4 warnings;
	/* Set component type */
	switch (frame->sc)
	{
		case FSC_VQ:
		case FSC_UF:
			comp_type = ERget(F_VQ0030_Frame);
			break;
		default:
			comp_type = ERget(F_VQ0031_Procedure);
			break;
	}
	/* Format message */
	if (frame->placeholder)
	{
		IIUGfmt(frame->summary, sizeof(frame->summary), 
			ERget(F_VQ0026_Summ), 2, comp_type, 
			frame->frame->name);
	}
	else
	{
		errors = frame->no_errs - (warnings = frame->warnings);
		IIUGfmt(frame->summary, sizeof(frame->summary),
			ERget(F_VQ0024_ErrSumm), 4, comp_type, 
			frame->frame->name, (PTR)&errors, (PTR)&warnings);
	}
}
/*
** Update the "fixed" flags in the catalogs and list files.
*/
static VOID
update_fixed_flags()
  {
    char cisfixed[20];
    i2 frameno;
    i2 wasfixed;
    i2 isfixed;
    i4 recordno;
    i4 rowstate;
	i4 cur_frameno = -1;	/* Frame number we're working on */
	i4 no_errs = 0;	/* Errors to change for current frame */
	i4 err_records[MAX_FRMERR];	/* Record numbers */
	bool err_fixed[MAX_FRMERR];		/* Fixed or not fixed */
	/* Loop through error tablefield */
/* # line 1557 "vqcmperr.qsc" */	/* unloadtable */
    {
      if (IItbact(_form,_errtf,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,19,cisfixed,_fixedcol);
        IItcogetio((short *)0,1,30,2,&frameno,_frameno_hcol);
        IItcogetio((short *)0,1,30,2,&wasfixed,_wasfixed_hcol);
        IItcogetio((short *)0,1,30,2,&isfixed,_isfixed_hcol);
        IItcogetio((short *)0,1,30,4,&recordno,_recordno_hcol);
        IItcogetio((short *)0,1,30,4,&rowstate,(char *)"_STATE");
        IITBceColEnd();
        {
/* # line 1562 "vqcmperr.qsc" */	/* host code */
		if (TS_CHANGED == rowstate || TS_UNCHANGED == rowstate)
		{
			if (frameno != cur_frameno)
			{
				/* fix it */
				if (no_errs > 0)
					update_frame(cur_frameno, no_errs, 
						     err_records, err_fixed);
				cur_frameno = frameno;
				no_errs = 0;
			}
			/* gather the relevant information */
			if (isfixed != wasfixed)
			{
				err_records[no_errs] = recordno;
				err_fixed[no_errs] = isfixed;
				no_errs++;
			}
		}
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 1584 "vqcmperr.qsc" */	/* host code */
	/* Fix the last one, if there's anything to do */
	if (no_errs > 0) 
		update_frame(cur_frameno, no_errs, err_records, err_fixed);
  }
/*
**	Update a frame with new "fixed" information/
*/
static VOID 
update_frame(frameno, no_errs, err_records, err_fixed)
i4	frameno;	/* frame index to update */
i4	no_errs;	/* Number of errors to change */
i4 err_records[];	/* record numbers of errors to change */
bool	err_fixed[];	/* Whether errors are fixed or not fixed */	
{
	ERR_FRAME *frame = err_frames + frameno;/* Current frame */
	APPL_COMP *cur_frame = frame->frame;	/* ditto */
	LOCATION  listloc;			/* List file location */
	char 	  lbuffer[MAX_LOC + 1];		/* location buffer */
	STATUS	  status;
	/* Update error */
	if (frame->sc == FSC_3P)
	{
		bool	was_fixed;		/* Was error already fixed */
		bool	changed;		/* Did fix status change */
		/* Set (or clear) unfixed flag */
		was_fixed = (cur_frame->flags & APC_UFERRS) == 0;
		if (err_fixed[0])
		{
			cur_frame->flags &= ~APC_UFERRS;
			changed = !was_fixed;
		}
		else
		{
			cur_frame->flags |= APC_UFERRS;
			changed = was_fixed;
		}
		/* Update the database if it changed */
		if (changed)
			_VOID_ IIAMufqUserFrameQuick((OO_OBJECT *)cur_frame);
	}
	else
	{
		/* Rewrite listing file */
		if (frame->listexists)
		{
			IIABmefMakeErrFile(cur_frame, &listloc, lbuffer);
			if ((status = IIVQuefUpdateErrorFile(
					&listloc, no_errs, 
					err_records, err_fixed))
				!= OK)
				_VOID_ IIUGerr(
					E_VQ0065_CannotUpdateListFile, 
					0, 2, cur_frame->name, &status);
		}
	}
}
/*
**	user_edited
**
**	Returns TRUE if the user appears to have edited the generated code, 
**	i.e. if the source file is newer than the generation date.  This may
**	be true because we don't have the latest generation date, but our
**	caller will handle this.
*/
static bool user_edited(comp)
USER_FRAME	*comp;	/* Component to check */
{
	LOCATION *sloc;			/* Source location */
	METAFRAME *mf = comp->mf; 	/* Component's metaframe */
	i4 loinflags;			/* LOinfo flags */
        LOINFORMATION   locinfo;	/* Location information */
	SYSTIME	stime;			/* Time source was gen'ed */
	/* Get source location and file change time */
	sloc = iiabMkLoc( comp->appl->source, comp->source );
	if(LOexist(sloc) != OK)
	{
		/* No source at all */
		return FALSE;
	}
	loinflags = LO_I_LAST;
	if (LOinfo(sloc, &loinflags, &locinfo) != OK)
	{
		/* 
		** This shouldn't happen.  Let's assume that if we can't read
		** read it, he couldn't edit it.
		*/
		return FALSE;
	}
	/* Get Gen time */
	IIUGdtsDateToSys(mf->gendate,&stime);
	/* Compare the two */
	return (TMcmp(&stime,&(locinfo.li_last)) < 0);
}
/*
**	Output title line, including total errors and warnings.
*/
static VOID
output_title(appl)
APPL	*appl;		/* Current application */
{
  char title[81];
	i4 errors;
	i4 warnings;
	i4 i;
	for (errors = warnings = i = 0; i < no_err_frames; i++)
	{
		register ERR_FRAME *cur_frame = err_frames + i;
		warnings += cur_frame->warnings;
		errors += (cur_frame->no_errs - cur_frame->warnings);
	}
	/* Format and output results */
	_VOID_ IIUGfmt(title, sizeof(title), ERget(S_VQ0021_ApplErrs), 
			3, appl->name, (PTR)&errors, (PTR)&warnings);
/* # line 1711 "vqcmperr.qsc" */	/* putform */
  {
    if (IIfsetio(_form) != 0) {
      IIputfldio(_title,(short *)0,1,32,0,title);
    } /* IIfsetio */
  }
/* # line 1712 "vqcmperr.qsc" */	/* host code */
}
/*
** has_errors.
**
**      Returns TRUE if a component has compilation errors or warnings.
**
*/
static bool
has_errors(comp)
APPL_COMP *comp;
{
        return (comp->compdate != NULL && *comp->compdate != EOS) ||
                                 (comp->flags & APC_OSLWARN) != 0;
}
/*
** Return a frame's superclass
*/
static i4
get_superclass(frame)
APPL_COMP	*frame;
{
	i4 superclass;
	switch (frame->class)
        {
            case OC_MUFRAME:
            case OC_APPFRAME:
            case OC_UPDFRAME:
            case OC_BRWFRAME:
		if (IIabVision)	
			superclass = FSC_VQ;
		else
			superclass = FSC_UF;
		break;
            case OC_OLDOSLFRAME:
            case OC_OSLFRAME:
		superclass = FSC_UF;
		break;
            case OC_OSLPROC:
		superclass = FSC_4P;
		break;
            case OC_HLPROC:
		superclass = FSC_3P;
		break;
	    default:
		superclass = FSC_OTHER;
		break;
	}
	return (superclass);
}
/*
** get_listfile_time
**
** Get the last modification time for a frame's listing file.
**
**      Returns         OK if we got the time
**
**      Side effect: makes LOCATION lstloc point to the current listing file.
**
** History:
**	19-aug-91 (blaise)
**	    Changed local variable "info" to "locinfo" to avoid conflict with
**	    MSDOS reserved word.
**	20-aug-91 (leighb) Bug Fix:
**		back out previous change because it is not needed because
**		there's no conflict.
**		'info' MUST be a static because it is used by the
**		caller of get_listfile_time in '*time'.
*/
static LOINFORMATION info;		 
static STATUS
get_listfile_time(comp, time)
APPL_COMP *comp;
SYSTIME **time;         /* Output */
{
        i4      what = LO_I_LAST;
        /* Make location for listing file */
        _VOID_ IIABmefMakeErrFile(comp, &lstloc, lstfilbuf);
        /* Get time, if we can */
        if (LOinfo(&lstloc, &what, &info) == OK && what == LO_I_LAST)
        {
                *time = &info.li_last;
                return OK;
        }
        return FAIL;    /* No date; presumably no listing file */
}
/*
**	See if we found the listing file (or lack of same) we expected.
**
**	Returns		TRUE if we found what we expected
**			FALSE otherwise
*/
static bool
check_listfile_time(frame)
ERR_FRAME *frame;
  {
	SYSTIME *list_time;
	bool	is_list;
    char msgbuf[320];
	is_list = get_listfile_time(frame->frame, &list_time) == OK;
	/* See if anything's changed */
	if ( (is_list && !frame->listexists) ||
	     (!is_list && frame->listexists) ||
	     (is_list && frame->listexists && 
			 TMcmp(list_time, &frame->listfile_time) != 0)
	   )
	{
		/* It's changed */
		IIUGfmt(msgbuf, sizeof(msgbuf), 
			ERget(E_VQ00E2_HasBeenRecompiled), 2, 
			frame->frame->name, frame_or_proc(frame));
/* # line 1837 "vqcmperr.qsc" */	/* message */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      IImessage(msgbuf);
    }
/* # line 1838 "vqcmperr.qsc" */	/* host code */
		frame->listexists = is_list;
		STRUCT_ASSIGN_MACRO(*list_time, frame->listfile_time);
		return FALSE;
	}
	else
	{
		/* No change */
		return TRUE;
	}
  }
/*
**	Annonce that no errors remain 
*/
static VOID
no_errors_left(appl)
APPL	*appl;
  {
    char msgbuf[80];
	IIUGfmt(msgbuf, sizeof(msgbuf), ERget(E_VQ0081_NoApplErrors), 
		1, appl->name);
/* # line 1860 "vqcmperr.qsc" */	/* message */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      IImessage(msgbuf);
    }
/* # line 1861 "vqcmperr.qsc" */	/* clear */
    {
      IIfldclear(_errtf);
      IIfldclear(_texttf);
    }
  }
/* # line 1864 "vqcmperr.qsc" */	/* host code */
/*
** Return the string for "Frame" or "Procedure".
*/
static char *
frame_or_proc(frame)
ERR_FRAME *frame;
{
	ER_MSGID msg;
	switch (frame->sc)
	{
	    case FSC_VQ:
	    case FSC_UF:
		msg = F_VQ0030_Frame;
		break;
	    default:
		msg = F_VQ0031_Procedure;
	}
	return ERget(msg);
}
/*
** Decode a location (bugs 34458, 34869).
** The format of a location is:
**	major_section\minor_section
** where minor_section may be null. Minor_section will have been encoded
** in IIFGbsBeginSection and IIFGesEndSection.
**
** The location will be decoded as follows:
**	First \	don't change (this is the one separating the major & minor
**		sections)
**	\\	translate to \
**	\_	translate to space
**	\/	translate to /
*/
static VOID
decode_loc(location, decoded_loc)
char	*location;		/* location name to be decoded */
	char	*decoded_loc;	/* decoded location name */
{
	char	*backslash = ERx("\\");
	char	*underscore = ERx("_");
	char	*space = ERx(" ");
	char	*slash = ERx("/");
	bool	firstbackslash = TRUE;
	while (*location != EOS)
	{
		if (CMcmpcase(location, backslash) == 0)    /* this is a "\" */
		{
			if(firstbackslash)
			/*
			** if this is the first "\" in the string, don't do
			** anything with it
			*/
			{
				CMcpyinc(location, decoded_loc);
				firstbackslash = FALSE;
			}
			else
			/* what's the next char? */
			{
				CMnext(location);
				if ((CMcmpcase(location, backslash) == 0) ||
					(CMcmpcase(location, slash) == 0))
				{
					/* translate "\\" to "\", "\/" to "/" */
					CMcpyinc(location, decoded_loc);
				}
				else if (CMcmpcase(location, underscore) == 0)
				{
					/* translate "\_" to " " */
					CMcpychar(space, decoded_loc);
					CMnext(location);
					CMnext(decoded_loc);
				}
			}
		}
		else	/* not a backslash */
		{
			/* copy character from location to decoded_loc */
			CMcpyinc(location, decoded_loc);
		}
	}
	*decoded_loc = EOS;	/* null terminate decoded string */
}
