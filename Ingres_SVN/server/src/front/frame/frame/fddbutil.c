# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	fddbutil.qsc
**
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
/**
** Name:	fddbutil.qsc - DB utility file for forms system.
**
** Description:
**	This file contains some db utility routines for the forms
**	system below the runtime layer.
**
** History:
**	07/23/88 (dkh) - Initial version.
**	03/20/91 (dkh) - Added support for (alerter) event in FRS.
**	04/20/91 (dkh) - Changed support for alerter to conform to LRC
**			 approved semantics.
**	07/18/91 (dkh) - Updated syntax to use DBEVENT instead of EVENT.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<st.h>		 
# include	<fe.h>
# include	<ft.h>
# include	<fmt.h>
# include	<adf.h>
# include	<frame.h>
# include	<ui.h>
# include	<fsicnsts.h>
FUNC_EXTERN	ADF_CB	*FEadfcb();
/*{
** Name:	IIFDesiExecSqlImm - Perform an EXEC SQL IMMEDIATE call.
**
** Description:
**	This routine does an EXEC SQL IMMEDIATE call on the passed
**	in buffer which is assumed to contain a sql statement to execute.
**	We assume that the syntax is correct and that the statement is
**	valid for whatever context the executing environment is in.
**
** Inputs:
**	sqlstmt	Buffer containing sql statement to execute.
**
** Outputs:
**	None.
**
**	Returns:
**		None.
**	Exceptions:
**		None.
**
** Side Effects:
**	Whatever the sql statement may cause.
**
** History:
**	07/23/88 (dkh) - Initial version.
**	17-aug-91 (leighb) DeskTop Porting Change: added st.h
*/
VOID
IIFDesiExecSqlImm(sqlstmt)
  char *sqlstmt;
{
	if (sqlstmt != NULL && *sqlstmt != EOS)
	{
/* # line 75 "fddbutil.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(sqlstmt);
    IIsyncup((char *)0,0);
  }
/* # line 76 "fddbutil.sc" */	/* host code */
	}
}
/*{
** Name:	IIFDaeAlerterEvent - Check for an (alerter) event.
**
** Description:
**	This routine simply checks for an (alerter) event by running
**	a "get event" statement.  If one is found, it returns TRUE,
**	FALSE otherwsie.
**
**	Side effects of this routine is that it will de-queue the
**	(alerter) event so that the user must use inquire_frs to
**	find out the specifics.  This routine also stores away the
**	various values of the event to allow the application to
**	inquire about the event later on.
**
** Inputs:
**	None.
**
** Outputs:
**
**	Returns:
**		TRUE	If an (alerter) event found.
**		FALSE	If no (alerter) event found.
**	Exceptions:
**		None.
**
** Side Effects:
**	None other than that listed above.
**
** History:
**	03/20/91 (dkh) - Initial version.
*/
i4
IIFDaeAlerterEvent()
{
  char a_name[FE_MAXNAME + 1];
/* # line 115 "fddbutil.sc" */	/* get dbevent */
  {
    IIsqInit((char *)0);
    IILQesEvStat(0,0);
  }
/* # line 116 "fddbutil.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,FE_MAXNAME + 1-1,a_name,74);
  }
/* # line 117 "fddbutil.sc" */	/* host code */
	if (a_name[0] != EOS)
	{
		return(TRUE);
	}
	return(FALSE);
}
