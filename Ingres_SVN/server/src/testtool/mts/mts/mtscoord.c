# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	mtscoord.sc
**
**	This program is the coordinator for MTS
**
*/
/*
**	modification history
**
**	who	when		what
**	---	----		---------------------------------------------
**
**	jds	2/14/92		add a 'drop table start' and 'commit' before
**				re-creating it.
**				add comments in routine clean_up
**				
**				=============================================
**
**
**	jds	3/4/92		drop ref to me.h - never gets used, not 
**				there anyway...
**
** 	bh	6/25/92		added libraries for ming - NEEDLIBS	
**
** 02/03/93 (erniec)
** allow error-checking-only option if # slaves set to 0, and trace = 1
**	syntax: mtscoord db_name    cc_test num_o_kids traceflag
**	ex      mtscoord mtsddb1/d  0       0          1 
** coord_sync: 
**	Increase sleep time waiting for slaves to start to 60 reps * 60000 (1hr)
**	print slave status trace if traceflag>1
** awaken_sync: 
**	Increase sleep time checking number of active slaves to 150000
**	print slave status trace if traceflag>1
** check_errors: 
**	always print a list of all terminating errors encountered
**	print a list of acceptable, expected errors if traceflag is set
**	23-dec-1997 (hanch04)
**	    Added FAIL to PCexit
**  30-Sep-1998 (merja01)
**     Change sql long types to i4 to make test protable to axp_osf.
**	25-may-1999 (somsa01)
**	    Include st.h .
**  13-jun-2000 (stephenb)
**     Add code to check for Star connections. In this case we need to unset
**     ddl concurrency to prevent hangs (bug 100282). Note: this change
**     can be removed when the bug is fixed.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
/* ming hints 
**
NEEDLIBS = MTSLIB LIBINGRES
**
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include	<time.h>
# include       <compat.h>
# include       <si.h>
# include	<st.h>
# include       <mh.h>
# include       <lo.h>
# include       <pc.h>
# include       <st.h>
# include       "trl.h"
/* 
** traceflag is a variable that is set by the user when he/she desires to see
** status information about the slaves. When traceflag is not set all the
** information that might get differences with the canons is filtered. This
** flag should be set when the test is not executed as part of the test suite.
*/
GLOBALDEF i4	traceflag;
/*
**	serial_list is the data structure for the list of routines the coord 
**	will execute. Each list member contains the routine number, the number
**	of the slave which executed it, the routine's count, and a pointer to 
**	the next member in the list.
*/
struct serial_list {
	i4	number;
	i4	slave;
	i4	count;
	struct	serial_list	*next;
	};
/*
**	tbl_list stores a relation (or attribute) name, and a pointer to the
**	next name in the list.
**
**	in addition, a field "type" was added so the same structure could be 
**	used in the init_tbls routine. the type field tells us whether the table
**	is initially empty or needs to be loaded. the field is unused in 
**	other routines. 
*/
struct tbl_list {
	char	tbl[13];
	char	type[2];
	struct	tbl_list	*next;
	};
/*
** COORDINATOR -
**	This is the coordinating program of MTS 
**  Its algorithm:
**
**	process arguments (# of slaves, cc_test number)
**	initialize data and log tables
**	sychronize slaves to start them
**	`sleep; synchonrize to awaken when all have ended
**
**	Parameters -
**		argc -	argument count
**		argv -	argument array
**		 [0] -	points to the directory spec of this executable
**		 [1] -	database name
**		 [2] -	concurrency test number
**		 [3] -  number of slave programs to manage
**		 [4] -  traceflag
*/
main(argc, argv)
i4		argc;
char		*argv[];
{
  char *dbname =argv[1];
	i4	num_kids; 		/* number of slaves to manage */
	i4	cctest_num;		/* which concurrency test this is */
	struct	tbl_list	*datalist;  /* holds list of data tables used */
	i4	baderror;		/* flag to denote serious errors in slaves */
/*	Use DBMS specific and NOT generic error messages */
/* # line 144 "mtscoord.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(3,(short *)0,1,32,0,(char *)"dbmserror");
  }
/* # line 146 "mtscoord.sc" */	/* host code */
/* 	PROCESS ARGUMENTS	 */
	proc_args(argc,argv,&num_kids,&cctest_num,&traceflag);
	SIprintf ("COORDINATOR: Opening database...\n");
	SIflush(stdout);
/* # line 154 "mtscoord.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqConnect(0,dbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
    if (sqlca.sqlcode < 0) 
      IIsqStop(&sqlca);
  }
/* # line 158 "mtscoord.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 159 "mtscoord.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set autocommit on");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 161 "mtscoord.sc" */	/* host code */
	if (STbcompare("/star", 5, &dbname[STlength(dbname) - 5], 5, TRUE) == 0)
	{
	    /* this is a star connection */
/* # line 164 "mtscoord.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set ddl_concurrency off");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 165 "mtscoord.sc" */	/* host code */
	}
	if (num_kids != 0)	/* just check error messages if no slaves */
	{
/*		SYNCHRONIZE SLAVES TO START THEM	*/
		SIprintf ("COORDINATOR: Going to synchronize %d kids now\n",num_kids);
		SIflush(stdout);
		/* get the coord and all slaves starting at the same time */
		coord_sync(num_kids);
		SIprintf ("COORDINATOR: waiting till they finish...\n");
		SIflush(stdout);
/*		SYNCHRONIZE TO AWAKEN WHEN ALL SLAVES ARE FINISHED	*/
		awaken_sync();
	}
/*	Check for errors encountered by the slaves	*/
	SIprintf ("\nCOORDINATOR: checking for errors encountered by the slaves.\n");
	SIflush(stdout);
	baderror = FALSE;
	check_errors (&baderror);
	if (baderror)
	{
		SIprintf ("COORDINATOR: Serious error(s) encountered by the slave(s).\n");
		SIflush(stdout);
	}
	else
	{
		SIprintf ("COORDINATOR: No serious errors found. \n");
		SIflush(stdout);
	}
	if (num_kids != 0)	/* just check error messages if no slaves */
	{
/*		Clean-up and EXIT */
		SIprintf ("COORDINATOR: cleaning up...\n");
		SIflush(stdout);
		cleanup(num_kids);
		SIprintf ("COORDINATOR: testing completed.\n");
		SIflush(stdout);
	}
/* # line 218 "mtscoord.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 219 "mtscoord.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 221 "mtscoord.sc" */	/* host code */
	PCexit(OK);
}
/*
** PROC_ARGS
**	Process the arguments. Find out if the correct number of arguments 
**	present, and how many slaves we have. 
**
**	Parameters -
**		argcount  - 	number of arguments passed to main
**		argvector - 	element [3] has the number of slaves
**		slaves    -	holds the number of slaves 
**		cctest	  -	to hold the cctest number we're executing
**		trace 	  -	trace flag
*/
proc_args(argcount,argvector,slaves,cctest,trace)
i4	argcount;
char	*argvector[];
i4	*slaves;
i4	*cctest;
i4 *trace;
{
	i4	status; 	/* error status	*/
/*	Use DBMS specific and NOT generic error messages */
/* # line 246 "mtscoord.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(3,(short *)0,1,32,0,(char *)"dbmserror");
  }
/* # line 248 "mtscoord.sc" */	/* host code */
	if (argcount != 5)
	{
		SIprintf("The syntax is: mtscoord db_name cc_test num_o_kids traceflag\n");
		SIflush(stdout);
		PCexit(FAIL);
	}
	if (( status=CVal(argvector[2],cctest) ) != OK)
	{
		SIprintf("%s is an invalid concurrency test number \n",
							        argvector[2]);
		SIflush(stdout);
		PCexit(FAIL);
	}
	if (( status=CVal(argvector[3],slaves) ) != OK)
	{
		SIprintf("%s is an invalid number of slave processes\n",
							        argvector[3]);
		SIflush(stdout);
		PCexit(FAIL);
	}
	if (( status=CVal(argvector[4],trace) ) != OK )
	{
		SIprintf("%s is an invalid trace flag \n",
							        argvector[4]);
		SIflush(stdout);
		PCexit(FAIL);
	}
} /* end proc_args */
/*
**	COORD_SYNC -	Sync up the coord and all it's slaves, 
**			such that they all exit this routine
**					at the same time.
**
**	Parameters -
**		no_of_slaves	how many slaves we expect to sync up.
**
*/
coord_sync(no_of_slaves)
  i4 no_of_slaves;
{
  i4 started_slaves;
	i4		loop = 0;
/*	Use DBMS specific and NOT generic error messages */
/* # line 302 "mtscoord.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(3,(short *)0,1,32,0,(char *)"dbmserror");
  }
/* # line 304 "mtscoord.sc" */	/* host code */
/* 
**	Find out if all the slaves have gotten their numbers and are ready to 
**	go. Each slave will append its number to slaves_up.
**	By counting how many rows the table has, the coordinator can tell
**	how many slaves are ready to go.
*/
	do 
	{ 
		started_slaves = 0;
/* # line 315 "mtscoord.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select count(*)from slaves_u\
p");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&started_slaves);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 319 "mtscoord.sc" */	/* host code */
		if (traceflag > 1)
		{
			SIprintf ("COORDINATOR: %d slaves out of %d are up\n",
					started_slaves, no_of_slaves);
			SIflush(stdout);
		}
		PCsleep(60000);		
	} while ((started_slaves != no_of_slaves) && (loop++ < 60));
	if (loop < 60)
	{
/* FIRST CRUCIAL TIME POINT -- the coordinator knows the slaves are ready.*/
/* # line 335 "mtscoord.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set autocommit off");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 337 "mtscoord.sc" */	/* host code */
/*
**	drop the START table - it's possible that it's still
**	around, which causes no END of problems - perhaps
**	previous test failed to complete and clean up...
*/
/* # line 343 "mtscoord.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table start");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 344 "mtscoord.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 346 "mtscoord.sc" */	/* host code */
/*
**	create the relation that tells the slaves that they can start
**	running.
*/
/* # line 351 "mtscoord.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table start(num small\
int)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 352 "mtscoord.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"insert into start(num)values\
(0)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 354 "mtscoord.sc" */	/* host code */
/* SECOND CRUCIAL TIME POINT -- the slaves know the coordinator is ready */
		SIprintf ("COORDINATOR: start table exists.\n");
		SIflush(stdout);
/*
** all the slaves are now ready and waiting to go. Releasing the lock
** on the start relation will allow them to run. 
*/
/* # line 363 "mtscoord.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 365 "mtscoord.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set autocommit on");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 366 "mtscoord.sc" */	/* host code */
	}
	else
	{
		SIprintf("COORDINATOR: Terminating because only %d slaves out of %d are up\n",
                                               started_slaves, no_of_slaves);
		SIflush(stdout);
/* # line 372 "mtscoord.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 373 "mtscoord.sc" */	/* host code */
		exit(1);
	}
}	/* end coord_sync */
/*
**	AWAKEN_SYNC --
**		Waits here until all the slaves have finished executing
**	their routines. Each slave deletes one row from the slaves_up
**	table when it is done so the coordinator can tell how many slaves 
**	are up. 
*/
awaken_sync()
{
  i4 running_kids;
/*	Use DBMS specific and NOT generic error messages */
/* # line 394 "mtscoord.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(3,(short *)0,1,32,0,(char *)"dbmserror");
  }
/* # line 396 "mtscoord.sc" */	/* host code */
	do 
	{ 
/* # line 398 "mtscoord.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select count(*)from slaves_u\
p");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&running_kids);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 402 "mtscoord.sc" */	/* host code */
		if (traceflag > 1)
		{
			SIprintf ("COORDINATOR: %d slaves are STILL up\n",running_kids);
			SIflush(stdout);
		}
		if (running_kids > 0) 
			PCsleep(150000);	
	} while (running_kids > 0);
} /* end awaken_sync */
/*
** CLEANUP -
**	destroys the start table which is used for synchronization purposes
**	and deletes any rows in the slaves_up table.
**
**	Parameters -
**		slaves	- how many slaves there are
*/
cleanup(slaves)
i4	slaves;
{
/*	Use DBMS specific and NOT generic error messages */
/* # line 430 "mtscoord.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(3,(short *)0,1,32,0,(char *)"dbmserror");
  }
/* # line 432 "mtscoord.sc" */	/* host code */
	/* First destroy the table that has the number of running slaves */
/* # line 434 "mtscoord.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from slaves_up");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 436 "mtscoord.sc" */	/* host code */
/*
**	This table is also dropped
**		- in the setup script
**		- just before it is created in this program
**
**	jds	2/14/92
**
*/
/* # line 445 "mtscoord.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table start");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 448 "mtscoord.sc" */	/* host code */
} /* end cleanup */
/*
**	CHECK_ERRORS --
**	Review the ``err_log'' table for errors encountered by the slaves.
**
*/
check_errors (baderror)
i4	*baderror;
{
  char letter[2];
  int number;
  int amount;
	i4	status;
/*	Use DBMS specific and NOT generic error messages */
/* # line 466 "mtscoord.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(3,(short *)0,1,32,0,(char *)"dbmserror");
  }
/* # line 468 "mtscoord.sc" */	/* host code */
/*
** 	Lets see how the test did. Look at the err_log table and report all
** 	INGRES "errors" (including deadlock and force-abort-limit)
*/
/* # line 473 "mtscoord.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select status, err_num, count(status)from err_msg group by status, er\
r_num");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,1,letter);
      IIgetdomio((short *)0,1,30,4,&number);
      IIgetdomio((short *)0,1,30,4,&amount);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 478 "mtscoord.sc" */	/* host code */
		if ((status=STcompare(letter,"A")) == 0)
		{
			SIprintf(" *** %d TERMINATING ERROR(s) on AUTOCOMMIT OFF, error number %d \n", amount,number);
			SIflush(stdout);
			*baderror = TRUE; 
		}
		else if ((status=STcompare(letter,"B")) == 0)
		{
			SIprintf(" *** %d TERMINATING ERROR(s) on COMMIT, error number %d \n", amount,number);
			SIflush(stdout);
			*baderror = TRUE; 
		}
		else if ((status=STcompare(letter,"C")) == 0)
		{
			SIprintf(" *** %d TERMINATING ERROR(s), error number %d \n", amount,number);
			SIflush(stdout);
			*baderror = TRUE; 
		}
		else if ((status=STcompare(letter,"a")) == 0  && traceflag)
		{
			SIprintf(" *** %d ERR_DEADLOCK error(s), error number %d \n", amount,number);
			SIflush(stdout);
		}
		else if ((status=STcompare(letter,"b")) == 0  && traceflag)
		{
			SIprintf(" *** %d GE_SERIALIZATION error(s), error number %d \n", amount,number);
			SIflush(stdout);
		}
		else if ((status=STcompare(letter,"c")) == 0  && traceflag)
		{
			SIprintf(" *** %d ERR_OUT_OF_LOCKS error(s), error number %d \n", amount,number);
			SIflush(stdout);
		}
		else if ((status=STcompare(letter,"d")) == 0  && traceflag)
		{
			SIprintf(" *** %d ERR_LOGFULL error(s), error number %d \n", amount,number);
			SIflush(stdout);
		}
		else if ((status=STcompare(letter,"e")) == 0  && traceflag)
		{
			SIprintf(" *** %d GE_OTHER_ERROR error(s), error number %d \n", amount,number);
			SIflush(stdout);
		}
		else if ((status=STcompare(letter,"f")) == 0  && traceflag)
		{
			SIprintf(" *** %d ERR_TIMEOUT error(s), error number %d \n", amount,number);
			SIflush(stdout);
		}
		else if ((status=STcompare(letter,"g")) == 0  && traceflag)
		{
			SIprintf(" *** %d USER_PASSWORD error(s), error number %d \n", amount,number);
			SIflush(stdout);
		}
		else if ((status=STcompare(letter,"h")) == 0  && traceflag)
		{
			SIprintf(" *** %d ERR_QUERYABORT error(s), error number %d \n", amount,number);
			SIflush(stdout);
		}
		else if ((status=STcompare(letter,"i")) == 0  && traceflag)
		{
			SIprintf(" *** %d ERR_CATDEADLK error(s), error number %d \n", amount,number);
			SIflush(stdout);
		}
		else if ((status=STcompare(letter,"j")) == 0  && traceflag)
		{
			SIprintf(" *** %d ERR_RQF_TIMEOUT error(s), error number %d \n", amount,number);
			SIflush(stdout);
		}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
    if (sqlca.sqlcode < 0) 
      merr();
  }
}	/* end check_errors */
