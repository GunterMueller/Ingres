# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 1986, 2005 Ingres Corporation
**
*/
#include    <compat.h>
#include    <pc.h>
#include    <gl.h>
#include    <sl.h>
#include    <gv.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <duf.h>
#include    <er.h>
#include    <cs.h>		/* required by lk.h */
#include    <lk.h>
#include    <usererror.h>
#include    <duerr.h>
#include    <adf.h>
#include    <adulcol.h>
#include    <aduucol.h>
# include <dudbms.h>
# include <duenv.h>
#include    <ducdb.h>
#include    <cv.h>
#include    <ex.h>
#include    <si.h>
#include    <duustrings.h>
#include    <lo.h>
#include    <nm.h>
/* added hdr for TITAN */
#include    <st.h>
#include    <nm.h>
#include    <cm.h>
#include    <me.h>
#include    <gcccl.h>
#include    <gc.h>
#include    <pm.h>
#include    <cui.h>
#include    <duucatdef.h>
# include <dustar.h>
#include   <ut.h>
#include   <duu.h>
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/**
** 
**  Name: DUCREATE.QSC  -   contains all of the equel procedures specific
**			    to the Ingres database utility, createdb.
**
**  Description:
**	  This file contains all of the equel procedures specific to the
**	Ingres database utility, createdb.
**
**          main()			    - the main entry point to this 
**					      program.
**	    duc_db_envinit()		    - get the info needed to create
**					      this db from the dbdb.
**	    duc_exit()			    - createdb exit handler.
**	    duc_1dbdb_init()		    - initializes the "dbdb" system 
**					      catalogs with information about
**					      database being created.
**	    duc_makedb()		    - create and configure the extended
**					      system catalogs.
**	    duc_0makecdb()		    - create and configure the extended
**					      system catalogs of a 
**					      cdb(coordinator database).
**	    duc_1create_iidd_catalogs()	    - creates the STAR catalogs that
**					      are prefixed by 'iidd_'.
**	    duc_2create_iidd_ddb_catalogs() - creates the STAR catalogs that
**					      are prefixed by 'iidd_ddb_'.
**	    duc_3modify_star_catalogs()	    - modify all STAR catalogs to have
**					      HASH structure.
**	    duc_4init_star_catalogs()	    - initialize the STAR system 
**					      catalogs.
**	    duc_5init_star_capabilities()   - initialize the STAR system 
**					      catalogs:
**						iidd_dbcapabilities, 
**						and iidd_ddb_dbcaps.
**          duc_gmake_gdb()                 - make special system catalogs for
**                                            Gateway databases.
**	    duc_goodClientList()	    - Check if client list is ok.
**          
**          duc_femodify()                  - Modifies front-end system catalogs
**					      to the mentioned page_size.   
** 
**  History:
**      19-Sep-1986 (ericj)
**          Initial creation.
**      13-Apr-1987 (ericj)
**          Added call to dui_makedb() to create frontend system catalogs.
**      13-Jul-1988 (alexc)
**	    Modified duc_makedb() and added duc_0makecdb() to create and 
**	    initialize STAR system catalogs.
**      28-Jul-88 (ericj)
**          Updated 'duc_makedb()' for new iiprocedure specification.
**	12-aug-1988 (greg)
**	    IIPROCEDURE: move dbpid[x] to end of tuple and add create_date
**			 precede all attribute names with dbp_
**      21-Sep-1988 (alexc)
**	    Initial creation of routines: duc_1create_iidd_catalogs(), 
**		duc_2create_iidd_ddb_catalogs(), duc_3modify_star_catalogs(),
**		duc_4init_star_catalogs() and duc_5init_star_capabilities().
**	17-Oct-1988 (alexc)
**	    modify to get nodename from II_INS_VNODE logical instead of using
**	    GChostname() and initialize numrows for iidd_physical_tables. 
**	    Also merge code from RPLUS.
**      23-oct-1988 (alexc)
**          Modify file to 80 columns.
**      24-oct-1988 (alexc)
**          create new TITAN catalog 'iidd_registrations'.
**      25-oct-1988 (alexc)
**          remove CVlower on DU_nodename.
**      26-oct-1988 (alexc)
**          start object_base count from 1000.
**      06-feb-89 (ralph)
**          Changed duc_makedb to specify new iiprotect and iiintegrity layout
**      03-mar-89 (ralph)
**          GRANT Enhancements, Phase 1:
**          Added proseq, progtype fields to iiprotect;
**          Added intseq field to iiintegrity;
**          Rearranged iiprotect fields to put proreserve at end.
**      08-mar-1989 (alexc)
**          Added to iidd_ddb_ldbids the following columns:
**		ldb_user, ldb_dba, ldb_dbaname.
**          Fixed duc_3modify_star_catalogs().
**	    Combine iidd_physical_tables, iidd_ddb_ingtables...etc into
**		iitables.
**          Fixed bug:  system_use initialize with correct value "S".
**          Change initialization value of all_to_all and ret_to_all
**		in star catalog iidd_tables to "Y".
**          Remove all references to duc_6init_fe_links(), and the
**		function itself.
**      17-mar-1989 (alexc)
**          replace NMgtAt() call with duu_get_vnode().
**	    Get node name from II_GCNxx_LCL_VNODE, not II_INS_VNODE anymore.
**      20-mar-1989 (alexc)
**          Fix number of parameters pass to du_get_dbid() and
**		this fix is for creating iidbdb duc_dbdb_envinit().
**      20-mar-89 (ralph)
**          GRANT Enhancements, Phase 1
**          duc_1makedbdb() creates iiusergroup/iiapplication_id 
**      27-mar-89 (neil)
**          Rules: Added creation and modify of iirule.
**      11-apr-1989 (alexc)
**          Added view iidd_ddb_finddbs for FINDDBS.
**      12-apr-1989 (alexc)
**          Added view iidd_physical_tables, modify to accept '/star' flag.
**	15-apr-1989 (alexc)
**	    Added two DDB objects: iiddb_netcost, and iiddb_nodecosts for
**	    use in optimizer.
**	17-apr-1989 (alexc)
**	    Move iiddb_netcost and iiddb_nodecosts from CDB to iidbdb.
**	10-may-89 (ralph)
**	    GRANT Enhancements, Phase 2:
**	    duc_1makedbdb() creates iidbpriv catalog;
**	    added minor format change to iiprotect.
**	    changed iiapplication_id to iirole;
**	    add new fields (default_group, reserved) to iiuser.
**	22-May-89 (teg)
**	    move local hdrs to lib.
**      24-May-89 (teg)
**          move DI, LK, LG to server
**	31-may-89 (ralph)
**	    GRANT Enhancements, Phase 2c:
**	    duc_1makedbdb creates iidbprivileges view.
**	12-sep-89 (ralph)
**	    B1 Enhancements:
**		Eliminate creation of iidbaccess.
**		Add DB_ACCESS and UP_SYSCAT to iidbprivileges.
**      03-jan-90 (neil)
**          Alerters/Terminator II: Added creation and modify of iievent.
**	16-jan-90 (ralph)
**	    Add password attribute to the iiuser catalog.
**	21-jan-1990 (alexc)
**	    Add creation of iidbms_comment and iisynonyms catalogs and
**		modify statements for these catalogs.
**      11-apr-90 (jennifer)
**          Bug 20463 - Initialize iisecuritystate with all events
**          set to off.
**      16-apr-90 (ralph)
**          Add DB_ADMIN attribute to IIDBPRIVILEGES
**	26-apr-1990 (alexc)
**	    add new STAR catalogs for 2PC: iidd_ddb_dxlog, iidd_ddb_dxldbs
**		and index iidd_ddb_xdxlog.
**      5-may-90  (pete)
**          Add more arguments to dui_makedb() call. Check if can
**	    can create temporary files.
**	29-may-90 (edwin)
**	    Move some literal strings out into duustrings.h.
**      13-jun-1990 (teresa)
**          add new STAR catalogs for 2PC: iidd_ddb_dxlog, iidd_ddb_dxldbs
**              and index iidd_ddb_xdxlog and fix modify of iisynonym
**	25-jun-1990 (teresa)
**	    fixed bug 30853 that existed because iidd_indexes was not
**	    populated from iiindexes.  Also added logic to make a union view
**	    joining iidd_registrations with iidd_views.  This is a partial fix
**	    to bug 30502, but not the entire solution.  I am putting this in
**	    now since I figured it out, and do NOT want this to get lost.
**	    (Still need to change catalogs to map iiviews to this new union
**	    view.)
**      10-jul-1990 (teresa)
**          removed obsolete catalogs: iidd_ddb_ldb_ingtables,
**          iidd_ddb_ldb_indexes, iidd_ddb_ldb_keycols, iidd_ddb_ldb_phytables,
**          iidd_ddb_ldb_xcolumns
**	24-jul-90 (teresa)
**	    changed .qh include files to .qsh.
**      08-aug-90 (ralph)
**          Add new fields to iiprotect for FIPS, REVOKE, GRANT, etc.
**	    Add iisectype base table.
**	    Add iisecurity_state, iiusers and iilocation_info iidbdb views.
**	    Revised definition of iidbprivileges for DB_ADMIN & GLOBAL_USAGE.
**	03-oct-90 (teresa)
**	    fix bug 33626 (do not check for ddb/cdb in iidatabase unless
**	    we are building a distributed database. Remove SET_UID ming hint,
**	    as DI has been moved into the server.
**	14-dec-90 (ralph)
**	    Don't connect as -u$ingres when invoking iiqef_alter_db
**	15-jan-91 (ralph)
**	    Rename attribute 'fill1' to 'dbflags' in table 'iidbpriv'
**	20-sep-91 (teresa)
**	    pick up ingres63p change 262603:
**	      6-jun-91 (rickh)
**		Move the setting of du_dbservice to argument parsing time.
**		otherwise, the wrong dbservice value is written to the
**		configuration file & FINDDBS incorrectly recovers the database.
**	      27-jun-91 (rickh)
**		Make sure that the extended catalogs are created, where 
**		relevant, by the gateway server.  The INGRES server may no 
**		longer access RMS databases.
**	      21-jun-91 (teresa)
**		Modified createdb to connect to star server to call upgradefe on
**		creation of a DDB.
**	09-nov-92 (jrb)
**	    Change ING_SORTDIR to ING_WORKDIR and DU_SORTS_LOC to DU_WORK_LOC 
**	    for ML Sorts project.
**	20-nov-92 (bonobo)
**		Updated MING hints with CUFLIB.
**	11-jan-93 (jpk)
**	    Included three new header files to support PM lookup
**	    of FIPS parameters.  Added forward references for
**	    new functions called to support the same.
**	05-apr-1993 (ralph)
**	    DELIM_IDENT:
**	    Set the case translation flags in dbservice.
**	    Pass duc_errcb to duc_iilgmo as required.
**	    Do case insensitive compares in duc_procedure()
**	 1-apr-93 (swm)
**	    Removed forward reference to MEreqmem(), this is now inherited
**	    from <me.h>.
**	18-may-1993 (robf)
**	    Removed SUPERUSER references, replaced by SECURITY
**	25-may-1993 (bryanp)
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**	26-july-1993 (jpk)
**		Set values of dbservice correctly for local, star dbs.
**		Support new flag -real_user_case.  Validate settings
**		for that and combinations of it and reg_id_case and
**		delimited_id_case.
**	27-jul-193 (ralph)
**	    DELIM_IDENT:
**	    Add support for DU_REAL_USER_MIXED flag.
**	8-aug-93 (ed)
**		unnest dbms.h
**	09-nov-93 (rblumer)
**	    added duc_xlate_username function to correctly case-translate the
**	    username for the target db and the iidbdb.  Removed uses of du_dba
**	    and du_usrname; replaced them with du_dba_target, du_xusrname, and
**	    du_unorm_usr. 
**	16-dec-93 (tyler)
**	    Replaced GChostname() with call to PMhost().
**      31-jan-94 (mikem)
**          Added include of <cs.h> before <lk.h>.  <lk.h> now references
**          a CS_SID type.
**	29-Jan-1994 (jpk)
**	    Bug #58408.
**	    Added debugging output support for upgradedb.
**	7-Feb-1994 (fredv)
**	    Createdb no longer needs SETUID, removed the MING hint.
**	25-Apr-1994 (fredv)
**	    Removed DMFLIB from the NEEDLIBS list. It isn't needed anymore.
**      07-Oct-1994 (brogr02) First change after CA acquisition.  Moved
**          call to duc_v10_iiregistered_objects() to after the tables
**          it depends on have been created.
**	15-Nov-1994 (nanpr01)
**	    Added SQL GRANT statement to allow/deny default access to
**          public. Bug #60370. 
**  31-Jan-1995 (reijo01)
**      Added table creations for the IMA tables,
**      iigw07_relation, iigw07_attribute, iigw07_index,
**      if the database is owned by $ingres. Bug #66605
**      02-feb-1995 (sarjo01) Bug 66517
**          In queries testing for existence of star tables
**          iistar_cdbs, iiddb_netcost, iiddb_nodecosts, added
**          check for names in upper case for FIPS
**      21-mar-1995 (sarjo01)
**          Bug 67417: various bad behavior with star/fips database.
**          Assure that owner is upper-cased when necessary.
**          It was not clear at this time if any specific symptoms
**          were attributable to the owner being forced to lower.
**	2-nov-1995 (angusm)
**	    1. Bug 71173. Prevent location names from being forced to 
**	    lowercase when reading the command line. Location names now
**	    validated by new function duc_val_loc() instead of
**	    du_chk3_locname() which is effectively obsolete.
**	    bug 71173. 
**	    2. Also, removed ifdef'd out code for
**	    command-line flags -delim_id_case, -reg_id_case
**	    which were replaced by PM items.
**	14-nov-1995(angusm)
**	    location names now force lower in all circumstances.
**	    (decision as per product owner). Modify above change.
**	13-jun-96 (nick)
**	    LINT directed changes.
**	13-sep-96 (nick)
**	    Add in EXsetclient() call ; if we don't do this we won't
**	    catch user interrupts.
**	    Remove coding standard violations from the signal handler.
**      28-sep-96 (mcgem01)
**              Global data moved to ducdata.c
**    01-nov-1996 (kamal03)
**            In function duc_val_loc1() changed calls to db_error:
**            first param. passed is expected to be a pointer.
**            Super restrictive VAX C compiler helped to find this bug.
**	20-may-97 (hayke02)
**	    In the function duc_db_envinit(), we now get the corect case
**	    strings (du_reg_id_case, du_delim_id_case, du_real_user_case)
**	    from the remote iidbdb. The case translation flags in du_dbservice 
**	    and the case of du_usrinit and du_dba_iidbdb are also now
**	    correclty set. This allows a non SQL-92 database to be created
**	    remotely from an SQL-92 installation (and vice versa). This
**	    change fixes bug 78208.
**	14-apr-97 (stephenb)
**	    Add code to check that the fe (createdb) version is the same
**	    major version as the be (iidbms) version. Architectural
**	    problems prevent createdb from working across major releases.
** 	22-apr-1997 (wonst02)
** 	    Added '-r' and duc_rdonly_flag to support readonly databases.
** 	    Removed unused duc_validate_crdb() routine.
**	18-aug-98 (mcgem01)
**	    Add INGRES as a valid server class when creating a database.
**      21-apr-1999 (hanch04)
**          replace STindex with STchr
**      28-apr-1999 (hanch04)
**          Replaced STlcopy with STncpy
**	12-nov-1998 (kitch01)
**		Bug 89657. For SQL-92 installation ensure effective user has
**		same case as actual user if the -u flag isn't used. This
**		prevents bogus E_DU3004_NO_ALIAS_FLAG errors. Amendment to
**		duc_xlate_username().
**      30-jun-1999 (chash01)
**           RMS gateway, maybe IMS and VSAM gateways later, does
**           not have the same version as Ingres dbms. thus we escape the
**           test for RMS GW for now.
**	21-may-1999 (hanch04)
**	    Replace STbcompare with STcasecmp,STncasecmp,STcmp,STncmp
**      28-sep-2001 (stial01)
**          createdb -p... Must -p or -private (b105913)
**      21-dec-2001 (stial01)
**          Set DU_LP64 for LP64 databases, fixed references to DU_3SER_RMS
**      09-jan-2002 (stial01)
**          Call duc_create_internal_procs to create procedures (SIR 106745)
**      08-may-2002 (stial01)
**          Moved setting of DU_LP64 to back/qef/qeq qeaiproc.c
**      14-Jun-2002 (hweho01)
**          Initialize the memory pointed by CM_AttrTab into the character   
**          set before calling routine duc_get_args. (B108031) 
**	15-jul-2002 (toumi01)
**	    To fix bug 108256 (issues 11642879, 11643646, 11681618,
**	    11962563), EOS delimit iidbcapabilities fields DB_NAME_CASE,
**	    DB_DELIMITED_CASE, and DB_REAL_USER_CASE to avoid storing
**	    spurious characters.
**      21-Apr-2004(nansa02) 
**          Added support for modifying system catalogs to specified page_size.      
**      09-Jun-2004(nansa02)        
**          To fix bug 112425 initialized duc_client_name inside duc_modify.
**	29-Sep-2004 (drivi01)
**	    Updated NEEDLIBS to link dynamic library SHEMBEDLIB to replace
**	    its static libraries. On unix SHEMBEDLIB=SHQLIB. Added NEEDLIBSW
**	    hint which is used for compiling on windows and compliments NEEDLIBS.
**      07-Dec-2004 (ashco01) Bug: 113584, Problem: INGSRV3070
**           CREATEDB returns VMS error message "NONAME-F-NOMSG, message
**           number 00000004" if supplied with a non-existent default
**           database location name. Replaced the return() of du_status
**           following duc_db_boot() with PCexit(FAIL) to exit the 
**           freestanding utility correctly. 
**      23-feb-2005 (stial01)
**          Explicitly set isolation level after connect
**	11-May-2005 (kodse01)
**	    BUG 114484
**	    Added proper header file inclusions.
**	12-jul-2005 (abbjo03)
**	    Add SIflush() after printing modifying catalogs to page size
**	    message.
**	19-Oct-2005 (schka24)
**	    Cause view-base columns to return nothing (i.e. 'N') since
**	    the view-base bit has been removed from iirelation.
**	23-nov-2006 (dougi)
**	    Reinstate view_base, since bit has been added back to relstat.
**
NEEDLIBS =	DBUTILLIB SHEMBEDLIB COMPATLIB 
NEEDLIBSW = 	SHCUFLIB SHADFLIB
OWNER =		INGUSER
PROGRAM =	createdb
**/
/*
**  Forward and/or External function references.
*/
void		    duc_exit();
FUNC_EXTERN 	    DU_STATUS       duu_chkenv();
FUNC_EXTERN 	    DU_STATUS       duc_grant_select_wgo_on_cats();
FUNC_EXTERN DU_STATUS duc_modify_pagesize(i4 pagesize, bool star_flag,
				 DU_ERROR *errcb);
char		    *cmdflagarg();
STATUS    	    get_pm_params();
i4		    duc_sqlerr_handler(i4 error);
STATUS		    validate_id_case_params(DU_ENV *duc_dbenv,
			DU_ERROR *duc_errcb);
static
DU_STATUS	duc_xlate_username(DU_ENV	*duc_dbenv, 
				   bool		dash_u_specified, 
				   u_i4	idmode, 
				   DU_ERROR	*duc_errcb);
static
STATUS		duc_val_loc(DU_ENV   *duc_dbenv, 
						DUC_MODE_VECT *duc_modes); 
static
STATUS	duc_val_loc1(PTR tmpstr, PTR locname);
static 
DU_STATUS	duc_0makecdb(DU_ENV *duc_dbenv, DU_ERROR *duc_errcb);
static
DU_STATUS	duc_1create_iidd_catalogs();
static
DU_STATUS	duc_2create_iidd_ddb_catalogs();
static
DU_STATUS	duc_4init_star_catalogs(DU_ENV *duc_dbenv, DU_ERROR *duc_errcb);
static
DU_STATUS	duc_3modify_star_catalogs();
static
DU_STATUS      duc_femodify();   
static
void		duu_get_vnode(char *);
/*
** Definition of all global variables owned by this file.
*/
/*  The following global variables are used by the createdb
**  exit handler to analyze the program state.
*/
GLOBALREF DU_ENV		*Duc_dbenv; 
GLOBALREF DU_ERROR		*Duc_errcb;
/*  The following global variables are used to perform retry on certain
**  Equel statements that are thought to possibly cause deadlock.
*/
GLOBALREF i4		Duc_deadlock;
GLOBALREF i4		Duc_dlock_cnt;
GLOBALREF i4		Duc_1dlock_retry;
GLOBALREF i4		Duc_iidbdb_read;
/*  This global variable is used by the exit handler.  If it is set and
**  CREATEDB failed for any reason, then the user will be informed to run
**  DESTROYDB to undo what CREATEDB has done.
*/
GLOBALREF i4			Duc_destroydb_msg;
GLOBALREF i4			Duc_distributed_flag;
GLOBALREF DU_STARCB		Duc_star_cb;
GLOBALREF       char    iiduNoFeClients[];      /* defined in duicrdb.qsc */
GLOBALREF	bool	upg_dbg_out;		/* defined in ducommon.qsc */
GLOBALREF DUU_CATDEF         *Dus_allcat_defs[]; /*Used for modifying the system catalogs*/
/*
**	GLobal structure for db values, to pick up
**	regular/delim id preferences from iidbdb
*/
/*
**  Definition of static variables and forward static functions.
*/
  static char Duc_appl_flag[DU_APPL_FLAG_MAX];
static STATUS duc_goodClientList();
/*{
**
**  Name: main() -   main entry point to the database utility, createdb.
**
**  Description:
**	This program creates a new database.  It takes the name of
**	the new database (syntax defined below) and a series of
**	flags (also defined below).
**
**	Unless the '-r' flag is specified, the database is both physically 
** 	created, as well as entered into the database database database 
** 	relation!  The '-r' flag is used to add an existing (readonly) database
** 	to the database database.
**
**	In order to perform this command, you must be enabled by
**	having the DU_UCREATEDB bit set in the user status word
**	of the database database.
**
**
**	Usage:
**		createdb [-S] [-uname] [-p] 
** 			 [-ddb_locationname|-rrdonly_locationname]
**			 [-cckp_locationname] [-jjnl_locationname] dbname
**			 [{ client_name}]
**
**	TITAN Usage:
**		createdb [-S] [-uname] [-p] [-ddb_locationname]
**		  [-cckp_locationname] [-jjnl_locationname] ddbname/d [cdbname]
**		 [{ client_name}]
**
**      RMS Gateway Usage:
**              createdb ..... dbname/rms
**
**	Positional Parameters:
**		databasename -- the name of the database to create.
**			It must conform to all the usual rules
**			about names.  Notice that this is more
**			restrictive than UNIX usually is:  names
**			must begin with an alpha, and must be
**			composed of alphanumerics.  It may be
**			at most 32 single-byte characters long.  Underscore
**			counts as an alphabetic.
**		    TITAN -- the name of the distributed database to create.
**			It must confrom to regular local database naming 
**			standards, but also have the following characteristic:
**			    if the cdbname(coordination database name) is not 
**			    specified, the ddbname(distributed database name)
**			    may be at most 22 characters, not counting "/d" 
**			    at the end.
**                  RMS Gateway -- the name of the Gateway database to create.
**                      It must conform to regular database naming standards.
**                      The maximum length of 32 characters does not include
**                      the trailing '/rms' -- that is, you may have a
**                      maximum of 28 characters including the '/rms'.
**		-S
**			Create database database.
**
**		client -- name of front-end dictionary client
**			("ingres", "windows_4gl", "visibuild", "ingres/dbd")
**
**	Flags:
**		-uXX
**			Run as user XX (login name).  This
**			flag may only be used by the INGRES superuser.
**			Normally, the database administrator for the
**			new database is the user who performs the
**			command, but the -u flag allows INGRES to
**			give the database to someone else.  Thus, it
**			is possible for someone to be a DBA without
**			having the DU_UCREATEDB bit set.  Note, as of Jupiter,
**			user codes will no longer be allowed with this
**			flag.
**		-p
**			Make database private.	The default is for the
**			database to be globally accessable.
**
**		-ddb_locationname	The locationname for the database.
**				The corresponding area must have an ingres
**				directory.  If this parameter is not specified
**				the database is placed on the default area
** 		-rrdonly_locationname	The locationname of a readonly database.
** 				The corresponding area must have an ingres
**				directory structure for the database 
** 				(./ingres/data/default/dbname).
**		-jjnl_locationname The locationname name for the database
**				journal Similar to db_locationname
**		-cckp_locationname The locationname name for the database
**				checkpoint.  Similar to db_locationname
**              -page_size      The page size for the system catalogs for that   
**                              database  
**	Diagnostics:
**		No database name specified.
**		You are not a valid INGRES user
**			You do not have an entry in the dbdb, and can
**			not do anything with INGRES at all.
**		You are not allowed this command
**			The DU_UCREATEDB bit is not set in your users
**			status in the dbdb
**		You may not use the -u flag
**			Only the INGRES superuser may say s/he is
**			someone else.
**		%s already exists
**			The database is already catalogued in the dbdb
**		Directory for %s already exists
**			Directory for db exists, but no dbdb
**			entry...weird situation 
**			requires manual cleanup
**
**  Inputs:
**      argc                            The command line argument count.
**	argv				The command line argument vector.
**
**  Outputs:
**	Returns:
**	    not applicable
**	Exceptions:
**
**  Side Effects:
**	  Creates a database in the specified location and updates the system
**	catalogs in the dbdb to reflect this.
**
**  History:
**      19-Sep-86 (ericj)
**          Rewritten for Jupiter.
**      19-oct-88 (alexc)
**          Modified for TITAN.
**      30-mar-89 (alexc)
**          Added dub_init_lk() call in the beginning of main() to
**	    insure user having the appropriate privileges before
**	    proceeding with CREATEDB.
**      22-mar-90 (fourt)
**          For a Gateway database, have initial 'Creating' msg printed
**          by duc_gmsg_creating().
**      27-aug-1990 (pete)
**          call duu_chkenv() at startup to check if can write temp files.
**      24-may-1989 (teg)
**          took DI and DI related processing out of Createdb.  Also made
**	    connections to iidbdb as $ingres.
**	25-jul-90 (teresa)
**	    merged in gateway and STAR changes from 6.3/02 (after final
**	    codefreeze)
**	14-dec-90 (ralph)
**	    Don't connect as -u$ingres when invoking iiqef_alter_db
**	20-sep-91 (teresa)
**	    drop in ingres63p change 262603:
**	      6-jun-91 (rickh)
**		Move the setting of du_dbservice to argument parsing time.
**		otherwise, the wrong dbservice value is written to the
**		configuration file & FINDDBS incorrectly recovers the database.
**	      21-jun-91 (teresa)
*		Modified createdb to connect to star server to call upgradefe on
**		creation of a DDB.
**	      27-jun-91 (rickh)
**		Make sure that the extended catalogs are created, where 
**		relevant, by the gateway server.  The INGRES server may no 
**		longer access RMS databases.
**	16-apr-92 (teresa)
**	    changed "for (;;)" control loop to "do { } while (0)" to make sun4
**	    compiler stop complaining.
**	11-jan-93 (jpk)
**	    added call to get PM parameters for FIPS -- "should we
**	    create a FIPS database?".
**	10-feb-1993 (jpk)
**	    support createdb over the net.
**	05-apr-1993 (ralph)
**	    DELIM_IDENT: Set the case translation flags in dbservice
**	24-July-1993 (jpk)
**	    Set values of dbservice correctly for DDB, CDB, LDB.
**	27-jul-193 (ralph)
**	    DELIM_IDENT:
**	    Add support for DU_REAL_USER_MIXED flag.
**      15-nov-93 (robf)
**        Add check to make sure PM load worked, otherwise  could be
**        running with inappropriate settings (like security attributes)
**	29-Jan-1994 (jpk)
**	    Added debugging output support for upgradedb.
**	15-Nov-1994 (forky01)
**	    altered how duc_db_boot is called to provide interface from
**	    du_get_dbid for creating cdb
**	15-Nov-1994 (nanpr01)
**	    Added SQL GRANT statement to allow/deny default access to
**          public. Bug #60370. 
**	31-Jan-1995 (reijo01)
**	    Added table creations for the IMA tables,
**	    iigw07_relation, iigw07_attribute, iigw07_index,
**	    if the database is owned by $ingres. Bug #66605
**	13-sep-96 (nick)
**	    Add in call to EXsetclient() so that we actually catch
**	    user interrupts.
**	31-oct-1997 (nanpr01)
**	    Added +w in the connect flag to avoid E_US0014 message.
**      30-jun-1999 (chash01)
**           RMS gateway, maybe IMS and VSAM gateways later, does
**           not have the same version as Ingres dbms. thus we escape the
**           test for RMS GW for now.
**	24-mar-2001 (stephenb)
**	    Add support for Unicode collation.
**       7-Sep-2004 (hanal04) Bug 48179 INGSRV2543
**          Use new -Z flag to prevent ING_SET values that require user to
**          be the DBA from generating unexpected errors.
**      21-Apr-2004 (nansa02)
**          Added support for modifying system catalogs page_size.   
**	15-Feb-2005 (gupsh01)
**	    Added support for unicode normal form C (NFC).
**	21-feb-05 (inkdo01)
**	    Fix tiny bug that made all dbs look like Unicode NFC.
**	13-Apr-2005 (thaju02)
**	    For distributed database, call du_get_dbid() in
**	    a multi-stmt xaction such that we'll cleanup properly
**	    if failure occurs. (B89866)
**	19-Apr-2007 (bonro01)
**	   Detect internal error in duc_init() and exit with
**	   a bad return code.
*/
main(argc, argv)
i4	    argc;
char	    *argv[];
{
  DU_ENV duc_dbenv;
    DUC_MODE_VECT	duc_modes;
    DU_ERROR		duc_errcb;
  i4 row_cnt;
    EX_CONTEXT		context;
  char *dba_override = "";
    char		override[64];
    char		tmp_buf[DU_APPL_FLAG_MAX];
    DU_STATUS		duc_db_envinit();
    DU_STATUS		duc_1dbdb_init();
    DU_STATUS		duc_get_args();
    DU_STATUS		duc_init();
    DU_STATUS		duc_makedb();
    STATUS		du_ex_handler();
    i4			duc_ingerr_handler();
    /* construct name of form node::database, so 2 for "::" */
  char node_dbname[GCC_L_NODE + DB_MAXNAME + 2 + 1];
  char dbms_vers[32];
    i4			idx1, idx2;
    char		fe_vers[] = GV_VER;
  char rms_gw_dbname[DB_MAXNAME + 4 + DU_TAIL_LEN];
    (void)EXsetclient(EX_INGRES_TOOL);
    MEadvise(ME_INGRES_ALLOC); 
    /* 
    ** Set an exception handler so that we can catch user interrupts
    ** and call an exit handler.
    */
    if (EXdeclare(du_ex_handler, &context) != OK)
    {
	duc_exit(&duc_errcb);
    }
    /* Initialize global variables and the application flag. */
    Duc_iidbdb_read	= FALSE;
    Duc_deadlock	= FALSE;
    Duc_1dlock_retry	= FALSE;
    Duc_dlock_cnt	= 0;
    Duc_destroydb_msg	= FALSE;
    Duc_errcb		= &duc_errcb;
    upg_dbg_out		= FALSE;	/* I am not upgradedb */
    do				/* control loop */
    {
	/* Initialize the createdb system utility data */
	if (duc_init(&duc_modes, &duc_dbenv, &duc_errcb) != E_DU_OK)
	{
	    duc_errcb.du_status=E_DU_IERROR;
	    break;
	}
	/* get PM parameters */
	if(get_pm_params(&duc_dbenv, &duc_errcb)!=OK)
	{
    		duc_errcb.du_status=E_DU_IERROR;
		break;
	}
	/* Set up the Equel error handling procedure. */
	IIseterr(duc_ingerr_handler);
	/* Tell Equel to remember the user password, this saves
	** extra prompts (everytime we connect/disconnect)
	*/
/* # line 752 "ducreate.sc" */	/* set_sql */
  {
    IILQssSetSqlio(15,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
  }
/* # line 753 "ducreate.sc" */	/* host code */
	/* Process the command line to get all the necessary parameters */
	if (duc_get_args(argc, argv, &duc_modes, &duc_dbenv, &duc_errcb) 
	    != E_DU_OK)
	    break;
        /* check for write access for creating temp files */
        if (duu_chkenv(DU_ENV_TWRITE) != E_DU_OK)
	{
            du_error(&duc_errcb, E_DU3513_NO_WRITE_ACCESS, 2,
                            0, ERx("Createdb"));
            break;
	}
	/* Issue an informative message to the user that the database is being
	** created.  This also marks the point of no return.  At this point
	** physical changes will be made to the dbdb data and the file system
	** that cannot be backed out.
	**
	** While we're in here, set the dbservice based on whether this
	** is a distributed or gateway database.
	*/
	duc_dbenv.du_dbservice = 0;
	Duc_distributed_flag = duc_modes.duc_star_flag;
	STcopy( duc_dbenv.du_dbname, duc_dbenv.dbnamePlusService );
	STcopy( DU_DBDBNAME, duc_dbenv.dbdbnamePlusService );
	if (Duc_distributed_flag)
	{
	    if((du_error(&duc_errcb, I_DU0500_STAR_UTIL_INTRO, 4,
		 0, DU_0CREATING, 0, duc_dbenv.du_ddbname))
		!= E_DU_OK)
		duc_exit(&duc_errcb);
	    /* Specify that this is a Coordinator database */
	    duc_dbenv.du_dbservice |= DU_2SER_CDB;
	}
        else  if (duc_modes.duc_gw_flag)
	{
	    /* type beginning msg for creating a gateway database */
	    duc_gmsg_creating(duc_modes.duc_gw_flag,
			      &duc_dbenv, &duc_errcb);
	/* Pick up Gateway's particular service */
	    duc_dbenv.du_dbservice = duc_gset_service(duc_modes.duc_gw_flag,
			&duc_errcb);
	    if (duc_dbenv.du_dbservice & DU_3SER_RMS)
	    {
		( void ) STcat(duc_dbenv.dbnamePlusService, DU_RMSTAIL);
		( void ) STcat(duc_dbenv.dbdbnamePlusService, DU_RMSTAIL);
	    }
	}
	else
	{
	    if ((du_error(&duc_errcb, I_DU0010_UTIL_INTRO, 4,
	    		   0, DU_0CREATING, 0, duc_dbenv.du_dbname))
		!= E_DU_OK)
		duc_exit(&duc_errcb);
	}
	/* set unicode types flag in dbservice */
	if (duc_modes.duc_national_flag)
	    duc_dbenv.du_dbservice |= DU_UTYPES_ALLOWED;
	if (duc_modes.duc_unicode_nfc)
            duc_dbenv.du_dbservice |= DU_UNICODE_NFC;
	/* Set the case translation flags in dbservice */
	if (duc_dbenv.du_delim_id_case[0] == 'M')
	    duc_dbenv.du_dbservice |= DU_DELIM_MIXED;
	else if (duc_dbenv.du_delim_id_case[0] == 'U')
	    duc_dbenv.du_dbservice |= DU_DELIM_UPPER;
	if (duc_dbenv.du_reg_id_case[0] == 'U')
	    duc_dbenv.du_dbservice |= DU_NAME_UPPER;
	if (duc_dbenv.du_real_user_case[0] == 'M')
	    duc_dbenv.du_dbservice |= DU_REAL_USER_MIXED;
        if (duc_dbenv.du_real_user_case[0] == 'U')
        {
            CVupper(duc_dbenv.du_usrinit);
            CVupper(duc_dbenv.du_dba_iidbdb);
        }
	if (duc_modes.duc_dbdb_flag)
	{
	    duc_boot_dbdb(&duc_dbenv, &Duc_appl_flag);
	    /* check user status if we are NOT B1 and do STAR specific
	    ** iidbdb work */
	    if (duc_dbdb_envinit(&duc_modes, &duc_dbenv, &duc_errcb) != E_DU_OK)
                break;
	}
	else
	{
	    /* Get the information from the dbdb necessary to create the
	    ** database.  This information includes the user's status and
	    ** the areas mapped to by any specified locations.
	    */
	    CVla((i4) DBA_CREATEDB, tmp_buf);
	    (void)STpolycat(2, DU_1APPL_FLAG, tmp_buf, Duc_appl_flag);
	    /* Open the dbdb */
	    if (STcompare(duc_dbenv.du_usrinit, duc_dbenv.du_xusrname))
	    {
		STpolycat(2, "-u", duc_dbenv.du_unorm_usr, override);
		dba_override = override;
	    }
	    /* handle createdb over the net */
	    if (STlength(duc_dbenv.du_nodename) > 0)
	    {
		STpolycat(3, duc_dbenv.du_nodename, ERx("::"),
		    duc_dbenv.dbdbnamePlusService, node_dbname);
	    }
	    else
	    {
		STcopy(duc_dbenv.dbdbnamePlusService, node_dbname);
	    }
/* # line 930 "ducreate.qsc" */	/* ingres */
  {
    IIingopen(0,Duc_appl_flag,DU_1DBA_DBDB_FLAG,(char *)"-Z",node_dbname,
    (char *)0);
  }
/* # line 932 "ducreate.qsc" */	/* host code */
	    Duc_iidbdb_read = TRUE;
	    duc_dbenv.du_dbopen	=   TRUE;
/* # line 935 "ducreate.qsc" */	/* set */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session isolation level serializable");
    IIsyncup((char *)0,0);
  }
/* # line 937 "ducreate.qsc" */	/* host code */
	    /*
	    ** We need to check that the DBMS version is that same as our
	    ** version (creating a database over the net to a different 
	    ** version of Ingres produces a "mix-and-match" database that
	    ** is no use to either release.
	    ** The version string starts with an optional string of
	    ** characters followed by the major version and a forward slash 
	    ** (/). Since we are only interested in the major release version
	    ** we will find the slash and compare strings to that point.
	    ** we assume createdb between minor releases will be compatable.
	    */
            /* 30-jun-1999 (chash01)
            ** However, RMS gateway, maybe IMS and VSAM gateways later, does
            ** not have the same version as Ingres dbms. thus we escape the
            ** test for RMS GW for now.
            */
	    if ( (duc_dbenv.du_dbservice & DU_3SER_RMS) == 0 )
            {
/* # line 955 "ducreate.qsc" */	/* retrieve */
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
/* # line 956 "ducreate.qsc" */	/* host code */
		idx1 = STchr(fe_vers, '/') - fe_vers;
		idx2 = STchr(dbms_vers, '/') - dbms_vers;
	        if (idx1 != idx2 || STbcompare(fe_vers, idx1, dbms_vers, 
		    idx2, FALSE) != 0)
	        {
		    /* not the same version */
		    du_error(&duc_errcb, E_DU351B_INCOMPATIBLE_VERSION, 4, 0, 
		        fe_vers, 0, dbms_vers);
		    SIprintf("%s\n", duc_errcb.du_errmsg);
/* # line 965 "ducreate.qsc" */	/* exit */
  {
    IIexit();
  }
/* # line 966 "ducreate.qsc" */	/* host code */
		    PCexit(FAIL);
	        }
	    }
	    /* get info from iidbdb */
	    if (duc_db_envinit(&duc_modes, &duc_dbenv, &duc_errcb) != E_DU_OK)
		break;
/* # line 975 "ducreate.qsc" */	/* exit */
  {
    IIexit();
  }
/* # line 977 "ducreate.qsc" */	/* ingres */
  {
    IIingopen(0,Duc_appl_flag,dba_override,node_dbname,(char *)"-Z",(char *)0);
  }
/* # line 979 "ducreate.qsc" */	/* set */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session isolation level serializable");
    IIsyncup((char *)0,0);
  }
/* # line 981 "ducreate.qsc" */	/* host code */
	    /* if this is a distributed database, put an entry into iidatabase
	    ** for the DDB, create CDB, mark the CDB as a CDB AND put an entry
	    ** into iistar_cdbs to describe this new distributed database.
	    */
	    if (Duc_distributed_flag)
	    {
/* # line 987 "ducreate.qsc" */	/* begin transaction */
  {
    IIxact(0);
  }
/* # line 988 "ducreate.qsc" */	/* host code */
	        if (du_get_dbid(&duc_modes, &duc_dbenv, FALSE, &duc_errcb)
		    != E_DU_OK)
		{
/* # line 992 "ducreate.qsc" */	/* abort */
  {
    IIxact(2);
  }
/* # line 992 "ducreate.qsc" */	/* host code */
		    break;
		}
/* # line 994 "ducreate.qsc" */	/* end transaction */
  {
    IIxact(1);
  }
/* # line 995 "ducreate.qsc" */	/* host code */
	    }
	    else
	    /*	Create the core catalogs and update the iidbdb. */
	    {
		if (duc_db_boot(&duc_errcb, &duc_dbenv, TRUE) != E_DU_OK)
		    break;
	    }
/* # line 1002 "ducreate.qsc" */	/* exit */
  {
    IIexit();
  }
/* # line 1003 "ducreate.qsc" */	/* host code */
	    /*
	    ** Open the new database, unless it's readonly
	    */
	    if (! duc_modes.duc_rdonly_flag)
	    {
	    	/* handle createdb over the net */
	    	if (STlength(duc_dbenv.du_nodename) > 0)
	    	{
		    STpolycat(3, duc_dbenv.du_nodename, ERx("::"),
		    	duc_dbenv.dbnamePlusService, node_dbname);
	    	}
	    	else
	    	{
		    STcopy(duc_dbenv.dbnamePlusService, node_dbname);
	    	}
/* # line 1018 "ducreate.qsc" */	/* ingres */
  {
    IIingopen(0,(char *)"+w",(char *)"-l",Duc_appl_flag,DU_1DBA_DBDB_FLAG,
    (char *)"-Z",node_dbname,(char *)0);
  }
/* # line 1020 "ducreate.qsc" */	/* host code */
	    	duc_dbenv.du_dbopen = TRUE;
	    }
	}
	/* 
	** Use EQUEL/ESQL to make the rest of the database configuration,
	** unless this is a readonly database. 
	*/
	if (! duc_modes.duc_rdonly_flag)
	{
	    if (duc_makedb(&duc_modes, &duc_dbenv, &duc_errcb) != E_DU_OK)
	    	break;
	    /* Create the IMA system catalogs if user = $ingres */
	    if (STncasecmp(duc_dbenv.du_xusrname,
		ERx("$ingres"), STlength( ERx("$ingres")) ) == 0)
	    {
	    	if (duc_create_IMA_tables() != E_DU_OK)
	    	    break;
	    }
	}
	/* Mark the database as operative in the database catalog of the
	** dbdb.
	*/
	if (duc_modes.duc_dbdb_flag)
	{
	    /* mark the database operational in the iidatabase catalog.
	    ** Note: we cannot call procedure iiqef_alter_db to do this for
	    **	     the iidbdb because the procedure will hang on a call to
	    **	     the lock manager.  So just manually update the iidatabase
	    **	     catalog.
	    */
	    /*
	    ** Update iidatabase and config file with new access and service.
	    */
	    {
		duc_dbenv.du_access = ((duc_dbenv.du_access | DU_OPERATIVE)
		    & ~DU_CREATEDB);
		IIlq_Protect(TRUE);
/* # line 1065 "ducreate.qsc" */	/* replace */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"replace iidatabase(access=");
    IIputdomio((short *)0,1,30,4,&duc_dbenv.du_access);
    IIwritio(0,(short *)0,1,32,0,(char *)")where iidatabase.name=");
    IIputdomio((short *)0,1,32,0,duc_dbenv.du_dbname);
    IIsyncup((char *)0,0);
  }
/* # line 1067 "ducreate.qsc" */	/* inquire_equel */
  {
    IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
  }
/* # line 1068 "ducreate.qsc" */	/* host code */
		if (row_cnt != 1)
		{
		    /* Bad replace to table, %0c, in database, %1c. */
		    du_error(&duc_errcb, E_DU3201_BAD_REPLACE, 4,
			     0, "iidatabase", 0, DU_DBDBNAME);
		    break;
		}		
		IIlq_Protect(FALSE);
	    };
	}
	else
	{
	    if (duc_dbenv.du_dbopen)
	    {
/* # line 1083 "ducreate.qsc" */	/* exit */
  {
    IIexit();
  }
/* # line 1084 "ducreate.qsc" */	/* host code */
	    }
	    duc_dbenv.du_dbopen	= FALSE;
	    /*
	    ** Don't connect as -u$ingres when invoking iiqef_alter_db
	    */
	    /* handle createdb over the net */
	    if (STlength(duc_dbenv.du_nodename) > 0)
	    {
		STpolycat(3, duc_dbenv.du_nodename, ERx("::"),
		    duc_dbenv.dbdbnamePlusService, node_dbname);
	    }
	    else
	    {
		STcopy(duc_dbenv.dbdbnamePlusService, node_dbname);
	    }
/* # line 1101 "ducreate.qsc" */	/* ingres */
  {
    IIingopen(0,Duc_appl_flag,node_dbname,(char *)"-Z",(char *)0);
  }
/* # line 1102 "ducreate.qsc" */	/* host code */
	    duc_dbenv.du_dbopen = TRUE;
/* # line 1104 "ducreate.qsc" */	/* set */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session isolation level serializable");
    IIsyncup((char *)0,0);
  }
/* # line 1106 "ducreate.qsc" */	/* host code */
            /* Add the private / public access in the iidbpriv */
	    {
  char sbuffer[255];
              if (duc_modes.duc_private_flag)
              {
                STprintf(sbuffer,ERx("GRANT NOACCESS ON DATABASE %s TO PUBLIC"),
                         duc_dbenv.du_dbname);
/* # line 1105 "ducreate.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
  }
/* # line 1106 "ducreate.sc" */	/* host code */
              }
	    }
	    /* Update iidatabase and config file with new access and service. */
	    {
  char dbname[DB_MAXNAME+1];
  i4 access_set;
  i4 access_clear;
		access_set = DU_OPERATIVE;
		access_clear = DU_CREATEDB;
		(void) STcopy (duc_dbenv.du_dbname, dbname);
/* # line 1118 "ducreate.sc" */	/* execute procedure */
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
/* # line 1125 "ducreate.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
		    /* we were unable to execute the internal procedure */
		    duc_sqlerr_handler(E_DU200C_BAD_IPROC_EXECUTE);
		}
	    };
	    if (Duc_distributed_flag)
	    {
		/* we are making a distributed database. We just finished
		** updating the CDB.  Now we need to update the iidatabase
		** tuple for the DDB.  The DDB does not have a config file
		** assocaited with it, so we need to update iidatabase
		** directly instead of using iiqef_alter_db.  This requires
		** system catalog update privilege.
		** NOTE:  I do not think this will work on B1.  It is
		**	  probably not possible to create distriubted DBs on
		**	  B1 installations unless we get some sort of QEF
		**	  internal procedure to insert tuples into iidatabase
		**	  and another to update the DDB iidatabase tuple
		**	  with operative access.
		*/
		duc_dbenv.du_access = ((duc_dbenv.du_access | DU_OPERATIVE)
		    & ~DU_CREATEDB);
		IIlq_Protect(TRUE);
/* # line 1165 "ducreate.qsc" */	/* replace */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"replace iidatabase(access=");
    IIputdomio((short *)0,1,30,4,&duc_dbenv.du_access);
    IIwritio(0,(short *)0,1,32,0,(char *)")where iidatabase.name=");
    IIputdomio((short *)0,1,32,0,duc_dbenv.du_ddbname);
    IIsyncup((char *)0,0);
  }
/* # line 1167 "ducreate.qsc" */	/* inquire_equel */
  {
    IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
  }
/* # line 1168 "ducreate.qsc" */	/* host code */
		IIlq_Protect(FALSE);
		if (row_cnt != 1)
		{
		    /* Bad replace to table, %0c, in database, %1c. */
		    du_error(&duc_errcb, E_DU3201_BAD_REPLACE, 4,
			     0, "iidatabase", 0, DU_DBDBNAME);
		    break;
		}
	    }  /* end if making a distributed database */
 	} /* end if NOT iidbdb */
       if(duc_modes.duc_page_size)       /*For modifying the system catalogs to page_size*/
         {
            SIprintf("\n\nModifying All System Catalogs to %d pagesize. . .\n\n",duc_modes.duc_page_size); 
	    SIflush(stdout);
IIlq_Protect(TRUE);    
/* # line 1187 "ducreate.qsc" */	/* ingres */
  {
    IIingopen(0,Duc_appl_flag,(char *)"+w",(char *)"-l",DU_1DBA_DBDB_FLAG,
    duc_dbenv.du_dbname,(char *)0);
  }
/* # line 1188 "ducreate.qsc" */	/* host code */
           duc_dbenv.du_dbopen = TRUE;
/* # line 1190 "ducreate.qsc" */	/* set */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session isolation level serializable");
    IIsyncup((char *)0,0);
  }
/* # line 1192 "ducreate.qsc" */	/* host code */
          if (duc_modify_pagesize(duc_modes.duc_page_size,
                        ((duc_dbenv.du_dbservice&DU_1SER_DDB) != 0),
                        &duc_errcb) != E_DU_OK)
              break;
          if (duc_femodify(&duc_modes, &duc_dbenv, &duc_errcb) != E_DU_OK)
              break;              
IIlq_Protect(FALSE); 
/* # line 1201 "ducreate.qsc" */	/* exit */
  {
    IIexit();
  }
/* # line 1202 "ducreate.qsc" */	/* host code */
          duc_dbenv.du_dbopen = FALSE;
         }
    } while (0);	/* end of control loop */
    duc_exit(&duc_errcb);
}
/*{
** Name: duc_unique24()	- create uniqueness test string
**
** Description:
**	This routine will check if it is necessary to enforce the 24 character
**	uniqueness requirement for database names, due to an LDB architecture
**	limitation i.e. database names need to be unique in first 24 characters
**	If this is true then a "quel" pattern matching constant is created to
**	be used to determine if any databases exist which could cause problems.
** Inputs:
**	in_nameptr			ptr to proposed database name, must be
**					at least DB_OLDMAXNAME + 1
**
**	out_size                        size of out_nameptr buffer
** Outputs:
**	out_nameptr			ptr to pattern matching name to test
**					for uniqueness
**	Returns:
**		TRUE - if name longer than 24 characters
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	9-dec-92 (ed)
**	    Force 32 character database names to be unique in first 24 characters
**	09-nov-93 (rblumer)
**	    in_nameptr is usually initialized to NULLs, not spaces,
**	    so check for NULLs in addition to spaces.
*/
bool
duc_unique24(in_nameptr, out_nameptr, out_size)
	char		*in_nameptr;
	char		*out_nameptr;
	i4		out_size;
{
    if (   (in_nameptr[DB_OLDMAXNAME] == ' ') 
	|| (in_nameptr[DB_OLDMAXNAME] == NULLCHAR))
	return(FALSE);
    MEfill((u_i2)out_size, (u_char)' ', (PTR)out_nameptr);
    MEcopy((PTR)in_nameptr, (u_i2)DB_OLDMAXNAME, (PTR)out_nameptr);
    out_nameptr[DB_OLDMAXNAME] = '*';		/* quel wild card */
    out_nameptr[DB_OLDMAXNAME+1] = (u_char)0;	/* null terminate */
    return(TRUE);
}
/*{
** Name: duc_db_envinit()	- initialize the database environment()
**
** Description:
**        This routine is used to initialize the database utility
**	environment, duc_dbenv, for databases other than the "dbdb".
**	This is done by accessing the appropriate info from the
**	"dbdb".  This routine also validates some of the info that
**	has already been initialized in the duc_dbenv struct.
**
**	  Note 1:   This routine assumes that the database, "dbdb",
**		    has already been opened.
**
** Inputs:
**	duc_dbenv			Ptr to a DU_ENV struct.
**	    .du_usrinit			The user's login name.
**	    .du_dbname			The name of the database to be created.
**	    .du_xusrname		User's effective name (untranslated).
**					This will different from the
**					.du_usrinit field only when the -u flag
**					is used or the "dbdb" is being created
**					by somebody other than DU_DBDB_DBA.
**	    .du_dba			The dba for this database.
**	    .du_dbloc			A DU_LOC struct for the default
**					database location.
**		.du_locname		The default database location name.
**	    .du_jnlloc			The journaling location for this db.
**		.du_locname		The location's name.
**	    .du_ckploc			The checkpointing location for this db.
**		.du_locname		The location's name.
**	    .du_sortloc			The default sort location for this db.
**		.du_locname		The location's name.
**	duc_errcb			Ptr to the error control block.
**
** Outputs:
**	*duc_dbenv
**	    .du_dbloc			Default database location.
**		.du_status		Status of the proposed db location.
**		.du_area		The area associated with this location.
**	    .du_jnlloc			Default journaling location.
**		.du_status		Status of the proposed journal location.
**		.du_area		The area associated with this location.
**	    .du_ckploc			Default checkpointing location.
**		.du_status		Status of the proposed checkpoint
**					location.
**		.du_area		The area associated with this location.
**	    .du_sortloc			Default sort location.
**		.du_status		Status of the proposed sort
**					location.
**		.du_area		The area associated with this location.
**      *duc_errcb                      If an error occurs, this struct will be
**					set by a call to du_error().
**	Returns:
**	    E_DU_OK			Completed successfully.
**	    E_DU3001_USER_NOT_KNOWN	The user's login name couldn't be found
**					in the "user" catalog of the "dbdb".
**	    E_DU3002_ALIAS_NOT_KNOWN	The effective user's name, .du_dba_iidbdb,
**					couldn't be found in the "user" catalog
**					of the "dbdb".
**	    E_DU3021_DB_EXISTS_CR	This database, .du_dbname, already
**					exists.
**	    E_DU3040_NO_DEF_DBLOC	The default database location,
**					.du_dbloc, doesn't exist in the
**					"iilocations" catalog of the "dbdb".
**	    E_DU3041_NO_JNLLOC		The journaling location for this db,
**					.du_jnlloc, doesn't exist in the
**					"iilocations" catalog of the "dbdb".
**	    E_DU3042_NO_CKPLOC		The checkpointing location for this
**					db, .du_ckploc, doesn't exist in the
**					"iilocations" catalog of the "dbdb".
**	    E_DU3043_NO_SORTLOC		The sorting location for this
**					db, .du_sortloc, doesn't exist in the
**					"iilocations" catalog of the "dbdb".
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      13-Oct-86 (ericj)
**          Initial creation.
**      17-feb-1989 (EdHsu)
**          Add code to support online backup
**	26-jul-9189 (Derek)
**	    Delete all non-star code.  Not needed anymore because
**	    internal database procedure does all the normal work.
**	25-jul-90 (teresa)
**	    Put back logic that validate's user's right to create the
**	    db.  This is still used in the case where we are NOT a B1
**	    installation.  (I have currently #if 0 this code out, till
**	    I work out with SCF exactly how to determine if this is B1 or
**	    not.)
**	03-oct-90 (teresa)
**	    fix bug 33626 (do not check for ddb/cdb in iidatabase unless
**	    we are building a distributed database.
**	05-nov-90 (teresa)
**	    add call to duc_procedure() to assure that all internal procedures
**	    createdb will use actually exist.
**	30-may-91 (teresa)
**	    fix bug where createdb was not correctly detecting if the DB already
**	    exists in the installation.
**	9-dec-92 (ed)
**	    Force 32 character database names to be unique in first 24 characters
**	11-jan-93 (jpk)
**	    add space in variables for null byte termination
**	09-nov-93 (rblumer)
**	    Removed uses of du_dba and du_usrname; replaced them with
**	    du_dba_target, du_xusrname, and du_unorm_usr.
**	02-nov-1995 (angusm)
**		Add validation for location names here. We could use the
**		PM stuff, but that is largely irrelevant to this since
**		database names are forced lowercase, and location-names
**		should comply with the defaults for 'iidbdb'. We should
**		therefore wait until here before validating so that
**		we have all the right flags from 'iidbdb'. Bug 71173.
**	13-may-1997 (nanpr01)
**	    If user doesnot have security permission stop him creating
**	    before.
*/
DU_STATUS
  duc_db_envinit(duc_modes, duc_dbenv, duc_errcb)
   DUC_MODE_VECT	*duc_modes;
  DU_ENV *duc_dbenv;
   DU_ERROR		*duc_errcb;
  {
    DB_STATUS		status;
    char exist_iistar[DB_MAXNAME + 4];
    i4			found_iistar = FALSE;
    char dummy_arg[DB_MAXNAME + 1];
    char unique24_arg[DB_MAXNAME + 1];
    i4			found	= FALSE;
    bool		unique_found	= FALSE;
    /* Determine if this is a valid Ingres user and if so, get their
    ** Ingres user status.
    */
/* # line 1396 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(du_usrstat=iiuser.status)where iiuser.name=");
      IIputdomio((short *)0,1,32,0,duc_dbenv->du_usrinit);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&duc_dbenv->du_usrstat);
        if (IIerrtest() != 0) goto IIrtB2;
        {
/* # line 1399 "ducreate.qsc" */	/* host code */
	found	= TRUE;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE2:;
    }
/* # line 1401 "ducreate.qsc" */	/* host code */
    if (!found)
    {
	return(du_error(duc_errcb,
			E_DU3001_USER_NOT_KNOWN, 2, 0, duc_dbenv->du_usrinit));
    }
    else
    {
	found	= FALSE;
    }
    /* If the user is trying to access the database with an alias, verify
    ** that the alias user is known to Ingres.  Otherwise the user name of
    ** the process becomes the effective user.
    */
    if (STcompare(duc_dbenv->du_xusrname, duc_dbenv->du_usrinit))
    {
#if 0
/* # line 1419 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(dummy_arg=iiuser.name)where iiuser.name=");
      IIputdomio((short *)0,1,32,0,duc_dbenv->du_xusrname);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,dummy_arg);
        if (IIerrtest() != 0) goto IIrtB3;
        {
/* # line 1422 "ducreate.qsc" */	/* host code */
	    found   = TRUE;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE3:;
    }
/* # line 1425 "ducreate.qsc" */	/* host code */
	if (!found)
	{
	    return(du_error(duc_errcb, E_DU3002_ALIAS_NOT_KNOWN,
			    2, 0, duc_dbenv->du_xusrname));
	}
	else
	{
	    found   = FALSE;
	}
#endif
	if ((duc_dbenv->du_usrstat & DU_USECURITY) == 0)
	    return(du_error(duc_errcb, E_DU3004_NO_ALIAS_FLAG, 0));
    }
    /* Get all the information from the database catalog pertaining to this
    ** database.
    */
/* # line 1443 "ducreate.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"range of d=iidatabase");
      IIsyncup((char *)0,0);
    }
/* # line 1444 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(dummy_arg=d.name)where d.name=");
      IIputdomio((short *)0,1,32,0,duc_dbenv->du_dbname);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,dummy_arg);
        if (IIerrtest() != 0) goto IIrtB4;
        {
/* # line 1446 "ducreate.qsc" */	/* host code */
	found	= TRUE;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE4:;
    }
/* # line 1448 "ducreate.qsc" */	/* host code */
    if (found)
    {
	/* need to modify this to take care of cdb & ddb already exist
	** message
	*/
	/* The database, %0c, already exists. */
	return(du_error(duc_errcb, E_DU3021_DB_EXISTS_CR, 2,
			0, duc_dbenv->du_dbname));
    }
    /* Enforce database name uniqueness requirement
    */
    if (duc_unique24(duc_dbenv->du_dbname, unique24_arg, DB_MAXNAME))
    {
/* # line 1461 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(dummy_arg=d.name)where d.name=");
      IIputdomio((short *)0,1,32,0,unique24_arg);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,dummy_arg);
        if (IIerrtest() != 0) goto IIrtB5;
        {
/* # line 1463 "ducreate.qsc" */	/* host code */
		unique_found	= TRUE;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE5:;
    }
/* # line 1465 "ducreate.qsc" */	/* host code */
    }
    Duc_distributed_flag = duc_modes->duc_star_flag;
    if (duc_modes->duc_star_flag)
    {
/* # line 1470 "ducreate.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"range of d=iidatabase");
      IIsyncup((char *)0,0);
    }
/* # line 1472 "ducreate.qsc" */	/* host code */
	/* TITAN: check to see if the CDB and DDB is already exist in
	** "iidatabase"
	*/
	found = FALSE;
/* # line 1476 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(dummy_arg=d.name)where d.name=");
      IIputdomio((short *)0,1,32,0,duc_dbenv->du_ddbname);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,dummy_arg);
        if (IIerrtest() != 0) goto IIrtB6;
        {
/* # line 1478 "ducreate.qsc" */	/* host code */
	    found	= TRUE;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE6:;
    }
/* # line 1481 "ducreate.qsc" */	/* host code */
	    /* Enforce database name uniqueness requirement
	    */
	if (!unique_found 
	    &&
	    duc_unique24(duc_dbenv->du_ddbname, unique24_arg, DB_MAXNAME))
	{
/* # line 1487 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(dummy_arg=d.name)where d.name=");
      IIputdomio((short *)0,1,32,0,unique24_arg);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE7;
IIrtB7:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,dummy_arg);
        if (IIerrtest() != 0) goto IIrtB7;
        {
/* # line 1489 "ducreate.qsc" */	/* host code */
		unique_found	= TRUE;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE7:;
    }
/* # line 1491 "ducreate.qsc" */	/* host code */
	}
/* # line 1493 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(dummy_arg=d.name)where d.name=");
      IIputdomio((short *)0,1,32,0,duc_dbenv->du_cdbname);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE8;
IIrtB8:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,dummy_arg);
        if (IIerrtest() != 0) goto IIrtB8;
        {
/* # line 1495 "ducreate.qsc" */	/* host code */
	    found	= TRUE;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE8:;
    }
/* # line 1497 "ducreate.qsc" */	/* host code */
	    /* Enforce database name uniqueness requirement
	    */
	if (!unique_found 
	    &&
	    duc_unique24(duc_dbenv->du_cdbname, unique24_arg, DB_MAXNAME))
	{
/* # line 1503 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(dummy_arg=d.name)where d.name=");
      IIputdomio((short *)0,1,32,0,unique24_arg);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE9;
IIrtB9:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,dummy_arg);
        if (IIerrtest() != 0) goto IIrtB9;
        {
/* # line 1505 "ducreate.qsc" */	/* host code */
		unique_found	= TRUE;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE9:;
    }
/* # line 1507 "ducreate.qsc" */	/* host code */
	}
	found_iistar = FALSE;
/* # line 1510 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(exist_iistar=iitables.table_name)where iitables.table_name=\"\
iistar_cdbs\" or iitables.table_name=\"IISTAR_CDBS\"");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE10;
IIrtB10:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 4-1,exist_iistar);
        if (IIerrtest() != 0) goto IIrtB10;
        {
/* # line 1514 "ducreate.qsc" */	/* host code */
	    found_iistar = TRUE;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE10:;
    }
/* # line 1517 "ducreate.qsc" */	/* host code */
	if (found_iistar)
	{
/* # line 1519 "ducreate.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"range of d=iistar_cdbs");
      IIsyncup((char *)0,0);
    }
/* # line 1520 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(dummy_arg=d.ddb_name)where d.ddb_name=");
      IIputdomio((short *)0,1,32,0,duc_dbenv->du_ddbname);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE11;
IIrtB11:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,dummy_arg);
        if (IIerrtest() != 0) goto IIrtB11;
        {
/* # line 1523 "ducreate.qsc" */	/* host code */
		found	= TRUE;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE11:;
    }
/* # line 1525 "ducreate.qsc" */	/* host code */
	}
	if (found)
	{
	    /* need to modify this to take care of cdb & ddb already 
	    ** exist message.
	    */
	    /* The database, %0c, already exists. */
	    return(du_error(duc_errcb, E_DU3021_DB_EXISTS_CR, 2,
			    0, duc_dbenv->du_ddbname));
	}
    }
    if (unique_found)
    {
	/* The database not unique in first 24 characters, %0c .*/
	return(du_error(duc_errcb, E_DU3022_DB_UNIQUE_CR, 2,
			0, unique24_arg));
    }
/* validate locations here */
/* Locations now:
	- allow delimited id
	- forced lowercase
*/
	status = duc_val_loc(duc_dbenv, duc_modes);
    if (status != E_DU_OK)
		return(status);
    /*
    ** Add CREATEDB to list to current session privileges
    ** in case we don't have it by default
    */
/* # line 1707 "ducreate.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session add privileges(createdb)");
    IIsyncup((char *)0,0);
  }
/* # line 1708 "ducreate.sc" */	/* host code */
    /* assure that all internal procedures createdb will use actually exist. */
    status = duc_upgrade_internal_procs("iidbdb", 
		(duc_dbenv->du_dbservice & DU_NAME_UPPER) ? TRUE : FALSE,
		FALSE); /* currentlty connected to iidbdb, which is not stardb */
    /*
    ** Bug 78208 - get the correct case strings from remote iidbdb,
    ** then set the correct case translation flags in du_dbservice,
    ** and set the correct case for du_usrinit and du_dba_iidbdb.
    */
    if (STlength(duc_dbenv->du_nodename) > 0)
    {
	/*
	** Reset the case translation flags in dbservice
	** and the case of du_usrinit and du_dba_iidbdb
	*/
	duc_dbenv->du_dbservice &= ~DU_DELIM_MIXED;
	duc_dbenv->du_dbservice &= ~DU_DELIM_UPPER;
	duc_dbenv->du_dbservice &= ~DU_NAME_UPPER;
	duc_dbenv->du_dbservice &= ~DU_REAL_USER_MIXED;
	CVlower(duc_dbenv->du_usrinit);
	CVlower(duc_dbenv->du_dba_iidbdb);
/* # line 1585 "ducreate.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"range of d=iidbcapabilitie\
s");
      IIsyncup((char *)0,0);
    }
/* # line 1586 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(dummy_arg=d.cap_value)where d.cap_capability=\"DB_NAME_CASE\"");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE12;
IIrtB12:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,dummy_arg);
        if (IIerrtest() != 0) goto IIrtB12;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE12:;
    }
/* # line 1589 "ducreate.qsc" */	/* host code */
	STncpy( duc_dbenv->du_reg_id_case, dummy_arg, DU_CASE_LEN);
	duc_dbenv->du_reg_id_case[DU_CASE_LEN-1] = EOS;
/* # line 1592 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(dummy_arg=d.cap_value)where d.cap_capability=\"DB_DELIMITED_\
CASE\"");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE13;
IIrtB13:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,dummy_arg);
        if (IIerrtest() != 0) goto IIrtB13;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE13:;
    }
/* # line 1595 "ducreate.qsc" */	/* host code */
	STncpy( duc_dbenv->du_delim_id_case, dummy_arg, DU_CASE_LEN);
	duc_dbenv->du_delim_id_case[DU_CASE_LEN-1] = EOS;
/* # line 1598 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(dummy_arg=d.cap_value)where d.cap_capability=\"DB_REAL_USER_\
CASE\"");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE14;
IIrtB14:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,dummy_arg);
        if (IIerrtest() != 0) goto IIrtB14;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE14:;
    }
/* # line 1601 "ducreate.qsc" */	/* host code */
	STncpy( duc_dbenv->du_real_user_case, dummy_arg, DU_CASE_LEN);
	duc_dbenv->du_real_user_case[DU_CASE_LEN-1] = EOS;
	/* Set the case translation flags in dbservice */
	if (duc_dbenv->du_delim_id_case[0] == 'M')
	    duc_dbenv->du_dbservice |= DU_DELIM_MIXED;
	else if (duc_dbenv->du_delim_id_case[0] == 'U')
	    duc_dbenv->du_dbservice |= DU_DELIM_UPPER;
	if (duc_dbenv->du_reg_id_case[0] == 'U')
	    duc_dbenv->du_dbservice |= DU_NAME_UPPER;
	if (duc_dbenv->du_real_user_case[0] == 'M')
	    duc_dbenv->du_dbservice |= DU_REAL_USER_MIXED;
        if (duc_dbenv->du_real_user_case[0] == 'U')
        {
            CVupper(duc_dbenv->du_usrinit);
            CVupper(duc_dbenv->du_dba_iidbdb);
        }
    }
    return (status);    
  }
/*{
** Name: duc_exit() -	exit handler for database utility, createdb.
**
** Description:
**        This is the exit handling routine for the database utility,
**	createdb.  If the database it open, it is closed.  If the database
**	is locked, the lock is released.  If an error has occured, the
**	corresponding message is printed.  Finally, and informative "closing"
**	message is printed.
**
** Inputs:
**      duc_errcb                       Ptr to the error-handling control block.
**	    .du_status			If the value is anything other than
**					E_DU_OK, then the message in du_errmsg
**					will be printed.
**	    .du_errmsg			If .du_status != E_DU_OK, this is buffer
**					containing formatted error message.
**
** Outputs:
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      A database, either the one being created or the dbdb, is closed.
**	    If a lock on the database being created is held, it is released.
**	    A message is printed out to the user.
**	    
** History:
**      10-Oct-86 (ericj)
**          Initial creation.
**      31-Oct-88 (teg)
**          Added logic to return fail status if apporpriate.
**      24-Feb-1989 (teg)
**          Added logic to release locks if any are held.
**	24-May-1989 (teg)
**	    Removed Lock release logic because DI / LK have been moved into the
**	    server.
**      19-sep-91 (teresa, bonobo)
**          Fix bug where failure status sometimes lost and a "db created
**          successfully" message printed on a failure exit.
**	28-jan-93 (jpk)
**	    Remove rollback.  We can get here via an sql error handler,
**	    thus might be in the middle of a transaction, thus you
**	    can't roll back.  Several ways to solve this problem, for
**	    createdb the right way is don't ever try to roll back, if
**	    something went wrong just emit the best diagnostic message
**	    you can and bail.
*/
void
  duc_exit(duc_errcb)
    DU_ERROR		*duc_errcb;
  {
    bool	    failexit = FALSE;
    if (Duc_dbenv->du_dbopen)
    {
/* # line 1685 "ducreate.qsc" */	/* exit */
    {
      IIexit();
    }
/* # line 1686 "ducreate.qsc" */	/* host code */
	Duc_dbenv->du_dbopen	= FALSE;
    }
    /* If the program is exiting because of an non Ingres/Equel error,
    ** print the formatted message.  Ingres/Equel errors will have been
    ** formatted and printed by LIBQ and the createdb Ingres error
    ** handler, duc_ingerr_handler.
    */
    if (duc_errcb->du_status != E_DU_OK
	&& duc_errcb->du_ingerr == DU_NONEXIST_ERROR
       )
    {
        DU_STATUS       temp_stat;
	temp_stat = duc_errcb->du_status;
	SIprintf("%s\n", duc_errcb->du_errmsg);
	du_reset_err(duc_errcb);
	duc_errcb->du_status = temp_stat;
    }
    switch(duc_errcb->du_status)
    {
	case E_DU_IERROR:
	case E_DU_UERROR:
	    if (Duc_distributed_flag == TRUE)
	    {
		du_error(duc_errcb, W_DU1010_UTIL_ABORT, 4,
		         0, DU_1CREATION, 0, Duc_dbenv->du_ddbname);
	        if (Duc_destroydb_msg)
		    du_error(duc_errcb, W_DU1080_RUN_DESTROYDB, 2,
			     0, Duc_dbenv->du_ddbname);
	    }
	    else
	    {
		du_error(duc_errcb, W_DU1010_UTIL_ABORT, 4,
			 0, DU_1CREATION, 0, Duc_dbenv->du_dbname);
		if (Duc_destroydb_msg)
		    du_error(duc_errcb, W_DU1080_RUN_DESTROYDB, 2,
			    0, Duc_dbenv->du_dbname);
	    }
	    failexit = TRUE;
	    break;	    
	case E_DU_OK:
	case E_DU_WARN:
	case E_DU_INFO:
	    if (Duc_distributed_flag == TRUE)
	    {
		du_error(duc_errcb, I_DU0501_STAR_UTIL_SUCCESS, 4,
			0, DU_1CREATION, 0, Duc_dbenv->du_ddbname);
		break;
	    }
	    else
	    {
		du_error(duc_errcb, I_DU0011_UTIL_SUCCESS, 4,
			0, DU_1CREATION, 0, Duc_dbenv->du_dbname);
		break;
	    }
    }
    if (failexit)
	PCexit(FAIL);
    else
	PCexit(OK);
  }
/*{
** Name: du_ex_handler() -	Exception handler for destroydb.
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
** Outputs:
**	Returns:
**	    The signal is resignaled if it is not recognized.
**	Exceptions:
**	    none
**
** Side Effects:
**	      If the exception was a user interrupt, the program is
**	    exited via a call to duc_exit().
**
** History:
**      23-Oct-86 (ericj)
**          Initial creation.
**	13-sep-96 (nick)
**	    You can't match exceptions directly with a case ; use EXmatch()
**	    to index them.
*/
STATUS
du_ex_handler(ex_args)
EX_ARGS		*ex_args;
{
    EX		    ex_state;
    switch (EXmatch(ex_args->exarg_num, 1, (EX)EXINTR))
    {
      case 1:
	(void) du_error(Duc_errcb, E_DU3600_INTERRUPT, 0);
	ex_state = EXDECLARE;
	break;
      default:
	(void)du_error(Duc_errcb, W_DU1010_UTIL_ABORT, 4,
		       0, DU_1CREATION, 0, Duc_dbenv->du_dbname);
	ex_state = EXRESIGNAL;
	break;
    }
    return(ex_state);
}
/*{
** Name: du_get_dbid -	gets a unique database id for database and updates
**			the iidatabase catalog in the database database.
**
** Description:
**        This routine gets a unique database id for the database being
**	created and updates the db_id field in the iidatabase catalog with
**	this value.
**	  This routine assumes that the database database has been opened
**	such that the database catalog can be updated.
**
** Inputs:
**	duc_modes			Ptr to DUC_MODE_VECT
**	    .duc_star_flag		Identify the database as Distributed 
**					or not.
**      du_dbenv                        Database description.
**	    .du_dbname			Name of database.
**	    .du_cdbname			Name of Coordinator database.
**	    .du_ddbname			Name of Distributed database.
**	no_upd				If TRUE, then don't update the db_id
**					field of the iidatabase catalog.  This
**					flag is used to get the database data-
**					base id before it has been booted to
**					an operational state.
**	du_errcb			Error control block.
**
** Outputs:
**      *du_dbenv
**	    .du_dbid			The unique id assigned to this database.
**	Returns:
**	    E_DU_OK			Completed successfully.
**	Exceptions:
**
** Side Effects:
**	      Updates the db_id field in the iidatabase catalog.
**
** History:
**      17-Jul-86 (ericj)
**          Initial creation.
**      16-feb-1989 (EdHsu)
**          Add code to support online backup
**	17-apr-1989 (alexc)
**	    Modified to include creation of iistar_cdbs, iiddb_netcost,
**	    iiddb_nodecosts for STAR.  These tables are created if it is
**	    not already in iidbdb.
**      22-mar-90 (fourt)
**          For Gateway dbs, get value of dbservice from duc_gset_service().
**	30-oct-90 (teresa)
**	    Fix minor bug where dump location was not filled in for distributed
**	    database.
**	20-sep-91 (teresa)
**	    adapt ingres63p change 262603
**	      6-jun-91 (rickh)
**		Move the setting of du_dbservice to argument parsing time.
**		otherwise, the wrong dbservice value is written to the
**		configuration file & FINDDBS incorrectly recovers the database
**  24-July-1993 (jpk)
**		Set values of dbservice correctly for DDB, CDB, LDB.
**  15-Nov-1994 (forky01)
**		Fix BUG # 53072. When a star database was created, the
**		ddb was created, then a fake cdb_id was generated using
**		TMsecs, followed by adding ddb to iistar_cdbs with the
**		untested cdb_id. Then the cdb would be created through
**		QEF, which could possibly generate a different cdb_id,
**		since TMsecs is being called for the new id moments
**		later.  Changed order of creation, so that the cdb
**		is created prior to updating iistar_cdbs.
*/
DU_STATUS
  du_get_dbid(duc_modes, du_dbenv, no_upd, du_errcb)
    DUC_MODE_VECT   *duc_modes;
  DU_ENV *du_dbenv;
    i4		    no_upd;
    DU_ERROR	    *du_errcb;
  {
    i4 row_cnt;
	i4		try_cnt;
#define		DU_DBID_MAXTRYS		100
#define		DU_DBIDDBDB		  1
#define		DU_DBIDMIN		  2
#define		DLOCK_WAIT_INTERVAL	((u_i4) 2000)
    char exist_iistar[DB_MAXNAME + 4];
    char exist_iinet_cost[DB_MAXNAME + 4];
    char exist_iinode_cost[DB_MAXNAME + 4];
	i4	found_iistar	    = FALSE;
	i4	found_iinet_cost    = FALSE;
	i4	found_iinode_cost   = FALSE;
    char current_node[DB_MAXNAME + 4];
    char cdb_create_date[DB_MAXNAME + 4];
    i4 _cdb_id;
    DU_STARCB *duc_star_cb;
	duc_star_cb = &Duc_star_cb;
	try_cnt	= 0;
	duu_get_vnode(current_node);
	if (current_node[0] == EOS)
	{
	    if ((du_error(du_errcb, E_DU510F_STAR_CANT_FIND_VNODE, 
		    0, 0)) != OK)
		duc_exit(du_errcb);
	}
	/* TITAN changes:  
	** This following block adds the Distributed database(DDB) entry
	** to the iidatabase table.
	*/
	try_cnt	= 0;
	for (;Duc_distributed_flag;)
	{
	    /*
	    ** Retain other bits (case translation for DELIM_IDENT)
	    ** but clear the CDB bit and set the DDB bit.
	    */
    i4 ddb_dbservice =	du_dbenv->du_dbservice;
	    ddb_dbservice &= ~DU_2SER_CDB;
	    ddb_dbservice |= DU_1SER_DDB;
	    if (!STcompare(du_dbenv->du_ddbname, DU_DBDBNAME))
	    {
		/* The database database has a special id. */
		du_dbenv->du_dbid   = DU_DBIDDBDB;
	    	if (no_upd)
		    break;
	    }
	    else
	    {
		du_dbenv->du_dbid   = TMsecs();
		if (du_dbenv->du_dbid <= DU_DBIDDBDB)
		    du_dbenv->du_dbid	= DU_DBIDMIN;
	    }
	    /* NOTE:  The following append to the database catalog also
	    ** serves as a synchronization mechanism.  The first user to
	    ** successfully append will block any following users from
	    ** creating a database of the same name because their append at
	    ** this point will fail.
	    */
            IIlq_Protect(TRUE);
	    Duc_1dlock_retry = TRUE;
	    for(;;)
	    {
		if (du_chk1_dbname(du_dbenv->du_ddbname) != OK)
		{
		    return(du_error(du_errcb, E_DU5109_STAR_INVALID_NAME,
			    4, 0, DU_0DISTRIBUTED, 0, du_dbenv->du_ddbname));
		}
/* # line 1962 "ducreate.qsc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append iidatabase(name=");
      IIputdomio((short *)0,1,32,0,du_dbenv->du_ddbname);
      IIwritio(0,(short *)0,1,32,0,(char *)", own=");
      IIputdomio((short *)0,1,32,0,du_dbenv->du_dba_iidbdb);
      IIwritio(0,(short *)0,1,32,0,(char *)", dbdev=");
      IIputdomio((short *)0,1,32,0,du_dbenv->du_dbloc.du_locname);
      IIwritio(0,(short *)0,1,32,0,(char *)", jnldev=");
      IIputdomio((short *)0,1,32,0,du_dbenv->du_jnlloc.du_locname);
      IIwritio(0,(short *)0,1,32,0,(char *)", ckpdev=");
      IIputdomio((short *)0,1,32,0,du_dbenv->du_ckploc.du_locname);
      IIwritio(0,(short *)0,1,32,0,(char *)", sortdev=");
      IIputdomio((short *)0,1,32,0,du_dbenv->du_sortloc.du_locname);
      IIwritio(0,(short *)0,1,32,0,(char *)", access=");
      IIputdomio((short *)0,1,30,sizeof(DU_RES_DDB),(void *)IILQint(DU_RES_DDB
      ));
      IIwritio(0,(short *)0,1,32,0,(char *)", dbservice=");
      IIputdomio((short *)0,1,30,4,&ddb_dbservice);
      IIwritio(0,(short *)0,1,32,0,(char *)", dbcmptlvl=");
      IIputdomio((short *)0,1,32,4,DU_CUR_DBCMPTLVL);
      IIwritio(0,(short *)0,1,32,0,(char *)", dbcmptminor=");
      IIputdomio((short *)0,1,30,sizeof(DU_1CUR_DBCMPTMINOR),(void *)IILQint(
      DU_1CUR_DBCMPTMINOR));
      IIwritio(0,(short *)0,1,32,0,(char *)", dmpdev=");
      IIputdomio((short *)0,1,32,0,du_dbenv->du_dmploc.du_locname);
      IIwritio(0,(short *)0,1,32,0,(char *)", db_id=");
      IIputdomio((short *)0,1,30,4,&du_dbenv->du_dbid);
      IIwritio(0,(short *)0,1,32,0,(char *)")");
      IIsyncup((char *)0,0);
    }
/* # line 1975 "ducreate.qsc" */	/* host code */
		/* Check to see if deadlock has occured.  If not, break
		** out of this loop.  Otherwise, sleep for awhile and
		** then retry the statement that caused deadlock.
		*/
		if (!Duc_deadlock)
		{
		    break;
		}
		else
		{
		    Duc_deadlock = FALSE;
		    PCsleep(DLOCK_WAIT_INTERVAL);
		}
	    }	    /* end of Deadlock while loop. */
	    Duc_1dlock_retry = FALSE;
/* # line 1991 "ducreate.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
    }
/* # line 1992 "ducreate.qsc" */	/* host code */
	    if (row_cnt	== 1)
	    {
	        /* We've found a unique id. */
		if (STcompare(du_dbenv->du_dbname, DU_DBDBNAME))
		    /* An entry has been successfully appended to the
		    ** iidatabase catalog of the iidbdb.  Set a global
		    ** variable so that is CREATEDB fails after this,
		    ** we'll inform the user to run DESTROYDB to clean up
		    ** after the partially created database.
		    */
		    Duc_destroydb_msg	= TRUE;
	        break;
	    }
	    /* Otherwise loop back and try a new id.  But make sure this
	    ** isn't done infinitely.
	    */
	    try_cnt++;
	    if (try_cnt	>=  DU_DBID_MAXTRYS)
	        return(du_error(du_errcb, E_DU31D0_DBID_MAXTRYS, 0));
	}	/* end of for(;;) loop. */
	/** The following block adds the CDB(coordinator database) and the
	    DDB(distributed database) entry to the iistar_cdbs table.
	**/
	if (Duc_distributed_flag)
	{
	    found_iistar = FALSE;
/* # line 2024 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(exist_iistar=iitables.table_name)where iitables.table_name=\"\
iistar_cdbs\" or iitables.table_name=\"IISTAR_CDBS\"");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE15;
IIrtB15:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 4-1,exist_iistar);
        if (IIerrtest() != 0) goto IIrtB15;
        {
/* # line 2028 "ducreate.qsc" */	/* host code */
	        found_iistar = TRUE;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE15:;
    }
/* # line 2031 "ducreate.qsc" */	/* host code */
	    if (found_iistar == FALSE)
	    {
		duc_6create_iidbdb_cats("iistar_cdbs");
	    }
/*
**  New tables created for Distributed Optimizer.
*/
/* # line 2040 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(exist_iinet_cost=iitables.table_name)where iitables.table_na\
me=\"iiddb_netcost\" or iitables.table_name=\"IIDDB_NETCOST\"");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE16;
IIrtB16:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 4-1,exist_iinet_cost);
        if (IIerrtest() != 0) goto IIrtB16;
        {
/* # line 2044 "ducreate.qsc" */	/* host code */
		found_iinet_cost = TRUE;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE16:;
    }
/* # line 2047 "ducreate.qsc" */	/* host code */
	    if (found_iinet_cost == FALSE)
	    {
		duc_6create_iidbdb_cats("iiddb_netcost");
	    }
/* # line 2052 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(exist_iinode_cost=iitables.table_name)where iitables.table_n\
ame=\"iiddb_nodecosts\" or iitables.table_name=\"IIDDB_NODECOSTS\"");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE17;
IIrtB17:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 4-1,exist_iinode_cost);
        if (IIerrtest() != 0) goto IIrtB17;
        {
/* # line 2056 "ducreate.qsc" */	/* host code */
		found_iinode_cost = TRUE;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE17:;
    }
/* # line 2059 "ducreate.qsc" */	/* host code */
	    if (found_iinode_cost == FALSE)
	    {
		duc_6create_iidbdb_cats("iiddb_nodecosts");
	    }
	    if (du_chk1_dbname(du_dbenv->du_cdbname) != OK)
	    {
	        return(du_error(du_errcb, E_DU5109_STAR_INVALID_NAME,
			    4, 0, DU_1COORDINATOR, 0, du_dbenv->du_cdbname));
	    }
	    if (du_chk1_dbname(du_dbenv->du_ddbname) != OK)
	    {
	        return(du_error(du_errcb, E_DU5109_STAR_INVALID_NAME,
			    4, 0, DU_0DISTRIBUTED, 0, du_dbenv->du_ddbname));
	    }
	    CVlower(current_node);
	    STcopy(current_node, duc_star_cb->current_node);
	    /*
	    ** Create CDB in iidatabase, so we can put cdb_id in 
	    ** iistar_cdbs under the ddb entry
	    */
	    if (duc_db_boot(du_errcb, du_dbenv, TRUE) != E_DU_OK)
	    {
	        return(du_errcb->du_status);
	    }
/* # line 2087 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(_cdb_id=iidatabase.db_id)where iidatabase.name=");
      IIputdomio((short *)0,1,32,0,du_dbenv->du_cdbname);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE18;
IIrtB18:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&_cdb_id);
        if (IIerrtest() != 0) goto IIrtB18;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE18:;
    }
/* # line 2090 "ducreate.qsc" */	/* host code */
	    duc_star_cb->cdb_id = _cdb_id;
/* # line 2092 "ducreate.qsc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append iistar_cdbs(ddb_nam\
e=");
      IIputdomio((short *)0,1,32,0,du_dbenv->du_ddbname);
      IIwritio(0,(short *)0,1,32,0,(char *)", ddb_owner=");
      IIputdomio((short *)0,1,32,0,du_dbenv->du_dba_iidbdb);
      IIwritio(0,(short *)0,1,32,0,(char *)", cdb_name=");
      IIputdomio((short *)0,1,32,0,du_dbenv->du_cdbname);
      IIwritio(0,(short *)0,1,32,0,(char *)", cdb_owner=");
      IIputdomio((short *)0,1,32,0,du_dbenv->du_dba_iidbdb);
      IIwritio(0,(short *)0,1,32,0,(char *)", cdb_node=");
      IIputdomio((short *)0,1,32,0,current_node);
      IIwritio(0,(short *)0,1,32,0,(char *)
", cdb_dbms=\"INGRES\", scheme_desc=\"UNKNOWN\", create_date=date_gmt(\
date(\"now\")), original=\"Y\", cdb_id=");
      IIputdomio((short *)0,1,30,4,&_cdb_id);
      IIwritio(0,(short *)0,1,32,0,(char *)", cdb_capability=0)");
      IIsyncup((char *)0,0);
    }
/* # line 2107 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(cdb_create_date=iistar_cdbs.create_date)");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE19;
IIrtB19:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 4-1,cdb_create_date);
        if (IIerrtest() != 0) goto IIrtB19;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE19:;
    }
/* # line 2108 "ducreate.qsc" */	/* host code */
	    STcopy(cdb_create_date, duc_star_cb->cdb_create_date);
	    STcopy(du_dbenv->du_ddbname, duc_star_cb->ddb_name);
	    STcopy(du_dbenv->du_cdbname, duc_star_cb->cdb_name);
	    STcopy(du_dbenv->du_dba_iidbdb, duc_star_cb->dba_name);
	} /* if Duc_distributed_flag */
	return(E_DU_OK);
  }
/*{
** Name: duc_ingerr_handler()	-   Error handler for Ingres errors
**				    encountered by Equel statements.
**
** Description:
**        This routine is used to handle Ingres errors that are generated
**	as a result of Equel statements.  These errors will either be
**	translated to a database utility error, reported and exited, or
**	sent back to the program to handle itself.
**	  This routine will be passed as an argument to IIseterr().
**
** Inputs:
**      ing_error                       Ingres error number passed to this
**					handler by Libq.
**
** Outputs:
**	Returns:
**	    0				Completed successfully.
**	Exceptions:
**	    none
**
** Side Effects:
**	  This routine updates the fields, du_status, and du_ingerr in the
**	DU_ERROR struct through the global pointer Duc_errcb.  This is done
**	so that duc_exit() can properly handle program exit.
**	  Also, this procedure may determine that a fatal Ingres error has
**	occured, and call the exit handling procedure directly.
**
** History:
**      22-Oct-86 (ericj)
**          Initial creation.
**      30-Nov-87 (ericj)
**          Update to handle Deadlock errors more appropriately.  In some
**	    cases, they will be treated fatally (where deadlock is
**	    thought not to be possible).  In other cases, we will try to
**	    redo the statement that caused deadlock.
*/
/* fix_me */
i4
  duc_ingerr_handler(ing_error)
i4	    *ing_error;
  {
    char *err_msg;
	if (*ing_error == DU_DEADLOCK_ERROR && Duc_1dlock_retry)
	{
	    /* Deadlock has occurred and we will try to redo the
	    ** statement that caused deadlock.
	    */
	    Duc_dlock_cnt++;
	    Duc_deadlock	= TRUE;
	}
	else if ( (*ing_error == E_US0010_NO_SUCH_DB)
	           && (Duc_iidbdb_read == FALSE) )
	{
	    /* we had an error opening the iidbdb */
	    (void) du_error(Duc_errcb, E_DU3131_MISSIING_DBDB, 0);
	    SIprintf("%s\n", Duc_errcb->du_errmsg);
	    du_reset_err(Duc_errcb);
	    Duc_errcb->du_status	= E_DU_UERROR;
	    Duc_errcb->du_ingerr	= *ing_error;
	    duc_exit(Duc_errcb);
	}
	else
	{
	    err_msg = Duc_errcb->du_errmsg;
/* # line 2183 "ducreate.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,32,0,err_msg,(char *)"errortext");
    }
/* # line 2184 "ducreate.qsc" */	/* host code */
	    SIprintf("%s\n", err_msg);
	    Duc_errcb->du_status	= E_DU_UERROR;
	    Duc_errcb->du_ingerr	= *ing_error;
	    duc_exit(Duc_errcb);
	}
	/* Put the return here instead of inside the if block to keep lint happy */
	return(E_DU_OK);
  }
/*{
** Name: duc_sqlerr_handler()	-   Error handler for Ingres errors
**				    encountered by SQL statements.
**
** Description:
**	These errors will either be translated to a database utility error,
**	reported and exited, or sent back to the program to handle itself.
**
** Inputs:
**      ing_error                       Createdb error number.
**
** Outputs:
**	Returns:
**	    0				Completed successfully.
**	Exceptions:
**	    none
**
** Side Effects:
**	  This routine updates the fields, du_status, and du_ingerr in the
**	DU_ERROR struct through the global pointer Duc_errcb.  This is done
**	so that duc_exit() can properly handle program exit.
**	  Also, this procedure may determine that a fatal Ingres error has
**	occured, and call the exit handling procedure directly.
**
** History:
**	28-aug-1989 (Derek)
**	    Created.
*/
/* fix_me */
i4
  duc_sqlerr_handler(error)
i4	    error;
  {
    char *err_msg;
	if (error == DU_DEADLOCK_ERROR && Duc_1dlock_retry)
	{
	    /* Deadlock has occurred and we will try to redo the
	    ** statement that caused deadlock.
	    */
	    Duc_dlock_cnt++;
	    Duc_deadlock	= TRUE;
	}
	else if ((error == E_US0010_NO_SUCH_DB) && (Duc_iidbdb_read == FALSE))
	{
	    /* we had an error opening the iidbdb */
	    (void) du_error(Duc_errcb, E_DU3131_MISSIING_DBDB, 0);
	    SIprintf("%s\n", Duc_errcb->du_errmsg);
	    du_reset_err(Duc_errcb);
	    Duc_errcb->du_status	= E_DU_UERROR;
	    Duc_errcb->du_ingerr	= error;
	    duc_exit(Duc_errcb);
	}
	else
	{
	    err_msg = Duc_errcb->du_errmsg;
/* # line 2250 "ducreate.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,32,0,err_msg,(char *)"errortext");
    }
/* # line 2251 "ducreate.qsc" */	/* host code */
	    SIprintf("%s\n", err_msg);
	    Duc_errcb->du_status	= E_DU_UERROR;
	    Duc_errcb->du_ingerr	= error;
	    duc_exit(Duc_errcb);
	}
	/* Put the return here instead of inside the if block to keep lint happy */
	return(E_DU_OK);
  }
/*{
** Name: duc_makedb() -	    make all of the non-core system catalogs
**
** Description:
**        This routine makes and configures all of the non-core system
**	catalogs for a database.  These catalogs are always made with
**	$ingres ownership.  If the database being created is the dbdb,
**	then catalogs unique to that database are also made.
**
** Inputs:
**      duc_dbenv                       Ptr to a DU_ENV struct.
**	    .du_dbname			Name of the database being created.
**	duc_errcb			Ptr to error-handling control block.
**
** Outputs:
**      *duc_errcb                      If an error occurs, this struct will be
**					set by a call to du_error().
**	Returns:
**	    E_DU_OK			Completed successfully.
**	Exceptions:
**	      Any equel errors that occur, will be signaled and caught by
**	    duc_ingerr_handler().
**
** Side Effects:
**	      Creates the extended system catalogs in the database,
**	    duc_dbenv->du_dbname.  Note, this routine opens and closes
**	    the database being created.
**	      Also, this procedure sets the variable duc_dbenv->du_dbopen
**	    to TRUE for the duration of its execution.  It sets it to FALSE
**	    upon normal procedure exit.  However, if an error occurs, it
**	    will be left as TRUE for the exit handler.
**
** History:
**      10-Oct-86 (ericj)
**          Initial creation.
**      22-Jan-87 (ericj)
**          Added code to update the relloc field in the iirelation
**	    catalog if necessary.  Also, changed the text datatype
**	    fields in iiqrytext and iitree to varchar datatype.
**      28-Jul-88 (ericj)
**          Updated for new 'iiprocedure' definition.
**	06-feb-89 (ralph)
**	    Changed duc_makedb to specify new iiprotect and iiintegrity layout
**	03-mar-89 (ralph)
**	    GRANT Enhancements, Phase 1:
**	    Added proseq, progtype fields to iiprotect;
**	    Added intseq field to iiintegrity;
**	    Rearranged iiprotect fields to put proreserve at end.
**	27-mar-89 (neil)
**	    Rules: Added create and modify of iirule.
**	10-may-89 (ralph)
**	    GRANT Enhancements, Phase 2:
**	    added minor format change to iiprotect.
**	25-May-1989 (teg)
**	    changed to call dub_mklocs to make database's journal, sort,
**	    checkpoint and dump locations
**	31-may-89 (ralph)
**	    GRANT Enhancements, Phase 2c:
**	    duc_1makedbdb creates iidbprivileges view.
**      03-jan-90 (neil)
**          Alerters/Terminator II: Added creation and modify of iievent.
**	21-jan-1990 (alexc)
**	    Add creation of iidbms_comment and iisynonyms catalogs and
**		modify statements for these catalogs.
**      08-aug-90 (ralph)
**          Add new fields to iiprotect for FIPS, REVOKE, GRANT, etc.
**	    Add iisectype base table
**	20-sep-91 (teresa)
**	    changes to retrofix ingres63p change 262603 to 6.5:
**	      21-jun-91 (teresa)
**		Modified createdb to connect to star server to call upgradefe on
**		creation of a DDB.
**	11-Jan-93 (jpk)
**	    Added call to new procedure duc_p5_alter_extension
**	    that builds the database procedures that are in all
**	    databases, not just the iidbdb.
**	12-Jan-93 (jpk)
**	    Added parameter duc_dbenv to dui_stdcts() to
**	    pass the values to deposit in iidbcapabilities
**	    that specify whether this is a FIPS database.
**	5-Feb-1993 (jpk)
**	    Register logging system managed objects for XA use.
**	02-mar-93 (andre)
**	    Added a call to duc_iierror() to create an internal dbproc iierror
**	    and GRANT EXECUTE WGO on it to PUBLIC
**	20-apr-93 (ralph)
**	    Pass duc_errcb to duc_iilgmo as required.
*/
  DU_ENV *g_duc_dbenv;
DU_STATUS
  duc_makedb(duc_modes, duc_dbenv, duc_errcb)
    DUC_MODE_VECT	*duc_modes;
  DU_ENV *duc_dbenv;
    DU_ERROR		*duc_errcb;
  {
	DU_STATUS	duc_0makecdb();
	DU_STATUS	status;
	char		installation_def_dbloc[DU_LOC_SIZE + 1];
	DU_STATUS	duc_1makedbdb();
	/* Now, make the rest of the system catalogs. */
        if ((status=duc_dbms_bld(duc_errcb, duc_dbenv, FALSE)) != E_DU_OK)
	    return(status);
	/* Create "database database" system catalogs if necessary. */
	if (!STcompare(duc_dbenv->du_dbname, DU_DBDBNAME))
	{
	    /* Create the system catalogs unique to the "dbdb" database. */
	    if ((status = duc_1makedbdb(duc_dbenv, duc_errcb)) != E_DU_OK)
	    {
		if (status == E_DU_FATAL)
		    duc_exit(duc_errcb);
		else
		    return(status);
	    }
	    /* make dbdb specific IMA registrations and views */
	    duc_iilgmo(duc_errcb);
	}
	/* build the internal db procedures */
	status = duc_create_internal_procs(duc_dbenv->du_dbname, 
	    (duc_dbenv->du_dbservice & DU_NAME_UPPER) ? TRUE : FALSE,
	    duc_modes->duc_star_flag);
	/* Create the standard catalog interface. */
	if ((status = dui_stdcts(duc_modes->duc_gw_flag, duc_errcb, duc_modes, duc_dbenv))
		!= E_DU_OK)
	    return(status);
	/* Create the "STAR" system catalogs. */
	if (duc_modes->duc_star_flag == TRUE)
	{
	    if(status=duc_0makecdb(duc_dbenv, duc_errcb) != E_DU_OK)
		return(status);
	    if ((du_error(duc_errcb, I_DU0503_STAR_UTIL_GENERAL, 4,
	        0, DU_0INITIALIZING, 0, "System Catalogs"))
		!= E_DU_OK)
		duc_exit(duc_errcb);
	    duc_create_star_views(duc_dbenv->du_dbname,
		(duc_dbenv->du_dbservice & DU_NAME_UPPER) ? TRUE : FALSE);
	    duc_4init_star_catalogs(duc_dbenv, duc_errcb);
	}
	/* Create the "frontend" system catalogs. */
	if ( (status = dui_femake(duc_errcb, duc_modes, FALSE, duc_dbenv) )
	   != E_DU_OK)
	    return(status);
        /* Create the special Gateway system catalogs. */
        if (duc_modes->duc_gw_flag)
	{
            if(status=duc_gmake_gdb(duc_modes->duc_gw_flag, duc_dbenv,
				    duc_errcb) != E_DU_OK)
		return(status);
	}
	/* 
	** Unless we are creating a STAR database (STAR does not know about
	** GRANT), issue GRANT SELECT TO PUBLIC WGO on all catalogs
	*/
	if (duc_modes->duc_star_flag == FALSE)
	{
	    status = duc_grant_select_wgo_on_cats(duc_errcb);
	    if (status != E_DU_OK)
	    {
		return(status);
	    }
	}
	return(E_DU_OK);
  }
static  char    ut_args[] = ERx("database = %S, equel = %S, user = %S, page_size= %N");
static  char    Fecatmod[] = ERx("modifyfe");   /* name of FE modify program */
/*{
** Name: dus_femodify - modify the front-end catalogs.
**
** Description:
**      This routine uses UTexe to call another executable: "modifyfe", which
**      modifies the front-end catalogs.
**
** Inputs:
**              duc_modes               Command line info.
**              duc_dbenv               Database environment info.
**              duc_errcb               DUF error-handling control block.
**
** Outputs:
**      Returns:
**          E_DU_OK                     Completed successfully.
**
**      Exceptions:
**          none
**
** Side Effects:
**
** History:
**         21-Apr-2004 (nansa02)
**          Created from sysmod.
**
*/   
DU_STATUS
duc_femodify(duc_modes, duc_dbenv, duc_errcb)
DUC_MODE_VECT   *duc_modes;
DU_ENV		*duc_dbenv;
DU_ERROR	*duc_errcb;
{
        char    buf[256];
        STATUS stat;
        CL_ERR_DESC err_code;
        duc_modes->duc_client_name[0] = EOS; 
	if (STcompare(duc_modes->duc_client_name,iiduNoFeClients)==0)
	{
		/* user specified no front end catalogs, so just return */
		return E_DU_OK;
	}
        if (duc_modes->duc_all_catalogs == TRUE)
	{
        	/* modify all FE catalogs. */
		STcopy (ut_args, buf);
	}
        else if (duc_modes->duc_client_name[0] == EOS)
        {
                return E_DU_OK;
        }
        else
        {
                /* duc_modes->duc_client_name contains a list of client names.
		** pass them to modifyfe.
		*/
                STpolycat (2, ut_args, ERx(", client = %S"), buf);
        }
        /* modify the front-end catalogs */
        stat = UTexe (UT_WAIT|UT_STRICT, NULL, (ModDictDesc *)NULL,
                        NULL, Fecatmod, &err_code, buf,
                        (duc_modes->duc_client_name[0] != EOS) ? 5 : 4,
                        duc_dbenv->du_dbname,
                        duu_xflag(duc_dbenv->du_dbname),
                        duc_dbenv->du_usrinit,
			duc_modes->duc_page_size,
                        (duc_modes->duc_client_name[0] != EOS)
                                 ? duc_modes->duc_client_name : NULL
                     );
        if (stat != OK)
        {
                /* something went wrong. Give warning & continue */
                /* save CL err info from UTexe so "du_error" can see it */
                STRUCT_ASSIGN_MACRO (err_code, duc_errcb->du_clsyserr);
                du_error(duc_errcb, W_DU1031_NOMOD_FE_CAT, 2,
			(i4 *) NULL, (PTR) Fecatmod);
        }
	return E_DU_OK;
}
/*{
** Name: duc_0makecdb	- creates the cdb catalogs and initialize their values.
**
** Description:
**	Create TITAN CDB catalogs, modify and initialize the necessary 
**	TITAN catalogs.
**
** Inputs:
**      duc_dbenv                       Ptr to DU_ENV
**	duc_errcb			Ptr to error-handling control block.
**
** Outputs:
**      *duc_errcb                      If an error occurs, this struct will be
**					set by a call to du_error().
**	Returns:
**	    E_DU_OK			Completed successfully.
**	Exceptions:
**
** Side Effects:
**
** History:
**
**      Jul-16-88 (alexc)
**          added to DUCREATEDB.QC for creating and initializing STAR internal
**	    catalogs.  project - TITAN.
*/
static DU_STATUS
  duc_0makecdb(duc_dbenv, duc_errcb)
  DU_ENV *duc_dbenv;
    DU_ERROR		*duc_errcb;
  {
    if ((du_error(duc_errcb, I_DU0503_STAR_UTIL_GENERAL, 4,
	    0, DU_0CREATING, 0, "System Catalogs"))
	!= E_DU_OK)
	duc_exit(duc_errcb);
    duc_1create_iidd_catalogs();
    duc_2create_iidd_ddb_catalogs();
    return(E_DU_OK);
  }
/*{
** Name: duc_1create_iidd_catalogs   - Create iidd_* catalogs for TITAN.
**
** Description:
**      Creates the TITAN catalogs that are named 'iidd_<name>'.
**
** Inputs:
**      none.
**
** Outputs:
**	Returns:
**	Exceptions:
**
** Side Effects:
**
** History:
**      30-Sept-88 (alexc)
**          created for TITAN.
**	23-Aug-90 (teresa)
**	    fix bug 30502
**	20-feb-1991 (teresa)
**	    add system_owner to iidd_dbconstants to reflect change to
**	    iidbconstants.
**	09-nov-92 (rganski)
**	    Added 4 new fields to iidd_stats: column_domain, is_complete,
**	    stat_version and hist_data_length. These correspond to existing
**	    sdomain and scomplete columns and new sversion and
**	    shistlength columns of iistatistics. For Character Histogram
**	    Enhancements project.
**	10-Jan-1993 (jpk)
**	    Added new star system catalog iidd_procedures
**	8-Feb-1993 (jpk)
**	    Encapsulate creation of iidd_stats so that upgradedb can
**	    call the same function that createdb calls.
**	27-Apr-1993 (jpk)
**	    Call duc_v10_iiregistered_objects() to create the
**	    iiregistered_objects star standard catalog.
**	22-May-1993 (jpk)
**	    Stop creating view iiddb_finddbs.  It was just a scratch
**	    table for finddbs.  Now that finddbs is gone, it is no
**	    longer needed.
**	07-Oct-1994 (brogr02)
**          Move call to duc_v10_iiregistered_objects() to duc_0makecdb(),
**          after the iidd_ddb_... tables have been created.
**	06-mar-1996 (nanpr01)
**	    Changed standard catalogue interface to include pagesize.
**	    Encapsulate creation of iidd_tables, iidd_indexes, 
**	    iidd_physical_tables so that upgradedb can call the same 
**	    function that createdb calls.
**	17-jul-1996 (ramra01)
**	    Added relversion and reltotwid to the std catalog interface.
*/
static DU_STATUS
duc_1create_iidd_catalogs()
{
  DU_STARCB *duc_star_cb;
    duc_star_cb = &Duc_star_cb;
    /*  This section copies the table definitions from system catalogs
    **	STAR catalogs.
    */
/* # line 2630 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_alt_columns(table_name=char(32), table_owner=char(32), ke\
y_id=i4, column_name=char(32), key_sequence=i2)");
    IIsyncup((char *)0,0);
  }
/* # line 2636 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_columns(table_name=char(32), table_owner=char(32), column\
_name=char(32), column_datatype=char(32), column_length=i4, column_sca\
le=i2, column_nulls=char(8), column_defaults=char(8), column_sequence=\
i2, key_sequence=i2, sort_direction=char(8), ");
    IIwritio(0,(short *)0,1,32,0,(char *)"column_ingdatatype=i2)");
    IIsyncup((char *)0,0);
  }
/* # line 2649 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_dbcapabilities(cap_capability=char(32), cap_value=char(32\
))");
    IIsyncup((char *)0,0);
  }
/* # line 2652 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_histograms(table_name=char(32), table_owner=char(32), col\
umn_name=char(32), text_sequence=i2, text_segment=char(228))");
    IIsyncup((char *)0,0);
  }
/* # line 2658 "ducreate.qsc" */	/* host code */
	duc_iidd_indexes();
/* # line 2660 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_index_columns(index_name=char(32), index_owner=char(32), \
column_name=char(32), key_sequence=i2, sort_direction=char(8))");
    IIsyncup((char *)0,0);
  }
/* # line 2666 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_integrities(table_name=char(32), table_owner=char(32), cr\
eate_date=char(25), integrity_number=i2, text_sequence=i2, text_segmen\
t=vchar(240))");
    IIsyncup((char *)0,0);
  }
/* # line 2673 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_multi_locations(table_name=char(32), table_owner=char(32)\
, loc_sequence=i2, location_name=char(32))");
    IIsyncup((char *)0,0);
  }
/* # line 2678 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_permits(object_name=char(32), object_owner=char(32), obje\
ct_type=char(8), create_date=char(25), permit_user=char(32), permit_nu\
mber=i2, text_sequence=i2, text_segment=vchar(240))");
    IIsyncup((char *)0,0);
  }
/* # line 2687 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_procedure(dbp_name=char(32)not null, dbp_owner=char(32)no\
t null, dbp_txtlen=i4 not null, dbp_txtid1=i4 not null, dbp_txtid2=i4 \
not null, dbp_mask1=i4 not null, dbp_mask2=i4 not null, dbp_id=i4 not \
null, dbp_idx=i4 not null, dbp_pad=char(12)not");
    IIwritio(0,(short *)0,1,32,0,(char *)" null, procedure_type=char(8\
))");
    IIsyncup((char *)0,0);
  }
/* # line 2699 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_procedures(procedure_name=char(32), procedure_owner=char(\
32), create_date=char(25), proc_subtype=varchar(1), text_sequence=i4, \
text_segment=varchar(240))");
    IIsyncup((char *)0,0);
  }
/* # line 2708 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_registrations(object_name=char(32), object_owner=char(32)\
, object_dml=char(8), object_type=char(8), object_subtype=char(8), tex\
t_sequence=i4, text_segment=varchar(240))");
    IIsyncup((char *)0,0);
  }
/* # line 2717 "ducreate.qsc" */	/* host code */
    duc_iidd_stats();
    duc_iidd_tables();
/* # line 2721 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_views(table_name=char(32), table_owner=char(32), view_dml\
=char(8), check_option=char(8), text_sequence=i4, text_segment=varchar\
(256))");
    IIsyncup((char *)0,0);
  }
/* # line 2729 "ducreate.qsc" */	/* host code */
    return(E_DU_OK);
}
/*{
** Name: duc_2create_iidd_ddb_catalogs   - 
**
** Description:
**      Create the TITAN catalogs that are named 'iidd_ddb_<name>'. 
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**
** Side Effects:
**
** History:
**      30-Sept-88 (alexc)
**          created for TITAN.
**	06-jan-1990 (alexc)
**	    create iidd_ddb_dxlog and iidd_ddb_dxldbs for 2PC support.
**      13-jun-1990 (teresa)
**          add new STAR catalogs for 2PC: iidd_ddb_dxlog, iidd_ddb_dxldbs
**              and index iidd_ddb_xdxlog and fix modify of iisynonym
**      09-jul-1990 (teresa)
**          add 4 new fields to iidd_ddb_dxldbs:
**          ldb_lxid1, ldb_lxid2, ldb_lxname, ldb_lxflags
**      10-jul-1990 (teresa)
**          removed obsolete catalogs: iidd_ddb_ldb_ingtables,
**          iidd_ddb_ldb_indexes, iidd_ddb_ldb_keycols, iidd_ddb_ldb_phytables,
**          iidd_ddb_ldb_xcolumns
*/
DU_STATUS
duc_2create_iidd_ddb_catalogs()
{
/* # line 2766 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_ddb_dbdepends(inid1=i4, inid2=i4, itype=i4, deid1=i4, dei\
d2=i4, dtype=i4, qid=i4)");
    IIsyncup((char *)0,0);
  }
/* # line 2774 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_ddb_ldbids(ldb_node=char(32), ldb_dbms=char(32), ldb_data\
base=char(32), ldb_longname=char(8), ldb_id=i4, ldb_user=char(8), ldb_\
dba=char(8), ldb_dbaname=char(32))");
    IIsyncup((char *)0,0);
  }
/* # line 2783 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_ddb_ldb_columns(object_base=i4, object_index=i4, local_co\
lumn=char(32), column_sequence=i4)");
    IIsyncup((char *)0,0);
  }
/* # line 2788 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_ddb_ldb_dbcaps(ldb_id=i4, cap_capability=char(32), cap_va\
lue=char(32), cap_level=i4)");
    IIsyncup((char *)0,0);
  }
/* # line 2793 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_ddb_long_ldbnames(long_ldbname=char(256), ldb_id=i4, long\
_alias=char(32))");
    IIsyncup((char *)0,0);
  }
/* # line 2797 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_ddb_objects(object_name=char(32), object_owner=char(32), \
object_base=i4, object_index=i4, qid1=i4, qid2=i4, create_date=char(25\
), object_type=char(8), alter_date=char(25), system_object=char(8), to\
_expire=char(8), expire_date=char(25))");
    IIsyncup((char *)0,0);
  }
/* # line 2810 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_ddb_object_base(object_base=i4)");
    IIsyncup((char *)0,0);
  }
/* # line 2812 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_ddb_tableinfo(object_base=i4, object_index=i4, local_type\
=char(8), table_name=char(32), table_owner=char(32), table_date=char(2\
5), table_alter=char(25), table_relstamp1=i4, table_relstamp2=i4, colu\
mns_mapped=char(8), ldb_id=i4)");
    IIsyncup((char *)0,0);
  }
/* # line 2824 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_ddb_tree(treetabbase=i4, treetabidx=i4, treeid1=i4, treei\
d2=i4, treeseq=i2, treemode=i2, treevers=i2, treetree=varchar(1024))");
    IIsyncup((char *)0,0);
  }
/* # line 2833 "ducreate.qsc" */	/* host code */
/* Create catalogs for 2PC support */
/* # line 2835 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_ddb_dxlog(dx_id1=integer not null not default, dx_id2=int\
eger not null not default, dx_name=char(32)not null not default, dx_fl\
ags=integer not null not default, dx_state=integer not null not defaul\
t, dx_create=char(25)not null not default, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"dx_modify=char(25)not null not default, dx_starid1=integer not null n\
ot default, dx_starid2=integer not null not default, dx_ddb_node=char(\
32)not null not default, dx_ddb_name=char(256)not null not default, dx\
_ddb_dbms=char(32)not null not default, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"dx_ddb_id=integer not null not default)");
    IIsyncup((char *)0,0);
  }
/* # line 2852 "ducreate.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_ddb_dxldbs(ldb_dxid1=integer not null not default, ldb_dx\
id2=integer not null not default, ldb_node=char(32)not null not defaul\
t, ldb_name=char(256)not null not default, ldb_dbms=char(32)not null n\
ot default, ldb_id=integer not null not ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"default, ldb_lxstate=integer not null not default, ldb_lxid1=integer \
not null not default, ldb_lxid2=integer not null not default, ldb_lxna\
me=char(32)not null not default, ldb_lxflags=integer not null not defa\
ult)");
    IIsyncup((char *)0,0);
  }
/* # line 2868 "ducreate.qsc" */	/* host code */
    return(E_DU_OK);
}
/*{
** Name: duc_3modify_star_catalogs	- 
**
** Description:
**      Modify the structure of TITAN catalogs. 
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**
** Side Effects:
**
** History:
**      30-Sept-88 (alexc)
**          created for TITAN.
**      10-jul-90 (teresa)
**          stop modifying obsolete catalog: iidd_ddb_ldb_indexes
**	06-mar-1996 (nanpr01)
**          Encapsulate creation of iidd_tables, iidd_indexes,
**          so that upgradedb can call the same function that createdb calls.
**	17-jul-1996 (ramra01)
**	    Added relversion and reltotwid fields.
*/
static DU_STATUS
duc_3modify_star_catalogs()
{
/* # line 2901 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_ddb_dbdepends to hash on inid1, inid2, itype");
    IIsyncup((char *)0,0);
  }
/* # line 2902 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_ddb_ldbids to hash on ldb_node, ldb_dbms, ldb_database");
    IIsyncup((char *)0,0);
  }
/* # line 2903 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_ddb_ldb_columns to hash on object_base, object_index, col\
umn_sequence");
    IIsyncup((char *)0,0);
  }
/* # line 2905 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_ddb_ldb_dbcaps to hash on ldb_id");
    IIsyncup((char *)0,0);
  }
/* # line 2906 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_ddb_long_ldbnames to hash on long_ldbname, ldb_id");
    IIsyncup((char *)0,0);
  }
/* # line 2907 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_ddb_objects to hash on object_name, object_owner");
    IIsyncup((char *)0,0);
  }
/* # line 2908 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_ddb_tableinfo to hash on object_base, object_index");
    IIsyncup((char *)0,0);
  }
/* # line 2909 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_ddb_tree to hash on treetabbase, treetabidx, treemode, tr\
eeseq");
    IIsyncup((char *)0,0);
  }
/* # line 2911 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_alt_columns to hash on table_name, table_owner");
    IIsyncup((char *)0,0);
  }
/* # line 2912 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_columns to hash on table_name, table_owner");
    IIsyncup((char *)0,0);
  }
/* # line 2913 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_histograms to hash on table_name, table_owner, column_nam\
e");
    IIsyncup((char *)0,0);
  }
/* # line 2915 "ducreate.qsc" */	/* host code */
	duc_modify_iidd_indexes();
/* # line 2916 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_index_columns to hash on index_name, index_owner");
    IIsyncup((char *)0,0);
  }
/* # line 2917 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_integrities to hash on table_name, table_owner");
    IIsyncup((char *)0,0);
  }
/* # line 2918 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_multi_locations to hash on table_name, table_owner");
    IIsyncup((char *)0,0);
  }
/* # line 2919 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_permits to hash on object_name, object_owner");
    IIsyncup((char *)0,0);
  }
/* # line 2920 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_procedure to hash on dbp_name, dbp_owner");
    IIsyncup((char *)0,0);
  }
/* # line 2921 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_registrations to hash on object_name, object_owner");
    IIsyncup((char *)0,0);
  }
/* # line 2922 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_stats to hash on table_name, table_owner");
    IIsyncup((char *)0,0);
  }
/* # line 2923 "ducreate.qsc" */	/* host code */
	duc_modify_iidd_tables();
/* # line 2924 "ducreate.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_views to hash on table_name, table_owner, text_sequence");
    IIsyncup((char *)0,0);
  }
/* # line 2927 "ducreate.qsc" */	/* host code */
/*
** Modify new STAR catalogs.
*/
/* # line 2930 "ducreate.qsc" */	/* index */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"index unique on iidd_ddb_dxlog=iidd_ddb_xdxlog(dx_id1, dx_id2)with st\
ructure=hash");
    IIsyncup((char *)0,0);
  }
/* # line 2933 "ducreate.qsc" */	/* host code */
    return(E_DU_OK);
}
/*{
** Name: duc_4init_star_catalogs   - 
**
** Description:
**      Initialize the TITAN catalogs with needed information. 
**
** Inputs:
**      duc_dbenv                       Ptr to DU_ENV
**	duc_errcb			Ptr to error-handling control block.
**
** Outputs:
**      *duc_errcb                      If an error occurs, this struct will be
**					set by a call to du_error().
**	Returns:
**	    E_DU_OK			Completed successfully.
**	Exceptions:
**
** Side Effects:
**
** History:
**      30-Sept-88 (alexc)
**          created for TITAN.
**	25-jun-1990 (teresa)
**	    fixed bug 30853 that existed because iidd_indexes was not
**	    populated from iiindexes.
**      26-jun-1990 (teresa)
**          changed createdb to use table lookup to build entries in
**          star catalogs for registered syscats with secondary indexes.
**          (redo of fix for bug 30853)
**      02-jul-90 (teresa)
**          changed iidd_tables.table_permits and iidd_table.table_integrities
**          to ' ' instead of populating from iitables.  This fixes serrious
**          bug that causes star server to AV when trying to update registered
**          table with integrities/permits.  (No bug number was filed)
**	10-feb-1993 (jpk)
**	    support createdb over the net.
**	31-mar-1993 (jpk)
**	    remove disconnect from database (done later and a bug to
**	    attempt to do twice).
**	26-May-1993 (jpk and barbara)
**	    support case name translation for star databases
**	31-oct-1997 (nanpr01)
**	    Added +w in the connect flag to avoid E_US0014 message.
*/
static DU_STATUS
  duc_4init_star_catalogs(duc_dbenv, duc_errcb)
  DU_ENV *duc_dbenv;
    DU_ERROR	*duc_errcb;
  {
    int DU_object_base, DU_object_index, DU_index;
    char *DU_ii_name, *DU_iidd_name, *DU_obj_type, *DU_loc_type;
    char *DU_ldbidx_nam;
    char DU_nodename[DB_MAXNAME+4], *DU_home_dbms;
    i4 duc_4row_cnt;
    DU_STATUS	du_status;
    char DU_effective_dba[DB_MAXNAME+4];
    /* construct name of form node::database, so 2 for "::" */
    char node_dbname[GCC_L_NODE + DB_MAXNAME + 2 + 1];
    if (!STcompare(DU_DBDBNAME, duc_dbenv->du_dbname))
	STcopy(DU_1DBA_DBDB_FLAG, DU_effective_dba);
    else
	STprintf(DU_effective_dba, "-u%s", duc_dbenv->du_unorm_usr);
    /* Close the Database, and the re-Open the database.
    ** This is necessary because of server caching */
/* # line 3004 "ducreate.qsc" */	/* exit */
    {
      IIexit();
    }
/* # line 3005 "ducreate.qsc" */	/* host code */
    duc_dbenv->du_dbopen	= FALSE;
    /* handle createdb over the net */
    if (STlength(duc_dbenv->du_nodename) > 0)
    {
	STpolycat(3, duc_dbenv->du_nodename, ERx("::"),
	    duc_dbenv->dbnamePlusService, node_dbname);
    }
    else
    {
	STcopy(duc_dbenv->dbnamePlusService, node_dbname);
    }
/* # line 3017 "ducreate.qsc" */	/* ingres */
    {
      IIingopen(0,Duc_appl_flag,(char *)"+w",(char *)"-l",DU_1DBA_DBDB_FLAG,
      node_dbname,(char *)0);
    }
/* # line 3018 "ducreate.qsc" */	/* host code */
    duc_dbenv->du_dbopen	= TRUE;
    IIlq_Protect(TRUE);
/* # line 3021 "ducreate.qsc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"set session isolation level serializable");
      IIsyncup((char *)0,0);
    }
/* # line 3023 "ducreate.qsc" */	/* host code */
    /*
    **	get host name.
    */
    duu_get_vnode(DU_nodename);
    if (DU_nodename[0] == EOS)
    {
    	if ((du_error(duc_errcb, E_DU510F_STAR_CANT_FIND_VNODE, 
    	    0, 0)) != OK)
    	    duc_exit(duc_errcb);
    }
    duc_5init_star_capabilities(duc_dbenv, duc_errcb);
/* # line 3036 "ducreate.qsc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidd_ddb_object_base(object_base=0)");
      IIsyncup((char *)0,0);
    }
/* # line 3038 "ducreate.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&duc_4row_cnt,(char *)"rowcount");
    }
/* # line 3039 "ducreate.qsc" */	/* host code */
    if (duc_4row_cnt != 1)
    {
	/* Bad append to table, %0c, in database, %1c. */
	du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
			    0, "iidd_ddb_object_base", 0, DU_DBDBNAME);
    }
/* # line 3046 "ducreate.qsc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidd_ddb_ldbids(ldb_node=");
      IIputdomio((short *)0,1,32,0,DU_nodename);
      IIwritio(0,(short *)0,1,32,0,(char *)
", ldb_dbms=\"INGRES\", ldb_database=");
      IIputdomio((short *)0,1,32,0,duc_dbenv->du_cdbname);
      IIwritio(0,(short *)0,1,32,0,(char *)
", ldb_longname=\"N\", ldb_id=1, ldb_user=\"Y\", ldb_dba=\"Y\", ldb_db\
aname=dbmsinfo(\"dba\"))");
      IIsyncup((char *)0,0);
    }
/* # line 3055 "ducreate.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&duc_4row_cnt,(char *)"rowcount");
    }
/* # line 3056 "ducreate.qsc" */	/* host code */
    if (duc_4row_cnt != 1)
    {
	/* Bad append to table, %0c, in database, %1c. */
	du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
			    0, "iidd_ddb_ldbids", 0, duc_dbenv->du_cdbname);
    }
    /*
    **	Initialize "iidd_ddb_*" tables in CDB.
    */
    DU_home_dbms = "INGRES";
    for (DU_object_index = 0, DU_object_base = 500; 
	STcompare((char *)(DU_link_object_map[DU_object_index].ii_name), "");
	DU_object_index++, DU_object_base++)
    {
	DU_ii_name	= DU_link_object_map[DU_object_index].ii_name;
	DU_iidd_name	= DU_link_object_map[DU_object_index].iidd_name;
	DU_obj_type	= DU_link_object_map[DU_object_index].obj_type;
	DU_loc_type	= DU_link_object_map[DU_object_index].loc_type;
	if (duc_dbenv->du_dbservice & DU_NAME_UPPER)
	{
	    CVupper(DU_ii_name);
	    CVupper(DU_iidd_name);
	}
	/* Initialize iidd_tables and substitute iidd_name into ii_name */
/* # line 3085 "ducreate.qsc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 3087 "ducreate.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducreate1",7640,5278);
        IIputdomio((short *)0,1,32,0,DU_ii_name);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducreate1",7640,5278);
          IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidd_tables(table_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,32,0,DU_ii_name);
          IIwritio(0,(short *)0,1,32,0,(char *)
", table_owner=iitables.table_owner, create_date=iitables.create_date,\
 alter_date=iitables.alter_date, table_type=iitables.table_type, table\
_subtype=iitables.table_subtype, table_version=iitables.table_version,\
 system_use=\"S\", table_stats=iitables.");
          IIwritio(0,(short *)0,1,32,0,(char *)
"table_stats, table_indexes=iitables.table_indexes, is_readonly=iitabl\
es.is_readonly, num_rows=iitables.num_rows, storage_structure=iitables\
.storage_structure, is_compressed=iitables.is_compressed, duplicate_ro\
ws=iitables.duplicate_rows, unique_rule=");
          IIwritio(0,(short *)0,1,32,0,(char *)
"iitables.unique_rule, number_pages=iitables.number_pages, overflow_pa\
ges=iitables.overflow_pages, row_width=iitables.row_width, expire_date\
=iitables.expire_date, modify_date=iitables.modify_date, location_name\
=iitables.location_name, table_integrities=");
          IIwritio(0,(short *)0,1,32,0,(char *)
"\" \", table_permits=\" \", all_to_all=\"Y\", ret_to_all=\"Y\", is_jo\
urnalled=iitables.is_journalled, view_base=iitables.view_base, multi_l\
ocations=iitables.multi_locations, table_ifillpct=iitables.table_ifill\
pct, table_dfillpct=iitables.table_dfillpct, ");
          IIwritio(0,(short *)0,1,32,0,(char *)
"table_lfillpct=iitables.table_lfillpct, table_minpages=iitables.table\
_minpages, table_maxpages=iitables.table_maxpages, table_relstamp1=iit\
ables.table_relstamp1, table_relstamp2=iitables.table_relstamp2, table\
_reltid=iitables.table_reltid, table_reltidx=");
          IIwritio(0,(short *)0,1,32,0,(char *)
"iitables.table_reltidx, table_pagesize=iitables.table_pagesize, table\
_relversion=iitables.table_relversion, table_reltotwid=iitables.table_\
reltotwid)where iitables.table_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
          IIputdomio((short *)0,1,32,0,DU_iidd_name);
          IIexDefine(0,(char *)"ducreate1",7640,5278);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 3133 "ducreate.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&duc_4row_cnt,(char *)"rowcount");
    }
/* # line 3134 "ducreate.qsc" */	/* host code */
	if (duc_4row_cnt == 0)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iidd_tables", 0, duc_dbenv->du_cdbname);
	}
/* # line 3141 "ducreate.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducreate2",7641,3836);
        IIputdomio((short *)0,1,32,0,DU_ii_name);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducreate2",7641,3836);
          IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidd_columns(table_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,32,0,DU_ii_name);
          IIwritio(0,(short *)0,1,32,0,(char *)
", table_owner=iicolumns.table_owner, column_name=iicolumns.column_nam\
e, column_datatype=iicolumns.column_datatype, column_length=iicolumns.\
column_length, column_scale=iicolumns.column_scale, column_nulls=iicol\
umns.column_nulls, column_defaults=iicolumns.");
          IIwritio(0,(short *)0,1,32,0,(char *)
"column_defaults, column_sequence=iicolumns.column_sequence, key_seque\
nce=iicolumns.key_sequence, sort_direction=iicolumns.sort_direction, c\
olumn_ingdatatype=iicolumns.column_ingdatatype)where iicolumns.table_n\
ame=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
          IIputdomio((short *)0,1,32,0,DU_iidd_name);
          IIexDefine(0,(char *)"ducreate2",7641,3836);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 3158 "ducreate.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&duc_4row_cnt,(char *)"rowcount");
    }
/* # line 3159 "ducreate.qsc" */	/* host code */
	if (duc_4row_cnt == 0)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iidd_columns", 0, duc_dbenv->du_cdbname);
	}
/* # line 3166 "ducreate.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducreate3",7642,30393);
        IIputdomio((short *)0,1,30,4,&DU_object_base);
        IIputdomio((short *)0,1,32,0,DU_loc_type);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducreate3",7642,30393);
          IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidd_ddb_tableinfo(object_base=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,30,4,&DU_object_base);
          IIwritio(0,(short *)0,1,32,0,(char *)
", object_index=0, local_type=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
          IIputdomio((short *)0,1,32,0,DU_loc_type);
          IIwritio(0,(short *)0,1,32,0,(char *)", table_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
          IIputdomio((short *)0,1,32,0,DU_iidd_name);
          IIwritio(0,(short *)0,1,32,0,(char *)
", table_owner=iitables.table_owner, table_date=iitables.create_date, \
table_alter=iitables.alter_date, table_relstamp1=iitables.table_relsta\
mp1, table_relstamp2=iitables.table_relstamp2, columns_mapped=\"N\", l\
db_id=1)where iitables.table_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
          IIputdomio((short *)0,1,32,0,DU_iidd_name);
          IIexDefine(0,(char *)"ducreate3",7642,30393);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 3182 "ducreate.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&duc_4row_cnt,(char *)"rowcount");
    }
/* # line 3183 "ducreate.qsc" */	/* host code */
	if (duc_4row_cnt == 0)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iidd_ddb_tableinfo",
				0, duc_dbenv->du_cdbname);
	}
/* # line 3191 "ducreate.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducreate4",7643,32154);
        IIputdomio((short *)0,1,32,0,DU_ii_name);
        IIputdomio((short *)0,1,30,4,&DU_object_base);
        IIputdomio((short *)0,1,32,0,DU_obj_type);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducreate4",7643,32154);
          IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidd_ddb_objects(object_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,32,0,DU_ii_name);
          IIwritio(0,(short *)0,1,32,0,(char *)
", object_owner=iitables.table_owner, object_base=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
          IIputdomio((short *)0,1,30,4,&DU_object_base);
          IIwritio(0,(short *)0,1,32,0,(char *)
", object_index=0, qid1=0, qid2=0, create_date=date_gmt(date(\"now\"))\
, object_type=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
          IIputdomio((short *)0,1,32,0,DU_obj_type);
          IIwritio(0,(short *)0,1,32,0,(char *)
", alter_date=date_gmt(date(\"now\")), system_object=\"Y\", to_expire=\
\"N\")where iitables.table_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
          IIputdomio((short *)0,1,32,0,DU_iidd_name);
          IIexDefine(0,(char *)"ducreate4",7643,32154);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 3207 "ducreate.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&duc_4row_cnt,(char *)"rowcount");
    }
/* # line 3208 "ducreate.qsc" */	/* host code */
	if (duc_4row_cnt == 0)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iidd_ddb_objects", 0, duc_dbenv->du_cdbname);
	}
/* # line 3215 "ducreate.qsc" */	/* replace */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducreate5",7644,29689);
        IIputdomio((short *)0,1,30,4,&DU_object_base);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducreate5",7644,29689);
          IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidd_ddb_object_base(object_base=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,30,4,&DU_object_base);
          IIwritio(0,(short *)0,1,32,0,(char *)")");
          IIexDefine(0,(char *)"ducreate5",7644,29689);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 3220 "ducreate.qsc" */	/* end transaction */
    {
      IIxact(1);
    }
/* # line 3222 "ducreate.qsc" */	/* host code */
    }
    /* now populate CDB catalogs to propogate any indexes from System
    ** catalogs put into CDB catalogs */
    for (DU_object_index = 0, DU_index = DU_object_base+1;
	STcompare((char *)(DU_index_object_map[DU_object_index].loctab_name), "");
	DU_object_index++, DU_index++)
    {
	DU_ii_name	= DU_index_object_map[DU_object_index].loctab_name;
	DU_iidd_name	= DU_index_object_map[DU_object_index].link_name;
	DU_ldbidx_nam 	= DU_index_object_map[DU_object_index].locidx_name;
	DU_obj_type	= "L",
	DU_loc_type	= "I";
	if (duc_dbenv->du_dbservice & DU_NAME_UPPER)
	{
	    CVupper(DU_ii_name);
	    CVupper(DU_iidd_name);
	    CVupper(DU_ldbidx_nam);
	}
	/* Initialize iidd_tables and substitute iidd_name into ii_name */
/* # line 3246 "ducreate.qsc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 3248 "ducreate.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(DU_object_base=iidd_ddb_objects.object_base)where iidd_ddb_o\
bjects.object_name=");
      IIputdomio((short *)0,1,32,0,DU_ii_name);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE20;
IIrtB20:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&DU_object_base);
        if (IIerrtest() != 0) goto IIrtB20;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE20:;
    }
/* # line 3251 "ducreate.qsc" */	/* host code */
	mk_idxname (DU_iidd_name, DU_object_base, DU_index);
/* # line 3253 "ducreate.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducreate6",7645,29337);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducreate6",7645,29337);
          IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidd_tables(table_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,32,0,DU_iidd_name);
          IIwritio(0,(short *)0,1,32,0,(char *)
", table_owner=iitables.table_owner, create_date=iitables.create_date,\
 alter_date=iitables.alter_date, table_type=iitables.table_type, table\
_subtype=iitables.table_subtype, table_version=iitables.table_version,\
 system_use=\"S\", table_stats=iitables.");
          IIwritio(0,(short *)0,1,32,0,(char *)
"table_stats, table_indexes=iitables.table_indexes, is_readonly=iitabl\
es.is_readonly, num_rows=iitables.num_rows, storage_structure=iitables\
.storage_structure, is_compressed=iitables.is_compressed, duplicate_ro\
ws=iitables.duplicate_rows, unique_rule=");
          IIwritio(0,(short *)0,1,32,0,(char *)
"iitables.unique_rule, number_pages=iitables.number_pages, overflow_pa\
ges=iitables.overflow_pages, row_width=iitables.row_width, expire_date\
=iitables.expire_date, modify_date=iitables.modify_date, location_name\
=iitables.location_name, table_integrities=");
          IIwritio(0,(short *)0,1,32,0,(char *)
"iitables.table_integrities, table_permits=iitables.table_permits, all\
_to_all=\"Y\", ret_to_all=\"Y\", is_journalled=iitables.is_journalled,\
 view_base=iitables.view_base, multi_locations=iitables.multi_location\
s, table_ifillpct=iitables.table_ifillpct, ");
          IIwritio(0,(short *)0,1,32,0,(char *)
"table_dfillpct=iitables.table_dfillpct, table_lfillpct=iitables.table\
_lfillpct, table_minpages=iitables.table_minpages, table_maxpages=iita\
bles.table_maxpages, table_relstamp1=iitables.table_relstamp1, table_r\
elstamp2=iitables.table_relstamp2, ");
          IIwritio(0,(short *)0,1,32,0,(char *)
"table_reltid=iitables.table_reltid, table_reltidx=iitables.table_relt\
idx, table_pagesize=iitables.table_pagesize, table_relversion=iitables\
.table_relversion, table_reltotwid=iitables.table_reltotwid)where iita\
bles.table_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
          IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
          IIexDefine(0,(char *)"ducreate6",7645,29337);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 3299 "ducreate.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&duc_4row_cnt,(char *)"rowcount");
    }
/* # line 3300 "ducreate.qsc" */	/* host code */
	if (duc_4row_cnt == 0)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iidd_tables", 0, duc_dbenv->du_cdbname);
	}
/* # line 3307 "ducreate.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducreate7",7646,5271);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducreate7",7646,5271);
          IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidd_columns(table_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,32,0,DU_iidd_name);
          IIwritio(0,(short *)0,1,32,0,(char *)
", table_owner=iicolumns.table_owner, column_name=iicolumns.column_nam\
e, column_datatype=iicolumns.column_datatype, column_length=iicolumns.\
column_length, column_scale=iicolumns.column_scale, column_nulls=iicol\
umns.column_nulls, column_defaults=iicolumns.");
          IIwritio(0,(short *)0,1,32,0,(char *)
"column_defaults, column_sequence=iicolumns.column_sequence, key_seque\
nce=iicolumns.key_sequence, sort_direction=iicolumns.sort_direction, c\
olumn_ingdatatype=iicolumns.column_ingdatatype)where iicolumns.table_n\
ame=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
          IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
          IIexDefine(0,(char *)"ducreate7",7646,5271);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 3324 "ducreate.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&duc_4row_cnt,(char *)"rowcount");
    }
/* # line 3325 "ducreate.qsc" */	/* host code */
	if (duc_4row_cnt == 0)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iidd_columns", 0, duc_dbenv->du_cdbname);
	}
/* # line 3332 "ducreate.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducreate8",7647,5110);
        IIputdomio((short *)0,1,30,4,&DU_object_base);
        IIputdomio((short *)0,1,30,4,&DU_index);
        IIputdomio((short *)0,1,32,0,DU_loc_type);
        IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
        IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducreate8",7647,5110);
          IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidd_ddb_tableinfo(object_base=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,30,4,&DU_object_base);
          IIwritio(0,(short *)0,1,32,0,(char *)", object_index=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
          IIputdomio((short *)0,1,30,4,&DU_index);
          IIwritio(0,(short *)0,1,32,0,(char *)", local_type=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
          IIputdomio((short *)0,1,32,0,DU_loc_type);
          IIwritio(0,(short *)0,1,32,0,(char *)", table_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
          IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
          IIwritio(0,(short *)0,1,32,0,(char *)
", table_owner=iitables.table_owner, table_date=iitables.create_date, \
table_alter=iitables.alter_date, table_relstamp1=iitables.table_relsta\
mp1, table_relstamp2=iitables.table_relstamp2, columns_mapped=\"N\", l\
db_id=1)where iitables.table_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
          IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
          IIexDefine(0,(char *)"ducreate8",7647,5110);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 3348 "ducreate.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&duc_4row_cnt,(char *)"rowcount");
    }
/* # line 3349 "ducreate.qsc" */	/* host code */
	if (duc_4row_cnt == 0)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iidd_ddb_tableinfo",
				0, duc_dbenv->du_cdbname);
	}
/* # line 3357 "ducreate.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducreate9",7648,4790);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIputdomio((short *)0,1,30,4,&DU_object_base);
        IIputdomio((short *)0,1,30,4,&DU_index);
        IIputdomio((short *)0,1,32,0,DU_obj_type);
        IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducreate9",7648,4790);
          IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidd_ddb_objects(object_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,32,0,DU_iidd_name);
          IIwritio(0,(short *)0,1,32,0,(char *)
", object_owner=iitables.table_owner, object_base=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
          IIputdomio((short *)0,1,30,4,&DU_object_base);
          IIwritio(0,(short *)0,1,32,0,(char *)", object_index=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
          IIputdomio((short *)0,1,30,4,&DU_index);
          IIwritio(0,(short *)0,1,32,0,(char *)
", qid1=0, qid2=0, create_date=date_gmt(date(\"now\")), object_type=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
          IIputdomio((short *)0,1,32,0,DU_obj_type);
          IIwritio(0,(short *)0,1,32,0,(char *)
", alter_date=date_gmt(date(\"now\")), system_object=\"Y\", to_expire=\
\"N\")where iitables.table_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
          IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
          IIexDefine(0,(char *)"ducreate9",7648,4790);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 3373 "ducreate.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducreate10",7649,13716);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducreate10",7649,13716);
          IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidd_indexes(index_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,32,0,DU_iidd_name);
          IIwritio(0,(short *)0,1,32,0,(char *)
", index_owner=iiindexes.index_owner, create_date=date_gmt(date(\"now\"\
)), base_name=iiindexes.base_name, base_owner=iiindexes.base_owner, st\
orage_structure=iiindexes.storage_structure, is_compressed=iiindexes.i\
s_compressed, unique_rule=iiindexes.");
          IIwritio(0,(short *)0,1,32,0,(char *)
"unique_rule, index_pagesize=iiindexes.index_pagesize)where iiindexes.\
index_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
          IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
          IIexDefine(0,(char *)"ducreate10",7649,13716);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 3388 "ducreate.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&duc_4row_cnt,(char *)"rowcount");
    }
/* # line 3389 "ducreate.qsc" */	/* host code */
	if (duc_4row_cnt == 0)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iidd_indexes", 0,duc_dbenv->du_cdbname);
	}
/* # line 3396 "ducreate.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducreate11",7650,13235);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducreate11",7650,13235);
          IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidd_index_columns(index_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,32,0,DU_iidd_name);
          IIwritio(0,(short *)0,1,32,0,(char *)
", index_owner=iiindex_columns.index_owner, column_name=iiindex_column\
s.column_name, key_sequence=iiindex_columns.key_sequence, sort_directi\
on=iiindex_columns.sort_direction)where iiindex_columns.index_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
          IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
          IIexDefine(0,(char *)"ducreate11",7650,13235);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 3407 "ducreate.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&duc_4row_cnt,(char *)"rowcount");
    }
/* # line 3408 "ducreate.qsc" */	/* host code */
	if (duc_4row_cnt == 0)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iidd_index_columns", 
				0,duc_dbenv->du_cdbname);
	}
/* # line 3415 "ducreate.qsc" */	/* replace */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducreate12",7651,20915);
        IIputdomio((short *)0,1,30,4,&DU_index);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducreate12",7651,20915);
          IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidd_ddb_object_base(object_base=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,30,4,&DU_index);
          IIwritio(0,(short *)0,1,32,0,(char *)")");
          IIexDefine(0,(char *)"ducreate12",7651,20915);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 3420 "ducreate.qsc" */	/* end transaction */
    {
      IIxact(1);
    }
/* # line 3422 "ducreate.qsc" */	/* host code */
    }
    if (du_error(duc_errcb, I_DU0503_STAR_UTIL_GENERAL, 4,
		0, DU_0MODIFYING, 0, "System Catalogs") != E_DU_OK)
	    duc_exit(duc_errcb);
    duc_3modify_star_catalogs();
/* # line 3430 "ducreate.qsc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidd_ddb_object_base(object_base=1000)");
      IIsyncup((char *)0,0);
    }
/* # line 3435 "ducreate.qsc" */	/* host code */
    /*
    ** initialize num_rows, number_pages, overflow_pages, row_width in
    ** iidd_tables.
    */
    for (DU_index = 0;
	*(DU_obj_stat_list[DU_index].DU_table_name) != EOS;
	DU_index++)
    {
	if (duc_dbenv->du_dbservice & DU_NAME_UPPER)
	{
	    CVupper(DU_obj_stat_list[DU_index].DU_local_name);
	    CVupper(DU_obj_stat_list[DU_index].DU_table_name);
	}
/* # line 3448 "ducreate.qsc" */	/* retrieve */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducreate13",7652,22512);
        IIputdomio((short *)0,1,32,0,DU_obj_stat_list[DU_index].DU_local_name);
        IIretinit((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducreate13",7652,22512);
          IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(DU_num_rows=iitables.num_rows, DU_number_pages=iitables.numb\
er_pages, DU_overflow_pages=iitables.overflow_pages, DU_row_width=iita\
bles.row_width)where iitables.table_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,32,0,
          DU_obj_stat_list[DU_index].DU_local_name);
          IIexDefine(0,(char *)"ducreate13",7652,22512);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
      if (IIerrtest() != 0) goto IIrtE21;
IIrtB21:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&DU_obj_stat_list[DU_index].DU_num_rows);
        IIgetdomio((short *)0,1,30,4,&
        DU_obj_stat_list[DU_index].DU_number_pages);
        IIgetdomio((short *)0,1,30,4,&
        DU_obj_stat_list[DU_index].DU_overflow_pages);
        IIgetdomio((short *)0,1,30,4,&DU_obj_stat_list[DU_index].DU_row_width);
        if (IIerrtest() != 0) goto IIrtB21;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE21:;
    }
/* # line 3458 "ducreate.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"range of ii_dd_table=iidd_tables");
      IIsyncup((char *)0,0);
    }
/* # line 3460 "ducreate.qsc" */	/* replace */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducreate14",7653,23952);
        IIputdomio((short *)0,1,30,4,&DU_obj_stat_list[DU_index].DU_num_rows);
        IIputdomio((short *)0,1,30,4,&
        DU_obj_stat_list[DU_index].DU_number_pages);
        IIputdomio((short *)0,1,30,4,&
        DU_obj_stat_list[DU_index].DU_overflow_pages);
        IIputdomio((short *)0,1,30,4,&DU_obj_stat_list[DU_index].DU_row_width);
        IIputdomio((short *)0,1,32,0,DU_obj_stat_list[DU_index].DU_table_name);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducreate14",7653,23952);
          IIwritio(0,(short *)0,1,32,0,(char *)
"replace ii_dd_table(num_rows=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,30,4,&DU_obj_stat_list[DU_index].DU_num_rows);
          IIwritio(0,(short *)0,1,32,0,(char *)", number_pages=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
          IIputdomio((short *)0,1,30,4,&
          DU_obj_stat_list[DU_index].DU_number_pages);
          IIwritio(0,(short *)0,1,32,0,(char *)", overflow_pages=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
          IIputdomio((short *)0,1,30,4,&
          DU_obj_stat_list[DU_index].DU_overflow_pages);
          IIwritio(0,(short *)0,1,32,0,(char *)", row_width=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
          IIputdomio((short *)0,1,30,4,&
          DU_obj_stat_list[DU_index].DU_row_width);
          IIwritio(0,(short *)0,1,32,0,(char *)
")where ii_dd_table.table_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
          IIputdomio((short *)0,1,32,0,
          DU_obj_stat_list[DU_index].DU_table_name);
          IIexDefine(0,(char *)"ducreate14",7653,23952);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 3469 "ducreate.qsc" */	/* host code */
    }
    IIlq_Protect(FALSE);
    return(E_DU_OK);
  }
/*{
** Name: duc_5init_star_capabilities	- 
**
** Description:
**      Initialize the iidd_dbcapabilities and iidd_ddb_ldb_dbcaps tables. 
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**
** Side Effects:
**
** History:
**    03-jun-1993 (barbara)
**	Update initial iidd_dbcapabilities values if database is
**	created with non-default case-translation semantics.
**    12-oct-1993 (barbara)
**	Update initial iidd_dbcapabilities and iidd_ddb_ldb_dbcaps values
**	for non-default DB_REAL_USER_CASE.
*/
DU_STATUS
  duc_5init_star_capabilities(duc_dbenv, duc_errcb)
  DU_ENV *duc_dbenv;
    DU_ERROR    *duc_errcb;
  {
    i4	du_status;
    i4 duc_5row_cnt;
    char *DU_capability, *DU_value;
    i4 DU_id, DU_level;
    i4 DU_index;
    char *DU_delim_case, *DU_reg_case, *DU_real_user;
    if (duc_dbenv->du_dbservice & DU_NAME_UPPER)
    {
	DU_reg_case = "UPPER";
    }
    if (duc_dbenv->du_dbservice & DU_DELIM_UPPER)
    {
	DU_delim_case = "UPPER";
    }
    else if (duc_dbenv->du_dbservice & DU_DELIM_MIXED)
    {
	DU_delim_case = "MIXED";
    }
    if (duc_dbenv->du_dbservice & DU_REAL_USER_MIXED)
    {
	DU_real_user = "MIXED";
    }
    /* initialize iidd_dbcapabilities */
    for (DU_index = 0;
	 STcompare((char *)DU_dbcapabilities[DU_index].cap_capability, "");
	 DU_index++)
    {
	DU_capability	= DU_dbcapabilities[DU_index].cap_capability;
	DU_value	= DU_dbcapabilities[DU_index].cap_value;
/* # line 3536 "ducreate.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducreate15",7654,21936);
        IIputdomio((short *)0,1,32,0,DU_capability);
        IIputdomio((short *)0,1,32,0,DU_value);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducreate15",7654,21936);
          IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidd_dbcapabilities(cap_capability=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,32,0,DU_capability);
          IIwritio(0,(short *)0,1,32,0,(char *)", cap_value=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
          IIputdomio((short *)0,1,32,0,DU_value);
          IIwritio(0,(short *)0,1,32,0,(char *)")");
          IIexDefine(0,(char *)"ducreate15",7654,21936);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 3542 "ducreate.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&duc_5row_cnt,(char *)"rowcount");
    }
/* # line 3543 "ducreate.qsc" */	/* host code */
	if (duc_5row_cnt != 1)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
			    0, "iidd_dbcapabilities", 0, duc_dbenv->du_cdbname);
	}
    }
    if (duc_dbenv->du_dbservice & DU_NAME_UPPER)
    {
/* # line 3553 "ducreate.qsc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidd_dbcapabilities(cap_value=");
      IIputdomio((short *)0,1,32,0,DU_reg_case);
      IIwritio(0,(short *)0,1,32,0,(char *)
")where iidd_dbcapabilities.cap_capability=\"DB_NAME_CASE\"");
      IIsyncup((char *)0,0);
    }
/* # line 3555 "ducreate.qsc" */	/* host code */
    }
    if (   duc_dbenv->du_dbservice & DU_DELIM_UPPER
	|| duc_dbenv->du_dbservice & DU_DELIM_MIXED)
    {
/* # line 3559 "ducreate.qsc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidd_dbcapabilities(cap_value=");
      IIputdomio((short *)0,1,32,0,DU_delim_case);
      IIwritio(0,(short *)0,1,32,0,(char *)
")where iidd_dbcapabilities.cap_capability=\"DB_DELIMITED_CASE\"");
      IIsyncup((char *)0,0);
    }
/* # line 3561 "ducreate.qsc" */	/* host code */
    }
    if (duc_dbenv->du_dbservice & DU_REAL_USER_MIXED)
    {
/* # line 3564 "ducreate.qsc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidd_dbcapabilities(cap_value=");
      IIputdomio((short *)0,1,32,0,DU_real_user);
      IIwritio(0,(short *)0,1,32,0,(char *)
")where iidd_dbcapabilities.cap_capability=\"DB_REAL_USER_CASE\"");
      IIsyncup((char *)0,0);
    }
/* # line 3566 "ducreate.qsc" */	/* host code */
    }
    else if (duc_dbenv->du_dbservice & DU_NAME_UPPER)
    {
/* # line 3569 "ducreate.qsc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidd_dbcapabilities(cap_value=");
      IIputdomio((short *)0,1,32,0,DU_reg_case);
      IIwritio(0,(short *)0,1,32,0,(char *)
")where iidd_dbcapabilities.cap_capability=\"DB_REAL_USER_CASE\"");
      IIsyncup((char *)0,0);
    }
/* # line 3571 "ducreate.qsc" */	/* host code */
    }
/* # line 3574 "ducreate.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&duc_5row_cnt,(char *)"rowcount");
    }
/* # line 3575 "ducreate.qsc" */	/* host code */
    if (duc_5row_cnt != 1)
    {
	/* Bad append to table, %0c, in database, %1c. */
	du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
			0, "iidd_dbcapabilities", 0, duc_dbenv->du_cdbname);
    }
    /* initialize iidd_ddb_ldb_dbcaps */
    for (DU_index = 0;
	 STcompare((char *)DU_dbcaps_value[DU_index].cap_capability, "");
	 DU_index++)
    {
	DU_id		= DU_dbcaps_value[DU_index].ldb_id;
	DU_capability	= DU_dbcaps_value[DU_index].cap_capability;
	DU_value	= DU_dbcaps_value[DU_index].cap_value;
	DU_level	= DU_dbcaps_value[DU_index].cap_level;
/* # line 3592 "ducreate.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducreate16",7655,20143);
        IIputdomio((short *)0,1,30,4,&DU_id);
        IIputdomio((short *)0,1,32,0,DU_capability);
        IIputdomio((short *)0,1,32,0,DU_value);
        IIputdomio((short *)0,1,30,4,&DU_level);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducreate16",7655,20143);
          IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidd_ddb_ldb_dbcaps(ldb_id=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,30,4,&DU_id);
          IIwritio(0,(short *)0,1,32,0,(char *)", cap_capability=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
          IIputdomio((short *)0,1,32,0,DU_capability);
          IIwritio(0,(short *)0,1,32,0,(char *)", cap_value=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
          IIputdomio((short *)0,1,32,0,DU_value);
          IIwritio(0,(short *)0,1,32,0,(char *)", cap_level=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
          IIputdomio((short *)0,1,30,4,&DU_level);
          IIwritio(0,(short *)0,1,32,0,(char *)")");
          IIexDefine(0,(char *)"ducreate16",7655,20143);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 3600 "ducreate.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&duc_5row_cnt,(char *)"rowcount");
    }
/* # line 3601 "ducreate.qsc" */	/* host code */
	if (duc_5row_cnt != 1)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
			    0, "iidd_ddb_ldb_dbcaps", 0, duc_dbenv->du_cdbname);
	}
    }
    if (duc_dbenv->du_dbservice & DU_NAME_UPPER)
    {
/* # line 3611 "ducreate.qsc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidd_ddb_ldb_dbcaps(cap_value=");
      IIputdomio((short *)0,1,32,0,DU_reg_case);
      IIwritio(0,(short *)0,1,32,0,(char *)
")where iidd_ddb_ldb_dbcaps.cap_capability=\"DB_NAME_CASE\"");
      IIsyncup((char *)0,0);
    }
/* # line 3613 "ducreate.qsc" */	/* host code */
    }
    if (   duc_dbenv->du_dbservice & DU_DELIM_UPPER
	|| duc_dbenv->du_dbservice & DU_DELIM_MIXED)
    {
/* # line 3617 "ducreate.qsc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidd_ddb_ldb_dbcaps(cap_value=");
      IIputdomio((short *)0,1,32,0,DU_delim_case);
      IIwritio(0,(short *)0,1,32,0,(char *)
")where iidd_ddb_ldb_dbcaps.cap_capability=\"DB_DELIMITED_CASE\"");
      IIsyncup((char *)0,0);
    }
/* # line 3619 "ducreate.qsc" */	/* host code */
    }
    if (duc_dbenv->du_dbservice & DU_REAL_USER_MIXED)
    {
/* # line 3622 "ducreate.qsc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidd_ddb_ldb_dbcaps(cap_value=");
      IIputdomio((short *)0,1,32,0,DU_real_user);
      IIwritio(0,(short *)0,1,32,0,(char *)
")where iidd_ddb_ldb_dbcaps.cap_capability=\"DB_REAL_USER_CASE\"");
      IIsyncup((char *)0,0);
    }
/* # line 3624 "ducreate.qsc" */	/* host code */
    }
    else if (duc_dbenv->du_dbservice & DU_NAME_UPPER)
    {
/* # line 3627 "ducreate.qsc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidd_ddb_ldb_dbcaps(cap_value=");
      IIputdomio((short *)0,1,32,0,DU_reg_case);
      IIwritio(0,(short *)0,1,32,0,(char *)
")where iidd_ddb_ldb_dbcaps.cap_capability=\"DB_REAL_USER_CASE\"");
      IIsyncup((char *)0,0);
    }
/* # line 3629 "ducreate.qsc" */	/* host code */
    }
/* # line 3631 "ducreate.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&duc_5row_cnt,(char *)"rowcount");
    }
/* # line 3632 "ducreate.qsc" */	/* host code */
    if (duc_5row_cnt != 1)
    {
	/* Bad append to table, %0c, in database, %1c. */
	du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
			0, "iidd_ddb_ldb_dbcaps", 0, duc_dbenv->du_cdbname);
    }
    return(E_DU_OK); 
  }
/*{
** Name: duc_gmake_gdb	- creates the gdb catalogs and initialize their values.
**
** Description:
**	Create, modify and initialize Gateway catalogs.  GATEWAY SPECIFIC
**	routing; we know upon entry that we're invoked for some Gateway.
**
** Inputs:
**	gateway				Which gateway is this call for.
**      duc_dbenv                       Ptr to DU_ENV
**	duc_errcb			Ptr to error-handling control block.
**
** Outputs:
**      *duc_errcb                      If an error occurs, this struct will be
**					set by a call to du_error().
**	Returns:
**	    E_DU_OK			Completed successfully.
**	Exceptions:
**	    none
**
** Side Effects:
**	    Creates, modifies and initializes iigwnn_relation, iigwnn_attribute,
**		iigwnn_index catalogs in a Gateway database.
**
** History:
**
**      19-Jun-89 (linda)
**          added to DUCREATEDB.QC for creating and initializing Gateway
**	    catalogs.  project - RMS Gateway.
**      22-Jun-89 (linda)
**          First created for RMS Gateway.  Table names are iigw02_relation,
**	    iigw02_attribute and iigw02_index (02 is code for the RMS
**	    Gateway).
**	24-dec-89 (paul)
**	    Catalogs types must match those in the internal struct descriptors
**	    for the catalogs. Changed 'text' types to 'char' types so that
**	    the catalogs match the structures.
**	    Also add another field to iigw02_attribute to contain the offset
**	    of the field in the rms file.
**	19-Mar-90 (fourt)
**	    Paste this one routine together from duc_2makegdb() and
**	    duc_3create_iigw_catalogs(), which are now gone, and generalize
**	    for future multiple gateways.
**	8-may-90 (linda)
**	    Final changes to catalog structures for RMS Gateway.  In
**	    particular:  (a) remove i4 gprecision field and replace it with i2
**	    gprec_scale (precision & scale, precision in high byte and scale in
**	    low byte).  Leave glength because for types other than packed
**	    decimal it is handy.  Also want an i4 field to easily store lengths
**	    > 32767 if needed in the future (RMS tapes can have records
**	    >32767...).  (b) remove the source and interp fields (both
**	    char(255)) from the iigw02_index table, they are not necessary.
**	18-may-90 (linda)
**	    Final (!) changes to iigw02_attribute catalog.  Add "gflags" field,
**	    analogous to "attflags" in iiattribute.  Used at present to
**	    indicate (a) whether an extended format was specified and (b)
**	    whether the attribute is a variant.
**	9-jul-90 (linda)
**	    Change name of GW_RMS_FLAG to DU_GW_RMS_FLAG, to correspond to
**	    naming conventions in commonhdr!dudbms.qh.
**      14-aug-90 (pete)
**          Add support for dictionary client names on createdb command line.
**	19-sep-90 (pete)
**	    Move defintion of iiduNoFeClients from here to duicrdb.qsc.
**	    (was causing link problems in fecvt60, which calls dui_makedb).
**	31-oct-90 (pete)
**	    Fix bug 33855, where creation of Star databases would get screwed
**	    up if any flags followed the CDBname.
**
*/
DU_STATUS
  duc_gmake_gdb(gateway, duc_dbenv, duc_errcb)
    i4			gateway;
  DU_ENV *duc_dbenv;
    DU_ERROR		*duc_errcb;
  {
    /* Emit informative msg that we're creating system catalogs */
    if ((du_error(duc_errcb, I_DU0521_GW_UTIL_GENERAL, 4,
	    0, DU_0CREATING, 0, "System Catalogs"))
	    != E_DU_OK)
	duc_exit(duc_errcb);
    switch (gateway)
    {
	case  DU_GW_RMS_FLAG:  /* Gateway to Vax VMS RMS file system */
/* # line 3727 "ducreate.qsc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"create iigw02_relation(xreltid=i4, xreltidx=i4, file_name=char(255))");
      IIsyncup((char *)0,0);
    }
/* # line 3733 "ducreate.qsc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"create iigw02_attribute(xrelid=i4, xrelidx=i4, xattid=i2, gtype=i4, g\
length=i4, goffset=i4, gprec_scale=i2, gflags=i2)");
      IIsyncup((char *)0,0);
    }
/* # line 3744 "ducreate.qsc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"create iigw02_index(xbaseid=i4, xindexid=i4, key_ref=i2)");
      IIsyncup((char *)0,0);
    }
/* # line 3751 "ducreate.qsc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"modify iigw02_relation to hash on xreltid");
      IIsyncup((char *)0,0);
    }
/* # line 3752 "ducreate.qsc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"modify iigw02_attribute to hash on xrelid, xrelidx");
      IIsyncup((char *)0,0);
    }
/* # line 3753 "ducreate.qsc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"modify iigw02_index to hash on xbaseid");
      IIsyncup((char *)0,0);
    }
/* # line 3754 "ducreate.qsc" */	/* host code */
		break;
	default:   /* 'Impossible' to get here */
		(void) duc_abort_internal(duc_errcb, DU_0GMAKE);
		/* never returns, but -- */
		return (E_DU_IERROR);
    }
    return(E_DU_OK);
  }
/*{
** Name: duc_get_args()	-   parse the command line initializing modes
**			    for the database utility, createdb.
**
** Description:
**        This routine parses the command line to set the mode flags
**	for the createdb database utility.
**
** Inputs:
**      argc                            Command line argument count.
**	argv				Command line argument vector.
**	duc_modes			Ptr to a DUC_MODE_VECT which
**					will describe the various modes
**					which createdb can operate under.
**	duc_dbenv			Ptr to a DU_ENV struct which describes
**					DBMS environment for a database utility.
**	duc_errcb			Ptr to error-handling control block.
**
** Outputs:
**      *duc_modes
**	    .duc_dbdb_flag		True, if the "dbdb" is being created.
**	    .duc_star_flag		True, if a distributed database is
**					to be created.
**	    .duc_private_flag		True, if the database being created is
**					to be private.
**	    .duc_dbloc_flag		True, if a "default" database location
**					has been specified.
**	    .duc_ckploc_flag		True, if a checkpointing location has
**					been specified.
**	    .duc_jnlloc_flag		True, if a journaling location has
**					been specified.
**	    .duc_sortloc_flag		True, if a sorting location has been
**					specified.
**	    .duc_rdonly_flag		True, if a "readonly" database location
**					has been specified.
**	    .duc_progname		The name of the program being
**					executed.
**	    .duc_client_name		Names of FE dictionary clients on
**					command line. If no clients, then
**					duc_client_name[0] == EOS
**	*duc_dbenv
**	    .du_dbname			Name of the database to be created.
**	    .du_cdbname			Name of the coordinator database to
**					be created.
**	    .du_ddbname			Name of the distributed database to
**					be created.
**	    .du_usrinit			Processes' user name.
**	    .du_xusrname		Name of the effective user.  This will
**					be different from .du_usrinit only if
**					the "-u" flag is used.
**	    .du_dba_target		The dba for the database to be created,
**					translated wrt new db semantics.
**					In this application, this will always
**					be the same as the effective user.
**	    .du_ckploc.du_locname	Name of the checkpointing location.
**	    .du_dbloc.du_locname	Name of the default database location.
**	    .du_jnlloc.du_locname	Name of the journaling location.
**	    .du_sortloc.du_locname	Name of the default sorting location.
**	    .du_access			Accessibility status of database.
**	    .du_collation		Database collation
**	*duc_errcb			If an error occurs, this will be set
**					by a call to du_error().
**
**	Returns:
**	    E_DU_OK			Completed successfully.
**	    E_DU3045_BAD_LOCNAME	A location name parameter was syntac-
**					tically wrong.
**	    E_DU3000_BAD_USRNAME	The user name parameter associated with
**					the -u flag was syntactically wrong.
**	    E_DU3100_UCODE_OLDUSAGE	User tried to use "user code" syntax
**					with the -u flag.
**	    E_DU3121_UNKNOWN_FLAG_CR	An unknown flag was passed on the
**					command line.
**	    E_DU3010_BAD_DBNAME		The database name parameter was
**					syntactically incorrect.
**	    E_DU3014_NO_DBNAME		No database name was given on the
**					command line.
**	    E_DU3132_BAD_DBDBNAME_CR	The "dbdb" flag was used on the command
**					line with a database name other than
**					"dbdb".
**	    E_DU3133_NO_DBDB_FLAG_CR	The database name given was "dbdb", but
**					the '-S' flag was not used on the
**					command line.
**	    E_DU3070_BAD_COLLATION	Invalid collation name specified
**	    E_DU3512_BAD_CLIENT_NM	The special client name "nofeclients"
**					was mixed with another client name.
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      13-Oct-86 (ericj)
**          Initial creation.
**      16-Jul-88 (alexc)
**          modified for TITAN, takes '/d' at the end of a database name as
**          a STAR database indicator.
**	14-May-89 (anton)
**	    Added collation flag
**      22-mar-90 (fourt)
**          Add logic to handle '/rms' on dbname to signal creation of an RMS
**          Gateway database.  Added CVlower's to cut down number of
**          comparisons.  Add call to duc_ginit_dbname() to set up dbname for
**          a Gateway db.
**	14-aug-90 (pete)
**	    Add support for dictionary client names on Createdb command line,
**	    and for the special client name: 'nofeclients'. Earlier support for
**	    -f flag removed.
**	10-oct-1990 (pete)
**	    Because of ambiguity between cdb names and client names,
**	    reintroduce the -f flag (with blessing of the LRC).
**	30-oct-90 (teresa)
**	    fix bug 33855.
**	16-apr-92 (teresa)
**	    '\/' -> '/' to make sun4 compiler stop complaining.
**	14-Jan-93 (jpk)
**	    Add parsing of -wlocationame for work locations
**	    and warning (but accept) for deprecated -slocation
**	    for sort locations.
**	15-Jan-93 (jpk)
**	    Add parsing of -DELIM_ID_CASE={UPPER,LOWER,MIXED}
**	    and -REG_ID_CASE={UPPER,LOWER}.
**	15-July-1993 (jpk)
**	    Set dbservice values correctly for LDB, CDB, DDB.
**	20-July-1993 (jpk)
**	    Add parsing of -REAL_USER_CASE={UPPER,LOWER}.
**	    Add validation of REG_ID_CASE, DELIM_ID_CASE,
**	    REAL_USER_CASE values.
**	09-nov-93 (rblumer)
**	    added duc_xlate_username function to correctly case-translate the
**	    username for the target db and the iidbdb.  Removed uses of du_dba
**	    and du_usrname; replaced them with du_dba_target, du_xusrname, and
**	    du_unorm_usr.
**	    Also added code to unnormalize any name specified using -u;
**	    this should allow delimited usernames to be more fully supported.
**	20-Feb-1994 (jpk)
**	    ifdef-ed out DELIM_ID_CASE, REG_ID_CASE, and REAL_USER_CASE.
**	    As per LRC decision they are now seen only when compiled
**	    with xDEBUG defined.  Installations may not contain both
**	    FIPS and ingres databases.
**	27-March-1994 (jpk)
**	    Put FIPS flags under control of conf_DELIM_TEST, not xDEBUG.
**	3-nov-1995 (angusm)
**		- Remove ifdef'd out DELIM_ID_CASE, REG_ID_CASE, 
**		and REAL_USER_CASE - these are now PM items.
***		- remove calls to du_chk3_locname - this is
**		  obsolete and should be replaced by checks
**		  for delim ids etc. Also, force II_JOURNAL etc to lowercase
**		  regardless - this is a historical inconsistency...
** 	22-apr-1997 (wonst02)
** 	    Added '-r' and duc_rdonly_flag to support readonly databases.
**	19-mar-2001 (stephenb)
**	    Add '-n' flag to indicate "national" database, i.e. one that
**	    supports nchar and nvarchar types.
**	2-apr-2001 (stephenb)
**	    Rename default Unicode collation table.
**	29-sep-2004 (devjo01)
**	    Replace du_chk3_locname with cui_chk3_locname.
**	09-may-2007 (gupsh01)
**	    Added check to create the unicode enabled database
**	    for UTF8 charactersets if either -n nor -i are given.
**	31-aug-2007 (gupsh01)
**	    Return the collation name with E_DU3070_BAD_COLLATION,
**	    when a unicode collation supplied is not valid.
*/
DU_STATUS
duc_get_args(argc, argv, duc_modes, duc_dbenv, duc_errcb)
i4		    argc;
char		    *argv[];
DUC_MODE_VECT	    *duc_modes;
DU_ENV		    *duc_dbenv;
DU_ERROR	    *duc_errcb;
{
    char	    *p;
    CL_ERR_DESC	    syserr;
    ADULTABLE	    *tbl;
    ADUUCETAB	    *utbl;
    char	    *uvtab;
    void	    du_get_usrname();
    static i4       dbname_arg = 0;
    static i4	    cdbname_arg = 0;
    char            *dbms="";
    char            *tail_str;
    i4              num_clients = 0;	/* nmbr dict client names on cmd line */
    bool	    client_flag = FALSE;      /* true if -f flag given */
    bool	    dash_u_specified = FALSE; /* true if -u flag given */
    u_i4	    idmode = 0;
    u_i4	    normal_len;
    DB_STATUS	    status;
    DB_ERROR	    error;
    i4		    du_status;
    char	    *id_flag_val;
    char	    *colon_ptr;		/* to detect node name syntax */
     i4             page_size; 
    bool	    dash_i_specified = FALSE;
    bool	    dash_n_specified = FALSE;
    /* Get the process's user name */
    du_get_usrname(duc_dbenv->du_usrinit);
    if (du_chk2_usrname(duc_dbenv->du_usrinit) != OK)
    {
	return(du_error(duc_errcb, E_DU3000_BAD_USRNAME, 2,
			0, duc_dbenv->du_usrinit));
    }
    else
    {
	/* Initialize the effective user name to that of the process's
	** user name.
	*/
	STcopy(duc_dbenv->du_usrinit, duc_dbenv->du_xusrname);
	STcopy(duc_dbenv->du_usrinit, duc_dbenv->du_unorm_usr);
    }
    /* Initialize the default locations */
    STcopy(ING_CKDIR, duc_dbenv->du_ckploc.du_locname);
    STcopy(ING_JNLDIR, duc_dbenv->du_jnlloc.du_locname);
    STcopy(ING_DBDIR, duc_dbenv->du_dbloc.du_locname);
    STcopy(ING_WORKDIR, duc_dbenv->du_sortloc.du_locname);
    STcopy(ING_DMPDIR, duc_dbenv->du_dmploc.du_locname);
    duc_modes->duc_national_flag = FALSE;
    duc_modes->duc_unicode_nfc = FALSE;
    duc_modes->duc_page_size = 0;
	/*   This is to work round (historical) inconsistency w.r.
	**   ii_database etc, which need to be passed to BE in lowercase
	**   irrespective of SQL-92 or default
	*/
	CVlower(duc_dbenv->du_ckploc.du_locname);
	CVlower(duc_dbenv->du_jnlloc.du_locname);
	CVlower(duc_dbenv->du_dbloc.du_locname);
	CVlower(duc_dbenv->du_sortloc.du_locname);
	CVlower(duc_dbenv->du_dmploc.du_locname);
    /* determine default collation */
    duc_dbenv->du_collation[0] = EOS;
    duc_dbenv->du_ucollation[0] = EOS;
    duc_dbenv->du_uninorm_flag = 0;
    NMgtAt("II_COLLATION", &p);
    if (p && *p)
    {
	STncpy( duc_dbenv->du_collation, p, DB_MAXNAME);
    }
    /* The first argument is the program being executed -- see if they
    ** stuck the /D or /STAR flags on the program name -- on most systems
    ** this will not occur.  The OS will parse the '/d' or '/star' into
    ** a separate argv[] parameter.  However, this little test should cover
    ** any OS that do not do this... */
    tail_str = STchr(argv[0], '/');
    if (tail_str)
    {
        /* YEP, the put the /D or /STAR where it did not belong, on the 
        ** command name.  Lets be forgiving, patch up the command name
        ** and remember that they want to make a DDB
        */
        CVlower(tail_str);
        if ( !STcompare(tail_str, "/d") || !STcompare(tail_str, "/star"))
        {
            dbms = "STAR";
            *tail_str = EOS;
        }
    }
    STcopy(argv[0], duc_modes->duc_progname);
/*
we can't validate location-names here, since
we need the settings for REG_ID and DELIM_ID
from iidbdb. Copy the values now and validate later
in duc_db_evinit()
*/
    while (--argc)
    {
	p   = *(++argv);
	switch (p[0])
	{
/*          case '\/': */
          case '/':
            CVlower(p);
            if ( !STcompare( p, "/d") ||  !STcompare( p, "/star"))
            {
                /* the intent is to make a distributed db, so flag it */
                dbms = "STAR";
            }
            break;
	  case '-':
	    switch (p[1])
	    {
	      case 'S':
		duc_modes->duc_dbdb_flag	= TRUE;
		break;
	      case 'p':
		if (!STcompare(p, "-p") || !STcompare(p, "-private"))
		{
		    duc_modes->duc_private_flag   = TRUE;
		    break;
		}
                if (STncasecmp(&p[1], "page_size=", 10 ) == 0)
                {
                    status = CVal(&p[11], &page_size);
                    if (status == OK &&
                            (page_size == 2048 || page_size == 4096
                            || page_size == 8192 || page_size == 16384
                            || page_size == 32768 || page_size == 65536))
                    {
                        duc_modes->duc_page_size = page_size;
                        break;
                    }
                    else
                      return du_error(duc_errcb, E_DU3303_BAD_PAGESIZE, 2, 0, page_size);  
                }
		return(du_error(duc_errcb, E_DU3121_UNKNOWN_FLAG_CR, 2, 0, p));
		break;
	      case 'd':
		if (! duc_modes->duc_rdonly_flag)
		{
		    duc_modes->duc_dbloc_flag = TRUE;
		    STcopy(&p[2], duc_dbenv->du_dbloc.du_locname);
		    duc_dbenv->du_dbloc.du_status = DU_DBS_LOC;
		}
		else
		{
		    return(du_error(duc_errcb, E_DU3122_CONFLICTING_FLAGS, 4,
			   0, "-d", 0, "-r"));
		}
		break;
	      case 'n':
	      case 'i':
		duc_modes->duc_national_flag = TRUE;
		if (p[1] == 'i')
		{
		  duc_dbenv->du_uninorm_flag = DU_UNICODE_NFC; 
    		  duc_modes->duc_unicode_nfc = TRUE;
    		  dash_i_specified = TRUE;
		}
		else
    		  dash_n_specified = TRUE;
                if (p[2] != EOS)
		{
		    if (cui_chk3_locname(&p[2]) != OK)
		    {
		    	return(du_error(duc_errcb, E_DU3070_BAD_COLLATION, 2,
				    0, &p[2]));
		    }
		    else
		    	STcopy(&p[2], duc_dbenv->du_ucollation);
		}
		else
    		    /* for unicode, use the default collation table unless 
    		    ** another is specified */
    		    STcopy("udefault", duc_dbenv->du_ucollation);
		break;
	      case 'r':
		if (! duc_modes->duc_dbloc_flag)
		{
		    duc_modes->duc_rdonly_flag = TRUE;
		    STcopy(&p[2], duc_dbenv->du_dbloc.du_locname);
		    duc_dbenv->du_dbloc.du_status = DU_DBS_LOC | DU_RDONLY_LOC;
		}
		else
		{
		    return(du_error(duc_errcb, E_DU3122_CONFLICTING_FLAGS, 4,
			   0, "-r", 0, "-d"));
		}
		break;
	      case 'j':
		    duc_modes->duc_jnlloc_flag	= TRUE;
		    STcopy(&p[2], duc_dbenv->du_jnlloc.du_locname);
		    break;
	      case 'c':
		    duc_modes->duc_ckploc_flag	= TRUE;
		    STcopy(&p[2], duc_dbenv->du_ckploc.du_locname);
		    break;
		  case 'b':
            duc_modes->duc_dmploc_flag  = TRUE;
            STcopy(&p[2], duc_dbenv->du_dmploc.du_locname);
            break;
	      case 's':		/* deprecated so warn but accept */
		du_error(duc_errcb, W_DU1085_SORTFLAG_DEPRECATED, 2,
			 0, &p[2]);
	      case 'w':
		    duc_modes->duc_sortloc_flag	= TRUE;
		    STcopy(&p[2], duc_dbenv->du_sortloc.du_locname);
		    break;
	      case 'u':
		if (p[2] == ':')
		{
		    return(du_error(duc_errcb, E_DU3100_UCODE_OLDUSAGE, 0));
		}
		else
		{
		    dash_u_specified = TRUE;
		    STcopy(&p[2], duc_dbenv->du_unorm_usr);
		    /* normalize the identifier
		    ** (i.e. remove any double-quote delimiters)
		    */
		    normal_len = sizeof(duc_dbenv->du_xusrname);
		    status = cui_idxlate(
			       (u_char *) duc_dbenv->du_unorm_usr, (u_i4 *) 0,
			       (u_char *) duc_dbenv->du_xusrname,  &normal_len,
			       (u_i4) CUI_ID_DLM_M, 
			       &idmode, &error);
		    if (DB_FAILURE_MACRO(status))
		    {
			/* since can't look up DBMS error returned in
			** error.err_code, use default 'bad username' error.
			*/
			du_status = du_error(duc_errcb, E_DU3000_BAD_USRNAME, 
					     2, 0, duc_dbenv->du_unorm_usr);
			return(du_status);
		    }
		    /* null-terminate the string
		     */
		    duc_dbenv->du_xusrname[normal_len] = EOS;
		    if (du_chk2_usrname(duc_dbenv->du_xusrname) != OK)
		    {
			return(du_error(duc_errcb, E_DU3000_BAD_USRNAME, 2,
			       0, duc_dbenv->du_xusrname));
		    }
		    break;
		}
	      case 'l':
		if (cui_chk3_locname(&p[2]) != OK)
		{
		    return(du_error(duc_errcb, E_DU3070_BAD_COLLATION, 2,
				    0, &p[2]));
		}
		else
		{
		    STcopy(&p[2], duc_dbenv->du_collation);
		    break;
		}
              case 'f':
		client_flag = TRUE;
                if (p[2] != EOS)
		{
		    /* check if "nofeclients" appears it is the only client */
		    if (num_clients &&
		    	duc_goodClientList(duc_modes->duc_client_name,
						&p[2]) != OK)
		    {
		        /* error. "nofeclients" mixed with another client nm */
                        return(du_error(duc_errcb, E_DU3512_BAD_CLIENT_NM, 2,
                            (i4 *) NULL, iiduNoFeClients));
		    }
		    /* A name was attached to -f flag; tack it onto what we
		    ** already have (if anything). NOTE: multiple -f flags
		    ** are allowed, but not documented (LRC wanted it that
		    ** way!).
		    */
                    STpolycat (3, duc_modes->duc_client_name,
				 (num_clients == 0) ? ERx("") : ERx(" "),
				 &p[2],
				 duc_modes->duc_client_name);
		    num_clients++ ;
		}
		/* multiple client names can appear following the -f
		** flag (e.g. -fingres visibuild windows_4gl).
		** Scan for more client names, or another flag,
		** or end of arguments.
		*/
		/* argc & argv should be incremented together, so don't do
		** it as part of the conditional on argc (e.g. "if (--argc)").
		*/
		for (--argc, p = *(++argv); argc ; --argc, p = *(++argv))
		{
		    if (p[0] == '-' || p[0] == '+' || p[0] == '/')
		    {
			/* uh oh, another flag. we're done with client list */
			break;
		    }
		    else
		    {
		        /*check if "nofeclients" appears it is the only client*/
			if (duc_goodClientList(duc_modes->duc_client_name, p)
				!= OK)
			{
		            /* error. "nofeclients" mixed with other client nm*/
                            return(du_error(duc_errcb, E_DU3512_BAD_CLIENT_NM,
				2, (i4 *) NULL, iiduNoFeClients));
		        }
			/* argv is a client name */
                        STpolycat (3, duc_modes->duc_client_name,
				(num_clients == 0) ? ERx("") : ERx(" "),
				 p,
				 duc_modes->duc_client_name);
		        num_clients++ ;
		    }
		}
		/* We're pointing at either last argument in list, which is
		** a client name, or at a flag that follows -f flag. Either way
		** we've gone one too far. Back up one argument to let outer
		** loop handle it.
		*/
		argc++;
		argv--;
                break;
	      default:
		return(du_error(duc_errcb, E_DU3121_UNKNOWN_FLAG_CR, 2, 0, p));
		break;
	    }	/* end of swith (p[1]) */
	    break;
	  case '+':
	    return(du_error(duc_errcb, E_DU3121_UNKNOWN_FLAG_CR, 2, 0, p));
	    break;
	  default:
            /* Must be a database name parameter -- must determine if it is
            ** a Local DB (LDB) or Distributed DB (DDB) request 
            */
            if (dbname_arg != 0)
            {
                    return(du_error(duc_errcb, E_DU510E_STAR_TOO_MANY_ARGS, 0));
                    /* specified database name twice */
            }
            dbname_arg++;
	    duc_dbenv->du_nodename[0] = EOS;	/* initialize */
	    /* check for a node name of form node::database */
	    if (((colon_ptr = STchr(p, ':')) != NULL) && 
		STchr(colon_ptr + 1, ':') == (colon_ptr + 1))
	    {
		/* copy nodename, everything to left of "::" */
		STncpy( duc_dbenv->du_nodename, p, colon_ptr - p);
		duc_dbenv->du_nodename[colon_ptr - p] = '\0';
		/* copy database name, everything to right of "::" */
		STcopy(colon_ptr + 2, p);
	    }
            tail_str = STchr(p, '/');
            if (tail_str)
            {
                /* Something is prepended to name.  See if it is a valid 
                ** distributed request flag --
                */
                CVlower(tail_str);
                if ( !STcompare(tail_str, "/d") || 
                     !STcompare(tail_str, "/star"))
                {
                    dbms = "STAR";
                    *tail_str = EOS; /* take flag off of ddb name */
                }
                /* -- or an RMS Gateway database */
                else if (!STcompare(tail_str, "/rms"))
                {
                    dbms = "RMS";
                    *tail_str = EOS;
                }
                /* -- or an INGRES database */
                else if (!STcompare(tail_str, "/ingres"))
                {
                    dbms = "INGRES";
                    *tail_str = EOS;
                }
            }
            if (!STcompare(dbms, "STAR"))
            {
                STcopy(p, duc_dbenv->du_ddbname);
                if (du_chk1_dbname(duc_dbenv->du_ddbname) != OK)
                {
                    return(du_error(duc_errcb, E_DU5109_STAR_INVALID_NAME,
                                    4, 0, "Distributed", 0, duc_dbenv->du_ddbname));
                }
		if (argc - 1 > 0)
  		{
		    char	    *next_p;
		    /* the parameter following DDB_name/STAR may be the CDB
		    ** name or it may be another flag.  If there is another
		    ** parameter after DDB_NAME and it is NOT a flag, then
		    ** use it as CDB name. (bug 33855)
		    */
		    next_p=argv[1];
		    if (next_p[0] != '-')
		    {
			STcopy( next_p, duc_dbenv->du_cdbname);
			argc--;
			argv++;
		    }
  		}
                else
                {
                    STcopy("", duc_dbenv->du_cdbname);
                }
                if (du_chk1_dbname(duc_dbenv->du_cdbname) != OK)
                {
                    if (STlength(duc_dbenv->du_ddbname) > 30) 
                        /* need to find out the define for this */
                    {
                        return(du_error(duc_errcb, 
                            E_DU5108_STAR_DDB_NAME_TOO_LONG, 2, 0, 30));
                        /* DDB name is too long to be modify as CDB name by
			** adding "ii" it
			*/
                    }
                    if (du_chk1_dbname(duc_dbenv->du_cdbname) != OK)
                    {
                        STcopy("ii", duc_dbenv->du_cdbname);
                        STcat(duc_dbenv->du_cdbname, duc_dbenv->du_ddbname);
                    }
                    else
                    {
                        return(du_error(duc_errcb, E_DU5109_STAR_INVALID_NAME,
                                        4, 0, "Coordinator", 0, duc_dbenv->du_cdbname));
                    }
                }
                duc_modes->duc_star_flag = TRUE;
                STcopy(duc_dbenv->du_cdbname, duc_dbenv->du_dbname);
                if (!STcompare(duc_dbenv->du_cdbname, duc_dbenv->du_ddbname))
                    return(du_error(duc_errcb, E_DU510D_STAR_DDB_EQS_CDB, 0));
                break;
            }
            else  if (!STcompare(dbms, "RMS"))
            {
                /* recall p by now has "/rms" truncated */
                duc_ginit_dbname(DU_GW_RMS_FLAG, p, duc_modes, duc_dbenv,
                    duc_errcb);
            }
            else 
            {
                /* neither distributed nor gateway, so must be local */
                if (du_chk1_dbname(p) == OK)
                {
                    STcopy(p, duc_dbenv->du_dbname);
                }
                else
                {
                    return(du_error(duc_errcb, E_DU3010_BAD_DBNAME, 2, 0,
				    p));
                }
                break;
            }
	}   /* end of switch(p[0]) */
    }	/* end of while loop */
    /* make sure identifier case params are legal */
    if (validate_id_case_params(duc_dbenv, duc_errcb) != OK)
    {
	SIprintf("\n");
	return du_error(duc_errcb, E_DU3519_LEGAL_REG_DELIM_ID, 0);
    }
    /* Check if -n flag was given with -i flag */
    if (dash_i_specified && dash_n_specified)
	return(du_error(duc_errcb, E_DU6521_UNORM_NFC_WITH_NFD, 0));
    /* If II_CHARSET is set to UTF8 then create the database 
    ** unicode enabled database with NFC normalization form 
    */
    if (!(dash_n_specified) && !(dash_i_specified) && CM_ischarsetUTF8())
    {
        dash_i_specified = TRUE;
        duc_modes->duc_national_flag = TRUE;
        duc_modes->duc_unicode_nfc = TRUE;
        duc_dbenv->du_uninorm_flag = DU_UNICODE_NFC; 
        STcopy("udefault", duc_dbenv->du_ucollation); 
	/* gets the unicode collation by default */
    }
    /* Check if -f flag was given with no clients */
    if (client_flag && (num_clients == 0))
	return(du_error(duc_errcb, E_DU3514_EMPTY_CLIENT_LIST,
		2, (i4 *) NULL, ERx("createdb")));
    /* just can not create the "dbdb" with any STAR options - "/d" */
    if (duc_modes->duc_star_flag
	&& !STcompare(duc_dbenv->du_ddbname, "dbdb"))
    {
	return(du_error(duc_errcb, E_DU510C_STAR_DBDB_NOT_LEGAL, 2,
		0, duc_dbenv->du_ddbname));
    }
    if (duc_modes->duc_star_flag
	&& !STcompare(duc_dbenv->du_cdbname, "dbdb"))
    {
	return(du_error(duc_errcb, E_DU510C_STAR_DBDB_NOT_LEGAL, 2,
		0, duc_dbenv->du_cdbname));
    }
    if (duc_modes->duc_star_flag
	&& (!STcompare(duc_dbenv->du_ddbname, DU_DBDBNAME) 
	|| !STcompare(duc_dbenv->du_cdbname, DU_DBDBNAME)))
    {
	return(du_error(duc_errcb, E_DU3132_BAD_DBDBNAME_CR, 2,
		    0, duc_dbenv->du_ddbname));
    }
    /* cannot create a private distributed database */
    if ( (duc_modes->duc_star_flag) && (duc_modes->duc_private_flag) )
    {
	return(du_error(duc_errcb, E_DU5107_PRIVATE_STAR, 2, 0,
			duc_dbenv->du_ddbname));
    }
    /* Check if a database name was given. */
    if (duc_dbenv->du_dbname[0] == EOS)
	return(du_error(duc_errcb, E_DU3014_NO_DBNAME, 0));
    /* If the "-S" flag was used, validate the database name. */
    if (duc_modes->duc_dbdb_flag
	&& STcompare(duc_dbenv->du_dbname, DU_DBDBNAME)
       )
	return(du_error(duc_errcb, E_DU3132_BAD_DBDBNAME_CR, 2,
			0, duc_dbenv->du_dbname));
    /* If the database name is "dbdb", make sure the '-S' flag was used. */
    if (!STcompare(duc_dbenv->du_dbname, DU_DBDBNAME)
	&& !duc_modes->duc_dbdb_flag
       )
	return(du_error(duc_errcb, E_DU3133_NO_DBDB_FLAG_CR, 0));
    /* Check that the collation exists */
    if (*duc_dbenv->du_collation
      && aducolinit(duc_dbenv->du_collation, MEreqmem, &tbl, &syserr) != OK)
    {
	return(du_error(duc_errcb, E_DU3070_BAD_COLLATION, 2,
		 0, duc_dbenv->du_collation));
    }
    if (*duc_dbenv->du_ucollation && aduucolinit(duc_dbenv->du_ucollation, 
	MEreqmem, &utbl, &uvtab, &syserr) != OK)
    {
	return(du_error(duc_errcb, E_DU3070_BAD_COLLATION, 2,
		 0, duc_dbenv->du_ucollation));
    }
    /* translate the user name according to necessary
    ** case-translation semantics
    */
    du_status = duc_xlate_username(duc_dbenv, dash_u_specified, 
				   idmode, duc_errcb);
    if (du_status != E_DU_OK)
	return(du_status);
    if (duc_modes->duc_private_flag)
	duc_dbenv->du_access = DU_CREATEDB;
    else
	duc_dbenv->du_access = DU_CREATEDB | DU_GLOBAL;
    if (duc_modes->duc_rdonly_flag)
	duc_dbenv->du_access |= DU_RDONLY;
#ifdef DU_PARSEDEBUG
    duu_dumpdbenv(duc_dbenv, duc_modes);
    return (E_DU_UERROR);	/* don't create the database */
#endif
    return(E_DU_OK);
}
#ifdef DU_PARSEDEBUG
/* dump portions of DU_ENV and DUC_MODE_VECT */
/*
** 	22-apr-1997 (wonst02)
** 	    Added duc_rdonly_flag to support readonly databases.
*/
duu_dumpdbenv(duc_dbenv, duc_modes)
DU_ENV              *duc_dbenv;
DUC_MODE_VECT       *duc_modes;
{
	SIprintf (ERx("\n*** DU_MODE_VECT:\n"));
	    SIprintf
	  (ERx("\tduc_dbdb_flag=%d, duc_private_flag=%d, duc_dbloc_flag=%d\n"),
		duc_modes->duc_dbdb_flag, duc_modes->duc_private_flag,
		duc_modes->duc_dbloc_flag);
	    SIprintf
	 (ERx("\tduc_jnlloc_flag=%d, duc_dmploc_flag=%d, duc_ckploc_flag=%d\n"),
		duc_modes->duc_jnlloc_flag, duc_modes->duc_dmploc_flag,
		duc_modes->duc_ckploc_flag);
	    SIprintf
	    (ERx("\tduc_sortloc_flag=%d, duc_rdonly_flag=%d\n"),
		duc_modes->duc_sortloc_flag, duc_modes->duc_rdonly_flag);
	    SIprintf
	    (ERx("\tduc_star_flag=%d, duc_gw_flag=%d\n"),
		duc_modes->duc_star_flag, duc_modes->duc_gw_flag);
	    SIprintf
	    (ERx("\tduc_progname=%s,\n\tduc_client_name=%s.\n"),
		duc_modes->duc_progname, duc_modes->duc_client_name);
	SIprintf (ERx("\n*** DU_ENV:\n"));
	    SIprintf (ERx("\tdu_dbname=%s, du_dbcmptlvl=%s\n"),
		duc_dbenv->du_dbname, duc_dbenv->du_dbcmptlvl);
	    SIprintf (ERx("\tdu_usrinit=%s, du_xusrname=%s\n"),
		duc_dbenv->du_usrinit, duc_dbenv->du_xusrname);
	    SIprintf (ERx("\tdu_unorm_usr=%s,\n"),
		duc_dbenv->du_unorm_usr);
	    SIprintf (ERx("\tdu_dba_target=%s, du_dba_iidbdb=%s\n"),
		duc_dbenv->du_dba_target, duc_dbenv->du_dba_iidbdb);
	    SIprintf(ERx("\tdu_dba=%s, du_collation=%s, du_ddbname=%s\n"),
		duc_dbenv->du_dba, duc_dbenv->du_collation,
		duc_dbenv->du_ddbname);
	    SIprintf(ERx("\tdu_cdbname=%s, du_gdbname=%s.\n"),
		duc_dbenv->du_cdbname, duc_dbenv->du_gdbname);
}
#endif
/*{
** Name: duc_xlate_username()	- translate the username according to various
** 				  case-translation semantics; also normalize it
**
** Description:
**	This routine will decide what case translation semantics are to be
**	used for the target database and the iidbdb, and fill in du_dba_target
**	and du_dba_iidbdb appropriately.
**	It assume the du_xusrname has already been normalized (i.e. all
**	double-quote delimiters and escape character have been removed).
**	
** Inputs:
**	duc_dbenv
**	    .du_xusrname		Name of the effective user.  This will
**					be different from .du_usrinit only if
**					the "-u" flag is used.
** Outputs:
**	duc_dbenv
**	    .du_dba_target		The dba for the database to be created,
**					translated according to the (new)
**					target db's semantics.
**	    .du_dba_target		The dba for the database to be created,
**					translated according to the iidbdb's
**					semantics.
**
**
**	Returns:
**		E_DU_OK, E_DU_UERROR
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	12-nov-93 (rblumer)
**	    created.
**	17-nov-93 (rblumer)
**	    forgot to return E_DU_OK on success, which caused bug on VMS.
**	12-nov-1998 (kitch01)
**		Bug 89657. In an SQL92 installation the effective userid is compared
**		to the actual userid in duc_db_envinit(). As the actual userid has been
**		converted to uppercase this test fails. If -u is not specified then
**		ensure du_xusrname is same case as du_dba_target for which it was used
**		as input. 
*/
static
DU_STATUS
duc_xlate_username(
		   DU_ENV	*duc_dbenv,
		   bool		dash_u_specified,
		   u_i4	idmode,
		   DU_ERROR	*duc_errcb)
{
    DB_STATUS	status;
    DB_ERROR	error;
    i4		du_status;
    u_i4	normal_len;
    u_i4	xlate_semantics = CUI_ID_NORM;	/* assume user name is
						** already normalized  */
    /* By default, the effective user will be the dba of the
    ** database to be created (i.e. the target db).
    ** 
    ** First translate it according to the target db's semantics.
    ** 
    ** if -u was specified,
    **     if idmode is regular,
    **         use reg_id_case semantics
    **     else
    **         use delim_id_case semantics
    ** If -u wasn't specified, use real_user_case semantics;
    **     
    */
    /* set up target db case semantics
     */
    if (dash_u_specified)
    {
	if (idmode & CUI_ID_REG)
	{
	    xlate_semantics |= CUI_ID_REG;
	    if (STequal(duc_dbenv->du_reg_id_case, "UPPER"))
		xlate_semantics |= CUI_ID_REG_U;
	    else
		xlate_semantics |= CUI_ID_REG_L;
	}
	else
	{
	    xlate_semantics |= CUI_ID_DLM;
	    if (STequal(duc_dbenv->du_delim_id_case, "LOWER"))
		xlate_semantics |= CUI_ID_DLM_L;
	    else if (STequal(duc_dbenv->du_delim_id_case, "UPPER"))
		xlate_semantics |= CUI_ID_DLM_U;
	    else
		xlate_semantics |= CUI_ID_DLM_M;
	}
    }
    else
    {
	xlate_semantics |= CUI_ID_DLM;
	if (STequal(duc_dbenv->du_real_user_case, "LOWER"))
	    xlate_semantics |= CUI_ID_DLM_L;
	else if (STequal(duc_dbenv->du_real_user_case, "UPPER"))
	    xlate_semantics |= CUI_ID_DLM_U;
	else
	    xlate_semantics |= CUI_ID_DLM_M;
    }
    normal_len = sizeof(duc_dbenv->du_dba_target);
    status = cui_idxlate((u_char *) duc_dbenv->du_xusrname, (u_i4 *) 0,
			 (u_char *) duc_dbenv->du_dba_target, &normal_len,
			 xlate_semantics, (u_i4 *) NULL, &error);
	/* Bug 89657. If SQL92 is used then the make sure the effective user is 
	** same case as target. This prevents the alias check failing later with
	** E_DU3004_NO_ALIAS_FLAG.
	*/
	if (!dash_u_specified)
		STcopy(duc_dbenv->du_dba_target, duc_dbenv->du_xusrname);
    if (DB_FAILURE_MACRO(status))
    {
	/* since can't look up DBMS error returned in error.err_code,
	** use default 'bad username' error.
	*/
	du_status = du_error(duc_errcb, E_DU3000_BAD_USRNAME, 
			     2, 0, duc_dbenv->du_xusrname);
	return(du_status);
    }
    /* null-terminate the string
     */
    duc_dbenv->du_dba_target[normal_len] = EOS;
    /* for now, just lower-case dba-for-iidbdb,
    ** since that is the current behavior           (rblumer 11/09/93).
    ** Eventually should fetch iidbdb semantics from iidatabase.
    */
    STcopy(duc_dbenv->du_xusrname, duc_dbenv->du_dba_iidbdb);
      CVlower(duc_dbenv->du_dba_iidbdb); 
/*** FIX ME  (rblumer 11/09/93)   **************
****
**** until we remove this variable entirely,
**** let's set it to something like the current behavior
***/
    STcopy(duc_dbenv->du_dba_iidbdb, duc_dbenv->du_dba);
/****************************/
    return(E_DU_OK);
}  /* end duc_xlate_username */
/*{
** Name: duc_init() -	initialize variables and structs used by createdb.
**
** Description:
**        This routine initializes the variables and structs used by 
**	createdb.
**
** Inputs:
**      duc_modes                       Ptr to a DUC_MODE_VECT struct.
**	duc_dbenv			Ptr to a DU_ENV struct.
**	duc_errcb			Ptr to the error-handling control block.
**
** Outputs:
**      *duc_modes			The initialized DUC_MODE_VECT.
**	*duc_dbenv			The initialized DU_ENV.
**	*duc_errcb			The initialized error-handling control
**					block.
**	Returns:
**	    E_DU_OK			Completed successfully.
**	Exceptions:
**	    none
**
** Side Effects:
**	      Initializes the global variable Duc_dbenv to point to the
**	    DUC_MODE_VECT address that was passed in.
**
** History:
**      13-Oct-86 (ericj)
**          Initial creation.
**      20-Mar-90 (fourt)
**          Initialize gateway flag duc_gw_flag too.
**      14-aug-90 (pete)
**          Initialize new duc_modes->duc_client_name.
** 	22-apr-1997 (wonst02)
** 	    Added duc_rdonly_flag to support readonly databases.
**	21-feb-05 (inkdo01)
**	    Belatedly init duc_national_flag and duc_unicode_nfc.
*/
DU_STATUS
duc_init(duc_modes, duc_dbenv, duc_errcb)
DUC_MODE_VECT		*duc_modes;
DU_ENV			*duc_dbenv;
DU_ERROR		*duc_errcb;
{
    /* Initialize the system utility environment */
    du_envinit(duc_dbenv);
    Duc_dbenv		= duc_dbenv;
    /* Initialize the system utility error-handling struct */
    du_reset_err(duc_errcb);
    /* fix_me */
    /* Ds_errcb	= du_errcb; */
    /* Initialize the createdb modes vector */
    duc_modes->duc_dbdb_flag	= FALSE;
    duc_modes->duc_star_flag	= FALSE;
    duc_modes->duc_private_flag	= FALSE;
    duc_modes->duc_dbloc_flag	= FALSE;
    duc_modes->duc_rdonly_flag	= FALSE;
    duc_modes->duc_jnlloc_flag	= FALSE;
    duc_modes->duc_ckploc_flag	= FALSE;
    duc_modes->duc_sortloc_flag	= FALSE;
    duc_modes->duc_dmploc_flag  = FALSE;
    duc_modes->duc_gw_flag      = FALSE;
    duc_modes->duc_national_flag = FALSE;
    duc_modes->duc_unicode_nfc = FALSE;
    duc_modes->duc_client_name[0]= EOS;
    duc_modes->duc_progname[0]	= EOS;
    /* Added for modifing all system catalogs*/
    duc_modes->duc_all_catalogs = TRUE; 
    Dus_allcat_defs[DU_CORE_CATS]       = &Dub_01corecat_defs[0];
    Dus_allcat_defs[DU_DBMS_CATS]       = &Dub_11dbmscat_defs[0];
    Dus_allcat_defs[DU_IIDBDB_CATS]     = &Dub_31iidbdbcat_defs[0];
    Dus_allcat_defs[DU_DDB_CATS]        = &Dub_41ddbcat_defs[0];
    Dus_allcat_defs[DU_SCI_CATS]        = &Dui_51scicat_defs[0];
    dub_corecats_def();
    if( dub_dbmscats_def() != E_DU_OK )
	return(E_DU_FATAL);
    dub_ddbcats_def();
    dub_iidbdbcats_def();     
    return(E_DU_OK);
}
/*{
** Name: duu_get_vnode	- gets II_GCNxx_LCL_VNODE form system attributes.
**
** Description: 
**
** Inputs:
**      None.
**
** Outputs:
**      duu_curr_vnode                  - Has EOS if fail to get vnode, else
**					    will store value of VNODE.
**					  Length DB_MAXNAME+4 minimum.
**	Returns:
**	Exceptions:
**
** Side Effects:
**
** History:
**      17-mar-1989 (alexc)
**          Initial Creation.
**      21-apr-1989 (alexc)
**          lowercase VNODE entry.
**	24-jan-1993 (jpk)
**	    Use PM lookup instead of NM lookup;
**	    find value in config.dat instead of symbol.tbl.
**	18-jun-93 (ed)
**	    GChostname does not have a return status
**	07-dec-93 (tyler)
**	    Replaced GChostname() with call to PMhost().
**	17-Jun-2004 (schka24)
**	    Use STlcopy instead of STcopy for safety.
**	1-Jul-2004 (schak24)
**	    Length oops in above, fix.  Nobody checks error return, make void.
*/
static void
duu_get_vnode(duu_curr_vnode)
char	duu_curr_vnode[];
{
    char    duu_v_log_node[DB_MAXNAME+4];
    char    *duu_vnode_p;
    char    *duu_installation;
    duu_curr_vnode[0] = EOS;		/* init so returns work */
    PMinit();
    if (PMload( (LOCATION *) NULL,  (PM_ERR_FUNC *) NULL) == OK)
    {
	if( ( PMsetDefault(1, PMhost() ) == OK )
	    && 
	    (	(PMget("ii.$.gcn.local_vnode", &duu_vnode_p) == OK)
		|| 
		(PMget("ii.$.gcn.lcl_vnode", &duu_vnode_p) == OK)
	    ))
	{
	    STlcopy(duu_vnode_p, duu_curr_vnode, DB_MAXNAME+4);
	    CVlower(duu_curr_vnode);
	    PMfree();
	    return;
        }
    }
    PMfree();
    NMgtAt("II_INSTALLATION", &duu_installation);
    STlpolycat(3, sizeof(duu_v_log_node)-1, "II_GCN",
	    (duu_installation == NULL ? "" : duu_installation),
	    "_LCL_VNODE",
	    duu_v_log_node);
    NMgtAt(duu_v_log_node, &duu_vnode_p);
    if (duu_vnode_p == NULL || *duu_vnode_p == EOS)
    {
	duu_curr_vnode[0] = EOS;
    }
    else
    {
	STlcopy(duu_vnode_p, duu_curr_vnode, DB_MAXNAME+4);
    }
    CVlower(duu_curr_vnode);
    return;
}
/*{
** Name: duc_abort_internal() -- halt processing due to internal (coding) error.
**				 "Impossible" that this should ever be executed.
**
** Description:
**	Something "impossible" has happened, so we print an error msg and exit.
**
** Inputs:
**	duc_errcb			Ptr to error control block.
**	rou_name			char-str name of routine calling us.
**
** Outputs:
**	duc_errcb
**		.du_status		Error status, set to E_DU_IERROR.
**
** Returns:
**	Never returns, since calls duc_exit().  But if somehow duc_exit()
**	returns, we will return E_DU_IERROR.
**
** Exceptions:
**	None raised here.
**
** Side Effects:
**	Prints error msg, calls duc_exit() to halt us.
**
** History:
**	19-Mar_90 (fourt)
**	Created.
*/
DU_STATUS
duc_abort_internal(duc_errcb, rou_name)
DU_ERROR	*duc_errcb;
char		*rou_name;
{
	duc_errcb->du_status = E_DU_IERROR;
	SIprintf(DU_0INT_ERR, rou_name);
	/* Duc_exit() never returns, but -- */
	return (E_DU_IERROR);
}
/*{
** Name: duc_ginit_dbname() -	store dbname and type in environment.
**
** Description:
**		Store dbname and type in environment.  GATEWAY-SPECIFIC
**		routine; we know upon entry which particular Gateway we're
**		being called for.
**
** Inputs:
**	gateway				Int defining which gateway this is for.
**      ustr				String dbname.
**	duc_modes			Ptr to a DU_MODE_VECT struct.
**	duc_dbenv			Ptr to a DU_ENV struct.
**	duc_errcb			Ptr to the error-handling control block.
**
** Outputs:
**	*duc_modes
**		.duc_gw_flag		Will contain argument 'gateway'.
**	*duc_dbenv
**		.dbname			Will contain "<dbname>".
**		.gdbname		Also will contain "<dbname>".
**	*duc_errcb			If an error occurs, this will be set by 
**					a call to du_error().
**
**	Returns:
**	    E_DU_OK, unless something "impossible" happens.
**
**	Exceptions:
**	    None.
**
** Side Effects:
**	None.
**
** History:
**      16-mar-90 (fourt)
**          Moved out of duc_get_args, generalized for (future) multiple
**		gateways.
*/
DU_STATUS
duc_ginit_dbname(gateway, ustr, duc_modes, duc_dbenv, duc_errcb)
i4			gateway;
char			*ustr;
DUC_MODE_VECT		*duc_modes;
DU_ENV			*duc_dbenv;
DU_ERROR		*duc_errcb;
{
	/* process on per-gateway basis */
	switch (gateway)
	{
	  case  DU_GW_RMS_FLAG:	/* Vax VMS RMS Gateway */
	        STcopy(ustr, duc_dbenv->du_gdbname);
	        if (du_chk1_dbname(duc_dbenv->du_gdbname) != OK)
	        {
			return(du_error(duc_errcb, E_DU5130_GW_INVALID_NAME,
	    		        4, 0, DU_0RMS_GATEWAY, 0,
	    			duc_dbenv->du_gdbname));
	        }
	        duc_modes->duc_gw_flag = gateway;
	        STcopy(duc_dbenv->du_gdbname, duc_dbenv->du_dbname);
		break;
	  default:	/* "Impossible" to arrive here */
		(void) duc_abort_internal(duc_errcb, DU_0GINIT);
		/* Never returns, but -- */
		return (E_DU_IERROR);
	}
	return (E_DU_OK);
}
/*{
** Name:  duc_gset_service()  - returns db service tag for a gateway.
**
** Description:
**	Given a particular gateway's code, return the code for that gateway's
**	service, as should be stored in 'dbservice' column of 'iidatabase'
**	table.  GATEWAY SPECIFIC routine; we know upon entry that we're
**	creating a Gateway database.
**
** Inputs:
**	gateway			Nat, which gateway is this.
**	duc_errcb		Ptr to error control block, in case of disaster.
**
** Outputs:
**	None.
**
** Returns:
**	Code for the proper service, a nat.
**
** Exceptions:
**	None.
**
** Side Effects:
**	None.
**
** History:
**	20-Mar-90 (fourt)
**	    Created.
*/
i4
duc_gset_service(gateway, duc_errcb)
i4	    gateway;
DU_ERROR    *duc_errcb;
{
    i4	    service;
    switch (gateway)
    {
      case  DU_GW_RMS_FLAG:    /* Vax VMS RMS gateway */
	service = DU_3SER_RMS;
	break;
      default:		    /* "Impossible" to arrive here */
	(void) duc_abort_internal(duc_errcb, DU_0GSET);
	/* Never returns, but -- we know 0 is an innocuous value -- */
	service = 0;
    }
    return (service);
}		
/*{
** Name: duc_gmsg_creating() -- issue per-gateway msg that we're creating
**				 a database.
**
** Description:
**	Writes an individual message, depending on which Gateway this command
**	was invoked for, telling the user that we're creating his Gateway
**	Database.  GATEWAY SPECIFIC routine -- we know upon entry that we're
**	being called for some Gateway.
**	
** Inputs:
**	gateway			Which gateway this call is for.
**	duc_dbenv		Ptr to environment struct.
**	duc_errcb		Ptr to error control block.
**
** Outputs:
**	None.
**
** Returns:
**	E_DU_OK, unless "impossible" error happens, in which case E_DU_IERROR.
**
** Side Effects:
**	Types a message out to the user.
**
** History:
**	21-Mar-90 (fourt)
**		Code snitched out of main(), generalized for future
**		multiple gateways.
*/
DU_STATUS
duc_gmsg_creating(gateway, duc_dbenv, duc_errcb)
i4		gateway;
DU_ENV		*duc_dbenv;
DU_ERROR	*duc_errcb;
{
	switch (gateway)
	{
	    case  DU_GW_RMS_FLAG:	/* Vax VMS RMS file system gateway */
		if((du_error(duc_errcb, I_DU0520_GW_UTIL_INTRO, 6,
		    0, DU_0CREATING, 0, DU_0RMS, 0, duc_dbenv->du_gdbname))
		    != E_DU_OK)
		    duc_exit(duc_errcb);
		break;
	    default:	/* "Impossible" to arrive here */
		(void) duc_abort_internal(duc_errcb, DU_0GMSG);
		/* Never returns, but -- */
		return (E_DU_IERROR);
	}
	return (E_DU_OK);
}
/*{
** Name: duc_dbdb_envinit() -	initialize the environment for creating the
**				"dbdb"
**
** Description:
**        This routine initializes the environment for the database utility,
**	createdb.  This involves finding the user in the "users" file and
**	setting their status accordingly.
**
** Inputs:
**      duc_dbenv                       Ptr to a DU_ENV struct.
**	    .du_usrinit			The user's login name.
**
**
** Outputs:
**      *duc_dbenv
**	    .du_usrstat			Gets set to the user status of the
**					user's login name.
**	    .du_xusrname			The user's effective name gets set
**					to DU_DBA_DBDB, the omnipotent one.
**	    .du_dba			Same as .du_xusrname.
**	    .du_dbloc			Default database location.
**		.du_status		Status of the proposed db location.
**		.du_area		The area associated with this location.
**	    .du_jnlloc			Default journaling location.
**		.du_status		Status of the proposed journal location.
**		.du_area		The area associated with this location.
**	    .du_ckploc			Default checkpointing location.
**		.du_status		Status of the proposed checkpoint
**					location.
**		.du_area		The area associated with this location.
**	    .du_sortloc			Default sorting location.
**		.du_status		Status of the proposed sort
**					location.
**		.du_area		The area associated with this location.
**	*duc_errcb			If an error occurs, this struct will
**					be set by a call to du_error().
**
**	Returns:
**	    E_DU_OK			Completed successfully.
**	    E_DU3171_USRF_NOT_FOUND	Users record corresponding to
**					.du_usrinit was not found in the
**					"users" file.
**	    E_DU3172_BAD_USRF_RECORD	A bad record was found in the
**					"users" file.
**	Exceptions:
**	    none
**
** Side Effects:
**	      The "users" file in ING_UTILITY is indirectly opened and
**	    closed through the call to du_find_usr().
**
** History:
**      10-Oct-86 (ericj)
**          Initial creation.
**	5-Feb-93 (jpk)
**	    Stop reading users file (elided from compilation for now).
*/
DU_STATUS
duc_dbdb_envinit(duc_modes, duc_dbenv, duc_errcb)
DUC_MODE_VECT		*duc_modes;
DU_ENV			*duc_dbenv;
DU_ERROR		*duc_errcb;
{
    char	usr_buf[DU_MAXLINE + 1];
    DU_USR	usr_rec;
    DU_STATUS	du_get_dbid();
    /* XXX get rid of users file */
#if 0
    /* Get the user record from the "users" file */
    if (du_find_usr(duc_dbenv->du_xusrname, usr_buf) != OK)
	return(du_error(duc_errcb, E_DU3171_USRF_NOT_FOUND, 2,
			0, duc_dbenv->du_xusrname));
    /* Parse the user record into fields */
    if (du_brk_usr(usr_buf, &usr_rec) != OK)
	return(du_error(duc_errcb, E_DU3172_BAD_USRF_RECORD, 0));
    duc_dbenv->du_usrstat    = usr_rec.du_ustatus;
#endif
    duc_dbenv->du_usrstat    = 0;
    /* Get the default database id for the database database. */
    if (du_get_dbid(duc_modes, duc_dbenv, TRUE, duc_errcb) != E_DU_OK)
	return(duc_errcb->du_status);
    return(E_DU_OK);
}
/*{
** Name:  duc_goodClientList()  - checks if client list is good.
**
** Description:
**	Check if the client list is ok. Currently, just checks if
**	"nofeclients" appears with some other client.
**
** Inputs:
**	client_list	list of clients saved so far.
**	this_client	current client to be added to list.
**
** Outputs:
**	None.
**
** Returns:
**	OK if client list is ok and this one can be added; FAIL otherwise.
**
** Exceptions:
**	None.
**
** Side Effects:
**	None.
**
** History:
**	10-oct-90 (pete)
**	    Initial Version.
*/
static STATUS
duc_goodClientList(client_list, this_client)
char	*client_list;
char	*this_client;
{
	if ( (STcasecmp(client_list, iiduNoFeClients) == 0)
	     ||
	     (*client_list != EOS
	      && (STcasecmp(this_client, iiduNoFeClients) == 0)
	     ) )
	{
	    /* Either we've already seen a client named "nofeclients", so
	    ** it's not legal to see any more clients,
	    ** Or another client already seen and the current one says
	    ** "nofeclients". Either way, it's bad news.
	    */
	    return (FAIL);
	}
	else
	{
	    return (OK);
	}
}
#define MAXFLAGSIZE	100		/* cmd line flag arg size */
/*{
** Name: cmdflagarg - parse command line flags with arguments
**
** Description:
**	Parse command lines like -DELIM_ID_CASE={UPPER,LOWER,MIXED}.
**	Comparisons are case insensitive but flag arg value is
**	returned as it was, preserving its case.
**
** Inputs:
**	candidate, string that may be of form FOO=BAR 
**	cmdflag, command line flag of form FOO
**	okvals, permissible value of cmd line flag e.g. "BAR,UPPER,LOWER,MIXED"
**
** Returns:
**	ptr to cmd line flag arg value e.g. BAR
**
** History:
**	10-Jan-93 (jpk)
**	    initial creation
*/
char	*
cmdflagarg(candidate, cmdflag, okvals)
char	*candidate;
char	*cmdflag;
char	*okvals;
{
    static char	buff[MAXFLAGSIZE];
    char	*p_candidate;		/* ptr to candidate string */
    char	*p_okvals_s = okvals;	/* ptr to beginning of a valid value */
    char	*p_okvals_e = okvals;	/* ptr to end of a valid value */
    buff[0] = '\0';		/* initialize so returns work */
    if (STncasecmp(candidate, cmdflag, STlength(cmdflag) ) != 0)
    {
	return buff;		/* cmd flag itself not found */
    }
    /* if "=" not found, or found but not right after flag */
    if (((p_candidate = STchr(candidate, '=')) == NULL) ||
	(p_candidate != (candidate + STlength(cmdflag))))
    {
	return buff;		/* then "=" not found in right place */
    }
    CMnext(p_candidate);	/* move pointer past "=" */
    do				/* compare to at least one valid value */
    {
	/* set ptr to ","; if "," not found, set ptr to end of string */
	if ((p_okvals_e = STchr(p_okvals_s, ',')) == NULL)
	{
	    p_okvals_e = okvals + STlength(okvals);
	}
	if (STncasecmp(p_candidate, p_okvals_s, p_okvals_e - p_okvals_s ) == 0)
	{
	    STncpy( buff, p_candidate, p_okvals_e - p_okvals_s);
	    buff[p_okvals_e - p_okvals_s] = '\0';
	    return buff;		/* return matching value */
	}
	if (p_okvals_e == (okvals + STlength(okvals)))
	{
	    return buff;		/* no matching values found */
	}
	else				/* else still loooking */
	{
	    p_okvals_s = p_okvals_e;
	    CMnext(p_okvals_s);
	}
    } while (1);
}
/*{
** Name: get_pm_params - get PM parameters regarding database creation
**
** Description:
**	Users can specify installation wide FIPS databases, or
**	the opposite, using config.dat.  This routine makes those
**	settings take effect.  Of course user can also override
**	the config.dat defaults by specifying something else
**	on the createdb command line.
**
** Inputs:
**      duc_dbenv                       Ptr to a DU_ENV struct.
**					Saves PM params until we
**					write them to iidbcapabilities.
**      duc_errcb                       Ptr to a DU_ERROR struct.
**
** Outputs:
**	None
**	*duc_errcb			If an error occurs, this struct will
**					be set by a call to du_error().
**
** History:
**	10-Jan-93 (jpk)
**	    initial creation
**	25-jun-93 (robf)
**	    Add check for Secure level (B1/C2) and INGRES_LOW if applicable
**	20-july-1993 (jpk)
**		added PM lookup for new parameter: real user case.
**	20-Aug-1993 (jpk)
**		Made call to GChostname type correct.
**	15-nov-93 (robf)
**              Return a status 
**	07-dec-93 (tyler)
**		Replaced GChostname() with call to PMhost().
*/
STATUS
get_pm_params(duc_dbenv, duc_errcb)
DU_ENV		*duc_dbenv;
DU_ERROR	*duc_errcb;
{
    char	*val;
    /* yes, createdb has a default default default.  If PM lookup fails,
    ** or neither parameter is set in config.dat, and no command
    ** line arguments specify a setting, this is how databases will
    ** be created:
    */
    STcopy("LOWER", duc_dbenv->du_reg_id_case);
    STcopy("LOWER", duc_dbenv->du_delim_id_case);
    STcopy("LOWER", duc_dbenv->du_real_user_case);
    duc_dbenv->du_got_delim_id_case = duc_dbenv->du_got_reg_id_case
	= duc_dbenv->du_got_real_user_case = 'd';
    /* thus, if user wants installation wide FIPS databases, she can
    ** set appropriate parameters in config.dat or via cbf; or if she wants
    ** this database being created right now to be FIPS, she can so specify
    ** on the createdb command line; lacking all that, databases
    ** are not FIPS, the way God intended.
    */
    PMinit();
    if (PMload( (LOCATION *) NULL,  (PM_ERR_FUNC *) NULL) != OK)
    {
	du_error(duc_errcb, W_DU1086_CANT_LOAD_PM_FILE, 0);
	return FAIL;
    }
    if( PMsetDefault( 1, PMhost() ) != OK )
    {
	du_error(duc_errcb, W_DU1088_PMSETDEFAULT_FAILED, 0);
	return FAIL;
    }
    if (PMget("ii.$.createdb.reg_id_case", &val) == OK)
    {
	CVlower(val);
	if (STequal(val, "lower") != 0)
	{
	    STcopy("LOWER", duc_dbenv->du_reg_id_case);
	    duc_dbenv->du_got_reg_id_case = 'p';
	}
	if (STequal(val, "upper") != 0)
	{
	    STcopy("UPPER", duc_dbenv->du_reg_id_case);
	    duc_dbenv->du_got_reg_id_case = 'p';
	}
	/* else needn't copy, use default default default */
    }
    if (PMget("ii.$.createdb.delim_id_case", &val) == OK)
    {
	CVlower(val);
	if (STequal(val, "lower") != 0)
	{
	    STcopy("LOWER", duc_dbenv->du_delim_id_case);
	    duc_dbenv->du_got_delim_id_case = 'p';
	}
	if (STequal(val, "upper") != 0)
	{
	    STcopy("UPPER", duc_dbenv->du_delim_id_case);
	    duc_dbenv->du_got_delim_id_case = 'p';
	}
	if (STequal(val, "mixed") != 0)
	{
	    STcopy("MIXED", duc_dbenv->du_delim_id_case);
	    duc_dbenv->du_got_delim_id_case = 'p';
	}
	/* else needn't copy, use default default default */
    }
    if (PMget("ii.$.secure.level", &val) == OK)
    {
	CVlower(val);
	if (STequal(val,"c2") != 0)
	{
		duc_dbenv->du_secure_level=DU_SECURE_C2;
	}
	else
		duc_dbenv->du_secure_level=DU_SECURE_NONE;
    }
    else
	duc_dbenv->du_secure_level=DU_SECURE_NONE;
    duc_dbenv->du_ingres_high=NULL;
    duc_dbenv->du_ingres_low=NULL;
    duc_dbenv->du_ingres_low=NULL;
    duc_dbenv->du_ingres_high=NULL;
    if (PMget("ii.$.createdb.real_user_case", &val) == OK)
    {
	CVlower(val);
	if (STequal(val, "lower") != 0)
	{
	    STcopy("LOWER", duc_dbenv->du_real_user_case);
	    duc_dbenv->du_got_real_user_case = 'p';
	}
	if (STequal(val, "upper") != 0)
	{
	    STcopy("UPPER", duc_dbenv->du_real_user_case);
	    duc_dbenv->du_got_real_user_case = 'p';
	}
	if (STequal(val, "mixed") != 0)
	{
	    STcopy("MIXED", duc_dbenv->du_real_user_case);
	    duc_dbenv->du_got_real_user_case = 'p';
	}
    }
    else
    {
	/* if no PM default for userid case, default is same as regid case */
	STcopy(duc_dbenv->du_reg_id_case, duc_dbenv->du_real_user_case);
    }
    PMfree();
    return OK;
}
/*{
** Name: validate_id_case_params: check reg ids, delim ids, real user values
**
** Description:
**	Make sure combinations of case translation for regular identifers,
**	delimited identifiers, real usernames are legal.  Legal combinations:
**
**	  real_user_case  reg_id_case delim_id_case  note
**	  -----------------------------------------------------
**	  lower	          lower	      lower	  "Ingres"
**	  lower	          lower	      mixed
**	  mixed	          lower	      mixed
**
**	  upper	          upper	      upper	  
**	  upper	          upper	      mixed    FIPS - default
**	  mixed	          upper	      mixed    FIPS - if want
**                                                    case-sensitive users
**
** Inputs:
**		duc_dbenv	Ptr to a DU_ENV struct.
**					Saves PM params until we
**					write them to iidbcapabilities.
**		duc_errcb	Ptr to a DU_ERROR struct.
**
** Outputs:
**		duc_errcb	If an error is detected, this struct will
**					be set by a call to du_error().
**
** History:
**	10-Jan-93 (jpk)
**	    initial creation
**	09-nov-93 (rblumer)
**	    added new error message for inconsistent reg_id and delim_id
**	    that doesn't mention real_user_case at all; plus don't mention 
**	    real_user_case PM setting if error was for reg_id and delim_id.
**	    Also changed DU0162 message parameters to use actual flag name.
*/
STATUS
validate_id_case_params(duc_dbenv, duc_errcb)
DU_ENV		*duc_dbenv;
DU_ERROR	*duc_errcb;
{
	STATUS	returnval = OK;
	int	last_err  = 0;			/* save last error found     */
	int	pm_msg_printed  = FALSE;	/* was a message printed out?*/
	/* are reg id and delim id settings consistent? */
	if ( (STequal(duc_dbenv->du_reg_id_case, "LOWER") &&
		 STequal(duc_dbenv->du_delim_id_case, "UPPER"))
	||	(STequal(duc_dbenv->du_reg_id_case, "UPPER") &&
		 STequal(duc_dbenv->du_delim_id_case, "LOWER")) )
	{
		du_error(duc_errcb, I_DU0164_BAD_CASE_FLAGS, 4,
			0, duc_dbenv->du_reg_id_case,
			0, duc_dbenv->du_delim_id_case);
		last_err = I_DU0164_BAD_CASE_FLAGS;
		returnval = FAIL;
	}
	/*
	** real user defaults to reg id if not specified.  This has
	** already been taken care of in get_pm_params(), but we
	** extend that rule here: even if there's a PM value for real
	** user, if command line specifies a value for reg id but not
	** one for real user, real user is set to be the same as
	** the reg id given on command line.  In other words, unless
	** the user specifies a value for real user on the command
	** line, real user is set to value of reg id.
	*/
	if (duc_dbenv->du_got_reg_id_case == 'c' &&
		duc_dbenv->du_got_real_user_case != 'c')
	{
		STcopy(duc_dbenv->du_reg_id_case, duc_dbenv->du_real_user_case);
	}
	/* real_user must be same as either reg_id or delim_id.
	** 
	** But only check for this error in haven't found a reg_id/delim_id
	** conflict, since (in most cases) fixing that will get rid of any
	** real_user conflicts.
	*/
	if ((returnval != FAIL)
	    && !STequal(duc_dbenv->du_real_user_case, duc_dbenv->du_reg_id_case)
	    && !STequal(duc_dbenv->du_real_user_case, duc_dbenv->du_delim_id_case))
	{
		du_error(duc_errcb, I_DU0160_BAD_USERID_CASE, 6,
				0, duc_dbenv->du_real_user_case,
				0, duc_dbenv->du_reg_id_case,
				0, duc_dbenv->du_delim_id_case);
		last_err = I_DU0160_BAD_USERID_CASE;
		SIprintf("\n");
		du_error(duc_errcb, I_DU0161_USERID_CASE, 0);
		returnval = FAIL;
	}
	/* then, if one or more params were taken from PM, further enlighten */
	if ( returnval == FAIL &&
			(duc_dbenv->du_got_real_user_case == 'p' ||
			duc_dbenv->du_got_reg_id_case == 'p' ||
			duc_dbenv->du_got_delim_id_case == 'p') )
	{
		if (duc_dbenv->du_got_reg_id_case == 'p')
		{
			du_error(duc_errcb, I_DU0162_INSTALLATION_VALUE, 4,
				0, duc_dbenv->du_reg_id_case, 0, ERx("reg_id_case"));
			pm_msg_printed  = TRUE;
		}
		if (duc_dbenv->du_got_delim_id_case == 'p')
		{
			du_error(duc_errcb, I_DU0162_INSTALLATION_VALUE, 4,
				0, duc_dbenv->du_delim_id_case, 0, ERx("delim_id_case"));
			pm_msg_printed  = TRUE;
		}
		/* don't mention real_user_case if there 
		** was a reg_id/delim_id conflict
		*/
		if ((duc_dbenv->du_got_real_user_case == 'p')
		    && (last_err != I_DU0164_BAD_CASE_FLAGS))
		{
			du_error(duc_errcb, I_DU0162_INSTALLATION_VALUE, 4,
				0, duc_dbenv->du_real_user_case, 0, ERx("real_user_case"));
			pm_msg_printed  = TRUE;
		}
		/* explain that the user can override the installation default */
		if (pm_msg_printed)
		    du_error(duc_errcb, I_DU0163_CMD_LINE_OVERRIDE, 0);
	}
	return returnval;
}
/*
** Name: duc_grant_select_wgo_on_cats - ensure that PUBLIC possesses SELECT WGO
** 					on all catalogs
**
** Description:
**	In the past, one was able to define a view over a catalog and 
**	GRANT SELECT on that view.  In 6.5, in order to grant SELECT on a view,
**	one must possess SELECT WGO on all tables used in the view definition.  
**	In order to enable users to continue granting SELECT on their views
**	defined over catalogs, we will ensure that PUBLIC possesses SELECT WGO
**	on all catalogs
**
** Output:
**	err_cb		filled in if an error occurs
**
** History:
**	11-oct-93 (andre)
**	    written
*/
DU_STATUS
duc_grant_select_wgo_on_cats(err_cb)
DU_ERROR            *err_cb;
{
  char grant_buf[1024];
/* # line 6281 "ducreate.sc" */	/* declare */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"declare global temporary table session.cat_grant(stmt)as select 'gran\
t select on ' +table_name +' to public with grant option' from iitable\
s where system_use='S' and table_type='T' or(table_type='V' and table_\
name in(select table_name from iiviews where ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"view_dml='S'))on commit preserve rows with norecovery");
    IIsyncup((char *)0,0);
  }
/* # line 6295 "ducreate.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"cat_grant",7640,24036);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select stmt from session.cat_grant");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"cat_grant",7640,24036);
  }
/* # line 6296 "ducreate.sc" */	/* host code */
    for (;;)
    {
/* # line 6298 "ducreate.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"cat_grant",7640,24036,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,1023,grant_buf);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 6299 "ducreate.sc" */	/* host code */
	if (sqlca.sqlcode == 100)
	{
	    break;
	}
	else if (sqlca.sqlcode != 0)
	{
	    if (du_error(err_cb, E_DU2014_GRANTING_SEL_ON_CATS, 0) != E_DU_OK)
		duc_exit(err_cb);
	    break;
	}
/* # line 6309 "ducreate.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(grant_buf);
    IIsyncup((char *)0,0);
  }
/* # line 6310 "ducreate.sc" */	/* host code */
    }
/* # line 6311 "ducreate.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"cat_grant",7640,24036);
  }
/* # line 6312 "ducreate.sc" */	/* host code */
    return(E_DU_OK);
}
/*
** Name:    duc_create_IMA_tables - create system catalogs for the IMA tables
**
** Description:
**  This routine creates the tables needed to query the IMA.
**
** Inputs:
**  none.
** Outputs:
**  none.
**
** Returns:
**  E_DB_OK
**
** History:
**  31-jan-95 (reijo01)
**     initial creation to solve bug 66605.
*/
DU_STATUS
duc_create_IMA_tables()
{
  char errbuf[64];
/* # line 6339 "ducreate.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iigw07_relation(tblid integer not null not default, tbli\
dx integer not null not default, flags integer not null not default)");
    IIsyncup((char *)0,0);
  }
/* # line 6345 "ducreate.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iigw07_attribute(tblid integer not null not default, tbl\
idx integer not null not default, attnum i2 not null not default, clas\
sid char(64)not null not default)");
    IIsyncup((char *)0,0);
  }
/* # line 6352 "ducreate.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iigw07_index(tblid integer not null not default, tblidx \
integer not null not default, attnum i2 not null not default, classid \
char(64)not null not default)");
    IIsyncup((char *)0,0);
  }
/* # line 6359 "ducreate.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iigw07_relation to btree unique on tblid, tblidx");
    IIsyncup((char *)0,0);
  }
/* # line 6361 "ducreate.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iigw07_attribute to btree unique on tblid, tblidx, attnum");
    IIsyncup((char *)0,0);
  }
/* # line 6363 "ducreate.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iigw07_index to btree unique on tblid, tblidx, attnum");
    IIsyncup((char *)0,0);
  }
/* # line 6364 "ducreate.sc" */	/* host code */
	if (sqlca.sqlcode != 0) {
/* # line 6365 "ducreate.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,32,63,errbuf,63);
  }
/* # line 6366 "ducreate.sc" */	/* host code */
		SIfprintf(stdout,"%s\n",errbuf);
	}
	 return(E_DU_OK);
}
/*
** Name:    duc_val_loc - check location names 
**
** Description:
**  This routine :
**		checks that location is not default, then
**		calls duc_val_loc1() for the low-level stuff.
**
** Inputs:
**  none.
** Outputs:
**  Modified values for location fields in duc_dbenv
**	from work done in duc_val_loc1
**
** Returns:
**  E_DB_OK
**	FAIL
** 
** History:
** 	22-apr-1997 (wonst02)
** 	    Added duc_rdonly_flag to support readonly databases.
**                     
*/
static STATUS
duc_val_loc(DU_ENV   *duc_dbenv, DUC_MODE_VECT *duc_modes)
{
	char	tmpstr[DB_MAXNAME+1];
	STATUS	stat;
	i4		mode;
/* validate locations :
	 - delim or regular id */
	if (duc_modes->duc_ckploc_flag == TRUE)
	{
		STcopy(duc_dbenv->du_ckploc.du_locname, tmpstr);
		stat = duc_val_loc1(tmpstr, duc_dbenv->du_ckploc.du_locname);
		if (stat != E_DU_OK)
			return(stat);
	}
	if (duc_modes->duc_jnlloc_flag == TRUE)
	{
		STcopy(duc_dbenv->du_jnlloc.du_locname, tmpstr);
		stat = duc_val_loc1(tmpstr, duc_dbenv->du_jnlloc.du_locname); 
		if (stat != E_DU_OK)
			return(stat);
	}
	if (duc_modes->duc_dbloc_flag || duc_modes->duc_rdonly_flag)
	{
		STcopy(duc_dbenv->du_dbloc.du_locname , tmpstr);
		stat = duc_val_loc1(tmpstr,duc_dbenv->du_dbloc.du_locname); 
		if (stat != E_DU_OK)
			return(stat);
	}
	if (duc_modes->duc_sortloc_flag == TRUE)
	{
		STcopy(duc_dbenv->du_sortloc.du_locname, tmpstr);
		stat = duc_val_loc1(tmpstr,duc_dbenv->du_sortloc.du_locname); 
		if (stat != E_DU_OK)
			return(stat);
	}
	if (duc_modes->duc_dmploc_flag == TRUE)
	{
		STcopy(duc_dbenv->du_dmploc.du_locname, tmpstr);
		stat = duc_val_loc1(tmpstr,duc_dbenv->du_dmploc.du_locname);
		if (stat != E_DU_OK)
			return(stat);
	}
return(E_DU_OK);
}
/*
** Name:    duc_val_loc1 - check location names 
**
** Description:
**  This routine :
**		does low-level work for duc_val_loc()
**		- validates location names as valid delimited ids
**		- forces lower-/upper-case depending on installation (default/SQL-92)
**
** Inputs:
**		tmpstr	-temp copy of location name
**		locname	- pointer to char[] in structure pointed
**				  to by duc_dbenv.
**  none.
** Outputs:
**  locname - location name, suitably normalized if a delim id,
**		      and in the right case
**
**	E_IC002E_bad_name if bad delimited identifier
** Returns:
**  E_DU_OK
**	FAIL if bad delimited identifier
*/
static STATUS
duc_val_loc1(PTR tmpstr, PTR locname )
{
	DU_ERROR	duc_errcb;
	i4			status=0;
	u_i4	idmode=0;
	i4			len;
	len = STlength(tmpstr);
	idmode |= CUI_ID_REG_L;
	/* check for regular id, force case if so */
	status = cui_f_idxlate(tmpstr, len , idmode);
	if (status == FALSE)	/* therefore delimited */
	{
		DB_ERROR	err_blk;
		idmode=0;
		idmode |= (CUI_ID_QUO | CUI_ID_QUO_L);
		status = cui_idxlate((u_char *)tmpstr, (u_i4 *)&len, 
							 (u_char *)locname ,(u_i4 *) &len, 
							 idmode,
							(u_i4 *)0, &err_blk);
		if (status == E_DB_ERROR)
		{
			du_error(&duc_errcb, err_blk.err_code, 0,0,NULL);
			du_error(&duc_errcb, E_DU3045_BAD_LOCNAME, 2, 0, &locname);
			return(status);
		}
	}
return(E_DU_OK);
}
