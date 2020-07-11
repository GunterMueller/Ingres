# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
**      All rights reserved.
*/
/*
** Includes
*/
#include <compat.h>
#include <st.h>
#include <dbms.h>
#include <fe.h>
#include <er.h>
#include "dba.h"
#include "msgs.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/*
** File: db_work.qc
**
**	This file contain routines which use quel statements to get the
**	relation names of a specified db.
**
** Contains Routines:
**
**	get_rel_name() - given a reltid and reltidx, this routine gets the
**		relation name from the iirelation catalog.
**	open_db() - opens the specified database.
**	close_db() - closes the currently open database.
**	error_handler() - called by the equel runtime system to allow
**		suppression of selected error messages
**	decl_errhndl() - called at startup to register the error_handler()
**
**	History:
**	23-may-95 (emmag)
**	    errno changed to errnum for desktop compilers.
**	08-Mar-1997 (jenjo02)
**	    Added "set session read only" to declare IPM to be a read-only
**	    session and avoid contention with CKPDB.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
/*
** Defines
*/
#define E_US0010_DB_NOT_EXIST 0x10
#define E_US0014_DB_NOT_AVAILABLE 0x14
#define E_US0003_NO_DB_ACCESS 0x03
/*
** Forward and External Declares
*/
STATUS get_rel_name();
VOID close_db();
VOID open_db();
int error_handler();
VOID decl_errhndl();
/*
** declare statics
**
**	is_db_opened - filewide static which indicates if a database is
**		currently open.  It is used by close_db to detect whether
**		or not it needs to issue an EXIT command.  This flag is set
**		when open_db successfully opens a database.  Should a
**		db open fail because the db is nonexistant, this flag is set
**		to FALSE.  Get_rel_name will catch this and prevent issuing
**		a query when no db is connected to.  Instead, it returns
**		a default table message.
*/
static bool is_db_opened = FALSE;
/*
**	get_rel_name
**	
**	Given a reltid and reltidx, looks up the table name in the
**	iirelation table. Assumes appropriate database is already open.
**	If a database is NOT open or a nonexistent dbname was supplied
**	to open_db, this routine returns a default message containing
**	the passed table ids
**
** Inputs:
**	relptr	- pointer to buffer where table name is placed
**	reltid - internal id for table
**	reltidx - internal id for table (if its an index, its > 0)
**
** Outputs:
**	table name is placed in buffer pointed to by relptr
**
** Returns:
**	OK if db was open, FAIL if db was not open
**
** History:
**	1/10/89		tomt	converted to R6
**	9/21/89		tomt	catches attempt to be called w/o an open db
**				and returns message in buffer pointed to by
**				relptr
**	10/5/89		tomt	return OK if was db open, FAIL if closed
**				and a table name could not be obtained
**	7/8/91		seng	is_db_open static variable name is clashing
**				with the globally declared is_db_open referred
**				to in the header file "dba.h".  Change static 
**				variable name to is_db_opened.
**	26-Aug-1993 (fredv)
**		Included <st.h>.
**      08-Dec-1994 (liibi01)
**              Cross integration from 6.4, removed unintentional trigraph.
**	 2-Nov-1995 (nick)
**		Remove string literal.
*/
STATUS
get_rel_name(relptr, reltid, reltidx)
  char *relptr;
  i4 reltid;
  i4 reltidx;
{
  int rcount = 0;
    STATUS	status = OK;
    if (flag_standalone)
    {
	STprintf(relptr, "<%d,%d>", reltid, reltidx);
	status = FAIL;
    }
    else if (!is_db_opened)
    {
	STprintf(relptr, "<can't find [%d,%d]>", reltid, reltidx);
	status = FAIL;
    }
    else
    {
/* # line 137 "db_work.qsc" */	/* retrieve */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIexExec(1,(char *)"db_work1",7446,13816);
      IIputdomio((short *)0,1,30,4,&reltid);
      IIputdomio((short *)0,1,30,4,&reltidx);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIexDefine(1,(char *)"db_work1",7446,13816);
        IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(relptr=trim(iirelation.relid))where iirelation.reltid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&reltid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and iirelation.reltidx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&reltidx);
        IIexDefine(0,(char *)"db_work1",7446,13816);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,0,relptr);
      if (IIerrtest() != 0) goto IIrtB1;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 139 "db_work.qsc" */	/* inquire_equel */
  {
    IIeqiqio((short *)0,1,30,4,&rcount,(char *)"rowcount");
  }
/* # line 140 "db_work.qsc" */	/* host code */
	if (!rcount)
	    STprintf(relptr, "%s%d,%d", KEY_UNKNOWN, reltid, reltidx);
    }
    return(status);
}
/*
** Open_db - opens the requested database and sets a filewide static to
**	indicate this.  If a nonexistent DB was specified, this routine
**	sets is_db_opened to 0.  get_rel_name will catch this and return
**	a default message.  This prevents a bad dbname from causing
**	the program from aborting.
**
** Inputs:
**	db_to_open - ptr to name of database to open
**
** Outputs:
**	is_db_opened will be toggled to TRUE  if no db is currently open
**
** Returns:
**	none
**
** Side Effects:
**	assumes a valid db name was passed.
**	An attempt to open a second database causes an error to be printed
**	and the program exits.
**
** History:
**	1/10/89		tomt	converted to R6
**	9/21/89		tomt	added error handling to catch attempts
**				to open nonexistent dbs
**	6/4/90		tomt    added other error handling for private dbs
**	11/2/95		nick    trap databases starting with '?? ='
**	08-Mar-1997 (jenjo02)
**	    Added "set session read only" to declare IPM to be a read-only
**	    session and avoid contention with CKPDB.
**	13-jun-2001 (hayke02)
**	    Modify (uncommented) fix for bug 87697 (natjo01's ingres!oping20
**	    change 434429) so that we now include the sqlca structure and
**	    also use inquire_sql(...dbmserror) rather than inquire_quel(...
**	    errorno). This prevents E_US0014 being returned to stdout (ipm
**	    itself), and then a hard exit from ipm, when a database is 
**	    exclusively locked. Instead, the E_US0014 error is now handled
**	    (ignored) as was the case before the fix for 87697. This change
**	    fixes bug 104839.
**	11-jul-2005 (hayke02)
**	    Use SET_SQL to guarantee we have an errortype of genericerror.
**	    This ensures that we don't see the behaviour seen in bug 104839
**	    when II_EMBED_SET is set to dbmserror because ingerror will then
**	    be 30140. This change fixes bug 114794, problem INGSRV 3346.
*/
VOID
open_db(db_to_open)
  char *db_to_open;
{
  int ingerr;
    if (flag_standalone) {
	is_db_opened = FALSE;
	return;
    }
    if (STbcompare(db_to_open, STlength(db_to_open), 
	KEY_UNKNOWN, STlength(KEY_UNKNOWN), 0) == 0)
    {
	is_db_opened = FALSE;
	return;
    }
    /*
    ** Attempt to open db is none are currently open.  Catch
    ** Bad DBnames instead of aborting
    */
    if (!is_db_opened) {
/* # line 241 "db_work.sc" */	/* set_sql */
  {
    IILQssSetSqlio(3,(short *)0,1,32,0,(char *)"genericerror");
  }
/* # line 242 "db_work.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqConnect(0,db_to_open,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 243 "db_work.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ingerr,0);
  }
/* # line 244 "db_work.sc" */	/* host code */
	if (ingerr > 0)	/* INGRES error */
	{
	    if ((ingerr == E_US0010_DB_NOT_EXIST) ||
		(ingerr == E_US0003_NO_DB_ACCESS) ||
		(ingerr == E_US0014_DB_NOT_AVAILABLE))	/* set flag to FALSE */
	    {
		is_db_opened = FALSE;		/* checked by get_rel_name */
	    }
	    else				/* another error - terminate */
	    {
		terminate(FAIL, "error opening database %s.  If DBMS server\
 is not running, you can still run IPM with -s flag", db_to_open);
	    }
	}
	else			/* NO error, set a flag and readlock nolock */
	{
	    /* avoid contention */
/* # line 239 "db_work.qsc" */	/* set */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"set session read only");
    IIsyncup((char *)0,0);
  }
/* # line 241 "db_work.qsc" */	/* host code */
/* # line 267 "db_work.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where readlock=nolock");
    IIsyncup((char *)0,0);
  }
/* # line 268 "db_work.sc" */	/* host code */
	    is_db_opened = TRUE;
	}
    }
    else			/* A DB is already open - ERROR */
    {
	terminate(FAIL, "database %s already open", db_to_open);
    }
    return;
}
/*
** Close_db - closes the currently open database and toggles a file wide
**	static flag.  If the flag is not set, this routine does a NOOP.
**
** Inputs:
**	none
**
** Outputs:
**	is_db_opened is toggled off if a db is open, else it is left alone
**
** Returns:
**	none
**
** History:
**	1/10/89		tomt	converted to R6
**	
*/
VOID
close_db()
{
    if (flag_standalone)
	return;
    if (is_db_opened) {
	is_db_opened = FALSE;	/* reset to FALSE because db is now closed */
/* # line 301 "db_work.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 302 "db_work.sc" */	/* host code */
    }
    return;
}	   
/*
** error_handler - suppress display of selected errors by the ingres
**	runtime system.  This routine is called by the ingres runtime
**	system.
**
** Inputs:
**	the INGRES error number
**
** Outputs:
**	none
**
** Returns:
**	0 - if the error message is to be suppressed
**	errno - if we want the INGRES runtime system to assume
**		default behaviour and print the message on the screen
**
** History:
**	9/21/89		tomt	written
**	
*/
int
error_handler(errnum)
int *errnum;
{
    switch (*errnum)
    {
	case E_US0010_DB_NOT_EXIST:
	case E_US0014_DB_NOT_AVAILABLE:
	    return(0);
	default:
	    return(*errnum);
    }
}
/*
** decl_errhndl - pass the address of the user written error handler
**	to the INGRES run time system.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Returns:
**	none
**
** History:
**	9/21/89		tomt	written
**	
*/
VOID
decl_errhndl()
{
    IIseterr(error_handler);
    return;
}