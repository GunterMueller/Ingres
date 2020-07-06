# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**Copyright (c) 2004 Ingres Corporation
*/
#include    <gcccl.h>
/**
** Name: DUENV.QSH -	structures that define a system utility's
**			environment.
**
** Description:
**        This file contains all of the structure definitions that are
**	needed to define a system utility's environment.  Note, some of
**	the structures in this file must be made known to Equel.
**
**	Dependencies:
**	    dbms.h
**	    duf.h
**	    lk.h
**
** History: 
**      28-Aug-86 (ericj)
**          Initial creation.
**	15-Apr-87 (ericj)
**	    Added status field to location structure.
**	20-Jan-88   (ericj)
**	    Updated DU_ENV struct.
**	5-Nov-88 (teg)
**	    Changed du_xxx_ingpath() routines to return status instead of VOID
**      7-dec-88    (alexc)
**          added du_ddbname, and du_cdbname to duenv, merge up to FCS class.
**	14-may-89 (anton)
**	    database collation argument addition
**	24-jul-90 (teresa)
**	    renamed from duenv.qh to duenv.qsh to solve unix build problems.
**	10-Jan-93 (jpk)
**	    updated DU_ENV structure for FIPS.
**	5-Feb-93 (jpk)
**	    add new du_env structure member, du_nodename, to support
**	    createdb and destroydb over the net.
**	25-jun-93 (robf)
**	    Added du_secure_level, du_ingres_low/high for secure db creation
**	20-jul-1993 (jpk)
**	    updated DU_ENV structure for FIPS again: real user case
**	    as well as regular identifier and delimited identifier
**	    case.  Also flag where we got the values: createdb defaults,
**	    installation defaults, or command line.
**      31-jan-94 (jrb)
**	    Added prototype for du_extdb_ingpath().
**	20-may-97 (hayke02)
**	    Added constant (DU_CASE_LEN = 7) for the length of the three
**	    case string char arrays (du_reg_id_case, du_delim_id_case,
**	    du_real_user_case).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*
[@forward_type_references@]
*/
/*
**  Forward and/or External function references.
*/
FUNC_EXTERN VOID    du_envinit();       /* Initializes the system utility
					** environment struct.
					*/
FUNC_EXTERN STATUS du_ckp_ingpath();	/* Generate an Ingres checkpoint
					** directory path for a database.
					*/
FUNC_EXTERN STATUS du_db_ingpath();	/* Generate an Ingres default data
					** directory path for a database.
					*/
FUNC_EXTERN STATUS du_ejnl_ingpath();   /* Generate an Ingres exp. journal
					** directory path for a database.
					*/
FUNC_EXTERN STATUS du_extdb_ingpath(); /* Generate an Ingres an ext. data
					** directory path for a database.
					*/
FUNC_EXTERN STATUS du_work_ingpath();   /* Generate an Ingres work
					** directory path for a database.
					*/
FUNC_EXTERN STATUS du_fjnl_ingpath();   /* Generate an Ingres full-journal
					** directory path for a database.
					*/
FUNC_EXTERN STATUS du_jnl_ingpath();	/* Generate an Ingres checkpoint
					** directory path for a database.
					*/
FUNC_EXTERN STATUS du_dmp_ingpath();	/* Generate an Ingres dump
					** directory path for a database.
					*/
/*
[@function_reference@]...
[@group_of_defined_constants@]...
[@global_variable_references@]
*/
/*}
** Name: DU_LOC -   a location and area associated with a database.
**
** Description:
**        This structure is used to hold a location and an area associated
**	with a database.  Note, portions of this structure are known to
**	Equel.
**
** History:
**      05-Sep-86 (ericj)
**          Initial creation.
**      15-apr-87 (ericj)
**          Added status field.
[@history_line@]...
[@history_template@]...
*/
  typedef struct _DU_LOC {
    i4 du_status;
    char du_locname[DU_LOC_SIZE + 4];
    char du_area[DB_AREA_MAX + 4];
  } DU_LOC;
/*}
** Name: LOC_LIST -	list of locations and areas that can be associated
**			with a database.
**
** Description:
**        This structure is used to build a list of locations and areas that
**	are associated with a database that is being operated on with a
**	system utility.  Note, portions of this structure are known to
**	Equel.
**
** History:
**      28-Aug-86 (ericj)
**          Initial creation.
**      05-Sep-86 (ericj)
**          Made a DU_LOC struct and added it as a member of this struct.
[@history_template@]...
*/
  typedef struct _DU_LOC_LIST {
	struct _DU_LOC_LIST *du_next;
    DU_LOC du_loc;
  } DU_LOC_LIST;
/*}
** Name: DU_ENV -   defines the environment for a system utility.
**
** Description:
**        This structure defines the environment for a system utility.
**	Note, portions of this structure are are known to Equel.
**
** History:
**      28-Aug-86 (ericj)
**          Initial creation.
**	20-Jan-87   (ericj)
**	    Added sortdev, dbservice, and dbcmptlvl fields.
**      7-dec-88    (alexc)
**          added du_ddbname, and du_cdbname to duenv, merge up to FCS class.
**	14-May-89 (anton)
**	    Added collation field
**	20-sep-91 (teresa)
**	    Added dbnamePlusService to structure for ingres63p change 262603
**	    and dbdbnamePlusService
**	25-jun-93 (robf)
**	    Added du_secure_level, du_ingres_low/high for secure db creation
**	10-nov-93 (rblumer)
**	    For delimited identifier support, and iiqef_createdb changes,
**	    added du_xusrname, du_unorm_usr, du_dba_target, and du_dba_iidbdb.
**	19-mar-2001 (stephenb)
**	    Added du_ucollation
**	03-feb-2005 (gupsh01)
**	    Added du_uninfc_flag.
[@history_template@]...
*/
#ifndef DU_TAIL_LEN
#define DU_TAIL_LEN  8
#endif
  typedef struct _DU_ENV {
    i4 du_usrstat;
    i4 du_user_uid;
    i4 du_access;
    i4 du_dbservice;
    i4 du_dbcmptminor;
    i4 du_dbid;
	i4	    du_dbopen;			    /* Is a database currently
						    ** open?
						    */
	i4	    du_1dblocked;		    /* Is a database currently
						    ** locked?
						    */
	LK_LLID	    du_lklist;			    /* lock list for this db */
    DU_LOC du_dbloc;
    DU_LOC du_ckploc;
    DU_LOC du_jnlloc;
    DU_LOC du_dmploc;
    DU_LOC du_sortloc;
    char du_dbname[DB_MAXNAME + 4];
    char du_nodename[GCC_L_NODE + 1];
    char du_dbcmptlvl[DB_MAXNAME+4];
# define	DU_CASE_LEN  7
    char du_delim_id_case[DU_CASE_LEN];
    char du_got_delim_id_case;
    char du_reg_id_case[DU_CASE_LEN];
    char du_got_reg_id_case;
    char du_real_user_case[DU_CASE_LEN];
    char du_got_real_user_case;
    char du_usrinit[DU_USR_SIZE + 4];
/*****************************************************************************/
    char du_dba[DU_USR_SIZE + 4];
/*****************************************************************************/
    char du_unorm_usr[DB_MAX_DELIMID + 4];
    char du_xusrname[DU_USR_SIZE + 4];
    char du_dba_iidbdb[DU_USR_SIZE + 4];
    char du_dba_target[DU_USR_SIZE + 4];
	char	    du_collation[DB_MAXNAME + 4];   /* db collation */
	char	    du_ucollation[DB_MAXNAME + 4];  /* Unicode collation */
	i4	    du_uninorm_flag;		    /* Unicode normalization */
# define DU_UNORMNFC_SET    0x01
	DU_LOC_LIST *du_db_locs;		    /* valid location/area
						    ** mappings for db
						    */
    char du_ddbname[DB_MAXNAME + 4];
    char du_cdbname[DB_MAXNAME + 4];
    char du_gdbname[DB_MAXNAME + 4];
    char dbnamePlusService[ DB_MAXNAME + DU_TAIL_LEN ];
    char dbdbnamePlusService[ DB_MAXNAME + DU_TAIL_LEN ];
    i4 du_secure_level;
# define	DU_SECURE_NONE  0x01
# define	DU_SECURE_C2	0x04
    char *du_ingres_low;
    char *du_ingres_high;
  } DU_ENV;
/*}
** Name: DU_USR -   describes an Ingres user.
**
** Description:
**        This struct is used to define an Ingres user's information that
**	is contained in the "users" file and "user" relation of the dbdb.
**
** History:
**      29-Sep-86 (ericj)
**          Initial creation.
[@history_template@]...
*/
  typedef struct _DU_USR {
    i4 du_ugid;
    i4 du_umid;
    i4 du_ustatus;
    char du_uname[DU_USR_SIZE + 4];
  } DU_USR;
/*
[@type_definitions@]
*/
