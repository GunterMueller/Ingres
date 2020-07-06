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
# include "errm.h"
/**
** Name:	cfgmenu.qc - configuration menu
**
** Description:
**	Defines
**		cfg_menu	- configuration menu
**
** History:
**	16-dec-96 (joea)
**		Created based on cfgmenu.osq in replicator library.
**	11-dec-97 (joea)
**		Remove BuildServer menuitem.
**/
GLOBALREF
  char RMdb_display[];
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmconfig_menu"), *tf_name = ERx("iitf"), *fcmd = ERx("cmd")
  , *fexpl = ERx("explanation");
/*{
** Name:	cfg_menu	- configuration menu
**
** Description:
**	Configure Replicator
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Returns:
**	0	- normal return
**	1	- exit from RepMgr
*/
STATUS
cfg_menu()
  {
    char menu_choice[81];
	STATUS	retval = 0;
	if (!form_initd)
	{
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), form_name) != OK)
		{
			IIUGerr(E_RM0006_Get_form_failed, UG_ERR_ERROR, 1,
				form_name);
			return (FAIL);
		}
		form_initd = TRUE;
	}
/* # line 73 "cfgmenu.qc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 75 "cfgmenu.qc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fcmd,(short *)0,1,32,0,ERget(F_RM000D_Databases));
        IItcoputio(fexpl,(short *)0,1,32,0,ERget(F_RM000E_Databases_expl));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 77 "cfgmenu.qc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fcmd,(short *)0,1,32,0,ERget(F_RM000F_CDDS));
        IItcoputio(fexpl,(short *)0,1,32,0,ERget(F_RM0010_CDDS_expl));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 79 "cfgmenu.qc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fcmd,(short *)0,1,32,0,ERget(F_RM0011_Tables));
        IItcoputio(fexpl,(short *)0,1,32,0,ERget(F_RM0012_Tables_expl));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 81 "cfgmenu.qc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fcmd,(short *)0,1,32,0,ERget(F_RM0015_MoveConfig));
        IItcoputio(fexpl,(short *)0,1,32,0,ERget(F_RM0016_MoveConfig_expl));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 83 "cfgmenu.qc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fcmd,(short *)0,1,32,0,ERget(F_RM0017_Activate));
        IItcoputio(fexpl,(short *)0,1,32,0,ERget(F_RM0018_Activate_expl));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 85 "cfgmenu.qc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fcmd,(short *)0,1,32,0,ERget(F_RM0019_MailList));
        IItcoputio(fexpl,(short *)0,1,32,0,ERget(F_RM001A_MailList_expl));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 87 "cfgmenu.qc" */	/* scroll */
    {
      if (IItbsetio(1,form_name,tf_name,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,1) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 89 "cfgmenu.qc" */	/* display */
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
/* # line 94 "cfgmenu.qc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,32,80,menu_choice,fcmd);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 95 "cfgmenu.qc" */	/* host code */
		if (STequal(menu_choice, ERget(F_RM000D_Databases)))
			db_summ();
		else if (STequal(menu_choice, ERget(F_RM000F_CDDS)))
			cdds_summ();
		else if (STequal(menu_choice, ERget(F_RM0011_Tables)))
			tbl_summ();
		else if (STequal(menu_choice, ERget(F_RM0015_MoveConfig)))
			move_config();
		else if (STequal(menu_choice, ERget(F_RM0017_Activate)))
			chgrec_activate();
		else if (STequal(menu_choice, ERget(F_RM0019_MailList)))
			mail_list();
          }
        } else if (IIretval() == 2) {
          {
/* # line 111 "cfgmenu.qc" */	/* host code */
		FEtabfnd(form_name, tf_name);
          }
        } else if (IIretval() == 3) {
          {
/* # line 116 "cfgmenu.qc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 121 "cfgmenu.qc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 127 "cfgmenu.qc" */	/* host code */
		FEhelp(ERx("rmcfgmen.hlp"), ERget(F_RM001B_Config_Rep));
          }
        } else if (IIretval() == 6) {
          {
/* # line 133 "cfgmenu.qc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 7) {
          {
/* # line 139 "cfgmenu.qc" */	/* host code */
		retval = 1;
/* # line 140 "cfgmenu.qc" */	/* breakdisplay */
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
      if (IInmuact(ERget(FE_Select),ERget(F_FE0108_ExplSelect),2,2,1) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,2) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),2,2,5) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,6) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Quit),ERget(F_RM0008_Quit_expl),2,2,7) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,2,2,7) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 143 "cfgmenu.qc" */	/* host code */
	return (retval);
  }
