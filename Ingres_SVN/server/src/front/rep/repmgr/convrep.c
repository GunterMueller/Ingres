# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <cm.h>
# include <st.h>
# include <si.h>
# include <er.h>
# include <ex.h>
# include <me.h>
# include <pc.h>
# include <ci.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <ui.h>
# include <ug.h>
# include <uigdata.h>
# include <rpu.h>
# include <tblobjs.h>
# include <tbldef.h>
# include "errm.h"
/**
** Name:	convrep.sc - convert Replicator data dictionary
**
** Description:
**	Defines
**		main		- convert data dictionary
**		error_exit	- exit on error
**		drop_old_rules	- drops old rules
**		drop_object	- drops a database object
**		save_old_tables	- saves neccessary old tables
**		convert_tables	- converts old tables
**		convert_supp_objs - converts support objects
**		drop_old_dict	- drops old data dictionary
**
** History:
**	09-jan-97 (joea)
**		Created based on convrep.sc in replicator library.
**	14-jan-97 (joea)
**		Remove dd_target_types.
**	22-sep-97 (joea) bug 85701
**		Call RMcheck_cat_level to verify catalog compatibility instead
**		of doing an incomplete query here.  Add capability to drop 1.1
**		rules and local procedures, drop tables discontinued from 1.1
**		to 2.0 and convert 1.1 dbprocs.
**	27-mar-98 (joea)
**		Discontinue use of RPing_ver_ok.
**	16-jun-98 (abbjo03)
**		Add indexes argument to modify_tables. Replace load_dict by
**		load_tables.
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
PROGRAM =	convrep
NEEDLIBS =	REPMGRLIB REPCOMNLIB SHFRAMELIB SHQLIB \
		SHCOMPATLIB SHEMBEDLIB
UNDEFS =	II_copyright
*/
# define DIM(a)		(sizeof(a) / sizeof(a[0]))
# define OBJ_TABLE	1
# define OBJ_DBPROC	2
# define OBJ_RULE	3
# define STMT_SIZE	10000
# define REP1_0		10
# define REP1_1		11
# define TRUNC_TBLNAME_LEN	10
# define CAT_LEVEL_SESSION 1
GLOBALREF TBLDEF *RMtbl;
/* These are the names of the Replicator catalogs in 1.0 releases */
static char *replic_tbls[] =
{
	ERx("dd_installation"),
	ERx("dd_connections"),
	ERx("dd_distribution"),
	ERx("dd_registered_tables"),
	ERx("dd_registered_columns"),
	ERx("dd_distrib_transaction"),
	ERx("dd_distribution_queue"),
	ERx("dd_quiet_installations"),
	ERx("dd_replicated_tables"),
	ERx("dd_target_type"),
	ERx("dd_messages"),		/* discontinued in 1.0/04 */
	ERx("dd_replica_target"),	/* discontinued in 1.0/04 */
	ERx("dd_mobile_opt"),		/* introduced in 1.0/05 */
};
static char *replic_tbls2[] =
{
	ERx("dd_target_types"),		/* discontinued in 2.0 */
	ERx("dd_transaction_id"),	/* discontinued in 2.0 */
};
static char *replic_tbls_same[] =
{
	ERx("dd_input_queue"),
	ERx("dd_mail_alert"),
	ERx("dd_servers"),
	ERx("dd_events"),
	ERx("dd_flag_values"),
	ERx("dd_server_flags")
};
/* REP 1.0 suffixes for rules and dbprocs */
static char *rule_suffixes[] =
{
	ERx("1"), ERx("2"), ERx("3")
};
static char *proc_suffixes[] =
{
	ERx("i"), ERx("u"), ERx("d"),
	ERx("i2"), ERx("u2"), ERx("d2")
};
/* REP 1.1 suffixes for rules and dbprocs */
static char *rule_suffixes11[] =
{
	ERx("ins"), ERx("upd"), ERx("del")
};
static char *proc_suffixes11[] =
{
	ERx("loi"), ERx("lou"), ERx("lod")
};
static char *old_shadow_cols =
	ERx("database_no, transaction_id, sequence_no, trans_time, \
distribution_time, in_archive, dd_routing, dd_priority, trans_type, new_key, \
old_database_no, old_transaction_id, old_sequence_no");
static char *new_shadow_cols =
	ERx("sourcedb, transaction_id, sequence_no, trans_time, \
distribution_time, in_archive, cdds_no, dd_priority, trans_type, new_key, \
old_sourcedb, old_transaction_id, old_sequence_no");
static char *old_archive_cols =
	ERx("database_no, transaction_id, sequence_no");
static char *new_archive_cols =
	ERx("sourcedb, transaction_id, sequence_no");
static struct tagTbl
{
	i4	no;
	char	name[DB_MAXNAME+1];
} *tbls = NULL;
static i4	curr_level = REP1_0;
  static i4 NumTables = 0;
  static char *db_name = NULL;
  static char *dba_name = ERx("");
static	ARG_DESC args[] =
{
	/* required */
	{ERx("database"), DB_CHR_TYPE, FARG_PROMPT, (PTR)&db_name},
	/* optional */
	{ERx("user"), DB_CHR_TYPE, FARG_FAIL, (PTR)&dba_name},
	NULL
};
static STATUS drop_old_rules(void);
static STATUS save_old_tables(void);
static STATUS convert_tables(void);
static STATUS convert_supp_objs(void);
static STATUS convert_dbprocs(void);
static STATUS drop_old_dict(void);
static void error_exit(void);
static STATUS drop_object(char *obj_name, i4  obj_type);
FUNC_EXTERN STATUS FEhandler(EX_ARGS *ex);
FUNC_EXTERN STATUS load_tables(void);
FUNC_EXTERN STATUS modify_tables(bool indexes);
/*{
** Name:	main - convert data dictionary
**
** Description:
**	Converts Replicator data dictionary.  Convrep does its job in three
**	independent stages (transactions) to ensure catalogs integrity and
**	recoverability: (1) disable the Change Recorder rules, (2) convert
**	the catalogs, and (3) convert the support objects.  It runs with an
**	exclusive table level lock level.  Convrep exits if current release
**	catalogs already exist.
**
** Inputs:
**	argc	- the number of arguments on the command line
**	argv	- the command line arguments
**		argv[1]	- the database to connect to
**
** Outputs:
**	none
**
** Returns:
**	OK or FAIL
*/
i4
main(
i4	argc,
char	*argv[])
{
  i4 exist;
	EX_CONTEXT	context;
	/* Tell EX this is an INGRES tool */
	(void)EXsetclient(EX_INGRES_TOOL);
	/* Use the INGRES memory allocator */
	(void)MEadvise(ME_INGRES_ALLOC);
	/* Initialize character set attribute table */
	if (IIUGinit() != OK)
		PCexit(FAIL);
	/* Parse the command line arguments */
	if (IIUGuapParse(argc, argv, ERx("convrep"), args) != OK)
		PCexit(FAIL);
	/* Set up the generic exception handler */
	if (EXdeclare(FEhandler, &context) != OK)
	{
		EXdelete();
		PCexit(FAIL);
	}
	/* Open the database - normal UI_SESSION */
	if (FEningres(NULL, (i4)0, db_name, dba_name, NULL) != OK)
	{
		EXdelete();
		PCexit(FAIL);
	}
/* # line 267 "convrep.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set AUTOCOMMIT OFF");
    IIsyncup((char *)0,0);
  }
/* # line 268 "convrep.sc" */	/* set_sql */
  {
    IILQssSetSqlio(3,(short *)0,1,32,0,(char *)"genericerror");
  }
/* # line 270 "convrep.sc" */	/* host code */
	if (!IIuiIsDBA)
	{
		EXdelete();
		IIUGerr(E_RM0002_Must_be_DBA, UG_ERR_FATAL, 0);
	}
	if (*dba_name == EOS)
		dba_name = IIuiDBA;
	SIprintf("Converting Replicator catalogs on database '%s' ...\n",
		db_name);
	if (RMcheck_cat_level())
	{
/* # line 283 "convrep.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 284 "convrep.sc" */	/* host code */
		FEing_exit();
		SIprintf("Replicator catalogs already at the current release level.  --  Exiting.\n");
		EXdelete();
		PCexit(FAIL);
	}
/* # line 289 "convrep.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 291 "convrep.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iitables where LOWERCASE(table_name)='dd_regist_t\
ables' and table_owner=");
    IIputdomio((short *)0,1,32,0,dba_name);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&exist);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 296 "convrep.sc" */	/* host code */
	if (RPdb_error_check(0, NULL) != OK)
		error_exit();
	if (exist)
		curr_level = REP1_1;
/* # line 301 "convrep.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 302 "convrep.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE session where LEVEL=table, READLOCK=EXCLUSIVE");
    IIsyncup((char *)0,0);
  }
/* # line 305 "convrep.sc" */	/* host code */
	SIprintf("\n    Dropping Change Recorder rules ...\n");
	if (drop_old_rules() != OK)
		error_exit();
	if (curr_level == REP1_0)
	{
		SIprintf("    Creating new catalogs ...\n");
		if (save_old_tables() != OK)
			error_exit();
		if (load_tables() != OK)
			error_exit();
		SIprintf("    Converting old catalogs ...\n");
		if (convert_tables() != OK)
			error_exit();
	}
	SIprintf("    Removing old catalogs ...\n");
	if (drop_old_dict() != OK)
		error_exit();
	SIprintf("    Remodifying catalogs ...\n");
	if (modify_tables(TRUE) != OK)
		error_exit();
/* # line 330 "convrep.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(CAT_LEVEL_SESSION);
    IIsqUser((char *)"$ingres");
    IIsqConnect(0,db_name,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 332 "convrep.sc" */	/* host code */
	if (RPdb_error_check(0, NULL) != OK)
		error_exit();
	if (update_cat_level() != OK)
		error_exit();
/* # line 338 "convrep.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(UI_SESSION),(void *)IILQint(
    UI_SESSION));
  }
/* # line 339 "convrep.sc" */	/* host code */
	if (RPdb_error_check(0, NULL) != OK)
		error_exit();
/* # line 341 "convrep.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 342 "convrep.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(CAT_LEVEL_SESSION),
    (void *)IILQint(CAT_LEVEL_SESSION));
  }
/* # line 343 "convrep.sc" */	/* host code */
	if (RPdb_error_check(0, NULL) != OK)
		error_exit();
/* # line 345 "convrep.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 346 "convrep.sc" */	/* host code */
	SIprintf("    Catalog conversion complete.\n");
/* # line 347 "convrep.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IIsqDisconnect();
  }
/* # line 349 "convrep.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(UI_SESSION),(void *)IILQint(
    UI_SESSION));
  }
/* # line 350 "convrep.sc" */	/* host code */
	if (RPdb_error_check(0, NULL) != OK)
		error_exit();
	SIprintf("\n    Converting support objects ...\n");
	if (curr_level == REP1_0)
	{
		if (convert_supp_objs() != OK)
			error_exit();
	}
	else if (curr_level == REP1_1)
	{
		if (convert_dbprocs() != OK)
			error_exit();
	}
/* # line 363 "convrep.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 364 "convrep.sc" */	/* host code */
	FEing_exit();
	/* free memory */
	if (tbls != NULL)
	{
		MEfree((PTR)tbls);
		tbls = NULL;
	}
	SIprintf("Database '%s' converted successfully.\n", db_name);
	EXdelete();
	PCexit(OK);
}
/*{
** Name:	error_exit - exit on error
**
** Description:
**	Rollback, disconnect and exit on error.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Returns:
**	none
*/
static void
error_exit()
{
/* # line 398 "convrep.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 399 "convrep.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(-99);
    IIsqDisconnect();
  }
/* # line 400 "convrep.sc" */	/* host code */
	SIprintf("Error converting database '%s'.\n", db_name);
	PCexit(FAIL);
}
/*{
** Name:	drop_old_rules - drops REP 1.0 or 1.1 rules
**
** Description:
**	Disable the change recorder by dropping old rules.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Returns:
**	OK or FAIL
*/
static STATUS
drop_old_rules()
{
	char	obj_name[DB_MAXNAME+1];
	i4	i;
	i4	j;
  i4 table_no;
  char table_name[DB_MAXNAME+1];
  char stmt[1024];
	char	*rt_10 = ERx("dd_registered_tables");
	char	*rt_11 = ERx("dd_regist_tables");
	STprintf(stmt, ERx("SELECT COUNT(*) FROM %s"),
		curr_level == REP1_0 ? rt_10 : rt_11);
/* # line 436 "convrep.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqMods(3);
    IIsqMods(1);
    IIsqExImmed(stmt);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&NumTables);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 437 "convrep.sc" */	/* host code */
	if (RPdb_error_check(0, NULL) != OK)
		return (FAIL);
	if (NumTables == 0)
		return (OK);
	/* allocate memory for table names */
	tbls = (struct tagTbl *)MEreqmem(0, (u_i4)(NumTables *
		sizeof(struct tagTbl)), TRUE, NULL);
	if (tbls == NULL)
		return (FAIL);
	/* get table names */
	i = 0;
	STprintf(stmt, ERx("SELECT table_no, TRIM(%s) FROM %s"),
		curr_level == REP1_0 ? ERx("tablename") : ERx("table_name"),
		curr_level == REP1_0 ? rt_10 : rt_11);
/* # line 454 "convrep.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqMods(3);
    IIsqExImmed(stmt);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&table_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,table_name);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 457 "convrep.sc" */	/* host code */
		if (i < NumTables)
		{
			tbls[i].no = table_no;
			STcopy(table_name, tbls[i].name);
		}
		++i;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if (RPdb_error_check(0, NULL) != OK)
		return (FAIL);
	for (i = 0; i < NumTables; ++i)
	{
		for (j = 0; j < DIM(rule_suffixes); ++j)
		{
			if (curr_level == REP1_1)
				STprintf(obj_name, ERx("%.*s%0*d%s"),
					TRUNC_TBLNAME_LEN,
					RPedit_name(EDNM_ALPHA, tbls[i].name,
					NULL), TBLOBJ_TBLNO_NDIGITS,
					(i4)tbls[i].no, rule_suffixes11[j]);
			else
				STprintf(obj_name, ERx("%s__%s"), tbls[i].name,
					rule_suffixes[j]);
			if (drop_object(obj_name, OBJ_RULE) != OK)
				return (FAIL);
		}
	}
	if (curr_level == REP1_1)
	{
/* # line 486 "convrep.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_tables set rules_created=''");
    IIsyncup((char *)0,0);
  }
/* # line 488 "convrep.sc" */	/* host code */
		if (RPdb_error_check(0, NULL) != OK)
			return (FAIL);
	}
/* # line 491 "convrep.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 493 "convrep.sc" */	/* host code */
	return (OK);
}
/*{
** Name:	drop_object - drops a database object
**
** Description:
**	Drops a database object if it exists.
**
** Inputs:
**	obj_name - name of the object
**	obj_type - type of the object
**
** Outputs:
**	none
**
** Returns:
**	OK or FAIL
*/
static STATUS
drop_object(
char	*obj_name,
i4	obj_type)
# if 0
  char *obj_name;
# endif
{
  i4 exist;
  char stmt[80];
	char	*drop_stmt;
	switch (obj_type)
	{
	case OBJ_TABLE:
/* # line 532 "convrep.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"convrep1",7714,20462);
      IIputdomio((short *)0,1,32,0,obj_name);
      IIputdomio((short *)0,1,32,0,dba_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"convrep1",7714,20462);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iitables where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,obj_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,dba_name);
        IIexDefine(0,(char *)"convrep1",7714,20462);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&exist);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 537 "convrep.sc" */	/* host code */
		drop_stmt = ERx("DROP TABLE %s");
		break;
	case OBJ_RULE:
/* # line 541 "convrep.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"convrep2",7715,17898);
      IIputdomio((short *)0,1,32,0,obj_name);
      IIputdomio((short *)0,1,32,0,dba_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"convrep2",7715,17898);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iirules where rule_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,obj_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and rule_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,dba_name);
        IIexDefine(0,(char *)"convrep2",7715,17898);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&exist);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 546 "convrep.sc" */	/* host code */
		drop_stmt = ERx("DROP RULE %s");
		break;
	case OBJ_DBPROC:
/* # line 550 "convrep.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"convrep3",7716,15226);
      IIputdomio((short *)0,1,32,0,obj_name);
      IIputdomio((short *)0,1,32,0,dba_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"convrep3",7716,15226);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiprocedures where procedure_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,obj_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and procedure_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,dba_name);
        IIexDefine(0,(char *)"convrep3",7716,15226);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&exist);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 555 "convrep.sc" */	/* host code */
		drop_stmt = ERx("DROP PROCEDURE %s");
		break;
	default:
		return (FAIL);
	}
	if (exist)
	{
		STprintf(stmt, drop_stmt, obj_name);
/* # line 565 "convrep.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
  }
/* # line 566 "convrep.sc" */	/* host code */
		if (RPdb_error_check(0, NULL) != OK)
			return (FAIL);
	}
	return (OK);
}
/*{
** Name:	save_old_tables - saves neccessary old tables
**
** Description:
**	Saves neccessary old tables (ones that have the same names as in
**	this release) to temporary tables.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Returns:
**	OK or FAIL
*/
static STATUS
save_old_tables()
{
	i4	i;
	char	tmp_table[DB_MAXNAME+1];
  char stmt[80];
  char err_buf[256];
	for (i = 0; i < DIM(replic_tbls_same); ++i)
	{
		STprintf(tmp_table, ERx("%s_tmp"), replic_tbls_same[i]);
		if (drop_object(tmp_table, OBJ_TABLE) != OK)
			return (FAIL);
		STprintf(stmt, ERx("CREATE TABLE %s AS SELECT * FROM %s"),
			tmp_table, replic_tbls_same[i]);
/* # line 607 "convrep.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
  }
/* # line 608 "convrep.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
			return (FAIL);
		if (drop_object(replic_tbls_same[i], OBJ_TABLE) != OK)
			return (FAIL);
	}
	return (OK);
}
/*{
** Name:	convert_tables - converts old tables
**
** Description:
**	Converts old tables.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Returns:
**	OK or FAIL
*/
static STATUS
convert_tables()
{
	DBEC_ERR_INFO	errinfo;
  char ing_ver[DB_MAXNAME+1];
  i4 rel_pre1003;
  i4 col_count;
  i4 max_table_no;
  char unique_rule[2];
  char table_indexes[2];
  short cdds_null_ind;
  short prio_null_ind;
  struct {
    i4 table_no;
    char table_name[DB_MAXNAME+1];
    char table_owner[DB_MAXNAME+1];
    i2 cdds_no;
    char cdds_lookup_table[DB_MAXNAME+1];
    char prio_lookup_table[DB_MAXNAME+1];
    char index_used[DB_MAXNAME+1];
  } reg_tbl;
	/* dd_databases */
/* # line 660 "convrep.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_databases(database_no, database_name, vnode_name, dbms\
_type, local_db, remark)select database_no, database_name, node_name, \
'ingres', 0, dba_comment from dd_connections");
    IIsyncup((char *)0,0);
  }
/* # line 665 "convrep.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
		return (FAIL);
/* # line 668 "convrep.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_databases d from dd_installation i set local_db=1 where d.d\
atabase_no=i.database_no");
    IIsyncup((char *)0,0);
  }
/* # line 672 "convrep.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
		return (FAIL);
	/* dd_regist_columns & dd_regist_tables */
	reg_tbl.table_no = 1;
/* # line 685 "convrep.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"reg_tbl_cursor",7717,30059);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select r.tablename, TRIM(t.table_owner), r.dd_routing, r.rule_lookup_\
table, r.priority_lookup_table, t.unique_rule, t.table_indexes from dd\
_registered_tables r, iitables t where r.tablename=t.table_name");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"reg_tbl_cursor",7717,30059);
  }
/* # line 686 "convrep.sc" */	/* host code */
	if (RPdb_error_check(0, NULL) != OK)
		return (FAIL);
	while (TRUE)
	{
/* # line 690 "convrep.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"reg_tbl_cursor",7717,30059,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,reg_tbl.table_name);
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,reg_tbl.table_owner);
      IIcsGetio((short *)0,1,30,2,&reg_tbl.cdds_no);
      IIcsGetio(&cdds_null_ind,1,32,DB_MAXNAME+1-1,reg_tbl.cdds_lookup_table);
      IIcsGetio(&prio_null_ind,1,32,DB_MAXNAME+1-1,reg_tbl.prio_lookup_table);
      IIcsGetio((short *)0,1,32,1,unique_rule);
      IIcsGetio((short *)0,1,32,1,table_indexes);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 696 "convrep.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
			return (FAIL);
		if (errinfo.rowcount != 1)
			break;
		if (cdds_null_ind == -1)
			*reg_tbl.cdds_lookup_table = EOS;
		if (prio_null_ind == -1)
			*reg_tbl.prio_lookup_table = EOS;
/* # line 706 "convrep.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_regist_tables values(");
    IIputdomio((short *)0,1,30,4,&reg_tbl.table_no);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,reg_tbl.table_name);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,reg_tbl.table_owner);
    IIwritio(0,(short *)0,1,32,0,(char *)", '', '', '', ");
    IIputdomio((short *)0,1,30,2,&reg_tbl.cdds_no);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,reg_tbl.cdds_lookup_table);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,reg_tbl.prio_lookup_table);
    IIwritio(0,(short *)0,1,32,0,(char *)", '')");
    IIsyncup((char *)0,0);
  }
/* # line 713 "convrep.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
			return (FAIL);
/* # line 716 "convrep.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_regist_columns select distinct ");
    IIputdomio((short *)0,1,30,4,&reg_tbl.table_no);
    IIwritio(0,(short *)0,1,32,0,(char *)
", c.column_name, c.column_sequence, 0 from dd_registered_columns r, i\
icolumns c where r.tablename=");
    IIputdomio((short *)0,1,32,0,reg_tbl.table_name);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and r.tablename=c.table_name and r.column_name=c.column_name and c.ta\
ble_owner=");
    IIputdomio((short *)0,1,32,0,reg_tbl.table_owner);
    IIsyncup((char *)0,0);
  }
/* # line 724 "convrep.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
			return (FAIL);
/* # line 727 "convrep.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_columns c from dd_registered_columns r set column_se\
quence=0 where r.tablename=");
    IIputdomio((short *)0,1,32,0,reg_tbl.table_name);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and r.column_name=c.column_name and r.replicated_column!='R'");
    IIsyncup((char *)0,0);
  }
/* # line 733 "convrep.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
			return (FAIL);
		/*
		** Find out which index we're using and update
		** dd_regist_tables and dd_regist_columns accordingly.
		*/
		if (CMcmpcase(unique_rule, ERx("U")) == 0)
		{	/* unique primary key */
			STcopy(reg_tbl.table_name, reg_tbl.index_used);
/* # line 743 "convrep.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"convrep4",7718,32377);
      IIputdomio((short *)0,1,32,0,reg_tbl.table_name);
      IIputdomio((short *)0,1,32,0,reg_tbl.table_owner);
      IIputdomio((short *)0,1,30,4,&reg_tbl.table_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"convrep4",7718,32377);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_columns r from iicolumns c set key_sequence=c.key_se\
quence where c.column_name=r.column_name and c.key_sequence>0 and c.ta\
ble_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,reg_tbl.table_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and c.table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,reg_tbl.table_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)"and r.table_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&reg_tbl.table_no);
        IIexDefine(0,(char *)"convrep4",7718,32377);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 751 "convrep.sc" */	/* host code */
			if (RPdb_error_check(0, NULL) != OK)
				return (FAIL);
		}
		else if (CMcmpcase(table_indexes, ERx("Y")) == 0)
		{	/* secondary index */
/* # line 756 "convrep.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_registered_columns where tablename=");
    IIputdomio((short *)0,1,32,0,reg_tbl.table_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and key_column='K'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&col_count);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 761 "convrep.sc" */	/* host code */
			if (RPdb_error_check(0, NULL) != OK)
				return (FAIL);
/* # line 770 "convrep.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"index_cursor",7719,27112);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select index_name from iiindexes where base_name=");
    IIputdomio((short *)0,1,32,0,reg_tbl.table_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and base_owner=");
    IIputdomio((short *)0,1,32,0,reg_tbl.table_owner);
    IIwritio(0,(short *)0,1,32,0,(char *)"and unique_rule='U'");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"index_cursor",7719,27112);
  }
/* # line 771 "convrep.sc" */	/* host code */
			if (RPdb_error_check(0, NULL) != OK)
				return (FAIL);
			while (TRUE)
			{
/* # line 775 "convrep.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"index_cursor",7719,27112,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,reg_tbl.index_used);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 777 "convrep.sc" */	/* host code */
				if (RPdb_error_check(DBEC_ZERO_ROWS_OK,
						&errinfo) != OK)
					return (FAIL);
				if (errinfo.rowcount != 1)
					break;
/* # line 783 "convrep.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"convrep5",7720,2166);
      IIputdomio((short *)0,1,30,4,&reg_tbl.table_no);
      IIputdomio((short *)0,1,32,0,reg_tbl.index_used);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"convrep5",7720,2166);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_columns r from iiindexes i, iiindex_columns c set ke\
y_sequence=c.key_sequence where r.column_name=c.column_name and r.tabl\
e_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&reg_tbl.table_no);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and c.index_name=i.index_name and c.index_owner=i.index_owner and i.i\
ndex_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,reg_tbl.index_used);
        IIexDefine(0,(char *)"convrep5",7720,2166);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 793 "convrep.sc" */	/* host code */
				if (RPdb_error_check(0, &errinfo) != OK)
					return (FAIL);
				if (errinfo.rowcount == col_count)
					break;
				else
					*reg_tbl.index_used = EOS;
			}
/* # line 800 "convrep.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"index_cursor",7719,27112);
  }
/* # line 801 "convrep.sc" */	/* host code */
			if (RPdb_error_check(0, NULL) != OK)
				return (FAIL);
			if (*reg_tbl.index_used == EOS)
				return (FAIL);
		}
		else	/* no indexes found */
		{
			return (FAIL);
		}
/* # line 812 "convrep.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_tables set columns_registered=DATE('now'), index_use\
d=");
    IIputdomio((short *)0,1,32,0,reg_tbl.index_used);
    IIwritio(0,(short *)0,1,32,0,(char *)"where table_no=");
    IIputdomio((short *)0,1,30,4,&reg_tbl.table_no);
    IIsyncup((char *)0,0);
  }
/* # line 816 "convrep.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
			return (FAIL);
		++reg_tbl.table_no;
	}
/* # line 821 "convrep.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"reg_tbl_cursor",7717,30059);
  }
/* # line 822 "convrep.sc" */	/* host code */
	if (RPdb_error_check(0, NULL) != OK)
		return (FAIL);
	/* dd_paths */
	rel_pre1003 = 0;
/* # line 827 "convrep.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select INT4(1)from iicolumns where table_name='dd_distribution' and c\
olumn_name='local'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&rel_pre1003);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 831 "convrep.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
		return (FAIL);
	if (rel_pre1003)
/* # line 835 "convrep.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_paths(cdds_no, sourcedb, localdb, targetdb)select dd_r\
outing, source, local, target from dd_distribution");
    IIsyncup((char *)0,0);
  }
/* # line 840 "convrep.sc" */	/* host code */
	else
/* # line 841 "convrep.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_paths(cdds_no, sourcedb, localdb, targetdb)select dd_r\
outing, source, localdb, target from dd_distribution");
    IIsyncup((char *)0,0);
  }
/* # line 846 "convrep.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
		return (FAIL);
	/* dd_cdds */
	/* don't insert the default CDDS (0) since load_dict() does this */
/* # line 851 "convrep.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_cdds(cdds_no, cdds_name, collision_mode, error_mode)se\
lect distinct dd_routing, 'CDDS #' +CHAR(dd_routing), 0, 0 from dd_dis\
tribution where dd_routing!=0");
    IIsyncup((char *)0,0);
  }
/* # line 857 "convrep.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
		return (FAIL);
	/* dd_db_cdds */
/* # line 861 "convrep.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_db_cdds(cdds_no, database_no, target_type)select disti\
nct dd_routing, source, 1 from dd_distribution");
    IIsyncup((char *)0,0);
  }
/* # line 864 "convrep.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
		return (FAIL);
/* # line 867 "convrep.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_db_cdds(cdds_no, database_no, target_type)select dd_ro\
uting, target, 1 from dd_distribution d where not exists(select * from\
 dd_db_cdds c where d.target=c.database_no and d.dd_routing=c.cdds_no)");
    IIsyncup((char *)0,0);
  }
/* # line 874 "convrep.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
		return (FAIL);
/* # line 877 "convrep.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_db_cdds d from dd_connections c set target_type=c.target_ty\
pe, server_no=c.server_role where d.database_no=c.database_no");
    IIsyncup((char *)0,0);
  }
/* # line 882 "convrep.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
		return (FAIL);
	/* dd_mail_alert */
/* # line 886 "convrep.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_mail_alert(mail_username)select distinct mail_username\
 from dd_mail_alert_tmp");
    IIsyncup((char *)0,0);
  }
/* # line 889 "convrep.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
		return (FAIL);
	/* dd_input_queue */
/* # line 893 "convrep.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_input_queue select database_no, transaction_id, sequen\
ce_no, trans_type, r.table_no, old_database_no, old_transaction_id, ol\
d_sequence_no, trans_time, dd_routing, dd_priority from dd_input_queue\
_tmp i, dd_regist_tables r where i.tablename=r");
    IIwritio(0,(short *)0,1,32,0,(char *)".table_name");
    IIsyncup((char *)0,0);
  }
/* # line 899 "convrep.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
		return (FAIL);
	/* dd_distrib_queue */
/* # line 903 "convrep.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_distrib_queue select target, database_no, transaction_\
id, sequence_no, trans_type, r.table_no, old_database_no, old_transact\
ion_id, old_sequence_no, trans_time, dd_routing, dd_priority from dd_d\
istribution_queue d, dd_regist_tables r where ");
    IIwritio(0,(short *)0,1,32,0,(char *)"d.tablename=r.table_name");
    IIsyncup((char *)0,0);
  }
/* # line 910 "convrep.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
		return (FAIL);
	/* dd_last_number */
/* # line 914 "convrep.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select max(table_no)from dd_regist_tables");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&max_table_no);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 917 "convrep.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_last_number set last_number=");
    IIputdomio((short *)0,1,30,4,&max_table_no);
    IIwritio(0,(short *)0,1,32,0,(char *)"where column_name='table_no'");
    IIsyncup((char *)0,0);
  }
/* # line 920 "convrep.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
		return (FAIL);
/* # line 923 "convrep.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_last_number from dd_distrib_transaction set last_number=nex\
t_transaction_id where column_name='next_transaction_id'");
    IIsyncup((char *)0,0);
  }
/* # line 927 "convrep.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
		return (FAIL);
	return (OK);
}
/*{
** Name:	drop_old_dict - drops old data dictionary
**
** Description:
**	Drops old data dictionary.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Returns:
**	OK or FAIL
*/
static STATUS
drop_old_dict()
{
	char	obj_name[DB_MAXNAME+1];
	i4	i;
	i4	j;
	/* drop 1.1 tables */
	for (i = 0; i < DIM(replic_tbls2); ++i)
		if (drop_object(replic_tbls2[i], OBJ_TABLE) != OK)
			return (FAIL);
/* # line 961 "convrep.sc" */	/* drop dbevent */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop dbevent dd_dispatch");
    IIsyncup((char *)0,0);
  }
/* # line 962 "convrep.sc" */	/* host code */
	if (RPdb_error_check(0, NULL) != OK)
		return (FAIL);
	if (curr_level == REP1_1)
		return (OK);
	/* drop tables */
	for (i = 0; i < DIM(replic_tbls); ++i)
		if (drop_object(replic_tbls[i], OBJ_TABLE) != OK)
			return (FAIL);
	/* drop temp tables */
	for (i = 0; i < DIM(replic_tbls_same); ++i)
	{
		STprintf(obj_name, ERx("%s_tmp"), replic_tbls_same[i]);
		if (drop_object(obj_name, OBJ_TABLE) != OK)
			return (FAIL);
	}
	return (OK);
}
/*{
** Name:	convert_supp_objs - converts support objects
**
** Description:
**	Converts shadow and archive tables and creates database procedures.
**	Old support objects will be dropped whether the conversion went
**	successfully or not.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Returns:
**	OK or FAIL
*/
static STATUS
convert_supp_objs()
{
	DBEC_ERR_INFO	errinfo;
	i4	err;
	i4	i;
	i4	j;
  char table_name[DB_MAXNAME+1];
  char obj_name[DB_MAXNAME+1];
  i4 table_no;
  char stmt[STMT_SIZE];
  char cols[STMT_SIZE];
	if (tbls == NULL)
	{
		if (NumTables == 0)
			return (OK);
		else
			return (FAIL);
	}
	for (i = 0; i < NumTables; ++i)
	{
		STcopy(tbls[i].name, table_name);
/* # line 1028 "convrep.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select table_no from dd_regist_tables where table_name=");
    IIputdomio((short *)0,1,32,0,table_name);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&table_no);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1032 "convrep.sc" */	/* host code */
		if (RPdb_error_check(0, NULL) != OK)
			return (FAIL);
		if (RMtbl_fetch(table_no, TRUE))
			return (FAIL);
		if (*RMtbl->columns_registered == EOS)
		{
			IIUGerr(E_RM0033_Must_reg_before_supp, UG_ERR_ERROR, 0);
			return (FAIL);
		}
		err = create_support_tables(table_no);
		if (err == OK || err == -1)
		{
			COLDEF	*col_p;
			/* convert shadow table */
			*cols = EOS;
			for (j = 0, col_p = RMtbl->col_p; j < RMtbl->ncols;
				++j, *col_p++)
			{
				if (col_p->key_sequence > 0)
				{
					STcat(cols, col_p->dlm_column_name);
					STcat(cols, ERx(", "));
				}
			}
			STprintf(stmt,
				ERx("INSERT INTO %s (%s%s) SELECT %s%s FROM %s_s"),
				RMtbl->sha_name, cols, new_shadow_cols, cols,
				old_shadow_cols, tbls[i].name);
/* # line 1065 "convrep.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
  }
/* # line 1066 "convrep.sc" */	/* host code */
			if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
				return (FAIL);
			/* convert archive table */
			*cols = EOS;
			for (j = 0, col_p = RMtbl->col_p; j < RMtbl->ncols;
				++j, *col_p++)
			{
				if (col_p->column_sequence)
				{
					STcat(cols, col_p->dlm_column_name);
					STcat(cols, ERx(", "));
				}
			}
			STprintf(stmt,
				ERx("INSERT INTO %s (%s%s) SELECT %s%s FROM %s_a"),
				RMtbl->arc_name, cols, new_archive_cols, cols,
				old_archive_cols, tbls[i].name);
/* # line 1084 "convrep.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
  }
/* # line 1085 "convrep.sc" */	/* host code */
			if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
				return (FAIL);
		}
		/* drop old support tables */
		STprintf(obj_name, ERx("%s_s"), tbls[i].name);
		if (drop_object(obj_name, OBJ_TABLE) != OK)
			return (FAIL);
		STprintf(obj_name, ERx("%s_a"), tbls[i].name);
		if (drop_object(obj_name, OBJ_TABLE) != OK)
			return (FAIL);
		/* drop old procedures */
		for (j = 0; j < DIM(proc_suffixes); ++j)
		{
			STprintf(obj_name, ERx("%s__%s"), tbls[i].name,
				proc_suffixes[j]);
			if (drop_object(obj_name, OBJ_DBPROC) != OK)
				return (FAIL);
		}
		/* create new database procedures */
		err = tbl_dbprocs(table_no);
		if (!err)
		{
/* # line 1110 "convrep.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_tables set supp_objs_created=DATE('now')where table_\
no=");
    IIputdomio((short *)0,1,30,4,&table_no);
    IIsyncup((char *)0,0);
  }
/* # line 1113 "convrep.sc" */	/* host code */
			if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
				return (FAIL);
		}
/* # line 1116 "convrep.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 1117 "convrep.sc" */	/* host code */
	}
	return (OK);
}
/*{
** Name:	convert_dbprocs - converts database procedures
**
** Description:
**	Destroys 1.1 local procedures and recreates remote procedures.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Returns:
**	OK or FAIL
*/
static STATUS
convert_dbprocs()
{
	i4	i;
	i4	j;
	i4	err;
	DBEC_ERR_INFO	errinfo;
	char	obj_name[DB_MAXNAME+1];
  i4 table_no;
	if (tbls == NULL)
	{
		if (NumTables == 0)
			return (OK);
		else
			return (FAIL);
	}
	/* force database procedures to be recreated */
/* # line 1159 "convrep.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_tables set columns_registered=DATE('now')");
    IIsyncup((char *)0,0);
  }
/* # line 1161 "convrep.sc" */	/* host code */
	if (RPdb_error_check(0, NULL) != OK)
		return (FAIL);
	for (i = 0; i < NumTables; ++i)
	{
		/* drop local procedures */
		for (j = 0; j < DIM(proc_suffixes11); ++j)
		{
			STprintf(obj_name, ERx("%.*s%0*d%s"), TRUNC_TBLNAME_LEN,
				RPedit_name(EDNM_ALPHA, tbls[i].name, NULL),
				TBLOBJ_TBLNO_NDIGITS, (i4)tbls[i].no, 
				proc_suffixes11[j]);
			if (drop_object(obj_name, OBJ_DBPROC) != OK)
				return (FAIL);
		}
		table_no = tbls[i].no;
		if (RMtbl_fetch(table_no, TRUE))
			return (FAIL);
		/* recreate remote database procedures */
		err = tbl_dbprocs(table_no);
		if (!err)
		{
/* # line 1185 "convrep.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_tables set supp_objs_created=DATE('now')where table_\
no=");
    IIputdomio((short *)0,1,30,4,&table_no);
    IIsyncup((char *)0,0);
  }
/* # line 1188 "convrep.sc" */	/* host code */
			if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
				return (FAIL);
		}
/* # line 1191 "convrep.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 1192 "convrep.sc" */	/* host code */
	}
	return (OK);
}
