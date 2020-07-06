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
# include <erfe.h>
# include <uf.h>
# include <ug.h>
# include <stdprmpt.h>
# include <rpu.h>
# include "errm.h"
/**
** Name:	dbsumm.qsc - database summary
**
** Description:
**	Defines
**		db_summ	- database summary
**
** History:
**	16-dec-96 (joea)
**		Created based on dbcatlg.osq in replicator library.
**	12-may-98 (padni01/joea) bug 88842
**		When inserting a new row, scroll display window to the position
**		where the new row will be added. Correct the code for finding
**		the position where the new row will be added.
**/
GLOBALREF
  char RMdb_display[];
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmdb_summary"), *tf_name = ERx("iitf"), *fdb_no = ERx("database_no")
  , *fvnode_name = ERx("vnode_name"), *fdb_name = ERx("database_name"), *
  ffull_db_name = ERx("vnode_dbname"), *fdb_owner = ERx("database_owner"), *
  fdbms_type = ERx("dbms_type"), *flocal_db = ERx("local_db"), *fremark = ERx("remark")
  ;
FUNC_EXTERN STATUS db_add(i2 *db_no, char *vnode_name, char *db_name,
	char *db_owner, char *dbms_type, char *remark);
FUNC_EXTERN STATUS database_delete(i2 db_no);
FUNC_EXTERN STATUS db_edit(i2 db_no, i2 local_db, char *vnode_name,
	char *db_name, char *db_owner, char *dbms_type, char *remark);
/*{
** Name:	db_summ	- database summary
**
** Description:
**	Summary of Replicated Database Information.
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
db_summ()
  {
  i2 db_no;
  char vnode_name[DB_MAXNAME+1];
  char db_name[DB_MAXNAME+1];
  char db_owner[DB_MAXNAME+1];
  char dbms_type[9];
  char remark[81];
  i2 local_db;
    i2 curr_db_no;
    i4 rownumber;
    char full_db_name[DB_MAXNAME*2+3];
	DBEC_ERR_INFO	errinfo;
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
/* # line 101 "dbsumm.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"r") != 0) {
        IIthidecol(fvnode_name,(char *)"char(32) not null");
        IIthidecol(fdb_name,(char *)"char(32) not null");
        IIthidecol(flocal_db,(char *)"i2 not null");
        IItfill();
      } /* IItbinit */
    }
/* # line 103 "dbsumm.qsc" */	/* host code */
	IIUGmsg(ERget(F_RM001C_Selecting_data), FALSE, 0);
/* # line 98 "dbsumm.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select database_no, vnode_name, database_name, database_owner, local_\
db, dbms_type, remark from dd_databases order by database_no");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&db_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,vnode_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,db_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,db_owner);
      IIgetdomio((short *)0,1,30,2,&local_db);
      IIgetdomio((short *)0,1,32,8,dbms_type);
      IIgetdomio((short *)0,1,32,80,remark);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 105 "dbsumm.sc" */	/* host code */
		STtrmwhite(vnode_name);
		STtrmwhite(db_name);
		STprintf(full_db_name, ERx("%s::%s"), vnode_name, db_name);
/* # line 114 "dbsumm.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fdb_no,(short *)0,1,30,2,&db_no);
        IItcoputio(fvnode_name,(short *)0,1,32,0,vnode_name);
        IItcoputio(fdb_name,(short *)0,1,32,0,db_name);
        IItcoputio(ffull_db_name,(short *)0,1,32,0,full_db_name);
        IItcoputio(fdb_owner,(short *)0,1,32,0,db_owner);
        IItcoputio(flocal_db,(short *)0,1,30,2,&local_db);
        IItcoputio(fdbms_type,(short *)0,1,32,0,dbms_type);
        IItcoputio(fremark,(short *)0,1,32,0,remark);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 119 "dbsumm.qsc" */	/* host code */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	RPdb_error_check(0, &errinfo);
	if (errinfo.errorno)
	{
/* # line 127 "dbsumm.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 128 "dbsumm.sc" */	/* host code */
		FTbell();
		IIUGerr(E_RM0012_Error_selecting, UG_ERR_ERROR, 0);
		return;
	}
	else if (errinfo.rowcount == 0)
	{
/* # line 134 "dbsumm.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 135 "dbsumm.sc" */	/* host code */
		IIUGerr(E_RM0013_No_data_found, UG_ERR_ERROR, 0);
		return;
	}
/* # line 138 "dbsumm.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 139 "dbsumm.sc" */	/* host code */
/* # line 136 "dbsumm.qsc" */	/* display */
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
/* # line 141 "dbsumm.qsc" */	/* host code */
		if (db_add(&db_no, vnode_name, db_name, db_owner, dbms_type,
			remark) == 1)
		{
/* # line 144 "dbsumm.qsc" */	/* unloadtable */
            {
              if (IItbact(form_name,tf_name,0) == 0) goto IItbE1;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,30,2,&curr_db_no,fdb_no);
                IItcogetio((short *)0,1,30,4,&rownumber,(char *)"_RECO\
RD");
                IITBceColEnd();
                {
/* # line 147 "dbsumm.qsc" */	/* host code */
				if (db_no < curr_db_no)
				{
					--rownumber;
/* # line 150 "dbsumm.qsc" */	/* endloop */
                  {
                    if (1) goto IItbE1;
                  }
/* # line 151 "dbsumm.qsc" */	/* host code */
				}
				else if (rownumber < 0)
				{
/* # line 154 "dbsumm.qsc" */	/* endloop */
                  {
                    if (1) goto IItbE1;
                  }
/* # line 155 "dbsumm.qsc" */	/* host code */
				}
                }
              } /* IItunload */
IItbE1:
              IItunend();
            }
/* # line 158 "dbsumm.qsc" */	/* host code */
			if (rownumber < 0)
/* # line 159 "dbsumm.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&rownumber,32,(char *)0,0);
              } /* IIiqset */
            }
/* # line 162 "dbsumm.qsc" */	/* host code */
			STprintf(full_db_name, ERx("%s::%s"), vnode_name,
				db_name);
			if (rownumber < 1)
			{
/* # line 166 "dbsumm.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 167 "dbsumm.qsc" */	/* insertrow */
            {
              if (IItbsetio(5,form_name,tf_name,0) != 0) {
                if (IItinsert() != 0) {
                  IItcoputio(fdb_no,(short *)0,1,30,2,&db_no);
                  IItcoputio(fvnode_name,(short *)0,1,32,0,vnode_name);
                  IItcoputio(fdb_name,(short *)0,1,32,0,db_name);
                  IItcoputio(ffull_db_name,(short *)0,1,32,0,full_db_name);
                  IItcoputio(fdb_owner,(short *)0,1,32,0,db_owner);
                  IItcoputio(fdbms_type,(short *)0,1,32,0,dbms_type);
                  IItcoputio(fremark,(short *)0,1,32,0,remark);
                  IITBceColEnd();
                } /* IItinsert */
              } /* IItbsetio */
            }
/* # line 174 "dbsumm.qsc" */	/* host code */
			}
			else
			{
/* # line 177 "dbsumm.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,rownumber) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 178 "dbsumm.qsc" */	/* insertrow */
            {
              if (IItbsetio(5,form_name,tf_name,-2) != 0) {
                if (IItinsert() != 0) {
                  IItcoputio(fdb_no,(short *)0,1,30,2,&db_no);
                  IItcoputio(fvnode_name,(short *)0,1,32,0,vnode_name);
                  IItcoputio(fdb_name,(short *)0,1,32,0,db_name);
                  IItcoputio(ffull_db_name,(short *)0,1,32,0,full_db_name);
                  IItcoputio(fdb_owner,(short *)0,1,32,0,db_owner);
                  IItcoputio(fdbms_type,(short *)0,1,32,0,dbms_type);
                  IItcoputio(fremark,(short *)0,1,32,0,remark);
                  IITBceColEnd();
                } /* IItinsert */
              } /* IItbsetio */
            }
/* # line 185 "dbsumm.qsc" */	/* host code */
			}
		}
          }
        } else if (IIretval() == 2) {
          {
/* # line 192 "dbsumm.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,2,&db_no,fdb_no);
                IItcogetio((short *)0,1,32,DB_MAXNAME*2+3-1,full_db_name,
                ffull_db_name);
                IItcogetio((short *)0,1,30,2,&local_db,flocal_db);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 194 "dbsumm.qsc" */	/* host code */
		if (local_db)
		{
			IIUGerr(E_RM0014_Cant_del_local_db, UG_ERR_ERROR, 0);
/* # line 197 "dbsumm.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 198 "dbsumm.qsc" */	/* host code */
		}
		if (IIUFccConfirmChoice(CONF_DELETE, full_db_name,
			ERget(F_RM0021_database), NULL, NULL) != CONFCH_YES)
/* # line 202 "dbsumm.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 204 "dbsumm.qsc" */	/* host code */
		IIUGmsg(ERget(F_RM0022_Deleting), FALSE, 0);
		if (database_delete(db_no) != OK)
		{
			IIUGerr(E_RM0015_Error_deleting, UG_ERR_ERROR, 0);
/* # line 208 "dbsumm.qsc" */	/* resume */
            {
              IIresfld(tf_name);
              if (1) goto IIfdB1;
            }
/* # line 209 "dbsumm.qsc" */	/* host code */
		}
/* # line 211 "dbsumm.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,form_name,tf_name,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 217 "dbsumm.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,2,&db_no,fdb_no);
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,vnode_name,
                fvnode_name);
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,db_name,fdb_name);
                IItcogetio((short *)0,1,32,DB_MAXNAME*2+3-1,full_db_name,
                ffull_db_name);
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,db_owner,fdb_owner);
                IItcogetio((short *)0,1,30,2,&local_db,flocal_db);
                IItcogetio((short *)0,1,32,8,dbms_type,fdbms_type);
                IItcogetio((short *)0,1,32,80,remark,fremark);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 223 "dbsumm.qsc" */	/* host code */
		if (db_edit(db_no, local_db, vnode_name, db_name, db_owner,
			dbms_type, remark) == 1)
		{
			STprintf(full_db_name, ERx("%s::%s"), vnode_name,
				db_name);
/* # line 228 "dbsumm.qsc" */	/* putrow */
            {
              if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                IItcoputio(fdb_no,(short *)0,1,30,2,&db_no);
                IItcoputio(fvnode_name,(short *)0,1,32,0,vnode_name);
                IItcoputio(fdb_name,(short *)0,1,32,0,db_name);
                IItcoputio(ffull_db_name,(short *)0,1,32,0,full_db_name);
                IItcoputio(fdb_owner,(short *)0,1,32,0,db_owner);
                IItcoputio(fdbms_type,(short *)0,1,32,0,dbms_type);
                IItcoputio(fremark,(short *)0,1,32,0,remark);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 233 "dbsumm.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 4) {
          {
/* # line 238 "dbsumm.qsc" */	/* host code */
		FEtabfnd(form_name, tf_name);
          }
        } else if (IIretval() == 5) {
          {
/* # line 243 "dbsumm.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 248 "dbsumm.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 7) {
          {
/* # line 254 "dbsumm.qsc" */	/* host code */
		FEhelp(ERx("rmdbsumm.hlp"), ERget(F_RM0025_Db_summary));
          }
        } else if (IIretval() == 8) {
          {
/* # line 260 "dbsumm.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(F_RM001D_Add),ERget(F_RM001E_Add_db_expl),2,2,1) == 0
      ) goto IIfdE1;
      if (IInmuact(ERget(F_RM001F_Delete),ERget(F_RM0020_Delete_db_expl),2,2,2)
       == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(13,(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM0023_Edit),ERget(F_RM0024_Edit_db_expl),2,2,3) ==
       0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,5) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),2,2,7) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,7) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,8) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,8) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
  }
