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
# include <adf.h>
# include <fmt.h>
# include <ft.h>
# include <frame.h>
# include <runtime.h>
# include <uf.h>
# include <ug.h>
# include <stdprmpt.h>
# include <rpu.h>
# include <targtype.h>
# include "errm.h"
/**
** Name:	cddsdb.qsc - CDDS databases
**
** Description:
**	Defines
**		cdds_databases	- CDDS databases
**
** History:
**	16-dec-96 (joea)
**		Created based on cddsdbty.osq in replicator library.
**	14-jan-97 (joea)
**		Use RMtarg_get_descr instead of dd_target_types table.
**	14-nov-97 (joea)
**		Correct call to IIUGerr.
**	30-nov-98 (abbjo03)
**		Allow server number zero, indicating no server.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
# define MAX_SERVER_NO		999
GLOBALREF
  char RMdb_display[];
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmcdds_dbs"), *tf_name = ERx("iitf"), *fcdds_no = ERx("cdds_no")
  , *fcdds_name = ERx("cdds_name"), *fdb_no = ERx("database_no"), *fold_db_no 
  = ERx("old_db_no"), *ffull_db_name = ERx("vnode_db_name"), *ftarget_type = ERx("target_type")
  , *ftarget_descr = ERx("short_description"), *fserver_no = ERx("server_no");
/*{
** Name:	cdds_databases	- CDDS databases
**
** Description:
**	Define the databases containing a CDDS, the target types and the
**	servers.
**
** Inputs:
**	cdds_no			- CDDS number
**	cdds_name		- database name
**
** Outputs:
**	none
**
** Returns:
**	none
*/
void
cdds_databases(
i2	cdds_no,
char	*cdds_name)
# if 0
  i2 cdds_no;
  char *cdds_name;
# endif
  {
  i2 db_no;
  i2 old_db_no;
  i2 target_type;
  char full_db_name[DB_MAXNAME*2+3];
  i2 server_no;
  char server_name[25];
  i4 server_exist;
    char *target_descr;
    i4 rowstate;
    i4 rownumber;
    i4 chng;
    char objname[FE_MAXNAME+1];
	DBEC_ERR_INFO	errinfo;
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
			return;
		}
		form_initd = TRUE;
	}
/* # line 129 "cddsdb.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"f") != 0) {
        IIthidecol(fold_db_no,(char *)"i2 not null");
        IItfill();
      } /* IItbinit */
    }
/* # line 130 "cddsdb.qsc" */	/* host code */
/* # line 122 "cddsdb.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select d.database_no, TRIM(db.vnode_name) +'::' +TRIM(db.database_nam\
e), d.target_type, d.server_no from dd_db_cdds d, dd_databases db wher\
e d.cdds_no=");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIwritio(0,(short *)0,1,32,0,(char *)"and d.database_no=db.databas\
e_no");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&db_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME*2+3-1,full_db_name);
      IIgetdomio((short *)0,1,30,2,&target_type);
      IIgetdomio((short *)0,1,30,2,&server_no);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 132 "cddsdb.sc" */	/* host code */
		target_descr = RMtarg_get_descr(target_type);
/* # line 141 "cddsdb.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fdb_no,(short *)0,1,30,2,&db_no);
        IItcoputio(fold_db_no,(short *)0,1,30,2,&db_no);
        IItcoputio(ffull_db_name,(short *)0,1,32,0,full_db_name);
        IItcoputio(ftarget_type,(short *)0,1,30,2,&target_type);
        IItcoputio(ftarget_descr,(short *)0,1,32,0,target_descr);
        IItcoputio(fserver_no,(short *)0,1,30,2,&server_no);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 145 "cddsdb.qsc" */	/* host code */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
	{
/* # line 149 "cddsdb.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 150 "cddsdb.sc" */	/* host code */
		FTbell();
		IIUGerr(E_RM0012_Error_selecting, UG_ERR_ERROR, 0);
		return;
	}
/* # line 154 "cddsdb.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 155 "cddsdb.sc" */	/* host code */
/* # line 155 "cddsdb.qsc" */	/* display */
    {
      if (IIdispfrm(form_name,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio((char *)"db",(short *)0,1,32,0,RMdb_display);
      IIputfldio(fcdds_no,(short *)0,1,30,2,&cdds_no);
      IIputfldio(fcdds_name,(short *)0,1,32,0,cdds_name);
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 160 "cddsdb.qsc" */	/* inquire_frs */
            {
              if (IIiqset(5,0,form_name,tf_name,(char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 161 "cddsdb.qsc" */	/* host code */
		if (chng)
		{
/* # line 163 "cddsdb.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,2,&db_no,fdb_no);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 164 "cddsdb.qsc" */	/* host code */
/* # line 185 "cddsdb.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(vnode_name) +'::' +TRIM(database_name)from dd_databases w\
here database_no=");
    IIputdomio((short *)0,1,30,2,&db_no);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME*2+3-1,full_db_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 190 "cddsdb.sc" */	/* host code */
			RPdb_error_check(0, &errinfo);
/* # line 191 "cddsdb.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 192 "cddsdb.sc" */	/* host code */
			if (errinfo.errorno)
			{
				IIUGerr(E_RM0025_Error_check_db_no,
					UG_ERR_ERROR, 0);
/* # line 175 "cddsdb.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 176 "cddsdb.qsc" */	/* host code */
			}
			if (errinfo.rowcount == 0)
			{
				IIUGerr(E_RM0026_Invalid_db_no, UG_ERR_ERROR,
					0);
/* # line 181 "cddsdb.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 182 "cddsdb.qsc" */	/* host code */
			}
/* # line 183 "cddsdb.qsc" */	/* putrow */
            {
              if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                IItcoputio(ffull_db_name,(short *)0,1,32,0,full_db_name);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 184 "cddsdb.qsc" */	/* host code */
		}
/* # line 185 "cddsdb.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 190 "cddsdb.qsc" */	/* inquire_frs */
            {
              if (IIiqset(5,0,form_name,tf_name,(char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 191 "cddsdb.qsc" */	/* host code */
		if (chng)
		{
/* # line 193 "cddsdb.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,2,&target_type,ftarget_type);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 194 "cddsdb.qsc" */	/* host code */
			if (!VALID_TARGET_TYPE(target_type))
			{
				IIUGerr(E_RM0027_Invalid_target_type,
					UG_ERR_ERROR, 0);
/* # line 198 "cddsdb.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 199 "cddsdb.qsc" */	/* host code */
			}
			target_descr = RMtarg_get_descr(target_type);
/* # line 201 "cddsdb.qsc" */	/* putrow */
            {
              if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                IItcoputio(ftarget_descr,(short *)0,1,32,0,target_descr);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 202 "cddsdb.qsc" */	/* host code */
		}
/* # line 203 "cddsdb.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 208 "cddsdb.qsc" */	/* inquire_frs */
            {
              if (IIiqset(5,0,form_name,tf_name,(char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 209 "cddsdb.qsc" */	/* host code */
		if (chng)
		{
/* # line 211 "cddsdb.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,2,&server_no,fserver_no);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 212 "cddsdb.qsc" */	/* host code */
			if (server_no < 0 || server_no > MAX_SERVER_NO)
			{
				IIUGerr(E_RM0028_Invalid_server_no,
					UG_ERR_ERROR, 0);
/* # line 216 "cddsdb.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 217 "cddsdb.qsc" */	/* host code */
			}
		}
/* # line 219 "cddsdb.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 225 "cddsdb.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 226 "cddsdb.qsc" */	/* host code */
		if (!chng)
		{
			IIUGerr(E_RM0029_No_changes_to_save, 0, 0);
/* # line 229 "cddsdb.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 230 "cddsdb.qsc" */	/* host code */
		}
/* # line 232 "cddsdb.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 233 "cddsdb.qsc" */	/* host code */
		IIUGmsg(ERget(F_RM0009_Saving), FALSE, 0);
/* # line 234 "cddsdb.qsc" */	/* unloadtable */
            {
              if (IItbact(form_name,tf_name,0) == 0) goto IItbE1;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,30,4,&rowstate,(char *)"_STATE");
                IItcogetio((short *)0,1,30,4,&rownumber,(char *)"_RECO\
RD");
                IItcogetio((short *)0,1,30,2,&old_db_no,fold_db_no);
                IITBceColEnd();
                {
/* # line 237 "cddsdb.qsc" */	/* host code */
			if (rowstate == stDELETE)
			{
				/*
				** delete the path where the database exists,
				** if any
				*/
/* # line 352 "cddsdb.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cddsdb1",7224,7356);
      IIputdomio((short *)0,1,30,2,&cdds_no);
      IIputdomio((short *)0,1,30,2,&old_db_no);
      IIputdomio((short *)0,1,30,2,&old_db_no);
      IIputdomio((short *)0,1,30,2,&old_db_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cddsdb1",7224,7356);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from dd_paths where cdds_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&cdds_no);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(localdb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,2,&old_db_no);
        IIwritio(0,(short *)0,1,32,0,(char *)"or sourcedb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&old_db_no);
        IIwritio(0,(short *)0,1,32,0,(char *)"or targetdb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,2,&old_db_no);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"cddsdb1",7224,7356);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 356 "cddsdb.sc" */	/* host code */
				if (RPdb_error_check(DBEC_ZERO_ROWS_OK,
					NULL) != OK)
				{
/* # line 359 "cddsdb.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 360 "cddsdb.sc" */	/* host code */
					IIUGerr(E_RM002A_Error_saving_tf,
						UG_ERR_ERROR, 0);
/* # line 254 "cddsdb.qsc" */	/* scroll */
                  {
                    if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                      IItbsmode((char *)"to");
                      if (IItscroll(0,rownumber) != 0) {
                      } /* IItscroll */
                    } /* IItbsetio */
                  }
/* # line 255 "cddsdb.qsc" */	/* resume */
                  {
                    IIresfld(tf_name);
                    IItunend();
                    if (1) goto IIfdB1;
                  }
/* # line 256 "cddsdb.qsc" */	/* host code */
				}
/* # line 379 "cddsdb.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cddsdb2",7225,5466);
      IIputdomio((short *)0,1,30,2,&cdds_no);
      IIputdomio((short *)0,1,30,2,&old_db_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cddsdb2",7225,5466);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from dd_db_cdds where cdds_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&cdds_no);
        IIwritio(0,(short *)0,1,32,0,(char *)"and database_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,2,&old_db_no);
        IIexDefine(0,(char *)"cddsdb2",7225,5466);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 381 "cddsdb.sc" */	/* host code */
				if (RPdb_error_check(DBEC_SINGLE_ROW, NULL)
					!= OK)
				{
/* # line 384 "cddsdb.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 385 "cddsdb.sc" */	/* host code */
					IIUGerr(E_RM002A_Error_saving_tf,
						UG_ERR_ERROR, 0);
/* # line 267 "cddsdb.qsc" */	/* scroll */
                  {
                    if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                      IItbsmode((char *)"to");
                      if (IItscroll(0,rownumber) != 0) {
                      } /* IItscroll */
                    } /* IItbsetio */
                  }
/* # line 268 "cddsdb.qsc" */	/* resume */
                  {
                    IIresfld(tf_name);
                    IItunend();
                    if (1) goto IIfdB1;
                  }
/* # line 269 "cddsdb.qsc" */	/* host code */
				}
			}
                }
              } /* IItunload */
IItbE1:
              IItunend();
            }
/* # line 273 "cddsdb.qsc" */	/* host code */
		/* process all but Deleted rows */
/* # line 274 "cddsdb.qsc" */	/* unloadtable */
            {
              if (IItbact(form_name,tf_name,0) == 0) goto IItbE2;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,30,4,&rowstate,(char *)"_STATE");
                IItcogetio((short *)0,1,30,4,&rownumber,(char *)"_RECO\
RD");
                IItcogetio((short *)0,1,30,2,&db_no,fdb_no);
                IItcogetio((short *)0,1,30,2,&old_db_no,fold_db_no);
                IItcogetio((short *)0,1,32,DB_MAXNAME*2+3-1,full_db_name,
                ffull_db_name);
                IItcogetio((short *)0,1,30,2,&target_type,ftarget_type);
                IItcogetio((short *)0,1,30,2,&server_no,fserver_no);
                IITBceColEnd();
                {
/* # line 279 "cddsdb.qsc" */	/* host code */
			if (rowstate == stNEW)
/* # line 428 "cddsdb.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cddsdb3",7226,2204);
      IIputdomio((short *)0,1,30,2,&db_no);
      IIputdomio((short *)0,1,30,2,&cdds_no);
      IIputdomio((short *)0,1,30,2,&target_type);
      IIputdomio((short *)0,1,30,2,&server_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cddsdb3",7226,2204);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_db_cdds(database_no, cdds_no, target_type, server_no)v\
alues(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&db_no);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,2,&cdds_no);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&target_type);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,2,&server_no);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"cddsdb3",7226,2204);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 433 "cddsdb.sc" */	/* host code */
			else if (rowstate == stCHANGE)
				/*
				** Update row using hidden version of key fields
				** in where clause.
				*/
/* # line 438 "cddsdb.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cddsdb4",7227,13945);
      IIputdomio((short *)0,1,30,2,&db_no);
      IIputdomio((short *)0,1,30,2,&target_type);
      IIputdomio((short *)0,1,30,2,&server_no);
      IIputdomio((short *)0,1,30,2,&cdds_no);
      IIputdomio((short *)0,1,30,2,&old_db_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cddsdb4",7227,13945);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_db_cdds set database_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&db_no);
        IIwritio(0,(short *)0,1,32,0,(char *)", target_type=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,2,&target_type);
        IIwritio(0,(short *)0,1,32,0,(char *)", server_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&server_no);
        IIwritio(0,(short *)0,1,32,0,(char *)"where cdds_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,2,&cdds_no);
        IIwritio(0,(short *)0,1,32,0,(char *)"and database_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,2,&old_db_no);
        IIexDefine(0,(char *)"cddsdb4",7227,13945);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 444 "cddsdb.sc" */	/* host code */
			if (rowstate == stNEW || rowstate == stCHANGE)
			{
				if (RPdb_error_check(DBEC_SINGLE_ROW, NULL)
					!= OK)
				{
/* # line 449 "cddsdb.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 450 "cddsdb.sc" */	/* host code */
					IIUGerr(E_RM002A_Error_saving_tf,
						UG_ERR_ERROR, 0);
/* # line 304 "cddsdb.qsc" */	/* scroll */
                  {
                    if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                      IItbsmode((char *)"to");
                      if (IItscroll(0,rownumber) != 0) {
                      } /* IItscroll */
                    } /* IItbsetio */
                  }
/* # line 305 "cddsdb.qsc" */	/* resume */
                  {
                    IIresfld(tf_name);
                    IItunend();
                    if (1) goto IIfdB1;
                  }
/* # line 306 "cddsdb.qsc" */	/* host code */
				}
				if (server_no)
				{
/* # line 470 "cddsdb.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_servers where server_no=");
    IIputdomio((short *)0,1,30,2,&server_no);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&server_exist);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 474 "cddsdb.sc" */	/* host code */
					if (RPdb_error_check(DBEC_SINGLE_ROW,
						NULL) != OK)
					{
/* # line 477 "cddsdb.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 478 "cddsdb.sc" */	/* host code */
						IIUGerr(E_RM002A_Error_saving_tf,
							UG_ERR_ERROR, 0);
/* # line 320 "cddsdb.qsc" */	/* scroll */
                  {
                    if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                      IItbsmode((char *)"to");
                      if (IItscroll(0,rownumber) != 0) {
                      } /* IItscroll */
                    } /* IItbsetio */
                  }
/* # line 322 "cddsdb.qsc" */	/* resume */
                  {
                    IIresfld(tf_name);
                    IItunend();
                    if (1) goto IIfdB1;
                  }
/* # line 323 "cddsdb.qsc" */	/* host code */
					}
					if (!server_exist)
					{
						STprintf(server_name,
							ERx("dd_server%d"),
							server_no);
/* # line 501 "cddsdb.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_servers(server_no, server_name)values(");
    IIputdomio((short *)0,1,30,2,&server_no);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,server_name);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 507 "cddsdb.sc" */	/* host code */
						if (RPdb_error_check(
							DBEC_SINGLE_ROW,
							NULL) != OK)
						{
/* # line 511 "cddsdb.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 512 "cddsdb.sc" */	/* host code */
							IIUGerr(E_RM002A_Error_saving_tf,
								UG_ERR_ERROR,
								0);
/* # line 343 "cddsdb.qsc" */	/* scroll */
                  {
                    if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                      IItbsmode((char *)"to");
                      if (IItscroll(0,rownumber) != 0) {
                      } /* IItscroll */
                    } /* IItbsetio */
                  }
/* # line 345 "cddsdb.qsc" */	/* resume */
                  {
                    IIresfld(tf_name);
                    IItunend();
                    if (1) goto IIfdB1;
                  }
/* # line 346 "cddsdb.qsc" */	/* host code */
						}
					}
				}
			}
                }
              } /* IItunload */
IItbE2:
              IItunend();
            }
/* # line 351 "cddsdb.qsc" */	/* host code */
		if (db_config_changed(0) != OK)
		{
/* # line 542 "cddsdb.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 543 "cddsdb.sc" */	/* host code */
/* # line 354 "cddsdb.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 355 "cddsdb.qsc" */	/* host code */
		}
/* # line 549 "cddsdb.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 550 "cddsdb.sc" */	/* host code */
		if (RPdb_error_check(0, NULL) != OK)
		{
/* # line 552 "cddsdb.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 553 "cddsdb.sc" */	/* host code */
			IIUGerr(E_RM000A_Error_committing, UG_ERR_ERROR, 0);
/* # line 362 "cddsdb.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 363 "cddsdb.qsc" */	/* host code */
		}
/* # line 365 "cddsdb.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 371 "cddsdb.qsc" */	/* insertrow */
            {
              if (IItbsetio(5,form_name,tf_name,-2) != 0) {
                if (IItinsert() != 0) {
                  IItcoputio((char *)"_STATE",(short *)0,1,30,sizeof(0),
                  (void *)IILQint(0));
                  IITBceColEnd();
                } /* IItinsert */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 378 "cddsdb.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,32,DB_MAXNAME*2+3-1,full_db_name,
                ffull_db_name);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 379 "cddsdb.qsc" */	/* host code */
		if (IIUFccConfirmChoice(CONF_DELETE, full_db_name,
			ERget(F_RM0021_database), NULL, NULL) != CONFCH_YES)
/* # line 381 "cddsdb.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 383 "cddsdb.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,form_name,tf_name,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
/* # line 384 "cddsdb.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
          }
        } else if (IIretval() == 7) {
          {
/* # line 391 "cddsdb.qsc" */	/* inquire_frs */
            {
              if (IIiqset(4,0,form_name,tf_name,(char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,objname,19,(char *)0,0);
              } /* IIiqset */
            }
/* # line 392 "cddsdb.qsc" */	/* host code */
		if (STequal(objname, fdb_no))
		{
			*list_rows = EOS;
/* # line 622 "cddsdb.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select database_no, TRIM(vnode_name) +'::' +database_name from dd_dat\
abases where database_no not in(select database_no from dd_db_cdds whe\
re cdds_no=");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIwritio(0,(short *)0,1,32,0,(char *)")order by database_no");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&db_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME*2+3-1,full_db_name);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 632 "cddsdb.sc" */	/* host code */
				STtrmwhite(full_db_name);
				STprintf(list_row, ERx("%d;%s\n"), (i4)db_no,
					full_db_name);
				STcat(list_rows, list_row);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
/* # line 637 "cddsdb.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 638 "cddsdb.sc" */	/* host code */
			row = IIFDlpListPick(NULL, list_rows, 4, -1, -1, NULL,
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
				STcopy(pend, full_db_name);
				*p2 = EOS;
				CVan(p1, &i);
				db_no = (i2)i;
/* # line 429 "cddsdb.qsc" */	/* putrow */
            {
              if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                IItcoputio(fdb_no,(short *)0,1,30,2,&db_no);
                IItcoputio(ffull_db_name,(short *)0,1,32,0,full_db_name);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 431 "cddsdb.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 432 "cddsdb.qsc" */	/* host code */
			}
		}
		else if (STequal(objname, ftarget_type))
		{
			*list_rows = EOS;
			for (target_type = TARG_FULL_PEER;
				target_type <= TARG_UNPROT_READ; ++target_type)
			{
				STprintf(list_row, ERx("%d;%s\n"),
					(i4)target_type,
					RMtarg_get_descr(target_type));
				STcat(list_rows, list_row);
			}
			row = IIFDlpListPick(NULL, list_rows, 3, -1, -1, NULL,
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
				target_descr = pend;
				*p2 = EOS;
				CVan(p1, &i);
				target_type = (i2)i;
/* # line 463 "cddsdb.qsc" */	/* putrow */
            {
              if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                IItcoputio(ftarget_type,(short *)0,1,30,2,&target_type);
                IItcoputio(ftarget_descr,(short *)0,1,32,0,target_descr);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 466 "cddsdb.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 467 "cddsdb.qsc" */	/* host code */
			}
		}
		else
		{
			IIUFfieldHelp();
		}
          }
        } else if (IIretval() == 8) {
          {
/* # line 477 "cddsdb.qsc" */	/* host code */
		FEtabfnd(form_name, tf_name);
          }
        } else if (IIretval() == 9) {
          {
/* # line 482 "cddsdb.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 10) {
          {
/* # line 487 "cddsdb.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 11) {
          {
/* # line 494 "cddsdb.qsc" */	/* host code */
		FEhelp(ERx("rmcddbty.hlp"), ERget(F_RM0034_Define_cdds_dbs));
          }
        } else if (IIretval() == 12) {
          {
/* # line 501 "cddsdb.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 502 "cddsdb.qsc" */	/* host code */
		if (chng && IIUFccConfirmChoice(CONF_GENERIC, NULL, NULL,
			NULL, NULL, S_RM000E_End_no_save, S_RM000F_Yes_end,
			S_RM0010_No_end, ERx(""), TRUE) != CONFCH_YES)
/* # line 505 "cddsdb.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 507 "cddsdb.qsc" */	/* breakdisplay */
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
      if (IITBcaClmAct(tf_name,fdb_no,0,1) == 0) goto IIfdE1;
      if (IITBcaClmAct(tf_name,ftarget_type,0,2) == 0) goto IIfdE1;
      if (IITBcaClmAct(tf_name,fserver_no,0,3) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Save),ERget(F_FE0117_SaveExpl),2,2,4) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Insert),ERget(F_RM0036_Insert_expl),2,2,5) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(12,(char *)0,2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM0032_RowDelete),ERget(F_RM0033_RowDelete_expl),0,
      0,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(14,(char *)0,0,0,6) == 0) goto IIfdE1;
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
