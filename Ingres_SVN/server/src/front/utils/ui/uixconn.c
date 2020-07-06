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
# include <ui.h>
# include <uigdata.h>
/**
** Name:	uixcon.qsc - turn on or off exclusive DB connection
**
** Description:
** 		- turn on or off exclusive DB connection
**
**	This file defines:
**
**	IIUIxc1_xConnOn() Turn on exclusive db connection.
**
**	IIUIxc0_xConnOff() Turn off exclusive db connection.
**			
**/
/* globals */
/* defines */
/* statics */
static	i4	UIXC_connect_stat = OK;
/*{
** Name:	IIUIxc1_xConnOn()
**
** Description:
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**		OK			successful
**		other			dbms error
**	Exceptions:
**		none
**
** Side Effects:
**	Tries to establish exclusive DB connection to current
**	database as defined by IIUIdbdata->database global.  If it fails,
**	there will be NO DB CONNECTION!!  Use IIUIxc0_xConnOff
**	to try to reestablish the connection.
** History:
**	18-mar-1988 (danielt) 
**		written
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
DB_STATUS
IIUIxc1_xConnOn()
{
  i4 err_num;
  UIDBDATA *uidbdata;
	i4	ret_stat;
	if (UIXC_connect_stat == OK)
	{
/* # line 67 "uixconn.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IIsqDisconnect();
  }
/* # line 68 "uixconn.sc" */	/* host code */
	}
	uidbdata = IIUIdbdata();
/* # line 70 "uixconn.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IIsqUser(uidbdata->suser);
    IIsqMods(2);
    IIsqConnect(0,uidbdata->database,(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
    IIsqParms(0,(char *)"share",32,(char *)"readers",0);
    IIsqParms(1,(char *)"mode",32,(char *)"updating",0);
    IIsqParms(2,(char *)0,0,(char *)0,0);
  }
/* # line 73 "uixconn.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err_num,2);
  }
/* # line 74 "uixconn.sc" */	/* host code */
	if (err_num != 0)
	{
		ret_stat = FAIL;
	}
	else
	{
		ret_stat = OK;
	}
	UIXC_connect_stat= ret_stat;
	return (ret_stat);
}
/*{
** Name:	IIUIxc0_xConnOff()
**
** Description:
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**		OK			successful
**		other			dbms error
**	Exceptions:
**		none
**
** Side Effects:
**	Tries to establish non-exclusive DB connection to current
**	database as defined by IIUIdbdata->database global.  If it fails,
**	there will be NO DB CONNECTION!!  
**	It is expected that a failure of this routine is
**	a nasty fatal error.
** History:
**	18-mar-1988 (danielt) 
**		written
*/
DB_STATUS
IIUIxc0_xConnOff()
{
  i4 err_num;
  UIDBDATA *uidbdata;
	i4	ret_stat;
	if (UIXC_connect_stat == OK)
	{
/* # line 119 "uixconn.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IIsqDisconnect();
  }
/* # line 120 "uixconn.sc" */	/* host code */
	}
	uidbdata = IIUIdbdata();
/* # line 122 "uixconn.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IIsqUser(uidbdata->suser);
    IIsqMods(2);
    IIsqConnect(0,uidbdata->database,(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
    IIsqParms(0,(char *)"share",32,(char *)"all users",0);
    IIsqParms(1,(char *)"mode",32,(char *)"updating",0);
    IIsqParms(2,(char *)0,0,(char *)0,0);
  }
/* # line 125 "uixconn.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err_num,2);
  }
/* # line 126 "uixconn.sc" */	/* host code */
	if (err_num != 0)
	{
		ret_stat = FAIL;
	}
	else
	{
		ret_stat = OK;
	}
	UIXC_connect_stat= ret_stat;
	return (ret_stat);
}
