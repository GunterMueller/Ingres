# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ergr.h>
# include	<er.h>
# include <ooclass.h>
# include	<oodefine.h>
# include	<uigdata.h>
/**
** Name:
**	grnameok.qc	- validate graph name
**
** Description:
**	Determines whether a given string is a valid graph name
**	by checking the format for being a legal Ingres name
**	checking the database to see if a graph of that name
**	currently exists.
**
**	This file defines:
**
**	GRnameOk(graphClass, name)
**
** Called:
**	via object utility Send.
**
** Returns:
**	bool:	TRUE if name is valid and useable.
**		FALSE if name is invalid or conflicts with existing
**			graph.
**
** History:
**	8-aug-1988 (danielt)
**		changed to just check for the ii_objects catalog
**		for an object of type OC_GRAPH with the given owner
**		and name.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
bool
GRnameOk(graphClass, name)
  OO_CLASS *graphClass;
  char *name;
{
  i4 dummy_id;
  char *user = IIuiUser;
	bool	found = FALSE;
	if (FEchkname(name, FALSE) != OK) 
	{
		IIUGmsg(ERget(E_GR000A_invalid_name), TRUE, 1, name);
		return (FALSE);
	}
/* # line 69 "grnameok.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select object_id from ii_objects where object_name=");
    IIputdomio((short *)0,1,32,0,name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and object_owner=");
    IIputdomio((short *)0,1,32,0,user);
    IIwritio(0,(short *)0,1,32,0,(char *)"and object_class=OC_GRAPH");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dummy_id);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 76 "grnameok.sc" */	/* host code */
		found = TRUE;
/* # line 77 "grnameok.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 79 "grnameok.sc" */	/* host code */
	if (found) 
	{
		IIUGmsg(ERget(E_GR000B_already_exists),TRUE,2,name,
			IIuiUser);
		return (FALSE);
	}
	return (TRUE);
}
