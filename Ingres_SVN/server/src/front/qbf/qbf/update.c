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
#include	<ug.h>
# include "mqbf.h"
#include	"mqeglobs.h"
#include	"erqf.h"
/**
** Name:	update.qc -  Query Update Frame Module.
**
** Description:
**	Contains the routine that implements the query update frame.  This
**	routine will be called after all the query has been set-up.  Defines:
**
**	IIqfUpdate()
**
**	Returns:
**	     1 = Done with updates, have calling routine
**		 show form in QUERY mode.
**	     0 = Error occurred in UPDATE mode.
**
**	Written: 2/28 /84 (nml)
**
** History:
**	Revision 6.5
**	4-aug-1993 (rdrane)
**		Add check of qdef->mqappendok to preclude append
**		operations on tables containing unsupported datatype
**		columns that are NOT NULL and have no defined DEFAULT.
**		Part of fix for b51411. Change all instances of IIUGerr()
**		to use #define for error severity.
**	Revision 6.4
**      01/13/92 (connie)
**              changed 'newrow' to 'iinewrow' which is less likely to
**              conflict with the user table column name
**	09/26/90 (emerson)
**		Don't display logical keys (bug 8593).
**	10/11/90 (emerson)
**		Replaced global variable allow_appends and allow_updates
**		by IIQFnkaNoKeyForAppends and IIQFnkuNoKeyForUpdates
**		(with opposite sense) because the names are misleading:
**		we now have other reasons for disallowing appends
**		(certain joins on logical keys), and we might have
**		other reasons for disallowing appends or updates
**		in the future.  Keeping separate flags allows us
**		to give specific error messages.  (bug 8593).
**	10/13/90 (emerson)
**		Check for unsupported appends of logical keys (bug 8593):
**		Put error message E_QF0170 if 'AddDetail' selected;
**		don't open up new rows in table field (this prevents user
**		from doing any appends).
**	11/23/90 (emerson)
**		Changed calling sequence for IIQFcdb_CopyDBDVs (bug 8593).
**
**	Revision 6.3
**	07-feb-90 (kenl)	bug fix 20007
**		When an error is encountered when unloading a tablefield, we
**		now set mqrstate to UNDEF instead of UNCHANGED to guarantee
**		that the row is not written to the database.
**	08-mar-90 (kenl)
**		Added ListChoices menuitem to allow user to obtain help
**		on fields.
**
**	Revision 6.1  88/09  wong
**	Modified for internation support of menuitems.  Merged duplicate
**	messages.  Merged 'mqmrowupd()' and 'mqdrowudp()' into '_rowupd()'.
**	10-aug-88 (kenl)
**		Changed INQUIRE_EQUEL to INQUIRE_FRS.  The old EQUEL
**		call used to check forms system (not any more).  Close
**		temporary file when QUERY selection is made.  Added
**		INQUIRE_FRS FORM after DELETEROW.  If an error happened
**		we now RESUME instead of reducing row count, etc.  On
**		SAVE command, we now validate all tablefields.
**	20-dec-88 (kenl)
**		Fixed problems involving mandatory fields and the forms
**		system.  Removed checks for QBF inserted row on deletes.  Now
**		we just delete it and re-insert it into the tablefield.  This
**		avoided validations being done by the forms system.  Also, in
**		_deltbl() we only scroll to row 1 if greater than 1 rows exist.
**
**	Revsion  6.0
**	07-dec-87 (kenl)
**		Turned module into a .QSC file in order to incorporate
**		the new header file: mqbf.qsh.  Reversed the order by
**		which a Master/Detail delete is done.  Now the Detail
**		is deleted first, followed by the Master.  Changed QUEL
**		transaction code to SQL.  Added calls to routine
**		IIQFcdb_CopyDBDVs before all PUTFORM and LOADFORM
**		statements that initially display values on a form so
**		that all hidden columns for keys are initialized.
**	11-dec-87 (kenl)
**		Removed the addition of 'newrow' to the list of hidden
**		columns.  This is now done in MQQDESC.QSC.
**	26-feb-88 (marian)
**		Took modifications to _chkchange from kenl.
**	26-feb-88 (marian) 
**		Integrate fix for 10883.  Don't validate if there were
**		no changes to the form.
**	18-jul-88 (kenl)
**		Fixed several bugs involving a master/detail joindef
**		where both are displayed in SimpleField format.  This
**		includes bug numbers 2818, 2816, 2827.  The routines
**		primarily affected were the MENUITEM additem and _chkchange().
**
**	Revision 5.0
**	17-jan-86 (marian)	bug # 6606
**		Set the global Mq_intransaction
**	17-jan-86 (marian)	bug # 6611
**		Determine if you're already in an unloadtable (caused
**		by interrupt), if so endloop.
**	16-jun-86 (mgw)		bug # 9582
**		Catch bad QG_alloc in QG_qbf properly and return error
**		message.
**	17-jun-86 (marian)	bug 9500
**		Catch errors in unloadtable loop.  This solves the
**		problem with tablefields and i2 overflow problems.
**		Changes were made to mmq_tblupd() and mqdmq_tblupd()
**	11-nov-86 (marian)	bug 10823
**		Don't do a replace for M/D single row format if there
**		were no detail rows retrieved.
**	19-nov-86 (marian)	bug 9170
**		When the 'AddDetail' menuitem is chosen and there are
**		no more rows, add the row and then print the message
**		that there are no more rows.
**
**	Revision 2.0  84/02/28  nadene
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**    25-Oct-2005 (hanje04)
**        Add prototype for _gnxtmas() to prevent compiler
**        errors with GCC 4.0 due to conflict with implicit declaration.
*/
#define	_HelpExpl		F_FE0100_ExplFrameHelp
#define	_EndExpl		F_FE0102_ExplEnd
static STATUS	_updrest();
static VOID	_updhlp();
static VOID	mmq_tblupd();
static bool	mqdmq_tblupd();
static bool	_qrydone();
static VOID	_stillupd();
static VOID	_rowupd();
static VOID	_deltbl();
static i4	_gnxtmas();
VOID	IIQFcdb_CopyDBDVs();
extern bool	Mq_intransaction;
extern bool	Mq_inunloadtable;
  extern char mq_frame[];
  extern char mq_tbl[];
  extern char mq_tids[];
STATUS
IIqfUpdate ( qdef )
register MQQDEF	*qdef;
  {
	register QDESC	*mqqry;
	i4		output;
	i4		i;
	i4		numflds;
	STATUS		status;
    i4 numrows;
    char *nextitem;
    char *scrollitem;
    char *additem;
    char *exititem;
	STATUS	rtncode;
	bool	anychanges;
	bool	master_allno;
	bool	detail_allno;
	bool	last_del;
	bool	no_masters;
	mqqry = qdef->mqqdesc;
	output = 0;
#ifdef DEBUG
	if (mq_debug)
	{
		SIfprintf(mqoutf,ERx("MQUPDATE: qdef_type = %d\n"), qdef_type);
		SIfprintf(mqoutf,ERx("MQUPDATE: tblfield = %d\n"), tblfield);
	}
#endif
	if (IIQG_qbf(mqqry, mqmrec, qdef->mqmrecl) != OK)
	{
		FDerror(ERRALLOC, 0, ERx(""));
		return FAIL;
	}
	/* open up temp file to store updates */
	if ( !mqwrtinit() )
	{
		FDerror(BADFILEOPEN, 0, ERx(""));
		return FAIL;
	}
	_VOID_ IIUIbeginXaction();
	Mq_intransaction = TRUE;
	if ( (status = IIQG_generate(mqqry, onetbl ? QI_START : QI_USTART,
				&output)) != OK )
	{
		mqwrtfin();
		mqwrtdel();
		if ( !IIUIdeadlock(status) )
		{
			_VOID_ IIUIendXaction();
			Mq_intransaction = FALSE;
		}
		return OK;
	}
	QG_mqqry = mqqry;	/* save for interrupt cleanup */
	if (output == QO_QDONE)
	{ /* no rows retrieved */
/* # line 235 "update.qsc" */	/* message */
    {
      IImessage(ERget(S_QF001C_NoRowsForQuery));
    }
/* # line 236 "update.qsc" */	/* sleep */
    {
      IIsleep(3);
    }
/* # line 238 "update.qsc" */	/* host code */
		mqwrtfin();
		mqwrtdel();
		_VOID_ IIUIendXaction();
		Mq_intransaction = FALSE;
		return OK;
	}
	master_allno = TRUE;
	detail_allno = TRUE;
	last_del = FALSE;
	no_masters = FALSE;
	/* enforce no deletes if specified */
	for ( i = 0 ; i < qdef->mqnumtabs ; ++i )
	{
		register RELINFO *rel = qdef->mqtabs[i];
		if (rel->opdelete)
		{
			if ( rel->mqtype == mqIN_MASTER )
				master_allno = FALSE;
			else
				detail_allno = FALSE;
		}
	}
	/* got data - redisplay the form in UPDATE mode */
	rtncode = OK;
	anychanges = FALSE;
	numrows = 0;
	exititem = ( mq_func == mqUPDATE ) ? ERget(FE_Quit) : ERget(FE_End);
	nextitem = scrollitem = additem = ERx("");
	if (tblfield)
	{
		/*
		**  mq_tids should ALWAYS have key columns and/or tids in
		**  it.  Otherwise, MQUPDATE should never be called.
		**  (the global variable IIQFnkuNoKeyForUpdates should be TRUE)
		*/
/* # line 280 "update.qsc" */	/* inittable */
    {
      if (IItbinit(mq_frame,mq_tbl,(char *)"u") != 0) {
        IIthidecol(mq_tids,(char *)0);
        IItfill();
      } /* IItbinit */
    }
/* # line 281 "update.qsc" */	/* host code */
		if (qdef_type == 0)
		{
			nextitem = ERget(F_QF0001_NextMaster);
		}
	}
	else
	{
		if (qdef_type == 0)
		{
			nextitem = ERget(F_QF0001_NextMaster);
			scrollitem = ERget(F_QF0002_NxtDetail);
			additem = ERget(F_QF001E_AddDetail);
		}
		else
		{
			nextitem = ERget(FE_Next);
		}
	}
/* # line 300 "update.qsc" */	/* display */
    {
      if (IIdispfrm(mq_frame,(char *)"u") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 303 "update.qsc" */	/* host code */
		if ( (status = mqretinit(qdef, &output, &numrows)) != OK )
		{
			IIUGerr( E_QF00AB_InitFormErr, UG_ERR_ERROR, 0 );
			if ((IIQG_generate(mqqry, QI_BREAK, &output)) != OK)
			{
				FDerror(BADBREAK, 0, ERx(""));
			}
			QG_mqqry = NULL;
			mqwrtfin();
			mqwrtdel();
			if ( !IIUIdeadlock(status) )
				rtncode = FAIL;
/* # line 317 "update.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 318 "update.qsc" */	/* host code */
		}
		numflds = mq_mflds(qdef);
		if  ((tblfield) && (numrows == 0))
		{
			/*
			** The real question is how do we get here?  If
			** there are indeed no rows, we should have returned
			** above out of the IIQG_generate() since output
			** should have been QO_QDONE.  This pretty much makes
			** E_QF0178_AppUnDataType unnecessary.  Oh well ... 
			*/
			if  (!qdef->mqappendok)
			{
				IIUGerr(E_QF0178_AppUnDataType,UG_ERR_ERROR,0);
/* # line 333 "update.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 334 "update.qsc" */	/* host code */
			}
			/* kludge to allow tabbing into table field */
			if  ((qdef_type == 0) &&
			     (!IIQFsddSystemUserLkeyDD) &&
			     (!IIQFsmmSystemUserLkeyMM))
			{
/* # line 341 "update.qsc" */	/* insertrow */
        {
          if (IItbsetio(5,mq_frame,mq_tbl,0) != 0) {
            if (IItinsert() != 0) {
              IItcoputio((char *)"iinewrow",(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
              IITBceColEnd();
            } /* IItinsert */
          } /* IItbsetio */
        }
/* # line 342 "update.qsc" */	/* host code */
				numrows = 1;
			}
		}
		if  (!qdef->mqappendok)
		{
			IIUGerr(E_QF0180_NoAppAllowed,UG_ERR_ERROR,0);
		}
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
            i4 fchange;
/* # line 356 "update.qsc" */	/* host code */
		if ( nextitem[0] == EOS )
		{
			FTbell();
/* # line 359 "update.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 360 "update.qsc" */	/* host code */
		}
		if (output == QO_QDONE)
		{
/* # line 363 "update.qsc" */	/* message */
            {
              IImessage(ERget(S_QF0079_No_more_rows_in_query));
            }
/* # line 364 "update.qsc" */	/* sleep */
            {
              IIsleep(3);
            }
/* # line 365 "update.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 366 "update.qsc" */	/* host code */
		}
		/* bug 10883
		** 	Make sure there was a change to the 
		** 	form before validating.
		*/
/* # line 372 "update.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&fchange,20,mq_frame,0);
              } /* IIiqset */
            }
/* # line 373 "update.qsc" */	/* host code */
		if (fchange)
		{
/* # line 375 "update.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 376 "update.qsc" */	/* host code */
			anychanges = TRUE;
		}
		fchange = _gnxtmas(qdef, &status, &output, &numrows);
		if ( fchange < 0 && IIUIdeadlock(status) )
		{
/* # line 383 "update.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 384 "update.qsc" */	/* host code */
		}
		if (output == QO_QDONE)
		{
			no_masters = TRUE;
		}
          }
        } else if (IIretval() == 2) {
          {
            i4 fchange;
/* # line 396 "update.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&fchange,20,mq_frame,0);
              } /* IIiqset */
            }
/* # line 397 "update.qsc" */	/* host code */
		if (fchange || anychanges)
		{
			anychanges = TRUE;
			if ( !IIUFver(ERget(S_QF00AC_DoNotUpdateQ), 0) )
			{
/* # line 402 "update.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 403 "update.qsc" */	/* host code */
			}
		}
/* # line 405 "update.qsc" */	/* message */
            {
              IImessage(ERget(S_QF00AF_ReturnQryMode));
            }
/* # line 406 "update.qsc" */	/* host code */
		if ((IIQG_generate(mqqry, QI_BREAK, &output)) != OK)
		{
			FDerror(BADBREAK, 0, ERx(""));
		}
		QG_mqqry = NULL;
		mqwrtfin();
		mqwrtdel();
		anychanges = FALSE;
/* # line 415 "update.qsc" */	/* clear */
            {
              IIclrflds();
            }
/* # line 416 "update.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 421 "update.qsc" */	/* host code */
		if (output == QO_QDONE && !(qdef_type && tblfield))
		{
/* # line 423 "update.qsc" */	/* message */
            {
              IImessage(ERget(S_QF0079_No_more_rows_in_query));
            }
/* # line 424 "update.qsc" */	/* sleep */
            {
              IIsleep(3);
            }
/* # line 425 "update.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 426 "update.qsc" */	/* host code */
		}
		if (master_allno && detail_allno)
		{
/* # line 429 "update.qsc" */	/* message */
            {
              IImessage(ERget(S_QF00B0_DeletesNotPermit));
            }
/* # line 430 "update.qsc" */	/* sleep */
            {
              IIsleep(3);
            }
/* # line 431 "update.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 432 "update.qsc" */	/* host code */
		}
		if (!tblfield && qdef_type == 1)
		{
			if ( _qrydone(output, TRUE) || numflds == 0 )
/* # line 437 "update.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 439 "update.qsc" */	/* getform */
            {
              if (IIfsetio(mq_frame) != 0) {
                IIrf_param(qdef->mqmgform, qdef->mqmgargv,(int *)0);
              } /* IIfsetio */
            }
/* # line 442 "update.qsc" */	/* host code */
			_rowupd( qdef, mqDELREC, mqIN_MASTER );
			if ( (status = mqgetrecord(qdef, &output)) != OK )
			{
/* # line 447 "update.qsc" */	/* clear */
            {
              IIclrflds();
            }
/* # line 448 "update.qsc" */	/* host code */
				if ( IIUIdeadlock(status) )
				{
					_stillupd(qdef);
/* # line 451 "update.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 452 "update.qsc" */	/* host code */
				}
			}
		    	if (output == QO_QDONE)
		    	{
/* # line 457 "update.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(21,mq_frame,0,(short *)0,1,32,0,(char *)"r");
              } /* IIiqset */
            }
/* # line 458 "update.qsc" */	/* host code */
		    	}
		}
		else
		{
            char *delsub1;
            char *delsub2;
            char *delsub3;
            i4 anyrows;
			bool	delrow;
			delsub1 = delsub2 = delsub3 = ERx("");
			if (tblfield)
			{
				if (qdef_type == 0)
				{
					delsub1 = ERget(F_QF0013_Master);
					delsub2 = ERget(F_QF001A_AllDetailRows);
					delsub3 = ERget(F_QF001B_DetailRow);
				}
				else
				{
					delsub1 = ERget(F_QF001C_AllRows);
					delsub2 = ERget(F_QF0003_Row);
				}
			}
			else
			{
				if (qdef_type == 0)
				{
					delsub1 = ERget(F_QF0013_Master);
					delsub2 = ERget(F_QF0014_Detail);
				}
			}
			for (;;)
			{
/* # line 493 "update.qsc" */	/* submenu */
            {
              goto IImuI2;
IIfdB2:
              IIrunmu(0);
              if (IIretval() == 1) {
                {
/* # line 496 "update.qsc" */	/* host code */
				if ( ( qdef_type != 1 &&
						_qrydone(output, TRUE) ) ||
							numflds == 0 )
/* # line 499 "update.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 500 "update.qsc" */	/* host code */
				if ( qdef_type == 1 )
				{ /* one-to-one tablefield */
/* # line 502 "update.qsc" */	/* inquire_frs */
                  {
                    if (IIiqset(3,0,mq_frame,(char *)0, (char *)0) != 0) {
                      IIiqfsio((short *)0,1,30,4,&anyrows,31,mq_tbl,0);
                    } /* IIiqset */
                  }
/* # line 505 "update.qsc" */	/* host code */
					if (anyrows == 0)
					{
						_VOID_ _qrydone(QO_QDONE, TRUE);
/* # line 508 "update.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 509 "update.qsc" */	/* host code */
					}
					_deltbl(numrows);
					_VOID_ mmq_tblupd(qdef);
/* # line 513 "update.qsc" */	/* clear */
                  {
                    IIfldclear(mq_tbl);
                  }
/* # line 514 "update.qsc" */	/* host code */
					numrows = 0;
					if  ((!IIQFsmmSystemUserLkeyMM) &&
					     (qdef->mqappendok))
					{
/* # line 518 "update.qsc" */	/* insertrow */
                  {
                    if (IItbsetio(5,mq_frame,mq_tbl,0) != 0) {
                      if (IItinsert() != 0) {
                        IItcoputio((char *)"iinewrow",(short *)0,1,30,sizeof(1
                        ),(void *)IILQint(1));
                        IITBceColEnd();
                      } /* IItinsert */
                    } /* IItbsetio */
                  }
/* # line 519 "update.qsc" */	/* host code */
						numrows = 1;
					}
/* # line 522 "update.qsc" */	/* set_frs */
                  {
                    if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                      IIstfsio(20,mq_frame,0,(short *)0,1,30,sizeof(0),
                      (void *)IILQint(0));
                    } /* IIiqset */
                  }
/* # line 524 "update.qsc" */	/* host code */
					if ( _updrest(qdef, &output, mqDELREC)
							== DLCKERR )
					{
/* # line 527 "update.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE1;
                  }
/* # line 528 "update.qsc" */	/* host code */
					}
				}
				else if ( tblfield )
				{ /* master/detail with tablefield */
/* # line 532 "update.qsc" */	/* getform */
                  {
                    if (IIfsetio(mq_frame) != 0) {
                      IIrf_param(qdef->mqmgform, qdef->mqmgargv,(int *)0);
                    } /* IIfsetio */
                  }
/* # line 535 "update.qsc" */	/* host code */
				    _deltbl(numrows);
				    _VOID_ mqdmq_tblupd(qdef);
				    if ( _updrest(qdef, &output, mqDELREC)
						== DLCKERR )
					{
/* # line 540 "update.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE1;
                  }
/* # line 541 "update.qsc" */	/* host code */
					}
					_rowupd( qdef, mqDELREC, mqIN_MASTER );
					numrows = 0;
					if ( (status = mqgetmaster(qdef,
								&output,
								&numrows
							)) != OK )
					{
/* # line 550 "update.qsc" */	/* clear */
                  {
                    IIclrflds();
                  }
/* # line 551 "update.qsc" */	/* host code */
						if ( IIUIdeadlock(status) )
						{
							_stillupd(qdef);
/* # line 554 "update.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE1;
                  }
/* # line 555 "update.qsc" */	/* host code */
						}
						/* appends are meaningless at this point so */
						/* make form read only till next query	    */
						anychanges = TRUE;
/* # line 560 "update.qsc" */	/* set_frs */
                  {
                    if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                      IIstfsio(21,mq_frame,0,(short *)0,1,32,0,(char *)"\
r");
                    } /* IIiqset */
                  }
/* # line 561 "update.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 562 "update.qsc" */	/* host code */
					}
					if  ((numrows == 0) &&
					     (!IIQFsddSystemUserLkeyDD) &&
					     (qdef->mqappendok))
					{
/* # line 567 "update.qsc" */	/* insertrow */
                  {
                    if (IItbsetio(5,mq_frame,mq_tbl,0) != 0) {
                      if (IItinsert() != 0) {
                        IItcoputio((char *)"iinewrow",(short *)0,1,30,sizeof(1
                        ),(void *)IILQint(1));
                        IITBceColEnd();
                      } /* IItinsert */
                    } /* IItbsetio */
                  }
/* # line 568 "update.qsc" */	/* host code */
						numrows = 1;
					}
				}
				else
				{ /* master/detail without tablefield */
/* # line 573 "update.qsc" */	/* getform */
                  {
                    if (IIfsetio(mq_frame) != 0) {
                      IIrf_param(qdef->mqmgform, qdef->mqmgargv,(int *)0);
                    } /* IIfsetio */
                  }
/* # line 576 "update.qsc" */	/* getform */
                  {
                    if (IIfsetio(mq_frame) != 0) {
                      IIrf_param(qdef->mqdgform, qdef->mqdgargv,(int *)0);
                    } /* IIfsetio */
                  }
/* # line 580 "update.qsc" */	/* host code */
					if (output != QO_DDONE)
					{
						_rowupd( qdef, mqDELREC, mqIN_DETAIL );
					}
					if ( IIUFver(ERget(S_QF00B1_DeleteAllAssoc),0) &&
						_updrest(qdef, &output,mqDELREC)
							== DLCKERR )
					{
/* # line 589 "update.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE1;
                  }
/* # line 590 "update.qsc" */	/* host code */
					}
					_rowupd( qdef, mqDELREC, mqIN_MASTER );
					if ( (status = mqgetmaster(qdef,
								&output,
								&numrows)
							) != OK )
					{
						no_masters = TRUE;
/* # line 599 "update.qsc" */	/* clear */
                  {
                    IIclrflds();
                  }
/* # line 600 "update.qsc" */	/* host code */
						if ( IIUIdeadlock(status) )
						{
							_stillupd(qdef);
/* # line 603 "update.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE1;
                  }
/* # line 604 "update.qsc" */	/* host code */
						}
						/*	appends are meaningless at this point so */
						/*	make form read only till next query	 */
/* # line 607 "update.qsc" */	/* set_frs */
                  {
                    if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                      IIstfsio(21,mq_frame,0,(short *)0,1,32,0,(char *)"\
r");
                    } /* IIiqset */
                  }
/* # line 608 "update.qsc" */	/* host code */
						anychanges = TRUE;
/* # line 609 "update.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 610 "update.qsc" */	/* host code */
					}
				}
				break;
                }
              } else if (IIretval() == 2) {
                {
/* # line 616 "update.qsc" */	/* host code */
				delrow = !tblfield || qdef_type != 0;
			delrowl:
				if (qdef_type == 0 && detail_allno)
				{
					FEmsg(ERget(S_QF00B2_Deletes_of_details_ar), TRUE);
/* # line 621 "update.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 622 "update.qsc" */	/* host code */
				}
				if (qdef_type)	/* one big table field */
				{
					if (numflds == 0)
/* # line 627 "update.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 628 "update.qsc" */	/* inquire_frs */
                  {
                    if (IIiqset(3,0,mq_frame,(char *)0, (char *)0) != 0) {
                      IIiqfsio((short *)0,1,30,4,&anyrows,31,mq_tbl,0);
                    } /* IIiqset */
                  }
/* # line 629 "update.qsc" */	/* host code */
					if (anyrows == 0)
					{
/* # line 631 "update.qsc" */	/* message */
                  {
                    IImessage(ERget(S_QF0079_No_more_rows_in_query));
                  }
/* # line 632 "update.qsc" */	/* sleep */
                  {
                    IIsleep(3);
                  }
/* # line 633 "update.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 634 "update.qsc" */	/* host code */
					}
				}
				else if (qdef_type == 0 && tblfield)	/* MD and tablefield */
				{
/* # line 638 "update.qsc" */	/* inquire_frs */
                  {
                    if (IIiqset(3,0,mq_frame,(char *)0, (char *)0) != 0) {
                      IIiqfsio((short *)0,1,30,4,&anyrows,31,mq_tbl,0);
                    } /* IIiqset */
                  }
/* # line 639 "update.qsc" */	/* host code */
					if (anyrows == 0)
					{
/* # line 641 "update.qsc" */	/* message */
                  {
                    IImessage(ERget(S_QF00B3_No_more_detail_rows));
                  }
/* # line 642 "update.qsc" */	/* sleep */
                  {
                    IIsleep(3);
                  }
/* # line 643 "update.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 644 "update.qsc" */	/* host code */
					}
				}
				/* MD and !tablefield */
				else if ( _qrydone(output, TRUE) || numflds == 0 )
/* # line 648 "update.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 649 "update.qsc" */	/* host code */
				else if ( qdef_type == 0 && output == QO_DDONE )
				{
/* # line 651 "update.qsc" */	/* message */
                  {
                    IImessage(ERget(S_QF00B4_No_detail_rows_to_del));
                  }
/* # line 652 "update.qsc" */	/* sleep */
                  {
                    IIsleep(3);
                  }
/* # line 653 "update.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 654 "update.qsc" */	/* host code */
				}
				if (tblfield)
				{
				    if (delrow)
                  {
                    i4 istable;
/* # line 662 "update.qsc" */	/* inquire_frs */
                    {
                      if (IIiqset(2,0,mq_frame,(char *)0, (char *)0) != 0) {
                        IIiqfsio((short *)0,1,30,4,&istable,28,(char *)0,0);
                      } /* IIiqset */
                    }
/* # line 664 "update.qsc" */	/* host code */
					if (istable)
                    {
                      i4 lrow;
                      i4 err;
/* # line 669 "update.qsc" */	/* inquire_frs */
                      {
                        if (IIiqset(3,0,mq_frame,(char *)0, (char *)0) != 0) 
                        {
                          IIiqfsio((short *)0,1,30,4,&lrow,29,mq_tbl,0);
                        } /* IIiqset */
                      }
/* # line 670 "update.qsc" */	/* deleterow */
                      {
                        if (IItbsetio(4,mq_frame,mq_tbl,lrow) != 0) {
                          if (IItdelrow(0) != 0) {
                          } /* IItdelrow */
                        } /* IItbsetio */
                      }
/* # line 671 "update.qsc" */	/* inquire_frs */
                      {
                        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0
                        ) {
                          IIiqfsio((short *)0,1,30,4,&err,1,(char *)0,0);
                        } /* IIiqset */
                      }
/* # line 672 "update.qsc" */	/* host code */
					    if ( err != OK )
					    {
/* # line 674 "update.qsc" */	/* resume */
                      {
                        if (1) goto IIfdB1;
                      }
/* # line 675 "update.qsc" */	/* host code */
					    }
					    else
					    {
/* # line 678 "update.qsc" */	/* set_frs */
                      {
                        if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0
                        ) {
                          IIstfsio(20,mq_frame,0,(short *)0,1,30,sizeof(1),
                          (void *)IILQint(1));
                        } /* IIiqset */
                      }
/* # line 679 "update.qsc" */	/* host code */
					        numrows--;
					    }
                    }
					else
					{
						FEmsg(ERget(S_QF00B5_Must_be_on_a_row_to_d), TRUE);
/* # line 685 "update.qsc" */	/* resume */
                    {
                      if (1) goto IIfdB1;
                    }
/* # line 686 "update.qsc" */	/* host code */
					}
                  }
				    else
				    { /* AllRows */
					_deltbl(numrows);
					if (qdef_type == 0)
					{
/* # line 693 "update.qsc" */	/* getform */
                  {
                    if (IIfsetio(mq_frame) != 0) {
                      IIrf_param(qdef->mqmgform, qdef->mqmgargv,(int *)0);
                    } /* IIfsetio */
                  }
/* # line 694 "update.qsc" */	/* host code */
						_VOID_ mqdmq_tblupd(qdef);
/* # line 695 "update.qsc" */	/* clear */
                  {
                    IIfldclear(mq_tbl);
                  }
/* # line 696 "update.qsc" */	/* host code */
					}
					if ( _updrest(qdef, &output, mqDELREC)
							== DLCKERR )
					{
/* # line 700 "update.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE1;
                  }
/* # line 701 "update.qsc" */	/* host code */
					}
					numrows = 0;
					if ( qdef_type == 0 )
					{
    					_rowupd( qdef, mqNOPREC, mqIN_MASTER );
					}
				    }
				    anychanges = TRUE;
				    if (numrows == 0)
				    {
					if (delrow)
					{
					    if (qdef_type == 0)
					    {
						_VOID_ mqdmq_tblupd(qdef);
					    }
					    else
					    {
						_VOID_ mmq_tblupd(qdef);
					    }
					}
/* # line 724 "update.qsc" */	/* clear */
                  {
                    IIfldclear(mq_tbl);
                  }
/* # line 725 "update.qsc" */	/* host code */
					if  ((!IIQFsddSystemUserLkeyDD) &&
					     (!IIQFsmmSystemUserLkeyMM) &&
					     (qdef->mqappendok))
					{
/* # line 729 "update.qsc" */	/* insertrow */
                  {
                    if (IItbsetio(5,mq_frame,mq_tbl,0) != 0) {
                      if (IItinsert() != 0) {
                        IItcoputio((char *)"iinewrow",(short *)0,1,30,sizeof(1
                        ),(void *)IILQint(1));
                        IITBceColEnd();
                      } /* IItinsert */
                    } /* IItbsetio */
                  }
/* # line 730 "update.qsc" */	/* host code */
						numrows = 1;
					}
				        if ( qdef_type != 0 )
/* # line 734 "update.qsc" */	/* set_frs */
                  {
                    if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                      IIstfsio(20,mq_frame,0,(short *)0,1,30,sizeof(0),
                      (void *)IILQint(0));
                    } /* IIiqset */
                  }
/* # line 735 "update.qsc" */	/* host code */
				    }
				}
				else
				{
					anychanges = TRUE;
/* # line 740 "update.qsc" */	/* getform */
                  {
                    if (IIfsetio(mq_frame) != 0) {
                      IIrf_param(qdef->mqmgform, qdef->mqmgargv,(int *)0);
                    } /* IIfsetio */
                  }
/* # line 741 "update.qsc" */	/* getform */
                  {
                    if (IIfsetio(mq_frame) != 0) {
                      IIrf_param(qdef->mqdgform, qdef->mqdgargv,(int *)0);
                    } /* IIfsetio */
                  }
/* # line 742 "update.qsc" */	/* host code */
					_rowupd( qdef, mqDELREC, mqIN_DETAIL );
					_rowupd( qdef, mqUPDREC, mqIN_MASTER );
					if ( (status = mqgetdtl(qdef, &output))
							!= OK )
					{
					    last_del = TRUE;
/* # line 748 "update.qsc" */	/* clear */
                  {
                    IIclrflds();
                  }
/* # line 749 "update.qsc" */	/* host code */
					    if ( IIUIdeadlock(status) )
					    {
					        _stillupd(qdef);
/* # line 752 "update.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE1;
                  }
/* # line 753 "update.qsc" */	/* host code */
					    }
					    IIQFcdb_CopyDBDVs(qdef, mqIN_MASTER);
/* # line 755 "update.qsc" */	/* putform */
                  {
                    if (IIfsetio(mq_frame) != 0) {
                      IIsf_param(qdef->mqmpform,
						qdef->mqmpargv,(int *)0);
                    } /* IIfsetio */
                  }
/* # line 757 "update.qsc" */	/* host code */
					}
/* # line 758 "update.qsc" */	/* set_frs */
                  {
                    if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                      IIstfsio(20,mq_frame,0,(short *)0,1,30,sizeof(0),
                      (void *)IILQint(0));
                    } /* IIiqset */
                  }
/* # line 759 "update.qsc" */	/* host code */
				}
/* # line 760 "update.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 761 "update.qsc" */	/* host code */
				break;
                }
              } else if (IIretval() == 3) {
                {
/* # line 765 "update.qsc" */	/* host code */
				delrow = TRUE;
				goto delrowl;
                }
              } else if (IIretval() == 4) {
                {
/* # line 771 "update.qsc" */	/* host code */
				char	*help_file;
				if (tblfield)
				{
				    help_file = ( qdef_type == 0 )
							? ERx("qdelsbtm.hlp")
							: ERx("qdelsbto.hlp");
				}
				else
				    help_file = ERx("qdelsbmd.hlp");
				FEhelp( help_file, ERget(S_QF00B6_DeleteMenu) );
				continue;
                }
              } else if (IIretval() == 5) {
                {
/* # line 787 "update.qsc" */	/* host code */
				break;
                }
              } else {
                if (1) goto IIfdE2;
              } /* IIretval */
              goto IIfdE2;
IImuI2:
              IImuonly();
              if (IIinitmu() == 0) goto IIfdE2;
              if (IInmuact(delsub1,(char *)0,2,2,1) == 0) goto IIfdE2;
              if (IInmuact(delsub2,(char *)0,2,2,2) == 0) goto IIfdE2;
              if (IInmuact(delsub3,(char *)0,2,2,3) == 0) goto IIfdE2;
              if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),2,2,4) == 0) goto 
              IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(1,(char *)0,2,2,4) == 0) goto IIfdE2;
              if (IInmuact(ERget(FE_End),ERget(_EndExpl),2,2,5) == 0) goto 
              IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(3,(char *)0,2,2,5) == 0) goto IIfdE2;
              if (IIendmu() == 0) goto IIfdE2;
              goto IIfdB2;
IIfdE2:;
            }
/* # line 789 "update.qsc" */	/* host code */
			} /* end SUBMENU */
		}
		anychanges = TRUE;
/* # line 792 "update.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 4) {
          {
            i4 fchange;
/* # line 799 "update.qsc" */	/* host code */
		if ( output == QO_QDONE && ( qdef_type == 0 || !tblfield ) )
		{
/* # line 801 "update.qsc" */	/* message */
            {
              IImessage(ERget(S_QF0079_No_more_rows_in_query));
            }
/* # line 802 "update.qsc" */	/* sleep */
            {
              IIsleep(3);
            }
/* # line 803 "update.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 804 "update.qsc" */	/* host code */
		}
/* # line 805 "update.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&fchange,20,mq_frame,0);
              } /* IIiqset */
            }
/* # line 806 "update.qsc" */	/* host code */
		if (fchange)
		{
			/* bug 10883
			** 	Make sure there was a change to the 
			** 	form before validating.
			*/
/* # line 812 "update.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 813 "update.qsc" */	/* host code */
			anychanges = TRUE;
/* # line 814 "update.qsc" */	/* getform */
            {
              if (IIfsetio(mq_frame) != 0) {
                IIrf_param(qdef->mqmgform, qdef->mqmgargv,(int *)0);
              } /* IIfsetio */
            }
/* # line 815 "update.qsc" */	/* getform */
            {
              if (IIfsetio(mq_frame) != 0) {
                IIrf_param(qdef->mqdgform, qdef->mqdgargv,(int *)0);
              } /* IIfsetio */
            }
/* # line 816 "update.qsc" */	/* host code */
			_rowupd( qdef, mqUPDREC, mqIN_MASTER );
			/* bug 10823
			**	Only do an update if there was a detail row
			**	retrieved.
			*/
			if (output != QO_DDONE)
			{
				_rowupd( qdef, mqUPDREC, mqIN_DETAIL );
			}
			/*
			** Do NOT reset 'fchange' because a change to join
			** field must be reflected for the next detail.
			*/
		}
		status = mqgetdtl(qdef, &output);
		if ( IIUIdeadlock(status) )
		{
			_stillupd(qdef);
/* # line 837 "update.qsc" */	/* clear */
            {
              IIclrflds();
            }
/* # line 838 "update.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 839 "update.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 5) {
          {
/* # line 844 "update.qsc" */	/* host code */
		if ( IIQFsddSystemUserLkeyDD )
		{
			IIUGerr(E_QF0170_DD_system_user_append,
				UG_ERR_ERROR, 0);
/* # line 848 "update.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 849 "update.qsc" */	/* host code */
		}
		if (numflds == 0)
/* # line 852 "update.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 854 "update.qsc" */	/* host code */
		if (no_masters)
		{
/* # line 856 "update.qsc" */	/* message */
            {
              IImessage(ERget(S_QF0079_No_more_rows_in_query));
            }
/* # line 857 "update.qsc" */	/* sleep */
            {
              IIsleep(3);
            }
/* # line 858 "update.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 859 "update.qsc" */	/* host code */
		}
/* # line 861 "update.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 863 "update.qsc" */	/* message */
            {
              IImessage(ERget(S_QF0152_DetailAdded));
            }
/* # line 865 "update.qsc" */	/* getform */
            {
              if (IIfsetio(mq_frame) != 0) {
                IIrf_param(qdef->mqmgform, qdef->mqmgargv,(int *)0);
              } /* IIfsetio */
            }
/* # line 866 "update.qsc" */	/* getform */
            {
              if (IIfsetio(mq_frame) != 0) {
                IIrf_param(qdef->mqdgform, qdef->mqdgargv,(int *)0);
              } /* IIfsetio */
            }
/* # line 867 "update.qsc" */	/* host code */
		anychanges = TRUE;
		_rowupd( qdef, mqNOPREC, mqIN_MASTER );
		_rowupd( qdef, mqADDREC, mqIN_DETAIL );
		/* bug 9170
		**	Do the set_frs before the if statement and do the
		**	if statement after appending the new row.
		*/
/* # line 875 "update.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,mq_frame,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 877 "update.qsc" */	/* host code */
		/* clear the detail by clearing all and then */
		/* restoring the master */
		IIQFcdb_CopyDBDVs( qdef, mqIN_MASTER );
/* # line 880 "update.qsc" */	/* clear */
            {
              IIclrflds();
            }
/* # line 881 "update.qsc" */	/* putform */
            {
              if (IIfsetio(mq_frame) != 0) {
                IIsf_param(qdef->mqmpform, qdef->mqmpargv,(int *)0);
              } /* IIfsetio */
            }
/* # line 883 "update.qsc" */	/* host code */
	 	if ( output != QO_DDONE && !last_del )
		{
		    IIQFcdb_CopyDBDVs( qdef, mqIN_DETAIL );
/* # line 886 "update.qsc" */	/* putform */
            {
              if (IIfsetio(mq_frame) != 0) {
                IIsf_param(qdef->mqdpform, qdef->mqdpargv,(int *)0);
              } /* IIfsetio */
            }
/* # line 887 "update.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 6) {
          {
            i4 fchange;
/* # line 894 "update.qsc" */	/* host code */
		if (numflds == 0)
			goto brkqry;
/* # line 896 "update.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&fchange,20,mq_frame,0);
              } /* IIiqset */
            }
/* # line 898 "update.qsc" */	/* host code */
		/* bug 10883
		** 	Make sure there was a change to the 
		** 	form before validating.
		*/
		if (fchange)
		{
			if ( output == QO_QDONE && qdef_type != 0 && tblfield )
			{ /* not on cleared field in this case */
/* # line 906 "update.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 907 "update.qsc" */	/* host code */
			}
			else if (output != QO_QDONE) 
			{ /* IN CASE OF A MANDATORY FIELD */
/* # line 910 "update.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 911 "update.qsc" */	/* host code */
			}
		}
		if ( fchange || anychanges )
		{
/* # line 916 "update.qsc" */	/* message */
            {
              IImessage(ERget(S_QF00B7_SavingChanges));
            }
/* # line 917 "update.qsc" */	/* host code */
			anychanges = TRUE;
			if (tblfield && fchange)
			{
			    if ( qdef_type == 0 )
			    {
/* # line 922 "update.qsc" */	/* getform */
            {
              if (IIfsetio(mq_frame) != 0) {
                IIrf_param(qdef->mqmgform, qdef->mqmgargv,(int *)0);
              } /* IIfsetio */
            }
/* # line 925 "update.qsc" */	/* host code */
				_rowupd( qdef, mqUPDREC, mqIN_MASTER );
				if ( !mqdmq_tblupd(qdef) &&
				    	_updrest(qdef, &output, mqUPDREC)
						== DLCKERR )
				{
/* # line 930 "update.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 931 "update.qsc" */	/* host code */
				}
			    }
			    else
			    {
				mmq_tblupd(qdef);
			    }
			}
			if ( !tblfield && fchange )
			{
/* # line 940 "update.qsc" */	/* getform */
            {
              if (IIfsetio(mq_frame) != 0) {
                IIrf_param(qdef->mqmgform, qdef->mqmgargv,(int *)0);
              } /* IIfsetio */
            }
/* # line 941 "update.qsc" */	/* host code */
			    if ( qdef_type != 0 )
			    {
				_rowupd( qdef, mqUPDREC, mqIN_MASTER );
			    }
			    else
			    {
/* # line 947 "update.qsc" */	/* getform */
            {
              if (IIfsetio(mq_frame) != 0) {
                IIrf_param(qdef->mqdgform, qdef->mqdgargv,(int *)0);
              } /* IIfsetio */
            }
/* # line 948 "update.qsc" */	/* host code */
				_rowupd( qdef, mqUPDREC, mqIN_MASTER );
				/* bug 10823
				**	Only do an update if there was a detail row
				**	retrieved.
				*/
				if (output != QO_DDONE)
				{
					_rowupd( qdef, mqUPDREC, mqIN_DETAIL );
				}
			    }
			}
			if (output != QO_QDONE)
			{
			    if ((IIQG_generate(mqqry, QI_BREAK, &output)) != OK)
			    {
				FDerror(BADBREAK, 0, ERx(""));
				mqwrtfin();
				mqwrtdel();
/* # line 969 "update.qsc" */	/* clear */
            {
              IIclrflds();
            }
/* # line 970 "update.qsc" */	/* host code */
				rtncode = FAIL;
/* # line 971 "update.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 972 "update.qsc" */	/* host code */
			    }
			    QG_mqqry = NULL;
			}
			mqwrtfin();
			IIqfWrtUpd(qdef);
			mqwrtdel();
			anychanges = FALSE;
		}
		else
		{
brkqry:
/* # line 985 "update.qsc" */	/* message */
            {
              IImessage(ERget(S_QF00B8_NoUpdates));
            }
/* # line 986 "update.qsc" */	/* sleep */
            {
              IIsleep(2);
            }
/* # line 987 "update.qsc" */	/* host code */
			if ((IIQG_generate(mqqry, QI_BREAK, &output)) != OK)
			{
				FDerror(BADBREAK, 0, ERx(""));
			}
			QG_mqqry = NULL;
			mqwrtfin();
			mqwrtdel();
		}
/* # line 995 "update.qsc" */	/* clear */
            {
              IIclrflds();
            }
/* # line 996 "update.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 7) {
          {
/* # line 1003 "update.qsc" */	/* host code */
		IIUFfieldHelp();
          }
        } else if (IIretval() == 8) {
          {
/* # line 1009 "update.qsc" */	/* host code */
		_updhlp();
          }
        } else if (IIretval() == 9) {
          {
/* # line 1014 "update.qsc" */	/* host code */
		if ( mq_func == mqBROWSE )
		{
			goto endit;
		}
		else
		{
			FTbell();	/* quit key not allowed */
		}
          }
        } else if (IIretval() == 10) {
          {
            i4 fchange;
/* # line 1028 "update.qsc" */	/* host code */
	endit:
/* # line 1029 "update.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&fchange,20,mq_frame,0);
              } /* IIiqset */
            }
/* # line 1030 "update.qsc" */	/* host code */
		if ( fchange || anychanges )
		{
			anychanges = TRUE;
			if ( !IIUFver(ERget(S_QF00AC_DoNotUpdateQ), 0) )
			{
/* # line 1035 "update.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1036 "update.qsc" */	/* host code */
			}
		}
/* # line 1038 "update.qsc" */	/* message */
            {
              IImessage(ERget(S_QF00BA_EndingUpdate));
            }
/* # line 1039 "update.qsc" */	/* host code */
		if ((IIQG_generate(mqqry, QI_BREAK, &output)) != OK)
			FDerror(BADBREAK, 0, ERx(""));
		QG_mqqry = NULL;
		mqwrtfin();
		mqwrtdel();
/* # line 1045 "update.qsc" */	/* clear */
            {
              IIclrflds();
            }
/* # line 1046 "update.qsc" */	/* host code */
		rtncode = FAIL;
/* # line 1047 "update.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 11) {
          {
            i4 botrow;
/* # line 1054 "update.qsc" */	/* host code */
		if ( output == QO_QDONE && (qdef_type == 0 || !tblfield ) )
		{
/* # line 1056 "update.qsc" */	/* message */
            {
              IImessage(ERget(S_QF0079_No_more_rows_in_query));
            }
/* # line 1057 "update.qsc" */	/* sleep */
            {
              IIsleep(3);
            }
/* # line 1058 "update.qsc" */	/* clear */
            {
              IIfldclear(mq_tbl);
            }
/* # line 1059 "update.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1060 "update.qsc" */	/* host code */
		}
/* # line 1061 "update.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 1062 "update.qsc" */	/* getrow */
            {
              if (IItbsetio(2,mq_frame,mq_tbl,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&botrow,(char *)"_RECORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 1063 "update.qsc" */	/* host code */
		if  ((botrow == numrows) &&
		     ((output == QO_QDONE) || (output == QO_DDONE)) &&
		     (!IIQFsddSystemUserLkeyDD) &&
		     (!IIQFsmmSystemUserLkeyMM) &&
		     (qdef->mqappendok))
		{
/* # line 1069 "update.qsc" */	/* insertrow */
            {
              if (IItbsetio(5,mq_frame,mq_tbl,-2) != 0) {
                if (IItinsert() != 0) {
                  IItcoputio((char *)"iinewrow",(short *)0,1,30,sizeof(1),
                  (void *)IILQint(1));
                  IITBceColEnd();
                } /* IItinsert */
              } /* IItbsetio */
            }
/* # line 1070 "update.qsc" */	/* host code */
			++numrows;
/* # line 1071 "update.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1072 "update.qsc" */	/* host code */
		}
		if (botrow >= numrows)
		{
/* # line 1075 "update.qsc" */	/* message */
            {
              IImessage(ERget(S_QF0022_RetrieveMore));
            }
/* # line 1076 "update.qsc" */	/* host code */
			status = ( mqgetscroll(qdef, &output, &numrows) );
			if ( IIUIdeadlock(status) )
			{
				_stillupd(qdef);
/* # line 1080 "update.qsc" */	/* clear */
            {
              IIclrflds();
            }
/* # line 1081 "update.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 1082 "update.qsc" */	/* host code */
			}
		}
/* # line 1084 "update.qsc" */	/* scroll */
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
      if (IInmuact(ERget(FE_Query),(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Delete),(char *)0,2,2,3) == 0) goto IIfdE1;
      if (IInmuact(scrollitem,(char *)0,2,2,4) == 0) goto IIfdE1;
      if (IInmuact(additem,(char *)0,2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Save),(char *)0,2,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,2,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,7) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,7) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,8) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,8) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,2,2,9) == 0) goto IIfdE1;
      if (IInmuact(exititem,ERget(_EndExpl),2,2,10) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,10) == 0) goto IIfdE1;
      if (IIactscrl(mq_tbl,0,11) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 1088 "update.qsc" */	/* host code */
	_VOID_ IIUIendXaction();
	Mq_intransaction = FALSE;
	return rtncode;
  }
static bool	_chkchange();
static VOID
mmq_tblupd ( qdef )
register MQQDEF	*qdef;
  {
	register i4	numrels = qdef->mqnumtabs;
    i4 cnt_rows = 0;
/* # line 1103 "update.qsc" */	/* unloadtable */
    {
      if (IItbact(mq_frame,mq_tbl,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItrc_param(qdef->mqmgform, qdef->mqmgargv,(int *)0);
        IITBceColEnd();
        {
/* # line 1106 "update.qsc" */	/* host code */
		register RELINFO	*tabl;
		register i4		i;
		i4			first;
          i4 err;
		/* bug 9500
		**	check for error in the unloadtable loop
		*/
		Mq_inunloadtable = TRUE;
		cnt_rows += 1;
/* # line 1117 "update.qsc" */	/* inquire_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&err,1,(char *)0,0);
            } /* IIiqset */
          }
/* # line 1118 "update.qsc" */	/* host code */
		if ( err != OK )
		{
			IIUGerr(E_QF00BB_IgnoreUpdateWarn, UG_ERR_ERROR,
					1, (PTR)&cnt_rows
			);
			/* bug 20007
			** Don't set mqrstate to UNCHANGED because we may
			** try doing something with it when the data is not
			** all there (as we were doing with detail tablefield).
			*/
			mqrstate = UNDEF;	/* ignore change */
		}
		/* end bug fix for bug # 9500 */
		first = ( qdef_type != 0 ) ? 1 : 2;
		switch(mqrstate)
		{
		   case UNCHANGED:
			for ( i = 0 ; i < numrels ; ++i )
			{
				tabl = qdef->mqtabs[i];
				mqwrtrec(tabl, qdef->mqmrecl, mqNOPREC, first);
				if ( first != 0 )
					first = 0;
			}
			break;
		   case CHANGED:
			for ( i = 0 ; i < numrels ; ++i )
			{
				i4	action;
				tabl = qdef->mqtabs[i];
				if (mqrowstat)
				{
					action = mqADDREC;
				}
				else if ( _chkchange(qdef, tabl) )
				{
					action = mqUPDREC;
				}
				else
				{
					action = mqNOPREC;
				}
				mqwrtrec( tabl, qdef->mqmrecl, action, first );
				if ( first != 0 )
					first = 0;
			}
			break;
		   case DELETED:
			if (mqrowstat)	/* newly added row was deleted, ignore */
				break;
			for ( i = numrels - 1 ; i >= 0 ; --i )
			{
				tabl = qdef->mqtabs[i];
				mqwrtrec(tabl, qdef->mqmrecl,
					tabl->opdelete ? mqDELREC : mqNOPREC,
					first
				);
				if ( first != 0 )
					first = 0;
			}
			break;
		   case NEW:
		   case UNDEF:
			break;
		}
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 1190 "update.qsc" */	/* host code */
	Mq_inunloadtable  = FALSE;
  }
static bool
mqdmq_tblupd(qdef)
register MQQDEF	*qdef;
  {
	register i4	numrels = qdef->mqnumtabs;
	bool		any_chgdone = TRUE;
    i4 cnt_rows = 0;
/* # line 1201 "update.qsc" */	/* unloadtable */
    {
      if (IItbact(mq_frame,mq_tbl,0) == 0) goto IItbE2;
      while (IItunload() != 0) {
        IItrc_param(qdef->mqdgform, qdef->mqdgargv,(int *)0);
        IITBceColEnd();
        {
/* # line 1204 "update.qsc" */	/* host code */
		register RELINFO	*tabl;
		register i4		i;
		i4			first;
          i4 err;
		/* bug 9500
		**	check for error in the unloadtable loop
		*/
		Mq_inunloadtable = TRUE;
		cnt_rows += 1;
/* # line 1215 "update.qsc" */	/* inquire_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&err,1,(char *)0,0);
            } /* IIiqset */
          }
/* # line 1216 "update.qsc" */	/* host code */
		if ( err != OK )
		{
			IIUGerr(E_QF00BB_IgnoreUpdateWarn, UG_ERR_ERROR,
					1, (PTR)&cnt_rows
			);
			/* bug 20007
			** Don't set mqrstate to UNCHANGED because we may
			** try doing something with it when the data is not
			** all there (as we were doing with detail tablefield).
			*/
			mqrstate = UNDEF;	/* ignore change */
		}
		/* end bug fix for bug # 9500 */
		first = ( qdef_type != 0 ) ? 1 : 2;
		switch(mqrstate)
		{
		  case UNCHANGED:	/* Check anyway in case join field not
					   in table field has changed and is
					   updateable. */
		  case CHANGED:
			for ( i = 0 ; i < numrels ; ++i )
			{
				tabl = qdef->mqtabs[i];
				if ( tabl->mqtype == mqIN_DETAIL )
				{
					i4	action;
					if (mqrowstat)
					{
						action = (mqrstate == UNCHANGED)
				 			? mqNOPREC : mqADDREC;
						any_chgdone = FALSE;
					}
					else if ( _chkchange(qdef, tabl) )
					{
						action = mqUPDREC;
						any_chgdone = FALSE;
					}
					else
					{
						action = mqNOPREC;
					}
					mqwrtrec( tabl, qdef->mqdrecl, action,
						first
					);
					if ( first != 0 )
						first = 0;
				}
			} /* end for */
			break;
		   case DELETED:
			if (mqrowstat)	/* newly added row deleted, ignore */
				break;
			for ( i = numrels - 1 ; i >= 0 ; --i )
			{
				tabl = qdef->mqtabs[i];
				if ( tabl->mqtype == mqIN_DETAIL )
				{
					mqwrtrec(tabl, qdef->mqdrecl,
						tabl->opdelete ? mqDELREC
								: mqNOPREC,
						first
					);
					if ( first != 0 )
						first = 0;
				}
			} /* end for */
			break;
		   case NEW:
		   case UNDEF:
			break;
		}
        }
      } /* IItunload */
IItbE2:
      IItunend();
    }
/* # line 1293 "update.qsc" */	/* host code */
	Mq_inunloadtable = FALSE;
	return any_chgdone;
  }
static VOID
_rowupd ( qdef, status, type )
register MQQDEF	*qdef;
i4		status;
i4		type;
{
	register RELINFO	*tabl;
	register i4		i;
	register i4		skip_type;
	i4			first;
	i4			recl;
	if ( type == mqIN_MASTER )
	{
		first = 1;
		skip_type = mqIN_DETAIL;
		recl = qdef->mqmrecl;
	}
	else /* if ( type == mqIN_DETAIL ) */
	{
		first = 2;
		skip_type = mqIN_MASTER;
		recl = qdef->mqdrecl;
	}
	if ( status == mqDELREC )
	{
		for ( i = qdef->mqnumtabs - 1 ; i >= 0 ; --i )
		{
			tabl = qdef->mqtabs[i];
			if ( qdef_type == 0 && tabl->mqtype == skip_type )
				continue;
			mqwrtrec(tabl, recl,
				tabl->opdelete ? mqDELREC : mqNOPREC, first
			);
			if ( first != 0 )
				first = 0;
		}
	}
	else if (status == mqUPDREC)
	{
		for ( i = 0 ; i < qdef->mqnumtabs ; ++i )
		{
			tabl = qdef->mqtabs[i];
			if ( qdef_type == 0 && tabl->mqtype == skip_type )
				continue;
			mqwrtrec(tabl, recl, 
				_chkchange(qdef, tabl) ? mqUPDREC : mqNOPREC,
				first
			);
			if ( first != 0 )
				first = 0;
		}
	}
	else if ( status == mqADDREC || status == mqNOPREC )
	{
		for ( i = 0 ; i < qdef->mqnumtabs ; ++i )
		{
			tabl = qdef->mqtabs[i];
			if ( qdef_type == 0 && tabl->mqtype == skip_type )
				continue;
			mqwrtrec(tabl, recl, status, first);
			if ( first != 0 )
				first = 0;
		}
	}
}
/*
** Name:	_gnxtmas() - get next master record.
**	Common code for CTRL_F and Next.
**
**	Returns:
**		status = 0 (no changes made)
**			 1 (changes made)
**			 2 (error, code in qg_code)
**	history - prs 1/85.
*/
static i4
_gnxtmas ( qdef, qg_code, output, numrows )
MQQDEF	*qdef;
i4	*qg_code;
i4	*output;
i4	*numrows;
  {
    i4 fchange;
	i4	status;
	status = 0;
/* # line 1390 "update.qsc" */	/* inquire_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&fchange,20,mq_frame,0);
      } /* IIiqset */
    }
/* # line 1391 "update.qsc" */	/* host code */
	if (qdef_type == 0)
	{
/* # line 1393 "update.qsc" */	/* message */
    {
      IImessage(ERget(S_QF001E_Retrieving));
    }
/* # line 1394 "update.qsc" */	/* host code */
		if (fchange)
		{
		    status = 1;
/* # line 1397 "update.qsc" */	/* getform */
    {
      if (IIfsetio(mq_frame) != 0) {
        IIrf_param(qdef->mqmgform, qdef->mqmgargv,(int *)0);
      } /* IIfsetio */
    }
/* # line 1398 "update.qsc" */	/* host code */
		    if (tblfield)
		    {
			_rowupd( qdef, mqUPDREC, mqIN_MASTER );
			if ( !mqdmq_tblupd(qdef) &&
				(*qg_code = _updrest(qdef, output, mqUPDREC))
						== DLCKERR)
			{
			    return (status = -1);
			}
		    }
		    else
		    {
/* # line 1410 "update.qsc" */	/* getform */
    {
      if (IIfsetio(mq_frame) != 0) {
        IIrf_param(qdef->mqdgform, qdef->mqdgargv,(int *)0);
      } /* IIfsetio */
    }
/* # line 1411 "update.qsc" */	/* host code */
			_rowupd( qdef, mqUPDREC, mqIN_MASTER );
			/* bug 10823
			**	Only do an update if there was a detail
			**	row retrieved.
			*/
			if (*output != QO_DDONE)
			{
			    _rowupd( qdef, mqUPDREC, mqIN_DETAIL );
			}
		    }
		}
		if ((*qg_code = mqgetmaster(qdef, output, numrows)) != OK)
		{
		    if ( IIUIdeadlock(*qg_code) )
		    {
			status = -1;
			return (status);
		    }
		    if (*output == QO_QDONE)
		    {
		    /*	appends are meaningless at this point so */
		    /*	make form read only till next query	 */
/* # line 1436 "update.qsc" */	/* set_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(21,mq_frame,0,(short *)0,1,32,0,(char *)"r");
      } /* IIiqset */
    }
/* # line 1437 "update.qsc" */	/* clear */
    {
      IIclrflds();
    }
/* # line 1438 "update.qsc" */	/* host code */
		    }
		}
		if  ((tblfield) && (*numrows == 0) &&
		     (!IIQFsddSystemUserLkeyDD) &&
		     (qdef->mqappendok))
		{
/* # line 1444 "update.qsc" */	/* insertrow */
    {
      if (IItbsetio(5,mq_frame,mq_tbl,0) != 0) {
        if (IItinsert() != 0) {
          IItcoputio((char *)"iinewrow",(short *)0,1,30,sizeof(1),
          (void *)IILQint(1));
          IITBceColEnd();
        } /* IItinsert */
      } /* IItbsetio */
    }
/* # line 1445 "update.qsc" */	/* host code */
		    *numrows = 1;
		}
/* # line 1447 "update.qsc" */	/* set_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(20,mq_frame,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 1448 "update.qsc" */	/* host code */
	}
	else
	{
		if (fchange)
		{
		    status = 1;
		    if (!tblfield)
		    {
/* # line 1456 "update.qsc" */	/* getform */
    {
      if (IIfsetio(mq_frame) != 0) {
        IIrf_param(qdef->mqmgform, qdef->mqmgargv,(int *)0);
      } /* IIfsetio */
    }
/* # line 1457 "update.qsc" */	/* host code */
			_rowupd( qdef, mqUPDREC, mqIN_MASTER );
		    }
		}
		if ((*qg_code = mqgetrecord(qdef, output)) != OK)
		{
		    if (IIUIdeadlock(*qg_code))
		    {
			status = -1;
			return(status);
		    }
		    if (*output == QO_QDONE)
		    {
		    /*	appends are meaningless at this point so */
		    /*	make form read only till next query	 */
/* # line 1471 "update.qsc" */	/* set_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(21,mq_frame,0,(short *)0,1,32,0,(char *)"r");
      } /* IIiqset */
    }
/* # line 1472 "update.qsc" */	/* clear */
    {
      IIclrflds();
    }
/* # line 1473 "update.qsc" */	/* host code */
		    }
		}
/* # line 1475 "update.qsc" */	/* set_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(20,mq_frame,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 1476 "update.qsc" */	/* host code */
	}
	return (status);
  }
static VOID
_deltbl ( numrows )
i4	numrows;
  {
    char table_name[FE_MAXNAME + 1];
	register i4	drow;
/* # line 1487 "update.qsc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,mq_frame,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,table_name,19,(char *)0,0);
      } /* IIiqset */
    }
/* # line 1488 "update.qsc" */	/* host code */
	if (STequal(mq_tbl, table_name))
	{
		for ( drow = 0 ; drow < numrows ; ++drow )
		{
/* # line 1492 "update.qsc" */	/* deleterow */
    {
      if (IItbsetio(4,mq_frame,mq_tbl,-2) != 0) {
        if (IItdelrow(0) != 0) {
        } /* IItdelrow */
      } /* IItbsetio */
    }
/* # line 1493 "update.qsc" */	/* host code */
		}
	}
	else
	{
		for ( drow = 0 ; drow < numrows ; ++drow )
		{
/* # line 1499 "update.qsc" */	/* deleterow */
    {
      if (IItbsetio(4,mq_frame,mq_tbl,1) != 0) {
        if (IItdelrow(0) != 0) {
        } /* IItdelrow */
      } /* IItbsetio */
    }
/* # line 1500 "update.qsc" */	/* host code */
		}
	}
  }
static STATUS
_updrest ( qdef, output, rstat )
register MQQDEF	*qdef;
register i4	*output;
i4		rstat;
  {
	register QDESC	*mqqry;
	register STATUS	done;
	STATUS	rcode;
	mqqry = qdef->mqqdesc;
	rcode = OK;
	done = ( qdef_type == 0 ) ? QO_DDONE : QO_QDONE;
	while ( *output != done &&
			(rcode = IIQG_generate(mqqry, QI_GET, output)) == OK )
	{
		if ( qdef_type == 0 )
		{ /* Detail */
			IIQFcdb_CopyDBDVs( qdef, mqIN_DETAIL );
			if ( rstat == mqUPDREC )
    {
      i4 curr_row;
/* # line 1528 "update.qsc" */	/* loadtable */
      {
        if (IItbact(mq_frame,mq_tbl,1) != 0) {
          IItsc_param(qdef->mqdpform, qdef->mqdpargv,(int *)0);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 1531 "update.qsc" */	/* scroll */
      {
        if (IItbsetio(1,mq_frame,mq_tbl,-3) != 0) {
          IItbsmode((char *)"to");
          if (IItscroll(0,-1) != 0) {
          } /* IItscroll */
        } /* IItbsetio */
      }
/* # line 1532 "update.qsc" */	/* inquire_frs */
      {
        if (IIiqset(3,0,mq_frame,(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&curr_row,29,mq_tbl,0);
        } /* IIiqset */
      }
/* # line 1535 "update.qsc" */	/* getrow */
      {
        if (IItbsetio(2,mq_frame,mq_tbl,curr_row) != 0) {
          IItrc_param(qdef->mqdgform, qdef->mqdgargv,(int *)0);
          IITBceColEnd();
        } /* IItbsetio */
      }
/* # line 1538 "update.qsc" */	/* host code */
#ifdef DEBUG
	if (mq_debug)
	{
		SIfprintf(mqoutf, ERx("MQUPDREST: update row %d\n"), curr_row);
	}
#endif
    }
		    _rowupd( qdef, rstat, mqIN_DETAIL );
		}
		else
		{ /* Master */
		    IIQFcdb_CopyDBDVs( qdef, mqIN_MASTER );
		    _rowupd( qdef, rstat, mqIN_MASTER );
		}
	} /* end while */
	if ( IIUIdeadlock(rcode) )
	{
		rcode = DLCKERR;
		_stillupd(qdef);
/* # line 1558 "update.qsc" */	/* clear */
    {
      IIclrflds();
    }
/* # line 1559 "update.qsc" */	/* host code */
	}
	return rcode;
  }
static bool
_chkchange ( qdef, tabl )
register MQQDEF		*qdef;
register RELINFO	*tabl;
{
	register i4	i;
	bool		tblchanged = FALSE;
	ADF_CB	*FEadfcb();
	for ( i = 0 ; i < qdef->mqnumatts ; ++i )
	{
		register ATTRIBINFO	*catt = qdef->mqatts[i];
		if ( catt->formfield[0] != EOS && catt->lkeyinfo == 0 &&
				STequal(tabl->mqrangevar, catt->rvar) )
  {
    char *name = catt->formfield;
    i4 colchanged;
			if ( catt->intbl == mqIN_TABLE  &&
					( catt->jpart != mqHIDDEN_JOIN ||
						catt->mdtype != mqIN_MASTER ||
						qdef_type != mqMD_JOIN )
					)
			{ /* table field */
/* # line 1590 "update.qsc" */	/* inquire_frs */
    {
      if (IIiqset(5,0,mq_frame,mq_tbl,(char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&colchanged,20,name,0);
      } /* IIiqset */
    }
/* # line 1593 "update.qsc" */	/* host code */
			}
			else
			{ /* simple field */
/* # line 1596 "update.qsc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,mq_frame,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&colchanged,20,name,0);
      } /* IIiqset */
    }
/* # line 1599 "update.qsc" */	/* host code */
			}
			/* What follows checks if a non-updateable column
			** of the table field was changed, since they are
			** not query-only so that new rows may be added.
			*/
			if ( colchanged )
			{
				if (catt->opupdate)
				{
					tblchanged = TRUE;
				}
				else if (catt->jpart == mqJOIN_FIELD)
				{
					if (catt->intbl)
					{
						IIUGerr(E_QF00BC_NoColUpdWarn,
							UG_ERR_ERROR, 2,
							(PTR)catt->formfield,
							(PTR)&mq_record
						);
					}
					else
					{
						IIUGerr(E_QF0153_NoFldUpdWarn,
							UG_ERR_ERROR, 2,
							(PTR)catt->formfield,
							(PTR)catt->rvar
						);
					}
				}
			}
			if ( tblchanged && !catt->intbl )
				break;
  }
	}
	return tblchanged;
}
/*
** Name:	_qrydone() - check if current query is done yet and print
**		  message if so.
*/
static bool
_qrydone ( output, msg )
i4	output;
bool	msg;
{
	if (output == QO_QDONE)
	{
		if (msg)
		{
/* # line 1652 "update.qsc" */	/* message */
  {
    IImessage(ERget(S_QF0079_No_more_rows_in_query));
  }
/* # line 1653 "update.qsc" */	/* sleep */
  {
    IIsleep(3);
  }
/* # line 1654 "update.qsc" */	/* host code */
		}
	/*	appends are meaningless at this point so */
	/*	make form read only till next query	 */
/* # line 1657 "update.qsc" */	/* set_frs */
  {
    if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(21,mq_frame,0,(short *)0,1,32,0,(char *)"r");
    } /* IIiqset */
  }
/* # line 1658 "update.qsc" */	/* host code */
		return TRUE;
	}
	else
		return FALSE;
}
/*
** Name:	_stillupd() -	routine called after a deadlock/abort occurs
**		 while browsing data in update mode. User is
**		 asked if updates done so far should still be
**		 written.  If so, then the file of updates is
**		 processed  (ie - changes are written to INGRES)
*/
static VOID
_stillupd ( qdef )
MQQDEF	*qdef;
  {
	bool resp;
	/*
	** since the transaction was just aborted, need to
	** start a new one.
	*/
	Mq_intransaction = TRUE;
	resp = IIUFver(ERget(S_QF00BD_SaveThusFarQ), 0);
	mqwrtfin();
	if (resp)
	{
		/*
		** process updates that have been stored
		** in temp file so far.
		*/
		IIqfWrtUpd(qdef);
	}
	mqwrtdel();
  }
/*
**	MQUPDHLP - help for update screens.
**
**		history - prs - 1/85.
**			7-aug-1987 (danielt) removed static char array
**			(for message extraction)
*/
static VOID
_updhlp()
{
	char	*help_file;
	if (qdef_type == 0)
	{
		if (tblfield)
		{
			help_file = (mq_func == mqUPDATE)
				? ERx("qupdmdtx.hlp") : ERx("qupdmdtn.hlp");
		}
		else
		{
			help_file = (mq_func == mqUPDATE)
				? ERx("qupdmdox.hlp") : ERx("qupdmdon.hlp");
		}
	}
	else
	{
		if (tblfield)
		{
			help_file = (mq_func == mqUPDATE)
				? ERx("qupdootx.hlp")  : ERx("qupdootn.hlp");
		}
		else
		{
			help_file = (mq_func == mqUPDATE)
				? ERx("qupdooox.hlp") : ERx("qupdooon.hlp");
		}
	}
	FEhelp( help_file, ERget(S_QF00BE_QBF_Update_Function) );
}
