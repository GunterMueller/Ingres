# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**     testinfo.c 
**
*/
/*
** History:
**
**	21-apr-1994 (donj)
**	Created.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlda.h"
#include <compat.h>
#include <cm.h>
#include <er.h>
#include <lo.h>
#include <si.h>
#include <st.h>
#include <tm.h>
#include <sepdefs.h>
#include <sepfiles.h>
#define testinfo_c
#include <fndefs.h>
GLOBALREF    i4            tracing ;
GLOBALREF    FILE         *traceptr ;
  GLOBALDEF i4 ing_not_found;
  GLOBALDEF char testinfo_errmsg[256];
  GLOBALDEF i4 ing_errno;
STATUS
testinfo_connect( char *db_name )
{
    STATUS                 ret_val = OK ;
  char *dbname = db_name ;
/* # line 63 "testinfo.sc" */	/* host code */
    ing_not_found = ing_errno = OK;
/* # line 65 "testinfo.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqConnect(0,dbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
    if (sqlca.sqlcode == 100) 
      testinfo_not_found();
    else if (sqlca.sqlcode < 0) 
      testinfo_error();
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      testinfo_error();
  }
/* # line 67 "testinfo.sc" */	/* host code */
    ret_val = (ing_not_found != OK)||(ing_errno != OK)
	    ? FAIL : OK ;
    return (ret_val);
}
STATUS
testinfo_disconnect()
{
    STATUS                 ret_val ;
/* # line 82 "testinfo.sc" */	/* host code */
    ing_not_found = ing_errno = OK;
/* # line 84 "testinfo.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
    if (sqlca.sqlcode == 100) 
      testinfo_not_found();
    else if (sqlca.sqlcode < 0) 
      testinfo_error();
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      testinfo_error();
  }
/* # line 86 "testinfo.sc" */	/* host code */
    ret_val = (ing_not_found != OK)||(ing_errno != OK)
	    ? FAIL : OK ;
    return (ret_val);
}
STATUS
testinfo_error()
{
    STATUS                 ret_val = OK ;
    /*
    ** Get text of error msg
    */
/* # line 101 "testinfo.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,255,testinfo_errmsg,63);
    IILQisInqSqlio((short *)0,1,30,4,&ing_errno,2);
  }
/* # line 104 "testinfo.sc" */	/* host code */
    if (tracing)
    {
	SIfprintf(traceptr,"ing_errno: %d\n", ing_errno);
	SIfprintf(traceptr,"testinfo_errmsg: %s\n", testinfo_errmsg);
	SIflush(traceptr);
    }
    else
    {
	SIprintf("ing_errno: %d\n", ing_errno);
	SIprintf("testinfo_errmsg: %s\n", testinfo_errmsg);
	SIflush(stdout);
    }
    return (ret_val);
}
STATUS
testinfo_not_found()
{
    STATUS                 ret_val = OK ;
    testinfo_error();
    ing_not_found = FAIL;
    return (ret_val);
}
