# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** merr.sc: Error handling procedure for MTS.
**
** 	If tracing, print error message of all errors encountered 
** 	This will probably cause diffs but we don't care if we're tracing
** 	Otherwise, conceal expected errors and only print unexpected ones
**	This will cause diffs, but they are usually terminating errors
**
**
** HISTORY:
**
** 04/20/92 ec	Added ERR_QUERYABORT  -4708
**
** 02/04/93 (erniec)
**	Add ERR_CATDEADLK, ERR_RQF_TIMEOUT to error list and if statement
**	Add ERR_LOGFULL to if statement
**	Remove ERR_SELECT which was suppressing real error messages
**	Modify comments
**
*/
/*
**	Move this OUTSIDE the function to get it into correct scope.
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include	<compat.h>
# include	<si.h>
# define ERR_DEADLOCK -4700
# define ERR_LOGFULL -4706
# define ERR_QUERYABORT -4708
# define ERR_CATDEADLK -196674
# define ERR_RQF_TIMEOUT -394566
GLOBALREF       i4 traceflag;      /*flag to set trace on/off */
merr()
{
  char errmsg[256];
/*      Use DBMS specific and NOT generic error messages */
/* # line 49 "merr.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(3,(short *)0,1,32,0,(char *)"dbmserror");
  }
/* # line 51 "merr.sc" */	/* host code */
	if (traceflag)		
	{
		SIprintf("MERR Trace: Returned errno = %d\n",sqlca.sqlcode);
/* # line 55 "merr.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,255,errmsg,63);
  }
/* # line 56 "merr.sc" */	/* host code */
		SIprintf("%s\n", errmsg);
		SIflush(stdout);
	}
	else if ((sqlca.sqlcode != ERR_DEADLOCK) && 
		 (sqlca.sqlcode != ERR_LOGFULL) && 
		 (sqlca.sqlcode != ERR_CATDEADLK) && 
		 (sqlca.sqlcode != ERR_RQF_TIMEOUT) && 
		 (sqlca.sqlcode != ERR_QUERYABORT))
	{
/* # line 65 "merr.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,255,errmsg,63);
  }
/* # line 66 "merr.sc" */	/* host code */
		SIprintf("%s\n", errmsg);
		SIflush(stdout);
	}
	return;
}
