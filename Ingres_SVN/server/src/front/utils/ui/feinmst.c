# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ui.h>
/**
** Name:    feinmst.qsc -	Check for Active MST Module.
**
** Description:
**	Contains a routine that checks whether there is a MST (multi-statement
**	transaction) pending, i.e., whether there has been a BEGIN TRANSACTION
**	issued without a subsequent intervening END TRANSACTION or ABORT.
**	Defines:
**
**	FEinMST()	check for active DBMS MST.
**
** History:
**	Revision 6.0  86/10/14  13:03:53  peterk
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*{
** Name:    FEinMST() -	Check for Active DBMS MST.
**
** Description:
**	Check whether there is an active MST pending by attempting
**	to issue a BEGIN TRANSACTION statement.  If it fails, then
**	there must have been a MST currently in progress.
**
**	When this routine returns there will always be a MST in
**	progress (either one that had previously been issued or
**	as a result of this routine successfully issuing the
**	BEGIN TRANSACTION.)
**
** Returns:
**	{bool}  TRUE		// a MST was already pending
**		FALSE		// no MST was pending
**
** Side Effects:
**	Will start a MST if one was not already in progress.
**
** History:
**	10-86 (peterk) - created.
**	10-sep-87 (daver)
**		Initialized the static Xacterr to FALSE before starting
**		the new transaction. If no error occurs, then msterr is
**		not called; hence Xacterr may be still TRUE from a previous
**		call, leading clients of this routine to the wrong conclusion.
**	30-sep-1987 (peter)
**		Close the transaction if it is open.
**	28-oct-1987 (peter)
**		Fix bug.  If transaction NOT open, close it.
**      4-mar-1988 (danielt)
**		return TRUE against a gateway
**	13-oct-1988 (danielt)
**		changed to use new INQUIRE_INGRES interface
*/
bool
FEinMST()
{
  i4 in_trans;
/* # line 74 "feinmst.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&in_trans,73);
  }
/* # line 75 "feinmst.sc" */	/* host code */
    	if (in_trans)
		return (TRUE);
	else
		return (FALSE);	
}
