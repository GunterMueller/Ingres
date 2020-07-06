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
** Name:	evtsend.qsc - send a dbevent to a RepServer
**
** Description:
**	Defines
**		evt_send	- send a dbevent to a RepServer
**		evt_raise	- raise a dbevent
**
** History:
**	03-dec-96 (joea)
**		Created based on send.osq in replicator library.
**	24-oct-97 (joea)
**		Commit after raising a dbevent in order not to leave an open
**		transaction.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
# define	SEND_TO_ALL	((i2)-1)
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmsend_event"), *tf_name = ERx("iitf"), *fsvr_no = ERx("server_no")
  ;
void evt_raise(char *evt_name, char *flag_name, i2 svr_no);
/*{
** Name:	evt_send - send dbevent to a RepServer
**
** Description:
**	Displays a list of active of active RepServers and allows user to
**	send a dbevent to one or all of the servers.
**
** Inputs:
**	evt_name	- event name
**	startup_value	- server startup parameter
**
** Outputs:
**	none
**
** Returns:
**	none
*/
void
evt_send(
char	*evt_name,
char	*startup_value)
  {
  i2 svr_no;
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
/* # line 87 "evtsend.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 88 "evtsend.qsc" */	/* host code */
	IIUGmsg(ERget(F_RM001C_Selecting_data), FALSE, 0);
/* # line 86 "evtsend.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select server_no from dd_servers where pid!='' order by server_no");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&svr_no);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 92 "evtsend.sc" */	/* host code */
/* # line 95 "evtsend.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fsvr_no,(short *)0,1,30,2,&svr_no);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 96 "evtsend.qsc" */	/* host code */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	RPdb_error_check(0, &errinfo);
	if (errinfo.errorno)
	{
/* # line 104 "evtsend.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 105 "evtsend.sc" */	/* host code */
		FTbell();
		IIUGerr(E_RM0012_Error_selecting, UG_ERR_ERROR, 0);
		return;
	}
	else if (errinfo.rowcount == 0)
	{
/* # line 111 "evtsend.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 112 "evtsend.sc" */	/* host code */
		IIUGerr(E_RM0013_No_data_found, UG_ERR_ERROR, 0);
		return;
	}
/* # line 115 "evtsend.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 116 "evtsend.sc" */	/* host code */
/* # line 113 "evtsend.qsc" */	/* display */
    {
      if (IIdispfrm(form_name,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 117 "evtsend.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,2,&svr_no,fsvr_no);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 118 "evtsend.qsc" */	/* host code */
		evt_raise(evt_name, startup_value, svr_no);
          }
        } else if (IIretval() == 2) {
          {
/* # line 124 "evtsend.qsc" */	/* host code */
		evt_raise(evt_name, startup_value, SEND_TO_ALL);
          }
        } else if (IIretval() == 3) {
          {
/* # line 129 "evtsend.qsc" */	/* host code */
		FEtabfnd(form_name, tf_name);
          }
        } else if (IIretval() == 4) {
          {
/* # line 134 "evtsend.qsc" */	/* scroll */
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
/* # line 139 "evtsend.qsc" */	/* scroll */
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
/* # line 145 "evtsend.qsc" */	/* host code */
		FEhelp(ERx("rmevtsnd.hlp"), ERget(F_RM0096_Select_svr_to_send));
          }
        } else if (IIretval() == 7) {
          {
/* # line 151 "evtsend.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(F_RM008F_Send),ERget(F_RM0094_Send_one_expl),2,2,1) 
      == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM006B_All),ERget(F_RM0095_Send_all_expl),2,2,2) ==
       0) goto IIfdE1;
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
/* # line 156 "evtsend.qsc" */	/* host code */
/*{
** Name:	evt_raise - raise a dbevent
**
** Description:
**	Raises dbevent to notify server(s) about changed startup flag.  If
**	svr_no is SEND_TO_ALL, sends an event to all servers.
**
** Inputs:
**	evt_name		- event name
**	flag_name	- server parameter
**	svr_no		- server number
**
** Outputs:
**	none
**
** Returns:
**	none
*/
void
evt_raise(
char	*evt_name,
char	*flag_name,
i2	svr_no)
{
	char	evt_name2[DB_MAXNAME+1];
  char stmt[256];
	STtrmwhite(evt_name);
	STtrmwhite(flag_name);
	if (svr_no == SEND_TO_ALL)
		STcopy(evt_name, evt_name2);
	else
		STprintf(evt_name2, ERx("%s%-d"), evt_name, (i4)svr_no);
	if (flag_name != NULL && *flag_name != EOS)
		STprintf(stmt, ERx("RAISE DBEVENT %s '%s'"), evt_name2,
			flag_name);
	else
		STprintf(stmt, ERx("RAISE DBEVENT %s"), evt_name2);
/* # line 254 "evtsend.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
  }
/* # line 255 "evtsend.sc" */	/* host code */
	if (RPdb_error_check(0, NULL) != OK)
		IIUGerr(E_RM0067_Error_issuing_event, UG_ERR_ERROR, 0);
	else
		IIUGerr(S_RM0068_Event_issued, 0, 0);
/* # line 259 "evtsend.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 260 "evtsend.sc" */	/* host code */
}
