# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <cm.h>
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
** Name:	evtlist.qsc - list of dbevents
**
** Description:
**	Defines
**		evt_list	- list of dbevents
**
** History:
**	16-dec-96 (joea)
**		Created based on events.osq in replicator library.
**	07-feb-97 (joea)
**		Add FRSkey4 to Send menu option.
**	02-apr-97 (joea)
**		Do not show dd_ping event.
**	26-jun-98 (abbjo03) bug 91700
**		Expand the startup_value buffer to the column length.
**	18-aug-98 (mcgem01)
**		Forms based utilities must use IIUFprompt in place of
**		IIUGprompt.
**/
# define	SEND_TO_ALL	((i2)-1)
GLOBALREF
  char RMdb_display[];
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmevents"), *tf_name = ERx("iitf"), *fevt_name = ERx("dbevent")
  , *fs_flag = ERx("s_flag"), *fstartup_value = ERx("startup_value"), *
  fevt_descr = ERx("event_desc");
FUNC_EXTERN void evt_raise(char *evt_name, char *flag_name, i2 server_no);
FUNC_EXTERN void evt_send(char *evt_name, char *startup_value);
/*{
** Name:	evt_list	- list of dbevents
**
** Description:
**	Send Events to Replication Servers
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
evt_list()
  {
  char evt_name[DB_MAXNAME+1];
  char startup_value[9];
  i2 s_flag;
  i4 sort_order;
  char evt_descr[501];
    char buff[FE_PROMPTSIZE+1];
	char	ans[6];		/* to get database or CDDS number */
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
/* # line 99 "evtlist.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"r") != 0) {
        IIthidecol(fs_flag,(char *)"i2 not null");
        IItfill();
      } /* IItbinit */
    }
/* # line 100 "evtlist.qsc" */	/* host code */
	IIUGmsg(ERget(F_RM001C_Selecting_data), FALSE, 0);
/* # line 94 "evtlist.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbevent, '', s_flag, sort_order, event_desc from dd_events whe\
re action in(2, 4)union select 'dd_set_server', startup_value, 1, 0, f\
lag_description from dd_flag_values order by sort_order, dbevent, 2");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,evt_name);
      IIgetdomio((short *)0,1,32,8,startup_value);
      IIgetdomio((short *)0,1,30,2,&s_flag);
      IIgetdomio((short *)0,1,30,4,&sort_order);
      IIgetdomio((short *)0,1,32,500,evt_descr);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 106 "evtlist.sc" */	/* host code */
/* # line 113 "evtlist.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fevt_name,(short *)0,1,32,0,evt_name);
        IItcoputio(fstartup_value,(short *)0,1,32,0,startup_value);
        IItcoputio(fs_flag,(short *)0,1,30,2,&s_flag);
        IItcoputio(fevt_descr,(short *)0,1,32,0,evt_descr);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 116 "evtlist.qsc" */	/* host code */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	RPdb_error_check(0, &errinfo);
	if (errinfo.errorno)
	{
/* # line 121 "evtlist.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 122 "evtlist.sc" */	/* host code */
		FTbell();
		IIUGerr(E_RM0012_Error_selecting, UG_ERR_ERROR, 0);
		return;
	}
	else if (errinfo.rowcount == 0)
	{
/* # line 128 "evtlist.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 129 "evtlist.sc" */	/* host code */
		IIUGerr(E_RM0013_No_data_found, UG_ERR_ERROR, 0);
		return;
	}
/* # line 132 "evtlist.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 133 "evtlist.sc" */	/* host code */
/* # line 133 "evtlist.qsc" */	/* display */
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
/* # line 138 "evtlist.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,evt_name,fevt_name);
                IItcogetio((short *)0,1,32,8,startup_value,fstartup_value);
                IItcogetio((short *)0,1,30,2,&s_flag,fs_flag);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 140 "evtlist.qsc" */	/* host code */
		if (STequal(startup_value, ERx("QCDn")) ||
			STequal(startup_value, ERx("UCDn")) ||
			STequal(startup_value, ERx("QDBn")) ||
			STequal(startup_value, ERx("UDBn")))
		{
			if (STequal(startup_value, ERx("QDBn")) ||
					STequal(startup_value, ERx("UDBn")))
				IIUFprompt(ERget(S_RM0065_Enter_db_no), 1,
					FALSE, ans, sizeof(ans), 0);
			else
				IIUFprompt(ERget(S_RM0066_Enter_cdds_no), 1,
					FALSE, ans, sizeof(ans), 0);
			STlcopy(startup_value, buff, 3);
			STcat(buff, ans);
		}
		else
		{
			STcopy(startup_value, buff);
		}
		if (s_flag == 0)
			evt_raise(evt_name, ERx(""), SEND_TO_ALL);
		else
			evt_send(evt_name, buff);
          }
        } else if (IIretval() == 2) {
          {
/* # line 169 "evtlist.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,32,500,evt_descr,fevt_descr);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 170 "evtlist.qsc" */	/* message */
            {
              IIFRgpcontrol(1,0);
              IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
              IIFRgpcontrol(2,0);
              IImessage(evt_descr);
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 175 "evtlist.qsc" */	/* host code */
		FEtabfnd(form_name, tf_name);
          }
        } else if (IIretval() == 4) {
          {
/* # line 180 "evtlist.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 185 "evtlist.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 191 "evtlist.qsc" */	/* host code */
		FEhelp(ERx("rmevents.hlp"), ERget(F_RM0093_Send_events));
          }
        } else if (IIretval() == 7) {
          {
/* # line 197 "evtlist.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(F_RM008F_Send),ERget(F_RM0090_Send_expl),2,2,1) == 0
      ) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM0091_Description),
      ERget(F_RM0092_Description_expl),2,2,2) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),2,2,6) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,7) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,7) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
  }
