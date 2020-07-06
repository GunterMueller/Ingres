# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**Copyright (c) 2004 Ingres Corporation
*/
/* duvetest.sc - test verifydb's dbms_catalogs operation
**
**  NO_OPTIM=ris_us5
**
**  Description:
**	This is an esql driver to test verifydb's dbms_catalogs operation.
**
**	HOW TO WRITE TESTS:
**	===================
**
**	Global variables are:
**		dbname - name of database to run tests on.  This is visible to
**			 esql as well as to the test driver.
**		dba    - DBA of the database to run tests on.  This is visible
**			 to ESQL as well as to the test driver.
**		user   - a valid ingres user name that you can use for any test
**			 that requires a valid user name (like someone to grant
**			 permits to, etc).  This is visible to ESQL as well as
**			 to the test driver.
**		test_id - contains name of catalog currently being tested.
**		test_status - this is the status variable.  If any test fails
**			 so that subsequent testing should be aborted, the
**			 test_status variable is set to TEST_FAIL.  Otherwise
**			 it should be set to TEST_OK.  All utility routines will
**			 check test_status before performing any operations and
**			 will silently exit if test_status is set to TEST_ABORT.
**		logfile - name of log file to output this test to
**
**	The basic structure is:
**	MAIN calls each of the following routines for each catagory of catalogs:
**	    1. core_cats() - tests verifydb checks/repairs for core catalogs
**	    2. qrymod_cats() - tests verifydb checks/repairs for qrymod catalogs
**	    3. qp_cats() - tests verifydb checks/repairs for query processing
**		           catalogs
**	    4. opt_cats() - tests verifydb checks/repairs for optimizer catalogs
**	    5. dbdb_cats() - tests verifydb checks/repairs for iidbdb only
**			     catalogs
**	    6. debug_me() - special area to dump any debug specific code.
**
**	Each of the above routines calls a routine named ck_<catalog_name> for
**	each catalog it must check.  For example, the routine to check the
**	iiattribute catalog is named ck_iiattribute, the routine to check the
**	iirelation catalog is named ck_iirelation, etc.  When ever you add a new
**	dbms catalog, you must write a new driver (ck_<catalog_name>) for that
**	catalog.  Each driver has one or more tests in it, which correspond
**	to the verifydb checks/repairs specificied for that catalog.  These
**	checks/repairs are specified in the "VERIFYDB DBMS_CATALOG Operation
**	Specification."
**
**	Each routine should start by checking that test_status is not set to
**	TEST_ABORT before doing anything (and if set to TEST_ABORT, it should
**	just return).  Also, each utility routine should set global variable
**	test_id to the name of the catalog it is testing.  For example,
**	ck_iiattribute should STcopy("iiattribute",test_id).  If you develop
**	a new driver, be sure to add a call to it in the approparite routine
**	[core_cats() if it is a core catalog, qrymod_cats() if it is a qrymod
**	catalog, qp_cats() if it is a query processing catalog, opt_cats() if
**	it is an optimizer catalog or dbdb_cats if it is an iidbdb-only catalog]
**	Each ck_<catalog_name> routine is comprised of 1 or more tests on
**	verifydb checks/repairs.
**
**	Each test is comprised of three sections:
**		1.  setup
**		2.  running verifydb (once in report mode and once in
**		    test_runi mode)
**		3.  cleanup -- the test driver needs to undo whatever setup
**		    it did in case verifydb fails, so that the subsequent tests
**		    will not have their results messed up by leftovers from a
**		    failed test.
**	There are utility routines that perform most functions you'll need:
**		open_db -- connects to a DB as DBA with "+Y" so you can update
**			   system catalogs.
**		open_A9db -- connects to a DB as DBA with +Y and -A9 so that you
**			     can update systemt catalogs and mess with indices
**		open_db_as_user -- open database as a specified user
**		close_db -- disconnects from an open database
**		report_vdb -- runs verifydb on the database in report mode
**		run_test -- determines whether or not to run specific test,
**			    based on value of argv[5] (which is only meaningful
**			    if argv[4] is not "1" for "all tests").
**			    **If you add new catalog tests, you'll need to
**			      update this routine to handle them and also
**			      update the catalog name constants.
**		run_vdb	-- runs verifydb on the database in test_runi mode.
**			 
**	This is a template for a typical test:
**	    
	    ************
	    ** test # **
	    ************
	    ** setup **
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    exec sql <<statements to set up for test go here >>
	    close_db();
	    ** run test **
	    report_vdb();
	    run_vdb();
	    ** clean up **
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    exec sql delete <<statements to clean up from test go here>>
	    close_db();
**
**
**  Inputs:
**	argv[0] = program pathname (default of operating system)
**	argv[1] = database name
**	argv[2] = dba name
**	argv[3] = user name
**	argv[4] = test inst:
**		  NULL - defaults to all verifydb tests
**	          '1'   - all verifydb tests
**		  '2'  - core catalog tests only
**		  '3'  - qrymod catalog tests only
**		  '4'  - qp catalog tests only
**		  '5'  - optimizer catalog tests only
**		  '6'  - dbdb catalog tests only
**		 '10'  - debug
**		 '20'  - debug
**	    argv[5] = subtest inst:
**		  varies with each test instr, but indicates to only run one
**		  specific subtest under each test type:
**			CORE:   101 to 131 - run iirelation test 1 to 31
**				201 - run iirel_idx test 1
**				301 to 303 - run iiattribute test 1 to 3 
**				401 to 404 - run iiindex test 1 to 3
**				501 to 502 - run iidevices test 1 to 2
**			QRYMOD: 101 to 110 - run iiintegrity tests 1 to 10 
**			        201 to 212 - run iiprotect tests 1 to 12
**			        301 to 306 - run iiqrytext tests 1 to 6
**			        401 to 403 - run iisynonym tests 1 to 3 
**			        501 to 506 - run iitree tests 1 to 6
**				601 to 605 - run iidbms_comment tests 1 to 5
**				701	   - run iikey test 1
**				750 to 754 - run iisecalarm tests 1 to 4
**				801        - run iidefault test 1
**			QP:	101 to 104 - run iidbdepends tests 1 to 4
**			QP:	100 to 104 - run iidbdepends tests 1 to 4
**					    currently iidbdepepends test 5 is
**					    not runnable.
**			        201 to 203 - run iievent tests 1 to 3 
**			        301 to 308 - run iiprocedure tests 1 to 8
**			        401 to 406 - run iirule tests 1 to 6 
**			        501 - run iixdbdepends test 1
**					     currently above test isn't runable
**				601 to 603 - run iigw06_attribute tests 1 to 3
**				701 to 703 - run iigw06_relation test 1 to 3
**				801 to 802 - run iipriv tests 1 to 2
**				901 to 902 - run iixpriv tests 1 to 2
**			      1001 to 1002 - run iixevent tests 1 and 2
**			      1101 to 1102 - run iixprocedure tests 1 and 2
**			OPT:    101 to 105 - run iistatistics tests 1 to 5
**			        201 to 202 - run iihistogram tests 1 to 2
**			IIDBDB: 101 to 102 - run iicdbid_idx tests 1 to 2
**					     currently above test isn't runable
**			        201 to 219 - run iidatabase tests 1 to 15
**			        301 to 302 - run iidbid_idx tests 1 to 2
**					     currently above test isn't runable
**			        401 to 403 - run iidbpriv tests 1 to 3
**			        501 to 502 - run iiextend tests 1 to 2
**			        601 to 602 - run iilocations tests 1 to 2
**			        701 to 705 - run iistar_cdbs tests 1 to 5
**			        801 to 804 - run iiuser tests 1 to 4
**			        901 - run iiusergroup test 1
**	                        950 to 903 - run iiprofile tests 1 to 3
**	    argv[6] = "SEP" if you want to run in sep mode.
**  Outputs:
**  -  messages to STDOUT via SIprintf;
**  -  several files in II_CONFIG containing the results of each test.  All
**     files start with "ii_config:iivdb" and have the extension of the DBA
**     name.  The 6th letter of each file is a test code as follows:
**		a = iidatabase catalog
**		c = core catalogs
**		d = dbdb catalogs
**		m = query mod catalogs
**		o = optimizer catalogs
**		p = query processing catalogs
**		r = iirelation catalog
**
PROGRAM =	testvdb
NEEDLIBS =      DBUTILLIB SQLCALIB LIBQLIB LIBQGCALIB GCFLIB \
                UGLIB FMTLIB AFELIB ADFLIB ULFLIB CUFLIB COMPATLIB MALLOCLIB
**
**  History :
**	04-jun-93 (teresa)
**	    initial creation
**	25-jun-93 (tomm)
**	    Added NEEDLIBS for ming.
**    	28-jun-93 (teresa)
**	    cleaned up the tests to assure they all correctly set up the
**	    test conditions.  Also added a feature to permit specificiation of
**	    and individual test via argv[5].
**	13-jul-93 (teresa)
**	    added support for sep flag.
**	2-aug-93 (stephenb)
**	    Added routines ck_iigw06_relation() and ck_iigw06_attribute().
**	    and updated other modules to fit them in, also added test #29
**	    to ck_iirelation()
**	18-sep-93 (teresa)
**	    Added routine ck_iidbms_comment and added new tests to
**	    ck_iidatabase.
**	22-dec-93 (robf)
**          Added routines ck_iisecalarm(), ck_iiprofile.
**	10-jan-94 (andre)
**	    remove redeclarations of DB_MAXNAME, DU_NAME_UPPER, DU_DELIM_UPPER,
**	    DU_DELIM_MIXED, and DU_REAL_USER_MIXED
**      11-jan-94 (anitap)
**          Corrected typos in open_db(), open_dbdb(), open_db_as_user().
**	    Added tests for FIPS' constraints and CREATE SCHEMA projects.
**	27-jan-94 (andre)
**	    redefined catalog constants to facilitate derivation of the first 
**	    valid test number for a given catalog.  Since redefining test 
**	    numbers would be too much work, I took a different approach:
**		catalogs in each group will be within the same 10000 group
**		(e.g. all core catalogs will be between 0 and 9999); to derive 
**		the first valid test number, one will subtract catalog number 
**		base (in case of core catalog - 0) from teh catalog constant 
**		and add 1
**
**	    defined IIXPROTECT, IIPRIV, IIXPRIV, IIXEVENT, IIXPROCEDURE
**	02-feb-94 (huffman) && (mhuishi)
**	    A conditional was_too_complex for the AXP C compiler to optimize.
**	    This is the programming 101 fix around it.
**	20-feb-96 (mosjo01/moojo03)
**	    NO_OPTIM=ris_us5 needed to resolve the following error:
**	    c89: 1501-229 Compilation ended due to lack of space.
**	    Error ocurred on AIX 3.2.5 using compiler xlc 1.3
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	30-Dec-2004 (schka24)
**	    remove iixprotect tests.
**	19-jan-2005 (abbjo03)
**	    Remove declaration of dmf_svcb.
**      07-Jun-2005 (inifa01) b111211 INGSRV2580
**          Verifydb reports error; S_DU1619_NO_VIEW iirelation indicates that
**          there is a view defined for table a (owner ingres), but none exists,
**          when view(s) on a table are dropped.
**          Were turning on iirelation.relstat TCB_VBASE bit.
**          TCB_VBASE not tested anywhere in 2.6 and not turned off when view
**          dropped qeu_cview() call to dmt_alter() to set bit has been removed,
**          removeing references to TCB_VBASE here also.
*/
    /**********************************/
    /* Global variables and constants */
    /**********************************/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
#include        <compat.h>
#include        <gl.h>
#include        <sl.h>
#include        <iicommon.h>
#include        <dbdbms.h>
#include	<er.h>
#include        <me.h>
#include        <si.h>
#include        <st.h>
#include        <pc.h>
#include	<dudbms.h>
#include	<duerr.h>
#include	<cs.h>
#include	<lg.h>
#include	<lk.h>
#include	<dm.h>
#include	<dmf.h>
#include	<dmp.h>
    /* constants to process input parameters */
#define		INVALID_OPCODE	    0
#define		MAX_DIGITS	    4
#define		BIT1		    0x0001
#define		BIT2		    0x0002
#define		BIT3		    0x0004
#define		BIT4		    0x0008
#define		BIT5		    0x0010
#define		BIT6		    0x0020
#define		BIT10		    0x0040
#define		BIT20		    0x0080
#define         IIDBDB_DB	    "iidbdb"
    /* file codes for name_log */
#define		EXT_POS		    "."
#define		BASE_LOG	    "iivdb"
#define		DTBASE_CODE	    "a"
#define		CORE_CODE	    "c"
#define		DBDB_CODE	    "d"
#define		QP_CODE		    "p"
#define		QRYMOD_CODE	    "m"
#define		STAT_CODE	    "s"
#define		REL_CODE	    "r"
#define		ID_00		    "00"
#define		ID_01		    "01"
#define		ID_02		    "02"
#define		ID_03		    "03"
#define		ID_04		    "04"
#define		ID_05		    "05"
#define		ID_06		    "06"
#define		ID_07		    "07"
#define		ID_08		    "08"
#define		ID_09		    "09"
#define		ID_0A		    "0A"
#define		ID_0B		    "0B"
#define		ID_0C		    "0C"
#define		ID_0D		    "0D"
#define		ID_0E		    "0E"
#define		ID_0F		    "0F"
#define		ID_10		    "10"
#define		ID_11		    "11"
#define		ID_12		    "12"
#define		ID_13		    "13"
#define		ID_14		    "14"
#define		ID_15		    "15"
#define		ID_16		    "16"
#define		ID_17		    "17"
#define		ID_18		    "18"
#define		ID_19		    "19"
#define		ID_1A		    "1A"
#define		ID_1B		    "1B"
#define		ID_1C		    "1C"
#define		ID_1D		    "1D"
#define		ID_1E		    "1E"
#define		ID_1F		    "1F"
#define         ID_6A               "6A"
#define         ID_9A               "9A"
#define         ID_9B               "9B"
#define         ID_28A              "28A"
/* codes for run_test() */
    /* dummy */
#define		II		    0
#define		IIDB_ACCESS	    0
    /* core catalogs */
#define		CORE_CATALOGS	    0
#define		IIRELATION	    100
#define		IIREL_IDX	    200
#define		IIATTRIBUTE	    300
#define		IIINDEX		    400
#define		IIDEVICES	    500
    /* qrymod catalogs */
#define		QRYMOD_CATALOGS	    10000
#define		IIINTEGRITY	    10100
#define		IIPROTECT	    10200
#define		IIQRYTEXT	    10300
#define		IISYNONYM	    10400
#define		IITREE		    10500
#define		IIDBMS_COMMENT	    10600
#define         IIKEY               10700
#define		IISECALARM 	    10750
#define         IIDEFAULT           10800
    /* query processing catalogs */
#define		QP_CATALOGS	    20000
#define		IIDBDEPENDS	    20100
#define		IIEVENT	    	    20200
#define		IIPROCEDURE	    20300
#define		IIRULE		    20400
#define		IIXDBDEPENDS	    20500
#define		IIGW06_ATTRIBUTE    20600
#define		IIGW06_RELATION     20700
#define		IIPRIV		    20800
#define		IIXPRIV		    20900
#define		IIXEVENT	    21000
#define		IIXPROCEDURE	    21100
    /* optimizer catalogs */
#define		OPTIMIZER_CATALOGS  30000
#define		IISTATISTICS	    30100
#define		IIHISTOGRAM	    30200
    /* iidbdb only catalogs */
#define		IIDBDB_CATALOGS	    40000
#define		IICDBID_IDX	    40100
#define		IIDATABASE	    40200
#define		IIDBID_IDX	    40300
#define		IIDBPRIV	    40400
#define		IIEXTEND	    40500
#define		IILOCATIONS	    40600
#define		IISTAR_CDBS	    40700
#define		IIUSER		    40800
#define		IIUSERGROUP	    40900
#define		IIPROFILE	    40950
/* misc relstat bits -- matches values in dmp.h */
#define			PRTUPS		    0x00000004L
#define			INTEGS		    0x00000008L
#define                 SCONCUR		    0x00000010L
#define                 VIEW		    0x00000020L
#define                 VBASE		    0x00000040L
#define			INDEX		    0x00000080L
#define                 IDXD		    0x00020000L
#define                 ZOPTSTAT	    0x00100000L
#define			MULTIPLE_LOC	    0x00400000L
#define			RULE		    0x01000000L
#define			COMMENT_BIT	    0x08000000L
/* relstat2 bits */
#define			ALARM_BIT	    0x01000000L
/* misc dbservice bits -- matches values in dudbms.qsh */
    /* global variables */
  char dbname[DB_MAXNAME+1];
  char dba[DB_MAXNAME+1];
  char test_database[DB_MAXNAME+1];
  char user[DB_MAXNAME+1];
    char	    logfile[DB_MAXNAME+1];
    char	    test_id[DB_MAXNAME+1];
    i4		    test_status;
#define			TEST_OK		    0
#define			TEST_FAIL	    1
#define			TEST_ABORT	    3
    i4		    subtest=0;
    int		    testmap;
    i4		    sep = FALSE;
    DB_STATUS	    suppress;
#define	DO_NOT_SUPPESS	0	/*  suppress no msgs */
#define SUPPRESS_ALL	-1	/*  suppress all msgs */
/*
** main -- main driver for verifydb dbms_catalogs operation tests
**
** Description:
**	Parses the input parameters, initializes global variables and
**	calls the appropriate processing routines based on input parameter
**	"test instruction", as follows:
**	    value:	    routine called:
**	    NULL or 1	    core_cats(), qrymod_cats(), qp_cats(), opt_cats(),
**			    and dbdb_cats()
**	    2		    core_cats()
**	    3		    qrymod_cats()
**	    4		    qp_cats()
**	    5		    opt_cats()
**	    6		    dbdb_cats()
**	    10		    debug_me()
**
** Inputs:
**	argc	    number of input parameters
**	argv[],	    Input parameters
**	    argv[0] = program pathname (default of operating system)
**	    argv[1] = database name
**	    argv[2] = dba name
**	    argv[3] = user name
**	    argv[4] = test inst:
**		  NULL - defaults to all verifydb tests
**	          '1'   - all verifydb tests
**		  '2'  - core catalog tests only
**		  '3'  - qrymod catalog tests only
**		  '4'  - qp catalog tests only
**		  '5'  - optimizer catalog tests only
**		  '6'  - dbdb catalog tests only
**		 '10'  - debug
**	    argv[5] = subtest inst:
**		  varies with each test instr, but indicates to only run one
**		  specific subtest under each test type:
**			CORE:   101 to 129 - run iirelation test 1 to 29
**				201 - run iirel_idx test 1
**				301 to 303 - run iiattribute test 1 to 3 
**				401 to 404 - run iiindex test 1 to 4 
**				501 to 502 - run iidevices test 1 to 2
**			QRYMOD: 101 to 110 - run iiintegrity tests 1 to 10 
**			        201 to 204 - run iiprotect tests 1 to 4
**			        301 to 306 - run iiqrytext tests 1 to 6
**			        401 to 403 - run iisynonym tests 1 to 3 
**			        501 to 506 - run iitree tests 1 to 6
**				601 to 605 - run iidbms_comment tests 1 to 5
**				701	   - run iikey test 1
**				751 to 754 - run iisecalarm test 1 to 4
**				801 	   - run iidefault test 1	
**			QP:	101 to 104 - run iidbdepends tests 1 to 4
**					    currently iidbdepepends test 5 is
**					    not runnable.
**			        201 to 203 - run iievent tests 1 to 3 
**			        301 to 304 - run iiprocedure tests 1 to 4 
**			        401 to 406 - run iirule tests 1 to 6 
**			        501 - run iixdbdepends test 1
**					     currently above test isn't runable
**			OPT:    101 to 105 - run iistatistics tests 1 to 5
**			        201 to 202 - run iihistogram tests 1 to 2
**			IIDBDB: 101 to 102 - run iicdbid_idx tests 1 to 2
**					     currently above test isn't runable
**			        201 to 215 - run iidatabase tests 1 to 15
**			        301 to 302 - run iidbid_idx tests 1 to 2
**					     currently above test isn't runable
**			        401 to 40~ - run iidbpriv tests 1 to ~
**			        501 to 50~ - run iiextend tests 1 to ~
**			        601 to 60~ - run iilocations tests 1 to ~
**			        701 to 70~ - run iistar_cdbs tests 1 to ~
**			        801 to 80~ - run iiuser tests 1 to ~
**			        901 to 90~ - run iiusergroup tests 1 to ~
**	                        950 to 95~- run iiprofile tests 1 to ~
**	    argv[6] = "SEP" if you want to run in sep mode.
**			    
**
** Outputs;
**	None.
** History:
**	08-jun-93 (teresa)
**	    initial creation.
**	22-jun-93 (teresa)
**	    added subtest parameter to enable user to indicate that a specific
**	    test should be run.
**	13-jul-93 (teresa)
**	    added support for sep flag.
**	15-jul-93 (teresa)
**	    fixed unix compiler warning.
**	10-nov-93 (anitap)
**	    Corrected inttabbase to protabbase in tests 3 & 4 in ck_iiprotect().
**	    Corrected ii dbdepends to iidbdepends and indi1 to inid1 and indid2
**	    to inid2 in test 5 in ck_iitree().
**	26-jan-94 (andre)
**	    added new tests for IIRELATION, IIPROCEDURE, and IIPROTECT;
**	    added new drivers for IIPRIV, IIXPRIV, IIXPROCEDURE, IIXEVENT, and
**	    IIXPROTECT
*/
main(argc, argv)
int    argc;
char   *argv[];
{
	int	test_instr = 0;
	int	i;
	int	ret_stat;
/* # line 539 "duvetest.sc" */	/* host code */
    MEadvise(ME_INGRES_ALLOC);
    test_status = TEST_OK;
    do		    /* control loop */
    {
	if (argc < 2)
	{
	    printargs();
	    SIprintf ("retry with database name\n");
	    break;
	}
	if (argc < 3)
	{
	    printargs();
	    SIprintf ("retry with DBA name\n");
	    break;
	}
	if (argc < 4)
	{
	    printargs();
	    SIprintf ("retry with user name\n");
	    break;
	}
	/* copy database name */
	for (i=0; i <= DB_MAXNAME; i++)
	{
	    if (argv[1][i])
		test_database[i] = argv[1][i];
	    else
	    {
		test_database[i] = '\0';
		break;
	    }
	}
	/* copy DBA name */
	for (i=0; i <= DB_MAXNAME; i++)
	{
	    if (argv[2][i])
		dba[i] = argv[2][i];
	    else
	    {
		dba[i] = '\0';
		break;
	    }
	}
	/* copy user name */
	for (i=0; i <= DB_MAXNAME; i++)
	{
	    if (argv[3][i])
		user[i] = argv[3][i];
	    else
	    {
		user[i] = '\0';
		break;
	    }
	}
	/* trim names to 32 characters to protect against idocy! */
	test_database[DB_MAXNAME] = '\0';
	STcopy(test_database,dbname); /*assume that user input db will be used--
				      ** this will not be true if user specifies
				      ** iidbdb only tests */
	dba[DB_MAXNAME]= '\0';
	user[DB_MAXNAME] = '\0';
	if (argc >= 6)
	    xlate_string(argv[5], &subtest);
	if (argc >=7 )
	   if (  ( (argv[6][0]=='s') || (argv[6][0]=='S') ) &&
		 ( (argv[6][1]=='e') || (argv[6][1]=='E') ) &&
		 ( (argv[6][2]=='p') || (argv[6][2]=='P') ) )
			sep = TRUE;
	/* now see what test case we have been asked to do */
	testmap = INVALID_OPCODE;
	if (argc == 4)
	{
	    testmap = BIT2 | BIT3 | BIT4 | BIT5 | BIT6;
	    subtest = 0;
	}
	else
	{
	    /* translate op_code to decimal and assure it is valid */
	    xlate_string(argv[4], &test_instr);
	    switch (test_instr)
	    {
		case 1:			    /* all verifydb tests */
		    testmap |= BIT2 | BIT3 | BIT4 | BIT5 | BIT6;
		    subtest = 0;
		    break;
		case 2:			    /* core catalog tests only */
		    testmap |= BIT2;
		    break;
		case 3:			    /* qrymod catalog tests only */
		    testmap |= BIT3;
		    break;
		case 4:			    /* qp catalog tests only */
		    testmap |= BIT4;
		    break;
		case 5:			    /* optimizer catalog tests only */
		    testmap |= BIT5;
		    break;
		case 6:			    /* iidbdb catalog tests only */
		    testmap |= BIT6;
		    break;
		case 10:		    /* debug */
		    testmap |= BIT10;
		    break;
		case 20:		    /* debug */
		    testmap |= BIT20;
		    break;
		default:
		    /* this op code is currently not implemented */
		    break;
	    }
	}
	if ( testmap == INVALID_OPCODE )
	{
	    printargs();
	    SIprintf ("invalid test instr input parameter.\n");
	    SIprintf ("please retry with a valid test instruction code.\n");
	    break;
	}
	if ( (testmap & BIT2) && (test_status != TEST_ABORT) )
	    core_cats();
	if ( (testmap & BIT3) && (test_status != TEST_ABORT) )
	    qrymod_cats();
	if ( (testmap & BIT4) && (test_status != TEST_ABORT) )
	    qp_cats();
	if ( (testmap & BIT5) && (test_status != TEST_ABORT) )
	    opt_cats();
	if ( (testmap & BIT6) && (test_status != TEST_ABORT) )
	{
	    /* force test to run on iidbdb database */
	    STcopy(IIDBDB_DB,dbname);		
	    /* run tests on iidbdb catalogs */
	    dbdb_cats();
	    /* restore name of test db incase there is any futher testing */
	    STcopy(test_database,dbname); 
	}
	if ( (testmap & BIT10) && (test_status != TEST_ABORT) )
	    debug_me();
	if ( (testmap & BIT20) && (test_status != TEST_ABORT) )
	    debug_me2();
    } while (0);	/* end of control loop */
    if (test_status == TEST_ABORT)
    {
	SIprintf ("Verifydb Tests aborted on catalog %s\n", test_id);
	ret_stat = FAIL;
    }
    else
    {
	SIprintf ("Verifydb Tests completed.\n");
	ret_stat = OK;
    }
    PCexit(ret_stat);
}
/*
** close_db -- close an open database
**
** Description:
**	disconnects from a database.
** Inputs:
**	None.
** Outputs;
**	None.
** History:
**	08-jun-93 (teresa)
**	    initial creation.
*/
close_db()
{
    if (test_status == TEST_ABORT)
	return;    
    else
    {
#ifdef DEBUG
	SIprintf ("disconnecting from database\n");
#endif
/* # line 730 "duvetest.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 731 "duvetest.sc" */	/* host code */
    }
}
/*
** ck_iiattribute - 
**
** Description:
**
**  This routine tests the verifydb iiattribute catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	08-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
**      23-nov-93 (anitap)
**          added test 3 to check that the default ids in iiattribute match
**          that in iidefault catalog.
*/
ck_iiattribute()
{
  i4 id1;
  i4 id2;
  i4 id3;
  i4 id4;
    SIprintf ("..Testing verifydb checks/repairs for catalog iiattribute..\n");
    SIflush(stdout);
    STcopy("iiattribute",test_id);
    do
    {
	if ( run_test(IIATTRIBUTE, 1))
	{
	    SIprintf ("....iiattribute test 1..\n");
	    SIflush(stdout);
	    /***********/
	    /* test 1  */
	    /***********/
	    /* setup */
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 782 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_att1(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 783 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_att1'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 785 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiattribute select 7, 7, attid, attxtra, attname, attoff,\
 attfrmt, attfrml, attfrmp, attkdom, attflag, attdefid1, attdefid2 fro\
m iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attname='b'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 790 "duvetest.sc" */	/* host code */
	    close_db();
	    /* run test */
	    report_vdb();
	    run_vdb();
	    /* clean up */
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 800 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 803 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 804 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelidx=7");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 805 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIATTRIBUTE, 2))
	{
	    SIprintf ("....iiattribute test 2..\n");
	    SIflush(stdout);
	    /***********/
	    /* test 2  */
	    /***********/
	    /* setup */
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 821 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_att2(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 822 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_att2'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 824 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiattribute set attfrmt=12 where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attname='b'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 826 "duvetest.sc" */	/* host code */
	    close_db();
	    /* run test */
	    report_vdb();
	    run_vdb();
	    /* clean up */
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 836 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 839 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 840 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIATTRIBUTE, 3))
        {
            SIprintf ("....iiattribute test 3..\n");
            SIflush(stdout);
            /***********/
            /* test 3  */
            /***********/
            /* setup */
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 857 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_att3(i i)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 858 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_att3'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 860 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select attdefid1, attdefid2 from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attname='i'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id3);
      IIgetdomio((short *)0,1,30,4,&id4);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 863 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiattribute set attdefid1=17 where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attname='i' and attdefid\
1=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attdefid2=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 867 "duvetest.sc" */	/* host code */
            close_db();
            /* run test */
            report_vdb();
            run_vdb();
            /* clean up */
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 877 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 880 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 881 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attdefid1=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attdefid2=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attname='i'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 884 "duvetest.sc" */	/* host code */
            close_db();
        }
    } while (0);	/* control loop */
}
/*
** ck_iicdbid_idx - 
**
** Description:
**
**  This routine tests the verifydb iicdbid_idxcatalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	16-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
*/
ck_iicdbid_idx()
{
  i4 id1;
  i4 id2;
  i4 val;
    SIprintf ("..Testing verifydb checks/repairs for catalog iicdbid_idx..\n");
    SIflush(stdout);
    STcopy("iicdbid_idx",test_id);
    do
    {
	if ( run_test(IICDBID_IDX, 1))
	{
	    SIprintf ("....iicdbid_idx test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 934 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select max(cdb_id)from iicdbid_idx");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 935 "duvetest.sc" */	/* host code */
	    id1 += 2;
/* # line 936 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"insert into iicdbid_idx valu\
es(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 937 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 947 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iicdbid_idx where cdb_id=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 948 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IICDBID_IDX, 2))
	{
	    SIprintf ("....iicdbid_idx test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 965 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select max(db_id)from iidata\
base");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 966 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select max(cdb_id)from iicdbid_idx");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 967 "duvetest.sc" */	/* host code */
	    id1 += 3;	    
/* # line 968 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iistar_cdbs values('fake_ddb', 'fake_dba', 'fake_cdb', 'f\
ake_dba', 'fake_node', 'ingres', 'UNKNOWN', ' ', 'Y', ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", '')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 971 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select tidp from iicdbid_idx where cdb_id=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 972 "duvetest.sc" */	/* host code */
	    val ++;
/* # line 973 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iicdbid_idx set tidp=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where cdb_id=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 974 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
	    val --;
/* # line 985 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iicdbid_idx set tidp=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where cdb_id=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 986 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iistar_cdbs where cdb_id=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 987 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iicdbid_idx where cdb_id=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 988 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iidatabase - 
**
** Description:
**
**  This routine tests the verifydb iidatabase catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	16-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
*/
ck_iidatabase()
{
  i4 id1;
  i4 id2;
  i4 val;
    SIprintf ("..Testing verifydb checks/repairs for catalog iidatabase..\n");
    SIflush(stdout);
    STcopy("iidatabase",test_id);
    do
    {
	if ( run_test(IIDATABASE, 1))
	{
	    name_log(DTBASE_CODE,ID_01);
	    SIprintf ("....iidatabase test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1039 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase(own, db_id)values('$ingres', 100000)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1041 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1052 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where db_id=100000 and own='$ingres'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1053 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDATABASE, 2))
	{
	    name_log(DTBASE_CODE,ID_02);
	    SIprintf ("....iidatabase test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1070 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_dbdb2', '$ingres', 'bad_default_d\
ev', 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, 2, 'ii\
_dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1073 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1083 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_dbdb2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1084 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDATABASE, 3))
	{
	    name_log(DTBASE_CODE,ID_03);
	    SIprintf ("....iidatabase test 3..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 3  **/
	    /*************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1101 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_dbdb3', 'fake_dba', 'ii_database'\
, 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, 3, 'ii_du\
mp')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1104 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_dbdb3', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1105 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1115 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_dbdb3'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1116 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_dbdb3'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1117 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDATABASE, 4))
	{
	    name_log(DTBASE_CODE,ID_04);
	    SIprintf ("....iidatabase test 4..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 4  **/
	    /*************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1134 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iilocations values(8, 'fake_dbloc2', 'fake_area')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1136 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_dbdb4', '$ingres', 'fake_dbloc2',\
 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, 4, 'ii_dum\
p')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1139 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1149 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_dbdb4'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1150 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iilocations where lname='fake_dbloc2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1151 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where lname='fake_dbloc2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1152 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDATABASE, 5))
	{
	    name_log(DTBASE_CODE,ID_05);
	    SIprintf ("....iidatabase test 5..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 5  **/
	    /*************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1169 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_dbdb5', '$ingres', 'ii_database',\
 'fake_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, 5, 'ii_d\
ump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1172 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_dbdb5', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1173 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1183 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_dbdb5'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1184 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_dbdb5'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1185 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDATABASE, 6))
	{
	    name_log(DTBASE_CODE,ID_06);
	    SIprintf ("....iidatabase test 6..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 6  **/
	    /*************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1202 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_dbdb6', '$ingres', 'ii_database',\
 'ii_checkpoint', 'fake_journal', 'ii_work', 17, 0, '6.4', 0, 6, 'ii_d\
ump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1205 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_dbdb6', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1206 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1216 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_dbdb6'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1217 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_dbdb6'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1218 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDATABASE, 7))
	{
	    name_log(DTBASE_CODE,ID_07);
	    SIprintf ("....iidatabase test 7..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 7  **/
	    /*************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1235 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_dbdb7', '$ingres', 'ii_database',\
 'ii_checkpoint', 'ii_journal', 'fake_sort', 17, 0, '6.4', 0, 7, 'ii_d\
ump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1238 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_dbdb7', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1239 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
     	    if (test_status == TEST_ABORT)
		break;
/* # line 1249 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_dbdb7'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1250 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_dbdb7'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1251 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDATABASE, 8))
	{
	    name_log(DTBASE_CODE,ID_08);
	    /*************/
	    /** test 8  **/
	    /*************/
	    /* this test is obsoleted */
	}
	if ( run_test(IIDATABASE, 9))
	{
	    name_log(DTBASE_CODE,ID_09);
	    SIprintf ("....iidatabase test 9..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 9  **/
	    /*************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1277 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_dbdb9', '$ingres', 'ii_database',\
 'ii_checkpoint', 'ii_journal', 'ii_work', 4, 0, '6.4', 0, 9, 'ii_dump\
')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1280 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_dbdb9', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1281 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1291 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_dbdb9'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1292 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_dbdb9'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1293 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDATABASE, 10))
	{
	    name_log(DTBASE_CODE,ID_0A);
	    SIprintf ("....iidatabase test 10..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 10 **/
	    /*************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1310 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_dbdb10', '$ingres', 'ii_database'\
, 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '7.6', 0, 987, 'ii_\
dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1313 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_dbdb10', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1314 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1324 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_dbdb10'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1325 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_dbdb10'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1326 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if (run_test(IIDATABASE, 11))
	{
	    name_log(DTBASE_CODE,ID_0B);
	    SIprintf ("....iidatabase test 11..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 11 **/
	    /*************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1343 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_dbdb11', '$ingres', 'ii_database'\
, 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 3, 11, 'ii_d\
ump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1346 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_dbdb11', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1347 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1357 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_dbdb11'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1358 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_dbdb11'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1359 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDATABASE, 12) )
	{
	    name_log(DTBASE_CODE,ID_0C);
	    SIprintf ("....iidatabase test 12..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 12 **/
	    /*************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1376 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_dbdb12', '$ingres', 'ii_database'\
, 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, 0, 'ii_du\
mp')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1379 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_dbdb12', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1380 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1390 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_dbdb12'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1391 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_dbdb12'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1392 "duvetest.sc" */	/* host code */
	    close_db();
	}
#if 0
-- this is currently untestable, since INGRES does not permit inserts of
-- tuples with duplicate keys into iidatabase
	if ( run_test(IIDATABASE, 13))
	{
	    name_log(DTBASE_CODE,ID_0D);
	    SIprintf ("....iidatabase test 13..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 13 **/
	    /*************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1412 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_dbdb13a', '$ingres', 'ii_database\
', 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, 13, 'ii_\
dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1415 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_dbdb13b', '$ingres', 'ii_database\
', 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, 13, 'ii_\
dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1418 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_dbdb13a', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1420 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_dbdb13b', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1422 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1432 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_dbdb13a'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1433 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_dbdb13b'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1434 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_dbdb13a'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1435 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_dbdb13b'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1436 "duvetest.sc" */	/* host code */
	    close_db();
	}
#endif
	if ( run_test(IIDATABASE, 14) )
	{
	    name_log(DTBASE_CODE,ID_0E);
	    /*************/
	    /** test 14 **/
	    /*************/
	    /* currently this is not testable because the server will not permit
	    ** us to directly minipulate secondary indexes */
	}
	if ( run_test(IIDATABASE, 15) )
	{
	    name_log(DTBASE_CODE,ID_0F);
	    SIprintf ("....iidatabase test 15..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 15 **/
	    /*************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1466 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_dbdb15', '$ingres', 'ii_database'\
, 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, 15, 'fake\
_dmp')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1469 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_dbdb15', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1470 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1480 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_dbdb15'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1481 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_dbdb15'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1482 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDATABASE, 16) )
	{
	    name_log(DTBASE_CODE,ID_10);
	    SIprintf ("....iidatabase test 16..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 16 **/
	    /*************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
	    val = DU_NAME_UPPER;
/* # line 1500 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_dbdb16', '$ingres', 'ii_database'\
, 'ii_checkpoint', 'ii_journal', 'ii_work', 17, ");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)", '7.0', 0, 16, 'ii_dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1503 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_dbdb16', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1504 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1514 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_dbdb16'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1515 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_dbdb16'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1516 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDATABASE, 17) )
	{
	    name_log(DTBASE_CODE,ID_11);
	    SIprintf ("....iidatabase test 17..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 17 **/
	    /*************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
	    val = DU_DELIM_UPPER;
/* # line 1534 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_dbdb17', '$ingres', 'ii_database'\
, 'ii_checkpoint', 'ii_journal', 'ii_work', 17, ");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)", '7.0', 0, 17, 'ii_dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1537 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_dbdb17', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1538 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1548 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_dbdb17'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1549 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_dbdb17'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1550 "duvetest.sc" */	/* host code */
	    close_db();
	}	
	if ( run_test(IIDATABASE, 18) )
	{
	    name_log(DTBASE_CODE,ID_12);
	    SIprintf ("....iidatabase test 18..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 18 **/
	    /*************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
	    val = DU_NAME_UPPER | DU_DELIM_UPPER | DU_DELIM_MIXED;
/* # line 1568 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_dbdb18', '$ingres', 'ii_database'\
, 'ii_checkpoint', 'ii_journal', 'ii_work', 17, ");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)", '7.0', 0, 18, 'ii_dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1571 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_dbdb18', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1572 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1582 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_dbdb18'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1583 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_dbdb18'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1584 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDATABASE, 19) )
	{
	    name_log(DTBASE_CODE,ID_13);
	    SIprintf ("....iidatabase test 19..\n");
	    SIflush(stdout);
	    /***************/
	    /** test 19   **/
	    /***************/
	    /** setup **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
	    val = DU_REAL_USER_MIXED;
/* # line 1602 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_dbdb19', '$ingres', 'ii_database'\
, 'ii_checkpoint', 'ii_journal', 'ii_work', 17, ");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)", '7.0', 0, 19, 'ii_dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1605 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_dbdb19', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1606 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_dbdb();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1616 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_dbdb19'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1617 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_dbdb19'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1618 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iidbaccess - 
**
** Description:
**
**  This routine tests the verifydb iidbaccesscatalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	16-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
*/
ck_iidbaccess()
{
  i4 id1;
  i4 id2;
    SIprintf ("..Testing verifydb checks/repairs for catalog iidbaccess..\n");
    SIflush(stdout);
    STcopy("iidbaccess",test_id);
    do
    {
/* not used for 6.5, so not coded.  Should be coded if verifydb is retrofit
** to 6.4
*/
	if ( run_test(IIDB_ACCESS, 1) )
	{
	    SIprintf ("....iidb_access test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test #  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    /* exec sql <<statements to set up for test go here >> */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    /* exec sql delete <<statements to clean up from test go here>> */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iidbdepends - 
**
** Description:
**
**  This routine tests the verifydb iidbdepends catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	16-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
**      24-nov-93 (anitap)
**          Added tests 1A & 1B to test if the dependent tuple exists for
**          procedures and rules supporting constraints.
**	    Also added tests 1C and 1D to test if the dependent tuple 
**	    exists for indexes and constraints.
**	    Corrected typo in test 3. Changed indid1 to inid1.
**	    Added i_qid, qid to insert statement in test 1 for views.
**	    Also changed the dtype for views to 17 from 0.
**	11-jan-94 (andre)
**	    corrected setup for tests 1 and 2 so that they test what they claim 
**	    to test. 
*/
ck_iidbdepends()
{
  i4 id1, id2, id3, id4;
  i4 val;
  i4 fakeid;
  i4 txt1, txt2, txt3, txt4;
  i4 dep_type, indep_type;
    SIprintf ("..Testing verifydb checks/repairs for catalog iidbdepends..\n");
    SIflush(stdout);
    STcopy("iidbdepends",test_id);
    do
    {
	if ( run_test(IIDBDEPENDS, 1) )
	{
	    SIprintf ("....iidbdepends test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    dep_type = DB_VIEW;
	    indep_type = DB_TABLE;
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1748 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_dbdep1(a i\
4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1749 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid from iirelation where relid='duve_dbdep1'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1751 "duvetest.sc" */	/* host code */
	    fakeid = id1+1;
/* # line 1752 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbdepends(inid1, inid2, itype, i_qid, qid, deid1, deid2\
, dtype)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
    IIputdomio((short *)0,1,30,4,&indep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, 0, ");
    IIputdomio((short *)0,1,30,4,&fakeid);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1755 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1765 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_dbdep1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1766 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where deid1=");
    IIputdomio((short *)0,1,30,4,&fakeid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1767 "duvetest.sc" */	/* host code */
	    close_db();
	    /* This test is actually part of test 1, but checks for
            ** dependent objects which are procedures for constraints.
            */
            SIprintf ("....iidbdepends test 1A..\n");
            SIflush(stdout);
            /*************/
            /** test 1A **/
            /*************/
	    indep_type = DB_CONS;
	    dep_type = DB_DBP;
            /** setup **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 1786 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_dbdep1a(a i4 not null unique)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1787 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_dbdep1ar(b i4 references duve_dbdep1a(a))");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1789 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_dbdep1ar'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1792 "duvetest.sc" */	/* host code */
            fakeid = id1+1;
/* # line 1793 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbdepends(inid1, inid2, itype, i_qid, qid, deid1, deid2\
, dtype)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&indep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1, 0, ");
    IIputdomio((short *)0,1,30,4,&fakeid);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1797 "duvetest.sc" */	/* host code */
            close_db();
            /** run test **/
            report_vdb();
            run_vdb();
            /** clean up **/
            open_db();
            if (test_status == TEST_ABORT)
		break;
/* # line 1807 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_dbdep1ar");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1808 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_dbdep1a");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1810 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1811 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where deid1=");
    IIputdomio((short *)0,1,30,4,&fakeid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1812 "duvetest.sc" */	/* host code */
            close_db();
            /* This test is actually part of test 1, but checks for
            ** dependent objects which are rules for constraints.
            */
            SIprintf ("....iidbdepends test 1B..\n");
            SIflush(stdout);
            /*************/
            /** test 1B **/
            /*************/
	    indep_type = DB_CONS;
	    dep_type = DB_RULE;
            /** setup **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 1832 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_dbdep1b(a i4 check(a is not null))");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1833 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_dbdep1b'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1836 "duvetest.sc" */	/* host code */
            fakeid = id1+1;
/* # line 1837 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbdepends(inid1, inid2, itype, i_qid, qid, deid1, deid2\
, dtype)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&indep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1, 0, ");
    IIputdomio((short *)0,1,30,4,&fakeid);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1840 "duvetest.sc" */	/* host code */
            close_db();
            /** run test **/
            report_vdb();
            run_vdb();
            /** clean up **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 1850 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_dbdep1b");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1852 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1853 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where deid1=");
    IIputdomio((short *)0,1,30,4,&fakeid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1854 "duvetest.sc" */	/* host code */
            close_db();
            /* This test is actually part of test 1, but checks for
            ** dependent objects which is index for UNIQUE
            ** constraint.
            */
            SIprintf ("....iidbdepends test 1C..\n");
            SIflush(stdout);
            /*************/
            /** test 1C **/
            /*************/
	    indep_type = DB_CONS;
	    dep_type = DB_INDEX;
            /** setup **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 1875 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_dbdep1c(a i4 not null unique)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1876 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_dbdep1c'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1879 "duvetest.sc" */	/* host code */
            fakeid = id1+1;
/* # line 1880 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select i_qid from iidbdepends where inid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dtype=");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id3);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1882 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbdepends(inid1, inid2, itype, i_qid, qid, deid1, deid2\
, dtype)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&indep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&fakeid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1886 "duvetest.sc" */	/* host code */
            close_db();
            /** run test **/
            report_vdb();
            run_vdb();
            /** clean up **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 1896 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_dbdep1c");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1898 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1899 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where deid1=");
    IIputdomio((short *)0,1,30,4,&fakeid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1900 "duvetest.sc" */	/* host code */
            close_db();
            /* This test is actually part of test 1, but checks for
            ** dependent objects which is constraint for REFERENTIAL 
            ** constraint dependent on an UNIQUE constraint.
            */
            SIprintf ("....iidbdepends test 1D..\n");
            SIflush(stdout);
            /*************/
            /** test 1D **/
            /*************/
	    indep_type = DB_CONS;
	    dep_type = DB_CONS;
            /** setup **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 1921 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_dbdep1d(a i4 not null unique)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1922 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_dbdep1dr(b i4 references duve_dbdep1d(a))");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1924 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_dbdep1d'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1927 "duvetest.sc" */	/* host code */
            fakeid = id1+1;
/* # line 1928 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select i_qid from iidbdepends where inid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dtype=");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id3);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1930 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbdepends(inid1, inid2, itype, i_qid, qid, deid1, deid2\
, dtype)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&indep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
    IIputdomio((short *)0,1,30,4,&fakeid);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1933 "duvetest.sc" */	/* host code */
            close_db();
            /** run test **/
            report_vdb();
            run_vdb();
            /** clean up **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 1943 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_dbdep1d");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1944 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_dbdep1dr");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1946 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1947 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where deid1=");
    IIputdomio((short *)0,1,30,4,&fakeid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1948 "duvetest.sc" */	/* host code */
            close_db();
        }
	if ( run_test(IIDBDEPENDS, 2) )
	{
	    SIprintf ("....iidbdepends test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    dep_type = DB_VIEW;
	    indep_type = DB_TABLE;
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1967 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_dbdep2(a i\
4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1968 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view v_duve_dbdep2 as select * from duve_dbdep2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1969 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, relqid1, relqid2 from iirelation where relid='v_duve_d\
bdep2'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&txt1);
      IIgetdomio((short *)0,1,30,4,&txt2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1972 "duvetest.sc" */	/* host code */
	    fakeid = id1 + 1;
/* # line 1973 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbdepends(inid1, inid2, itype, i_qid, qid, deid1, deid2\
, dtype)values(");
    IIputdomio((short *)0,1,30,4,&fakeid);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
    IIputdomio((short *)0,1,30,4,&indep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, 0, ");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1976 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 1986 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop v_duve_dbdep2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1987 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where inid1=");
    IIputdomio((short *)0,1,30,4,&fakeid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1989 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&txt1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&txt2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1990 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_dbdep2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 1991 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDBDEPENDS, 3) )
	{
	    SIprintf ("....iidbdepends test 3..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 3  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2007 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_dbdep3(a i\
4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2008 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view v_duve_dbdep3 as select * from duve_dbdep3");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2009 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2019 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop view v_duve_dbdep3");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2020 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_dbdep3");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2021 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDBDEPENDS, 4) )
	{
	    SIprintf ("....iidbdepends test 4..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 4  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2037 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_dbdep4(a i\
4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2038 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view v_duve_dbdep4 as select * from duve_dbdep4");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2039 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'duve_dbdep4'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&txt1);
      IIgetdomio((short *)0,1,30,4,&txt2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2042 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'v_duve_dbdep4'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id3);
      IIgetdomio((short *)0,1,30,4,&id4);
      IIgetdomio((short *)0,1,30,4,&txt3);
      IIgetdomio((short *)0,1,30,4,&txt4);
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2045 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2055 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop view v_duve_dbdep4");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2056 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_dbdep4");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2058 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2060 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2062 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&txt1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&txt2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2064 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&txt3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&txt4);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2066 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2068 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2070 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2072 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2074 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where inid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and deid1=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and deid2=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2075 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDBDEPENDS, 5) )
	{
	    SIprintf ("....iidbdepends test 5..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 5  **/
	    /*************/
	    /* this is currently not testable, since there is no way to
	    ** corrupt the secondary index via esql.
	    */
	}
    } while (0);  /* control loop */
}
/*
** ck_iixpriv - exercise IIXPRIV tests
**
** Description:
**
**  This routine tests the verifydb iixpriv catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	27-jan-94 (andre)
**	    written
*/
ck_iixpriv()
{
  i4 id1;
  i4 val;
  i4 priv;
  u_i4 tid;
    SIprintf ("..Testing verifydb checks/repairs for catalog iixpriv..\n");
    SIflush(stdout);
    STcopy("iixpriv",test_id);
    do
    {
	if ( run_test(IIXPRIV, 1) )
	{
	    SIprintf ("....iixpriv test 1..\n");
	    SIflush(stdout);
#if	0
	    /* 
	    ** skip this test - cleanup gets messy - dropping the index may 
	    ** result in a deadlock (it is used in qeu_d_cascade()) and cursor 
	    ** delete fails due to insufficient privileges - both can be 
	    ** addressed in BE, but not now
	    */
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_A9db();
	    if (test_status == TEST_ABORT)
		break;
	    /* 
	    ** insert into IIXPRIV a tuple for which there should be no 
	    ** match in IIPRIV
	    */
/* # line 2149 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iixpriv(i_obj_id, i_obj_idx, i_priv, i_priv_grantee, tidp\
)values(0, 0, 0, 'andre', 0)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2152 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_A9db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2163 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iixpriv where i_obj_id=0 and i_obj_idx=0 and i_priv=0");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2164 "duvetest.sc" */	/* host code */
	    close_db();
#else
	    SIprintf ("this test is currently skipped.\n");
	    SIflush(stdout);
#endif
	}
	if ( run_test(IIXPRIV, 2) )
	{
	    SIprintf ("....iixpriv test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    /*
	    ** create a view on iirelation and a dummy table to avoid iirelation
	    ** test 13, then delete the newly added IIXPRIV tuple
	    */
/* # line 2189 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_iixpriv2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2190 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_iixpriv2_t");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2191 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_iixpriv2_t(i i)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2192 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view duve_iixpriv2 as select * from iirelation, duve_iixpriv2_\
t");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2194 "duvetest.sc" */	/* host code */
	    close_db();
	    open_A9db();
/* # line 2197 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select p.tid, i_obj_id, i_priv from iipriv p, iirelation r where relo\
wner=user and relid='duve_iixpriv2' and d_obj_id=reltid and d_obj_idx=\
reltidx");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tid);
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&priv);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2204 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from iixpriv where ti\
dp=");
    IIputdomio((short *)0,1,30,4,&tid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2205 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_A9db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2215 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iixpriv where i_obj_id=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and i_obj_idx=0");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2221 "duvetest.sc" */	/* host code */
	    if (val == 0)
	    {
/* # line 2223 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iixpriv(i_obj_id, i_obj_idx, i_priv, i_priv_grantee, tidp\
)select ");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
    IIputdomio((short *)0,1,30,4,&priv);
    IIwritio(0,(short *)0,1,32,0,(char *)
", user, tid from iipriv where i_obj_id=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2228 "duvetest.sc" */	/* host code */
	    }
/* # line 2230 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_iixpriv2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2231 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_iixpriv2_t");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2233 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iipriv - test verifydb IIPRIV checks/repairs
**
** Description:
**
**  This routine tests the verifydb iipriv catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	27-jan-94 (andre)
**	    written
*/
ck_iipriv()
{
  i4 id1, id2, id3, id4;
  i4 val;
  i4 fakeid;
  i4 txt1, txt2, txt3, txt4;
  i4 dep_type, indep_type;
  i4 priv;
    SIprintf ("..Testing verifydb checks/repairs for catalog iipriv..\n");
    SIflush(stdout);
    STcopy("iipriv",test_id);
    do
    {
	if ( run_test(IIPRIV, 1) )
	{
	    SIprintf ("....iipriv test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    /*
	    ** insert a tuple representing a privilege descriptor for 
	    ** non-existant object and let verifydb find it
	    */
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    dep_type = DB_VIEW;
	    priv = DB_RETRIEVE | DB_TEST | DB_AGGREGATE;
/* # line 2294 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iipriv(d_obj_id, d_obj_idx, d_priv_number, d_obj_type, i_\
obj_id, i_obj_idx, i_priv, i_priv_grantee, prv_flags, i_priv_map1, i_p\
riv_map2, i_priv_map3, i_priv_map4, i_priv_map5, i_priv_map6, i_priv_m\
ap7, i_priv_map8, i_priv_map9, i_priv_mapa)");
    IIwritio(0,(short *)0,1,32,0,(char *)"values(0, 0, 0, ");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1, 1, ");
    IIputdomio((short *)0,1,30,4,&priv);
    IIwritio(0,(short *)0,1,32,0,(char *)
", 'andre', 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2306 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2316 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iipriv where d_obj_id=0 and d_obj_idx=0");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2317 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIPRIV, 2) )
	{
	    SIprintf ("....iipriv test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    /*
	    ** create a dbproc selecting from IIRELATION, update its privilege 
	    ** descriptor to make it look as if it depends on UPDATE privilege,
	    ** then let verifydb at it
	    */
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2340 "duvetest.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"duve_iipriv2");
    IIwritio(0,(short *)0,1,32,0,(char *)"drop procedure duve_iipriv2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2341 "duvetest.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"duve_iipriv2");
    IIwritio(0,(short *)0,1,32,0,(char *)"create procedure duve_iipriv\
2 as");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                declare cnt \
i;");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                begin");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    select count(*) into :cnt from iirelation;");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                end");
    IIputctrl(0);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2347 "duvetest.sc" */	/* host code */
	    dep_type = DB_DBP;
	    priv = DB_REPLACE;
/* # line 2349 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iipriv from iiprocedure set i_priv=");
    IIputdomio((short *)0,1,30,4,&priv);
    IIwritio(0,(short *)0,1,32,0,(char *)
"where dbp_name='duve_iipriv2' and dbp_owner=user and d_obj_id=dbp_id \
and d_obj_idx=dbp_idx and d_obj_type=");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2358 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2368 "duvetest.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"duve_iipriv2");
    IIwritio(0,(short *)0,1,32,0,(char *)"drop procedure duve_iipriv2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2369 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iidbid_idx - 
**
** Description:
**
**  This routine tests the verifydb iidbid_idx catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	16-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
*/
ck_iidbid_idx()
{
  i4 id1;
  i4 id2;
  i4 val;
    SIprintf ("..Testing verifydb checks/repairs for catalog iidbid_idx..\n");
    SIflush(stdout);
    STcopy("iidbid_idx",test_id);
    do
    {
	if ( run_test(IIDBID_IDX, 1) )
	{
	    SIprintf ("....iidbid_idx test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2419 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select max(db_id)from iidata\
base");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2420 "duvetest.sc" */	/* host code */
	    val++;
/* # line 2421 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"insert into iidbid_idx value\
s(");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2422 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2432 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbid_idx where db_id=");
    IIputdomio((short *)0,1,30,4,&val);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2433 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDBID_IDX, 2) )
	{
	    SIprintf ("....iidbid_idx test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2449 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_dbdb2', '$ingres', 'ii_default_de\
v', 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, 22, 'ii\
_dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2452 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select db_id, tidp from iidbid_idx");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2453 "duvetest.sc" */	/* host code */
	    id2 = val +1;
/* # line 2454 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iidbid_idx set tipd=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"where db_id=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2455 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2465 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where db_id=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2466 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where db_id=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2467 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iidbms_comment() - 
**
** Description:
**
**  This routine tests the verifydb iidbms_comment catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	18-sep-93 (teresa)
**	    Initial Creation.
*/
ck_iidbms_comment()
{
  i4 id1;
  i4 id2;
  i4 val;
    SIprintf ("..Testing verifydb checks/repairs for catalog iidbms_comment..\n");
    SIflush(stdout);
    STcopy("iidbms_comment",test_id);
    do
    {
	if ( run_test(IIDBMS_COMMENT,1) )
	{
	    SIprintf ("....iidbms_comment test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2515 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_cmnt1(a i4\
)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2516 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_cmnt1'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2518 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_cmnt1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2519 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbms_comment values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1, 0, 'hi', 0, 'long remar\
k')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2521 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2531 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbms_comment where comtabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2532 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDBMS_COMMENT,2) )
	{
	    SIprintf ("....iidbms_comment test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    	    if (test_status == TEST_ABORT)
		break;
/* # line 2548 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_cmnt2(a i4\
)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2549 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_cmnt2'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2551 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbms_comment values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1, 2, 'hi', 0, 'long remar\
k')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2553 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2563 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbms_comment where comtabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2564 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_cmnt2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2565 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDBMS_COMMENT,3) )
	{
	    SIprintf ("....iidbms_comment test 3..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 3  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2581 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_cmnt3(a i4\
)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2582 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relstat from iirelation where relid='duve_cmn\
t3'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2585 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbms_comment values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1, 0, 'hi', 0, 'long remar\
k')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2587 "duvetest.sc" */	/* host code */
	    val |= TCB_COMMENT;
/* # line 2588 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where reltid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2590 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2600 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbms_comment where comtabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2601 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_cmnt3");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2602 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDBMS_COMMENT,4) )
	{
	    SIprintf ("....iidbms_comment test 4..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 4  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2618 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_cmnt4(a i4\
)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2619 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relstat from iirelation where relid='duve_cmn\
t4'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2622 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbms_comment values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", 2, 2, 'hi', 0, 'long remar\
k')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2624 "duvetest.sc" */	/* host code */
	    val |= TCB_COMMENT;
/* # line 2625 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where reltid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2627 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2637 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbms_comment where comtabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2638 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_cmnt4");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2639 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDBMS_COMMENT,5) )
	{
	    SIprintf ("....iidbms_comment test 5..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 5  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2655 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_cmnt5(a i4\
)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2656 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relstat from iirelation where relid='duve_cmn\
t5'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2659 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbms_comment values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1, 2, 'hi', 0, 'long remar\
k1')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2661 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbms_comment values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1, 2, 'hi', 1, 'long remar\
k1')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2663 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbms_comment values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1, 2, 'hi', 3, 'long remar\
k1')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2665 "duvetest.sc" */	/* host code */
	    val |= TCB_COMMENT;
/* # line 2666 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where reltid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2668 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2678 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbms_comment where comtabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2679 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_cmnt5");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2680 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iidbpriv - 
**
** Description:
**
**  This routine tests the verifydb iidbpriv catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	16-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
*/
ck_iidbpriv()
{
  i4 id1;
  i4 id2;
    SIprintf ("..Testing verifydb checks/repairs for catalog iidbpriv..\n");
    SIflush(stdout);
    STcopy("iidbpriv",test_id);
    do
    {
	if ( run_test(IIDBPRIV, 1) )
	{
	    SIprintf ("....iidbpriv test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2729 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbpriv(dbname, grantee, control, flags)values('duve_non\
existent_db', '$ingres', 0, 0)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2731 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2741 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbpriv where dbname='duve_nonexistent_db'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2742 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDBPRIV, 2) )
	{
	    SIprintf ("....iidbpriv test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2758 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbpriv(dbname, grantee, control, flags)values('iidbdb',\
 'duve_nonexistent_dba', 0, 0)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2760 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2770 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbpriv where grantee='duve_nonexistent_dba'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2771 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDBPRIV, 3 ))
	{
	    SIprintf ("....iidbpriv test 3..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 3  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2787 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_ficticious_db1', '$ingres', 'ii_d\
atabase', 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, 2\
, 'ii_dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2790 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_ficticious_db1', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2792 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbpriv(dbname, grantee, control, flags)values('duve_fic\
ticious_db1', '$ingres', 15, 31)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2794 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2804 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_ficticious_db1'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2805 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_ficticious_db1'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2806 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbpriv where dbname='duve_ficticious_db1'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2807 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iidefault -
**
** Description:
**
** This routine tests the verifydb iidefault catalog checks/repairs.
**
** Inputs:
**      None.
** Outputs:
**      global variable: test_status    - status of tests: TEST_OK, TEST_ABORT
** History:
**     23-nov-93 (anitap)
**         initial creation for FIPS constraints' project.
*/
ck_iidefault()
{
  i4 id1, id2, id3;
    SIprintf ("..Testing verifydb checks/repairs for catalog iidefault..\n");
    SIflush(stdout);
    STcopy("iidefault",test_id);
    do
    {
        if (run_test(IIDEFAULT, 1) )
        {
           SIprintf ("....iidefault test 1..\n");
           SIflush(stdout);
           /*************/
           /** test 1 **/
           /************/
           /** setup **/
           open_db();
           if (test_status == TEST_ABORT)
              break;
/* # line 2853 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_def1(a char(10)with default 'great')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2854 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid from iirelation where relid='duve_def1'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2856 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select attdefid1, attdefid2 from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attname='a'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&id3);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2859 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attname='a'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2860 "duvetest.sc" */	/* host code */
           close_db();
           /** run test **/
           report_vdb();
           run_vdb();
           /** clean up **/
           open_db();
           if (test_status == TEST_ABORT)
              break;
/* # line 2870 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidefault where defid1=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and defid2=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2872 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_def1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2873 "duvetest.sc" */	/* host code */
           close_db();
        }
    } while (0);        /* control loop */
}
/*
** ck_iidevices - 
**
** Description:
**
**  This routine tests the verifydb iidevices catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	09-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
*/
ck_iidevices()
{
  i4 id1;
  i4 id2;
  i4 stat;
    SIprintf ("..Testing verifydb checks/repairs for catalog iidevices..\n");
    SIflush(stdout);
    STcopy("iidevices",test_id);
    do
    {
	if ( run_test(IIDEVICES, 1) )
	{
	    SIprintf ("....iidevices test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2922 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidevices values(28, 28, 1, 'ii_database')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2923 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2933 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidevices where devrelidx=28 and devrelid=28");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2934 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIDEVICES, 2) )
	{
	    SIprintf ("....iidevices test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 2950 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_dev2(a i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2951 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_dev2'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2953 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"insert into iidevices values\
(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1, 'bogas_location')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2955 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirelation set relloccount=relloccount +1 where relid='duve_de\
v2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2957 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='duve_dev2'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&stat);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2959 "duvetest.sc" */	/* host code */
	    stat |= TCB_MULTIPLE_LOC;
/* # line 2960 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&stat);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_dev2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2962 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    stat &= ~TCB_MULTIPLE_LOC;
/* # line 2973 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&stat);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_dev2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2975 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirelation set relloccount=0 where relid='duve_dev2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2978 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidevices where devrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and devrelidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2979 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_dev2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2980 "duvetest.sc" */	/* host code */
	    close_db();	
/* # line 2980 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_dev2(a i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 2981 "duvetest.sc" */	/* host code */
	}
    } while (0);  /* control loop */
}
/*
** ck_iixevent - exercise IIXEVENT tests
**
** Description:
**
**  This routine tests the verifydb iixevent catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	27-jan-94 (andre)
**	    written
*/
ck_iixevent()
{
  i4 id1;
  i4 val;
  u_i4 tid;
    SIprintf ("..Testing verifydb checks/repairs for catalog iixevent..\n");
    SIflush(stdout);
    STcopy("iixevent",test_id);
    do
    {
	if ( run_test(IIXEVENT, 1) )
	{
	    SIprintf ("....iixevent test 1..\n");
	    SIflush(stdout);
#if	0
	    /* 
	    ** skip this test - cleanup gets messy - dropping the index may 
	    ** result in a deadlock (it is used in qeu_d_cascade()) and cursor 
	    ** delete fails due to insufficient privileges - both can be 
	    ** addressed in BE, but not now
	    */
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_A9db();
	    if (test_status == TEST_ABORT)
		break;
	    /* 
	    ** insert into IIXEVENT a tuple for which there should be no 
	    ** match in IIEVENT
	    */
/* # line 3040 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iixevent(event_idbase, event_idx, tidp)values(0, 0, 0)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3043 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_A9db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3054 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iixevent where event_idbase=0 and event_idx=0");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3055 "duvetest.sc" */	/* host code */
	    close_db();
#else
	    SIprintf ("this test is currently skipped.\n");
	    SIflush(stdout);
#endif
	}
	if ( run_test(IIXEVENT, 2) )
	{
	    SIprintf ("....iixevent test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_A9db();
	    if (test_status == TEST_ABORT)
		break;
	    /*
	    ** create a dbevent, then delete the newly added IIXEVENT tuple
	    */
/* # line 3079 "duvetest.sc" */	/* drop dbevent */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop dbevent duve_iixevent2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3080 "duvetest.sc" */	/* create dbevent */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create dbevent duve_iixevent\
2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3081 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select tid, event_idbase from iievent where event_owner=user and even\
t_name='duve_iixevent2'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tid);
      IIgetdomio((short *)0,1,30,4,&id1);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3086 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from iixevent where t\
idp=");
    IIputdomio((short *)0,1,30,4,&tid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3087 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_A9db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3097 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iixevent where event_idbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and event_idx=0");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3103 "duvetest.sc" */	/* host code */
	    if (val == 0)
	    {
/* # line 3105 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iixevent(event_idbase, event_idx, tidp)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
    IIputdomio((short *)0,1,30,4,&tid);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3108 "duvetest.sc" */	/* host code */
	    }
/* # line 3110 "duvetest.sc" */	/* drop dbevent */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop dbevent duve_iixevent2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3112 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iievent - 
**
** Description:
**
**  This routine tests the verifydb iievent catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	18-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
**      23-nov-93 (anitap)
**          Added test 3 to check that schema is created for orphaned event.
*/
ck_iievent()
{
  i4 id1, id2;
  i4 txt1, txt2;
    SIprintf ("..Testing verifydb checks/repairs for catalog iievent..\n");
    SIflush(stdout);
    STcopy("iievent",test_id);
    do
    {
	if ( run_test(IIEVENT, 1) )
	{
	    SIprintf ("....iievent test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3163 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iievent(event_name, event_owner, event_qryid1, event_qryi\
d2)values('duve_event1', 'dummy', 100, 99)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3166 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3176 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iievent where event_name='duve_event1'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3177 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIEVENT, 2) )
	{
	    SIprintf ("....iievent test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3193 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_event2(a i\
4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3194 "duvetest.sc" */	/* create dbevent */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create dbevent duve_event");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3195 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid from iirelation where relid='duve_event2'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3197 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select event_idbase, event_qryid1, event_qryid2 from iievent where ev\
ent_name='duve_event'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&txt1);
      IIgetdomio((short *)0,1,30,4,&txt2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3200 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iievent set event_idb\
ase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"where event_name='duve_event\
'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3202 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3212 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iievent set event_idb\
ase=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"where event_name='duve_event\
'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3214 "duvetest.sc" */	/* drop dbevent */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop dbevent duve_event");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3215 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_event2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3216 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&txt1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&txt2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3217 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iievent where event_name='duve_event'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3218 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if (run_test (IIEVENT, 3) )
        {
           SIprintf("....iievent test 3..\n");
           SIflush(stdout);
           /*************/
           /** test 3  **/
           /*************/
           /** setup   **/
           open_db();
           if (test_status == TEST_ABORT)
               break;
/* # line 3234 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3235 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session authorization su\
per");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3236 "duvetest.sc" */	/* create dbevent */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create dbevent duve_event");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3237 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3238 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session authorization $ingres");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3239 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iischema where schema_name='super'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3240 "duvetest.sc" */	/* host code */
           close_db();
           /** run test **/
           report_vdb();
           run_vdb();
           /** clean up **/
           open_db();
           if (test_status == TEST_ABORT)
              break;
/* # line 3250 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3251 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session authorization su\
per");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3252 "duvetest.sc" */	/* drop dbevent */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop dbevent duve_event");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3253 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3254 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session authorization $ingres");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3255 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iischema where schema_name='super'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3256 "duvetest.sc" */	/* host code */
           close_db();
        }
    } while (0);  /* control loop */
}
/*
** ck_iiextend - 
**
** Description:
**
**  This routine tests the verifydb iiextend catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	16-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
*/
ck_iiextend()
{
  i4 id1;
  i4 id2;
    SIprintf ("..Testing verifydb checks/repairs for catalog iiextend..\n");
    SIflush(stdout);
    STcopy("iiextend",test_id);
    do
    {
	if ( run_test(IIEXTEND, 1) )
	{
	    SIprintf ("....iiextend test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3306 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_ficticious_db2', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3308 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3318 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_ficticious_db2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3319 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIEXTEND, 2) )
	{
	    SIprintf ("....iiextend test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3335 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_ficticious_db3', '$ingres', 'ii_d\
atabase', 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, 3\
3, 'ii_dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3338 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('duve_ficticious_loc_2', 'duve_ficticious\
_db3', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3340 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3350 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_ficticious_db3'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3351 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_ficticious_db3'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3352 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iigw06_attribute - 
**
** Description:
**
** This routine tests the verifydb iigw06_attribute catalog checks/repairs
**
** Inputs:
**	none.
** Outputs:
**	global variable test_status - status of tests: TEST_OK, TEST_ABORT
** History:
**	2-aug-93 (stephenb)
**	    Initial creation.
*/
ck_iigw06_attribute()
{
  i4 reltid;
    SIprintf("..Testing verifydb checks/repairs for catalog iigw06_attribute..\n");
    SIflush(stdout);
    STcopy("iigw06_attribute",test_id);
    do
    {
	if (run_test(IIGW06_ATTRIBUTE, 1) )
	{
	    SIprintf ("....iigw06_attribute test 1..\n");
	    SIflush(stdout);
	    /************/
	    /** test 1 **/
	    /************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    /*
	    ** Put a good row in using register, then corrupt it.
	    */
/* # line 3400 "duvetest.sc" */	/* register table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register table verifydb_test(database char(24)not null)as import from\
 'current' with DBMS=SXA");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3402 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid from iirelation where relid='verifydb_test'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&reltid);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3404 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iigw06_relation where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3405 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3406 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3407 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3417 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iigw06_attribute where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3418 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if (run_test(IIGW06_ATTRIBUTE, 2) )
	{
	    SIprintf("....iigw06_attribute test 2..\n");
	    SIflush(stdout);
	    /************/
	    /** test 2 **/
	    /************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3434 "duvetest.sc" */	/* register table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register table verifydb_test(database char(24)not null)as import from\
 'current' with DBMS=SXA");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3436 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid from iirelation where relid='verifydb_test'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&reltid);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3438 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3439 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3440 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3450 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iigw06_attribute where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3451 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iigw06_relation where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3452 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIGW06_ATTRIBUTE, 3) )
	{
	    SIprintf ("....iigw06_attribute test 3..\n");
	    SIflush(stdout);
	    /************/
	    /** test 3 **/
	    /************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3468 "duvetest.sc" */	/* register table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register table verifydb_test(database char(24)not null)as import from\
 'current' with DBMS=SXA");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3470 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid from iirelation where relid='verifydb_test'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&reltid);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3472 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iigw06_attribute set reltidx=1 where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3474 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3484 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iigw06_attribute where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3485 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iigw06_relation where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3486 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3487 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3488 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0); /* control loop */
}
/*
** ck_iigw06_relation
**
** Description:
**
** This routine tests the verifydb iigw06_relation checks/repairs
**
** Inputs:
**	none.
** Outputs:
**	global variable: test_status - status of tests: TEST_OK, TEST_ABORT
** History:
**	3-aug-93 (stephenb)
**	    Initial creation.
*/
ck_iigw06_relation()
{
  i4 reltid;
    SIprintf("..Testing verifydb checks/repairs for catalog iigw06_relation..\n");
    SIflush(stdout);
    STcopy("iigw06_relation", test_id);
    do
    {
	if ( run_test(IIGW06_RELATION, 1) )
	{
	    SIprintf("....iigw06_relation test 1..\n");
	    SIflush(stdout);
	    /************/
	    /** test 1 **/
	    /************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3532 "duvetest.sc" */	/* register table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register table verifydb_test(database char(24)not null)as import from\
 'current' with DBMS=SXA");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3534 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid from iirelation where relid='verifydb_test'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&reltid);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3536 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3537 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3538 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iigw06_attribute where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3539 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3549 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iigw06_relation where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3550 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIGW06_RELATION, 2) )
	{
	    SIprintf("....iigw06_relation test 2..\n");
	    SIflush(stdout);
	    /************/
	    /** test 2 **/
	    /************/
	    /** set up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3566 "duvetest.sc" */	/* register table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register table verifydb_test(database char(24)not null)as import from\
 'current' with DBMS=SXA");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3568 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid from iirelation where relid='verifydb_test'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&reltid);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3570 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iigw06_attribute where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3571 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3581 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3582 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iigw06_relation where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3583 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3584 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if (run_test(IIGW06_RELATION, 3) )
	{
	    SIprintf("....iigw06_relation test 3..\n");
	    SIflush(stdout);
	    /************/
	    /** test 3 **/
	    /************/
	    /** set up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3600 "duvetest.sc" */	/* register table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register table verifydb_test(database char(24)not null)as import from\
 'current' with DBMS=SXA");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3602 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid from iirelation where relid='verifydb_test'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&reltid);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3604 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iigw06_relation set reltidx=1 where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3606 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
/* # line 3614 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3615 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iigw06_relation where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3616 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3617 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iigw06_attribute where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3618 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while(0);  /* control loop */
}
/*
** ck_iihistogram - 
**
** Description:
**
**  This routine tests the verifydb iihistogram catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	18-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
**	18-oct-93 (teresa)
**	    changed test1 to assure that iistatistics is not empty, since an
**	    empty iistatistics yeilds S_DU1651_INVALID_IIHISTOGRAM instead of
**	    the desired S_DU164A_NO_STATS_FOR_HISTOGRAM
*/
ck_iihistogram()
{
  i4 id1;
  i4 id2;
  i4 val;
    SIprintf ("..Testing verifydb checks/repairs for catalog iihistogram..\n");
    SIflush(stdout);
    STcopy("iiihistogram",test_id);
    do
    {
	if ( run_test(IIHISTOGRAM, 1) )
	{
	    SIprintf ("....iihistogram test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3670 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_hist1(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3671 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relstat from iirelation where relid='duve_his\
t1'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3673 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iistatistics(stabbase, stabindex, satno, snumcells)values\
(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1, 0)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3675 "duvetest.sc" */	/* host code */
	    val |= TCB_ZOPTSTAT;
/* # line 3676 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_hist1'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3678 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select max(reltid)");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3679 "duvetest.sc" */	/* host code */
	    id1++;
/* # line 3680 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"insert into iihistogram valu\
es(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, 1, 0, 'hi there')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3681 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3691 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iihistogram where htabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3692 "duvetest.sc" */	/* host code */
	    id1--;
/* # line 3693 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_hist1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3694 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iistatistics where stabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and stabindex=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3696 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIHISTOGRAM, 2) )
	{	
	    SIprintf ("....iihistogram test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3711 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_hist2(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3712 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relstat from iirelation where relid='duve_his\
t2'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3714 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iistatistics(stabbase, stabindex, satno, snumcells)values\
(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1, 50)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3716 "duvetest.sc" */	/* host code */
	    val |= TCB_ZOPTSTAT;
/* # line 3717 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_hist2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3719 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"insert into iihistogram valu\
es(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1, 0, 'hi there')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3720 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"insert into iihistogram valu\
es(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1, 1, 'hi there')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3721 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"insert into iihistogram valu\
es(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1, 3, 'hi there')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3722 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3732 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iihistogram where htabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3733 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iistatistics where stabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3734 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_hist2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3735 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iiindex - 
**
** Description:
**
**  This routine tests the verifydb index catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	09-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
**	14-jan-94 (anitap)
**	    added logic to test check #4 in DUVECHK.SC for ckindex().
*/
ck_iiindex()
{
  i4 id1;
  i4 id2;
  i4 stat, dep_type;
    SIprintf ("..Testing verifydb checks/repairs for catalog iiindex..\n");
    SIflush(stdout);
    STcopy("index",test_id);
    do
    {
	if ( run_test(IIINDEX, 1) )
	{
	    SIprintf ("....iiindex test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3786 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiindex(baseid, indexid, sequence)values(30, 30, 0)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3788 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3798 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiindex where baseid=30 and indexid=30");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3799 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIINDEX, 2) )
	{
	    SIprintf ("....iiindex test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3815 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_indx2(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3816 "duvetest.sc" */	/* create index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create index i_duve_indx2 on duve_indx2(a)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3817 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='i_duve_indx2'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&stat);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3819 "duvetest.sc" */	/* host code */
	    stat &= ~TCB_INDEX;
/* # line 3820 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&stat);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='i_duve_indx2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3822 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    stat |= TCB_INDEX;
/* # line 3833 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&stat);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='i_duve_indx2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3835 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_indx2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3836 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIINDEX, 3) )
	{
	    SIprintf ("....iiindex test 3..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 3  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3852 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_indx3(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3853 "duvetest.sc" */	/* create index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create index i_duve_indx3 on duve_indx3(a)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3854 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='i_duve_indx3'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3856 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiindex set idom2=2, idom3=3 where baseid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and indexid=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3858 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3868 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiindex set idom2=0, idom3=0 where baseid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and indexid=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3870 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop index i_duve_indx3");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3871 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_indx3");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3872 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIINDEX, 4) )
	{
	    SIprintf ("....iiindex test 4..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 4  **/
	    /*************/
	    dep_type = DB_INDEX;
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3889 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_indx4(a i4 not null unique)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3890 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_indx4'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3892 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where inid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dtype=");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3894 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3904 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_indx4");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3905 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iiintegrity - 
**
** Description:
**
**  This routine tests the verifydb iiintegrity catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	11-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
**      23-nov-93 (anitap)
**          Added tests 5-10 for FIPS constraints' project.
**	01-feb-94 (anitap)
**	    Get rid of iiqrytext tuples for procedures and rules and iitree
**	    tuples for rules associated with check and referential constraints.
*/
ck_iiintegrity()
{
  i4 id1;
  i4 id2;
  i4 id3;
  i4 id4;
  i4 id5;
  i4 id6;
  i4 id7;
  i4 id8;
  i4 id9;
  i4 id10;
  i4 id11;
  i4 id12;
  i4 id13;
  i4 id14;
  i4 id15;
  i4 id16;
  i4 val;
  i4 qryid1, qryid2, treeid1, treeid2;
  i4 dep_type, indep_type, dep1_type;
  i4 stat_type, stat1_type, stat2_type;
    SIprintf ("..Testing verifydb checks/repairs for catalog iiintegrity..\n");
    SIflush(stdout);
    STcopy("iiintegrity",test_id);
    do
    {
	if ( run_test(IIINTEGRITY, 1) )
	{
	    SIprintf ("....iiintegrity test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3976 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiintegrity(inttabbase, inttabidx)values(31, 31)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3978 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 3989 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiintegrity where inttabbase=31 and inttabidx=31");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 3990 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIINTEGRITY, 2) )
	{
	    SIprintf ("....iiintegrity test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 4006 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_int2(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4007 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into duve_int2 values(0, 0)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4008 "duvetest.sc" */	/* create integrity */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create integrity on duve_int2 is a<100");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4009 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='duve_int2'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4011 "duvetest.sc" */	/* host code */
	    val &= ~TCB_INTEGS;
/* # line 4012 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_int2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4014 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    val |= TCB_INTEGS;
/* # line 4025 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_int2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4027 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_int2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4028 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIINTEGRITY, 3) )
	{
	    SIprintf ("....iiintegrity test 3..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 3  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 4044 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_int3(a i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4045 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"insert into duve_int3 values\
(1)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4046 "duvetest.sc" */	/* create integrity */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create integrity on duve_int3 is a>0");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4047 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_int3'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4049 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select inttreeid1, inttreeid2, intqryid1, intqryid2 from iiintegrity \
where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&treeid1);
      IIgetdomio((short *)0,1,30,4,&treeid2);
      IIgetdomio((short *)0,1,30,4,&qryid1);
      IIgetdomio((short *)0,1,30,4,&qryid2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4053 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&treeid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&treeid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4054 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 4065 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4067 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qryid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qryid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4068 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_int3");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4069 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIINTEGRITY, 4) )
	{
	    SIprintf ("....iiintegrity test 4..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 4  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 4085 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_int4(a i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4086 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"insert into duve_int4 values\
(1)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4087 "duvetest.sc" */	/* create integrity */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create integrity on duve_int4 is a>0");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4088 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_int4'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4090 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select inttreeid1, inttreeid2, intqryid1, intqryid2 from iiintegrity \
where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&treeid1);
      IIgetdomio((short *)0,1,30,4,&treeid2);
      IIgetdomio((short *)0,1,30,4,&qryid1);
      IIgetdomio((short *)0,1,30,4,&qryid2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4094 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qryid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qryid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4095 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 4106 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4108 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&treeid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&treeid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4109 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_int4");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4110 "duvetest.sc" */	/* host code */
	    close_db();
	}	
	if ( run_test(IIINTEGRITY, 5) )
        {
            SIprintf ("....iiintegrity test 5..\n");
            SIflush(stdout);
            /*************/
            /** test 5  **/
            /*************/
            /* This test is part of test 5. Tests for keys
            ** for unique constraints.
            */
            /** setup **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 4130 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_int5(a i4 not null unique)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4131 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_int5'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4133 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select inttreeid1, inttreeid2, intqryid1, intqryid2, consid1, consid2\
 from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&treeid1);
      IIgetdomio((short *)0,1,30,4,&treeid2);
      IIgetdomio((short *)0,1,30,4,&qryid1);
      IIgetdomio((short *)0,1,30,4,&qryid2);
      IIgetdomio((short *)0,1,30,4,&id3);
      IIgetdomio((short *)0,1,30,4,&id4);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4139 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iikey where key_consid1=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and key_consid2=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4140 "duvetest.sc" */	/* host code */
            close_db();
            /** run test **/
            report_vdb();
            run_vdb();
            /** clean up **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 4150 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_int5");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4152 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4154 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&treeid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&treeid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4156 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qryid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qryid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4157 "duvetest.sc" */	/* host code */
            close_db();
        }
	if ( run_test(IIINTEGRITY, 6) )
        {
            SIprintf ("....iiintegrity test 6..\n");
            SIflush(stdout);
            /*************/
            /** test 6  **/
            /*************/
	    stat_type = DBR_S_INSERT;
	    stat1_type = DBR_S_UPDATE;	
	    stat2_type = DBR_S_DELETE;
            /* This test is part of test 6. Tests that the unique
	    ** constraint on which the referential depends is 
	    ** present.	
            */
            /** setup **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 4181 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_int6(a i4 not null unique)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4182 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_int6r(ar i4 references duve_int6(a))");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4183 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_int6'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4185 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_int6r'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id5);
      IIgetdomio((short *)0,1,30,4,&id6);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4188 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select inttreeid1, inttreeid2, intqryid1, intqryid2, consid1, consid2\
 from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&treeid1);
      IIgetdomio((short *)0,1,30,4,&treeid2);
      IIgetdomio((short *)0,1,30,4,&qryid1);
      IIgetdomio((short *)0,1,30,4,&qryid2);
      IIgetdomio((short *)0,1,30,4,&id3);
      IIgetdomio((short *)0,1,30,4,&id4);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4193 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select intqryid1, intqryid2 from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id5);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id6);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id7);
      IIgetdomio((short *)0,1,30,4,&id8);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4196 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_qryid1, rule_qryid2 from iirule where rule_tabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_tabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_statement=");
    IIputdomio((short *)0,1,30,4,&stat2_type);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id9);
      IIgetdomio((short *)0,1,30,4,&id10);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4200 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_qryid1, rule_qryid2 from iirule where rule_tabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_tabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_statement=");
    IIputdomio((short *)0,1,30,4,&stat1_type);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id11);
      IIgetdomio((short *)0,1,30,4,&id12);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4204 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_qryid1, rule_qryid2 from iirule where rule_tabbase=");
    IIputdomio((short *)0,1,30,4,&id5);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_tabidx=");
    IIputdomio((short *)0,1,30,4,&id6);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_statement=");
    IIputdomio((short *)0,1,30,4,&stat1_type);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id13);
      IIgetdomio((short *)0,1,30,4,&id14);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4208 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_qryid1, rule_qryid2 from iirule where rule_tabbase=");
    IIputdomio((short *)0,1,30,4,&id5);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_tabidx=");
    IIputdomio((short *)0,1,30,4,&id6);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_statement=");
    IIputdomio((short *)0,1,30,4,&stat_type);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id15);
      IIgetdomio((short *)0,1,30,4,&id16);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4213 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4214 "duvetest.sc" */	/* host code */
            close_db();
            /** run test **/
            report_vdb();
            run_vdb();
            /** clean up **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 4224 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_int6");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4225 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_int6r");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4227 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4229 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&treeid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&treeid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4231 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qryid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qryid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4233 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id7);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id8);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4235 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id9);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id10);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4237 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id11);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id12);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4239 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id13);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id14);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4241 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id15);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id16);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4242 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iikey where key_consid1=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and key_consid2=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4244 "duvetest.sc" */	/* host code */
            close_db();
        }
        if ( run_test(IIINTEGRITY, 7) )
        {
            SIprintf ("....iiintegrity test 7..\n");
            SIflush(stdout);
            /*************/
            /** test 7  **/
            /*************/
	    dep_type = DB_INDEX;
            /* Tests for iidbdepends for unique constraint */
            /** setup **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 4263 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_int7(a i4 not null unique)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4264 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_int7'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4266 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select indexid from iiindex where baseid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id3);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4268 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where inid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and deid2=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dtype=");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4269 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select inttreeid1, inttreeid2, intqryid1, intqryid2, consid1, consid2\
 from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&treeid1);
      IIgetdomio((short *)0,1,30,4,&treeid2);
      IIgetdomio((short *)0,1,30,4,&qryid1);
      IIgetdomio((short *)0,1,30,4,&qryid2);
      IIgetdomio((short *)0,1,30,4,&id3);
      IIgetdomio((short *)0,1,30,4,&id4);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4274 "duvetest.sc" */	/* host code */
            close_db();
            /** run test **/
            report_vdb();
            run_vdb();
            /** clean up **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 4284 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_int7");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4286 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4288 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&treeid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&treeid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4290 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qryid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qryid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4292 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iikey where key_consid1=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and key_consid2=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4294 "duvetest.sc" */	/* host code */
            close_db();
            /* This test is part of test 7. Tests for iidbdepends
            ** for check constraints.
            */
            SIprintf ("....iiintegrity test 7A..\n");
            SIflush(stdout);
            /*************/
            /** test 7A **/
            /*************/
	    dep_type = DB_RULE;
            /** setup **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 4312 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_int7a(a i4 check(a is not null))");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4313 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_int7a'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4315 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_id1, rule_id2, rule_qryid1, rule_qryid2, rule_treeid1, ru\
le_treeid2 from iirule where rule_tabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_tabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id3);
      IIgetdomio((short *)0,1,30,4,&id4);
      IIgetdomio((short *)0,1,30,4,&id7);
      IIgetdomio((short *)0,1,30,4,&id8);
      IIgetdomio((short *)0,1,30,4,&id9);
      IIgetdomio((short *)0,1,30,4,&id10);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4320 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where inid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and deid1=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and deid2=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dtype=");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4322 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select inttreeid1, inttreeid2, intqryid1, intqryid2, consid1, consid2\
 from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&treeid1);
      IIgetdomio((short *)0,1,30,4,&treeid2);
      IIgetdomio((short *)0,1,30,4,&qryid1);
      IIgetdomio((short *)0,1,30,4,&qryid2);
      IIgetdomio((short *)0,1,30,4,&id5);
      IIgetdomio((short *)0,1,30,4,&id6);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4327 "duvetest.sc" */	/* host code */
            close_db();
            /** run test **/
            report_vdb();
            run_vdb();
            /** clean up **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 4337 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_int7a");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4339 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4341 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&treeid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&treeid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4343 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&id9);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&id10);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4345 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qryid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qryid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4347 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id7);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id8);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4348 "duvetest.sc" */	/* host code */
            close_db();
            /* This test is part of test 7. Tests for iidbdepends
            ** for referential constraints.
            */
            SIprintf ("....iiintegrity test 7B..\n");
            SIflush(stdout);
            /*************/
            /** test 7B **/
            /*************/
	    indep_type = DB_CONS;
	    dep_type = DB_DBP;
	    dep1_type = DB_INDEX;
            /** setup **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 4369 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_int7b(a i4 not null unique)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4370 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_int7br(b i4 references duve_int7b(a))");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4372 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_int7br'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4374 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select deid1, deid2, dbp_txtid1, dbp_txtid2 from iidbdepends, iiproce\
dure where inid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dtype=");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)"and itype=");
    IIputdomio((short *)0,1,30,4,&indep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and deid1=dbp_id and deid2=dbp_idx and dbp_parameter_count=2");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id3);
      IIgetdomio((short *)0,1,30,4,&id4);
      IIgetdomio((short *)0,1,30,4,&id7);
      IIgetdomio((short *)0,1,30,4,&id8);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4382 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where inid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dtype=");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)"and itype=");
    IIputdomio((short *)0,1,30,4,&indep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)"and deid1=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and deid2=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4386 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where inid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dtype=");
    IIputdomio((short *)0,1,30,4,&dep1_type);
    IIwritio(0,(short *)0,1,32,0,(char *)"and itype=");
    IIputdomio((short *)0,1,30,4,&indep_type);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4388 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select inttreeid1, inttreeid2, intqryid1, intqryid2, consid1, consid2\
 from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&treeid1);
      IIgetdomio((short *)0,1,30,4,&treeid2);
      IIgetdomio((short *)0,1,30,4,&qryid1);
      IIgetdomio((short *)0,1,30,4,&qryid2);
      IIgetdomio((short *)0,1,30,4,&id5);
      IIgetdomio((short *)0,1,30,4,&id6);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4393 "duvetest.sc" */	/* host code */
            close_db();
            /** run test **/
            report_vdb();
            run_vdb();
            /** clean up **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 4403 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_int7b");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4404 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_int7br");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4406 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4408 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&treeid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&treeid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4410 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qryid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qryid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4412 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id7);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id8);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4414 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiprocedure where dbp_id=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dbp_idx=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4415 "duvetest.sc" */	/* host code */
            close_db();
        }
        if ( run_test(IIINTEGRITY, 8) )
        {
            SIprintf ("....iiintegrity test 8..\n");
            SIflush(stdout);
            /*************/
            /** test 8  **/
            /*************/
            /* This test is part of test 8. Tests for rules
            ** for check constraints.
            */
            /** setup **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 4434 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_int8(a i4 check(a is not null))");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4435 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_int8'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4437 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_qryid1, rule_qryid2, rule_treeid1, rule_treeid2 from iiru\
le where rule_tabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_tabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id5);
      IIgetdomio((short *)0,1,30,4,&id6);
      IIgetdomio((short *)0,1,30,4,&id7);
      IIgetdomio((short *)0,1,30,4,&id8);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4442 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirule where rule_tabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_tabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4444 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select inttreeid1, inttreeid2, intqryid1, intqryid2, consid1, consid2\
 from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&treeid1);
      IIgetdomio((short *)0,1,30,4,&treeid2);
      IIgetdomio((short *)0,1,30,4,&qryid1);
      IIgetdomio((short *)0,1,30,4,&qryid2);
      IIgetdomio((short *)0,1,30,4,&id3);
      IIgetdomio((short *)0,1,30,4,&id4);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4449 "duvetest.sc" */	/* host code */
            close_db();
            /** run test **/
            report_vdb();
            run_vdb();
            /** clean up **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 4459 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_int8");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4461 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4463 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&treeid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&treeid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4465 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&id7);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&id8);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4467 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qryid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qryid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4469 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id5);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id6);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4470 "duvetest.sc" */	/* host code */
            close_db();
            /* This test is part of test 8. Tests for rules
            ** for referential constraints.
            */
            SIprintf ("....iiintegrity test 8A..\n");
            SIflush(stdout);
            /*************/
            /** test 8A **/
            /*************/
	    stat_type = DBR_S_INSERT;
            /** setup **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 4488 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_int8a(a i4 not null unique)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4489 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_int8ar(b i4 references duve_int8a(a))");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4491 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_int8ar'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4493 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_treeid1, rule_treeid2, rule_qryid1, rule_qryid2 from iiru\
le where rule_tabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_tabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_statement=");
    IIputdomio((short *)0,1,30,4,&stat_type);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id7);
      IIgetdomio((short *)0,1,30,4,&id8);
      IIgetdomio((short *)0,1,30,4,&id9);
      IIgetdomio((short *)0,1,30,4,&id10);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4497 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirule where rule_tabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_tabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_statement=");
    IIputdomio((short *)0,1,30,4,&stat_type);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4499 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select inttreeid1, inttreeid2, intqryid1, intqryid2, consid1, consid2\
 from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&treeid1);
      IIgetdomio((short *)0,1,30,4,&treeid2);
      IIgetdomio((short *)0,1,30,4,&qryid1);
      IIgetdomio((short *)0,1,30,4,&qryid2);
      IIgetdomio((short *)0,1,30,4,&id5);
      IIgetdomio((short *)0,1,30,4,&id6);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4504 "duvetest.sc" */	/* host code */
            close_db();
            /** run test **/
            report_vdb();
            run_vdb();
            /** clean up **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 4514 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_int8a");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4515 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_int8ar");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4517 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4519 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&treeid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&treeid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4521 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&id7);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&id8);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4523 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qryid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qryid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4525 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id9);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id10);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4526 "duvetest.sc" */	/* host code */
            close_db();
        }
        if ( run_test(IIINTEGRITY, 9) )
        {
            SIprintf ("....iiintegrity test 9..\n");
            SIflush(stdout);
            /*************/
            /** test 9  **/
            /*************/
	    dep_type = DB_DBP;
	    indep_type = DB_CONS;
            /* Tests for procedures for referential constraints */
            /** setup **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 4546 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_int9(a i4 not null unique)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4547 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_int9r(b i4 references duve_int9(a))");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4549 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_int9r'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4551 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select deid1, deid2, dbp_txtid1, dbp_txtid2 from iidbdepends, iiproce\
dure where inid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dtype=");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)"and itype=");
    IIputdomio((short *)0,1,30,4,&indep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and deid1=dbp_id and deid2=dbp_idx and dbp_parameter_count=2");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id3);
      IIgetdomio((short *)0,1,30,4,&id4);
      IIgetdomio((short *)0,1,30,4,&id5);
      IIgetdomio((short *)0,1,30,4,&id6);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4558 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_qryid1, rule_qryid2, rule_treeid1, rule_treeid2 from iiru\
le, iiprocedure where rule_dbp_name=dbp_name and dbp_id=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dbp_idx=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id7);
      IIgetdomio((short *)0,1,30,4,&id8);
      IIgetdomio((short *)0,1,30,4,&id9);
      IIgetdomio((short *)0,1,30,4,&id10);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4564 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiprocedure where dbp_id=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dbp_idx=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4566 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select inttreeid1, inttreeid2, intqryid1, intqryid2, consid1, consid2\
 from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&treeid1);
      IIgetdomio((short *)0,1,30,4,&treeid2);
      IIgetdomio((short *)0,1,30,4,&qryid1);
      IIgetdomio((short *)0,1,30,4,&qryid2);
      IIgetdomio((short *)0,1,30,4,&id3);
      IIgetdomio((short *)0,1,30,4,&id4);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4571 "duvetest.sc" */	/* host code */
            close_db();
            /** run test **/
            report_vdb();
            run_vdb();
            /** clean up **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 4581 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_int9");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4582 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_int9r");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4584 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4586 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&treeid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&treeid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4588 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&id9);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&id10);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4590 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qryid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qryid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4592 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id7);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id8);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4594 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id5);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id6);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4596 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirule where rule_tabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_tabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4598 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iikey where key_consid1=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and key_consid2=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4600 "duvetest.sc" */	/* host code */
            close_db();
        }
        if ( run_test(IIINTEGRITY, 10) )
        {
            SIprintf ("....iiintegrity test 10..\n");
            SIflush(stdout);
            /*************/
            /** test 10  **/
            /*************/
            /* Tests for indexes for UNIQUE constraint */
            /** setup **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 4617 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_int10(a i4 not null unique)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4618 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_int10'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4620 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiindex where baseid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4621 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select inttreeid1, inttreeid2, intqryid1, intqryid2, consid1, consid2\
 from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&treeid1);
      IIgetdomio((short *)0,1,30,4,&treeid2);
      IIgetdomio((short *)0,1,30,4,&qryid1);
      IIgetdomio((short *)0,1,30,4,&qryid2);
      IIgetdomio((short *)0,1,30,4,&id3);
      IIgetdomio((short *)0,1,30,4,&id4);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4626 "duvetest.sc" */	/* host code */
            close_db();
            /** run test **/
            report_vdb();
            run_vdb();
            /** clean up **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 4636 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_int10");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4638 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4640 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&treeid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&treeid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4642 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qryid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qryid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4644 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iikey where key_consid1=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and key_consid2=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4646 "duvetest.sc" */	/* host code */
            close_db();
        }
    } while (0);  /* control loop */
}
/* ck_iikey -
**
** Description:
**
** This routine tests the verifydb iikey catalog checks
**
** Inputs:
**      None.
** Outputs:
**      global variable: test_status    - status of tests: TEST_OK, TEST_ABORT
** History:
**      23-nov-93 (anitap)
**          initial creation for FIPS constraints and CREATE SCHEMA projects.
*/
ck_iikey()
{
  i4 id1;
  i4 id2;
  i4 id3;
  i4 id4;
  i4 fakeid;
    SIprintf ("..Testing verifydb checks for catalog iikey..\n");
    SIflush(stdout);
    STcopy("iikey", test_id);
    do
    {
        if (run_test(IIKEY, 1))
        {
           SIprintf ("....iikey test 1..\n");
           SIflush(stdout);
           /*************/
           /** test 1  **/
           /*************/
           /** setup **/
           open_db();
           if (test_status == TEST_ABORT)
              break;
/* # line 4694 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_key1(a i4 not null unique)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4695 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_key1'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4698 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select consid1, consid2 from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id3);
      IIgetdomio((short *)0,1,30,4,&id4);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4702 "duvetest.sc" */	/* host code */
	   fakeid = id3 + 1;
/* # line 4703 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iikey set key_consid1\
=");
    IIputdomio((short *)0,1,30,4,&fakeid);
    IIwritio(0,(short *)0,1,32,0,(char *)"where key_consid1=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and key_consid2=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4706 "duvetest.sc" */	/* host code */
           close_db();
           /** run test **/
           report_vdb();
           run_vdb();
           /** clean up **/
           open_db();
           if (test_status == TEST_ABORT)
              break;
/* # line 4716 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iikey where key_consid1=");
    IIputdomio((short *)0,1,30,4,&fakeid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and key_consid2=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4718 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_key1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4719 "duvetest.sc" */	/* host code */
           close_db();
        }
    } while (0);  /* control loop */
}
/*
** ck_iilocations - 
**
** Description:
**
**  This routine tests the verifydb iilocations catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	16-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
*/
ck_iilocations()
{
  i4 id1;
  i4 id2;
    SIprintf ("..Testing verifydb checks/repairs for catalog iilocations..\n");
    SIflush(stdout);
    STcopy("iilocations",test_id);
    do
    {
	if ( run_test(IILOCATIONS, 1) )
	{
	    SIprintf ("....iilocations test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 4768 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iilocations values(8, '_bad!loc', 'full_pathname_goes_her\
e')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4770 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 4780 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iilocations where lname='_bad!loc'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4781 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IILOCATIONS, 2) )
	{
	    SIprintf ("....iilocations test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 4797 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iilocations values( -1, 'fake_loc_2', 'full_pathname_goes\
_here')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4799 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 4809 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iilocations where lname='fake_loc_2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4810 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iixprocedure - exercise IIXPROCEDURE tests
**
** Description:
**
**  This routine tests the verifydb iixprocedure catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	27-jan-94 (andre)
**	    written
*/
ck_iixprocedure()
{
  i4 id1;
  i4 val;
  u_i4 tid;
    SIprintf ("..Testing verifydb checks/repairs for catalog iixprocedure..\n");
    SIflush(stdout);
    STcopy("iixprocedure",test_id);
    do
    {
	if ( run_test(IIXPROCEDURE, 1) )
	{
	    SIprintf ("....iixprocedure test 1..\n");
	    SIflush(stdout);
#if	0
	    /* 
	    ** skip this test - cleanup gets messy - dropping the index may 
	    ** result in a deadlock (it is used in qeu_d_cascade()) and cursor 
	    ** delete fails due to insufficient privileges - both can be 
	    ** addressed in BE, but not now
	    */
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_A9db();
	    if (test_status == TEST_ABORT)
		break;
	    /* 
	    ** insert into IIXPROCEDURE a tuple for which there should be no 
	    ** match in IIPROCEDURE
	    */
/* # line 4871 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iixprocedure(dbp_id, dbp_idx, tidp)values(0, 0, 0)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4874 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_A9db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 4885 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iixprocedure where dbp_id=0 and dbp_idx=0");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4886 "duvetest.sc" */	/* host code */
	    close_db();
#else
	    SIprintf ("this test is currently skipped.\n");
	    SIflush(stdout);
#endif
	}
	if ( run_test(IIXPROCEDURE, 2) )
	{
	    SIprintf ("....iixprocedure test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_A9db();
	    if (test_status == TEST_ABORT)
		break;
	    /*
	    ** create a dbproc, then delete the newly added IIXPROCEDURE tuple
	    */
/* # line 4910 "duvetest.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"duve_iixprocedure2");
    IIwritio(0,(short *)0,1,32,0,(char *)
"drop procedure duve_iixprocedure2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4911 "duvetest.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"duve_iixprocedure2");
    IIwritio(0,(short *)0,1,32,0,(char *)
"create procedure duve_iixprocedure2 as begin return; end");
    IIputctrl(0);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4912 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select tid, dbp_id from iiprocedure where dbp_owner=user and dbp_name\
='duve_iixprocedure2'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tid);
      IIgetdomio((short *)0,1,30,4,&id1);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4917 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iixprocedure where tidp=");
    IIputdomio((short *)0,1,30,4,&tid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4918 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_A9db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 4928 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iixprocedure where dbp_id=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dbp_idx=0");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4934 "duvetest.sc" */	/* host code */
	    if (val == 0)
	    {
/* # line 4936 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iixprocedure(dbp_id, dbp_idx, tidp)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
    IIputdomio((short *)0,1,30,4,&tid);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4939 "duvetest.sc" */	/* host code */
	    }
/* # line 4941 "duvetest.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"duve_iixprocedure2");
    IIwritio(0,(short *)0,1,32,0,(char *)
"drop procedure duve_iixprocedure2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 4943 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iiprocedure - 
**
** Description:
**
**  This routine tests the verifydb iiproccedure catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	16-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
**      23-nov-93 (anitap)
**          Added test 3 to check schema is created for orphaned procedure.
**          Also added test 4 to check that iidbdepends tuples are present
**          for procedures for referential constraints.
**	28-jan-94 (andre)
**	    added tests 5-8
**	29-jan-94 (andre)
**	    updated cleanup for tests 1, 2, and 4 since verifydb deletes 
**	    IIPROCEDURE tuple for the dbproc.
**	02-feb-94 (anitap)
**	    updated cleanup for tests 1,2 and 4 to drop the procedures.
*/
ck_iiprocedure()
{
  i4 id1;
  i4 id2;
  i4 id3;
  i4 id4;
  i4 id5;
  i4 id6;
  i4 dbp_type;
  i4 dbp_mask, dep_type, indep_type;
    dbp_type = DB_DBP;
    SIprintf ("..Testing verifydb checks/repairs for catalog iiprocedure..\n");
    SIflush(stdout);
    STcopy("iiprocedure",test_id);
    do
    {
	if ( run_test(IIPROCEDURE, 1) )
	{
	    SIprintf ("....iiprocedure test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5010 "duvetest.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"duve_proc1");
    IIwritio(0,(short *)0,1,32,0,(char *)"create procedure duve_proc1 \
as");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                begin");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    message 'hi';");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                end");
    IIputctrl(0);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5014 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbp_txtid1, dbp_txtid2 from iiprocedure where dbp_name='duve_p\
roc1'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5017 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprocedure set dbp_txtid1=123, dbp_txtid2=456 where dbp_name=\
'duve_proc1'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5019 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5030 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5031 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIPROCEDURE, 2) )
	{
	    SIprintf ("....iiprocedure test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5047 "duvetest.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"duve_proc2");
    IIwritio(0,(short *)0,1,32,0,(char *)"create procedure duve_proc2 \
as");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                begin");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    message 'hi';");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                end");
    IIputctrl(0);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5051 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_prottbl(a \
i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5052 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbp_txtid1, dbp_txtid2, dbp_id from iiprocedure where dbp_name\
='duve_proc2'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&id3);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5055 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid from iirelation where relid='duve_prottbl'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id4);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5057 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iiprocedure set dbp_i\
d=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIwritio(0,(short *)0,1,32,0,(char *)"where dbp_name='duve_proc2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5059 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5070 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5071 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_prottbl");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5072 "duvetest.sc" */	/* host code */
	    close_db();
	}
        if ( run_test(IIPROCEDURE, 3) )
        {
            SIprintf ("....iiprocedure test 3..\n");
            SIflush(stdout);
            /*************/
            /** test 3  **/
            /*************/
            /** setup **/
            open_db();
            if (test_status == TEST_ABORT)
               break;
/* # line 5088 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5089 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session authorization su\
per");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5090 "duvetest.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"duve_proc3");
    IIwritio(0,(short *)0,1,32,0,(char *)"create procedure duve_proc3 \
as");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                begin");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    message 'hi';");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                end");
    IIputctrl(0);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5094 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5095 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session authorization $ingres");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5096 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iischema where schema_name='super'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5097 "duvetest.sc" */	/* host code */
           close_db();
           /** run test **/
           report_vdb();
           run_vdb();
           /** clean up **/
           open_db();
           if (test_status == TEST_ABORT)
              break;
/* # line 5107 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5108 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session authorization su\
per");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5109 "duvetest.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"duve_proc3");
    IIwritio(0,(short *)0,1,32,0,(char *)"drop procedure duve_proc3");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5110 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5111 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session authorization $ingres");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5112 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iischema where schema_name='super'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5113 "duvetest.sc" */	/* host code */
           close_db();
        }
        if ( run_test(IIPROCEDURE, 4) )
        {
            SIprintf ("....iiprocedure test 4..\n");
            SIflush(stdout);
            /*************/
            /** test 4  **/
            /*************/
	    dep_type = DB_DBP;
	    indep_type = DB_CONS;
            /** setup **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 5131 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_proc(a i4 not null unique)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5132 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_procr(b i4 references duve_proc(a))");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5134 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_procr'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5137 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select deid1, deid2, dbp_txtid1, dbp_txtid2 from iidbdepends, iiproce\
dure where inid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dtype=");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)"and itype=");
    IIputdomio((short *)0,1,30,4,&indep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and deid1=dbp_id and deid2=dbp_idx and dbp_parameter_count=2");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id3);
      IIgetdomio((short *)0,1,30,4,&id4);
      IIgetdomio((short *)0,1,30,4,&id5);
      IIgetdomio((short *)0,1,30,4,&id6);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5144 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where inid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dtype=");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIwritio(0,(short *)0,1,32,0,(char *)"and deid1=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIwritio(0,(short *)0,1,32,0,(char *)"and deid2=");
    IIputdomio((short *)0,1,30,4,&id4);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5149 "duvetest.sc" */	/* host code */
            close_db();
            /** run test **/
            report_vdb();
            run_vdb();
            /** clean up **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 5159 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_proc");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5160 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_procr");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5162 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id5);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id6);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5163 "duvetest.sc" */	/* host code */
            close_db();
        }
	if ( run_test(IIPROCEDURE, 5) )
	{
	    SIprintf ("....iiprocedure test 5..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 5  **/
	    /*************/
	    /** setup **/
	    /*
	    ** create a dbproc dependent on SELECT privilege on iirelation, then
	    ** delete the IIPRIV tuple representing the privilege descriptor
	    */
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5186 "duvetest.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"duve_proc5");
    IIwritio(0,(short *)0,1,32,0,(char *)"drop procedure duve_proc5");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5188 "duvetest.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"duve_proc5");
    IIwritio(0,(short *)0,1,32,0,(char *)"create procedure duve_proc5 \
as");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                declare cnt \
i;");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                begin");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    select count(*) into :cnt from iirelation;");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                end");
    IIputctrl(0);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5196 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iipriv where exists(select 1 from iiprocedure where dbp_n\
ame='duve_proc5' and dbp_owner=user and d_obj_id=dbp_id and d_obj_idx=\
dbp_idx and d_priv_number=0 and d_obj_type=");
    IIputdomio((short *)0,1,30,4,&dbp_type);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5206 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5217 "duvetest.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"duve_proc5");
    IIwritio(0,(short *)0,1,32,0,(char *)"drop procedure duve_proc5");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5219 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIPROCEDURE, 6) )
	{
	    SIprintf ("....iiprocedure test 6..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 6  **/
	    /*************/
	    /** setup **/
	    /*
	    ** create a dbproc dependent on SELECT privilege on iirelation and 
	    ** on user's table, then update dbp_mask1 to indicate that there 
	    ** exists no independent object/privilege list
	    */
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5243 "duvetest.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"duve_proc6");
    IIwritio(0,(short *)0,1,32,0,(char *)"drop procedure duve_proc6");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5244 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_proc6_tbl1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5246 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_proc6_tbl1(i i)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5248 "duvetest.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"duve_proc6");
    IIwritio(0,(short *)0,1,32,0,(char *)"create procedure duve_proc6 \
as");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                begin");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    insert into duve_proc6_tbl1");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                        select count(*) from iirelation;");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                end");
    IIputctrl(0);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5254 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbp_mask1 from iiprocedure where dbp_name='duve_proc6' and dbp\
_owner=user");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dbp_mask);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5258 "duvetest.sc" */	/* host code */
	    dbp_mask &= ~DB_DBP_INDEP_LIST;
/* # line 5260 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprocedure set dbp_mask1=");
    IIputdomio((short *)0,1,30,4,&dbp_mask);
    IIwritio(0,(short *)0,1,32,0,(char *)
"where dbp_name='duve_proc6' and dbp_owner=user");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5264 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5275 "duvetest.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"duve_proc6");
    IIwritio(0,(short *)0,1,32,0,(char *)"drop procedure duve_proc6");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5276 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_proc6_tbl1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5278 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIPROCEDURE, 7) )
	{
	    SIprintf ("....iiprocedure test 7..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 7  **/
	    /*************/
	    /** setup **/
	    /*
	    ** create a dbproc dependent on SELECT privilege on iirelation, 
	    ** then update (dbp_ubt_id, dbp_ubt_idx) with (reltid, reltidx) of 
	    ** IIRELATION
	    */
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5302 "duvetest.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"duve_proc7");
    IIwritio(0,(short *)0,1,32,0,(char *)"drop procedure duve_proc7");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5304 "duvetest.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"duve_proc7");
    IIwritio(0,(short *)0,1,32,0,(char *)"create procedure duve_proc7 \
as");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                declare cnt \
i;");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                begin");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    select count(*) into :cnt from iirelation;");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                end");
    IIputctrl(0);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5310 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprocedure set dbp_ubt_id=1, dbp_ubt_idx=0 where dbp_name='du\
ve_proc7' and dbp_owner=user");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5314 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5325 "duvetest.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"duve_proc7");
    IIwritio(0,(short *)0,1,32,0,(char *)"drop procedure duve_proc7");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5327 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIPROCEDURE, 8) )
	{
	    SIprintf ("....iiprocedure test 8..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 8  **/
	    /*************/
	    /** setup **/
	    /*
	    ** create a dbproc dependent on user's table, grant a permit on it,
	    ** then update dbp_mask1 to indicate that the dbproc is not 
	    ** grantable
	    */
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5351 "duvetest.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"duve_proc8");
    IIwritio(0,(short *)0,1,32,0,(char *)"drop procedure duve_proc8");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5352 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_proc8_tbl1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5354 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_proc8_tbl1(i i)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5356 "duvetest.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"duve_proc8");
    IIwritio(0,(short *)0,1,32,0,(char *)"create procedure duve_proc8 \
as");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                begin");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    insert into duve_proc8_tbl1 values(0);");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                end");
    IIputctrl(0);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5361 "duvetest.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant all on procedure duve_proc8 to public");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5363 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbp_mask1 from iiprocedure where dbp_name='duve_proc8' and dbp\
_owner=user");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dbp_mask);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5367 "duvetest.sc" */	/* host code */
	    dbp_mask &= ~DB_DBPGRANT_OK;
/* # line 5369 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprocedure set dbp_mask1=");
    IIputdomio((short *)0,1,30,4,&dbp_mask);
    IIwritio(0,(short *)0,1,32,0,(char *)
"where dbp_name='duve_proc8' and dbp_owner=user");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5373 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select proqryid1, proqryid2 from iiprotect where probjname='duve_proc\
8' and probjowner=user");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5377 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5388 "duvetest.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"duve_proc8");
    IIwritio(0,(short *)0,1,32,0,(char *)"drop procedure duve_proc8");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5390 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5391 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_proc8_tbl1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5393 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iiprotect - 
**
** Description:
**
**  This routine tests the verifydb iiprotect catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	11-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
**	10-nov-93 (anitap)
**	    Corrected inttabbase to protabbase in tests 3 & 4.
**	28-jan-94 (andre)
**	    added tests 5-12
*/
ck_iiprotect()
{
  i4 id1;
  i4 id2;
  i4 val;
  i4 qryid1, qryid2, treeid1, treeid2;
    SIprintf ("..Testing verifydb checks/repairs for catalog iiprotect..\n");
    SIflush(stdout);
    STcopy("iiprotect",test_id);
    do
    {
	if ( run_test(IIPROTECT, 1) )
	{
	    SIprintf ("....iiprotect test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5448 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_pro1(i i)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5449 "duvetest.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on duve_pro1 to public");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5450 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select proqryid1, proqryid2 from iiprotect where probjname='duve_pro1\
' and probjowner=user");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&qryid1);
      IIgetdomio((short *)0,1,30,4,&qryid2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5453 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprotect set protabbase=31, protabidx=31 where probjname='duv\
e_pro1' and probjowner=user");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5456 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5467 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qryid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qryid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5468 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_pro1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5469 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIPROTECT, 2) )
	{
	    SIprintf ("....iiprotect test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5485 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_pro2(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5486 "duvetest.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on duve_pro2 to '$ingres'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5487 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='duve_pro2'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5489 "duvetest.sc" */	/* host code */
	    val &= ~TCB_PRTUPS;
/* # line 5490 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_pro2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5492 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    val |= TCB_PRTUPS;
/* # line 5503 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_pro2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5505 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_pro2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5506 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIPROTECT, 3) )
	{
	    SIprintf ("....iiprotect test 3..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 3  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5522 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_pro3(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5523 "duvetest.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on duve_pro3 to '$ingres'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5524 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_pro3'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5526 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select protreeid1, protreeid2, proqryid1, proqryid2 from iiprotect wh\
ere protabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and protabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&treeid1);
      IIgetdomio((short *)0,1,30,4,&treeid2);
      IIgetdomio((short *)0,1,30,4,&qryid1);
      IIgetdomio((short *)0,1,30,4,&qryid2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5529 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprotect set protreeid1=321 where protabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and protabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5531 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5542 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiprotect where protabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and protabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5544 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&treeid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&treeid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5546 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qryid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qryid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5547 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_pro2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5548 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIPROTECT, 4) )
	{
	    SIprintf ("....iiprotect test 4..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 4  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5564 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_pro3(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5565 "duvetest.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on duve_pro3 to '$ingres'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5566 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_pro3'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5568 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select protreeid1, protreeid2, proqryid1, proqryid2 from iiprotect wh\
ere protabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and protabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&treeid1);
      IIgetdomio((short *)0,1,30,4,&treeid2);
      IIgetdomio((short *)0,1,30,4,&qryid1);
      IIgetdomio((short *)0,1,30,4,&qryid2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5572 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qryid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qryid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5573 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5584 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiprotect where protabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and protabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5586 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&treeid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&treeid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5587 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_pro3");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5588 "duvetest.sc" */	/* host code */
	    close_db();	
	}	
	if ( run_test(IIPROTECT, 5) )
	{
	    SIprintf ("....iiprotect test 5..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 5  **/
	    /*************/
	    /** setup **/
	    /*
	    ** create a table, grant a permit on it, then overwrite probjname
	    */
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5610 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_pro5");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5611 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_pro5(i i)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5612 "duvetest.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on duve_pro5 to public");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5613 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprotect set probjname='gibberish' where probjname='duve_pro5\
' and probjowner=user");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5617 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5628 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_pro5");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5630 "duvetest.sc" */	/* host code */
	    close_db();	
	}	
	if ( run_test(IIPROTECT, 6) )
	{
	    SIprintf ("....iiprotect test 6..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 6  **/
	    /*************/
	    /** setup **/
	    /*
	    ** create a table, grant a permit on it, then overwrite proopset
	    */
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    val = DB_EXECUTE;
/* # line 5654 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_pro6");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5655 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_pro6(i i)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5656 "duvetest.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on duve_pro6 to public");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5657 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select proqryid1, proqryid2 from iiprotect where probjname='duve_pro6\
' and probjowner=user");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5660 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iiprotect set proopse\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)
"where probjname='duve_pro6' and probjowner=user");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5664 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5676 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiprotect where probjname='duve_pro6' and probjowner=user");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5678 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5679 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_pro6");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5681 "duvetest.sc" */	/* host code */
	    close_db();	
	}	
	if ( run_test(IIPROTECT, 7) )
	{
	    SIprintf ("....iiprotect test 7..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 7  **/
	    /*************/
	    /** setup **/
	    /*
	    ** create a table, grant a permit on it, then overwrite proopctl
	    */
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    val = DB_EXECUTE;
/* # line 5705 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_pro7");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5706 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_pro7(i i)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5707 "duvetest.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on duve_pro7 to public");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5708 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select proqryid1, proqryid2 from iiprotect where probjname='duve_pro7\
' and probjowner=user");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5711 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iiprotect set proopct\
l=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)
"where probjname='duve_pro7' and probjowner=user");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5715 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5727 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiprotect where probjname='duve_pro7' and probjowner=user");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5729 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5730 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_pro7");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5732 "duvetest.sc" */	/* host code */
	    close_db();	
	}	
	if ( run_test(IIPROTECT, 8) )
	{
	    SIprintf ("....iiprotect test 8..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 8  **/
	    /*************/
	    /** setup **/
	    /*
	    ** create a table, grant a permit on it, then overwrite 
	    ** IIQRYTEXT. status to make this look like a permit created 
	    ** using QUEL
	    */
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5756 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_pro8");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5757 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_pro8(i i)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5758 "duvetest.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on duve_pro8 to public");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5759 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select iiqrytext.status from iiqrytext, iiprotect where probjname='du\
ve_pro8' and probjowner=user and txtid1=proqryid1 and txtid2=proqryid2");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5767 "duvetest.sc" */	/* host code */
	    val &= ~DBQ_SQL;
/* # line 5769 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiqrytext from iiprotect set status=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)
"where probjname='duve_pro8' and probjowner=user and txtid1=proqryid1 \
and txtid2=proqryid2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5777 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5788 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_pro8");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5790 "duvetest.sc" */	/* host code */
	    close_db();	
	}	
	if ( run_test(IIPROTECT, 9) )
	{
	    SIprintf ("....iiprotect test 9..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 9  **/
	    /*************/
	    /** setup **/
	    /*
	    ** create a table, grant a permit on it, then overwrite 
	    ** IIPROTECT.proflags to make this look like a permit created using 
	    ** QUEL
	    */
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5814 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_pro9");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5815 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_pro9(i i)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5816 "duvetest.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on duve_pro9 to public");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5817 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select proflags from iiprotect where probjname='duve_pro9' and probjo\
wner=user");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5821 "duvetest.sc" */	/* host code */
	    val &= ~DBP_SQL_PERM;
/* # line 5823 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iiprotect set proflag\
s=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)
"where probjname='duve_pro9' and probjowner=user");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5827 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5838 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_pro9");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5840 "duvetest.sc" */	/* host code */
	    close_db();	
	}	
	if ( run_test(IIPROTECT, 10) )
	{
	    SIprintf ("....iiprotect test 10..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 10  **/
	    /*************/
	    /** setup **/
	    /*
	    ** create a table, grant a permit on it, then overwrite 
	    ** IIPROTECT.proopset to make this look like a permit conveys
	    ** multiple privileges
	    */
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5864 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_pro10");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5865 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_pro10(i i)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5866 "duvetest.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on duve_pro10 to public");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5867 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select proopset from iiprotect where probjname='duve_pro10' and probj\
owner=user");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5871 "duvetest.sc" */	/* host code */
	    val |= DB_REPLACE;
/* # line 5873 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iiprotect set proopse\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)
"where probjname='duve_pro10' and probjowner=user");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5877 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5888 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_pro10");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5890 "duvetest.sc" */	/* host code */
	    close_db();	
	}	
	if ( run_test(IIPROTECT, 11) )
	{
	    SIprintf ("....iiprotect test 11..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 11  **/
	    /*************/
	    /** setup **/
	    /*
	    ** create a table, grant a permit on it, then overwrite 
	    ** IIPROTECT.proflags to make this look like a permit was created 
	    ** prior to 6.5
	    */
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5914 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_pro11");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5915 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_pro11(i i)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5916 "duvetest.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on duve_pro11 to public");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5917 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select proflags from iiprotect where probjname='duve_pro11' and probj\
owner=user");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5921 "duvetest.sc" */	/* host code */
	    val &= ~DBP_65_PLUS_PERM;
/* # line 5923 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iiprotect set proflag\
s=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)
"where probjname='duve_pro11' and probjowner=user");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5927 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5938 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_pro11");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5940 "duvetest.sc" */	/* host code */
	    close_db();	
	}	
	if ( run_test(IIPROTECT, 12) )
	{
	    SIprintf ("....iiprotect test 12..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 12  **/
	    /*************/
	    /** setup **/
	    /*
	    ** as user SUPER, create a view over IIRELATION (on which everyone 
	    ** has SELECT WGO), grant SELECT on that view, then overwrite 
	    ** prograntor to make it look like as if the permit has been granted
	    ** by a user who does not own the view and does not possess
	    ** GRANT OPTION FOR privilege being conveyed by the permit (i.e.
	    ** SELECT)
	    */
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 5967 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5968 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session authorization su\
per");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5969 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view duve_pro12 as select * from iirelation");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5970 "duvetest.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on duve_pro12 to public");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5972 "duvetest.sc" */	/* host code */
	    /*
	    ** remember query text id for both the view and the permit since by
	    ** the time verifydb is done with them, the IIQRYTEXT tuples will be
	    ** the only thing left of them
	    */
/* # line 5977 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relqid1, relqid2 from iirelation where relid='duve_pro12' and \
relowner=user");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5980 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select proqryid1, proqryid2 from iiprotect where probjname='duve_pro1\
2' and probjowner=user");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&qryid1);
      IIgetdomio((short *)0,1,30,4,&qryid2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5984 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprotect set prograntor='$ingres' where probjname='duve_pro12\
' and probjowner=user");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 5988 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6000 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where(txtid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)")or(txtid1=");
    IIputdomio((short *)0,1,30,4,&qryid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qryid2);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6003 "duvetest.sc" */	/* host code */
	    close_db();	
	}	
    } while (0);  /* control loop */
}
/*
** ck_iiqrytext - 
**
** Description:
**
**  This routine tests the verifydb iiqrytext catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	11-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
*/
ck_iiqrytext()
{
  i4 id1, id2;
  i4 id1_a, id2_a;
  i4 id1_b, id2_b;
  i4 id1_c, id2_c;
  i4 id1_d, id2_d;
  i4 id1_e, id2_e;
  i4 qry1, qry2;
  i4 qry1_a, qry2_a;
  i4 qry1_b, qry2_b;
  i4 qry1_c, qry2_c;
  i4 qry1_d, qry2_d;
  i4 qry1_e, qry2_e;
  i4 val, val2;
    SIprintf ("..Testing verifydb checks/repairs for catalog iiqrytext..\n");
    SIflush(stdout);
    STcopy("iiqrytext",test_id);
    do
    {
	if ( run_test(IIQRYTEXT, 1) )
	{
	    SIprintf ("....iiqrytext test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6062 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiqrytext values(0, 0, 17, 0, 0, 'dummy entry')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6063 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6073 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=0 and txtid2=0");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6074 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIQRYTEXT, 3) || run_test(IIQRYTEXT, 4) ||
	     run_test(IIQRYTEXT, 5) || run_test(IIQRYTEXT, 7) ||
	     run_test(IIQRYTEXT, 8) || run_test(IIQRYTEXT, 9) )
	{
	    SIprintf ("....iiqrytext tests 3,4,5,7,8,9..\n");
	    SIflush(stdout);
	    /********************/
	    /** tests 3-5, 7-9 **/
	    /********************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6092 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_qrytab(a i4, b i4, c i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6093 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view duve_qryview_a as select a, b from duve_qrytab");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6094 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view duve_qryview_b as select a, b from duve_qrytab");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6095 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view duve_qryview_c as select a, b from duve_qrytab");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6096 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view duve_qryview_d as select a, b from duve_qrytab");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6097 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view duve_qryview_e as select a, b from duve_qrytab");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6098 "duvetest.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on duve_qrytab to '$ingres'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6100 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'duve_qryview_a'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1_a);
      IIgetdomio((short *)0,1,30,4,&id2_a);
      IIgetdomio((short *)0,1,30,4,&qry1_a);
      IIgetdomio((short *)0,1,30,4,&qry2_a);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6104 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'duve_qryview_b'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1_b);
      IIgetdomio((short *)0,1,30,4,&id2_b);
      IIgetdomio((short *)0,1,30,4,&qry1_b);
      IIgetdomio((short *)0,1,30,4,&qry2_b);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6108 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'duve_qryview_c'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1_c);
      IIgetdomio((short *)0,1,30,4,&id2_c);
      IIgetdomio((short *)0,1,30,4,&qry1_c);
      IIgetdomio((short *)0,1,30,4,&qry2_c);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6112 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'duve_qryview_d'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1_d);
      IIgetdomio((short *)0,1,30,4,&id2_d);
      IIgetdomio((short *)0,1,30,4,&qry1_d);
      IIgetdomio((short *)0,1,30,4,&qry2_d);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6116 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'duve_qryview_e'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1_e);
      IIgetdomio((short *)0,1,30,4,&id2_e);
      IIgetdomio((short *)0,1,30,4,&qry1_e);
      IIgetdomio((short *)0,1,30,4,&qry2_e);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6120 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_qrytab'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6122 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select proqryid1, proqryid2 from iiprotect where protabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and protabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&qry1);
      IIgetdomio((short *)0,1,30,4,&qry2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6124 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiqrytext set mode=19 where txtid1=");
    IIputdomio((short *)0,1,30,4,&qry1_a);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qry2_a);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6126 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiqrytext set mode=20 where txtid1=");
    IIputdomio((short *)0,1,30,4,&qry1_b);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qry2_b);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6128 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiqrytext set mode=33 where txtid1=");
    IIputdomio((short *)0,1,30,4,&qry1_c);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qry2_c);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6130 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiqrytext set mode=87 where txtid1=");
    IIputdomio((short *)0,1,30,4,&qry1_d);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qry2_d);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6132 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiqrytext set mode=110 where txtid1=");
    IIputdomio((short *)0,1,30,4,&qry1_e);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qry2_e);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6134 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiqrytext set mode=17 where txtid1=");
    IIputdomio((short *)0,1,30,4,&qry1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qry2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6136 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6146 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiqrytext set mode=17 where txtid1=");
    IIputdomio((short *)0,1,30,4,&qry1_a);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qry2_a);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6148 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiqrytext set mode=17 where txtid1=");
    IIputdomio((short *)0,1,30,4,&qry1_b);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qry2_b);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6150 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiqrytext set mode=17 where txtid1=");
    IIputdomio((short *)0,1,30,4,&qry1_c);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qry2_c);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6152 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiqrytext set mode=17 where txtid1=");
    IIputdomio((short *)0,1,30,4,&qry1_d);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qry2_d);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6154 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiqrytext set mode=17 where txtid1=");
    IIputdomio((short *)0,1,30,4,&qry1_e);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qry2_e);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6156 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiqrytext set mode=19 where txtid1=");
    IIputdomio((short *)0,1,30,4,&qry1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qry2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6158 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop view duve_qryview_a");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6159 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop view duve_qryview_b");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6160 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop view duve_qryview_c");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6161 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop view duve_qryview_d");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6162 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop view duve_qryview_e");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6163 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_qrytab");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6164 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIQRYTEXT, 2) )
	{
	    SIprintf ("....iiqrytext test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6180 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_qrytab2(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6181 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view duve_qryview as select * from duve_qrytab2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6182 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'duve_qryview'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&qry1);
      IIgetdomio((short *)0,1,30,4,&qry2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6185 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiqrytext set mode=11 where txtid1=");
    IIputdomio((short *)0,1,30,4,&qry1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qry2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6187 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6197 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiqrytext set mode=17 where txtid1=");
    IIputdomio((short *)0,1,30,4,&qry1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qry2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6199 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop view duve_qryview");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6200 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_qrytab2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6201 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIQRYTEXT, 6) )
	{
	    SIprintf ("....iiqrytext test 6..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 6  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6217 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_qrytab6(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6218 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view duve_qryview as select * from duve_qrytab6");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6219 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'duve_qryview'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&qry1);
      IIgetdomio((short *)0,1,30,4,&qry2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6222 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiqrytext set seq=1 where txtid1=");
    IIputdomio((short *)0,1,30,4,&qry1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qry2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6224 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6234 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiqrytext set seq=0 where txtid1=");
    IIputdomio((short *)0,1,30,4,&qry1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qry2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6236 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop view duve_qryview");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6237 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_qrytab6");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6238 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iirelation - 
**
** Description:
**
**  This routine tests the verifydb iirelation catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	09-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
**	4-aug-93 (stephenb)
**	    added test #29 to test that a row exists in iigw06_relation
**	    if we have a security audit gateway table.
**      23-nov-93 (anitap)
**          added test #28 to test that schema is created for orphaned
**          table.
**	28-jan-94 (andre)
**	    added tests 30 and 31
*/
ck_iirelation()
{
  i4 id1, id2;
  i4 val, val2;
  i4 txt1, txt2;
  char charval[DB_MAXNAME+1];
  i4 reltid;
    SIprintf ("..Testing verifydb checks/repairs for catalog iirelation..\n");
    SIflush(stdout);
    STcopy("iirelation",test_id);
    do
    {
#if 0
/* this test currently causes an AV in server due to a DMF bug ...
*/
	if ( run_test(IIRELATION,1) )
	{
	    SIprintf ("....iirelation test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_01);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6303 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirelation set reltid=0 where relid='iiintegrity'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6304 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iirelation(reltid, reltidx, relid)values(0, 0, 'duve_rel1\
')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6306 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6316 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where relid='duve_rel1'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6317 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirelation set reltid=23 where relid='iiintegrity'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6318 "duvetest.sc" */	/* host code */
	    close_db();
	}
#endif
	if ( run_test(IIRELATION,2) )
	{
	    SIprintf ("....iirelation test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2a **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_02);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6336 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel2a(a i4, b i4, c i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6337 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_rel2a'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6339 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiattribute set attid=4 where attid=3 and attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6341 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
 		break;
/* # line 6351 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiattribute set attid=3 where attid=4 and attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6353 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel2a");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6354 "duvetest.sc" */	/* host code */
	    close_db();
	    /*************/
	    /** test 2b **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6364 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel2b(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6365 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relwid from iirelation where relid='duve_rel2b'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6367 "duvetest.sc" */	/* host code */
	    val += 20;
/* # line 6368 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relwid\
=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel2b'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6369 "duvetest.sc" */	/* host code */
	    val -= 20;
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6380 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relwid\
=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel2b'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6381 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel2b");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6382 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,3) )
	{
	    SIprintf ("....iirelation test 3..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 3  **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_03);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6399 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel3(a i4, b i4, c i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6400 "duvetest.sc" */	/* create index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create index i_duve_rel3 on duve_rel3(a, b)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6401 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirelation set relkeys=4 where relid='i_duve_rel3'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6403 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6413 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirelation set relkeys=3 where relid='i_duve_rel3'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6415 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop index i_duve_rel3");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6416 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel3");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6417 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,4) )
	{
	    SIprintf ("....iirelation test 4..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 4  **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_04);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6434 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_rel4(a i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6435 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relspec from iirelation where relid='duve_rel4'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6437 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirelation set relspec=500 where relid='duve_rel4'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6439 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6449 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relspe\
c=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel4'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6451 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel4");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6452 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,5) )
	{
	    SIprintf ("....iirelation test 5..\n");
	    SIflush(stdout);
	    /*********************/
	    /** tests 5a and 5b **/
	    /*********************/
	    /** setup **/
	    name_log(REL_CODE,ID_05);
	    open_db_as_user("$ingres");
	    if (test_status == TEST_ABORT)
		break;
/* # line 6469 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_5a1(a i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6470 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table iiduve_5a2(a i4\
)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6471 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='duve_5a1'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6473 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='iiduve_5a2'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6475 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='iiduve_5a2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6477 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val2);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_5a1'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6479 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db_as_user("$ingres");
	    if (test_status == TEST_ABORT)
		break;
/* # line 6489 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_5a1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6490 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop iiduve_5a2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6491 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,6) )
	{
	    SIprintf ("....iirelation test 6..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 6  **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_06);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6508 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel6(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6509 "duvetest.sc" */	/* create index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create index i_duve_rel6 on duve_rel6(a)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6510 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='duve_rel6'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6512 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat, reltid, reltidx from iirelation where relid='i_duve_r\
el6'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val2);
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6515 "duvetest.sc" */	/* host code */
	    val |= TCB_INDEX;
	    val2 &= ~TCB_INDEX;
/* # line 6517 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel6'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6519 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val2);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='i_duve_rel6'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6521 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    val &= ~TCB_INDEX;
	    val2 |= TCB_INDEX;
/* # line 6533 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel6'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6535 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val2);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='i_duve_rel6'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6537 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop index i_duve_rel6");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6538 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel6");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6539 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiindex where baseid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6540 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6541 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6542 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,7) )
	{
	    SIprintf ("....iirelation test 7..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 7  **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_07);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6559 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel7(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6560 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, relstat from iirelation where relid='duve_rel7'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6562 "duvetest.sc" */	/* host code */
	    id2 = id1+1;
	    val |= TCB_INDEX;
/* # line 6564 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"insert into iirelation selec\
t ");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)
", 'i_duve_rel7', relowner, relatts, relwid, relkeys, relspec, ");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)
", reltups, relpages, relprim, relmain, relsave, relstamp1, relstamp2,\
 relloc, relcmptlvl, relcreate, relqid1, relqid2, relmoddate, relidxco\
unt, relifill, reldfill, rellfill, relmin, relmax, relloccount, relgwi\
d, relgwother, relhigh_logkey, rellow_logkey, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"relfhdr, relallocation, relextend, relcomptype, relpgtype, relstat2, \
relfree from iirelation where relid='duve_rel7'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6572 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"insert into iiattribute sele\
ct ");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)
", attid, attxtra, attname, attoff, attfrmt, attfrml, attfrmp, attkdom\
, attflag, attdefid1, attdefid2 from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6576 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6586 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiindex where baseid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6587 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6588 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6589 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,8) )
	{
	    SIprintf ("....iirelation test 8..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 8  **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_08);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6606 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel8(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6607 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='duve_rel8'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6609 "duvetest.sc" */	/* host code */
	    val |= TCB_PRTUPS;
/* # line 6610 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel8'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6612 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6622 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel8");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6623 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,9) )
	{
	    SIprintf ("....iirelation test 9..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 9  **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_09);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6640 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel9(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6641 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='duve_rel9'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6643 "duvetest.sc" */	/* host code */
	    val |= TCB_INTEGS;
/* # line 6644 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel9'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6646 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6656 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel9");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6657 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,10) )
	{
	    SIprintf ("....iirelation test 10..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 10 **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_0A);
	    open_db_as_user("$ingres");
	    if (test_status == TEST_ABORT)
		break;
/* # line 6674 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table iiduve_rel10(a \
i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6675 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='iiduve_rel10'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6677 "duvetest.sc" */	/* host code */
	    val |= TCB_CONCUR;
/* # line 6678 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='iiduve_rel10'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6680 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    val &= ~TCB_CONCUR;
/* # line 6691 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='iiduve_rel10'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6693 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iiduve_rel10");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6694 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,11) )
	{
	    SIprintf ("....iirelation test 11..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 11 **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_0B);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6711 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='iidevices'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6713 "duvetest.sc" */	/* host code */
	    val &= ~TCB_CONCUR;
/* # line 6714 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='iidevices'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6716 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    val |= TCB_CONCUR;
/* # line 6727 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='iidevices'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6729 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,12) )
	{
	    SIprintf ("....iirelation test 12..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 12 **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_0C);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6746 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_rel12(a i4\
)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6747 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view v_duve_rel12 as select * from duve_rel12");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6748 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'v_duve_rel12'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&val);
      IIgetdomio((short *)0,1,30,4,&val2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6752 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6753 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6764 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6765 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6766 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&val2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6767 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where deid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and deid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6769 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6770 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel12");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6771 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,13) )
	{
	    SIprintf ("....iirelation test 13..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 13 **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_0D);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6788 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_rel13(a i4\
)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6789 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view v_duve_rel13 as select * from duve_rel13");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6790 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'v_duve_rel13'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&val);
      IIgetdomio((short *)0,1,30,4,&val2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6793 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where deid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and deid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6794 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6805 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6807 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6808 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6809 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&val2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6810 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where deid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and deid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6811 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel13");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6812 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,14) )
	{
	    SIprintf ("....iirelation test 14..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 14 **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_0E);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6829 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_rel14(a i4\
)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6830 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view v_duve_rel14 as select * from duve_rel14");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6831 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'v_duve_rel14'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&val);
      IIgetdomio((short *)0,1,30,4,&val2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6834 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&val2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6835 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6846 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6848 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6849 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6850 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&val2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6851 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where deid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and deid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6852 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel14");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6853 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,15) )
	{
	    SIprintf ("....iirelation test 15..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 15 **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_0F);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6870 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_rel15(a i4\
)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6871 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='duve_rel15'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6874 "duvetest.sc" */	/* host code */
	    /*
	    ** if we don't set the TCB_VGRANT_OK bit, test 31 will complain that
	    ** IIPRIV does not contain description of privileges on which this,
	    ** apparently not "always grantable" view, depends
	    */
	    val |= TCB_VIEW | TCB_VGRANT_OK;
/* # line 6881 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel15'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6883 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    val &= ~TCB_VIEW;
/* # line 6894 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel15'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6896 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel15");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6897 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,16) )
	{
	    SIprintf ("....iirelation test 16..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 16 **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_10);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6914 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel16(a i4, tidp i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6915 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, relstat from iirelation where relid='duve_rel16'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6917 "duvetest.sc" */	/* host code */
	    val |= TCB_INDEX;
	    id2 = id1 +1;
/* # line 6919 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"insert into iirelation selec\
t ");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)
", 'i_duve_rel16', relowner, relatts, relwid, relkeys, relspec, ");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)
", reltups, relpages, relprim, relmain, relsave, relstamp1, relstamp2,\
 relloc, relcmptlvl, relcreate, relqid1, relqid2, relmoddate, relidxco\
unt, relifill, reldfill, rellfill, relmin, relmax, relloccount, relgwi\
d, relgwother, relhigh_logkey, rellow_logkey, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"relfhdr, relallocation, relextend, relcomptype, relpgtype, relstat2, \
relfree from iirelation where relid='duve_rel16'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6927 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where relid='duve_rel16'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6928 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"insert into iiattribute sele\
ct ");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)
", attid, attxtra, attname, attoff, attfrmt, attfrml, attfrmp, attkdom\
, attflag, attdefid1, attdefid2 from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=0");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6934 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=0");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6935 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6945 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6946 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6947 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,17) )
	{
	    SIprintf ("....relation test 17..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 17 **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_11);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 6964 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel17(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6965 "duvetest.sc" */	/* create index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create index i_duve_rel17 on duve_rel17(a)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6966 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='duve_rel17'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6968 "duvetest.sc" */	/* host code */
	    val &= ~TCB_IDXD;
/* # line 6969 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel17'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6971 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'i_duve_rel17'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&txt1);
      IIgetdomio((short *)0,1,30,4,&txt2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6974 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    val |= TCB_IDXD;
/* # line 6985 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel17'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6987 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop index duve_rel17");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6988 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel17");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6989 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiindex where baseid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6990 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6991 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltidx=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6992 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6993 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&txt1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&txt2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 6994 "duvetest.sc" */	/* host code */
    	    close_db();
	}
	if ( run_test(IIRELATION,18) )
	{
	    SIprintf ("....iirelation test 18..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 18 **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_12);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7011 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel18a(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7012 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel18b(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7013 "duvetest.sc" */	/* create index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create index i_duve_rel18 on duve_rel18a(a)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7014 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relidxcount from iirelation where relid='duve_rel18a'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7016 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relidxcount, relstat from iirelation where relid='duve_rel18b'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val2);
      IIgetdomio((short *)0,1,30,4,&id1);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7018 "duvetest.sc" */	/* host code */
	    val++;
	    val2++;
	    id1 |= TCB_IDXD;
/* # line 7021 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirelation set relidxcount=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel18a'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7023 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirelation set relidxcount=");
    IIputdomio((short *)0,1,30,4,&val2);
    IIwritio(0,(short *)0,1,32,0,(char *)", relstat=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel18b'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7025 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    val--;
	    val2--;
	    id1 &= ~TCB_IDXD;
/* # line 7038 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirelation set relidxcount=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel18a'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7040 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirelation set relidxcount=");
    IIputdomio((short *)0,1,30,4,&val2);
    IIwritio(0,(short *)0,1,32,0,(char *)", relstat=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel18b'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7042 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop index i_duve_rel18");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7043 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel18a");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7044 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel18b");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7045 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,19) )
	{
	    SIprintf ("....iirelation test 19..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 19 **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_13);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7062 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel19(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7063 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='duve_rel19'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7065 "duvetest.sc" */	/* host code */
	    val |= TCB_ZOPTSTAT;
/* # line 7066 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel19'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7068 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7078 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel19");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7079 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,20) )
	{
	    SIprintf ("....iirelation test 20..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 20 **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_14);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7096 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel20(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7097 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, relstat, relloccount from iirelation where relid='duve\
_rel20'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&val);
      IIgetdomio((short *)0,1,30,4,&val2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7100 "duvetest.sc" */	/* host code */
	    val |= TCB_MULTIPLE_LOC;
	    val2++;
/* # line 7102 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)", relloccount=");
    IIputdomio((short *)0,1,30,4,&val2);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel20'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7104 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    val2--;
	    val &= ~TCB_MULTIPLE_LOC;
/* # line 7116 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)", relloccount=");
    IIputdomio((short *)0,1,30,4,&val2);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel20'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7118 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel20");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7119 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where reltid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7120 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7121 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,21) )
	{
	    SIprintf ("....iirelation test 21..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 21 **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_15);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7138 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_rel21(a i4\
)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7139 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relloc from iirelation where relid='duve_rel21'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,charval);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7141 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirelation set relloc='fake_location' where relid='duve_rel21'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7143 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7153 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relloc\
=");
    IIputdomio((short *)0,1,32,0,charval);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel21'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7155 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel21");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7156 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,22) )
	{
	    SIprintf ("....iirelation test 22..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 22 **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_16);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7173 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel22(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7174 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reldfill from iirelation where relid='duve_rel22'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7176 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirelation set reldfill=0 where relid='duve_rel22'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7178 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7188 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set reldfi\
ll=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel22'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7190 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel22");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7191 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,23) )
	{
	    SIprintf ("....iirelation test 23..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 23 **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_17);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7208 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel23(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7209 "duvetest.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"modify duve_rel23 to hash on\
 a");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7210 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reldfill from iirelation where relid='duve_rel23'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7212 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirelation set reldfill=0 where relid='duve_rel23'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7214 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7224 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set reldfi\
ll=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel23'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7226 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel23");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7227 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,24) )
	{
	    SIprintf ("....iirelation test 24..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 24 **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_18);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7244 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel24(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7245 "duvetest.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"modify duve_rel24 to isam on\
 a");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7246 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reldfill from iirelation where relid='duve_rel24'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7248 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirelation set reldfill=0 where relid='duve_rel24'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7250 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7260 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set reldfi\
ll=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel24'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7262 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel24");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7263 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRELATION,25) )
	{
	    SIprintf ("....iirelation test 25..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 25 **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_19);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7280 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel25(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7281 "duvetest.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"modify duve_rel25 to btree o\
n a");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7282 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reldfill from iirelation where relid='duve_rel25'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7284 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirelation set reldfill=0 where relid='duve_rel25'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7286 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7296 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set reldfi\
ll=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel25'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7298 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel25");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7299 "duvetest.sc" */	/* host code */
	    close_db();
	}
	    /*************/
	    /** test 26 **/
	    /*************/
		    /* this cannot safely be tested, so this test is skipped. */
	if ( run_test(IIRELATION,27) )
	{
	    SIprintf ("....iirelation test 27..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 27 **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_1B);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7322 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel27(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7323 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='duve_rel27'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7325 "duvetest.sc" */	/* host code */
	    val |= TCB_RULE;
/* # line 7326 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rel27'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7328 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7338 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel27");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7339 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if (run_test (IIRELATION, 28) )
        {
           SIprintf("....iirelation test 28..\n");
           SIflush(stdout);
           /*************/
           /** test 28 **/
           /*************/
           /* checks for schema for orphaned relation */
           /** setup   **/
           name_log(REL_CODE,ID_1C);
           open_db();
           if (test_status == TEST_ABORT)
               break;
/* # line 7358 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7359 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session authorization su\
per");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7360 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel28(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7361 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7362 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session authorization $ingres");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7363 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iischema where schema_name='super'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7364 "duvetest.sc" */	/* host code */
           close_db();
           /** run test **/
           report_vdb();
           run_vdb();
           /** clean up **/
           open_db();
           if (test_status == TEST_ABORT)
              break;
/* # line 7374 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7375 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session authorization su\
per");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7376 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel28");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7377 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7378 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session authorization $ingres");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7379 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iischema where schema_name='super'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7380 "duvetest.sc" */	/* host code */
           close_db();
           /* Orphaned views. Part A of test 1 */
           SIprintf("....iirelation test 28A..\n");
           SIflush(stdout);
           /**************/
           /** test 28A **/
           /**************/
           /** setup   **/
           open_db();
           if (test_status == TEST_ABORT)
               break;
/* # line 7395 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7396 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session authorization go\
d");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7397 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel28a(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7398 "duvetest.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant all on duve_rel28a to super");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7399 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7400 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session authorization su\
per");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7401 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view v_duve_rel28a as select * from god.duve_rel28a");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7403 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7404 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session authorization $ingres");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7405 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iischema where schema_name='super'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7406 "duvetest.sc" */	/* host code */
           close_db();
           /** run test **/
           report_vdb();
           run_vdb();
           /** clean up **/
           open_db();
           if (test_status == TEST_ABORT)
              break;
/* # line 7416 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7417 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session authorization go\
d");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7418 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rel28a");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7419 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7420 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session authorization su\
per");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7421 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop v_duve_rel28a");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7422 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7423 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session authorization $ingres");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7424 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iischema where schema_name='super'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7425 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iischema where schema_name='god'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7426 "duvetest.sc" */	/* host code */
           close_db();
        }
	if (run_test (IIRELATION, 29) )
	{
	    SIprintf("....iirelation test 29..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 29 **/
	    /*************/
	    /** setup **/
	    name_log(REL_CODE,ID_1D);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7443 "duvetest.sc" */	/* register table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register table verifydb_test(database char(24)not null)as import from\
 'current' with DBMS=SXA");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7445 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid from iirelation where relid='verifydb_test'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&reltid);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7447 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iigw06_relation where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7448 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7458 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iigw06_attribute where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7459 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7460 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7461 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if (run_test (IIRELATION, 30) )
	{
	    SIprintf("....iirelation test 30..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 30 **/
	    /*************/
	    /** setup **/
	    /*
	    ** create a view that depends on SELECT on IIRELATION, then set
	    ** TCB_VGRANT_OK in relstat to make it look "always grantable"
	    */
	    name_log(REL_CODE,ID_1E);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7485 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_rel30");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7486 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view duve_rel30 as select * from iirelation");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7488 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='duve_rel30' and relowner=\
user");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7492 "duvetest.sc" */	/* host code */
	    val |= TCB_VGRANT_OK;
/* # line 7494 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)
"where relid='duve_rel30' and relowner=user");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7498 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7509 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_rel30");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7511 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if (run_test (IIRELATION, 31) )
	{
	    SIprintf("....iirelation test 31..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 31 **/
	    /*************/
	    /** setup **/
	    /*
	    ** create a view dependent on its owner's base table (which makes 
	    ** the view "always grantable"), then reset TCB_VGRANT_OK bit in 
	    ** relstat
	    */
	    name_log(REL_CODE,ID_1F);
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7536 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_rel31_view1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7537 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_rel31_tbl1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7539 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rel31_tbl1(i i)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7540 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view duve_rel31_view1 as select * from duve_rel31_tbl1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7543 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat, relqid1, relqid2 from iirelation where relid='duve_re\
l31_view1' and relowner=user");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
      IIgetdomio((short *)0,1,30,4,&txt1);
      IIgetdomio((short *)0,1,30,4,&txt2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7547 "duvetest.sc" */	/* host code */
	    val &= ~TCB_VGRANT_OK;
/* # line 7549 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)
"where relid='duve_rel31_view1' and relowner=user");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7553 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    /* 
	    ** code in duvechk.sc will delete catalog info pertaining to 
	    ** duve_rel31_view1 except for the query text - we need to clean 
	    ** it up to avoid hearing about this tuple throughout the remaining
	    ** tests.
	    */
/* # line 7571 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&txt1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&txt2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7573 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop duve_rel31_tbl1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7575 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iirel_idx - 
**
** Description:
**
**  This routine tests the verifydb iirel_idx catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	09-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
*/
ck_iirel_idx()
{
  i4 id1;
  i4 id2;
    /** actually, at this time, it is not possible to update a secondary
    *** index, so we cannot really set up for this test.  The code is left
    *** here incase this ever changes.  At this point, this routine is a
    *** no-op and it is not possible to test this verifydb check/repair.
    ***/
    SIprintf ("..Testing verifydb checks/repairs for catalog iirel_idx..\n");
    SIflush(stdout);
    STcopy("iirel_idx",test_id);
    do
    {
	if ( run_test(IIREL_IDX, 1) )
	{
	    SIprintf ("....iirel_idx test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7629 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iirel_idx values('fake_relidx', 'fake_dba', 100)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7630 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7640 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirel_idx where relid='fake_relidx'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7641 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iirule - 
**
** Description:
**
**  This routine tests the verifydb iirule catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	16-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
**	13-jan-94 (anitap)
**  	    Added logic to test check #6 in ckrule().
**	02-feb-94 (anitap)
**	    Updated cleanup of test check #6.	
*/
ck_iirule()
{
  i4 id1, id2, id3;
  i4 val;
  i4 txt1, txt2;
  i4 dep_type;
    SIprintf ("..Testing verifydb checks/repairs for catalog iirule..\n");
    SIflush(stdout);
    STcopy("iirule",test_id);
    do
    {
	if ( run_test(IIRULE, 1) )
	{
	    SIprintf ("....iirule test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7696 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select max(reltid)from iirelation");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7697 "duvetest.sc" */	/* host code */
	    id1++;	
/* # line 7698 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iirule(rule_name, rule_owner, rule_type, rule_tabbase, ru\
le_tabidx)values('duve_rule1', '$ingres', 1, ");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7701 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7711 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirule where rule_name='duve_rule1'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7712 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRULE, 2) )
	{
	    SIprintf ("....iirule test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7728 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_rul2(a i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7729 "duvetest.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"p_duve_rul2");
    IIwritio(0,(short *)0,1,32,0,(char *)"create procedure p_duve_rul2\
 as");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                begin");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    message 'hi';");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                end");
    IIputctrl(0);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7733 "duvetest.sc" */	/* create rule */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create rule r_duve_rul2 after insert on duve_rul2 execute procedure p\
_duve_rul2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7735 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, relstat from iirelation where relid='duve_rul2'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7737 "duvetest.sc" */	/* host code */
	    val &= ~TCB_RULE;
/* # line 7738 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rul2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7739 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    val |= TCB_RULE;
/* # line 7750 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_rul2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7751 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rul2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7752 "duvetest.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"p_duve_rul2");
    IIwritio(0,(short *)0,1,32,0,(char *)"drop procedure p_duve_rul2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7753 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRULE, 3) )
	{
	    SIprintf ("....iirule test 3..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 3  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7769 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rul3(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7770 "duvetest.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"p_duve_rul3");
    IIwritio(0,(short *)0,1,32,0,(char *)"create procedure p_duve_rul3\
 as");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                begin");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    message 'hi';");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                end");
    IIputctrl(0);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7774 "duvetest.sc" */	/* create rule */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create rule r_duve_rul3 after update on duve_rul3 execute procedure p\
_duve_rul3");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7776 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_tabbase, rule_tabidx, rule_qryid1, rule_qryid2 from iirul\
e where rule_name='r_duve_rul3'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&txt1);
      IIgetdomio((short *)0,1,30,4,&txt2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7780 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&txt1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&txt2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7781 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7791 "duvetest.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"p_duve_rul3");
    IIwritio(0,(short *)0,1,32,0,(char *)"drop procedure p_duve_rul3");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7792 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rul3");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7794 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7795 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirule where rule_name='r_duve_rul3'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7796 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where inid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7797 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRULE, 4) )
	{
	    SIprintf ("....iirule test 4..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 4  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7813 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rul4(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7814 "duvetest.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"p_duve_rul4");
    IIwritio(0,(short *)0,1,32,0,(char *)"create procedure p_duve_rul4\
 as");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                begin");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    message 'hi';");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                end");
    IIputctrl(0);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7818 "duvetest.sc" */	/* create rule */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create rule r_duve_rul4 after update on duve_rul4 execute procedure p\
_duve_rul4");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7820 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbp_txtid1, dbp_txtid2 from iiprocedure where dbp_name='p_duve\
_rul4'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&txt1);
      IIgetdomio((short *)0,1,30,4,&txt2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7823 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiprocedure where dbp_name='p_duve_rul4'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7825 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&txt1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&txt2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7826 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_tabbase, rule_tabidx, rule_qryid1, rule_qryid2 from iirul\
e where rule_name='r_duve_rul4'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&txt1);
      IIgetdomio((short *)0,1,30,4,&txt2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7829 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7839 "duvetest.sc" */	/* drop rule */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop rule r_duve_rul4");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7840 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rul4");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7841 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirule where rule_name='r_duve_rul4'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7843 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&txt1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&txt2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7845 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7846 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IIRULE, 5) )
	{
	    SIprintf ("....iirule test 5..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 5  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7862 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rul5(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7863 "duvetest.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"p_duve_rul5");
    IIwritio(0,(short *)0,1,32,0,(char *)"create procedure p_duve_rul5\
 as");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                begin");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    message 'hi';");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                end");
    IIputctrl(0);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7867 "duvetest.sc" */	/* create rule */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create rule r_duve_rul5 after update on duve_rul5 execute procedure p\
_duve_rul5");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7869 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_tabbase, rule_tabidx, rule_qryid1, rule_qryid2 from iirul\
e where rule_name='r_duve_rul5'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&txt1);
      IIgetdomio((short *)0,1,30,4,&txt2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7872 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirule set rule_type=2 where rule_name='r_duve_rul5'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7874 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7884 "duvetest.sc" */	/* drop rule */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop rule r_duve_rul5");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7885 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirule where rule_name='r_duve_rul5'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7887 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&txt1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&txt2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7889 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7890 "duvetest.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"p_duve_rul5");
    IIwritio(0,(short *)0,1,32,0,(char *)"drop procedure p_duve_rul5");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7891 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rul5");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7892 "duvetest.sc" */	/* host code */
	    close_db();
	}
        if ( run_test(IIRULE, 6) )
        {
            SIprintf ("....iirule test 6..\n");
            SIflush(stdout);
            /*************/
            /** test 6  **/
            /*************/
	    dep_type = DB_RULE;	
            /** setup **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 7910 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_rul6(a i4 check(a is not null))");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7911 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid from iirelation where relid='duve_rul6'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7913 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_treeid1, rule_treeid2, rule_qryid1, rule_qryid2 from iiru\
le where rule_tabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&id3);
      IIgetdomio((short *)0,1,30,4,&txt1);
      IIgetdomio((short *)0,1,30,4,&txt2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7916 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where inid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dtype=");
    IIputdomio((short *)0,1,30,4,&dep_type);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7918 "duvetest.sc" */	/* host code */
            close_db();
            /** run test **/
            report_vdb();
            run_vdb();
            /** clean up **/
            open_db();
            if (test_status == TEST_ABORT)
                break;
/* # line 7928 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_rul6");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7930 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&id3);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7932 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&txt1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&txt2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7933 "duvetest.sc" */	/* host code */
            close_db();
        }
    } while (0);  /* control loop */
}
/*
** ck_iistar_cdbs - 
**
** Description:
**
**  This routine tests the verifydb iistar_cdbs catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	16-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
*/
ck_iistar_cdbs()
{
  i4 id1;
  i4 id2;
    SIprintf ("..Testing verifydb checks/repairs for catalog iistar_cdbs..\n");
    SIflush(stdout);
    STcopy("iistar_cdbs ",test_id);
    do
    {
	if ( run_test(IISTAR_CDBS, 1) )
	{
	    SIprintf ("....iistar_cdbs test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 7982 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_ficticious_db4', '$ingres', 'ii_d\
atabase', 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, 4\
4, 'ii_dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7985 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_ficticious_db4', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7987 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iistar_cdbs(cdb_name, cdb_owner, ddb_name, ddb_owner, cdb\
_id)values('duve_ficticious_db4', '$ingres', 'idontexist', '$ingres', \
44)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 7991 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8001 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_ficticious_db4'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8002 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_ficticious_db4'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8004 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iistar_cdbs where cdb_name='duve_ficticious_db4'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8005 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IISTAR_CDBS, 2) )
	{
	    SIprintf ("....iistar_cdbs test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8021 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_ficticious_db5', '$ingres', 'ii_d\
atabase', 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, 5\
5, 'ii_dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8024 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_ficticious_db5', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8026 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iistar_cdbs(ddb_name, ddb_owner, cdb_name, cdb_owner, cdb\
_id)values('duve_ficticious_db5', '$ingres', 'idontexist', '$ingres', \
55)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8030 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8040 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_ficticious_db5'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8041 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_ficticious_db5'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8043 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iistar_cdbs where ddb_name='duve_ficticious_db5'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8044 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IISTAR_CDBS, 3) )
	{
	    SIprintf ("....iistar_cdbs test 3..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 3  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8060 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_ficticious_db6', '$ingres', 'ii_d\
atabase', 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, 6\
6, 'ii_dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8063 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_ficticious_db6', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8065 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_ficticious_db7', '$ingres', 'ii_d\
atabase', 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, 7\
7, 'ii_dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8068 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_ficticious_db7', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8070 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iistar_cdbs(ddb_name, ddb_owner, cdb_name, cdb_owner, cdb\
_id)values('duve_ficticious_db7', 'duve_ficticious_dba', 'duve_fictici\
ous_db6', '$ingres', 66)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8074 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8084 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_ficticious_db6'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8085 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_ficticious_db6'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8086 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_ficticious_db7'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8087 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_ficticious_db7'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8089 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iistar_cdbs where ddb_name='duve_ficticious_db7'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8090 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IISTAR_CDBS, 4) )
	{
	    SIprintf ("....iistar_cdbs test 4..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 4  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8106 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_ficticious_db8', '$ingres', 'ii_d\
atabase', 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, 8\
8, 'ii_dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8109 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_ficticious_db8', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8111 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_ficticious_db9', '$ingres', 'ii_d\
atabase', 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, 9\
9, 'ii_dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8114 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_ficticious_db9', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8116 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iistar_cdbs(ddb_name, ddb_owner, cdb_name, cdb_owner, cdb\
_id)values('duve_ficticious_db9', '$ingres', 'duve_ficticious_db8', 'd\
uve_ficticious_dba', 88)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8120 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8130 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_ficticious_db8'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8131 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_ficticious_db8'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8132 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_ficticious_db9'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8133 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_ficticious_db9'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8135 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iistar_cdbs where ddb_name='duve_ficticious_db9'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8136 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IISTAR_CDBS, 5) )
	{
	    SIprintf ("....iistar_cdbs test 5..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 5  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8152 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_ficticious_db10', '$ingres', 'ii_\
database', 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, \
110, 'ii_dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8155 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_ficticious_db10', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8157 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidatabase values('duve_ficticious_db11', '$ingres', 'ii_\
database', 'ii_checkpoint', 'ii_journal', 'ii_work', 17, 0, '6.4', 0, \
111, 'ii_dump')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8160 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend values('ii_database', 'duve_ficticious_db11', 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8162 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iistar_cdbs(ddb_name, ddb_owner, cdb_name, cdb_owner, cdb\
_id)values('duve_ficticious_db11', '$ingres', 'duve_ficticious_db10', \
'$ingres', 66)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8166 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8176 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_ficticious_db10'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8177 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_ficticious_db10'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8178 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name='duve_ficticious_db11'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8179 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname='duve_ficticious_db11'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8181 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iistar_cdbs where ddb_name='duve_ficticious_db11'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8182 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iistatistics - 
**
** Description:
**
**  This routine tests the verifydb iistatistics catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	17-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
*/
ck_iistatistics()
{
  i4 id1;
  i4 id2;
  i4 val;
    SIprintf ("..Testing verifydb checks/repairs for catalog iistatistics..\n");
    SIflush(stdout);
    STcopy("iistatistics",test_id);
    do
    {
	if ( run_test(IISTATISTICS, 1) )
	{
	    SIprintf ("....iistatistics test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8232 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select max(reltid)from iirelation");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8233 "duvetest.sc" */	/* host code */
	    val++;
/* # line 8234 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iistatistics(stabbase)values(");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8235 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8245 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iistatistics where stabbase=");
    IIputdomio((short *)0,1,30,4,&val);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8246 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IISTATISTICS, 2) )
	{
	    SIprintf ("....iistatistics test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8262 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_stat2(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8263 "duvetest.sc" */	/* create index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create index i_duve_stat2 on duve_stat2(a)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8264 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relstat from iirelation where relid='i_duve_s\
tat2'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8266 "duvetest.sc" */	/* host code */
	    val |= TCB_ZOPTSTAT;
/* # line 8267 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='i_duve_stat2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8269 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iistatistics(stabbase, stabindex)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8271 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8281 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iistatistics where stabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8282 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop index i_duve_stat2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8283 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_stat2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8284 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IISTATISTICS, 3) )
	{
	    SIprintf ("....iistatistics test 3..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 3  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8300 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_stat3(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8301 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_stat3'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8303 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iistatistics(stabbase, stabindex, satno)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8305 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8315 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iistatistics where stabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8316 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_stat3");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8317 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IISTATISTICS, 4) )
	{
	    SIprintf ("....iistatistics test 4..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 4  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8333 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_stat4(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8334 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relstat from iirelation where relid='duve_sta\
t4'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8336 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iistatistics(stabbase, stabindex, satno)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", 3)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8338 "duvetest.sc" */	/* host code */
	    val |= TCB_ZOPTSTAT;
/* # line 8339 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_stat4'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8341 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8351 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iistatistics where stabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8352 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_stat4");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8353 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IISTATISTICS, 5) )
	{
	    SIprintf ("....iistatistics test 5..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 5  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8369 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_stat5(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8370 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relstat from iirelation where relid='duve_sta\
t5'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&val);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8373 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iistatistics(stabbase, stabindex, satno, snumcells)values\
(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1, 5)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8375 "duvetest.sc" */	/* host code */
	    val |= TCB_ZOPTSTAT;
/* # line 8376 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_stat5'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8378 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8388 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iistatistics where stabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8389 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_stat5");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8390 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iisynonym - 
**
** Description:
**
**  This routine tests the verifydb iisynonym catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	09-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
**      23-nov-93 (anitap)
**          added test 3 to check that schema is created for orphaned synonym.
*/
ck_iisynonym()
{
  i4 id1;
  i4 id2;
    SIprintf ("..Testing verifydb checks/repairs for catalog iisynonym..\n");
    SIflush(stdout);
    STcopy("iisynonym",test_id);
    do
    {
	if ( run_test(IISYNONYM, 1) )
	{
	    SIprintf ("....iisynonym test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8441 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_syn1(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8442 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_syn1'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8444 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iisynonym(synonym_name, synonym_owner, syntabbase, syntab\
idx, synid, synidx)values('duve_syn1', ");
    IIputdomio((short *)0,1,32,0,dba);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8447 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8458 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iisynonym where syntabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and syntabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8459 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_syn1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8460 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IISYNONYM, 2) )
	{
	    SIprintf ("....iisynonym test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8476 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_syn2(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8477 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_syn2'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8479 "duvetest.sc" */	/* host code */
	    id1++;
/* # line 8480 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iisynonym(synonym_name, synonym_owner, syntabbase, syntab\
idx, synid, synidx)values('duve_syn2', ");
    IIputdomio((short *)0,1,32,0,dba);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8483 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    id1--;
/* # line 8495 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iisynonym where syntabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and syntabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8496 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_syn2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8497 "duvetest.sc" */	/* host code */
	    close_db();
	}
        if ( run_test(IISYNONYM, 3) )
        {
            SIprintf ("....iisynonym test 3..\n");
            SIflush(stdout);
            /*************/
            /** test 3  **/
            /*************/
            /** setup **/
            open_db();
            if (test_status == TEST_ABORT)
               break;
/* # line 8513 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8514 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session authorization su\
per");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8515 "duvetest.sc" */	/* create synonym */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create synonym duve_syn3 for iirelation");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8516 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8517 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session authorization $ingres");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8518 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iischema where schema_name='super'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8519 "duvetest.sc" */	/* host code */
           close_db();
           /** run test **/
           report_vdb();
           run_vdb();
           /** clean up **/
           open_db();
           if (test_status == TEST_ABORT)
              break;
/* # line 8529 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8530 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session authorization su\
per");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8531 "duvetest.sc" */	/* drop synonym */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop synonym duve_syn3");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8532 "duvetest.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8533 "duvetest.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session authorization $ingres");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8534 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iischema where schema_name='super'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8535 "duvetest.sc" */	/* host code */
           close_db();
        }
    } while (0);  /* control loop */
}
/*
** ck_iitree - 
**
** Description:
**
**  This routine tests the verifydb iitree catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	09-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
**	10-nov-93 (anitap)
**	    Corrected ii dbdepends to iidbdepends and indi1 to inid1 and indid2
**	    to inid2 in test 5.
*/
ck_iitree()
{
  i4 id1, id2;
  i4 id1_a, id2_a;
  i4 id1_b, id2_b;
  i4 id1_c, id2_c;
  i4 val1, val2, val3, val4;
  i4 qid1_a, qid2_a, qid1_b, qid2_b, qid1_c, qid2_c, qid1_d, qid2_d;
  i4 qid1_e, qid2_e, rid1_e, rid2_e;
  i4 rid1_a, rid2_a, rid1_b, rid2_b, rid1_c, rid2_c, rid1_d, rid2_d;
    SIprintf ("..Testing verifydb checks/repairs for catalog iitree..\n");
    SIflush(stdout);
    STcopy("iitree",test_id);
    do
    {
	if ( run_test(IITREE, 1) )
	{
	    SIprintf ("....iitree test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8593 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select max(reltid)from iirelation");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8594 "duvetest.sc" */	/* host code */
	    id1++;
/* # line 8595 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"insert into iitree values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, 100, 101, 0, 17, 0, 'hi\
')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8596 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8606 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8607 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IITREE, 2) )
	{
	    SIprintf ("....iitree test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8623 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_tree2(a i4\
)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8624 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view duve_treeview2 as select * from duve_tree2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8625 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'duve_treeview2'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&id1_a);
      IIgetdomio((short *)0,1,30,4,&id2_a);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8628 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select treeid1, treeid2 from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1_b);
      IIgetdomio((short *)0,1,30,4,&id2_b);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8630 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iitree set treemode=11 where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8632 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8642 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iitree set treemode=17 where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8644 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop view duve_treeview2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8645 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_tree2");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8646 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&id1_b);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&id2_b);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8648 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id1_a);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id2_a);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8649 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where deid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and deid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8650 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IITREE, 3) )
	{
	    /*************/
	    /** test 3  **/
	    /*************/
	    /* cant test this because it's caught by the iirelation tests */
	}
	if ( run_test(IITREE, 4) )
	{
	    SIprintf ("....iitree test 4..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 4  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8675 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_tree4a(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8676 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_tree4b(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8677 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into duve_tree4b values(1, 1)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8678 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_tree4c(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8679 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into duve_tree4c values(1, 1)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8680 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_tree4d(a i4, b i4)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8681 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into duve_tree4d values(1, 1)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8682 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view v_duve_tree4a as select * from duve_tree4a");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8683 "duvetest.sc" */	/* create permit */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create permit select on duve_tree4b to '$ingres' where a>5");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8685 "duvetest.sc" */	/* create integrity */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create integrity on duve_tree4c is b<100");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8686 "duvetest.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"p_duve_tree4");
    IIwritio(0,(short *)0,1,32,0,(char *)"create procedure p_duve_tree\
4 as");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                    begin");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                            message 'hi';");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                    end");
    IIputctrl(0);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8690 "duvetest.sc" */	/* create rule */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create rule r_duve_tree4 after insert on duve_tree4d where old.a>new.\
a execute procedure p_duve_tree4");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8692 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'duve_tree4a'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&rid1_a);
      IIgetdomio((short *)0,1,30,4,&rid2_a);
      IIgetdomio((short *)0,1,30,4,&qid1_a);
      IIgetdomio((short *)0,1,30,4,&qid2_a);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8695 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'duve_tree4b'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&rid1_b);
      IIgetdomio((short *)0,1,30,4,&rid2_b);
      IIgetdomio((short *)0,1,30,4,&qid1_b);
      IIgetdomio((short *)0,1,30,4,&qid2_b);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8698 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'duve_tree4c'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&rid1_c);
      IIgetdomio((short *)0,1,30,4,&rid2_c);
      IIgetdomio((short *)0,1,30,4,&qid1_c);
      IIgetdomio((short *)0,1,30,4,&qid2_c);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8701 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'duve_tree4d'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&rid1_d);
      IIgetdomio((short *)0,1,30,4,&rid2_d);
      IIgetdomio((short *)0,1,30,4,&qid1_d);
      IIgetdomio((short *)0,1,30,4,&qid2_d);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8704 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='duve_tree4a'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val1);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8706 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='duve_tree4b'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8708 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='duve_tree4c'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val3);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8710 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relstat from iirelation where relid='duve_tree4d'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&val4);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8712 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relqid1, relqid2 from iirelation where relid='p_duve_tree4'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8714 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'v_duve_tree4a'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&rid1_e);
      IIgetdomio((short *)0,1,30,4,&rid2_e);
      IIgetdomio((short *)0,1,30,4,&qid1_e);
      IIgetdomio((short *)0,1,30,4,&qid2_e);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8717 "duvetest.sc" */	/* host code */
	    val1 &= ~TCB_VIEW;
	    val2 &= ~TCB_PRTUPS;
	    val3 &= ~TCB_INTEGS;
	    val4 &= ~TCB_RULE;
/* # line 8721 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val1);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_tree4a'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8723 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val2);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_tree4b'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8725 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val3);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_tree4c'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8727 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val4);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_tree4d'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8729 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    val1 |= TCB_VIEW;
	    val2 |= TCB_PRTUPS;
	    val3 |= TCB_INTEGS;
	    val4 |= TCB_RULE;
/* # line 8743 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val1);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_tree4a'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8745 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val2);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_tree4b'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8747 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val3);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_tree4c'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8749 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&val4);
    IIwritio(0,(short *)0,1,32,0,(char *)"where relid='duve_tree4d'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8751 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop view v_duve_tree4a");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8752 "duvetest.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"p_duve_tree");
    IIwritio(0,(short *)0,1,32,0,(char *)"drop procedure p_duve_tree");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8753 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_tree4a");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8754 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_tree4b");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8755 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_tree4c");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8756 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_tree4d");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8758 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&rid1_a);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&rid2_a);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8760 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&rid1_b);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&rid2_b);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8762 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&rid1_c);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&rid2_c);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8764 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&rid1_d);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&rid2_d);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8766 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&rid1_e);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&rid2_e);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8768 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qid1_a);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qid2_a);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8770 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qid1_b);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qid2_b);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8772 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qid1_c);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qid2_c);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8774 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qid1_d);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qid2_d);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8776 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&qid1_e);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&qid2_e);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8778 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&rid1_a);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&rid2_a);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8780 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&rid1_b);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&rid2_b);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8782 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&rid1_c);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&rid2_c);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8784 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&rid1_d);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&rid2_d);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8786 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&rid1_e);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&rid2_e);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8788 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&rid1_a);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&rid2_a);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8790 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&rid1_b);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&rid2_b);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8792 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&rid1_c);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&rid2_c);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8794 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&rid1_d);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&rid2_d);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8796 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&rid1_e);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&rid2_e);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8797 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where deid1=");
    IIputdomio((short *)0,1,30,4,&rid1_e);
    IIwritio(0,(short *)0,1,32,0,(char *)"and deid2=");
    IIputdomio((short *)0,1,30,4,&rid2_e);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8798 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IITREE, 5) )
	{
	    SIprintf ("....iitree test 5..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 5  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8814 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_tree5(a i4\
)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8815 "duvetest.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view duve_vtree5 as select * from duve_tree5");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8816 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx, relqid1, relqid2 from iirelation where relid=\
'duve_vtree5'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
      IIgetdomio((short *)0,1,30,4,&val1);
      IIgetdomio((short *)0,1,30,4,&val2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8819 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iitree set treeseq=1 where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8821 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8831 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iitree set treeseq=0 where treetabbase=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8833 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop view duve_vtree5");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8835 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8837 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8838 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treeid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8839 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where inid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8840 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_tree5");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8842 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&val1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&val2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8843 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IITREE, 6) )
	{
	    SIprintf ("....iitree test 6..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 6  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8859 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create table duve_tree6(a i4\
)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8860 "duvetest.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"duve_treeproc");
    IIwritio(0,(short *)0,1,32,0,(char *)"create procedure duve_treepr\
oc =");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                    begin");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                            message 'hi';");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                    end");
    IIputctrl(0);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8864 "duvetest.sc" */	/* create rule */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create rule duve_treerule after update on duve_tree6 where new.a>old.\
a execute procedure duve_treeproc");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8867 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reltid, reltidx from iirelation where relid='duve_tree6'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&rid1_a);
      IIgetdomio((short *)0,1,30,4,&rid2_a);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8869 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_qryid1, rule_qryid2 from iirule where rule_name='duve_tre\
erule' and rule_owner=");
    IIputdomio((short *)0,1,32,0,dba);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
      IIgetdomio((short *)0,1,30,4,&id2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8871 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirule where rule_name='duve_treerule' and rule_owner=");
    IIputdomio((short *)0,1,32,0,dba);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8873 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8883 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8884 "duvetest.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"duve_treeproc");
    IIwritio(0,(short *)0,1,32,0,(char *)"drop procedure duve_treeproc");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8885 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_tree6");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8887 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbdepends where inid1=");
    IIputdomio((short *)0,1,30,4,&rid1_a);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inid2=");
    IIputdomio((short *)0,1,30,4,&rid2_a);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8889 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&rid1_a);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&rid2_a);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8890 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IITREE, 7) )
	{
	    /*************/
	    /** test 7  **/
	    /*************/
	    /* not testable at this time because currently events do not cause
    	    ** tree tuples to be generated */
	}
    } while (0);  /* control loop */
}
/*
** ck_iiuser - 
**
** Description:
**
**  This routine tests the verifydb iiuser catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	16-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
**	15-Oct-93 (teresa)
**	    Remove test 1 since the verifydb functionality for test 1 has been
**	    commented out.  This was necessary because the Delimited Identifier
**          project changed the semantics for detecting valid user names and now
**          almost anything is a valid user name.
**	22-dec-93 (robf)
**          added test 4 (profile name)
*/
ck_iiuser()
{
  i4 id1;
  i4 id2;
    SIprintf ("..Testing verifydb checks/repairs for catalog iiuser..\n");
    SIflush(stdout);
    STcopy("iiuser",test_id);
    do
    {
#if 0
	if ( run_test(IIUSER, 1) )
	{
	    SIprintf ("....iiuser test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8957 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiuser(name, status)values('**bad**', 35345)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8958 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8968 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiuser where name='**bad**'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8969 "duvetest.sc" */	/* host code */
	    close_db();
	}   
#endif
	if ( run_test(IIUSER, 2) )
	{
	    SIprintf ("....iiuser test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8986 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiuser(name, status)values('duve_user2',  -1)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8987 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 8997 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiuser where name='duve_user2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 8998 "duvetest.sc" */	/* host code */
	    close_db();
	}   
	if ( run_test(IIUSER, 3) )
	{
	    SIprintf ("....iiuser test 3..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 3  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 9014 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiuser(name, status, default_group)values('duve_user3', 3\
5345, 'duve_nonexistent_default_group')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 9016 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 9026 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiuser where name='duve_user3'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 9027 "duvetest.sc" */	/* host code */
	    close_db();
	}   
	if ( run_test(IIUSER, 4) )
	{
	    SIprintf ("....iiuser test 4..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 4  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 9042 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiuser(name, status, profile)values('duve_user4', 35345, \
'duve_nonexistent_profile')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 9044 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 9054 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiuser where name='duve_user4'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 9055 "duvetest.sc" */	/* host code */
	    close_db();
	}   
    } while (0);  /* control loop */
}
/*
** ck_iiusergroup - 
**
** Description:
**
**  This routine tests the verifydb iiusergroup catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	16-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
*/
ck_iiusergroup()
{
  i4 id1;
  i4 id2;
    SIprintf ("..Testing verifydb checks/repairs for catalog iiusergroup..\n");
    SIflush(stdout);
    STcopy("iiusergroup",test_id);
    do
    {
	if ( run_test(IIUSERGROUP, 1) )
	{   
	    SIprintf ("....iiusergroup test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 9104 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiusergroup(groupid, groupmem)values('valid_group', '    \
                            ')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 9106 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiusergroup(groupid, groupmem)values('valid_group', 'inva\
lid_member')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 9108 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 9118 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiusergroup where groupid='valid_group'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 9119 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iiprofile - 
**
** Description:
**
**  This routine tests the verifydb iiprofile catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	22-dec-93 (robf)
**	    Initial creation.
*/
ck_iiprofile()
{
  i4 id1;
  i4 id2;
    SIprintf ("..Testing verifydb checks/repairs for catalog iiprofile..\n");
    SIflush(stdout);
    STcopy("iiprofile",test_id);
    do
    {
	if ( run_test(IIPROFILE, 1) )
	{
	    SIprintf ("....iiprofile test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 9166 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiprofile(name, status)values('**bad**', 35345)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 9167 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 9177 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiprofile where name='**bad**'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 9178 "duvetest.sc" */	/* host code */
	    close_db();
	}   
	if ( run_test(IIPROFILE, 2) )
	{
	    SIprintf ("....iiprofile test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 9194 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiprofile(name, status)values('duve_user2',  -1)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 9195 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 9205 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiprofile where name='duve_user2'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 9206 "duvetest.sc" */	/* host code */
	    close_db();
	}   
	if ( run_test(IIPROFILE, 3) )
	{
	    SIprintf ("....iiprofile test 3..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 3  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 9222 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiprofile where name=''");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 9223 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	}   
    } while (0);  /* control loop */
}
/*
** ck_iixdbdepends - 
**
** Description:
**
**  This routine tests the verifydb iixdbdependscatalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	16-jun-93 (teresa)
**	    Initial creation.
**	22-jun-93 (teresa)
**	    added logic to handle subtest parameter to each test.
*/
ck_iixdbdepends()
{
  i4 id1, id2;
    SIprintf ("..Testing verifydb checks/repairs for catalog iixdbdepends..\n");
    SIflush(stdout);
    STcopy("iixddbdepends",test_id);
    do
    {
	if ( run_test(IIXDBDEPENDS, 1) )
	{
	    SIprintf ("....iixdbdepends test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 9276 "duvetest.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select max(reltid)from iirelation");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id1);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 9277 "duvetest.sc" */	/* host code */
	    id1 ++;
	    id2 = id1+1;
/* # line 9279 "duvetest.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"insert into iixdbdepends val\
ues(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)", 17, 1, 100)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 9280 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 9290 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iixdbdepends where deid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 9291 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_suppress() - 
**
** Description:
**
**  This routine determines whether or not to supply a suppress message
**  number value for a given test.  The suppress value only works when a
**  specified test is selected.  Example:
**
**	Error message S_DU167D_NO_RULES indicates that at one time there were
**	rules defined on a table.  However, it is perfectly valid for this
**	bit to be set even if there are no rules currently defined.  In such
**	a case, verifydb reports that the bit is set and recommends clearing it.
**	If we are running a test suite and there are some tables in the user's
**	database which used to have rules and no longer do, it will generate
**	some diffs that we do not expect -- but these will not be significant
**	diffs.  However, if we are running iitree test #6 (or iirelation test
**	#27), then we purposely set up error conditions where this message is
**	expected.
**
**	So, the trick is to suppress S_DU167D_NO_RULES unless we are running
**	a test where we expect it in the diffs (iirelation #27 or iitree #6).
**
**	This is a little more complicated than it may seem because there are
**	different ways that the user can specify to run these specific tests.
**	The following conditions may trigger iitree test 6 to be run (and this
**	is how the suppress logic works in each case):
**	1.  The user has specified only iitree test 6 - argv[4] = 3 and
**	    argv[5] = 506.  In this case, we will specify to suppress
**	    all messages but S_DU167D_NO_RULES.  (This is the usual run_all or
**	    run_seq case.)
**	2.  The user has specified all iitree tests be run -- argv[4] = 3 and
**	    argv[5] = 500.  In this case, ALL messages will be suppressed
**	    for all iitree tests except test 6, which will suppress all messages
**	    except S_DU167D_NO_RULES.
**	3.  The user has specified all qrymod tests be run -- argv[4] = 3 and
**	    argv[5] = 0.  In this case, no messages will be suppressed.
**	4.  The user has specified that all tests be run -- argv[4] = 1 or
**	    argv[4] is not supplied.  In this case, no messages will be
**	    suppressed.
**
**	This description section accurately describes suppressed messages as of
**	22-dec-1993.  But, the way developers develop, this will undoubtedly get
**	out of date.  So, if you want to know the details of what is currently
**	being suppressed, refer to routine duve_suppress() in duvetalk.c.
**	There is a lookup table named suppress_list[], which contains the
**	official list of all suppressed messages.  The current list is:
**
**	Message ID:		    Test that generates that message:
**	------------------------    ---------------------------------
**	S_DU1611_NO_PROTECTS	    [CORE] iirelation test 8
**	S_DU1612_NO_INTEGS	    [CORE] iirelation test 9
**	S_DU1619_NO_VIEW	    [CORE] iirelation test 13
**	S_DU161E_NO_STATISTICS	    [CORE] iirelation test 19
**	S_DU165E_BAD_DB		    [IIDBDB] iidatabase test 9
**	S_DU167A_WRONG_CDB_ID	    [IIDBDB] iistar_cdbs test 5
**	S_DU167D_NO_RULES	    [CORE] iirelation test 27 AND
**				    [QRYMOD] iitree test 3
**
**  Inputs:
**	Catalog			A numeric code specifying which catalog is
**				being operated on.
**	testid			a numeric code indaciting which test is being
**				run
**  Outputs:
**	global variable: suppress - suppress instruction:
**					DO_NOT_SUPPESS	- suppress no msgs
**				    or
**					SUPPRESS_ALL	- suppress all msgs
**				    or
**					specific value of msg to suppress
**
**  History:
**	22-dec-93 (teresa)
**	    Initial creation
**	27-jan-94 (andre)
**	    added cases for IIXPROTECT, IIPRIV, IIXPRIV, IIXEVENT, and 
*8	    IIXPROCEDURE
*/
ck_suppress(catalog, testid)
i4	    catalog;
i4	    testid;
{
    i4	retstat=FALSE;
    i4	map = BIT2 | BIT3 | BIT4 | BIT5 | BIT6 ;
    /* don't suppress any messages if running all tests or if debug mode
    ** has been selected
    */
    if (
	(testmap & BIT10)   /* debug */
       ||
	(testmap & BIT20)   /* debug */
       ||
       /* run all tests */
	( (testmap & map ) == map )
       )
	{
	    /* indicate we are not to suppress messages */
	    suppress = DO_NOT_SUPPESS;
	    return;
	}
    if (subtest == 0)
    {
	/* we have been asked to run all catalogs in a category, so do NOT
	** suppress any messages
	*/
	suppress = DO_NOT_SUPPESS;
	return;
    }
    /* if we get here, we are either running one specific test or we are
    ** running all tests for a given catalog.  In either case, we suppress
    ** all messages unless there is a specific test that requires a specific
    ** message NOT be suppressed.
    */    
    switch (catalog)
    {
	case IIATTRIBUTE:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IICDBID_IDX:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
    	case IIDATABASE:
	    if (testid == 9)
		suppress = S_DU165E_BAD_DB;
	    else
		/* there are no special messages to avoid suppressing, so
		** suppress all messages
		*/
		suppress = SUPPRESS_ALL;
	    break;
	case IIDBDEPENDS:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIDBID_IDX:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIDBMS_COMMENT:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIDBPRIV:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIDEVICES:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIDEFAULT:
	    /* there are no special messages to avoid suppressing, so suppress
            ** all messages
            */
            suppress = SUPPRESS_ALL;
            break;
	case IIEVENT:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIEXTEND:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIGW06_ATTRIBUTE:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIGW06_RELATION:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIHISTOGRAM:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIINDEX:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIINTEGRITY:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIKEY:
	    /* there are no special messages to avoid suppressing, so suppress
            ** all messages
            */
            suppress = SUPPRESS_ALL;
            break;	
	case IILOCATIONS:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIPROCEDURE:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIPROTECT:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIQRYTEXT:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIRELATION:
	    if (testid == 8)
		suppress = S_DU1611_NO_PROTECTS;
	    else if (testid == 9)
		suppress = S_DU1612_NO_INTEGS;
	    else if (testid == 13)
		suppress = S_DU1619_NO_VIEW;
	    else if (testid == 19)
		suppress = S_DU161E_NO_STATISTICS;
	    else if (testid == 27)
		suppress = S_DU167D_NO_RULES;
	    else
		/* there are no special messages to avoid suppressing, so
		** suppress all messages
		*/
		suppress = SUPPRESS_ALL;
	    break;
	case IIREL_IDX:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIRULE:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IISTAR_CDBS:
	    if (testid == 5)
		suppress = S_DU167A_WRONG_CDB_ID;
	    else
		/* there are no special messages to avoid suppressing, so
		** suppress all messages
		*/
		suppress = SUPPRESS_ALL;
	    break;
	case IISTATISTICS:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IISYNONYM:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IITREE:
	    if (testid == 6)
		suppress = S_DU167D_NO_RULES;
	    else
		/* there are no special messages to avoid suppressing, so
		** suppress all messages
		*/
		suppress = SUPPRESS_ALL;
	    break;
	case IIUSER:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIUSERGROUP:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIXDBDEPENDS:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIPRIV:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIXPRIV:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIXEVENT:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	case IIXPROCEDURE:
	    /* there are no special messages to avoid suppressing, so suppress
	    ** all messages
	    */
	    suppress = SUPPRESS_ALL;
	    break;
	default:
	    /* unexpected case, so don't suppress any messages. */
	    suppress = DO_NOT_SUPPESS;
	    break;
    }
}
/*
** core_cats() - this routine tests the verifydb core catalog checks/repairs
**
** Description:
**
** This routine tests the verifydb core catalog checks/repairs.  This includes
** checks/repairs on the following catalogs:
**	iiattribute
**	iidevices
**	iiindex
**	iirelation
**	iirel_idx
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**	    is set by subordinate routines.
**	Outputs to log files:   ii_config:iivdbc01.DBA to
**				ii_config:iivdbc04.DBA
**	    and
**				ii_config:iivdbr00.DBA to 
**				ii_config:iivdbr0f.DBA
**	    where DBA is the value of the DBA name input parameter
**  History:
**	08-jun-93 (teresa)
**	    Initial creation.
*/
core_cats()
{
    SIprintf("Testing Verifydb Check/Repairs On Core Catalogs\n");
    SIflush(stdout);
    if (test_status != TEST_ABORT)
    {
	name_log(CORE_CODE,ID_01);
	ck_iiattribute();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(CORE_CODE,ID_02);
	ck_iidevices();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(CORE_CODE,ID_03);
	ck_iiindex();
    }
    if (test_status != TEST_ABORT)
    {
	/* iirelation outputs several log files since there are so
	** many check/repair sets in it.  So it gets its own alpha character: r
	*/
	name_log(REL_CODE,ID_00);
	ck_iirelation();
    }
/* currently the server does not allow us to update secondry indees, so we
** cannot test this.
    if (test_status != TEST_ABORT)
    {
	name_log(CORE_CODE,ID_04);
	ck_iirel_idx();
    }
*/
}
/*
** dbdb_cats() - this routine tests the verifydb iidbdb-only catalog
**	       checks/repairs
**
** Description:
**
** This routine tests the verifydb iidbdb-only catalog checks/repairs.
** This includes checks/repairs on the following catalogs:
**	iicdbid_idx
**	iidatabase
**	iidbaccess
**	iidbid_idx
**	iidbprv
**	iiextend
**	iilocations
**	iistar_cdbs
**	iiuser
**	iiusergroup
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**	    is set by subordinate routines.
**	Outputs to log files:   ii_config:iivdbd01.DBA to
**				ii_config:iivdbd0A.DBA
**			     and iidatabase tests in
**				ii_config:iivdba01.DBA to
**				ii_config:iivdba0F.DBA
**	    where DBA is the value of the DBA name input parameter
**	    and ii_config:iivdbd02.DBA is currently not used.
**		
**  History:
**	16-jun-93 (teresa)
**	    Initial creation.
*/
dbdb_cats()
{
    SIprintf("Testing Verifydb Check/Repairs On iidbdb-Only Catalogs\n");
    SIflush(stdout);
/* we currently cannot test this because the server does not support direct
** updates on secondary indexes 
    if (test_status != TEST_ABORT)
    {
	name_log(DBDB_CODE,ID_01);
	ck_iicdbid_idx();
    }
*/
    /* catalog iidatabase has so many tests that it gets its own code */
    if (test_status != TEST_ABORT)
    {
	name_log(DTBASE_CODE,ID_00);
	ck_iidatabase();
    }
/* This catalog does not exist in release 6.5, so no sense coding a test for it
** for release 6.5 (now called Series 2000, I1/2). 
    if (test_status != TEST_ABORT)
    {
	name_log(DBDB_CODE,ID_03);
	ck_iidbaccess();
    }
*/
/* we currently cannot test this because the server does not support direct
** updates on secondary indexes 
    if (test_status != TEST_ABORT)
    {
	name_log(DBDB_CODE,ID_04);
	ck_iidbid_idx();
    }
*/
    if (test_status != TEST_ABORT)
    {
	name_log(DBDB_CODE,ID_05);
	ck_iidbpriv();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(DBDB_CODE,ID_06);
	ck_iiextend();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(DBDB_CODE,ID_07);
	ck_iilocations();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(DBDB_CODE,ID_08);
	ck_iistar_cdbs();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(DBDB_CODE,ID_09);
	ck_iiuser();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(DBDB_CODE,ID_0A);
	ck_iiusergroup();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(DBDB_CODE,ID_0B);
	ck_iiprofile();
    }
}
/*
** debug_me() - A debug work area
**
** Description:
**
** This routine is a place to cut-n-paste portions of a routine that you
** are trying to debug or test.  You can put whatever you like in here, from
** a call to a single ck_<catalog_name>()  routine to whatever you need to
** debug.
**
** You can skip all the verifydb tests and jump right to this rouitne by
** entering a test instruction of 10.  Example: use database "mydb" owned
** by "joe_programmer", with a valid installation user name of "samspade";
** just skip to this debug section and skip routines to check core catalogs,
** qrymod catalogs, etc:
**
**	test_vdb mydb joe_programmer samspade 10
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**	    is set by subordinate routines.
**  History:
**	Does not apply as this is a dynamic work area.
*/
debug_me()
{
	ck_iiattribute();
}
/*
** debug_me2() - A debug work area
**
** Description:
**
** This routine is a place to cut-n-paste portions of a routine that you
** are trying to debug or test.  You can put whatever you like in here, from
** a call to a single ck_<catalog_name>()  routine to whatever you need to
** debug.
**
** You can skip all the verifydb tests and jump right to this routne by
** entering a test instruction of 20.  Example: use database "mydb" owned
** by "joe_programmer", with a valid installation user name of "samspade";
** just skip to this debug section and skip routines to check core catalogs,
** qrymod catalogs, etc:
**
**	test_vdb mydb joe_programmer samspade 20
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**	    is set by subordinate routines.
**  History:
**	Does not apply as this is a dynamic work area.
*/
debug_me2()
{
}
/*
** name_log() - build name for verifydb output log file
**
** Description:
**
**  This routine builds a name for the verifydb log file and places that name
**  into global variable 'logfile'.  These are the nameing conventions:
**
**	filename.ext where:
**
**	1.  All file names do not exceed 8 characters.
**	2.  All file names start with "iivdb" (BASE_LOG)
**	3.  The 6th letter of each file name is a class code, as follows:
**		Core Catalogs (except iirelation)   'C'
**		iirelation core catalog		    'R'
**		iidbdb-only catalogs		    'D'
**		query processing catalogs	    'P'
**		query modification catalogs	    'M'
**		optimizer/statictics catalogs	    'S'
**	4.  The 7th and 8th characters of the filename are a numberic code
**	    ranging from 00 to 2F (defined via constants ID_00 to ID_2F).
**	5.  The extenstion is the dba name (input parameter ARG_V[2])
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: logfile
**  History:
**	17-jun-93 (teresa)
**	    Initial Creation.
*/
name_log(class_code,id)
char	*class_code;
char	*id;
{
    STpolycat( 5,
		BASE_LOG, class_code, id, EXT_POS, dba,
		logfile
	     );
}
/*
** open_db -- open a database
**
** Description:
**	connects to a database as the DBA with +Y priv.
** Inputs:
**	None.
** Outputs;
**	None.
** History:
**	08-jun-93 (teresa)
**	    initial creation.
**      11-jan-94 (anitap)
**          Corrected typo: conenct -> connect in SIprintf error message text.
**	05-mar-94 (teresa)
**	    "+Y" -> '+Y' because of change to FE that no longer accepts "+Y"
*/
open_db()
{
    if (test_status == TEST_ABORT)
	return;
    else
    {
#ifdef DEBUG
	SIprintf ("connecting to database %s\n as user %s\n", dbname, dba);
#endif
/* # line 9954 "duvetest.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqUser(dba);
    IIsqConnect(0,dbname,(char *)"+Y",(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 9955 "duvetest.sc" */	/* host code */
	/* if connection fails, then quit */
	if (sqlca.sqlcode < 0)
	{
	    test_status = TEST_ABORT;
	    SIprintf (" FATAL ERROR: unable to connect to database.\n");
	}
    }
}
/*
** open_A9db -- open a database with +Y and -A9
**
** Description:
**	connects to a database as the DBA with +Y and -A9 flags.
** Inputs:
**	None.
** Outputs;
**	None.
** History:
**	31-jan-94 (andre)
**	    written
**	05-mar-94 (teresa)
**	    "+Y" -> '+Y' because of change to FE that no longer accepts "+Y"
**	    Also change "-A9" to '-A9'
*/
open_A9db()
{
    if (test_status == TEST_ABORT)
	return;
    else
    {
#ifdef DEBUG
	SIprintf ("connecting to database %s\n as user %s\n", dbname, dba);
#endif
/* # line 9990 "duvetest.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqUser(dba);
    IIsqConnect(0,dbname,(char *)"+Y",(char *)"-A9",(char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 9991 "duvetest.sc" */	/* host code */
	/* if connection fails, then quit */
	if (sqlca.sqlcode < 0)
	{
	    test_status = TEST_ABORT;
	    SIprintf (" FATAL ERROR: unable to conenct to database.\n");
	}
    }
}
/*
** open_dbdb -- open iidbdb database
**
** Description:
**	connects to iidbdb as '$ingres' with +Y priv.
** Inputs:
**	None.
** Outputs;
**	None.
** History:
**	19-jun-93 (teresa)
**	    initial creation.
**      11-jan-94 (anitap)
**          Corrected typo: conenct -> connect in SIprintf error message text.
**	05-mar-94 (teresa)
**	    "+Y" -> '+Y' because of change to FE that no longer accepts "+Y"
*/
open_dbdb()
{
    if (test_status == TEST_ABORT)
	return;
    else
    {
#ifdef DEBUG
	SIprintf ("connecting to database iidbdb as user $ingres\n");
#endif
/* # line 10027 "duvetest.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqUser((char *)"$ingres");
    IIsqConnect(0,(char *)"iidbdb",(char *)"+Y",(char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10028 "duvetest.sc" */	/* host code */
	/* if connection fails, then quit */
	if (sqlca.sqlcode < 0)
	{
	    test_status = TEST_ABORT;
	    SIprintf (" FATAL ERROR: unable to connect to iidbdb database.\n");
	}
    }
}
/*
** open_db_as_user -- open a database as the specified user
**
** Description:
**	connects to a database as the specified user with +Y priv.
** Inputs:
**	None.
** Outputs;
**	None.
** History:
**	09-jun-93 (teresa)
**	    initial creation.
**      11-jan-94 (anitap)
**          Corrected typo: conenct -> connect in SIprintf error message text.
**	05-mar-94 (teresa)
**	    "+Y" -> '+Y' because of change to FE that no longer accepts "+Y"
*/
open_db_as_user(user)
char	*user;
{
  char uname[DB_MAXNAME+1];
    STcopy( user, uname);
    if (test_status == TEST_ABORT)
	return;
    else
    {
#ifdef DEBUG
	SIprintf ("connecting to database %s\n as user %s\n", dbname, dba);
#endif
/* # line 10070 "duvetest.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqUser(uname);
    IIsqConnect(0,dbname,(char *)"+Y",(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10071 "duvetest.sc" */	/* host code */
	/* if connection fails, then quit */
	if (sqlca.sqlcode < 0)
	{
	    test_status = TEST_ABORT;
	    SIprintf (" FATAL ERROR: unable to connect to database.\n");
	}
    }
}
/*
** opt_cats() - this routine tests the verifydb optimizer catalog checks/repairs
**
** Description:
**
** This routine tests the verifydb optimizer catalog checks/repairs.
** This includes checks/repairs on the following catalogs:
**	iistatistics
**	iihistogram
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**	    is set by subordinate routines.
**	Outputs to log files:   ii_config:iivdbs01.DBA to
**				ii_config:iivdbs02.DBA
**	    where DBA is the value of the DBA name input parameter
**  History:
**	16-jun-93 (teresa)
**	    Initial creation.
*/
opt_cats()
{
    SIprintf("Testing Verifydb Check/Repairs On Optimizer Catalogs\n");
    SIflush(stdout);
    if (test_status != TEST_ABORT)
    {
	name_log(STAT_CODE,ID_01);
    	ck_iistatistics();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(STAT_CODE,ID_02);
	ck_iihistogram();
    }
}
/*
** printargs -- print arguments for verifydb's test executive
**
** Description:
**	If the user does not give all required parameters, this routine
**	identifies what they should be 
** Inputs:
**	None.
** Outputs;
**	None.
** History:
**	08-jun-93 (teresa)
**	    initial creation.
*/
printargs()
{
    SIprintf("\n VERIFYDB TEST SUITE.  Arguments:\n");
    SIprintf("	argv[0] = program pathname (default of operating system)\n");
    SIprintf("	argv[1] = database name\n");
    SIprintf("	argv[2] = dba name\n");
    SIprintf("	argv[3] = user name\n");
    SIprintf("	argv[4] = test inst:\n");
    SIprintf("		  NULL - DEFAULTS to all verifydb tests\n");
    SIprintf("	          '1'  - all verifydb tests\n");
    SIprintf("		  '2'  - core catalog tests only\n");
    SIprintf("		  '3'  - qrymod catalog tests only\n");
    SIprintf("		  '4'  - qp catalog tests only\n");
    SIprintf("		  '5'  - optimizer catalog tests only\n");
    SIprintf("		  '6'  - dbdb catalog tests only\n\n");
    SIprintf("	          '10' - debug mode\n");
}
/*
** qp_cats() - this routine tests the verifydb query processing catalog
**	       checks/repairs
**
** Description:
**
** This routine tests the verifydb query processing catalog checks/repairs.
** This includes checks/repairs on the following catalogs:
**	iidbdepends
**	iievent
**	iiprocedure
**	iirule
**	iixdbdepends
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**	    is set by subordinate routines.
**	Outputs to log files:   ii_config:iivdbp01.DBA to
**				ii_config:iivdbp05.DBA
**	    where DBA is the value of the DBA name input parameter
**  History:
**	16-jun-93 (teresa)
**	    Initial creation.
**	27-jan-94 (andre)
**	    added calls to ck_iipriv(), ck_iixpriv(), ck_iixevent(), and
**	    ck_ooxprocedure()
*/
qp_cats()
{
    SIprintf("Testing Verifydb Check/Repairs On Query Processing Catalogs\n");
    SIflush(stdout);
    if (test_status != TEST_ABORT)
    {
	name_log(QP_CODE,ID_01);
	ck_iidbdepends();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(QP_CODE,ID_02);
	ck_iievent();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(QP_CODE,ID_03);
	ck_iiprocedure();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(QP_CODE,ID_04);
	ck_iirule();
    }
/* currently the server does not allow us to update secondary indexes, so
** we cannot test this
    if (test_status != TEST_ABORT)
    {
	name_log(QP_CODE,ID_05);
	ck_iixdbdepends();
    }
*/
    if (test_status != TEST_ABORT)
    {
	name_log(QP_CODE,ID_06);
	ck_iigw06_attribute();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(QP_CODE,ID_07);
	ck_iigw06_relation();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(QP_CODE,ID_08);
	ck_iipriv();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(QP_CODE,ID_09);
	ck_iixpriv();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(QP_CODE,ID_0A);
	ck_iixevent();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(QP_CODE,ID_0B);
	ck_iixprocedure();
    }
}
/*
** qrymod_cats() - this routine tests the verifydb qrymod catalog checks/repairs
**
** Description:
**
** This routine tests the verifydb qrymod catalog checks/repairs.  This includes
** checks/repairs on the following catalogs:
**	iidefault
**	iiintegrity
**	iikey
**	iiprotect
**	iiqrytxt
**	iisynonym
**	iitree
**	iidbms_comment
**	iisecalarm
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**	    is set by subordinate routines.
**	Outputs to log files:   ii_config:iivdbm01.DBA to
**				ii_config:iivdbm06.DBA
**	    where DBA is the value of the DBA name input parameter
**  History:
**	08-jun-93 (teresa)
**	    Initial creation.
**	18-sep-93 (teresa)
**	    added support for iidbms_comment
**	22-dec-93 (robf)
**          add support for iisecalarm
**	13-jan-94 (anitap)
**	    added support for iikey and iidefault.
**	27-jan-94 (andre)
**	    added call to ck_iixprotect
**	30-Dec-2004 (schka24)
**	    remove same
*/
qrymod_cats()
{
    SIprintf("Testing Verifydb Check/Repairs On QryMod Catalogs\n");
    SIflush(stdout);
    if (test_status != TEST_ABORT)
    {
	name_log(QRYMOD_CODE,ID_01);
	ck_iiintegrity();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(QRYMOD_CODE,ID_02);
	ck_iiprotect();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(QRYMOD_CODE,ID_03);
	ck_iiqrytext();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(QRYMOD_CODE,ID_04);
	ck_iisynonym();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(QRYMOD_CODE,ID_05);
	ck_iitree();
    }
    if (test_status != TEST_ABORT)
    {
	name_log(QRYMOD_CODE,ID_06);
	ck_iidbms_comment();
    }
    if (test_status != TEST_ABORT)
    {
        name_log(QRYMOD_CODE,ID_07);
        ck_iikey();
    }
    if (test_status != TEST_ABORT)
    {
        name_log(QRYMOD_CODE,ID_08);
        ck_iidefault();
    }
}
/*
** report_vdb -- run verifydb (in report mode) via PCcmdline
**
** Description:
**	constructs a command line to run verifydb in report mode and to redirect
**	the output log file to ii_config:iitstvdb.<DBA>  Then calls PCcmdline to
**	execute that command.
** Inputs:
**	Global variable:    dbname  - name of database to open
**	Global variable:    dba	    - name of DBA
** Outputs;
**	Global variable:    test_status	- TEST_OK or TEST_ABORT
** History:
**	08-jun-93 (teresa)
**	    initial creation.
**	13-jul-93 (teresa)
**	    added support for sep flag.
**	23-dec-93 (teresa)
**	    added support for suppress flag.
*/
report_vdb()
{
    char	    cmd[200];
    CL_ERR_DESC	    errdesc;		/* used for PCcmdline() calls */
    char	    sup_cmd[30];
    char	    num_buf[15];
    if (test_status == TEST_ABORT)
	return;    
    if (suppress == DO_NOT_SUPPESS)
	STcopy (" ",sup_cmd);
    else if (suppress == SUPPRESS_ALL)
	STcopy (" -dbms_test",sup_cmd);
    else
    {
	CVlx((u_i4)(suppress-E_DUF_MASK), num_buf);
	STpolycat( 2, " -dbms_test", num_buf, sup_cmd);
    }
    if (sep)
	STpolycat( 4,
		    "verifydb -mreport -odbms -sdbn ",dbname,
		    " -sep", sup_cmd, cmd);
    else
	STpolycat( 5,
		    "verifydb -mreport -odbms -sdbn ",dbname,
		    " -test -lf", logfile, sup_cmd,
		    cmd);
    if (PCcmdline(NULL, cmd, (bool)PC_WAIT, NULL, &errdesc) != OK)
    {
	test_status = TEST_ABORT;
	SIprintf("...ERROR!  Unable to run verifydb.\n");
    }
}
/* run_test()	-  determine if specified test should be run.
**
**  Description:
**
**  Translates the value of argv[5] (subtest) from a numeric value as per
**  below table to see if it applies to this specific test.  Is given
**  an input of catalog (a code indicating which catalog) and test number.
**  It checks to see if the given test number should be run for the given
**  catalog based on the value of argv[5], which is stored in global variable
**  subtest.  If subtest is zero, then run all tests.  Otherwise run only
**  specified test.
**  Another enhancement: if an even multiple of 100 is chosen, it will run
**  all the tests for a given catalog.  If mod(subtest,100) is not zero, then it
**  runs only the specified test.  For example:
**	if argv[4] specifies qrymod tests and subtest is 200, then ALL
**	    the iiprotect tests will be run
**  However
**	if argv[4] specifies qrymod tests and subtest is 201, then ONLY
**	    iiprotect test #1 will be run
**  Futhermore
**	if argv[4] specifies qrymod tests and subtest is 0, then
**	    ALL iiprotect tests will be run
**
**			CORE:   101 to 127 - run iirelation test 1 to 29
**					   currently there is no test 26 or 28
**				201 - run iirel_idx test 1
**				301 to 302 - run iiattribute test 1 to 2
**				401 to 403 - run iiindex test 1 to 3
**				501 to 502 - run iidevices test 1 to 2
**			QRYMOD: 101 to 104 - run iiintegrity tests 1 to 4
**			        201 to 204 - run iiprotect tests 1 to 4
**			        301 to 306 - run iiqrytext tests 1 to 6
**			        401 to 402 - run iisynonym tests 1 to 2
**			        501 to 506 - run iitree tests 1 to 6
**			        601 to 605 - run iidbms_comment tests 1 to 5
**				701 to 704 - run iisecalarm tests 1 to 4
**			QP:	101 to 104 - run iidbdepends tests 1 to 4
**					    currently iidbdepepends test 5 is
**					    not runnable.
**			        201 to 202 - run iievent tests 1 to 2
**			        301 to 302 - run iiprocedure tests 1 to 2
**			        401 to 405 - run iirule tests 1 to 5
**			        501 - run iixdbdepends test 1
**					     currently above test isn't runable
**				601 to 603 - run iigw06_attribute tests 1 to 3
**				701 to 703 - run iigw06_relation tests 1 to 3
**			OPT:    101 to 105 - run iistatistics tests 1 to 5
**			        201 to 202 - run iihistogram tests 1 to 2
**			IIDBDB: 101 to 102 - run iicdbid_idx tests 1 to 2
**					     currently above test isn't runable
**			        201 to 215 - run iidatabase tests 1 to 15
**			        301 to 302 - run iidbid_idx tests 1 to 2
**					     currently above test isn't runable
**			        401 to 40~ - run iidbpriv tests 1 to ~
**			        501 to 50~ - run iiextend tests 1 to ~
**			        601 to 60~ - run iilocations tests 1 to ~
**			        701 to 70~ - run iistar_cdbs tests 1 to ~
**			        801 to 80~ - run iiuser tests 1 to ~
**			        901 to 90~ - run iiusergroup tests 1 to ~
**
** Inputs:
**	Catalog			A numeric code specifying which catalog is
**				being operated on.
**	testid			a numeric code indaciting which test is being
**				run
** Outputs;
**	RETURNS:    TRUE	if test should be run
**		    FALSE	if test should be skipped
** History:
**	22-jun-93 (teresa)
**	    initial creation.
**	2-aug-93 (stephenb)
**	    Added checks for iigw06_relation and iigw06_attribute tests.
**	18-sep-93 (teresa)
**	    Added Support for iidbms_comment
**	22-dec-93 (teresa)
**	    added call to ck_suppress
**	27-jan-94 (andre)
**	    rewrote to use symbolic constants, added checks for IIXPRIV, IIPRIV,
**	    IIXEVENT, IIXPROCEDURE, IIXPROTECT
**	02-feb-94 (huffman) && (mhuishi)
**	    A conditional was_too_complex for the AXP C compiler to optimize.
**	    This is the programming 101 fix around it.
*/
run_test(catalog, testid)
i4	    catalog;
i4	    testid;
{
    bool	retstat;
    i4	test_base;
    switch (catalog)
    {
	case IIRELATION:
	case IIREL_IDX:
	case IIATTRIBUTE:
	case IIINDEX:
	case IIDEVICES:
	    test_base = catalog - CORE_CATALOGS;
	    break;
	case IIINTEGRITY:
	case IIPROTECT:
	case IIQRYTEXT:
	case IISYNONYM:
	case IITREE:
	case IIDBMS_COMMENT:
	case IIKEY:
	case IIDEFAULT:
	case IISECALARM:
	    test_base = catalog - QRYMOD_CATALOGS;
	    break;
	case IIDBDEPENDS:
	case IIEVENT:
	case IIPROCEDURE:
	case IIRULE:
	case IIXDBDEPENDS:
	case IIGW06_ATTRIBUTE:
	case IIGW06_RELATION:
	case IIPRIV:
	case IIXPRIV:
	case IIXEVENT:
	case IIXPROCEDURE:
	    test_base = catalog - QP_CATALOGS;
	    break;
	case IISTATISTICS:
	case IIHISTOGRAM:
	    test_base = catalog - OPTIMIZER_CATALOGS;
	    break;
	case IICDBID_IDX:
	case IIDATABASE:
	case IIDBID_IDX:
	case IIDBPRIV:
	case IIEXTEND:
	case IILOCATIONS:
	case IISTAR_CDBS:
	case IIUSER:
	case IIUSERGROUP:
	case IIPROFILE:
	    test_base = catalog - IIDBDB_CATALOGS;
	    break;
	default:
	    test_base = -1;
	    break;
    }
    /*
    ** test will be run if catalog id is one of those we know about, and one of 
    ** the following is true:
    **	- we were told to run all tests on catalogs withing this group 
    **	  (subtest == 0),
    **	- we were told to run all tests on this catalog (subtest == test_base),
    **	- we were told to run the specified test (subtest == test_base + testid)
    */
    retstat = FALSE;
    if (   test_base != -1 )
	 if (   subtest == 0
	     || subtest == test_base 
	     || subtest == test_base + testid)
	    retstat = TRUE;
    if (retstat)
	ck_suppress(catalog, testid);
    else
	suppress = DO_NOT_SUPPESS;
    return (retstat);
}
/*
** run_vdb -- run verifydb (in TEST_RUNI mode) via PCcmdline
**
** Description:
**	constructs a command line to run verifydb in test_runi mode and to
**	redirect the output log file to ii_config:iitstvdb.<DBA>  Then calls
**	PCcmdline to execute that command.
** Inputs:
**	Global variable:    dbname  - name of database to open
**	Global variable:    dba	    - name of DBA
** Outputs;
**	Global variable:    test_status	- TEST_OK or TEST_ABORT
** History:
**	08-jun-93 (teresa)
**	    initial creation.
**	13-jul-93 (teresa)
**	    added support for sep flag.
**	23-dec-93 (teresa)
**	    added support for suppress flag.
*/
run_vdb()
{
    char	    cmd[200];
    CL_ERR_DESC	    errdesc;		/* used for PCcmdline() calls */
    char	    sup_cmd[30];
    char	    num_buf[15];
    if (test_status == TEST_ABORT)
	return;    
    if (suppress == DO_NOT_SUPPESS)
	STcopy (" ",sup_cmd);
    else if (suppress == SUPPRESS_ALL)
	STcopy (" -dbms_test",sup_cmd);
    else
    {
	CVlx((u_i4)(suppress-E_DUF_MASK), num_buf);
	STpolycat( 2, " -dbms_test", num_buf, sup_cmd);
    }
    if (sep)
	STpolycat( 4,
		    "verifydb -mtest_runi -odbms -sdbn ",dbname,
		    " -sep", sup_cmd, cmd);
    else
	STpolycat( 5,
		    "verifydb -mtest_runi -odbms -sdbn ",dbname,
		    " -test -lf", logfile, sup_cmd,
		    cmd);
    if (PCcmdline(NULL, cmd, (bool)PC_WAIT, NULL, &errdesc) != OK)
    {
	test_status = TEST_ABORT;
	SIprintf("...ERROR!  Unable to run verifydb.\n");
    }
}
/* xlate_string	- translate ascii string to decimal
**
**  Description:
**	This routine translates a null terminated string from ascii to hex.
**	The string must not be more than MAX_DIGITS+1 long, where the last
**	character is a null termination.  All characters in the string must
**	be ascii representations of digits with values between '0' and '9'.
**  Inputs:
**	op_code		    Op code string to translate
**  Outputs:
**	none
**  History
**	08-jun-93 (teresa)
**	    Initial creation.
*/
xlate_string(num_string, number)
char	    *num_string;
int	    *number;
{
	int	value;
	int	num_digits;
	int	i;
	do 
	{
	    for (num_digits= 0; num_digits<= MAX_DIGITS; num_digits++)
	    {   
		if (num_string[num_digits] == '\0')
		    break;
	    }
	    if ( 
		 (num_digits == MAX_DIGITS) && (num_string[num_digits] != '\0')
	       ||
		 num_digits <= 0
	       )
	    {
		/* op code is too long */
		value = INVALID_OPCODE;
		break;
	    }	
	    /* xlate ASCII to decimal */	
	    for (i=0, value=0; i < num_digits; i++)
	    {
		value *= 10;
		switch (num_string[i])
		{
		case '1':
		    value += 1;
		    break;
		case '2':
		    value += 2;
		    break;	    
		case '3':
		    value += 3;
		    break;
		case '4':
		    value += 4;
		    break;
		case '5':
		    value += 5;
		    break;
		case '6':
		    value += 6;
		    break;
		case '7':
		    value += 7;
		    break;
		case '8':
		    value += 8;
		    break;
		case '9':
		    value += 9;
		    break;
		}
	    }	    	    
	} while (0);  /* control loop */
	*number = value;    
}
/* this is the basis to build your routine on */
/*
** dummy() - 
**
** Description:
**
**  
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
*/
dummy()
{
  i4 id1;
  i4 id2;
    SIprintf ("..Testing verifydb checks/repairs for catalog ii*****..\n");
    SIflush(stdout);
    STcopy("ii*****",test_id);
    do
    {
	if ( run_test(II,1) )
	{
	    SIprintf ("....ii***** test #..\n");
	    SIflush(stdout);
	    /*************/
	    /** test #  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    /* exec sql <<statements to set up for test go here >> */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
	    /* exec sql delete <<statements to clean up from test go here>> */
	    close_db();
	}
    } while (0);  /* control loop */
}
/*
** ck_iisecalarm() - 
**
** Description:
**
**  This routine tests the verifydb iidbms_comment catalog checks/repairs.
**
**  Inputs:
**	None.
**  Outputs:
**	global variable: test_status	- status of tests: TEST_OK, TEST_ABORT
**  History:
**	22-dec-93  (robf)
**	    Initial Creation.
*/
ck_iisecalarm()
{
  i4 id1;
  i4 id2;
  i4 val;
    SIprintf ("..Testing verifydb checks/repairs for catalog iisecalarm..\n");
    SIflush(stdout);
    STcopy("iisecalarm",test_id);
    do
    {
	if ( run_test(IISECALARM,1) )
	{
	    SIprintf ("....iisecalarm test 1..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 1  **/
	    /*************/
	    /** setup **/
	    open_db();
/* # line 10794 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_secal1(name c10)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10795 "duvetest.sc" */	/* create dbevent */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create dbevent duve_secal1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10796 "duvetest.sc" */	/* create security_alarm */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create security_alarm duve_secal1 on table duve_secal1 raise dbevent \
duve_secal1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10798 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iisecalarm set obj_id1= -1, obj_id2= -1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10799 "duvetest.sc" */	/* host code */
	    if (test_status == TEST_ABORT)
		break;
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 10811 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_secal1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10812 "duvetest.sc" */	/* drop dbevent */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop dbevent duve_secal1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10813 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IISECALARM,2) )
	{
	    SIprintf ("....iisecalarm test 2..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 2  **/
	    /*************/
	    /** setup **/
	    open_db();
	    	    if (test_status == TEST_ABORT)
		break;
/* # line 10829 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_secal1(name c10)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10830 "duvetest.sc" */	/* create dbevent */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create dbevent duve_secal1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10831 "duvetest.sc" */	/* create security_alarm */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create security_alarm duve_secal1 on table duve_secal1 raise dbevent \
duve_secal1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10833 "duvetest.sc" */	/* host code */
	    id1=ALARM_BIT;
/* # line 10834 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirelation set relstat2=relstat2 -");
    IIputdomio((short *)0,1,30,4,&id1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10835 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 10845 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_secal1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10846 "duvetest.sc" */	/* drop dbevent */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop dbevent duve_secal1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10847 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IISECALARM,3) )
	{
	    SIprintf ("....iisecalarm test 3..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 3  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 10863 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_secal1(name c10)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10864 "duvetest.sc" */	/* create dbevent */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create dbevent duve_secal1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10865 "duvetest.sc" */	/* create security_alarm */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create security_alarm duve_secal1 on table duve_secal1 raise dbevent \
duve_secal1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10867 "duvetest.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext q where exists(select * from iisecalarm a where\
 a.alarm_name='duve_secal1' and q.txtid1=a.alarm_txtid1 and q.txtid2=a\
.alarm_txtid2)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10874 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 10884 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_secal1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10885 "duvetest.sc" */	/* drop dbevent */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop dbevent duve_secal1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10886 "duvetest.sc" */	/* host code */
	    close_db();
	}
	if ( run_test(IISECALARM,4) )
	{
	    SIprintf ("....iisecalarm test 4..\n");
	    SIflush(stdout);
	    /*************/
	    /** test 4  **/
	    /*************/
	    /** setup **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 10902 "duvetest.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table duve_secal1(name c10)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10903 "duvetest.sc" */	/* create dbevent */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"create dbevent duve_secal1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10904 "duvetest.sc" */	/* create security_alarm */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create security_alarm duve_secal1 on table duve_secal1 raise dbevent \
duve_secal1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10906 "duvetest.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iisecalarm set event_id1= -1, event_id2= -1 where alarm_name='\
duve_secal1'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10908 "duvetest.sc" */	/* host code */
	    close_db();
	    /** run test **/
	    report_vdb();
	    run_vdb();
	    /** clean up **/
	    open_db();
	    if (test_status == TEST_ABORT)
		break;
/* # line 10918 "duvetest.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table duve_secal1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10919 "duvetest.sc" */	/* drop dbevent */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop dbevent duve_secal1");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode == 700) 
      IIsqPrint(&sqlca);
  }
/* # line 10920 "duvetest.sc" */	/* host code */
	    close_db();
	}
    } while (0);  /* control loop */
}
