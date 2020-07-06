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
#include	<erfe.h>
#include	<ug.h>
#include	<uf.h>
#include	<st.h>
#include        <generr.h>
#include        <stdprmpt.h>
#include	"eric.h"
#include	"global.h"
# include "ictables.h"
/*
fool ming:
#include        "ictables.h"
*/
/**
** NO_OPTIM = hp9_mpe
**
** Name:	db.qsc	-	Database info frame handling
**
** History:
**	18-may-89 (mgw)
**		Put quoted strings into ERx() calls for internationalization.
**      27-sep-1989 (jennifer)
**             Added new SQL statement support for B1/C2 security requirements.
**             For this file GRANT ACCESS and REVOKE ACCESS to databases
**	       statements replaced inserts/replaces into iilocation
**	       catalog directly.
**	       Also changed to use iidbpriv inplace of iidbaccess.
**      ~1-Jan-90 (pete)
**		Allow selection of Help & End even when on invalid field;
**		check for unsaved changes when user hits menuitem End.
**      23-apr-1990 (pete)
**              fix 6.3 bug 21342 with bad explanations for Help & End.
**      19-oct-1990 (pete)
**              Merge in changes done by Jennifer Lyerla in 9/89 for B1 proj.
**	8-nov-1990 (pete)
**		Make changes as per FRC proposal for CHOTS/TMAC project.
**      30-Jun-92 (fredb)
**              Turn off optimization for MPE/iX (hp9_mpe) because we get a
**              bus error with it on.
**      27-jul-1992 (thomasm)
**              Don't optimize this file on hp8
**	4-Mar-1994 (fredv)
**		Removed NO_OPTIM for hp8_us5. Believe it or not, NO OPTIM
**		and -g both will cause bug B60048.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/* # line 64 "db.sc" */	/* host code */
  GLOBALREF char F_Maxname[];
GLOBALREF       bool    Mgr;
GLOBALREF bool  IiicPrivSecurity; /* TRUE if user has SECURITY privilege */
  GLOBALREF IIUSER Iiuser;
  GLOBALREF IIROLE Iirole;
  GLOBALREF IIUSERGROUP Iiusergroup;
  GLOBALREF IIDATABASE Iidatabase;
  GLOBALREF IIDBPRIV Iidbpriv;
GLOBALREF char  *IC_PublicUser;	/* "Public" */
FUNC_EXTERN	STATUS	dbform();
FUNC_EXTERN     STATUS  iiicsdSelectDatabase();
FUNC_EXTERN     STATUS  iiicspSelectDbpriv();
FUNC_EXTERN     STATUS  iiicsuSelectUser();
FUNC_EXTERN     STATUS  iiicsgSelectUsergroup();
FUNC_EXTERN     STATUS  iiicsrSelectRole();
FUNC_EXTERN	char	*puborpriv();
FUNC_EXTERN	STATUS	iiicvaValidateAuthId();
FUNC_EXTERN	char	*IIICgtGetType();
FUNC_EXTERN	bool	IIUGyn();
static	STATUS	loadPrivs();	/* function that loads privs into tbl field */
static		STATUS	db_init();
STATUS	dbaccform();
static		STATUS	acc_init();
static		STATUS	acc_write();
static		STATUS	get_distdbinfo();
  static char Dbfrm[]= ERx("dbfrm");
  static char Accessfrm[]= ERx("dbaccess");
  static char Access_tbl[]= ERx("access_table");
  static char Object_name[FE_MAXNAME+1];
  static char Form_add = FALSE;
  static char Dbtitle[80]= ERx("");
  static char *Dbmode;
  static char *Create_menuitem;
  static char *Edit_menuitem;
  static char *Delete_menuitem;
  static char *Save_menuitem;
  static char *Listchoices_menuitem;
  static char *Accmode;
  static char Acctitle[80]= ERx("");
static  char    Empty[] = ERx("");
/*{
** Name:        dbform() - Information about an INGRES Database display loop.
**
** Description:
**	Runs the display loop for the "Information about an INGRES Database"
**	screen of Accessdb/Catalogdb.
**
** Inputs:
**	char	*dbname	- Name of database.
**
** Outputs:
**      none.
**
** Returns:
**      FAIL    - if error occurs.
**      OK      - if all goes ok.
**
** Side Effects:
**
** History:
**	8-nov-1990 (pete)
**		Added this function documentation.
*/
STATUS
dbform(dbname)
char	*dbname;
  {
    STATUS  err = OK;
    if (fchkdbname(dbname) != OK )
	return FAIL;
    if (!Form_add)
    {
	if (IIUFgtfGetForm(IIUFlcfLocateForm(), Dbfrm) != OK)
	{
	    IIUGerr(S_IC0059_Get_Form_Error, UG_ERR_ERROR, 0);
	    return FAIL;
	}
	/* User can make changes if running Accessdb (as opposed to Catalogdb) &
	** has SECURITY Priv.
	*/
	if (Mgr)
	    STlcopy(ERget(S_IC0080_Accessdb_Db_Info), Dbtitle, sizeof(Dbtitle));
	else
	    STlcopy(ERget(S_IC0081_Catalogdb_Db_Info), Dbtitle, sizeof(Dbtitle));
	if (Mgr && IiicPrivSecurity)
	    Dbmode = ERx("update");
	else
	    Dbmode = ERx("read");
	Form_add = TRUE;
    }
/* # line 175 "db.qsc" */	/* display */
    {
      if (IIdispfrm(Dbfrm,Dbmode) == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio((char *)"title",(short *)0,1,32,0,Dbtitle);
      {
/* # line 178 "db.qsc" */	/* host code */
	if (db_init(dbname) != OK)
	{
	    err = FAIL;
/* # line 181 "db.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 182 "db.qsc" */	/* host code */
	}
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 188 "db.qsc" */	/* host code */
	FEhelp(ERx("icdbfrm.hlp"), ERget(S_IC001D_db_form));
          }
        } else if (IIretval() == 2) {
          {
/* # line 194 "db.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,2,1) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,2,2) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,2,2) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 199 "db.qsc" */	/* host code */
    return err;
  }
/*{
** Name:	db_init - initialize entries on database screen.
**
** Description:
**
** Inputs:
**	##char	*db_name	name of database.
**
** Outputs:
**
**	Returns:
**		OK if database was found & user is authorized to look at it;
**		FAIL otherwise.
**
**	Exceptions:
**		NONE
**
** Side Effects:
**
** History:
**	13-apr-1990	(pete)
**		I added this function documentation + added support
**		for the ii_dump location in iidatabase.dmpdev, which
**		somehow got forgotten when ii_dump was implemented for 6.3
**		(jupbug 9752). Also, simplify how WHERE clause in query
**		of iidatabase is built.
**	9-nov-90 (pete)
**		Made changes for the CHOTS/TMAC security project.
**	20-sep-93 (robf)
**              Fix two existing bugs:
**	        - "type" not always set, could AV depending on the stack
**	        - Make type not-hidden for CDB/STAR databases
**      14-jan-94 (huffman)
**              type needs to be initlized to local.  AXP VMS cannot
**              work with it uninitlized.
*/
static STATUS
  db_init(db_name)
  char *db_name;
  {
    char *type;
    char cdbnm[DB_GW1_MAXNAME+1];
    char cdbnd[DB_GW1_MAXNAME+1];
/* # line 247 "db.qsc" */	/* message */
    {
      IImessage(ERget(S_IC001E_Retrieving_db_info));
    }
/* # line 249 "db.qsc" */	/* host code */
	/* select row from iidatabase */
	if (iiicsdSelectDatabase(db_name, &Iidatabase) != OK)
		return FAIL;
	*cdbnm = EOS;
	*cdbnd = EOS;
	/*
	** Determine db type ("distributed" or "Star-CDB") and get
	** corresponding string to display in Type field.
	** If distributed, also get Coordinator DB info from iistar_cdbs catalog
	*/
/* # line 261 "db.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Dbfrm,(char *)0, (char *)0) != 0) {
        IIstfsio(54,(char *)"cdb_name",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(54,(char *)"cdb_node",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 263 "db.qsc" */	/* host code */
	if ( Iidatabase.dbservice & DB_DISTRIBUTED ) 
	{
	    type = ERget(F_IC0010_Distributed);
	    (void) get_distdbinfo(db_name, Iidatabase.own, cdbnm, cdbnd);
	    /* make the cdb_name & cdb_node fields visible */
/* # line 270 "db.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Dbfrm,(char *)0, (char *)0) != 0) {
        IIstfsio(54,(char *)"cdb_name",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
        IIstfsio(54,(char *)"cdb_node",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 272 "db.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Dbfrm,(char *)0, (char *)0) != 0) {
        IIstfsio(54,(char *)"type",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 273 "db.qsc" */	/* host code */
	}
	else if (  Iidatabase.dbservice & DB_STAR_CDB ) 
	{
	    type = ERget(F_IC0012_Star_CDB);
/* # line 277 "db.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Dbfrm,(char *)0, (char *)0) != 0) {
        IIstfsio(54,(char *)"type",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 278 "db.qsc" */	/* host code */
	}
	else 
	{
/*          type is a local db */
            type = (char *)ERget(F_IC0011_Local) ;
/* # line 283 "db.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Dbfrm,(char *)0, (char *)0) != 0) {
        IIstfsio(54,(char *)"type",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 284 "db.qsc" */	/* host code */
	    type = ERx(""); /* So not empty */
	}
/* # line 288 "db.qsc" */	/* putform */
    {
      if (IIfsetio(Dbfrm) != 0) {
        IIputfldio((char *)"dbname",(short *)0,1,32,0,db_name);
        IIputfldio((char *)"ownname",(short *)0,1,32,0,Iidatabase.own);
        IIputfldio((char *)"dbdev",(short *)0,1,32,0,Iidatabase.dbdev);
        IIputfldio((char *)"ckpdev",(short *)0,1,32,0,Iidatabase.ckpdev);
        IIputfldio((char *)"wrkdev",(short *)0,1,32,0,Iidatabase.sortdev);
        IIputfldio((char *)"jnldev",(short *)0,1,32,0,Iidatabase.jnldev);
        IIputfldio((char *)"dmpdev",(short *)0,1,32,0,Iidatabase.dmpdev);
        IIputfldio((char *)"type",(short *)0,1,32,0,type);
        IIputfldio((char *)"cdb_name",(short *)0,1,32,0,cdbnm);
        IIputfldio((char *)"cdb_node",(short *)0,1,32,0,cdbnd);
      } /* IIfsetio */
    }
/* # line 299 "db.qsc" */	/* host code */
	return OK;
  }
/*{
** Name:        dbaccform() - Information about an INGRES Database display loop.
**
** Description:
**
** Inputs:
**	char	*dbname	- Name of database.
**
** Outputs:
**      none.
**
** Returns:
**      FAIL    - if error occurs.
**      OK      - if all goes ok.
**
** Side Effects:
**
** History:
**	14-jul-1993 (kellyp)
**	    Replaced ListChoices_menuitem with three menuitems for 
**	    User, Group, & Role (b52705).
**	21-jul-1993 (kellyp)
**	    Corrected error messages for ListGroups and ListRoles.
**	21-jul-1993 (kellyp)
**	    Corrected the error made in the previous change of transposing
**	    the error messages.
**	4-aug-1993 (kellyp)
**	    When returning a value from the picklist, place the value on the
**	    current row if the current row is empty. Otherwise, place the
**	    value at the end of the table. Access defaults to YES if the
**	    database is PRIVATE and NO if the database is PUBLIC.
**	6-aug-1993 (kellyp)
**	    Added ListChoices menuitem.
**	18-aug-1993 (kellyp)
**	    Deleted ListUsers, ListGroups, ListChoices
**	24-aug-1993 (kellyp)
**	    Database access will not reflect Group and Role info.
**	16-sep-1993 (kellyp)
**	    Deleted IIUFfieldhelp
**	18-nov-1993 (kellyp)
**	    Should not allow giving/revoking access to/from DBA.
**	    Not a meaning operation. (bug 56655)
*/
STATUS
dbaccform(dbname)
char	*dbname;
  {
    STATUS  err = OK;
    char *type;
    if (Acctitle[0] == EOS)
    {
	if (IIUFgtfGetForm(IIUFlcfLocateForm(), Accessfrm) != OK)
	{
	    IIUGerr(S_IC0059_Get_Form_Error, UG_ERR_ERROR, 0);
	    return FAIL;
	}
	/* User can make changes if running Accessdb (as opposed to Catalogdb) &
	** has SECURITY Priv.
	*/
	if (Mgr)
	{
	    STlcopy(ERget(S_IC0102_Acc_Dbaccess_Title), 
					Acctitle, sizeof(Acctitle));
	}
	else
	{
	    STlcopy(ERget(S_IC0103_Cat_Dbaccess_Title), 
					Acctitle, sizeof(Acctitle));
	}
	if (Mgr && IiicPrivSecurity)
	{
	    Create_menuitem = ERget(FE_Create);
	    Edit_menuitem = ERget(FE_Edit);
	    Delete_menuitem = ERget(FE_Delete);
	    Save_menuitem = ERget(FE_Save);
	    Listchoices_menuitem = ERget(FE_Lookup);
	    Accmode = ERx("update");
/* # line 382 "db.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Accessfrm,(char *)0, (char *)0) != 0) {
        IIstfsio(53,(char *)"defaultaccess",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
        IIstfsio(17,(char *)"defaultaccess",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 385 "db.qsc" */	/* host code */
	}
	else
	{
	    /* User is either running Catalogdb, or Accessdb & with only
	    ** MAINTAIN_LOCATIONS privilege.
	    */
	    Create_menuitem = Empty;
	    Edit_menuitem = Empty;
	    Delete_menuitem = Empty;
	    Save_menuitem = Empty;
	    Listchoices_menuitem = Empty;
	    Accmode = ERx("read");
/* # line 397 "db.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Accessfrm,(char *)0, (char *)0) != 0) {
        IIstfsio(53,(char *)"defaultaccess",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(17,(char *)"defaultaccess",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 399 "db.qsc" */	/* host code */
	}
    }
/* # line 402 "db.qsc" */	/* display */
    {
      if (IIdispfrm(Accessfrm,Accmode) == 0) goto IIfdE2;
      goto IImuI2;
IIfdI2:;
      if (IIfsetio((char *)0) == 0) goto IIfdE2;
      IIputfldio((char *)"title",(short *)0,1,32,0,Acctitle);
      {
/* # line 405 "db.qsc" */	/* host code */
	if (acc_init(dbname) != OK)
	{
	    err = FAIL;
/* # line 408 "db.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE2;
        }
/* # line 409 "db.qsc" */	/* host code */
	}
      }
IIfdB2:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 416 "db.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,19,
                (char *)0,0);
              } /* IIiqset */
            }
/* # line 417 "db.qsc" */	/* host code */
        if (STequal(Object_name, Access_tbl))
        {
/* # line 419 "db.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,Accessfrm,Access_tbl,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
/* # line 420 "db.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,Accessfrm,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 421 "db.qsc" */	/* host code */
        }
        else
            IIUGerr(E_IC006C_NotOnTableField, UG_ERR_ERROR, 0);
          }
        } else if (IIretval() == 2) {
          {
/* # line 430 "db.qsc" */	/* host code */
        if (Save_menuitem[0] == EOS)
        {
            /* this was selected via the FRSkey */
            FTbell();
/* # line 434 "db.qsc" */	/* resume */
            {
              if (1) goto IIfdB2;
            }
/* # line 435 "db.qsc" */	/* host code */
        }
/* # line 437 "db.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB2;
              } /* IIchkfrm */
            }
/* # line 438 "db.qsc" */	/* host code */
	if (acc_write(dbname) != OK) 
	{
/* # line 440 "db.qsc" */	/* resume */
            {
              IIresfld(Access_tbl);
              if (1) goto IIfdB2;
            }
/* # line 441 "db.qsc" */	/* host code */
	}
/* # line 442 "db.qsc" */	/* clear */
            {
              IIfldclear(Access_tbl);
            }
/* # line 443 "db.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else if (IIretval() == 3) {
          {
            char nm[FE_MAXNAME + 1];
            char tmpname[FE_MAXNAME + 1];
            char tmpaccess[FE_MAXNAME + 1];
/* # line 454 "db.qsc" */	/* host code */
        if (Listchoices_menuitem[0] == EOS)
        {
            /* this was selected via the FRSkey */
            FTbell();
/* # line 458 "db.qsc" */	/* resume */
            {
              if (1) goto IIfdB2;
            }
/* # line 459 "db.qsc" */	/* host code */
        }
/* # line 461 "db.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,19,
                (char *)0,0);
              } /* IIiqset */
            }
/* # line 462 "db.qsc" */	/* host code */
	if (STequal(Object_name, ERx("defaultaccess")))
	{
	    char    msg[256];
	    char    lpstr[2048];  
	    char    *public = ERget(F_IC000C_public);
	    char    *private = ERget(F_IC000D_private);
            char *choice;
	    i4     rtn;
	    STprintf(msg, ERget(S_IC0090_select_default_access), dbname);
	    /* 
	     * Slow message (or was it fast?) caused private message to
	     * overwrite public due to usage of static static string.
	     * so separate calls to ERget()
	     *
	     * STprintf(lpstr, ERx("%s;%s\n%s;%s"),
             *	public, "Available to all but selected users.",
	     * private, "Access denied to all but selected users.");
	     */
 	    STprintf(lpstr, "%s;%s\n", public, ERget(S_IC0091_pub_exp));
	    STcat( lpstr, private);
	    STcat( lpstr, ";" );
	    STcat( lpstr, ERget(S_IC0092_priv_exp));
	    rtn = IIFDlpListPick(msg, lpstr, 0, -1, -1,
		  ERget(S_IC0093_defacc_help_title), ERx("icdefacc.hlp"), 
		  NULL, NULL );
	    if (rtn >= 0)
	    {
		choice = (rtn == 0 ? public : private);
/* # line 495 "db.qsc" */	/* putform */
            {
              if (IIfsetio(Accessfrm) != 0) {
                IIputfldio((char *)"defaultaccess",(short *)0,1,32,0,choice);
              } /* IIfsetio */
            }
/* # line 496 "db.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,Accessfrm,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 497 "db.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB2;
            }
/* # line 498 "db.qsc" */	/* host code */
	    }
/* # line 499 "db.qsc" */	/* resume */
            {
              IIresfld((char *)"defaultaccess");
              if (1) goto IIfdB2;
            }
/* # line 500 "db.qsc" */	/* host code */
	}
	else if (STequal(Object_name, Access_tbl))
	{
/* # line 504 "db.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,34,
                (char *)0,0);
              } /* IIiqset */
            }
/* # line 505 "db.qsc" */	/* host code */
	    if (STequal(Object_name, ERx("uname")))
	    {
		icListPicks(ERget(F_IC001B_UserType), 
				ERx("iiusers"), ERx("user_name"), nm);
		if (nm[0] != EOS)
		{
            i4 state = stNEW;
/* # line 513 "db.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Accessfrm,Access_tbl,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,tmpname,(char *)"\
uname");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 514 "db.qsc" */	/* getform */
            {
              if (IIfsetio(Accessfrm) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpaccess,(char *)"\
defaultaccess");
              } /* IIfsetio */
            }
/* # line 515 "db.qsc" */	/* host code */
		    if (STequal(tmpaccess, ERget(F_IC000D_private)))
			STcopy(Yes, tmpaccess);
		    else
			STcopy(No, tmpaccess);
		    if (STequal(tmpname, ERx("")))
/* # line 521 "db.qsc" */	/* putrow */
            {
              if (IItbsetio(3,Accessfrm,Access_tbl,-2) != 0) {
                IItcoputio((char *)"uname",(short *)0,1,32,0,nm);
                IItcoputio((char *)"type",(short *)0,1,32,0,
                ERget(F_IC001B_UserType));
                IItcoputio((char *)"access",(short *)0,1,32,0,tmpaccess);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 524 "db.qsc" */	/* host code */
		    else
		    {
/* # line 526 "db.qsc" */	/* loadtable */
            {
              if (IItbact(Accessfrm,Access_tbl,1) != 0) {
                IItcoputio((char *)"uname",(short *)0,1,32,0,nm);
                IItcoputio((char *)"type",(short *)0,1,32,0,
                ERget(F_IC001B_UserType));
                IItcoputio((char *)"access",(short *)0,1,32,0,tmpaccess);
                IItcoputio((char *)"_STATE",(short *)0,1,30,4,&state);
                IITBceColEnd();
              } /* IItbact */
            }
/* # line 529 "db.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Accessfrm,Access_tbl,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 530 "db.qsc" */	/* host code */
		    }
/* # line 531 "db.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,Accessfrm,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 532 "db.qsc" */	/* host code */
		}
		else
		{
/* # line 535 "db.qsc" */	/* resume */
            {
              IIrescol(Access_tbl,(char *)"uname");
              if (1) goto IIfdB2;
            }
/* # line 536 "db.qsc" */	/* host code */
		}
	    }
	}
	else
/* # line 541 "db.qsc" */	/* resume */
            {
              if (1) goto IIfdB2;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 550 "db.qsc" */	/* host code */
        if (Mgr)
            FEhelp(ERx("icdbacc.hlp"), ERget(S_IC011D_dbacc_form));
        else
	    FEhelp(ERx("icroacc.hlp"), ERget(S_IC011D_dbacc_form));
          }
        } else if (IIretval() == 5) {
          {
/* # line 560 "db.qsc" */	/* host code */
        if (Mgr)
        {
            /* user is running Accessdb */
            i4 changed;
/* # line 565 "db.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&changed,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 566 "db.qsc" */	/* host code */
            if ( changed )
            {
/* # line 568 "db.qsc" */	/* getform */
            {
              if (IIfsetio(Accessfrm) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,(char *)"\
dbname");
              } /* IIfsetio */
            }
/* # line 570 "db.qsc" */	/* host code */
                changed = CONFIRM(Object_name, ERget(F_IC001A_Database));
                switch(changed)
                {
                case CONFCH_YES:
/* # line 574 "db.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB2;
              } /* IIchkfrm */
            }
/* # line 575 "db.qsc" */	/* host code */
		    if (acc_write(Object_name) != OK) 
		    {
/* # line 577 "db.qsc" */	/* resume */
            {
              IIresfld(Access_tbl);
              if (1) goto IIfdB2;
            }
/* # line 578 "db.qsc" */	/* host code */
		    }
/* # line 579 "db.qsc" */	/* clear */
            {
              IIfldclear(Access_tbl);
            }
/* # line 580 "db.qsc" */	/* host code */
                    /* fall through */
                case CONFCH_NO:
/* # line 582 "db.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
/* # line 583 "db.qsc" */	/* host code */
                default:
                    break;
                }
/* # line 587 "db.qsc" */	/* resume */
            {
              if (1) goto IIfdB2;
            }
/* # line 588 "db.qsc" */	/* host code */
            }
        }
/* # line 590 "db.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 596 "db.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,19,
                (char *)0,0);
              } /* IIiqset */
            }
/* # line 597 "db.qsc" */	/* host code */
        if (STequal(Object_name, Access_tbl))
            FEtabfnd(Accessfrm, Object_name);
        else
            IIUGerr(E_IC006C_NotOnTableField, UG_ERR_ERROR, 0);
          }
        } else if (IIretval() == 7) {
          {
/* # line 606 "db.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,19,
                (char *)0,0);
              } /* IIiqset */
            }
/* # line 607 "db.qsc" */	/* host code */
        if (STequal(Object_name, Access_tbl))
/* # line 608 "db.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Accessfrm,Object_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 609 "db.qsc" */	/* host code */
        else
            IIUGerr(E_IC006C_NotOnTableField, UG_ERR_ERROR, 0);
          }
        } else if (IIretval() == 8) {
          {
/* # line 616 "db.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,19,
                (char *)0,0);
              } /* IIiqset */
            }
/* # line 617 "db.qsc" */	/* host code */
        if (STequal(Object_name, Access_tbl))
/* # line 618 "db.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Accessfrm,Object_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 619 "db.qsc" */	/* host code */
        else
            IIUGerr(E_IC006C_NotOnTableField, UG_ERR_ERROR, 0);
          }
        } else if (IIretval() == 9) {
          {
/* # line 625 "db.qsc" */	/* host code */
	STATUS stat;
            i4 changed;
            i4 auth_type;
            char tmpaccess[FE_MAXNAME + 1];
            char tmpname[FE_MAXNAME + 1];
/* # line 631 "db.qsc" */	/* inquire_frs */
            {
              if (IIiqset(5,0,(char *)"",(char *)"",(char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&changed,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 632 "db.qsc" */	/* host code */
        if (changed != 0)
        {
            /* User changed the Authorization Identifier value.
	    ** Validate it.
	    */
/* # line 637 "db.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Accessfrm,Access_tbl,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,(char *)"\
uname");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 639 "db.qsc" */	/* host code */
	    if (STtrmwhite(Object_name) > 0)
	    {
# ifndef UNIX
		/* User, Group & Role names are case sensitive on UNIX only */
		IIUGlbo_lowerBEobject(Object_name);
# endif
		/* check type of & validate Authorization Identifier */
		stat = iiicvaValidateAuthId(Object_name, &type, &auth_type);
		if (stat != OK)
		{
		    if (stat == IC_INVALID)
		    {
			/* no such authorization identifier */
			IIUGerr(E_IC006E_AuthId_DoesntExist, UG_ERR_ERROR, 1,
						    Object_name);
		    }
/* # line 656 "db.qsc" */	/* resume */
            {
              if (1) goto IIfdB2;
            }
/* # line 657 "db.qsc" */	/* host code */
		}
/* # line 660 "db.qsc" */	/* getform */
            {
              if (IIfsetio(Accessfrm) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpaccess,(char *)"\
defaultaccess");
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpname,(char *)"\
ownname");
              } /* IIfsetio */
            }
/* # line 663 "db.qsc" */	/* host code */
		if (STequal(tmpname, Object_name))
		{
		    IIUGerr(E_IC0001_Cannot_use_DBA, UG_ERR_ERROR, 0);
		}
		if (STequal(tmpaccess, ERget(F_IC000D_private)))
		    STcopy(Yes, tmpaccess);
		else
		    STcopy(No, tmpaccess);
/* # line 672 "db.qsc" */	/* putrow */
            {
              if (IItbsetio(3,Accessfrm,Access_tbl,-2) != 0) {
                IItcoputio((char *)"type",(short *)0,1,32,0,type);
                IItcoputio((char *)"int_type",(short *)0,1,30,4,&auth_type);
                IItcoputio((char *)"access",(short *)0,1,32,0,tmpaccess);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 674 "db.qsc" */	/* host code */
	    }
            /* mark column so won't be validated next time */
/* # line 677 "db.qsc" */	/* set_frs */
            {
              if (IIiqset(5,0,(char *)"",(char *)"",(char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 678 "db.qsc" */	/* host code */
        }
	else
/* # line 680 "db.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB2;
            }
          }
        } else if (IIretval() == 10) {
          {
/* # line 685 "db.qsc" */	/* host code */
	STATUS	stat;
            i4 changed;
            char *yorn = Empty;
            char ybuf[128];
/* # line 690 "db.qsc" */	/* inquire_frs */
            {
              if (IIiqset(5,0,(char *)"",(char *)"",(char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&changed,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 691 "db.qsc" */	/* host code */
	if (changed != 0)
        {
            /* User entered, hopefully, yes or no.  */
/* # line 694 "db.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Accessfrm,Access_tbl,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,(char *)"\
uname");
                IItcogetio((short *)0,1,32,127,ybuf,(char *)"access");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 696 "db.qsc" */	/* host code */
	    if (Object_name[0] != EOS)
	    {
		yorn = IIUGyn(ybuf, &stat) ? Yes : No;
		if (stat == E_UG0007_Illegal_Response
		    || stat == E_UG0006_NULL_Response)
		{
                    /* illegal yes/no response */
                    IIUGerr(E_IC0130_Yes_or_No, UG_ERR_ERROR, 0);
/* # line 704 "db.qsc" */	/* resume */
            {
              if (1) goto IIfdB2;
            }
/* # line 705 "db.qsc" */	/* host code */
		}
/* # line 706 "db.qsc" */	/* putrow */
            {
              if (IItbsetio(3,Accessfrm,Access_tbl,-2) != 0) {
                IItcoputio((char *)"access",(short *)0,1,32,0,yorn);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 707 "db.qsc" */	/* host code */
	    }
	    else
	    {
/* # line 710 "db.qsc" */	/* putrow */
            {
              if (IItbsetio(3,Accessfrm,Access_tbl,-2) != 0) {
                IItcoputio((char *)"access",(short *)0,1,32,0,(char *)"");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 711 "db.qsc" */	/* host code */
	    }
/* # line 712 "db.qsc" */	/* set_frs */
            {
              if (IIiqset(5,0,(char *)"",(char *)"",(char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 713 "db.qsc" */	/* host code */
        }
/* # line 715 "db.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB2;
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
      if (IIinitmu() == 0) goto IIfdE2;
      if (IInmuact(Delete_menuitem,ERget(F_IC0018_RowDeleteExpl),0,0,1) == 0) 
      goto IIfdE2;
      if (IInmuact(Save_menuitem,ERget(F_FE0117_SaveExpl),2,0,2) == 0) goto 
      IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,1,2) == 0) goto IIfdE2;
      if (IInmuact(Listchoices_menuitem,ERget(FE_LookupExpl),0,0,3) == 0) 
      goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,3) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,2,4) == 0) 
      goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,4) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,2,5) == 0) goto 
      IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,2,5) == 0) goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,6) == 0) goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,7) == 0) goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,8) == 0) goto IIfdE2;
      if (IITBcaClmAct(Access_tbl,(char *)"uname",0,9) == 0) goto IIfdE2;
      if (IITBcaClmAct(Access_tbl,(char *)"access",0,10) == 0) goto IIfdE2;
      if (IIendmu() == 0) goto IIfdE2;
      goto IIfdI2;
IIfdE2:;
      IIendfrm();
    }
/* # line 720 "db.qsc" */	/* host code */
    return err;
  }
/*{
** Name:	acc_init - initialize entries on database access screen.
**
** Description:
**
** Inputs:
**	##char	*db_name	name of database.
**
** Outputs:
**
**	Returns:
**		OK if database was found & user is authorized to look at it;
**		FAIL otherwise.
**
**	Exceptions:
**		NONE
**
** Side Effects:
**
** History:
**	13-apr-1990	(pete)
**		I added this function documentation + added support
**		for the ii_dump location in iidatabase.dmpdev, which
**		somehow got forgotten when ii_dump was implemented for 6.3
**		(jupbug 9752). Also, simplify how WHERE clause in query
**		of iidatabase is built.
**	9-nov-90 (pete)
**		Made changes for the CHOTS/TMAC security project.
**	17-mar-94 (michael)
**              Added select on db_access field of iidbprivileges to correctly
**              display private|public in defaultaccess field of "Access" form.
**      19-Oct-98 (wanya01)
**              Fix bug 93383.
*/
static STATUS
  acc_init(db_name)
  char *db_name;
  {
  char *d_global;
    char *tb_mode;
    char cdbnm[DB_GW1_MAXNAME+1];
    char cdbnd[DB_GW1_MAXNAME+1];
	int tmp;
        int d_flag;
/* # line 775 "db.qsc" */	/* message */
    {
      IImessage(ERget(S_IC001E_Retrieving_db_info));
    }
/* # line 777 "db.qsc" */	/* host code */
	/* select row from iidatabase */
	if (iiicsdSelectDatabase(db_name, &Iidatabase) != OK)
		return FAIL;
	/*
	** Load Grants into table field.
	*/
	tb_mode = Mgr ?  ERx("fill") : ERx("read");
	/* create hidden versions of all displayed fields */
/* # line 788 "db.qsc" */	/* inittable */
    {
      if (IItbinit(Accessfrm,Access_tbl,tb_mode) != 0) {
        IIthidecol((char *)"name_orig",F_Maxname);
        IIthidecol((char *)"int_type",(char *)"i2");
        IIthidecol((char *)"int_type_orig",(char *)"i2");
        IIthidecol((char *)"access_orig",(char *)"c4");
        IItfill();
      } /* IItbinit */
    }
/* # line 793 "db.qsc" */	/* host code */
        /* as a default, access is public and the iidpriv table is empty. 
	 * set the bit on to reflect this default
	 */
	Iidbpriv.flags |= DB_ACCESS;
        d_flag = 1;
	/* select all grants for this database from iidbpriv */
	(void) iiicspSelectDbpriv(db_name, &Iidbpriv, loadPrivs, &d_flag);
	*cdbnm = EOS;
	*cdbnd = EOS;
        /* initialize access flag as a default */
	/* set if database is "public" or "private"  */
        if (d_flag)
          d_global = ERget(F_IC000C_public);
        else
          d_global = ERget(F_IC000D_private);
/* # line 813 "db.qsc" */	/* putform */
    {
      if (IIfsetio(Accessfrm) != 0) {
        IIputfldio((char *)"dbname",(short *)0,1,32,0,db_name);
        IIputfldio((char *)"ownname",(short *)0,1,32,0,Iidatabase.own);
        IIputfldio((char *)"defaultaccess",(short *)0,1,32,0,d_global);
        IIputfldio((char *)"orig_def_access",(short *)0,1,32,0,d_global);
      } /* IIfsetio */
    }
/* # line 819 "db.qsc" */	/* host code */
	return OK;
  }
/*{	acc_write	Write changes about a database to dbdb
**
**	Update database global access flag and make incremental changes
**	to user authorization list
**
**	Returns:
**		OK	database updated ok.
**		FAIL	error updating database.
**
**	History:
**
**	12-mar-86 (marian)	bug # 7162
**		Added 'and iidbaccess.dbname = Db_name'
**		to the where clause of the delete commands so
**		only the rows where it corresponds to the given
**		databse are deleted.
**	08-sep-87 (daver)
**		Set the Database to be operative when changing
**		to be global or private.
**	30-nov-89 (jennifer) 
**		Change iidbaccess to use new REVOKE statement.
**	12-nov-1990 (pete)
**		changed to handle sense of ACCESS/NOACCESS, authorization
**		identifiers, rather than just users, and user Public. (CHOTS)
**	19-jul-1991 (pete)
**		Wrap CVlower(username) with "#ifndef UNIX", as done
**		in ctrl.qsc. Bug 38718.
**	4-apr-1992 (pete)
**		Removed couple unneeded CVlower(u_name) calls in acc_write.
**		One call was unnecessary, cause u_name not used in first
**		unloadtable loop; second was redundant. These were probably
**		bugs from integrating TMAC code line.
**	22-jun-1993 (kellyp)
**		Corrected REVOKE and GRANT access syntax (bug 52706)
**	14-jul-1993 (kellyp)
**		Corrected value for the GRANT statement.
**	16-mar-1994 (michael)
**		Added GRANT statements for modified default access of db.
**		Was previously only flipping bits.
*/
static STATUS
  acc_write(db_name)
  char *db_name;
  {
    STATUS      uval = OK;
    char name_orig[FE_MAXNAME+1];
    i4 int_type_orig;
    char access_orig[3 + 1];
    char type[6 + 1];
    i4 int_type;
    char access[3 +1];
    char defaccess[20];
    char origdefaccess[20];
    char u_name[FE_MAXNAME+1];
    i4 u_state;
    i4 u_record;
  char sbuffer[256];
    /* NOTE: assume all displayed values in table field are valid */
/* # line 890 "db.qsc" */	/* message */
    {
      IImessage(ERget( S_IC0023_Updating_database ));
    }
/* # line 892 "db.qsc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 894 "db.qsc" */	/* host code */
    /* Handle default (public vs. private) access. */
/* # line 895 "db.qsc" */	/* getform */
    {
      if (IIfsetio(Accessfrm) != 0) {
        IIgetfldio((short *)0,1,32,19,defaccess,(char *)"defaultaccess");
        IIgetfldio((short *)0,1,32,19,origdefaccess,(char *)
"orig_def_access");
      } /* IIfsetio */
    }
/* # line 897 "db.qsc" */	/* host code */
    if (!STequal(defaccess, origdefaccess))
    {
  i4 bit_n = DB_GLOBAL;
  i4 bit;
	bit = (STequal(defaccess, ERx("private")) ? DB_PRIVATE : DB_GLOBAL);
	if (bit == DB_PRIVATE) 
	{
	    STprintf(sbuffer, ERx("GRANT NOACCESS ON DATABASE %s TO PUBLIC"),
                     db_name);
/* # line 1279 "db.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1280 "db.sc" */	/* host code */
        }
        else
	{
	    STprintf(sbuffer, ERx("GRANT ACCESS ON DATABASE %s TO PUBLIC"),
                     db_name);
/* # line 1285 "db.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1286 "db.sc" */	/* host code */
        }
	iiuicw1_CatWriteOn();
/* # line 1288 "db.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iidatabase i set access=((i.access/(2*");
    IIputdomio((short *)0,1,30,4,&bit_n);
    IIwritio(0,(short *)0,1,32,0,(char *)"))*2*");
    IIputdomio((short *)0,1,30,4,&bit_n);
    IIwritio(0,(short *)0,1,32,0,(char *)") +MOD(i.access, ");
    IIputdomio((short *)0,1,30,4,&bit_n);
    IIwritio(0,(short *)0,1,32,0,(char *)") +");
    IIputdomio((short *)0,1,30,4,&bit);
    IIwritio(0,(short *)0,1,32,0,(char *)"where i.name=");
    IIputdomio((short *)0,1,32,0,db_name);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1292 "db.sc" */	/* host code */
	if (FEinqerr() != OK)
	    uval = FAIL;
	iiuicw0_CatWriteOff();
	if (uval != OK)
	    goto done;
    }
    /* Process stDELETE rows first (UNLOADTABLE delivers them last!). */
/* # line 932 "db.qsc" */	/* unloadtable */
    {
      if (IItbact(Accessfrm,Access_tbl,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,u_name,(char *)"unam\
e");
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,name_orig,(char *)
"name_orig");
        IItcogetio((short *)0,1,32,6 + 1-1,type,(char *)"type");
        IItcogetio((short *)0,1,30,4,&int_type_orig,(char *)"int_type_\
orig");
        IItcogetio((short *)0,1,30,4,&int_type,(char *)"int_type");
        IItcogetio((short *)0,1,32,3 +1-1,access,(char *)"access");
        IItcogetio((short *)0,1,32,3 + 1-1,access_orig,(char *)
"access_orig");
        IItcogetio((short *)0,1,30,4,&u_state,(char *)"_STATE");
        IItcogetio((short *)0,1,30,4,&u_record,(char *)"_RECORD");
        IITBceColEnd();
        {
/* # line 944 "db.qsc" */	/* host code */
	switch (u_state)
	{
	  case stDELETE:
	    /*
	    ** Delete old user.
	    **
	    ** WITH THIS RELEASE, DBAs have unlimited privileges on their
	    ** Databases, so ok to allow DBA privs to be REVOKED; code added
	    ** for bug 21815 removed (bug 21815 was about problems when privs
	    ** for DBA on privated dbase were deleted)  (pete)
	    */
	    if (int_type_orig != TYPE_PUBLIC)
	    {
	        STprintf(sbuffer,
		   ERx("REVOKE %sACCESS ON DATABASE %s FROM %s %s"),
		   access_orig[0] == Yes[0] ? Empty : No,
		   db_name,
		   IIICgtGetType(int_type_orig),  /* User, Group or Role */
	    	   name_orig);
	    }
	    else
	    {
	        STprintf(sbuffer,
		   ERx("REVOKE %sACCESS ON DATABASE %s FROM PUBLIC"),
		  (access_orig[0] == Yes[0]) ? Empty : No,
		   db_name);
	    }
/* # line 1346 "db.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1347 "db.sc" */	/* host code */
            if (FEinqerr() != OK && sqlca.sqlcode != (-E_GE0032_WARNING))
            {
                uval = FAIL;
/* # line 976 "db.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 977 "db.qsc" */	/* host code */
            }
	} 
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 981 "db.qsc" */	/* host code */
    if (uval == FAIL)
	goto done;
    /* process everything but stDELETE rows */
/* # line 985 "db.qsc" */	/* unloadtable */
    {
      if (IItbact(Accessfrm,Access_tbl,0) == 0) goto IItbE2;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,u_name,(char *)"unam\
e");
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,name_orig,(char *)
"name_orig");
        IItcogetio((short *)0,1,32,6 + 1-1,type,(char *)"type");
        IItcogetio((short *)0,1,30,4,&int_type_orig,(char *)"int_type_\
orig");
        IItcogetio((short *)0,1,30,4,&int_type,(char *)"int_type");
        IItcogetio((short *)0,1,32,3 +1-1,access,(char *)"access");
        IItcogetio((short *)0,1,32,3 + 1-1,access_orig,(char *)
"access_orig");
        IItcogetio((short *)0,1,30,4,&u_state,(char *)"_STATE");
        IItcogetio((short *)0,1,30,4,&u_record,(char *)"_RECORD");
        IITBceColEnd();
        {
/* # line 998 "db.qsc" */	/* host code */
# ifndef UNIX
	/* User, Group & Role names are case sensitive on UNIX only */
        IIUGlbo_lowerBEobject(u_name);
# endif
	if (STtrmwhite(u_name) == 0)
	    continue;
	switch (u_state)
	{
	  case stCHANGE:
	    /*
	    ** Delete old Access, and then add new Access using Fall Thru...
	    */
	    if (int_type_orig != TYPE_PUBLIC)
	    {
	        STprintf(sbuffer,
		   ERx("REVOKE %sACCESS ON DATABASE %s FROM %s %s"),
		  (access_orig[0] == Yes[0]) ? Empty : No,
		   db_name,
		   IIICgtGetType(int_type_orig),  /* User, Group or Role */
	    	   name_orig);
	    }
	    else
	    {
	        STprintf(sbuffer,
		   ERx("REVOKE %sACCESS ON DATABASE %s FROM PUBLIC"),
		  (access_orig[0] == Yes[0]) ? Empty : No,
		   db_name);
	    }
/* # line 1414 "db.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1415 "db.sc" */	/* host code */
            if (FEinqerr() != OK && sqlca.sqlcode != (-E_GE0032_WARNING))
            {
                uval = FAIL;
/* # line 1032 "db.qsc" */	/* endloop */
          {
            if (1) goto IItbE2;
          }
/* # line 1033 "db.qsc" */	/* host code */
            }
	    /* stCHANGE, fall through ... */
	  case stNEW:
	    if (int_type != TYPE_PUBLIC)
	    {
	        STprintf(sbuffer,
		    ERx("GRANT %sACCESS ON DATABASE %s TO %s %s"),
		    (access[0] == Yes[0]) ? Empty : No,
		    db_name,
		    type,  /* User, Group or Role */
		    u_name);
	    }
	    else
	    {
	    	STprintf(sbuffer,
			ERx("GRANT %sACCESS ON DATABASE %s TO PUBLIC"),
			(access[0] == Yes[0]) ? Empty : No,
			db_name);
	    }
/* # line 1442 "db.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1443 "db.sc" */	/* host code */
            if (FEinqerr() != OK && sqlca.sqlcode != (-E_GE0032_WARNING))
            {
                uval = FAIL;
/* # line 1061 "db.qsc" */	/* endloop */
          {
            if (1) goto IItbE2;
          }
/* # line 1062 "db.qsc" */	/* host code */
            }
	    break;
	} 
        }
      } /* IItunload */
IItbE2:
      IItunend();
    }
/* # line 1069 "db.qsc" */	/* host code */
done:
    if (uval == OK)
/* # line 1462 "db.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1463 "db.sc" */	/* host code */
    else
/* # line 1464 "db.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1465 "db.sc" */	/* host code */
    return uval;
  }
/*{
** Name:	loadPrivs - load privileges into table field.
**
** Description:
**	stuff rows into a table field.
**
** Inputs:
**	IIDBPRIV *iidbpriv	structure full of good stuff to put into
**				table field.
** Outputs:
**
**	Returns:
**		OK if row loaded ok; FAIL otherwise.
**
**	Exceptions:
**		NONE
**
** Side Effects:
**
** History:
**	9-nov-1990 (pete)
**		Initial Version.
*/
static STATUS
  loadPrivs(iidbpriv)
  IIDBPRIV *iidbpriv;
  {
    char *type;
    char *access;
    if ((iidbpriv->control & DB_ACCESS) != 0)
	/* this tuple specifies a Database ACCESS privilege */
	access = ((iidbpriv->flags & DB_ACCESS) != 0) ? Yes : No;
    else
	return OK;	/* don't load this row. It probably specifies
			** a DIO or CPU limit, or some other database
			** priv. We only want Access privs.
			*/
    switch(iidbpriv->gtype)
    {
    case TYPE_USER:
	type = ERget(F_IC001B_UserType);
	break;
    case TYPE_GROUP:
	type = ERget(F_IC001C_GroupType);
	break;
    case TYPE_ROLE:
	type = ERget(F_IC001D_RoleType);
	break;
    case TYPE_PUBLIC:
	type = ERget(F_IC001E_PublicType);
	STcopy(IC_PublicUser, iidbpriv->grantee);
	break;
    default:
	type = ERget(F_IC001F_UnknownType);
	break;
    }
/* # line 1138 "db.qsc" */	/* loadtable */
    {
      if (IItbact(Accessfrm,Access_tbl,1) != 0) {
        IItcoputio((char *)"uname",(short *)0,1,32,0,iidbpriv->grantee);
        IItcoputio((char *)"name_orig",(short *)0,1,32,0,iidbpriv->grantee);
        IItcoputio((char *)"type",(short *)0,1,32,0,type);
        IItcoputio((char *)"int_type",(short *)0,1,30,4,&iidbpriv->gtype);
        IItcoputio((char *)"int_type_orig",(short *)0,1,30,4,&iidbpriv->gtype);
        IItcoputio((char *)"access",(short *)0,1,32,0,access);
        IItcoputio((char *)"access_orig",(short *)0,1,32,0,access);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1149 "db.qsc" */	/* host code */
    return OK;
  }
/*{
** Name:	get_distdbinfo - get info on a distributed database.
**
** Description:
**	Select info on the coordinator database name and node for a
**	distributed database.
**
** Inputs:
**	char *db_name	name of distributed database.
**	char *d_ownname	owner of database.
**	char *cdbnm	write name of CDB here.
**	char *cdbnd	write node-name of CDB here.
**
** Outputs:
**	Writes to *cdbnm & *cdbnd.
**
**	Returns:
**		OK if CDB info found ok.
**		FAIL otherwise.
**
**	Exceptions:
**		NONE
**
** Side Effects:
**
** History:
**	9-nov-1990 (pete)
**		Added this function documentation & removed reference
**		to a couple fields which were just "dynamic" field titles
**		from the days before invisible fields.
*/
static STATUS
get_distdbinfo (db_name, d_ownname, cdbnm, cdbnd)
  char *db_name;
  char *d_ownname;
  char *cdbnm;
  char *cdbnd;
{
	STATUS	stat = OK;
	*cdbnm = EOS; /* if query fails, these will be empty strings. */
	*cdbnd = EOS;
	/* following should get an error if >1 CDB for a dist dbase */
/* # line 1584 "db.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select cdb_name, cdb_node from iistar_cdbs where ddb_name=");
    IIputdomio((short *)0,1,32,0,db_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and ddb_owner=");
    IIputdomio((short *)0,1,32,0,d_ownname);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,0,cdbnm);
      IIgetdomio((short *)0,1,32,0,cdbnd);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1589 "db.sc" */	/* host code */
	if (sqlca.sqlcode < 0) 
	{
	    IIUGerr(E_IC0064_Err_getting_CDBinfo, UG_ERR_ERROR, 1, db_name);
	    stat = FAIL;
	}
	else if (sqlca.sqlcode == 100 ) 
	{
	    IIUGerr(E_IC0065_No_CDB_info_found, UG_ERR_ERROR, 0);
	    stat = FAIL;
	}
	return stat;
}
/*{
** Name:	IIICgtGetType - convert numeric to string type name.
**
** Description:	convert numeric to string type name (User, Group, Role,
**	Public).
**
** Inputs:
**	i4	int_type_orig
**
** Outputs:
**
**	Returns:
**		string type name equivalent of input arg.
**
** History:
**	12-nov-1990 (pete)
**		Initial version.
*/
char *
IIICgtGetType(int_type_orig)
i4	int_type_orig;
{
	char *type;
	switch (int_type_orig)
	{
	case TYPE_USER:
		type = ERget(F_IC001B_UserType);
		break;
	case TYPE_GROUP:
		type = ERget(F_IC001C_GroupType);
		break;
	case TYPE_ROLE:
		type = ERget(F_IC001D_RoleType);
		break;
	case TYPE_PUBLIC:
		type = ERget(F_IC001E_PublicType);
		break;
	default:
		type = ERget(F_IC001F_UnknownType);
		break;
	}
	return type;
}
/*{
** Name:   iiicvaValidateAuthId - validate Auth Id & return its Type.
**
** Description:
**
** Inputs:
**	char	*name		Auth id name to be checked.
**	char	**type		This routine writes the auth id type here.
**	i4	*int_type	Internal numeric equivalent of "type". This
**				routine writes to this.
**
** Outputs:
**	Writes to parameters: "type" and "int_type".
**
** Returns:
**	FAIL if error occurred during query; OK otherwise.
**
** Side Effects:
**
** History:
**	12-nov-1990 (pete)
**		Initial Version.
*/
STATUS
iiicvaValidateAuthId(name, type, int_type)
char	*name;
char	**type;
i4	*int_type;
{
	STATUS stat = OK;
	i4	cnt;
	/* see if it's Public */
	if (STbcompare(name, 0, IC_PublicUser, 0, TRUE) == 0)
	{
            *type = IC_PublicUser;
	    *int_type = TYPE_PUBLIC;
	    goto done;
	}
	/* Assertion: it's not Public.
	** See if it's a User.
	*/
	if (iiicsuSelectUser(name, &Iiuser) != OK)
	{
	    stat = FAIL;
	    goto done;
	}
	else if (FEinqrows() > 0)
	{
	    /* Authorization Identifier is a User */
            *type = ERget(F_IC001B_UserType);
	    *int_type = TYPE_USER;
	    goto done;
	}
	/* Assertion: it's not Public or User.
	** See if it's a Group.
	*/
        cnt = iiicueUsergroupExists(name);
        if (FEinqerr() != OK)
	{
	    stat = FAIL;
	    goto done;
	}
        else if (cnt > 0)
        {
	    /* Authorization Identifier is a Group */
            *type = ERget(F_IC001C_GroupType);
	    *int_type = TYPE_GROUP;
	    goto done;
        }
	/* Assertion: it's not Public, User or Group.
	** See if it's a Role.
	*/
        if (iiicsrSelectRole(name, &Iirole) != OK)
	{
	    stat = FAIL;
	    goto done;
	}
        else if (FEinqrows() > 0)
        {
	    /* Authorization Identifier is a Role */
            *type = ERget(F_IC001D_RoleType);
	    *int_type = TYPE_ROLE;
	    goto done;
        }
	/* Assertion:  Invalid name; it's not Public, User, Group or Role. */
	stat = IC_INVALID;
done:
	if (stat == FAIL)
            IIUGerr(E_IC006F_Retrieve_AuthError, UG_ERR_ERROR, 1, name);
	return stat;
}
