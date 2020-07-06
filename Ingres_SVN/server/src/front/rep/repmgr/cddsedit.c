# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**Copyright (c) 2004 Ingres Corporation
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
# include <uf.h>
# include <ug.h>
# include <stdprmpt.h>
# include <rpu.h>
# include "errm.h"
/**
** Name:	cddsedit.qsc - edit a CDDS
**
** Description:
**	Defines
**		cdds_edit	- edit a CDDS
**
** History:
**	16-dec-96 (joea)
**		Created based on cddsedit.osq in replicator library.
**	14-nov-97 (joea)
**		Correct call to IIUGerr.
**	30-nov-98 (abbjo03)
**		Generalize the mode lookup functions. Add generic mode
**		validation function.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	16-oct-2001 (abbjo03)
**	    Change READONLY to const.
**	08-jul-2004 (abbjo03)
**	    Fix mode_lookup to use ESQL/EQUEL field name for falpha_mode.
**/
GLOBALREF
  char RMdb_display[];
static bool	form_initd;
static const
  char *form_name = ERx("rmcdds_detail"), *fcdds_no = ERx("cdds_no"), *
  fcdds_name = ERx("cdds_name"), *fcoll_mode = ERx("collision_mode"), *
  fcoll_mode_alpha = ERx("collision_mode_alpha"), *ferr_mode = ERx("error_mode")
  , *ferr_mode_alpha = ERx("error_mode_alpha");
static char *coll_mode_opt = ERx("collision_mode");
static char *err_mode_opt = ERx("error_mode");
bool mode_valid(char *mode_name, const char *fint_mode, char *mode_alpha);
void mode_lookup(char *mode_name, const char *fint_mode,
	const char *falpha_mode);
static bool pick_mode(char *list_rows, i2 *mode, char *mode_alpha);
/*{
** Name:	cdds_edit	- edit a CDDS
**
** Description:
**	Edit an existing Replicated Database definition
**
** Inputs:
**	cdds_no		- CDDS number
**	cdds_name	- CDDS name
**	collision_mode	- collision mode
**	coll_mode_alpha	- collision mode text
**	error_mode	- error mode
**	err_mode_alpha	- error mode text
**
** Outputs:
**	cdds_name	- CDDS name
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
cdds_edit(
i2	cdds_no,
char	*cdds_name,
i2	*collision_mode,
char	*coll_mode_alpha,
i2	*error_mode,
char	*err_mode_alpha)
# if 0
  i2 cdds_no;
  char *cdds_name;
  char *coll_mode_alpha;
  char *err_mode_alpha;
# endif
  {
  i4 cdds_exist;
  i2 coll_mode;
  i2 err_mode;
    i4 chng;
    char objname[FE_MAXNAME+1];
	STATUS	retval;
	coll_mode = *collision_mode;
	err_mode = *error_mode;
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
/* # line 132 "cddsedit.qsc" */	/* display */
    {
      if (IIdispfrm(form_name,(char *)"u") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio((char *)"db",(short *)0,1,32,0,RMdb_display);
      IIputfldio(fcdds_no,(short *)0,1,30,2,&cdds_no);
      IIputfldio(fcdds_name,(short *)0,1,32,0,cdds_name);
      IIputfldio(fcoll_mode,(short *)0,1,30,2,&coll_mode);
      IIputfldio(fcoll_mode_alpha,(short *)0,1,32,0,coll_mode_alpha);
      IIputfldio(ferr_mode,(short *)0,1,30,2,&err_mode);
      IIputfldio(ferr_mode_alpha,(short *)0,1,32,0,err_mode_alpha);
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 139 "cddsedit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 140 "cddsedit.qsc" */	/* host code */
		if (chng)
		{
			/* Check for existing cdds_name */
/* # line 143 "cddsedit.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,0,cdds_name,fcdds_name);
              } /* IIfsetio */
            }
/* # line 144 "cddsedit.qsc" */	/* host code */
			STtrmwhite(cdds_name);
/* # line 155 "cddsedit.sc" */	/* select */
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
/* # line 159 "cddsedit.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 160 "cddsedit.sc" */	/* host code */
			if (cdds_exist)
			{
				IIUGerr(E_RM0022_Cdds_name_in_use,
					UG_ERR_ERROR, 0);
/* # line 154 "cddsedit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 155 "cddsedit.qsc" */	/* host code */
			}
		}
/* # line 157 "cddsedit.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 162 "cddsedit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 163 "cddsedit.qsc" */	/* host code */
		if (chng)
		{
			if (!mode_valid(coll_mode_opt, fcoll_mode,
				coll_mode_alpha))
			{
				IIUGerr(E_RM0023_Invalid_coll_mode,
					UG_ERR_ERROR, 0);
/* # line 170 "cddsedit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 171 "cddsedit.qsc" */	/* host code */
			}
/* # line 172 "cddsedit.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(fcoll_mode_alpha,(short *)0,1,32,0,coll_mode_alpha);
              } /* IIfsetio */
            }
/* # line 173 "cddsedit.qsc" */	/* host code */
		}
/* # line 174 "cddsedit.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 179 "cddsedit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 180 "cddsedit.qsc" */	/* host code */
		if (chng)
		{
			if (!mode_valid(err_mode_opt, ferr_mode,
				err_mode_alpha))
			{
				IIUGerr(E_RM0024_Invalid_err_mode,
					UG_ERR_ERROR, 0);
/* # line 187 "cddsedit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 188 "cddsedit.qsc" */	/* host code */
			}
/* # line 189 "cddsedit.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(ferr_mode_alpha,(short *)0,1,32,0,err_mode_alpha);
              } /* IIfsetio */
            }
/* # line 190 "cddsedit.qsc" */	/* host code */
		}
/* # line 191 "cddsedit.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 197 "cddsedit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 198 "cddsedit.qsc" */	/* host code */
		if (!chng)
		{
			IIUGerr(E_RM0029_No_changes_to_save, 0, 0);
/* # line 201 "cddsedit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 202 "cddsedit.qsc" */	/* host code */
		}
/* # line 204 "cddsedit.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 205 "cddsedit.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,30,2,&cdds_no,fcdds_no);
                IIgetfldio((short *)0,1,32,0,cdds_name,fcdds_name);
                IIgetfldio((short *)0,1,30,2,&coll_mode,fcoll_mode);
                IIgetfldio((short *)0,1,32,0,coll_mode_alpha,fcoll_mode_alpha);
                IIgetfldio((short *)0,1,30,2,&err_mode,ferr_mode);
                IIgetfldio((short *)0,1,32,0,err_mode_alpha,ferr_mode_alpha);
              } /* IIfsetio */
            }
/* # line 210 "cddsedit.qsc" */	/* host code */
		IIUGmsg(ERget(F_RM0009_Saving), FALSE, 0);
		retval = 0;
/* # line 287 "cddsedit.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cddsedit1",7224,19707);
      IIputdomio((short *)0,1,32,0,cdds_name);
      IIputdomio((short *)0,1,30,2,&coll_mode);
      IIputdomio((short *)0,1,30,2,&err_mode);
      IIputdomio((short *)0,1,30,2,&cdds_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cddsedit1",7224,19707);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_cdds set cdds_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,cdds_name);
        IIwritio(0,(short *)0,1,32,0,(char *)", collision_mode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,2,&coll_mode);
        IIwritio(0,(short *)0,1,32,0,(char *)", error_mode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&err_mode);
        IIwritio(0,(short *)0,1,32,0,(char *)"where cdds_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,2,&cdds_no);
        IIexDefine(0,(char *)"cddsedit1",7224,19707);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 292 "cddsedit.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
		{
/* # line 294 "cddsedit.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 295 "cddsedit.sc" */	/* host code */
			retval = -1;
		}
		else if (db_config_changed(0) != OK)
		{
/* # line 299 "cddsedit.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 300 "cddsedit.sc" */	/* host code */
			retval = -1;
		}
		if (retval)
		{
			FTbell();
			IIUGerr(E_RM0009_Error_saving, UG_ERR_ERROR, 0);
/* # line 232 "cddsedit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 233 "cddsedit.qsc" */	/* host code */
		}
/* # line 312 "cddsedit.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 313 "cddsedit.sc" */	/* host code */
		if (RPdb_error_check(0, NULL) != OK)
		{
/* # line 315 "cddsedit.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 316 "cddsedit.sc" */	/* host code */
			IIUGerr(E_RM000A_Error_committing, UG_ERR_ERROR, 0);
/* # line 240 "cddsedit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 241 "cddsedit.qsc" */	/* host code */
		}
		*collision_mode = coll_mode;
		*error_mode = err_mode;
		retval = 1;
/* # line 246 "cddsedit.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 253 "cddsedit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,objname,19,(char *)0,0);
              } /* IIiqset */
            }
/* # line 254 "cddsedit.qsc" */	/* host code */
		if (STequal(objname, fcoll_mode))
			mode_lookup(coll_mode_opt, fcoll_mode,
				fcoll_mode_alpha);
		else if (STequal(objname, ferr_mode))
			mode_lookup(err_mode_opt, ferr_mode, ferr_mode_alpha);
		else
			IIUFfieldHelp();
          }
        } else if (IIretval() == 6) {
          {
/* # line 267 "cddsedit.qsc" */	/* host code */
		FEhelp(ERx("rmcddsed.hlp"), ERget(F_RM0031_Edit_cdds));
          }
        } else if (IIretval() == 7) {
          {
/* # line 274 "cddsedit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 275 "cddsedit.qsc" */	/* host code */
		if (chng && IIUFccConfirmChoice(CONF_GENERIC, NULL, NULL,
			NULL, NULL, S_RM000E_End_no_save, S_RM000F_Yes_end,
			S_RM0010_No_end, ERx(""), TRUE) != CONFCH_YES)
/* # line 278 "cddsedit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 280 "cddsedit.qsc" */	/* host code */
		retval = 0;
/* # line 281 "cddsedit.qsc" */	/* breakdisplay */
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
      if (IIFRafActFld(fcdds_name,0,1) == 0) goto IIfdE1;
      if (IIFRafActFld(fcoll_mode,0,2) == 0) goto IIfdE1;
      if (IIFRafActFld(ferr_mode,0,3) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Save),ERget(F_FE0117_SaveExpl),2,2,4) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Lookup),ERget(F_FE0116_LookupExpl),0,0,5) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,6) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,7) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,7) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 284 "cddsedit.qsc" */	/* host code */
	return (retval);
  }
bool
mode_valid(
char		*mode_name,
const char	*fint_mode,
char		*mode_alpha)
# if 0
  char *mode_name;
  char *fint_mode;
  char *mode_alpha;
# endif
{
  i2 mode_int;
	DBEC_ERR_INFO	errinfo;
/* # line 306 "cddsedit.qsc" */	/* getform */
  {
    if (IIfsetio(form_name) != 0) {
      IIgetfldio((short *)0,1,30,2,&mode_int,fint_mode);
    } /* IIfsetio */
  }
/* # line 307 "cddsedit.qsc" */	/* host code */
/* # line 436 "cddsedit.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select alpha_value from dd_option_values where option_name=");
    IIputdomio((short *)0,1,32,0,mode_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and numeric_value=");
    IIputdomio((short *)0,1,30,2,&mode_int);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,0,mode_alpha);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 441 "cddsedit.sc" */	/* host code */
	RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo);
/* # line 442 "cddsedit.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 443 "cddsedit.sc" */	/* host code */
	if (errinfo.errorno)
	{
/* # line 316 "cddsedit.qsc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage((char *)"Error validating collision mode ...");
  }
/* # line 318 "cddsedit.qsc" */	/* host code */
		return (FALSE);
	}
	else if (errinfo.rowcount == 0)
	{
		return (FALSE);
	}
	return (TRUE);
}
void
mode_lookup(
char		*mode_name,
const char	*fint_mode,
const char	*falpha_mode)
# if 0
  char *mode_name;
  char *falpha_mode;
# endif
  {
  i2 mode_int;
  char mode_alpha[21];
	char	list_row[36];
	char	list_rows[2048];
	*list_rows = EOS;
/* # line 480 "cddsedit.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select INT2(numeric_value), alpha_value from dd_option_values where o\
ption_name=");
    IIputdomio((short *)0,1,32,0,mode_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"order by 1");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&mode_int);
      IIgetdomio((short *)0,1,32,0,mode_alpha);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 486 "cddsedit.sc" */	/* host code */
		STtrmwhite(mode_alpha);
		STprintf(list_row, ERx("%d;%s\n"), (i4)mode_int, mode_alpha);
		STcat(list_rows, list_row);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 490 "cddsedit.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 491 "cddsedit.sc" */	/* host code */
	if (pick_mode(list_rows, &mode_int, mode_alpha))
	{
/* # line 361 "cddsedit.qsc" */	/* putform */
    {
      if (IIfsetio(form_name) != 0) {
        IIputfldio(fint_mode,(short *)0,1,30,2,&mode_int);
        IIputfldio(falpha_mode,(short *)0,1,32,0,mode_alpha);
      } /* IIfsetio */
    }
/* # line 363 "cddsedit.qsc" */	/* set_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 364 "cddsedit.qsc" */	/* host code */
	}
  }
static bool
pick_mode(
char	*list_rows,
i2	*mode,
char	*mode_alpha)
{
	i4	i;
	i4	row;
	char	*p1, *p2, *pend;
	row = IIFDlpListPick(NULL, list_rows, 5, -1, -1, NULL, NULL, NULL,
		NULL);
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
		STcopy(pend, mode_alpha);
		*p2 = EOS;
		CVan(p1, &i);
		*mode = (i2)i;
		return (TRUE);
	}
	return (FALSE);
}
