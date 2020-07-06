# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/* Copyright (c) 1990, 2005 Ingres Corporation
**
**
*/
#include    <compat.h>
#include    <gl.h>
#include    <pc.h>
#include    <st.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <duf.h>
#include    <er.h>
#include    <usererror.h>
#include    <duerr.h>
#include    <cs.h>
#include    <lk.h>
# include <dudbms.h>
# include <duenv.h>
#include    <ducdb.h>
#include    <me.h>
#include    <nm.h>
#include    <si.h>
#include    <cui.h>
#include    <st.h>
# include <dustar.h>
#include    <tm.h>
#include    <duucatdef.h>
#include    <lo.h>
#include    <pm.h>
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/*
**  Name: ducommon.qsc - createdb / convto60 shared routines
**
**  Description:
**      This module is routines that used to be duplicated between createdb
**	and convto60.  They are put into this module for build (link) purposes. 
**
**          duc_dbms_bld	    - Builds DBMS catalogs
**	    duc_1makedbdb	    - Make and configure system catalogs 
**				      specific to the "dbdb".
**	    duc_6create_iidbdb_cats - Creates STAR system catalogs in iidbdb
**	    duc_cpy_usrf	    - append all of the "users" file entries
**				      to the "iiuser" table in the "iidbdb".
**
**  History:
**	08-Mar-1991 (teresa)
**	    initial creation
**	16-sep-1992 (robf)
**	    added catalogs for C2 Security Audit Gateway (SXA)
**	09-nov-92 (rickh)
**	    FIPS CONSTRAINTS improvements.
**	25-sep-92 (rblumer)
**	    merged with 9/8 integration changes:
**	11-sep-92 (andre)
**	    MSB in relstat is no longer being set - change
**	    mod((relstat+1073741824+1073741824),x) back to mod(relstat,x)
**	05-oct-92 (rblumer)
**	    merge system_generated with system_use column,
**          add 4 more standard catalogs for CONSTRAINTS.
**      09-nov-92 (jrb)
**          Change ING_SORTDIR to ING_WORKDIR and DU_SORTS_LOC to DU_WORK_LOC
**	    and append tuple to iiextend for work locations and turn on new
**          DU_EXT_DATA bit in iiextend for data locations.  ML Sorts Project.
**	18-nov-92 (robf)
**	    When REGISTERing iiaudit, add an explicit check for success
**	    to make sure a diagnostic is issued.
**	    Update creation of iisectype to correspond to new audit types
**	    Make sure iiaudit includes object owner column
**	05-dec-92 (rblumer)
**	    add column_mandatory and column_default_value to iicolumns,
**	    take deftype out of iidefault, get rid of iidefaults view.
**	16-dec-92 (rickh)
**	    Modify IIINTEGRITY immediately after you create it.  This allows
**	    subsequent tables to be created with "NOT NULL" columns.  In the
**	    brave, new FIPS CONSTRAINTS world, "NOT NULL" creates a constraint,
**	    which in turn invokes QEF code that expects IIINTEGRITY to have
**	    a key structure.
**	14-jan-93 (rblumer)
**	    iiprocedures.system_use now looks at bit 0x80 = 128, not 0x10 = 16.
**	20-jan-93 (anitap)
**	    Fixed bugs in duc_iischema() and duc_m9_schema(). Changed 
**	    schema_name & schema_owner to 32. Added schema_idx to 
**	    iischema.
**	08-feb-93 (anitap)
**	    Fixed bugs in duc_iischema() and duc_m9_schema().
**	2-Feb-1993 (jpk)
**	    Encapsulate creation and modify back to storage structure
**	    of system catalogs so that upgradedb can call the same
**	    function as createdb.
**	20-feb-93 (rblumer)
**	    add tuples to IIDEFAULT for DB_DEF_UNKNOWN, logical keys,
**	    INITIAL_USER, and DBA; in IICOLUMNS, changed column_has_default
**	    and column_defaults to be 4-valued; change position to key_position
**	    in IIKEY and IIKEYS, as position is a reserved word in FULL SQL92.
**	22-feb-93 (rickh)
**	    Upped the length of the value string in IIDEFAULT by 1 byte.  This
**	    forces (given the null indicator byte and the 2 byte length
**	    descriptor) 4 byte alignment in our DBMS.H structure, which is good
**	    for the secondary index (which has a concluding tidp column).
**	2-mar-93 (rickh)
**	    Add tuple to iischema, binding $ingres to its canonical schema id.
**	07-mar-93 (rblumer)
**	    added pp_ to all column names in iiprocedure, and key_ to all
**	    columns in iikey, to following existing catalog conventions;
**	    add 'and intnumber = 1' to WHERE clause for iiconstraints.
**	9-mar-93 (rickh)
**	    Added DB_DEF_ID_NEEDS_CONVERTING and DB_DEF_ID_UNRECOGNIZED
**	    tuples to IIDEFAULT.
**	23-mar-93 (rblumer)
**	    check in iiconstraints should be 'intseq = 0', not 'intnumber = 1';
**	    change column_defaults in iicolumns to handle calculated columns
**	    in a backward-compatible way.
**	24-mar-93 (rickh)
**	    Extended the 07-mar-93 fix to add "key_" to the columns mentioned
**	    in duc_modify_iikey and duc_v_iikeys.
**	05-apr-93 (rblumer)
**	    change iitables and iiindexes to use correct relstat2 bits for
**	    unique_scope; changed views for iiref_constraints and
**	    iiconstraint_indexes to use new way of joining iidbdepends
**	    with iiintegrity. 
**	13-apr-93 (rblumer)
**	    in duc_1makedbdb, check status of duc_cpy_usrf and return if error.
**	14-apr-93 (ralph)
**	    DELIM_IDENT:
**	    Special handling for non-standard case translation semantics
**	    when creating the iidbdb in duc_1makedbdb.
**	    Do case insensitive compares in duc_p5_alter_extension().
**	    Initialize proc_own in duc_p5_alter_extension().
**	24-may-1993 (bryanp)
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**	02-jun-1993 (ralph)
**	    DELIM_IDENT:
**	    Pass in info on case translation semantics when creating iidbdb.
**	    Remove delimiters in du_add_privd_user();
**	    if the calls wants them, the caller should specify them.
**	    Make search for relation name case insensitive in rel_exists().
**	21-jul-93 (ralph)
**	    DELIM-IDENT:
**	    Add support for DU_REAL_USER_MIXED when creating the iidbdb.
**	    Collapse all case translation flags for iidbdb creation into -G.
*	04-aug-1993 (shailaja)
**	    Unnested comments.
**	8-aug-93 (ed)
**	    unnest dbms.h
**	09-nov-93 (rblumer)
**	    iiqef_createdb has 2 new parameters: the translated and
**	    untranslated database owner name.
**	    added upper_case parameter to duc_iischema function call.
**	    For iidatabase append, changed hard-coded $ingres ownername to be
**	    upper/lower case depending on dbservice translation semantics.
**	13-oct-93 (swm)
**	    Bug #56448
**	    Removed d_print() function because it accepts a variable number
**	    of variable-sized arguments, which is not portable. It could
**	    not become a varargs function as this practice is outlawed in
**	    main line code.
**	    Replaced invocations of the function with calls to SIprintf. This
**	    works because SIprintf is a varargs function.
**      31-jan-94 (mikem)
**          Added include of <cs.h> before <lk.h>.  <lk.h> now references
**          a CS_SID type.
**      7-dec-1994 (andyw)
**          Changed view to handle default work location for
**          partial backup & recovery project
**      12-Dec-1994 (liibi01) -- Cross integration from 6.4 (Bug 59255).
**          Added update of create date for core catalogs to fix bug 59255.
**	25-jan-1995 (medji01)
**	    Backed out changes for bug fix 59255 applied during cross
**	    integration from 6.4. This is this incorrect place for those
**	    fixes, they have already been applied to dmmcre.c to fix bug
**	    45676.
**      01-mar-95 (harpa06)
**          Bug #66957: Changed the Database version number to OPING1.1 from
**          ING6.0
**	10-nov-95 (nick)
**	    Encapsulate iihistogram code.
**	13-nov-95 (prida01)
**	    Fix for bug 71292. Add iidd_stats code
**	 4-apr-96 (nick)
**	    Encapsulate iimulti_locations code here to allow calling from
**	    both dui and upgradedb.
**      06-mar-96 (nanpr01)
**          Changed iitables, iiphysical_tables, iiindexes standard catalogue 
**	    interface to return relation page size.
**	 4-apr-96 (nick)
**	    Encapsulate iimulti_locations code here to allow calling from
**	    both dui and upgradedb.
**	08-may-1996 (shero03)
**	    Added iirange for RTree
**	15-jul-1996 (angusm)
**	    Intersolv ODBC v3.0 project: add new catalogs 'iiproc_params',
**	    'iiproc_access' and alter definition of 'iiaccess'
**	05-jul-96 (pchang)
**	    Corrected user privilege constant for AUDITOR column for 
**	    iiusers, iiprofiles and iiroles (b66257, b74261 & b75538).
**      28-sep-96 (mcgem01)
**	    Global data moved to ducdata.c
**	23-oct-96 (hanch04)
**	    Added reltotwid and relversion
**	26-Mar-1997 (jenjo02)
**	    Table priority project:
**	    Added table cache priority to iitables.
**	01-jun-1999 (kitch01)
**	    As part of fix for bug 97040 move creation of iilog_help view
**	    here as procedure duc_iilog_help(). This allows both createdb
**	    and upgradedb to use this function.
**	21-may-1999 (hanch04)
**	    Replace STbcompare with STcasecmp,STncasecmp,STcmp,STncmp
**      03-aug-1999 (stial01)
**          Added duc_iietab(), duc_modify_iietab()
**      02-Mar-2001 (hanch04)
**          Bumped release id to external "II 2.6" internal 860 or 8.6
**	02-Mar-2001 (jenjo02)
**	    Added raw_blocks to iilocation_info view.
**      16-apr-2001 (stial01)
**          Added tups_per_page, keys_per_page, keys_per_leaf to iitables view
**	17-apr-2001 (stial01)
**          Change tableinfo to iitableinfo
**	26-apr-2001 (horda03)
**	    For every user defined in CONFIG.DAT, add an entry to IIUSER when
**	    IIDBDB is created. (Bug 100039)
**      27-apr-2001 (stial01)
**          execute procedure iiqef_create_db with correct number of parameters
**	11-May-2001 (jenjo02)
**	    rawblocks moved from iilocations to iiextend, rawpct added 
**	    to iilocations.
**	23-May-2001 (jenjo02)
**	    Added duc_p6_checklocarea() to install internal procedure
**	    iiQEF_check_loc_area.
**	15-Oct-2001 (jenjo02)
**	    Deleted short-lived iiQEF_check_loc_area. Area directories
**	    are now made by CREATE/ALTER LOCATION.
**      24-0ct-2001 (stial01)
**          Compare lowercase(dbp_name), in case SQL92 installation (b106147)
**      09-jan-2002 (stial01)
**          Added routines to create/drop/upgrade Standard catalog views
**          (SIR 106745)
**	02-Oct-2002 (hanch04)
**	    If runing against a 64-bit server, then dbservice should be set 
**	    to LP64.
**	21-nov-2002 (somsa01)
**	    On Windows, in duc_cpy_usrf(), create "administrator".
**	29-jan-03 (inkdo01)
**	    Overhauled column names in iisequences to be "compound" (and 
**	    avoid use of reserved words).
**      07-Apr-2003 (hanch04)
**          Bumped release id to external "II 2.65" internal 865 or 8.65
**	19-Dec-2003 (schka24)
**	    Create partitioned-table catalogs; update iitables view with
**	    partitioning columns.
**      13-Jan-2004 (sheco02)
**          Bumped release id to external "II 3.0" internal 900 or 9.00
**	28-jan-2004 (somsa01)
**	    Only manually create the "Administrator" user on Windows if
**	    PMload() fails. Normally we take the users which are in
**	    config.dat and add them to the iiuser table.
**	29-Jan-2004 (schka24)
**	    Ditch iipartition, turns out we don't need it.
**	19-Feb-2004 (schka24)
**	    Get num_rows and number_pages via iitableinfo because they are
**	    computed inside DMF for partitioned masters.
**      10-mar-2004 (rigka01)
**	    Cross change 466625 for 109159 to avoid bug 111927/INGCBT516 
**	20-Apr-2004 (bonro01)
**	    Allow Ingres to be installed as a user other than 'ingres'
**	29-Apr-2004 (gorvi01)
**		Added DU_IIQEF_DEL_LOCATION constant. Added case for del_location
**		internal procedure in duc_create_one_internal_proc.
**	21-Jun-2004 (schka24)
**	    Added standard catalog views for partitioning catalogs.
**	7-Sep-2004 (schka24)
**	    iiotables and iiocolumns have to be here for upgradedb.
**	28-dec-04 (inkdo01)
**	    Added "column_collid" to iicolumns.
**	29-Dec-2004 (schka24)
**	    Change iiprotect to btree; make all btree's fillfactor=100.
**      23-feb-2005 (stial01)
**          Explicitly set isolation level after connect
**	19-Oct-2005 (schka24)
**	    Cause view_base columns to return nothing (ie. 'N') as part
**	    of integrating inifa01's change 479603 which removed the vbase
**	    bit from iirelation.  N.B. it is OK to leave existing catalog
**	    views alone since at present vbase is meaningless anyway.
**	7-Nov-2005 (schka24)
**	    We shouldn't have removed the B1 security columns from the SCI
**	    views, breaks compatability with earlier releases.  Put 'em
**	    back as dummy columns.  (For the security-label columns, use
**	    a char.)
**      17-Jan-2005 (hweho01)
**          Changed table version to ING9.0. 
**      01-Feb-2006 (hweho01)
**          Modified table version string to "II9.0", need to maintain the
**          compatibility with current/previous releases.
**      27-Apr-2006 (hweho01)
**          Changed table version to II9.1 for new release.  
**	10-May-2006 (jenjo02)
**	    Added "is_clustered" to DU_IITABLES, DU_IIPHYSICAL_TABLES views.
**	30-May-2006 (jenjo02)
**	    Backed out "is_clustered"; iistructure(relspec,relstat) modified to return
**	    "clustered" as structure type.
**	19-Sep-2006 (gupsh01)
**	    Use ingresdate instead of date when creating the catalogs with
**	    date columns.
**      23-oct-2006 (stial01)
**           Back out Ingres2007 change to iistructure()
**      23-oct-2006 (stial01)
**           Table version should not advance for ingres2006r2
**	23-nov-2006 (dougi)
**	    Reinstate view_base column to be based on relstat.
**      11-Mar-2004 (hanal04) Bug 111923 INGSRV2751
**          The view lgmo_xa_dis_tran_ids casts the branch_seqnum and
**          branch_flags as INT2. These fields are i4's in 
**          DB_XA_EXTD_DIS_TRAN_ID and all code other code references.
**          If either value was greater than a MAXI2 the existing
**          code would generate an interger overflow that was
**          difficult to track down.
**          This change will resolve the problem for newly created 
**          iidbdb databases. Existing iidbdb's must drop the
**          view lgmo_xa_dis_tran_ids replacing INT2 with INT4 in
**          all cases. The grant seen below must also be reissued.
**      22-Aug-2007 (kiria01) b118999
**          Switch the charextract to use byteextract to ensure that
**          only one byte was returned as opposed to 1 potentially
**          multi-byte character.
*/
GLOBALREF	DU_ERROR	*Duc_errcb; 
FUNC_EXTERN	DU_STATUS	duc_cpy_usrf();
FUNC_EXTERN	DU_STATUS	sqlerr_handler();
FUNC_EXTERN	DU_STATUS	duc_finish_dbdb_create();
FUNC_EXTERN	void		duc_iidbms_comment();
FUNC_EXTERN	void		duc_iisynonym();
FUNC_EXTERN	void		duc_iipriv();
FUNC_EXTERN	void		duc_iistatistics();
FUNC_EXTERN	void		duc_modify_iidd_stats();
FUNC_EXTERN	void		duc_modify_iistatistics();
FUNC_EXTERN	void		duc_iihistogram();
FUNC_EXTERN	void		duc_modify_iihistogram();
FUNC_EXTERN	void		duc_iicolcompare();
FUNC_EXTERN	void		duc_modify_iicolcompare();
FUNC_EXTERN	void		duc_iidd_stats();
FUNC_EXTERN	void		duc_iidevices();
FUNC_EXTERN	void		duc_modify_iidevices();
FUNC_EXTERN	void		duc_iiextend();
FUNC_EXTERN	void		duc_modify_iiextend();
FUNC_EXTERN	void		duc_iievent();
FUNC_EXTERN	void		duc_iidatabase();
FUNC_EXTERN	void		duc_modify_iidatabase();
FUNC_EXTERN	void		duc_iilocations();
FUNC_EXTERN	void		duc_modify_iilocations();
FUNC_EXTERN	void		duc_iirole();
FUNC_EXTERN	void		duc_modify_iirole();
FUNC_EXTERN	void		duc_iiusergroup();
FUNC_EXTERN	void		duc_modify_iiusergroup();
FUNC_EXTERN     void            duc_modify_procedure();
FUNC_EXTERN     void            duc_modify_iievent();
FUNC_EXTERN     void            duc_iisequence();
FUNC_EXTERN     void            duc_modify_iisequence();
FUNC_EXTERN	void		duc_iipartcats();
FUNC_EXTERN	void		duc_modify_iipartcats();
FUNC_EXTERN     void            duc_iidbdepends();
FUNC_EXTERN     void            duc_modify_iidbdepends();
FUNC_EXTERN	void		duc_iisectype();
FUNC_EXTERN	DU_STATUS	duc_iigwsxa(DU_ERROR *);
FUNC_EXTERN	DU_STATUS	duc_iisecuritystate();
FUNC_EXTERN	VOID		duc_iirule();
FUNC_EXTERN	VOID		duc_modify_iirule();
FUNC_EXTERN	VOID		duc_iiintegrity();
FUNC_EXTERN	VOID		duc_modify_iiintegrity();
FUNC_EXTERN	VOID		duc_iikey();
FUNC_EXTERN	VOID		duc_modify_iikey();
FUNC_EXTERN	VOID		duc_iidefault();
FUNC_EXTERN	VOID		duc_modify_iidefault();
FUNC_EXTERN	VOID		duc_iiprotect();
FUNC_EXTERN	VOID		duc_iisecalarm();
FUNC_EXTERN	VOID		duc_m_iisecalarm();
FUNC_EXTERN	VOID		duc_iiuser();
FUNC_EXTERN	VOID		duc_iidbpriv();
FUNC_EXTERN	VOID		duc_modify_iidbpriv();
FUNC_EXTERN	VOID		duc_iiprocedure();
FUNC_EXTERN	VOID		duc_modify_iiprocedure();
FUNC_EXTERN	VOID		duc_iiprocedure_parameter();
FUNC_EXTERN	VOID		duc_iigw07();
FUNC_EXTERN	VOID		duc_modify_iigw07();
FUNC_EXTERN	VOID		duc_iischema();
FUNC_EXTERN	VOID		duc_iilgmo( DU_ERROR *duc_errcb );
FUNC_EXTERN	VOID		duc_m1_dbmscomment();
FUNC_EXTERN	VOID		duc_m2_synonym();
FUNC_EXTERN	VOID		duc_m3_sectype();
FUNC_EXTERN	VOID		duc_m4_protect();
FUNC_EXTERN	VOID		duc_m5_user();
FUNC_EXTERN	VOID		duc_m6_dbpriv();
FUNC_EXTERN	VOID		duc_m7_priv();
FUNC_EXTERN	VOID		duc_m8_gwsxa();
FUNC_EXTERN	VOID		duc_m9_schema();
FUNC_EXTERN 	VOID 		duc_m_iiprofile();
FUNC_EXTERN	VOID 		duc_iiprofile();
FUNC_EXTERN	VOID 		duc_iisesspriv();
FUNC_EXTERN	VOID		duc_iirolegrant();
FUNC_EXTERN	VOID		duc_m_iirolegrant();
FUNC_EXTERN	VOID		duc_iidd_tables();
FUNC_EXTERN	VOID		duc_modify_iidd_tables();
FUNC_EXTERN	VOID		duc_iidd_indexes();
FUNC_EXTERN	VOID		duc_modify_iidd_indexes();
FUNC_EXTERN	VOID		duc_iirange();
FUNC_EXTERN	VOID		duc_modify_iirange();
FUNC_EXTERN     VOID            duc_iietab();
FUNC_EXTERN     VOID            duc_modify_iietab();
bool				proc_exists(char *proc_name);
bool				rel_exists(char *rel_name);
bool				tbl_lacks_col(char *tname, char *cname);
bool				view_exists(char *name);
STATUS				ensure_not_exists(char *name);
DU_STATUS			duc_create_specific_stdcat_view(
				    char	*dbname,
				    char	*viewname,
				    bool	upcase);
GLOBALREF	bool		upg_dbg_out;
GLOBALREF	DUU_CATDEF	*Dus_allcat_defs[];	/* For pagesize mod */
static 		VOID		duc_sqlerror();
FUNC_EXTERN	VOID		duc_r_iiaudit();
static		VOID		duc_v_iisecurity_alarms(bool is_dbdb);
static		VOID		duc_v_iidd_ddb_views();
static		VOID		duc_v_iidd_dbconstants();
static		VOID		duc_v_iidd_physical_tables();
static		VOID		duc_v_iidd_registered_objects();
#define DU_ALL			1  /* create object in all databases */
#define DU_IIDBDB		2  /* only create object in iidbdb */
#define DU_STAR			3  /* only create object if star db */
#define DU_IIPROC_ACCESS	101
#define DU_IIPROC_PARAMS	102
#define DU_IIAUDITTABLES	103
#define DU_IISESSION_PRIVILEGES	104
#define DU_STAR_CDBINFO		105
#define DU_IIDATABASE_INFO	106
#define DU_IIUSERS		107
#define DU_IILOCATION_INFO	108
#define DU_IIEXTEND_INFO	109
#define DU_IIDBPRIVILEGES	110
#define DU_IIPROFILES		111
#define DU_IIROLES		112
#define DU_IIROLEGRANTS		113
#define DU_IISECURITY_STATE	114
				/* 115 unused */
#define DU_IIDBCONSTANTS	116
#define DU_IITABLES		117
#define DU_IICOLUMNS		118
#define DU_IIPHYSICAL_COLUMNS	119
#define DU_IIPHYSICAL_TABLES	120
#define DU_IIVIEWS		121
#define DU_IIINDEXES		122
#define DU_IIINDEX_COLUMNS	123
#define DU_IISTATS		124
#define DU_IIHISTOGRAMS		125
#define DU_IIINGRES_TABLES	126
#define DU_IIMULTI_LOCATIONS	127
#define DU_IIKEY_COLUMNS	128
#define DU_IIEVENTS		129
#define DU_IIPERMITS		130
#define DU_IISECURITY_ALARMS	131
#define DU_IIALT_COLUMNS	132
#define DU_IIFILE_INFO		133
#define DU_IILOG_HELP		134
#define DU_IIPROCEDURES		135
#define DU_IIREGISTRATIONS	136
#define DU_IIDB_COMMENTS	137
#define DU_IIDB_SUBCOMMENTS	138
#define DU_IISYNONYMS		139
#define DU_IIINTEGRITIES	140
#define DU_IIRULES		141
#define DU_IICONSTRAINTS	142
#define DU_IIKEYS		143
#define DU_IIREF_CONSTRAINTS	144
#define DU_IICONSTRAINT_INDEXES	145
#define DU_IIACCESS		146
#define DU_IIDD_DDB_VIEWS	147
#define DU_IIDD_DBCONSTANTS	148
#define DU_IIDD_PHYSICAL_TABLES	149
#define DU_IIDD_REGISTERED_OBJECTS	150
#define DU_IISEQUENCES		151
#define DU_IIDISTSCHEMES	152
#define DU_IIDISTCOLS		153
#define DU_IILPARTITIONS	154
#define DU_IIOTABLES		155	/* Deprecated, but still in existence */
#define DU_IIOCOLUMNS		156	/* Ditto */
typedef struct _DU_OBJ
{
    char *du_objname;
    i4   du_obj_id;
    i4   du_dbflag;
} DU_OBJ;
DU_OBJ Du_stdcat [] = {
{ "iiproc_access",	DU_IIPROC_ACCESS,	DU_ALL },
{ "iiproc_params",	DU_IIPROC_PARAMS,	DU_ALL },
{ "iiaudittables",	DU_IIAUDITTABLES,	DU_ALL },
{ "iisession_privileges", DU_IISESSION_PRIVILEGES, DU_ALL },
{ "iistar_cdbinfo",	DU_STAR_CDBINFO,	DU_IIDBDB},
{ "iidatabase_info",	DU_IIDATABASE_INFO,	DU_IIDBDB},
{ "iiusers",		DU_IIUSERS,		DU_IIDBDB},
{ "iilocation_info",    DU_IILOCATION_INFO,	DU_IIDBDB},
{ "iiextend_info",	DU_IIEXTEND_INFO,	DU_IIDBDB},
{ "iidbprivileges",	DU_IIDBPRIVILEGES,	DU_IIDBDB},
{ "iiprofiles",		DU_IIPROFILES,		DU_IIDBDB},
{ "iiroles",		DU_IIROLES,		DU_IIDBDB},
{ "iirolegrants",	DU_IIROLEGRANTS,	DU_IIDBDB},
{ "iisecurity_state",   DU_IISECURITY_STATE,	DU_IIDBDB},
{ "iidbconstants",	DU_IIDBCONSTANTS,	DU_ALL},
{ "iitables",		DU_IITABLES,		DU_ALL},
{ "iicolumns",		DU_IICOLUMNS,		DU_ALL},
{ "iiphysical_columns", DU_IIPHYSICAL_COLUMNS,	DU_ALL},
{ "iiphysical_tables",  DU_IIPHYSICAL_TABLES,	DU_ALL},
{ "iiviews",		DU_IIVIEWS,		DU_ALL},
{ "iiindexes",		DU_IIINDEXES,		DU_ALL},
{ "iiindex_columns",	DU_IIINDEX_COLUMNS,	DU_ALL},
{ "iistats",		DU_IISTATS,		DU_ALL},
{ "iihistograms",	DU_IIHISTOGRAMS,	DU_ALL},
{ "iiingres_tables",	DU_IIINGRES_TABLES,	DU_ALL},
{ "iimulti_locations",  DU_IIMULTI_LOCATIONS,	DU_ALL},
{ "iikey_columns",	DU_IIKEY_COLUMNS,	DU_ALL},
{ "iievents",		DU_IIEVENTS,		DU_ALL},
{ "iipermits",		DU_IIPERMITS,		DU_ALL},
{ "iisecurity_alarms",  DU_IISECURITY_ALARMS,	DU_ALL},
{ "iialt_columns",	DU_IIALT_COLUMNS,	DU_ALL},
{ "iifile_info",	DU_IIFILE_INFO,		DU_ALL},
{ "iilog_help",		DU_IILOG_HELP,		DU_ALL},
{ "iiprocedures",	DU_IIPROCEDURES,	DU_ALL},
{ "iiregistrations",	DU_IIREGISTRATIONS,	DU_ALL},
{ "iidb_comments",	DU_IIDB_COMMENTS,	DU_ALL},
{ "iidb_subcomments",	DU_IIDB_SUBCOMMENTS,	DU_ALL},
{ "iisynonyms",		DU_IISYNONYMS,		DU_ALL},
{ "iiintegrities",	DU_IIINTEGRITIES,	DU_ALL},
{ "iirules",		DU_IIRULES,		DU_ALL},
{ "iiconstraints",	DU_IICONSTRAINTS,	DU_ALL},
{ "iikeys",		DU_IIKEYS,		DU_ALL},
{ "iiref_constraints",	DU_IIREF_CONSTRAINTS,	DU_ALL},
{ "iiconstraint_indexes", DU_IICONSTRAINT_INDEXES, DU_ALL},
{ "iiaccess",		DU_IIACCESS,		DU_ALL},
{ "iidd_ddb_views",	DU_IIDD_DDB_VIEWS,	DU_STAR},
{ "iidd_dbconstants",	DU_IIDD_DBCONSTANTS,	DU_STAR},
{ "iidd_physical_tables", DU_IIDD_PHYSICAL_TABLES, DU_STAR},
{ "iidd_registered_objects", DU_IIDD_REGISTERED_OBJECTS, DU_STAR},
{ "iisequences",	DU_IISEQUENCES,		DU_ALL},
{ "iidistschemes",	DU_IIDISTSCHEMES,	DU_ALL},
{ "iidistcols",		DU_IIDISTCOLS,		DU_ALL},
{ "iilpartitions",	DU_IILPARTITIONS,	DU_ALL},
{ "iiotables",		DU_IIOTABLES,		DU_ALL},
{ "iiocolumns",		DU_IIOCOLUMNS,		DU_ALL}
};
#define MAX_STDCAT_VIEWS sizeof(Du_stdcat)/sizeof(DU_OBJ)
#define DU_IIQEF_CREATE_DB	200
#define DU_IIQEF_ALTER_DB	201
#define DU_IIQEF_DESTROY_DB	202
#define DU_IIQEF_ADD_LOCATION	203
#define DU_IIQEF_ALTER_EXTENSION 204
#define DU_IIERROR		205
#define DU_IIQEF_DEL_LOCATION	206
DU_OBJ Du_dbprocs [] = {
{ "iiqef_create_db",	DU_IIQEF_CREATE_DB,	DU_IIDBDB },
{ "iiqef_alter_db",	DU_IIQEF_ALTER_DB,	DU_IIDBDB },
{ "iiqef_destroy_db",	DU_IIQEF_DESTROY_DB,	DU_IIDBDB },
{ "iiqef_add_location",	DU_IIQEF_ADD_LOCATION,	DU_IIDBDB },
{ "iiqef_alter_extension",DU_IIQEF_ALTER_EXTENSION,DU_ALL },
{ "iierror",		DU_IIERROR,		DU_ALL },
{ "iiqef_del_location",	DU_IIQEF_DEL_LOCATION,	DU_IIDBDB },
};
#define MAX_INTERNAL_PROCS sizeof(Du_dbprocs)/sizeof(DU_OBJ)
static		STATUS		duc_create_one_stdcat_view(
				    char	*dbname,
				    DU_OBJ	*view,
				    bool	upcase);
static		STATUS		duc_create_one_internal_proc(
				    char	*dbname,
				    DU_OBJ	*proc,
				    bool	upcase);
static DU_STATUS duc_find_cats();
/*{
** Name: duc_dbms_bld	- Build DBMS catalogs
**
** Description:
**        This routine makes and configures all of the non-core system
**      catalogs for a standard database.  These catalogs are made with 
**      $ingres ownership.
**	   This routine is moved here from ducreate.qsc because it is shared
**	between createdb and convto60.
**
** Inputs:
**      duc_dbenv                       Ptr to DU_ENV
**	duc_errcb			Ptr to error-handling control block.
**	convert_flag			TRUE -> called by convto60, so suppress
**					    informative msgs to user
**					FALSE -> called by createdb, so do not
**					    suppress informative msgs to user
**
** Outputs:
**      *duc_errcb                      If an error occurs, this struct will be
**					set by a call to du_error().
**	Returns:
**	    E_DU_OK			Completed successfully.
**	    D_DU_ERROR			an error was encountered while 
**				        outputting an informative message.
**	Exceptions:
**	    none
**
** Side Effects:
**	dbms catalogs are created and modified.  Informative messages may be
**	output to user.
**
** History:
**	01-mar-91 (teresa)
**	    initial creation.  Lifted code from duc_makedb because that code
**	    was common with convto60. The code is also removed from convto60
**	    an replaced with a call to this routine.
**	11-oct-91 (teresa)
**	    moved functionality common to 6.5 upgradedb into seperate 
**	    subroutines.
**	15-sep-92 (andre)
**	    change IIDBDEPENDS.i_qid and IIDBDEPENDS.qid to i4.
**	16-sep-92 (robf)
**	     add C2/Security Audit Gateway (SXA) catalog creation.
**	09-nov-92 (rganski)
**	    Added two new fields, sversion and shistlength, to iistatistics.
**	    Part of Character Histogram Enhancements project.
**	09-nov-92 (rickh)
**	    29-jul-92 (rickh)
**	    FIPS CONSTRAINTS improvements.  New tables:  iikey, iidefault.
**	    New fields in iiintegrity & iirule.
**	    20-aug-92 (rblumer)
**		added 2 new default types: DBD_NOT_DEFAULT & DBD_NOT_SPECIFIED;
**		changed length of defvalue in iidefault from 1000 to 1500.
**	13-dec-92 (jpk)
**	    create, modify iischema
**	18-jan-93 (rganski)
**	    Changed iistatistics.sversion to char[8].
**	16-dec-92 (rickh)
**	    Modify IIINTEGRITY immediately after you create it.  This allows
**	    subsequent tables to be created with "NOT NULL" columns.  In the
**	    brave, new FIPS CONSTRAINTS world, "NOT NULL" creates a constraint,
**	    which in turn invokes QEF code that expects IIINTEGRITY to have
**	    a key structure.
**	23-dec-92 (rblumer)
**	    changed name of column_mandatory to column_has_default 
**	    and flipped Y and N values.
**	06-jan-92 (rickh)
**	    Removed the second call to duc_m9_schema.  We don't need two
**	    indexes of the same name on iischema.
**	10-Feb-1993 (jpk)
**	    Encapsulate creation and modification of iidevices into
**	    duc_iidevices and duc_modify_iidevices respectively,
**	    similarly duc_iistatistics and duc_modify_iistatistics,
**	    duc_iievent and duc_modify_iievent, duc_iidatabase and
**	    duc_modify_iidatabase, duc_iirole and duc_modify_iirole,
**	    duc_iiextend and duc_modify_iiextend, duc_iiusergroup
**	    and duc_modify_iiusergroup.  Merged duc_iixpriv
**	    into duc_modify_iipriv so secondary indices get rebuilt
**	    whenever primary storage structure is remodified, similarly
**	    duc_iixprocedure and duc_modify_iiprocedure, and duc_iixevent
**	    and duc_modify_iievent.
**	14-Feb-1993 (jpk)
**	    Added call to duc_iigw07 and duc_modify_iigw07 to support
**	    gateway infrastructure for registration of logging system
**	    managed objects.
**	02-mar-93 (andre)
**	    created a function duc_iierror() to create an unternal dbproc
**	    iierror() and to GRANT EXECUTE TO PUBLIC WITH GRANT OPTION on it
**	13-nov-93 (rblumer)
**	    added upper_case argument to duc_iischema function call.
**	26-nov-93 (robf)
**          Create iisecalarm catalog to hold security alarms
**	19-jul-94 (robf)
**          Correct integration problem, duc_iischema was getting called
**          twice along the way.
**	25-jan-1995 (medji01)
**	    Backed out changes for bug fix 59255 applied during cross
**	    integration from 6.4. This is this incorrect place for those
**	    fixes, they have already been applied to dmmcre.c to fix bug
**	    45676.
**	10-nov-95 (nick)
**	    Create duc_modify_iihistogram() and duc_iihistogram().
**	15-jul-96 (angusm)
**		Intersolv ODBC v 3.0 project: Add duc_v_iiproc_params(), 
**		duc_v_iiproc_access().
**	5-mar-02 (inkdo01)
**	    Added duc_iisequence() and duc_modify_iisequence().
**	19-dec-2003 (schka24)
**	    Add create, modify for partitioning catalogs.
**	3-jan-06 (dougi)
**	    Add create, modify for iicolcompare catalog.
**	25-oct-2006 (dougi)
**	    Disable calls to create, modify iicolcompare 'til we're ready.
*/
DU_STATUS
  duc_dbms_bld(duc_errcb, dbenv, convert_flag)
DU_ERROR	*duc_errcb;
  DU_ENV *dbenv;
bool		convert_flag;
  {
	DU_STATUS	status;
	i4		upper_case;
	/* Now, make the rest of the system catalogs. */
	if (! convert_flag)
	{
	    /* Print an informational message. */
	    if ((du_error(duc_errcb, I_DU0201_CR_DBMS_CATS, 0))
		!= E_DU_OK
	       )
		return(duc_errcb->du_status);
	}
/* # line 738 "ducommon.qsc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"set session isolation level serializable");
      IIsyncup((char *)0,0);
    }
/* # line 739 "ducommon.qsc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set nojournaling");
      IIsyncup((char *)0,0);
    }
/* # line 742 "ducommon.qsc" */	/* host code */
	/* finish up core catalogs */
	/* The minpages are for an empty database */
/* # line 744 "ducommon.qsc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"modify iirelation to hash with minpages=32");
      IIsyncup((char *)0,0);
    }
/* # line 745 "ducommon.qsc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"modify iiattribute to hash with minpages=128");
      IIsyncup((char *)0,0);
    }
/* # line 746 "ducommon.qsc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify iiindex to hash");
      IIsyncup((char *)0,0);
    }
/* # line 747 "ducommon.qsc" */	/* host code */
	duc_iidevices();
	/* now start dbms catalogs */
/* # line 749 "ducommon.qsc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"create iiqrytext(txtid1=i4, txtid2=i4, mode=i4, seq=i4, status=i4, tx\
t=varchar(240))");
      IIsyncup((char *)0,0);
    }
/* # line 753 "ducommon.qsc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"create iitree(treetabbase=i4, treetabidx=i4, treeid1=i4, treeid2=i4, \
treeseq=i2, treemode=i2, treevers=i2, treetree=varchar(1024))");
      IIsyncup((char *)0,0);
    }
/* # line 757 "ducommon.qsc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"modify iitree to btree on treetabbase, treetabidx, treemode, treeseq \
with fillfactor=100, compression=(data)");
      IIsyncup((char *)0,0);
    }
/* # line 760 "ducommon.qsc" */	/* host code */
	duc_iiprotect();    /* build catalog iiprotect */
	duc_m4_protect();	/* modify iiprotect to desired sturcture */
	duc_iiintegrity( );	/* build catalog iiintegrity */
	/* we must modify IIINTEGRITY immediately after creating it.  
	** This allows subsequent tables to be created with "NOT NULL"
	** columns.  In the brave, new FIPS CONSTRAINTS world, "NOT NULL"
	** creates a constraint, which in turn invokes QEF code that
	** expects IIINTEGRITY to have a key structure.
	*/
	duc_modify_iiintegrity( );	/* add keys to iiintegrity.  */
        duc_iisecalarm();		/* create iisecalarm */
	duc_m_iisecalarm();		/* modify iisecalarm */
	duc_iidbdepends();		/* build iidbdepends */
	duc_iistatistics();
	duc_iihistogram();
/*	duc_iicolcompare();	not just yet */
	duc_iiprocedure( );	/* create iiprocedure catalog */
	duc_iiprocedure_parameter( );
	duc_iirule( );		/* create iirule catalog */
	duc_iikey( );	/* build iikey catalog */
	duc_iirange( );		/* create iirange catalog */
	duc_iietab( );          /* create iiextended relation catalog */
	duc_iidefault( );	/* build iidefault catalog */
	duc_iievent();		/* build iievent catalog */
	duc_iisequence();	/* build iisequence catalog */
	duc_iipartcats();	/* build partitioning catalogs */
	/* Print an informational message. */
	if ((du_error(duc_errcb, I_DU0203_MOD_DBMS_CATS, 0))
	    != E_DU_OK
	   )
	    return(duc_errcb->du_status);
	duc_iidbms_comment();	/* build iidbms_comment */
	duc_iisynonym();	/* build iisynonym */
	duc_iipriv();		/* build IIPRIV */
	duc_iisectype();	/* build iisectype */
	_VOID_ duc_iigwsxa(duc_errcb);	/* build C2/SXA gateway catalogs */
	upper_case = ((dbenv->du_dbservice & DU_NAME_UPPER) != 0);
	duc_iisesspriv();	/* iisession_privileges */
	duc_iischema(upper_case);	/* build iischema */
	duc_m9_schema();		/* build iischema */
	duc_m1_dbmscomment();	/* modify dbmscomment to desired structure */
	duc_m2_synonym();	/* modify iisynonym to desired sturcture */
	duc_m7_priv();		/* modify IIPRIV to desired structure */
	duc_m3_sectype();	/* modify iisectype to desired sturcture */
	duc_modify_iidevices();
/* # line 828 "ducommon.qsc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"modify iiqrytext to btree on txtid1, txtid2, mode, seq with fillfacto\
r=100");
      IIsyncup((char *)0,0);
    }
/* # line 831 "ducommon.qsc" */	/* host code */
	duc_modify_iidbdepends();	/* modiyy back to btree, add index */
	duc_modify_iistatistics();	/* modify back to hash */	
	duc_modify_iihistogram();	/* modify back to hash */	
/*	duc_modify_iicolcompare();	not yet /* modify back to hash */	
	/* modify iiprocedure and build IIXPROCEDURE - index on IIPROCEDURE */
	duc_modify_iiprocedure();
	duc_modify_iirule( );	/* give iirule a key structure and 2nd index */
	duc_modify_iikey( );	/* give iikey keys */
	duc_modify_iidefault( );	/* give iidefault keys */
	duc_modify_iirange( );	/* give iirange a key structure */
	duc_modify_iietab( );   /* give iiextended_relation a key structure */
	/* modify iievent and build IIXEVENT - an index on IIEVENT */
	duc_modify_iievent();
	duc_modify_iisequence(); /* modify iisequence */
	duc_modify_iipartcats();	/* Modify partitioning catalogs */
	return (E_DU_OK);
  }
/*{
** Name: duc_1makedbdb() -  make and configure system catalogs specific to
**			    the "dbdb".
**
** Description:
**        This routine makes and configures system catalogs that are
**	specific to the "database database".
**
**	Note:  The create statements below must correspond to the
**	structures defined in dudbms.qsh.  These structures are used
**	by the DBMS to interpret tuples directly from data pages.
**
** Inputs:
**      duc_errcb                       Createdb error-control block.
**
** Outputs:
**      *duc_errcb                      If an error occurs, this block will be
**					set by a call to du_error().
**	Returns:
**	    E_DU_OK			Completed successfully.
**	Exceptions:
**	    none
**
** Side Effects:
**	      The "dbdb" specific catalogs are made and modified.
**
** History:
**      25-nov-86 (ericj)
**          Initial creation.
**      16-feb-1989 (edhsu)
**          online backup modificatiion.
**      17-apr-1989 (alexc)
**          Modified for STAR.  Added creation definition for catalogs:
**	    iistar_cdbs, iiddb_netcost, and iiddb_nodecosts.
**      20-mar-89 (ralph)
**          GRANT Enhancements, Phase 1:
**          Create and modify iiusergroup and iiapplication_id catalogs
**	10-may-89 (ralph)
**	    GRANT Enhancements, Phase 2:
**	    duc_1makedbdb() creates iidbpriv catalog.
**	    changed iiapplication_id to iirole;
**	    add new fields (default_group, reserved) to iiuser.
**	12-sep-89 (ralph)
**	    B1 Enhancements:
**		Eliminate creation of iidbaccess.
**		Add DB_ACCESS and UP_SYSCAT to iidbprivileges.
**	16-jan-90 (ralph)
**	    Add password attribute to the iiuser catalog.
**      11-apr-90 (jennifer)
**          Bug 20463 - Initialize iisecuritystate with all events
**          set to off.
**	08-aug-90 (ralph)
**	    Add iisecurity_state, iiusers, and iilocation_info iidbdb views
**	    Revised definition of iidbprivileges for DB_ADMIN & GLOBAL_USAGE.
**	19-sep-1990 (Derek)
**	    Make sure that iidbdb is owned by $ingres not user running createdb.
**	15-jan-91 (ralph)
**	    Rename attribute 'fill1' to 'dbflags' in table 'iidbpriv'
**	2-apr-91 (teresa)
**	    Moved to DUCOMMON.QSC because this logic is shared by convto60 and
**	    createdb.
**	09-nov-92 (jrb)
**          Now turn on DU_EXT_DATA bit when adding tuple to iiextend for 
**	    II_DATABASE, and also add another tuple for II_WORK to iiextend.
**	    ML Sorts Project.
**	7-Feb-1993 (jpk)
**	    Stop reading in users file.  Retain call to duc_cpy_userf()
**	    for now, it will minimally populate the iiuser table.  Add
**	    dbenv parameter to duc_cpy_usrf() so it can get username
**	    so on VMS it can drop a tuple into iiuser for the user
**	    running createdb -S iidbdb.
**	13-apr-93 (rblumer)
**	    check status of duc_cpy_usrf and return if error.
**	14-apr-93 (ralph)
**	    DELIM_IDENT:
**	    Special handling for non-standard case translation semantics
**	    when creating the iidbdb in duc_1makedbdb.
**	04-aug-1993 (shailaja)
**	    Unnested comments.
**	25-aug-93 (robf)
**	   Create/modify iiprofile table
**	09-nov-93 (rblumer)
**	    For iidatabase append, 
**	    changed hard-coded $ingres ownername to be upper/lower case
**	    depending on dbservice translation semantics.
**	27-March-1994 (jpk)
**	    move rebuilding of index iidbid_idx to duc_modify_iidatabase()
**	    not inline in the caller so upgradedb gets it too.
**	    Similarly move rebuilding of index iicdbid_idx to
**	    duc_6create_iidbdb_cats().
**
[@history_template@]...
*/
DU_STATUS
duc_1makedbdb(du_dbenv, duc_errcb)
DU_ENV		*du_dbenv;
DU_ERROR	*duc_errcb;
  {
    int row_cnt;
    char database_name[DB_MAXNAME+4];
    char owner[DB_MAXNAME+4];
    char dbloc[DB_MAXNAME+4];
    char jnlloc[DB_MAXNAME+4];
    char sortloc[DB_MAXNAME+4];
    char ckploc[DB_MAXNAME+4];
    char dumploc[DB_MAXNAME+4];
    char areaname[DB_AREA_MAX+4];
    char err_msg[256];
    i4 db_access;
    i4 db_service;
    i4 status_value;
    char *db_compat;
    i4 db_min_compat;
	DU_STATUS stat;
	/* Print an informational message. */
	if ((du_error(duc_errcb, I_DU0202_CR_DBDB_CATS, 0)) != E_DU_OK)
	    return(duc_errcb->du_status);
	duc_iidatabase();
/* table "iistar_cdbs" created for TITAN date: 01-JUL-88 */
	duc_6create_iidbdb_cats("iistar_cdbs");
/*
**  New tables created for Distributed Optimizer.
*/
	duc_6create_iidbdb_cats("iiddb_netcost");
	duc_6create_iidbdb_cats("iiddb_nodecosts");
	duc_iiuser();
	duc_m5_user();	/* modify iiuser to desired storage sructure */
	duc_iiprofile();  /* create iiprofile catalog */
	duc_m_iiprofile();
	duc_iiextend();
	duc_iilocations();
	duc_iiusergroup();
	duc_iirole();
	duc_iidbpriv();	/* build iidbpriv catalog */
	duc_iirolegrant(); /* build iirolegrant */
	duc_m_iirolegrant(); /* modify iirolegrant */
	duc_iigw07();	/* build iigw07_relation and iigw07_attribute */
	duc_modify_iidatabase();
	duc_modify_iilocations();
	duc_modify_iiextend();
	duc_modify_iiusergroup();
	duc_modify_iirole();
	duc_modify_iigw07();
	duc_m6_dbpriv();	/* modify iidbpriv to desired structure */
	/*
	**  Populate database database catalogs to describe itself.
	*/
	STcopy(du_dbenv->du_dbname, database_name);
	STcopy(du_dbenv->du_dbloc.du_locname, dbloc);
	status_value = DU_EXT_OPERATIVE | DU_EXT_DATA;
/* # line 1035 "ducommon.qsc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append iiextend(dname=");
      IIputdomio((short *)0,1,32,0,database_name);
      IIwritio(0,(short *)0,1,32,0,(char *)", lname=");
      IIputdomio((short *)0,1,32,0,dbloc);
      IIwritio(0,(short *)0,1,32,0,(char *)", status=");
      IIputdomio((short *)0,1,30,4,&status_value);
      IIwritio(0,(short *)0,1,32,0,(char *)")");
      IIsyncup((char *)0,0);
    }
/* # line 1037 "ducommon.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
    }
/* # line 1038 "ducommon.qsc" */	/* host code */
	if (row_cnt != 1)
	{
	    return (du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
		0, "iiextend", 0, DU_DBDBNAME));
	}
	STcopy(du_dbenv->du_dbloc.du_area, areaname);
	status_value = DU_DBS_LOC;
/* # line 1046 "ducommon.qsc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append iilocations(lname=");
      IIputdomio((short *)0,1,32,0,dbloc);
      IIwritio(0,(short *)0,1,32,0,(char *)", area=");
      IIputdomio((short *)0,1,32,0,areaname);
      IIwritio(0,(short *)0,1,32,0,(char *)", status=");
      IIputdomio((short *)0,1,30,4,&status_value);
      IIwritio(0,(short *)0,1,32,0,(char *)")");
      IIsyncup((char *)0,0);
    }
/* # line 1048 "ducommon.qsc" */	/* host code */
      STcopy(du_dbenv->du_sortloc.du_locname, sortloc);
      status_value = DU_EXT_OPERATIVE | DU_EXT_WORK;
/* # line 1050 "ducommon.qsc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append iiextend(dname=");
      IIputdomio((short *)0,1,32,0,database_name);
      IIwritio(0,(short *)0,1,32,0,(char *)", lname=");
      IIputdomio((short *)0,1,32,0,sortloc);
      IIwritio(0,(short *)0,1,32,0,(char *)", status=");
      IIputdomio((short *)0,1,30,4,&status_value);
      IIwritio(0,(short *)0,1,32,0,(char *)")");
      IIsyncup((char *)0,0);
    }
/* # line 1053 "ducommon.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
    }
/* # line 1054 "ducommon.qsc" */	/* host code */
	if (row_cnt != 1)
	{
	    return (du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
		0, "iilocations", 0, DU_DBDBNAME));
	}
	STcopy(du_dbenv->du_jnlloc.du_locname, jnlloc);
	STcopy(du_dbenv->du_jnlloc.du_area, areaname);
	status_value = DU_JNLS_LOC;
/* # line 1063 "ducommon.qsc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append iilocations(lname=");
      IIputdomio((short *)0,1,32,0,jnlloc);
      IIwritio(0,(short *)0,1,32,0,(char *)", area=");
      IIputdomio((short *)0,1,32,0,areaname);
      IIwritio(0,(short *)0,1,32,0,(char *)", status=");
      IIputdomio((short *)0,1,30,4,&status_value);
      IIwritio(0,(short *)0,1,32,0,(char *)")");
      IIsyncup((char *)0,0);
    }
/* # line 1065 "ducommon.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
    }
/* # line 1066 "ducommon.qsc" */	/* host code */
	if (row_cnt != 1)
	{
	    return (du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
		0, "iilocations", 0, DU_DBDBNAME));
	}
	STcopy(du_dbenv->du_dmploc.du_locname, dumploc);
	STcopy(du_dbenv->du_dmploc.du_area, areaname);
	status_value = DU_DMPS_LOC;
/* # line 1075 "ducommon.qsc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append iilocations(lname=");
      IIputdomio((short *)0,1,32,0,dumploc);
      IIwritio(0,(short *)0,1,32,0,(char *)", area=");
      IIputdomio((short *)0,1,32,0,areaname);
      IIwritio(0,(short *)0,1,32,0,(char *)", status=");
      IIputdomio((short *)0,1,30,4,&status_value);
      IIwritio(0,(short *)0,1,32,0,(char *)")");
      IIsyncup((char *)0,0);
    }
/* # line 1077 "ducommon.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
    }
/* # line 1078 "ducommon.qsc" */	/* host code */
	if (row_cnt != 1)
	{
	    return (du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
		0, "iilocations", 0, DU_DBDBNAME));
	}
	STcopy(du_dbenv->du_ckploc.du_locname, ckploc);
	STcopy(du_dbenv->du_ckploc.du_area, areaname);
	status_value = DU_CKPS_LOC;
/* # line 1087 "ducommon.qsc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append iilocations(lname=");
      IIputdomio((short *)0,1,32,0,ckploc);
      IIwritio(0,(short *)0,1,32,0,(char *)", area=");
      IIputdomio((short *)0,1,32,0,areaname);
      IIwritio(0,(short *)0,1,32,0,(char *)", status=");
      IIputdomio((short *)0,1,30,4,&status_value);
      IIwritio(0,(short *)0,1,32,0,(char *)")");
      IIsyncup((char *)0,0);
    }
/* # line 1089 "ducommon.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
    }
/* # line 1090 "ducommon.qsc" */	/* host code */
	if (row_cnt != 1)
	{
	    return (du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
		0, "iilocations", 0, DU_DBDBNAME));
	}
	STcopy(du_dbenv->du_sortloc.du_locname, sortloc);
	STcopy(du_dbenv->du_sortloc.du_area, areaname);
	status_value = DU_WORK_LOC;
/* # line 1099 "ducommon.qsc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append iilocations(lname=");
      IIputdomio((short *)0,1,32,0,sortloc);
      IIwritio(0,(short *)0,1,32,0,(char *)", area=");
      IIputdomio((short *)0,1,32,0,areaname);
      IIwritio(0,(short *)0,1,32,0,(char *)", status=");
      IIputdomio((short *)0,1,30,4,&status_value);
      IIwritio(0,(short *)0,1,32,0,(char *)")");
      IIsyncup((char *)0,0);
    }
/* # line 1101 "ducommon.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
    }
/* # line 1102 "ducommon.qsc" */	/* host code */
	if (row_cnt != 1)
	{
	    return (du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
		0, "iilocations", 0, DU_DBDBNAME));
	}
	stat = duc_cpy_usrf(duc_errcb, du_dbenv);
	if (stat != E_DU_OK)
	    return(stat);
	duc_r_iiaudit();              /* register iiaudit catalog */
	stat = duc_iisecuritystate(duc_errcb);
	if (stat != E_DU_OK)
		return stat;
	db_access = du_dbenv->du_access;
	db_service = du_dbenv->du_dbservice;
	db_compat = DU_CUR_DBCMPTLVL;
	db_min_compat = DU_1CUR_DBCMPTMINOR;
	if (du_dbenv->du_dbservice & DU_NAME_UPPER)
	    STcopy(ERx("$INGRES"), owner);
	else
	    STcopy(ERx("$ingres"), owner);
/* # line 1128 "ducommon.qsc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append iidatabase(name=");
      IIputdomio((short *)0,1,32,0,database_name);
      IIwritio(0,(short *)0,1,32,0,(char *)", own=");
      IIputdomio((short *)0,1,32,0,owner);
      IIwritio(0,(short *)0,1,32,0,(char *)", dbdev=");
      IIputdomio((short *)0,1,32,0,dbloc);
      IIwritio(0,(short *)0,1,32,0,(char *)", jnldev=");
      IIputdomio((short *)0,1,32,0,jnlloc);
      IIwritio(0,(short *)0,1,32,0,(char *)", ckpdev=");
      IIputdomio((short *)0,1,32,0,ckploc);
      IIwritio(0,(short *)0,1,32,0,(char *)", sortdev=");
      IIputdomio((short *)0,1,32,0,sortloc);
      IIwritio(0,(short *)0,1,32,0,(char *)", dmpdev=");
      IIputdomio((short *)0,1,32,0,dumploc);
      IIwritio(0,(short *)0,1,32,0,(char *)", access=");
      IIputdomio((short *)0,1,30,4,&db_access);
      IIwritio(0,(short *)0,1,32,0,(char *)", dbservice=");
      IIputdomio((short *)0,1,30,4,&db_service);
      IIwritio(0,(short *)0,1,32,0,(char *)", dbcmptlvl=");
      IIputdomio((short *)0,1,32,0,db_compat);
      IIwritio(0,(short *)0,1,32,0,(char *)", dbcmptminor=");
      IIputdomio((short *)0,1,30,4,&db_min_compat);
      IIwritio(0,(short *)0,1,32,0,(char *)", db_id=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1142 "ducommon.qsc" */	/* host code */
	return(E_DU_OK);
  }
/*{
** Name: duc_6create_iidbdb_cats	- Creates STAR system catalogs in
**					    iidbdb.
**
** Description:
**      Takes the name of the catalog, and creates it.
**
** Inputs:
**      catalog_name                    Name of iidbdb catalog.
**
** Outputs:
**	Returns:
**	    E_DB_OK
**	Exceptions:
**	    None.
**
** Side Effects:
**	    none.
**
** History:
**      23-apr-1989 (alexc)
**          Creation.
**	08-mar-91 (teresa)
**	    moved here from ducreate becuase it is also used by convto60.
**	20-sep-91 (teresa)
**	    pick up ingres63p 263287 from 6.4:
**	      15-aug-91 (teresa)
**		add call to duc_star_cdbinfo() to make standard catalog 
**		iistar_cdbinfo
**	27-March-1994 (jpk)
**		move rebuilding of index so createdb and upgradedb get
**		same results.
[@history_template@]...
*/
duc_6create_iidbdb_cats(catalog_name)
char	*catalog_name;
{
    if (!STcompare(catalog_name, "iistar_cdbs"))
    {
/* # line 1184 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iistar_cdbs(ddb_name=char(32), ddb_owner=char(32), cdb_name=ch\
ar(32), cdb_owner=char(32), cdb_node=char(32), cdb_dbms=char(32), sche\
me_desc=char(32), create_date=char(25), original=char(8), cdb_id=i4, c\
db_capability=i4 not null)");
    IIsyncup((char *)0,0);
  }
/* # line 1196 "ducommon.qsc" */	/* index */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"index unique on iistar_cdbs=iicdbid_idx(cdb_id)with structure=hash");
    IIsyncup((char *)0,0);
  }
/* # line 1199 "ducommon.qsc" */	/* host code */
    }
    if (!STcompare(catalog_name, "iiddb_netcost"))
    {
/* # line 1203 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iiddb_netcost(net_src=char(32), net_dest=char(32), net_cost=f8\
, net_exp1=f8, net_exp2=f8)");
    IIsyncup((char *)0,0);
  }
/* # line 1210 "ducommon.qsc" */	/* host code */
    }
    if (!STcompare(catalog_name, "iiddb_nodecosts"))
    {
/* # line 1214 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iiddb_nodecosts(cpu_name=char(32), cpu_cost=f8, dio_cost=f8, c\
reate_cost=f8, page_size=i4, block_read=i4, sort_size=i4, cache_size=i\
4, cpu_exp0=i4, cpu_exp1=f8, cpu_exp2=f8)");
    IIsyncup((char *)0,0);
  }
/* # line 1227 "ducommon.qsc" */	/* host code */
    }
    return(E_DU_OK);
}
/*{
**  Name: duc_cpy_usrf()	-   load "users" file entries into "user"
**				    table of dbdb.
**
**  Description:
**	This routine opens the "users" file in ING_UTILITY and reads in
**	the entries appending them to the "user" table of the dbdb.  This
**	is done when creating the dbdb.  
**	  Note 1:   This routine assumes that the "dbdb" database has already
**		    been opened.
**	  Note 2:   It probably makes sense to move this functionality to
**		    finddbs.
**
**  Inputs:
**      duc_errcb                       Ptr to DU_ERRCB
**	duc_dbenv			ptr to DUC_DBENV
**
**  Outputs:
**      *duc_errcb			If an error occurs, this structure
**					will be set by a call to du_error().
**	
**	Returns:
**	    E_DU_OK			Completed successfully.
**	    E_DU3170_BAD_USRF_OPEN	The "users" file could not be opened.
**	    E_DU3172_BAD_USRF_RECORD	There was a bad users record in the
**					"users" file.
**	Exceptions:
**	    none
**
**  Side Effects:
**	    Opens and closes the "users" file in the ING_UTILITY
**	    location.  Updates the "user" system catalog in the database,
**	    "dbdb".
**
**  History:
**      09-Oct-86 (ericj)
**          Initial creation.
**	02-apr-91 (teresa)
**	    moved to ducommon.qsc from ducreate.qsc
**	7-Feb-1993 (jpk)
**	    Stop reading in users file.  Add dbenv parameter to pass username
**	    so on VMS can drop a tuple into iiuser for the user who is
**	    running createdb -S iidbdb.  If you have priv enough to create
**	    the iidbdb, you have priv enough to be listed in it.  The question
**	    of whether you have priv enough to create the iidbdb, however,
**	    is not, should, and will be addressed.
**	09-nov-93 (rblumer)
**	    Use case-insensitive comparison for ingres, so that don't add 
**	    ingres user in two different cases in FIPS iidbdb  (B55960).
**	    Use case-translated version of name to pass to cui_idunorm.
**	    When creating iidbdb with real_user_case on UNIX, need to add root
**	    as a delimited username (since root is lower-cased and regid_case
**	    could be upper).
**	    Also return generic 'bad user' error if cui_idunorm gets an error.
**	20-Feb-1994 (jpk)
**	    Fix bugs 58905, 57759, and 58347: usernames of form ingresxx,
**	    e.g. ingres65, are perfectly valid and should be added as
**	    ingres users when they createdb the iidbdb.
**      01-oct-2002 (zhahu02)
**          Updated to be compiled fine. (b108842)
**	21-nov-2002 (somsa01)
**	    On Windows, create "administrator".
**	28-jan-2004 (somsa01)
**	    Only manually create the "Administrator" user on Windows if
**	    PMload() fails. Normally we take the users which are in
**	    config.dat and add them to the iiuser table.
*/
DU_STATUS
  duc_cpy_usrf(duc_errcb, duc_dbenv)
   DU_ERROR		*duc_errcb;
  DU_ENV *duc_dbenv;
  {
    FILE		*usrf;
    char		ubuf[DU_MAXLINE + 1];
    DU_STATUS		du_status = E_DU_OK;
    DU_USR usr_rec;
    i4 row_cnt;
    i4 u_status = 0;
    STATUS		du_brk_usr();
    char		unnormal_user[2*DB_MAXNAME+2];
    char		*username;
    STATUS              status;
    PM_SCAN_REC         state;
    /* XXX get rid of users file */
    if (du_add_privd_user("$ingres") < 0)
    {
	du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iiuser", 0, DU_DBDBNAME);
	return du_status;
    }
    /* add username of install user as a privileged user */
    {
	char		unnormal_user[2*DB_MAXNAME+3];
	u_i4		unnormal_len;
	u_i4		normal_len;
	u_i4	mode = CUI_ID_DLM | CUI_ID_STRIP;
	DB_STATUS	status;
	DB_ERROR	error;
	/*
	** We must "unnormalize" the username!
	*/
	unnormal_len = sizeof(unnormal_user)-1;
	normal_len = STlength(duc_dbenv->du_dba_target);
	status = cui_idunorm((u_char *) duc_dbenv->du_dba_target, &normal_len,
			     (u_char *) unnormal_user, &unnormal_len,
			     mode, &error);
	if (DB_FAILURE_MACRO(status))
	{
	    /* since can't look up DBMS error returned in error.err_code,
	    ** use default 'bad username' error.
	    */
	    du_status = du_error(duc_errcb, E_DU3000_BAD_USRNAME, 
				 2, 0, duc_dbenv->du_dba_target);
	    return du_status;
	}
	unnormal_user[unnormal_len] = EOS;
	if (du_add_privd_user(unnormal_user) < 0)
	{
	    du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iiuser", 0, DU_DBDBNAME);
	    return du_status;
	}
    }
    /* Add any privileged users specified in CONFIG.DAT as Ingres users */
    PMinit();
    if (PMload( (LOCATION *) NULL, (PM_ERR_FUNC *) NULL) != OK)
    {
       /* Just add SYSTEM account - like we did before. */
#ifdef VMS
	if (du_add_privd_user("SYSTEM") < 0)
	{
	    du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iiuser", 0, DU_DBDBNAME);
	    return du_status;
	}
#endif
#ifdef UNIX
	/* if real_user_case is mixed,
	** have to delimit root with double quotes
	** or else it will be mapped to regid_case.
	**  (do the same for ingres???  Will that work on VMS???
	**                                              FIX ME rblumer 11/15/93)
	*/
	if (duc_dbenv->du_dbservice & DU_REAL_USER_MIXED)
	    username = ERx("\"root\"");
	else
	    username = ERx("root");
	if (du_add_privd_user(username) < 0)
	{
	    du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iiuser", 0, DU_DBDBNAME);
	    return du_status;
	}
#endif
#ifdef NT_GENERIC
	if (du_add_privd_user("administrator") < 0)
	{
	    du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iiuser", 0, DU_DBDBNAME);
	    return du_status;
	}
#endif
    }
    else
    {
       char *user_list = "ii.$.privileges.user.%";
       char priv_user [2*DB_MAXNAME+2];
       char *sea_str, *entry, *value;
       PMsetDefault( 1, PMhost());
       sea_str = PMexpToRegExp( user_list );
       for (status = PMscan( sea_str, &state, NULL, &entry, &value);
            status == OK;
            status = PMscan( NULL, &state, NULL, &entry, &value))
       {
          username = (char *)PMgetElem( 4, entry );
          if ( (STbcompare(ERx("$ingres"), 0, username, 0, TRUE) != 0) &&
               (STbcompare(duc_dbenv->du_xusrname, 0, username, 0, TRUE) != 0) )
          {
             /* We haven't added this user before - so add it now.
             ** May need to delimit "root".
             */
             if ( (duc_dbenv->du_dbservice & DU_REAL_USER_MIXED) &&
                  (STbcompare(ERx("root"), 0, username, 0, TRUE) == 0) )
             {
                STprintf(unnormal_user, ERx( "\"%s\""), username);
                username = unnormal_user;
             }
             if (du_add_privd_user(username) < 0)
             {
                PMfree();
                du_status = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
                                     0, "iiuser", 0, DU_DBDBNAME);
                return du_status;
             }
          }
       }
    }
    PMfree();
    return E_DU_OK;
  }
/*{
** Name: sqlerr_handler()	-   Error handler for Ingres errors
**				    encountered by SQL statements.
**
** Description:
**	These errors will either be translated to a database utility error,
**	reported and exited, or sent back to the program to handle itself.
**
** Inputs:
**      ing_error                       Createdb error number.
**	errcb				Ptr to executable's error control block
**
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
**	02-apr-91 (teresa)
**	    Created.
**	
[@history_template@]...
*/
DU_STATUS
  sqlerr_handler(errcb, error)
DU_ERROR    *errcb;
i4	    error;
  {
    char *err_msg;
	err_msg = errcb->du_errmsg;
/* # line 1491 "ducommon.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,32,0,err_msg,(char *)"errortext");
    }
/* # line 1492 "ducommon.qsc" */	/* host code */
	SIprintf("%s\n", err_msg);
	errcb->du_status	= E_DU_UERROR;
	errcb->du_ingerr	= error;
	return (E_DU_FATAL);
  }
/*{
** Name: duc_boot_dbdb() -  make and configure dbms catalogs specific to
**			    the "dbdb".
**
** Description:
**        This routine makes and configures DBMS catalogs that are
**	specific to the "database database".
**
**	Note:  The create statements below must correspond to the
**	structures defined in dudbms.qsh.  These structures are used
**	by the DBMS to interpret tuples directly from data pages.
**
** Inputs:
**      duc_dbenv			Control block containing a description
**					of this installation
**	appl_flag			Ptr to a char array to contain the
**					  application flag.
** Outputs:
**	appl_flag			Initialized application flag.
**	Returns:
**	    E_DU_OK			Completed successfully.
**	Exceptions:
**	    none
**
** Side Effects:
**	      The "dbdb" specific dbms catalogs are made and modified.
**
** History:
**	01-apr-91 (teresa)
**		initial creation.
**	10-feb-1993 (jpk)
**	    support createdb over the net.
**	02-jun-1993 (ralph)
**	    DELIM_IDENT:
**	    Pass in info on case translation semantics when creating iidbdb.
**	21-jul-93 (ralph)
**	    DELIM-IDENT:
**	    Add support for DU_REAL_USER_MIXED when creating the iidbdb.
**	    Collapse all case translation flags for iidbdb creation into -G.
*/
DU_STATUS
  duc_boot_dbdb(duc_dbenv, appl_flag)
  DU_ENV *duc_dbenv;
  char *appl_flag;
  {
	char		tmp_buf[DU_APPL_FLAG_MAX];
	/* construct name of form node::database, so 2 for "::" */
    char node_dbname[GCC_L_NODE + DB_MAXNAME + 2 + 1];
    char case_semantics[6];
    char lp64[2];
	    /* Define the default areas for the "dbdb" database */
	    STcopy(ING_CKDIR, duc_dbenv->du_ckploc.du_area);
	    STcopy(ING_DBDIR, duc_dbenv->du_dbloc.du_area);
	    STcopy(ING_JNLDIR, duc_dbenv->du_jnlloc.du_area);
	    STcopy(ING_WORKDIR, duc_dbenv->du_sortloc.du_area);
	    STcopy(ING_DMPDIR,duc_dbenv->du_dmploc.du_area);
	    /* Build the special application flag that will cause the
	    ** server to boot the iiddbdb (make iidbdb.dir, config file and
	    ** core catalogs).  However, do not actually do this until we
	    ** verify that this user is authorized to boot ingres.
	    */
	    CVla((i4) DBA_CREATE_DBDBF1, tmp_buf);
	    (VOID)STpolycat(2, DU_1APPL_FLAG, tmp_buf, appl_flag);
	    /* this ingres connection will automatically create the iidbdb
	    ** default data directory and will create the core catalogs
	    ** NOTE:  be sure that duc_dbenv->dbdbnamePlusService is initialized
	    **	      to the iidbdb name plus any special server type flag
	    **	      (such as /rms for rms gw)
	    */
	    if (STlength(duc_dbenv->du_nodename) > 0)
	    {
		STpolycat(3, duc_dbenv->du_nodename, ERx("::"),
		    duc_dbenv->dbdbnamePlusService, node_dbname);
	    }
	    else
	    {
		STcopy(duc_dbenv->dbdbnamePlusService, node_dbname);
	    }
	    /*
	    ** When creating iidbdb, we pass the case translation masks
	    ** in via the -G and -R flags.  The -G flag specifies the
	    ** case translation semantics for delimited identifiers,
	    ** and the -R flag specifies the case translation semantics
	    ** for regular identifiers.  
	    ** This is only done when creating the iidbdb;  other databases
	    ** are created via iiqef_create_db, and the case translation
	    ** masks are passed in via the dbservice parameter.
	    */
	    case_semantics[0] = '-';
	    case_semantics[1] = 'G';
	    if (duc_dbenv->du_dbservice & DU_DELIM_UPPER)
		case_semantics[2] = 'U';
	    else if (duc_dbenv->du_dbservice & DU_DELIM_MIXED)
		case_semantics[2] = 'M';
	    else
		case_semantics[2] = 'L';
	    if (duc_dbenv->du_dbservice & DU_NAME_UPPER)
		case_semantics[3]= 'U';
	    else
		case_semantics[3]= 'L';
	    if (duc_dbenv->du_dbservice & DU_REAL_USER_MIXED)
		case_semantics[4] = 'M';
	    else if (duc_dbenv->du_dbservice & DU_NAME_UPPER)
		case_semantics[4] = 'U';
	    else
		case_semantics[4] = 'L';	
	    case_semantics[5] = EOS;
/* # line 1617 "ducommon.qsc" */	/* ingres */
    {
      IIingopen(0,appl_flag,DU_1DBA_DBDB_FLAG,case_semantics,node_dbname,
      (char *)0);
    }
/* # line 1619 "ducommon.qsc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"set session isolation level serializable");
      IIsyncup((char *)0,0);
    }
/* # line 1621 "ducommon.qsc" */	/* host code */
            /*
            ** Check to see if this is a 64-bit server
            */
/* # line 1625 "ducommon.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(lp64=dbmsinfo(\"lp64\"))");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,1,lp64);
        if (IIerrtest() != 0) goto IIrtB1;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE1:;
    }
/* # line 1627 "ducommon.qsc" */	/* host code */
            if (lp64[0] == 'Y')
                duc_dbenv->du_dbservice |= DU_LP64;
	    duc_dbenv->du_dbopen =   TRUE;
	    return E_DU_OK;
  }
/*{
** Name: duc_db_boot() -  make and configure dbms catalogs for a user db
**
** Description:
**        This routine makes and configures DBMS catalogs that are
**	required by the DBMS server for any user dtabase.
**
** Inputs:
**      duc_dbenv			Control block containing a description
**					of this installation
**	need_dir_flg			Controls whether or not DMBS server
**					  will create the DB default directory
**					  or will expect it to already exist.
**					  IF called from createdb, set to TRUE
**					  IF called from convto60, set to FALSE
** Outputs:
**	Returns:
**	    E_DU_OK			Completed successfully.
**	Exceptions:
**	    none
**
** Side Effects:
**	      The dbms catalogs are made and modified.
**
** History:
**	01-apr-91 (teresa)
**		initial creation.
**	17-jan-91 (teresa)
**		put dbservice into iiqef_create_db instead of having to call
**		iiqef_alter_db.
**	22-July-1993 (jpk)
**		set value of dbservice correctly.
**	09-nov-93 (rblumer)
**		fill in new translated & untranslated owner name parameters.
**	 4-mar-94 (robf)
**              Ensure createdb privilege is active.
**	15-nov-94 (forky01)
**		remove unused parms to make it cleaner to call.  This is
**		necessary because of a fix for BUG #53072, which requires
**		this to be called from duc_get_dbid, to create the cdb in
**		iidatabase prior to appending the ddb info to iistar_cdbs
**	24-mar-2001 (stephenb)
**	    	Add support for Unicode collation.
*/
DU_STATUS
duc_db_boot(duc_errcb, duc_dbenv, need_dir_flg)
DU_ERROR	*duc_errcb;
DU_ENV		*duc_dbenv;
i4		need_dir_flg;
{
  char dbname[DB_MAXNAME+4];
  i4 access;
  i4 service;
  char db_loc[DB_MAXNAME+4];
  char jnl_loc[DB_MAXNAME+4];
  char ckp_loc[DB_MAXNAME+4];
  char dmp_loc[DB_MAXNAME+4];
  char srt_loc[DB_MAXNAME+4];
  char collation[DB_MAXNAME+4];
  char ucollation[DB_MAXNAME+4];
  char trans_owner_name[DB_MAXNAME+4];
  char untrans_owner_name[DB_MAXNAME+4];
  i4 needdir;
  i4 pcnt;
	access  = duc_dbenv->du_access;
	service = duc_dbenv->du_dbservice;
	(VOID) STcopy (duc_dbenv->du_dbname, dbname);
	(VOID) STcopy (duc_dbenv->du_dbloc.du_locname,   db_loc);
	(VOID) STcopy (duc_dbenv->du_jnlloc.du_locname,  jnl_loc);
	(VOID) STcopy (duc_dbenv->du_ckploc.du_locname,  ckp_loc);
	(VOID) STcopy (duc_dbenv->du_dmploc.du_locname,  dmp_loc);
	(VOID) STcopy (duc_dbenv->du_sortloc.du_locname, srt_loc);
	(VOID) STcopy (duc_dbenv->du_collation,  collation);
	(VOID) STcopy (duc_dbenv->du_ucollation, ucollation);
	(VOID) STcopy (duc_dbenv->du_dba_target, trans_owner_name);
	(VOID) STcopy (duc_dbenv->du_xusrname,   untrans_owner_name);
	needdir = need_dir_flg;
/* # line 1739 "ducommon.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session add privileges(createdb)");
    IIsyncup((char *)0,0);
  }
/* # line 1740 "ducommon.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbp_parameter_count from iiprocedure where lowercase(dbp_name)\
='iiqef_create_db'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&pcnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1742 "ducommon.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    /* we were unable to execute the internal procedure */
	    sqlerr_handler(duc_errcb,E_DU200C_BAD_IPROC_EXECUTE);
		return (E_DU_FATAL);
	}
	if (pcnt == 13)
	{
/* # line 1750 "ducommon.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"iiQEF_create_db");
    IILQprvProcValio((char *)"dbname",0,(short *)0,1,32,0,dbname);
    IILQprvProcValio((char *)"db_loc_name",0,(short *)0,1,32,0,db_loc);
    IILQprvProcValio((char *)"jnl_loc_name",0,(short *)0,1,32,0,jnl_loc);
    IILQprvProcValio((char *)"ckp_loc_name",0,(short *)0,1,32,0,ckp_loc);
    IILQprvProcValio((char *)"dmp_loc_name",0,(short *)0,1,32,0,dmp_loc);
    IILQprvProcValio((char *)"srt_loc_name",0,(short *)0,1,32,0,srt_loc);
    IILQprvProcValio((char *)"db_access",0,(short *)0,1,30,4,&access);
    IILQprvProcValio((char *)"collation",0,(short *)0,1,32,0,collation);
    IILQprvProcValio((char *)"need_dbdir_flg",0,(short *)0,1,30,4,&needdir);
    IILQprvProcValio((char *)"db_service",0,(short *)0,1,30,4,&service);
    IILQprvProcValio((char *)"translated_owner_name",0,(short *)0,1,32,0,
    trans_owner_name);
    IILQprvProcValio((char *)"untranslated_owner_name",0,(short *)0,1,32,0,
    untrans_owner_name);
    IILQprvProcValio((char *)"ucollation",0,(short *)0,1,32,0,ucollation);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 1764 "ducommon.sc" */	/* host code */
	}
	else
	{
/* # line 1767 "ducommon.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"iiQEF_create_db");
    IILQprvProcValio((char *)"dbname",0,(short *)0,1,32,0,dbname);
    IILQprvProcValio((char *)"db_loc_name",0,(short *)0,1,32,0,db_loc);
    IILQprvProcValio((char *)"jnl_loc_name",0,(short *)0,1,32,0,jnl_loc);
    IILQprvProcValio((char *)"ckp_loc_name",0,(short *)0,1,32,0,ckp_loc);
    IILQprvProcValio((char *)"dmp_loc_name",0,(short *)0,1,32,0,dmp_loc);
    IILQprvProcValio((char *)"srt_loc_name",0,(short *)0,1,32,0,srt_loc);
    IILQprvProcValio((char *)"db_access",0,(short *)0,1,30,4,&access);
    IILQprvProcValio((char *)"collation",0,(short *)0,1,32,0,collation);
    IILQprvProcValio((char *)"need_dbdir_flg",0,(short *)0,1,30,4,&needdir);
    IILQprvProcValio((char *)"db_service",0,(short *)0,1,30,4,&service);
    IILQprvProcValio((char *)"translated_owner_name",0,(short *)0,1,32,0,
    trans_owner_name);
    IILQprvProcValio((char *)"untranslated_owner_name",0,(short *)0,1,32,0,
    untrans_owner_name);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 1780 "ducommon.sc" */	/* host code */
	}
	if (sqlca.sqlcode < 0)
	{
	    /* we were unable to execute the internal procedure */
	    sqlerr_handler(duc_errcb,E_DU200C_BAD_IPROC_EXECUTE);
		return (E_DU_FATAL);
	}
	return E_DU_OK;
}
/*{
** Name:  duc_iidbms_comment() -- build catalog iidbms_comment
**
** Description:
**        This routine makes the catalog iidbms_comment.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	11-oct-91 (teresa)
**		initial creation.
*/
VOID
duc_iidbms_comment()
{
/* # line 1815 "ducommon.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iidbms_comment(comtabbase integer not null, comtabidx in\
teger not null, comattid smallint not null, comtype smallint not null,\
 short_remark char(60)not null, text_sequence smallint not null, long_\
remark varchar(1600)not null)");
    IIsyncup((char *)0,0);
  }
/* # line 1825 "ducommon.sc" */	/* host code */
}
/*{
** Name:  duc_iisynonym() -- build catalog iisynonym 
**
** Description:
**        This routine makes the catalog iisynonym .
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	11-oct-91 (teresa)
**		initial creation.
**	15-oct-93 (andre)
**		added synid and synidx
*/
VOID
duc_iisynonym()
{
/* # line 1854 "ducommon.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iisynonym(synonym_name char(32)not null, synonym_owner c\
har(32)not null, syntabbase integer not null, syntabidx integer not nu\
ll, synid integer not null, synidx integer not null)");
    IIsyncup((char *)0,0);
  }
/* # line 1863 "ducommon.sc" */	/* host code */
}
/*{
** Name:  duc_iipriv() -- build catalog IIPRIV
**
** Description:
**        Create catalog IIPRIV.
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-may-92 (andre)
**	    initial creation.
**	16-sep-92 (andre)
**	    added prv_flags to IIPRIV
**	7-Sep-04 (schka24)
**	    Added bitmap cols for 1024 columns, got missed!
**	10-Sep-2004 (schka24)
**	    Also make them with default so that upgradedb can be lazy.
*/
VOID
duc_iipriv()
{
/* # line 1896 "ducommon.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iipriv(d_obj_id integer not null not default, d_obj_idx \
integer not null not default, d_priv_number smallint not null with def\
ault, d_obj_type smallint not null with default, i_obj_id integer not \
null with default, i_obj_idx integer not null ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"with default, i_priv integer not null with default, i_priv_grantee ch\
ar(32)not null with default, prv_flags integer not null with default, \
i_priv_map1 integer not null with default, i_priv_map2 integer not nul\
l with default, i_priv_map3 integer not null ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"with default, i_priv_map4 integer not null with default, i_priv_map5 \
integer not null with default, i_priv_map6 integer not null with defau\
lt, i_priv_map7 integer not null with default, i_priv_map8 integer not\
 null with default, i_priv_map9 integer not ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"null with default, i_priv_mapa integer not null with default, i_priv_\
map11 integer not null with default, i_priv_map12 integer not null wit\
h default, i_priv_map13 integer not null with default, i_priv_map14 in\
teger not null with default, i_priv_map15 ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"integer not null with default, i_priv_map16 integer not null with def\
ault, i_priv_map17 integer not null with default, i_priv_map18 integer\
 not null with default, i_priv_map19 integer not null with default, i_\
priv_map20 integer not null with default, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"i_priv_map21 integer not null with default, i_priv_map22 integer not \
null with default, i_priv_map23 integer not null with default, i_priv_\
map24 integer not null with default, i_priv_map25 integer not null wit\
h default, i_priv_map26 integer not null with ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"default, i_priv_map27 integer not null with default, i_priv_map28 int\
eger not null with default, i_priv_map29 integer not null with default\
, i_priv_map30 integer not null with default, i_priv_map31 integer not\
 null with default, i_priv_map32 integer not ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"null with default, i_priv_map33 integer not null with default)");
    IIsyncup((char *)0,0);
  }
/* # line 1941 "ducommon.sc" */	/* host code */
}
/*{
** Name:  duc_iistatistics() -- build system catalog IISTATISTICS
**
** Description:
**        Create catalog IISTATISTICS
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-jan-93 (jpk)
**	    initial creation.  Encapsulated from duc_dbms_bld()
**	    so that 6.5 upgradedb can call it.
*/
VOID
duc_iistatistics()
{
/* # line 1955 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iistatistics(stabbase=i4, stabindex=i4, snunique=f4, sreptfact\
=f4, snull=f4, satno=i2, snumcells=i2, sdomain=i2, scomplete=i1, suniq\
ue=i1, sdate=ingresdate, sversion=char(8), shistlength=i2)");
    IIsyncup((char *)0,0);
  }
/* # line 1959 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iihistogram() -- build system catalog IIHISTOGRAM
**
** Description:
**        Create catalog IIHISTOGRAM
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	10-nov-95 (nick)
**	    initial creation.
**	13-aug-2007 (dougi)
**	    Explicitly declare histogram column as BYTE (since that's really
**	    what it is).
*/
VOID
duc_iihistogram()
{
/* # line 1989 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iihistogram(htabbase=i4, htabindex=i4, hatno=i2, hsequence=i2,\
 histogram=byte(228))");
    IIsyncup((char *)0,0);
  }
/* # line 1991 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iicolcompare() -- build system catalog IICOLCOMPARE
**
** Description:
**        Create catalog IICOLCOMPARE
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	3-jan-06 (dougi) 
**	    Created.
*/
VOID
duc_iicolcompare()
{
/* # line 2018 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iicolcompare(ctabbase1=i4, ctabbase2=i4, ctabidx1=i4, ctabidx2\
=i4, ccolcount=i4, catno11=i2, catno12=i2, catno13=i2, catno14=i2, cat\
no21=i2, catno22=i2, catno23=i2, catno24=i2, cltcount=f4, ceqcount=f4,\
 cgtcount=f4)");
    IIsyncup((char *)0,0);
  }
/* # line 2023 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iidepends() -- build system catalog IIDEPENDS
**
** Description:
**        Create catalog IIDEPENDS
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	28-Feb-93 (jpk)
**	    initial creation.  Encapsulated from duc_dbms_bld()
**	    so that 6.5 upgradedb can call it.
**
*/
VOID
duc_iidbdepends()
{
/* # line 2052 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidbdepends(inid1=i4, inid2=i4, itype=i4, i_qid=i4, qid=i4, de\
id1=i4, deid2=i4, dtype=i4)");
    IIsyncup((char *)0,0);
  }
/* # line 2054 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_modify_iidepends() -- modify catalog IIDEPENDS back to btree
**
** Description:
**        modify the system catalog IIDEPENDS back to btree structure
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-jan-93 (jpk)
**	    initial creation.  Encapsulated from duc_dbms_bld()
**	    so that 6.5 upgradedb can call it.
*/
VOID
duc_modify_iidbdepends()
{
/* # line 2082 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidbdepends to btree on inid1, inid2, itype, i_qid with fillfa\
ctor=100");
    IIsyncup((char *)0,0);
  }
/* # line 2085 "ducommon.qsc" */	/* index */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"index on iidbdepends=iixdbdepends(deid1, deid2, dtype, qid)with struc\
ture=btree");
    IIsyncup((char *)0,0);
  }
/* # line 2087 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iidd_stats() -- build catalog IIDD_STATS
**
** Description:
**        Create catalog IIDD_STATS
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-jan-93 (jpk)
**	    initial creation.  Encapsulated from duc_1create_iidd_catalogs()
**	    so that 6.5 upgradedb can call it.
*/
VOID
duc_iidd_stats()
{
/* # line 2115 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_stats(table_name=char(32), table_owner=char(32), column_n\
ame=char(32), create_date=char(25), num_unique=f4, rept_factor=f4, has\
_unique=char(8), pct_nulls=f4, num_cells=i2, column_domain=i2, is_comp\
lete=char(8), stat_version=char(8), ");
    IIwritio(0,(short *)0,1,32,0,(char *)"hist_data_length=i2)");
    IIsyncup((char *)0,0);
  }
/* # line 2128 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iidevices() -- build catalog IIDEVICES
**
** Description:
**        Create catalog IIDEVICES
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-jan-93 (jpk)
**	    initial creation.  Encapsulated from duc_dbms_bld()
**	    so that 6.5 upgradedb can call it.
*/
VOID
duc_iidevices()
{
/* # line 2156 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidevices(devrelid=i4, devrelidx=i4, devrelocid=i4, devloc=cha\
r(32))");
    IIsyncup((char *)0,0);
  }
/* # line 2158 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_modify_iidevices() -- modify catalog IIDEVICES back to hash
**
** Description:
**        modify the system catalog IIDEVICES back to hash structure
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-jan-93 (jpk)
**	    initial creation.  Encapsulated from duc_dbms_bld()
**	    so that 6.5 upgradedb can call it.
*/
VOID
duc_modify_iidevices()
{
/* # line 2186 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidevices to hash on devrelid, devrelidx");
    IIsyncup((char *)0,0);
  }
/* # line 2187 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_modify_iistatistics() -- modify catalog IISTATISTICS back to hash
**
** Description:
**        modify the system catalog IISTATISTICS back to hash structure
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-jan-93 (jpk)
**	    initial creation.  Encapsulated from duc_dbms_bld()
**	    so that 6.5 upgradedb can call it.
*/
VOID
duc_modify_iistatistics()
{
/* # line 2215 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iistatistics to hash on stabbase, stabindex where minpages=8");
    IIsyncup((char *)0,0);
  }
/* # line 2217 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_modify_iihistogram() -- modify catalog IIHISTOGRAM back to hash
**
** Description:
**        modify the system catalog IIHISTOGRAM back to hash structure
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	10-nov-95 (nick)
**	    initial creation.
*/
VOID
duc_modify_iihistogram()
{
/* # line 2244 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iihistogram to hash on htabbase, htabindex where minpages=8");
    IIsyncup((char *)0,0);
  }
/* # line 2246 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_modify_iicolcompare() -- modify catalog IICOLCOMPARE back to hash
**
** Description:
**        modify the system catalog IICOLCOMPARE back to hash structure
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	3-jan-06 (dougi)
**	    Creation.
*/
VOID
duc_modify_iicolcompare()
{
/* # line 2273 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iicolcompare to hash on ctabbase1, ctabidx1 where minpages=8");
    IIsyncup((char *)0,0);
  }
/* # line 2275 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_modify_iidd_stats() -- modify catalog IIDD_STATS back to hash
**
** Description:
**        modify the system catalog IIDD_STATS back to hash structure
**
** Inputs:
**      none
** Outputs:
**      none
** Returns:
**      none
** Exceptions:
**      none
**
** Side Effects:
**      none
**
** History:
**      10-Nov-95 (prida01)
**          initial creation.  
**          so that 6.5 upgradedb can call it.
*/
VOID
duc_modify_iidd_stats()
{
/* # line 2303 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_stats to hash on table_name, table_owner where minpages=8");
    IIsyncup((char *)0,0);
  }
/* # line 2305 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_modify_iidatabase() -- modify catalog IIDATABASE back to hash
**
** Description:
**        modify the system catalog IIDATABASE back to hash structure
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-jan-93 (jpk)
**	    initial creation.  Encapsulated from duc_1makedbdb
**	    so that 6.5 upgradedb can call it.
**	27-March-1994 (jpk)
**	    further encapsulation: rebuilding index here,
**	    not inline in the caller, so that upgradedb gets it too.
*/
VOID
duc_modify_iidatabase()
{
/* # line 2336 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidatabase to hash unique on name where minpages=5");
    IIsyncup((char *)0,0);
  }
/* # line 2338 "ducommon.qsc" */	/* index */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"index unique on iidatabase=iidbid_idx(db_id)with structure=hash");
    IIsyncup((char *)0,0);
  }
/* # line 2340 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_modify_iilocations() -- modify catalog IILOCATIONS back to hash
**
** Description:
**        modify the system catalog IILOCATIONS back to hash structure
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-jan-93 (jpk)
**	    initial creation.  Encapsulated from duc_1makedbdb
**	    so that 6.5 upgradedb can call it.
*/
VOID
duc_modify_iilocations()
{
/* # line 2368 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iilocations to hash unique on lname where minpages=4");
    IIsyncup((char *)0,0);
  }
/* # line 2370 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_modify_iiextend() -- modify catalog IIEXTEND back to hash
**
** Description:
**        modify the system catalog IIEXTEND back to hash structure
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-jan-93 (jpk)
**	    initial creation.  Encapsulated from duc_1makedbdb
**	    so that 6.5 upgradedb can call it.
*/
VOID
duc_modify_iiextend()
{
/* # line 2398 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iiextend to hash on dname where minpages=4");
    IIsyncup((char *)0,0);
  }
/* # line 2400 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_modify_iirole() -- modify catalog IIROLE back to hash
**
** Description:
**        modify the system catalog IIROLE back to hash structure
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-jan-93 (jpk)
**	    initial creation.  Encapsulated from duc_1makedbdb
**	    so that 6.5 upgradedb can call it.
*/
VOID
duc_modify_iirole()
{
/* # line 2428 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iirole to hash unique on roleid where minpages=2");
    IIsyncup((char *)0,0);
  }
/* # line 2430 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_modify_iiusergroup() -- modify catalog IIUSERGROUP back to hash
**
** Description:
**        modify the system catalog IIUSERGROUP back to hash structure
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-jan-93 (jpk)
**	    initial creation.  Encapsulated from duc_1makedbdb
**	    so that 6.5 upgradedb can call it.
*/
VOID
duc_modify_iiusergroup()
{
	/*
	** Note: iiusergroup should remain btree since QEF will
	** perform partial searches that are not allowed against
	** hashed tables
	*/
/* # line 2465 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iiusergroup to btree unique on groupid, groupmem");
    IIsyncup((char *)0,0);
  }
/* # line 2466 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iiextend() -- build catalog IIEXTEND
**
** Description:
**        Create catalog IIEXTEND
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-jan-93 (jpk)
**	    initial creation.  Encapsulated from duc_1makedbdb()
**	    so that 6.5 upgradedb can call it.
**	02-Apr-2001 (jenjo02)
**	    Added rawstart.
**	11-May-2001 (jenjo02)
**	    Moved rawblocks to here from iilocation.
*/
VOID
duc_iiextend()
{
/* # line 2499 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iiextend(lname=varchar(32), dname=varchar(32), status=i4, raws\
tart=i4, rawblocks=i4)");
    IIsyncup((char *)0,0);
  }
/* # line 2501 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iievent() -- build catalog IIEVENT
**
** Description:
**        Create catalog IIEVENT
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-jan-93 (jpk)
**	    initial creation.  Encapsulated from duc_dbms_bld()
**	    so that 6.5 upgradedb can call it.
**	21-jun-93 (robf)
**	    Added event_secid
**
*/
VOID
duc_iievent()
{
/* # line 2532 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iievent(event_name=char(32), event_owner=char(32), event_creat\
e=ingresdate, event_type=i4, event_idbase=i4, event_idx=i4, event_qryi\
d1=i4, event_qryid2=i4, event_free=char(8))");
    IIsyncup((char *)0,0);
  }
/* # line 2537 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iidatabase() -- build catalog IIDATABASE
**
** Description:
**        Create catalog IIDATABASE
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-jan-93 (jpk)
**	    initial creation.  Encapsulated from duc_1makedbdb
**	    so that 6.5 upgradedb can call it.
*/
VOID
duc_iidatabase()
{
/* # line 2566 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidatabase(name=char(32), own=char(32), dbdev=char(32), ckpdev\
=char(32), jnldev=char(32), sortdev=char(32), access=i4, dbservice=i4,\
 dbcmptlvl=char(4), dbcmptminor=i4, db_id=i4, dmpdev=char(32), dbfree=\
char(8))");
    IIsyncup((char *)0,0);
  }
/* # line 2579 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iilocations() -- build catalog IILOCATIONS
**
** Description:
**        Create catalog IILOCATIONS
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-jan-93 (jpk)
**	    initial creation.  Encapsulated from duc_1makedbdb()
**	    so that 6.5 upgradedb can call it.
**	11-mar-1999 (nanpr01)
**	    Support raw locations.
**	11-May-2001 (jenjo02)
**	    replaced rawblocks with rawpct.
*/
VOID
duc_iilocations()
{
/* # line 2611 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iilocations(status=i4, lname=varchar(32), area=varchar(128), f\
ree=char(8), rawpct=i4)");
    IIsyncup((char *)0,0);
  }
/* # line 2613 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iirole() -- build catalog IIROLE
**
** Description:
**        Create catalog IIROLE
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-jan-93 (jpk)
**	    initial creation.  Encapsulated from duc_1makedbdb()
**	    so that 6.5 upgradedb can call it.
**	24-jun-93  (robf)
**	    Added secure attributes
**	7-mar-94 (robf)
**          Added rolestatus, roleflags, 
**	    renamed roleeffsecid to rolesecid
*/
VOID
duc_iirole()
{
/* # line 2646 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iirole(roleid=char(32), rolepass=char(24), reserve1=char(16), \
rolestatus=i4, roleflags=i4, reserve=char(8))");
    IIsyncup((char *)0,0);
  }
/* # line 2652 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iiusergroup() -- build catalog IIUSERGROUP
**
** Description:
**        Create catalog IIUSERGROUP
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-jan-93 (jpk)
**	    initial creation.  Encapsulated from duc_1makedbdb()
**	    so that 6.5 upgradedb can call it.
*/
VOID
duc_iiusergroup()
{
/* # line 2680 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iiusergroup(groupid=char(32), groupmem=char(32), reserve=char(\
32))");
    IIsyncup((char *)0,0);
  }
/* # line 2683 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iiuser() -- build catalog iiuser
**
** Description:
**        This routine makes the catalog iiuser.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	11-oct-91 (teresa)
**		initial creation.
**	2-jun-92 (robf)
**		Add new iiuser attributes
**	19-oct-93 (robf)
**              Add user_priv attribute.
*/
VOID
  duc_iiuser()
  {
/* # line 2714 "ducommon.qsc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"create iiuser(name=char(32), gid=i2, mid=i2, status=i4, default_group\
=char(32), password=char(24), reserve=char(8), profile_name=char(32), \
default_priv=integer4, expire_date=ingresdate, flags_mask=integer4, us\
er_priv=integer4)");
      IIsyncup((char *)0,0);
    }
  }
/* # line 2724 "ducommon.qsc" */	/* host code */
/*{
** Name:  duc_iisequence() -- build catalog IISEQUENCE
**
** Description:
**        Create catalog IISEQUENCE
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	5-mar-02 (inkdo01)
**	    Written.
**
*/
void
duc_iisequence()
{
/* # line 2750 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iisequence(seq_name=char(32), seq_owner=char(32), seq_create=i\
ngresdate, seq_modify=ingresdate, seq_idbase=i4, seq_idx=i4, seq_type=\
i2, seq_length=i2, seq_prec=i4, seq_start_dec=decimal(31), seq_start_i\
nt=i4, seq_incr_dec=decimal(31), seq_incr_int=");
    IIwritio(0,(short *)0,1,32,0,(char *)
"i4, seq_next_dec=decimal(31), seq_next_int=i4, seq_min_dec=decimal(31\
), seq_min_int=i4, seq_max_dec=decimal(31), seq_max_int=i4, seq_cache=\
i4, seq_flag=i4, seq_free=char(8))");
    IIsyncup((char *)0,0);
  }
/* # line 2762 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iipartcats() -- create partitioned table catalogs.
**
** Description:
**	Create the partitioned table catalogs.  There are four of
**	these:  iidistscheme, iidistcol, iidistval, and iipartname.
**	Nothing special about any of them...
**
** Inputs:
**	None
** Outputs:
**	None
** Returns:
**	None
** Exceptions:
**	None
**
** History:
**	19-Dec-2003 (schka24)
**	    New partitioned table catalogs.
**	29-Jan-2004 (schka24)
**	    Ditch iipartition, didn't need it.
**
*/
void
duc_iipartcats()
{
/* # line 2790 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidistscheme(mastid=i4 not null not default, mastidx=i4 not nu\
ll not default, levelno=i2 not null not default, ncols=i2 not null wit\
h default, nparts=i2 not null with default, distrule=i2 not null not d\
efault)");
    IIsyncup((char *)0,0);
  }
/* # line 2797 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidistcol(mastid=i4 not null not default, mastidx=i4 not null \
not default, levelno=i2 not null not default, colseq=i2 not null not d\
efault, attid=i2 not null not default)");
    IIsyncup((char *)0,0);
  }
/* # line 2803 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidistval(mastid=i4 not null not default, mastidx=i4 not null \
not default, levelno=i2 not null not default, valseq=i2 not null not d\
efault, colseq=i2 not null with default, partseq=i2 not null not defau\
lt, oper=i1 not null not default, distvalue=");
    IIwritio(0,(short *)0,1,32,0,(char *)"varchar(1500)with null)");
    IIsyncup((char *)0,0);
  }
/* # line 2812 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iipartname(mastid=i4 not null not default, mastidx=i4 not null\
 not default, levelno=i2 not null not default, partseq=i2 not null wit\
h default, partname=char(32)not null with default)");
    IIsyncup((char *)0,0);
  }
/* # line 2818 "ducommon.qsc" */	/* host code */
} /* duc_iipartcats */
/*{
** Name:  duc_modify_iipartcats() -- modify partitioned table catalogs.
**
** Description:
**	Modify the partitioned table catalogs.  There are four of
**	these:  iidistscheme, iidistcol, iidistval, and iipartname.
**	Nothing special about any of them...
**	They are all btree because no compelling reasons exist to
**	make them anything else;  generally all the rows for a given
**	master table are sucked in by rdf.
**
** Inputs:
**	None
** Outputs:
**	None
** Returns:
**	None
** Exceptions:
**	None
**
** History:
**	19-Dec-2003 (schka24)
**	    New partitioned table catalogs.
**	31-Dec-2003 (schka24)
**	    Forgot to make iidistval compressed, fix.
**	29-Jan-2004 (schka24)
**	    iipartition unused, get rid of it.
**
*/
void
duc_modify_iipartcats()
{
/* # line 2852 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidistscheme to btree unique on mastid, mastidx, levelno with \
fillfactor=100");
    IIsyncup((char *)0,0);
  }
/* # line 2855 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidistcol to btree unique on mastid, mastidx, levelno, colseq \
with fillfactor=100");
    IIsyncup((char *)0,0);
  }
/* # line 2858 "ducommon.qsc" */	/* host code */
/* iidistval has a large, mostly empty value column -- compress it */
/* # line 2859 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidistval to btree unique on mastid, mastidx, levelno, valseq,\
 colseq with fillfactor=100, compression=(data)");
    IIsyncup((char *)0,0);
  }
/* # line 2863 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iipartname to btree unique on mastid, mastidx, levelno, partse\
q with fillfactor=100");
    IIsyncup((char *)0,0);
  }
/* # line 2866 "ducommon.qsc" */	/* host code */
} /* duc_modify_iipartcats */
/*{
** Name:  duc_modify_iiprocedure() -- modify iiprocedure and build index on it
**
** Description:
**        Create catalog IIXPROCEDURE.
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	18-jun-92 (andre)
**		initial creation.
**	21-mar-94 (teresa)
**	    Fix bug 59075 -- modify iiprocedure to btree UNIQUE to avoid
**	    race condition where two different threads can create the same
**	    procedure at the same time.
*/
VOID
duc_modify_iiprocedure()
{
/* # line 2897 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iiprocedure to btree unique on dbp_name, dbp_owner with fillfa\
ctor=100");
    IIsyncup((char *)0,0);
  }
/* # line 2899 "ducommon.qsc" */	/* index */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"index on iiprocedure=iixprocedure(dbp_id, dbp_idx)with structure=btre\
e");
    IIsyncup((char *)0,0);
  }
/* # line 2901 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_modify_iievent() -- modify IIEVENT and add index on it
**
** Description:
**        Create catalog IIXEVENT.
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	18-jun-92 (andre)
**		initial creation.
**	31-jan-92 (jpk)
**		modified to both modify and create index
*/
VOID
duc_modify_iievent()
{
/* # line 2930 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iievent to HASH on event_name, event_owner with MINPAGES=30");
    IIsyncup((char *)0,0);
  }
/* # line 2934 "ducommon.qsc" */	/* index */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"index on iievent=iixevent(event_idbase, event_idx)with structure=btre\
e");
    IIsyncup((char *)0,0);
  }
/* # line 2936 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_modify_iisequence() -- modify IISEQUENCE 
**
** Description:
**        Modify catalog IISEQUENCE.
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	5-mar-02 (inkdo01)
**	    Written.
*/
void
duc_modify_iisequence()
{
/* # line 2963 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iisequence to HASH on seq_name, seq_owner with MINPAGES=30");
    IIsyncup((char *)0,0);
  }
/* # line 2966 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iisectype () -- build catalog iisectype
**
** Description:
** Definition for iisectype base table.
**
** This base table will be created and populated in *every* database.
** It is used by the iisecurity_state view to interpret the contents
** of the iisecuritystate base table.  It is also used by the 
** iisecurity_status view to interpret the output from dbmsinfo('secstate').
**
** Each tuple represents a defined security event, a security label, or
** unknown security types, events or labels.  All defined security 
** events are provided by Ingres.  The customer must provide tuples
** for system- or site-specific security labels.
**
** Attribute definitions include:
**
**	sec_type -	The type of security item  represented by this
**			tuple, including:
**				`Event'	  - Security Event 
**				`Unknown' - Unknown type
**
**	sec_name -	If sec_type is `Event', this is the name of the
**			security event represented by the tuple.
**
**	sec_typenum -	Internal representation of sec_type, including:
**				0	- Unknown
**				1	- Event
**
**	sec_namenum -	Internal representation of sec_name.  For tuples
**			with sec_type of `Unknown', this attribute is zero.
**			For tuples with sec_type
**			of event, this attribute contains:
**				0	- Unknown
**				1	- Database
**				2	- Role
**				3	- Procedure
**				4	- Table
**				5	- Location
**				6	- View
**				7	- Row
**				8	- Security
**				9	- User
**				11	- Alarm
**				12	- Rule
**				13	- DbEvent
**				14	- Installation
**				15	- All
**				16	- Resource
**				17	- Query text
**
**	sec_index -	Contains the index (1-based) into the output of
**			dbmsinfo('secstate') for the status of the associated
**			security event.  This zero for unknown security
**			events, or record types which are not security events.
**
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	11-oct-91 (teresa)
**	     initial creation.
**	18-nov-92 (robf)
**		Add INSTALLATION code. Also rework to make table-driven
**		rather than multiple INSERTS.
**	2-jul-93  (robf)
**	     Add RESOURCE, QRYTEXT auditing
**	04-aug-1993 (shailaja)
**		Unnested comments.
*/
VOID
duc_iisectype()
{
  typedef struct {
    char *type;
    char *name;
    int typenum;
    int namenum;
    int secindex;
  } SECTYPE;
  static SECTYPE sectypes[]= {
{"Unknown", "Unknown", 0, 0, 0 },
{"Event",   "Unknown", 1, 0, 0 },
{"Event",   "Database", 1, 1, 2 },
{"Event",   "Role", 	1, 2, 3 },
{"Event",   "Procedure",1, 3, 4 },
{"Event",   "Table",	1, 4, 5 },
{"Event",   "Location",	1, 5, 6 },
{"Event",   "View",	1, 6, 7 },
{"Event",   "Row",	1, 7, 8 },
{"Event",   "Security",	1, 8, 9 },
{"Event",   "User",	1, 9, 10 },
{"Event",   "Alarm",	1, 11,12 },
{"Event",   "Rule",	1, 12,13 },
{"Event",   "DbEvent",	1, 13,14 },
{"Event",   "Installation", 1, 14,15 },
{"Event",   "All",	1, 15,1 },
{"Event",   "Resource",	1, 16,17 },
{"Event",   "Query_text",	1, 17,18 } };
  int rowno;
/* # line 3227 "ducommon.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iisectype(sec_type c16, sec_name c32, sec_typenum i2, se\
c_namenum i2, sec_index i2)");
    IIsyncup((char *)0,0);
  }
/* # line 3233 "ducommon.sc" */	/* host code */
	for ( rowno=0; rowno< sizeof(sectypes)/sizeof(SECTYPE); rowno++)
	{
/* # line 3236 "ducommon.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"ducommon1",6220,22132);
      IIputdomio((short *)0,1,32,0,sectypes[rowno].type);
      IIputdomio((short *)0,1,32,0,sectypes[rowno].name);
      IIputdomio((short *)0,1,30,4,&sectypes[rowno].typenum);
      IIputdomio((short *)0,1,30,4,&sectypes[rowno].namenum);
      IIputdomio((short *)0,1,30,4,&sectypes[rowno].secindex);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"ducommon1",6220,22132);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iisectype(sec_type, sec_name, sec_typenum, sec_namenum, s\
ec_index)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,sectypes[rowno].type);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,sectypes[rowno].name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&sectypes[rowno].typenum);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&sectypes[rowno].namenum);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&sectypes[rowno].secindex);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"ducommon1",6220,22132);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 3243 "ducommon.sc" */	/* host code */
	}
}
/*{
** Name:  duc_iisecuritystate() -- build catalog iisecuritystate
**
** Description:
**        This routine makes the catalog iisecuritystate.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    status - E_DB_OK or error code
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	11-oct-91 (teresa)
**		initial creation.
*/
DU_STATUS
duc_iisecuritystate(duc_errcb)
DU_ERROR	*duc_errcb;
{
  i4 sec_type_var;
  i4 sec_state_var;
  i4 sec_id;
  i4 row_cnt;
	DB_STATUS	stat;
/* # line 3138 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iisecuritystate(type=i4, id=i4, state=i4)");
    IIsyncup((char *)0,0);
  }
/* # line 3143 "ducommon.qsc" */	/* host code */
	/* Print an informational message. */
	if ((du_error(duc_errcb, I_DU0204_MOD_DBDB_CATS, 0))
	    != E_DU_OK
	   )
	    return(duc_errcb->du_status);
        /*
        **  Populate security state table to initial state of off.
        **  For B1 this should probably be on.  Did not want to
        **  have any special B1 flags in CREATEDB.
        */
        sec_type_var= 1;
        sec_state_var = 0;
        for (sec_id =1; sec_id <= 17; sec_id++)
        {
/* # line 3160 "ducommon.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"append iisecuritystate(type=");
    IIputdomio((short *)0,1,30,4,&sec_type_var);
    IIwritio(0,(short *)0,1,32,0,(char *)", id=");
    IIputdomio((short *)0,1,30,4,&sec_id);
    IIwritio(0,(short *)0,1,32,0,(char *)", state=");
    IIputdomio((short *)0,1,30,4,&sec_state_var);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 3162 "ducommon.qsc" */	/* inquire_equel */
  {
    IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
  }
/* # line 3163 "ducommon.qsc" */	/* host code */
	    if (row_cnt != 1)
	    {
		return (du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
			0, "iisecuritystate", 0, DU_DBDBNAME));
	    }
        }
	return E_DU_OK;
}
/*{
** Name:  duc_iirule() -- build catalog iirule
**
** Description:
**        This routine makes the catalog iirule.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	29-jul-92 (rickh)
**		initial creation.
**	24-jun-93 (robf)
**		Added new secure attributes
**	15-mar-2002 (toumi01)
**	    add rule_col11-33 to support 1024 cols/row
*/
VOID
duc_iirule()
{
/* # line 3201 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iirule(rule_name=char(32), rule_owner=char(32), rule_type=i2, \
rule_flags=i2, rule_tabbase=i4, rule_tabidx=i4, rule_qryid1=i4, rule_q\
ryid2=i4, rule_treeid1=i4, rule_treeid2=i4, rule_statement=i4, rule_db\
p_name=char(32), rule_dbp_owner=char(32), ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"rule_time_date=ingresdate, rule_time_int=ingresdate, rule_col1=i4, ru\
le_col2=i4, rule_col3=i4, rule_col4=i4, rule_col5=i4, rule_col6=i4, ru\
le_col7=i4, rule_col8=i4, rule_col9=i4, rule_cola=i4, rule_col11=i4, r\
ule_col12=i4, rule_col13=i4, rule_col14=i4, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"rule_col15=i4, rule_col16=i4, rule_col17=i4, rule_col18=i4, rule_col1\
9=i4, rule_col20=i4, rule_col21=i4, rule_col22=i4, rule_col23=i4, rule\
_col24=i4, rule_col25=i4, rule_col26=i4, rule_col27=i4, rule_col28=i4,\
 rule_col29=i4, rule_col30=i4, rule_col31=i4, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"rule_col32=i4, rule_col33=i4, rule_id1=i4, rule_id2=i4, rule_dbp_para\
m=i2, rule_pad=i2, rule_free=char(8))");
    IIsyncup((char *)0,0);
  }
/* # line 3256 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_modify_iirule() -- build iirule keys
**
** Description:
**        This routine makes keys for the catalog iirule.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	29-jul-92 (rickh)
**		initial creation.
**	20-may-1999 (nanpr01)
**	    SIR 89267 - Need additional index to check the existence
**	    of rule by its owner and name.
*/
VOID
duc_modify_iirule()
{
/* # line 3288 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iirule to HASH on rule_tabbase, rule_tabidx with MINPAGES=32");
    IIsyncup((char *)0,0);
  }
/* # line 3292 "ducommon.qsc" */	/* host code */
/* # line 3409 "ducommon.sc" */	/* create index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create index iiruleidx on iirule(rule_id1, rule_id2)with STRUCTURE=HA\
SH, MINPAGES=32");
    IIsyncup((char *)0,0);
  }
/* # line 3412 "ducommon.sc" */	/* create unique index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create unique index iiruleidx1 on iirule(rule_owner, rule_name)with S\
TRUCTURE=HASH, MINPAGES=32");
    IIsyncup((char *)0,0);
  }
/* # line 3415 "ducommon.sc" */	/* host code */
}
/*{
** Name:  duc_iiintegrity() -- build iiintegrity catalog
**
** Description:
**        This routine makes the iiintegrity catalog.
**
**  NOTE: this catalog and the iiqrytext catalog are used to store
**        SQL92 constraints.  Since NOT NULL in SQL92 is translated into 
**        a CHECK constraint,
**          NO CATALOG WITH A NOT NULL COLUMN CAN BE CREATED BEFORE THESE
**          CATALOGS ARE CREATED, OR BEFORE IIINTEGRITY IS MODIFIED.
**        The only way around this bootstrapping problem is to create
**        iiqrytext and iiintegrity using QUEL, not SQL.
**          Since most (if not all) catalogs have NOT NULL columns,
**        this implies that any catalogs that need to be created before
**        these two catalogs MUST be created using QUEL.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	29-jul-92 (rickh)
**		initial creation.
**	28-Feb-93 (jpk)
**		added this note: this routine must be equel, since
**		new iiintegrity must be created with QUEL; else on upgrade
**		to 6.5 DBMS will try to insert check constraints
**		corresponding to NOT NULLS specified in iiintegrity
**		columns into iiintegrity itself.
**	26-oct-93 (andre)
**	    added cons_create_date.  I don't know who came up with idea to have 
**	    half the attributes begin with int while the other half begins with
**	    cons, but cons_create_date contains creation date for constraints 
**	    AND integrities
**	16-aug-99 (inkdo01)
**	    Added consdelrule and consupdrule columns to store referential 
**	    actions.
**	15-mar-2002 (toumi01)
**	    add intdomset11-33 to support 1024 cols/row
*/
VOID
  duc_iiintegrity()
  {
    int err_no;
/* # line 3356 "ducommon.qsc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"create iiintegrity(inttabbase=i4, inttabidx=i4, intqryid1=i4, intqryi\
d2=i4, inttreeid1=i4, inttreeid2=i4, intresvar=i2, intnumber=i2, intse\
q=i4, intdomset1=i4, intdomset2=i4, intdomset3=i4, intdomset4=i4, intd\
omset5=i4, intdomset6=i4, intdomset7=i4, ");
      IIwritio(0,(short *)0,1,32,0,(char *)
"intdomset8=i4, intdomset9=i4, intdomseta=i4, intdomset11=i4, intdomse\
t12=i4, intdomset13=i4, intdomset14=i4, intdomset15=i4, intdomset16=i4\
, intdomset17=i4, intdomset18=i4, intdomset19=i4, intdomset20=i4, intd\
omset21=i4, intdomset22=i4, intdomset23=i4, ");
      IIwritio(0,(short *)0,1,32,0,(char *)
"intdomset24=i4, intdomset25=i4, intdomset26=i4, intdomset27=i4, intdo\
mset28=i4, intdomset29=i4, intdomset30=i4, intdomset31=i4, intdomset32\
=i4, intdomset33=i4, consname=char(32), consid1=i4, consid2=i4, conssc\
hema_id1=i4, consschema_id2=i4, consflags=i4, ");
      IIwritio(0,(short *)0,1,32,0,(char *)
"cons_create_date=i4, consdelrule=i2, consupdrule=i2, intreserve=char(\
28))");
      IIsyncup((char *)0,0);
    }
/* # line 3380 "ducommon.qsc" */	/* host code */
	/* xxx debugging for upgradedb. should be trapped more generally */
/* # line 3381 "ducommon.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&err_no,(char *)"errorno");
    }
/* # line 3382 "ducommon.qsc" */	/* host code */
	if (err_no != 0)
	{
		if (upg_dbg_out)
		{
			SIprintf("]==> error %d creating iiintegrity\n",
				err_no);
		}
	}
  }
/*{
** Name:  duc_modify_iiintegrity() -- add keys to iiintegrity catalog
**
** Description:
**        This routine adds keys to the iiintegrity catalog.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	29-jul-92 (rickh)
**		initial creation.
**	28-feb-1993 (jpk)
**		added index
*/
VOID
duc_modify_iiintegrity()
{
  int err_no;
/* # line 3422 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iiintegrity to hash on inttabbase, inttabidx where minpages=8");
    IIsyncup((char *)0,0);
  }
/* # line 3425 "ducommon.qsc" */	/* host code */
	/* xxx debugging for upgradedb. should be trapped more generally */
/* # line 3426 "ducommon.qsc" */	/* inquire_equel */
  {
    IIeqiqio((short *)0,1,30,4,&err_no,(char *)"errorno");
  }
/* # line 3427 "ducommon.qsc" */	/* host code */
	if (err_no != 0)
	{
		if (upg_dbg_out)
		{
			SIprintf("]==> error %d modifying iiintegrity\n",
				err_no);
		}
	}
/* # line 3436 "ducommon.qsc" */	/* index */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"index on iiintegrity=iiintegrityidx(consschema_id1, consschema_id2, c\
onsname)with structure=hash, minpages=32");
    IIsyncup((char *)0,0);
  }
/* # line 3440 "ducommon.qsc" */	/* inquire_equel */
  {
    IIeqiqio((short *)0,1,30,4,&err_no,(char *)"errorno");
  }
/* # line 3441 "ducommon.qsc" */	/* host code */
	/* xxx debugging for upgradedb. should be trapped more generally */
	if (err_no != 0)
	{
		if (upg_dbg_out)
		{
			SIprintf("]==> error %d index on iiintegrity\n",
				err_no);
		}
	}
}
/*{
** Name:  duc_iikey() -- build iikey catalog
**
** Description:
**        This routine makes the iikey catalog.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	29-jul-92 (rickh)
**		initial creation.
**	09-feb-93 (rblumer)
**	    added key_ to all the column names to follow catalog conventions
**	    (and because position is a reserved word in FULL SQL92 and we don't
**	    want to use reserved words for catalog names); change all columns
**	    to be NULL columns, to get rid of the extra null byte.
*/
VOID
duc_iikey()
{
/* # line 3610 "ducommon.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iikey(key_consid1 i4 not null, key_consid2 i4 not null, \
key_attid i2 not null, key_position i2 not null)");
    IIsyncup((char *)0,0);
  }
/* # line 3617 "ducommon.sc" */	/* host code */
	/* create standard views on iikey here? */
}
/*{
** Name:  duc_modify_iikey() -- add keys to iikey catalog
**
** Description:
**        This routine adds keys to the iikey catalog.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	29-jul-92 (rickh)
**		initial creation.
**	24-mar-93 (rickh)
**	    Extended the 07-mar-93 fix to add "key_" to the columns mentioned
**	    in duc_modify_iikey and duc_v_iikeys.
*/
VOID
duc_modify_iikey()
{
/* # line 3648 "ducommon.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iikey to HASH on key_consid1, key_consid2 with MINPAGES=32");
    IIsyncup((char *)0,0);
  }
/* # line 3650 "ducommon.sc" */	/* host code */
}
/*{
** Name:  duc_iirange() -- build catalog iirange
**
** Description:
**        This routine makes the catalog iirange.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	08-may-1996 (shero03)
**		initial creation.
*/
VOID
duc_iirange()
{
/* # line 3677 "ducommon.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iirange(rng_baseid integer not null, rng_indexid integer\
 not null, rng_ll1 float not null, rng_ll2 float not null, rng_ll3 flo\
at not null, rng_ll4 float not null, rng_ur1 float not null, rng_ur2 f\
loat not null, rng_ur3 float not null, rng_ur4");
    IIwritio(0,(short *)0,1,32,0,(char *)
" float not null, rng_dimension smallint not null, rng_hilbertsize sma\
llint not null, rng_rangedt smallint not null, rng_rangetype char(1)no\
t null)");
    IIsyncup((char *)0,0);
  }
/* # line 3693 "ducommon.sc" */	/* host code */
}
/*{
** Name:  duc_modify_iirange() -- build iirange keys
**
** Description:
**        This routine makes keys for the catalog iirange.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	08-may-1996 (shero03)
**		initial creation.
*/
VOID
duc_modify_iirange()
{
/* # line 3720 "ducommon.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iirange to HASH on rng_baseid, rng_indexid with MINPAGES=2, AL\
LOCATION=4");
    IIsyncup((char *)0,0);
  }
/* # line 3722 "ducommon.sc" */	/* host code */
}
/*{
** Name:  duc_iietab() -- build iiextended_relation
**
** Description:
**        This routine makes the catalog iiextended_relation.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	03-aug-1999 (stial01)
**		initial creation.
**      05-dec-2003 (ashco01) INGSRV2629, Bug 111419.
**              Permission 'SELECT to PUBLIC with GRANT OPTION' granted
**              on table after creation. This is performed by createdb but
**              was not being perfomed when creating table during upgradedb
**              from a pre-2.5 database.
*/
VOID
duc_iietab()
{
/* # line 3754 "ducommon.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iiextended_relation(etab_base integer not null, etab_ext\
ension integer not null, etab_type integer not null, etab_attid intege\
r not null, etab_reserved char(16)not null)with noduplicates");
    IIsyncup((char *)0,0);
  }
/* # line 3762 "ducommon.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on iiextended_relation to public with grant OPTION");
    IIsyncup((char *)0,0);
  }
/* # line 3765 "ducommon.sc" */	/* host code */
}
/*{
** Name:  duc_modify_iietab() -- build iiextended_relation keys
**
** Description:
**        This routine makes keys for the catalog iiextended_relation.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	03-aug-1999 (stial01)
**		initial creation.
*/
VOID
duc_modify_iietab()
{
/* # line 3792 "ducommon.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iiextended_relation to HASH on etab_base with MINPAGES=2, ALLO\
CATION=4");
    IIsyncup((char *)0,0);
  }
/* # line 3794 "ducommon.sc" */	/* host code */
}
/*{
** Name:  duc_iidefault() -- build iidefault catalog
**
** Description:
**        This routine makes the iidefault catalog.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	29-jul-92 (rickh)
**		initial creation.
**	26-oct-92 (rblumer)
**	    split IS_LITERAL into QUOTED_LITERAL and UNQUOTED_LITERAL
**	05-dec-92 (rblumer)
**	    take deftype out of CREATE TABLE and INSERTs;
**	    make defid1 and defid2 be NOT NULL columns.
**	19-jan-93 (rblumer)
**	    add DB_DEF_UNKNOWN canonical tuple, mainly for views;
**	    add tuples for logical keys, INITIAL_USER, and DBA.
**	22-feb-93 (rickh)
**	    Upped the length of the value string in IIDEFAULT by 1 byte.  This
**	    forces (given the null indicator byte and the 2 byte length
**	    descriptor) 4 byte alignment in our DBMS.H structure, which is good
**	    for the secondary index (which has a concluding tidp column).
**	9-mar-93 (rickh)
**	    Added DB_DEF_ID_NEEDS_CONVERTING and DB_DEF_ID_UNRECOGNIZED
**	    tuples to IIDEFAULT.
*/
VOID
duc_iidefault()
{
  i4 id1;
  struct {
    short size;
    char buf[DB_LEN_TAB_LOGKEY];
  } tabkey;
  struct {
    short size;
    char buf[DB_LEN_OBJ_LOGKEY];
  } objkey;
/* # line 3848 "ducommon.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iidefault(defid1 i4 not null, defid2 i4 not null, defval\
ue varchar(1501))");
    IIsyncup((char *)0,0);
  }
/* # line 3854 "ducommon.sc" */	/* host code */
	/* populate iidefault with the canonical defaults 
	 */
	id1 = DB_DEF_NOT_DEFAULT;
/* # line 3857 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into IIDEFAULT(defid1, defid2, defvalue)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, null)");
    IIsyncup((char *)0,0);
  }
/* # line 3861 "ducommon.sc" */	/* host code */
	id1 = DB_DEF_NOT_SPECIFIED;
/* # line 3862 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into IIDEFAULT(defid1, defid2, defvalue)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, null)");
    IIsyncup((char *)0,0);
  }
/* # line 3866 "ducommon.sc" */	/* host code */
	id1 = DB_DEF_UNKNOWN;
/* # line 3867 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into IIDEFAULT(defid1, defid2, defvalue)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, null)");
    IIsyncup((char *)0,0);
  }
/* # line 3871 "ducommon.sc" */	/* host code */
	id1 = DB_DEF_ID_0;
/* # line 3872 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into IIDEFAULT(defid1, defid2, defvalue)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, '0')");
    IIsyncup((char *)0,0);
  }
/* # line 3876 "ducommon.sc" */	/* host code */
	id1 = DB_DEF_ID_BLANK;
/* # line 3877 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into IIDEFAULT(defid1, defid2, defvalue)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, ''' ''')");
    IIsyncup((char *)0,0);
  }
/* # line 3881 "ducommon.sc" */	/* host code */
	/* the default for a table_key is a string of NULL bytes.
	 */
	tabkey.size = DB_LEN_TAB_LOGKEY;
	MEfill((u_i2) DB_LEN_TAB_LOGKEY, NULLCHAR, tabkey.buf);
	id1 = DB_DEF_ID_TABKEY;
/* # line 3886 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into IIDEFAULT(defid1, defid2, defvalue)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, table_key(");
    IIputdomio((short *)0,1,21,0,&tabkey);
    IIwritio(0,(short *)0,1,32,0,(char *)"))");
    IIsyncup((char *)0,0);
  }
/* # line 3890 "ducommon.sc" */	/* host code */
	/* the default for an object_key is a string of NULL bytes
	 */
	objkey.size = DB_LEN_OBJ_LOGKEY;
	MEfill((u_i2) DB_LEN_OBJ_LOGKEY, NULLCHAR, objkey.buf);
	id1 = DB_DEF_ID_LOGKEY;
/* # line 3895 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into IIDEFAULT(defid1, defid2, defvalue)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, object_key(");
    IIputdomio((short *)0,1,21,0,&objkey);
    IIwritio(0,(short *)0,1,32,0,(char *)"))");
    IIsyncup((char *)0,0);
  }
/* # line 3899 "ducommon.sc" */	/* host code */
	id1 = DB_DEF_ID_NULL;
/* # line 3900 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into IIDEFAULT(defid1, defid2, defvalue)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, 'NULL')");
    IIsyncup((char *)0,0);
  }
/* # line 3904 "ducommon.sc" */	/* host code */
	id1 = DB_DEF_ID_USERNAME;
/* # line 3905 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into IIDEFAULT(defid1, defid2, defvalue)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, 'CURRENT_USER')");
    IIsyncup((char *)0,0);
  }
/* # line 3909 "ducommon.sc" */	/* host code */
	id1 = DB_DEF_ID_CURRENT_DATE;
/* # line 3910 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into IIDEFAULT(defid1, defid2, defvalue)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, 'CURRENT_DATE')");
    IIsyncup((char *)0,0);
  }
/* # line 3914 "ducommon.sc" */	/* host code */
	id1 = DB_DEF_ID_CURRENT_TIMESTAMP;
/* # line 3915 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into IIDEFAULT(defid1, defid2, defvalue)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, 'CURRENT_TIMESTAMP')");
    IIsyncup((char *)0,0);
  }
/* # line 3919 "ducommon.sc" */	/* host code */
	id1 = DB_DEF_ID_CURRENT_TIME;
/* # line 3920 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into IIDEFAULT(defid1, defid2, defvalue)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, 'CURRENT_TIME')");
    IIsyncup((char *)0,0);
  }
/* # line 3924 "ducommon.sc" */	/* host code */
	id1 = DB_DEF_ID_SESSION_USER;
/* # line 3925 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into IIDEFAULT(defid1, defid2, defvalue)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, 'SESSION_USER')");
    IIsyncup((char *)0,0);
  }
/* # line 3929 "ducommon.sc" */	/* host code */
	id1 = DB_DEF_ID_SYSTEM_USER;
/* # line 3930 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into IIDEFAULT(defid1, defid2, defvalue)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, 'SYSTEM_USER')");
    IIsyncup((char *)0,0);
  }
/* # line 3934 "ducommon.sc" */	/* host code */
	id1 = DB_DEF_ID_INITIAL_USER;
/* # line 3935 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into IIDEFAULT(defid1, defid2, defvalue)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, 'INITIAL_USER')");
    IIsyncup((char *)0,0);
  }
/* # line 3939 "ducommon.sc" */	/* host code */
	id1 = DB_DEF_ID_DBA;
/* # line 3940 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into IIDEFAULT(defid1, defid2, defvalue)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, '$DBA')");
    IIsyncup((char *)0,0);
  }
/* # line 3944 "ducommon.sc" */	/* host code */
	id1 = DB_DEF_ID_NEEDS_CONVERTING;
/* # line 3945 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into IIDEFAULT(defid1, defid2)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0)");
    IIsyncup((char *)0,0);
  }
/* # line 3949 "ducommon.sc" */	/* host code */
	id1 = DB_DEF_ID_UNRECOGNIZED;
/* # line 3950 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into IIDEFAULT(defid1, defid2)values(");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0)");
    IIsyncup((char *)0,0);
  }
/* # line 3954 "ducommon.sc" */	/* host code */
} /* end duc_iidefault */
/*{
** Name:  duc_modify_iidefault() -- add keys to iidefault catalog
**
** Description:
**      This routine adds keys to the iidefault catalog.
**
**	Note: we need to compress the default_value field in the base table
**	      and in all indexes where it appears.  Otherwise we'll have
**	      1500 bytes of space for a value that will usually be > 50 bytes.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	29-jul-92 (rickh)
**	    initial creation.
**	05-dec-92 (rblumer)
**	    took deftype out of index; added WITH COMPRESSION 
**	    to both base table and index (want to compress defvalue column)
**	    
*/
VOID
duc_modify_iidefault()
{
/* # line 3989 "ducommon.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidefault to HASH on defid1, defid2 with MINPAGES=32, COMPRESS\
ION");
    IIsyncup((char *)0,0);
  }
/* # line 3993 "ducommon.sc" */	/* create index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create index iidefaultidx on iidefault(defvalue)with MINPAGES=32, STR\
UCTURE=HASH, COMPRESSION");
    IIsyncup((char *)0,0);
  }
/* # line 3997 "ducommon.sc" */	/* host code */
}
/*{
** Name:  duc_iiprotect() -- build catalog iiprotect
**
** Description:
**        This routine makes the catalog iiprotect.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	11-oct-91 (teresa)
**	    initial creation.
**      24-aug-92 (andre)
**	    renamed profill1 and profill2 to proflags and prodepth, respectively
**	15-mar-2002 (toumi01)
**	    add prodomset11-33 to support 1024 cols/row
*/
VOID
duc_iiprotect()
{
/* # line 3946 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iiprotect(protabbase=i4, protabidx=i4, propermid=i2, proflags=\
i2, protodbgn=i2, protodend=i2, prodowbgn=i2, prodowend=i2, proqryid1=\
i4, proqryid2=i4, protreeid1=i4, protreeid2=i4, procrtime1=i4, procrti\
me2=i4, proopctl=i4, proopset=i4, provalue=i4");
    IIwritio(0,(short *)0,1,32,0,(char *)
", prodepth=i2, probjtype=char(1), probjstat=char(1), probjname=char(3\
2), probjowner=char(32), prograntor=char(32), prouser=char(32), proter\
m=char(16), profill3=i2, progtype=i2, proseq=i4, prodomset1=i4, prodom\
set2=i4, prodomset3=i4, prodomset4=i4, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"prodomset5=i4, prodomset6=i4, prodomset7=i4, prodomset8=i4, prodomset\
9=i4, prodomseta=i4, prodomset11=i4, prodomset12=i4, prodomset13=i4, p\
rodomset14=i4, prodomset15=i4, prodomset16=i4, prodomset17=i4, prodoms\
et18=i4, prodomset19=i4, prodomset20=i4, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"prodomset21=i4, prodomset22=i4, prodomset23=i4, prodomset24=i4, prodo\
mset25=i4, prodomset26=i4, prodomset27=i4, prodomset28=i4, prodomset29\
=i4, prodomset30=i4, prodomset31=i4, prodomset32=i4, prodomset33=i4, p\
roreserve=char(32))");
    IIsyncup((char *)0,0);
  }
/* # line 3981 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iidbpriv() -- build catalog iidbpriv
**
** Description:
**        This routine makes the catalog iidbpriv.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	11-oct-91 (teresa)
**		initial creation.
**	22-oct-93 (robf)
**              Add idle/connect/priority_limit
*/
VOID
  duc_iidbpriv()
  {
/* # line 4010 "ducommon.qsc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"create iidbpriv(dbname=char(32), grantee=char(32), gtype=i2, dbflags=\
i2, control=i4, flags=i4, qrow_limit=i4, qdio_limit=i4, qcpu_limit=i4,\
 qpage_limit=i4, qcost_limit=i4, idle_time_limit=i4, connect_time_limi\
t=i4, priority_limit=i4, reserve=char(32))");
      IIsyncup((char *)0,0);
    }
  }
/* # line 4024 "ducommon.qsc" */	/* host code */
/*{
** Name:  duc_iiprocedure() -- build catalog iiprocedure
**
** Description:
**        This routine makes the catalog iiprocedure.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	4-nov-92 (rickh)
**		initial creation.
**	21-jun-93 (robf)
**		remove dbp_pad(8),
**		added dbp_secid and dbp_eff_secid
**	01-sep-93 (andre)
**	    added dbp_ubt_id and dbp_ubt_idx
**	26-oct-93 (andre)
**	    added dbp_create_date
*/
VOID
duc_iiprocedure()
{
/* # line 4056 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iiprocedure(dbp_name=char(32)not null, dbp_owner=char(32)not n\
ull, dbp_txtlen=i4 not null, dbp_txtid1=i4 not null, dbp_txtid2=i4 not\
 null, dbp_mask1=i4 not null, dbp_mask2=i4 not null, dbp_id=i4 not nul\
l, dbp_idx=i4 not null, dbp_parameter_count=i4");
    IIwritio(0,(short *)0,1,32,0,(char *)
" not null, dbp_record_width=i4 not null, dbp_ubt_id=i4 not null, dbp_\
ubt_idx=i4 not null, dbp_create_date=i4 not null, dbp_free=char(16)not\
 null)");
    IIsyncup((char *)0,0);
  }
/* # line 4072 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iiprocedure_parameter() -- build catalog iiprocedure_parameter
**
** Description:
**        This routine makes the catalog iiprocedure_parameter.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	4-nov-92 (rickh)
**		initial creation.
**	03-mar-93 (andre)
**	    attributes must be declared NOT NULL, or all the hell breaks loose
**	    when QEF tries to access the catalog (QEF thinks that the size of a
**	    tuple equal to the size of DB_PROCEDURE_PARAMETER, but DMF knows
**	    it's is longer because of all the NULL bytes
**	07-mar-93 (rblumer)
**	    added pp_ to all the column names, because
**	         - precision and pad are SQL92 keywords and we don't want to
**	           use keywords for catalogs, and
**	         - using a common prefix follows existing catalog conventions
*/
VOID
duc_iiprocedure_parameter()
{
/* # line 4175 "ducommon.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iiprocedure_parameter(pp_procid1 I4 not null, pp_procid2\
 I4 not null, pp_name CHAR(32)not null, pp_defid1 I4 not null, pp_defi\
d2 I4 not null, pp_flags I4 not null, pp_number I2 not null, pp_offset\
 I2 not null, pp_length I2 not null, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"pp_datatype I2 not null, pp_precision I2 not null, pp_pad I2 not null\
)");
    IIsyncup((char *)0,0);
  }
/* # line 4191 "ducommon.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iiprocedure_parameter to HASH on pp_procid1, pp_procid2 with M\
INPAGES=32");
    IIsyncup((char *)0,0);
  }
/* # line 4194 "ducommon.sc" */	/* host code */
}
/*{
** Name:  duc_iigw07() -- build iigw07_relation, iigw07_attribute, iigw07_index
**
** Description:
**      This routine makes the system catalogs iigw07_relation,
**	iigw07_attribute, and iigw07_index for later IMA use.
**	This routine must be called before duc_iilgmo().
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** History:
**	10-feb-1993 (jpk)
**		initial creation.
*/
VOID
duc_iigw07()
{
  i4 cnt;
    /* wipe the slate clean for upgradedb's sake */
    ensure_not_exists("iigw07_relation");
    ensure_not_exists("iigw07_attribute");
    ensure_not_exists("iigw07_index");
    /* now safe to make them */
/* # line 4227 "ducommon.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iigw07_relation(tblid integer not null not default, tbli\
dx integer not null not default, flags integer not null not default)");
    IIsyncup((char *)0,0);
  }
/* # line 4232 "ducommon.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iigw07_attribute(tblid integer not null not default, tbl\
idx integer not null not default, attnum i2 not null not default, clas\
sid char(64)not null not default)");
    IIsyncup((char *)0,0);
  }
/* # line 4238 "ducommon.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iigw07_index(tblid integer not null not default, tblidx \
integer not null not default, attnum i2 not null not default, classid \
char(64)not null not default)");
    IIsyncup((char *)0,0);
  }
/* # line 4244 "ducommon.sc" */	/* host code */
}
/*{
** Name: duc_modify_iigw07() - modify catalogs iigw07_{relation,attribute,index}
**
** Description:
**	This routine modifies the system catalogs iigw07_relation,
**	iigw07_attribute, asd iigw07_index back to btree
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** History:
**	10-feb-93 (jpk)
**		initial creation.
*/
VOID
duc_modify_iigw07()
{
/* # line 4268 "ducommon.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iigw07_relation to btree on tblid, tblidx");
    IIsyncup((char *)0,0);
  }
/* # line 4269 "ducommon.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iigw07_attribute to btree on tblid, tblidx, attnum");
    IIsyncup((char *)0,0);
  }
/* # line 4270 "ducommon.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iigw07_index to btree on tblid, tblidx, attnum");
    IIsyncup((char *)0,0);
  }
/* # line 4271 "ducommon.sc" */	/* host code */
}
/*{
** Name:  duc_iilgmo() -- register IMA table and define view on it for 2PC
**
** Description:
**	For 2PC recovery in XA environments IMA provides useful
**	information.  This routine registers the necessary
**	tables and provides a useful view.  The view will be used
**	by XA servers.
**
** Inputs:
**	duc_errdb	PTR to error control block
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
** Side Effects:
**	None
**
** History:
**	30 January 1993 (jpk)
**		initial creation.
**	20 July 1993 (jpk)
**		Added GRANT SELECT on lgmo_xa_dis_tran_ids TO PUBLIC.
**	3 Jan 1994 (jpk)
**		modified view definition of view lgmo_xa_dis_tran_ids
**		as per larrym; new definition is a view with a room.
**	28 March 1994 (jpk)
**		modified view definition of view lgmo_xa_dis_tran_ids
**		to work in FIPS iidbdb; can't use "$ingres" because
**		in FIPS it's "$INGRES"; can't use $ingres without quotes
**		because of esqlc bug 57353.  So omit $ingres.  Can
**		add it back in for FCS2 if desired.
*/
VOID
duc_iilgmo(duc_errcb)
DU_ERROR	*duc_errcb;
{
    IIlq_Protect(TRUE);
    /* force clean slate for upgradedb's sake */
    if (rel_exists("lgmo_xa_dis_tran_ids"))
    {
/* # line 4315 "ducommon.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop view lgmo_xa_dis_tran_i\
ds");
    IIsyncup((char *)0,0);
  }
/* # line 4316 "ducommon.sc" */	/* host code */
    }
    if (rel_exists("lgmo_xa_dis_tran_ids"))
    {
/* # line 4319 "ducommon.sc" */	/* remove */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"remove lgmo_lgd");
    IIsyncup((char *)0,0);
  }
/* # line 4320 "ducommon.sc" */	/* host code */
    }
    if (rel_exists("lgmo_lxb"))
    {
/* # line 4323 "ducommon.sc" */	/* remove */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"remove lgmo_lxb");
    IIsyncup((char *)0,0);
  }
/* # line 4324 "ducommon.sc" */	/* host code */
    }
    if (rel_exists("lgmo_lpb"))
    {
/* # line 4327 "ducommon.sc" */	/* remove */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"remove lgmo_lpb");
    IIsyncup((char *)0,0);
  }
/* # line 4328 "ducommon.sc" */	/* host code */
    }
    if (rel_exists("lgmo_ldb"))
    {
/* # line 4331 "ducommon.sc" */	/* remove */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"remove lgmo_ldb");
    IIsyncup((char *)0,0);
  }
/* # line 4332 "ducommon.sc" */	/* host code */
    }
/* # line 4333 "ducommon.sc" */	/* register table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register table lgmo_lgd(place varchar(4)not null not default is 'VNOD\
E', lgd_status char(100)not null not default is 'exp.dmf.lg.lgd_status\
', lgd_status_num i4 not null not default is 'exp.dmf.lg.lgd_status_nu\
m', lgd_buf_cnt i4 not null not default is ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'exp.dmf.lg.lgd_buf_cnt', lgd_lpb_inuse i4 not null not default is 'e\
xp.dmf.lg.lgd_lpb_inuse', lgd_lxb_inuse i4 not null not default is 'ex\
p.dmf.lg.lgd_lxb_inuse', lgd_ldb_inuse i4 not null not default is 'exp\
.dmf.lg.lgd_ldb_inuse', lgd_lpd_inuse i4 not ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"null not default is 'exp.dmf.lg.lgd_lpd_inuse', lgd_protect_count i4 \
not null not default is 'exp.dmf.lg.lgd_protect_count', lgd_n_logwrite\
rs i4 not null not default is 'exp.dmf.lg.lgd_n_logwriters', lgd_no_bc\
p i4 not null not default is ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'exp.dmf.lg.lgd_no_bcp', lgd_sbk_count i4 not null not default is 'ex\
p.dmf.lg.lgd_sbk_count', lgd_sbk_size i4 not null not default is 'exp.\
dmf.lg.lgd_sbk_size', lgd_lbk_count i4 not null not default is 'exp.dm\
f.lg.lgd_lbk_count', lgd_lbk_size i4 not null ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"not default is 'exp.dmf.lg.lgd_lbk_size', lgd_forced_lga char(30)not \
null not default is 'exp.dmf.lg.lgd_forced_lga', lgd_forced_high i4 no\
t null not default is 'exp.dmf.lg.lgd_forced_lga.lga_high', lgd_forced\
_low i4 not null not default is ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'exp.dmf.lg.lgd_forced_lga.lga_low', lgd_j_first_cp_high i4 not null \
not default is 'exp.dmf.lg.lgd_j_first_cp.lga_high', lgd_j_first_cp_lg\
a char(30)not null not default is 'exp.dmf.lg.lgd_j_first_cp', lgd_j_f\
irst_cp_low i4 not null not default is ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'exp.dmf.lg.lgd_j_first_cp.lga_low', lgd_j_last_cp_lga char(30)not nu\
ll not default is 'exp.dmf.lg.lgd_j_last_cp', lgd_j_last_cp_high i4 no\
t null not default is 'exp.dmf.lg.lgd_j_last_cp.lga_high', lgd_j_last_\
cp_low i4 not null not default is ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'exp.dmf.lg.lgd_j_last_cp.lga_low', lgd_d_first_cp_lga char(30)not nu\
ll not default is 'exp.dmf.lg.lgd_d_first_cp', lgd_d_first_cp_high i4 \
not null not default is 'exp.dmf.lg.lgd_d_first_cp.lga_high', lgd_d_fi\
rst_cp_low i4 not null not default is ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'exp.dmf.lg.lgd_d_first_cp.lga_low', lgd_d_last_cp_lga char(30)not nu\
ll not default is 'exp.dmf.lg.lgd_d_last_cp', lgd_d_last_cp_high i4 no\
t null not default is 'exp.dmf.lg.lgd_d_last_cp.lga_high', lgd_d_last_\
cp_low i4 not null not default is ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'exp.dmf.lg.lgd_d_last_cp.lga_low', lgd_stat_add i4 not null not defa\
ult is 'exp.dmf.lg.lgd_stat.add', lgd_stat_remove i4 not null not defa\
ult is 'exp.dmf.lg.lgd_stat.remove', lgd_stat_begin i4 not null not de\
fault is 'exp.dmf.lg.lgd_stat.begin', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"lgd_stat_end i4 not null not default is 'exp.dmf.lg.lgd_stat.end', lg\
d_stat_write i4 not null not default is 'exp.dmf.lg.lgd_stat.write', l\
gd_stat_split i4 not null not default is 'exp.dmf.lg.lgd_stat.split', \
lgd_stat_force i4 not null not default is ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'exp.dmf.lg.lgd_stat.force', lgd_stat_readio i4 not null not default \
is 'exp.dmf.lg.lgd_stat.readio', lgd_stat_writeio i4 not null not defa\
ult is 'exp.dmf.lg.lgd_stat.writeio', lgd_stat_wait i4 not null not de\
fault is 'exp.dmf.lg.lgd_stat.wait', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"lgd_stat_group_force i4 not null not default is 'exp.dmf.lg.lgd_stat.\
group_force', lgd_stat_group_count i4 not null not default is 'exp.dmf\
.lg.lgd_stat.group_count', lgd_stat_inconsist_db i4 not null not defau\
lt is 'exp.dmf.lg.lgd_stat.inconsist_db', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"lgd_stat_pgyback_check i4 not null not default is 'exp.dmf.lg.lgd_sta\
t.pgyback_check', lgd_stat_pgyback_write i4 not null not default is 'e\
xp.dmf.lg.lgd_stat.pgyback_write', lgd_stat_kbytes i4 not null not def\
ault is 'exp.dmf.lg.lgd_stat.kbytes', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"lgd_stat_free_wait i4 not null not default is 'exp.dmf.lg.lgd_stat.fr\
ee_wait', lgd_stat_stall_wait i4 not null not default is 'exp.dmf.lg.l\
gd_stat.stall_wait', lgd_stat_bcp_stall_wait i4 not null not default i\
s 'exp.dmf.lg.lgd_stat.bcp_stall_wait', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"lgd_stat_log_readio i4 not null not default is 'exp.dmf.lg.lgd_stat.l\
og_readio', lgd_stat_dual_readio i4 not null not default is 'exp.dmf.l\
g.lgd_stat.dual_readio', lgd_stat_log_writeio i4 not null not default \
is 'exp.dmf.lg.lgd_stat.log_writeio', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"lgd_stat_dual_writeio i4 not null not default is 'exp.dmf.lg.lgd_stat\
.dual_writeio', lgd_cpstall i4 not null not default is 'exp.dmf.lg.lgd\
_cpstall', lgd_stall_limit i4 not null not default is 'exp.dmf.lg.lgd_\
stall_limit', lgd_check_stall i4 not null not ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"default is 'exp.dmf.lg.lgd_check_stall', lgd_dmu_cnt i4 not null not \
default is 'exp.dmf.lg.lgd_dmu_cnt', lgd_active_log i4 not null not de\
fault is 'exp.dmf.lg.lgd_active_log')as import from 'tables' with dbms\
=IMA, structure=sortkeyed, key=(place)");
    IIsyncup((char *)0,0);
  }
/* # line 4441 "ducommon.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	    sqlerr_handler(duc_errcb, W_DU186B_REGISTER_FAILED);
    }
/* # line 4445 "ducommon.sc" */	/* register table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register table lgmo_lpb(place varchar(4)not null not default is 'VNOD\
E', lpb_id_instance i4 not null not default is 'exp.dmf.lg.lpb_id.id_i\
nstance', lpb_id_id i4 not null not default is 'exp.dmf.lg.lpb_id.id_i\
d', lpb_status char(100)not null not default ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"is 'exp.dmf.lg.lpb_status', lpb_status_num i4 not null not default is\
 'exp.dmf.lg.lpb_status_num', lpb_pid i4 not null not default is 'exp.\
dmf.lg.lpb_pid', lpb_cond i4 not null not default is 'exp.dmf.lg.lpb_c\
ond', lpb_bufmgr_id i4 not null not default is");
    IIwritio(0,(short *)0,1,32,0,(char *)
" 'exp.dmf.lg.lpb_bufmgr_id', lpb_force_abort_sid i4 not null not defa\
ult is 'exp.dmf.lg.lpb_force_abort_sid', lpb_gcmt_sid i4 not null not \
default is 'exp.dmf.lg.lpb_gcmt_sid', lpb_stat_readio i4 not null not \
default is 'exp.dmf.lg.lpb_stat.readio', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"lpb_stat_write i4 not null not default is 'exp.dmf.lg.lpb_stat.write'\
, lpb_stat_force i4 not null not default is 'exp.dmf.lg.lpb_stat.force\
', lpb_stat_wait i4 not null not default is 'exp.dmf.lg.lpb_stat.wait'\
, lpb_stat_begin i4 not null not default is ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'exp.dmf.lg.lpb_stat.begin', lpb_stat_end i4 not null not default is \
'exp.dmf.lg.lpb_stat.end')as import from 'tables' with dbms=IMA, struc\
ture=unique sortkeyed, key=(place, lpb_id_id)");
    IIsyncup((char *)0,0);
  }
/* # line 4481 "ducommon.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	    sqlerr_handler(duc_errcb, W_DU186B_REGISTER_FAILED);
    }
/* # line 4485 "ducommon.sc" */	/* register table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register table lgmo_ldb(place varchar(4)not null not default is 'VNOD\
E', ldb_id_instance i4 not null not default is 'exp.dmf.lg.ldb_id.id_i\
nstance', ldb_id_id i4 not null not default is 'exp.dmf.lg.ldb_id.id_i\
d', ldb_status char(100)not null not default ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"is 'exp.dmf.lg.ldb_status', ldb_status_num i4 not null not default is\
 'exp.dmf.lg.ldb_status_num', ldb_l_buffer i4 not null not default is \
'exp.dmf.lg.ldb_l_buffer', ldb_buffer char(200)not null not default is\
 'exp.dmf.lg.ldb_buffer', ldb_db_name char(32)");
    IIwritio(0,(short *)0,1,32,0,(char *)
"not null not default is 'exp.dmf.lg.ldb_db_name', ldb_db_owner char(3\
2)not null not default is 'exp.dmf.lg.ldb_db_owner', ldb_j_first_lga c\
har(30)not null not default is 'exp.dmf.lg.ldb_j_first_cp', ldb_j_firs\
t_high i4 not null not default is ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'exp.dmf.lg.ldb_j_first_cp.lga_high', ldb_j_first_low i4 not null not\
 default is 'exp.dmf.lg.ldb_j_first_cp.lga_low', ldb_j_last_lga char(3\
0)not null not default is 'exp.dmf.lg.ldb_j_last_cp', ldb_j_last_high \
i4 not null not default is ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'exp.dmf.lg.ldb_j_last_cp.lga_high', ldb_j_last_low i4 not null not d\
efault is 'exp.dmf.lg.ldb_j_last_cp.lga_low', ldb_d_first_lga char(30)\
not null not default is 'exp.dmf.lg.ldb_d_first_cp', ldb_d_first_high \
i4 not null not default is ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'exp.dmf.lg.ldb_d_first_cp.lga_high', ldb_d_first_low i4 not null not\
 default is 'exp.dmf.lg.ldb_d_first_cp.lga_low', ldb_d_last_lga char(3\
0)not null not default is 'exp.dmf.lg.ldb_d_last_cp', ldb_d_last_high \
i4 not null not default is ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'exp.dmf.lg.ldb_d_last_cp.lga_high', ldb_d_last_low i4 not null not d\
efault is 'exp.dmf.lg.ldb_d_last_cp.lga_low', ldb_sbackup_lga char(30)\
not null not default is 'exp.dmf.lg.ldb_sbackup', ldb_sbackup_high i4 \
not null not default is ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'exp.dmf.lg.ldb_sbackup.lga_high', ldb_sbackup_low i4 not null not de\
fault is 'exp.dmf.lg.ldb_sbackup.lga_low', ldb_stat_read i4 not null n\
ot default is 'exp.dmf.lg.ldb_stat.read', ldb_stat_write i4 not null n\
ot default is 'exp.dmf.lg.ldb_stat.write', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"ldb_stat_begin i4 not null not default is 'exp.dmf.lg.ldb_stat.begin'\
, ldb_stat_end i4 not null not default is 'exp.dmf.lg.ldb_stat.end', l\
db_stat_force i4 not null not default is 'exp.dmf.lg.ldb_stat.force', \
ldb_stat_wait i4 not null not default is ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'exp.dmf.lg.ldb_stat.wait')as import from 'tables' with dbms=IMA, str\
ucture=unique sortkeyed, key=(place, ldb_id_id)");
    IIsyncup((char *)0,0);
  }
/* # line 4549 "ducommon.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	    sqlerr_handler(duc_errcb, W_DU186B_REGISTER_FAILED);
    }
/* # line 4553 "ducommon.sc" */	/* register table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register table lgmo_lxb(place varchar(4)not null not default is 'VNOD\
E', lxb_id_instance i4 not null not default is 'exp.dmf.lg.lxb_id.id_i\
nstance', lxb_id_id i4 not null not default is 'exp.dmf.lg.lxb_id.id_i\
d', lxb_status char(100)not null not default ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"is 'exp.dmf.lg.lxb_status', lxb_status_num i4 not null not default is\
 'exp.dmf.lg.lxb_status_num', lxb_db_name char(32)not null not default\
 is 'exp.dmf.lg.lxb_db_name', lxb_db_owner char(32)not null not defaul\
t is 'exp.dmf.lg.lxb_db_owner', lxb_db_id_id ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"i4 not null not default is 'exp.dmf.lg.lxb_db_id_id', lxb_pr_id_id i4\
 not null not default is 'exp.dmf.lg.lxb_pr_id_id', lxb_wait_reason i4\
 not null not default is 'exp.dmf.lg.lxb_wait_reason', lxb_sequence i4\
 not null not default is ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'exp.dmf.lg.lxb_sequence', lxb_first_lga char(30)not null not default\
 is 'exp.dmf.lg.lxb_first_lga', lxb_first_high i4 not null not default\
 is 'exp.dmf.lg.lxb_first_lga.lga_high', lxb_first_low i4 not null not\
 default is 'exp.dmf.lg.lxb_first_lga.lga_low'");
    IIwritio(0,(short *)0,1,32,0,(char *)
", lxb_last_lga char(30)not null not default is 'exp.dmf.lg.lxb_last_l\
ga', lxb_last_high i4 not null not default is 'exp.dmf.lg.lxb_last_lga\
.lga_high', lxb_last_low i4 not null not default is 'exp.dmf.lg.lxb_la\
st_lga.lga_low', lxb_cp_lga char(30)not null ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"not default is 'exp.dmf.lg.lxb_cp_lga', lxb_cp_high i4 not null not d\
efault is 'exp.dmf.lg.lxb_cp_lga.lga_high', lxb_cp_low i4 not null not\
 default is 'exp.dmf.lg.lxb_cp_lga.lga_low', lxb_tran_id char(16)not n\
ull not default is 'exp.dmf.lg.lxb_tran_id', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"lxb_tran_high i4 not null not default is 'exp.dmf.lg.lxb_tran_id.db_h\
igh_tran', lxb_tran_low i4 not null not default is 'exp.dmf.lg.lxb_tra\
n_id.db_low_tran', lxb_dis_tran_id_hexdump char(350)not null not defau\
lt is 'exp.dmf.lg.lxb_dis_tran_id_hexdump', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"lxb_pid i4 not null not default is 'exp.dmf.lg.lxb_pid', lxb_sid i4 n\
ot null not default is 'exp.dmf.lg.lxb_sid', lxb_dmu_cnt i4 not null n\
ot default is 'exp.dmf.lg.lxb_dmu_cnt', lxb_user_name char(100)not nul\
l not default is 'exp.dmf.lg.lxb_user_name', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"lxb_is_prepared char(1)not null not default is 'exp.dmf.lg.lxb_is_pre\
pared', lxb_is_xa_dis_tran_id char(1)not null not default is 'exp.dmf.\
lg.lxb_is_xa_dis_tran_id', lxb_stat_split i4 not null not default is '\
exp.dmf.lg.lxb_stat.split', lxb_stat_write i4 ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"not null not default is 'exp.dmf.lg.lxb_stat.write', lxb_stat_force i\
4 not null not default is 'exp.dmf.lg.lxb_stat.force', lxb_stat_wait i\
4 not null not default is 'exp.dmf.lg.lxb_stat.wait')as import from 't\
ables' with dbms=IMA, structure=unique ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"sortkeyed, key=(place, lxb_id_id)");
    IIsyncup((char *)0,0);
  }
/* # line 4625 "ducommon.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	    sqlerr_handler(duc_errcb, W_DU186B_REGISTER_FAILED);
    }
/* # line 4629 "ducommon.sc" */	/* register table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register table lgmo_xa_reduced_reg(place varchar(4)not null not defau\
lt is 'VNODE', lxb_id_id i4 not null not default is 'exp.dmf.lg.lxb_id\
.id_id', lxb_dis_tran_id_hexdump char(350)not null not default is 'exp\
.dmf.lg.lxb_dis_tran_id_hexdump', lxb_db_name ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"char(32)not null not default is 'exp.dmf.lg.lxb_db_name', lxb_is_prep\
ared char(1)not null not default is 'exp.dmf.lg.lxb_is_prepared', lxb_\
is_xa_dis_tran_id char(1)not null not default is 'exp.dmf.lg.lxb_is_xa\
_dis_tran_id')as import from 'tables' with ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"dbms=IMA, structure=unique sortkeyed, key=(place, lxb_id_id)");
    IIsyncup((char *)0,0);
  }
/* # line 4645 "ducommon.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	    sqlerr_handler(duc_errcb, W_DU186B_REGISTER_FAILED);
    }
/* # line 4649 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view lgmo_xa_dis_tran_ids(xa_dis_tran_id, xa_seqnum, xa_flags,\
 xa_database_name)as select left(TRIM(lxb_dis_tran_id_hexdump), LOCATE\
(TRIM(lxb_dis_tran_id_hexdump), ':XA') +2), INT4(left(right(TRIM(lxb_d\
is_tran_id_hexdump), (LENGTH(TRIM(");
    IIwritio(0,(short *)0,1,32,0,(char *)
"lxb_dis_tran_id_hexdump)) -LOCATE(TRIM(lxb_dis_tran_id_hexdump), ':XA\
:')) -3), LOCATE(right(TRIM(lxb_dis_tran_id_hexdump), (LENGTH(TRIM(lxb\
_dis_tran_id_hexdump)) -LOCATE(TRIM(lxb_dis_tran_id_hexdump), ':XA:'))\
 -3), ':') -1)), INT4(left(right(right(TRIM(");
    IIwritio(0,(short *)0,1,32,0,(char *)
"lxb_dis_tran_id_hexdump), (LENGTH(TRIM(lxb_dis_tran_id_hexdump)) -LOC\
ATE(TRIM(lxb_dis_tran_id_hexdump), ':XA:')) -3), LENGTH(right(TRIM(lxb\
_dis_tran_id_hexdump), (LENGTH(TRIM(lxb_dis_tran_id_hexdump)) -LOCATE(\
TRIM(lxb_dis_tran_id_hexdump), ':XA:')) -3)) -");
    IIwritio(0,(short *)0,1,32,0,(char *)
"LOCATE(right(TRIM(lxb_dis_tran_id_hexdump), (LENGTH(TRIM(lxb_dis_tran\
_id_hexdump)) -LOCATE(TRIM(lxb_dis_tran_id_hexdump), ':XA:')) -3), ':'\
)), LOCATE(right(right(TRIM(lxb_dis_tran_id_hexdump), (LENGTH(TRIM(lxb\
_dis_tran_id_hexdump)) -LOCATE(TRIM(");
    IIwritio(0,(short *)0,1,32,0,(char *)
"lxb_dis_tran_id_hexdump), ':XA:')) -3), LENGTH(right(TRIM(lxb_dis_tra\
n_id_hexdump), (LENGTH(TRIM(lxb_dis_tran_id_hexdump)) -LOCATE(TRIM(lxb\
_dis_tran_id_hexdump), ':XA:')) -3)) -LOCATE(right(TRIM(lxb_dis_tran_i\
d_hexdump), (LENGTH(TRIM(");
    IIwritio(0,(short *)0,1,32,0,(char *)
"lxb_dis_tran_id_hexdump)) -LOCATE(TRIM(lxb_dis_tran_id_hexdump), ':XA\
:')) -3), ':')), ':EX') -1)), lxb_db_name from lgmo_xa_reduced_reg whe\
re lxb_is_prepared='Y' and lxb_is_xa_dis_tran_id='Y'");
    IIsyncup((char *)0,0);
  }
/* # line 4752 "ducommon.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	    sqlerr_handler(duc_errcb, W_DU186B_REGISTER_FAILED);
    }
/* # line 4756 "ducommon.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 4757 "ducommon.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on lgmo_xa_dis_tran_ids to public");
    IIsyncup((char *)0,0);
  }
/* # line 4758 "ducommon.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	    sqlerr_handler(duc_errcb, W_DU186B_REGISTER_FAILED);
    }
/* # line 4762 "ducommon.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 4763 "ducommon.sc" */	/* host code */
}
/*{
** Name:  duc_iischema() -- build system catalog iischema
**
** Description:
**        This routine makes the catalog iischema
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	13 December 1992 (jpk)
**		initial creation.
**	20-jan-93 (anitap)
**	    Fixed bug. Changed schema_name and schema_owner to be 32 instead of
**	    24. Added schema_idx.
**	08-feb-93 (anitap)
**	    Changed the columns in iischema to be non-nullable.
**	2-mar-93 (rickh)
**	    Add tuple to iischema, binding $ingres to its canonical schema id.
**	09-nov-93 (rblumer)
**	    Changed hard-coded $ingres schema name to be upper/lower case
**	    depending on case-translation semantics.
**	    Added upper_case flag to tell us which way to go.
**	19-jul-94 (robf)
**          Make sure error diagnostic on failure to insert is reported.
*/
VOID
duc_iischema(upper_case)
i4  upper_case;
{
  i4 id1;
  char owner[DB_MAXNAME+1];
/* # line 4808 "ducommon.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iischema(schema_name char(32)not null, schema_owner char\
(32)not null, schema_id i4 not null, schema_idx i4 not null)");
    IIsyncup((char *)0,0);
  }
/* # line 4815 "ducommon.sc" */	/* host code */
    /* populate iischema with canonical schema ids */
	if (upper_case)
	    MEmove(sizeof(ERx("$ingres")) - 1, (PTR) ERx("$INGRES"), ' ',
		   sizeof(owner), owner);
	else
	    MEmove(sizeof(ERx("$ingres")) - 1, (PTR) ERx("$ingres"), ' ',
		   sizeof(owner), owner);
	owner[sizeof(owner)-1] = EOS;
	id1 = DB_INGRES_SCHEMA_ID;
/* # line 4824 "ducommon.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iischema(schema_name, schema_owner, schema_id, schema_idx\
)values(");
    IIputdomio((short *)0,1,32,0,owner);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,owner);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0)");
    IIsyncup((char *)0,0);
  }
/* # line 4830 "ducommon.sc" */	/* host code */
	if(sqlca.sqlcode<0)
	{
		duc_sqlerror();
	}
}
/*{
** Name:  duc_v4_iisecurity_alarms() -- build catalog iisecurity_alarms
**
** Description:
**        This routine makes the catalog iisecurity_alarms.
**
** Inputs:
**	is_dbdb - TRUE if creating iidbdb
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	11-oct-91 (teresa)
**		initial creation.
**	14-Jan-93 (jpk)
**		modify definition to use gmt_timestamp
**	26-oct-93 (andre)
**	    creation date will be obtained by applying gmt_timestamp() to the
**	    different iiprotect attribute - procrtime1 (proqryid1 will be based 
**	    on id of the object)
**
**	    modified definition of iisecurity_alarms to take advantage of the 
**	    fact that object name and owner are now available from IIPROTECT 
**
**	    qualification of the view must include "AND q.mode = 19" because it
**	    is possible for a permit/security_alarm and integrity/constraint 
** 	    defined on the same table to have identical query text id
**	6-dec-93 (robf)
**          Rewritten to reference iisecalarm instead of iiprotect,
**	    also add new fields for named alarms, database alarms. 
**	21-dec-93 (robf)
**          Pick up dbevent name/owner from iievent via event id
**      18-jul-94 (robf)
**          Add error check, updated iidbdb definition since failed on
**          upgradedb (assumed previous range variable r defined)
*/
static VOID
duc_v_iisecurity_alarms(bool is_dbdb)
{
    if(is_dbdb)
    {
	/*
	** DBDB, list table and database/installation alarms
	*/
/* # line 4887 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iisecurity_alarms(alarm_name, object_name, object_owner, \
object_type, create_date, subject_type, security_user, security_number\
, dbevent_name, dbevent_owner, dbevent_text, text_sequence, text_segme\
nt)as select a.alarm_name, r.relid, r.relowner");
    IIwritio(0,(short *)0,1,32,0,(char *)
", char(a.obj_type), gmt_timestamp(a.alarm_txtid1), byteextract('UGRP'\
, int4(a.subj_type) +1), a.subj_name, int2(a.alarm_num), e.event_name,\
 e.event_owner, a.event_text, int2(q.seq) +1, q.txt from iirelation r,\
 iiqrytext q, iisecalarm a left join iievent e");
    IIwritio(0,(short *)0,1,32,0,(char *)
" on e.event_idbase=a.event_id1 and e.event_idx=a.event_id2 where r.re\
ltid=a.obj_id1 and r.reltidx=a.obj_id2 and a.alarm_txtid1=q.txtid1 and\
 a.alarm_txtid2=q.txtid2 and a.obj_type='T' union all select a.alarm_n\
ame, a.obj_name, d.own, 'D', gmt_timestamp(a.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"alarm_txtid1), byteextract('UGRP', int4(a.subj_type) +1), a.subj_name\
, int2(a.alarm_num), e.event_name, e.event_owner, a.event_text, int2(q\
.seq) +1, q.txt from iiqrytext q, iidatabase d, iisecalarm a left join\
 iievent e on e.event_idbase=a.event_id1 and e");
    IIwritio(0,(short *)0,1,32,0,(char *)
".event_idx=a.event_id2 where d.name=a.obj_name and a.alarm_txtid1=q.t\
xtid1 and a.alarm_txtid2=q.txtid2 and a.obj_name!='' and a.obj_type='D\
' union all select a.alarm_name, '', '', 'I', gmt_timestamp(a.alarm_tx\
tid1), byteextract('UGRP', int4(a.subj_type) +");
    IIwritio(0,(short *)0,1,32,0,(char *)
"1), a.subj_name, int2(a.alarm_num), e.event_name, e.event_owner, a.ev\
ent_text, int2(q.seq) +1, q.txt from iiqrytext q, iisecalarm a left jo\
in iievent e on e.event_idbase=a.event_id1 and e.event_idx=a.event_id2\
 where a.alarm_txtid1=q.txtid1 and a.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"alarm_txtid2=q.txtid2 and a.obj_name='' and a.obj_type='D'");
    IIsyncup((char *)0,0);
  }
/* # line 4979 "ducommon.sc" */	/* host code */
    }
    else
    {
	/*
	** Regular databases, list just table alarms
	*/
/* # line 4986 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iisecurity_alarms(alarm_name, object_name, object_owner, \
object_type, create_date, subject_type, security_user, security_number\
, dbevent_name, dbevent_owner, dbevent_text, text_sequence, text_segme\
nt)as select a.alarm_name, r.relid, r.relowner");
    IIwritio(0,(short *)0,1,32,0,(char *)
", char(a.obj_type), gmt_timestamp(a.alarm_txtid1), byteextract('UGRP'\
, int4(a.subj_type) +1), a.subj_name, int2(a.alarm_num), e.event_name,\
 e.event_owner, a.event_text, int2(q.seq) +1, q.txt from iirelation r,\
 iiqrytext q, iisecalarm a left join iievent e");
    IIwritio(0,(short *)0,1,32,0,(char *)
" on e.event_idbase=a.event_id1 and e.event_idx=a.event_id2 where r.re\
ltid=a.obj_id1 and r.reltidx=a.obj_id2 and a.alarm_txtid1=q.txtid1 and\
 a.alarm_txtid2=q.txtid2 and a.obj_type='T'");
    IIsyncup((char *)0,0);
  }
/* # line 5026 "ducommon.sc" */	/* host code */
    }
    if(sqlca.sqlcode<0)
    {
	/*
	** Print SQL error message if present.
	*/
	duc_sqlerror();
    }
}
/*{
** Name:  duc_m1_dbmscomment() -- modify iidbms_comment
**
** Description:
**        This routine modifies the system catalog to its desired storage
**	structure.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	14-oct-91 (teresa)
**		initial creation.
**	15-oct-1993 (jpk)
**		change keying; have text_sequence as
**		part of unique key to allow comments
**		longer than one line.
**	
*/
VOID
duc_m1_dbmscomment()
{
/* # line 5046 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidbms_comment to btree unique on comtabbase, comtabidx, comat\
tid, text_sequence with fillfactor=100");
    IIsyncup((char *)0,0);
  }
/* # line 5049 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_m2_synonym() -- modify iisynonyn
**
** Description:
**        This routine modifies the system catalog to its desired storage
**	structure.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	14-oct-91 (teresa)
**		initial creation.
**	15-oct-93 (andre)
**		added code to create IIXSYNONYM
*/
VOID
duc_m2_synonym()
{
/* # line 5080 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iisynonym to hash unique on synonym_name, synonym_owner");
    IIsyncup((char *)0,0);
  }
/* # line 5081 "ducommon.qsc" */	/* index */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"index on iisynonym=iixsynonym(syntabbase, syntabidx)with structure=bt\
ree");
    IIsyncup((char *)0,0);
  }
/* # line 5083 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_m7_priv() -- modify iipriv
**
** Description:
**        Modify IIPRIV to its desired storage structure.
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	29-may-92 (andre)
**		initial creation.
**	10-feb-93 (jpk)
**		add index from duc_iixpriv
*/
VOID
duc_m7_priv()
{
/* # line 5113 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iipriv to btree on d_obj_id, d_obj_idx, d_priv_number with fil\
lfactor=100");
    IIsyncup((char *)0,0);
  }
/* # line 5115 "ducommon.qsc" */	/* index */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"index on iipriv=iixpriv(i_obj_id, i_obj_idx, i_priv, i_priv_grantee)w\
ith structure=btree, key=(i_obj_id, i_obj_idx, i_priv)");
    IIsyncup((char *)0,0);
  }
/* # line 5117 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_m3_sectype() - modify iisectype
**
** Description:
**        This routine modifies the system catalog to its desired storage
**	structure.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	14-oct-91 (teresa)
**		initial creation.
*/
VOID
duc_m3_sectype()
{
/* # line 5145 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iisectype to BTREE unique on sec_type, sec_name");
    IIsyncup((char *)0,0);
  }
/* # line 5146 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_m4_protect() - modify iiprotect
**
** Description:
**        This routine modifies the system catalog to its desired storage
**	structure.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	14-oct-91 (teresa)
**		initial creation.
**	13-dec-92 (jpk)
**		add index on iiprotect
**	29-Dec-2004 (schka24)
**	    remove iixprotect index; change iiprotect to btree because we
**	    dump many more rows into it than 6.4 and earlier did.  Hash
**	    quickly builds up long hash chains, and there's no good initial
**	    minpages to use.
*/
VOID
duc_m4_protect()
{
/* # line 5181 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iiprotect to btree on protabbase, protabidx with fillfactor=10\
0");
    IIsyncup((char *)0,0);
  }
/* # line 5183 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_m9_schema() - modify iischema
**
** Description:
**        This routine modifies the system catalog to its desired storage
**	structure.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	13 December 1992 (jpk)
**		initial creation.
**	20-jan-93 (anitap)
**	    Fixed bug. Added schema_idx to the hash unique and the index lists. 
**	08-feb-93 (anitap)
**	    iischema catalog will be keyed only on schema name.
**	27-apr-93 (anitap)
**	    index on iischema will be hash. 
*/
VOID
duc_m9_schema()
{
/* # line 5216 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iischema to hash unique on schema_name where minpages=8");
    IIsyncup((char *)0,0);
  }
/* # line 5218 "ducommon.qsc" */	/* index */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"index on iischema=iischemaidx(schema_id, schema_idx)with structure=ha\
sh");
    IIsyncup((char *)0,0);
  }
/* # line 5220 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_m5_user() - modify iiuser
**
** Description:
**        This routine modifies the system catalog to its desired storage
**	structure.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	14-oct-91 (teresa)
**		initial creation.
*/
VOID
duc_m5_user()
{
/* # line 5248 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iiuser to hash unique on name where minpages=4");
    IIsyncup((char *)0,0);
  }
/* # line 5250 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_m6_dbpriv() - modify iidbpriv
**
** Description:
**        This routine modifies the system catalog to its desired storage
**	structure.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	14-oct-91 (teresa)
**		initial creation.
*/
VOID
duc_m6_dbpriv() 
{
	/*
	** Note: iidbpriv should remain btree since QEF will
	** perform partial searches that are not allowed against
	** hashed tables
	*/
/* # line 5283 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidbpriv to btree unique on grantee, gtype, dbname with fillfa\
ctor=100");
    IIsyncup((char *)0,0);
  }
/* # line 5285 "ducommon.qsc" */	/* host code */
}
/*{
** Name: mk_idxname - Build a star INDEX name from base and index id.
**
** Description:
**      This converts a table base id to ascii, a table index id to
**	ascii, then builds a star index name as:
**	    ddx_<base_id>_<index_id>
**	and places the index name into the string provided by the caller.
**
**	example:
**	    call with base=500, index = 721
**	    builds index name:	ddx_500_721
**
** Inputs:
**	base	-  int containing table base id
**	index	-  int containing table index id
**	name	-  pointer to char string long enought to hold name (ie,
**			DB_MAXNAME+1)
**
** Outputs:
**      *name	-   name of star index identifier as built from base and index
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	26-jun-90 (teresa)
**	    initial creation.
*/
VOID
mk_idxname (name, base, index)
char	*name;
int	base;
int	index;
{
#define	    PREFIX  "iiddx_"
#define	    PREFIX_SIZE 6
    char    base_str[DB_MAXNAME];
    char    index_str[DB_MAXNAME];
    i4	    b_len, i_len;
    CVna (base,base_str);
    CVna (index,index_str);
    b_len = STtrmwhite (base_str);
    i_len = STtrmwhite (index_str);
    if ( (PREFIX_SIZE + b_len + i_len + 1) > DB_MAXNAME )
    {
	/* Name is too long to fix into string, so use shorter version */
	(VOID) STpolycat( 2, PREFIX, index_str, name);
    }
    else
    {	/* the usual case */
	(VOID) STpolycat( 4, PREFIX, base_str, "_", index_str, name);
    }
}
/*{
** Name:  duc_register_starcats() - Register star catalogs
**
** Description:
**        This routine populates star catalogs with information from the CDB
**	so that star will be able to map its catalogs to CDB catalogs.  It
**	populates the following catalogs:
**		iidd_tables
**		iidd_columns
**		iidd_ddb_objects
**		iidd_ddb_tbleinfo
**	also, it populates information about secondary indexes (if any) for
**	these tables.
**
**	The information of what to populate is table driven, and the tables
**	can be found in header file dustar.qsh.
**
** Inputs:
**	object_base	starting object base
**	map		array of DU_link_object_entry structures, used to
**			populate the star catalogs
**	idx_map		array of DU_index_object_entry structures, used to
**			populate the star catalogs with secondary index info
**	cdbname		name of the CDB
**	duc_errcb	ERROR CONTROL BLOCK
**
** Outputs:
**	err_cb->du_errmsg	- formatted error message if error is
*				  encountered
**	Returns:
**	    E_DU_OK
**	    E_DU_ERROR
**	Exceptions:
**	    none
**
** Side Effects:
**	      iidd_ddb_object_base is incremented
**
** History:
**	14-oct-91 (teresa)
**		initial creation.
*/
DU_STATUS
  duc_register_starcats(object_base, map, idx_map, cdbname, duc_errcb)
i4			object_base;
DU_link_object_entry    map[];
DU_index_object_entry   idx_map[];
char			*cdbname;
DU_ERROR		*duc_errcb;
  {
    i4		object_index=0;
    char *DU_ii_name, *DU_iidd_name, *DU_obj_type, *DU_loc_type;
    char *DU_ldbidx_nam;
    i4 DU_object_base, DU_index;
    i4 obj_base;
    i4 row_cnt;
    DU_STATUS	stat;
    /*
    **	Initialize "iidd_ddb_*" tables in CDB.
    */
    for (object_index = 0;
	STcompare((char *)(map[object_index].ii_name), "");
	object_index++, object_base++)
    {
	DU_ii_name	= map[object_index].ii_name;
	DU_iidd_name	= map[object_index].iidd_name;
	DU_obj_type	= map[object_index].obj_type;
	DU_loc_type	= map[object_index].loc_type;
	DU_object_base	= object_base;
	/* Initialize iidd_tables and substitute iidd_name into ii_name */
/* # line 5423 "ducommon.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducommon1",6220,5214);
        IIputdomio((short *)0,1,32,0,DU_ii_name);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducommon1",6220,5214);
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
          IIexDefine(0,(char *)"ducommon1",6220,5214);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 5469 "ducommon.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
    }
/* # line 5470 "ducommon.qsc" */	/* host code */
	if (row_cnt == 0)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    stat = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iidd_tables", 0, cdbname);
	}
/* # line 5477 "ducommon.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducommon2",6221,3927);
        IIputdomio((short *)0,1,32,0,DU_ii_name);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducommon2",6221,3927);
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
          IIexDefine(0,(char *)"ducommon2",6221,3927);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 5494 "ducommon.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
    }
/* # line 5495 "ducommon.qsc" */	/* host code */
	if (row_cnt == 0)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    stat = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iidd_columns", 0, cdbname);
	}
/* # line 5502 "ducommon.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducommon3",6222,13774);
        IIputdomio((short *)0,1,30,4,&DU_object_base);
        IIputdomio((short *)0,1,32,0,DU_loc_type);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducommon3",6222,13774);
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
          IIexDefine(0,(char *)"ducommon3",6222,13774);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 5518 "ducommon.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
    }
/* # line 5519 "ducommon.qsc" */	/* host code */
	if (row_cnt == 0)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    stat = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iidd_ddb_tableinfo",
				0, cdbname);
	}
/* # line 5527 "ducommon.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducommon4",6223,15533);
        IIputdomio((short *)0,1,32,0,DU_ii_name);
        IIputdomio((short *)0,1,30,4,&DU_object_base);
        IIputdomio((short *)0,1,32,0,DU_obj_type);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducommon4",6223,15533);
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
          IIexDefine(0,(char *)"ducommon4",6223,15533);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 5543 "ducommon.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
    }
/* # line 5544 "ducommon.qsc" */	/* host code */
	if (row_cnt == 0)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    stat = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iidd_ddb_objects", 0, cdbname);
	}
    }
    DU_object_base++;
/* # line 5553 "ducommon.qsc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidd_ddb_object_base(object_base=");
      IIputdomio((short *)0,1,30,4,&DU_object_base);
      IIwritio(0,(short *)0,1,32,0,(char *)")");
      IIsyncup((char *)0,0);
    }
/* # line 5556 "ducommon.qsc" */	/* host code */
    /* now populate CDB catalogs to propogate any indexes from System
    ** catalogs put into CDB catalogs */
    for (object_index = 0, DU_index = DU_object_base;
	STcompare((char *)(idx_map[object_index].loctab_name), "");
	object_index++, DU_index++)
    {
	DU_ii_name	= idx_map[object_index].loctab_name;
	DU_iidd_name	= idx_map[object_index].link_name;
	DU_ldbidx_nam 	= idx_map[object_index].locidx_name;
	DU_obj_type	= "L",
	DU_loc_type	= "I";
	/* Initialize iidd_tables and substitute iidd_name into ii_name */
/* # line 5571 "ducommon.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(obj_base=iidd_ddb_objects.object_base)where iidd_ddb_objects\
.object_name=");
      IIputdomio((short *)0,1,32,0,DU_ii_name);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&obj_base);
        if (IIerrtest() != 0) goto IIrtB2;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE2:;
    }
/* # line 5574 "ducommon.qsc" */	/* host code */
	mk_idxname (DU_iidd_name, obj_base, DU_index);
/* # line 5576 "ducommon.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducommon5",6224,13495);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducommon5",6224,13495);
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
          IIexDefine(0,(char *)"ducommon5",6224,13495);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 5622 "ducommon.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
    }
/* # line 5623 "ducommon.qsc" */	/* host code */
	if (row_cnt == 0)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    stat = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iidd_tables", 0, cdbname);
	}
/* # line 5630 "ducommon.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducommon6",6225,3794);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducommon6",6225,3794);
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
          IIexDefine(0,(char *)"ducommon6",6225,3794);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 5647 "ducommon.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
    }
/* # line 5648 "ducommon.qsc" */	/* host code */
	if (row_cnt == 0)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    stat = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iidd_columns", 0, cdbname);
	}
/* # line 5655 "ducommon.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducommon7",6226,5495);
        IIputdomio((short *)0,1,30,4,&DU_object_base);
        IIputdomio((short *)0,1,30,4,&DU_index);
        IIputdomio((short *)0,1,32,0,DU_loc_type);
        IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
        IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducommon7",6226,5495);
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
          IIexDefine(0,(char *)"ducommon7",6226,5495);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 5671 "ducommon.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
    }
/* # line 5672 "ducommon.qsc" */	/* host code */
	if (row_cnt == 0)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    stat = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iidd_ddb_tableinfo",
				0, cdbname);
	}
/* # line 5680 "ducommon.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducommon8",6227,7252);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIputdomio((short *)0,1,30,4,&DU_object_base);
        IIputdomio((short *)0,1,30,4,&DU_index);
        IIputdomio((short *)0,1,32,0,DU_obj_type);
        IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducommon8",6227,7252);
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
          IIexDefine(0,(char *)"ducommon8",6227,7252);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 5696 "ducommon.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducommon9",6228,6827);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducommon9",6228,6827);
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
          IIexDefine(0,(char *)"ducommon9",6228,6827);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 5711 "ducommon.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
    }
/* # line 5712 "ducommon.qsc" */	/* host code */
	if (row_cnt == 0)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    stat = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iidd_indexes", 0,cdbname);
	}
/* # line 5719 "ducommon.qsc" */	/* append */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIexExec(1,(char *)"ducommon10",6229,29909);
        IIputdomio((short *)0,1,32,0,DU_iidd_name);
        IIputdomio((short *)0,1,32,0,DU_ldbidx_nam);
        IIsyncup((char *)0,0);
        if (IInexec() == 0) {
          IIexDefine(1,(char *)"ducommon10",6229,29909);
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
          IIexDefine(0,(char *)"ducommon10",6229,29909);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
    }
/* # line 5730 "ducommon.qsc" */	/* inquire_equel */
    {
      IIeqiqio((short *)0,1,30,4,&row_cnt,(char *)"rowcount");
    }
/* # line 5731 "ducommon.qsc" */	/* host code */
	if (row_cnt == 0)
	{
	    /* Bad append to table, %0c, in database, %1c. */
	    stat = du_error(duc_errcb, E_DU3200_BAD_APPEND, 4,
				0, "iidd_index_columns", 
				0, cdbname);
	}
    }
    DU_index++;
/* # line 5741 "ducommon.qsc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidd_ddb_object_base(object_base=");
      IIputdomio((short *)0,1,30,4,&DU_index);
      IIwritio(0,(short *)0,1,32,0,(char *)")");
      IIsyncup((char *)0,0);
    }
/* # line 5743 "ducommon.qsc" */	/* host code */
    return (E_DU_OK);
  }
/*{
** Name:  duc_r_iiaudit -- build catalog iiaudit
**
** Description:
**
**	Definition for the iipermits registration.
**
**	NOTE: iigwsxa MUST have been called prior to this otherwise
**	      the REGISTER will fail
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	08-oct-92 (robf)
**	   initial creation.
**	18-nov-92 (robf)
**	   Add check for failure just in case global error handling
**	   doesn't pick it up.
**	9-jul-93 (robf)
**         Added detailinfo/detailnum/sessionid/objectlabel fields
**	23-dec-93 (robf)
**         Added querytext_sequence
**	18-jul-94 (robf)
**         Initialiaze errcb flags values, this was causing errors in
**         upgradedb to get lost.
*/
VOID
duc_r_iiaudit()
{
	/*
	**	This is an iidbdb-only catalog right now
	*/
/* # line 6068 "ducommon.sc" */	/* register table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register table iiaudit(audittime ingresdate not null, user_name char(\
32)not null, real_name char(32)not null, userprivileges char(32)not nu\
ll, objprivileges char(32)not null, database char(32)not null, auditst\
atus char(1)not null, auditevent char(24)not ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"null, objecttype char(24)not null, objectname char(32)not null, objec\
towner char(32)not null, description char(80)not null, sessionid char(\
16)not null, detailinfo char(256)not null, detailnum integer not null,\
 querytext_sequence integer not null)as import");
    IIwritio(0,(short *)0,1,32,0,(char *)
" from 'current' with DBMS='sxa', NOUPDATE, STRUCTURE=NONE");
    IIsyncup((char *)0,0);
  }
/* # line 6088 "ducommon.sc" */	/* host code */
	if (sqlca.sqlcode<0)
	{
		/*
		**	REGISTER went wrong so make sure a diagnostic is
		**	issued. 
		*/
		duc_sqlerror();
	}
}
/*{
** Name:  duc_iigwsxa() - Create catalogs for C2 Security Audit Gateway (SXA)
**
** Description:
**        This routine creates the various catalogs required for
**	  the C2 Security Audit Gateway (SXA):
**	  It populates the following catalogs:
**		iigw06_relation
**		iigw06_attribute
** Inputs:
**	duc_errcb - Error control block.
**
** Outputs:
**	Returns:
**	    E_DU_OK
**	    E_DU_ERROR
**	Exceptions:
**	    none
**
** Side Effects:
**	      tables/views are created
**
** History:
**	16-sep-92 (robf)
**		initial creation.
**	18-dec-92 (robf)
**		Updated attribute names in iigw06_attribute to 32.
**	16-feb-94 (robf)
**              AUDITOR privilege is needed to view audit logs, not SECURITY.
*/
DU_STATUS 
duc_iigwsxa(duc_errcb)
DU_ERROR	*duc_errcb;
{
	DB_STATUS stat;
	stat=E_DB_OK;
	/*
	**	Create base gateway catalogs
	*/
/* # line 6136 "ducommon.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iigw06_relation(reltid integer4 not null, reltidx intege\
r4 not null, audit_log char(256)not null, reg_date integer4 not null, \
flags integer4 not null)with noduplicates");
    IIsyncup((char *)0,0);
  }
/* # line 6143 "ducommon.sc" */	/* host code */
	if ( sqlca.sqlcode<0)
	{
		/*
		**	
		*/
/*	xxx	SIprintf("about to emit E_DU3204_BAD_CREATE\n");*/
		stat = du_error(duc_errcb, E_DU3204_BAD_CREATE, 2,
				0, "iigw06_relation");
/*		SIprintf("emitted E_DU3204_BAD_CREATE, returning\n");*/
		return E_DB_ERROR;
	}
/* # line 6154 "ducommon.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iigw06_attribute(reltid integer4 not null, reltidx integ\
er4 not null, attid integer2 not null, attname char(32)not null, audit\
id integer2 not null, auditname char(32)not null)with noduplicates");
    IIsyncup((char *)0,0);
  }
/* # line 6162 "ducommon.sc" */	/* host code */
	if ( sqlca.sqlcode<0)
	{
		/*
		**	
		*/
		stat = du_error(duc_errcb, E_DU3204_BAD_CREATE, 2,
				0, "iigw06_attribute");
		return E_DB_ERROR;
	}
	/*
	**	Modify the tables. We MUST do this before trying to
	**	issue the REGISTER statement below or GWF will fail
	**	with a positioning error.
	*/
	duc_m8_gwsxa();
	return E_DB_OK;
}
/*{
** Name:  duc_m8_gwsxa() - Modify catalogs for C2 Security Audit Gateway (SXA)
**
** Description:
**        This routine creates the various catalogs required for
**	  the C2 Security Audit Gateway (SXA):
**	  It updates the following catalogs:
**		iigw06_relation
**		iigw06_attribute
** Inputs:
**	none
**
** Outputs:
**	Returns:
**	    none
**
** Side Effects:
**	      tables are created
**
** History:
**	16-sep-92 (robf)
**		initial creation.
*/
VOID
duc_m8_gwsxa()
{
/* # line 6205 "ducommon.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iigw06_relation to HASH unique on reltid, reltidx");
    IIsyncup((char *)0,0);
  }
/* # line 6208 "ducommon.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iigw06_attribute to HASH on reltid, reltidx");
    IIsyncup((char *)0,0);
  }
/* # line 6211 "ducommon.sc" */	/* host code */
}
/*{
** Name:  du_add_privd_user - add a privileged user.
**
** Description:
**      Add a user with full privs.  Used for initially populating
**	the user table.  Constructs a "create user" statement.
**
** Inputs:
**	user_name	name of user to add
** Outputs:
**	none
** Returns:
**	value of sqlcode returned by the server after the create user stmt
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**	19-feb-93 (jpk)
**	    initial creation
**	02-jun-93 (ralph)
**	    DELIM_IDENT:
**	    Remove delimiters in du_add_privd_user();
**	    if the calls wants them, the caller should specify them.
**	6-jul-93 (robf)
**	    Added new privileges
*/
int
du_add_privd_user(user_name)
char *user_name;
{
  char sbuffer[256];
    IIlq_Protect(TRUE);
    STprintf(sbuffer, "create user %s with privileges =\
(createdb, trace, security, maintain_locations, operator, maintain_audit,\
auditor, maintain_users)",
	user_name);
/* # line 6253 "ducommon.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
  }
/* # line 6254 "ducommon.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	/* XXX this should be msg-ized! */
	SIprintf("]==> <%s> returns %d\n", sbuffer, sqlca.sqlcode);
    }
    else
    {
/* # line 6261 "ducommon.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 6262 "ducommon.sc" */	/* host code */
    }
    return sqlca.sqlcode;
}
/*{
** Name: tbl_lacks_col	- determines whether a table needs 6.5 conversion
**
** Description:
**	
**	Verify the named table doesn't have the named column.
**	Intended to be called to find out if a table is already
**	in 6.5 format, e.g. already has a column.  Thus it is
**	also intended to return TRUE if table itself does not
**	exist.  This still means table needs 6.5 conversion.
**	If it isn't even there it clearly isn't in 6.5 format.
**
** Inputs:
**	table name
**	column name
**
** Outputs:
**	none
**
** Returns:
**	TRUE if table lacks the col, FALSE otherwise
**
** History:
**	January 1993 (jpk)
**	     written
**	07-jan-1997 (nanpr01)
**	    FIPS upgrade bug 87948.
*/
bool
tbl_lacks_col(tname, cname)
  char *tname;
  char *cname;
{
  int tup_cnt;
    /* xxx debugging for upgradedb. should go away in next integration */
    if (upg_dbg_out)
    {
	SIprintf("]==> does tbl %s lack col %s?\n", tname, cname);
    }
/* # line 6308 "ducommon.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiattribute a, iirelation r where lowercase(r.rel\
id)=");
    IIputdomio((short *)0,1,32,0,tname);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and lowercase(r.relowner)='$ingres' and lowercase(a.attname)=");
    IIputdomio((short *)0,1,32,0,cname);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and a.attrelid=r.reltid and a.attrelidx=r.reltidx");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tup_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 6313 "ducommon.sc" */	/* host code */
    if (tup_cnt == 0)
    {
	return TRUE;
    }
    else
    {
	return FALSE;
    }
    /* NOTREACHED */
}
/*{
** Name: rel_exists - return whether table, view, or index already exists in db
**
** Description:
**
**	Modularize idempotency checks.  If table, view, or index already exists
**	in database, don't re-build it.  This routine counts
**	on nothing being right other than iirelation.  It verifies
**	both that the table exists and that it's owned by $ingres.
**
** Inputs:
**	rel_name	name of table, view, or index to check for
**
** Returns:
**	TRUE if table, view, or index exists and is owned by $ingres
**	FALSE otherwise
**
** Outputs:
**	none
**
** History:
**	1-mar-1993 (jpk)
**	    Wrote.
**	11-jun-1993 (ralph)
**	    DELIM_IDENT:
**	    Make search for relation name case insensitive in rel_exists().
**	    We can get away with this because we own the tables,
**	    and we know we don't introduce namespace collision
**	    when treating names insensitively.
**	21-jun-1993 (robf)
**	    Made a repeated select per comment
*/
bool
rel_exists(rel_name)
  char *rel_name;
{
  i4 cnt;
/* # line 6364 "ducommon.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"ducommon2",6221,25715);
      IIputdomio((short *)0,1,32,0,rel_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"ducommon2",6221,25715);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(reltid)from iirelation where lowercase(relid)=lowercase(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,rel_name);
        IIwritio(0,(short *)0,1,32,0,(char *)
")and lowercase(relowner)='$ingres'");
        IIexDefine(0,(char *)"ducommon2",6221,25715);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 6367 "ducommon.sc" */	/* host code */
	/* actually if iirelation is unique on (relid, relowner), cnt
	** could only be 0 or 1, but to make this function robust, the
	** test is simply if 1 or more matches are found
	*/
	if (cnt > 0)
	{
	    return TRUE;
	}
	else
	{
	    return FALSE;
	}
}
/*{
** Name: view_exists - return whether view already exists in db
**
** Description:
**
**	Modularize idempotency checks.  If view already exists
**	in database, don't re-build it.  If not, don't drop it.
**
** Inputs:
**	name		view to check for
**	owner		owner of view
**
** Returns:
**	TRUE if view exists
**	FALSE otherwise
**
** Outputs:
**	none
**
** History:
**	10-aug-1993 (jpk)
*/
bool
view_exists(name)
  char *name;
{
  i4 cnt;
/* # line 6411 "ducommon.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(reltid)from iirelation where lowercase(relid)=lowercase(");
    IIputdomio((short *)0,1,32,0,name);
    IIwritio(0,(short *)0,1,32,0,(char *)
")and lowercase(relowner)='$ingres' and byteextract('TVI', (mod((relst\
at/32), (2)) +(2*(mod((relstat/128), (2))))) +1)='V'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 6416 "ducommon.sc" */	/* host code */
	return (cnt > 0);
}
/*{
** Name: ensure_not_exists - drop table or view if it exists
**
** Description:
**
**	Modularize idempotency checks.  If table or view already
**	exists in database, drop it.  Since this is meant for
**	createdb and upgradedb use, ownership of the table is
**	assumed to be $ingres (owner is ignored for tables).
**	Similarly caller is responsible for setting session
**	authorization first, again presumably to $ingres.
**
** Inputs:
**	name		table or view to check for
**	owner		owner of view
**	type		T or V
**
** Returns:
**	TRUE if view exists
**	FALSE otherwise
**
** Outputs:
**	none
**
** History:
**	10-aug-1993 (jpk)
*/
STATUS
ensure_not_exists(name)
char	*name;
{
  i4 cnt;
  char buff[DB_MAXNAME + 50];
	bool    exists;
/* # line 6454 "ducommon.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(reltid)from iirelation where lowercase(relid)=lowercase(");
    IIputdomio((short *)0,1,32,0,name);
    IIwritio(0,(short *)0,1,32,0,(char *)
")and lowercase(relowner)='$ingres'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 6457 "ducommon.sc" */	/* host code */
	if (cnt > 0)
	{
	    STprintf(buff, "drop %s", name);
/* # line 6460 "ducommon.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(buff);
    IIsyncup((char *)0,0);
  }
/* # line 6461 "ducommon.sc" */	/* host code */
	    if (sqlca.sqlcode<0)
		duc_sqlerror();
	    return ((sqlca.sqlcode < 0) ? FAIL : OK);
	}
	else
	    return (OK);
}
/*{
** Name:  duc_iiprofile() -- build catalog iiprofile
**
** Description:
**      This routine makes the catalog iiprofile
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	25-aug-93 (robf)
**		initial creation.
*/
VOID
duc_iiprofile()
  {
/* # line 6247 "ducommon.qsc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"create iiprofile(name=char(32), status=i4, default_group=char(32), re\
serve=char(8), default_priv=integer4, expire_date=ingresdate, flags_ma\
sk=integer4)");
      IIsyncup((char *)0,0);
    }
/* # line 6254 "ducommon.qsc" */	/* host code */
	/*
	** This is the "default" profile
	*/
/* # line 6257 "ducommon.qsc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append to iiprofile(name=\"\
\")");
      IIsyncup((char *)0,0);
    }
  }
/* # line 6260 "ducommon.qsc" */	/* host code */
/*{
** Name:  duc_m_iiprofile() -- modify catalog iiprofile
**
** Description:
**      This routine modifies the catalog iiprofile
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	25-aug-93 (robf)
**		initial creation.
*/
VOID
duc_m_iiprofile()
  {
/* # line 6285 "ducommon.qsc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"modify iiprofile to hash unique on name");
      IIsyncup((char *)0,0);
    }
  }
/* # line 6289 "ducommon.qsc" */	/* host code */
/*{
** Name:  duc_iisesspriv() -- build catalog iisessionpriv
**
** Description:
**      This routine makes the catalog iisession_privivileges plus
**	the lookup table iiprivlist
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	9-sep-93 (robf)
**		initial creation.
*/
VOID
duc_iisesspriv()
  {
  char *privlist[]= {
	"MAINTAIN_AUDIT",
	"MAINTAIN_USERS",
	"MAINTAIN_LOCATIONS",
	"SECURITY",
	"OPERATOR",
	"AUDITOR",
	"CREATEDB",
	"TRACE",
	"INSERT_UP",
	"INSERT_DOWN",
	"WRITE_UP",
	"WRITE_DOWN",
	"WRITE_FIXED",
	0
	};
  int priv;
/* # line 6591 "ducommon.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iiprivlist(privname char(32)not null not default)");
    IIsyncup((char *)0,0);
  }
/* # line 6593 "ducommon.sc" */	/* host code */
	for(priv=0;privlist[priv]; priv++)
	{
/* # line 6595 "ducommon.sc" */	/* repeat insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"ducommon3",6222,5242);
      IIputdomio((short *)0,1,32,0,privlist[priv]);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"ducommon3",6222,5242);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiprivlist(privname)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,privlist[priv]);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"ducommon3",6222,5242);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 6597 "ducommon.sc" */	/* host code */
	}
/* # line 6598 "ducommon.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 6599 "ducommon.sc" */	/* host code */
  }
/*{
** Name:  duc_iisecalarm() -- build catalog iisecalarm
**
** Description:
**        This routine makes the catalog iisecalarm.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	26-nov-93 (robf)
**		initial creation.
*/
VOID
duc_iisecalarm()
{
/* # line 6373 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iisecalarm(alarm_name=char(32), alarm_num=i4, obj_id1=i4, obj_\
id2=i4, obj_name=char(32), obj_type=char(1), subj_type=i1, subj_name=c\
har(32), alarm_flags=i2, alarm_txtid1=i4, alarm_txtid2=i4, alarm_opctl\
=i4, alarm_opset=i4, event_id1=i4, event_id2=");
    IIwritio(0,(short *)0,1,32,0,(char *)
"i4, event_text=char(256), alarm_id1=i4, alarm_id2=i4, alarm_reserve=c\
har(32))");
    IIsyncup((char *)0,0);
  }
/* # line 6392 "ducommon.qsc" */	/* host code */
}
/*
** Name; duc_m_iisecalarm -- Modify iisecalarm
**
** Description:
**	Modify iisecalarm
**
** History:
**	13-jul-94 (robf)
**         Modularized out of duc_iisecalarm to allow upgradedb to
**	   call seperately
*/
VOID
duc_m_iisecalarm()
{
	/*
	** Normal access is by object type, then object id.
	** Note multiple alarms are allowed per object id.
	*/
/* # line 6411 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iisecalarm to chash on obj_type, obj_id1, obj_id2 with minpage\
s=32");
    IIsyncup((char *)0,0);
  }
/* # line 6413 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iirolegrant() -- build catalog iirolegrant
**
** Description:
**        This routine makes the catalog iirolegrant.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	4-feb-94 (robf)
**		initial creation.
*/
VOID
duc_iirolegrant()
{
/* # line 6441 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iirolegrant(rgr_rolename=char(32), rgr_flags=i4, rgr_gtype=i2,\
 rgr_grantee=char(32), rgr_reserve=char(32))");
    IIsyncup((char *)0,0);
  }
/* # line 6449 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_m_iirolegrant() -- modify catalog iirolegrant
**
** Description:
**        This routine modifies the catalog iirolegrant.
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	15-jul-94 (robf)
**		initial creation, split from duc_iirolegrant
*/
VOID
duc_m_iirolegrant()
{
	/*
	** Normal access is by grantee name, but we key unique 
	** by grantee/rolename to ensure access is OK
	*/
/* # line 6481 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iirolegrant to btree unique on rgr_grantee, rgr_rolename");
    IIsyncup((char *)0,0);
  }
/* # line 6482 "ducommon.qsc" */	/* host code */
}
/*
** Name: duc_sqlerror
**
** Description:
**    Format and print an INGRES error. This differs from other handlers
**    like duc_sqlerr_handler() in that this routine simply extracts and
**    prints the last sqlerror, with no side effects like exiting the
**    program or requiring specific control blocks to be passed. This allows
**    it to be used in both createdb and upgradedb.
**
** History:
**	18-jul-94 (robf)
**          Added to help trace upgradedb errors
*/
static VOID
duc_sqlerror()
{
  char emesg[ER_MAX_LEN+1];
	/* Get the error text */
/* # line 6758 "ducommon.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,32,ER_MAX_LEN+1-1,emesg,63);
  }
/* # line 6759 "ducommon.sc" */	/* host code */
	/* and print */
	SIprintf("%s\n",emesg);
	SIflush(stdout);
}
/*{
** Name:  duc_iidd_tables() -- build catalog IIDD_TABLES
**
** Description:
**        Create catalog IIDD_TABLES
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**      06-mar-96 (nanpr01)
**	    initial creation.  Encapsulated from duc_1create_iidd_catalogs()
**	    so that 2.0 upgradedb can call it.
**	23-jul-96 (ramra01)
**	    Added relversion and reltotwid columns as part of alter table
**	    add/drop column support.
*/
VOID
duc_iidd_tables()
{
/* # line 6543 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_tables(table_name=char(32), table_owner=char(32), create_\
date=char(25), alter_date=char(25), table_type=char(8), table_subtype=\
char(8), table_version=char(8), system_use=char(8), table_stats=char(8\
), table_indexes=char(8), is_readonly=char(8)");
    IIwritio(0,(short *)0,1,32,0,(char *)
", num_rows=i4, storage_structure=char(16), is_compressed=char(8), dup\
licate_rows=char(8), unique_rule=char(8), number_pages=i4, overflow_pa\
ges=i4, row_width=i4, expire_date=i4, modify_date=char(25), location_n\
ame=char(32), table_integrities=char(8), ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"table_permits=char(8), all_to_all=char(8), ret_to_all=char(8), is_jou\
rnalled=char(8), view_base=char(8), multi_locations=char(8), table_ifi\
llpct=i2, table_dfillpct=i2, table_lfillpct=i2, table_minpages=i4, tab\
le_maxpages=i4, table_relstamp1=i4, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"table_relstamp2=i4, table_reltid=i4, table_reltidx=i4, table_pagesize\
=i4, table_relversion=i2, table_reltotwid=i4)");
    IIsyncup((char *)0,0);
  }
/* # line 6584 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iidd_indexes() -- build catalog IIDD_INDEXES
**
** Description:
**        Create catalog IIDD_INDEXES
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**      06-mar-96 (nanpr01)
**	    initial creation.  Encapsulated from duc_1create_iidd_catalogs()
**	    so that 2.0 upgradedb can call it.
*/
VOID
duc_iidd_indexes()
{
/* # line 6612 "ducommon.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidd_indexes(index_name=char(32), index_owner=char(32), create\
_date=char(25), base_name=char(32), base_owner=char(32), storage_struc\
ture=char(16), is_compressed=char(8), unique_rule=char(8), index_pages\
ize=i4)");
    IIsyncup((char *)0,0);
  }
/* # line 6621 "ducommon.qsc" */	/* host code */
}
/*
** Name:	duc_v_iidd_ddb_views	- create star iidd_ddb_views.
**
** Description:
**	This routine creates the star std catalog for iiviews. In most cases,
**	the star standard catalog is a table in the CDB that maps to the std 
**	catalog.  The mapping is done by joining iidd_ddb_objects.object_base
**	with iidd_ddb_tableinfo.object_base and iidd_ddb_objects.object_index
**	with iidd_ddb_object_index.  The name that the FEs use to see the
**	standard catalogs are in iidd_ddb_objects.object_name, and the
**	name that STAR maps the catalog to is in iidd_ddb_tableinfo.table_name.
**	In most cases, the mapping is just changing the "ii" prefix to "iidd_".
**	However, that is NOT the case with iiviews, which maps to iidd_ddb_views
**
**	example:
**		   iidd_ddb_objects:
**		  +------------+--------------+-------------------+
**		  |object_base | object_index | object_name       |
**		  +------------+--------------+-------------------+
**		  | 528	       | 0	      | iitables          |
**		  | 529        | 0	      | iiviews		  |
**		  | 540	       | 0            | iiobjects	  |
**		  | :	       | :            | :      :          |
**		  +------------+--------------+-------------------+
**
**		  iidd_ddb_tableinfo:
**		  +------------+--------------+-------------------+
**		  |object_base | object_index | table_name        |
**		  +------------+--------------+-------------------+
**		  | 528	       | 0	      | iidd_tables       |
**		  | 529        | 0	      | iidd_ddb_views    |
**		  | 540	       | 0            | iidd_objects	  |
**		  | :	       | :            | :      :          |
**		  +------------+--------------+-------------------+
**
**		  STAR would translate iitables to iidd_tables and
**		  STAR would translate iiviews to iidd_ddb_views and
**		  STAR would translate iiobjects to iidd_objects
**
**	The reason that the iiviews catalog maps differently is because the
**	iidd_views catalog contains query text for views that are created.
**	However, it does not contain text for registered views.  The FEs
**	do not know the difference between a created view and a registered
**	view, nor should they.  So, we create a union view between
**	iidd_views and iidd_registrations, and map the fe std catalog
**	iiviews to this union view.
**	
** Inputs:
**	none.
** Outputs:
**	none.
**
** Returns:
**	E_DB_OK
**
** History:
**	20-jun-90 (teresa)
**	   initial creation to solve bug 30502
*/
static VOID
duc_v_iidd_ddb_views()
{
DU_ERROR	*duc_errcb;
/* # line 6916 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iidd_ddb_views(table_name, table_owner, view_dml, check_o\
ption, text_sequence, text_segment)as select table_name, table_owner, \
view_dml, check_option, text_sequence, text_segment from iidd_views un\
ion all select object_name, object_owner, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"object_dml, ' ', text_sequence, text_segment from iidd_registrations \
where object_type='V'");
    IIsyncup((char *)0,0);
  }
/* # line 6927 "ducommon.sc" */	/* host code */
}
/*
** Name:	duc_v_iidd_dbconstants	- create star iidd_dbconstants.
**
** Description:
**	This routine creates the star std catalog iidd_dbconstants.
**      The definition of this catalog in standard catalog interface is 
**      define as user_name = char(32), dba_name = char(32) and
**	system_owner = char(32)
**	
** Inputs:
**	none.
** Outputs:
**	none.
**
** Returns:
**	E_DB_OK
**
** History:
**      09-nov-2001 (stial01)
**         moved here from ducreate
*/
static VOID
duc_v_iidd_dbconstants()
{
/* # line 6726 "ducommon.qsc" */	/* define view */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"define view iidd_dbconstants(user_name=dbmsinfo(\"username\"), dba_na\
me=dbmsinfo(\"dba\"), system_owner=\"$ingres                         \"\
)");
    IIsyncup((char *)0,0);
  }
/* # line 6730 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_iidd_physical_tables () -- build catalog IIDD_PHYSICAL_TABLES
**
** Description:
**        Create catalog IIDD_PHYSICAL_TABLES
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**      06-mar-96 (nanpr01)
**	    initial creation.  Encapsulated from duc_1create_iidd_catalogs()
**	    so that 2.0 upgradedb can call it.
**	23-oct-96 (hanch04)
**	    Added reltotwid and relversion.
**	11-nov-96 (nanpr01)
**	    fixup reltotwid and relversion.
*/
static VOID
duc_v_iidd_physical_tables()
{
/* # line 6762 "ducommon.qsc" */	/* define view */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"define view iidd_physical_tables(iidd_tables.table_name, iidd_tables.\
table_owner, iidd_tables.table_stats, iidd_tables.table_indexes, iidd_\
tables.is_readonly, iidd_tables.num_rows, iidd_tables.storage_structur\
e, iidd_tables.is_compressed, iidd_tables.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"duplicate_rows, iidd_tables.unique_rule, iidd_tables.number_pages, ii\
dd_tables.overflow_pages, iidd_tables.row_width, iidd_tables.table_pag\
esize, iidd_tables.table_relversion, iidd_tables.table_reltotwid)");
    IIsyncup((char *)0,0);
  }
/* # line 6771 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_v_iidd_registered_objects -- build catalog iidd_registered_objects
**
** Description:
**
**	This routine builds the STAR standard catalog iiregistered_objects
**	as a view in the CDB.  It assumes that we are connected to the CDB
**	via a local server.
**
**
** Inputs:
**	none
** Outputs:
**	none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	      None
**
** History:
**	16-oct-91 (teresa)
**		initial creation.
**	13-octo-93 (jpk)
**		get rid of consistency checks.  It's up to the caller
**		to call for a star db and not other sorts of dbs.
*/
static VOID
duc_v_iidd_registered_objects()
{
  i4 cnt;
#if	0
    /* consistency checks - assure that the underlaying catalogs exist.
    ** if not, return silently 
    */
/* # line 7046 "ducommon.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(relid)from iirelation where relid='iidd_ddb_objects'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 7048 "ducommon.sc" */	/* host code */
    if (cnt == 0 ) 
    {
	d_print("duc_v_iidd_registered_objects: returning from pt 1\n");
	return;	/* this is not a distribted database, or this catalog would
		** exist */
    }
/* # line 7054 "ducommon.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(relid)from iirelation where relid='iidd_ddb_ldbids'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 7056 "ducommon.sc" */	/* host code */
    if (cnt == 0 ) 
    {
	d_print("duc_v_iidd_registered_objects: returning from pt 2\n");
	return;	/* this is not a distribted database, or this catalog would
		** exist */
    }
/* # line 7062 "ducommon.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(relid)from iirelation where relid='iidd_ddb_tableinfo'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 7064 "ducommon.sc" */	/* host code */
    if (cnt == 0 ) 
    {
	d_print("duc_v_iidd_registered_objects: returning from pt 3\n");
	return;	/* this is not a distribted database, or this catalog would
		** exist */
    }
#endif
    /* now create the iidd_registered_objects view */
/* # line 7073 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iidd_registered_objects(ddb_object_name, ddb_object_owner\
, register_date, ldb_database, ldb_node, ldb_dbmstype, ldb_object_name\
, ldb_object_owner, ldb_object_type)as select object_name, object_owne\
r, create_date, ldb_database, ldb_node, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"ldb_dbms, table_name, table_owner, local_type from iidd_ddb_objects, \
iidd_ddb_ldbids, iidd_ddb_tableinfo where iidd_ddb_objects.object_base\
=iidd_ddb_tableinfo.object_base and iidd_ddb_objects.object_index=iidd\
_ddb_tableinfo.object_index and ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"iidd_ddb_tableinfo.ldb_id=iidd_ddb_ldbids.ldb_id and(system_object='N\
' or system_object=' ')and object_type='L'");
    IIsyncup((char *)0,0);
  }
/* # line 7099 "ducommon.sc" */	/* host code */
}
/*{
** Name:  duc_modify_iidd_tables() -- modify catalog IIDD_TABLES
**
** Description:
**        Modify catalog IIDD_TABLES
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**      06-mar-96 (nanpr01)
**	    initial creation.  Encapsulated from duc_1create_iidd_catalogs()
**	    so that 2.0 upgradedb can call it.
*/
VOID
duc_modify_iidd_tables()
{
/* # line 6895 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_tables to hash on table_name, table_owner");
    IIsyncup((char *)0,0);
  }
/* # line 6896 "ducommon.qsc" */	/* host code */
}
/*{
** Name:  duc_modify_iidd_indexes() -- modify catalog IIDD_INDEXES
**
** Description:
**        Modify catalog IIDD_INDEXES
**
** Inputs:
**	none
** Outputs:
**	none
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** History:
**      06-mar-96 (nanpr01)
**	    initial creation.  Encapsulated from duc_1create_iidd_catalogs()
**	    so that 2.0 upgradedb can call it.
*/
VOID
duc_modify_iidd_indexes()
{
/* # line 6924 "ducommon.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iidd_indexes to hash on index_name, index_owner");
    IIsyncup((char *)0,0);
  }
/* # line 6925 "ducommon.qsc" */	/* host code */
}
STATUS
duc_drop_stdcat_views(
char            *dbname,
bool		star_flag)
{
    bool	is_iidbdb;
    i4		i;
    STATUS	status;
  char sbuffer[256];
    if (!STcompare(dbname, DU_DBDBNAME))
	is_iidbdb = TRUE;
    else 
	is_iidbdb = FALSE;
    /*
    ** Drop in reverse order of create,
    ** but this is not strictly necessary since standard catalogs
    ** are not view on views
    */
    for (i = MAX_STDCAT_VIEWS - 1; i >= 0; i--)
    {
	if (Du_stdcat[i].du_dbflag == DU_IIDBDB && !is_iidbdb)
	    continue;
	if (Du_stdcat[i].du_dbflag == DU_STAR && !star_flag)
	    continue;
	if (!rel_exists(Du_stdcat[i].du_objname))
	    continue;
	STprintf(sbuffer, "drop view %s\n", Du_stdcat[i].du_objname);
/* # line 7197 "ducommon.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
  }
/* # line 7198 "ducommon.sc" */	/* host code */
	/* Ignore drop errors */
    }
/* # line 7200 "ducommon.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7201 "ducommon.sc" */	/* host code */
}
/*
** Name:  duc_grant_stdcat_views - grant permits to recreated views
**
** Description:
**        This is similar to duc_create_stdcat_view, except it
**        grants lost permits for the recreated views.
**
** Inputs:
**      none
** Outputs:
**      none
** Returns:
**      none
**
** Side Effects:
**      none
**
** History:
**      14-nov-03 (wanfr01)
**          initial creation.
*/
STATUS
duc_grant_stdcat_views(
char            *dbname,
bool            star_flag)
{
    i4          i;
    STATUS      status;
  char sbuffer[256];
    for (i = MAX_STDCAT_VIEWS - 1; i >= 0; i--)
    {
        if (!rel_exists(Du_stdcat[i].du_objname))
            continue;
        STprintf(sbuffer, "grant select on  %s to public with grant option\n", Du_stdcat[i].du_objname);
/* # line 7238 "ducommon.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
  }
/* # line 7239 "ducommon.sc" */	/* host code */
        if (sqlca.sqlcode < 0)
        {
            /* XXX this should be msg-ized! */
            /* This routine will blindly attempt to grant select to quel
               permits, so ignore those errors */
            if (sqlca.sqlerrd[0] != 3598)
                SIprintf("]==> <%s> returns %d\n", sbuffer, sqlca.sqlcode);
        }
    }
/* # line 7248 "ducommon.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7249 "ducommon.sc" */	/* host code */
}
STATUS
duc_upgrade_stdcat_views(
char            *dbname,
bool		upcase,
bool		star_flag)
{
    bool	is_iidbdb;
    i4		i;
    STATUS	status;
    if (!STcompare(dbname, DU_DBDBNAME))
	is_iidbdb = TRUE;
    else 
	is_iidbdb = FALSE;
    for (i = 0; i < MAX_STDCAT_VIEWS; i++)
    {
	if (Du_stdcat[i].du_dbflag == DU_IIDBDB && !is_iidbdb)
	    continue;
	if (Du_stdcat[i].du_dbflag == DU_STAR && !star_flag)
	    continue;
	if (!view_exists(Du_stdcat[i].du_objname))
	    status = duc_create_one_stdcat_view(dbname, &Du_stdcat[i], upcase);
    }
/* # line 7272 "ducommon.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7273 "ducommon.sc" */	/* host code */
}
STATUS
duc_create_stdcat_views(
char            *dbname,
bool		upcase)
{
    bool	is_iidbdb;
    i4		i;
    STATUS	status;
    if (!STcompare(dbname, DU_DBDBNAME))
	is_iidbdb = TRUE;
    else 
	is_iidbdb = FALSE;
    for (i = 0; i < MAX_STDCAT_VIEWS; i++)
    {
	if (Du_stdcat[i].du_dbflag == DU_IIDBDB && !is_iidbdb)
	    continue;
	/* Star views get created AFTER star catalogs */
	if (Du_stdcat[i].du_dbflag == DU_STAR)
	    continue;
	status = duc_create_one_stdcat_view(dbname, &Du_stdcat[i], upcase);
    }
/* # line 7295 "ducommon.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7296 "ducommon.sc" */	/* host code */
}
DU_STATUS
duc_create_specific_stdcat_view(char *dbname, char *viewname, bool upcase)
{
    i4 i;
    STATUS status;
  char sql_view[DB_MAXNAME+1];
    STcopy(viewname, sql_view);
    for (i = 0; i < MAX_STDCAT_VIEWS; ++i)
    {
	if (STcompare(viewname, Du_stdcat[i].du_objname) == 0)
	{
	    status = duc_create_one_stdcat_view(dbname, &Du_stdcat[i], upcase);
	    if (status == OK)
	    {
/* # line 7313 "ducommon.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on : sql_view to public with grant option");
    IIsyncup((char *)0,0);
  }
/* # line 7314 "ducommon.sc" */	/* host code */
		return (E_DU_OK);
	    }
	    return (E_DU_IERROR);
	}
    }
    return (E_DU_IERROR);
}
STATUS
duc_create_star_views(
char            *dbname,
bool		upcase)
{
    i4		i;
    STATUS	status;
    for (i = 0; i < MAX_STDCAT_VIEWS; i++)
    {
	if (Du_stdcat[i].du_dbflag == DU_STAR)
	    status = duc_create_one_stdcat_view(dbname, &Du_stdcat[i], upcase);
	else
	    continue;
    }
/* # line 7335 "ducommon.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7336 "ducommon.sc" */	/* host code */
}
static STATUS
duc_create_one_stdcat_view(
char		*dbname,
DU_OBJ		*view,
bool		upcase)
{
    bool	is_iidbdb;
    if (!STcompare(dbname, DU_DBDBNAME))
	is_iidbdb = TRUE;
    else 
	is_iidbdb = FALSE;
    switch (view->du_obj_id)
    {
	case DU_IIPROC_ACCESS:
	{
/* # line 7352 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iiproc_access as select object_name=p.probjname, object_o\
wner=p.probjowner, permit_grantor=p.prograntor, object_type=p.probjtyp\
e, create_date=gmt_timestamp(p.procrtime1), permit_user=p.prouser, per\
mit_depth=p.prodepth, permit_number=int2(p.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"propermid), text_sequence=int2(q.seq) +1, text_segment=q.txt from iiq\
rytext q, iiprotect p where p.proqryid1=q.txtid1 and p.proqryid2=q.txt\
id2 and q.mode=19 and mod((p.proopset/128), (2))=0 and p.probjtype='P'");
    IIsyncup((char *)0,0);
  }
/* # line 7366 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIPROC_PARAMS:
	{
/* # line 7370 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iiproc_params as select procedure_name=p1.dbp_name, proce\
dure_owner=p1.dbp_owner, param_name=p2.pp_name, param_sequence=p2.pp_n\
umber, param_datatype=uppercase(iitypename(ii_ext_type(p2.pp_datatype,\
 p2.pp_length))), param_datatype_code=p2.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"pp_datatype, param_length=iiuserlen(ii_ext_type(p2.pp_datatype, p2.pp\
_length), (ii_ext_length(p2.pp_datatype, p2.pp_length)*65536) +p2.pp_p\
recision), param_scale=int4(mod(p2.pp_precision, (256))), param_nulls=\
byteextract('YN', (mod(p2.pp_flags, 2)) +1), ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"param_defaults=byteextract('YN', (mod((p2.pp_flags/2), (2))) +1), par\
am_default_val=squeeze(d.defvalue)from iiprocedure p1, iiprocedure_par\
ameter p2, iidefault d where(p2.pp_procid1=p1.dbp_id and p2.pp_procid2\
=p1.dbp_idx)and(p2.pp_defid1=d.defid1 and p2.");
    IIwritio(0,(short *)0,1,32,0,(char *)"pp_defid2=d.defid2)");
    IIsyncup((char *)0,0);
  }
/* # line 7389 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIAUDITTABLES:
	{
/* # line 7393 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iiaudittables as select table_name=r.relid, table_owner=r\
.relowner, g.audit_log, register_date=gmt_timestamp(g.reg_date)from ii\
relation r, iigw06_relation g where r.reltid=g.reltid and r.reltidx=g.\
reltidx and session_priv('auditor')='Y'");
    IIsyncup((char *)0,0);
  }
/* # line 7402 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IISESSION_PRIVILEGES:
	{
/* # line 7406 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iisession_privileges(priv_name, priv_access)as select pri\
vname, session_priv(privname)from iiprivlist where session_priv(privna\
me)!='N' or dbmsinfo('security_priv')='Y'");
    IIsyncup((char *)0,0);
  }
/* # line 7412 "ducommon.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on iisession_privileges to public");
    IIsyncup((char *)0,0);
  }
/* # line 7413 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_STAR_CDBINFO:
	{
/* # line 7417 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iistar_cdbinfo(ddb_name, ddb_owner, cdb_name, cdb_owner, \
cdb_node, cdb_dbms, cdb_create_date)as select ddb_name, ddb_owner, cdb\
_name, cdb_owner, cdb_node, cdb_dbms, create_date from iistar_cdbs");
    IIsyncup((char *)0,0);
  }
/* # line 7424 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIDATABASE_INFO:
	{
/* # line 7428 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iidatabase_info(database_name, database_owner, data_locat\
ion, work_location, ckp_location, jnl_location, dump_location, compat_\
level, compat_level_minor, database_service, security_label, access, d\
atabase_id)as select name, own, dbdev, sortdev");
    IIwritio(0,(short *)0,1,32,0,(char *)
", ckpdev, jnldev, dmpdev, dbcmptlvl, dbcmptminor, dbservice, char('  \
      '), access, db_id from iidatabase");
    IIsyncup((char *)0,0);
  }
/* # line 7437 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIUSERS:
	{
/* # line 7441 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iiusers(user_name, createdb, trace, audit_all, security, \
maintain_locations, operator, maintain_users, maintain_audit, auditor,\
 audit_query_text, expire_date, profile_name, lim_sec_label, default_g\
roup, internal_status, internal_def_priv, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"internal_flags)as select u.name, byteextract('NRY', mod((u.status), 2\
) +mod((u.default_priv), 2) +1), byteextract('NRY', mod((u.status/16),\
 2) +mod((u.default_priv/16), 2) +1), byteextract('NY', mod((u.status/\
1024), 2) +1), byteextract('NRY', mod((u.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"status/32768), 2) +mod((u.default_priv/32768), 2) +1), byteextract('N\
RY', mod((u.status/2048), 2) +mod((u.default_priv/2048), 2) +1), bytee\
xtract('NRY', mod((u.status/512), 2) +mod((u.default_priv/512), 2) +1)\
, byteextract('NRY', mod((u.status/65536), 2)");
    IIwritio(0,(short *)0,1,32,0,(char *)
" +mod((u.default_priv/65536), 2) +1), byteextract('NRY', mod((u.statu\
s/16384), 2) +mod((u.default_priv/16384), 2) +1), byteextract('NRY', m\
od((u.status/8192), 2) +mod((u.default_priv/8192), 2) +1), byteextract\
('NY', mod((u.status/16777216), 2) +1), u.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"expire_date, u.profile_name, char('        '), u.default_group, u.sta\
tus, u.default_priv, u.flags_mask from iiuser u");
    IIsyncup((char *)0,0);
  }
/* # line 7488 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IILOCATION_INFO:
	{
/* # line 7492 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iilocation_info(location_name, data_usage, jrnl_usage, ck\
pt_usage, work_usage, dump_usage, awork_usage, location_area, security\
_label, raw_pct, status)as select l.lname, byteextract('NY', mod((l.st\
atus/8), (2)) +1), byteextract('NY', mod((l.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"status/64), (2)) +1), byteextract('NY', mod((l.status/512), (2)) +1),\
 byteextract('NY', mod((l.status/16), (2)) +1), byteextract('NY', mod(\
(l.status/2), (2)) +1), byteextract('NY', mod((l.status/32), (2)) +1),\
 l.area, char('        '), l.rawpct, status ");
    IIwritio(0,(short *)0,1,32,0,(char *)"from iilocations l");
    IIsyncup((char *)0,0);
  }
/* # line 7517 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIEXTEND_INFO:
	{
	    /*
	    ** View extensions to databases/locations which are visible to
	    ** the user. Note this assumes iidatabase_info and iilocation_info
	    ** are configured appropriately to display locations/databases to
	    ** the user.
	    */
/* # line 7527 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iiextend_info(location_name, database_name, status, raw_s\
tart, raw_blocks)as select e.lname, e.dname, e.status, e.rawstart, e.r\
awblocks from iiextend e, iidatabase_info d, iilocation_info l where e\
.lname=l.location_name and e.dname=d.");
    IIwritio(0,(short *)0,1,32,0,(char *)"database_name");
    IIsyncup((char *)0,0);
  }
/* # line 7532 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIDBPRIVILEGES:
	{
/* # line 7334 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of d=iidbpriv");
    IIsyncup((char *)0,0);
  }
/* # line 7335 "ducommon.qsc" */	/* define view */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"define view iidbprivileges(database_name=d.dbname, grantee_name=d.gra\
ntee, gr_type=byteextract(\" UGRP\", (d.gtype+2)), cr_tab=byteextract(\
\"UNY\", (mod((d.control/256), (2))+mod((d.flags/256), (2))+1)), cr_pr\
oc=byteextract(\"UNY\", (mod((d.control/512), ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"(2))+mod((d.flags/512), (2))+1)), lk_mode=byteextract(\"UNY\", (mod((\
d.control/1024), (2))+mod((d.flags/1024), (2))+1)), db_access=byteextr\
act(\"UNY\", (mod((d.control/2048), (2))+mod((d.flags/2048), (2))+1)),\
 up_syscat=byteextract(\"UNY\", (mod((d.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"control/4096), (2))+mod((d.flags/4096), (2))+1)), db_admin=byteextrac\
t(\"UNY\", (mod((d.control/8192), (2))+mod((d.flags/8192), (2))+1)), g\
lobal_usage=byteextract(\"UNY\", (mod((d.control/16384), (2))+mod((d.f\
lags/16384), (2))+1)), qry_io_lim=d.qdio_limit");
    IIwritio(0,(short *)0,1,32,0,(char *)
", qry_io=byteextract(\"UNY\", (mod((d.control/2), (2))+mod((d.flags/2\
), (2))+1)), qry_row_lim=d.qrow_limit, qry_row=byteextract(\"UNY\", (m\
od((d.control), (2))+mod((d.flags), (2))+1)), sel_syscat=byteextract(\"\
UNY\", (mod((d.control/32768), (2))+mod((d.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"flags/32768), (2))+1)), tbl_stats=byteextract(\"UNY\", (mod((d.contro\
l/65536), (2))+mod((d.flags/65536), (2))+1)), idle_time=byteextract(\"\
UNY\", (mod((d.control/131072), (2))+mod((d.flags/131072), (2))+1)), i\
dle_time_lim=d.idle_time_limit, conn_time=");
    IIwritio(0,(short *)0,1,32,0,(char *)
"byteextract(\"UNY\", (mod((d.control/262144), (2))+mod((d.flags/26214\
4), (2))+1)), conn_time_lim=d.connect_time_limit, sess_prio=byteextrac\
t(\"UNY\", (mod((d.control/524288), (2))+mod((d.flags/524288), (2))+1)\
), sess_prio_lim=d.priority_limit)");
    IIsyncup((char *)0,0);
  }
/* # line 7388 "ducommon.qsc" */	/* host code */
	    break;
	}
	case DU_IIPROFILES:
	{
/* # line 7580 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iiprofiles(profile_name, createdb, trace, audit_all, secu\
rity, maintain_locations, operator, maintain_users, maintain_audit, au\
ditor, audit_query_text, expire_date, lim_sec_label, default_group, in\
ternal_status)as select p.name, byteextract(");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'NRY', mod((p.status), 2) +mod((p.default_priv), 2) +1), byteextract(\
'NRY', mod((p.status/16), 2) +mod((p.default_priv/16), 2) +1), byteext\
ract('NY', mod((p.status/1024), 2) +1), byteextract('NRY', mod((p.stat\
us/32768), 2) +mod((p.default_priv/32768), 2)");
    IIwritio(0,(short *)0,1,32,0,(char *)
" +1), byteextract('NRY', mod((p.status/2048), 2) +mod((p.default_priv\
/2048), 2) +1), byteextract('NRY', mod((p.status/512), 2) +mod((p.defa\
ult_priv/512), 2) +1), byteextract('NRY', mod((p.status/65536), 2) +mo\
d((p.default_priv/65536), 2) +1), byteextract(");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'NRY', mod((p.status/16384), 2) +mod((p.default_priv/16384), 2) +1), \
byteextract('NRY', mod((p.status/8192), 2) +mod((p.default_priv/8192),\
 2) +1), byteextract('NY', mod((p.status/16777216), 2) +1), p.expire_d\
ate, char('        '), p.default_group, p.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"status from iiprofile p where p.name!=''");
    IIsyncup((char *)0,0);
  }
/* # line 7622 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIROLES:
	{
/* # line 7626 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iiroles(role_name, createdb, trace, audit_all, security, \
maintain_locations, operator, maintain_users, maintain_audit, auditor,\
 audit_query_text, security_label, lim_sec_label, internal_status, int\
ernal_flags)as select r.roleid, byteextract(");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'NY', mod((r.rolestatus), 2) +1), byteextract('NY', mod((r.rolestatus\
/16), 2) +1), byteextract('NY', mod((r.rolestatus/1024), 2) +1), bytee\
xtract('NY', mod((r.rolestatus/32768), 2) +1), byteextract('NY', mod((\
r.rolestatus/2048), 2) +1), byteextract('NY', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"mod((r.rolestatus/512), 2) +1), byteextract('NY', mod((r.rolestatus/6\
5536), 2) +1), byteextract('NY', mod((r.rolestatus/16384), 2) +1), byt\
eextract('NY', mod((r.rolestatus/8192), 2) +1), byteextract('NY', mod(\
(r.rolestatus/16777216), 2) +1), char(");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'        '), char('        '), r.rolestatus, r.roleflags from iirole \
r");
    IIsyncup((char *)0,0);
  }
/* # line 7658 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIROLEGRANTS:
	{
/* # line 7662 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iirolegrants(role_name, gr_type, grantee_name, admin_opti\
on)as select rgr_rolename, byteextract(' UGRP', (rgr_gtype +2)), rgr_g\
rantee, byteextract('NY', rgr_flags +1)from iirolegrant rg, iirole r w\
here rg.rgr_rolename=r.roleid");
    IIsyncup((char *)0,0);
  }
/* # line 7671 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IISECURITY_STATE:
	{
/* # line 7675 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iisecurity_state(type, name, state, number)as select ifnu\
ll(t.sec_type, 'Unknown'), ifnull(t.sec_name, 'Unknown'), byteextract(\
'DE', s.state +1), s.id from iisecuritystate s left join iisectype t o\
n s.type=t.sec_typenum and s.id=t.sec_namenum ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"where session_priv('maintain_audit')='Y' and s.id!=10");
    IIsyncup((char *)0,0);
  }
/* # line 7690 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIDBCONSTANTS:
	{
  char owner[DB_MAXNAME+1];
	    if (upcase)
		MEmove(sizeof(ERx("$ingres")) - 1, (PTR) ERx("$INGRES"), ' ',
		       sizeof(owner), owner);
	    else
		MEmove(sizeof(ERx("$ingres")) - 1, (PTR) ERx("$ingres"), ' ',
		       sizeof(owner), owner);
	    owner[sizeof(owner)-1] = EOS;
/* # line 7525 "ducommon.qsc" */	/* define view */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"define view iidbconstants(user_name=username, dba_name=dba, system_ow\
ner=");
    IIputdomio((short *)0,1,32,0,owner);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 7528 "ducommon.qsc" */	/* host code */
	    break;
	}
	case DU_IITABLES:
	{
	    /*
	    ** Need to calculate table_subtype here from r.relstat.  
	    ** Valid subtypes are:
	    ** "N" (native), "I" (import (gateway)), "L" (link (star)).  
	    ** At the moment we account for "N" and "I" but not for "L"...
	    **
	    ** There are only 4 characters in the byteextract definition of
	    ** system_use, because the result of the mod's in system_use should 
	    ** never be more than 4 (even though the mathematical limit is 6). 
	    ** 4 is the upper limit because the relstat 0x01 and 0x4000 bits 
	    ** should never be set at the same time as the relstat2 0x10 bit.
	    **          
	    */
/* # line 7729 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iitables as select relid table_name, relowner table_owner\
, gmt_timestamp(relcreate)create_date, gmt_timestamp(relcreate)alter_d\
ate, byteextract('TVIP', (mod((relstat/32), 2) +(2*mod((relstat/128), \
2)) +(3*mod((relstat2/32768), 2))) +1)");
    IIwritio(0,(short *)0,1,32,0,(char *)
"table_type, byteextract('NI', mod((relstat/8388608), (2)) +1)table_su\
btype, char('II9.0')table_version, byteextract('USSG', (mod(relstat, (\
2))) +(mod((relstat/16384), (2))) +(3*mod((relstat2/16), (2))) +1)syst\
em_use, byteextract('NY', mod((relstat/1048576");
    IIwritio(0,(short *)0,1,32,0,(char *)
"), (2)) +1)table_stats, byteextract('NY', mod((relstat/131072), (2)) \
+1)table_indexes, byteextract('NY', mod((relstat2/128), (2)) +1)is_rea\
donly, byteextract('NY', mod((relstat2/256), (2)) +1)concurrent_access\
, iitblstat(int4(iitableinfo('num_rows', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"reltid, reltidx)))num_rows, uppercase(iistructure(relspec))storage_st\
ructure, byteextract('NYH', (mod((relstat/512), (2)) +(int1(relcomptyp\
e/7))) +1)is_compressed, byteextract('NY', mod((relstat/1024), (2)) +1\
)key_is_compressed, byteextract('UD', mod((");
    IIwritio(0,(short *)0,1,32,0,(char *)
"relstat/2097152), (2)) +1)duplicate_rows, byteextract('DU', mod((rels\
tat/65536), (2)) +1)unique_rule, iitblstat(int4(iitableinfo('num_pages\
', reltid, reltidx)))number_pages, iitblstat(iitotal_overflow_pages(re\
ltid, reltidx))overflow_pages, int4(relwid)");
    IIwritio(0,(short *)0,1,32,0,(char *)
"row_width, relsave expire_date, gmt_timestamp(relmoddate)modify_date,\
 relloc location_name, byteextract('NY', mod((relstat/8), (2)) +1)tabl\
e_integrities, byteextract('NY', mod((relstat/4), (2)) +1)table_permit\
s, byteextract('YN', mod((relstat/4096), (2))");
    IIwritio(0,(short *)0,1,32,0,(char *)
" +1)all_to_all, byteextract('YN', mod((relstat/8192), (2)) +1)ret_to_\
all, byteextract('NYC', (2*mod((relstat/32768), (2))) +mod((relstat/26\
2144), (2)) +1)is_journalled, byteextract('NY', mod((relstat/64), (2))\
 +1)view_base, byteextract('NY', mod((relstat/");
    IIwritio(0,(short *)0,1,32,0,(char *)
"4194304), (2)) +1)multi_locations, relifill table_ifillpct, reldfill \
table_dfillpct, rellfill table_lfillpct, relmin table_minpages, relmax\
 table_maxpages, relstamp1 table_relstamp1, relstamp2 table_relstamp2,\
 reltid table_reltid, reltidx table_reltidx, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"byteextract(' RS', (mod((relstat/65536), (2)) +mod((relstat2/64), (2)\
)) +1)unique_scope, relallocation allocation_size, relextend extend_si\
ze, iitblstat(iitotal_allocated_pages(reltid, reltidx))allocated_pages\
, char(' ')label_granularity, byteextract('NY'");
    IIwritio(0,(short *)0,1,32,0,(char *)
", mod(relstat2/8388608, 2) +1)row_security_audit, char('        ')sec\
urity_label, relpgsize table_pagesize, relversion table_relversion, in\
t4(reltotwid)table_reltotwid, reltcpri table_reltcpri, iitblstat(int4(\
iitableinfo('tups_per_page', reltid, reltidx))");
    IIwritio(0,(short *)0,1,32,0,(char *)
")tups_per_page, iitblstat(int4(iitableinfo('keys_per_page', reltid, r\
eltidx)))keys_per_page, iitblstat(int4(iitableinfo('keys_per_leaf', re\
ltid, reltidx)))keys_per_leaf, relnparts phys_partitions, relnpartleve\
ls partition_dimensions from iirelation");
    IIsyncup((char *)0,0);
  }
/* # line 7793 "ducommon.sc" */	/* host code */
	    if(sqlca.sqlcode<0)
	    {
		duc_sqlerror();
	    }
	    break;
	}
	case DU_IICOLUMNS:
	{
	    /*
	    ** NOTE: the COLUMN_HAS_DEFAULT column maps the attdefid1 field onto
	    ** a 4-valued column.  It maps 0 to 1, 1 to 3, 2 to 4, and all other
	    ** (positive) numbers to 5.  These numbers are then mapped to chars
	    ** using byteextract():  1 to 'N', 3 to 'U', 4 to ' ', and 5 to 'Y'.
	    ** Attdefid1 values > 2^31 become negative numbers in SQL, 
	    ** and most are mapped to 6 or 7, so extra 'Y' characters have
	    ** been put into the column definition.
	    ** But -2 and -1 map to larger numbers and so are not handled
	    ** correctly.
	    **	
	    ** The same numeric mapping is used for the COLUMN_DEFAULTS column,
	    ** only with a different alphabetic mapping on the numbers.
	    ** The COLUMN_DEFAULTS column also has an additional element to take
	    ** into account whether the column is nullable or not; this is for 
	    ** backward compatibility with calculated columns in views, which 
	    ** show N for nullable columns and Y for not-nullable columns. 
	    ** Thus the final mapping from attdefid1 to COLUMN_DEFAULTS is:  
	    ** 0 to 'N', 1 to 'N', 2 to 'N' if nullable or 'Y' if not nullable,
	    ** and all other values to 'Y'.
	    ** Note that the only default-id value that changes due to
	    ** nullability is 2 (which is DB_DEF_ID_UNKNOWN and is only used
	    ** for calculated columns [at least in non-gateway catalogs]). 
	    **
	    ** 10/05/92   rickh,rblumer
	    **      
	    ** In the future, we will need to have a KNOWN_NOT_NULLABLE field in
	    ** the COLUMNS table of the SQL92 INFO_SCHEMA catalog.  It will be
	    ** denoted by the 2nd bit in the attflag column.  We have decided 
	    ** not to put this into the INGRES IICOLUMNS view at this time,
	    ** but may decide to add it in the future.
	    */
/* # line 7833 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iicolumns as select table_name=r.relid, table_owner=r.rel\
owner, column_name=a.attname, column_datatype=uppercase(iitypename(ii_\
ext_type(a.attfrmt, a.attfrml))), column_length=iiuserlen(ii_ext_type(\
a.attfrmt, a.attfrml), (ii_ext_length(a.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"attfrmt, a.attfrml)*65536) +a.attfrmp), column_scale=int4(mod(a.attfr\
mp, (256))), column_collid=a.attcollid, column_nulls=byteextract('NY',\
 abs(((a.attfrmt/abs(a.attfrmt)) -1)/2) +1), column_defaults=byteextra\
ct('NNNNYYYY', float8(d.defid1)*5/(d.defid1 +(");
    IIwritio(0,(short *)0,1,32,0,(char *)
"float8(7)/float8(10))) +1 +(((a.attfrmt/abs(a.attfrmt)) +1)/2)), colu\
mn_sequence=int4(a.attid), key_sequence=int4(a.attkdom), sort_directio\
n=byteextract('AD', mod((a.attflag/8), (2)) +1), column_ingdatatype=ii\
_ext_type(a.attfrmt, a.attfrml), ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"column_internal_datatype=uppercase(iitypename(a.attfrmt)), column_int\
ernal_length=iiuserlen(a.attfrmt, (a.attfrml*65536) +a.attfrmp), colum\
n_internal_ingtype=a.attfrmt, column_system_maintained=byteextract('NY\
', mod((a.attflag/64), (2)) +1), ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"column_updateable=byteextract('YNNN  ', ((2*mod((r.relstat/128), (2))\
) +(4*mod((r.relstat/32), (2))) +mod((a.attflag/64), (2))) +1), column\
_has_default=byteextract('N U YYY', float8(d.defid1)*5/(d.defid1 +(flo\
at8(7)/float8(10))) +1), column_default_val=d.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"defvalue, security_audit_key=byteextract('NY', mod(a.attflag/8192, 2)\
 +1)from iirelation r, iiattribute a, iidefault d where a.attrelid=r.r\
eltid and a.attrelidx=r.reltidx and a.attdefid1=d.defid1 and a.attdefi\
d2=d.defid2 and a.attver_dropped=0 and mod(a.");
    IIwritio(0,(short *)0,1,32,0,(char *)"attflag/128, 2)=0");
    IIsyncup((char *)0,0);
  }
/* # line 7876 "ducommon.sc" */	/* host code */
	    if(sqlca.sqlcode<0)
	    {
		duc_sqlerror();
	    }
	    break;
	}
	case DU_IIPHYSICAL_COLUMNS:
	{
	    /*
	    ** iiphysical columns is identical to iicolumns 
	    ** (and MUST be kept in sync)
	    ** except that it shows both visible and hidden columns.
	    */
/* # line 7889 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iiphysical_columns as select table_name=r.relid, table_ow\
ner=r.relowner, column_name=a.attname, column_datatype=uppercase(iityp\
ename(ii_ext_type(a.attfrmt, a.attfrml))), column_length=iiuserlen(ii_\
ext_type(a.attfrmt, a.attfrml), (ii_ext_length");
    IIwritio(0,(short *)0,1,32,0,(char *)
"(a.attfrmt, a.attfrml)*65536) +a.attfrmp), column_scale=int4(mod(a.at\
tfrmp, (256))), column_collid=a.attcollid, column_nulls=byteextract('N\
Y', abs(((a.attfrmt/abs(a.attfrmt)) -1)/2) +1), column_defaults=byteex\
tract('NNNNYYYY', float8(d.defid1)*5/(d.defid1");
    IIwritio(0,(short *)0,1,32,0,(char *)
" +(float8(7)/float8(10))) +1 +(((a.attfrmt/abs(a.attfrmt)) +1)/2)), c\
olumn_sequence=int4(a.attid), key_sequence=int4(a.attkdom), sort_direc\
tion=byteextract('AD', mod((a.attflag/8), (2)) +1), column_ingdatatype\
=ii_ext_type(a.attfrmt, a.attfrml), ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"column_internal_datatype=uppercase(iitypename(a.attfrmt)), column_int\
ernal_length=iiuserlen(a.attfrmt, (a.attfrml*65536) +a.attfrmp), colum\
n_internal_ingtype=a.attfrmt, column_system_maintained=byteextract('NY\
', mod((a.attflag/64), (2)) +1), ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"column_updateable=byteextract('YNNN  ', ((2*mod((r.relstat/128), (2))\
) +(4*mod((r.relstat/32), (2))) +mod((a.attflag/64), (2))) +1), column\
_has_default=byteextract('N U YYY', float8(d.defid1)*5/(d.defid1 +(flo\
at8(7)/float8(10))) +1), column_default_val=d.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"defvalue, security_audit_key=byteextract('NY', mod(a.attflag/8192, 2)\
 +1)from iirelation r, iiattribute a, iidefault d where a.attrelid=r.r\
eltid and a.attrelidx=r.reltidx and a.attdefid1=d.defid1 and a.attdefi\
d2=d.defid2");
    IIsyncup((char *)0,0);
  }
/* # line 7929 "ducommon.sc" */	/* host code */
	    if(sqlca.sqlcode<0)
	    {
		duc_sqlerror();
	    }
	    break;
	}
	case DU_IIPHYSICAL_TABLES:
	{
/* # line 7764 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of r=iirelation");
    IIsyncup((char *)0,0);
  }
/* # line 7765 "ducommon.qsc" */	/* define view */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"define view iiphysical_tables(table_name=r.relid, table_owner=r.relow\
ner, table_stats=byteextract(\"NY\", mod((r.relstat/1048576), (2))+1),\
 table_indexes=byteextract(\"NY\", mod((r.relstat/131072), (2))+1), is\
_readonly=byteextract(\"NY\", mod((r.relstat2/");
    IIwritio(0,(short *)0,1,32,0,(char *)
"128), (2))+1), concurrent_access=byteextract(\"NY\", mod((r.relstat2/\
256), (2))+1), num_rows=int4(r.reltups), storage_structure=uppercase(i\
istructure(r.relspec)), is_compressed=byteextract(\"NY\", (mod((r.rels\
tat/512), (2)))+1), key_is_compressed=");
    IIwritio(0,(short *)0,1,32,0,(char *)
"byteextract(\"NY\", mod((r.relstat/1024), (2))+1), duplicate_rows=byt\
eextract(\"UD\", mod((r.relstat/2097152), (2))+1), unique_rule=byteext\
ract(\"DU\", mod((r.relstat/65536), (2))+1), number_pages=int4(r.relpa\
ges), overflow_pages=iitotal_overflow_pages(r.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"reltid, r.reltidx), row_width=int4(r.relwid), allocation_size=r.relal\
location, extend_size=r.relextend, allocated_pages=iitotal_allocated_p\
ages(r.reltid, r.reltidx), label_granularity=char(\" \"), row_security\
_audit=byteextract(\"NY\", mod(r.relstat2/");
    IIwritio(0,(short *)0,1,32,0,(char *)
"8388608, 2)+1), security_label=char(\"        \"), table_pagesize=r.r\
elpgsize, table_relversion=r.relversion, table_reltotwid=int4(r.reltot\
wid))where mod((r.relstat/32), (2))=0");
    IIsyncup((char *)0,0);
  }
/* # line 7798 "ducommon.qsc" */	/* host code */
	    break;
	}
	case DU_IIVIEWS:
	{
/* # line 7803 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of r=iirelation");
    IIsyncup((char *)0,0);
  }
/* # line 7804 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of q=iiqrytext");
    IIsyncup((char *)0,0);
  }
/* # line 7805 "ducommon.qsc" */	/* define view */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"define view iiviews(table_name=r.relid, table_owner=r.relowner, view_\
dml=byteextract(\"QS\", (mod((q.status/2), (2))+1)), check_option=byte\
extract(\"NY\", (mod(q.status, (2))+1)), text_sequence=q.seq+1, text_s\
egment=q.txt)where r.relqid1=q.txtid1 and r.");
    IIwritio(0,(short *)0,1,32,0,(char *)"relqid2=q.txtid2");
    IIsyncup((char *)0,0);
  }
/* # line 7814 "ducommon.qsc" */	/* host code */
	    break;
	}
	case DU_IIINDEXES:
	{
/* # line 7819 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of i=iiindex");
    IIsyncup((char *)0,0);
  }
/* # line 7820 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of r=iirelation");
    IIsyncup((char *)0,0);
  }
/* # line 7821 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of rb=iirelation");
    IIsyncup((char *)0,0);
  }
/* # line 7822 "ducommon.qsc" */	/* define view */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"define view iiindexes(index_name=r.relid, index_owner=r.relowner, cre\
ate_date=gmt_timestamp(r.relcreate), base_name=rb.relid, base_owner=rb\
.relowner, storage_structure=uppercase(iistructure(r.relspec)), is_com\
pressed=byteextract(\"NY\", mod((r.relstat/512");
    IIwritio(0,(short *)0,1,32,0,(char *)
"), (2))+1), key_is_compressed=byteextract(\"NY\", mod((r.relstat/1024\
), (2))+1), unique_rule=byteextract(\"DU\", mod((r.relstat/65536), (2)\
)+1), unique_scope=byteextract(\" RS\", (mod((r.relstat/65536), (2))+m\
od((r.relstat2/64), (2)))+1), system_use=");
    IIwritio(0,(short *)0,1,32,0,(char *)
"byteextract(\"USSG\", (mod(r.relstat, (2)))+(mod((r.relstat/16384), (\
2)))+(3*mod((r.relstat2/16), (2)))+1), persistent=byteextract(\"NY\", \
mod((r.relstat2/4), (2))+1), index_pagesize=r.relpgsize)where r.reltid\
=i.baseid and r.reltidx=i.indexid and rb.");
    IIwritio(0,(short *)0,1,32,0,(char *)"reltid=i.baseid and rb.relti\
dx=0");
    IIsyncup((char *)0,0);
  }
/* # line 7842 "ducommon.qsc" */	/* host code */
	    break;
	}
	case DU_IIINDEX_COLUMNS:
	{
/* # line 7847 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of r=iirelation");
    IIsyncup((char *)0,0);
  }
/* # line 7848 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of a=iiattribute");
    IIsyncup((char *)0,0);
  }
/* # line 7849 "ducommon.qsc" */	/* define view */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"define view iiindex_columns(index_name=r.relid, index_owner=r.relowne\
r, column_name=a.attname, key_sequence=int2(a.attkdom), sort_direction\
=byteextract(\"AD\", mod((a.attflag/8), (2))+1))where a.attkdom>0 and \
r.reltid=a.attrelid and r.reltidx=a.attrelidx ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"and a.attname!=\"tidp\" and a.attname!=\"TIDP\" and r.reltidx!=0");
    IIsyncup((char *)0,0);
  }
/* # line 7860 "ducommon.qsc" */	/* host code */
	    break;
	}
	case DU_IISTATS:
	{
/* # line 8067 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iistats(table_name, table_owner, column_name, create_date\
, num_unique, rept_factor, has_unique, pct_nulls, num_cells, column_do\
main, is_complete, stat_version, hist_data_length)as select r.relid, r\
.relowner, IFNULL(a.attname, 'IICOMPOSITE'), ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"date_gmt(z.sdate), float4(z.snunique), float4(z.sreptfact), byteextra\
ct('NY', z.sunique +1), float4(snull), int2(snumcells), z.sdomain, byt\
eextract('NY', z.scomplete +1), z.sversion, z.shistlength from((iirela\
tion r inner join iistatistics z on z.stabbase");
    IIwritio(0,(short *)0,1,32,0,(char *)
"=r.reltid and z.stabindex=r.reltidx)left join iiattribute a on r.relt\
id=a.attrelid and r.reltidx=a.attrelidx and z.satno=a.attid)where DBMS\
INFO('table_statistics')<>'N'");
    IIsyncup((char *)0,0);
  }
/* # line 8099 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIHISTOGRAMS:
	{
/* # line 8103 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iihistograms(table_name, table_owner, column_name, text_s\
equence, text_segment)as select r.relid, r.relowner, IFNULL(a.attname,\
 'IICOMPOSITE'), z.hsequence +1, z.histogram from((iirelation r inner \
join iihistogram z on z.htabbase=r.reltid and ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"z.htabindex=r.reltidx)left join iiattribute a on r.reltid=a.attrelid \
and r.reltidx=a.attrelidx and z.hatno=a.attid)where DBMSINFO('table_st\
atistics')<>'N'");
    IIsyncup((char *)0,0);
  }
/* # line 8115 "ducommon.sc" */	/* host code */
	    if(sqlca.sqlcode<0)
	    {
		duc_sqlerror();
	    }
	    break;
	}
	case DU_IIINGRES_TABLES:
	{
/* # line 7927 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of r=iirelation");
    IIsyncup((char *)0,0);
  }
/* # line 7928 "ducommon.qsc" */	/* define view */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"define view iiingres_tables(table_name=r.relid, table_owner=r.relowne\
r, expire_date=gmt_timestamp(r.relsave), table_integrities=byteextract\
(\"NY\", mod((r.relstat/8), (2))+1), table_permits=byteextract(\"NY\",\
 mod((r.relstat/4), (2))+1), all_to_all=");
    IIwritio(0,(short *)0,1,32,0,(char *)
"byteextract(\"YN\", mod((r.relstat/4096), (2))+1), ret_to_all=byteext\
ract(\"YN\", mod((r.relstat/8192), (2))+1), row_width=int4(r.relwid), \
is_journalled=byteextract(\"NYC\", (2*mod((r.relstat/32768), (2)))+mod\
((r.relstat/262144), (2))+1), view_base=");
    IIwritio(0,(short *)0,1,32,0,(char *)
"byteextract(\"NY\", mod((r.relstat/64), (2))+1), modify_date=gmt_time\
stamp(r.relmoddate), table_ifillpct=int2(r.relifill), table_dfillpct=i\
nt2(r.reldfill), table_lfillpct=int2(r.rellfill), table_minpages=int4(\
r.relmin), table_maxpages=int4(r.relmax), ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"location_name=char(r.relloc), table_reltid=r.reltid, table_reltidx=r.\
reltidx)");
    IIsyncup((char *)0,0);
  }
/* # line 7953 "ducommon.qsc" */	/* host code */
	    break;
	}
	case DU_IIMULTI_LOCATIONS:
	{
/* # line 7958 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of r=iirelation");
    IIsyncup((char *)0,0);
  }
/* # line 7959 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of d=iidevices");
    IIsyncup((char *)0,0);
  }
/* # line 7960 "ducommon.qsc" */	/* define view */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"define view iimulti_locations(table_name=r.relid, table_owner=r.relow\
ner, loc_sequence=d.devrelocid, location_name=d.devloc)where d.devreli\
d=r.reltid and d.devrelidx=r.reltidx and length(d.devloc)!=0");
    IIsyncup((char *)0,0);
  }
/* # line 7968 "ducommon.qsc" */	/* host code */
	    break;
	}
	case DU_IIKEY_COLUMNS:
	{
/* # line 7973 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of a=iiattribute");
    IIsyncup((char *)0,0);
  }
/* # line 7974 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of r=iirelation");
    IIsyncup((char *)0,0);
  }
/* # line 7975 "ducommon.qsc" */	/* define view */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"define view iikey_columns(table_name=r.relid, table_owner=r.relowner,\
 column_name=a.attname, key_sequence=int2(a.attkdom), sort_direction=t\
ext(\"A\"))where a.attkdom>0 and a.attrelid=r.reltid and a.attrelidx=r\
.reltidx");
    IIsyncup((char *)0,0);
  }
/* # line 7984 "ducommon.qsc" */	/* host code */
	    break;
	}
	case DU_IIEVENTS:
	{
/* # line 7989 "ducommon.qsc" */	/* define view */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"define view iievents(iievent.event_name, iievent.event_owner, text_se\
quence=int2(iiqrytext.seq)+1, text_segment=iiqrytext.txt, security_lab\
el=char(\"        \"))where iiqrytext.txtid1=iievent.event_qryid1 and \
iiqrytext.txtid2=iievent.event_qryid2");
    IIsyncup((char *)0,0);
  }
/* # line 8000 "ducommon.qsc" */	/* host code */
	    break;
	}
	case DU_IIPERMITS:
	{
/* # line 8216 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iipermits as select object_name=p.probjname, object_owner\
=p.probjowner, permit_grantor=p.prograntor, object_type=p.probjtype, c\
reate_date=gmt_timestamp(p.procrtime1), permit_user=p.prouser, permit_\
depth=p.prodepth, permit_number=int2(p.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"propermid), text_sequence=int2(q.seq) +1, text_segment=q.txt from iiq\
rytext q, iiprotect p where p.proqryid1=q.txtid1 and p.proqryid2=q.txt\
id2 and q.mode=19 and mod((p.proopset/128), (2))=0");
    IIsyncup((char *)0,0);
  }
/* # line 8231 "ducommon.sc" */	/* host code */
	    if(sqlca.sqlcode<0)
	    {
		duc_sqlerror();
	    }
	    break;
	}
	case DU_IISECURITY_ALARMS:
	{
	    duc_v_iisecurity_alarms(is_iidbdb);
	    break;
	}
	case DU_IIALT_COLUMNS:
	{
	    /*
	    ** iialt_columns - This view gives column information about unique
	    ** secondary indexes for a table. 
	    ** The table_name and table_owner are for the base table
	    ** The other 3 columns are for the unique secondary index
	    ** defined on that table. The 'tidp' column is not shown.
	    */
/* # line 8251 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iialt_columns(table_name, table_owner, key_id, column_nam\
e, key_sequence)as select r1.relid, r1.relowner, r2.reltidx, a.attname\
, int2(a.attkdom)from iirelation r1, iirelation r2, iiattribute a wher\
e r1.relidxcount>0 and r1.reltid=r2.reltid and");
    IIwritio(0,(short *)0,1,32,0,(char *)
" r2.reltidx!=0 and MOD(r2.relstat/65536, (2))=1 and r2.reltid=a.attre\
lid and r2.reltidx=a.attrelidx and a.attname!='tidp' and a.attname!='T\
IDP'");
    IIsyncup((char *)0,0);
  }
/* # line 8261 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIFILE_INFO:
	{
	    /* This view gives the physical filenames associated with a
	    ** table for each location that the table exists in.
	    **
	    ** This is used primarily by tech support, but may be made
	    ** available to the customer, upon request.
	    */
/* # line 8272 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iifile_info(table_name, owner_name, file_name, file_ext, \
location, base_id, index_id)as select r.relid, r.relowner, ii_tabid_di\
(reltid, reltidx), 't00', r.relloc, r.reltid, r.reltidx from iirelatio\
n r where r.reltidx=0 and(mod((r.relstat/32), ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"(2))=0)union all select r.relid, r.relowner, ii_tabid_di(reltid, relt\
idx), 't00', r.relloc, r.reltid, r.reltidx from iirelation r where r.r\
eltidx!=0 union all select r.relid, r.relowner, ii_tabid_di(reltid, re\
ltidx), 't' +byteextract('0123456789abcdef', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"mod((d.devrelocid/16), (16)) +1) +byteextract('0123456789abcdef', mod\
((d.devrelocid), (16)) +1), d.devloc, r.reltid, r.reltidx from iirelat\
ion r, iidevices d where r.reltidx=0 and(mod((r.relstat/32), (2))=0)an\
d d.devrelid=r.reltid and d.devrelidx=r.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"reltidx and(mod((r.relstat/4194304), (2))!=0)union all select r.relid\
, r.relowner, ii_tabid_di(reltid, reltidx), 't' +byteextract('01234567\
89abcdef', mod((d.devrelocid/16), (16)) +1) +byteextract('0123456789ab\
cdef', mod((d.devrelocid), (16)) +1), d.devloc");
    IIwritio(0,(short *)0,1,32,0,(char *)
", r.reltid, r.reltidx from iirelation r, iidevices d where r.reltidx!\
=0 and(mod((r.relstat/4194304), (2))!=0)and d.devrelid=r.reltid and d.\
devrelidx=r.reltidx");
    IIsyncup((char *)0,0);
  }
/* # line 8317 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IILOG_HELP:
	{
/* # line 8122 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of r=iirelation");
    IIsyncup((char *)0,0);
  }
/* # line 8123 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of a=iiattribute");
    IIsyncup((char *)0,0);
  }
/* # line 8124 "ducommon.qsc" */	/* define view */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"define view iilog_help(table_name=r.relid, table_owner=r.relowner, cr\
eate_date=gmt_timestamp(r.relcreate), table_type=char(byteextract(\"TV\
I\", (mod((r.relstat/32), (2))+(2*(mod((r.relstat/128), (2)))))+1)+\" \
      \"), table_subtype=char(\"N\"), ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"table_version=char(\"II9.0\"), system_use=byteextract(\"USS\", (mod(r\
.relstat, (2)))+(mod((r.relstat/16384), (2)))+1), column_name=a.attnam\
e, column_datatype=iitypename(a.attfrmt), column_length=iiuserlen(a.at\
tfrmt, (a.attfrml*65536)+a.attfrmp), ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"column_nulls=byteextract(\"NY\", (abs((a.attfrmt/abs(a.attfrmt))-1)/2\
)+1), column_defaults=byteextract(\"YN\", mod(a.attflag, (2))+1+(abs((\
a.attfrmt/abs(a.attfrmt))-1)/2)), column_sequence=int2(a.attid), key_s\
equence=int2(a.attkdom))where a.attrelid=r.");
    IIwritio(0,(short *)0,1,32,0,(char *)"reltid and a.attrelidx=r.rel\
tidx");
    IIsyncup((char *)0,0);
  }
/* # line 8149 "ducommon.qsc" */	/* host code */
	    break;
	}
	case DU_IIPROCEDURES:
	{
/* # line 8154 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of q=iiqrytext");
    IIsyncup((char *)0,0);
  }
/* # line 8155 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of p=iiprocedure");
    IIsyncup((char *)0,0);
  }
/* # line 8156 "ducommon.qsc" */	/* define view */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"define view iiprocedures(procedure_name=p.dbp_name, procedure_owner=p\
.dbp_owner, create_date=gmt_timestamp(p.dbp_create_date), proc_subtype\
=\"N\", text_sequence=int2(q.seq)+1, text_segment=q.txt, system_use=by\
teextract(\"UG\", mod((p.dbp_mask1/128), (2))+");
    IIwritio(0,(short *)0,1,32,0,(char *)
"1), security_label=char(\"        \"))where q.txtid1=p.dbp_txtid1 and\
 q.txtid2=p.dbp_txtid2");
    IIsyncup((char *)0,0);
  }
/* # line 8168 "ducommon.qsc" */	/* host code */
	    break;
	}
    case DU_IIREGISTRATIONS:
	{
/* # line 8173 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of r=iirelation");
    IIsyncup((char *)0,0);
  }
/* # line 8174 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of q=iiqrytext");
    IIsyncup((char *)0,0);
  }
/* # line 8175 "ducommon.qsc" */	/* define view */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"define view iiregistrations(object_name=r.relid, object_owner=r.relow\
ner, object_dml=byteextract(\"QS\", (mod((q.status/2), (2))+1)), objec\
t_type=char(byteextract(\"TVI\", (mod((r.relstat/32), (2))+(2*(mod((r.\
relstat/128), (2)))))+1)+\" \"), ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"object_subtype=byteextract(\"LL I\", q.mode-99), text_sequence=q.seq+\
1, text_segment=q.txt)where r.relqid1=q.txtid1 and r.relqid2=q.txtid2 \
and(q.mode=100 or q.mode=101 or q.mode=103)");
    IIsyncup((char *)0,0);
  }
/* # line 8187 "ducommon.qsc" */	/* host code */
	    /*
	    **  q.mode = DB_CRT_LINK = 100 for CREATE...AS LINK
	    **			(obsolete but still accepted)
	    **	   = DB_REG_LINK = 101 for REGISTER...AS LINK
	    **			(how we want users to express it)
	    **	   = DB_REG_IMPORT = 103 for REGISTER...AS IMPORT
	    */
	    break;
	}
	case DU_IIDB_COMMENTS:
	{
/* # line 8419 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iidb_comments(object_name, object_owner, object_type, sho\
rt_remark, text_sequence, long_remark)as select relid, relowner, char(\
'T'), short_remark, text_sequence +1, long_remark from iirelation, iid\
bms_comment where reltid=comtabbase and ");
    IIwritio(0,(short *)0,1,32,0,(char *)"reltidx=comtabidx and comtyp\
e=1");
    IIsyncup((char *)0,0);
  }
/* # line 8428 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIDB_SUBCOMMENTS:
	{
/* # line 8432 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iidb_subcomments(object_name, object_owner, subobject_nam\
e, subobject_type, short_remark, text_sequence, long_remark)as select \
relid, relowner, attname, char('C'), short_remark, text_sequence +1, l\
ong_remark from iirelation, iidbms_comment, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"iiattribute where reltid=comtabbase and reltidx=comtabidx and attintl\
_id=comattid and attrelid=reltid and attrelidx=reltidx and comtype=2");
    IIsyncup((char *)0,0);
  }
/* # line 8443 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IISYNONYMS:
	{
/* # line 8447 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iisynonyms(synonym_name, synonym_owner, table_name, table\
_owner)as select synonym_name, synonym_owner, relid, relowner from iir\
elation, iisynonym where reltid=syntabbase and reltidx=syntabidx");
    IIsyncup((char *)0,0);
  }
/* # line 8452 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIINTEGRITIES:
	{
/* # line 8243 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of i=iiintegrity");
    IIsyncup((char *)0,0);
  }
/* # line 8244 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of r=iirelation");
    IIsyncup((char *)0,0);
  }
/* # line 8245 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of q=iiqrytext");
    IIsyncup((char *)0,0);
  }
/* # line 8246 "ducommon.qsc" */	/* define view */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"define view iiintegrities(table_name=r.relid, table_owner=r.relowner,\
 create_date=gmt_timestamp(i.cons_create_date), integrity_number=int2(\
i.intnumber), text_sequence=int2(q.seq)+1, text_segment=q.txt)where r.\
reltid=i.inttabbase and r.reltidx=i.inttabidx ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"and q.txtid1=i.intqryid1 and q.txtid2=i.intqryid2 and q.mode=20 and i\
.consflags=0");
    IIsyncup((char *)0,0);
  }
/* # line 8258 "ducommon.qsc" */	/* host code */
	    break;
	}
	case DU_IIRULES:
	{
/* # line 8488 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iirules as select rule_name=i.rule_name, rule_owner=i.rul\
e_owner, table_name=r.relid, text_sequence=int2(q.seq) +1, text_segmen\
t=q.txt, system_use=byteextract('UG', mod((i.rule_flags/8), (2)) +1)fr\
om iirelation r, (iirule i left join iiqrytext");
    IIwritio(0,(short *)0,1,32,0,(char *)
" q on q.txtid1=i.rule_qryid1 and q.txtid2=i.rule_qryid2)where r.relti\
d=i.rule_tabbase and r.reltidx=i.rule_tabidx");
    IIsyncup((char *)0,0);
  }
/* # line 8503 "ducommon.sc" */	/* host code */
	    if(sqlca.sqlcode<0)
	    {
		duc_sqlerror();
	    }
	    break;
	}
	case DU_IICONSTRAINTS:
	{
/* # line 8511 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iiconstraints as select constraint_name=i.consname, schem\
a_name=r.relowner, table_name=r.relid, constraint_type=byteextract('UC\
 R    P', mod(i.consflags, 16)), create_date=gmt_timestamp(i.cons_crea\
te_date), text_sequence=q.seq +1, text_segment");
    IIwritio(0,(short *)0,1,32,0,(char *)
"=q.txt, system_use=byteextract('UG', mod(i.consflags/32, 2) +1)from i\
iintegrity i, iirelation r, iiqrytext q where i.inttabbase=r.reltid an\
d i.inttabidx=r.reltidx and i.intqryid1=q.txtid1 and i.intqryid2=q.txt\
id2 and q.mode=20 and i.consflags!=0 and i.");
    IIwritio(0,(short *)0,1,32,0,(char *)"intseq=0");
    IIsyncup((char *)0,0);
  }
/* # line 8526 "ducommon.sc" */	/* host code */
	    if(sqlca.sqlcode<0)
	    {
		duc_sqlerror();
	    }
	    break;
	}
	case DU_IIKEYS:
	{
/* # line 8534 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iikeys as select constraint_name=i.consname, schema_name=\
r.relowner, table_name=r.relid, column_name=a.attname, key_position=k.\
key_position from iiintegrity i, iirelation r, iiattribute a, iikey k \
where k.key_consid1=i.consid1 and k.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"key_consid2=i.consid2 and i.inttabbase=r.reltid and i.inttabidx=r.rel\
tidx and i.inttabbase=a.attrelid and i.inttabidx=a.attrelidx and k.key\
_attid=a.attintl_id");
    IIsyncup((char *)0,0);
  }
/* # line 8546 "ducommon.sc" */	/* host code */
	    if(sqlca.sqlcode<0)
	    {
		duc_sqlerror();
	    }
	    break;
	}
	case DU_IIREF_CONSTRAINTS:
	{
/* # line 8554 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iiref_constraints as select ref_constraint_name=i.consnam\
e, ref_schema_name=r.relowner, ref_table_name=r.relid, unique_constrai\
nt_name=j.consname, unique_schema_name=s.relowner, unique_table_name=s\
.relid from iiintegrity i, iirelation r, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"iidbdepends d, iiintegrity j, iirelation s where d.dtype=149 and d.it\
ype=149 and d.deid1=i.inttabbase and d.deid2=i.inttabidx and d.qid=i.i\
ntnumber and i.inttabbase=r.reltid and i.inttabidx=r.reltidx and d.ini\
d1=j.inttabbase and d.inid2=j.inttabidx and d.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"i_qid=j.intnumber and j.inttabbase=s.reltid and j.inttabidx=s.reltidx");
    IIsyncup((char *)0,0);
  }
/* # line 8577 "ducommon.sc" */	/* host code */
	    if(sqlca.sqlcode<0)
	    {
		duc_sqlerror();
	    }
	    break;
	}
	case DU_IICONSTRAINT_INDEXES:
	{
/* # line 8585 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iiconstraint_indexes as select constraint_name=i.consname\
, schema_name=r.relowner, index_name=r.relid from iiintegrity i, iirel\
ation r, iidbdepends d where d.dtype=10 and d.itype=149 and i.inttabba\
se=d.inid1 and i.inttabidx=d.inid2 and i.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"intnumber=d.i_qid and d.deid1=r.reltid and d.deid2=r.reltidx");
    IIsyncup((char *)0,0);
  }
/* # line 8597 "ducommon.sc" */	/* host code */
	    if(sqlca.sqlcode<0)
	    {
		duc_sqlerror();
	    }
	    break;
	}
	case DU_IIACCESS:
	{
/* # line 8605 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iiaccess as select table_name=r.relid, table_owner=r.relo\
wner, table_type=byteextract('TVI', (mod((r.relstat/32), (2)) +(2*(mod\
((r.relstat/128), (2))))) +1), system_use=byteextract('USSG', (mod(r.r\
elstat, (2))) +(mod((r.relstat/16384), (2))) +");
    IIwritio(0,(short *)0,1,32,0,(char *)
"(3*mod((r.relstat2/16), (2))) +1), permit_user=p.prouser, permit_type\
=iipermittype(p.proopset, 0)from iirelation r, iiprotect p where r.rel\
tid=p.protabbase and(mod((p.proopset/128), 2)=0)");
    IIsyncup((char *)0,0);
  }
/* # line 8620 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIDD_DDB_VIEWS:
	{
	    duc_v_iidd_ddb_views();
	    break;
	}
	case DU_IIDD_DBCONSTANTS:
	{
	    duc_v_iidd_dbconstants();
	    break;
	}
	case DU_IIDD_PHYSICAL_TABLES:
	{
	    duc_v_iidd_physical_tables();
	    break;
	}
	case DU_IIDD_REGISTERED_OBJECTS:
	{
	    duc_v_iidd_registered_objects();
	    break;
	}
	case DU_IISEQUENCES:
	{
	    /* Note the naughty use of explicit constants for the data type codes. 
	    ** There's not much to be done, since the references are contained
	    ** inside the SQL code. */
/* # line 8647 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iisequences(seq_name, seq_owner, create_date, modify_date\
, data_type, seq_length, seq_precision, start_value, increment_value, \
next_value, min_value, max_value, cache_size, start_flag, incr_flag, m\
in_flag, max_flag, restart_flag, cache_flag, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"cycle_flag, order_flag)as select seq_name, seq_owner, seq_create, seq\
_modify, case when seq_type=30 then 'integer' else 'decimal' end, seq_\
length, seq_prec, case when seq_type=30 then seq_start_int else seq_st\
art_dec end, case when seq_type=30 then ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"seq_incr_int else seq_incr_dec end, case when seq_type=30 then seq_ne\
xt_int else seq_next_dec end, case when seq_type=30 then seq_min_int e\
lse seq_min_dec end, case when seq_type=30 then seq_max_int else seq_m\
ax_dec end, seq_cache, byteextract(' Y', mod(");
    IIwritio(0,(short *)0,1,32,0,(char *)
"seq_flag/1024, 2) +1), byteextract(' Y', mod(seq_flag/2048, 2) +1), b\
yteextract(' NY', mod(seq_flag/128, 2) +mod(seq_flag/4, 2)*2 +1), byte\
extract(' NY', mod(seq_flag/64, 2) +mod(seq_flag/2, 2)*2 +1), byteextr\
act(' Y', mod(seq_flag/4096, 2) +1), ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"byteextract(' NY', mod(seq_flag/256, 2) +mod(seq_flag/8, 2)*2 +1), by\
teextract('NY', mod(seq_flag/1, 2) +1), byteextract('NY', mod(seq_flag\
/16, 2) +1)from iisequence");
    IIsyncup((char *)0,0);
  }
/* # line 8686 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIDISTSCHEMES:
	{
/* # line 8690 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iidistschemes as select r.relid as table_name, r.relowner\
 as table_owner, d.levelno as dimension, d.ncols as partitioning_colum\
ns, d.nparts as logical_partitions, case d.distrule when 0 then 'AUTOM\
ATIC' when 1 then 'HASH' when 2 then 'LIST' ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"when 3 then 'RANGE' else '?' end as partition_rule from iirelation r,\
 iidistscheme d where d.mastid=r.reltid and d.mastidx=r.reltidx");
    IIsyncup((char *)0,0);
  }
/* # line 8705 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIDISTCOLS:
	{
/* # line 8709 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iidistcols as select r.relid as table_name, r.relowner as\
 table_owner, d.levelno as dimension, a.attname as column_name, d.cols\
eq as column_sequence, UPPERCASE(IITYPENAME(II_EXT_TYPE(a.attfrmt, a.a\
ttfrml)))as column_datatype from iirelation r");
    IIwritio(0,(short *)0,1,32,0,(char *)
", iidistcol d, iiattribute a where d.mastid=r.reltid and d.mastidx=r.\
reltidx and a.attrelid=r.reltid and a.attrelidx=r.reltidx and a.attid=\
d.attid");
    IIsyncup((char *)0,0);
  }
/* # line 8720 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IILPARTITIONS:
	{
/* # line 8724 "ducommon.sc" */	/* create view */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create view iilpartitions as select r.relid as table_name, r.relowner\
 as table_owner, d.levelno as dimension, d.partseq as logical_partseq,\
 d.partname as partition_name, IFNULL(v.valseq, 0)as value_sequence, I\
FNULL(v.colseq, 0)as column_sequence, case v.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"oper when 0 then '<' when 1 then '<=' when 2 then '=' when 4 then '>=\
' when 5 then '>' when 6 then 'DEFAULT' else ' ' end as operator, v.di\
stvalue as value from iirelation r join iipartname d on d.mastid=r.rel\
tid and d.mastidx=r.reltidx left join ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"iidistval v on v.mastid=d.mastid and v.mastidx=d.mastidx and v.leveln\
o=d.levelno and v.partseq=d.partseq");
    IIsyncup((char *)0,0);
  }
/* # line 8747 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIOTABLES:
	{
	    /* Ancient, deprecated iitables */
/* # line 8552 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of r=iirelation");
    IIsyncup((char *)0,0);
  }
/* # line 8553 "ducommon.qsc" */	/* define view */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"define view iiotables(table_name=r.relid, table_owner=r.relowner, cre\
ate_date=gmt_timestamp(r.relcreate), alter_date=gmt_timestamp(r.relcre\
ate), table_type=char(byteextract(\"TVI\", (mod((r.relstat/32), (2))+(\
2*(mod((r.relstat/128), (2)))))+1)+\"       \"");
    IIwritio(0,(short *)0,1,32,0,(char *)
"), table_subtype=char(\"N\"), table_version=char(\"II9.0\"), system_u\
se=byteextract(\"USS\", (mod(r.relstat, (2)))+(mod((r.relstat/16384), \
(2)))+1))");
    IIsyncup((char *)0,0);
  }
/* # line 8567 "ducommon.qsc" */	/* host code */
	    break;
	}
	case DU_IIOCOLUMNS:
	{
/* # line 8572 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of a=iiattribute");
    IIsyncup((char *)0,0);
  }
/* # line 8573 "ducommon.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of r=iirelation");
    IIsyncup((char *)0,0);
  }
/* # line 8574 "ducommon.qsc" */	/* define view */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"define view iiocolumns(table_name=r.relid, table_owner=r.relowner, co\
lumn_name=a.attname, create_date=gmt_timestamp(r.relcreate), alter_dat\
e=gmt_timestamp(r.relcreate), column_datatype=uppercase(iitypename(a.a\
ttfrmt)), column_length=iiuserlen(a.attfrmt, (");
    IIwritio(0,(short *)0,1,32,0,(char *)
"a.attfrml*65536)+a.attfrmp), column_scale=int2(mod(a.attfrmp, (256)))\
, column_nulls=byteextract(\"NY\", (abs((a.attfrmt/abs(a.attfrmt))-1)/\
2)+1), column_defaults=byteextract(\"YN\", mod(a.attflag, (2))+1+(abs(\
(a.attfrmt/abs(a.attfrmt))-1)/2)), ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"column_sequence=int2(a.attid), key_sequence=int2(a.attkdom*(mod((r.re\
lstat/65536), (2)))))where a.attrelid=r.reltid and a.attrelidx=r.relti\
dx");
    IIsyncup((char *)0,0);
  }
/* # line 8592 "ducommon.qsc" */	/* host code */
	    break;
	}
	default:
	{
	    printf("Unknown view %s\n", view->du_objname);
	    break;
	}
    } /* end switch */
}
static STATUS
duc_create_one_internal_proc(
char		*dbname,
DU_OBJ		*proc,
bool		upcase)
{
  char buf[256];
    DU_ERROR		duc_errcb;
    IIlq_Protect(TRUE);
    switch (proc->du_obj_id)
    {
	case DU_IIQEF_CREATE_DB:
	{
/* # line 8828 "ducommon.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"iiqef_create_db");
    IIwritio(0,(short *)0,1,32,0,(char *)
"create procedure iiqef_create_db(");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                dbname         = char(32) not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                db_loc_name    = char(32) not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                jnl_loc_name   = char(32) not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                ckp_loc_name   = char(32) not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                dmp_loc_name   = char(32) not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                srt_loc_name   = char(32) not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                db_access      = integer  not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                collation      = char(32) not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                need_dbdir_flg = integer  not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                db_service     = integer  not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                translated_owner_name   = char(32) not null not defau\
lt,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                untranslated_owner_name = char(32) not null not defau\
lt,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                ucollation     = char(32) not null not default)");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"            AS BEGIN");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                EXECUTE INTERNAL;");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"            END");
    IIputctrl(0);
    IIsyncup((char *)0,0);
  }
/* # line 8845 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIQEF_ALTER_DB:
	{
/* # line 8849 "ducommon.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"iiqef_alter_db");
    IIwritio(0,(short *)0,1,32,0,(char *)"create procedure iiqef_alter\
_db(");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                dbname = char(32) not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                access_on = integer not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                access_off = integer not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                service_on = integer not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                service_off = integer not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                last_table_id = integer not null not default)");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"            AS BEGIN");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                EXECUTE INTERNAL;");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"            END");
    IIputctrl(0);
    IIsyncup((char *)0,0);
  }
/* # line 8859 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIQEF_DESTROY_DB:
	{
/* # line 8863 "ducommon.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"iiqef_destroy_db");
    IIwritio(0,(short *)0,1,32,0,(char *)
"create procedure iiqef_destroy_db(");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    dbname = char(32) not null not default)");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"            AS BEGIN");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                EXECUTE INTERNAL;");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"            END");
    IIputctrl(0);
    IIsyncup((char *)0,0);
  }
/* # line 8868 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIQEF_ADD_LOCATION:
	{
/* # line 8872 "ducommon.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"iiqef_add_location");
    IIwritio(0,(short *)0,1,32,0,(char *)
"create procedure iiqef_add_location(");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    database_name = char(32) not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    location_name = char(32) not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    access = integer not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    need_dbdir_flg = integer not null not default)");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"            AS BEGIN");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                EXECUTE INTERNAL;");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"            END");
    IIputctrl(0);
    IIsyncup((char *)0,0);
  }
/* # line 8880 "ducommon.sc" */	/* host code */
	    break;
	}
	case DU_IIQEF_ALTER_EXTENSION:
	{
	    if (!proc_exists("iiqef_alter_extension"))
	    {
/* # line 8886 "ducommon.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"iiqef_alter_extension");
    IIwritio(0,(short *)0,1,32,0,(char *)
"create procedure iiqef_alter_extension(");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                        database_name = char(32) not null not default\
,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                        location_name = char(32) not null not default\
,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                        drop_loc_type = integer not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                        add_loc_type = integer not null not default)");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                AS BEGIN");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    EXECUTE INTERNAL;");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                END");
    IIputctrl(0);
    IIsyncup((char *)0,0);
  }
/* # line 8894 "ducommon.sc" */	/* host code */
	    }
	    break;
	}
	case DU_IIERROR:
	{
	    if (!proc_exists("iierror"))
	    {
/* # line 8901 "ducommon.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"iierror");
    IIwritio(0,(short *)0,1,32,0,(char *)"create procedure iierror");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                        (errorno integer not null with default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                         detail  integer not null with default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                         p_count integer not null with default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                         p1      varchar(256) not null with default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                         p2      varchar(256) not null with default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                         p3      varchar(256) not null with default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                         p4      varchar(256) not null with default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                         p5      varchar(256) not null with default)");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                AS");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                    BEGIN");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                        execute internal;");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"                    END");
    IIputctrl(0);
    IIsyncup((char *)0,0);
  }
/* # line 8914 "ducommon.sc" */	/* host code */
	    }
	    break;
	}
	case DU_IIQEF_DEL_LOCATION:
	{
/* # line 8919 "ducommon.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"iiqef_del_location");
    IIwritio(0,(short *)0,1,32,0,(char *)
"create procedure iiqef_del_location(");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    database_name = char(32) not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    location_name = char(32) not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    access = integer not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                    need_dbdir_flg = integer not null not default)");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"            AS BEGIN");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"                EXECUTE INTERNAL;");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"            END");
    IIputctrl(0);
    IIsyncup((char *)0,0);
  }
/* # line 8927 "ducommon.sc" */	/* host code */
	    break;
	}
	default:
	{
	    printf("Unknown procedure %s\n", proc->du_objname);
	    break;
	}
    }
    if (sqlca.sqlcode < 0)
    {
	if (sqlerr_handler(&duc_errcb,E_DU200C_BAD_IPROC_EXECUTE) == E_DU_FATAL)
	{
	    IIlq_Protect(FALSE);
	    return (E_DU_FATAL);
	}
    }
    STprintf(buf, "GRANT EXECUTE ON PROCEDURE %s TO PUBLIC", proc->du_objname);
/* # line 8944 "ducommon.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(buf);
    IIsyncup((char *)0,0);
  }
/* # line 8945 "ducommon.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	IIlq_Protect(FALSE);
	if (sqlerr_handler(&duc_errcb,E_DU200C_BAD_IPROC_EXECUTE) == E_DU_FATAL)
	    return (E_DU_FATAL);
    }
    IIlq_Protect(FALSE);
    return (E_DU_OK);
}
bool
proc_exists(proc_name)
  char *proc_name;
{
  i4 cnt;
/* # line 8963 "ducommon.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"ducommon4",6223,25555);
      IIputdomio((short *)0,1,32,0,proc_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"ducommon4",6223,25555);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiprocedure where lowercase(dbp_name)=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,proc_name);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and lowercase(dbp_owner)='$ingres'");
        IIexDefine(0,(char *)"ducommon4",6223,25555);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 8966 "ducommon.sc" */	/* host code */
    if (cnt > 0)
	return TRUE;
    else
	return (FALSE);
}
STATUS
duc_drop_internal_procs(
char            *dbname,
bool		star_flag)
{
    bool	is_iidbdb;
    i4		i;
    STATUS	status;
  char sbuffer[256];
    if (!STcompare(dbname, DU_DBDBNAME))
	is_iidbdb = TRUE;
    else 
	is_iidbdb = FALSE;
    /* Drop in reverse order of create */
    for (i = MAX_INTERNAL_PROCS - 1; i >= 0; i--)
    {
	if (Du_dbprocs[i].du_dbflag == DU_IIDBDB && !is_iidbdb)
	    continue;
	if (Du_dbprocs[i].du_dbflag == DU_STAR && !star_flag)
	    continue;
	if (!proc_exists(Du_dbprocs[i].du_objname))
	    continue;
	STprintf(sbuffer, "drop procedure %s\n", Du_dbprocs[i].du_objname);
/* # line 8996 "ducommon.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
  }
/* # line 8997 "ducommon.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    /* XXX this should be msg-ized! */
	    SIprintf("]==> <%s> returns %d\n", sbuffer, sqlca.sqlcode);
	}
    }
/* # line 9003 "ducommon.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9004 "ducommon.sc" */	/* host code */
}
STATUS
duc_upgrade_internal_procs(
char            *dbname,
bool		upcase,
bool		star_flag)
{
    bool	is_iidbdb;
    i4		i;
    STATUS	status;
    if (!STcompare(dbname, DU_DBDBNAME))
	is_iidbdb = TRUE;
    else 
	is_iidbdb = FALSE;
    for (i = 0; i < MAX_INTERNAL_PROCS; i++)
    {
	if (Du_dbprocs[i].du_dbflag == DU_IIDBDB && !is_iidbdb)
	    continue;
	if (Du_dbprocs[i].du_dbflag == DU_STAR && !star_flag)
	    continue;
	if (!proc_exists(Du_dbprocs[i].du_objname))
	    status = duc_create_one_internal_proc(dbname, &Du_dbprocs[i],upcase);
    }
/* # line 9027 "ducommon.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9028 "ducommon.sc" */	/* host code */
    return (E_DU_OK);
}
STATUS
duc_create_internal_procs(
char            *dbname,
bool		upcase,
bool		star_flag)
{
    bool	is_iidbdb;
    i4		i;
    STATUS	status;
    if (!STcompare(dbname, DU_DBDBNAME))
	is_iidbdb = TRUE;
    else 
	is_iidbdb = FALSE;
    for (i = 0; i < MAX_INTERNAL_PROCS; i++)
    {
	if (Du_dbprocs[i].du_dbflag == DU_IIDBDB && !is_iidbdb)
	    continue;
	if (Du_dbprocs[i].du_dbflag == DU_STAR && !star_flag)
	    continue;
	status = duc_create_one_internal_proc(dbname, &Du_dbprocs[i], upcase);
    }
/* # line 9051 "ducommon.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9052 "ducommon.sc" */	/* host code */
}
/*{
** Name: duc_modify_pagesize() -     modify the system catalogs.
**
** Description:
**        This routine modifies a database's back-end and standard catalogs
**      to a predetermined storage structure.  The catalogs to be modified
**      are determined by the values set in dus_modes when the command line
**      is parsed.
**
** Inputs:
**	page_size		Page size to modify to
**	star_flag		TRUE if a star database
**	err_cb			DU_ERROR * place to return error codes
**
**
** Outputs:
**      Returns:
**          E_DU_OK                     Completed successfully.
**      Exceptions:
**          Some Ingres/Equel error might be detected which will cause
**          the Ingres error handler for this utility to be called.
**
** Side Effects:
**         The back-end & standard catalogs are modified to a predetermined
**         storage structure.
**
** History: 
**         21-Apr-2004-(nansa02)
**         Created from sysmod.
**	11-Sep-2004 (schka24)
**	    Move to common so that upgradedb can use it.
**
*/ 
DU_STATUS
duc_modify_pagesize(i4 page_size, bool star_flag, DU_ERROR *duc_errcb)
{
  char *cp;
    char	**cp_arr;
    DUU_CATDEF	*catdef_p;
    char        *where;
    char        modbuf[256];
    i4		i,j;
    IIlq_Protect(TRUE);
    if (duc_find_cats(page_size, duc_errcb, star_flag) != E_DU_OK)
	return (E_DU_IERROR);
    for (i = 0; i < DU_MAXCAT_DEFS ; i++)
    {
	/* If not a distributed database, skip Star catalogs */
	if ( ! star_flag && (i == DU_DDB_CATS) )
	    continue;
	if (Dus_allcat_defs[i] == NULL)
      /* There's a set of catalog definitions missing. */
	    continue;
	for (catdef_p = Dus_allcat_defs[i];
	     catdef_p->du_relname;
	     catdef_p++
	    )
	{
	    if (catdef_p->du_exists)
	    {
		if (catdef_p->du_modify)
		{
		    cp = catdef_p->du_modify;
		    if (du_error(duc_errcb, I_DU0100_MODIFYING_SY, 2,
				 0, catdef_p->du_relname) != E_DU_OK
		       )
			return(duc_errcb->du_status);
		    if (page_size != 0)
		    {
			cp = &modbuf[0];
			if (STstrindex(catdef_p->du_modify, "where", 0, TRUE)
			    || STstrindex(catdef_p->du_modify, "with", 0, TRUE))
			    cp = STprintf(cp, "%s ,page_size=%d",
					catdef_p->du_modify, page_size);
		      else
			    cp = STprintf(cp, "%s with page_size=%d",
					catdef_p->du_modify, page_size);
		    }
/* # line 8966 "ducommon.qsc" */	/* iistatement */
  {
    IIwritio(1,(short *)0,1,32,0,cp);
    IIsyncup((char *)0,0);
  }
/* # line 8967 "ducommon.qsc" */	/* host code */
		}
		if (catdef_p->du_index_cnt > 0)
		{
		    for (j = 1, cp_arr = catdef_p->du_index;
			 j <= catdef_p->du_index_cnt; j++, cp_arr++
			)
		    {
			cp  = *cp_arr;
			if (page_size != 0)
			{
			    cp = &modbuf[0];
			    if (STstrindex(*cp_arr, "where", 0, TRUE)
				|| STstrindex(*cp_arr, "with", 0, TRUE))
				cp = STprintf(cp, "%s ,page_size=%d",
					    *cp_arr, page_size);
			    else
				cp = STprintf(cp, "%s with page_size=%d",
					    *cp_arr, page_size);
			}
/* # line 8986 "ducommon.qsc" */	/* iistatement */
  {
    IIwritio(1,(short *)0,1,32,0,cp);
    IIsyncup((char *)0,0);
  }
/* # line 8987 "ducommon.qsc" */	/* host code */
		    }
		}
		continue;
	    }
	}
    }	/* end of while loop. */
    IIlq_Protect(FALSE);
    return(E_DU_OK);
}
/*{
** Name: duc_find_cats() -  find the system catalogs that exist in this db.
**
** Description:
**	This routine retrieves the names of all the system catalogs
**	in the database that are not already the specified page size.
**	Extended catalogs (ii_xxx style) are skipped.  The database
**	should already be open.
**
**      Note that if the database is distributed, the catalog information
**      will retrieve from iidd_ddb_objects catalog.
**
** Inputs:
**	page_size			Desired page size
**      duc_errcb                       Sysmod error-handling control block.
**      ddb_flag                        value of (iidatabase.dbservice &
**                                          DU_1SER_DDB )
**
** Outputs:
**      *duc_modes
**      *duc_errcb                      If an error occurs, this block will be
**                                      set by a call to du_error().
**      Returns:
**          E_DU_OK                     Successful completion.
**          W_DU1030_UNKNOWN_CATALOG_SY This is not a fatal error.  It will
**                                      inform the user that a catalog exists
**                                      which this utility doesn't know about.
**      Exceptions:
**          none
**
** Side Effects:
**          none
**
** History :
**          21-Apr-2004 (nansa02)
**            Created from sysmod.
**
*/
DU_STATUS
  duc_find_cats(page_size,duc_errcb,star_flag)
  i4 page_size;
DU_ERROR *duc_errcb;
i4 star_flag;
  {
    char cat_name[DB_MAXNAME + 1];
	i4	    i;
	DUU_CATDEF   *catdef_p;
    if ( star_flag )
    {
/* # line 9053 "ducommon.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"range of a=iidd_ddb_object\
s");
      IIsyncup((char *)0,0);
    }
/* # line 9054 "ducommon.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"range of b=iidd_ddb_tablei\
nfo");
      IIsyncup((char *)0,0);
    }
/* # line 9055 "ducommon.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve unique(cat_name=a.object_name)where a.system_object=\"Y\" an\
d(a.object_type=\"L\" or a.object_type=\"T\")and b.object_base=a.objec\
t_base and b.local_type=\"T\"");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,cat_name);
        if (IIerrtest() != 0) goto IIrtB3;
        {
/* # line 9060 "ducommon.qsc" */	/* host code */
		STtrmwhite(cat_name);
	    for (i = 0; i < DU_MAXCAT_DEFS; i++)
	    {
		if (Dus_allcat_defs[i] == NULL)
		    /* There's a set of catalog definitions missing. */
		    continue;
		for (catdef_p = Dus_allcat_defs[i];
		     catdef_p->du_relname;
		     catdef_p++
		    )
		{
		    /*
		    ** Case insensitive compare to check system catalogs
		    */
		    if (!STcasecmp(catdef_p->du_relname, cat_name ))
		    {
			catdef_p->du_exists	= TRUE;
			break;
		    }
		}
		if (catdef_p->du_exists)
		    break;
	    }	/* end of while loop. */
	    if (i > DU_MAXCAT_DEFS)
	        du_error(duc_errcb, W_DU1030_UNKNOWN_CATALOG_SY,
			 2, 0, cat_name);
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE3:;
    }
/* # line 9091 "ducommon.qsc" */	/* host code */
    }
/* NOTE: iidbcapabilities (a standard catalog) would be excluded by this query
** without the OR clause below. It should be handled here, along with
** other standard catalogs; the "modifyfe/upgradefe" facilities only
** handle front-end catalogs. iirelation can't currently distinguish between
** a standard catalog and a front-end catalog (they are both considered
** "extended" catalogs).	(5/90 pete)
*/
/* # line 9100 "ducommon.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(cat_name=iirelation.relid)where mod(iirelation.relstat, 2)=1\
 and iirelation.relpgsize!=");
      IIputdomio((short *)0,1,30,4,&page_size);
      IIwritio(0,(short *)0,1,32,0,(char *)
"and mod(iirelation.relstat/32, 2)=0 and mod(iirelation.relstat/128, 2\
)=0 and(mod(iirelation.relstat/16384, 2)=0 or iirelation.relid=\"iidbc\
apabilities\" or iirelation.relid=\"iisectype\" or iirelation.relid=\"\
iiprivlist\")");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,cat_name);
        if (IIerrtest() != 0) goto IIrtB4;
        {
/* # line 9114 "ducommon.qsc" */	/* host code */
	    (VOID) STtrmwhite(cat_name);
	    for (i = 0; i < DU_MAXCAT_DEFS; i++)
	    {
		if (Dus_allcat_defs[i] == NULL)
		    /* There's a set of catalog definitions missing. */
		    continue;
		for (catdef_p = Dus_allcat_defs[i];
		     catdef_p->du_relname;
		     catdef_p++
		    )
		{
		    /*
		    ** Case insensitive compare to check system catalogs
		    */
		    if (!STcasecmp(catdef_p->du_relname, cat_name ))
		    {
			catdef_p->du_exists	= TRUE;
			break;
		    }
		}
		if (catdef_p->du_exists)
		    break;
	    }	/* end of while loop. */
	    if (i > DU_MAXCAT_DEFS)
	        du_error(duc_errcb, W_DU1030_UNKNOWN_CATALOG_SY,
			 2, 0, cat_name);
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE4:;
    }
/* # line 9146 "ducommon.qsc" */	/* host code */
	return(E_DU_OK);
  }
