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
# include <ui.h>
# include <uigdata.h>
/**
** Name:    uixact.qsc -Front-End Utility Transaction Management Module.
**
** Description:
**	These are routines to simulate QUEL transactions using 
**	the SET AUTOCOMMIT statements.  
**	IMPORTANT NOTE:
**		These routines assume that all INGRES FE code runs
**	with autocommit ON, and that ALL transaction statements issued
**	by INGRES FE products are done through this interface.
**	Note that the forms system looks at the LIBQ transaction state
**	information to determine whether to COMMIT after initializing
**	a form, and so does not use these routines.
**
** Defines:
**	IIUIautocommit()
**	IIUIbeginXaction()
**	IIUIendXaction()
**	IIUIabortXaction()
**	IIUIinXaction()
**	
** History:
**	25-aug-1988 (danielt)
**		written
**	3/90 (bobm)
**		added labelled Xact wrappers
**	11/90 (bobm)
**		added IIUIfxcForceXactCommit
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*
** label count is started at a strange number to make it unlikely
** that a random argument to IIUIlxeLabelXactEnd will match a set
** IIUIdbdata()->xactlabel.
*/
static i4  Labcount = 1001;
/*
** If this flag is turned on, SQL commits will be done before all
** transaction starts.  This is chiefly to accomodate low traffic
** FE's which do not run in autocommit state.   Added for fecvt60.
*/
static bool ForceCommit = FALSE;
/*{
** Name:	IIUIfxcForceXactCommit() -	set forced commits.
**
** Description:
**	Allows the setting of a flag which will cause commits on
**	all transaction starts, to make sure everything is "flushed".
**
** Inputs:
**	state - Setting for "force commit" flag.
**
** History:
**	11/90 (bobm) - written.
*/
VOID
IIUIfxcForceXactCommit(state)
bool state;
{
	ForceCommit = state;
}
/*{
** Name:	IIUIautocommit() -	Set the DBMS AutoCommit State.
**
** Description: Routine to turn autocommit on or off.  Currently
**	these routines use the SET AUTOCOMMIT statement.  If this 
**	is not supported against all gateways, this routine will 
**	need to be changed (to put support into LIBQ for this, for
**	example).
**
** Inputs:
** 	i4	set_flag; UI_AC_ON - turn autocommit on 	
**			  UI_AC_OFF - turn autocommit off
**
** Returns:
**	{DB_STATUS}	OK			successful
**			other			dbms error
**
** Side Effects:
**	changes autocommit state of the DBMS.
**
** History:
**	aug-25-1988 (danielt) 
**		written
*/
DB_STATUS
IIUIautocommit(set_flag)
i4	set_flag;
{
  i4 error_no;
	if (set_flag == UI_AC_OFF)
	{
/* # line 112 "uixact.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set AUTOCOMMIT OFF");
    IIsyncup((char *)0,0);
  }
/* # line 113 "uixact.sc" */	/* host code */
		IIUIdbdata()->in_transaction = TRUE;
	}
	else 
	{
/* # line 117 "uixact.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set AUTOCOMMIT on");
    IIsyncup((char *)0,0);
  }
/* # line 118 "uixact.sc" */	/* host code */
		IIUIdbdata()->in_transaction = FALSE;
	}
/* # line 120 "uixact.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&error_no,2);
  }
/* # line 121 "uixact.sc" */	/* host code */
	return (error_no);
}
/*{
** Name: IIUIlxbLabelXactBegin()
**
** Description: 
**	This call is to allow us to "wrap" routines which perform
**	transactions with a larger transaction.  This starts a transaction,
**	and returns a number to the caller.  All transaction begin / ends
**	will essentially be ignored until IIUIlxeLabelXactEnd is called
**	with the same number.  If these are nested, only the outer one
**	is effective.
**
**	If any aborts occur within the scope of the labelled transaction,
**	we will abort on the end of it, which may create a redundant
**	rollback, but is safer.  We DO actually abort on the "abort"
**	call, to be sure it gets done. 
**
**	This is being provided expressly for the -q option of copyapp, and
**	should be used with care, obviously.
**
** Outputs:
**	label - label number to close transaction.
**
** History:
**	3/90 (bobm)
**		written
*/
STATUS
IIUIlxbLabelXactBegin(label)
i4  *label;
{
	STATUS stat;
	*label = ++Labcount;
	if (IIUIdbdata()->xactlabel != 0)
		return OK;
	if ((stat = IIUIbeginXaction()) == OK)
	{
		IIUIdbdata()->xactlabel = *label;
		IIUIdbdata()->lababort = FALSE;
	}
	return stat;
}
/*{
** Name: IIUIlxeLabelXactEnd()
**
** Description: 
**	End labelled transaction.  This will be called with the number
**	returned by IIUIlxbLabelXactBegin, and will actually commit
**	that transaction, if it was the "outermost" one.  If it appears
**	that no labelled transaction is active, we just commit, although
**	such a situation indicates something wrong.
**
** Outputs:
**	label - label number to close transaction.
**
** History:
**	3/90 (bobm)
**		written
*/
STATUS
IIUIlxeLabelXactEnd(label)
i4  label;
{
	if (IIUIdbdata()->xactlabel == 0)
		return IIUIendXaction();
	if (IIUIdbdata()->xactlabel != label)
		return OK;
	IIUIdbdata()->xactlabel = 0;
	if (IIUIdbdata()->lababort)
		return IIUIabortXaction();
	return IIUIendXaction();
}
/*{
** Name: IIUIbeginXaction()
**
** Description: simulates QUEL begin transaction statement by
**	turning autocommit off in backend or gateway.
**
** Returns:
**	{DB_STATUS}	OK			successful
**			other			dbms error
**
** Side Effects:
**	changes autocommit state of the DBMS.
**
** History:
**	aug-25-1988 (danielt) 
**		written
*/
DB_STATUS
IIUIbeginXaction()
{
	if (IIUIdbdata()->xactlabel != 0)
		return OK;
	if (ForceCommit)
	{
/* # line 218 "uixact.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 219 "uixact.sc" */	/* host code */
	}
	return (IIUIautocommit(UI_AC_OFF));
}
/*{
** Name: IIUIendXaction()
**
** Description: simulates QUEL end transaction statement by
**	commiting all work, then turning autocommit on in backend or gateway.
**
** Returns:
**	{DB_STATUS}	OK			successful
**			other			dbms error
** Side Effects:
**	changes autocommit state of the DBMS.
*
** History:
**	aug-25-1988 (danielt) 
**		written
*/
DB_STATUS
IIUIendXaction()
{
	if (IIUIdbdata()->xactlabel != 0)
		return OK;
/* # line 243 "uixact.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 244 "uixact.sc" */	/* host code */
	return (IIUIautocommit(UI_AC_ON));	
}
/*{
** Name: IIUIabortXaction()
**
** Description: simulates QUEL abort transaction statement by
**	rolling back all work, then turning autocommit on in backend 
**	or gateway.
**
** Returns:
**	{DB_STATUS}	OK			successful
**			other			dbms error
**
** Side Effects:
**	changes autocommit state of the DBMS.
**
** History:
**	aug-25-1988 (danielt) 
**		written
*/
DB_STATUS
IIUIabortXaction()
{
/* # line 267 "uixact.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 268 "uixact.sc" */	/* host code */
	if (IIUIdbdata()->xactlabel != 0)
	{
		IIUIdbdata()->lababort = TRUE;
		return OK;
	}
	return IIUIautocommit(UI_AC_ON);	
}
/*{
** Name:	IIUIinXaction()
**
** Description:
**	Boolean function returning whether there
**	is an FE transaction currently in effect.
**
** Returns:
**	{bool}	TRUE 	transaction active
**		FALSE   no transaction active
** History:
**	21-apr-1989 (danielt) 
**		written
*/
bool
IIUIinXaction()
{
	return IIUIdbdata()->in_transaction;
}
