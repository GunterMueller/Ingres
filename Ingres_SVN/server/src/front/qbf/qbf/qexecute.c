# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<ex.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
# include	<ug.h>
# include "mqbf.h"
#include	"mqeglobs.h"
#include	"erqf.h"
/**
** Name:	qexecute.qc -	Query Execution Module.
**
** Description:
**	This module performs the query execution for MQBF.
**
**	Routines:
**	     mqqexecute()
**
**	Written: 1/14/84 (nml)
**
** History:
**	Revision 6.5
**		30-jul-1993 (rdrane)
**			Add check of qdef->mqappendok to preclude
**			APPEND operations on tables containing
**			unsupported datatype columns that are NOT NULL and
**			have no defined DEFAULT.  Part of fix for b51411.
**			Change all instances of IIUGerr()m to use #define for
**			error severity.
**	Revision 6.4
**		10/11/90 (emerson)
**			Replaced global variable allow_appends and allow_updates
**			by IIQFnkaNoKeyForAppends and IIQFnkuNoKeyForUpdates
**			(with opposite sense) because the names are misleading:
**			we now have other reasons for disallowing appends
**			(certain joins on logical keys), and we might have
**			other reasons for disallowing appends or updates
**			in the future.  Keeping separate flags allows us
**			to give specific error messages.  (bug 8593).
**		10/11/90 (emerson)
**			Disallow appends if there are any MM joins
**			from a system-maintained logical key
**			to a user-maintained logical key.
**			We can't support these until the back-end
**			supports feedback of a system-generated logical key.
**			(bug 8593).
**
**	Revision 6.1  88/08/19  kenl
**		19-aug-88 (kenl)
**			Added GLOBALREF of save_prompt and the logic to deal
**			with the case where the user tries to QUIT without
**			having saved a JoinDef.
**		11-sep-88  wong  Modified for international support of
**			menuitems.  (Optimized, too.)
**		19-oct-88 (kenl)
**			Removed cnt_tlist.  The DBMS is responsible for
**			generating an error if the target list is too long.
**		23-nov-88 (kenl)
**			Allow appends when there are no keys.  Fix originated
**			from the IBM group (ham).
**		05-dec-89 (kenl)
**			We no longer provide the 'Start' and 'NewQueryTarget'
**			menuitems when editing a JoinDef (-J flag).
**
**	Revision 6.0  87/08/13  danielt
**		13-aug-87 (danielt)
**			Changed FEmsg() calls to IIUGerr() for error messages
**		07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header file: mqbf.qsh. Added code to handle new
**			error message for the dis-allowing of updates/appends
**			when no unique keys/tids are found for a table. Changed
**			QUEL ABORT to an SQL ROLLBACK.
**		22-apr-88 (kenl)
**			Removed globalref of allow_updates.  This now appears
**			in mqglobs.h.
**
**	Revision 5.0  86/01/17  marian
**		17-jan-86 (marian)	bug # 4805
**			Added a cnt_tlist() to count the # of fields in
**			the target list.
**		17-jan-86 (marian)	bug # 6606
**			Check to see if you're inside a MST before starting
**			up another one.
**
**	Revision 3.0  85/11/18  marian
**		18-nov-85 (marian)	bug # 6552
**			Took the call to EXdeclare out of an if statement
**			to force it to always do the switch.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# define    ROLLBACK_ENTIRE_XACT    TRUE
# define    ALL_BACKENDS	    FALSE	
extern i4	save_prompt;
extern bool	MQingres;
extern bool	Mq_intransaction;
extern bool	Mq_inunloadtable;
GLOBALREF	bool	IIQF_NoDetail;
static EX	qex_handler();
#define	_HelpExpl		F_FE0100_ExplFrameHelp
#define	_EndExpl		F_FE0102_ExplEnd
mqqexecute(qdef)
MQQDEF	*qdef;
  {
    char *emenu;
    char *startmenu;
    char *nqtmenu;
	i4		rtncode;
	i4		qcode;
	bool		prev_qry;
	bool		last_qry;
	EX_CONTEXT	context;
	rtncode = 0;
	prev_qry = FALSE;
	last_qry = FALSE;
	startmenu = emenu = nqtmenu = ERx("");
	if (mq_name && !mq_editjoin)
	{
		if (mq_func == mqNOFUNC)
		{
			startmenu = ERget(F_QF0021_Start);
			nqtmenu = ERget(F_QF0012_NewQueryTarget);
		}
	}
	else
	{
		emenu = ERget(FE_End);
	}
#ifndef NOLASTQRY
	if (IIrnginit(mq_frame) != OK)
	{
		IIUGerr(E_QF004A_RngInitErr, UG_ERR_ERROR, 1,
			ERx("mqqexecute"));
	}
#endif	/* NOLASTQRY */
/* # line 158 "qexecute.qsc" */	/* display */
    {
      if (IIdispfrm((char *)"mqexecute",(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 161 "qexecute.qsc" */	/* host code */
		/*
		**  Bug # 6552 -- Eliminate the if statement and always do
		**		the switch
		*/
		_VOID_ EXdeclare(qex_handler, &context);
		/*  Bug # 6606
		**	Check to see if you're already in an MST.
		**	(marian 1/16/86)
		*/
		if ( Mq_intransaction )
		{
			IIUIasp_abortSP( (char *)NULL, ROLLBACK_ENTIRE_XACT,
				ALL_BACKENDS
			);
			Mq_intransaction = FALSE;
		}
		switch(mq_func)
		{
			case mqNOFUNC:
			case mqALL:
				break;
			case mqAPPEND:
				goto appstart;
				break;
			case mqBROWSE:
				goto brwstart;
				break;
			case mqUPDATE:
				goto updstart;
				break;
			default:
				break;
		}
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 205 "qexecute.qsc" */	/* host code */
appstart:
		if (any_cats)
		{
			IIUGerr(E_QF0085_NotOnSysCats, 
				UG_ERR_ERROR, 1, ERget(F_QF0024_APPEND));
		}
		else if ( any_views )
		{
			IIUGerr(E_QF0086_NotOnViews, 
				UG_ERR_ERROR, 1, ERget(F_QF0024_APPEND));
		}
		else if ( IIQFnkaNoKeyForAppends )
		{
			IIUGerr(E_QF0088_NotWithoutKey, 
				UG_ERR_ERROR, 1, ERget(F_QF0024_APPEND));
		}
		else if ( IIQFsmmSystemUserLkeyMM )
		{
			IIUGerr(E_QF0171_MM_system_user_append, UG_ERR_ERROR,
				0);
		}
		else if ( (qdef_type == 0 && !tblfield) || IIQF_NoDetail )
		{
			/*
			** master-detail with no table field
			** APPEND mode not allowed for this case
			*/
			IIUGerr(E_QF0087_NotForMD, 
				UG_ERR_ERROR, 1, ERget(F_QF0024_APPEND));
		}
		else if ( !qdef->mqappendok )
		{
			IIUGerr(E_QF0177_UnDataType,
				UG_ERR_ERROR, 1, ERget(F_QF0024_APPEND));
		}
		else
		{
			IIqfAppend(qdef);
		}
		if (mq_func == mqAPPEND)
		{
			rtncode = 1;
/* # line 248 "qexecute.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 249 "qexecute.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 2) {
          {
/* # line 254 "qexecute.qsc" */	/* host code */
brwstart:
		/* Bug 4805
		**	Count the # of fields in the target list
		*/
		while ( (qcode = IIqfGetQuery(qdef, prev_qry, last_qry)) != 0 )
		{
			last_qry = FALSE;
			if (qcode == -1)
			{
				if (mq_func != mqNOFUNC)
					rtncode = 1;
/* # line 265 "qexecute.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 266 "qexecute.qsc" */	/* host code */
			}
			else if (qcode >= 2)
			{
				IIqfSetSort(qdef);
				if (qcode == 3)
				{
					last_qry = TRUE;
					prev_qry = TRUE;
				}
				continue;
			}
			prev_qry = TRUE;
			if ( IIqfBrowse(qdef) == 0 )
				break;
		} /* end while */
		if ( mq_func == mqBROWSE )
		{
			rtncode = 1;
/* # line 284 "qexecute.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 285 "qexecute.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 3) {
          {
/* # line 291 "qexecute.qsc" */	/* host code */
updstart:
		if (any_cats)
		{
			IIUGerr(E_QF0085_NotOnSysCats, 
				UG_ERR_ERROR, 1, ERget(F_QF0025_UPDATE));
		}
		else if ( any_views )
		{
			IIUGerr(E_QF0086_NotOnViews, 
				UG_ERR_ERROR, 1, ERget(F_QF0025_UPDATE));
		}
		else if ( IIQFnkuNoKeyForUpdates )
		{
			IIUGerr(E_QF0088_NotWithoutKey, 
				UG_ERR_ERROR, 1, ERget(F_QF0025_UPDATE));
		}
		/* Bug 4805
		**	Count the # of fields in the target list
		*/
		else
		{
			while ( (qcode = IIqfGetQuery(qdef, prev_qry, last_qry))
					!= 0 )
			{
				last_qry = FALSE;
				if (qcode == -1)
				{
					if (mq_func != mqNOFUNC)
						rtncode = 1;
/* # line 320 "qexecute.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 321 "qexecute.qsc" */	/* host code */
				}
				else if (qcode >= 2)
				{
					IIqfSetSort(qdef);
					if (qcode == 3)
					{
						last_qry = TRUE;
						prev_qry = TRUE;
					}
					continue;
				}
				prev_qry = TRUE;
				if ( IIqfUpdate(qdef) == FAIL )
					break;
			} /* end while */
		}
		if ( mq_func == mqUPDATE )
		{
			rtncode = 1;
/* # line 342 "qexecute.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 343 "qexecute.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 4) {
          {
/* # line 348 "qexecute.qsc" */	/* host code */
		if ( getNewName(qdef) == OK )
		{
/* # line 350 "qexecute.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 351 "qexecute.qsc" */	/* host code */
			rtncode = 2;
/* # line 352 "qexecute.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF1;
            }
/* # line 353 "qexecute.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 5) {
          {
/* # line 358 "qexecute.qsc" */	/* host code */
#ifndef NOLASTQRY
		if (IIrngend(mq_frame) != OK)
		{
			IIUGerr(E_QF003D_mqqexecute__couldn_t_,
				UG_ERR_ERROR,0);
		}
#endif	/* NOLASTQRY */
		mq_name = FALSE;
/* # line 366 "qexecute.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 367 "qexecute.qsc" */	/* host code */
		rtncode = 1;
/* # line 368 "qexecute.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF1;
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 374 "qexecute.qsc" */	/* host code */
		char	*help_file;
		if (mq_name)
			help_file = (mq_func == mqNOFUNC) ? ERx("qqryints.hlp")
							: ERx("qqryint.hlp");
		else
			help_file = ERx("qqbfint.hlp");
		FEhelp( help_file, ERget(S_QF008B_QBF_Execution_Phase) );
          }
        } else if (IIretval() == 7) {
          {
/* # line 386 "qexecute.qsc" */	/* host code */
		if ( *emenu != EOS )
		{
#ifndef NOLASTQRY
			if (IIrngend(mq_frame) != OK)
			{
				IIUGerr(E_QF003D_mqqexecute__couldn_t_,
					UG_ERR_ERROR,0);
			}
#endif	/* NOLASTQRY */
/* # line 395 "qexecute.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 396 "qexecute.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF1;
            }
/* # line 397 "qexecute.qsc" */	/* host code */
		}
		else
		{
			goto quitit;
		}
          }
        } else if (IIretval() == 8) {
          {
/* # line 406 "qexecute.qsc" */	/* host code */
	quitit:
		if (save_prompt != 0)
		{
			if ( !IIUFver(ERget(S_QF00A2_NotSavedJoinDefQ),
					1, ERget(FE_Quit) ) )
			{
/* # line 412 "qexecute.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 413 "qexecute.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF1;
            }
/* # line 414 "qexecute.qsc" */	/* host code */
			}
			save_prompt = 0;
		}
#ifndef NOLASTQRY
		if (IIrngend(mq_frame) != OK)
		{
			IIUGerr(E_QF003D_mqqexecute__couldn_t_,
				UG_ERR_ERROR,0);
		}
#endif	/* NOLASTQRY */
/* # line 425 "qexecute.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 426 "qexecute.qsc" */	/* host code */
		if (!mq_equel)
		{
			mqdbexit();
		}
		mqexit(OK);
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
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Append),(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Retrieve),(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Update),(char *)0,2,2,3) == 0) goto IIfdE1;
      if (IInmuact(nqtmenu,(char *)0,2,2,4) == 0) goto IIfdE1;
      if (IInmuact(startmenu,(char *)0,2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,6) == 0) goto IIfdE1;
      if (IInmuact(emenu,(char *)0,2,2,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,2,7) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Quit),(char *)0,0,2,8) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,0,2,8) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 434 "qexecute.qsc" */	/* host code */
	EXdelete();
	return (Qcode = rtncode);
  }
static EX
qex_handler(ex)
EX_ARGS *ex;
{
/* # line 442 "qexecute.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 443 "qexecute.qsc" */	/* host code */
	if (ex->exarg_num == EXINTR)
	{
		if (MQingres)
			IIresync();
		/* Clean up UNLOADTABLE */
		if ( Mq_inunloadtable )
			IItunend();
		Mq_inunloadtable = FALSE;
		mqcleanup();
		IIpopfrm(ERx("mqexecute"));
		return EXDECLARE;
	}
	else
		return EXRESIGNAL;
}
