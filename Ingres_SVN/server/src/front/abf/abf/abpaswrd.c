# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<st.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include       <stdprmpt.h>
# include	"erab.h"
/**
** Name:	abpaswrd.qsc -- Get password for a role
**
** Description:
**	This file defines:
**
**	IIABgrpGetRolePwd	-	Get password for a role
**
** History:
**	8/90 (Mike S) Initial version
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/* # define's */
/* GLOBALDEF's */
/* extern's */
FUNC_EXTERN char *IIUGfmt();
/* static's */
static READONLY
  char _form[]= ERx("abf_role_password"), _title[]= ERx("title"), _password[]= ERx("password")
  ;
static	bool init_done = FALSE;
/*{
** Name:	IIABgrpGetRolePwd       -       Get password for a role
**
** Description:
**	Put up a popup to get the password for a role.  We will force the
**	user to enter it twice, since it isn't echoed.
**
**	There is, unfortunately, no way to verify the password's correctness.
**	When there is, we should do it here.
**
** Inputs:
**	roleid		char *		Role whose password we're getting
**	prev_pwd	char *		Previous password (NULL if none)
**
** Outputs:
**	password	char *		Role password 
**
**	Returns:
**			OK	Password was entered and verified
**			FAIL	User Cancelled
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	8/90 (Mike S) Initial Version
*/
STATUS
IIABgrpGetRolePwd(roleid, prev_pwd, password)
char	*roleid;
char	*prev_pwd;
  char *password;
{
  char pbuf[FE_MAXNAME+1];
  char e_title[80];
  char v_title[80];
	i4 pass = 1;
	STATUS	status;
	if (roleid == NULL || *roleid == EOS)
		return FAIL;		/* Shouldn't happen */
	if (prev_pwd != NULL)
	{
		STATUS conf_status;
		/* See if previous password is OK */
		conf_status = IIUFccConfirmChoice(CONF_GENERIC,
				roleid, ERx(""),
				ERget(F_AB02A3_PrevPwdHelp), 
				ERx("abprvpwd.hlp"),
				F_AB02A4_PrevPwdTitle,
				F_AB02A5_PrevPwdYes,
				F_AB02A6_PrevPwdNo,
				ERx(""), TRUE);
	      	switch (conf_status)
	      	{
		    case CONFCH_YES:
			/* Use previous password */
			STcopy(prev_pwd, password);
			return OK;
		    case CONFCH_CANCEL:
			/* Don't enter a password at all */
			return FAIL;
		}
		/* If we got this far, user wants to enter a new password */
	}
	/* Get password from user */
	if (!init_done)
	{
		IIARiaForm(_form);
		init_done = TRUE;
	}
/* # line 124 "abpaswrd.qsc" */	/* display */
  {
    if (IIdispfrm(_form,(char *)"f") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 127 "abpaswrd.qsc" */	/* host code */
		_VOID_ IIUGfmt(e_title, sizeof(e_title), 
			ERget(F_AB02A0_EnterPassword), 1, roleid);
		_VOID_ IIUGfmt(v_title, sizeof(v_title), 
			ERget(F_AB02A1_VerifyPassword), 1, roleid);
/* # line 131 "abpaswrd.qsc" */	/* putform */
      {
        if (IIfsetio(_form) != 0) {
          IIputfldio(_title,(short *)0,1,32,0,e_title);
        } /* IIfsetio */
      }
/* # line 132 "abpaswrd.qsc" */	/* clear */
      {
        IIfldclear(_password);
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 138 "abpaswrd.qsc" */	/* host code */
		if (pass == 1)
		{
			/* Get password to verify */
/* # line 141 "abpaswrd.qsc" */	/* getform */
          {
            if (IIfsetio(_form) != 0) {
              IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,pbuf,_password);
            } /* IIfsetio */
          }
/* # line 143 "abpaswrd.qsc" */	/* host code */
			/* Prompt for password verification */
/* # line 144 "abpaswrd.qsc" */	/* putform */
          {
            if (IIfsetio(_form) != 0) {
              IIputfldio(_title,(short *)0,1,32,0,v_title);
            } /* IIfsetio */
          }
/* # line 145 "abpaswrd.qsc" */	/* clear */
          {
            IIfldclear(_password);
          }
/* # line 146 "abpaswrd.qsc" */	/* host code */
			pass = 2;
		}
		else
		{
			/* Check verification */
/* # line 151 "abpaswrd.qsc" */	/* getform */
          {
            if (IIfsetio(_form) != 0) {
              IIgetfldio((short *)0,1,32,0,password,_password);
            } /* IIfsetio */
          }
/* # line 152 "abpaswrd.qsc" */	/* host code */
			if (!STequal(pbuf, password))
			{
				IIUGerr(E_AB02A1_PwdBadVerify, 0, 0);
/* # line 155 "abpaswrd.qsc" */	/* putform */
          {
            if (IIfsetio(_form) != 0) {
              IIputfldio(_title,(short *)0,1,32,0,e_title);
            } /* IIfsetio */
          }
/* # line 156 "abpaswrd.qsc" */	/* clear */
          {
            IIfldclear(_password);
          }
/* # line 157 "abpaswrd.qsc" */	/* host code */
				pass = 1;
			}
			else
			{
				status = OK;
/* # line 162 "abpaswrd.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
/* # line 163 "abpaswrd.qsc" */	/* host code */
			}
		}	
        }
      } else if (IIretval() == 2) {
        {
/* # line 170 "abpaswrd.qsc" */	/* host code */
		if (pass == 1)
		{
			/* Cancel whole operation */
			status = FAIL;
/* # line 174 "abpaswrd.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
/* # line 175 "abpaswrd.qsc" */	/* host code */
		}
		else
		{
			/* Cancel verification */
/* # line 179 "abpaswrd.qsc" */	/* putform */
          {
            if (IIfsetio(_form) != 0) {
              IIputfldio(_title,(short *)0,1,32,0,e_title);
            } /* IIfsetio */
          }
/* # line 180 "abpaswrd.qsc" */	/* clear */
          {
            IIfldclear(_password);
          }
/* # line 181 "abpaswrd.qsc" */	/* host code */
			pass = 1;
		}
        }
      } else if (IIretval() == 3) {
        {
/* # line 188 "abpaswrd.qsc" */	/* host code */
                FEhelp(ERx("abpaswrd.hlp"), ERget(S_AB02A0_PwdHelpBanner));
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
    if (IInmuact(ERget(FE_OK),ERget(F_AB02A2_PwdOKexpl),2,2,1) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),2,2,2) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0101_ExplKeyHelp),2,2,3) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 191 "abpaswrd.qsc" */	/* host code */
	return status;
}
