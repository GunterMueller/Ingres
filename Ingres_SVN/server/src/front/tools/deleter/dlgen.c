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
# include <ooclass.h>
# include	<er.h>
# include	<cu.h>
# include	<ug.h>
/*
** Name:	dl_gen		Delete a generic object type.
**
** Description:
**	Deletes a generic object type using the copyutil delete
**	routines.  This will be done for most object types,
**	such as graphs, reports, etc.  Applications and forms
**	need to be treated specially at this time.
**
**	The names etc come from a retrieve of the ii_objects table,
**	so they should exist (except for any concurrency issues).
**
** Inputs:
**	oname	- name of application.
**	oowner  - owner name of application.
**	oclass  - class id of application.
**	oid	- id of application
**
** Outputs:
**	STATUS of any database errors that may have occurred.
**
** History:
**	19-nov-1987 (peter)
**		Rewritten for 6.0.
*/
STATUS
dl_gen(oname, oowner, oclass, oid)
char		*oname;
char		*oowner;
OOID		oclass;
OOID		oid;
{
	STATUS		retstat;
	STATUS		IICUdoDeleteObject();
	retstat = IICUdoDeleteObject(oclass, oid);
	return(retstat);
}
