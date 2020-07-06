# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<me.h>
#include	<st.h>
#include	<ut.h>
# include       <ftdefs.h>
#include	<er.h>
#include	<dbms.h>
#include	<fe.h>
#include	<si.h>
# include <ui.h>
#include	<ug.h>
#include	<uigdata.h>
#include	<lqgdata.h>
#include	<stdprmpt.h>
#include	<generr.h>
#ifdef DBI
    #include	<qp.h>
    #include	<ql.h>
#endif /* DBI */
# ifdef wgl_win
# include <iisqlca.h>
# endif
/**
** Name:	feingres.sc -	Front-End DBMS (Ingres) Start-up Module.
**
** Description:
**	Contains routines that start up the DBMS connection (INGRES) and
**	initializes some global data retrieved from database constants.
**	Defines:
**
**	FEingres()	DBMS connect utility (wrapper).
**	FEningres()	DBMS connect utility.
**	FEing_exit()	DBMS disconnect utility.
**	FEing_cleanup()	clean up DBMS on exit.
**	IIUIck1stAutoCom()	check initial auto-commit state.
**
** History:
**
**      22-may-1990 (pete) add more error messages to match up with new
**              return codes from CheckDictVer.
**	3-may-1990  (pete) add call to UTexe for DictCheck.
**	24-apr-1990 (pete)
**		Gut 'FEingres' and create 'FEningres', in preparation
**		for IIUICheckDictVer().
**
**	Revision 6.4  89/11/30  mikes
**    	Moved global data into structure.
**	89/12  wong  Added compatiblity check.
**
**	Revision 6.3  89/11  wong
**	Modified to connect to UI_SESSION to support multiple sessions.
**
**	Revision 6.2  88/12  wong
**	Added set of 'IIUIisdba'.
**
**	Revision 6.1  88/10  danielt
**	Gateway changes.
**	88/11  danielt  Performance changes.
**	89/03  wong  Bug 3768, 4360 and 4361 (4224) changes:  Abstracted out
**	'IIUIsharedData()' etc. and added 'IIUIck1stAutoCom()'.
**
**	Revision 6.0  88/03
**	05-may-1987 (sandyd)
**		Removed the hack which was previously required for INGRES/STAR:
**		("where any(fdframes.frname) = 1 or 1 = 1).  Also removed
**		the "_Version" global.
**	14-may-1987 (sandyd)
**		Added IIUIdba and IIUIuser.  These will REPLACE 'User', 'Dba',
**		and 'UserName' in the near future.
**		Also changed buffer sizes from DB_MAXNAME to FE_MAXNAME
**		(which will be needed for DG).
**	03-aug-1987 (wong)
**		Removed 'User', 'Dba', and 'UserName'.  Also, 'FEcx_savearea'.
**	22-oct-1987 (peter)
**		Change HACKFOR50 SQL to QUEL.  Add IIUIdatabase.
**	28-oct-1987 (peter)
**		Trim the trailing spaces off the globals.
**
**	Revision 5.0  86/08
**	08-oct-1986 (sandyd)
**		Added changes necessary for Saturn.  Also changed the array
**		allocations to use literals from ingconst.h.  Added "trim"
**		function on Dba and User in anticipation of our switch to
**		using full login names for those codes.  Also included things
**		PeterK needed for terminal monitor:  ing_cleanup() made into
**		FEing_cleanup(), and FEingres() now takes up to 13 parameters.
**	9-oct-1986 (Joe)
**		Added code to check for error after starting INGRES.
**		This can happen if caller specified -$ flag.
**		In this case, don't do any queries.
**	17-oct-1986 (sandyd)
**		"##exit" if the retrieve fails.  Database should be left
**		CLOSED if FEingres() returns anything other than success.
**	03-dec-1986 (sandyd)
**		Updated comments to reflect changes made to support
**		distributed views.
**	30-dec-1986 (a.dea) -- Zero fill FEcx_savearea.
**
**	Revision 4.0  86/01/22  wong
**	Initial revision.
**	28-aug-1990 (Joe)
**	     Changed to use the global data structure of IIUIdbdata()
**           and to call IIUIssdStoreSessionData instead of IIUIgetData
**	     and IIUIsharedData.
**	28-sep-1990 (Joe)
**	     Made IIUIchk1stAutoCom and IIui1stAutocom work like they
**	     used to.  See the explanation in IIUIchk1stAutoCom for
**	     an explanation of what this does.
**	11/02/90 (dkh) - Replaced IILIBQgdata with IILQgdata().
**	09/10/93 (valerier)
**           Fixed number of UTexe arguments for WindowsNT.
**	21-jul-93 (joe)
**		Added EXEC SQL INCLUDE SQLCA to the include section
**	03-sep-93 (chan)
**		Add LOADDS modifier for FEing_cleanup.
**	05-jan-94 (sandyd)
**		Added ifdefs for DBI (Database Independence) project.
**		Because we don't support the -X flag under DBI, much of
**		FEningres() stuff becomes irrelevant.
**	25-apr-94 (sandyd)
**		Added DBI case to milovan's new IIUISQLconnect() routine.
**		Also fixed DBI FEningres() to call IIQLisqInitSql() *before*
**		doing the connect, to make sure wart storage is available
**		for possible errors on the connect.
**	07-may-94 (sandyd)
**		Straightening out DBI connection model --- to permit handling
**		of connection errors.  Must leave new hdbc current, even on
**		failure, so that caller's INQUIRE's will have a relevant hdbc
**		to look at.  NOTE that this means the *caller* is responsible 
**		for freeing the hdbc, regardless of whether the connection
**		succeeded.
**	13-oct-94 (leighb)
**		Changed static's to STATICDEF's.
**	19-apr-95 (leighb)
**		Replace "GetVersion() & 0x80000000" with "IIwin32s()".
**		Added include on me.h.
**	22-may-95 (leighb)
**		Replace "IIwin32s()" with "IIgetOSVersion()".
**	13-jun-95 (fraser)
**		DBI change: fix case where error causes access violation
**		because program has not obtained a connection handle.
**	12-dec-1996 (donc)
**	    Moved form OpenROAD, which appears to have a lot more fixes and
**	    DBI support.  Changed STATICDEFs back to statics. 
**	02-jan-97 (mcgem01)
**	    Various builds have been broken by the introduction of the
**	    LOADDS compiler directive.  This modifier should have been 
**	    added for Windows 3.1 only.
**	10-feb-97 (puree)
**		Remove IIgetOSVersion().
**	15-Mar-1997 (jenjo02)
**	    Added SET TRANSACTION READ ONLY prior to select dbmsinfo to get
**	    autocommit state, to avoid starting a read-write transaction
**	    which might be blocked by a pending CKPDB.
**	28-Mar-1997 (jenjo02)
**	    Backed out above change (SET TRANSACTION READ ONLY). Front-end
**	    clients are "traditionally backward compatible", so this new SQL
**	    will fail on pre 2.0 servers.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	22-feb-2005 (srisu02)
**	    Change FE client version from 2 to 4
**	16-jun-2005 (gupsh01)
**	    Added code to initialize the unicode collation table for
**	    unicode database connections.
**	27-sep-2005 (lunbr01)  bug 115280, problem INGCBT 575
**          OpenROAD and other Ingres forms-based applications crash
**          intermittently when using vnodeless connection strings;
**          this is because they are typically longer than normal
**          vnode strings and can overlay allotted memory.
**          Increase MAX_WORD_SIZE from 64 to 256.
*/
VOID	FEing_exit();
STATUS	FEinqerr();
VOID	FEset_exit();
VOID	FEclr_exit();
FUNC_EXTERN     char    *IIxflag();
FUNC_EXTERN	STATUS	IIUICheckDictVer();
static VOID	dbms_exit();
static STATUS	upgradedict();
static bool	checkif_upgrade();
static STATUS	checkdictmsg();
static i4 iiuiDict_checked();
static VOID delete_htab();
static char	G_upgradefe[]	= ERx("upgradefe");
/*
** These variables are used by DG code to give a with clause to use
** on the connection.  The WithClause variable is set by the routine
** 
** RO_Status will be set to TRUE if the WithClause was determined to
** have a mode=readonly in it.
*/
  static char *WithClause = NULL;
# ifdef DGC_AOS
static bool	RO_Status = FALSE;	/* if WithClause contains readonly */
# endif /* DGC_AOS */
/*
** This variable is set by calling IIUIck1stAutoCom.  If it is set, then
** if FEingres is starting a new session (i.e. this is not connecting to
** a shared session with the -X flag), IIUIdbdata->firstAutocom will be set
** to whether autocommit is on or off.
*/
static bool	check1st = FALSE;	/* Check auto-commit on 1st startup */
/*
** Name:	FEing_cleanup() -	Clean-Up DBMS on Exit.
**
** Description:
**	Re-synchronize with the DBMS and then terminate connection.
**
** History:
**	09/89 (jhw) -- Execute ROLLBACK to abort any transactions. JupBug #7946.
**	11/89 (jhw) -- Set session before re-sync., etc. for multiple session
**		support.
**	03-sep-93 (chan)
**		Add LOADDS modifier for MS-Win 3.1
**	30-jan-94 (sandyd)
**		Ifdef'ed for DBI.  (Our DBI preprocessor doesn't handle
**		SET SESSION statements.  And we need to get rid of the
**		direct LIBQ call.)
**	02-jan-97 (mcgem01)
**	    Various builds have been broken by the introduction of the
**	    LOADDS compiler directive.  This modifier should have been 
**	    added for Windows 3.1 only.
*/
# ifdef WIN16
i4  LOADDS
# else
i4  
# endif
FEing_cleanup()
{
# ifdef DBI
	{
	    IIQL_HDBC	hdbc;
	    hdbc = IIQPgshGetSessionHandle((i4)UI_SESSION);
	    IIQPscdSetCurrentDbc(hdbc);
	    (void) IIQLqcaQryCancel((IIQL_HGENERIC)hdbc, IIQL_HTYPE_DBC);
	}
# else /* not DBI */
/* # line 250 "feingres.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(UI_SESSION),(void *)IILQint(
    UI_SESSION));
  }
/* # line 251 "feingres.sc" */	/* host code */
	IIresync();
# endif /* DBI */
/* # line 253 "feingres.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 254 "feingres.sc" */	/* host code */
	dbms_exit();
}
/*{
** Name:	FEingres() -	Front-End DBMS Connect Utility.
**
** Description:
**	Starts a connection to the DBMS (INGRES) with the passed in parameters
**	in parameters as arguments.  The list of these parameters should be
**	null-terminated.  Parameters themselves can not be null pointers (since
**	that would terminate the input), but any parameters that point to empty
**	strings will be ignored.  Once the DBMS is started, the user name and
**	DBA name are retrieved from the database, and the database name set.
**
**	If the DBMS session is being shared with a parent process (i.e.
**	the -X flag is present), FEingres assumes that the caller is
**	either another Ingres FE, or an ABF application (since they are
**	the only things that know about the -X flag).  In this case,
**	FEingres stores the autocommit state so that FEing_exit can restore
**	it to what it was.
**
** Inputs:
**	db_name	{char *}  The database name on which to start-up the DBMS.
**	a[0-12]	{char *}  additional parameters to be passed to the DBMS
**				(terminated by a NULL pointer.)
**
** Returns:
**	{STATUS}	'FEinqerr()' If this return value is not OK, then the
**					database is always left in a closed
**					state.  Failure can occur both from
**					'IIdbconnect()', and from a retrieve
**					for the Dba and Username.
**
** Side Effects:
**	Sets up 'IIUIgdata-> 'user', 'dba', 'database' and the exit clean-up
**	routine.
**
** History:
**	01/86 (jhw) -- Written.
**	08-oct-1986 (sandyd)
**		(deleted 5.0 STAR changes 05/87)
**	9-oct-1986 (Joe)
**		Check for errors after starting INGRES.  If there are
**		errors, don't run any queries.
**	17-oct-1986 (sandyd)
**		"##exit" if the retrieve fails.  Database should be left
**		CLOSED if FEingres() returns anything other than success.
**	03-aug-1987 (jhw) -- Removed 'User', 'Dba', 'UserName'; replaced with
**				'IIUIuser' and 'IIUIdba'.
**	22-oct-1987 (peter)
**		Added IIUIdatabase.
**	28-oct-1987 (peter)
**		Trim trailing spaces in globals.
**	02/88 (jhw) -- Added support to commit transactions
**					and set the autocommit state.
**	4-sept-1988 (danielt)
**		performance/transaction changes
**	10/88 (jhw) corrected previous changes.
**	8-dec-1988 (danielt)
**		changed save buffer size to UI_SAVEBUF_SIZE
**	03/89 (jhw) - Abstracted out 'IIUIsharedData()' and 'IIUIgetData()'.
**	11/89 (jhw) -- Modified to connect to UI_SESSION for multiple session.
**			support.
**	04/23/90 (pete) -- change name to FEningres() and make FEingres into
**		a wrapper routine with two additional arguments:
**		dictClientNm & dictClientVrs. These are to be used
**		with new support for IIUICheckDictVer().
**	05/08/90 (pete -- All front-ends that call FEingres identify
**		themselves as Client "INGRES" Version 1 (formerly set
**		to Version 2).
**      18-jun-1990 (barbara)
**              LIBQ's new default behavior is NOT to quit on association
**              failures (bug 21832).  SET_INGRES PROGRAMQUIT provides backward
**              compatibility for front ends.
**	17-aug-1990 (Joe)
**	    Changed to call IIUIssdStoreSessionData to save the UI data.
**	    NOTE:  It is important that this routine call
**	    IIUIssdStoreSessionData before calling any of the UI routines
**	    that return the global data like IIUIautocommit.
**	9/12/90 (Mike S)
**	    Use Ingres version 2
**	10/1/90 (pete)
**	    Add argument to E_UI002A_UPGRADE_NOTDBA & E_UI0029_UPGRADE_DBA
**	    (message text changed to reference upgradefe also).
**	30-jan-94 (sandyd)
**	    Added ifdefs for DBI project.  Since we don't support the -X flag
**	    in DBI, it was simplest to just ifdef almost the whole function as 
**	    a block.
**	07-may-94 (sandyd)
**	    Straightening out DBI connection model --- to permit handling
**	    of connection errors.  Must leave new hdbc current, even on
**	    failure, so that caller's INQUIRE's will have a relevant hdbc
**	    to look at.  NOTE that this means the *caller* is responsible 
**	    for freeing the hdbc, regardless of whether the connection
**	    succeeded.
**	13-jun-95 (fraser)
**	    DBI change: get connection handle before registering error.
**	16-jun-2005 (gupsh01)
**	    Added code to initialize the unicode collation table for
**	    unicode database connections.
**	3-May-07 (kibro01) b118230
**	    FE tools need to know about date_type_alias
**	18-Jun-07 (kibro01) b118230/b118511
**	    Backed out change for b118230, now fixed in FEadfcb()
*/
/* VARARGS2 */
STATUS
FEingres (db_name, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)
  char *db_name, *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10, *a11, *a12
  ;
{
	return	( FEningres(ERx("INGRES"), (i4) 4, db_name,
		      a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12));
}
/* VARARGS4 */
STATUS
FEningres (dictClientNm, dictClientVrs, db_name,
	   a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)
  char *dictClientNm;
  i4 dictClientVrs;
  char *db_name, *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10, *a11, *a12
  ;
# ifndef DBI
{
	register i4	i;
	bool		read_globals;
	bool		appl;
	bool		sharedSession = FALSE;
        bool	        autocommit = FALSE;
	STATUS		rval;
	STATUS		dval;
	char		*arg_list[12];
	/* XXX Hack for SCF bug #9065.  Remove when corrected. */
	if ( WithClause == NULL || *WithClause == EOS )
/* # line 413 "feingres.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(UI_SESSION);
    IIsqConnect(0,db_name,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,(char *)0);
  }
/* # line 417 "feingres.sc" */	/* host code */
	else
/* # line 418 "feingres.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(UI_SESSION);
    IIsqMods(2);
    IIsqConnect(0,db_name,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,(char *)0);
    IIsqParms(0,WithClause,32,(char *)0,0);
    IIsqParms(2,(char *)0,0,(char *)0,0);
  }
/* # line 423 "feingres.sc" */	/* host code */
	/*
	** Quit on association failures for compatibility with
	** pre 6.3 releases.
	*/
/* # line 427 "feingres.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(7,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
  }
/* # line 428 "feingres.sc" */	/* host code */
	if ( (rval = FEinqerr()) != OK )
		return rval;
	FEset_exit(FEing_cleanup);
	arg_list[0] = a1;
	arg_list[1] = a2;
	arg_list[2] = a3;
	arg_list[3] = a4;
	arg_list[4] = a5;
	arg_list[5] = a6;
	arg_list[6] = a7;
	arg_list[7] = a8;
	arg_list[8] = a9;
	arg_list[9] = a10;
	arg_list[10] = a11;
	arg_list[11] = a12;
	for ( i = 0 ; i <= (sizeof(arg_list)/sizeof(char *)) &&
			arg_list[i] != (char *) NULL ; ++i )
	{
		if ( arg_list[i][0] == '-' && arg_list[i][1] == 'X' )
		{ /* shared DBMS session */
			sharedSession = TRUE;
			check1st = FALSE;
			break;
		}
	}
	/* if shared session restore from GCA */
	read_globals = ( !sharedSession
			 || IIUIssdStoreSessionData(db_name, FALSE,
						    TRUE, &appl) != OK );
	if ( sharedSession && !read_globals && !appl )
		autocommit = TRUE;	/* all FEs run with auto-commit on */
	else if ( check1st || ( sharedSession && ( read_globals || appl ) ) )
	{ /* asked to check or user application calling a Front-End */
  char buf[2];
/* # line 464 "feingres.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbmsinfo('AUTOCOMMIT_STATE')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,1,buf);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 466 "feingres.sc" */	/* host code */
		autocommit = buf[0] - '0';
	}
	if ( read_globals )
	{ /* Must retrieve from DBMS */
		/* ... and it's not an application */
		if ((rval = IIUIssdStoreSessionData(db_name, FALSE,
						    FALSE, &appl)) != OK )
		{
			FEing_exit();
			return rval;
		}
	}
	/*
	** A transaction may have been active on entry, or the SELECT
	** may have started one (if AUTOCOMMIT is off.)  In any case,
	** commit a transaction (which is a no-op if one isn't active.)
	*/
/* # line 483 "feingres.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 484 "feingres.sc" */	/* host code */
	/*
	** set autocommit flag on if this is the first DB connection
	** (because we made an SQL connection, which turns autocommit
	** off) or if this is a child process and autocommit was turned
	** off in the calling process.
	*/
	if ( !sharedSession || !autocommit )
		IIUIautocommit(UI_AC_ON);
        { 
  char mybuf[10];
/* # line 496 "feingres.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbmsinfo('UNICODE_NORMALIZATION')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,9,mybuf);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 497 "feingres.sc" */	/* host code */
	  if (STlength(mybuf) >= 3)
	  {
  	    if (STbcompare( mybuf, 3, "NFC",3,0) == 0)
     	    { 
	      IILQucolinit(1);
  	    }
  	    if (STbcompare( mybuf, 3, "NFD",3,0) == 0)
  	    {
    	      IILQucolinit(0);
  	    }
	  }
        }
	/* Check FE Dictionary compatibility */
	if ( (dictClientNm != NULL) )
	{
	    /* NULL dictClientNm means bypass the dictionary check;
	    ** for example, terminal monitor or Accessdb may
	    ** not want a FE catalog check to take place.
	    */
	    if ((dval = IIUICheckDictVer(dictClientNm,
					 dictClientVrs)) == OK)
	    {
		    /* Ok to start this FE; dictionary is compatible */
	    }
	    else
	    {
		    /* FE cannot start. Check exact reason (in dval)
		    ** why can't start and prompt user or issue
		    ** message as appropriate.
		    */
		    if (checkdictmsg(dval, db_name,
				dictClientNm, dictClientVrs) != OK)
		    {
			/* Frontend cannot start. either catalogs
			** cannot be upgraded, or they can be
			** upgraded, but user doesn't want to
			** upgrade them.
			*/
			FEing_exit();
			return FAIL;
		    }
		    /* Assertion: it's ok to start this frontend.
		    ** checkdictmsg successfully patched up the
		    ** INGRES catalogs.
		    */
	    }
	}
	/*
	** firstAutoCom is only set if the user has called IIUIck1stAutoCom,
	** and that is only true if check1st is TRUE.
	** If it is we have to call IIUIsetData since we just changed something
	** that is stored in the shared data.
	*/
	if (check1st)
	{
	    IIUIdbdata()->firstAutoCom = autocommit;
	    IIUIsetData(FALSE);
	}
	/*
	** In any case, we need to store the autocommit state that
	** was true on entry so that we can reset it when FEing_exit is
	** called.
	*/
	IIUIdbdata()->entryAC = autocommit;
	IIUIdbdata()->sharedSession = sharedSession;
# ifdef DGC_AOS
	IIUIdbdata()->ROstatus = RO_Status;
# endif /* DGC_AOS */
	return rval;
}
# else /* DBI */
/*
**==================== DBI version of FEningres() ============================
** Notes::
**	This leaves a new hdbc current, regardless of whether the connection
**	succeeds or fails.  That is necessary so that subsequent INQUIRE's
**	(as in setdbstate) will have a place to obtain connection status info.
**	Of course, this means that the caller is responsible for freeing the
** 	new hdbc.
**
**	This routine returns immediately on any error.  This means the new
**	hdbc may or may not be in a connected state when this routine returns
**	a non-OK status.  But since the caller has to free the hdbc anyway,
**	that will automatically clean up any possible dangling connections,
**	and so there would be no benefit to doing extra cleanup work here.
*/
{
	register i4	i;
	bool		appl;
	char		*arg_list[12];
	IIQL_HDBC	hdbc;
	RETCODE		qlstat;
	/*
	** Find out how many arguments have been passed, so we can inform
	** QL.  (The first null pointer is considered to terminate the whole 
	** list.)
	*/
	arg_list[0] = a1;
	arg_list[1] = a2;
	arg_list[2] = a3;
	arg_list[3] = a4;
	arg_list[4] = a5;
	arg_list[5] = a6;
	arg_list[6] = a7;
	arg_list[7] = a8;
	arg_list[8] = a9;
	arg_list[9] = a10;
	arg_list[10] = a11;
	arg_list[11] = a12;
	for ( i = 0 ; i < (sizeof(arg_list)/sizeof(char *)) ; ++i )
	{
		if (arg_list[i] == NULL)
			break;
	}
	/* i now contains the number of arguments. */
	/*
	** Allocate a new current conection.
	*/
	if (IIQPcdaCurDbcAlloc((i4)UI_SESSION) != IIQL_STAT_SUCCESS)
	    return (FAIL);
	/*
	** Try to connect.
	*/
        if (IIQPcdcCurDbcConnect(db_name, i, arg_list) != IIQL_STAT_SUCCESS)
	    return (FAIL);
	/*
	** Populate IIUIdbdata structure for this session.
	*/
	if (IIUIssdStoreSessionData(db_name, FALSE, FALSE, &appl) != OK)
	    return (FAIL);
	/*
	** Establish expected FE autocommit state.
	*/
/* # line 630 "feingres.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 631 "feingres.sc" */	/* host code */
	if (FEinqerr() != OK)
	    return (FAIL);
	if (IIUIautocommit(UI_AC_ON) != OK)
	    return (FAIL);
	/*
	** DBI_FIXME:  For the first release we aren't going to worry
	** about dictionary upgrades.  For now we are just checking
	** for existence and will not attempt to run an upgradefe (which
	** does not yet work on DBI).
	*/
	/*
	** Check FE Dictionary compatibility.  
	** (NULL dictClientNm means bypass the dictionary check.)
	*/
	if ( (dictClientNm != NULL) )
	{
	    if (IIUICheckDictVer(dictClientNm, dictClientVrs) != OK)
	    {
		/*
		** Dictionary check failed.  Blast a message to the trace 
		** window (just for overkill), and also register it with QL 
		** as a pseudo connection error (so the caller can inquire 
		** on the cause).
		*/
		IIUGerr(E_UI003F_DBI_INSTALL, UG_ERR_ERROR, (i4) 0, NULL, 
			NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		/*
		**  Get current connection handle so that error can be
		**  registered.
		*/
		hdbc = IIQPgcdGetCurrentDbc();
		(void) IIQLeqeErrQlibError((IIQL_HGENERIC)hdbc, IIQL_HTYPE_DBC,
				GE_CONFIG_ERROR, E_UI0040_BadCatalaogs, 
					IIQL_ERR_ERROR, 1, &db_name);
		return (FAIL);
	    }
	}
	FEset_exit(FEing_cleanup);
	return (OK);
}
/*
**==================== END DBI version of FEningres() ========================
*/
# endif /* DBI */
/**
** Name:	checkdictmsg - issue incompatible dictionary message.
**
** Description:
**	Check why front-end can't start. If catalogs are upgradeable, then
**	prompt user if they want to upgrade; if they don't, or if catalogs
**	aren't upgradeable issue message & return FAIL.
**
** Inputs:
**	STATUS dval;		return code from CheckDictVer.
**	char   *db_name;	name of database.
**	char   *dictClientNm;	name of front-end Client.
**	i4 dictClientVrs;	Client version checked.
**
** Returns:
**	OK if ok to start front-end (catalogs are ok now); FAIL otherwise.
**
** History:
**	22-may-1990 (pete)	Initial version.
**	27-aug-1990 (rdesmond)	Added arg dictClientVrs to upgradedict() call.
**	1-oct-1990 (pete)	Added new argument to E_UI0029_UPGRADE_DBA &
**		E_UI002A_UPGRADE_NOTDBA.
**	1-nov-1990 (pete)
**		Added new error message for DictCheck status UI_NO_CLIENT.
*/
static STATUS
checkdictmsg(dval, db_name, dictClientNm, dictClientVrs)
STATUS dval;
char   *db_name;
char   *dictClientNm;
i4 dictClientVrs;
{
	bool isdba;
	STATUS stat = FAIL;
	isdba = STequal(IIuiUser, IIuiDBA);
	if (dval == UI_INSTALLABLE)
	{
	    /* FE can't run without creation of new catalogs.
	    ** Upgrade can only be done by DBA.
	    ** If user is DBA, ask if want to do the upgrade.
	    */
	    if (isdba)
	    {
	        if (checkif_upgrade(db_name, dictClientNm))
		{
		    /* user wants to upgrade. */
		    if (upgradedict(dictClientNm, dictClientVrs, db_name) !=OK)
		    {
			/* error happened during upgrade */
		    }
		    else
			stat = OK;	/* successful upgrade. FE can start */
		}
	        else
	            ; /* User doesn't want to upgrade. FE should not start up */
	    }
	    else
	    {
		/* this user can't do the upgrade. != DBA */
		IIUGerr(E_UI0028_INSTALL_NOTDBA, UG_ERR_ERROR, (i4) 3,
			(PTR) IIuiDBA, (PTR) db_name, (PTR) dictClientNm, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	    }
	}
	else if (dval == UI_NEEDS_UPGRADE)
	{
	    /* database must be upgraded. can't do that here */
	    if (isdba)
	    {
		/* this user can do the upgrade. == DBA */
		IIUGerr(E_UI0029_UPGRADE_DBA, UG_ERR_ERROR, (i4) 2,
			(PTR) db_name, (PTR) dictClientNm, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	    }
	    else
	    {
		/* this user can't do the upgrade. != DBA */
		IIUGerr(E_UI002A_UPGRADE_NOTDBA, UG_ERR_ERROR, (i4) 3,
			(PTR) IIuiDBA, (PTR) db_name, (PTR) dictClientNm, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	    }
	}
	else if ((dval == UI_UNINSTALLABLE) || (dval == UI_DESUPPORTED))
	{
	    /* incompatible catalogs; cannot be upgraded */
	    IIUGerr(E_UI002B_CANNOT_UPGRADE, UG_ERR_ERROR, (i4) 1,
			(PTR) db_name, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	}
	else if (dval == UI_NO_CLIENT)
	{
	    /* "enter_client" couldn't find the requested client in
	    ** the client table.
	    */
	    IIUGerr(E_UI0036_NO_SUCH_CLIENT, UG_ERR_ERROR, (i4) 2,
			(PTR) dictClientNm, (PTR) &dictClientVrs, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	}
	else
	{
	    /* other error; unrecognized return code from CheckDict */
	    IIUGerr(E_UI002C_CHECKDICT_ERR, UG_ERR_ERROR, (i4) 4,
			(PTR) db_name, (PTR) &dval, (PTR) dictClientNm,
			(PTR) &dictClientVrs, NULL, NULL, NULL, NULL, NULL, NULL);
	}
	return stat;
}
/**
** Name:	checkif_upgrade - check if user wants to upgrade database
**
** Description:
**	Ask the user if they want to upgrade the database and check
**	their response. Ask them with a simple prompt. See below for
**	discussion of future enhancement to ask users in forms mode
**	with a ConfirmChoice popup.
**
**	Note about prompting users when FRS is up:
**	Unfortunately, 'uf' routines can't be called from anywhere
**	except a product library. Even iiugfrs_setting() can't
**	help, because UFLIB is above RUNTIMELIB (iiugfrs_setting is
**	called from iiforms.c, which is in RUNTIMELIB). If a solution is
**	ever found for this, the following popup is a much nicer
**	looking way to prompt the user about the upgrade (but note that
**	IIUFccConfirmChoice would have to be added to iiugfrs_setting
**	and called here thru a function pointer). These S_UI messages exist.
**	    if (IIUFccConfirmChoice(CONF_GENERIC,
**		ERx(""), ERx(""), ERx(""), ERx(""),
**		S_UI0023_UpgradeTitle,
**		S_UI0024_YesExpl,
**		S_UI0025_NoExpl,
**		ERx(""), TRUE) == CONFCH_YES)
**
** Inputs:
**	char   *db_name;	name of database.
**	char   *dictClientNm;	name of front-end Client.
**
** Returns:
**	TRUE if user wants to upgrade; FALSE otherwise.
**
** History:
**	3-may-1990 (pete)	Initial version.
**	26-dec-1990 (pete)
**		Fix bug 35060; don't prompt user if either stdin or stdout !=
**		terminal. Had problems with non-interactive process
**		(like 4gl interpreter) trying to prompt user.
**	08-aug-94 (leighb)
**		Don't try to run UPGRADEF.EXE at same time as W4GL on Win32s.
*/
static bool
checkif_upgrade(db_name, dictClientNm)
char   *db_name;
char   *dictClientNm;
{
	char response[FE_MAXNAME+1];
	STATUS stat;
	if (!SIterminal(stdin) || !SIterminal(stdout))
	{
	    /* Either stdin or stdout isn't a terminal. don't prompt
	    ** for input.
	    */
	    IIUGerr(E_UI0029_UPGRADE_DBA, UG_ERR_ERROR, (i4) 2,
		(PTR) db_name, (PTR) dictClientNm, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	    return FALSE;
	}
	/* callback function is set to NULL when LIBQ starts & to "II_fend"
	** when FRS starts.
	*/
# ifndef DBI
	if (IILQgdata()->f_end != NULL)
	    _VOID_ (*IILQgdata()->f_end)( TRUE );	/* turn off forms mode*/
# endif /* DBI */
	_VOID_ SIfprintf (stdout, ERget(S_UI0026_UpgradeTitle2));
	IIUGprompt (ERget(S_UI0027_UpgradePrompt), 0,
		FALSE, response, FE_MAXNAME, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
# ifndef DBI
	if (IILQgdata()->f_end != NULL)
	    _VOID_ (*IILQgdata()->f_end)( FALSE );	/* turn on forms mode */
# endif /* DBI */
	if (IIUGyn(response, &stat))
	{
		return TRUE;
	}
	return FALSE;
}
/**
** Name:	upgradedict - upgrade front-end catalogs for new product.
**
** Description:
**	Call the "upgradefe" executable to add any missing catalogs
**	needed by the current client (e.g. "INGRES", "WINDOWS_4GL", etc).
**	The user may have just purchased a new product and recieved a new
**	authorization string.
**
**	Pass upgradefe the "noupgrade" flag so it will add new catalogs
**	only, and not attempt to upgrade previously existing versions
**	of catalogs (don't want it to do that cause can take a very
**	long time).
**
** Inputs:
**	cname		name of the client.
**	cversion	version of the client.
**	db_name		name of the database.
**
** Returns:
**	OK if upgrade went ok, FAIL otherwise.
**
** History:
**	2-may-1990 (pete)	Initial version.
**	27-aug-1990 (rdesmond)	Added parameter cversion.
**	26-dec-1990 (pete)	Fixed bug 35102 so "noupgrade" (-c) parameter
**				is passed to upgradefe.
**	09/10/93 (valerier)     Fixed number of UTexe arguments for WindowsNT.
**	28-mar-94 (leighb)	Fixed number of UTexe args for WindowsNT again.
*/
static STATUS
upgradedict(cname, cversion, db_name)
char *cname;
i4   cversion;
char *db_name;
{
	STATUS status;
	CL_ERR_DESC err_code;
	char	buf[256];
	FEmsg (ERget(S_UI0018_UpgradingDict), FALSE,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	/* callback function is set to NULL when LIBQ starts & to "II_fend"
	** when FRS starts.
	*/
# ifndef DBI
	if (IILQgdata()->f_end != NULL)
	    _VOID_ (*IILQgdata()->f_end)( TRUE );       /* turn off forms mode*/
# endif /* DBI */
	STcopy ( ERx
       ("database = %S, client = %S, version = %N, equel = %S, noupgrade = %S"),
	  buf);
	status = UTexe(UT_WAIT|UT_STRICT, NULL,
		(ModDictDesc *) NULL,
		NULL, G_upgradefe, &err_code, buf,
		5, db_name, cname, cversion, IIxflag(), ERx(""),
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
# ifndef DBI
	if (IILQgdata()->f_end != NULL)
	    _VOID_ (*IILQgdata()->f_end)( FALSE );	/* turn on forms mode */
# endif /* DBI */
        if ((status != OK) && (status != FAIL))
        {
		/* print message if error other than FAIL */
		IIUGerr(E_UI0017_DictInstallErr, UG_ERR_FATAL, 3, (PTR) cname,
			(PTR) &status, (PTR) G_upgradefe, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		/*NOT REACHED*/
        }
	else if (status != OK)
	{
		return FAIL;
	}
	return OK;
}
/*{
** Name:	FEing_exit() -	Front-End DBMS Disconnect Utility.
**
** Description:
**	Terminates a connection with the DBMS (INGRES) on a database.
**
** Side Effects:
**	Clears 'IIUIgdata-> 'user', 'dba', 'database', and the exit clean-up
**	routine.
**
** History:
**	11/89 (jhw) -- Set session for disconnect for multiple-session support.
**	30-jan-94 (sandyd)
**		Ifdef'ed for DBI.  (Our DBI preprocessor doesn't handle
**		SET SESSION statements.)
*/
VOID
FEing_exit ()
{
	FEclr_exit(FEing_cleanup);
# ifdef DBI
	IIQPscdSetCurrentDbc(IIQPgshGetSessionHandle((i4)UI_SESSION));
# else /* not DBI */
/* # line 951 "feingres.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(UI_SESSION),(void *)IILQint(
    UI_SESSION));
  }
/* # line 952 "feingres.sc" */	/* host code */
# endif /* DBI */
	dbms_exit();
}
/*
** Name:	dbms_exit() -	DBMS Disconnect.
**
** Description:
**	Disconnects from the DBMS.
**
** Side Effects:
**	Clears 'IIUIgdata-> 'user', 'dba' and 'database'.
**
** History:
**	02/88 (jhw) -- Written.
**	21-oct-1988 (danielt)
**		changed to use new autocommit routine, also to
**		simplify the logic of when to turn on or off autocommit
**		(if the FE process was called using the -X flag, simply
**		reset autocommit state to the same as it was called with).
**	11/89 (jhw) -- Modified to disconnect from session UI_SESSION for
**			multiple sessions.
**	30-jan-94 (sandyd)
**		Ifdef'ed for DBI.  (Our DBI preprocessor doesn't handle
**		CONNECT/DISCONNECT statements.)
*/
static VOID
dbms_exit ()
{
# ifdef DBI
	(void) IIQPcddCurDbcDisconnect();
	(void) IIQPcdfCurDbcFree();
# else /* not DBI */
	if ( IIUIdbdata()->sharedSession )
	{ /* reset autocommit */
		IIUIautocommit(IIUIdbdata()->entryAC ? UI_AC_ON:UI_AC_OFF);
	}
/* # line 988 "feingres.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(UI_SESSION);
    IIsqDisconnect();
  }
/* # line 989 "feingres.sc" */	/* host code */
# endif /* DBI */
}
/*{
** Name:	IIUIck1stAutoCom() -	Check Initial AutoCommit State.
**
** Description:
**	Sets a flag that causes 'FEingres()' to check the initial auto-commit
**	state on first time start-up of a DBMS session and causes
**	IIUIdbdata()->firstAutoCom to be set to whether autocommit was
**	on or off after that first connection..
**
**	NOTE:  Obviously this should be called before 'FEingres()'.
**
**	This routine is used by ingmenu and the terminal monitor.  The
**	purpose is to cause firstAutoCom to be set correctly.  If
**	firstAutoCom is TRUE for a DB session, it means that the session
**	was originally started (perhaps in the parent process if for this
**	process the DB session is a shared session - i.e. -X flag was given)
**	by FEingres, IIUIck1stAutoCom had been called before that FEingres
**	call, and the connection that FEingres established had autocommit on.
**	The only way this can happen (as far as I can figure out) is if
**	the user had set ING_SET to "set autocommit on".  In any case,
**	the sql terminal monitor uses the value of firstAutoCom to determine
**	whether to set autocommit off or to leave it alone.
**
** History:
**	03/89 (jhw) -- Written.
**	17-aug-1990 (Joe)
**	    Changed to a noop, since this isn't used.
**	28-sep-1990 (Joe)
**	    Well, I was wrong.  It is used by ingmenu and the terminal
**	    monitor.  Restored to what it did before.
*/
VOID
IIUIck1stAutoCom ( )
{
    check1st = TRUE;
}
/*{
** Name:	IIUIswc_SetWithClause() -	Set up WITH clause for connect.
**
** Description:
**	Saves a pointer that points to a frontend supplied WITH clause to be
**	used when 'FEingres()' is called for connection to a database.
**
** History:
**	12/89 (kenl) -- Written.
*/
VOID
IIUIswc_SetWithClause(WithStr)
char	*WithStr;
{
	WithClause = WithStr;
# ifdef DGC_AOS
	/*
	** Until INGRES supports a readonly mode, DG will use the following
	** code to check if the WITH clause indicates to open the database
	** for reading only.  We will key off of the word 'reading' in all
	** cases and assume that it was part of a 'mode=' option.  The 
	** static variable RO_Status will then be checked everytime the
	** routine IIcro_CheckReadOnly() is called.
	*/
	{
		char	*x = WithStr;
		while ((x = STindex(x, ERx("r"), 0)) != (char *)NULL)
		{
			if (STbcompare(x, 7, ERx("reading"), 7, TRUE) == 0)
			{
				RO_Status = TRUE;
				break;
			}
			x++;
		}
	}
# endif /* DGC_AOS */
}
# ifdef DGC_AOS
/*{
** Name:	IIUIcro_CheckReadOnly() -	Return value of RO_Status
**
** Description:
**	Only used by Data General.  This routine is typically called right
**	after certain menuitems are selected and before executing any code
**	for that menuitem.  The menuitems of interest all relate to writing
**	something to the database (ex. FE_Save, FE_Rename, etc.).  If this
**	routine returns TRUE, then an  appropriate error message is
**	displayed (regarding readonly access to the database) and, in most
**	cases, a ## RESUME is executed.
**
**	Example of calling program:
**	    ## activate menuitem ERget(FE_Destroy)
**	    ## {
**	    # ifdef DGC_AOS
**		/*
**		** Until INGRES supports a readonly mode, DG will use the
**		** following code to avoid ugly error messages that occur
**		** when trying to write to the database when we are only
**		** connected for readonly access.
**		*/
**		if (IIUIcro_CheckReadOnly())
**		{
**			IIUGerr(E_UI0016_ReadOnly_err, 0, 0);
**	    ##		resume
**		}
**	    # endif
**
** History:
**	1/90 (kenl) -- Written.
**	28-aug-1990 (Joe)
**	    Changed to keep the readonly status in UIDBDATA structure.
*/
bool
IIUIcro_CheckReadOnly()
{
	return (IIUIdbdata()->ROstatus);
}
# endif	/* DGC_AOS */
static PTR 	htab_ctx;			/* identifies a hash table */
static char	*concat_all3();
/*{
** Name:    IIUISQLconnect()
**
** Description:
**  Starts a connection to the DBMS (INGRES).  If II_DICTIONARY_OK
**	is defined this function will not check FE dictionaries.
**
** Input:
**  db_name {char *}  The database name on which to start-up the DBMS.
**  a[1-12] {char *}  additional parameters to be passed to the DBMS
**              
** Returns:
**  	return value from FEinqerr, or OK or FAIL
**
** History:
**	25-mar-94 (milovan)
**		Created.
**	26-apr-94 (sandyd)
**		Started history section.  Added DBI flavor of this routine.
**	07-may-94 (sandyd)
**		Straightening out DBI connection model --- to permit handling
**		of connection errors.  Must leave new hdbc current, even on
**		failure, so that caller's INQUIRE's (as in setdbstate) will 
**		have a relevant hdbc to look at.  NOTE that this means the 
**		*caller* is responsible for freeing the hdbc, regardless of 
**		whether the connection succeeded.
**	13-jun-95 (fraser)
**		DBI change: get connection handle before registering error.
*/
STATUS
IIUISQLconnect(db_name, session_id, client_name, client_version,
			 a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)
  char *db_name;
  i4 session_id;
  char *client_name;
  i4 client_version;
  char *a1;
  char *a2;
  char *a3;
  char *a4;
  char *a5;
  char *a6;
  char *a7;
  char *a8;
  char *a9;
  char *a10;
  char *a11;
  char *a12;
# ifndef DBI
{
  char buf[2];
  i4 oldsession;
  i4 errorno;
    i4		autocommit = FALSE, htab_enter = FALSE;
	char	*check_dict = NULL;
    STATUS	rval = OK;
    STATUS	dval;
/* # line 1169 "feingres.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&oldsession,11);
  }
/* # line 1170 "feingres.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(session_id);
    IIsqConnect(0,db_name,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,(char *)0);
  }
/* # line 1173 "feingres.sc" */	/* host code */
    if ((rval = FEinqerr()) != OK)
        return rval;
    /* 
	** Are we required to check FE dictionary compatibility?
	*/
	NMgtAt(ERx("II_DICTIONARY_OK"), &check_dict);
 	if (check_dict != NULL && *check_dict != EOS)
		return OK;
	/*
	** Have we already checked them?
	*/
	if (iiuiDict_checked(db_name, client_name, client_version))
	{
		return OK;
	}
	/*
	**  Get and save the current autocommit state, in order to
	**  restore it after the dictionaries have been checked.
	*/
/* # line 1192 "feingres.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbmsinfo('AUTOCOMMIT_STATE')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,1,buf);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1193 "feingres.sc" */	/* host code */
	autocommit = buf[0] - '0';
	/*
	**  Commit the work so we can set autocommit ON
	*/
/* # line 1197 "feingres.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 1198 "feingres.sc" */	/* host code */
    /*
    ** set autocommit ON if it is not already ON
    */
	if (!autocommit)
/* # line 1202 "feingres.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set AUTOCOMMIT on");
    IIsyncup((char *)0,0);
  }
/* # line 1203 "feingres.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbmsinfo('UNICODE_NORMALIZATION')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,1,buf);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1204 "feingres.sc" */	/* host code */
    if (STlength(buf) >= 3)
    {
      if (STbcompare( buf, 3, "NFC",3,0) == 0)
      {
        IILQucolinit(1);
      }
      if (STbcompare( buf, 3, "NFD",3,0) == 0)
      {
        IILQucolinit(0);
      }
    }
    if ((dval = IIUICheckDictVer(client_name, client_version)) != OK)
    {
#ifdef wgl_win   
        /*
        ** Do not execute checkdictmsg() on Windows3.1
        ** since it would start a new program...
        */	
        IIUGerr(E_UI0029_UPGRADE_DBA, UG_ERR_ERROR, (i4) 2,
           		(PTR) db_name, (PTR) client_name, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        rval = FAIL;
#else 
        /* 
		** FE cannot start before the catalogs are upgraded/installed. 
		** Check exact reason (in dval) why can't start and prompt user 
		** or issue a message as appropriate.
        */
        if (checkdictmsg(dval, db_name, client_name, client_version) != OK)
        {
            /* 
			** Frontend cannot start. Either catalogs can't be upgraded, or
			** they can be upgraded, but user doesn't want to upgrade them.
            */
            rval = FAIL;
        }
        /* 
		** If checkdictmsg() returns OK then the frontend may be started.
        ** since checkdictmsg() successfully patched up the INGRES catalogs.
        */
#endif
    }
	if (rval == OK)
	{
		char *word = concat_all3(db_name, client_name, client_version);
		/*
		** Add a new entry to the hash table
		*/
		IIUGheHtabEnter(htab_ctx, word, (PTR)word);
		/*
		**  If autocommit was OFF (= FALSE), we reset it, otherwise it stays ON.
		*/
		if (!autocommit)
/* # line 1256 "feingres.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set AUTOCOMMIT OFF");
    IIsyncup((char *)0,0);
  }
/* # line 1257 "feingres.sc" */	/* host code */
	}
	else
	{
/* # line 1260 "feingres.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(session_id);
    IIsqDisconnect();
  }
/* # line 1261 "feingres.sc" */	/* host code */
		if (oldsession != 0)
		{
			/* Restore old session */
/* # line 1264 "feingres.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&oldsession);
  }
/* # line 1265 "feingres.sc" */	/* host code */
		}
		IIfakeerr(DB_SQL, GE_CONFIG_ERROR, E_UI0040_BadCatalaogs,
			  UG_ERR_ERROR, 1, db_name, (PTR)NULL,
			  (PTR)NULL, (PTR)NULL, (PTR)NULL);
	}
	return rval;
}
# else /* DBI */
/*
**==================== DBI version of IIUISQLconnect() =======================
** Notes::
**	This leaves a new hdbc current, regardless of whether the connection
**	succeeds or fails.  That is necessary so that subsequent INQUIRE's
**	(as in setdbstate) will have a place to obtain connection status info.
**	Of course, this means that the caller is responsible for freeing the
** 	new hdbc.
**
**	This routine returns immediately on any error.  This means the new
**	hdbc may or may not be in a connected state when this routine returns
**	a non-OK status.  But since the caller has to free the hdbc anyway,
**	that will automatically clean up any possible dangling connections,
**	and so there would be no benefit to doing extra cleanup work here.
*/
{
    register i4	i;
    char		*arg_list[12];
    IIQL_HDBC		hdbc;
    char		*check_dict = NULL;
    bool		no_catalog_check = FALSE;
    i4			no_catalog_check_index;
    STATUS		rstat;
    /*
    ** Find out how many arguments have been passed, so we can inform
    ** QL.  (The first null pointer is considered to terminate the whole 
    ** list.)  Also, look for the "-no_catalog_check" option.
    */
    arg_list[0] = a1;
    arg_list[1] = a2;
    arg_list[2] = a3;
    arg_list[3] = a4;
    arg_list[4] = a5;
    arg_list[5] = a6;
    arg_list[6] = a7;
    arg_list[7] = a8;
    arg_list[8] = a9;
    arg_list[9] = a10;
    arg_list[10] = a11;
    arg_list[11] = a12;
    for ( i = 0 ; i < (sizeof(arg_list)/sizeof(char *)) ; ++i )
    {
	if (arg_list[i] == NULL)
	{
	    break;
	}
	else
	{
	    if (STequal(arg_list[i], "-no_catalog_check"))
	    {
		no_catalog_check = TRUE;
		no_catalog_check_index = i;
	    }
	}
    }
    /*
    ** i now contains the number of arguments.  If "-no_catalog_check"
    ** was found, strip it out of the argument list so that we don't pass
    ** it down to QP/QL.
    */
    if (no_catalog_check)
    {
	--i;
	if (i > 0)
	{
	    arg_list[no_catalog_check_index] = arg_list[i];
	}
    }
    /*
    ** Allocate a new current connection.
    */
    if (IIQPcdaCurDbcAlloc(session_id) != IIQL_STAT_SUCCESS)
    {
	return (FAIL);
    }
    /*
    ** Try to connect.
    */
    if (IIQPcdcCurDbcConnect(db_name, i, arg_list) != IIQL_STAT_SUCCESS)
    {
	return (FAIL);
    }
    /* 
    ** Are we required to check FE dictionary compatibility?
    */
    NMgtAt(ERx("II_DICTIONARY_OK"), &check_dict);
    if (no_catalog_check || (check_dict != NULL && *check_dict != EOS))
    {
	return (OK);
    }
    /*
    ** Have we already checked them?
    */
    if (iiuiDict_checked(db_name, client_name, client_version))
    {
	return (OK);
    }
    /*
    ** CHECK FOR CATALOGS
    **
    ** I really don't believe there is any need to fiddle with the 
    ** autocommit state in the DBI case.  (We are not spawning upgradefe 
    ** or anything...)  By default, SQL Server and ORACLE don't hold read 
    ** locks, so setting autocommit probably won't even give us more 
    ** concurrency on the catalog queries.
    **
    ** DBI_FIXME: This may need to be reconsidered when we go to INGRES.
    **
    */
    if ((rstat = IIUICheckDictVer(client_name, client_version)) != OK)
    {
    	/*
	** The catalog check failed.  Blast a message to the trace window 
	** (just for overkill), and also register it with QL as a pseudo
	** connection error (so the caller can inquire on the actual cause).
	*/
        IIUGerr(E_UI003F_DBI_INSTALL, UG_ERR_ERROR, (i4) 0, NULL, NULL,
			    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	/*
	**  Get the current connection handle so that the error can be
	**  registered.
	*/
	hdbc = IIQPgcdGetCurrentDbc();
	IIQLeqeErrQlibError((IIQL_HGENERIC)hdbc, IIQL_HTYPE_DBC,
				GE_CONFIG_ERROR, E_UI0040_BadCatalaogs, 
					IIQL_ERR_ERROR, 1, &db_name);
    }
    else
    {
	char *word = concat_all3(db_name, client_name, client_version);
	/*
	** Add a new entry to the hash table
	*/
	IIUGheHtabEnter(htab_ctx, word, (PTR)word);
/* # line 1407 "feingres.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 1408 "feingres.sc" */	/* host code */
	rstat = FEinqerr();
    }
    return (rstat);
}
/*
**==================== END DBI version of IIUISQLconnect() ===================
*/
# endif /* DBI */
#define MAX_WORD_SIZE 256
/*
**  Once the hash table is full the function will return FALSE
**  for all the subsequent calls, meaning that dictionaries will
**  be checked every time IIUISQLconnect is executed for any new
**  dbname-client_name-version which is not in the table.
**
*/
static i4
iiuiDict_checked(dbname, cname, cversion)
char 	*dbname;
char 	*cname;
i4		cversion;
{
	char  	*word;
	PTR		htdata;
	static  first = 1;
	if (first)
	{
		IIUGhiHtabInit(100, NULL, NULL, NULL, &htab_ctx);
		first = 0;
	}
	word = concat_all3(dbname, cname, cversion);
	if (IIUGhfHtabFind(htab_ctx, word, &htdata) != OK)
		return FALSE;
	return TRUE;
}
static char *
concat_all3(dbname, cname, cversion)
char    *dbname;
char    *cname;
i4      cversion;
{
	char    *word, vers[MAX_WORD_SIZE];
	word = (char *)MEreqmem(0,(i4) sizeof(char)*MAX_WORD_SIZE, FALSE, (STATUS *)NULL);
    word[0] = EOS;
    vers[0] = EOS;
    CVna(cversion, vers);
    if (dbname)
        STcat(word, dbname);
    if (cname)
        STcat(word, cname);
    STcat(word, vers);
	return word;
}
