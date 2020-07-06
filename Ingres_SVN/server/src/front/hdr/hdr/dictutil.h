# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
/**
** Name:	dictutil.h -	Dictionary utility definitions.
**
** Description:
**	Contains definitions pertinent to the dictionary utilities.
**
** History:
**      25-sep-1990 (pete)
**              Added support for the various file extensions of executable
**		on different operating systems (DDIMGEXT).
**	1-nov-1990 (pete)
**		Removed NO_CLIENT to ui.qsh as UI_NO_CLIENT.
**	9-nov-1990 (pete)
**		Removed all the DDIMGEXT #define's. Now that LO_EXE_EXT
**		exists in lo.h, we should all be using that.
**	3-jan-1991 (pete)
**		Added new stuff for Star support.
**	31-jul-1991 (pete)
**		Added more comments. Added module status "UNKNOWN".
**	10-mar-1994 (pete)
**		Add couple new #defines: DD_CREATE_INDEX, DD_DROP_INDEX.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/* Identify statement type in .cre, .mfy files and work done on a table
** by upgrade executable.
*/
#define DD_OTHER_STMT   10
#define DD_CREATE_TABLE 11
#define DD_DROP_TABLE   12
#define DD_IGNORE_ERROR 13
#define DD_MODIFY_TABLE 14
#define DD_ALTER_TABLE  15
#define DD_CREATE_INDEX 16
#define DD_DROP_INDEX   17
/* typedef used by module upgrade executables */
typedef struct
{
        char *tab;		/* table to be upgraded */
        char *col;		/* column to check to see if upgrade needed */
        STATUS (*is_done)();	/* call this to check if conversion needed */
        VOID (*convert)();	/* call this to do the conversion */
	i4  upgrade_type;	/* type work done in upgrade. used for Star.
				** (e.g. DD_ALTER_TABLE, DD_CREATE_TABLE)
				*/
	bool did_upgrade;	/* TRUE if upgrade was done; FALSE otherwise */
} DD_DRIVE;
/*
** Dictionary module bits.
**
** Note that these are bit codes, so that one may mask together
** sets of modules.
*/
#define DMB_CORE	1
#define DMB_APPDEV1	2
#define DMB_APPDEV2	4
#define DMB_APPDEV3	8
#define DMB_METASCHEMA	0x10
#define DMB_PHYSDATMOD	0x20
/*
** Structures required for dictionary installation.
*/
  typedef struct _moduledesc {
    char modname[33];
    i4 modversion;
    char remark[61];
    i4 status;
    bool compat;
    i4 numparents;
    struct _moduledesc	*installed_child;	/* Pts to installed child of
						** this module (higher version
						** number), if this module's
						** status = DESC_COMPAT. Status
						** of child pointed to should be
						** INSTALLED or UPGRADE_ABORT
						** and "compat" should be TRUE.
						*/
    struct _moduledesc	**parents;		/* descendent modules of this
						** one (lower version numbers).
						*/
    char filebase[9];
  } MODULEDESC;
  typedef struct {
    char cliname[33];
    i4 cliversion;
    i4 status;
    bool required;
    char filebase[9];
    i4 numsupmods;
    MODULEDESC	**supmods;	/* pointer to moduletable info for every
				** module this client is dependent on.
				*/
  } CLIENTDESC;
# ifdef NT_GENERIC
# define connect ii_connect
# endif
/*
** Defines for dictionary installation
*/
#define	UNKNOWN		0
#define	INSTALLED	101
#define	ANC_COMPAT	102
#define	ANC_INCOMPAT	103
#define	DESC_COMPAT 	104
#define	DESC_INCOMPAT	105
#define	NOTINSTALLED 	106
#define	UPGRADE_ABORT 	107
#define	MODIFIED 	108
#define	DO_INBUFSIZE 	600
#define CREATE	0
#define MODIFY	1
#define CLEANUP	2
#define UPGRADE	3
/*
** Procedure declarations
*/
STATUS init_moduletable();
STATUS init_clienttable();
STATUS enter_client();
STATUS installallmodules();
STATUS modifyallmodules();
STATUS modulestatus();
STATUS descstatus();
MODULEDESC * getchild();
STATUS ancstatus();
STATUS removeclidep();
STATUS enterclidep();
STATUS installmodule();
STATUS createmodule();
STATUS upgrademodule();
STATUS mod_dict_status();
bool confirmincompat();
STATUS updatedictmodules();
STATUS addtodictmodules();
STATUS create_from_file();
STATUS modifymodule();
STATUS execsql_from_file();
STATUS upgrade_from_file();
STATUS execsql();
