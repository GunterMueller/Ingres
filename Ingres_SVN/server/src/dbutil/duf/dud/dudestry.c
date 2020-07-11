# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**Copyright (c) 2004 Ingres Corporation
*/
#include    <compat.h>
#include    <pc.h>
#include    <gl.h>
#include    <sl.h>
#include    <gv.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <duf.h>
#include    <cs.h>
#include    <lk.h>
#include    <er.h>
#include    <duerr.h>
# include <dudbms.h>
# include <duenv.h>
# include <duddb.h>
#include    <duustrings.h>
#include    <ex.h>
#include    <lo.h>
#include    <me.h>
#include    <si.h>
#include    <st.h>
#include    <pm.h>
#include    <cui.h>
# include <dustar.h>
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/**
**
**  Name: DUDESTRY.QSC  -   contains all of the equel procedures specific to
**			    the Ingres system utility, destroydb.
**
**  Description:
**        This file contains all of the equel procedures specific to the
**	Ingres system utility, destroydb.
**
**          dubmain()		- the main entry point to this program.
**	    dud_env_init()	- get the info from the dbdb that is needed
**				  to destroy the database.
**	    dud_exit()		- destroydb exit handler.
**	    dud_ex_handler()	- destroydb exception handler.
**	    dud_finish_dbdb()	- remove dbdb entries that correspond to the
**				  database being destroyed.
**	    dud_ingerr_handler()- Ingres error handler for destroydb. 
**	    dud_init_star()	- Initialize variables used in TITAN change.
**	    dud_star_setup()	- Take care of entries in iistar_cdb, and iidbdb if
**				    it is a distributed database destruction.
**
**
**  History:
**      26-aug-86 (ericj)
**          Initial creation.
**      17-Apr-87 (ericj)
**	    Bug fixing and updates for MVS group.
**      7-dec-88 (alexc)
**	    TITAN modifications to destroydb(Merged up to FCS class)
**	    Dec 7, 88.
**      15-mar-89 (greg)
**          Changed "commented out" to IFDEF'ed out.  If this code is
**          obsolete it must be deleted.
**      27-mar-1989 (alexc)
**          Merge in Inconsistant database fixed, and Destroying CDB
**	    by name change.
**	4-may-89 (teg)
**	    Merge titan/terminator code, changed greg's ifndef to ifdef.  This
**	    section of code should be deleted after TERMINATOR FCS if it turns
**	    out that it is not required.
**      27-mar-1989 (alexc)
**          Create function dud_init_star().
**	12-may-89 (ralph)
**	    GRANT Enhancements, Phase 2:
**	    Delete all tuples in iidbpriv associated with destroyed database
**      22-May-89 (teg)
**          move local hdrs to lib, remove log-for RCS
**	30-May-89 (teg)
**	    remove DI from destroydb
**	24-jul-90 (teresa)
**	    changed .qh include files to .qsh to solve unix build problems.
**	19-nov-90 (teresa)
**	    remove set_uid ming hint as DI has been moved into the DBMS server.
**      20-nov-92 (bonobo)
**          Updated MING hint to include CUFLIB.
**	21-dec-92 (robf)
**	    Old -s option is a no-op. Issue a warning if its used
**	    per LRC decision.
**	25-may-1993 (bryanp)
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**	8-aug-93 (ed)
**	    unnest dbms.h
**	1-oct-93 (robf)
**          Make privileges active before destroying database
**      31-jan-94 (mikem)
**          Added include of <cs.h> before <lk.h>.  <lk.h> now references
**          a CS_SID type.
**      15-feb-1994 (andyw)
**          After trying to destroy a database which is not owned leaves the
**          database as inconsistent. Swapped the values for access_set/
**          access_clear
**	25-Apr-1994 (fredv)
**	    Removed DMFLIB from the NEEDLIBS list. It isn't needed anymore.
**	10-Nov-1994 (ramra01)
**	    Destroydb with nowait flag
**      12-Dec-1994 (liibi01)
**          Cross integration from 6.4(Bug 53269). The exception handler for
**          DESTROYDB only released the lock on the database undergoing
**          deletion upon normal termination or the receipt of SIGINT. Alter
**          the handler to perform the lock release on the receipt of SIGHUP,
**          SIGQUIT and SIGTERM also.
**      21-mar-1995 (sarjo01)
**          Bug 67417: add test for upper case 'IISTAR_CDBS' as well as
**          'iistar_cdbs' to allow query to work correctly in FIPS.
**      16-aug-1995 (harpa06)
**          Bug #70604 - Flag an error if a database to destroy has already
**          been specified and the user is trying to destroy more than one
**          on the commnad line.
**	30-aug-96 (nick)
**	    Tidy -l flag stuff.
**	13-sep-96 (nick)
**	    Call EXsetclient() to ensure we catch user interrupts.
**      28-sep-96 (mcgem01)
**              Global data moved to duddata.c
**      21-apr-1999 (hanch04)
**          replace STindex with STchr
**      28-apr-1999 (hanch04)
**          Replaced STlcopy with STncpy
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      17-Apr-2001(hweho01)
**              Removed the prototype of du_error(), it has been declarated
**              in duerr.h.
**	6-july-2004 (shust01)
**	    Initialize dud_nowait_flag.  If a session had a lock on a database,
**	    destroydb was, at times, returning "E_US120A Database is already 
**	    open" instead of waiting for the lock to be released. bug 112079.
**      01-sep-2004 (sheco02)
**          X-integrate change 469768 to main.
**	29-Sep-2004 (drivi01)
**	    Updated NEEDLIBS to link dynamic library SHQLIB to replace
**	    its static libraries. Added NEEDLIBSW hint which is used
**	    for compiling on windows and compliments NEEDLIBS.
**      23-feb-2005 (stial01)
**          Explicitly set isolation level after connect
**	29-Mar-2005 (drivi01)
**	    Added a restriction on destroydb to check major build 
**	    releases for remote destroydb command to vnode and give
** 	    an error if the remote server has a different version.
**/
/*
NEEDLIBS =      DBUTILLIB SHQLIB COMPATLIB 
NEEDLIBSW = 	SHCUFLIB SHEMBEDLIB
OWNER =         INGUSER
PROGRAM =       destroydb
*/
/*
[@forward_type_references@]
*/
/*
**  Forward and/or External function references.
*/
VOID			dud_exit();	/* Destroydb exit handler */
STATUS 			get_pm_params(DU_ENV          *dud_dbenv,
				      DU_ERROR        *dud_errcb);
/*
[@function_reference@]...
*/
/*
[@#defines_of_other_constants@]
*/
/*
[@type_definitions@]
*/
GLOBALREF DU_ENV	      *Dud_dbenv;
GLOBALREF DUD_MODE_VECT	      *Dud_modes;
GLOBALREF DU_ERROR	      *Dud_errcb;
GLOBALREF i4		      Dud_deadlock;  /* Boolean to flag whether
					    ** deadlock has been detected.
					    */
GLOBALREF i4		      Dud_dlock_cnt; /* A count of the number of times
					    ** deadlock has been detected.
					    */
GLOBALREF i4		      Dud_1dlock_retry; /* Should DESTROYDB do
						 ** retry upon detecting
						 ** deadlock?
						 */
GLOBALREF i4		      Dud_star_status;/* Added for TITAN:
					    ** Distribute Destroydb status.
					    */
/*
**  This comment is here to trick the Equel/C preprocessor.  This was done
**  because Equel doesn't handle variables that are ZERO_FILLed.  However,
**  this is a Jupiter coding standard.  This comment makes the following
**  variable known to Equel.
**
  GLOBALDEF char Dud_appl_flag[DU_APPL_FLAG_MAX];
*/
/*
**  Now, the variable will actually be defined and allocated.
*/
GLOBALREF	char	     Dud_appl_flag[DU_APPL_FLAG_MAX];
/*
[@static_variable_or_function_definition@]...
*/
/*{
** Name: main()	-   the main entry point to the program, destroydb.
**
** Description:
**        This is the main entry point to the Ingres system utility,
**	destroydb.  Destroydb is used to destroy Ingres databases.
**	A user must be the DBA for the database or an Ingres super-user
**	to use this utility.  Destroydb cannot be used to destroy the
**	dbdb.
**
** Command line syntax:
**	destroydb [-uusername] [-p] dbname
**
**	The "-u" flag will allow an Ingres super-user to destroy
**	a database owned by "username".  
**
**      The "-p" flag will query a user whether they actually want to 
**	destroy the database in question.  
**
**
**	The "-l" flag will abort the operation if the database is busy.
**
**	"dbname" is the database to be destroyed.
**
**	Note: Any of the calls to dud_exit() may terminate the
**	execution of this program.
**
**	This program is designed to be idempotent.  That is, it
**	can be called with the same parameters after multiple
**	failures and the end result will be the same as if it were
**	called once successfully.  This should be true provided the
**	source of failure was not program error.
**
** Inputs:
**      argc                            The command line argument count.
**	argv				The command line argument vector.
**
** Side Effects:
**	      Destroys the database passed into it as an argument.
**
** History:
**      26-aug-86 (ericj)
**          Initial creation.
**	30-may-89 (teg)
**	    Remove DI and LK.
**	23-aug-1989 (Derek)
**	    Overhauled to use internal procedure to perform all the normal
**	    operations of destroydb.  All access and permission and user
**	    checks are performed by the server.
**	12-mar-92 (rog)
**	    Copy the database name into dud_dbenv.du_dbname here instead
**	    of in dud_init_star in case an error occurs and we need to
**	    report the database name before we call dud_init_star.
**	11-Feb-1993 (jpk)
**	    Support destroydb over the net.
**      15-feb-1994 (andyw)
**          After trying to destroy a database which is not owned leaves the
**          database as inconsistent. Swapped the values for access_set/
**          access_clear
**	10-Nov-1994 (ramra01)
**	    destroydb nowait flag process
**	30-aug-96 (nick)
**	    Tidy '-l' stuff.
**	13-sep-96 (nick)
**	    Call EXsetclient() else we don't catch user interrupts.
**	18-jun-1997 (nanpr01)
**	    Bug 83154 : Delimited identifiers are not getting recognized.
**	    The problem is really with use of quel which is interpreting
**	    the id differently(*, % has diff meaning in quel). So I have
**	    switched to sql.
**      25-Jun-2002 (hweho01)
**          Initialize the memory pointed by CM_AttrTab into the character
**          set before calling routine dud_get_args. (B108112)
**       7-Sep-2004 (hanal04) Bug 48179 INGSRV2543
**          Use new -Z flag to prevent ING_SET values that require user to
**          be the DBA from generating unexpected errors.
**	06-Jun-2005 (thaju02)
**	    Disable query of iiuser. -u already validated by backend 
**	    scs_dbdb_info/scs_icsxlate during connect. (B114667)
**          
[@history_template@]...
*/
VOID
main(argc, argv)
i4	argc;
char	*argv[];
{
  DU_ENV dud_dbenv;
  DUD_MODE_VECT dud_modes;
    DU_ERROR	    dud_errcb;
  i4 row_cnt;
  char *dba_override = "";
    char	    override[64];
    /* construct name of form node::database, so 2 for "::" */
  char node_dbname[GCC_L_NODE + DB_MAXNAME + 2 + 1];
  char dummy_arg[DB_MAXNAME + 1];
  char dbms_vers[32];
    i4		    idx1, idx2;
    char	    fe_vers[] = GV_VER;
  char dbname[DB_MAXNAME+1];
  i4 access_set;
  i4 access_clear;
  char dummy_name[DB_MAXNAME + 1];
  char dummy_xname[DB_MAXNAME + 1];
  i4 dummy_stat;
    DU_STATUS	    dud_finish_dbdb();
    DU_STATUS	    dud_init();
    DU_STATUS	    dud_get_args();
    DU_STATUS	    dui_delabfpath();
    STATUS	    dud_ex_handler();
    EX_CONTEXT	    context;
    i4		    dud_ingerr_handler();
    MEadvise(ME_INGRES_ALLOC);
    if (EXsetclient(EX_INGRES_TOOL) != OK)
    {
	dud_exit(&dud_errcb);
    }
    if (EXdeclare(dud_ex_handler, &context) != OK)
    {
	dud_exit(&dud_errcb);
    }
    /* Call IIUGinit to initialize CM attribute table */
    if( IIUGinit() != OK)
    {
        dud_exit(&dud_errcb);
    }
    if ((dud_init(&dud_dbenv, &dud_modes, &dud_errcb)) != E_DU_OK)
	dud_exit(&dud_errcb);
    /* get PM parameters */
    if (get_pm_params(&dud_dbenv, &dud_errcb) != OK)
    {
	dud_errcb.du_status = E_DU_IERROR;
	dud_exit(&dud_errcb);
    }
    /*	Process the command line to get all the necessary parameters.	*/
    if ((dud_get_args(argc, argv, &dud_dbenv, &dud_modes, &dud_errcb))
			!= E_DU_OK)
	dud_exit(&dud_errcb);
    /*
    **  We need to copy the database name here so that we can report
    **  its name if an error occurs.
    */
    STcopy(dud_modes.dud_dbname, dud_dbenv.du_dbname);
    /*
    **	If the "-p" flag has been passed in, query the user whether they really
    **	want to destroy this database.
    */
    if (dud_modes.dud_query_flag)
    {
	if (dud_confirm(dud_modes.dud_dbname) != TRUE)
	{
	    du_error(&dud_errcb, E_DU3102_NO_CONFIRM_DS, 2,
		     0, dud_modes.dud_dbname);
	    dud_exit(&dud_errcb);
	}
    }
    /*	Set up a Equel error handling procedure by calling IIseterr().	*/
    IIseterr(dud_ingerr_handler);
    /*	Construct -u flag if needed. */
    if (STcompare(dud_dbenv.du_usrinit, dud_dbenv.du_xusrname))
    {
	STpolycat(2, "-u", dud_dbenv.du_unorm_usr, override);
	dba_override = override;
    }
    /* Open the dbdb. */
    /* handle destroydb over the net */
    if (STlength(dud_dbenv.du_nodename) > 0)
    {
	STpolycat(3, dud_dbenv.du_nodename, ERx("::"), DU_DBDBNAME, 
		  node_dbname);
    }
    else
    {
	STcopy(DU_DBDBNAME, node_dbname);
    }
/* # line 432 "dudestry.qsc" */	/* ingres */
  {
    IIingopen(0,Dud_appl_flag,dba_override,node_dbname,(char *)"-Z",(char *)0);
  }
/* # line 434 "dudestry.qsc" */	/* host code */
    /* Set a flag that the dbdb has been opened. */
    dud_dbenv.du_dbopen	= TRUE;
/* # line 438 "dudestry.qsc" */	/* set */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session isolation level serializable");
    IIsyncup((char *)0,0);
  }
/* # line 440 "dudestry.qsc" */	/* host code */
    /*
    ** We need to check that the DBMS version is that same as our
    ** version (destroying a database over the net should have a similar
    ** behavior to createdb, which doesn't allow creation of databases
    ** remotely if installations have different versions).
    ** The version string starts with an optional string of
    ** characters followed by the major version and a forward slash 
    ** (/). Since we are only interested in the major release version
    ** we will find the slash and compare strings to that point.
    ** we assume createdb between minor releases will be compatable.
    */
    if ( (dud_dbenv.du_dbservice & DU_3SER_RMS) == 0 )
    {
/* # line 454 "dudestry.qsc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(dbms_vers=dbmsinfo(\"_version\"))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,31,dbms_vers);
      if (IIerrtest() != 0) goto IIrtB1;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 455 "dudestry.qsc" */	/* host code */
	idx1 = STchr(fe_vers, '/') - fe_vers;
	idx2 = STchr(dbms_vers, '/') - dbms_vers;
	if (idx1 != idx2 || STbcompare(fe_vers, idx1, dbms_vers, 
	    idx2, FALSE) != 0)
	{
	    /* not the same version */
	    du_error(&dud_errcb, E_DU351B_INCOMPATIBLE_VERSION, 4, 0, 
	        fe_vers, 0, dbms_vers);
	    SIprintf("%s\n", dud_errcb.du_errmsg);
/* # line 464 "dudestry.qsc" */	/* exit */
  {
    IIexit();
  }
/* # line 465 "dudestry.qsc" */	/* host code */
	    PCexit(FAIL);
	}
     }
    /* 
    ** check the priviledge if you are using -u flag. You better have
    ** security priviledge to do this
    */
    if (STcompare(dud_dbenv.du_xusrname, dud_dbenv.du_usrinit))
    {
	/* 
	** bug 86833 : before checking the -u flag, get case semantics
	** and convert appropriately
	*/
	/* Set the case translation flags in dbservice */
	if (dud_dbenv.du_real_user_case[0] == 'U')
	{
	    CVupper(dud_dbenv.du_usrinit);
	    CVupper(dud_dbenv.du_xusrname);
	}
	STcopy(dud_dbenv.du_usrinit, dummy_xname);
/* # line 461 "dudestry.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select status from iiuser where name=");
    IIputdomio((short *)0,1,32,0,dummy_xname);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dummy_stat);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 463 "dudestry.sc" */	/* host code */
	if (sqlca.sqlcode != 0)
	{
	    du_error(&dud_errcb, E_DU3001_USER_NOT_KNOWN, 2, 0, 
			    dud_dbenv.du_usrinit);
	    dud_exit(&dud_errcb);
	}
	dud_dbenv.du_usrstat = dummy_stat;
	if ((dud_dbenv.du_usrstat & DU_USECURITY) == 0)
	{
	    du_error(&dud_errcb, E_DU3004_NO_ALIAS_FLAG, 0);
	    dud_exit(&dud_errcb);
	}
#if 0
	STcopy(dud_dbenv.du_xusrname, dummy_xname);
/* # line 477 "dudestry.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select name from iiuser where name=");
    IIputdomio((short *)0,1,32,0,dummy_xname);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,dummy_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 479 "dudestry.sc" */	/* host code */
	if (sqlca.sqlcode != 0)
	{
	    du_error(&dud_errcb, E_DU3001_USER_NOT_KNOWN, 2, 0, 
			    dud_dbenv.du_xusrname);
	    dud_exit(&dud_errcb);
	}
#endif
    }
    /* Make all privileges active */
/* # line 488 "dudestry.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session with privileges=\
all");
    IIsyncup((char *)0,0);
  }
/* # line 489 "dudestry.sc" */	/* host code */
    if (dud_init_star(&dud_dbenv, &dud_modes, &dud_errcb, &Dud_star_status)
	    != E_DU_OK)
    {
	dud_exit(&dud_errcb);
    }
    /* Issue an informative message to the user that the database is being
    ** destroyed.  This also marks the point of no return.  At this point
    ** physical changes will be made to the database database and the
    ** file system that cannot be backed out.
    */
    if ((Dud_star_status & DUD_1STAR_DISTRIB) == DUD_1STAR_DISTRIB)
    {
	if ((du_error(&dud_errcb, I_DU0500_STAR_UTIL_INTRO, 4,
		0, DU_0DESTROYING, 0, dud_dbenv.du_ddbname))
	    != E_DU_OK)
	{
	    dud_exit(&dud_errcb);
	}
    }
    else
    {
	if ((du_error(&dud_errcb, I_DU0010_UTIL_INTRO, 4,
		0, DU_0DESTROYING, 0, dud_modes.dud_dbname))
	    != E_DU_OK)
	{
	    dud_exit(&dud_errcb); 
	}
    }
    /*
    **	Mark the database as destroy in progress.
    **  This disallows future access to the database.
    */
/* # line 558 "dudestry.qsc" */	/* begin transaction */
  {
    IIxact(0);
  }
/* # line 560 "dudestry.qsc" */	/* host code */
    /* Mark the database as being destroyed */
    {
	access_set = DU_OPERATIVE;
	if (dud_modes.dud_nowait_flag)
	    access_clear = DU_DESTROYDBNOWAIT;
	else
	    access_clear = DU_DESTROYDB;
	(VOID) STcopy (dud_dbenv.du_dbname, dbname);
/* # line 534 "dudestry.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"iiqef_alter_db");
    IILQprvProcValio((char *)"dbname",0,(short *)0,1,32,0,dbname);
    IILQprvProcValio((char *)"access_on",0,(short *)0,1,30,4,&access_set);
    IILQprvProcValio((char *)"access_off",0,(short *)0,1,30,4,&access_clear);
    IILQprvProcValio((char *)"service_on",0,(short *)0,1,30,sizeof(0),
    (void *)IILQint(0));
    IILQprvProcValio((char *)"service_off",0,(short *)0,1,30,sizeof(0),
    (void *)IILQint(0));
    IILQprvProcValio((char *)"last_table_id",0,(short *)0,1,30,sizeof(0),
    (void *)IILQint(0));
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 541 "dudestry.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    /* we were unable to execute the internal procedure */
	    dud_sqlerr_handler(E_DU200C_BAD_IPROC_EXECUTE);
	}
    }
/* # line 586 "dudestry.qsc" */	/* end transaction */
  {
    IIxact(1);
  }
/* # line 588 "dudestry.qsc" */	/* begin transaction */
  {
    IIxact(0);
  }
/* # line 590 "dudestry.qsc" */	/* host code */
    /*  Destroy the database. */
    {
/* # line 558 "dudestry.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"iiqef_destroy_db");
    IILQprvProcValio((char *)"dbname",0,(short *)0,1,32,0,dbname);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 560 "dudestry.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    /* we were unable to execute the internal procedure */
	    dud_sqlerr_handler(E_DU200C_BAD_IPROC_EXECUTE);
	}
    }
    /*	Delete all of the ABF directories.  This routine must be idempotent. */
    if ((dui_delabfpath(&dud_dbenv, &dud_errcb)) != E_DU_OK)
	dud_exit(&dud_errcb);
    if (dud_star_setup(&dud_dbenv, &dud_modes, &dud_errcb, Dud_star_status)
	!= E_DU_OK)
    {
	dud_exit(&dud_errcb);
    }
    /*
    **	If this transaction aborts after the database files have been deleted
    **  but before the ABF directories have been deleted, and STAR catalogs
    **  have been updated, the transaction can be restarted.  The database
    **  files will not exist but the procedure will recognize this and not
    **	return an error.
    */
/* # line 623 "dudestry.qsc" */	/* end transaction */
  {
    IIxact(1);
  }
/* # line 625 "dudestry.qsc" */	/* exit */
  {
    IIexit();
  }
/* # line 626 "dudestry.qsc" */	/* host code */
    dud_dbenv.du_dbopen	= FALSE;
    dud_exit(&dud_errcb);
}
/*{
** Name: dud_exit() -	destroydb exit handler.
**
** Description:
**        This is the destroydb exit handler.  It will close any database
**	that is open and release any locks that are held.  If an error
**	has occurred, the message will be printed.  Finally, a synopsis
**	message will be printed according to the exit status.
**
** Inputs:
**      dud_errcb                       Error handling control block.
**	    .du_status			The status on program exit.
**	    .du_errmsg			Any error message that may have
**					been formatted before this routine
**					was called.
**
** Outputs:
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      Several messages may be printed, then the program is exited.
**	    Any open database is closed.
**
** History:
**      23-Oct-86 (ericj)
**          Initial creation.
**	31-Oct-88 (teg)
**	    Added logic to exit with fail status if appropriate.
**	30-May-89 (teg)
**	    removed logic to do LKrelease, as LK functions have moved to the
**	    server
[@history_template@]...
*/
VOID
dud_exit(dud_errcb)
DU_ERROR	    *dud_errcb;
{
    DU_STATUS	    save_status;
    DU_STATUS	    status;
    bool	    failexit = FALSE;    
    /* If the program is exiting because of a non Ingres/Equel error,
    ** print the formatted message.  Ingres/Equel errors will have been
    ** formatted and printed by LIBQ and the destroydb Ingres error
    ** handler, dud_ingerr_handler.
    */
    save_status	= dud_errcb->du_status;
    if (dud_errcb->du_status != E_DU_OK
	&& dud_errcb->du_ingerr == DU_NONEXIST_ERROR
       )
    {
	SIprintf("%s\n\n", dud_errcb->du_errmsg);
	du_reset_err(dud_errcb);
    }
    if (Dud_dbenv->du_dbopen)
    {
/* # line 692 "dudestry.qsc" */	/* exit */
  {
    IIexit();
  }
/* # line 693 "dudestry.qsc" */	/* host code */
	Dud_dbenv->du_dbopen	= FALSE;
    }
    switch(save_status)
    {
	case E_DU_IERROR:
	case E_DU_UERROR:
	    if ((Dud_star_status & DUD_1STAR_DISTRIB) == DUD_1STAR_DISTRIB)
	    {
		du_error(dud_errcb, W_DU1010_UTIL_ABORT, 4,
			0, DU_1DESTRUCTION, 0, Dud_dbenv->du_ddbname);
	    }
	    else
	    {
		du_error(dud_errcb, W_DU1010_UTIL_ABORT, 4,
			0, DU_1DESTRUCTION, 0, Dud_dbenv->du_dbname);
	    }
	    failexit = TRUE;
	    break;
	case E_DU_OK:
	case E_DU_WARN:
	case E_DU_INFO:
	    if ((Dud_star_status & DUD_1STAR_DISTRIB) == DUD_1STAR_DISTRIB)
	    {
		du_error(dud_errcb, I_DU0501_STAR_UTIL_SUCCESS, 4,
			0, DU_1DESTRUCTION, 0, Dud_dbenv->du_ddbname);
	    }
	    else
	    {
	        du_error(dud_errcb, I_DU0011_UTIL_SUCCESS, 4,
		         0, DU_1DESTRUCTION, 0, Dud_dbenv->du_dbname);
	    }
	    break;
    }
    if (failexit)
        PCexit(FAIL);
    else
	PCexit(OK);
}
/*{
** Name: dud_ex_handler() -	Exception handler for destroydb.
**
** Description:
**        This is the exception handler for the database utility, destroydb.
**	If the exception is generated by the user, a warning message
**	is printed and the program is exited.  Any other exception will
**	generate an utility warning message and resignal the exception.
**
** Inputs:
**      ex_args                         EX_ARGS struct.
**	    .ex_argnum			Tag for the exception that occurred.
**
** Outputs:
**	Returns:
**	    The signal is resignaled if it is not recognized.
**	Exceptions:
**	    none
**
** Side Effects:
**	      If the exception was a user interrupt, the program is
**	    exited via a call to dud_exit().
**
** History:
**      23-Oct-86 (ericj)
**          Initial creation.
**	26-Jul-89 (teg)
**	    Changed EX handler to pass on unexpected exceptions without printing
**	    a failure message because some UNIX CL is implemented by spawning
**	    child processes that generate an exception to indicate they have
**	    finished.  Technically, this type of exception should not reach
**	    DUF's EX handler, but somehow it does...
**	6-Sep-90 (pete)
**	    Removed "case EXSTOP" from UNIX compiles only, at Teresa/DaveB's
**	    request. EXSTOP was removed from UNIX CL and thus this routine
**	    won't compile on UNIX with EXSTOP in it. This fix should help
**	    porting.
**	06-sep-90 (teresa)
**	    clean up to meet coding standards.
**      12-Dec-94 (liib01)
**          Cross integration from 6.4(Bug 53269). The exception handler for
**          DESTROYDB only released the lock on the database undergoing         **          deletion upon normal termination or the receipt of SIGINT. Alter
**          the handler to perform the lock release on the receipt of SIGHUP,
**          SIGQUIT and SIGTERM also.
**      17-Sep-96 (rodjo04)
**          Changed bad symbol STOP to EXSTOP from last submission (427204).
**
*/
STATUS
dud_ex_handler(ex_args)
EX_ARGS		    *ex_args;
{
    STATUS	status;
    /* report appropriate message */
    switch (EXmatch(ex_args->exarg_num, 4, (EX)EXINTR, (EX)EXQUIT, 
					(EX)EXHANGUP, (EX)EXKILL))
    {
      case 1:
      case 2:
      case 3:
      case 4:
	du_error(Dud_errcb, E_DU3600_INTERRUPT, 0);
	status = EXDECLARE;
	break;
      default:
	switch(EXmatch(ex_args->exarg_num, 7, (EX)EXFLTDIV, (EX)EXFLTOVF,
					      (EX)EXFLTUND, (EX)EXINTDIV,
					      (EX)EXINTOVF, (EX)EXSEGVIO,
# ifdef UNIX
					      (EX)EXBUSERR
# else
                                              (EX)EXSTOP
# endif
							))
	{
	    case 1:
	    case 2:
	    case 3:
	    case 4:
	    case 5:
	    case 6:
	    case 7:
		du_error(Dud_errcb, W_DU1010_UTIL_ABORT, 4,
			0, "Destruction", 0, Dud_modes->dud_dbname);
		status = EXRESIGNAL;
		break;
	    default:
		status = EXDECLARE;
		break;
	}
    }
    return(status);
}
/*{
** Name: dud_ingerr_handler	-   Error handler for Ingres errors
**				    encountered by Equel statements.
**
** Description:
**        This routine is used to handler Ingres errors that are generated
**	as a result of Equel statements.  These errors will either be
**	translated to a destroydb error, reported and exited, or sent back
**	to the program to handle itself.
**	  This routine will be passed as an argument to IIseterr().
**
** Inputs:
**      ing_error                       Ingres error number passed to this
**					handler by Libq.
**
** Outputs:
**	Returns:
**	    E_DU_OK
**	Exceptions:
**	    none
**
** Side Effects:
**	  This routine updates the fields, du_status, and du_ingerr in the
**	DU_ERROR struct through the global pointer Dud_errcb.  This is done
**	so that dud_exit() can properly handle program exit.
**	  Also, this procedure may determine that a fatal Ingres error has
**	occured, and call the exit handling procedure directly.
**
** History:
**      04-Sep-86 (ericj)
**          Initial creation.
**	25-sep-90 (teresa)
**	    fix bug where destroydb should not close a db that is not open
[@history_template@]...
*/
/* {@fix_me@} */
i4
  dud_ingerr_handler(ing_error)
i4		*ing_error;
  {
    char *err_msg;
	if (*ing_error == DU_DEADLOCK_ERROR && Dud_1dlock_retry)
	{
	    Dud_dlock_cnt++;
	    Dud_deadlock	= TRUE;
	}
	else
	{
	    /* Display the Ingres Equel error message, close the dbdb, and exit
	    ** the program.
	    */
	    /* {@fix_me@} */
	    err_msg = Dud_errcb->du_errmsg;
/* # line 887 "dudestry.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,32,0,err_msg,(char *)"errortext");
    }
/* # line 888 "dudestry.qsc" */	/* host code */
	    Dud_errcb->du_status = E_DU_UERROR;
	    Dud_errcb->du_ingerr = *ing_error;
/* # line 890 "dudestry.qsc" */	/* set_equel */
    {
      IIeqstio((char *)"errordisp",(short *)0,1,30,sizeof(1),(void *)IILQint(1
      ));
    }
/* # line 891 "dudestry.qsc" */	/* host code */
	    if (Dud_dbenv->du_dbopen==TRUE)
	    {
		/* dont close the database if it is the OPEN statement that
		** has failed.
		*/
/* # line 896 "dudestry.qsc" */	/* exit */
    {
      IIexit();
    }
/* # line 897 "dudestry.qsc" */	/* host code */
	    }
	    dud_exit(Dud_errcb);
	}
	/* Put the return here instead of inside the if block to keep lint happy */
	return((i4) E_DU_OK);
  }
/*{
** Name: dud_sqlerr_handler	-   Error handler for Ingres errors
**				    encountered by Sql statements.
**
** Description:
**	These errors will either be
**	translated to a destroydb error, reported and exited, or sent back
**	to the program to handle itself.
**
** Inputs:
**      error                       Ingres error number.
**
** Outputs:
**	Returns:
**	    E_DU_OK
**	Exceptions:
**	    none
**
** Side Effects:
**	  This routine updates the fields, du_status, and du_ingerr in the
**	DU_ERROR struct through the global pointer Dud_errcb.  This is done
**	so that dud_exit() can properly handle program exit.
**	  Also, this procedure may determine that a fatal Ingres error has
**	occured, and call the exit handling procedure directly.
**
** History:
**	25-sep-90 (teresa)
**	    fix bug where destroydb should not close a db that is not open
[@history_template@]...
*/
/* {@fix_me@} -- shouldn't this be in SQL???*/
i4
  dud_sqlerr_handler(error)
i4		error;
  {
    char *err_msg;
	if (error == DU_DEADLOCK_ERROR && Dud_1dlock_retry)
	{
	    Dud_dlock_cnt++;
	    Dud_deadlock	= TRUE;
	}
	else
	{
	    /* Display the Ingres Equel error message, close the dbdb, and exit
	    ** the program.
	    */
	    /* {@fix_me@} */
	    err_msg = Dud_errcb->du_errmsg;
/* # line 954 "dudestry.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,32,0,err_msg,(char *)"errortext");
    }
/* # line 955 "dudestry.qsc" */	/* host code */
	    Dud_errcb->du_status = E_DU_UERROR;
	    Dud_errcb->du_ingerr = error;
/* # line 957 "dudestry.qsc" */	/* set_equel */
    {
      IIeqstio((char *)"errordisp",(short *)0,1,30,sizeof(1),(void *)IILQint(1
      ));
    }
/* # line 958 "dudestry.qsc" */	/* host code */
	    if (Dud_dbenv->du_dbopen==TRUE)
	    {
		/* dont close the database if it is not open */
/* # line 961 "dudestry.qsc" */	/* exit */
    {
      IIexit();
    }
/* # line 962 "dudestry.qsc" */	/* host code */
	    }
	    dud_exit(Dud_errcb);
	}
	/* Put the return here instead of inside the if block to keep lint happy */
	return((i4) E_DU_OK);
  }
/*{
** Name: dud_init_star	-   Initialize the Star variables according to information in iidbdb.
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	    E_DU_OK
**	Exceptions:
**	    none
**
** Side Effects:
**
** History:
**      27-mar-1989 (alexc)
**          Initial creation.
[@history_template@]...
*/
DU_STATUS
  dud_init_star(dud_dbenv, dud_modes, dud_errcb, star_status)
  DU_ENV *dud_dbenv;
  DUD_MODE_VECT *dud_modes;
    DU_ERROR		*dud_errcb;
    i4			*star_status;
  {
    char dud_buffer[DB_MAXNAME];
    i4	    dud_status;
    dud_dbenv->du_ddbname[0] = EOS;
    dud_dbenv->du_cdbname[0] = EOS;
    dud_status = DUD_0STAR_NONE;
    *star_status = DUD_0STAR_NONE;
/* # line 1004 "dudestry.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(dud_buffer=iitables.table_name)where(iitables.table_name=\"i\
istar_cdbs\")or(iitables.table_name=\"IISTAR_CDBS\")");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME-1,dud_buffer);
        if (IIerrtest() != 0) goto IIrtB2;
        {
/* # line 1008 "dudestry.qsc" */	/* host code */
	dud_status = DUD_5IISTARCDB_EXIST;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE2:;
    }
/* # line 1011 "dudestry.qsc" */	/* host code */
    if ((dud_status & DUD_5IISTARCDB_EXIST) == DUD_5IISTARCDB_EXIST)
    {
	/* iistar_cdbs exist in iidbdb */
/* # line 1014 "dudestry.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(du_ddbname=iistar_cdbs.ddb_name, du_cdbname=iistar_cdbs.cdb_\
name)where(iistar_cdbs.ddb_name=");
      IIputdomio((short *)0,1,32,0,dud_modes->dud_dbname);
      IIwritio(0,(short *)0,1,32,0,(char *)")");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 4-1,dud_dbenv->du_ddbname);
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 4-1,dud_dbenv->du_cdbname);
        if (IIerrtest() != 0) goto IIrtB3;
        {
/* # line 1018 "dudestry.qsc" */	/* host code */
	    /*  This means the Database name in question is a
	    **	distributed database.
	    */
	    dud_status |= DUD_2STAR_RM_DDB;
	    dud_status |= DUD_8DDB_IN_STARCDB;
	    (VOID) STtrmwhite(dud_dbenv->du_ddbname);
	    (VOID) STtrmwhite(dud_dbenv->du_cdbname);
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE3:;
    }
/* # line 1027 "dudestry.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(du_ddbname=iistar_cdbs.ddb_name, du_cdbname=iistar_cdbs.cdb_\
name)where(iistar_cdbs.cdb_name=");
      IIputdomio((short *)0,1,32,0,dud_modes->dud_dbname);
      IIwritio(0,(short *)0,1,32,0,(char *)")");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 4-1,dud_dbenv->du_ddbname);
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 4-1,dud_dbenv->du_cdbname);
        if (IIerrtest() != 0) goto IIrtB4;
        {
/* # line 1031 "dudestry.qsc" */	/* host code */
	    /*  This means the Database name in question is a
	    **	coordinator database.
	    */
	    dud_status |= DUD_3STAR_RM_CDB;
	    dud_status |= DUD_9CDB_IN_STARCDB;
	    (VOID) STtrmwhite(dud_dbenv->du_ddbname);
	    (VOID) STtrmwhite(dud_dbenv->du_cdbname);
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE4:;
    }
/* # line 1040 "dudestry.qsc" */	/* host code */
	if (((dud_status & DUD_8DDB_IN_STARCDB) != DUD_8DDB_IN_STARCDB) &&
	    ((dud_status & DUD_9CDB_IN_STARCDB) != DUD_9CDB_IN_STARCDB))
	{
	    *star_status = DUD_15DESTRY_LDB;
	    return(E_DU_OK);
	}
/* # line 1047 "dudestry.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(dud_buffer=iidatabase.name)where(iidatabase.name=");
      IIputdomio((short *)0,1,32,0,dud_dbenv->du_ddbname);
      IIwritio(0,(short *)0,1,32,0,(char *)")");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME-1,dud_buffer);
        if (IIerrtest() != 0) goto IIrtB5;
        {
/* # line 1050 "dudestry.qsc" */	/* host code */
	    dud_status |= DUD_7DDB_IN_IIDBDB;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE5:;
    }
/* # line 1053 "dudestry.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(dud_buffer=iidatabase.name)where(iidatabase.name=");
      IIputdomio((short *)0,1,32,0,dud_dbenv->du_cdbname);
      IIwritio(0,(short *)0,1,32,0,(char *)")");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME-1,dud_buffer);
        if (IIerrtest() != 0) goto IIrtB6;
        {
/* # line 1056 "dudestry.qsc" */	/* host code */
	    dud_status |= DUD_6CDB_IN_IIDBDB;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE6:;
    }
/* # line 1059 "dudestry.qsc" */	/* host code */
	if ((dud_status & DUD_2STAR_RM_DDB) == DUD_2STAR_RM_DDB)
	{
	    /* Database name in question is in iistar_cdb as a
	    ** DDB name, if both DDB and CDB exist in iidatabase
	    ** then we can destroy the DDB.
	    */
	    if (((dud_status & DUD_7DDB_IN_IIDBDB) == DUD_7DDB_IN_IIDBDB) &&
		((dud_status & DUD_6CDB_IN_IIDBDB) == DUD_6CDB_IN_IIDBDB))
	    {
		*star_status = (DUD_1STAR_DISTRIB |
				DUD_10RM_STARDDB_ENTRY |
				DUD_12RM_DDB_IIDBDB |
				DUD_14DESTRY_CDB_AS_LDB);
	    }
	    if (((dud_status & DUD_7DDB_IN_IIDBDB) != DUD_7DDB_IN_IIDBDB) &&
		((dud_status & DUD_6CDB_IN_IIDBDB) == DUD_6CDB_IN_IIDBDB))
	    {
		/* CDB exist in iidbdb, and DDB does not.
		** Print error message that the DDB does not exist.
		** as a iidatabase entry.
		*/
		*star_status = (DUD_10RM_STARDDB_ENTRY |
				DUD_14DESTRY_CDB_AS_LDB);
	    }
	    if (((dud_status & DUD_6CDB_IN_IIDBDB) != DUD_6CDB_IN_IIDBDB) &&
		((dud_status & DUD_7DDB_IN_IIDBDB) == DUD_7DDB_IN_IIDBDB))
	    {
		/* DDB exist in iidbdb, and CDB does not.
		** Print error message that the CDB does not exist.
		** as a iidatabase entry.
		*/
		*star_status = (DUD_1STAR_DISTRIB |
				DUD_10RM_STARDDB_ENTRY |
				DUD_12RM_DDB_IIDBDB);
	    }
	    if (((dud_status & DUD_6CDB_IN_IIDBDB) != DUD_6CDB_IN_IIDBDB) &&
		((dud_status & DUD_7DDB_IN_IIDBDB) != DUD_7DDB_IN_IIDBDB))
	    {
		/* Both CDB and DDB does not exist in iidbdb.
		** Print error message.
		*/
	        *star_status = (DUD_4STAR_ABORT |
				DUD_10RM_STARDDB_ENTRY);
	    }
	}
	else if ((dud_status & DUD_3STAR_RM_CDB) == DUD_3STAR_RM_CDB)
	{
	    if (((dud_status & DUD_7DDB_IN_IIDBDB) == DUD_7DDB_IN_IIDBDB) &&
		((dud_status & DUD_6CDB_IN_IIDBDB) == DUD_6CDB_IN_IIDBDB))
	    {
		*star_status = (dud_status | DUD_4STAR_ABORT);
	    }
	    if (((dud_status & DUD_7DDB_IN_IIDBDB) != DUD_7DDB_IN_IIDBDB) &&
		((dud_status & DUD_6CDB_IN_IIDBDB) == DUD_6CDB_IN_IIDBDB))
	    {
		/* CDB exist in iidbdb, and DDB does not.
		** Print error message that the DDB does not exist.
		** as a iidatabase entry.
		*/
		*star_status = (DUD_1STAR_DISTRIB |
				DUD_11RM_STARCDB_ENTRY |
				DUD_14DESTRY_CDB_AS_LDB);
	    }
	    if (((dud_status & DUD_6CDB_IN_IIDBDB) != DUD_6CDB_IN_IIDBDB) &&
		((dud_status & DUD_7DDB_IN_IIDBDB) == DUD_7DDB_IN_IIDBDB))
	    {
		/* DDB exist in iidbdb, and CDB does not.
		** Print error message that the CDB does not exist.
		** as a iidatabase entry.
		*/
		*star_status = (DUD_1STAR_DISTRIB |
				DUD_11RM_STARCDB_ENTRY |
				DUD_12RM_DDB_IIDBDB);
	    }
	    if (((dud_status & DUD_6CDB_IN_IIDBDB) != DUD_6CDB_IN_IIDBDB) &&
		((dud_status & DUD_7DDB_IN_IIDBDB) != DUD_7DDB_IN_IIDBDB))
	    {
		/* Both CDB and DDB does not exist in iidbdb.
		** Print error message.
		*/
	        *star_status = (DUD_1STAR_DISTRIB |
				DUD_11RM_STARCDB_ENTRY);
	    }
	}
    }
    else
    {
	/*  iistar_cdbs does not exist in iidbdb, means there is no
	**  distributed database.
	*/
	*star_status = DUD_15DESTRY_LDB;
	return(E_DU_OK);
    }
    if ((*star_status & DUD_14DESTRY_CDB_AS_LDB) == DUD_14DESTRY_CDB_AS_LDB)
    {
	STcopy(dud_dbenv->du_cdbname, dud_dbenv->du_dbname);
	STcopy(dud_dbenv->du_cdbname, dud_modes->dud_dbname);
    }
    if ((*star_status & DUD_4STAR_ABORT) == DUD_4STAR_ABORT)
    {
	if ((*star_status & DUD_10RM_STARDDB_ENTRY) == DUD_10RM_STARDDB_ENTRY)
	{
	    /* CDB does not exist */
	    du_error(dud_errcb, E_DU5102_STAR_DDB_DONT_EXIST, 2,
		     0, dud_dbenv->du_ddbname);
	    dud_exit(dud_errcb);
	}
	else if (((*star_status & DUD_7DDB_IN_IIDBDB) == DUD_7DDB_IN_IIDBDB) &&
		((*star_status & DUD_6CDB_IN_IIDBDB) == DUD_6CDB_IN_IIDBDB))
	{
	    /* Can not destroy CDB */
	    du_error(dud_errcb, E_DU5101_STAR_CANT_DEST_CDB, 2,
		     0, dud_dbenv->du_cdbname);
	    dud_exit(dud_errcb);
	}
    }
    if (((*star_status & DUD_14DESTRY_CDB_AS_LDB)
	    != DUD_14DESTRY_CDB_AS_LDB) &&
	((*star_status & DUD_15DESTRY_LDB)
	    != DUD_15DESTRY_LDB))
    {
	dud_exit(&dud_errcb);
    }
    return(E_DU_OK);
  }
/*{
** Name: dud_star_setup	- Executes the needed commands according to the state
**			of the star_status variable.
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	    E_DU_OK
**	Exceptions:
**	    none
**
** Side Effects:
**
** History:
**      27-mar-1989 (alexc)
**          Initial creation.
[@history_template@]...
*/
DU_STATUS
  dud_star_setup(dud_dbenv, dud_modes, dud_errcb, star_status)
  DU_ENV *dud_dbenv;
  DUD_MODE_VECT *dud_modes;
    DU_ERROR		*dud_errcb;
    i4			star_status;
  {
    if ((star_status & DUD_10RM_STARDDB_ENTRY) == DUD_10RM_STARDDB_ENTRY)
    {
	IIlq_Protect(TRUE);
/* # line 1218 "dudestry.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"range of cdb_catalog=iistar_cdbs");
      IIsyncup((char *)0,0);
    }
/* # line 1219 "dudestry.qsc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"delete cdb_catalog where cdb_catalog.ddb_name=");
      IIputdomio((short *)0,1,32,0,dud_dbenv->du_ddbname);
      IIsyncup((char *)0,0);
    }
/* # line 1220 "dudestry.qsc" */	/* host code */
	IIlq_Protect(FALSE);
    }
    if ((star_status & DUD_11RM_STARCDB_ENTRY) == DUD_11RM_STARCDB_ENTRY)
    {
	IIlq_Protect(TRUE);
/* # line 1225 "dudestry.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"range of cdb_catalog=iistar_cdbs");
      IIsyncup((char *)0,0);
    }
/* # line 1226 "dudestry.qsc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"delete cdb_catalog where cdb_catalog.cdb_name=");
      IIputdomio((short *)0,1,32,0,dud_dbenv->du_cdbname);
      IIsyncup((char *)0,0);
    }
/* # line 1227 "dudestry.qsc" */	/* host code */
	IIlq_Protect(FALSE);
    }
    if ((star_status & DUD_12RM_DDB_IIDBDB) == DUD_12RM_DDB_IIDBDB)
    {
	IIlq_Protect(TRUE);
/* # line 1232 "dudestry.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"range of database_catalog=iidatabase");
      IIsyncup((char *)0,0);
    }
/* # line 1233 "dudestry.qsc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"delete database_catalog where database_catalog.name=");
      IIputdomio((short *)0,1,32,0,dud_dbenv->du_ddbname);
      IIsyncup((char *)0,0);
    }
/* # line 1235 "dudestry.qsc" */	/* host code */
	IIlq_Protect(FALSE);
    }
    if ((star_status & DUD_13RM_CDB_IIDBDB) == DUD_13RM_CDB_IIDBDB)
    {
	IIlq_Protect(TRUE);
/* # line 1240 "dudestry.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"range of database_catalog=iidatabase");
      IIsyncup((char *)0,0);
    }
/* # line 1241 "dudestry.qsc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"delete database_catalog where database_catalog.name=");
      IIputdomio((short *)0,1,32,0,dud_dbenv->du_cdbname);
      IIsyncup((char *)0,0);
    }
/* # line 1243 "dudestry.qsc" */	/* host code */
    }
    return(E_DU_OK);
  }
/*{
** Name: dud_confirm()	-   query the user to confirm the destruction of the
**			    database.
**
** Description:
**        This routine queries the user to get confirmation before destroying
**	the database. 
**
**
** Inputs:
**	dbname				Ptr to buffer containing name of
**					database to be destroyed.
**
** Outputs:
**	Returns:
**	    TRUE			User confirmed destruction of db.
**	    FALSE			User did not want to destroy db.
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      04-Sep-86 (ericj)
**          Initial creation.
[@history_template@]...
*/
i4
dud_confirm(dbname)
char		*dbname;
{
    /* {@fix_me@} */
#define	MAXLINE		128
    char                reply[MAXLINE],
			reply1[MAXLINE];
    char		buf[MAXLINE];
    char		*ptr;
    i4			status;
    status  = FALSE;
    for(;;)
    {
	ptr = STpolycat(3, DU_DSTRYMSG, dbname, DU_ANSWER_PROMPT, buf);
	SIprintf("%s", buf);
	if (SIgetrec(reply, MAXLINE, stdin) != OK)
	    break;
	STzapblank(reply, reply1);
	CVlower(reply1);
	if (!STcompare(DU_YES, reply1)
	    || (reply1[0] == DU_Y && reply1[1] == '\0')
	   )
	{
	    status  = TRUE;
	    break;
	}
	if (!STcompare(DU_NO, reply1) || 
	   (reply[0] == DU_N && reply1[1] == '\0'))
	{
	    break;
	}
    }	    /* end of for stmt */
    return(status);
}
/*{
** Name: dud_get_args	-   parse the command line, setting flags and
**			    parameters accordingly.
**
** Description:
**        This routine parses the command line, setting mode flags and
**	utility parameter appropriately.
**
** Inputs:
**      argc                            Command line argument count.
**	argv				Command line argument vector.
**	dud_dbenv			ptr to environment control block
**	dud_modes			Ptr to a DUD_MODE_VECT.
**	dud_errcb			Ptr to an error control block.
**
** Outputs:
**	*dud_errcb
**	    .du_duilerr			The utility error code that was
**					generated if anything other than
**					E_DU_OK is returned by this routine.
**	    .du_errmsg			Buffer containing the formatted error
**					message that corresponds to the error
**					code in .du_duilerr.
**      *dud_modes                       The resulting modes and parameters.
**	    .dud_query_flag		TRUE, iff the "query user for
**					confirmation" flag was used.
**	    .dud_alias_usr		Buffer containing the "user alias".
**					This is the parameter associated with
**					the "-u" flag.  If the "-u" flag is not
**					used, this is the NULL string.
**	    .dud_nowait_flag		TRUE, iff '-l' specified.
**	    .dud_dbname			Buffer containing the name of the
**					database to be destroyed.
**	Returns:
**	    E_DU_OK			Routine completed successfully.
**	    E_DU_UERROR			A user-generated error was detected.
**
**	    The following system utility error codes can be set in the
**	    .du_duilerr field of the dud_errcb when a DU_STATUS of
**	    E_DU_UERROR is returned.
**	    E_DU3100_UCODE_OLDUSAGE	Somebody tried to use an Ingres user
**					code with the "-u" flag.
**	    E_DU3000_BAD_USRNAME	A syntactically incorrect user name
**					was passed with the "-u" flag.
**	    E_DU3101_UNKNOWN_FLAG_DU	An unknown parameter was passed on
**					the command line.
**	    E_DU3010_BAD_DBNAME		A syntactically incorrect Ingres
**					database name given.
**	    E_DU3012_NODS_DBDB_DS	The user requested that the database
**					database be destroyed.  This is never
**					allowed.
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      08-Sep-86 (ericj)
**          Initial creation.
**	21-dec-92 (robf)
**	    Old -s option is a no-op. Issue a warning if its used
**	    per LRC decision.
**	11-Feb-1993 (jpk)
**	    Support destroydb over the net: parse node from database name
**	    on command line.  New function parameter to point to environment
**	    where node name gets stored.
**	10-Nov-1994 (ramra01)
**	    destroydb nowait mode if cmd line option set 'l'
**	16-aug-1995 (harpa06)
**	    Bug #70604 - Flag an error if a database to destroy has already
**	    been specified and the user is trying to destroy more than one
**	    on the commnad line.
**	12-may-1997 (nanpr01)
**	    -uflag can be used by anyone to destroy a database.
[@history_template@]...
*/
DU_STATUS
dud_get_args(argc, argv, dud_dbenv, dud_modes, dud_errcb)
i4		    argc;
char		    *argv[];
DU_ENV		    *dud_dbenv;
DUD_MODE_VECT	    *dud_modes;
DU_ERROR	    *dud_errcb;
{
    char	    *p;
    char	    *colon_ptr;		/* to detect node name syntax */
    u_i4       idmode = 0;
    u_i4           normal_len;
    DB_STATUS       status;
    DB_ERROR        error;
    VOID            du_get_usrname();
    /* Get the process's user name */
    du_get_usrname(dud_dbenv->du_usrinit);
    if (du_chk2_usrname(dud_dbenv->du_usrinit) != OK)
	return(du_error(dud_errcb, E_DU3000_BAD_USRNAME, 2, 0, 
			dud_dbenv->du_usrinit));
    else
    {
	/* 
	** Initialize the effective user name to that of the process's
	** user name.
	*/
	STcopy(dud_dbenv->du_usrinit, dud_dbenv->du_xusrname);
    }
    /* The first arg is the program being executed. */
    while (--argc)
    {
	p   = *(++argv);
	switch (p[0])
	{
	  case '-':
	    switch (p[1])
	    {
	      case 'u':
		if (p[2] == ':')
		{
		    return(du_error(dud_errcb, E_DU3100_UCODE_OLDUSAGE, 0));
		}
		else
		{
		    if (du_chk2_usrname(&p[2]) != OK)
			return(du_error(dud_errcb, E_DU3000_BAD_USRNAME, 2,
					0, &p[2]));
		    else
			STcopy(&p[2],dud_dbenv->du_unorm_usr); 
		}
		/* normalize the identifier
		** (i.e. remove any double-quote delimiters)
		*/
		normal_len = sizeof(dud_dbenv->du_xusrname);
		status = cui_idxlate(
			    (u_char *) dud_dbenv->du_unorm_usr, (u_i4 *) 0,
			    (u_char *) dud_dbenv->du_xusrname,  &normal_len,
			    (u_i4) CUI_ID_DLM_M, &idmode, &error);
		if (DB_FAILURE_MACRO(status))
		{
		    status = du_error(dud_errcb, E_DU3000_BAD_USRNAME,
					 2, 0, dud_dbenv->du_unorm_usr);
		    return(status);
		}
		/* 
		** null-terminate the string
		*/
		dud_dbenv->du_xusrname[normal_len] = EOS;
		if (du_chk2_usrname(dud_dbenv->du_xusrname) != OK)
		{
		    return(du_error(dud_errcb, E_DU3000_BAD_USRNAME, 2,
					0, dud_dbenv->du_xusrname));
		}
		break;
	      case 's':
		/*
		** This option used to mean SUPERUSER. This is now 
		** determined automatically, so is a no-op. Per LRC
		** decision accept the option but issue a warning.
		*/
	        _VOID_ du_error(dud_errcb, W_DU1831_SUPUSR_OBSOLETE, 0);
		break;
	      case 'p':
		dud_modes->dud_query_flag = TRUE;
		break;
	      case 'l':
		dud_modes->dud_nowait_flag = TRUE;
		break;
	      default:
		return(du_error(dud_errcb, E_DU3101_UNKNOWN_FLAG_DS, 2, 0, p));
		break;
	    }	/* end of switch (p[1]) */
	    break;
	  default:
	    /* Must be a database name parameter */
	    /* check if a database to destroy has already been specified */
	    if (STlength(dud_modes->dud_dbname) != 0)
		return (du_error(dud_errcb, E_DU351A_MULTI_DB_DESTRY, 0));
	    else
	    {
	       dud_dbenv->du_nodename[0] = EOS;	/* initialize */
	       /* check for a node name of form node::database */
	       if (((colon_ptr = STchr(p, ':')) != NULL) && 
		   STchr(colon_ptr + 1, ':') == (colon_ptr + 1))
	       {
		   /* copy nodename, everything to left of "::" */
		   STncpy( dud_dbenv->du_nodename, p, colon_ptr - p);
		   dud_dbenv->du_nodename[colon_ptr - p] = '\0';
		   /* copy database name, everything to right of "::" */
		   STcopy(colon_ptr + 2, p);
	       }
	       if (du_chk1_dbname(p) != OK)
		   return(du_error(dud_errcb, E_DU3010_BAD_DBNAME, 2, 0, p));
	       else
		   STcopy(p, dud_modes->dud_dbname);
	       break;
	    }
	}   /* end of switch(p[0]) */
    }	/* end of while loop */
    if (dud_modes->dud_dbname[0] == EOS)
	return(du_error(dud_errcb, E_DU3014_NO_DBNAME, 0));
    /*  Is the user trying to destroy the database database,
    **  which is not allowed.
    */
    if (!STcompare(dud_modes->dud_dbname, DU_DBDBNAME))
	return(du_error(dud_errcb, E_DU3012_NODS_DBDB_DS, 0));
    return(E_DU_OK);
}
/*{
** Name: dud_init()	-   initialize variables and structures used in
**			    destroydb.
**
** Description:
**        This routine initializes the variables and structures used by
**	destroydb.
**
** Inputs:
**      du_env                          Ptr to DU_ENV struct to be initialized.
**	dud_modes			Ptr to DUD_MODE_VECT struct to be
**					initialized.
**	du_errcb			Ptr to DU_ERROR struct to be
**					initialized.
**
** Outputs:
**      *du_env                         Initialized DU_ENV struct.
**					lock list.
**	*dud_modes			Initialized DUD_MODE_VECT struct.
**	*du_errcb			Initialized DU_ERROR struct.
**	Returns:
**	    E_DU_OK			Completed successfully.
**	    E_DU3190_BAD_LKINIT		Couldn't initialize the locking system
**					for this process.
**	    E_DU3193_BAD_LKLIST_CREATE	Couldn't create a lock list for this
**					process.
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      11-Sep-86 (ericj)
**          Initial creation.
**	1-Dec-88 (teg)
**	    Unix requires that LKrelease be called after LKcreate_list.  So
**	    I set the du_1dblocked flag here (instead of waiting until a
**	    specific lock is taken).  This will assure all error exit paths
**	    will release the locklist created here. [bug 4126]
**	6-july-2004 (shust01)
**	    Initialize dud_nowait_flag.  If a session had a lock on a database,
**	    destroydb was, at times, returning "E_US120A Database is already 
**	    open" instead of waiting for the lock to be released. bug 112079.
*/
DU_STATUS
dud_init(du_env, dud_modes, du_errcb)
DU_ENV		    *du_env;
DUD_MODE_VECT	    *dud_modes;
DU_ERROR	    *du_errcb;
{
    DU_STATUS		status;
    char		tmp_buf[DU_APPL_FLAG_MAX];
    /* Initialize the variables used for deadlock detection and retry. */
    Dud_dlock_cnt	= 0;
    Dud_1dlock_retry	= FALSE;
    Dud_deadlock	= FALSE;
    /* Initialize the application startup flag. */
    CVla((i4) DBA_DESTROYDB, tmp_buf);
    STpolycat((i4) 2, DU_1APPL_FLAG, tmp_buf, Dud_appl_flag);
    /* Initialize the system utility environment */
    du_envinit(du_env);
    Dud_dbenv	= du_env;
    /* Initialize the system utility error-handling struct */
    du_reset_err(du_errcb);
    Dud_errcb	= du_errcb;
    /* initialize the destroydb modes vector */
    dud_modes->dud_super_usr	= FALSE;
    dud_modes->dud_query_flag	= FALSE;
    dud_modes->dud_nowait_flag	= FALSE;
    dud_modes->dud_alias_usr[0]	= EOS;
    dud_modes->dud_dbname[0]	= EOS;
    Dud_modes	= dud_modes;
    du_env->du_1dblocked = TRUE;
    return(E_DU_OK);
}
/*{
** Name: get_pm_params - get PM parameters regarding database destruction.
**
** Description:
**      Users can specify installation wide FIPS databases, or
**      the opposite, using config.dat.  This routine makes those
**      settings take effect.  
**
** Inputs:
**      dud_dbenv                       Ptr to a DU_ENV struct.
**                                      Saves PM params until we
**                                      write them to iidbcapabilities.
**      dud_errcb                       Ptr to a DU_ERROR struct.
**
** Outputs:
**      None
**      *dud_errcb                      If an error occurs, this struct will
**                                      be set by a call to du_error().
**
** History:
**	03-nov-97 (nanpr01)
**	    copied from createdb for bug 86833.
*/
STATUS
get_pm_params(dud_dbenv, dud_errcb)
DU_ENV          *dud_dbenv;
DU_ERROR        *dud_errcb;
{
    char        *val;
    /* 
    ** yes, createdb has a default default default.  
    */
    STcopy("LOWER", dud_dbenv->du_reg_id_case);
    STcopy("LOWER", dud_dbenv->du_delim_id_case);
    STcopy("LOWER", dud_dbenv->du_real_user_case);
    dud_dbenv->du_got_delim_id_case = dud_dbenv->du_got_reg_id_case
        = dud_dbenv->du_got_real_user_case = 'd';
    /* thus, if user wants installation wide FIPS databases, she can
    ** set appropriate parameters in config.dat or via cbf; 
    ** lacking all that, databases are not FIPS, the way God intended.
    */
    PMinit();
    if (PMload( (LOCATION *) NULL,  (PM_ERR_FUNC *) NULL) != OK)
    {
	du_error(dud_errcb, W_DU1086_CANT_LOAD_PM_FILE, 0);
	return FAIL;
    }
    if( PMsetDefault( 1, PMhost() ) != OK )
    {
	du_error(dud_errcb, W_DU1088_PMSETDEFAULT_FAILED, 0);
	return FAIL;
    }
    if (PMget("ii.$.createdb.reg_id_case", &val) == OK)
    {
	CVlower(val);
	if (STequal(val, "lower") != 0)
	{
	    STcopy("LOWER", dud_dbenv->du_reg_id_case);
	    dud_dbenv->du_got_reg_id_case = 'p';
	}
	if (STequal(val, "upper") != 0)
	{
	    STcopy("UPPER", dud_dbenv->du_reg_id_case);
	    dud_dbenv->du_got_reg_id_case = 'p';
	}
	/* else needn't copy, use default default default */
    }
    if (PMget("ii.$.createdb.delim_id_case", &val) == OK)
    {
	CVlower(val);
	if (STequal(val, "lower") != 0)
	{
	    STcopy("LOWER", dud_dbenv->du_delim_id_case);
	    dud_dbenv->du_got_delim_id_case = 'p';
	}
	if (STequal(val, "upper") != 0)
	{
	    STcopy("UPPER", dud_dbenv->du_delim_id_case);
	    dud_dbenv->du_got_delim_id_case = 'p';
	}
	if (STequal(val, "mixed") != 0)
	{
	    STcopy("MIXED", dud_dbenv->du_delim_id_case);
	    dud_dbenv->du_got_delim_id_case = 'p';
	}
	/* else needn't copy, use default default default */
    }
    if (PMget("ii.$.createdb.real_user_case", &val) == OK)
    {
	CVlower(val);
	if (STequal(val, "lower") != 0)
	{
	    STcopy("LOWER", dud_dbenv->du_real_user_case);
	    dud_dbenv->du_got_real_user_case = 'p';
	}
	if (STequal(val, "upper") != 0)
	{
	    STcopy("UPPER", dud_dbenv->du_real_user_case);
	    dud_dbenv->du_got_real_user_case = 'p';
	}
	if (STequal(val, "mixed") != 0)
	{
	    STcopy("MIXED", dud_dbenv->du_real_user_case);
	    dud_dbenv->du_got_real_user_case = 'p';
	}
    }
    else
    {
	/* if no PM default for userid case, default is same as regid case */
	STcopy(dud_dbenv->du_reg_id_case, dud_dbenv->du_real_user_case);
    }
    PMfree();
    return OK;
}