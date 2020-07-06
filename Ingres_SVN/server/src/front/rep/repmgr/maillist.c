# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <cm.h>
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
# include "errm.h"
/**
** Name:	maillist.qsc - error mail list
**
** Description:
**	Defines
**		mail_list	- error mail list
**		load_tblfld	- load table field
**
** History:
**	16-dec-96 (joea)
**		Created based on mailcat.osq in replicator library.
**	14-nov-97 (joea)
**		Correct call to IIUGerr.
**/
GLOBALREF
  char RMdb_display[];
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmmail_catalog"), *tf_name = ERx("iitf"), *fmail_addr 
  = ERx("mail_username"), *fold_mail_addr = ERx("old_mail_addr");
static STATUS load_tblfld(void);
/*{
** Name:	mail_list	- error mail list
**
** Description:
**	Update Error Mail List
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
mail_list()
  {
  char mail_addr[81];
  char old_mail_addr[81];
    i4 rownumber;
    i4 rowstate;
    i4 chng;
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
/* # line 89 "maillist.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"f") != 0) {
        IIthidecol(fold_mail_addr,(char *)"varchar(80) not null");
        IItfill();
      } /* IItbinit */
    }
/* # line 91 "maillist.qsc" */	/* host code */
	if (load_tblfld() != OK)
		return;
/* # line 94 "maillist.qsc" */	/* display */
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
/* # line 99 "maillist.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 100 "maillist.qsc" */	/* host code */
		if (!chng)
		{
			IIUGerr(E_RM0029_No_changes_to_save, 0, 0);
/* # line 103 "maillist.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 104 "maillist.qsc" */	/* host code */
		}
/* # line 106 "maillist.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 107 "maillist.qsc" */	/* host code */
		IIUGmsg(ERget(F_RM0009_Saving), FALSE, 0);
		/* Process deleted rows first. */
/* # line 110 "maillist.qsc" */	/* unloadtable */
            {
              if (IItbact(form_name,tf_name,0) == 0) goto IItbE1;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,30,4,&rowstate,(char *)"_STATE");
                IItcogetio((short *)0,1,30,4,&rownumber,(char *)"_RECO\
RD");
                IItcogetio((short *)0,1,32,80,old_mail_addr,fold_mail_addr);
                IITBceColEnd();
                {
/* # line 113 "maillist.qsc" */	/* host code */
			if (rowstate == stDELETE)
			{
				/*
				** delete row using hidden field key in where
				** clause.
				*/
/* # line 142 "maillist.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"maillist1",7216,6452);
      IIputdomio((short *)0,1,32,0,old_mail_addr);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"maillist1",7216,6452);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from dd_mail_alert where mail_username=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,old_mail_addr);
        IIexDefine(0,(char *)"maillist1",7216,6452);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 143 "maillist.sc" */	/* host code */
				if (RPdb_error_check(DBEC_SINGLE_ROW, NULL)
					!= OK)
				{
/* # line 146 "maillist.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 147 "maillist.sc" */	/* host code */
					IIUGerr(E_RM002A_Error_saving_tf,
						UG_ERR_ERROR, 0);
/* # line 127 "maillist.qsc" */	/* scroll */
                  {
                    if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                      IItbsmode((char *)"to");
                      if (IItscroll(0,rownumber) != 0) {
                      } /* IItscroll */
                    } /* IItbsetio */
                  }
/* # line 128 "maillist.qsc" */	/* resume */
                  {
                    IIresfld(tf_name);
                    IItunend();
                    if (1) goto IIfdB1;
                  }
/* # line 129 "maillist.qsc" */	/* host code */
				}
			}
                }
              } /* IItunload */
IItbE1:
              IItunend();
            }
/* # line 133 "maillist.qsc" */	/* host code */
		/* process all but Deleted rows */
/* # line 134 "maillist.qsc" */	/* unloadtable */
            {
              if (IItbact(form_name,tf_name,0) == 0) goto IItbE2;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,30,4,&rowstate,(char *)"_STATE");
                IItcogetio((short *)0,1,30,4,&rownumber,(char *)"_RECO\
RD");
                IItcogetio((short *)0,1,32,80,mail_addr,fmail_addr);
                IItcogetio((short *)0,1,32,80,old_mail_addr,fold_mail_addr);
                IITBceColEnd();
                {
/* # line 138 "maillist.qsc" */	/* host code */
			if (rowstate == stNEW)
/* # line 186 "maillist.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"maillist2",7217,7153);
      IIputdomio((short *)0,1,32,0,mail_addr);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"maillist2",7217,7153);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_mail_alert(mail_username)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,mail_addr);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"maillist2",7217,7153);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 189 "maillist.sc" */	/* host code */
			else if (rowstate == stCHANGE)
				/*
				** Update row using hidden version of key field
				** in where clause.
				*/
/* # line 194 "maillist.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"maillist3",7218,10549);
      IIputdomio((short *)0,1,32,0,mail_addr);
      IIputdomio((short *)0,1,32,0,old_mail_addr);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"maillist3",7218,10549);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_mail_alert set mail_username=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,mail_addr);
        IIwritio(0,(short *)0,1,32,0,(char *)"where mail_username=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,old_mail_addr);
        IIexDefine(0,(char *)"maillist3",7218,10549);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 197 "maillist.sc" */	/* host code */
			if (rowstate == stNEW || rowstate == stCHANGE)
			{
				if (RPdb_error_check(DBEC_SINGLE_ROW, NULL)
					!= OK)
				{
/* # line 202 "maillist.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 203 "maillist.sc" */	/* host code */
					IIUGerr(E_RM002A_Error_saving_tf,
						UG_ERR_ERROR, 0);
/* # line 158 "maillist.qsc" */	/* scroll */
                  {
                    if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                      IItbsmode((char *)"to");
                      if (IItscroll(0,rownumber) != 0) {
                      } /* IItscroll */
                    } /* IItbsetio */
                  }
/* # line 159 "maillist.qsc" */	/* resume */
                  {
                    IIresfld(tf_name);
                    IItunend();
                    if (1) goto IIfdB1;
                  }
/* # line 160 "maillist.qsc" */	/* host code */
				}
			}
                }
              } /* IItunload */
IItbE2:
              IItunend();
            }
/* # line 164 "maillist.qsc" */	/* host code */
/* # line 228 "maillist.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 229 "maillist.sc" */	/* host code */
		if (RPdb_error_check(0, NULL) != OK)
		{
/* # line 231 "maillist.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 232 "maillist.sc" */	/* host code */
			IIUGerr(E_RM000A_Error_committing, UG_ERR_ERROR, 0);
/* # line 169 "maillist.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 170 "maillist.qsc" */	/* host code */
		}
/* # line 172 "maillist.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 173 "maillist.qsc" */	/* host code */
		IIUGmsg(ERget(F_RM0070_Changes_saved), FALSE, 0);
		PCsleep(2000);
/* # line 176 "maillist.qsc" */	/* clear */
            {
              IIfldclear(tf_name);
            }
/* # line 177 "maillist.qsc" */	/* host code */
		if (load_tblfld() != OK)
/* # line 178 "maillist.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 185 "maillist.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,form_name,tf_name,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
/* # line 186 "maillist.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 191 "maillist.qsc" */	/* host code */
		FEtabfnd(form_name, tf_name);
          }
        } else if (IIretval() == 4) {
          {
/* # line 196 "maillist.qsc" */	/* scroll */
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
/* # line 201 "maillist.qsc" */	/* scroll */
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
/* # line 207 "maillist.qsc" */	/* host code */
		FEhelp(ERx("rmmaills.hlp"), ERget(F_RM0071_Update_error_mail));
          }
        } else if (IIretval() == 7) {
          {
/* # line 214 "maillist.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 215 "maillist.qsc" */	/* host code */
		if (chng && IIUFccConfirmChoice(CONF_GENERIC, NULL, NULL,
			NULL, NULL, S_RM000E_End_no_save, S_RM000F_Yes_end,
			S_RM0010_No_end, ERx(""), TRUE) != CONFCH_YES)
/* # line 218 "maillist.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 220 "maillist.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(FE_Save),ERget(F_FE0117_SaveExpl),2,2,1) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM0032_RowDelete),ERget(F_RM0033_RowDelete_expl),0,
      0,2) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(14,(char *)0,0,0,2) == 0) goto IIfdE1;
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
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,7) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,7) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
  }
/* # line 225 "maillist.qsc" */	/* host code */
static STATUS
load_tblfld()
  {
  char mail_addr[81];
	IIUGmsg(ERget(F_RM001C_Selecting_data), FALSE, 0);
/* # line 375 "maillist.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select mail_username from dd_mail_alert order by 1");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,80,mail_addr);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 380 "maillist.sc" */	/* host code */
/* # line 238 "maillist.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fmail_addr,(short *)0,1,32,0,mail_addr);
        IItcoputio(fold_mail_addr,(short *)0,1,32,0,mail_addr);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 240 "maillist.qsc" */	/* host code */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, NULL) != OK)
	{
/* # line 392 "maillist.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 393 "maillist.sc" */	/* host code */
		FTbell();
		IIUGerr(E_RM0012_Error_selecting, UG_ERR_ERROR, 0);
		return (FAIL);
	}
/* # line 397 "maillist.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 398 "maillist.sc" */	/* host code */
	return (OK);
  }
