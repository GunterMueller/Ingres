# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**Copyright (c) 2004 Ingres Corporation
*/
#include    <compat.h>
#include    <pc.h>
#include    <gl.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <duf.h>
#include    <er.h>
#include    <me.h>
#include    <st.h>
#include    <si.h>
#include    <ds.h>
#include    <ut.h>
#include    <cs.h>      /* required by lk.h */
#include    <lk.h>	/* required by duenv.h */
#include    <duerr.h>
# include <dudbms.h>
# include <duenv.h>
#include    <ducdb.h>
#include    <duu.h>
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/**
**
**  Name: DUICRDB.QC -	CREATEDB support routines that are owned by the
**			frontend group.
**
**  Description:
**	  The routines in this module are owned by the FE group.
**	They are use exclusively by the database utility, createdb.
**
**
**          dui_makedb() -	make the system catalogs that are used by and
**				under the control of the frontend group.
**	    dui_stdcats() -	create the standard catalog interface
**
**  History:
**      13-Apr-87 (ericj)
**          Initial creation (for SandyD).
**	15-apr-87 (sandyd)
**	    Changed "rcosection" and "rcocommand" back to char(12).
**	20-apr-87 (sandyd)
**	    Moved "modifies" into "duimod.qc" module.
**	    Removed "define permit" since that is not supported on catalogs now.
**	23-apr-87 (sandyd)
**	    Added "alter_count" and "last_altered_by" in ii_objects.
**	    Changed "relid" to "depid" in ii_dependencies.
**	24-apr-87 (sandyd)
**	    Changed "cstring" to "estring" in ii_encodings.
**	24-apr-87 (sandyd)
**	    Added "language" to ii_object and ii_longremark.
**	24-apr-87 (sandyd)
**	    Fixed missing comma after "alter_date".
**	11-may-87 (sandyd)
**	    Added "repbcount" to ii_reports.
**	18-may-87 (sandyd)
**	    Change required by standard catalog (gateway) specs:
**	    All required fields limited to 240 bytes.  Also changed
**	    long CHARs to VARCHAR to make compression more likely.
**	    Added call to dui_femod().
**	27-may-87 (sandyd)
**	    Would you believe ... changed ii_encodings.estring back to
**	    1990, and ii_encoded_forms.cfdata back to 1960.  (Frontend
**	    code will dynamically adapt to backends that don't support
**	    those lengths.)  Also changed ii_objects DATE's to CHAR(25)
**	    format, as per Standard Catalog spec.  Also replaced ii_dependencies
**	    with ii_abfdependencies.
**	13-aug-87 (greg)
**	    Add peter's routine, dui_stdcats().
**	21-aug-87 (sandyd)
**	    Made dui_stdcats() externally known, so that FECVT60 can call it.
**	31-aug-1987 (peter)
**	    Add parentheses in view definitions to avoid II_DECIMAL problems.
**	17-sep-1987 (peter)
**	    Change types of some fields for consistency.
**	01-oct-1987 (peter)
**	    Took out function casts on relid and relowner for performance.
**	03-feb-1988 (peter)
**	    Move locations to a new table.  Take out alternate key tables.
**	    Update defaultability field.  
**	18-aug-1988 (peter)
**	    Add update to iicolumns to fix problem with precision
**	       for decimal.
**	    Add iialternate_keys, iialt_columns, iiprocedures.
**	    Change definition of iipermits to reflect permissions
**	       on procedures.
**	       to avoid name conflict in future.
**	    Add table_subtype to iitables.
**	01-nov-1988 (jrb)
**	    Added new function: dui_fegrants() which grants all privileges on
**	    extended catalogs to the dba.  This was done because certain VARs
**	    were used to updating these catalogs.
**	19-may-1989 (neil)
**	    Added "iirules" view for TM support.
**	20-jun-89 (ralph)
**	    Added MAX_COLUMNS to iidbcapabilities
**	26-jun-1989 (mikem)
**	    Added 3 new columns "column_internal_datatype", 
**	    "column_internal_length", and "column_system_maintained" to 
**	    iicolumns standard catalog.  Also added new capabilities 
**	    INGRES_LOGICAL_KEY, INGRES_RULES, INGRES_UDT, INGRES_AUTH_GROUP, 
**	    and INGRES_AUTH_ROLE.
**	14-jul-1989 (ralph)
**	    Changed the following catapbilities for terminator:
**		QUEL_LEVEL was "ING6.0" now "ING7.0"
**		SQL_LEVEL  was "ING6.0" now "ING7.0"
**		INGRES/SQL_LEVEL  was "00601" now "00700"
**		INGRES/QUEL_LEVEL was "00601" now "00700"
**		COMMON/SQL_LEVEL  was "00601" now "00700"
**	4-Aug-1989 (teg)
**	    dropped in ii_sequence_value (catalog and procedure) for J. Wong
**	    for FE development.
**	18-Sep-1989 (pete)
**	    Added new definition for standard catalog view iialt_columns;
**	    the 6.x version was wrong and didn't work as documented. (bug 8110)
**	18-Sep-1989 (pete)
**	    Changed the MODIFY statement for ii_sequence_value to specify
**	    "hash UNIQUE" and "fillfactor = 1". Also change name of this
**	    catalog to be plural (ii_sequence_values), and
**	    change the database procedure _ii_sequence_value
**	    to access the table under the new (plural) name.
**	22-sep-1989 (teg)
**	    As per Steering Committee decision, ingres 7.0 is now ingres 6.3,
**	    so Changed the following catapbilities for terminator:
**		QUEL_LEVEL was "ING7.0" now "ING6.3"
**		SQL_LEVEL  was "ING7.0" now "ING6.3"
**		INGRES/SQL_LEVEL  was "00700" now "00603"
**		INGRES/QUEL_LEVEL was "00700" now "00603"
**		COMMON/SQL_LEVEL  was "00700" now "00603"
**	08-oct-89 (ralph)
**	    B1 Enhancements:
**		Changed iipermits definition to exclude security alarm tuples.
**		Added iisecurity_alarms view definition.
**	31-oct-1989 (cal)
**          Due to vifred, vigraph problems running with the changes made for
**          B1, cfdata is going back to varchar(1960) and encode_estring is going
**	    back to varchar(1990).  This change has also been made to 6.4 line,
**          but will need fixing before B1 is built.
**	28-nov-1989 (alexc)
**	    Add the following rows to iidbcapabilities for bug fix(8885)
**		UNIQUE_KEY_REQ	N
**		ESCAPE		Y
**		OWNER_NAME	QUOTED
**	03-jan-90 (neil)
**	    Alerters: Add "iievents" and modified "iipermits" to use "iievent".
**	06-jan-1990 (alexc)
**	    Create standard catalog view for COMMENTs on columns, and UNLOADDB
**		to unload COMMENTs.
**	22-mar-90 (fourt)
**	    Add definition of view 'iiregistrations', for Gateway db's.
**	28-mar-1990 (alexc)
**	    Add comtype=1 as qualifier for creating view iidb_comments, and
**		comtype=2 as qualifier for creating view iidb_subcomments.
**	16-apr-90 (alex)
**	    Correct column names for iiregistrations.  From table_XXX to
**		object_XXX.
**      19-apr-1990 (alexc)
**          iitables, iiphysical_tables, and iiingres_tables have incorrect
**              initialization values = "N ", correct values is = "NY" in all
**              cases.
**	27-apr-1990 (alexc)
**	    Add the following rows to iidbcapabilities for bug fix(8887)
**		UNIQUE_KEY_REQ	N
**		ESCAPE		Y
**		OWNER_NAME	QUOTED
**	27-apr-1990 (alexc)
**	    Modify iiregistrations to conform with change from GW on
**		object_subtype and qualification for create View.
**	10-may-90 (fourt)
**	    Generalize setup of 'iidbcapabilities' to make entries dependent
**	    on which (if any) Gateway we're using.
**	17-may-1990 (pete)
**	    Modify to support new -f flag for front-end dictionary creation.
**	    Also add call to dui_bldDefClientList(), if user doesn't
**	    specify the -f flag.
**	21-may-1990 (pete)
**	    Removed routine dui_fegrants. NO longer needed: done by upgradefe.
**	    Also, change order so Std Cats are created before Front-end
**	    catalogs.
**	23-may-90 (edwin)
**	    Slightly revise definition of iiregistrations to reflect retention
**	    of token DB_CRT_LINK.
**	25-may-90 (edwin)
**	    No more call to duc_abort_internal(), to fix link problems.
**	29-may-90 (teg)
**	    fixed alexc's bugfix 8887, which was really a fix for bug 8885,
**	    not 8887.  Alexc intended to set ESCAPE to Y, but set it to N
**	    instead.  I corrected it to "Y".
**	9-jul-90 (linda)
**	    Changed GW_NONE, GW_RMS_FLAG constant names to DU_GW_NONE,
**	    DU_GW_RMS_FLAG to correspond to naming conventions in file
**	    commonhdr!dudbms.qh.
**	12-jul-1990 (pete)
**	    Remove call to dui_femod -- this routine no longer needed cause
**	    front-end cats are modified by catinstall. Only pass "-s" flag
**	    if client list came from authorization string.
**	08-jul-1990 (boba)
**	    Change "##include <dudbms.qh>" to "#include <dudbms.h)" as
**	    building a .qsc file that ##include's a .qh is not supported
**	    by either the core product or porting build tools.  Many of
**	    the .c files in back already #include this derived header
**	    file.  As of 6.5 the .qh and .qc files will be changed to .qsh
**	    and .qsc files, respectively.  The back .c files, however,
**	    will continue to include the derived .h.
**	24-jul-1990 (teresa)
**	    Changed .qh include files to .qsh to fix unix build problem.
**	08-aug-90 (ralph)
**	    Add iisecurity_status view definition
**	12-sep-1990 (sandyd)
**	    Merged in boba's change (described above) and had to make a similar
**	    change regarding "duenv.qh", which pete had introduced as part of
**	    his upgradefe work.
**	19-sep-90 (pete)
**	    add wrapper for convto60 (fecv60()), dui_fecvt60() which calls
**	    dui_makedb(). also, define
**	    iiduNoFeClients here rather than in ducutil.c. Was causing link
**	    problems for fecv60() which calls dui_makedb().
**	28-sep-1990 (pete)
**	    split code for dui_bldDefClientList into separate file.
**	04-oct-90 (teresa)
**	   fix STAR bug 30502
**      08-OCT-90 (DEREK)
**	    Took encode_estring back to varchar(1900) and
**          cfdata back to varchar(1900) for B1.  New frontends (6.4?)
**          should handle this.
**	10-dec-90 (neil)
**	    Alerters: Add "iievents" and modified "iipermits" to use "iievent".
**	25-mar-1991 (bryanp)
**	    Added support for Btree index compression: new column added to
**	    iitables, iiphysical_tables, and iiindexes views:
**		"key_is_compressed" (values are "y", "n").
**	    Also picked up some miscellaneous catalog changes from the Front
**	    Ends group (Pete):
**		ING6.3 ==> ING6.4, 00603 ==> 00604 (iidbcapabilities)
**		new column in iidbconstants -- system_owner
**		correct the definition of iiphysical_tables.duplicate_rows
**		New column column_updateable in iicolumns.
**		sort_direction in iiindex_columns no longer hard-coded.
**	9-23-91 (teresa)
**	    Picking up 6.4 bugfixes into 6.5 by starting with 6.5 baseline
**	    and editing in differences that are 6.5 specific (such as 
**	    00604->00700, mod(relstat,x)->mod((relstat+1073741824+1073741824),x)
**	11-sep-92 (andre)
**	    MSB in relstat is no longer being set - change
**	    mod((relstat+1073741824+1073741824),x) back to mod(relstat,x)
**	29-jul-92 (rickh)
**	    FIPS CONSTRAINTS improvements.
**	  05-oct-92 (rblumer)
**	    Added iidefaults, iiconstraints, iikeys, & iiref_constraints views.
**	05-dec-92 (rblumer)
**	    removed call to duc_v_iidefaults; the view is no longer needed.
**	24-may-1993 (bryanp)
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**	04-aug-93 (shailaja)
**	    Unnested comments.
**	8-aug-93 (ed)
**	    unnest dbms.h
**	9-sep-93 (robf)
**	   Add INGRES_MAC_LEVEL 
**	13-nov-93 (rblumer))
**	    changed several functions to use new du_xusrname field;
**	    changed dui_scats_exist function and iidbconstants view
**	    to handle case-translation semantics correctly.
**	18-nov-93 (rblumer)
**	    Changed iiindex_columns and iialt_columns to filter out upper-case
**	    tidp columns in addition to lower-case tidp columns.
**	    This fixes Star bug 56239.
**	16-dec-93 (robf)
**         Create iiphysical_columns
**      31-jan-94 (mikem)
**          Added include of <cs.h> before <lk.h>.  <lk.h> now references
**          a CS_SID type.
**	 4-apr-96 (nick)
**	    Move definition of iimulti_locations to its own function.
**      28-sep-96 (mcgem01)
**              Global data moved to duidata.c
**	15-may-97 (mcgem01)
**	    Add include of ds.h.
**	21-may-1999 (hanch04)
**	    Replace STbcompare with STcasecmp,STncasecmp,STcmp,STncmp
**	01-jun-1999 (kitch01)
**	   Bug 97040. In dui_stdcats() move the creation of iilog_help
**	   to ducommon.c so that upgradedb may call it as well. Fix the
**	   view to use gmt_timestamp instead of _date for Y2K compatability.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      02-Mar-2001 (hanch04)
**          Bumped release id to external "II 2.6" internal 860 or 8.6
**	18-May-01 (gordy)
**	    Added NATIONAL_CHARACTER_SET row to iidbcapabilities.
**	07-Jan-02 (toumi01)
**	    Increased the MAX_COLUMNS from 300 to 1024.
**      09-jan-2002 (stial01)
**          Call duc_create_stdcat_views (SIR 106745)
**      01-Oct-2002 (hanje04)
**          BUG 108823
**          Include duu.h so that the duu functions are correctly prototyped.
**          Having a default return type of i4 was causing problems on 64bit
**          Linux (and potentially others) where there was no prototype and 
**          the function was returning a char *.
**      07-Apr-2003 (hanch04)
**          Bumped release id to external "II 2.65" internal 865 or 8.65
**      13-Jan-2004 (sheco02)
**          Bumped release id to external "II 3.0" internal 900 or 9.0
**      07-apr-2005 (stial01)
**          Added new iidbcapabilities rows.
**      08-apr-2005 (stial01)
**          Another new iidbcapabilities row.
*/
/*
[@forward_type_references@]
[@forward_function_references@]
[@#defines_of_other_constants@]
[@type_definitions@]
[@global_variable_definitions@]
[@static_variable_or_function_definitions@]
*/
#define		DB_ERROR	17
#define		IGNORE_FE	18
GLOBALREF char  iiduNoFeClients[]; /* special client name*/
FUNC_EXTERN	DB_STATUS	dui_stdcats();
FUNC_EXTERN	char	*duu_xflag();
FUNC_EXTERN	STATUS	dui_bldDefClientList();
FUNC_EXTERN	DB_STATUS	dui_stdcts();
FUNC_EXTERN	DB_STATUS	dui_femake();
static	bool	dui_scats_exist();
/*{
** Name: dui_makedb()	- make all of the frontend system catalogs.
**
** Description:
**        This routine makes and configures all of the frontend system and
**	standard catalogs that are required at "createdb time".  It assumes that
**	that the database has already been opened with the appropriate
**	priveledges, and that the core system catalogs have already been made.
**
**	It is assumed that the calling routine is in a transaction is this
**	routine is called.  If the caller does not want transacation semantics,
**	the caller should call subordinate routines directly, using a false
**	for transaction_flag.
**
** Inputs:
**	gateway				Which (if any) Gateway this is for.
**      duc_errcb                       Ptr to DUF error-handling control block.
**
**	duc_modes			Contains info:
**	    .duc_client_name		    any client names specified via -f
**	    .duc_star_flag		    boolean.  True if DDB being built
**      duc_dbenv                       Ptr to DU_ENV
**
** Outputs:
**      *duc_errcb                      If an error occurs, this struct is set
**					by a call to du_error().
**	Returns:
**	    E_DU_OK			Completed successfully.
**	Exceptions:
**	      Any equel errors that occur, will be signaled and caught by
**	    the Ingres error handler, duc_ingerr_handler().  Note, this may
**	    cause program termination.
**
** Side Effects:
**	      Creates the extended system catalogs that are controlled by
**	    the frontend group.
**
** History:
**      13-Apr-87 (ericj)
**          Initial creation (for Sandyd).
**	15-apr-87 (sandyd)
**	    Changed "rcosection" and "rcocommand" back to char(12).
**	20-apr-87 (sandyd)
**	    Moved "modifies" into "duimod.qc" module.
**	    Removed "define permit" since that is not supported on catalogs now.
**	23-apr-87 (sandyd)
**	    Added "alter_count" and "last_altered_by" in ii_objects.
**	    Changed "relid" to "depid" in ii_dependencies.
**	24-apr-87 (sandyd)
**	    Changed "cstring" to "estring" in ii_encodings.
**	24-apr-87 (sandyd)
**	    Added "language" to ii_object and ii_longremark.
**	24-apr-87 (sandyd)
**	    Fixed missing comma after "alter_date".
**	11-may-87 (sandyd)
**	    Added "repbcount" to ii_reports.
**	    Change required by standard catalog (gateway) specs:
**	    All required fields limited to 240 bytes.  Also changed
**	    long CHARs to VARCHAR to make compression more likely.
**	    Added call to dui_femod().
**	27-may-87 (sandyd)
**	    Would you believe ... changed ii_encodings.estring back to
**	    1990, and ii_encoded_forms.cfdata back to 1960.  (Frontend
**	    code will dynamically adapt to backends that don't support
**	    those lengths.)  Also changed ii_objects DATE's to CHAR(25)
**	    format, as per Standard Catalog spec.  Also replaced ii_dependencies
**	    with ii_abfdependencies.
**	26-oct-88 (jrb)
**	    Changed extended catalog column names to newly designated names so
**	    that chances for collision with reserved words are reduced.
**	4-Aug-1989 (teg)
**	    dropped in ii_sequence_value (catalog and procedure) for J. Wong
**	    for FE development.
**	18-Sep-1989 (pete)
**	    Added new definition for standard catalog view iialt_columns;
**	    the 6.x version was wrong and didn't work as documented. (bug8110).
**	18-Sep-1989 (pete)
**	    Changed the MODIFY statement for ii_sequence_value to specify
**	    "hash UNIQUE" and "fillfactor = 1". Also change name of this
**	    catalog to be plural (ii_sequence_values), and
**	    change the database procedure _ii_sequence_value
**	    to access the table under the new (plural) name.
**	5-may-1990 (pete)
**	    Add support for "clients" flag (-f).
**	22-may-1990 (pete)
**	    Create standard catalogs before Front-end catalogs, so
**	    "upgradefe" can check who is $ingres by selecting the
**	    owner of the "iitables" standard catalog. (it may need to do that
**	    because it is not supposed to issue a "GRANT ALL ON tblname TO dba"
**	    statement when the dba == $ingres. On Gateways, the owner
**	    of the system catalogs may not be "$ingres").
**	12-jul-1990 (pete)
**	    Remove call to dui_femod -- this routine no longer needed cause
**	    front-end cats are modified by catinstall.
**	4-oct-1990 (teresa/pete)
**	    Add IIlq_Protect(TRUE) following UTexe call. Creation of Star
**	    catalogs was failing before this.
**	03-feb-91 (teresa)
**	    Bypass creation of FE catalogs if star was selected.   The user
**	    needs to use upgradefe upon the newly created star DB to get
**	    Distirbuted FE catalogs.
**	11-apr-91 (pete)
**	    Check if standard catalogs exist before just start creating them.
**	    Assume existing set is correct if they exist and don't try to
**	    create them again.
**	21-jun-91 (teresa)
**	    rearchitected some inline stuff into separate routines so they
**	    may be called directly from createdb.  This permits createdb to
**	    create a distributed DB using a star server for the upgradefe
**	    spawn. (bug 38435)
**	19-sep-91 (pete)
**	    Fix bug 39575. Add return statement to dui_makedb() on successful
**	    completion.
**	12-Jan-93 (jpk)
**	    Added parameter duc_dbenv to dui_stdcts() to
**	    pass the values to deposit in iidbcapabilities
**	    that specify whether this is a FIPS database.
[@history_template@]...
*/
DU_STATUS
dui_makedb(gateway, duc_errcb, duc_modes, duc_dbenv)
    i4			gateway;
    DU_ERROR		*duc_errcb;
    DUC_MODE_VECT	*duc_modes;
    DU_ENV		*duc_dbenv;
{
	STATUS  	stat;
	DU_STATUS	du_stat;
	if (!dui_scats_exist())
	{
	    /* Standard catalogs don't exist. Create them. */
	    if ( dui_stdcts(gateway, duc_errcb, duc_modes, duc_dbenv) != E_DU_OK )
	        return(duc_errcb->du_status);
	}
	/*
	** Create & Modify the Front-end catalogs.
	*/
	if (dui_femake(duc_errcb, duc_modes, TRUE, duc_dbenv) != E_DB_OK)
	    return(duc_errcb->du_status);
	return(E_DB_OK);
}
/*{
** Name: dui_scats_exist	- check if standard catalogs exist.
**
** Description:
**	Check iirelation for existance of the iidbcapabilities catalog.
**	If it exists, then return TRUE; else return FALSE.
**
** Inputs:
**	NONE
**
** Outputs:
**					by a call to du_error().
**	Returns:
**		TRUE	standard catalogs exist.
**		FALSE	standard catalogs don't exist; ok to create them.
** History:
**	11-apr-91 (pete)
**		Initial version.
**	13-nov-93 (rblumer)
**		changed to handle upper or lower-case names,
**		by lower-casing fields before comparison.
*/
static bool
dui_scats_exist()
{
  i4 cnt = 0;
/* # line 502 "duicrdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iirelation where lowercase(relid)='iidbcapabiliti\
es' and lowercase(relowner)='$ingres'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 507 "duicrdb.sc" */	/* host code */
	if (cnt > 0)
	    return TRUE;
	else
	    return FALSE;
}
/*{
** Name: dui_stdcts()	- make the standard catalog interface
**
** Description:
**        This routine makes and configures the standard catalog interface.
**	It is hacked from dui_makedb(), because createdb is being modified
**	to bring up a star server before evoking upgradefe.  The previous
**	architecture coupled the standard and FE catalogs, and occurred
**	before createdb created the star catalogs.  (Createdb needs the
**	Std Cat I/F to create the star catalogs, but the star catalogs must
**	be fully created before upgradefe can be spawned in the event that
**	we are creating a distributed DB.  Since createdb and fecvt60 share
**	modules in this file, the parent module dui_makedb is being left
**	intact but pieces of it are being pulled out into serarate subroutines.
**	These subroutines will be called directly from createdb and via
**	dui_makedb from fecvt60.
**
**	This routine assumes that that the database has already been opened
**	with the appropriate priveledges, and that the core system catalogs have
**	already been made.
**
**	NOTE: this routine does NOT start a transaction before creating
**	the standard catalogs. If the caller wants either all or none of the
**	standard catalogs created, the caller must supply the transaction
**	semantics
**
** Inputs:
**	gateway				Which (if any) Gateway this is for.
**      duc_errcb                       Ptr to DUF error-handling control block.
**      duc_dbenv                       Ptr to DU_ENV
**
** Outputs:
**      *duc_errcb                      If an error occurs, this struct is set
**					by a call to du_error().
**	Returns:
**	    E_DU_OK			Completed successfully.
**	Exceptions:
**	      Any equel errors that occur, will be signaled and caught by
**	    the Ingres error handler, duc_ingerr_handler().  Note, this may
**	    cause program termination.
**
** Side Effects:
**	      Creates the extended system catalogs that are controlled by
**	    the frontend group.
**
** History:
**	21-jun-91 (teresa)
**	    Initial creation from dui_makedb().  (bug 38435)
**	12-Jan-93 (jpk)
**	    Added parameter duc_dbenv to dui_stdcts() to
**	    pass the values to deposit in iidbcapabilities
**	    that specify whether this is a FIPS database.
[@history_template@]...
*/
DU_STATUS
dui_stdcts(gateway, duc_errcb, duc_modes, duc_dbenv)
    i4			gateway;
    DU_ERROR		*duc_errcb;
    DUC_MODE_VECT	*duc_modes;
    DU_ENV		*duc_dbenv;
{
	/* Print informational message. */
	if ((du_error(duc_errcb, I_DU0206_CR_SCI_CATS, 0)) != E_DU_OK )
	    return(duc_errcb->du_status);
	/* create standard catalog interface */
	_VOID_ dui_stdcats(gateway, duc_errcb, duc_modes, duc_dbenv);
	/* modify the standard catalogs */
	_VOID_ dui_scmod(duc_errcb);
	return (E_DU_OK);
}
/*
** Name:	dui_stdcats	- create standard catalogs.
**
** Description:
**	Create the standard catalog views for INGRES version 6.0.
**	These views are defined in the document "A Standard Catalog
**	Interface for 6.1", revision 3.1, dated October, 1988.
**
**	These catalogs are defined for both INGRES and gateways
**	in order to provide a clean abstraction for database
**	information.
**
** Inputs:
**	gateway				which (if any) Gateway this is for.
**	duc_errcb			error cb, in case of trouble.
**      duc_dbenv                       Ptr to DU_ENV
**
** Outputs:
**	none.
**
** Returns:
**	E_DB_OK, unless "impossible" error occurs, in which case E_DB_ERROR.
**
** History:
**	17-jun-1987 (peter)
**	    Written.  The iiviews, iiintegrities, and iipermits tables 
**	    are not defined in final form yet.
**	13-aug-1987 (peter)
**	    Added final support for iiviews, iiintegrities and iipermits.
**	31-aug-1987 (peter)
**	    Change view definitions to parenthesize expressions to
**	    avoid a problem that showed up when using II_DECIMAL
**	    set to comma.
**	02-oct-1987 (peter)
**	    Take out casts on relid and relowner for performance.
**	03-feb-1988 (peter)
**	    Create new iimulti_locations table.  
**	    Change definition of defaultability in iicolumns.
**	03-mar-1988 (peterj)
**	    Update overflow page to be 0 for all btrees.
**	18-aug-1988 (peter)
**	    Add update to iicolumns to fix problem with precision
**	       for decimal.
**	    Add iialternate_keys, iialt_columns, iiprocedures.
**	    Change definition of iipermits to reflect permissions
**	       on procedures.
**	    Update names of many columns to reflect LRC version
**	       to avoid name conflict in future.
**	    Add table_subtype to iitables.
**	13-sep-1988 (teg)
**	    fixed definition of column duplicate_rows of iiphysical_tables
**	03-Oct-1988 (teg)
**	    added sql definition for iifile_info
**	03-Oct-1988 (bruceb)
**	    added quel definition for ii_loghelp
**	03-nov-1988 (jrb)
**	    changed name of ii_loghelp to iilog_help so that it wouldn't be
**	    considered an extended catalog
**	15-nov-1988 (jrb)
**	    Changed several catalogs to reflect LRC changes.  `cap_description'
**	    was removed from `iidbcapabilities', `proc_subtype' was added to
**	    `iiprocedures', and seven new rows were added to the
**	    iidbcapabilities table.
**	19-nov-1988 (jrb)
**	    Changed iitables and iicolumns to iiotables and iiocolumns
**	    respectively.  Added new iitables and iicolumns views as defined
**	    by the LRC.  Removed iialternate_keys view.
**	25-nov-1988 (jrb)
**	    Changed definition of expire_date in iiingres_tables so that it
**	    works correctly.
**	09-mar-1989 (teg)
**	    Fixed iifile_info view defination to also include file at default
**	    location of table with multiple locations.
**	19-may-1989 (neil)
**	    Added "iirules" view for TM support.
**	20-jun-89 (ralph)
**	    Added MAX_COLUMNS to iidbcapabilities
**	26-jun-1989 (mikem)
**	    Added new columns "column_internal_datatype", 
**	    "column_internal_length", and "column_system_maintained" to 
**	    iicolumns standard catalog.  Also added new capabilities 
**	    INGRES_LOGICAL_KEY, INGRES_RULES, INGRES_UDT, INGRES_AUTH_GROUP, 
**	    and INGRES_AUTH_ROLE.
**	14-jul-1989 (ralph)
**	    Changed the following catapbilities for terminator:
**		QUEL_LEVEL was "ING6.0" now "ING7.0"
**		SQL_LEVEL  was "ING6.0" now "ING7.0"
**		INGRES/SQL_LEVEL  was "00601" now "00700"
**		INGRES/QUEL_LEVEL was "00601" now "00700"
**		COMMON/SQL_LEVEL  was "00601" now "00700"
**	18-aug-1989 (fred)
**	    Changed definition of iicolumns so that the column_internal_length
**	    column specified the user length rather than the internal length.
**	12-sep-89 (ralph)
**	    B1 Enhancements:
**		Changed iipermits definition to exclude security alarm tuples.
**		Added iisecurity_alarms view definition.
**	22-sep-1989 (teg)
**	    As per Steering Committee decision, ingres 7.0 is now ingres 6.3,
**	    so Changed the following catapbilities for terminator:
**		QUEL_LEVEL was "ING7.0" now "ING6.3"
**		SQL_LEVEL  was "ING7.0" now "ING6.3"
**		INGRES/SQL_LEVEL  was "00700" now "00603"
**		INGRES/QUEL_LEVEL was "00700" now "00603"
**		COMMON/SQL_LEVEL  was "00700" now "00603"
**	06-jan-1990 (alexc)
**	    Create Views: iidb_comments, iidb_subcomments, iicommenttext,
**		and iisynonym for COMMENTs and SYNONYMS.
**	20-mar-1990 (fourt)
**	    Add define of view iiregistrations, one of the standard catalogs
**	    that we'll be needing for Gateways.
**      19-apr-1990 (alexc)
**          iitables, iiphysical_tables, and iiingres_tables have incorrect
**              initialization values = "N ", correct values is = "NY" in all
**              cases.
**	27-apr-1990 (alexc)
**	    Add the following rows to iidbcapabilities for bug fix(8887)
**		UNIQUE_KEY_REQ	N
**		ESCAPE		Y
**		OWNER_NAME	QUOTED
**	27-apr-1990 (alexc)
**	    Modify iiregistrations to conform with change from GW on
**		object_subtype and qualification for create View.
**	10-may-90 (fourt)
**	    Generalize setup of 'iidbcapabilities' to make entries dependent
**	    on which (if any) Gateway we're using.
**	23-may-90 (edwin)
**	    Slightly revise definition of iiregistrations to reflect retention
**	    of token DB_CRT_LINK.
**	25-may-90 (edwin)
**	    No more call to duc_abort_internal(), to fix link problems.
**	29-may-90 (teg)
**	    fixed alexc's bugfix 8887, which was really a fix for bug 8885,
**	    not 8887.  Alexc intended to set ESCAPE to Y, but set it to N
**	    instead.  I corrected it to "Y".
**	10-dec-90 (neil)
**	    Alerters: Add "iievents" and modified "iipermits" to use "iievent".
**	20-feb-91 (teresa)
**	    add system_owner to iidbconstants, as per approved LRC proposal.
**	25-mar-1991 (bryanp)
**	    Added support for Btree index compression: new column added to
**	    iitables, iiphysical_tables, and iiindexes views:
**		"key_is_compressed" (values are "y", "n").
**	    Also picked up some miscellaneous catalog changes from the Front
**	    Ends group (Pete):
**		ING6.3 ==> ING6.4, 00603 ==> 00604 (iidbcapabilities)
**		correct the definition of iiphysical_tables.duplicate_rows
**		New column column_updateable in iicolumns.
**		sort_direction in iiindex_columns no longer hard-coded.
**	29-apr-91 (teresa)
**		add standard_catalog_level to iidbcapabilities
**	09-nov-92 (rganski)
**		Added 4 new fields to iistats: column_domain, is_complete,
**		stat_version and hist_data_length. These correspond to existing
**		sdomain and scomplete columns and new sversion and
**		shistlength columns of iistatistics. For Character Histogram
**		Enhancements project.
**	09-nov-92 (rickh,rblumer)
**	  FIPS Constraints.
**	    Added new fields to iitables, iicolumns, iiindexes, iirules,
**	    iiintegrities, and iiprocedures.  Moved the code into
**	    appropriately named duc* routines.  Added iiconstraint_indexes
**	    view.
**	    Added iidefaults, iiconstraints, iikeys, & iiref_constraints views.
**	13-dec-92 (jpk)
**		added iiaccess standard catalog.
**	12-Jan-93 (jpk)
**	    Added parameter duc_dbenv to get the values to deposit in
**	    iidbcapabilities that specify whether this is a FIPS database.
**	    Added corresponding support to deposit values.
**	14-Jan-93 (jpk)
**	    modify definition of iiotables, iiocolumns, iiingres_tables
**	    to use gmt_timestamp
**	05-dec-92 (rblumer)
**	    removed call to duc_v_iidefaults; the view is no longer needed.
**	9-Feb-1993 (jpk)
**	    Encapsulate definition of iiphysical_tables so that upgradedb can
**	    call the same function that createdb calls.
**	22-May-1993 (jpk)
**	    COMMON/SQL_LEVEL is now deprecated; OPEN/SQL_LEVEL is preferred.
**	    Append a tuple for the latter to iidbcapabiilities.
**	16-Jul-1993 (jpk)
**	    Added iidbcapabilities tuple DB_REAL_USER_CASE.
**	9-sep-93 (robf)
**	   Add INGRES_MAC_LEVEL 
**	13-nov-93 (rblumer)
**	    Changed iidbconstants to use upper/lower case system_owner
**	    as appropriate for the current case semantics.
**	18-nov-93 (rblumer)
**	    Changed iiindex_columns and iialt_columns to filter out upper-case
**	    tidp columns in addition to lower-case tidp columns.
**	    This fixes Star bug 56239.
**	2-dec-93 (robf)
**         iistats and iihistograms are empty to those without table_statistics
**	   privilege. Note we allow unless denied for backwards compatibility.
**	6-dec-93 (robf)
**         Tell ii_v4_security_alarms whether we are creating the iidbdb  or
**	   just a regular database.
**	16-Feb-1994 (jpk)
**	    Moved iistats view definition to separate function in ducommon
**	    so upgradedb can call it.
**	11-jul-94 (robf)
**          Moved iievents view definition to seperate function in  ducommon
**          so upgradedb can call it.
**	14-jul-94 (robf)
**          Moved iialt_columns view definition to seperate function in
**	    ducommon so upgradedb can call it.
**	 4-apr-96 (nick)
**	    Move definition of iimulti_locations to its own function.
**      26-apr-1996 (musro02)
**          Changed table_version from ING6.0 to OPING1.2
**      18-jul-1996 (hanch04)
**          Changed table_version from OPING1.2 to OI2.0
**	09-nov-1998 (mcgme01)
**	    Changed "OI2.0" to "II2.5"
**	01-jun-1999 (kitch01)
**	   Bug 97040. Amend the iilog_help view to use gmt_timestamp function
**     to make it Y2K compliant. Also move the creation to a new function
**     duc_v_iiloghelp in ducommon so that upgradedb can use it to rectify
**	   older databases. This is an internal view that is no longer used.
**	2-nov-00 (inkdo01)
**	    Changed iihistograms view creation to external call (consistent
**	    with sibling view iistats).
**	18-May-01 (gordy)
**	    Added NATIONAL_CHARACTER_SET row to iidbcapabilities.
**	7-Sep-2004 (schka24)
**	    Make iiotables and iiocolumns "real" std cats so upgradedb
**	    can recreate them.  Remove hand-coded definitions here.
*/
DU_STATUS
dui_stdcats(gateway, duc_errcb, duc_modes, duc_dbenv)
i4		gateway;
DU_ERROR	*duc_errcb;
DUC_MODE_VECT   *duc_modes;
  DU_ENV *duc_dbenv;
{
	i4	col, nchar_set;
	/*
	**  We only have entries here for capabilities in iidbcapabilities for
	**  which the RMS Gateway has a non-standard value.  In the future,
	**  e.g. for new Gateways with new odd capabilities, this can be 
	**  added to.  This stuff is indexed by 'col'.
	*/
  static char *cap_ingres[]= { "Y", "N" };
  static char *cap_savepoints[]= { "Y", "N" };
  static char *cap_dbms_type[]= { "INGRES", "RMS" };
  static char *cap_nchar_set[]= { "N", "Y" };
  char owner[DB_MAXNAME+1];
	nchar_set = *duc_dbenv->du_ucollation ? 1 : 0;
	/* pick out column for whichever Gateway we're for */
	switch (gateway)
	{
	  case  DU_GW_NONE:
		col = 0;
		break;
	  case  DU_GW_RMS_FLAG:
		col = 1;
		break;
	  default:	/* "impossible" to arrive here */
		if ((du_error(duc_errcb, E_DU2013_GW_STDCATS_INT_ERR, 0))
			!= E_DU_OK)
				return(duc_errcb->du_status);
		return(E_DB_ERROR);
	}
	IIlq_Protect(TRUE);
/* # line 876 "duicrdb.qsc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create iidbcapabilities(cap_capability=char(32), cap_value=char(32))");
    IIsyncup((char *)0,0);
  }
/* # line 879 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=");
    IIputdomio((short *)0,1,32,10,DU_DB1_QUEL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)", cap_value=");
    IIputdomio((short *)0,1,32,7,DU_DB1_CUR_QUEL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 882 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=");
    IIputdomio((short *)0,1,32,9,DU_DB2_SQL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)", cap_value=");
    IIputdomio((short *)0,1,32,7,DU_DB2_CUR_SQL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 885 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"DISTRIBUTED\", cap_value=\
\"N\")");
    IIsyncup((char *)0,0);
  }
/* # line 888 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"MIXEDCASE_NAMES\", cap_va\
lue=\"N\")");
    IIsyncup((char *)0,0);
  }
/* # line 891 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"INGRES\", cap_value=");
    IIputdomio((short *)0,1,32,0,cap_ingres[col]);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 894 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=");
    IIputdomio((short *)0,1,32,16,DU_DB3_INGSQL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)", cap_value=");
    IIputdomio((short *)0,1,32,5,DU_DB3_CUR_INGSQL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 897 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=");
    IIputdomio((short *)0,1,32,16,DU_DB4_CMNSQL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)", cap_value=");
    IIputdomio((short *)0,1,32,5,DU_DB4_CUR_CMNSQL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 900 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=");
    IIputdomio((short *)0,1,32,17,DU_DB5_INGQUEL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)", cap_value=");
    IIputdomio((short *)0,1,32,5,DU_DB5_CUR_INGQUEL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 903 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SAVEPOINTS\", cap_value=");
    IIputdomio((short *)0,1,32,0,cap_savepoints[col]);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 906 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"DBMS_TYPE\", cap_value=");
    IIputdomio((short *)0,1,32,0,cap_dbms_type[col]);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 909 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"PHYSICAL_SOURCE\", cap_va\
lue=\"T\")");
    IIsyncup((char *)0,0);
  }
/* # line 912 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"MAX_COLUMNS\", cap_value=\
\"1024\")");
    IIsyncup((char *)0,0);
  }
/* # line 915 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"INGRES_RULES\", cap_value\
=\"Y\")");
    IIsyncup((char *)0,0);
  }
/* # line 918 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"INGRES_UDT\", cap_value=\"\
Y\")");
    IIsyncup((char *)0,0);
  }
/* # line 921 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"INGRES_AUTH_GROUP\", cap_\
value=\"Y\")");
    IIsyncup((char *)0,0);
  }
/* # line 924 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"INGRES_AUTH_ROLE\", cap_v\
alue=\"Y\")");
    IIsyncup((char *)0,0);
  }
/* # line 927 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"INGRES_LOGICAL_KEY\", cap\
_value=\"Y\")");
    IIsyncup((char *)0,0);
  }
/* # line 930 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"UNIQUE_KEY_REQ\", cap_val\
ue=\"N\")");
    IIsyncup((char *)0,0);
  }
/* # line 933 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"ESCAPE\", cap_value=\"Y\"\
)");
    IIsyncup((char *)0,0);
  }
/* # line 936 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"OWNER_NAME\", cap_value=\"\
QUOTED\")");
    IIsyncup((char *)0,0);
  }
/* # line 939 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=");
    IIputdomio((short *)0,1,32,22,DU_DB6_STDCAT_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)", cap_value=");
    IIputdomio((short *)0,1,32,5,DU_DB6_CUR_STDCAT_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 942 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=");
    IIputdomio((short *)0,1,32,14,DU_DB7_OPENSQL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)", cap_value=");
    IIputdomio((short *)0,1,32,5,DU_DB7_CUR_OPENSQL_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 945 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"DB_NAME_CASE\", cap_value\
=");
    IIputdomio((short *)0,1,32,0,duc_dbenv->du_reg_id_case);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 948 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"DB_DELIMITED_CASE\", cap_\
value=");
    IIputdomio((short *)0,1,32,0,duc_dbenv->du_delim_id_case);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 951 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"DB_REAL_USER_CASE\", cap_\
value=");
    IIputdomio((short *)0,1,32,0,duc_dbenv->du_real_user_case);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 954 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"NATIONAL_CHARACTER_SET\",\
 cap_value=");
    IIputdomio((short *)0,1,32,0,cap_nchar_set[nchar_set]);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 957 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_BYTE_LITERAL_LEN\"\
, cap_value=\"32000\")");
    IIsyncup((char *)0,0);
  }
/* # line 960 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_CHAR_LITERAL_LEN\"\
, cap_value=\"32000\")");
    IIsyncup((char *)0,0);
  }
/* # line 963 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_BYTE_COLUMN_LEN\"\
, cap_value=\"32000\")");
    IIsyncup((char *)0,0);
  }
/* # line 966 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_VBYT_COLUMN_LEN\"\
, cap_value=\"32000\")");
    IIsyncup((char *)0,0);
  }
/* # line 969 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_CHAR_COLUMN_LEN\"\
, cap_value=\"32000\")");
    IIsyncup((char *)0,0);
  }
/* # line 972 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_VCHR_COLUMN_LEN\"\
, cap_value=\"32000\")");
    IIsyncup((char *)0,0);
  }
/* # line 975 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_NCHR_COLUMN_LEN\"\
, cap_value=\"16000\")");
    IIsyncup((char *)0,0);
  }
/* # line 978 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_NVCHR_COLUMN_LEN\"\
, cap_value=\"16000\")");
    IIsyncup((char *)0,0);
  }
/* # line 981 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_SCHEMA_NAME_LEN\"\
, cap_value=\"32\")");
    IIsyncup((char *)0,0);
  }
/* # line 984 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_TABLE_NAME_LEN\",\
 cap_value=\"32\")");
    IIsyncup((char *)0,0);
  }
/* # line 987 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_COLUMN_NAME_LEN\"\
, cap_value=\"32\")");
    IIsyncup((char *)0,0);
  }
/* # line 990 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_USER_NAME_LEN\", \
cap_value=\"32\")");
    IIsyncup((char *)0,0);
  }
/* # line 993 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_ROW_LEN\", cap_va\
lue=\"262144\")");
    IIsyncup((char *)0,0);
  }
/* # line 996 "duicrdb.qsc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append to iidbcapabilities(cap_capability=\"SQL_MAX_STATEMENTS\", cap\
_value=\"0\")");
    IIsyncup((char *)0,0);
  }
/* # line 1000 "duicrdb.qsc" */	/* host code */
	if (duc_dbenv->du_dbservice & DU_NAME_UPPER)
	    MEmove(sizeof(ERx("$ingres")) - 1, (PTR) ERx("$INGRES"), ' ',
		   sizeof(owner), owner);
	else
	    MEmove(sizeof(ERx("$ingres")) - 1, (PTR) ERx("$ingres"), ' ',
		   sizeof(owner), owner);
	owner[sizeof(owner)-1] = EOS;
	/* Create all standard catalog views */
	duc_create_stdcat_views(duc_dbenv->du_dbname, 
		(duc_dbenv->du_dbservice & DU_NAME_UPPER) ? TRUE : FALSE);
	return(E_DB_OK);
} /* end dui_stdcats */
/*{
** Name: dui_femake()	- make all of the frontend system catalogs.
**
** Description:
**        This routine makes and configures all of the frontend system
**	catalogs that are required at "createdb time".  It assumes that
**	that the database has already been opened with the appropriate
**	priveledges, and that the core system catalogs and standard
**	catalog interface have already been made.
**
**	It is hacked from dui_makedb(), because createdb is being modified
**	to bring up a star server before evoking upgradefe.  The previous
**	architecture coupled the standard and FE catalogs, and occurred
**	before createdb created the star catalogs.  (Createdb needs the
**	Std Cat I/F to create the star catalogs, but the star catalogs must
**	be fully created before upgradefe can be spawned in the event that
**	we are creating a distributed DB.  Since createdb and fecvt60 share
**	modules in this file, the parent module dui_makedb is being left
**	intact but pieces of it are being pulled out into serarate subroutines.
**	These subroutines will be called directly from createdb and via
**	dui_makedb from fecvt60.
**
**	This routine assumes that that the database has already been opened
**	with the appropriate priveledges, and that the core system catalogs have
**	already been made.
**
**	NOTE: this routine does NOT start a transaction before creating
**	the standard catalogs. If the caller wants either all or none of the
**	standard catalogs created, the caller must supply the transaction
**	semantics.
**
** Inputs:
**      duc_errcb                       Ptr to DUF error-handling control block.
**	duc_modes			Contains info:
**	    .duc_client_name		    any client names specified via -f
**	    .duc_star_flag		    boolean.  True if DDB being built
**	transaction_flag		Boolean:  true->caller is using
**						transacations
**	duc_dbenv			Contains environmental info:
**	    .du_ddbname			    name of distributed db
**	    .du_cdbname			    name of coordinator db
**	    .du_dbname			    name of DB that we are connected to
**					     (CDB when connencted via local 
**					      server and DDB when connected to
**					      star server)
** Outputs:
**      *duc_errcb                      If an error occurs, this struct is set
**					by a call to du_error().
**	Returns:
**	    E_DU_OK			Completed successfully.
**	Exceptions:
**	      Any equel errors that occur, will be signaled and caught by
**	    the Ingres error handler, duc_ingerr_handler().  Note, this may
**	    cause program termination.
**
** Side Effects:
**	      Creates the extended system catalogs that are controlled by
**	    the frontend group.
**
** History:
**	21-jun-91 (teresa)
**	    creation by lifting from dui_makedb so that this code may be 
**	    shared between createdb (being rearchitected to connect to a star
**	    server before spawning upgradefe) and fecvt60, which does not
**	    deal with DDBs and CDBs.   (bug 38435)
**      08-May-2000 (hweho01)
**          Fixed the mismatch of type in the arg list of du_error() call. 
[@history_template@]...
*/
/*
** NOTE:
**      'basecats' & 'silent' do not have a format in the utexe.def file,
**      but we have to provide a dummy argument, or UTexe won't
**      parse them.  The argument to the %S ("") gets dropped on the floor.
*/
static	char	ut_args[] =
    ERx("database = %S, equel = %S, user = %S, basecats = %S");
static  char	Fecatins[] = ERx("upgradefe");	/* FE dictionary creat*/
static  char	Empty_str[]    = ERx("");
DU_STATUS
  dui_femake(duc_errcb, duc_modes, transaction_flag, duc_dbenv)
    DU_ERROR		*duc_errcb;
    DUC_MODE_VECT	*duc_modes;
    bool		transaction_flag;
    DU_ENV		*duc_dbenv;
  {
	char		buf[256];
	STATUS  	stat;
	DU_STATUS	du_stat;
	CL_ERR_DESC	err_code;
	bool		fecats = TRUE;
	i4		cnt_silent = 0;	/* set to 1 if -s flag for UTexe */
	i4		cnt_client = 0; /* set to 1 if client list for UTexe */
	PTR		p_vararg1 = NULL;	/* passed in UTexe call */
	PTR		p_vararg2 = NULL;	/* passed in UTexe call */
	/*
	** Create & Modify the Front-end catalogs.
	*/
	/* check for client names on createdb command line */
	if (duc_modes->duc_client_name[0] == EOS)
	{
	    /* no client names on cmd line; create client
	    ** catalogs based on user's authorization string. We 
	    ** will create front-end catalogs for everything they
	    ** are authorized to run.
	    ** Also, pass the "-s" flag to "upgradefe".
	    */
	    char clist[DU_MAX_CLIENT_NAME];
	    if (dui_bldDefClientList(clist, duc_errcb) == OK)
	    {
		STcopy (clist, duc_modes->duc_client_name);
		STpolycat (2, ut_args, ERx(", silent = %S, client = %S"),
				     buf);
		cnt_silent = 1;
		cnt_client = 1;
		p_vararg1 = (PTR) Empty_str;
		p_vararg2 = (PTR) &duc_modes->duc_client_name[0];
	    }
	    else
	    {
		/* Problems locating or reading file "prods.lst". Issue
		** warning message & continue.
		*/
		du_error(duc_errcb, W_DU1081_CANT_READ_PRODS_LST, 0);
		/* create basecats only. */
		STcopy (ut_args, buf);
		STpolycat (2, ut_args, ERx(", silent = %S"), buf);
		cnt_silent = 1;
		p_vararg1 = (PTR) Empty_str;
	    }
	}
	else if (STcasecmp(duc_modes->duc_client_name, iiduNoFeClients) == 0)
	{
		/* "nofeclients" passed to CREATEDB. Don't create Front-end
		** catalogs; skip call to upgradefe.
		*/
		fecats = FALSE;
	}
	else
	{
	    /* List of client names was passed to CREATEDB.
	    ** Pass this same list of client names on to upgradefe.
	    ** Do NOT pass the "-s" flag.
	    */
	    STpolycat (2, ut_args, ERx(", client = %S"), buf);
	    cnt_client = 1;
	    p_vararg1 = (PTR) &duc_modes->duc_client_name[0];
	}
	if (fecats)
	{
	    /* Print an informational message. */
	    if ((du_error(duc_errcb, I_DU0205_CR_FE_CATS, 0)) != E_DU_OK )
	        return(duc_errcb->du_status);
	    if (duc_modes->duc_star_flag)
	    {
		DB_STATUS	star_stat;
		star_stat = use_star(TRUE,transaction_flag,duc_dbenv,duc_errcb);
		switch (star_stat)
		{
		case E_DU_OK:
		    /* star server is up OK */
		    break;
		case DB_ERROR:
		    /* could not connect to star server and could not connect
		    ** back to local server */
		    du_error(duc_errcb, E_DU2100_BAD_RECONNECT, 0);
		    return(duc_errcb->du_status);
		case IGNORE_FE:
		    /* could not connect to star server, but was able to 
		    ** connect back to local server, so skip upgradefe call */
		    if ((du_error(duc_errcb, W_DU1084_NO_FECAT_CRE, 2,
			0, duc_dbenv->du_ddbname)) 
		    != E_DU_OK )
			return(duc_errcb->du_status);
		    fecats=FALSE;
		    break;
		}		
	    }
	    if (fecats)
	    {
		/* Create and modify the front-end catalogs by spawning
		** a call to the executable "catinstall" with a list of
		** front-end client names. Catinstall will create the
		** catalogs needed by those clients.
		** This call has 4 fixed + 2 variable arguments.
		*/
		stat = UTexe (UT_WAIT|UT_STRICT, NULL, (ModDictDesc *)NULL,
			    NULL, Fecatins, &err_code, buf,
 			    (4 + cnt_silent + cnt_client),
			    duc_dbenv->du_dbname,
			    duu_xflag(duc_dbenv->du_dbname),
			    duc_dbenv->du_usrinit,
			    Empty_str,		/* basecats (fixed arg) */
			    p_vararg1,		/* silent or client */
			    p_vararg2,		/* client */
			    NULL
			 );
		if (stat != OK)
		{
		    /* something went wrong. stop createdb.	*/
		    /* save CL err info from UTexe so "du_error" can see it */
		    STRUCT_ASSIGN_MACRO (err_code, duc_errcb->du_clsyserr);
		    du_error(duc_errcb, E_DU3511_NOCR_FE_CATS, 2,
			     0, Fecatins);
		    return(duc_errcb->du_status);
		}
		else
		{
		    /* Above UTexe call had side effect of turning off
		    ** catalog update privilege. Turn it back on.
		    */
		    IIlq_Protect(TRUE);
		}
	    }
	    if ( (duc_modes->duc_star_flag) && (fecats) )
	    {
		DB_STATUS	star_stat;
		/* we need to close the star session and open a local one */
		star_stat =use_star(FALSE,transaction_flag,duc_dbenv,duc_errcb);
		if (stat == DB_ERROR)
		{
		    du_error(duc_errcb, E_DU2101_BAD_DISCONNECT, 0);
		    return(duc_errcb->du_status);		    
		}
		else
		    IIlq_Protect(TRUE);
	    }
	}
	return (E_DU_OK);
  }
/*{
** Name: use_star()	- Connect or disconnet from a star server
**
** Description:
**
**        This routine connects or disconnects from a star server.  If
**	it is called to connect to a star server, then it expects a session
**	to be open to the local server, which it will commit if the 
**	in_transaction flag is true and NOT commit if in_transaction is false.
**	It will disconnect from the local server and will then attempt to 
**	connect to the star server as the DBA.  If it cannot make a connection 
**	to the star server, it will try to reconnect to the local server.  If
**	it connects back to the local server, it will return IGNORE_FE, which
**	should prevent the caller from spawning upgradefe.  If it cannot
**	reconect to the local server, it will generate an error message
**	and return DB_ERROR.  It will NOT use an error handler that automatically
**	terminates processing because it is called from either of two different
**	executables and does not know which exit handler to return to.
**
**	If this routine succeeds in connnecting to a star server, it will
**	overwrite the cdb name in duc_dbenv->du_dbname with the DDB name.
**	Thus any exit/error handler can tell if we are connected to star by 
**	testing to see if duc_dbenv->du_dbname is identical to 
**	duc_dbenv->du_ddbname.
**
**	If start_session is false, then use_star assumes this is a request
**	to terminate the star session and reestablish the local session.  So,
**	use_star will attempt to commit the star session (guarenteed to be in
**	a transaction since it was opened as an sql session) and then it will
**	close the star session and attempt to open a new session to the LDB.
**	If use_star() cannot get a new session, it returns DB_ERROR.  Otherwise,
**	it copies the CDB name back to duc_dbenv->du_dbname so error/exit 
**	handlers can know that they are attached to a dbms server rather than to
**	a star server.  Use_star() gets system catalog update priv and then
**	determines whether or not to turn off the SQL transaction by examining
**	the in_transaction flag.
**
** Inputs:
**	start_session			BOOLEAN:  TRUE=>calling routine wants to
**							establish star session
**						  FALSE=>calling routine wants
**							to close star session
**	in_transaction			BOOLEAN:  TRUE=>calling routine was
**							in a transaction
**						  FALSE=>no transaction for 
**							calling routine
**      duc_errcb                       Ptr to DUF error-handling control block.
**	duc_dbenv			Contains environmental info:
**	    .du_usrinit			    user name (dba if du_xusrname is
**						unspecified)
**	    .du_xusrname		    -U user name (if specified)
**	    .du_ddbname			    name of distributed db
**	    .du_cdbname			    name of coordinator db
**	    .du_dbname			    name of DB that we are connected to
**					     (CDB when connencted via local 
**					      server and DDB when connected to
**					      star server)
** Outputs:
**      *duc_errcb                      If an error occurs, this struct is set
**					by a call to du_error().
**	Returns:
**	    E_DU_OK			Completed successfully.
**	    DB_ERROR			Error that is NOT trapped by INGRES or
**					    SQL handler
**	    IGNORE_FE			Unable to connect to STAR server, but
**					    was able to reestablish connection
**					    to local server
**	Exceptions:
**	      Any equel errors that occur, will be signaled and caught by
**	    the Ingres error handler, duc_ingerr_handler().  Note, this may
**	    cause program termination.
**
** Side Effects:
**	      Changes which type of server the utility uses to connect to the
**	target database.
**
** History:
**	21-jun-91 (teresa)
**	    Initial creation ( for bug 38435)
**	13-nov-93 (rblumer))
**	    changed to use new du_xusrname field.
*/
DU_STATUS
use_star(start_session, in_transaction, duc_dbenv, duc_errcb)
bool		start_session;
bool		in_transaction;
DU_ERROR	*duc_errcb;
DU_ENV		*duc_dbenv;
{
  char cdb_name[DB_MAXNAME+DU_NULL_LEN];
  char ddb_name[DB_MAXNAME+DU_TAIL_LEN];
  char dba_name[DB_MAXNAME+DU_NULL_LEN];
  char appl_flag[DU_APPL_FLAG_MAX];
	i4	    i;
	char	    tmp_buf[DU_APPL_FLAG_MAX];
	DB_STATUS   retstat;
	/*************************/
	/* init key info for sql */
	/*************************/
	/* build ddb name */
	STcopy (duc_dbenv->du_ddbname, ddb_name);
	i = STtrmwhite(ddb_name);
        STcopy(DU_STARTAIL,&ddb_name[i]);
	/* get cdb name and Dba name */
	STcopy (duc_dbenv->du_cdbname, cdb_name);
	if (duc_dbenv->du_xusrname[0] == '\0')
	    STcopy (duc_dbenv->du_usrinit, dba_name);
	else
	    STcopy (duc_dbenv->du_xusrname, dba_name);
	/* build appliation flag */
	CVla((i4) DBA_CREATEDB, tmp_buf);
	(VOID)STpolycat(2, DU_1APPL_FLAG, tmp_buf,appl_flag);
	/* disable error handler and handle each error manually */
/* # line 1525 "duicrdb.sc" */	/* host code */
	if (start_session)
	{
	    /* disconnect from local server and connect to star server.
	    ** if we cannot connect to star server, then print a warning that
	    ** user will need to run upgradefe manually.  Otherwise, return
	    ** connected the.
	    */
	    if (in_transaction)
	    {
/* # line 1534 "duicrdb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1535 "duicrdb.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
		    /* There was an error committing the transaction */
		    du_error(duc_errcb, E_DU2103_BAD_COMMIT, 0);
		    return DB_ERROR;
		}
	    }
/* # line 1542 "duicrdb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 1543 "duicrdb.sc" */	/* host code */
	    duc_dbenv->du_dbopen = FALSE;
	    if (sqlca.sqlcode < 0)
	    {
		/* There was an error disconnecting from the database */
		du_error(duc_errcb, E_DU2105_BAD_LOCALDISCONNECT, 0);
		return DB_ERROR;
	    }
/* # line 1550 "duicrdb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqUser(dba_name);
    IIsqConnect(0,ddb_name,appl_flag,(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 1552 "duicrdb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		/* we could not connect to the  star server, so attempt to
		** reconnect to the local server and give up on FE catalogs */
/* # line 1556 "duicrdb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqUser(dba_name);
    IIsqConnect(0,cdb_name,appl_flag,(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 1558 "duicrdb.sc" */	/* host code */
                if (sqlca.sqlcode == 0)
                {
		    /* print warning */
		    du_error(duc_errcb, W_DU1083_BAD_STARCONNECT, 0);
		    duc_dbenv->du_dbopen = TRUE;
		    retstat=IGNORE_FE;
		}
		else
		{
		    retstat=DB_ERROR;		    
		}		
	    }
	    else
	    {
		/* indicate that the DDB is now open */
		duc_dbenv->du_dbopen = TRUE;
		STcopy (duc_dbenv->du_ddbname,duc_dbenv->du_dbname);
		retstat = E_DU_OK;
	    }
	}
	else
	{
	    /* request to close star server session and open local server
	    ** session */
/* # line 1582 "duicrdb.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1583 "duicrdb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 1584 "duicrdb.sc" */	/* host code */
	    duc_dbenv->du_dbopen = FALSE;
	    if (sqlca.sqlcode < 0)
	    {
		/* There was an error disconnecting from the star database */
		du_error(duc_errcb, E_DU2102_BAD_STARDISCONNECT, 0);
		return DB_ERROR;
	    }
/* # line 1591 "duicrdb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqUser(dba_name);
    IIsqConnect(0,cdb_name,appl_flag,(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 1593 "duicrdb.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		/* we could not reconnect to the local server */
		du_error(duc_errcb, E_DU2104_BAD_LOCALCONNECT, 0);
		retstat = DB_ERROR;
	    }
	    else
	    {
		/* we are back to the Local server.  Reset the db name from
		** the ddb name to the CDB name.  If the caller does not want
		** to be in a transaction, then set autocommit off */
		duc_dbenv->du_dbopen = TRUE;
		STcopy (duc_dbenv->du_cdbname,duc_dbenv->du_dbname);
		if  (! in_transaction )
/* # line 1607 "duicrdb.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set autocommit off");
    IIsyncup((char *)0,0);
  }
/* # line 1608 "duicrdb.sc" */	/* host code */
		retstat = E_DU_OK;
	    }
	}
	return(retstat);
}
