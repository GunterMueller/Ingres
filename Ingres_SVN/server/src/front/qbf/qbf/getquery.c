# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
# include "mqbf.h"
#include	"mqeglobs.h"
#include	"erqf.h"
/**
** Name:	getquery.qc -	Get Query Qualification From Form.
**
** Description:
**	This module is used to perform the QUERY mode of QBF for both the
**	BROWSE and UPDATE modes.  It checks the form to see what fields have
**	been filled in to be used as qualifications for the query and then
**	finishes creating the necessary qualification string.  It is also used
**	to set up the desired range variables for the current qdef.
**
**	Routines:
**		IIqfGetQuery()
**
**	Returns:
**		-1 = Return to QDEF phase - this return code is sent for
**			an error in setting up the range variables.
**		 0 = Return to top QueryExecution form.
**		 1 = Go to run the query.
**		 2 = Go into Setsort mode.
**
** History:
**	Written: 1/16/84 (nml)
**	12-aug-1987 (danielt)
**		changed FEmsg() calls to IIUGerr() calls
**	07-dec-87 (kenl)
**		Turned module into a .QSC file in order to incorporate
**		the new header file: mqbf.qsh.  Changed call to IIrvalid
**		so that the address of routine IIQFblfBldFrom is passed
**		in addition to the address of the MQQDEF (qdef).
**		IIQFblfBldFrom builds the FROM clause required by QG
**		(for the Master portion of a 1-1 or Master/Detail JoinDef)
**		to perform an SQL SELECT.  Added code to handle the case
**		where IIQFblfBldFrom may not build a FROM clause if no
**		fields were filled in on the form.
**	23-aug-88 (kenl)
**		Added DML level to call on IIrvalid.
**	08-mar-90 (kenl)
**		Added ListChoices menuitem to allow user to get help
**		on fields.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
#define	_HelpExpl		F_FE0100_ExplFrameHelp
#define	_EndExpl		F_FE0102_ExplEnd
extern i4	MQdblang;
  extern char mq_frame[];
  extern char mq_tbl[];
STATUS	IIQFblfBldFrom();
IIqfGetQuery ( qdef, prev_qry, last_qry )
MQQDEF	*qdef;
bool	prev_qry;
bool	last_qry;
  {
	register QDESC	*mqqry;
	char		*qfrom;
	i4		rtncode;
	bool		did_last_qry;
    char *lmenu;
    char *enditem;
	static READONLY char	_procname[]	= ERx("IIqfGetQuery");
#ifndef	NOLASTQRY
	if (IIrngon(mq_frame) != OK)
	{
		IIUGerr(E_QF0048_RngOnErr, 0, 0);
		return -1;
	}
	lmenu = ERget(F_QF0008_LastQuery);
#else
	lmenu = ERx("");
#endif	/* NOLASTQRY */
	mqqry = qdef->mqqdesc;
	qfrom = ((QRY_SPEC *)mqqry->qg_from)->qs_var;
	rtncode = 0;
	enditem = (mq_func == mqBROWSE || mq_func == mqUPDATE)
			? ERget(FE_Quit) : ERget(FE_End);
	if (tblfield)
	{
/* # line 107 "getquery.qsc" */	/* inittable */
    {
      if (IItbinit(mq_frame,mq_tbl,(char *)"q") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 108 "getquery.qsc" */	/* host code */
	}
/* # line 109 "getquery.qsc" */	/* display */
    {
      if (IIdispfrm(mq_frame,(char *)"q") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 112 "getquery.qsc" */	/* clear */
        {
          IIclrflds();
        }
/* # line 113 "getquery.qsc" */	/* host code */
# ifndef	NOLASTQRY
		if (last_qry)
		{
			if (IIrnglast(mq_frame) != OK)
			{
				IIUGerr(E_QF0049_LastQryErr, 0, 0);
				rtncode = -1;
/* # line 120 "getquery.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 121 "getquery.qsc" */	/* host code */
			}
		}
		else
		{
			if (IIrnginit(mq_frame) != OK)
			{
				IIUGerr(E_QF004A_RngInitErr, 0, 1, _procname);
				rtncode = -1;
/* # line 129 "getquery.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 130 "getquery.qsc" */	/* host code */
			}
		}
# endif		/* NOLASTQRY */
		did_last_qry = last_qry;
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 139 "getquery.qsc" */	/* host code */
		did_last_qry = FALSE;
#ifndef NOLASTQRY
		/* initialize FROM clause to empty */
		*qfrom = EOS;
		if ( !IIrvalid(mq_frame, IIQFblfBldFrom, qdef, MQdblang) )
		{
/* # line 146 "getquery.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 147 "getquery.qsc" */	/* host code */
		}
		/*
		**  If we return from IIrvalid with *qfrom still NULL then
		**  NO fields were filled in to query about.  Therefore,
		**  IIQFblfBldFrom was not called and no FROM clause was
		**  built.  Since no fields were filled in, the SELECT to
		**  be done by QG for the Master WILL NOT involve any
		**  Detail tables.  The FROM clause for the Master will be
		**  built to include ONLY Master tables.
		*/
		if (*qfrom == EOS)
		{
			IIqfAddFrom( qdef, mqIN_MASTER, qfrom );
		}
		/* remove trailing comma */
		*((qfrom + STlength(qfrom)) - 1) = EOS;
#endif	/* NOLASTQRY */
		if ( mqqry->qg_sort != NULL || ( mqqry->qg_child != NULL &&
		     			mqqry->qg_child->qg_sort != NULL ) )
		{
/* # line 170 "getquery.qsc" */	/* message */
            {
              IImessage(ERget(S_QF0044_RetrievingSorted));
            }
/* # line 172 "getquery.qsc" */	/* host code */
		}
		else
		{
/* # line 175 "getquery.qsc" */	/* message */
            {
              IImessage(ERget(S_QF0045_RetrievingData));
            }
/* # line 176 "getquery.qsc" */	/* host code */
		}
		rtncode = 1;
		IIqbfclr(FALSE);
/* # line 179 "getquery.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 184 "getquery.qsc" */	/* host code */
		did_last_qry = FALSE;
/* # line 185 "getquery.qsc" */	/* clear */
            {
              IIclrflds();
            }
/* # line 186 "getquery.qsc" */	/* host code */
#ifndef NOLASTQRY
		if (IIrnginit(mq_frame) != OK)
		{
			IIUGerr(E_QF004A_RngInitErr, 0, 1, _procname);
			rtncode = -1;
/* # line 191 "getquery.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 192 "getquery.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 3) {
          {
/* # line 197 "getquery.qsc" */	/* host code */
		if (!prev_qry)
		{
			IIUGerr(E_QF004B_NoLastQry, 0, 0);
/* # line 200 "getquery.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 201 "getquery.qsc" */	/* host code */
		}
/* # line 202 "getquery.qsc" */	/* clear */
            {
              IIclrflds();
            }
/* # line 203 "getquery.qsc" */	/* host code */
		if (IIrnglast(mq_frame) != OK)
		{
			IIUGerr(E_QF0049_LastQryErr, 0, 0);
			rtncode = -1;
/* # line 207 "getquery.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 208 "getquery.qsc" */	/* host code */
		}
		did_last_qry = TRUE;
#endif	/* NOLASTQRY */
          }
        } else if (IIretval() == 4) {
          {
            i4 changed;
/* # line 217 "getquery.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&changed,20,mq_frame,0);
              } /* IIiqset */
            }
/* # line 218 "getquery.qsc" */	/* host code */
		if (changed)
		{
#ifndef NOLASTQRY
			if ( !IIrvalid(mq_frame, NULL, (PTR)NULL, MQdblang) )
			{
/* # line 223 "getquery.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 224 "getquery.qsc" */	/* host code */
			}
#endif	/* NOLASTQRY */
			rtncode = 3;
		}
		else if (did_last_qry)
		{
			rtncode = 3;
		}
		else
		{
			rtncode = 2;
		}
		IIqbfclr(FALSE);
/* # line 237 "getquery.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 244 "getquery.qsc" */	/* host code */
		IIUFfieldHelp();
          }
        } else if (IIretval() == 6) {
          {
/* # line 249 "getquery.qsc" */	/* host code */
		char	*help_file;
		help_file = ( mq_func == mqBROWSE || mq_func == mqUPDATE )
				? ERx("qqueryx.hlp") : ERx("qqueryn.hlp");
		FEhelp( help_file, ERget(S_QF0046_QBF_Query_State) );
          }
        } else if (IIretval() == 7) {
          {
/* # line 258 "getquery.qsc" */	/* host code */
		if ( mq_func != mqBROWSE && mq_func != mqUPDATE )
			FTbell();	/* quit key not allowed */
		else
		{
			rtncode = 0;
/* # line 263 "getquery.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 264 "getquery.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 8) {
          {
/* # line 268 "getquery.qsc" */	/* host code */
		rtncode = 0;
/* # line 269 "getquery.qsc" */	/* breakdisplay */
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
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Go),(char *)0,2,2,1) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Blank),(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IInmuact(lmenu,(char *)0,2,2,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_QF0009_Order),(char *)0,2,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,5) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),(char *)0,2,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,0,2,7) == 0) goto IIfdE1;
      if (IInmuact(enditem,(char *)0,2,2,8) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,2,8) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 272 "getquery.qsc" */	/* host code */
	IIqbfclr(TRUE);
#ifndef	NOLASTQRY
	if ( IIunrng(mq_frame) != OK )
	{
		IIUGerr( E_QF0002_RangeError, 0, 1, _procname );
		return -1;
	}
#endif	/* NOLASTQRY */
	return rtncode;
  }
