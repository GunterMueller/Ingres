# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*  
** Copyright (c) 1989, 2005 Ingres Corporation
**
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
#include    <compat.h>
#include    <gl.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <qeaiproc.h> 
#include    <tr.h>
#include    <pc.h>
#include    <me.h>
#include    <si.h>
#include    <cs.h>
#include    <ex.h>
#include    <st.h>
#include    <ut.h>
#include    <cv.h>
#include    <lo.h>
#include    <er.h>
#include    <duf.h>
#include    <lk.h>
#include    <di.h>
#include    <nm.h>
#include    <dmf.h>
#include    <duerr.h>
#include    <duenv.h>
# include <dudbms.h>
#include    <duu.h>
#include    <lg.h>
#include    <dm.h>
#include    <dmp.h>
#include    <dm0c.h>
#include    <pm.h>
/* 24-may-1995 b68770        */
#include    <tm.h>
/* 24-may-1995 b68770        */
#include    <duvfiles.h>
# include <upgradedb.h>
# include <dustar.h>
#include    <duucatdef.h>
#include    <generr.h>
/*
** values below are copied from dudbms.qh.  Since these are needed for SQL
** and this is currrently a QUEL only file, these values are copied here.
** At some future date, we should change dudbms.qsh to make sql definitions as
** well as QUEL ones.
*/
# define DU_SQL_NONE 100
# define DU_OPERATIVE 16
# define DU_CONVERTING 32
# define DU_UPGRADING 64
# define DU_0DBMINOR 0
# define DU_1DBMINOR 1
# define DU_2DBMINOR 2
# define DU_3DBMINOR 3
# define DU_1CUR_DBCMPTMINOR 0
# define DU_1DBV61A "6.1a"
# define DU_2DVV62 "6.2"
# define DU_4DBV70 "7.0"
# define DU_5DBV634 "6.34"
# define DU_6DBV80 "8.0"
# define DU_6DBV85 "8.5"
# define DU_6DBV86 "8.6"
# define DU_6DBV9 "9.0"
# define DU_6DBV902 "9.02"
# define DU_6DBV904 "9.04"
# define DU_CUR_DBCMPTLVL "9.04"
# define DBDB_SESSIONID 1
# define DB_SESSIONID 2
# define STAR_SESSIONID 3
#define		OLD_DB_MAXNAME		24	/* for DB_MAXNAME conversion */
/* relstat bits, should be in sync with TCB bits in back!dmf!hdr dmp.h */
#define		DU_RELSTAT_DUPLICATES		0x00200000L
/*
**  Name: UPGRADEDB.QSC - Convert 6.x INGRES database or installation to the
**			  current ingres release level.
**
**  Description:
**      This program will convert a previous version 6 INGRES database
**	to the current release.
**
**	Routines in this file are arranged more or less at random.
**	Please fix me someday.
**
**  History:
**      09-jun-89 (ralph)
**          Adapted to 7.0 from jrb's cv60to61 utility
**	09-aug-89 (ralph)
**	    Added cv_sequence() and it's invocation in cv_database
**	13-aug-89 (ralph)
**	    Allow conversion to continue when database is inoperative if
**	    the DU_CONVERTING flag is also on in iidatabase.access
**	26-aug-89 (ralph)
**	    Do not convert if database is being journaled
**	    Allow conversion for any Release 6 database
**	16-sep-89 (ralph)
**	    Check server version to ensure it is 7.0.
**	    Destroy temporary work tables prior to creating them.
**	    Destroy new catalogs prior to creating them.
**	    Check for errors more frequently in conversion subroutines.
**	    Added cv_altcolumns for converting iialt_columns.
**	    Changed ii_sequence_value to ii_sequence_values & related changes.
**	22-sep-89 (ralph)
**	    Renamed to UPGRADEDB.QSC.
**	    Changed current version from 7.0 to 6.3.
**	    Allow conversion of 7.0 databases to 6.3 format.
**	11-oct-89 (ralph)
**	    Allow conversion of 7.0 databases even if journaled
**	19-oct-89 (ralph)
**	    Delete config files in the dump directory.
**	14-dec-89 (ralph)
**	    Allow conversion of 6.3 database if one was specified on the
**	    command line.  If -all specified, continue to bypass conversion.
**	19-apr-90 (ralph)
**	    Changed iidbprivileges definition, and allowed it to be recreated.
**	    Allow iidbdb to be converted even if already 6.3 when specified.
**	07-jun-90 (ralph)
**	    Add logic to qualify fe/be conversion separately.
**	02-jul-90 (ralph)
**	    Added 6.3/02 iidbcapabilities entries
**	    Ensure user is authorized to convert database as follows:
**		If "-all" is specified, user must be superuser
**		If dbname is specified, user must be superuser or DBA.
**	16-jul-90 (ralph)
**	    Correct typo when destroying previous ii_sequence_value[s]
**	13-sep-90 (ralph)
**	    Correct merge conflict resolutions
**	2-oct-90 (pete)
**	    Remove function 'cv_sequence', which creates ii_sequence_values
**	    and a database procedure based on that (_ii_sequence_value).
**	    These are now handled by upgradefe.
**     10-oct-90 (hhsiung)
**          Rework a statement to get rid of compiler warning about not
**          able to take the address of this object.
**	10-dec-90 (neil)
**	    Alerters: include iievent in 6.3/04 conversion.
**	    Modify method to create iipermits (may need *recreation* if
**	    new format but with iievent).
**	9-jan-91 (stevet)
**	    Added IIUGinit call to initialize CM attribute table.
**	1-feb-1991 (bryanp)
**	    Added support for Btree index compression: dbcmptlvl => 6.34,
**	    standard catalogs changes to iitables, iiphysical_tables, and
**	    iiindexes. Added new internal subroutines cv_tables, cv_indexes,
**	    and cv_physical_tables. set_cmptlvl returns new value.
**	    Also picked up some miscellaneous catalog changes from the Front
**	    Ends group (Pete):
**		ING6.3 ==> ING6.4, 00603 ==> 00604 (iidbcapabilities)
**		new column in iidbconstants -- system_owner
**			This change required adding cv_dbconstants.
**		correct definition of iiphysical_tables.duplicate_rows.
**		new column 'column_updateable' in iicolumns.
**		sort direction in iiindex_columns no longer hard coded.
**			This change required adding cv_index_columns.
**	28-mar-1991 (bryanp)
**	    Repaired previous change by changing DU_3DBV63 to DU_5DBV634 in the
**	    relevant places.
**	07-may-91 (teresa)
**	    Fix bug 37439 by using DU_UPGRADING bitflag instead of DU_CONVERTING
**	    which allows upgradedb to distinguish a db marked for upgrade from
**	    an unconverted v5 db.
**	22-jul-91 (teresa)
**	    Extracted messages to erduf.msg and made several modifications to
**	    meet coding standards.  Also did major restructuring due to some
**	    architectual bugs.  Changed the message/logging mechanism to be
**	    more compatible with the rest of DUF.
**	16-apr-92 (teresa)
**	    Pick up ingres63p change 265521:
**		2-apr-92 (seg)
**		Needed different constant for "upgradedb", so replaced all 
**		literals (ERX("upgradedb")) with a variable reference and
**		ifdef'ed the variable's initialization.
**	05-oct-92 (jpk)
**		Remove references to MSB of relstat, cv_relstat() routine.
**	08-oct-92 (robf)
**	     Add support for new C2-SXA gateway catalogs (iigw06_relation,
**	     iigw06_attribute, iiaudittables) plus the new iidbdb catalog 
**           iiaudit.
**      20-nov-92 (bonobo)
**          Updated MING hint to include CUFLIB.
**	7-Jan-1993 (daveb)
**	    forward declare open_target().  HP compiler fataled bool not being int.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**		Include <lg.h> to pick up logging system typedefs
**	8-aug-93 (ed)
**		unnest dbms.h
**	26-oct-93 (robf)
**	   DU_USUPER --> DU_USECURITY
**	16-nov-93 (rblumer)
**	    added value for new 'upper_case' argument to duc_iischema function
**	    call in new_for_65_syscats.
**	    This argument is FALSE for 6.4 databases, since only lower-case
**	    databases were allowed in 6.4.
**	20-nov-1993 (jpk)
**	    Populate iischema.  Drop and rebuild rules, procedures, 
**	    and integrities as well as views and permits.
**	13-oct-93 (swm)
**	    Bug #56448
**	    Removed d_print() function because it accepts a variable number
**	    of variable-sized arguments, which is not portable. It could
**	    not become a varargs function as this practice is outlawed in
**	    main line code.
**	    Replaced invocations of the function with calls to SIprintf. This
**	    works because SIprintf is a varargs function.
**	    Tidied up previous history comment.
**	29-Jan-1994 (jpk)
**	    Bug #58408.
**	    Made those SIprintfs depend on upg_dbg_out flag.
**	01-feb-1994 (wolf) 
**	    Move the #include cs.h ahead of #include lk.h so the compiler
**	    knows what a CS_SID is.
**	10-mar-94 (swm)
**	    Bug #60474
**	    Added missing control block argument in several invocations
**	    of du_talk function. Errors should now be logged correctly.
**	8-jul-94 (robf)
**          Updated for ES 1.1
**      10-nov-94 (chech02)
**          Bug #56476
**          deleted 'register iiprocedure as link' stmt in cv_star_cats().
**      03-feb-95 (chech02)
**          Bug# 66464, 66467, 64589, 64590
**            1. Use 'CREATE TABLE tmp_tble' to create tmporary tables, 
**               instead of ' DECLARE GLOBAL TEMPORARY TABLE'.
**            2. set session authorization to '$ingres' in cv_permits().  
**            3. add chk_create_workdir() to create $II_SYSTEM/ingres/work/
**               default/dbname if not exists.
**          
**      02-feb-1995 (sarjo01) Bug 65613
**          cv_initialize(): Removed du_chk1_dbname() call which gave error
**          if dbname was qualified with vnode::
**      17-feb-1995 (chech02) Bug 66923
**          Check journing first before doing order_critical_65es().
**      08-mar-1995 (chech02) Bug 67365
**          1. Add rm_iitmp_tables() to remove all iitmp_ tables.
**          2. If a super user, then add 'operator' and 'create location'
**             privileges to the super user. 
**      20-mar-1995 (canor01) Bug 67535
**          Connect as $ingres so back end knows this is an
**          administrative procedure
**      21-mar-1995 (chech02) Bug 67443
**          Not allow to upgrade from the non-sql92 installtion to the sql92
**          installtion.
**      27-mar-1995 (chech02) 67532
**          Cant access views in stardb. Added cv_user_star_iiviews() to
**          populate user views in stardb.
**      29-mar-1995 (chech02) 67789
**          In Release 6.4 create permit insert was allowed on aggregate views
**          that is not permitted in 1.1. Modified exec_txt() to test for sql 
**          return code of -34000 when attempting to create a permit.
**      26-apr-1995 (wolf) 68397
**          Unix pathname used in chk_create_workdir() causes a/v on VMS.
**      24-may-1995 (lawst01) 68770
**          Initialize 'dbp_create_date' field for a procedure - when 
**          converting from 6.4 database. Field currently contains 0
**          will use upgrade date in field.
**      2-jun-1995 (chech02) 68695
**          Close 'iidbdb' session before call to upgradefe on the star
**          database.
**      2-jun-1995 (chech02) 68696
**          Even the current level of the database is 11/03, calls
**          upgradefe to display the status of front end catalogs.
**      2-jun-1995 (lawst01) 69067
**          Change permissions on PEworld function call to "+r+w+x" from "+r+w"
**      5-jul-1995 (chech02) 65613
**          Allowed upgradedb to process dbnames in 'vnode::dbname' format.
**      5-sep-1995 (chech02)
**          1. Before forking to upgradedb, database shall be closed and 
**             opened db without exclusive lock. 
**          2. Used LOingpath() to create work sub dirtory.                   
**      21-sep-1995 (chech02) b71412
**          Since the system table iistatistics has been changed, we just 
**          need to delete all rows from iistatistics to fix this bug.
**      25-sep-1995 (chech02) b71608
**          Added duc_iierror() to create internal db procedure iierror.
**      03-nov-1995 (lawst01) b71777
**          Added 'grant_select_64_11' to 'grant select on table to public'
**          if the TCB_PRTUPS (0x00000004) flag is set - tuple exists in
**          iiprotect table and the TCB_PROTECT (0x00002000) flag is not
**          set. In 64 for a grant select on table-name to public no entry
**          was stored in the ii protect table, so it is necessary to
**          issue the grant select ... after the database is upgraded.
**	09-nov-1995 (canor01)
**	    Initial upgrade from V65ES_LEVEL to V12_LEVEL, which currently
**	    consists of upgrading the iitables view.
**	10-nov-95 (nick)
**	    Call duc_modify_iihistogram() rather than use QUEL explicitly
**	    in order_critical_65().
**	13-nov-95 (prida01)  71292
**	    On a star database upgradedb doesn't upgrade iistats and iidd_stats
**	    properly.
**	5-dec-95 (stephenb)
**	    open_target() may fail in such a way as to leave us switched
**	    into a bad connection (notably when the target database is 
**	    inconsistent), this causes problems upgrading subsequent databases.
**	    Makse sure we switch back to the iidbdb connection before
**	    continuing
**	05-Jan-96 (nive)
**	    In function cv_65_iiextend, added code to
**	    update iiextend to pad lname and dname upto 32 length 
**	    while upgrading dbs from R6. Also, pad lname ii_work upto
**	    32 length during its insertion into iiextend. (Bug 71480)
**	21-Feb-96 (prida01)
**	    When upgrading a database of form vnode::database the vnode
**	    is needed for connecting to the database, but causes problems
**	    when checking database directories exist etc. Remove it when
**	    not needed.
**      06-mar-1996 (nanpr01)
**          Added upgrade code for 2.0 upgrade - Variable Page Size Project. 
**	 4-apr-96 (nick)
**	    Upgrade iimulti_locations.
**	31-may-96 (prida01)
**	    Modify vnode change to work for local databases bug 76245
**      31-May-96 (fanra01)
**          Changed collect_dbs to return a success or failure status to affect
**          the upgr_exit.
**	13-jun-1996 (nick)
**	    LINT directed changes.
**	22-july-1996 (angusm)
**		Intersolv ODBC driver support changes - add calls to add
**		catalogs 'iiproc_params','iiproc_access', and ensure that
**		'iiaccess' is rebuilt.
**	22-Aug-96 (hanch04)
**		Remove comments around prototype add_dbname.
**      28-sep-96 (mcgem01)
**              Global data moved to duvdata.c
**      16-Oct-1996 (hanch04)
**          Added relversion, reltotwid for 2.0
**	    Modifed iiattribute to have more room for new columns.
**		1.  Insert new attributes into iiattribute table.
**		2.  Disconnect and reconnect to flush caches.
**		3.  Modify iiattribute to hash, 64 bytes read in,
**			96 written out.  
**		4.  Drop iicolumns view and recreate.
**		5.  Update fields in iirelation.
**		6.  Update new fields in iiattribute to defaults.
**	28-jan-97 (inkdo01)
**	    Added cv_20_hashdate_tables to normalize hashed date columns
**	    following bug fix for 74033.
**	09-may-96 (mcgem01)
**	    Change the minor release id to 1 so that we now detect the need
**	    to uggrade 2.0 beta databases, to include reltcpri the
**	    (table cache priority).  Add table_pagesize, table_relversion, 
**	    table_reltotwid and reltcpri to iitables.
**	17-aug-1997 (hanch04)
**	    Connect to iidbdb with +w flag to wait for lock.
**	22-aug-97 (kinpa04)
**	    Change level check from 'level' to 'upgd_cb.Dbname_ptr->level'
**	    while handling databases since 'level' is set to that of
**	    the last database in query list.
**	22-aug-1997 (nanpr01)
**	    FIPS upgrade fails with  E_DU6019 CANT_UPGRADESQL92 message.
**	    bug 84356 & 74447.
**	04-feb-1998 (hanch04)
**	    Change the minor release id to 0, added DU_6DBV85
**	06-Mar-1998 (kosma01)
**	    AIX 4.x loses $ingres authorization when before converting
**	    6.4 Ingres iistatistics. Reauthorize in cv_65_iistatistics
**      13-mar-98 (nicph02)
**              Added cleanup routine rm_iiqef_procedures to get rid of
**              temp iiQEF_ procedures as the upgradedb terminates
**              (bug 89164)
**	02-apr-1998 (i4jo01)
**	    Crash when creating internal procedure iierror.
**	    Needed parameter when calling duc_iierror. (b90225)
**      03-Aug-1998 (wanya01)
**          Add a new flag -c for concurrent upgradedb.
**          If -c is specified, we will connect to iidbdb without taking
**          exclusive lock on it.
**	30-oct-1998 (kitch01)
**		Bug 92451. Add 0 days instead of 0 hours. This will still allow
**		the date to be normalized but without appending 00:00:00 to
**		date only date values. Function cv_20_hashdate_tables().
**	09-nov-1998 (mcgem01)
**	    Changed table version to "II2.5"
**      21-apr-1999 (hanch04)
**          replace STindex with STchr
**      28-apr-1999 (hanch04)
**          Replaced STlcopy with STncpy
**	12-may-99 (stephenb)
**	    Add function changed_cats_25() for 2.5 catalog changes
**	21-may-1999 (hanch04)
**	    Replace STbcompare with STcasecmp,STncasecmp,STcmp,STncmp
**      27-may-99 (hanch04)
**	    Add function chmod_data_600().
**      03-aug-1999 (stial01)
**          create/modify iiextended_relation if it doesn't exist
**          Call iiQEF_convert_table for all tables having (blob) extensions
**      01-jun-1999 (kitch01)
**	   As part of the fix for 97040, recreate the iilog_help view during
**	   an upgrade.
**      08-jul-99 (chash01) rms gateway code merge, set dbmstype to DU_DBRMS 
**           if test says RMS.
**        12-dec-97 (chash01)
**          for rms gateway, converting views and imported table causes
**          error, they must be excluded.
**       1-oct-1999 (mosjo01) SIR 97564
**          Reposition ex.h after cs.h to avoid compiler error on _jmpbuf.
**       5-feb-2001 (horda03) Bug 103873.
**          du_error() removes the '\t' following the Ingres error code in the
**          returned error message, if the message is not a WARNING nor an
**          INFORMATIONAL. If the message stills contaiNs '\t', then the first
**          part of the message will be lost. Similarly, if the message does
**          not contain a '\t' the Ingres error code will be displayed.
**      19-jan-2000 (stial01)
**          Call iiQEF_convert_table for btree/rtree tables (so the dbms can 
**          validate keys per page calculations, fixed error handling.
**	31-jan-2000 (hanch04)
**	    Call new_for_65_syscats_iidbdb before changed_for_65_syscats_iidbdb
**      19-May-2000 (hweho01)
**          Reposition cs.h before ex.h avoids compiler errors with __jmpbuf
**          on AIX 4.3 platform.
**	29-aug-2000 (somsa01)
**	    In changed_cats_25(), changed c3 cursor to 'c4', as c3 is
**	    already in use in this file.
**	3-nov-00 (inkdo01)
**	    Minor changes to iistats, iihistograms stdcat views require
**	    drop/recreate (for composite histograms).
**	09-nov-2000 (somsa01)
**	    In cv_65_iirule(), check to make sure the iiruleidx and
**	    iiruleidx1 indices exist.
**	17-nov-2000 (somsa01)
**	    Modified the last change such that the iirule indices are
**	    created for all databases less than 2.5 .
**      02-Mar-2001 (hanch04)
**          Bumped release id to external "II 2.6" internal 860 or 8.6
**	02-Mar-2001 (jenjo02)
**	    Added changed_for_26_iidbdb_cats(), cv_26_iilocations().
**	02-Apr-2001 (jenjo02)
**	    Added cv_26_iiextend().
**      16-apr-2001 (stial01)
**          Added changed_for_26_stdcats
**      27-apr-2001 (stial01)
**          Added new_for_26_dbprocs to drop / re-create iiqef_create_db
**          because of new ucollation parameter for unicode databases
**          Moved call to grant_select_64_11 (it was causing errors)
**	11-May-2001 (jenjo02)
**	    rawblocks moved from iilocations to iiextend, rawpct added
**	    to iilocations.
**      16-may-2001 (stial01)
**          If upgradedb iidbdb, upgrade imadb and icedb as well (B104731)
**	23-May-2001 (jenjo02)
**	    Added installation of iiQEF_check_loc_area procedure
**	    to new_for_26_dbprocs().
**	1-aug-2001 (stephenb)
**	    Fix reference to STncpy(), it causes a fatal compile error
**	    on i64_aix.
**      03-oct-2001 (stial01)
**          Changes for new 2.6 iiextend columns (rawstart, rawblocks) (b105956)
**      04-oct-2001 (stial01)
**          grant_select_64_11() grant select with grant option (b78332)
**	15-Oct-2001 (jenjo02)
**	    Deleted short-lived iiQEF_check_loc_area. Area directories
**	    are now made by CREATE/ALTER LOCATION.
**      09-jan-2002 (stial01)
**          Using unloaddb to fix query trees, new -tree option (SIR 106745)
**      13-mar-2002 (stial01)
**          Remove references to location structure members
**      27-mar-2002 (stial01)
**          cv_database() Disable error check after drop procedure iiqef_create_db,
**          fix_qry_trees() make sure filename < DI_FILENAME_MAX
**          cv_iidbdb() move set_cmptlv before add_dbname
**      08-may-2002 (stial01)
**          fix_dbservice() check dbmsinfo('lp64') to see if SERVER is lp64
**      09-may-2002 (stial01)
**          fix_dbservice() call ii_update_config with newdbservice flags
**	26-jun-2002 (hanch04)
**	    Make copies of some system catalogs so that the saved rows
**	    can be inserted into the updated tables.
**	    Re-ordered some steps to allow a 6.4 database to upgrade.
**	    Modify the query for checking that fix_qry_trees worked.
**	01-Oct-2002 (hanje04)
**	    BUG 108823
**	    Include duu.h so that the duu functions are correctly prototyped.
**	    Having a default return type of i4 was causing problems on 64bit
**	    Linux (and potentially others) where there was no prototype and 
**	    the function was returning a char *.
**      19-dec-2002 (stial01)
**          fix_qry_trees() recreate internal procedures before executing
**          unloaddb script - we need may reference iierror.
**          Also when validating procedure count, don't count system procs.
**      07-jan-2003 (stial01)
**          Added fix_ref_constraints()
**      08-jan-2003 (stial01)
**          fix_ref_constraints: need to make sure we process only referential
**          constraints - as check constraints may have rules which reference
**          iierror - which has not been redefined yet
**      09-jan-2003 (stial01)
**          renamed fix_ref_constraints -> fix_sysgen_rules, handles all system
**          generated rules (ref and check constraints)
**      12-feb-2003 (stial01)
**          fix_qry_tree() clean up validation of view,integ,proc counts.
**          Strip include iiud.scr from copy.in (b109623)
**      14-feb-2003 (stial01)
**          fixed iidbdb connect options [3a], added -l so we lock database,
**          so we don't exceed locks per_tx_limit  (b109644)
**      07-Apr-2003 (hanch04)
**          Bumped release id to external "II 2.65" internal 865 or 8.65
**	11-apr-2003 (devjo01)
**	    - b109489: Correct problem where directory paths for locations
**	      were being truncated at 32 chars. (Root cause was a side effect
**	      of ancient unresolved b54123!)
**	    - b110050: Capture and surpress "unloaddb" output so user does
**	      not instructions for unloading and reloading his database,
**	      which are misleading in this context.
**	    - Cleanup compiler warnings. (add forward declare static func
**	      fix_dbservice, fix missing return value in non-void func.)
**	10-Jul-2003 (schka70)
**	    "chmod 0600 *" is no good for db's with lots of tables.
**	    Use xargs instead.  Also, translate locations names properly.
**      24-jul-2003 (stial01)
**          Fixed disconnect timing problem causing unloaddb to fail (b110617)
**      21-aug-2003 (ashco01) 
**          Problem: INGSRV#2479, Bug: 110760
**          chmod_data_600() can add considerable run-time overhead when
**          upgrading several thousand databases. Added II_UPGRADEDB_CHMOD
**          environment variable to disable chmod during upgradedb, allowing
**          the customer to manually chmod the data files when convenient.
**	14-Nov-2003 (wanfr01)
**	    Bug 110915, INGCBT492
**	    Rules can not be directly recreated if they were part of a foreign
**	    key constraint against a table not owned by the constraint owner.
**	    Added code to drop/recreate the constraint instead.
**      02-dec-2003 (chash01)
**          bug111385, upgradedb fails on rms gateway database with E_DM011A.
**          Modify the dbname_ptr string to remove "/rms" when execute
**          ii_update_config(), and add it back afterwards. Change is in
**          cv_upgrade().
**      05-dec-2003 (chash01)
**          For bug 111385, I missed third arg for STindex().
**      13-Jan-2004 (sheco02)
**          Bumped release id to external "II 3.0" internal 900 or 9.0
**     28-jan-04 (chash01) bug 111713/INGDBA275. interestingly, some RMS
**         gateway registered table/index has relspec=11
**         (btree) thus we have to eliminate them for update. 
**         test relstat for TCB_GATEWAY bit set, select those
**         that do not have this bit set.
**     09-feb-04 (chash01) bug 111713/INGDBA275 actually is the result of
**         not knowing TIDP is part of key (by Ingres definition). Change
**         in gwrms.c will fix this bug.  The code change indicated in
**         last comment is unneccessary and is now removed.  The change in
**         1997 that excludes the RMS gateway table from updating
**         is now taken care of in dmu_convert() and is no longer needed.  
**         It is removed also. 
**	11-mar-2004 (somsa01)
**	   When upgrading iidbdb, upgrade icetutor and icesvr as well. Also,
**	    split convert part of collect_dbs() into convert_dbs() to allow
**	    for multiple calls to collect_dbs() for adding multiple system
**	    databases. Also, removed some unused variables. Also, in
**	    set_cmptlvl(), Added use of DU_6DBV865 (Release 3).
**	26-Aug-2004 (schka24)
**	    Implement r3 upgrading, changes too numerous to list.
**	    Various bug fixes: large-page catalog tables, cross-user ref
**	    constraints with quel permits, 6.4 grant detritus, minor things.
**	27-oct-2004 (somsa01)
**	    Corrected CMDLINE setting for Windows.
**	27-oct-2004 (somsa01)
**	    Updated runs of unloaddb to be executed via PCcmdline(), so
**	    that we have better control of "-u" passing.
**	30-Dec-2004 (schka24)
**	    Changes for 3.0.2: attcollid, remove iixprotect.
**	    Also, rearrange functionality so that upgradedb itself doesn't
**	    have to run set-user-id to ingres.  Don't try to convert iidbdb
**	    if we're not ingres, create logs in an open directory, etc.
**	13-jul-2005 (abbjo03)
**	    Back out change of 27-oct-2004 as it causes errors when unloaddb
**	    is run on VMS.    
**      17-Jan-2005 (hweho01)
**          Bumped release level 9.02 to 9.04.
**      27-feb-2006 (stial01)
**          Add +w to sql CMDLINE so sql waits for upgradedb disconnect, b115750
**      02-mar-2006 (stial01)
**          fix_qry_trees() Make sure errors get reported to upgrade.log
**      27-Apr-2006 (hweho01)
**          Changed release level to 9.10.
**      20-oct-2006 (stial01)
**          Database level should not advance for ingres2006r2
*/
/*
MODE =          SETUID
NOODLIBS =	DBUTILLIB SQLCALIB LIBQLIB FMTLIB AFELIB ADFLIB \
			LIBQGCALIB CUFLIB GCFLIB COMPATLIB MALLOCLIB
NEEDLIBS =      DBUTILLIB SQLCALIB GNLIB UFLIB RUNSYSLIB RUNTIMELIB \
		FDLIB FTLIB FEDSLIB \
                LIBQSYSLIB LIBQLIB UGLIB FMTLIB AFELIB \
                LIBQGCALIB CUFLIB GCFLIB ADFLIB \
                COMPATLIB MALLOCLIB
OWNER =         INGUSER
MODE =		SETUID
PROGRAM =       upgradedb
*/
/*
** global variables 
*/
GLOBALREF DU_UPGRADE_CB	upgr_cb;	/* upgradedb control block */
static  char vnode[DB_MAXNAME+1];
/* externally referenced routines */
FUNC_EXTERN	DU_STATUS	dui_bldDefClientList();
FUNC_EXTERN	DU_STATUS	du_fclose();
FUNC_EXTERN	VOID		du_get_usrname();
FUNC_EXTERN     DU_STATUS	du_log();
FUNC_EXTERN     VOID		du_talk();
FUNC_EXTERN     VOID		upgr_exit();
FUNC_EXTERN	STATUS		setauth(char *owner);
FUNC_EXTERN	VOID		duc_iiprotect();
FUNC_EXTERN	VOID		duc_iiuser();
FUNC_EXTERN	VOID		duc_iidbpriv();
FUNC_EXTERN	VOID		duc_iistatistics();
FUNC_EXTERN	VOID		duc_modify_iidd_stats();
FUNC_EXTERN	VOID		duc_modify_iistatistics();
FUNC_EXTERN	VOID		duc_iihistogram();
FUNC_EXTERN	VOID		duc_modify_iihistogram();
FUNC_EXTERN	VOID		duc_iidd_stats();
FUNC_EXTERN	VOID		duc_iidevices();
FUNC_EXTERN	VOID		duc_modify_iidevices();
FUNC_EXTERN	VOID		duc_iiprocedure();
FUNC_EXTERN	VOID		duc_modify_iiprocedure();
FUNC_EXTERN	VOID		duc_iiextend();
FUNC_EXTERN	VOID		duc_modify_iiextend();
FUNC_EXTERN	VOID		duc_iievent();
FUNC_EXTERN	VOID		duc_iirange();
FUNC_EXTERN	VOID		duc_modify_iirange();
FUNC_EXTERN	VOID		duc_iietab();
FUNC_EXTERN	VOID		duc_modify_iietab();
FUNC_EXTERN	VOID		duc_iidatabase();
FUNC_EXTERN	VOID		duc_modify_iidatabase();
FUNC_EXTERN	VOID		duc_iilocations();
FUNC_EXTERN	VOID		duc_modify_iilocations();
FUNC_EXTERN	VOID		duc_iirole();
FUNC_EXTERN	VOID		duc_modify_iirole();
FUNC_EXTERN	VOID		duc_iiusergroup();
FUNC_EXTERN	VOID		duc_modify_iiusergroup();
FUNC_EXTERN	VOID		duc_modify_iievent();
FUNC_EXTERN	VOID		duc_m1_dbmscomment();
FUNC_EXTERN	VOID		duc_m2_synonym();
FUNC_EXTERN	VOID		duc_m3_sectype();
FUNC_EXTERN	VOID		duc_m4_protect();
FUNC_EXTERN	VOID		duc_m5_user();
FUNC_EXTERN	VOID		duc_m6_dbpriv();
FUNC_EXTERN     VOID            duc_iidbdepends();
FUNC_EXTERN     VOID            duc_modify_iidbdepends();
FUNC_EXTERN	bool		rel_exists(char *rel_name);
FUNC_EXTERN	bool		tbl_lacks_col(char *tname, char *cname);
FUNC_EXTERN	STATUS		ensure_not_exists(char *name);
FUNC_EXTERN	DU_STATUS	duc_iigwsxa(DU_ERROR *);
FUNC_EXTERN	VOID     	duc_iidd_tables();
FUNC_EXTERN	VOID     	duc_modify_iidd_tables();
FUNC_EXTERN	VOID     	duc_iidd_indexes();
FUNC_EXTERN	VOID     	duc_modify_iidd_indexes();
FUNC_EXTERN	VOID     	duc_iidd_physical_tables();
FUNC_EXTERN VOID		duc_iilog_help();
FUNC_EXTERN	DU_STATUS	duc_create_specific_stdcat_view(
			char *dbname, char *viewname, bool upcase);
FUNC_EXTERN	STATUS		duc_drop_stdcat_views(char *dbname, bool star_flag);
FUNC_EXTERN     STATUS          duc_grant_stdcat_views(char *dbname, bool star_flag);
FUNC_EXTERN	DU_STATUS	duc_modify_pagesize(i4 pagesize,
					bool star_flag, DU_ERROR *errcb);
GLOBALREF	bool		upg_dbg_out;
static          bool            upg_concurrent;
static		bool		upg_trees;
static		bool		upgrade_iidd_stats;
GLOBALREF	DUU_CATDEF	*Dus_allcat_defs[];
static		STATUS		fix_sysgen_rules();
/*
[@static_variable_or_function_definitions@]
*/
static char *upgradedb_name = 
#ifdef OS2
	ERx("upgraded");
#else
	ERx("upgradedb");
#endif
static VOID	append_clientlist();
static VOID	add_dbname(); 
static VOID	collect_dbs();
static STATUS	convert_dbs();
static VOID	cv_database();
static VOID	cv_dbaccess();
static VOID	cv_dbcap();
static VOID	cv_dbpriv();
static VOID	cv_event();
static VOID	cv_group();
static VOID	cv_iidbdb();
static VOID	cv_iistar_cdbinfo(bool upcase);
static VOID	cv_initialize();
static VOID	cv_pre64_integrity();
static VOID	cv_online();
static VOID	cv_protect();
static VOID	cv_rule();
static VOID	cv_pre64_role();
static VOID	cv_star_cats();
static VOID	cv_pre64_user();
static VOID	cv_upgrade();
static VOID	du_create_logdir(DU_FILE *fp, char *logdir, char *subdir);
static VOID	du_fopen(DU_FILE *fp, char *filename);
static VOID	fe_dictinst();
static VOID	order_critical_65(char *dbname, DBNAME_NODE *db_star_name, 
				  bool upcase);
static VOID	new_for_65_syscats(char *dname);
static VOID     new_for_65es_syscats_iidbdb(char *dbname);
static VOID	new_for_65es_syscats(char *dname);
static VOID	new_for_65es_stdcats(char *dname);
static VOID	changed_for_65_syscats(char *dname);
static VOID	changed_for_65_syscats_iidbdb(char *dbname);
static VOID	changed_for_65es_syscats_iidbdb(char *dbname);
static VOID	starto64();
static VOID	star_2pc();
static VOID	update_iidatabase();
static DU_STATUS validate_db();
static VOID	star_std_cats();
static bool	open_target(i4 open_mode);
#define CONNECT_EXCL	1
#define CONNECT_SHARE	0
static bool	needs_widening(char *tname, char *cname);
static VOID	make_copy_of(char *dname, char *tname);
static DU_STATUS copy_existing_columns(char *dbname, char *tblname);
static VOID	cv_65_iidd_stats(char *dname);
static VOID     cv_65_iistats(char *dname);
static VOID	cv_65_iistatistics(char *dname);
static VOID	cv_iiintegrity(char *dname);
static VOID	cv_iirule(char *dname);
static VOID	cv_65_iiextend(char *dbname);
static VOID	cv_65_iiusergroup(char *dbname);
static VOID	fix_qry_trees(char *dbname, char *conn_dbname, bool star_db, i4 level);
static VOID	fix_copy_in(LOCATION *in, LOCATION *out, bool fix_iiud);
static VOID	cv_65es_iirule(char *dname);
static VOID	cv_iievent(char *dbname, bool upcase);
static VOID	cv_iipriv(char *dbname);
static VOID	cv_iiprocedure(char *dbname);
static VOID	cv_iisequence(char *dbname);
static VOID 	cv_65es_iisecuritystate(char *dbname);
static VOID 	cv_65es_iisectype(char *dbname);
static VOID	cv_iiaudit(char *dbname);
static VOID 	cv_iidatabase(char *dbname);
static VOID 	cv_65es_iiuser(char *dbname);
static VOID	cv_r3_iiuser_prof(char *dbname, bool upcase);
static VOID 	cv_65es_iisectype(char *dbname);
static VOID 	cv_65es_iisecuritystate(char	*dbname);
static VOID 	cv_iilocations(char	*dbname);
static VOID 	cv_iirole(char	*dbname);
static VOID	cv_65es_iidbpriv(char *dbname);
static VOID 	cv_65es_iirolegrant(char *dbname);
static VOID 	order_critical_65es(char *dbname, bool upcase);
FUNC_EXTERN STATUS 	populate_secalarm(char *dbname);
FUNC_EXTERN VOID	populate_iischema();
       STATUS	handle_ex(EX_ARGS *ex_parms);
static VOID	order_critical_20(char *dbname, DBNAME_NODE *db_star_name,
				  bool upcase);
static VOID	order_critical_rehash(char *dbname, i4 level);
static VOID	order_critical_r3(DBNAME_NODE *info);
static VOID	order_critical_r302(DBNAME_NODE *info, char *name_if_iidbdb);
static VOID 	bootstrap_syscats(char *dbname, DBNAME_NODE *db_star_name, 
				     bool upcase);
static VOID 	star_20_std_cats(bool upcase);
static VOID	new_for_20_stdcats(char *dname, bool upcase);
static VOID 	cv_20_hashdate_tables(char    *dbname);
static VOID     rm_iiqef_procedures();
static VOID	changed_cats_25(char		*dbname,
				DBNAME_NODE	*db_star_name,
				bool		upcase);
static VOID	changed_for_r3_syscats(char *dbname, bool upcase);
static VOID	new_for_r3_syscats(char *dbname);
static VOID	changed_for_r3_syscats_iidbdb(char *dbname, bool upcase);
static VOID	chmod_data_600();
static VOID 	cv_26_iiextend(char	*dbname);
static STATUS	fix_dbservice(char     *dbname);
static VOID	fix_syscat_grants(char *dbname);
static bool	fix_64_grants(char *dbname,
				LOCATION *grant_fix_loc,
				char *grant_fix_loc_buf);
static bool	fix_constraints(char *dbname,
				i4 level,
				LOCATION *ref_fix_loc,
				char *ref_fix_loc_buf);
/*{
** Name: main	- Entry point for UPGRADEDB
** Description:
**
**	    main calls the proper functions for UPGRADEDB to startup,
**	    mark databases inoperative, check their cmptlvl's in iidbdb,
**	    convert them, and mark them operative again.
** Inputs:
**      argc		Command line argument count.
**	argv		Pointer to base of array of argument strings.
**
** Outputs:
**	none
**
**	Returns:
**	    OK		If completed successfully.
**	    FAIL	If error encountered or user terminated.
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      06-jun-89 (ralph)
**          Initial creation and coding.
**	06-jul-90 (ralph)
**	    Pass username and userstat parameters to various routines for
**	    verifying user permission to perform upgrade.
**	09-jan-91 (stevet)
**	    Added IIUGinit call to initialize CM attribute table.
**	08-apr-91 (bonobo)
**	    Added mysteriously absent MEadvise call.
**	22-jul-91 (teresa)
**	    Extracted messages to erduf.msg and made several modifications to
**	    meet coding standards. 
**	05-oct-92 (jpk)
**	    Added checking for CSinitiate() return status
**      31-May-96 (fanra01)
**          Initialise status to OK and added check of return from collect_dbs
**	13-sep-96 (nick)
**	    If we don't call EXsetclient() we won't catch user interrupts.
**	12-mar-2004 (somsa01)
**	    collect_dbs() is strictly used to add db names to the master
**	    list. convert_dbs() is now used to do the database conversions.
**	19-Apr-2007 (bonro01)
**	    Detect internal error in cv_initialize() and exit with Fail
**	    return code.
*/
main (argc, argv)
i4	argc;
char	*argv[];
{
    bool	dbdb_specified;
    char	username[DB_MAXNAME+1];
    i4	userstat; 
    DU_ERROR	errcb;
    STATUS	status = OK;
    (void)EXsetclient(EX_INGRES_TOOL);
    MEadvise(ME_INGRES_ALLOC);
    /* Call IIUGinit to initialize CM attribute table */
    if( IIUGinit() != OK)
    {
	upgr_exit(DU_FAIL_EXIT, 0);
    }
    /* setup error handling, open log file, and check arguments */
    cv_initialize(argc, argv, &errcb);
    /*
    ** declare an exception handler (note: we always PCexit in the handler
    ** so EXdeclare here does not check for a returned value)
    */
    EXdeclare(handle_ex, &upgr_cb.Context_ex);	    
    /* declare error handler for esqlc errors */
/* # line 862 "upgradedb.sc" */	/* host code */
    /* Check iidbdb -- convert it before any other databases.  This routine
    ** will leave the iidbdb open with system catalog update priv unless there
    ** was an error converting the iidbdb */
    dbdb_specified = (STcompare(upgr_cb.du_dbname, DU_DBDBNAME) == 0);
    cv_iidbdb(dbdb_specified, username, &userstat);
    if (upgr_cb.Iidbdbok == FALSE)
	upgr_exit(DU_FAIL_EXIT, W_DU1850_IIDBDB_ERR);
    /* Process any databases the user specified (other than iidbdb) */
    /* Convert target databases, unless just iidbdb was specified */
    du_talk (DU_MODESENS, &upgr_cb, I_DU008C_COLLECT_DBS, 0);
    if (!dbdb_specified)
	collect_dbs(upgr_cb.du_dbname, username, userstat);
    else
    {
	if (upgr_cb.imadb_cvt)
	    collect_dbs("imadb", username, userstat);
	if (upgr_cb.icedb_cvt)
	    collect_dbs("icedb", username, userstat);
	if (upgr_cb.icesvr_cvt)
	    collect_dbs("icesvr", username, userstat);
	if (upgr_cb.icetutor_cvt)
	    collect_dbs("icetutor", username, userstat);
    }
    status = convert_dbs();
    if (status == OK)
    {
        upgr_exit (DU_OK_EXIT, I_DU0081_UPGRADEDB_DONE);
    }
    else
    {
        upgr_exit (DU_FAIL_EXIT, 0);
    }
}
/*{
** Name: set_cmptlvl	- Sets internal compat level from dbcmptlvl/dbcmptminor
**
** Description:
**	    Sets an internal compatibility level from values for
**	    dbcmptlvl and dbcmptminor passed in.
**
** Inputs:
**	    cmptlvl		the value for dbcmptlvl
**	    minorlvl		the value for dbcmptminor
**	    
** Outputs:
**	    none
**	    
**	Returns:
**	    level	    the internal compat level:
**				60 - cmptvlv is "6.0"
**				61 - cmptvlv is "6.1a", minorlvl = 3
**				62 - cmptvlv is "6.2",  minorlvl = 0
**				63 - cmptvlv is "6.3",  minorlvl = 0
**				64 - cmptvlv is "6.34", minorlvl = 0
**				65 - cmptvlv is "7.0",  minorlvl = 0
**				652- cmptvlv is "7.0",  minorlvl = 2 (1.1 ES)
**				653- cmptvlv is "7.0",  minorlvl = 3 (1.2)
**				 0 is returned for all others
**	    
**	Side Effects:
**	    none
**	    
** History:
**	07-jun-81 (ralph)
**	    Initial creation
**	22-sep-89 (ralph)
**	    Added return of 63.
**	1-feb-1991 (bryanp)
**	    Added DU_5DBV634, returning 64
**      10-may-91 (markm)
**          Trim white space from end of compat level string
**	22-jul-91 (teresa)
**	    Extracted messages to erduf.msg
**	8-jul-94 (robf)
**          Added  support for ES 1.1, major 7.0, minor 2
**      06-mar-1996 (nanpr01)
**          Added  support for Variable Page Size Project updates.
**	    major 8.0, minor 0. Changes are in iirelation catalog,
**	    and std system catalog iitables, iiindexes and iiphysical_tables.
**	12-mar-2004 (somsa01)
**	    Added major 9.0 (Release 3), minor 0.
*/
i4
set_cmptlvl(cmptlvl, minorlvl)
char	*cmptlvl;
i4	minorlvl;
{
	i4	level;
	i4     len;
    /* get rid of trailing blanks - otherwise comparison 
    ** returns 0
    */
    if (upg_dbg_out)
    {
	SIprintf("]==> in set_cmptlvl(%s, %d)\n", cmptlvl, minorlvl);
    }
    len = STtrmwhite(cmptlvl);
    if (upgr_cb.upgtrace[UPG05_COMPATLVL])
    {
	upgr_cb.this_trace = UPG05_COMPATLVL;
	/* simulate the db having gone away */
	du_talk (DU_MODESENS, &upgr_cb, I_DU00B5_TRACE_POINT, 2, 
		 sizeof(upgr_cb.this_trace), &upgr_cb.this_trace);
	du_talk(DU_MODESENS, &upgr_cb, I_DU0083_COMPAT_INFO,  4,
		0, cmptlvl, sizeof(len), &len);
    }
    if (STcompare(cmptlvl, DU_0DBV60) == 0)
	level = V60_LEVEL;
    else if ((STcompare(cmptlvl, DU_1DBV61A) == 0) &&
	(minorlvl == DU_3DBMINOR))
	level = V61_LEVEL;
    else if ((STcompare(cmptlvl, DU_2DVV62) == 0) &&
	     (minorlvl == DU_0DBMINOR))
	level = V62_LEVEL;
    else if ((STcompare(cmptlvl, DU_3DBV63) == 0) &&
	     (minorlvl == DU_0DBMINOR))
	level = V63_LEVEL;
    else if ((STcompare(cmptlvl, DU_5DBV634) == 0) &&
	     (minorlvl == DU_0DBMINOR))
	level = V64_LEVEL;
    else if ((STcompare(cmptlvl, DU_4DBV70) == 0) &&
	     (minorlvl == DU_0DBMINOR))
	level = V65_LEVEL;
    else if ((STcompare(cmptlvl, DU_4DBV70) == 0) &&
	     (minorlvl == DU_2DBMINOR))
	level = V65ES_LEVEL;
    else if ((STcompare(cmptlvl, DU_4DBV70) == 0) &&
	     (minorlvl == DU_3DBMINOR))
	level = V12_LEVEL;
    else if ((STcompare(cmptlvl, DU_6DBV80) == 0) &&
	     (minorlvl == DU_0DBMINOR))
	level = V20_LEVEL;
    else if ((STcompare(cmptlvl, DU_6DBV80) == 0) &&
	     (minorlvl == DU_1DBMINOR))
	level = V20TCP_LEVEL;
    else if ((STcompare(cmptlvl, DU_6DBV85) == 0) &&
	     (minorlvl == DU_0DBMINOR))
	level = V25_LEVEL;
    else if ((STcompare(cmptlvl, DU_6DBV86) == 0) &&
	     (minorlvl == DU_0DBMINOR))
	level = V26_LEVEL;
    else if ((STcompare(cmptlvl, DU_6DBV9) == 0) &&
	     (minorlvl == DU_0DBMINOR))
	level = V30_LEVEL;
    else if ((STcompare(cmptlvl, DU_6DBV902) == 0) &&
	     (minorlvl == DU_0DBMINOR))
	level = V302_LEVEL;
    else if ((STcompare(cmptlvl, DU_6DBV904) == 0) &&
	     (minorlvl == DU_0DBMINOR))
	level = V904_LEVEL;
    else
	level = UNKNOWN_LEVEL;
    if (upgr_cb.upgtrace[UPG05_COMPATLVL])
	du_talk(DU_MODESENS, &upgr_cb, I_DU0084_COMPAT_INFO, 2,
		sizeof(level), &level);
    if (upg_dbg_out)
    {
	SIprintf("]==> we are at level %d\n", level);
    }
    return (level);
} 
/*{
** Name: cv_initialize	- UPGRADEDB initialization procedures
**
** Description:
**	    This routine sets up UPGRADEDB's runtime environment by opening a
**	    log file, declaring an SQL error handler, and checking for the
**	    proper command line arguments and setting any upgradedb trace
**	    points (specified via II_UPGRADEDB_TRACE)
**	    
** Inputs:
**	nargs		    number of command line arguments
**	argv		    array of command line arguments
**
** Outputs:
**	    none
**	
**	Returns:
**	    nothing
**
** Side Effects:
**	Handler is declared.
**
** History:
**	06-aug-91 (teresa)
**	    Rewrote from cv_startup. Changed to recognize some command flags,
**	    extracted messages to erduf.msg and made several modifications to
**	    meet coding standards.  Also changed initialization of log file
**	    to use a DUF compatible mechanism and added -nolog logic.
[@history_template@]...
**	24-jan-1994 (gmanning)
**	    Change register to register i4  for NT environment.
**      02-feb-1995 (sarjo01) Bug 65613
**          Removed du_chk1_dbname() call which gave error if dbname
**          was qualified with vnode::
**      08-jul-1999 (chash01)
**          set dbms type to DU_DBRMS if tested as RMS.
**	10-Sep-2004 (schka24)
**	    Init catalog-modify data in case of large-page-catalog database.
*/
static VOID
cv_initialize(nargs, argv, errcb)
i4	    nargs;
char	    *argv[];
DU_ERROR    *errcb;
{
    i4	    handle_equelerr();
    bool	    open_log_file=TRUE;
    char	    datestr[TM_SIZE_STAMP+1];
    char	    *sep = "*************************************************";
    i4	    length;
    register i4     i;
    char	    *p;
    char	    *tmp;
    static i4	    dbname_arg = 0;
    static i4	    cdbname_arg = 0;
    char	    *dbms="";
    char	    *tail_str;
    char            *colon_ptr;
    i4              num_clients = 0;	/* nmbr dict client names on cmd line */
    bool	    client_flag = FALSE; /* true if -f flag given */
    MEadvise(ME_INGRES_ALLOC);	    /* CL will allocate memory */
    upgr_cb.Iidbdbok = TRUE;
    upgr_cb.du_msg = errcb;
    /* Get the process's user name */
    du_get_usrname(upgr_cb.duowner);
    if (du_chk2_usrname(upgr_cb.duowner) != OK)
    {
	du_talk(DU_ALWAYS, &upgr_cb, E_DU3000_BAD_USRNAME, 2,
		0, upgr_cb.duowner);
	upgr_exit (DU_FAIL_EXIT, I_DU0080_UPGRADEDB_TERM);
    }
    /* initialize the log file name to the default.  This may be reset
    ** if the undocumented -l flag is specified */
    STcopy(ERx("upgrade.log"),upgr_cb.du_logname);
    /* scan input parameters to see if -nolog (or -n) is specified.  If so,
    ** then skip opening upgradedb log file.  Also check to see if -l is
    ** specified to rename log file */
    for (i= 1; (nargs - i); i++)
    {
	if (argv[i][0] == '-')	/* see if its a flag --if so, which? */
	{   
	    CVlower(argv[i]);	/* force to lower case */
	    if (argv[i][1]=='n')	/* check for nolog */
		open_log_file=FALSE;
	    if (argv[i][1]=='l')
	    {
	        /* this is an undocumented flag.  Format is -lNAME where
		** NAME is an alternate log file filename, replacing the
		** default of "upgrade.log".  This will be ignored if
		** -n is also specified.
		*/
		STcopy(&argv[i][2], upgr_cb.du_logname);
	    }
	}
    }
    /* We may need the log directory regardless, as a work area */
    du_create_logdir(&upgr_cb.du_log, upgradedb_name, upgr_cb.duowner);
    if (open_log_file)
    {
	/* -n or -nolog were not specified, so open upgradedb log file
	** as:  ii_config:[<login_name>]upgrade.log
	*/
	du_fopen(&upgr_cb.du_log, upgr_cb.du_logname );
	/* put id and timestamp into log */
	TMcvtsecs ( TMsecs(), datestr);		/* get datestring */
	length = STnlength ( sizeof(datestr), datestr);
	datestr[length] = '\0';	/* null terminate */
	du_log( upgr_cb.du_skiplog, &upgr_cb.du_log, sep);
	du_log( upgr_cb.du_skiplog, &upgr_cb.du_log, DU_UPGRADEDB);
	du_log( upgr_cb.du_skiplog, &upgr_cb.du_log, datestr);
	du_log( upgr_cb.du_skiplog, &upgr_cb.du_log, sep);
    }
    /* for now upgradedb does not do much of the fancy stuff that verifydb
    ** does.  So set up defaults.  Note that memory is only allocated for 1
    ** db name.  users may only specify a single database name at this time,
    ** or they may specify the -all flag
    */
    upgr_cb.du_scope = DU_SPECIFY;
    upgr_cb.du_mode  = DU_RUN;
    /* initialize debugging output to be disabled by default */
    upg_dbg_out = FALSE;
    /* Initialize concurrent upgrade to be disabled by default */
    upg_concurrent = FALSE;
    /* Initialize upgrade trees to be disabled by default */
    upg_trees = FALSE;
    /* now parse command line arguments */
    while (--nargs)
    {
	p   = *(++argv);
	switch (p[0])
	{
	  case '/':
	    CVlower(p);
	    if ( !STcompare( p, DU_SLASHD) ||  !STcompare( p, DU_STARTAIL))
	    {
		/* this is a distributed database */
		upgr_cb.du_dbmstype = DU_DBSTAR;
	    }
	    else if ( !STcompare( p, DU_RMSTAIL) )
	    {
		/* this is a RMS gateway database */
		upgr_cb.du_dbmstype = DU_DBRMS;
	    }
	    else
		upgr_cb.du_dbmstype = DU_DBINGRES;
	    break;
	  case '-':
	    switch (p[1])
	    {
	      case 'a':
		if(MEcmp((PTR)p, (PTR)"-all", (u_i2)4)==DU_IDENTICAL)
		    upgr_cb.du_scope = DU_INSTLTN;
	        break;
	      case 'u':
		if (du_chk2_usrname(&p[2]) != OK)
		{
		    du_talk (DU_ALWAYS, &upgr_cb, E_DU6000_UPGRADEDB_USAGE, 0);
		    upgr_exit (DU_FAIL_EXIT, I_DU0080_UPGRADEDB_TERM);
		}
		else
		{
		    STcopy(&p[2], upgr_cb.du_user);
		    break;
		}
	      case 'l':
	        /* this is an undocumented flag.  Format is -lNAME where
		** NAME is an alternate log file filename, replacing the
		** default of "upgrade.log".  This flag is handled in the
		** earlier pre-scan loop, so here all we do is break.
		*/
		break;
	      case 'd':
	        /* this is an undocumented flag; it enables (voluminous)
		** debugging output.
		*/
		upg_dbg_out = TRUE;
		break;
              case 'c':
                upg_concurrent = TRUE;
                break;
	      case 'n':
		open_log_file=FALSE;
		upgr_cb.du_skiplog = TRUE;
		break;
              case 'f':
		client_flag = TRUE;
                tmp = STskipblank(&p[2], (i4) DU_MAX_CLIENT_NAME);
                if ( (tmp != (char *) NULL) && (*tmp != EOS))
		    append_clientlist ( upgr_cb.du_feclients,
					&p[2],
					&upgr_cb.du_fecnt );
		/* multiple client names can appear following the -f
		** flag (e.g. -fingres visibuild windows_4gl).
		** Scan for more client names, or another flag,
		** or end of arguments.
		*/
		/* nargs & argv should be incremented together, so don't do
		** it as part of the conditional on nargs (e.g. "if (--nargs)").
		*/
		for (--nargs, p = *(++argv); nargs ; --nargs, p = *(++argv))
		{
		    if (p[0] == '-' || p[0] == '+' || p[0] == '/')
		    {
			/* uh oh, another flag. we're done with client list */
			break;
		    }
		    else
			append_clientlist ( upgr_cb.du_feclients,
					    p,
					    &upgr_cb.du_fecnt );
		}
		/* We're pointing at either last argument in list, which is
		** a client name, or at a flag that follows -f flag. Either way
		** we've gone one too far. Back up one argument to let outer
		** loop handle it.
		*/
		nargs++;
		argv--;
                break;
	      case 't':
		if(MEcmp((PTR)p, (PTR)"-tree", (u_i2)4)==DU_IDENTICAL)
		    upg_trees = TRUE;
	        break;
	      default:
		upgr_exit (DU_FAIL_EXIT, E_DU6000_UPGRADEDB_USAGE);
	    }	/* end of switch (p[1]) */
	    break;
	  default:
	    /* Must be a database name parameter -- must determine if it is
	    ** a Local DB (LDB) or Distributed DB (DDB) request or a gateway
	    */
	    if (dbname_arg != 0)
	    {
		    /* specified database name twice */
		    du_talk (DU_ALWAYS, &upgr_cb, E_DU6014_DBNAME_TWICE, 0);
		    upgr_exit (DU_FAIL_EXIT, E_DU6000_UPGRADEDB_USAGE);
	    }
	    dbname_arg++;
	    tail_str = STchr(p, '/');
	    if (tail_str)
	    {
		/* Something is prepended to name.  See if it is a valid 
		** distributed request flag --
		*/
		CVlower(tail_str);
		if ( !STcompare(tail_str, DU_SLASHD) || 
		     !STcompare(tail_str, DU_STARTAIL))
		{
		    upgr_cb.du_dbmstype = DU_DBSTAR;
		    *tail_str = EOS; /* take flag off of ddb name */
		}
		/* -- or an RMS Gateway database */
		else if (!STcompare(tail_str, DU_RMSTAIL))
		{
		    upgr_cb.du_dbmstype = DU_DBRMS;
		    *tail_str = EOS; /* take flag off of db name */
		}
		else
		{
		    du_talk (DU_ALWAYS, &upgr_cb, E_DU3010_BAD_DBNAME, 2, 0, p);
		    upgr_exit(DU_FAIL_EXIT, E_DU6000_UPGRADEDB_USAGE);
		}
	    }
            CVlower (p);
            STcopy(p, upgr_cb.du_dbname);
	    if (((colon_ptr = STchr(p, ':')) != NULL) &&
		STchr(colon_ptr + 1, ':') == (colon_ptr + 1))
            {
	      STncpy( vnode, p, colon_ptr - p);
	      vnode[colon_ptr-p] = '\0';
            }
	    else
	      vnode[0] = '\0';
	}   /* end of switch(p[0]) */
    }	/* end of while loop */
    /* Check if -f flag was given with no clients */
    if (client_flag && (upgr_cb.du_fecnt == 0))
    {
	du_talk (DU_ALWAYS, &upgr_cb, E_DU3514_EMPTY_CLIENT_LIST, 2,
		 (i4 *) NULL, upgradedb_name );
	upgr_exit(DU_FAIL_EXIT, I_DU0080_UPGRADEDB_TERM);
    }
    /* Check if a database name was given or -all was specified */
    if ( (upgr_cb.du_dbname[0] == EOS) && (upgr_cb.du_nscopes == 0) &&
         (upgr_cb.du_scope != DU_INSTLTN)
       )
    {
	du_talk (DU_ALWAYS, &upgr_cb, E_DU3014_NO_DBNAME, 0);
	upgr_exit (DU_FAIL_EXIT,E_DU6000_UPGRADEDB_USAGE);
    }
    if (! upgr_cb.du_nscopes )
    {
	if (upgr_cb.du_scope == DU_INSTLTN)
	    du_talk(DU_MODESENS, &upgr_cb, I_DU0082_UPGRADEDB_BEGIN, 2, 
		    0, ERx("-all") );
	else
	    du_talk(DU_MODESENS, &upgr_cb, I_DU0082_UPGRADEDB_BEGIN, 2, 
		    0, upgr_cb.du_dbname);
    }
    else
	;  /* not implemented yet -- only dbname or -all are allowed now */
    /* declare error handler for equelc errors */
    _VOID_ IIseterr(handle_equelerr);
    /* implement UPGRADEDB Tracepoints */
    {
	i4		    ctr1;
	char                *trace = NULL;
	NMgtAt("II_UPGRADEDB_TRACE", &trace);
	if ((trace != NULL) && (*trace != '\0'))
	{
	    char    *comma;
	    char    commavalue = ',';
	    i4 trace_point;
	    /* initialize all trace points to false.  This is redundant since
	    ** the structure is zerofilled, but do it anyhow for clairity */
	    for (ctr1=0; ctr1<MAX_TRACE_PTS; ctr1++)
		upgr_cb.upgtrace[ctr1]=FALSE;
	    /* start loop to set trace points */
	    do
	    {
		if (comma = STchr(trace, commavalue))
		    *comma = '\0';
		if (CVal(trace, &trace_point))
		    break;
		trace = &comma[1];
		if (trace_point < 0 ||
		    trace_point >= MAX_TRACE_PTS)
		{
		    continue;
		}
		upgr_cb.upgtrace[trace_point]=TRUE;
	    } while (comma);
	}
    }
    /* Set up stuff needed for nonstandard pagesize databases.
    ** These get their catalogs modified after upgrading is done,
    ** a la sysmod.
    */
    Dus_allcat_defs[DU_CORE_CATS]       = &Dub_01corecat_defs[0];
    Dus_allcat_defs[DU_DBMS_CATS]       = &Dub_11dbmscat_defs[0];
    Dus_allcat_defs[DU_IIDBDB_CATS]     = &Dub_31iidbdbcat_defs[0];
    Dus_allcat_defs[DU_DDB_CATS]        = &Dub_41ddbcat_defs[0];
    Dus_allcat_defs[DU_SCI_CATS]        = &Dui_51scicat_defs[0];
    dub_corecats_def();
    if( dub_dbmscats_def() != E_DU_OK )
	upgr_exit(DU_FAIL_EXIT, 0);
    dub_ddbcats_def();
    dub_iidbdbcats_def();     
}
/*{
** Name:  append_clientlist()  - validates a client and adds it to the 
**				 client list.
**
** Description:
**	Check if the client list is ok. Currently, just checks if
**	"nofeclients" appears with some other client.  If the client is
**	compatible with the other clients, then it adds the new client to
**	the list.  Otherwise it prints an error and branches to the exit
**	handler.
**
** Inputs:
**	clients 	list of clients saved so far.
**	new		current client to be added to list.
**	num_clients	running counter of clients added to list.
**
** Outputs:
**	clients		"new" is appended to the list.
**	num_clients	incremented if client is added to list.
**
** Returns:
**	None.
**
** Exceptions:
**	None.
**
** Side Effects:
**	May branch to exit handler if "NOFECLIENTS" is used in conjunction with
**	any other client name.
**
** History:
**	07-aug-91 (teresa)
**	    Initial creation
*/
static VOID
append_clientlist(clients, new, num_clients)
char	*clients;
char	*new;
i4	*num_clients;
{
	if ( (STcasecmp(clients, ERx("nofeclients") ) == 0)
	     ||
	     (*clients != EOS
	      && (STcasecmp(new, ERx("nofeclients") ) == 0)
	     ) )
	{
	    /* There is already a client named "nofeclients", so
	    ** it's not legal to see any other client names used in conjunction
	    ** with this.  Print error and abort.
	    */
	    du_talk( DU_ALWAYS, &upgr_cb, E_DU6015_BAD_CLIENT_NM, 2,
			0, ERx("nofeclients"));
	    upgr_exit( DU_FAIL_EXIT, I_DU0080_UPGRADEDB_TERM);
	}
	else
	{
	    /* Clint name is valid, so  append name to  the list we already
	    ** already have (which may be empty). NOTE: multiple -f flags
	    ** are allowed, but not documented (LRC wanted it that way!).
	    */
	    if (*num_clients == 0)
		/* 1st one on list */
		STcopy (new, clients);
	    else
		/* append to list */
		STpolycat (3, clients, ERx(" "), new, clients);
	    (*num_clients)++ ;
	}
}
/*{
** Name: cv_iidbdb	 - Convert the iidbdb
**
** Description:
**	    This routine assumes that we are connected to the iidbdb as
**	    $ingres with system catalog update privilege.  It extracts
**	    the cmptlvl of the iidbdb and then calls the appropriate
**	    conversion functions to convert the iidbdb to current format.
**
** Inputs:
**	dbdb_specified		Convert the iidbdb if already at current level
**	
** Outputs:
**	usernamep		Buffer to contain current user name
**	userstatp		Pointer to i4 for user status from iiuser
**	
** History:
**	05-nov-88 (jrb)
**	    Inital creation.
**	28-nov-88 (jrb)
**	    Added "abort" parameter so we could abort conversion if db is
**	    of improper compat level.
**	14-jun-89 (ralph)
**	    Adapted jrb's process_iidbdb() routine to 7.0 requirements
**	16-sep-89 (ralph)
**	    Check server version to ensure it is 7.0.
**	19-oct-89 (ralph)
**	    Delete config files in the dump directory.
**	19-apr-90 (ralph)
**	    Allow iidbdb to be converted even if already 6.3 when specified.
**	06-jul-90 (ralph)
**	    Return user name and status
**      10-may-91 (markm)
**          Fix for Beta - disallow any journaled 6.3 databases to be upgraded
**          and if database is at 6.3 level do not delete the config file
**          in the dmp directory.
**       3-jun-91 (markm)
**          Changed the way that upgradedb gets the username. Because
**          it runs setuid ingres dbmsinfo('username') on UNIX always
**          returns 'ingres' - changed so calls du_get_usrname() which
**          calls IDname() and gets the "actual" username not "effective"
**          Also added check for super-user status on upgrading the iidbdb.
**	22-jul-91 (teresa)
**	    Extracted messages to erduf.msg and made several modifications to
**	    meet coding standards. 
**	22-jul-91 (teresa)
**	    Extracted messages to erduf.msg and made several modifications to
**	    meet coding standards.
**	10-Jan-93 (jpk)
**	    add +U to first connect to iidbdb.  Needed to convert
**	    for increase iirelation tuple width, DB_MAXNAME, and
**	    file extend as soon as connect to server complete.
**	    For same reason added call to order_critical_65() as
**	    soon as server returns control to upgradedb.
**	19-Oct-1993 (jpk)
**	    go back to area name II_DATABASE instead of "default";
**	    Rachael's fix to internal database procedure makes
**	    this the correct parameter to pass it.
**	15-Feb-1994 (jpk)
**	    move order critical routine to after control block
**	    structure has been allocated.
**      13-jul-94 (robf)
**          Rework pre-65 conversions to be a bit more selective 
**      02-mar-95 (lawst01)  bug 67253
**          Remove 'extra' if test before the test to mark the access_flag
**          for iidbdb operable.
**      20-mar-1995 (canor01) Bug 67535
**          Connect as $ingres so back end knows this is an
**          administrative procedure
**      06-mar-1996 (nanpr01)
**          Added upgrade code for 2.0 upgrade - Variable Page Size Project. 
**      13-mar-98 (nicph02)
**          Added call to rm_iiqef_procedures to drop iiqef_ procedures.
**	27-Aug-2004 (schka24)
**	    r3 upgrade calls.  Remove journal check, server disables journaling
**	    at core catalog conversion.  Remove concurrent-upgrade option for
**	    iidbdb during its conversion.
**	4-Jan-2005 (schka24)
**	    Mods to make iidbdb connect work for ordinary users; we're
**	    removing the set-user-id privilege from upgradedb.
**	14-Feb-2005 (bonro01)
**	    Move code that sets upgrade flags for imadb, icedb, icesvr, and
**	    icetutor, after exit_cv_iidbdb1: label so that these databases
**	    are also upgraded during a 64-bit add-on upgrade along with
**	    the iidbdb.
**      07-apr-2005 (stial01)
**          cv_dbcap() Added new iidbcapabilities rows.
**      08-apr-2005 (stial01)
**          cv_dbcap() Another new iidbcapabilities row.
**	23-Jan-2006 (kschendel)
**	    Announce the server version if mismatch.  (This usually
**	    indicates a foul-up on our part.)
**	31-Jan-2006 (kschendel)
**	    Always redo iidbdb stdcat views for 9.04 upgrades.
**	22-May-2006 (kschendel)
**	    Grant the stdcat views here after we recreate all of 'em.
**
[@history_template@]...
*/
static VOID
cv_iidbdb(dbdb_specified,usernamep,userstatp)
bool	dbdb_specified;
char	*usernamep;
i4	*userstatp;
{
  i4 minorlvl;
  i4 access_flags;
  i4 cversion;
  i4 userstat;
  i4 proc_instruction;
  i4 service_flags;
  char *username;
  char cmptlvl[CMPT_LEN];
  char locname[DU_LOC_SIZE + 1];
  char areaname[DB_AREA_MAX + 1];
  char locdump[DU_LOC_SIZE + 1];
  char areadump[DB_AREA_MAX + 1];
  char dbdb_name[DB_MAXNAME+1];
  char iidbdbname[DB_MAXNAME+1];
  i4 imadb_cnt;
  i4 icedb_cnt;
  i4 icesvr_cnt;
  i4 icetutor_cnt;
  char dbmsinfo_lp64[10];
  struct {
    short buff_size;
    char buff[DU_STDCAT_NAMESIZE+1];
  } infobuff;
    char	dbdb_dba[DB_MAXNAME+1];
    char	cdbname_temp[DB_MAXNAME+1];
    char	ddbname_temp[DB_MAXNAME+1];
    bool	really_upgrade;		/* Really upgrading iidbdb */
    bool	star_flag=FALSE;
    i4		level;
    DU_ERROR	duv_errcb;
    DU_ENV	dbenv;
    bool 	upcase = FALSE;
    /* Init SQL variable pointer */
    if (upgr_cb.du_user[0]=='\0')
	username = upgr_cb.duowner;
    else
	username = upgr_cb.du_user;
    userstat = 0;			/* Init SQL variable */
    STcopy(username, usernamep);	/* return username to caller --
					** this is just a convenient pointer
					** into the upgr_cb indicating which of
					** the two possible variable names to
					** use. */
    setauth(NULL);		/* Forget any current auth setting */
    really_upgrade = FALSE;
    /* connect to the iidbdb as $ingres.  If we're J Random User, this will
    ** fail, which means that we shouldn't be trying to upgrade the iidbdb.
    ** (The first upgradedb run always has to be against iidbdb run by the
    ** installation owner, which will succeed here.)
    */
/* # line 1600 "upgradedb.sc" */	/* host code */
    upgr_cb.Ignore_errors = TRUE;
    du_talk(DU_MODESENS, &upgr_cb, I_DU0085_IIDBDB_CONNECT, 0);
    if (upg_dbg_out)
    {
	SIprintf("]==> [1] connect to iidbdb session %d id %s options = -A6 +w\n",
		DBDB_SESSIONID, DU_DBA_DBDB);
    }
    if (vnode[0] != '\0')
       STprintf(iidbdbname, "%s::iidbdb", vnode);
    else
       STcopy("iidbdb", iidbdbname);
/* # line 1611 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DBDB_SESSIONID);
    IIsqUser(DU_DBA_DBDB);
    IIsqConnect(0,iidbdbname,(char *)"-A6",(char *)"+w",(char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 1613 "upgradedb.sc" */	/* host code */
    /* *Note* The above connect will cause the server to upgrade
    ** the database config file (if needed) and the core catalogs.
    ** iirelation and iiattribute are fixed to reflect the core catalog
    ** changes.  No other tables or table descriptors are changed.
    ** Journaling is forcibly turned off (a la alterdb -disable_journaling).
    */
    if (sqlca.sqlcode < 0)
    {
	/* If this failed because of "no privilege", the iidbdb is OK but
	** we're not a privileged user.  Open iidbdb again without the
	** fancy options (so that we can collect db names), and return
	** without attempting to re-upgrade iidbdb.
	**
	** *Note* we're still going to want to update iidatabase.  It
	** turns out that all that's needed is IIlq_Protect.  If or when
	** that hole gets fixed, this connect will probably need to
	** invoke a system standard role of some sort.
	*/
	if (sqlca.sqlcode != -GE_NO_PRIVILEGE || dbdb_specified)
	    upgr_exit(DU_FAIL_EXIT, E_DU6002_BAD_IIDBDB_CONNECT);
/* # line 1633 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DBDB_SESSIONID);
    IIsqConnect(0,iidbdbname,(char *)"+w",(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 1635 "upgradedb.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	    upgr_exit(DU_FAIL_EXIT, E_DU6002_BAD_IIDBDB_CONNECT);
	if (upg_dbg_out)
	{
	    SIprintf("]==> connected to iidbdb (unprivileged)\n");
	}
	upgr_cb.dbdb_open = TRUE;
	upgr_cb.dbdb_cvt = FALSE;
	upgr_cb.Ignore_errors = FALSE;
	IIlq_Protect(TRUE);			/* The magic word */
	return;
    }
/* # line 1648 "upgradedb.sc" */	/* host code */
    upgr_cb.Ignore_errors = FALSE;
    if (upg_dbg_out)
    {
	SIprintf("]==> connected to iidbdb\n");
    }
    /* Note that just because we were able to connect -u$ingres to the
    ** iidbdb doesn't make us the installation owner.  As noted above,
    ** though, the first time we get here we ARE the installation owner,
    ** unless the user is doing something very wrong (like bypassing the
    ** setup scripts).
    */
    /* Make sure there's a work directory in case the iidatabase/iilocations
    ** query decides to use it!
    */
    if (vnode[0] == '\0')
        chk_create_workdir("iidbdb");
    upgr_cb.dbdb_open = TRUE;
    IIlq_Protect(TRUE);		/* allow system catalog update priv */
    /* Make sure we are connected to a server at the proper level.
    ** e.g. 6.5 upgradedb will only let itself be run against a 6.5 server.
    */
/* # line 1669 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select dbmsinfo('_version')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,21,DU_STDCAT_NAMESIZE+1+2,&infobuff);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1670 "upgradedb.sc" */	/* host code */
    if (upgr_cb.Iidbdbok == FALSE)
	return;
    if (sqlca.sqlerrd[2] == 0 || infobuff.buff_size == 0 )
	    upgr_exit (DU_FAIL_EXIT, E_DU6003_UNKNOWN_VERSION);
    /* Beware that the _version string is longer than SERVER_VERSION,
    ** only compare the beginning.
    */
    infobuff.buff[infobuff.buff_size] = '\0';
    if (MEcmp(infobuff.buff, SERVER_VERSION, STlength(SERVER_VERSION)) != 0)
    {
	du_talk(DU_ALWAYS, &upgr_cb, E_DU6004_WRONG_VERSION, 4,
		0, SERVER_VERSION,
		0, infobuff.buff);
	upgr_exit (DU_FAIL_EXIT, 0);
    }
    STcopy(DU_DBDBNAME, dbenv.du_dbname);
    /* Find out if dbms is lp64 */
/* # line 1687 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select dbmsinfo('lp64')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,9,dbmsinfo_lp64);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1688 "upgradedb.sc" */	/* host code */
    if (dbmsinfo_lp64[0] == 'Y')
	upgr_cb.dbmsinfo_lp64 = 'Y';
    else
	upgr_cb.dbmsinfo_lp64 = 'N';
    if (upg_dbg_out)
	SIprintf("dbmsinfo('lp64') returns '%c'\n", upgr_cb.dbmsinfo_lp64);
    /* Get information about the iidbdb database */
/* # line 1695 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select iidatabase.dbcmptlvl, iidatabase.dbcmptminor, iidatabase.acces\
s, iidatabase.dbdev, iidatabase.dbservice, iilocations.area from iidat\
abase, iilocations where iidatabase.name='iidbdb' and iidatabase.dbdev\
=iilocations.lname");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,CMPT_LEN-1,cmptlvl);
      IIgetdomio((short *)0,1,30,4,&minorlvl);
      IIgetdomio((short *)0,1,30,4,&access_flags);
      IIgetdomio((short *)0,1,32,DU_LOC_SIZE + 1-1,locname);
      IIgetdomio((short *)0,1,30,4,&service_flags);
      IIgetdomio((short *)0,1,32,DB_AREA_MAX + 1-1,areaname);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1715 "upgradedb.sc" */	/* host code */
    if (upgr_cb.Iidbdbok == FALSE)
	return;
    if (sqlca.sqlerrd[SQL_NUM_ROWS] == 0)
	upgr_exit (DU_FAIL_EXIT, E_DU6005_NONEXISTENT_IIDBDB);
    /* if db is already inoperative we halt with an error */
    if (((access_flags & DU_OPERATIVE) == 0) &&
	((access_flags & DU_UPGRADING) == 0)
       )
	upgr_exit (DU_FAIL_EXIT, E_DU6006_INOPERATIVE_IIDBDB);
    if (service_flags & DU_NAME_UPPER)
    {
	CVupper(username);
	upcase = TRUE;
    }
    /* Get information about the user -
    ** This used to be done by checking with dbmsinfo(), but because
    ** upgradedb ran as setuid ingres on UNIX this was always returning
    ** ingres as the user.  (We no longer do the suid stuff but this
    ** code works ok as is.)
    */
/* # line 1735 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select iiuser.status from iiuser where iiuser.name=");
    IIputdomio((short *)0,1,32,0,username);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&userstat);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1737 "upgradedb.sc" */	/* host code */
    if (upgr_cb.Iidbdbok == FALSE)
	return;
    if (sqlca.sqlerrd[SQL_NUM_ROWS] == 0)
	upgr_exit (DU_FAIL_EXIT, E_DU6007_NONEXISTENT_USER);
    *userstatp = (userstat & DU_USECURITY);
    if (userstat & DU_USECURITY)
	du_talk(DU_MODESENS, &upgr_cb, I_DU0086_SUPERUSER, 2, 0, username);
    else
	du_talk(DU_MODESENS, &upgr_cb, I_DU0087_NOTSUPERUSER, 2, 0, username);
    /* Get the level to see if we need to convert */
    level  = set_cmptlvl(cmptlvl, minorlvl);
    /* build an entry that subordinate routines will require */
    STcopy(ERx("iidbdb"),dbdb_name);
    STcopy(ERx("$ingres"), dbdb_dba);
    if (upcase)
      CVupper(dbdb_dba);
    add_dbname(dbdb_name, dbdb_dba, level, access_flags,
		locname, areaname, cdbname_temp, ddbname_temp, star_flag, 
		upcase, service_flags);
    upgr_cb.Dbname_ptr = upgr_cb.Dbname_head;
    /* Check the level to see if we need to convert */
    if (level == 0)
	upgr_exit(DU_FAIL_EXIT, E_DU6008_IIDBDB_WRONG_LEVEL);
    else if ( level == CURRENT_LEVEL )
    {
	if (upg_dbg_out)
	{
	    SIprintf("]==> do NOT need to convert the iidbdb\n");
	}
        upgr_cb.dbdb_cvt=1;
        goto  exit_cv_iidbdb1;      /*  lawst01  01-27-95  */
    }
    /* Check that we are a super-user, if we want to update the iidbdb */
    if ( (userstat & DU_USECURITY) == 0)
    {
	    upgr_exit( DU_FAIL_EXIT, E_DU6009_IIDBDB_UPGR_DISALLOW);
    }
    /* this is where we need to disconnect and reconnect if we are 
    ** doing an upgradedb of the iidbdb and need to lock it.
    */
/* # line 1777 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1778 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 1779 "upgradedb.sc" */	/* host code */
    if (upg_dbg_out)
    {
	SIprintf("]==> (a) disconnect\n");
    }
    upgr_cb.dbdb_open = FALSE;
    if (upg_dbg_out)
    {
	SIprintf("]==> [2] connect to iidbdb session %d id %s -A6 -l +U +w\n",
	DBDB_SESSIONID, DU_DBA_DBDB);
    }
/* # line 1789 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DBDB_SESSIONID);
    IIsqUser(DU_DBA_DBDB);
    IIsqConnect(0,iidbdbname,(char *)"-A6",(char *)"-l",(char *)"+U",(char *)"\
+w",(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 1792 "upgradedb.sc" */	/* host code */
    if (upg_dbg_out)
    {
	SIprintf("]==> connected to iidbdb\n");
    }
    if (sqlca.sqlcode < 0)
	upgr_exit(DU_FAIL_EXIT, E_DU600D_BAD_IIDBDB_RECONNECT);
    else
	du_talk (DU_MODESENS, &upgr_cb, I_DU00B6_DB_IDENTIFIER, 2,
		 0, dbenv.du_dbname);
    upgr_cb.dbdb_open = TRUE;
    IIlq_Protect(TRUE);			/* system catalog update priv */
    really_upgrade = TRUE;
    /* Rehash the core catalogs if the server rewrote them upon connect. */
    order_critical_rehash(DU_DBDBNAME, level);
    /* perform 6.5 order-critical conversion */
    if (level < V65_LEVEL)
    {
	order_critical_65(DU_DBDBNAME, upgr_cb.Dbname_ptr, 
			  upgr_cb.Dbname_ptr->upcase);
    }
    else if (level < V65ES_LEVEL)
    {
	order_critical_65es(DU_DBDBNAME, upgr_cb.Dbname_ptr->upcase);
    }
    /*
    ** After the 65/65ES upgrade, upgrade catalogs to V20_LEVEL
    */
    if (level < V20_LEVEL)
    {
	order_critical_20(DU_DBDBNAME, upgr_cb.Dbname_ptr, 
			  upgr_cb.Dbname_ptr->upcase);
    }
    /* Order critical bootstrap changes for r3 */
    if (level < V30_LEVEL)
    {
	order_critical_r3(upgr_cb.Dbname_ptr);
    }
    /* Order critical bootstrap changes for r3.0.2 */
    if (level < V302_LEVEL)
    {
	order_critical_r302(upgr_cb.Dbname_ptr, iidbdbname);
    }
    /* Set the access flags to indicate inoperative/converting */
    if (access_flags   & DU_OPERATIVE)
	access_flags  -= DU_OPERATIVE;
    if (!(access_flags & DU_UPGRADING))
	access_flags  += DU_UPGRADING;
    dbenv.du_access = access_flags;
    if (upgr_cb.upgtrace[UPG04_SKIP_BECVT])
    {
	/* we hit a trace point to skip upgrading the back end catalogs */
	du_talk (DU_MODESENS, &upgr_cb, W_DU1856_TRACE_4, 0);
    }
    else
    {
	/* Update the access field in the config file as inoperative */
	proc_instruction = QEA_UPD_ACCESS;
	if (upg_dbg_out)
	{
	    SIprintf("]==> [1] executing ii_update_config(%s, %s, %d, %d)\n",
		dbdb_name, areaname, proc_instruction, access_flags);
	}
/* # line 1854 "upgradedb.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"ii_update_config");
    IILQprvProcValio((char *)"database_name",0,(short *)0,1,32,0,dbdb_name);
    IILQprvProcValio((char *)"location_area",0,(short *)0,1,32,0,areaname);
    IILQprvProcValio((char *)"update_map",0,(short *)0,1,30,4,&
    proc_instruction);
    IILQprvProcValio((char *)"access",0,(short *)0,1,30,4,&access_flags);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 1859 "upgradedb.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    du_talk (DU_ALWAYS, &upgr_cb, E_DU600B_ACCESS_UPDATE_ERR, 2,
		     0, dbenv.du_dbname);
	    upgr_exit(DU_FAIL_EXIT, 0);
	}
	/* Mark the database inoperative */
/* # line 1866 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iidatabase set access\
=");
    IIputdomio((short *)0,1,30,4,&access_flags);
    IIwritio(0,(short *)0,1,32,0,(char *)"where name='iidbdb'");
    IIsyncup((char *)0,0);
  }
/* # line 1868 "upgradedb.sc" */	/* host code */
	if (upgr_cb.Iidbdbok == FALSE)
	    return;
	/* Commit inoperability */
/* # line 1871 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1872 "upgradedb.sc" */	/* host code */
	if (upgr_cb.Iidbdbok == FALSE)
	    return;
	/* Convert the iidbdb */
	upgr_cb.dbdb_cvt = TRUE;
	du_talk (DU_MODESENS, &upgr_cb, I_DU0088_UPGRADE_ID, 4,
		0, dbenv.du_dbname, 0, SERVER_VERSION);
	/*
	** These are only necessary when converting from pre-64
	*/
	if (level < V64_LEVEL )
	{
	    cv_pre64_user();		
	    if (upgr_cb.Iidbdbok == FALSE)
	    	return;
	    cv_group();		
	    if (upgr_cb.Iidbdbok == FALSE)
	        return;
	    cv_pre64_role();	
	    if (upgr_cb.Iidbdbok == FALSE)
	    return;
	    cv_online();		
	    if (upgr_cb.Iidbdbok == FALSE)
	         return;
	}
	if (level < V65ES_LEVEL)
	{
	    cv_dbaccess();		/* 65 needed, but not for 65ES */
	    if (upgr_cb.Iidbdbok == FALSE)
	        return;
	}
          /* This does the main conversion for every database */
        cv_database(dbenv.du_dbname);
        if (upgr_cb.Iidbdbok == FALSE)
            return;
        cv_iistar_cdbinfo(upgr_cb.Dbname_ptr->upcase);
        if (upgr_cb.Iidbdbok == FALSE)
            return;
	if (level < V65_LEVEL)
	{
	    /* these were created in 6.5 and need to be created if they
	    ** do not exist.  They will NOT exist in any database that is
	    ** level 6.4 or below.  They should already exist for 6.5 dbs
	    ** or above.
	    */
	    /* iigw07_relation, iigw07_attribute,iigw07_indexes */
	    duc_iigw07();
	    duc_modify_iigw07();
	    /* add IMA tables for XA use */
	    duc_iilgmo(&duv_errcb);
	    if (upgr_cb.Iidbdbok == FALSE)
		return;
	    /* convert iidbdb catalogs that have changed for 6.5, e.g.
	    ** added columns or increased column width for DB_MAXNAME.
	    */
	    changed_for_65_syscats_iidbdb("iidbdb");
	    if (upgr_cb.Iidbdbok == FALSE)
		return;
	    file_extend_convert_user();	/* a no-op, one hopes, but be safe */
	}
	if (level < V65ES_LEVEL)
	{
	    /* these were created in 6.5 ES and need to be created if they
	    ** do not exist.  They will NOT exist in any database that is
	    ** level 6.5 ES or below.  They should already exist for 6.5 ES dbs
	    ** or above.
	    */
	    new_for_65es_syscats_iidbdb("iidbdb");
	    if (upgr_cb.Iidbdbok == FALSE)
		return;
	    /* convert iidbdb catalogs that have changed for 6.5 ES, e.g.
	    ** added columns. 
	    */
	    changed_for_65es_syscats_iidbdb("iidbdb");
	    if (upgr_cb.Iidbdbok == FALSE)
		return;
	    /* Removed iiaudit, will be done by r3 conversion */
	    if (upgr_cb.Iidbdbok == FALSE)
		return;
	}
	/* update the std catalog interface for iidbdb */
	if (level < V20_LEVEL)
	{
	    new_for_20_stdcats("iidbdb", upgr_cb.Dbname_ptr->upcase);
	    if (upgr_cb.Iidbdbok == FALSE)
	       return;
	}
	if (level < V26_LEVEL)
	{
	    cv_iilocations("iidbdb");
	    cv_26_iiextend("iidbdb");
	    if (upgr_cb.Iidbdbok == FALSE)
		return;
	}
	if (level < V30_LEVEL)
	{
	    changed_for_r3_syscats_iidbdb("iidbdb", upgr_cb.Dbname_ptr->upcase);
	}
	/* 3.0.2 is all done in "order critical", nothing more here */
        /* Any stdcat views dropped should be re-created, unless it's
	** going to be done by fix-qry-trees.
	** NOTE: cv_database likely did all the standard ones already
	** if we aren't fixing qry trees, we still need to do the iidbdb
	** ones and regrant them all.
	*/
	if (! (upgr_cb.Dbname_ptr->tree_convert || upg_trees) )
	{
	    du_talk(DU_MODESENS, &upgr_cb, I_DU0103_RECREATING_STDCAT_VIEWS, 0);
	    duc_upgrade_stdcat_views("iidbdb", upgr_cb.Dbname_ptr->upcase,
                    upgr_cb.Dbname_ptr->star_flag);
	    /* Regrant select to public on all system catalog views. */
	    duc_grant_stdcat_views("iidbdb", upgr_cb.Dbname_ptr->star_flag);
	}
	/* conversion done; now upgrade cmptlvl and mark operative */
	access_flags += DU_OPERATIVE;
	access_flags -= DU_UPGRADING;
	dbenv.du_access = access_flags;
	_VOID_ STcopy(DU_CUR_DBCMPTLVL, cmptlvl);
	minorlvl = DU_1CUR_DBCMPTMINOR;
/* # line 1990 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iidatabase set access\
=");
    IIputdomio((short *)0,1,30,4,&access_flags);
    IIwritio(0,(short *)0,1,32,0,(char *)", dbcmptlvl=");
    IIputdomio((short *)0,1,32,4,DU_CUR_DBCMPTLVL);
    IIwritio(0,(short *)0,1,32,0,(char *)", dbcmptminor=");
    IIputdomio((short *)0,1,30,sizeof(DU_1CUR_DBCMPTMINOR),(void *)IILQint(
    DU_1CUR_DBCMPTMINOR));
    IIwritio(0,(short *)0,1,32,0,(char *)"where name='iidbdb'");
    IIsyncup((char *)0,0);
  }
/* # line 1997 "upgradedb.sc" */	/* host code */
	if (upgr_cb.Iidbdbok == FALSE)
	    return;
	/* Now update the access and compat fields of the config file.
	** If the original level was r3, we had to do some catalog upgrade
	** work, set the new cversion now.  If the original was pre-r3,
	** all the necessary work was done by core catalog conversion at
	** connect time.
	*/
	proc_instruction = QEA_UPD_ACCESS | QEA_UPD_CMPTLVL | QEA_UPD_CMPTMINOR;
	if (level == V30_LEVEL)
	{
	    proc_instruction |= QEA_UPD_CVERSION;
	}
	cversion = DSC_VCURRENT;
	if (upg_dbg_out)
	{
	    SIprintf(
	    "]==> [2] executing ii_update_config(%s, %s, %d, %d, %d, %d, %d)\n",
	    dbdb_name, areaname, proc_instruction, access_flags,
	    cmptlvl, minorlvl, cversion);
	}
/* # line 2018 "upgradedb.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"ii_update_config");
    IILQprvProcValio((char *)"database_name",0,(short *)0,1,32,0,dbdb_name);
    IILQprvProcValio((char *)"location_area",0,(short *)0,1,32,0,areaname);
    IILQprvProcValio((char *)"update_map",0,(short *)0,1,30,4,&
    proc_instruction);
    IILQprvProcValio((char *)"access",0,(short *)0,1,30,4,&access_flags);
    IILQprvProcValio((char *)"cmptlvl",0,(short *)0,1,32,0,cmptlvl);
    IILQprvProcValio((char *)"cmptminor",0,(short *)0,1,30,4,&minorlvl);
    IILQprvProcValio((char *)"cversion",0,(short *)0,1,30,4,&cversion);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 2026 "upgradedb.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    du_talk (DU_ALWAYS, &upgr_cb, E_DU600B_ACCESS_UPDATE_ERR, 2,
		    0, dbenv.du_dbname);
	    du_talk (DU_ALWAYS, &upgr_cb, E_DU600C_CMPT_UPDATE_ERR, 2,
		    0, dbenv.du_dbname);
	    upgr_exit( DU_FAIL_EXIT, 0);
	}
	/*
	** We must commit now, disconnect and reconnect
	** or we will not be able to access "dmpdev" in iidatabase.
	*/
/* # line 2038 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 2039 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 2040 "upgradedb.sc" */	/* host code */
	if (upg_dbg_out)
	{
	    SIprintf("]==> (b) disconnect\n");
	}
	upgr_cb.dbdb_open = FALSE;
	if (upg_dbg_out)
	{
	    SIprintf(
	    "]==> [3] connect to iidbdb session %d id %s -A6 -l +w\n",
	    DB_SESSIONID, DU_DBA_DBDB);
	}
/* # line 2051 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DBDB_SESSIONID);
    IIsqUser(DU_DBA_DBDB);
    IIsqConnect(0,iidbdbname,(char *)"-A6",(char *)"-l",(char *)"+w",
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 2054 "upgradedb.sc" */	/* host code */
	if (upg_dbg_out)
	{
	    SIprintf("]==> connected\n");
	}
	if (sqlca.sqlcode < 0)
	    upgr_exit( DU_FAIL_EXIT, E_DU600D_BAD_IIDBDB_RECONNECT);
	upgr_cb.dbdb_open = TRUE;
	IIlq_Protect(TRUE);			/* system catalog update priv */
	/* Get info to delete the dump directory's config file */
/* # line 2063 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select iidatabase.dmpdev, iilocations.area from iidatabase, iilocatio\
ns where iidatabase.name='iidbdb' and iidatabase.dmpdev=iilocations.ln\
ame");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DU_LOC_SIZE + 1-1,locdump);
      IIgetdomio((short *)0,1,32,DB_AREA_MAX + 1-1,areadump);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 2068 "upgradedb.sc" */	/* host code */
	if (upgr_cb.Iidbdbok == FALSE)
	    return;
	if (sqlca.sqlerrd[SQL_NUM_ROWS] == 0)
	    upgr_exit(DU_FAIL_EXIT,E_DU600E_NO_DUMP_INFO);
	/* We will only delete configuration files in the dump directory
	** if this is a 6.1 or 6.2 database
	*/
	if ( level <= V62_LEVEL )
	{
	    if (upg_dbg_out)
	    {
		SIprintf("]==> executing ii_del_dmp_config (%s, %s)\n",
		    dbdb_name, areaname);
	    }
/* # line 2082 "upgradedb.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"ii_del_dmp_config");
    IILQprvProcValio((char *)"database_name",0,(short *)0,1,32,0,dbdb_name);
    IILQprvProcValio((char *)"location_area",0,(short *)0,1,32,0,areaname);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 2085 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		    du_talk (DU_ALWAYS, &upgr_cb, E_DU600F_BAD_DUMP_CLEANUP, 2,
			    0, dbenv.du_dbname);
		    upgr_exit( DU_FAIL_EXIT, 0);
	    }
	}
	/* Commit dbms upgrade then do fe catalog upgrade */
/* # line 2093 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 2094 "upgradedb.sc" */	/* host code */
    } /* endif db_convert is true */
    /* Jump here if iidbdb was already current version.  We might still have
    ** to upgrade trees if switching 64-bit-ness, or if -tree.  Note: do
    ** not attempt to upgrade FE cats unless we upgraded iidbdb itself, or
    ** if user explicitly said "upgradedb iidbdb".  Reason is that there's
    ** no simple FE catalog version to look at to skip the work, and
    ** we don't want to do redundant stuff to iidbdb every time thru.
    */
exit_cv_iidbdb1:
    /* See if we need to convert imadb, icedb, icesvr, icetutor */
/* # line 2104 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iidatabase where lowercase(name)='imadb'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&imadb_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 2106 "upgradedb.sc" */	/* host code */
    if (imadb_cnt == 1)
	upgr_cb.imadb_cvt = TRUE;
/* # line 2108 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iidatabase where lowercase(name)='icedb'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&icedb_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 2110 "upgradedb.sc" */	/* host code */
    if (icedb_cnt == 1)
	upgr_cb.icedb_cvt = TRUE;
/* # line 2112 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iidatabase where lowercase(name)='icesvr'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&icesvr_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 2114 "upgradedb.sc" */	/* host code */
    if (icesvr_cnt == 1)
	upgr_cb.icesvr_cvt = TRUE;
/* # line 2116 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iidatabase where lowercase(name)='icetutor'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&icetutor_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 2118 "upgradedb.sc" */	/* host code */
    if (icetutor_cnt == 1)
	upgr_cb.icetutor_cvt = TRUE;
/* # line 2120 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 2121 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 2122 "upgradedb.sc" */	/* host code */
    if (upg_dbg_out)
    {
	SIprintf("]==> [3a] connect to iidbdb session %d id %s options = -A6 +w -l\n",
		DBDB_SESSIONID, DU_DBA_DBDB);
    }
/* # line 2127 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DBDB_SESSIONID);
    IIsqUser(DU_DBA_DBDB);
    IIsqConnect(0,iidbdbname,(char *)"-A6",(char *)"+w",(char *)"-l",
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 2129 "upgradedb.sc" */	/* host code */
    /* Fix trees (and standard catalog views) */
    if (upgr_cb.Dbname_ptr->tree_convert || upg_trees)
	fix_qry_trees("iidbdb", iidbdbname, FALSE, level);
    IIlq_Protect (TRUE);         /* system catalog update privilege */
    if (dbdb_specified || really_upgrade)
    {
	if (upgr_cb.upgtrace[UPG03_SKIP_FECVT])
	{
	    /* we hit a trace point to skip front end catalog conversion */
	    du_talk (DU_MODESENS, &upgr_cb, W_DU1855_TRACE_3, 0);
	}
	else
	    fe_dictinst(dbenv.du_dbname,DU_DBA_DBDB, level);
    }
    /* Issue message to let user know if the database was [not] converted */
    if (really_upgrade && upgr_cb.Iidbdbok)
	du_talk ( DU_MODESENS, &upgr_cb, I_DU0089_DB_UPGRADED, 4,
	     0, dbenv.du_dbname, 0, SERVER_VERSION);
    /* indicate that we are done converting the iidbdb */
    upgr_cb.dbdb_cvt = FALSE;
    /* commit any fe work.  disconnect and reconnect without a lock so that
    ** other users may access the installation (to create new dbs, etc)...
    */
    if (really_upgrade)
    {
	rm_iitmp_tables();          /* remove all iitmp_ tables */
	rm_iiqef_procedures();      /* Bug 89164: remove all iiQEF procedures */
    }
/* # line 2157 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 2158 "upgradedb.sc" */	/* host code */
    if (upgr_cb.Dbname_ptr->tree_convert)
	fix_dbservice(iidbdbname);
/* # line 2160 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 2161 "upgradedb.sc" */	/* host code */
    if (upg_dbg_out)
    {
	SIprintf("]==> (c) disconnect\n");
    }
    upgr_cb.dbdb_open = FALSE;
    if (upg_dbg_out)
    {
	SIprintf("]==> [4] connect to iidbdb session %d id %s options +w\n",
	    DBDB_SESSIONID,DU_DBA_DBDB);
    }
    /* iidbdb is done, shouldn't need upgradedb flag any more */
/* # line 2172 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DBDB_SESSIONID);
    IIsqUser(DU_DBA_DBDB);
    IIsqConnect(0,iidbdbname,(char *)"+w",(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 2174 "upgradedb.sc" */	/* host code */
    if (upg_dbg_out)
    {
	SIprintf("]==> connected\n");
    }
    if (sqlca.sqlcode < 0)
	upgr_exit(DU_FAIL_EXIT, E_DU600D_BAD_IIDBDB_RECONNECT);
    upgr_cb.dbdb_open = TRUE;
    /* Remove old execute protect if old database */
    if (level < V26_LEVEL)
	chmod_data_600();
    IIlq_Protect(TRUE);			/* system catalog update priv */
}
/*{
** Name: collect_dbs	- Collect all dbs which are in need of conversion
**			  and convert them
**
** Description:
**	This routine queries iidatabase for all dbs in need of conversion
**	and marks them inoperative (which will fail if any db is already
**	inoperative).  After each db is processed, it is added--along with
**	its cmptlvl--to a linked list of dbs.
**
**	This routine assumes that we are conenected to the iidbdb on the
**	DB_SESSION_ID with system catalog update priv.  It will stay connected
**	to the iidbdb even when individual dbs are being upgradedb.  It
**	does not disconnect unless it encounters a fatal error.
**
**	It does commits as it marks each database operative or inoperative
**	because it will operate on several DBs and if 1 fails, we do not want
**	to lose the status of the others.
**
**	This will build a list of all DBs in the installation that meet the
**	qualification and cache it in memory. The alternative is to open a
**	cursor on iidatabase and walk it.  However, the cursor is not
**	desirable because it will keep a lock on the iidatabase table for a
**	long period of time, which will keep other users from using any other
**	databases in the installation.  This routine will periodically take
**	and release table-level share-mode locking on the iidatabase catalog
**	to assure the consistency of the catalog while it is querying it.
**
**	It is possible that a database could be destroyed (destroydb) after it 
**	has been selected for upgrade and cached on the upgradedb cache, but
**	before upgradedb actually performed on the database.  If this HIGHLY
**	UNLIKELY, but there is a window and sometimes  users do strange things.
**	Therefore logic is built into upgradedb to handle this.  That logic
**	is:  if the connect to the target db fails, then scan iidatabase to
**	see if the db entry is still listed in iidatabase.  If the listing has
**	gone away print a warning that the database was destroyed after 
**	selection for upgrade.  If the listing is still there, then attempt a
**	reconnect and handle as an error if the reconnect fails.
**	
** Inputs:
**	dbname		The name of the database to "collect"
**			if "-all", all non-iidbdb databases are "collected"
**	username	The name of the current user.
**	superuser	zero if non-superuser; non-zero if superuser;
**
** Outputs:
**	none
**	
** Returns:
**	none
**
** Side Effects:
**	none
**
** History:
**	05-nov-88 (jrb)
**	    Extracted from main.
**	28-nov-88 (jrb)
**	    Added message to tell user there were no databases of the proper
**	    level to be converted.
**	07-jun-89 (ralph)
**	    Adapted from jrb's cv60to61 utility.
**	19-oct-89 (ralph)
**	    Collect info to delete config files in the dump directory.
**	14-dec-89 (ralph)
**	    Allow conversion of 6.3 database if one was specified on the
**	    command line.  If -all specified, continue to bypass conversion.
**	07-jun-90 (ralph)
**	    Add logic to qualify fe/be conversion separately.
**	06-jul-90 (ralph)
**	    Ensure user has permission to perform conversion, as follows:
**		If "-all" was specified, user must be a super user.
**		If dbname was specified, user must be a super user or the DBA.
**	03-may-91 (teresa)
**	    Add support for STAR databases here.
**      10-may-91 (markm)
**          Fix for Beta - disallow any journaled 6.3 databases to be upgraded
**          and if database is at 6.3 level do not delete the config file
**          in the dmp directory.
**	24-jul-91 (teresa)
**	    Extracted messages to erduf.msg and made several modifications to
**	    meet coding standards.  Also changed logic to be a master executive
**	    that causes the other DBs to be upgraded and marked operative again.
**	02-dec-91 (teresa)
**	    fix bug 40507.
**	16-apr-92 (teresa)
**	    change "for" control loop to "do" to appease sun4 compiler warnings.
**	19-Oct-1993 (jpk)
**	    go back to area name II_DATABASE instead of "default";
**	    Rachael's fix to internal database procedure makes
**	    this the correct parameter to pass it.
**	14-jul-94 (robf)
**          Only run order_critical_65() if not already on 65, and add
**          order_critical_65es() if already at 65.
**      28-jan-95 (lawst001)  Bug 66464
**          If a single database was specified for conversion and this 
**          database was already at the current level (1.1) upgradedb would
**          upgrade it anyway.  However, if the -all flag was specified
**          only databases below the 1.1 level would be upgraded.
**	5-dec-95 (stephenb)
**	    open_target() may fail in such a way as to leave us switched
**	    into a bad connection (notably when the target database is 
**	    inconsistent), this causes problems upgrading subsequent databases.
**	    Makse sure we switch back to the iidbdb connection before
**	    continuing
**	21-feb-96 (prida01)
**	    remove vnode from database name when checking if directory exists
**	    74749
**      06-mar-1996 (nanpr01)
**          Added upgrade code for 2.0 upgrade - Variable Page Size Project. 
**	31-may-96 (prida01)
**	    modify vnode change to work for local databases.
**      13-mar-98 (nicph02)
**          Added call to rm_iiqef_procedures to drop iiqef_ procedures.
**	12-may-99 (stephenb)
**	    Add call to changed_cats_25() for 2.5 catalog changes
**      31-Oct-03 (wanfr01)
**          Bug 111216, INGSRV 2583
**          Moved one of the error code checks so it runs
**          if debug mode isn't set (upgradedb -d)
**	12-mar-2004 (somsa01)
**	    Split out convert part into convert_dbs(), so that this routine
**	    can be invoked multiple times to add multiple system databases
**	    to the master list.
**	22-Sep-2004 (schka24)
**	    Split "all" loop so that we always do Star databases last.
**	    Otherwise we might upgrade a Star database before some DB
**	    that it references, which won't fly.
**	    (If a Star DB references another Star DB, then .. oh well...)
*/
static VOID
collect_dbs(dbname, username, superuser)
char	*dbname;
char	*username;
i4	superuser;
{
  char *dbname_curr;
  char dbname_tmp[DB_MAXNAME+1];
  char cdbname_tmp[DB_MAXNAME+1];
  char ddbname_tmp[DB_MAXNAME+1];
  char dba[DB_MAXNAME+1];
  i4 access_flags;
  i4 dbservice;
  char cmptlvl[CMPT_LEN];
  i4 minorlvl;
  i4 level;
  char locname[DU_LOC_SIZE + 1];
  char areaname[DB_AREA_MAX + 1];
    bool	    star_flag = FALSE;
    bool	    convert_multiple;
    char            *colon_ptr;
    bool 	    upcase = FALSE;
    bool	    any_star = FALSE;
    dbname_curr = dbname;
    /*
    ** We have two ways of extracting databases to be converted:
    **     o If -all specified, use a select loop
    **	   o If dbname specified, select on that specific database
    */
    convert_multiple = (upgr_cb.du_scope == DU_INSTLTN);
    /* start a new tranaction and set up a lock on the iidatabase table */
/* # line 2349 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 2350 "upgradedb.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode on iidatabase where level=table");
    IIsyncup((char *)0,0);
  }
/* # line 2351 "upgradedb.sc" */	/* host code */
    if (convert_multiple)
    {
	if (upg_dbg_out)
	{
	    SIprintf("]==> converting multiple\n");
	}
	/* bypass the conversion if the user is NOT a super user */
	if (superuser == 0)
	    upgr_exit (DU_FAIL_EXIT, E_DU6010_NOT_SUPERUSER);
	/* Skip iidbdb, imadb on -all;  imadb should have been done when
	** iidbdb was done, and doing it after the initial upgrade can
	** hang (if the remote command server is running).
	** If for some reason imadb upgrade was skipped, it needs to be
	** upgraded by itself explicitly.
	*/
/* # line 2383 "upgradedb.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"x",7658,16075);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select iidatabase.name, iidatabase.own, iidatabase.access, iidatabase\
.dbcmptlvl, iidatabase.dbservice, iidatabase.dbcmptminor, iidatabase.d\
bdev, iilocations.area from iidatabase, iilocations where(lowercase(ii\
database.own)!='$ingres' or lowercase(");
    IIwritio(0,(short *)0,1,32,0,(char *)
"iidatabase.name)not in('iidbdb', 'imadb'))and iidatabase.dbdev=iiloca\
tions.lname order by own, name");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"x",7658,16075);
  }
/* # line 2384 "upgradedb.sc" */	/* host code */
	/* loop for each database in the installation */
	for (;;)
	{
/* # line 2387 "upgradedb.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"x",7658,16075,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,dbname_tmp);
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,dba);
      IIcsGetio((short *)0,1,30,4,&access_flags);
      IIcsGetio((short *)0,1,32,CMPT_LEN-1,cmptlvl);
      IIcsGetio((short *)0,1,30,4,&dbservice);
      IIcsGetio((short *)0,1,30,4,&minorlvl);
      IIcsGetio((short *)0,1,32,DU_LOC_SIZE + 1-1,locname);
      IIcsGetio((short *)0,1,32,DB_AREA_MAX + 1-1,areaname);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 2396 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode == DU_SQL_NONE)
	    {       
/* # line 2398 "upgradedb.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"x",7658,16075);
  }
/* # line 2399 "upgradedb.sc" */	/* host code */
		break;
	    }
	    if (upg_dbg_out)
	    {
		SIprintf(
		"]==> considering database %s owned by %s dbservice %d\n\n",
		dbname_tmp, dba, dbservice);
	    }
	    if (dbservice & (DU_1SER_DDB | DU_2SER_CDB) )
	    {
		/* Skip star DB's first pass; need second pass */
		any_star = TRUE;
		continue;
	    }
	    /* is it FIPS database */
	    if (dbservice & DU_NAME_UPPER)
	      upcase = TRUE;
	    else
	      upcase = FALSE;
	    _VOID_ STtrmwhite(dbname_tmp);
	    _VOID_ STtrmwhite(locname);
	    _VOID_ STtrmwhite(areaname);
	    _VOID_ STtrmwhite(dba);
	    /* Calculate database's level */
	    level = set_cmptlvl(cmptlvl, minorlvl);
	    /* Add the database to the list */
	    add_dbname(dbname_tmp, dba, level, access_flags, locname, areaname,
	       cdbname_tmp, ddbname_tmp, FALSE, upcase, dbservice);
	}  /* end cursor loop to get db info from iidatabase */
	if (any_star)
	{
	    /* Do it again to get star databases.  Unfortunately it's not real
	    ** convenient to sort the cursor loop so that Star stuff comes at
	    ** the end, although it wouldn't be impossible (just clumsy).
	    */
/* # line 2434 "upgradedb.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"x",7658,16075);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select iidatabase.name, iidatabase.own, iidatabase.access, iidatabase\
.dbcmptlvl, iidatabase.dbservice, iidatabase.dbcmptminor, iidatabase.d\
bdev, iilocations.area from iidatabase, iilocations where(lowercase(ii\
database.own)!='$ingres' or lowercase(");
    IIwritio(0,(short *)0,1,32,0,(char *)
"iidatabase.name)not in('iidbdb', 'imadb'))and iidatabase.dbdev=iiloca\
tions.lname order by own, name");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"x",7658,16075);
  }
/* # line 2435 "upgradedb.sc" */	/* host code */
	    /* loop for each database in the installation */
	    for (;;)
	    {
/* # line 2438 "upgradedb.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"x",7658,16075,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,dbname_tmp);
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,dba);
      IIcsGetio((short *)0,1,30,4,&access_flags);
      IIcsGetio((short *)0,1,32,CMPT_LEN-1,cmptlvl);
      IIcsGetio((short *)0,1,30,4,&dbservice);
      IIcsGetio((short *)0,1,30,4,&minorlvl);
      IIcsGetio((short *)0,1,32,DU_LOC_SIZE + 1-1,locname);
      IIcsGetio((short *)0,1,32,DB_AREA_MAX + 1-1,areaname);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 2447 "upgradedb.sc" */	/* host code */
		if (sqlca.sqlcode == DU_SQL_NONE)
		{       
/* # line 2449 "upgradedb.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"x",7658,16075);
  }
/* # line 2450 "upgradedb.sc" */	/* host code */
		    break;
		}
		if (upg_dbg_out)
		{
		    SIprintf(
		    "]==> considering database %s owned by %s dbservice %d\n\n",
		    dbname_tmp, dba, dbservice);
		}
		/* Only star databases this time */
		if ((dbservice & (DU_1SER_DDB | DU_2SER_CDB))  == 0)
		    continue;
		/* illegal state of STAR DB, so SKIP this one but do not
		** generate an error */
		if ((dbservice & DU_1SER_DDB) && (dbservice & DU_2SER_CDB) )
		{
		    /* print error message that a db cannot be marked as 
		    ** both a CDB and a DDB.  Skip this DB */
		    du_talk ( DU_MODESENS, &upgr_cb, W_DU1852_CDB_AND_DDB, 2,
			      0, ddbname_tmp);
		    continue;
		}
		else if (dbservice & DU_1SER_DDB)
		{
		    /* get cdb info from iistar_cdbs */
/* # line 2474 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select cdb_name from iistar_cdbs where ddb_name=");
    IIputdomio((short *)0,1,32,0,dbname_tmp);
    IIwritio(0,(short *)0,1,32,0,(char *)"and ddb_owner=");
    IIputdomio((short *)0,1,32,0,dba);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,cdbname_tmp);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 2476 "upgradedb.sc" */	/* host code */
		    /* set up DBNAME_NODE structure correctly for DDB */
		    STzapblank(dbname_tmp, ddbname_tmp);
		    STzapblank(cdbname_tmp,dbname_tmp);
		    _VOID_ STtrmwhite (cdbname_tmp);
		}
		else
		{
		    /* this must be a CDB.  Currently, we dont operate on
		    ** CDBS directly.  Rather, we ask the user to operate
		    ** on the DDB.  Since this is an all statement, we simply
		    ** note that this db is a CDB that upgradedb is skipping,
		    ** and DO NOT consider this an error
		    */
		    /* get ddb name from iistar_cdbs */
/* # line 2490 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select ddb_name from iistar_cdbs where cdb_name=");
    IIputdomio((short *)0,1,32,0,dbname_tmp);
    IIwritio(0,(short *)0,1,32,0,(char *)"and cdb_owner=");
    IIputdomio((short *)0,1,32,0,dba);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,ddbname_tmp);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 2492 "upgradedb.sc" */	/* host code */
		    _VOID_ STtrmwhite (ddbname_tmp);
		    _VOID_ STtrmwhite (dbname_tmp);
		    /* now print warning that dbname_tmp is a CDB for STAR DB
		    ** ddbname_tmp and that we are skipping this one...
		    */
		    du_talk ( DU_MODESENS, &upgr_cb, W_DU1853_CDB_SKIPPED, 6,
				0, dbname_tmp, 0, ddbname_tmp, 0, dbname_tmp);
		    continue;
		}
		/* is it FIPS database */
		if (dbservice & DU_NAME_UPPER)
		  upcase = TRUE;
		else
		  upcase = FALSE;
		_VOID_ STtrmwhite(dbname_tmp);
		_VOID_ STtrmwhite(locname);
		_VOID_ STtrmwhite(areaname);
		_VOID_ STtrmwhite(dba);
		/* Calculate database's level */
		level = set_cmptlvl(cmptlvl, minorlvl);
		/* Add the database to the list */
		add_dbname(dbname_tmp, dba, level, access_flags, locname, areaname,
		   cdbname_tmp, ddbname_tmp, TRUE, upcase, dbservice);
	    }  /* end cursor loop to get db info from iidatabase */
	} /* any_star */
    }
    else					/* Collect specific database */
    do		    /* control loop */
    {
	if (upg_dbg_out)
	{
	    SIprintf("]==> NOT converting multiple\n");
	}
	if ((colon_ptr = STchr(dbname, ':')) != NULL)
	  STcopy(colon_ptr+2, dbname_curr);
/* # line 2527 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select iidatabase.name, iidatabase.own, iidatabase.access, iidatabase\
.dbservice, iidatabase.dbcmptlvl, iidatabase.dbcmptminor, iidatabase.d\
bdev, iilocations.area from iidatabase, iilocations where iidatabase.n\
ame=");
    IIputdomio((short *)0,1,32,0,dbname_curr);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and iidatabase.dbdev=iilocations.lname");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,dbname_tmp);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,dba);
      IIgetdomio((short *)0,1,30,4,&access_flags);
      IIgetdomio((short *)0,1,30,4,&dbservice);
      IIgetdomio((short *)0,1,32,CMPT_LEN-1,cmptlvl);
      IIgetdomio((short *)0,1,30,4,&minorlvl);
      IIgetdomio((short *)0,1,32,DU_LOC_SIZE + 1-1,locname);
      IIgetdomio((short *)0,1,32,DB_AREA_MAX + 1-1,areaname);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 2551 "upgradedb.sc" */	/* host code */
	/* Ensure the database actually exists */
	if (sqlca.sqlerrd[SQL_NUM_ROWS] == 0)
	{
	    du_talk (DU_ALWAYS, &upgr_cb, E_DU6012_NONEXISTENT_TARGET, 2,
		      0, dbname_curr);
	    upgr_exit(DU_FAIL_EXIT, 0);
	}
	if (colon_ptr != NULL)
	   STprintf(dbname_tmp, "%s::%s", vnode, dbname_curr);
	/* see if database is a star database */
	if (dbservice & (DU_1SER_DDB | DU_2SER_CDB) )
	{
	    if ((dbservice & DU_1SER_DDB) && (dbservice & DU_2SER_CDB) )
	    {
		/* illegal state of STAR DB, so SKIP this one but do not
		** generate an error */
		du_talk ( DU_MODESENS, &upgr_cb, W_DU1852_CDB_AND_DDB, 2,
			  0, ddbname_tmp);
		upgr_exit(DU_FAIL_EXIT, 0);
	    }
	    else if (dbservice & DU_1SER_DDB)
	    {
		/* get cdb info from iistar_cdbs */
/* # line 2574 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select cdb_name from iistar_cdbs where ddb_name=");
    IIputdomio((short *)0,1,32,0,dbname_tmp);
    IIwritio(0,(short *)0,1,32,0,(char *)"and ddb_owner=");
    IIputdomio((short *)0,1,32,0,dba);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,cdbname_tmp);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 2576 "upgradedb.sc" */	/* host code */
		/* set up DBNAME_NODE structure correctly for DDB */
		STzapblank(dbname_tmp, ddbname_tmp);
		STzapblank(cdbname_tmp,dbname_tmp);
		_VOID_ STtrmwhite (cdbname_tmp);
		star_flag = TRUE;		    
	    }
	    else
	    {
		/* this must be a CDB.  Currently, we dont operate on
		** CDBS directly.  Rather, we ask the user to operate
		** on the DDB.  Since this is an all statement, we simply
		** note that this db is a CDB that upgradedb is skipping,
		** and DO NOT consider this an error
		*/
		/* get ddb name from iistar_cdbs */
/* # line 2591 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select ddb_name from iistar_cdbs where cdb_name=");
    IIputdomio((short *)0,1,32,0,dbname_tmp);
    IIwritio(0,(short *)0,1,32,0,(char *)"and cdb_owner=");
    IIputdomio((short *)0,1,32,0,dba);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,ddbname_tmp);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 2593 "upgradedb.sc" */	/* host code */
		_VOID_ STtrmwhite (ddbname_tmp);
		_VOID_ STtrmwhite (dbname_tmp);
		/* now print warning that dbname_tmp is a CDB for STAR DB
		** ddbname_tmp and that we are skipping this one...
		*/
		du_talk ( DU_MODESENS, &upgr_cb, W_DU1853_CDB_SKIPPED, 6,
			    0, dbname_tmp, 0, ddbname_tmp, 0, dbname_tmp);
		upgr_exit(DU_FAIL_EXIT, 0);
	    }
	}  /* endif DB is marked as CDB or DDB */
	/* is it FIPS database */
	if (dbservice & DU_NAME_UPPER)
	   upcase = TRUE;
	else
	   upcase = FALSE;
	_VOID_ STtrmwhite(dbname_tmp);
	_VOID_ STtrmwhite(locname);
	_VOID_ STtrmwhite(areaname);
	_VOID_ STtrmwhite(dba);
	if ((superuser == 0) &&
	    (STcompare(dba, username) != 0))
	{
	    du_talk (DU_ALWAYS, &upgr_cb, E_DU6011_NOT_DBA, 4,
		      0, dbname_curr, 0, dba);
	    upgr_exit ( DU_FAIL_EXIT, 0);
	}
	/* Calculate database's level */
	level = set_cmptlvl(cmptlvl, minorlvl);
	/* Add the database to the list */
	add_dbname(dbname_tmp, dba, level, access_flags, locname, areaname, 
		   cdbname_tmp, ddbname_tmp, star_flag, upcase, dbservice);
    } while (0);
    /* commit to free lock on iidatabase */
/* # line 2626 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 2627 "upgradedb.sc" */	/* host code */
}
/*{
** Name: convert_dbs	- Perform conversion on list of databases collected
**
** Description:
**	This routine converts each database in the global list collected
**	by the collect_dbs() routine.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Returns:
**	OK		If all databases are upgraded.
**	FAIL		If any upgrade fails.
**
** Side Effects:
**	none
**
** History:
**	12-mar-2004 (somsa01)
**	    Split from collect_dbs() to allow multiple calls to collect_dbs().
**	27-Aug-2004 (schka24)
**	    Add r3 upgrade calls.  Remove journal check, server turns off
**	    journaling when it gets a conversion-connect.  Fix bug that
**	    could check for upgrade temps in the iidbdb session instead
**	    of the database session, leaving junk in the db..
*/
static STATUS
convert_dbs()
{
  char *dbname_curr;
  i4 access_flags;
  char *areaname;
  char iidbdbname[DB_MAXNAME+1];
  char dbname_star[DB_MAXNAME+DU_TAIL_LEN+1];
  char locdump[DU_LOC_SIZE + 1];
  char areadump[DB_AREA_MAX + 1];
  i4 proc_instruction;
    i4		    convert_count = 0;
    DU_ENV	    dbenv;
    char	    target_name[DB_MAXNAME+1];
    STATUS 	    status = OK;
    STATUS	    tree_status = OK;
    /* Pre-qualify each database for conversion */
    for  (upgr_cb.Dbname_ptr = upgr_cb.Dbname_head;
	  upgr_cb.Dbname_ptr != NULL;
	  upgr_cb.Dbname_ptr = upgr_cb.Dbname_ptr->nextdb)
    {
	/* do not process the iidbdb in this loop, as it will have already
	** been processed by routine cv_iidbdb */
	if ( STcompare(upgr_cb.Dbname_ptr->dbname, ERx("iidbdb")) == 0)
	    continue;
	if (upgr_cb.Dbname_ptr->star_flag)
	    STcopy(upgr_cb.Dbname_ptr->ddbname, target_name);
	else
	    STcopy(upgr_cb.Dbname_ptr->dbname, target_name);
	if (upgr_cb.Dbname_ptr->errors)
	{
	    du_talk (DU_MODESENS, &upgr_cb, W_DU1854_ERROR_SKIP, 2,
		    0, target_name);
            status = FAIL;
	    continue;
	}
	else
	    du_talk (DU_MODESENS, &upgr_cb, I_DU00B6_DB_IDENTIFIER, 2,
		     0, target_name);
	upgrade_iidd_stats = FALSE;
	/* Prequalify for FE conversion here.... */
	upgr_cb.Dbname_ptr->fe_convert = TRUE;		/* Always convert */
	if (upgr_cb.upgtrace[UPG03_SKIP_FECVT])
	{
	    du_talk (DU_MODESENS, &upgr_cb, W_DU1855_TRACE_3, 0);
	    upgr_cb.Dbname_ptr->fe_convert = FALSE;	/* skip upgradefe */
	}
	/* Prequalify for BE conversion here.... */
	upgr_cb.Dbname_ptr->be_convert = TRUE;		/* Assume conversion */
	if (upgr_cb.upgtrace[UPG04_SKIP_BECVT])
	{
	    du_talk (DU_MODESENS, &upgr_cb, W_DU1856_TRACE_4, 0);
	    upgr_cb.Dbname_ptr->be_convert = FALSE;	/* skip for star dbs */
	}
	if (((upgr_cb.Dbname_ptr->access & DU_OPERATIVE) == 0) &&
		 ((upgr_cb.Dbname_ptr->access & DU_UPGRADING) == 0)
	   )
	{
	    if (upgr_cb.Dbname_ptr->access & DU_CONVERTING)
	    {
		du_talk (DU_MODESENS, &upgr_cb, W_DU1857_UNCONVERTED_V5DB,
			 2, 0, target_name);
		upgr_cb.Dbname_ptr->errors = TRUE;
		status = FAIL;
	    }
	    else
	    {
		du_talk (DU_MODESENS, &upgr_cb, W_DU1858_INOPERATIVE_DB,
			 2, 0, target_name);
		upgr_cb.Dbname_ptr->errors = TRUE;
		status = FAIL;
	    }
	}
        if ( upgr_cb.Dbname_ptr->level == CURRENT_LEVEL ) /* lawst01   */
	{
	    /*
	    ** Don't do BE conversion on this database if at current level.
	    ** Still does FE conversion because BE version is recorded before
	    ** FE is upgraded.  There's no good way to tell whether FE was
	    ** upgraded beforehand, just do it twicet.
	    */
	    upgr_cb.Dbname_ptr->be_convert = FALSE;
	    upgr_cb.Dbname_ptr->fe_convert = TRUE;
	}
	else if (upgr_cb.Dbname_ptr->level == 0)
	{
	    du_talk (DU_MODESENS, &upgr_cb, W_DU1859_WRONG_LEVEL,
			 2, 0, target_name);
	    upgr_cb.Dbname_ptr->errors = TRUE;
	    status = FAIL;
	}
	/* At this point, we have completed prequalification.  Check result */
	if (upgr_cb.Dbname_ptr->errors)		/* Skip this db if errors */
	    continue;
	if (upgr_cb.Dbname_ptr->be_convert ||	/* See if anything to be done */
	    upgr_cb.Dbname_ptr->fe_convert ||
	    upgr_cb.Dbname_ptr->tree_convert)
	    ++convert_count;			/* Count the database */
	else					/* Nothing to be done */
	{
	    /*
	    ** Database was not selected for either BE or FE conversion.
	    ** Issue remainder of informational message and bypass this one.
	    */
	    du_talk (DU_MODESENS, &upgr_cb, W_DU185A_ALREADY_CURRENT,
			 4, 0, target_name, 0, SERVER_VERSION);
	    upgr_cb.Dbname_ptr->errors = TRUE;
	    continue;
	}
	/* Remainder of loop for be and/or fe conversion of a database */
	if (upgr_cb.Dbname_ptr->be_convert)
	{
	    du_talk (DU_MODESENS, &upgr_cb, I_DU008D_UPGRADE_TARGET, 2,
		     0, target_name);
            if (vnode[0] == '\0')
	    {
		/* This is probably not needed but can't hurt.  The attempt
		** to create a work dir will fail (silently) if we're not
		** the installation owner.
		*/
		if (upgr_cb.Dbname_ptr->star_flag)
		   chk_create_workdir(upgr_cb.Dbname_ptr->dbname);
		else
		   chk_create_workdir(target_name);
            }
	    /* Get info on dump area to delete dump config files.  Remember, 
	    ** this query will take a share-mode lock on iidatabase */
	    if (upg_dbg_out)
	    {
		SIprintf("]==> do back end convert\n");
	    }
/* # line 2790 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select iidatabase.dmpdev, iilocations.area from iidatabase, iilocatio\
ns where iidatabase.name='iidbdb' and iidatabase.dmpdev=iilocations.ln\
ame");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DU_LOC_SIZE + 1-1,locdump);
      IIgetdomio((short *)0,1,32,DB_AREA_MAX + 1-1,areadump);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 2795 "upgradedb.sc" */	/* host code */
	    if ((upgr_cb.Dbname_ptr->errors) ||
		(sqlca.sqlerrd[SQL_NUM_ROWS] == 0))
	    {
		du_talk (DU_MODESENS, &upgr_cb, W_DU185B_BAD_DUMP_INFO, 0);
		upgr_cb.Dbname_ptr->errors = TRUE;
		status = FAIL;
		--convert_count;
/* # line 2802 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 2803 "upgradedb.sc" */	/* host code */
		continue;
	    }
	    _VOID_ STcopy(locdump,  upgr_cb.Dbname_ptr->locdump);
	    _VOID_ STcopy(areadump, upgr_cb.Dbname_ptr->areadump);
	    /*
	    ** Open the target database.  
	    **
	    ** If there is an error opening this database, then skip to the
	    ** next database in the list
	    */
	    if (! open_target(CONNECT_EXCL))
	    {
		/*
		** switch back to the iidbdb connection before skiping
		** to next db
		*/
/* # line 2819 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DBDB_SESSIONID),(void *)IILQint(
    DBDB_SESSIONID));
  }
/* # line 2820 "upgradedb.sc" */	/* host code */
		status = FAIL;
		continue;
	    }
	    /* set session id to the open database */
	    if (upg_dbg_out)
	    {
		SIprintf("]==> (a) setting session to user database\n");
	    }
/* # line 2828 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DB_SESSIONID),(void *)IILQint(
    DB_SESSIONID));
  }
/* # line 2829 "upgradedb.sc" */	/* host code */
	    if (upg_dbg_out)
	    {
		SIprintf("]==> doing order-critical conversions, level %d\n",
			upgr_cb.Dbname_ptr->level);
	    }
	    /* Rehash core catalogs if server connect rewrote them */
	    order_critical_rehash(upgr_cb.Dbname_ptr->dbname,
				upgr_cb.Dbname_ptr->level);
	    if (upgr_cb.Dbname_ptr->level < V65_LEVEL)
	    {
	    	/* jpk: 6.5 order-critical conversion */
	    	order_critical_65(upgr_cb.Dbname_ptr->dbname,
				  upgr_cb.Dbname_ptr, 
				  upgr_cb.Dbname_ptr->upcase);
	    }
	    else if (upgr_cb.Dbname_ptr->level < V65ES_LEVEL)
	    {
		 order_critical_65es(upgr_cb.Dbname_ptr->dbname,
				     upgr_cb.Dbname_ptr->upcase);
	    }
	    /* 
	    ** Need to rebuild trees if < V65_LEVEL, V65ES_LEVEL, V20_LEVEL
	    ** also for 2.6 ports
	    */
	    /*
	    ** After the 65/65ES upgrade, upgrade the iirelation to V20_LEVEL
	    */
	    if (upgr_cb.Dbname_ptr->level < V20_LEVEL)
	    {
		order_critical_20(upgr_cb.Dbname_ptr->dbname,
				    upgr_cb.Dbname_ptr, 
				    upgr_cb.Dbname_ptr->upcase);
	    }
	    /* Order critical changes for r3 */
	    if (upgr_cb.Dbname_ptr->level < V30_LEVEL)
	    {
		order_critical_r3(upgr_cb.Dbname_ptr);
	    }
	    /* Order critical changes for r3.0.2 */
	    if (upgr_cb.Dbname_ptr->level < V302_LEVEL)
	    {
		order_critical_r302(upgr_cb.Dbname_ptr, NULL);
	    }
	    /* Get the status fields from iidatabase */
	    dbname_curr  = upgr_cb.Dbname_ptr->dbname;
	    access_flags = upgr_cb.Dbname_ptr->access;
	    areaname = upgr_cb.Dbname_ptr->areaname;
	    if (access_flags   & DU_OPERATIVE)
		access_flags  -= DU_OPERATIVE;
	    if (!(access_flags & DU_UPGRADING))
		access_flags  += DU_UPGRADING;
	    upgr_cb.Dbname_ptr->access = access_flags;
	    _VOID_ STcopy(upgr_cb.Dbname_ptr->dbname, dbenv.du_dbname);
	    _VOID_ STcopy(upgr_cb.Dbname_ptr->locname,
			  dbenv.du_dbloc.du_locname);
	    _VOID_ STcopy(upgr_cb.Dbname_ptr->areaname, dbenv.du_dbloc.du_area);
	    /* Remove vnode from database name 'ii_read_config_value'
	    ** checks the database directory exists, so we don't need 
	    ** the vnode (bug 74749)
	    */	
	    {
		char *tmp_dbname = NULL;
	    	if (((tmp_dbname = (STchr(dbname_curr, ':'))) != NULL) &&
	     	STchr(tmp_dbname+ 1, ':') == (tmp_dbname + 1))	
		    tmp_dbname += 2;
		if (tmp_dbname != NULL)
			dbname_curr = tmp_dbname;
	    }
	    dbenv.du_access = upgr_cb.Dbname_ptr->access;
	    /* Update the access field in the config file as inoperative */
	    proc_instruction = QEA_UPD_ACCESS;
	    if (upg_dbg_out)
	    {
		SIprintf("]==> [3] executing ii_update_config(%s, %s, %d, %d)\n",
		dbname_curr, areaname, proc_instruction, access_flags);
	    }
/* # line 2905 "upgradedb.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"ii_update_config");
    IILQprvProcValio((char *)"database_name",0,(short *)0,1,32,0,dbname_curr);
    IILQprvProcValio((char *)"location_area",0,(short *)0,1,32,0,areaname);
    IILQprvProcValio((char *)"update_map",0,(short *)0,1,30,4,&
    proc_instruction);
    IILQprvProcValio((char *)"access",0,(short *)0,1,30,4,&access_flags);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 2910 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk (DU_MODESENS, &upgr_cb, W_DU185E_BAD_CONFIG_UPDATE, 0);
		upgr_cb.Dbname_ptr->errors = TRUE;
		status = FAIL;
		--convert_count;
		/* if db is open, close it */
		if (upgr_cb.Connected)
		{
/* # line 2919 "upgradedb.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 2920 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 2921 "upgradedb.sc" */	/* host code */
			if (upg_dbg_out)
			{
			    SIprintf("]==> (f) disconnect\n");
			}
			upgr_cb.Connected = FALSE;
		}
		/* set session id back to iidbdb */
		if (upg_dbg_out)
		{
		    SIprintf("]==> (d) setting session to database database\n");
		}
/* # line 2932 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DBDB_SESSIONID),(void *)IILQint(
    DBDB_SESSIONID));
  }
/* # line 2933 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 2934 "upgradedb.sc" */	/* host code */
		continue;
	    }
	    /* Mark the database inoperative in the iidatabase catalog */
	    if (upg_dbg_out)
	    {
		SIprintf("]==> (e) setting session to database database\n");
	    }
/* # line 2941 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DBDB_SESSIONID),(void *)IILQint(
    DBDB_SESSIONID));
  }
/* # line 2942 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 2943 "upgradedb.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode on iidatabase where level=table");
    IIsyncup((char *)0,0);
  }
/* # line 2944 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iidatabase set access\
=");
    IIputdomio((short *)0,1,30,4,&access_flags);
    IIwritio(0,(short *)0,1,32,0,(char *)"where name=");
    IIputdomio((short *)0,1,32,0,dbname_curr);
    IIsyncup((char *)0,0);
  }
/* # line 2946 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 2947 "upgradedb.sc" */	/* host code */
	}
	else if (upgr_cb.Dbname_ptr->tree_convert)
	{
	    /* Just a tree convert */
	    du_talk (DU_MODESENS, &upgr_cb, I_DU008D_UPGRADE_TARGET, 2,
		     0, target_name);
	    /*
	    ** Open the target database.  
	    **
	    ** If there is an error opening this database, then skip to the
	    ** next database in the list
	    */
	    if (! open_target(CONNECT_EXCL))
	    {
		/*
		** switch back to the iidbdb connection before skiping
		** to next db
		*/
/* # line 2965 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DBDB_SESSIONID),(void *)IILQint(
    DBDB_SESSIONID));
  }
/* # line 2966 "upgradedb.sc" */	/* host code */
		status = FAIL;
		continue;
	    }
	    /* set session id to the open database */
	    if (upg_dbg_out)
	    {
		SIprintf("]==> (a) setting session to user database\n");
	    }
/* # line 2974 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DB_SESSIONID),(void *)IILQint(
    DB_SESSIONID));
  }
/* # line 2975 "upgradedb.sc" */	/* host code */
	}
	else if (upgr_cb.Dbname_ptr->fe_convert)
	{
	    /* identify database, since FE catalogs will be converted */
	    du_talk (DU_MODESENS, &upgr_cb, I_DU008D_UPGRADE_TARGET, 2,
		     0, target_name);
	}
	/* go upgrade the database */
	cv_upgrade();
	if (upgr_cb.Dbname_ptr->tree_error)
	    tree_status = FAIL;
        rm_iitmp_tables();        /* remove all iitmp_ tables */
        rm_iiqef_procedures();    /* Bug 89164: remove all iiQEF procedures */
	/* when returning from the routine that operates on a given db, it
	** is important to assure that SQL knows its back to the iidbdb
	** session 
	*/
	if (upgr_cb.Dbname_ptr->be_convert || upgr_cb.Dbname_ptr->tree_convert)
	    update_iidatabase();    /* go mark db as operative in the iidbdb */
/* # line 2994 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DBDB_SESSIONID),(void *)IILQint(
    DBDB_SESSIONID));
  }
/* # line 2995 "upgradedb.sc" */	/* host code */
        if (upgr_cb.Dbname_ptr->star_flag)
	{
/* # line 2997 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 2998 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 2999 "upgradedb.sc" */	/* host code */
	    if (upg_dbg_out)
	    {
		SIprintf("]==> (g) disconnect\n");
	    }
	    upgr_cb.dbdb_open = FALSE;
	    _VOID_ STpolycat (2, upgr_cb.Dbname_ptr->ddbname, DU_STARTAIL,
			dbname_star);
	    upgr_cb.Ignore_errors = TRUE;
/* # line 3007 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(STAR_SESSIONID);
    IIsqUser(DU_DBA_DBDB);
    IIsqConnect(0,dbname_star,(char *)"-A6",(char *)"+w",(char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 3009 "upgradedb.sc" */	/* host code */
	    upgr_cb.Ignore_errors = FALSE;
	    if (upg_dbg_out)
	    {
		SIprintf ("]==> connected to %s\n", dbname_star);
	    }
	    check_sql (sqlca.sqlcode);
	    if (sqlca.sqlcode < 0)   /* no star server */
		return FAIL;
	    IIlq_Protect (TRUE);         /* system catalog update privilege */
	    /*
	    ** If we updated iidd_stats in the local db, 
	    ** we have to re-register iistats in the distributed db
	    */
	    cv_65_iistats(dbname_star);
	    /* Drop what looks like an obsolete catalog (no error check) */
	    upgr_cb.Ignore_errors = TRUE;
/* # line 3025 "upgradedb.sc" */	/* remove */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"remove iiddb_finddbs");
    IIsyncup((char *)0,0);
  }
/* # line 3026 "upgradedb.sc" */	/* host code */
	    upgr_cb.Ignore_errors = FALSE;
/* # line 3027 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3028 "upgradedb.sc" */	/* host code */
	    /* We are connected to the distributed database now */
	    if (upgr_cb.Dbname_ptr->tree_convert || upg_trees)
		fix_qry_trees(upgr_cb.Dbname_ptr->ddbname, dbname_star, 
			TRUE, upgr_cb.Dbname_ptr->level); 
	    if (upgr_cb.Dbname_ptr->tree_error)
		tree_status = FAIL;
/* # line 3034 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3035 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 3036 "upgradedb.sc" */	/* host code */
	    if (upg_dbg_out)
	    {
	       SIprintf("]==> [4] about to connect to db iidbdb session %d -A6\n",
		  DBDB_SESSIONID);
	    }
	    if (vnode[0] != '\0')
	       STprintf(iidbdbname, "%s::iidbdb", vnode);
	    else
	       STcopy("iidbdb", iidbdbname);
	    if (upg_dbg_out)
	    {
		SIprintf("]==> [5] connect to iidbdb session %d options = %s\n",
			DBDB_SESSIONID, 
			upg_concurrent ? "+w" : "-A6 +w");
	    }
	    /* Reconnect the iidbdb session -- iidbdb is converted now,
	    ** no need for -A6 or other weird stuff.
	    ** Also, just connect as the running user, we'll say the magic
	    ** word to make iidatabase updates work.
	    */
/* # line 3056 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DBDB_SESSIONID);
    IIsqConnect(0,iidbdbname,(char *)"+w",(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 3058 "upgradedb.sc" */	/* host code */
	    if (upg_dbg_out)
	    {
		SIprintf("]==> connected\n");
	    }
	    if (sqlca.sqlcode < 0)
	       upgr_exit(DU_FAIL_EXIT, E_DU600D_BAD_IIDBDB_RECONNECT);
	    upgr_cb.dbdb_open = TRUE;
	    IIlq_Protect(TRUE);		/* system catalog update priv */
	}
    }  /* end loop for each database in potential target list */
    if (tree_status)
    {
	upgr_exit(DU_FAIL_EXIT, W_DU1873_UPGRADEDB_TREE);
    }
    return (status);
}
/*{
** Name: open_target	    - Open the target database
**
** Description:
**	    
**  This routine connects to the target database.  If there is an error 
**  connecting to the database, it verifies the db is still listed
**  in iidatabase and (if so) retries.  If the database is NOT listed, it 
**  prints a warning that the db has been destroyed during upgradedb execution.
**
**  Note: this routine connects as $ingres.  Upgradedb will do a
**		set session authorization xxxxx
**	  command to become the DBA later in the processing.  But the first
**	  few things upgradedb does requires that upgradedb connect as $ingres.
**
** Inputs:
**	none
**	
** Outputs:
**	none
** Returns:
**	TRUE	    -> database opened successfully
**	FALSE	    -> unable to open database
**	
** Side Effects:
**	none
**	
** History:
**	20-oct-92 (teresa)
**	    initial creation
**	20-jan-93 (jpk)
**	    changed "set user authorization" to "set session authorization"
**	    to comply with new standards.
**	11-Feb-1993 (daveb)
**	    make static to match declaration above.
**	11-Sep-2004 (schka24)
**	    Arrange for nicer message when reconnecting.
[@history_template@]...
*/
static bool
open_target(i4 open_mode)
{
  char dbname_curr[DB_MAXNAME+DU_TAIL_LEN+1];
  char dba_curr[DB_MAXNAME+1];
    i4 msgid;
    do		/* control loop */
    {
	/* Disconnect from the previous database if we are somehow conencted */
	if (upgr_cb.Connected)
	{
	    if (upg_dbg_out)
	    {
		SIprintf("]==> (f) setting session to user database\n");
	    }
/* # line 3130 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DB_SESSIONID),(void *)IILQint(
    DB_SESSIONID));
  }
/* # line 3131 "upgradedb.sc" */	/* host code */
	    if (upg_dbg_out)
	    {
		SIprintf("]==> disconnecting from session %d\n", DB_SESSIONID);
	    }
/* # line 3135 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3136 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 3137 "upgradedb.sc" */	/* host code */
	    if (upg_dbg_out)
	    {
		SIprintf("]==> (g) disconnect\n");
	    }
	    upgr_cb.Connected = FALSE;
	}
	if (upgr_cb.du_dbmstype == DU_DBRMS)
	    _VOID_ STpolycat(2, upgr_cb.Dbname_ptr->dbname, DU_RMSTAIL, 
			     dbname_curr);
	else
	    _VOID_ STcopy(upgr_cb.Dbname_ptr->dbname, dbname_curr);
	_VOID_ STcopy(upgr_cb.Dbname_ptr->dba, dba_curr);
	/* Connect to the target database */
	if (upgr_cb.Dbname_ptr->star_flag)
	    STcopy (upgr_cb.Dbname_ptr->ddbname, upgr_cb.du_opendb);
	else
	    STcopy (dbname_curr, upgr_cb.du_opendb);
	/* Be spiffy with the messages, different one first time */
	if (upgr_cb.Dbname_ptr->first_open)
	{
	    upgr_cb.Dbname_ptr->first_open = FALSE;
	    msgid = I_DU008E_USER_DB_CONNECT;
	}
	else
	{
	    msgid = I_DU0109_USER_DB_RECONNECT;
	}
	du_talk ( DU_MODESENS, &upgr_cb, msgid, 4,
		  0, upgr_cb.du_opendb, 0, dba_curr);
	if (upgr_cb.upgtrace[UPG06_DESTROYED])
	{
	    upgr_cb.this_trace = UPG06_DESTROYED;
	    /* simulate the db having gone away */
	    du_talk (DU_MODESENS, &upgr_cb, I_DU00B5_TRACE_POINT, 2, 
		     sizeof(upgr_cb.this_trace), &upgr_cb.this_trace);
	    STcopy("nonexistent_db",dbname_curr);
	}
	/* note: we connect as $ingres even though we tell the user we
	**	 are connecting as the DBA.  That is because we need to execute
	**	 some internal procedures that can only be run as $ingres, but
	**	 we will be setting user to the DBA for the mainline portion of
	**	 the conversion.
	*/
	if (upg_dbg_out)
	{
	    SIprintf("]==> [6] connect to %s session %d id %s %s +w\n",
		dbname_curr, DB_SESSIONID, DU_DBA_DBDB, 
		open_mode == CONNECT_EXCL ? "-A6 -l +U" : "");
	}
	if (open_mode == CONNECT_EXCL)
	{
/* # line 3188 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DB_SESSIONID);
    IIsqUser(DU_DBA_DBDB);
    IIsqConnect(0,dbname_curr,(char *)"-A6",(char *)"-l",(char *)"+U",(char *)"\
+w",(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 3191 "upgradedb.sc" */	/* host code */
	}
	else
	{
/* # line 3194 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DB_SESSIONID);
    IIsqUser(DU_DBA_DBDB);
    IIsqConnect(0,dbname_curr,(char *)"-A6",(char *)"+w",(char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 3197 "upgradedb.sc" */	/* host code */
	}
	if (upg_dbg_out)
	{
	    SIprintf("]==> connected to %s\n", dbname_curr);
	}
	check_sql(sqlca.sqlcode);
	if (sqlca.sqlcode < 0)
	{
	    /* see if db is still known to installation -- perhaps it
	    ** was destroyed after info about this db was cached */
            if (upg_dbg_out)
            {
                SIprintf("]==> Verifying db.....\n");
            }
	    if (validate_db(dbname_curr))
	    {
                if (upg_dbg_out)
                {
                    SIprintf("]==> db validated\n");
                }
		/* retry the connect */
		if (open_mode == CONNECT_EXCL)
		{
/* # line 3220 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DB_SESSIONID);
    IIsqUser(DU_DBA_DBDB);
    IIsqConnect(0,dbname_curr,(char *)"-A6",(char *)"-l",(char *)"+U",(char *)"\
+w",(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 3223 "upgradedb.sc" */	/* host code */
		}
		else
		{
/* # line 3226 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DB_SESSIONID);
    IIsqUser(DU_DBA_DBDB);
    IIsqConnect(0,dbname_curr,(char *)"-A6",(char *)"+w",(char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 3229 "upgradedb.sc" */	/* host code */
		}
		if (sqlca.sqlcode < 0)
		{   
		    if (upgr_cb.upgtrace[UPG06_DESTROYED])
		    {
			/* get the correct name back */
			STcopy(upgr_cb.Dbname_ptr->dbname, dbname_curr);
		    }		    
		    /* still failed, so print error */
		    du_talk ( DU_MODESENS, &upgr_cb, W_DU1860_SKIP_DB, 2,
			  0, upgr_cb.du_opendb);
		    break;
		}
		else
		{
		    upgr_cb.Connected = TRUE;
		}
	    }
	    else
	    {
		/* the database no longer exists, so print message and
		** break */
		du_talk (DU_MODESENS, &upgr_cb, W_DU1868_DESTROYED_DB, 2,
			  0, upgr_cb.du_opendb);
		break;
	    }
	}
	else
	{
	    upgr_cb.Connected = TRUE;
	}
	IIlq_Protect(TRUE);		/* system catalog update priv */
	setauth(NULL);			/* Forget last auth setting */
    } while (0);     /* end of control loop */
    return upgr_cb.Connected;
}
/*{
** Name: cv_upgrade	    - Upgrade the database
**
** Description:
**	    
**  This is the controller for converting a database.  It expects that the
**  database has already been opened. It performs backend and frontend 
**  conversion.  
**
** Inputs:
**	none
**	
** Outputs:
**	none
**	
** Side Effects:
**	none
**	
** History:
**	24-jul-91 (teresa)
**	    initial creation
**	16-apr-92 (teresa)
**	    change "for" control loop to "do" to appease sun4 compiler warnings.
**	19-Oct-1993 (jpk)
**	    go back to area name II_DATABASE instead of "default";
**	    Rachael's fix to internal database procedure makes
**	    this the correct parameter to pass it.
**	14-jul-94 (robf)
**          Moved Converting message here.
**	21-Feb-96 (prida01)
**	    Remove vnode part of database name  (74749)
**	31-may-96 (prida01)
**	    Modify vnode change to work for local databases.
**	11-Sep-2004 (schka24)
**	    Fix catalogs to large page size after upgrade if they were before.
**	11-Jan-2005 (schka24)
**	    cmptlvl was too small by 1, smashed areaname -- how did that work?
**	18-Sep-2006 (gupsh01)
**	    Added update_date_alias() to support aliasing Date keyword to 
**	    ingresdate data type for earlier installations.
**	25-Sep-2006 (gupsh01)
**	    Removed update_date_alias() as it is done by iisudbms.
[@history_template@]...
*/
static VOID
cv_upgrade()
{
  i4 proc_instruction;
  char dbname_curr[DB_MAXNAME+DU_TAIL_LEN+1], *dbname_ptr;
  char dba_curr[DB_MAXNAME+1];
  char areaname[DB_AREA_MAX + 1];
  char cmptlvl[CMPT_LEN];
  i4 minorlvl;
  i4 page_size;
  i4 cversion;
    i4		access_flags;
    DU_ENV	dbenv;
    do		/* control loop */
    {
	if (upgr_cb.du_dbmstype == DU_DBRMS)
	    _VOID_ STpolycat(2, upgr_cb.Dbname_ptr->dbname, DU_RMSTAIL, 
			     dbname_curr);
	else
	    _VOID_ STcopy(upgr_cb.Dbname_ptr->dbname, dbname_curr);
	_VOID_ STcopy(upgr_cb.Dbname_ptr->dba, dba_curr);
	/* Create a pointer to database name that doesn't include vnode */
	{
	    char *tmp_dbname;
	    	if (((tmp_dbname = (STchr(dbname_curr, ':'))) != NULL) &&
	   	STchr(tmp_dbname + 1, ':') == (tmp_dbname + 1))
	           tmp_dbname += 2;
	    if (tmp_dbname != NULL)
		dbname_ptr = tmp_dbname;
	    else
		dbname_ptr = dbname_curr;
	}
	/* Delete the dump config file if one exists */
	if (upgr_cb.Dbname_ptr->be_convert)
	{
	    _VOID_ STcopy(upgr_cb.Dbname_ptr->dbname, dbenv.du_dbname);
	    _VOID_ STcopy(upgr_cb.Dbname_ptr->locdump,
			  dbenv.du_dbloc.du_locname);
	    _VOID_ STcopy(upgr_cb.Dbname_ptr->areadump,
			  dbenv.du_dbloc.du_area);
	    _VOID_ STcopy(upgr_cb.Dbname_ptr->areadump, areaname);
            /* We will only delete configuration files in the dump directory
            ** if this is a 6.1 or 6.2 database
            */
            if ( upgr_cb.Dbname_ptr->level <= V62_LEVEL )
            {
		if (upg_dbg_out)
		{
		    SIprintf("]==> executing ii_del_dmp_config (%s, %s)\n",
			dbname_curr, areaname);
		}
/* # line 3362 "upgradedb.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"ii_del_dmp_config");
    IILQprvProcValio((char *)"database_name",0,(short *)0,1,32,0,dbname_curr);
    IILQprvProcValio((char *)"location_area",0,(short *)0,1,32,0,areaname);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 3365 "upgradedb.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
	        {
		    du_talk ( DU_MODESENS, &upgr_cb, W_DU185F_BAD_DELETE,
			      2, 0, upgr_cb.Dbname_ptr->dbname);
		    upgr_cb.Dbname_ptr->errors = TRUE;
		    break;
	        }
	    }
	}
	/* Perform be conversion */
	if (upgr_cb.Dbname_ptr->be_convert)
	{
	    do	    /* inner control loop */
	    {
		/* set the user to the DBA for the BE conversion */
/* # line 3380 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3381 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed((char *)"set session authorization $dba");
    IIsyncup((char *)0,0);
  }
/* # line 3382 "upgradedb.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
		    /* report error setting name and abort */
		    du_talk ( DU_MODESENS, &upgr_cb, W_DU1860_SKIP_DB, 2,
			  0, upgr_cb.du_opendb);
		    break;
		}
	        if (upgr_cb.Dbname_ptr != NULL)
	        {
		    if (upgr_cb.Dbname_ptr->star_flag)
		        du_talk (DU_MODESENS, &upgr_cb, I_DU0088_UPGRADE_ID, 4,
			     0, upgr_cb.Dbname_ptr->ddbname, 0, SERVER_VERSION);
		    else
		        du_talk (DU_MODESENS, &upgr_cb, I_DU0088_UPGRADE_ID, 4,
			     0, dbname_curr, 0, SERVER_VERSION);
	        }
		/* now convert the currently connected database */
		cv_database(dbname_curr);
		/* Check for errors during conversion */
		if (upgr_cb.Dbname_ptr->errors == TRUE)
		{
/* # line 3403 "upgradedb.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 3404 "upgradedb.sc" */	/* host code */
		    break;
		}
		else
		{
/* # line 3408 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3409 "upgradedb.sc" */	/* host code */
		}
		/* Update the compat fields in the config file */
		_VOID_ STcopy(upgr_cb.Dbname_ptr->dbname, dbenv.du_dbname);
		_VOID_ STcopy(upgr_cb.Dbname_ptr->locname,  
			      dbenv.du_dbloc.du_locname);
		_VOID_ STcopy(upgr_cb.Dbname_ptr->areaname, 
			      areaname);
		_VOID_ STcopy(DU_CUR_DBCMPTLVL, cmptlvl);
		minorlvl = DU_1CUR_DBCMPTMINOR ;
		access_flags = upgr_cb.Dbname_ptr->access;
		if (!(access_flags & DU_OPERATIVE))
		    access_flags += DU_OPERATIVE;
		if (access_flags & DU_UPGRADING)
		    access_flags -= DU_UPGRADING;
		dbenv.du_access = access_flags;
		upgr_cb.Dbname_ptr->access = access_flags;
		/* go mark db as operative in the iidbdb */
	        if (upgr_cb.Dbname_ptr->be_convert || 
		    upgr_cb.Dbname_ptr->tree_convert)
		    update_iidatabase();
		if (! open_target(CONNECT_EXCL))
		{
		    SIprintf("Error re-opening database %s \n", 
		    dbname_ptr);
		    continue;
		}
/* # line 3435 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DB_SESSIONID),(void *)IILQint(
    DB_SESSIONID));
  }
/* # line 3436 "upgradedb.sc" */	/* host code */
		/* the the use back to $ingres to execute internal procedures */
/* # line 3437 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed((char *)"set session authorization $ingres");
    IIsyncup((char *)0,0);
  }
/* # line 3438 "upgradedb.sc" */	/* host code */
		/* Now update the access and compat fields of the config file */
		proc_instruction = QEA_UPD_ACCESS | QEA_UPD_CMPTLVL | 
				   QEA_UPD_CMPTMINOR;
		cversion = DSC_VCURRENT;
		/* Only update core-catalog version if we came from r3.
		** DMF's core-catalog rewriter takes catalogs to vcurrent,
		** but it doesn't run if db was r3 to start with.
		*/
		if (upgr_cb.Dbname_ptr->level == V30_LEVEL)
		    proc_instruction |= QEA_UPD_CVERSION;
		if (upg_dbg_out)
		{
		    SIprintf("]==> [2] ii_update_config(%s, %s, %d, %d, %d, %d)\n",
		    dbname_curr, areaname, proc_instruction, access_flags,
		    cmptlvl, minorlvl);
		}
                /*
                ** for RMS, we need to use the name without "/rms". 
                ** Strip it and add back later.
                */
                if( upgr_cb.du_dbmstype == DU_DBRMS )
                {
                    char *tmp = STindex(dbname_ptr, "/", STlength(dbname_ptr));
                    *tmp = '\0';
                }
/* # line 3463 "upgradedb.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"ii_update_config");
    IILQprvProcValio((char *)"database_name",0,(short *)0,1,32,0,dbname_ptr);
    IILQprvProcValio((char *)"location_area",0,(short *)0,1,32,0,areaname);
    IILQprvProcValio((char *)"update_map",0,(short *)0,1,30,4,&
    proc_instruction);
    IILQprvProcValio((char *)"access",0,(short *)0,1,30,4,&access_flags);
    IILQprvProcValio((char *)"cmptlvl",0,(short *)0,1,32,0,cmptlvl);
    IILQprvProcValio((char *)"cmptminor",0,(short *)0,1,30,4,&minorlvl);
    IILQprvProcValio((char *)"cversion",0,(short *)0,1,30,4,&cversion);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 3471 "upgradedb.sc" */	/* host code */
                if( upgr_cb.du_dbmstype == DU_DBRMS )
                    STcat( dbname_ptr, DU_RMSTAIL ); 
		if (sqlca.sqlcode < 0)
		{
		    du_talk ( DU_MODESENS, &upgr_cb, W_DU1862_CNF_UPDATE_FAIL,
			      2, 0, upgr_cb.du_opendb);
		    upgr_cb.Dbname_ptr->errors = TRUE;
		}
		/*
		** We must commit now, disconnect and reconnect
		** or we will not be able to access "dmpdev" in iidatabase.
		*/
		/* Disconnect from target database */
		if (upgr_cb.Connected)
		{
		    if (upg_dbg_out)
		    {
			SIprintf("]==> (h) setting session to user database\n");
		    }
/* # line 3490 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DB_SESSIONID),(void *)IILQint(
    DB_SESSIONID));
  }
/* # line 3491 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3492 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 3493 "upgradedb.sc" */	/* host code */
		    upgr_cb.Connected = FALSE;
		    if (upg_dbg_out)
		    {
		        SIprintf("]==> (b) disconnect user database\n");
		    }
		}
		if (! open_target(CONNECT_EXCL))
		{
		    SIprintf("Error re-opening database %s \n", 
		    dbname_ptr);
		    continue;
		}
/* # line 3505 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DB_SESSIONID),(void *)IILQint(
    DB_SESSIONID));
  }
/* # line 3506 "upgradedb.sc" */	/* host code */
		/* If the database was created with large-page
		** catalogs, remodify the catalogs.  The modifier
		** utility will only redo the ones that are the wrong
		** page size.
		** We'll assume that the core catalog(s) are always the
		** proper page size, as they are handled in the server.
		*/
		if (! upgr_cb.Dbname_ptr->star_flag)
		{
/* # line 3515 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relpgsize from iirelation where lowercase(relid)='iirelation' \
and lowercase(relowner)='$ingres'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&page_size);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 3520 "upgradedb.sc" */	/* host code */
		    check_sql(sqlca.sqlcode);
		    if (sqlca.sqlcode >= 0 && page_size > 2048)
		    {
			/* Make utility babble away */
			upgr_cb.du_msg->du_flags &= ~DU_SAVEMSG;
			(void) duc_modify_pagesize(page_size, FALSE,
				upgr_cb.du_msg);
			/* Ignore errors, this is a nice-to-do step. */
			/* Routine turns off upd-cat, turn back on */
			IIlq_Protect(TRUE);
			upgr_cb.du_msg->du_flags |= DU_SAVEMSG;
		    }
		}
	    } while (0); /* end of control loop */
	} /* be conversion */
	/* Fix up query trees if needed */
	if (upgr_cb.Dbname_ptr->tree_convert || upg_trees)
	{
	    char    dbname_curr[DB_MAXNAME+DU_TAIL_LEN+1];
/* # line 3539 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3540 "upgradedb.sc" */	/* host code */
	    /* If this is star db, we're connecting to local database now */
	    if (upgr_cb.du_dbmstype == DU_DBRMS)
		_VOID_ STpolycat(2, upgr_cb.Dbname_ptr->dbname, DU_RMSTAIL, 
				 dbname_curr);
	    else
		_VOID_ STcopy(upgr_cb.Dbname_ptr->dbname, dbname_curr);
	    fix_qry_trees(upgr_cb.Dbname_ptr->dbname, dbname_curr, 
		    FALSE, upgr_cb.Dbname_ptr->level); 
/* # line 3548 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DB_SESSIONID),(void *)IILQint(
    DB_SESSIONID));
  }
/* # line 3549 "upgradedb.sc" */	/* host code */
	    IIlq_Protect (TRUE);         /* system catalog update privilege */
	}
	else if (upgr_cb.Dbname_ptr->level == V30_LEVEL)
	{
	    /* Query tree update not needed, if db was an r3 database
	    ** update iicolumns to include the attcollid column.
	    */
/* # line 3556 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop view iicolumns");
    IIsyncup((char *)0,0);
  }
/* # line 3557 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
			    0, "iicolumns", 0, dbenv.du_dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	    (void) duc_create_specific_stdcat_view(dbenv.du_dbname, "iicolumns",
			upgr_cb.Dbname_ptr->upcase);
/* # line 3565 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop view iiphysical_columns");
    IIsyncup((char *)0,0);
  }
/* # line 3566 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
			    0, "iiphysical_columns", 0, dbenv.du_dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	    (void) duc_create_specific_stdcat_view(dbenv.du_dbname, "iiphysical_columns",
			upgr_cb.Dbname_ptr->upcase);
	}
	/* Now perform fe conversion */
	if ((upgr_cb.Dbname_ptr->fe_convert) &&
	    !(upgr_cb.Dbname_ptr->errors))
	{
	    /* May not be connected if no BE or tree convert.
	    ** Open the db now, don't be looking at iidbdb here!
	    */
	    if (! upgr_cb.Connected)
	    {
		if (! open_target(CONNECT_EXCL))
		{
		    SIprintf("Error re-opening database %s \n", dbname_ptr);
		    break;
		}
	    }
	    /* set user to DBA for FE convert */
/* # line 3591 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DB_SESSIONID),(void *)IILQint(
    DB_SESSIONID));
  }
/* # line 3592 "upgradedb.sc" */	/* host code */
	    IIlq_Protect (TRUE);         /* system catalog update privilege */
/* # line 3593 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3594 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed((char *)"set session authorization $dba");
    IIsyncup((char *)0,0);
  }
/* # line 3595 "upgradedb.sc" */	/* host code */
	    fe_dictinst(dbname_curr, dba_curr, upgr_cb.Dbname_ptr->level);
	    /* set user back to $ingres */
/* # line 3597 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3598 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed((char *)"set session authorization $ingres");
    IIsyncup((char *)0,0);
  }
/* # line 3599 "upgradedb.sc" */	/* host code */
	}
    } while (0);     /* end of control loop */
    /* Issue message to indicate whether or not the database was converted */
    if (upgr_cb.Dbname_ptr->be_convert)
    {    
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    du_talk (DU_MODESENS, &upgr_cb, I_DU008F_NOT_UPGRADED, 4, 
		      0, upgr_cb.du_opendb, 0, SERVER_VERSION);
	else
	    du_talk (DU_MODESENS, &upgr_cb, I_DU0090_IS_UPGRADED, 4, 
		      0, upgr_cb.du_opendb, 0, SERVER_VERSION);
    }
}
/*{
** Name: update_iidatabase()	-  Changes compat level and access codes in 
**				   iidatabase
**
** Description:
**	    This routine marks the database as operative and at the current
**	    level.  It switches from the user session to the iidbdb session.
**	    It does a commit after the update so that the shared lock taken
**	    on iidatabase is released as soon as we are done with it.
**	    
** Inputs:
**	none
**
** Outputs:
**	none
**
** Side Effects:
**	none
**
** History:
**	24-jul-91 (teresa)
**	    initial creation to update db's access in iidatabase.
**	17-dec-91 (teresa)
**	    Moved the set_sql staetment out of the control loop so that it is
**          always executed and this routine always returns with session set
**	    to the iidbdb session.
**	16-apr-92 (teresa)
**	    change "for" control loop to "do" to appease sun4 compiler warnings.
**	21-Feb-96 (prida01)
**	    Remove vnode part of database name (747490 
[@history_template@]...
*/
static VOID
update_iidatabase()
{
  char *dbname_curr;
  char *dbname_star;
  i4 access_flags;
    if (upg_dbg_out)
    {
	SIprintf("]==> (g) setting session to database database\n");
    }
/* # line 3656 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DBDB_SESSIONID),(void *)IILQint(
    DBDB_SESSIONID));
  }
/* # line 3657 "upgradedb.sc" */	/* host code */
    do		/* control loop */
    {
	if (upgr_cb.Dbname_ptr->errors)
	    break;
	dbname_curr = upgr_cb.Dbname_ptr->dbname;
	if (upgr_cb.Dbname_ptr->star_flag)
	    dbname_star = upgr_cb.Dbname_ptr->ddbname;
	/* Remove vnode from database name 74749*/
	{
	    char *tmp_dbname;
	    if (((tmp_dbname = (STchr(dbname_curr, ':'))) != NULL) &&
	    	STchr(tmp_dbname+ 1, ':') == (tmp_dbname + 1))
	    	    tmp_dbname += 2;
	    if (tmp_dbname != NULL)
		dbname_curr = tmp_dbname;
	}
	access_flags = upgr_cb.Dbname_ptr->access;
	if (upgr_cb.Dbname_ptr->tree_convert)
	{
	    fix_dbservice(dbname_curr);
	    if (upgr_cb.Dbname_ptr->star_flag)
		fix_dbservice(dbname_star);
	}
	/* Update the iidatabase entry for the converted database */
	if (upgr_cb.Dbname_ptr->be_convert)
	{
/* # line 3683 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3684 "upgradedb.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode on iidatabase where level=table");
    IIsyncup((char *)0,0);
  }
/* # line 3685 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iidatabase set dbcmpt\
lvl=");
    IIputdomio((short *)0,1,32,4,DU_CUR_DBCMPTLVL);
    IIwritio(0,(short *)0,1,32,0,(char *)", dbcmptminor=");
    IIputdomio((short *)0,1,30,sizeof(DU_1CUR_DBCMPTMINOR),(void *)IILQint(
    DU_1CUR_DBCMPTMINOR));
    IIwritio(0,(short *)0,1,32,0,(char *)", access=");
    IIputdomio((short *)0,1,30,4,&access_flags);
    IIwritio(0,(short *)0,1,32,0,(char *)"where name=");
    IIputdomio((short *)0,1,32,0,dbname_curr);
    IIsyncup((char *)0,0);
  }
/* # line 3690 "upgradedb.sc" */	/* host code */
	    /* If this is a STAR DB, update the DDB entry as well */
	    if (upgr_cb.Dbname_ptr->star_flag)
	    {
/* # line 3693 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3694 "upgradedb.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode on iidatabase where level=table");
    IIsyncup((char *)0,0);
  }
/* # line 3695 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iidatabase set dbcmpt\
lvl=");
    IIputdomio((short *)0,1,32,4,DU_CUR_DBCMPTLVL);
    IIwritio(0,(short *)0,1,32,0,(char *)", dbcmptminor=");
    IIputdomio((short *)0,1,30,sizeof(DU_1CUR_DBCMPTMINOR),(void *)IILQint(
    DU_1CUR_DBCMPTMINOR));
    IIwritio(0,(short *)0,1,32,0,(char *)", access=");
    IIputdomio((short *)0,1,30,4,&access_flags);
    IIwritio(0,(short *)0,1,32,0,(char *)"where name=");
    IIputdomio((short *)0,1,32,0,dbname_star);
    IIsyncup((char *)0,0);
  }
/* # line 3700 "upgradedb.sc" */	/* host code */
	    }
/* # line 3701 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3702 "upgradedb.sc" */	/* host code */
	}
    } while (0); /* end of control loop */
    /* Disconnect from target database */
    if (upgr_cb.Connected)
    {
	if (upg_dbg_out)
	{
	    SIprintf("]==> (h) setting session to user database\n");
	}
/* # line 3711 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DB_SESSIONID),(void *)IILQint(
    DB_SESSIONID));
  }
/* # line 3712 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3713 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 3714 "upgradedb.sc" */	/* host code */
	upgr_cb.Connected = FALSE;
    }
}
/*{
** Name: cv_database	- Convert non-iidbdb-only catalogs to latest format
**
** Description:
**	This routine just calls all the conversion functions necessary
**	to convert the currently connected database's non-dbdb-only
**	catalogs to latest format.
**
** Inputs:
**	dbname_curr		Name of the currently open database.
**	
** Outputs:
**	none
**	
** History:
**      07-jun-89 (ralph)
**          Initial creation.
**	09-aug-89 (ralph)
**	    Added call to cv_sequence()
**	2-oct-90 (pete)
**	    Removed call to cv_sequence()
**	10-dec-90 (neil)
**	    Alerters: introduce call to cv_event() *before* cv_protect() so
**	    that iipermits may reference the [new] iievent table.
**	1-feb-1991 (bryanp)
**	    Btree index compression: call cv_tables(), cv_indexes(), and
**	    cv_physical_tables() to update standard catalogs for new index
**	    compression information.
**	    Call cv_dbconstants to convert the iidbconstants view.
**	    Call cv_index_columns to convert the iiindex_columns view.
**	03-may-91 (teresa)
**	    add support for STAR.
**	07-25-91 (teresa)
**	    bring up to coding standards
**	16-apr-92 (teresa)
**	    change "for" control loop to "do" to appease sun4 compiler warnings.
**	05-oct-92 (jpk)
**	    Remove references to MSB of relstat, cv_relstat() routine.
**	1-feb-93 (jpk)
**	    Skip routines that convert to 6.4 if database already 6.4.
**	    Otherwise some needless conversion will not be harmless.
**	    E.g. if you drop and recreate iipermits, it will try to
**	    define a view on iiprotect, which may not yet have
**	    the 6.5 columns that the view is being defined on.
**	28-March-1994 (jpk)
**	    Add call to create database procedures that are new for 6.5.
**	8-jul-94 (robf)
**          Update to 65ES levels
**	14-jul-94 (robf)
**          Took out Converting... message and moved into
**          cv_upgrade() so people upgrading iidbdb didn't  get two messages
**      06-mar-1996 (nanpr01)
**          Added upgrade code for 2.0 upgrade - Variable Page Size Project. 
**      12-apr-1999 (nanpr01)
**          Fixed core catalogs that are created by 2.0 createdb to fix the
**	    attintl_id field.
**	17-nov-2000 (somsa01)
**	    Added running of cv_25_iirule().
**      31-Oct-2003 (wanfr01)
**          Bug 110858, INGSRV 2493
**          Recreate select permits to public on system catalog views
**          after they are dropped and recreated.
**	25-Aug-2004 (schka24)
**	    Update dbcapabilities for any non-current level;
**	    remove attintl update, done by core catalog stuff in server now.
**	    Add r3 update calls.
**	23-Jan-2006 (kschendel)
**	    Addition of B1 security dummy columns to SCI views requires
**	    that we drop/recreate stdcat views for pre-9.0.4 db's.
**	22-May-2006 (kschendel)
**	    Drop security-alarms catalog for iidbdb, since it has to be
**	    created in a special iidbdb-ish way.  Let it get recreated
**	    by the iidbdb code, or by fix qry trees if we go that way.
**	    Also, don't do stdcat grants for iidbdb yet, we haven't recreated
**	    all of the iidbdb stdcat views.
[@history_template@]...
*/
static VOID
cv_database(dbname_curr)
char	*dbname_curr;
{
    bool is_iidbdb;
    DU_ERROR    errcb;
    if (upg_dbg_out)
    {
	SIprintf("]==> in cv_database (%s)\n", dbname_curr);
    }
    is_iidbdb = TRUE;
    if (STcompare(dbname_curr, DU_DBDBNAME) != 0)	/* if user db */
    {
	is_iidbdb = FALSE;
	if (upg_dbg_out)
	{
	    SIprintf("]==> (i) setting session to user database\n");
	}
/* # line 3812 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DB_SESSIONID),(void *)IILQint(
    DB_SESSIONID));
  }
/* # line 3813 "upgradedb.sc" */	/* host code */
    }
    /* Make sure all privileges are active */
/* # line 3815 "upgradedb.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session with privileges=\
all");
    IIsyncup((char *)0,0);
  }
/* # line 3816 "upgradedb.sc" */	/* host code */
    /* Call routines to convert specific areas */
    do		    /* control loop */
    {
	if (upgr_cb.Dbname_ptr->level < V904_LEVEL)
	{
	    /* drop all standard catalog views */
	    du_talk(DU_MODESENS, &upgr_cb, I_DU0102_DROPPING_STDCAT_VIEWS, 0);
	    duc_drop_stdcat_views(dbname_curr, upgr_cb.Dbname_ptr->star_flag);
	}
	if (upgr_cb.Dbname_ptr->level < V30_LEVEL)
	{
	    /* do protect first because of relstat problem if you do
	    ** any other catalog first.
	    ** (kschendel) I don't know what that means, but it's true
	    ** that iiprotect is very nearly a core catalog and needs
	    ** to be fixed up first.
	    */
            cv_protect();
	}
        if ( upgr_cb.Dbname_ptr->errors == TRUE )
		break;
	if (upgr_cb.Dbname_ptr->level < V64_LEVEL)
	{
	    cv_event(dbname_curr);
	    if ( upgr_cb.Dbname_ptr->errors == TRUE )
		break;
	    cv_rule();
	    if (upgr_cb.upgtrace[UPG07_ABORT])
	    {
		/* this trace point causes upgradedb to abort in the middle of 
		** processing */
		upgr_cb.this_trace = UPG07_ABORT;
		/* force an abort in the middle */
		du_talk (DU_MODESENS, &upgr_cb, I_DU00B5_TRACE_POINT, 2, 
			sizeof(upgr_cb.this_trace), &upgr_cb.this_trace);
		if (upgr_cb.dbdb_cvt)
		{
		    /* abort upgrade of the iidbdb */
		    upgr_cb.Iidbdbok = FALSE;
		}
		/* abort user and/or iidbdb upgrade*/
		upgr_cb.Dbname_ptr->errors = TRUE;
	    }
	    if ( upgr_cb.Dbname_ptr->errors == TRUE )
		break;
	}
	/*
	** dbcapabilities need to be updated in most every release
	*/
	if (upgr_cb.Dbname_ptr->level < CURRENT_LEVEL)
	{
	    cv_dbcap();		
	    if ( upgr_cb.Dbname_ptr->errors == TRUE )
		break;
/* # line 3870 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3871 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed((char *)"set session authorization $ingres");
    IIsyncup((char *)0,0);
  }
/* # line 3872 "upgradedb.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session with privileges=\
all");
    IIsyncup((char *)0,0);
  }
/* # line 3873 "upgradedb.sc" */	/* host code */
	    IIlq_Protect(TRUE);
/* # line 3874 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3875 "upgradedb.sc" */	/* host code */
	}
	if (upg_dbg_out)
	{
	    SIprintf("]==> are we pre-65? compare %d with %d\n",
		upgr_cb.Dbname_ptr->level, V65_LEVEL);
	}
	if (upgr_cb.Dbname_ptr->level < V65_LEVEL)
	{
	    if (upg_dbg_out)
	    {
		SIprintf("]==> yes, we are pre-65 \n");
		SIprintf("\n]==> *** DOING 65 UPDATES *** \n");
	    }
	    /* pre-65, so first rebuild system catalogs changing in 6.5 */
/* # line 3889 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3890 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed((char *)"set session authorization $ingres");
    IIsyncup((char *)0,0);
  }
/* # line 3891 "upgradedb.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session with privileges=\
all");
    IIsyncup((char *)0,0);
  }
/* # line 3892 "upgradedb.sc" */	/* host code */
	    IIlq_Protect(TRUE);
	    /* build 'new' system catalogs required for every 6.5 db */
	    new_for_65_syscats(dbname_curr);
	    changed_for_65_syscats(dbname_curr);
	    if ( upgr_cb.Dbname_ptr->errors == TRUE )
		break;
	    /* Create schemas */
	    populate_iischema();
	    IIlq_Protect(TRUE);
	    if (!rel_exists("iigw06_relation"))
	    {
		/* Standard Catalogs: C2-SXA catalogs */
		MEfill ( sizeof(errcb), '\0', (PTR)&errcb );
		du_talk(DU_MODESENS, &upgr_cb, I_DU00CD_C2SXA_CRE, 0);
		duc_iigwsxa(&errcb); /* build iigw06_relation/attribute etc */
	    }
	    file_extend_convert_user();
	    IIlq_Protect(TRUE);
	}
	if (upg_dbg_out)
	{
	    SIprintf("]==> are we pre-65 ES? compare %d with %d\n",
		upgr_cb.Dbname_ptr->level, V65ES_LEVEL);
	}
	if (upgr_cb.Dbname_ptr->level < V65ES_LEVEL)
	{
	    if (upg_dbg_out)
	    {
		SIprintf("]==> yes, we are pre-65 ES\n");
		SIprintf("\n]==> *** DOING 65 ES UPDATES *** \n");
	    }
	    /* pre-65 ES, so first rebuild system catalogs changing in 6.5 ES*/
/* # line 3924 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3925 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed((char *)"set session authorization $ingres");
    IIsyncup((char *)0,0);
  }
/* # line 3926 "upgradedb.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session with privileges=\
all");
    IIsyncup((char *)0,0);
  }
/* # line 3927 "upgradedb.sc" */	/* host code */
	    IIlq_Protect(TRUE);
	    /* "changed syscats" superseded by r3 */
	    /* build 'new' system catalogs required for every 6.5 ES db */
	    new_for_65es_syscats(dbname_curr); 
	    if ( upgr_cb.Dbname_ptr->errors == TRUE )
		break;
	    IIlq_Protect(TRUE);
	    new_for_65es_stdcats(dbname_curr);
	    if ( upgr_cb.Dbname_ptr->errors == TRUE )
		break;
	    IIlq_Protect(TRUE);
	}
	else if (upgr_cb.Dbname_ptr->level < V12_LEVEL)
	{
	    if (upg_dbg_out)
	    {
		SIprintf("]==> yes, we are pre-1.2\n");
		SIprintf("\n]==> *** DOING 1.2 UPDATES *** \n");
	    }
	    /* pre-65 ES, so first rebuild system catalogs changing in 6.5 ES*/
/* # line 3947 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3948 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed((char *)"set session authorization $ingres");
    IIsyncup((char *)0,0);
  }
/* # line 3949 "upgradedb.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session with privileges=\
all");
    IIsyncup((char *)0,0);
  }
/* # line 3950 "upgradedb.sc" */	/* host code */
	    IIlq_Protect(TRUE);
	}
	if (upgr_cb.Dbname_ptr->level < V20_LEVEL)
	{
	    if (upg_dbg_out)
	    {
		SIprintf("]==> yes, we are pre-2.0\n");
		SIprintf("\n]==> *** DOING 2.0 UPDATES *** \n");
	    }
	    /* Normalize hashed date columns (as per bug 74033) */
	    cv_20_hashdate_tables(dbname_curr);
	    /* pre-20 , so first rebuild system catalogs changing in 2.0 */
/* # line 3962 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3963 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed((char *)"set session authorization $ingres");
    IIsyncup((char *)0,0);
  }
/* # line 3964 "upgradedb.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session with privileges=\
all");
    IIsyncup((char *)0,0);
  }
/* # line 3965 "upgradedb.sc" */	/* host code */
	    IIlq_Protect(TRUE);
	    new_for_20_stdcats(dbname_curr, upgr_cb.Dbname_ptr->upcase);
	    if ( upgr_cb.Dbname_ptr->errors == TRUE )
		break;
	    IIlq_Protect(TRUE);
	}
	/*
	** Catalog changes for 2.5
	*/
	if (upgr_cb.Dbname_ptr->level < V25_LEVEL)
	{
	    changed_cats_25(upgr_cb.Dbname_ptr->dbname,
			    upgr_cb.Dbname_ptr,
			    upgr_cb.Dbname_ptr->upcase);
	}
	if (upgr_cb.Dbname_ptr->level < V26_LEVEL && is_iidbdb)
	{
	    if (upg_dbg_out)
	    {
		SIprintf("]==> yes, we are pre-2.6\n");
		SIprintf("\n]==> *** DOING 2.6 UPDATES *** \n");
	    }
/* # line 3987 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 3988 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed((char *)"set session authorization $ingres");
    IIsyncup((char *)0,0);
  }
/* # line 3989 "upgradedb.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session with privileges=\
all");
    IIsyncup((char *)0,0);
  }
/* # line 3990 "upgradedb.sc" */	/* host code */
	    IIlq_Protect(TRUE);
	    /* need to drop internal procedures that have changed */
	    upgr_cb.Ignore_errors = TRUE;
/* # line 3993 "upgradedb.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"iiqef_create_db");
    IIwritio(0,(short *)0,1,32,0,(char *)"drop procedure iiqef_create_\
db");
    IIsyncup((char *)0,0);
  }
/* # line 3994 "upgradedb.sc" */	/* host code */
	    upgr_cb.Ignore_errors = FALSE;
	    IIlq_Protect(TRUE);
	}
	if (upgr_cb.Dbname_ptr->level < V30_LEVEL)
	{
	    if (upg_dbg_out)
		SIprintf("]==> *** Doing r3 Updates ***\n");
	    new_for_r3_syscats(dbname_curr);
	    if (upgr_cb.Dbname_ptr->errors)
		break;
	    changed_for_r3_syscats(dbname_curr, upgr_cb.Dbname_ptr->upcase);
	    if (upgr_cb.Dbname_ptr->errors)
		break;
	}
	/* Any stdcat views dropped should be re-created */
	/* If this is iidbdb, don't do the iidbdb views yet.
	** (maybe they depend on iidbdb stuff not upgraded yet.)
	** Don't bother to do this if we're going to upgrade trees, the
	** fix-qry-tree routine drops and recreates all std cats
	** just to make sure that unloaddb has a good set to work with
	** However we DO need standard catalog views for Star.
	*/
	if (upgr_cb.Dbname_ptr->level < V904_LEVEL
	  && (! (upgr_cb.Dbname_ptr->tree_convert || upg_trees)
	      || upgr_cb.Dbname_ptr->star_flag) )
	{
	    du_talk(DU_MODESENS, &upgr_cb, I_DU0103_RECREATING_STDCAT_VIEWS, 0);
	    duc_upgrade_stdcat_views("", upgr_cb.Dbname_ptr->upcase,
		    upgr_cb.Dbname_ptr->star_flag);
	    if (is_iidbdb)
	    {
		/* FIXME:  iisecurity_alarms is currently the only SCI view
		** that is a) in all databases and b) DIFFERENT in iidbdb
		** vs user db's.  Ideally this would be handled with
		** separate db context vs what-cats-to-do flags to the
		** view utilities, but I can't summon the energy to do
		** that right now.  Instead, drop that one view and let
		** the iidbdb code recreate it.
		*/
/* # line 4033 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop view iisecurity_alarms");
    IIsyncup((char *)0,0);
  }
/* # line 4034 "upgradedb.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
		    du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
			    0, "iisecurity_alarms",
			    0, dbname_curr);
		    /* but keep going */
		}
/* # line 4041 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 4042 "upgradedb.sc" */	/* host code */
		/* Don't do grants yet, wait till all catalogs are done */
	    }
	    else
	    {
		/* Regrant select to public on system catalog views. */
		duc_grant_stdcat_views(dbname_curr, upgr_cb.Dbname_ptr->star_flag);
	    }
	    /* Make sure we re-create any internal procedures dropped */
	    duc_upgrade_internal_procs(dbname_curr, upgr_cb.Dbname_ptr->upcase,
		    upgr_cb.Dbname_ptr->star_flag);
	    IIlq_Protect(TRUE);		/* Restore priv, gets turned off */
	}
/* # line 4054 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 4055 "upgradedb.sc" */	/* host code */
	if (setauth("$ingres") != OK)
	    SIprintf("Warning: SET SESSION AUTHORIZATION $ingres failed\n");
/* # line 4057 "upgradedb.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session with privileges=\
all");
    IIsyncup((char *)0,0);
  }
/* # line 4058 "upgradedb.sc" */	/* host code */
	if ( (upgr_cb.Dbname_ptr) && (upgr_cb.Dbname_ptr->star_flag))
	{
	    cv_star_cats();
	}
    } while (0);    /* end of control loop */
}
/*{
** Name: add_dbname - Add database name to linked list
**
** Description:
**	Adds a DBNAME_NODE to the end of a linked list of DBNAME_NODEs
**	
** Inputs:
**	dbname			    Name of database to be added to list.
**	level			    internally generated cmptlvl for the db
**	cdbname			    Name of CDB if star_flag is true
**	ddbname			    Name of DDB if star_flag is true
**	star_flag		    set to true if we are upgrading a
**				      distributed database
**	upcase			    set to tru for FIPS database
**      dbservice		    dbservice flags
**
** Outputs:
**	upgr_cb.Dbname_head	    Points to new struct containing dbname.
**
**      Returns:
**	    FAIL		    If out of memory
**
** Side Effects:
**	    
** History:
**	05-aug-88 (jrb)
**	    Initial coding.
**	10-sep-88 (jrb)
**	    Now sets the new "errors" field to FALSE.
**	04-nov-88 (jrb)
**	    Added new input parameter for setting minorid on each db.
**	05-nov-88 (jrb)
**	    Added dba to struct so that we can enter each db as the dba
**	    rather than as $ingres (had problems trying to do the grants
**	    as $ingres).
**	07-jun-89 (ralph)
**	    Adapted for CV60TO70
**	07-jun-90 (ralph)
**	    Add logic to qualify fe/be conversion separately.
**	03-may-91 (teresa)
**	    Add support for STAR (ie, cdbs and ddbs)
**	25-jul-91 (teresa)
**	    move error message text to erduf.msg
**	22-Sep-2004 (schka24)
**	    Add to the end, not to the front, so that we can arrange
**	    for Star databases to follow non-Star databases.
[@history_template@]...
*/
static VOID
add_dbname(dbname, dba, level, access, locname, areaname,
	   cdbname, ddbname, star_flag, upcase, dbservice)
char		*dbname;
char		*dba;
i4		level;
i4		access;
char		*locname;
char		*areaname;
char		*cdbname;
char		*ddbname;
i4		star_flag;
bool		upcase;
i4		dbservice;
{
    DBNAME_NODE	    *temp;
    DBNAME_NODE	    *p;
    STATUS	    status;
    temp = (DBNAME_NODE *) MEreqmem((u_i4)0, 
				    (u_i4)sizeof(DBNAME_NODE), 
				    (bool)FALSE, 
				    &status);
    if (status != OK)
	upgr_exit ( DU_FAIL_EXIT, E_DU6013_NO_MORE_MEMORY);
    _VOID_ STcopy(dbname, temp->dbname);	/* insert into list */
    _VOID_ STcopy(dba, temp->dba);
    _VOID_ STcopy(locname, temp->locname);
    _VOID_ STcopy(areaname, temp->areaname);
    if (star_flag)
    {
	_VOID_ STcopy(ddbname, temp->ddbname);
	_VOID_ STcopy(cdbname, temp->cdbname);
	temp->star_flag=TRUE;	
    }
    else
    {
	temp->star_flag=FALSE;
	temp->ddbname[0]='\0';
	temp->cdbname[0]='\0';
    }
    if (upcase)
      temp->upcase = TRUE;
    else
      temp->upcase = FALSE;
    temp->level	  = level;
    temp->access  = access;
    temp->errors  = FALSE;
    temp->tree_error = FALSE;
    temp->nextdb  = NULL;
    temp->be_convert = FALSE;
    temp->fe_convert = FALSE;
    temp->did_bootstrap = FALSE;
    temp->first_open = TRUE;
    /* Determine if we need to do tree upgrade */
    if (upg_trees 
	|| (upgr_cb.dbmsinfo_lp64 == 'Y' && (dbservice & DU_LP64) == 0)
	|| (upgr_cb.dbmsinfo_lp64 != 'Y' && (dbservice & DU_LP64)) )
    {
	temp->tree_convert = TRUE;
    }
    else
    {
	temp->tree_convert = FALSE;
    }
    p = upgr_cb.Dbname_head;
    if (p == NULL)
    {
	upgr_cb.Dbname_head = temp;
    }
    else
    {
	while (p->nextdb != NULL)
	    p = p->nextdb;
	p->nextdb = temp;
    }
}
/*{
** Name: handle_esqlerr - Print esql error
**
** Description:
**	If an esql error occurs, this routine retrieves the error from
**	the SQLCA and prints it.  The error is marked either in the iidbdb
**	or the current database in the dbname list depending on what
**	upgr_cb.Dbname_ptr is set to.
**	
** Inputs:
**	none
**
** Outputs:
**	none
**
**     Side Effects:
**	    Will affect the upgr_cb.Dbname_ptr->errors field or the upgr_cb.Iidbdbok
**	    global.
**	    
** History:
**	04-aug-88 (jrb)
**	    Initial coding.
**	15-oct-91 (teresa)
**	    modified to expect possible Dbname_ptr during iidbdb conversion
[@history_template@]...
*/
VOID
handle_esqlerr()
{
  char error_buf[SQL_MSG_BUF_LEN];
    if (upgr_cb.Ignore_errors)
	return;
    if (upgr_cb.Dbname_ptr != NULL)
    {
	/* either we had an error while actually upgrading the iidbdb or
	** we had an error on the use database */
	if (STcompare (upgr_cb.Dbname_ptr->dbname, ERx("iidbdb"))==0)
	{   
	    /* its the iidbdb */
	    if (upgr_cb.Iidbdbok)
	    {
		SIprintf(
    "UPGRADEDB cannot complete any conversions due to the following error\n");
		du_log (upgr_cb.du_skiplog, &upgr_cb.du_log,
    "UPGRADEDB cannot complete any conversions due to the following error\n");
		upgr_cb.Iidbdbok = FALSE;
		upgr_cb.Dbname_ptr->errors = TRUE;
	    }
	}
	else if (!upgr_cb.Dbname_ptr->errors)
	{   
	    /* its a user database and the error has not previously been
	    ** reported */
	    SIprintf("Aborting upgrade of this database because of error");
	    du_log (upgr_cb.du_skiplog, &upgr_cb.du_log,
		   "Aborting upgrade of this database because of error");
	    upgr_cb.Dbname_ptr->errors = TRUE;  /* this db was not fully converted */
	}
    }
    else	/* iidbdb error during informational queries  before we
		** began converting the iidbdb */
    {
	if (upgr_cb.Iidbdbok)
	{
	    SIprintf(
"UPGRADEDB cannot complete any conversions due to the following error\n");
	    du_log (upgr_cb.du_skiplog, &upgr_cb.du_log,
"UPGRADEDB cannot complete any conversions due to the following error\n");
	    upgr_cb.Iidbdbok = FALSE;
	}
    }
/* # line 4261 "upgradedb.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,SQL_MSG_BUF_LEN-1,error_buf,63);
  }
/* # line 4262 "upgradedb.sc" */	/* host code */
    SIprintf("\n%s",error_buf);
    du_log (upgr_cb.du_skiplog, &upgr_cb.du_log, error_buf);
}
/*{
** Name: handle_equelerr - Print equel error
**
** Description:
**	If an equel error occurs, this routine retrieves the error from
**	ingres and prints it.  The error is marked either in the iidbdb
**	or the current database in the dbname list depending on what
**	upgr_cb.Dbname_ptr is set to.
**
**	*Note* this handler is only in effect for part of upgradedb,
**	namely those routines textually between main() and upgr_exit().
**	I suspect someone didn't understand how "whenever sqlerror" works.
**	This needs to be fixed, but there's lots of code later in upgradedb
**	that ignores errors without setting the Ignore_error flag...
**	(schka24 Sep 2004)
**
** Inputs:
**	ernum					ingres error number
**
** Outputs:
**	none
**
**	Returns:
**	    0		To supress printing of error message by ingres.
**
** Side Effects:
**	Will affect the upgr_cb.Dbname_ptr->errors field or the Iidbdbok global.
**	    
** History:
**	28-nov-88 (jrb)
**	    Initial coding.
**	15-oct-91 (teresa)
**	    modified to expect possible Dbname_ptr during iidbdb conversion
[@history_template@]...
*/
i4
handle_equelerr(ernum)
i4 *ernum;
{
  char error_buf[SQL_MSG_BUF_LEN];
    if (upgr_cb.Ignore_errors)
	return(0);
    if (upgr_cb.Dbname_ptr != NULL)
    {
	/* either we had an error while actually upgrading the iidbdb or
	** we had an error on the use database */
	if (STcompare (upgr_cb.Dbname_ptr->dbname, ERx("iidbdb"))==0)
	{   
	    /* its the iidbdb */
	    if (upgr_cb.Iidbdbok)
	    {
		SIprintf(
    "UPGRADEDB cannot complete any conversions due to the following error\n");
		du_log (upgr_cb.du_skiplog, &upgr_cb.du_log,
    "UPGRADEDB cannot complete any conversions due to the following error\n");
		upgr_cb.Iidbdbok = FALSE;
	    }
	}
	else if (!upgr_cb.Dbname_ptr->errors)
	{   
	    /* its a user database and the error has not previously been
	    ** reported */
	    SIprintf("Aborting upgrade of this database because of error");
	    du_log (upgr_cb.du_skiplog, &upgr_cb.du_log,
		   "Aborting upgrade of this database because of error");
	    upgr_cb.Dbname_ptr->errors = TRUE;  /* this db was not fully converted */
	}
    }
    else	/* iidbdb error during informational queries  before we
		** began converting the iidbdb */
    {
	if (upgr_cb.Iidbdbok)
	{
	    SIprintf(
"UPGRADEDB cannot complete any conversions due to the following error\n");
	    du_log (upgr_cb.du_skiplog, &upgr_cb.du_log,
"UPGRADEDB cannot complete any conversions due to the following error\n");
	    upgr_cb.Iidbdbok = FALSE;
	}
    }
/* # line 4812 "upgradedb.qsc" */	/* inquire_ingres */
  {
    IIeqiqio((short *)0,1,32,SQL_MSG_BUF_LEN-1,error_buf,(char *)
"ERRORTEXT");
  }
/* # line 4814 "upgradedb.qsc" */	/* host code */
    SIprintf("\n%s",error_buf);
    du_log( upgr_cb.du_skiplog, &upgr_cb.du_log, error_buf);
    /* returning 0 tells ingres not to print error message */
    return(0);
}
/*{
** Name: handle_ex	- Exception handler for user aborts or AVs
**
** Description:
**      If the user aborts the program or an unexpected exception occurs,
**	this function will clean up and abort smoothly. 
**
** Inputs:
**      ex_parms                        This is an EX_ARGS struct.
**
** Outputs:
**      none
**
** History:
**	04-aug-88 (jrb)
**	    Initial coding.
**	23-jul-91 (teresa)
**	    rewrote to meet coding standards and also to use exit handler.
**	13-sep-96 (nick)
**	    Must use EXmatch() to comapre against EX types.
[@history_template@]...
*/
STATUS
handle_ex(ex_parms)
EX_ARGS            *ex_parms;
{
    switch (EXmatch(ex_parms->exarg_num, 2, EXBUSERR, EXINTR))
    {
      case 1:
	du_talk(DU_ALWAYS, &upgr_cb, E_DU3601_ACCESS_VIOLATION, 0);
	break;
      case 2:
	du_talk(DU_ALWAYS, &upgr_cb, E_DU3600_INTERRUPT, 0);
	break;
      default:
      {
	char	name[DB_MAXNAME+1];
	if (upgr_cb.du_opendb[0]=='\0')
	    STcopy (DU_DBDBNAME, name);
	else
	    STcopy (upgr_cb.du_opendb, name);
	du_talk(DU_ALWAYS, &upgr_cb, W_DU1010_UTIL_ABORT, 4,
		 0, "Upgrade", 0, name);
	break;
      }
    }
    upgr_exit(DU_FAIL_EXIT, I_DU0080_UPGRADEDB_TERM);
    /* Put the return value here so lint won't complain. */
    return(EXRESIGNAL);
}
/*{
** Name: cv_pre64_user	- convert iiuser to 6.3 format
**
** Description:
**	This routine converts iiuser to 6.3 format
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	17-jun-89 (ralph)
**	    Initial creation for CV60TO70
**	14-oct-91 (teresa)
**	    Rewrote in SQL.  ALso changed for 6.5:  added password to catalog
**	12-jul-94 (robf)
**          iiuser no longer has a reserved field
**	20-Sep-2004 (schka24)
**	    Don't get faked out by ansi uppercase databases
[@history_template@]...
*/
static VOID
cv_pre64_user()
{
  i4 default_group;
  i4 password;
    if (upgr_cb.Iidbdbok)
	    SIprintf("]==> cv_pre64_user():\n");
/* # line 4438 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(attrelid)from iiattribute where lowercase(attname)='defa\
ult_group' and attrelid=");
    IIputdomio((short *)0,1,30,sizeof(DU_B_USER_TAB_ID),(void *)IILQint(
    DU_B_USER_TAB_ID));
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,sizeof(DU_I_USER_TAB_ID),(void *)IILQint(
    DU_I_USER_TAB_ID));
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&default_group);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 4442 "upgradedb.sc" */	/* host code */
    if (!upgr_cb.Iidbdbok)
	return;			/* error, so give up */
/* # line 4444 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(attrelid)from iiattribute where lowercase(attname)='pass\
word' and attrelid=");
    IIputdomio((short *)0,1,30,sizeof(DU_B_USER_TAB_ID),(void *)IILQint(
    DU_B_USER_TAB_ID));
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,sizeof(DU_I_USER_TAB_ID),(void *)IILQint(
    DU_I_USER_TAB_ID));
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&password);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 4448 "upgradedb.sc" */	/* host code */
    if (!upgr_cb.Iidbdbok)
	return;			/* error, so give up */
    if (password !=0 )
    {
	du_talk (DU_MODESENS,&upgr_cb, I_DU0091_IIUSER_CATALOG, 2,
		  0, SERVER_VERSION);
	return;
    }
    du_talk (DU_MODESENS,&upgr_cb, I_DU0092_CONVERTING_IIUSER, 0);
/* # line 4457 "upgradedb.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iiuser_temp as select * from iiuser");
    IIsyncup((char *)0,0);
  }
/* # line 4458 "upgradedb.sc" */	/* host code */
    if (!upgr_cb.Iidbdbok)
	return;
    upgr_cb.Ignore_errors = TRUE;
/* # line 4461 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop iiuser");
    IIsyncup((char *)0,0);
  }
/* # line 4462 "upgradedb.sc" */	/* host code */
    upgr_cb.Ignore_errors = FALSE;
    /* create current format iiuser catalog */
    duc_iiuser();
    if (!upgr_cb.Iidbdbok)
	return;
    /* now populate iiuser from iiuser_z.  Since the new attributes are
    ** created with not null and defaults, they will automatically be
    ** initialized with the correct default value.
    */
    if (default_group)
    {	/* 6.3 or later */
	/* this just needs the password attribute appended. */
/* # line 4474 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiuser(name, gid, mid, status, default_group)select name,\
 gid, mid, status, default_group from iiuser_temp");
    IIsyncup((char *)0,0);
  }
/* # line 4477 "upgradedb.sc" */	/* host code */
    }
    else
    {	/* pre 6.3 */
	/* this requires 3 fields to be appended:  default_group, reserved and
	** password */
/* # line 4482 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiuser(name, gid, mid, status)select name, gid, mid, stat\
us from iiuser_temp");
    IIsyncup((char *)0,0);
  }
/* # line 4484 "upgradedb.sc" */	/* host code */
    }
    if (!upgr_cb.Iidbdbok)
	return;
    /* now modify the catalog to the preferred storage format */
    duc_m5_user();
}
/*{
** Name: cv_group	- create iiusergroup for 6.3
**
** Description:
**	Creates iiusergroup for 6.3
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	17-jun-89 (ralph)
**	    Initial creation for CV60TO70
[@history_template@]...
*/
static VOID
cv_group()
{
  int rows_group;
  i4 tid_group;
/* # line 5000 "upgradedb.qsc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(tid_group=iirelation.tid)where iirelation.reltid=");
    IIputdomio((short *)0,1,30,sizeof(DU_B_GRPID_TAB_ID),(void *)IILQint(
    DU_B_GRPID_TAB_ID));
    IIwritio(0,(short *)0,1,32,0,(char *)"and iirelation.reltidx=");
    IIputdomio((short *)0,1,30,sizeof(DU_I_GRPID_TAB_ID),(void *)IILQint(
    DU_I_GRPID_TAB_ID));
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tid_group);
      if (IIerrtest() != 0) goto IIrtB1;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 5006 "upgradedb.qsc" */	/* inquire_ingres */
  {
    IIeqiqio((short *)0,1,30,4,&rows_group,(char *)"ROWCOUNT");
  }
/* # line 5009 "upgradedb.qsc" */	/* host code */
	/* Exit  if this catalog is present */
	if (rows_group != 0)
	{
	    du_talk( DU_MODESENS, &upgr_cb, I_DU0093_IIUSERGROUP_EXISTS, 0);
	    return;
	}
	du_talk( DU_MODESENS, &upgr_cb, I_DU0094_CRE_IIUSERGROUP, 0);
	upgr_cb.Ignore_errors = TRUE;
/* # line 5018 "upgradedb.qsc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy iiusergroup");
    IIsyncup((char *)0,0);
  }
/* # line 5019 "upgradedb.qsc" */	/* host code */
	upgr_cb.Ignore_errors = FALSE;
	duc_iiusergroup();
	duc_modify_iiusergroup();
}
/*{
** Name: cv_pre64_role	- Create/modify iirole for 6.5ES
**
** Description:
**	Create the iirole catalog
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	17-jun-89 (ralph)
**	    Initial creation for CV60TO70
**      12-jul-94 (robf)
**          Updated for 6.5 ES
[@history_template@]...
*/
static VOID
cv_pre64_role()
{
  int rows_role;
  i4 tid_role;
/* # line 5050 "upgradedb.qsc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(tid_role=iirelation.tid)where iirelation.reltid=");
    IIputdomio((short *)0,1,30,sizeof(DU_B_APLID_TAB_ID),(void *)IILQint(
    DU_B_APLID_TAB_ID));
    IIwritio(0,(short *)0,1,32,0,(char *)"and iirelation.reltidx=");
    IIputdomio((short *)0,1,30,sizeof(DU_I_APLID_TAB_ID),(void *)IILQint(
    DU_I_APLID_TAB_ID));
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tid_role);
      if (IIerrtest() != 0) goto IIrtB2;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
/* # line 5056 "upgradedb.qsc" */	/* inquire_ingres */
  {
    IIeqiqio((short *)0,1,30,4,&rows_role,(char *)"ROWCOUNT");
  }
/* # line 5059 "upgradedb.qsc" */	/* host code */
	/* Exit  if this catalog is present */
	if (rows_role != 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, I_DU0095_IIROLE_EXISTS, 0);
	    return;
	}
	du_talk(DU_MODESENS, &upgr_cb, I_DU0096_CRE_IIROLE, 0);
	upgr_cb.Ignore_errors = TRUE;
/* # line 5069 "upgradedb.qsc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy iirole");
    IIsyncup((char *)0,0);
  }
/* # line 5070 "upgradedb.qsc" */	/* host code */
	upgr_cb.Ignore_errors = FALSE;
/* # line 5072 "upgradedb.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iirole(roleid=char(24), rolepass=char(24), reserve=char(32))");
    IIsyncup((char *)0,0);
  }
/* # line 5076 "upgradedb.qsc" */	/* host code */
	if (!upgr_cb.Iidbdbok)
		return;
/* # line 5079 "upgradedb.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iirole to hash unique on roleid where minpages=2");
    IIsyncup((char *)0,0);
  }
/* # line 5082 "upgradedb.qsc" */	/* host code */
}
/*{
** Name: cv_dbpriv	- Create the iidbpriv catalog
**
** Description:
**	Create the iidbpriv catalog.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	17-jun-89 (ralph)
**	    Initial creation for CV60TO70
**	19-apr-90 (ralph)
**	    Changed iidbprivileges definition, and allowed it to be recreated.
**	15-oct-91 (teresa)
**	    fill1 changed to dbflags from 6.4 to 6.5.  Had to rewrite routine
**	    since it only delt with creating table if it does not exist.  Now
**	    we need to create it if it does not exist and change it if it
**	    exists in 6.4 format.  Rewrote in SQL intead of QUEL.
[@history_template@]...
*/
static VOID
cv_dbpriv()
{
  i4 reltid, reltidx;
	if (!rel_exists("iidbpriv"))
	{
	    du_talk(DU_MODESENS, &upgr_cb, I_DU009C_CRE_IIDBPRIVILEGES, 0);
	    duc_iidbpriv();
	    if (!upgr_cb.Iidbdbok)
		    return;
	    duc_m6_dbpriv();
	    if (!upgr_cb.Iidbdbok)
		    return;
	}
	else
	{
	    /* catalog exists, so see if it is in the current format */
	    if (tbl_lacks_col("iidbpriv", "dbflags"))
	    {
		/*
		** we need to rename the fill1 attribute to have the name of
		** dbflags.  Do not initialize this field, it already defaults
		** to zero, and that is the correct value.
		*/
		if (upg_dbg_out)
		    SIprintf("]==> Whacking iiattribute for iidbpriv.dbflags\n");
		reltid = DU_B_DBPRIV_TAB_ID;
		reltidx = DU_I_DBPRIV_TAB_ID;
		du_talk(DU_MODESENS, &upgr_cb, I_DU00B7_IIDBPRIV_UPDATE, 0);
/* # line 4689 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiattribute set attname='dbflags' where attname='fill1' and at\
trelid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&reltidx);
    IIsyncup((char *)0,0);
  }
/* # line 4692 "upgradedb.sc" */	/* host code */
		if (!upgr_cb.Iidbdbok)
			return;
	    }
	}
}
/*{
** Name:	cv_iistar_cdbinfo - Create the iistar_cdbinfo catalog
**
** Description:
**	Create the ciistar_cdbinfo catalog.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	14-aug-91 (teresa)
**	    Initial creation.
**	31-mar-92 (teresa)
**	    fix bug 42250 by assuring iistar_cdbs exist before building a view
**	    off of it.
[@history_template@]...
*/
static VOID
cv_iistar_cdbinfo(bool upcase)
{
    /* 
    ** See whether or not this installation supports distributed databases.
    ** If not, then set up basic catalogs 
    */
    if (!rel_exists("iistar_cdbs"))
    {
	/*
	** this is NOT an installation that supports STAR -- set up the
	** distributed iidbdb catalogs incase STAR is supported in the future
	*/
	du_talk(DU_MODESENS, &upgr_cb, I_DU00CB_CRE_STAR_CATS, 0);
	duc_6create_iidbdb_cats("iistar_cdbs");
	if (!rel_exists("iiddb_netcost"))
	    duc_6create_iidbdb_cats("iiddb_netcost");
	if (!rel_exists("iiddb_nodecosts"))
	    duc_6create_iidbdb_cats("iiddb_nodecosts");	
    }
}
/*{
** Name: cv_online	- Conversion for online backup
**
** Description:
**	Performs necessary conversion for online backup.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	17-jun-89 (ralph)
**	    Initial creation for CV60TO70
**	20-Sep-2004 (schka24)
**	    Don't get faked out by ansi uppercase databases
[@history_template@]...
*/
static VOID
cv_online()
{
  int rows_database;
  i4 tid_database;
/* # line 5219 "upgradedb.qsc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(tid_database=iiattribute.tid)where lowercase(iiattribute.att\
name)=\"dmpdev\" and iiattribute.attrelid=");
    IIputdomio((short *)0,1,30,sizeof(DU_B_DATABASE_TAB_ID),(void *)IILQint(
    DU_B_DATABASE_TAB_ID));
    IIwritio(0,(short *)0,1,32,0,(char *)"and iiattribute.attrelidx=");
    IIputdomio((short *)0,1,30,sizeof(DU_I_DATABASE_TAB_ID),(void *)IILQint(
    DU_I_DATABASE_TAB_ID));
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tid_database);
      if (IIerrtest() != 0) goto IIrtB3;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
/* # line 5226 "upgradedb.qsc" */	/* inquire_ingres */
  {
    IIeqiqio((short *)0,1,30,4,&rows_database,(char *)"ROWCOUNT");
  }
/* # line 5229 "upgradedb.qsc" */	/* host code */
	/* Exit if this field is present */
	if (rows_database != 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, I_DU009D_IIDATABASE_CURRENT, 2,
		      0, SERVER_VERSION);
	    return;
	}
	du_talk(DU_MODESENS, &upgr_cb, I_DU009E_UPG_IIDATABASE, 0);
/* # line 5239 "upgradedb.qsc" */	/* replace */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidatabase(xtra=\"ii_dump\")");
    IIsyncup((char *)0,0);
  }
/* # line 5241 "upgradedb.qsc" */	/* host code */
	if (!upgr_cb.Iidbdbok)
		return;
/* # line 5244 "upgradedb.qsc" */	/* replace */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"replace iiattribute(attname=\"dmpdev\")where iiattribute.attname=\"xt\
ra\" and iiattribute.attrelid=");
    IIputdomio((short *)0,1,30,sizeof(DU_B_DATABASE_TAB_ID),(void *)IILQint(
    DU_B_DATABASE_TAB_ID));
    IIsyncup((char *)0,0);
  }
/* # line 5248 "upgradedb.qsc" */	/* host code */
	if (!upgr_cb.Iidbdbok)
		return;
	du_talk(DU_MODESENS, &upgr_cb, I_DU009F_APPEND_DUMP_LOC, 0);
/* # line 5253 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iilocations(status=2, lname=\"ii_dump\", area=\"II_DUMP\")");
    IIsyncup((char *)0,0);
  }
/* # line 5254 "upgradedb.qsc" */	/* host code */
}
/*{
** Name: cv_dbcap	- Add iidbcapabilities for 6.5
**
** Description:
**	Add iidbcapabilities entries for 6.5
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	17-jun-89 (ralph)
**	    Initial creation for CV60TO70
**	02-jul-90 (ralph)
**	    Added 6.3/02 iidbcapabilities entries
**	12-mar-1991 (bryanp)
**	    Picked up some standard catalog changes from the FE group:
**		ING6.3 ==> ING6.4, 00603 ==> 00604 (iidbcapabilities)
**	06-may-91 (teresa)
**	    and STANDARD_CATALOG_LEVEL to iidbcapabilities.
**      09-Mar-95 (lawst01) bug 67381
**          added DB_DELIMITED_CASE and DB_REAL_USER_CASE both with 
**          values of 'LOWER' to iidbcapabilities table.
**      06-mar-1996 (nanpr01)
**	    Instead of using OPEN/SQL_LEVEL and 00605, use the standard
**	    defines DU_DB7_OPENSQL_LEVEL and DU_DB7_CUR_OPENSQL_LEVEL.
**	07-jan-2002 (toumi01)
**	    changed MAX_COLUMNS from 300 to 1024
**	30-Aug-2004 (schka24)
**	    update existing max-columns to 1024 as well.
[@history_template@]...
*/
static VOID
cv_dbcap()
{
  int rows_dbcap;
  i4 tid_dbcap;
	du_talk (DU_MODESENS, &upgr_cb, I_DU00A0_UPDATING_CAPS, 0);
/* # line 5297 "upgradedb.qsc" */	/* replace */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidbcapabilities(cap_value=");
    IIputdomio((short *)0,1,32,7,DU_DB1_CUR_QUEL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)
")where iidbcapabilities.cap_capability=");
    IIputdomio((short *)0,1,32,10,DU_DB1_QUEL_LEVEL);
    IIsyncup((char *)0,0);
  }
/* # line 5299 "upgradedb.qsc" */	/* replace */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidbcapabilities(cap_value=");
    IIputdomio((short *)0,1,32,7,DU_DB2_CUR_SQL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)
")where iidbcapabilities.cap_capability=");
    IIputdomio((short *)0,1,32,9,DU_DB2_SQL_LEVEL);
    IIsyncup((char *)0,0);
  }
/* # line 5301 "upgradedb.qsc" */	/* replace */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidbcapabilities(cap_value=");
    IIputdomio((short *)0,1,32,5,DU_DB3_CUR_INGSQL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)
")where iidbcapabilities.cap_capability=");
    IIputdomio((short *)0,1,32,16,DU_DB3_INGSQL_LEVEL);
    IIsyncup((char *)0,0);
  }
/* # line 5303 "upgradedb.qsc" */	/* replace */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidbcapabilities(cap_value=");
    IIputdomio((short *)0,1,32,5,DU_DB4_CUR_CMNSQL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)
")where iidbcapabilities.cap_capability=");
    IIputdomio((short *)0,1,32,16,DU_DB4_CMNSQL_LEVEL);
    IIsyncup((char *)0,0);
  }
/* # line 5305 "upgradedb.qsc" */	/* replace */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidbcapabilities(cap_value=");
    IIputdomio((short *)0,1,32,5,DU_DB5_CUR_INGQUEL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)
")where iidbcapabilities.cap_capability=");
    IIputdomio((short *)0,1,32,17,DU_DB5_INGQUEL_LEVEL);
    IIsyncup((char *)0,0);
  }
/* # line 5307 "upgradedb.qsc" */	/* replace */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidbcapabilities(cap_value=");
    IIputdomio((short *)0,1,32,5,DU_DB6_CUR_STDCAT_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)
")where iidbcapabilities.cap_capability=");
    IIputdomio((short *)0,1,32,22,DU_DB6_STDCAT_LEVEL);
    IIsyncup((char *)0,0);
  }
/* # line 5309 "upgradedb.qsc" */	/* replace */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"replace iidbcapabilities(cap_value=\"1024\")where iidbcapabilities.ca\
p_capability=\"MAX_COLUMNS\"");
    IIsyncup((char *)0,0);
  }
/* # line 5312 "upgradedb.qsc" */	/* host code */
	if (    (upgr_cb.Dbname_ptr == NULL  &&  upgr_cb.Iidbdbok == FALSE)
	    ||  (upgr_cb.Dbname_ptr != NULL  &&  upgr_cb.Dbname_ptr->errors == TRUE))
	    return;
	/* Add new iidbcapabilities entries for 6.3 */
/* # line 5318 "upgradedb.qsc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(tid_dbcap=iidbcapabilities.tid)where iidbcapabilities.cap_ca\
pability=\"INGRES_RULES\"");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tid_dbcap);
      if (IIerrtest() != 0) goto IIrtB4;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE4:;
  }
/* # line 5321 "upgradedb.qsc" */	/* inquire_ingres */
  {
    IIeqiqio((short *)0,1,30,4,&rows_dbcap,(char *)"ROWCOUNT");
  }
/* # line 5324 "upgradedb.qsc" */	/* host code */
	/* Assume if INGRES_RULES not defined, no 6.3 capabilities defined */
	if (rows_dbcap == 0)
	{
/* # line 5328 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"INGRES_RULES\", cap_value\
=\"Y\")");
    IIsyncup((char *)0,0);
  }
/* # line 5330 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"INGRES_UDT\", cap_value=\"\
Y\")");
    IIsyncup((char *)0,0);
  }
/* # line 5332 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"INGRES_AUTH_GROUP\", cap_\
value=\"Y\")");
    IIsyncup((char *)0,0);
  }
/* # line 5334 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"INGRES_AUTH_ROLE\", cap_v\
alue=\"Y\")");
    IIsyncup((char *)0,0);
  }
/* # line 5336 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"INGRES_LOGICAL_KEY\", cap\
_value=\"Y\")");
    IIsyncup((char *)0,0);
  }
/* # line 5338 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"MAX_COLUMNS\", cap_value=\
\"1024\")");
    IIsyncup((char *)0,0);
  }
/* # line 5340 "upgradedb.qsc" */	/* host code */
	}
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
	/* Add new iidbcapabilities entries for 6.3/02 */
/* # line 5347 "upgradedb.qsc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(tid_dbcap=iidbcapabilities.tid)where iidbcapabilities.cap_ca\
pability=\"UNIQUE_KEY_REQ\"");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tid_dbcap);
      if (IIerrtest() != 0) goto IIrtB5;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE5:;
  }
/* # line 5350 "upgradedb.qsc" */	/* inquire_ingres */
  {
    IIeqiqio((short *)0,1,30,4,&rows_dbcap,(char *)"ROWCOUNT");
  }
/* # line 5352 "upgradedb.qsc" */	/* host code */
	/* Assume if UNIQUE_KEY_REQ not defined, 6.3/02 entries not present */
	if (rows_dbcap == 0)
	{
/* # line 5356 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"UNIQUE_KEY_REQ\", cap_val\
ue=\"N\")");
    IIsyncup((char *)0,0);
  }
/* # line 5358 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"ESCAPE\", cap_value=\"Y\"\
)");
    IIsyncup((char *)0,0);
  }
/* # line 5360 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"OWNER_NAME\", cap_value=\"\
QUOTED\")");
    IIsyncup((char *)0,0);
  }
/* # line 5362 "upgradedb.qsc" */	/* host code */
	}
	/* 6.4 STANDARD_CATALOG_LEVEL */
/* # line 5365 "upgradedb.qsc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(tid_dbcap=iidbcapabilities.tid)where iidbcapabilities.cap_ca\
pability=");
    IIputdomio((short *)0,1,32,22,DU_DB6_STDCAT_LEVEL);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tid_dbcap);
      if (IIerrtest() != 0) goto IIrtB6;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE6:;
  }
/* # line 5368 "upgradedb.qsc" */	/* inquire_ingres */
  {
    IIeqiqio((short *)0,1,30,4,&rows_dbcap,(char *)"ROWCOUNT");
  }
/* # line 5370 "upgradedb.qsc" */	/* host code */
	if (rows_dbcap == 0)
/* # line 5371 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=");
    IIputdomio((short *)0,1,32,22,DU_DB6_STDCAT_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)", cap_value=");
    IIputdomio((short *)0,1,32,5,DU_DB6_CUR_STDCAT_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 5375 "upgradedb.qsc" */	/* host code */
	/* 6.5 STANDARD_CATALOG_LEVEL */
/* # line 5376 "upgradedb.qsc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(tid_dbcap=iidbcapabilities.tid)where iidbcapabilities.cap_ca\
pability=");
    IIputdomio((short *)0,1,32,14,DU_DB7_OPENSQL_LEVEL);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE7;
IIrtB7:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tid_dbcap);
      if (IIerrtest() != 0) goto IIrtB7;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE7:;
  }
/* # line 5379 "upgradedb.qsc" */	/* inquire_ingres */
  {
    IIeqiqio((short *)0,1,30,4,&rows_dbcap,(char *)"ROWCOUNT");
  }
/* # line 5381 "upgradedb.qsc" */	/* host code */
        if (rows_dbcap == 0)
        {
/* # line 5383 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=");
    IIputdomio((short *)0,1,32,14,DU_DB7_OPENSQL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)", cap_value=");
    IIputdomio((short *)0,1,32,5,DU_DB7_CUR_OPENSQL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 5387 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"DB_DELIMITED_CASE\", cap_\
value=\"LOWER\")");
    IIsyncup((char *)0,0);
  }
/* # line 5390 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"DB_REAL_USER_CASE\", cap_\
value=\"LOWER\")");
    IIsyncup((char *)0,0);
  }
/* # line 5392 "upgradedb.qsc" */	/* host code */
        }
/* # line 5394 "upgradedb.qsc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(tid_dbcap=iidbcapabilities.tid)where iidbcapabilities.cap_ca\
pability=\"SQL_MAX_BYTE_LITERAL_LEN\"");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE8;
IIrtB8:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tid_dbcap);
      if (IIerrtest() != 0) goto IIrtB8;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE8:;
  }
/* # line 5397 "upgradedb.qsc" */	/* inquire_ingres */
  {
    IIeqiqio((short *)0,1,30,4,&rows_dbcap,(char *)"ROWCOUNT");
  }
/* # line 5399 "upgradedb.qsc" */	/* host code */
        if (rows_dbcap == 0)
        {
/* # line 5401 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_BYTE_LITERAL_LEN\"\
, cap_value=\"32000\")");
    IIsyncup((char *)0,0);
  }
/* # line 5404 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_CHAR_LITERAL_LEN\"\
, cap_value=\"32000\")");
    IIsyncup((char *)0,0);
  }
/* # line 5407 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_BYTE_COLUMN_LEN\"\
, cap_value=\"32000\")");
    IIsyncup((char *)0,0);
  }
/* # line 5410 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_VBYT_COLUMN_LEN\"\
, cap_value=\"32000\")");
    IIsyncup((char *)0,0);
  }
/* # line 5413 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_CHAR_COLUMN_LEN\"\
, cap_value=\"32000\")");
    IIsyncup((char *)0,0);
  }
/* # line 5416 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_VCHR_COLUMN_LEN\"\
, cap_value=\"32000\")");
    IIsyncup((char *)0,0);
  }
/* # line 5419 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_NCHR_COLUMN_LEN\"\
, cap_value=\"16000\")");
    IIsyncup((char *)0,0);
  }
/* # line 5422 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_NVCHR_COLUMN_LEN\"\
, cap_value=\"16000\")");
    IIsyncup((char *)0,0);
  }
/* # line 5425 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_SCHEMA_NAME_LEN\"\
, cap_value=\"32\")");
    IIsyncup((char *)0,0);
  }
/* # line 5428 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_TABLE_NAME_LEN\",\
 cap_value=\"32\")");
    IIsyncup((char *)0,0);
  }
/* # line 5431 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_COLUMN_NAME_LEN\"\
, cap_value=\"32\")");
    IIsyncup((char *)0,0);
  }
/* # line 5434 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_USER_NAME_LEN\", \
cap_value=\"32\")");
    IIsyncup((char *)0,0);
  }
/* # line 5437 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_ROW_LEN\", cap_va\
lue=\"262144\")");
    IIsyncup((char *)0,0);
  }
/* # line 5440 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_STATEMENTS\", cap\
_value=\"0\")");
    IIsyncup((char *)0,0);
  }
/* # line 5443 "upgradedb.qsc" */	/* host code */
	}
}
/*{
** Name: cv_protect	- Convert iiprotect
**
** Description:
**	Convert iiprotect.  This catalog will have different formats
**	depending on whether it is 6.2 (or below) or 6.3/6.4.  In either case,
**	bring up to the current format.
**
**	6.2->6.3:  add the following fields:
**		profill1,   progtype,	proseq,
**		prodomset5, prodomset6,	prodomset7,
**		prodomset8, prodomset9,	prodomseta,
**		proreserve
**		INITIALIZE  progtype=3  where prouser=" "
**		INITIALIZE prodomset5 to prodomseta = -1 where
**			prodomset1 to proddomset4 = -1
**
**	6.3/6.4 -> 6.5: add the following fields:
**	    procrtime1	- timestamp MSB of when protection was created
**	    procrtime2	- timestamp LSB of when protection was created
**	    proopctl	- bitmap of defined operations (initialize to the
**			    same as proopset)
**	    provalue	- value associated with permit
**	    profill2	- filler for alignment
**	    probjtype	- from either iirelation.relstat or find object in
**			    iievent or iiprocedure
**			  find object in 
**			    DBOB_TABLE      'T' --  Object is a table
**			    DBOB_VIEW       'V' --  Object is a view
**			    DBOB_INDEX      'I' --  Object is an index
**			    DBOB_DBPROC     'P' --  Object is a dbproc
**			    DBOB_EVENT      'E' --  Object is an event
**			    DBOB_LOC        'L' --  Object is a location
**	    probjstat	-   
**			    DB_SYSTEM       'S' --  Object is system owned  
**	    probjname	- Object name	(iirelation.relid, or iievent.event_name
**			    or iiprocedure.dbp_name -- locations not supported
**			    in 6.4 or before)
**	    probjowner	- Owner of the object	(iirelation.relowner or
**			    iievent.event_owner or iiprocedure.dbp_owner)
**	    prograntor	- person who granted the permit (initilize to DBA)
**	    profill3	- filler for alignment
**		profill1,   progtype,	proseq,
**		prodomset5, prodomset6,	prodomset7,
**		prodomset8, prodomset9,	prodomseta,
**		proreserve
**		INITIALIZE  progtype=3  where prouser=" "
**		INITIALIZE prodomset5 to prodomseta = -1 where
**			prodomset1 to proddomset4 = -1
**
**	NOTE: for 6.4, DBOB_LOC are not supported, so dont
**		       worry about these during conversion to 6.5.
**
**	For conversion to r3, add additional prodomset columns.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	17-jun-89 (ralph)
**	    Initial creation for CV60TO70
**	10-dec-90 (neil)
**	    Alerters: iipermits also includes references to iievent, so
**	    this is called *after* cv_event.
**	14-oct-91 (teresa)
**	    updated to convert to 6.5, also rewrote in SQL
**	11-sep-92 (andre)
**	    MSB in relstat is no longer being set - change
**	    mod((relstat+1073741824+1073741824),x) back to mod(relstat,x)
**	05-oct-92 (jpk)
**		Remove references to MSB of relstat, cv_relstat() routine.
**	14-jul-94 (robf)
**          Initialize the procrtime values (since they are available)
**          and proflags (since otherwise verifydb may complain about
**          QUEL/SQL permit mismatch) (reported as bug 64589)
**	30-Aug-2004 (schka24)
**	    Additions for r3.  Fix objtype for dbevents from 6.x.
**	    Delete the old iiprotect entry for itself, rest of that
**	    old priv is deleted by the drop.
[@history_template@]...
*/
static VOID
cv_protect()
{
    bool prior_63;
    bool prior_65;
    bool prior_r3;
    prior_r3 = tbl_lacks_col("iiprotect","prodomset33");
    prior_65 = tbl_lacks_col("iiprotect","procrtime1");
    prior_63 = tbl_lacks_col("iiprotect","proseq");
    /* Exit if no conversion needed */
    if ( ! prior_r3 )
    {
	du_talk(DU_MODESENS, &upgr_cb, I_DU00A1_IIPROTECT_CURRENT, 2,
		  0, SERVER_VERSION);
	return;
    }
    /* we need to upgrade the iiprotect catalog */
    du_talk(DU_MODESENS, &upgr_cb, I_DU00A2_UPG_IIPROTECT, 0);
    make_copy_of(upgr_cb.Dbname_ptr->dbname, "iiprotect");
/* # line 5363 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iiprotect");
    IIsyncup((char *)0,0);
  }
/* # line 5364 "upgradedb.sc" */	/* host code */
    if (upgr_cb.Dbname_ptr->errors == TRUE)
	return;
    duc_iiprotect();    /* create iiprotect catalog in current format */
    if (upgr_cb.Dbname_ptr->errors == TRUE)
	return;
    /* This is something of a kludge mostly to make verifydb happy.
    ** When we drop iiprotect, the server will clean out the protections
    ** for it;  which is clearly sort of circular, but rows get deleted
    ** from iiqrytext (at least).  Unfortunately we've already picked
    ** up a copy of the iiprotect row for iiprotect, and it will get
    ** stuffed back into the new iiprotect;  thus leaving a discrepancy
    ** between iiprotect and iiqrytext.
    ** The cure is to remove iiprotect's row(s) from the temp copy
    ** and properly re-grant iiprotect later on.
    */
/* # line 5379 "upgradedb.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitmp_iiprotect p where exists(select * from iirelation r\
 where lowercase(r.relid)='iiprotect' and lowercase(r.relowner)='$ingr\
es' and p.protabbase=r.reltid and p.protabidx=r.reltidx)");
    IIsyncup((char *)0,0);
  }
/* # line 5384 "upgradedb.sc" */	/* host code */
    check_sql(sqlca.sqlcode);
    /* when we populate iiprotect from iitmp_iiprotect, all non-specified
    ** fields will take on default values */
    if (!prior_65 && !prior_63)
    {
	/* r3 conversion: more domset columns */
	if (copy_existing_columns(upgr_cb.Dbname_ptr->dbname, "iiprotect") != E_DU_OK)
	    return;
/* # line 5392 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iitmp_iiprotect");
    IIsyncup((char *)0,0);
  }
/* # line 5393 "upgradedb.sc" */	/* host code */
	duc_m4_protect();
/* # line 5394 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 5395 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprotect set prodomset11= -1, prodomset12= -1, prodomset13= -\
1, prodomset14= -1, prodomset15= -1, prodomset16= -1, prodomset17= -1,\
 prodomset18= -1, prodomset19= -1, prodomset20= -1, prodomset21= -1, p\
rodomset22= -1, prodomset23= -1, prodomset24=");
    IIwritio(0,(short *)0,1,32,0,(char *)
" -1, prodomset25= -1, prodomset26= -1, prodomset27= -1, prodomset28= \
-1, prodomset29= -1, prodomset30= -1, prodomset31= -1, prodomset32= -1\
, prodomset33= -1 where prodomset1= -1 and prodomset2= -1 and prodomse\
t3= -1 and prodomset4= -1 and prodomset5= -1 ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"and prodomset6= -1 and prodomset7= -1 and prodomset8= -1 and prodomse\
t9= -1 and prodomseta= -1");
    IIsyncup((char *)0,0);
  }
/* # line 5408 "upgradedb.sc" */	/* host code */
	check_sql(sqlca.sqlcode);
/* # line 5409 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 5410 "upgradedb.sc" */	/* host code */
	/* Fix junk leftover from pre-R3 conversions from 6.4:  it put
	** P instead of E into the object type for converted dbevent
	** permits.  (Permits created post-64 are OK.)
	*/
/* # line 5414 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprotect p from iievent ev set probjtype='E', probjname=ev.ev\
ent_name, probjowner=ev.event_owner where ev.event_idbase=p.protabbase\
 and ev.event_idx=p.protabidx and p.probjtype!='E'");
    IIsyncup((char *)0,0);
  }
/* # line 5421 "upgradedb.sc" */	/* host code */
	check_sql(sqlca.sqlcode);
	return;
    }
    if (!prior_63)
    {
	/* it's already at 6.3, so just fill in 6.5 fields */
/* # line 5427 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiprotect(protabbase, protabidx, propermid, protodbgn, pr\
otodend, prodowbgn, prodowend, proqryid1, proqryid2, protreeid1, protr\
eeid2, proopset, prouser, proterm, progtype, proseq, prodomset1, prodo\
mset2, prodomset3, prodomset4, prodomset5, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"prodomset6, prodomset7, prodomset8, prodomset9, prodomseta, proreserv\
e, prograntor, proopctl, probjtype, procrtime1, procrtime2)select prot\
abbase, protabidx, propermid, protodbgn, protodend, prodowbgn, prodowe\
nd, proqryid1, proqryid2, protreeid1, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"protreeid2, proopset, prouser, proterm, progtype, proseq, prodomset1,\
 prodomset2, prodomset3, prodomset4, prodomset5, prodomset6, prodomset\
7, prodomset8, prodomset9, prodomseta, proreserve, dbmsinfo('dba'), pr\
oopset, 'P', proqryid1, proqryid2 from ");
    IIwritio(0,(short *)0,1,32,0,(char *)"iitmp_iiprotect");
    IIsyncup((char *)0,0);
  }
/* # line 5443 "upgradedb.sc" */	/* host code */
	/* 
	** Mark SQL permits (from iiqrytext) as SQL in iiprotect as well
	** this is done since  verifydb does a consistency check between
	** the two flags
	*/
/* # line 5448 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprotect p set proflags=1 where exists(select * from iiqrytex\
t q where q.txtid1=p.proqryid1 and q.txtid2=p.proqryid2 and q.status=2\
)and proflags=0");
    IIsyncup((char *)0,0);
  }
/* # line 5456 "upgradedb.sc" */	/* host code */
	/* Fix up newly added r3 columns */
/* # line 5457 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprotect set prodomset11= -1, prodomset12= -1, prodomset13= -\
1, prodomset14= -1, prodomset15= -1, prodomset16= -1, prodomset17= -1,\
 prodomset18= -1, prodomset19= -1, prodomset20= -1, prodomset21= -1, p\
rodomset22= -1, prodomset23= -1, prodomset24=");
    IIwritio(0,(short *)0,1,32,0,(char *)
" -1, prodomset25= -1, prodomset26= -1, prodomset27= -1, prodomset28= \
-1, prodomset29= -1, prodomset30= -1, prodomset31= -1, prodomset32= -1\
, prodomset33= -1 where prodomset1= -1 and prodomset2= -1 and prodomse\
t3= -1 and prodomset4= -1 and prodomset5= -1 ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"and prodomset6= -1 and prodomset7= -1 and prodomset8= -1 and prodomse\
t9= -1 and prodomseta= -1");
    IIsyncup((char *)0,0);
  }
/* # line 5470 "upgradedb.sc" */	/* host code */
	check_sql(sqlca.sqlcode);
    }
    else
    {
	/* pre-6.3, so fill in 6.3 and 6.5 fields */
/* # line 5475 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiprotect(protabbase, protabidx, propermid, protodbgn, pr\
otodend, prodowbgn, prodowend, proqryid1, proqryid2, protreeid1, protr\
eeid2, proopset, prouser, proterm, prodomset1, prodomset2, prodomset3,\
 prodomset4, prograntor, proopctl, probjtype)");
    IIwritio(0,(short *)0,1,32,0,(char *)
"select protabbase, protabidx, propermid, protodbgn, protodend, prodow\
bgn, prodowend, proqryid1, proqryid2, protreeid1, protreeid2, proopset\
, prouser, proterm, prodomset1, prodomset2, prodomset3, prodomset4, db\
msinfo('dba'), proopset, 'P' from ");
    IIwritio(0,(short *)0,1,32,0,(char *)"iitmp_iiprotect");
    IIsyncup((char *)0,0);
  }
/* # line 5487 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprotect set progtype=3 where prouser=' %'");
    IIsyncup((char *)0,0);
  }
/* # line 5488 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprotect set prodomset5= -1, prodomset6= -1, prodomset7= -1, \
prodomset8= -1, prodomset9= -1, prodomseta= -1, prodomset11= -1, prodo\
mset12= -1, prodomset13= -1, prodomset14= -1, prodomset15= -1, prodoms\
et16= -1, prodomset17= -1, prodomset18= -1, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"prodomset19= -1, prodomset20= -1, prodomset21= -1, prodomset22= -1, p\
rodomset23= -1, prodomset24= -1, prodomset25= -1, prodomset26= -1, pro\
domset27= -1, prodomset28= -1, prodomset29= -1, prodomset30= -1, prodo\
mset31= -1, prodomset32= -1, prodomset33= -1 ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"where prodomset1= -1 and prodomset2= -1 and prodomset3= -1 and prodom\
set4= -1");
    IIsyncup((char *)0,0);
  }
/* # line 5502 "upgradedb.sc" */	/* host code */
    }
    if (upgr_cb.Dbname_ptr->errors == TRUE)
	return;
    /* now fill in 6.5 specific fields */
    /* FIX_ME: this may not be correct.  I am waiting for inputs from
    **	    Ralph Harnden about how to correctly populate these
    **	    fields and I have just guessed at how to do it. 
    **  populate as follows:
    **	proopctl	-- set to proopset
    **	provalue	-- default
    **	profill2	-- default
    **	probjtype	-- set all to P, the update to either T,V or
    **			    I based on relstat
    **	probjstat	-- default
    **	probjname	-- iirelation.relid or iiprocedure.dbp_name
    **	probjowner	-- iirelation.relowner or iiprocedure.dbp_owner
    **	prograntor	-- dba name
    **	profill3	-- default
    **	proflags	-- 1 if SQL, 0 otherwise
    */
    if (upg_dbg_out)
	SIprintf("]==> Updating specific permit fields...\n");
    /* set probjtype for everything except for procedures -- this field
    ** was initialized to procedures */
/* # line 5526 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprotect from iirelation set probjtype=charextract('TVI', (mo\
d((relstat/32), (2)) +(2*(mod((relstat/128), (2))))) +1)where protabba\
se=reltid and protabidx=reltidx");
    IIsyncup((char *)0,0);
  }
/* # line 5531 "upgradedb.sc" */	/* host code */
    check_sql(sqlca.sqlcode);
    /* Don't forget dbevents.  Fortunately, events use the same
    ** tabid space as tables and procs.
    */
/* # line 5535 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprotect p from iievent ev set probjtype='E', probjname=ev.ev\
ent_name, probjowner=ev.event_owner where ev.event_idbase=p.protabbase\
 and ev.event_idx=p.protabidx");
    IIsyncup((char *)0,0);
  }
/* # line 5541 "upgradedb.sc" */	/* host code */
    check_sql(sqlca.sqlcode);
    /* get object name/owner for tables, views, indexes */
/* # line 5543 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprotect from iirelation set probjname=relid, probjowner=relo\
wner where protabbase=reltid and protabidx=reltidx");
    IIsyncup((char *)0,0);
  }
/* # line 5546 "upgradedb.sc" */	/* host code */
    check_sql(sqlca.sqlcode);
    /* get object name/owner for procedures */
/* # line 5548 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprotect from iiprocedure set probjname=dbp_name, probjowner=\
dbp_owner where protabbase=dbp_id and protabidx=dbp_idx");
    IIsyncup((char *)0,0);
  }
/* # line 5551 "upgradedb.sc" */	/* host code */
    check_sql(sqlca.sqlcode);
    if (upgr_cb.Dbname_ptr->errors == TRUE)
	return;
    /* get rid of the temp table */
/* # line 5555 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iitmp_iiprotect");
    IIsyncup((char *)0,0);
  }
/* # line 5556 "upgradedb.sc" */	/* host code */
    /* sysmod iiprotect */
    duc_m4_protect();
}
/*{
** Name: cv_pre64_integrity	Convert iiintegrity for 6.3
**
** Description:
**	Convert iiintegrity for 6.3
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	17-jun-89 (ralph)
**	    Initial creation for CV60TO70
**	20-Sep-2004 (schka24)
**	    Don't get faked out by ansi uppercase databases
[@history_template@]...
*/
static VOID
cv_pre64_integrity()
{
  int rows_integrity;
  i4 tid_integrity;
/* # line 5794 "upgradedb.qsc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(tid_integrity=iiattribute.tid)where lowercase(iiattribute.at\
tname)=\"intseq\" and iiattribute.attrelid=");
    IIputdomio((short *)0,1,30,sizeof(DU_B_INTEGRITY_TAB_ID),(void *)IILQint(
    DU_B_INTEGRITY_TAB_ID));
    IIwritio(0,(short *)0,1,32,0,(char *)"and iiattribute.attrelidx=");
    IIputdomio((short *)0,1,30,sizeof(DU_I_INTEGRITY_TAB_ID),(void *)IILQint(
    DU_I_INTEGRITY_TAB_ID));
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE9;
IIrtB9:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tid_integrity);
      if (IIerrtest() != 0) goto IIrtB9;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE9:;
  }
/* # line 5801 "upgradedb.qsc" */	/* inquire_ingres */
  {
    IIeqiqio((short *)0,1,30,4,&rows_integrity,(char *)"ROWCOUNT");
  }
/* # line 5804 "upgradedb.qsc" */	/* host code */
	/* Exit if this field is present */
	if (rows_integrity != 0)
	{
	    return;
	}
	du_talk(DU_MODESENS, &upgr_cb, I_DU00A5_UPG_IIINTEGRITY, 0);
	upgr_cb.Ignore_errors = TRUE;
/* # line 5813 "upgradedb.qsc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"destroy iiintegrity_y, iiintegrity_z");
    IIsyncup((char *)0,0);
  }
/* # line 5814 "upgradedb.qsc" */	/* host code */
	upgr_cb.Ignore_errors = FALSE;
/* # line 5816 "upgradedb.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iiintegrity_z(intseq=i4, intdomset5=i4, intdomset6=i4, intdoms\
et7=i4, intdomset8=i4, intdomset9=i4, intdomseta=i4, intreserve=char(3\
2))");
    IIsyncup((char *)0,0);
  }
/* # line 5822 "upgradedb.qsc" */	/* host code */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
/* # line 5825 "upgradedb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iiintegrity_z(intseq=0, intdomset5=0, intdomset6=0, intdoms\
et7=0, intdomset8=0, intdomset9=0, intdomseta=0, intreserve=\" \")");
    IIsyncup((char *)0,0);
  }
/* # line 5831 "upgradedb.qsc" */	/* host code */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
/* # line 5834 "upgradedb.qsc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve iiintegrity_y(iiintegrity.inttabbase, iiintegrity.inttabidx,\
 iiintegrity.intqryid1, iiintegrity.intqryid2, iiintegrity.inttreeid1,\
 iiintegrity.inttreeid2, iiintegrity.intresvar, iiintegrity.intnumber,\
 iiintegrity_z.intseq, iiintegrity.intdomset1");
    IIwritio(0,(short *)0,1,32,0,(char *)
", iiintegrity.intdomset2, iiintegrity.intdomset3, iiintegrity.intdoms\
et4, iiintegrity_z.intdomset5, iiintegrity_z.intdomset6, iiintegrity_z\
.intdomset7, iiintegrity_z.intdomset8, iiintegrity_z.intdomset9, iiint\
egrity_z.intdomseta, iiintegrity_z.intreserve)");
    IIsyncup((char *)0,0);
  }
/* # line 5846 "upgradedb.qsc" */	/* host code */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
/* # line 5849 "upgradedb.qsc" */	/* replace */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"replace iiintegrity_y(intdomset5=-1, intdomset6=-1, intdomset7=-1, in\
tdomset8=-1, intdomset9=-1, intdomseta=-1)where iiintegrity_y.intdomse\
t1=-1 and iiintegrity_y.intdomset2=-1 and iiintegrity_y.intdomset3=-1 \
and iiintegrity_y.intdomset4=-1");
    IIsyncup((char *)0,0);
  }
/* # line 5859 "upgradedb.qsc" */	/* host code */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
/* # line 5862 "upgradedb.qsc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy iiintegrities");
    IIsyncup((char *)0,0);
  }
/* # line 5864 "upgradedb.qsc" */	/* host code */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
/* # line 5867 "upgradedb.qsc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy iiintegrity");
    IIsyncup((char *)0,0);
  }
/* # line 5869 "upgradedb.qsc" */	/* host code */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
/* # line 5872 "upgradedb.qsc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve iiintegrity(iiintegrity_y.inttabbase, iiintegrity_y.inttabid\
x, iiintegrity_y.intqryid1, iiintegrity_y.intqryid2, iiintegrity_y.int\
treeid1, iiintegrity_y.inttreeid2, iiintegrity_y.intresvar, iiintegrit\
y_y.intnumber, iiintegrity_y.intseq, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"iiintegrity_y.intdomset1, iiintegrity_y.intdomset2, iiintegrity_y.int\
domset3, iiintegrity_y.intdomset4, iiintegrity_y.intdomset5, iiintegri\
ty_y.intdomset6, iiintegrity_y.intdomset7, iiintegrity_y.intdomset8, i\
iintegrity_y.intdomset9, iiintegrity_y.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"intdomseta, iiintegrity_y.intreserve)");
    IIsyncup((char *)0,0);
  }
/* # line 5884 "upgradedb.qsc" */	/* host code */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
/* # line 5887 "upgradedb.qsc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iiintegrity to HASH on inttabbase, inttabidx where MINPAGES=11");
    IIsyncup((char *)0,0);
  }
/* # line 5893 "upgradedb.qsc" */	/* host code */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
/* # line 5896 "upgradedb.qsc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"destroy iiintegrity_y, iiintegrity_z");
    IIsyncup((char *)0,0);
  }
/* # line 5899 "upgradedb.qsc" */	/* host code */
}
/*{
** Name: cv_rule	- Create iirule
**
** Description:
**	Create the iirule catalog
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	17-jun-89 (ralph)
**	    Initial creation for CV60TO70
**	30-Aug-2004 (schka24)
**	    if we need to do this, create to current level.
[@history_template@]...
*/
static VOID
cv_rule()
{
  int rows_rule;
  i4 tid_rule;
/* # line 5926 "upgradedb.qsc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(tid_rule=iirelation.tid)where iirelation.reltid=");
    IIputdomio((short *)0,1,30,sizeof(DU_B_RULE_TAB_ID),(void *)IILQint(
    DU_B_RULE_TAB_ID));
    IIwritio(0,(short *)0,1,32,0,(char *)"and iirelation.reltidx=");
    IIputdomio((short *)0,1,30,sizeof(DU_I_RULE_TAB_ID),(void *)IILQint(
    DU_I_RULE_TAB_ID));
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE10;
IIrtB10:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tid_rule);
      if (IIerrtest() != 0) goto IIrtB10;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE10:;
  }
/* # line 5932 "upgradedb.qsc" */	/* inquire_ingres */
  {
    IIeqiqio((short *)0,1,30,4,&rows_rule,(char *)"ROWCOUNT");
  }
/* # line 5935 "upgradedb.qsc" */	/* host code */
	/* Exit  if this catalog is present */
	if (rows_rule != 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, I_DU00A6_IIRULE_EXISTS, 0);
	    return;
	}
	du_talk(DU_MODESENS, &upgr_cb, I_DU00A7_CRE_RULES_CATS, 0);
	upgr_cb.Ignore_errors = TRUE;
/* # line 5945 "upgradedb.qsc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy iirules");
    IIsyncup((char *)0,0);
  }
/* # line 5946 "upgradedb.qsc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy iirule");
    IIsyncup((char *)0,0);
  }
/* # line 5947 "upgradedb.qsc" */	/* host code */
	upgr_cb.Ignore_errors = FALSE;
	duc_iirule();
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
	duc_modify_iirule();
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
}
/*{
** Name: cv_event	- Create table iievent, view iievents, and (later)
**			  update iipermits to reflect iievent.
**
** Description:
**	Create the iievent catalog and dependencies
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	10-dec-90 (neil)
**	    Alerters: first written for 6.3/04.
**	29-oct-91 (teresa)
**	    removed semicolens from end of quel statements, as this sometimes
**	    generates equel errors.  Also removed meaningless drop commands
**	    that occur immediately after a test to verify that the catalog
**	    does not exist.
**	30-Aug-2004 (schka24)
**	    use create utility, create to current layout.
*/
static VOID
cv_event()
{
  int rows_event;
  i4 tid_event;
  int tabid_event = DM_B_EVENT_TAB_ID;
/* # line 5991 "upgradedb.qsc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(tid_event=iirelation.tid)where iirelation.reltid=");
    IIputdomio((short *)0,1,30,4,&tabid_event);
    IIwritio(0,(short *)0,1,32,0,(char *)"and iirelation.reltidx=0");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE11;
IIrtB11:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tid_event);
      if (IIerrtest() != 0) goto IIrtB11;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE11:;
  }
/* # line 5994 "upgradedb.qsc" */	/* inquire_ingres */
  {
    IIeqiqio((short *)0,1,30,4,&rows_event,(char *)"ROWCOUNT");
  }
/* # line 5996 "upgradedb.qsc" */	/* host code */
	/* Return if this catalog exists */
	if (rows_event == 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, I_DU00A8_CRE_IIEVENT, 0);
	    duc_iievent();
	    if (upgr_cb.Dbname_ptr->errors == TRUE)
   		return;
	    duc_modify_iievent();
	    if (upgr_cb.Dbname_ptr->errors == TRUE)
		return;
	    du_talk(DU_MODESENS, &upgr_cb, I_DU00A9_CRE_IIEVENTS, 0);
	}
	else
	    du_talk(DU_MODESENS, &upgr_cb, I_DU00AA_EVENTS_CATS, 0);
	upgr_cb.Ignore_errors = TRUE;
/* # line 6014 "upgradedb.qsc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy iievents");
    IIsyncup((char *)0,0);
  }
/* # line 6015 "upgradedb.qsc" */	/* host code */
	upgr_cb.Ignore_errors = FALSE;
} /* cv_event */
/*{
** Name: cv_star_cats - Master to convert star CDB catalogs
**
** Description:
**	This routine makes star CDB catalogs compatabile with what
**	the current star server expects.
**
**	If CDB was at level 6.2, then it needs the star 2pc catalogs added
**	to bring up to 6.3 compatibility, plus any other changes required to
**	bring up to current level compatability.
**
**	If CDB was at level 6.3, then it needs a little touch up to the
**	iidbcapabilities catalog to bring up to 6.4, plus any other changes
**	required to bring it up to the current level compatability.
**
**	If CDB was at level 6.4, then we need to create the STAR STANDARD
**	CATALOGS.  The current 6.4 star standard catalogs include:
**		iiregisted_objects
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Side Effects:
**	none.
**
** History:
**	03-may-91 (teresa)
**	    Created for STAR.
**	16-oct-91 (teresa)
**	    added support for 6.5 star standard catalog iiregistered_objects.
**	10-Jan-93 (jpk)
**	    added support for 6.5 star standard catalog iiprocedures
**      06-mar-1996 (nanpr01)
**	    added support for 2.0 star standard catalog. Force update of
**	    star standard catalog.
**	21-Sep-2004 (schka24)
**	    Drop obsolete (6.4?) catalog views.
*/
static VOID
cv_star_cats()
{
    du_talk(DU_MODESENS, &upgr_cb, I_DU0114_CONVERTING_STAR, 0);
    switch (upgr_cb.Dbname_ptr->level)
    {
    case V61_LEVEL:
	/* there was no STAR in 6.1, so there is nothing to do */
	break;	
    case V62_LEVEL:
	/* some earlier releases of 6.2 did NOT have star 2PC catalogs.  So,
	** call a routine to assure they are created, then do the same
	** conversion we did for 6.3 by falling through to the 6.3 case */
	star_2pc();
    case V63_LEVEL:
	/* 6.2 and 6.3 star cdbs are identical, so upgrade them to 6.4 */
	starto64();
    case V64_LEVEL:
	/* 6.4 level, so build star starndard catalogs */
	star_std_cats();
    case V65_LEVEL:
    case V65ES_LEVEL:
    case V12_LEVEL:
    case V20_LEVEL:
    case V25_LEVEL:
    case V26_LEVEL:
	/* Refresh Star version of iitables, etc */
	star_20_std_cats(upgr_cb.Dbname_ptr->upcase);
	/* Drop a couple of obsolete Star catalog views.  I don't know
	** how far back these go, so just pitch them, no errors.
	*/
/* # line 5946 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 5947 "upgradedb.sc" */	/* host code */
	upgr_cb.Ignore_errors = TRUE;
/* # line 5948 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop iidd_ddb_finddbs");
    IIsyncup((char *)0,0);
  }
/* # line 5949 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop iidd_views_cat");
    IIsyncup((char *)0,0);
  }
/* # line 5950 "upgradedb.sc" */	/* host code */
	upgr_cb.Ignore_errors = FALSE;
/* # line 5951 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 5952 "upgradedb.sc" */	/* host code */
	break;
    default:
	/* already at current level, so break */
	if ((upgr_cb.upgtrace[UPG02_STAR_STDCATS]) &&
	    (upgr_cb.Dbname_ptr->level > V12_LEVEL))
	{
	    /* force star standard catalogs to be built if they dont exist */
	    star_std_cats();
	    star_20_std_cats(upgr_cb.Dbname_ptr->upcase);
	}
	break;
    }
}
/*{
** Name: starto64 - make 6.2 or 6.3 CDB 6.4 compatabile
**
** Description:
**	This routine makes star 6.2 or 6.3 CDB catalogs compatabile with
**	6.4.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Side Effects:
**	none.
**
** History:
**	03-may-91 (teresa)
**	    Created for STAR.
*/
static VOID
starto64()
{
  i4 cnt;
    /* change INGRES/QUEL_LEVEL from 00100 to 00000 in iidd_dbcapabilities */
/* # line 5992 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iidd_dbcapabilities set cap_value=");
    IIputdomio((short *)0,1,32,5,DU_DB5_STAR_INGQUEL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)"where cap_capability=");
    IIputdomio((short *)0,1,32,17,DU_DB5_INGQUEL_LEVEL);
    IIsyncup((char *)0,0);
  }
/* # line 5994 "upgradedb.sc" */	/* host code */
    /* add STANDARD_CATALOG_LEVEL to iidd_dbcapabilities */
/* # line 5995 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(cap_capability)from iidd_dbcapabilities where cap_capabi\
lity=");
    IIputdomio((short *)0,1,32,22,DU_DB6_STDCAT_LEVEL);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 5997 "upgradedb.sc" */	/* host code */
    if (cnt==0)
    /* 6.2 STANDARD_CATALOG_LEVEL -- 6.2, 6.3 and 6.4 star all ship with a
    ** 6.2 standard catalog interface */
/* # line 6000 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidd_dbcapabilities(cap_capability, cap_value)values(");
    IIputdomio((short *)0,1,32,22,DU_DB6_STDCAT_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,5,DU_DB6_STAR_STDCAT_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 6002 "upgradedb.sc" */	/* host code */
}
/*{
** Name: star_std_cats() - build the star standard catalog interface
**
** Description:
**
**	This routine builds the star standard catalog interface.  This is
**	currently comprised of:
**
**	Star Standard Catalog ii_registered_objects:
**	    COL NAME		DATA TYPE   DESCRIPTION
**	    -----------------  ---------    ------------------------------------
**	    ddb_object_name    char(32)	    name of star registered object (the 
**					    registered object will be a table, 
**					    view or index)
**	    ddb_object_owner   char(32)	    owner of star registered object
**	    register_date      char(25)	    date object was registered
**	    ldb_database       char(32)	    name of LDB database that registered
**					    object resides in
**	    ldb_node	       char(32)	    node that ldb_database resides on
**	    ldb_dbmstype       char(32)	    type of database ldb_database is 
**					    ('INGRES', 'RMS', 'DB2','RDB' etc).
**					    This is the same types used by 
**					    iinamu.
**	    ldb_object_name    char(32)	    name the LDB uses for the 
**					    registered object
**	    ldb_object_owner   char(32)	    owner of the registered object in 
**					    the LDB
**	    ldb_object_type    char(8)	    Type of local object:  
**						"T" for table,
**						"V" for view, 
**						"I" for index.
**
**
**	The stardard catalog is created in the CDB with the "ii" prefix being
**	replaced with the "iidd_" prefix. Example:  iiregistered_objects is
**	created in the LDB as iidd_registered_objects.
**
**	Then a routine is called to populate the STAR catalogs from the CDB
**	catalogs to make the star standard catalogs known to the installation.
**	This routine is shared between createdb and upgradedb, so it resides
**	in ducommon.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Side Effects:
**	none.
**
** History:
**	16-oct-91 (teresa)
**	    Created for STAR.
*/
static VOID
star_std_cats()
{
  i4 obj_base;
    DU_STATUS	stat;
/* # line 6065 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select object_base +1 from iidd_ddb_object_base");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&obj_base);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 6066 "upgradedb.sc" */	/* host code */
    stat=   duc_register_starcats(obj_base, &DU_upg_object_map[0], 
				  &DU_upg_idx_map[0], 
				  upgr_cb.Dbname_ptr->cdbname,
				  &upgr_cb.du_msg);
    if (stat != E_DU_OK)
    {
	upgr_cb.Dbname_ptr->errors = TRUE;
	du_talk(DU_ERR_FMTD, &upgr_cb, 0, upgr_cb.du_msg->du_utilerr, 0);
	return;
    }
}
/*{
** Name: star_2pc - make star 2pc catalogs
**
** Description:
**	This routine makes star 2pc catalogs for 6.1 CDBs that are being
**	converted to 6.4 or above.  If the catalog already exists, this routine
**	does NOT touch them.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Side Effects:
**	none.
**
** History:
**	03-may-91 (teresa)
**	    Created for STAR.
*/
static VOID
star_2pc()
{
    if (!rel_exists("iidd_ddb_dxlog"))
    {
/* # line 6103 "upgradedb.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iidd_ddb_dxlog(dx_id1 integer not null not default, dx_i\
d2 integer not null not default, dx_name char(32)not null not default,\
 dx_flags integer not null not default, dx_state integer not null not \
default, dx_create char(25)not null not ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"default, dx_modify char(25)not null not default, dx_starid1 integer n\
ot null not default, dx_starid2 integer not null not default, dx_ddb_n\
ode char(32)not null not default, dx_ddb_name char(256)not null not de\
fault, dx_ddb_dbms char(32)not null not ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"default, dx_ddb_id integer not null not default)");
    IIsyncup((char *)0,0);
  }
/* # line 6117 "upgradedb.sc" */	/* create unique index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create unique index iidd_ddb_xdxlog on iidd_ddb_dxlog(dx_id1, dx_id2)");
    IIsyncup((char *)0,0);
  }
/* # line 6119 "upgradedb.sc" */	/* host code */
    }
    if (!rel_exists("iidd_ddb_dxldbs"))
    {
/* # line 6122 "upgradedb.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iidd_ddb_dxldbs(ldb_dxid1 integer not null not default, \
ldb_dxid2 integer not null not default, ldb_node char(32)not null not \
default, ldb_name char(256)not null not default, ldb_dbms char(32)not \
null not default, ldb_id integer not null not ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"default, ldb_lxstate integer not null not default)");
    IIsyncup((char *)0,0);
  }
/* # line 6130 "upgradedb.sc" */	/* host code */
    }
}
/*{
** Name: upgr_exit	- upgradedb exit handler
**
** Description:
**
**  Exit neatly by:
**  1) Print any user specified messages, specified via parameter msg_id. That
**	parameter must specify a message that does not require any parameters.
**	If DU_OK_EXIT was specified, the message will be printed according
**	to the message mode (ie, DU_MODESENS).  If DU_FAIL_EXIT was specified,
**	the message will always be printed.
**  2) Close the upgradedb log file.
**  3) disconnect from any dbs that upgradedb may be connected to.  If
**     any DBs are left open when the exit handler is called, it will
**     abort the transaction if DU_FAIL_EXIT is specified and commit the 
**     transaction if DU_OK_EXIT is specified.  
**
** Inputs:
**	    exit_stat		    Exit status to return to invoker:
**					DU_OK_EXIT -- returns PCexit(OK)
**					DU_FAIL_EXIT -- returns PCexit(FAIL)
**				        NOTE: if value is neither DU_OK_EXIT
**					      nor DU_FAIL_EXIT, then it is 
**					      forced to DU_FAIL_EXIT.
**	    msg_id			message to print on exit
**
** Outputs:
**	none
**	Returns:
**	    OK via PC_EXIT
**	    FAIL via PC_EXIT
**	Exceptions:
**	    none
**
** Side Effects:
**	    log file is closed
**	    if any ESQL transaction exists, it is aborted
**
** History:
**      23-jul-91 (teresa)
**          Initial creation.
**      13-mar-98 (nicph02)
**          Added call to rm_iiqef_procedures to drop iiqef_ procedures.
[@history_template@]...
*/
VOID
upgr_exit(exit_stat, msg_id)
DU_STATUS	    exit_stat;
i4		    msg_id;
{
    i4		msg_mode;
/* # line 6184 "upgradedb.sc" */	/* host code */
    if (exit_stat == DU_OK_EXIT)
	msg_mode = DU_MODESENS;
    else if (exit_stat == DU_FAIL_EXIT)
	msg_mode = DU_ALWAYS;
    else
    {
	/* invalid exit_stat was specified, so force to FAIL */
	exit_stat = DU_FAIL_EXIT;
	msg_mode = DU_ALWAYS;
    }
    /* caller may specify a message to print.  If a message is specified,
    ** print it with the correct message mode */
    if (msg_id)
        du_talk (msg_mode, &upgr_cb, msg_id, 0);
    /* see if there is a database to close. */
    if (upgr_cb.Connected)
    {
	if (upg_dbg_out)
	{
	    SIprintf("]==> (j) setting session to user database\n");
	}
/* # line 6205 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DB_SESSIONID),(void *)IILQint(
    DB_SESSIONID));
  }
/* # line 6206 "upgradedb.sc" */	/* host code */
	if (exit_stat == DU_OK_EXIT)
/* # line 6207 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 6208 "upgradedb.sc" */	/* host code */
	else 
/* # line 6209 "upgradedb.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 6210 "upgradedb.sc" */	/* host code */
	IIlq_Protect(FALSE);		/* Remove catalog update permission */
        rm_iiqef_procedures();    /* Bug 89164: remove all iiQEF procedures */
/* # line 6212 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 6213 "upgradedb.sc" */	/* host code */
    }
    if (upgr_cb.dbdb_open)
    {
	if (upg_dbg_out)
	{
	    SIprintf("]==> (k) setting session to database database\n");
	}
/* # line 6220 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DBDB_SESSIONID),(void *)IILQint(
    DBDB_SESSIONID));
  }
/* # line 6221 "upgradedb.sc" */	/* host code */
	if (exit_stat == DU_OK_EXIT)
/* # line 6222 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 6223 "upgradedb.sc" */	/* host code */
	else 
/* # line 6224 "upgradedb.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 6225 "upgradedb.sc" */	/* host code */
	IIlq_Protect(FALSE);
/* # line 6226 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 6227 "upgradedb.sc" */	/* host code */
    }
    du_fclose(&upgr_cb.du_log, upgr_cb.du_msg);
    if (exit_stat == DU_OK_EXIT)
	    PCexit(OK);
    else
	    PCexit(FAIL);
}
/*{
** Name: validate_db - verify db is known to the installation
**
** Description:
**
**  Search the iidatabase catalog to see if the input dbname is in that
**  catalog.  If not, return FALSE.  If so, return TRUE.  In either case,
**  there is a special set lock on iidatabase, so be sure to do a commit
**  after accessing iidatabase.
**
**  This routine is necessary because there is a short window where a database
**  name may have been cached and then some other session did a destroydb on 
**  this database.  So, if upgradedb cannot connect to the target database, 
**  then we need to assure that the database still exists.  The usual case
**  will be that the database does indeed still exist.
**
** Inputs:
**	dbname			name of database (null terminated)
**
** Outputs:
**	none
**	Returns:
**	    TRUE		=> database is still known to installation
**	    FALSE		=> database is no longer known to installation
**	Exceptions:
**	    none
**
** Side Effects:
**	    Session is switched to iidbdb session and then switched back.
**	    A brief lock is taken (then released) on the iidatabase
**
** History:
**	07-aug-91  (teresa)
**	    initial creation.
*/
static DU_STATUS
validate_db(dbname)
char	*dbname;
{
    char	*tail;
  char dname[DB_MAXNAME+1];
  i4 cnt;
    if (upgr_cb.du_dbmstype == DU_DBRMS)
    {
	/* strip off the RMS tail */
	tail = STindex(dbname, "/", DB_MAXNAME+1);
	*tail = '\0';
    }
    STcopy(dbname,dname);
    if (upg_dbg_out)
    {
	SIprintf("]==> (l) setting session to database database\n");
    }
/* # line 6289 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DBDB_SESSIONID),(void *)IILQint(
    DBDB_SESSIONID));
  }
/* # line 6290 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(name)from iidatabase where name=");
    IIputdomio((short *)0,1,32,0,dname);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 6292 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 6293 "upgradedb.sc" */	/* host code */
    return (cnt == 1);
}
/*{
** Name: du_fopen - Open/create a log directory
**
** Description:
**      This routine creates a log directory path and ensures that the
**	directory exists.  This directory is also used as a work directory
**	when doing fix-query-tree operations.  The directory is:
**		II_SYSTEM~ingres~files~logdirname~subdirname
**	Where ~ is an abstraction for path specifications that are actually 
**	specific to various operating systems.
**
**	A pictorial representation of the directory structure is:
**
**			II_SYSTEM
**			   |
**			INGRES
**			   |
**			FILES
**			   |
**			Logdirname
**			   |
**	    +--------------+-------------------------+
**	    |		   |			     |
**	subdirname1	subdirname2	...	subdirnameN
**	    |		   |			     |
**	filename	filename		filename
**
**
**	If the subdirname parameter is specified and that subdirectory does not
**	already exist, then this routine creates it.  If the logdirname 
**	parameter is specified and the directory does not already exist, then
**	this routine creates it.
**
** Inputs:
**	fp				Status or Log file descriptor.
**	logdirname			Name of log directory
**	subdirname			Name of subdirectory that log file is
**					to reside in.  
**
** Outputs:
**	None
**
** Side Effects:
**	 If the 'subdirname' or 'logdirname' directory doesn't exist, then
**	 the directory is created.
**
** History:
**	8-aug-91 (teresa)
**	    initial creation.
**	3-Jan-2005 (schka24)
**	    Split up original du_fopen, trees need work dir even if -nolog.
[@history_template@]...
*/
static VOID
du_create_logdir(fp, logdirname, subdirname)
DU_FILE		    *fp;
char		    *logdirname;
char		    *subdirname;
{
    STATUS	    cl_stat;
    char	    *cp;
    char	    logname[LO_NM_LEN+1];   /* buffer to hold largest legal
					    ** directory name length */
    char	    dirname[LO_NM_LEN+1];   /* buffer to hold largest legal
					    ** subdirectory name length */
    LOCATION	    temploc;
    LOINFORMATION   locinfo;
    i4		    flags = LO_I_TYPE;
    /* initialization */
    fp->du_flocbuf[0] = '\0';
    MEfill (sizeof(locinfo), '\0', (PTR) &locinfo);
    /* use as many characters in subdir name as are legal for this host. */
    STncpy (dirname, subdirname, LO_NM_LEN);    
    dirname[LO_NM_LEN]='\0';
    STncpy (logname, logdirname, LO_NM_LEN);
    logname[LO_NM_LEN]='\0';
    /* build the pathname to the parent directory.
    ** Typically this is $II_SYSTEM/ingres/upgradedb
    */
    cl_stat = NMloc(FILES, PATH, logname, &temploc);
    if (cl_stat != OK)
    {
	/* print error message that we could not build the pathname to the log 
	** file directory */
	du_talk (DU_ALWAYS, &upgr_cb, E_DU6016_CANT_CRE_DIR, 2, 0, logname);
	upgr_exit(DU_FAIL_EXIT, I_DU0080_UPGRADEDB_TERM);
    }
    LOcopy (&temploc, fp->du_flocbuf, &fp->du_floc);
    /* now see if the directory exists. */
    cl_stat = LOinfo ( &fp->du_floc, &flags, &locinfo);
    if (cl_stat != OK)
    {
	/* location does not exist, so create it */
	cl_stat = LOcreate(&fp->du_floc);
	if (cl_stat != OK)
	{
	    /* print error message that we could not create the directory */
	    du_talk (DU_ALWAYS, &upgr_cb, E_DU6016_CANT_CRE_DIR, 2, 
		    0, logname);
	    upgr_exit(DU_FAIL_EXIT, I_DU0080_UPGRADEDB_TERM);
	}
    }
    /* Always give read/write permission to parent subdirectory.
    ** This is a precaution because we're going to create <username>
    ** subdirs here as potentially non-ingres users, and we need this
    ** directory to be world writable even if it existed.
    ** This call will fail (silently) if we're not the installation owner.
    */
    PEworld("+r+w+x", &fp->du_floc);
    /* Append logfile subdir, typically now $II_SYSTEM/upgradedb/username */
    cl_stat = LOfaddpath(&fp->du_floc, dirname, &fp->du_floc);
    if (cl_stat != OK)
    {
	/* print error message that we could not build the pathname to 
	** the log file directory */
	du_talk (DU_ALWAYS, &upgr_cb, E_DU6016_CANT_CRE_DIR, 2, 
		 0, dirname);
	upgr_exit(DU_FAIL_EXIT, I_DU0080_UPGRADEDB_TERM);
    }
    /* now see if the directory exists. */
    cl_stat = LOinfo ( &fp->du_floc, &flags, &locinfo);
    if (cl_stat != OK)
    {
	/* location does not exist, so create it */
	cl_stat = LOcreate(&fp->du_floc);
	if (cl_stat != OK)
	{
	    /* print error message that we could not create the 
	    ** directory */
	    du_talk (DU_ALWAYS, &upgr_cb, E_DU6016_CANT_CRE_DIR, 2, 
		     0, dirname);
	    upgr_exit(DU_FAIL_EXIT, I_DU0080_UPGRADEDB_TERM);
	}
	/* give read/write permission to subdirectory */
	PEworld("+r+w+x", &fp->du_floc);
    }
    /* Save the directory location for use by others */
    LOcopy(&fp->du_floc, upgr_cb.log_dir_buf, &upgr_cb.log_dir_loc);
}
/*{
** Name: du_fopen - Open/create a log file
**
** Description:
**	This routine appends a filename to the already-built log directory
**	path, and opens that log file in append mode.
**	The default filename is "upgrade.log".
**	We expect that the log/upgradedb work directory has already been
**	set up and created.
**
**	If the log file (filename) does not exist, it will be created.  In
**	either case, it is opened for append.
**
** Inputs:
**	fp				Status or Log file descriptor.
**	filename			Name of the log file.  Examples
**					are "upgrade.log" or "iivdb.log"
**
** Outputs:
**	None
**
** Side Effects:
**	The file is created if needed, and opened for append.
**
** History:
**	3-Jan-2005 (schka24)
**	    Split up original du_fopen.
*/
static VOID
du_fopen(fp, filename)
DU_FILE		    *fp;
char		    *filename;
{
    STATUS	    cl_stat;
    char	    *cp;
    char	    file[LO_NM_LEN+1];	    /* buffer to hold largest legal
					    ** filename length */
    /* use as many characters in file name as are legal for this host. */
    STncpy (file, filename, LO_NM_LEN);
    file[LO_NM_LEN]='\0';
    if (file[0] == '\0')
    {
	/* It is illegal to not specify the filename, so use the default of
	** "upgrade.log"
	*/
	(VOID)STcopy (file, "upgrade.log");
    }
    /* now append the filename to the location */
    cl_stat = LOfstfile ( file, &fp->du_floc);
    /* translate the filename to a string */
    LOtos(&fp->du_floc, &cp);
    STcopy(cp,fp->du_flocbuf);
    /* give read/write permission to the world, just incase ... */
    PEworld("+r+w", &fp->du_floc);
    /* now open the file.  This open will create the file if it does not
    ** exist and append to it if it does exist */
    if ( (cl_stat = SIopen(&fp->du_floc, "a", &fp->du_file)) != OK )
    {
	/* print error message that we could not open the log file */
	du_talk (DU_ALWAYS, &upgr_cb, E_DU6017_CANT_OPEN_LOG, 2, 
		 0, fp->du_flocbuf);
	upgr_exit(DU_FAIL_EXIT, I_DU0080_UPGRADEDB_TERM);
    }
}
/*{
** Name: du_fclose	- close the given file.
**
** Description:
**        This routine is called to close the log file.  Note, if 
**	  the file is not open, this routine is a no-op.
**
** Inputs:
**	errcb			Message control block.
**	fp			log file descriptor.
**	    .du_file		    Pointer for generic file descriptor.
**				    If NULL, then the given file is not open.
**
** Outputs:
**	    errcb			If an error occurs, this struct is
**					set by calling du_error().
**	    fp
**		.du_file		Upon successfully closing the given file
**					this field is set to NULL.
**	Returns:
**	    E_DU_OK			Completed successfully.
**	    E_DU2410_GEN_CL_FAIL	A CL routine failed.
**	Exceptions:
**	    none
**
** Side Effects:
**	      Close the given file.
**
** History:
**      08-aug-91 (teresa)
**          Initial Creation.
*/
DU_STATUS
du_fclose(fp, errcb)
DU_FILE	    *fp;
DU_ERROR    *errcb;
{
    if (fp->du_file != NULL)
    {
	/* The file should be open. */
	if ((errcb->du_clerr=SIclose(fp->du_file))!=OK)
	    return(du_error(errcb, E_DU2410_GEN_CL_FAIL,
			    4, 0, ERx("SIclose"), 0, ERx("du_fclose")));
	else
	    /* Record that the file has been closed. */
	    fp->du_file  = NULL;
    }
    return( (DU_STATUS) E_DU_OK);
}
/*{
** Name: du_log	- Log string to log file
**
** Description
**	This routine outputs a null terminated string to the log
**	file.  The file must be open before this routine is called.  The
**	character string must be null terminated.
**
**	WARNING:  this routine assumes message length is not too long for
**		  a message.  It also assumes (does not take the time to
**		  verify) that the msg buffer is null terminated.  However,
**		  it does verify that the log file is open.
**
**
** Inputs:
**	skip				flag to skip logging
**	fp				log file descriptor
**	msg_buffer			contains null terminated msg to output
**
** Outputs:
**	none
**	Returns:
**	    E_DU_IERROR			(log file was not open)
**	    E_DU_OK	    
**	Exceptions:
**	    none
**
** Side Effects:
**	      msg is output to buffer
**
** History:
**      8-aug-91 (teresa)
**          initial creation
*/
DU_STATUS
du_log(skip, fp, msg_buffer)
bool	    skip;
DU_FILE	    *fp;
char	    *msg_buffer;
{
    /* this is a no-op if the skip logging feature is selected via -n flag */
    if (skip)
	return( (DU_STATUS) E_DU_OK);
    if (fp->du_file == NULL)  /* no open log file */
	return ( (DU_STATUS) E_DU_IERROR);
    SIfprintf(fp->du_file, "    %s\n", msg_buffer);
    SIflush(fp->du_file);
    return ( (DU_STATUS) E_DU_OK);
}
/*{
** Name: du_talk	- handle I/O
**
** Description
**
**	This routine handles output to stdout and to the log file.  It examines
**	message type and mode to determine what I/O actions to take.
**
**  NOTES:  This routine assumes that the log file is open before it is called.
**
**
** Inputs:
**	du_mtype			message type:
**					    DU_ALWAYS
**					    DU_MODESENS
**					    DU_ERR_FMTD
**	cb				control block.  Contains
**					 mode and error control block
**					 and flag indicating if logging is
**					 bypassed
**      du_errno                        error code number indicating which
**					 error message to format
**	p_cnt				#parameters (5 max)
**	(note: parameters optional, but must match p_cnt)
**	p1_len				#bytes length of 1st parameter --
**					 null terminated strings should have
**					 a length of zero
**	p1_ptr				address of 1st parameter
**	p2_len				#bytes length of parameter #2 --
**					 null terminated strings should have
**					 a length of zero
**	p2_ptr				address of 2nd parameter
**	p3_len				#bytes length of parameter #3 --
**					 null terminated strings should have
**					 a length of zero
**	p3_ptr				address of 3rd parameter
**	p4_len				#bytes length of parameter #4 --
**					 null terminated strings should have
**					 a length of zero
**	p4_ptr				address of 4th parameter
**	p5_len				#bytes length of parameter #5 --
**					 null terminated strings should have
**					 a length of zero
**	p5_ptr				address of 5th parameter
**
**
** Outputs:
**      cb				control block.  Contains:
**	    du_msg.du_errmsg		    error message that was output
**	    du_msg.du_utilerr		    error message id that was output
**
**	Returns:
**	    NONE
**	Exceptions:
**	    none
**
** Side Effects:
**	    msg may be redefined to error message
**	    disables du_error from automatically outputting warning/error
**	      msgs and resetting control block.
**
** History:
**  08-aug-91 (teresa)
**	Initial creation.  Simplified from duve_talk
**  13-oct-93 (swm)
**	Bug #56448
**	Changed length parameters type from i4 * to i4 to be
**	consistent with actual usage and description of Inputs, above.
**  05-feb-2001 (horda03) Bug 103873
**      Remove the Ingres error code from the message being displayed.
**      For Warnings and Information messages, the Ingres error code is
**      preceded by a '\t', otherwise the Ingres error code is preceded by a
**      '\n'. See du_error().
*/
/* VARARGS4 */
VOID
du_talk(du_mtype, cb, du_errno, pcnt, p1_len, p1_ptr, 
	p2_len, p2_ptr, p3_len, p3_ptr, p4_len, p4_ptr, p5_len, p5_ptr)
i4		    du_mtype;	    
DU_UPGRADE_CB	    *cb;
i4		    du_errno;
i4		    pcnt;
i4		    p1_len;
i4		    *p1_ptr;
i4		    p2_len;
i4		    *p2_ptr;
i4		    p3_len;
i4		    *p3_ptr;
i4		    p4_len;
i4		    *p4_ptr;
i4		    p5_len;
i4		    *p5_ptr;
{
    char	    *msg_ptr;
    char            *strip;
    i4              err_reported = (i4) du_errno;
    /* Reset the system error control block */
    MEfill( (u_i2) sizeof(DU_ERROR), 0, (PTR) cb->du_msg);
    cb->du_msg->du_ingerr     = DU_NONEXIST_ERROR;
    cb->du_msg->du_utilerr    = DU_NONEXIST_ERROR;
    if (du_mtype == DU_ERR_FMTD)
    {
	/* the message is an already formatted error message, so translate
	** the message type to DU_ALWAYS and skip formatting it 
	*/
	du_mtype = DU_ALWAYS;
    }
    else
    {
	/*
	** tell du_error to format msg only.  Else it will output info/warning 
	** and clear the message buffer.
	*/
	cb->du_msg->du_flags |= DU_SAVEMSG;
	switch(pcnt/2)
	{
	    case 0:
		du_error(cb->du_msg, du_errno, 0);
		break;
	    case 1:
		du_error(cb->du_msg, du_errno, 2,
			  p1_len, p1_ptr);
		break;
	    case 2:
		du_error(cb->du_msg, du_errno, 4,
			  p1_len, p1_ptr, p2_len, p2_ptr);
		break;
	    case 3:
		du_error(cb->du_msg, du_errno, 6,
			  p1_len, p1_ptr, p2_len, p2_ptr, p3_len, p3_ptr);
		break;
	    case 4:
		du_error(cb->du_msg, du_errno, 8,
			  p1_len, p1_ptr, p2_len, p2_ptr, p3_len, p3_ptr,
			  p4_len, p4_ptr);
		break;
	    case 5:
		du_error(cb->du_msg, du_errno, 10,
			  p1_len, p1_ptr, p2_len, p2_ptr, p3_len, p3_ptr,
			  p4_len, p4_ptr, p5_len, p5_ptr);
		break;
	    default:
                err_reported = E_DU2414_BAD_CASE_LABEL;
		du_error(cb->du_msg, E_DU2414_BAD_CASE_LABEL, 4,
				 sizeof (pcnt), &pcnt,
				 0, ERx("DU_TALK"));
	}
    }
    /* Determine which character will follow the message number.
    ** Refer to du_error().
    */
    strip = ( (err_reported < DU_INFO_BOUNDARY) ||
              (err_reported < DU_WARN_BOUNDARY))  ? "\t" : "\n";
    /* strip of message number */
    msg_ptr = STindex(cb->du_msg->du_errmsg, strip, 0);
    if (msg_ptr != NULL)
	msg_ptr++;
    else
	/* could not stip off message number, so use whole message */
	msg_ptr = &cb->du_msg->du_errmsg[0];
    /*
    **  output msg text to stdout:
    **	 if msg type is DU_ALWAYS (reports errors) always output msg to stdout.
    **	 Otherwise, output msg to stdout if mode is not RUNSILENT
    */
    if ( (cb->du_mode != DU_SILENT) || (du_mtype == DU_ALWAYS) )
    {
	SIprintf("%s\n",msg_ptr);
	SIflush(stdout);
    }
    /*
    ** Determine whether or not to output msg to log file.
    */
    if (cb->du_log.du_file != NULL) /* see if log file exists */
    {
	SIfprintf(cb->du_log.du_file, "%s\n", msg_ptr);
	SIflush(cb->du_log.du_file);
    }
}
/*{
** Name: fe_dictinst	- install FE dictionary
**
** Description:
**	Add 6.4 FE dictionary catalogs.  If the user has not specified a
**	client list, then build the client list from ii_authorization, reading
**	file 
**
** Inputs:
**	dbname	- database name
**	dba	- DBA.
**      level   - current db level
**
** Outputs:
**	none
**
** History:
**	5/90 (bobm)
**	     written
**	10/2/90 (pete)
**	     Fix how basecats argument is passed; Pass client "INGRES"; 
**	     pass "silent" flag.
**	05/03/91 (teresa)
**	     fix bug when error status returned from UTexe was used as an
**	     error message code.
**	05/03/91 (teresa)
**	    add star support.
**	09-may-91 (markm)
**	    fix bugs 37007 and 370379 by doing a IIlq_Protect(TRUE) call after
**	    coming back from UTexe call to upgradefe.
**	02-dec-91 (teresa)
**	    change if (upgr_cb.du_clients[0]==EOS) -> if (upgr_cb.du_fecnt == 0)
**	10-Sep-2004 (schka24)
**	    Don't run this if database doesn't at least have one of the
**	    CORE module catalogs.  Otherwise what ends up happening is
**	    that upgrade installs all the latest front-end poop,
**	    willy-nilly.
*/
static	char	ut_args[] =
    ERx("database = %S, equel = %S, user = %S, basecats = %S");
static  char	Fecatins[] = ERx("upgradefe");	/* FE dictionary creat*/
static  char	Empty_str[]    = ERx("");
static VOID
fe_dictinst(dbname,dba, level)
char *dbname;
char *dba;
i4   level;
{
	char		buf[ER_MAX_LEN];
	STATUS  	stat;
	CL_ERR_DESC	err_code;
	i4		cnt_silent = 0;	/* set to 1 if -s flag for UTexe */
	i4		cnt_client = 0; /* set to 1 if client list for UTexe */
	PTR		p_vararg1 = NULL;	/* passed in UTexe call */
	PTR		p_vararg2 = NULL;	/* passed in UTexe call */
	bool		dofe_flag = TRUE;
	char		*duu_xflag();
	i4		i;
  char cdb_name[DB_MAXNAME+1];
  char ddb_name[DB_MAXNAME+DU_TAIL_LEN+1];
  char dba_name[DB_MAXNAME+1];
  char iidbdbname[DB_MAXNAME+1];
	/* If we don't at least have a CORE module catalog, return.
	** ii_dict_modules sounds like a good candidate.
	*/
	if (!rel_exists("ii_dict_modules"))
	    return;
	if (upgr_cb.du_fecnt == 0)
	{
	    /* no client names on cmd line; create client
	    ** catalogs based on user's authorization string. We 
	    ** will create front-end catalogs for everything they
	    ** are authorized to run.
	    ** Also, pass the "-s" flag to "upgradefe".
	    */
	    char clist[DU_MAX_CLIENT_NAME];
	    if (dui_bldDefClientList(clist, upgr_cb.du_msg) == OK)
	    {
		STcopy (clist, upgr_cb.du_feclients);
                if (level == CURRENT_LEVEL)
		{
		 cnt_client = 1;
		 p_vararg1 = (PTR) &upgr_cb.du_feclients[0];
		 STpolycat (2, ut_args, ERx(",client = %S"), buf);
                }
		else
		{
		STpolycat (2, ut_args, ERx(", silent = %S, client = %S"),
				     buf);
                cnt_silent=  1;
		cnt_client = 1;
		p_vararg1 = (PTR) Empty_str;
		p_vararg2 = (PTR) &upgr_cb.du_feclients[0];
		}
	    }
	    else
	    {
		/* Problems locating or reading file "prods.lst". Issue
		** warning message & continue.
		*/
		du_talk(DU_MODESENS, &upgr_cb, W_DU1081_CANT_READ_PRODS_LST, 0);
		/* create basecats only. */
		STcopy (ut_args, buf);
		STpolycat (2, ut_args, ERx(", silent = %S"), buf);
		cnt_silent = 1;
		p_vararg1 = (PTR) Empty_str;
	    }
	}
	else if (STcasecmp(upgr_cb.du_feclients, ERx("nofeclients") ) == 0)
	{
		/* "nofeclients" passed to CREATEDB. Don't create Front-end
		** catalogs; skip call to upgradefe.
		*/
		dofe_flag = FALSE;
	}
	else
	{
	    /* List of client names was an input parameter.
	    ** Pass this same list of client names on to upgradefe.
	    ** Do NOT pass the "-s" flag.
	    */
	    STpolycat (2, ut_args, ERx(", client = %S"), buf);
	    cnt_client = 1;
	    p_vararg1 = (PTR) &upgr_cb.du_feclients[0];
	}
	if (dofe_flag)
	    du_talk (DU_MODESENS, &upgr_cb, I_DU00B4_FE_UPGRADE, 0);
        if (level != CURRENT_LEVEL &&
	    upgr_cb.Dbname_ptr && upgr_cb.Dbname_ptr->star_flag && dofe_flag)
	{
	    STcopy(upgr_cb.Dbname_ptr->cdbname,cdb_name);
	    STcopy(upgr_cb.Dbname_ptr->ddbname,ddb_name);
	    i = STtrmwhite(ddb_name);
	    STcopy("/star",&ddb_name[i]);
	    STcopy(upgr_cb.Dbname_ptr->dba,dba_name);
	    STcopy(upgr_cb.Dbname_ptr->ddbname,dbname);
	    /* disconnect from local server and connect to star server.
	    ** if we cannot connect to star server, then print a warning that
	    ** user will need to run upgradefe manually.  Otherwise, run
	    ** upgradefe from distriburted server.  In either case, be sure
	    ** to reconnect to local server before leaving...
	    */
	    if (!upgr_cb.Connected)
	    {	    
		du_talk(DU_ALWAYS, &upgr_cb, E_DU2106_UNOPENED_DB, 2,
			0, upgr_cb.Dbname_ptr->dbname);
		return;
	    }
	    if (upg_dbg_out)
	    {
		SIprintf("]==> (m) setting session to user database\n");
	    }
/* # line 6929 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DB_SESSIONID),(void *)IILQint(
    DB_SESSIONID));
  }
/* # line 6930 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 6931 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 6932 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DBDB_SESSIONID),(void *)IILQint(
    DBDB_SESSIONID));
  }
/* # line 6933 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 6934 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 6935 "upgradedb.sc" */	/* host code */
	    upgr_cb.Connected = FALSE;
	    /* try and connect to the star server. If we cannot connect to it,
	    ** supress SERVER error messages.  Instead print a warning that
	    ** the user will need to run upgradefe manually.
	    */
	    upgr_cb.Ignore_errors = TRUE;
/* # line 6941 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DB_SESSIONID);
    IIsqUser(dba_name);
    IIsqConnect(0,ddb_name,(char *)"-A6",(char *)"+U",(char *)"+w",(char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 6943 "upgradedb.sc" */	/* host code */
	    upgr_cb.Ignore_errors = FALSE;
	    if (sqlca.sqlcode < 0)
	    {
		dofe_flag = FALSE;
		du_talk(DU_MODESENS, &upgr_cb, W_DU1864_FE_CAT_FAIL, 2,
			0, ddb_name);
		/* reconnect to the cdb .. */
/* # line 6950 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DB_SESSIONID);
    IIsqUser(dba_name);
    IIsqConnect(0,cdb_name,(char *)"-A6",(char *)"+U",(char *)"+w",(char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 6952 "upgradedb.sc" */	/* host code */
		if (sqlca.sqlcode == 0)
		    upgr_cb.Connected = TRUE;
                /* reconnect to the iidbdb */
		if (vnode[0] != '\0')
		   STprintf(iidbdbname, "%s::iidbdb", vnode);
		else
		   STcopy("iidbdb", iidbdbname);
		if (upg_dbg_out)
		{
		    SIprintf("]==> [7] connect to iidbdb session %d id %s options = %s\n",
			    DBDB_SESSIONID, DU_DBA_DBDB,
			    upg_concurrent ? "+w" : "-A6 +w");
		}
		/* Reconnect the iidbdb session.  iidbdb is converted, do
		** a normal reconnect.
		*/
/* # line 6968 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DBDB_SESSIONID);
    IIsqConnect(0,iidbdbname,(char *)"+w",(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 6970 "upgradedb.sc" */	/* host code */
		IIlq_Protect(TRUE);
	    }
	    else
		upgr_cb.Connected = TRUE;
	}
	if  (( dofe_flag && (upgr_cb.Connected || upgr_cb.dbdb_cvt) ) ||
	      level == CURRENT_LEVEL)
	{
	    upgr_cb.Ignore_errors = TRUE;	/* Bypass local error recovery*/
	    if (upgr_cb.upgtrace[UPG01_SKIP_UPGRADEFE])
	    {
		du_talk(DU_MODESENS, &upgr_cb, I_DU00B3_TRACE_01, 0);
		stat = 0;
	    }
	    else
	        stat = UTexe(UT_WAIT|UT_STRICT, NULL, NULL,
		    NULL, ERx("upgradefe"), &err_code, buf,
		    (4 + cnt_silent + cnt_client),
		    dbname, duu_xflag(dbname), dba,
		    Empty_str,          /* basecats (fixed arg) */
		    p_vararg1,          /* silent or client */
		    p_vararg2,          /* client */
		    NULL);
	    upgr_cb.Ignore_errors = FALSE;	/*Resume local error recovery */
	    switch(stat)
	    {
	    case OK:
		    break;
	    case UTENOPROG:
		    du_talk(DU_MODESENS, &upgr_cb, W_DU1865_MISSING_UTEXE_DEF,
			    0);
		    break;
	    case UTENOARG:
		    du_talk(DU_MODESENS, &upgr_cb, W_DU1866_BAD_UTEXE_DEF, 0);
		    break;
	    default:
		    du_talk(DU_MODESENS, &upgr_cb, W_DU1867_UGRADEFE_FAIL,
			    2, sizeof(stat), &stat);
		    break;
	    }
	}  /* endif we are upgr_cb.Connected to LDB or DDB */
        if (level != CURRENT_LEVEL &&
	    upgr_cb.Dbname_ptr && upgr_cb.Dbname_ptr->star_flag && dofe_flag)
	{
	    /* disconnect from DDB and reconnect to LDB */
	    if (upg_dbg_out)
	    {
		SIprintf("]==> (n) setting session to user database\n");
	    }
/* # line 7019 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DB_SESSIONID),(void *)IILQint(
    DB_SESSIONID));
  }
/* # line 7020 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7021 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 7022 "upgradedb.sc" */	/* host code */
	    upgr_cb.Connected = FALSE;
/* # line 7023 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DB_SESSIONID);
    IIsqUser(dba_name);
    IIsqConnect(0,cdb_name,(char *)"-A6",(char *)"+U",(char *)"+w",(char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 7025 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode == 0)
		upgr_cb.Connected = TRUE;
            /* reconnect to the iidbdb */
            if (vnode[0] != '\0')
	       STprintf(iidbdbname, "%s::iidbdb", vnode);
	    else
	       STcopy("iidbdb", iidbdbname);
	    if (upg_dbg_out)
	    {
		SIprintf("]==> [8] connect to iidbdb session %d id %s options = %s\n",
			DBDB_SESSIONID, DU_DBA_DBDB,
			upg_concurrent ? "+w" : "-A6 +w");
	    }
/* # line 7038 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DBDB_SESSIONID);
    IIsqConnect(0,iidbdbname,(char *)"+w",(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 7040 "upgradedb.sc" */	/* host code */
	    IIlq_Protect(TRUE);		/* for iidbdb */
	}  /* endif we are doing a distributed DB */
	/* if we did the UTexe call for UPGRADEFE, then system catalog update
	** will be turned off on return from UPGRADEFE, so turn it back on */
	if (upgr_cb.Connected || upgr_cb.dbdb_cvt)
	    IIlq_Protect(TRUE);
}
/*{
** Name: changed_for_65_syscats	-  alter system catalogs changing for 6.5
**
** Description:
**
**	This routine builds the catalogs changed for 6.5 -- columns
**	added, columns got wider for DB_MAXNAME project, etc.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	January 1993 (jpk)
**	    wrote
**	30-Aug-2004 (schka24)
**	    Remove iiprotect, done earlier in driver.
*/
static VOID
changed_for_65_syscats(dbname)
char	*dbname;
{
    /* XXX something crazed prevents this from working here */
    /* XXX and yet somehow it works fine if done earlier (see below) */;
/*    cv_65_iistatistics(dbname);*/
    if (upg_dbg_out)
    {
	SIprintf("]==> changed_for_65_syscats(%s)\n\n", dbname);
    }
    cv_iirule(dbname);
    /* iiprotect already done in main cv driver */
}
/*{
** Name: changed_for_65_syscats_iidbdb	-  alter iidbdb syscats changing in 6.5
**
** Description:
**
**	This routine rebuilds the catalogs changed for 6.5 -- columns
**	added, columns got wider for DB_MAXNAME project, etc.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	January 1993 (jpk)
**	    wrote
**	15-jul-94 (robf)
**          Pulled out conversions which are also done in 65 ES conversion.
*/
static VOID
changed_for_65_syscats_iidbdb(dbname)
char	*dbname;
{
    if (upg_dbg_out)
    {
	SIprintf("]==> changed_for_65_syscats_iidbdb(%s)\n\n", dbname);
    }
    du_talk(DU_MODESENS, &upgr_cb, I_DU00E7_CONVERTING_TBL, 2, 0, "iiextend");
    cv_65_iiextend(dbname);
    du_talk(DU_MODESENS, &upgr_cb, I_DU00E7_CONVERTING_TBL, 2, 0,
	"iiusergroup");
    cv_65_iiusergroup(dbname);
}
/*{
** Name: cv_65_iidd_stats - alter iidd_stats for 6.5
**
** Description:
**
**
** Inputs:
**      database name
**
** Outputs:
**      none
**
** History:
**      November 1995 (prida01)
**          wrote
*/
static VOID
cv_65_iidd_stats(dbname)
char    *dbname;
{
    /*if iidd_stats in the distributed database lcks 6.5 columns add them */
    if ( tbl_lacks_col("iidd_stats", "column_domain") &&
         tbl_lacks_col("iidd_stats", "is_complete") &&
         tbl_lacks_col("iidd_stats", "stat_version") &&
         tbl_lacks_col("iidd_stats", "hist_data_length") )
    {
	bool   existed = rel_exists("iidd_stats");
	upgrade_iidd_stats = TRUE;
	if (existed)
	{
            make_copy_of(dbname, "iidd_stats");
/* # line 7146 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iidd_stats");
    IIsyncup((char *)0,0);
  }
/* # line 7147 "upgradedb.sc" */	/* host code */
            if (sqlca.sqlcode < 0)
            {
                du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
                    0, "iidd_stats", 0, dbname);
                upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
            }
        }
	/* create table iidd_stats */
        duc_iidd_stats();
        if (sqlca.sqlcode < 0)
        {
            du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
                0, "iidd_stats", 0, dbname);
            upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
        }
        if (existed)
        {
                /* xxx may get sql error here because pad is truncated */
/* # line 7165 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidd_stats(table_name, table_owner, column_name, create_d\
ate, num_unique, rept_factor, has_unique, pct_nulls, num_cells)select \
table_name, table_owner, column_name, create_date, num_unique, rept_fa\
ctor, has_unique, pct_nulls, num_cells from ");
    IIwritio(0,(short *)0,1,32,0,(char *)"iitmp_iidd_stats");
    IIsyncup((char *)0,0);
  }
/* # line 7172 "upgradedb.sc" */	/* host code */
                /* newcols? column_domain,is_complete,stat_version,hist_data_length */
                if (sqlca.sqlcode < 0)
                {
                   du_talk(DU_FAIL_EXIT, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
                       0, "iidd_stats", 0, dbname);
                   upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
                }
        }
        duc_modify_iidd_stats();   /* modify back to hash */
        if (sqlca.sqlcode < 0)
        {
            /* warning: modify didn't work */
            du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
                0, ERx("iidd_stats"), 0, dbname);
        }
/* # line 7187 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7188 "upgradedb.sc" */	/* host code */
    }
}
/*{
** Name: cv_65_iistats - alter iistats for 6.5
**
** Description:
**
**
** Inputs:
**      database name
**
** Outputs:
**      none
**
** History:
**      November 1995 (prida01)
**          wrote
*/
static VOID
cv_65_iistats(dbname)
char *dbname;
{
    /* If we upgrade iidd_stats in the cdb then we have to do the same  */
    /* for this one. This is registered from iidd_stats   */
    if (upgrade_iidd_stats)
    {
/* # line 7214 "upgradedb.sc" */	/* remove table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"remove table iistats");
    IIsyncup((char *)0,0);
  }
/* # line 7215 "upgradedb.sc" */	/* register table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register table iistats as link from iidd_stats");
    IIsyncup((char *)0,0);
  }
/* # line 7216 "upgradedb.sc" */	/* host code */
    	if (sqlca.sqlcode < 0)
    	{
		du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
		0, "iistats", 0, dbname);
        	upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
    	}
/* # line 7222 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7223 "upgradedb.sc" */	/* host code */
    }	
}
/*{
** Name: cv_65_iistatistics - alter iistatistics for 6.5
**
** Description:
**
**	Add columns sversion, shistlength to iistatistics for 6.5
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	January 1993 (jpk)
**	    wrote
**	January 07, 1998 (kosma01)
**	    AIX 4.x loses $ingres authorization. Reobtain it here.
*/
static VOID
cv_65_iistatistics(dbname)
char	*dbname;
{
    /* if iistatistics lacks both new 6.5 columns, add them */
    if ( tbl_lacks_col("iistatistics", "sversion") &&
	 tbl_lacks_col("iistatistics", "shistlength") )
    {
	bool	existed = rel_exists("iistatistics");
	du_talk(DU_MODESENS, &upgr_cb, I_DU00E1_IISTATISTICS, 0);
	if (existed)
	{
/* # line 7256 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7257 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed((char *)"set session authorization $ingres");
    IIsyncup((char *)0,0);
  }
/* # line 7258 "upgradedb.sc" */	/* host code */
	    make_copy_of(dbname, "iistatistics");
/* # line 7259 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iistatistics");
    IIsyncup((char *)0,0);
  }
/* # line 7260 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
		    0, "iistatistics", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_iistatistics();		/* create new iistatistics */
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
		0, "iistatistics", 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
	if (existed)
	{
	    /* xxx may get sql error here because pad is truncated */
/* # line 7277 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iistatistics(stabbase, stabindex, snunique, sreptfact, sn\
ull, satno, snumcells, sdomain, scomplete, sunique, sdate)select stabb\
ase, stabindex, snunique, sreptfact, snull, satno, snumcells, sdomain,\
 scomplete, sunique, sdate from ");
    IIwritio(0,(short *)0,1,32,0,(char *)"iitmp_iistatistics");
    IIsyncup((char *)0,0);
  }
/* # line 7284 "upgradedb.sc" */	/* host code */
	    /* xxx vals for newcols? sversion, shistlength */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_FAIL_EXIT, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
		    0, "iistatistics", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_modify_iistatistics();	/* modify back to hash */
	if (sqlca.sqlcode < 0)
	{
	    /* warning: modify didn't work */
	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
		0, ERx("iistatistics"), 0, dbname);
	}
/* # line 7299 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7300 "upgradedb.sc" */	/* host code */
    }
}
/*{
** Name: cv_iiintegrity - alter iiintegrity
**
** Description:
**
**	Add columns consname, consid1, consid2, consschema_id1,
**	consschema_id2, consflags to iiintegrity for 6.5
**
**	Add more columns intdomset11 thru 33 for r3
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	January 1993 (jpk)
**	    wrote
**	February 1993 (jpk)
**	    ensure new iiintegrity is created with QUEL; else DBMS will try to
**	    insert check constraints corresponding to NOT NULLS specified
**	    in iiintegrity columns into iintegrity itself.  Classic bootstrap
**	    problem.
**	30-Aug-2004 (schka24)
**	    r3 changes.
**	
*/
static VOID
cv_iiintegrity(dbname)
char	*dbname;
{
    bool pre_r3;
    /* Perform pre-64 conversions if needed */
    cv_pre64_integrity();
    /* if iiintegrity lacks new r3 columns, add them */
    pre_r3 = tbl_lacks_col("iiintegrity", "intdomset33");
    if (pre_r3)
    {
	bool	existed = rel_exists("iiintegrity");
	du_talk(DU_MODESENS, &upgr_cb, I_DU00DD_IIINTEGRITY, 0);
	if (existed)
	{
    	    make_copy_of(dbname, "iiintegrity");
/* # line 7346 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iiintegrity");
    IIsyncup((char *)0,0);
  }
/* # line 7347 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
		    0, "iiintegrity", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_iiintegrity();		/* create new iiintegrity */
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_FAIL_EXIT, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
		0, "iiintegrity", 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
	/* repopulate only if there was an original */
	if (existed)
	{	
	    if (copy_existing_columns(dbname,"iiintegrity") != E_DU_OK)
	    {
		if (upg_dbg_out)
		{
		    SIprintf("]==> problem repopulating iiintegrity\n");
		}
		du_talk(DU_FAIL_EXIT, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
		    0, "iiintegrity", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	    if (pre_r3)
	    {
		/* If we added more domset columns... */
/* # line 7377 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiintegrity set intdomset11= -1, intdomset12= -1, intdomset13=\
 -1, intdomset14= -1, intdomset15= -1, intdomset16= -1, intdomset17= -\
1, intdomset18= -1, intdomset19= -1, intdomset20= -1, intdomset21= -1,\
 intdomset22= -1, intdomset23= -1, intdomset24");
    IIwritio(0,(short *)0,1,32,0,(char *)
"= -1, intdomset25= -1, intdomset26= -1, intdomset27= -1, intdomset28=\
 -1, intdomset29= -1, intdomset30= -1, intdomset31= -1, intdomset32= -\
1, intdomset33= -1 where intdomset1= -1 and intdomset2= -1 and intdoms\
et3= -1 and intdomset4= -1 and intdomset5= -1 ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"and intdomset6= -1 and intdomset7= -1 and intdomset8= -1 and intdomse\
t9= -1 and intdomseta= -1");
    IIsyncup((char *)0,0);
  }
/* # line 7390 "upgradedb.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
		du_talk(DU_FAIL_EXIT, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
		    0, "iiintegrity", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
		}
	    }
	}
	duc_modify_iiintegrity();	/* modify back to hash */
	if (sqlca.sqlcode < 0)
	{
	    /* warning: modify didn't work */
	    if (upg_dbg_out)
	    {
		SIprintf("]==> problem modifying iiintegrity\n");
	    }
	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
		0, ERx("iiintegrity"), 0, dbname);
	}
/* # line 7409 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7410 "upgradedb.sc" */	/* host code */
    }
}
/*{
** Name: cv_65_iidevices - alter iidevices for 6.5
**
** Description:
**
**	DB_MAXNAME conversion for core catalog iidevices.
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	January 1993 (jpk)
**	    wrote
**	March 1993 (jpk)
**	    Separated into its own function.
*/
static VOID
cv_65_iidevices(dbname)
char	*dbname;
{
    if (needs_widening("iidevices", "devloc"))
    {   
	bool	existed = rel_exists("iidevices");
	du_talk(DU_MODESENS, &upgr_cb, I_DU00DF_IIDEVICES, 0);
	if (existed)
	{
	    make_copy_of(dbname, "iidevices");
/* # line 7442 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table \"$ingres\".iidev\
ices");
    IIsyncup((char *)0,0);
  }
/* # line 7443 "upgradedb.sc" */	/* host code */
	    if (upg_dbg_out)
	    {
		SIprintf("]==> sqlcode %d dropping iidevices\n", sqlca.sqlcode);
	    }
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
		    0, "iidevices", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
/* # line 7454 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7455 "upgradedb.sc" */	/* host code */
	duc_iidevices();		/* create new iidevices */
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_FAIL_EXIT, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
		0, "iidevices", 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
	if (existed)
	{
	    /* straightforward to repopulate, names are the same */
/* # line 7465 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidevices select * from iitmp_iidevices");
    IIsyncup((char *)0,0);
  }
/* # line 7466 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_FAIL_EXIT, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
		    0, "iidevices", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_modify_iidevices();		/* modify iidevices back to hash */
	if (sqlca.sqlcode < 0)
	{
	    /* warning: modify didn't work */
	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
		0, ERx("iidevices"), 0, dbname);
	}
/* # line 7480 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7481 "upgradedb.sc" */	/* host code */
    }
}
/*{
** Name: cv_iiprocedure - alter iiprocedure to current level
**
** Description:
**
**	Add columns, widen name/owner columns for 6.5.
**
**	For release 3, eliminate two security id columns and combine
**	them into dbp_free.  (In theory this could be done by hacking
**	around with iiattribute, but we'll do it the "right" way.)
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	January 1993 (jpk)
**	    wrote
**	March 1993 (jpk)
**	    Separated into its own function.
**	12-jul-94 (robf)
**          65ES adds new columns, allow for this in processing.
**	07-jan-1998 (nanpr01)
**	    After FIPS upgrade with refenrential integrity, insert/update
**	    fails. bug 87948
**	30-Aug-2004 (schka24)
**	    r3 upgrade additions.
**	3-Dec-2004 (schka24)
**	    Stupid thinko prevented secid columns from being removed.
*/
static VOID
cv_iiprocedure(char *dbname)
{
    bool existed;
    bool got_65es_cols=FALSE;
    bool got_65_cols=FALSE;
    bool got_r3_cols = FALSE;
    SYSTIME Current_date;
  i4 sql_current_date;
    TMnow (&Current_date);
    sql_current_date = Current_date.TM_secs;
    /*
    ** Check whether we have 65, 65ES, r3, or old column definitions
    */
    got_r3_cols = ! tbl_lacks_col("iiprocedure", "dbp_free");
    got_65es_cols = ! tbl_lacks_col("iiprocedure", "dbp_secid");
    got_65_cols = ! tbl_lacks_col("iiprocedure", "dbp_create_date");
    /* Careful with the tests here, dbp_secid disappears in r3 */
    /* If we don't have the r3 dbp_free, redo the table */
    if (got_r3_cols)
	return;
    existed = rel_exists("iiprocedure");
    du_talk(DU_MODESENS, &upgr_cb, I_DU00E0_IIPROCEDURE, 0);
    if (existed)
    {
	make_copy_of(dbname, "iiprocedure");
/* # line 7543 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iiprocedure");
    IIsyncup((char *)0,0);
  }
/* # line 7544 "upgradedb.sc" */	/* host code */
	check_sql(sqlca.sqlcode);
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
		0, "iiprocedure", 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
    }
    duc_iiprocedure();		/* create new iiprocedure */
    if (sqlca.sqlcode < 0)
    {
	du_talk(DU_FAIL_EXIT, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
	    0, "iiprocedure", 0, dbname);
	upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
    }
    if (existed)
    {
	/* Manual recopy if don't have any of the 6.5 stuff,
	** otherwise just use copy-existing-columns.
	*/
	if( (!(got_65_cols || got_65es_cols))
	  || needs_widening("iiprocedure","dbp_name")
	  || needs_widening("iiprocedure","dbp_owner") )
	{
/* # line 7568 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiprocedure(dbp_name, dbp_owner, dbp_txtlen, dbp_txtid1, \
dbp_txtid2, dbp_mask1, dbp_mask2, dbp_id, dbp_idx, dbp_create_date)sel\
ect dbp_name, dbp_owner, dbp_txtlen, dbp_txtid1, dbp_txtid2, dbp_mask1\
, dbp_mask2, dbp_id, dbp_idx, ");
    IIputdomio((short *)0,1,30,4,&sql_current_date);
    IIwritio(0,(short *)0,1,32,0,(char *)"from iitmp_iiprocedure");
    IIsyncup((char *)0,0);
  }
/* # line 7577 "upgradedb.sc" */	/* host code */
	    /* xxx vals for newcols? dbp_parameter_count, dbp_record_width */
	    check_sql(sqlca.sqlcode);
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_FAIL_EXIT, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
		    0, "iiprocedure", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	else
	{
	    if (copy_existing_columns(dbname, "iiprocedure") != E_DU_OK)
	    {
		du_talk(DU_FAIL_EXIT, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
		    0, "iiprocedure", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
    }
    duc_modify_iiprocedure(); /* modify back to btree, create index */
    if (sqlca.sqlcode < 0)
    {
	/* warning: modify didn't work */
	du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
	    0, ERx("iiprocedure"), 0, dbname);
    }
/* # line 7603 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7604 "upgradedb.sc" */	/* host code */
}
/*{
** Name: cv_65_iiextend - alter iiextend for 6.5
**
** Description:
**
**	DB_MAXNAME conversion for iiextend
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	January 1993 (jpk)
**	    wrote
**	March 1994 (jpk)
**	    MLSORTS: set DU_EXT_DATA bit in status column;
**	    add work locations to iiextend.
**	05-Jan-96 (nive)
**		update iiextend to pad lname and dname upto 32 length 
**		while upgrading dbs from R6. Also, pad lname ii_work upto
**		32 length during its insertion into iiextend. (Bug 71480)
*/
static VOID
cv_65_iiextend(dbname)
char	*dbname;
{
    /* convert only if iiextend columns need widening */
    if (needs_widening("iiextend", "lname") &&
	needs_widening("iiextend", "dname"))
    {
	bool	existed = rel_exists("iiextend");
	if (existed)
	{
	    make_copy_of(dbname, "iiextend");
/* # line 7641 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iiextend");
    IIsyncup((char *)0,0);
  }
/* # line 7642 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
		    0, "iiextend", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
/* # line 7649 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7650 "upgradedb.sc" */	/* host code */
	duc_iiextend();		/* create new iiextend */
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
		0, "iiextend", 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
/* # line 7657 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7658 "upgradedb.sc" */	/* host code */
	if (existed)
	{
	    /* easy, just DB_MAXNAME conversion */
/* # line 7661 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend(lname, dname, status, rawstart, rawblocks)select\
 lname, dname, status, 0, 0 from iitmp_iiextend");
    IIsyncup((char *)0,0);
  }
/* # line 7664 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
		    0, "iiextend", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
/*
Bug 71480 - need to pad lname and dname upto 32 length in iiextend for 
dbs upgraded from R6
*/
/* # line 7674 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiextend set lname=pad(lname), dname=pad(dname)");
    IIsyncup((char *)0,0);
  }
/* # line 7675 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
		    0, "iiextend", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_modify_iiextend();	/* modify back to hash and add index */
	if (sqlca.sqlcode < 0)
	{
	    /* warning: modify didn't work */
	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
		0, ERx("iiextend"), 0, dbname);
	}
/* # line 7689 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7690 "upgradedb.sc" */	/* host code */
    }
    /*
    ** well, OK, this is harmless to do every time:
    ** set DU_EXT_DATA bit (value is 2) in status column in iiextend.
    ** Exception: work locations get 4 set, not 2.  So for idempotency,
    ** qualification excludes them, and those with DU_EXT_DATA already set.
    */
/* # line 7697 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiextend set status=status +2 where mod((status/2), 2)=0 and l\
name!='ii_work'");
    IIsyncup((char *)0,0);
  }
/* # line 7699 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7700 "upgradedb.sc" */	/* host code */
    /*
    ** this too is harmless to do every time, and easier to do here
    ** than for every database being converted, and harmless to do
    ** for every database even if some are for some unknown reason
    ** not being converted: add work locations to iiextend.
    ** How to do it: add a work location for every database that lacks one.
    ** This is idempotent even if you upgrade the iidbdb several times.
    ** The magic value 5 comes from the DU_OPERATIVE bit (value is 1)
    ** ORed with the extension time (value is 4).
    */
/*
Bug 71480 - need to pad lname and dname upto 32 length in iiextend 
*/
/* # line 7713 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend select 'ii_work                         ', dname\
, 5, 0, 0 from iiextend where not exists(select dname from iiextend wh\
ere trim(lname)='ii_work')");
    IIsyncup((char *)0,0);
  }
/* # line 7717 "upgradedb.sc" */	/* host code */
}
/*{
** Name: cv_65_iidbdepends - alter iidbdepends for 6.5
**
** Description:
**
**	DB_MAXNAME conversion for iidbdepends
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	January 1993 (jpk)
**	    wrote
*/
static VOID
cv_65_iidbdepends(dbname)
char	*dbname;
{
    /* convert only if iidbdepends needs it */
    if (tbl_lacks_col("iidbdepends", "i_qid"))
    {
	bool	existed = rel_exists("iidbdepends");
	du_talk(DU_MODESENS, &upgr_cb, I_DU00DE_IIDBDEPENDS, 0);
	if (existed)
	{
	    make_copy_of(dbname, "iidbdepends");
/* # line 7747 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iidbdepends");
    IIsyncup((char *)0,0);
  }
/* # line 7748 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
		    0, "iidbdepends", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
/* # line 7755 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7756 "upgradedb.sc" */	/* host code */
	duc_iidbdepends();		/* create new iidbdepends */
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
		0, "iidbdepends", 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
/* # line 7763 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7764 "upgradedb.sc" */	/* host code */
	if (existed)
	{
	    /* easy, just one new column and new left to right ordering */
/* # line 7767 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbdepends(inid1, inid2, itype, deid1, deid2, dtype, qid\
, i_qid)select inid1, inid2, itype, deid1, deid2, dtype, qid, 0 from i\
itmp_iidbdepends");
    IIsyncup((char *)0,0);
  }
/* # line 7772 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
		    0, "iidbdepends", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_modify_iidbdepends();	/* modify back to hash and add index */
	if (sqlca.sqlcode < 0)
	{
	    /* warning: modify didn't work */
	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
		0, ERx("iidbdepends"), 0, dbname);
	}
/* # line 7786 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7787 "upgradedb.sc" */	/* host code */
    }
}
/*{
** Name: cv_65_iiusergroup - alter iiusergroup for 6.5
**
** Description:
**
**	DB_MAXNAME conversion for iiusergroup
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	January 1993 (jpk)
**	    wrote
*/
static VOID
cv_65_iiusergroup(dbname)
char	*dbname;
{
    /* convert only if iiusergroup column needs widening */
    if (needs_widening("iiusergroup", "groupid") &&
	needs_widening("iiusergroup", "groupmem"))
    {
	bool	existed = rel_exists("iiusergroup");
	if (existed)
	{
	    make_copy_of(dbname, "iiusergroup");
/* # line 7818 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iiusergroup");
    IIsyncup((char *)0,0);
  }
/* # line 7819 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
		    0, "iiusergroup", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
/* # line 7826 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7827 "upgradedb.sc" */	/* host code */
	duc_iiusergroup();		/* create new iiusergroup */
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
		0, "iiusergroup", 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
/* # line 7834 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7835 "upgradedb.sc" */	/* host code */
	/* easy, just DB_MAXNAME conversion */
	if (existed)
	{
/* # line 7838 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiusergroup select * from iitmp_iiusergroup");
    IIsyncup((char *)0,0);
  }
/* # line 7840 "upgradedb.sc" */	/* host code */
	}
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
		0, "iiusergroup", 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
	duc_modify_iiusergroup();	/* modify back to hash and add index */
	if (sqlca.sqlcode < 0)
	{
	    /* warning: modify didn't work */
	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
		0, ERx("iiusergroup"), 0, dbname);
	}
/* # line 7854 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 7855 "upgradedb.sc" */	/* host code */
    }
}
/*{
** Name: new_for_65_syscats -  build system catalogs new for 6.5
**
** Description:
**
**	This routine builds the system catalogs new for 6.5.  If, in some future
**	release one of these catalogs changes, then it should be pulled out
**	of this routine and an individual routine written that determines
**	whether to create the catalog in the new format or to modify the
**	catalog that was created for 6.5.  It creates the following system
**	catalogs:
**
**	    iidbms_comment
**	    iidbms_subcomment
**	    iisectype
**	    iischema
**	    iisynonym
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	15-oct-91 (teresa)
**	    created for 6.5.
**	08-oct-92 (robf)
**	    Added C2-SXA gateway tables.
**	13-dec-92 (jpk)
**	    Added iiaccess standard catalog
**	13-dec-92 (jpk)
**	    Added iischema system catalog
**	1-mar-1993 (jpk)
**	    renamed and separated out the standard catalogs.
**	16-nov-93 (rblumer)
**	    added value for new 'upper_case' argument in call to duc_iischema.
**	    This argument is FALSE for 6.4 databases, since only lower-case
**	    databases were allowed in 6.4.
**	15-dec-1993 (jpk) move building of iiprocedure_parameter earlier.
*/
static VOID
new_for_65_syscats(dbname)
char	*dbname;
{
    /* DBMS CATALOGS:  iidbms_comment and iidbms_subcomment */
    if (!rel_exists("iidbms_comment"))
    {
	du_talk(DU_MODESENS, &upgr_cb, I_DU00B8_IIDBMS_COMMENT_CRE, 0);
	duc_iidbms_comment();   /* build iidbms_comment */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
	duc_m1_dbmscomment();   /* modify dbmscomment to desired structure */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
    }
    /* DBMS CATALOGS:  iisectype	*/
    if (!rel_exists("iisectype"))
    {
	du_talk(DU_MODESENS, &upgr_cb, I_DU00B9_IISECTYPE_CRE, 0);
	duc_iisectype();        /* build iisectype */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
	duc_m3_sectype();       /* modify iisectype to desired sturcture */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
    }
    /* DBMS CATALOGS:  iisynonym	*/
    if (!rel_exists("iisynonym"))
    {
	du_talk(DU_MODESENS, &upgr_cb, I_DU00BA_IISYNONYM_CRE, 0);
	duc_iisynonym();        /* build iisynonym */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
	duc_m2_synonym();       /* modify iisynonyn to desired sturcture */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
    }
    /* DBMS CATALOGS:  iischema	*/
    if (!rel_exists("iischema"))
    {
	du_talk(DU_MODESENS, &upgr_cb, I_DU00D0_IISCHEMA_CRE, 0);
	duc_iischema((i4)FALSE);        /* build iischema */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
	duc_m9_schema();       /* modify iischema to desired sturcture */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
    }
    /* DBMS CATALOGS:  iidefault	*/
    if (!rel_exists("iidefault"))
    {
	du_talk(DU_MODESENS, &upgr_cb, I_DU00D3_IIDEFAULT_CRE, 0);
	duc_iidefault();	/* builds and populates iidefault */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
	duc_modify_iidefault();	/* modify to desired structure */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
    }
    /* DBMS CATALOGS:  iikey	*/
    if (!rel_exists("iikey"))
    {
	du_talk(DU_MODESENS, &upgr_cb, I_DU00D4_IIKEY_CRE, 0);
	duc_iikey();	/* builds and populates iikey */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
	duc_modify_iikey();	/* modify to desired structure */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
    }
    /* hmmm, this gets done in bootstrap_syscats(), but harmless to check */
    /* DBMS CATALOGS:  iipriv	*/
    if (!rel_exists("iipriv"))
    {
	du_talk(DU_MODESENS, &upgr_cb, I_DU00D9_IIPRIV, 0);
	if (upg_dbg_out)
	{
	    SIprintf("]==> adding iipriv\n");
	}
	duc_iipriv();	/* builds and populates iipriv */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
	duc_m7_priv();	/* modify to desired structure */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
    }
}
/*{
** Name: cv_dbaccess - move data from iidbaccess into 6.5 catalogs
**
** Description:
**
**	The iidbaccess catalog has been obsoleted. Now information from
**	that catalog is placed into tuples in iidbpriv, converted as
**	follows:
**
**	      IIDBPRIV:			Value:
**		dbname			iidbaccess.dbname
**		grantee			iidbaccess.usrname
**		gtype			<default>
**		dbflags			<default>
**		control			DBPR_ACCESS (0x00000800)
**		flags			DBPR_ACCESS (0x00000800)
**		qrow_limit		<default>
**		qdio_limit		<default>
**		qcpu_limit		<default>
**		qpage_limit		<default>
**		qcost_limit		<default>
**		reserve			<default>
**
**	NOTE:  in iidbaccess the DBA is listed, but in iidbpriv, only non-dbas
**		are listed.  So, we need to eleminate the tuples from iidbaccess
**		that give the DBA access to the private db before we append
**		tuples to iidbpriv.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	16-oct-91 (teresa)
**	    created for 6.5.
*/
static VOID
cv_dbaccess()
{
  i4 flag;
  i4 cnt;
    if (!rel_exists("iidbaccess"))
    {
	/* there is not an iidbaccess catalog, so just return */
	du_talk(DU_MODESENS, &upgr_cb, I_DU00C8_PRIV_DB_CURRENT, 0);
	return;	    
    }
    /*
    ** if we get here then there is an iidbaccess catalog, so assure that
    ** all non-dba users who have access to private databases end up
    ** in the iidbpriv catalog.  Be careful not to make duplicate tuples
    ** if upgradedb is run multiple times on the iidbdb 
    */
    flag = DBPR_ACCESS;
    /* first get rid of any tuples that describe a dba's access to a
    ** private database */
/* # line 8045 "upgradedb.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iidbaccess_temp as select * from iidbaccess");
    IIsyncup((char *)0,0);
  }
/* # line 8046 "upgradedb.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbaccess_temp where dbname in(select name from iidataba\
se, iidbaccess_temp where name=dbname and own=usrname)and usrname in(s\
elect own from iidatabase, iidbaccess_temp where name=dbname and own=u\
srname)");
    IIsyncup((char *)0,0);
  }
/* # line 8052 "upgradedb.sc" */	/* host code */
    /* delete any tuples from the work table that are already in
    ** iidbpriv */
/* # line 8054 "upgradedb.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbaccess_temp where dbname in(select x.dbname from iidb\
access_temp x, iidbpriv y where x.dbname=y.dbname and x.usrname=y.gran\
tee and y.control=");
    IIputdomio((short *)0,1,30,4,&flag);
    IIwritio(0,(short *)0,1,32,0,(char *)
")and usrname in(select grantee from iidbaccess_temp x, iidbpriv y whe\
re x.dbname=y.dbname and x.usrname=y.grantee and y.control=");
    IIputdomio((short *)0,1,30,4,&flag);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 8060 "upgradedb.sc" */	/* host code */
    /* see if there are any tuples to append to iidbpriv */
    /*exec sql select count(*) into :cnt from iidbpriv_temp; */
/* # line 8062 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iidbaccess_temp");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 8063 "upgradedb.sc" */	/* host code */
    if (cnt)
    {
	du_talk(DU_MODESENS, &upgr_cb, I_DU00C7_UPDT_IIDBPRIV, 0);
	/* Now put the remaining tuples into iidbpriv */
/* # line 8067 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbpriv(dbname, grantee, control, flags)select dbname, u\
srname, ");
    IIputdomio((short *)0,1,30,4,&flag);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&flag);
    IIwritio(0,(short *)0,1,32,0,(char *)"from iidbaccess_temp");
    IIsyncup((char *)0,0);
  }
/* # line 8069 "upgradedb.sc" */	/* host code */
    }
    else
	du_talk(DU_MODESENS, &upgr_cb, I_DU00C8_PRIV_DB_CURRENT, 0);
    /* finally get rid of the work table */
/* # line 8073 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop iidbaccess_temp");
    IIsyncup((char *)0,0);
  }
/* # line 8074 "upgradedb.sc" */	/* host code */
}
/*{
** Name: order_critical_rehash - rehash core catalogs after connect
**
** Description:
**	The core catalog conversion (iirelation, iirel_idx, iiattribute,
**	and iiindex) is too delicate to be done by upgradedb, in general.
**	The server will take care of these catalogs upon upgradedb
**	connect.  In some cases, some catalogs are rewritten as single-
**	bucket hash tables (to make it simple for the server).  When
**	this happens, it's a real good idea to rehash the core catalog(s)
**	before we get serious about doing queries.
**
** Inputs:
**	dbname			The database name
**	level			Level we're converting from
**
** Outputs:
**	None
**
** History:
**	26-Aug-2004 (schka24)
**	    Written for r3 upgrade.
**	3-Jan-2005 (schka24)
**	    No-op if db was at r3.
*/
static VOID
order_critical_rehash(char *dbname, i4 level)
{
    if (level >= V30_LEVEL)
	return;
    if (upg_dbg_out)
	SIprintf("]==> order-critical-rehash for db %s\n",dbname);
    du_talk(DU_MODESENS, &upgr_cb, I_DU00DA_MODIFY_CORE, 0);
    if (level < V65_LEVEL)
    {
	if (upg_dbg_out) SIprintf("]==>   ... iirelation\n");
	du_talk(DU_MODESENS, &upgr_cb, I_DU0100_MODIFYING_SY, 2, 0, "iirelation");
/* # line 8112 "upgradedb.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iirelation to hash with allocation=100");
    IIsyncup((char *)0,0);
  }
/* # line 8113 "upgradedb.sc" */	/* host code */
	check_sql(sqlca.sqlcode);
	if(sqlca.sqlcode<0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
		0, ERx("iirelation"), 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
    }
    /* For r3, all prior versions rewrite iiattribute */
    if (level < V30_LEVEL)
    {
	if (upg_dbg_out) SIprintf("]==>   ... iiattribute\n");
	du_talk(DU_MODESENS, &upgr_cb, I_DU0100_MODIFYING_SY, 2, 0, "iiattribute");
/* # line 8126 "upgradedb.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify iiattribute to hash with allocation=100");
    IIsyncup((char *)0,0);
  }
/* # line 8127 "upgradedb.sc" */	/* host code */
	check_sql(sqlca.sqlcode);
	if(sqlca.sqlcode<0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
		0, ERx("iiattribute"), 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
    }
    /* 6.4 conversion doesn't rewrite iiindex but let's rehash it
    ** just to be safe (and because upgradedb always has done).
    */
    if (level < V65_LEVEL)
    {
	if (upg_dbg_out) SIprintf("]==>   ... iiindex\n");
	du_talk(DU_MODESENS, &upgr_cb, I_DU0100_MODIFYING_SY, 2, 0, "iiindex");
/* # line 8142 "upgradedb.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"modify iiindex to hash");
    IIsyncup((char *)0,0);
  }
/* # line 8143 "upgradedb.sc" */	/* host code */
	check_sql(sqlca.sqlcode);
	if(sqlca.sqlcode<0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
		0, ERx("iiindex"), 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
    }
/* # line 8151 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 8152 "upgradedb.sc" */	/* host code */
} /* order_critical_rehash */
/*{
** Name: order_critical_65	- perform order-critical 6.5 conversion
**
** Description:
**	Some parts of 6.5 conversion are order-critical, must be
**	performed immediately after connect to database.  This is
**	because the server has just converted core catalogs
**	(for increase iirelation tuple width and DB_MAXNAME projects)
**	and system catalogs (for file extend project).  So at this
**	point, before any other database access, upgradedb must
**	perform corresponding conversions:
**		modify iirelation and iiattribute back to hash
**		define and call iiQEF_yet_to_be_named to fix iiattribute values
**
** Inputs:
**	database name
**
** Outputs:
**	None
**
** History:
**	January 1993 (jpk)
**	     written
**	November 1993 (jpk)
**	     make sure to COMMIT before setting authorization.
**	     Stop making copy of iievent here; not order critical.
**	18-jul-94 (robf)
**           Check for core MODIFY statements failing. If these go wrong
**           possibly other things will fail so bail out here.
**	10-nov-95 (nick)
**	    Call duc_modify_iihistogram().
**	08-Jul-99 (carsu07)
**	     Removing the statistics flag in iirelation for a number of 
**	     system catalogs. (Bug 94301) 
**	26-Aug-2004 (schka24)
**	    Don't need to rehash here.  "set nostats" now done in core
**	    catalog rewrite in server, remove.
*/
static VOID
order_critical_65(char *dbname, DBNAME_NODE *db_star_name, bool upcase)
{
    if (upg_dbg_out)
    {
	SIprintf("]==> order critical from-64 for db %s: truncate stats, histo\n",
	    dbname);
    }
/* # line 8199 "upgradedb.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"modify iistatistics to trunc\
ated");
    IIsyncup((char *)0,0);
  }
/* # line 8200 "upgradedb.sc" */	/* host code */
    duc_modify_iistatistics();
/* # line 8201 "upgradedb.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"modify iihistogram to trunca\
ted");
    IIsyncup((char *)0,0);
  }
/* # line 8202 "upgradedb.sc" */	/* host code */
    duc_modify_iihistogram();
    /* Trees need to be converted */
    upgr_cb.Dbname_ptr->tree_convert = TRUE;
    /* fix the "boostrap" system catalogs */
    bootstrap_syscats(dbname, db_star_name, upcase);
    file_extend_convert_ingres();
    make_udt_defaults();
}
/*{
** Name: make_udt_defaults -- set UDT default ids for iiattribute.
**
** Description:
**	Server must and will perform almost all iiattribute id
**	conversion at connect time.  Upgradedb here converts
**	iiattribute ids for UDTs only.
**
** Inputs:
**	None
**
** Outputs:
**	None
**
** History:
**	July 1993 (jpk)
**	     written
*/
make_udt_defaults()
{
  i4 return_status;
    du_talk(DU_MODESENS, &upgr_cb, I_DU00E4_MAKE_UDT_DEFAULTS, 0);
    if (proc_exists("iiqef_make_udt_defaults"))
    {
/* # line 8236 "upgradedb.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"iiQEF_make_udt_defaults");
    IIwritio(0,(short *)0,1,32,0,(char *)
"drop procedure iiQEF_make_udt_defaults");
    IIsyncup((char *)0,0);
  }
/* # line 8237 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 8238 "upgradedb.sc" */	/* host code */
    }
/* # line 8239 "upgradedb.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"iiQEF_make_udt_defaults");
    IIwritio(0,(short *)0,1,32,0,(char *)
"create procedure iiQEF_make_udt_defaults as");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"    begin");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"        execute internal;");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"    end");
    IIputctrl(0);
    IIsyncup((char *)0,0);
  }
/* # line 8243 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 8244 "upgradedb.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"iiQEF_make_udt_defaults");
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
    IIeqiqio((short *)0,1,30,4,&return_status,(char *)"iiret");
  }
/* # line 8246 "upgradedb.sc" */	/* host code */
    if (return_status != 0)
    {
	if (upg_dbg_out)
	{
	    SIprintf("]==> error executing iiQEF_make_udt_defaults\n\n");
	}
	upgr_exit(DU_FAIL_EXIT, 0);
    }
}
/*{
 ** Name: make_copy_of - save copy of named tables
 **
 ** Description:
 **	used by make_copies_of_65_tables to modularize making
 **	a copy of a table into a temporary table.  Since our
 **	embedded SQL doesn't permit variable substitution for
 **	table names, this routine is coded in QUEL.  Note that
 **	it hardwires in the temp tablename convention:
 **	table iifoo is copied into temporary table iitmp_iifoo.
 **
 ** Inputs:
 **	database name
 **     table name
 **
 ** Outputs:
 **	none
 **
 ** History:
 **	January 1993 (jpk)
 **	     written
 */
static VOID
make_copy_of(dbname, tblname)
char	*dbname;
  char *tblname;
{
  char tmpname[DB_MAXNAME + 1];
  int err_no;
    STcopy("iitmp_", tmpname);
    STcat(tmpname, tblname);
    /* If old copy of temporary table is hanging around, drop it.
    ** Better to be sure that the temporary table we are about to
    ** repopulate from is known to be a correct and current copy.
    ** (Possible exception: what if syscat is missing but temp table
    ** is present and has rows, should we really blow away what looks
    ** to be the existing copy from a failed upgrade?  Don't know.)
    */
    if (rel_exists(tmpname))
    {
/* # line 8624 "upgradedb.qsc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
    IIwritio(1,(short *)0,1,32,0,tmpname);
    IIsyncup((char *)0,0);
  }
/* # line 8625 "upgradedb.qsc" */	/* inquire_equel */
  {
    IIeqiqio((short *)0,1,30,4,&err_no,(char *)"errorno");
  }
/* # line 8626 "upgradedb.qsc" */	/* host code */
	if (err_no != 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU241B_NODROP_TEMP, 4,
		0, tblname, 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
    }
    /* if a system catalog is missing in 6.4, upgradedb can still
    ** create the 6.5 catalog, but of course can't repopulate it;
    ** it will be the right 6.5 shape but it will be empty.  Here,
    ** just don't try to copy from a non-existent table.
    */
    if (!rel_exists(tblname))
    {
	du_talk(DU_MODESENS, &upgr_cb, W_DU186A_MISSING_SYSCAT, 4,
	    0, tblname, 0, dbname);
	return;
    }
    /* sys cat is there and tmp table is not, so now safe to make copy */
    if (upg_dbg_out)
    {
	SIprintf("]==> copying %s into %s\n", tblname, tmpname);
    }
/* # line 8652 "upgradedb.qsc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"retrieve ");
    IIwritio(1,(short *)0,1,32,0,tmpname);
    IIwritio(0,(short *)0,1,32,0,(char *)"(");
    IIwritio(1,(short *)0,1,32,0,tblname);
    IIwritio(0,(short *)0,1,32,0,(char *)".all)");
    IIsyncup((char *)0,0);
  }
/* # line 8653 "upgradedb.qsc" */	/* inquire_equel */
  {
    IIeqiqio((short *)0,1,30,4,&err_no,(char *)"errorno");
  }
/* # line 8654 "upgradedb.qsc" */	/* host code */
    if (err_no != 0)
    {
	du_talk(DU_MODESENS, &upgr_cb, E_DU2416_NOCOPY_SYSCAT, 4,
	    0, tblname, 0, dbname);
	upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
    }
    if (!rel_exists(tmpname))
    {
	if (upg_dbg_out)
	{
	    SIprintf("]==> make_copy_of failed!  copying %s to %s\n",
		tblname, tmpname);
	}
    }
}
/*{
** Name: copy_existing_columns - Copy columns from temp copy to new catalog
**
** Description:
**	When a catalog changes, very often all we need to do is copy the
**	existing catalog to a temp table, drop/recreate the catalog, and
**	copy the previously existing columns back into the catalog.
**	This routine will take care of the copy step, copying only those
**	columns that are found in both old and new versions of the
**	catalog.  Any other (new) columns are defaulted.
**
**	Note: if defaulting causes a problem, extend this routine to pass
**	two strings for insert-column-list and select-result-values, to be
**	appended to the insert.
**
** Inputs:
**	dbname				Name of the database (for errors)
**	tblname				Name of the catalog
**					(copy is assumed to be iitmp_tblname)
**
** Outputs:
**	Returns E_DU_xxx OK/ierror status
**	(error status sets upgr_cb.Dbname_ptr->errors)
**
** History:
**	30-Aug-2004 (schka24)
**	    Written.
**
*/
static DU_STATUS
copy_existing_columns(char *dbname, char *tblname)
{
    bool first_time;			/* First-time flag for comma */
  char colname[DB_MAXNAME+1];
  char insstring[850];
  char selstring[850];
  char sqlstring[1800];
  char tmpname[DB_MAXNAME+1];
  char tname[DB_MAXNAME+1];
    STcopy(tblname, tname);
    STcopy("iitmp_", tmpname);
    STcat(tmpname, tname);		/* Build table names */
    insstring[0] = '\0';
    selstring[0] = '\0';
    first_time = TRUE;
    /* All this lowercasing BS is to deal with ANSI-compliant databases */
    /* The varchar is for blank stripping */
/* # line 8407 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select varchar(a.attname)from iiattribute a, iirelation r where lower\
case(r.relid)=");
    IIputdomio((short *)0,1,32,0,tmpname);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and lowercase(r.relowner)='$ingres' and a.attrelid=r.reltid and a.att\
relidx=r.reltidx and exists(select 1 from iiattribute a1, iirelation r\
1 where lowercase(a1.attname)=lowercase(a.attname)and lowercase(r1.rel\
id)=");
    IIputdomio((short *)0,1,32,0,tname);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and lowercase(r1.relowner)='$ingres' and a1.attrelid=r1.reltid and a1\
.attrelidx=r1.reltidx)");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE8;
IIrtB8:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,colname);
      if (IIerrtest() != 0) goto IIrtB8;
/* # line 8417 "upgradedb.sc" */	/* host code */
	if (first_time)
	{
	    first_time = FALSE;
	}
	else
	{
	    STcat(insstring, ",");
	    STcat(selstring, ",");
	}
	/* no delimited ID worries with catalogs */
	STcat(insstring, colname);
	STcat(selstring, colname);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE8:;
  }
    if (sqlca.sqlcode < 0)
    {
	du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
	    0, tname, 0, dbname);
	upgr_cb.Dbname_ptr->errors = TRUE;
	return (E_DU_IERROR);
    }
/* # line 8437 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 8438 "upgradedb.sc" */	/* host code */
    STprintf(sqlstring, "insert into %s (%s) select %s from %s",
		tname, insstring, selstring, tmpname);
    if (upg_dbg_out)
    {
	SIprintf("]==> copy_existing_columns: %s\n", sqlstring);
    }
/* # line 8444 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sqlstring);
    IIsyncup((char *)0,0);
  }
/* # line 8445 "upgradedb.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
	    0, tname, 0, dbname);
	upgr_cb.Dbname_ptr->errors = TRUE;
	return (E_DU_IERROR);
    }
/* # line 8452 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 8453 "upgradedb.sc" */	/* host code */
    return (E_DU_OK);
}
/*{
** Name: bootstrap_syscats - convert order critical system cats to current
**
** Description:
**	The core catalog iidevices and the system catalogs iiintegrity,
**	iiprocedure, iipriv, iidbdepends, and iidevices are "bootstrap".
**	That means they MUST be converted in that order AND they must
**	be converted and correct at current level to proceed with the
**	conversion of the other system catalogs etc.
**
**	So, this routine converts these bootstrap system catalogs.
**	(using the time-honored copy into temporary table and back method).
**	It MUST be called immediately after modifying iirelation,
**	iiattribute, and iiindex.
**
**	If no converting is needed for any given catalog, nothing is done.
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	January 1993 (jpk)
**	    written
**	March 1993 (jpk)
**	    made more modular; separated out functions to convert
**	    each catalog.
**	November 1993 (jpk)
**	    Make sure to COMMIT before setting authorization.
**	December 1993 (jpk)
**	    Add iiprocedure_parameter to list of syscats to rebuild early;
**	    need for iiQEF_convert_table procedure to work.
**	14-jul-94 (robf)
**         Renamed from bootstrap_syscats
**	13-jun-95 (prida01)
**	    Add extra parameter to bootstrap_65_syscats for star
**	27-Aug-2004 (schka24)
**	    Since this is convert-to-current, not convert-to-6.5, rename
**	    and move talking into actual converter routines.
*/
static VOID
bootstrap_syscats(char *dbname, DBNAME_NODE *db_star_name, bool upcase)
{
    /* This is purely an optimization, it doesn't hurt to go through
    ** here multiple times.
    */
    if (upgr_cb.Dbname_ptr->did_bootstrap)
	return;
    if (upg_dbg_out)
    {
	SIprintf("]==> get iiintegrity, iipriv, iidbdepends to current\n");
    }
    du_talk(DU_MODESENS, &upgr_cb, I_DU00DC_UPGR_BOOT_CAT, 0);
    cv_iiintegrity(dbname);		/* iiintegrity */
    cv_iipriv(dbname);
    cv_65_iidbdepends(dbname);		/* iidbdepends */
    if (upg_dbg_out)
    {
	SIprintf("]==> get iidevices, iiprocedure, iiprocedure_parameter to 6.5 shape\n");
    }
    /* force priv guarantees xxx should be taken out?  was debugging? */
    IIlq_Protect(TRUE);
/* # line 8519 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 8520 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed((char *)"set session authorization $ingres");
    IIsyncup((char *)0,0);
  }
/* # line 8521 "upgradedb.sc" */	/* host code */
    cv_65_iidevices(dbname);		/* iidevices */
    cv_iiprocedure(dbname);		/* iiprocedure */
    if (!rel_exists("iiprocedure_parameter"))
    {
	du_talk(DU_MODESENS, &upgr_cb, I_DU00D2_IIPROCPARAM_CRE, 0);
	/* builds and modifies iiprocedure_parameter */
	duc_iiprocedure_parameter();
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
    }
    /* XXX something crazed prevents this from being done later */
    cv_65_iistatistics(dbname);
    /* fix for bug 71292 */
    if (db_star_name->star_flag)
    {
	cv_65_iidd_stats(db_star_name->dbname);
    }
    upgr_cb.Dbname_ptr->did_bootstrap = TRUE;
}
/*{
** Name: needs_widening	- returns whether a table needs DB_MAXNAME conversion
**
** Description:
**	
**	Verify that either
**	    the table exists and the column is of width 32: return FALSE;
**	or
**	    another condition obtains (including no table): return TRUE.
**
** Inputs:
**	table name
**	column name
**
** Outputs:
**	none
**
** Returns:
**	TRUE if table needs widening
**	FALSE otherwise
**
** History:
**	January 1993 (jpk)
**	     written
*/
static bool
needs_widening(tname, cname)
  char *tname;
  char *cname;
{
  int width;
    if (upg_dbg_out)
    {
	SIprintf("]==> does col %s in tbl %s need widening?\n", cname, tname);
    }
    if (!rel_exists(tname))
    {
	return TRUE;
    }
/* # line 8583 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select iiuserlen(ii_ext_type(a.attfrmt, a.attfrml), (ii_ext_length(a.\
attfrmt, a.attfrml)*65536) +a.attfrmp)from iiattribute a, iirelation r\
 where lowercase(r.relid)=");
    IIputdomio((short *)0,1,32,0,tname);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and lowercase(r.relowner)='$ingres' and lowercase(a.attname)=");
    IIputdomio((short *)0,1,32,0,cname);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and a.attrelid=r.reltid and a.attrelidx=r.reltidx");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&width);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 8589 "upgradedb.sc" */	/* host code */
    if (width == DB_MAXNAME)
    {
	return FALSE;
    }
    else	/* all other conditions, including 0 if something went wrong */
    {
	return TRUE;
    }
    /* NOTREACHED */
}
/*{
** Name: file_extend_convert_ingres	- convert INGRES tables for file extend
**
** Description:
**	Use internal database procedure iiQEF_convert_table.
**	Iterate over the set of INGRES tables, calling
**	internal database procedure iiQEF_convert_table on each.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	January 1993 (jpk)
**	    written
**	March 1993 (jpk)
**	    Separated out conversion of user tables (can and should
**	    be performed later in the upgrade sequence).
*/
file_extend_convert_ingres()
{
  u_i4 reltid;
  u_i4 reltidx;
  i4 return_status;
  char relname[DB_MAXNAME + 1];
    du_talk(DU_MODESENS, &upgr_cb, I_DU00E2_FILE_EXTEND_INGRES, 0);
/* # line 8629 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 8630 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed((char *)"set session authorization $ingres");
    IIsyncup((char *)0,0);
  }
/* # line 8631 "upgradedb.sc" */	/* host code */
    /* add internal db procedure: iiQEF_convert_table */
    add_iiqef_convert_table();
    /* call iiQEF_convert_table to convert unconverted system cats */
    /* XXX this query includes views, which is a no-op */
    if (upg_dbg_out)
    {
	SIprintf("]==> call iiQEF_convert_table for INGRES tables\n");
    }
    /* Note: no ANSI uppercase worries here, 6.4 databases are lowercase */
/* # line 8643 "upgradedb.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c1",7659,18646);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select trim(relid), reltid, reltidx from iirelation where relowner='$\
ingres' and relfhdr=0");
    IIcsQuery((char *)"c1",7659,18646);
  }
/* # line 8645 "upgradedb.sc" */	/* host code */
    for (;;)
    {
/* # line 8647 "upgradedb.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c1",7659,18646,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,DB_MAXNAME + 1-1,relname);
      IIcsGetio((short *)0,1,30,4,&reltid);
      IIcsGetio((short *)0,1,30,4,&reltidx);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode == 100) 
      goto closec1;
  }
/* # line 8648 "upgradedb.sc" */	/* host code */
	if (upg_dbg_out)
	{
	    SIprintf("]==> file extend converting ingres table %s\n", relname);
	}
/*
** perhaps unwise to parade long list of INGRES tables past user?
** du_talk(DU_MODESENS, &upgr_cb, I_DU00E3_FILE_EXTEND_TBL, 2, 0, relname);
*/
/* # line 8656 "upgradedb.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"iiQEF_convert_table");
    IILQprvProcValio((char *)"table_id",0,(short *)0,1,30,4,&reltid);
    IILQprvProcValio((char *)"index_id",0,(short *)0,1,30,4,&reltidx);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
    IIeqiqio((short *)0,1,30,4,&return_status,(char *)"iiret");
  }
/* # line 8659 "upgradedb.sc" */	/* host code */
	if (return_status != 0)
	{
	    if (upg_dbg_out)
	    {
		SIprintf("]==> error executing iiQEF_convert_table on %s (%d,%d)\n",
		    relname, reltid, reltidx);
	    }
	    du_talk(DU_MODESENS, &upgr_cb, I_DU00E6_FILE_EXTEND_TBL_ERR, 2,
		0, return_status, 0, relname);
	    upgr_exit(DU_FAIL_EXIT, 0);
	}
    }
    closec1:
/* # line 8672 "upgradedb.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c1",7659,18646);
    if (sqlca.sqlcode == 100) 
      goto closec1;
  }
/* # line 8673 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 100) 
      goto closec1;
  }
/* # line 8675 "upgradedb.sc" */	/* host code */
}
/*{
** Name: file_extend_convert_user	- convert user tables for file extend
**
** Description:
**	Use internal database procedure iiQEF_convert_table.
**	Iterate over the set of user tables, calling
**	iiQEF_convert_table on each.  Actually over set of ALL
**	unconverted tables, for robustness; handles possibility
**	that somehow an INGRES table didn't get converted earlier.
**	Thus at end of this procedure ALL tables are known to
**	be converted for file extend.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	March 1993 (jpk)
**	    Separated out from file_extend_convert.
**      08-jul-1999 (chash01) (orignally, 1997)
**          for rms gateway, converting views and imported table causes
**          error, they must be excluded. Declare another cursor c3 to 
**          handle rms gateway stuff.  This is because embedded sql can
**          not handle C if---else language structure.
**      21-Jul-1999 (wanya01)
**      Retrieve BE errors during table file_extend_convert. Abort
**      upgradedb if errors is found.  (Bug 98017)
**      09-feb-04 (chash01)  then change for RMS gateway is no longer needed.
**	7-Sep-2004 (schka24)
**	    Skip views, don't need to convert them.  DMF would skip them
**	    anyway but no point in cluttering up the log.
*/
file_extend_convert_user()
{
  u_i4 reltid;
  u_i4 reltidx;
  i4 return_status;
  char relname[DB_MAXNAME + 1];
  char error_buf[SQL_MSG_BUF_LEN];
  int err;
    du_talk(DU_MODESENS, &upgr_cb, I_DU00E5_FILE_EXTEND_USER, 0);
/* # line 8721 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 8722 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed((char *)"set session authorization $ingres");
    IIsyncup((char *)0,0);
  }
/* # line 8723 "upgradedb.sc" */	/* host code */
    /* call iiQEF_convert_table to convert user tables */
    if (upg_dbg_out)
    {
	SIprintf("]==> call iiQEF_convert_table for user tables\n");
    }
/* # line 8732 "upgradedb.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c2",7660,29896);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select trim(relid), reltid, reltidx from iirelation where relfhdr=0 a\
nd mod(relstat/32, 2)!=1");
    IIcsQuery((char *)"c2",7660,29896);
  }
/* # line 8734 "upgradedb.sc" */	/* host code */
        for (;;)
        {
/* # line 8736 "upgradedb.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c2",7660,29896,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,DB_MAXNAME + 1-1,relname);
      IIcsGetio((short *)0,1,30,4,&reltid);
      IIcsGetio((short *)0,1,30,4,&reltidx);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode == 100) 
      goto closec2;
  }
/* # line 8737 "upgradedb.sc" */	/* host code */
	    if (upg_dbg_out)
	    {
	        SIprintf("]==> file extend converting user table %s\n", relname);
	    }
	    du_talk(DU_MODESENS, &upgr_cb, I_DU00E3_FILE_EXTEND_TBL,
                    2, 0, relname);
/* # line 8743 "upgradedb.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"iiQEF_convert_table");
    IILQprvProcValio((char *)"table_id",0,(short *)0,1,30,4,&reltid);
    IILQprvProcValio((char *)"index_id",0,(short *)0,1,30,4,&reltidx);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
    IIeqiqio((short *)0,1,30,4,&return_status,(char *)"iiret");
  }
/* # line 8746 "upgradedb.sc" */	/* host code */
	    if (return_status != 0)
	    {
	        if (upg_dbg_out)
	        {
		SIprintf("]==> error executing iiQEF_convert_table on %s (%d,%d)\n",
		    relname, reltid, reltidx);
	        }
                SIprintf("Aborting upgrade of this database because of error");
                du_log (upgr_cb.du_skiplog, &upgr_cb.du_log,
                   "Aborting upgrade of this database because of error");
/* # line 8756 "upgradedb.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,SQL_MSG_BUF_LEN-1,error_buf,63);
    IILQisInqSqlio((short *)0,1,30,4,&err,0);
  }
/* # line 8757 "upgradedb.sc" */	/* host code */
                du_log (upgr_cb.du_skiplog, &upgr_cb.du_log, error_buf);
                SIprintf("\n%s", error_buf);
	        du_talk(DU_MODESENS, &upgr_cb, I_DU00E6_FILE_EXTEND_TBL_ERR, 2,
		        0, err, 0, relname);
	        upgr_exit(DU_FAIL_EXIT, 0);
	    }
        }
closec2:
/* # line 8765 "upgradedb.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c2",7660,29896);
    if (sqlca.sqlcode == 100) 
      goto closec2;
  }
/* # line 8766 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 100) 
      goto closec2;
  }
/* # line 8768 "upgradedb.sc" */	/* host code */
}
/*{
** Name: add_iiqef_convert_table - add internal db proc iiQEF_convert_table
**
** Description:
**	Adds internal database procedure iiQEF_convert_table.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	January 1993 (jpk)
**	     written
*/
add_iiqef_convert_table()
{
    if (upg_dbg_out)
    {
	SIprintf("]==> adding iiQEF_convert_table\n");
    }
    if (proc_exists("iiqef_convert_table"))
    {
/* # line 8793 "upgradedb.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"iiQEF_convert_table");
    IIwritio(0,(short *)0,1,32,0,(char *)
"drop procedure iiQEF_convert_table");
    IIsyncup((char *)0,0);
  }
/* # line 8794 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 8795 "upgradedb.sc" */	/* host code */
    }
/* # line 8796 "upgradedb.sc" */	/* create procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(0,(char *)"iiQEF_convert_table");
    IIwritio(0,(short *)0,1,32,0,(char *)
"create procedure iiQEF_convert_table (");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"        table_id        i4 not null not default,");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"        index_id        i4 not null not default ) as");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"    begin");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"        execute internal;");
    IIputctrl(0);
    IIwritio(0,(short *)0,1,32,0,(char *)"    end");
    IIputctrl(0);
    IIsyncup((char *)0,0);
  }
/* # line 8802 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 8803 "upgradedb.sc" */	/* host code */
}
/*{
** Name: new_for_65es_syscats -  build system catalogs new for 6.5 ES
**
** Description:
**
**	This routine builds the system catalogs new for 6.5 ES.  If, 
**      in some future release one of these catalogs changes, then it should 
**      be pulled out of this routine and an individual routine written 
**	that determines
**	whether to create the catalog in the new format or to modify the
**	catalog that was created for 6.5.  It creates the following system
**	catalogs:
**
**	    iisecalarm
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	11-jul-94 (robf)
**	    created for 6.5 ES.
*/
static VOID
new_for_65es_syscats(dbname)
char	*dbname;
{
    /* iisecalarm */
    if (!rel_exists("iisecalarm"))
    {
	du_talk(DU_MODESENS, &upgr_cb, I_DU00F1_IISECALARM_CRE, 0);
	duc_iisecalarm();        /* build iisecalarm */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
	duc_m_iisecalarm(); /* modify iisecalarm */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	    return;
	/* Now fill the security alarm table */
	if(populate_secalarm(dbname)!=OK)
	    return;
    }
}
/*{
** Name: new_for_65es_stdcats	-  build standard catalogs new for 6.5 ES
**
** Description:
**
**	This routine builds the standard catalogs new for 6.5 ES. If, in some
**	release one of these catalogs changes, then they should be pulled out
**	of this routine and an individual routine written that determines
**	whether to create the catalog in the new format or to modify the
**	catalog that was created for 6.5.  It creates the following 
**	standard catalogs:
**
**	    iisession_privileges
**
** Inputs:
**	database name
**
** History:
**	11-jul-94 (robf)
**	   Created
*/
static VOID
new_for_65es_stdcats(dbname)
char	*dbname;
{
    if (rel_exists("iiprivlist"))
    {
	/*
	** Also created by duc_iisess_priv()
	*/
/* # line 8878 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop iiprivlist");
    IIsyncup((char *)0,0);
  }
/* # line 8879 "upgradedb.sc" */	/* host code */
    }
    duc_iisesspriv();
    if (upgr_cb.Dbname_ptr->errors == TRUE)
	return;
}
/*{
** Name: new_for_65es_syscats_iidbdb -  build system catalogs new for 6.5ES 
**	 iidbdb.  
** 
** Description: 
**	This routine builds the new 6.5 system catalogs specific to the iidbdb.
**	If, in some future release one of these catalogs changes, it should 
**	be pulled out of this routine and an individual routine written that 
**	determines whether to create the catalog in the new format or to modify 
**	the catalog that was created for 6.5.  It creates the following 
**	system catalogs:
**	    iirolegrant
**	    iiprofile
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	11-jul-94 (robf)
**	    created 
*/
static VOID
new_for_65es_syscats_iidbdb(dbname)
char	*dbname;
{
    if (upg_dbg_out)
    {
	SIprintf("]==> new_for_65es_syscats_iidbdb(%s)\n\n", dbname);
    }
    cv_65es_iirolegrant(dbname);
    /* iiprofile */
    if (!rel_exists("iiprofile"))
    {
	du_talk(DU_MODESENS, &upgr_cb, I_DU00F5_IIPROFILE_CRE, 0);
	duc_iiprofile();
        if (upgr_cb.Dbname_ptr->errors == TRUE)
		return;
	duc_m_iiprofile();
        if (upgr_cb.Dbname_ptr->errors == TRUE)
		return;
    }                                     
}
/*{
** Name: changed_for_65es_syscats_iidbdb -  alter iidbdb syscats changing 
**       in 6.5 ES.
**
** Description:
**
**	This routine rebuilds the catalogs changed for 6.5 ES in the
**      iidbdb -- columns added, etc. Catalogs changed are:
**
**		iiuser
**		iidatabase
**		iilocations
**	 	iirole
**	        iisectype
**		iisecuritystate
**		iidbpriv
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	11-jul-94 (robf)
**	    created
*/
static VOID
changed_for_65es_syscats_iidbdb(dbname)
char	*dbname;
{
    if (upg_dbg_out)
    {
	SIprintf("]==> changed_for_65es_syscats_iidbdb(%s)\n\n", dbname);
    }
    du_talk(DU_MODESENS, &upgr_cb, I_DU00E7_CONVERTING_TBL, 2, 0, "iiuser");
    cv_65es_iiuser(dbname);
    cv_iidatabase(dbname);
    cv_iilocations(dbname);
    cv_iirole(dbname);
    du_talk(DU_MODESENS, &upgr_cb, I_DU00E7_CONVERTING_TBL, 2, 0, 
		"iisectype");
    cv_65es_iisectype(dbname);
    du_talk(DU_MODESENS, &upgr_cb, I_DU00E7_CONVERTING_TBL, 2, 0, 
		"iisecuritystate");
    cv_65es_iisecuritystate(dbname);
    du_talk(DU_MODESENS, &upgr_cb, I_DU00E7_CONVERTING_TBL, 2, 0, "iidbpriv");
    cv_65es_iidbpriv(dbname);	
}
/*{
** Name: cv_65es_iisectype - alter iisectype for 6.5 ES
**
** Description:
**
**	Allow for new types in audit system.
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	11-jul-94 (robf)
**	    created
*/
static VOID
cv_65es_iisectype(char *dbname)
{
    /*
    ** This catalog is just used to resolve view iisecurity_state, so
    ** we can destroy and recreate.
    */
    ensure_not_exists("iisectype");
    duc_iisectype();
    duc_m3_sectype();
/* # line 9004 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9005 "upgradedb.sc" */	/* host code */
}
/*{
** Name: cv_65es_iirolegrant - create iirolegrant for 6.5 ES
**
** Description:
**
**	Creates and populates catalog.
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	15-jul-94 (robf)
**	    created
*/
static VOID
cv_65es_iirolegrant(char *dbname)
{
    du_talk(DU_MODESENS, &upgr_cb, I_DU00F4_IIROLEGRANT_CRE, 0);
    /* iirolegrant */
    if (!rel_exists("iirolegrant"))
    {
	duc_iirolegrant();
        if (upgr_cb.Dbname_ptr->errors == TRUE)
		return;
    }                                     
    /*
    ** Now add role grants for backwards compatibility
    */
    if (upg_dbg_out)
	    SIprintf("]==> Filling iirolegrant...\n");
/* # line 9039 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iirolegrant(rgr_rolename, rgr_flags, rgr_gtype, rgr_grant\
ee)select distinct roleid, 0, 3, 'public' from iirole where roleid not\
 in(select rgr_rolename from iirolegrant)");
    IIsyncup((char *)0,0);
  }
/* # line 9044 "upgradedb.sc" */	/* host code */
    check_sql(sqlca.sqlcode);
    if (sqlca.sqlcode < 0)
    {
	du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
	    0, "iirolegrant", 0, dbname);
	upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
    }
    duc_m_iirolegrant();
    if (sqlca.sqlcode < 0)
    {
        /* warning: modify didn't work */
        du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
	    0, ERx("iirolegrant"), 0, dbname);
    }
/* # line 9058 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9059 "upgradedb.sc" */	/* host code */
}
/*{
** Name: cv_65es_iisecuritystate - alter iisecuritytype for 6.5 ES
**
** Description:
**
**	Allow for new types in audit system, without losing current
**      audit state information
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	11-jul-94 (robf)
**	    created
*/
static VOID
cv_65es_iisecuritystate(char *dbname)
{
  i4 cnt;
    /* 16==Resource */
/* # line 9085 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iisecuritystate where state=16");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 9089 "upgradedb.sc" */	/* host code */
    if (upgr_cb.Dbname_ptr->errors == TRUE)
	return;
    if(cnt!=1)
    {
	/* Add new values */
/* # line 9094 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iisecuritystate(type, id, state)values(1, 16, 0)");
    IIsyncup((char *)0,0);
  }
/* # line 9096 "upgradedb.sc" */	/* host code */
        if (upgr_cb.Dbname_ptr->errors == TRUE)
	{
		du_talk(DU_MODESENS, &upgr_cb, E_DU3200_BAD_APPEND, 4,
		    0, "iisecuritystate", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
/* # line 9102 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9103 "upgradedb.sc" */	/* host code */
    }
    /* 17==QueryText */
/* # line 9105 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iisecuritystate where state=17");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 9109 "upgradedb.sc" */	/* host code */
    if (upgr_cb.Dbname_ptr->errors == TRUE)
	return;
    if(cnt!=1)
    {
/* # line 9113 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iisecuritystate(type, id, state)values(1, 17, 0)");
    IIsyncup((char *)0,0);
  }
/* # line 9115 "upgradedb.sc" */	/* host code */
        if (upgr_cb.Dbname_ptr->errors == TRUE)
	{
		du_talk(DU_MODESENS, &upgr_cb, E_DU3200_BAD_APPEND, 4,
		    0, "iisecuritystate", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
    }
/* # line 9122 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9123 "upgradedb.sc" */	/* host code */
}
/*{
** Name: changed_for_r3_syscats-  alter system catalogs changing for r3
**
** Description:
**
**	This routine builds the catalogs changed for release 3:
**	   iirule
**         iievent
**	   iisequence
**
**	iiprocedure and iiintegrity are done in "order critical", in the
**	bootstrap-syscats call.  iiprotect is done by hand early in the main
**	convert driver.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	27-Aug-2004 (schka24)
**	    Written (was originally 65ES conversion, which no longer applies).
*/
static VOID
changed_for_r3_syscats(char *dbname, bool upcase)
{
    if (upg_dbg_out)
    {
	SIprintf("]==> changed_for_r3_syscats(%s)\n\n", dbname);
    }
    cv_iirule(dbname);
    cv_iievent(dbname, upcase);
    cv_iisequence(dbname);
}
/*{
** Name: cv_iievent - alter iievent to current
**
** Description:
**
**	Drop event_secid column, make it event_free
**	Widen columns if necessary 
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	12-jul-94 (robf)
**	    created
**	27-Aug-2004 (schka24)
**	    event_secid becomes event_free
*/
static VOID
cv_iievent(char *dbname, bool upcase)
{
    bool widen;
    widen = needs_widening("iievent", "event_name") ||
		needs_widening("iievent", "event_owner");
    if ( tbl_lacks_col("iievent", "event_free") ||
        widen)
    {
	du_talk(DU_MODESENS, &upgr_cb, I_DU00E7_CONVERTING_TBL, 2, 0, "iievent");
	/* See if table has event_secid, in that case just hack up
	** iiattribute to change the name instead of fooling with
	** recreating the table.
	*/
	if (! tbl_lacks_col("iievent","event_secid") && !widen)
	{
	    /* Must be the most recent pre-r3 version, just rename
	    ** the secid column.  (core catalog conversion took care of
	    ** old security label data type.)
	    */
	    if (upg_dbg_out)
		SIprintf("]==> Whacking iiattribute for iievent.event_secid\n");
	    if (upcase)
	    {
/* # line 9203 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiattribute a set attname='EVENT_FREE' where lowercase(a.attna\
me)='event_secid' and exists(select * from iirelation r where lowercas\
e(r.relid)='iievent' and lowercase(r.relowner)='$ingres' and a.attreli\
d=r.reltid and a.attrelidx=r.reltidx)");
    IIsyncup((char *)0,0);
  }
/* # line 9210 "upgradedb.sc" */	/* host code */
	    }
	    else
	    {
/* # line 9213 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiattribute a set attname='event_free' where lowercase(a.attna\
me)='event_secid' and exists(select * from iirelation r where lowercas\
e(r.relid)='iievent' and lowercase(r.relowner)='$ingres' and a.attreli\
d=r.reltid and a.attrelidx=r.reltidx)");
    IIsyncup((char *)0,0);
  }
/* # line 9220 "upgradedb.sc" */	/* host code */
	    }
	    if (sqlca.sqlcode < 0)
	    {
    		du_talk(DU_MODESENS, &upgr_cb, E_DU3201_BAD_REPLACE, 4,
        		0, "iievent", 0, "iidbdb");
        	upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
/* # line 9227 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9228 "upgradedb.sc" */	/* host code */
	    return;
	}
        /* Need to convert iievent to add event_free column and
	** widen if necessary
	*/
        make_copy_of(dbname, "iievent");
        upgr_cb.Ignore_errors = TRUE;
/* # line 9235 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop iievent");
    IIsyncup((char *)0,0);
  }
/* # line 9236 "upgradedb.sc" */	/* host code */
        if (sqlca.sqlcode < 0)
        {
    		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
        		0, "iievent", 0, "iidbdb");
        	upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
        }
        upgr_cb.Ignore_errors = FALSE;
        upgr_cb.Dbname_ptr->errors = FALSE;
        duc_iievent();
	if (sqlca.sqlcode < 0)
        {
            du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
        		0, "iievent", 0, "iidbdb");
        	upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
        }
	if (copy_existing_columns(dbname,"iievent") != E_DU_OK)
        {
        	du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
        		0, "iievent", 0, "iidbdb");
        	upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
        }
/* # line 9257 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9258 "upgradedb.sc" */	/* host code */
        /* Modify iievent to current structure */
	duc_modify_iievent();
    }
} 
/*{
** Name: cv_iipriv - alter iipriv to current
**
** Description:
**
**	Create additional iipriv columns for 1024 column limit.
**	(Creates the table if it doesn't exist.)
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	8-Sep-2004 (schka24)
**	    created for extra iipriv columns
*/
static VOID
cv_iipriv(char *dbname)
{
    bool exists;
    exists = rel_exists("iipriv");
    if ( !exists || tbl_lacks_col("iipriv", "i_priv_map33") )
    {
	if (upg_dbg_out)
	{
	    SIprintf("]==> %sing iipriv\n",exists ? "Convert" : "Creat");
	}
	du_talk(DU_MODESENS, &upgr_cb, I_DU00E7_CONVERTING_TBL, 2, 0, "iipriv");
        /* Need to convert iipriv to add more bitmap columns */
	if (exists)
	{
	    make_copy_of(dbname, "iipriv");
	    upgr_cb.Ignore_errors = TRUE;
	    /* Server does special stuff to let iipriv drop get through:
	    ** it doesn't check dependencies.  Drop the index by hand.
	    */
/* # line 9300 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop index iixpriv");
    IIsyncup((char *)0,0);
  }
/* # line 9301 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iipriv");
    IIsyncup((char *)0,0);
  }
/* # line 9302 "upgradedb.sc" */	/* host code */
	    check_sql(sqlca.sqlcode);
	    if (sqlca.sqlcode < 0)
	    {
    		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
        		0, "iipriv", 0, "iidbdb");
        	upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	    upgr_cb.Ignore_errors = FALSE;
	    upgr_cb.Dbname_ptr->errors = FALSE;
	}
        duc_iipriv();
	if (sqlca.sqlcode < 0)
        {
            du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
        		0, "iipriv", 0, "iidbdb");
        	upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
        }
	if (exists)
	{
	    if (copy_existing_columns(dbname,"iipriv") != E_DU_OK)
	    {
        	du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
        		0, "iipriv", 0, "iidbdb");
        	upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
/* # line 9327 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iipriv set i_priv_map11= -1, i_priv_map12= -1, i_priv_map13= -\
1, i_priv_map14= -1, i_priv_map15= -1, i_priv_map16= -1, i_priv_map17=\
 -1, i_priv_map18= -1, i_priv_map19= -1, i_priv_map20= -1, i_priv_map2\
1= -1, i_priv_map22= -1, i_priv_map23= -1, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"i_priv_map24= -1, i_priv_map25= -1, i_priv_map26= -1, i_priv_map27= -\
1, i_priv_map28= -1, i_priv_map29= -1, i_priv_map30= -1, i_priv_map31=\
 -1, i_priv_map32= -1, i_priv_map33= -1 where i_priv_map1= -1 and i_pr\
iv_map2= -1 and i_priv_map3= -1 and ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"i_priv_map4= -1 and i_priv_map5= -1 and i_priv_map6= -1 and i_priv_ma\
p7= -1 and i_priv_map8= -1 and i_priv_map9= -1 and i_priv_mapa= -1");
    IIsyncup((char *)0,0);
  }
/* # line 9340 "upgradedb.sc" */	/* host code */
	}
        /* Modify iipriv to current structure */
	duc_m7_priv();
/* # line 9343 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9344 "upgradedb.sc" */	/* host code */
    }
} 
/*{
** Name: cv_iirule - alter iirule to current for release 3
**
** Description:
**
**	Delete rule_secid, add more rule_colxx to iirule.
**	This also works for conversion from 6.4 since we drop/recreate.
**	No special init of new columns needed.
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	12-jul-94 (robf)
**	    created
**	27-Aug-2004 (schka24)
**	    transmogrify into r3 update, change name
*/
static VOID
cv_iirule(dbname)
char	*dbname;
{
    /* if iirule lacks release 3 columns, add them */
    if ( tbl_lacks_col("iirule", "rule_col33"))
    {
	bool	existed = rel_exists("iirule");
	du_talk(DU_MODESENS, &upgr_cb, I_DU00E7_CONVERTING_TBL, 2, 0, "iirule");
	if (existed)
	{
            make_copy_of(dbname, "iirule");
/* # line 9379 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iirule");
    IIsyncup((char *)0,0);
  }
/* # line 9380 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
		    0, "iirule", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_iirule();		/* create new iirule */
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
		0, "iirule", 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
 	if (existed)
 	{
	    if (copy_existing_columns(dbname, "iirule") != E_DU_OK)
 	    {
 		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
 	    }
 	}
/* # line 9401 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9402 "upgradedb.sc" */	/* host code */
 	duc_modify_iirule();	/* modify back to hash and add index */
 	if (sqlca.sqlcode < 0)
 	{
 	    /* warning: modify didn't work */
 	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
 		0, ERx("iirule"), 0, dbname);
 	}
/* # line 9409 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9410 "upgradedb.sc" */	/* host code */
    }
}
/*{
** Name: cv_iisequence - create or alter iisequence to current for release 3
**
** Description:
**
**	iisequence was added in 2.6SP2, so it can't be treated as strictly
**	a "new" catalog.  If it exists, and has the old security-id column,
**	fix it.
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	30-Aug-2004 (schka24)
**	    written
*/
static VOID
cv_iisequence(dbname)
char	*dbname;
{
    bool existed;
    /* if iisequence doesn't exist, or has the 2.6SP2 column, upgrade it */
    existed = rel_exists("iisequence");
    if ( ! existed || !tbl_lacks_col("iisequence", "seq_secid"))
    {
	du_talk(DU_MODESENS, &upgr_cb, I_DU00E7_CONVERTING_TBL, 2, 0, "iisequence");
	if (existed)
	{
            make_copy_of(dbname, "iisequence");
/* # line 9444 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iisequence");
    IIsyncup((char *)0,0);
  }
/* # line 9445 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
		    0, "iisequence", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_iisequence();		/* create new iisequence */
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
		0, "iisequence", 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
 	if (existed)
 	{
	    if (copy_existing_columns(dbname, "iisequence") != E_DU_OK)
 	    {
 		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
 	    }
 	}
/* # line 9466 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9467 "upgradedb.sc" */	/* host code */
 	duc_modify_iisequence();	/* modify back to hash */
 	if (sqlca.sqlcode < 0)
 	{
 	    /* warning: modify didn't work */
 	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
 		0, ERx("iisequence"), 0, dbname);
 	}
/* # line 9474 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9475 "upgradedb.sc" */	/* host code */
    }
}
/*{
** Name: new_for_r3_syscats - New catalogs for release 3
**
**	Add the partitioning catalogs for release 3.
**	Note that iisequence isn't new, see "changed for r3".
**
** Inputs:
**	dbname		Database name
**
** Outputs:
**	None
**
** History:
**	30-Aug-2004 (schka24)
**	    Written.
*/
static VOID
new_for_r3_syscats(char *dbname)
{
    if (! rel_exists("iipartname"))
    {
	du_talk(DU_MODESENS, &upgr_cb, I_DU0101_PARTITION_CRE, 0);
	duc_iipartcats();
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
		0, "partitioning catalogs", 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
	duc_modify_iipartcats();
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
		0, "partitioning catalogs", 0, dbname);
	}
/* # line 9512 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9513 "upgradedb.sc" */	/* host code */
    }
} /* new_for_r3_syscats */
/*{
** Name: changed_for_r3_syscats_iidbdb -  alter iidbdb syscats changing 
**       in release 3.
**
** Description:
**
**	This routine rebuilds the catalogs changed for release in the
**      iidbdb -- columns added, etc.  These are mostly to remove security ID's.
**	Catalogs changed are:
**
**		iiaudit
**		iidatabase
**		iilocations
**		iiprofile
**	 	iirole
**		iiuser
**
** Inputs:
**	dbname		Database name
**	upcase		TRUE if ansi (uppercase name) database
**
** Outputs:
**	none
**
** History:
**	30-Aug-2004 (schka24)
**	    Written.
*/
static VOID
changed_for_r3_syscats_iidbdb(char *dbname, bool upcase)
{
    if (upg_dbg_out)
    {
	SIprintf("]==> changed_for_r3_syscats_iidbdb(%s)\n\n", dbname);
    }
    cv_iiaudit(dbname);
    cv_r3_iiuser_prof(dbname,upcase);
    cv_iidatabase(dbname);
    cv_iilocations(dbname);
    cv_iirole(dbname);
    /* Drop no-longer-existing B1 catalogs */
    ensure_not_exists("iilevelaudit");
    ensure_not_exists("iilabelaudit");
    ensure_not_exists("iilabelmap");
} /* changed_for_r3_syscats_iidbdb */
/*{
** Name: cv_iiaudit - alter iiaudit for release 3
**
** Description:
**
**	Remove objectlabel column added in 6.5ES.  This is a registration,
**	not a regular table.
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	30-Aug-2004 (schka24)
**	    r3 changes.
*/
static VOID
cv_iiaudit(char *dbname)
{
    du_talk(DU_MODESENS, &upgr_cb, I_DU00E7_CONVERTING_TBL, 2, 0, "iiaudit");
/* # line 9582 "upgradedb.sc" */	/* remove */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"remove iiaudit");
    IIsyncup((char *)0,0);
  }
/* # line 9583 "upgradedb.sc" */	/* host code */
    duc_r_iiaudit();			/* Re-register */
    if (sqlca.sqlcode < 0)
    {
	du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
	    0, "iiaudit", 0, dbname);
	/* Allow upgrade to continue */
    }
/* # line 9590 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9591 "upgradedb.sc" */	/* host code */
}
/*{
** Name: cv_iidatabase - alter iidatabase for release 3
**
** Description:
**
**	Remove dbsecid column added in 6.5ES
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	12-jul-94 (robf)
**	    created
**	30-Aug-2004 (schka24)
**	    r3 changes.
*/
static VOID
cv_iidatabase(char *dbname)
{
    /* convert if iidatabase columns need widening or new column needed*/
    if ( (!tbl_lacks_col("iidatabase","dbsecid")) ||
	tbl_lacks_col("iidatabase", "dbfree") ||
        needs_widening("iidatabase", "name") ||
	needs_widening("iidatabase", "own") ||
	needs_widening("iidatabase", "dbdev") ||
	needs_widening("iidatabase", "ckpdev") ||
	needs_widening("iidatabase", "jnldev") ||
	needs_widening("iidatabase", "sortdev") ||
	needs_widening("iidatabase", "dmpdev"))
    {
	bool	existed = rel_exists("iidatabase");
	du_talk(DU_MODESENS, &upgr_cb, I_DU00E7_CONVERTING_TBL, 2, 0, "iidatabase");
	if (existed)
	{
	    make_copy_of(dbname, "iidatabase");
/* # line 9630 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iidatabase");
    IIsyncup((char *)0,0);
  }
/* # line 9631 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
		    0, "iidatabase", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_iidatabase();		/* create new iidatabase */
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
		0, "iidatabase", 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
	if (existed)
	{
	    if (copy_existing_columns(dbname, "iidatabase") != E_DU_OK)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
		    0, "iidatabase", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_modify_iidatabase();	/* modify back to hash and add index */
	if (sqlca.sqlcode < 0)
	{
	    /* warning: modify didn't work */
	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
		0, ERx("iidatabase"), 0, dbname);
	}
    }
    if (upgr_cb.Dbname_ptr->level < V65ES_LEVEL)
    {
	/*
	** This is a hang-over from the 64->65 conversion, 
	** since 65 database may have other sort devices we
	** don't do this automatically 
	*/
/* # line 9669 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iidatabase set sortdev='ii_work'");
    IIsyncup((char *)0,0);
  }
/* # line 9670 "upgradedb.sc" */	/* host code */
    }
/* # line 9671 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9672 "upgradedb.sc" */	/* host code */
}
/*{
** Name: cv_65es_iiuser - alter iiuser for 6.5 ES
**
** Description:
**
**     Add columns:
**	   - lim_secid
**         - profile_name 
**         - default_priv 
**         - expire_date
**         - flags_mask 
**         - user_priv 
**
**     Modify SECURITY privilege to be  SECURITY, MAINTAIN_USERS, AUDITOR, 
**     and MAINTAIN_AUDIT.
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	12-jul-94 (robf)
**	    created
*/
static VOID
cv_65es_iiuser(dbname)
char	*dbname;
{
    /* convert only if needed */
    if ( tbl_lacks_col("iiuser", "expire_date") ||
        needs_widening("iiuser", "name")  ||
	needs_widening("iiuser", "default_group") ||
	tbl_lacks_col("iiuser", "password"))
    {
	bool	existed = rel_exists("iiuser");
	if (existed)
	{
	    make_copy_of(dbname, "iiuser");
/* # line 9713 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iiuser");
    IIsyncup((char *)0,0);
  }
/* # line 9714 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
		    0, "iiuser", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_iiuser();		/* create new iiuser */
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
		0, "iiuser", 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
	if (existed)
	{
/* # line 9730 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiuser(name, gid, mid, status, default_group, profile_nam\
e, default_priv, expire_date, flags_mask, user_priv)select name, gid, \
mid, status, default_group, '', status, '', 16384, status from iitmp_i\
iuser");
    IIsyncup((char *)0,0);
  }
/* # line 9736 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
		    0, "iiuser", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_m5_user();	/* modify back to hash and add index */
	if (sqlca.sqlcode < 0)
	{
	    /* warning: modify didn't work */
	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
		0, ERx("iiuser"), 0, dbname);
	}
    }
    /*
    ** Now update user status so the SECURITY users now have appropriate
    ** finer-grained privileges (MAINTAIN_USERS, AUDITOR, MAINTAIN_AUDIT)
    */
/* # line 9755 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiuser set status=status +65536, default_priv=default_priv +65\
536, user_priv=user_priv +65536 where mod(status/32768, 2)=1 and mod(s\
tatus/65536, 2)=0");
    IIsyncup((char *)0,0);
  }
/* # line 9761 "upgradedb.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
	    0, "iiuser", 0, dbname);
	upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
    }
/* # line 9767 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiuser set status=status +16384, default_priv=default_priv +16\
384, user_priv=user_priv +16384 where mod(status/32768, 2)=1 and mod(s\
tatus/16384, 2)=0");
    IIsyncup((char *)0,0);
  }
/* # line 9773 "upgradedb.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
	    0, "iiuser", 0, dbname);
	upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
    }
/* # line 9779 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiuser set status=status +8192, default_priv=default_priv +819\
2, user_priv=user_priv +8192 where mod(status/32768, 2)=1 and mod(stat\
us/8192, 2)=0");
    IIsyncup((char *)0,0);
  }
/* # line 9785 "upgradedb.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
	    0, "iiuser", 0, dbname);
	upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
    }
    /* if a super user, then add 'operator' and 'create location' 
       privileges to the super user. (#67365) 
    */
    if (upgr_cb.Dbname_ptr->level< V65_LEVEL) 
    {
/* # line 9796 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiuser set status=status +2560, default_priv=default_priv +256\
0, user_priv=user_priv +2560 where mod(status/32768, 2)=1");
    IIsyncup((char *)0,0);
  }
/* # line 9801 "upgradedb.sc" */	/* host code */
      if (sqlca.sqlcode < 0)
       {
	du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
	    0, "iiuser", 0, dbname);
	upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
       }
     }
    /*
    /*
    ** Mark privileges flags appropriately.
    ** This ensures privileges are maintained when
    ** later merging/unmerging profile information.
    */
/* # line 9814 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiuser set flags_mask=24 where status!=0");
    IIsyncup((char *)0,0);
  }
/* # line 9817 "upgradedb.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
	    0, "iiuser", 0, dbname);
	upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
    }
    /*
    ** same for default group
    */
/* # line 9826 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiuser set flags_mask=flags_mask +4 where default_group!=''");
    IIsyncup((char *)0,0);
  }
/* # line 9829 "upgradedb.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
	    0, "iiuser", 0, dbname);
	upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
    }
/* # line 9835 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9836 "upgradedb.sc" */	/* host code */
}
/*{
** Name: cv_r3_iiuser_prof - alter iiuser/iiprofile for release 3.
**
** Description:
**	In release 3, the lim_secid column was changed to "reserve"
**	in the iiuser and iiprofile tables.  Since the column is otherwise
**	unchanged, we'll just hack up the iiattribute column name.
**
** Inputs:
**	dbname			The database name
**	upcase			TRUE if uppercase-name database
**
** Outputs:
**	none
**
** History:
**	30-Aug-2004 (schka24)
**	    Release 3 updates.
*/
static VOID
cv_r3_iiuser_prof(char *dbname, bool upcase)
{
    if (tbl_lacks_col("iiuser", "reserve")
      && !tbl_lacks_col("iiuser", "lim_secid") )
    {
	if (upg_dbg_out)
	    SIprintf("]==> Whacking iiattribute for iiuser, iiprofile lim_secid\n");
	if (upcase)
	{
/* # line 9866 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiattribute a set attname='RESERVE' where lowercase(attname)='\
lim_secid' and exists(select 1 from iirelation r where lowercase(r.rel\
id)in('iiuser', 'iiprofile')and lowercase(r.relowner)='$ingres' and a.\
attrelid=r.reltid and a.attrelidx=r.reltidx)");
    IIsyncup((char *)0,0);
  }
/* # line 9873 "upgradedb.sc" */	/* host code */
	}
	else
	{
/* # line 9876 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiattribute a set attname='reserve' where lowercase(attname)='\
lim_secid' and exists(select 1 from iirelation r where lowercase(r.rel\
id)in('iiuser', 'iiprofile')and lowercase(r.relowner)='$ingres' and a.\
attrelid=r.reltid and a.attrelidx=r.reltidx)");
    IIsyncup((char *)0,0);
  }
/* # line 9883 "upgradedb.sc" */	/* host code */
	}
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU3201_BAD_REPLACE, 4,
		0, "iiattribute", 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
/* # line 9890 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9891 "upgradedb.sc" */	/* host code */
    }
}
/*{
** Name: cv_iilocations - alter iilocations
**
** Description:
**
**	Make sure table has rawpct column (2.6) and free column (r3,
**	renamed from 6.5ES lsecid column).  To make life simpler we'll just
**	recreate the locations table as needed.
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	12-jul-94 (robf)
**	    created
**	30-Aug-2004 (schka24)
**	    Make this be a convert-to-current type of routine.
*/
static VOID
cv_iilocations(dbname)
char	*dbname;
{
    /* 
    ** convert if iilocations column needs widening or new column 
    ** needs to be added
    */
    if (tbl_lacks_col("iilocations", "free") ||
	tbl_lacks_col("iilocations", "rawpct") ||
	!tbl_lacks_col("iilocations", "lsecid") ||
        needs_widening("iilocations", "lname"))
    {
	bool	existed = rel_exists("iilocations");
	du_talk(DU_MODESENS, &upgr_cb, I_DU00E7_CONVERTING_TBL, 2, 0,
	"iilocations");
	if (existed)
	{
	    make_copy_of(dbname, "iilocations");
/* # line 9933 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iilocations");
    IIsyncup((char *)0,0);
  }
/* # line 9934 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
		    0, "iilocations", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_iilocations();		/* create new iilocations */
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
		0, "iilocations", 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
	if (existed)
	{
	    if (copy_existing_columns(dbname, "iilocations") != E_DU_OK)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
		    0, "iilocations", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_modify_iilocations();	/* modify back to hash and add index */
	if (sqlca.sqlcode < 0)
	{
	    /* warning: modify didn't work */
	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
		0, ERx("iilocations"), 0, dbname);
	}
    }
    /* These are carried over from the 6.5 conversion */
    /* well, OK, this is harmless to do every time */
/* # line 9967 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iilocations set lname='ii_work' where lowercase(lname)='ii_sor\
t'");
    IIsyncup((char *)0,0);
  }
/* # line 9969 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iilocations set area='II_WORK' where lowercase(area)='ii_sort'");
    IIsyncup((char *)0,0);
  }
/* # line 9971 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 9972 "upgradedb.sc" */	/* host code */
}
/*{
** Name: cv_iirole - alter iirole for release 3
**
** Description:
**
**	Update iirole catalog to eliminate security ID columns.
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	12-jul-94 (robf)
**	    created
**	30-Aug-2004 (schka24)
**	    Revise for r3.
*/
static VOID
cv_iirole(dbname)
char	*dbname;
{
    /* convert only if need to add column or need to widen columns*/
    if (tbl_lacks_col("iirole", "roleflags") ||
        needs_widening("iirole", "roleid") ||
	(! tbl_lacks_col("iirole","rolesecid")) )
    {
	bool	existed = rel_exists("iirole");
	du_talk(DU_MODESENS, &upgr_cb, I_DU00E7_CONVERTING_TBL, 2, 0, "iirole");
	if (existed)
	{
	    make_copy_of(dbname, "iirole");
/* # line 10006 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iirole");
    IIsyncup((char *)0,0);
  }
/* # line 10007 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
		    0, "iirole", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_iirole();		/* create new iirole */
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
		0, "iirole", 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
	if (existed)
	{
	    /* Add new columns */
	    if (copy_existing_columns(dbname, "iirole") != E_DU_OK)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
		    0, "iirole", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_modify_iirole();	/* modify back to hash and add index */
	if (sqlca.sqlcode < 0)
	{
	    /* warning: modify didn't work */
	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
		0, ERx("iirole"), 0, dbname);
	}
/* # line 10038 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 10039 "upgradedb.sc" */	/* host code */
    }
}
/*
** Name: order_critical_65es
**
** Description:
**	65 ES conversions which must be done early.
**
** Inputs:
**	dbname  -  database name
**
** Outputs:
**	none
**
** History:
**	14-jul-94 (robf)
**          Created
**	25-Aug-2004 (schka24)
**	    Remove relsecid att stuff, server handles now.
*/
static VOID
order_critical_65es(char *dbname, bool upcase)
{
    if (upg_dbg_out)
    {
	SIprintf("]==> Doing order_critical_65es\n");
    }
    /* Trees need to be converted */
    upgr_cb.Dbname_ptr->tree_convert = TRUE;
    cv_iiprocedure(dbname);		/* iiprocedure */
}
/*{
** Name: cv_65es_iidbpriv - alter iidbpriv for 6.5 ES
**
** Description:
**
**	Add new columns (added atomically)
**	widen existing columns where necessary
**
** Inputs:
**	database name
**
** Outputs:
**	none
**
** History:
**	15-jul-94 (robf)
**	    created
*/
static VOID
cv_65es_iidbpriv(dbname)
char	*dbname;
{
    bool has_fill=FALSE;
    /* convert if iidbpriv columns need widening or columns are added */
    if (needs_widening("iidbpriv", "dbname") ||
	needs_widening("iidbpriv", "grantee") ||
	tbl_lacks_col("iidbpriv", "idle_time_limit"))
    {
	bool	existed = rel_exists("iidbpriv");
	if (existed)
	{
	    make_copy_of(dbname, "iidbpriv");
	    /*
	    ** The fill1 column changed from 6.4 to 6.5 to dbflags
	    */
	    if(tbl_lacks_col("iidbpriv", "fill1"))
		has_fill=FALSE;
	    else
		has_fill=TRUE;
/* # line 10109 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iidbpriv");
    IIsyncup((char *)0,0);
  }
/* # line 10110 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
		    0, "iidbpriv", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_iidbpriv();		/* create new iidbpriv */
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
		0, "iidbpriv", 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
	if (existed)
	{
	    if(has_fill)
	    {
/* # line 10128 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbpriv(dbname, grantee, gtype, dbflags, control, flags,\
 qrow_limit, qdio_limit, qcpu_limit, qpage_limit, qcost_limit, reserve\
)select dbname, grantee, gtype, fill1, control, flags, qrow_limit, qdi\
o_limit, qcpu_limit, qpage_limit, qcost_limit");
    IIwritio(0,(short *)0,1,32,0,(char *)", reserve from iitmp_iidbpri\
v");
    IIsyncup((char *)0,0);
  }
/* # line 10135 "upgradedb.sc" */	/* host code */
	    }
	    else
	    {
/* # line 10138 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbpriv(dbname, grantee, gtype, dbflags, control, flags,\
 qrow_limit, qdio_limit, qcpu_limit, qpage_limit, qcost_limit, reserve\
)select dbname, grantee, gtype, dbflags, control, flags, qrow_limit, q\
dio_limit, qcpu_limit, qpage_limit, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"qcost_limit, reserve from iitmp_iidbpriv");
    IIsyncup((char *)0,0);
  }
/* # line 10145 "upgradedb.sc" */	/* host code */
	    }
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
		    0, "iidbpriv", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_m6_dbpriv();	/* modify to btree */
	if (sqlca.sqlcode < 0)
	{
	    /* warning: modify didn't work */
	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
		0, ERx("iidbpriv"), 0, dbname);
	}
/* # line 10160 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 10161 "upgradedb.sc" */	/* host code */
    }
}
/*{
** Name: cv_26_iiextend - alter iiextend for 2.6 raw locations
**
** Description:
**	Add rawstart, rawblocks columns.
**
** Inputs:
**	database name ("iidbdb")
**
** Outputs:
**	none
**
** History:
**	02-Apr-2001 (jenjo02)
**	    Created.
**	11-May-2001 (jenjo02)
**	    Added rawblocks, moved here from iilocations.
*/
static VOID
cv_26_iiextend(
char	*dbname)
{
    /* 
    ** Recreate iiextend to add rawstart, rawblocks columns
    ** convert if iiextend column needs widening or new column 
    ** needs to be added
    */
    if ( tbl_lacks_col("iiextend", "rawstart") ||
         tbl_lacks_col("iiextend", "rawblocks") )
    {
	bool	existed = rel_exists("iiextend");
	du_talk(DU_MODESENS, &upgr_cb, I_DU00E7_CONVERTING_TBL, 2, 0, "iiextend");
	if (existed)
	{
	    make_copy_of(dbname, "iiextend");
/* # line 10198 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iiextend");
    IIsyncup((char *)0,0);
  }
/* # line 10199 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
		    0, "iiextend", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_iiextend();		/* create new iiextend */
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
		0, "iiextend", 0, dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
	if (existed)
	{
	    /* Add new columns */
/* # line 10216 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend(lname, dname, status, rawstart, rawblocks)select\
 lname, dname, status, 0, 0 from iitmp_iiextend");
    IIsyncup((char *)0,0);
  }
/* # line 10219 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
		    0, "iiextend", 0, dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	duc_modify_iiextend();	/* modify back to hash and add index */
	if (sqlca.sqlcode < 0)
	{
	    /* warning: modify didn't work */
	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
		0, ERx("iiextend"), 0, dbname);
	}
    }
}
/* Create a work directory for a database.  This can fail silently if
** we aren't the installation owner, which is OK.
** This ought to be needed only for iidbdb and only when upgrading from
** a 6.4 installation without work dirs;  the initial iidbdb query might
** need a work location and can fail without this step.
*/
chk_create_workdir(dbname)
 char *dbname;
{
   char     *pworkdir;
   LOCATION workloc;
   NMgtAt("II_WORK", &pworkdir);
   LOingpath(pworkdir, dbname, LO_WORK, &workloc);
   if (LOexist(&workloc) != OK)
       return(LOcreate(&workloc));
   return(OK);
}
/* rm_iitmp_tables
** description: remove all iitm_ tables created from make_copy_of()
** input : none
** output: none
*/
rm_iitmp_tables()
{
    ensure_not_exists("iitmp_iistatistics");
    ensure_not_exists("iitmp_iidd_stats");
    ensure_not_exists("iitmp_iidevices");
    ensure_not_exists("iitmp_iiextend");
    ensure_not_exists("iitmp_iidbdepends");
    ensure_not_exists("iitmp_iiusergroup");
    ensure_not_exists("iitmp_iirelation");
    ensure_not_exists("iitmp_iiqrytext");
    ensure_not_exists("iitmp_iiprotect");
    ensure_not_exists("iitmp_iiprocedure"); 
    ensure_not_exists("iitmp_iirule"); 
    ensure_not_exists("iitmp_iiintegrity"); 
    ensure_not_exists("iitmp_iievent"); 
    ensure_not_exists("iitmp_iirule"); 
    ensure_not_exists("iitmp_iidatabase"); 
    ensure_not_exists("iitmp_iiuser"); 
    ensure_not_exists("iitmp_iilocations"); 
    ensure_not_exists("iitmp_iirole"); 
    ensure_not_exists("iitmp_iidbpriv"); 
    ensure_not_exists("iitmp_iisequence");
    ensure_not_exists("xxtmp_views");
    ensure_not_exists("xxtmp_permits");
/* # line 10281 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 10282 "upgradedb.sc" */	/* host code */
}
/*{
** Name: rm_iiqef_procedures - Clean-up routine
**
** Makes sure upgradedb doesn't leave any temporary
** procedures (iiQEF_ ...) behind.
** Inputs
**      none
** Outputs:
**      none
**
** Exceptions:
**      none
**
** Side effects:
**      none
**
** History:
**      12-march-1998 (nicph02)
**      Created: Fix to bug 89164
[@history_template@]...
**/
STATIC void rm_iiqef_procedures()
{
  if (proc_exists("iiqef_make_udt_defaults"))
  {
/* # line 10308 "upgradedb.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"iiQEF_make_udt_defaults");
    IIwritio(0,(short *)0,1,32,0,(char *)
"drop procedure iiQEF_make_udt_defaults");
    IIsyncup((char *)0,0);
  }
/* # line 10309 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 10310 "upgradedb.sc" */	/* host code */
  }
  if (proc_exists("iiqef_convert_table"))
  {
/* # line 10313 "upgradedb.sc" */	/* drop procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(1,(char *)"iiqef_convert_table");
    IIwritio(0,(short *)0,1,32,0,(char *)
"drop procedure iiqef_convert_table");
    IIsyncup((char *)0,0);
  }
/* # line 10314 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 10315 "upgradedb.sc" */	/* host code */
  }
}
/*
** Name: order_critical_20
**
** Description:
**	2.0 conversions which must be done early.
**
** Inputs:
**	dbname  -  database name
**
** Outputs:
**	none
**
** History:
**      06-mar-1996 (nanpr01)
**          Created
**	16-Oct-1996 (hanch04)
**	    Added relversion, reltotwid, modified iiattribute.
**	26-Mar-1997 (jenjo02)
**	    Table priority project:
**	    Added reltcpri (table cache priority), reduced relfree.
**	25-Aug-2004 (schka24)
**	    Remove core catalog whirling, server will do it.
*/
static VOID
order_critical_20(char * dbname, DBNAME_NODE *db_star_name, bool upcase)
{
    if (upg_dbg_out)
    {
	SIprintf("]==> Doing order_critical_20\n");
    }
    /* Trees need to be converted */
    upgr_cb.Dbname_ptr->tree_convert = TRUE;
    /* done if not star */
    if (!db_star_name->star_flag)
	return;
/* # line 10352 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop view iidd_physical_tabl\
es");
    IIsyncup((char *)0,0);
  }
/* # line 10353 "upgradedb.sc" */	/* host code */
    /*if iidd_tables in the distributed database lacks 2.0 columns add them */
    if ( tbl_lacks_col("iidd_tables", "table_pagesize") )
    {
	bool   existed = rel_exists("iidd_tables");
	if (existed)
	{
	    make_copy_of(db_star_name->dbname, "iidd_tables");
/* # line 10360 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iidd_tables");
    IIsyncup((char *)0,0);
  }
/* # line 10361 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
		    0, "iidd_tables", 0, db_star_name->dbname);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	/* create table iidd_tables */
	duc_iidd_tables();
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
		0, "iidd_tables", 0, db_star_name->dbname);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
	if (existed)
	{
/* # line 10378 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidd_tables(table_name, table_owner, create_date, alter_d\
ate, table_type, table_subtype, table_version, system_use, table_stats\
, table_indexes, is_readonly, num_rows, storage_structure, is_compress\
ed, duplicate_rows, unique_rule, number_pages");
    IIwritio(0,(short *)0,1,32,0,(char *)
", overflow_pages, row_width, expire_date, modify_date, location_name,\
 table_integrities, table_permits, all_to_all, ret_to_all, is_journall\
ed, view_base, multi_locations, table_ifillpct, table_dfillpct, table_\
lfillpct, table_minpages, table_maxpages, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"table_relstamp1, table_relstamp2, table_reltid, table_reltidx, table_\
pagesize, table_relversion, table_reltotwid)select table_name, table_o\
wner, create_date, alter_date, table_type, table_subtype, table_versio\
n, system_use, table_stats, table_indexes, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"is_readonly, num_rows, storage_structure, is_compressed, duplicate_ro\
ws, unique_rule, number_pages, overflow_pages, row_width, expire_date,\
 modify_date, location_name, table_integrities, table_permits, all_to_\
all, ret_to_all, is_journalled, view_base, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"multi_locations, table_ifillpct, table_dfillpct, table_lfillpct, tabl\
e_minpages, table_maxpages, table_relstamp1, table_relstamp2, table_re\
ltid, table_reltidx, 2048, 0, row_width from iitmp_iidd_tables");
    IIsyncup((char *)0,0);
  }
/* # line 10405 "upgradedb.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
		   du_talk(DU_FAIL_EXIT, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
		       0, "iidd_tables", 0, db_star_name->dbname);
		   upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
		}
	}
	duc_modify_iidd_tables();   /* modify back to hash */
	if (sqlca.sqlcode < 0)
	{
	    /* warning: modify didn't work */
	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
		0, ERx("iidd_tables"), 0, db_star_name->dbname);
	}
/* # line 10419 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 10420 "upgradedb.sc" */	/* host code */
    }
    /*if iidd_indexes in the distributed database lacks 2.0 columns add them */
    if ( tbl_lacks_col("iidd_indexes", "table_pagesize") )
    {
	bool   existed = rel_exists("iidd_indexes");
	if (existed)
	{
	    make_copy_of(db_star_name->dbname, "iidd_indexes");
/* # line 10428 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iidd_indexes");
    IIsyncup((char *)0,0);
  }
/* # line 10429 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
		    0, "iidd_indexes", 0, db_star_name);
		upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	/* create table iidd_indexes */
	duc_iidd_indexes();
	if (sqlca.sqlcode < 0)
	{
	    du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
		0, "iidd_indexes", 0, db_star_name);
	    upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
	if (existed)
	{
		/* xxx may get sql error here because pad is truncated */
/* # line 10447 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidd_indexes(index_name, index_owner, create_date, base_n\
ame, base_owner, storage_structure, is_compressed, unique_rule, index_\
pagesize)select index_name, index_owner, create_date, base_name, base_\
owner, storage_structure, is_compressed, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"unique_rule, 2048 from iitmp_iidd_indexes");
    IIsyncup((char *)0,0);
  }
/* # line 10456 "upgradedb.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
		   du_talk(DU_FAIL_EXIT, &upgr_cb, E_DU2419_NOREPOP_SYSCAT, 4,
		       0, "iidd_indexes", 0, db_star_name);
		   upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
		}
	}
	duc_modify_iidd_indexes();   /* modify back to hash */
	if (sqlca.sqlcode < 0)
	{
	    /* warning: modify didn't work */
	    du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
                0, ERx("iidd_indexes"), 0, db_star_name);
	}
/* # line 10470 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 10471 "upgradedb.sc" */	/* host code */
    }
}
/*{
** Name: order_critical_r3 - Initial order critical changes for Release 3
**
** Description:
**
**	Perform order critical changes for r3.  iiattribute has already
**	been rehashed.  What's left to do is to modify some of the
**	"bootstrap" catalogs for 1024 columns (added domset columns),
**	and indicate that trees must be updated.
**
**	It's not entirely clear that the bootstrap catalogs *must* be
**	updated here, but we might as well;  it keeps the machinery
**	similar to upgrades from old versions eg 6.4.
**
**	The caller checks that the database is not yet at r3 level.
**
** Inputs:
**	info			DBNAME_NODE pointer to DB info
**
** Outputs:
**	none
**
** History:
**	27-Aug-2004 (schka24)
**	    Written.
*/
static void
order_critical_r3(DBNAME_NODE *info)
{
    if (upg_dbg_out)
    {
	SIprintf("]==> order critical r3 db %s\n",
	    info->dbname);
    }
    /* Trees need to be converted thanks to 1024 columns */
    info->tree_convert = TRUE;
    /* fix the "boostrap" system catalogs */
    bootstrap_syscats(info->dbname, info, info->upcase);
} /* order_critical_r3 */
/*{
** Name: order_critical_r302 - Initial order critical changes for Release 3.0.2
**
** Description:
**
**	Perform order critical changes for r3.0.2.  This release adds
**	an iiattribute attcollid column, extracting a bit of attfree.  Since
**	there's no reshuffling of iiattribute, DMF doesn't bother doing
**	this change, so we'll do it here.
**
**	We also reapply the storage structure for iiprotect, which changes
**	it to a btree and drops the unused iixprotect index.
**
**	The caller checks that the database is not yet at 3.0.2 level.
**
** Inputs:
**	info			DBNAME_NODE pointer to DB info
**	name_if_iidbdb		Connection DB name if iidbdb, else NULL
**
** Outputs:
**	none
**
** Side Effects:
**	May do a disconnect/reconnect if the DB had been at r3 level.
**
** History:
**	3-Jan-2005 (schka24)
**	    Written.
*/
static void
order_critical_r302(DBNAME_NODE *info, char *name_if_iidbdb)
{
  char attname[DB_MAXNAME + 1];
  char *iidbdbname;
  i4 reltid, reltidx;
  i4 sess_id;
    iidbdbname = name_if_iidbdb;
    if (upg_dbg_out)
    {
	SIprintf("]==> order critical r302 db %s, prior level %d\n",
	    info->dbname, info->level);
    }
    /* If the DB was originally older than R3, the DMF core catalog
    ** conversion rewrites iiattribute and therefore brings it all the
    ** way up to r3.0.2 level.  We only need to piss with the iiattribute
    ** columns if the database was originally an r3 database.
    */
    if (info->level == V30_LEVEL
      && tbl_lacks_col("iiattribute","attcollid"))
    {
	/* Shrink attfree, create attcollid */
	reltid = DM_B_ATTRIBUTE_TAB_ID;
	reltidx = DM_I_ATTRIBUTE_TAB_ID;
	STcopy("attcollid", attname);
	if (info->upcase)
	    STcopy("ATTCOLLID", attname);
/* # line 10570 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiattribute set attoff=attoff +2, attfrml=attfrml -2, attid=at\
tid +1, attintl_id=attintl_id +1 where attrelid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&reltidx);
    IIwritio(0,(short *)0,1,32,0,(char *)"and lowercase(attname)='attf\
ree'");
    IIsyncup((char *)0,0);
  }
/* # line 10577 "upgradedb.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	   du_talk(DU_FAIL_EXIT, &upgr_cb, E_DU601A_NOUPDATE_SYSCAT, 4,
	       0, "iiattribute", 0, sqlca.sqlcode);
	   upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
/* # line 10583 "upgradedb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiattribute(attrelid, attrelidx, attid, attname, attoff, \
attfrml, attkdom, attflag, attdefid1, attdefid2, attintl_id, attver_ad\
ded, attver_dropped, attval_from, attfrmt, attfrmp, attver_altcol)valu\
es(");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&reltidx);
    IIwritio(0,(short *)0,1,32,0,(char *)", 19, ");
    IIputdomio((short *)0,1,32,0,attname);
    IIwritio(0,(short *)0,1,32,0,(char *)
", 78, 2, 0, 0, 3, 0, 19, 0, 0, 0, 30, 0, 0)");
    IIsyncup((char *)0,0);
  }
/* # line 10594 "upgradedb.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	   du_talk(DU_FAIL_EXIT, &upgr_cb, E_DU601A_NOUPDATE_SYSCAT, 4,
	       0, "iiattribute", 0, sqlca.sqlcode);
	   upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
	if (sqlca.sqlcode < 0)
	{
	   du_talk(DU_FAIL_EXIT, &upgr_cb, E_DU601A_NOUPDATE_SYSCAT, 4,
	       0, "iirelation", 0, sqlca.sqlcode);
	   upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
/* # line 10606 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iirelation set relatts=relatts +1 where reltid=");
    IIputdomio((short *)0,1,30,4,&reltid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&reltidx);
    IIsyncup((char *)0,0);
  }
/* # line 10609 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 10610 "upgradedb.sc" */	/* host code */
	/* Need to disconnect/reconnect, else server is confused about
	** what's in iiattribute (which is always held open while the DB
	** is open).
	*/
	if (upg_dbg_out)
	{
	    SIprintf("]==> Disconnect/reconnect to flush iiattribute change\n");
	}
/* # line 10618 "upgradedb.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&sess_id,11);
  }
/* # line 10619 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(sess_id);
    IIsqDisconnect();
  }
/* # line 10620 "upgradedb.sc" */	/* host code */
	upgr_cb.Connected = FALSE;
	if (iidbdbname == NULL)
	{
	    if (! open_target(CONNECT_EXCL) )
	    {
		SIprintf("Error re-opening database %s \n", upgr_cb.du_opendb);
	        upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	    }
	}
	else
	{
	    /* iidbdb, reconnect by hand here */
/* # line 10632 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DBDB_SESSIONID);
    IIsqUser(DU_DBA_DBDB);
    IIsqConnect(0,iidbdbname,(char *)"-A6",(char *)"-l",(char *)"+U",(char *)"\
+w",(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 10635 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
		upgr_exit(DU_FAIL_EXIT, E_DU600D_BAD_IIDBDB_RECONNECT);
	    IIlq_Protect(TRUE);
	}
	/* Ensure that the new attcollid column is zero */
/* # line 10640 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiattribute set attcollid=0");
    IIsyncup((char *)0,0);
  }
/* # line 10642 "upgradedb.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	   du_talk(DU_FAIL_EXIT, &upgr_cb, E_DU601A_NOUPDATE_SYSCAT, 4,
	       0, "iiattribute", 0, sqlca.sqlcode);
	   upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	}
    }
    /* Modify iiprotect to btree, get rid of iixprotect */
    duc_m4_protect();
/* # line 10651 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 10652 "upgradedb.sc" */	/* host code */
} /* order_critical_r302 */
/*{
** Name: star_20_std_cats() - rebuild the star standard catalog interface
**
** Description: Refresh the iidd_columns for iitables, iiphysical_tables and
**		iiindexes for new column information. 
**
**
** Inputs:
**      database name
**
** Outputs:
**      none
**
** History:
**      06-mar-1996 (nanpr01)
**         Written. 
*/
static VOID
star_20_std_cats(bool upcase)
{
  int DU_object_index;
  char *DU_ii_name, *DU_iidd_name, *DU_obj_type, *DU_loc_type;
  i4 duc_4row_cnt;
    for (DU_object_index = 0;
     STcompare((char *)(DU_upg20_link_object_map[DU_object_index].ii_name), "");
     DU_object_index++)
    {
	if (upcase)
	{
	    DU_ii_name = DU_upg20_link_object_map[DU_object_index].ii_name;
	    CVupper(DU_ii_name);
	    DU_iidd_name = DU_upg20_link_object_map[DU_object_index].iidd_name;
	    CVupper(DU_iidd_name);
	}
	else
	{
	    DU_ii_name = DU_upg20_link_object_map[DU_object_index].ii_name;
	    DU_iidd_name = DU_upg20_link_object_map[DU_object_index].iidd_name;
	}
        DU_obj_type = DU_upg20_link_object_map[DU_object_index].obj_type;
        DU_loc_type = DU_upg20_link_object_map[DU_object_index].loc_type;
        /* Initialize iidd_tables and substitute iidd_name into ii_name */
/* # line 11180 "upgradedb.qsc" */	/* delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIexExec(1,(char *)"upgradedb1",7658,21220);
      IIputdomio((short *)0,1,32,0,DU_ii_name);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIexDefine(1,(char *)"upgradedb1",7658,21220);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete iidd_columns where iidd_columns.table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,DU_ii_name);
        IIexDefine(0,(char *)"upgradedb1",7658,21220);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 11183 "upgradedb.qsc" */	/* append */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIexExec(1,(char *)"upgradedb2",7659,12234);
      IIputdomio((short *)0,1,32,0,DU_ii_name);
      IIputdomio((short *)0,1,32,0,DU_iidd_name);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIexDefine(1,(char *)"upgradedb2",7659,12234);
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
        IIexDefine(0,(char *)"upgradedb2",7659,12234);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 11200 "upgradedb.qsc" */	/* inquire_equel */
  {
    IIeqiqio((short *)0,1,30,4,&duc_4row_cnt,(char *)"rowcount");
  }
/* # line 11201 "upgradedb.qsc" */	/* host code */
        if (duc_4row_cnt == 0)
        {
            /* Bad append to table, %0c, in database, %1c. */
            du_talk(DU_MODESENS, &upgr_cb, E_DU2417_NODROP_SYSCAT, 4,
                    0, "iidd_columns", 0, upgr_cb.Dbname_ptr->dbname);
            upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
        }
	/* Commit Transaction */
/* # line 11210 "upgradedb.qsc" */	/* end transaction */
  {
    IIxact(1);
  }
/* # line 11212 "upgradedb.qsc" */	/* host code */
	/* Start another transaction */
/* # line 11213 "upgradedb.qsc" */	/* begin transaction */
  {
    IIxact(0);
  }
/* # line 11215 "upgradedb.qsc" */	/* host code */
    }
}
/*{
** Name: new_for_20_stdcats	-  build standard catalogs new for 2.0
**
** Description:
**
**	This routine builds the standard catalogs new for 2.0.  
**	It creates the following standard catalogs:
**
**
**
** Inputs:
**	database name
**
** History:
**	21-may-1996 (shero03)
**	    Created so iirange gets created
**	25-nov-1997 (nanpr01)
**	    bug 87315 : upgradedb is not creating iirange sys cat.
*/
static VOID
new_for_20_stdcats(char *dbname, bool upcase)
{
    if (!rel_exists("iirange"))
    {
	du_talk(DU_MODESENS, &upgr_cb, I_DU00D3_IIDEFAULT_CRE, 0);
	duc_iirange();	/* builds and populates iirange */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	{
	  /* error: create table didn't work */
	  du_talk(DU_MODESENS, &upgr_cb, E_DU2418_NOCREATE_SYSCAT, 4,
	    0, "iirange", 0, dbname);
	  upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	  return;
	}
	duc_modify_iirange();	/* modify to desired structure */
	if (upgr_cb.Dbname_ptr->errors == TRUE)
	{
	  /* warning: modify didn't work */
	  du_talk(DU_MODESENS, &upgr_cb, W_DU1869_NOMODIFY, 4,
	    0, ERx("iirange"), 0, dbname);
	   upgr_exit(DU_FAIL_EXIT, E_DU241A_SYSCAT_UPGRADE_FAILED);
	  return;
	}
	/* ensure the changes take effect */
/* # line 10818 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 10819 "upgradedb.sc" */	/* host code */
    }
}
/*{
** Name: cv_20_hashdate_tables - reformat hased date columns.
**
** Description:
**	Looks for tables containing date columns with hash indexes defined
**	on them. Such columns are the target of a subsequent update
**	which "normalizes" the date representation, then repositions
**	the row in the index. This is required by the fix to bugs 66329 and
**	74033. 
**
** Inputs:
**	database name (though call from cv_database assures we're already
**	connected).
**
** Outputs:
**	none
**
** History:
**	28-jan-1997 (inkdo01)
**	    Written.
**	30-oct-1998 (kitch01)
**		Bug 92451. Add 0 days instead of 0 hours. This will still allow
**		the date to be normalized but without appending 00:00:00 to
**		date only date values.
*/
static VOID
cv_20_hashdate_tables(dbname)
char	*dbname;
{
  char tablename[DB_MAXNAME+1];
  char attname[DB_MAXNAME+1];
  char dynstmt[500];
    int     i, k, savecode;      
    char    oldtable[32] = "                                ";
    static char    string1[ 7] = "update ";
    static char    string2[ 5] = " set ";
    static char    string3[ 3] = " = ";
    static char    string4[10] = "+'0 days'";
    static char    string5[ 2] = ", ";
    i = 1; k = 0;
    strncpy(&dynstmt[0], string1, 7);	/* glue in background pieces */
    strncpy(&dynstmt[39], string2, 5);
    strncpy(&dynstmt[76], string3, 3);
    strncpy(&dynstmt[111], string4, 10);
    /* get table, column pairs for all date columns with hash indexes
    ** defined on them. */
/* # line 10881 "upgradedb.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"dupdcurs",7661,18902);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select attname, relid from \"$ingres\".iiattribute a, \"$ingres\".iir\
elation b where a.attrelid=b.reltid and a.attrelidx=b.reltidx and a.at\
trelidx=0 and exists(select * from \"$ingres\".iiattribute a1, \"$ingr\
es\".iirelation b1 where a1.attkdom>0 and a1.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"attrelid=b1.reltid and a1.attfrmt in( -3, 3)and a1.attrelidx=b1.relti\
dx and b1.relspec=7 and a.attname=a1.attname and a.attrelid=a1.attreli\
d)order by relid");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"dupdcurs",7661,18902);
  }
/* # line 10882 "upgradedb.sc" */	/* host code */
    /* fetch loop retrieves all qualifying table, column pairs and
    ** whacks 'em into the corresponding update syntax. */
    while(sqlca.sqlcode == 0)
    {
/* # line 10886 "upgradedb.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"dupdcurs",7661,18902,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,DB_MAXNAME + 1-1,attname);
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,tablename);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 10887 "upgradedb.sc" */	/* host code */
	savecode = sqlca.sqlcode;
        if (strncmp(tablename, oldtable, 32) != 0 || k >= 400
		|| sqlca.sqlcode > 0)
	{
	    if (i == 0)
	    {
		dynstmt[k] = 0;	/* terminate the string */
		if (upg_dbg_out)
		{
		    SIprintf("]==> Hashed date column update:\n %s \n",
			dynstmt);
		}
		/* update <table> set <col1> = <col1> +'0 hours', 
		**  <col2> = <col2> +'0 hours', ... */
		/* Amended to be +'0 days'. Bug 92451*/
/* # line 10902 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(dynstmt);
    IIsyncup((char *)0,0);
  }
/* # line 10903 "upgradedb.sc" */	/* host code */
		sqlca.sqlcode = savecode;	/* restore for loop ctrl */
	    }
	    if (savecode >0) break;	/* all done */
	    i = 0;		/* show not 1st pass */
	    strncpy(oldtable, tablename, 32);	/* remember curr name */
	    strncpy(&dynstmt[7], tablename, 32);/*start update stmt build */
	    strncpy(&dynstmt[44], attname, 32);
	    strncpy(&dynstmt[79], attname, 32);
	    k = 121;
	    continue;		/* loop for next row */
	}
	/* Another column to current table */
	strncpy(&dynstmt[k], string5, 2);		/* , */
	strncpy(&dynstmt[k+2], attname, 32);	/* <column> */
	strncpy(&dynstmt[k+34], string3, 3);	/* = */
	strncpy(&dynstmt[k+37], attname, 32);	/* <column> */
	strncpy(&dynstmt[k+69], string4, 10);	/* +'0 hours' amended to be +'0 days'*/
	k += 79;
    }
/* # line 10922 "upgradedb.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"dupdcurs",7661,18902);
  }
/* # line 10923 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 10924 "upgradedb.sc" */	/* host code */
}   
/*
** Name:  changed_cats_25 - Update catalog data/format that changes for 2.5
**
** Description:
**	This function is designed to make any updates to catalogs that 
**	changed in 2.5, it is executed after the 6.5 (OpenIngres) and 2.0 
**	upgrade operations, and includes changes that happened between
**	2.0 and 2.5. Changes are:
**
**	    Blob extension tables now allow duplicate records to prevent 
**	    an unnecessary duplicate search when data is added
**
**	    iirule is remodified to add an index for rule lookup by name.
**
**	The updates herein depend on a functioning "create procedure"
**	mechanism, so they're done after the initial order critical catalog
**	updates.
**
** Inputs:
**	dbname		name of database
**
** Outputs:
**	None
**
** Returns:
**	None
**
** History:
**	12-may-99 (stephenb)
**	    Created
**	20-may-1999 (nanpr01)
**	    SIR 89267 - Need additional index to check the existence
**	    of rule by its owner and name.
**      03-aug-1999 (stial01)
**          create/modify iiextended_relation if it doesn't exist
**          Call iiQEF_convert_table for all tables having (blob) extensions
**	16-aug-99 (inkdo01)
**	    Added call to cv_25_iiintegrity() to add referential action
**	    columns.
**	29-aug-2000 (somsa01)
**	    Changed c3 cursor to 'c4', as c3 is already in use in this file.
**	3-nov-00 (inkdo01)
**	    Drop/recreate iistats, iihistograms views to support composite
**	    histograms.
**     28-jan-04 (chash01) interestingly, some RMS
**         gateway registered table/index has relspec=11
**         (btree) thus we has to eliminate them for update. 
**         test relstat for TCB_GATEWAY bit set, select those
**         that do not have this bit set.
**     09-feb-04 (chash01)
**         Further testing found that the problem with RMS Gateway imported
**         table update is the result of not being able to tell that TIDP 
**         is also part of index key (by Ingres definition).  Correction
**         is made in gwrms.c and the last change is back out.
**	30-Aug-2004 (schka24)
**	    iiintegrity done in bootstrap for r3.
*/
static VOID	
changed_cats_25(char		*dbname,
		DBNAME_NODE	*db_star_name,
		bool		upcase)
{
  i4 relstat;
  u_i4 reltid;
  u_i4 reltidx;
  i4 return_status;
  char relname[DB_MAXNAME + 1];
    /* allow duplicate rows in extension tables */
    if (rel_exists("iiextended_relation"))
    {
/* # line 11001 "upgradedb.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"relcurs",7662,27846);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select r.relstat from iirelation r, iiextended_relation e where e.eta\
b_extension=r.reltid and r.reltidx=0");
    IIcsQuery((char *)"relcurs",7662,27846);
  }
/* # line 11003 "upgradedb.sc" */	/* host code */
	for(;;)
	{
/* # line 11005 "upgradedb.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"relcurs",7662,27846,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&relstat);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode == 100) 
      goto close_relcurs;
  }
/* # line 11006 "upgradedb.sc" */	/* host code */
	    relstat |= DU_RELSTAT_DUPLICATES;
/* # line 11007 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&relstat);
    IIcsERplace((char *)"relcurs",7662,27846);
    if (sqlca.sqlcode == 100) 
      goto close_relcurs;
  }
/* # line 11010 "upgradedb.sc" */	/* host code */
	}
        close_relcurs:
/* # line 11012 "upgradedb.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"relcurs",7662,27846);
    if (sqlca.sqlcode == 100) 
      goto close_relcurs;
  }
/* # line 11013 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 100) 
      goto close_relcurs;
  }
/* # line 11015 "upgradedb.sc" */	/* host code */
    }
    else
    {
	/* Create/modify iiextended_relation */
	duc_iietab();
	duc_modify_iietab();
    }
    /*
    ** Make sure etabs exist for all peripheral attributes
    ** First add internal db procedure: iiQEF_convert_table
    */
    add_iiqef_convert_table();
    /*
    ** 
    ** Select all tables having extension table
    ** Select all tables that are BTREE or RTREE
    ** 28-jan-04 (chash01) interestingly, almost all RMS
    ** gateway registered table/index has relspec=11
    ** (btree) thus we has to eliminate them for update.
    ** 09-feb-04 (chash01) above change is backed out.
    ** fix now is in gwrms.c  
    */
/* # line 11040 "upgradedb.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c4",7663,11477);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select relid, reltid, reltidx from iirelation where mod(relstat2/2, 2\
)=1 or relspec=11 or relspec=13");
    IIcsQuery((char *)"c4",7663,11477);
  }
/* # line 11042 "upgradedb.sc" */	/* host code */
    for (;;)
    {
/* # line 11044 "upgradedb.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c4",7663,11477,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,DB_MAXNAME + 1-1,relname);
      IIcsGetio((short *)0,1,30,4,&reltid);
      IIcsGetio((short *)0,1,30,4,&reltidx);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode == 100) 
      goto closec4;
  }
/* # line 11045 "upgradedb.sc" */	/* host code */
	du_talk(DU_MODESENS, &upgr_cb, I_DU00FE_VERIFYING_TBL, 2, 0, relname);
/* # line 11046 "upgradedb.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"iiQEF_convert_table");
    IILQprvProcValio((char *)"table_id",0,(short *)0,1,30,4,&reltid);
    IILQprvProcValio((char *)"index_id",0,(short *)0,1,30,4,&reltidx);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
    IIeqiqio((short *)0,1,30,4,&return_status,(char *)"iiret");
  }
/* # line 11049 "upgradedb.sc" */	/* host code */
	if (sqlca.sqlcode < 0 || return_status != 0)
	{
	    SIprintf("]==> error executing iiQEF_convert_table on %s (%d,%d)(%d,%d)\n",
		    relname, reltid, reltidx, sqlca.sqlcode, return_status);
	    upgr_exit(DU_FAIL_EXIT, 0);
	}
    }
    closec4:
/* # line 11057 "upgradedb.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c4",7663,11477);
    if (sqlca.sqlcode == 100) 
      goto closec4;
  }
/* # line 11058 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode == 100) 
      goto closec4;
  }
/* # line 11060 "upgradedb.sc" */	/* host code */
    /* Create the new secondary index to avoid locking problems */
    duc_modify_iirule();
}
/*
** Name:  chmod_data_600 - Chmod data files to 600
**
** Description:
**		For UNIX file systems, change file to permission to 
**		read/write for owner.  This removes the executable bit
**		that has been set in previous releases.
**
** Inputs:
**	None
**
** Outputs:
**	None
**
** Returns:
**	None
**
** History:
**	27-may-99 (hanch04)
**	    Created
**	23-May-2001 (jenjo02)
**	    Ignore RAW locations.
**	10-Jul-2003 (schka70)
**	    "chmod 0600 *" is no good for db's with lots of tables.
**	    Use xargs instead.  Also, translate locations names properly.
**	    Use symbolic length constants!
*/
static VOID	
chmod_data_600()
{
#ifdef UNIX
  char locarea[ DB_AREA_MAX + 1 ];
  i4 stat;
  char chmoddbname[ DB_MAXNAME + 1];
    /* Below needs enough for "ls -1 loc-path | xargs chmod 600" */
    char dataloc_buf[ MAX_LOC + 80 + 1 ];
    CL_ERR_DESC		err_code;
    LOCATION		redirectloc;
    char		redirectbuf[MAX_LOC];
    LOCATION locpath;			/* Actual path struct for locname */
    STATUS		lo_status;
    STcopy("/dev/null", redirectbuf);
    LOfroms( FILENAME & PATH, redirectbuf, &redirectloc );
    /*Load table */
/* # line 11109 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select l.location_area, database_name, e.status from iiextend_info e,\
 iilocation_info l where e.status>0 and e.location_name=l.location_nam\
e order by e.location_name");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE12;
IIrtB12:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_AREA_MAX + 1-1,locarea);
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,chmoddbname);
      IIgetdomio((short *)0,1,30,4,&stat);
      if (IIerrtest() != 0) goto IIrtB12;
/* # line 11116 "upgradedb.sc" */	/* host code */
  {
        if ( (stat & (DU_EXT_DATA | DU_EXT_RAW)) == DU_EXT_DATA )
	{
	    STcopy(ERx("cd "), dataloc_buf);
	    /* Translate the data location that's in locarea */
	    lo_status = LOingpath(locarea, chmoddbname, LO_DB, &locpath);
	    if (lo_status == OK) {
		STcat(dataloc_buf, locpath.string);
	 	STcat(dataloc_buf, ERx("; ls -1 | xargs chmod 0600 "));
		PCcmdline((LOCATION *) NULL, dataloc_buf, PC_WAIT, 
		      &redirectloc, &err_code);
	    }
	}
  }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE12:;
  }
#endif /* UNIX */
/* # line 11132 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 11133 "upgradedb.sc" */	/* host code */
}
/*{
** Name: fix_qry_trees() - Rebuild views, quel permits, rules, integrities
**			   Optionally rebuild ALL permits
**                         Optionally add copy_into, copy_from permission
**
** Description:
**	This routine rebuilds all query trees.
**
**	Unloaddb is used to do most of the work.  Flags are passed to unloaddb
**	so that it generates drop and recreate of views, rules, quel
**	integrities, and optionally permits.  (Permits are rebuilt only if
**	the database is 6.4 or earlier, or if QUEL permits with trees
**	exist.  6.4 permits are redone not because of query trees, but
**	because ALL permits are stored differently post-6.4.)
**
**	Unloaddb can't conveniently handle system generated rules (for
**	constraints), so they are dealt with separately (fix_sysgen_rules
**	and fix_constraints).  It also depends on valid standard
**	catalog views, so those are reconstructed before calling unloaddb.
**
**	Note: if star_db is TRUE, we're connected to the Star database
**	(ddb) only, and must use conn_dbname instead of open_target() for
**	reconnects.  If star_db is FALSE, we should have connections to
**	both iidbdb and the target database open.  (The target might be a
**	star CDB in this case, i.e. star_db <> Dbname_ptr->star_flag.)
**
** Inputs:
**	dbname		name of current database
**      conn_dbname	name to use for connect
**	star_db		TRUE if connected to dbname/star
**      level		level of current database
**
** if level < V65ES:
** 1) Redefine 'GRANT ALL' permits:
**    Change procedure 'GRANT ALL' permits to 'GRANT EXECUTE'
**    Change event 'GRANT ALL' permits to 'GRANT REGISTER, RAISE'
**
** 2) Add copy_into, copy_from GRANTS
**    Change grant insert -> grant insert, copy_into
**    Change grant select -> grant select, copy_from
**
** Outputs:
**      upgr_cb.Dbname_ptr->tree_error
**
** History:
**    ??/??/?? - initial version
**    11-apr-2003 devjo01 (b110050)
**	Capture and surpress "unloaddb" output so user does not see
**	instructions for unloading and reloading his database, which
**	are misleading in this context.
**    31-Oct-2003 (wanfr01)
**          Bug 110858, INGSRV 2493
**          Recreate select permits to public on system catalog views
**          after they are dropped and recreated.
**	10-Sep-2004 (schka24)
**	    More work on getting grants right.  Always redo catalog
**	    grants (old db's have them screwed up from old conversions).
**	    Reissue 6.4 all-to-public and select-to-public so that they're
**	    in new form.  Fix object counter check for uppercase DB's.
**	    Also, rework cross-owner referential constraint fixup, see
**	    fix_ref_constraints.
**	24-Sep-2004 (schka24)
**	    Get Star upgrades working; fix 1.2 upgrades with constraints.
**	27-oct-2004 (somsa01)
**	    Updated runs of unloaddb to be executed via PCcmdline(), so
**	    that we have better control of "-u" passing.
**	13-jul-2005 (abbjo03)
**	    Back out change of 27-oct-2004 as it causes errors when unloaddb
**	    is run on VMS.    
*/
static  char    unloaddb_args[] = ERx(
  "database = %S, user = %S, directory = %S, viewonly = %S, procedureonly = %S, rulesonly = %S, dropincluded = %S, integonly = %S");
/* If ANY QUEL permits in db.... drop and recreate ALL permits */
static  char    unloaddb_args_permits[] = ERx(
  "database = %S, user = %S, directory = %S, viewonly = %S, procedureonly = %S, rulesonly = %S, dropincluded = %S, integonly = %S, permitsonly = %S");
static  char    unloaddb_star_args[] = ERx(
  "database = %S, user = %S, directory = %S, viewonly = %S, dropincluded = %S");
static  char    Fe_unloaddb[] = ERx("unloaddb");  
static  char    Fe_sql[] = ERx("sql");  
/*
** All of this was copied from front!misc!hdr xf.h 
** The -A6 flag was added so the server knows it is called from upgradedb
*/
# ifdef	VMS /* quote the flags to prevent lowercasing */
#define CMD_SQLSYMDEF  ERx("sql :== $II_SYSTEM:[ingres.bin]tm.exe -qSQL")
#define CMDLINE	ERx("sql +w -s -f4F79.38 -f8F79.38 -u%s \"+U\" \"-A6\" %s <%s >%s\n")
#endif
# ifdef	UNIX
#define CMDLINE	ERx("sql +w -s -f4F79.38 -f8F79.38 -u'%s' +U -A6 %s < %s > %s\n")
# endif
# ifdef	hp9_mpe
#define CMDLINE	ERx("sql +w -s -f4F79.38 -f8F79.38 -u%s +U -A6 %s < %s > %s\n")
# endif
# if defined (MSDOS) || defined (NT_GENERIC)
#define CMDLINE	ERx("sql +w -s -f4F79.38 -f8F79.38 -u%s +U -A6 %s < %s > %s") 
# endif                        
static VOID 
fix_qry_trees(
char *dbname, char *conn_dbname, bool star_db, i4 level)
{
    DU_ERROR		errcb;
    STATUS		cl_stat;
    char		ut_args[LO_NM_LEN+1 + 500];
    CL_ERR_DESC		err_code;
    LOCATION		copyin_loc;
    char		copyin_loc_buf[MAX_LOC];
    LOCATION		grant_fix_loc;
    char		grant_fix_loc_buf[MAX_LOC];
    char		grant_fix_file[DI_FILENAME_MAX + 1];
    LOCATION		grant_out_loc;
    char		grant_out_loc_buf[MAX_LOC];
    char		grant_out_file[DI_FILENAME_MAX + 1];
    LOCATION		ref_fix_loc;
    char		ref_fix_loc_buf[MAX_LOC];
    char		ref_fix_file[DI_FILENAME_MAX + 1];
    LOCATION		ref_out_loc;
    char		ref_out_loc_buf[MAX_LOC];
    char		ref_out_file[DI_FILENAME_MAX + 1];
    LOCATION		sql_input_loc;
    char		sql_input_loc_buf[MAX_LOC];
    char		sql_input_file[DI_FILENAME_MAX + 1];
    LOCATION		sql_output_loc;
    char		sql_output_loc_buf[MAX_LOC];
    char		sql_output_file[DI_FILENAME_MAX + 1];
    char		sql_cmd[(LO_NM_LEN * 3) + 500];
    char		*dirname;
    i4			n;
    FILE		*fp;
    bool		db_open;
    bool		grant_65es = FALSE;
    bool		fix_iiud = TRUE; /* remove iiud.scr from copy.in */
  char *sql_conn_dbname;
  char start_dt[26];
  i4 quel_perm_cnt;
  char tbuf[512];
  i4 pre_view_cnt, pre_intg_cnt, pre_proc_cnt;
  i4 post_view_cnt, post_intg_cnt, post_proc_cnt;
  i4 old_view_cnt, old_intg_cnt, old_proc_cnt;
  i4 pre_cons_cnt, post_cons_cnt;
  char *iidbdbname;
    bool		did_unload = FALSE;
    bool		grant_fixes = FALSE;
    bool		cons_fixes = FALSE;
    i4			new_cnt;
    if (upg_dbg_out)
	SIprintf("Fixing query trees for database %s connect dbname %s\n", 
	dbname, conn_dbname);
     sql_conn_dbname = conn_dbname;
    if (setauth("$ingres") != OK)
	SIprintf("Warning: SET SESSION AUTHORIZATION $ingres failed\n");
    if (level < V65ES_LEVEL)
	grant_65es = TRUE;
/* # line 11288 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select date('now')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,25,start_dt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 11289 "upgradedb.sc" */	/* host code */
    if (star_db)
    {
	/* Re-register distributed views on standard catalog views */
	DU_link_object_entry	*map = &DU_link_object_map[0];
	i4			object_index;
	for (object_index = 0;
	    STcompare((char *)(map[object_index].ii_name), "");
		object_index++)
	{
	    if (map[object_index].loc_type[0] != 'V')
		continue;
	    STprintf(tbuf, "remove %s", map[object_index].ii_name);
/* # line 11301 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(tbuf);
    IIsyncup((char *)0,0);
  }
/* # line 11302 "upgradedb.sc" */	/* host code */
	    check_sql(sqlca.sqlcode);
	    /* Ignore remove errors */
	    STprintf(tbuf, "register view %s as link from %s",
		map[object_index].ii_name, map[object_index].iidd_name);
/* # line 11306 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(tbuf);
    IIsyncup((char *)0,0);
  }
/* # line 11307 "upgradedb.sc" */	/* host code */
	    check_sql(sqlca.sqlcode);
	    if (sqlca.sqlcode < 0)
		SIprintf("SQL error %d statement: %s\n", sqlca.sqlcode, tbuf);
/* # line 11310 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 11311 "upgradedb.sc" */	/* host code */
	}
	pre_cons_cnt = 0;		/* Meaningless in a star DB */
    }
    else
    {
	/* Local database, drop all standard catalog views */
	/* Probably dropped already if normal upgrade path, but make sure;
	** not dropped if Star CDB, drop now (again!).
	*/
	/* Count constraints now before we drop any, other things counted
	** later.  Note: no std cat views can be used here.
	*/
	pre_cons_cnt = -1;
/* # line 11324 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiintegrity where consflags<>0");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&pre_cons_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 11327 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 11328 "upgradedb.sc" */	/* host code */
	du_talk(DU_MODESENS, &upgr_cb, I_DU0102_DROPPING_STDCAT_VIEWS, 0);
	duc_drop_stdcat_views(dbname, upgr_cb.Dbname_ptr->star_flag); 
	/* There's been various issues with system catalog grants
	** in previous conversions.  Take the brute force approach and
	** regrant all the system catalogs (base tables only).
	** This also regrants any tables that were redone during upgrade.
	** Stdcat views are re-granted below, after re-create.
	*/
	if (level < V30_LEVEL)
	{
	    fix_syscat_grants(dbname);
	}
	/* For conversions from 6.4 or earlier, fix ALL/SELECT to PUBLIC
	** grants to look like the later style.  Do this now while
	** the std cats are out of the way.
	*/
	if (level < V65_LEVEL)
	{
	    /*
	    ** Build unique grant-fix sql filename
	    ** Just in case this isn't the first time running upgradedb...
	    ** We don't want to step on previous scripts (no real
	    ** reason why not, but seems like a good idea!)
	    */
	    for ( n = 1;  ; n++)
	    {
		/* Build location for grant fixing: dbname.gNN */
		STprintf(grant_fix_file, "%s.g%02d", dbname, n);
		LOcopy(&upgr_cb.log_dir_loc, grant_fix_loc_buf, &grant_fix_loc);
		cl_stat = LOfstfile ( grant_fix_file, &grant_fix_loc);
		if (n == 99 || (cl_stat = SIopen(&grant_fix_loc, "r", &fp)) != OK )
		    break;
		SIclose(fp);
	    }
	    /* Build grant-sql output filename too */
	    STprintf(grant_out_file, "%s.go%02d", dbname, n);
	    LOcopy(&upgr_cb.log_dir_loc, grant_out_loc_buf, &grant_out_loc);
	    cl_stat = LOfstfile ( grant_out_file, &grant_out_loc);
	    /* Write script for -to-public grant fixup */
	    grant_fixes = fix_64_grants(dbname,&grant_fix_loc,grant_fix_loc_buf);
	}
	/* Drop all internal procedures */
	duc_drop_internal_procs(dbname, upgr_cb.Dbname_ptr->star_flag);
	/* Re-create all stdcat views.
	** Re-create Star views if this is a Star CDB.
	*/
	du_talk(DU_MODESENS, &upgr_cb, I_DU0103_RECREATING_STDCAT_VIEWS, 0);
	duc_create_stdcat_views(dbname, upgr_cb.Dbname_ptr->upcase);
	if (upgr_cb.Dbname_ptr->star_flag)
	    duc_create_star_views(dbname, upgr_cb.Dbname_ptr->upcase);
	if (level >= V65_LEVEL)
	{
	    /*
	    ** Post-64 database might have constraints.
	    ** Build unique constraint-fix sql filename.  (rNN because
	    ** originally this was just for cross-user referential
	    ** constraints, before I discovered the 1.2 situation.)
	    */
	    for ( n = 1;  ; n++)
	    {
		/* Build location for ref constraint fixing: dbname.rNN */
		STprintf(ref_fix_file, "%s.r%02d", dbname, n);
		LOcopy(&upgr_cb.log_dir_loc, ref_fix_loc_buf, &ref_fix_loc);
		cl_stat = LOfstfile ( ref_fix_file, &ref_fix_loc);
		if (n == 99 || (cl_stat = SIopen(&ref_fix_loc, "r", &fp)) != OK )
		    break;
		SIclose(fp);
	    }
	    /* Build ref-sql output filename too */
	    STprintf(ref_out_file, "%s.ro%02d", dbname, n);
	    LOcopy(&upgr_cb.log_dir_loc, ref_out_loc_buf, &ref_out_loc);
	    cl_stat = LOfstfile ( ref_out_file, &ref_out_loc);
	    /* Drop cross-user ref constraints, or all 1.2 ref and check
	    ** constraints, write script to recreate
	    */
	    cons_fixes = fix_constraints(dbname,level,&ref_fix_loc,ref_fix_loc_buf);
	}
	/*
	** See if this db has any QUEL permits (which have trees)
	** No need to do this if we're dumping pre-6.5es permits anyway.
	**
	** If so, drop and recreate all permissions/grants for ALL tables
	** I could've tried to call copydb for the tables having quel permits..
	** but I am not sure if I can do this correctly for tables with 
	** delimited ids...
	*/
	if (!grant_65es)
	{
/* # line 11416 "upgradedb.sc" */	/* declare */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"declare global temporary table session.quelperms as select distinct o\
bject_name, object_owner, permit_number from iipermits where text_sequ\
ence='1' and text_segment not like 'grant%' and text_segment not like \
'create%' on commit preserve rows with ");
    IIwritio(0,(short *)0,1,32,0,(char *)"NORECOVERY");
    IIsyncup((char *)0,0);
  }
/* # line 11422 "upgradedb.sc" */	/* host code */
	    quel_perm_cnt = 0;
/* # line 11423 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from session.quelperms");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&quel_perm_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 11424 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 11425 "upgradedb.sc" */	/* host code */
	    if (quel_perm_cnt > 0)
		grant_65es = TRUE;
	}
	/* System catalog views should have select to public WGO */
	duc_grant_stdcat_views(dbname, upgr_cb.Dbname_ptr->upcase);
    }
    /* Get more pre-counts now that we have std catalog views */
    pre_view_cnt = pre_intg_cnt = pre_proc_cnt = -1;
/* # line 11433 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iitables where table_type='V' and lowercase(table\
_name)not like 'ii%'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&pre_view_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 11435 "upgradedb.sc" */	/* host code */
    if (! star_db)
    {
/* # line 11437 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiintegrities");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&pre_intg_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 11438 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiprocedures where text_sequence=1 and lowercase(\
procedure_name)not like 'ii%' and system_use<>'G'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&pre_proc_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 11442 "upgradedb.sc" */	/* host code */
    }
/* # line 11443 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 11444 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 11445 "upgradedb.sc" */	/* host code */
    if (upg_dbg_out)
    {
	SIprintf("]==> (h) disconnect\n");
    }
    upgr_cb.Connected = FALSE;
    /*
    ** Need to make sure the (exclusive) connection is ended 
    ** unloaddb below will fail if the exclusive connection hasn't ended
    ** Try to connect wait - without requesting db lock - then disconnect again
    */
    if (STcompare(dbname, DU_DBDBNAME) == 0)
    {
	if (upg_dbg_out)
	{
	    SIprintf("]==> [9] connect to iidbdb session %d id %s options = +w\n",
		    DBDB_SESSIONID, DU_DBA_DBDB);
	}
	iidbdbname = conn_dbname;
	/* If we're doing iidbdb, we must be ingres, this will work */
/* # line 11464 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DBDB_SESSIONID);
    IIsqUser(DU_DBA_DBDB);
    IIsqConnect(0,iidbdbname,(char *)"+w",(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 11466 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 11467 "upgradedb.sc" */	/* host code */
	upgr_cb.Connected = FALSE;
	if (upg_dbg_out)
	{
	    SIprintf("]==> (i) disconnect\n");
	}
    }
    else if (star_db)
    {
	if (upg_dbg_out)
	{
	    SIprintf("]==> [9] connect to %s session %d id %s options = +w\n",
		    conn_dbname, STAR_SESSIONID, DU_DBA_DBDB);
	}
/* # line 11480 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(STAR_SESSIONID);
    IIsqUser(DU_DBA_DBDB);
    IIsqConnect(0,sql_conn_dbname,(char *)"+w",(char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 11482 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 11483 "upgradedb.sc" */	/* host code */
	if (upg_dbg_out)
	    SIprintf("]==> (j) disconnect\n");
    }
    else
    {
	if (open_target(CONNECT_SHARE))
	{
/* # line 11490 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DB_SESSIONID),(void *)IILQint(
    DB_SESSIONID));
  }
/* # line 11491 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 11492 "upgradedb.sc" */	/* host code */
	    upgr_cb.Connected = FALSE;
	    if (upg_dbg_out)
	    {
		SIprintf("]==> (j) disconnect\n");
	    }
	}
    }
    /*
    ** call unloaddb to generate script to create views, procedures, rules 
    ** and maybe permissions
    ** If upgr_cb.Dbname_ptr->star_flag, connect to the local db
    */
    du_talk(DU_MODESENS, &upgr_cb, I_DU0104_UNLOADING_TREES, 0);
    LOtos(&upgr_cb.log_dir_loc, &dirname);
    if (upg_dbg_out)
	SIprintf("upgradedb calling UNLOADDB db %s directory = %s\n",
		dbname, dirname);
    /* Build location for copy.in file that unloaddb will create */
    LOcopy(&upgr_cb.log_dir_loc, copyin_loc_buf, &copyin_loc);
    cl_stat = LOfstfile ( "copy.in", &copyin_loc);
    /*
    ** Build unique sql input and output file names 
    ** Just in case this isn't the first time running upgradedb...
    ** We don't want to step on previous unloaddb scripts
    */
    for ( n = 1;  ; n++)
    {
	/* Build location for backup of copy.in -> dbname.iNN */
	/* Filename may be DB_MAXNAME.iNN, must be < DI_FILENAME_NAME */
	STprintf(sql_input_file, "%s.i%c%c", dbname, 
	    ((n / 10) % 10) + '0', (n % 10) + '0');
	LOcopy(&upgr_cb.log_dir_loc, sql_input_loc_buf, &sql_input_loc);
	cl_stat = LOfstfile ( sql_input_file, &sql_input_loc);
	if ( (cl_stat = SIopen(&sql_input_loc, "r", &fp)) == OK )
	{
	    SIclose(fp);
	    continue;
	}
	/* Build location for sql output -> dbname.oNN */
	/* Filename may be DB_MAXNAME.oNN, must be < DI_FILENAME_NAME */
	STprintf(sql_output_file, "%s.o%c%c", dbname, 
	    ((n / 10) % 10) + '0', (n % 10) + '0');
	LOcopy(&upgr_cb.log_dir_loc, sql_output_loc_buf, &sql_output_loc);
	cl_stat = LOfstfile ( sql_output_file, &sql_output_loc);
	if ( (cl_stat = SIopen(&sql_output_loc, "r", &fp)) == OK )
	{
	    SIclose(fp);
	    continue;
	}
	/* Found an unused input/output file name */
	break;
    }
    /* Use "dbname.oNN" to capture unwanted "unloaddb" output. */
    if (star_db)
    {
	STcopy (unloaddb_star_args, ut_args);
	cl_stat = UTexe(UT_WAIT|UT_STRICT, &sql_output_loc, NULL,
	    NULL, Fe_unloaddb, &err_code, ut_args, 5,
	    conn_dbname, upgr_cb.Dbname_ptr->dba, dirname, Empty_str, Empty_str);
    }
    else if (grant_65es)
    {
	STcopy (unloaddb_args_permits, ut_args);
	cl_stat = UTexe(UT_WAIT|UT_STRICT, &sql_output_loc, NULL,
	    NULL, Fe_unloaddb, &err_code, ut_args, 9,
	    conn_dbname, upgr_cb.Dbname_ptr->dba, dirname, 
	    Empty_str, Empty_str, Empty_str, Empty_str, Empty_str, Empty_str);
    }
    else
    {
	STcopy (unloaddb_args, ut_args);
	cl_stat = UTexe(UT_WAIT|UT_STRICT, &sql_output_loc, NULL,
	    NULL, Fe_unloaddb, &err_code, ut_args, 8,
	    conn_dbname, upgr_cb.Dbname_ptr->dba, dirname, 
	    Empty_str, Empty_str, Empty_str, Empty_str, Empty_str);
    }
    if (upg_dbg_out)
	SIprintf("UTARGS: %s (%s %s %s) status = %d\n",
	    ut_args, conn_dbname, upgr_cb.Dbname_ptr->dba, dirname, cl_stat);
    if (cl_stat != OK)
    {
	/*
	** Save CL err info from UTexe so "du_error" can see it,
	** and don't truncate "dbname.oNN".
	*/
	STRUCT_ASSIGN_MACRO (err_code, errcb.du_clsyserr);
	du_talk (DU_ALWAYS, &upgr_cb, W_DU186C_UPGRADEDB_UNLOADDB, 2, 
		 0, dirname);
	upgr_cb.Dbname_ptr->tree_error = TRUE;
    }
    else
    {
	/* Truncate "dbname.oNN", for reuse by PCcmdline. */
	if ( OK == SIopen(&sql_output_loc, "w", &fp) )
	{
	    SIclose(fp);
	}
	/*
	**
	** Changes to be made to copy.in:
	** (Read copy.in... make these changes ... write dbname.sql0000n)
	** 
	** 1) Unloaddb adds the following line:
	**  \include $II_SYSTEM/ingres/files/iiud.scr
	** This is a quel script that causes the execution of the script
	** to fail with the error E_UD000C if there are front end catalogs. 
	** We need to strip this line from the unloaddb copy.in script
	**
	** 2) GRANT change for 65ES
	** grant insert -> grant insert, copy_into
	** grant select -> grant select, copy_from
	** grant ALL changes
	*/
	if (fix_iiud)
	{
	    if (upg_dbg_out)
	    {
		SIprintf("Copy: %s \nTo: %s\n(remove iiud.scr %d, fix grants %d)\n", 
			copyin_loc_buf, sql_input_loc_buf,
			fix_iiud, grant_65es);
	    }
	    fix_copy_in(&copyin_loc, &sql_input_loc, fix_iiud);
	}
	else
	{
	    /* Make back up of copy.in -> dbname.sql0000n */
	    if (upg_dbg_out)
		SIprintf("copy %s to %s\n", copyin_loc_buf, sql_input_loc_buf);
	    SIcopy(&copyin_loc, &sql_input_loc);
	}
	/*
	** Build our own command line for sql
	** We need to specify -A6 to connect as upgradedb
	** This is necessary so drop views won't need to read old trees
	**     - see back!qef!qeu qeuqmod.c qeu_v_ubt_id()
	** Also re-define check constraints won't (re)validate constraint
	**     - see back!qef!qeq qeaddl.c qea_createIntegrity{}
	**/
	{
	    char    ingresname[10];
	    STcopy("$ingres", ingresname);
	    if (upgr_cb.Dbname_ptr->upcase)
		CVupper(ingresname);
	    STprintf(sql_cmd, CMDLINE, ingresname, 
		    conn_dbname, sql_input_loc_buf, sql_output_loc_buf);
	}
	/* unloaddb successful */
	did_unload = TRUE;
    }
    /* Even if we had errors... try to redefine internal procedures */
    if (STcompare(dbname, DU_DBDBNAME) == 0)
    {
	if (upg_dbg_out)
	{
	    SIprintf("]==> [9] connect to iidbdb session %d id %s options = %s\n",
		    DBDB_SESSIONID, DU_DBA_DBDB,
		    upg_concurrent ? "+w" : "-A6 +w -l");
	}
	iidbdbname = conn_dbname;
/* # line 11651 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DBDB_SESSIONID);
    IIsqUser(DU_DBA_DBDB);
    IIsqConnect(0,iidbdbname,(char *)"-A6",(char *)"+w",(char *)"-l",
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 11653 "upgradedb.sc" */	/* host code */
	db_open = TRUE;
    }
    else if (star_db)
    {
	if (upg_dbg_out)
	{
	    SIprintf("]==> connect to %s session %d id %s options = -A6 +w -l\n",
		    conn_dbname, STAR_SESSIONID, DU_DBA_DBDB);
	}
/* # line 11662 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(STAR_SESSIONID);
    IIsqUser(DU_DBA_DBDB);
    IIsqConnect(0,sql_conn_dbname,(char *)"-A6",(char *)"+w",(char *)"\
-l",(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 11664 "upgradedb.sc" */	/* host code */
	db_open = TRUE;
    }
    else
    {
	db_open = open_target(CONNECT_EXCL);
/* # line 11669 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DB_SESSIONID),(void *)IILQint(
    DB_SESSIONID));
  }
/* # line 11670 "upgradedb.sc" */	/* host code */
    }
    if (db_open)
    {
	if (!star_db)
	{
	    if (upg_dbg_out)
		SIprintf("Recreating internal procedures\n");
	    /* Re-create all internal procedures AFTER unloaddb, reload.ing */
	    duc_upgrade_internal_procs(dbname, upgr_cb.Dbname_ptr->upcase,
			    upgr_cb.Dbname_ptr->star_flag);
	    IIlq_Protect(TRUE);		/* Restore privs */
	}
	/* Need to redefine rules for referential constraints */
	/* (schka24) I'd rather see this after reload, but no problems
	** have been observed.
	** This doesn't work for upgrades from 1.2.  OI 1.2 didn't create
	** the rule text for the constraint rules!  We'll recreate the
	** entire constraint, below.
	*/
	if (level > V12_LEVEL)
	{
	    if (upg_dbg_out)
		SIprintf("Recreating system generated rules\n");
	    (void) fix_sysgen_rules();
	}
/* # line 11695 "upgradedb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 11696 "upgradedb.sc" */	/* host code */
	upgr_cb.Connected = FALSE;
    }
#ifdef VMS
    if (did_unload || grant_fixes || cons_fixes)
    {
	if (PCcmdline(NULL, CMD_SQLSYMDEF, PC_WAIT, NULL, &err_code) != OK)
	    SIprintf("Defining 'sql' global symbol failed\n");
    }
#endif
    if (did_unload)
    {
	if (upg_dbg_out)
	    SIprintf("Executing the command line: \n%s\n", sql_cmd);
	du_talk(DU_MODESENS, &upgr_cb, I_DU0105_RELOADING_TREES, 0);
	if (PCcmdline(NULL, sql_cmd, (bool)PC_WAIT, NULL, &err_code) != OK)
	{
	    /* save CL err info from UTexe so "du_error" can see it */
	    STRUCT_ASSIGN_MACRO (err_code, errcb.du_clsyserr);
	    du_talk (DU_ALWAYS, &upgr_cb, W_DU186D_UPGRADEDB_SQL, 2, 
		 0, sql_cmd);
	    SIprintf("PCcmdline '%s' failed\n", sql_cmd);
	    upgr_cb.Dbname_ptr->tree_error = TRUE;
	}
    }
    if (grant_fixes)
    {
	char    ingresname[10];
	STcopy("$ingres", ingresname);
	if (upgr_cb.Dbname_ptr->upcase)
	    CVupper(ingresname);
	STprintf(sql_cmd, CMDLINE, ingresname, 
		    conn_dbname, grant_fix_loc_buf, grant_out_loc_buf);
	if (upg_dbg_out)
	    SIprintf("Executing the command line: \n%s\n", sql_cmd);
	if (PCcmdline(NULL, sql_cmd, (bool)PC_WAIT, NULL, &err_code) != OK)
	{
	    /* save CL err info from UTexe so "du_error" can see it */
	    STRUCT_ASSIGN_MACRO (err_code, errcb.du_clsyserr);
	    du_talk (DU_ALWAYS, &upgr_cb, W_DU186D_UPGRADEDB_SQL, 2, 
		 0, sql_cmd);
	    SIprintf("PCcmdline '%s' failed\n", sql_cmd);
	    upgr_cb.Dbname_ptr->tree_error = TRUE;
	}
    }
    if (cons_fixes)
    {
	char    ingresname[10];
	if (level > V12_LEVEL)
	    du_talk(DU_MODESENS, &upgr_cb, I_DU0111_RECREATING_REF_CONS, 0);
	else
	    du_talk(DU_MODESENS, &upgr_cb, I_DU0113_RECREATING_CONS, 0);
	STcopy("$ingres", ingresname);
	if (upgr_cb.Dbname_ptr->upcase)
	    CVupper(ingresname);
	STprintf(sql_cmd, CMDLINE, ingresname, 
		    conn_dbname, ref_fix_loc_buf, ref_out_loc_buf);
	if (upg_dbg_out)
	    SIprintf("Executing the command line: \n%s\n", sql_cmd);
	if (PCcmdline(NULL, sql_cmd, (bool)PC_WAIT, NULL, &err_code) != OK)
	{
	    /* save CL err info from UTexe so "du_error" can see it */
	    STRUCT_ASSIGN_MACRO (err_code, errcb.du_clsyserr);
	    du_talk (DU_ALWAYS, &upgr_cb, W_DU186D_UPGRADEDB_SQL, 2, 
		 0, sql_cmd);
	    SIprintf("PCcmdline '%s' failed\n", sql_cmd);
	    upgr_cb.Dbname_ptr->tree_error = TRUE;
	}
    }
    /* Verify unload-reload was complete */
    if (!STcompare(dbname, DU_DBDBNAME))
    {
	if (upg_dbg_out)
	{
	    SIprintf("]==> [10] connect to iidbdb session %d id %s options = %s\n",
		    DBDB_SESSIONID, DU_DBA_DBDB,
		    upg_concurrent ? "+w" : "-A6 +w -l");
	}
	iidbdbname = conn_dbname;
	/* Reopen iidbdb for remainder of conversion, if any */
/* # line 11775 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(DBDB_SESSIONID);
    IIsqUser(DU_DBA_DBDB);
    IIsqConnect(0,iidbdbname,(char *)"-A6",(char *)"+w",(char *)"-l",
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 11777 "upgradedb.sc" */	/* host code */
	db_open = TRUE;
    }
    else if (star_db)
    {
	if (upg_dbg_out)
	{
	    SIprintf("]==> connect to %s session %d id %s options = -A6 +w -l\n",
		    conn_dbname, STAR_SESSIONID, DU_DBA_DBDB);
	}
/* # line 11786 "upgradedb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(STAR_SESSIONID);
    IIsqUser(DU_DBA_DBDB);
    IIsqConnect(0,sql_conn_dbname,(char *)"-A6",(char *)"+w",(char *)"\
-l",(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 11788 "upgradedb.sc" */	/* host code */
	db_open = TRUE;
    }
    else
    {
	db_open = open_target(CONNECT_EXCL);
/* # line 11793 "upgradedb.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(DB_SESSIONID),(void *)IILQint(
    DB_SESSIONID));
  }
/* # line 11794 "upgradedb.sc" */	/* host code */
    }
    if (!db_open)
    {
	SIprintf("Error re-opening database %s connect dbname %s\n", 
		dbname, conn_dbname);
	return;
    }
    /*
    ** Make sure all views, check constraints, quel permits, procedures
    ** were recreated
    */
    post_view_cnt = post_intg_cnt = post_proc_cnt = -1;
    post_cons_cnt = -1;
/* # line 11807 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iitables where table_type='V' and lowercase(table\
_name)not like 'ii%'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&post_view_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 11810 "upgradedb.sc" */	/* host code */
    if (star_db)
    {
	/* Zero out meaningless quantities */
	pre_intg_cnt = post_intg_cnt = 0;
	pre_proc_cnt = post_proc_cnt = 0;
	pre_cons_cnt = post_cons_cnt = 0;
    }
    else
    {
/* # line 11819 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiintegrities");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&post_intg_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 11820 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiprocedures where text_sequence=1 and lowercase(\
procedure_name)not like 'ii%' and system_use<>'G'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&post_proc_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 11824 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiintegrity where consflags<>0");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&post_cons_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 11827 "upgradedb.sc" */	/* host code */
    }
    old_view_cnt = old_intg_cnt = old_proc_cnt = -1;
/* # line 11829 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iitables where table_type='V' and lowercase(table\
_name)not like 'ii%' and date(create_date)<");
    IIputdomio((short *)0,1,32,0,start_dt);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&old_view_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 11833 "upgradedb.sc" */	/* host code */
    if (! star_db)
    {
/* # line 11835 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiintegrities where date(create_date)<");
    IIputdomio((short *)0,1,32,0,start_dt);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&old_intg_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 11837 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiprocedures where text_sequence=1 and lowercase(\
procedure_name)not like 'ii%' and system_use<>'G' and date(create_date\
)<");
    IIputdomio((short *)0,1,32,0,start_dt);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&old_proc_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 11842 "upgradedb.sc" */	/* host code */
    }
    /* No "old" for constraints since we only need to recreate cross
    ** user ref constraints.  The others will look "old".
    */
    if (upg_dbg_out)
    {
	SIprintf("Upgraded trees for %d views %d integrities %d procs %d cons\n",
	    pre_view_cnt, pre_intg_cnt, pre_proc_cnt, pre_cons_cnt);
	SIprintf("After upgrade: %d views %d integrities %d procs %d cons\n",
	    post_view_cnt, post_intg_cnt, post_proc_cnt, post_cons_cnt);
	SIprintf("Create date < start date for %d views %d integrities %d procs\n",
	    old_view_cnt, old_intg_cnt, old_proc_cnt);
    }
    /* No "old" for constraints since we only need to recreate cross
    ** user ref constraints.  The others will look "old".
    */
    /* See if all views were upgraded successfully */
    if (post_view_cnt != pre_view_cnt || old_view_cnt > 0)
    {
	new_cnt = post_view_cnt - old_view_cnt;
	du_talk(DU_MODESENS, &upgr_cb, W_DU186E_VIEW_UPGRADE, 4,
	    sizeof(pre_view_cnt), &pre_view_cnt, sizeof(new_cnt), &new_cnt);
	upgr_cb.Dbname_ptr->tree_error = TRUE;
    }
    /* See if all integrities were upgraded successfully */
    if (post_intg_cnt != pre_intg_cnt || old_intg_cnt > 0)
    {
	new_cnt = post_intg_cnt - old_intg_cnt;
	du_talk(DU_MODESENS, &upgr_cb, W_DU186F_INTEG_UPGRADE, 4,
	    sizeof(pre_intg_cnt), &pre_intg_cnt, sizeof(new_cnt), &new_cnt);
	upgr_cb.Dbname_ptr->tree_error = TRUE;
    }
    /*
    ** See if all procedures were upgraded successfully 
    ** Constraints can create additional procs (although one would think
    ** that they would be tagged G), so check for < instead of !=
    */
    if (post_proc_cnt < pre_proc_cnt || old_proc_cnt > 0)
    {
	new_cnt = post_proc_cnt - old_proc_cnt;
	du_talk(DU_MODESENS, &upgr_cb, W_DU1870_PROC_UPGRADE, 4,
	    sizeof(pre_proc_cnt), &pre_proc_cnt, sizeof(new_cnt), &new_cnt);
	upgr_cb.Dbname_ptr->tree_error = TRUE;
    }
    /* See if all integrity constraints were upgraded successfully */
    if (post_cons_cnt != pre_cons_cnt)
    {
	du_talk(DU_MODESENS, &upgr_cb, W_DU1871_CONS_UPGRADE, 4,
	    sizeof(pre_cons_cnt), &pre_cons_cnt,
	    sizeof(post_cons_cnt), &post_cons_cnt);
	upgr_cb.Dbname_ptr->tree_error = TRUE;
    }
    if (upgr_cb.Dbname_ptr->tree_error)
	du_talk(DU_MODESENS, &upgr_cb, W_DU1872_UPGRADEDB_TREE_DB, 2,
		    0, dbname);
    return;
}
/*
** Name: fix_sysgen_rules
**
** Description:
**	The unloaddb process won't touch system generated rules.
**	These are generated for constraints.  We could drop and re-add
**	the constraints, but that can take a long time as the constraint
**	condition has to be re-checked, indexes built, etc.
**
**	To regenerate the rule tree, we'll create a rule identical to
**	the rule under consideration, but with a fake name.  We then
**	exchange the iitree ID's between new and old rules, and drop
**	the new rule (which thus drops the old tree).
**
**	This little trick doesn't work for REF constraints that refer
**	to another user's table, as one can't normally create a rule on
**	someone else's table.  But that's what a REF constraint does.
**	Cross-user REF constraints cause permit issues as well, so
**	they are dropped before we get here, and recreated later on.
**
** Inputs:
**	none
**
** Output:
**	Returns OK/notok status.
**
** History:
**	?????
**
**	11-Sep-2004 (schka24)
**	    Take out the special ref-constraint stuff.  It doesn't always
**	    work as coded, and there are other reasons why they have to
**	    be dropped and recreated anyway.
*/
static STATUS
fix_sysgen_rules()
{
  char tmp_rule_name[DB_MAXNAME + 1];
  char cur_rule_name[DB_MAXNAME + 1];
  char cur_rule_owner[DB_MAXNAME + 1];
  char rule_name[DB_MAXNAME + 1];
  char rule_owner[DB_MAXNAME + 1];
  char table_name[DB_MAXNAME + 1];
  i4 text_sequence;
  char text_segment[512];
  char tbuf[100+DB_MAXNAME];
  char *rule_buf;
  i4 cnt;
  i4 max_text_sequence;
  i2 max_null;
  i4 tmp_rule_treeid1;
  i4 tmp_rule_treeid2;
  i4 sys_rule_treeid1;
  i4 sys_rule_treeid2;
  char err_msg[500];
	STATUS		status;
	i4		rows_found;
    /* Init temp rule name */
    STcopy("temp_upgradedb_rule_for_rconstr", &tmp_rule_name[0]);
    if (upgr_cb.Dbname_ptr->upcase)
	CVupper(tmp_rule_name);
    /* Init current rule name, rule owner to empty */
    cur_rule_name[0] = '\0';
    cur_rule_owner[0] = '\0';
/* # line 11965 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*), max(text_sequence)from iirules where system_use='G'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
      IIgetdomio(&max_null,1,30,4,&max_text_sequence);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 11968 "upgradedb.sc" */	/* host code */
    if (sqlca.sqlcode != 0)
	return (FAIL);
    else if (!cnt)
    {
	return (OK);
    }
    if (upg_dbg_out)
	SIprintf("iirules cnt = %d max_null = %d\n", cnt, max_null);
    /* Allocate buffer for rule text */
    rule_buf = (PTR)MEreqmem(0, (max_text_sequence + 1) * sizeof(text_segment),
		TRUE, &status);
    if (status != OK)
    {
	return (FAIL);
    }
    for ( ; ; )
    {
/* # line 11985 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 11986 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed((char *)"set session authorization $ingres");
    IIsyncup((char *)0,0);
  }
/* # line 11987 "upgradedb.sc" */	/* host code */
	/* get the next rule text and create a temp rule just like it */
	rows_found = 0;
	MEfill(sizeof(text_segment), '\0', &text_segment[0]);
	/* Hmm, can we do this any less efficiently... */
/* # line 11991 "upgradedb.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"upgradedb1",7664,8438);
      IIputdomio((short *)0,1,32,0,cur_rule_owner);
      IIputdomio((short *)0,1,32,0,cur_rule_owner);
      IIputdomio((short *)0,1,32,0,cur_rule_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"upgradedb1",7664,8438);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select varchar(rule_name)as rule_name, varchar(rule_owner)as rule_own\
er, varchar(table_name)as table_name, text_sequence, text_segment from\
 iirules where system_use='G' and(rule_owner>");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,cur_rule_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)"or(rule_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,cur_rule_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)"and rule_name>");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,cur_rule_name);
        IIwritio(0,(short *)0,1,32,0,(char *)
"))order by rule_owner, rule_name, text_sequence");
        IIexDefine(0,(char *)"upgradedb1",7664,8438);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE13;
IIrtB13:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,rule_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,rule_owner);
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,table_name);
      IIgetdomio((short *)0,1,30,4,&text_sequence);
      IIgetdomio((short *)0,1,32,511,text_segment);
      if (IIerrtest() != 0) goto IIrtB13;
/* # line 12004 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode != 0)
	    {
/* # line 12006 "upgradedb.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE13;
      }
/* # line 12007 "upgradedb.sc" */	/* host code */
	    }
	    if (text_sequence == 1)
	    {
		char *cp;
		if (rows_found)
		{
/* # line 12013 "upgradedb.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE13;
      }
/* # line 12014 "upgradedb.sc" */	/* host code */
		}
		cp = STstrindex(text_segment, "AFTER", 0, TRUE);
		STprintf(rule_buf, "create rule %s %s", tmp_rule_name, cp); 
		STcopy(rule_name, &cur_rule_name[0]);
		STcopy(rule_owner,  &cur_rule_owner[0]);
	    }
	    else
	    {
		STcat(rule_buf, text_segment);
	    }
	    rows_found++;
	    /* clear text_segment buffer after each row */
	    MEfill(sizeof(text_segment), '\0', &text_segment[0]);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE13:;
  }
	if (!rows_found)
	    break;
	/* set session authorization not allowed if already in transaction */
/* # line 12031 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 12032 "upgradedb.sc" */	/* host code */
	STprintf(tbuf, "set session authorization %s", cur_rule_owner);
/* # line 12033 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(tbuf);
    IIsyncup((char *)0,0);
  }
/* # line 12034 "upgradedb.sc" */	/* host code */
	if (sqlca.sqlcode != 0)
	{
	    SIprintf("sqlcode %d\n", sqlca.sqlcode);
/* # line 12037 "upgradedb.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,499,err_msg,63);
  }
/* # line 12038 "upgradedb.sc" */	/* host code */
	    SIprintf("]==> error text: %s\n\n", err_msg);
	    continue;
	}
	if (upg_dbg_out)
	    SIprintf("%s\n", rule_buf);
/* # line 12043 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(rule_buf);
    IIsyncup((char *)0,0);
  }
/* # line 12044 "upgradedb.sc" */	/* host code */
	if (sqlca.sqlcode != 0)
	{
	   SIprintf("Error for user %s: ", rule_owner);
	   SIprintf("%s returned ", rule_buf);
	   SIprintf("sqlcode %d\n", sqlca.sqlcode);
/* # line 12049 "upgradedb.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,499,err_msg,63);
  }
/* # line 12050 "upgradedb.sc" */	/* host code */
	   SIprintf("]==> error (%d) -  text : %s\n\n", sqlca.sqlcode, err_msg);
	    continue;
	}   
        /* update iirule and switch rule_treeid1, rule_treeid2 */
/* # line 12054 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 12055 "upgradedb.sc" */	/* host code */
	if (sqlca.sqlcode == 0)
	{
/* # line 12057 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed((char *)"set session authorization $ingres");
    IIsyncup((char *)0,0);
  }
/* # line 12058 "upgradedb.sc" */	/* host code */
	}
	if (sqlca.sqlcode == 0)
	{
/* # line 12061 "upgradedb.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session with privileges=\
all");
    IIsyncup((char *)0,0);
  }
/* # line 12062 "upgradedb.sc" */	/* host code */
	    IIlq_Protect(TRUE);
	}
	if (sqlca.sqlcode == 0)
	{
/* # line 12066 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_treeid1, rule_treeid2 from iirule where rule_name=");
    IIputdomio((short *)0,1,32,0,cur_rule_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_owner=");
    IIputdomio((short *)0,1,32,0,cur_rule_owner);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&sys_rule_treeid1);
      IIgetdomio((short *)0,1,30,4,&sys_rule_treeid2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 12071 "upgradedb.sc" */	/* host code */
	}
	if (sqlca.sqlcode == 0)
	{
/* # line 12074 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_treeid1, rule_treeid2 from iirule where rule_name=");
    IIputdomio((short *)0,1,32,0,tmp_rule_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_owner=");
    IIputdomio((short *)0,1,32,0,cur_rule_owner);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tmp_rule_treeid1);
      IIgetdomio((short *)0,1,30,4,&tmp_rule_treeid2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 12079 "upgradedb.sc" */	/* host code */
	}
	if (sqlca.sqlcode == 0)
	{
/* # line 12082 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirule set rule_treei\
d1=");
    IIputdomio((short *)0,1,30,4,&tmp_rule_treeid1);
    IIwritio(0,(short *)0,1,32,0,(char *)", rule_treeid2=");
    IIputdomio((short *)0,1,30,4,&tmp_rule_treeid2);
    IIwritio(0,(short *)0,1,32,0,(char *)"where rule_name=");
    IIputdomio((short *)0,1,32,0,cur_rule_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_owner=");
    IIputdomio((short *)0,1,32,0,cur_rule_owner);
    IIsyncup((char *)0,0);
  }
/* # line 12087 "upgradedb.sc" */	/* host code */
	}
	if (sqlca.sqlcode == 0)
	{
/* # line 12090 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirule set rule_treei\
d1=");
    IIputdomio((short *)0,1,30,4,&sys_rule_treeid1);
    IIwritio(0,(short *)0,1,32,0,(char *)", rule_treeid2=");
    IIputdomio((short *)0,1,30,4,&sys_rule_treeid2);
    IIwritio(0,(short *)0,1,32,0,(char *)"where rule_name=");
    IIputdomio((short *)0,1,32,0,tmp_rule_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_owner=");
    IIputdomio((short *)0,1,32,0,cur_rule_owner);
    IIsyncup((char *)0,0);
  }
/* # line 12095 "upgradedb.sc" */	/* host code */
	}
	if (sqlca.sqlcode == 0)
	{
/* # line 12098 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 12099 "upgradedb.sc" */	/* host code */
	}
	else
	{
	    SIprintf("sqlcode %d\n", sqlca.sqlcode);
/* # line 12103 "upgradedb.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,499,err_msg,63);
  }
/* # line 12104 "upgradedb.sc" */	/* host code */
	    SIprintf("]==> error text: %s\n\n", err_msg);
/* # line 12105 "upgradedb.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 12106 "upgradedb.sc" */	/* host code */
	}
	STprintf(tbuf, "set session authorization %s", cur_rule_owner);
/* # line 12108 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(tbuf);
    IIsyncup((char *)0,0);
  }
/* # line 12109 "upgradedb.sc" */	/* host code */
	/* drop temp rule */
	STprintf(tbuf, "drop rule %s\n", tmp_rule_name);
/* # line 12111 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(tbuf);
    IIsyncup((char *)0,0);
  }
/* # line 12112 "upgradedb.sc" */	/* host code */
	/* try to find the next rule */
    }
    MEfree(rule_buf);
    return (OK);
}
/*{
** Name:	fix_copy_in - Fix copy.in file for upgradedb
** 
** Description: The copy.in file generated by unloaddb may need some editing:
**
** 1) Remove the 'include iiud.scr' 
**    iiud.scr is a quel script that causes the execution of the script
**    to fail with the error E_UD000C if there are front end catalogs.
**
** (We used to mess with old 6.4-style grants, but it was pointless and
** I removed it - schka24.)
**
** Inputs:
**      in		input file
**      out		output file
**      fix_iiud	Strip out include iiud.scr ?
**
** Outputs:
**	none.
**
** History:
**      13-dec-01 (stial01)
**         Re-wrote original routine in doview.qsc, which selected query text
**         and issued sql statements. Here we just update the file created
**         by unloaddb, so we don't have to worry about delimited IDs
**	7-Sep-04 (schka24)
**	   Probably too late to be practical, but ... don't mess with
**	   6.4 style grants.  The server properly translates ALL grants
**	   to whatever ALL means these days (which is IMO the proper action),
**	   and there's no real need to attach COPY-IN and COPY-OUT to
**	   SELECT and INSERT grants.
*/
# define MAX_LINE 16384 /* 16 KB Buffer */
void
fix_copy_in(LOCATION *in, LOCATION *out, bool fix_iiud)
{
    FILE            *in_fp;
    FILE            *out_fp;
    PTR             buf;
    PTR		    buf2;	
    PTR		    cur_buf;
    PTR		    str;
    char	    *cp;
    STATUS          status;
    i4		    count;
    status = SIopen(in, "r", &in_fp);
    if (status == OK)
    {
	status = SIopen(out, "w", &out_fp);
	if (status == OK)
	{
	    buf = (PTR)MEreqmem(0, 2 * MAX_LINE, TRUE, &status);
	    if (status == OK)
	    {
		buf2 = buf + MAX_LINE;
		while (SIgetrec(buf, MAX_LINE, in_fp) == OK)
		{
		    cur_buf = buf;
		    str = NULL;
		    /* Skip leading blanks and tabs */
		    for (cp = buf; 
			(*cp == ' ' || *cp == '\t') && cp < buf + MAX_LINE; 
			cp++);
		    if (fix_iiud && MEcmp(ERx("\\include"), cp, 8) == 0
			&& STstrindex(cp, "iiud.scr", 0, TRUE))
		    {
			/* found the line to remove */
			fix_iiud = FALSE; 
			/* continue without copying this line */
			continue;
		    }
		    status = SIwrite((i4)STlength(cur_buf), cur_buf, &count, out_fp);
		    if (status != OK)
			break;
		}
		if (status == ENDFILE)
		    status = OK;
		MEfree(buf);
	    }
	    SIclose(out_fp);
	}
	SIclose(in_fp);
    }
    /* Pitch return status because nobody cares! */
} /* fix_copy_in */
static STATUS
fix_dbservice(dbname)
  char *dbname;
{
  i4 dbservice;
  i4 newdbservice;
  char *dbname_ptr;
  i4 proc_instruction;
  char areaname[DB_AREA_MAX + 1];
    if (!upgr_cb.dbdb_open)
	return (E_DB_ERROR);
/* # line 12217 "upgradedb.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode on iidatabase where level=table");
    IIsyncup((char *)0,0);
  }
/* # line 12218 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select iidatabase.dbservice, iilocations.area from iidatabase, iiloca\
tions where iidatabase.name=");
    IIputdomio((short *)0,1,32,0,dbname);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and iidatabase.dbdev=iilocations.lname");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dbservice);
      IIgetdomio((short *)0,1,32,DB_AREA_MAX + 1-1,areaname);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 12222 "upgradedb.sc" */	/* host code */
    _VOID_ STtrmwhite(areaname);
    newdbservice = dbservice;
    if (sqlca.sqlcode != 0)
    {
	SIprintf("Cannot update dbservice for database %s\n", dbname);
    }
    else
    {
	if (upgr_cb.dbmsinfo_lp64 == 'Y')
	    newdbservice = dbservice | DU_LP64;
	else
	    newdbservice = dbservice & ~DU_LP64;
	if (newdbservice != dbservice)
	{
	    dbname_ptr = dbname;
	    if (upg_dbg_out)
		SIprintf("Updating iidatabase.dbservice for %s from %x to %x\n",
		    dbname, dbservice, newdbservice);
/* # line 12240 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed((char *)"set session authorization $ingres");
    IIsyncup((char *)0,0);
  }
/* # line 12241 "upgradedb.sc" */	/* host code */
	    /* Update the dbservice field in the config file */
	    proc_instruction = QEA_UPD_SERVICE;
	    if (upg_dbg_out)
	    {
		SIprintf("]==> [2] ii_update_config(%s, %s, %d, %d)\n",
		dbname_ptr, areaname, proc_instruction, newdbservice);
	    }
/* # line 12248 "upgradedb.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"ii_update_config");
    IILQprvProcValio((char *)"database_name",0,(short *)0,1,32,0,dbname_ptr);
    IILQprvProcValio((char *)"location_area",0,(short *)0,1,32,0,areaname);
    IILQprvProcValio((char *)"update_map",0,(short *)0,1,30,4,&
    proc_instruction);
    IILQprvProcValio((char *)"service",0,(short *)0,1,30,4,&newdbservice);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 12253 "upgradedb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		du_talk ( DU_MODESENS, &upgr_cb, W_DU1862_CNF_UPDATE_FAIL,
			  2, 0, upgr_cb.du_opendb);
		upgr_cb.Dbname_ptr->errors = TRUE;
	    }
	    /* Update iidatabase.dbservice */
/* # line 12260 "upgradedb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iidatabase set dbserv\
ice=");
    IIputdomio((short *)0,1,30,4,&newdbservice);
    IIwritio(0,(short *)0,1,32,0,(char *)"where name=");
    IIputdomio((short *)0,1,32,0,dbname);
    IIsyncup((char *)0,0);
  }
/* # line 12262 "upgradedb.sc" */	/* host code */
	}
	else if (upg_dbg_out)
	    SIprintf("Do not need to update iidatabase.dbservice for %s (%x)\n", 
		    dbname, dbservice);
    }
/* # line 12267 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 12268 "upgradedb.sc" */	/* host code */
    return (E_DU_OK);
}
/*}
** Name: fix_syscat_grants
**
** Description:
**	Various problems have arisen over the years with the way
**	that catalogs have been granted.  E.g. old conversions from
**	6.4 left the extended catalogs in a state where they were
**	granted all to the DBA, by the DBA, which had no grant option!
**	(The grant should have been issued by $ingres.)
**	Verifydb in particular has a hissy-fit over this, and wants
**	to drop the catalog. (!)
**
**	In order to clean up the situation, we'll simply drop all
**	base table catalog grants, and reissue them.
**	Ordinary catalogs are granted select to public with grant option.
**	Extended catalogs are additionally granted all to dba.
**	(The grant option lets people define and grant views on
**	catalogs.)  We don't need to deal with standard catalog views,
**	since this code runs while they're all dropped.
**
**	It should be noted that this fixup will take care of catalogs
**	that are dropped/recreated during upgrade, as well.  (The
**	catalog duc_blah create routines typically don't grant anything.)
**
**	It should also be noted that DROP PERMIT is being used instead
**	of the harder-to-generate REVOKE.  DROP PERMIT operates in
**	RESTRICT mode, not CASCADE mode, and so some drops may (silently)
**	fail if users have defined views on system catalogs.  However if
**	a catalog has a view on it, it's probably granted properly
**	already.
**
** Inputs:
**	dbname			The database name
**
** Outputs:
**	none
**
** History
**	8-Sep-2004 (schka24)
**	    Written to clean up detritus from old upgrades.
*/
static void
fix_syscat_grants(char *dbname)
{
  char grant_stmt[DB_MAXNAME+50];
  char table_name[DB_MAXNAME+1];
  i4 relstat;
    du_talk(DU_MODESENS, &upgr_cb, I_DU0107_FIXING_CAT_GRANTS, 0);
    /* Cursors suck toxic wastes, but it's the easiest way.
    ** The varchar() is for blank stripping.
    */
    /* No views, no indexes, and [extended] catalogs only.
    ** that aren't ii_xxx catalogs (eg iidbcapabilities).
    ** (because they aren't treated the same by createdb either.)
    ** Basically, all the front-end stuff.
    */
/* # line 12338 "upgradedb.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"grant_c1",7665,17396);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select varchar(relid)as relid, relstat from iirelation where lowercas\
e(relowner)='$ingres' and(mod(relstat/16384, 2)=1 or mod(relstat, 2)=1\
)and mod(relstat/32, 2)=0 and mod(relstat/128, 2)=0 and lowercase(reli\
d)not in('iirelation', 'iiprotect', ");
    IIwritio(0,(short *)0,1,32,0,(char *)"'iiqrytext', 'iitree')");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"grant_c1",7665,17396);
  }
/* # line 12339 "upgradedb.sc" */	/* host code */
    check_sql(sqlca.sqlcode);
    if (sqlca.sqlcode < 0)
    {
/* # line 12342 "upgradedb.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 12343 "upgradedb.sc" */	/* host code */
	SIprintf("Error fixing system catalog grants\n");
	return;
    }
    for (;;)
    {
/* # line 12348 "upgradedb.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"grant_c1",7665,17396,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,DB_MAXNAME + 1-1,table_name);
      IIcsGetio((short *)0,1,30,4,&relstat);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 12349 "upgradedb.sc" */	/* host code */
	if (sqlca.sqlcode == DU_SQL_NONE)
	    break;
	CVlower(table_name);
	STprintf(grant_stmt, "drop permit on %s all", table_name);
	if (upg_dbg_out)
	    SIprintf("]==> %s\n", grant_stmt);
/* # line 12355 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(grant_stmt);
    IIsyncup((char *)0,0);
  }
/* # line 12356 "upgradedb.sc" */	/* host code */
	/* Ignore drop error... */
	/* Extended catalogs get "all to dba".  Note: only do
	** ii_xxx type catalogs.  iidbcapabilities and one other are
	** marked as ext-catalogs, but don't get the all-to-dba priv.
	** All catalog names are at least 3 chars long...!
	*/
	if ((relstat & TCB_EXTCATALOG) && table_name[2] == '_')
	{
	    STprintf(grant_stmt, "grant all on %s to %s", table_name,upgr_cb.Dbname_ptr->dba);
	    if (upg_dbg_out)
		SIprintf("]==> %s\n", grant_stmt);
/* # line 12367 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(grant_stmt);
    IIsyncup((char *)0,0);
  }
/* # line 12368 "upgradedb.sc" */	/* host code */
	    check_sql(sqlca.sqlcode);
	    if (sqlca.sqlcode < 0)
	    {
/* # line 12371 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 12372 "upgradedb.sc" */	/* host code */
		SIprintf("Error granting extended catalog %s\n",table_name);
		break;
	    }
	}
	/* Now for the regular select-to-public grant */
	STprintf(grant_stmt, "grant select on %s to public with grant option", table_name);
	if (upg_dbg_out)
	    SIprintf("]==> %s\n", grant_stmt);
/* # line 12380 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(grant_stmt);
    IIsyncup((char *)0,0);
  }
/* # line 12381 "upgradedb.sc" */	/* host code */
	check_sql(sqlca.sqlcode);
	if (sqlca.sqlcode < 0)
	{
/* # line 12384 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 12385 "upgradedb.sc" */	/* host code */
	    SIprintf("Error granting catalog %s\n",table_name);
	    break;
	}
    }
/* # line 12389 "upgradedb.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"grant_c1",7665,17396);
  }
/* # line 12390 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 12391 "upgradedb.sc" */	/* host code */
    /* Do iirelation and permit catalogs separately.  "DROP" does a
    ** protect/qrytext/tree scan and you get "close that table" error.
    ** Don't mess with iiprotect or iiqrytext at all, other than
    ** making sure that a grant is there.
    */
/* # line 12396 "upgradedb.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop permit on iirelation al\
l");
    IIsyncup((char *)0,0);
  }
/* # line 12397 "upgradedb.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on iirelation to public with grant option");
    IIsyncup((char *)0,0);
  }
/* # line 12398 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 12399 "upgradedb.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on iiqrytext to public with grant option");
    IIsyncup((char *)0,0);
  }
/* # line 12400 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 12401 "upgradedb.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on iiprotect to public with grant option");
    IIsyncup((char *)0,0);
  }
/* # line 12402 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 12403 "upgradedb.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"grant select on iitree to public with grant option");
    IIsyncup((char *)0,0);
  }
/* # line 12404 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 12405 "upgradedb.sc" */	/* host code */
} /* fix_syscat_grants */
/*}
** Name: fix_64_grants
**
** Description:
**	When converting from 6.4 or earlier, grants of all-to-public
**	or select-to-public are ONLY recorded in iirelation.  (Unlike
**	later releases, which have the iirelation bit, but also record
**	the individual grants composing the ALL privilege.)
**	We need to detect these all-to-public and select-to-public
**	grants, and reissue them as proper grant statements so
**	that utilities such as unloaddb understand them.
**
**	Fixing these grant problems brings up a nice little chicken-
**	and-egg situation.  We can't regrant views until the query
**	trees are rebuilt with unload/reload.  If we wait to do this
**	until after the unloaddb script runs, we lose all memory of
**	the original public grants (since the script drops all permits).
**	The best answer seems to be to write a script file with the
**	proper grants, to be run after unloaddb and reload runs.
**
**	This routine is called from fix-query-trees after all standard
**	catalog views are dropped (so we don't need to deal with them).
**
** Inputs:
**	dbname			The database name
**	grant_fix_loc		LOCATION for grant-fix script file
**	grant_fix_loc_buf	String buffer for above (for msgs)
**
** Outputs:
**	TRUE if need to run grant-fix script, else FALSE
**
** History
**	8-Sep-2004 (schka24)
**	    Write, eight years too late.  Bah.  Replaces grant_select_64_11
**	    which was being called too late, and didn't fix ALL grants.
*/
static bool
fix_64_grants(char *dbname, LOCATION* grant_fix_loc,
	char *grant_fix_loc_buf)
{
    bool any_grants;			/* TRUE if anything scripted */
    char last_owner[DB_MAXNAME+1];	/* Previous owner */
    FILE *fp;				/* Script output stream */
  char all_to_all[1+1];
  char grant_stmt[DB_MAXNAME+50];
  char select_to_all[1+1];
  char table_name[DB_MAXNAME+1];
  char table_owner[DB_MAXNAME+1];
    du_talk(DU_MODESENS, &upgr_cb, I_DU0106_FIXING_64_GRANTS, 0);
    /* Generate grant all or grant select to public statements, for
    ** everything that has the all-to-all or ret-to-all bits clear.
    ** We'll skip standard or extended catalogs, as we dealt with them
    ** in fix-syscat-grants.
    **
    ** Note that the all-to-all and ret-to-all bits work in negative
    ** logic, ie set means OFF.
    */
    if (SIopen(grant_fix_loc, "w", &fp) != OK)
    {
	SIprintf("Error opening grant-fix output file %s\n", grant_fix_loc_buf);
	return (FALSE);
    }
    if (upg_dbg_out)
	SIprintf("]==> Writing public grants to %s\n",grant_fix_loc_buf);
    any_grants = FALSE;
    /* Set for continue after errors, autocommit */
    SIfprintf(fp, "\\continue\nset autocommit on\n\\p\\g\n");
    last_owner[0] = '\0';
    /* Fire up select loop */
/* # line 12478 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select varchar(relid)as relid, varchar(relowner)as relowner, charextr\
act('YN', mod((relstat/4096), (2)) +1)as all_to_all, charextract('YN',\
 mod((relstat/8192), (2)) +1)as ret_to_all from iirelation where mod(r\
elstat/4096, 4)!=3 and mod(relstat/128, 2)=0 ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"and(relowner!='$ingres' or mod(relstat/16384, 2) +mod(relstat, 2)=0)o\
rder by relowner, relid");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE15;
IIrtB15:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,table_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,table_owner);
      IIgetdomio((short *)0,1,32,1+1-1,all_to_all);
      IIgetdomio((short *)0,1,32,1+1-1,select_to_all);
      if (IIerrtest() != 0) goto IIrtB15;
/* # line 12489 "upgradedb.sc" */	/* host code */
	any_grants = TRUE;
	/* If different owner, emit authorization change */
	if (STcompare(table_owner, last_owner) != 0)
	{
	    SIfprintf(fp, "set session authorization %s\n\\p\\g\n", table_owner);
	    STcopy(table_owner, last_owner);
	}
	/* Issue the proper grant. */
	SIfprintf(fp, "grant %s on %s to public\n\\p\\g\n",
		    all_to_all[0] == 'Y' ? "all" : "select", table_name);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE15:;
  }
    check_sql(sqlca.sqlcode);
    if (sqlca.sqlcode < 0)
    {
/* # line 12503 "upgradedb.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 12504 "upgradedb.sc" */	/* host code */
	SIprintf("Error retrieving ALL and SELECT public grants\n");
	any_grants = FALSE;
    }
/* # line 12507 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 12508 "upgradedb.sc" */	/* host code */
    SIclose(fp);
    return (any_grants);
} /* fix_64_grants */
/*}
** Name: fix_constraints - Fix 1.2 or cross-user referential constraints
**
** Description:
**	This routine fixes up constraints for a couple of problem cases:
**	cross-user constraints, or 1.2 constraints.  The problems are
**	rather different, but the solution is the same, which is why
**	they are both dealt with here.
**
**	Cross-user referential integrity constraints pose an upgrade
**	problem, in a a couple different ways:
**
**	1. The constraint rule(s) generated for the referenced table
**	are created by the constraint user C, but belong to and apply
**	to the referenced table owner R.  Normally, one can't create
**	rules for someone else's table.  This will blow out the
**	rule tree fixups in fix_system_rules.
**
**	2. The constraint depends on the REFERENCES permission
**	on the referenced table.  If there are permits with query
**	trees, unloaddb is told to drop all permits and recreate them
**	all.  The permit drop on the referenced table will fail
**	because (for convenience) it's done with DROP ALL, not REVOKE,
**	and DROP operates in RESTRICT mode, not CASCADE mode.  (Using
**	REVOKE doesn't really help, because REVOKE CASCADE would
**	drop the constraint!)
**
**	Note that these problems only arise when the referencing and
**	referenced tables have different owners.
**
**	1.2 constraints that generate rules (namely referential and
**	check constraints), create rules with no text!  Amazing.  Since
**	there's no rule text, the trick used by fix_sysgen_rules to
**	rebuild a new version rule tree doesn't work.  The only solution
**	is to drop and recreate all ref and check constraints.
**
**	This routine fixes either situation by dropping the selected
**	constraints, and writing a script file to recreate them after
**	all other query trees (and permits) are dropped and recreated.
**
**	We're called after standard catalogs have been recreated, but
**	before unloaddb is run to generate permit and object drops
**	and recreates.
**
** Inputs:
**	dbname			The database name
**	level			The original database version
**	ref_fix_loc		LOCATION for ref-fix script file
**	ref_fix_loc_buf		String buffer for above (for msgs)
**
** Outputs:
**	TRUE if need to run ref-recreate script, else FALSE
**
** History
**	12-Sep-2004 (schka24)
**	    Replace original cross-user workaround, which failed in the
**	    presence of QUEL permits thanks to issue #2 above.
*/
static bool
fix_constraints(char *dbname, i4 level, LOCATION* ref_fix_loc,
	char *ref_fix_loc_buf)
{
    bool first_time;			/* First-time loop flag */
    char last_owner[DB_MAXNAME+1];	/* Previous owner */
    FILE *fp;				/* Script output stream */
    STATUS st;				/* General status holder */
  char cons_name[DB_MAXNAME+3];
  char sel_cons_name[DB_MAXNAME+1];
  char sel_table_name[DB_MAXNAME+1];
  char sel_schema_name[DB_MAXNAME+1];
  char sql_stmt[2*DB_MAXNAME+100];
  char table_name[DB_MAXNAME+3];
  char table_owner[DB_MAXNAME+1];
  char text_segment[250];
  char *text_buf;
  i4 maxseq;
  i4 n;
    /* First, see if there are any of these buggers to worry about */
    if (level > V12_LEVEL)
    {
/* # line 12593 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*), ifnull(max(c.text_sequence), 0)as max_seq from iiref\
_constraints r, iiconstraints c where r.ref_schema_name<>r.unique_sche\
ma_name and c.constraint_name=r.ref_constraint_name and c.schema_name=\
r.ref_schema_name and c.table_name=r.");
    IIwritio(0,(short *)0,1,32,0,(char *)"ref_table_name");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&n);
      IIgetdomio((short *)0,1,30,4,&maxseq);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 12600 "upgradedb.sc" */	/* host code */
    }
    else
    {
/* # line 12603 "upgradedb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*), ifnull(max(text_sequence), 0)as max_seq from iiconst\
raints where constraint_type in('C', 'R')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&n);
      IIgetdomio((short *)0,1,30,4,&maxseq);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 12607 "upgradedb.sc" */	/* host code */
    }
    check_sql(sqlca.sqlcode);
    if (sqlca.sqlcode < 0)
    {
	SIprintf("Error checking for cross-user ref constraints\n");
	return (FALSE);
    }
    if (n == 0)
	return (FALSE);
    /* Work to do, allocate some text working space */
    if (level > V12_LEVEL)
	du_talk(DU_MODESENS, &upgr_cb, I_DU0110_DROPPING_REF_CONS, 0);
    else
	du_talk(DU_MODESENS, &upgr_cb, I_DU0112_DROPPING_CONS, 0);
    text_buf = (char *) MEreqmem(0, (maxseq + 1) * sizeof(text_segment),
		TRUE, &st);
    if (st != OK)
    {
	SIprintf("MEreqmem for %d failed\n", (maxseq+1) * sizeof(text_segment));
	return (FALSE);
    }
    if (SIopen(ref_fix_loc, "w", &fp) != OK)
    {
	SIprintf("Error opening ref-constraint-fix output file %s\n", ref_fix_loc_buf);
	return (FALSE);
    }
    if (upg_dbg_out)
	SIprintf("]==> Writing ref constraint re-adds to %s\n",ref_fix_loc_buf);
    /* Set for continue after errors, autocommit */
    SIfprintf(fp, "\\continue\nset autocommit on\n\\p\\g\n");
    last_owner[0] = '\0';
    /* Select a cross-user ref constraint, drop it, and write a recreate
    ** request into the ref fixup file.
    ** Unfortunately, the drop may require a user switch, which requires
    ** a commit, so this can't be done with a cursor.  Laboriously
    ** re-select each time.
    ** Varchar for blank strip.
    */
    for (;;)
    {
	first_time = TRUE;
	text_buf[0] = EOS;
	/* Easiest to just duplicate the text gathering loop for
	** the two different conditions: ref constraints or 1.2 constraints
	*/
	if (level > V12_LEVEL)
	{
/* # line 12654 "upgradedb.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"upgradedb2",7666,8641);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"upgradedb2",7666,8641);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select varchar(r.ref_constraint_name)as ref_constraint_name, varchar(\
r.ref_schema_name)as ref_schema_name, varchar(r.ref_table_name)as ref_\
table_name, c.text_sequence, c.text_segment from iiref_constraints r, \
iiconstraints c where r.ref_schema_name<>r.");
        IIwritio(0,(short *)0,1,32,0,(char *)
"unique_schema_name and c.constraint_name=r.ref_constraint_name and c.\
schema_name=r.ref_schema_name and c.table_name=r.ref_table_name order \
by ref_schema_name, ref_table_name, ref_constraint_name, text_sequence");
        IIexDefine(0,(char *)"upgradedb2",7666,8641);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE16;
IIrtB16:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,sel_cons_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,sel_schema_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,sel_table_name);
      IIgetdomio((short *)0,1,30,4,&n);
      IIgetdomio((short *)0,1,32,511,text_segment);
      if (IIerrtest() != 0) goto IIrtB16;
/* # line 12670 "upgradedb.sc" */	/* host code */
		if (n == 1)
		{
		    /* First segment, see if first or new constraint */
		    if (! first_time)
		    {
			/* Start of new constraint, skip out */
/* # line 12676 "upgradedb.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE16;
      }
/* # line 12677 "upgradedb.sc" */	/* host code */
		    }
		    first_time = FALSE;
		    /* Move constraint info to safe place. */
		    STcopy(sel_cons_name, cons_name);
		    STcopy(sel_schema_name, table_owner);
		    STcopy(sel_table_name, table_name);
		}
		STcat(text_buf, text_segment);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE16:;
  }
	}
	else
	{
/* # line 12689 "upgradedb.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"upgradedb3",7667,8693);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"upgradedb3",7667,8693);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select varchar(constraint_name)as constraint_name, varchar(schema_nam\
e)as schema_name, varchar(table_name)as table_name, text_sequence, tex\
t_segment from iiconstraints where constraint_type in('C', 'R')order b\
y schema_name, table_name, constraint_name, ");
        IIwritio(0,(short *)0,1,32,0,(char *)"text_sequence");
        IIexDefine(0,(char *)"upgradedb3",7667,8693);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE17;
IIrtB17:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,sel_cons_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,sel_schema_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,sel_table_name);
      IIgetdomio((short *)0,1,30,4,&n);
      IIgetdomio((short *)0,1,32,511,text_segment);
      if (IIerrtest() != 0) goto IIrtB17;
/* # line 12701 "upgradedb.sc" */	/* host code */
		if (n == 1)
		{
		    /* First segment, see if first or new constraint */
		    if (! first_time)
		    {
			/* Start of new constraint, skip out */
/* # line 12707 "upgradedb.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE17;
      }
/* # line 12708 "upgradedb.sc" */	/* host code */
		    }
		    first_time = FALSE;
		    /* Move constraint info to safe place. */
		    STcopy(sel_cons_name, cons_name);
		    STcopy(sel_schema_name, table_owner);
		    STcopy(sel_table_name, table_name);
		}
		STcat(text_buf, text_segment);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE17:;
  }
	}
	check_sql(sqlca.sqlcode);
	if (sqlca.sqlcode < 0)
	{
	    SIprintf("Error gathering constraint info\n");
	    break;
	}
/* # line 12724 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 12725 "upgradedb.sc" */	/* host code */
	if (first_time)
	    break;			/* Finished, exit */
	/* Drop this constraint */
	if (setauth(table_owner) != OK)
	{
	    SIprintf("fix_constraints: error setting to user %s", table_owner);
	    break;
	}
	STprintf(sql_stmt, "alter table \"%s\" drop constraint \"%s\" restrict",
		table_name, cons_name);
	if (upg_dbg_out)
	    SIprintf("]==> fix_constraint: %s\n",sql_stmt);
/* # line 12737 "upgradedb.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sql_stmt);
    IIsyncup((char *)0,0);
  }
/* # line 12738 "upgradedb.sc" */	/* host code */
	check_sql(sqlca.sqlcode);
	if (sqlca.sqlcode < 0)
	{
	    SIprintf("Error dropping constraint %s (table %s, user %s)\n",
		cons_name,table_name,table_owner);
	    break;
	}
	/* Write the recreate string */
	if (STcompare(table_owner, last_owner) != 0)
	{
	    SIfprintf(fp, "set session authorization %s\n\\p\\g\n", table_owner);
	    STcopy(table_owner, last_owner);
	}
	/* In the learn-something-new-every-day dept:
	** If the constraint name starts with "$", we could omit the
	** entire "constraint xxx" phrase, and let the server (re)name
	** it.  That would also allow removal of the hack in pslsgram.yi
	** that I added to let upgradedb define objects with $-names.
	** I didn't know that syntax was valid.
	** Todo someday.
	*/
	SIfprintf(fp, "alter table \"%s\" add constraint \"%s\" %s\n\\p\\g\n",
		table_name, cons_name, text_buf);
    } /* for */
    /* All cross-user ref constraints should be gone unless an error occurred */
    SIclose(fp);
/* # line 12764 "upgradedb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 12765 "upgradedb.sc" */	/* host code */
    setauth("$ingres");
    return (TRUE);
} /* fix_constraints */
