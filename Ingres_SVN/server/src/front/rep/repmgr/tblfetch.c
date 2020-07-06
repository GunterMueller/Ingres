# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <st.h>
# include <me.h>
# include <er.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <ug.h>
# include <rpu.h>
# include <targtype.h>
# include <tblobjs.h>
# include "errm.h"
# include <tbldef.h>
/*
** Name:	tblfetch.sc - fetch table information
**
** Description:
**	Defines
**		RMtbl_fetch	- fetch table information
**
** History:
**	09-jan-97 (joea)
**		Created based on tblfetch.sc in replicator library.
**	14-jan-97 (joea)
**		Use defined constants for target types.
**	18-sep-97 (joea)
**		Retrieve base table page size.
**	10-oct-97 (joea) bug 83765
**		If a table exists but the CDDS/database relationship has not
**		been specified, warn the user and mark the table as URO.
**		Initialize RMtbl to NULL and only set it to point to the cached
**		structure if there are no errors.
**	04-may-98 (joea)
**		When checking for table existence, allow zero rows to be
**		treated as no error.
**	27-aug-98 (abbjo03) bug 92717
**		Since SECURITY_LABEL is reported as CHAR in iicolumns.
**		column_datatype, retrieve column_internal_datatype instead.
**	29-sep-98 (abbjo03)
**		Replace ddba_messageit with IIUGerr.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      29-Nov-1999 (hanch04)
**          First stage of updating ME calls to use OS memory management.
**          Make sure me.h is included.  Use SIZE_TYPE for lengths.
[@history_template@]...
**/
  static TBLDEF tbl;
GLOBALDEF TBLDEF	*RMtbl = NULL;
/*{
** Name:	RMtbl_fetch - fetch table information
**
** Description:
**	Retrieve information on a table and construct the names of all related
**	objects
**
** Inputs:
**	table_no	- table number
**	force		- force the information to be refreshed
**
** Outputs:
**	none
**
** Returns:
**	OK
**	E_RM00FA_Tbl_not_found
**	E_RM00FD_No_regist_cols
**	E_RM00FE_Err_alloc_col
**	E_RM00FF_Err_col_count
**
** Side effects:
**	The global RMtbl is set to point to the 'tbl' cache.
*/
STATUS
RMtbl_fetch(
i4	table_no,
bool	force)
{
	DBEC_ERR_INFO	errinfo;
	STATUS	err;
  COLDEF *col_p;
  short null_ind = 0;
  char stmt[2048];
  i4 cdds_tbl_exists;
	if (tbl.table_no == table_no && !force)
	{
		RMtbl = &tbl;
		return (OK);
	}
	if (tbl.ncols > 0 && tbl.col_p != NULL)
	{
		MEfree((char *)tbl.col_p);
		tbl.ncols = 0;
	}
	tbl.table_no = table_no;
	/* Populate tbldef based on table_no */
/* # line 118 "tblfetch.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"tblfetch1",7199,18304);
      IIputdomio((short *)0,1,30,4,&tbl.table_no);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"tblfetch1",7199,18304);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(table_name), TRIM(table_owner), cdds_no, columns_register\
ed, supp_objs_created, rules_created, TRIM(cdds_lookup_table), TRIM(pr\
io_lookup_table), TRIM(index_used)from dd_regist_tables where table_no\
=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&tbl.table_no);
        IIexDefine(0,(char *)"tblfetch1",7199,18304);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,tbl.table_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,tbl.table_owner);
      IIgetdomio((short *)0,1,30,2,&tbl.cdds_no);
      IIgetdomio((short *)0,1,32,25,tbl.columns_registered);
      IIgetdomio((short *)0,1,32,25,tbl.supp_objs_created);
      IIgetdomio((short *)0,1,32,25,tbl.rules_created);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,tbl.cdds_lookup_table);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,tbl.prio_lookup_table);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,tbl.index_used);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 130 "tblfetch.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
	{
		if (errinfo.errorno)
		{
			return (errinfo.errorno);
		}
		else	/* If table is not found, then return an error */
		{
			IIUGerr(E_RM00FA_Tbl_not_found, UG_ERR_ERROR, 1,
				&tbl.table_no);
			return (E_RM00FA_Tbl_not_found);
		}
	}
	RPedit_name(EDNM_DELIMIT, tbl.table_name, tbl.dlm_table_name);
	RPedit_name(EDNM_DELIMIT, tbl.table_owner, tbl.dlm_table_owner);
	STtrmwhite(tbl.columns_registered);
	STtrmwhite(tbl.supp_objs_created);
	STtrmwhite(tbl.rules_created);
/* # line 150 "tblfetch.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select table_pagesize from iitables where table_name=");
    IIputdomio((short *)0,1,32,0,tbl.table_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
    IIputdomio((short *)0,1,32,0,tbl.table_owner);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tbl.page_size);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 155 "tblfetch.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		return (errinfo.errorno);
	/*
	** If the base table doesn't exist, we're presumably being called from
	** MoveConfig (which still moves everything in sight).  Set target_type
	** to URO and return.
	*/
	if (!errinfo.rowcount)
	{
		tbl.target_type = TARG_UNPROT_READ;
		tbl.ncols = 0;
		tbl.col_p = col_p = NULL;
		RMtbl = &tbl;
		return (OK);
	}
	cdds_tbl_exists = FALSE;
	if (*tbl.cdds_lookup_table != EOS)
	{
/* # line 175 "tblfetch.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iitables where table_name=");
    IIputdomio((short *)0,1,32,0,tbl.cdds_lookup_table);
    IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
    IIputdomio((short *)0,1,32,0,tbl.table_owner);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cdds_tbl_exists);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 180 "tblfetch.sc" */	/* host code */
		if (RPdb_error_check(0, &errinfo) != OK)
			return (errinfo.errorno);
	}
	if (cdds_tbl_exists)
	{
		STprintf(stmt,
			ERx("SELECT MIN(target_type) FROM dd_db_cdds \
c, dd_databases d, %s l WHERE c.cdds_no = l.cdds_no AND c.database_no = \
d.database_no AND d.local_db = 1"),
			tbl.cdds_lookup_table);
/* # line 191 "tblfetch.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqMods(3);
    IIsqMods(1);
    IIsqExImmed(stmt);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio(&null_ind,1,30,2,&tbl.target_type);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 193 "tblfetch.sc" */	/* host code */
	}
	else
	{
/* # line 196 "tblfetch.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select c.target_type from dd_db_cdds c, dd_databases d where c.cdds_n\
o=");
    IIputdomio((short *)0,1,30,2,&tbl.cdds_no);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and c.database_no=d.database_no and d.local_db=1");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&tbl.target_type);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 201 "tblfetch.sc" */	/* host code */
	}
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
		return (errinfo.errorno);
	}
	else if (errinfo.rowcount == 0 || null_ind == -1)
	{
		/*
		** The table happens to exist in this database but the
		** dd_db_cdds catalog or lookup table don't expect it to
		** be here.  Warn the user and specify the CDDS as URO
		** so we won't create support objects.
		*/
		if (cdds_tbl_exists)
		{
			IIUGerr(E_RM00FB_No_valid_cdds, UG_ERR_ERROR, 2,
				tbl.table_owner, tbl.table_name);
		}
		else
		{
			i4	cdds_no = (i4)tbl.cdds_no;
			IIUGerr(E_RM00FC_Miss_cdds_defn, UG_ERR_ERROR, 1,
				&cdds_no);
		}
		tbl.target_type = TARG_UNPROT_READ;
		RMtbl = &tbl;
		return (OK);
	}
/* # line 232 "tblfetch.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_regist_columns where table_no=");
    IIputdomio((short *)0,1,30,4,&tbl.table_no);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tbl.ncols);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 235 "tblfetch.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
		return (errinfo.errorno);
	if (tbl.ncols == 0)
	{
		IIUGerr(E_RM00FD_No_regist_cols, UG_ERR_ERROR, 2,
			tbl.table_owner, tbl.table_name);
		return (E_RM00FD_No_regist_cols);
	}
	if (err = RPtblobj_name(tbl.table_name, tbl.table_no, TBLOBJ_ARC_TBL,
			tbl.arc_name))
		return (err);
	if (err = RPtblobj_name(tbl.table_name, tbl.table_no, TBLOBJ_SHA_TBL,
			tbl.sha_name))
		return (err);
	if (err = RPtblobj_name(tbl.table_name, tbl.table_no,
			TBLOBJ_REM_DEL_PROC, tbl.rem_del_proc))
		return (err);
	if (err = RPtblobj_name(tbl.table_name, tbl.table_no,
			TBLOBJ_REM_UPD_PROC, tbl.rem_upd_proc))
		return (err);
	if (err = RPtblobj_name(tbl.table_name, tbl.table_no,
			TBLOBJ_REM_INS_PROC, tbl.rem_ins_proc))
		return (err);
	col_p = (COLDEF *)MEreqmem(0, (u_i4)tbl.ncols * sizeof(COLDEF),
		TRUE, (STATUS *)NULL);
	if (col_p == NULL)
	{
		IIUGerr(E_RM00FE_Err_alloc_col, UG_ERR_ERROR, 0);
		return (E_RM00FE_Err_alloc_col);
	}
	tbl.col_p = col_p;
/* # line 275 "tblfetch.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"tblfetch2",7200,16130);
      IIputdomio((short *)0,1,32,0,tbl.table_name);
      IIputdomio((short *)0,1,32,0,tbl.table_owner);
      IIputdomio((short *)0,1,30,4,&tbl.table_no);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"tblfetch2",7200,16130);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(c.column_name), LOWERCASE(TRIM(c.column_internal_datatype\
)), c.column_length, c.column_scale, d.column_sequence, d.key_sequence\
, c.column_nulls, c.column_defaults from iicolumns c, dd_regist_column\
s d where c.table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,tbl.table_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and c.table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,tbl.table_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and c.column_name=d.column_name and d.table_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&tbl.table_no);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by d.column_sequen\
ce");
        IIexDefine(0,(char *)"tblfetch2",7200,16130);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,col_p->column_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,col_p->column_datatype);
      IIgetdomio((short *)0,1,30,4,&col_p->column_length);
      IIgetdomio((short *)0,1,30,4,&col_p->column_scale);
      IIgetdomio((short *)0,1,30,4,&col_p->column_sequence);
      IIgetdomio((short *)0,1,30,4,&col_p->key_sequence);
      IIgetdomio((short *)0,1,32,1,col_p->column_nulls);
      IIgetdomio((short *)0,1,32,1,col_p->column_defaults);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 292 "tblfetch.sc" */	/* host code */
		RPedit_name(EDNM_DELIMIT, col_p->column_name,
			col_p->dlm_column_name);
		col_p++;
		if ((col_p - tbl.col_p) >= tbl.ncols)
/* # line 296 "tblfetch.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 298 "tblfetch.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo) != OK)
	{
		if (errinfo.errorno)
		{
			return (errinfo.errorno);
		}
		else
		{
			IIUGerr(E_RM00FF_Err_col_count, UG_ERR_ERROR, 2,
				tbl.table_owner, tbl.table_name);
			return (E_RM00FF_Err_col_count);
		}
	}
	RMtbl = &tbl;
	return (OK);
}
