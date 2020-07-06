# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<si.h>
# include	<lo.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include <ooclass.h>
# include	<cf.h>
#include	"ercf.h"
/*{
** Name:	CF_EXISTS - given an object class, name, and owner, check
**			to see if it already exists in the database.
**
** Description:
**
** Input params:
**	class		OO class of the object.
**	name		object name.
**	owner		owner of the object.
**	id		pointer to id,	which is set to OOID if exists;
**			may be null if return ID is not needed.
**
** Output params:
**	id		pointer to id,	which is set to OOID if exists;
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
**	03-nov-87 (danielt) - changed QUEL retrieve loop to singleton
**	    SQL select
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	01/25/90 (dkh) - Changes for new LO on VMS.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
bool
cf_objexist(class, name, owner, id)
  i4 class;
  char *name;
  char *owner;
i4	*id;
{
	bool	exists;
  i4 rcount;
  i4 obj_id = 0;
	if (name == NULL || owner == NULL)
		return(FALSE);
	exists = FALSE;
/* # line 74 "cfobjexi.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select object_id from ii_objects where object_class=");
    IIputdomio((short *)0,1,30,4,&class);
    IIwritio(0,(short *)0,1,32,0,(char *)"and object_name=");
    IIputdomio((short *)0,1,32,0,name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and object_owner=");
    IIputdomio((short *)0,1,32,0,owner);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&obj_id);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 80 "cfobjexi.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&rcount,8);
  }
/* # line 81 "cfobjexi.sc" */	/* host code */
	if (rcount != 0)
		exists = TRUE;
	if (id != (i4 *)NULL)
		*id = obj_id;
	return(exists);
}
bool
cf_conflict(filename, owner)
char	*filename;
char	*owner;
{
	char		inbuf[750];
	FILE		*fd;
	LOCATION	loc;
	char		locbuf[MAX_LOC + 1];
	/* Open intermediate file */
	STcopy(filename, locbuf);
	LOfroms(PATH & FILENAME, locbuf, &loc);
	if (SIopen(&loc, ERx("r"), &fd) != OK)
	{
	    IIUGerr(E_CF0000_CANNOT_OPEN_FILE, 0, 1, filename);
	    return(TRUE);
	}
	for (;;)
	{	/* read until end of file reached */
		if (SIgetrec(inbuf, 750, fd) == ENDFILE)
			break;		/* end of file */
		/* only the hdr rec types contain object names to check */
		switch(cf_rectype(inbuf))
		{
			case RT_QBFHDR:
				cf_gettok(inbuf);
				if (cf_objexist(OC_QBFNAME, cf_gettok(NULL),
				    owner, NULL))
					return(TRUE);
				break;
			case RT_JDEFHDR:
				cf_gettok(inbuf);
				if (cf_objexist(OC_JOINDEF, cf_gettok(NULL),
				    owner, NULL))
					return(TRUE);
				break;
			case RT_FORMHDR:
				cf_gettok(inbuf);
				if (cf_objexist(OC_FORM, cf_gettok(NULL),
				    owner, NULL))
					return(TRUE);
				break;
		}
	}
	SIclose(fd);
	return(FALSE);
}
