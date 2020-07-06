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
#include	<ui.h>
# include "mqbf.h"
#include	"mqeglobs.h"
#include	"erqf.h"
/**
** Name:	append.qc -	Query Append Module.
**
** Description:
**	This module constitutes the append function of MQBF.  Routines:
**
**	IIqfAppend()	query (QBF) append frame.
**
**	Written: 2/15/84 (nml)
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
**		10/13/90 (emerson)
**			Make table field invisible if we can't support appends
**			of detail records entered into it.  This can happen
**			if there are certain kinds of joins on logical keys.
**			(bug 8593).
**		10/26/90 (emerson)
**			Changes to correctly implement appends of details
**			in a MD join on 2 user-maintained logical keys;
**			for logical key support (bug 8593).
**		10-jun-91 (davel)
**			Replaced ## MESSAGE with FEmsg() calls for messages 
**			that take printf-style arguments. (bug 37371)
**              11-jan-1996 (toumi01; from 1.1 axp_osf port)
**                      Added kchin's changes (from 6.4) for axp_osf
**                      10/15/93 (kchin)
**                      Cast argument to PTR when calling FEmsg(), this
**                      is due to the change of datatype of variable parameters
**                      to PTR in FEmsg() routine.  The change here is made in
**                      IIqfAppend() and noaddmsg().
**
**	Revision 6.1  88/09  wong
**	Modified for international support of menuitems.  (Optimized, too.)
**	Also, merged duplicate messages.
**	08-mar-90 (kenl)
**		Added ListChoices menuitem to enable field help.
**
**	Revision 6.0  88/06/23  kenl
**		10-aug-1987 (danielt)
**			message extraction changes.  Substituttion of message
**			id's for strings, and IIUGerr() for FEmsg().
**		07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header file: mqbf.qsh.  Replaced QUEL APPEND
**			will a call to IIQFsda_sqlInsertDBDV which does an SQL
**			INSERT using DB Data Values.  Replace QUEL transaction
**			statements with INGRES/Gateway SQL statements.
**		11-dec-87 (kenl)
**			Removed the addition of 'newrow' as a hidden field.
**			This is now done in MMQDESC.QSC.
**		23-jun-88 (kenl)
**			Changed FEinqerr() to INQUIRE_FRS when referencing
**			the FRS.  In the old days (rev 5.0) INQUIRE_EQUEL
**			would return both DBMS and FRS.  Re-fix of #9500.
**
**	Revision 5.0  86/06/17  marian
**		17-jan-86 (marian)	bug # 6606
**			Set the global Mq_intransaction
**		17-jan-86 (marian)	bug # 6611
**			Determine if you're already in an unloadtable (caused
**			by interrupt), if so ENDLOOP.
**		17-jun-86 (marian)
**			Catch errors in unloadtable loop.  This solves the
**			problem with tablefields and i2 overflow problems.
**			Changes were made in mqappend().
**
**	Revision 2.0  84/02/15  nadene
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
#define	_HelpExpl		F_FE0100_ExplFrameHelp
#define	_EndExpl		F_FE0102_ExplEnd
VOID	IIQFsda_sqlInsertDBDV();
extern bool	Mq_intransaction;	/* determines if in a MST */
extern bool	Mq_inunloadtable;	/* determines if in a UNLOADTABLE */
FUNC_EXTERN ADF_CB  *FEadfcb();
/*
**  The two globals below record the logical key attributes
**  that are masters (in the case where the joindef is master-detail).
**  The array IIQFmla_MLkeyInMDArray contains pointers to all such attributes;
**  IIQFmlc_MLkeyInMDCount indicates how many such attributes have been found.
*/
GLOBALREF ATTRIBINFO	*IIQFmla_MLkeyInMDArray[MQ_MAXATTS];
GLOBALREF i4	IIQFmlc_MLkeyInMDCount;
  extern char mq_frame[];
  extern char mq_tbl[];
  extern char mq_tids[];
static VOID noaddmsg(/* i4  */);
#define	_ALL	2
#define	_MASTER	0
#define _DETAIL	1
static STATUS addrow(/*MQQDEF *, i4, bool */);
STATUS
IIqfAppend ( qdef )
MQQDEF	*qdef;
  {
    char *insrtitem;
    char *exititem;
    i4 err;
	i4	totrows;
	i4	mrows;		/* number of master rows appended */
	i4	numflds;
	bool	detail_appends_supported = TRUE;
#ifndef	NOLASTQRY
	if (IIunrng(mq_frame) != OK)
	{
	    IIUGerr( E_QF0002_RangeError, 0, 1, ERx("IIqfAppend()") );
	    return FAIL;
	}
#endif		/* NOLASTQRY */
	totrows = 0;
	mrows = 0;
	numflds = mq_mflds(qdef);
	exititem = (mq_func == mqAPPEND) ? ERget(FE_Quit) : ERget(FE_End);
	if (tblfield)
	{
	    /*
	    **  For this release, certain conditions will cause us
	    **  to be unable to do appends of details.
	    **  In this case, we will make the table field update mode,
	    **  so that if the user tries to enter data in it, he will
	    **  get a beep.  This behavior is consistent with the behavior
	    **  on the update screen if the user tries to enter new data
	    **  into a blank table.  Ideally I should put out an error
	    **	message, but I don't want to unless the user actually tries
	    **	to enter data, and I don't know how to have the Forms System
	    **  hand me such events.  This problem should go away in the next
	    **  release (when the back-end folks support feedback on insertion
	    **  of system-maintained logical keys).
	    */
	    if (qdef_type == 0 /* MD */ &&
		(IIQFsmdSystemUserLkeyMD || IIQFsddSystemUserLkeyDD))
	    { 
		detail_appends_supported = FALSE;
/* # line 180 "append.qsc" */	/* inittable */
    {
      if (IItbinit(mq_frame,mq_tbl,(char *)"u") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 181 "append.qsc" */	/* host code */
		insrtitem = ERx("");
	    }
	    else
	    {
		/*
		**  mq_tids should ALWAYS have key columns and/or TIDs in it.
		**  Otherwise, this routine should NEVER have been called.
		**  (global variable IIQFnkaNoKeyForAppends would be set TRUE)
		*/
/* # line 190 "append.qsc" */	/* inittable */
    {
      if (IItbinit(mq_frame,mq_tbl,(char *)"f") != 0) {
        IIthidecol(mq_tids,(char *)0);
        IItfill();
      } /* IItbinit */
    }
/* # line 191 "append.qsc" */	/* host code */
		insrtitem = ERget(FE_Insert);
	    }
	}
	else
	{
	    insrtitem = ERx("");
	}
	if (detail_appends_supported)
	{
		/*
		**  If we're appending details to a master,
		**  then for each user-maintained logical key in the master
		**  we need to set its DBV to "empty" (the value that
		**  the back-end will supply when we insert the master
		**  without specifying the logical key column).
		**
		**  This is necessary in cases where a detail logical key
		**  is joined to the user logical key in the master, because
		**  the param string and arguments for appends to detail tables
		**  (mqaparam and mqaargv in RELINFO) need to include
		**  these joined logical keys (for the case where
		**  a detail is being appended to an *existing* master).
		**
		**  Note that we set the DBV of the master, not the detail.
		**  This works because joined DBV's share the same data value.
		**  It produces anomalies when a nullable logical key is
		**  joined to a non-nullable logical key, but they are
		**  the same anomalies that appear elsewhere in QBF
		**  (e.g. adding a detail to an existsing master,
		**  or adding a detail to a new master where they are joined
		**  on a *non*-logical key that's nullable on one side
		**  but not the other).
		**
		**  We accomplish the "set to empty" by calling adc_getempty.
		**  In order for it to work properly, we have to temporarily
		**  set db_datatype to ADF's internal code for logical keys
		**  instead of the code (DB_CHA_TYPE) that it exported to the
		**  front-ends.  If we didn't do this, non-nullable logical
		**  keys would be set to spaces instead of binary zeros.
		*/
		i4		i;
		ATTRIBINFO	*att;
		ADF_CB		*adf_cb = FEadfcb();
		for (i = 0; i < IIQFmlc_MLkeyInMDCount; i++)
		{
			att = IIQFmla_MLkeyInMDArray[i];
			if (!(att->lkeyinfo & mqSYSTEM_MAINT))
			{
				DB_DT_ID	old_datatype, new_datatype;
				if (att->lkeyinfo & mqTABLE_KEY)
				{
					new_datatype = DB_TABKEY_TYPE;
				}
				else
				{
					new_datatype = DB_LOGKEY_TYPE;
				}
				old_datatype = att->dbv.db_datatype;
				if (old_datatype > 0)
				{
					att->dbv.db_datatype = new_datatype;
				}
				else
				{
					att->dbv.db_datatype = -new_datatype;
				}
				(VOID) adc_getempty(adf_cb, &att->dbv);
				att->dbv.db_datatype = old_datatype;
			}
		}
	}
/* # line 266 "append.qsc" */	/* display */
    {
      if (IIdispfrm(mq_frame,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 270 "append.qsc" */	/* host code */
	    bool	good;
	    i4		addrtn = -1;
	    if (numflds == 0)
	    {
		IIUGerr(E_QF0003_NoFields, 0, 0);
/* # line 276 "append.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 277 "append.qsc" */	/* host code */
	    }
/* # line 279 "append.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 280 "append.qsc" */	/* host code */
	    good = TRUE;
	    if (!tblfield)
	    { /* append one row to each table in the 1-1 case */
/* # line 283 "append.qsc" */	/* getform */
            {
              if (IIfsetio(mq_frame) != 0) {
                IIrf_param(qdef->mqmgform, qdef->mqmgargv,(int *)0);
              } /* IIfsetio */
            }
/* # line 285 "append.qsc" */	/* host code */
		/* check for error from GETFORM, e.g., mismatches
		** between ** form field type and column data type. 
		** Fixes 6.0 bug 1304: entering NULL in a Nullable form
		** field where the underlying DB column type is NOT NULL
		** NOT DEFAULT.
		*/
/* # line 291 "append.qsc" */	/* inquire_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&err,1,(char *)0,0);
              } /* IIiqset */
            }
/* # line 292 "append.qsc" */	/* host code */
		if ( err != OK )
/* # line 293 "append.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 295 "append.qsc" */	/* host code */
		FEmsg(ERget(S_QF0004_Append), FALSE, (PTR)(totrows+1));
		_VOID_ IIUIbeginXaction();
		Mq_intransaction = TRUE;
		addrtn = addrow( qdef, _ALL );
		if ( addrtn != -1 )
		{
		    if (addrtn == 0)
		    {
			FEmsg(ERget(S_QF0005_NoRow), TRUE, (PTR)totrows);
		    }
		    /*
		    ** unsuccessful append...so backout transaction
		    */
		    _VOID_ IIUIabortXaction();
		    Mq_intransaction = FALSE;
		}
		else
		{
		    _VOID_ IIUIendXaction();
		    Mq_intransaction = FALSE;
		    totrows += 1;
		    /*
		    ** Force default values to be displayed on form.
		    ** Force keep previous values to be REDISPLAYed.
		    ** Clear other fields.
		    */
		    IIinitsp(mq_frame, FALSE, fdmdADD);
/* # line 325 "append.qsc" */	/* redisplay */
            {
              IIredisp();
            }
/* # line 326 "append.qsc" */	/* host code */
		}
	    }
	    else
	    { /* table field on form */
            register i4 numrows = 0;
		if (qdef_type == 1)
		{ /* append table from table field */
/* # line 334 "append.qsc" */	/* message */
            {
              IImessage(ERget(S_QF000E_Appending));
            }
/* # line 336 "append.qsc" */	/* host code */
		    do
		    { /* while not deadlock */
			_VOID_ IIUIbeginXaction();
			Mq_intransaction = TRUE;
/* # line 340 "append.qsc" */	/* unloadtable */
            {
              if (IItbact(mq_frame,mq_tbl,0) == 0) goto IItbE1;
              while (IItunload() != 0) {
                IItrc_param(qdef->mqmgform, qdef->mqmgargv,(int *)0);
                IITBceColEnd();
                {
/* # line 343 "append.qsc" */	/* host code */
			    Mq_inunloadtable = TRUE;
			    /* bug 9500
			    **	check for errors in unloadtable loop
			    */
/* # line 347 "append.qsc" */	/* inquire_frs */
                  {
                    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                      IIiqfsio((short *)0,1,30,4,&err,1,(char *)0,0);
                    } /* IIiqset */
                  }
/* # line 348 "append.qsc" */	/* host code */
			    if ( mqrstate != UNDEF && mqrstate != UNCHANGED )
			    {
				numrows += 1;
				if ( err != OK ||
				    (addrtn = addrow(qdef, _ALL)) != -1 )
				{
				    if ( err != OK || addrtn != DLCKERR )
				    {
					if (addrtn == 0)
						noaddmsg(numrows);
/* # line 358 "append.qsc" */	/* scroll */
                  {
                    if (IItbsetio(1,mq_frame,mq_tbl,-3) != 0) {
                      IItbsmode((char *)"to");
                      if (IItscroll(0,numrows) != 0) {
                      } /* IItscroll */
                    } /* IItbsetio */
                  }
/* # line 359 "append.qsc" */	/* host code */
					good = FALSE;
				    }
/* # line 361 "append.qsc" */	/* endloop */
                  {
                    if (1) goto IItbE1;
                  }
/* # line 362 "append.qsc" */	/* host code */
				}
			    }
                }
              } /* IItunload */
IItbE1:
              IItunend();
            }
/* # line 365 "append.qsc" */	/* host code */
			Mq_inunloadtable = FALSE;
		    } while ( addrtn == DLCKERR );
		}
		else
		{ /* master/detail with tbl field */
		    FEmsg(ERget(S_QF0004_Append), FALSE, (PTR)(mrows+1));
/* # line 371 "append.qsc" */	/* getform */
            {
              if (IIfsetio(mq_frame) != 0) {
                IIrf_param(qdef->mqmgform, qdef->mqmgargv,(int *)0);
              } /* IIfsetio */
            }
/* # line 373 "append.qsc" */	/* inquire_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&err,1,(char *)0,0);
              } /* IIiqset */
            }
/* # line 374 "append.qsc" */	/* host code */
		    if ( err != OK )
/* # line 375 "append.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 376 "append.qsc" */	/* host code */
		    else do
		    {
			_VOID_ IIUIbeginXaction();
			Mq_intransaction = TRUE;
			addrtn = addrow( qdef, _MASTER );
			if ( addrtn == DLCKERR )
				continue;
			if ( addrtn != -1 )
			{
			    if (addrtn == 0)
			    {
				IIUGerr(E_QF0007_NoMaster,0,0);
			    }
			    good = FALSE;
			}
			else if (detail_appends_supported)
			{
/* # line 394 "append.qsc" */	/* unloadtable */
            {
              if (IItbact(mq_frame,mq_tbl,0) == 0) goto IItbE2;
              while (IItunload() != 0) {
                IItrc_param(qdef->mqdgform, qdef->mqdgargv,(int *)0);
                IITBceColEnd();
                {
/* # line 397 "append.qsc" */	/* host code */
				Mq_inunloadtable = TRUE;
				/* bug 9500
				**	check for errors in unloadtable loop
				*/
/* # line 401 "append.qsc" */	/* inquire_frs */
                  {
                    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                      IIiqfsio((short *)0,1,30,4,&err,1,(char *)0,0);
                    } /* IIiqset */
                  }
/* # line 403 "append.qsc" */	/* host code */
				if (  mqrstate != UNDEF 
				   && mqrstate != UNCHANGED 
				   )
				{
				    numrows += 1;
				    if ( err != OK ||
					(addrtn = addrow(qdef, _DETAIL))
					!= -1 )
				    {
					if ( err != OK || addrtn != DLCKERR )
					{
					    if (addrtn == 0)
						noaddmsg(numrows);
/* # line 416 "append.qsc" */	/* scroll */
                  {
                    if (IItbsetio(1,mq_frame,mq_tbl,-3) != 0) {
                      IItbsmode((char *)"to");
                      if (IItscroll(0,numrows) != 0) {
                      } /* IItscroll */
                    } /* IItbsetio */
                  }
/* # line 417 "append.qsc" */	/* host code */
					    good = FALSE;
					}
/* # line 419 "append.qsc" */	/* endloop */
                  {
                    if (1) goto IItbE2;
                  }
/* # line 420 "append.qsc" */	/* host code */
				    }
				}
                }
              } /* IItunload */
IItbE2:
              IItunend();
            }
/* # line 423 "append.qsc" */	/* host code */
			    Mq_inunloadtable = FALSE;
			}
		    } while ( addrtn == DLCKERR );
		}
		if (!good)
		{
		    _VOID_ IIUIabortXaction();
		    Mq_intransaction = FALSE;
		}
		else
		{
		    _VOID_ IIUIendXaction();
		    Mq_intransaction = FALSE;
		    mrows += 1;
		    totrows += numrows;
		    /*
		    ** Force default values to be displayed on form.
		    ** Force keep previous values to be REDISPLAYed.
		    ** Clear all other fields.
		    */
/* # line 445 "append.qsc" */	/* clear */
            {
              IIfldclear(mq_tbl);
            }
/* # line 446 "append.qsc" */	/* host code */
		    IIinitsp(mq_frame, FALSE, fdmdADD);
/* # line 447 "append.qsc" */	/* redisplay */
            {
              IIredisp();
            }
/* # line 448 "append.qsc" */	/* host code */
		}
	    }
          }
        } else if (IIretval() == 2) {
          {
            i4 istable;
            i4 lrow;
            i4 rnum;
            i4 rowstate;
/* # line 459 "append.qsc" */	/* message */
            {
              IImessage(ERget(S_QF0008_InsertRow));
            }
/* # line 460 "append.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,mq_frame,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&istable,28,mq_tbl,0);
              } /* IIiqset */
            }
/* # line 461 "append.qsc" */	/* host code */
	    if (!istable)
	    {
		IIUGerr(E_QF000F_InsertOnTableFld,0,0);
/* # line 464 "append.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 465 "append.qsc" */	/* host code */
	    }
/* # line 466 "append.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,mq_frame,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&lrow,31,mq_tbl,0);
                IIiqfsio((short *)0,1,30,4,&rnum,29,mq_tbl,0);
              } /* IIiqset */
            }
/* # line 467 "append.qsc" */	/* getrow */
            {
              if (IItbsetio(2,mq_frame,mq_tbl,rnum) != 0) {
                IItcogetio((short *)0,1,30,4,&rowstate,(char *)"_RECOR\
D");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 468 "append.qsc" */	/* host code */
	    if ( lrow == 1 && rowstate == UNDEF )
	    {
			IIUGerr(E_QF0009_CannotInsert,0,0); 
	    }
	    else
	    {
		rnum -= 1;
/* # line 475 "append.qsc" */	/* insertrow */
            {
              if (IItbsetio(5,mq_frame,mq_tbl,rnum) != 0) {
                if (IItinsert() != 0) {
                  IITBceColEnd();
                } /* IItinsert */
              } /* IItbsetio */
            }
/* # line 476 "append.qsc" */	/* host code */
	    }
/* # line 477 "append.qsc" */	/* resume */
            {
              IIresfld(mq_tbl);
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 484 "append.qsc" */	/* host code */
		IIUFfieldHelp();
          }
        } else if (IIretval() == 4) {
          {
/* # line 491 "append.qsc" */	/* host code */
	    FEhelp(tblfield ? ERx("qapptx.hlp") : ERx("qappox.hlp"),
		ERget(S_QF000A_QBF_Append_Function));
          }
        } else if (IIretval() == 5) {
          {
/* # line 497 "append.qsc" */	/* host code */
	    if ( mq_func != mqAPPEND )
		FTbell();	/* quit key not allowed */
	    else
		goto endit;	/* OK */
          }
        } else if (IIretval() == 6) {
          {
            i4 changed;
/* # line 506 "append.qsc" */	/* host code */
endit:
/* # line 508 "append.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&changed,20,mq_frame,0);
              } /* IIiqset */
            }
/* # line 509 "append.qsc" */	/* host code */
	    if ( changed &&
		!IIUFver(ERget(S_QF000B_DoNotAppendQ),0) )
	    {
/* # line 512 "append.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 513 "append.qsc" */	/* host code */
	    }
	    if (qdef_type == 0)
		FEmsg(ERget(S_QF000C_TotalMDAppend), FALSE, (PTR)mrows, (PTR)totrows);
	    else
		FEmsg(ERget(S_QF000D_TotalAppend), FALSE, (PTR)totrows);
/* # line 519 "append.qsc" */	/* sleep */
            {
              IIsleep(3);
            }
/* # line 520 "append.qsc" */	/* clear */
            {
              IIclrflds();
            }
/* # line 521 "append.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(FE_Append),(char *)0,2,2,1) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(insrtitem,(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,3) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,0,2,5) == 0) goto IIfdE1;
      if (IInmuact(exititem,ERget(_EndExpl),0,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,2,6) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 524 "append.qsc" */	/* host code */
	return OK;
  }
static VOID
noaddmsg ( numrows )
i4	numrows;
{
	FEmsg(ERget(S_QF0001_NoRows), TRUE, (PTR)numrows);
}
/*
** addrow - does actual appends to tables in the qdef.
**		  The value of 'which' determines the tables
**		  that get rows appended to them:
**		which = _ALL -> all tables
**		which = _DETAIL -> detail tables
**		which = _MASTER -> master tables
**
**	Returns: -1 - all appends were succesful
**		  0 - no rows appended
**		num - an INGRES error occurred and no
**			  rows were appended. Return INGRES
**			  error number.
*/
static STATUS
addrow ( qdef, which )
register MQQDEF	*qdef;
register i4	which;
{
	register i4	i;
	/*
	** On deadlock, the transaction has already been
	** restarted.  Just restart the loop if desired.
	*/
apptry:
	for ( i = 0 ; i < qdef->mqnumtabs ; ++i )
	{
		register RELINFO *onerel = qdef->mqtabs[i];
		if ( which == _ALL ||
				( which == _DETAIL && onerel->mqtype == 1 ) ||
				( which == _MASTER && onerel->mqtype == 0 ) )
		{
			STATUS	err;
			i4	rcnt;
			/*
			** The following procedure call performs a dynamic
			** SQL INSERT using DB Data Values.
			*/
			IIQFsda_sqlInsertDBDV(onerel);
			err = FEinqerr();
			if (IIUIdeadlock(err))
			{
				err = DLCKERR;
			}
			if ( err == DLCKERR && (!tblfield))
			{
				goto beg_xact;
			}
			else if ( err != OK )
				return err;
			else if ( (rcnt = FEinqrows()) <= 0 )
				return 0;
		}
	}
	/* successful append */
	return -1;
beg_xact:
	Mq_intransaction = TRUE;
	_VOID_ IIUIbeginXaction();
	goto apptry;
}
