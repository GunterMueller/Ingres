# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
** dualarm.sc - alarm checker: look for alarm entries to be executed
**
** History:
**	23-aug-1993 (dianeh)
**		Created (from ingres63p!generic!starutil!duf!dua!dualarm.sc);
**		updated for 6.5 -- most notably, since this stuff now goes in
**		$II_SYSTEM/sig/star, the calls to NMloc() had to be changed to
**		use NMgtAt()'s instead.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	04-apr-06 (toumi01)
**	    Check that the wakeup def file is a regular file and, if it is,
**	    that we can clear it.
**	05-apr-06 (drivi01) on behalf of toumi01.
**	    Port change to Windows by simplifying call to LOinfo to return
**	    only the basic file type.
*/
/* The scheduler will be running as the ingres SYSTEM administrator but any
   commands that you spawn must only run as the user otherwise you will have
   a big security hole. */
/* Need a quiet mode where only errors are printed, no status messages */
/* Need to print out ALL errors - so maybe SQLPRINT needs to be on? */
/*
DB names must be up to 256!
Try run/detach /input=x.com sys$system:dcl.exe
Do 'register as refresh' on all link objects
Cannot be applied to SQL Gateway since issues non-common SQL and
	uses -u flag (without password)
Can work on STAR except if CDB is SQL Gateway for same reasons as above.
Ability to specify command line flags to be used on the connection.
Use standard error message files for log printouts
*/
/*
NEEDLIBS = SQLCALIB TBACCLIB LIBQLIB LIBQGCALIB GCFLIB FDLIB FTLIB FEDSLIB FMTLIB UILIB UGLIB AFELIB ADFLIB CUFLIB COMPATLIB MALLOCLIB
UNDEFS = IIeqiqio
PROGRAM = scheduler
*/
/*
** NOTE ABOUT NAMING
**
** The word "alarm" was originally used to describe what this program dealt
** with. Later on the this was changed to the word "schedule".
**
** Therefore you will see a mixture of these ideas.  Currently, the product has 
** a working name of "SQL*SCHEDULER" and the idea is that this product is
** used for scheduling sets of SQL queries to be executed at various times.
**
** This product has been enhanced to schedule the replication of CDBs (where the
** STAR catalogs are kept).
**
** This product will be enhanced to schedule the running of some INGRES
** subsystems (like running reports every monday at 5am).
*/
/*
** NOTE ABOUT THE SCHEDULER CATALOGS
**
** The scheduler creates its own catalogs
** 
** The scheduler catalogs should be owned by $ingres.  However this did not work
** out due to permission problems.
**
** Basically, there should be one set of catalogs per database and each user
** must have access to only those rows in the catalogs that belong to them.
** So the catalog is owned by the DBA and there is no permission granted on
** this to anyone.  However, there is a view on top of the catalog with the
** qualification where alarm_user = dbmsinfo('username').
**
** This allows a user to access only those rows in the catalog which they own.
** This is the only way to do this in SQL (in QUEL, one could just define a
** permission like this, however in SQL, one must define a view to get this
** functionality).
**
** The view is given full access to everyone.  In SQL the permission on the
** view is automatically propogated to the base table (when the base table is
** accessed through the view).  HOWEVER, if the base table is owned by $ingres,
*  this permission propogation does not occur.  That is why the catalogs are
** owned by the DBA and not $ingres.
**
** An integrity with the same qualification as above is created on the catalog
** base table so that users may only add rows for themselves.
*/
/*
** NOTE ABOUT THE STRUCTURE OF THIS PROGRAM
**
** This program is logically several seperate items which have all been
** combined into one executable for ease of maintenance, porting, shipping, etc.
**
** There are basically three parts:
**	master
**	slave
**	utility
**
** There will typically be several instances of this program running on a system
** with each instance running in a different mode.
**
** The master is started at INGRES startup time.  It checks the master scheduler
** database (iialarmdb) and figures out how many slaves should be started and
** then starts each one.  If a slave dies it simply restarts it.  (Currently
** only one slave is started - the code for multiple slaves is here but not
** working since I could not figure out how to get a detached process to spawn
** subprocesses.)  The master must be run by the INGRES system administrator.
**
** A slave process connects to iialarmdb to figure out what databases it is
** supposed to service and then begins making its 'rounds'.  It checks each
** database for which it is responsible and performs the actions requested by
** any schedules needing servicing.  As it goes through each database it
** remembers the time of the next scheduled event for that database.  After
** servicing all databases the slave sleeps until the next scheduled event
** (unless a 'wakeup' occurs in which case the slave wakes up immediately at
** begins its rounds again).
**
** The slave runs as the INGRES system administrator but uses the '-u' flag
** whenever connecting to a user database.  In this way the the queries in a
** schedule are executed with the appropriate userid and permissions.
** 
** The utility mode of this program is used to review and modify schedules.
** This mode is run directly by the user.
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include	<compat.h>
# include	<dbms.h>
# include	<me.h>
# include	<st.h>
# include	<tm.h>
# include	<pc.h>
# include	<lo.h>
# include	<nm.h>
# include	<cm.h>
# include	<er.h>
# include	<tr.h>
# include	<si.h>
# include	<ut.h>
/* sqlca.sqlcode codes */
#define	ZERO_ROWS	100
#define	DEADLOCK	-4700 	/* use 4700 in osl/sql,
				   osl/quel & equel */
/* various subroutine return codes */
# define NOMOREALARMS	2
# define MOREALARMS	3
# define WAKEUP_EVENT	4
# define STOP_EVENT	5
# define ERROR_EVENT	6
# define NO_DATABASES	7
/* maximum number of alarm checker daemons (only one is currently implemented */
# define	ALARM_CHECKERS_MAX	256
struct alarm_chkrs {
	char		id[4];
	PID		pid;
};
struct alarm_chkrs	alarm_checker_arr[ALARM_CHECKERS_MAX];
i4	T_quiet;	/* quiet mode */
char	*T_node;	/* node of database currently being processed */
char	*T_dbms;	/* dbms of database currently being processed */
char	*T_alarm_id;	/* alarm currently being processed */
char	*T_alarm_desc;	/* description of alarm currently being processed */
int	*T_query_seq;	/* number of query within the current alarm */
char	Instid[10];	/* 2 letter installation id, null terminated */
char	T_curnode[65];	/* virtual node name of the node that this process is
			   currently running on */
bool	T_isterminal;	/* TRUE if stdin is a terminal (i.e., we are not running
			   as detatched or subprocess) */
  char *T_alarm_checker_id;
  char *T_user;
  char *T_database;
char	*ERget();
main(argc,argv)
int argc;
char **argv;
{
    char	*p;			/* tmp pointer */
    char	vnodelog[33];		/* virtual node name of the node we are
					   running on */
    char	*cnode;			/* tmp pointer */
    LOCATION	logfile;		/* where to log status and error messages */
    char	logfilebuf[MAX_LOC];	/* used with 'logfile' above */
    CL_SYS_ERR	clerrcode;		/* status return for TRset_file call */
    i4		retval;			/* generic "return-value" */
    /* needed for frontends */
    MEadvise( ME_INGRES_ALLOC );
    T_quiet = 0;	/* noisy */
    T_isterminal = SIterminal(stdin);
    /* get installation id and virtual node name to use for
       logfile naming purposes */
    NMgtAt("II_INSTALLATION", &p);
    if (p && *p)
    {
	STcopy(p, Instid);
	Instid[2]=0; /* in case installation ID was big */
    }
    else
	Instid[0] = 0;
    /* find out the current node */
    T_curnode[0] = 0;
    if (Instid && *Instid)
    {
	STcopy("II_GCNxx_LCL_VNODE", vnodelog);
	vnodelog[6] = Instid[0];
	vnodelog[7] = Instid[1];
    }
    else
	STcopy("II_GCN_LCL_VNODE", vnodelog);
    NMgtAt(vnodelog, &cnode);
    if (cnode && *cnode)
    {
	STcopy(cnode, T_curnode);
	CVlower(T_curnode);
    }
#ifdef hp9_mpe
    (void) STcopy("alarm.log", logfilebuf);
#else
    if (argc == 3 && STcompare(argv[1], "slave") == 0)
	(void) STpolycat(3, "alarm_", argv[2], ".log", logfilebuf);
    else
	(void) STcopy("alarm.log", logfilebuf);
#endif
    /* set up the output logging file */
    if (T_curnode[0])
	(void) STpolycat(3, logfilebuf, "_", T_curnode, logfilebuf);
    /* if a detached job then log in the files directory */
    if (T_isterminal == FALSE)
    {
	NMloc(FILES, FILENAME, logfilebuf, &logfile);
	LOcopy(&logfile, logfilebuf, &logfile);
    }
    if (TRset_file(TR_F_OPEN , logfilebuf,
	    STlength(logfilebuf), &clerrcode) != OK)
    	PCexit(1);
    /* figure out what to do and then call the appropriate service routine */
    if (argc == 2 && STcompare(argv[1], "wakeup") == 0)
	wakeup();
    else if (argc == 2 && STcompare(argv[1], "stop") == 0)
	stop();
    else if (argc == 2 && STcompare(argv[1], "master") == 0)
	master();
    else if (argc < 2 && (T_isterminal == FALSE))
	master();
#ifdef unix
    /* on UNIX a special program is used to start up "detached" processes.
       This program always passes one argument, the installationi id, so
       that a 'ps' will show the installation that the process belongs to.
       This is the first 'frontend' program that has been started as a
       daemon - we probably want to invent a CL way of dealing with this. */
    else if (argc == 2 && STcompare(argv[1], Instid) == 0)
	master();
#endif
    else if (argc < 2)
	usage();
    else if (STcompare(argv[1], "add") == 0)
	mastermaint(1, argc, argv);
    else if (STcompare(argv[1], "drop") == 0)
	mastermaint(2, argc, argv);
    else if (STcompare(argv[1], "enable") == 0)
	mastermaint(3, argc, argv);
    else if (STcompare(argv[1], "disable") == 0)
	mastermaint(4, argc, argv);
    else if (STcompare(argv[1], "show") == 0)
	mastermaint(5, argc, argv);
    else if (STcompare(argv[1], "slave") == 0)
    {
	/* 'slave' will return if it found a 'stop' or 'wakeup' event
	   file.   On a 'stop' event, this slave will stop.  On
	   a 'wakeup' event, the 'slave' should
	   go back to the iialarmdb and reread the 
	   information, and then start checking for alarms
	   right away */
	for (;;)
	{
	    if ((retval = slave(argc, argv)) == STOP_EVENT)
	    {
		err_print(0, "Slave 'def' has been stopped\n");
		PCexit(0);
	    }
	    else if (retval == WAKEUP_EVENT)
	    {
		err_print(0, "Slave 'def' has been awakened\n");
		continue;
	    }
	    else if (retval == NO_DATABASES)
	    {
		err_print(0, "Slave 'def' has no databases to service\n");
		err_print(0, "Sleep for 1 hour\n");
		if (goodsleep(60 * 60) == STOP_EVENT)
		{
		    err_print(0, "Slave 'def' has been stopped\n");
		    PCexit(0);
		}
		continue;
	    }
	    else
	    {
		err_print(1, "Slave 'def' had error, sleep for 5 minutes then try again\n");
		if (goodsleep(5 * 60) == STOP_EVENT)
		{
		    err_print(0, "Slave 'def' has been stopped\n");
		    PCexit(0);
		}
	    }
	}
    }
    else if (STcompare(argv[1], "init") == 0)
    {
	/* create the scheduler catalogs */
	PCexit(createtables(argc, argv));
    }
    else if (STcompare(argv[1], "update") == 0)
    {
	/* startup QBF to allow schedules to be browsed or deleted or updated */
	sched_update(argc, argv);
    }
    else
	usage();
    PCexit(FAIL);
}
sched_update(argc, argv)
int	argc;
char	**argv;
{
    STATUS	status;
    CL_ERR_DESC	clerrv;
    char	*p;
  long tmp;
  char *dbname;
/* # line 369 "dualarm.sc" */	/* host code */
    T_alarm_checker_id = "update";
    if (argc != 3)
	usage();
    dbname = argv[2];
/* # line 376 "dualarm.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqConnect(0,dbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 378 "dualarm.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	err_print(1, "Error connecting to %s to check username\n", dbname);
	wakeup();
	return (FAIL);
    }
    tmp = 0;
/* # line 386 "dualarm.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select 1 from iidbconstants where dbmsinfo('username')=dbmsinfo('dba'\
)");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tmp);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 388 "dualarm.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
	err_print(1, "Error checking username/dba(1)\n");
    if (tmp == 0)
    {
    err_print(1, "Only the dba for this database may run update the catalogs\n");
    err_print(1, "This restriction will probably be removed in the near future\n");
/* # line 395 "dualarm.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 396 "dualarm.sc" */	/* host code */
	wakeup();
	return (FAIL);
    }
/* # line 400 "dualarm.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 402 "dualarm.sc" */	/* host code */
    status = UTexe(UT_WAIT, NULL, NULL, NULL, "qbf", &clerrv,
	    "database = %S, entry = %S", 2, dbname, "iialarm");
    if (status)
    {
	err_print(1, "Error encountered while calling QBF to update catalogs\n");
	p = ERget(status);
	err_print(1, "%s\n", p);
    }
    wakeup();
}
createtables(argc, argv)
int	argc;
char	**argv;
{
    int		isstar = 0;
    STATUS	status;
    LOCATION	formfile;
    char	formfilebuf[MAX_LOC];
    char	*p;
    CL_ERR_DESC clerr;
    char	*ptr ;
    char	buf[MAX_LOC] ;
  char *dbname;
  long tmp;
  char dbms_type[33];
    /*
    ** Create catalogs for either the iialarmdb master database or the
    ** user's database.  
    **
    ** All catalogs are created as owned by the dba (not $ingres).  This
    ** is so the (non-dba) user's can update the catalog for rows that
    ** they own.  If the catalogs were owned by $ingres then "update-system
    ** catalog" permission is required even if the user had permission on
    ** an overlying view.
    */
/* # line 448 "dualarm.sc" */	/* host code */
    T_alarm_checker_id = "init";
    if (argc == 3 && STcompare(argv[2], "master") == 0)
    {
	err_print(0, "Connecting to master database iialarmdb...\n");
/* # line 454 "dualarm.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqConnect(0,(char *)"iialarmdb",(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 456 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
err_print(1, "Error connecting to iialarmdb (do 'createdb iialarmdb' first)\n");
	    return (FAIL);
	}
	tmp = 0;
/* # line 463 "dualarm.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select 1 from iidbconstants where dbmsinfo('username')=dbmsinfo('dba'\
)");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tmp);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 465 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error checking username/dba(2)\n");
	if (tmp == 0)
	{
	  err_print(1, "Only the dba for this database may create the catalogs\n");
/* # line 471 "dualarm.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 472 "dualarm.sc" */	/* host code */
	    return (FAIL);
	}
	crtabs(1, 0, 1);
/* # line 477 "dualarm.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 479 "dualarm.sc" */	/* host code */
	return (OK);
    }
    else if (argc == 4 && STcompare(argv[2], "user") == 0)
    {
	/*
	** The following two tables are created in any database that wishes to
	** have alarm service.  Each user in a database must have their own
	** set of the following two tables.  This is to ensure security (to
	** prevent one user from entering alarm queries for another)
	*/
	dbname = argv[3];
	err_print(0, "Connecting to user database %s...\n", dbname);
/* # line 492 "dualarm.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqConnect(0,dbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 494 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    err_print(1, "Error connecting to database:%s, exiting\n", dbname);
	    return (FAIL);
	}
	tmp = 0;
/* # line 501 "dualarm.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select 1 from iidbconstants where dbmsinfo('username')=dbmsinfo('dba'\
)");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tmp);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 503 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error checking username/dba(3)\n");
	if (tmp == 0)
	{
	    err_print(1, "Only the dba for this database may create the catalogs\n");
/* # line 509 "dualarm.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 510 "dualarm.sc" */	/* host code */
	    return (FAIL);
	}
	dbms_type[0] = 0;
/* # line 514 "dualarm.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select cap_value from iidbcapabilities where cap_capability='DBMS_TYP\
E'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,32,dbms_type);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 516 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error checking dbms_type\n");
	if (STcompare(dbms_type, "STAR                            ") == 0)
	    isstar++;
	if (isstar)
	{
	    err_print(0, "This is a Distributed Database\n");
/* # line 525 "dualarm.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 526 "dualarm.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
		err_print(1, "Error commiting before direct connect\n");
/* # line 528 "dualarm.sc" */	/* direct connect */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"direct connect");
    IIsyncup((char *)0,0);
  }
/* # line 529 "dualarm.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
		err_print(1, "Error doing direct connect\n");
	}
	crtabs(0, 0, 1);
	/* insert some examples (disabled) */
	err_print(0, "Load examples...\n");
	/* first a snapshot copy example */
/* # line 539 "dualarm.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into i_user_alarm_settings values(dbmsinfo('username'), 'Snaps\
hot Copy Example', 'Snapshot of onsale is onsale_newyork (which is in \
another LDB). Do once/week on Sunday at 3:00pm GMT with up to 10 retry\
s once/hr', '', 0, '1989_01_01 00:00:00 GMT', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'1989_06_11 15:00:00 GMT', '', 1, 'week', '1989_06_12 00:00:00 GMT', \
'', 0, 10, 1, 'hour', '', '', 0, '', 'N')");
    IIsyncup((char *)0,0);
  }
/* # line 545 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Could not load examples (master)\n");
/* # line 547 "dualarm.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into i_user_alarm_query_text values(dbmsinfo('username'), 'Sna\
pshot Copy Example', 0, 'S', 'S', '', 0, 'delete from onsale_newyork',\
 0, 0)");
    IIsyncup((char *)0,0);
  }
/* # line 550 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Could not load examples (detail)\n");
/* # line 552 "dualarm.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into i_user_alarm_query_text values(dbmsinfo('username'), 'Sna\
pshot Copy Example', 1, 'S', 'S', '', 0, 'insert into onsale_newyork s\
elect * from onsale where state = ''NY''', 0, 0)");
    IIsyncup((char *)0,0);
  }
/* # line 556 "dualarm.sc" */	/* host code */
	/* next, a data rollup example */
/* # line 557 "dualarm.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into i_user_alarm_settings values(dbmsinfo('username'), 'Data \
Rollup Example', 'Rollup worldwide sales data on last day of month (28\
,29,30 or 31 depending on month). On error, retry indefintely every 4 \
hours.', '', 0, '1989_01_01 00:00:00 GMT', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'1989_01_31 17:00:00 GMT', '', 1, 'month', '1989_06_12 00:00:00 GMT',\
 '', 0, 1000, 4, 'hours', '', '', 0, '', 'N')");
    IIsyncup((char *)0,0);
  }
/* # line 563 "dualarm.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into i_user_alarm_query_text values(dbmsinfo('username'), 'Dat\
a Rollup Example', 0, 'S', 'S', '', 0, 'drop companysales', 0, 0)");
    IIsyncup((char *)0,0);
  }
/* # line 566 "dualarm.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into i_user_alarm_query_text values(dbmsinfo('username'), 'Dat\
a Rollup Example', 1, 'S', 'S', '', 0, 'create table companysales as s\
elect * from eastcoastsales', 0, 0)");
    IIsyncup((char *)0,0);
  }
/* # line 569 "dualarm.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into i_user_alarm_query_text values(dbmsinfo('username'), 'Dat\
a Rollup Example', 2, 'S', 'S', '', 0, 'insert into companysales selec\
t * from westcostsales', 0, 0)");
    IIsyncup((char *)0,0);
  }
/* # line 572 "dualarm.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into i_user_alarm_query_text values(dbmsinfo('username'), 'Dat\
a Rollup Example', 3, 'S', 'S', '', 0, 'insert into companysales selec\
t * from europeansales', 0, 0)");
    IIsyncup((char *)0,0);
  }
/* # line 576 "dualarm.sc" */	/* host code */
	/* next a replicated CDB example */
/* # line 577 "dualarm.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into i_user_alarm_settings values(dbmsinfo('username'), 'Repli\
cated CDB Example', 'Replicate Star Catalogs into \"NY_DDB\" (with CDB\
 \"iiny_ddb\"). Replicate every midnight (GMT) with up to 4 retrys onc\
e/half-hr', '', 0, '1989_01_01 00:00:00 GMT', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'1989_06_12 00:00:00 GMT', '', 1, 'day', '1989_06_12 00:00:00 GMT', '\
', 0, 4, 30, 'minutes', '', '', 0, '', 'N')");
    IIsyncup((char *)0,0);
  }
/* # line 583 "dualarm.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into i_user_alarm_query_text values(dbmsinfo('username'), 'Rep\
licated CDB Example', 0, 'S', 'B', '', 0, 'replicate cdb newyork iiny_\
ddb ingres', 0, 0)");
    IIsyncup((char *)0,0);
  }
/* # line 586 "dualarm.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 589 "dualarm.sc" */	/* host code */
	if (isstar)
	{
/* # line 591 "dualarm.sc" */	/* direct disconnect */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"direct disconnect");
    IIsyncup((char *)0,0);
  }
/* # line 592 "dualarm.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
		err_print(1, "Error doing direct disconnect\n");
/* # line 594 "dualarm.sc" */	/* register */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register i_alarm_settings as link");
    IIsyncup((char *)0,0);
  }
/* # line 595 "dualarm.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
		err_print(1, "Error registering i_alarm_settings\n");
/* # line 597 "dualarm.sc" */	/* register */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register i_user_alarm_settings as link");
    IIsyncup((char *)0,0);
  }
/* # line 598 "dualarm.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
		err_print(1, "Error registering i_user_alarm_settings\n");
/* # line 600 "dualarm.sc" */	/* register */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register i_alarm_query_text as link");
    IIsyncup((char *)0,0);
  }
/* # line 601 "dualarm.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
		err_print(1, "Error registering i_alarm_query_text\n");
/* # line 603 "dualarm.sc" */	/* register */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register i_user_alarm_query_text as link");
    IIsyncup((char *)0,0);
  }
/* # line 604 "dualarm.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
		err_print(1, "Error registering i_user_alarm_query_text\n");
	}
/* # line 608 "dualarm.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 610 "dualarm.sc" */	/* host code */
	err_print(0, "Load data entry and update forms...\n");
/* iialarm.frm now lives in II_SYSTEM/ingres/sig/star -- use NMgtAt */
	NMgtAt( ERx( "II_SYSTEM" ), &ptr ) ;
	if( !ptr || !*ptr )
	{
	    SIprintf(ERx("\nII_SYSTEM must be set to the full pathname of\n"));
	    SIprintf(ERx("\nthe directory where INGRES is installed.\n "));
	    SIprintf(ERx("\nPlease set II_SYSTEM and re-run this program.\n"));
	    PCexit( FAIL ) ;
	}
	buf[0] = 0 ;
	STpolycat( 2, ptr, ERx( "ingres/sig/star/iialarm.frm" ), buf ) ;
	LOfroms( PATH&FILENAME, buf, &formfile ) ;
	LOtos(&formfile, &p);
	STcopy(p, formfilebuf);
	status = UTexe(UT_WAIT, NULL, (PTR)NULL, (PTR)NULL, 
		       "copyform", &clerr,
	               "inflag = %S, replace = %S, database = %S, file = %S",
	               4, "-i", "-r", dbname, formfilebuf);
	if (status)
	{
err_print(1, "Error encountered while loading data entry and update form\n");
	    p = ERget(status);
	    err_print(1, "%s\n", p);
	}
    }
    else
	usage();
}
master()
{
    i4		retval;
    i4		ac_index;
    i4		i;
    PID		pid;
    i4		sargc;
    char	*sargv[10];
    int		status;
    int		sqlca_sqlcode;
    char	*cp;
    char	alarm_id_buf[40];
    char	proc_name[30];
  char alarm_checker_id[4];
  char criteria_node[33];
  char criteria_dbms[33];
  char criteria_database[33];
  char criteria_user[33];
  long alarm_priority;
    T_alarm_checker_id = "master";
    /* delete wakeup and stop event files if they were left hanging around
       from some previous incarnation of this program */
    wakeupexists();
    stopexists();
/* # line 677 "dualarm.sc" */	/* host code */
	/* see if we are the master.  The master enters iialarmdb
	   and figures out the alarm checker assignments and then
	   starts up a subprocess for each alarm checker.  If an
	   alarm checker dies for some reason then the master waits
	   for awhile and then restarts it */
	msg("Master Startup\n");
	sqlca.sqlcode = 0;
	err_print(0, "Connecting to iialarmdb...\n");
/* # line 686 "dualarm.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqConnect(0,(char *)"iialarmdb",(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 688 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		err_print(1, "Could not connect to iialarmdb, exiting\n");
		PCexit(FAIL);
	}
	/* initialize i_alarmed_dbs */
/* # line 695 "dualarm.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update i_alarmed_dbs set alarm_checker_id='def'");
    IIsyncup((char *)0,0);
  }
/* # line 696 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		err_print(1, "Error initializing i_alarmed_dbs\n");
/* # line 699 "dualarm.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 700 "dualarm.sc" */	/* host code */
		PCexit(FAIL);
	}
#ifdef NOTDEF
for now there will be only one alarm checker until we figure out
how to have multiple ones on VMS (see below).  I could not get
a detached process to spawn off suprocesses.  This can be done
but no VMS it is a lot of work (see PCcmdline code for an example
of how hard this is to do  in VMS land)
/* # line 716 "dualarm.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"i_alarm_checkers_csr",7785,6844);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct alarm_checker_id, criteria_node, criteria_dbms, crite\
ria_database, criteria_user, alarm_priority from i_alarm_checkers orde\
r by alarm_priority, alarm_checker_id");
    IIcsQuery((char *)"i_alarm_checkers_csr",7785,6844);
  }
/* # line 717 "dualarm.sc" */	/* host code */
	sqlca_sqlcode = sqlca.sqlcode;
	if (sqlca.sqlcode < 0)
	{
err_print(1, "Error opening i_alarm_checkers cursor - will use default checker\n");
	}
	/* for each row in i_alarm_checkers assign this alarm checker
	   to all matching alarm tables.  The effect of this is that
	   the four 'criteria' are ANDed and multiple rows with the
	   same alarm_checker_id are ORed.  If two alarm_checker_id
	   could service the same alarm table, then the alarm_checker_id
	   with the highest priority number will be assigned. */
	for (; sqlca_sqlcode >= 0;)
	{
/* # line 731 "dualarm.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"i_alarm_checkers_csr",7785,6844,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,3,alarm_checker_id);
      IIcsGetio((short *)0,1,32,32,criteria_node);
      IIcsGetio((short *)0,1,32,32,criteria_dbms);
      IIcsGetio((short *)0,1,32,32,criteria_database);
      IIcsGetio((short *)0,1,32,32,criteria_user);
      IIcsGetio((short *)0,1,30,4,&alarm_priority);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 735 "dualarm.sc" */	/* host code */
		if (sqlca.sqlcode == ZERO_ROWS)
			break;
		else if (sqlca.sqlcode < 0)
		{
			err_print(1, "Error fetching from i_alarm_checkers\n");
			break;
		}
		(void)STtrmwhite(alarm_checker_id);
		/* want to use this name as part of a file name so
		   get rid of any embedded spaces */
		for (cp = alarm_checker_id; *cp; cp++)
			if (*cp == ' ')
				*cp = '_';
		(void)STtrmwhite(criteria_node);
		(void)STtrmwhite(criteria_dbms);
		(void)STtrmwhite(criteria_database);
		(void)STtrmwhite(criteria_user);
/* # line 754 "dualarm.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update i_alarmed_dbs set alarm_checker_id=");
    IIputdomio((short *)0,1,32,0,alarm_checker_id);
    IIwritio(0,(short *)0,1,32,0,(char *)"where alarm_node like ");
    IIputdomio((short *)0,1,32,0,criteria_node);
    IIwritio(0,(short *)0,1,32,0,(char *)"and alarm_dbms like ");
    IIputdomio((short *)0,1,32,0,criteria_dbms);
    IIwritio(0,(short *)0,1,32,0,(char *)"and alarm_database like ");
    IIputdomio((short *)0,1,32,0,criteria_database);
    IIwritio(0,(short *)0,1,32,0,(char *)"and alarm_user like ");
    IIputdomio((short *)0,1,32,0,criteria_user);
    IIsyncup((char *)0,0);
  }
/* # line 761 "dualarm.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			err_print(1, "Error updating i_alarmed_dbs\n");
			break;
		}
	}
	if (sqlca_sqlcode >= 0)
	{
/* # line 770 "dualarm.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"i_alarm_checkers_csr",7785,6844);
  }
/* # line 771 "dualarm.sc" */	/* host code */
	}
	/* determine all the alarm checkers that need to be started.
	   This list may include the checker "iidefault_alarm_checker" */
	ac_index = 0;
/* # line 776 "dualarm.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct alarm_checker_id from i_alarmed_dbs where(alarm_enabl\
ed='y' or alarm_enabled='Y')");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,3,alarm_checker_id);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 780 "dualarm.sc" */	/* host code */
		if (ac_index >= ALARM_CHECKERS_MAX)
		{
		err_print(1, "More than %d alarm checkers, extras ignored\n");
/* # line 783 "dualarm.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
/* # line 784 "dualarm.sc" */	/* host code */
		}
		STcopy(alarm_checker_id, alarm_checker_arr[ac_index].id);
		alarm_checker_arr[ac_index].pid = 0;
		ac_index++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error selecting alarm_checker list\n");
#endif
	err_print(0, "Disconnecting from iialarmdb...\n");
/* # line 794 "dualarm.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 796 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	err_print(1, "Error while disconnecting from iialarmdb, exiting\n");
		PCexit(FAIL);
	}
	sargv[0] = "iialarmchk";
	sargv[1] = "slave";
	sargv[2] = "def";
	for (;;)
	{
	    if ((retval = slave(3, sargv)) == STOP_EVENT)
	    {
		err_print(0, "Slave 'def' has been stopped\n");
		PCexit(0);
	    }
	    else if (retval == WAKEUP_EVENT)
	    {
		err_print(0, "Slave 'def' has been awakened\n");
		continue;
	    }
	    else if (retval == NO_DATABASES)
	    {
		err_print(0, "Slave 'def' has no databases to service\n");
		err_print(0, "Sleep for 1 hour\n");
		if (goodsleep(60 * 60) == STOP_EVENT)
		{
		    err_print(0, "Slave 'def' has been stopped\n");
		    PCexit(0);
		}
		continue;
	    }
	    else
	    {
		err_print(1, "Slave 'def' had error, sleep for 5 minutes then try again\n");
		if (goodsleep(5 * 60) == STOP_EVENT)
		{
		    err_print(0, "Slave 'def' has been stopped\n");
		    PCexit(0);
		}
	    }
	}
#ifdef WONT_WORK_ON_VMS
/*
basically it seems that a detached process may now spawn subprocesses.
The detached process could create other detached processes (I think).
*/
	/* now start up each alarm checker.  Periodically check to see if
	   any have died and if they have restart them. */
	for (;;)
	{
		for(i = 0; i < ac_index; i++)
		{
			pid = alarm_checker_arr[i].pid;
			if (pid) 
			{
				/* see if the process is still around */
				if (pid_probe(pid) == FAIL)
				{
					alarm_checker_arr[i].pid = pid = 0;
		msg("iialarmchk with id:%s failed, restarting\n");
				}
			}
			/* if 0 then start alarm checker */
			if (pid == 0)
			{
			    sargc = 3;
			    sargv[0] = "scheduler ";
			    sargv[1] = "slave ";
			    /* quote this arg to keep its case correct */
			    (void)STpolycat(3, "\"", alarm_checker_arr[i].id,
				"\" ", alarm_id_buf);
			    sargv[2] = alarm_id_buf;
			    /* process name will be 'iialarm_' +
			       installation_id + alarm_checker_id */
			    STcopy("iialarm_", proc_name);
			    if (Instid && *Instid)
				STpolycat(3, proc_name, Instid, "_", proc_name);
			    STcat(proc_name, alarm_checker_arr[i].id);
			    err_print(0, "Spawning alarm checker:%s%s%s\n",
				sargv[0], sargv[1], sargv[2]);
/*
sargv[0]="@ii_system:[ingres.utility]iirunscd ";
sargv[1]=alarm_id_buf;
sargc=2;
err_print(0, "Spawning %s%s\n", sargv[0], sargv[1]);
			    status = PCnspawn(sargc, sargv, (i1)1, (char *) 0,
				(char *) 0, &pid, proc_name);
*/
status = PCcmdline((char *)0,"@ii_system:[ingres.utility]iirunscd dog");
			    if (status != OK)
			msg("Cannot spawn iialarmchk with id:%s\n",
				alarm_checker_arr[i].id);
			    else
				alarm_checker_arr[i].pid = pid;
			}
		}
		/* for now just break.  When pid_probe is written then
		   we can sleep here, and then periodically wake up to
		   see if any slaves have crashed
		goodsleep(3600L); */
		break;
	}
#endif
	PCexit(OK);
}
# define ALARM_TABLE_MAX	1024
struct alarm_tbls {
    char	alarm_node[33];
    char	alarm_dbms[33];
    char	alarm_database[33];
    char	alarm_user[33];
    long	next_time;
    int		alarm_table_enabled;
};
struct alarm_tbls	i_alarmed_dbs[ALARM_TABLE_MAX];
/* for each database for which this slave has responsibility (by 
   checking the alarm assignments in database iialarmdb), connect
   to the database and process any alarms.  When done, sleep for
   a reasonable period of time calculated by remembering the time
   of the next alarm for each of the databases */
slave(argc, argv)
int	argc;
char	**argv;
{
    i4		at_index;
    i4		i;
    long	next_time;
    long	min_time;
    long	next_alarm_seconds;
    SYSTIME	stime;
    i4		test_mode = 0;
    char	*test_id;
    i4		retval;
    i4		wakeup;
  long alarm_sleep_seconds;
  char alarm_node[33];
  char alarm_dbms[33];
  char alarm_database[33];
  char alarm_user[33];
  char dbname[100];
/* # line 957 "dualarm.sc" */	/* host code */
    if (argc < 3)
	usage();
    if (STcompare(argv[1], "test") == 0)
    {
	test_mode = 1;
	T_alarm_checker_id = "test";
    	if (argc != 7)
 	{
	    usage();
	    PCexit(FAIL);
	}
    }
    else if (STcompare(argv[1], "slave") == 0)
    {
	(void)STtrmwhite(argv[2]);
	T_alarm_checker_id = argv[2];
    }
    else
	usage();
    err_print(0, "Slave alarm checker startup...\n");
    if (test_mode)
    {
	STcopy(argv[5], i_alarmed_dbs[0].alarm_node);
	CVupper(argv[4]);
	STcopy(argv[4], i_alarmed_dbs[0].alarm_dbms);
	STcopy(argv[3], i_alarmed_dbs[0].alarm_database);
	STcopy(argv[2], i_alarmed_dbs[0].alarm_user);
	i_alarmed_dbs[0].next_time = 0;
	i_alarmed_dbs[0].alarm_table_enabled = 1;
	if (!CMnmstart(argv[2]))
	{
		err_print(1, "Illegal user name:%s, exiting\n", argv[2]);
		PCexit(FAIL);
	}
	else if (!CMnmstart(argv[3]))
	{
		err_print(1, "Illegal database name:%s, exiting\n", argv[3]);
		PCexit(FAIL);
	}
	else
		at_index = 1;
    }
    else
    {
    /* connect to iialarmdb and read in all entries in i_alarmed_dbs
       to be serviced by this alarm checker. */
    err_print(0, "Connecting to iialarmdb...\n");
/* # line 1009 "dualarm.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqConnect(0,(char *)"iialarmdb",(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 1011 "dualarm.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	err_print(1, "Error connecting to iialarmdb, exiting\n");
	return (ERROR_EVENT);
    }
    at_index = 0;
/* # line 1018 "dualarm.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select alarm_node, alarm_dbms, alarm_database, alarm_user from i_alar\
med_dbs where alarm_checker_id=");
    IIputdomio((short *)0,1,32,0,T_alarm_checker_id);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and(alarm_enabled='y' or alarm_enabled='Y')");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,32,alarm_node);
      IIgetdomio((short *)0,1,32,32,alarm_dbms);
      IIgetdomio((short *)0,1,32,32,alarm_database);
      IIgetdomio((short *)0,1,32,32,alarm_user);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 1024 "dualarm.sc" */	/* host code */
	if (at_index >= ALARM_TABLE_MAX)
	{
		err_print(1, "More than %d alarm table entries, extras ignored\n",
			ALARM_TABLE_MAX);
/* # line 1028 "dualarm.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE2;
      }
/* # line 1029 "dualarm.sc" */	/* host code */
	}
	(void)STtrmwhite(alarm_node);
	STcopy(alarm_node, i_alarmed_dbs[at_index].alarm_node);
	(void)STtrmwhite(alarm_dbms);
	CVupper(alarm_dbms);
	STcopy(alarm_dbms, i_alarmed_dbs[at_index].alarm_dbms);
	(void)STtrmwhite(alarm_database);
	STcopy(alarm_database, i_alarmed_dbs[at_index].alarm_database);
	(void)STtrmwhite(alarm_user);
	STcopy(alarm_user, i_alarmed_dbs[at_index].alarm_user);
	i_alarmed_dbs[at_index].next_time = 0;
	i_alarmed_dbs[at_index].alarm_table_enabled = 1;
	if (!CMnmstart(alarm_user))
		err_print(1, "Illegal user name:%s, skipping this entry\n");
	else if (!CMnmstart(alarm_database))
		err_print(1, "Illegal database name:%s, skipping this entry\n");
	else
	{
		err_print(0, "Will service %s::%s/%s, user:%s\n", alarm_node,
			alarm_database, alarm_dbms, alarm_user);
		at_index++;
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
    if (sqlca.sqlcode < 0)
	err_print(1, "Error selecting alarms for this alarm_checker\n");
    err_print(0, "Exiting from iialarmdb...\n");
/* # line 1060 "dualarm.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 1061 "dualarm.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	err_print(1, "Error disconnecting from iialarmdb, exiting\n");
	return (ERROR_EVENT);
    }
    } /* end of 'else test_mode' */
    if (at_index == 0)
    {
	msg("No alarm tables for this alarm checker.  Exiting...\n");
	return (NO_DATABASES);
    }
    /* For each alarmed database connect to the database and then
       call alarm_check */
    for (;;)
    {
/* # line 1082 "dualarm.sc" */	/* host code */
	TMnow(&stime);
	min_time = stime.TM_secs + (60 * 60);
	for (i = 0;i < at_index; i++)
	{
	    TMnow(&stime);
	    /* skip this database if there are no alarms */
	    if (!i_alarmed_dbs[i].alarm_table_enabled)
		continue;
	    /* skip this database if there are no alarms ready to be
	       services - but remember the next one in this database
	       to be serviced */
	    if ((next_time = i_alarmed_dbs[i].next_time) > stime.TM_secs)
	    {
		if (next_time < min_time)
		    min_time = next_time;
		continue;
	    }
	    /* set up some globals for error reporting */
	    T_node = i_alarmed_dbs[i].alarm_node;
	    T_dbms = i_alarmed_dbs[i].alarm_dbms;
	    T_database = i_alarmed_dbs[i].alarm_database;
	    T_user = i_alarmed_dbs[i].alarm_user;
	    T_alarm_id = (char *) 0;
	    T_alarm_desc = (char *) 0;
	    T_query_seq = 0;
	    /* If DBMS is blank or INGRES do not use '/dbms' suffix since
	       there is a bug in DBMS server.  Always use the "-uusername"
	       flag. */
	    dbname[0] = 0;
	    (void) STpolycat(2, T_node, "::", dbname);
	    (void) STcat(dbname, T_database);
	    if (T_dbms[0] && STcompare(T_dbms, "INGRES"))
		(void) STpolycat(3, dbname, "/", T_dbms, dbname);
	    sqlca.sqlcode = 0;
	    slave_print(0, "Connecting to '%s', user:%s...\n", dbname, T_user);
/* # line 1123 "dualarm.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqUser(T_user);
    IIsqConnect(0,dbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 1124 "dualarm.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		slave_print(1, "Error connecting, skip this database\n");
		continue;
	    }
	    if (test_mode)
		test_id = argv[6];
	    else
		test_id = 0;
	    /* check alarms */
	    retval = alarm_check(&next_alarm_seconds, test_mode, test_id);
	    if (retval == NOMOREALARMS)
		i_alarmed_dbs[i].alarm_table_enabled = 0;
	    else
	    {
		/* remember when the next alarm will be ready */
		TMnow(&stime);
		i_alarmed_dbs[i].next_time = next_time = stime.TM_secs
		    + next_alarm_seconds;
		if (next_time < min_time)
		    min_time = next_time;
	    }
	    sqlca.sqlcode = 0;
	    slave_print(0, "Disconnecting from '%s', user:%s...\n",
		dbname, T_user);
/* # line 1153 "dualarm.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 1154 "dualarm.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		slave_print(1, "Error disconnecting, exiting\n");
		return (retval);
	    }
	}
	if (test_mode)
	{
	    slave_print(0, "Testing complete, exiting\n");
	    return (STOP_EVENT);
	}
	/* if woken up by wakeup file then return and get called
	   from the top (go back and check iialarmdb */
	if (retval == WAKEUP_EVENT || retval == STOP_EVENT)
	    return (retval);
	/* now go to sleep until approximately the next alarm would
	   be fired */
	TMnow(&stime);
	min_time = min_time - stime.TM_secs;
	if (min_time > 0)
	{
	    sqlca.sqlcode = 0;
	    err_print(0, "Go to sleep for %d seconds...\n", min_time);
	    wakeup = goodsleep(min_time);
	    err_print(0, "Waking up from %d second sleep...\n", min_time);
	    /* if woken up by wakeup file then return and get called
	       from the top (go back and check iialarmdb */
	    if (wakeup == WAKEUP_EVENT || wakeup == STOP_EVENT)
		return (wakeup);
	}
    }
    return (WAKEUP_EVENT);
}
# define	QUERYBUF_SIZE	4096
# define	SQL_TYPE	0
# define	BUILTIN_TYPE	1
# define	REPORT_TYPE	2
/* check alarms in the alarm table in <T_node, T_dbms, T_database, T_user> */
alarm_check(next_in_seconds, test_mode, test_id)
long	*next_in_seconds;
int	test_mode;
char	*test_id;
{
    i4		stop_on_error;
    i4		alarm_aborted;
    i4		query_type;
  char querybuf[QUERYBUF_SIZE + 1];
  char alarm_id[33];
  char talarm_id[33];
  char alarm_desc[129];
  long priority;
  long tpriority;
  float overdue;
  float toverdue;
  float fmin;
  long query_seq;
  char failure_action[2];
  char text_dml[2];
  long text_sequence;
  char text_segment[1951];
  long iialarmchk_error;
  long sqlca_sqlcode;
  long query_size;
  long len;
  long next_alarm_seconds;
  short next_alarm_null;
  long guess1;
  long guess2;
  long guess3;
  long guess_interval_next;
  long retry_enabled;
  long tretry_enabled;
  long retry_number;
  long tretry_number;
  char interval_units[10];
  long interval_num;
  char interval1[64];
  char interval2[64];
  char interval3[64];
  char *ip;
  long days;
  long hours;
  long minutes;
  long seconds;
  long secs_per_unit;
	T_alarm_id = alarm_id;
	T_alarm_desc = alarm_desc;
	T_query_seq = &query_seq;
	alarm_id[0] = 0;
	alarm_desc[0] = 0;
	query_seq = 0;
/* # line 1261 "dualarm.sc" */	/* host code */
# ifdef NOTDEF
STAR has bug with sort clause 'overdue'
/*	exec sql DECLARE i_user_alarm_settings_csr CURSOR FOR */
	    SELECT DISTINCT alarm_id, alarm_priority, retry_enabled,
		    retry_number, overdue =
		interval('seconds', date('now') -
		    date(ifnull(next_time, start_time)))
	    FROM i_user_alarm_settings
	    WHERE
		(alarm_enabled = 'Y' or alarm_enabled = 'y')
		and date('now') >= date(ifnull(next_time, start_time))
	    ORDER BY alarm_priority, overdue desc;
# endif
/* # line 1275 "dualarm.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update i_user_alarm_settings set next_time=start_time where next_time\
 is null");
    IIsyncup((char *)0,0);
  }
/* # line 1277 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Error changing null next_time's\n");
	    return (NOMOREALARMS);
	}
	/* for each alarm_id that needs to be executed */
	for (;;)
	{
	    if (wakeupexists())
		return (WAKEUP_EVENT);
	    else if (stopexists())
		return (STOP_EVENT);
		alarm_id[0] = 0;
		alarm_desc[0] = 0;
		query_seq = 0;
/* # line 1299 "dualarm.sc" */	/* host code */
#ifdef NOTDEF
		if (!test_mode)
		{
/*		    exec sql OPEN i_user_alarm_settings_csr; */
		}
#endif
		if (!test_mode && sqlca.sqlcode < 0)
		{
	slave_print(1, "Error opening alarm_setting cursor, exit database\n");
			return (NOMOREALARMS);
		}
		if (test_mode)
		{
		    STcopy(test_id, alarm_id);
		}
		else
		{
		    fmin = 1e10;
/* # line 1320 "dualarm.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct alarm_id, alarm_priority, retry_enabled, retry_number\
, overdue=interval('seconds', date('now') -date(ifnull(next_time, star\
t_time)))from i_user_alarm_settings where(alarm_enabled='Y' or alarm_e\
nabled='y')and date('now')>=date(ifnull(");
    IIwritio(0,(short *)0,1,32,0,(char *)
"next_time, start_time))order by alarm_priority");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,32,talarm_id);
      IIgetdomio((short *)0,1,30,4,&tpriority);
      IIgetdomio((short *)0,1,30,4,&tretry_enabled);
      IIgetdomio((short *)0,1,30,4,&tretry_number);
      IIgetdomio((short *)0,1,31,4,&toverdue);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 1330 "dualarm.sc" */	/* host code */
			/* find minimum - normally there would just be an 'exec sql endselect;'
			   here and the orderby clause would be 'alarm_priority, overdue desc'.
			   However, the 'overdue desc' causes an AV in the STAR server. */
			if (toverdue < fmin)
			{
			    fmin = toverdue;
			    STcopy(talarm_id, alarm_id);
			    priority = tpriority;
			    retry_enabled = tretry_enabled;
			    retry_number = tretry_number;
			    overdue = toverdue;
			}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
/*		    exec sql FETCH i_user_alarm_settings_csr INTO :alarm_id,
			:priority, :retry_enabled, :retry_number, :overdue;*/
		    if (sqlca.sqlcode < 0)
		    {
	slave_print(1, "Error fetching alarm_setting cursor, exit database\n");
/*			exec sql close i_user_alarm_settings_csr; */
			return (NOMOREALARMS);
		    }
		    if (sqlca.sqlcode == ZERO_ROWS)
		    {
/* # line 1356 "dualarm.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1357 "dualarm.sc" */	/* host code */
			if (sqlca.sqlcode < 0)
			slave_print(1, "Error committing when no more alarms\n");
slave_print(0, "No pending entries in i_user_alarm_settings: commit work and exit database\n");
			break ;
		    }
		}
		slave_print(0, "Begin processing this alarm\n");
		alarm_aborted = 0;
		/* process each query for this alarm_id */
		for (query_seq = 0; ;query_seq++)
		{
			/* get all pieces of the query */
			query_size = 0;
			iialarmchk_error = 0;
/* # line 1373 "dualarm.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct text_dml, query_seq, failure_action, text_sequence, t\
ext_segment from i_user_alarm_query_text where alarm_id=");
    IIputdomio((short *)0,1,32,0,alarm_id);
    IIwritio(0,(short *)0,1,32,0,(char *)"and query_seq=");
    IIputdomio((short *)0,1,30,4,&query_seq);
    IIwritio(0,(short *)0,1,32,0,(char *)"order by text_sequence");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,1,text_dml);
      IIgetdomio((short *)0,1,30,4,&query_seq);
      IIgetdomio((short *)0,1,32,1,failure_action);
      IIgetdomio((short *)0,1,30,4,&text_sequence);
      IIgetdomio((short *)0,1,32,1950,text_segment);
      if (IIerrtest() != 0) goto IIrtB4;
/* # line 1383 "dualarm.sc" */	/* host code */
			    len = STlength(text_segment);
			    /* leave room for trailing null */
			    if (len + query_size >= QUERYBUF_SIZE)
			    {
			     slave_print(1, "Query too large; limit is %d bytes\n",
				    QUERYBUF_SIZE);
/* # line 1390 "dualarm.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE4;
      }
/* # line 1391 "dualarm.sc" */	/* host code */
			    }
			    MEcopy(text_segment, (u_i2)len,
				&querybuf[query_size]);
			    query_size += len;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE4:;
  }
			if (STcompare(text_dml, "S") == 0)
			    query_type = SQL_TYPE;
			else if (STcompare(text_dml, "B") == 0)
			    query_type = BUILTIN_TYPE;
			else if (STcompare(text_dml, "R") == 0)
			    query_type = REPORT_TYPE;
			else
			{
			    slave_print(1, "Can only handle S-SQL, B-Builtin, R-Report Writer\n");
			    iialarmchk_error = 2;
			}
			if (sqlca.sqlcode < 0)
			    slave_print(1, "Error retrieving query text\n");
			if (sqlca.sqlcode == ZERO_ROWS)
			    break;
			if (query_size == 0)
			{
				slave_print(1, "Zero length query\n");
				iialarmchk_error = 3;
			}
			stop_on_error = (failure_action[0] == 'S');
			sqlca_sqlcode = 0;
			if (!iialarmchk_error)
			{
			    querybuf[query_size] = 0;
			    if (query_type == BUILTIN_TYPE)
			    {
				slave_print(0, "Executing Builtin:%s\n", querybuf);
				iialarmchk_error =builtin(querybuf, query_size);
				if (iialarmchk_error)
				    slave_print(1, "Error executing builtin\n");
			    }
			    else if (query_type == REPORT_TYPE)
			    {
				slave_print(0, "Executing report command:%s\n", querybuf);
				iialarmchk_error = callreport(querybuf);
				if (iialarmchk_error)
				    slave_print(1, "Error executing report command\n");
			    }
			    else
			    {
				slave_print(0, "Executing:%s\n", querybuf);
/* # line 1442 "dualarm.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(querybuf);
    IIsyncup((char *)0,0);
  }
/* # line 1443 "dualarm.sc" */	/* host code */
				sqlca_sqlcode = sqlca.sqlcode;
				if (sqlca.sqlcode < 0)
				    slave_print(1, "Error executing query\n");
			    }
			}
			if (stop_on_error && (iialarmchk_error ||
				sqlca_sqlcode))
			    alarm_aborted = 1;
			if (alarm_aborted)
			{
		slave_print(1, "Failure code is 'S'.  Aborting this alarm\n");
/* # line 1456 "dualarm.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 1457 "dualarm.sc" */	/* host code */
			    if (sqlca.sqlcode < 0)
				slave_print(1, "Error executing rollback\n");
			}
			/* store the error code */
/* # line 1462 "dualarm.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update i_user_alarm_query_text set iialarmchk_error=");
    IIputdomio((short *)0,1,30,4,&iialarmchk_error);
    IIwritio(0,(short *)0,1,32,0,(char *)", sqlca_sqlcode=");
    IIputdomio((short *)0,1,30,4,&sqlca_sqlcode);
    IIwritio(0,(short *)0,1,32,0,(char *)"where alarm_id=");
    IIputdomio((short *)0,1,32,0,alarm_id);
    IIwritio(0,(short *)0,1,32,0,(char *)"and query_seq=");
    IIputdomio((short *)0,1,30,4,&query_seq);
    IIwritio(0,(short *)0,1,32,0,(char *)"and text_sequence=0");
    IIsyncup((char *)0,0);
  }
/* # line 1469 "dualarm.sc" */	/* host code */
			if (sqlca.sqlcode < 0)
			    slave_print(1, "Error storing error codes\n");
			if (alarm_aborted)
			{
			    /* Alarm has failed.  If this was already a
			       retry from previous failure then bump retry
			       count.  Otherwise setup the first retry if the
			       user has specified retry information */
			    if (retry_enabled)
			    {
				retry_enabled++;
/* # line 1481 "dualarm.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update i_user_alarm_settings set retry_enabled=");
    IIputdomio((short *)0,1,30,4,&retry_enabled);
    IIwritio(0,(short *)0,1,32,0,(char *)"where alarm_id=");
    IIputdomio((short *)0,1,32,0,alarm_id);
    IIsyncup((char *)0,0);
  }
/* # line 1484 "dualarm.sc" */	/* host code */
				if (sqlca.sqlcode < 0)
			    slave_print(1, "Error incrementing retry_enabled\n");
				else
				    slave_print(0, "Retry count incremented\n");
			    }
			    else if (retry_number)
			    {
/* # line 1491 "dualarm.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update i_user_alarm_settings set save_start_time=start_time, save_int\
erval_num=interval_num, save_interval_units=interval_units, retry_enab\
led=1, start_time=date_gmt(date('now')), interval_num=retry_interval_n\
um, interval_units=retry_interval_units where ");
    IIwritio(0,(short *)0,1,32,0,(char *)"alarm_id=");
    IIputdomio((short *)0,1,32,0,alarm_id);
    IIsyncup((char *)0,0);
  }
/* # line 1501 "dualarm.sc" */	/* host code */
				if (sqlca.sqlcode < 0)
				    slave_print(0, "Error enabling retry mode\n");
				else
				    slave_print(0, "Retry mode is now enabled\n");
			    }
			    break;
			}
		}
		/* Turn off retry mode if alarm was successful or if
		   retry count was exceeded */
		if (retry_enabled &&
		    (!alarm_aborted || (retry_enabled > retry_number)))
		{
/* # line 1515 "dualarm.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update i_user_alarm_settings set start_time=save_start_time, interval\
_num=save_interval_num, interval_units=save_interval_units, retry_enab\
led=0 where alarm_id=");
    IIputdomio((short *)0,1,32,0,alarm_id);
    IIsyncup((char *)0,0);
  }
/* # line 1522 "dualarm.sc" */	/* host code */
		    if (sqlca.sqlcode < 0)
			slave_print(1, "Error disabling retry mode\n");
		    else if (alarm_aborted)
		slave_print(0, "Retry count exceeded, retry mode now disabled\n");
		    else
		slave_print(0, "Alarm successful, retry mode now disabled\n");
		}
		/* calculate the next time to schedule the alarm.  This
		   is tricky when the interval_unit is months. */
/* # line 1532 "dualarm.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select interval_units, interval_num, guess_interval_next=int4(interva\
l(interval_units, date('now') -date(start_time))/interval_num) +1 from\
 i_user_alarm_settings where alarm_id=");
    IIputdomio((short *)0,1,32,0,alarm_id);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,9,interval_units);
      IIgetdomio((short *)0,1,30,4,&interval_num);
      IIgetdomio((short *)0,1,30,4,&guess_interval_next);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1547 "dualarm.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		   slave_print(1, "Error selecting initial next interval guess\n");
		/* this should never happen if units are months or greater - if months
		   or greater than this scheme would not work since months are not precise
		   and also updating start_time that originally was on jan 31 to a Feb 28
		   would then never again get 30th and 31st dates */
/* # line 1555 "dualarm.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select secs_per_unit=int4(interval('seconds', '1 ' +interval_units) +\
.1)from i_user_alarm_settings where alarm_id=");
    IIputdomio((short *)0,1,32,0,alarm_id);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&secs_per_unit);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1560 "dualarm.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		    slave_print(1, "Error computing secs_per_unit\n");
		build_interval((guess_interval_next-1) * interval_num,
		    secs_per_unit, interval1);
		build_interval((guess_interval_next) * interval_num,
		    secs_per_unit, interval2);
		build_interval((guess_interval_next+1) * interval_num,
		    secs_per_unit, interval3);
		/* figure out the next time an alarm should occur */
/* # line 1571 "dualarm.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select guess1=interval('seconds', (date(start_time) +");
    IIputdomio((short *)0,1,32,0,interval1);
    IIwritio(0,(short *)0,1,32,0,(char *)
") -date('now')), guess2=interval('seconds', (date(start_time) +");
    IIputdomio((short *)0,1,32,0,interval2);
    IIwritio(0,(short *)0,1,32,0,(char *)
") -date('now')), guess3=interval('seconds', (date(start_time) +");
    IIputdomio((short *)0,1,32,0,interval3);
    IIwritio(0,(short *)0,1,32,0,(char *)
") -date('now'))from i_user_alarm_settings where alarm_id=");
    IIputdomio((short *)0,1,32,0,alarm_id);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&guess1);
      IIgetdomio((short *)0,1,30,4,&guess2);
      IIgetdomio((short *)0,1,30,4,&guess3);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1581 "dualarm.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		   slave_print(1, "Error selecting three next interval guesses\n");
		/* use the smallest guess_interval_next value that yields
		   a next scheduled alarm time after right now */
		if (guess1 > 0)
		    ip = interval1;
		else if (guess2 > 0)  /* This should be the the case except for
					  some boundary cases for months and
					  years */
		    ip = interval2;
		else if (guess3 >= 0)
		    ip = interval3;
		else 
		    /* should probably disable this alarm */
		slave_print(1, "Cannot compute next sched interval:%d:%d:%d\n\n",
			guess1, guess2, guess3);
		/* update the next_time and last_time_run fields */
/* # line 1600 "dualarm.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update i_user_alarm_settings set last_time_run=date_gmt(date('now')),\
 next_time=date_gmt(date(start_time) +");
    IIputdomio((short *)0,1,32,0,ip);
    IIwritio(0,(short *)0,1,32,0,(char *)")where alarm_id=");
    IIputdomio((short *)0,1,32,0,alarm_id);
    IIsyncup((char *)0,0);
  }
/* # line 1604 "dualarm.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		   slave_print(1, "Error setting next_time\n");
		/* we have complete this alarm_id so commit everything
		   (don't want to hold locks too long or have transactions
		   that are too big) */
/* # line 1610 "dualarm.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1611 "dualarm.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		   slave_print(1, "Error commiting for current alarm_id\n");
		/* Do not confuse user if alarm queries were aborted */
		query_seq = 0;
		if (!alarm_aborted)
		    slave_print(0, "WORK COMMITTED\n");
		alarm_id[0] = 0;   /* so does not come out in logging msgs*/
		if (test_mode)
			return (NOMOREALARMS);
	}
	if (wakeupexists())
	    return (WAKEUP_EVENT);
	else if (stopexists())
	    return (STOP_EVENT);
	/* find number of seconds until next alarm, could be negative
	   if there is an alarm that is ready to be serviced right now */
/* # line 1632 "dualarm.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct next_alarm_seconds=min(interval('seconds', date(ifnul\
l(next_time, start_time)) -date('now')))from i_user_alarm_settings whe\
re(alarm_enabled='Y' or alarm_enabled='y')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio(&next_alarm_null,1,30,4,&next_alarm_seconds);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1639 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Error determining seconds to next alarm - use 1 hr\n");
	    next_alarm_seconds = 60 * 60;
	}
	/* notice that 'min' returns null if no rows meet the qualification */
	else if (sqlca.sqlcode == ZERO_ROWS || next_alarm_null == -1)
	{
slave_print(0, "No more alarms for this database, database checking disabled\n");
	    return(NOMOREALARMS);
	}
	*next_in_seconds = next_alarm_seconds;
	return (MOREALARMS);
}
slave_print(type, fmt, arg1, arg2, arg3, arg4, arg5, arg6)
i4	type;
char	*fmt, *arg1, *arg2, *arg3, *arg4, *arg5, *arg6;
{
  char err_msg[257];
    SYSTIME	stime;
    char	timestr[64];
    char	alarm_buf[33];
    char	*cp;
    /* if quiet mode and this is just a status message then ignore */
    if (T_quiet && type == 0)
	return;
    /* if both status and error messages are being displayed then
	print out an indicator to help differentiate the messages */
    if (!T_quiet && type)
	cp = " ***";
    else
	cp = "";
    TMnow(&stime);
    TMstr(&stime, timestr);
    TRdisplay("%s::[iischeduler_%s, '%s']: %s%s\n", T_curnode, Instid,
	T_alarm_checker_id, timestr, cp);
    TRdisplay("    %s::%s/%s, user:%s\n", T_node, T_database, T_dbms, T_user);
    if (T_alarm_id && *T_alarm_id)
    {
	(void)STcopy(T_alarm_id, alarm_buf);
	(void)STtrmwhite(alarm_buf);
	TRdisplay("    alarm_id:%s, query_seq:%d\n", alarm_buf, *T_query_seq);
    }
    if (sqlca.sqlcode < 0)
    {
/* # line 1695 "dualarm.sc" */	/* copy SQLERROR */
  {
    IIeqiqio((short *)0,1,32,256,err_msg,(char *)"errortext");
  }
/* # line 1696 "dualarm.sc" */	/* host code */
	    err_msg[256] = (char)0;
	    msg("    %s", err_msg);
	    /* some error messages do not have terminating newlines */
	    if (err_msg[STlength(err_msg)-1] != '\n')
		msg("\n");
	    sqlca.sqlcode = 0;
    }
    msg("    ");
    msg(fmt, arg1, arg2, arg3, arg4, arg5, arg6);
}
err_print(type, fmt, arg1, arg2, arg3, arg4, arg5, arg6)
i4	type;
char	*fmt, *arg1, *arg2, *arg3, *arg4, *arg5, *arg6;
{
  char err_msg[257];
    SYSTIME	stime;
    char	timestr[64];
    char	*cp;
    /* if quiet mode and this is just a status message then ignore */
    if (T_quiet && type == 0)
	return;
    TMnow(&stime);
    TMstr(&stime, timestr);
    /* if both status and error messages are being displayed then
	print out an indicator to help differentiate the messages */
    if (!T_quiet && type)
	cp = " ***";
    else
	cp = "";
    TRdisplay("%s::[iischeduler_%s, '%s']: %s%s\n", T_curnode, Instid,
	T_alarm_checker_id, timestr, cp);
    if (sqlca.sqlcode < 0)
    {
/* # line 1739 "dualarm.sc" */	/* copy SQLERROR */
  {
    IIeqiqio((short *)0,1,32,256,err_msg,(char *)"errortext");
  }
/* # line 1740 "dualarm.sc" */	/* host code */
	    err_msg[256] = (char)0;
	    msg("    %s", err_msg);
	    /* some error messages do not have terminating newlines */
	    if (err_msg[STlength(err_msg)-1] != '\n')
		msg("\n");
	    sqlca.sqlcode = 0;
    }
    msg("    ");
    msg(fmt, arg1, arg2, arg3, arg4, arg5, arg6);
}
/* When we figure out how to check for the existence of the given process
   id, then put it in this routine.  For now the master will not be able
   to restart a slave that crashes */
pid_probe(pid)
i4	pid;
{
    return (OK);
}
usage()
{
	msg("Usage: scheduler master\n");
/*	msg("       scheduler slave <alarm_checker_id>\n");*/
/*msg("       scheduler test <usr> <database> <dbms> <node> <alarm_id>\n");*/
	msg("       scheduler init master\n");
msg("       scheduler init user [<node>::]<database>[/<dbms>]\n");
msg("       scheduler update [<node>::]<database>[/<dbms>]\n");
/*
	msg("       scheduler wakeup <alarm_checker_id>\n");
	msg("       scheduler wakeupall\n");
	msg("       scheduler stop <alarm_checker_id>\n");
	msg("       scheduler stopall\n");
*/
	msg("       scheduler wakeup\n");
	msg("       scheduler stop\n");
	msg("       scheduler [add|drop|enable|disable] <node> <database> <dbms>\n");
	msg("       scheduler show\n");
	PCexit(FAIL);
}
msg(fmt, arg1, arg2, arg3, arg4, arg5, arg6)
char	*fmt, *arg1, *arg2, *arg3, *arg4, *arg5, *arg6;
{
	TRdisplay(fmt, arg1, arg2, arg3, arg4, arg5, arg6);
	TRflush();
	if (T_isterminal == TRUE)
	{
	    SIprintf(fmt, arg1, arg2, arg3, arg4, arg5, arg6);
	    SIflush(stdout);
	}
}
/* PCsleep has bug that it cannot sleep for more than 6
   minutes (or 3 minutes - I didn't check to see if it did
   unsigned arithmetic) handled .  Larger values cause it to overflow an 
   internal calculation resulting in shorter sleep periods */
/* return 1 if woken up by wakeup file, 0 otherwise */
goodsleep(seconds)
long	seconds;
{
    long	i;
    long	j;
    j = 0;
    for (i = seconds; i > 180; i -= 180)
    {
	/* check for the wakeup file every 9 minutes */
	if (((j++)%3) == 0 && wakeupexists())
	    return(WAKEUP_EVENT);
	if (((j++)%3) == 0 && stopexists())
	    return(STOP_EVENT);
	PCsleep(179000L); /* try to compensate for our wakeup and compute time*/
    }
    PCsleep(i * 1000);
    return (0);
}
/* if years or months, just build a simple date interval string.
   but if seconds, minutes or hours then 'num_units' might overflow
   so normalize to next higher unit.  For instance '40000 minutes'
   is an illegal interval.  Overflow occurs silently and one gets
   a negative number of minutes.  Similar problems for all the units,
   however it is not likely to be encountered except for seconds, 
   minutes and hours. */
build_interval(num_units, secs_per_unit, bp)
long	num_units;
long	secs_per_unit;
char	*bp;
{
    long	seconds;
    long	days;
    long	hours;
    long	minutes;
    char	daybuf[11];
    char	hourbuf[11];
    char	minutebuf[11];
    char	secondbuf[11];
    if (secs_per_unit > 60 * 60 * 24 * 32)
    {
	CVla(num_units, bp);
	STcat(bp, " years");
    }
    else if (secs_per_unit > 60 * 60 * 25)
    {
	CVla(num_units, bp);
	STcat(bp, " months");
    }
    else
    {
	seconds = num_units * secs_per_unit;
	days = seconds / (24 * 60 * 60);
	CVla(days, daybuf);
	seconds -= (days * (24 * 60 * 60));
	hours = seconds / (60 * 60);
	CVla(hours, hourbuf);
	seconds -= (hours * (60 * 60));
	minutes = seconds / (60);
	CVla(minutes, minutebuf);
	seconds -= (minutes * (60));
	CVla(seconds, secondbuf);
	STpolycat(8, daybuf, " days ", hourbuf, " hours ", minutebuf,
	    " minutes ", secondbuf, " seconds", bp);
    }
}
struct copytab {
    char	*table_name;
    char	*file_name;
};
struct copytab copytabarr[] = {
	{"ii_abfdependencies","ii_aa.tmp"},
	{"ii_abfobjects", "ii_ab.tmp"},
	{"ii_encoded_forms", "ii_ac.tmp"},
	{"ii_encodings", "ii_ad.tmp"},
	{"ii_fields", "ii_ae.tmp"},
	{"ii_forms", "ii_af.tmp"},
	{"ii_gropts", "ii_ag.tmp"},
	{"ii_id", "ii_ah.tmp"},
	{"ii_joindefs", "ii_ai.tmp"},
	{"ii_longremarks", "ii_aj.tmp"},
	{"ii_objects", "ii_ak.tmp"},
	{"ii_qbfnames", "ii_am.tmp"},
	{"ii_rcommands", "ii_an.tmp"},
	{"ii_reports", "ii_ao.tmp"},
	{"ii_trim", "ii_ap.tmp"},
	{"iidd_alt_columns", "ii_aq.tmp"},
	{"iidd_columns", "ii_ar.tmp"},
	{"iidd_dbcapabilities", "ii_as.tmp"},
	{"iidd_ddb_dbdepends", "ii_au.tmp"},
	{"iidd_ddb_ldb_columns", "ii_aw.tmp"},
	{"iidd_ddb_ldb_dbcaps", "ii_ax.tmp"},
	{"iidd_ddb_ldb_indexes", "ii_ay.tmp"},
	{"iidd_ddb_ldb_ingtables", "ii_az.tmp"},
	{"iidd_ddb_ldb_keycols", "ii_ba.tmp"},
	{"iidd_ddb_ldb_phytables", "ii_bb.tmp"},
	{"iidd_ddb_ldb_xcolumns", "ii_bc.tmp"},
	{"iidd_ddb_ldbids", "ii_bd.tmp"},
	{"iidd_ddb_long_ldbnames", "ii_be.tmp"},
	{"iidd_ddb_object_base", "ii_bf.tmp"},
	{"iidd_ddb_objects", "ii_bg.tmp"},
	{"iidd_ddb_tableinfo", "ii_bh.tmp"},
	{"iidd_ddb_tree", "ii_bi.tmp"},
	{"iidd_histograms", "ii_bj.tmp"},
	{"iidd_index_columns", "ii_bk.tmp"},
	{"iidd_indexes", "ii_bl.tmp"},
	{"iidd_integrities", "ii_bm.tmp"},
	{"iidd_multi_locations", "ii_bn.tmp"},
	{"iidd_permits", "ii_bo.tmp"},
	{"iidd_procedure", "ii_bq.tmp"},
	{"iidd_registrations", "ii_br.tmp"},
	{"iidd_stats", "ii_bs.tmp"},
	{"iidd_tables", "ii_bt.tmp"},
	{"iidd_views", "ii_bu.tmp"},
	{"i_alarm_settings", "ii_bv.tmp"},
	{"i_alarm_query_text", "ii_bw.tmp"}
    };
# define	WORDARRAYSIZE	10
builtin(query_buf, query_size)
char	*query_buf;
i4	query_size;
{
    i4		count = WORDARRAYSIZE;
    char	*wordarray[WORDARRAYSIZE];
    i4		i;
    char	locbuf[MAX_LOC];
    LOCATION	loc;
  char *tablename;
  char *filename;
  char *node;
  char *database;
  char *dbms;
  int tmp;
  char dbms_type[33];
  char orig_cdbname[33];
/* # line 1948 "dualarm.sc" */	/* host code */
    query_buf[query_size] = 0;
    STgetwords(query_buf, &count, wordarray);
    CVlower(wordarray[0]);
    CVlower(wordarray[1]);
    if (count == 5 && STcompare(wordarray[0], "replicate") == 0 &&
	STcompare(wordarray[1], "cdb") == 0)
    {
	node = wordarray[2];
	database = wordarray[3];
	dbms = wordarray[4];
	/* scheme is:
	    direct connect to CDB
	    copy out catalogs from this CDB using exclusive table locks
		so we get a valid snapshot
	    direct connect to replicate CDB
	    create alarm catalogs if not already there
	    drop all data from catalogs
	    copy in catalogs
	    modify iidd_ddb_ldbids to make CDB entry be the new replicate CDB
	    Note:  user tables in the original CDB are not copied.  This means
	    that there will be danglink links to any user tables in the 
	    original CDB.  If the user wants the user tables copied then 
	    seperate snapshot copies must be set up.
	*/
	dbms_type[0] = 0;
/* # line 1976 "dualarm.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select cap_value from iidbcapabilities where cap_capability='DBMS_TYP\
E'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,32,dbms_type);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1978 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Error checking to see if DDB\n");
	    return (6);
	}
	if (STcompare(dbms_type, "STAR                            "))
	{
	    slave_print(1, "This command is only valid for a Distributed Database\n");
	    return (6);
	}
	/* copy out */
	/*  must commit before the direct connect */
/* # line 1993 "dualarm.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1994 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Error commiting before direct connect for this builtin\n");
	    return (6);
	}
/* # line 1999 "dualarm.sc" */	/* direct connect */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"direct connect");
    IIsyncup((char *)0,0);
  }
/* # line 2000 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Cannot connect\n");
	    return(6);
	}
	/* any returns from this procedure after this point must do
	   a LOreset() */
	LOsave();
/* # line 2010 "dualarm.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where level=table, readlock=exclusive, timeout=6\
0");
    IIsyncup((char *)0,0);
  }
/* # line 2012 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Cannot set lockmode\n");
	    goto errreturn1;
	}
	/* check to see that we don't have any links to the replicate CDB.
	   This will be disallowed since we will replace our the current
	   CDB information in iidd_ddb_ldbids with the replicate CDB and
	   we don't want two entries in iidd_ddb_ldbids for the same LDB */
	tmp = 0;
/* # line 2023 "dualarm.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select 1 from iidd_ddb_ldbids where ldb_node=");
    IIputdomio((short *)0,1,32,0,node);
    IIwritio(0,(short *)0,1,32,0,(char *)"and ldb_dbms=");
    IIputdomio((short *)0,1,32,0,dbms);
    IIwritio(0,(short *)0,1,32,0,(char *)"and ldb_database=");
    IIputdomio((short *)0,1,32,0,database);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tmp);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 2025 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Error checking CDB for reference to replicate CDB\n");
	    goto errreturn1;
	}
	if (tmp == 1)
	{
	    slave_print(1, "Replicate CDB may not be referenced by this DDB, remove appropriate links\n");
	    goto errreturn1;
	}
	/* check that we are not trying to replicate onto ourselves */
	dbms_type[0] = 0;
/* # line 2038 "dualarm.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbmsinfo('database')from iidbconstants");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,32,orig_cdbname);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 2040 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Error checking if replicating CDB onto itself\n");
	    goto errreturn1;
	}
	/* For this error check, assume that the CDB is on the same node as
	   the STAR server and is of the same dbms type. This will handle most
	   cases.  Users can use node name aliases so not a fullproof check. */
	if (STcompare(database, orig_cdbname) == 0 &&
	    STcompare(dbms, T_dbms) == 0 &&
	    STcompare(node, T_node) == 0)
	{
	    slave_print(1, "Attempt to replicate a CDB onto itself\n");
	    goto errreturn1;
	}
	/* use user's current default directory if running from a terminal,
	   otherwise change to ingres/files/alarmtmp */
	if (T_isterminal == FALSE)
	{
	    STcopy("alarmtmp", locbuf);
	    NMloc(FILES, PATH, locbuf, &loc);
	    LOchange(&loc);
	}
	slave_print(0, "About to copy out STAR Catalogs from original CDB...\n");
	for (i = 0; i < sizeof(copytabarr) / sizeof (copytabarr[0]); i++)
	{
	    tablename = copytabarr[i].table_name;
	    filename = copytabarr[i].file_name;
	    /* this is what I really want to do but the ESQL precompiler will
	       not currently support it: */
	    /* exec sql copy table tablename () into :filename; */
	    {
		IIsqInit(&sqlca);
		IIwritio(0,(short *)0,1,32,0,"copy table ");
		IIwritio(0,(short *)0,1,32,0,tablename);
		IIwritio(0,(short *)0,1,32,0,"() into ");
		IIvarstrio((short *)0,1,32,0,filename);
		IIsyncup((char *)0,0);
	    }
	    if (sqlca.sqlcode < 0)
	    {
		slave_print(1, "Cannot copy table:%s\n", tablename);
		goto errreturn1;
	    }
	}
/* # line 2089 "dualarm.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 2090 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Cannot commit after copying tables out\n");
	    goto errreturn1;
	}
/* # line 2096 "dualarm.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where level=session, readlock=session, timeout=s\
ession");
    IIsyncup((char *)0,0);
  }
/* # line 2098 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Cannot reset lockmode\n");
	    goto errreturn1;
	}
/* # line 2104 "dualarm.sc" */	/* direct disconnect */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"direct disconnect");
    IIsyncup((char *)0,0);
  }
/* # line 2105 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Cannot disconnect after copying tables out\n");
	    goto errreturn2;
	}
	/* now copy back into the replicated CDB */
/* # line 2112 "dualarm.sc" */	/* direct connect */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"direct connect with node=");
    IIvarstrio((short *)0,1,32,0,node);
    IIwritio(0,(short *)0,1,32,0,(char *)", database=");
    IIvarstrio((short *)0,1,32,0,database);
    IIwritio(0,(short *)0,1,32,0,(char *)", dbms=");
    IIvarstrio((short *)0,1,32,0,dbms);
    IIsyncup((char *)0,0);
  }
/* # line 2113 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Cannot connect to replicate CDB\n");
	    goto errreturn2;
	}
	IIlq_Protect(TRUE); /* allow updating of catalogs */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Cannot set catalog protection mode\n");
	    goto errreturn1;
	}
/* # line 2125 "dualarm.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 2126 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Cannot commit after setting catalog protection mode\n");
	    goto errreturn3;
	}
/* # line 2132 "dualarm.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where level=table, readlock=exclusive, timeout=6\
0");
    IIsyncup((char *)0,0);
  }
/* # line 2134 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Cannot set lockmode in replicate CDB\n");
	    goto errreturn3;
	}
	/* see if user is the dba of this database - if not disallow the command */
	tmp = 0;
/* # line 2142 "dualarm.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select 1 from iidbconstants where dbmsinfo('username')=dbmsinfo('dba'\
)");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tmp);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 2144 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Error checking dba of replicate CDB\n");
	    goto errreturn3;
	}
	if (tmp == 0)
	{
	    slave_print(1, "Must be dba of replicate CDB\n");
	    goto errreturn3;
	}
	/* alarm catalogs may not yet have been created in the replicate,
	   so create them now */
	crtabs(0, 1, 0); /* user database catalogs, drop first, not verbose */
	/* delete all data from all the catalogs */
	for (i = 0; i < sizeof(copytabarr) / sizeof (copytabarr[0]); i++)
	{
	    tablename = copytabarr[i].table_name;
	    /* this is what I really want to do but our ESQL precompilers do
		do not support it: */
	    /* exec sql delete from tablename; */
	    {
		IIsqInit(&sqlca);
		IIwritio(0,(short *)0,1,32,0,"delete from ");
		IIwritio(0,(short *)0,1,32,0,tablename);
		IIsyncup((char *)0,0);
	    }
	    if (sqlca.sqlcode < 0)
	    {
		slave_print(1, "Cannot delete from table:%s in replicate CDB\n",
		    tablename);
		goto errreturn3;
	    }
	}
	slave_print(0, "About to copy STAR Catalogs into Replicate CDB...\n");
	/* copy in the catalogs */
	for (i = 0; i < sizeof(copytabarr) / sizeof (copytabarr[0]); i++)
	{
	    tablename = copytabarr[i].table_name;
	    filename = copytabarr[i].file_name;
	    /* this is what I really want to do but our ESQL precompilers do not
   		currently allow it: */
	    /* exec sql copy table tablename () from :filename; */
	    {
		IIsqInit(&sqlca);
		IIwritio(0,(short *)0,1,32,0,"copy table ");
		IIwritio(0,(short *)0,1,32,0,tablename);
		IIwritio(0,(short *)0,1,32,0,"() from ");
		IIvarstrio((short *)0,1,32,0,filename);
		IIsyncup((char *)0,0);
	    }
	    if (sqlca.sqlcode < 0)
	    {
		slave_print(1, "Cannot copy table:%s into replicate CDB\n",
		    tablename);
		goto errreturn3;
	    }
	}
/* # line 2207 "dualarm.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iidd_ddb_ldbids set ldb_node=");
    IIputdomio((short *)0,1,32,0,node);
    IIwritio(0,(short *)0,1,32,0,(char *)", ldb_dbms=");
    IIputdomio((short *)0,1,32,0,dbms);
    IIwritio(0,(short *)0,1,32,0,(char *)", ldb_database=");
    IIputdomio((short *)0,1,32,0,database);
    IIwritio(0,(short *)0,1,32,0,(char *)"where ldb_id=1");
    IIsyncup((char *)0,0);
  }
/* # line 2210 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Error updating iidd_ddb_ldbids with Replicate CDB name\n");
	    goto errreturn3;
	}
	if (sqlca.sqlcode == ZERO_ROWS)
	{
	    slave_print(1, "No rows updated when updating iidd_ddb_ldbids with Replicate CDB name\n");
	    goto errreturn3;
	}
/* # line 2221 "dualarm.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 2222 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Cannot commit the replicate CDB (1)\n");
	    goto errreturn3;
	}
/* # line 2228 "dualarm.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where level=session, readlock=session, timeout=s\
ession");
    IIsyncup((char *)0,0);
  }
/* # line 2230 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Cannot reset lockmode in replicate CDB\n");
	    goto errreturn3;
	}
/* # line 2236 "dualarm.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 2237 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Cannot commit the replicate CDB (2)\n");
	    goto errreturn3;
	}
	IIlq_Protect(FALSE); /* allow updating of catalogs */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Cannot disable catalog protect mode in replicate CDB\n");
	    goto errreturn1;
	}
/* # line 2250 "dualarm.sc" */	/* direct disconnect */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"direct disconnect");
    IIsyncup((char *)0,0);
  }
/* # line 2251 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    slave_print(1, "Cannot disconnect from replicate CDB\n");
	    goto errreturn2;
	}
	delallcopytmps();
	LOreset();
	return (0);
    errreturn1:
	delallcopytmps();
/* # line 2263 "dualarm.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 2264 "dualarm.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where level=session, readlock=session, timeout=s\
ession");
    IIsyncup((char *)0,0);
  }
/* # line 2266 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    slave_print(1, "Cannot reset lockmode error(1) return\n");
/* # line 2268 "dualarm.sc" */	/* direct disconnect */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"direct disconnect");
    IIsyncup((char *)0,0);
  }
/* # line 2269 "dualarm.sc" */	/* host code */
	LOreset();
	return (6);
    errreturn2:
	delallcopytmps();
/* # line 2274 "dualarm.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 2275 "dualarm.sc" */	/* host code */
	LOreset();
	return (6);
    errreturn3:
	delallcopytmps();
/* # line 2280 "dualarm.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 2281 "dualarm.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where level=session, readlock=session, timeout=s\
ession");
    IIsyncup((char *)0,0);
  }
/* # line 2283 "dualarm.sc" */	/* host code */
	IIlq_Protect(FALSE);
/* # line 2284 "dualarm.sc" */	/* direct disconnect */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"direct disconnect");
    IIsyncup((char *)0,0);
  }
/* # line 2285 "dualarm.sc" */	/* host code */
	LOreset();
	return (6);
    }
    else
    {
	slave_print(1, "Error - unrecognized command:%s\n", wordarray[0]);
	return (4);
    }
    return(5);
}
/* execute an arbitrary OS command using PCcmdline */
callreport(query_buf)
char	*query_buf;
{
    STATUS	retval;
  char tmp2[36];
    (void) STpolycat(2, "-u", T_user, tmp2);
/* # line 2308 "dualarm.sc" */	/* call */
  {
    IIsqInit(&sqlca);
    IIutsys(0,(char *)"report",(char *)0);
    IIutsys(1,(char *)"database",T_database);
    IIutsys(1,(char *)"report",(char *)"");
    IIutsys(1,(char *)"flags",tmp2);
    IIutsys(2,(char *)0,(char *)0);
  }
/* # line 2310 "dualarm.sc" */	/* host code */
    /* need to check and return status */
    return (0);
}
crtabs(master, dropfirst, verbose)
i4	master;
i4	dropfirst;
i4	verbose;
{
/* # line 2323 "dualarm.sc" */	/* host code */
    if (master)
    {
	if (dropfirst)
	{
/* # line 2327 "dualarm.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop i_alarm_checkers");
    IIsyncup((char *)0,0);
  }
/* # line 2328 "dualarm.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop i_alarmed_dbs");
    IIsyncup((char *)0,0);
  }
/* # line 2329 "dualarm.sc" */	/* host code */
	    /* don't check error status on the above */
	}
	if (verbose)
	    err_print(0, "Creating table 'i_alarm_checkers'...\n");
/* # line 2334 "dualarm.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table i_alarm_checkers(alarm_checker_id varchar(3)not null not\
 default, alarm_align char(3)not null with default, criteria_node varc\
har(32)not null not default, criteria_dbms varchar(32)not null not def\
ault, criteria_database varchar(32)not null ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"not default, criteria_user varchar(32)not null not default, alarm_pri\
ority integer not null with default)");
    IIsyncup((char *)0,0);
  }
/* # line 2367 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error creating i_alarm_checkers\n");
/* # line 2369 "dualarm.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify i_alarm_checkers to btree on alarm_checker_id");
    IIsyncup((char *)0,0);
  }
/* # line 2370 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "errprint && Error modifying i_alarm_checkers\n");
	/*
	** These databases need to be checked for alarms by the specified alarm
	** checker.
	*/
	if (verbose)
	    err_print(1, "Creating table 'i_alarmed_dbs'...\n");
/* # line 2379 "dualarm.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table i_alarmed_dbs(alarm_node varchar(32)not null not default\
, alarm_dbms varchar(32)not null not default, alarm_database varchar(3\
2)not null not default, alarm_user varchar(32)not null not default, al\
arm_conarg0 varchar(8)not null with default, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"alarm_conarg1 varchar(8)not null with default, alarm_conarg2 varchar(\
8)not null with default, alarm_conarg3 varchar(8)not null with default\
, alarm_conarg4 varchar(8)not null with default, alarm_conarg5 varchar\
(8)not null with default, alarm_conarg6 ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"varchar(8)not null with default, alarm_conarg7 varchar(8)not null wit\
h default, alarm_conarg8 varchar(8)not null with default, alarm_conarg\
9 varchar(8)not null with default, alarm_checker_id varchar(3)with nul\
l, alarm_enabled varchar(1)not null with ");
    IIwritio(0,(short *)0,1,32,0,(char *)"default)");
    IIsyncup((char *)0,0);
  }
/* # line 2427 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error creating i_alarmed_dbs\n");
/* # line 2429 "dualarm.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify i_alarmed_dbs to btree unique on alarm_node, alarm_dbms, alarm\
_database, alarm_user");
    IIsyncup((char *)0,0);
  }
/* # line 2431 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error modifying i_alarmed_dbs\n");
	/* make sure user only adds entries for themself */
/* # line 2435 "dualarm.sc" */	/* create integrity */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create integrity on i_alarmed_dbs is alarm_user=dbmsinfo('username')");
    IIsyncup((char *)0,0);
  }
/* # line 2437 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error creating integrity on i_alarmed_dbs\n");
	/* make sure user only accesses/modifies their own entries */
	if (verbose)
	    err_print(1, "Creating table 'i_user_alarmed_dbs'...\n");
/* # line 2443 "dualarm.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view i_user_alarmed_dbs as select * from i_alarmed_dbs where a\
larm_user=dbmsinfo('username')");
    IIsyncup((char *)0,0);
  }
/* # line 2445 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error creating view i_user_alarmed_dbs\n");
/* # line 2447 "dualarm.sc" */	/* create permit */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create permit all on i_user_alarmed_dbs to all");
    IIsyncup((char *)0,0);
  }
/* # line 2448 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error creating permit on i_user_alarmed_dbs\n");
	/*
	** After 'iialarm' sets the alarm_checker_id's in i_alarmed_dbs, it 
	** spawns one process for each alarm checker.  Each alarm checker is
	** given its alarm_checker_id.  Using this id each alarm checker 
	** selects all matching rows from i_alarmed_dbs.  For each matching
	** row the alarm checker connects to the specified database as the
	** specified user and then checks the alarm table for alarms that need
	** to be serviced. After all alarms have been serviced for that alarm
	** table, the next database is accessed and its alarm table is
	** serviced.  When a complete cycle through all alarm tables is made
	** with no alarms to be serviced, then the alarm checker sleeps for
	** the amount of time specified in alarm_sleeptime.  After waking up,
	** the cycle repeats.
	*/
    }
    else /* create user database tables */
    {
	/*
	** The following two tables are created in any database that wishes to
	** have alarm service.  Each user in a database must have their own
	** set of the following two tables.  This is to ensure security (to
	** prevent one user from entering alarm queries for another)
	*/
	if (dropfirst)
	{
/* # line 2477 "dualarm.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop i_alarm_settings");
    IIsyncup((char *)0,0);
  }
/* # line 2478 "dualarm.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop i_alarm_query_text");
    IIsyncup((char *)0,0);
  }
/* # line 2479 "dualarm.sc" */	/* host code */
	    /* don't check error status on the above */
	}
	if (verbose)
	    err_print(1, "Creating table 'i_alarm_settings'...\n");
/* # line 2484 "dualarm.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table i_alarm_settings(alarm_user varchar(32)not null not defa\
ult, alarm_id varchar(32)not null not default, alarm_desc varchar(128)\
not null with default, alarm_align1 char(2)not null with default, alar\
m_priority integer not null with default, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"last_time_run char(25)not null with default, start_time char(25)not n\
ull not default, alarm_align2 char(2)not null with default, interval_n\
um integer not null not default, interval_units char(8)not null not de\
fault, next_time char(25)with null, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"alarm_align3 char(2)not null with default, retry_enabled integer not \
null with default, retry_number integer not null with default, retry_i\
nterval_num integer not null with default, retry_interval_units char(8\
)not null with default, save_start_time char(");
    IIwritio(0,(short *)0,1,32,0,(char *)
"25)not null with default, alarm_align4 char(1)not null with default, \
save_interval_num integer not null with default, save_interval_units c\
har(8)not null with default, alarm_enabled char(1)not null not default\
)");
    IIsyncup((char *)0,0);
  }
/* # line 2559 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error creating i_alarm_settings\n");
/* # line 2562 "dualarm.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify i_alarm_settings to btree unique on alarm_id");
    IIsyncup((char *)0,0);
  }
/* # line 2563 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error modifying i_alarm_settings\n");
	/* make sure user only adds entries for themself */
/* # line 2567 "dualarm.sc" */	/* create integrity */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create integrity on i_alarm_settings is alarm_user=dbmsinfo('username\
')");
    IIsyncup((char *)0,0);
  }
/* # line 2569 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error creating integrity on i_alarm_settings\n");
	/* make sure user only accesses/modifies their own entries */
	if (verbose)
	    err_print(1, "Creating table 'i_user_alarm_settings'...\n");
/* # line 2575 "dualarm.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view i_user_alarm_settings as select * from i_alarm_settings w\
here alarm_user=dbmsinfo('username')");
    IIsyncup((char *)0,0);
  }
/* # line 2577 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error creating i_user_alarm_settings\n");
/* # line 2580 "dualarm.sc" */	/* create permit */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create permit all on i_user_alarm_settings to all");
    IIsyncup((char *)0,0);
  }
/* # line 2581 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error creating permit on i_user_alarm_settings\n");
	if (verbose)
	    err_print(1, "Creating table 'i_alarm_query_text'...\n");
/* # line 2586 "dualarm.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table i_alarm_query_text(alarm_user varchar(32)not null not de\
fault, alarm_id varchar(32)not null not default, query_seq integer not\
 null with default, failure_action char(1)not null not default, text_d\
ml char(1)not null not default, alarm_align1 ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"char(2)not null with default, text_sequence integer not null with def\
ault, text_segment varchar(1900)not null with default, sqlca_sqlcode i\
nteger not null with default, iialarmchk_error integer not null with d\
efault)");
    IIsyncup((char *)0,0);
  }
/* # line 2626 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error creating i_alarm_query_text\n");
/* # line 2629 "dualarm.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify i_alarm_query_text to cbtree unique on alarm_id, query_seq, te\
xt_sequence");
    IIsyncup((char *)0,0);
  }
/* # line 2631 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error modifying i_alarm_query_text\n");
	/* make sure user only adds entries for themself */
/* # line 2635 "dualarm.sc" */	/* create integrity */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create integrity on i_alarm_query_text is alarm_user=dbmsinfo('userna\
me')");
    IIsyncup((char *)0,0);
  }
/* # line 2637 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error creating integrity on i_alarm_query_text\n");
	/* make sure user only accesses/modifies their own entries */
	if (verbose)
	    err_print(1, "Creating table 'i_user_alarm_query_text'...\n");
/* # line 2643 "dualarm.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view i_user_alarm_query_text as select * from i_alarm_query_te\
xt where alarm_user=dbmsinfo('username')");
    IIsyncup((char *)0,0);
  }
/* # line 2645 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error creating i_user_alarm_query_text\n");
/* # line 2648 "dualarm.sc" */	/* create permit */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create permit all on i_user_alarm_query_text to all");
    IIsyncup((char *)0,0);
  }
/* # line 2649 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    err_print(1, "Error creating permit on i_user_alarm_query_text\n");
    }
}
delallcopytmps()
{
    i4		i;
    LOCATION	loc;
    char	locbuf[MAX_LOC];
    for (i = 0; i < sizeof(copytabarr) / sizeof (copytabarr[0]); i++)
    {
	STcopy(copytabarr[i].file_name, locbuf);
	LOfroms(FILENAME, locbuf, &loc);
	LOdelete(&loc);
    }
}
stop()
{
    LOCATION	stop_loc;
    LOCATION	save_loc;
    char	stop_file[MAX_LOC];
    char	save_file[MAX_LOC];
    /* first delete stop file if it exists - this avoids getting multiple
       stop files created in VMS */
    stopexists();
    (void) STpolycat(2, "alarmstp.", /*T_alarm_checker_id*/ "def", stop_file);
    NMloc(FILES, FILENAME, stop_file, &stop_loc);
    LOcopy(&stop_loc, save_file, &save_loc);
    if (LOcreate(&save_loc) != OK)
    {
	msg("Unable to create stop file - probably lacking appropriate permissions.\n");
	msg("Only the INGRES system administrator is allowed to run this command.\n");
    }
}
/* we cannot just create the wakup file since this program does not have
   permission to create the wakeup file in the 'files' directory.  This
   program cannot run suid (on unix) since it needs to run as the user
   in order for the correct database permissions, and userid, to apply
   when checking and updating schedules.  (On VMS we could just install
   this program with SYSPRV but this is not a portable solution). Also note that
   by NOT giving this program excess permissions, the 'stop' command above
   works as desired, namely only the INGRES system administrator (or someone
   with write access to the ../files directory) can perform a stop. */
wakeup()
{
    /* Call the wakeup program */
    LOCATION	wakeup_loc;
    LOCATION	save_loc;
    char	wakeup_name[MAX_LOC];
    char	wakeup_cmd[MAX_LOC + 4];
    CL_ERR_DESC  clerr;
    /* the program name is 'wakeup' */
    /* wakeup now lives in II_SYSTEM/ingres/sig/star -- use NMgtAt */
    char *ptr ;
    char buf[MAX_LOC] ;
    NMgtAt( ERx( "II_SYSTEM" ), &ptr ) ;
    if( !ptr || !*ptr )
    {
      SIprintf( ERx( "\nII_SYSTEM must be set to the full pathname of\n " ) );
      SIprintf( ERx( "\nthe directory where INGRES is installed.\n " ) );
      SIprintf( ERx( "\nPlease set II_SYSTEM and re-run this program.\n " ) );
      PCexit( FAIL ) ;
    }
    buf[0] = 0 ; 
    STpolycat( 2, ptr, ERx( "ingres/sig/star/wakeup" ), buf ) ;
    LOfroms( PATH&FILENAME, buf, &wakeup_loc ) ;
    LOcopy(&wakeup_loc,wakeup_name,&save_loc);
    wakeup_cmd[0] = '\0';
    if (STindex(wakeup_name,"[",0) != NULL)
    {
	/* VMS */
	STcopy("RUN ",wakeup_cmd);
    }
    STcat(wakeup_cmd,wakeup_name);
    if (PCcmdline((char *)0, wakeup_cmd, 0, 0, &clerr) != OK)
    {
	msg("Unable to create wakeup file.\n");
    }
}
/* only called in master or slave mode, never in utility mode */
stopexists()
{
    LOCATION	stop_loc;
    LOCATION	save_loc;
    char	stop_file[MAX_LOC];
    char	save_file[MAX_LOC];
    (void) STpolycat(2, "alarmstp.", /*T_alarm_checker_id*/ "def", stop_file);
    NMloc(FILES, FILENAME, stop_file, &stop_loc);
    LOcopy(&stop_loc, save_file, &save_loc);
    if (LOexist(&save_loc) == OK)
    {
	LOdelete(&save_loc);
	return (TRUE);
    }
    return (FALSE);
}
/* only called in master or slave mode, never in utility mode */
wakeupexists()
{
    LOCATION	wakeup_loc;
    LOCATION	save_loc;
    LOINFORMATION	loinf;
    char	wakeup_file[MAX_LOC];
    char	save_file[MAX_LOC];
    i4		flaginf;
    (void) STpolycat(2, "alarmwkp.", /*T_alarm_checker_id*/ "def", wakeup_file);
    NMloc(FILES, FILENAME, wakeup_file, &wakeup_loc);
    LOcopy(&wakeup_loc, save_file, &save_loc);
    flaginf = LO_I_TYPE ;
    if (LOinfo(&save_loc, &flaginf, &loinf) == OK)
    {
	if (!(flaginf & LO_I_TYPE))
	{
	    err_print(1,"Error detecting type of wakeup file\n");
	    PCexit(FAIL);
	}
	if (loinf.li_type != LO_IS_FILE)
	{
	    err_print(1,"Wakeup file is not a regular file\n");
	    PCexit(FAIL);
	}
	if (LOdelete(&save_loc) != OK)
	{
	    err_print(1,"Error clearing the wakeup file\n");
	    PCexit(FAIL);
	}
	return (TRUE);
    }
    return (FALSE);
}
mastermaint(function, argc, argv)
i4	function;
i4	argc;
char	**argv;
{
/* connect and use dbmsinfo('username') as the username */
  char *database;
  char *node;
  char *dbms;
  char databasebuf[33];
  char nodebuf[33];
  char dbmsbuf[33];
  char userbuf[33];
  char alarmbuf[4];
/* # line 2817 "dualarm.sc" */	/* host code */
    T_alarm_checker_id = "maint";
    node = argv[2];
    database = argv[3];
    dbms = argv[4];
/* # line 2823 "dualarm.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqConnect(0,(char *)"iialarmdb",(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 2825 "dualarm.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	err_print(1, "Error connecting to iialarmdb, exiting\n");
	PCexit(1);
    }
    if (function == 1 && argc == 5) /* add */
    {
/* # line 2832 "dualarm.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into i_user_alarmed_dbs values(");
    IIputdomio((short *)0,1,32,0,node);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,dbms);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,database);
    IIwritio(0,(short *)0,1,32,0,(char *)
", dbmsinfo('username'), '', '', '', '', '', '', '', '', '', '', 'def'\
, 'Y')");
    IIsyncup((char *)0,0);
  }
/* # line 2838 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    err_print(1, "Error adding database, no database added\n");
/* # line 2841 "dualarm.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 2842 "dualarm.sc" */	/* host code */
	}
	else if (sqlca.sqlcode == ZERO_ROWS)
	{
	    err_print(1, "No database added, probable duplicate database specified\n");
	}
	else
	{
	    err_print(1, "One database added\n");
	    wakeup();
	}
    }
    else if (function == 2 && argc == 5) /* drop */
    {
/* # line 2856 "dualarm.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from i_user_alarmed_dbs where alarm_node=");
    IIputdomio((short *)0,1,32,0,node);
    IIwritio(0,(short *)0,1,32,0,(char *)"and alarm_dbms=");
    IIputdomio((short *)0,1,32,0,dbms);
    IIwritio(0,(short *)0,1,32,0,(char *)"and alarm_database=");
    IIputdomio((short *)0,1,32,0,database);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and alarm_user=dbmsinfo('username')");
    IIsyncup((char *)0,0);
  }
/* # line 2860 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    err_print(1, "Error droping database, no database dropped\n");
/* # line 2863 "dualarm.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 2864 "dualarm.sc" */	/* host code */
	}
	else if (sqlca.sqlcode == ZERO_ROWS)
	{
	    err_print(1, "No database dropped, specified database not found\n");
	}
	else
	{
	    err_print(1, "One database dropped\n");
	    wakeup();
	}
    }
    else if (function == 3 && argc == 5) /* enable */
    {
/* # line 2877 "dualarm.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update i_user_alarmed_dbs set alarm_enabled='Y' where alarm_node=");
    IIputdomio((short *)0,1,32,0,node);
    IIwritio(0,(short *)0,1,32,0,(char *)"and alarm_dbms=");
    IIputdomio((short *)0,1,32,0,dbms);
    IIwritio(0,(short *)0,1,32,0,(char *)"and alarm_database=");
    IIputdomio((short *)0,1,32,0,database);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and alarm_user=dbmsinfo('username')");
    IIsyncup((char *)0,0);
  }
/* # line 2882 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    err_print(1, "Error enabling database, no database enabled\n");
/* # line 2885 "dualarm.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 2886 "dualarm.sc" */	/* host code */
	}
	else if (sqlca.sqlcode == ZERO_ROWS)
	{
	    err_print(1, "No database enabled, specified database not found\n");
	}
	else
	{
	    err_print(1, "One database enabled\n");
	    wakeup();
	}
    }
    else if (function == 4 && argc == 5) /* disable */
    {
/* # line 2899 "dualarm.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update i_user_alarmed_dbs set alarm_enabled='N' where alarm_node=");
    IIputdomio((short *)0,1,32,0,node);
    IIwritio(0,(short *)0,1,32,0,(char *)"and alarm_dbms=");
    IIputdomio((short *)0,1,32,0,dbms);
    IIwritio(0,(short *)0,1,32,0,(char *)"and alarm_database=");
    IIputdomio((short *)0,1,32,0,database);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and alarm_user=dbmsinfo('username')");
    IIsyncup((char *)0,0);
  }
/* # line 2904 "dualarm.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    err_print(1, "Error disabling database, no database disabled\n");
/* # line 2907 "dualarm.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 2908 "dualarm.sc" */	/* host code */
	}
	else if (sqlca.sqlcode == ZERO_ROWS)
	{
	    err_print(1, "No database disabled, specified database not found\n");
	}
	else
	{
	    err_print(1, "One database disabled\n");
	    wakeup();
	}
    }
    else if (function == 5 && argc == 2) /* show */
    {
	SIprintf("NODE              DATABASE          DBMS              USER              ENABLED\n");
/* # line 2922 "dualarm.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select alarm_node, alarm_dbms, alarm_database, alarm_user, alarm_enab\
led from i_user_alarmed_dbs");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,32,nodebuf);
      IIgetdomio((short *)0,1,32,32,dbmsbuf);
      IIgetdomio((short *)0,1,32,32,databasebuf);
      IIgetdomio((short *)0,1,32,32,userbuf);
      IIgetdomio((short *)0,1,32,3,alarmbuf);
      if (IIerrtest() != 0) goto IIrtB5;
/* # line 2927 "dualarm.sc" */	/* host code */
	    SIprintf("%-18s%-18s%-18s%-18s%c\n", nodebuf, databasebuf, dbmsbuf,
		userbuf, alarmbuf[0]);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE5:;
  }
	if (sqlca.sqlcode < 0)
	{
	    err_print(1, "Error listing entries\n");
/* # line 2933 "dualarm.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 2934 "dualarm.sc" */	/* host code */
	}
    }
    else
    {
/* # line 2938 "dualarm.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 2939 "dualarm.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 2940 "dualarm.sc" */	/* host code */
	usage();
    }
/* # line 2943 "dualarm.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 2944 "dualarm.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 2945 "dualarm.sc" */	/* host code */
}
