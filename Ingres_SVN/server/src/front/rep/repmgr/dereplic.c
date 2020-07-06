# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <st.h>
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
# include <generr.h>
# include <rpu.h>
# include <tblobjs.h>
# include "errm.h"
/**
** Name:	dereplic.sc - remove Replicator objects
**
** Description:
**	Defines
**		main		- remove Replicator objects
**		drop_supp_objs	- drops support objects
**		drop_dict	- drops data dictionary
**		drop_object	- drops a database object
**
** History:
**	16-dec-96 (joea)
**		Created based on dereplic.sc in replicator library.
**	27-mar-98 (joea)
**		Discontinue use of RPing_ver_ok.
**	19-oct-98 (abbjo03)
**		Remove iidbcapabilities row in order not to confuse repmgr after
**		dereplic has run. Check catalog level at the start.
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
PROGRAM =	dereplic
NEEDLIBS =	REPMGRLIB REPCOMNLIB SHFRAMELIB SHQLIB \
		SHCOMPATLIB SHEMBEDLIB
UNDEFS =	II_copyright
*/
# define DIM(a)		(sizeof(a) / sizeof(a[0]))
static i4	proc_codes[] =
{
	TBLOBJ_REM_INS_PROC,
	TBLOBJ_REM_UPD_PROC,
	TBLOBJ_REM_DEL_PROC
};
static i4	supptbl_codes[] =
{
	TBLOBJ_ARC_TBL,
	TBLOBJ_SHA_TBL
};
  static char *db_name;
static	char	*user_name;
static	ARG_DESC args[] =
{
	/* required */
	{ERx("database"), DB_CHR_TYPE, FARG_PROMPT, (PTR)&db_name},
	/* optional */
	{ERx("user"), DB_CHR_TYPE, FARG_FAIL, (PTR)&user_name},
	NULL
};
static STATUS drop_supp_objs(void);
static STATUS drop_dict(void);
static STATUS drop_object(char *obj_type, char *obj_name);
FUNC_EXTERN STATUS drop_event(void);
FUNC_EXTERN bool RMcheck_cat_level(void);
FUNC_EXTERN STATUS FEhandler(EX_ARGS *ex);
/*{
** Name:	main - remove Replicator objects
**
** Description:
**	Drops the Replicator data dictionary tables, procedures,
**	and dbevents.
**
** Inputs:
**	argc - the number of arguments on the command line
**	argv - the command line arguments
**		argv[1] - the database to connect to
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
	EX_CONTEXT	context;
	/* Tell EX this is an INGRES tool */
	(void)EXsetclient(EX_INGRES_TOOL);
	/* Use the INGRES memory allocator */
	(void)MEadvise(ME_INGRES_ALLOC);
	/* Initialize character set attribute table */
	if (IIUGinit() != OK)
		PCexit(FAIL);
	/* Parse the command line arguments */
	if (IIUGuapParse(argc, argv, ERx("dereplic"), args) != OK)
		PCexit(FAIL);
	/* Set up the generic exception handler */
	if (EXdeclare(FEhandler, &context) != OK)
	{
		EXdelete();
		PCexit(FAIL);
	}
	/* Open the database */
	if (FEningres(NULL, (i4)0, db_name, user_name, NULL) != OK)
	{
		EXdelete();
		PCexit(FAIL);
	}
/* # line 154 "dereplic.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set AUTOCOMMIT OFF");
    IIsyncup((char *)0,0);
  }
/* # line 155 "dereplic.sc" */	/* set_sql */
  {
    IILQssSetSqlio(3,(short *)0,1,32,0,(char *)"genericerror");
  }
/* # line 157 "dereplic.sc" */	/* host code */
	if (!IIuiIsDBA)
	{
		EXdelete();
		IIUGerr(E_RM0002_Must_be_DBA, UG_ERR_FATAL, 0);
	}
	if (!RMcheck_cat_level())
	{
		EXdelete();
		IIUGerr(E_RM0093_Miss_rep_catalogs, UG_ERR_FATAL, 1, db_name);
	}
	IIUGmsg(ERget(S_RM0075_Dereplic_db), FALSE, 1, db_name);
	if (drop_supp_objs() != OK || drop_dict() != OK)
	{
/* # line 172 "dereplic.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 173 "dereplic.sc" */	/* host code */
		FEing_exit();
		IIUGerr(E_RM0076_Error_dereplic_db, UG_ERR_ERROR, 1, db_name);
		EXdelete();
		PCexit(FAIL);
	}
/* # line 180 "dereplic.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 181 "dereplic.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session authorization '$ingres'");
    IIsyncup((char *)0,0);
  }
/* # line 183 "dereplic.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbcapabilities where cap_capability='REP_CATALOG_LEVEL'");
    IIsyncup((char *)0,0);
  }
/* # line 184 "dereplic.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 185 "dereplic.sc" */	/* host code */
	FEing_exit();
	IIUGmsg(ERget(S_RM0077_Dereplic_success), FALSE, 1, db_name);
	EXdelete();
	PCexit(OK);
}
/*{
** Name:	drop_supp_objs - drops support objects
**
** Description:
**	Drops support tables and procedures.
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
	DBEC_ERR_INFO	errinfo;
	i4	*tbl_nums = NULL;
  i4 num_tables = 0;
  i4 tbl_no;
  char tbl_name[DB_MAXNAME+1];
  char tbl_owner[DB_MAXNAME+1];
  char obj_name[DB_MAXNAME+1];
	i4	i, j;
/* # line 222 "dereplic.sc" */	/* select */
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
/* # line 225 "dereplic.sc" */	/* host code */
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
/* # line 239 "dereplic.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select table_no from dd_regist_tables");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tbl_no);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 243 "dereplic.sc" */	/* host code */
		if (i < num_tables)
			tbl_nums[i] = tbl_no;
		++i;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if (RPdb_error_check(0, NULL) != OK)
		return (FAIL);
	for (i = 0; i < num_tables; ++i)
	{
		tbl_no = tbl_nums[i];
/* # line 253 "dereplic.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name, table_owner from dd_regist_tables where table_no=");
    IIputdomio((short *)0,1,30,4,&tbl_no);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,tbl_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,tbl_owner);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 257 "dereplic.sc" */	/* host code */
		if (RPdb_error_check(0, NULL) != OK)
			return (FAIL);
		STtrmwhite(tbl_name);
		STtrmwhite(tbl_owner);
		/* drop procedures */
		for (j = 0; j < DIM(proc_codes); ++j)
		{
			RPtblobj_name(tbl_name, tbl_no, proc_codes[j],
				obj_name);
			if (drop_object(ERx("PROCEDURE"), obj_name) != OK)
				return (FAIL);
		}
		/* drop support tables */
		for (j = 0; j < DIM(supptbl_codes); ++j)
		{
			RPtblobj_name(tbl_name, tbl_no, supptbl_codes[j],
				obj_name);
			if (drop_object(ERx("TABLE"), obj_name) != OK)
				return (FAIL);
/* # line 279 "dereplic.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from dd_support_tables where LOWERCASE(table_name)=");
    IIputdomio((short *)0,1,32,0,obj_name);
    IIsyncup((char *)0,0);
  }
/* # line 280 "dereplic.sc" */	/* host code */
			if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
				return (FAIL);
		}
/* # line 284 "dereplic.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 285 "dereplic.sc" */	/* host code */
	}
	/* free memory */
	MEfree((PTR)tbl_nums);
	return (OK);
}
/*{
** Name:	drop_dict - drops data dictionary
**
** Description:
**	Drops the Replicator data dictionary tables.
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
drop_dict()
{
	DBEC_ERR_INFO	errinfo;
	char	(*tbl_names)[DB_MAXNAME+1] = NULL;
  i4 num_tables = 0;
  i4 tbl_no;
  char tbl_name[DB_MAXNAME+1];
  char obj_name[DB_MAXNAME+1];
	i4	i;
/* # line 322 "dereplic.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_support_tables where LOWERCASE(table_name)!='d\
d_support_tables'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&num_tables);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 326 "dereplic.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
		return (FAIL);
	if (num_tables == 0)
		return (FAIL);
	/* allocate memory for table names */
	tbl_names = (char (*)[])MEreqmem(0,
		(u_i4)num_tables * sizeof(*tbl_names), TRUE, NULL);
	if (tbl_names == NULL)
		return (FAIL);
	/* get table names */
	i = 0;
/* # line 340 "dereplic.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(table_name)from dd_support_tables where LOWERCASE(table_n\
ame)!='dd_support_tables'");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,tbl_name);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 345 "dereplic.sc" */	/* host code */
		if (i < num_tables)
			STcopy(tbl_name, tbl_names[i]);
		++i;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
	if (RPdb_error_check(0, NULL) != OK)
		return (FAIL);
/* # line 351 "dereplic.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 353 "dereplic.sc" */	/* host code */
	/* drop data dictionary */
	for (i = 0; i < num_tables; ++i)
		if (drop_object(ERx("TABLE"), tbl_names[i]) != OK)
			return (FAIL);
	if (drop_object(ERx("TABLE"), ERx("dd_support_tables")) != OK)
		return (FAIL);
/* # line 361 "dereplic.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 363 "dereplic.sc" */	/* host code */
	/* free memory */
	MEfree((PTR)tbl_names);
	return (drop_events());
}
/*{
** Name:	drop_object - drops a database object
**
** Description:
**	Drops a database object.
**
** Inputs:
**	obj_type	- type of the object
**	obj_name	- name of the object
**
** Outputs:
**	none
**
** Returns:
**	OK or FAIL
*/
static STATUS
drop_object(
char	*obj_type,
char	*obj_name)
{
	DBEC_ERR_INFO	errinfo;
  char stmt[128];
	STprintf(stmt, ERx("DROP %s %s"), obj_type, obj_name);
/* # line 397 "dereplic.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
  }
/* # line 398 "dereplic.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo) != OK)
	{
		/* rollback on all errors except object not found */
		if (errinfo.errorno != GE_TABLE_NOT_FOUND &&
				errinfo.errorno != GE_NOT_FOUND)
			return (FAIL);
	}
	return (OK);
}
