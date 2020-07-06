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
** Name:	instacc.qsc	-	Access Grants
**
** Defines:
**	IIICiaInstallationAccess	- Installation-Wide Access GRANTs.
**
** History:
**	15-nov-1990 (pete)
**		Initial Version.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**    25-Oct-2005 (hanje04)
**        Add prototype for authid_init() & loadIAPrivs() to prevent compiler
**        errors with GCC 4.0 due to conflict with implicit declaration.
**/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/* # line 45 "instacc.sc" */	/* host code */
  GLOBALREF char F_Maxname[];
GLOBALREF       bool    Mgr;
GLOBALREF bool  IiicPrivSecurity; /* TRUE if user has SECURITY privilege */
  GLOBALREF IIDBPRIV Iidbpriv;
GLOBALREF char  *IC_PublicUser; /* "Public" */
FUNC_EXTERN     STATUS  iiicspSelectDbpriv();
FUNC_EXTERN	STATUS	iiicvaValidateAuthId();
FUNC_EXTERN	STATUS	IIICiaInstallationAccess();
FUNC_EXTERN     char    *IIICgtGetType();
static		STATUS	authid_write();
static		STATUS	authid_init();
static		STATUS	loadIAPrivs();
  static char Insacc_frm[]= ERx("icinsacc");
  static char Authid_tbl[]= ERx("authid_access");
  static bool InsaccFormAdd = FALSE;
  static char Object_name[FE_MAXNAME+1];
static  char    Empty[] = ERx("");
/*{
** Name:        IIICiaInstallationAccess() - Installation Wide Access Privs.
**
** Description:
**	Maintain list of INSTALLATION grants on authorization identifiers.
**
** Inputs:
**	NONE
**
** Outputs:
**
** Returns:
**      FAIL    - if error occurs.
**      OK      - if all goes ok.
**
** Side Effects:
**
** History:
**	15-nov-1990 (pete)
**		Initial Version.
*/
STATUS
IIICiaInstallationAccess()
  {
    STATUS  err = OK;
    char *save_menuitem;
    char *delete_menuitem;
    char *listchoices_menuitem;
    char *type;
    i4 work_nat;
    char *mode;
    char tmpname[FE_MAXNAME + 1];
    if (!InsaccFormAdd)
    {
	if (IIUFgtfGetForm(IIUFlcfLocateForm(), Insacc_frm) != OK)
	{
	    IIUGerr(S_IC0059_Get_Form_Error, UG_ERR_ERROR, 0);
	    return FAIL;
	}
	InsaccFormAdd = TRUE;
    }
    /* User can make changes if running Accessdb (as opposed to Catalogdb) &
    ** has SECURITY Priv.
    */
    if (Mgr && IiicPrivSecurity)
    {
        save_menuitem = ERget(FE_Save);
        delete_menuitem = ERget(FE_Delete);
        listchoices_menuitem = ERget(FE_Lookup);
	mode = ERx("update");
    }
    else
    {
	/* User is running Accessdb with only SYSTEM_ADMIN privilege. */
        save_menuitem = Empty;
        delete_menuitem = Empty;
        listchoices_menuitem = Empty;
	mode = ERx("read");
    }
/* # line 140 "instacc.qsc" */	/* display */
    {
      if (IIdispfrm(Insacc_frm,mode) == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 143 "instacc.qsc" */	/* host code */
	if (authid_init() != OK)
	{
	    err = FAIL;
/* # line 146 "instacc.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 147 "instacc.qsc" */	/* host code */
	}
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 153 "instacc.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,19,
                (char *)0,0);
              } /* IIiqset */
            }
/* # line 154 "instacc.qsc" */	/* host code */
        if (STequal(Object_name, Authid_tbl))
        {
/* # line 156 "instacc.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,Insacc_frm,Authid_tbl,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
/* # line 157 "instacc.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,Insacc_frm,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 158 "instacc.qsc" */	/* host code */
        }
        else
            IIUGerr(E_IC006C_NotOnTableField, UG_ERR_ERROR, 0);
          }
        } else if (IIretval() == 2) {
          {
/* # line 167 "instacc.qsc" */	/* host code */
        if (save_menuitem[0] == EOS)
        {
            /* this was selected via the FRSkey */
            FTbell();
/* # line 171 "instacc.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 172 "instacc.qsc" */	/* host code */
        }
/* # line 174 "instacc.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 175 "instacc.qsc" */	/* host code */
	if (authid_write() != OK) {
/* # line 176 "instacc.qsc" */	/* resume */
            {
              IIresfld(Authid_tbl);
              if (1) goto IIfdB1;
            }
/* # line 177 "instacc.qsc" */	/* host code */
	}
/* # line 178 "instacc.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 185 "instacc.qsc" */	/* host code */
        if (listchoices_menuitem[0] == EOS)
        {
            /* this was selected via the FRSkey */
            FTbell();
/* # line 189 "instacc.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 190 "instacc.qsc" */	/* host code */
        }
/* # line 191 "instacc.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,tmpname,19,(char *)0
                ,0);
              } /* IIiqset */
            }
/* # line 192 "instacc.qsc" */	/* host code */
        if (STequal(tmpname, Authid_tbl))
        {
            icListPicks(ERget(F_IC001B_UserType),
                                ERx("iiusers"), ERx("user_name"), tmpname);
            if (tmpname[0] != EOS)
            {
		/* This listpick really ought to handle groups and roles, too.*/
/* # line 199 "instacc.qsc" */	/* putrow */
            {
              if (IItbsetio(3,Insacc_frm,Authid_tbl,-2) != 0) {
                IItcoputio((char *)"identifier",(short *)0,1,32,0,tmpname);
                IItcoputio((char *)"type",(short *)0,1,32,0,
                ERget(F_IC0009_User));
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 201 "instacc.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,Insacc_frm,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 202 "instacc.qsc" */	/* resume */
            {
              IIrescol(Authid_tbl,(char *)"access");
              if (1) goto IIfdB1;
            }
/* # line 203 "instacc.qsc" */	/* host code */
            }
        }
        else
            IIUGerr(E_IC006C_NotOnTableField, UG_ERR_ERROR, 0);
          }
        } else if (IIretval() == 4) {
          {
/* # line 212 "instacc.qsc" */	/* host code */
        FEhelp(ERx("icinsacc.hlp"), ERget(S_IC0074_InstallationAccess));
          }
        } else if (IIretval() == 5) {
          {
/* # line 218 "instacc.qsc" */	/* host code */
        if (Mgr)
        {
            /* user is running Accessdb */
            i4 changed;
/* # line 223 "instacc.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&changed,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 224 "instacc.qsc" */	/* host code */
            if ( changed )
            {
                changed = CONFIRM(ERget(F_IC0022_InsAccGrants), Empty);
                switch(changed)
                {
                case CONFCH_YES:
/* # line 230 "instacc.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 231 "instacc.qsc" */	/* host code */
		    if (authid_write() != OK) {
/* # line 232 "instacc.qsc" */	/* resume */
            {
              IIresfld(Authid_tbl);
              if (1) goto IIfdB1;
            }
/* # line 233 "instacc.qsc" */	/* host code */
		    }
                    /* fall through */
                case CONFCH_NO:
/* # line 236 "instacc.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 237 "instacc.qsc" */	/* host code */
                default:
                    break;
                }
/* # line 241 "instacc.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 242 "instacc.qsc" */	/* host code */
            }
        }
/* # line 245 "instacc.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 252 "instacc.qsc" */	/* host code */
        FEtabfnd(Insacc_frm, Authid_tbl);
          }
        } else if (IIretval() == 7) {
          {
/* # line 258 "instacc.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Insacc_frm,Authid_tbl,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 8) {
          {
/* # line 264 "instacc.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Insacc_frm,Authid_tbl,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 9) {
          {
/* # line 269 "instacc.qsc" */	/* host code */
	STATUS stat;
/* # line 271 "instacc.qsc" */	/* inquire_frs */
            {
              if (IIiqset(5,0,(char *)"",(char *)"",(char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&work_nat,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 272 "instacc.qsc" */	/* host code */
        if (work_nat == 1)
        {
            /* User changed the Authorization Identifier value.
	    ** Validate it.
	    */
/* # line 277 "instacc.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Insacc_frm,Authid_tbl,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,(char *)"\
identifier");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 279 "instacc.qsc" */	/* host code */
# ifndef UNIX
	    /* User, Group & Role names are case sensitive on UNIX only */
            IIUGlbo_lowerBEobject(Object_name);
# endif
            /* Check type of & VALIDATE Authorization Identifier ("public"
	    ** is also valid.
	    */
            if ((stat = iiicvaValidateAuthId(Object_name, &type, &work_nat))
									!= OK)
	    {
		if (stat == IC_INVALID)
		{
                    /* no such authorization identifier */
                    IIUGerr(E_IC006E_AuthId_DoesntExist, UG_ERR_ERROR, 1,
                                                Object_name);
		}
/* # line 296 "instacc.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 297 "instacc.qsc" */	/* host code */
            }
/* # line 299 "instacc.qsc" */	/* putrow */
            {
              if (IItbsetio(3,Insacc_frm,Authid_tbl,-2) != 0) {
                IItcoputio((char *)"type",(short *)0,1,32,0,type);
                IItcoputio((char *)"int_type",(short *)0,1,30,4,&work_nat);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 301 "instacc.qsc" */	/* host code */
            /* mark column so won't be validated next time */
/* # line 302 "instacc.qsc" */	/* set_frs */
            {
              if (IIiqset(5,0,(char *)"",(char *)"",(char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 303 "instacc.qsc" */	/* host code */
        }
/* # line 305 "instacc.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
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
      if (IInmuact(delete_menuitem,ERget(F_IC0018_RowDeleteExpl),0,0,1) == 0) 
      goto IIfdE1;
      if (IInmuact(save_menuitem,ERget(F_FE0117_SaveExpl),2,1,2) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,1,2) == 0) goto IIfdE1;
      if (IInmuact(listchoices_menuitem,ERget(FE_LookupExpl),0,0,3) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0101_ExplKeyHelp),0,0,4) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,5) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,5) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,8) == 0) goto IIfdE1;
      if (IITBcaClmAct(Authid_tbl,(char *)"identifier",0,9) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 310 "instacc.qsc" */	/* host code */
    return err;
  }
/*{
** Name:	authid_init - initialize entries in authid_access tbl field.
**
** Description:
**
** Inputs:
**
** Outputs:
**
**	Returns:
**		OK if all went well.
**		FAIL otherwise.
**
**	Exceptions:
**		NONE
**
** Side Effects:
**
** History:
**	15-nov-90 (pete)
**		Initial Version.
**      19-Oct-98 (wanya01)
**              Add dflag in iiicspSeleteDbpriv function call.
*/
static STATUS
  authid_init()
  {
    char *d_global;
    char *tb_mode;
    char u_name[FE_MAXNAME+1];
    char *type;
        int     dflag;
	STATUS	loadIAPrivs();	/* function that loads privs into tbl field */
/* # line 348 "instacc.qsc" */	/* message */
    {
      IImessage(ERget(S_IC0076_Retrieving_InsAcc));
    }
/* # line 350 "instacc.qsc" */	/* host code */
	/* create hidden versions of all displayed fields */
/* # line 351 "instacc.qsc" */	/* inittable */
    {
      if (IItbinit(Insacc_frm,Authid_tbl,ERx("fill")) != 0) {
        IIthidecol((char *)"identifier_orig",F_Maxname);
        IIthidecol((char *)"int_type",(char *)"i2");
        IIthidecol((char *)"int_type_orig",(char *)"i2");
        IIthidecol((char *)"access_orig",(char *)"c3");
        IItfill();
      } /* IItbinit */
    }
/* # line 357 "instacc.qsc" */	/* host code */
	/* Select all "ON INSTALLATION" grants from iidbpriv (these grants
	** are detected by iidbpriv.dbname='').
	*/
	_VOID_ iiicspSelectDbpriv(ERx(""), &Iidbpriv, loadIAPrivs,&dflag);
	return OK;
  }
/*{
** Name:	loadIAPrivs - load INSTALLATION access privs into table field.
**
** Description:
**	Loadtable GRANT ACCESS ON INSTALLATION type rows from iidbpriv
**	into a table field.
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
**	15-nov-1990 (pete)
**		Initial Version.
*/
static STATUS
  loadIAPrivs(iidbpriv)
  IIDBPRIV *iidbpriv;
  {
    char *type;
    char *access;
    if ((iidbpriv->control & DB_ACCESS) != 0)
	/* this tuple specifies a Database ACCESS privilege */
	access = ((iidbpriv->flags & DB_ACCESS) != 0) ? Yes : No ;
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
	type = IC_PublicUser;
	STcopy(IC_PublicUser, iidbpriv->grantee);
	break;
    default:
	type = ERget(F_IC001F_UnknownType);
	break;
    }
/* # line 425 "instacc.qsc" */	/* loadtable */
    {
      if (IItbact(Insacc_frm,Authid_tbl,1) != 0) {
        IItcoputio((char *)"identifier",(short *)0,1,32,0,iidbpriv->grantee);
        IItcoputio((char *)"identifier_orig",(short *)0,1,32,0,
        iidbpriv->grantee);
        IItcoputio((char *)"type",(short *)0,1,32,0,type);
        IItcoputio((char *)"int_type",(short *)0,1,30,4,&iidbpriv->gtype);
        IItcoputio((char *)"int_type_orig",(short *)0,1,30,4,&iidbpriv->gtype);
        IItcoputio((char *)"access",(short *)0,1,32,0,access);
        IItcoputio((char *)"access_orig",(short *)0,1,32,0,access);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 436 "instacc.qsc" */	/* host code */
    return OK;
  }
/*{	authid_write	Write changes about a database to dbdb
**
**	Unload table field and issue GRANT/REVOKE statements.
**
**	Returns:
**		OK	database updated ok.
**		FAIL	error updating database.
**
**	History:
**	15-nov-1990 (pete)
**		Initial version.
*/
static STATUS
  authid_write()
  {
    STATUS      uval = OK;
    char name_orig[FE_MAXNAME+1];
    i4 int_type_orig;
    char access_orig[3 +1];
    char type[6 +1];
    i4 int_type;
    char access[3 +1];
    char u_name[FE_MAXNAME+1];
    i4 u_state;
    i4 u_record;
  char sbuffer[256];
    /* NOTE: assume all displayed values in table field are valid */
/* # line 474 "instacc.qsc" */	/* message */
    {
      IImessage(ERget(S_IC0079_UpdateInstAccGrnts));
    }
/* # line 476 "instacc.qsc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 478 "instacc.qsc" */	/* host code */
/* Process stDELETE rows first (UNLOADTABLE delivers them last!). */
/* # line 479 "instacc.qsc" */	/* unloadtable */
    {
      if (IItbact(Insacc_frm,Authid_tbl,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,u_name,(char *)
"identifier");
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,name_orig,(char *)
"identifier_orig");
        IItcogetio((short *)0,1,32,6 +1-1,type,(char *)"type");
        IItcogetio((short *)0,1,30,4,&int_type_orig,(char *)"int_type_\
orig");
        IItcogetio((short *)0,1,30,4,&int_type,(char *)"int_type");
        IItcogetio((short *)0,1,32,3 +1-1,access,(char *)"access");
        IItcogetio((short *)0,1,32,3 +1-1,access_orig,(char *)"access_\
orig");
        IItcogetio((short *)0,1,30,4,&u_state,(char *)"_STATE");
        IItcogetio((short *)0,1,30,4,&u_record,(char *)"_RECORD");
        IITBceColEnd();
        {
/* # line 491 "instacc.qsc" */	/* host code */
	switch (u_state)
	{
	  case stDELETE:
	    if (int_type_orig != TYPE_PUBLIC)
	    {
	        STprintf(sbuffer,
		   ERx("REVOKE %sACCESS ON INSTALLATION FROM %s '%s'"),
                  (access_orig[0] == Yes[0]) ? Empty : ERx("NO"),
		   IIICgtGetType(int_type_orig),  /* User, Group or Role */
	    	   name_orig);
	    }
	    else
	    {
	        STprintf(sbuffer,
		   ERx("REVOKE %sACCESS ON INSTALLATION FROM PUBLIC"),
                  (access_orig[0] == Yes[0]) ? Empty : ERx("NO"));
	    }
/* # line 675 "instacc.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 676 "instacc.sc" */	/* host code */
            if (FEinqerr() != OK && sqlca.sqlcode != (-E_GE0032_WARNING))
            {
                uval = FAIL;
/* # line 514 "instacc.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 515 "instacc.qsc" */	/* host code */
            }
	} 
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 519 "instacc.qsc" */	/* host code */
    if (uval == FAIL)
	goto done;
    /* process everything but stDELETE rows */
/* # line 523 "instacc.qsc" */	/* unloadtable */
    {
      if (IItbact(Insacc_frm,Authid_tbl,0) == 0) goto IItbE2;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,u_name,(char *)
"identifier");
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,name_orig,(char *)
"identifier_orig");
        IItcogetio((short *)0,1,32,6 +1-1,type,(char *)"type");
        IItcogetio((short *)0,1,30,4,&int_type_orig,(char *)"int_type_\
orig");
        IItcogetio((short *)0,1,30,4,&int_type,(char *)"int_type");
        IItcogetio((short *)0,1,32,3 +1-1,access,(char *)"access");
        IItcogetio((short *)0,1,32,3 +1-1,access_orig,(char *)"access_\
orig");
        IItcogetio((short *)0,1,30,4,&u_state,(char *)"_STATE");
        IItcogetio((short *)0,1,30,4,&u_record,(char *)"_RECORD");
        IITBceColEnd();
        {
/* # line 536 "instacc.qsc" */	/* host code */
# ifndef UNIX
	/* User, Group & Role names are case sensitive on UNIX only */
        IIUGlbo_lowerBEobject(u_name);
# endif
	switch (u_state)
	{
	  case stCHANGE:
	    /*
	    ** Delete old GRANT, and then add new one using Fall Thru...
	    */
	    if (int_type_orig != TYPE_PUBLIC)
	    {
	        STprintf(sbuffer,
		   ERx("REVOKE %sACCESS ON INSTALLATION FROM %s '%s'"),
                  (access_orig[0] == Yes[0]) ? Empty : ERx("NO"),
		   IIICgtGetType(int_type_orig),  /* User, Group or Role */
	    	   name_orig);
	    }
	    else
	    {
	        STprintf(sbuffer,
		   ERx("REVOKE %sACCESS ON INSTALLATION FROM PUBLIC"),
                  (access_orig[0] == Yes[0]) ? Empty : ERx("NO"));
	    }
/* # line 739 "instacc.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 740 "instacc.sc" */	/* host code */
            if (FEinqerr() != OK && sqlca.sqlcode != (-E_GE0032_WARNING))
            {
                uval = FAIL;
/* # line 565 "instacc.qsc" */	/* endloop */
          {
            if (1) goto IItbE2;
          }
/* # line 566 "instacc.qsc" */	/* host code */
            }
	    /* stCHANGE, fall through ... */
	  case stNEW:
	    /* Add new user to database */
	    if (int_type != TYPE_PUBLIC)
	    {
	        STprintf(sbuffer,
	        	ERx("GRANT %sACCESS ON INSTALLATION TO %s '%s'"),
                  (access_orig[0] == Yes[0]) ? Empty : ERx("NO"),
			IIICgtGetType(int_type),  /* User, Group or Role */
	    		u_name);
	    }
	    else
	    {
	    	STprintf(sbuffer,
			ERx("GRANT %sACCESS ON INSTALLATION TO PUBLIC"),
                  (access_orig[0] == Yes[0]) ? Empty : ERx("NO"));
	    }
/* # line 766 "instacc.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 767 "instacc.sc" */	/* host code */
            if (FEinqerr() != OK && sqlca.sqlcode != (-E_GE0032_WARNING))
            {
                uval = FAIL;
/* # line 591 "instacc.qsc" */	/* endloop */
          {
            if (1) goto IItbE2;
          }
/* # line 592 "instacc.qsc" */	/* host code */
            }
	    break;
	} 
        }
      } /* IItunload */
IItbE2:
      IItunend();
    }
/* # line 597 "instacc.qsc" */	/* host code */
done:
    if (uval == OK)
/* # line 786 "instacc.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 787 "instacc.sc" */	/* host code */
    else
/* # line 788 "instacc.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 789 "instacc.sc" */	/* host code */
    return uval;
  }
