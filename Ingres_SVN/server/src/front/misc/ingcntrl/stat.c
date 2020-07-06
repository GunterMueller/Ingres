# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<gl.h>
#include	<sl.h>
#include	<iicommon.h>
#include	<fe.h>
#include	<er.h>
#include	<erug.h>
#include	"eric.h"
#include	"global.h"
/**
** Name:        STAT.QSC        - INGCNTRL USER FRAME HANDLING
**
** Description:
**
**	put_user_stat	- Put user permission info on screen.
**	get_user_stat 	- Get user permission info from screen.
**
** History:
**	31-oct-90 (pete)
**		Added this module header.
**	31-oct-90 (pete)
**		Added support for 6 new user permissions (for B1). Inceased
**		a few char buffer sizes to FE_MAXNAME+1.
**	20-sep-93 (robf)
**	        Add support for new user privilege/audit
**	21-jul-94  (robf)
**              Add support for requestable (non-default) privileges
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
FUNC_EXTERN     bool    IIUGyn();
typedef struct {
	i4	sw_mask;		/* status word mask */
	char	sw_name[FE_MAXNAME+1];		/* field name on form */
} SWDESC;
static	SWDESC	swdesc[] = {
		U_CREATDB,	ERx("createdb"),
		U_SECURITY,	ERx("security"),
		U_SYSADMIN,	ERx("system_admin"),
		U_MAINTAIN_USER,	ERx("maintain_users"),
		U_OPERATOR,	ERx("operator"),
		U_TRACE,	ERx("trace"),
		U_ALTER_AUDIT,	ERx("maintain_audit"),
		U_AUDITOR,	ERx("auditor"),
		0,		ERx("")
};
static	SWDESC	swaudit[] = {
		U_AUDIT,	ERx("audit_all"),
		U_AUDIT_QRYTEXT,ERx("audit_query_text"),
		0,		ERx("")
};
  GLOBALREF char Usrform[];
/*
** put_user_stat - puts user privs & audit values to screen
*/
VOID
put_user_stat (status, def_stat)
i4	status;
i4     def_stat;
  {
    SWDESC	*p;
    char *fnm;
    char *fval;
    for ( p = &swdesc[0] ; p->sw_mask != 0 ; p++)
    {
	fnm = p->sw_name;
	if (def_stat & p->sw_mask )
		fval = Yes;
	else if(status & p->sw_mask)
		fval = Request;
	else
		fval = No;
/* # line 89 "stat.qsc" */	/* putform */
    {
      if (IIfsetio(Usrform) != 0) {
        IIputfldio(fnm,(short *)0,1,32,0,fval);
      } /* IIfsetio */
    }
/* # line 90 "stat.qsc" */	/* host code */
    }
    /* And now audit flags */
    for ( p = &swaudit[0] ; p->sw_mask != 0 ; p++)
    {
	fnm = p->sw_name;
	if(status & p->sw_mask)
		fval = Yes;
	else
		fval = No;
/* # line 100 "stat.qsc" */	/* putform */
    {
      if (IIfsetio(Usrform) != 0) {
        IIputfldio(fnm,(short *)0,1,32,0,fval);
      } /* IIfsetio */
    }
/* # line 101 "stat.qsc" */	/* host code */
    }
  }
VOID
get_user_stat (sp, def_stat)
i4	*sp;
i4 *def_stat;
  {
    register SWDESC	*p;
    for (p = &swdesc[0] ; p->sw_mask != 0 ; p++)
    {
    char *fnm = p->sw_name;
    char fval[32];
/* # line 116 "stat.qsc" */	/* getform */
    {
      if (IIfsetio(Usrform) != 0) {
        IIgetfldio((short *)0,1,32,31,fval,fnm);
      } /* IIfsetio */
    }
/* # line 117 "stat.qsc" */	/* host code */
	if(*fval=='y')
	{
	    *sp |= p->sw_mask;
	    *def_stat |=  p->sw_mask;
	}
	else if (*fval== 'r')
	{
	    *sp |= p->sw_mask;
	    *def_stat &=  ~p->sw_mask;
	}
	else
	{
	    *def_stat &=  ~p->sw_mask;
	    *sp &=  ~p->sw_mask;
	}
    }
  }
/*
** Name: get_audit_stat
**
** Description:
**	Builds the audit status mask from user input fields
**
** Outputs:
**	sp - Status
**
** History:
**	20-sep-93 (robf)
**	    Created from get_user_stat()
*/
VOID
get_audit_stat(sp)
i4	*sp;
  {
    register SWDESC	*p;
    *sp = 0;
    for (p = &swaudit[0] ; p->sw_mask != 0 ; p++)
    {
    char *fnm = p->sw_name;
    char fval[32];
/* # line 160 "stat.qsc" */	/* getform */
    {
      if (IIfsetio(Usrform) != 0) {
        IIgetfldio((short *)0,1,32,31,fval,fnm);
      } /* IIfsetio */
    }
/* # line 161 "stat.qsc" */	/* host code */
	if (IIUGyn(fval, (STATUS* )NULL))
	    *sp |= p->sw_mask;
    }
  }
VOID
ul_user_stat ()
  {
    SWDESC	*p;
    char *nm;
    for ( p = &swdesc[0] ; p->sw_mask != 0 ; p++)
    {
	nm = p->sw_name;
/* # line 175 "stat.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Usrform,(char *)0, (char *)0) != 0) {
        IIstfsio(17,nm,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 176 "stat.qsc" */	/* host code */
    }
    for ( p = &swaudit[0] ; p->sw_mask != 0 ; p++)
    {
	nm = p->sw_name;
/* # line 180 "stat.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Usrform,(char *)0, (char *)0) != 0) {
        IIstfsio(17,nm,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 181 "stat.qsc" */	/* host code */
    }
  }
VOID
ro_user_stat ()
  {
    SWDESC	*p;
    char *nm;
    for ( p = &swdesc[0] ; p->sw_mask != 0 ; p++)
    {
	nm = p->sw_name;
/* # line 193 "stat.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Usrform,(char *)0, (char *)0) != 0) {
        IIstfsio(53,nm,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 194 "stat.qsc" */	/* host code */
    }
    for ( p = &swaudit[0] ; p->sw_mask != 0 ; p++)
    {
	nm = p->sw_name;
/* # line 198 "stat.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Usrform,(char *)0, (char *)0) != 0) {
        IIstfsio(53,nm,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 199 "stat.qsc" */	/* host code */
    }
  }
