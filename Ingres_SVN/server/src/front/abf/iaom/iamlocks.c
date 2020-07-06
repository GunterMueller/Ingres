# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 1989, 2004 Ingres Corporation
*/
# include <compat.h>
# include <id.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include <fe.h>
# include <er.h>
# include <cm.h>
# include <st.h>
# include <ug.h>
# include <ooclass.h>
# include <oocat.h>
# include <uigdata.h>
# include "eram.h"
/**
** Name:	iamlocks.c -	locking utilities
**
** Description:
**
**	Utilities called from abf to handle locking
**
**	IIAMelExistingLocks - find existing locks
**	IIAMalApplLocks - find existing locks in an application
**	IIAMslSessLocks - find existing locks for a session.
**	IIAMdlDeleteLocks - delete a lock or several locks
**	IIAMlcuLockCleanUp - clean up stale locks
**	IIAMclCreateLock - create a new lock
**	IIAMfsiFindSessionId - return a new session id to use with locks
**	IIAMcolChangeOtherLocks - change lock type for other sessions
**	IIAMcslChangeSessLock - change lock type for this session
**/
char *UGcat_now();
/*{
** Name - IIAMelExistingLocks
**
** Description:
**	Find existing locks for a given object.
**
** Inputs:
**	objid - object id.
**	hook - action routine to pass lock information to:
**
**		(*hook)(who,date,how,sessid)
**		char *who;
**		char *date;
**		char *how;
**		i4 sessid;
**
**	This routine should return TRUE to continue retrieving locks,
**	FALSE to end the retrieve.  Since it is being called inside a
**	select, it may not do any DB access.  The character buffers
**	are overwritten between calls, so copies must be made if the
**	strings are to be retained.
**
** Outputs:
**
** Returns:
**	STATUS
**
** History:
**	12/89 (bobm)	written
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
STATUS
IIAMelExistingLocks(objid,hook)
  OOID objid;
bool (*hook)();
{
  i4 sessid;
  char who[FE_MAXNAME+1];
  char dstamp[26];
  char why[17];
/* # line 90 "iamlocks.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamlocks1",6608,18172);
      IIputdomio((short *)0,1,30,4,&objid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamlocks1",6608,18172);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select session_id, locked_by, lock_date, lock_type from ii_locks wher\
e entity_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&objid);
        IIexDefine(0,(char *)"iamlocks1",6608,18172);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&sessid);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,who);
      IIgetdomio((short *)0,1,32,25,dstamp);
      IIgetdomio((short *)0,1,32,16,why);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 95 "iamlocks.sc" */	/* host code */
		if (! (*hook)(who,dstamp,why,sessid))
/* # line 96 "iamlocks.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 99 "iamlocks.sc" */	/* host code */
	return FEinqerr();
}
/*{
** Name - IIAMslSessionLocks
**
** Description:
**	Find existing locks for a given session.
**
** Inputs:
**	sessid - session id.
**	hook - action routine to pass lock information to:
**
**		(*hook)(who,date,how,objid)
**		char *who;
**		char *date;
**		char *how;
**		OOID objid;
**
**	This routine should return TRUE to continue retrieving locks,
**	FALSE to end the retrieve.  Since it is being called inside a
**	select, it may not do any DB access.  The character buffers
**	are overwritten between calls, so copies must be made if the
**	strings are to be retained.
**
** Outputs:
**
** Returns:
**	STATUS
**
** History:
**	12/89 (bobm)	written
*/
STATUS
IIAMslSessionLocks(sessid,hook)
  OOID sessid;
bool (*hook)();
{
  OOID objid;
  char who[FE_MAXNAME+1];
  char dstamp[26];
  char why[17];
/* # line 148 "iamlocks.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamlocks2",6609,20202);
      IIputdomio((short *)0,1,30,4,&sessid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamlocks2",6609,20202);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select entity_id, locked_by, lock_date, lock_type from ii_locks where\
 session_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&sessid);
        IIexDefine(0,(char *)"iamlocks2",6609,20202);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&objid);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,who);
      IIgetdomio((short *)0,1,32,25,dstamp);
      IIgetdomio((short *)0,1,32,16,why);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 153 "iamlocks.sc" */	/* host code */
		if (! (*hook)(who,dstamp,why,objid))
/* # line 154 "iamlocks.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE2;
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
/* # line 157 "iamlocks.sc" */	/* host code */
	return FEinqerr();
}
/*{
** Name - IIAMalApplLocks
**
** Description:
**	Find locks for a given application.
**
** Inputs:
**	applid - application id, OC_UNDEFINED for all applications.
**	hook - action routine to pass lock information to:
**
**		(*hook)(who,date,how,sessid,oname,class,objid)
**		char *who;
**		char *date;
**		char *how;
**		i4 sessid;
**		char *oname;
**		OOID class;
**		OOID objid;
**
**	This routine should return TRUE to continue retrieving locks,
**	FALSE to end the retrieve.  Since it is being called inside a
**	select, it may not do any DB access.  The character buffers
**	are overwritten between calls, so copies must be made if the
**	strings are to be retained.
**
** Outputs:
**
** Returns:
**	STATUS
**
** History:
**	12/89 (bobm)	written
*/
STATUS
IIAMalApplLocks(applid,hook)
  OOID applid;
bool (*hook)();
{
  i4 sessid;
  char who[FE_MAXNAME+1];
  char dstamp[26];
  char why[17];
  char oname[FE_MAXNAME+1];
  OOID oclass;
  OOID oid;
/* # line 211 "iamlocks.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamlocks3",6610,18154);
      IIputdomio((short *)0,1,30,4,&applid);
      IIputdomio((short *)0,1,30,4,&applid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamlocks3",6610,18154);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select ii_objects.object_id, ii_objects.object_class, ii_objects.obje\
ct_name, ii_locks.session_id, ii_locks.locked_by, ii_locks.lock_date, \
ii_locks.lock_type from ii_objects, ii_locks, ii_abfobjects where(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&applid);
        IIwritio(0,(short *)0,1,32,0,(char *)
"= -1 or ii_abfobjects.applid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&applid);
        IIwritio(0,(short *)0,1,32,0,(char *)
")and ii_objects.object_id=ii_abfobjects.object_id and ii_locks.entity\
_id=ii_objects.object_id order by ii_locks.lock_date");
        IIexDefine(0,(char *)"iamlocks3",6610,18154);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&oid);
      IIgetdomio((short *)0,1,30,4,&oclass);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,oname);
      IIgetdomio((short *)0,1,30,4,&sessid);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,who);
      IIgetdomio((short *)0,1,32,25,dstamp);
      IIgetdomio((short *)0,1,32,16,why);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 223 "iamlocks.sc" */	/* host code */
	{
		if (! (*hook)(who,dstamp,why,sessid,oname,oclass,oid))
/* # line 225 "iamlocks.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE3;
      }
/* # line 226 "iamlocks.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
	return FEinqerr();
}
/*{
** Name - IIAMdlDeleteLocks
**
** Description:
**	Delete locks for a specific object, or for a session.
**
** Inputs:
**	objid - object id - OC_UNDEFINED to delete all session locks.
**	sessid - session id - OC_UNDEFINED to delete all locks on object.
**
** Returns:
**	STATUS
**
** History:
**	12/89 (bobm)	written
*/
STATUS
IIAMdlDeleteLocks(objid,sessid)
  OOID objid;
  i4 sessid;
{
	STATUS rval;
	iiuicw1_CatWriteOn();
	if (objid == OC_UNDEFINED)
	{
/* # line 264 "iamlocks.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamlocks4",6611,32747);
      IIputdomio((short *)0,1,30,4,&sessid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamlocks4",6611,32747);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_locks where session_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&sessid);
        IIexDefine(0,(char *)"iamlocks4",6611,32747);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 265 "iamlocks.sc" */	/* host code */
	}
	else
	{
		if (sessid == OC_UNDEFINED)
		{
/* # line 272 "iamlocks.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamlocks5",6612,17641);
      IIputdomio((short *)0,1,30,4,&objid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamlocks5",6612,17641);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_locks where entity_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&objid);
        IIexDefine(0,(char *)"iamlocks5",6612,17641);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 273 "iamlocks.sc" */	/* host code */
		}
		else
		{
/* # line 278 "iamlocks.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamlocks6",6613,16872);
      IIputdomio((short *)0,1,30,4,&objid);
      IIputdomio((short *)0,1,30,4,&sessid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamlocks6",6613,16872);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_locks where entity_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&objid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and session_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&sessid);
        IIexDefine(0,(char *)"iamlocks6",6613,16872);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 280 "iamlocks.sc" */	/* host code */
		}
	}
	rval = FEinqerr();
	iiuicw0_CatWriteOff();
	return rval;
}
/*{
** Name - IIAMlcuLockCleanUp
**
** Description:
**	Delete stale locks.  This only deletes locks with non-zero
**	session ids's.
**
** Inputs:
**	dstr - date string.  Locks older than this will be removed.
**
** Returns:
**	STATUS
**
** History:
**	2/90 (bobm)	written
*/
STATUS
IIAMlcuLockCleanUp(dstr)
  char *dstr;
{
	STATUS rval;
	iiuicw1_CatWriteOn();
/* # line 318 "iamlocks.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamlocks7",6614,20456);
      IIputdomio((short *)0,1,32,0,dstr);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamlocks7",6614,20456);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_locks where session_id<>0 and lock_date<");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,dstr);
        IIexDefine(0,(char *)"iamlocks7",6614,20456);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 320 "iamlocks.sc" */	/* host code */
	rval = FEinqerr();
	iiuicw0_CatWriteOff();
	return rval;
}
/*{
** Name - IIAMclCreateLock
**
** Description:
**	Create a lock
**
** Inputs:
**	objid - object id.
**	sessid - sessid.
**	name - name of locker.
**	how - how locked (type of lock).
**
** Returns:
**	STATUS
**
** History:
**	12/89 (bobm)	written
*/
STATUS
IIAMclCreateLock(objid,sessid,name,how)
  OOID objid;
  i4 sessid;
  char *name;
  char *how;
{
  char *dtstr;
	STATUS	rval;
	dtstr = UGcat_now();
	iiuicw1_CatWriteOn();
/* # line 364 "iamlocks.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamlocks8",6615,17908);
      IIputdomio((short *)0,1,30,4,&objid);
      IIputdomio((short *)0,1,30,4,&sessid);
      IIputdomio((short *)0,1,32,0,name);
      IIputdomio((short *)0,1,32,0,dtstr);
      IIputdomio((short *)0,1,32,0,how);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamlocks8",6615,17908);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_locks(entity_id, session_id, locked_by, lock_date, loc\
k_type)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&objid);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&sessid);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,dtstr);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,how);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iamlocks8",6615,17908);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 381 "iamlocks.sc" */	/* host code */
	rval = FEinqerr();
	iiuicw0_CatWriteOff();
	return rval;
}
/*{
** Name - IIAMfsiFetchSessionId
**
** Description:
**	Get a unique session id and appropriate name for locking operations.
**
** Inputs:
**	name - buffer for name.
**
** Outputs:
**	sessid - returned session id.
**	name - name returned.
**
** Returns:
**	STATUS
**
** History:
**	12/89 (bobm)	written
**	6/90 (Mike S)	Strip trailing whitespace from name
*/
STATUS
IIAMfsiFetchSessionId(sessid,name)
i4 *sessid;
char *name;
{
	i4 odelta;
	odelta = IIOOsetidDelta(1);
	iiuicw1_CatWriteOn();
	*sessid = IIOOnewId();
	iiuicw0_CatWriteOff();
	_VOID_ IIOOsetidDelta(odelta);
	if (*sessid <= OC_UNDEFINED )
                return FAIL;
	IDname(&name);	/* yes, address of.  yes, it's silly */
	_VOID_ STtrmwhite(name); /* Strip trailing whitespace (for VMS) */
	return IIAMclCreateLock(0,*sessid,name,ERx("abf"));
}
/*{
** Name - IIAMcolChangeOtherLocks - change lock type for other sessions
**
** Description:
**	Changes the type of lock on other sessions.
**
** Inputs:
**	objid - object to change lock on.
**	sessid - session id to leave alone.
**	how - new lock type.
**
** Returns:
**	STATUS
**
** History:
**	12/89 (bobm)	written
*/
STATUS
IIAMcolChangeOtherLocks(objid,sessid,how)
  OOID objid;
  i4 sessid;
  char *how;
{
	STATUS rval;
	iiuicw1_CatWriteOn();
/* # line 466 "iamlocks.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamlocks9",6616,24547);
      IIputdomio((short *)0,1,32,0,how);
      IIputdomio((short *)0,1,30,4,&objid);
      IIputdomio((short *)0,1,30,4,&sessid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamlocks9",6616,24547);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_locks set lock_type=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,how);
        IIwritio(0,(short *)0,1,32,0,(char *)"where entity_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&objid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and session_id<>");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&sessid);
        IIexDefine(0,(char *)"iamlocks9",6616,24547);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 470 "iamlocks.sc" */	/* host code */
	rval = FEinqerr();
	iiuicw0_CatWriteOff();
	return rval;
}
/*{
** Name - IIAMcslChangeSessLock - change lock type for other sessions
**
** Description:
**	Changes the type of lock on a given session.
**
** Inputs:
**	objid - object to change lock on.
**	sessid - session id to change lock on.
**	how - new lock type.
**
** Returns:
**	STATUS
**
** History:
**	12/89 (bobm)	written
*/
STATUS
IIAMcslChangeSessLock(objid,sessid,how)
  OOID objid;
  i4 sessid;
  char *how;
{
	STATUS rval;
	iiuicw1_CatWriteOn();
/* # line 507 "iamlocks.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamlocks10",6617,8930);
      IIputdomio((short *)0,1,32,0,how);
      IIputdomio((short *)0,1,30,4,&objid);
      IIputdomio((short *)0,1,30,4,&sessid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamlocks10",6617,8930);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_locks set lock_type=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,how);
        IIwritio(0,(short *)0,1,32,0,(char *)"where entity_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&objid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and session_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&sessid);
        IIexDefine(0,(char *)"iamlocks10",6617,8930);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 511 "iamlocks.sc" */	/* host code */
	rval = FEinqerr();
	iiuicw0_CatWriteOff();
	return rval;
}
