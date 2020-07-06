# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <cm.h>
# include <cv.h>
# include <st.h>
# include <er.h>
# include <pc.h>
# include <te.h>
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
** Name:	monitor.qsc - replication monitor
**
** Description:
**	Defines
**		rep_monitor		- replication monitor
**		load_server_tblfld	- load the servers tablefield
**		refresh_queue_stats	- refresh queue statistics
**
** History:
**	03-dec-96 (joea)
**		Created based on monitor.osq in replicator library.
**	30-jan-97 (joea)
**		Remove the 'Server has shutdown' message when user cancels a
**		Stop action on a server that is not running. Always reset the
**		counter when a dd_serverN event is received.
**	24-oct-97 (joea)
**		Make sure that we commit after raising dbevents in order not to
**		leave an open transaction.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**    25-Oct-2005 (hanje04)
**        Add prototype for load_server_tblfld() to prevent compiler
**        errors with GCC 4.0 due to conflict with implicit declaration.
**/
# define MON_TIMEOUT 5
GLOBALREF
  char RMdb_display[];
GLOBALREF
  i2 RMlocal_db_no;
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmreplic_monitor"), *tf_name = ERx("servers"), *
  tf2_name = ERx("tblfield"), *fsvr_no = ERx("server_no"), *fsvr_name = ERx("server_name")
  , *fcounter = ERx("counter"), *fsvr_status = ERx("ebuff"), *finput_cnt = ERx("input")
  , *fdistrib_cnt = ERx("distribution"), *ftrans_info = ERx("ebuff"), *
  ftrans_cnt = ERx("num");
static bool	start = TRUE;
static STATUS load_server_tblfield(void);
static void refresh_queue_stats(void);
static STATUS load_server_tblfld(void);
/*{
** Name:	rep_monitor - replication monitor
**
** Description:
**	Handles processing on monitor screen.
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
rep_monitor()
  {
  char evt_name[DB_MAXNAME+1];
  char stmt[81];
  char evt_text[132];
  i4 cnt;
    i2 svr_no;
    char svr_name[25];
    char trans_info[132];
    char fld_name[FE_MAXNAME+1];
    char run_cmd[81];
    char *shell_menu;
    i4 counter;
	bool	found;
	bool	svr_evt;
	CL_ERR_DESC	err_code;
# ifdef VMS
	shell_menu = ERget(FE_Spawn);
# else
	shell_menu = ERget(FE_Shell);
# endif
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
	if (start)
	{
/* # line 136 "monitor.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"r") != 0) {
        IIthidecol(fsvr_name,(char *)"char(24) not null");
        IIthidecol(fcounter,(char *)"i4 not null");
        IItfill();
      } /* IItbinit */
    }
/* # line 138 "monitor.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf2_name,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 139 "monitor.qsc" */	/* host code */
		if (load_server_tblfld() != OK)
			return;
/* # line 135 "monitor.sc" */	/* register dbevent */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"register dbevent dd_insert");
    IIsyncup((char *)0,0);
  }
/* # line 136 "monitor.sc" */	/* register dbevent */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"register dbevent dd_update");
    IIsyncup((char *)0,0);
  }
/* # line 137 "monitor.sc" */	/* register dbevent */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"register dbevent dd_delete");
    IIsyncup((char *)0,0);
  }
/* # line 138 "monitor.sc" */	/* register dbevent */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"register dbevent dd_transact\
ion");
    IIsyncup((char *)0,0);
  }
/* # line 139 "monitor.sc" */	/* register dbevent */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"register dbevent dd_insert2");
    IIsyncup((char *)0,0);
  }
/* # line 140 "monitor.sc" */	/* register dbevent */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"register dbevent dd_update2");
    IIsyncup((char *)0,0);
  }
/* # line 141 "monitor.sc" */	/* register dbevent */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"register dbevent dd_delete2");
    IIsyncup((char *)0,0);
  }
/* # line 142 "monitor.sc" */	/* register dbevent */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"register dbevent dd_transact\
ion2");
    IIsyncup((char *)0,0);
  }
/* # line 143 "monitor.sc" */	/* host code */
/* # line 151 "monitor.qsc" */	/* unloadtable */
    {
      if (IItbact(form_name,tf_name,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,24,svr_name,fsvr_name);
        IITBceColEnd();
        {
/* # line 153 "monitor.qsc" */	/* host code */
			STprintf(stmt, ERx("REGISTER DBEVENT %s"), svr_name);
/* # line 152 "monitor.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
  }
/* # line 153 "monitor.sc" */	/* host code */
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 157 "monitor.qsc" */	/* host code */
/* # line 159 "monitor.sc" */	/* raise dbevent */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"raise dbevent dd_ping");
    IIsyncup((char *)0,0);
  }
/* # line 160 "monitor.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 161 "monitor.sc" */	/* host code */
		start = FALSE;
	}
	refresh_queue_stats();
/* # line 163 "monitor.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(MON_TIMEOUT),
        (void *)IILQint(MON_TIMEOUT));
      } /* IIiqset */
    }
/* # line 165 "monitor.qsc" */	/* display */
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
/* # line 169 "monitor.qsc" */	/* host code */
		while (TRUE)
		{
/* # line 185 "monitor.sc" */	/* get dbevent */
  {
    IIsqInit((char *)0);
    IILQesEvStat(0,0);
  }
/* # line 186 "monitor.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,32,DB_MAXNAME+1-1,evt_name,74);
    IILQisInqSqlio((short *)0,1,32,131,evt_text,78);
  }
/* # line 188 "monitor.sc" */	/* host code */
			CVlower(evt_name);
			if (*evt_name == EOS)
				break;
			svr_evt = FALSE;
			if (STequal(evt_name, ERx("dd_insert")) ||
				STequal(evt_name, ERx("dd_update")) ||
				STequal(evt_name, ERx("dd_delete")) ||
				STequal(evt_name, ERx("dd_transaction")) ||
				STequal(evt_name, ERx("dd_insert2")) ||
				STequal(evt_name, ERx("dd_update2")) ||
				STequal(evt_name, ERx("dd_delete2")) ||
				STequal(evt_name, ERx("dd_transaction2")))
			{
/* # line 188 "monitor.qsc" */	/* getform */
            {
              if (IIfsetio(form_name) != 0) {
                IIgetfldio((short *)0,1,30,4,&cnt,evt_name);
              } /* IIfsetio */
            }
/* # line 189 "monitor.qsc" */	/* host code */
				++cnt;
/* # line 190 "monitor.qsc" */	/* putform */
            {
              if (IIfsetio(form_name) != 0) {
                IIputfldio(evt_name,(short *)0,1,30,4,&cnt);
              } /* IIfsetio */
            }
/* # line 191 "monitor.qsc" */	/* host code */
			}
			else
			{
/* # line 194 "monitor.qsc" */	/* unloadtable */
            {
              if (IItbact(form_name,tf_name,0) == 0) goto IItbE2;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,32,24,svr_name,fsvr_name);
                IITBceColEnd();
                {
/* # line 197 "monitor.qsc" */	/* host code */
					if (STequal(evt_name, svr_name))
					{
/* # line 199 "monitor.qsc" */	/* putrow */
                  {
                    if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                      IItcoputio(fsvr_status,(short *)0,1,32,0,evt_text);
                      IItcoputio(fcounter,(short *)0,1,30,sizeof(0),
                      (void *)IILQint(0));
                      IITBceColEnd();
                    } /* IItbsetio */
                  }
/* # line 202 "monitor.qsc" */	/* endloop */
                  {
                    if (1) goto IItbE2;
                  }
/* # line 203 "monitor.qsc" */	/* host code */
					}
                }
              } /* IItunload */
IItbE2:
              IItunend();
            }
/* # line 205 "monitor.qsc" */	/* host code */
				svr_evt = TRUE;
			}
			if (!svr_evt)
			{
				found = FALSE;
/* # line 211 "monitor.qsc" */	/* unloadtable */
            {
              if (IItbact(form_name,tf2_name,0) == 0) goto IItbE3;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,32,131,trans_info,ftrans_info);
                IItcogetio((short *)0,1,30,4,&cnt,ftrans_cnt);
                IITBceColEnd();
                {
/* # line 214 "monitor.qsc" */	/* host code */
					if (STequal(evt_text, trans_info))
					{
						++cnt;
/* # line 217 "monitor.qsc" */	/* putrow */
                  {
                    if (IItbsetio(3,form_name,tf2_name,-2) != 0) {
                      IItcoputio(ftrans_cnt,(short *)0,1,30,4,&cnt);
                      IITBceColEnd();
                    } /* IItbsetio */
                  }
/* # line 219 "monitor.qsc" */	/* host code */
						found = TRUE;
/* # line 220 "monitor.qsc" */	/* endloop */
                  {
                    if (1) goto IItbE3;
                  }
/* # line 221 "monitor.qsc" */	/* host code */
					}
                }
              } /* IItunload */
IItbE3:
              IItunend();
            }
/* # line 224 "monitor.qsc" */	/* host code */
				if (!found)
/* # line 225 "monitor.qsc" */	/* loadtable */
            {
              if (IItbact(form_name,tf2_name,1) != 0) {
                IItcoputio(ftrans_info,(short *)0,1,32,0,evt_text);
                IItcoputio(ftrans_cnt,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
                IITBceColEnd();
              } /* IItbact */
            }
/* # line 228 "monitor.qsc" */	/* host code */
			}
		}
/* # line 231 "monitor.qsc" */	/* unloadtable */
            {
              if (IItbact(form_name,tf_name,0) == 0) goto IItbE4;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,30,2,&svr_no,fsvr_no);
                IItcogetio((short *)0,1,30,4,&counter,fcounter);
                IITBceColEnd();
                {
/* # line 234 "monitor.qsc" */	/* host code */
			if (counter != 0)
				--counter;
			if (counter == 1)
			{
				counter = 0;
				if (!is_server_running(svr_no))
/* # line 240 "monitor.qsc" */	/* putrow */
                  {
                    if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                      IItcoputio(fsvr_status,(short *)0,1,32,0,
                      ERget(S_RM007E_Server_couldnt_start));
                      IITBceColEnd();
                    } /* IItbsetio */
                  }
/* # line 242 "monitor.qsc" */	/* host code */
			}
/* # line 243 "monitor.qsc" */	/* putrow */
                  {
                    if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                      IItcoputio(fcounter,(short *)0,1,30,4,&counter);
                      IITBceColEnd();
                    } /* IItbsetio */
                  }
                }
              } /* IItunload */
IItbE4:
              IItunend();
            }
/* # line 246 "monitor.qsc" */	/* redisplay */
            {
              IIredisp();
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 252 "monitor.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,fld_name,19,(char *)0,
                0);
              } /* IIiqset */
            }
/* # line 254 "monitor.qsc" */	/* host code */
		if (!STequal(fld_name, tf_name))
		{
			IIUGerr(E_RM0056_Must_be_on_server_tf, UG_ERR_ERROR,
				0);
/* # line 258 "monitor.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 259 "monitor.qsc" */	/* host code */
		}
/* # line 261 "monitor.qsc" */	/* display */
            {
              if (IInestmu() == 0) goto IIfdE2;
              goto IImuI2;
IIfdI2:;
IIfdB2:
              while (IIrunnest() != 0) {
                if (IIretval() == 1) {
                  {
/* # line 265 "monitor.qsc" */	/* getrow */
                    {
                      if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                        IItcogetio((short *)0,1,30,2,&svr_no,fsvr_no);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 266 "monitor.qsc" */	/* host code */
			if (!RMopt_exists(svr_no))
			{
				IIUGerr(E_RM0057_No_config_file, UG_ERR_ERROR,
					0);
/* # line 270 "monitor.qsc" */	/* resume */
                    {
                      if (1) goto IIfdB2;
                    }
/* # line 271 "monitor.qsc" */	/* host code */
			}
			if (is_server_running(svr_no))
			{
				IIUGerr(E_RM0058_Svr_already_running,
					UG_ERR_ERROR, 0);
/* # line 277 "monitor.qsc" */	/* resume */
                    {
                      if (1) goto IIfdB2;
                    }
/* # line 278 "monitor.qsc" */	/* host code */
			}
			/* Give the server 2 mins (24 * 5 secs) to start */
/* # line 281 "monitor.qsc" */	/* putrow */
                    {
                      if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                        IItcoputio(fcounter,(short *)0,1,30,sizeof(25),
                        (void *)IILQint(25));
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 282 "monitor.qsc" */	/* host code */
			STprintf(run_cmd, "rpserver %d", (i4)svr_no);
			TErestore(TE_NORMAL);
			PCcmdline(NULL, run_cmd, PC_WAIT, NULL, &err_code);
			TErestore(TE_FORMS);
/* # line 287 "monitor.qsc" */	/* putrow */
                    {
                      if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                        IItcoputio(fsvr_status,(short *)0,1,32,0,
                        ERget(F_RM0077_Starting_rep_server));
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
                  }
                } else if (IIretval() == 2) {
                  {
/* # line 294 "monitor.qsc" */	/* putrow */
                    {
                      if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                        IItcoputio(fcounter,(short *)0,1,30,sizeof(0),
                        (void *)IILQint(0));
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 295 "monitor.qsc" */	/* getrow */
                    {
                      if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                        IItcogetio((short *)0,1,30,2,&svr_no,fsvr_no);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 296 "monitor.qsc" */	/* host code */
			if (!is_server_running(svr_no))
			{
				if (IIUFccConfirmChoice(CONF_GENERIC, NULL,
					NULL, NULL, NULL,
					S_RM0059_Server_not_running,
					S_RM005A_Yes_stop, S_RM005B_No_stop,
					ERx(""), TRUE) != CONFCH_YES)
/* # line 303 "monitor.qsc" */	/* resume */
                    {
                      if (1) goto IIfdB2;
                    }
/* # line 304 "monitor.qsc" */	/* host code */
			}
			STprintf(stmt, ERx("RAISE DBEVENT dd_stop_server%d"),
				(i4)svr_no);
/* # line 456 "monitor.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
  }
/* # line 457 "monitor.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 458 "monitor.sc" */	/* host code */
/* # line 311 "monitor.qsc" */	/* putrow */
                    {
                      if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                        IItcoputio(fsvr_status,(short *)0,1,32,0,
                        ERget(F_RM007A_Stopping_rep_server));
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
                  }
                } else if (IIretval() == 3) {
                  {
/* # line 318 "monitor.qsc" */	/* set_frs */
                    {
                      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) 
                      {
                        IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
                        (void *)IILQint(0));
                      } /* IIiqset */
                    }
/* # line 319 "monitor.qsc" */	/* getrow */
                    {
                      if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                        IItcogetio((short *)0,1,30,2,&svr_no,fsvr_no);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 320 "monitor.qsc" */	/* host code */
			edit_cfg_file(svr_no);
/* # line 321 "monitor.qsc" */	/* set_frs */
                    {
                      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) 
                      {
                        IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(
                        MON_TIMEOUT),(void *)IILQint(MON_TIMEOUT));
                      } /* IIiqset */
                    }
                  }
                } else if (IIretval() == 4) {
                  {
/* # line 327 "monitor.qsc" */	/* set_frs */
                    {
                      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) 
                      {
                        IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
                        (void *)IILQint(0));
                      } /* IIiqset */
                    }
/* # line 328 "monitor.qsc" */	/* getrow */
                    {
                      if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                        IItcogetio((short *)0,1,30,2,&svr_no,fsvr_no);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 329 "monitor.qsc" */	/* host code */
			svr_show_assignments(svr_no);
/* # line 330 "monitor.qsc" */	/* set_frs */
                    {
                      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) 
                      {
                        IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(
                        MON_TIMEOUT),(void *)IILQint(MON_TIMEOUT));
                      } /* IIiqset */
                    }
                  }
                } else if (IIretval() == 5) {
                  {
/* # line 336 "monitor.qsc" */	/* breakdisplay */
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
              if (IInmuact(ERget(F_RM0075_Start),ERget(F_RM0076_Start_expl),2,
              2,1) == 0) goto IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE2;
              if (IInmuact(ERget(F_RM0078_Stop),ERget(F_RM0079_Stop_expl),2,2,
              2) == 0) goto IIfdE2;
              if (IInmuact(ERget(F_RM007B_EditConfig),
              ERget(F_RM007C_EditConfig_expl),2,2,3) == 0) goto IIfdE2;
              if (IInmuact(ERget(F_RM007D_ShowAssign),
              ERget(F_RM007E_ShowAssign_expl),2,2,4) == 0) goto IIfdE2;
              if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),2,2,5) 
              == 0) goto IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(9,(char *)0,2,2,5) == 0) goto IIfdE2;
              if (IIendmu() == 0) goto IIfdE2;
              goto IIfdI2;
IIfdE2:;
              IIendnest();
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 343 "monitor.qsc" */	/* host code */
/* # line 562 "monitor.sc" */	/* raise dbevent */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"raise dbevent dd_ping");
    IIsyncup((char *)0,0);
  }
/* # line 563 "monitor.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 564 "monitor.sc" */	/* host code */
          }
        } else if (IIretval() == 4) {
          {
/* # line 350 "monitor.qsc" */	/* host code */
		refresh_queue_stats();
          }
        } else if (IIretval() == 5) {
          {
/* # line 356 "monitor.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 357 "monitor.qsc" */	/* host code */
		evt_list();
/* # line 358 "monitor.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(MON_TIMEOUT),
                (void *)IILQint(MON_TIMEOUT));
              } /* IIiqset */
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 363 "monitor.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 364 "monitor.qsc" */	/* call */
            {
              IIutsys(0,(char *)"system",(char *)0);
              IIutsys(1,(char *)"command",ERx(""));
              IIutsys(2,(char *)0,(char *)0);
            }
/* # line 365 "monitor.qsc" */	/* redisplay */
            {
              IIredisp();
            }
/* # line 366 "monitor.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(MON_TIMEOUT),
                (void *)IILQint(MON_TIMEOUT));
              } /* IIiqset */
            }
          }
        } else if (IIretval() == 7) {
          {
/* # line 372 "monitor.qsc" */	/* clear */
            {
              IIclrflds();
            }
/* # line 373 "monitor.qsc" */	/* putform */
            {
              if (IIfsetio(form_name) != 0) {
                IIputfldio((char *)"db",(short *)0,1,32,0,RMdb_display);
              } /* IIfsetio */
            }
/* # line 374 "monitor.qsc" */	/* host code */
		if (load_server_tblfld() != OK)
/* # line 375 "monitor.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 376 "monitor.qsc" */	/* host code */
/* # line 635 "monitor.sc" */	/* raise dbevent */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"raise dbevent dd_ping");
    IIsyncup((char *)0,0);
  }
/* # line 636 "monitor.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 637 "monitor.sc" */	/* host code */
          }
        } else if (IIretval() == 8) {
          {
/* # line 383 "monitor.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 384 "monitor.qsc" */	/* host code */
		FEhelp(ERx("rmmonitr.hlp"),
			ERget(F_RM0085_Replication_monitor));
/* # line 386 "monitor.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(MON_TIMEOUT),
                (void *)IILQint(MON_TIMEOUT));
              } /* IIiqset */
            }
          }
        } else if (IIretval() == 9) {
          {
/* # line 392 "monitor.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 393 "monitor.qsc" */	/* breakdisplay */
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
      if (IIFRtoact(0,1) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM0072_Server),ERget(F_RM0073_Server_expl),2,2,2) 
      == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM007F_Ping),ERget(F_RM0080_Ping_expl),2,2,3) == 0
      ) goto IIfdE1;
      if (IInmuact(ERget(F_RM0081_Queues),ERget(F_RM0082_Queues_expl),2,2,4) 
      == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM0083_Events),ERget(F_RM0084_Events_expl),2,2,5) 
      == 0) goto IIfdE1;
      if (IInmuact(shell_menu,ERget(F_FE010A_ExplShell),2,2,6) == 0) goto 
      IIfdE1;
      if (IInmuact(ERget(F_RM000A_Clear),ERget(F_RM000B_Clear_expl),2,2,7) == 
      0) goto IIfdE1;
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
/* # line 398 "monitor.qsc" */	/* host code */
/*{
** Name:	load_server_tblfld - load the servers tablefield
**
** Description:
**	Reinitializes the servers tablefield from the dd_servers table.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Returns:
**	OK or FAIL.
*/
static STATUS
load_server_tblfld()
  {
  i2 svr_no;
  char svr_name[25];
	DBEC_ERR_INFO	errinfo;
/* # line 732 "monitor.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct s.server_no, server_name from dd_servers s, dd_db_cdd\
s c where s.server_no=c.server_no and c.database_no!=");
    IIputdomio((short *)0,1,30,2,&RMlocal_db_no);
    IIwritio(0,(short *)0,1,32,0,(char *)"order by server_no");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&svr_no);
      IIgetdomio((short *)0,1,32,24,svr_name);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 739 "monitor.sc" */	/* host code */
/* # line 429 "monitor.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fsvr_no,(short *)0,1,30,2,&svr_no);
        IItcoputio(fsvr_name,(short *)0,1,32,0,svr_name);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 431 "monitor.qsc" */	/* host code */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	RPdb_error_check(0, &errinfo);
/* # line 750 "monitor.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 751 "monitor.sc" */	/* host code */
	if (errinfo.errorno)
	{
		IIUGerr(E_RM0012_Error_selecting, UG_ERR_ERROR, 0);
		return (FAIL);
	}
	else if (errinfo.rowcount == 0)
	{
		IIUGerr(E_RM005C_No_server_assignments, UG_ERR_ERROR, 0);
		return (FAIL);
	}
	return (OK);
  }
/*{
** Name:	refresh_queue_stats - refresh queue statistics
**
** Description:
**	Queries the input and distribution queues and displays the number of
**	items on each queue.
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
static void
refresh_queue_stats()
  {
  i4 input_cnt;
  i4 distrib_cnt;
/* # line 786 "monitor.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 787 "monitor.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE session where READLOCK=NOLOCK");
    IIsyncup((char *)0,0);
  }
/* # line 788 "monitor.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_input_queue");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&input_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 791 "monitor.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_distrib_queue");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&distrib_cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 794 "monitor.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 795 "monitor.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE session where READLOCK=session");
    IIsyncup((char *)0,0);
  }
/* # line 796 "monitor.sc" */	/* host code */
/* # line 483 "monitor.qsc" */	/* putform */
    {
      if (IIfsetio(form_name) != 0) {
        IIputfldio(finput_cnt,(short *)0,1,30,4,&input_cnt);
        IIputfldio(fdistrib_cnt,(short *)0,1,30,4,&distrib_cnt);
      } /* IIfsetio */
    }
  }
