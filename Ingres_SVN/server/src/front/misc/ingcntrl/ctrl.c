# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
#include	<compat.h>
#include	<er.h>
#include	<gl.h>
#include	<sl.h>
#include	<st.h>
#include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
#include	<uf.h>
#include	<erfe.h>
#include	<uigdata.h>
#include	"eric.h"
#include	"global.h"
GLOBALREF       bool    Mgr;
GLOBALREF bool  IiicPrivSysadmin;  /* TRUE if user has SYSTEM_ADMIN privilege */
GLOBALREF bool  IiicPrivSecurity;
static bool	Dbcat_added = FALSE;
  static char Dbtblfrm[]= ERx("dbtblfrm");
static VOID init_dbtbl();
static VOID dbcatalog();
#define POLICY_MESSAGE_LENGTH 72+1
#define MAX_CAP_VALUE_LENGTH  6+1
#define MAX_CAPABILITY_LENGTH 22+1
/*{
** Name:	mgrform - display the main Accessdb menu.
**
** Description:
**	Display the main Accessdb screen & menu.
**
** Inputs:
**	NONE
**
** Outputs:
**
**	Returns:
**		VOID
**
**	Exceptions:
**		NONE
**
** Side Effects:
**
** History:
**	11-apr-1990 (pete)
**		Added this module description + declare mgrform as a VOID +
**		allow users to enter too-long a username & database name.
**      23-apr-1990 (pete)
**              fix 6.3 bug 21342 with bad explanations for Help & End.
**      15-jun-1993 (kellyp)
**              replaced QUIT menuitem with END menuitem in the Database
**		Catalog frame.
**      21-jun-1993 (kellyp)
**		Removed ACCESSGRANT menuitem. It's too buggy and everything
**		done through this menuitem can be done directly in SQL, so
**		no functionality is denied.
**	26-Aug-1993 (fredv)
**		included <st.h>.
**	26-Jan-1994 (michael)
**		Added function to INITIALIZE block to display alpha case policy
**		statement.
**	30-Nov-1994 (nanpr01)
**		Fixed Bug # 60935. Cursor now waits in the menu line
**		rather than case sensitive field. 
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
VOID
mgrform()
  {
    char db_name[(2*DB_MAXNAME)+1];
    char *accessgrants_menuitem = ERx("");
    char casePolicyMsg[POLICY_MESSAGE_LENGTH];
  char username[FE_MAXNAME+1];
	void	fmtCaseMsg();
	if ( IIUFgtfGetForm(IIUFlcfLocateForm(), ERx("ctrlmenu")) != OK )
	{
	    IIUGerr(S_IC0059_Get_Form_Error, UG_ERR_FATAL, 0);
	}
/* # line 88 "ctrl.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select name from iiuser where name=user");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,username);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 92 "ctrl.sc" */	/* host code */
	fmtCaseMsg( casePolicyMsg );
/* # line 101 "ctrl.qsc" */	/* display */
    {
      if (IIdispfrm((char *)"ctrlmenu",(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio((char *)"case_policy",(short *)0,1,32,0,casePolicyMsg);
      IIputfldio((char *)"openingres_uname",(short *)0,1,32,0,username);
      {
/* # line 104 "ctrl.qsc" */	/* putform */
        {
          if (IIfsetio((char *)"ctrlmenu") != 0) {
            IIputfldio((char *)"case_policy",(short *)0,1,32,0,casePolicyMsg);
            IIputfldio((char *)"openingres_uname",(short *)0,1,32,0,username);
          } /* IIfsetio */
        }
/* # line 106 "ctrl.qsc" */	/* resume */
        {
          IIresmu();
          if (1) goto IIfdB1;
        }
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 111 "ctrl.qsc" */	/* host code */
		dbcatalog(ERx(""));
/* # line 112 "ctrl.qsc" */	/* resume */
            {
              IIresmu();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 116 "ctrl.qsc" */	/* host code */
		loclst_form();
/* # line 117 "ctrl.qsc" */	/* resume */
            {
              IIresmu();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 121 "ctrl.qsc" */	/* host code */
		usrtbl_form();
/* # line 122 "ctrl.qsc" */	/* resume */
            {
              IIresmu();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 128 "ctrl.qsc" */	/* host code */
		FEhelp(ERx("icmgr.hlp"), ERget(S_IC001B_Manager_Menu_Form));
/* # line 129 "ctrl.qsc" */	/* resume */
            {
              IIresmu();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 133 "ctrl.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(F_IC0001_Databases),(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_IC0005_Locations),(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_IC0006_Users),(char *)0,2,2,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,2,4) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Quit),(char *)0,2,2,5) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,2,2,5) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,2,5) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
  }
/* # line 138 "ctrl.qsc" */	/* host code */
/*{
** Name:        fmtCaseMsg - Format message for displaying on form.
**
** Description:
**      Format a message describing the case sensitivity policy for displaying
**      on Form.
**
** Inputs:
**      char pointer to place message into.
**
** Outputs:
**
**      Returns:
**              VOID
**
**      Exceptions:
**              NONE
**
** Side Effects:
**
** History:
**      25-jan-1994 (michael)
**              created this function to format message describing case
**              sensitivity/insensitivity policy for displaying on the screen.
*/
void 
fmtCaseMsg( msg )
char *msg;
{
  char cap_capability[MAX_CAPABILITY_LENGTH];
  char cap_value[MAX_CAP_VALUE_LENGTH];
    char tmpMsg[POLICY_MESSAGE_LENGTH];
    /* get capabilities that describe case policy from iidbdb */
/* # line 227 "ctrl.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select cap_capability, cap_value from iidbcapabilities where cap_capa\
bility='DB_DELIMITED_CASE'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,MAX_CAPABILITY_LENGTH-1,cap_capability);
      IIgetdomio((short *)0,1,32,MAX_CAP_VALUE_LENGTH-1,cap_value);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 231 "ctrl.sc" */	/* host code */
    STtrmwhite( cap_value );
    /* format policy string */
    if (STequal( cap_value, "MIXED" ))
    {
	STprintf( tmpMsg, "case sensitive" );
    } else {
	STprintf( tmpMsg, "case insensitive" );
    }
    /* little rear cover here */
    if (msg == (char *)NULL)
    {
	msg = STalloc( tmpMsg );
    } else {
	STcopy(tmpMsg, msg);
    }
}
/*{
** Name:	dbcatalog - display the databases.
**
** Description:
**	Display the databases.
**
** Inputs:
**	NONE
**
** Outputs:
**
**	Returns:
**		VOID
**
**	Exceptions:
**		NONE
**
** Side Effects:
**
** History:
**	11-apr-1990 (pete)
**		Added this module description + declare mgrform as a VOID +
**		allow users to enter too-long a username & database name.
**      23-apr-1990 (pete)
**              fix 6.3 bug 21342 with bad explanations for Help & End.
*/
static VOID
dbcatalog(username)
char	*username;
  {
    char dbname[DB_MAXNAME+1];
    char *extend_menuitem;
    char *dbcat_title;
    char *accessgrants_menuitem = ERx("");
	if (!Dbcat_added)
	{
	    if ( IIUFgtfGetForm(IIUFlcfLocateForm(), Dbtblfrm) != OK )
	    {
		IIUGerr(S_IC0059_Get_Form_Error, UG_ERR_FATAL, 0);
	    }
	    Dbcat_added = TRUE;
	}
	if (Mgr && IiicPrivSecurity)
        {
	    dbcat_title = ERget(F_IC0002_Acc_Database_Catalog);
            extend_menuitem = ERget(F_IC0007_ExtendDB);
        }
        else
        {
            /* User is either running Catalogdb, or Accessdb & with only
            ** SYSTEM_ADMIN privilege.
            */
	    dbcat_title = ERget(F_IC0003_Cat_Database_Catalog);
            extend_menuitem = ERget(F_IC000B_DbExtensions);
        }
/* # line 262 "ctrl.qsc" */	/* display */
    {
      if (IIdispfrm(Dbtblfrm,(char *)"r") == 0) goto IIfdE2;
      goto IImuI2;
IIfdI2:;
      if (IIfsetio((char *)0) == 0) goto IIfdE2;
      {
/* # line 265 "ctrl.qsc" */	/* host code */
		init_dbtbl(username);
      }
IIfdB2:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 270 "ctrl.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Dbtblfrm,(char *)"db_table",-2) != 0) {
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,dbname,(char *)
"db_name");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 271 "ctrl.qsc" */	/* host code */
		if ( STtrmwhite(dbname) > 0 && dbname[0] != EOS )
			(VOID) dbaccform(dbname);
          }
        } else if (IIretval() == 2) {
          {
/* # line 277 "ctrl.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Dbtblfrm,(char *)"db_table",-2) != 0) {
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,dbname,(char *)
"db_name");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 278 "ctrl.qsc" */	/* host code */
		if ( STtrmwhite(dbname) > 0 && dbname[0] != EOS )
			(VOID) extendform(dbname);
          }
        } else if (IIretval() == 3) {
          {
/* # line 284 "ctrl.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Dbtblfrm,(char *)"db_table",-2) != 0) {
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,dbname,(char *)
"db_name");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 285 "ctrl.qsc" */	/* host code */
		if ( STtrmwhite(dbname) > 0 && dbname[0] != EOS )
			(VOID) dbform(dbname);
          }
        } else if (IIretval() == 4) {
          {
/* # line 292 "ctrl.qsc" */	/* host code */
		FEhelp(ERx("icdblst.hlp"), ERget(S_IC001B_Manager_Menu_Form));
          }
        } else if (IIretval() == 5) {
          {
/* # line 297 "ctrl.qsc" */	/* breakdisplay */
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
      goto IIfdE2;
IImuI2:
      if (IIinitmu() == 0) goto IIfdE2;
      if (IInmuact(ERget(F_IC0024_AccessGrants),(char *)0,2,2,1) == 0) goto 
      IIfdE2;
      if (IInmuact(extend_menuitem,(char *)0,2,2,2) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_MoreInfo),(char *)0,2,2,3) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,2,4) == 0) 
      goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,4) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,5) == 0) goto 
      IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,5) == 0) goto IIfdE2;
      if (IIendmu() == 0) goto IIfdE2;
      goto IIfdI2;
IIfdE2:;
      IIendfrm();
    }
  }
/* # line 301 "ctrl.qsc" */	/* host code */
/*{
** Name:	infoform - display the main Catalogdb menu.
**
** Description:
**	Display the main Catalogdb screen & menu.
**
** Inputs:
**	NONE
**
** Outputs:
**
**	Returns:
**		VOID
**
**	Exceptions:
**		NONE
**
** Side Effects:
**
** History:
**	7-nov-1990 (pete)
**		Added this module description & changed rouserform() call to
**		userform(). Remove queries that RETRIEVE the username
**		and replace with IIUIgdata() call. Also changed rodbform()
**		to dbform().
**	14-oct-1993 (kellyp)
**		CATALOGDB now has it's own top menu frame.
**	14-oct-1993 (kellyp)
**		Changed the name of the top menu frame from cctrlmeu
**		to catamenu ( for portability restriction ).
*/
infoform(uc)
char	*uc;
  {
	if (IIUFgtfGetForm(IIUFlcfLocateForm(), ERx("catamenu")) != OK)
	    IIUGerr(S_IC0059_Get_Form_Error, UG_ERR_FATAL, 0);
/* # line 339 "ctrl.qsc" */	/* display */
    {
      if (IIdispfrm((char *)"catamenu",(char *)"f") == 0) goto IIfdE3;
      goto IImuI3;
IIfdI3:;
      if (IIfsetio((char *)0) == 0) goto IIfdE3;
IIfdB3:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 344 "ctrl.qsc" */	/* host code */
		dbcatalog(uc);
          }
        } else if (IIretval() == 2) {
          {
/* # line 348 "ctrl.qsc" */	/* host code */
		(VOID) userform(IIUIdbdata()->suser);
          }
        } else if (IIretval() == 3) {
          {
/* # line 352 "ctrl.qsc" */	/* host code */
		loclst_form();
          }
        } else if (IIretval() == 4) {
          {
/* # line 357 "ctrl.qsc" */	/* host code */
		FEhelp(ERx("icusr.hlp"), ERget(S_IC001C_User_Menu_Form));
          }
        } else if (IIretval() == 5) {
          {
/* # line 361 "ctrl.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(F_IC0001_Databases),(char *)0,2,2,1) == 0) goto IIfdE3;
      if (IInmuact(ERget(F_IC0009_User),(char *)0,2,2,2) == 0) goto IIfdE3;
      if (IInmuact(ERget(F_IC0005_Locations),(char *)0,2,2,3) == 0) goto IIfdE3;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,2,4) == 0) 
      goto IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,4) == 0) goto IIfdE3;
      if (IInmuact(ERget(FE_Quit),(char *)0,2,2,5) == 0) goto IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,2,2,5) == 0) goto IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,2,5) == 0) goto IIfdE3;
      if (IIendmu() == 0) goto IIfdE3;
      goto IIfdI3;
IIfdE3:;
      IIendfrm();
    }
  }
/* # line 366 "ctrl.qsc" */	/* host code */
/*{
** Name:	init_dbtbl - display the main Database menus.
**
** Description:
**	Display the main Database screen & menus.
**
** Inputs:
**	uname
**
** Outputs:
**
**	Returns:
**		VOID
**
**	Exceptions:
**		NONE
**
** Side Effects:
**
** History:
**	12-jul-1993 (kellyp)
**		Added this module description.
**	20-sep-1993 (robf)
**              Query standard catalog (iidatabase_info) not internal catalog
**	        (iidatabases). This allows users without QUERY_SYSCAT priv
**	        to use accessdb/catalogdb
*/
static VOID
init_dbtbl(uname)
  char *uname;
  {
  char owner[FE_MAXNAME+1];
  char name[DB_MAXNAME+1];
  u_i4 dbservice;
	/* Initialize read table field */
/* # line 407 "ctrl.qsc" */	/* inittable */
    {
      if (IItbinit(Dbtblfrm,(char *)"db_table",(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 408 "ctrl.qsc" */	/* message */
    {
      IImessage(ERget(S_IC0026_Retrieving_db_names));
    }
/* # line 410 "ctrl.qsc" */	/* host code */
	/* Retrieve database info ... */
/* # line 547 "ctrl.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select database_name, database_owner, database_service from iidatabas\
e_info where database_owner=");
    IIputdomio((short *)0,1,32,0,uname);
    IIwritio(0,(short *)0,1,32,0,(char *)"or ''=");
    IIputdomio((short *)0,1,32,0,uname);
    IIwritio(0,(short *)0,1,32,0,(char *)"order by database_name");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,owner);
      IIgetdomio((short *)0,1,30,4,&dbservice);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 553 "ctrl.sc" */	/* host code */
	{
		if ( dbservice & DB_DISTRIBUTED )
		{
/* # line 420 "ctrl.qsc" */	/* loadtable */
    {
      if (IItbact(Dbtblfrm,(char *)"db_table",1) != 0) {
        IItcoputio((char *)"db_name",(short *)0,1,32,0,name);
        IItcoputio((char *)"db_owner",(short *)0,1,32,0,owner);
        IItcoputio((char *)"db_type",(short *)0,1,32,0,
        ERget(F_IC0010_Distributed));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 423 "ctrl.qsc" */	/* host code */
		}
		else if ( dbservice & DB_STAR_CDB )
		{
/* # line 426 "ctrl.qsc" */	/* loadtable */
    {
      if (IItbact(Dbtblfrm,(char *)"db_table",1) != 0) {
        IItcoputio((char *)"db_name",(short *)0,1,32,0,name);
        IItcoputio((char *)"db_owner",(short *)0,1,32,0,owner);
        IItcoputio((char *)"db_type",(short *)0,1,32,0,
        ERget(F_IC0012_Star_CDB));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 429 "ctrl.qsc" */	/* host code */
		}
		else
		{
/* # line 432 "ctrl.qsc" */	/* loadtable */
    {
      if (IItbact(Dbtblfrm,(char *)"db_table",1) != 0) {
        IItcoputio((char *)"db_name",(short *)0,1,32,0,name);
        IItcoputio((char *)"db_owner",(short *)0,1,32,0,owner);
        IItcoputio((char *)"db_type",(short *)0,1,32,0,ERget(F_IC0011_Local));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 435 "ctrl.qsc" */	/* host code */
		}
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
  }
