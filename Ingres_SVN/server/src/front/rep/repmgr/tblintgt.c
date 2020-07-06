# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <cm.h>
# include <st.h>
# include <cv.h>
# include <er.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <erfe.h>
# include <uf.h>
# include <ug.h>
# include <rpu.h>
# include "errm.h"
/**
** Name:	tblintgt.qsc - table integrity report screen
**
** Description:
**	Defines
**		tbl_integ		- table integrity report screen
**		build_order_clause	- build order clause
**
** History:
**	16-dec-96 (joea)
**		Created based on tblinteg.osq in replicator library.
**	14-jan-97 (joea)
**		Change in fast message numbers to close gap.
**	31-jan-97 (joea)
**		Correct QUEL begin/end to SQL begin/end in loadtable of column
**		names.
**	12-feb-98 (joea)
**		In build_order_clause, correct the test for inserting comma
**		separators.  Order the sort_order tablefield by column_sequence
**		so that the ORDER clause is prepared correctly. Only select
**		replicated columns into the tablefield.
**	05-jun-98 (padni01) bug 88184
**		Assume databaseno1 to be the local database.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	18-Apr-2007 (kibro01) b117799
**	    Ensure duplicate columns are not selected into ORDER BY clause
**/
# define MAX_COLS	100
GLOBALREF
  char RMdb_display[];
GLOBALREF
  i2 RMlocal_db_no;
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmtbl_integrity"), *tf_name = ERx("sort_order"), *
  ftbl_name = ERx("table_name"), *ftbl_owner = ERx("table_owner"), *fcdds_no = ERx("cdds_no")
  , *fcol_name = ERx("column_name"), *fsort_order = ERx("sort_order"), *
  fkey_seq = ERx("key_sequence"), *fdb_no_1 = ERx("databaseno1"), *fdb_no_2 = ERx("databaseno2")
  , *fdb_name_1 = ERx("dbname1"), *fdb_name_2 = ERx("dbname2"), *fbegin_time = ERx("begin_time")
  , *fend_time = ERx("end_time"), *trans_time = ERx("trans_time");
FUNC_EXTERN STATUS table_integrity(i2 db1, i2 db2, i4 table_no, i2 cdds_no,
	char *begin_time, char *end_time, char *order_clause, char *filename);
static void build_order_clause(char *order_clause);
/*{
** Name:	tbl_integ	- table integrity report screen
**
** Description:
**	Handles the sort_order tablefield for the table integrity report. When
**	"Go" is selected, creates the table integrity report.
**
** Inputs:
**	file_name		- file name
**
** Outputs:
**	none
**
** Returns:
**	OK
**	-1	- error running the report
**	1	- no report run
*/
STATUS
tbl_integ(
char	*file_name)
  {
  char stmt[256];
  i4 cnt;
  i2 db_no_1;
  i2 db_no_2;
  char full_db_name[DB_MAXNAME*2+3];
  i2 cdds_no;
  i4 tbl_no;
  char tbl_name[DB_MAXNAME+1];
  char tbl_owner[DB_MAXNAME+1];
  char cdds_name[DB_MAXNAME+1];
  char cdds_lookup_tbl[DB_MAXNAME+1];
  char col_name[DB_MAXNAME+1];
  i4 key_seq;
  i4 col_seq;
  char begin_time[26];
  char end_time[26];
    i4 chng;
    char msg[256];
    char objname[FE_MAXNAME+1];
	DBEC_ERR_INFO	errinfo;
	STATUS	retval = 1;
	char	order_clause[1024];
	i4	i;
	i4	row;
	char	*p1, *p2, *pend;
	char	list_row[36];
	char	list_rows[2048];
	if (!form_initd)
	{
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), form_name) != OK)
		{
			IIUGerr(E_RM0006_Get_form_failed, UG_ERR_ERROR, 1,
				form_name);
			return (retval);
		}
		form_initd = TRUE;
	}
/* # line 149 "tblintgt.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"u") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 151 "tblintgt.qsc" */	/* display */
    {
      if (IIdispfrm(form_name,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio((char *)"db",(short *)0,1,32,0,RMdb_display);
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 155 "tblintgt.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,DB_MAXNAME+1-1,tbl_name,ftbl_name);
              } /* IIfsetio */
            }
/* # line 156 "tblintgt.qsc" */	/* host code */
/* # line 152 "tblintgt.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_regist_tables where table_name=");
    IIputdomio((short *)0,1,32,0,tbl_name);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 156 "tblintgt.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
		{
			IIUGerr(E_RM006F_Error_check_tbl_reg, UG_ERR_ERROR, 1,
				tbl_name);
/* # line 164 "tblintgt.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 165 "tblintgt.qsc" */	/* host code */
		}
		if (cnt == 0)
		{
			/* Check to see if this is a real table */
/* # line 169 "tblintgt.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iitables where table_name=");
    IIputdomio((short *)0,1,32,0,tbl_name);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 173 "tblintgt.sc" */	/* host code */
			if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
			{
				IIUGerr(E_RM006F_Error_check_tbl_reg,
					UG_ERR_ERROR, 1, tbl_name);
/* # line 177 "tblintgt.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 178 "tblintgt.qsc" */	/* host code */
			}
			if (cnt > 0)
				IIUGerr(E_RM0070_Tbl_not_registered,
					UG_ERR_ERROR, 0);
			else
				IIUGerr(E_RM0071_Tbl_not_exist, UG_ERR_ERROR,
					0);
/* # line 185 "tblintgt.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 186 "tblintgt.qsc" */	/* host code */
		}
		else if (cnt == 1)	/* Found exactly one table */
		{
/* # line 189 "tblintgt.qsc" */	/* set_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIstfsio(53,ftbl_owner,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 190 "tblintgt.qsc" */	/* host code */
/* # line 205 "tblintgt.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select table_no, cdds_lookup_table, table_owner, cdds_no from dd_regi\
st_tables where table_name=");
    IIputdomio((short *)0,1,32,0,tbl_name);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tbl_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,cdds_lookup_tbl);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,tbl_owner);
      IIgetdomio((short *)0,1,30,2,&cdds_no);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 211 "tblintgt.sc" */	/* host code */
			if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
			{
				IIUGerr(E_RM006F_Error_check_tbl_reg,
					UG_ERR_ERROR, 1, tbl_name);
/* # line 200 "tblintgt.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 201 "tblintgt.qsc" */	/* host code */
			}
/* # line 202 "tblintgt.qsc" */	/* putform */
            {
              if (IIfsetio(form_name) != 0) {
                IIputfldio(ftbl_owner,(short *)0,1,32,0,tbl_owner);
                IIputfldio(fcdds_no,(short *)0,1,30,2,&cdds_no);
              } /* IIfsetio */
            }
/* # line 204 "tblintgt.qsc" */	/* host code */
		}
		else		/* More than one, present ListChoice */
		{
			*list_rows = EOS;
/* # line 233 "tblintgt.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select table_owner from dd_regist_tables where table_name=");
    IIputdomio((short *)0,1,32,0,tbl_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"order by table_owner");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,tbl_owner);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 239 "tblintgt.sc" */	/* host code */
				STtrmwhite(tbl_owner);
				STprintf(list_row, ERx("%s\n"), tbl_owner);
				STcat(list_rows, list_row);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 243 "tblintgt.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 244 "tblintgt.sc" */	/* host code */
			row = IIFDlpListPick(ERget(F_RM00A2_Table_Owner),
				list_rows, 3, -1, -1, NULL, NULL, NULL, NULL);
			if (row >= 0)
			{
				p1 = list_rows;
				pend = STindex(p1, ERx("\n"), 0);
				for (i = 0; i < row; ++i)
				{
					p1 = CMnext(pend);
					pend = STindex(p1, ERx("\n"), 0);
				}
				*pend = EOS;
				STcopy(p1, tbl_owner);
/* # line 257 "tblintgt.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select table_no, cdds_no, cdds_lookup_table from dd_regist_tables whe\
re table_name=");
    IIputdomio((short *)0,1,32,0,tbl_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
    IIputdomio((short *)0,1,32,0,tbl_owner);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tbl_no);
      IIgetdomio((short *)0,1,30,2,&cdds_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,cdds_lookup_tbl);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 264 "tblintgt.sc" */	/* host code */
/* # line 239 "tblintgt.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(ftbl_owner,(short *)0,1,32,0,tbl_owner);
                IIputfldio(fcdds_no,(short *)0,1,30,2,&cdds_no);
              } /* IIfsetio */
            }
/* # line 241 "tblintgt.qsc" */	/* host code */
			}
			else
			{
/* # line 244 "tblintgt.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 245 "tblintgt.qsc" */	/* host code */
			}
		}
		STtrmwhite(cdds_lookup_tbl);
		if (*cdds_lookup_tbl == EOS)
/* # line 250 "tblintgt.qsc" */	/* set_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIstfsio(53,fcdds_no,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 251 "tblintgt.qsc" */	/* host code */
		else
/* # line 252 "tblintgt.qsc" */	/* set_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIstfsio(53,fcdds_no,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 254 "tblintgt.qsc" */	/* host code */
/* # line 301 "tblintgt.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(vnode_name) +'::' +TRIM(database_name)from dd_databases w\
here database_no=");
    IIputdomio((short *)0,1,30,2,&RMlocal_db_no);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME*2+3-1,full_db_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 305 "tblintgt.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
		{
/* # line 307 "tblintgt.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 308 "tblintgt.sc" */	/* host code */
			IIUGerr(E_RM0030_Err_retrieve_db_name, UG_ERR_ERROR, 0);
/* # line 262 "tblintgt.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 263 "tblintgt.qsc" */	/* host code */
		}
/* # line 315 "tblintgt.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 316 "tblintgt.sc" */	/* host code */
/* # line 265 "tblintgt.qsc" */	/* putform */
            {
              if (IIfsetio(form_name) != 0) {
                IIputfldio(fdb_no_1,(short *)0,1,30,2,&RMlocal_db_no);
                IIputfldio(fdb_name_1,(short *)0,1,32,0,full_db_name);
              } /* IIfsetio */
            }
/* # line 268 "tblintgt.qsc" */	/* host code */
		/* Populate the sort_order tablefield */
/* # line 269 "tblintgt.qsc" */	/* inittable */
            {
              if (IItbinit(form_name,tf_name,(char *)"u") != 0) {
                IItfill();
              } /* IItbinit */
            }
/* # line 270 "tblintgt.qsc" */	/* host code */
/* # line 332 "tblintgt.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct c.column_name, r.key_sequence, r.column_sequence from\
 iicolumns c, dd_regist_columns r, dd_regist_tables t where c.table_na\
me=");
    IIputdomio((short *)0,1,32,0,tbl_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and c.table_owner=");
    IIputdomio((short *)0,1,32,0,tbl_owner);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and r.table_no=t.table_no and c.column_name=r.column_name and t.table\
_name=c.table_name and t.table_owner=c.table_owner and r.column_sequen\
ce!=0 order by column_sequence");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,col_name);
      IIgetdomio((short *)0,1,30,4,&key_seq);
      IIgetdomio((short *)0,1,30,4,&col_seq);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 346 "tblintgt.sc" */	/* host code */
/* # line 284 "tblintgt.qsc" */	/* loadtable */
            {
              if (IItbact(form_name,tf_name,1) != 0) {
                IItcoputio(fcol_name,(short *)0,1,32,0,col_name);
                IItcoputio(fkey_seq,(short *)0,1,30,4,&key_seq);
                IITBceColEnd();
              } /* IItbact */
            }
/* # line 286 "tblintgt.qsc" */	/* host code */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
/* # line 356 "tblintgt.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 357 "tblintgt.sc" */	/* host code */
/* # line 288 "tblintgt.qsc" */	/* loadtable */
            {
              if (IItbact(form_name,tf_name,1) != 0) {
                IItcoputio(fcol_name,(short *)0,1,32,0,trans_time);
                IITBceColEnd();
              } /* IItbact */
            }
/* # line 289 "tblintgt.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 294 "tblintgt.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,30,2,&cdds_no,fcdds_no);
              } /* IIfsetio */
            }
/* # line 295 "tblintgt.qsc" */	/* host code */
		cnt = 0;
		STprintf(stmt,
			ERx("SELECT COUNT(*) FROM %s WHERE cdds_no = %d"),
			cdds_lookup_tbl, (i4)cdds_no);
/* # line 383 "tblintgt.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqMods(3);
    IIsqMods(1);
    IIsqExImmed(stmt);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 384 "tblintgt.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
		{
/* # line 386 "tblintgt.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 387 "tblintgt.sc" */	/* host code */
			IIUGerr(E_RM003D_Error_check_cdds, UG_ERR_ERROR, 0);
/* # line 304 "tblintgt.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 305 "tblintgt.qsc" */	/* host code */
		}
		else if (cnt == 0)
		{
/* # line 396 "tblintgt.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 397 "tblintgt.sc" */	/* host code */
			IIUGerr(E_RM0072_Invalid_cdds_no, UG_ERR_ERROR, 0);
/* # line 310 "tblintgt.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 311 "tblintgt.qsc" */	/* host code */
		}
/* # line 404 "tblintgt.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 405 "tblintgt.sc" */	/* host code */
/* # line 313 "tblintgt.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 318 "tblintgt.qsc" */	/* getform */
            {
              if (IIfsetio(form_name) != 0) {
                IIgetfldio((short *)0,1,30,2,&db_no_2,fdb_no_2);
              } /* IIfsetio */
            }
/* # line 319 "tblintgt.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 320 "tblintgt.qsc" */	/* host code */
		if (chng || db_no_2 == 0)
		{
/* # line 428 "tblintgt.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(vnode_name) +'::' +TRIM(database_name)from dd_databases w\
here database_no=");
    IIputdomio((short *)0,1,30,2,&db_no_2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME*2+3-1,full_db_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 433 "tblintgt.sc" */	/* host code */
			if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo)
				!= OK)
			{
/* # line 436 "tblintgt.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 437 "tblintgt.sc" */	/* host code */
				IIUGerr(E_RM0025_Error_check_db_no,
					UG_ERR_ERROR, 0);
/* # line 333 "tblintgt.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 334 "tblintgt.qsc" */	/* host code */
			}
			if (errinfo.rowcount == 0)
			{
/* # line 447 "tblintgt.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 448 "tblintgt.sc" */	/* host code */
				IIUGerr(E_RM0026_Invalid_db_no, UG_ERR_ERROR,
					0);
/* # line 340 "tblintgt.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 341 "tblintgt.qsc" */	/* host code */
			}
/* # line 456 "tblintgt.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 457 "tblintgt.sc" */	/* host code */
/* # line 343 "tblintgt.qsc" */	/* putform */
            {
              if (IIfsetio(form_name) != 0) {
                IIputfldio(fdb_name_2,(short *)0,1,32,0,full_db_name);
              } /* IIfsetio */
            }
/* # line 344 "tblintgt.qsc" */	/* host code */
		}
/* # line 345 "tblintgt.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 351 "tblintgt.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 352 "tblintgt.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,30,2,&db_no_1,fdb_no_1);
                IIgetfldio((short *)0,1,30,2,&db_no_2,fdb_no_2);
                IIgetfldio((short *)0,1,30,2,&cdds_no,fcdds_no);
                IIgetfldio((short *)0,1,32,25,begin_time,fbegin_time);
                IIgetfldio((short *)0,1,32,25,end_time,fend_time);
              } /* IIfsetio */
            }
/* # line 355 "tblintgt.qsc" */	/* host code */
		IIUGmsg(ERget(F_RM00A4_Working), FALSE, 0);
		build_order_clause(order_clause);
		retval = table_integrity(db_no_1, db_no_2, tbl_no, cdds_no,
			begin_time, end_time, order_clause, file_name);
/* # line 494 "tblintgt.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 495 "tblintgt.sc" */	/* host code */
		if (retval != OK)
			retval = -1;
/* # line 364 "tblintgt.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 371 "tblintgt.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,objname,19,(char *)0,0);
              } /* IIiqset */
            }
/* # line 372 "tblintgt.qsc" */	/* host code */
		if (STequal(objname, fcdds_no))
		{
			*list_rows = EOS;
			STprintf(stmt,
				ERx("SELECT DISTINCT d.cdds_no, d.cdds_name FROM %s t, dd_cdds d WHERE t.cdds_no = d.cdds_no ORDER BY cdds_no"),
				cdds_lookup_tbl);
/* # line 517 "tblintgt.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqMods(3);
    IIsqExImmed(stmt);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&cdds_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,cdds_name);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 520 "tblintgt.sc" */	/* host code */
				STtrmwhite(cdds_name);
				STprintf(list_row, ERx("%d;%s\n"), cdds_no,
					cdds_name);
				STcat(list_rows, list_row);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
/* # line 525 "tblintgt.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 526 "tblintgt.sc" */	/* host code */
			row = -1;
			if (*list_rows != EOS)
			{
				row = IIFDlpListPick(NULL, list_rows, 3,
					-1, -1, NULL, NULL, NULL, NULL);
			}
			else
			{
				IIUGerr(E_RM0073_No_valid_cdds, UG_ERR_ERROR,
					0);
/* # line 397 "tblintgt.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 398 "tblintgt.qsc" */	/* host code */
			}
			if (row >= 0)
			{
				p1 = list_rows;
				pend = STindex(p1, ERx("\n"), 0);
				for (i = 0; i < row; ++i)
				{
					p1 = CMnext(pend);
					pend = STindex(p1, ERx("\n"), 0);
				}
				*pend = EOS;
				p2 = pend = STindex(p1, ERx(";"), 0);
				CMnext(pend);
				STcopy(pend, cdds_name);
				*p2 = EOS;
				CVan(p1, &i);
				cdds_no = (i2)i;
/* # line 415 "tblintgt.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(fcdds_no,(short *)0,1,30,2,&cdds_no);
              } /* IIfsetio */
            }
/* # line 416 "tblintgt.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 417 "tblintgt.qsc" */	/* host code */
			}
		}
		else
		{
			IIUFfieldHelp();
		}
          }
        } else if (IIretval() == 6) {
          {
/* # line 429 "tblintgt.qsc" */	/* host code */
		FEhelp(ERx("rmtblint.hlp"), ERget(F_RM00A5_Tbl_integrity_rpt));
          }
        } else if (IIretval() == 7) {
          {
/* # line 436 "tblintgt.qsc" */	/* host code */
		retval = 1;
/* # line 437 "tblintgt.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else {
          if (1) goto IIfdE1;
        } /* IIretval */
      } /* IIrunform */
IIfdF1:
      if (IIchkfrm() == 0) goto IIfdB1;
      goto IIfdE1;
IImuI1:
      if (IIFRafActFld(ftbl_name,0,1) == 0) goto IIfdE1;
      if (IIFRafActFld(fcdds_no,0,2) == 0) goto IIfdE1;
      if (IIFRafActFld(fdb_no_2,0,3) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Go),ERget(F_RM00A3_Go_tbl_integ),2,2,4) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Lookup),ERget(F_FE0116_LookupExpl),0,0,5) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,6) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,7) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,7) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 440 "tblintgt.qsc" */	/* host code */
	return (retval);
  }
/*{
** Name:	build_order_clause	- build order clause
**
** Description:
**	builds the order by clause for the table integrity report
**	based on the values off the table integrity screen
**
** Inputs:
**	none
**
** Outputs:
**	order_clause	- order clause
**
** Returns:
**	none
*/
static void
build_order_clause(
char	*order_clause)
  {
    char col_name[DB_MAXNAME+1];
    i4 idx;
    short null_ind;
	i4	i;
	i4	col_order[MAX_COLS];
	char	col_names[MAX_COLS][DB_MAXNAME+1];
	char	tmp[10];
	*order_clause = EOS;
	for (idx = 0; idx < MAX_COLS; idx++)
		*col_names[idx] = EOS;
	i = 0;
/* # line 477 "tblintgt.qsc" */	/* unloadtable */
    {
      if (IItbact(form_name,tf_name,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,col_name,fcol_name);
        IItcogetio(&null_ind,1,30,4,&idx,fsort_order);
        IITBceColEnd();
        {
/* # line 480 "tblintgt.qsc" */	/* host code */
		++i;
		if (null_ind != -1)
		{
			STcopy(col_name, col_names[idx]);
			col_order[idx] = i;
		}
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 488 "tblintgt.qsc" */	/* host code */
	for (idx = 0; idx < MAX_COLS; idx++)
	{
		if (*col_names[idx] != EOS)
		{
			STprintf(tmp, ERx("%d"), col_order[idx] + 3);
			if (*order_clause != EOS)
				STcat(order_clause, ERx(", "));
			STcat(order_clause, tmp);
		}
	}
  }
