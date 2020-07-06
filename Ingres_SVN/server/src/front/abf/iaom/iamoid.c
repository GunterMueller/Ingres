# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 1987, 2004 Ingres Corporation
*/
#include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<cv.h>		/* 6-x_PC_80x86 */
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
# include <ooclass.h>
#include	<ilerror.h>
/**
** Name:	iamoid.sc -	Application Fetch Object ID Routine.
**
** Description:
**	Contains a routine that fetches the ID for an application object
**	or component object.  Defines:
**
**	IIAMoiObjId()	fetch application object id.
**	iiamWithName()	fetch application component id by name.
**
** History:
**      Revision 6.4/01
**      10/02/91 (jillb--DGC)
**              Changed exec include for ooclass.sh to ## include ooclass.qsh
**              to prevent rebuilding everything everytime.
**
**	Revision 6.3/03/00  89/07  wong
**	Abstracted functionality into 'iiamWithName()', which can be called
**	from the 'iiamDb*Name()' method routines in "iammeth.qsc".
**
**	Revision 6.2  89/03  wong
**	Added support for all types of application components.
**
**	Revision 6.0  87/05  bobm
**	Initial revision.
*/
/*{
** Name:	IIAMoiObjId() -	Fetch Application Object ID.
**
** Description:
**	Fetch ID for application object or component object.  If the input class
**	is not an application (OC_APPL,) the application ID must be provided,
**	since the application is required for uniqueness.
**
**	If the input class is a 4GL frame (OC_OSLFRAME,) a `frame' class ID will
**	be returned.  If the input class is a 4GL procedure (OC_OSLPROC,) a
**	`proc' class ID will be returned.  For other input classes, the returned
**	ID will have a class identical to the input class.
**
** Inputs:
**	oname	{char *}  object name.
**	inclass	{OOID}  desired object class, may actually indicate several
**				classes (if OC_OSLFRAME or OC_OSLPROC.)
**	apid	{OOID}  application ID, unused if inclass = OC_APPL
**
** Outputs:
**	oid	{OOID *}  Object ID.
**	oclass	{OOID *}  returned object class.
**
** Return:
**	{STATUS}	OK		success
** 			ILE_NOOBJ	doesn't exist
** 			ILE_FAIL	DB query failed
**
** History:
**	5/87 (bobm)	written
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	07/89 (jhw)  Modified to call 'iiamWithName()' and now errors on
**		multiple rows.
**	11/89 (jhw)  Added global and constant support.
**	26-oct-93 (robf) Changed iclass to inclass to avoid CMW define problem
**	                 while porting.
*/
STATUS
IIAMoiObjId ( oname, inclass, apid, oid, oclass ) 
char	*oname;
OOID	inclass;
OOID	apid;
OOID	*oid;
OOID	*oclass;
{
	OOID	cur_id;
	OOID	cur_class;
  OOID new_id;
  OOID new_class;
  char objname[FE_MAXNAME + 1];
	_VOID_ STlcopy(oname, objname, sizeof(objname)-1);
	CVlower(objname);
	/*
	** NOTE: in the application case, we wind up not doing a join.
	*/
	*oid = new_id = cur_id = OC_UNDEFINED;
	if ( inclass == OC_APPL )
	{
		/*cur_id = OOsnd(OC_APPL, _withName, objname, _);*/
/* # line 104 "iamoid.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select object_id, object_class from ii_objects where object_name=");
    IIputdomio((short *)0,1,32,0,objname);
    IIwritio(0,(short *)0,1,32,0,(char *)"and object_class=");
    IIputdomio((short *)0,1,30,sizeof(OC_APPL),(void *)IILQint(OC_APPL));
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&new_id);
      IIgetdomio((short *)0,1,30,4,&new_class);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 108 "iamoid.sc" */	/* host code */
		cur_id = new_id;
		cur_class = new_class;
	}
	else
	{
		switch (inclass)
		{
		  case OC_OSLFRAME:
			cur_id = iiamWithName( apid, objname, &cur_class,
						OC_APPLFRAME,
						OC_UNDEFINED,
						OC_UNDEFINED,
						OC_UNDEFINED,
						OC_UNDEFINED,
						OC_UNDEFINED
			);
			break;
		  case OC_OSLPROC:
			cur_id = iiamWithName( apid, objname, &cur_class,
						OC_HLPROC,
						OC_OSLPROC,
						OC_DBPROC,
						OC_UNPROC,
						OC_UNDEFINED,
						OC_UNDEFINED
			);
			break;
		  case OC_GLOBAL:
		  case OC_CONST:
			cur_id = iiamWithName( apid, objname, &cur_class,
						OC_GLOBAL,
						OC_CONST,
						OC_UNDEFINED,
						OC_UNDEFINED,
						OC_UNDEFINED,
						OC_UNDEFINED
			);
			break;
		  default:
			cur_id = iiamWithName( apid, objname, &cur_class,
						inclass,
						OC_UNDEFINED,
						OC_UNDEFINED,
						OC_UNDEFINED,
						OC_UNDEFINED,
						OC_UNDEFINED
			);
			break;
		} /* end switch */
	}
	if ( FEinqerr() != OK )
		return ILE_FAIL;
	if ( cur_id <= 0 )
		return ILE_NOOBJ;
	*oid = cur_id;
	*oclass = cur_class;
	return OK;
}
/*{
** Name:	iiamWithName() -	Fetch Application Component ID by Name.
**
** Description:
**	Returns an ID (and class) for an application component given by name
**	and a set of input classes.  Since these classes are inclusive of the
**	name space for a given object, only one or no object should be found.
**
** Input:
**	appid	{OOID}  Application ID.
**	name	{char *}  The name of the object.
**	c1-c6	{OOID}  The class(es) to which the named object should belong.
**				Note the frame class, OC_APPLFRAME stands for
**				all frame classes.
** Outputs:
**	class	{OOID *}  The class of the component object.
**
** Returns:
**	{OOID}	The ID of the object.
**
** History:
**	07/89 (jhw) -- Written.
**	03/90 (jhw) -- Special case escape for QBF names.
**	05/90 (jhw) -- Open/SQL fix:  Cannot use ":var = :var".  JupBug #21267.
**	17-mar-94 (rudyw)
**		Fix for 59810. Extract the constant comparison from a repeated
**		select where clause. Use the constant comparison to set up a 
**		conditional clause with two separate selects which each can be
**		handled by the optimizer in a way that is best for performance.
*/
OOID
iiamWithName ( appid, name, class, c1, c2, c3, c4, c5, c6 )
  OOID appid;
  char *name;
  OOID *class;
  OOID c1;
  OOID c2;
  OOID c3;
  OOID c4;
  OOID c5;
  OOID c6;
{
  OOID id;
	OOID	oclass;
	if ( class == NULL )
		class = &oclass;
	*class = OC_UNDEFINED;
	if ( c1 == 2200 ) /* :OC_APPLFRAME, but not Open/SQL. */
	{
/* # line 219 "iamoid.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamoid1",6613,20468);
      IIputdomio((short *)0,1,32,0,name);
      IIputdomio((short *)0,1,30,sizeof(OC_APPLFRAME),(void *)IILQint(
      OC_APPLFRAME));
      IIputdomio((short *)0,1,30,sizeof(OC_APPLFRAME),(void *)IILQint(
      OC_APPLFRAME));
      IIputdomio((short *)0,1,30,sizeof(OC_QBFNAME),(void *)IILQint(OC_QBFNAME
      ));
      IIputdomio((short *)0,1,30,4,&c1);
      IIputdomio((short *)0,1,30,4,&c2);
      IIputdomio((short *)0,1,30,4,&c3);
      IIputdomio((short *)0,1,30,4,&c4);
      IIputdomio((short *)0,1,30,4,&c5);
      IIputdomio((short *)0,1,30,4,&c6);
      IIputdomio((short *)0,1,30,4,&appid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"iamoid1",6613,20468);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct obj.object_id, object_class from ii_abfobjects app, i\
i_objects obj where object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and((object_class>=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,sizeof(OC_APPLFRAME),(void *)IILQint(
        OC_APPLFRAME));
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_class<=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,sizeof(OC_APPLFRAME),(void *)IILQint(
        OC_APPLFRAME));
        IIwritio(0,(short *)0,1,32,0,(char *)" +99 and object_class<>");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,sizeof(OC_QBFNAME),(void *)IILQint(
        OC_QBFNAME));
        IIwritio(0,(short *)0,1,32,0,(char *)")or object_class in(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&c1);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&c2);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,30,4,&c3);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,30,4,&c4);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,30,4,&c5);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $9=");
        IIputdomio((short *)0,1,30,4,&c6);
        IIwritio(0,(short *)0,1,32,0,(char *)
"))and obj.object_id=app.object_id and applid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $10=");
        IIputdomio((short *)0,1,30,4,&appid);
        IIexDefine(0,(char *)"iamoid1",6613,20468);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id);
      IIgetdomio((short *)0,1,30,4,class);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 230 "iamoid.sc" */	/* host code */
	}
	else
	{
/* # line 233 "iamoid.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamoid2",6614,28885);
      IIputdomio((short *)0,1,32,0,name);
      IIputdomio((short *)0,1,30,4,&c1);
      IIputdomio((short *)0,1,30,4,&c2);
      IIputdomio((short *)0,1,30,4,&c3);
      IIputdomio((short *)0,1,30,4,&c4);
      IIputdomio((short *)0,1,30,4,&c5);
      IIputdomio((short *)0,1,30,4,&c6);
      IIputdomio((short *)0,1,30,4,&appid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"iamoid2",6614,28885);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct obj.object_id, object_class from ii_abfobjects app, i\
i_objects obj where object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_class in(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&c1);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&c2);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&c3);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&c4);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&c5);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,30,4,&c6);
        IIwritio(0,(short *)0,1,32,0,(char *)
")and obj.object_id=app.object_id and applid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,30,4,&appid);
        IIexDefine(0,(char *)"iamoid2",6614,28885);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id);
      IIgetdomio((short *)0,1,30,4,class);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 240 "iamoid.sc" */	/* host code */
	}
	return ( FEinqerr() != OK || FEinqrows() == 0 ) ? nil : id;
}
