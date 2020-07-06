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
# include <stdprmpt.h>
# include <rpu.h>
# include "errm.h"
/**
** Name:	tbledit.qsc - edit a registered table
**
** Description:
**	Defines
**		tbl_edit	- edit a registered table
**
** History:
**	10-jan-97 (joea)
**		Created based on tbledit.osq in replicator library.
**	24-mar-97 (joea) bug 81176
**		Set tbl_num return variable after call to tbl_register().
**	23-jun-97 (joea) bug 83328
**		Correct getform statement on priority lookup table field.
**	10-oct-97 (joea)
**		Remove unused argument to dropsupp.
**	14-nov-97 (joea)
**		Issue an error message if zero rows selected in ListChoices
**		menu item.
**	02-sep-98 (abbjo03)
**		Replace ddba_messageit with IIUGerr. In the Save option, add
**		error checking after calling dropsupp.
**	22-mar-99 (abbjo03)
**		Correct selection of valid lookup tables in a SQL-92
**		installation.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	20-mar-02 (toumi01)
**		Change 300 to DB_MAX_COLS (at present 1024).
**/
  typedef struct {
    char col_name[DB_MAXNAME+1];
    i4 key_seq;
    char datatype[DB_MAXNAME+1];
  } DD_COLUMN;
  static DD_COLUMN keys[DB_MAX_COLS];
GLOBALREF
  i2 RMlocal_db_no;
GLOBALREF
  char RMdb_display[];
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmtbl_detail"), *tf_name = ERx("iitf"), *ftbl_name = ERx("table_name")
  , *ftbl_owner = ERx("table_owner"), *fcdds_no = ERx("cdds_no"), *fcdds_name 
  = ERx("cdds_name"), *findex_used = ERx("index_used"), *fcdds_lookup_tbl = ERx("cdds_lookup_table")
  , *fprio_lookup_tbl = ERx("prio_lookup_table"), *fcols_registd = ERx("columns_registered")
  , *fsupp_objs_created = ERx("supp_objs_created"), *frules_created = ERx("rules_created")
  , *fcol_name = ERx("column_name"), *fcol_seq = ERx("col_seq"), *
  fbase_col_seq = ERx("base_col_seq"), *fkey_seq = ERx("key_sequence"), *
  fdatatype = ERx("datatype"), *fmandatory = ERx("mandatory"), *freplic_col = ERx("replicated_column")
  ;
FUNC_EXTERN STATUS tbl_register(i4 *tbl_no, char *tbl_name,
	char *tbl_owner, i2 cdds_no, char *unique_rule,
	char *tbl_indexes, char *columns_registered, char *index_used);
FUNC_EXTERN STATUS dropsupp(i4 tbl_no);
FUNC_EXTERN STATUS tblsave(i4 tbl_no, char *tbl_name);
static void set_table_keys(i4 tbl_no, char *tbl_name, char *tbl_owner,
	char *index_used);
static bool pick_table(char *list_rows, char *title, char *tbl_name);
/*{
** Name:	tbl_edit	- edit a registered table
**
** Description:
**	Display and manage details of table registration including CDDS and
**	priority lookups, CDDS assignment, and registration of key and
**	replicated columns.
**
** Inputs:
**
** Outputs:
**
** Returns:
**	none
*/
void
tbl_edit(
i4	*tbl_num,
char	*tbl_name,
char	*tbl_owner,
i2	*cdds_num,
char	*cdds_name,
char	*cdds_lookup_tbl,
char	*prio_lookup_tbl,
char	*index_used,
char	*cols_registd,
char	*supp_objs_created,
char	*rules_created,
char	*unique_rule,
char	*tbl_indexes)
# if 0
  char *tbl_name;
  char *tbl_owner;
  char *cdds_name;
  char *cdds_lookup_tbl;
  char *prio_lookup_tbl;
  char *index_used;
  char *cols_registd;
  char *supp_objs_created;
  char *rules_created;
# endif
  {
  i4 index_exist;
  i4 lookup_exist;
  char col_name[DB_MAXNAME+1];
  i4 base_col_seq;
  i4 col_seq;
  i4 key_seq;
  char datatype[DB_MAXNAME+1];
  char mandatory[2];
  char replic_col[2];
  char local_db_name[DB_MAXNAME*2+3];
  i2 cdds_no;
  i4 tbl_no;
  char new_cdds_name[DB_MAXNAME+1];
  char lookup_tbl[DB_MAXNAME+1];
  char index_name[DB_MAXNAME+1];
  char cdds_lookup[DB_MAXNAME+1];
  char prio_lookup[DB_MAXNAME+1];
  i4 n;
    i4 chng;
    char objname[FE_MAXNAME+1];
    i4 in_tf;
	DBEC_ERR_INFO	errinfo;
    char *yes = ERget(F_UG0001_Yes1);
    char *no = ERget(F_UG0006_No1);
	char	list_row[36];
	char	list_rows[2048];
	i4	i;
	i4	row;
	char	*p1, *p2, *pend;
	cdds_no = *cdds_num;
	tbl_no = *tbl_num;
	STcopy(index_used, index_name);
	STcopy(cdds_lookup_tbl, cdds_lookup);
	STcopy(prio_lookup_tbl, prio_lookup);
	if (!form_initd)
	{
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), form_name) != OK)
		{
			IIUGerr(E_RM0006_Get_form_failed, UG_ERR_ERROR, 1,
				form_name);
			return;
		}
		form_initd = TRUE;
	}
/* # line 203 "tbledit.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"u") != 0) {
        IIthidecol(fcol_seq,(char *)"i4 not null");
        IIthidecol(fbase_col_seq,(char *)"i4 not null");
        IIthidecol(fdatatype,(char *)"char(32) not null");
        IIthidecol(fmandatory,(char *)"char(1) not null");
        IItfill();
      } /* IItbinit */
    }
/* # line 207 "tbledit.qsc" */	/* host code */
	STtrmwhite(tbl_name);
	STtrmwhite(tbl_owner);
	IIUGmsg(ERget(F_RM0053_Selecting_column_reg), FALSE, 2, tbl_owner,
		tbl_name);
	/*
	** Determine whether the dd_regist_columns table has been populated
	** for this table
	*/
	errinfo.rowcount = 0;
	while (errinfo.rowcount == 0)
	{
		if (tbl_no)
		{
/* # line 199 "tbledit.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select d.column_name, i.column_sequence, d.column_sequence, d.key_seq\
uence, LOWERCASE(TRIM(i.column_datatype)), CHAREXTRACT('ny', LOCATE(i.\
column_nulls +i.column_defaults, 'Y') -1)from dd_regist_columns d, dd_\
regist_tables t, iicolumns i where d.table_no=");
    IIputdomio((short *)0,1,30,4,&tbl_no);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and d.table_no=t.table_no and d.column_name=i.column_name and t.table\
_name=i.table_name and t.table_owner=i.table_owner order by 2");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,col_name);
      IIgetdomio((short *)0,1,30,4,&base_col_seq);
      IIgetdomio((short *)0,1,30,4,&col_seq);
      IIgetdomio((short *)0,1,30,4,&key_seq);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,datatype);
      IIgetdomio((short *)0,1,32,1,mandatory);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 217 "tbledit.sc" */	/* host code */
				if (col_seq)
					STcopy(yes, replic_col);
				else
					STcopy(no, replic_col);
/* # line 242 "tbledit.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(freplic_col,(short *)0,1,32,0,replic_col);
        IItcoputio(fcol_name,(short *)0,1,32,0,col_name);
        IItcoputio(fkey_seq,(short *)0,1,30,4,&key_seq);
        IItcoputio(fcol_seq,(short *)0,1,30,4,&col_seq);
        IItcoputio(fbase_col_seq,(short *)0,1,30,4,&base_col_seq);
        IItcoputio(fdatatype,(short *)0,1,32,0,datatype);
        IItcoputio(fmandatory,(short *)0,1,32,0,mandatory);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 249 "tbledit.qsc" */	/* host code */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
			RPdb_error_check(0, &errinfo);
			if (errinfo.errorno)
			{
/* # line 239 "tbledit.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 240 "tbledit.sc" */	/* host code */
				FTbell();
				IIUGerr(E_RM0012_Error_selecting,
					UG_ERR_ERROR, 0);
				return;
			}
		}
		if (errinfo.rowcount == 0 || tbl_no == 0)
		{
/* # line 248 "tbledit.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 249 "tbledit.sc" */	/* host code */
			if (IIUFccConfirmChoice(CONF_GENERIC, NULL, NULL,
				NULL, NULL, S_RM0038_No_table_regist_yet,
				S_RM0039_Yes_register, S_RM003A_No_register,
				ERx(""), TRUE) != CONFCH_YES)
				return;
			if (tbl_register(&tbl_no, tbl_name, tbl_owner, cdds_no,
					unique_rule, tbl_indexes, cols_registd,
					index_used) != OK)
				return;
			*tbl_num = tbl_no;
		}
	}
/* # line 261 "tbledit.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 262 "tbledit.sc" */	/* host code */
	/* Make sure that a unique key exists */
/* # line 263 "tbledit.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"tbledit1",7214,27894);
      IIputdomio((short *)0,1,32,0,tbl_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"tbledit1",7214,27894);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiindexes where base_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,tbl_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and unique_rule='U'");
        IIexDefine(0,(char *)"tbledit1",7214,27894);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&index_exist);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 268 "tbledit.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
	{
/* # line 270 "tbledit.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 271 "tbledit.sc" */	/* host code */
		FTbell();
		IIUGerr(E_RM003B_Err_check_uniq_index, UG_ERR_ERROR, 0);
	}
	else if (!index_exist && CMcmpcase(unique_rule, "U"))
	{
/* # line 276 "tbledit.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 277 "tbledit.sc" */	/* host code */
		IIUGerr(E_RM003C_No_unique_indexes, UG_ERR_ERROR, 0);
	}
/* # line 295 "tbledit.qsc" */	/* display */
    {
      if (IIdispfrm(form_name,(char *)"u") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio((char *)"db",(short *)0,1,32,0,RMdb_display);
      IIputfldio(ftbl_name,(short *)0,1,32,0,tbl_name);
      IIputfldio(ftbl_owner,(short *)0,1,32,0,tbl_owner);
      IIputfldio(fcdds_no,(short *)0,1,30,2,&cdds_no);
      IIputfldio(fcdds_name,(short *)0,1,32,0,cdds_name);
      IIputfldio(findex_used,(short *)0,1,32,0,index_used);
      IIputfldio(fcdds_lookup_tbl,(short *)0,1,32,0,cdds_lookup_tbl);
      IIputfldio(fprio_lookup_tbl,(short *)0,1,32,0,prio_lookup_tbl);
      IIputfldio(fcols_registd,(short *)0,1,32,0,cols_registd);
      IIputfldio(fsupp_objs_created,(short *)0,1,32,0,supp_objs_created);
      IIputfldio(frules_created,(short *)0,1,32,0,rules_created);
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 306 "tbledit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 307 "tbledit.qsc" */	/* host code */
		if (chng)
		{
/* # line 309 "tbledit.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,30,2,&cdds_no,fcdds_no);
              } /* IIfsetio */
            }
/* # line 310 "tbledit.qsc" */	/* host code */
/* # line 316 "tbledit.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"tbledit2",7215,30973);
      IIputdomio((short *)0,1,30,2,&cdds_no);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"tbledit2",7215,30973);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select cdds_name from dd_cdds where cdds_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&cdds_no);
        IIexDefine(0,(char *)"tbledit2",7215,30973);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,new_cdds_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 320 "tbledit.sc" */	/* host code */
			RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo);
			if (errinfo.errorno)
			{
				FTbell();
				IIUGerr(E_RM003D_Error_check_cdds,
					UG_ERR_ERROR, 0);
/* # line 320 "tbledit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 321 "tbledit.qsc" */	/* host code */
			}
			else if (errinfo.rowcount == 0)
			{
				i4	pcdds_no = cdds_no;
				FTbell();
				IIUGerr(E_RM002C_Not_valid_value, UG_ERR_ERROR,
					1, &pcdds_no);
/* # line 329 "tbledit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 330 "tbledit.qsc" */	/* host code */
			}
/* # line 331 "tbledit.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(fcdds_name,(short *)0,1,32,0,new_cdds_name);
              } /* IIfsetio */
            }
/* # line 332 "tbledit.qsc" */	/* host code */
		}
/* # line 333 "tbledit.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 338 "tbledit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 339 "tbledit.qsc" */	/* host code */
		if (chng)
		{
/* # line 341 "tbledit.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,DB_MAXNAME+1-1,index_name,
                findex_used);
              } /* IIfsetio */
            }
/* # line 342 "tbledit.qsc" */	/* host code */
			if (STequal(index_name, tbl_name) &&
				!CMcmpcase(unique_rule, ERx("U")))
			{
				set_table_keys(tbl_no, tbl_name, tbl_owner,
					index_name);
/* # line 347 "tbledit.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
/* # line 348 "tbledit.qsc" */	/* host code */
			}
/* # line 389 "tbledit.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"tbledit3",7216,29891);
      IIputdomio((short *)0,1,32,0,tbl_name);
      IIputdomio((short *)0,1,32,0,index_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"tbledit3",7216,29891);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiindexes where base_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,tbl_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and index_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,index_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and unique_rule='U'");
        IIexDefine(0,(char *)"tbledit3",7216,29891);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&index_exist);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 395 "tbledit.sc" */	/* host code */
			RPdb_error_check(0, &errinfo);
/* # line 396 "tbledit.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 397 "tbledit.sc" */	/* host code */
			if (errinfo.errorno)
			{
				FTbell();
				IIUGerr(E_RM003E_Error_check_index,
					UG_ERR_ERROR, 0);
/* # line 362 "tbledit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 363 "tbledit.qsc" */	/* host code */
			}
			if (!index_exist)
			{
				FTbell();
				IIUGerr(E_RM003F_Not_valid_value_str,
					UG_ERR_ERROR, 1, index_name);
/* # line 369 "tbledit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 370 "tbledit.qsc" */	/* host code */
			}
			set_table_keys(tbl_no, tbl_name, tbl_owner,
				index_name);
		}
/* # line 374 "tbledit.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 379 "tbledit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 380 "tbledit.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,DB_MAXNAME+1-1,cdds_lookup,
                fcdds_lookup_tbl);
              } /* IIfsetio */
            }
/* # line 381 "tbledit.qsc" */	/* host code */
		if (chng && *cdds_lookup != EOS)
		{
/* # line 446 "tbledit.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"tbledit4",7217,28394);
      IIputdomio((short *)0,1,32,0,cdds_lookup);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"tbledit4",7217,28394);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iicolumns where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,cdds_lookup);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and column_name in('cdds_no', 'CDDS_NO')");
        IIexDefine(0,(char *)"tbledit4",7217,28394);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&lookup_exist);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 451 "tbledit.sc" */	/* host code */
			RPdb_error_check(0, &errinfo);
/* # line 452 "tbledit.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 453 "tbledit.sc" */	/* host code */
			if (errinfo.errorno)
			{
				FTbell();
				IIUGerr(E_RM0040_Error_check_lookup,
					UG_ERR_ERROR, 0);
/* # line 395 "tbledit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 396 "tbledit.qsc" */	/* host code */
			}
			if (!lookup_exist)
			{
				FTbell();
				IIUGerr(E_RM003F_Not_valid_value_str,
					UG_ERR_ERROR, 1, cdds_lookup);
/* # line 402 "tbledit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 403 "tbledit.qsc" */	/* host code */
			}
		}
/* # line 405 "tbledit.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 410 "tbledit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 411 "tbledit.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,DB_MAXNAME+1-1,prio_lookup,
                fprio_lookup_tbl);
              } /* IIfsetio */
            }
/* # line 412 "tbledit.qsc" */	/* host code */
		if (chng && *prio_lookup != EOS)
		{
/* # line 500 "tbledit.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"tbledit5",7218,20673);
      IIputdomio((short *)0,1,32,0,prio_lookup);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"tbledit5",7218,20673);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iicolumns where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,prio_lookup);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and column_name in('dd_priority', 'DD_PRIORITY')");
        IIexDefine(0,(char *)"tbledit5",7218,20673);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&lookup_exist);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 506 "tbledit.sc" */	/* host code */
			RPdb_error_check(0, &errinfo);
/* # line 507 "tbledit.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 508 "tbledit.sc" */	/* host code */
			if (errinfo.errorno)
			{
				FTbell();
				IIUGerr(E_RM0040_Error_check_lookup,
					UG_ERR_ERROR, 0);
/* # line 427 "tbledit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 428 "tbledit.qsc" */	/* host code */
			}
			if (!lookup_exist)
			{
				FTbell();
				IIUGerr(E_RM003F_Not_valid_value_str,
					UG_ERR_ERROR, 1, cdds_lookup);
/* # line 434 "tbledit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 435 "tbledit.qsc" */	/* host code */
			}
		}
/* # line 437 "tbledit.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 442 "tbledit.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&key_seq,fkey_seq);
                IItcogetio((short *)0,1,32,1,replic_col,freplic_col);
                IItcogetio((short *)0,1,32,1,mandatory,fmandatory);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 444 "tbledit.qsc" */	/* host code */
		if (key_seq > 0 && CMcmpcase(replic_col, yes))
		{
			IIUGerr(E_RM0041_Key_col_must_regist, UG_ERR_ERROR, 0);
/* # line 447 "tbledit.qsc" */	/* putrow */
            {
              if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                IItcoputio(freplic_col,(short *)0,1,32,0,yes);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 448 "tbledit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 449 "tbledit.qsc" */	/* host code */
		}
		if (!CMcmpcase(mandatory, yes) && CMcmpcase(replic_col, yes))
		{
			IIUGerr(E_RM0042_Not_null_must_regist, UG_ERR_ERROR, 0);
/* # line 454 "tbledit.qsc" */	/* putrow */
            {
              if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                IItcoputio(freplic_col,(short *)0,1,32,0,yes);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 455 "tbledit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 456 "tbledit.qsc" */	/* host code */
		}
		if (!CMcmpcase(replic_col, yes) || !CMcmpcase(replic_col, no))
/* # line 459 "tbledit.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
/* # line 460 "tbledit.qsc" */	/* host code */
		IIUGerr(E_RM0043_Col_regist_y_or_n, UG_ERR_ERROR, 0);
          }
        } else if (IIretval() == 6) {
          {
/* # line 466 "tbledit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 467 "tbledit.qsc" */	/* host code */
		if (!chng)
		{
			IIUGerr(E_RM0029_No_changes_to_save, 0, 0);
/* # line 470 "tbledit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 471 "tbledit.qsc" */	/* host code */
		}
/* # line 473 "tbledit.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 474 "tbledit.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,30,2,&cdds_no,fcdds_no);
                IIgetfldio((short *)0,1,32,DB_MAXNAME+1-1,new_cdds_name,
                fcdds_name);
                IIgetfldio((short *)0,1,32,DB_MAXNAME+1-1,index_name,
                findex_used);
                IIgetfldio((short *)0,1,32,DB_MAXNAME+1-1,cdds_lookup,
                fcdds_lookup_tbl);
                IIgetfldio((short *)0,1,32,DB_MAXNAME+1-1,prio_lookup,
                fprio_lookup_tbl);
                IIgetfldio((short *)0,1,32,0,supp_objs_created,
                fsupp_objs_created);
                IIgetfldio((short *)0,1,32,0,rules_created,frules_created);
              } /* IIfsetio */
            }
/* # line 481 "tbledit.qsc" */	/* host code */
		STtrmwhite(supp_objs_created);
		STtrmwhite(rules_created);
		if (*supp_objs_created != EOS || *rules_created != EOS)
		{
			if (IIUFccConfirmChoice(CONF_GENERIC, NULL, NULL,
				NULL, NULL, S_RM0044_Drop_existing_objects,
				S_RM0045_Yes_drop, S_RM0046_No_drop,
				ERx(""), TRUE) != CONFCH_YES)
/* # line 489 "tbledit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 491 "tbledit.qsc" */	/* host code */
/* # line 645 "tbledit.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(vnode_name) +'::' +TRIM(database_name)from dd_databases w\
here database_no=");
    IIputdomio((short *)0,1,30,2,&RMlocal_db_no);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME*2+3-1,local_db_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 650 "tbledit.sc" */	/* host code */
			if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
			{
/* # line 652 "tbledit.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 653 "tbledit.sc" */	/* host code */
				IIUGerr(E_RM0030_Err_retrieve_db_name,
					UG_ERR_ERROR, 0);
/* # line 501 "tbledit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 502 "tbledit.qsc" */	/* host code */
			}
			if (dropsupp(tbl_no) != OK)
/* # line 505 "tbledit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 506 "tbledit.qsc" */	/* host code */
			*supp_objs_created = *rules_created = EOS;
/* # line 507 "tbledit.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(fsupp_objs_created,(short *)0,1,32,0,
                supp_objs_created);
                IIputfldio(frules_created,(short *)0,1,32,0,rules_created);
              } /* IIfsetio */
            }
/* # line 509 "tbledit.qsc" */	/* host code */
		}
		if (tblsave(tbl_no, tbl_name) != OK)
		{
			IIUGerr(E_RM0009_Error_saving, UG_ERR_ERROR, 0);
/* # line 514 "tbledit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 515 "tbledit.qsc" */	/* host code */
		}
		*cdds_num = (i2)cdds_no;
		STcopy(new_cdds_name, cdds_name);
		STcopy(index_name, index_used);
		STcopy(cdds_lookup, cdds_lookup_tbl);
		STcopy(prio_lookup, prio_lookup_tbl);
/* # line 522 "tbledit.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 7) {
          {
/* # line 529 "tbledit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,objname,19,(char *)0,0);
              } /* IIiqset */
            }
/* # line 530 "tbledit.qsc" */	/* host code */
		*list_rows = EOS;
		if (STequal(objname, findex_used))
		{
/* # line 709 "tbledit.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name from iitables where table_name=");
    IIputdomio((short *)0,1,32,0,tbl_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
    IIputdomio((short *)0,1,32,0,tbl_owner);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and unique_rule='U' union select distinct index_name from iiindexes w\
here base_name=");
    IIputdomio((short *)0,1,32,0,tbl_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and index_owner=");
    IIputdomio((short *)0,1,32,0,tbl_owner);
    IIwritio(0,(short *)0,1,32,0,(char *)"and unique_rule='U' order by\
 1");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,index_name);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 723 "tbledit.sc" */	/* host code */
				STtrmwhite(index_name);
				STprintf(list_row, ERx("%s\n"), index_name);
				STcat(list_rows, list_row);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
/* # line 727 "tbledit.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 728 "tbledit.sc" */	/* host code */
			if (pick_table(list_rows, ERget(S_RM004B_Unique_keys),
				index_name))
			{
				set_table_keys(tbl_no, tbl_name, tbl_owner,
					index_name);
/* # line 557 "tbledit.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(findex_used,(short *)0,1,32,0,index_name);
              } /* IIfsetio */
            }
/* # line 558 "tbledit.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 559 "tbledit.qsc" */	/* host code */
			}
		}
		else if (STequal(objname, fcdds_no))
		{
/* # line 751 "tbledit.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select cdds_no, cdds_name from dd_cdds order by cdds_no");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&cdds_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,new_cdds_name);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 756 "tbledit.sc" */	/* host code */
				STtrmwhite(new_cdds_name);
				STprintf(list_row, ERx("%d;%s\n"),
					(i4)cdds_no, new_cdds_name);
				STcat(list_rows, list_row);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
/* # line 761 "tbledit.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 762 "tbledit.sc" */	/* host code */
			row = IIFDlpListPick(NULL, list_rows, 5, -1, -1, NULL,
				NULL, NULL, NULL);
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
				STcopy(pend, new_cdds_name);
				*p2 = EOS;
				CVan(p1, &i);
				cdds_no = (i2)i;
/* # line 592 "tbledit.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(fcdds_no,(short *)0,1,30,2,&cdds_no);
                IIputfldio(fcdds_name,(short *)0,1,32,0,new_cdds_name);
              } /* IIfsetio */
            }
/* # line 594 "tbledit.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 595 "tbledit.qsc" */	/* host code */
			}
		}
		else if (STequal(objname, fcdds_lookup_tbl))
		{
/* # line 799 "tbledit.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name from iicolumns c where c.column_name in('cdds_no', \
'CDDS_NO')and not exists(select * from dd_support_tables s where LOWER\
CASE(s.table_name)=LOWERCASE(c.table_name))order by table_name");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,lookup_tbl);
      if (IIerrtest() != 0) goto IIrtB4;
/* # line 810 "tbledit.sc" */	/* host code */
				STtrmwhite(lookup_tbl);
				STprintf(list_row, ERx("%s\n"), lookup_tbl);
				STcat(list_rows, list_row);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE4:;
  }
/* # line 814 "tbledit.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&n,8);
  }
/* # line 815 "tbledit.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 816 "tbledit.sc" */	/* host code */
			if (!n)
			{
				IIUGerr(E_RM0080_No_valid_choices,
					UG_ERR_ERROR, 0);
/* # line 620 "tbledit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 621 "tbledit.qsc" */	/* host code */
			}
			if (pick_table(list_rows,
				ERget(S_RM004C_Cdds_lookup_tables),
				lookup_tbl))
			{
/* # line 627 "tbledit.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(fcdds_lookup_tbl,(short *)0,1,32,0,lookup_tbl);
              } /* IIfsetio */
            }
/* # line 628 "tbledit.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 629 "tbledit.qsc" */	/* host code */
			}
		}
		else if (STequal(objname, fprio_lookup_tbl))
		{
/* # line 848 "tbledit.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name from iicolumns c where c.column_name in('dd_priorit\
y', 'DD_PRIORITY')and not exists(select * from dd_support_tables s whe\
re LOWERCASE(s.table_name)=LOWERCASE(c.table_name))order by table_name");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,lookup_tbl);
      if (IIerrtest() != 0) goto IIrtB5;
/* # line 860 "tbledit.sc" */	/* host code */
				STtrmwhite(lookup_tbl);
				STprintf(list_row, ERx("%s\n"), lookup_tbl);
				STcat(list_rows, list_row);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE5:;
  }
/* # line 864 "tbledit.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&n,8);
  }
/* # line 865 "tbledit.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 866 "tbledit.sc" */	/* host code */
			if (!n)
			{
				IIUGerr(E_RM0080_No_valid_choices,
					UG_ERR_ERROR, 0);
/* # line 655 "tbledit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 656 "tbledit.qsc" */	/* host code */
			}
			if (pick_table(list_rows,
				ERget(S_RM004D_Prio_lookup_tables),
				lookup_tbl))
			{
/* # line 662 "tbledit.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(fprio_lookup_tbl,(short *)0,1,32,0,lookup_tbl);
              } /* IIfsetio */
            }
/* # line 663 "tbledit.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 664 "tbledit.qsc" */	/* host code */
			}
		}
		else
		{
			IIUFfieldHelp();
		}
          }
        } else if (IIretval() == 8) {
          {
/* # line 674 "tbledit.qsc" */	/* host code */
		FEtabfnd(form_name, tf_name);
          }
        } else if (IIretval() == 9) {
          {
/* # line 679 "tbledit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,objname,19,(char *)0,0);
                IIiqfsio((short *)0,1,30,4,&in_tf,28,(char *)0,0);
              } /* IIiqset */
            }
/* # line 680 "tbledit.qsc" */	/* host code */
		if (!in_tf)
		{
			FTbell();
			IIUGerr(E_RM0048_Cursor_in_tf_for_top, UG_ERR_ERROR,
				0);
		}
		else
		{
/* # line 688 "tbledit.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 689 "tbledit.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 10) {
          {
/* # line 694 "tbledit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,objname,19,(char *)0,0);
                IIiqfsio((short *)0,1,30,4,&in_tf,28,(char *)0,0);
              } /* IIiqset */
            }
/* # line 695 "tbledit.qsc" */	/* host code */
		if (!in_tf)
		{
			FTbell();
			IIUGerr(E_RM0049_Cursor_in_tf_for_bot, UG_ERR_ERROR,
				0);
		}
		else
		{
/* # line 703 "tbledit.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 704 "tbledit.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 11) {
          {
/* # line 711 "tbledit.qsc" */	/* host code */
		FEhelp(ERx("rmtbledt.hlp"), ERget(F_RM0054_Register_columns));
          }
        } else if (IIretval() == 12) {
          {
/* # line 718 "tbledit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 719 "tbledit.qsc" */	/* host code */
		if (chng && IIUFccConfirmChoice(CONF_GENERIC, NULL, NULL,
			NULL, NULL, S_RM000E_End_no_save, S_RM000F_Yes_end,
			S_RM0010_No_end, ERx(""), TRUE) != CONFCH_YES)
/* # line 722 "tbledit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 724 "tbledit.qsc" */	/* breakdisplay */
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
      if (IIFRafActFld(fcdds_no,0,1) == 0) goto IIfdE1;
      if (IIFRafActFld(findex_used,0,2) == 0) goto IIfdE1;
      if (IIFRafActFld(fcdds_lookup_tbl,0,3) == 0) goto IIfdE1;
      if (IIFRafActFld(fprio_lookup_tbl,0,4) == 0) goto IIfdE1;
      if (IITBcaClmAct(tf_name,freplic_col,0,5) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Save),ERget(F_FE0117_SaveExpl),2,2,6) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,2,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Lookup),ERget(F_FE0116_LookupExpl),0,0,7) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,8) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,9) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,10) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,11) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,11) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,12) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,12) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
  }
/* # line 729 "tbledit.qsc" */	/* host code */
/*
** This procedure fills a local array (keys) with the key columns and resets
** the tablefield with new sequence numbers.  Also validates that no illegal
** (money/float) fields are being used
*/
static void
set_table_keys(
i4	tbl_no,
char	*tbl_name,
char	*tbl_owner,
char	*index_used)
# if 0
  i4 tbl_no;
  char *tbl_name;
  char *tbl_owner;
  char *index_used;
# endif
  {
  char col_name[DB_MAXNAME+1];
  char replic_col[2];
	i4	i;
	DBEC_ERR_INFO	errinfo;
    char *yes = ERget(F_UG0001_Yes1);
	i = 0;
	if (!STequal(index_used, tbl_name))
	{
/* # line 1063 "tbledit.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select d.column_name, c.key_sequence, LOWERCASE(TRIM(c.column_datatyp\
e))from iiindexes i, iicolumns c, dd_regist_columns d where c.column_n\
ame=d.column_name and c.table_name=");
    IIputdomio((short *)0,1,32,0,index_used);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and c.table_owner=i.base_owner and d.table_no=");
    IIputdomio((short *)0,1,30,4,&tbl_no);
    IIwritio(0,(short *)0,1,32,0,(char *)"and i.base_name=");
    IIputdomio((short *)0,1,32,0,tbl_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and i.base_owner=");
    IIputdomio((short *)0,1,32,0,tbl_owner);
    IIwritio(0,(short *)0,1,32,0,(char *)"and i.index_name=");
    IIputdomio((short *)0,1,32,0,index_used);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,(keys[i]).col_name);
      IIgetdomio((short *)0,1,30,4,&(keys[i]).key_seq);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,(keys[i]).datatype);
      if (IIerrtest() != 0) goto IIrtB6;
/* # line 1075 "tbledit.sc" */	/* host code */
			STtrmwhite(keys[i].col_name);
			++i;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE6:;
  }
	}
	else
	{
/* # line 1081 "tbledit.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select d.column_name, c.key_sequence, LOWERCASE(TRIM(c.column_datatyp\
e))from dd_regist_columns d, dd_regist_tables t, iicolumns c where d.t\
able_no=");
    IIputdomio((short *)0,1,30,4,&tbl_no);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and d.table_no=t.table_no and d.column_name=c.column_name and t.table\
_name=c.table_name and t.table_owner=c.table_owner and c.key_sequence>\
0");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE7;
IIrtB7:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,(keys[i]).col_name);
      IIgetdomio((short *)0,1,30,4,&(keys[i]).key_seq);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,(keys[i]).datatype);
      if (IIerrtest() != 0) goto IIrtB7;
/* # line 1093 "tbledit.sc" */	/* host code */
			STtrmwhite(keys[i].col_name);
			++i;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE7:;
  }
	}
	RPdb_error_check(0, &errinfo);
	if (errinfo.errorno)
	{
/* # line 1100 "tbledit.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 1101 "tbledit.sc" */	/* host code */
		FTbell();
		IIUGerr(E_RM004A_Error_select_keys, UG_ERR_ERROR, 0);
		return;
	}
	else if (errinfo.rowcount == 0)
	{
/* # line 1107 "tbledit.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 1108 "tbledit.sc" */	/* host code */
		IIUGerr(E_RM0047_No_keys_found, UG_ERR_ERROR, 0);
		return;
	}
	/* Check for invalid key datatypes */
	for (i = 0; i < errinfo.rowcount; ++i)
	{
		if (keys[i].key_seq > 0 &&
			(STequal(keys[i].datatype, ERx("money")) ||
			STequal(keys[i].datatype, ERx("float"))))
		{
			IIUGerr(E_RM0081_Invalid_key_type, UG_ERR_ERROR, 0);
			keys[i].key_seq = 0;
			return;
		}
	}
/* # line 822 "tbledit.qsc" */	/* unloadtable */
    {
      if (IItbact(form_name,tf_name,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,col_name,fcol_name);
        IItcogetio((short *)0,1,32,1,replic_col,freplic_col);
        IITBceColEnd();
        {
/* # line 825 "tbledit.qsc" */	/* host code */
		if (!CMcmpcase(replic_col, yes))
/* # line 826 "tbledit.qsc" */	/* putrow */
          {
            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
              IItcoputio(fkey_seq,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 827 "tbledit.qsc" */	/* host code */
		for (i = 0; i < errinfo.rowcount; ++i)
		{
			if (STequal(keys[i].col_name, col_name))
			{
/* # line 831 "tbledit.qsc" */	/* putrow */
          {
            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
              IItcoputio(fkey_seq,(short *)0,1,30,4,&keys[i].key_seq);
              IItcoputio(freplic_col,(short *)0,1,32,0,yes);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 834 "tbledit.qsc" */	/* host code */
			}
		}
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
  }
/* # line 840 "tbledit.qsc" */	/* host code */
static bool
pick_table(
char	*list_rows,
char	*title,
char	*tbl_name)
{
	i4	i;
	i4	row;
	char	*p1, *pend;
	row = IIFDlpListPick(title, list_rows, 5, -1, -1, NULL, NULL, NULL,
		NULL);
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
		STcopy(p1, tbl_name);
		return (TRUE);
	}
	return (FALSE);
}
