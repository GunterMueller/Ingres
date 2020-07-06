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
** Name:	dbpubacc.qsc	-	Access Grants
**
** Defines:
**	IIICdpDBPublicAccess		- Database Public Access.
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
**        Add prototypes for dbpub_init() & dbpub_write() to prevent compiler
**        errors with GCC 4.0 due to conflict with implicit declaration.
**/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/* # line 45 "dbpubacc.sc" */	/* host code */
  GLOBALREF char F_Maxname[];
GLOBALREF       bool    Mgr;
GLOBALREF bool  IiicPrivSecurity; /* TRUE if user has SECURITY privilege */
  GLOBALREF IIDBPRIV Iidbpriv;
  GLOBALREF IIDATABASE Iidatabase;
GLOBALREF	char	*IC_allDBs;	/* "all databases" */
FUNC_EXTERN     STATUS  iiicspSelectDbpriv();
FUNC_EXTERN     STATUS  iiicsdSelectDatabase();
FUNC_EXTERN     STATUS  iiicsp2SelectDbpriv2();
FUNC_EXTERN	STATUS	iiicvaValidateAuthId();
FUNC_EXTERN	STATUS	IIICdpDBPublicAccess();
FUNC_EXTERN     char    *IIICgtGetType();
static		STATUS	dbacc_write();
static		STATUS	loadDBPubPrivs();
static		STATUS	dbpub_init();
static		STATUS	dbpub_write();
  static char Dbpacc_frm[]= ERx("icdbpacc");
  static char Dbacc_tbl[]= ERx("db_access");
  static bool DbaccFormAdd = FALSE;
  static char Object_name[FE_MAXNAME+1];
static  char    Empty[] = ERx("");
/*{
** Name:        IIICdpDBPublicAccess() - Databases with Grants to PUBLIC.
**
** Description:
**	Maintain list of Databases with PUBLIC access.
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
IIICdpDBPublicAccess()
  {
    STATUS  err = OK;
    char *save_menuitem;
    char *delete_menuitem;
    char *listchoices_menuitem;
    i4 work_nat;
    char *mode;
    char tmpname[FE_MAXNAME + 1];
    if (!DbaccFormAdd)
    {
	if (IIUFgtfGetForm(IIUFlcfLocateForm(), Dbpacc_frm) != OK)
	{
	    IIUGerr(S_IC0059_Get_Form_Error, UG_ERR_ERROR, 0);
	    return FAIL;
	}
	DbaccFormAdd = TRUE;
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
/* # line 143 "dbpubacc.qsc" */	/* display */
    {
      if (IIdispfrm(Dbpacc_frm,mode) == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 146 "dbpubacc.qsc" */	/* host code */
	if (dbpub_init() != OK)
	{
	    err = FAIL;
/* # line 149 "dbpubacc.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 150 "dbpubacc.qsc" */	/* host code */
	}
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 156 "dbpubacc.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,19,
                (char *)0,0);
              } /* IIiqset */
            }
/* # line 157 "dbpubacc.qsc" */	/* host code */
        if (STequal(Object_name, Dbacc_tbl))
        {
/* # line 159 "dbpubacc.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,Dbpacc_frm,Dbacc_tbl,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
/* # line 160 "dbpubacc.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,Dbpacc_frm,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 161 "dbpubacc.qsc" */	/* host code */
        }
        else
            IIUGerr(E_IC006C_NotOnTableField, UG_ERR_ERROR, 0);
          }
        } else if (IIretval() == 2) {
          {
/* # line 170 "dbpubacc.qsc" */	/* host code */
        if (save_menuitem[0] == EOS)
        {
            /* this was selected via the FRSkey */
            FTbell();
/* # line 174 "dbpubacc.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 175 "dbpubacc.qsc" */	/* host code */
        }
/* # line 177 "dbpubacc.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 178 "dbpubacc.qsc" */	/* host code */
	if (dbpub_write() != OK) {
/* # line 179 "dbpubacc.qsc" */	/* resume */
            {
              IIresfld(Dbacc_tbl);
              if (1) goto IIfdB1;
            }
/* # line 180 "dbpubacc.qsc" */	/* host code */
	}
/* # line 181 "dbpubacc.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 188 "dbpubacc.qsc" */	/* host code */
        if (listchoices_menuitem[0] == EOS)
        {
            /* this was selected via the FRSkey */
            FTbell();
/* # line 192 "dbpubacc.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 193 "dbpubacc.qsc" */	/* host code */
        }
/* # line 194 "dbpubacc.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,tmpname,19,(char *)0
                ,0);
              } /* IIiqset */
            }
/* # line 195 "dbpubacc.qsc" */	/* host code */
        if (STequal(tmpname, Dbacc_tbl))
        {
            icListPicks(ERget(FE_Database),
                            ERx("iidatabase"), ERx("name"), tmpname);
            if (tmpname[0] != EOS)
            {
/* # line 201 "dbpubacc.qsc" */	/* loadtable */
            {
              if (IItbact(Dbpacc_frm,Dbacc_tbl,1) != 0) {
                IItcoputio((char *)"database",(short *)0,1,32,0,tmpname);
                IITBceColEnd();
              } /* IItbact */
            }
/* # line 202 "dbpubacc.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,Dbpacc_frm,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 203 "dbpubacc.qsc" */	/* resume */
            {
              IIrescol(Dbacc_tbl,(char *)"access");
              if (1) goto IIfdB1;
            }
/* # line 204 "dbpubacc.qsc" */	/* host code */
            }
        }
        else
            IIUGerr(E_IC006C_NotOnTableField, UG_ERR_ERROR, 0);
          }
        } else if (IIretval() == 4) {
          {
/* # line 213 "dbpubacc.qsc" */	/* host code */
        FEhelp(ERx("icdbpacc.hlp"), ERget(S_IC0075_DBPublicAccess));
          }
        } else if (IIretval() == 5) {
          {
/* # line 219 "dbpubacc.qsc" */	/* host code */
        if (Mgr)
        {
            /* user is running Accessdb */
            i4 changed;
/* # line 224 "dbpubacc.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&changed,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 225 "dbpubacc.qsc" */	/* host code */
            if ( changed )
            {
                changed = CONFIRM(ERget(F_IC0023_DBPublicAcc), Empty);
                switch(changed)
                {
                case CONFCH_YES:
/* # line 231 "dbpubacc.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 232 "dbpubacc.qsc" */	/* host code */
		    if (dbpub_write() != OK) 
		    {
/* # line 234 "dbpubacc.qsc" */	/* resume */
            {
              IIresfld(Dbacc_tbl);
              if (1) goto IIfdB1;
            }
/* # line 235 "dbpubacc.qsc" */	/* host code */
		    }
                    /* fall through */
                case CONFCH_NO:
/* # line 238 "dbpubacc.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 239 "dbpubacc.qsc" */	/* host code */
                default:
                    break;
                }
/* # line 243 "dbpubacc.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 244 "dbpubacc.qsc" */	/* host code */
            }
        }
/* # line 247 "dbpubacc.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 254 "dbpubacc.qsc" */	/* host code */
	FEtabfnd(Dbpacc_frm, Dbacc_tbl);
          }
        } else if (IIretval() == 7) {
          {
/* # line 260 "dbpubacc.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Dbpacc_frm,Dbacc_tbl,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 8) {
          {
/* # line 266 "dbpubacc.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Dbpacc_frm,Dbacc_tbl,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 9) {
          {
/* # line 271 "dbpubacc.qsc" */	/* inquire_frs */
            {
              if (IIiqset(5,0,(char *)"",(char *)"",(char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&work_nat,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 272 "dbpubacc.qsc" */	/* host code */
        if (work_nat == 1)
        {
            /* user has changed this value. validate it */
/* # line 276 "dbpubacc.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Dbpacc_frm,Dbacc_tbl,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,Object_name,(char *)"\
database");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 278 "dbpubacc.qsc" */	/* host code */
	    /* database names are never case sensitive */
            IIUGlbo_lowerBEobject(Object_name);
	    if (!STequal(IC_allDBs, Object_name))
	    {
                /* get Database info */
		if (iiicsdSelectDatabase(Object_name, &Iidatabase) != OK)
                {
                    /* no such database */
/* # line 287 "dbpubacc.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 288 "dbpubacc.qsc" */	/* host code */
                }
	    }
            /* mark column so won't be validated next time */
/* # line 292 "dbpubacc.qsc" */	/* set_frs */
            {
              if (IIiqset(5,0,(char *)"",(char *)"",(char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 293 "dbpubacc.qsc" */	/* host code */
        }
/* # line 295 "dbpubacc.qsc" */	/* resume */
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
      if (IITBcaClmAct(Dbacc_tbl,(char *)"database",0,9) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 300 "dbpubacc.qsc" */	/* host code */
    return err;
  }
/*{
** Name:	dbpub_init - initialize entries in db_access tbl field.
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
*/
static STATUS
  dbpub_init()
  {
	STATUS	loadDBPubPrivs(); /* function that loads privs into tbl field */
/* # line 330 "dbpubacc.qsc" */	/* message */
    {
      IImessage(ERget(S_IC0077_Retrieving_DBPubAcc));
    }
/* # line 332 "dbpubacc.qsc" */	/* host code */
	/* create hidden versions of all displayed fields */
/* # line 333 "dbpubacc.qsc" */	/* inittable */
    {
      if (IItbinit(Dbpacc_frm,Dbacc_tbl,ERx("fill")) != 0) {
        IIthidecol((char *)"database_orig",F_Maxname);
        IIthidecol((char *)"access_orig",(char *)"c3");
        IItfill();
      } /* IItbinit */
    }
/* # line 337 "dbpubacc.qsc" */	/* host code */
	/* Select PUBLIC Database grants (gtype=3). */
	_VOID_ iiicsp2SelectDbpriv2(3, &Iidbpriv, loadDBPubPrivs);
	return OK;
  }
/*{
** Name:	loadDBPubPrivs - load Public Database Grants into table field.
**
** Description:
**	Load table field with GRANT ACCESS ON DATABASE TO PUBLIC type rows from
**	iidbpriv into a table field.
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
  loadDBPubPrivs(iidbpriv)
  IIDBPRIV *iidbpriv;
  {
    char *access;
    if ((iidbpriv->control & DB_ACCESS) != 0)
	/* this tuple specifies a Database ACCESS privilege */
	access = ((iidbpriv->flags & DB_ACCESS) != 0) ? Yes : No ;
    else
	return OK;	/* don't load this row. It probably specifies
			** a DIO or CPU limit, or some other database
			** priv. We only want Access privs.
			*/
    if (iidbpriv->dbname[0] == EOS)
    {
	/* empty dbname = GRANT ON INSTALLATION TO PUBLIC */
	STcopy (IC_allDBs, iidbpriv->dbname);
    }
/* # line 388 "dbpubacc.qsc" */	/* loadtable */
    {
      if (IItbact(Dbpacc_frm,Dbacc_tbl,1) != 0) {
        IItcoputio((char *)"database",(short *)0,1,32,0,iidbpriv->dbname);
        IItcoputio((char *)"database_orig",(short *)0,1,32,0,iidbpriv->dbname);
        IItcoputio((char *)"access",(short *)0,1,32,0,access);
        IItcoputio((char *)"access_orig",(short *)0,1,32,0,access);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 396 "dbpubacc.qsc" */	/* host code */
    return OK;
  }
/*{	dbpub_write	Write changes about Public GRANTs to dbdb
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
  dbpub_write()
  {
    STATUS      uval = OK;
    char dbname_orig[FE_MAXNAME+1];
    char db_name[FE_MAXNAME+1];
    char access_orig[3 +1];
    char access[3 +1];
    i4 db_state;
  char sbuffer[256];
    /* NOTE: assume all displayed values in table field are valid */
/* # line 428 "dbpubacc.qsc" */	/* message */
    {
      IImessage(ERget(S_IC0078_UpdatePubAccGrnts));
    }
/* # line 430 "dbpubacc.qsc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 432 "dbpubacc.qsc" */	/* host code */
    /* process stDELETE rows */
/* # line 433 "dbpubacc.qsc" */	/* unloadtable */
    {
      if (IItbact(Dbpacc_frm,Dbacc_tbl,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,db_name,(char *)
"database");
        IItcogetio((short *)0,1,32,3 +1-1,access,(char *)"access");
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,dbname_orig,(char *)
"database_orig");
        IItcogetio((short *)0,1,32,3 +1-1,access_orig,(char *)"access_\
orig");
        IItcogetio((short *)0,1,30,4,&db_state,(char *)"_STATE");
        IITBceColEnd();
        {
/* # line 438 "dbpubacc.qsc" */	/* host code */
        sbuffer[0] = EOS;
        switch (db_state)
        {
        case stDELETE:
            /* Delete the Access, with the original database name */
	    if (STbcompare(IC_allDBs, 0, dbname_orig, 0, TRUE) != 0)
            {
                /* REVOKE from particular database */
                STprintf(sbuffer,
                        ERx("REVOKE %sACCESS ON DATABASE %s FROM PUBLIC"),
		  (access_orig[0] == Yes[0]) ? Empty : ERx("NO"),
                        dbname_orig);
            }
	    else
	    {
                /* blank database name means ON INSTALLATION */
                STprintf(sbuffer,
                        ERx("REVOKE %sACCESS ON INSTALLATION FROM PUBLIC"),
                  (access_orig[0] == Yes[0]) ? Empty : ERx("NO"));
	    }
/* # line 620 "dbpubacc.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 621 "dbpubacc.sc" */	/* host code */
            if ( FEinqerr() != OK && sqlca.sqlcode != (-E_GE0032_WARNING))
            {
                uval = FAIL;
/* # line 466 "dbpubacc.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 467 "dbpubacc.qsc" */	/* host code */
            }
            break;
        }
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 474 "dbpubacc.qsc" */	/* host code */
    if (uval == FAIL)
        goto done;
    /* process all but stDELETE statements */
/* # line 478 "dbpubacc.qsc" */	/* unloadtable */
    {
      if (IItbact(Dbpacc_frm,Dbacc_tbl,0) == 0) goto IItbE2;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,db_name,(char *)
"database");
        IItcogetio((short *)0,1,32,3 +1-1,access,(char *)"access");
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,dbname_orig,(char *)
"database_orig");
        IItcogetio((short *)0,1,32,3 +1-1,access_orig,(char *)"access_\
orig");
        IItcogetio((short *)0,1,30,4,&db_state,(char *)"_STATE");
        IITBceColEnd();
        {
/* # line 483 "dbpubacc.qsc" */	/* host code */
        sbuffer[0] = EOS;
        switch (db_state)
        {
        case stCHANGE:
            /* Delete old access, and add new one */
	    if (STbcompare(IC_allDBs, 0, dbname_orig, 0, TRUE) != 0)
            {
                /* REVOKE from particular database */
                STprintf(sbuffer,
                        ERx("REVOKE %sACCESS ON DATABASE %s FROM PUBLIC"),
                  (access_orig[0] == Yes[0]) ? Empty : ERx("NO"),
                        dbname_orig);
            }
	    else
	    {
                STprintf(sbuffer,
                        ERx("REVOKE %sACCESS ON INSTALLATION FROM PUBLIC"),
                  (access_orig[0] == Yes[0]) ? Empty : ERx("NO"));
	    }
/* # line 675 "dbpubacc.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 676 "dbpubacc.sc" */	/* host code */
            if ( FEinqerr() != OK && sqlca.sqlcode != (-E_GE0032_WARNING))
            {
                uval = FAIL;
/* # line 510 "dbpubacc.qsc" */	/* endloop */
          {
            if (1) goto IItbE2;
          }
/* # line 511 "dbpubacc.qsc" */	/* host code */
            }
            /* fall through */
        case stNEW:
            if (db_name[0] != EOS)
            {
		if (STbcompare(IC_allDBs, 0, db_name, 0, TRUE) == 0)
		{
		    /* Issue GRANT ON INSTALLATION */
                    STprintf(sbuffer,
                        ERx("GRANT %sACCESS ON INSTALLATION TO PUBLIC"),
                        (access[0] == Yes[0]) ? Empty : ERx("NO"));
		}
		else
		{
                    STprintf(sbuffer,
                        ERx("GRANT %sACCESS ON DATABASE %s TO PUBLIC"),
                        (access[0] == Yes[0]) ? Empty : ERx("NO"),
                        db_name);
		}
/* # line 703 "dbpubacc.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 704 "dbpubacc.sc" */	/* host code */
                if ( FEinqerr() != OK && sqlca.sqlcode != (-E_GE0032_WARNING))
                {
                        uval = FAIL;
/* # line 538 "dbpubacc.qsc" */	/* endloop */
          {
            if (1) goto IItbE2;
          }
/* # line 539 "dbpubacc.qsc" */	/* host code */
                }
            }
            break;
        default:
            break;
        }
        }
      } /* IItunload */
IItbE2:
      IItunend();
    }
/* # line 550 "dbpubacc.qsc" */	/* host code */
done:
    if (uval == OK)
/* # line 726 "dbpubacc.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 727 "dbpubacc.sc" */	/* host code */
    else
/* # line 728 "dbpubacc.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 729 "dbpubacc.sc" */	/* host code */
    return uval;
  }
