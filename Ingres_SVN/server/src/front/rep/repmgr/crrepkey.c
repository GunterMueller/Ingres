# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <st.h>
# include <cv.h>
# include <nm.h>
# include <lo.h>
# include <er.h>
# include <me.h>
# include <si.h>
# include <gl.h>
# include <iicommon.h>
# include <adf.h>
# include <fe.h>
# include <ug.h>
# include <rpu.h>
# include <tblobjs.h>
# include <wchar.h>
# include "errm.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlda.h"
# include <tbldef.h>
/**
** Name:	crrepkey.sc - create replication keys
**
** Description:
**	Defines
**		create_replication_keys - create replication keys
**		free_sqlda		- free the sqlda buffer
**		get_cdds_no		- get the CDDS number
**		get_dd_priority		- get the priority
**
** History:
**	09-jan-97 (joea)
**		Created based on crrepkey.sc in replicator library.
**	29-apr-97 (joea) bug 81857
**		Remove inserts into dd_transaction_id. Determine if temporary
**		table exists before trying to drop it. Correct manipulation of
**		locations for shadow and input queue files.
**	18-nov-97 (padni01) bug 87055
**		Add missing case for text datatype (to be same as that for the
**		varchar datatype) for building values part of insert string.
**	09-mar-98 (padni01) bug 86940
**		In building values part of insert string, prefix wc with 'r.'
**		and not prefix tmp with the 'tab' character but write 'tab' to 
**		file_data before writing tmp to it.
**	22-jul-98 (padni01) bug 90827
**		Add support for decimal datatype
**	26-nov-98 (abbjo03/islsa01) bug 92413
**		This fix prevents converting varchar data to char data while
**		data are copied from source table to shadow table. For example,
**		if the source table contains varchar(30) - data '234\\567' (that
**		is a single backslash). The shadow table should exactly hold
**		same data '234\\567' instead of '234567', which is caused by
**		char conversion from varchar. The local variable len_spec is
**		called to store the length of data value.
**	23-mar-1999 (islsa01) bug 95574
**		Required to change the aliases in the select string. Rather,
**		this bug occurs when the lookup column is NOT the same as the
**		replication key on the base table. Instead of 'r' the alias for
**		the lookup table and 't' the alias for the base table, changes
**		are made to switch the aliases.
**	26-oct-99 (gygro01) bug 99272/pb ingrep 67
**		Createkeys fails if base table keys has datatype char/varchar/
**		date/byte/byte varying/text/c and horizontal partitioning is
**		setup. Problem is due to missing apostrophe around the values.
**	10-nov-99 (abbjo03) sir 99462
**		Count the actual number of rows in the shadow table. If it's
**		empty, drop the index, disable journaling and modify it to heap
**		to allow use of LOAD on the copy. Use a global temporary table
**		and only select the key columns into it. Change DD_TRANS_MAX
**		to II_REP_CREATEKEYS_TRANS_MAX which should hold a number of
**		rows and will now be documented.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
# define	MAX_SIZE		6000
# define	TRANS_MAX		10000
# define	MODIFY_TABLE_LIMIT	10000
GLOBALREF ADF_CB	*RMadf_cb;
GLOBALREF
  TBLDEF *RMtbl;
FUNC_EXTERN STATUS RMmodify_input_queue(void);
FUNC_EXTERN STATUS RMmodify_shadow(TBLDEF *tbl, bool recreate_index);
static void free_sqlda(IISQLDA *sqlda, i4  column_count);
static STATUS get_cdds_no(char *wc, i2 *cdds_no);
static STATUS get_dd_priority(char *wc, i2 *dd_priority);
/*{
** Name:	create_replication_keys - create replication keys
**
** Description:
**	Creates the replication keys in the shadow table and, optionally,
**	the input queue table.
**
** Inputs:
**	table_no	- table number
**	queue_flag	- do the input queue too
**
** Outputs:
**	none
**
** Returns:
**
** History
** 19-Aug-1998 (nicph02)
**	Bug 92547: Text for error message 1139 was not displaying the datatype
** 20-Aug-1998 (nicph02)
**	Bug 90716: Added support to 'byte varying'/'byte' datatype
**	03-Dec-2003 (gupsh01)
**	    Added support for nchar/nvarchar datatypes.
**	05-Apr-2004 (gupsh01)
**	    Fixed server crash due to stack overflow on windows.
**	    bug 112097.
**	05-Oct-2006 (gupsh01)
**	    Added support for ANSI date/time types.
*/
STATUS
create_replication_keys(
i4	table_no,
bool	queue_flag)
{
	FILE	*shadow_file;
	FILE	*queue_file;
	i4	exists;
	i4	column_count;
	short	null_ind[DB_MAX_COLS];
	char	file_data[MAX_SIZE];
	char	shadow_filename[MAX_LOC+1];
	char	queue_filename[MAX_LOC+1];
	LOCATION	sh_loc;
	LOCATION	iq_loc;
	char	*loc_name_ptr;
	COLDEF	*col_p;
	i4	i, j;
	char	exists_where_clause[MAX_SIZE];
	char	tmp[MAX_SIZE];
	char	wc[MAX_SIZE];
	char	key_cols[MAX_SIZE];
	char	tmp_table[DB_MAXNAME+1];
	char	column_name[DB_MAX_COLS][DB_MAXNAME*2+3];
	char	column_datatype[DB_MAX_COLS][DB_MAXNAME+1];
	i4	column_length[DB_MAX_COLS];
	i4	approx_rows;
	i4	rows_processed = 0;
  i4 trans_max = TRANS_MAX;
  char stmt[256];
  char copy_stmt[MAX_SIZE];
  char qcopy_stmt[512];
  i2 sourcedb = 0;
  i4 transaction_id = 0;
  char timestamp[26];
  i2 cdds_no;
  i2 dd_priority;
  i4 done;
  STATUS err;
  STATUS stat;
  char select_stmt[MAX_SIZE];
  i4 session1;
  i4 session2;
  char dbname[DB_MAXNAME*2+3];
  char dba[DB_MAXNAME+1];
  i4 base_rows, shadow_rows, input_rows;
  wchar_t **nch_val;
  i4 nch_cnt = 0;
  struct {
    short len;
    wchar_t text[256];
  } *nvch_val;
  i4 nvch_cnt = 0;
	char	*env_ptr;
	IISQLDA	_sqlda;
	IISQLDA	*sqlda = &_sqlda;
    for (;;)
	{
		nch_val = (wchar_t **)MEreqmem (0, 
				(sizeof(wchar_t *) * DB_MAX_COLS), 0, &stat);
		if (stat)
		  break;
		for (i=0; i < DB_MAX_COLS; i++)
		{
		  nch_val[i] = (wchar_t *)MEreqmem (0, 
				(sizeof(wchar_t) * 256), 0, &stat); 
		}
		if (stat)
		  break;
  		nvch_val =  MEreqmem (0, 
				sizeof(nvch_val) * DB_MAX_COLS, 0, &stat); 
		if (stat)
		  break;
		NMgtAt(ERx("II_REP_CREATEKEYS_TRANS_MAX"), &env_ptr);
		if (env_ptr != NULL && *env_ptr != EOS)
		{
			CVal(env_ptr, &trans_max);
			if (trans_max < TRANS_MAX)
				trans_max = TRANS_MAX;
		}
		err = RMtbl_fetch(table_no, TRUE);
		if (err)
		{
/* # line 226 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 227 "crrepkey.sc" */	/* host code */
			return (err);
		}
/* # line 229 "crrepkey.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 231 "crrepkey.sc" */	/* host code */
		IIUGmsg(ERget(F_RM00DD_Creating_keys), FALSE, 2, RMtbl->table_owner,
			RMtbl->table_name);
/* # line 233 "crrepkey.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session with ON_ERROR=rollback TRANSACTION");
    IIsyncup((char *)0,0);
  }
/* # line 234 "crrepkey.sc" */	/* set_sql */
  {
    IILQssSetSqlio(6,(short *)0,1,30,4,&trans_max);
  }
/* # line 235 "crrepkey.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select database_no, TRIM(vnode_name) +'::' +TRIM(database_name), DBMS\
INFO('dba')from dd_databases where local_db=1");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&sourcedb);
      IIgetdomio((short *)0,1,32,DB_MAXNAME*2+3-1,dbname);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,dba);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 240 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 241 "crrepkey.sc" */	/* host code */
		if (err)
		{
			IIUGerr(E_RM0030_Err_retrieve_db_name, UG_ERR_ERROR, 0);
/* # line 244 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 245 "crrepkey.sc" */	/* host code */
			return (E_RM0030_Err_retrieve_db_name);
		}
		err = table_exists(RMtbl->table_name, RMtbl->table_owner, &exists);
		if (err)
		{
/* # line 252 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 253 "crrepkey.sc" */	/* host code */
			return (err);
		}
		if (!exists)
		{
			IIUGerr(E_RM0124_Tbl_not_exist, UG_ERR_ERROR, 2,
				RMtbl->table_owner, RMtbl->table_name);
/* # line 259 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 260 "crrepkey.sc" */	/* host code */
			return (E_RM0124_Tbl_not_exist);
		}
		err = table_exists(RMtbl->sha_name, dba, &exists);
		if (err)
		{
/* # line 266 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 267 "crrepkey.sc" */	/* host code */
			return (err);
		}
		if (!exists)
		{
			IIUGerr(E_RM0125_Shadow_not_exist, UG_ERR_ERROR, 2,
				RMtbl->table_owner, RMtbl->table_name);
/* # line 273 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 274 "crrepkey.sc" */	/* host code */
			return (E_RM0125_Shadow_not_exist);
		}
		STprintf(stmt, ERx("SELECT COUNT(*) FROM %s"), RMtbl->sha_name);
/* # line 278 "crrepkey.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqMods(3);
    IIsqMods(1);
    IIsqExImmed(stmt);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&shadow_rows);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 280 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 281 "crrepkey.sc" */	/* host code */
		if (err)
		{
			IIUGerr(E_RM0126_Err_rtrv_num_rows, UG_ERR_ERROR, 2,
				RMtbl->table_owner, RMtbl->sha_name);
			return (E_RM0126_Err_rtrv_num_rows);
		}
/* # line 287 "crrepkey.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select num_rows from iitables where table_name=");
    IIputdomio((short *)0,1,32,0,RMtbl->table_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
    IIputdomio((short *)0,1,32,0,RMtbl->table_owner);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&base_rows);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 292 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 293 "crrepkey.sc" */	/* host code */
		if (err)
		{
			IIUGerr(E_RM0126_Err_rtrv_num_rows, UG_ERR_ERROR, 2,
				RMtbl->table_owner, RMtbl->table_name);
			return (E_RM0126_Err_rtrv_num_rows);
		}
		if (!shadow_rows)
		{
			char	idx_name[DB_MAXNAME+1];
			STprintf(stmt, ERx("SET NOJOURNALING ON %s"), RMtbl->sha_name);
/* # line 305 "crrepkey.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
  }
/* # line 306 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 307 "crrepkey.sc" */	/* host code */
			if (err)
			{
				IIUGerr(E_RM0127_Err_set_nojournal, UG_ERR_ERROR, 2,
					RMtbl->table_owner, RMtbl->sha_name);
/* # line 311 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 312 "crrepkey.sc" */	/* host code */
				return (E_RM0127_Err_set_nojournal);
			}
			RPtblobj_name(RMtbl->table_name, table_no, TBLOBJ_SHA_INDEX1,
				idx_name);
			err = table_exists(idx_name, RMtbl->table_owner, &exists);
			if (err != OK)
			{
/* # line 319 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 320 "crrepkey.sc" */	/* host code */
				return (err);
			}
			if (exists)
			{
				STprintf(stmt, ERx("DROP INDEX %s"), idx_name);
/* # line 325 "crrepkey.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
  }
/* # line 326 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 327 "crrepkey.sc" */	/* host code */
				if (err)
				{
					IIUGerr(E_RM0128_Err_dropping, UG_ERR_ERROR, 1,
						idx_name);
/* # line 331 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 332 "crrepkey.sc" */	/* host code */
					return (E_RM0128_Err_dropping);
				}
			}
			STprintf(stmt, ERx("MODIFY %s TO HEAP"), RMtbl->sha_name);
/* # line 336 "crrepkey.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
  }
/* # line 337 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 338 "crrepkey.sc" */	/* host code */
			if (err)
			{
				IIUGerr(E_RM0129_Err_modifying, UG_ERR_ERROR, 1,
					RMtbl->sha_name);
/* # line 342 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 343 "crrepkey.sc" */	/* host code */
				return (E_RM0129_Err_modifying);
			}
		}
		else
		{
			if (RPtblobj_name(RMtbl->table_name, table_no, TBLOBJ_TMP_TBL,
				tmp_table))
			{
/* # line 351 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 352 "crrepkey.sc" */	/* host code */
				IIUGerr(E_RM00EB_Err_gen_obj_name, UG_ERR_ERROR, 2,
					RMtbl->table_owner, RMtbl->table_name);
				return (E_RM00EB_Err_gen_obj_name);
			}
			*key_cols = EOS;
			for (col_p = RMtbl->col_p; col_p < RMtbl->col_p + RMtbl->ncols;
				++col_p)
			{
				if (!col_p->key_sequence)
					continue;
				if (*key_cols)
					STcat(key_cols, ", ");
				STcat(key_cols, col_p->dlm_column_name);
			}
			STprintf(stmt, ERx("DECLARE GLOBAL TEMPORARY TABLE SESSION.%s \
	AS SELECT %s FROM %s ON COMMIT PRESERVE ROWS WITH NORECOVERY, STRUCTURE = HEAP,\
	NOCOMPRESSION"),
				tmp_table, key_cols, RMtbl->sha_name);
/* # line 370 "crrepkey.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
  }
/* # line 371 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 372 "crrepkey.sc" */	/* host code */
			if (err)
			{
				IIUGerr(E_RM012A_Err_creat_tmp_tbl, UG_ERR_ERROR, 1,
					tmp_table);
				return (E_RM012A_Err_creat_tmp_tbl);
			}
		}
		if (NMloc(TEMP, PATH, NULL, &sh_loc) != OK)
			return (FAIL);
		LOcopy(&sh_loc, shadow_filename, &sh_loc);
		if (LOuniq(ERx("sh"), ERx("dat"), &sh_loc) != OK)
			return (FAIL);
		LOtos(&sh_loc, &loc_name_ptr);
		STcopy(loc_name_ptr, shadow_filename);
		if (SIfopen(&sh_loc, ERx("w"), SI_TXT, SI_MAX_TXT_REC, &shadow_file)
			!= OK)
		{
/* # line 390 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 391 "crrepkey.sc" */	/* host code */
			IIUGerr(E_RM005E_Error_open_file, UG_ERR_ERROR, 1,
				shadow_filename);
			return (E_RM005E_Error_open_file);
		}
		if (queue_flag)
		{
/* # line 398 "crrepkey.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select num_rows from iitables where table_name='dd_input_queue' and t\
able_owner=");
    IIputdomio((short *)0,1,32,0,dba);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&input_rows);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 403 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 404 "crrepkey.sc" */	/* host code */
			if (err)
			{
				IIUGerr(E_RM0126_Err_rtrv_num_rows, UG_ERR_ERROR, 2,
					dba, ERx("dd_input_queue"));
				return (E_RM0126_Err_rtrv_num_rows);
			}
			if (input_rows < MODIFY_TABLE_LIMIT)
			{
/* # line 412 "crrepkey.sc" */	/* modify */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"modify dd_input_queue to HEA\
P");
    IIsyncup((char *)0,0);
  }
/* # line 413 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 414 "crrepkey.sc" */	/* host code */
				if (err)
				{
					IIUGerr(E_RM0129_Err_modifying, UG_ERR_ERROR, 1,
						ERx("dd_input_queue"));
					return (E_RM0129_Err_modifying);
				}
			}
			if (NMloc(TEMP, PATH, NULL, &iq_loc) != OK)
				return (FAIL);
			LOcopy(&iq_loc, queue_filename, &iq_loc);
			if (LOuniq(ERx("iq"), ERx("dat"), &iq_loc) != OK)
				return (FAIL);
			LOtos(&iq_loc, &loc_name_ptr);
			STcopy(loc_name_ptr, queue_filename);
			if (SIfopen(&iq_loc, ERx("w"), SI_TXT, SI_MAX_TXT_REC,
				&queue_file) != OK)
			{
/* # line 432 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 433 "crrepkey.sc" */	/* host code */
				IIUGerr(E_RM005E_Error_open_file, UG_ERR_ERROR, 1,
					queue_filename);
				return (E_RM005E_Error_open_file);
			}
			STprintf(qcopy_stmt, ERx("COPY TABLE dd_input_queue (sourcedb =\
	c0tab, transaction_id = c0tab, sequence_no = c0tab, table_no = c0tab, \
	old_sourcedb = c0tab, old_transaction_id = c0tab, old_sequence_no = c0tab, \
	trans_time = c0tab, trans_type = c0tab, cdds_no = c0tab, dd_priority = c0nl) \
	FROM '%s'"),
				queue_filename);
		}
		STcopy(ERx("SELECT DISTINCT "), select_stmt);
		*exists_where_clause = EOS;
		STprintf(copy_stmt,
			ERx("COPY TABLE %s (sourcedb = c0tab, transaction_id = c0tab, \
	sequence_no = c0tab, trans_type = c0tab, trans_time = c0"),
			RMtbl->sha_name);
		for (column_count = 0, col_p = RMtbl->col_p;
			col_p < RMtbl->col_p + RMtbl->ncols; ++col_p)
		{
			if (!col_p->key_sequence)
				continue;
			if (column_count)
			{
				STcat(select_stmt, ERx(", "));
				STcat(exists_where_clause, ERx(" AND "));
			}
			STprintf(tmp, ERx("s.%s = t.%s"), col_p->dlm_column_name,
				col_p->dlm_column_name);
			STcat(exists_where_clause, tmp);
			if (STequal(col_p->column_datatype, ERx("date")) || 
			    STequal(col_p->column_datatype, ERx("ingresdate")) || 
			    STequal(col_p->column_datatype, ERx("ansidate")) || 
			    STequal(col_p->column_datatype, 
						ERx("time without time zone")) ||
			    STequal(col_p->column_datatype, 
						ERx("time with time zone")) ||
			    STequal(col_p->column_datatype, 
						ERx("time with local time zone")) ||
			    STequal(col_p->column_datatype, 
						ERx("timestamp without time zone")) ||
			    STequal(col_p->column_datatype, 
						ERx("timestamp with time zone")) ||
			    STequal(col_p->column_datatype, 
						ERx("timestamp with local time zone")) ||
			    STequal(col_p->column_datatype, 
						ERx("interval year to month")) ||
			    STequal(col_p->column_datatype, 
						ERx("interval day to second")))
			{
			    STprintf(tmp, ERx("%s = CHAR(%s)"),
					col_p->dlm_column_name, col_p->dlm_column_name);
			    if (STequal(col_p->column_datatype, ERx("date")) || 
			        STequal(col_p->column_datatype, ERx("ingresdate")))
			      column_length[column_count] = 26;
			    else if (STequal(col_p->column_datatype, ERx("ansidate")))
			      column_length[column_count] = 18;
			    else if (STequal(col_p->column_datatype, 
						ERx("time without time zone")))
			      column_length[column_count] = 22;
			    else if (STequal(col_p->column_datatype, 
						ERx("time with time zone")) ||
			    	     STequal(col_p->column_datatype, 
						ERx("time with local time zone")))
				column_length[column_count] = 32;
			    else if (STequal(col_p->column_datatype, 
						ERx("timestamp without time zone")))
				column_length[column_count] = 40;
			    else if (STequal(col_p->column_datatype, 
						ERx("timestamp with time zone")) ||
			    	     STequal(col_p->column_datatype, 
						ERx("timestamp with local time zone")))
				column_length[column_count] = 50;
			    else if (STequal(col_p->column_datatype, 
						ERx("interval year to month")))
				column_length[column_count] = 16;
			    else if (STequal(col_p->column_datatype, 
						ERx("interval day to second")))
				column_length[column_count] = 46;
			}
			else if (STequal(col_p->column_datatype, ERx("decimal")))
			{
				STprintf(tmp, ERx("%s = CHAR(%s)"),
					col_p->dlm_column_name, col_p->dlm_column_name);
				column_length[column_count] = col_p->column_length + 2;
			}
			else if (STequal(col_p->column_datatype, ERx("money")))
			{
				STprintf(tmp, ERx("%s = FLOAT8(%s)"),
					col_p->dlm_column_name, col_p->dlm_column_name);
				column_length[column_count] = sizeof(double);
			}
			else if (STequal(col_p->column_datatype, ERx("varchar")) ||
				STequal(col_p->column_datatype, ERx("text")) ||
				STequal(col_p->column_datatype, ERx("byte varying")))
			{
				column_length[column_count] = col_p->column_length +
					sizeof(short);
				STcopy(col_p->dlm_column_name, tmp);
			}
			else if (STequal(col_p->column_datatype, ERx("nvarchar")))
			{
				column_length[column_count] =
					col_p->column_length * sizeof(wchar_t) + 
					sizeof (short);
				STcopy(col_p->dlm_column_name, tmp);
			}
			else if (STequal(col_p->column_datatype, ERx("integer")) ||
				STequal(col_p->column_datatype, ERx("float")) ||
				STequal(col_p->column_datatype, ERx("c")) ||
				STequal(col_p->column_datatype, ERx("char")) ||
				STequal(col_p->column_datatype, ERx("byte")))
			{
				STcopy(col_p->dlm_column_name, tmp);
				column_length[column_count] = col_p->column_length;
			}
			else if (STequal(col_p->column_datatype, ERx("nchar")))
			{
				column_length[column_count] =
					col_p->column_length * sizeof(wchar_t);
				STcopy(col_p->dlm_column_name, tmp);
			}
			else
			{
				IIUGerr(E_RM012B_Unsupp_datatype, UG_ERR_ERROR, 2,
					col_p->column_datatype, col_p->column_name);
				free_sqlda(sqlda, column_count);
				return (E_RM012B_Unsupp_datatype);
			}
			STcat(select_stmt, tmp);
			sqlda->sqlvar[column_count].sqldata = (char *)MEreqmem(0,
				(u_i4)(column_length[column_count] + 1), TRUE, NULL);
			if (sqlda->sqlvar[column_count].sqldata == NULL)
			{
				IIUGerr(E_RM00FE_Err_alloc_col, UG_ERR_ERROR, 0);
				free_sqlda(sqlda, column_count);
				return (E_RM00FE_Err_alloc_col);
			}
			sqlda->sqlvar[column_count].sqlind = &null_ind[column_count];
			if (STequal(col_p->column_datatype, ERx("varchar")) ||
				STequal(col_p->column_datatype, ERx("text")) ||
				STequal(col_p->column_datatype, ERx("byte varying")))
				STprintf(tmp, ERx("tab, %s = varchar(0)"),
					col_p->dlm_column_name);
			else if (STequal(col_p->column_datatype, ERx("nchar")))
				STprintf(tmp, ERx("tab, %s = nchar(0)"),
					col_p->dlm_column_name);
			else if (STequal(col_p->column_datatype, ERx("nvarchar")))
				STprintf(tmp, ERx("tab, %s = nvarchar(0)"),
					col_p->dlm_column_name);
			else
				STprintf(tmp, ERx("tab, %s = c0"),
					col_p->dlm_column_name);
			STcat(copy_stmt, tmp);
			STcopy(col_p->dlm_column_name, column_name[column_count]);
			STcopy(col_p->column_datatype, column_datatype[column_count]);
			++column_count;
		}
		if (err)
		{
			free_sqlda(sqlda, column_count);
			return (err);
		}
/* # line 600 "crrepkey.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 601 "crrepkey.sc" */	/* host code */
		STcat(exists_where_clause, ERx(")"));
		STprintf(tmp, ERx(" FROM %s.%s"), RMtbl->dlm_table_owner,
			RMtbl->dlm_table_name);
		STcat(select_stmt, tmp);
		if (shadow_rows)
		{
			STprintf(tmp, ERx(" t WHERE NOT EXISTS (SELECT * FROM \
	SESSION.%s s WHERE %s"),
				tmp_table, exists_where_clause);
			STcat(select_stmt, tmp);
		}
		STcat(copy_stmt,
			ERx("tab, cdds_no = c0tab, dd_priority = c0nl) FROM '"));
		STcat(copy_stmt, shadow_filename);
		STcat(copy_stmt, ERx("'"));
		sqlda->sqln = column_count;
/* # line 620 "crrepkey.sc" */	/* prepare */
  {
    IIsqInit((char *)0);
    IIsqPrepare(0,(char *)"s1",(char *)0,0,select_stmt);
  }
/* # line 621 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 622 "crrepkey.sc" */	/* host code */
		if (err)
		{
			IIUGerr(E_RM012C_Err_prep_stmt, UG_ERR_ERROR, 0);
/* # line 625 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 626 "crrepkey.sc" */	/* host code */
			free_sqlda(sqlda, column_count);
			return (E_RM012C_Err_prep_stmt);
		}
/* # line 629 "crrepkey.sc" */	/* describe */
  {
    IIsqInit((char *)0);
    IIsqDescribe(0,(char *)"s1",sqlda,0);
  }
/* # line 630 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 631 "crrepkey.sc" */	/* host code */
		if (err)
		{
			IIUGerr(E_RM012D_Err_descr_stmt, UG_ERR_ERROR, 0);
/* # line 634 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 635 "crrepkey.sc" */	/* host code */
			free_sqlda(sqlda, column_count);
			return (E_RM012D_Err_descr_stmt);
		}
/* # line 639 "crrepkey.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"c1",7206,3707);
    IIwritio(0,(short *)0,1,32,0,(char *)"s1");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"c1",7206,3707);
  }
/* # line 640 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 641 "crrepkey.sc" */	/* host code */
		if (err)
		{
			IIUGerr(E_RM012E_Err_open_base_curs, UG_ERR_ERROR, 0);
/* # line 644 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 645 "crrepkey.sc" */	/* host code */
			free_sqlda(sqlda, column_count);
			return (E_RM012E_Err_open_base_curs);
		}
/* # line 649 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&session1,11);
  }
/* # line 650 "crrepkey.sc" */	/* host code */
		session2 = session1 + 10;
/* # line 651 "crrepkey.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(session2);
    IIsqUser(dba);
    IIsqConnect(0,dbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 652 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 653 "crrepkey.sc" */	/* host code */
		if (err)
		{
			IIUGerr(E_RM0084_Error_connecting, UG_ERR_ERROR, 1, dbname);
			free_sqlda(sqlda, column_count);
/* # line 657 "crrepkey.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&session1);
  }
/* # line 658 "crrepkey.sc" */	/* host code */
			return (E_RM0084_Error_connecting);
		}
/* # line 660 "crrepkey.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session with ON_ERROR=rollback TRANSACTION");
    IIsyncup((char *)0,0);
  }
/* # line 662 "crrepkey.sc" */	/* host code */
		approx_rows = (i4)(((base_rows - shadow_rows) / 100) + 0.5) * 100;
		for (done = 0; !done && !err;)
		{
			bool	found = FALSE;
/* # line 667 "crrepkey.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&session2);
  }
/* # line 668 "crrepkey.sc" */	/* host code */
			/* get transaction_id, trans_time */
/* # line 669 "crrepkey.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"crrepkey1",7207,15228);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"crrepkey1",7207,15228);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select INT4(right(DBMSINFO('db_tran_id'), 16)), DATE('now')");
        IIexDefine(0,(char *)"crrepkey1",7207,15228);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&transaction_id);
      IIgetdomio((short *)0,1,32,25,timestamp);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 672 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 673 "crrepkey.sc" */	/* host code */
			if (err)
			{
				IIUGerr(E_RM012F_Err_get_trans_id, UG_ERR_ERROR, 0);
/* # line 676 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 677 "crrepkey.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IIsqDisconnect();
  }
/* # line 678 "crrepkey.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&session1);
  }
/* # line 679 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 680 "crrepkey.sc" */	/* host code */
				free_sqlda(sqlda, column_count);
				return (E_RM012F_Err_get_trans_id);
			}
			STtrmwhite(timestamp);
			for (i = 0; !err && i < trans_max && !done; i++)
			{
/* # line 686 "crrepkey.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&session1);
  }
/* # line 687 "crrepkey.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"c1",7206,3707,-1,-1) != 0) {
      IIcsDaGet(0,sqlda);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 688 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 689 "crrepkey.sc" */	/* host code */
				if (err)
				{
					IIUGerr(E_RM0130_Err_fetch_base, UG_ERR_ERROR,
						0);
/* # line 693 "crrepkey.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&session2);
  }
/* # line 694 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 695 "crrepkey.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IIsqDisconnect();
  }
/* # line 696 "crrepkey.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&session1);
  }
/* # line 697 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 698 "crrepkey.sc" */	/* host code */
					free_sqlda(sqlda, column_count);
					return (E_RM0130_Err_fetch_base);
				}
				if (!done)
				{
					short	data_length;
					found = TRUE;
					/* build values part of insert string */
					STprintf(file_data, ERx("%d\t%d\t%d\t1\t%s"),
						sourcedb, transaction_id, i, timestamp);
					for (j = 0; j < column_count; j++)
					{
						if (j == 0)
							STcopy(ERx("r."), wc);
						else
							STcat(wc, ERx(" AND r."));
						data_length = sqlda->sqlvar[j].sqlname.sqlnamel;
						sqlda->sqlvar[j].sqlname.sqlnamec[data_length] = 0;
						if (STequal(column_datatype[j],
							ERx("integer")))
						{
							if (column_length[j] == 1)
								STprintf(tmp, ERx("%d"),
									*(char *)sqlda->sqlvar[j].sqldata);
							else if (column_length[j] == 2)
								STprintf(tmp, ERx("%d"),
									*(short *)sqlda->sqlvar[j].sqldata);
							else if (column_length[j] == 4)
								STprintf(tmp, ERx("%d"),
									*(i4 *)sqlda->sqlvar[j].sqldata);
						}
						else if (STequal(column_datatype[j],
							ERx("float")) ||
							STequal(column_datatype[j],
							ERx("money")))
						{
							if (column_length[j] == 4)
								STprintf(tmp,
									ERx("%.13g"),
									*(float *)sqlda->sqlvar[j].sqldata,
									(char)RMadf_cb->adf_decimal.db_decimal);
							else if (column_length[j] == 8)
								STprintf(tmp,
									ERx("%.13g"),
									*(double *)sqlda->sqlvar[j].sqldata,
									(char)RMadf_cb->adf_decimal.db_decimal);
						}
						else if (STequal(column_datatype[j],
							ERx("varchar")) ||
							STequal(column_datatype[j],
							ERx("text")) ||
							STequal(column_datatype[j],
							ERx("byte varying")))
						{
							data_length = *(short *)sqlda->sqlvar[j].sqldata;
							sqlda->sqlvar[j].sqldata[data_length
								+ sizeof(short)] = 0;
							STprintf(tmp, ERx("%s"),
								&sqlda->sqlvar[j].sqldata[sizeof(short)]);
						}
						else if (STequal(column_datatype[j], 
												ERx("nvarchar")) || 
												STequal(column_datatype[j], 
												ERx("nchar")))
											{
						DB_DATA_VALUE unidata;
						DB_DATA_VALUE utf8data;
						STATUS stat = OK;
						if (STequal(column_datatype[j], ERx("nvarchar")))
						{
							data_length = *(short *)sqlda->sqlvar[j].sqldata;
							nvch_val[nvch_cnt].len = 
										*(short *)sqlda->sqlvar[j].sqldata;
							wcsncpy (nvch_val[nvch_cnt].text, 
										(wchar_t *)sqlda->sqlvar[j].sqldata,
										*(short *)sqlda->sqlvar[j].sqldata);
							(nvch_val[nvch_cnt]).text[nvch_val[nvch_cnt].len] 
								= L'\0';
							nvch_cnt++;
						}
						else 
						{
							data_length = sqlda->sqlvar[j].sqllen;
							wcsncpy (nch_val[nch_cnt], 
										(wchar_t *)sqlda->sqlvar[j].sqldata,
										sqlda->sqlvar[j].sqllen);
							nch_val[nch_cnt][sqlda->sqlvar[j].sqllen] = L'\0';
							nch_cnt++;
						}
						/*
						** convert the nvarchar data to utf8 in order to 
						** write to the file.
						*/
						unidata.db_data = sqlda->sqlvar[j].sqldata;
						unidata.db_length = data_length;
						unidata.db_datatype = sqlda->sqlvar[j].sqltype;
						unidata.db_prec = 0;
						/* Allocte space for results UTF8 data can 
						** be 4 times the original unicode data */
						utf8data.db_length = data_length * 4; 
						utf8data.db_data = (char *)MEreqmem(0, 
							utf8data.db_length + sizeof(short), TRUE, NULL);
						utf8data.db_datatype = DB_VCH_TYPE;
						unidata.db_prec = 0;
						stat = adu_nvchr_toutf8 (RMadf_cb, &unidata, 
											&utf8data);
						/* copy UTF8 data*/
						if (STequal(column_datatype[j], ERx("nvarchar")))
							data_length = *(short *)utf8data.db_data; 
						else 
							data_length = utf8data.db_length; 
						MEcopy (utf8data.db_data + sizeof(short),
										data_length, tmp);
						tmp[data_length] = '\0';
						if (utf8data.db_data)
							MEfree (utf8data.db_data);
						}
						else
						{
							STprintf(tmp, ERx("%s"),
								sqlda->sqlvar[j].sqldata);
						}
						STcat(file_data, ERx("	"));
						if (STequal(column_datatype[j],
							ERx("varchar")) ||
							STequal(column_datatype[j],
							ERx("text")) ||
							STequal(column_datatype[j],
							ERx("nvarchar")) ||
							STequal(column_datatype[j],
							ERx("nchar")) ||
							STequal(column_datatype[j],
							ERx("byte varying")))
						{
							char	len_spec[6];
							STprintf(len_spec, "%5d",
								data_length);
							STcat(file_data, len_spec);
						}
						STcat(file_data, tmp);
						STcat(wc, column_name[j]);
						STcat(wc, ERx(" = "));
						if (STequal(column_datatype[j],
							ERx("varchar")) ||
							STequal(column_datatype[j],
							ERx("text")) ||
							STequal(column_datatype[j],
							ERx("date")) ||
							STequal(column_datatype[j],
							ERx("ingresdate")) ||
							STequal(column_datatype[j],
							ERx("ansidate")) ||
							STequal(column_datatype[j],
							ERx("time without time zone")) ||
							STequal(column_datatype[j],
							ERx("timestamp without time zone")) ||
							STequal(column_datatype[j],
							ERx("time with time zone")) ||
							STequal(column_datatype[j],
							ERx("timestamp with time zone")) ||
							STequal(column_datatype[j],
							ERx("time with local time zone")) ||
							STequal(column_datatype[j],
							ERx("timestamp with local time zone")) ||
							STequal(column_datatype[j],
							ERx("time")) ||
							STequal(column_datatype[j],
							ERx("timestamp")) ||
							STequal(column_datatype[j],
							ERx("interval year to month")) ||
							STequal(column_datatype[j],
							ERx("interval day to second")) ||
							STequal(column_datatype[j],
							ERx("c")) ||
							STequal(column_datatype[j],
							ERx("byte")) ||
							STequal(column_datatype[j],
							ERx("byte varying")) ||
							STequal(column_datatype[j],
							ERx("char")))
						{
							STcat(wc, ERx("'"));
							STcat(wc, tmp);
							STcat(wc, ERx("'"));
						}
						else if (STequal(column_datatype[j],
												ERx("nvarchar")))
											{
												char    vdata[15];
												i4 i = nvch_cnt;
												STprintf(vdata,":nvch_val[%d]", nvch_cnt);
												STcat(wc, vdata);
											}
											else if (STequal(column_datatype[j],
												ERx("nchar")))
											{
												char    vdata[15];
													i4 i = nch_cnt;
													STprintf(vdata,":nch_val[%d]", nch_cnt);
													STcat(wc, vdata);
											}
						else
						{
							STcat(wc, tmp);
						}
					}
					err = get_cdds_no(wc, &cdds_no);
					if (err)
					{
/* # line 909 "crrepkey.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&session2);
  }
/* # line 910 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 911 "crrepkey.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IIsqDisconnect();
  }
/* # line 912 "crrepkey.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&session1);
  }
/* # line 913 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 914 "crrepkey.sc" */	/* host code */
						free_sqlda(sqlda, column_count);
						return (err);
					}
					err = get_dd_priority(wc, &dd_priority);
					if (err)
					{
/* # line 920 "crrepkey.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&session2);
  }
/* # line 921 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 922 "crrepkey.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IIsqDisconnect();
  }
/* # line 923 "crrepkey.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&session1);
  }
/* # line 924 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 925 "crrepkey.sc" */	/* host code */
						free_sqlda(sqlda, column_count);
						return (err);
					}
					STprintf(tmp, ERx("	%d"), cdds_no);
					STcat(file_data, tmp);
					STprintf(tmp, ERx("	%d"), dd_priority);
					STcat(file_data, tmp);
					SIfprintf(shadow_file, ERx("%s\n"), file_data);
					if (queue_flag)
						SIfprintf(queue_file,
							ERx("%d	%d	%d	%d	%d	%d	%d	%s	%d	%d	%d\n"),
							sourcedb, transaction_id, i,
							RMtbl->table_no, 0, 0, 0,
							timestamp, 1, cdds_no,
							dd_priority);
					++rows_processed;
				}
			}
			if (found)
			{
				SIclose(shadow_file);
/* # line 946 "crrepkey.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&session2);
  }
/* # line 947 "crrepkey.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(copy_stmt);
    IIsyncup((char *)0,0);
  }
/* # line 948 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 949 "crrepkey.sc" */	/* host code */
				if (err)
				{
					IIUGerr(E_RM0131_Err_copy_from, UG_ERR_ERROR,
						2, RMtbl->sha_name, shadow_filename);
/* # line 953 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 954 "crrepkey.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IIsqDisconnect();
  }
/* # line 955 "crrepkey.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&session1);
  }
/* # line 956 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 957 "crrepkey.sc" */	/* host code */
					free_sqlda(sqlda, column_count);
					return (E_RM0131_Err_copy_from);
				}
				if (SIfopen(&sh_loc, ERx("w"), SI_TXT, SI_MAX_TXT_REC,
					&shadow_file) != OK)
				{
					IIUGerr(E_RM005E_Error_open_file, UG_ERR_ERROR,
						1, shadow_filename);
/* # line 965 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 966 "crrepkey.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IIsqDisconnect();
  }
/* # line 967 "crrepkey.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&session1);
  }
/* # line 968 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 969 "crrepkey.sc" */	/* host code */
					free_sqlda(sqlda, column_count);
					return (E_RM005E_Error_open_file);
				}
				if (queue_flag)
				{
					SIclose(queue_file);
/* # line 975 "crrepkey.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(qcopy_stmt);
    IIsyncup((char *)0,0);
  }
/* # line 976 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 977 "crrepkey.sc" */	/* host code */
					if (err)
					{
						IIUGerr(E_RM0131_Err_copy_from,
							UG_ERR_ERROR, 2,
							ERx("dd_input_queue"),
							queue_filename);
/* # line 983 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 984 "crrepkey.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IIsqDisconnect();
  }
/* # line 985 "crrepkey.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&session1);
  }
/* # line 986 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 987 "crrepkey.sc" */	/* host code */
						free_sqlda(sqlda, column_count);
						return (E_RM0131_Err_copy_from);
					}
					if (SIfopen(&iq_loc, ERx("w"), SI_TXT,
						SI_MAX_TXT_REC, &queue_file) != OK)
					{
						IIUGerr(E_RM005E_Error_open_file,
							UG_ERR_ERROR, 1,
							queue_filename);
/* # line 996 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 997 "crrepkey.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IIsqDisconnect();
  }
/* # line 998 "crrepkey.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&session1);
  }
/* # line 999 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 1000 "crrepkey.sc" */	/* host code */
						free_sqlda(sqlda, column_count);
						return (E_RM005E_Error_open_file);
					}
				}
				IIUGmsg(ERget(F_RM00DE_Rows_completed), FALSE, 2,
					&rows_processed, &approx_rows);
/* # line 1007 "crrepkey.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 1008 "crrepkey.sc" */	/* host code */
			}
		}
		SIclose(shadow_file);
		LOdelete(&sh_loc);
		if (queue_flag)
		{
			SIclose(queue_file);
			LOdelete(&iq_loc);
		}
/* # line 1017 "crrepkey.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&session2);
  }
/* # line 1018 "crrepkey.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(session2);
    IIsqDisconnect();
  }
/* # line 1019 "crrepkey.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&session1);
  }
/* # line 1020 "crrepkey.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c1",7206,3707);
  }
/* # line 1021 "crrepkey.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 1022 "crrepkey.sc" */	/* host code */
		free_sqlda(sqlda, column_count);
		if (!shadow_rows)
		{
			err = RMmodify_shadow(RMtbl, TRUE);
			if (err)
			{
/* # line 1029 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 1030 "crrepkey.sc" */	/* host code */
				IIUGerr(E_RM0129_Err_modifying, UG_ERR_ERROR, 1,
					RMtbl->sha_name);
				return (E_RM0129_Err_modifying);
			}
/* # line 1034 "crrepkey.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 1035 "crrepkey.sc" */	/* host code */
			STprintf(stmt, ERx("SET JOURNALING ON %s"), RMtbl->sha_name);
/* # line 1036 "crrepkey.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
  }
/* # line 1037 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 1038 "crrepkey.sc" */	/* host code */
			if (err)
			{
				IIUGerr(E_RM0134_Err_set_journal, UG_ERR_ERROR, 2,
					RMtbl->table_owner, RMtbl->sha_name);
/* # line 1042 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 1043 "crrepkey.sc" */	/* host code */
				return (E_RM0134_Err_set_journal);
			}
/* # line 1045 "crrepkey.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 1046 "crrepkey.sc" */	/* host code */
		}
		if (queue_flag && (input_rows < MODIFY_TABLE_LIMIT))
		{
			if (RMmodify_input_queue() != OK)
			{
/* # line 1051 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 1052 "crrepkey.sc" */	/* host code */
				IIUGerr(E_RM0129_Err_modifying, UG_ERR_ERROR, 1,
					ERx("dd_input_queue"));
				return (E_RM0129_Err_modifying);
			}
/* # line 1056 "crrepkey.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 1057 "crrepkey.sc" */	/* host code */
		}
		if (shadow_rows)
		{
			STprintf(stmt, ERx("DROP TABLE SESSION.%s"), tmp_table);
/* # line 1062 "crrepkey.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
  }
/* # line 1063 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 1064 "crrepkey.sc" */	/* host code */
			if (err)
			{
/* # line 1066 "crrepkey.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 1067 "crrepkey.sc" */	/* host code */
				IIUGerr(E_RM0128_Err_dropping, UG_ERR_ERROR, 1,
					tmp_table);
				return (E_RM0128_Err_dropping);
			}
/* # line 1071 "crrepkey.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 1072 "crrepkey.sc" */	/* host code */
		}
		break;
	}
	if (stat)
	{
		IIUGerr(E_RM0135_Err_alloc_ucode_key, UG_ERR_ERROR, 1,
					tmp_table);
		return (E_RM0135_Err_alloc_ucode_key);
	}
	/* clean up the unicode values if assigned */
	for (i=0; i<DB_MAX_COLS; i++)
	{
	  if (nch_val[i])
	    MEfree (nch_val[i]);
	}
	if (nch_val)
	  MEfree (nch_val);
	if (nvch_val)
	  MEfree (nvch_val);
	return (OK);
}
/*{
** Name:	free_sqlda - free SQLDA
**
** Description:
**	free allocated space for the sqlda
**
** Inputs:
**	sqlda		- the SQLDA
**	column_count	- the number of columns allocated
**
** Outputs:
**	none
**
** Returns:
**	none
*/
static void
free_sqlda(
IISQLDA	*sqlda,
i4	column_count)
{
	i4 i;
	for (i = 0; i < column_count; i++)
		MEfree(sqlda->sqlvar[i].sqldata);
}
/*{
** Name:	get_cdds_no - get CDDS number
**
** Description:
**	Lookup a row's cdds_no if there is a cdds_lookup_table.  Else return
**	the default for the table.
**
** Inputs:
**	wc		- where_clause
**
** Outputs:
**	cdds_no		- CDDS number
**
** Returns:
**	?
*/
static STATUS
get_cdds_no(
char	*wc,
i2	*cdds_no)
{
  i2 cdds_num;
  STATUS err = 0;
  char select_string[MAX_SIZE];
  char col_name[DB_MAXNAME+1];
  i4 column_number = 0;
  i4 row_count;
	char	expand_string[512];
	*cdds_no = RMtbl->cdds_no;
	*expand_string = EOS;
	if (*RMtbl->cdds_lookup_table != EOS)
	{
		/* build where clause */
/* # line 1164 "crrepkey.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(column_name)from iicolumns where table_name=");
    IIputdomio((short *)0,1,32,0,RMtbl->cdds_lookup_table);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and table_owner=DBMSINFO('dba')and column_name!='cdds_no' and column_\
name!='dd_priority'");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,col_name);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 1172 "crrepkey.sc" */	/* host code */
			if (column_number)
				STcat(expand_string, ERx(" AND "));
			STcat(expand_string, ERx("r."));
			STcat(expand_string, RPedit_name(EDNM_DELIMIT,
				col_name, NULL));
			STcat(expand_string, ERx(" = "));
			STcat(expand_string, ERx("t."));
			STcat(expand_string, RPedit_name(EDNM_DELIMIT,
				col_name, NULL));
			column_number++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 1183 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 1184 "crrepkey.sc" */	/* host code */
		if (err)
		{
			IIUGerr(E_RM0132_Err_rtrv_col_name, UG_ERR_ERROR, 0);
			return (E_RM0132_Err_rtrv_col_name);
		}
		STprintf(select_string, ERx("SELECT t.cdds_no FROM %s t, \
%s.%s r WHERE %s AND %s"),
			RMtbl->cdds_lookup_table, RMtbl->dlm_table_owner,
			RMtbl->dlm_table_name, expand_string, wc);
/* # line 1193 "crrepkey.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqMods(3);
    IIsqMods(1);
    IIsqExImmed(select_string);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&cdds_num);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1194 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
    IILQisInqSqlio((short *)0,1,30,4,&row_count,8);
  }
/* # line 1195 "crrepkey.sc" */	/* host code */
		if (err)
		{
			IIUGerr(E_RM0133_Err_lookup_value, UG_ERR_ERROR, 2,
				ERx("cdds_no"), RMtbl->cdds_lookup_table);
			return (E_RM0133_Err_lookup_value);
		}
		if (row_count)
			*cdds_no = cdds_num;
	}
	return (OK);
}
/*{
** Name:	get_dd_priority - get priority
**
** Description:
**	get a rows' dd_priority if a priority lookup table is defined.
**
** Inputs:
**	wc - where clause of the row
**
** Outputs:
**	dd_priority - priority of the row
**
** Returns:
**	non-zero - error number, if error occurred
**	0	- if no error encountered
*/
static STATUS
get_dd_priority(
char	*wc,
i2	*dd_priority)
{
  i2 dd_prio;
  STATUS err = 0;
  char select_string[MAX_SIZE];
  char col_name[DB_MAXNAME+1];
  i4 column_number = 0;
  i4 row_count;
	char	expand_string[512];
	*dd_priority = 0;
	*expand_string = EOS;
	if (*RMtbl->prio_lookup_table != EOS)
	{
		/* build where clause */
/* # line 1245 "crrepkey.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(column_name)from iicolumns where table_name=");
    IIputdomio((short *)0,1,32,0,RMtbl->prio_lookup_table);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and table_owner=DBMSINFO('dba')and column_name!='cdds_no' and column_\
name!='dd_priority'");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,col_name);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 1253 "crrepkey.sc" */	/* host code */
			if (column_number)
				STcat(expand_string, ERx(" AND "));
			STcat(expand_string, ERx("r."));
			STcat(expand_string, RPedit_name(EDNM_DELIMIT,
				col_name, NULL));
			STcat(expand_string, ERx(" = "));
			STcat(expand_string, ERx("t."));
			STcat(expand_string, RPedit_name(EDNM_DELIMIT,
				col_name, NULL));
			column_number++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
/* # line 1264 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 1265 "crrepkey.sc" */	/* host code */
		if (err)
		{
			IIUGerr(E_RM0132_Err_rtrv_col_name, UG_ERR_ERROR, 0);
			return (E_RM0132_Err_rtrv_col_name);
		}
		STprintf(select_string, ERx("SELECT r.dd_priority FROM %s r, \
%s.%s t WHERE %s AND %s"),
			RMtbl->prio_lookup_table, RMtbl->dlm_table_owner,
			RMtbl->dlm_table_name, expand_string, wc);
/* # line 1274 "crrepkey.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqMods(3);
    IIsqMods(1);
    IIsqExImmed(select_string);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&dd_prio);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1275 "crrepkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
    IILQisInqSqlio((short *)0,1,30,4,&row_count,8);
  }
/* # line 1276 "crrepkey.sc" */	/* host code */
		if (err)
		{
			IIUGerr(E_RM0133_Err_lookup_value, UG_ERR_ERROR, 2,
				ERx("dd_priority"), RMtbl->prio_lookup_table);
			return (E_RM0133_Err_lookup_value);
		}
		if (row_count)
			*dd_priority = dd_prio;
	}
	return (OK);
}
