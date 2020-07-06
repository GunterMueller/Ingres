# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<st.h>
#include	<er.h>
#include	<si.h>
#include	<fe.h>
#include	<erfe.h>
#include	<ug.h>
#include	<uf.h>
#include	<generr.h>
#include	<stdprmpt.h>
#include	"eric.h"
#include	"global.h"
# include "ictables.h"
/*
fool ming
#include	"ictables.sh"
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/**
** Name:    usertable.qc -	Display Table of DBMS Users Module.
**
** History:
**	18-may-89 (mgw)
**		Put string in ERx() call for internationalization
** 	15-Dec-89 (pete) porting change 90971 - Form will core dump on PS/2 AIX
**		if compiled with optimization (added "NO_OPTIM" line above).
**	30-oct-90 (pete)
**		Change user permissions from 4: Createdb, UpdSysCat, SetTRace,
**		SuperUser; to 6: Createdb, SysAdmin, SecAdmin, Operator,
**		SetTRace, AuditAll. This was done to support backend changes
**		made for TMAC/CHOTS and (later) the 6.5 (B1) project.
**      02-jun-93 (huffman)
**              Last change on 22-mar-93 did not have the si.h include file
**              This is a final resolution to it.
**	20-sep-1993 (robf)
**              Query standard catalog (iilocation_info) not internal catalog
**	        (iilocations). This allows users without QUERY_SYSCAT priv
**	        to use accessdb/catalogdb
**	21-jul-94 (robf)
**              Ability to create/destroy users toggles off MAINTAIN_USERS
**	        privilege, not SECURITY so update menuchoices accordingly.
**      03-feb-94 (harpa06) Bug 66690
**              Surround the user to be deleted in double quotes (") regardless
**              of whether or not there are special characters embedded in the
**              user name.
**      03-may-1999 (hanch04)
**          SIputrec returns EOF on error.
**	10-may-1999 (walro03)
**		Remove obsolete version string ps2_us5.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
FUNC_EXTERN bool            IIICiruIsReqUser();
FUNC_EXTERN i4		icpriv_string();
FUNC_EXTERN char	*icdelimit();
static	bool	Utf_add = FALSE;
  static char Usrtblfrm[]= ERx("usrtblfrm");
  static char Users_tbl[]= ERx("users");
  static char *Delete_menuitem;
  static char *Create_menuitem;
  static char *Edit_menuitem;
  static char *DumpUser_menuitem;
GLOBALREF       bool    Mgr;
GLOBALREF bool  IiicPrivSecurity; /* TRUE if user has SECURITY privilege */
GLOBALREF bool  IiicPrivMaintUser; /* TRUE if user has MAINTAIN_USERS privilege */
/*{
** Name:    usrtbl_form() -	Display Table of DBMS Users Frame.
**
** Description:
**	Frame to display a table of all the users known to the DBMS.
**
** History:
**	21-jul-93 (kellyp)
**		Switched the order of DELETE menuitem and EDIT menuitem
**		to make it consistent with other frames that have CREATE,
**		DELETE and EDIT menuitems.
**
*/
static VOID	usrtblinit();
static VOID	users_script();
static bool 	check_deluser();
static STATUS 	user_delete();
usrtbl_form()
  {
    char uname[FE_MAXNAME + 1];
	if (!Utf_add)
	{
		STATUS	stat;
		Utf_add = TRUE;
		stat = IIUFgtfGetForm(IIUFlcfLocateForm(), Usrtblfrm);
		if (stat != OK)
		{
			IIUGerr(S_IC0059_Get_Form_Error, UG_ERR_FATAL, 0);
		}
		if (Mgr && IiicPrivMaintUser)
		{
			Delete_menuitem = ERget(FE_Delete);
			Create_menuitem = ERget(FE_Create);
			Edit_menuitem = ERget(FE_Edit);
			DumpUser_menuitem = ERget(F_IC0070_User_Script);
		}
		else
		{
			Delete_menuitem = ERx("");
			Create_menuitem = ERx("");
			DumpUser_menuitem = ERx("");
			Edit_menuitem = ERget(FE_Examine);
		}
	}
/* # line 134 "usertbl.qsc" */	/* display */
    {
      if (IIdispfrm(Usrtblfrm,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 137 "usertbl.qsc" */	/* host code */
		usrtblinit();
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 142 "usertbl.qsc" */	/* host code */
		if (userform((char *)NULL) == OK)
			usrtblinit();
          }
        } else if (IIretval() == 2) {
          {
/* # line 148 "usertbl.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Usrtblfrm,Users_tbl,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,uname,(char *)"\
user_name");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 149 "usertbl.qsc" */	/* host code */
                (VOID) STtrmwhite(uname);
                if (uname[0] != EOS)
		    (VOID) userform(uname);
          }
        } else if (IIretval() == 3) {
          {
/* # line 156 "usertbl.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Usrtblfrm,Users_tbl,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,uname,(char *)"\
user_name");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 157 "usertbl.qsc" */	/* host code */
                if ( STtrmwhite(uname) <= 0 || uname[0] == EOS )
/* # line 158 "usertbl.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 160 "usertbl.qsc" */	/* host code */
		if (check_deluser(uname) == FALSE)
		{
		    	/* Don't even try to delete this user. 
			** Error already displayed. 
			*/
/* # line 165 "usertbl.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 166 "usertbl.qsc" */	/* host code */
		}
                if ( IIUFccConfirmChoice(CONF_DELETE, uname,
			ERget(F_IC0009_User), NULL, NULL) != CONFCH_YES)
		{
/* # line 171 "usertbl.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 172 "usertbl.qsc" */	/* host code */
		}
	        if (user_delete(uname) != OK)
/* # line 175 "usertbl.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 177 "usertbl.qsc" */	/* host code */
		usrtblinit();
          }
        } else if (IIretval() == 4) {
          {
/* # line 183 "usertbl.qsc" */	/* host code */
		(VOID) users_script();
          }
        } else if (IIretval() == 5) {
          {
/* # line 189 "usertbl.qsc" */	/* host code */
            FEtabfnd(Usrtblfrm, Users_tbl);
          }
        } else if (IIretval() == 6) {
          {
/* # line 195 "usertbl.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Usrtblfrm,Users_tbl,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 7) {
          {
/* # line 201 "usertbl.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Usrtblfrm,Users_tbl,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 8) {
          {
/* # line 207 "usertbl.qsc" */	/* host code */
		FEhelp(ERx("icusrtbl.hlp"), ERget(S_IC0057_User_table));
          }
        } else if (IIretval() == 9) {
          {
/* # line 213 "usertbl.qsc" */	/* breakdisplay */
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
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      goto IIfdE1;
IImuI1:
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(Create_menuitem,(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(Edit_menuitem,(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IInmuact(Delete_menuitem,(char *)0,2,2,3) == 0) goto IIfdE1;
      if (IInmuact(DumpUser_menuitem,(char *)0,2,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,5) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,7) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0101_ExplKeyHelp),0,0,8) == 0) 
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
  }
/* # line 218 "usertbl.qsc" */	/* host code */
static VOID
usrtblinit()
  {
  char user_name[FE_MAXNAME+1];
/* # line 285 "usertbl.sc" */	/* host code */
/* # line 226 "usertbl.qsc" */	/* message */
    {
      IImessage(ERget(S_IC0058_Retrieving_user_names));
    }
/* # line 228 "usertbl.qsc" */	/* inittable */
    {
      if (IItbinit(Usrtblfrm,Users_tbl,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 230 "usertbl.qsc" */	/* host code */
/* # line 296 "usertbl.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select user_name from iiusers order by user_name");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,user_name);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 301 "usertbl.sc" */	/* host code */
	{
/* # line 236 "usertbl.qsc" */	/* loadtable */
    {
      if (IItbact(Usrtblfrm,Users_tbl,1) != 0) {
        IItcoputio((char *)"user_name",(short *)0,1,32,0,user_name);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 237 "usertbl.qsc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
  }
static bool
check_deluser(u_name)
  char *u_name;
{
  char tablename[FE_MAXNAME + 1];
    bool isdba = FALSE;
    if (STtrmwhite(u_name) <= 0 || u_name[0] == EOS)
        return (FALSE);
    if (IIICiruIsReqUser(u_name))
    {
        IIUGerr(E_IC006B_CannotDeleteUser, UG_ERR_ERROR, 1, (PTR) u_name);
        return (FALSE);
    }
/* # line 330 "usertbl.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select database_name from iidatabase_info where database_owner=");
    IIputdomio((short *)0,1,32,0,u_name);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,tablename);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 335 "usertbl.sc" */	/* host code */
    {
        IIUGerr(E_IC0055_User_is_a_DBA_no_del, 0, 0);
	isdba = TRUE;
/* # line 338 "usertbl.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE2;
        if (sqlca.sqlcode < 0) 
          IIsqPrint(&sqlca);
      }
/* # line 339 "usertbl.sc" */	/* host code */
    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
    if (isdba)
        return (FALSE);
    return (TRUE);
}
/*{
** Name:	user_delete()	- Delete a user
**
** Description:
**	Makes some validity checks, then deletes the user via a call to
**	deluser().
**
** Inputs:
**	char	*u_name;
**
** Outputs:
**	none.
**
** Returns:
**	OK	- User was deleted
**	FAIL	- User could not be deleted (failed some validity check, etc.)
**
** History:
**	2-apr-1991 (pete)
**		Remove check for username=$ingres, as part of fix for bug 21138.
**		Pass in the u_name argument rather than GETFORMing it again.
**		Move the tests for whether user is a Newuser, or a DBA to another
**		routine, so these checks can be made BEFORE user is prompted for
**		"do you really want to delete user...".
**	17-sep-1993 (kellyp)
**		Changed DROP USER to NOT use single-quotes for user names. (b55053)
**
**	16-Feb-1994 (michael)
**		deleted usage of explicit double quotes and UN-normalized uname 
**		(to add any necessary quotes).
**      03-feb-1994 (harpa06) Bug 66690
**              Surrounded the user to be deleted in double quotes (") 
**		regardless of whether or not there are special characters 
**		embedded in the user name.
*/
static STATUS
user_delete(u_name)
char	*u_name;
  {
  char sbuffer[256];
	    char		uNrmlUserName[FE_UNRML_MAXNAME + 1];
/* # line 325 "usertbl.qsc" */	/* message */
    {
      IImessage(ERget(S_IC0023_Updating_database));
    }
/* # line 326 "usertbl.qsc" */	/* host code */
    IIUGrqd_Requote_dlm(u_name,uNrmlUserName);
    STprintf(sbuffer, ERx("DROP USER %s"), uNrmlUserName);
/* # line 395 "usertbl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 396 "usertbl.sc" */	/* host code */
    if (sqlca.sqlcode < 0 && sqlca.sqlcode != (-E_GE0032_WARNING))
    {
	IIUGerr(E_IC0056_User_does_not_exist, 0, 0);
	return FAIL;		/* no such user */
    }
    return OK;
  }
/*{
** Name:	users_script()	- create SQL script of CREATE USER cmds.
**
** Description: Creates an SQL script which creates users from existing 
**		system.
**
** Inputs:
**	none.
**
** Outputs:
**	none.
**
** Returns:
**	none.
**
** History:
**	16-mar-1993 (billc) 
**		written
**	22-jun-1993 (kellyp)
**		Appended \g to end of the users.sqls.sql  (bug 52055)
**	20-jul-94 (robf)
**              Explicitly issue NOPROFILE if applicable.
**		Issue a \go after each statements rather than at
**		the end.
**		
*/
#define SCRIPT_OUT	ERx("users.sql")
static void
users_script()
{
  char user_name[33];
  char default_group[33];
  char profile_name[33];
  char expire_date[25];
  char audit_all[2];
  char audit_query_text[2];
  i4 status;
  i4 def_priv;
    LOCATION	pathloc;
    char	pathlocbuf[MAX_LOC + 1];
    LOCATION	fileloc;
    char	filelocbuf[MAX_LOC + 1];
    char	*s;
    FILE	*fp;
    char	obuf[2048];
    char	privbuf[1024];
    char	auditbuf[1024];
    char	defprivbuf[1024];
    bool	got_priv=FALSE;
    bool	got_defpriv=FALSE;
    bool	got_audit=FALSE;
    STATUS	stat;
    STcopy(SCRIPT_OUT, filelocbuf);
    if ((stat = LOfroms(FILENAME, filelocbuf, &fileloc)) != OK
      || (stat = LOgt(pathlocbuf, &pathloc)) != OK
      || (stat = LOstfile(&fileloc, &pathloc)) != OK
      || (stat = SIfopen(&pathloc, ERx("w"), SI_TXT, 0, &fp)) != OK)
    {
	goto badbad;
    }
/* # line 406 "usertbl.qsc" */	/* message */
  {
    IImessage(ERget(S_IC0062_Writing_Script));
  }
/* # line 408 "usertbl.qsc" */	/* host code */
/* # line 470 "usertbl.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select user_name, default_group, profile_name, audit_all, audit_query\
_text, internal_status, internal_def_priv, expire_date from iiusers or\
der by user_name");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,user_name);
      IIgetdomio((short *)0,1,32,32,default_group);
      IIgetdomio((short *)0,1,32,32,profile_name);
      IIgetdomio((short *)0,1,32,1,audit_all);
      IIgetdomio((short *)0,1,32,1,audit_query_text);
      IIgetdomio((short *)0,1,30,4,&status);
      IIgetdomio((short *)0,1,30,4,&def_priv);
      IIgetdomio((short *)0,1,32,24,expire_date);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 490 "usertbl.sc" */	/* host code */
    {
	(void) STtrmwhite(user_name);
	if (!STequal(user_name, ERx("$ingres")) 
		&& !STequal(user_name, ERx("$ingres")))
	{
	    STprintf(obuf, ERx("CREATE USER %s WITH\n"), icdelimit(user_name));
	    (void) STtrmwhite(default_group);
	    (void) STtrmwhite(expire_date);
	    (void) STtrmwhite(profile_name);
	    /*
	    ** Build privileges/audit lists
	    */
	    if(icpriv_string(status, privbuf)>0)
		got_priv=TRUE;
	    else
		got_priv=FALSE;
	    if(icpriv_string(def_priv, defprivbuf)>0)
		got_defpriv=TRUE;
	    else
		got_defpriv=FALSE;
	    if(icaudit_string(status, auditbuf)>0)
		got_audit=TRUE;
	    else
		got_audit=FALSE;
	    if (default_group[0] != EOS)
	    {
		STcat(obuf, ERx("  GROUP="));
		STcat(obuf, icdelimit(default_group));
	    }
	    else 
		STcat(obuf, ERx("  NOGROUP"));
	    if (got_priv)
	    {
		STcat(obuf, ERx(",\n  PRIVILEGES=("));
		STcat(obuf, privbuf);
		STcat(obuf, ERx(")"));
	    }
	    else
		STcat(obuf, ERx(",\n  NOPRIVILEGES"));
	    if(got_defpriv)
	    {
		STcat(obuf, ERx(",\n  DEFAULT_PRIVILEGES=("));
		STcat(obuf, defprivbuf);
		STcat(obuf, ERx(")"));
	    }
	    else
		STcat(obuf, ERx(",\n  NODEFAULT_PRIVILEGES"));
	    if(expire_date[0]!=EOS)
	    {
		STcat(obuf, ERx(",\n  EXPIRE_DATE='"));
		STcat(obuf, expire_date);
		STcat(obuf, ERx("'"));
	    }
	    else
		STcat(obuf, ERx(",\n  NOEXPIRE_DATE"));
	    if(profile_name[0]!=EOS)
	    {
		STcat(obuf, ERx(",\n  PROFILE="));
		STcat(obuf, icdelimit(profile_name));
	    }
	    else
		STcat(obuf, ERx(",\n  NOPROFILE"));
	    if(got_audit)
	    {
		STcat(obuf, ERx(",\n  SECURITY_AUDIT=("));
		STcat(obuf, auditbuf);
		STcat(obuf, ERx(")"));
	    }
	    else
		STcat(obuf, ERx(",\n  NOSECURITY_AUDIT"));
	    STcat(obuf, ERx(";\n\\go\n"));
	    if (SIputrec(obuf, fp) == EOF)
	    {
		stat = FAIL;
/* # line 564 "usertbl.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE3;
        if (sqlca.sqlcode < 0) 
          IIsqPrint(&sqlca);
      }
/* # line 565 "usertbl.sc" */	/* host code */
	    }
	}
    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 569 "usertbl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 570 "usertbl.sc" */	/* host code */
    if (stat == OK && SIclose(fp) == OK)
    {
  char tbuf[1034];
	LOtos(&pathloc, &s);
	(void) IIUGfmt(tbuf, sizeof(tbuf)-1, ERget(E_IC0069_Script_Success),
		1, (PTR) s);
/* # line 530 "usertbl.qsc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(tbuf);
  }
/* # line 531 "usertbl.qsc" */	/* host code */
	return;
    }
badbad:
    LOtos(&pathloc, &s);
    IIUGerr(E_IC0066_Script_Fail, UG_ERR_ERROR, 1, (PTR) s);
}
