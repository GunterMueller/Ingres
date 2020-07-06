# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 1993, 2004 Ingres Corporation
*/
# include       <compat.h>
# include	<si.h>
# include       <st.h>
# include       <gl.h>
# include       <iicommon.h>
# include       <fe.h>
# include	<tm.h>
# include	<ug.h>
# include	<ooclass.h>
# include	<oocat.h>
# include	<abfdbcat.h>
# include	"erie.h"
# include       "ieimpexp.h"
/**
** Name:        ieiiobjs.sc -    Write the objrec structure to ii_objects
**
** Defines:
**
**      IIIEwio_WriteIIObjects()      - populate ii_objects catalog
**
** History:
**      18-aug-1993 (daver)
**		First Written.
**	02-oct-1993 (daver)
**		String 'now' works if datatype is date. Unfortunately,
**		ii_objects stores dates as strings, so 'now' is 'now'. Call 
**		a UG function to find out what time it is now, and use
**		that to populate the ii_objects.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/* # line 45 "ieiiobjs.sc" */	/* host code */
/*{
** Name: IIIEwio_WriteIIObjects		- populate ii_objects catalog
**
** Description:
**
** Write the objrec struct to ii_objects. No file i/o here;
** fp, inbuf still pointing to line with object on it. If performance
** becomes an issue, we *could* hold our catalog writes by storing
** representations in memory, until an entire object is read in. I doubt if
** the effort required will be worth it.
**
** Input:
**	IEOBJREC  *objrec	pointer to structure containing object info
**	i4        appid		application id object belongs to
**	char      *appowner	app. owner (object becomes owned by them)
**
** Output:
**	None
**
** Returns:
**	OK
**	Ingres status
**
** Side Effects:
**	Populates the catalogs (either updates or inserts) the object
**	described by the data in the objrec structure.
**
** History:
**	18-aug-1993 (daver)
**		First Written/Documented.
**	02-oct-1993 (daver)
**		String 'now' works if datatype is date. Unfortunately,
**		ii_objects stores dates as strings, so 'now' is 'now'. Call 
**		a UG function to find out what time it is now, and use
**		that to populate the ii_objects.
*/
STATUS
IIIEwio_WriteIIObjects(objrec, appid, appowner)
IEOBJREC	*objrec;
  i4 appid;
  char *appowner;
{
	i4	rectype;			/* good ol' record type! */
  char *shortrem;
  char *longrem;
  i4 objid;
  i4 objclass;
  char *objname;
  char now_date[TM_SIZE_STAMP+2];
	STATUS	rval;
	if (IEDebug)
		SIprintf("\nIn WriteIIObjects: %s  %d:%s; appid=%d\n\n",
		objrec->name, objrec->class,
			IICUrtnRectypeToName(objrec->class), appid);
	/* set up the char strings for updating */
	shortrem = objrec->short_remark;
	longrem = objrec->long_remark;
	objid = objrec->id;
	objclass = objrec->class;
	objname = objrec->name;
	STcopy(UGcat_now(),now_date);
	if (objrec->update)
	{
		/*
		** First update ii_objects with the info in objrec.
		** then update the longremarks. 
		**
		** Change the ownership of the object as well. This
		** will make ALL items owned by the central, czarist
		** application owner. IF we were to allow folks to 
		** specify the ownership of these objects, we'd set the
		** appowner parameter in the iiimport module, and allow the
		** new owner name to propagate itself around.
		*/
/* # line 128 "ieiiobjs.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_objects set object_owner=");
    IIputdomio((short *)0,1,32,0,appowner);
    IIwritio(0,(short *)0,1,32,0,(char *)", short_remark=");
    IIputdomio((short *)0,1,32,0,shortrem);
    IIwritio(0,(short *)0,1,32,0,(char *)", alter_date=");
    IIputdomio((short *)0,1,32,0,now_date);
    IIwritio(0,(short *)0,1,32,0,(char *)
", alter_count=alter_count +1, last_altered_by=dbmsinfo('username')whe\
re object_id=");
    IIputdomio((short *)0,1,30,4,&objid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and object_name=");
    IIputdomio((short *)0,1,32,0,objname);
    IIwritio(0,(short *)0,1,32,0,(char *)"and object_class=");
    IIputdomio((short *)0,1,30,4,&objclass);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 138 "ieiiobjs.sc" */	/* host code */
		rval = FEinqerr();
		if (rval != OK)
			return (rval);
		if (*longrem != EOS)
		{
/* # line 144 "ieiiobjs.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_longremarks set long_remark=");
    IIputdomio((short *)0,1,32,0,longrem);
    IIwritio(0,(short *)0,1,32,0,(char *)"where object_id=");
    IIputdomio((short *)0,1,30,4,&objid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 148 "ieiiobjs.sc" */	/* host code */
			rval = FEinqerr();
			if (rval != OK)
				return (rval);
		}
	}
	else
	{
		/*
		** New objects are ALWAYS added as if the application
		** owner inserted them, which will allow for greater
		** consistancy (all objects owned by the same user id).
		*/
/* # line 160 "ieiiobjs.sc" */	/* repeat insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"ieiiobjs1",6975,10873);
      IIputdomio((short *)0,1,30,4,&objid);
      IIputdomio((short *)0,1,30,4,&objclass);
      IIputdomio((short *)0,1,32,0,objname);
      IIputdomio((short *)0,1,32,0,appowner);
      IIputdomio((short *)0,1,32,0,shortrem);
      IIputdomio((short *)0,1,32,0,now_date);
      IIputdomio((short *)0,1,32,0,now_date);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"ieiiobjs1",6975,10873);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_objects(object_id, object_class, object_name, object_o\
wner, object_env, is_current, short_remark, object_language, create_da\
te, alter_date, alter_count, last_altered_by)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&objid);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&objclass);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,objname);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,appowner);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, 0, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,shortrem);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,now_date);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,now_date);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, dbmsinfo('username'\
))");
        IIexDefine(0,(char *)"ieiiobjs1",6975,10873);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 170 "ieiiobjs.sc" */	/* host code */
		rval = FEinqerr();
		if (rval != OK)
			return (rval);
		if (*longrem != EOS)
		{
/* # line 176 "ieiiobjs.sc" */	/* repeat insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"ieiiobjs2",6976,22965);
      IIputdomio((short *)0,1,30,4,&objid);
      IIputdomio((short *)0,1,32,0,longrem);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"ieiiobjs2",6976,22965);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_longremarks(object_id, remark_sequence, remark_languag\
e, long_remark)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&objid);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, 0, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,longrem);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"ieiiobjs2",6976,22965);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 182 "ieiiobjs.sc" */	/* host code */
			rval = FEinqerr();
			if (rval != OK)
				return (rval);
		}
	}
	return OK;
}
