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
# include	<oodefine.h>
# include <oocat.h>
/**
** Name:	ooisindb.qc	- test if object is in database
**
** Description:
**	This file defines:
**
**	OOisInDb()	Tests whther object is stored in database by
**			retrieving from ii_objects.
**
** History:
**	6/22/87 (peterk) - created from routine in ob.qc
**	10/23/1987 (danielt) - SQLized the retrieve loop
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
i4
OOisInDb(self)
  OO_OBJECT *self;
{
  OOID dummy_id;
	i4	in_db = FALSE;
/* # line 44 "ooisindb.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"ooisindb1",6307,16782);
      IIputdomio((short *)0,1,30,4,&self->ooid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"ooisindb1",6307,16782);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select object_id from ii_objects where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&self->ooid);
        IIexDefine(0,(char *)"ooisindb1",6307,16782);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dummy_id);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 49 "ooisindb.sc" */	/* host code */
	    in_db = TRUE;
/* # line 50 "ooisindb.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 52 "ooisindb.sc" */	/* host code */
	return  (in_db);
}
