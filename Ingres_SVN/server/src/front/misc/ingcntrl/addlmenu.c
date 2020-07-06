# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<er.h>
#include	<st.h>
#include	<me.h>
#include	<gl.h>
#include	<sl.h>
#include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
#include	<uf.h>
#include	<erug.h>
#include        <generr.h>
#include        <stdprmpt.h>
#include	<ui.h>
#include	"eric.h"
#include	"global.h"
/**
** Name:	addlmenu.qc -	AccessDB Location Name Frame.
**
** Description:
**	Contains the frame and associated code for creating and modifying
**	location names.  Defines:
**
**	addlocform()	- create a location.
**	editlocform()	- edit a Location.
**
** History:
**	Revision 6.0  88/04  peterk
**	16-sep-87 (daver) 
**		Removed location deleting code, as locations can not be
**		deleted in 6.0. Also made FE_MAXNAME form changes.
**	03-nov-88 (kathryn)	bug# 3296
**		Removed area_err(). When a new location name is being added
**		and the area specified is not valid for the use specified, 
**		the loc_info variable is no longer automatically changed from 
**		_Yes to _No.  Also, If no is entered for all variables 
**		(DB's, Jrnls, Chkpts) and a save is entered the user will now
**		be prompted to ensure he wants to save a location that can't
**		be used.
**	12/04/88 (dkh) - Changed where clause to place variable on
**			 right hand side of "=" operator.
**	11-may-89 (mgw)
**		Added TERMINATOR handling for II_DUMP location.
**      27-sep-1989 (jennifer)
**             Added new SQL statement support for B1/C2 security requirements.
**             For this file CREATE LOCATION and ALTER LOCATION statements 
**             replaced inserts/replaces into iilocation catalog directly.
**	15-Dec-89 (pete) - Fix for jupbug 7992. Add (validate=0,activate=0) to
**		End menuitem.
**	~1-Jan-90 (pete) - Allow selection of Help & End even when on
**			invalid field; check for unsaved changed when
**			user hits menuitem End.
**	11-apr-1990 (pete)
**		change error message for bad location name to give more info.
**	23-apr-1990 (pete)
**		fix 6.3 bug 21342 with bad explanations for Help & End.
**	19-oct-1990 (pete)
**		Merge in changes done by Jennifer Lyerla in 9/89 for B1 proj.
**	13-nov-1990 (pete)
**		Make more changes to be consistent with work done in
**		db.qsc & user.qsc.
**	5-aug-1992 (jonb)
**		Add quotes to CREATE LOCATION statement.
**      7-dec-1992 (jonb)
**              Change "Sort" locations to "work" locations, add support
**              for ML sort.
**      18-aug-1993 (kellyp)
**              Added CANCEL menuitem to the CREATE screen.
**	17-sep-1993 (kellyp)
**		Removed CANCEL menuitem from the CREATE screen
**	20-sep-1993 (robf)
**              Query standard catalog (iilocation_info) not internal catalog
**	        (iilocations). This allows users without QUERY_SYSCAT priv
**	        to use accessdb/catalogdb
**	20-dec-1994 (nanpr01)
**		Bug # 66003 : allow changing and modifying the labels.
**	05-may-1995 (harpa06)
**		Allowed delimited location names to be created.
**	16-aug-95 (emmag)
**		Print an error when illegal location areas are specified.
**	16-nov-95 (pchang)
**		Rewrote location area verification code such that when creating
**		/editing a new location, we now verify that the appropriate 
**		Ingres paths have been correctly setup for the chosen usage of 
**		the location.  (B72363)
**		Fixed the problem of not being able to alter location specified
**		with delimited names.
**	01-Dec-1995 (mckba01)
**		Change above also fixed 66298, and 66309, except when
**		no usage type is specified for a location. In functions
**		loc_write(), Store original area name and restore after 
**		LOfroms to avoid displaying and translating incorrect 
**		area name.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	09-Apr-2001 (jenjo02)
**	    Add support for RAW locations.
**	11-May-2001 (jenjo02)
**	    "rawblocks" moved from iilocations to iiextend, "rawpct"
**	    in iilocations replaces "rawblocks" as input field.
**	23-May-2001 (jenjo02)
**	    Removed LO, replacing chk_loc_area() with calls to
**	    iiicaeAreaExists() which invokes a new internal procedure
**	    in the server to test for the existence of location
**	    paths. This (finally) allows non-Ingres, but privileged,
**	    users to create and extend locations, just like the
**	    documentation says.
**	15-Oct-2001 (jenjo02)
**	    Missing area directories are now made in the server
**	    by CREATE/ALTER LOCATION, relieving us from the 
**	    responsibility of checking existence.
**    25-Oct-2005 (hanje04)
**        Add prototype for check_loc_name() to prevent compiler
**        errors with GCC 4.0 due to conflict with implicit declaration.
**/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/* # line 126 "addlmenu.sc" */	/* host code */
GLOBALREF       bool    Mgr;
GLOBALREF bool  IiicPrivSysadmin;  /* TRUE if user has SYSTEM_ADMIN privilege */
static STATUS	loc_init();
static STATUS	loc_write();
static STATUS	check_loc_name();
  typedef struct {
    char *li_databases;
    char *li_journals;
    char *li_checkpts;
    char *li_dumps;
    char *li_work;
    char li_area[DB_AREA_MAX+1];
    bool li_new;
    i4 li_status;
    i4 li_rawpct;
    i4 li_rawstart;
    i4 li_rawblocks;
  } LOC_INFO;
  static char *NewLocfrm = ERx("locnew");
  static char *OldLocfrm = ERx("nlocation");
  static char *LocDbTbl = ERx("dbases");
static		bool    Dmps_visible = TRUE;
  static char *Save_menuitem = ERx("");
  static char *Mode;
/*}
** Name:        LOC_USAGE - Location usage.
*/
typedef struct{
        i4              id;     /* symbolic name of usage */
        char            *name;  /* official INGRES/SQL name of loc usage */
} LOC_USAGE;
static LOC_USAGE Loc_usages[] =
{
        {DU_DBS_LOC,     ERx("DATABASE")},
        {DU_JNLS_LOC,    ERx("JOURNAL")},
        {DU_CKPS_LOC,    ERx("CHECKPOINT")},
        {DU_DMPS_LOC,    ERx("DUMP")},
        {DU_WORK_LOC,    ERx("WORK")},  
        {0,    NULL}
};
static bool LocFrmInit = FALSE;
  static char EditLocTitle[80]= ERx("");
STATUS
  addlocform()
  {
    LOC_INFO loc_info;
	if (!(Mgr && IiicPrivSysadmin))
	    return FAIL;
	if (!LocFrmInit)
	{
	    /* User can make changes if running Accessdb 
	    ** (as opposed to Catalogdb) & has SYSTEM_ADMIN Priv.
	    */
	    if (IIUFgtfGetForm(IIUFlcfLocateForm(), NewLocfrm) != OK)
		IIUGerr(S_IC0059_Get_Form_Error, UG_ERR_FATAL, 0);
	    LocFrmInit = TRUE;
	}
	if (loc_init(NewLocfrm, (char *) NULL, &loc_info) != OK)
		return FAIL;
/* # line 204 "addlmenu.qsc" */	/* display */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      if (IIdispfrm(NewLocfrm,(char *)"u") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 207 "addlmenu.qsc" */	/* putform */
        {
          if (IIfsetio(NewLocfrm) != 0) {
            IIputfldio((char *)"locname",(short *)0,1,32,0,ERx(""));
            IIputfldio((char *)"area",(short *)0,1,32,0,loc_info.li_area);
            IIputfldio((char *)"dbs",(short *)0,1,32,0,loc_info.li_databases);
            IIputfldio((char *)"jnls",(short *)0,1,32,0,loc_info.li_journals);
            IIputfldio((char *)"ckps",(short *)0,1,32,0,loc_info.li_checkpts);
            IIputfldio((char *)"dmps",(short *)0,1,32,0,loc_info.li_dumps);
            IIputfldio((char *)"work",(short *)0,1,32,0,loc_info.li_work);
            IIputfldio((char *)"rawpct",(short *)0,1,30,4,&loc_info.li_rawpct);
          } /* IIfsetio */
        }
/* # line 218 "addlmenu.qsc" */	/* resume */
        {
          IIresfld((char *)"locname");
          if (1) goto IIfdB1;
        }
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 225 "addlmenu.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 227 "addlmenu.qsc" */	/* host code */
		if (loc_write(NewLocfrm, loc_info.li_new) == FAIL)
/* # line 228 "addlmenu.qsc" */	/* resume */
            {
              IIresfld((char *)"area");
              if (1) goto IIfdB1;
            }
/* # line 229 "addlmenu.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 2) {
          {
            char l_name[FE_MAXNAME + 1];
/* # line 236 "addlmenu.qsc" */	/* getform */
            {
              if (IIfsetio(NewLocfrm) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,l_name,(char *)"\
locname");
              } /* IIfsetio */
            }
/* # line 237 "addlmenu.qsc" */	/* host code */
	    if (l_name[0] != EOS && check_loc_name(l_name) == OK)
/* # line 238 "addlmenu.qsc" */	/* resume */
            {
              IIresfld((char *)"area");
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 245 "addlmenu.qsc" */	/* host code */
	    FEhelp(ERx("icnlocfm.hlp"), ERget(S_IC0004_Add_Locations_Form));
          }
        } else if (IIretval() == 4) {
          {
            i4 changed;
/* # line 254 "addlmenu.qsc" */	/* host code */
            /* user is running Accessdb */
/* # line 255 "addlmenu.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&changed,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 256 "addlmenu.qsc" */	/* host code */
            if ( changed )
            {
            char l_name[FE_MAXNAME+1];
/* # line 260 "addlmenu.qsc" */	/* getform */
            {
              if (IIfsetio(NewLocfrm) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,l_name,(char *)
"locname");
              } /* IIfsetio */
            }
/* # line 262 "addlmenu.qsc" */	/* host code */
                changed = CONFIRM(l_name, ERget(F_IC0021_Location));
		if (changed == CONFCH_YES)
                {
/* # line 265 "addlmenu.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 266 "addlmenu.qsc" */	/* host code */
		    if (loc_write(NewLocfrm, loc_info.li_new) == FAIL)
/* # line 267 "addlmenu.qsc" */	/* resume */
            {
              IIresfld((char *)"area");
              if (1) goto IIfdB1;
            }
/* # line 268 "addlmenu.qsc" */	/* host code */
		}
		else if (changed != CONFCH_NO)
		{
/* # line 271 "addlmenu.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 272 "addlmenu.qsc" */	/* host code */
		}
	    }
/* # line 274 "addlmenu.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(FE_Save),ERget(F_FE0117_SaveExpl),2,0,1) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,0,1) == 0) goto IIfdE1;
      if (IIFRafActFld((char *)"locname",0,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0101_ExplKeyHelp),0,0,3) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,4) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,4) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 278 "addlmenu.qsc" */	/* host code */
	return OK;
  }
STATUS
  editlocform(loc)
  char *loc;
  {
    LOC_INFO loc_info;
    i4 nrows;
	if (EditLocTitle[0] == EOS)
	{
	    if (IIUFgtfGetForm(IIUFlcfLocateForm(), OldLocfrm) != OK)
		    IIUGerr(S_IC0059_Get_Form_Error, UG_ERR_FATAL, 0);
	    /* User can make changes if running Accessdb 
	    ** (as opposed to Catalogdb) & has SYSTEM_ADMIN Priv.
	    */
	    if (Mgr && IiicPrivSysadmin)
	    {
		Save_menuitem = ERget(FE_Save);
		Mode = ERx("update");
		STlcopy(ERget(S_IC0097_Loc_Edit_Title), EditLocTitle, 
					sizeof(EditLocTitle));
	    }
	    else
	    {
		Mode = ERx("read");
		if (Mgr)
		{
		    STlcopy(ERget(S_IC0095_Loc_Info_Title), EditLocTitle, 
					sizeof(EditLocTitle));
		}
		else
		{
		    STlcopy(ERget(S_IC0094_Loc_Info_Title), EditLocTitle, 
					sizeof(EditLocTitle));
		}
	    }
	}
	if (loc_init(OldLocfrm, loc, &loc_info) != OK)
		return FAIL;
/* # line 325 "addlmenu.qsc" */	/* display */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      if (IIdispfrm(OldLocfrm,Mode) == 0) goto IIfdE2;
      goto IImuI2;
IIfdI2:;
      if (IIfsetio((char *)0) == 0) goto IIfdE2;
      IIputfldio((char *)"title",(short *)0,1,32,0,EditLocTitle);
      {
/* # line 328 "addlmenu.qsc" */	/* host code */
	    if (Mgr && IiicPrivSysadmin)
	    {
/* # line 330 "addlmenu.qsc" */	/* set_frs */
        {
          if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(17,(char *)"dbs",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(53,(char *)"dbs",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(17,(char *)"jnls",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(53,(char *)"jnls",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(17,(char *)"ckps",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(53,(char *)"ckps",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(17,(char *)"work",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(53,(char *)"work",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(17,(char *)"dmps",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(53,(char *)"dmps",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(17,(char *)"area",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(53,(char *)"area",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(54,(char *)"rawstart",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(54,(char *)"rawblocks",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(54,(char *)"rawpct",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(17,(char *)"rawpct",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(53,(char *)"rawpct",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 340 "addlmenu.qsc" */	/* host code */
	    }
	    else
	    {
/* # line 343 "addlmenu.qsc" */	/* set_frs */
        {
          if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(53,(char *)"dbs",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(17,(char *)"dbs",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(53,(char *)"jnls",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(17,(char *)"jnls",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(53,(char *)"ckps",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(17,(char *)"ckps",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(53,(char *)"work",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(17,(char *)"work",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(53,(char *)"dmps",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(17,(char *)"dmps",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(53,(char *)"area",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(17,(char *)"area",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(54,(char *)"rawstart",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(54,(char *)"rawblocks",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(54,(char *)"rawpct",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(53,(char *)"rawpct",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(17,(char *)"rawpct",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 353 "addlmenu.qsc" */	/* host code */
	    }
/* # line 355 "addlmenu.qsc" */	/* putform */
        {
          if (IIfsetio(OldLocfrm) != 0) {
            IIputfldio((char *)"locname",(short *)0,1,32,0,loc);
            IIputfldio((char *)"area",(short *)0,1,32,0,loc_info.li_area);
            IIputfldio((char *)"dbs",(short *)0,1,32,0,loc_info.li_databases);
            IIputfldio((char *)"jnls",(short *)0,1,32,0,loc_info.li_journals);
            IIputfldio((char *)"ckps",(short *)0,1,32,0,loc_info.li_checkpts);
            IIputfldio((char *)"dmps",(short *)0,1,32,0,loc_info.li_dumps);
            IIputfldio((char *)"work",(short *)0,1,32,0,loc_info.li_work);
            IIputfldio((char *)"rawpct",(short *)0,1,30,4,&loc_info.li_rawpct);
            IIputfldio((char *)"rawstart",(short *)0,1,30,4,&
            loc_info.li_rawstart);
            IIputfldio((char *)"rawblocks",(short *)0,1,30,4,&
            loc_info.li_rawblocks);
          } /* IIfsetio */
        }
/* # line 368 "addlmenu.qsc" */	/* inquire_frs */
        {
          if (IIiqset(3,0,OldLocfrm,(char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,30,4,&nrows,31,LocDbTbl,0);
          } /* IIiqset */
        }
/* # line 369 "addlmenu.qsc" */	/* host code */
	    if (nrows != 0)
	    {
		/* Already extended to a database */
/* # line 372 "addlmenu.qsc" */	/* set_frs */
        {
          if (IIiqset(2,0,OldLocfrm,(char *)0, (char *)0) != 0) {
            IIstfsio(53,(char *)"area",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(17,(char *)"area",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 375 "addlmenu.qsc" */	/* host code */
		/* If extended raw location, can't change usage or rawpct */
		if ( loc_info.li_rawblocks )
/* # line 377 "addlmenu.qsc" */	/* set_frs */
        {
          if (IIiqset(2,0,OldLocfrm,(char *)0, (char *)0) != 0) {
            IIstfsio(53,(char *)"dbs",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(17,(char *)"dbs",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(53,(char *)"jnls",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(17,(char *)"jnls",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(53,(char *)"ckps",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(17,(char *)"ckps",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(53,(char *)"work",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(17,(char *)"work",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(53,(char *)"dmps",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(17,(char *)"dmps",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(53,(char *)"rawpct",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(17,(char *)"rawpct",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(54,(char *)"rawstart",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(54,(char *)"rawblocks",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 386 "addlmenu.qsc" */	/* host code */
		else
		    /* Cooked, not raw, hide raw fields */
/* # line 388 "addlmenu.qsc" */	/* set_frs */
        {
          if (IIiqset(2,0,OldLocfrm,(char *)0, (char *)0) != 0) {
            IIstfsio(54,(char *)"rawpct",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(54,(char *)"rawstart",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(54,(char *)"rawblocks",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 392 "addlmenu.qsc" */	/* host code */
	    }
/* # line 393 "addlmenu.qsc" */	/* resume */
        {
          IIresfld((char *)"dbs");
          if (1) goto IIfdB2;
        }
      }
IIfdB2:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 400 "addlmenu.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB2;
              } /* IIchkfrm */
            }
/* # line 401 "addlmenu.qsc" */	/* host code */
		if (loc_write(OldLocfrm, loc_info.li_new) == FAIL)
/* # line 402 "addlmenu.qsc" */	/* resume */
            {
              IIresfld((char *)"dbs");
              if (1) goto IIfdB2;
            }
/* # line 403 "addlmenu.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 409 "addlmenu.qsc" */	/* host code */
	    if (Mgr)
		FEhelp(ERx("icelocfm.hlp"),ERget(S_IC0114_Edit_Locations_Form));
	    else
		FEhelp(ERx("icrlocfm.hlp"),
				ERget(S_IC0115_Exam_Locations_Form));
          }
        } else if (IIretval() == 3) {
          {
            i4 changed;
/* # line 422 "addlmenu.qsc" */	/* host code */
            if (Save_menuitem[0] == EOS)
/* # line 423 "addlmenu.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
/* # line 425 "addlmenu.qsc" */	/* host code */
            /* user is running Accessdb */
/* # line 426 "addlmenu.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&changed,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 427 "addlmenu.qsc" */	/* host code */
            if ( changed != 0 )
            {
            char l_name[FE_MAXNAME+1];
/* # line 431 "addlmenu.qsc" */	/* getform */
            {
              if (IIfsetio(OldLocfrm) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,l_name,(char *)
"locname");
              } /* IIfsetio */
            }
/* # line 432 "addlmenu.qsc" */	/* host code */
                changed = CONFIRM(l_name, ERget(F_IC0021_Location));
		if (changed == CONFCH_YES)
                {
/* # line 435 "addlmenu.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB2;
              } /* IIchkfrm */
            }
/* # line 436 "addlmenu.qsc" */	/* host code */
		    if (loc_write(OldLocfrm, loc_info.li_new) == FAIL)
/* # line 437 "addlmenu.qsc" */	/* resume */
            {
              IIresfld((char *)"area");
              if (1) goto IIfdB2;
            }
/* # line 438 "addlmenu.qsc" */	/* host code */
		}
		else if (changed != CONFCH_NO)
		{
/* # line 441 "addlmenu.qsc" */	/* resume */
            {
              if (1) goto IIfdB2;
            }
/* # line 442 "addlmenu.qsc" */	/* host code */
		}
	    }
/* # line 444 "addlmenu.qsc" */	/* breakdisplay */
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
      if (IIinitmu() == 0) goto IIfdE2;
      if (IInmuact(Save_menuitem,ERget(F_FE0117_SaveExpl),2,0,1) == 0) goto 
      IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,0,1) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0101_ExplKeyHelp),0,0,2) == 0) 
      goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,2) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,3) == 0) goto 
      IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,3) == 0) goto IIfdE2;
      if (IIendmu() == 0) goto IIfdE2;
      goto IIfdI2;
IIfdE2:;
      IIendfrm();
    }
/* # line 448 "addlmenu.qsc" */	/* host code */
	return OK;
  }
/* loc_init -- initialize location form */
static bool 	Dmp_checked = FALSE;
static STATUS
  loc_init(frmname, locname, linfo)
  char *frmname;
  char *locname;
  register LOC_INFO *linfo;
  {
  char loc_name[DB_MAXNAME+1];
  char db_name[DB_MAXNAME+1];
  char db_usage[4], work_usage[4], dump_usage[4], jnl_usage[4], ckp_usage[4];
	STATUS		stat;
	/* If location 'ii_dump' exists, then make field "dmps" visible. */
	if (!Dmp_checked)
	{
/* # line 723 "addlmenu.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select location_name from iilocation_info where location_name='ii_dum\
p' or location_name='II_DUMP'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,loc_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 727 "addlmenu.sc" */	/* host code */
	    if (FEinqerr() != OK)
		return FAIL;
	    if (FEinqrows() <= 0)
	    {
		/* make Dumps field Invisible */
/* # line 485 "addlmenu.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,frmname,(char *)0, (char *)0) != 0) {
        IIstfsio(54,(char *)"dmps",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 486 "addlmenu.qsc" */	/* host code */
		Dmps_visible = FALSE;
	    }
	    else
	    {
		/* make Dumps field visible */
/* # line 491 "addlmenu.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,frmname,(char *)0, (char *)0) != 0) {
        IIstfsio(54,(char *)"dmps",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 492 "addlmenu.qsc" */	/* host code */
		Dmps_visible = TRUE;
	    }
	    Dmp_checked = TRUE;
	}
	if (locname == NULL)
	{
	    /* new location being added */
/* # line 501 "addlmenu.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,frmname,(char *)0, (char *)0) != 0) {
        IIstfsio(17,(char *)"locname",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(53,(char *)"locname",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 503 "addlmenu.qsc" */	/* host code */
	    /* default status information */
	    linfo->li_databases  = Yes;
	    linfo->li_journals = No;
	    linfo->li_checkpts = No;
	    linfo->li_status	= 0;
	    linfo->li_dumps = No;
	    linfo->li_work = No;
	    linfo->li_new   = TRUE;
	    *linfo->li_area = EOS;
	    linfo->li_rawpct   = 0;
	    linfo->li_rawstart   = 0;
	    linfo->li_rawblocks   = 0;
	}
	else
	{
/* # line 518 "addlmenu.qsc" */	/* inittable */
    {
      if (IItbinit(frmname,LocDbTbl,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 520 "addlmenu.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,frmname,(char *)0, (char *)0) != 0) {
        IIstfsio(17,(char *)"locname",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
        IIstfsio(53,(char *)"locname",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 523 "addlmenu.qsc" */	/* host code */
	    /* lookup location name */
/* # line 802 "addlmenu.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select location_area, data_usage, jrnl_usage, ckpt_usage, work_usage,\
 dump_usage, raw_pct from iilocation_info where location_name=");
    IIputdomio((short *)0,1,32,0,locname);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_AREA_MAX+1-1,linfo->li_area);
      IIgetdomio((short *)0,1,32,3,db_usage);
      IIgetdomio((short *)0,1,32,3,jnl_usage);
      IIgetdomio((short *)0,1,32,3,ckp_usage);
      IIgetdomio((short *)0,1,32,3,work_usage);
      IIgetdomio((short *)0,1,32,3,dump_usage);
      IIgetdomio((short *)0,1,30,4,&linfo->li_rawpct);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 810 "addlmenu.sc" */	/* host code */
	    if ((stat = FEinqerr()) != OK)
		    return stat;
	    /* extract status bits (this could just as well use
	    ** linfo->li_status)
	    */
	    linfo->li_databases = (*db_usage=='Y')  ? Yes : No;
	    linfo->li_journals  = (*jnl_usage=='Y') ? Yes : No;
	    linfo->li_checkpts  = (*ckp_usage=='Y') ? Yes : No;
	    linfo->li_dumps     = (*dump_usage=='Y') ? Yes : No;
	    linfo->li_work      = (*work_usage=='Y') ? Yes : No;
	    linfo->li_new = FALSE;
	    /* clear particular bits in status so can reset later according
	    ** to what user enters on screen.
	    */
	    linfo->li_status=0;
	    /* load dbases table */
/* # line 826 "addlmenu.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select database_name, raw_start, raw_blocks from iiextend_info where \
location_name=");
    IIputdomio((short *)0,1,32,0,locname);
    IIwritio(0,(short *)0,1,32,0,(char *)"order by database_name");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,db_name);
      IIgetdomio((short *)0,1,30,4,&linfo->li_rawstart);
      IIgetdomio((short *)0,1,30,4,&linfo->li_rawblocks);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 834 "addlmenu.sc" */	/* host code */
	    {
/* # line 562 "addlmenu.qsc" */	/* loadtable */
    {
      if (IItbact(frmname,LocDbTbl,1) != 0) {
        IItcoputio((char *)"dbname",(short *)0,1,32,0,db_name);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 563 "addlmenu.qsc" */	/* host code */
	    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
	    if ((stat = FEinqerr()) != OK)
		    return stat;
	}
	return OK;
  }
/* check_loc_name -- check a location name for validity. */
static STATUS
  check_loc_name(locname)
  char *locname;
  {
  char db_usage[4];
	STATUS		stat;
	/* make sure location name is legal */
	if (STtrmwhite(locname) <= 0 || 
            (IIUGdlm_ChkdlmBEobject(locname,NULL,FALSE)==UI_BOGUS_ID))
	{
	        i4      maxn = DB_MAXNAME;
        	IIUGerr(E_IC002E_bad_name, 0, 2, (PTR) locname, (PTR)&maxn);
		return FAIL;
	}
	IIUGlbo_lowerBEobject(locname);
	/* lookup location name */
/* # line 872 "addlmenu.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select data_usage from iilocation_info where location_name=");
    IIputdomio((short *)0,1,32,0,locname);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,3,db_usage);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 876 "addlmenu.sc" */	/* host code */
	if ((stat = FEinqerr()) != OK)
		return stat;
	if (FEinqrows() > 0)
	{
		IIUGerr(E_IC0120_Loc_Already_Exists, UG_ERR_ERROR, 0);
		return FAIL;
	}
	return OK;
  }
/* loc_write -- */
static STATUS
  loc_write(frmname, new)
  char *frmname;
bool	new;
  {
    char databases[4];
    char journals[4];
    char checkpts[4];
    char dumps[4];
    char wrk[4];
    char usagebuf[512];
        i4      i;
    char locname[DB_MAXNAME+1];
    char area[DB_AREA_MAX+1];
    ER_MSGID msgid;
    i4 rawpct;
    char *bad_path;
	char    *value = NULL;
  char sbuffer[1024];
	i4 	loc_stat = 0, autocommit_off = FALSE;
	char	crawpct[8];
	if (new)
	    msgid = S_IC0008_Location_Appended;
	else
	    msgid = S_IC0072_Location_Updated;
/* # line 644 "addlmenu.qsc" */	/* getform */
    {
      if (IIfsetio(frmname) != 0) {
        IIgetfldio((short *)0,1,32,DB_AREA_MAX+1-1,area,(char *)"area");
        IIgetfldio((short *)0,1,32,DB_MAXNAME+1-1,locname,(char *)"loc\
name");
        IIgetfldio((short *)0,1,32,3,databases,(char *)"dbs");
        IIgetfldio((short *)0,1,32,3,journals,(char *)"jnls");
        IIgetfldio((short *)0,1,32,3,checkpts,(char *)"ckps");
        IIgetfldio((short *)0,1,32,3,dumps,(char *)"dmps");
        IIgetfldio((short *)0,1,32,3,wrk,(char *)"work");
        IIgetfldio((short *)0,1,30,4,&rawpct,(char *)"rawpct");
      } /* IIfsetio */
    }
/* # line 651 "addlmenu.qsc" */	/* host code */
	if ( *databases == *Yes )
	    loc_stat |= DU_DBS_LOC;
	if ( *journals == *Yes )
	    loc_stat |= DU_JNLS_LOC;
	if ( *checkpts == *Yes )
	    loc_stat |= DU_CKPS_LOC;
	if ( *wrk == *Yes )
	    loc_stat |= DU_WORK_LOC;
	if ( Dmps_visible && *dumps == *Yes )
	    loc_stat |= DU_DMPS_LOC;
	if (!new)   /* If this is an edit... */
	{
    i4 ch, chraw;
/* # line 666 "addlmenu.qsc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,frmname,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&ch,20,(char *)"area",0);
        IIiqfsio((short *)0,1,30,4,&chraw,20,(char *)"rawpct",0);
      } /* IIiqset */
    }
/* # line 668 "addlmenu.qsc" */	/* host code */
	    if ( ch || chraw )
	    {
		/* we've changed the "area" or "rawpct" field in a location 
		** definition.  This isn't supported by "alter location", 
		** alas, so we'll use "drop location" and then create a whole
		** new area. 
		*/
		/*
		** ACCESSDB runs with autocommit ON. If the DROP LOCATION
		** succeeds and the upcoming CREATE fails, the DROP
		** will have been committed and we'll be left without
		** a location. To preserve the original LOCATION in
		** the event of CREATE failure, turn off autocommit,
		** then turn it back on after the CREATE.
		*/
/* # line 965 "addlmenu.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set AUTOCOMMIT OFF");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 966 "addlmenu.sc" */	/* host code */
		STprintf(sbuffer, ERx("DROP LOCATION %s"), locname);
/* # line 967 "addlmenu.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 968 "addlmenu.sc" */	/* host code */
		if (sqlca.sqlcode < 0 && sqlca.sqlcode != (-E_GE0032_WARNING)) 
		{
/* # line 970 "addlmenu.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set AUTOCOMMIT on");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 971 "addlmenu.sc" */	/* host code */
		    return (FAIL);
		}
		new = TRUE;
		autocommit_off = TRUE;
	    }
	}
	/* build comma-separated list of LOC_USAGEs */
	for (i = 0, usagebuf[0] = EOS; Loc_usages[i].name != NULL; i++)
	{
	    if (loc_stat & Loc_usages[i].id)
	    {
		if (usagebuf[0] != EOS)
		    STcat(usagebuf, ERx(",")); /*don't do this for first one*/
		STcat(usagebuf, Loc_usages[i].name);
	    }
	}
	if (new)
	{
	    /* CREATE new location */
	    STprintf(sbuffer, ERx("CREATE LOCATION %s WITH AREA = '%s', "),
		    locname, area);
	}
	else
	{
	    char qlocname[FE_MAXNAME + 1];
	    /* ALTER existing location */
	    STpolycat(3, ERx("\""), locname, ERx("\""), qlocname);
	    STprintf(sbuffer, ERx("ALTER LOCATION %s WITH "), qlocname);
	}
	if (usagebuf[0] != EOS)
	{
	    STcat(sbuffer, ERx("USAGE = ("));
	    STcat(sbuffer, usagebuf);
	    STcat(sbuffer, ERx(")"));
	}
	else
	{
	    STcat(sbuffer, ERx("NOUSAGE"));
	}
	/* Append RAWPCT if new location */
	if ( new && rawpct )
	{
	    STcat(sbuffer, ERx(", RAWPCT = "));
	    CVna(rawpct, crawpct);
	    STcat(sbuffer, crawpct);
	}
/* # line 1017 "addlmenu.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1018 "addlmenu.sc" */	/* host code */
	if (sqlca.sqlcode < 0 && sqlca.sqlcode != (-E_GE0032_WARNING)) 
	{
	    if ( autocommit_off )
	    {
		/* Rollback the DROP LOCATION */
/* # line 1023 "addlmenu.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1024 "addlmenu.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set AUTOCOMMIT on");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1025 "addlmenu.sc" */	/* host code */
	    }
	    return (FAIL);
	}
	if ( autocommit_off )
	{
/* # line 1030 "addlmenu.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1031 "addlmenu.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set AUTOCOMMIT on");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1032 "addlmenu.sc" */	/* host code */
	}
	/*
	** Print message that loc has beeen updated or appended
	** (bug 10758).
	*/
/* # line 767 "addlmenu.qsc" */	/* message */
    {
      IImessage(ERget(msgid));
    }
/* # line 768 "addlmenu.qsc" */	/* sleep */
    {
      IIsleep(1);
    }
/* # line 770 "addlmenu.qsc" */	/* host code */
	return (OK);
  }
