# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <st.h>
# include <si.h>
# include <cm.h>
# include <nm.h>
# include <er.h>
# include <me.h>
# include <lo.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <ug.h>
# include <ug.h>
# include <rpu.h>
# include <tbldef.h>
# include "errm.h"
/**
** Name:	move1cfg.sc - Move configuration information
**
** Description:
**	Defines
**		move_1_config - move configuration to one target database
**
** History:
**	09-jan-97 (joea)
**		Created based on move1cfg.sc in replicator library.
**	28-jan-97 (joea)
**		Add MAXPAGES to modify for dd_last_number to prevent lock
**		escalation. Call trace point DM102 to prevent catalog problems
**		when destroying dd_reg_tbl_idx.
**	17-mar-97 (joea) bug 81128
**		Replace dd_regist_tables.table_owner with the target DBA.
**	02-jul-97 (joea)
**		Use DELETE instead of MODIFY TO TRUNCATED/reMODIFY to initialize**		the catalogs.
**      25-Sep-97 (fanra01)
**              Modified remove_temp function to reload the location structures
**              from the statically stored filenames.  The buffer held in the
**              location structure returned from NMloc is not persistent and
**              can be modified during calls to NMloc.
**              This caused the symptom where the files in the directory
**              defined by II_CONFIG were deleted.
**	14-oct-97 (joea) bug 83765
**		Remove unused argument to dropsupp and drop_supp_objs. After
**		loading dd_regist_tables, reset supp_objs_created. Conversely,
**		after calling tbl_dbprocs, update supp_objs_created.
**	18-may-98 (padni01) bug 89865
**		Use date_gmt instead of date to set supp_objs_created field 
**		of dd_regist_tables.
**	01-oct-98 (abbjo03)
**		Replace ddba_messageit with IIUGerr.
**	19-oct-98 (abbjo03) bug 93780
**		Connect to target database requesting exclusive lock.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      14-Dec-2005 (hanal04) Bug 115562 INGREP182
**          Ensure ERRORTYPE is set to genericerror after connecting to
**          avoid unexpected failures. This is consistent with other 
**          CONNECTs in front/rep/repmgr.
**/
# define DIM(a)		(sizeof(a) / sizeof(a[0]))
GLOBALREF TBLDEF	*RMtbl;
  static char filenames[7][MAX_LOC+1];
static struct cfgtbl
{
	char	*table_name;
	char	*prefix;
	LOCATION	loc;
} cfgtbls[] =
{
	{ ERx("dd_databases"),		ERx("db"), },
	{ ERx("dd_regist_tables"),	ERx("tb"), },
	{ ERx("dd_regist_columns"),	ERx("co"), },
	{ ERx("dd_cdds"),		ERx("cd"), },
	{ ERx("dd_db_cdds"),		ERx("dc"), },
	{ ERx("dd_paths"),		ERx("pa"), },
	{ ERx("dd_last_number"),	ERx("ls"), }
};
static void remove_temp(void);
static STATUS drop_supp_objs(void);
/*{
** Name:	move_1_config - Move configuration to one target database
**
** Description:
**	Copies the Replicator configuration from the local database to the
**	specified target.
**
** Inputs:
**	database_no	- the target database number (unused)
**	vnode_name	- the target nodename
**	dbname		- the target database
**
** Outputs:
**	none
**
** Returns:
**	OK				- success
**	E_RM0082_Error_rtrv_tables	- error retrieving registd table count
**	E_RM0120_Incomplete_cfg		- incomplete configuration
**	E_RM010C_Err_copy_out		- error copying catalog out
**	E_RM0084_Error_connecting	- error connecting to target database
**	E_RM0003_No_rep_catalogs	- catalogs don't exist
**	E_RM010D_Err_clear_cat		- error deleting catalogs
**	E_RM010E_Err_copy_in		- error copying catalog in
**	E_RM010F_Err_adj_owner		- error adjusting DBA table ownership
**	E_RM0110_Err_adj_localdb	- error adjusting local_db flag
**	E_RM0111_Err_rtrv_type		- error retrieving DBMS type
**	E_RM0112_Err_open_tbl_curs	- error opening dd_regist_tables cursor
**	E_RM0113_Err_fetch_tbl_curs	- error fetching from dd_regist_tables
**	Also may return errors from RMtbl_fetch, create_support_tables and
**	tbl_dbprocs.
*/
STATUS
move_1_config(
i2	database_no,
char	*vnode_name,
char	*dbname)
# if 0
  char *vnode_name;
  char *dbname;
# endif
{
	DBEC_ERR_INFO errinfo;
	i4	i;
	char	*p;
	STATUS	err;
  i4 old_session;
  i4 cnt;
  char full_dbname[DB_MAXNAME*2+3];
  i4 other_session = 103;
  char source_dba[DB_MAXNAME+1];
  char target_dba[DB_MAXNAME+1];
  char username[DB_MAXNAME+1];
  i4 table_no;
  char dbms_type[9];
/* # line 159 "move1cfg.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&old_session,11);
  }
/* # line 161 "move1cfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_regist_tables");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 163 "move1cfg.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo))
	{
		IIUGerr(E_RM0082_Error_rtrv_tables, UG_ERR_ERROR, 0);
		return (E_RM0082_Error_rtrv_tables);
	}
	if (cnt == 0)
	{
		IIUGerr(E_RM0120_Incomplete_cfg, UG_ERR_ERROR, 0);
		return (E_RM0120_Incomplete_cfg);
	}
	if (*vnode_name == EOS)
		STcopy(dbname, full_dbname);
	else
		STprintf(full_dbname, ERx("%s::%s"), vnode_name, dbname);
	SIprintf(ERget(F_RM00D4_Moving_cats), full_dbname);
	SIflush(stdout);
	/* For all tables, copy data to flat files */
	for (i = 0; i < DIM(cfgtbls); ++i)
	{
		/* Build filename */
		if (NMloc(TEMP, PATH, NULL, &cfgtbls[i].loc) != OK)
			return (FAIL);
		if (LOuniq(cfgtbls[i].prefix, ERx("dat"), &cfgtbls[i].loc)
				!= OK)
			return (FAIL);
		LOtos(&cfgtbls[i].loc, &p);
		STcopy(p, filenames[i]);
		switch (i)
		{
		case 0:
/* # line 196 "move1cfg.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"copy table dd_databases()int\
o ");
    IIvarstrio((short *)0,1,32,0,filenames[i]);
    IIsyncup((char *)0,0);
  }
/* # line 198 "move1cfg.sc" */	/* host code */
			break;
		case 1:
/* # line 201 "move1cfg.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"copy table dd_regist_tables()into ");
    IIvarstrio((short *)0,1,32,0,filenames[i]);
    IIsyncup((char *)0,0);
  }
/* # line 203 "move1cfg.sc" */	/* host code */
			break;
		case 2:
/* # line 206 "move1cfg.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"copy table dd_regist_columns()into ");
    IIvarstrio((short *)0,1,32,0,filenames[i]);
    IIsyncup((char *)0,0);
  }
/* # line 208 "move1cfg.sc" */	/* host code */
			break;
		case 3:
/* # line 211 "move1cfg.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"copy table dd_cdds()into ");
    IIvarstrio((short *)0,1,32,0,filenames[i]);
    IIsyncup((char *)0,0);
  }
/* # line 213 "move1cfg.sc" */	/* host code */
			break;
		case 4:
/* # line 215 "move1cfg.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"copy table dd_db_cdds()into ");
    IIvarstrio((short *)0,1,32,0,filenames[i]);
    IIsyncup((char *)0,0);
  }
/* # line 217 "move1cfg.sc" */	/* host code */
			break;
		case 5:
/* # line 220 "move1cfg.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"copy table dd_paths()into ");
    IIvarstrio((short *)0,1,32,0,filenames[i]);
    IIsyncup((char *)0,0);
  }
/* # line 222 "move1cfg.sc" */	/* host code */
			break;
		case 6:
/* # line 225 "move1cfg.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"copy table dd_last_number()i\
nto ");
    IIvarstrio((short *)0,1,32,0,filenames[i]);
    IIsyncup((char *)0,0);
  }
/* # line 227 "move1cfg.sc" */	/* host code */
		}
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo))
		{
			IIUGerr(E_RM010C_Err_copy_out, UG_ERR_ERROR, 1,
				cfgtbls[i].table_name);
			remove_temp();
			return (E_RM010C_Err_copy_out);
		}
	}
/* # line 237 "move1cfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select DBMSINFO('dba')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,source_dba);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 239 "move1cfg.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqConnect(0,full_dbname,(char *)"-l",(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 241 "move1cfg.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo))
	{
		IIUGerr(E_RM0084_Error_connecting, UG_ERR_ERROR, 1,
			full_dbname);
/* # line 245 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 246 "move1cfg.sc" */	/* host code */
		remove_temp();
		return (E_RM0084_Error_connecting);
	}
	/* See if user is the DBA; if not, reconnect as DBA. */
/* # line 251 "move1cfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select DBMSINFO('dba')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,target_dba);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 252 "move1cfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select DBMSINFO('username')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,username);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 253 "move1cfg.sc" */	/* host code */
	if (!STequal(target_dba, username))
	{
/* # line 255 "move1cfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 256 "move1cfg.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqUser(target_dba);
    IIsqConnect(0,full_dbname,(char *)"-l",(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 258 "move1cfg.sc" */	/* host code */
		if (RPdb_error_check(0, &errinfo))
		{
			IIUGerr(E_RM0084_Error_connecting, UG_ERR_ERROR, 1,
				full_dbname);
/* # line 262 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 263 "move1cfg.sc" */	/* host code */
			remove_temp();
			return (E_RM0084_Error_connecting);
		}
	}
	/* check that Replicator catalogs exist and are up to date */
	if (!RMcheck_cat_level())
	{
		IIUGerr(E_RM0003_No_rep_catalogs, UG_ERR_ERROR, 0);
/* # line 272 "move1cfg.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 273 "move1cfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 274 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 275 "move1cfg.sc" */	/* host code */
		remove_temp();
		return (E_RM0003_No_rep_catalogs);
	}
	/* Turn off replication */
/* # line 280 "move1cfg.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set TRACE POINT DM102");
    IIsyncup((char *)0,0);
  }
/* # line 282 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(3,(short *)0,1,32,0,(char *)"genericerror");
  }
/* # line 284 "move1cfg.sc" */	/* host code */
	/* drop old support objects */
	if ((err = drop_supp_objs()) != OK)
	{
/* # line 288 "move1cfg.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 289 "move1cfg.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set NOTRACE POINT DM102");
    IIsyncup((char *)0,0);
  }
/* # line 290 "move1cfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 291 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 292 "move1cfg.sc" */	/* host code */
		remove_temp();
		return (FAIL);
	}
	/* loop to move configuration data */
	for (i = 0; i < DIM(cfgtbls); ++i)
	{
		/* delete existing rows */
		switch (i)
		{
		case 0:
/* # line 303 "move1cfg.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from dd_databases");
    IIsyncup((char *)0,0);
  }
/* # line 304 "move1cfg.sc" */	/* host code */
			break;
		case 1:
/* # line 307 "move1cfg.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from dd_regist_tables");
    IIsyncup((char *)0,0);
  }
/* # line 308 "move1cfg.sc" */	/* host code */
			break;
		case 2:
/* # line 311 "move1cfg.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from dd_regist_column\
s");
    IIsyncup((char *)0,0);
  }
/* # line 312 "move1cfg.sc" */	/* host code */
			break;
		case 3:
/* # line 315 "move1cfg.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from dd_cdds");
    IIsyncup((char *)0,0);
  }
/* # line 316 "move1cfg.sc" */	/* host code */
			break;
		case 4:
/* # line 319 "move1cfg.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from dd_db_cdds");
    IIsyncup((char *)0,0);
  }
/* # line 320 "move1cfg.sc" */	/* host code */
			break;
		case 5:
/* # line 323 "move1cfg.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from dd_paths");
    IIsyncup((char *)0,0);
  }
/* # line 324 "move1cfg.sc" */	/* host code */
			break;
		case 6:
/* # line 327 "move1cfg.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from dd_last_number");
    IIsyncup((char *)0,0);
  }
/* # line 328 "move1cfg.sc" */	/* host code */
			break;
		}
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo))
		{
			IIUGerr(E_RM010D_Err_clear_cat, UG_ERR_ERROR, 1,
				cfgtbls[i].table_name);
/* # line 334 "move1cfg.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set NOTRACE POINT DM102");
    IIsyncup((char *)0,0);
  }
/* # line 335 "move1cfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 336 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 337 "move1cfg.sc" */	/* host code */
			remove_temp();
			return (E_RM010D_Err_clear_cat);
		}
		/* Copy data into cfg tables */
		switch (i)
		{
		case 0:
/* # line 345 "move1cfg.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"copy table dd_databases()fro\
m ");
    IIvarstrio((short *)0,1,32,0,filenames[i]);
    IIsyncup((char *)0,0);
  }
/* # line 347 "move1cfg.sc" */	/* host code */
			break;
		case 1:
/* # line 350 "move1cfg.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"copy table dd_regist_tables()from ");
    IIvarstrio((short *)0,1,32,0,filenames[i]);
    IIsyncup((char *)0,0);
  }
/* # line 352 "move1cfg.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_tables set supp_objs_created='', rules_created=''");
    IIsyncup((char *)0,0);
  }
/* # line 355 "move1cfg.sc" */	/* host code */
			break;
		case 2:
/* # line 358 "move1cfg.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"copy table dd_regist_columns()from ");
    IIvarstrio((short *)0,1,32,0,filenames[i]);
    IIsyncup((char *)0,0);
  }
/* # line 360 "move1cfg.sc" */	/* host code */
			break;
		case 3:
/* # line 363 "move1cfg.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"copy table dd_cdds()from ");
    IIvarstrio((short *)0,1,32,0,filenames[i]);
    IIsyncup((char *)0,0);
  }
/* # line 365 "move1cfg.sc" */	/* host code */
			break;
		case 4:
/* # line 368 "move1cfg.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"copy table dd_db_cdds()from ");
    IIvarstrio((short *)0,1,32,0,filenames[i]);
    IIsyncup((char *)0,0);
  }
/* # line 370 "move1cfg.sc" */	/* host code */
			break;
		case 5:
/* # line 373 "move1cfg.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"copy table dd_paths()from ");
    IIvarstrio((short *)0,1,32,0,filenames[i]);
    IIsyncup((char *)0,0);
  }
/* # line 375 "move1cfg.sc" */	/* host code */
			break;
		case 6:
/* # line 378 "move1cfg.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"copy table dd_last_number()f\
rom ");
    IIvarstrio((short *)0,1,32,0,filenames[i]);
    IIsyncup((char *)0,0);
  }
/* # line 380 "move1cfg.sc" */	/* host code */
			break;
		}
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo))
		{
			IIUGerr(E_RM010E_Err_copy_in, UG_ERR_ERROR, 1,
				cfgtbls[i].table_name);
/* # line 386 "move1cfg.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set NOTRACE POINT DM102");
    IIsyncup((char *)0,0);
  }
/* # line 387 "move1cfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 388 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 389 "move1cfg.sc" */	/* host code */
			remove_temp();
			return (E_RM010E_Err_copy_in);
		}
		/* Adjust ownership of DBA tables */
		if (i == 1)
		{
/* # line 396 "move1cfg.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_tables set table_owner=");
    IIputdomio((short *)0,1,32,0,target_dba);
    IIwritio(0,(short *)0,1,32,0,(char *)"where table_owner=");
    IIputdomio((short *)0,1,32,0,source_dba);
    IIsyncup((char *)0,0);
  }
/* # line 399 "move1cfg.sc" */	/* host code */
			if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo))
			{
				IIUGerr(E_RM010F_Err_adj_owner, UG_ERR_ERROR,
					0);
/* # line 403 "move1cfg.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set NOTRACE POINT DM102");
    IIsyncup((char *)0,0);
  }
/* # line 404 "move1cfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 405 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 406 "move1cfg.sc" */	/* host code */
				remove_temp();
				return (E_RM010F_Err_adj_owner);
			}
		}
	}
	/* Reset the localdb flag */
/* # line 413 "move1cfg.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_databases set local_db=0 where local_db=1");
    IIsyncup((char *)0,0);
  }
/* # line 416 "move1cfg.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo))
	{
		IIUGerr(E_RM0110_Err_adj_localdb, UG_ERR_ERROR, 0);
/* # line 419 "move1cfg.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set NOTRACE POINT DM102");
    IIsyncup((char *)0,0);
  }
/* # line 420 "move1cfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 421 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 422 "move1cfg.sc" */	/* host code */
		remove_temp();
		return (E_RM0110_Err_adj_localdb);
	}
/* # line 426 "move1cfg.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_databases set local_db=1 where vnode_name=");
    IIputdomio((short *)0,1,32,0,vnode_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and database_name=");
    IIputdomio((short *)0,1,32,0,dbname);
    IIsyncup((char *)0,0);
  }
/* # line 430 "move1cfg.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo))
	{
		IIUGerr(E_RM0110_Err_adj_localdb, UG_ERR_ERROR, 0);
/* # line 433 "move1cfg.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set NOTRACE POINT DM102");
    IIsyncup((char *)0,0);
  }
/* # line 434 "move1cfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 435 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 436 "move1cfg.sc" */	/* host code */
		remove_temp();
		return (E_RM0110_Err_adj_localdb);
	}
/* # line 439 "move1cfg.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 441 "move1cfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(dbms_type)from dd_databases where local_db=1");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,8,dbms_type);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 445 "move1cfg.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo))
	{
		IIUGerr(E_RM0111_Err_rtrv_type, UG_ERR_ERROR, 0);
/* # line 448 "move1cfg.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set NOTRACE POINT DM102");
    IIsyncup((char *)0,0);
  }
/* # line 449 "move1cfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 450 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 451 "move1cfg.sc" */	/* host code */
		remove_temp();
		return (E_RM0111_Err_rtrv_type);
	}
/* # line 454 "move1cfg.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 456 "move1cfg.sc" */	/* host code */
	/* if remote database is not Ingres, don't create support objects */
	if (!STequal(dbms_type, ERx("ingres")))
	{
/* # line 459 "move1cfg.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set NOTRACE POINT DM102");
    IIsyncup((char *)0,0);
  }
/* # line 460 "move1cfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 461 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 462 "move1cfg.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 463 "move1cfg.sc" */	/* host code */
		return (OK);
	}
	/* Retrieve list of tables from dd_regist_tables */
/* # line 467 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 473 "move1cfg.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"c1",7201,22769);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select t.table_no from dd_regist_tables t where t.columns_registered!\
=''");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"c1",7201,22769);
  }
/* # line 474 "move1cfg.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo))
	{
		IIUGerr(E_RM0112_Err_open_tbl_curs, UG_ERR_ERROR, 0);
/* # line 477 "move1cfg.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c1",7201,22769);
  }
/* # line 478 "move1cfg.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 479 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&other_session);
  }
/* # line 480 "move1cfg.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set NOTRACE POINT DM102");
    IIsyncup((char *)0,0);
  }
/* # line 481 "move1cfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 482 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 483 "move1cfg.sc" */	/* host code */
		return (E_RM0112_Err_open_tbl_curs);
	}
	while (TRUE)
	{
/* # line 488 "move1cfg.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"c1",7201,22769,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&table_no);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 489 "move1cfg.sc" */	/* host code */
		err = RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo);
		if (err)
		{
			IIUGerr(E_RM0113_Err_fetch_tbl_curs, UG_ERR_ERROR, 0);
/* # line 493 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&other_session);
  }
/* # line 494 "move1cfg.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set NOTRACE POINT DM102");
    IIsyncup((char *)0,0);
  }
/* # line 495 "move1cfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 496 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 497 "move1cfg.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c1",7201,22769);
  }
/* # line 498 "move1cfg.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 499 "move1cfg.sc" */	/* host code */
			return (E_RM0113_Err_fetch_tbl_curs);
		}
		if (errinfo.rowcount == 0)
			break;
/* # line 504 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&other_session);
  }
/* # line 505 "move1cfg.sc" */	/* host code */
		err = RMtbl_fetch(table_no, TRUE);
		if (err)
		{
/* # line 508 "move1cfg.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set NOTRACE POINT DM102");
    IIsyncup((char *)0,0);
  }
/* # line 509 "move1cfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 510 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 511 "move1cfg.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c1",7201,22769);
  }
/* # line 512 "move1cfg.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 513 "move1cfg.sc" */	/* host code */
			return (err);
		}
		if (RMtbl->target_type > 2)
		{
/* # line 518 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 519 "move1cfg.sc" */	/* host code */
			continue;
		}
		err = create_support_tables(table_no);
		if (err && err != -1)
		{
/* # line 525 "move1cfg.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set NOTRACE POINT DM102");
    IIsyncup((char *)0,0);
  }
/* # line 526 "move1cfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 527 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 528 "move1cfg.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c1",7201,22769);
  }
/* # line 529 "move1cfg.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 530 "move1cfg.sc" */	/* host code */
			return (err);
		}
		err = tbl_dbprocs(table_no);
		if (err && err != -1)
		{
/* # line 535 "move1cfg.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set NOTRACE POINT DM102");
    IIsyncup((char *)0,0);
  }
/* # line 536 "move1cfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 537 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 538 "move1cfg.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c1",7201,22769);
  }
/* # line 539 "move1cfg.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 540 "move1cfg.sc" */	/* host code */
			return (err);
		}
/* # line 543 "move1cfg.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_tables set supp_objs_created=DATE_GMT('now')where ta\
ble_no=");
    IIputdomio((short *)0,1,30,4,&table_no);
    IIsyncup((char *)0,0);
  }
/* # line 546 "move1cfg.sc" */	/* host code */
		if ((err = RPdb_error_check(DBEC_SINGLE_ROW, NULL)) != OK)
		{
/* # line 548 "move1cfg.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set NOTRACE POINT DM102");
    IIsyncup((char *)0,0);
  }
/* # line 549 "move1cfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 550 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 551 "move1cfg.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c1",7201,22769);
  }
/* # line 552 "move1cfg.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 553 "move1cfg.sc" */	/* host code */
			return (err);
		}
		/* Return to local session */
/* # line 557 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 558 "move1cfg.sc" */	/* host code */
	}
/* # line 560 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&other_session);
  }
/* # line 561 "move1cfg.sc" */	/* host code */
	/* Turn replication back on */
/* # line 562 "move1cfg.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 563 "move1cfg.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set NOTRACE POINT DM102");
    IIsyncup((char *)0,0);
  }
/* # line 564 "move1cfg.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 565 "move1cfg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 566 "move1cfg.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c1",7201,22769);
  }
/* # line 567 "move1cfg.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 568 "move1cfg.sc" */	/* host code */
	return (OK);
}
/*{
** Name:	remove_temp - remove temporary files
**
** Description:
**      Removes temporary files
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Returns:
**      none
**
** History:
**      Add a reload of locations structure from
*/
static void
remove_temp()
{
	i4	i;
	for (i = 0; i < DIM(cfgtbls); ++i)
        {
                LOfroms(PATH & FILENAME, filenames[i], &cfgtbls[i].loc);
		LOdelete(&cfgtbls[i].loc);
        }
}
/*{
** Name:	drop_supp_objs - drops support objects
**
** Description:
**	Drops support tables, procedures and rules.
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
drop_supp_objs()
{
	i4	*tbl_nums;
	STATUS	err;
	i4	i;
  i4 num_tables = 0;
  i4 table_no;
/* # line 628 "move1cfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_regist_tables");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&num_tables);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 631 "move1cfg.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
		return (FAIL);
	if (num_tables == 0)
		return (OK);
	/* allocate memory for registered table numbers */
	tbl_nums = (i4 *)MEreqmem(0, (u_i4)num_tables * sizeof(i4), TRUE,
		NULL);
	if (tbl_nums == NULL)
		return (FAIL);
	/* get registered table numbers */
	i = 0;
/* # line 645 "move1cfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select table_no from dd_regist_tables");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&table_no);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 649 "move1cfg.sc" */	/* host code */
		if (i < num_tables)
			tbl_nums[i] = table_no;
		++i;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if (RPdb_error_check(0, NULL) != OK)
		return (FAIL);
/* # line 655 "move1cfg.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 657 "move1cfg.sc" */	/* host code */
	for (i = 0; i < num_tables; ++i)
	{
		if ((err = dropsupp(tbl_nums[i])) != OK)
			return (err);
/* # line 662 "move1cfg.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 663 "move1cfg.sc" */	/* host code */
	}
	MEfree((PTR)tbl_nums);
	return (OK);
}
