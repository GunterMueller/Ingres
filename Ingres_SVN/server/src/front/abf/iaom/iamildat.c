# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 1989, 2004 Ingres Corporation
*/
#include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
# include <ooclass.h>
#include	<oocat.h>
/**
** Name:	iamildat.sc -	Return Date for Interpreted Frame Object
**					from Catalogs.
** Description:
**	Contains a routine that returns the catalog date for an interpreted
**	frame object.  Defines:
**
**	IIAMfdFidDate()	get catalog date for IL object.
**
** History:
**	Revision 6.2  89/08  wong
**	Initial revision.	(For JupBug #7226.)
**/
/*{
** Name:	IIAMfdFidDate() - Get Catalog Date for Interpreted Frame Object.
**
** Description:
**	Given an ID for an IL object, return the last altered date for
**	the object from the system catalogs.
**
** Input:
**	objid	{OOID}  The IL object ID.
**	date	{char []}  The reference to the date to be returned.
**
** Output:
**	date	{char []}  The catalog last altered date.
**
** Returns:
**	{STATUS}  OK when a date is read.
**		  FAIL if the object does not exist.
** History:
**	08/89 (jhw) -- Written for JupBug #7227.
*/
STATUS
IIAMfdFidDate ( objid, date )
  OOID objid;
  char date[OODATESIZE+1];
{
	STATUS	rval;
/* # line 57 "iamildat.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamildat1",6608,21917);
      IIputdomio((short *)0,1,30,4,&objid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"iamildat1",6608,21917);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select alter_date from ii_objects, ii_encodings where ii_objects.obje\
ct_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&objid);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and ii_encodings.encode_object=ii_objects.object_id and encode_sequen\
ce=0");
        IIexDefine(0,(char *)"iamildat1",6608,21917);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,OODATESIZE+1-1,date);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 63 "iamildat.sc" */	/* host code */
	if ( (rval = FEinqerr()) != OK )
		return rval;
	else if ( FEinqrows() != 1 )
		return FAIL;
	else
		return OK;
}
