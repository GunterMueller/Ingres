# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <st.h>
# include <er.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <ug.h>
# include <rpu.h>
# include "errm.h"
/**
** Name:	tblsave.qsc - save table information
**
** Description:
**	Defines
**		tblsave		- save table information
**
** History:
**	10-jan-97 (joea)
**		Created based on tablesave.sc in replicator library.
**	18-may-98 (padni01) bug 89865
**		Use date_gmt instead of date to set columns_registered field 
**		of dd_regist_tables.
**/
static READONLY
  char *form_name = ERx("rmtbl_detail"), *tf_name = ERx("iitf"), *fcdds_no = ERx("cdds_no")
  , *fcdds_name = ERx("cdds_name"), *findex_used = ERx("index_used"), *
  fcdds_lookup_tbl = ERx("cdds_lookup_table"), *fprio_lookup_tbl = ERx("prio_lookup_table")
  , *fsupp_objs_created = ERx("supp_objs_created"), *frules_created = ERx("rules_created")
  , *fcol_name = ERx("column_name"), *fcol_seq = ERx("col_seq"), *
  fbase_col_seq = ERx("base_col_seq"), *fkey_seq = ERx("key_sequence"), *
  freplic_col = ERx("replicated_column");
FUNC_EXTERN STATUS db_config_changed(i2 db_no);
/*{
** Name:	tblsave - save table information
**
** Description:
**	Save information from table registration screen
**
** Inputs:
**	tbl_no	- table number
**	tbl_name	- table name
**
** Outputs:
**	none
**
** Returns:
**	OK or FAIL
*/
STATUS
tblsave(
i4	tbl_no,
char	*tbl_name)
# if 0
  i4 tbl_no;
  char *tbl_name;
# endif
  {
  char cdds_lookup_tbl[DB_MAXNAME+1];
  char prio_lookup_tbl[DB_MAXNAME+1];
  i2 cdds_no;
  char index_used[DB_MAXNAME+1];
  char supp_objs_created[25];
  char rules_created[25];
  i4 col_seq;
  i4 base_col_seq;
  i4 key_seq;
  char col_name[DB_MAXNAME+1];
    char replic_col[2];
	DBEC_ERR_INFO	errinfo;
	STATUS	retval = OK;
	/* Clean out existing columns for this table */
/* # line 81 "tblsave.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from dd_regist_columns where table_no=");
    IIputdomio((short *)0,1,30,4,&tbl_no);
    IIsyncup((char *)0,0);
  }
/* # line 82 "tblsave.sc" */	/* host code */
	if (RPdb_error_check(0, NULL) != OK)
	{
/* # line 84 "tblsave.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 85 "tblsave.sc" */	/* host code */
		IIUGerr(E_RM004E_Error_del_regist_col, UG_ERR_ERROR, 0);
		return (FAIL);
	}
/* # line 102 "tblsave.qsc" */	/* unloadtable */
    {
      if (IItbact(form_name,tf_name,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,col_name,fcol_name);
        IItcogetio((short *)0,1,30,4,&key_seq,fkey_seq);
        IItcogetio((short *)0,1,30,4,&col_seq,fcol_seq);
        IItcogetio((short *)0,1,30,4,&base_col_seq,fbase_col_seq);
        IItcogetio((short *)0,1,32,1,replic_col,freplic_col);
        IITBceColEnd();
        {
/* # line 106 "tblsave.qsc" */	/* host code */
		/* Add interpretation of y/n in replicated_column */
		if (STequal(replic_col, ERget(F_UG0006_No1)))
			col_seq = 0;
		else
			col_seq = base_col_seq;
/* # line 105 "tblsave.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"tblsave1",7212,7142);
      IIputdomio((short *)0,1,30,4,&tbl_no);
      IIputdomio((short *)0,1,32,0,col_name);
      IIputdomio((short *)0,1,30,4,&col_seq);
      IIputdomio((short *)0,1,30,4,&key_seq);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"tblsave1",7212,7142);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_regist_columns(table_no, column_name, column_sequence,\
 key_sequence)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&tbl_no);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,col_name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&col_seq);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&key_seq);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"tblsave1",7212,7142);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 110 "tblsave.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
		{
/* # line 112 "tblsave.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 113 "tblsave.sc" */	/* host code */
			IIUGerr(E_RM004F_Error_ins_regist_col, UG_ERR_ERROR,
				0);
			retval = FAIL;
/* # line 123 "tblsave.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 124 "tblsave.qsc" */	/* host code */
		}
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 126 "tblsave.qsc" */	/* host code */
	if (retval)
		return (retval);
/* # line 129 "tblsave.qsc" */	/* getform */
    {
      if (IIfsetio(form_name) != 0) {
        IIgetfldio((short *)0,1,30,2,&cdds_no,fcdds_no);
        IIgetfldio((short *)0,1,32,DB_MAXNAME+1-1,index_used,findex_used);
        IIgetfldio((short *)0,1,32,24,supp_objs_created,fsupp_objs_created);
        IIgetfldio((short *)0,1,32,24,rules_created,frules_created);
        IIgetfldio((short *)0,1,32,DB_MAXNAME+1-1,cdds_lookup_tbl,
        fcdds_lookup_tbl);
        IIgetfldio((short *)0,1,32,DB_MAXNAME+1-1,prio_lookup_tbl,
        fprio_lookup_tbl);
      } /* IIfsetio */
    }
/* # line 134 "tblsave.qsc" */	/* host code */
/* # line 144 "tblsave.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"tblsave2",7213,5757);
      IIputdomio((short *)0,1,30,2,&cdds_no);
      IIputdomio((short *)0,1,32,0,index_used);
      IIputdomio((short *)0,1,32,0,supp_objs_created);
      IIputdomio((short *)0,1,32,0,rules_created);
      IIputdomio((short *)0,1,32,0,cdds_lookup_tbl);
      IIputdomio((short *)0,1,32,0,prio_lookup_tbl);
      IIputdomio((short *)0,1,30,4,&tbl_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"tblsave2",7213,5757);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_tables set cdds_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&cdds_no);
        IIwritio(0,(short *)0,1,32,0,(char *)", index_used=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,index_used);
        IIwritio(0,(short *)0,1,32,0,(char *)", supp_objs_created=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,supp_objs_created);
        IIwritio(0,(short *)0,1,32,0,(char *)", rules_created=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,rules_created);
        IIwritio(0,(short *)0,1,32,0,(char *)", cdds_lookup_table=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,cdds_lookup_tbl);
        IIwritio(0,(short *)0,1,32,0,(char *)", prio_lookup_table=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,prio_lookup_tbl);
        IIwritio(0,(short *)0,1,32,0,(char *)
", columns_registered=DATE_GMT('now')where table_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,30,4,&tbl_no);
        IIexDefine(0,(char *)"tblsave2",7213,5757);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 153 "tblsave.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
/* # line 155 "tblsave.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 156 "tblsave.sc" */	/* host code */
		IIUGerr(E_RM0050_Error_updt_regist_tbl, UG_ERR_ERROR, 0);
		return (FAIL);
	}
	/* No row was updated, do an insert */
	if (errinfo.rowcount == 0)
	{
/* # line 162 "tblsave.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"tblsave3",7214,3684);
      IIputdomio((short *)0,1,30,4,&tbl_no);
      IIputdomio((short *)0,1,32,0,tbl_name);
      IIputdomio((short *)0,1,30,2,&cdds_no);
      IIputdomio((short *)0,1,32,0,index_used);
      IIputdomio((short *)0,1,32,0,cdds_lookup_tbl);
      IIputdomio((short *)0,1,32,0,prio_lookup_tbl);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"tblsave3",7214,3684);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_regist_tables(table_no, table_name, cdds_no, index_use\
d, cdds_lookup_table, prio_lookup_table, columns_registered)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&tbl_no);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,tbl_name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&cdds_no);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,index_used);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,cdds_lookup_tbl);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,prio_lookup_tbl);
        IIwritio(0,(short *)0,1,32,0,(char *)", DATE_GMT('now'))");
        IIexDefine(0,(char *)"tblsave3",7214,3684);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 169 "tblsave.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
		{
/* # line 171 "tblsave.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 172 "tblsave.sc" */	/* host code */
			IIUGerr(E_RM0050_Error_updt_regist_tbl, UG_ERR_ERROR,
				0);
			return (FAIL);
		}
	}
	if (db_config_changed(0) != OK)
	{
/* # line 179 "tblsave.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 180 "tblsave.sc" */	/* host code */
		return (FAIL);
	}
/* # line 182 "tblsave.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 183 "tblsave.sc" */	/* host code */
	return (OK);
  }
