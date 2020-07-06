# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <st.h>
# include <lo.h>
# include <si.h>
# include <er.h>
# include <ex.h>
# include <me.h>
# include <pc.h>
# include <ci.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <ug.h>
# include <uigdata.h>
# include <rpu.h>
# include <targtype.h>
# include "errm.h"
/**
** Name:	repdbcfg.sc - configure Replicator databases
**
** Description:
**	Defines
**		main		- configure Replicator databases
**		cdds_exist	- does the CDDS exist?
**		db_cfg		- configure a database
**		db_cdds_cfg	- add a database to a CDDS
**		path_cfg	- configure a path
**
** History:
**	16-dec-96 (joea)
**		Created based on repdbcfg.sc in replicator library.
**      15-jan-97 (hanch04)
**              Added missing NEEDLIB
**	16-jan-97 (joea)
**		Use defined constants for target types.
**	27-mar-98 (joea)
**		Discontinue use of RPing_ver_ok.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      11-Jun-2004 (hanch04)
**          Removed reference to CI for the open source release.
**	29-Sep-2004 (drivi01)
**	    Updated NEEDLIBS to link dynamic library SHQLIB and SHFRAMELIB
**	    to replace its static libraries.
**/
/*
PROGRAM =	repdbcfg
NEEDLIBS =      REPMGRLIB REPCOMNLIB SHFRAMELIB SHQLIB SHCOMPATLIB SHEMBEDLIB
UNDEFS =	II_copyright
*/
# define MAX_WORDS		5
# define DEFAULT_CDDS_NO	50
  static i2 local_db_no;
static	char	*cfg_db_name = NULL;
static	char	*user_name = ERx("");
static	char	*cfg_file_name = ERx("");
static	ARG_DESC args[] =
{
	/* required */
	{ERx("database"), DB_CHR_TYPE, FARG_PROMPT, (PTR)&cfg_db_name},
	{ERx("config_file"), DB_CHR_TYPE, FARG_PROMPT, (PTR)&cfg_file_name},
	/* optional */
	{ERx("user"), DB_CHR_TYPE, FARG_FAIL, (PTR)&user_name},
	NULL
};
static bool cdds_exist(i2 cdds_no);
static STATUS db_cfg(i2 db_no, char *db_name, char *vnode_name,
	char *db_owner, i2 cdds_no, i2 target_type);
static STATUS db_cdds_cfg(i2 cdds_no, i2 db_no, i2 target_type);
static STATUS path_cfg(i2 cdds_no, i2 source_db, i2 local_db,
	i2 target_db);
FUNC_EXTERN STATUS FEhandler(EX_ARGS *ex);
/*{
** Name:	main	- configure Replicator databases
**
** Description:
**	Configures multiple Replicator databases from a configuration file.
**	This is intended primarily to configure Desktop Replicator databases
**	for several users.
**
**	The configuration file has the following format:
**
**	user_name [db_no] [db_name] [cdds_no] [target_type]
**
** Inputs:
**	argc - the number of arguments on the command line
**	argv - the command line arguments
**		argv[1] - the database to connect to
**		argv[2] - name of file with configuration information
**
** Outputs:
**	none
**
** Returns:
**	OK on success
*/
i4
main(
i4	argc,
char	*argv[])
{
	LOCATION	cfg_file_loc;
	FILE	*cfg_file;
	char	cfg_file_buf[MAX_LOC+1];
	char	buf[SI_MAX_TXT_REC+1];
	i4	count;
	char	*words[MAX_WORDS];
	char	db_owner[DB_MAXNAME+1];
	char	db_name[DB_MAXNAME+1];
	i2	cdds_no;
	i4	tmp_nat;
	char	targ_str[3];
	i2	target_type;
	i4	num_dbs = 0;
  i2 db_no;
	EX_CONTEXT	context;
	/* Tell EX this is an INGRES tool */
	(void)EXsetclient(EX_INGRES_TOOL);
	/* Use the INGRES memory allocator */
	(void)MEadvise(ME_INGRES_ALLOC);
	/* Initialize character set attribute table */
	if (IIUGinit() != OK)
		PCexit(FAIL);
	/* Parse the command line arguments */
	if (IIUGuapParse(argc, argv, ERx("repdbcfg"), args) != OK)
		PCexit(FAIL);
	/* Set up the generic exception handler */
	if (EXdeclare(FEhandler, &context) != OK)
	{
		EXdelete();
		PCexit(FAIL);
	}
	STcopy(cfg_file_name, cfg_file_buf);
	if (LOfroms(PATH & FILENAME, cfg_file_buf, &cfg_file_loc) != OK)
	{
		SIprintf("Invalid file name\n");
		PCexit(FAIL);
	}
	if (SIfopen(&cfg_file_loc, ERx("r"), SI_TXT, SI_MAX_TXT_REC, &cfg_file)
		!= OK)
	{
		SIprintf("Cannot open configuration information file '%s'.\n",
			cfg_file_name);
		PCexit(FAIL);
	}
	/* Open the database */
	if (FEningres(NULL, (i4)0, cfg_db_name, user_name, NULL) != OK)
	{
		EXdelete();
		PCexit(FAIL);
	}
/* # line 185 "repdbcfg.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set AUTOCOMMIT OFF");
    IIsyncup((char *)0,0);
  }
/* # line 186 "repdbcfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(3,(short *)0,1,32,0,(char *)"genericerror");
  }
/* # line 188 "repdbcfg.sc" */	/* host code */
	if (!IIuiIsDBA)
	{
		EXdelete();
		IIUGerr(E_RM0002_Must_be_DBA, UG_ERR_FATAL, 0);
	}
	if (!RMcheck_cat_level())
	{
		EXdelete();
		IIUGerr(E_RM0003_No_rep_catalogs, UG_ERR_FATAL, 0);
	}
/* # line 200 "repdbcfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select database_no from dd_databases where local_db=1");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&local_db_no);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 204 "repdbcfg.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
	{
		IIUGerr(E_RM0004_Error_get_local_db, UG_ERR_ERROR, 0);
		PCexit(FAIL);
	}
	db_no = 0;
	cdds_no = DEFAULT_CDDS_NO;
	target_type = TARG_FULL_PEER;
	while (SIgetrec(buf, (i4)sizeof(buf), cfg_file) != ENDFILE)
	{
		count = MAX_WORDS;
		STgetwords(buf, &count, words);
		if (count == 0)		/* blank line */
			continue;
		if (count == 1 && db_no == 0)
		{
/* # line 221 "repdbcfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select IFNULL(max(database_no), 100) +1 from dd_databases where datab\
ase_no>100");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&db_no);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 225 "repdbcfg.sc" */	/* host code */
			if (RPdb_error_check(0, NULL) != OK)
			{
				SIprintf("Error retrieving next database number.\n");
/* # line 228 "repdbcfg.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 229 "repdbcfg.sc" */	/* host code */
				break;
			}
/* # line 231 "repdbcfg.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 232 "repdbcfg.sc" */	/* host code */
		}
		if (count > 0)
		{
			if (STlength(words[0]) > sizeof(db_owner) - 1)
			{
				SIprintf("User name too long:  '%s'.\n",
					words[0]);
				continue;
			}
			STcopy(words[0], db_owner);
			STcopy(words[0], db_name);
		}
		if (count > 1)
		{
			if (CVan(words[1], &tmp_nat) != OK)
			{
				SIprintf("Invalid database number:  '%s'.\n",
					words[1]);
				continue;
			}
			db_no = (i2)tmp_nat;
			if (db_no < 1 || db_no > MAXI2)
			{
				SIprintf("Invalid database number:  %d.\n",
					tmp_nat);
				continue;
			}
		}
		if (count > 2)
		{
			if (STlength(words[2]) > sizeof(db_name) - 1)
			{
				SIprintf("Database name too long:  '%s'.\n",
					words[2]);
				continue;
			}
			STcopy(words[2], db_name);
		}
		if (count > 3)
		{
			if (CVan(words[3], &tmp_nat) != OK)
			{
				SIprintf("Invalid CDDS number:  '%s'.\n",
					words[3]);
				continue;
			}
			if (tmp_nat < 0 || tmp_nat > MAXI2)
			{
				SIprintf("Invalid CDDS number:  %d.\n",
					tmp_nat);
				continue;
			}
			cdds_no = (i2)tmp_nat;
		}
		if (count > 4)
		{
			if (STlength(words[4]) > sizeof(targ_str) - 1)
			{
				SIprintf("Invalid target type:  '%s'.\n",
					words[4]);
				continue;
			}
			STcopy(words[4], targ_str);
			CVupper(targ_str);
			if (STequal(targ_str, ERx("FP")))
			{
				target_type = TARG_FULL_PEER;
			}
			else if (STequal(targ_str, ERx("PR")))
			{
				target_type = TARG_PROT_READ;
			}
			else if (STequal(targ_str, ERx("UR")))
			{
				target_type = TARG_UNPROT_READ;
			}
			else
			{
				SIprintf("Invalid target type:  '%s'.\n",
					words[4]);
				continue;
			}
		}
		if (!cdds_exist(cdds_no))
		{
			SIprintf("Undefined CDDS number:  %d.\n", cdds_no);
			continue;
		}
		if (db_cfg(db_no, db_name, ERx("mobile"), db_owner, cdds_no,
			target_type) != OK)
		{
			SIprintf("Error configuring database for user '%s'.\n\n",
				db_owner);
/* # line 325 "repdbcfg.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 326 "repdbcfg.sc" */	/* host code */
			continue;
		}
/* # line 328 "repdbcfg.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 329 "repdbcfg.sc" */	/* host code */
		++db_no;
		++num_dbs;
	}
	FEing_exit();
	SIprintf("%d databases configured.\n", num_dbs);
	EXdelete();
	PCexit(OK);
}
/*{
** Name:	cdds_exist	- does the CDDS exist?
**
** Description:
**	Verifies that the given CDDS exists in dd_cdds.
**
** Inputs:
**	cdds_no		- CDDS number
**
** Outputs:
**	none
**
** Returns:
**	TRUE or FALSE
*/
static bool
cdds_exist(
i2	cdds_no)
# if 0
  i2 cdds_no;
# endif
{
  i4 cnt;
/* # line 370 "repdbcfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_cdds where cdds_no=");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 373 "repdbcfg.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
		return (FALSE);
	return (cnt ? TRUE : FALSE);
}
/*{
** Name:	db_cfg	- configure a database
**
** Description:
**	Inserts a row into dd_databases for a given database, and one or two
**	rows into dd_paths to create paths from the local database to
**	the new database.
**
** Inputs:
**	db_no		- the database number
**	db_name		- database name
**	vnode_name	- vnode name
**	db_owner	- database owner name
**	cdds_no		- CDDS number
**	target_type	- the target type
**
** Outputs:
**	none
**
** Returns:
**	OK or FAIL
*/
static STATUS
db_cfg(
i2	db_no,
char	*db_name,
char	*vnode_name,
char	*db_owner,
i2	cdds_no,
i2	target_type)
# if 0
  i2 db_no;
  char *db_name;
  char *vnode_name;
  char *db_owner;
# endif
{
	DBEC_ERR_INFO	errinfo;
	STATUS		stat;
/* # line 422 "repdbcfg.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_databases(database_no, vnode_name, database_name, data\
base_owner, dbms_type, local_db, config_changed)values(");
    IIputdomio((short *)0,1,30,2,&db_no);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,vnode_name);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,db_name);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,db_owner);
    IIwritio(0,(short *)0,1,32,0,(char *)", 'opingdt', 0, DATE('now'))");
    IIsyncup((char *)0,0);
  }
/* # line 428 "repdbcfg.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
	{
		if (errinfo.errorno)
		{
			SIprintf("DBMS error %d adding database %d.\n",
				errinfo.errorno, db_no);
			return (errinfo.errorno);
		}
		else
		{
			SIprintf("Error adding database %d.\n", db_no);
			return (FAIL);
		}
	}
	stat = db_cdds_cfg(cdds_no, local_db_no, 1);
	if (stat)
		return (stat);
	stat = db_cdds_cfg(cdds_no, db_no, target_type);
	if (stat)
		return (stat);
	stat = path_cfg(cdds_no, local_db_no, local_db_no, db_no);
	if (stat)
		return (stat);
	if (target_type == TARG_FULL_PEER)
	{
		stat = path_cfg(cdds_no, db_no, db_no, local_db_no);
		if (stat)
			return (stat);
	}
	return (OK);
}
/*{
** Name:	db_cdds_cfg	- add a database to a CDDS
**
** Description:
**	Inserts a row into dd_db_cdds for a given database and CDDS, if it does
**	not already exist.
**
** Inputs:
**	cdds_no		- CDDS number
**	db_no		- the database number
**	target_type	- the CDDS target type
**
** Outputs:
**	none
**
** Returns:
**	OK or FAIL
*/
static STATUS
db_cdds_cfg(
i2	cdds_no,
i2	db_no,
i2	target_type)
# if 0
  i2 cdds_no;
  i2 db_no;
  i2 target_type;
# endif
{
  i4 cnt;
	DBEC_ERR_INFO	errinfo;
/* # line 502 "repdbcfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_db_cdds where cdds_no=");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIwritio(0,(short *)0,1,32,0,(char *)"and database_no=");
    IIputdomio((short *)0,1,30,2,&db_no);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 506 "repdbcfg.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
		return (FAIL);
	if (cnt)
		return (OK);
/* # line 512 "repdbcfg.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_db_cdds(cdds_no, database_no, target_type, server_no)v\
alues(");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,2,&db_no);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,2,&target_type);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1)");
    IIsyncup((char *)0,0);
  }
/* # line 516 "repdbcfg.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
	{
		if (errinfo.errorno)
		{
			SIprintf("DBMS error %d adding database %d to CDDS %d.\n",
				errinfo.errorno, db_no, cdds_no);
			return (errinfo.errorno);
		}
		else
		{
			SIprintf("Error adding database %d to CDDS %d.\n",
				db_no, cdds_no);
			return (FAIL);
		}
	}
	return (OK);
}
/*{
** Name:	path_cfg	- configure a data propagation path
**
** Description:
**	Inserts a row into dd_paths for a given CDDS.
**
** Inputs:
**	cdds_no		- CDDS number
**	source_db	- the source database number
**	local_db	- the local database number
**	target_db	- the target database number
**
** Outputs:
**	none
**
** Returns:
**	OK or FAIL
*/
static STATUS
path_cfg(
i2	cdds_no,
i2	source_db,
i2	local_db,
i2	target_db)
# if 0
  i2 cdds_no;
  i2 source_db;
  i2 local_db;
  i2 target_db;
# endif
{
	DBEC_ERR_INFO	errinfo;
/* # line 571 "repdbcfg.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_paths(cdds_no, sourcedb, localdb, targetdb)values(");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,2,&source_db);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,2,&local_db);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,2,&target_db);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 575 "repdbcfg.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
	{
		if (errinfo.errorno)
		{
			SIprintf("DBMS error %d adding path for CDDS %d.\n",
				errinfo.errorno, cdds_no);
			return (errinfo.errorno);
		}
		else
		{
			SIprintf("Error adding path for CDDS %d.\n", cdds_no);
			return (FAIL);
		}
	}
	return (OK);
}
