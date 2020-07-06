# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 1987, 2004 Ingres Corporation
*/
#include	<compat.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<oodep.h>
# include <ooclass.h>
# include <uigdata.h>
#include	<acatrec.h>
#include	"iamint.h"
/**
** Name:	iamdpad.sc -	ABF Dependency Catalog Module.
**
** Description:
**	Contains routines to add and delete records from the ABF dependency
**	catalog.  This module is internal to IAOM.  Defines:
**
**	IIAMadlAddDepList()	insert dependency list into catalog.
**	iiamAddDep()		insert dependency for component into catalog.
**	IIAMdpDeleteDeps()	delete dependencies from catalog.
**	iiamRrdReadRecordDeps() read record rependencies for application.
**
** History:
**
**	Revision 6.0  87/10  bobm
**	Initial revision (extracted from "iamcadd.qc.")
**
**	Revision 6.1  88/08/18  kenl
**	Changed QUEL to SQL.
**
**	Revision 6.2  89/01  wong
**	Added 'IIAMdpDeleteDeps()' and abstracted 'iiamAddDep()'.
**
**	7/90 (Mike S)
**	Redid for dependency manager.  Application ID now goes into
**	ii_abfdependencies, and we don't need dummy dependencies.
**
**	Revision 6.5
**	18-may-93 (davel)
**		Added new argument to iiamAddDep() for depended-upon 
**		object owner. Also modify iiamRrdReadRecordDeps() to read
**		the object owner into the ACATDEP structs.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
char *fid_name();
GLOBALREF char *Catowner;
/*{
** Name:	IIAMadlAddDepList() -	Add Dependency Records.
**
** Description:
**	add dependency records to database for a known object.
**	Note that this routine is not usable by ABF, since it can't
**	update the DM graph.  It's only provided for OSL, copyapp, and
**	abfto60.
**
**	Caller handles any transaction management.
**
** Inputs:
**	dep	dependency list.
**	applid	application id
**	oid	id of object depending on dep.
**
** Returns:
**	{STATUS}	OK		success
**			FAIL		database write failure
**
** History:
**	10/87 (bobm)	extracted from iamcadd.qc
**	18-aug-88 (kenl)
**		Changed QUEL to SQL.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	01/89 (jhw) -- Abstracted out 'iiamAddDep()'.
**	7/90 (Mike S) -- changed name from IIAMdpAddDeps
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
*/
STATUS
IIAMadlAddDepList ( dep, applid, oid )
register ACATDEP	*dep;
OOID			applid;
OOID			oid;
{
	OO_OBJECT odummy;
	/*
	** We no longer need dummy dependencies, since we've eliminated the
	** 3-way join.
	*/
	for ( ; dep != NULL; dep = dep->next )
	{
		STATUS	rval;
		odummy.ooid = oid;
		odummy.class = OC_UNDEFINED;
		if (dep->class == OC_ILCODE)
			dep->name = fid_name(dep->id);
		if ( (rval =
			iiamAddDep(&odummy, applid, dep->name, dep->class, 
				dep->deptype, dep->owner, dep->deplink, 
				dep->deporder))
				!= OK )
			return rval;
	}
	return OK;
}
/*{
** Name:	iiamAddDep() -	Insert Dependency for Component into Catalog.
**
** Description:
**	Insert a dependency for an ABF component object.  The user is the
**	current one.
**	
**	If the object_class of "comp" is OC_UNDEFINED, don't put the
**	dependency into the graph.
**
** Inputs:
**	comp		{OO_OBJECT *} ABF application component object
**	applid		{OOID}  The ABF application object id.
**	dep_name	{char *}  The name of the depended on object.
**	dep_class	{OOID}  The class of the depended on object.
**	dep_type	{OOID}  The type of dependency.
**	dep_owner	{char *}  The owner of the depended on object.
**	dep_link	{char *}  The dependency link name (for a menu type).
**	dep_order	{nat } The dependency order (for a menu type).
**
** Returns:
**	{STATUS}  The DBMS query status for INSERT INTO.
**
** History:
**	01/89 (jhw)  Abstracted from 'IIAMdepApp()'.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	18-may-93 (davel)
**		Added owner argument for depended-upon object owner.
**		This is usually passed in as NULL 
**		but can also be the explicit owner of the
**		object.  Currently the only case of this is for the "type of
**		table x.y" construct, where the dependency owner will be
**		the table owner ("x" in the above example).  For all other
**		cases when NULL is passed in, default to the current user.
*/
STATUS
iiamAddDep ( comp, applid, dep_name, dep_class, dep_type, dep_owner, 
	     dep_link, dep_order )
  OO_OBJECT *comp;
  OOID applid;
  char *dep_name;
  OOID dep_class;
  OOID dep_type;
  char *dep_owner;
  char *dep_link;
  i4 dep_order;
{
	if (dep_owner == (char *)NULL)
	{
		dep_owner = (dep_type == OC_DTTABLE_TYPE) 
			    ? ERx("") : IIUIdbdata()->user;
	}
/* # line 186 "iamdpad.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamdpad1",6609,15068);
      IIputdomio((short *)0,1,30,4,&comp->ooid);
      IIputdomio((short *)0,1,30,4,&applid);
      IIputdomio((short *)0,1,32,0,dep_name);
      IIputdomio((short *)0,1,32,0,dep_owner);
      IIputdomio((short *)0,1,30,4,&dep_class);
      IIputdomio((short *)0,1,30,4,&dep_type);
      IIputdomio((short *)0,1,32,0,dep_link);
      IIputdomio((short *)0,1,30,4,&dep_order);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamdpad1",6609,15068);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_abfdependencies(object_id, abfdef_applid, abfdef_name,\
 abfdef_owner, object_class, abfdef_deptype, abf_linkname, abf_deporde\
r)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&comp->ooid);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&applid);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,dep_name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,dep_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&dep_class);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&dep_type);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,dep_link);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,30,4,&dep_order);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iamdpad1",6609,15068);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 195 "iamdpad.sc" */	/* host code */
	/* Add to graph, as well */
	if (comp->class != OC_UNDEFINED)
		iiamDMaAddDep(comp, dep_name, dep_class, dep_type, 
			      dep_link, dep_order);
	return FEinqerr();
}
/*{
** Name:	IIAMdpDeleteDeps() -	Delete Dependencies from Catalog.
**
** Description:
**	Delete the dependencies for an ABF component object.  Either all
**	dependencies will be removed, or just a certain type of dependencies
**	as specified by the input parameter.
**
** Inputs:
**	comp		{OO_OBJECT *}  The ABF application component object.
**	depty1,2,3	{OOID}  The dependency type to be deleted,
**				all if depty1 = OC_UNDEFINED.
**
** Returns:
**	{STATUS}  The DBMS query status for DELETE FROM.
**
** History:
**	01/89 (jhw)  Written.
*/
STATUS
IIAMdpDeleteDeps ( comp, depty1, depty2, depty3 )
  OO_OBJECT *comp;
  OOID depty1;
  OOID depty2;
  OOID depty3;
{
/* # line 233 "iamdpad.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamdpad2",6610,19340);
      IIputdomio((short *)0,1,30,4,&comp->ooid);
      IIputdomio((short *)0,1,30,4,&depty1);
      IIputdomio((short *)0,1,30,4,&depty1);
      IIputdomio((short *)0,1,30,4,&depty2);
      IIputdomio((short *)0,1,30,4,&depty3);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamdpad2",6610,19340);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_abfdependencies dep where dep.object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&comp->ooid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&depty1);
        IIwritio(0,(short *)0,1,32,0,(char *)"= -1 or dep.abfdef_depty\
pe=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&depty1);
        IIwritio(0,(short *)0,1,32,0,(char *)"or dep.abfdef_deptype=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&depty2);
        IIwritio(0,(short *)0,1,32,0,(char *)"or dep.abfdef_deptype=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&depty3);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iamdpad2",6610,19340);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 240 "iamdpad.sc" */	/* host code */
	/* Delete them from dependency graph */
	iiamDMdDelDep(comp, depty1, depty2, depty3);
	return FEinqerr();
}
/*{
** Name:	iiamRrdReadRecordDeps() -	Read Record Dependencies.
**
** Description:
**	Read the record dependencies for an ABF application.  
**
** Inputs:
**
** Returns:
**	
**
** History:
**	11/89 (billc)  Written.
**	18-may-93 (davel)
**		Select the depended-upon object owner also, and store in
**		the ACATDEP struct..
*/
ACATDEP *
iiamRrdReadRecordDeps ( applid, tag, stat )
  OOID applid;
TAGID	tag;
STATUS	*stat;
{
  char dname[FE_MAXNAME + 1];
  char oname[FE_MAXNAME + 1];
  OOID class;
  OOID deptype;
  i4 deporder;
	ACATDEP	*ret = NULL;
/* # line 283 "iamdpad.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamdpad3",6611,3834);
      IIputdomio((short *)0,1,30,sizeof(OC_RECORD),(void *)IILQint(OC_RECORD));
      IIputdomio((short *)0,1,30,4,&applid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamdpad3",6611,3834);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct d.abfdef_name, d.abfdef_owner, d.object_class, d.abfd\
ef_deptype, d.abf_deporder from ii_abfdependencies d where d.object_cl\
ass=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,sizeof(OC_RECORD),(void *)IILQint(OC_RECORD
        ));
        IIwritio(0,(short *)0,1,32,0,(char *)"and d.abfdef_applid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&applid);
        IIexDefine(0,(char *)"iamdpad3",6611,3834);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,dname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,oname);
      IIgetdomio((short *)0,1,30,4,&class);
      IIgetdomio((short *)0,1,30,4,&deptype);
      IIgetdomio((short *)0,1,30,4,&deporder);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 290 "iamdpad.sc" */	/* host code */
	    	{
			ACATDEP	*tmp;
			tmp = (ACATDEP*)FEreqmem(tag, sizeof(ACATDEP), 
						FALSE, stat
					);
			if (tmp == NULL)
/* # line 297 "iamdpad.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
/* # line 299 "iamdpad.sc" */	/* host code */
			tmp->name = FEtsalloc(tag, dname);
			tmp->owner = FEtsalloc(tag, oname);
			tmp->class = class;
			tmp->deptype = deptype;
			tmp->deporder = deporder;
			tmp->next = ret;
			ret = tmp;
	    	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if (stat != NULL)
		*stat = FEinqerr();
	return ret;
}
