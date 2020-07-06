# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <cm.h>
# include <st.h>
# include <tm.h>
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
# include "errm.h"
# include <si.h>
/**
**  Name:	arcclean.sc - archive clean
**
**  Description:
**	Defines
**		main		- archive clean main routine
**		sqlerr_check	- SQL error check
**		clean_tables	- clean the shadow and archive tables
**		delete_rows	- delete the old rows
**		modify_table	- remodify a table
**		build_shadow_stmt - build statement for shadow table
**		error_exit	- exit on error
**
** FIXME:
**	1. needs lock timeout and deadlock handling
**	2. Make use of is_compressed, key_is_compressed and duplicate_rows
**	   columns.
**	3. The modify_table doesn't work for tables owned by a non-DBA.
**
** History:
**	16-dec-96 (joea)
**		Created based on arcclean.sc in replicator library.
**      15-jan-97 (hanch04)
**              Added missing NEEDLIBS
**	25-jun-97 (joea)
**		Add in_archive to shadow table index, but not as part of key.
**	19-nov-97 (joea)
**		Set dba_name as a separate variable and copy IIuiDBA into it
**		since the command line argument still has the "-u" prefix.
**	01-dec-97 (padni01) bug 87279
**		In declaration of tbl_cursor in cleanTables(), select from
**		dd_databases where local_db = 1.
**	27-mar-98 (joea)
**		Discontinue use of RPing_ver_ok.
**	04-jun-98 (abbjo03) bug 91185
**		Check if the shadow index is persistent before attempting to
**		recreate it.
**	15-jun-98 (padni01) bug 89367
**		Add support for SQL 92 installations. Convert to lowercase 
**		before doing comparisons in where clauses.
**	02-nov-99 (gygro01) bug 99359 / pb ingrep 68
**		Arcclean deletes on a transaction basis, which can leave
**		base table entries without any shadow table entry.
**		This is mainly due to not checking the in_archive flag
**		during delete.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      17-may-02 (padni01) bug 107820
**              Add support for SQL 92 installations. Convert to lowercase
**              (dba_name and sha_idx_name) before doing comparisons in
**              where clauses.
**	11-Dec-2003 (inifa01) BUG 95231 INGREP 51
**		'arcclean' does not work correctly when a database contains
**		multiple CDDS's with different target types eg CDDS 0 with 
**		target type 1 (FULL PEER) and CDDS 1 with target type 2 (PRO)
**		Records from the FULL PEER archive table are not cleared and
**		there is an attempt to modify the non-existent PRO archive  
**		table resulting in error; Could not modify <archive table-name> 
**		- no iitables row found.
**		FIX: loop that stores rows to be deleted in dd_arcclean_tx_id 
**		sets 'cdds_type' to the value of the last row stored. added
**		'target_type' field to dd_arcclean_tx_id so that 'cdds_type'
**		for each row can be set before delete_rows() is called.
**      11-Jun-2004 (hanch04)
**          Removed reference to CI for the open source release.
**	29-Sep-2004 (drivi01)
**	    Updated NEEDLIBS to link dynamic library SHQLIB and SHFRAMELIB
**	    to replace its static libraries.
**	10-May-2007 (kibro01) b118062
**	    Update logic to delete rows where they exist in the shadow
**	    table but not in the archive, but also not any more in the
**	    base table, since that is the condition when they have been
**	    deleted - otherwise arcclean never removes such rows.
**	    Also ensure that statement lengths cannot be exceeded.
**/
/*
PROGRAM =	arcclean
NEEDLIBS =      REPMGRLIB REPCOMNLIB SHFRAMELIB SHQLIB SHCOMPATLIB \
		SHEMBEDLIB 
UNDEFS =	II_copyright
*/
# define SHADOW_TABLE		1
# define ARCHIVE_TABLE		2
# define TM_SIZE_STR		27
# define PURGE_SESSION 1
#define BASE_STMT_LEN	4096
static	char	*prog_name = ERx("arcclean");
static	char	time_buf[TM_SIZE_STR];
static	SYSTIME	now;
  static i2 cdds_type;
  static char dba_name[DB_MAXNAME+1];
  static char *db_name = NULL;
static	char	*user_name = ERx("");
static	char	*before_time = ERx("");
static	ARG_DESC args[] =
{
	/* required */
	{ERx("database"), DB_CHR_TYPE, FARG_PROMPT, (PTR)&db_name},
	{ERx("before_time"), DB_CHR_TYPE, FARG_PROMPT, (PTR)&before_time},
	/* optional */
	{ERx("user"), DB_CHR_TYPE, FARG_FAIL, (PTR)&user_name},
	NULL
};
static void clean_tables(void);
static void delete_rows(char *tbl_name, char *tbl_owner, i4 tbl_no, i2 db_no,
	i4 tx_id, char *cond_list);
static void modify_table(char *tbl_name, char *sha_idx_name, i4  tbl_type);
static void build_shadow_stmt(char *tbl_name, char *idx_name, char *str);
static i4 sqlerr_check(char *id_string, i4 num_rows_expected);
static void error_exit(void);
FUNC_EXTERN STATUS FEhandler(EX_ARGS *ex);
/*{
** Name:	main - archive clean
**
** Description:
**	remove unwanted rows from the shadow and archibe tables. An unwanted
**	row is one whose transaction date precedes a user-specified parameter
**	and which does not have a corresponding row in the input or
**	distribution queues.
**
** Inputs:
**	argc	- the number of arguments on the command line
**	argv	- the command line arguments
**		argv[1]	- the database to connect to
**		argv[2]	- the database to connect to
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
	if (IIUGuapParse(argc, argv, prog_name, args) != OK)
		PCexit(FAIL);
	/* Set up the generic exception handler */
	if (EXdeclare(FEhandler, &context) != OK)
	{
		EXdelete();
		PCexit(FAIL);
	}
	/* Open the database - normal UI_SESSION */
	if (FEningres(NULL, (i4)0, db_name, user_name, NULL) != OK)
	{
		EXdelete();
		PCexit(FAIL);
	}
/* # line 213 "arcclean.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set AUTOCOMMIT OFF");
    IIsyncup((char *)0,0);
  }
/* # line 214 "arcclean.sc" */	/* set_sql */
  {
    IILQssSetSqlio(3,(short *)0,1,32,0,(char *)"genericerror");
  }
/* # line 216 "arcclean.sc" */	/* host code */
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
	/*
	** connect two sessions - one to build the temp table and one to
	** do the deletion.
	*/
	STcopy(IIuiDBA, dba_name);
/* # line 233 "arcclean.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(PURGE_SESSION);
    IIsqUser(dba_name);
    IIsqConnect(0,db_name,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 235 "arcclean.sc" */	/* host code */
	if (RPdb_error_check(0, NULL) != OK)
	{
		SIprintf("%s: Cannot connect to database %s (session 2)\n",
			prog_name, db_name);
		SIflush(stdout);
		error_exit();
	}
/* # line 242 "arcclean.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE session where TIMEOUT=0");
    IIsyncup((char *)0,0);
  }
/* # line 244 "arcclean.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(UI_SESSION),(void *)IILQint(
    UI_SESSION));
  }
/* # line 245 "arcclean.sc" */	/* host code */
	TMnow(&now);
	TMstr(&now, time_buf);
	SIprintf("\n%s starting on '%s' at %s\n", prog_name, db_name,
		time_buf);
	SIflush(stdout);
	clean_tables();
	FEing_exit();
	TMnow(&now);
	TMstr(&now, time_buf);
	SIprintf("%s completed on %s at %s\n", prog_name, db_name,
		time_buf);
	SIflush(stdout);
	EXdelete();
	PCexit(OK);
}
/*{
**	sqlerr_check
**
**	check for an INGRES error - and the expectd number of rows:
**	-1 indicates that the caller does not care about the number
**	of rows. On an error - disconnect and exit.
*/
static i4
sqlerr_check(
char	*id_string,
i4	num_rows_expected)
{
	DBEC_ERR_INFO	errinfo;
	(void)RPdb_error_check(0, &errinfo);
	if (errinfo.errorno)
	{
		SIprintf("\n%s: DBMS error on %s\n", prog_name, id_string);
		SIflush(stdout);
		error_exit();
	}
	switch (num_rows_expected)
	{
	case -1:			/* don't care */
		break;
	case 0:
		if (errinfo.rowcount == 0)
			return;
		break;
	default:
		if (num_rows_expected != errinfo.rowcount)
		{
			SIprintf("%s: %s expected %d rows, but found %d\n",
				prog_name, id_string, num_rows_expected,
				errinfo.rowcount);
			SIflush(stdout);
			error_exit();
		}
		break;
	}
	return ((i4)errinfo.rowcount);
}
/*{
** Name:	clean_tables - clean the shadow and archive tables
**
** Description:
**	For each table in the dd_regist_tables list, read from the
**	corresponding shadow table all rows before the specified date. If a
**	corresponding IQ or DQ row does not exist, delete from the archive and
**	shadow for this database_no, transaction_id pair.
**
**	In addition, do not delete any rows whose transaction ID matches the
**	old transaction ID in either queue.
**
** FIXME:  Check dd_db_cdds and exit early if database doesn't have any
**	   CDDS's with target_type 1 or 2.
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
clean_tables()
{
  char tbl_name[DB_MAXNAME+1];
  char tbl_owner[DB_MAXNAME+1];
  char column_name[DB_MAXNAME+1];
  i4 tbl_no;
  i2 db_no;
  i4 tx_id;
  char *stmt;
  char *idx_list;
  i4 idx_list_len;
  char *cond_list;
	char	shadow_name[DB_MAXNAME+1];
	char	sha_idx_name[DB_MAXNAME+1];
	char	archive_name[DB_MAXNAME+1];
	i4	len;
	char	*txt;
	char	*txt_and = ERx(" AND ");
	char	*txt_sdot = ERx("s.");
	char	*txt_tdot = ERx(" = t.");
/* # line 360 "arcclean.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 361 "arcclean.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE session where READLOCK=NOLOCK");
    IIsyncup((char *)0,0);
  }
/* # line 363 "arcclean.sc" */	/* host code */
	/* check to see if the dd_arcclean_tx_id is present */
/* # line 364 "arcclean.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iitables where LOWERCASE(table_name)='dd_arcclean\
_tx_id' and LOWERCASE(table_owner)=LOWERCASE(");
    IIputdomio((short *)0,1,32,0,dba_name);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tx_id);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 369 "arcclean.sc" */	/* host code */
	sqlerr_check("id table check", 1);
	if (tx_id != 0)
	{
		SIprintf("%s: The temporary table dd_arcclean_tx_id exists in this database\n",
			prog_name);
		SIprintf("either someone else is running this program, or the table must be dropped.\n");
		SIflush(stdout);
		error_exit();
	}
/* # line 380 "arcclean.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table dd_arcclean_tx_id(table_name VARCHAR(32)not null, table_\
owner VARCHAR(32)not null, table_no INTEGER not null, target_type INTE\
GER not null, database_no SMALLINT not null, transaction_id INTEGER no\
t null)");
    IIsyncup((char *)0,0);
  }
/* # line 387 "arcclean.sc" */	/* host code */
	sqlerr_check("temp table create", 0);
/* # line 388 "arcclean.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 398 "arcclean.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"tbl_cursor",7717,28411);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(table_name), TRIM(table_owner), table_no, c.target_type f\
rom dd_regist_tables t, dd_db_cdds c, dd_databases d where d.local_db=\
1 and d.database_no=c.database_no and t.cdds_no=c.cdds_no and c.target\
_type in(1, 2)");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"tbl_cursor",7717,28411);
  }
/* # line 399 "arcclean.sc" */	/* host code */
	sqlerr_check("table cursor", 0);
	while (TRUE)
	{
/* # line 402 "arcclean.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"tbl_cursor",7717,28411,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,tbl_name);
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,tbl_owner);
      IIcsGetio((short *)0,1,30,4,&tbl_no);
      IIcsGetio((short *)0,1,30,2,&cdds_type);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 405 "arcclean.sc" */	/* host code */
		if (sqlerr_check("table cursor", -1) == 0)
			break;
		/* Get a list of key column names so that we can do a
		** match between shadow and base table to identify rows
		** which exist in the shadow but not any longer in the 
		** base table.  (kibro01) b118026
		*/
/* # line 419 "arcclean.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"key_cursor",7718,3771);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(column_name)from dd_regist_columns where table_no=");
    IIputdomio((short *)0,1,30,4,&tbl_no);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and key_sequence!=0 order by key_sequence");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"key_cursor",7718,3771);
  }
/* # line 420 "arcclean.sc" */	/* host code */
		idx_list_len = BASE_STMT_LEN;
		idx_list = MEreqmem(0, idx_list_len, TRUE, NULL);
		if (idx_list == NULL)
		{
		    SIprintf("Cannot allocate memory for archive cleaning\n");
		    SIflush(stdout);
		    error_exit();
		}
		STprintf(idx_list,"");
		while (TRUE)
		{
/* # line 432 "arcclean.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"key_cursor",7718,3771,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,column_name);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 434 "arcclean.sc" */	/* host code */
			if (sqlerr_check("key_cursor", -1) == 0)
				break;
			len = STlength(idx_list);
			if (len)
				STcat(idx_list,txt_and);
			if (len + STlength(txt_sdot) + STlength(txt_tdot) +
				STlength(column_name)*2 + 1 > idx_list_len)
			{
			    char *new_idx_list;
			    idx_list_len += BASE_STMT_LEN;
			    new_idx_list = MEreqmem(0, idx_list_len, TRUE, NULL);
			    if (new_idx_list == NULL)
			    {
				SIprintf("Cannot allocate memory for archive cleaning(2)\n");
				SIflush(stdout);
				error_exit();
			    }
			    STcopy(idx_list, new_idx_list);
			    MEfree(idx_list);
			    idx_list = new_idx_list;
			}
			STcat(idx_list,txt_sdot);
			STcat(idx_list,column_name);
			STcat(idx_list,txt_tdot);
			STcat(idx_list,column_name);
		}
/* # line 460 "arcclean.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"key_cursor",7718,3771);
  }
/* # line 462 "arcclean.sc" */	/* host code */
		/* Now build up the extra segment for the WHERE clause
		** include a check for shadow table entries where no matching
		** record exists in the base table, checked by...
		** NOT EXISTS(SELECT * FROM base t WHERE s.key1=t.key1 AND...)
		** using all the unique key items (kibro01) b118062
		*/
		txt = ERx(" AND (in_archive = 1 OR " \
			"(in_archive = 0 AND new_key = 1) OR " \
			"NOT EXISTS (SELECT * FROM %s t WHERE %s))");
		len = STlength(idx_list) + STlength(txt) + DB_MAXNAME + 1;
		cond_list = MEreqmem(0, len, TRUE, NULL);
		if (cond_list == NULL)
		{
			SIprintf("Cannot allocate memory for archive cleaning(3)\n");
			SIflush(stdout);
			error_exit();
		}
		STprintf(cond_list, txt, tbl_name, idx_list);
		MEfree(idx_list);
		/*  build a temp table (of known name) */
		SIprintf("Checking '%s.%s' ...\n", tbl_owner, tbl_name);
		SIflush(stdout);
		RPtblobj_name(tbl_name, tbl_no, TBLOBJ_SHA_TBL, shadow_name);
		/* Allocate enough for query below plus cond-list */
		len = 1000 + STlength(cond_list);
		stmt = MEreqmem(0, len, TRUE, NULL);
		if (stmt == NULL)
		{
			SIprintf("Cannot allocate memory for archive cleaning(4)\n");
			SIflush(stdout);
			error_exit();
		}
		STprintf(stmt,
			ERx("INSERT INTO dd_arcclean_tx_id SELECT DISTINCT "));
		STcat(stmt, RPedit_name(EDNM_SLITERAL, tbl_name, NULL));
		STcat(stmt, ERx(", "));
		STcat(stmt, RPedit_name(EDNM_SLITERAL, tbl_owner, NULL));
		STcat(stmt, ERx(", "));
		STprintf(stmt, ERx("%s%d"), stmt, tbl_no);
		STcat(stmt, ERx(", "));
		STprintf(stmt, ERx("%s%d"), stmt, cdds_type);
		STcat(stmt, ERx(", sourcedb, transaction_id FROM "));
		STcat(stmt, shadow_name);
		STcat(stmt, ERx(" s WHERE trans_time <= '"));
		STcat(stmt, before_time);
		STcat(stmt, ERx("'"));
		STcat(stmt, cond_list);
/* # line 513 "arcclean.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
  }
/* # line 514 "arcclean.sc" */	/* host code */
		sqlerr_check("tmp table insert", -1);
		MEfree(stmt);
	}
/* # line 517 "arcclean.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"tbl_cursor",7717,28411);
  }
/* # line 518 "arcclean.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 520 "arcclean.sc" */	/* host code */
	TMnow(&now);
	TMstr(&now, time_buf);
	SIprintf("Check complete - beginning clean at %s\n", time_buf);
	SIflush(stdout);
/* # line 528 "arcclean.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"tx_cursor",7719,218);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(table_name), TRIM(table_owner), table_no, target_type, da\
tabase_no, transaction_id from dd_arcclean_tx_id");
    IIcsQuery((char *)"tx_cursor",7719,218);
  }
/* # line 529 "arcclean.sc" */	/* host code */
	sqlerr_check("tx_cursor open", 0);
	while (TRUE)
	{
/* # line 532 "arcclean.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"tx_cursor",7719,218,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,tbl_name);
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,tbl_owner);
      IIcsGetio((short *)0,1,30,4,&tbl_no);
      IIcsGetio((short *)0,1,30,2,&cdds_type);
      IIcsGetio((short *)0,1,30,2,&db_no);
      IIcsGetio((short *)0,1,30,4,&tx_id);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 535 "arcclean.sc" */	/* host code */
		if (sqlerr_check("tx_cursor fetch", -1) == 0)
			break;
/* # line 538 "arcclean.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(PURGE_SESSION),(void *)IILQint(
    PURGE_SESSION));
  }
/* # line 539 "arcclean.sc" */	/* host code */
		delete_rows(tbl_name, tbl_owner, tbl_no, db_no, tx_id, 
			cond_list);
/* # line 541 "arcclean.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(UI_SESSION),(void *)IILQint(
    UI_SESSION));
  }
/* # line 542 "arcclean.sc" */	/* host code */
	}
/* # line 543 "arcclean.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(PURGE_SESSION),(void *)IILQint(
    PURGE_SESSION));
  }
/* # line 544 "arcclean.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 545 "arcclean.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(UI_SESSION),(void *)IILQint(
    UI_SESSION));
  }
/* # line 546 "arcclean.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"tx_cursor",7719,218);
  }
/* # line 547 "arcclean.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 549 "arcclean.sc" */	/* host code */
	MEfree(cond_list);
	/*
	** now get the table names from dd_regist_tables again and
	** modify the shadow and archive tables
	*/
/* # line 555 "arcclean.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(UI_SESSION),(void *)IILQint(
    UI_SESSION));
  }
/* # line 556 "arcclean.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"tbl_cursor",7717,28411);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(table_name), TRIM(table_owner), table_no, c.target_type f\
rom dd_regist_tables t, dd_db_cdds c, dd_databases d where d.local_db=\
1 and d.database_no=c.database_no and t.cdds_no=c.cdds_no and c.target\
_type in(1, 2)");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"tbl_cursor",7717,28411);
  }
/* # line 557 "arcclean.sc" */	/* host code */
	sqlerr_check("table cursor", 0);
	while (TRUE)
	{
/* # line 560 "arcclean.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"tbl_cursor",7717,28411,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,tbl_name);
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,tbl_owner);
      IIcsGetio((short *)0,1,30,4,&tbl_no);
      IIcsGetio((short *)0,1,30,2,&cdds_type);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 563 "arcclean.sc" */	/* host code */
		if (sqlerr_check("table cursor", -1) == 0)
			break;
		RPtblobj_name(tbl_name, tbl_no, TBLOBJ_SHA_TBL, shadow_name);
		RPtblobj_name(tbl_name, tbl_no, TBLOBJ_SHA_INDEX1,
			sha_idx_name);
		if (cdds_type == 1) /* arch tbl only exists for full peer */
			RPtblobj_name(tbl_name, tbl_no, TBLOBJ_ARC_TBL,
				archive_name);
/* # line 573 "arcclean.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(PURGE_SESSION),(void *)IILQint(
    PURGE_SESSION));
  }
/* # line 574 "arcclean.sc" */	/* host code */
		modify_table(shadow_name, sha_idx_name, SHADOW_TABLE);
		if (cdds_type == 1) /* arch tbl only exists for full peer */
			modify_table(archive_name, NULL, ARCHIVE_TABLE);
/* # line 577 "arcclean.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(UI_SESSION),(void *)IILQint(
    UI_SESSION));
  }
/* # line 578 "arcclean.sc" */	/* host code */
	}
/* # line 579 "arcclean.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"tbl_cursor",7717,28411);
  }
/* # line 580 "arcclean.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 582 "arcclean.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(PURGE_SESSION),(void *)IILQint(
    PURGE_SESSION));
  }
/* # line 583 "arcclean.sc" */	/* host code */
	modify_table(ERx("dd_input_queue"), NULL, FALSE);
	modify_table(ERx("dd_distrib_queue"), NULL, FALSE);
/* # line 585 "arcclean.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 586 "arcclean.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IIsqDisconnect();
  }
/* # line 588 "arcclean.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(UI_SESSION),(void *)IILQint(
    UI_SESSION));
  }
/* # line 589 "arcclean.sc" */	/* drop */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table dd_arcclean_tx_id");
    IIsyncup((char *)0,0);
  }
/* # line 590 "arcclean.sc" */	/* host code */
	sqlerr_check("tx_id_table drop", 0); 
/* # line 591 "arcclean.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 592 "arcclean.sc" */	/* host code */
}
/*{
** Name:	delete_rows - delete the old rows
**
** Description:
**	Purge the rows from the shadow and archive table
**
** Inputs:
**	tbl_name	- table name
**	tbl_owner	- table owner
**	tbl_no		- table number
**	db_no		- database number
**	tx_id		- transaction id
**	cond_list	- WHERE clause for key-column matching with base table
**
** Outputs:
**	none
**
** Returns:
**	none
*/
static void
delete_rows(
char	*tbl_name,
char	*tbl_owner,
i4	tbl_no,
i2	db_no,
i4	tx_id,
char	*cond_list)
# if 0
  char *tbl_name;
  i2 db_no;
  i4 tx_id;
# endif
{
  char *stmt;
  i4 stmt_len;
  i4 input_rowcount;
  i4 distrib_rowcount;
	i4	rows;
	char	shadow_name[DB_MAXNAME+1];
	char	archive_name[DB_MAXNAME+1];
	char	*sql_qry =
	    ERx("DELETE FROM %s s WHERE sourcedb = ? AND transaction_id = ?");
	stmt_len = STlength(sql_qry) + STlength(cond_list) + (DB_MAXNAME * 3) + 1;
	stmt = MEreqmem(0, stmt_len, TRUE, NULL);
	if (stmt == NULL)
		return;
/* # line 648 "arcclean.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"arcclean1",7720,6363);
      IIputdomio((short *)0,1,30,2,&db_no);
      IIputdomio((short *)0,1,30,4,&tx_id);
      IIputdomio((short *)0,1,30,2,&db_no);
      IIputdomio((short *)0,1,30,4,&tx_id);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"arcclean1",7720,6363);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_input_queue where(sourcedb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&db_no);
        IIwritio(0,(short *)0,1,32,0,(char *)"and transaction_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&tx_id);
        IIwritio(0,(short *)0,1,32,0,(char *)")or(old_sourcedb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&db_no);
        IIwritio(0,(short *)0,1,32,0,(char *)"and old_transaction_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&tx_id);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"arcclean1",7720,6363);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&input_rowcount);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 655 "arcclean.sc" */	/* host code */
	sqlerr_check("Input Q row count", 1);
/* # line 656 "arcclean.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 658 "arcclean.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"arcclean2",7721,8153);
      IIputdomio((short *)0,1,30,2,&db_no);
      IIputdomio((short *)0,1,30,4,&tx_id);
      IIputdomio((short *)0,1,30,2,&db_no);
      IIputdomio((short *)0,1,30,4,&tx_id);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"arcclean2",7721,8153);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_distrib_queue where(sourcedb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&db_no);
        IIwritio(0,(short *)0,1,32,0,(char *)"and transaction_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&tx_id);
        IIwritio(0,(short *)0,1,32,0,(char *)")or(old_sourcedb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&db_no);
        IIwritio(0,(short *)0,1,32,0,(char *)"and old_transaction_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&tx_id);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"arcclean2",7721,8153);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&distrib_rowcount);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 665 "arcclean.sc" */	/* host code */
	sqlerr_check("Distribution Q row count", 1);
/* # line 666 "arcclean.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 668 "arcclean.sc" */	/* host code */
	SIprintf("Tx [ '%s.%s' %d %d ] .. ", tbl_owner, tbl_name, db_no,
		tx_id);
	SIflush(stdout);
	if (input_rowcount == 0 && distrib_rowcount == 0)
	{
		RPtblobj_name(tbl_name, tbl_no, TBLOBJ_SHA_TBL, shadow_name);
		STprintf(stmt, sql_qry, shadow_name);
		STcat(stmt, cond_list);
/* # line 677 "arcclean.sc" */	/* prepare */
  {
    IIsqInit((char *)0);
    IIsqPrepare(0,(char *)"s_delete",(char *)0,0,stmt);
  }
/* # line 678 "arcclean.sc" */	/* host code */
		sqlerr_check("s prepare", 0);
/* # line 680 "arcclean.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExStmt((char *)"s_delete",1);
    IIputdomio((short *)0,1,30,2,&db_no);
    IIputdomio((short *)0,1,30,4,&tx_id);
    IIsyncup((char *)0,0);
  }
/* # line 681 "arcclean.sc" */	/* host code */
		rows = sqlerr_check("shadow delete", -1);
		SIprintf("shadow (%d %s) ", (i4)rows, rows == 1 ? "row" :
			"rows");
		SIflush(stdout);
		if (cdds_type == 1)	/* full peer */
		{
			RPtblobj_name(tbl_name, tbl_no, TBLOBJ_ARC_TBL,
				archive_name);
			STprintf(stmt, sql_qry, archive_name);
/* # line 691 "arcclean.sc" */	/* prepare */
  {
    IIsqInit((char *)0);
    IIsqPrepare(0,(char *)"a_delete",(char *)0,0,stmt);
  }
/* # line 692 "arcclean.sc" */	/* host code */
			sqlerr_check("a prepare", 0);
/* # line 694 "arcclean.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExStmt((char *)"a_delete",1);
    IIputdomio((short *)0,1,30,2,&db_no);
    IIputdomio((short *)0,1,30,4,&tx_id);
    IIsyncup((char *)0,0);
  }
/* # line 695 "arcclean.sc" */	/* host code */
			rows = sqlerr_check("archive delete", -1);
			SIprintf("archive (%d %s)", (i4)rows, rows == 1 ?
				"row" : "rows");
			SIflush(stdout);
		}
		SIprintf(" deleted\n");
		SIflush(stdout);
	}
	MEfree(stmt);
}
static void
modify_table(
char	*tbl_name,
char	*sha_idx_name,
i4	tbl_type)
# if 0
  char *tbl_name;
  char *sha_idx_name;
# endif
{
  char col_name[DB_MAXNAME+1];
  i4 key_seq;
  char sort_direction[5];
  char storage_structure[10];
  char is_compressed[5];
  char key_is_compressed[5];
  char duplicate_rows[5];
  char unique_rule[5];
  char location_name[DB_MAXNAME+1];
  char multi_locations[5];
  i4 table_ifillpct;
  i4 table_dfillpct;
  i4 table_lfillpct;
  i4 table_minpages;
  i4 table_maxpages;
  char persistent[2];
  char stmt[1024];
  char shadow_stmt[1024];
	char	convbuf[80];
	char	keybuf[1024];
	i4	rows;
	/* build the key clause for the modify statement */
	SIprintf("Modifying %s ...\n", tbl_name);
	SIflush(stdout);
	*keybuf = EOS;
/* # line 748 "arcclean.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"arcclean3",7722,3576);
      IIputdomio((short *)0,1,32,0,tbl_name);
      IIputdomio((short *)0,1,32,0,dba_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"arcclean3",7722,3576);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(column_name), key_sequence, TRIM(sort_direction)from iico\
lumns where LOWERCASE(table_name)=LOWERCASE(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,tbl_name);
        IIwritio(0,(short *)0,1,32,0,(char *)
")and LOWERCASE(table_owner)=LOWERCASE(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,dba_name);
        IIwritio(0,(short *)0,1,32,0,(char *)
")and key_sequence!=0 order by key_sequence");
        IIexDefine(0,(char *)"arcclean3",7722,3576);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,col_name);
      IIgetdomio((short *)0,1,30,4,&key_seq);
      IIgetdomio((short *)0,1,32,4,sort_direction);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 758 "arcclean.sc" */	/* host code */
		if (key_seq != 1)
			STcat(keybuf, ERx(", "));
		STcat(keybuf, RPedit_name(EDNM_DELIMIT, col_name, NULL));
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	rows = sqlerr_check("iicolumns lookup", -1);
	/*
	** determine the location of the tables and re-modify.
	** if this was the shadow table - record details of the
	** secondary index before destroying it.
	*/
	if (tbl_type == SHADOW_TABLE)
	{
/* # line 771 "arcclean.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"arcclean4",7723,29879);
      IIputdomio((short *)0,1,32,0,sha_idx_name);
      IIputdomio((short *)0,1,32,0,dba_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"arcclean4",7723,29879);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select persistent from iiindexes where LOWERCASE(index_name)=LOWERCAS\
E(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,sha_idx_name);
        IIwritio(0,(short *)0,1,32,0,(char *)
")and LOWERCASE(index_owner)=LOWERCASE(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,dba_name);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"arcclean4",7723,29879);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,1,persistent);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 776 "arcclean.sc" */	/* host code */
		sqlerr_check("iiindexes persistent", 1);
		if (CMcmpcase(persistent, ERx("Y")))
			build_shadow_stmt(tbl_name, sha_idx_name, shadow_stmt);
	}
/* # line 781 "arcclean.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"arcclean5",7724,31636);
      IIputdomio((short *)0,1,32,0,tbl_name);
      IIputdomio((short *)0,1,32,0,dba_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"arcclean5",7724,31636);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(storage_structure), is_compressed, key_is_compressed, dup\
licate_rows, unique_rule, TRIM(location_name), multi_locations, table_\
ifillpct, table_dfillpct, table_lfillpct, table_minpages, table_maxpag\
es from iitables where LOWERCASE(table_name)=");
        IIwritio(0,(short *)0,1,32,0,(char *)"LOWERCASE(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,tbl_name);
        IIwritio(0,(short *)0,1,32,0,(char *)
")and LOWERCASE(table_owner)=LOWERCASE(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,dba_name);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"arcclean5",7724,31636);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,9,storage_structure);
      IIgetdomio((short *)0,1,32,4,is_compressed);
      IIgetdomio((short *)0,1,32,4,key_is_compressed);
      IIgetdomio((short *)0,1,32,4,duplicate_rows);
      IIgetdomio((short *)0,1,32,4,unique_rule);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,location_name);
      IIgetdomio((short *)0,1,32,4,multi_locations);
      IIgetdomio((short *)0,1,30,4,&table_ifillpct);
      IIgetdomio((short *)0,1,30,4,&table_dfillpct);
      IIgetdomio((short *)0,1,30,4,&table_lfillpct);
      IIgetdomio((short *)0,1,30,4,&table_minpages);
      IIgetdomio((short *)0,1,30,4,&table_maxpages);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 794 "arcclean.sc" */	/* host code */
	rows = sqlerr_check("iitables lookup", -1);
	if (rows != 1)
	{
		SIprintf("Could not modify %s - no iitables row found.\n",
			tbl_name);
		SIflush(stdout);
/* # line 800 "arcclean.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 801 "arcclean.sc" */	/* host code */
		return;
	}
/* # line 803 "arcclean.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 805 "arcclean.sc" */	/* host code */
	/* build the modify statement */
	STprintf(stmt, ERx("MODIFY %s TO %s%s ON %s WITH "), tbl_name,
		storage_structure, *unique_rule == 'U' ? ERx(" UNIQUE") :
		ERx(" "), keybuf);
	/*
	** build the with clause
	**	- minpages and maxpages only apply with hash
	**	- ifillpct (indexfill == nonleaffill) with btree
	**	- lfillpct (leaffill) with btree
	**	- dfillpct (datafill) with btree hash and isam
	*/
	STprintf(convbuf, ERx("FILLFACTOR = %d, "), table_dfillpct);
	STcat(stmt, convbuf);
	if (STcompare(storage_structure, ERx("BTREE")) == 0)
	{
		STprintf(convbuf, ERx("NONLEAFFILL = %d, "), table_ifillpct);
		STcat(stmt, convbuf);
		STprintf(convbuf, ERx("LEAFFILL = %d, "), table_lfillpct);
		STcat(stmt, convbuf);
	}
	else if (STcompare(storage_structure, ERx("HASH")) == 0)
	{
		STprintf(convbuf, ERx("MINPAGES = %d, "), table_minpages);
		STcat(stmt, convbuf);
		STprintf(convbuf, ERx("MAXPAGES = %d, "), table_maxpages);
		STcat(stmt, convbuf);
	}
	/*
	** HEAP and ISAM are not considered - HEAP should
	** not be the structure, and ISAM only has a fillfactor
	*/
	/* now the location clause */
	STcat(stmt, ERx(" LOCATION = ("));
	STcat(stmt, location_name);
	if (*multi_locations == 'Y')
	{
/* # line 843 "arcclean.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(location_name), loc_sequence from iimulti_locations where\
 LOWERCASE(table_name)=LOWERCASE(");
    IIputdomio((short *)0,1,32,0,tbl_name);
    IIwritio(0,(short *)0,1,32,0,(char *)
")and LOWERCASE(table_owner)=LOWERCASE(");
    IIputdomio((short *)0,1,32,0,dba_name);
    IIwritio(0,(short *)0,1,32,0,(char *)")order by loc_sequence");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,location_name);
      IIgetdomio((short *)0,1,30,4,&key_seq);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 850 "arcclean.sc" */	/* host code */
			STcat(stmt, ERx(", "));
			STcat(stmt, location_name);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
		sqlerr_check("iicolumns lookup", -1);
/* # line 854 "arcclean.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 855 "arcclean.sc" */	/* host code */
	}
	STcat(stmt, ERx(")"));
/* # line 857 "arcclean.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
  }
/* # line 858 "arcclean.sc" */	/* host code */
	rows = sqlerr_check("table modify", -1);
	SIprintf(ERx(" (%d %s)\n"), rows, rows == 1 ? "row" : "rows");
	SIflush(stdout);
/* # line 861 "arcclean.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 863 "arcclean.sc" */	/* host code */
	if (tbl_type == SHADOW_TABLE && CMcmpcase(persistent, ERx("Y")))
	{
		if (*shadow_stmt != EOS)
		{
			SIprintf("Creating %s ...\n", sha_idx_name);
			SIflush(stdout);
/* # line 869 "arcclean.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(shadow_stmt);
    IIsyncup((char *)0,0);
  }
/* # line 870 "arcclean.sc" */	/* host code */
			rows = sqlerr_check("index creation", -1);
			SIprintf(ERx(" (%d %s)\n"), rows, rows == 1 ? "row" :
				"rows");
			SIflush(stdout);
/* # line 874 "arcclean.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 875 "arcclean.sc" */	/* host code */
		}
		else
		{
			SIprintf("Could not create %s - no iitables row found.\n",
				sha_idx_name);
			SIflush(stdout);
		}
	}
}
static void
build_shadow_stmt(
char	*tbl_name,
char	*idx_name,
char	*str)
# if 0
  char *idx_name;
# endif
{
  char col_name[DB_MAXNAME+1];
  char sort_direction[5];
  i4 key_seq;
  char storage_structure[10];
  char is_compressed[5];
  char key_is_compressed[5];
  char duplicate_rows[5];
  char unique_rule[5];
  char location_name[DB_MAXNAME+1];
  char multi_locations[5];
  i4 table_ifillpct;
  i4 table_dfillpct;
  i4 table_lfillpct;
  i4 table_minpages;
  i4 table_maxpages;
	char	convbuf[80];
	char	stmt[1024];
	char	keybuf[1024];
	i4	rows;
	/* build the key clause for the index statement */
	*keybuf = *stmt = EOS;
/* # line 921 "arcclean.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"arcclean6",7725,28372);
      IIputdomio((short *)0,1,32,0,idx_name);
      IIputdomio((short *)0,1,32,0,dba_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"arcclean6",7725,28372);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(column_name), key_sequence, TRIM(sort_direction)from iico\
lumns where LOWERCASE(table_name)=LOWERCASE(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,idx_name);
        IIwritio(0,(short *)0,1,32,0,(char *)
")and LOWERCASE(table_owner)=LOWERCASE(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,dba_name);
        IIwritio(0,(short *)0,1,32,0,(char *)
")and key_sequence!=0 and LOWERCASE(column_name)!='tidp' order by key_\
sequence");
        IIexDefine(0,(char *)"arcclean6",7725,28372);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,col_name);
      IIgetdomio((short *)0,1,30,4,&key_seq);
      IIgetdomio((short *)0,1,32,4,sort_direction);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 932 "arcclean.sc" */	/* host code */
		if (key_seq != 1)
			STcat(keybuf, ERx(", "));
		STcat(keybuf, RPedit_name(EDNM_DELIMIT, col_name, NULL));
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
	sqlerr_check("iicolumns lookup", -1);
	/* determine the location of the index */
/* # line 939 "arcclean.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"arcclean7",7726,5555);
      IIputdomio((short *)0,1,32,0,idx_name);
      IIputdomio((short *)0,1,32,0,dba_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"arcclean7",7726,5555);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(storage_structure), is_compressed, key_is_compressed, dup\
licate_rows, unique_rule, TRIM(location_name), multi_locations, table_\
ifillpct, table_dfillpct, table_lfillpct, table_minpages, table_maxpag\
es from iitables where LOWERCASE(table_name)=");
        IIwritio(0,(short *)0,1,32,0,(char *)"LOWERCASE(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,idx_name);
        IIwritio(0,(short *)0,1,32,0,(char *)
")and LOWERCASE(table_owner)=LOWERCASE(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,dba_name);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"arcclean7",7726,5555);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,9,storage_structure);
      IIgetdomio((short *)0,1,32,4,is_compressed);
      IIgetdomio((short *)0,1,32,4,key_is_compressed);
      IIgetdomio((short *)0,1,32,4,duplicate_rows);
      IIgetdomio((short *)0,1,32,4,unique_rule);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,location_name);
      IIgetdomio((short *)0,1,32,4,multi_locations);
      IIgetdomio((short *)0,1,30,4,&table_ifillpct);
      IIgetdomio((short *)0,1,30,4,&table_dfillpct);
      IIgetdomio((short *)0,1,30,4,&table_lfillpct);
      IIgetdomio((short *)0,1,30,4,&table_minpages);
      IIgetdomio((short *)0,1,30,4,&table_maxpages);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 952 "arcclean.sc" */	/* host code */
	rows = sqlerr_check("iitables lookup", -1);
	if (rows != 1)
	{
/* # line 955 "arcclean.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 956 "arcclean.sc" */	/* host code */
		STcopy(stmt, str);
		return;
	}
	/* build the create index statement */
	STprintf(stmt,
		ERx("CREATE %sINDEX %s ON %s (%s, in_archive) WITH STRUCTURE \
= %s, KEY = (%s), "),
		*unique_rule == 'U' ? ERx("UNIQUE ") : ERx(""), idx_name,
		tbl_name, keybuf, storage_structure, keybuf);
	/*
	** build the with clause
	**	- minpages and maxpages only apply with hash
	**	- ifillpct (indexfill == nonleaffill) with btree
	**	- lfillpct (leaffill) with btree
	**	- dfillpct (datafill) with btree hash and isam
	*/
	STprintf(convbuf, ERx("FILLFACTOR = %d, "), table_dfillpct);
	STcat(stmt, convbuf);
	if (STcompare(storage_structure, ERx("BTREE")) == 0) {
		STprintf(convbuf, ERx("NONLEAFFILL = %d, "), table_ifillpct);
		STcat(stmt, convbuf);
		STprintf(convbuf, ERx("LEAFFILL = %d, "), table_lfillpct);
		STcat(stmt, convbuf);
	}
	else if (STcompare(storage_structure, ERx("HASH")) == 0)
	{
		STprintf(convbuf, ERx("MINPAGES = %d, "), table_minpages);
		STcat(stmt, convbuf);
		STprintf(convbuf, ERx("MAXPAGES = %d, "), table_maxpages);
		STcat(stmt, convbuf);
	}
	/* ISAM is not considered since it only has a fillfactor */
	/* now the location clause */
	STcat(stmt, ERx(" LOCATION = ("));
	STcat(stmt, location_name);
	if (*multi_locations == 'Y')
	{
/* # line 996 "arcclean.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(location_name), loc_sequence from iimulti_locations where\
 LOWERCASE(table_name)=LOWERCASE(");
    IIputdomio((short *)0,1,32,0,idx_name);
    IIwritio(0,(short *)0,1,32,0,(char *)
")and LOWERCASE(table_owner)=LOWERCASE(");
    IIputdomio((short *)0,1,32,0,dba_name);
    IIwritio(0,(short *)0,1,32,0,(char *)")order by loc_sequence");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,location_name);
      IIgetdomio((short *)0,1,30,4,&key_seq);
      if (IIerrtest() != 0) goto IIrtB4;
/* # line 1003 "arcclean.sc" */	/* host code */
			STcat(stmt, ERx(", "));
			STcat(stmt, location_name);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE4:;
  }
		sqlerr_check("iicolumns lookup", -1);
/* # line 1007 "arcclean.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 1008 "arcclean.sc" */	/* host code */
	}
	STcat(stmt, ERx(")"));
	/* done building the string.. */
	STcopy(stmt, str);
}
/*
**	disconnect if required and exit, cleaning up the dd_arcclean_tx_id
**	do not check the return from the drop - the table may not
**	exist.
*/
void
error_exit()
{
  i4 curr_session;
/* # line 1028 "arcclean.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&curr_session,11);
  }
/* # line 1029 "arcclean.sc" */	/* host code */
	if (curr_session == UI_SESSION)
/* # line 1030 "arcclean.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(PURGE_SESSION),(void *)IILQint(
    PURGE_SESSION));
  }
/* # line 1032 "arcclean.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 1033 "arcclean.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IIsqDisconnect();
  }
/* # line 1035 "arcclean.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(UI_SESSION),(void *)IILQint(
    UI_SESSION));
  }
/* # line 1036 "arcclean.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 1037 "arcclean.sc" */	/* host code */
	FEing_exit();
	EXdelete();
	PCexit(FAIL);
}
