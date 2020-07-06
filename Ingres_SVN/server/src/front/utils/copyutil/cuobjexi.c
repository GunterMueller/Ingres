# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<si.h>
# include	<lo.h>
# include <ooclass.h>
# include	<cu.h>
# include	<er.h>
# include	"ercu.h"
# include 	<uigdata.h>
/*{
** Name:	CU_EXISTS - given an object class, name, and owner, check 
**			to see if it already exists in the database.
**
** Description:
**
** Input params:
**	class		OO class of the object.
**	name		object name.
**	id		pointer to id,  which is set to OOID if exists;
**			may be null if return ID is not needed.
**
** Output params:
**	id		pointer to id,  which is set to OOID if exists;
**
** Returns:
**	TRUE		if objects exists.
**	FALSE		if objects does not exist.
**
** Exceptions:
**	none
**
** Side Effects:
**
** History:
**	22-Apr-87 (rdesmond) - written.
**	3-jul-1987 (Joe)
**		Modified for copy utility.
**	2-feb-1988 (Joe)
**		Fix for BUG 1892. Applications don't follow
**		the normal ownership rules of other FE objects
**		since an application is global to the database.
**		(How could I have forgotten that?).
**		Also, make the retrieve a repeat and took out
**		the range statement.
**	05-sep-88 (kenl)
**		Changed QUEL to SQL.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
bool
cu_objexist(class, name, id)
  i4 class;
  char *name;
i4	*id;
{
	bool	exists;
  i4 obj_id;
  char *owner;
  i4 hiid;
  i4 lowid;
	/*
	** Special case report since it is the only class
	** that uses a range of classes.
	*/
	hiid = lowid = class;
	if (class == OC_REPORT)
	{
	    hiid = OC_RBFREP;
	}
	/*
	** If the class is application, then the owner
	** isn't important since the application is global.
	** Look for any owner.
	*/
	if (class == OC_APPL)
	    owner = "%";
	else
	    owner = IIUIdbdata()->user;
	if (name == NULL)
		return(FALSE);
	exists = FALSE;
/* # line 102 "cuobjexi.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cuobjexi1",6483,6334);
      IIputdomio((short *)0,1,30,4,&lowid);
      IIputdomio((short *)0,1,30,4,&hiid);
      IIputdomio((short *)0,1,32,0,name);
      IIputdomio((short *)0,1,32,0,owner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cuobjexi1",6483,6334);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select object_id from ii_objects where object_class>=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&lowid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_class<=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&hiid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_owner like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,owner);
        IIexDefine(0,(char *)"cuobjexi1",6483,6334);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&obj_id);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 106 "cuobjexi.sc" */	/* host code */
		exists = TRUE;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if (id != (i4 *)NULL)
		*id = obj_id;
	return(exists);
}
/*{
** Name:	cu_gbfexist	- See if a gbf graph exists.
**
** Description:
**	Checks to see if a gbf graph exists.
**
**	If the GBF catalogs don't exists it just returns FALSE.
**	Note that since GBF graphs are old, the owner is part
**	of the key.
**
** Inputs:
**	class		The class.
**
**	name		The name of the graph.
**
**	owner		The owner of the graph.
**
** Outputs:
**	Returns:
**		TRUE is graph exists.
**		FALSE otherwise.
** History:
**	7-aug-1987 (Joe)
**		Initial Version.
**	2-feb-1988 (Joe)
**		Took out range statements and made
**		retrieve repeat.
**	9-sept-92 (blaise)
**		Added new argument to FErelexists().
*/
bool
cu_gbfexist(class, name, owner)
i4	class;
  char *name;
  char *owner;
{
  i4 c;
    if (FErelexists(ERx("gcommands"), NULL) != OK)
    {
	return FALSE;
    }
/* # line 157 "cuobjexi.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cuobjexi2",6484,6396);
      IIputdomio((short *)0,1,32,0,name);
      IIputdomio((short *)0,1,32,0,owner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"cuobjexi2",6484,6396);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(distinct graphid)from graphs where graphid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and gowner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,owner);
        IIexDefine(0,(char *)"cuobjexi2",6484,6396);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&c);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 161 "cuobjexi.sc" */	/* host code */
    return (c >= 1 ? TRUE : FALSE);
}
