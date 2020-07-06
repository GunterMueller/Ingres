# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
**      All rights reserved.
*/
/*
** Name:        ui.qsh -  Front-End DBMS Utilities Interface Definitions.
**
** Description:
**      Interface definitions for the front-end DBMS utilities, UI, which
**      should be included whenever accessing procedures in UI library.
**
** History:
**	Revision 6.4  91/05/05  wong
**	Added capability version levels, UI_LEVEL_*.
**
**	Revision 6.3  89/11  wong
**	Added UI_SESSION identifier for multiple session support.
**	18-apr-1990 (pete)  Add new columns to end of FE_ATT_INFO:
**		sort_direction, column_internal_datatype,
**		column_internal_length, column_internal_ingtype,
**		column_system_maintained. (jupbug 9355)  Also, removed
**		FE_ATT_SOURCE (replaced with FE_ATT_INFO.) (wong)
**
**      Revision 6.0  87/04/02  rdesmond.
**      Initial version.
**      15-apr-1987 (sandyd)
**              Added function declarations for iiuicw module.
**      05/21/87 (dkh) - Integrated VMS changes to FE_REL_SOURCE.
**      23-nov-1987 (rdesmond)
**              Converted to .qh file for improved relatt routines.
**      06-jun-1988 (sandyd)
**              Integrated IBM change from Emerson, which corrected
**              function declarations IIUIdcd_dist() and IIUIdci_initcap()
**              to match how they were spelled in the defining module.
**      13-jan-1989 (wolf)
**              Add UI_KEY_Y, UI_KEY_N and UI_KEY_U.
**	21-may-1990 (rdesmond)
**		added UI_INSTALLABLE, UI_NEEDS_UPGRADE, UI_UNINSTALLABLE and
**		UI_DESUPPORTED.
**	1-nov-1990 (pete)
**		added UI_NO_CLIENT (removed NO_CLIENT from dictutil.qsh).
**	20-mar-1991 (kathryn)
**		added key_is_compressed to UI_HELP_INFO structure.
**	10-jun-1992 (rdrane)
**		Added declaration of IIUIdlmcase() and IIUIscl_StdCatLevel().
**	9-jul-1992 (rdrane)
**		Added declarations of delimited identifier and wildcard scan
**		related routines.
**	20-jul-1992 (rdrane)
**		Added declaration of wildcard escape routine
**		IIUGewc_EscapeWildcard(). Added definition of ICSYNONYM
**		to support 6.5 database synonyms.
**	23-jul-1992 (rdrane)
**		Moved FE_RSLV_NAME typedef from fe.h so that ESQL can access
**		it directly.
**	31-jul-1992 (rdrane)
**		Add IIUGdlm_ChkdlmBEobject() return value constant definitions.
**	12-aug-1992 (rdrane)
**		Add protection against multiple includes.  Add UI_EWC_ESC_CHAR
**		constant to establish FE standard wildcard escape character.
**		Add declaration of IIUIpassword().
**	03-sep-1992 (rdrane)
**		Replace declaration of IIUGsrw_LookUp() with IIUGIsSQLKeyWord(),
**		as the former is now defunct and the latter has been upgraded.
**	15-sep-1992 (rdrane)
**		Add definition of catalog owner id.  It needs to be special
**		cased since its value is not a valid identifier.
**	1-oct-1992 (rdrane)
**		Add declaration of IIUGxri_id().
**	19-jul-1993 (rogerl)
**		Add default_value to FE_ATT_INFO for tables utility
**	31-dec-1993 (rdrane)
**		Add UI_UNDEFCASE to denote the no delimitedc identifier case
**		capability exists, and so delimited identifiers are not
**		supported.  Add declration of IIUIcsl_CommonSQLLevel().
**      27-feb-1996 (lawst01)
**         Windows 3.1 port changes - add some fcn prototypes.
**	06-may-1996 (chech02)
**	   Added more function prototypes for windows 3.1 port.
**	06-mar-1996 (harpa06 & stial01)
**		Added UI_LEVEL_66.
**              Added page_size to UI_HELP_INFO
**	09-Aug-1996 (ramra01)
**	   Added relversion and reltotwid to UI_HELP_INFO
**	29-Jan-1997 (hanch04)
**	   Added UI_LEVEL_800
**	25-Mar-1997 (jenjo02)
**	   Added cache_pri to UI_HELP_INFO
**	03-feb-1998 (hanch04)
**	   Added UI_LEVEL_850
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      02-Mar-2001 (hanch04)
**          Bumped release id to external "II 2.6" internal 860 or 8.6
**      07-Apr-2003 (hanch04)
**          Bumped release id to external "II 2.65" internal 865 or 8.65
**	12-Aug-2003 (yeema01) Bug# 110712
**	   Reporter crashed due to pointer size problem. 
**	   Need to explicitly define pointer size as 64-bits pointer since
**	   OpenROAD is built in 32-bits pointer size context.
**	30-Apr-2004 (schka24)
**	    Release ID is now II3.0, internal 00900.
**	    Added help stuff for partitioned tables.
**	13-oct-05 (inkdo01)
**	    Internal ID is 902 (collationIDs, other stuff?).
**	17-Jan-2006 (hweho01)
**	    Release ID is now 00904.
**	27-Apr-2006 (hweho01)
**	    Change release ID to 00910 for new release.
**	10-May-2006 (jenjo02)
**	    Add "is_clustered" to UI_HELP_INFO.
**	30-May-2006 (jenjo02)
**	    Removed "is_clustered"; "structure" will be "clustered"
**	07-May-2007 (drivi01)
**	    Updated internal version to 920.
*/
#ifndef UI_HDR_INCLUDED
#define	UI_HDR_INCLUDED
#include <erui.h>
#if defined(xde) && defined(axp_osf)
#pragma pointer_size save
#pragma pointer_size long
#endif
# define UI_SESSION -2
# define ICUNKNOWN 0
# define ICINDEX 1000
# define ICTABLE 2000
# define ICVIEW 3000
# define ICSYNONYM 4000
# define MODE_OPEN 1000
# define MODE_FETCH 1100
# define MODE_CLOSE 1200
# define STATUS_OPEN 1000
# define STATUS_DONE 1100
# define STATUS_CLOSED 1200
# define CURR_VALID 1000
# define CURR_INVALID 1100
# define UI_DATESIZE 25
# define UI_SAVEBUF_SIZE 1000
# define UI_LEVEL_NONE "00000"
# define UI_LEVEL_60 "00600"
# define UI_LEVEL_61 "00601"
# define UI_LEVEL_62 "00602"
# define UI_LEVEL_63 "00603"
# define UI_LEVEL_64 "00604"
# define UI_LEVEL_65 "00605"
# define UI_LEVEL_66 "00606"
# define UI_LEVEL_800 "00800"
# define UI_LEVEL_850 "00850"
# define UI_LEVEL_860 "00860"
# define UI_LEVEL_900 "00900"
# define UI_LEVEL_902 "00902"
# define UI_LEVEL_904 "00904"
# define UI_LEVEL_910 "00910"
# define UI_LEVEL_920 "00920"
# define UI_DML_NOLEVEL 0
# define UI_DML_QUEL 1
# define UI_DML_SQL 2
# define UI_DML_GTWSQL 3
# define UI_MAX_QBUF 5000
# define UI_AC_OFF 0
# define UI_AC_ON 1
# define UI_LOWERCASE 1000
# define UI_UPPERCASE 2000
# define UI_MIXEDCASE 3000
# define UI_UNDEFCASE 4000
# define UI_KEY_Y 1
# define UI_KEY_N 2
# define UI_KEY_U 3
# define UI_AC_OFF 0
# define UI_AC_ON 1
  typedef i4 DBSTATUS;
  typedef struct {
    char *name;
    char *owner_dest;
    char *name_dest;
    bool is_nrml;
    bool owner_spec;
    bool owner_dlm;
    bool name_dlm;
    bool is_syn;
  } FE_RSLV_NAME;
  typedef struct {
    char table_name[FE_MAXNAME + 1];
    char table_owner[FE_MAXNAME + 1];
    char create_date[26];
    char alter_date[26];
    char table_type[9];
    char version[9];
    char system_use[2];
    char unique_rule[2];
  } FE_REL_SOURCE;
  typedef struct {
    i4 status;
    FE_REL_SOURCE relsrcbuf[2];
    i4 bufstatus;
    bool dets_wanted;
    bool cats_wanted;
  } FE_REL_QBLK;
  typedef struct {
    char name[FE_MAXNAME+1];
    char owner[FE_MAXNAME+1];
    DATE create_date;
    DATE alter_date;
    char version[9];
    i4 type;
    bool catalog;
    bool unique_keys;
  } FE_REL_INFO;
  typedef struct {
    char table_name[FE_MAXNAME+1];
    char table_owner[FE_MAXNAME+1];
    char create_date[UI_DATESIZE+1];
    char alter_date[UI_DATESIZE+1];
    char table_type[2];
    char table_subtype[2];
    char table_version[9];
    char system_use[2];
    char table_stats[2];
    char table_indexes[2];
    char is_readonly[2];
    i4 num_rows;
    char storage_structure[17];
    char is_compressed[2];
    char duplicate_rows[2];
    char unique_rule[2];
    i4 number_pages;
    i4 overflow_pages;
    char expire_date[UI_DATESIZE + 1];
    char table_integrities[2];
    char table_permits[2];
    char all_to_all[2];
    char ret_to_all[2];
    i4 row_width;
    char is_journalled[2];
    char multi_locations[2];
    char view_base[2];
    char location_name[FE_MAXNAME + 1];
    char key_is_compressed[2];
    i4 page_size;
    i4 rel_version;
    i4 rel_totwid;
    i2 cache_pri;
    u_i2 phys_partitions;
    i2 partition_dimensions;
  } UI_HELP_INFO;
  typedef struct {
    char table_name[FE_MAXNAME+1];
    char owner[FE_MAXNAME+1];
    char column_name[FE_MAXNAME+1];
    DATE create_date;
    DATE alter_date;
    char type[FE_MAXNAME+1];
    i4 extern_length;
    i2 scale;
    bool nulls;
    bool defaults;
    i4 seq;
    i4 keyseq;
    char sort_direction[2];
    char column_internal_datatype[FE_MAXNAME + 1];
    i2 column_internal_length;
    i2 column_internal_ingtype;
    char column_system_maintained[2];
    char column_has_default[2];
    char default_type[11];
    char default_value[ DB_MAX_COLUMN_DEFAULT_LENGTH + 1 ];
    i2 adf_type;
    i2 intern_prec;
    i4 intern_length;
  } FE_ATT_INFO;
typedef struct
{
	i4		status; /* STATUS_OPEN | STATUS_DONE | STATUS_CLOSED */
	FE_ATT_INFO	current_rec;
	i4		current_status; /* CURR_VALID | CURR_INVALID */
} FE_ATT_QBLK;
/*
** Declarations for iiuicw module (and others).
*/
VOID		iiuicw1_CatWriteOn();
VOID		iiuicw0_CatWriteOff();
i4		IIUIdml();
DB_STATUS	IIUIdci_initcap();
i4		IIUIdcq_quelLevel();
i4		IIUIdcs_sqlLevel();
i4		IIUIcase();
i4		IIUIdlmcase();
char		*IIUIscl_StdCatLevel();
char		*IIUIcsl_CommonSQLLevel();
bool		IIUIdcd_dist();
DB_STATUS	IIUIcao_catOpen();
DB_STATUS	IIUIcaf_catFetch();
VOID		IIUIcac_catClose();
bool		IIUIesp_estabSP();
VOID		IIUIcsp_clearSP();
VOID		IIUIasp_abortSP();
DB_STATUS	IIUIrao_relOpen();
DB_STATUS	IIUIraf_relFetch();
VOID		IIUIrac_relClose();
DB_STATUS	IIUIautocommit();
DB_STATUS	IIUIbeginXaction();
DB_STATUS	IIUIendXaction();
DB_STATUS	IIUIabortXaction();
char		*IIUIpassword();
VOID		IIUIgdt_GetDefaultType();
#ifdef WIN16
DB_STATUS       IIUIupdate(char*, char*, DB_DATA_VALUE[], char*, nat);
DB_STATUS       IIUIinsert(char*, char*, DB_DATA_VALUE[], nat);
bool            IIUIdeadlock (i4);
bool            IIUIinXaction();
DB_STATUS       IIUIbeginXaction();
DB_STATUS       IIUIendXaction();
DB_STATUS       IIUIabortXaction();
DB_STATUS       iiuiAttrInfo (PTR, i4, ...);
bool            IIUIdcl_logkeys();
STATUS          IIUIssdStoreSessionData(char*, bool, bool, bool*);
VOID            IIUIsetData(bool);
STATUS		IIUIsharedData(bool*); 
VOID            IIUIgdt_GetDefaultType(PTR,nat,PTR,PTR,PTR);
#endif /* WIN16 */
/*
** Return status codes for IIUICheckDictVer()
*/
#define UI_INSTALLABLE		101
#define UI_NEEDS_UPGRADE	102
#define UI_UNINSTALLABLE	103
#define UI_DESUPPORTED		104
#define UI_NO_CLIENT		202
/*
** Additional return code from IIUIClientInstalled()
*/
#define UI_NOTINSTALLED		200
/* Return code from init_moduletable() */
#define UI_NOTDIRECTORY		1000
/*
**	Delimited identifier related routine declarations
*/
i4	IIUGdlm_ChkdlmBEobject();
VOID	IIUGdbo_dlmBEobject();
VOID	IIUGrqd_Requote_dlm();
bool	IIUGxri_id();
bool	IIUGIsSQLKeyWord();
/*
** Return values from IIUGdlm_ChkdlmBEobject()
*/
#define UI_REG_ID		0	/* Valid Regular Identifier	*/
#define UI_DELIM_ID		1	/* Valid Regular Identifier	*/
#define UI_BOGUS_ID		-1	/* Invalid Identifier		*/
/*
**	Wildcard scan related routine declarations
*/
VOID	IIUGwildcard();
bool	IIUGscanwildcard();
VOID	IIUGewc_EscapeWildcard();
/*
** Standard wildcard escape character, as known to FErel*()
** routines which employ LIKE predicates in their WHERE clauses
*/
#define UI_EWC_ESC_CHAR		('\\')
/*
** Owner ID for INGRES FE Catalogs.  FE_resolve() needs this string
** so that it doesn't consider the returned owner as being an invalid
** identifier and thus fail the table resolution.
*/
#define	UI_FE_CAT_ID_65		"$ingres"
#if defined(xde) && defined(axp_osf)
#pragma pointer_size restore
#endif
#endif
