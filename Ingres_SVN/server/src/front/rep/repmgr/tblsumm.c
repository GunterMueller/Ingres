# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <st.h>
# include <er.h>
# include <tm.h>
# include <pc.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <erfe.h>
# include <uf.h>
# include <ug.h>
# include <rpu.h>
# include <tbldef.h>
# include <targtype.h>
# include "errm.h"
/**
** Name:	tblsumm.qsc - table registration summary
**
** Description:
**	Defines
**		tbl_summ	- table registration summary
**
** History:
**	16-dec-96 (joea)
**		Created based on tblcatlg.osq in replicator library.
**	19-may-97 (joea) bug 82149
**		Deal with tbl_rules() returning -1 (no need to activate).
**	25-sep-97 (joea)
**		Remove unused argument to tbl_deactivate.
**	14-oct-97 (joea) bug 83765
**		Remove unused argument to tbl_rules. After calling RMtbl_fetch,
**		do not create support objects if the target type is URO.
**	18-may-98 (padni01) bug 89865
**		Use date_gmt instead of date to set supp_objs_created field 
**		of dd_regist_tables.
**	24-aug-98 (abbjo03)
**		Add FRSkey4 (Go) to Register and ThisTable options.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
GLOBALREF
  char RMdb_display[];
GLOBALREF TBLDEF	*RMtbl;
GLOBALREF
  i2 RMlocal_db_no;
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmtbl_summary"), *tf_name = ERx("iitf"), *ftbl_no = ERx("table_no")
  , *ftbl_name = ERx("table_name"), *ftbl_owner = ERx("table_owner"), *
  fcdds_no = ERx("cdds_no"), *fcdds_name = ERx("cdds_name"), *fcolumns = ERx("columns")
  , *fsupport = ERx("support"), *frules = ERx("rules"), *fcols_registd = ERx("cols_registd")
  , *fsupp_objs_created = ERx("supp_objs_created"), *frules_created = ERx("rules_created")
  , *findex_used = ERx("index_used"), *fcdds_lookup_tbl = ERx("cdds_lookup_tbl")
  , *fprio_lookup_tbl = ERx("prio_lookup_tbl"), *funique_rule = ERx("unique_rule")
  , *ftbl_indexes = ERx("tbl_indexes"), *ferror_no = ERx("error_no"), *blank = ERx("")
  ;
FUNC_EXTERN STATUS tbl_register(i4 *tbl_no, char *tbl_name, char *tbl_owner,
	i2 cdds_no, char *unique_rule, char *tbl_indexes,
	char *columns_registered, char *index_used);
FUNC_EXTERN STATUS deregister(i4 *tbl_no, char *full_db_name);
FUNC_EXTERN void tbl_edit(i4 *tbl_no, char *tbl_name, char *tbl_owner,
	i2 *cdds_num, char *cdds_name, char *cdds_lookup_tbl,
	char *prio_lookup_tbl, char *index_used, char *columns_registered,
	char *supp_objs_created, char *rules_created, char *unique_rule,
	char *tbl_indexes);
FUNC_EXTERN STATUS create_support_tables(i4 tbl_no);
FUNC_EXTERN STATUS tbl_dbprocs(i4 tbl_no);
FUNC_EXTERN STATUS create_replication_keys(i4 tbl_no, bool queue_flag);
static void display_errors(void);
/*{
** Name:	tbl_summ	- table registration summary
**
** Description:
**	Initial registration of tables for replication
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
void
tbl_summ()
  {
  i4 tbl_no;
  char tbl_name[DB_MAXNAME+1];
  char tbl_owner[DB_MAXNAME+1];
  i2 cdds_no;
  char cdds_name[DB_MAXNAME+1];
  char cols_registd[26];
  char supp_objs_created[26];
  char rules_created[26];
  char columns[4];
  char support[4];
  char rules[4];
  char index_used[DB_MAXNAME+1];
  char cdds_lookup_tbl[DB_MAXNAME+1];
  char prio_lookup_tbl[DB_MAXNAME+1];
  char unique_rule[2];
  char tbl_indexes[2];
  char local_db_name[DB_MAXNAME*2+3];
  i4 err_no;
  char timestamp[26];
    i4 rownumber;
	DBEC_ERR_INFO	errinfo;
    char *yes = ERget(F_RM0038_yes);
    char *err = ERget(F_RM003E_err);
	i4	nerrs;
	SYSTIME	stime;
	struct TMhuman	now;
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
/* # line 135 "tblsumm.sc" */	/* select */
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
/* # line 139 "tblsumm.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
	{
		IIUGerr(E_RM0030_Err_retrieve_db_name, UG_ERR_ERROR, 0);
		return;
	}
/* # line 168 "tblsumm.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"r") != 0) {
        IIthidecol(ftbl_no,(char *)"i4 not null");
        IIthidecol(fcdds_no,(char *)"i2 not null");
        IIthidecol(fcols_registd,(char *)"char(25) not null");
        IIthidecol(fsupp_objs_created,(char *)"char(25) not null");
        IIthidecol(frules_created,(char *)"char(25) not null");
        IIthidecol(findex_used,(char *)"char(32) not null");
        IIthidecol(fcdds_lookup_tbl,(char *)"char(32) not null");
        IIthidecol(fprio_lookup_tbl,(char *)"char(32) not null");
        IIthidecol(funique_rule,(char *)"char(1) not null");
        IIthidecol(ftbl_indexes,(char *)"char(1) not null");
        IIthidecol(ferror_no,(char *)"i4 not null");
        IItfill();
      } /* IItbinit */
    }
/* # line 177 "tblsumm.qsc" */	/* host code */
	IIUGmsg(ERget(F_RM001C_Selecting_data), FALSE, 0);
	/*
	** FIXME: unique_rule and table_indexes are only used by tbl_register
	**	so they could be retrieved there only when needed.
	*/
/* # line 167 "tblsumm.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select t.table_no, t.table_name, t.table_owner, t.cdds_no, c.cdds_nam\
e, t.columns_registered, t.supp_objs_created, t.rules_created, t.index\
_used, t.cdds_lookup_table, t.prio_lookup_table, i.unique_rule, i.tabl\
e_indexes from iitables i, dd_regist_tables t");
    IIwritio(0,(short *)0,1,32,0,(char *)
", dd_cdds c where i.table_name=t.table_name and i.table_owner=t.table\
_owner and i.table_type='T' and c.cdds_no=t.cdds_no union select 0, i.\
table_name, i.table_owner, 0, c.cdds_name, '', '', '', '', '', '', i.u\
nique_rule, i.table_indexes from iitables i, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"dd_cdds c where i.system_use!='S' and i.table_name not like 'ii%' and\
 i.table_type='T' and not exists(select table_name from dd_regist_tabl\
es t where t.table_name=i.table_name and t.table_owner=i.table_owner)a\
nd not exists(select table_name from ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"dd_support_tables s where LOWERCASE(s.table_name)=LOWERCASE(i.table_n\
ame))and c.cdds_no=0 order by 2, 3");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tbl_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,tbl_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,tbl_owner);
      IIgetdomio((short *)0,1,30,2,&cdds_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,cdds_name);
      IIgetdomio((short *)0,1,32,25,cols_registd);
      IIgetdomio((short *)0,1,32,25,supp_objs_created);
      IIgetdomio((short *)0,1,32,25,rules_created);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,index_used);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,cdds_lookup_tbl);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,prio_lookup_tbl);
      IIgetdomio((short *)0,1,32,1,unique_rule);
      IIgetdomio((short *)0,1,32,1,tbl_indexes);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 202 "tblsumm.sc" */	/* host code */
		STtrmwhite(tbl_name);
		STtrmwhite(tbl_owner);
		STtrmwhite(cols_registd);
		STtrmwhite(supp_objs_created);
		STtrmwhite(rules_created);
		*columns = *support = *rules = EOS;
		if (*cols_registd != EOS)
			STcopy(yes, columns);
		if (*supp_objs_created != EOS)
			STcopy(yes, support);
		if (*rules_created != EOS)
			STcopy(yes, rules);
/* # line 229 "tblsumm.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(ftbl_no,(short *)0,1,30,4,&tbl_no);
        IItcoputio(ftbl_name,(short *)0,1,32,0,tbl_name);
        IItcoputio(ftbl_owner,(short *)0,1,32,0,tbl_owner);
        IItcoputio(fcdds_no,(short *)0,1,30,2,&cdds_no);
        IItcoputio(fcdds_name,(short *)0,1,32,0,cdds_name);
        IItcoputio(fcols_registd,(short *)0,1,32,0,cols_registd);
        IItcoputio(fsupp_objs_created,(short *)0,1,32,0,supp_objs_created);
        IItcoputio(frules_created,(short *)0,1,32,0,rules_created);
        IItcoputio(fcolumns,(short *)0,1,32,0,columns);
        IItcoputio(fsupport,(short *)0,1,32,0,support);
        IItcoputio(frules,(short *)0,1,32,0,rules);
        IItcoputio(findex_used,(short *)0,1,32,0,index_used);
        IItcoputio(fcdds_lookup_tbl,(short *)0,1,32,0,cdds_lookup_tbl);
        IItcoputio(fprio_lookup_tbl,(short *)0,1,32,0,prio_lookup_tbl);
        IItcoputio(funique_rule,(short *)0,1,32,0,unique_rule);
        IItcoputio(ftbl_indexes,(short *)0,1,32,0,tbl_indexes);
        IItcoputio(ferror_no,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 241 "tblsumm.qsc" */	/* host code */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	RPdb_error_check(0, &errinfo);
	if (errinfo.errorno)
	{
/* # line 242 "tblsumm.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 243 "tblsumm.sc" */	/* host code */
		FTbell();
		IIUGerr(E_RM0012_Error_selecting, UG_ERR_ERROR, 0);
		return;
	}
	else if (errinfo.rowcount == 0)
	{
/* # line 249 "tblsumm.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 250 "tblsumm.sc" */	/* host code */
		IIUGerr(E_RM0013_No_data_found, UG_ERR_ERROR, 0);
		return;
	}
/* # line 253 "tblsumm.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 254 "tblsumm.sc" */	/* host code */
/* # line 258 "tblsumm.qsc" */	/* display */
    {
      if (IIdispfrm(form_name,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio((char *)"db",(short *)0,1,32,0,RMdb_display);
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 263 "tblsumm.qsc" */	/* display */
            {
              if (IInestmu() == 0) goto IIfdE2;
              goto IImuI2;
IIfdI2:;
              if (IIfsetio((char *)0) == 0) goto IIfdE2;
              {
/* # line 266 "tblsumm.qsc" */	/* resume */
                {
                  IIresmu();
                  if (1) goto IIfdB2;
                }
              }
IIfdB2:
              while (IIrunnest() != 0) {
                if (IIretval() == 1) {
                  {
/* # line 272 "tblsumm.qsc" */	/* getrow */
                    {
                      if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                        IItcogetio((short *)0,1,32,3,columns,fcolumns);
                        IItcogetio((short *)0,1,30,4,&tbl_no,ftbl_no);
                        IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,tbl_name,
                        ftbl_name);
                        IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,tbl_owner,
                        ftbl_owner);
                        IItcogetio((short *)0,1,30,2,&cdds_no,fcdds_no);
                        IItcogetio((short *)0,1,32,1,unique_rule,funique_rule);
                        IItcogetio((short *)0,1,32,1,tbl_indexes,ftbl_indexes);
                        IItcogetio((short *)0,1,32,25,cols_registd,
                        fcols_registd);
                        IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,index_used,
                        findex_used);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 279 "tblsumm.qsc" */	/* host code */
			if (STequal(columns, yes))
			{
				IIUGerr(E_RM0031_Tbl_already_regist,
					UG_ERR_ERROR, 0);
/* # line 283 "tblsumm.qsc" */	/* resume */
                    {
                      if (1) goto IIfdB2;
                    }
/* # line 284 "tblsumm.qsc" */	/* host code */
			}
			err_no = tbl_register(&tbl_no, tbl_name, tbl_owner,
				cdds_no, unique_rule, tbl_indexes,
				cols_registd, index_used);
			if (err_no)
/* # line 289 "tblsumm.qsc" */	/* putrow */
                    {
                      if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                        IItcoputio(ftbl_no,(short *)0,1,30,sizeof(0),
                        (void *)IILQint(0));
                        IItcoputio(fcolumns,(short *)0,1,32,0,err);
                        IItcoputio(ferror_no,(short *)0,1,30,4,&err_no);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 291 "tblsumm.qsc" */	/* host code */
			else
/* # line 292 "tblsumm.qsc" */	/* putrow */
                    {
                      if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                        IItcoputio(ftbl_no,(short *)0,1,30,4,&tbl_no);
                        IItcoputio(fcolumns,(short *)0,1,32,0,yes);
                        IItcoputio(fcols_registd,(short *)0,1,32,0,
                        cols_registd);
                        IItcoputio(findex_used,(short *)0,1,32,0,index_used);
                        IItcoputio(ferror_no,(short *)0,1,30,4,&err_no);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 297 "tblsumm.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE2;
                    }
                  }
                } else if (IIretval() == 2) {
                  {
/* # line 303 "tblsumm.qsc" */	/* unloadtable */
                    {
                      if (IItbact(form_name,tf_name,0) == 0) goto IItbE1;
                      while (IItunload() != 0) {
                        IItcogetio((short *)0,1,32,3,columns,fcolumns);
                        IItcogetio((short *)0,1,30,4,&tbl_no,ftbl_no);
                        IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,tbl_name,
                        ftbl_name);
                        IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,tbl_owner,
                        ftbl_owner);
                        IItcogetio((short *)0,1,30,2,&cdds_no,fcdds_no);
                        IItcogetio((short *)0,1,32,1,unique_rule,funique_rule);
                        IItcogetio((short *)0,1,32,1,tbl_indexes,ftbl_indexes);
                        IItcogetio((short *)0,1,32,25,cols_registd,
                        fcols_registd);
                        IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,index_used,
                        findex_used);
                        IItcogetio((short *)0,1,30,4,&rownumber,(char *)
"_RECORD");
                        IITBceColEnd();
                        {
/* # line 311 "tblsumm.qsc" */	/* scroll */
                          {
                            if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                              IItbsmode((char *)"to");
                              if (IItscroll(0,rownumber) != 0) {
                              } /* IItscroll */
                            } /* IItbsetio */
                          }
/* # line 312 "tblsumm.qsc" */	/* redisplay */
                          {
                            IIredisp();
                          }
/* # line 313 "tblsumm.qsc" */	/* host code */
				if (!STequal(columns, yes))
				{
					err_no = tbl_register(&tbl_no,
						tbl_name, tbl_owner, cdds_no,
						unique_rule, tbl_indexes,
						cols_registd, index_used);
					if (err_no)
/* # line 320 "tblsumm.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(ftbl_no,(short *)0,1,30,sizeof(0),
                              (void *)IILQint(0));
                              IItcoputio(fcolumns,(short *)0,1,32,0,err);
                              IItcoputio(ferror_no,(short *)0,1,30,4,&err_no);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
/* # line 324 "tblsumm.qsc" */	/* host code */
					else
/* # line 325 "tblsumm.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(ftbl_no,(short *)0,1,30,4,&tbl_no);
                              IItcoputio(fcolumns,(short *)0,1,32,0,yes);
                              IItcoputio(fcols_registd,(short *)0,1,32,0,
                              cols_registd);
                              IItcoputio(findex_used,(short *)0,1,32,0,
                              index_used);
                              IItcoputio(ferror_no,(short *)0,1,30,4,&err_no);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
/* # line 333 "tblsumm.qsc" */	/* host code */
				}
                        }
                      } /* IItunload */
IItbE1:
                      IItunend();
                    }
/* # line 336 "tblsumm.qsc" */	/* scroll */
                    {
                      if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                        IItbsmode((char *)"to");
                        if (IItscroll(0,1) != 0) {
                        } /* IItscroll */
                      } /* IItbsetio */
                    }
/* # line 337 "tblsumm.qsc" */	/* host code */
			display_errors();
/* # line 338 "tblsumm.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE2;
                    }
                  }
                } else if (IIretval() == 3) {
                  {
/* # line 344 "tblsumm.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE2;
                    }
                  }
                } else {
                  if (1) goto IIfdE2;
                } /* IIretval */
              } /* IIrunnest */
IIfdF2:
              if (IIchkfrm() == 0) goto IIfdB2;
              goto IIfdE2;
IImuI2:
              if (IIinitmu() == 0) goto IIfdE2;
              if (IInmuact(ERget(F_RM003C_ThisTable),
              ERget(F_RM003D_ThisTable_reg_expl),2,2,1) == 0) goto IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE2;
              if (IInmuact(ERget(F_RM003F_AllTables),
              ERget(F_RM0040_AllTables_reg_expl),2,2,2) == 0) goto IIfdE2;
              if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),2,2,3) 
              == 0) goto IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(9,(char *)0,2,2,3) == 0) goto IIfdE2;
              if (IIendmu() == 0) goto IIfdE2;
              goto IIfdI2;
IIfdE2:;
              IIendnest();
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 351 "tblsumm.qsc" */	/* display */
            {
              if (IInestmu() == 0) goto IIfdE3;
              goto IImuI3;
IIfdI3:;
              if (IIfsetio((char *)0) == 0) goto IIfdE3;
              {
/* # line 354 "tblsumm.qsc" */	/* resume */
                {
                  IIresmu();
                  if (1) goto IIfdB3;
                }
              }
IIfdB3:
              while (IIrunnest() != 0) {
                if (IIretval() == 1) {
                  {
/* # line 360 "tblsumm.qsc" */	/* getrow */
                    {
                      if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                        IItcogetio((short *)0,1,30,4,&tbl_no,ftbl_no);
                        IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,tbl_name,
                        ftbl_name);
                        IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,tbl_owner,
                        ftbl_owner);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 362 "tblsumm.qsc" */	/* host code */
			IIUGmsg(ERget(F_RM0044_Deregistering), FALSE, 2,
				tbl_owner, tbl_name);
			err_no = deregister(&tbl_no, local_db_name);
			if (err_no == 0)
/* # line 366 "tblsumm.qsc" */	/* putrow */
                    {
                      if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                        IItcoputio(ftbl_no,(short *)0,1,30,sizeof(0),
                        (void *)IILQint(0));
                        IItcoputio(fcolumns,(short *)0,1,32,0,blank);
                        IItcoputio(fsupport,(short *)0,1,32,0,blank);
                        IItcoputio(frules,(short *)0,1,32,0,blank);
                        IItcoputio(fcols_registd,(short *)0,1,32,0,blank);
                        IItcoputio(fsupp_objs_created,(short *)0,1,32,0,blank);
                        IItcoputio(frules_created,(short *)0,1,32,0,blank);
                        IItcoputio(ferror_no,(short *)0,1,30,4,&err_no);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 372 "tblsumm.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE3;
                    }
                  }
                } else if (IIretval() == 2) {
                  {
/* # line 378 "tblsumm.qsc" */	/* host code */
			nerrs = 0;
/* # line 379 "tblsumm.qsc" */	/* unloadtable */
                    {
                      if (IItbact(form_name,tf_name,0) == 0) goto IItbE2;
                      while (IItunload() != 0) {
                        IItcogetio((short *)0,1,30,4,&tbl_no,ftbl_no);
                        IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,tbl_name,
                        ftbl_name);
                        IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,tbl_owner,
                        ftbl_owner);
                        IItcogetio((short *)0,1,32,3,columns,fcolumns);
                        IItcogetio((short *)0,1,32,3,support,fsupport);
                        IItcogetio((short *)0,1,30,4,&rownumber,(char *)
"_RECORD");
                        IITBceColEnd();
                        {
/* # line 384 "tblsumm.qsc" */	/* scroll */
                          {
                            if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                              IItbsmode((char *)"to");
                              if (IItscroll(0,rownumber) != 0) {
                              } /* IItscroll */
                            } /* IItbsetio */
                          }
/* # line 385 "tblsumm.qsc" */	/* redisplay */
                          {
                            IIredisp();
                          }
/* # line 386 "tblsumm.qsc" */	/* host code */
				if (STequal(columns, yes) ||
					STequal(support, yes))
				{
					IIUGmsg(ERget(F_RM0044_Deregistering),
						FALSE, 2, tbl_owner, tbl_name);
					err_no = deregister(&tbl_no,
						local_db_name);
					if (err_no == 0)
					{
/* # line 395 "tblsumm.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(ftbl_no,(short *)0,1,30,sizeof(0),
                              (void *)IILQint(0));
                              IItcoputio(fcolumns,(short *)0,1,32,0,blank);
                              IItcoputio(fsupport,(short *)0,1,32,0,blank);
                              IItcoputio(frules,(short *)0,1,32,0,blank);
                              IItcoputio(fcols_registd,(short *)0,1,32,0,blank);
                              IItcoputio(fsupp_objs_created,(short *)0,1,32,0,
                              blank);
                              IItcoputio(frules_created,(short *)0,1,32,0,
                              blank);
                              IItcoputio(ferror_no,(short *)0,1,30,4,&err_no);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
/* # line 405 "tblsumm.qsc" */	/* host code */
					}
					else
					{
/* # line 408 "tblsumm.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(ftbl_no,(short *)0,1,30,4,&tbl_no);
                              IItcoputio(ferror_no,(short *)0,1,30,4,&err_no);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
/* # line 411 "tblsumm.qsc" */	/* host code */
						++nerrs;
					}
				}
                        }
                      } /* IItunload */
IItbE2:
                      IItunend();
                    }
/* # line 415 "tblsumm.qsc" */	/* scroll */
                    {
                      if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                        IItbsmode((char *)"to");
                        if (IItscroll(0,1) != 0) {
                        } /* IItscroll */
                      } /* IItbsetio */
                    }
/* # line 416 "tblsumm.qsc" */	/* host code */
			if (nerrs == 0)
			{
/* # line 612 "tblsumm.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_last_number set last_number=0 where column_name='table_no'");
    IIsyncup((char *)0,0);
  }
/* # line 615 "tblsumm.sc" */	/* host code */
				if (RPdb_error_check(DBEC_SINGLE_ROW, NULL)
					!= OK)
				{
/* # line 618 "tblsumm.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 619 "tblsumm.sc" */	/* host code */
					IIUGerr(E_RM0032_Err_updt_last_tbl_no,
						UG_ERR_ERROR, 0);
/* # line 427 "tblsumm.qsc" */	/* resume */
                    {
                      if (1) goto IIfdB3;
                    }
/* # line 428 "tblsumm.qsc" */	/* host code */
				}
/* # line 627 "tblsumm.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 628 "tblsumm.sc" */	/* host code */
			}
/* # line 432 "tblsumm.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE3;
                    }
                  }
                } else if (IIretval() == 3) {
                  {
/* # line 438 "tblsumm.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE3;
                    }
                  }
                } else {
                  if (1) goto IIfdE3;
                } /* IIretval */
              } /* IIrunnest */
IIfdF3:
              if (IIchkfrm() == 0) goto IIfdB3;
              goto IIfdE3;
IImuI3:
              if (IIinitmu() == 0) goto IIfdE3;
              if (IInmuact(ERget(F_RM003C_ThisTable),
              ERget(F_RM0043_ThisTable_dereg_expl),2,2,1) == 0) goto IIfdE3;
              IIFRInternal(0);
              if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE3;
              if (IInmuact(ERget(F_RM003F_AllTables),
              ERget(F_RM0045_AllTables_dereg_expl),2,2,2) == 0) goto IIfdE3;
              if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),2,2,3) 
              == 0) goto IIfdE3;
              IIFRInternal(0);
              if (IInfrskact(9,(char *)0,2,2,3) == 0) goto IIfdE3;
              if (IIendmu() == 0) goto IIfdE3;
              goto IIfdI3;
IIfdE3:;
              IIendnest();
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 445 "tblsumm.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&tbl_no,ftbl_no);
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,tbl_name,ftbl_name);
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,tbl_owner,ftbl_owner);
                IItcogetio((short *)0,1,30,2,&cdds_no,fcdds_no);
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,cdds_name,fcdds_name);
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,cdds_lookup_tbl,
                fcdds_lookup_tbl);
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,prio_lookup_tbl,
                fprio_lookup_tbl);
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,index_used,
                findex_used);
                IItcogetio((short *)0,1,32,25,cols_registd,fcols_registd);
                IItcogetio((short *)0,1,32,25,supp_objs_created,
                fsupp_objs_created);
                IItcogetio((short *)0,1,32,25,rules_created,frules_created);
                IItcogetio((short *)0,1,32,1,unique_rule,funique_rule);
                IItcogetio((short *)0,1,32,1,tbl_indexes,ftbl_indexes);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 455 "tblsumm.qsc" */	/* host code */
		tbl_edit(&tbl_no, tbl_name, tbl_owner, &cdds_no, cdds_name,
			cdds_lookup_tbl, prio_lookup_tbl, index_used,
			cols_registd, supp_objs_created, rules_created,
			unique_rule, tbl_indexes);
		*columns = *support = *rules = EOS;
		if (*cols_registd != EOS)
			STcopy(yes, columns);
		if (*supp_objs_created != EOS)
			STcopy(yes, support);
		if (*rules_created != EOS)
			STcopy(yes, rules);
/* # line 468 "tblsumm.qsc" */	/* putrow */
            {
              if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                IItcoputio(ftbl_no,(short *)0,1,30,4,&tbl_no);
                IItcoputio(fcdds_no,(short *)0,1,30,2,&cdds_no);
                IItcoputio(fcdds_name,(short *)0,1,32,0,cdds_name);
                IItcoputio(fcdds_lookup_tbl,(short *)0,1,32,0,cdds_lookup_tbl);
                IItcoputio(fprio_lookup_tbl,(short *)0,1,32,0,prio_lookup_tbl);
                IItcoputio(findex_used,(short *)0,1,32,0,index_used);
                IItcoputio(fcols_registd,(short *)0,1,32,0,cols_registd);
                IItcoputio(fsupp_objs_created,(short *)0,1,32,0,
                supp_objs_created);
                IItcoputio(frules_created,(short *)0,1,32,0,rules_created);
                IItcoputio(fcolumns,(short *)0,1,32,0,columns);
                IItcoputio(fsupport,(short *)0,1,32,0,support);
                IItcoputio(frules,(short *)0,1,32,0,rules);
                IITBceColEnd();
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 481 "tblsumm.qsc" */	/* display */
            {
              if (IInestmu() == 0) goto IIfdE4;
              goto IImuI4;
IIfdI4:;
              if (IIfsetio((char *)0) == 0) goto IIfdE4;
              {
/* # line 484 "tblsumm.qsc" */	/* resume */
                {
                  IIresmu();
                  if (1) goto IIfdB4;
                }
              }
IIfdB4:
              while (IIrunnest() != 0) {
                if (IIretval() == 1) {
                  {
/* # line 490 "tblsumm.qsc" */	/* getrow */
                    {
                      if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                        IItcogetio((short *)0,1,30,4,&tbl_no,ftbl_no);
                        IItcogetio((short *)0,1,32,3,columns,fcolumns);
                        IItcogetio((short *)0,1,32,25,supp_objs_created,
                        fsupp_objs_created);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 494 "tblsumm.qsc" */	/* host code */
			if (*columns == EOS)
			{
				IIUGerr(E_RM0033_Must_reg_before_supp,
					UG_ERR_ERROR, 0);
				err_no = 1;
			}
			else if (STequal(columns, err))
			{
				IIUGerr(E_RM0034_Correct_reg_errors,
					UG_ERR_ERROR, 0);
				err_no = 1;
			}
			else		/* columns = "yes" */
			{
				TMnow(&stime);
				TMbreak(&stime, &now);
				STprintf(timestamp, ERx("%s-%s-%s %s:%s:%s"),
					now.day, now.month, now.year, now.hour,
					now.mins, now.sec);
/* # line 513 "tblsumm.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 515 "tblsumm.qsc" */	/* host code */
				/* Fetch complete table definition */
				err_no = RMtbl_fetch(tbl_no, TRUE);
				if (err_no == 0 &&
					RMtbl->target_type != TARG_UNPROT_READ)
				{
					err_no = create_support_tables(tbl_no);
					if (err_no <= 0)
					{
						err_no = tbl_dbprocs(tbl_no);
						if (err_no == 0)
						{
/* # line 526 "tblsumm.qsc" */	/* putrow */
                    {
                      if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                        IItcoputio(fsupp_objs_created,(short *)0,1,32,0,
                        timestamp);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 530 "tblsumm.qsc" */	/* host code */
/* # line 797 "tblsumm.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_tables set supp_objs_created=DATE_GMT(");
    IIputdomio((short *)0,1,32,0,timestamp);
    IIwritio(0,(short *)0,1,32,0,(char *)")where table_no=");
    IIputdomio((short *)0,1,30,4,&tbl_no);
    IIsyncup((char *)0,0);
  }
/* # line 800 "tblsumm.sc" */	/* host code */
							err_no = RPdb_error_check(
								DBEC_SINGLE_ROW,
								NULL);
						}
					}
				}
			}
			if (err_no <= 0 &&
				RMtbl->target_type != TARG_UNPROT_READ)
			{
				if (db_config_changed(0) != OK)
				{
/* # line 812 "tblsumm.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 813 "tblsumm.sc" */	/* host code */
					err_no = 1;
				}
				else
				{
/* # line 817 "tblsumm.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 818 "tblsumm.sc" */	/* host code */
				}
/* # line 552 "tblsumm.qsc" */	/* putrow */
                    {
                      if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                        IItcoputio(fsupport,(short *)0,1,32,0,yes);
                        IItcoputio((char *)"error_no",(short *)0,1,30,4,&
                        err_no);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 554 "tblsumm.qsc" */	/* host code */
			}
			else
			{
/* # line 557 "tblsumm.qsc" */	/* putrow */
                    {
                      if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                        IItcoputio(fsupport,(short *)0,1,32,0,err);
                        IItcoputio(ferror_no,(short *)0,1,30,4,&err_no);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 559 "tblsumm.qsc" */	/* host code */
			}
/* # line 560 "tblsumm.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 561 "tblsumm.qsc" */	/* redisplay */
                    {
                      IIredisp();
                    }
/* # line 562 "tblsumm.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE4;
                    }
                  }
                } else if (IIretval() == 2) {
                  {
/* # line 568 "tblsumm.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 569 "tblsumm.qsc" */	/* unloadtable */
                    {
                      if (IItbact(form_name,tf_name,0) == 0) goto IItbE3;
                      while (IItunload() != 0) {
                        IItcogetio((short *)0,1,30,4,&tbl_no,ftbl_no);
                        IItcogetio((short *)0,1,32,3,columns,fcolumns);
                        IItcogetio((short *)0,1,32,25,supp_objs_created,
                        fsupp_objs_created);
                        IItcogetio((short *)0,1,30,4,&rownumber,(char *)
"_RECORD");
                        IITBceColEnd();
                        {
/* # line 574 "tblsumm.qsc" */	/* host code */
				if (STequal(columns, err))
				{
					IIUGmsg(ERget(
						E_RM0034_Correct_reg_errors),
						FALSE, 0);
					PCsleep(2000);
					err_no = 1;
				}
				else if (STequal(columns, yes))
				{
					TMnow(&stime);
					TMbreak(&stime, &now);
					STprintf(timestamp,
						ERx("%s-%s-%s %s:%s:%s"),
						now.day, now.month, now.year,
						now.hour, now.mins, now.sec);
					/* Fetch complete table definition */
					err_no = RMtbl_fetch(tbl_no, TRUE);
					if (err_no == 0 && RMtbl->target_type
						!= TARG_UNPROT_READ)
					{
						err_no = create_support_tables(
							tbl_no);
						if (err_no <= 0)
						{
							err_no = tbl_dbprocs(
								tbl_no);
							if (err_no == 0)
							{
/* # line 603 "tblsumm.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(fsupp_objs_created,(short *)0,1,32,0,
                              timestamp);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
/* # line 607 "tblsumm.qsc" */	/* host code */
/* # line 912 "tblsumm.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_tables set supp_objs_created=DATE_GMT(");
    IIputdomio((short *)0,1,32,0,timestamp);
    IIwritio(0,(short *)0,1,32,0,(char *)")where table_no=");
    IIputdomio((short *)0,1,30,4,&tbl_no);
    IIsyncup((char *)0,0);
  }
/* # line 915 "tblsumm.sc" */	/* host code */
								err_no = RPdb_error_check(
									DBEC_SINGLE_ROW,
									NULL);
							}
						}
					}
					if (err_no <= 0 && RMtbl->target_type
						!= TARG_UNPROT_READ)
					{
/* # line 619 "tblsumm.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(fsupport,(short *)0,1,32,0,yes);
                              IItcoputio(ferror_no,(short *)0,1,30,4,&err_no);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
/* # line 622 "tblsumm.qsc" */	/* host code */
						if (db_config_changed(0) != OK)
/* # line 934 "tblsumm.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 935 "tblsumm.sc" */	/* host code */
						else
/* # line 936 "tblsumm.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 937 "tblsumm.sc" */	/* host code */
					}
					else
					{
/* # line 629 "tblsumm.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(fsupport,(short *)0,1,32,0,err);
                              IItcoputio(ferror_no,(short *)0,1,30,4,&err_no);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
/* # line 632 "tblsumm.qsc" */	/* host code */
					}
				}
                        }
                      } /* IItunload */
IItbE3:
                      IItunend();
                    }
/* # line 635 "tblsumm.qsc" */	/* scroll */
                    {
                      if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                        IItbsmode((char *)"to");
                        if (IItscroll(0,1) != 0) {
                        } /* IItscroll */
                      } /* IItbsetio */
                    }
/* # line 636 "tblsumm.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 637 "tblsumm.qsc" */	/* redisplay */
                    {
                      IIredisp();
                    }
/* # line 638 "tblsumm.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE4;
                    }
                  }
                } else if (IIretval() == 3) {
                  {
/* # line 644 "tblsumm.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE4;
                    }
                  }
                } else {
                  if (1) goto IIfdE4;
                } /* IIretval */
              } /* IIrunnest */
IIfdF4:
              if (IIchkfrm() == 0) goto IIfdB4;
              goto IIfdE4;
IImuI4:
              if (IIinitmu() == 0) goto IIfdE4;
              if (IInmuact(ERget(F_RM003C_ThisTable),
              ERget(F_RM0049_ThisTable_supp_expl),2,2,1) == 0) goto IIfdE4;
              IIFRInternal(0);
              if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE4;
              if (IInmuact(ERget(F_RM003F_AllTables),
              ERget(F_RM004A_AllTables_supp_expl),2,2,2) == 0) goto IIfdE4;
              if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),2,2,3) 
              == 0) goto IIfdE4;
              IIFRInternal(0);
              if (IInfrskact(9,(char *)0,2,2,3) == 0) goto IIfdE4;
              if (IIendmu() == 0) goto IIfdE4;
              goto IIfdI4;
IIfdE4:;
              IIendnest();
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 651 "tblsumm.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&tbl_no,ftbl_no);
                IItcogetio((short *)0,1,32,3,columns,fcolumns);
                IItcogetio((short *)0,1,32,3,support,fsupport);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 653 "tblsumm.qsc" */	/* host code */
		if (!(STequal(columns, yes) && STequal(support, yes)))
		{
			IIUGerr(E_RM0035_Reg_supp_before_activ, UG_ERR_ERROR,
				0);
/* # line 658 "tblsumm.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 659 "tblsumm.qsc" */	/* host code */
		}
/* # line 661 "tblsumm.qsc" */	/* display */
            {
              if (IInestmu() == 0) goto IIfdE5;
              goto IImuI5;
IIfdI5:;
              if (IIfsetio((char *)0) == 0) goto IIfdE5;
              {
/* # line 664 "tblsumm.qsc" */	/* resume */
                {
                  IIresmu();
                  if (1) goto IIfdB5;
                }
              }
IIfdB5:
              while (IIrunnest() != 0) {
                if (IIretval() == 1) {
                  {
/* # line 669 "tblsumm.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 670 "tblsumm.qsc" */	/* host code */
			err_no = tbl_rules(tbl_no);
			if (err_no == OK)
			{
				TMnow(&stime);
				TMbreak(&stime, &now);
				STprintf(timestamp, ERx("%s-%s-%s %s:%s:%s"),
					now.day, now.month, now.year, now.hour,
					now.mins, now.sec);
/* # line 678 "tblsumm.qsc" */	/* putrow */
                    {
                      if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                        IItcoputio(frules,(short *)0,1,32,0,yes);
                        IItcoputio(frules_created,(short *)0,1,32,0,timestamp);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 680 "tblsumm.qsc" */	/* host code */
			}
			else if (err_no != -1)
			{
/* # line 683 "tblsumm.qsc" */	/* putrow */
                    {
                      if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                        IItcoputio(frules,(short *)0,1,32,0,err);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 684 "tblsumm.qsc" */	/* host code */
			}
/* # line 1082 "tblsumm.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 1083 "tblsumm.sc" */	/* host code */
/* # line 686 "tblsumm.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 687 "tblsumm.qsc" */	/* redisplay */
                    {
                      IIredisp();
                    }
/* # line 688 "tblsumm.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE5;
                    }
                  }
                } else if (IIretval() == 2) {
                  {
/* # line 693 "tblsumm.qsc" */	/* host code */
			err_no = tbl_deactivate(tbl_no);
			if (err_no == 0)
			{
/* # line 696 "tblsumm.qsc" */	/* putrow */
                    {
                      if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                        IItcoputio(frules,(short *)0,1,32,0,blank);
                        IItcoputio(frules_created,(short *)0,1,32,0,blank);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 698 "tblsumm.qsc" */	/* host code */
/* # line 1111 "tblsumm.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 1112 "tblsumm.sc" */	/* host code */
			}
			else
			{
/* # line 702 "tblsumm.qsc" */	/* putrow */
                    {
                      if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                        IItcoputio(frules,(short *)0,1,32,0,err);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 703 "tblsumm.qsc" */	/* host code */
/* # line 1123 "tblsumm.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 1124 "tblsumm.sc" */	/* host code */
			}
/* # line 705 "tblsumm.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE5;
                    }
                  }
                } else if (IIretval() == 3) {
                  {
/* # line 711 "tblsumm.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE5;
                    }
                  }
                } else {
                  if (1) goto IIfdE5;
                } /* IIretval */
              } /* IIrunnest */
IIfdF5:
              if (IIchkfrm() == 0) goto IIfdB5;
              goto IIfdE5;
IImuI5:
              if (IIinitmu() == 0) goto IIfdE5;
              if (IInmuact(ERget(F_RM0017_Activate),(char *)0,2,2,1) == 0) 
              goto IIfdE5;
              if (IInmuact(ERget(F_RM004D_DeActivate),(char *)0,2,2,2) == 0) 
              goto IIfdE5;
              if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),2,2,3) 
              == 0) goto IIfdE5;
              IIFRInternal(0);
              if (IInfrskact(9,(char *)0,2,2,3) == 0) goto IIfdE5;
              if (IIendmu() == 0) goto IIfdE5;
              goto IIfdI5;
IIfdE5:;
              IIendnest();
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 718 "tblsumm.qsc" */	/* display */
            {
              if (IInestmu() == 0) goto IIfdE6;
              goto IImuI6;
IIfdI6:;
              if (IIfsetio((char *)0) == 0) goto IIfdE6;
              {
/* # line 721 "tblsumm.qsc" */	/* resume */
                {
                  IIresmu();
                  if (1) goto IIfdB6;
                }
              }
IIfdB6:
              while (IIrunnest() != 0) {
                if (IIretval() == 1) {
                  {
/* # line 726 "tblsumm.qsc" */	/* getrow */
                    {
                      if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                        IItcogetio((short *)0,1,30,4,&tbl_no,ftbl_no);
                        IItcogetio((short *)0,1,32,3,support,fsupport);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 728 "tblsumm.qsc" */	/* display */
                    {
                      if (IInestmu() == 0) goto IIfdE7;
                      goto IImuI7;
IIfdI7:;
                      if (IIfsetio((char *)0) == 0) goto IIfdE7;
                      {
/* # line 731 "tblsumm.qsc" */	/* resume */
                        {
                          IIresmu();
                          if (1) goto IIfdB7;
                        }
                      }
IIfdB7:
                      while (IIrunnest() != 0) {
                        if (IIretval() == 1) {
                          {
/* # line 736 "tblsumm.qsc" */	/* host code */
				if (STequal(support, yes))
					create_replication_keys(tbl_no,
						FALSE);
/* # line 739 "tblsumm.qsc" */	/* breakdisplay */
                            {
                              if (1) goto IIfdE7;
                            }
                          }
                        } else if (IIretval() == 2) {
                          {
/* # line 744 "tblsumm.qsc" */	/* host code */
				if (STequal(support, yes))
					create_replication_keys(tbl_no,
						TRUE);
/* # line 747 "tblsumm.qsc" */	/* breakdisplay */
                            {
                              if (1) goto IIfdE7;
                            }
                          }
                        } else if (IIretval() == 3) {
                          {
/* # line 753 "tblsumm.qsc" */	/* breakdisplay */
                            {
                              if (1) goto IIfdE7;
                            }
                          }
                        } else {
                          if (1) goto IIfdE7;
                        } /* IIretval */
                      } /* IIrunnest */
IIfdF7:
                      if (IIchkfrm() == 0) goto IIfdB7;
                      goto IIfdE7;
IImuI7:
                      if (IIinitmu() == 0) goto IIfdE7;
                      if (IInmuact(ERget(F_RM0050_ShadowTableOnly),(char *)0,2
                      ,2,1) == 0) goto IIfdE7;
                      if (IInmuact(ERget(F_RM0051_BothQueue_ShadowTable),
                      (char *)0,2,2,2) == 0) goto IIfdE7;
                      if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel)
                      ,2,2,3) == 0) goto IIfdE7;
                      IIFRInternal(0);
                      if (IInfrskact(9,(char *)0,2,2,3) == 0) goto IIfdE7;
                      if (IIendmu() == 0) goto IIfdE7;
                      goto IIfdI7;
IIfdE7:;
                      IIendnest();
                    }
/* # line 755 "tblsumm.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE6;
                    }
                  }
                } else if (IIretval() == 2) {
                  {
/* # line 760 "tblsumm.qsc" */	/* display */
                    {
                      if (IInestmu() == 0) goto IIfdE8;
                      goto IImuI8;
IIfdI8:;
IIfdB8:
                      while (IIrunnest() != 0) {
                        if (IIretval() == 1) {
                          {
/* # line 763 "tblsumm.qsc" */	/* unloadtable */
                            {
                              if (IItbact(form_name,tf_name,0) == 0) goto 
                              IItbE4;
                              while (IItunload() != 0) {
                                IItcogetio((short *)0,1,30,4,&tbl_no,ftbl_no);
                                IItcogetio((short *)0,1,32,3,support,fsupport);
                                IItcogetio((short *)0,1,30,4,&rownumber,
                                (char *)"_RECORD");
                                IITBceColEnd();
                                {
/* # line 767 "tblsumm.qsc" */	/* scroll */
                                  {
                                    if (IItbsetio(1,form_name,tf_name,-3) != 0
                                    ) {
                                      IItbsmode((char *)"to");
                                      if (IItscroll(0,rownumber) != 0) {
                                      } /* IItscroll */
                                    } /* IItbsetio */
                                  }
/* # line 768 "tblsumm.qsc" */	/* redisplay */
                                  {
                                    IIredisp();
                                  }
/* # line 769 "tblsumm.qsc" */	/* host code */
					if (STequal(support, yes))
						create_replication_keys(tbl_no,
							FALSE);
                                }
                              } /* IItunload */
IItbE4:
                              IItunend();
                            }
/* # line 773 "tblsumm.qsc" */	/* breakdisplay */
                            {
                              if (1) goto IIfdE8;
                            }
                          }
                        } else if (IIretval() == 2) {
                          {
/* # line 778 "tblsumm.qsc" */	/* unloadtable */
                            {
                              if (IItbact(form_name,tf_name,0) == 0) goto 
                              IItbE5;
                              while (IItunload() != 0) {
                                IItcogetio((short *)0,1,30,4,&tbl_no,ftbl_no);
                                IItcogetio((short *)0,1,32,3,support,fsupport);
                                IItcogetio((short *)0,1,30,4,&rownumber,
                                (char *)"_RECORD");
                                IITBceColEnd();
                                {
/* # line 782 "tblsumm.qsc" */	/* scroll */
                                  {
                                    if (IItbsetio(1,form_name,tf_name,-3) != 0
                                    ) {
                                      IItbsmode((char *)"to");
                                      if (IItscroll(0,rownumber) != 0) {
                                      } /* IItscroll */
                                    } /* IItbsetio */
                                  }
/* # line 783 "tblsumm.qsc" */	/* redisplay */
                                  {
                                    IIredisp();
                                  }
/* # line 784 "tblsumm.qsc" */	/* host code */
					if (STequal(support, yes))
						create_replication_keys(tbl_no,
							TRUE);
                                }
                              } /* IItunload */
IItbE5:
                              IItunend();
                            }
/* # line 788 "tblsumm.qsc" */	/* breakdisplay */
                            {
                              if (1) goto IIfdE8;
                            }
                          }
                        } else if (IIretval() == 3) {
                          {
/* # line 794 "tblsumm.qsc" */	/* breakdisplay */
                            {
                              if (1) goto IIfdE8;
                            }
                          }
                        } else {
                          if (1) goto IIfdE8;
                        } /* IIretval */
                      } /* IIrunnest */
IIfdF8:
                      if (IIchkfrm() == 0) goto IIfdB8;
                      goto IIfdE8;
IImuI8:
                      if (IIinitmu() == 0) goto IIfdE8;
                      if (IInmuact(ERget(F_RM0050_ShadowTableOnly),(char *)0,2
                      ,2,1) == 0) goto IIfdE8;
                      if (IInmuact(ERget(F_RM0051_BothQueue_ShadowTable),
                      (char *)0,2,2,2) == 0) goto IIfdE8;
                      if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel)
                      ,2,2,3) == 0) goto IIfdE8;
                      IIFRInternal(0);
                      if (IInfrskact(9,(char *)0,2,2,3) == 0) goto IIfdE8;
                      if (IIendmu() == 0) goto IIfdE8;
                      goto IIfdI8;
IIfdE8:;
                      IIendnest();
                    }
/* # line 796 "tblsumm.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE6;
                    }
                  }
                } else if (IIretval() == 3) {
                  {
/* # line 802 "tblsumm.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE6;
                    }
                  }
                } else {
                  if (1) goto IIfdE6;
                } /* IIretval */
              } /* IIrunnest */
IIfdF6:
              if (IIchkfrm() == 0) goto IIfdB6;
              goto IIfdE6;
IImuI6:
              if (IIinitmu() == 0) goto IIfdE6;
              if (IInmuact(ERget(F_RM003C_ThisTable),(char *)0,2,2,1) == 0) 
              goto IIfdE6;
              IIFRInternal(0);
              if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE6;
              if (IInmuact(ERget(F_RM003F_AllTables),(char *)0,2,2,2) == 0) 
              goto IIfdE6;
              if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),2,2,3) 
              == 0) goto IIfdE6;
              IIFRInternal(0);
              if (IInfrskact(9,(char *)0,2,2,3) == 0) goto IIfdE6;
              if (IIendmu() == 0) goto IIfdE6;
              goto IIfdI6;
IIfdE6:;
              IIendnest();
            }
          }
        } else if (IIretval() == 7) {
          {
/* # line 808 "tblsumm.qsc" */	/* host code */
		FEtabfnd(form_name, tf_name);
          }
        } else if (IIretval() == 8) {
          {
/* # line 813 "tblsumm.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 9) {
          {
/* # line 818 "tblsumm.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 10) {
          {
/* # line 824 "tblsumm.qsc" */	/* host code */
		FEhelp(ERx("rmtblsum.hlp"),
			ERget(F_RM0052_Table_Registration));
          }
        } else if (IIretval() == 11) {
          {
/* # line 831 "tblsumm.qsc" */	/* breakdisplay */
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
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM003A_Register),ERget(F_RM003B_Register_expl),2,2,
      1) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM0041_Deregister),ERget(F_RM0042_Deregister_expl),
      2,2,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM0023_Edit),ERget(F_RM0046_Edit_tbl_expl),2,2,3) 
      == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM0047_Support),ERget(F_RM0048_Support_expl),2,2,4)
       == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM004B_Activation),ERget(F_RM004C_Activation_expl),
      2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM004E_CreateKeys),ERget(F_RM004F_CreateKeys_expl),
      2,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,8) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,9) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),2,2,10) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,10) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,11) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,11) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
  }
/* # line 836 "tblsumm.qsc" */	/* host code */
static void
display_errors()
  {
    char tbl_name[DB_MAXNAME+1];
    char tbl_owner[DB_MAXNAME+1];
    i4 err_no;
	i4	lerr_no;
	i4	nerrs;
	nerrs = 0;
/* # line 846 "tblsumm.qsc" */	/* unloadtable */
    {
      if (IItbact(form_name,tf_name,0) == 0) goto IItbE6;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,30,4,&err_no,(char *)"error_no");
        IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,tbl_name,ftbl_name);
        IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,tbl_owner,ftbl_owner);
        IITBceColEnd();
        {
/* # line 849 "tblsumm.qsc" */	/* host code */
		if (err_no)
		{
			lerr_no = err_no;
			IIUGmsg(ERget(E_RM0036_Err_processing_tbl), FALSE, 3,
				&lerr_no, tbl_owner, tbl_name);
			PCsleep(2000);
			++nerrs;
		}
        }
      } /* IItunload */
IItbE6:
      IItunend();
    }
/* # line 859 "tblsumm.qsc" */	/* host code */
	if (nerrs > 0)
		IIUGerr(E_RM0037_Found_regist_errors, UG_ERR_ERROR, 1, &nerrs);
  }
