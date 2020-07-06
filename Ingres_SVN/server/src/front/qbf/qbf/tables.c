# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
NO_OPTIM = i64_hpu
*/
#include	<compat.h>
#include	<cv.h>		/* 6-x_PC_80x86 */
#include	<st.h>
#include	<cm.h>
#ifdef DEBUG
#include	<tm.h>
#include	<si.h>
#endif
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
#include	<uf.h>
#include	<ui.h>
#include	<stdprmpt.h>
# include "mqbf.h"
# include "mqsymbol.h"
#include	<erug.h>
#include	<uigdata.h>
#include	"erqf.h"
/**
** Name:	tables.qc -	Table Entry routine for MQBF.
**
**	This module brings up the Table Entry Form in MQBF.
**	The user then fills in the table field on the form
**	with table names from the database and table types
**	for each table.
**
**	Related modules:
**		"getinfo.qsc"
**		"getnames.qsc"
**		"getattr.qsc"
**
** History:
**	Revision 6.5
**		18-dec-92 (sylviap)
**			Disable GO menuitem if called with the -N flag.  This
**			internal flag is used by RBF/VIFRED.  We want to create
**			joindefs from RBF/VIFRED, but don't want users to 
**			add/modify data.  -N is exactly like -J except GO is 
**			disabled.
**
**	Revision 6.4
**		08-mar-90 (kenl)
**			Added listpick for 'Role' when creating a JoinDef
**			and added call to IITUtpTblPik() to provide a listpick
**			of all tables.
**		7/2/90 (martym)
**			Added 'IIQFSaveJD()', and changed to use 
**			'IIUFccConfirmChoice()' on quits and ends.
**		7/5/90 (martym)
**			Fixed bug #31247, so rules updates are saved.
**		09/12/90 (dkh)
**			Fixed problem with ListChoices working incorrectly
**			if cursor is not on the table field in the form.
**		10/13/90 (emerson)
**			Fix to mq_tables (see its history).
**		10/13/90 (emerson)
**			Fix to IIQFSaveJD (bugs 31956 and 31871).
**		10/13/90 (emerson)
**			Fixes to mq_tables and IIQFSaveJD for bug 33914.
**		15-aug-91 (pearl)
**			Fix to mq_tables for bug 35021.
**			Before call to reallyEnd(), set save_prompt to true
**			if mq_editjoin is set (called from ABF with -J flag)
**			and if mqcr.id is -1 (not preexisting joindef).  That
**			way the user will be prompted to save the form even if
**			it has been created from ABF as a duplicate joindef
**			with no changes made.
**		29-apr-92 (pearl)
**			Make port specific change for nc5_us5.
**		27-jul-92 (thomasm)
**			Hp/8 optimizer dislikes this file.
**      	27-jan-93 (pghosh)
**          		Modified nc5_us5 to nc4_us5. Till date nc5_us5 & 
**			nc4_us5 were being used for the same port. This is 
**			to clean up the mess so that later on no confusion 
**			is created.
**      	15-jun-95 (popri01)
**          		Removed nc4_us5 ifdef as it is no longer necessary
**			and caused the following compiler error:
**		19-jan-1999 (muhpa01)
**			Removed NO_OPTIM for hp8_us5.
**    Symbol declaration is inconsistent with a previous declaration
**    at "/ingres/6504/src/front/hdr/hdr/oocat.h",L70/C23.
**    Type of previous name is `const char[]'; that of new name is `char[]'.
**
**	Revision 6.1  88/09  wong
**		03-nov-88 (kenl)
**			Changed hastables() routine.  It was returning a
**			bad indication if the UNLOADTABLE failed due to
**			validation, etc.
**		09-nov-88 (sylviap)
**			Added a NULL parameter when calling OOsaveSequence.
**		20-dec-88 (kenl)	bug 4062
**			QBF will no longer let you exit without a warning
**			if you have modified a bad JoinDef.
**		13-mar-89 (kenl)	bug 3972, 4061
**			No longer allows a user to save a JoinDef in which
**			no tables are joined.
**		05-dec-89 (kenl)
**			Removed parameter lastsaved.  We now reference a 
**			global, mq_lastsaved.  This change was required by
**			the new calling sequences needed for the -J flag.
**			Also add code to support the -J flag.
**
**	Revision 6.0
**		11-sep-87 (kenl)
**			added extern of reallyEnd.
**		07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header files: mqbf.qsh, mqsymbol.qsh, and
**			oosymbol.qsh.  Added SQL transaction code surrounding
**			the save of the JoinDef to insure that either all or
**			none of the JoinDef save takes place.
**		10-dec-87 (bruceb)
**			Changed buffer size from DB_MAXNAME to FE_MAXNAME.
**		11-feb-1988 (peter)	bug 2016.
**			Add the lastsaved parameter to mq_tables. 
**		12-feb-1988 (peter)	bug 2012.
**			On Save, if form is changed, recalculate the
**			data structures.
**		22-apr-88 (kenl)
**			Made changes to fix numerous JoinDef bugs.
**		19-aug-88 (kenl)
**			Changed save_prompt declaration to a GLOBALREF.
**
**	Revision 5.0
**		10/20/86 (KY)  -- Changed CH.h to CM.h.
**		8/13/86 (danielt)
**			changed FEmsg() calls to IIUGerr() for error messages.
**		16-jun-87 (marian)	bug 10298
**			Move calls to mq_chkchange() and mq_sw_change()
**			after call to  IIqfGetInfo(). This allows IIqfGetInfo()
**			to determine which tables are masters and which are 
**			details before calling mqtblyes() in mq_chkchange().
**		17-jun-87 (marian)	bug 8402
**			Always call mqtblyes() when tablefield is 'yes' and
**			mqchgintbl() when tablefield is 'no'.  This takes
**			care of the problem of resetting the tablefield
**			indicator for each attribute when a m/d tablefield
**			is changed to a m/m tablefield.
**		07-mar-95 (harpa06) Bug #67269
**			Display default abbreviations for tables that belong to
**			the user if they are part of the JoinDef definition. 
**			This also takes care of the problem of using a table the
**			user has deleted from the JoinDef definition in a new 
**			join operation.
**
**			The owner of a table in JoinDef definition is not dis-
**			played if the owner of the table is the current user of
**			QBF
**
**	Revision 2.0  83/03/01  nadene
**	Initial revision.
**	21-jan-03 (hayke02)
**	    Remove local prototyping of IIUFccConfirmChoice() - this is now
**	    done in fe.h. Also, make prototying of IIQFSaveJD() 'null' for
**	    rs4_us5.
*/
# define _qfrole_prmpt  ERget(F_QF0029_qfrole_prmpt)
# define _qfrole_opt    ERget(F_QF0030_qfrole_opt)
# define MAX_ROLE_SIZE 10
#define     QFS_RES_COLUMN	    1
#define     QFS_RES_RESFIELD        2
#define     QFS_RES_ONLY            3
#define     QFS_RES_FIELD           4
#define     QFS_RETURN_OK           5
GLOBALREF i4	save_prompt; 
GLOBALREF OOID	mq_joinid; 
GLOBALCONSTREF        char    _iiOOempty[];
FUNC_EXTERN	char	*IITUtpTblPik();
VOID	mdtablefield();
# ifndef rs4_us5
i4  IIQFSaveJD( MQQDEF *qdef, bool NotShowSubMenue, i4 *changed, char *currname, char *res_field, bool force_rebuild);
# else
i4  IIQFSaveJD();
# endif /* rs4_us5 */
static i4 reallyEnd ( i4 changed, i4 type, char *help_subject, char *help_file );
static STATUS	_master_check();
static STATUS	qfsvdisp();
STATUS OOsaveSequence( OOID *id, char *savename, char *catform,
	char *cattabl, char *saveform, char *title,
	PTR ptr, i4 (*display)(), i4 (*save)(),
	MENUPARAM *menup, STATUS (*setup)(), char *help_scr,
	bool bypass);
static bool
hastables()
{
  char tname[FE_MAXNAME + 1];
	bool	rtnvalue = TRUE;
/* # line 212 "tables.qsc" */	/* unloadtable */
  {
    if (IItbact(_mqbftbls,_tables,0) == 0) goto IItbE1;
    while (IItunload() != 0) {
      IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,tname,_table);
      IITBceColEnd();
      {
/* # line 214 "tables.qsc" */	/* host code */
		/*
		**  rtnvalue is set to TRUE above in case the UNLOADTABLE
		**  fails.  Thus we will return a value that indicates that
		**  something is on the form.  Here we reset rtnvalue to FALSE
		**  so that the return value will be false if no tables are
		**  found in the table field.
		*/
		rtnvalue = FALSE;
		if (STtrmwhite(tname) > 0)
		{
			rtnvalue = TRUE;
/* # line 225 "tables.qsc" */	/* endloop */
        {
          if (1) goto IItbE1;
        }
/* # line 226 "tables.qsc" */	/* host code */
		}
      }
    } /* IItunload */
IItbE1:
    IItunend();
  }
/* # line 229 "tables.qsc" */	/* host code */
	return rtnvalue;
}
VOID
tbl_reload()
{
	/*
	**	This is a dummy routine for the overlay linker
	**	to reload the tables routines after completing
	**	the query execution phase.
	*/
}
STATUS	mqwritetodb();
/*{
** Name:	mq_tables() -	Edit/create/run joindefs.
**
** Inputs:
**	qdef	{MQQDEF *}  QDEF structure.
**	is_old	{bool}  TRUE if this joindef exists.
**
** Outputs:
**
** History:
**	11-feb-1988 (peter)
**		Added the lastsaved parameter.
**	12-feb-1988 (peter)
**		Bug 2012.  On Save, see if things are changed on the
**		forms.  If so, recalculate the strucutures.
**	28-aug-1990 (Joe)
**		Changed references to IIUIgdata to the UIDBDATA structure
**		returned from IIUIdbdata().
**	10/13/90 (emerson)
**		When 'Joins' is selected and a data-type incompatibility
**		is detected before calling mq_joins (to display
**		the Join Specification screen), don't allow normal 
**		processing to proceed if the user canceled out of
**		the Join Specification screen.  This involves changing
**		the way mq_joins and IIqfGetInfo are called. (bug 33914).
*/
#define	_HelpExpl		F_FE0100_ExplFrameHelp
#define	_EndExpl		F_FE0102_ExplEnd
VOID
mq_tables(qdef, is_old)
MQQDEF		*qdef;
bool		is_old;
  {
	ER_MSGID	yorn;
	bool		another;
    i4 chgd;
    i4 chgd2;
    i4 changed = 0;
    char res_field[FE_MAXNAME+1];
    char currname[FE_MAXNAME + 1];
	i4		choice;
	char		*help_subject;
	i4 		stat;
	bool		unfixed_errors = FALSE;
	do
	{
		another = FALSE;
		qdef_type = -1;
#ifdef DEBUG
	if (mq_debug)
	{
		SIfprintf(mqoutf,
			ERx("MQTABLES: at start of routine, msecs=%d\n"),
			TMcpu()
		);
	}
#endif
/* # line 306 "tables.qsc" */	/* inittable */
    {
      if (IItbinit(_mqbftbls,_tables,(char *)"f") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 308 "tables.qsc" */	/* display */
    {
      if (IIdispfrm(_mqbftbls,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 311 "tables.qsc" */	/* host code */
			if (is_old)
			{
				STcopy(qdef->mqcr.name, currname);
/* # line 314 "tables.qsc" */	/* message */
        {
          IImessage(ERget(S_QF0095_RetrievingJoinDef));
        }
/* # line 316 "tables.qsc" */	/* host code */
				/* load everything about the JoinDef from OO */
				if (mq_name)
				{
				    OOcatRecord(qdef->mqcr.id, &qdef->mqcr);
				}
				if ( !mqgetdefined(qdef) )
				{
					if ( mq_forms || mq_qflag )
					{
						/* only reinit if not editing*/
						if (!mq_editjoin)
						{
							mq_reinit(0, qdef);
						}
/* # line 330 "tables.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 331 "tables.qsc" */	/* host code */
					}
					qdef_type = -1;
					IIUGerr(E_QF0096_JoinDefFetchErr, 0, 0);
				}
				yorn = tblfield ? F_UG0002_Yes2 : F_UG0007_No2;
			}
			else if (onetbl)
			{
				qdef->mqnumjoins = 0;
				if (!mq_cfrm)
				{
					if ( (tblfield = mq_tflag) )
					{
						STcopy(ERx("mqbftbl"), mq_tbl);
						yorn = F_UG0002_Yes2;
					}
					else
					{
						mq_tbl[0] = EOS;
						yorn = F_UG0007_No2;
					}
				}
#ifdef DEBUG
	if (mq_debug)
	{
		SIfprintf(mqoutf,
			ERx("MQTABLES: before IIqfBldInfo, msecs=%d\n"),
			TMcpu()
		);
	}
#endif
				if ( IIqfBldInfo(qdef) == OK )
				{ /* successful */
					STcopy(qdef->mqcr.name, currname);
				}
				else
				{ /* table doesn't exist */
					mq_reinit(0, qdef);
/* # line 371 "tables.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 372 "tables.qsc" */	/* host code */
				}
			}
			else if (mq_editjoin)
			{
				yorn = F_UG0002_Yes2;
			}
#ifdef DEBUG
	if (mq_debug)
	{
		SIfprintf(mqoutf,
			ERx("MQTABLES: after IIqfBldInfo, msecs=%d\n"),
			TMcpu()
		);
	}
#endif
			if ( is_old || onetbl || mq_editjoin)
        {
          char yesno[15+1];
				/*
				** If editing a JoinDef we have several
				** bookkeeping things to take care of:
				**  1. Initialize main catalog for OO save
				**  2. Copy the JoinDef name user desires
				**     (as opposed to one we may have just
				**     copied from) to form/internal field
				**  3. Set up new owner
				**  4. Set field on form to dislayonly
				*/
				if (mq_editjoin)
				{
					OOcatEmpty(OC_JOINDEF, _iicatalog,
						_cattable);
					STcopy(mq_joindef, qdef->mqcr.name);
					qdef->mqcr.id = mq_joinid;
					STcopy(qdef->mqcr.name, currname);
					STcopy(IIUIdbdata()->user, qdef->mqcr.owner);
/* # line 409 "tables.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,_mqbftbls,(char *)0, (char *)0) != 0) {
              IIstfsio(53,_name,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 411 "tables.qsc" */	/* host code */
				}
				if (is_old || onetbl)
				{
					mdtablefield(qdef, _mqbftbls, _tables);
				}
				STlcopy( ERget(yorn), yesno, sizeof(yesno)-1 );
				CVupper( yesno );
/* # line 418 "tables.qsc" */	/* putform */
          {
            if (IIfsetio(_mqbftbls) != 0) {
              IIputfldio((char *)"name",(short *)0,1,32,0,currname);
              IIputfldio((char *)"tablefield",(short *)0,1,32,0,yesno);
            } /* IIfsetio */
          }
/* # line 423 "tables.qsc" */	/* host code */
				if ((mq_name || mq_going) && !mq_editjoin)
				{
					goto qrybld;
				}
        }
                	/*
                	** Disable/enable 'Go' menuitem.  Disable if QBF is 
			** called w/-N flag (creating/editing a joindef).  We 
			** do not want user to be able to add/modify data.
			** RBF and VIFRED use the -N flag.
                	*/
                	if (mq_nogo)
/* # line 435 "tables.qsc" */	/* set_frs */
        {
          if (IIiqset(7,0,_mqbftbls,(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(FE_Go),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 437 "tables.qsc" */	/* host code */
                	else
/* # line 438 "tables.qsc" */	/* set_frs */
        {
          if (IIiqset(7,0,_mqbftbls,(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(FE_Go),0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 445 "tables.qsc" */	/* host code */
			if ( _master_check() == FAIL )
			{
/* # line 447 "tables.qsc" */	/* resume */
            {
              IIrescol(_tables,_role);
              if (1) goto IIfdB1;
            }
/* # line 448 "tables.qsc" */	/* host code */
			}
/* # line 449 "tables.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 454 "tables.qsc" */	/* host code */
			STATUS	rcode;
			i4	tblwidth;
			if ( _master_check() == FAIL )
			{
/* # line 459 "tables.qsc" */	/* resume */
            {
              IIrescol(_tables,_role);
              if (1) goto IIfdB1;
            }
/* # line 460 "tables.qsc" */	/* host code */
			}
/* # line 462 "tables.qsc" */	/* message */
            {
              IImessage(ERget(S_QF0098_Executing));
            }
/* # line 464 "tables.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chgd,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 465 "tables.qsc" */	/* host code */
			changed |= chgd;
			if (changed != 0)
			{ /* Changes have been made */
				save_prompt |= changed;
/* # line 471 "tables.qsc" */	/* validate */
            {
              if (IIvalfld((char *)"tablefield") == 0) {
                goto IIfdB1;
              } /* IIvalfld */
            }
/* # line 472 "tables.qsc" */	/* host code */
				if ( IIqfGetInfo(qdef, res_field, FALSE,
					unfixed_errors ) != OK )
/* # line 474 "tables.qsc" */	/* resume */
            {
              IIresfld(res_field);
              if (1) goto IIfdB1;
            }
/* # line 476 "tables.qsc" */	/* host code */
				changed = 0;
/* # line 477 "tables.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 478 "tables.qsc" */	/* host code */
			}
qrybld:
#ifdef DEBUG
	if (mq_debug)
	{
		SIfprintf(mqoutf,
			ERx("MQTABLES: at start of qrybld, msecs=%d\n"),
			TMcpu()
		);
	}
#endif
			if (qdef->mqnumjoins == 0 && qdef->mqnumtabs > 1)
			{
				IIUGerr(E_QF0099_NoJoinFlds, UG_ERR_ERROR, 0);
/* # line 492 "tables.qsc" */	/* resume */
            {
              IIresfld((char *)"name");
              if (1) goto IIfdB1;
            }
/* # line 493 "tables.qsc" */	/* host code */
				/*NOTREACHED*/
			}
			else if (qdef->mqnumjoins < qdef->mqnumtabs-1)
			{
				IIUGerr(E_QF009A_NotAllJoined, UG_ERR_ERROR, 0);
/* # line 498 "tables.qsc" */	/* resume */
            {
              IIresfld((char *)"name");
              if (1) goto IIfdB1;
            }
/* # line 499 "tables.qsc" */	/* host code */
				/*NOTREACHED*/
			}
			else if ( !mqjmdchk(qdef) )
			{
/* # line 503 "tables.qsc" */	/* resume */
            {
              IIresfld((char *)"name");
              if (1) goto IIfdB1;
            }
/* # line 504 "tables.qsc" */	/* host code */
				/*NOTREACHED*/
			}
			rcode = mqbldquery(qdef, &tblwidth);
#ifdef DEBUG
	if (mq_debug)
	{
		SIfprintf(mqoutf,
			ERx("MQTABLES: after mqbldquery, msecs=%d\n"),
			TMcpu()
		);
	}
#endif
			if (rcode == 0)
			{
/* # line 518 "tables.qsc" */	/* resume */
            {
              IIresfld((char *)"name");
              if (1) goto IIfdB1;
            }
/* # line 519 "tables.qsc" */	/* host code */
				/*NOTREACHED*/
			}
			if (rcode < 0)
			{
				mq_reinit(0, qdef); /* reinitialize */
/* # line 524 "tables.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 525 "tables.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 526 "tables.qsc" */	/* host code */
				/*NOTREACHED*/
			}
			mq_go(qdef, tblwidth);
			/* bug 4062 */
			if (( !mq_going && Qcode == 0 ) || (save_prompt != 0) )
			{
/* # line 533 "tables.qsc" */	/* resume */
            {
              IIresfld((char *)"name");
              if (1) goto IIfdB1;
            }
/* # line 534 "tables.qsc" */	/* host code */
				/*NOTREACHED*/
			}
			if ( mq_going || Qcode == 1 )
			{
				Qcode = 1;
				mq_reinit(0, qdef);
			}
			else
			{
				char qname[FE_MAXNAME + 1];
				OOID	qid;
				/*
				** a NewQueryTarget has been selected, whose
				** name is contained in the qdef structure.
				*/
				is_old = !onetbl;
				another = TRUE;
				/* don't reinit globals */
				/* save the qdef name and id */
				STcopy(qdef->mqcr.name, qname);
				qid = qdef->mqcr.id;
				mq_free(qdef);
				STcopy(qname, qdef->mqcr.name);
				qdef->mqcr.id = qid;
			}
/* # line 560 "tables.qsc" */	/* clear */
            {
              IIclrflds();
            }
/* # line 561 "tables.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 3) {
          {
            char yes[15+1];
/* # line 568 "tables.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chgd,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 569 "tables.qsc" */	/* host code */
			changed |= chgd;
			save_prompt |= changed;
			if ( save_prompt && hastables() &&
				!IIUFver(ERget(S_QF009B_NotSavedBlankQ), 0) )
			{
/* # line 574 "tables.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 575 "tables.qsc" */	/* host code */
			}
			changed = 0;
			save_prompt = 0;
			mq_reinit(0, qdef);	/* reinitialize */
/* # line 579 "tables.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 580 "tables.qsc" */	/* clear */
            {
              IIclrflds();
            }
/* # line 582 "tables.qsc" */	/* host code */
			STlcopy( ERget(F_UG0002_Yes2), yes, sizeof(yes)-1 );
			CVupper( yes );
/* # line 584 "tables.qsc" */	/* putform */
            {
              if (IIfsetio(_mqbftbls) != 0) {
                IIputfldio((char *)"tablefield",(short *)0,1,32,0,yes);
              } /* IIfsetio */
            }
/* # line 585 "tables.qsc" */	/* resume */
            {
              IIresfld((char *)"name");
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 590 "tables.qsc" */	/* host code */
			if ( _master_check() == FAIL )
			{
/* # line 592 "tables.qsc" */	/* resume */
            {
              IIrescol(_tables,_role);
              if (1) goto IIfdB1;
            }
/* # line 593 "tables.qsc" */	/* host code */
			}
/* # line 595 "tables.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chgd,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 596 "tables.qsc" */	/* host code */
			changed |= chgd;
			if (changed != 0)
			{
				save_prompt |= changed;
/* # line 601 "tables.qsc" */	/* validate */
            {
              if (IIvalfld((char *)"tablefield") == 0) {
                goto IIfdB1;
              } /* IIvalfld */
            }
/* # line 602 "tables.qsc" */	/* host code */
				if ( IIqfGetInfo(qdef, res_field, FALSE,
					unfixed_errors ) != OK )
/* # line 604 "tables.qsc" */	/* resume */
            {
              IIresfld(res_field);
              if (1) goto IIfdB1;
            }
/* # line 606 "tables.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 608 "tables.qsc" */	/* host code */
	   			changed = 0;
			}
			if (qdef_type == -1)
			{
				IIUGerr(E_QF009C_JoinDefNotRun, 0, 0);
			}
			else if (mq_forms)
			{
				IIUGerr(E_QF009D_NoChangeForm, 0, 0);
			}
			else
			{
				if ( !mq_qdframes )
				{
					if ( !mqqdfrms() )
					{
						FDerror(NOQDFRMS, 0, ERx(""));
/* # line 625 "tables.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 626 "tables.qsc" */	/* host code */
					}
					mq_qdframes = TRUE;
				}
				if ( !mq_chgdispl(qdef, &chgd) )
				{
					IIUGerr(E_QF009E_ChangeDispErr, 0, 0);
				}
				changed |= chgd;
				save_prompt |= changed;
			}
          }
        } else if (IIretval() == 5) {
          {
/* # line 640 "tables.qsc" */	/* host code */
			STATUS	rcode = OK;
			if ( _master_check() == FAIL )
			{
/* # line 644 "tables.qsc" */	/* resume */
            {
              IIrescol(_tables,_role);
              if (1) goto IIfdB1;
            }
/* # line 645 "tables.qsc" */	/* host code */
			}
/* # line 647 "tables.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chgd,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 648 "tables.qsc" */	/* host code */
			changed |= chgd;
			if (changed != 0)
			{
				save_prompt |= changed;
/* # line 653 "tables.qsc" */	/* validate */
            {
              if (IIvalfld((char *)"tablefield") == 0) {
                goto IIfdB1;
              } /* IIvalfld */
            }
/* # line 654 "tables.qsc" */	/* host code */
				rcode = IIqfGetInfo(qdef, res_field, TRUE,
					unfixed_errors );
				if ( rcode != OK && rcode != -10 )
				{
/* # line 658 "tables.qsc" */	/* resume */
            {
              IIresfld(res_field);
              if (1) goto IIfdB1;
            }
/* # line 659 "tables.qsc" */	/* host code */
				}
/* # line 661 "tables.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 663 "tables.qsc" */	/* host code */
				changed = 0; 
			}
			if (qdef->mqnumtabs <= 1)
			{
				IIUGerr(E_QF009F_ReqMoreThan1, 0, 0);
/* # line 669 "tables.qsc" */	/* resume */
            {
              IIresfld((char *)"name");
              if (1) goto IIfdB1;
            }
/* # line 670 "tables.qsc" */	/* host code */
			}
			if (!mq_qdframes)
			{
				if ( !mqqdfrms() )
				{
					FDerror(NOQDFRMS, 0, ERx(""));
/* # line 677 "tables.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 678 "tables.qsc" */	/* host code */
				}
				mq_qdframes = TRUE;
			}
			/* If bad datatypes, force mq_joins to redo joins */
			chgd = ( rcode == -10 ) ? 1 : 0;
			chgd2 = 0;
			for (;;)
			{
				stat = mq_joins(qdef, &chgd);
				if (chgd)
				{
					unfixed_errors = FALSE;
				}
				else if (rcode == -10)
				{
					unfixed_errors = TRUE;
					changed = 1;
/* # line 697 "tables.qsc" */	/* resume */
            {
              IIresfld(res_field);
              if (1) goto IIfdB1;
            }
/* # line 698 "tables.qsc" */	/* host code */
				}
				unfixed_errors = FALSE;
				if (stat == 0)
				{
					break;
				}
				changed |= chgd;
				if (!mq_rules(qdef, &chgd2))
				{
					break;
				}
				changed |= chgd2;
			}
			changed |= chgd;
			changed |= chgd2;
			save_prompt |= changed;
/* # line 714 "tables.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 715 "tables.qsc" */	/* resume */
            {
              IIresfld((char *)"name");
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 719 "tables.qsc" */	/* host code */
			if ( _master_check() == FAIL )
			{
/* # line 721 "tables.qsc" */	/* resume */
            {
              IIrescol(_tables,_role);
              if (1) goto IIfdB1;
            }
/* # line 722 "tables.qsc" */	/* host code */
			}
/* # line 724 "tables.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chgd,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 725 "tables.qsc" */	/* host code */
			changed |= chgd;
			if (changed != 0)
			{
				save_prompt |= changed;
/* # line 730 "tables.qsc" */	/* validate */
            {
              if (IIvalfld((char *)"tablefield") == 0) {
                goto IIfdB1;
              } /* IIvalfld */
            }
/* # line 731 "tables.qsc" */	/* host code */
				if ( IIqfGetInfo(qdef, res_field, FALSE,
					unfixed_errors ) != OK )
/* # line 733 "tables.qsc" */	/* resume */
            {
              IIresfld(res_field);
              if (1) goto IIfdB1;
            }
/* # line 735 "tables.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 736 "tables.qsc" */	/* host code */
				changed = 0;
			}
			if (!mq_qdframes)
			{
				if ( !mqqdfrms() )
				{
					FDerror(NOQDFRMS, 0, ERx(""));
/* # line 744 "tables.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 745 "tables.qsc" */	/* host code */
				}
				mq_qdframes = TRUE;
			}
			while (mq_rules(qdef, &chgd))
			{
				changed |= chgd;
				if (qdef->mqnumtabs > 1)
				{
					chgd = 0;
					if (!mq_joins(qdef, &chgd))
						break;
					changed |= chgd;
				}
				else
				{
					IIUGerr(E_QF009F_ReqMoreThan1, 0, 0);
/* # line 761 "tables.qsc" */	/* resume */
            {
              IIresfld((char *)"name");
              if (1) goto IIfdB1;
            }
/* # line 762 "tables.qsc" */	/* host code */
				}
			}
			changed |= chgd;
			save_prompt |= changed;
/* # line 766 "tables.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 767 "tables.qsc" */	/* resume */
            {
              IIresfld((char *)"name");
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 7) {
          {
/* # line 772 "tables.qsc" */	/* validate */
            {
              if (IIvalfld((char *)"tablefield") == 0) {
                goto IIfdB1;
              } /* IIvalfld */
            }
/* # line 774 "tables.qsc" */	/* host code */
			stat = IIQFSaveJD(qdef, FALSE, &changed, 
					currname, res_field, unfixed_errors);
			if (stat == QFS_RES_COLUMN)
			{
/* # line 779 "tables.qsc" */	/* resume */
            {
              IIrescol(_tables,_role);
              if (1) goto IIfdB1;
            }
/* # line 780 "tables.qsc" */	/* host code */
			}
			else
			if (stat == QFS_RES_RESFIELD)
			{
/* # line 784 "tables.qsc" */	/* resume */
            {
              IIresfld(res_field);
              if (1) goto IIfdB1;
            }
/* # line 785 "tables.qsc" */	/* host code */
			}
			else
			if (stat == QFS_RES_ONLY)
			{
/* # line 789 "tables.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 790 "tables.qsc" */	/* host code */
			}
			else
			if (stat == QFS_RES_FIELD)
			{
/* # line 794 "tables.qsc" */	/* resume */
            {
              IIresfld((char *)"name");
              if (1) goto IIfdB1;
            }
/* # line 795 "tables.qsc" */	/* host code */
			}
          }
        } else if (IIretval() == 8) {
          {
            char fname[FE_MAXNAME + 1];
/* # line 803 "tables.qsc" */	/* host code */
			/* owner.table needs splitting into its components */
            char table_part[FE_MAXNAME + 1];
            char owner_part[FE_MAXNAME + 1];
			i4	pick;
            i4 isontbl;
			/* for 6.5 owner.table support */
			FE_RSLV_NAME	resolved_name;
			char	*full_name;
/* # line 813 "tables.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,_mqbftbls,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&isontbl,28,(char *)0,0);
              } /* IIiqset */
            }
/* # line 814 "tables.qsc" */	/* host code */
			if (!isontbl)
			{
				IIUFfieldHelp();
/* # line 817 "tables.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
/* # line 818 "tables.qsc" */	/* host code */
			}
/* # line 819 "tables.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_mqbftbls,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,fname,34,_tables,0);
              } /* IIiqset */
            }
/* # line 821 "tables.qsc" */	/* host code */
			/*
			** should we also display the owner/table popup
			** when listchoices is selected from the owner
			** column?
			*/
			if (STequal(fname, _table)) 
			{
				full_name = IITUtpTblPik(ERx("%"), ERx("QBF"));
				if ((full_name != NULL) && (*full_name != EOS))
				{
					/*
					** Fill in structure for FE_decompose,
					** which will split up the owner.table 
					** name returned by listpick routine 
					** into owner, table components and 
					** display them in the table field
					*/
					resolved_name.name = full_name;
					resolved_name.name_dest = table_part;
					resolved_name.owner_dest = owner_part;
					resolved_name.is_nrml = FALSE;
					FE_decompose(&resolved_name);
/* # line 843 "tables.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_mqbftbls,_tables,-2) != 0) {
                IItcoputio(_table,(short *)0,1,32,0,table_part);
                IItcoputio(_owner,(short *)0,1,32,0,owner_part);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 847 "tables.qsc" */	/* host code */
				}
			}
			else if (STequal(fname, _role))
			{
				pick = IIFDlpListPick(_qfrole_prmpt,
					_qfrole_opt, 2, -1, -1, _qfrole_prmpt, 
					ERx("qfrole.hlp"),
					(i4 (*)())NULL, (PTR)NULL);
				if (pick >= 0)
				{
/* # line 857 "tables.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_mqbftbls,_tables,-2) != 0) {
                IItcoputio(_role,(short *)0,1,32,0,
                ERget((pick == 0)
						? F_QF0015_MASTER 
						: F_QF0016_detail));
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 861 "tables.qsc" */	/* host code */
				}
			}
			else
			{
				IIUFfieldHelp();
			}
/* # line 867 "tables.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 9) {
          {
/* # line 874 "tables.qsc" */	/* host code */
			FEhelp( ERx("qtbls.hlp"),
				ERget(S_QF00A1_QBF_Table_Entry_Form)
			);
          }
        } else if (IIretval() == 10) {
          {
/* # line 882 "tables.qsc" */	/* host code */
			help_subject = STalloc(ERget(S_QF0164_SaveJDEnd));
			if (mq_editjoin && qdef->mqcr.id == -1)
			{
				save_prompt = 1;
			}
			if ( (choice = reallyEnd(save_prompt, CONF_END, 
				help_subject, 
				ERx("qfsavend.hlp"))) == CONFCH_YES )
			{
/* # line 891 "tables.qsc" */	/* validate */
            {
              if (IIvalfld((char *)"tablefield") == 0) {
                goto IIfdB1;
              } /* IIvalfld */
            }
/* # line 893 "tables.qsc" */	/* host code */
				stat = IIQFSaveJD(qdef, TRUE, &changed, 
						currname, res_field,
						unfixed_errors);
				if (stat == QFS_RES_COLUMN)
				{
/* # line 899 "tables.qsc" */	/* resume */
            {
              IIrescol(_tables,_role);
              if (1) goto IIfdB1;
            }
/* # line 900 "tables.qsc" */	/* host code */
				}
				else
				if (stat == QFS_RES_RESFIELD)
				{
/* # line 904 "tables.qsc" */	/* resume */
            {
              IIresfld(res_field);
              if (1) goto IIfdB1;
            }
/* # line 905 "tables.qsc" */	/* host code */
				}
				else
				if (stat == QFS_RES_ONLY)
				{
/* # line 909 "tables.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 910 "tables.qsc" */	/* host code */
				}
				else
				if (stat == QFS_RES_FIELD)
				{
/* # line 914 "tables.qsc" */	/* resume */
            {
              IIresfld((char *)"name");
              if (1) goto IIfdB1;
            }
/* # line 915 "tables.qsc" */	/* host code */
				}
			}
			else
			if (choice == CONFCH_CANCEL)
			{
/* # line 920 "tables.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 921 "tables.qsc" */	/* host code */
			}
			save_prompt = 0;
			if (!mq_editjoin)
			{
				mq_reinit(0, qdef);	/* reinitialize */
			}
/* # line 928 "tables.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 11) {
          {
/* # line 933 "tables.qsc" */	/* host code */
			help_subject = STalloc(ERget(S_QF0165_SaveJDQuit));
			if (mq_editjoin && qdef->mqcr.id == -1)
			{
				save_prompt = 1;
			}
			if ( (choice = reallyEnd(save_prompt, CONF_QUIT,
				help_subject,
				ERx("qfsvquit.hlp"))) == CONFCH_YES )
			{
/* # line 942 "tables.qsc" */	/* validate */
            {
              if (IIvalfld((char *)"tablefield") == 0) {
                goto IIfdB1;
              } /* IIvalfld */
            }
/* # line 944 "tables.qsc" */	/* host code */
				stat = IIQFSaveJD(qdef, TRUE, &changed, 
						currname, res_field,
						unfixed_errors);
				if (stat == QFS_RES_COLUMN)
				{
/* # line 950 "tables.qsc" */	/* resume */
            {
              IIrescol(_tables,_role);
              if (1) goto IIfdB1;
            }
/* # line 951 "tables.qsc" */	/* host code */
				}
				else
				if (stat == QFS_RES_RESFIELD)
				{
/* # line 955 "tables.qsc" */	/* resume */
            {
              IIresfld(res_field);
              if (1) goto IIfdB1;
            }
/* # line 956 "tables.qsc" */	/* host code */
				}
				else
				if (stat == QFS_RES_ONLY)
				{
/* # line 960 "tables.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 961 "tables.qsc" */	/* host code */
				}
				else
				if (stat == QFS_RES_FIELD)
				{
/* # line 965 "tables.qsc" */	/* resume */
            {
              IIresfld((char *)"name");
              if (1) goto IIfdB1;
            }
/* # line 966 "tables.qsc" */	/* host code */
				}
			}
			else
			if (choice == CONFCH_CANCEL)
			{
/* # line 971 "tables.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 972 "tables.qsc" */	/* host code */
			}
			save_prompt = 0;
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
      if (IITBcaClmAct(_tables,_table,0,1) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Go),(char *)0,2,2,2) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Blank),(char *)0,2,2,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_QF0019_ChangeDisplay),(char *)0,2,2,4) == 0) goto 
      IIfdE1;
      if (IInmuact(ERget(F_QF0022_Joins),(char *)0,2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_QF000A_Rules),(char *)0,2,2,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Save),(char *)0,2,2,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,2,7) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,8) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,8) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,9) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,9) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(_EndExpl),2,2,10) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,10) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Quit),(char *)0,2,2,11) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,0,2,11) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 983 "tables.qsc" */	/* host code */
	}
	while (another);
  }
static i4
reallyEnd ( changed, type, help_subject, help_file )
i4	changed;
i4	type;
char	*help_subject;
char	*help_file;
  {
    i4 chgd;
	i4	choice;
/* # line 997 "tables.qsc" */	/* inquire_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&chgd,20,(char *)0,0);
      } /* IIiqset */
    }
/* # line 998 "tables.qsc" */	/* host code */
	changed |= chgd;
	if ( changed && hastables() && (choice=IIUFccConfirmChoice(type, NULL, 
			ERx("JoinDef"), help_subject, help_file)) == CONFCH_YES)
	{
		return(choice);
	}
/* # line 1006 "tables.qsc" */	/* clear */
    {
      IIclrscr();
    }
/* # line 1007 "tables.qsc" */	/* host code */
	return(choice);
  }
static STATUS
_master_check()
{
  char chk_table[FE_MAXNAME + 1];
  char chk_role[MAX_ROLE_SIZE];
	char	*_master;
	bool	masterfound;
	/* bug 6313
	**	Add activate block to make sure there is at
	**	least one master table in each joindef.
	**	(marian 1/14/85)
	*/
	_master = ERget(F_QF0015_MASTER);
	/* Note:  Column _role should have MASTER/detail validation */
	masterfound = FALSE;
/* # line 1027 "tables.qsc" */	/* unloadtable */
  {
    if (IItbact(_mqbftbls,_tables,0) == 0) goto IItbE2;
    while (IItunload() != 0) {
      IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,chk_table,_table);
      IItcogetio((short *)0,1,32,MAX_ROLE_SIZE-1,chk_role,_role);
      IITBceColEnd();
      {
/* # line 1032 "tables.qsc" */	/* host code */
		if (*chk_table != EOS && ( *chk_role == EOS ||
					CMcmpnocase(_master, chk_role) == 0 ) )
		{
			masterfound = TRUE;
/* # line 1036 "tables.qsc" */	/* endloop */
        {
          if (1) goto IItbE2;
        }
/* # line 1037 "tables.qsc" */	/* host code */
		}
      }
    } /* IItunload */
IItbE2:
    IItunend();
  }
/* # line 1039 "tables.qsc" */	/* host code */
	if (!masterfound)
	{
		IIUGerr( E_QF0097_Enter1Table, UG_ERR_ERROR, 0 );
		return FAIL;
	}
	return OK;
}
VOID
mdtablefield(qdef, aform, tfield)
register MQQDEF	*qdef;
  char *aform, *tfield;
  {
	register i4	i;
	for ( i = 0 ; i < qdef->mqnumtabs ; ++i )
    {
      char relid[FE_UNRML_MAXNAME+1];
      char rngid[FE_UNRML_MAXNAME+1];
      char rel_owner[FE_UNRML_MAXNAME+1];
		register RELINFO	*rel = qdef->mqtabs[i];
		/*
		** we need to call IIUGxri_id() now to possibly re-quote
		** the table, owner, and range (correlation) variables,
		** in case they're delim ids.
		**
		** replaced ternary operators with if-then-else, trading
		** "cleverness" for clarity.
		*/
		IIUGxri_id(rel->mqrelid, relid);
/* Bug #67269 */
/* display abbreviation */
		IIUGxri_id(rel->mqrangevar, rngid);
		IIUGxri_id(rel->mqrelown, rel_owner);
/* # line 1079 "tables.qsc" */	/* loadtable */
      {
        if (IItbact(aform,tfield,1) != 0) {
          IItcoputio(_table,(short *)0,1,32,0,relid);
          IItcoputio(_owner,(short *)0,1,32,0,rel_owner);
          IItcoputio(_abbr,(short *)0,1,32,0,rngid);
          IItcoputio(_role,(short *)0,1,32,0,
          ERget((rel->mqtype == 0)
					? F_QF0015_MASTER : F_QF0016_detail
				));
          IITBceColEnd();
        } /* IItbact */
      }
    }
  }
/* # line 1091 "tables.qsc" */	/* host code */
/*{
** Name:	qfsvdisp	- set 'name' field on OO save form to query
**
** Description:
**	When a JoinDef is being saved we do not want the user to be able
**	to change the name of the JoinDef if QBF was invoked with the -J
**	flag.  In this case, the user can only edit and save the JoinDef
**	that was specified on the command line.
**
** Inputs:
**
** Outputs:
**	Returns:
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	01-dec-89  (kenl)  - new interface
*/
static STATUS
qfsvdisp()
{
	if (mq_editjoin)
	{
/* # line 1117 "tables.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,_iisave,(char *)0, (char *)0) != 0) {
      IIstfsio(21,_name,0,(short *)0,1,32,0,(char *)"q");
    } /* IIiqset */
  }
/* # line 1118 "tables.qsc" */	/* host code */
	}
	return(OK);
}
/*
** Name:	IIQFSaveJD	- Save a JoinDef.
**
** Description:
**	Performs the seqeunce of validating and saving a JoinDef.
**
** Inputs:
**	qdef		- {MQQDEF *}  QDEF structure.
**	NotShowSubMenu 	- Flag to indicate if save seq submenu should be shown.
**	changed		- Flag to indicate change has been made to the JoinDef.
**	currname	- The current name of the JoinDef.
**	res_field	- a character array which will be set to the name
**			  of the field to RESUME to, if IIqfGetInfo 
**			  detects an error.
**	force_rebuild	- should IIqfGetInfo be forced to rebuild
**			  table, attribute, and join information?
**
** Outputs:
**	Returns:
**		QFS_RES_COLUMN	  -  Perform a resume column.
**		QFS_RES_RESFIELD  -  Perform a resume field (res_field).
**		QFS_RES_ONLY      -  Perform a resume.
**		QFS_RES_FIELD     -  perform a resume field.
**		QFS_RETURN_OK     -  Don't need to perform anything.
**
** Side Effects:
**	JoinDef will be saved in the database.
**
** History:
**	7-2-90  (martym) - Written for QBF.
**	10/13/90 (emerson) 
**		Check for errors E_QF009A and E_QF012B after check for E_QF0099
**		(as is done when 'Go' is selected).  (bugs 31956 and 31871).
**	10/13/90 (emerson) 
**		Add new parameter force_rebuild to this function and to
**		called function IIqfGetInfo, to force it to rebuild 
**		table, attribute, and join information
**		(thereby reflagging any errors that haven't been fixed).
**		(bug 33914).
**	15-Apr-2003 (bonro01)
**		Added prototype for OOsaveSequence() to prevent core dump.
**	24-Jun-2003 (bonro01)
**		To fix problems in QBF.
**		Added prototype for IIQFSaveJD() and reallyEnd().
**		Added NO_OPTIM for i64_hpu.
*/
i4  IIQFSaveJD(qdef, NotShowSubMenue, changed, currname, res_field,
		force_rebuild)
MQQDEF		*qdef;
bool 		NotShowSubMenue;
i4		*changed; 
  char *currname;
char		*res_field;
bool		force_rebuild;
{
  i4 chgd;
	if (_master_check() == FAIL)
	{
		return(QFS_RES_COLUMN);
	}
/* # line 1187 "tables.qsc" */	/* inquire_frs */
  {
    if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,30,4,&chgd,20,(char *)0,0);
    } /* IIiqset */
  }
/* # line 1189 "tables.qsc" */	/* host code */
	*changed |= chgd;
	if (*changed != 0)
	{
		if (IIqfGetInfo(qdef, res_field, FALSE, force_rebuild) != OK)
			return(QFS_RES_RESFIELD);
/* # line 1196 "tables.qsc" */	/* set_frs */
  {
    if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 1198 "tables.qsc" */	/* host code */
		save_prompt |= *changed;
		*changed = 0;
	}
	if (tblfield && mqcalwidth(qdef) <= 0)
	{
		return(QFS_RES_ONLY);
	}
/* # line 1207 "tables.qsc" */	/* getform */
  {
    if (IIfsetio(_mqbftbls) != 0) {
      IIgetfldio((short *)0,1,32,0,currname,(char *)"name");
    } /* IIfsetio */
  }
/* # line 1209 "tables.qsc" */	/* host code */
	if (STtrmwhite(currname) == 0)
	{
		qdef->mqcr.id = -1;
	}
	if (qdef->mqnumjoins == 0 && qdef->mqnumtabs > 1)
	{
		IIUGerr(E_QF0099_NoJoinFlds, UG_ERR_ERROR, 0);
		return(QFS_RES_FIELD);
	}
	if (qdef->mqnumjoins < qdef->mqnumtabs-1)
	{
		IIUGerr(E_QF009A_NotAllJoined, UG_ERR_ERROR, 0);
		return(QFS_RES_FIELD);
	}
	if ( !mqjmdchk(qdef) )	/* checks for error E_QF012B */
	{
		return(QFS_RES_FIELD);
	}
	OOcatObject(OC_JOINDEF, &qdef->mqcr);
	/*
	**  We now pass in the address of routine qfsvdisp()
	**  which, when editing a JoinDef, will turn off
	**  the ability of the user to change JoinDef name
	*/
	if ( OOsaveSequence(&qdef->mqcr.id, currname,
			_iicatalog, _cattable, _iisave,
			ERget(S_QF00A3_JoinDefSaveTitle),
			qdef, qfsvdisp, mqwritetodb,
			NULL, NULL, NULL, NotShowSubMenue) == OK )
	{
		OOcatRecord(qdef->mqcr.id, &qdef->mqcr);
		STcopy(qdef->mqcr.name, currname);
		STcopy(qdef->mqcr.name, mq_lastsaved);
/* # line 1247 "tables.qsc" */	/* putform */
  {
    if (IIfsetio(_mqbftbls) != 0) {
      IIputfldio((char *)"name",(short *)0,1,32,0,currname);
    } /* IIfsetio */
  }
/* # line 1248 "tables.qsc" */	/* set_frs */
  {
    if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 1249 "tables.qsc" */	/* host code */
		*changed = 0;
		save_prompt = 0;
	}
	return(QFS_RETURN_OK);
}
