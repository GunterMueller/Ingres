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
#include	<fe.h>
#include	<ug.h>
#include	<ui.h>
# include <ooclass.h>
#include	<oodefine.h>
# include	"ooldef.h"
# include <oosymbol.h>
#include	"eroo.h"
/**
** Name:	id.sc -	Generic ID Allocation Module.
**
** Description:
**	Contains routines used to allocate object IDs.  Defines:
**
**	IIOOsetidDelta()	set DB ID delta.
**	IIOOnewId()		generate new object id.
**	iiooInqResult()		inquire on query status.
**
** History:
**	Revision 6.1  88/06  wong
**	Moved from "cmethod.sc" and completed support for delta ID allocation.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	12/19/89 (dkh) - VMS shared lib changes - Added include of ooldef.h
**      22-oct-90 (sandyd)
**              Fixed #include of local ooldef.h to use "" instead of <>.
**	08-jun-92 (leighb) DeskTop Porting Change:
**		Changed 'errno' to 'errnum' cuz 'errno' is a macro in WIN/NT.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
# define STD_DELTA 1
  static i4 Delta = STD_DELTA;
i4
IIOOsetidDelta ( delta )
i4	delta;
{
	i4 old;
	old = Delta;
	Delta = delta;
	return old;
}
/*{
** Name:	IIOOnewId() -	Generate new Object ID.
**
** Description:
**	Generate a new temporary or permanent object ID for the current database.
**	IDs are allocated from the DB on a block basis (where the block size is
**	given by 'Delta'.)
**
**	If 'tempidflag' is < 0, a temporary ID is generated:
**		TempId < ID
**	by decrementing TempId.
**
**	Else a permanent ID is generated:
**		MAXSYSOBJECT < ID < TempId
**	by referring to the "ii_id" table in the current database.
**
** Input params:
**	tempidflag	{nat}   if < 0 generate a temporary ID
**				else generate a permanent ID from the database
**
** Returns:
**	{OOID} 		Object ID
**
** Side Effects:
**	If a permanent ID is generated, table "ii_id" in database
**	is updated.  If there is an open transaction, it is ended.
**	  This routine creates a new transaction.
**
** History:
**  	29-dec-1987 (danielt) SQLized database access
**	10-feb-1987 (danielt) put in calls to UI transactional routines
*/
static char READONLY	_iiid[] = ERx("ii_id");
typedef struct _id {
			struct _id	*_next;
			OOID		nextId;
			OOID		lastId;
} IdInterval;
static IdInterval	Last = {NULL, 0, UNKNOWN};
OOID
IIOOnewId ()
{
	bool		IIUIinXaction();
	DB_STATUS	IIUIbeginXaction();
	DB_STATUS	IIUIendXaction();
	DB_STATUS	IIUIabortXaction();
	bool	xact = IIUIinXaction();
	while ( Last.nextId > Last.lastId )
	{ /* allocate more from DB */
		STATUS	stat;
		if ( !xact )
			_VOID_ IIUIbeginXaction();
		iiuicw1_CatWriteOn();
/* # line 112 "ooid.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_id set object_id=object_id +");
    IIputdomio((short *)0,1,30,4,&Delta);
    IIsyncup((char *)0,0);
  }
/* # line 113 "ooid.sc" */	/* host code */
		if ( (stat = iiooInqResult(ERx("REPLACE"), _iiid)) == OK )
		{ /* get new ID */
  OOID newdbLast;
/* # line 118 "ooid.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select object_id from ii_id");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&newdbLast);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 121 "ooid.sc" */	/* host code */
			if ( (stat = iiooInqResult(ERx("FETCH"), _iiid)) == OK )
			{
				i4	rows = FEinqrows();
				iiuicw0_CatWriteOff();
				if ( !xact )
					_VOID_ IIUIendXaction();
				if ( rows != 1 )
			   		IIUGerr(E_OO003E_ii_id_corrupted, UG_ERR_FATAL, 0);
				else if ( newdbLast >= (unsigned)TempId )
			   		IIUGerr(E_OO000F_No_more_ids, UG_ERR_FATAL, 0);
				else
				{
					Last.nextId = newdbLast - Delta + 1;
					Last.lastId = newdbLast;
					break;
				}
			}
		}
		if ( !xact && stat != OK && !IIUIdeadlock(stat) )
			_VOID_ IIUIabortXaction();
	} /* end while */
	return Last.nextId++;
}
OOID
IIOOtmpId()
{
	GLOBALREF i4	TempId;
	return TempId--;
}
bool
OOisTempId(id)
OOID	id;
{
	return id > (unsigned)TempId;
}
/*{
** Name:	iiooInqResult() -	Inquire on DBMS Query Status.
**
** Description:
**	Inquire on DBMS query status.  Internal to Object module (OO.)
**
** Inputs:
**	s	{char *}  Query type.
**	r	{char *}  Query table.
**
** Returns:
**	{STATUS}  Query status.
*/
STATUS
iiooInqResult ( s, r )
char	*s, *r;
{
	GLOBALREF bool	DDebug;
  i4 rcount, errnum;
/* # line 178 "ooid.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&rcount,8);
    IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
  }
/* # line 179 "ooid.sc" */	/* host code */
	if ( errnum != OK )
	{
		IIUGerr( E_OO0018_QUEL_error_in, UG_ERR_ERROR, 3, s, r, &errnum );
	}
	if ( DDebug )
	{
		IIUGmsg( ERget(S_OO0019_row_count_for_query), FALSE, 3, &rcount, s, r );
	}
	return errnum;
}
