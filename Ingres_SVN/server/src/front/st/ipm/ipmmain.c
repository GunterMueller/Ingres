# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  Copyright (c) 2004 Ingres Corporation
**  All rights reserved.
*/
#include <compat.h>
#include <dbms.h>
#include <fe.h>
#include <cs.h>
#include <pc.h>
#include <lk.h>
#include <cx.h>
#include <lg.h>
#include <er.h>
#include <me.h>
#include <si.h>
#include <st.h>
#include        <adf.h>
#include        <ft.h>
#include        <fmt.h>
#include        <frame.h>
#include "lists.h"
#include "msgs.h" 
#include "dba.h"
#include "servinfo.h"
/*
**
**  File: ingdba.qc
**
**  Purpose - this is the top routine for the new lockinfo for R6.  this
**	version does more.  In addition to displaying lock information,
**	it displays a list of servers and allows viewing and manipulation
**      of the servers/sessions in each server.
**
**  This file contains:
**	main - top level routine
**	dbamain - displays the main form
**      process_args - validates the command line options and sets globals
**	log_submenu - log submenu for the main INGDBA screen
**	lock_submenu - lock submenu for the main INGDBA screen
**
**  History
**	1/6/89		tomt	created
**	8/16/89		tomt	added logging system functionality
**	9/6/89		tomt	added frs timeout option on startup
**	9/8/89		tomt	broke submenus into seperate procedures
**	9/19/89		tomt	now ADDFORM in routines where forms are displayed
**      3/12/90         tomt    added new startup options to filter out log/lock
**	18 Sept 1990	jpk	modified top frame to conform to forms
**				standards: no duplicate menu items for
**				things defined in tablefield, no Help
**				or Quit in tablefield, "Select" instead of "Go"
**      16-nov-1992 (larrym)
**          Added CUFLIB to NEEDLIBS.  CUF is the new common utility facility.
**	13 Dec 1992	jpk	added DMFLIB, stubbed out ule_format
**				(still not quite right but ok for now)
**      5/26/93 (vijay)
**		include pc.h
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**	10-mar-95 (reijo01) Bug 59221#
**	 	Cleaned up help information. added -h flag for help.	
**	18-Dec-97 (gordy)
**	    Added SQLCALIB to NEEDLIBS, now required by LIBQ.
**	20-May-1998 (hanch04)
**	    Changed name of ule_format to ule_doformat for new macro.
**      21-apr-1999 (hanch04)
**          Added st.h
**	12-jun-2001 (devjo01)
**	    Replace LGcluster() with CXcluster_enabled(), add cx.h (s103715)
**      25-Sep-2002 (hanch04)
**          PCexeclp64 needs to only be called from the 32-bit version of
**          the exe only in a 32/64 bit build environment.
**	02-oct-2002 (devjo01)
**	    Set NUMA context with CXget_context.
**
***************************
**Various flags are available:
**
**	Flag		Action					
**
**	-f		writes output to "locks.out:		(DISABLED)
**	-ffile		writes output to "file"			(DISABLED)
**	(no f)		writes output to sys$output
**	-ddbname	process only locks on a particular db
**	-r		set frs timeout for certain screens
**	(no r)		use default of 0 for timeout
**	-l		process all INGRES locks,any type		
**	-llocktype	process only INGRES locks of type locktype(II, lr ..)  
**	(no l)		process only II, lr, dr, and gp locks
**	-n		print NL granted locks		
**	(no n)		don't print NL granted locks
**	-t		process only locks for specified table (must specify db)
**	(no t)		don't screen locks based on any particular table
**      -i              display inactive and other transactions in tx display
**      (no i)          suppress display of inactive transactions (default)
**      -e              display system locklists in lock list display
**      (no e)          suppress display of system lock lists
**
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	13-oct-2001 (somsa01)
**	    Added GLOBALDEF of 64-bit form.
**	06-sep-2002 (somsa01)
**	    For the 32/64 bit build,
**	    call PCexeclp64 to execute the 64 bit version
**	    if II_LP64_ENABLED is true.
**	11-sep-2002 (somsa01)
**	    Prior change is not for Windows.
**	29-Sep-2004 (drivi01)
**	    Updated NEEDLIBS to link dynamic library SHQLIB and SHFRAMELIB
**	    to replace its static libraries. Added NEEDLIBSW hint which is 
**	    used for compiling on windows and compliments NEEDLIBS.
*/
/*
**      MKMFIN Hints
**
PROGRAM = ipm
NEEDLIBS =      IPMLIB COMPATLIB SHFRAMELIB SHQLIB DMFLIB 
NEEDLIBSW = 	SHGCFLIB SHEMBEDLIB
UNDEFS =        II_copyright ule_doformat
*/
   GLOBALREF int *fdispdb;
   GLOBALREF int *fdisplgtx;
   GLOBALREF int *fmorelkres;
   GLOBALREF int *fshowlkres;
   GLOBALREF int *fmorelgdb;
   GLOBALREF int *fsessdetail;
   GLOBALREF int *fsessdetail64;
   GLOBALREF int *fshowopts;
   GLOBALREF int *fservdetail;
   GLOBALREF int *fshowlksummno;
   GLOBALREF int *fmorelklock;
   GLOBALREF int *fmorelghdtx;
   GLOBALREF int *fdisplgdb;
   GLOBALREF int *fshowlklock;
   GLOBALREF int *fdisplkres;
   GLOBALREF int *fmorelklists;
   GLOBALREF int *fshowblock;
   GLOBALREF int *fshowlgsumm;
   GLOBALREF int *fshowlksumm;
   GLOBALREF int *fdisplklists;
   GLOBALREF int *fshowlghdr;
   GLOBALREF int *fshowsess;
   GLOBALREF int *fshowsess64;
   GLOBALREF int *fdispsrv;
   GLOBALREF int *fdisplgproc;
   GLOBALREF int *fmorelgtx;
   GLOBALREF int *fdisplktype;
   GLOBALREF int *fqrytext;
  GLOBALREF int *fingdba;
  GLOBALREF int *flockmenu;
  GLOBALREF int *flogmenu;
  GLOBALREF int *fmyhelp;
VOID sys_init();
VOID process_args();
VOID ingdba();
VOID read_iidbdb();
VOID enable_frs();
VOID dbamain();
VOID lklginfo();
VOID get_lklgstats();
VOID set_options();
VOID setlangcode();
VOID syntax_error();
VOID display_help();
VOID lock_submenu();
VOID log_submenu();
VOID decl_errhndl();
FUNC_EXTERN VOID close_db();
i4  getlockcat();
# define MENUITEM1 1
# define MENUITEM2 2
# define MENUITEM3 3
# define MENUITEM4 4
# define MENUITEM5 5
# define MENUITEM6 6
# define MENUITEM7 7
/* Menu items for main menu */
  static struct _menu_choices {
    char option[15];
    char description[52];
    int num;
  } menu_choices[]= {
	{"Server_List", "Display a list of servers to examine", MENUITEM1},
	{"Lock_Info", "Display locks and related information", MENUITEM2},
	{"Log_Info", "Display information on the logging system", MENUITEM3},
	{" ", " ", -1}
  };
/* Menu items for lock menu */
  static struct _lkmenu_choices {
    char option[15];
    char description[52];
    int num;
  } lkmenu_choices[]= {
	{"Summary", "Display a Locking System summary", MENUITEM1},
	{"Locks", "Display locklists and related information", MENUITEM2},
	{"Resources", "Display resource lists and related information", MENUITEM3},
	{"Refresh", "Get new snapshot of locking system information", MENUITEM4},
	{" ", " ", -1}
  };
/* Menu items for log menu */
  static struct _lgmenu_choices {
    char option[15];
    char description[52];
    int num;
  } lgmenu_choices[]= {
	{"Summary", "Display a Logging System summary", MENUITEM1},
	{"Header", "Display the log file header", MENUITEM2},
	{"Processes", "Display the active processes in the logging system", MENUITEM3},
	{"Databases", "Display logging information on each open database", MENUITEM4},
	{"Transactions", "Display logging information on each transaction", MENUITEM5},
	{"Refresh", "Get new snapshot of logging system information", MENUITEM6},
	{" ", " ", -1}
  };
/*
**
**  Name: main()
**
**  Purpose - this is the top routine for the new lockinfo for R6.  this
**	version does more.  In addition to displaying lock information,
**	it displays a list of servers and allows viewing and manipulation
**      of the servers/sessions in each server.
**
** Inputs:
**	argv, argc - command line arguments
**
** Outputs
**	none
**
** Returns
**	none
**
** History
**	1/6/89		tomt	created
**	8/27/89		tomt	added logging forms and got rid of 'f_' in form
**				names replaced with 'f'.
**	9/19/89		tomt	moved addforms to files where forms are displayed
**	2 May 1991	jpk	moved MEadvise and SIeqinit to main,
**				process_args before sys_init(), both to
**				support client mode (flag_client)
**	14-jun-95 (tutto01)
**	    Redefined main to ii_user_main for NT.
**	24-sep-95 (tutto01)
**	    Restored main, run as a console app on NT.
**	11/1/95		nick	remove file output code
*/
main(argc, argv)
i4  argc;
char **argv;
{
#if defined(conf_ADD_ON64) && !defined(LP64)
    char	*lp64enabled;
#endif  /* conf_ADD_ON64 */
    /* Initialize the INGRES memory allocator and lk/lg system. */
    MEadvise(ME_INGRES_ALLOC);
#if defined(conf_ADD_ON64) && !defined(LP64)
    /*
    ** Try to exec the 64-bit version
    */
    NMgtAt("II_LP64_ENABLED", &lp64enabled);
    if ( (lp64enabled && *lp64enabled) &&
       ( !(STbcompare(lp64enabled, 0, "ON", 0, TRUE)) ||
	 !(STbcompare(lp64enabled, 0, "TRUE", 0, TRUE))))
    {
	PCexeclp64(argc, argv);
    }
#endif  /* conf_ADD_ON64 */
    /* Establish NUMA context */
    if ( OK != CXget_context(&argc, argv, CX_NSC_STD_NUMA, NULL, 0) )
	PCexit(FAIL);
    /* Set up i/o streams */
    SIeqinit();
    process_args(argc, argv);
    sys_init();
    enable_frs();	/* turn on INGRES FRS */
/* # line 302 "ipmmain.qsc" */	/* addform */
  {
    IIaddform(fingdba);
  }
/* # line 303 "ipmmain.qsc" */	/* addform */
  {
    IIaddform(flockmenu);
  }
/* # line 304 "ipmmain.qsc" */	/* addform */
  {
    IIaddform(flogmenu);
  }
/* # line 306 "ipmmain.qsc" */	/* host code */
    FEcopyright("IPM", "1990");
    /*
    ** Read in list of dbs, tids from iidbdb if user did not specify
    ** a database.  If a db was specified on command line, these were
    ** read in process_args()
    */
    if (!flag_db && !flag_standalone)	/* can't do it if server down */
	read_iidbdb();
    /* Display the main form.  All control is from there */
    dbamain();
    /* clean up and exit */
    terminate(OK);
}
/*
** Routine: dbamain
**
** This routine displays the main form
**
** History
**	1/6/89		tomt	created
**	6/23/89		tomt	fixed "Go" submenu
**	6/23/89		tomt	populate the scrolling window with
**				lock submenu choices when the proper
**				submenu is chosen
**	8/18/89		tomt	populate the scrolling window with
**				logging submenu choices when proper
**				submenu is chosen
**	9/8/89		tomt	broke submenus into seperate procedures
**	18 Sept 1990	jpk	removed duplicate menu items for
**				choices defined in tablefield, no Help
**				or Quit in tablefield
*/
VOID
dbamain()
{
  int i, row_num;
  int menu_number, tab_size;
  char dbname[FE_MAXNAME + 1];
/* # line 349 "ipmmain.qsc" */	/* display */
  {
    if (IIdispfrm((char *)"fingdba",(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 353 "ipmmain.qsc" */	/* host code */
	/* set up selection tablefield */
/* # line 354 "ipmmain.qsc" */	/* inittable */
      {
        if (IItbinit((char *)"fingdba",(char *)"options",(char *)"r") != 0) {
          IIthidecol((char *)"num",(char *)"i2");
          IItfill();
        } /* IItbinit */
      }
/* # line 355 "ipmmain.qsc" */	/* host code */
	for (i = 0; menu_choices[i].num > 0; i++) 
	{
/* # line 357 "ipmmain.qsc" */	/* loadtable */
      {
        if (IItbact((char *)"fingdba",(char *)"options",1) != 0) {
          IItcoputio((char *)"option",(short *)0,1,32,0,menu_choices[i].option);
          IItcoputio((char *)"description",(short *)0,1,32,0,
          menu_choices[i].description);
          IItcoputio((char *)"num",(short *)0,1,30,4,&menu_choices[i].num);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 361 "ipmmain.qsc" */	/* host code */
	}
	/* Determine tablefield size */
/* # line 363 "ipmmain.qsc" */	/* inquire_frs */
      {
        if (IIiqset(3,0,(char *)"fingdba",(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&tab_size,31,(char *)"options",0);
        } /* IIiqset */
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 368 "ipmmain.qsc" */	/* getrow */
          {
            if (IItbsetio(2,(char *)"fingdba",(char *)"options",-2) != 0) {
              IItcogetio((short *)0,1,30,4,&menu_number,(char *)"num");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 369 "ipmmain.qsc" */	/* host code */
	switch (menu_number) 
	{
	    case MENUITEM1:		/* server */
		if (flag_standalone)
		{
		    POP_MSG("server information is not available in\
 standalone mode (-s option)");
		    break;
		}
		close_db();		/* taken from menu item code */
		if (dispsrv() != OK)
		{
		    POP_MSG("Error getting server information");
                }
	        break;
	    case MENUITEM2:		/* lock */
		lock_submenu();
		break;
	    case MENUITEM3:		/* log */
		log_submenu();
		break;
	    default:
	        break;
	}
        }
      } else if (IIretval() == 2) {
        {
/* # line 399 "ipmmain.qsc" */	/* host code */
	set_options();
        }
      } else if (IIretval() == 3) {
        {
/* # line 403 "ipmmain.qsc" */	/* host code */
	my_help("ipmmain.hlp", "Top Menu");
        }
      } else if (IIretval() == 4) {
        {
/* # line 407 "ipmmain.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 5) {
        {
        }
      } else if (IIretval() == 6) {
        {
        }
      } else {
        if (1) goto IIfdE1;
      } /* IIretval */
    } /* IIrunform */
IIfdF1:
    if (IIchkfrm() == 0) goto IIfdB1;
    goto IIfdE1;
IImuI1:
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact((char *)"Select",(char *)0,2,2,1) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact((char *)"Options",(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact((char *)"Help",(char *)0,2,2,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact((char *)"Quit",(char *)0,2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(2,(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IIactscrl((char *)"options",0,5) == 0) goto IIfdE1;
    if (IIactscrl((char *)"options",1,6) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 415 "ipmmain.qsc" */	/* host code */
    return;
}
/*
**  Process_args
**
**  Parses the command line and validates it.  Reports errors accordingly.
**  If the command line is valid, appropriate internal flags are set.
**  Invalid input prints an error message and prints help.
**
**  Change History
**  
**   1/25/89     tomt     Converted to R6 version of lockinfo
**   10/1/89	 tomt	  put in message for file output that it is not done
**   3/13/90     tomt     added filter for surpressing inactive tx display
*/
VOID
process_args(argc, argv)
i4  argc;
char **argv;
{
    for (argc--, argv++; argc > 0; argc--, argv++) 
    {
	if (*argv[0] != '-')
	{
	    syntax_error(argv[0], "illegal parameter (missed flag?)");
	}
	(*argv)++;
	switch (*argv[0])
	{	
	    /*	CASE d:
	    ** 	   -ddbname	process only locks on a db
	    **     otherwise    process all databases
	    */	
	    case 'd' :
	    {
		if (flag_standalone)
		{
		    /* can't lookup db's if standalone */
		    syntax_error("-d", "-s flag incompatible with -d flag");
		}
		if (flag_db)
		{
		    syntax_error("-d", "-d flag specified twice");
		}
		(*argv)++;
	      	if (*argv[0] == NULL)
		{
		    syntax_error("-d", "no database name specified for -d");
		}
		else
		{
		    STcopy(*argv, ptr_dbname);
		}
		flag_db = TRUE;
	        break;
	    }
	    /*	CASE r:
	    **     -r		specified frs timeout value
	    **     otherwise    if used interactivly, screens
	    **			w/autorefresh will not.
	    */	
	    case 'r' :
	    {
		frs_timeout = 0;
		(*argv)++;
	      	if (*argv[0] == NULL)
		{
		    syntax_error("-r", "no refresh time specified");
		}
		else
		{
		    if (CVan(*argv, &frs_timeout) != OK /* keep in gbl var */
			|| frs_timeout < 0 || frs_timeout > 999)
		    {
		        syntax_error("-r", "the refresh value \
is an integer between 1 and 999 (seconds)");
		    }
		}
		break;
	    }
	    /*	CASE l:
	    **     -l		process ALL lock types
	    ** 	   -llocktyupe	process ONLY lock types
	    **     otherwise    process II, rl, rd, pg locks only
	    */	
	    case 'l' :
	    {
		if (flag_locktype)
		{
		    syntax_error("-l", "the -l flag was specfied twice");
		}
		if (flag_all_locks)
		{
		    syntax_error("-l", "the -l flag was specfied twice");
		}
		if (flag_table) 
		{
		    syntax_error("-t", "can't specify both a table and a\
 resource type.");
		}
		if (!flag_nl)
		    flag_nl = TRUE;	/* when looking for a specific lktype,
				** we will print null granted locks also
				*/
		/* XXX doesn't this mean flag_nl = TRUE regardless? */
		(*argv)++;
	      	if (*argv[0] == NULL)
		{
		    flag_all_locks = TRUE;
		}
		else
		{
		    if ((ing_locktype = trns_lnm_to_lktyp(*argv)) == 0) 
		    {
		        syntax_error("-l", "nonexistent resource type");
		    }
 	            flag_locktype = TRUE;
		}
	        break;
	    }
	    /*	CASE n:
	    **     -n		print out all locks including those
	    **			which are granted null
	    **     otherwise    print out locks which are not granted
	    **			NULL
	    */	
	    case 'n' :
	    {
		if (flag_locktype || flag_all_locks)
		    break;	/* those options already set flag_nl */
		if (flag_nl) 	/* this can be increm by -l flg */
		{
		    syntax_error("-n", "-n flag specified twice");
		}
	        flag_nl = TRUE;
	        break;
	    }
	    /*	CASE t:
	    **     -t		process locks for a specific table;
	    **			also must specify a particular db.
	    **			This flag and -l are mutually exclusive.
	    **     otherwise    process all tables.
	    */	
	    case 't' :
	    {
		if (flag_table)
		{
		    syntax_error("-t", "-t flag specified twice");
		}
		if (flag_locktype || flag_all_locks)
		{
		    syntax_error("-l", "can't specify both a table and a\
 resource type.");
		}
		if (!flag_nl)
		    flag_nl = TRUE;	/* when looking for a specific table,
				** we will print null granted locks also
				*/
		/* XXX doesn't this mean flag_nl = TRUE regardless? */
		(*argv)++;
	      	if (*argv[0] == NULL)
		{
		    syntax_error("-t", "no table specified for -t");
		}
		else
		{
		    STcopy(*argv, ptr_tablename);
		}
	        flag_table = TRUE;
        	break;
	    }
	    /*	CASE e:
	    **     -e		display system lock lists 
	    **     otherwise    display all lock lists (default)
	    */	
	    case 'e' :
	    {
		if (flag_nonprot_lklists)
		{
		    syntax_error("-e", "-e flag specified twice");
		}
	        flag_nonprot_lklists = TRUE;
	        break;
	    }
	    /*	CASE I:
	    **     -I		display internal sessions
	    **     otherwise    don't display internal sessions (default)
	    */	
	    case 'I' :
	    {
	        flag_internal_sess = TRUE;
	        break;
	    }
	    /*	CASE i:
	    **     -i		display inactive transactions also
	    **     otherwise    don't display inactive xactions (default)
	    */	
	    case 'i' :
	    {
		if (flag_inactive_tx)
		{
		    syntax_error("-i", "-i flag specified twice");
		}
	        flag_inactive_tx = TRUE;
	        break;
	    }
	    /*	CASE S:
	    **     -s		standalone mode -- don't try to open databases
	    **     otherwise    open databases
	    */	
	    case 's' :
	    {
		if (flag_db)
		{
		    syntax_error("-s", "-s flag incompatible with -d flag");
		}
	        flag_standalone = TRUE;
	        break;
	    }
	    /*	CASE L:
	    **     -L		enable IPM debugging to file IPM.LOG
	    **     otherwise    IPM debugging log is disabled
	    */	
	    case 'L' :
	    {
		if (flag_debug_log)
		{
		    ;	/* undocumented for now, so do NOT tell user
		    	** that she specified it twice, again for now
		    	*/
		}
	        flag_debug_log = TRUE;
	        break;
	    }
	    /*	CASE c:
	    **     -c		client mode, do ONLY server and sessions
	    **     otherwise    do ALL functionality, incl. log and lock
	    */	
	    case 'c' :
	    {
	        flag_client = TRUE;
	        break;
	    }
	    /*	CASE h:
	    **     -h	display flags and their usage	
	    */	
	    case 'h' :
	    {
	        display_help();    
			terminate(OK);
	        break;
	    }
	    default:
	    {
		syntax_error(argv[0], "unknown flag");
	    }
	}      /* end of switch on char following - */  
    } /* end of for loop */
    /*
    ** If user specified a database, we load db names here instead of in the
    ** CASE stomt.  We get database names here after we know
    ** ALL of the command line args and ruled out bad combinations
    */
    if (flag_db && !flag_standalone)	/* can't do it if server down */
    {
	read_iidbdb();			/* Read in all the tids */
	/* Search for the tid of ptr_dbname */
	if ((dbname_id = get_db_id(ptr_dbname)) == -1)
	{
	    terminate(FAIL, "Database '%s' not found.  Exiting...",
		ptr_dbname);
	}
    }
    /*
    ** ensure that if the user specified a lock type other than
    ** database level, that they also specify a database.  
    */
    {
	i4 lock_cat;
	lock_cat = getlockcat(ing_locktype);	/* lock catagory */
        if (!flag_db && flag_locktype && (lock_cat != DATABASE_LOCK) &&
	    (lock_cat != OTHER_LOCK))
	{
	    syntax_error("-l", "no database specified for -l");
        }
    }
    /*
    ** if user specifies a particular table, they
    ** must also specify a database also.
    */
    if (flag_table && !flag_db)
    {
	syntax_error("-t", "no database specified for -t");
    }
} /* end of process_args */
/*
** sys_init() - perform any system initializations
**
** History
**	1/6/89 		tomt	written
**	6/9/89		tomt    added call to CSinitiate.
**	6/15/89		tomt	added call to determine language code for INGRES
**	8/16/89		tomt 	added call to LGinitialize
**	9/20/89		tomt	made errmsgs more meaningful
**	5/15/07		toumi01	add process info to shared memory
*/
VOID
sys_init()
{
    STATUS		status;
    CL_SYS_ERR		sys_err;
    /*
    **
    ** Bug #4405
    ** lockstat/logstat acts as a "psuedo-server" in that it must connect to
    ** the "event system" when it uses LK.  The CSinitiate call is necessary
    ** to initialize this, and to set up a cleanup routine which will free
    ** a slot in the system shared memory segment when the program exits.
    */
    if (!flag_client)
    {
	if ((status = CSinitiate(0, 0, 0)) != OK)
	{
	    terminate(FAIL, "IPM: Attempt to connect to server failed.\n\
Check permissions or installation.  Either the correct privileges are not\n\
set or the INGRES system is not running");
	}
	if ((status = LKinitialize(&sys_err, ERx("ipm"))) != OK)
	{			/* could not initialize LK system */
	    terminate(FAIL, "IPM: Attempt to initialize the locking system failed.\n\
Check permissions or installation.  Either the correct privileges are not\n\
set or the INGRES system is not running.");
	}
	if ((status = LGinitialize(&sys_err, ERx("ipm"))) != OK)
	{			/* could not initialize LG system */
	    terminate(FAIL, "IPM: Attempt to initialize the logging system failed.\n\
Check permissions or installation.  Either the correct privileges are not\n\
set or the INGRES system is not running.");
	}
    }
    /* Determine if we are operating on a cluster and set a global if we are */
    if (CXcluster_enabled() != 0)
	flag_cluster = TRUE;
    else
	flag_cluster = FALSE;
    /* Set language code for INGRES error messages */
    setlangcode();
    /* Set up INGRES error handling */
    decl_errhndl();
}
/*
** lock_submenu() this contains the locking submenu for the main screen
**
** History
**	9/8/89		tomt	took out of main display loop
**	18 Sept 1990	jpk	modified top frame to conform to forms
**				standards: no duplicate menu items for
**				things defined in tablefield, no Help
**				or Quit in tablefield, "Select" instead of "Go"
*/
VOID
lock_submenu()
{
  int j, rownum;
  int menunumber, table_size;
  char db_name[FE_MAXNAME + 1];
/* # line 817 "ipmmain.qsc" */	/* display */
  {
    if (IIdispfrm((char *)"flockmenu",(char *)"r") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    {
/* # line 820 "ipmmain.qsc" */	/* host code */
	get_lklgstats(GET_LOCKLIST, (PTR) 0);
	get_lklgstats(GET_LOCKRES, (PTR) 0);
	/* fill the database field with the proper strings XXX again? */
	/* set up selection tablefield */
/* # line 826 "ipmmain.qsc" */	/* inittable */
      {
        if (IItbinit((char *)"flockmenu",(char *)"options",(char *)"r") != 0) 
        {
          IIthidecol((char *)"num",(char *)"i2");
          IItfill();
        } /* IItbinit */
      }
/* # line 827 "ipmmain.qsc" */	/* host code */
        for (j = 0; lkmenu_choices[j].num > 0; j++) 
	{
/* # line 829 "ipmmain.qsc" */	/* loadtable */
      {
        if (IItbact((char *)"flockmenu",(char *)"options",1) != 0) {
          IItcoputio((char *)"option",(short *)0,1,32,0,
          lkmenu_choices[j].option);
          IItcoputio((char *)"description",(short *)0,1,32,0,
          lkmenu_choices[j].description);
          IItcoputio((char *)"num",(short *)0,1,30,4,&lkmenu_choices[j].num);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 833 "ipmmain.qsc" */	/* host code */
	}
	/* Determine tablefield size */
/* # line 836 "ipmmain.qsc" */	/* inquire_frs */
      {
        if (IIiqset(3,0,(char *)"flockmenu",(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&table_size,31,(char *)"options",0);
        } /* IIiqset */
      }
    }
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 841 "ipmmain.qsc" */	/* getrow */
          {
            if (IItbsetio(2,(char *)"flockmenu",(char *)"options",-2) != 0) {
              IItcogetio((short *)0,1,30,4,&menunumber,(char *)"num");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 842 "ipmmain.qsc" */	/* host code */
	switch (menunumber) 
	{
	    case MENUITEM1:			/* Summary */
		lklginfo(GET_LOCKSTAT);
	        break;
	    case MENUITEM2:			/* Locklists */
/* # line 849 "ipmmain.qsc" */	/* message */
          {
            IImessage((char *)"Loading lock information...");
          }
/* # line 850 "ipmmain.qsc" */	/* host code */
		lklginfo(GET_LOCKLIST);
	        break;
	    case MENUITEM3:			/* Resources */
/* # line 854 "ipmmain.qsc" */	/* message */
          {
            IImessage((char *)"Loading resource information...");
          }
/* # line 855 "ipmmain.qsc" */	/* host code */
		lklginfo(GET_LOCKRES);
	        break;
	    case MENUITEM4:			/* Refresh */
/* # line 859 "ipmmain.qsc" */	/* message */
          {
            IImessage((char *)"Refreshing lock information...");
          }
/* # line 860 "ipmmain.qsc" */	/* host code */
        	get_lklgstats(GET_LOCKLIST, (PTR) 0);
		get_lklgstats(GET_LOCKRES, (PTR) 0);
	        break;
	    default:
		break;
	}
        }
      } else if (IIretval() == 2) {
        {
/* # line 870 "ipmmain.qsc" */	/* host code */
	set_options();
        }
      } else if (IIretval() == 3) {
        {
/* # line 874 "ipmmain.qsc" */	/* host code */
	/*  Restore the main menu choices to the selection tablefield */
/* # line 875 "ipmmain.qsc" */	/* inittable */
          {
            if (IItbinit((char *)"flockmenu",(char *)"options",(char *)"\
r") != 0) {
              IIthidecol((char *)"num",(char *)"i2");
              IItfill();
            } /* IItbinit */
          }
/* # line 876 "ipmmain.qsc" */	/* host code */
        for (j = 0; menu_choices[j].num > 0; j++) 
	{
/* # line 878 "ipmmain.qsc" */	/* loadtable */
          {
            if (IItbact((char *)"flockmenu",(char *)"options",1) != 0) {
              IItcoputio((char *)"option",(short *)0,1,32,0,
              menu_choices[j].option);
              IItcoputio((char *)"description",(short *)0,1,32,0,
              menu_choices[j].description);
              IItcoputio((char *)"num",(short *)0,1,30,4,&menu_choices[j].num);
              IITBceColEnd();
            } /* IItbact */
          }
/* # line 882 "ipmmain.qsc" */	/* host code */
	}
/* # line 883 "ipmmain.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 4) {
        {
        }
      } else if (IIretval() == 5) {
        {
        }
      } else {
        if (1) goto IIfdE2;
      } /* IIretval */
    } /* IIrunform */
IIfdF2:
    if (IIchkfrm() == 0) goto IIfdB2;
    goto IIfdE2;
IImuI2:
    if (IIinitmu() == 0) goto IIfdE2;
    if (IInmuact((char *)"Select",(char *)0,2,2,1) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE2;
    if (IInmuact((char *)"Options",(char *)0,2,2,2) == 0) goto IIfdE2;
    if (IInmuact((char *)"End",(char *)0,2,2,3) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE2;
    if (IIactscrl((char *)"options",0,4) == 0) goto IIfdE2;
    if (IIactscrl((char *)"options",1,5) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 891 "ipmmain.qsc" */	/* host code */
    return;
}
/*
** log_submenu() this contains the logging submenu for the main screen
**
** History
**	9/8/89		tomt	took out of main display loop
**	18 Sept 1990	jpk	modified top frame to conform to forms
**				standards: no duplicate menu items for
**				things defined in tablefield, no Help
**				or Quit in tablefield, "Select" instead of "Go"
*/
VOID
log_submenu()
{
  int k, rowno;
  int menu_num, tabsize;
  char dbnam[FE_MAXNAME + 1];
/* # line 911 "ipmmain.qsc" */	/* display */
  {
    if (IIdispfrm((char *)"flogmenu",(char *)"r") == 0) goto IIfdE3;
    goto IImuI3;
IIfdI3:;
    if (IIfsetio((char *)0) == 0) goto IIfdE3;
    {
/* # line 914 "ipmmain.qsc" */	/* host code */
        get_lklgstats(GET_LOGPROCESS, (PTR) 0);
	get_lklgstats(GET_LOGXACTLIST, (PTR) 0);
	get_lklgstats(GET_LOGDBLIST, (PTR) 0);
	/* fill the database field with the proper strings XXX again? */
	/* set up selection tablefield	*/
/* # line 921 "ipmmain.qsc" */	/* inittable */
      {
        if (IItbinit((char *)"flogmenu",(char *)"options",(char *)"r") != 0) 
        {
          IIthidecol((char *)"num",(char *)"i2");
          IItfill();
        } /* IItbinit */
      }
/* # line 922 "ipmmain.qsc" */	/* host code */
        for (k = 0; lgmenu_choices[k].num > 0; k++) 
	{
/* # line 924 "ipmmain.qsc" */	/* loadtable */
      {
        if (IItbact((char *)"flogmenu",(char *)"options",1) != 0) {
          IItcoputio((char *)"option",(short *)0,1,32,0,
          lgmenu_choices[k].option);
          IItcoputio((char *)"description",(short *)0,1,32,0,
          lgmenu_choices[k].description);
          IItcoputio((char *)"num",(short *)0,1,30,4,&lgmenu_choices[k].num);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 928 "ipmmain.qsc" */	/* host code */
	}
	/* Determine tablefield size */
/* # line 930 "ipmmain.qsc" */	/* inquire_frs */
      {
        if (IIiqset(3,0,(char *)"flogmenu",(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&tabsize,31,(char *)"options",0);
        } /* IIiqset */
      }
    }
IIfdB3:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 935 "ipmmain.qsc" */	/* getrow */
          {
            if (IItbsetio(2,(char *)"flogmenu",(char *)"options",-2) != 0) {
              IItcogetio((short *)0,1,30,4,&menu_num,(char *)"num");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 936 "ipmmain.qsc" */	/* host code */
	switch (menu_num) 
	{
	    case MENUITEM1:		/* Summary */
/* # line 939 "ipmmain.qsc" */	/* message */
          {
            IImessage((char *)"Loading summary logging statistics ...");
          }
/* # line 940 "ipmmain.qsc" */	/* host code */
		lklginfo(GET_LOGSTAT);
	        break;
	    case MENUITEM2:		/* Header */
/* # line 944 "ipmmain.qsc" */	/* message */
          {
            IImessage((char *)"Loading logging header information ...");
          }
/* # line 945 "ipmmain.qsc" */	/* host code */
		lklginfo(GET_LOGHEADER);
	        break;
	    case MENUITEM3:		/* Processes */
/* # line 949 "ipmmain.qsc" */	/* message */
          {
            IImessage((char *)"Loading process information ...");
          }
/* # line 950 "ipmmain.qsc" */	/* host code */
		lklginfo(GET_LOGPROCESS);
	        break;
	    case MENUITEM4:		/* Databases */
/* # line 954 "ipmmain.qsc" */	/* message */
          {
            IImessage((char *)"Loading database information ...");
          }
/* # line 955 "ipmmain.qsc" */	/* host code */
		lklginfo(GET_LOGDBLIST);
	        break;
	    case MENUITEM5:		/* Transactions */
/* # line 959 "ipmmain.qsc" */	/* message */
          {
            IImessage((char *)"Loading transaction information ...");
          }
/* # line 960 "ipmmain.qsc" */	/* host code */
		lklginfo(GET_LOGXACTLIST);
	        break;
	    case MENUITEM6:		/* Refresh */
/* # line 964 "ipmmain.qsc" */	/* message */
          {
            IImessage((char *)"Refreshing logging information...");
          }
/* # line 965 "ipmmain.qsc" */	/* host code */
        	get_lklgstats(GET_LOGXACTLIST, (PTR) 0);
        	get_lklgstats(GET_LOGPROCESS, (PTR) 0);
        	get_lklgstats(GET_LOGDBLIST, (PTR) 0);
	        break;
	    default:
	break;
	}
        }
      } else if (IIretval() == 2) {
        {
/* # line 977 "ipmmain.qsc" */	/* host code */
	set_options();
        }
      } else if (IIretval() == 3) {
        {
/* # line 982 "ipmmain.qsc" */	/* host code */
	/* Restore the main menu choices to the selection tablefield */
/* # line 983 "ipmmain.qsc" */	/* inittable */
          {
            if (IItbinit((char *)"flogmenu",(char *)"options",(char *)"\
r") != 0) {
              IIthidecol((char *)"num",(char *)"i2");
              IItfill();
            } /* IItbinit */
          }
/* # line 984 "ipmmain.qsc" */	/* host code */
        for (k = 0; menu_choices[k].num > 0; k++) 
	{
/* # line 986 "ipmmain.qsc" */	/* loadtable */
          {
            if (IItbact((char *)"flogmenu",(char *)"options",1) != 0) {
              IItcoputio((char *)"option",(short *)0,1,32,0,
              menu_choices[k].option);
              IItcoputio((char *)"description",(short *)0,1,32,0,
              menu_choices[k].description);
              IItcoputio((char *)"num",(short *)0,1,30,4,&menu_choices[k].num);
              IITBceColEnd();
            } /* IItbact */
          }
/* # line 990 "ipmmain.qsc" */	/* host code */
	}
/* # line 991 "ipmmain.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE3;
          }
        }
      } else if (IIretval() == 4) {
        {
        }
      } else if (IIretval() == 5) {
        {
        }
      } else {
        if (1) goto IIfdE3;
      } /* IIretval */
    } /* IIrunform */
IIfdF3:
    if (IIchkfrm() == 0) goto IIfdB3;
    goto IIfdE3;
IImuI3:
    if (IIinitmu() == 0) goto IIfdE3;
    if (IInmuact((char *)"Select",(char *)0,2,2,1) == 0) goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE3;
    if (IInmuact((char *)"Options",(char *)0,2,2,2) == 0) goto IIfdE3;
    if (IInmuact((char *)"End",(char *)0,2,2,3) == 0) goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE3;
    if (IIactscrl((char *)"options",0,4) == 0) goto IIfdE3;
    if (IIactscrl((char *)"options",1,5) == 0) goto IIfdE3;
    if (IIendmu() == 0) goto IIfdE3;
    goto IIfdI3;
IIfdE3:;
    IIendfrm();
  }
/* # line 999 "ipmmain.qsc" */	/* host code */
    return;
}
/* VARARGS31 */
DB_STATUS
ule_doformat(error_code, clerror, flag, generic_error, msg_buffer,
	msg_buf_length, msg_length, err_code, num_parms,
    psize1,  pval1, psize2,  pval2, psize3,  pval3, psize4,  pval4,
    psize5,  pval5, psize6,  pval6, psize7,  pval7, psize8,  pval8,
    psize9,  pval9, psize10, pval10, psize11, pval11, psize12, pval12)
i4		    error_code;
CL_ERR_DESC	    *clerror;
i4		    flag;
i4		    *generic_error;
char		    *msg_buffer;
i4		    msg_buf_length;
i4		    *msg_length;
i4		    *err_code;
i4		    num_parms;
i4		    psize1;
PTR		    pval1;
i4		    psize2;
PTR		    pval2;
i4		    psize3;
PTR		    pval3;
i4		    psize4;
PTR		    pval4;
i4		    psize5;
PTR		    pval5;
i4		    psize6;
PTR		    pval6;
i4		    psize7;
PTR		    pval7;
i4		    psize8;
PTR		    pval8;
i4		    psize9;
PTR		    pval9;
i4		    psize10;
PTR		    pval10;
i4		    psize11;
PTR		    pval11;
i4		    psize12;
PTR		    pval12;
{
    SIprintf("ule_format called with error code %d\n", error_code);
    return OK;
}
