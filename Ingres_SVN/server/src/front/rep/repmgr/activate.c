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
# include <rpu.h>
# include "errm.h"
/**
** Name:	activate.qsc - activate Change Recorder
**
** Description:
**	Defines
**		chgrec_activate	- activate Change Recorder
**		CheckReturnCode	- check return code
**
** History:
**	16-dec-96 (joea)
**		Created based on activate.osq in replicator library.
**	01-oct-98 (abbjo03)
**		Add FRSkey4 (Go) to the Activate options.
**	10-dec-98 (abbjo03)
**		Change turn_recorder's 3rd parameter into a bool. Remove extra
**		FRSkey4 in Database/Deactivate.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
# define NOT_CALLED		(-2)
# define NO_ACTION_YET		(-1)
# define TBL_ACTIVATE		0
# define TBL_DEACT		1
GLOBALREF
  char RMdb_display[];
FUNC_EXTERN STATUS turn_recorder(char *dbname, i2 cdds_no, bool activate);
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmactivate"), *tf_name = ERx("iitf"), *fcdds_no = ERx("cdds_no")
  , *fcdds_name = ERx("cdds_name"), *fdb_no = ERx("db_no"), *ffull_db_name = ERx("db_name")
  , *fstatus = ERx("status"), *blank = ERx("");
static void CheckReturnCode(i4 what, i4  retval);
/*{
** Name:	chgrec_activate	- activate Change Recorder
**
** Description:
**	Activate the selected target databases or CDDSs.
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
chgrec_activate()
  {
  i2 cdds_no;
  char cdds_name[DB_MAXNAME+1];
  i2 db_no;
  char full_db_name[DB_MAXNAME*2+3];
  i2 curr_db;
  i2 curr_cdds;
  char status[5];
	i4	retval;
	i4	what;
	DBEC_ERR_INFO	errinfo;
    char *done = ERget(F_RM0062_done);
    char *err = ERget(F_RM003E_err);
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
/* # line 111 "activate.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 112 "activate.qsc" */	/* host code */
	IIUGmsg(ERget(F_RM001C_Selecting_data), FALSE, 0);
/* # line 101 "activate.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dc.cdds_no, cdds_name, dc.database_no, TRIM(vnode_name) +'::' \
+TRIM(database_name)from dd_db_cdds dc, dd_databases d, dd_cdds c wher\
e dc.database_no=d.database_no and d.dbms_type='ingres' and dc.cdds_no\
=c.cdds_no and dc.target_type=1 order by ");
    IIwritio(0,(short *)0,1,32,0,(char *)"cdds_no, database_no");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&cdds_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,cdds_name);
      IIgetdomio((short *)0,1,30,2,&db_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME*2+3-1,full_db_name);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 112 "activate.sc" */	/* host code */
		STtrmwhite(cdds_name);
/* # line 125 "activate.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fcdds_no,(short *)0,1,30,2,&cdds_no);
        IItcoputio(fcdds_name,(short *)0,1,32,0,cdds_name);
        IItcoputio(fdb_no,(short *)0,1,30,2,&db_no);
        IItcoputio(ffull_db_name,(short *)0,1,32,0,full_db_name);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 128 "activate.qsc" */	/* host code */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	RPdb_error_check(0, &errinfo);
	if (errinfo.errorno)
	{
/* # line 128 "activate.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 129 "activate.sc" */	/* host code */
		FTbell();
		IIUGerr(E_RM0012_Error_selecting, UG_ERR_ERROR, 0);
		return;
	}
	else if (errinfo.rowcount == 0)
	{
/* # line 135 "activate.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 136 "activate.sc" */	/* host code */
		IIUGerr(E_RM0013_No_data_found, UG_ERR_ERROR, 0);
		return;
	}
/* # line 139 "activate.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 140 "activate.sc" */	/* host code */
/* # line 145 "activate.qsc" */	/* display */
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
/* # line 150 "activate.qsc" */	/* host code */
		retval = NOT_CALLED;
		what = NO_ACTION_YET;
/* # line 152 "activate.qsc" */	/* set_frs */
            {
              if (IIiqset(4,0,form_name,tf_name,(char *)0) != 0) {
                IIstfsio(54,fstatus,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 154 "activate.qsc" */	/* display */
            {
              if (IInestmu() == 0) goto IIfdE2;
              goto IImuI2;
IIfdI2:;
              if (IIfsetio((char *)0) == 0) goto IIfdE2;
              {
/* # line 157 "activate.qsc" */	/* resume */
                {
                  IIresmu();
                  if (1) goto IIfdB2;
                }
              }
IIfdB2:
              while (IIrunnest() != 0) {
                if (IIretval() == 1) {
                  {
/* # line 162 "activate.qsc" */	/* host code */
			what = TBL_ACTIVATE;
/* # line 163 "activate.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 164 "activate.qsc" */	/* getrow */
                    {
                      if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                        IItcogetio((short *)0,1,30,2,&cdds_no,fcdds_no);
                        IItcogetio((short *)0,1,32,DB_MAXNAME*2+3-1,
                        full_db_name,ffull_db_name);
                        IItcogetio((short *)0,1,32,4,status,fstatus);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 166 "activate.qsc" */	/* host code */
			if (!STequal(status, done))
				retval = turn_recorder(full_db_name, cdds_no,
					TRUE);
			if (retval == 0 || retval == -1)
/* # line 170 "activate.qsc" */	/* putrow */
                    {
                      if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                        IItcoputio(fstatus,(short *)0,1,32,0,done);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 171 "activate.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 172 "activate.qsc" */	/* redisplay */
                    {
                      IIredisp();
                    }
/* # line 173 "activate.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE2;
                    }
                  }
                } else if (IIretval() == 2) {
                  {
/* # line 179 "activate.qsc" */	/* host code */
			what = TBL_DEACT;
/* # line 180 "activate.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 181 "activate.qsc" */	/* getrow */
                    {
                      if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                        IItcogetio((short *)0,1,30,2,&cdds_no,fcdds_no);
                        IItcogetio((short *)0,1,32,DB_MAXNAME*2+3-1,
                        full_db_name,ffull_db_name);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 183 "activate.qsc" */	/* host code */
			retval = turn_recorder(full_db_name, cdds_no, FALSE);
			if (retval == 0 || retval == -1)
/* # line 185 "activate.qsc" */	/* putrow */
                    {
                      if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                        IItcoputio(fstatus,(short *)0,1,32,0,blank);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 186 "activate.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 187 "activate.qsc" */	/* redisplay */
                    {
                      IIredisp();
                    }
/* # line 188 "activate.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE2;
                    }
                  }
                } else if (IIretval() == 3) {
                  {
/* # line 194 "activate.qsc" */	/* breakdisplay */
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
              if (IInmuact(ERget(F_RM0017_Activate),
              ERget(F_RM0061_Activ_cdds_db_expl),2,2,1) == 0) goto IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE2;
              if (IInmuact(ERget(F_RM004D_DeActivate),
              ERget(F_RM0063_Deactiv_cdds_db_expl),2,2,2) == 0) goto IIfdE2;
              if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),2,2,3) 
              == 0) goto IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(9,(char *)0,2,2,3) == 0) goto IIfdE2;
              if (IIendmu() == 0) goto IIfdE2;
              goto IIfdI2;
IIfdE2:;
              IIendnest();
            }
/* # line 197 "activate.qsc" */	/* host code */
		CheckReturnCode(what, retval);
          }
        } else if (IIretval() == 2) {
          {
/* # line 203 "activate.qsc" */	/* host code */
		retval = NOT_CALLED;
		what = NO_ACTION_YET;
/* # line 205 "activate.qsc" */	/* set_frs */
            {
              if (IIiqset(4,0,form_name,tf_name,(char *)0) != 0) {
                IIstfsio(54,fstatus,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 207 "activate.qsc" */	/* display */
            {
              if (IInestmu() == 0) goto IIfdE3;
              goto IImuI3;
IIfdI3:;
              if (IIfsetio((char *)0) == 0) goto IIfdE3;
              {
/* # line 210 "activate.qsc" */	/* resume */
                {
                  IIresmu();
                  if (1) goto IIfdB3;
                }
              }
IIfdB3:
              while (IIrunnest() != 0) {
                if (IIretval() == 1) {
                  {
/* # line 215 "activate.qsc" */	/* host code */
			what = TBL_ACTIVATE;
/* # line 216 "activate.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 217 "activate.qsc" */	/* getrow */
                    {
                      if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                        IItcogetio((short *)0,1,30,2,&curr_cdds,fcdds_no);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 218 "activate.qsc" */	/* unloadtable */
                    {
                      if (IItbact(form_name,tf_name,0) == 0) goto IItbE1;
                      while (IItunload() != 0) {
                        IItcogetio((short *)0,1,30,2,&cdds_no,fcdds_no);
                        IItcogetio((short *)0,1,32,DB_MAXNAME*2+3-1,
                        full_db_name,ffull_db_name);
                        IItcogetio((short *)0,1,32,4,status,fstatus);
                        IITBceColEnd();
                        {
/* # line 221 "activate.qsc" */	/* host code */
				if (curr_cdds == cdds_no &&
					!STequal(status, done))
				{
					retval = turn_recorder(full_db_name,
						cdds_no, TRUE);
					if (retval > 0)
					{
/* # line 228 "activate.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(fstatus,(short *)0,1,32,0,err);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
/* # line 230 "activate.qsc" */	/* endloop */
                          {
                            if (1) goto IItbE1;
                          }
/* # line 231 "activate.qsc" */	/* host code */
					}
/* # line 232 "activate.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(fstatus,(short *)0,1,32,0,done);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
/* # line 234 "activate.qsc" */	/* host code */
				}
                        }
                      } /* IItunload */
IItbE1:
                      IItunend();
                    }
/* # line 236 "activate.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 237 "activate.qsc" */	/* redisplay */
                    {
                      IIredisp();
                    }
/* # line 238 "activate.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE3;
                    }
                  }
                } else if (IIretval() == 2) {
                  {
/* # line 244 "activate.qsc" */	/* host code */
			what = TBL_DEACT;
/* # line 245 "activate.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 246 "activate.qsc" */	/* getrow */
                    {
                      if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                        IItcogetio((short *)0,1,30,2,&curr_cdds,fcdds_no);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 247 "activate.qsc" */	/* unloadtable */
                    {
                      if (IItbact(form_name,tf_name,0) == 0) goto IItbE2;
                      while (IItunload() != 0) {
                        IItcogetio((short *)0,1,30,2,&cdds_no,fcdds_no);
                        IItcogetio((short *)0,1,32,DB_MAXNAME*2+3-1,
                        full_db_name,ffull_db_name);
                        IITBceColEnd();
                        {
/* # line 250 "activate.qsc" */	/* host code */
				if (curr_cdds == cdds_no)
				{
					retval = turn_recorder(full_db_name,
						cdds_no, FALSE);
					if (retval > 0)
					{
/* # line 256 "activate.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(fstatus,(short *)0,1,32,0,err);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
/* # line 258 "activate.qsc" */	/* endloop */
                          {
                            if (1) goto IItbE2;
                          }
/* # line 259 "activate.qsc" */	/* host code */
					}
/* # line 260 "activate.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(fstatus,(short *)0,1,32,0,blank);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
/* # line 262 "activate.qsc" */	/* host code */
				}
                        }
                      } /* IItunload */
IItbE2:
                      IItunend();
                    }
/* # line 264 "activate.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 265 "activate.qsc" */	/* redisplay */
                    {
                      IIredisp();
                    }
/* # line 266 "activate.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE3;
                    }
                  }
                } else if (IIretval() == 3) {
                  {
/* # line 272 "activate.qsc" */	/* breakdisplay */
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
              if (IInmuact(ERget(F_RM0017_Activate),
              ERget(F_RM0065_Activ_cdds_expl),2,2,1) == 0) goto IIfdE3;
              IIFRInternal(0);
              if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE3;
              if (IInmuact(ERget(F_RM004D_DeActivate),
              ERget(F_RM0066_Deactiv_cdds_expl),2,2,2) == 0) goto IIfdE3;
              if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),2,2,3) 
              == 0) goto IIfdE3;
              IIFRInternal(0);
              if (IInfrskact(9,(char *)0,2,2,3) == 0) goto IIfdE3;
              if (IIendmu() == 0) goto IIfdE3;
              goto IIfdI3;
IIfdE3:;
              IIendnest();
            }
/* # line 275 "activate.qsc" */	/* host code */
		CheckReturnCode(what, retval);
          }
        } else if (IIretval() == 3) {
          {
/* # line 281 "activate.qsc" */	/* host code */
		retval = NOT_CALLED;
		what = NO_ACTION_YET;
/* # line 283 "activate.qsc" */	/* set_frs */
            {
              if (IIiqset(4,0,form_name,tf_name,(char *)0) != 0) {
                IIstfsio(54,fstatus,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 285 "activate.qsc" */	/* display */
            {
              if (IInestmu() == 0) goto IIfdE4;
              goto IImuI4;
IIfdI4:;
              if (IIfsetio((char *)0) == 0) goto IIfdE4;
              {
/* # line 288 "activate.qsc" */	/* resume */
                {
                  IIresmu();
                  if (1) goto IIfdB4;
                }
              }
IIfdB4:
              while (IIrunnest() != 0) {
                if (IIretval() == 1) {
                  {
/* # line 293 "activate.qsc" */	/* host code */
			what = TBL_ACTIVATE;
/* # line 294 "activate.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 295 "activate.qsc" */	/* host code */
			retval = OK;
/* # line 296 "activate.qsc" */	/* getrow */
                    {
                      if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                        IItcogetio((short *)0,1,30,2,&curr_db,fdb_no);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 297 "activate.qsc" */	/* unloadtable */
                    {
                      if (IItbact(form_name,tf_name,0) == 0) goto IItbE3;
                      while (IItunload() != 0) {
                        IItcogetio((short *)0,1,30,2,&cdds_no,fcdds_no);
                        IItcogetio((short *)0,1,32,DB_MAXNAME*2+3-1,
                        full_db_name,ffull_db_name);
                        IItcogetio((short *)0,1,30,2,&db_no,fdb_no);
                        IItcogetio((short *)0,1,32,4,status,fstatus);
                        IITBceColEnd();
                        {
/* # line 301 "activate.qsc" */	/* host code */
				if (curr_db == db_no &&
					!STequal(status, done))
				{
					retval = turn_recorder(full_db_name,
						cdds_no, TRUE);
					if (retval > 0)
					{
/* # line 308 "activate.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(fstatus,(short *)0,1,32,0,err);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
/* # line 310 "activate.qsc" */	/* endloop */
                          {
                            if (1) goto IItbE3;
                          }
/* # line 311 "activate.qsc" */	/* host code */
					}
/* # line 312 "activate.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(fstatus,(short *)0,1,32,0,done);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
/* # line 314 "activate.qsc" */	/* host code */
				}
                        }
                      } /* IItunload */
IItbE3:
                      IItunend();
                    }
/* # line 316 "activate.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 317 "activate.qsc" */	/* redisplay */
                    {
                      IIredisp();
                    }
/* # line 318 "activate.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE4;
                    }
                  }
                } else if (IIretval() == 2) {
                  {
/* # line 324 "activate.qsc" */	/* host code */
			what = TBL_DEACT;
/* # line 325 "activate.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 326 "activate.qsc" */	/* getrow */
                    {
                      if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                        IItcogetio((short *)0,1,30,2,&curr_db,fdb_no);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 327 "activate.qsc" */	/* unloadtable */
                    {
                      if (IItbact(form_name,tf_name,0) == 0) goto IItbE4;
                      while (IItunload() != 0) {
                        IItcogetio((short *)0,1,30,2,&cdds_no,fcdds_no);
                        IItcogetio((short *)0,1,32,DB_MAXNAME*2+3-1,
                        full_db_name,ffull_db_name);
                        IItcogetio((short *)0,1,30,2,&db_no,fdb_no);
                        IITBceColEnd();
                        {
/* # line 330 "activate.qsc" */	/* host code */
				if (curr_db == db_no)
				{
					retval = turn_recorder(full_db_name,
						cdds_no, FALSE);
					if (retval > 0)
					{
/* # line 336 "activate.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(fstatus,(short *)0,1,32,0,err);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
/* # line 338 "activate.qsc" */	/* endloop */
                          {
                            if (1) goto IItbE4;
                          }
/* # line 339 "activate.qsc" */	/* host code */
					}
/* # line 340 "activate.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(fstatus,(short *)0,1,32,0,blank);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
/* # line 342 "activate.qsc" */	/* host code */
				}
                        }
                      } /* IItunload */
IItbE4:
                      IItunend();
                    }
/* # line 344 "activate.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 345 "activate.qsc" */	/* redisplay */
                    {
                      IIredisp();
                    }
/* # line 346 "activate.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE4;
                    }
                  }
                } else if (IIretval() == 3) {
                  {
/* # line 352 "activate.qsc" */	/* breakdisplay */
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
              if (IInmuact(ERget(F_RM0017_Activate),
              ERget(F_RM0069_Activ_db_expl),2,2,1) == 0) goto IIfdE4;
              IIFRInternal(0);
              if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE4;
              if (IInmuact(ERget(F_RM004D_DeActivate),
              ERget(F_RM006A_Deactiv_db_expl),2,2,2) == 0) goto IIfdE4;
              if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),2,2,3) 
              == 0) goto IIfdE4;
              IIFRInternal(0);
              if (IInfrskact(9,(char *)0,2,2,3) == 0) goto IIfdE4;
              if (IIendmu() == 0) goto IIfdE4;
              goto IIfdI4;
IIfdE4:;
              IIendnest();
            }
/* # line 355 "activate.qsc" */	/* host code */
		CheckReturnCode(what, retval);
          }
        } else if (IIretval() == 4) {
          {
/* # line 360 "activate.qsc" */	/* host code */
		retval = NOT_CALLED;
		what = NO_ACTION_YET;
/* # line 362 "activate.qsc" */	/* set_frs */
            {
              if (IIiqset(4,0,form_name,tf_name,(char *)0) != 0) {
                IIstfsio(54,fstatus,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 364 "activate.qsc" */	/* display */
            {
              if (IInestmu() == 0) goto IIfdE5;
              goto IImuI5;
IIfdI5:;
              if (IIfsetio((char *)0) == 0) goto IIfdE5;
              {
/* # line 367 "activate.qsc" */	/* resume */
                {
                  IIresmu();
                  if (1) goto IIfdB5;
                }
              }
IIfdB5:
              while (IIrunnest() != 0) {
                if (IIretval() == 1) {
                  {
/* # line 372 "activate.qsc" */	/* host code */
			what = TBL_ACTIVATE;
/* # line 373 "activate.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 374 "activate.qsc" */	/* unloadtable */
                    {
                      if (IItbact(form_name,tf_name,0) == 0) goto IItbE5;
                      while (IItunload() != 0) {
                        IItcogetio((short *)0,1,30,2,&cdds_no,fcdds_no);
                        IItcogetio((short *)0,1,32,DB_MAXNAME*2+3-1,
                        full_db_name,ffull_db_name);
                        IItcogetio((short *)0,1,32,4,status,fstatus);
                        IITBceColEnd();
                        {
/* # line 377 "activate.qsc" */	/* host code */
				if (!STequal(status, done))
				{
					retval = turn_recorder(full_db_name,
						cdds_no, TRUE);
					if (retval > 0)
					{
/* # line 383 "activate.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(fstatus,(short *)0,1,32,0,err);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
/* # line 385 "activate.qsc" */	/* endloop */
                          {
                            if (1) goto IItbE5;
                          }
/* # line 386 "activate.qsc" */	/* host code */
					}
/* # line 387 "activate.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(fstatus,(short *)0,1,32,0,done);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
/* # line 389 "activate.qsc" */	/* host code */
				}
                        }
                      } /* IItunload */
IItbE5:
                      IItunend();
                    }
/* # line 391 "activate.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 392 "activate.qsc" */	/* redisplay */
                    {
                      IIredisp();
                    }
/* # line 393 "activate.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE5;
                    }
                  }
                } else if (IIretval() == 2) {
                  {
/* # line 399 "activate.qsc" */	/* host code */
			what = TBL_DEACT;
/* # line 400 "activate.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 401 "activate.qsc" */	/* unloadtable */
                    {
                      if (IItbact(form_name,tf_name,0) == 0) goto IItbE6;
                      while (IItunload() != 0) {
                        IItcogetio((short *)0,1,30,2,&cdds_no,fcdds_no);
                        IItcogetio((short *)0,1,32,DB_MAXNAME*2+3-1,
                        full_db_name,ffull_db_name);
                        IITBceColEnd();
                        {
/* # line 404 "activate.qsc" */	/* host code */
				retval = turn_recorder(full_db_name, cdds_no,
					FALSE);
				if (retval > 0)
				{
/* # line 408 "activate.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(fstatus,(short *)0,1,32,0,err);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
/* # line 410 "activate.qsc" */	/* endloop */
                          {
                            if (1) goto IItbE6;
                          }
/* # line 411 "activate.qsc" */	/* host code */
				}
/* # line 412 "activate.qsc" */	/* putrow */
                          {
                            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                              IItcoputio(fstatus,(short *)0,1,32,0,blank);
                              IITBceColEnd();
                            } /* IItbsetio */
                          }
                        }
                      } /* IItunload */
IItbE6:
                      IItunend();
                    }
/* # line 414 "activate.qsc" */	/* clear */
                    {
                      IIclrscr();
                    }
/* # line 415 "activate.qsc" */	/* redisplay */
                    {
                      IIredisp();
                    }
/* # line 416 "activate.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE5;
                    }
                  }
                } else if (IIretval() == 3) {
                  {
/* # line 422 "activate.qsc" */	/* breakdisplay */
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
              if (IInmuact(ERget(F_RM0017_Activate),
              ERget(F_RM006D_Activ_all_expl),2,2,1) == 0) goto IIfdE5;
              IIFRInternal(0);
              if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE5;
              if (IInmuact(ERget(F_RM004D_DeActivate),
              ERget(F_RM006E_Deactiv_all_expl),2,2,2) == 0) goto IIfdE5;
              if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),2,2,3) 
              == 0) goto IIfdE5;
              IIFRInternal(0);
              if (IInfrskact(9,(char *)0,2,2,3) == 0) goto IIfdE5;
              if (IIendmu() == 0) goto IIfdE5;
              goto IIfdI5;
IIfdE5:;
              IIendnest();
            }
/* # line 425 "activate.qsc" */	/* host code */
		CheckReturnCode(what, retval);
          }
        } else if (IIretval() == 5) {
          {
/* # line 430 "activate.qsc" */	/* host code */
		FEtabfnd(form_name, tf_name);
          }
        } else if (IIretval() == 6) {
          {
/* # line 435 "activate.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 7) {
          {
/* # line 440 "activate.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 8) {
          {
/* # line 446 "activate.qsc" */	/* host code */
		FEhelp(ERx("rmactchg.hlp"), ERget(F_RM006F_Activ_change_rec));
          }
        } else if (IIretval() == 9) {
          {
/* # line 452 "activate.qsc" */	/* set_frs */
            {
              if (IIiqset(4,0,form_name,tf_name,(char *)0) != 0) {
                IIstfsio(54,fstatus,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 453 "activate.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(F_RM005F_ThisRow),ERget(F_RM0060_ThisRow_expl),2,2,1)
       == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM000F_CDDS),ERget(F_RM0064_Cdds_expl),2,2,2) == 0
      ) goto IIfdE1;
      if (IInmuact(ERget(F_RM0067_Database),ERget(F_RM0068_Database_expl),2,2,
      3) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM006B_All),ERget(F_RM006C_All_expl),2,2,4) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,5) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,7) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),2,2,8) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,8) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,9) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,9) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
  }
/* # line 458 "activate.qsc" */	/* host code */
/*{
** Name:	CheckReturnCode	- check return code
**
** Description:
**	Check the return value from turn_recorder and issue an appropriate
**	message in case of error.
**
** Inputs:
**	what	- activation or deactivation
**	retval	- return value from turn_recorder
**
** Outputs:
**	none
**
** Returns:
**	none
*/
static void
CheckReturnCode(
i4	act_type,
i4	retval)
  {
    char cdds_name[DB_MAXNAME+1];
    char full_db_name[DB_MAXNAME*2+3];
	if (retval > 0)
	{
/* # line 485 "activate.qsc" */	/* getrow */
    {
      if (IItbsetio(2,form_name,tf_name,-2) != 0) {
        IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,cdds_name,fcdds_name);
        IItcogetio((short *)0,1,32,DB_MAXNAME*2+3-1,full_db_name,ffull_db_name);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 487 "activate.qsc" */	/* host code */
		if (act_type == TBL_DEACT)
			IIUGerr(E_RM0054_Error_deactiv_cdds_db, UG_ERR_ERROR,
				2, cdds_name, full_db_name);
		else
			IIUGerr(E_RM0055_Error_activ_cdds_db, UG_ERR_ERROR,
				2, cdds_name, full_db_name);
	}
  }
