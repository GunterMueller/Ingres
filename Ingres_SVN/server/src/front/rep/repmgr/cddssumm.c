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
** Name:	cddssumm.qsc - CDDS summary
**
** Description:
**	Defines
**		cdds_summ	- CDDS summary
**
** History:
**	05-dec-96 (joea)
**		Created based on cddscat.osq in replicator library.
**	12-may-98 (padni01/joea) bug 88842
**		When inserting a new row, scroll display window to the position
**		where the new row will be added. Correct the code for finding
**		the position where the new row will be added.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
GLOBALREF
  char RMdb_display[];
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmcdds_summary"), *tf_name = ERx("iitf"), *fcdds_no = ERx("cdds_no")
  , *fcdds_name = ERx("cdds_name"), *fcoll_mode = ERx("collision_mode"), *
  fcoll_mode_alpha = ERx("collision_mode_alpha"), *ferr_mode = ERx("error_mode")
  , *ferr_mode_alpha = ERx("error_mode_alpha");
FUNC_EXTERN STATUS cdds_add(i2 *cdds_no, char *cdds_name,
	i2 *collision_mode, char *collision_mode_alpha, i2 *error_mode,
	char *error_mode_alpha);
FUNC_EXTERN STATUS cdds_delete(i2 cdds_no, i4  RegTableCount);
FUNC_EXTERN STATUS cdds_edit(i2 cdds_no, char *cdds_name,
	i2 *collision_mode, char *collision_mode_alpha, i2 *error_mode,
	char *error_mode_alpha);
FUNC_EXTERN void cdds_databases(i2 cdds_no, char *cdds_name);
FUNC_EXTERN void cdds_path(i2 cdds_no, char *cdds_name);
FUNC_EXTERN void cdds_regist_tables(i2 cdds_no);
/*{
** Name:	cdds_summ	- CDDS summary
**
** Description:
**	Summary of Consistent Distributed Data Set Information
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
cdds_summ()
  {
  i2 cdds_no;
  char cdds_name[DB_MAXNAME+1];
  i2 coll_mode;
  char coll_mode_alpha[21];
  i2 err_mode;
  char err_mode_alpha[21];
  i4 cnt_tables;
    i2 curr_cdds_no;
    i4 rownumber;
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
/* # line 108 "cddssumm.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"r") != 0) {
        IIthidecol(fcoll_mode,(char *)"i2 not null");
        IIthidecol(ferr_mode,(char *)"i2 not null");
        IItfill();
      } /* IItbinit */
    }
/* # line 110 "cddssumm.qsc" */	/* host code */
	IIUGmsg(ERget(F_RM001C_Selecting_data), FALSE, 0);
/* # line 105 "cddssumm.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select cdds_no, cdds_name, collision_mode, o1.alpha_value, error_mode\
, o2.alpha_value from dd_cdds, dd_option_values o1, dd_option_values o\
2 where o1.option_name='collision_mode' and collision_mode=o1.numeric_\
value and o2.option_name='error_mode' and ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"error_mode=o2.numeric_value order by cdds_no");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&cdds_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,cdds_name);
      IIgetdomio((short *)0,1,30,2,&coll_mode);
      IIgetdomio((short *)0,1,32,20,coll_mode_alpha);
      IIgetdomio((short *)0,1,30,2,&err_mode);
      IIgetdomio((short *)0,1,32,20,err_mode_alpha);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 116 "cddssumm.sc" */	/* host code */
		STtrmwhite(cdds_name);
/* # line 123 "cddssumm.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fcdds_no,(short *)0,1,30,2,&cdds_no);
        IItcoputio(fcdds_name,(short *)0,1,32,0,cdds_name);
        IItcoputio(fcoll_mode,(short *)0,1,30,2,&coll_mode);
        IItcoputio(fcoll_mode_alpha,(short *)0,1,32,0,coll_mode_alpha);
        IItcoputio(ferr_mode,(short *)0,1,30,2,&err_mode);
        IItcoputio(ferr_mode_alpha,(short *)0,1,32,0,err_mode_alpha);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 127 "cddssumm.qsc" */	/* host code */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	RPdb_error_check(0, &errinfo);
	if (errinfo.errorno)
	{
/* # line 134 "cddssumm.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 135 "cddssumm.sc" */	/* host code */
		FTbell();
		IIUGerr(E_RM0012_Error_selecting, UG_ERR_ERROR, 0);
		return;
	}
	else if (errinfo.rowcount == 0)
	{
/* # line 141 "cddssumm.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 142 "cddssumm.sc" */	/* host code */
		IIUGerr(E_RM0013_No_data_found, UG_ERR_ERROR, 0);
		return;
	}
/* # line 145 "cddssumm.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 146 "cddssumm.sc" */	/* host code */
/* # line 144 "cddssumm.qsc" */	/* display */
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
/* # line 149 "cddssumm.qsc" */	/* host code */
		if (cdds_add(&cdds_no, cdds_name, &coll_mode,
			coll_mode_alpha, &err_mode, err_mode_alpha) == 1)
		{
/* # line 152 "cddssumm.qsc" */	/* unloadtable */
            {
              if (IItbact(form_name,tf_name,0) == 0) goto IItbE1;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,30,2,&curr_cdds_no,fcdds_no);
                IItcogetio((short *)0,1,30,4,&rownumber,(char *)"_RECO\
RD");
                IITBceColEnd();
                {
/* # line 155 "cddssumm.qsc" */	/* host code */
				if (cdds_no < curr_cdds_no)
				{
					--rownumber;
/* # line 158 "cddssumm.qsc" */	/* endloop */
                  {
                    if (1) goto IItbE1;
                  }
/* # line 159 "cddssumm.qsc" */	/* host code */
				}
				else if (rownumber < 0)
				{
/* # line 162 "cddssumm.qsc" */	/* endloop */
                  {
                    if (1) goto IItbE1;
                  }
/* # line 163 "cddssumm.qsc" */	/* host code */
				}
                }
              } /* IItunload */
IItbE1:
              IItunend();
            }
/* # line 166 "cddssumm.qsc" */	/* host code */
			if (rownumber < 0)
/* # line 167 "cddssumm.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&rownumber,32,(char *)0,0);
              } /* IIiqset */
            }
/* # line 170 "cddssumm.qsc" */	/* host code */
			if (rownumber < 1)
			{
/* # line 172 "cddssumm.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 173 "cddssumm.qsc" */	/* insertrow */
            {
              if (IItbsetio(5,form_name,tf_name,0) != 0) {
                if (IItinsert() != 0) {
                  IItcoputio(fcdds_no,(short *)0,1,30,2,&cdds_no);
                  IItcoputio(fcdds_name,(short *)0,1,32,0,cdds_name);
                  IItcoputio(fcoll_mode,(short *)0,1,30,2,&coll_mode);
                  IItcoputio(fcoll_mode_alpha,(short *)0,1,32,0,
                  coll_mode_alpha);
                  IItcoputio(ferr_mode,(short *)0,1,30,2,&err_mode);
                  IItcoputio(ferr_mode_alpha,(short *)0,1,32,0,err_mode_alpha);
                  IITBceColEnd();
                } /* IItinsert */
              } /* IItbsetio */
            }
/* # line 180 "cddssumm.qsc" */	/* host code */
			}
			else
			{
/* # line 183 "cddssumm.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,rownumber) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 184 "cddssumm.qsc" */	/* insertrow */
            {
              if (IItbsetio(5,form_name,tf_name,-2) != 0) {
                if (IItinsert() != 0) {
                  IItcoputio(fcdds_no,(short *)0,1,30,2,&cdds_no);
                  IItcoputio(fcdds_name,(short *)0,1,32,0,cdds_name);
                  IItcoputio(fcoll_mode,(short *)0,1,30,2,&coll_mode);
                  IItcoputio(fcoll_mode_alpha,(short *)0,1,32,0,
                  coll_mode_alpha);
                  IItcoputio(ferr_mode,(short *)0,1,30,2,&err_mode);
                  IItcoputio(ferr_mode_alpha,(short *)0,1,32,0,err_mode_alpha);
                  IITBceColEnd();
                } /* IItinsert */
              } /* IItbsetio */
            }
/* # line 190 "cddssumm.qsc" */	/* host code */
			}
		}
          }
        } else if (IIretval() == 2) {
          {
/* # line 197 "cddssumm.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,2,&cdds_no,fcdds_no);
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,cdds_name,fcdds_name);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 199 "cddssumm.qsc" */	/* host code */
		if (cdds_no == 0)
		{
			IIUGerr(E_RM001B_Cant_del_default_cdds, UG_ERR_ERROR,
				0);
/* # line 203 "cddssumm.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 204 "cddssumm.qsc" */	/* host code */
		}
		/* Be sure there are no registered tables using this cdds */
/* # line 280 "cddssumm.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_regist_tables where cdds_no=");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt_tables);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 284 "cddssumm.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
		{
/* # line 286 "cddssumm.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 287 "cddssumm.sc" */	/* host code */
			IIUGerr(E_RM001C_Error_check_cdds_tbls, UG_ERR_ERROR,
				0);
/* # line 216 "cddssumm.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 217 "cddssumm.qsc" */	/* host code */
		}
/* # line 295 "cddssumm.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 296 "cddssumm.sc" */	/* host code */
		if (cnt_tables && IIUFccConfirmChoice(CONF_GENERIC, NULL, NULL,
			NULL, NULL, S_RM001D_Cdds_has_tables,
			S_RM001E_Yes_reassign, S_RM001F_No_reassign, ERx(""),
			TRUE) != CONFCH_YES)
/* # line 223 "cddssumm.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 225 "cddssumm.qsc" */	/* host code */
		if (IIUFccConfirmChoice(CONF_DELETE, cdds_name,
			ERget(F_RM000F_CDDS), NULL, NULL) != CONFCH_YES)
/* # line 227 "cddssumm.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 229 "cddssumm.qsc" */	/* host code */
		IIUGmsg(ERget(F_RM0022_Deleting), FALSE, 0);
		if (cdds_delete(cdds_no, cnt_tables) != OK)
		{
			IIUGerr(E_RM0015_Error_deleting, UG_ERR_ERROR, 0);
/* # line 233 "cddssumm.qsc" */	/* resume */
            {
              IIresfld(tf_name);
              if (1) goto IIfdB1;
            }
/* # line 234 "cddssumm.qsc" */	/* host code */
		}
/* # line 236 "cddssumm.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,form_name,tf_name,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 242 "cddssumm.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,2,&cdds_no,fcdds_no);
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,cdds_name,fcdds_name);
                IItcogetio((short *)0,1,30,2,&coll_mode,fcoll_mode);
                IItcogetio((short *)0,1,32,20,coll_mode_alpha,fcoll_mode_alpha);
                IItcogetio((short *)0,1,30,2,&err_mode,ferr_mode);
                IItcogetio((short *)0,1,32,20,err_mode_alpha,ferr_mode_alpha);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 247 "cddssumm.qsc" */	/* host code */
		if (cdds_edit(cdds_no, cdds_name, &coll_mode, coll_mode_alpha,
			&err_mode, err_mode_alpha) == 1)
		{
/* # line 250 "cddssumm.qsc" */	/* putrow */
            {
              if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                IItcoputio(fcdds_name,(short *)0,1,32,0,cdds_name);
                IItcoputio(fcoll_mode,(short *)0,1,30,2,&coll_mode);
                IItcoputio(fcoll_mode_alpha,(short *)0,1,32,0,coll_mode_alpha);
                IItcoputio(ferr_mode,(short *)0,1,30,2,&err_mode);
                IItcoputio(ferr_mode_alpha,(short *)0,1,32,0,err_mode_alpha);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 255 "cddssumm.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 4) {
          {
/* # line 261 "cddssumm.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,2,&cdds_no,fcdds_no);
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,cdds_name,fcdds_name);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 263 "cddssumm.qsc" */	/* host code */
		cdds_databases(cdds_no, cdds_name);
          }
        } else if (IIretval() == 5) {
          {
/* # line 269 "cddssumm.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,2,&cdds_no,fcdds_no);
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,cdds_name,fcdds_name);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 271 "cddssumm.qsc" */	/* host code */
		cdds_path(cdds_no, cdds_name);
          }
        } else if (IIretval() == 6) {
          {
/* # line 277 "cddssumm.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,2,&cdds_no,fcdds_no);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 278 "cddssumm.qsc" */	/* host code */
		cdds_regist_tables(cdds_no);
          }
        } else if (IIretval() == 7) {
          {
/* # line 283 "cddssumm.qsc" */	/* host code */
		FEtabfnd(form_name, tf_name);
          }
        } else if (IIretval() == 8) {
          {
/* # line 288 "cddssumm.qsc" */	/* scroll */
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
/* # line 293 "cddssumm.qsc" */	/* scroll */
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
/* # line 299 "cddssumm.qsc" */	/* host code */
		FEhelp(ERx("rmcddsum.hlp"), ERget(F_RM002F_Cdds_summary));
          }
        } else if (IIretval() == 11) {
          {
/* # line 305 "cddssumm.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(F_RM001D_Add),ERget(F_RM0028_Add_cdds_expl),2,2,1) ==
       0) goto IIfdE1;
      if (IInmuact(ERget(F_RM001F_Delete),ERget(F_RM0029_Delete_cdds_expl),2,2
      ,2) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(13,(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM0023_Edit),ERget(F_RM002A_Edit_cdds_expl),2,2,3) 
      == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM000D_Databases),ERget(F_RM002B_Databases_expl),2,
      2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM002C_Paths),ERget(F_RM002D_Paths_expl),2,2,5) == 
      0) goto IIfdE1;
      if (IInmuact(ERget(F_RM0011_Tables),ERget(F_RM002E_Tables_expl),2,2,6) 
      == 0) goto IIfdE1;
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
