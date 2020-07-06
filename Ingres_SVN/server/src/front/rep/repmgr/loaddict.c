# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <st.h>
# include <nm.h>
# include <lo.h>
# include <pc.h>
# include <er.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <ug.h>
# include <rpu.h>
# include "errm.h"
/**
** Name:	loaddict.sc - Load data dictionary objects
**
** Description:
**	Defines
**		load_tables		- create and load dictionary tables
**		update_cat_level	- update Replicator catalog level
**		RMcheck_cat_level	- check Replicator catalog level
**
** History:
**	16-dec-96 (joea)
**		Created based on loaddict.sc in replicator library.
**	14-jan-97 (joea)
**		Remove dd_target_types.
**	04-feb-97 (joea)
**		In update_cat_level, also update if level is current.
**	08-apr-97 (mcgem01)
**		Changed CA-OpenIngres to OpenIngres.
**	20-apr-98 (mcgem01)
**		Product name change to Ingres.
**	16-jun-98 (abbjo03)
**		Move individual calls in load_dict to repcat and convrep, and
**		eliminate load_dict.
**	23-sep-98 (abbjo03)
**		Replace ddba_messageit with IIUGerr.
**	03-sep-99 (abbjo03) sir 98644
**		Remove NODUPLICATES clause from input and distribution queues.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	10-Jul-2007 (kibro01) b118702
**	    Ensure internal dd_x tables always use ingresdate
**/
# define CURR_CATALOG_LEVEL "00200"
# define	MAX_SERVERS		10
# define	SERVER_NAME		ERx("dd_server")
# define	DIM(a)			(sizeof(a) / sizeof(a[0]))
STATUS update_cat_level(void);
  static struct {
    char *type;
    char *descrip;
    i2 gateway;
  } dbms_types[]=
{
	{ERx("alb"),		ERx("HP Allbase Gateway"),	1},
	{ERx("db2"),		ERx("IBM DB2 Gateway"),		1},
	{ERx("dcom"),		ERx("CA-Datacom"),		1},
	{ERx("idms"),		ERx("IDMS Gateway"),		1},
	{ERx("ims"),		ERx("IMS Gateway"),		1},
	{ERx("informix"),	ERx("Informix Gateway"),	1},
	{ERx("ingres"),		ERx("Ingres"),			0},
	{ERx("mssql"),		ERx("MS SQL Server"),		1},
	{ERx("opingdt"),	ERx("OpenIngres Desktop"),	1},
	{ERx("oracle"),		ERx("Oracle Gateway"),		1},
	{ERx("rdb"),		ERx("Rdb Gateway"),		1},
	{ERx("rms"),		ERx("DEC RMS Gateway"),		1},
	{ERx("sybase"),		ERx("Sybase Gateway"),		1},
	{ERx("vsam"),		ERx("VSAM Gateway"),		1},
};
  static char *supp_tbls[]=
{
	ERx("dd_nodes"),
	ERx("dd_databases"),
	ERx("dd_paths"),
	ERx("dd_cdds"),
	ERx("dd_db_cdds"),
	ERx("dd_regist_tables"),
	ERx("dd_regist_columns"),
	ERx("dd_input_queue"),
	ERx("dd_distrib_queue"),
	ERx("dd_mail_alert"),
	ERx("dd_servers"),
	ERx("dd_support_tables"),
	ERx("dd_last_number"),
	ERx("dd_events"),
	ERx("dd_flag_values"),
	ERx("dd_option_values"),
	ERx("dd_server_flags"),
	ERx("dd_dbms_types"),
};
/*{
** Name:	load_tables - create and load data dictionary tables
**
** Description:
**	Creates and loads the Replicator data dictionary tables.
**	Replicator DD tables are created.  Some of them are loaded with
**	static information.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Returns:
**	OK on success, FAIL if there is a database error.
*/
STATUS
load_tables()
{
	DBEC_ERR_INFO	errinfo;
	char	*p;
  i4 i;
  char server_name[25];
  char datafile[MAX_LOC+1];
	LOCATION	loc;
	i4	tbl = 0;
	if (NMloc(FILES, PATH, NULL, &loc) != OK)
	{
		SIprintf("II_CONFIG not defined\n");
		return (FAIL);
	}
	if (LOfaddpath(&loc, ERx("rep"), &loc) != OK)
	{
		SIprintf("Error getting Replicator files directory\n");
		return (FAIL);
	}
/* # line 155 "loaddict.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_nodes(vnode_name CHAR(32)not null, node_type SMALLINT\
 not null with default)with NODUPLICATES, JOURNALING");
    IIsyncup((char *)0,0);
  }
/* # line 159 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		IIUGerr(E_RM00D8_Err_creating_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	++tbl;
/* # line 167 "loaddict.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_databases(database_no SMALLINT not null, vnode_name C\
HAR(32)not null, database_name CHAR(32)not null, database_owner CHAR(3\
2)not null with default, dbms_type CHAR(8)not null with default, secur\
ity_level CHAR(2)not null with default, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"local_db SMALLINT not null with default, config_changed CHAR(25)not n\
ull with default, remark VARCHAR(80)not null with default)with NODUPLI\
CATES, JOURNALING");
    IIsyncup((char *)0,0);
  }
/* # line 178 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		IIUGerr(E_RM00D8_Err_creating_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	++tbl;
/* # line 186 "loaddict.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_paths(cdds_no SMALLINT not null, localdb SMALLINT not\
 null, sourcedb SMALLINT not null, targetdb SMALLINT not null, final_t\
arget SMALLINT not null with default)with NODUPLICATES, JOURNALING");
    IIsyncup((char *)0,0);
  }
/* # line 193 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		IIUGerr(E_RM00D8_Err_creating_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	++tbl;
/* # line 201 "loaddict.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_cdds(cdds_no SMALLINT not null, cdds_name CHAR(32)not\
 null, control_db SMALLINT not null with default, collision_mode SMALL\
INT not null with default, error_mode SMALLINT not null with default)w\
ith NODUPLICATES, JOURNALING");
    IIsyncup((char *)0,0);
  }
/* # line 208 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		IIUGerr(E_RM00D8_Err_creating_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
/* # line 215 "loaddict.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_cdds values(0, 'Default CDDS', 0, 0, 0)");
    IIsyncup((char *)0,0);
  }
/* # line 217 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
	{
		IIUGerr(E_RM00D9_Err_loading_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	++tbl;
/* # line 225 "loaddict.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_db_cdds(cdds_no SMALLINT not null, database_no SMALLI\
NT not null, target_type SMALLINT not null, is_quiet SMALLINT not null\
 with default, server_no SMALLINT not null with default)with NODUPLICA\
TES, JOURNALING");
    IIsyncup((char *)0,0);
  }
/* # line 232 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		IIUGerr(E_RM00D8_Err_creating_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	++tbl;
/* # line 240 "loaddict.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_regist_tables(table_no INTEGER not null, table_name C\
HAR(32)not null, table_owner CHAR(32)not null with default, columns_re\
gistered CHAR(25)not null with default, supp_objs_created CHAR(25)not \
null with default, rules_created CHAR(25)not ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"null with default, cdds_no SMALLINT not null with default, cdds_looku\
p_table CHAR(32)not null with default, prio_lookup_table CHAR(32)not n\
ull with default, index_used CHAR(32)not null with default)with NODUPL\
ICATES, JOURNALING");
    IIsyncup((char *)0,0);
  }
/* # line 252 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		IIUGerr(E_RM00D8_Err_creating_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	++tbl;
/* # line 260 "loaddict.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_regist_columns(table_no INTEGER not null, column_name\
 CHAR(32)not null, column_sequence INTEGER not null, key_sequence INTE\
GER not null with default)with NODUPLICATES, JOURNALING");
    IIsyncup((char *)0,0);
  }
/* # line 266 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		IIUGerr(E_RM00D8_Err_creating_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	++tbl;
/* # line 274 "loaddict.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_input_queue(sourcedb SMALLINT not null, transaction_i\
d INTEGER not null, sequence_no INTEGER not null, trans_type SMALLINT \
not null, table_no INTEGER not null, old_sourcedb SMALLINT not null, o\
ld_transaction_id INTEGER not null, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"old_sequence_no INTEGER not null, trans_time INGRESDATE with null, cd\
ds_no SMALLINT not null with default, dd_priority SMALLINT not null wi\
th default)with JOURNALING");
    IIsyncup((char *)0,0);
  }
/* # line 287 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		IIUGerr(E_RM00D8_Err_creating_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	++tbl;
/* # line 295 "loaddict.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_distrib_queue(targetdb SMALLINT not null, sourcedb SM\
ALLINT not null, transaction_id INTEGER not null, sequence_no INTEGER \
not null, trans_type SMALLINT not null, table_no INTEGER not null, old\
_sourcedb SMALLINT not null, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"old_transaction_id INTEGER not null, old_sequence_no INTEGER not null\
, trans_time INGRESDATE not null, cdds_no SMALLINT not null with defau\
lt, dd_priority SMALLINT not null with default)with JOURNALING");
    IIsyncup((char *)0,0);
  }
/* # line 309 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		IIUGerr(E_RM00D8_Err_creating_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	++tbl;
/* # line 317 "loaddict.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_mail_alert(mail_username VARCHAR(80)not null)with NOD\
UPLICATES, JOURNALING");
    IIsyncup((char *)0,0);
  }
/* # line 320 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		IIUGerr(E_RM00D8_Err_creating_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	++tbl;
/* # line 328 "loaddict.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_servers(server_no SMALLINT not null, server_name VARC\
HAR(24)not null, pid VARCHAR(12)not null with default)with NODUPLICATE\
S, JOURNALING");
    IIsyncup((char *)0,0);
  }
/* # line 333 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		IIUGerr(E_RM00D8_Err_creating_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	for (i = 1; i <= MAX_SERVERS; ++i)
	{
		STprintf(server_name, ERx("%s%d"), SERVER_NAME, i);
/* # line 343 "loaddict.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"insert into dd_servers value\
s(");
    IIputdomio((short *)0,1,30,4,&i);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,server_name);
    IIwritio(0,(short *)0,1,32,0,(char *)", '')");
    IIsyncup((char *)0,0);
  }
/* # line 345 "loaddict.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
		{
			IIUGerr(E_RM00D9_Err_loading_cat, UG_ERR_ERROR, 1,
				supp_tbls[tbl]);
			return (FAIL);
		}
	}
	++tbl;
/* # line 354 "loaddict.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_support_tables(table_name CHAR(32)not null)with NODUP\
LICATES, JOURNALING");
    IIsyncup((char *)0,0);
  }
/* # line 357 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		IIUGerr(E_RM00D8_Err_creating_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	for (i = 0; i < DIM(supp_tbls); ++i)
	{
/* # line 366 "loaddict.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_support_tables values(");
    IIputdomio((short *)0,1,32,0,supp_tbls[i]);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 368 "loaddict.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
		{
			IIUGerr(E_RM00D9_Err_loading_cat, UG_ERR_ERROR, 1,
				supp_tbls[tbl]);
			return (FAIL);
		}
	}
	++tbl;
/* # line 377 "loaddict.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_last_number(column_name CHAR(32)not null, last_number\
 INTEGER not null, filler VARCHAR(1500)not null with default)with NODU\
PLICATES");
    IIsyncup((char *)0,0);
  }
/* # line 382 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		IIUGerr(E_RM00D8_Err_creating_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
/* # line 389 "loaddict.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_last_number values('table_no', 0, '')");
    IIsyncup((char *)0,0);
  }
/* # line 391 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
	{
		IIUGerr(E_RM00D9_Err_loading_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
/* # line 398 "loaddict.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_last_number values('next_transaction_id', 1, '')");
    IIsyncup((char *)0,0);
  }
/* # line 400 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
	{
		IIUGerr(E_RM00D9_Err_loading_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	++tbl;
/* # line 408 "loaddict.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_events(dbevent CHAR(32)not null, action SMALLINT not \
null, sort_order INTEGER not null with default, s_flag SMALLINT not nu\
ll, event_desc VARCHAR(80)not null with default)with NODUPLICATES, JOU\
RNALING");
    IIsyncup((char *)0,0);
  }
/* # line 415 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		IIUGerr(E_RM00D8_Err_creating_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	LOfstfile(ERx("events.dat"), &loc);
	LOtos(&loc, &p);
	STcopy(p, datafile);
/* # line 425 "loaddict.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"copy table dd_events(dbevent=c0tab, action=c0tab, sort_order=c0tab, s\
_flag=c0tab, event_desc=c0nl, nl=d1)from ");
    IIvarstrio((short *)0,1,32,0,datafile);
    IIsyncup((char *)0,0);
  }
/* # line 433 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo) != OK)
	{
		IIUGerr(E_RM00D9_Err_loading_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	++tbl;
/* # line 441 "loaddict.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_flag_values(startup_flag VARCHAR(6)not null, startup_\
value VARCHAR(8)not null, flag_description VARCHAR(500)not null)with N\
ODUPLICATES, JOURNALING");
    IIsyncup((char *)0,0);
  }
/* # line 446 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		IIUGerr(E_RM00D8_Err_creating_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	LOfstfile(ERx("flagvals.dat"), &loc);
	LOtos(&loc, &p);
	STcopy(p, datafile);
/* # line 456 "loaddict.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"copy table dd_flag_values(startup_flag=c0tab, startup_value=c0tab, fl\
ag_description=c0nl, nl=d1)from ");
    IIvarstrio((short *)0,1,32,0,datafile);
    IIsyncup((char *)0,0);
  }
/* # line 462 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo) != OK)
	{
		IIUGerr(E_RM00D9_Err_loading_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	++tbl;
/* # line 470 "loaddict.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_option_values(option_type CHAR(1)not null, option_nam\
e CHAR(20)not null, numeric_value INTEGER not null, alpha_value CHAR(2\
0)not null, option_desc VARCHAR(250)not null with default, startup_fla\
g CHAR(1)not null, flag_name CHAR(6)not null ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"not default)with NODUPLICATES, JOURNALING");
    IIsyncup((char *)0,0);
  }
/* # line 479 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		IIUGerr(E_RM00D8_Err_creating_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	LOfstfile(ERx("optnvals.dat"), &loc);
	LOtos(&loc, &p);
	STcopy(p, datafile);
/* # line 489 "loaddict.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"copy table dd_option_values(option_type=c0tab, option_name=c0tab, num\
eric_value=c0tab, alpha_value=c0tab, option_desc=c0tab, startup_flag=c\
0tab, flag_name=c0nl, nl=d1)from ");
    IIvarstrio((short *)0,1,32,0,datafile);
    IIsyncup((char *)0,0);
  }
/* # line 499 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo) != OK)
	{
		IIUGerr(E_RM00D9_Err_loading_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	++tbl;
/* # line 507 "loaddict.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_server_flags(flag_name CHAR(6)not null, option_name C\
HAR(20)not null, short_description CHAR(20)not null, flag_description \
VARCHAR(500)not null with default, startup_flag CHAR(1)not null)with N\
ODUPLICATES, JOURNALING");
    IIsyncup((char *)0,0);
  }
/* # line 514 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		IIUGerr(E_RM00D8_Err_creating_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	LOfstfile(ERx("svrflags.dat"), &loc);
	LOtos(&loc, &p);
	STcopy(p, datafile);
/* # line 524 "loaddict.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"copy table dd_server_flags(flag_name=c0tab, option_name=c0tab, short_\
description=c0tab, flag_description=c0tab, startup_flag=c0nl, nl=d1)fr\
om ");
    IIvarstrio((short *)0,1,32,0,datafile);
    IIsyncup((char *)0,0);
  }
/* # line 532 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo) != OK)
	{
		IIUGerr(E_RM00D9_Err_loading_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	++tbl;
/* # line 540 "loaddict.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_dbms_types(dbms_type CHAR(8)not null, short_descripti\
on VARCHAR(20)not null, gateway SMALLINT not null with default)with NO\
DUPLICATES, JOURNALING");
    IIsyncup((char *)0,0);
  }
/* # line 545 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		IIUGerr(E_RM00D8_Err_creating_cat, UG_ERR_ERROR, 1,
			supp_tbls[tbl]);
		return (FAIL);
	}
	for (i = 0; i < DIM(dbms_types); ++i)
	{
/* # line 554 "loaddict.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_dbms_types values(");
    IIputdomio((short *)0,1,32,0,dbms_types[i].type);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,dbms_types[i].descrip);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,2,&dbms_types[i].gateway);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 557 "loaddict.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
		{
			IIUGerr(E_RM00D9_Err_loading_cat, UG_ERR_ERROR, 1,
				supp_tbls[tbl]);
			return (FAIL);
		}
	}
	return (OK);
}
/*{
** Name:	update_cat_level - Update Replicator catalog level
**
** Description:
**	Inserts/updates Replicator catalog level in iidbcapabilities.
**	Callers are responsible for connecting as $ingres and committing
**	the update/insert.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Returns:
**	OK on success, FAIL if there is a database error.
*/
STATUS
update_cat_level()
{
	DBEC_ERR_INFO	errinfo;
/* # line 591 "loaddict.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iidbcapabilities set cap_value=");
    IIputdomio((short *)0,1,32,5,CURR_CATALOG_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)
"where cap_capability='REP_CATALOG_LEVEL' and cap_value<=");
    IIputdomio((short *)0,1,32,5,CURR_CATALOG_LEVEL);
    IIsyncup((char *)0,0);
  }
/* # line 595 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		return (FAIL);
	if (errinfo.rowcount == 0)
	{
/* # line 599 "loaddict.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbcapabilities values('REP_CATALOG_LEVEL', ");
    IIputdomio((short *)0,1,32,5,CURR_CATALOG_LEVEL);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 601 "loaddict.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
			return (FAIL);
	}
	return (OK);
}
/*{
** Name:	RMcheck_cat_level - check Replicator catalog level
**
** Description:
**	Checks if the data dictionary catalogs are up to the current
**	release level.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Returns:
**	TRUE if release level OK, FALSE otherwise
*/
bool
RMcheck_cat_level()
{
  i4 catalogs_exist;
/* # line 632 "loaddict.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iidbcapabilities where cap_capability='REP_CATALO\
G_LEVEL' and cap_value>=");
    IIputdomio((short *)0,1,32,5,CURR_CATALOG_LEVEL);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&catalogs_exist);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 637 "loaddict.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK ||
			!catalogs_exist)
		return (FALSE);
	else
		return (TRUE);
}
