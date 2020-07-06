# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <st.h>
# include <nm.h>
# include <lo.h>
# include <er.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <erfe.h>
# include <uf.h>
# include <ug.h>
# include "errm.h"
/**
** Name:	reports.qc - reports menu
**
** Description:
**	Defines
**		reports		- reports menu
**
** History:
**	16-dec-96 (joea)
**		Created based on reports.osq in replicator library.
**	14-jan-97 (joea)
**		Change in fast message numbers to close gap.
**/
GLOBALREF i2	RMlocal_db_no;
GLOBALREF
  char RMdb_display[];
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmreport_menu"), *tf_name = ERx("iitf"), *fcmd = ERx("cmd")
  , *fexpl = ERx("explanation");
/*{
** Name:	reports	- reports menu
**
** Description:
**	Resolver and Other Reports
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Returns:
**	OK	- normal return
**	1	- exit from RepMgr
*/
STATUS
reports()
  {
	STATUS	err;
    char msg[1024];
    char choice[81];
	STATUS	retval = OK;
	char	*p;
	char	file_name[MAX_LOC+1];
	LOCATION	loc;
	if (!form_initd)
	{
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), form_name) != OK)
		{
			IIUGerr(E_RM0006_Get_form_failed, UG_ERR_ERROR, 1,
				form_name);
			return (retval);
		}
		form_initd = TRUE;
	}
/* # line 82 "reports.qc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 83 "reports.qc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fcmd,(short *)0,1,32,0,ERget(F_RM0097_Collision));
        IItcoputio(fexpl,(short *)0,1,32,0,ERget(F_RM0098_Collision_expl));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 85 "reports.qc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fcmd,(short *)0,1,32,0,ERget(F_RM0099_Integrity));
        IItcoputio(fexpl,(short *)0,1,32,0,ERget(F_RM009A_Integrity_expl));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 87 "reports.qc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fcmd,(short *)0,1,32,0,ERget(F_RM009B_DistribConfig));
        IItcoputio(fexpl,(short *)0,1,32,0,ERget(F_RM009C_DistribConfig_expl));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 89 "reports.qc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fcmd,(short *)0,1,32,0,ERget(F_RM000D_Databases));
        IItcoputio(fexpl,(short *)0,1,32,0,ERget(F_RM009D_Databases_rpt_expl));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 91 "reports.qc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fcmd,(short *)0,1,32,0,ERget(F_RM000F_CDDS));
        IItcoputio(fexpl,(short *)0,1,32,0,ERget(F_RM009E_Cdds_rpt_expl));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 93 "reports.qc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fcmd,(short *)0,1,32,0,ERget(F_RM0011_Tables));
        IItcoputio(fexpl,(short *)0,1,32,0,ERget(F_RM009F_Tables_rpt_expl));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 95 "reports.qc" */	/* scroll */
    {
      if (IItbsetio(1,form_name,tf_name,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,1) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 97 "reports.qc" */	/* display */
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
/* # line 102 "reports.qc" */	/* host code */
		NMloc(TEMP, PATH, NULL, &loc);
/* # line 104 "reports.qc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,32,80,choice,fcmd);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 105 "reports.qc" */	/* host code */
		if (STequal(choice, ERget(F_RM0097_Collision)))
		{
			LOfstfile(ERx("rpcollsn.rpt"), &loc);
			LOtos(&loc, &p);
			STcopy(p, file_name);
			IIUGmsg(ERget(F_RM00A1_Compar_distrib_queue), FALSE,
				0);
			if (queue_collision(RMlocal_db_no, file_name) != OK)
			{
				IIUGerr(E_RM0069_Error_collision_rpt,
					UG_ERR_ERROR, 0);
/* # line 116 "reports.qc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 117 "reports.qc" */	/* host code */
			}
		}
		else if (STequal(choice, ERget(F_RM0099_Integrity)))
		{
			LOfstfile(ERx("rptblint.rpt"), &loc);
			LOtos(&loc, &p);
			STcopy(p, file_name);
			err = tbl_integ(file_name);
			if (err == 1)		/* end/cancel */
			{
/* # line 127 "reports.qc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 128 "reports.qc" */	/* host code */
			}
			else if (err)		/* error */
			{
				IIUGerr(E_RM006A_Error_tbl_integ_rpt,
					UG_ERR_ERROR, 0);
/* # line 133 "reports.qc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 134 "reports.qc" */	/* host code */
			}
		}
		else if (STequal(choice, ERget(F_RM009B_DistribConfig)))
		{
			LOfstfile(ERx("rpckdcfg.rpt"), &loc);
			LOtos(&loc, &p);
			STcopy(p, file_name);
			if (check_distrib_config(file_name) != OK)
			{
				IIUGerr(E_RM006B_Error_cfg_check_rpt,
					UG_ERR_ERROR, 0);
/* # line 145 "reports.qc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 146 "reports.qc" */	/* host code */
			}
		}
		else if (STequal(choice, ERget(F_RM000D_Databases)))
		{
			LOfstfile(ERx("rpdbs.rpt"), &loc);
			LOtos(&loc, &p);
			STcopy(p, file_name);
			if (databases_report(file_name) != OK)
			{
				IIUGerr(E_RM006C_Error_dbs_rpt, UG_ERR_ERROR,
					0);
/* # line 157 "reports.qc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 158 "reports.qc" */	/* host code */
			}
		}
		else if (STequal(choice, ERget(F_RM000F_CDDS)))
		{
			LOfstfile(ERx("rppaths.rpt"), &loc);
			LOtos(&loc, &p);
			STcopy(p, file_name);
			if (paths_report(file_name) != OK)
			{
				IIUGerr(E_RM006D_Error_paths_rpt, UG_ERR_ERROR,
					0);
/* # line 169 "reports.qc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 170 "reports.qc" */	/* host code */
			}
		}
		else if (STequal(choice, ERget(F_RM0011_Tables)))
		{
			LOfstfile(ERx("rptables.rpt"), &loc);
			LOtos(&loc, &p);
			STcopy(p, file_name);
			if (tables_report(file_name) != OK)
			{
				IIUGerr(E_RM006E_Error_tables_rpt,
					UG_ERR_ERROR, 0);
/* # line 181 "reports.qc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 182 "reports.qc" */	/* host code */
			}
		}
		file_display(file_name);
          }
        } else if (IIretval() == 2) {
          {
/* # line 189 "reports.qc" */	/* host code */
		FEtabfnd(form_name, tf_name);
          }
        } else if (IIretval() == 3) {
          {
/* # line 194 "reports.qc" */	/* scroll */
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
/* # line 199 "reports.qc" */	/* scroll */
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
/* # line 205 "reports.qc" */	/* host code */
		FEhelp(ERx("rmreport.hlp"),
			ERget(F_RM00A0_Resolver_other_rpts));
          }
        } else if (IIretval() == 6) {
          {
/* # line 212 "reports.qc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 7) {
          {
/* # line 218 "reports.qc" */	/* host code */
		retval = 1;
/* # line 219 "reports.qc" */	/* breakdisplay */
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
/* # line 222 "reports.qc" */	/* host code */
	return (retval);
  }
