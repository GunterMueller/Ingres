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
** Name:	cddsadd.qsc - add a CDDS
**
** Description:
**	Defines
**		cdds_add	- add a CDDS
**
** History:
**	16-dec-96 (joea)
**		Created based on cddsadd.osq in replicator library.
**	14-nov-97 (joea)
**		Correct call to IIUGerr.
**	30-nov-98 (abbjo03)
**		Generalize the mode lookup functions. Use new generic mode
**		validation function. Add mode validation in Save menu, so that
**		the default modes will be visible when we return to the CDDS
**		Summary.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	16-oct-2001 (abbjo03)
**	    Change READONLY to const.
**/
GLOBALREF
  char RMdb_display[];
static bool	form_initd;
static const
  char *form_name = ERx("rmcdds_detail"), *fcdds_no = ERx("cdds_no"), *
  fcdds_name = ERx("cdds_name"), *fcoll_mode = ERx("collision_mode"), *
  fcoll_mode_alpha = ERx("collision_mode_alpha"), *ferr_mode = ERx("error_mode")
  , *ferr_mode_alpha = ERx("error_mode_alpha");
FUNC_EXTERN bool mode_valid(char *mode_name, const char *fint_mode,
	char *mode_alpha);
FUNC_EXTERN void mode_lookup(char *mode_name, const char *fint_mode,
	const char *falpha_mode);
/*{
** Name:	cdds_add	- add a CDDS
**
** Description:
**	Add CDDS Definition.
**
** Inputs:
**	none
**
** Outputs:
**	cdds_no		- CDDS number
**	cdds_name	- database name
**	collision_mode	- collision mode
**	coll_mode_alpha	- collision mode text
**	error_mode	- error mode
**	err_mode_alpha	- error mode text
**
** Returns:
**	0	- no changes were made
**	1	- changes were made and saved
*/
STATUS
cdds_add(
i2	*cdds_no,
char	*cdds_name,
i2	*collision_mode,
char	*coll_mode_alpha,
i2	*error_mode,
char	*err_mode_alpha)
# if 0
  char *cdds_name;
  char *coll_mode_alpha;
  char *err_mode_alpha;
# endif
  {
  i4 cdds_exist;
  i2 cdds_num;
  i2 coll_mode;
  i2 err_mode;
    i4 chng;
    char objname[FE_MAXNAME+1];
	STATUS	retval;
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
	*coll_mode_alpha = *err_mode_alpha = EOS;
/* # line 123 "cddsadd.qsc" */	/* display */
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
/* # line 127 "cddsadd.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 128 "cddsadd.qsc" */	/* host code */
		if (chng)
		{
/* # line 130 "cddsadd.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,30,2,&cdds_num,fcdds_no);
              } /* IIfsetio */
            }
/* # line 131 "cddsadd.qsc" */	/* host code */
			if (cdds_num < 0 || cdds_num > 32767)
			{
				IIUGerr(E_RM0020_Invalid_cdds_no, UG_ERR_ERROR,
					0);
/* # line 135 "cddsadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 136 "cddsadd.qsc" */	/* host code */
			}
			/* Check for existing cdds_no */
/* # line 149 "cddsadd.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_cdds where cdds_no=");
    IIputdomio((short *)0,1,30,2,&cdds_num);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cdds_exist);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 153 "cddsadd.sc" */	/* host code */
			if (cdds_exist)
			{
				i4	pcdds_no = cdds_num;
				IIUGerr(E_RM0021_Cdds_no_in_use, UG_ERR_ERROR,
					1, &pcdds_no);
/* # line 149 "cddsadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 150 "cddsadd.qsc" */	/* host code */
			}
		}
/* # line 152 "cddsadd.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 157 "cddsadd.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 158 "cddsadd.qsc" */	/* host code */
		if (chng)
		{
			/* Check for existing cdds_name */
/* # line 161 "cddsadd.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,0,cdds_name,fcdds_name);
              } /* IIfsetio */
            }
/* # line 162 "cddsadd.qsc" */	/* host code */
			STtrmwhite(cdds_name);
/* # line 191 "cddsadd.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_cdds where cdds_name=");
    IIputdomio((short *)0,1,32,0,cdds_name);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cdds_exist);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 195 "cddsadd.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 196 "cddsadd.sc" */	/* host code */
			if (cdds_exist)
			{
				IIUGerr(E_RM0022_Cdds_name_in_use,
					UG_ERR_ERROR, 0);
/* # line 172 "cddsadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 173 "cddsadd.qsc" */	/* host code */
			}
		}
/* # line 175 "cddsadd.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 180 "cddsadd.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 181 "cddsadd.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,0,coll_mode_alpha,fcoll_mode_alpha);
              } /* IIfsetio */
            }
/* # line 182 "cddsadd.qsc" */	/* host code */
		if (chng || *coll_mode_alpha == EOS)
		{
			if (!mode_valid(ERx("collision_mode"), fcoll_mode,
				coll_mode_alpha))
			{
				IIUGerr(E_RM0023_Invalid_coll_mode,
					UG_ERR_ERROR, 0);
/* # line 189 "cddsadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 190 "cddsadd.qsc" */	/* host code */
			}
/* # line 191 "cddsadd.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(fcoll_mode_alpha,(short *)0,1,32,0,coll_mode_alpha);
              } /* IIfsetio */
            }
/* # line 192 "cddsadd.qsc" */	/* host code */
		}
/* # line 193 "cddsadd.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 198 "cddsadd.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 199 "cddsadd.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,0,err_mode_alpha,ferr_mode_alpha);
              } /* IIfsetio */
            }
/* # line 200 "cddsadd.qsc" */	/* host code */
		if (chng || *err_mode_alpha == EOS)
		{
			if (!mode_valid(ERx("error_mode"), ferr_mode,
				err_mode_alpha))
			{
				IIUGerr(E_RM0024_Invalid_err_mode,
					UG_ERR_ERROR, 0);
/* # line 207 "cddsadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 208 "cddsadd.qsc" */	/* host code */
			}
/* # line 209 "cddsadd.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(ferr_mode_alpha,(short *)0,1,32,0,err_mode_alpha);
              } /* IIfsetio */
            }
/* # line 210 "cddsadd.qsc" */	/* host code */
		}
/* # line 211 "cddsadd.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 217 "cddsadd.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 218 "cddsadd.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,30,2,&cdds_num,fcdds_no);
                IIgetfldio((short *)0,1,32,0,cdds_name,fcdds_name);
                IIgetfldio((short *)0,1,30,2,&coll_mode,fcoll_mode);
                IIgetfldio((short *)0,1,32,0,coll_mode_alpha,fcoll_mode_alpha);
                IIgetfldio((short *)0,1,30,2,&err_mode,ferr_mode);
                IIgetfldio((short *)0,1,32,0,err_mode_alpha,ferr_mode_alpha);
              } /* IIfsetio */
            }
/* # line 222 "cddsadd.qsc" */	/* host code */
		if (*coll_mode_alpha == EOS)
		{
			if (!mode_valid(ERx("collision_mode"), fcoll_mode,
				coll_mode_alpha))
			{
				IIUGerr(E_RM0023_Invalid_coll_mode,
					UG_ERR_ERROR, 0);
/* # line 229 "cddsadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 230 "cddsadd.qsc" */	/* host code */
			}
/* # line 231 "cddsadd.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(fcoll_mode_alpha,(short *)0,1,32,0,coll_mode_alpha);
              } /* IIfsetio */
            }
/* # line 232 "cddsadd.qsc" */	/* host code */
		}
		if (*err_mode_alpha == EOS)
		{
			if (!mode_valid(ERx("error_mode"), ferr_mode,
				err_mode_alpha))
			{
				IIUGerr(E_RM0024_Invalid_err_mode,
					UG_ERR_ERROR, 0);
/* # line 240 "cddsadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 241 "cddsadd.qsc" */	/* host code */
			}
/* # line 242 "cddsadd.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(ferr_mode_alpha,(short *)0,1,32,0,err_mode_alpha);
              } /* IIfsetio */
            }
/* # line 243 "cddsadd.qsc" */	/* host code */
		}
		IIUGmsg(ERget(F_RM0009_Saving), FALSE, 0);
		retval = 0;
/* # line 361 "cddsadd.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cddsadd1",7225,22367);
      IIputdomio((short *)0,1,30,2,&cdds_num);
      IIputdomio((short *)0,1,32,0,cdds_name);
      IIputdomio((short *)0,1,30,2,&coll_mode);
      IIputdomio((short *)0,1,30,2,&err_mode);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cddsadd1",7225,22367);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_cdds(cdds_no, cdds_name, collision_mode, error_mode)va\
lues(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&cdds_num);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,cdds_name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&coll_mode);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,2,&err_mode);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"cddsadd1",7225,22367);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 365 "cddsadd.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
		{
/* # line 367 "cddsadd.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 368 "cddsadd.sc" */	/* host code */
			retval = -1;
		}
		else if (db_config_changed(0) != OK)
		{
/* # line 372 "cddsadd.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 373 "cddsadd.sc" */	/* host code */
			retval = -1;
		}
		if (retval)
		{
			FTbell();
			IIUGerr(E_RM0009_Error_saving, UG_ERR_ERROR, 0);
/* # line 266 "cddsadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 267 "cddsadd.qsc" */	/* host code */
		}
/* # line 385 "cddsadd.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 386 "cddsadd.sc" */	/* host code */
		if (RPdb_error_check(0, NULL) != OK)
		{
/* # line 388 "cddsadd.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 389 "cddsadd.sc" */	/* host code */
			IIUGerr(E_RM000A_Error_committing, UG_ERR_ERROR, 0);
/* # line 274 "cddsadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 275 "cddsadd.qsc" */	/* host code */
		}
		*cdds_no = cdds_num;
		*collision_mode = coll_mode;
		*error_mode = err_mode;
		retval = 1;
/* # line 281 "cddsadd.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 288 "cddsadd.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 289 "cddsadd.qsc" */	/* host code */
		if (chng && IIUFccConfirmChoice(CONF_GENERIC, NULL, NULL,
			NULL, NULL, S_RM000B_Clear_form, S_RM000C_Yes_clear,
			S_RM000D_No_clear, ERx(""), TRUE) != CONFCH_YES)
/* # line 292 "cddsadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 294 "cddsadd.qsc" */	/* clear */
            {
              IIclrflds();
            }
/* # line 295 "cddsadd.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 296 "cddsadd.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio((char *)"db",(short *)0,1,32,0,RMdb_display);
              } /* IIfsetio */
            }
/* # line 297 "cddsadd.qsc" */	/* resume */
            {
              IIresfld((char *)"cdds_no");
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 7) {
          {
/* # line 304 "cddsadd.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,objname,19,(char *)0,0);
              } /* IIiqset */
            }
/* # line 305 "cddsadd.qsc" */	/* host code */
		if (STequal(objname, fcoll_mode))
			mode_lookup(ERx("collision_mode"), fcoll_mode,
				fcoll_mode_alpha);
		else if (STequal(objname, ferr_mode))
			mode_lookup(ERx("error_mode"), ferr_mode,
				ferr_mode_alpha);
		else
			IIUFfieldHelp();
          }
        } else if (IIretval() == 8) {
          {
/* # line 319 "cddsadd.qsc" */	/* host code */
		FEhelp(ERx("rmcddsad.hlp"), ERget(F_RM0030_Add_cdds));
          }
        } else if (IIretval() == 9) {
          {
/* # line 326 "cddsadd.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 327 "cddsadd.qsc" */	/* host code */
		if (chng && IIUFccConfirmChoice(CONF_GENERIC, NULL, NULL,
			NULL, NULL, S_RM000E_End_no_save, S_RM000F_Yes_end,
			S_RM0010_No_end, ERx(""), TRUE) != CONFCH_YES)
/* # line 330 "cddsadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 332 "cddsadd.qsc" */	/* host code */
		retval = 0;
/* # line 333 "cddsadd.qsc" */	/* breakdisplay */
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
      if (IIFRafActFld(fcdds_no,0,1) == 0) goto IIfdE1;
      if (IIFRafActFld(fcdds_name,0,2) == 0) goto IIfdE1;
      if (IIFRafActFld(fcoll_mode,0,3) == 0) goto IIfdE1;
      if (IIFRafActFld(ferr_mode,0,4) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Save),ERget(F_FE0117_SaveExpl),2,2,5) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM000A_Clear),ERget(F_RM000B_Clear_expl),0,0,6) == 
      0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(16,(char *)0,0,0,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Lookup),ERget(F_FE0116_LookupExpl),0,0,7) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,7) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,8) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,8) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,9) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,9) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 336 "cddsadd.qsc" */	/* host code */
	return (retval);
  }
