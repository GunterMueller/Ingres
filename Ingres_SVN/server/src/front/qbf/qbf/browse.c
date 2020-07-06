# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
# include "mqbf.h"
#include	<ug.h>
#include	"erqf.h"
/**
** Name:	browse.qc -	Query Browse Module.
**
** Description:
**	This module constitutes the data browsing portion of MQBF.  It is
**	called after all the query strings, param args, etc. have been set up.
**
**	Routines:
**		IIqfBrowse()
**
**	Returns:
**		1 = Done browsing, have calling routine show form in QUERY mode.
**		0 = Error occurred in BROWSE mode.
**
** History:
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
**	Revision 6.1
**		88/09  wong  Modified for international support of menuitems.
**
**	Revision 6.0
**		07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header file: mqbf.qsh
**		11-dec-87
**			Removed the addition of 'newrow' to the list of hidden
**			hidden columns.  This is taken care of in MQQDESC.QSC.
**		15-dec-1987 (peter)
**			Integrate bug fix 12221 in 6.0.
**		24-feb-88 (marian)
**			Make bug 9826 work correctly.  Took out ifdef FT3270.
**			The forms system will print out the message, but the
**			activate on the scroll will repeatedly get called
**			and cause the message to be printed several times, so
**			handle this the same way FT3270 does.
**		22-apr-88 (kenl)
**			Removed GLOBALREF to allow_updates.  This is now
**			in mqglobs.h
**
**	Revision 5.0
**		16-jun-86 (mgw)		bug # 9582
**			Catch bad QG_alloc in QG_qbf properly and return error
**			message.
**		11-sep-86 (marian)	bug # 9878
**			Don't print "out of data below" message for non FT3270
**			machines.  The forms system takes care of this.	 Changed
**			the ifndef FT3270 to ifdef FT3270 and took out the line
**			to print the message for non FT3270 machines.
**		23-jan87 (marian)
**			Call qg_generate with QI_USTART if not onetbl.
**		21-apr-87 (marian) 	bug # 12221
**			Changed IIqfBrowse() so it now checks the return
**			from qg_generate() to determine if the temporary file
**			was created successfully.
**
**	Revision 3.0
**		 06/4/85 (drh)	Initialize mqrowstat in mqnxtmaster and
**				mqgetscroll
**				- can't rely on mqretinit being called first.
**
**	Revision 2.0  84/01/16  nadene
**	Written: 1/16/84 (nml)
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
#define	_HelpExpl		F_FE0100_ExplFrameHelp
#define	_EndExpl		F_FE0102_ExplEnd
  extern char mq_frame[];
  extern char mq_tbl[];
  extern char mq_tids[];
i4
IIqfBrowse ( mq_qdef )
MQQDEF	*mq_qdef;
  {
    char *nextitem;
    char *scrollitem;
    char *exititem;
	register QDESC	*mqqry;
	i4		output;
	i4		rtncode;
	i4		numrows;
	STATUS		retcode;
	bool		outofdatamsg = FALSE;
	mqqry = mq_qdef->mqqdesc;
	rtncode = 1;
# ifdef DEBUG
	if (mq_debug)
	{
		SIfprintf(mqoutf,ERx("MQBROWSE: qdef_type = %d\n"), qdef_type);
		SIfprintf(mqoutf,ERx("MQBROWSE: tblfield = %d\n"), tblfield);
	}
# endif
	if ( (retcode = IIQG_qbf(mqqry, mqmrec, mq_qdef->mqmrecl)) != OK )
	{
		FDerror(ERRALLOC, 0, ERx(""));
		return 0;
	}
	/*	You want to call QG_generate with QI_USTART if more than
	**	one table is involved.
	*/
	if ( (retcode = IIQG_generate(mqqry, onetbl ? QI_START : QI_USTART,
					 &output)) != OK )
	{
		return 1;	/*OK*/
	}
	QG_mqqry = mqqry;	/* save for interrupt cleanup */
	if (output == QO_QDONE)
	{
		/* no rows retrieved */
/* # line 146 "browse.qsc" */	/* message */
    {
      IImessage(ERget(S_QF001C_NoRowsForQuery));
    }
/* # line 147 "browse.qsc" */	/* sleep */
    {
      IIsleep(3);
    }
/* # line 148 "browse.qsc" */	/* host code */
		return 1;	/*OK*/
	}
	/* got data - redisplay the form in READ mode */
	exititem = ( mq_func == mqBROWSE ) ? ERget(FE_Quit) : ERget(FE_End);
	nextitem = scrollitem = ERx("");
	if (tblfield)
	{
		if (!IIQFnkaNoKeyForAppends)
		{
/* # line 159 "browse.qsc" */	/* inittable */
    {
      if (IItbinit(mq_frame,mq_tbl,(char *)"r") != 0) {
        IIthidecol(mq_tids,(char *)0);
        IItfill();
      } /* IItbinit */
    }
/* # line 160 "browse.qsc" */	/* host code */
		}
		else
		{
/* # line 163 "browse.qsc" */	/* inittable */
    {
      if (IItbinit(mq_frame,mq_tbl,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 164 "browse.qsc" */	/* host code */
		}
		if ( qdef_type == 0 )
			nextitem = ERget(F_QF0001_NextMaster);
	}
	else
	{
		if (qdef_type == 0)
		{
			nextitem = ERget(F_QF0001_NextMaster);
			scrollitem = ERget(F_QF0002_NxtDetail);
		}
		else
		{
			nextitem = ERget(FE_Next);
		}
	}
	numrows = 0;
/* # line 184 "browse.qsc" */	/* display */
    {
      if (IIdispfrm(mq_frame,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 187 "browse.qsc" */	/* host code */
		if ( (retcode = mqretinit(mq_qdef, &output, &numrows)) != OK )
		{
			IIUGerr(E_QF001D_FormInitError,0,0);
			_VOID_ IIQG_generate(mqqry, QI_BREAK, &output);
			QG_mqqry = NULL;
			rtncode = 0;
/* # line 193 "browse.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 194 "browse.qsc" */	/* host code */
		}
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 199 "browse.qsc" */	/* host code */
		if ( nextitem[0] == EOS )
		{
			FTbell();
/* # line 202 "browse.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 203 "browse.qsc" */	/* host code */
		}
		if (qdef_type == 0)
		{
/* # line 206 "browse.qsc" */	/* message */
            {
              IImessage(ERget(S_QF001E_Retrieving));
            }
/* # line 207 "browse.qsc" */	/* host code */
			if ((mqgetmaster(mq_qdef, &output, &numrows)) != OK)
/* # line 208 "browse.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 209 "browse.qsc" */	/* host code */
		}
		else
		{
			if ((mqgetrecord(mq_qdef, &output)) != OK)
/* # line 213 "browse.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 214 "browse.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 2) {
          {
/* # line 219 "browse.qsc" */	/* message */
            {
              IImessage(ERget(S_QF001F_BackToQuery));
            }
/* # line 220 "browse.qsc" */	/* host code */
		if ( (retcode = IIQG_generate(mqqry, QI_BREAK, &output)) != OK )
		{
			rtncode = 0;
		}
		QG_mqqry = NULL;
/* # line 225 "browse.qsc" */	/* clear */
            {
              IIclrflds();
            }
/* # line 226 "browse.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 231 "browse.qsc" */	/* host code */
		if ((mqgetdtl(mq_qdef, &output)) != OK)
/* # line 232 "browse.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 238 "browse.qsc" */	/* host code */
		char	*help_file;
		if (qdef_type == 0)
		{
			if (tblfield)
			{
				help_file = (mq_func == mqBROWSE)
						? ERx("qretmdtx.hlp")
						: ERx("qretmdtn.hlp");
			}
			else
			{
				help_file = (mq_func == mqBROWSE)
						? ERx("qretmdox.hlp")
						: ERx("qretootx.hlp");
			}
		}
		else
		{
			if (tblfield)
			{
				help_file = (mq_func == mqBROWSE)
						? ERx("qretootx.hlp")
						: ERx("qretootn.hlp");
			}
			else
			{
				help_file = (mq_func == mqBROWSE)
						? ERx("qretooox.hlp")
						: ERx("qretooon.hlp");
			}
		}
		FEhelp( help_file, ERget(S_QF001B_QBF_Retrieve_Function) );
          }
        } else if (IIretval() == 5) {
          {
/* # line 275 "browse.qsc" */	/* host code */
		if ( mq_func != mqBROWSE )
			FTbell();	/* quit key not allowed */
		else
			goto endit;	/* OK */
          }
        } else if (IIretval() == 6) {
          {
/* # line 283 "browse.qsc" */	/* host code */
endit:
/* # line 284 "browse.qsc" */	/* message */
            {
              IImessage(ERget(S_QF0020_EndBrowse));
            }
/* # line 285 "browse.qsc" */	/* host code */
		_VOID_ IIQG_generate(mqqry, QI_BREAK, &output);
		QG_mqqry = NULL;
/* # line 287 "browse.qsc" */	/* clear */
            {
              IIclrflds();
            }
/* # line 288 "browse.qsc" */	/* host code */
		rtncode = 0;
/* # line 289 "browse.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 7) {
          {
            i4 botrow;
/* # line 296 "browse.qsc" */	/* getrow */
            {
              if (IItbsetio(2,mq_frame,mq_tbl,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&botrow,(char *)"_RECORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 297 "browse.qsc" */	/* host code */
/*
**	The 'outofdatamsg' flag is used by CMS to prevent the "Out of Data
**	Below" message from being issued more than once when scrolling
**	more than 1 record past the end of the table field. This same
**	problem occurs on DEC systems as well if the user tries to issue
**	"13j" or something like that. Unfortunately on CMS the user is
**	much more likely to get into this state (because of the way
**	FT/3270's "table field scroll" works), and the multiple error
**	messages are more likely to appear. The flag is "reset" if
**	the user scrolls back up and then down for a second time.
*/
		if ( botrow == numrows && ( output == QO_QDONE ||
						 output == QO_DDONE ) )
		{
			if (!outofdatamsg)
			{
				outofdatamsg = TRUE;
				winerr(ERget(E_QF0021_OutOfData));
			}
/* # line 316 "browse.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 317 "browse.qsc" */	/* host code */
		}
		else if ( botrow != numrows )
		{
			outofdatamsg = FALSE;
		}
		if (botrow >= numrows)
		{
/* # line 325 "browse.qsc" */	/* message */
            {
              IImessage(ERget(S_QF0022_RetrieveMore));
            }
/* # line 326 "browse.qsc" */	/* host code */
			if ((mqgetscroll(mq_qdef, &output, &numrows)) != OK)
/* # line 327 "browse.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 328 "browse.qsc" */	/* host code */
		}
/* # line 330 "browse.qsc" */	/* scroll */
            {
              if (IItbsetio(1,mq_frame,mq_tbl,-3) != 0) {
                IItbsmode((char *)"up");
                if (IItscroll(0,-3) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
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
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(nextitem,(char *)0,2,2,1) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Query),(char *)0,0,2,2) == 0) goto IIfdE1;
      if (IInmuact(scrollitem,(char *)0,2,2,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,0,2,5) == 0) goto IIfdE1;
      if (IInmuact(exititem,ERget(_EndExpl),0,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,2,6) == 0) goto IIfdE1;
      if (IIactscrl(mq_tbl,0,7) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 334 "browse.qsc" */	/* host code */
	return rtncode;
  }
