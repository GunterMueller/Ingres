# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
**      All rights reserved.
*/
#include        <compat.h>
#include        <st.h>
#include        <er.h>
#include        <cv.h>
#include        <gl.h>
#include        <sl.h>
#include        <cm.h>
#include        <iicommon.h>
#include        <fe.h>
#include        <erfe.h>
#include        <ug.h>
#include        <uf.h>
#include        <ui.h>
#include        <stdprmpt.h>
#include        <generr.h>
#include        "eric.h"
#include        "global.h"
# include "ictables.h"
/*
fool ming:
#include        "ictables.qsh"
*/
# define USR_EXT_PWD_FLAG       0x20000
/**
** Name:        USER.QSC        - INGCNTRL USER FRAME HANDLING
**
** Description:
**      This file contains routines for handling the ACCESSDB/CATALOGDB
**      User frame. It allows users to see information about user accounts
**      and to alter it or add new users in ACCESSDB.
**      This file defines:
**
**      userform()      Entry point for the user form from ACCESSDB
**
** History:
**      18-may-89 (mgw)
**              Made updates to handle default groups for TERMINATOR project.
**      27-sep-1989 (jennifer)
**             Added new SQL statement support for B1/C2 security requirements.
**             For this file CREATE USER, DROP USER, and ALTER USER statements 
**             replaced inserts/replaces into iiuser catalog directly.  Also
**             substitued GRANT ACCESS statements to replace direct updates 
**             to iidbpriv.
**      15-Dec-89 (pete) porting change 90971 - Form will core dump on PS/2 AIX
**              if compiled with optimization (added "NO_OPTIM" line above).
**      ~1-Jan-90 (pete) - Allow selection of Help, End & Delete even when on
**                      invalid field; check for unsaved changed when
**                      user hits menuitem End.
**      23-apr-1990 (pete)
**              fix 6.3 bug 21342 with bad explanations for Help & End.
**      19-oct-1990 (pete)
**              Merge in changes done by Jennifer Lyerla in 9/89 for B1 proj.
**      30-oct-90 (pete)
**              Change user permissions from 4: Createdb, UpdSysCat, SetTRace,
**              SuperUser; to 6: Createdb, SysAdmin, SecAdmin, Operator,
**              SetTRace, AuditAll. This was done to support backend changes
**              made for TMAC/CHOTS and (later) the 6.5 (B1) project.
**              Also support access modes in table field.
**      5-jan-1991 (pete)
**              Fix bug 35121 -- bad cursor positioning after enter bad
**              group name. (already fixed in Orion, 6.5, line).
**      20-sep-1993 (robf)
**              Query standard catalog (iilocation_info) not internal catalog
**              (iilocations)
**              Add support for MAINTAIN_USER, MAINTAIN_AUDIT and AUDITOR
**              privileges, plus QUERY_TEXT security auditing.
**      23-may-94 (michael/kirke)
**              accessdb in a fips installation, regular identifiers are
**              stored as case-sensitive instead of all uppercase.
**      12-dec-94 (harpa06)
**              Integrated INGRES 6.4 bug fix #50334 by nick:
**              Don't allow the removal of super user permissions from
**              $ingres or ingres.
**      30-dec-1994 (andyw)
**              Problems found creating users in HP platforms
**              where we get a syntax error on create user
**              appears to be an uninitialized variable in user_create()
**      16-jan-1995 (ramra01)
**              Backout fix by harpa06. Causes parser error on
**              user Edit or Create
**      17-jan-94 (harpa06)
**              Bug #66223 - Removed the problem of when tabbing through the
**              User Information screen and then saving the information, the
**              user Permissions would change from their previous values to
**              "r"s (privilege on Request.) A check is now done to see if there
**              were any REAL changes.
**
**              Bug #50334 - Re-submitted fix since ir did not cause parser errors
**      30-jan-1995 (liibi01)
**              Accessdb doesn't convert group name and profile name into
**              upper case in create or edit screen in FIPS installation. And
**              it doesn't convert them to lower case in regular installation.  
**      7-feb-95 (hanch04)
**              fix mixing EXEC SQL lines
**      20-Mar-1995 (liibi01)
**             Bug 67522, when creating user, select save, user can input any
**             invalid value besides 'Y' or 'N' and the program treats the
**             invalid input value as 'Y'.
**      23-mar-95 (sarjo01)
**              Bug 67523: userform(): change order of menu items so that
**              ListChoices (9) is last. That way, when it is disabled and
**              reenabled moving through fields, the () number of other
**              items won't be changed.
**      01-may-95 (harpa06)
**              Bug #68422: Added a flag in user_privileges to indicate that the
**              priviliges form has been modified which will reflect the User
**              Information form.
**      23-aug-95 (emmag)
**              Changed OpenINGRES to OpenIngres.
**      07-nov-1995 (canor01)
**              Add forward function declarations to clean up compiler 
**              warnings.
**      12-jul-96 (sarjo01)
**              Bug 77705: user_create(): increase size of promptMsg[]
**              buffer to 80. STprintf() was overrunning this buffer,
**              corrupting other stack vars. This caused a memory access
**              violation on NT.
**	06-jan-97 (mcgem01)
**		Bug 79866.   Only update the changed_access flag if the current
**		tablefield is Access_tbl.
**	01-jun-98 (kitch01)
**		Bug 74731. Disallow ability to set external password to yes if there
**		is no external password verification utility setup for this user.
**		Bug 77451. If there is no user password set for this user then set the 
**		user password field to 'no'.
**		Bug 91019. Invalid 'alter user' syntax generated when a user tries to
**		change their own password.
**      07-Jan-99 (hanal04)
**              Ensure user_modify and user_create do not submit querries
**              which modify the audit state when the user running accessdb
**              does not have the maintain_audit privilege. b81618.
**	10-may-1999 (walro03)
**		Remove obsolete version string ps2_us5.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	20-Apr-2004 (bonro01)
**	    Allow Ingres to be installed as a user other than 'ingres'.
**	    'ingres' is no longer a special userid and may be deleted.
**	16-Nov-2004 (bonro01)
**	    Prevent install userid from being deleted.
**/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/* # line 152 "user.sc" */	/* host code */
FUNC_EXTERN     STATUS  fchkuname();
FUNC_EXTERN     VOID    put_user_stat();
FUNC_EXTERN     VOID    get_user_stat();
FUNC_EXTERN     VOID    ro_user_stat();
FUNC_EXTERN     VOID    ul_user_stat();
FUNC_EXTERN     STATUS  iiicsuSelectUser();
FUNC_EXTERN     STATUS  iiicsgSelectUsergroup();
FUNC_EXTERN     STATUS  iiicsrSelectRole();
FUNC_EXTERN     STATUS  iiicsdSelectDatabase();
FUNC_EXTERN     i4      iiicueUsergroupExists();
FUNC_EXTERN     STATUS  icSetGlobals();
FUNC_EXTERN     i4      icaudit_string();
FUNC_EXTERN     STATUS  user_password();
static bool     Newuser = FALSE;
static bool     Isdba = FALSE;
static STATUS   write_user_pwd(char *user_name);
static VOID     user_privileges (char *id, i4 *stat, i4 *def_stat);
static STATUS   profile_exist_chk(char *profile);
  static char Resume_fld[FE_MAXNAME+1];
  GLOBALREF char Usrform[];
  GLOBALREF char Pwdform[];
  GLOBALREF char Prvform[];
GLOBALREF       bool    Mgr;
  GLOBALREF char F_Maxname[];
GLOBALREF bool  IiicPrivMaintUser; /* TRUE if user has MAINTAIN_USERS priv*/
GLOBALREF bool  IiicC2Security;    /* TRUE if installation has C2 Security. */
/* b81618 */
GLOBALREF bool  IiicPrivMntAudit;  /* TRUE if user has MAINTAIN_AUDIT priv. */
GLOBALREF       char    *IC_allDBs;     /* "all databases" */
GLOBALREF       char    *Real_username;
GLOBALREF IIROLE Iirole;
GLOBALREF IIUSERGROUP Iiusergroup;
GLOBALREF IIUSER Iiuser;
GLOBALREF IIDATABASE Iidatabase;
  static char U_group[((2 * FE_MAXNAME) + 2 + 1)]= ERx("");
  static char U_profile[((2 * FE_MAXNAME) + 2 + 1)]= ERx("");
  static char U_exp_date[((2 * FE_MAXNAME) + 2 + 1)]= ERx("");
  static i4 U_status;
  static i4 U_def_status;
  static char U_name[((2 * FE_MAXNAME) + 2 + 1)]= ERx("");
static          bool    F_add = FALSE;
static  char    Comma[]         = ERx(","),
		SingleQuote[]   = ERx("'"),
		Rparen[]        = ERx(")"),
		Empty[]         = ERx("");
	/* names of 2 table fields on form "usrfrm": */
  static char Access_tbl[]= ERx("access");
  static char Database_tbl[]= ERx("owns");
  static char Object_name[FE_MAXNAME+1];
static STATUS   user_init();
static STATUS   user_new();
static STATUS   user_write();
static STATUS   get_dbtbl();
static STATUS   user_create();
static STATUS   user_modify();
static bool     usr_chg_chk();
static bool     usr_mod_chk();
static bool     usr_delim_chk();
static STATUS   grp_exist_chk();
static void		check_pwd_enabled();
bool            IIICiruIsReqUser();
char            *icdelimit();
i4              icpriv_string();
  static char *Mode;
  static char *Save_menuitem;
  static char *Listchoices_menuitem;
  static char *Title;
  static char *Password_menuitem;
  static char *Privileges_menuitem;
static          bool    PrivChg = FALSE;
/*{
** Name:        userform()      - Main routine for ACCESSDB's User frame
**
** Description:
**      Calls user_init() to initialize the USRFRM form and make some
**      preliminary validity checks. Then goes into a display loop for
**      the main menu of the User frame of ACCESSDB.
**
** Inputs:
**      id      - Name of a valid or new INGRES user
**
** Outputs:
**      none.
**
** Returns:
**      FAIL    - if user could not be initialized
**      OK      - successful frame interaction
**
** Side Effects:
**
** History:
**      18-may-89 (mgw)
**              Added comments.
**      31-oct-90 (pete)
**              Suppress the Group field on the screen if no group support
**              (rather than switching between 2 different screens, as before).
**      2-apr-91 (pete)
**              fix bug 21138; don't allow user ingres or $ingres to be
**              deleted.
**      6-aug-93 (kellyp)
**              Make access for authorized database default to yes.
**              Also, when returning a value from the picklist, place the
**              value on the current row if the current row is empty.
**              Otherwise, place the value at the end of the table.
**      18-aug-93 (kellyp)
**              Added CANCEL menuitem to the CREATE screen.
**      24-aug-93 (kellyp)
**              Deleted ACTIVATION for the access column,
**              changed RowDelete to Delete, plus more. 
**      16-sep-93 (kellyp)
**              Took out IIUFfieldhelp
**      17-sep-93 (kellyp)
**              Removed CANCEL menuitem from the CREATE screen.
**      22-nov-93 (kellyp)
**              Should not ACTIVATE validation when save_menuitem is
**              selected since this will bring up the error message twice. 
**              ( related to bug 56833 )
**      20-jul-94 (robf)
**              Listchoices menuitem was working inconsistently profile name
**              field, corrected.
**      12-dec-94 (nanpr01)
**              BUG # 65953. When creating user, id is set to null.
**              So in user_privileges parameter, use U_name instead of id.
**      17-jan-94 (harpa06)
**              Bug #66223 - Removed the problem of when tabbing through the
**              User Information screen and then saving the information, the
**              user Permissions would change from their previous values to
**              "r"s (privilege on Request.) A check is now done to see if there
**              were any REAL changes.
**      23-mar-95 (sarjo01)
**              Bug 67523: change order of menu items so that
**              ListChoices (9) is last. That way, when it is disabled and
**              reenabled moving through fields, the () number of other
**              items won't be changed.
**      30-jul-96 (rodjo04)
**              Bug 77423: Fix given so that when you edit a users'
**              privilege(s), it will save what you edited via Save(F3). 
**      18-Sep-96 (rodjo04)
**              Bug 77418: Fix given so that when a user adds a database to 
**              the 'Authorized Databases' field and presses Save(F3) without
**              first tabbing or pressing return (while in the field), It 
**              will save the row that contains the database object. 
**      13-Nov-96 (rodjo04)
**              Bug 79063: Check now given to see if the cursor is positioned 
**              on a table field in the form before trying to get row 
**              information. 
**              
*/
STATUS
userform (id)
char    *id;
  {
    /* Check if form added */
    if (!F_add)
    {
	STATUS  stat;
	stat = IIUFgtfGetForm(IIUFlcfLocateForm(), Usrform);
	if (stat != OK)
	{
	    IIUGerr(S_IC0059_Get_Form_Error, UG_ERR_ERROR, 0);
	    return FAIL;
	}
	stat = IIUFgtfGetForm(IIUFlcfLocateForm(), Pwdform);
	if (stat != OK)
	{
	    IIUGerr(S_IC0059_Get_Form_Error, UG_ERR_ERROR, 0);
	    return FAIL;
	}
	stat = IIUFgtfGetForm(IIUFlcfLocateForm(), Prvform);
	if (stat != OK)
	{
	    IIUGerr(S_IC0059_Get_Form_Error, UG_ERR_ERROR, 0);
	    return FAIL;
	}
	if (!Mgr)
	    Title = ERget(F_IC0083_Catalogdb_User_Info);
	if (!IiicC2Security)
	{
	    /* Installation has not purchased Security Auditing feature. */
/* # line 358 "user.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Usrform,(char *)0, (char *)0) != 0) {
        IIstfsio(54,(char *)"audit_all",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(54,(char *)"audit_query_text",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(54,(char *)"maintain_audit",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(54,(char *)"auditor",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 362 "user.qsc" */	/* host code */
	}
	F_add = TRUE;
    }
    if (Mgr)
    {
	if (id == NULL)
	    Title = ERget(F_IC0111_Accessdb_Create_User);
	else
	    Title = ERget(F_IC0082_Accessdb_User_Info);
    }
    /*
    ** Must change password after creation
    */
    if (id == NULL)
	Password_menuitem = Empty;
    else
	Password_menuitem = ERx("Password");
    Privileges_menuitem = ERx("Privileges");
    if (Mgr && IiicPrivMaintUser)
    {
	Save_menuitem = ERget(FE_Save);
	Listchoices_menuitem = ERget(FE_Lookup);
	Mode = ERx("update");
	/* underline permission fields. */
	ul_user_stat();
/* # line 390 "user.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Usrform,(char *)0, (char *)0) != 0) {
        IIstfsio(17,(char *)"default_group",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(17,(char *)"profile_name",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(17,(char *)"expire_date",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 393 "user.qsc" */	/* host code */
    }
    else
    {
	Save_menuitem = Empty;
	Listchoices_menuitem = Empty;
	Mode = ERx("read");
	/* set permission fields to be readonly. */
	ro_user_stat();
/* # line 402 "user.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Usrform,(char *)0, (char *)0) != 0) {
        IIstfsio(53,(char *)"default_group",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(53,(char *)"profile_name",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(53,(char *)"expire_date",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 405 "user.qsc" */	/* host code */
    }
    /* Check if user exists and initialize form */
    if (user_init(id) != OK)
	return FAIL;
/* # line 411 "user.qsc" */	/* display */
    {
      if (IIdispfrm(Usrform,Mode) == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio((char *)"title",(short *)0,1,32,0,Title);
      {
/* # line 414 "user.qsc" */	/* putform */
        {
          if (IIfsetio(Usrform) != 0) {
            IIputfldio((char *)"name",(short *)0,1,32,0,U_name);
            IIputfldio((char *)"default_group",(short *)0,1,32,0,U_group);
            IIputfldio((char *)"profile_name",(short *)0,1,32,0,U_profile);
            IIputfldio((char *)"expire_date",(short *)0,1,32,0,U_exp_date);
          } /* IIfsetio */
        }
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
            char u_name[FE_MAXNAME + 1];
/* # line 422 "user.qsc" */	/* host code */
	int rc = 0;
/* # line 424 "user.qsc" */	/* getform */
            {
              if (IIfsetio(Usrform) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,u_name,(char *)"\
name");
              } /* IIfsetio */
            }
/* # line 425 "user.qsc" */	/* host code */
	if (IIUGdlm_ChkdlmBEobject( u_name, u_name, FALSE ) == UI_BOGUS_ID)
	{
	    i4       Maxn = DB_MAXNAME;
	    IIUGerr(E_IC002D_bad_username, 0, 2, u_name, (PTR)&Maxn);
/* # line 429 "user.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 430 "user.qsc" */	/* host code */
	}
	rc = user_new(u_name);
/* # line 433 "user.qsc" */	/* putform */
            {
              if (IIfsetio(Usrform) != 0) {
                IIputfldio((char *)"name",(short *)0,1,32,0,u_name);
              } /* IIfsetio */
            }
/* # line 434 "user.qsc" */	/* host code */
	if (rc == OK)
/* # line 435 "user.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
/* # line 436 "user.qsc" */	/* host code */
	else
/* # line 437 "user.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 442 "user.qsc" */	/* set_frs */
            {
              if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIstfsio(67,ERget(FE_Lookup),0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
          }
        } else if (IIretval() == 3) {
          {
            char u_profile[FE_MAXNAME + 1];
/* # line 448 "user.qsc" */	/* set_frs */
            {
              if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIstfsio(67,ERget(FE_Lookup),0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 449 "user.qsc" */	/* getform */
            {
              if (IIfsetio(Usrform) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,u_profile,(char *)"\
profile_name");
              } /* IIfsetio */
            }
/* # line 450 "user.qsc" */	/* host code */
	IIUGdlm_ChkdlmBEobject( u_profile, u_profile, FALSE);
/* # line 451 "user.qsc" */	/* putform */
            {
              if (IIfsetio(Usrform) != 0) {
                IIputfldio((char *)"profile_name",(short *)0,1,32,0,u_profile);
              } /* IIfsetio */
            }
/* # line 452 "user.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 457 "user.qsc" */	/* set_frs */
            {
              if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIstfsio(67,ERget(FE_Lookup),0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
          }
        } else if (IIretval() == 5) {
          {
            char u_group[FE_MAXNAME + 1];
/* # line 464 "user.qsc" */	/* set_frs */
            {
              if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIstfsio(67,ERget(FE_Lookup),0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 465 "user.qsc" */	/* getform */
            {
              if (IIfsetio(Usrform) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,u_group,(char *)"\
default_group");
              } /* IIfsetio */
            }
/* # line 466 "user.qsc" */	/* host code */
	IIUGdlm_ChkdlmBEobject( u_group, u_group, FALSE);
/* # line 467 "user.qsc" */	/* putform */
            {
              if (IIfsetio(Usrform) != 0) {
                IIputfldio((char *)"default_group",(short *)0,1,32,0,u_group);
              } /* IIfsetio */
            }
/* # line 468 "user.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 6) {
          {
            i4 changed;
/* # line 475 "user.qsc" */	/* inquire_frs */
            {
              if (IIiqset(5,0,(char *)"",(char *)"",(char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&changed,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 476 "user.qsc" */	/* host code */
	if (changed != 0)
	{
	    /* user has changed this value. validate it */
/* # line 480 "user.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Usrform,Access_tbl,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,(char *)"\
dbname");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 482 "user.qsc" */	/* host code */
	    if (STtrmwhite(Object_name) <= 0)
/* # line 483 "user.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
/* # line 485 "user.qsc" */	/* host code */
	    /* database names are never case sensitive */
	    IIUGlbo_lowerBEobject(Object_name);
	    if (!STequal(IC_allDBs, Object_name))
	    {
		/* get Database info */
		if (iiicsdSelectDatabase(Object_name, &Iidatabase) != OK)
		{
		    /* no such database */
/* # line 494 "user.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 495 "user.qsc" */	/* host code */
		}
	    }
/* # line 498 "user.qsc" */	/* putrow */
            {
              if (IItbsetio(3,Usrform,Access_tbl,-2) != 0) {
                IItcoputio((char *)"access",(short *)0,1,32,0,ERx("yes"));
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 500 "user.qsc" */	/* host code */
	    /* mark column so won't be validated next time */
/* # line 501 "user.qsc" */	/* set_frs */
            {
              if (IIiqset(5,0,(char *)"",(char *)"",(char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 502 "user.qsc" */	/* host code */
	}
	else
/* # line 504 "user.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 7) {
          {
            i4 changed_access = 0;
            i4 istable;
/* # line 514 "user.qsc" */	/* host code */
	STATUS      stat;
	if (Save_menuitem[0] == EOS)
	{
	    /* this was selected via the FRSkey */
	    FTbell();
/* # line 520 "user.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 521 "user.qsc" */	/* host code */
	}
	if (Mgr)
	{
	    /* user is running Accessdb */
            i4 changed;
/* # line 530 "user.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&changed,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 532 "user.qsc" */	/* host code */
	    if ((!changed) && (!PrivChg))
/* # line 533 "user.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 534 "user.qsc" */	/* host code */
	}
    /*  We know that the user has updated something, but did
    **  the user update the access table form without pressing
    **  tab or return? See if so and put the row.
    */
/* # line 541 "user.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&istable,28,(char *)0,0);
              } /* IIiqset */
            }
/* # line 543 "user.qsc" */	/* host code */
      if (istable) 
      {
/* # line 546 "user.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,19,
                (char *)0,0);
              } /* IIiqset */
            }
/* # line 548 "user.qsc" */	/* host code */
        if (STequal(Object_name, Access_tbl))
/* # line 549 "user.qsc" */	/* inquire_frs */
            {
              if (IIiqset(5,0,(char *)"",(char *)"",(char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&changed_access,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 551 "user.qsc" */	/* host code */
	  if (changed_access != 0)
	  {
	      /* user has changed this value. validate it */
/* # line 555 "user.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Usrform,Access_tbl,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,(char *)"\
dbname");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 557 "user.qsc" */	/* host code */
	      if (STtrmwhite(Object_name) <= 0)
/* # line 558 "user.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
/* # line 560 "user.qsc" */	/* host code */
	      /* database names are never case sensitive */
	      IIUGlbo_lowerBEobject(Object_name);
	      if (!STequal(IC_allDBs, Object_name))
	      {
		  /* get Database info */
		  if (iiicsdSelectDatabase(Object_name, &Iidatabase) != OK)
		  {
		      /* no such database */
/* # line 569 "user.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 570 "user.qsc" */	/* host code */
		  }
	      }
/* # line 573 "user.qsc" */	/* putrow */
            {
              if (IItbsetio(3,Usrform,Access_tbl,-2) != 0) {
                IItcoputio((char *)"access",(short *)0,1,32,0,ERx("yes"));
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 575 "user.qsc" */	/* host code */
	      /* mark column so won't be validated next time */
/* # line 576 "user.qsc" */	/* set_frs */
            {
              if (IIiqset(5,0,(char *)"",(char *)"",(char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 577 "user.qsc" */	/* host code */
	  }
      }   
/* # line 581 "user.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 583 "user.qsc" */	/* host code */
	STcopy (Access_tbl, Resume_fld);
	if (usr_chg_chk() || user_write() != OK)
/* # line 586 "user.qsc" */	/* resume */
            {
              IIresfld(Resume_fld);
              if (1) goto IIfdB1;
            }
/* # line 588 "user.qsc" */	/* host code */
    PrivChg=FALSE;
/* # line 589 "user.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 8) {
          {
/* # line 595 "user.qsc" */	/* host code */
	if (Mgr)
	{
	    if (Newuser)
		FEhelp(ERx("icusrfrm.hlp"), ERget(S_IC004B_User_form));
	    else
		FEhelp(ERx("iceusrfr.hlp"), ERget(S_IC004B_User_form));
	}
	else
	{
	    FEhelp(ERx("icrusrfr.hlp"), ERget(S_IC004B_User_form));
	}
          }
        } else if (IIretval() == 9) {
          {
            i4 changed;
/* # line 614 "user.qsc" */	/* host code */
	if (Mgr)
	{
	    /* user is running Accessdb */
/* # line 617 "user.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&changed,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 619 "user.qsc" */	/* host code */
	    if ((changed)||(PrivChg))
	    {
		if (usr_chg_chk())
		{
		    PrivChg=FALSE;
/* # line 624 "user.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 625 "user.qsc" */	/* host code */
		}
/* # line 627 "user.qsc" */	/* getform */
            {
              if (IIfsetio(Usrform) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,(char *)"\
name");
              } /* IIfsetio */
            }
/* # line 629 "user.qsc" */	/* host code */
		changed = CONFIRM(Object_name, ERget(F_IC0019_User));
		switch(changed)
		{
		case CONFCH_YES:
/* # line 633 "user.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 634 "user.qsc" */	/* host code */
		    if (user_write() != OK)
			break;
		    /* fall through */
		case CONFCH_NO:
		    PrivChg=FALSE;
/* # line 639 "user.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 640 "user.qsc" */	/* host code */
		default:
		    break;
		}
/* # line 644 "user.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 645 "user.qsc" */	/* host code */
	    }
	}
	PrivChg=FALSE;
/* # line 648 "user.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 10) {
          {
/* # line 654 "user.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,19,
                (char *)0,0);
              } /* IIiqset */
            }
/* # line 655 "user.qsc" */	/* host code */
	if (STequal(Object_name, Access_tbl) ||
	    STequal(Object_name, Database_tbl) )
	{
	    FEtabfnd(Usrform, Object_name);
	}
	else
	    IIUGerr(E_IC006C_NotOnTableField, UG_ERR_ERROR, 0);
          }
        } else if (IIretval() == 11) {
          {
/* # line 666 "user.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,19,
                (char *)0,0);
              } /* IIiqset */
            }
/* # line 667 "user.qsc" */	/* host code */
	if (STequal(Object_name, Access_tbl) ||
	    STequal(Object_name, Database_tbl) )
	{
/* # line 670 "user.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Usrform,Object_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 671 "user.qsc" */	/* host code */
	}
	else
	    IIUGerr(E_IC006C_NotOnTableField, UG_ERR_ERROR, 0);
          }
        } else if (IIretval() == 12) {
          {
/* # line 678 "user.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,19,
                (char *)0,0);
              } /* IIiqset */
            }
/* # line 679 "user.qsc" */	/* host code */
	if (STequal(Object_name, Access_tbl) ||
	    STequal(Object_name, Database_tbl) )
	{
/* # line 682 "user.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Usrform,Object_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 683 "user.qsc" */	/* host code */
	}
	else
	    IIUGerr(E_IC006C_NotOnTableField, UG_ERR_ERROR, 0);
          }
        } else if (IIretval() == 13) {
          {
/* # line 690 "user.qsc" */	/* host code */
	(VOID)user_password(id);
          }
        } else if (IIretval() == 14) {
          {
/* # line 695 "user.qsc" */	/* host code */
       get_user_stat(&U_status, &U_def_status);
       user_privileges(U_name, &U_status, &U_def_status);
       put_user_stat(U_status, U_def_status);
          }
        } else if (IIretval() == 15) {
          {
            char nm[FE_MAXNAME + 1];
            char tmpname[FE_MAXNAME + 1];
/* # line 707 "user.qsc" */	/* host code */
	if (Listchoices_menuitem[0] == EOS)
	{
	    /* this was selected via the FRSkey */
	    FTbell();
/* # line 711 "user.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 712 "user.qsc" */	/* host code */
	}
/* # line 714 "user.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,19,
                (char *)0,0);
              } /* IIiqset */
            }
/* # line 715 "user.qsc" */	/* host code */
	if (STequal(Object_name, Access_tbl))
	{
/* # line 717 "user.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,34,
                (char *)0,0);
              } /* IIiqset */
            }
/* # line 718 "user.qsc" */	/* host code */
	    if (STequal(Object_name, ERx("dbname")))
	    {
		icListPicks(ERget(FE_Database),
			    ERx("iidatabase"), ERx("name"), nm);
		if (nm[0] != EOS)
		{
/* # line 724 "user.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Usrform,Access_tbl,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,tmpname,(char *)"\
dbname");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 725 "user.qsc" */	/* host code */
		    if (STequal(tmpname, ERx("")))
/* # line 726 "user.qsc" */	/* putrow */
            {
              if (IItbsetio(3,Usrform,Access_tbl,-2) != 0) {
                IItcoputio((char *)"dbname",(short *)0,1,32,0,nm);
                IItcoputio((char *)"access",(short *)0,1,32,0,ERx("yes"));
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 728 "user.qsc" */	/* host code */
		    else
		    {
/* # line 730 "user.qsc" */	/* loadtable */
            {
              if (IItbact(Usrform,Access_tbl,1) != 0) {
                IItcoputio((char *)"dbname",(short *)0,1,32,0,nm);
                IItcoputio((char *)"access",(short *)0,1,32,0,ERx("yes"));
                IITBceColEnd();
              } /* IItbact */
            }
/* # line 732 "user.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Usrform,Access_tbl,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 733 "user.qsc" */	/* host code */
		    }
/* # line 735 "user.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,Usrform,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 736 "user.qsc" */	/* host code */
		}
	    }
	}
	else if (STequal(Object_name, ERx("default_group")))
	{
	    icListPicks(ERget(F_IC001C_GroupType),
			    ERx("iiusergroup"), ERx("groupid"), nm);
	    if (nm[0] != EOS)
	    {
		STcopy (Iiuser.default_group, U_group);
/* # line 746 "user.qsc" */	/* putform */
            {
              if (IIfsetio(Usrform) != 0) {
                IIputfldio((char *)"default_group",(short *)0,1,32,0,nm);
              } /* IIfsetio */
            }
/* # line 747 "user.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,Usrform,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 748 "user.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
/* # line 749 "user.qsc" */	/* host code */
	    }
	}
	else if (STequal(Object_name, ERx("profile_name")))
	{
	    icListPicks(ERget(F_IC0039_ProfileType),
			    ERx("iiprofiles"), ERx("profile_name"), nm);
	    if (nm[0] != EOS)
	    {
		STcopy (Iiuser.profile, U_profile);
/* # line 758 "user.qsc" */	/* putform */
            {
              if (IIfsetio(Usrform) != 0) {
                IIputfldio((char *)"profile_name",(short *)0,1,32,0,nm);
              } /* IIfsetio */
            }
/* # line 759 "user.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,Usrform,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 760 "user.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
/* # line 761 "user.qsc" */	/* host code */
	    }
	}
	else
	{
/* # line 765 "user.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 766 "user.qsc" */	/* host code */
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
      if (IIFRafActFld((char *)"name",0,1) == 0) goto IIfdE1;
      if (IIFRafActFld((char *)"profile_name",1,2) == 0) goto IIfdE1;
      if (IIFRafActFld((char *)"profile_name",0,3) == 0) goto IIfdE1;
      if (IIFRafActFld((char *)"default_group",1,4) == 0) goto IIfdE1;
      if (IIFRafActFld((char *)"default_group",0,5) == 0) goto IIfdE1;
      if (IITBcaClmAct(Access_tbl,(char *)"dbname",0,6) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(Save_menuitem,ERget(F_FE0117_SaveExpl),2,0,7) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,0,7) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,8) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,8) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,9) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,9) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,10) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,11) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,12) == 0) goto IIfdE1;
      if (IInmuact(Password_menuitem,(char *)0,2,2,13) == 0) goto IIfdE1;
      if (IInmuact(Privileges_menuitem,(char *)0,2,2,14) == 0) goto IIfdE1;
      if (IInmuact(Listchoices_menuitem,ERget(FE_LookupExpl),0,0,15) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,15) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 772 "user.qsc" */	/* host code */
    return OK;
  }
/*{
** Name:        user_init       Initialize user form for a user
**
** Description:
**      Initialize user form for a new or existing user specified by id.
**      Performs validity checks and makes form updates.
**
**      Note:  User names are case sensitive!
**
** Inputs:
**      id      - User name
**
** Returns:
**      FAIL    - Validity check failure.
**      OK      - OK
**
**
** Side Effects:
**      Globals Updated:
**              Newuser         New user flag
**              U_name          User name
**              U_group         Default Group name
**              Isdba           Owns databases (valid only if an existing user)
**
** History:
**      18-may-89 (mgw)
**              Added validity checks to prevent name collisions between new
**              user names and groups or roles for TERMINATOR. Also added
**              code to update U_group with the user's default group.
**      11-apr-90 (pete)
**              Change couple QUEL queries that check for existing user name to
**              SQL so any pattern match characters entered by user will
**              be treated as literals (jupbug 20469).
**      11-oct-93 (kellyp)
**              "Database Owned" and "Authorized Database" tables are now
**              invisible for the User/Create frame - took out the
**              Delete menuitem since it no longer makes sense.
**      21-jul-94 (robf)
**              Handle default privileges.
*/
static STATUS
user_init (id)
char    *id;
  {
  char db_access[32];
  char db_name[FE_MAXNAME+1];
    char *tb_mode = Mgr ? ERx("fill") : ERx("read");
    /* Initialize "owns" and "access" table fields */
/* # line 829 "user.qsc" */	/* inittable */
    {
      if (IItbinit(Usrform,Database_tbl,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 830 "user.qsc" */	/* inittable */
    {
      if (IItbinit(Usrform,Access_tbl,tb_mode) != 0) {
        IIthidecol((char *)"dbname_orig",F_Maxname);
        IIthidecol((char *)"access_orig",(char *)"c3");
        IItfill();
      } /* IItbinit */
    }
/* # line 833 "user.qsc" */	/* host code */
    U_name[0] = EOS;
    if (id == NULL)
    {
	    /* Creating a new user. */
/* # line 839 "user.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Usrform,(char *)0, (char *)0) != 0) {
        IIstfsio(17,(char *)"name",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(53,(char *)"name",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
        IIstfsio(54,Database_tbl,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1
        ));
        IIstfsio(54,Access_tbl,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 843 "user.qsc" */	/* host code */
	    Newuser = TRUE;
	    Isdba = FALSE;
	    put_user_stat((i4) U_CREATDB, (i4)U_CREATDB);
	    U_group[0] = EOS;
	    U_status=U_CREATDB;
	    U_def_status=U_status;
	    return OK;
    }
    /* OK, we have an existing user, so fetch the info */
    STlcopy(id, U_name, sizeof(U_name) - 1);
    if (STtrmwhite(U_name) <= 0)
	return FAIL;
    /* just double-check this.  Shouldn't happen, but... */
    if (iiicsuSelectUser(U_name, &Iiuser) != OK)
	return FAIL;
/* # line 862 "user.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Usrform,(char *)0, (char *)0) != 0) {
        IIstfsio(17,(char *)"name",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
        IIstfsio(53,(char *)"name",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(54,Database_tbl,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0
        ));
        IIstfsio(54,Access_tbl,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 866 "user.qsc" */	/* host code */
    Newuser = FALSE;
    Isdba = FALSE;
    put_user_stat(Iiuser.status, Iiuser.default_status);
    U_status=Iiuser.status;
    U_def_status=Iiuser.default_status;
    STcopy (Iiuser.default_group, U_group); /* need this in global.*/
    STcopy (Iiuser.profile, U_profile);
    STcopy (Iiuser.expire_date, U_exp_date);
/* # line 1247 "user.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select database_name from iidatabase_info where database_owner=");
    IIputdomio((short *)0,1,32,0,U_name);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,db_name);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 1252 "user.sc" */	/* host code */
    {
/* # line 881 "user.qsc" */	/* loadtable */
    {
      if (IItbact(Usrform,Database_tbl,1) != 0) {
        IItcoputio((char *)"dbname",(short *)0,1,32,0,db_name);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 882 "user.qsc" */	/* host code */
	    Isdba = TRUE;
    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1264 "user.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select database_name, db_access from iidbprivileges where grantee_nam\
e=");
    IIputdomio((short *)0,1,32,0,U_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and db_access='Y'");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,db_name);
      IIgetdomio((short *)0,1,32,31,db_access);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 1270 "user.sc" */	/* host code */
    {
	STtrmwhite(db_name);
	CVlower(db_access);
	if (db_access[0] == 'y')
	    STcopy(Yes, db_access);
	else if (db_access[0] == 'n')
	    STcopy(No, db_access);
	if (db_name[0] == EOS)
	{
	    /* empty dbname = GRANT ON INSTALLATION TO USER */
	    STcopy (IC_allDBs, db_name);
	}
/* # line 906 "user.qsc" */	/* loadtable */
    {
      if (IItbact(Usrform,Access_tbl,1) != 0) {
        IItcoputio((char *)"dbname",(short *)0,1,32,0,db_name);
        IItcoputio((char *)"dbname_orig",(short *)0,1,32,0,db_name);
        IItcoputio((char *)"access",(short *)0,1,32,0,db_access);
        IItcoputio((char *)"access_orig",(short *)0,1,32,0,db_access);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 910 "user.qsc" */	/* host code */
    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
    return OK;
  }
/*{
** Name:        user_new        
**
** Description:
**
** Inputs:
**      id      - User name
**
** Returns:
**      FAIL    - User name already exists, or clashes with a group name.
**      OK      - User name is OK to create.
*/
static STATUS
user_new (id)
char    *id;
  {
    i4          grpcnt =0;
    STlcopy(id, U_name, sizeof(U_name) - 1);
    if (U_name[0] == EOS)
    {
	/* We know it's bad.  fchkuname will give a nice error msg. */
	(void) fchkuname(U_name);
	return FAIL;
    }
# ifndef UNIX
    IIUGlbo_lowerBEobject(U_name);
# endif
    /* get user info */
    if (iiicsuSelectUser(U_name, &Iiuser) != OK || FEinqrows() > 0)
    {
	/* user already exists. */
	IIUGerr(E_IC004E_A_user_already_exists, UG_ERR_ERROR, 0);
	return FAIL;
    }
    /* Check that new user name doesn't collide with an existing
    ** group name (user & group names share same name space)
    */
    if (IIUIdcg_group())
    {
	grpcnt = iiicueUsergroupExists(U_name);
	if (FEinqerr() != OK)
	    return FAIL;
	else if (grpcnt > 0)
	{
	    IIUGerr(E_IC0024_A_grp_already_exists, UG_ERR_ERROR, 0);
	    return FAIL;
	}
    }
    /* Check that new user name doesn't collide with an existing
    ** role name (user, group & role names share same name space)
    */
    if (IIUIdco_role())
    {
	if (iiicsrSelectRole(U_name, &Iirole) != OK)
	    return FAIL;
	if (FEinqrows() > 0)
	{
	    IIUGerr(E_IC002F_A_role_already_exists, 0, 0);
	    return FAIL;
	}
    }
    /*
    ** Check that new user name doesn't collide with an existing
    ** profile name (user & profiles share the same name space )
    */
    if(profile_exist_chk(U_name)==OK)
    {
	     IIUGerr(E_IC0071_A_profile_exists, UG_ERR_ERROR, 0);
	     return FAIL;
    }
    return OK;
  }
/*{
** Name:        user_write              Write user form back to ingres
**
** Description:
**      Perform either a user create or a user update.
**      The dbdb and the users file are updated
**
**      Note:  User names are case sensitive!
**
** Returns:
**      OK              ok
**      FAIL            error of some sort ( bad database table, etc ..)
**
** History:
**      18-may-89 (mgw)
**              Added default group handling for TERMINATOR.
**      20-jul-94 (robf)
**              Trim white space from expire date & group.
**      12-dec-94 (harpa06)
**              Integrated bug fix #50334 by nick:
**              Don't allow the removal of super user permissions from
**              $ingres or ingres.
*/
static STATUS
user_write()
  {
    char u_name[((2 * FE_MAXNAME) + 2 + 1)];
    char u_group[((2 * FE_MAXNAME) + 2 + 1)];
    char u_profile[((2 * FE_MAXNAME) + 2 + 1)];
    char u_exp_date[26];
    i4     u_audit=0;
    STATUS      stat;
/* # line 1031 "user.qsc" */	/* getform */
    {
      if (IIfsetio(Usrform) != 0) {
        IIgetfldio((short *)0,1,32,((2 * FE_MAXNAME) + 2 + 1)-1,u_name,
        (char *)"name");
        IIgetfldio((short *)0,1,32,((2 * FE_MAXNAME) + 2 + 1)-1,u_group,
        (char *)"default_group");
        IIgetfldio((short *)0,1,32,((2 * FE_MAXNAME) + 2 + 1)-1,u_profile,
        (char *)"profile_name");
        IIgetfldio((short *)0,1,32,25,u_exp_date,(char *)"expire_date");
      } /* IIfsetio */
    }
/* # line 1035 "user.qsc" */	/* host code */
    if (usr_chg_chk())
	return FAIL;
    if (usr_mod_chk())
	return FAIL;
    STtrmwhite(u_profile);
    if(u_profile[0]!=EOS && profile_exist_chk(u_profile)!=OK)
    {
	/*
	** Couldn't find the profile
	*/
	IIUGerr(E_IC0070_Profile_not_found, 0, 0);
	STcopy(ERx("profile_name"), Resume_fld);
	return FAIL;
    }
    STtrmwhite(u_exp_date);
    STtrmwhite(u_group);
    get_user_stat(&U_status, &U_def_status);
    get_audit_stat(&u_audit);
    if (Newuser)
	stat = user_create(u_name, U_status, u_group, u_audit, 
				U_def_status, u_profile,u_exp_date);
    else
	stat = user_modify(u_name, U_status, u_group, u_audit,
				U_def_status, u_profile, u_exp_date);
    return stat;
  }
/*{
** Name:        USER_CREATE             Create an INGRES user
**
** Description:
**      Put user in dbdb, and setup the list of databases he may access
**
** Inputs:
**      x_name          - user name
**      x_status        - database access privileges
**      x_group         - default group
**      x_audit         - audit flags
**      x_defpriv       - default privileges
**      x_profile       - profile
**      x_exp_date      - expire date
**
** Returns:
**      OK      ok
**      FAIL    error - existing user, etc..
**
** History:
**      18-may-89 (mgw)
**              Added handling for Default Groups for TERMINATOR.
**      2-nov-1990 (pete)
**              Removed replicated code to build CREATE USER stmt & created
**              array "Priv" declared above.
**      20-sep-93 (robf)
**              Added profile, audit, expire_date info
**      20-jul-94 (robf)
**              In confirmation popup, only specify group if there is
**              one and default to positive response. (following suggestion)
**              Add support for default privileges.
**      30-dec-1994 (andyw)
**              Problems found creating users in HP platforms
**              where we get a syntax error on create user
**              appears to be an uninitialized variable in user_create()
**              moved (needgroup) to after privcnt check
**      12-jul-96 (sarjo01)
**              Bug 77705: increase size of promptMsg[] buffer to 80. 
**              STprintf() was overrunning this buffer, corrupting other
**              stack vars. This caused a memory access violation on NT.
**      07-Jan-99 (hanal04)
**              If user (of accessdb) does not have maintain_audit priv do 
**              not add SECURITY_AUDIT options to the CREATE USER query.
**              b81618.
*/
static STATUS
  user_create(x_name, x_status, x_group, x_audit, x_defpriv, 
		x_profile, x_exp_date)
  char *x_name;
  i4 x_status;
  char *x_group;
  i4 x_audit;
  i4 x_defpriv;
  char *x_profile;
  char *x_exp_date;
  {
    char db_name[FE_MAXNAME+1];
    i4 db_state;
    i4 changed;
    STATUS      uval;
    bool        addit = FALSE;
    char        tmpbuf[256];
    char        audbuf[256];
    char        defprivbuf[256];
    char access[3 +1];
    char promptMsg[160];
    char answer[1];
    i4          privcnt = 0;
    i4          audcnt=0;
    i4          defprivcnt=0;
    bool        needgroup = FALSE;
    char        i=0;
    bool        add_comma=FALSE;
  char sbuffer[1024];
/* # line 1148 "user.qsc" */	/* message */
    {
      IImessage(ERget(S_IC004D_Working_dot_dot_dot));
    }
/* # line 1150 "user.qsc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,Usrform,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&changed,20,(char *)"name",0);
      } /* IIiqset */
    }
/* # line 1152 "user.qsc" */	/* host code */
    if (changed)
    {
	/* If the name field has been activated, this may already have
	 * been done. However, it's been changed so reformat it for redisplay
	 * and CREATE.
	 */
	if (IIUGdlm_ChkdlmBEobject( x_name, x_name, FALSE ) == UI_BOGUS_ID)
	{
	    i4       Maxn = DB_MAXNAME;
	    IIUGerr(E_IC002D_bad_username, 0, 2, x_name, (PTR)&Maxn);
	    return FAIL;
	}
/* # line 1164 "user.qsc" */	/* putform */
    {
      if (IIfsetio(Usrform) != 0) {
        IIputfldio((char *)"name",(short *)0,1,32,0,x_name);
      } /* IIfsetio */
    }
/* # line 1165 "user.qsc" */	/* host code */
    }
    /*
    **  Only print group name if specified, makes prompt cleaner
    */
    if(x_group[0]!='\0')
    {
	STprintf( promptMsg,
	    "Creating Ingres user '%s' in group '%s'. Is this correct (y|n)? ",
	    x_name, x_group );
    }
    else
    {
	STprintf( promptMsg,
	    "Creating Ingres user '%s'. Is this correct (y|n)? ",
	    x_name);
    }
/* # line 1184 "user.qsc" */	/* prompt */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      IIprmptio(0,promptMsg,(short *)0,1,32,0,answer);
    }
/* # line 1186 "user.qsc" */	/* host code */
    if (answer[0] == 'n' || answer[0] == 'N')
    {
	return FAIL;
    }
    else if ( answer[0] != 'Y' && answer[0] != 'y' )
    {
       IIUGerr(E_IC0130_Yes_or_No, 0, 0);
       return FAIL;
    } 
    /*
    ** We should handle delimited ids here!
    */
    /* Check that no user with same name already exists */
    if (iiicsuSelectUser(x_name, &Iiuser) != OK)
	return FAIL;
    else if (FEinqrows() > 0)
    {
	IIUGerr(E_IC004E_A_user_already_exists, 0, 0);
	return FAIL;
    }
    /* Make sure group exists and x_name is a member of that group */
    if (x_group != NULL && *x_group != EOS)
    {
	if (grp_exist_chk(x_group, x_name, &addit) != OK)
	{
		STcopy(ERx("default_group"), Resume_fld);
		return FAIL;
	}
    }
    /* build list of PRIVILEGES  & AUDITS*/
    privcnt = icpriv_string(x_status, tmpbuf);
    audcnt = icaudit_string(x_audit, audbuf);
    defprivcnt = icpriv_string(x_defpriv, defprivbuf);
    if (IIUIdcg_group() && (x_group != NULL) && (*x_group != EOS))
	needgroup = TRUE;
    else
	needgroup = FALSE;
    STprintf (sbuffer, ERx("CREATE USER %s"), icdelimit(x_name));
    /*
    ** b81618 - If user does not have maintain_audit priv do not attempt
    ** to change the auditing system.
    */
    if (needgroup || privcnt > 0 || (audcnt>0 && IiicPrivMntAudit)
	|| x_exp_date[0]!=EOS
	|| x_profile[0]!=EOS)
    {
	STcat (sbuffer, ERx(" WITH"));  /* we'll need a WITH clause */
	if (privcnt > 0)
	{
	    STcat (sbuffer, ERx(" PRIVILEGES = ("));
	    STcat (sbuffer, tmpbuf);
	    STcat (sbuffer, Rparen);
	    add_comma=TRUE;
	}
	if (needgroup)
	{
	    if(add_comma)
		STcat(sbuffer,",");
	    STcat (sbuffer, ERx(" GROUP = "));
	    STcat (sbuffer, icdelimit(x_group));
	    add_comma=TRUE;
	}
	if (defprivcnt>0)
	{
	    if(add_comma)
		STcat(sbuffer,",");
	    STcat (sbuffer, ERx(" DEFAULT_PRIVILEGES = ("));
	    STcat (sbuffer, defprivbuf);
	    STcat (sbuffer, Rparen);
	    add_comma=TRUE;
	}
	else
	{
	    if(add_comma)
		STcat(sbuffer,",");
	    STcat (sbuffer, ERx(" NODEFAULT_PRIVILEGES"));
	    add_comma=TRUE;
	}
        /* b81618 */
	if (audcnt>0 && IiicPrivMntAudit)
	{
	    if(add_comma)
	        STcat(sbuffer,",");
	    STcat (sbuffer, ERx(" SECURITY_AUDIT = ("));
	    STcat (sbuffer, audbuf);
	    STcat (sbuffer, Rparen);
	    add_comma=TRUE;
        }
	if (x_profile[0]!=EOS)
	{
	    if(add_comma)
		STcat(sbuffer,",");
	    STcat (sbuffer, ERx(" PROFILE = "));
	    STcat (sbuffer, icdelimit(x_profile));
	    add_comma=TRUE;
	}
	if (x_exp_date[0]!=EOS)
	{
	    if(add_comma)
		STcat(sbuffer,",");
	    STcat (sbuffer, ERx(" EXPIRE_DATE = '"));
	    STcat (sbuffer, x_exp_date);
	    STcat (sbuffer, "'");
	    add_comma=TRUE;
	}
    }
/* # line 1303 "user.qsc" */	/* message */
    {
      IImessage(ERget(S_IC0023_Updating_database));
    }
/* # line 1305 "user.qsc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1307 "user.qsc" */	/* host code */
/* # line 1686 "user.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1687 "user.sc" */	/* host code */
    if (sqlca.sqlcode < 0 && sqlca.sqlcode != (-E_GE0032_WARNING))
    {
/* # line 1689 "user.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1690 "user.sc" */	/* host code */
	IIUGerr(E_IC004F_Cannot_add_user, 0, 1, x_name);
	STcopy (ERx("name"), Resume_fld);       /* resume here*/
	return FAIL;
    }
    /* Write list of authorized databases */
    uval = OK;
/* # line 1319 "user.qsc" */	/* unloadtable */
    {
      if (IItbact(Usrform,Access_tbl,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,db_name,(char *)"dbn\
ame");
        IItcogetio((short *)0,1,32,3 +1-1,access,(char *)"access");
        IItcogetio((short *)0,1,30,4,&db_state,(char *)"_STATE");
        IITBceColEnd();
        {
/* # line 1322 "user.qsc" */	/* host code */
	if (db_state == stNEW)
	{
	    if (db_name[0] != EOS)
	    {
		if (STbcompare(IC_allDBs, 0, db_name, 0, TRUE) != 0)
		{
		    STprintf(sbuffer,
		    ERx("GRANT %sACCESS ON DATABASE %s TO USER %s"),
			(access[0] == Yes[0]) ? Empty : ERx("NO"),
			db_name, icdelimit(x_name));
		}
		else
		{
		    /* Issue GRANT ON INSTALLATION */
		    STprintf(sbuffer,
			ERx("GRANT %sACCESS ON INSTALLATION TO USER %s"),
			(access[0] == Yes[0]) ? Empty : ERx("NO"),
			icdelimit(x_name));
		}
/* # line 1726 "user.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1727 "user.sc" */	/* host code */
		if ( FEinqerr() != OK && sqlca.sqlcode != (-E_GE0032_WARNING))
		{
		    uval = FAIL;
/* # line 1346 "user.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 1347 "user.qsc" */	/* host code */
		}
	    }
	}
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 1352 "user.qsc" */	/* host code */
    if (uval == OK)
    {
/* # line 1746 "user.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1747 "user.sc" */	/* host code */
	Newuser = TRUE;
    }
    else
    {
/* # line 1751 "user.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1752 "user.sc" */	/* host code */
    }
    return uval;
  }
/*{
** Name:        USER_MODIFY             Modify an INGRES user
**
** Description:
**      Replace user information and incrementally update user access list
**
** Inputs:
**      x_name          - user name
**      x_gid           - group id (probably defunct)
**      x_mid           - 
**      x_status        - database access privileges
**      x_group         - default group
**      x_audit         - audit flags
**      x_profile       - profile
**      x_exp_date      - expire date
**
** Returns:
**      OK      ok
**      FAIL    error - illegal change
** History:
**      18-may-89 (mgw)
**              Added Default Group handling for TERMINATOR.
**      2-nov-1990 (pete)
**              Removed replicated code to build ALTER USER stmt & created
**              array "Priv" declared above.
**      15-nov-1991 (pete)
**              If user updates own user privilege info, then call icSetGlobals
**              again to reset cached user priv info.
**      07-nov-1995 (canor01)
**              If there are no new databases added to auth list, only deletes,
**              don't do a GRANT ACCESS.
**      07-Jan-99 (hanal04)
**              If user (of accessdb) does not have maintain_audit priv do 
**              not add SECURITY_AUDIT or NOSECURITY_AUDIT options to the 
**              ALTER USER query. b81618.
**      27-Apr-2004 (hanal04) Bug 111896 INGCBT513
**              NOPROFILE becomes default profile. This causes E_US1968 in
**              psyuser.c if NOPRIVILEGES is set because NODEFAULT_PRIVILEGES 
**              has not been used to block the NOPRIVILEGES from acquiring 
**              DEFAULT_PRIVILEGES from the default profile.
*/
static STATUS
user_modify(x_name, x_status, x_group, x_audit, x_defpriv, 
		x_profile, x_exp_date)
  char *x_name;
  i4 x_status;
  char *x_group;
  i4 x_audit;
  i4 x_defpriv;
  char *x_profile;
  char *x_exp_date;
  {
    char db_name[FE_MAXNAME+1];
    char dbname_orig[FE_MAXNAME+1];
    char access[3 +1];
    char access_orig[3 +1];
    i4 db_state;
	bool    addit = FALSE;
	char    tmpbuf[256];
	char    audbuf[256];
	char    defprivbuf[256];
	i4     privcnt = 0;
	i4     defprivcnt = 0;
	bool    needgroup=FALSE;
	i4     audcnt = 0;
	i4     i=0;
	STATUS  uval = OK;
  char sbuffer[1024];
    /* Make sure group exists and x_name is a member of that group */
    if (x_group != NULL && *x_group != EOS)
    {
	if (grp_exist_chk(x_group, x_name, &addit) != OK)
	{
		STcopy(ERx("default_group"), Resume_fld);
		return FAIL;
	}
	needgroup=TRUE;
    }
    /* build list of PRIVILEGES  & AUDITS*/
    privcnt = icpriv_string(x_status, tmpbuf);
    audcnt = icaudit_string(x_audit, audbuf);
    defprivcnt = icpriv_string(x_defpriv, defprivbuf);
    STprintf (sbuffer, ERx("ALTER USER %s"), icdelimit(x_name));
    STcat (sbuffer, ERx(" WITH"));      /* we'll need a WITH clause */
    if (needgroup)
    {
	STcat (sbuffer, ERx(" GROUP = "));
	STcat (sbuffer, icdelimit(x_group));
    }
    else
	STcat (sbuffer, ERx(" NOGROUP "));
    if (privcnt > 0)
    {
	STcat (sbuffer, ERx(",PRIVILEGES = ("));
	STcat (sbuffer, tmpbuf);
	STcat (sbuffer, Rparen);
        if (defprivcnt>0)
        {
            STcat (sbuffer, ERx(",DEFAULT_PRIVILEGES = ("));
            STcat (sbuffer, defprivbuf);
            STcat (sbuffer, Rparen);
        }
        else
        {
            STcat (sbuffer, ERx(",NODEFAULT_PRIVILEGES"));
        }
    }
    else
    {
	STcat (sbuffer,ERx(",NOPRIVILEGES"));
        STcat (sbuffer, ERx(",NODEFAULT_PRIVILEGES"));
    }
    /*
    ** b81618 - If user does not have maintain_audit priv do not attempt
    ** to change the auditing system.
    */
    if(IiicPrivMntAudit)
    {
       if (audcnt>0)
       {
	   STcat (sbuffer, ERx(",SECURITY_AUDIT = ("));
	   STcat (sbuffer, audbuf);
	   STcat (sbuffer, Rparen);
       }
       else
	   STcat (sbuffer,ERx(",NOSECURITY_AUDIT"));
    }
    if (x_profile[0]!=EOS)
    {
	STcat (sbuffer, ERx(",PROFILE = "));
	STcat (sbuffer, icdelimit(x_profile));
    }
    else
	STcat (sbuffer,ERx(",NOPROFILE"));
    if (x_exp_date[0]!=EOS)
    {
	STcat(sbuffer,",");
	STcat (sbuffer, ERx(" EXPIRE_DATE = '"));
	STcat (sbuffer, x_exp_date);
	STcat (sbuffer, "'");
    }
    else
	STcat (sbuffer,ERx(",NOEXPIRE_DATE"));
/* # line 1523 "user.qsc" */	/* message */
    {
      IImessage(ERget(S_IC0023_Updating_database));
    }
/* # line 1525 "user.qsc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1527 "user.qsc" */	/* host code */
/* # line 1909 "user.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1910 "user.sc" */	/* host code */
    if (sqlca.sqlcode < 0 && sqlca.sqlcode != (-E_GE0032_WARNING) )
    {
/* # line 1912 "user.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1913 "user.sc" */	/* host code */
	return FAIL;
    }
    /* process stDELETE rows */
/* # line 1536 "user.qsc" */	/* unloadtable */
    {
      if (IItbact(Usrform,Access_tbl,0) == 0) goto IItbE2;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,db_name,(char *)"dbn\
ame");
        IItcogetio((short *)0,1,32,3 +1-1,access,(char *)"access");
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,dbname_orig,(char *)
"dbname_orig");
        IItcogetio((short *)0,1,32,3 +1-1,access_orig,(char *)"access_\
orig");
        IItcogetio((short *)0,1,30,4,&db_state,(char *)"_STATE");
        IITBceColEnd();
        {
/* # line 1541 "user.qsc" */	/* host code */
	sbuffer[0] = EOS;
	switch (db_state)
	{
	case stDELETE:
	    /* Delete the Access, with the original database name */
	    if (STbcompare(IC_allDBs, 0, dbname_orig, 0, TRUE) != 0)
	    {
		/* don't try to REVOKE if empty database name */
		STprintf(sbuffer,
			ERx("REVOKE %sACCESS ON DATABASE %s FROM USER %s"),
			(access_orig[0] == Yes[0]) ? Empty : ERx("NO"),
			dbname_orig, icdelimit(x_name));
	    }
	    else
	    {
		/* REVOKE an INSTALLATION access */
		STprintf(sbuffer,
			ERx("REVOKE %sACCESS ON INSTALLATION FROM USER %s"),
			(access_orig[0] == Yes[0]) ? Empty : ERx("NO"),
			icdelimit(x_name));
	    }
/* # line 1952 "user.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1953 "user.sc" */	/* host code */
	    if ( FEinqerr() != OK && sqlca.sqlcode != (-E_GE0032_WARNING))
	    {
		uval = FAIL;
/* # line 1568 "user.qsc" */	/* endloop */
          {
            if (1) goto IItbE2;
          }
/* # line 1569 "user.qsc" */	/* host code */
	    }
	    break;
	}
        }
      } /* IItunload */
IItbE2:
      IItunend();
    }
/* # line 1574 "user.qsc" */	/* host code */
    if (uval == FAIL)
	goto done;
    /* process all but stDELETE statements */
/* # line 1578 "user.qsc" */	/* unloadtable */
    {
      if (IItbact(Usrform,Access_tbl,0) == 0) goto IItbE3;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,db_name,(char *)"dbn\
ame");
        IItcogetio((short *)0,1,32,3 +1-1,access,(char *)"access");
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,dbname_orig,(char *)
"dbname_orig");
        IItcogetio((short *)0,1,32,3 +1-1,access_orig,(char *)"access_\
orig");
        IItcogetio((short *)0,1,30,4,&db_state,(char *)"_STATE");
        IITBceColEnd();
        {
/* # line 1583 "user.qsc" */	/* host code */
	sbuffer[0] = EOS;
	switch (db_state)
	{
	case stCHANGE:
	    /* Delete old access, and add new one */
	    if (STbcompare(IC_allDBs, 0, dbname_orig, 0, TRUE) != 0)
	    {
		STprintf(sbuffer,
		  ERx("REVOKE %sACCESS ON DATABASE %s FROM USER %s"),
			(access_orig[0] == Yes[0]) ? Empty : ERx("NO"),
		  dbname_orig, icdelimit(x_name));
	    }
	    else
	    {
		/* REVOKE an INSTALLATION access */
		STprintf(sbuffer,
			ERx("REVOKE %sACCESS ON INSTALLATION FROM USER %s"),
			(access_orig[0] == Yes[0]) ? Empty : ERx("NO"),
			icdelimit(x_name));
	    }
/* # line 2008 "user.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 2009 "user.sc" */	/* host code */
	    if ( FEinqerr() != OK && sqlca.sqlcode != (-E_GE0032_WARNING))
	    {
		    uval = FAIL;
/* # line 1611 "user.qsc" */	/* endloop */
          {
            if (1) goto IItbE3;
          }
/* # line 1612 "user.qsc" */	/* host code */
	    }
	    /* fall through */
	case stNEW:
	    /* if new dbname, do the GRANT */
	    if (db_name[0] != EOS)
	    {
		/* Add new user to database */
		if (STbcompare(IC_allDBs, 0, db_name, 0, TRUE) != 0)
		{
		    STprintf(sbuffer, 
			ERx("GRANT %sACCESS ON DATABASE %s TO USER %s"),
			(access[0] == Yes[0]) ? Empty : ERx("NO"),
			db_name, icdelimit(x_name));
		}
		else
		{
			/* Issue GRANT ON INSTALLATION */
		    STprintf(sbuffer,
			ERx("GRANT %sACCESS ON INSTALLATION TO USER %s"),
			(access[0] == Yes[0]) ? Empty : ERx("NO"),
			icdelimit(x_name));
		}
/* # line 2039 "user.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 2040 "user.sc" */	/* host code */
		if ( FEinqerr() != OK && sqlca.sqlcode != (-E_GE0032_WARNING))
		{
		    uval = FAIL;
/* # line 1641 "user.qsc" */	/* endloop */
          {
            if (1) goto IItbE3;
          }
/* # line 1642 "user.qsc" */	/* host code */
		}
	    }
	    break;
	default:
		break;
	}
        }
      } /* IItunload */
IItbE3:
      IItunend();
    }
/* # line 1653 "user.qsc" */	/* host code */
done:
    if (uval == OK)
    {
/* # line 2063 "user.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 2064 "user.sc" */	/* host code */
	/* If user just updated their own privileges, then reset
	** our cached user-priv info.
	*/
	if (STequal(Real_username, x_name) == 0)
	    _VOID_ icSetGlobals(Real_username, &Iiuser);
    }
    else
    {
/* # line 2072 "user.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 2073 "user.sc" */	/* host code */
    }
    return uval;
  }
/*{
** Name:        usr_chg_chk()   - Disallow updates to the $ingres user
**
** Returns:
**      FALSE   - OK - user isn't trying to update the $ingres account
**      TRUE    - ERROR - user is trying to update the $ingres account
**
** History:
*/
static bool
usr_chg_chk()
{
    bool        err = FALSE;
    if (!Newuser && STequal(U_name, ERx("$ingres")))
  {
    char u_name[FE_MAXNAME+1];
/* # line 1691 "user.qsc" */	/* getform */
    {
      if (IIfsetio(Usrform) != 0) {
        IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,u_name,(char *)"name");
      } /* IIfsetio */
    }
/* # line 1692 "user.qsc" */	/* host code */
	if (STtrmwhite(u_name) > 0 && STequal(u_name, ERx("$ingres")))
	{
	    IIUGerr(E_IC0051_cant_change_ingres, 0, 0);
/* # line 1695 "user.qsc" */	/* putform */
    {
      if (IIfsetio(Usrform) != 0) {
        IIputfldio((char *)"name",(short *)0,1,32,0,U_name);
      } /* IIfsetio */
    }
/* # line 1696 "user.qsc" */	/* host code */
	    err = TRUE;
	}
  }
    return err;
}
/*{
** Name:        grp_exist_chk()         - Insure selected default group exists
**
** Description:
**      Checks to make sure group exists in preparation for setting a default
**      group
**
** Inputs:
**      x_grp   - selected default group
**      x_user  - user name
**
** Outputs:
**      addit   - TRUE if x_user is not a member of selected default group,
**                but user wants to add x_user to that group 
**
** Returns:
**      FAIL    - selected default group doesn't exist or it does, but user
**                doesn't want to add it to that group or some database
**                interaction failed
**      OK      - group exists or will be added
**
** Side Effects:
**      sets default group field back to what it was on entry into the user
**      frame if failure occurs
**
** History:
**      18-may-89 (mgw)
**              Written - for TERMINATOR.
**      6-nov-90 (pete)
**              Replaced in-line RETRIEVEs with calls to functions.
*/
static STATUS
grp_exist_chk(x_grp, x_user, addit)
char    *x_grp;
char    *x_user;
bool            *addit;
  {
    i4          grpcnt = 0;
# ifndef UNIX
    IIUGlbo_lowerBEobject(x_grp);
# endif
    if (iiicsgSelectUsergroup(x_grp, x_user, &Iiusergroup) != OK)
	return FAIL;
    else if (FEinqrows() > 0)
	return OK;
    else
    {
	grpcnt = iiicueUsergroupExists(x_grp);
	if (FEinqerr() != OK)
	    return FAIL;
	else if (grpcnt <= 0)
	{
	    /* Error: group does not exist */    
	    IIUGerr(E_IC0063_No_Such_Group, 0, 1, x_grp);
/* # line 1759 "user.qsc" */	/* putform */
    {
      if (IIfsetio(Usrform) != 0) {
        IIputfldio((char *)"default_group",(short *)0,1,32,0,U_group);
      } /* IIfsetio */
    }
/* # line 1760 "user.qsc" */	/* host code */
	    return FAIL;
	}
    }
    *addit = TRUE;
    return OK;
  }
/*
** Name: profile_exist_chk
**
** Description: Check if a profile exists
**
** Inputs:
**      Profile name
**
** Returns:
**      OK      - Exists
**      FAIL    - Doesn't
**
** History:
**      20-sep-93 (robf)
*/
static STATUS
profile_exist_chk (x_profile)
char *x_profile;
{
  i4 nrows;
  char *prof_name;
	prof_name=x_profile;
/* # line 2205 "user.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"user1",7133,8636);
      IIputdomio((short *)0,1,32,0,prof_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"user1",7133,8636);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiprofiles where profile_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,prof_name);
        IIexDefine(0,(char *)"user1",7133,8636);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&nrows);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 2209 "user.sc" */	/* host code */
	if (FEinqerr() != OK)
	    return FAIL;
	if(nrows<1)
		return FAIL;
	else
		return OK;
}
/* required ingres usernames. these must be valid and can't be deleted */
static char     *Req_users[] = { 
	ERx("$ingres"),
	NULL
	};
/*{
** Name:        IIICiruIsReqUser - Is required username.
**
** Description:
**      Check if user is a required user (and, for example, should not
**      be deleted).
**
** Inputs:
**      char    *user           username to check.
**
** Outputs:
**
**      Returns:
**              Return TRUE if username argument is a required ingres
**              user; FALSE otherwise.
**
** Side Effects:
**
** History:
**      2-apr-1991 (pete)
**              Initial version.
*/
bool
IIICiruIsReqUser(user)
char    *user;
{
	i4 i;
	for (i=0; Req_users[i] != NULL; i++)
	{
	    if (STbcompare(user, 0, Req_users[i], 0, TRUE) == 0)
		return (TRUE);
	}
	if (STbcompare(user, 0, SystemAdminUser, 0, TRUE) == 0)
	    return (TRUE);
	return (FALSE);
}
/*{
** Name:        icdelimit - Delimit an identifier if necessary
**
** Description:
**      Check if a delimiter requires delimiting, and fix it up if needed.
**
** Inputs:
**      char    *id             identifier to check.
**
** Outputs:
**
**      Returns:
**              A pointer to the input identifier if it was unchanged, or
**              to the delimited version if it was changed.
**
*/
char *
icdelimit(id)
char    *id;
{
    char idbuf[((2 * FE_MAXNAME) + 2 + 1)];
    if (STtrmwhite(id) == 0 || !IIUGdlm_ChkdlmBEobject(id, NULL, TRUE))
    {
	/*
	** This is wrong, but it's the only way to handle empty group
	** names.  An empty group name is actually a legitimate 
	** name consisting of a single blank, requiring delimitation.
	** However, that breaks everything, since in INGRES an empty
	** group name means no group.
	*/
	return id;
    }
    IIUGrqd_Requote_dlm(id, idbuf);
    return STalloc(idbuf);
}
/*
** Name:        PRIVS - CREATE/ALTER USER privileges structure.
*/
typedef struct{
	i4             id;     /* symbolic name of privilege */
	char            *name;  /* official INGRES/SQL name of privilege */
} PRIV;
static PRIV Privs[] =
{
	{U_CREATDB,             ERx("CREATEDB")},
	{U_TRACE,               ERx("TRACE")},
	{U_SECURITY,            ERx("SECURITY")},
	{U_SYSADMIN,            ERx("MAINTAIN_LOCATIONS")},
	{U_OPERATOR,            ERx("OPERATOR")},
	{U_AUDITOR,             ERx("AUDITOR")},
	{U_ALTER_AUDIT,         ERx("MAINTAIN_AUDIT")},
	{U_MAINTAIN_USER,       ERx("MAINTAIN_USERS")},
	{0,     NULL}
};
static PRIV Audit[] =
{
	{U_AUDIT,               ERx("ALL_EVENTS")},
	{U_AUDIT_QRYTEXT,       ERx("QUERY_TEXT")},
	{0,     NULL}
};
i4
icpriv_string(statmask, buf)
i4 statmask;
char    *buf;
{
    i4  i;
    i4  count = 0;
    buf[0] = EOS;
    for (i = 0; Privs[i].id != 0 ; i++)
    {
	if (statmask & Privs[i].id)
	{
	    if (count > 0)
		STcat(buf, Comma);      /* don't do this for first one */
	    STcat(buf, Privs[i].name);
	    count++;
	}
    }
    return (count);
}
i4
icaudit_string(statmask, buf)
i4 statmask;
char    *buf;
{
    i4  i;
    i4  count = 0;
    buf[0] = EOS;
    for (i = 0; Audit[i].id != 0 ; i++)
    {
	if (statmask & Audit[i].id)
	{
	    if (count > 0)
		STcat(buf, Comma);      /* don't do this for first one */
	    STcat(buf, Audit[i].name);
	    count++;
	}
    }
    return (count);
}
/*
** Name: user_password - update user password
**
** Description:
**      Change user password.
**
** Inputs:
**      user_name - name of user being changed
**
** History:
**      20-jul-94 (robf)
**         Created
**	01-jun-98 (kitch01)
**		Bug 74731. Disallow ability to set external password to yes if there
**		is no external password verification utility setup for this user.
**		New flag 'extvalid' set for this.
**		Bug 77451. If there is no user password set for this user then set the 
**		user password field to 'no'. New flag 'ingpwd' set for this.
**		New function check_pwd_enabled will check if this user has an ingres
**		password and/or has external password authentication enabled.
*/
STATUS
user_password (char *user_name)
  {
    char *uname;
    char pwdyn[10];
    char pwdextern[10];
    char pwdstr[25];
	bool extvalid;
	bool ingpwd;
    uname=user_name;
	check_pwd_enabled(user_name, &extvalid, &ingpwd);
	if (ingpwd)
       STcopy(ERx("yes"),pwdyn);
	else
	   STcopy(ERx("no"),pwdyn);
    if(Iiuser.internal_flags & USR_EXT_PWD_FLAG)
	{
	   STcopy(ERx("yes"),pwdextern);
	   STcopy(ERx("yes"),pwdyn);
	}
    else        
	STcopy(ERx("no"), pwdextern);
    Title = ERget(S_IC0151_User_pwd_title);
    if(!Mgr)
    {
	if(Iiuser.internal_flags & USR_EXT_PWD_FLAG)
	{
	    /*
	    ** External authentication, thus can't change anything
	    */
	    Mode=ERx("read");
/* # line 2024 "user.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Pwdform,(char *)0, (char *)0) != 0) {
        IIstfsio(54,(char *)"pwd_extern",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
        IIstfsio(54,(char *)"pwd_yn",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 2026 "user.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Pwdform,(char *)0, (char *)0) != 0) {
        IIstfsio(54,(char *)"pwd_str",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(54,(char *)"pwd_str2",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 2028 "user.qsc" */	/* host code */
	}
	else
	{
	    Mode=ERx("update"); 
/* # line 2032 "user.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Pwdform,(char *)0, (char *)0) != 0) {
        IIstfsio(54,(char *)"pwd_extern",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(54,(char *)"pwd_yn",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 2034 "user.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Pwdform,(char *)0, (char *)0) != 0) {
        IIstfsio(54,(char *)"pwd_str",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
        IIstfsio(54,(char *)"pwd_str2",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 2036 "user.qsc" */	/* host code */
	}
    }
    else
    {
	Mode=ERx("update");
	if (extvalid)
	{
		if(*pwdyn=='y')
		{
/* # line 2045 "user.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Pwdform,(char *)0, (char *)0) != 0) {
        IIstfsio(54,(char *)"pwd_extern",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
        IIstfsio(54,(char *)"pwd_str",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 2047 "user.qsc" */	/* host code */
		}
		else
		{
/* # line 2050 "user.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Pwdform,(char *)0, (char *)0) != 0) {
        IIstfsio(54,(char *)"pwd_extern",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(54,(char *)"pwd_str",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 2052 "user.qsc" */	/* host code */
		}
	}
	else
	{
/* # line 2056 "user.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Pwdform,(char *)0, (char *)0) != 0) {
        IIstfsio(54,(char *)"pwd_extern",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(54,(char *)"pwd_yn",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 2058 "user.qsc" */	/* host code */
	}
/* # line 2059 "user.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Pwdform,(char *)0, (char *)0) != 0) {
        IIstfsio(54,(char *)"pwd_str",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(54,(char *)"pwd_str2",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 2061 "user.qsc" */	/* host code */
    }
/* # line 2063 "user.qsc" */	/* display */
    {
      if (IIdispfrm(Pwdform,Mode) == 0) goto IIfdE2;
      goto IImuI2;
IIfdI2:;
      if (IIfsetio((char *)0) == 0) goto IIfdE2;
      IIputfldio((char *)"name",(short *)0,1,32,0,uname);
      IIputfldio((char *)"pwd_yn",(short *)0,1,32,0,pwdyn);
      IIputfldio((char *)"pwd_extern",(short *)0,1,32,0,pwdextern);
      IIputfldio((char *)"title",(short *)0,1,32,0,Title);
IIfdB2:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 2068 "user.qsc" */	/* getform */
            {
              if (IIfsetio(Pwdform) != 0) {
                IIgetfldio((short *)0,1,32,9,pwdyn,(char *)"pwd_yn");
              } /* IIfsetio */
            }
/* # line 2069 "user.qsc" */	/* host code */
	if(*pwdyn=='y')
	{
		if (extvalid)
		{
/* # line 2073 "user.qsc" */	/* set_frs */
            {
              if (IIiqset(2,0,Pwdform,(char *)0, (char *)0) != 0) {
                IIstfsio(54,(char *)"pwd_extern",0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 2074 "user.qsc" */	/* host code */
		}
		else
		{
/* # line 2077 "user.qsc" */	/* set_frs */
            {
              if (IIiqset(2,0,Pwdform,(char *)0, (char *)0) != 0) {
                IIstfsio(54,(char *)"pwd_extern",0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
                IIstfsio(54,(char *)"pwd_str",0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 2079 "user.qsc" */	/* host code */
		}
	}
	else
	{
/* # line 2083 "user.qsc" */	/* set_frs */
            {
              if (IIiqset(2,0,Pwdform,(char *)0, (char *)0) != 0) {
                IIstfsio(54,(char *)"pwd_extern",0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
                IIstfsio(54,(char *)"pwd_str",0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
                IIstfsio(54,(char *)"pwd_str2",0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 2086 "user.qsc" */	/* host code */
	}
/* # line 2087 "user.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB2;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 2091 "user.qsc" */	/* getform */
            {
              if (IIfsetio(Pwdform) != 0) {
                IIgetfldio((short *)0,1,32,9,pwdextern,(char *)"pwd_ex\
tern");
              } /* IIfsetio */
            }
/* # line 2092 "user.qsc" */	/* host code */
	if(*pwdextern=='n')
	{
/* # line 2094 "user.qsc" */	/* set_frs */
            {
              if (IIiqset(2,0,Pwdform,(char *)0, (char *)0) != 0) {
                IIstfsio(54,(char *)"pwd_str",0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 2096 "user.qsc" */	/* host code */
	}
	else
	{
/* # line 2099 "user.qsc" */	/* set_frs */
            {
              if (IIiqset(2,0,Pwdform,(char *)0, (char *)0) != 0) {
                IIstfsio(54,(char *)"pwd_str",0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
                IIstfsio(54,(char *)"pwd_str2",0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 2102 "user.qsc" */	/* host code */
	}
/* # line 2103 "user.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB2;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 2112 "user.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB2;
              } /* IIchkfrm */
            }
/* # line 2113 "user.qsc" */	/* host code */
	if(write_user_pwd(user_name)!=OK)
/* # line 2114 "user.qsc" */	/* resume */
            {
              if (1) goto IIfdB2;
            }
/* # line 2116 "user.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 2121 "user.qsc" */	/* getform */
            {
              if (IIfsetio(Pwdform) != 0) {
                IIgetfldio((short *)0,1,32,24,pwdstr,(char *)"pwd_str");
              } /* IIfsetio */
            }
/* # line 2122 "user.qsc" */	/* host code */
	STtrmwhite(pwdstr);
	if(pwdstr[0]=='\0')
/* # line 2124 "user.qsc" */	/* set_frs */
            {
              if (IIiqset(2,0,Pwdform,(char *)0, (char *)0) != 0) {
                IIstfsio(54,(char *)"pwd_str2",0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 2125 "user.qsc" */	/* host code */
	else
/* # line 2126 "user.qsc" */	/* set_frs */
            {
              if (IIiqset(2,0,Pwdform,(char *)0, (char *)0) != 0) {
                IIstfsio(54,(char *)"pwd_str2",0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 2127 "user.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB2;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 2133 "user.qsc" */	/* host code */
	FEhelp(ERx("icusrpwd.hlp"), ERget(S_IC004B_User_form));
          }
        } else if (IIretval() == 6) {
          {
            i4 changed;
/* # line 2142 "user.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&changed,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 2144 "user.qsc" */	/* host code */
	    if ( changed )
	    {
/* # line 2147 "user.qsc" */	/* getform */
            {
              if (IIfsetio(Pwdform) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,(char *)"\
name");
              } /* IIfsetio */
            }
/* # line 2149 "user.qsc" */	/* host code */
		changed = CONFIRM(Object_name, ERget(F_IC0019_User));
		if(changed==CONFCH_YES)
		{
/* # line 2152 "user.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB2;
              } /* IIchkfrm */
            }
/* # line 2153 "user.qsc" */	/* host code */
		    if(write_user_pwd(user_name)!=OK)
/* # line 2154 "user.qsc" */	/* resume */
            {
              if (1) goto IIfdB2;
            }
/* # line 2155 "user.qsc" */	/* host code */
		}
	    }
/* # line 2157 "user.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else {
          if (1) goto IIfdE2;
        } /* IIretval */
      } /* IIrunform */
IIfdF2:
      if (IIchkfrm() == 0) goto IIfdB2;
      if (IIfsetio((char *)0) == 0) goto IIfdE2;
      goto IIfdE2;
IImuI2:
      if (IIFRafActFld((char *)"pwd_yn",0,1) == 0) goto IIfdE2;
      if (IIFRafActFld((char *)"pwd_extern",0,2) == 0) goto IIfdE2;
      if (IIinitmu() == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Save),ERget(F_FE0117_SaveExpl),2,0,3) == 0) goto 
      IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,0,3) == 0) goto IIfdE2;
      if (IIFRafActFld((char *)"pwd_str",0,4) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,5) == 0) 
      goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,5) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,6) == 0) goto 
      IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,6) == 0) goto IIfdE2;
      if (IIendmu() == 0) goto IIfdE2;
      goto IIfdI2;
IIfdE2:;
      IIendfrm();
    }
/* # line 2161 "user.qsc" */	/* host code */
    return OK;
  }
/*
** Name: write_user_pwd - write user password to database
**
** Description:
**      Saves user password information from Pwdform to database
**
** Inputs:
**      user_name - name for user having password altered
**
** Returns:
**      OK -   operation succeeded
**      FAIL - operation failed
**
** History:
**      20-jul-94 (robf)
**          Created.
**	01-jun-98 (kitch01)
**		Bug 91019. Invalid 'alter user' syntax generated when a user tries to
**		change their own password.
*/
static STATUS
write_user_pwd(char *user_name)
  {
    char pwdyn[10];
    char pwdextern[10];
    char pwdstr[25];
    char pwdstr2[25];
    char oldpwd[25];
  char stmt[255];
	bool    extern_pwd=FALSE;
/* # line 2197 "user.qsc" */	/* getform */
    {
      if (IIfsetio(Pwdform) != 0) {
        IIgetfldio((short *)0,1,32,9,pwdyn,(char *)"pwd_yn");
        IIgetfldio((short *)0,1,32,9,pwdextern,(char *)"pwd_extern");
        IIgetfldio((short *)0,1,32,24,pwdstr,(char *)"pwd_str");
        IIgetfldio((short *)0,1,32,24,pwdstr2,(char *)"pwd_str2");
      } /* IIfsetio */
    }
/* # line 2199 "user.qsc" */	/* host code */
	if(pwdyn[0]=='n')
	{
	     STprintf(stmt,"alter user %s with nopassword",
		icdelimit(user_name));
	}
	else if (pwdextern[0]=='y')
	{
		STprintf(stmt,"alter user %s with external_password",
			icdelimit(user_name));
		extern_pwd=TRUE;
	}
	else
	{
		if(STcompare(pwdstr,pwdstr2)!=0)
		{
			IIUGerr(E_IC0152_Password_mismatch, UG_ERR_ERROR, 0);
			return FAIL;
		}
		if(STlength(pwdstr)<3 || STindex(pwdstr,"'",0)!=NULL)
		{
			IIUGerr(E_IC0153_Password_invalid, UG_ERR_ERROR, 0);
			return FAIL;
		}
		/*
		** Need to check whether user is changing their old
		** password, or assigning a new password
		*/
		if (IiicPrivMaintUser)
		{
		    STprintf(stmt,"alter user %s with password='%s'",
			icdelimit(user_name),
			pwdstr);
		}
		else
		{
/* # line 2234 "user.qsc" */	/* prompt */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      IIprmptio(1,(char *)"Enter your old password:",(short *)0,1,32,24,oldpwd);
    }
/* # line 2236 "user.qsc" */	/* host code */
		    if(*oldpwd=='\0')
			return FAIL;
		    STprintf(stmt,"alter user %s with password='%s', oldpassword='%s'",
				icdelimit(user_name), pwdstr, oldpwd);
		}
	}
/* # line 2842 "user.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 2843 "user.sc" */	/* host code */
	if (sqlca.sqlcode < 0 && sqlca.sqlcode != (-E_GE0032_WARNING))
	{
/* # line 2845 "user.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 2846 "user.sc" */	/* host code */
		/* Diagnostic already issued from DBMS */
		return FAIL;
	}
	else
	{
		if(extern_pwd)
			Iiuser.internal_flags|=USR_EXT_PWD_FLAG;
		else
			Iiuser.internal_flags&= ~USR_EXT_PWD_FLAG;
/* # line 2855 "user.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 2856 "user.sc" */	/* host code */
		return OK;
	}
  }
/*
** Name: user_privileges - build user privileges
**
** Description: Edit full list of user subject privileges, including 
**              defaults.
**
** Inputs:
**      id - user id being edited
**
**      stat - Privilege status mask
**
**      def_stat - Default Privilege status mask
**
** Outputs:
**      stat - Updated Privilege status mask
**
**      def_stat - Updated Default Privilege status mask
**
** History:
**      21-jul-94 (robf)
**          Created
**      01-may-95 (harpa06)
**          Bug #68422: Added a flag to indicate that the priviliges form has
**          been modified which will reflect the User Information form.
*/
static VOID
user_privileges (char *id, i4 *stat, i4 *def_stat)
  {
    char *uname;
    i4 i;
    char p_name[33];
    char p_value[3];
    i4 rowno;
    uname=id;
    Title = ERget(S_IC0150_User_priv_title);
/* # line 2297 "user.qsc" */	/* inittable */
    {
      if (IItbinit(Prvform,(char *)"priv_table",(char *)"u") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 2298 "user.qsc" */	/* host code */
    for (i = 0; Privs[i].id != 0 ; i++)
    {
	char *p;
	STcopy(Privs[i].name, p_name);
	for(p=p_name; *p; p++)
	{
	   if (*p=='_')
		*p=' ';
	   else if(CMupper(p))
		CMtolower(p,p);
	}
	if(*def_stat & Privs[i].id)
/* # line 2310 "user.qsc" */	/* loadtable */
    {
      if (IItbact(Prvform,(char *)"priv_table",1) != 0) {
        IItcoputio((char *)"pvalue",(short *)0,1,32,0,(char *)"y");
        IItcoputio((char *)"pname",(short *)0,1,32,0,p_name);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 2311 "user.qsc" */	/* host code */
	else if(*stat & Privs[i].id)
/* # line 2312 "user.qsc" */	/* loadtable */
    {
      if (IItbact(Prvform,(char *)"priv_table",1) != 0) {
        IItcoputio((char *)"pvalue",(short *)0,1,32,0,(char *)"r");
        IItcoputio((char *)"pname",(short *)0,1,32,0,p_name);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 2313 "user.qsc" */	/* host code */
	else
/* # line 2314 "user.qsc" */	/* loadtable */
    {
      if (IItbact(Prvform,(char *)"priv_table",1) != 0) {
        IItcoputio((char *)"pvalue",(short *)0,1,32,0,(char *)"n");
        IItcoputio((char *)"pname",(short *)0,1,32,0,p_name);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 2315 "user.qsc" */	/* host code */
    }
    if (Mgr)
	Mode = ERx("update");
    else
	Mode = ERx("read");
/* # line 2322 "user.qsc" */	/* display */
    {
      if (IIdispfrm(Prvform,Mode) == 0) goto IIfdE3;
      goto IImuI3;
IIfdI3:;
      if (IIfsetio((char *)0) == 0) goto IIfdE3;
      IIputfldio((char *)"name",(short *)0,1,32,0,uname);
      IIputfldio((char *)"title",(short *)0,1,32,0,Title);
IIfdB3:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 2328 "user.qsc" */	/* host code */
	FEhelp(ERx("icprivs.hlp"), ERget(S_IC004B_User_form));
          }
        } else if (IIretval() == 2) {
          {
/* # line 2335 "user.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE3;
            }
          }
        } else if (IIretval() == 3) {
          {
            i4 changed;
/* # line 2343 "user.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&changed,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 2345 "user.qsc" */	/* host code */
	if (!changed )
/* # line 2346 "user.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE3;
            }
/* # line 2348 "user.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB3;
              } /* IIchkfrm */
            }
/* # line 2350 "user.qsc" */	/* host code */
	PrivChg=TRUE;
/* # line 2352 "user.qsc" */	/* unloadtable */
            {
              if (IItbact(Prvform,(char *)"priv_table",0) == 0) goto IItbE4;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,30,4,&rowno,(char *)"_RECORD");
                IItcogetio((short *)0,1,32,2,p_value,(char *)"pvalue");
                IITBceColEnd();
                {
/* # line 2354 "user.qsc" */	/* host code */
	    i=rowno-1;
	    if(*p_value=='y')
	    {
		*def_stat|=Privs[i].id;
		*stat|=Privs[i].id;
	    }
	    else if(*p_value=='r')
	    {
		*def_stat&= ~Privs[i].id;
		*stat|=Privs[i].id;
	    }
	    else
	    {
		*def_stat&= ~Privs[i].id;
		*stat&= ~Privs[i].id;
	    }
                }
              } /* IItunload */
IItbE4:
              IItunend();
            }
/* # line 2371 "user.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE3;
            }
          }
        } else {
          if (1) goto IIfdE3;
        } /* IIretval */
      } /* IIrunform */
IIfdF3:
      if (IIchkfrm() == 0) goto IIfdB3;
      if (IIfsetio((char *)0) == 0) goto IIfdE3;
      goto IIfdE3;
IImuI3:
      if (IIinitmu() == 0) goto IIfdE3;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,1) == 0) 
      goto IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,1) == 0) goto IIfdE3;
      if (IInmuact(ERget(FE_Cancel),ERget(F_FE0102_ExplEnd),0,0,2) == 0) goto 
      IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,0,0,2) == 0) goto IIfdE3;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,3) == 0) goto 
      IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,3) == 0) goto IIfdE3;
      if (IIendmu() == 0) goto IIfdE3;
      goto IIfdI3;
IIfdE3:;
      IIendfrm();
    }
/* # line 2375 "user.qsc" */	/* host code */
    return ;
  }
/*{
** Name:        usr_mod_chk()   - Disallow removal of super user from ingres
**
** Returns:
**      FALSE   - OK - user isn't trying to update the ingres account
**      TRUE    - ERROR - user is trying to update the ingres account
**
** History:
**
**      12-dec-94 (harpa06)
**          Created for OpenINGRES 1.1. Replica of creation in INGRES 6.4 by
**          nick.
**      05-apr-95 (canor01)
**          get_user_stat() takes two parameters, but was only being
**          passed one, causing SEGV in AIX.
**	20-apr-98 (mcgem01)
**	    Product name change to Ingres.
*/
static bool
usr_mod_chk()
{
    bool        err = FALSE;
     if (!Newuser && IIICiruIsReqUser(U_name))
     {
       i4     u_status = 0;
       i4     u_def_status;
       get_user_stat(&u_status,&u_def_status);
       if (!(u_status & U_SUPER))
       {
	   IIUGerr(E_IC005E_Cannot_remove_super, 0, 1, U_name);
	   err = TRUE;
       }
     }
    return err;
}
/*
** Name:	check_pwd_enabled() - Check password authentication enabled
**
** Inputs:
**		authname - name of user
** Returns:
**      extvalid - FALSE   - External password authentication has not been enabled for
**						  this user
**                 TRUE    - External password authentication has been enabled for this
**						  user
**      ingpwd   - FALSE   - Ingres password not set for this user
**                 TRUE    - Ingres password has been set for this user
**
** History:
**
**		26-May-1998 (kitch01)
**			Created to resolve bugs 74731 and 77451.
**
*/
static void check_pwd_enabled(char *authname, bool *extvalid, bool *ingpwd)
{
  i4 pwdlen;
  char *username;
	char pmname[64];
	char *pmvalue;
	*extvalid = FALSE;
	username = icdelimit(authname);
/* # line 3121 "user.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select length(password)from iiuser where name=");
    IIputdomio((short *)0,1,32,0,username);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&pwdlen);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 3125 "user.sc" */	/* host code */
	*ingpwd = pwdlen ? TRUE : FALSE;
	STprintf(pmname, "ii.$.secure.role_auth.%*s",
			STtrmwhite(authname), (char*)authname);
	if (PMget(pmname, &pmvalue) == OK)
	{
		*extvalid = TRUE;
		return;
	}
	STprintf(pmname, "ii.$.secure.user_auth.%*s",
			STtrmwhite(authname), (char*)authname);
	if (PMget(pmname, &pmvalue) == OK)
	{
		*extvalid = TRUE;
		return;
	}
	return;
}
