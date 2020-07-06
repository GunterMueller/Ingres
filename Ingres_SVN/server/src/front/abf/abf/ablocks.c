# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
# include	<nm.h>		/* 6-x_PC_80x86 */
# include	<cv.h>		/* 6-x_PC_80x86 */
#include	<st.h>
#include	<er.h>
#include	<tm.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
#include	<stdprmpt.h>
# include <ooclass.h>
#include	<uigdata.h>
#include	<abclass.h>
#include	"erab.h"
#include	<oodefine.h>
/**
** Name:	ablocks.qc -	ABF Lock Utilities Module.
**
** Description:
**	Contains the lock checking / lock manipulation routines:
**
**	IIABcelCheckEditLock - check for lock on edit
**	IIABcdlCheckDestroyLock - check for lock on destroy / remove
**	IIABnelNewEditLock - new edit lock.
**	IIABrlRemLock - remove locks.
**	IIABffcFrameFlowChanged - flag FF change for other sessions
**	IIABffsFrameFlowState - check FF state, query for update if changed.
**	IIABlaLockAdm - lock admin screen
**	IIABliLockInit - initialize locks.
**
** History:
**	12/89 (bobm) initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	28-Mar-2005 (lakvi01)
**	    Added oodefine.h for OO prototypes.
**    25-Oct-2005 (hanje04)
**        Add prototype for la_upd() to prevent compiler
**        errors with GCC 4.0 due to conflict with implicit declaration.
*/
OO_CLASS	*OOpclass();
GLOBALREF char	*IIabExename;
GLOBALREF bool	IIabVision;
/*
** Note on the semantics of the _fetch routines -
** they are handlers that receive one row of a select, and return TRUE
** to indicate that the select is to continue, or false to terminate
** it.  The return code has nothing to do with the acceptability of
** the data.
*/
static bool el_fetch();
static bool ffs_fetch();
static bool la_fetch();
static bool det_fetch();
static bool sd_fetch();
static bool dest_fetch();
static bool tree_fetch();
static i4 la_upd (
	OOID    applid);
static i4  zap_it();
static READONLY
  char _LockWarn[]= ERx("ablockwarn"), _LockersTbl[]= ERx("lockers"), _User[]= ERx("user")
  , _Since[]= ERx("since"), _LockAdmin[]= ERx("ablokadm"), _ApplTbl[]= ERx("apptab")
  , _LockTrace[]= ERx("abloktr"), _LockDetail[]= ERx("ablokdet"), _LockTbl[]= ERx("locktab")
  , _Component[]= ERx("comp"), _SessionID[]= ERx("sessid"), _ApplID[]= ERx("applid")
  , _ObjID[]= ERx("objid");
static READONLY	char	_EntryAct[]	= ERx("entry"),
			_WriteAct[]	= ERx("write");
static char	Locker[FE_MAXNAME+1] = {EOS};
static i4	Session = 0;
/*
** used to handle locks more than 1 week old - We don't want to
** incur the DB overhead to remove them at the start of our session.
** We filter them out when fetching, and set Kill_old if we need
** to remove them at exit.  We are relying on string comparisons
** of the date_gmt format here.
*/
static bool	Kill_old = FALSE;
static char	Week_ago[26];
  static i4 Detsid = 0;
static APPL	*Treeapp = NULL;
#define INITCHECK	if (Session == 0) IIABliLockInit()
/*{
** Name:	IIABliLockInit - initialize lock information
**
** Description:
**	Initialize information for locking within this abf session.
**	This is intended to be called explicitly at the beginning of
**	abf, but INITCHECK makes sure it gets called anyway.  If not
**	called at the beginning of abf, the start-of-session time will
**	not be accurate.  We may accept this later on, if the start up
**	penalty is too great.
**
** Input:
**
** Returns:
**
** History:
**	12/89 (bobm) -- Written.
*/
VOID
IIABliLockInit()
{
	SYSTIME now;
	/*
	** if it fails, set Session to 1 so that we don't keep trying
	** to do this, even though there is then a possibility that
	** we will be entering bogus sessions into the lock table.
	*/
	if (IIAMfsiFetchSessionId(&Session,Locker) != OK)
	{
		Session = 1;
		IIUGerr(E_AB00A0_LockSet, UG_ERR_ERROR, 0);
	}
	/*
	** get date_gmt format string for one week ago
	*/
	TMnow(&now);
	now.TM_secs -= 604800L;
	IIUGdfsDateFromSys( &now, Week_ago );
	IIARiaForm(_LockWarn);
	IIARiaForm(_LockAdmin);
/* # line 158 "ablocks.qsc" */	/* inittable */
  {
    if (IItbinit(_LockAdmin,_ApplTbl,(char *)"r") != 0) {
      IIthidecol(_SessionID,(char *)"i4");
      IIthidecol(_ApplID,(char *)"i4");
      IItfill();
    } /* IItbinit */
  }
/* # line 159 "ablocks.qsc" */	/* host code */
	IIARiaForm(_LockDetail);
/* # line 160 "ablocks.qsc" */	/* inittable */
  {
    if (IItbinit(_LockDetail,_LockTbl,(char *)"r") != 0) {
      IIthidecol(_ObjID,(char *)"i4");
      IIthidecol(_SessionID,(char *)"i4");
      IIthidecol(_User,(char *)"c32");
      IItfill();
    } /* IItbinit */
  }
/* # line 162 "ablocks.qsc" */	/* host code */
	IIARiaForm(_LockTrace);
}
/*{
** Name:	IIABcelCheckEditLock - check for lock
**
** Description:
**	Checks for edit locks on component user wishes to edit.  If a lock
**	exists, user is given a chance to decide not to edit.  If user
**	chooses to edit, a lock is placed on the component.  The component
**	may actually be an entire application, in which case the lock
**	type created, and the form used to display other users differs.
**
** Input:
**	comp	- component being editted.
**
** Returns:
**	{STATUS}  OK to go ahead and edit, or FAIL
**
** History:
**	12/89 (bobm) -- Written.
*/
STATUS
  IIABcelCheckEditLock( comp )
APPL_COMP *comp;
  {
	STATUS		rval;
	char		*how;
	OO_CLASS	*ocl;
	ER_MSGID	obj_lock_warn;
    i4 row_cnt;
    char _warn[250+1];
    char _desc[120];
    char name[FE_MAXNAME];
	i4		sid;
	INITCHECK;
	if (comp->class == OC_APPL)
	{
		how = _EntryAct;
		obj_lock_warn =
			(IIabVision) ? F_AB00B6_VappLocked : F_AB00B5_AppLocked;
	}
	else
	{
		how = _WriteAct;
		obj_lock_warn = F_AB00B4_ObjectLocked;
	}
/* # line 213 "ablocks.qsc" */	/* inittable */
    {
      if (IItbinit(_LockWarn,_LockersTbl,(char *)"r") != 0) {
        IIthidecol(_SessionID,(char *)"i4");
        IItfill();
      } /* IItbinit */
    }
/* # line 215 "ablocks.qsc" */	/* host code */
	if ( IIAMelExistingLocks(comp->ooid, el_fetch) != OK )
	{
		/*
		** try to set a lock anyway.  Only give one error
		** message, however.  We treat the "set" error as
		** more serious.
		*/
		IIUGerr( ( IIAMclCreateLock(comp->ooid, Session, Locker, how)
				!= OK ) ? E_AB00A0_LockSet : E_AB00A3_LockFetch,
				UG_ERR_ERROR, 0
		);
		return OK;
	}
/* # line 229 "ablocks.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,_LockWarn,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&row_cnt,32,_LockersTbl,0);
      } /* IIiqset */
    }
/* # line 231 "ablocks.qsc" */	/* host code */
	if ( row_cnt == 0 )
	{
		if (IIAMclCreateLock(comp->ooid, Session, Locker, how) != OK)
			IIUGerr(E_AB00A0_LockSet, UG_ERR_ERROR, 0);
		return OK;
	}
	ocl = OOpclass(comp->class);
	if (ocl == NULL)
		_VOID_ STprintf(_desc,ERx("`%s'"),comp->name);
	else
		_VOID_ STprintf(_desc,ERx("%s: `%s'"),ocl->name,comp->name);
	_VOID_ STprintf(_warn, ERget(F_AB00B3_LockInfo), ERget(obj_lock_warn));
/* # line 246 "ablocks.qsc" */	/* display */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      if (IIdispfrm(_LockWarn,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio((char *)"desc",(short *)0,1,32,0,_desc);
      IIputfldio((char *)"warning",(short *)0,1,32,0,_warn);
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 252 "ablocks.qsc" */	/* host code */
		if (IIAMclCreateLock(comp->ooid, Session, Locker, how) != OK)
			IIUGerr(E_AB00A0_LockSet, UG_ERR_ERROR, 0);
		rval = OK;
/* # line 255 "ablocks.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 261 "ablocks.qsc" */	/* host code */
		rval = FAIL;
/* # line 262 "ablocks.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 267 "ablocks.qsc" */	/* host code */
		row_cnt -= zap_it(_LockWarn, _LockersTbl, OC_UNDEFINED);
		if (row_cnt <= 0)
		{
			if (IIAMclCreateLock(comp->ooid,
					Session, Locker, how) != OK)
				IIUGerr(E_AB00A0_LockSet, UG_ERR_ERROR, 0);
			rval = OK;
/* # line 274 "ablocks.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF1;
            }
/* # line 275 "ablocks.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 4) {
          {
/* # line 281 "ablocks.qsc" */	/* host code */
		FEhelp(ERx("abedlock.hlp"), ERget(F_AB00A3_ELockHelp));
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
      if (IInmuact(ERget(FE_OK),ERget(F_AB00A1_OkExpl),2,2,1) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Cancel),ERget(F_AB00A2_CanExpl),2,2,2) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Delete),ERget(F_FE0111_DeleteExpl),2,2,3) == 0) 
      goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),2,2,4) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,4) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 284 "ablocks.qsc" */	/* host code */
	return rval;
  }
/*{
** Name: el_fetch
**
** static routine to process edit lock fetch - load them into tablefield,
** and bump row count.
*/
static bool
  el_fetch( who, dstr, how, sid )
  char *who;
  char *dstr;
char	*how;
  OOID sid;
  {
	if (STcompare(dstr,Week_ago) < 0)
	{
		Kill_old = TRUE;
		return TRUE;
	}
/* # line 306 "ablocks.qsc" */	/* loadtable */
    {
      if (IItbact(_LockWarn,_LockersTbl,1) != 0) {
        IItcoputio(_User,(short *)0,1,32,0,who);
        IItcoputio(_Since,(short *)0,1,32,0,dstr);
        IItcoputio(_SessionID,(short *)0,1,30,4,&sid);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 309 "ablocks.qsc" */	/* host code */
	return TRUE;
  }
/*{
** Name:	IIABcdlCheckDestroyLock - check for lock on destroy/remove
**
** Description:
**	Checks for locks on component user wishes to destroy.
**
**	If lock exists, either an advisory message might be given, or
**	the operation may be prevented.  The advisory message is used
**	for the "Remove" operation from Vision.  We may actually look
**	for the entire subtree called by this component, and produce
**	an advisory if any of them are being editted.  Only an edit of
**	the component itself prevents the operation.
**
**	We return a flag to allow the caller to know whether or not we
**	prompted the user.  This allows redundant confirmations to
**	be suppressed.
**
** Input:
**	app	- application
**	comp	- component being clobbered.
**	wtree	- include call tree.
**	strict	- do not allow operation if locked.
**
** Returns:
**	pdone	- returned TRUE if user was prompted, FALSE otherwise
**
**	{STATUS}  OK to go ahead, or FAIL
**
** History:
**	12/89 (bobm) -- Written.
*/
STATUS
  IIABcdlCheckDestroyLock( app, comp, wtree, strict, pdone )
APPL *app;
APPL_COMP *comp;
bool wtree;
bool strict;
bool *pdone;
  {
	OO_CLASS	*ocl;
	STATUS		rval;
    i4 row_cnt;
    char _desc[120];
	INITCHECK;
	*pdone = FALSE;
	/*
	** cheap trick - we are using a row of this tablefield to
	** format a date in dest_fetch.  Hence we init the table now.
	** if dest_fetch uses the table we are returning.  We also
	** use a getform in dest_fetch to recover _desc.
	*/
/* # line 367 "ablocks.qsc" */	/* inittable */
    {
      if (IItbinit(_LockTrace,_LockersTbl,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 369 "ablocks.qsc" */	/* host code */
	ocl = OOpclass(comp->class);
	if (ocl == NULL)
		STprintf(_desc, ERx("`%s'"),comp->name);
	else
		STprintf(_desc, ERx("%s: `%s'"),ocl->name,comp->name);
	if (strict)
	{
/* # line 377 "ablocks.qsc" */	/* putform */
    {
      if (IIfsetio(_LockTrace) != 0) {
        IIputfldio((char *)"desc",(short *)0,1,32,0,_desc);
      } /* IIfsetio */
    }
/* # line 378 "ablocks.qsc" */	/* host code */
		if (IIAMelExistingLocks(comp->ooid,dest_fetch) != OK)
		{
			IIUGerr(E_AB00A3_LockFetch, UG_ERR_ERROR, 0);
			return FAIL;
		}
/* # line 383 "ablocks.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,_LockTrace,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&row_cnt,32,_LockersTbl,0);
      } /* IIiqset */
    }
/* # line 385 "ablocks.qsc" */	/* host code */
		if ( row_cnt > 0 )
			return FAIL;
	}
	if (! wtree)
		return OK;
	Treeapp = app;
	IIABmstMarkSubTree(app,comp);
	if (IIAMalApplLocks(app->ooid,tree_fetch) != OK)
	{
		IIUGerr(E_AB00A3_LockFetch, UG_ERR_ERROR, 0);
		return OK;
	}
/* # line 402 "ablocks.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,_LockTrace,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&row_cnt,32,_LockersTbl,0);
      } /* IIiqset */
    }
/* # line 403 "ablocks.qsc" */	/* host code */
	if ( row_cnt == 0 )
		return OK;
	*pdone = TRUE;
/* # line 408 "ablocks.qsc" */	/* display */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      if (IIdispfrm(_LockTrace,(char *)"r") == 0) goto IIfdE2;
      goto IImuI2;
IIfdI2:;
      if (IIfsetio((char *)0) == 0) goto IIfdE2;
      IIputfldio((char *)"desc",(short *)0,1,32,0,_desc);
IIfdB2:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 414 "ablocks.qsc" */	/* host code */
		rval = OK;
/* # line 415 "ablocks.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF2;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 421 "ablocks.qsc" */	/* host code */
		rval = FAIL;
/* # line 422 "ablocks.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF2;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 428 "ablocks.qsc" */	/* host code */
		FEhelp(ERx("abloktr.hlp"), ERget(F_AB00B1_LockDestHelp));
          }
        } else {
          if (1) goto IIfdE2;
        } /* IIretval */
      } /* IIrunform */
IIfdF2:
      if (IIchkfrm() == 0) goto IIfdB2;
      goto IIfdE2;
IImuI2:
      if (IIinitmu() == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_OK),ERget(F_AB00A1_OkExpl),2,2,1) == 0) goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Cancel),ERget(F_AB00A2_CanExpl),2,2,2) == 0) goto 
      IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,2,2,2) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),2,2,3) == 0) 
      goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE2;
      if (IIendmu() == 0) goto IIfdE2;
      goto IIfdI2;
IIfdE2:;
      IIendfrm();
    }
/* # line 431 "ablocks.qsc" */	/* host code */
	return rval;
  }
/*
** Name:
**
** static routine to perform fetch for strict object lock.  This
** prints the message, and terminates query once found.  We use the
** form as a formatting mechanism, and a repository for the appropriate
** item name.
*/
static bool
dest_fetch(who,dstr)
char *who;
  char *dstr;
  {
    char fd[25+1];
    char nbuf[120];
	if (STcompare(dstr,Week_ago) < 0)
	{
		Kill_old = TRUE;
		return TRUE;
	}
/* # line 458 "ablocks.qsc" */	/* loadtable */
    {
      if (IItbact(_LockTrace,_LockersTbl,1) != 0) {
        IItcoputio(_Since,(short *)0,1,32,0,dstr);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 459 "ablocks.qsc" */	/* getrow */
    {
      if (IItbsetio(2,_LockTrace,_LockersTbl,1) != 0) {
        IItcogetio((short *)0,1,32,25+1-1,fd,_Since);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 460 "ablocks.qsc" */	/* getform */
    {
      if (IIfsetio(_LockTrace) != 0) {
        IIgetfldio((short *)0,1,32,119,nbuf,(char *)"desc");
      } /* IIfsetio */
    }
/* # line 462 "ablocks.qsc" */	/* host code */
	IIUGerr(E_AB00A4_DestLock, 0, 3, nbuf, who, fd);
	return FALSE;
  }
/*{
** Name:	tree_fetch
**
** static routine to perform fetch for subtree check.  We are passed
** all of the locked items in the application.  We filter out the
** ones not marked as being in our subtree.
*/
static bool
  tree_fetch(who,dstr,how,sid,oname,class,objid)
  char *who;
  char *dstr;
char *how;
OOID sid;
  char *oname;
OOID class;
OOID objid;
  {
	APPL_COMP	*obj;
	/*
	** we would LIKE to call OOp(), but that may cause a nested
	** query.
	*/
	for (obj = (APPL_COMP *) Treeapp->comps; obj != NULL; obj = obj->_next)
		if (obj->ooid == objid)
			break;
	if (obj == NULL || (obj->flags & TREE_MARK) == 0)
		return TRUE;
	if (STcompare(dstr,Week_ago) < 0)
	{
		Kill_old = TRUE;
		return TRUE;
	}
/* # line 504 "ablocks.qsc" */	/* loadtable */
    {
      if (IItbact(_LockTrace,_LockersTbl,1) != 0) {
        IItcoputio(_User,(short *)0,1,32,0,who);
        IItcoputio((char *)"frame",(short *)0,1,32,0,oname);
        IItcoputio(_Since,(short *)0,1,32,0,dstr);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 508 "ablocks.qsc" */	/* host code */
	return TRUE;
  }
/*{
** Name:	IIABnelNewEditLock - check for lock
**
** Description:
**	Call to set an edit lock on newly created component, just prior
**	to edit.  You COULD simply call CheckEditLock, but since the
**	component is brand new, there wouldn't be any locks, and you'd
**	be wasting a query.
**
** Input:
**	objid - object id.
**
** Returns:
**	{STATUS}  OK to go ahead and edit, or FAIL
**
** History:
**	12/89 (bobm) -- Written.
*/
STATUS
IIABnelNewEditLock(objid)
OOID objid;
{
	if (IIAMclCreateLock(objid, Session, Locker, _WriteAct) != OK)
		IIUGerr(E_AB00A0_LockSet, UG_ERR_ERROR, 0);
}
/*{
** Name:	IIABrlRemLock - remove locks.
**
** Description:
**	Remove lock or locks for this session.
**
** Input:
**	objid - object id to remove lock on, or OC_UNDEFINED to remove
**		all locks for this session.
**
**		OC_UNDEFINED is used on abf exit, so we also remove
**		locks > 1 week old at this point if we encountered any.
**
** Returns:
**	STATUS
**
** History:
**	12/89 (bobm) -- Written.
*/
STATUS
IIABrlRemLock(objid)
OOID objid;
{
	STATUS rval;
	INITCHECK;
	if ((rval = IIAMdlDeleteLocks(objid, Session)) != OK)
		IIUGerr(E_AB00A0_LockSet, UG_ERR_ERROR, 0);
	/*
	** if rval != OK, we probably failed due to DB errors.
	** Don't bother with cleanup that would likely fail as well.
	*/
	if (rval == OK && objid == OC_UNDEFINED && Kill_old)
	{
		IIAMlcuLockCleanUp(Week_ago);
		Kill_old = FALSE;
	}
	return rval;
}
/*{
** Name:	IIABffcFrameFlowChanged
**
** Description:
**	Use locks to flag changes to the frame flow diagram in
**	other sessions
**
** Input:
**	applid - application id.
**
** Returns:
**	STATUS
**
** History:
**	12/89 (bobm) -- Written.
*/
STATUS
IIABffcFrameFlowChanged(applid)
OOID applid;
{
	STATUS rval;
	INITCHECK;
	if ( (rval = IIAMcolChangeOtherLocks(applid, Session, ERx("refresh")))
			!= OK )
		IIUGerr(E_AB00A0_LockSet, UG_ERR_ERROR, 0);
	return rval;
}
/*{
** Name:	IIABffsFrameFlowState
**
** Description:
**	Examine state of session lock on application to see if frame flow
**	has changed.
**
** Input:
**	appl	- application.
**
** Returns:
**	TRUE - state has changed, and user has requested update.
**
** History:
**	12/89 (bobm) -- Written.
*/
static bool Chg;
bool
IIABffsFrameFlowState(appl)
APPL *appl;
{
	i4 cc;
	INITCHECK;
	/*
	** if lock check somehow fails, punt!  We don't bother with
	** an error message here because if this is failing it would
	** be incredibly obnoxious to keep seeing the message periodically.
	*/
	Chg = FALSE;
	_VOID_ IIAMelExistingLocks(appl->ooid,ffs_fetch);
	if (!Chg)
		return FALSE;
	_VOID_ IIAMcslChangeSessLock(appl->ooid, Session, ERx("entry"));
	cc = IIUFccConfirmChoice(CONF_GENERIC, ERx(""), ERx(""),
		ERget(F_AB00A5_RCHelp), ERx("abfflst.hlp"), F_AB00A6_RTitle,
		F_AB00A7_Yexp, F_AB00A8_Nexp, ERx(""), FALSE);
	return (bool)( cc == CONFCH_YES );
}
/*{
** Name: ffs_fetch
**
** static routine to look for application lock for a given session.
** sets Chg to TRUE if needed, returns FALSE to terminate query when
** record has been found.
*/
static bool
ffs_fetch(who,dstr,how,sessid)
char *who;
char *dstr;
char *how;
i4	sessid;
{
	if (sessid != Session)
		return TRUE;
	if (!STequal(how, _EntryAct) )
		Chg = TRUE;
	return FALSE;
}
/*{
** Name:	IIABlaLockAdm
**
** Description:
**	screen to administer/list lock information.
**
** Input:
**	applid - ID of application to restrict to, or OC_UNDEFINED
**
** Returns:
**	none.
**
** History:
**	12/89 (bobm) -- Written.
*/
static VOID	det_frame();
VOID
  IIABlaLockAdm(applid)
OOID	applid;
  {
	i4	rows;
    char tbuf[120];
    i4 old_to;
    i4 my_to;
	char	*ptr;
	INITCHECK;
	my_to = 300;
	NMgtAt(ERx("II_AFD_TIMEOUT"),&ptr);
	if (ptr != NULL && *ptr != EOS)
	{
		_VOID_ CVan(ptr,&my_to);
		if (my_to != 0 && my_to < 20)
			my_to = 20;
	}
/* # line 721 "ablocks.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&old_to,50,(char *)0,0);
      } /* IIiqset */
    }
/* # line 722 "ablocks.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&my_to);
      } /* IIiqset */
    }
/* # line 724 "ablocks.qsc" */	/* host code */
	IIUGmsg(ERget(S_AB00A1_LockRead), FALSE, 0);
	IIUGfmt(tbuf, sizeof(tbuf)-1, ERget(F_AB00A9_LokTitle), 1, IIabExename);
/* # line 728 "ablocks.qsc" */	/* display */
    {
      if (IIdispfrm(_LockAdmin,(char *)"r") == 0) goto IIfdE3;
      goto IImuI3;
IIfdI3:;
      if (IIfsetio((char *)0) == 0) goto IIfdE3;
      IIputfldio((char *)"title",(short *)0,1,32,0,tbuf);
      {
/* # line 732 "ablocks.qsc" */	/* host code */
		if ( (rows = la_upd(applid)) < 0 )
/* # line 733 "ablocks.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE3;
        }
      }
IIfdB3:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 738 "ablocks.qsc" */	/* host code */
		IIUGmsg(ERget(S_AB00A1_LockRead), FALSE, 0);
		rows = la_upd(applid);
          }
        } else if (IIretval() == 2) {
          {
/* # line 744 "ablocks.qsc" */	/* host code */
		IIUGmsg(ERget(S_AB00A1_LockRead), FALSE, 0);
		rows = la_upd(applid);
          }
        } else if (IIretval() == 3) {
          {
/* # line 750 "ablocks.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 751 "ablocks.qsc" */	/* host code */
		if ( rows == 0 )
			IIUGerr(E_AB00A5_NoLocks, UG_ERR_ERROR, 0);
		else
			rows -= zap_it(_LockAdmin,_ApplTbl,OC_UNDEFINED);
/* # line 755 "ablocks.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&my_to);
              } /* IIiqset */
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 760 "ablocks.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 761 "ablocks.qsc" */	/* host code */
		if ( rows == 0 )
			IIUGerr(E_AB00A5_NoLocks, UG_ERR_ERROR, 0);
		else
			det_frame();
/* # line 765 "ablocks.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&my_to);
              } /* IIiqset */
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 771 "ablocks.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 772 "ablocks.qsc" */	/* host code */
		FEhelp(ERx("ablokadm.hlp"), tbuf);
/* # line 773 "ablocks.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&my_to);
              } /* IIiqset */
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 779 "ablocks.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF3;
            }
          }
        } else {
          if (1) goto IIfdE3;
        } /* IIretval */
      } /* IIrunform */
IIfdF3:
      if (IIchkfrm() == 0) goto IIfdB3;
      goto IIfdE3;
IImuI3:
      if (IIFRtoact(0,1) == 0) goto IIfdE3;
      if (IIinitmu() == 0) goto IIfdE3;
      if (IInmuact(ERget(F_AB00A0_Refresh),ERget(F_AB00A4_RefExpl),2,2,2) == 0
      ) goto IIfdE3;
      if (IInmuact(ERget(FE_Delete),ERget(F_FE0111_DeleteExpl),2,2,3) == 0) 
      goto IIfdE3;
      if (IInmuact(ERget(FE_MoreInfo),ERget(F_FE0107_ExplMoreInfo),2,2,4) == 0
      ) goto IIfdE3;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),2,2,5) == 0) 
      goto IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,5) == 0) goto IIfdE3;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,6) == 0) goto 
      IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE3;
      if (IIendmu() == 0) goto IIfdE3;
      goto IIfdI3;
IIfdE3:;
      IIendfrm();
    }
/* # line 782 "ablocks.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&old_to);
      } /* IIiqset */
    }
  }
/* # line 786 "ablocks.qsc" */	/* host code */
/*
** Name: zap_it
**
** static routine to allow DBA to zap locks for a any user session, or a user
** to zap their own locks.
**
** get pertinent info from hidden columns of tablefield, delete from DB,
** and remove tablefield row.  Returns number of rows removed.
**
** Called with oid = OC_UNDEFINED to delete all locks for a session, or
** with the object id to delete locks on.
**
** History:
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to IIUIdbdata().  Also,
**	    use the isdba field instead of doing STequal on user and dba.
*/
static i4
  zap_it(fm,tbl,oid)
  char *fm;
  char *tbl;
i4 oid;
  {
    i4 sid;
    char nm[FE_MAXNAME+1];
/* # line 814 "ablocks.qsc" */	/* getrow */
    {
      if (IItbsetio(2,fm,tbl,-2) != 0) {
        IItcogetio((short *)0,1,30,4,&sid,_SessionID);
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,nm,_User);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 816 "ablocks.qsc" */	/* host code */
	if (sid == Session)
	{
		IIUGerr(E_AB00A6_OwnLock, UG_ERR_ERROR, 0);
		return 0;
	}
	if ( ! STequal(nm,Locker) && ! IIUIdbdata()->isdba )
	{
		IIUGerr(E_AB02F7_LockOwner, UG_ERR_ERROR, 0);
		return 0;
	}
	if ( IIUFccConfirmChoice(CONF_DELETE, ERx(""), ERget(F_AB00AA_Lock),
				ERget(F_AB00AB_LDHelp), ERx("ablokdel.hlp"))
			== CONFCH_YES )
	{
		IIUGmsg(ERget(S_AB00A2_LockDelete), FALSE, 0);
		if ( IIAMdlDeleteLocks(oid, sid) == OK )
		{
/* # line 835 "ablocks.qsc" */	/* deleterow */
    {
      if (IItbsetio(4,fm,tbl,-2) != 0) {
        if (IItdelrow(0) != 0) {
        } /* IItdelrow */
      } /* IItbsetio */
    }
/* # line 836 "ablocks.qsc" */	/* host code */
			return 1;
		}
		IIUGerr(E_AB00A0_LockSet, UG_ERR_ERROR, 0);
		return 0;
	}
	return 0;
  }
/*{
** Name:	la_upd
**
** static routine to update lock listing screen.  Session / application
** id are attached to tablefield to allow us to do the "details" screen
*/
static i4
la_upd ( applid )
OOID	applid;
{
  i4 row_cnt;
/* # line 857 "ablocks.qsc" */	/* clear */
  {
    IIfldclear(_ApplTbl);
  }
/* # line 859 "ablocks.qsc" */	/* host code */
	if ( IIAMalApplLocks(applid,la_fetch) != OK )
	{
		IIUGerr(E_AB00A3_LockFetch, UG_ERR_ERROR, 0);
		return -1;
	}
/* # line 865 "ablocks.qsc" */	/* inquire_frs */
  {
    if (IIiqset(3,0,_LockAdmin,(char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,30,4,&row_cnt,32,_ApplTbl,0);
    } /* IIiqset */
  }
/* # line 867 "ablocks.qsc" */	/* host code */
	return row_cnt;
}
/*{
** Name:	la_fetch
**
** static routine to perform fetch for lock listing update.
** We look explicitly for the locks on applications.
*/
static bool
  la_fetch( who, dstr, how, sid, oname, class, objid )
  char *who;
  char *dstr;
char	*how;
  i4 sid;
  char *oname;
OOID	class;
  OOID objid;
  {
	if (class != OC_APPL)
		return TRUE;
/* # line 889 "ablocks.qsc" */	/* loadtable */
    {
      if (IItbact(_LockAdmin,_ApplTbl,1) != 0) {
        IItcoputio(_User,(short *)0,1,32,0,who);
        IItcoputio((char *)"appl",(short *)0,1,32,0,oname);
        IItcoputio(_Since,(short *)0,1,32,0,dstr);
        IItcoputio(_SessionID,(short *)0,1,30,4,&sid);
        IItcoputio(_ApplID,(short *)0,1,30,4,&objid);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 893 "ablocks.qsc" */	/* host code */
	return TRUE;
  }
/*
** Name:	det_frame
**
** Details frame for locks for a given user session.
**
** History:
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to IIUIdbdata().  Also,
**	    use the isdba field instead of doing STequal on user and dba.
*/
static VOID
  det_frame()
  {
    OOID oid;
    char *_delete;
    char name[FE_MAXNAME+1];
    i4 row_cnt;
/* # line 915 "ablocks.qsc" */	/* getrow */
    {
      if (IItbsetio(2,_LockAdmin,_ApplTbl,-2) != 0) {
        IItcogetio((short *)0,1,30,4,&Detsid,_SessionID);
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,name,_User);
        IItcogetio((short *)0,1,30,4,&oid,_ApplID);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 919 "ablocks.qsc" */	/* host code */
	/*
	** the "delete" menuitem goes with the deletion of individual
	** component locks.  Currently, we are not turning on this feature.
	*/
#ifdef UNLOCK_COMPONENTS
	if (STequal(name,Locker) || IIUIdbdata()->isdba )
		_delete = ERget(FE_Delete);
	else
		_delete = NULL;
#else
	_delete = NULL;
#endif
/* # line 932 "ablocks.qsc" */	/* display */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      if (IIdispfrm(_LockDetail,(char *)"r") == 0) goto IIfdE4;
      goto IImuI4;
IIfdI4:;
      if (IIfsetio((char *)0) == 0) goto IIfdE4;
      IIputfldio(_User,(short *)0,1,32,0,name);
      {
/* # line 936 "ablocks.qsc" */	/* clear */
        {
          IIfldclear(_LockTbl);
        }
/* # line 938 "ablocks.qsc" */	/* host code */
		/*
		** no error message - simply display empty table.  DB failure
		** probably produced an error message already, anyway
		*/
		_VOID_ IIAMslSessionLocks(Detsid, sd_fetch);
		_VOID_ IIAMalApplLocks(oid, det_fetch);
      }
IIfdB4:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 949 "ablocks.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_LockDetail,_LockTbl,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,name,_Component);
                IItcogetio((short *)0,1,30,4,&oid,_ObjID);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 950 "ablocks.qsc" */	/* host code */
		zap_it(_LockDetail, _LockTbl, oid);
/* # line 951 "ablocks.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_LockWarn,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&row_cnt,32,_LockersTbl,0);
              } /* IIiqset */
            }
/* # line 952 "ablocks.qsc" */	/* host code */
		if (row_cnt == 0)
		{
/* # line 954 "ablocks.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF4;
            }
/* # line 955 "ablocks.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 2) {
          {
/* # line 961 "ablocks.qsc" */	/* host code */
		FEhelp(ERx("ablokdet.hlp"), ERget(F_AB00AE_LDetHelp));
          }
        } else if (IIretval() == 3) {
          {
/* # line 967 "ablocks.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF4;
            }
          }
        } else {
          if (1) goto IIfdE4;
        } /* IIretval */
      } /* IIrunform */
IIfdF4:
      if (IIchkfrm() == 0) goto IIfdB4;
      goto IIfdE4;
IImuI4:
      if (IIinitmu() == 0) goto IIfdE4;
      if (IInmuact(_delete,ERget(F_FE0111_DeleteExpl),2,2,1) == 0) goto IIfdE4;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),2,2,2) == 0) 
      goto IIfdE4;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,2) == 0) goto IIfdE4;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,3) == 0) goto 
      IIfdE4;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE4;
      if (IIendmu() == 0) goto IIfdE4;
      goto IIfdI4;
IIfdE4:;
      IIendfrm();
    }
  }
/* # line 972 "ablocks.qsc" */	/* host code */
static bool
  sd_fetch( who, dstr, how, id )
char *who;
  char *dstr;
char *how;
OOID id;
  {
	if (id != 0)
		return TRUE;
/* # line 982 "ablocks.qsc" */	/* putform */
    {
      if (IIfsetio(_LockDetail) != 0) {
        IIputfldio(_Since,(short *)0,1,32,0,dstr);
      } /* IIfsetio */
    }
/* # line 984 "ablocks.qsc" */	/* host code */
	return FALSE;
  }
static bool
  det_fetch( who, dstr, how, sid, oname, class, oid )
  char *who;
  char *dstr;
  char *how;
  OOID sid;
  char *oname;
OOID	class;
  OOID oid;
  {
	if (sid == Detsid)
	{
		OO_CLASS	*ocl;
		ocl = OOpclass(class);
/* # line 1003 "ablocks.qsc" */	/* loadtable */
    {
      if (IItbact(_LockDetail,_LockTbl,1) != 0) {
        IItcoputio(_Component,(short *)0,1,32,0,oname);
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERx(ocl == NULL ? "?" : ocl->name));
        IItcoputio((char *)"locktype",(short *)0,1,32,0,how);
        IItcoputio(_Since,(short *)0,1,32,0,dstr);
        IItcoputio(_ObjID,(short *)0,1,30,4,&oid);
        IItcoputio(_User,(short *)0,1,32,0,who);
        IItcoputio(_SessionID,(short *)0,1,30,4,&sid);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1007 "ablocks.qsc" */	/* host code */
	}
	return TRUE;
  }
