# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <st.h>
# include <cm.h>
# include <er.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <ug.h>
# include <rpu.h>
# include "errm.h"
/*
** Name:	tblregst.sc - default table registration
**
** Description:
**	Defines
**		tbl_register	- default table registration
**		get_unique_index - get unique secondary index
**		check_for_null_keys	- check for null keys
**
** History:
**	16-dec-96 (joea)
**		Created based on tblrgstr.sc in replicator library.
**	18-may-98 (padni01) bug 89865
**		Use date_gmt instead of date to set columns_registered field 
**		of dd_regist_tables.
**	21-sep-98 (abbjo03)
**		Merge cknulkey.sc. Replace ddba_messageit with IIUGerr.
**	06-mar-2000 (abbjo03) bug 92674
**		Change check_for_null_keys not to include dd_regist_tables in
**		its query since the table to be registered hasn't yet been
**		inserted there.
**/
static STATUS get_unique_index(char *table_name, char *table_owner,
	char *index_name);
static STATUS check_for_null_keys(i4 tableno, char *table_owner,
	char *table_name);
STATUS db_config_changed(i2 db_no);
/*{
** Name:	tbl_register - default table registration
**
** Description:
**	Performs default registration for a specific table.
**
** Inputs:
**	table_no	- an existing table number
**	table_name	- table name
**	table_owner	- table owner
**	cdds_no		- CDDS number
**	unique_rule	- does table have unique index
**	table_indexes	- does table have secondary indexes
**
** Outputs:
**	table_no	- the newly assigned table number
**	columns_registered - date/time as of which registration occurred
**	index_used	- the primary or secondary index used for replication
**
** Returns:
**	?
*/
STATUS
tbl_register(
i4	*table_no,
char	*table_name,
char	*table_owner,
i2	cdds_no,
char	*unique_rule,
char	*table_indexes,
char	*columns_registered,
char	*index_used)
# if 0
  i4 *table_no;
  char *table_name;
  char *table_owner;
  i2 cdds_no;
  char *columns_registered;
  char *index_used;
# endif
{
	DBEC_ERR_INFO	errinfo;
	bool	table_registered = FALSE;
	STATUS	retval;
	/* Assure that table number is assigned and unique */
	if (*table_no == 0)
	{
/* # line 95 "tblregst.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_last_number set last_number=last_number +1 where column_nam\
e='table_no'");
    IIsyncup((char *)0,0);
  }
/* # line 98 "tblregst.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
		{
/* # line 100 "tblregst.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 101 "tblregst.sc" */	/* host code */
			IIUGerr(E_RM00E2_Err_asgn_tbl_no, UG_ERR_ERROR, 2,
				table_owner, table_name);
			return (E_RM00E2_Err_asgn_tbl_no);
		}
/* # line 106 "tblregst.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select last_number from dd_last_number where column_name='table_no'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,table_no);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 109 "tblregst.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
		{
/* # line 111 "tblregst.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 112 "tblregst.sc" */	/* host code */
			IIUGerr(E_RM00E2_Err_asgn_tbl_no, UG_ERR_ERROR, 2,
				table_owner, table_name);
			return (E_RM00E2_Err_asgn_tbl_no);
		}
	}
	else	/* Eliminate existing column registration */
	{
/* # line 120 "tblregst.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from dd_regist_columns where table_no=");
    IIputdomio((short *)0,1,30,4,table_no);
    IIsyncup((char *)0,0);
  }
/* # line 121 "tblregst.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
		{
			IIUGerr(E_RM004E_Error_del_regist_col,
				UG_ERR_ERROR, 0);
/* # line 125 "tblregst.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 126 "tblregst.sc" */	/* host code */
			return (E_RM004E_Error_del_regist_col);
		}
		table_registered = TRUE;
	}
	/* Do column registration */
/* # line 133 "tblregst.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"tblregst1",7197,248);
      IIputdomio((short *)0,1,30,4,table_no);
      IIputdomio((short *)0,1,32,0,table_name);
      IIputdomio((short *)0,1,32,0,table_owner);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"tblregst1",7197,248);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_regist_columns(table_no, column_name, column_sequence,\
 key_sequence)select");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,table_no);
        IIwritio(0,(short *)0,1,32,0,(char *)
", column_name, column_sequence, 0 from iicolumns where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,table_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,table_owner);
        IIexDefine(0,(char *)"tblregst1",7197,248);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 140 "tblregst.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo) != OK)
	{
/* # line 142 "tblregst.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 143 "tblregst.sc" */	/* host code */
		IIUGerr(E_RM00DE_Err_add_regist_cols, UG_ERR_ERROR, 2,
			table_owner, table_name);
		return (E_RM00DE_Err_add_regist_cols);
	}
	if (CMcmpcase(unique_rule, ERx("U")) == 0)  /* unique primary index */
	{
		STcopy(table_name, index_used);
/* # line 151 "tblregst.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"tblregst2",7198,185);
      IIputdomio((short *)0,1,32,0,table_name);
      IIputdomio((short *)0,1,32,0,table_owner);
      IIputdomio((short *)0,1,30,4,table_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"tblregst2",7198,185);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_columns r from iicolumns c set key_sequence=c.key_se\
quence where c.table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,table_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and c.table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,table_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and c.column_name=r.column_name and c.key_sequence>0 and r.table_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,table_no);
        IIexDefine(0,(char *)"tblregst2",7198,185);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 159 "tblregst.sc" */	/* host code */
		if (RPdb_error_check(0, &errinfo) != OK)
		{
/* # line 161 "tblregst.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 162 "tblregst.sc" */	/* host code */
			IIUGerr(E_RM00DF_Err_updt_regist_cols, UG_ERR_ERROR, 2,
				table_owner, table_name);
			return (E_RM00DF_Err_updt_regist_cols);
		}
	}
	else if (CMcmpcase(table_indexes, ERx("Y")))	/* no secondaries */
	{
/* # line 169 "tblregst.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 170 "tblregst.sc" */	/* host code */
		IIUGerr(E_RM00E0_No_unique_key, UG_ERR_ERROR, 2, table_owner,
			table_name);
		return (E_RM00E0_No_unique_key);
	}
	else
	{
		retval = get_unique_index(table_name, table_owner, index_used);
		switch (retval)
		{
		case 1:		/* unique secondary found */
/* # line 180 "tblregst.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"tblregst3",7199,229);
      IIputdomio((short *)0,1,30,4,table_no);
      IIputdomio((short *)0,1,32,0,index_used);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"tblregst3",7199,229);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_columns r from iiindexes i, iicolumns c set key_sequ\
ence=c.key_sequence where c.table_name=i.index_name and c.table_owner=\
i.index_owner and r.column_name=c.column_name and r.table_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,table_no);
        IIwritio(0,(short *)0,1,32,0,(char *)"and i.index_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,index_used);
        IIexDefine(0,(char *)"tblregst3",7199,229);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 188 "tblregst.sc" */	/* host code */
			if (RPdb_error_check(0, &errinfo) != OK)
			{
/* # line 190 "tblregst.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 191 "tblregst.sc" */	/* host code */
				IIUGerr(E_RM00DF_Err_updt_regist_cols,
					UG_ERR_ERROR, 2, table_owner,
					table_name);
				return (E_RM00DF_Err_updt_regist_cols);
			}
			break;
		case 0:		/* no unique secondary */
/* # line 199 "tblregst.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 200 "tblregst.sc" */	/* host code */
			IIUGerr(E_RM00E0_No_unique_key, UG_ERR_ERROR, 2,
				table_owner, table_name);
			return (E_RM00E0_No_unique_key);
		default:
/* # line 205 "tblregst.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 206 "tblregst.sc" */	/* host code */
			IIUGerr(E_RM00E1_Err_get_index, UG_ERR_ERROR, 2,
				table_owner, table_name);
			return (E_RM00E1_Err_get_index);
		}
	}
	/*
	** As a final precaution, make sure that none of the key columns are
	** nullable
	*/
	retval = check_for_null_keys(*table_no, table_owner, table_name);
	if (retval)	/* key columns are nullable or there was an error */
	{
/* # line 219 "tblregst.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 220 "tblregst.sc" */	/* host code */
		return (retval);
	}
	/* Insert newly registered table into dd_regist_tables */
/* # line 224 "tblregst.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select DATE_GMT('now')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,0,columns_registered);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 225 "tblregst.sc" */	/* host code */
	if (!table_registered)
	{
/* # line 227 "tblregst.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"tblregst4",7200,691);
      IIputdomio((short *)0,1,30,4,table_no);
      IIputdomio((short *)0,1,32,0,table_name);
      IIputdomio((short *)0,1,32,0,table_owner);
      IIputdomio((short *)0,1,32,0,columns_registered);
      IIputdomio((short *)0,1,32,0,index_used);
      IIputdomio((short *)0,1,30,2,&cdds_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"tblregst4",7200,691);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_regist_tables(table_no, table_name, table_owner, colum\
ns_registered, index_used, cdds_no)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,table_no);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,table_name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,table_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,columns_registered);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,index_used);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,2,&cdds_no);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"tblregst4",7200,691);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 232 "tblregst.sc" */	/* host code */
	}
	else
	{
/* # line 235 "tblregst.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_tables set columns_registered=");
    IIputdomio((short *)0,1,32,0,columns_registered);
    IIwritio(0,(short *)0,1,32,0,(char *)
", supp_objs_created='', index_used=");
    IIputdomio((short *)0,1,32,0,index_used);
    IIwritio(0,(short *)0,1,32,0,(char *)", cdds_no=");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIwritio(0,(short *)0,1,32,0,(char *)"where table_no=");
    IIputdomio((short *)0,1,30,4,table_no);
    IIsyncup((char *)0,0);
  }
/* # line 241 "tblregst.sc" */	/* host code */
	}
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
	{
		if (errinfo.errorno)
		{
/* # line 246 "tblregst.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 247 "tblregst.sc" */	/* host code */
			return (errinfo.errorno);
		}
		else
		{
/* # line 251 "tblregst.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 252 "tblregst.sc" */	/* host code */
			IIUGerr(E_RM00DF_Err_updt_regist_cols, UG_ERR_ERROR, 2,
				table_owner, table_name);
			return (E_RM00DF_Err_updt_regist_cols);
		}
	}
	if (db_config_changed(0) != OK)
	{
/* # line 259 "tblregst.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 260 "tblregst.sc" */	/* host code */
		return (FAIL);
	}
/* # line 263 "tblregst.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 265 "tblregst.sc" */	/* host code */
	return (OK);
}
/*
** Name:	get_unique_index - get unique secondary index
**
** Description:
**	returns 1 if unique secondary found 0 if no unique secondary found
**	-1 if error
**
** Inputs:
**	table_name	- which table
**	table_owner	- who owns it
**
** Outputs:
**	index_name	- first unique index found
**
** Returns:
**	?
*/
static STATUS
get_unique_index(
char	*table_name,
char	*table_owner,
char	*index_name)
# if 0
  char *table_name;
  char *table_owner;
  char *index_name;
# endif
{
	DBEC_ERR_INFO	errinfo;
  short null_ind = -1;
/* # line 304 "tblregst.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"tblregst5",7201,1443);
      IIputdomio((short *)0,1,32,0,table_name);
      IIputdomio((short *)0,1,32,0,table_owner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"tblregst5",7201,1443);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select min(index_name)from iiindexes where base_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,table_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and base_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,table_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)"and unique_rule='U'");
        IIexDefine(0,(char *)"tblregst5",7201,1443);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio(&null_ind,1,32,0,index_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 310 "tblregst.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
		if (errinfo.errorno)
			return (-1);
	if (null_ind == -1)	/* none found */
		return (0);
	else
		return (1);
}
/*{
** Name:	check_for_null_keys - check for null keys
**
** Description:
**	Checks whether the specified table allows null values in its key
**	columns.
**
** Inputs:
**	tableno		- table number
**	table_owner	- table owner
**	table_name	- table name
**
** Outputs:
**	none
**
** Returns:
**	OK, E_RM00DC_Err_ck_null_cols, E_RM00DD_Tbl_null_keys
*/
static STATUS
check_for_null_keys(
i4	tableno,
char	*table_owner,
char	*table_name)
# if 0
  i4 tableno;
  char *table_owner;
  char *table_name;
# endif
{
	DBEC_ERR_INFO	errinfo;
  i4 cnt;
/* # line 357 "tblregst.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_regist_columns c, iicolumns i where c.table_no\
=");
    IIputdomio((short *)0,1,30,4,&tableno);
    IIwritio(0,(short *)0,1,32,0,(char *)"and i.table_name=");
    IIputdomio((short *)0,1,32,0,table_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and i.table_owner=");
    IIputdomio((short *)0,1,32,0,table_owner);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and i.column_nulls='Y' and i.column_name=c.column_name and c.key_sequ\
ence>0");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 365 "tblregst.sc" */	/* host code */
	RPdb_error_check(0, &errinfo);
	if (errinfo.errorno)
	{
		IIUGerr(E_RM00DC_Err_ck_null_cols, UG_ERR_ERROR, 2, table_owner,
			table_name);
		return (E_RM00DC_Err_ck_null_cols);
	}
	if (cnt)
	{
		IIUGerr(E_RM00DD_Tbl_null_keys, UG_ERR_ERROR, 2, table_owner,
			table_name);
		return (E_RM00DD_Tbl_null_keys);
	}
	return (OK);
}
