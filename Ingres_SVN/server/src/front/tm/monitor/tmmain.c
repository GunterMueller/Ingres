# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
**
NEEDLIBS =  MONITORLIB QRLIB SHFRAMELIB SHQLIB \
	    SHCOMPATLIB 
NEEDLIBSW = SHEMBEDLIB SHADFLIB 
UNDEFS =	II_copyright
**
OWNER = INGUSER
**
PROGRAM = (PROG0PRFX)tm
**
*/
# include	<compat.h>
# include	<cv.h>		/* 6-x_PC_80x86 */
# include	<pc.h>		/* 6-x_PC_80x86 */
# include	<ci.h>
# include	<ex.h>
# include	<nm.h>
# include	<me.h>
# include	<st.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ug.h>
# include <ui.h>
# include	"monitor.h"
# include	"ermo.h"
# include	<tm.h>
# include	<adf.h>
# include	<itline.h>
# include	<qr.h>
# include	<uigdata.h>
#ifdef SEP
# include <tc.h>
GLOBALDEF	TCFILE *IITMincomm = NULL;	/* input COMM-file */
GLOBALDEF	TCFILE *IITMoutcomm = NULL;	/* output COMM-file */
#endif /* SEP */
#ifdef	PMFE				 
#define	WANT_SQL	1		 
#endif					 
# define MAX_ING_OPTS   13
GLOBALREF	bool 	Runtime_INGRES;
/**
** Name:	tmmain.sc
**
**  INTERACTIVE TERMINAL MONITOR
**
**	The monitor gathers text from the standard input and performs
**	a variety of rudimentary editting functions.  This program
**	is the main setup.  Monitor() is then called, which does the
**	real work.
**
**	variables:
**	Nodayfile -- zero prints all messages; positive one suppresses
**		dayfile and logout but not prompts; negative one
**		suppresses all printed material except results from \p.
**	Newline -- set when the last character in the query buffer
**		is a newline.
**	P_rompt -- set when a prompt character is needed.
**	Autoclear -- set when the query buffer should be cleared before
**		putting another character in.
**	Nautoclear -- if set, suppresses the autoclear function
**		entirely.
**	PartialAccess -- if set, allows access to a partially created (or
**			 converted) DB by not accessing any frontend 
**			 catalogs.
**	HistoryRecall -- if set, allows command history recall feature enabled.
**
**	flags:
**	-M 	 -- trace flag
**	-partial -- access a partially created DB. For internal use only. 
**	-history_recall -- allows command history recall feature enabled.
**	-d 	 -- suppress dayfile
**	-s 	 -- suppress prompt (sets -d)
**	-a 	 -- disable autoclear function
**
**	The last three options can be set by stating "+x".
**
**	Uses trace flag 9
**	Proc_error uses trace flag 11
**
**	History:
**
**	8/17/85 (peb)	Included multinational support
**	85/09/20	 14:50:54  roger
**		Added "syserr" to UNDEFS hint.
**		"#ifdef MTR1" trace vector declaration.
**		Removed useless variable p and assignment to it.
**		Handle startup errors in a consistent manner
**		(reducing exit points helps); always EXdelete()
**		before exit.
**	10/07/85 (ericj) -- Added calls to CIcapability to check that
**		the installation has QUEL terminal monitor authorization
**	10/08/85 (Mike Berrow) -- Changes for multi-channel networking.
**	85/10/11	 11:58:23  roger
**		ibs integration (CMS changes, putprintf(), etc.).
**		Also added TERMDRLIB (ugh!) to NEEDLIBS hint for
**		IT routines.
**	85/11/08  12:44:28  mikem
**		iisndmulti -> IIsndmulti and mkmuffing hints.
**	85.11.21 (bml) -- bug #6382, in which \quit did not work in
**		an \include or startup file.
**		A very hacked in solution in response to an
**		emergency field request.
**	86/01/11  13:18:40  daved
**		handle new network copy pipeblocks.
**	86/01/13	 12:56:01  boba
**		Remove unwanted NO_EX code reintroduced by RPM integration.
**		Check value returned by NMgtAt.
**	86/02/06	16:24:47  berrow
**		Ingres/Net 4.0 modifications
**	86/04/11	 14:50:27  seiwald
**		initialize international character translation
**	86/04/29	17:10:16  roger
**		Deleted history prior to 1985.
**	86/05/15  16:09:52  perform
**		40mps sync point.  Check-in ericj's CI changes.
**	86/06/08  13:25:35  perform
**		avoid unnecessary STcopy-STlength calls
**	86/06/20  15:48:37  perform
**		integrate SATURN's terminal monitor intro message usage
**	86/07/14  19:02:43  perform
**		zap Mon_star_[intro,exit], handle through control string.
**	86/07/23  16:50:14  peterk
**		6.0 baseline from dragon:/40/back/rplus
**	86/10/14  15:01:09  peterk
**		working version 2.
**		eliminated BE include files.
**		eliminated necessary libs to NEEDLIBS hint;
**		change to FE style of command line interpretation,
**		exception handler setup,
**		BE startup, error handling.
**	86/11/06  09:43:35  peterk
**		no longer need BE libs PARSER, ADT, SUPSTUB; no longer
**		need ADT function table so can eliminate call to
**		ll_intrns_set_init(); still need IUTIL for ipanic().
**	86/11/06  12:27:59  peterk
**		4.0 integration changes: change to Mon_intro string to
**		parameterize message w.r.t. Version and Env_ID;  eliminate
**		check for TM_QUEL capability in distribution key when TM
**		not run interactive from a terminal so unloaddb
**		scripts will work.
**	86/11/12  11:48:50  peterk
**		put back check for no database name as per old TM.
**		eliminate #include of <version.h>;
**		eliminate unnecessary global defs cp_tm_init() and cp_init;
**		take ITUIL out of NEEDLIBS list.
**	86/12/29  16:19:08  peterk
**		change over to 6.0 headers;
**		change to 6.0 FE library structure for NEEDLIBS
**	87/03/18  16:41:12  peterk
**		fix NEEDLIBS
**		remove QUEL'isms
**		add fix to shift-out of Graphics mode in exception handler.
**	87/03/31  15:42:29  peterk
**		use FEadfcb() (not FEadfscb()).
**	87/04/10  12:43:16  peterk
**		set Tm_semicolon to TRUE when started up in SQL mode.
**		streamline NEEDLIBS.
**	87/04/10  13:00:44  peterk
**		added #include {compat,dbms,fe}.h
**	87/04/22  11:27:38  peterk
**		change NEEDLIBS
**	07-may-1987	(daver)
**		Removed IIsendlog() and IIsndmulti() calls as the ADF_CB
**		already contains the multinational information.
**	08/25/87 (scl) Changed "startup" to "startup.data" for
**		MVS and added some debugging code
**	25-may-88 (bruceb(
**		Use FEingres() rather than IIdbconnect() for db startup.
**		As result, no longer require retrieval of IIUIdba and
**		IIUIuser (done in FEingres).  getargs() changed to
**		remove the dbname from the new argv; dbname passed to
**		FEingres as separate argument.
**	29-aug-88 (bruceb)
**		Removed the authorization check.  This is because can't just
**		check for QUEL_TM (could be running SQL/on an SQL only system),
**		and can't check for the 'appropriate' language since users
**		unloadding 5.0 SQL databases will have QUEL views (in 5.0 the
**		SQL monitor was a translator into QUEL, so SQL view text was
**		really saved as QUEL text) so neither language is really
**		correct.
**	28-nov-1988	(kathryn)	bug # 2997
**		Recognize the -v flag as terminal monitor argument.
**		-v is vertical separator.
**	28-nov-88 (bruceb)
**		If -v, then set global to be checked in putresults().
**	22-mar-89 (marian) 	bug # 4872
**		Read initialization files for sql the same way they are
**		read for quel.  Look for 'startsql' where 'startup' is 
**		looked for and 'dbname_SQL_INIT' where 'dbname_ING is
**		looked for when sql is used.
**      17-apr-89 (wolf)
**              IBM updates: ifdef's, -qsql used by default for SQL,
**              change "dayfile" to "dayfile.data"
**	10-May-89 (Eduardo)
**		Added support to SEP tool
**	26-may-89 (kathryn) Added check for logical II_TM_ON_ERROR.
**	17-aug-89 (teresal) Fixed II_TM_ON_ERROR so it can be set to
**		'\nocontinue' mode.
**	31-may-89 (wolf)
**		Remove some FT3270 ifndef's to get boxes around results. (IBM
**		porting change 9/30/89 - teresal).
**	02-oct-89 (teresal)
**		Unix porting change - change PROGRAM name from 'ingres' to 'tm'.
**	28-sep-89 (teresal) Added check for Runtime INGRES installation.
**	05-oct-89 (teresal) Added new '-partial' command line flag.
**	12-dec-89 (teresal) Changed NEEDLIBS to include UFLIB for password
**		prompting routines needed for '-P' and '-R' ingres DBMS flags.
**	28-dec-89 (kenl)
**		Added handling of the +c (connection/with clause) flag.
**	01-dec-89 (wolf)
**		Moved NMgtAt calls out of if stmts; it's VOID! (IBM porting
**		change 12/21/89)
**	25-jan-90 (teresal)
**		Modified to comply with standards for calling 
**		LOfroms routine, i.e., made location buffer size MAX_LOC+1.
**	12-Feb-90 (Eduardo)
**		Communicate 'not able to connect message' to SEP
**	14-mar-90 (teresal)
**		Removed UFLIB from NEEDLIBS - it's no longer needed.
**      29-aug-90 (pete)
**              Change to call FEningres, rather than FEingres and to pass
**              a NULL client name so no dictionary check will be done.
**	10-oct-90 (stevet)
**		Added IIUGinit call to initialize character set attribute 
**		table.
**	15-may-1991 (jhw)  Corrected partial session set-up.
**	17-mar-92 (seg)	Exception handlers return STATUS, not EX.
**	24-mar-92 (seg) Must EXdelete first handler before EXdeclare of second.
**	28-sep-92 (fredb) Fix for sep filename error.  MPE/iX cannot have
**			  underbars in names.
**	10/11/92 (dkh) - Removed readonly qualification on references
**			 declaration to Env_ID and Version.  Alpha
**			 cross compiler does not like the qualification
**			 for globalrefs.
**	16-nov-1992 (larrym)
**	    Added CUFLIB to NEEDLIBS.  CUF is the new common utility facility.
**	08-feb-93 (rogerl) - Issue err mess and quit at -R with cmd line passwd
**			(bug#37344)
**	10-feb-93 (rogerl) - Back out '08-feb-93 (rogerl)' above.  This bug
**			is already fixed.
**	9-jun-93 (rogerl) - initialize 'Monincomment1'; used to treat SQL style
**			comments (-- <blah> \n) similar to C style.
**	03/25/94 (dkh) - Added call to EXsetclient().
**	21-mar-94 (donc) - Replace CI Runtime checks with CI_CREATE_TABLE
**      25-oct-94 (sarjo01) - getargs(): for bug 65284, added missing test
**                            for +s command line arg.
**	31-may-95 (harpa06) Bug 68876
**              Since QUEL is currently not supported in STAR, created an error
**              message to be displayed if the user trys to invoke QUEL with
**              a STAR database. The error message will be displayed and then
**              TM will quit.
**      15-jan-1996 (toumi01; from 1.1 axp_osf port)
**              Added kchin's changes (from 6.4) for axp_osf
**              10/15/93 (kchin)
**              Cast 2nd argument to PTR when calling putprintf(), this
**              is due to the change of datatype of variable parameters
**              in putprintf() routine itself.  The change is made in
**              mon_exhandler().
**      29-aug-1996 (i4jo01)
**              In getargs(), make sure that '-s' is not followed by
**              a non null character, in case it is a paramater to be
**              passed to the back end.   
**      28-Nov-1997 (carsu07)
**              Added a check for new environment variable II_TM_SWITCH.
**              When II_TM_SWITCH is 'true' and II_TM_ON_ERROR is 'nocontinue'
**              then an error will terminate processing in the current
**              go_block and continue at the next go_block.
**              When II_TM_SWITCH is 'false' or not set and II_TM_ON_ERROR is
**              'nocontinue' then an error will cause all further processing
**              to terminate. This variable only applies when II_TM_ON_ERROR
**              is set to nocontinue.  (Bug #87415)
**	26-apr-1999 (mcgem01)
**		The product name is no longer hard-coded into the terminal
**		monitor login and logout banners.
**	07-jun-1999 (somsa01)
**		Modified the above change to not use STprintf() to pre-format
**		the banners, but rather use STpolycat().
**	20-apr-2000 (somsa01)
**		Updated MING hint for program name to account for different
**		products.
**	17-mar-2004 (kodse01)
**		Enabled -P flag to be used as -P<user-password> in the command line.
**		Also to accept -R<rolename/Role-password> in the command line. 
**		It does not prompt for passwords if provided in command line.
**	29-apr-2004 (kodse01)
**		Added flag -history_recall to enable command history recall support
**		in terminal monitor. History recall feature is not default.
**      11-Jun-2004 (hanch04)
**          Removed reference to CI for the open source release.
**	17-Jun-2004 (schka24)
**	    Safer env variable handling.
**	29-Sep-2004 (drivi01)
**	    Updated NEEDLIBS to link dynamic library SHQLIB and SHFRAMELIB
**	    to replace its static libraries. Added NEEDLIBSW hint which is 
**	    used for compiling on windows and compliments NEEDLIBS.
**  14-Oct-2004 (fanra01)
**      Bug 113240
**      Add command option list flattening from an array of command line
**      options to a pointer array of parameters, to avoid dereferencing
**      invalid command line array elements.
**	06-May-2005 (toumi01)
**		Return non-zero OS RC if /nocontinue and we get an error.
**	16-Jun-2005 (gupsh01)
**	    Initialize unicode collation table now for coercing unicode 
**	    data for display in terminal monitor.
*/
FUNC_EXTERN	VOID	go_buffer();
FUNC_EXTERN	STATUS	FEningres();
GLOBALDEF bool	Dingres ZERO_FILL;
GLOBALDEF char	Mon_intro[FE_PROMPTSIZE+1];
GLOBALDEF char	Mon_exit_msg[FE_PROMPTSIZE+1];
#ifdef FE_UTA
GLOBALDEF	char	*Xpipe;
GLOBALDEF	char	*Uname;
#endif /* FE_UTA */
GLOBALDEF	char	*Dbname;
GLOBALDEF	ADF_CB	*Tm_adfscb;
GLOBALDEF	QRB	*Tm_qrb;
GLOBALDEF	bool HistoryRecall = FALSE;
/*
** Default is to use the line drawing/box characters for surrounding
** the results of a retrieve/select statement.
*/
GLOBALDEF       bool    IIMOupcUsePrintChars = FALSE;
GLOBALDEF	char	*IIMOildInitLnDraw = NULL;
GLOBALREF	bool	Tm_semicolon;
i4
main(argc, argv)
i4	argc;
char	*argv[];
{
	STATUS	tmquel();
	/* Tell EX this is an ingres tool. */
	(void) EXsetclient(EX_INGRES_TOOL);
	MEadvise(ME_INGRES_ALLOC);
	PCexit((i4) tmquel(argc, argv));
}
/*{
** Name:	tmquel() -		Main line of TM/QUEL
**
** Description:
**
** Returns:
**	STATUS
**
** History:
**	09-feb-1988	- Modified to swicth back to QUEL after startup. (ncg)
**			- BUG 1712: Modified to COMMIT after startup. (ncg)
**			- BUG 1882: Checked for II_SQL_INIT. (ncg)
**	09-dec-1990	- Prompt for Password after all other flags have been
**			  processed.
**	15-may-1991 (jhw) Moved 'FEset_exit()' to 'PAingres()'.
**	02-aug-1991 (kathryn) Bug 38893
**		Add global IIMOildInitLnDraw to save return value of ITldopen().
**		This string is needed to initialize IT line drawing chars.
**		Issued here, and after return from child processes (edit/shell).
**	02-mar-1992 (kathryn) Bug 39902
**		Changed arguments to FEcopyright from messages  F_MO0030 and
**		F_MO0031 to literal strings.  Product Name and copyright year
**		remain the same across all languages so ERget of messages is not
**		needed.  Bug 39902 was ERget of messages failed and sent
**		bad arguments to FEcopyright() which assumes all arguments
**		are literals from FE's and therefore will always be valid.
**	24-mar-92 (seg) Must EXdelete first handler before EXdeclare of second.
**	31-may-1995 (harpa06) Bug #68876
**		Since QUEL is currently not supported in STAR, created an error
**		message to be displayed if the user trys to invoke QUEL with
**		a STAR database. The error message will be displayed and then
**		TM will quit.
**      25-sep-96 (rodjo04)
**              BUG 77034: Fix given so that terminal monitor can use the
**              environment variable II_TM_ON_ERROR to either terminate or
**              continue on error. Added a boolean parameter to IItmmonitor().
**              This new parameter is necessary so that we can tell the
**              difference between stdin, redirected input, and the use
**              of an include file.
**  14-Oct-2004 (fanra01)
**      Ensure arguments to PAingres and FEingres are properly initialised.
*/
static char	*getargs();
static STATUS	mon_exhandler();
static STATUS	PAingres();
STATUS
tmquel ( ac, av )
i4	ac;
char	*av[];
{
	LOCATION	fulloc,
			temp_loc;	/* NMloc uses static buffer */
	SYSTIME		time;
	char		fulloc_buf[MAX_LOC+1];
	char		errbuf[ER_MAX_LEN];
	char		buffer[100];
	char		*name = NULL;
	char		*on_err = NULL;
	i4		status;
	i4		dayfile_status;
	bool		startup_error;
	FILE		*dump_file = NULL;
	bool		save_keys = FALSE;
	EX_CONTEXT	context;
	ARGRET		rarg;
	i4		pos;
	STATUS		err;
        char            *func = NULL;
	GLOBALREF char	Env_ID[];	/* Environment id string */
	GLOBALREF char	Version[];	/* INGRES version number */
	struct qbuf	*q_open();
	STATUS		FEhandler();	/* FE interrupt handler */
	ADF_CB		*FEadfcb();
	char		*ITldopen();
    char*   cmdargs[MAX_ING_OPTS];
    int     argcnt;
    /*
    ** Ensure that the parameters passed to PAingres and FEingres are
    ** properly initialised.  This prevents dereferencing more array
    ** entries in argv than are required. 
    */
    for(argcnt = 0; argcnt < MAX_ING_OPTS; argcnt+=1)
    {
        /*
        ** Unpack the arguments and repackage them in cmdargs.
        ** Ensuring that only ac args are duplicated.  All
        ** other values are initialised to NULL.
        */
        cmdargs[argcnt] = (argcnt < ac) ? av[argcnt] : NULL;
    }
    av = cmdargs;
	/* Call IIUGinit to initialize character set attribute table */
	if ( IIUGinit() != OK)
	{
		PCexit(FAIL);
	}
	if (EXdeclare(FEhandler, &context) != OK)
	{
		EXdelete();
		IIUGerr(E_MO002F_Cant_set_exc_handler, UG_ERR_FATAL, 0);
	}
	FEcopyright(ERx("TERMINAL MONITOR"), ERx("1981"));
	/* initialize ADF session before processing arguments */
	if ((Tm_adfscb = FEadfcb()) == NULL)
	    IIUGerr(E_MO0001_Cant_init_ADF_ctrlblk, UG_ERR_FATAL, 0);
	Inisterm = SIterminal(stdin);
	Outisterm = SIterminal(stdout);
	/* Initialize IT line drawing */
	{
	    IIMOildInitLnDraw = STalloc(ITldopen());
	    if (Outisterm && IIMOildInitLnDraw != NULL)
		SIprintf(ERx("%s"), IIMOildInitLnDraw);
	}
	/* process flag arguments */
	av[ac]	= NULL;
	if ((Dbname = getargs(&ac, av))== NULL)
	{
	    putprintf(ERget(F_MO0032_No_database_name));
#ifdef SEP
	    if (IITMoutcomm != NULL)
	    {
		TCclose(IITMincomm);
		TCclose(IITMoutcomm);
	    }
#endif /* SEP */
	    EXdelete();
	    PCexit((i4) FAIL);
	}
	startup_error	= FALSE;
# ifndef	FT3270
	/* initialize international character translation */
	ITinit();
# endif	/* FT3270 */
	/*
	** Query language set in getargs based on passed in -q flag.
	** Default language is still quel when both available.
	*/
	/*
	** Start INGRES
	*/
	/*
	** The '-partial' command flag permits access to a partially created
	** or converted DB.  It should be used in conjunction with the 
	** backend flag '-A1' and is intended only for internal use.
	** If the '-partial' command flag is used, the TM avoids accessing any 
	** FE catalogs (as they may not exist) by calling PAingres (a 
	** stripped down version of FEingres) which just does a DB connect. 
	*/
	if (Partial_Access)
	{
	    if (PAingres(Dbname, av[1], av[2], av[3], av[4], av[5], av[6],
		av[7], av[8], av[9], av[10], av[11], av[12]) != OK)
	    {
#ifdef SEP
		if (IITMoutcomm != NULL)
		{
			putprintf(
			 ERx("Terminal Monitor could not connect to partial database"));
			TCclose(IITMincomm);
			TCclose(IITMoutcomm);
		}
#endif /* SEP */
		EXdelete();
		PCexit( (i4)FAIL );
	    }
	    IIUIautocommit( ( Tm_adfscb->adf_qlang == DB_SQL)
				? UI_AC_OFF : UI_AC_ON
	    );
	}
	else
	{
	    IIUIck1stAutoCom();
	    /* NULL first arg to FEningres says not to do FE dictionary check */
	    if (FEningres(NULL, (i4) 0, Dbname, av[1], av[2], av[3],
		av[4], av[5], av[6],
		av[7], av[8], av[9], av[10], av[11], av[12]) != OK)
	    {
#ifdef SEP
		if (IITMoutcomm != NULL)
		{
			putprintf(
			 ERx("Terminal Monitor could not connect to database"));
			TCclose(IITMincomm);
			TCclose(IITMoutcomm);
		}
#endif /* SEP */
		EXdelete();
		PCexit( (i4)FAIL );
	    }
	    if (( Tm_adfscb->adf_qlang == DB_SQL) && 
		( IIUIdbdata()->firstAutoCom == FALSE))
		IIUIautocommit(UI_AC_OFF);
	}
        /* Set the unicode collation table and normalization 
	** fields in Tm_adfscb 
	*/
	IItm_adfucolset (Tm_adfscb);
	/* Since QUEL is not currently supported by STAR, tell the user that
	   QUEL isn't supported by STAR and gracefully exit if the user is in
	   fact trying to invoke QUEL with a STAR DB */
	if (IIUIdcd_dist() && (Tm_adfscb->adf_qlang == DB_QUEL))
		IIUGerr(E_MO0071_STAR_not_supported, UG_ERR_FATAL, 0);
	/* initialize startup and shutdown messages */
	STpolycat(2, SystemProductName, ERget(F_MO0033_TM_login_banner),
		  Mon_intro);
	STpolycat(2, SystemProductName, ERget(F_MO0034_INGRES_exit_message),
		  Mon_exit_msg);
	/* preinitialize macros */
	macinit((i4 (*)())NULL, (char **)NULL); /* The joy of casting */
	/* print the dayfile */
	if (Nodayfile >= 0)
	{
		TMnow(&time);
		TMstr(&time, buffer);
		/* print out different header message for ingres/star */
		putprintf(Mon_intro, Dingres ? ERget(F_MO002E__STAR) : ERx(""),
				     Env_ID, Version, buffer);
	}
	/*
	** Get dayfile location even if -d is turned on.
	** The LOfstfile call for the startup file will use
	** the location built by this call later.
	**
	** The location must be copied into a "safe" buffer
	**   as NMloc() uses a global buffer.  Hence if
	**   we get an error the error message will be screwy
	**   as ERreport also calls NMloc().
	**
	*/
# ifdef IBM370
	STcopy(ERx("dayfile.data"), buffer);
# else
	STcopy(ERx("dayfile"), buffer);
# endif
	/*
	**  if NMloc() and LOcopy() return OK then dayfile_status is OK
	**  if NMloc() doesn't return OK LOcopy() isn't called
	**  Note:- LOcopy copy now returns VOID (MMB)
	*/
	if ((dayfile_status = NMloc(FILES, FILENAME, buffer, &temp_loc)) == OK)
		LOcopy(&temp_loc, fulloc_buf, &fulloc);
	if (Nodayfile == 0)
	{
		if (!dayfile_status)
		{
			SIcat(&fulloc, stdout);
		}
		else
		{
			/* error in obtaining dayfile location */
			ERreport(dayfile_status, errbuf);
			putprintf(ERget(F_MO0035_Error_finding_dayfile),
				errbuf);
		}
	}
	if ((Qryiop = q_open('w')) == NULL)
		/* ttymon: qbuf fail */
		ipanic(E_MO0062_1501301);
	/* GO TO IT ... */
	P_rompt		= Newline = TRUE;
	Userdflag	= Nodayfile;
	Nodayfile	= -1;
	/* run the system initialization file */
# ifdef	IBM370
	STcopy(ERx("startup.data"), fulloc_buf);
	NMloc(FILES, FILENAME, fulloc_buf, &fulloc);
# else
 	/*
 	** bug 4872
 	** If this is sql then look for startsql.
 	*/
 	LOfstfile( ERx( (Tm_adfscb->adf_qlang == DB_SQL)
 			? "startsql" : "startup"),
 			&fulloc);
# endif
	/*
	** (bml) -- bug fix #6382 -- a hacked in solution to make \quit
	** in an included file work
	*/
	if ( include(&fulloc, ERget(F_MO0036_while_run_sys_init_fi)) == -1 &&
			quit(OK) )
		return OK;
	/*
	** BUG 1882 - If starting in SQL the startup file is called
	**	      II_SQL_INIT. If in QUEL INIT_INGRES. (ncg)
	*/
	STcopy( ERx( (Tm_adfscb->adf_qlang == DB_SQL)
			? "II_SQL_INIT" : "INIT_INGRES"),
		buffer
	);
	/* find out what the user initialization file is */
	NMgtAt(buffer, &name);
	if ( name != NULL && *name != EOS )
	{
		STlcopy(name, fulloc_buf, sizeof(fulloc_buf)-1);
		if ( (status = LOfroms(PATH & FILENAME, fulloc_buf, &fulloc)) != OK )
		{
			ERreport(status, errbuf);
			putprintf(ERget(F_MO0037_Error_in_User_Startup),
				 name, errbuf);
		}
		/*
		** (bml) -- bug fix #6382 -- a hacked in solution
		** to make \quit in an included file work
		*/
		else if ( include(&fulloc, ERget(F_MO0038_while_run_INIT_INGRES)
				) == -1 && quit(OK) )
		{
			return OK;
		}
	}
	/* Data base start up file */
	STcopy(Dbname, buffer);
 	/*
 	** bug 4872
 	** If this is sql then attach _SQL_INIT.
 	*/
 	CVupper(STcat(buffer, ERx(  (Tm_adfscb->adf_qlang == DB_SQL)
 			? "_SQL_INIT" : "_ING")));
	NMgtAt(buffer, &name);
	if ( name != NULL && *name != EOS )
	{
		STlcopy(name, fulloc_buf, sizeof(fulloc_buf)-1);
		if ( (status = LOfroms(PATH & FILENAME, fulloc_buf, &fulloc)) != OK )
		{
			ERreport(status, errbuf);
			putprintf(ERget(F_MO0039_Error_Startup_FileNam),
				name, errbuf);
		}
		/*
		** (bml) -- bug fix #6382 -- a hacked in solution to
		** make \quit in an included file work
		*/
		else if ( include(&fulloc,
					ERget(F_MO003A_while_run_db_startupf)
				) == -1 && quit(OK) )
		{
			return OK;
		}
	}
	NMgtAt((ERx("II_TM_ON_ERROR")), &on_err);
	if ((on_err != NULL) && (*on_err != EOS))
	{
		CVlower(on_err);
		if (STcompare(on_err,ERget(F_MO0019_continue)) == 0)
		{
			Err_terminate = FALSE;
		}
		else
		{
			Err_terminate = TRUE;
		}
	}
        NMgtAt((ERx("II_TM_SWITCH")), &func);
        if ((func != NULL) && (*func != EOS))
        {
                CVlower(func);
                if (STcompare(func,"true") == 0)
                    Switch_func = TRUE;
                else
                    Switch_func = FALSE;
        }
	/* Check if this is a Runtime INGRES installation.  Note: the TM
	** treats fully restrictive and partially restrictive Runtime
	** INGRES in the same way - the TM doesn't allow tables to be created
	** (or dropped, if SQL) when running Runtime INGRES.
	*/
	Runtime_INGRES = FALSE;
	Nodayfile = Userdflag;
	EXdelete();						 /* pop FEhandler off EX chain */
	EXdeclare(mon_exhandler, &context);	/* set up exception handling */
	EXinterrupt((i4)EX_RESET);		/* interrupts should be on now,
						**	no matter what */
	for ( ; Idepth > 0; Idepth--)
		SIclose(Ifiled[Idepth]);
	Input		= stdin;
	Nodayfile	= Userdflag;
	cgprompt();
        if (!Inisterm)
            err = IItmmonitor(TRUE);
        else
            err = IItmmonitor(FALSE);
	_VOID_ quit(err);
#ifdef SEP
	if (IITMincomm != NULL)
	{
		TCclose(IITMincomm);
		TCclose(IITMoutcomm);
	}
#endif /* SEP */
	EXdelete();	/* For mon_exhandler */
	return OK;
}
/*
** tm_intr() - TM interrupt handler for user-generated interrupts
**
** History:
**	12/10/84 (lichtman)	-- reset Do_macros to Savedomacros on interrupt.
**		necessary because Do_macros is sometimes temporarily made TRUE.
**	12/9/87 (peterk) - add code to deal with dangling transactions that
**		may have been started by QR (e.g. for HELP ALL).
**	12/11/90 (kathryn) - Re-open qry buffer in append mode.
**		
*/
static VOID
tm_intr(typ)
i4	typ;
{
	GLOBALREF i4	Moninquote;
	GLOBALREF i4	Monincomment;
	GLOBALREF i4	Monincomment1;
	Newline		= P_rompt = TRUE;
	Nodayfile	= Userdflag;
	Oneline		= FALSE;
	Idepth		= 0;
	Input		= stdin;
	Moninquote	= FALSE;
	Monincomment	= FALSE;
	Monincomment1	= FALSE;
	Do_macros	= Savedomacros;
	if (typ != 2)
		/* tm_intr: typ %d */
		ipanic(E_MO0063_1501302, typ);
	if (Outisterm)
	{
	    char	*p = ITldclose();
	    if (p != NULL)
		SIprintf(ERx("%s"), p);
	}
# ifndef	CMS
	putprintf(ERget(F_MO003B_Interrupt));
# endif
	IIbreak();
	/*
	** take care of case where QR has started a transaction itself
	** internally, such as in HELP ALL before the OPEN CURSOR
	*/
	/*
	** NOTE: The call to FEinMST() is here temporarily to work
	** around a 6.0 BE bug, in which if immediately after a
 	** BEGIN TRANSACTION statement is completed, an interrupt is
	** sent to the BE, the BEGIN TRANSACTION appears to get backed
	** out.  The FEinMST ensures that in fact a transaction is
	** active and the ABORT will not fail.
	*/
	if (Tm_qrb && Tm_qrb->endtrans && FEinMST())
/* # line 791 "tmmain.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 792 "tmmain.sc" */	/* host code */
	/* reset Tm_qrb so that if another interrupt occurs before the
	** next \g (go), we won't try to look at a meaningless QRB.
	*/
	Tm_qrb = NULL;
	if (q_ropen(Qryiop, 'a') == NULL)
		/* tm_intr: q_ropen 2 */
		ipanic(E_MO0069_1501401);
}
/*
**	GETARGS - remove terminal arguments from command line
**
**	Purpose:
**		To remove the terminal monitor arguments from the command line
**		and to update argc and argv to reflect this.  The idea is
**		to pass the rest of the command line to ingres.
**
**	Parameters:
**		argc	- pointer to arguement count
**		argv	- *** pointer to arguement vector list
**
**	Returns:
**		0	- ok
**		-1	- bad flag
**	History:
**		20-sept-85	(marian)	bug # 6190
**				made sure *targv = 0 to 0 out the last location
**		09-feb-1988	(neil)		bug # 1818
**				ignored more than one flag w/o a '-'
**		28-nov-1988	(kathryn)	bug # 2997
**			Recognize the -v flag as terminal monitor argument.
**			-v is vertical separator.
**		28-nov-88 (bruceb)
**			If -v, then set global to be checked in putresults().
**		10-mar-89 (kathryn)	Bug # 4105
**			Recognize the +a flag as a terminal monitor argument.
**			+a enables the Autoclear function. (same as default).
**		05-oct-89 (teresal)
**			Add new '-partial' flag.
**		12-dec-89 (teresal)
**			Recognize dbms flags '-R' (Role)  and '-P' (Password)
**			and prompt for Password (and/or Role Id) if required.
**      	09-nov-1990 (kathryn)
**          	        For -P and -R startup flags:  Generate error message 
**          		and exit if IIUIpasswd or IIUIroleID fail.  Both will
**          		return NULL if password is included with startup flag.
**		05-dec-1990 (kathryn)
**			Correct coding bug from above fix for -P and -R flags.
**		01-feb-1991 (kathryn)
**			Removed file qr/fefmtflg.c which contained FEfmtflg(),
**			to handle the formatting flags as well and the 
**			"-q" flag.  LIBQ now handles all formatting flags, 
**			so moved handling of "-q" flag from qr/fefmtflg.c 
**			to this file and removed fefmtflg.c.  
**		07-aug-1991 (kathryn) 	Bug 38822
**			No longer ignore startup flags which do not begin with
**			"-" or "+".  Pass all unknown flags to DBMS.
**              25-oct-1994 (sarjo01) Bug 65284
**                      Added case for +s option.
**		17-mar-2004 (kodse01)
**			Enabled -P flag to be used as -P<user-password> in the command line.
**			Also to accept -R<rolename/Role-password> in the command line. 
**			It does not prompt for passwords if provided in command line.
*/
static char *
getargs(argc, argv)
i4		*argc;
register char	**argv;
{
	register char	**targv;
	register i4	targc;
	char		*database = NULL;
	char		*IIUIpassword();
	char		*IIUIroleID();
	char		*with_ptr;
	char		*cp;
	bool		passwd_flag = FALSE;
#ifdef SEP
	LOCATION	inLoc,outLoc;
	char		inFile[MAX_LOC+1],outFile[MAX_LOC+1];
	char		*cptr;
#endif /* SEP */
# ifdef WANT_SQL
	Tm_adfscb->adf_qlang = DB_SQL;
# endif
	for (targc = *argc, *argc = 0, targv = argv; targc--; argv++)
	{
	    if (**argv == '-')
	    {
		switch ((*argv)[1])
		{
		    case 'a':
			Nautoclear = 1;
			break;
		    case 'd':
			Nodayfile = 1;
			break;
		    case 'q':
			*targv = *argv;
			(*targv)[1] = 'L';   /* its still 'L' to the backend */
			if (STbcompare(&(*targv)[2], 0, ERx("quel"), 
			    0, TRUE) == 0)
			{
				Tm_adfscb->adf_qlang = DB_QUEL;
			}
			else if (STbcompare(&(*targv)[2], 0, ERx("sql"), 
				 0, TRUE) == 0)
			{
				Tm_adfscb->adf_qlang = DB_SQL;
			}
			*targv++;
			++*argc;
			break;
		    case 's':
                        /*  the 's' could be part of some back end param */
                        if ((*argv)[2] == EOS)
                            Nodayfile = -1;
                        else
                        {
                            *targv++;
                            ++*argc;
                        }
			break;
		    case 'v':			/* Bug 2997 */
			ITsetvl((*argv)+2);	/* vertical separator */
			IIMOupcUsePrintChars = TRUE;
			break;
		    case 'P':			/* Ingres Password flag */
			if ((*argv)[2] == EOS)
				passwd_flag = TRUE;
			else
				*targv++ = *argv; /* Take the password on command line */
			++*argc;
			break;
		    case 'R':			/* Ingres Role flag */
			/* If no role id is given, prompt for one */
			if ((*argv)[2] == EOS)
			{
			    char rbuf[FE_MAXNAME+3];
			    FEprompt(ERx("Enter Role Id:"), FALSE,
				sizeof(rbuf) - 1, rbuf+2);
			    if (rbuf[2] != EOS)
			    {
					rbuf[0] = '-';
					rbuf[1] = 'R';
					*argv = STalloc(rbuf);
			    }	
			}
			else
			{
				char slash[2] = "/";
				if (STindex(*argv, slash, 0) == NULL) 
				/* Prompt for password */
				if ((cp = IIUIroleID(*argv)) != NULL)
				{
			    	*argv = cp;
				}
				else
				{
			    	IIUGerr(E_UG0014_BadArgument, 0, 1, *argv);
			    	if (Tm_adfscb->adf_qlang == DB_SQL)
			        	IIUGerr(E_UG0011_ArgSyntax,0,2,ERx("sql"),
					STalloc(ERget(E_MO0070_R_FlagSyntax)));
			    	else
					IIUGerr(E_UG0011_ArgSyntax,0,2,ERx("quel"),
					STalloc(ERget(E_MO0070_R_FlagSyntax)));
			    		EXdelete();
			    		PCexit(FAIL);
				}
			}
			*targv++ = *argv;	/* Pass on to dbms */ 	
			++*argc;
			break;
#ifdef SEP
		    case '*':
			cptr = (*argv)+2;
# ifndef hp9_mpe
			STpolycat(3,ERx("in_"),cptr,ERx(".sep"),inFile);
			STpolycat(3,ERx("out_"),cptr,ERx(".sep"),outFile);
# else
			STpolycat(3,ERx("in"),cptr,ERx(".sep"),inFile);
			STpolycat(3,ERx("out"),cptr,ERx(".sep"),outFile);
# endif
			LOfroms(FILENAME & PATH, inFile, &inLoc);
			LOfroms(FILENAME & PATH, outFile, &outLoc);
			if (TCopen(&inLoc,ERx("r"),&IITMincomm) == OK)
			{
			    if (TCopen(&outLoc,ERx("w"),&IITMoutcomm) == OK)
			    {
				TCputc(TC_BOS,IITMoutcomm);
				TCflush(IITMoutcomm);
			    }
			    else
				IITMincomm = NULL;
			}
			break;
#endif /* SEP */
		    default:
			/* Look for '-history_recall' flag */
			if (STbcompare(((*argv)+1),0,ERx("history_recall"),0,TRUE) == 0)
			{
			    HistoryRecall = TRUE;
			    break;
			}
			/* Look for '-partial' flag */
			if (STbcompare(((*argv)+1),0,ERx("partial"),0,TRUE) == 0)
			{
			    Partial_Access = TRUE;
			    break;
			}
			/* Save all other flags for ingres */
			*targv++ = *argv;
			++*argc;
# ifdef	MVS 
			if (MEcmp(*argv, ERx("-xdc"), 4)==0)
			    MEcopy(ERx("!dbc"), 4, *argv);
			if (MEcmp(*argv, ERx("-XDC"), 4)==0)
			    MEcopy(ERx("!dbc"), 4, *argv);
# endif
			if ((*argv)[1] == 'r')
			    Dingres	= TRUE;
		}
	    }
	    else if (**argv == '+')
	    {
		switch ((*argv)[1])
		{
		    case 'a':			/* bug 4105 */
			Nautoclear = 0;
			break;
		    case 'c':			/* connection parameters */
		    case 'C':
			/* set up WITH clause for CONNECT */
			with_ptr = STalloc(*argv + 2);
			IIUIswc_SetWithClause(with_ptr);
			break;
                    case 's':
                        Nodayfile = 0;
                        break;
		    default:
			*targv++ = *argv;
			++*argc;
		}
	    }
	    else
	    {
		/*
		** BUG 1818: If got a database then ignore any flags
		**	     that are not preceded by a '-'. (ncg)
		** BUG 38822: Don't ignore any startup flags.
		**	If got database name then pass flag to DBMS.
		*/
		if (*argc != 0 && !database)/* First time through = "ingres" */
		{
		    database = *argv;
		    /* don't reset targv, argc; returning dbname separately */
		}
		else
		{
		    *targv++ = *argv;
		    ++*argc;
		}
	    }
	}
	if (passwd_flag)
	{
	    if ((cp = IIUIpassword(ERx("-P"))) != NULL)
		*targv++ = cp;
	    else
	    {
		IIUGerr(E_UG0014_BadArgument, 0, 1, cp);
		if (Tm_adfscb->adf_qlang == DB_SQL)
			IIUGerr(E_UG0011_ArgSyntax,0,2,ERx("sql"),
			STalloc(ERget(E_MO006F_P_FlagSyntax)));
		else
			IIUGerr(E_UG0011_ArgSyntax,0,2,ERx("quel"),
			STalloc(ERget(E_MO006F_P_FlagSyntax)));
		EXdelete();
		PCexit(FAIL);
	    }
	}
	if (Tm_adfscb->adf_qlang == DB_SQL)
	    Tm_semicolon = TRUE;
	*targv = 0;			/*	bug # 6190	*/
	return database;
}
/*
**  EXHANDLER -- the terminal monitor exception handler
**	Handles interrupts, syserrs, errors from the backend.
**	If it gets an exception it doesn't know what to do with,
**	it prints an error message and then acts like it got an
**	interrupt.
**
**	Parameters:
**		exargs	- standard argument to all exception handlers
**				(see Compatibility Library Specs.)
**
**	History:
**		03/24/83 (lichtman) -- written for Compatibility Library
**					translation effort
**	17-mar-92 (seg)	Exception handlers return STATUS, not EX
**	07-jun-99 (wanfr01)
**		Bug 95500, INGCBT 173
** 		Rollback work upon interrupts, so prior work is not
**		committed.
**      10-Apr-2003 (hanal04) Bug 109389 INGSRV 2041
**              Log E_MO003C using IIUGerr() to ensure an E_CCXXXX
**		error code is generated when TM hits an unknown
**              exception. Many client scripts grep for E_ to deterine
**              whether batch jobs have hit a problem.
**		
*/
static STATUS
mon_exhandler(exargs)
EX_ARGS *exargs;
{
	EX	exno;
	exno = EXmatch(exargs->exarg_num, 7,
			EXINTR, EXFATER, EXQUIT, EXKILL,
			EXKILLBACKEND, EX_MON_LJMP, EXDECLARE);
	switch (exno)
	{
		case 1:
			/*
			** Disable all interrupts while we are
			** processing this one.
			** This EXenable will happen after the long jump.
			*/
			EXinterrupt( EX_OFF );
			tm_intr((i4)2);
			return EXDECLARE;
		case 2:
		case 3:
		case 4:
		case 5:
/* # line 1126 "tmmain.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 1127 "tmmain.sc" */	/* host code */
			_VOID_ quit(OK);	/* may not return */
			/* if it returns, then fall into ... */
		case 6:
			return EXDECLARE;
		case 7:
			return (0);
		default:
			IIUGerr(E_MO003C_Unknown_exception, UG_ERR_ERROR, 1,
				(PTR)&exargs->exarg_num);
/* # line 1136 "tmmain.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 1137 "tmmain.sc" */	/* host code */
			_VOID_ quit(OK);	/* may not return */
			return EXRESIGNAL;
	}
}
/*
** Name:        PAingres() -    Partial Access DBMS Connect routine.
**
** Description:  
**      Starts a connection to the DBMS (INGRES) with the passed in parameters
**      in parameters as arguments.  Sets the user and the DBA name to a dummy
**      string.  Sets the database name.
**
**	Note:  This is a stripped down version of 'FEingres()' that does the
**	minimal DB connect for accessing a partially created or converted DB.
**	Make sure that the module exits cleanup is set-up correctly.
**
** Inputs:
**      a[0-12] {char *}  The database name on which to start-up the DBMS, and
**                        additional parameters to be passed to the DBMS
**                        (terminated by a NULL pointer.)
**
** Returns:
**      {STATUS}        'FEinqerr()' If this return value is not OK, then the
**                                      database is always left in a closed
**
** Side Effects:
**	Calls IIUIpsdPartialSessionData
**
** History:
**      05-oct-1989 (teresal) -- Written.
**	28-aug-1990 (Joe)
**	    Changed to call IIUIpsdPartialSessionData so that any
**	    calls to IIUIdbdata after this point will get a UIDBDATA
**	    for a partial session.
**	15-may-1991 (jhw) Open with UI_SESSION, moved 'FEset_exit()' here, and
**		disconnect using 'FEing_exit()' to duplicate 'FEningres()'
**		functionality.
*/
static STATUS
PAingres (db_name, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)
  char *db_name, *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10, *a11, *a12
  ;
{
	STATUS	rval;
	i4	FEing_cleanup();
/* # line 1195 "tmmain.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(UI_SESSION);
    IIsqConnect(0,db_name,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,(char *)0);
  }
/* # line 1199 "tmmain.sc" */	/* host code */
	if ( (rval = FEinqerr()) != OK )
		return rval;
	FEset_exit(FEing_cleanup);
	if ((rval = IIUIpsdPartialSessionData(db_name)) != OK)
	{
		FEing_exit();
		return rval;
	}
/* # line 1207 "tmmain.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 1208 "tmmain.sc" */	/* host code */
	return OK;
}