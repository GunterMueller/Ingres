# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <stdarg.h>
# include <compat.h>
# include <cm.h>
# include <er.h>
# include <lo.h>
# include <si.h>
# include <st.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <ug.h>
# include <rpu.h>
# include "errm.h"
/**
** Name:	ckdstcfg.sc - check distributed configuration
**
** Description:
**	Defines
**		check_distrib_config	- check distributed configuration
**		check_remote_config	- check remote database configuration
**		open_remote_db		- open a remote database
**		check_databases		- check databases
**		check_regist_tables	- check registered tables
**		check_regist_columns	- check registered columns
**		check_cdds		- check cdds
**		check_db_cdds		- check db_cdds
**		check_paths		- check paths
**		set_session		- set session
**		rpt_error_display	- display report error
**
** History:
**	16-dec-96 (joea)
**		Created based on ckdstcfg.sc in replicator library.
**	01-oct-98 (abbjo03)
**		Replace ddba_messageit with IIUGerr. Added rpt_error_display to
**		simplify error reporting code.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
static void check_remote_config(char *l_db, char *r_db, i2 r_dbno);
static STATUS open_remote_db(char *r_db, i4 r_sess);
static STATUS check_databases(i4 sess_a, char *db_a, i4 sess_b, char *db_b,
	i4 *n_err);
static STATUS check_regist_tables(i4 sess_a, char *db_a, i4 sess_b, char *db_b,
	i4 *n_err);
static STATUS check_regist_columns(i4 sess_a, char *db_a, i4 sess_b, char *db_b,
	i4 *n_err);
static STATUS check_cdds(i4 sess_a, char *db_a, i4 sess_b, char *db_b,
	i4 *n_err);
static STATUS check_db_cdds(i4 sess_a, char *db_a, i4 sess_b, char *db_b,
	i4 *n_err);
static STATUS check_paths(i4 sess_a, char *db_a, i4 sess_b, char *db_b,
	i4 *n_err);
static STATUS set_session(i4 sess_no);
static STATUS rpt_error_display(ER_MSGID err_code, bool display, i4 errorno,
	i4 parcount, ...);
static FILE	*report_fp = NULL;
  static i4 Local_Session;
/*{
** Name:	check_distrib_config - check distributed configuration
**
** Description:
**	Create a report showing any problems with the configuration
**	in databases in the dd_connections table.
**
** Inputs:
**	filename	- full path and name of the output file
**
** Outputs:
**	none
**
** Returns:
**	0 - OK
**	else - Error
*/
STATUS
check_distrib_config(
char	*filename)
{
  char remote_vnode[DB_MAXNAME+1];
  char remote_dbname[DB_MAXNAME+1];
  char remote_db[DB_MAXNAME*2+3];
  i2 remote_dbno;
  char local_dbname[DB_MAXNAME+1];
  char report_date[26];
  i4 cnt;
  i4 done = 0;
  STATUS err = OK;
  char tmp[1024];
	LOCATION	loc;
	char		report_filename[MAX_LOC+1];
	DBEC_ERR_INFO	errinfo;
	bool		c1_open = FALSE;
	i4		i;
	u_i4		un;
	char		*p;
/* # line 114 "ckdstcfg.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&Local_Session,11);
  }
/* # line 116 "ckdstcfg.sc" */	/* host code */
	STcopy(filename, report_filename);
	LOfroms(PATH & FILENAME, report_filename, &loc);
	if (SIfopen(&loc, ERx("w"), SI_TXT, SI_MAX_TXT_REC, &report_fp) != OK)
	{
		IIUGerr(E_RM0087_Err_open_report, UG_ERR_ERROR, 1,
			report_filename);
		return (E_RM0087_Err_open_report);
	}
/* # line 125 "ckdstcfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select DBMSINFO('DATABASE'), DATE('now')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,local_dbname);
      IIgetdomio((short *)0,1,32,25,report_date);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 127 "ckdstcfg.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
		err = rpt_error_display(E_RM0088_Err_rtrv_db_name, TRUE,
			errinfo.errorno, 0);
	if (!err)
	{
		p = ERget(F_RM00A9_Prod_name);
		un = 40 + STlength(p) / (u_i4)2;
		SIfprintf(report_fp, ERx("%*s\n"), un, p);
		p = ERget(F_RM00AA_Distrib_Cfg_Checker);
		un = 40 + STlength(p) / (u_i4)2;
		SIfprintf(report_fp, ERx("%*s\n"), un, p);
		STtrmwhite(local_dbname);
		STprintf(tmp, ERget(F_RM00AB_From_local_db), local_dbname);
		un = 40 + STlength(tmp) / (u_i4)2;
		SIfprintf(report_fp, ERx("%*s\n"), un, tmp);
		STtrmwhite(report_date);
		un = 40 + STlength(report_date) / (u_i4)2;
		SIfprintf(report_fp, ERx("%*s\n"), un, report_date);
/* # line 147 "ckdstcfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select count(*)from dd_paths");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 150 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
			err = rpt_error_display(E_RM0089_Err_check_paths, TRUE,
				errinfo.errorno, 0);
		else if (cnt == 0)
			SIfprintf(report_fp, ERx("%s\n"),
				ERget(E_RM008A_No_paths));
	}
	if (!err)
	{
/* # line 168 "ckdstcfg.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"C1",7209,7933);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct d.vnode_name, d.database_name, d.database_no from dd_\
databases d, dd_db_cdds c where d.local_db=0 and d.database_no=c.datab\
ase_no and c.target_type in(1, 2)order by database_no");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"C1",7209,7933);
  }
/* # line 169 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
			err = rpt_error_display(E_RM008B_Err_open_dbinfo_curs,
				TRUE, errinfo.errorno, 0);
		else
			c1_open = TRUE;
	}
	while (!err && !done)
	{
		if ((err = set_session(Local_Session)) != OK)
			continue;
/* # line 180 "ckdstcfg.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"C1",7209,7933,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,remote_vnode);
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,remote_dbname);
      IIcsGetio((short *)0,1,30,2,&remote_dbno);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 182 "ckdstcfg.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 183 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM008C_Err_fetch_dbinfo_curs,
				TRUE, errinfo.errorno, 0);
			continue;
		}
		else if (done)
		{
			continue;
		}
		STtrmwhite(remote_vnode);
		STtrmwhite(remote_dbname);
		if (*remote_vnode == EOS)
			STprintf(remote_db, ERx("%s"), remote_dbname);
		else
			STprintf(remote_db, ERx("%s::%s"), remote_vnode,
				remote_dbname);
		IIUGmsg(remote_db, FALSE, 0);
		check_remote_config(local_dbname, remote_db, remote_dbno);
	}
	if ((i = set_session(Local_Session)) != OK)
	{
		if (!err)
			err = i;
	}
	else
	{
		if (c1_open)
		{
/* # line 215 "ckdstcfg.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"C1",7209,7933);
  }
/* # line 216 "ckdstcfg.sc" */	/* host code */
			if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
				err = rpt_error_display(
					E_RM008D_Err_close_dbinfo_curs, TRUE,
					errinfo.errorno, 0);
		}
		if (err)
		{
/* # line 224 "ckdstcfg.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 225 "ckdstcfg.sc" */	/* host code */
			if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
				err = rpt_error_display(
					E_RM008E_Err_local_rollback, TRUE,
					errinfo.errorno, 0);
		}
		else
		{
/* # line 232 "ckdstcfg.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 233 "ckdstcfg.sc" */	/* host code */
			if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
				err = rpt_error_display(
					E_RM008F_Err_local_commit, TRUE,
					errinfo.errorno, 0);
		}
	}
	SIclose(report_fp);
	return (err);
}
/*{
** Name:	check_remote_config - check remote database configuration
**
** Description:
**	Check a remote database configuration against the local one.
**
** Inputs:
**	l_db	- local database name
**	r_db	- remote database name
**	r_dbno	- remote database number
**
** Outputs:
**	none
**
** Returns:
**	none
*/
static void
check_remote_config(
char	*l_db,
char	*r_db,
i2	r_dbno)
{
  i4 l_sess = Local_Session;
  i4 r_sess = (i4)r_dbno;
  char tmp[1024];
	bool	remote_open = FALSE;
	i4	n_err = 0;
	STATUS	err = 0;
	STprintf(tmp, ERget(F_RM00AC_Report_on_remote_db), r_db, r_dbno);
	SIfprintf(report_fp, ERx("\n\n%s\n"), tmp);
	err = open_remote_db(r_db, r_sess);
	if (!err)
	{
		remote_open = TRUE;
		err = check_databases(l_sess, l_db, r_sess, r_db, &n_err) ||
			check_databases(r_sess, r_db, l_sess, l_db, &n_err) ||
			check_cdds(l_sess, l_db, r_sess, r_db, &n_err) ||
			check_cdds(r_sess, r_db, l_sess, l_db, &n_err) ||
			check_db_cdds(l_sess, l_db, r_sess, r_db, &n_err) ||
			check_db_cdds(r_sess, r_db, l_sess, l_db, &n_err) ||
			check_paths(l_sess, l_db, r_sess, r_db, &n_err) ||
			check_paths(r_sess, r_db, l_sess, l_db, &n_err) ||
			check_regist_tables(l_sess, l_db, r_sess, r_db, &n_err)
			|| check_regist_tables(r_sess, r_db, l_sess, l_db,
			&n_err) || check_regist_columns(l_sess, l_db, r_sess,
			r_db, &n_err) || check_regist_columns(r_sess, r_db,
			l_sess, l_db, &n_err);
	}
	if (err)
		SIfprintf(report_fp, ERget(E_RM0090_No_further_check), r_dbno);
	else if (n_err == 0)
		SIfprintf(report_fp, ERx("%s\n"), ERget(F_RM00AD_No_problems));
	if (remote_open == TRUE && set_session(r_sess) == OK)
	{
/* # line 307 "ckdstcfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(r_sess);
    IIsqDisconnect();
  }
/* # line 308 "ckdstcfg.sc" */	/* host code */
		remote_open = FALSE;
	}
}
/*{
** Name:	open_remote_db - open a remote database
**
** Description:
**	Open a remote database for configuration checking.
**
** Inputs:
**	r_db	- remote database name
**	r_sess	- remote database session number
**
** Outputs:
**	none
**
** Returns:
**	0 - OK
**	else - Error
*/
static STATUS
open_remote_db(
char	*r_db,
i4	r_sess)
{
  char *db = r_db;
  i4 sess = r_sess;
  char username[DB_MAXNAME+1];
  char dba[DB_MAXNAME+1];
  i4 cnt;
	DBEC_ERR_INFO	errinfo;
	STATUS		err;
/* # line 345 "ckdstcfg.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(sess);
    IIsqConnect(0,db,(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0);
  }
/* # line 346 "ckdstcfg.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		return (rpt_error_display(E_RM0084_Error_connecting, TRUE,
			errinfo.errorno, 1, db));
	/* See if user is the DBA; if not, reconnect as DBA. */
/* # line 351 "ckdstcfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select DBMSINFO('DBA'), DBMSINFO('USERNAME')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,dba);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,username);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 353 "ckdstcfg.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
	{
		err = rpt_error_display(E_RM0091_Err_rtrv_dba, TRUE,
			errinfo.errorno, 1, db);
/* # line 357 "ckdstcfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(sess);
    IIsqDisconnect();
  }
/* # line 358 "ckdstcfg.sc" */	/* host code */
		return (err);
	}
	if (STcompare(dba, username))
	{
/* # line 363 "ckdstcfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(sess);
    IIsqDisconnect();
  }
/* # line 364 "ckdstcfg.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(sess);
    IIsqUser(dba);
    IIsqConnect(0,db,(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0);
  }
/* # line 365 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
			return (rpt_error_display(E_RM0084_Error_connecting,
				TRUE, errinfo.errorno, 1, db));
	}
/* # line 370 "ckdstcfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iitables where LOWERCASE(table_name)='dd_database\
s' and table_owner=DBMSINFO('dba')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 374 "ckdstcfg.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
	{
		err = rpt_error_display(E_RM0092_Err_ck_catalogs, TRUE,
			errinfo.errorno, 1, db);
/* # line 378 "ckdstcfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(sess);
    IIsqDisconnect();
  }
/* # line 379 "ckdstcfg.sc" */	/* host code */
		return (err);
	}
	else if (cnt == 0)
	{
		err = rpt_error_display(E_RM0093_Miss_rep_catalogs, TRUE,
			errinfo.errorno, 1, db);
/* # line 385 "ckdstcfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(sess);
    IIsqDisconnect();
  }
/* # line 386 "ckdstcfg.sc" */	/* host code */
		return (err);
	}
	return (OK);
}
/*{
** Name:	check_databases - check databases
**
** Description:
**	Compare the dd_databases tables in sess_a vs. sess_b.
**
** Inputs:
**	sess_a	- Ingres session number
**	sess_b	- Ingres session number
**
** Outputs:
**	n_err	- Error count
**
** Returns:
**	0 - OK
**	else - Error
*/
static STATUS
check_databases(
i4	sess_a,
char	*db_a,
i4	sess_b,
char	*db_b,
i4	*n_err)
{
  i4 session_a = sess_a;
  i4 session_b = sess_b;
  i2 database_no;
  char database_name[DB_MAXNAME+1];
  char database_name_b[DB_MAXNAME+1];
  char vnode_name[DB_MAXNAME+1];
  char vnode_name_b[DB_MAXNAME+1];
  char dbms_type[9];
  char dbms_type_b[9];
  i4 done = 0;
  STATUS err = 0;
	DBEC_ERR_INFO	errinfo;
	bool		c2_open = FALSE;
	i4		i;
	i4		db_no;
	if ((err = set_session(session_a)) != OK)
		++*n_err;
	if (!err)
	{
/* # line 445 "ckdstcfg.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"c2",7210,20985);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select database_no, database_name, vnode_name, dbms_type from dd_data\
bases order by database_no");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"c2",7210,20985);
  }
/* # line 446 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM0094_Err_open_db_curs, TRUE,
				errinfo.errorno, 1, db_a);
			++*n_err;
		}
		else
		{
			c2_open = TRUE;
		}
	}
	while (!err && !done)
	{
		if ((err = set_session(session_a)) != OK)
		{
			++*n_err;
			continue;
		}
/* # line 465 "ckdstcfg.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"c2",7210,20985,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,2,&database_no);
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,database_name);
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,vnode_name);
      IIcsGetio((short *)0,1,32,8,dbms_type);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 468 "ckdstcfg.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 469 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM0095_Err_fetch_db_curs,
				TRUE, errinfo.errorno, 1, db_a);
			++*n_err;
			continue;
		}
		else if (done)
		{
			continue;
		}
		db_no = (i4)database_no;
		STtrmwhite(vnode_name);
		STtrmwhite(database_name);
		STtrmwhite(dbms_type);
		if ((err = set_session(session_b)) != OK)
		{
			++*n_err;
			continue;
		}
/* # line 490 "ckdstcfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select database_name, vnode_name, dbms_type from dd_databases where d\
atabase_no=");
    IIputdomio((short *)0,1,30,2,&database_no);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,database_name_b);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,vnode_name_b);
      IIgetdomio((short *)0,1,32,8,dbms_type_b);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 494 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM0096_Err_rtrv_db_info, TRUE,
				errinfo.errorno, 2, &db_no, db_b);
			++*n_err;
			continue;
		}
		else if (errinfo.rowcount == 0)
		{
			SIfprintf(report_fp, ERx("%s\n"),
				ERget(E_RM0097_Dd_databases_err));
			rpt_error_display(E_RM0098_Db_not_found, FALSE, 0, 3,
				&db_no, database_name, db_b);
			++*n_err;
			continue;
		}
		STtrmwhite(vnode_name_b);
		STtrmwhite(database_name_b);
		STtrmwhite(dbms_type_b);
		if (session_a == Local_Session)
		{
			if (STcompare(database_name_b, database_name))
			{
				SIfprintf(report_fp, ERx("%s\n"),
					ERget(E_RM0097_Dd_databases_err));
				rpt_error_display(E_RM0099_Db_name_diff, FALSE,
					0, 5, &db_no, database_name, db_a,
					database_name_b, db_b);
				++*n_err;
			}
			if (STcompare(vnode_name_b, vnode_name))
			{
				SIfprintf(report_fp, ERx("%s\n"),
					ERget(E_RM0097_Dd_databases_err));
				rpt_error_display(E_RM009A_Vnode_name_diff,
					FALSE, 0, 5, &db_no, vnode_name, db_a,
					vnode_name_b, db_b);
				++*n_err;
			}
			if (STcompare(dbms_type_b, dbms_type))
			{
				SIfprintf(report_fp, ERx("%s\n"),
					ERget(E_RM0097_Dd_databases_err));
				rpt_error_display(E_RM009B_Dbms_type_diff,
					FALSE, 0, 5, &db_no, dbms_type, db_a,
					dbms_type_b, db_b);
				++*n_err;
			}
		}
	}
	i = set_session(session_a);
	if (i != OK)
	{
		++*n_err;
		if (!err)
			err = i;
	}
	else if (c2_open)
	{
/* # line 556 "ckdstcfg.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c2",7210,20985);
  }
/* # line 557 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			rpt_error_display(E_RM009C_Err_close_db_curs,
				TRUE, errinfo.errorno, 1, db_a);
			if (!err)
				err = E_RM009C_Err_close_db_curs;
			++*n_err;
		}
	}
	return (err);
}
/*{
** Name:	check_regist_tables - check registered tables
**
** Description:
**	Compare the dd_regist_tables tables in sess_a vs. sess_b.  Also checks
**	that columns have been registered & support tables created.
**
** Inputs:
**	sess_a	- Ingres session number
**	sess_b	- Ingres session number
**
** Outputs:
**	n_err	- Error count
**
** Returns:
**	0 - OK
**	else - Error
*/
static STATUS
check_regist_tables(
i4	sess_a,
char	*db_a,
i4	sess_b,
char	*db_b,
i4	*n_err)
{
  i4 session_a = sess_a;
  i4 session_b = sess_b;
  i4 table_no;
  char table_name[DB_MAXNAME+1];
  char table_name_b[DB_MAXNAME+1];
  char table_owner[DB_MAXNAME+1];
  char table_owner_b[DB_MAXNAME+1];
  char registered[26];
  char created[26];
  i2 cdds_no;
  i2 cdds_no_b;
  i4 done = 0;
  STATUS err = 0;
	DBEC_ERR_INFO	errinfo;
	bool		c3_open = FALSE;
	i4		i;
	if ((err = set_session(session_a)) != OK)
		++*n_err;
	if (!err)
	{
/* # line 624 "ckdstcfg.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"c3",7211,31225);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select table_no, table_name, table_owner, cdds_no from dd_regist_tabl\
es order by table_no");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"c3",7211,31225);
  }
/* # line 625 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM009D_Err_open_tbl_curs,
				TRUE, errinfo.errorno, 1, db_a);
			++*n_err;
		}
		else
		{
			c3_open = TRUE;
		}
	}
	while (!err && !done)
	{
		if ((err = set_session(session_a)) != OK)
		{
			++*n_err;
			continue;
		}
/* # line 644 "ckdstcfg.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"c3",7211,31225,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&table_no);
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,table_name);
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,table_owner);
      IIcsGetio((short *)0,1,30,2,&cdds_no);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 646 "ckdstcfg.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 647 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM009E_Err_fetch_tbl_curs,
				TRUE, errinfo.errorno, 1, db_a);
			++*n_err;
			continue;
		}
		else if (done)
		{
			continue;
		}
		STtrmwhite(table_name);
		STtrmwhite(table_owner);
		if ((err = set_session(session_b)) != OK)
		{
			++*n_err;
			continue;
		}
/* # line 666 "ckdstcfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name, table_owner, cdds_no, columns_registered, supp_obj\
s_created from dd_regist_tables where table_no=");
    IIputdomio((short *)0,1,30,4,&table_no);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,table_name_b);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,table_owner_b);
      IIgetdomio((short *)0,1,30,2,&cdds_no_b);
      IIgetdomio((short *)0,1,32,25,registered);
      IIgetdomio((short *)0,1,32,25,created);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 672 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM009F_Err_rtrv_tbl_info,
				TRUE, errinfo.errorno, 2, &table_no, db_b);
			++*n_err;
			continue;
		}
		else if (errinfo.rowcount == 0)
		{
			SIfprintf(report_fp, ERx("%s\n"),
				ERget(E_RM00A0_Dd_regist_tbls_err));
			rpt_error_display(E_RM00A1_Tbl_not_found, FALSE, 0, 3,
				&table_no, table_name, db_b);
			++*n_err;
			continue;
		}
		STtrmwhite(table_name_b);
		STtrmwhite(table_owner_b);
		STtrmwhite(created);
		STtrmwhite(registered);
		if (*created == EOS)
		{
			SIfprintf(report_fp, ERx("%s\n"),
				ERget(E_RM00A0_Dd_regist_tbls_err));
			rpt_error_display(E_RM00A2_Supp_not_created, FALSE, 0,
				4, &table_no, table_name_b, table_owner_b,
				db_b);
			++*n_err;
		}
		if (*registered == EOS)
		{
			SIfprintf(report_fp, ERx("%s\n"),
				ERget(E_RM00A0_Dd_regist_tbls_err));
			rpt_error_display(E_RM00A3_Cols_not_regist, FALSE, 0, 4,
				&table_no, table_name_b, table_owner_b, db_b);
			++*n_err;
		}
		if (session_a == Local_Session)
		{
			if (STcompare(table_name_b, table_name))
			{
				SIfprintf(report_fp, ERx("%s\n"),
					ERget(E_RM00A0_Dd_regist_tbls_err));
				rpt_error_display(E_RM00A4_Tbl_name_diff, FALSE,
					0, 5, &table_no, table_name, db_a,
					table_name_b, db_b);
				++*n_err;
			}
			if (STcompare(table_owner_b, table_owner))
			{
				SIfprintf(report_fp, ERx("%s\n"),
					ERget(E_RM00A0_Dd_regist_tbls_err));
				rpt_error_display(E_RM00A5_Tbl_owner_diff,
					FALSE, 0, 5, &table_no, table_owner,
					db_a, table_owner_b, db_b);
				++*n_err;
			}
			if (cdds_no_b != cdds_no)
			{
				i4	cdds_num = (i4)cdds_no;
				i4	cdds_num_b = (i4)cdds_no_b;
				SIfprintf(report_fp, ERx("%s\n"),
					ERget(E_RM00A0_Dd_regist_tbls_err));
				rpt_error_display(E_RM00A6_Tbl_cdds_diff, FALSE,
					0, 5, &table_no, &cdds_num, db_a,
					&cdds_num_b, db_b);
				++*n_err;
			}
		}
	}
	i = set_session(session_a);
	if (i != OK)
	{
		++*n_err;
		if (!err)
			err = i;
	}
	else if (c3_open)
	{
/* # line 755 "ckdstcfg.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c3",7211,31225);
  }
/* # line 756 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			rpt_error_display(E_RM00A7_Err_close_tbl_curs,
				TRUE, errinfo.errorno, 1, db_a);
			if (!err)
				err = E_RM00A7_Err_close_tbl_curs;
			++*n_err;
		}
	}
	return (err);
}
/*{
** Name:	check_regist_columns - check registered columns
**
** Description:
**	Compare the dd_regist_columns table in sess_a vs. sessb.
**
** Inputs:
**	sess_a	- Ingres session number
**	sess_b	- Ingres session number
**
** Outputs:
**	n_err	- Error count
**
** Returns:
**	0 - OK
**	else - Error
*/
static STATUS
check_regist_columns(
i4	sess_a,
char	*db_a,
i4	sess_b,
char	*db_b,
i4	*n_err)
{
  i4 session_a = sess_a;
  i4 session_b = sess_b;
  i4 table_no;
  char table_name[DB_MAXNAME+1];
  char column_name[DB_MAXNAME+1];
  i4 key_sequence;
  i4 key_sequence_b;
  i4 column_sequence;
  i4 column_sequence_b;
  i4 done = 0;
  STATUS err = 0;
	DBEC_ERR_INFO	errinfo;
	bool		c4_open = FALSE;
	i4		i;
	if ((err = set_session(session_a)) != OK)
		++*n_err;
	if (!err)
	{
/* # line 822 "ckdstcfg.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"c4",7212,3317);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select c.table_no, t.table_name, c.column_name, c.key_sequence, c.col\
umn_sequence from dd_regist_columns c, dd_regist_tables t where c.tabl\
e_no=t.table_no order by table_no, column_name");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"c4",7212,3317);
  }
/* # line 823 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM00A8_Err_open_col_curs,
				TRUE, errinfo.errorno, 1, db_a);
			++*n_err;
		}
		else
		{
			c4_open = TRUE;
		}
	}
	while (!err && !done)
	{
		if ((err = set_session(session_a)) != OK)
		{
			++*n_err;
			continue;
		}
/* # line 842 "ckdstcfg.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"c4",7212,3317,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&table_no);
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,table_name);
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,column_name);
      IIcsGetio((short *)0,1,30,4,&key_sequence);
      IIcsGetio((short *)0,1,30,4,&column_sequence);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 845 "ckdstcfg.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 846 "ckdstcfg.sc" */	/* host code */
		STtrmwhite(column_name);
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM00A9_Err_fetch_col_curs,
				TRUE, errinfo.errorno, 1, db_a);
			++*n_err;
			continue;
		}
		else if (done)
		{
			continue;
		}
		if ((err = set_session(session_b)) != OK)
		{
			++*n_err;
			continue;
		}
/* # line 864 "ckdstcfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select key_sequence, column_sequence from dd_regist_columns where tab\
le_no=");
    IIputdomio((short *)0,1,30,4,&table_no);
    IIwritio(0,(short *)0,1,32,0,(char *)"and column_name=");
    IIputdomio((short *)0,1,32,0,column_name);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&key_sequence_b);
      IIgetdomio((short *)0,1,30,4,&column_sequence_b);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 869 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM00AA_Err_rtrv_col_info,
				TRUE, errinfo.errorno, 3, &table_no,
				column_name, db_b);
			++*n_err;
			continue;
		}
		else if (errinfo.rowcount == 0)
		{
			SIfprintf(report_fp, ERx("%s\n"),
				ERget(E_RM00AB_Dd_regist_cols_err));
			rpt_error_display(E_RM00AC_Col_not_found, FALSE, 0, 3,
				&table_no, column_name, db_b);
			++*n_err;
			continue;
		}
		if (session_a == Local_Session)
		{
			if (key_sequence_b != key_sequence)
			{
				SIfprintf(report_fp, ERx("%s\n"),
					ERget(E_RM00AB_Dd_regist_cols_err));
				rpt_error_display(E_RM00AD_Col_key_seq_diff,
					FALSE, 0, 6, &table_no, column_name,
					&key_sequence, db_a, &key_sequence_b,
					db_b);
				++*n_err;
			}
			if (column_sequence_b != column_sequence)
			{
				SIfprintf(report_fp, ERx("%s\n"),
					ERget(E_RM00AB_Dd_regist_cols_err));
				rpt_error_display(E_RM00AE_Col_col_seq_diff,
					FALSE, 0, 6, &table_no, column_name,
					&column_sequence, db_a,
					&column_sequence_b, db_b);
				++*n_err;
			}
		}
	}
	i = set_session(session_a);
	if (i != OK)
	{
		++*n_err;
		if (!err)
			err = i;
	}
	else if (c4_open)
	{
/* # line 921 "ckdstcfg.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c4",7212,3317);
  }
/* # line 922 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			rpt_error_display(E_RM00AF_Err_close_col_curs,
				TRUE, errinfo.errorno, 1, db_a);
			if (!err)
				err = E_RM00AF_Err_close_col_curs;
			++*n_err;
		}
	}
	return (err);
}
/*{
** Name:	check_cdds - check cdds
**
** Description:
**	Compare the dd_cdds tables in sess_a vs. sess_b.
**
** Inputs:
**	sess_a	- Ingres session number
**	sess_b	- Ingres session number
**
** Outputs:
**	n_err	- Error count
**
** Returns:
**	0 - OK
**	else - Error
*/
static STATUS
check_cdds(
i4	sess_a,
char	*db_a,
i4	sess_b,
char	*db_b,
i4	*n_err)
{
  i4 session_a = sess_a;
  i4 session_b = sess_b;
  i2 cdds_no;
  char cdds_name[DB_MAXNAME+1];
  char cdds_name_b[DB_MAXNAME+1];
  i2 collision_mode;
  i2 collision_mode_b;
  i2 error_mode;
  i2 error_mode_b;
  i4 done = 0;
  STATUS err = 0;
	DBEC_ERR_INFO	errinfo;
	bool		c5_open = FALSE;
	i4		i;
	i4		cdds_num;
	if ((err = set_session(session_a)) != OK)
		++*n_err;
	if (!err)
	{
/* # line 987 "ckdstcfg.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"c5",7213,16090);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select cdds_no, cdds_name, collision_mode, error_mode from dd_cdds or\
der by cdds_no");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"c5",7213,16090);
  }
/* # line 988 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM00B0_Err_open_cdds_curs,
				TRUE, errinfo.errorno, 1, db_a);
			++*n_err;
		}
		else
		{
			c5_open = TRUE;
		}
	}
	while (!err && !done)
	{
		if ((err = set_session(session_a)) != OK)
		{
			++*n_err;
			continue;
		}
/* # line 1007 "ckdstcfg.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"c5",7213,16090,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,2,&cdds_no);
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,cdds_name);
      IIcsGetio((short *)0,1,30,2,&collision_mode);
      IIcsGetio((short *)0,1,30,2,&error_mode);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 1010 "ckdstcfg.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 1011 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM00B1_Err_fetch_cdds_curs,
				TRUE, errinfo.errorno, 1, db_a);
			++*n_err;
			continue;
		}
		else if (done)
		{
			continue;
		}
		cdds_num = (i4)cdds_no;
		STtrmwhite(cdds_name);
		if ((err = set_session(session_b)) != OK)
		{
			++*n_err;
			continue;
		}
/* # line 1030 "ckdstcfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select cdds_name, collision_mode, error_mode from dd_cdds where cdds_\
no=");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,cdds_name_b);
      IIgetdomio((short *)0,1,30,2,&collision_mode_b);
      IIgetdomio((short *)0,1,30,2,&error_mode_b);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1034 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM00B2_Err_rtrv_cdds_info,
				TRUE, errinfo.errorno, 2, &cdds_num, db_b);
			++*n_err;
			continue;
		}
		else if (errinfo.rowcount == 0)
		{
			SIfprintf(report_fp, ERx("%s\n"),
				ERget(E_RM00B3_Dd_cdds_err));
			rpt_error_display(E_RM00B4_Cdds_not_found, FALSE, 0, 2,
				&cdds_num, db_b);
			++*n_err;
			continue;
		}
		STtrmwhite(cdds_name_b);
		if (session_a == Local_Session)
		{
			if (STcompare(cdds_name_b, cdds_name))
			{
				SIfprintf(report_fp, ERx("%s\n"),
					ERget(E_RM00B3_Dd_cdds_err));
				rpt_error_display(E_RM00B5_Cdds_name_diff,
					FALSE, 0, 5, &cdds_num, cdds_name, db_a,
					cdds_name_b, db_b);
				++*n_err;
			}
			if (collision_mode_b != collision_mode)
			{
				i4	coll_mode = (i4)collision_mode;
				i4	coll_mode_b = (i4)collision_mode_b;
				SIfprintf(report_fp, ERx("%s\n"),
					ERget(E_RM00B3_Dd_cdds_err));
				rpt_error_display(E_RM00B6_Cdds_coll_mode_diff,
					FALSE, 0, 5, &cdds_num, &coll_mode,
					db_a, &coll_mode_b, db_b);
				++*n_err;
			}
			if (error_mode_b != error_mode)
			{
				i4	err_mode = (i4)error_mode;
				i4	err_mode_b = (i4)error_mode_b;
				SIfprintf(report_fp, ERx("%s\n"),
					ERget(E_RM00B3_Dd_cdds_err));
				rpt_error_display(E_RM00B7_Cdds_err_mode_diff,
					FALSE, 0, 5, &cdds_num, &err_mode, db_a,
					&err_mode_b, db_b);
				++*n_err;
			}
		}
	}
	i = set_session(session_a);
	if (i != OK)
	{
		++*n_err;
		if (!err)
			err = i;
	}
	else if (c5_open)
	{
/* # line 1099 "ckdstcfg.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c5",7213,16090);
  }
/* # line 1100 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			rpt_error_display(E_RM00B8_Err_close_cdds_curs,
				TRUE, errinfo.errorno, 1, db_a);
			if (!err)
				err = E_RM00B8_Err_close_cdds_curs;
			++*n_err;
		}
	}
	return (err);
}
/*{
** Name:	check_db_cdds - check db_cdds
**
** Description:
**	Compare the dd_db_cdds tables in sess_a vs. sess_b.
**
** Inputs:
**	sess_a	- Ingres session number
**	sess_b	- Ingres session number
**
** Outputs:
**	n_err	- Error count
**
** Returns:
**	0 - OK
**	else - Error
*/
static STATUS
check_db_cdds(
i4	sess_a,
char	*db_a,
i4	sess_b,
char	*db_b,
i4	*n_err)
{
  i4 session_a = sess_a;
  i4 session_b = sess_b;
  i2 cdds_no;
  i2 database_no;
  i2 target_type;
  i2 target_type_b;
  i4 done = 0;
  STATUS err = 0;
	DBEC_ERR_INFO	errinfo;
	bool		c6_open = FALSE;
	i4		i;
	i4		db_num;
	i4		cdds_num;
	if ((err = set_session(session_a)) != OK)
		++*n_err;
	if (!err)
	{
/* # line 1163 "ckdstcfg.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"c6",7214,25554);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select cdds_no, database_no, target_type from dd_db_cdds order by cdd\
s_no, database_no");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"c6",7214,25554);
  }
/* # line 1164 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM00B9_Err_open_dbcdds_curs,
				TRUE, errinfo.errorno, 1, db_a);
			++*n_err;
		}
		else
		{
			c6_open = TRUE;
		}
	}
	while (!err && !done)
	{
		if ((err = set_session(session_a)) != OK)
		{
			++*n_err;
			continue;
		}
/* # line 1183 "ckdstcfg.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"c6",7214,25554,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,2,&cdds_no);
      IIcsGetio((short *)0,1,30,2,&database_no);
      IIcsGetio((short *)0,1,30,2,&target_type);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 1185 "ckdstcfg.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 1186 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM00BA_Err_fetch_dbcdds_curs,
				TRUE, errinfo.errorno, 1, db_a);
			++*n_err;
			continue;
		}
		else if (done)
		{
			continue;
		}
		db_num = (i4)database_no;
		cdds_num = (i4)cdds_no;
		if ((err = set_session(session_b)) != OK)
		{
			++*n_err;
			continue;
		}
/* # line 1205 "ckdstcfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select target_type from dd_db_cdds where cdds_no=");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIwritio(0,(short *)0,1,32,0,(char *)"and database_no=");
    IIputdomio((short *)0,1,30,2,&database_no);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&target_type_b);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1210 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM00BB_Err_rtrv_dbcdds_info,
				TRUE, errinfo.errorno, 3, &cdds_num, &db_num,
				db_b);
			++*n_err;
			continue;
		}
		else if (errinfo.rowcount == 0)
		{
			SIfprintf(report_fp, ERx("%s\n"),
				ERget(E_RM00BC_Dd_db_cdds_err));
			rpt_error_display(E_RM00BD_Dbcdds_not_found, FALSE, 0,
				3, &cdds_num, &db_num, db_b);
			++*n_err;
			continue;
		}
		if (session_a == Local_Session)
		{
			if (target_type_b != target_type)
			{
				i4	type = (i4)target_type;
				i4	type_b = (i4)target_type_b;
				SIfprintf(report_fp, ERx("%s\n"),
					ERget(E_RM00BC_Dd_db_cdds_err));
				rpt_error_display(E_RM00BE_Dbcdds_targtype_diff,
					FALSE, 0, 6, &db_num, &cdds_num, &type,
					db_a, &type_b, db_b);
				++*n_err;
			}
		}
	}
	i = set_session(session_a);
	if (i != OK)
	{
		++*n_err;
		if (!err)
			err = i;
	}
	else if (c6_open)
	{
/* # line 1254 "ckdstcfg.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c6",7214,25554);
  }
/* # line 1255 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			rpt_error_display(E_RM00BF_Err_close_dbcdds_curs,
				TRUE, errinfo.errorno, 1, db_a);
			if (!err)
				err = E_RM00BF_Err_close_dbcdds_curs;
			++*n_err;
		}
	}
	return (err);
}
/*{
** Name:	check_paths - check paths
**
** Description:
**	Compare the dd_paths tables in sess_a vs. sess_b.
**
** Inputs:
**	sess_a	- Ingres session number
**	sess_b	- Ingres session number
**
** Outputs:
**	n_err	- Error count
**
** Returns:
**	0 - OK
**	else - Error
*/
static STATUS
check_paths(
i4	sess_a,
char	*db_a,
i4	sess_b,
char	*db_b,
i4	*n_err)
{
  i4 session_a = sess_a;
  i4 session_b = sess_b;
  i2 cdds_no;
  i2 localdb;
  i2 sourcedb;
  i2 targetdb;
  i2 final_target;
  i2 final_target_b;
  i4 done = 0;
  STATUS err = 0;
	DBEC_ERR_INFO	errinfo;
	bool		c7_open = FALSE;
	i4		i;
	i4		cdds_num;
	i4		src_db;
	i4		local_db;
	i4		targ_db;
	if ((err = set_session(session_a)) != OK)
		++*n_err;
	if (!err)
	{
/* # line 1323 "ckdstcfg.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"c7",7215,31954);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select cdds_no, localdb, sourcedb, targetdb, final_target from dd_pat\
hs order by cdds_no, localdb, sourcedb, targetdb");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"c7",7215,31954);
  }
/* # line 1324 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM00C0_Err_open_path_curs,
				TRUE, errinfo.errorno, 1, db_a);
			++*n_err;
		}
		else
		{
			c7_open = TRUE;
		}
	}
	while (!err && !done)
	{
		if ((err = set_session(session_a)) != OK)
		{
			++*n_err;
			continue;
		}
/* # line 1343 "ckdstcfg.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"c7",7215,31954,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,2,&cdds_no);
      IIcsGetio((short *)0,1,30,2,&localdb);
      IIcsGetio((short *)0,1,30,2,&sourcedb);
      IIcsGetio((short *)0,1,30,2,&targetdb);
      IIcsGetio((short *)0,1,30,2,&final_target);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 1346 "ckdstcfg.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 1347 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM00C1_Err_fetch_path_curs,
				TRUE, errinfo.errorno, 1, db_a);
			++*n_err;
			continue;
		}
		else if (done)
		{
			continue;
		}
		cdds_num = (i4)cdds_no;
		src_db = (i4)sourcedb;
		local_db = (i4)localdb;
		targ_db = (i4)targetdb;
		if ((err = set_session(session_b)) != OK)
		{
			++*n_err;
			continue;
		}
/* # line 1368 "ckdstcfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select final_target from dd_paths where cdds_no=");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIwritio(0,(short *)0,1,32,0,(char *)"and localdb=");
    IIputdomio((short *)0,1,30,2,&localdb);
    IIwritio(0,(short *)0,1,32,0,(char *)"and sourcedb=");
    IIputdomio((short *)0,1,30,2,&sourcedb);
    IIwritio(0,(short *)0,1,32,0,(char *)"and targetdb=");
    IIputdomio((short *)0,1,30,2,&targetdb);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&final_target_b);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1375 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			err = rpt_error_display(E_RM00C2_Err_rtrv_path_info,
				TRUE, errinfo.errorno, 5, &cdds_num, &src_db,
				&local_db, &targ_db, db_b);
			++*n_err;
			continue;
		}
		else if (errinfo.rowcount == 0)
		{
			SIfprintf(report_fp, ERx("%s\n"),
				ERget(E_RM00C3_Dd_paths_err));
			rpt_error_display(E_RM00C4_Path_not_found, FALSE, 0,
				5, &cdds_num, &src_db, &local_db, &targ_db,
				db_b);
			++*n_err;
			continue;
		}
		if (session_a == Local_Session)
		{
			if (final_target_b != final_target)
			{
				i4	final = (i4)final_target;
				i4	final_b = (i4)final_target_b;
				SIfprintf(report_fp, ERx("%s\n"),
					ERget(E_RM00C3_Dd_paths_err));
				rpt_error_display(E_RM00C5_Path_target_diff,
					FALSE, 0, 8, &cdds_num, &src_db,
					&local_db, &targ_db, &final, db_a,
					&final_b, db_b);
				++*n_err;
			}
		}
	}
	i = set_session(session_a);
	if (i != OK)
	{
		++*n_err;
		if (!err)
			err = i;
	}
	else if (c7_open)
	{
/* # line 1421 "ckdstcfg.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c7",7215,31954);
  }
/* # line 1422 "ckdstcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			rpt_error_display(E_RM00C6_Err_close_path_curs, TRUE,
				errinfo.errorno, 1, db_a);
			if (!err)
				err = E_RM00C6_Err_close_path_curs;
			++*n_err;
		}
	}
	return (err);
}
/*{
** Name:	set_session - set session
**
** Description:
**	Set the current session.
**
** Inputs:
**	sess_no - Session number to set to
**
** Outputs:
**	none
**
** Returns:
**	OK
**	else - Error
*/
static STATUS
set_session(
i4	sess_no)
{
  i4 session_no = sess_no;
  i4 cur_sess;
/* # line 1461 "ckdstcfg.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&cur_sess,11);
  }
/* # line 1462 "ckdstcfg.sc" */	/* host code */
	if (cur_sess == session_no)
		return (OK);
/* # line 1465 "ckdstcfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&session_no);
  }
/* # line 1466 "ckdstcfg.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&cur_sess,11);
  }
/* # line 1467 "ckdstcfg.sc" */	/* host code */
	if (cur_sess == session_no)
		return (OK);
	rpt_error_display(E_RM00C7_Err_set_session, TRUE, 0, 1, &session_no);
	return (E_RM00C7_Err_set_session);
}
/*{
** Name:	rpt_error_display - display report error
**
** Description:
**	Write a report error to the report file and optionally display an error
**	message on the terminal.
**
** Inputs:
**	msg_id		- message number
**	display		- display error on terminal?
**	errorno		- SQL error number
**	parcount	- parameter count
**
** Outputs:
**	none
**
** Returns:
**	msg_id
*/
static STATUS
rpt_error_display(
ER_MSGID	msg_id,
bool		display,
i4		errorno,
i4		parcount,
...)
{
  char errortext[257];
	char		msg_buf[ER_MAX_LEN];
	ER_ARGUMENT	params[10];
	i4		i;
	STATUS		status;
	DB_SQLSTATE	sqlstate;
	i4		len;
	CL_ERR_DESC	err_code;
	va_list		ap;
	va_start(ap, parcount);
	for (i = 0; i < parcount; ++i)
	{
		params[i].er_size = ER_PTR_ARGUMENT;
		params[i].er_value = va_arg(ap, PTR);
	}
	va_end(ap);
	status = ERslookup(msg_id, (CL_ERR_DESC *)NULL, ER_TEXTONLY,
		 sqlstate.db_sqlstate, msg_buf, sizeof(msg_buf),
		 iiuglcd_langcode(), &len, &err_code, parcount, params);
	SIfprintf(report_fp, ERx("%s\n"), msg_buf);
	if (errorno)
	{
/* # line 1527 "ckdstcfg.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,32,256,errortext,63);
  }
/* # line 1528 "ckdstcfg.sc" */	/* host code */
		SIfprintf(report_fp, ERx("%s\n"), errortext);
	}
	if (display)
	{
		IIUGerr(msg_id, UG_ERR_ERROR, parcount, params[0].er_value,
			params[1].er_value, params[2].er_value,
			params[3].er_value, params[4].er_value,
			params[5].er_value, params[6].er_value,
			params[7].er_value, params[8].er_value,
			params[9].er_value);
	}
	return ((STATUS)msg_id);
}
