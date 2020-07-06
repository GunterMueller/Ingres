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
# include	<fe.h>
# include <ooclass.h>
# include	<cf.h>
/*{
** Name:	cf_delobj - delete an object from the database
**
** Description:
**	delete a form-related object from the database.
**
** Input params:
**	class		OO class of the object to delete.
**	id		OOID of the object to delete.
**
** Output params:
**	none.
**
** Returns:
**	none.
**
** Exceptions:
**	none.
**
** Side Effects:
**
** History:
**	22-Apr-87 (rdesmond) - written.
**	03-nov-87 (danielt) changed QUEL statements to vanilla SQL
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
*/
VOID
cf_delobj(class, id)
  i4 class;
  i4 id;
{
	iiuicw1_CatWriteOn();
/* # line 51 "cfdelobj.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_objects where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIsyncup((char *)0,0);
  }
/* # line 53 "cfdelobj.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_longremarks where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIsyncup((char *)0,0);
  }
/* # line 54 "cfdelobj.sc" */	/* host code */
	switch(class)
	{
		case OC_QBFNAME:
/* # line 58 "cfdelobj.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_qbfnames where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIsyncup((char *)0,0);
  }
/* # line 59 "cfdelobj.sc" */	/* host code */
			break;
		case OC_JOINDEF:
/* # line 62 "cfdelobj.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_joindefs where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIsyncup((char *)0,0);
  }
/* # line 63 "cfdelobj.sc" */	/* host code */
			break;
		case OC_FORM:
/* # line 66 "cfdelobj.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_forms where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIsyncup((char *)0,0);
  }
/* # line 68 "cfdelobj.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_fields where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIsyncup((char *)0,0);
  }
/* # line 70 "cfdelobj.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_trim where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIsyncup((char *)0,0);
  }
/* # line 72 "cfdelobj.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_encoded_forms where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIsyncup((char *)0,0);
  }
/* # line 73 "cfdelobj.sc" */	/* host code */
			break;
	}
	iiuicw0_CatWriteOff();
}
