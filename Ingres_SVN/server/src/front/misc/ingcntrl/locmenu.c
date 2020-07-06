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
#include	<st.h>
#include	<erfe.h>
#include	<ug.h>
#include	<uf.h>
#include        <generr.h>
#include        <stdprmpt.h>
#include	"eric.h"
#include	"global.h"
#include	"ictables.h"
/*
** Name:        locmenu.qsc -
**
** Description:
**
**      Defines:
**		extendform
**
** History:
**      ~1-Jan-90 (pete) - Allow selection of Help & End even when on
**                      invalid field; check for unsaved changed when
**                      user hits menuitem End.
**      23-apr-1990 (pete)
**              fix 6.3 bug 21342 with bad explanations for Help & End.
**      19-oct-1990 (pete)
**              Merge in changes done by Jennifer Lyerla in 9/89 for B1 proj.
**	12-nov-1990 (pete)
**		Other changes to make consistent with db.qsc & user.qsc.
**		(display popup if end without saving; suppress menuitems
**		if not enuf priv...) 
**	4-aug-1992 (jonb)
**		Change tricky loop code to slightly less tricky loop in
**		order to avoid a compiler warning.
**	7-dec-1992 (jonb)
**		Add support for ML sorts.
**	8-jul-1993 (kellyp)
**		Corrected to set the value of rtn
**	6-aug-1993 (kellyp)
**		When returning a value from the picklist, place the value on
**		on the current row if the current row is empty.  Otherwise,
**		placfe the value at the end of the table.
**	16-sep-1993 (kellyp)
**		Switched the order of Alter and ListChoices in the way
**		they appear in the menu line.
**	20-sep-1993 (robf)
**              Query standard catalog (iilocation_info) not internal catalog
**	        (iilocations). This allows users without QUERY_SYSCAT priv
**	22-nov-1993 (kellyp)
**		Should not activate validation when save_menuitem is selected
**		since this will bring up the error message twice.
**		( fix to bug 56833 )
**	        to use accessdb/catalogdb
**	26-oct-1994 (juliet)
**		Modified code such that user is able to extend database
**		to a location for both data and work usages.
**	25-jan-1995 (brogr02)
**		Added check to make sure user has entered a usage type 
**		(bug 66480)
**  7-may-1996 (angusm)
**      Add check to the "Alter" key from the 'Extend' screen for databases
**		(function extnd_alter) to prevent user from altering all work 
**		locations to Auxiliary Work locations (so that they no longer have 
**		a work location defined). (bug 75683)
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
**	28-feb-06 (hayke02)
**	    In extnd_alter() we now restrict the (single row) query selecting
**	    iiextend_info.status to a work or aux work row. This prevents
**	    E_LQ006A when a location is used for both data and work/aux work.
**	    This change fixes bug 102500, problem INGSRV 1263.
*/
FUNC_EXTERN     STATUS  iiicsdSelectDatabase();
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/* # line 93 "locmenu.sc" */	/* host code */
GLOBALREF       bool    Mgr;
GLOBALREF bool  IiicPrivSysadmin;  /* TRUE if user has SYSTEM_ADMIN privilege */
GLOBALREF IIDATABASE Iidatabase;
static	STATUS	extnd_init();
static	i4	extnd_write();
static	VOID	extnd_alter();
  static char Extenddbfrm[]= ERx("extdb");
  static char Loc_tbl[]= ERx("dbloctbl");
static char *
validate_location(loc_name, rpct)
  char *loc_name;
  i4 *rpct;
{
  i4 dbstat;
  i4 rawpct;
  char dbname[FE_MAXNAME + 1];
	i4 rtn;
	char msg[256], lpstr[256];
	STtrmwhite(loc_name);
	/* lookup location name */
/* # line 117 "locmenu.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select i.status, i.raw_pct from iilocation_info i where i.location_na\
me=");
    IIputdomio((short *)0,1,32,0,loc_name);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dbstat);
      IIgetdomio((short *)0,1,30,4,&rawpct);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 121 "locmenu.sc" */	/* host code */
	if (FEinqerr() != OK || FEinqrows() == 0)
	{
	    IIUGerr(E_IC0049_Location_doesnt_exist,UG_ERR_ERROR,
			1, (PTR)loc_name);
	    return NULL;
	}
	*rpct = rawpct;
	dbstat &= (DU_DBS_LOC | DU_WORK_LOC | DU_AWORK_LOC);
	switch (dbstat)
	{
	    case 0:  /* Location can't be used as an extend target */
		IIUGerr(E_IC004A_Location_is_not_avail, UG_ERR_ERROR, 
				1, (PTR) loc_name);
		return NULL;
	    case DU_DBS_LOC: 	/* Extend for DB */
		rtn=0;
		break;
	    case DU_WORK_LOC:	/* Extend for Work */
		rtn=1;
		break;
	    case DU_AWORK_LOC:	/* Extend for Auxiliary Work */
		rtn=2;
		break;
	    default:       /* Db or work, take your pick */
/* # line 163 "locmenu.qsc" */	/* getform */
  {
    if (IIfsetio(Extenddbfrm) != 0) {
      IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,dbname,(char *)"dbna\
me");
    } /* IIfsetio */
  }
/* # line 165 "locmenu.qsc" */	/* host code */
		STprintf(msg, ERget(S_IC005A_select_extension_type), dbname);
		STprintf(lpstr, ERx("%s;%s\n%s;%s\n%s;%s"),
		    ERget(F_IC0030_ext_data), ERget(S_IC005B_ext_data),
		    ERget(F_IC0033_ext_work), ERget(S_IC005C_ext_work),
		    ERget(F_IC0032_ext_work_aux), ERget(S_IC005C_ext_work));
		rtn = IIFDlpListPick( msg, lpstr, 0, -1, -1, 
				      (char*)NULL, (char*)NULL, NULL, NULL );
		if (rtn < 0)
		    return NULL;
		break;
	}
	switch (rtn)
	{
	  case 0:	return ERget(F_IC0030_ext_data);
	  case 1:	return ERget(F_IC0033_ext_work);
	  case 2:	return ERget(F_IC0032_ext_work_aux);
	  default:	return NULL;
	}
}
  static char Ext_title[80]= ERx("");
  static char *Save_menuitem;
  static char *Listchoices_menuitem;
  static char *Alter_menuitem;
  static char *Mode;
STATUS
extendform (dname)
  char *dname;
  {
    i4 err_row;
    STATUS stat = OK;
    char loc_name[FE_MAXNAME+1];
    char usage[FE_MAXNAME+1];
/* # line 201 "locmenu.qsc" */	/* message */
    {
      IImessage(ERget(S_IC003F_Looking_up_Locations));
    }
/* # line 203 "locmenu.qsc" */	/* host code */
    /*
    ** The extenddb form has to be 'addform'ed each time we enter
    ** to take a snapshot of iilocations.lname for validations.
    ** This is an interface WIN as users selecting ListChoices
    ** will be able to see a list of possible locations and select
    ** from it.
    */
    if (IIUFgtfGetForm(IIUFlcfLocateForm(), Extenddbfrm) != OK)
    {
	IIUGerr(S_IC0059_Get_Form_Error, UG_ERR_FATAL, 0);
    }
    /* User can make changes if running Accessdb (as opposed to Catalogdb) &
    ** has SYSTEM_ADMIN Priv.
    */
    if (Ext_title[0] == EOS)
    {
	if (Mgr)
	{
	    STlcopy(ERget(S_IC0100_Acc_Extend_Title), Ext_title, 
					sizeof(Ext_title));
	}
	else
	{
	    STlcopy(ERget(S_IC0101_Cat_Extend_Title), Ext_title, 
					sizeof(Ext_title));
	}
	if (Mgr && IiicPrivSysadmin)
	{
	    Save_menuitem = ERget(FE_Save);
	    Listchoices_menuitem = ERget(FE_Lookup);
	    Alter_menuitem = ERget(F_IC0026_Alter);
	    Mode = ERx("fill");
	}
	else
	{
	    Save_menuitem = ERx("");
	    Listchoices_menuitem = ERx("");
	    Alter_menuitem = ERx("");
	    Mode = ERx("read");
	}
    }
/* # line 247 "locmenu.qsc" */	/* inittable */
    {
      if (IItbinit(Extenddbfrm,Loc_tbl,Mode) != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 249 "locmenu.qsc" */	/* display */
    {
      if (IIdispfrm(Extenddbfrm,Mode) == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio((char *)"title",(short *)0,1,32,0,Ext_title);
      {
/* # line 253 "locmenu.qsc" */	/* host code */
	if (extnd_init(dname, Extenddbfrm, Loc_tbl) != OK)
	{
	    stat = FAIL;
/* # line 256 "locmenu.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 257 "locmenu.qsc" */	/* host code */
	}
/* # line 258 "locmenu.qsc" */	/* resume */
        {
          IIresfld(Loc_tbl);
          if (1) goto IIfdB1;
        }
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 265 "locmenu.qsc" */	/* host code */
        if (Save_menuitem[0] == EOS)
        {
            /* this was selected via the FRSkey */
            FTbell();
/* # line 269 "locmenu.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 270 "locmenu.qsc" */	/* host code */
        }
	err_row = extnd_write(dname, Iidatabase.own);
	if (err_row > 0)	/* error occurred; put cursor on bad row */
	{
/* # line 275 "locmenu.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Extenddbfrm,Loc_tbl,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,err_row) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 276 "locmenu.qsc" */	/* resume */
            {
              IIresfld(Loc_tbl);
              if (1) goto IIfdB1;
            }
/* # line 277 "locmenu.qsc" */	/* host code */
	}
	else
	{
	    /* success */
/* # line 281 "locmenu.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 282 "locmenu.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 2) {
          {
            char name[FE_MAXNAME + 1];
/* # line 290 "locmenu.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,name,22,(char *)0,0);
              } /* IIiqset */
            }
/* # line 292 "locmenu.qsc" */	/* host code */
        if (Alter_menuitem[0] == EOS)   /* This was selected via the FRSkey */
        {
            FTbell();
/* # line 295 "locmenu.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 296 "locmenu.qsc" */	/* host code */
        }
/* # line 298 "locmenu.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Extenddbfrm,Loc_tbl,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,loc_name,(char *)"\
dbloc");
                IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,usage,(char *)
"dbusage");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 300 "locmenu.qsc" */	/* host code */
	if (*loc_name == EOS)
	{
	    FTbell();
/* # line 303 "locmenu.qsc" */	/* resume */
            {
              IIrescol(Loc_tbl,(char *)"dbloc");
              if (1) goto IIfdB1;
            }
/* # line 304 "locmenu.qsc" */	/* host code */
	}
	if (STequal(usage, ERget(F_IC0032_ext_work_aux)))
	{
	    extnd_alter(loc_name, F_IC0033_ext_work, DU_EXT_AWORK, DU_EXT_WORK);
	}
	else if (STequal(usage, ERget(F_IC0033_ext_work)))
	{
	    extnd_alter(loc_name, F_IC0032_ext_work_aux,
			 DU_EXT_WORK, DU_EXT_AWORK );
	}
	else
	{
	    IIUGerr(E_IC005D_cant_alter, UG_ERR_ERROR, 0);
	}
/* # line 320 "locmenu.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
            char tmpname[FE_MAXNAME + 1];
            char tmplocnm[FE_MAXNAME + 1];
/* # line 331 "locmenu.qsc" */	/* host code */
        if (Listchoices_menuitem[0] == EOS)
        {
            /* this was selected via the FRSkey */
            FTbell();
/* # line 335 "locmenu.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 336 "locmenu.qsc" */	/* host code */
        }
/* # line 337 "locmenu.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,tmpname,19,(char *)0
                ,0);
              } /* IIiqset */
            }
/* # line 338 "locmenu.qsc" */	/* host code */
        if (!STequal(tmpname, Loc_tbl))
        {
            IIUGerr(E_IC006C_NotOnTableField, UG_ERR_ERROR, 0);
/* # line 341 "locmenu.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 342 "locmenu.qsc" */	/* host code */
	}
	icListPicks(ERget(F_IC0091_Location),
			ERx("iilocation_info"), ERx("location_name"), tmpname);
	if (tmpname[0] != EOS)
	{
            char *tmpusage;
            i4 state = stNEW;
            i4 rawblocks;
            i2 rawblocks_ind;
	    i4  rawpct;
	    if ( NULL == (tmpusage = validate_location(tmpname, &rawpct)))
/* # line 354 "locmenu.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 356 "locmenu.qsc" */	/* host code */
	    /* Don't show "0" for rawblocks if rawpct == 0 */
	    rawblocks = 0;
	    if ( rawpct )
		rawblocks_ind = 0;
	    else
		rawblocks_ind = -1;
/* # line 363 "locmenu.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Extenddbfrm,Loc_tbl,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,tmplocnm,(char *)"\
dbloc");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 364 "locmenu.qsc" */	/* host code */
	    if (STequal(tmplocnm, ERx("")))
/* # line 365 "locmenu.qsc" */	/* putrow */
            {
              if (IItbsetio(3,Extenddbfrm,Loc_tbl,-2) != 0) {
                IItcoputio((char *)"dbloc",(short *)0,1,32,0,tmpname);
                IItcoputio((char *)"dbusage",(short *)0,1,32,0,tmpusage);
                IItcoputio((char *)"rawblocks",&rawblocks_ind,1,30,4,&
                rawblocks);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 368 "locmenu.qsc" */	/* host code */
	    else
	    {
/* # line 370 "locmenu.qsc" */	/* loadtable */
            {
              if (IItbact(Extenddbfrm,Loc_tbl,1) != 0) {
                IItcoputio((char *)"dbloc",(short *)0,1,32,0,tmpname);
                IItcoputio((char *)"dbusage",(short *)0,1,32,0,tmpusage);
                IItcoputio((char *)"_STATE",(short *)0,1,30,4,&state);
                IItcoputio((char *)"rawblocks",&rawblocks_ind,1,30,4,&
                rawblocks);
                IITBceColEnd();
              } /* IItbact */
            }
/* # line 373 "locmenu.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Extenddbfrm,Loc_tbl,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 374 "locmenu.qsc" */	/* host code */
	    }
/* # line 376 "locmenu.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,Extenddbfrm,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 377 "locmenu.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 4) {
          {
/* # line 384 "locmenu.qsc" */	/* host code */
	if (Mgr)
	    FEhelp(ERx("icextfrm.hlp"), ERget(S_IC003E_Extenddb_Form));
	else
	    FEhelp(ERx("icext.hlp"), ERget(S_IC003E_Extenddb_Form));
          }
        } else if (IIretval() == 5) {
          {
/* # line 393 "locmenu.qsc" */	/* host code */
        if (Mgr)
        {
            /* user is running Accessdb */
            i4 changed;
/* # line 398 "locmenu.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&changed,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 399 "locmenu.qsc" */	/* host code */
            if ( changed )
            {
                changed = CONFIRM(dname, ERget(F_IC001A_Database));
                switch(changed)
                {
                case CONFCH_YES:
                    if ((err_row = extnd_write(dname, Iidatabase.own)) > 0)
		    {
/* # line 407 "locmenu.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Extenddbfrm,Loc_tbl,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,err_row) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 408 "locmenu.qsc" */	/* resume */
            {
              IIresfld(Loc_tbl);
              if (1) goto IIfdB1;
            }
/* # line 409 "locmenu.qsc" */	/* host code */
                    }
                    /* fall through */
                case CONFCH_NO:
/* # line 412 "locmenu.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 413 "locmenu.qsc" */	/* host code */
                default:
                    break;
                }
/* # line 416 "locmenu.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 417 "locmenu.qsc" */	/* host code */
            }
        }
/* # line 419 "locmenu.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 6) {
          {
            i2 c1;
            char *tag;
            i4 rblocks;
            i2 rblocks_ind;
/* # line 428 "locmenu.qsc" */	/* host code */
	i4   rpct;
/* # line 430 "locmenu.qsc" */	/* inquire_frs */
            {
              if (IIiqset(5,0,Extenddbfrm,Loc_tbl,(char *)0) != 0) {
                IIiqfsio((short *)0,1,30,2,&c1,20,(char *)"dbloc",0);
              } /* IIiqset */
            }
/* # line 432 "locmenu.qsc" */	/* host code */
	if (c1)
	{
/* # line 434 "locmenu.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Extenddbfrm,Loc_tbl,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,loc_name,(char *)"\
dbloc");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 436 "locmenu.qsc" */	/* host code */
	    if (*loc_name == EOS)
/* # line 437 "locmenu.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
/* # line 439 "locmenu.qsc" */	/* host code */
	    if ( NULL == (tag = validate_location(loc_name, &rpct)))
/* # line 440 "locmenu.qsc" */	/* resume */
            {
              IIrescol(Loc_tbl,(char *)"dbloc");
              if (1) goto IIfdB1;
            }
/* # line 442 "locmenu.qsc" */	/* host code */
	    /* Don't show "0" for rawblocks if rawpct == 0 */
	    rblocks = 0;
	    if ( rpct )
		rblocks_ind = 0;
	    else
		rblocks_ind = -1;
/* # line 449 "locmenu.qsc" */	/* putrow */
            {
              if (IItbsetio(3,Extenddbfrm,Loc_tbl,-2) != 0) {
                IItcoputio((char *)"dbusage",(short *)0,1,32,0,tag);
                IItcoputio((char *)"rawblocks",&rblocks_ind,1,30,4,&rblocks);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 451 "locmenu.qsc" */	/* host code */
	}
/* # line 452 "locmenu.qsc" */	/* resume */
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
      if (IInmuact(Save_menuitem,ERget(F_FE0117_SaveExpl),2,0,1) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,0,1) == 0) goto IIfdE1;
      if (IInmuact(Alter_menuitem,ERget(F_IC002A_AlterExpl),0,0,2) == 0) goto 
      IIfdE1;
      if (IInmuact(Listchoices_menuitem,ERget(FE_LookupExpl),0,0,3) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,2,4) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,5) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,5) == 0) goto IIfdE1;
      if (IITBcaClmAct(Loc_tbl,(char *)"all",0,6) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 456 "locmenu.qsc" */	/* host code */
    return stat;
  }
/*
** Name:        extend_init
**
** Description: initialize Extenddbfrm tables and fields.
**	Note:  User names are case sensitive!
**
** History:
**	jul-14-1993 (kellyp)
**	Added module description and corrected the logic which puts the
**	location usage to include AUXILIARY WORK and not just DATABASE &
**	WORK.
**
*/
static STATUS
extnd_init (db_nm, formname, tblname)
  char *db_nm;
  char *formname;
  char *tblname;
  {
  char *u_name;
  char locname[FE_MAXNAME+1];
  i4 stat;
  i4 rawstart;
  i4 rawblocks;
  i2 rawstart_ind, rawblocks_ind;
  char *tag;
    if (STtrmwhite(db_nm) <= 0)
	return FAIL;
    /* select row from iidatabase */
    if (iiicsdSelectDatabase(db_nm, &Iidatabase) != OK)
	    return FAIL;
    u_name = Iidatabase.own;
/* # line 501 "locmenu.qsc" */	/* putform */
    {
      if (IIfsetio(formname) != 0) {
        IIputfldio((char *)"dbname",(short *)0,1,32,0,db_nm);
        IIputfldio((char *)"dbowner",(short *)0,1,32,0,u_name);
      } /* IIfsetio */
    }
/* # line 503 "locmenu.qsc" */	/* host code */
    /*Load table */
/* # line 681 "locmenu.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select e.location_name, e.status, e.raw_start, e.raw_blocks from iiex\
tend_info e, iilocation_info l where e.status>0 and e.database_name=");
    IIputdomio((short *)0,1,32,0,db_nm);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and e.location_name=l.location_name order by e.location_name");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,locname);
      IIgetdomio((short *)0,1,30,4,&stat);
      IIgetdomio((short *)0,1,30,4,&rawstart);
      IIgetdomio((short *)0,1,30,4,&rawblocks);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 691 "locmenu.sc" */	/* host code */
    {
	if (stat & DU_EXT_DATA)
		tag = ERget(F_IC0030_ext_data);
	else if (stat & DU_EXT_WORK)
		tag = ERget(F_IC0033_ext_work);
	else if (stat & DU_EXT_AWORK)
		tag = ERget(F_IC0032_ext_work_aux);
	/* Reduce clutter, don't show rawblocks, rawstart if none */
	if ( rawblocks == 0 )
	{
	    rawblocks_ind = -1;
	    rawstart_ind = -1;
	}
	else
	{
	    rawblocks_ind = 0;
	    rawstart_ind = 0;
	}
/* # line 534 "locmenu.qsc" */	/* loadtable */
      {
        if (IItbact(formname,tblname,1) != 0) {
          IItcoputio((char *)"dbloc",(short *)0,1,32,0,locname);
          IItcoputio((char *)"dbusage",(short *)0,1,32,0,tag);
          IItcoputio((char *)"rawstart",&rawstart_ind,1,30,4,&rawstart);
          IItcoputio((char *)"rawblocks",&rawblocks_ind,1,30,4,&rawblocks);
          IITBceColEnd();
        } /* IItbact */
      }
    }
/* # line 538 "locmenu.qsc" */	/* host code */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
    if (FEinqerr() != OK)
	return FAIL;
    return OK;
  }
/* EXTEND_WRITE -- validates and updates system catalogs.
**
**	return status:
**		> 0	row number where and error occurred in
**			a location name or data base name.
**
**			locationnames must exist in iilocations catalog.
**			dbnames must exist in data base catalog.
**			modification of names can only occurr to
**			thoses that are not in use.
**
**		  0	OK
**		< 0	FAIL
**
**
**		On error the old data will be replaced ( if modification)
**		or the cursor will point to incorrect new data.
** History:
**	24-oct-88 (kathryn)  	Bug# 2070
**		Defined loc_name of form extenddb.frm to be nullable, and
**		added variable loc_null to determine if location name on
**		the form is null, when save command is entered.  Allowing
**		loc_name to be null enables the user to move around the form
**		rather than being held on the loc_name field until a valid
**		location name is entered.
**       27-sep-1989 (jennifer)
**              change call from credbloc to have an internal database
**		procedure to create the location and add it to a database.
**	9-jul-91 (pete)
**		If error occurs saving, then don't try to write out the
**		new ext.bin file. Don't try to save empty new rows.
**		Bug 38400.
**	15-nov-1991 (pete)
**		Add new argument, need_dbdir_flg, to procedure call:
**		iiqef_add_location(). Definition of proc was changed in server.
**	8-jul-1993 (kellyp)
**		Set the value of need_dbdir_flg to 1. Corrected the logic to set
**		the loc_stat value in the case of reextending.
**	13-jul-1993 (kellyp)
**		Corrected extnd_write(). Query needs to be further restricted by
**		the database name.  Reversed change made on 7/8/93 to have a case
**		for a location of type database for reextending.
**	13-jul-1993 (kellyp)
**		Reinstated change made on 7/8/93.
**	18-jul-1993 (kellyp)
**		Corrected misplaced ENDLOOP (bug 56895)
**	22-nov-1993 (kellyp)
**		Should not do another validation of the location (bug56833)
**	26-oct-1994 (juliet)
**		Modified code such that user is able to extend database to
**		a location for both data and work usages.
**	8-sep-1995 (angusm)
**		Respect case-preference of installation: do not convert
**		location name to lower-case. (bug 70947)
*/
static STATUS	gooddata();
static i4
  extnd_write(db_name, uname)
  char *db_name;
char	*uname;
  {
    i4 state;
    i4 rec;
	i4	badrow = 0;
	bool	reextending;
  i4 loc_usage;
  i4 usage_or_oper;
  char area[DB_AREA_MAX+1];
  i4 loc_stat;
  char loc_name[FE_MAXNAME+1];
  char loc_type[FE_MAXNAME+1];
/* # line 621 "locmenu.qsc" */	/* message */
    {
      IImessage(ERget(S_IC0041_Validating_DB_extens));
    }
/* # line 623 "locmenu.qsc" */	/* unloadtable */
    {
      if (IItbact(Extenddbfrm,Loc_tbl,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,loc_name,(char *)"db\
loc");
        IItcogetio((short *)0,1,30,4,&state,(char *)"_STATE");
        IItcogetio((short *)0,1,30,4,&rec,(char *)"_RECORD");
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,loc_type,(char *)
"dbusage");
        IITBceColEnd();
        {
/* # line 631 "locmenu.qsc" */	/* host code */
/* bug 70947:
   don't convert to lower-case:
   don't even call IIUGlbo_lowerBEobject() - we are extending
   to a location which has been picked from from those existing
   in 'iilocations', so case is as it should be. Location name
   is converted to correct case via IIUGlbo_lowerBEobject()
   on creation. 
*/
		STtrmwhite(loc_name); 
		switch(state)
		{
		 case stUNDEF:  
		 case stUNCHANGED:
		 case stDELETE:
		 case stCHANGE:
		    break;
		 case stNEW:
		    if (loc_name[0] == EOS)
			    break;	/* empty new row -- skip it */
		    reextending = FALSE;
		    /* 
		    ** 10-26-94, J. Thai, convert selected usage 
		    ** string to an integer value. Modified the  
		    ** sql select below, such that only the tuple
		    ** which references the database name, the  
		    ** location name specified, and the approp- 
		    ** riate usage is selected.     
		    */
                    if (STequal(loc_type, ERget(F_IC0030_ext_data)))
                        loc_usage = DU_EXT_DATA;
                    else if (STequal(loc_type, ERget(F_IC0033_ext_work)))
		        loc_usage = DU_EXT_WORK;
                    else if (STequal(loc_type, ERget(F_IC0032_ext_work_aux)))
		        loc_usage = DU_EXT_AWORK;
		    else
		    {
			    IIUGerr(E_IC0154_Usage_required,
					    UG_ERR_ERROR, 0);
			    badrow = rec;
/* # line 679 "locmenu.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 680 "locmenu.qsc" */	/* host code */
		    }
		    usage_or_oper = (loc_usage | DU_EXT_OPERATIVE);
/* # line 865 "locmenu.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct status from iiextend_info where location_name=");
    IIputdomio((short *)0,1,32,0,loc_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and database_name=");
    IIputdomio((short *)0,1,32,0,db_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and(status=");
    IIputdomio((short *)0,1,30,4,&loc_usage);
    IIwritio(0,(short *)0,1,32,0,(char *)"or status=");
    IIputdomio((short *)0,1,30,4,&usage_or_oper);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&loc_stat);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 872 "locmenu.sc" */	/* host code */
		    /* no duplicates allowed */
		    if(FEinqerr() != OK)
		    {
			    IIUGerr(E_IC0046_Errors_extending_db, UG_ERR_ERROR,
				2, db_name, loc_name);
			    badrow = rec;
/* # line 698 "locmenu.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 699 "locmenu.qsc" */	/* host code */
		    }
		    else if (FEinqrows() > 0)
		    {
			/*
			** An entry may exist, however, if an extension
			** was attempted and we crashed or failed. If
			** the operable flag is 0, then this the case.
			** We're attempting to 're-extend' the database.
			** Set a flag so no other rows are appended.
			*/
			/* 10-26-94 (juliet) Modified error E_IC0042...
			** text to specify that the location has been
			** extended to the database for this usage already.
			** Modified IIUGerr call parameters, added 
			** location usage as error message argument. */
			if (loc_stat & DU_EXT_OPERATIVE)
			{
			    IIUGerr(E_IC0042_Location_already_exis, 
				    UG_ERR_ERROR, 3,
				    loc_name, db_name, loc_type
			    );
			    break;
			}
			else
			{
			    reextending = TRUE;
/* # line 727 "locmenu.qsc" */	/* message */
          {
            IImessage(ERget(S_IC0043_Attempting_Re_Extend));
          }
/* # line 728 "locmenu.qsc" */	/* host code */
			}
		    }
		    else
		    {
/* # line 732 "locmenu.qsc" */	/* message */
          {
            IImessage(ERget(S_IC0044_Creating_the_New_Exte));
          }
/* # line 733 "locmenu.qsc" */	/* host code */
		    }
/* # line 924 "locmenu.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select location_area from iilocation_info where location_name=");
    IIputdomio((short *)0,1,32,0,loc_name);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_AREA_MAX+1-1,area);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 928 "locmenu.sc" */	/* host code */
		    if (FEinqerr() != OK)
		    {
			    IIUGerr(E_IC0045_Error_retrieving_area, 
					    UG_ERR_ERROR, 1, loc_name);
			    badrow = rec;
/* # line 744 "locmenu.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 745 "locmenu.qsc" */	/* host code */
		    }
		    STtrmwhite(area);
		    /*
		    ** Don't append this if we're re-extending:
		    ** a row already exists in the iiextend catalog.
		    */
		    if (!reextending)
		    {
			if (STequal(loc_type, ERget(F_IC0030_ext_data)))
			    loc_stat = DU_DBS_LOC;
			else if (STequal(loc_type, ERget(F_IC0033_ext_work)))
			    loc_stat = DU_WORK_LOC;
			else if (STequal(loc_type, ERget(F_IC0032_ext_work_aux)))
			    loc_stat = DU_AWORK_LOC;
			else
			    loc_stat = 0;
/* # line 954 "locmenu.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"iiqef_add_location");
    IILQprvProcValio((char *)"database_name",0,(short *)0,1,32,0,db_name);
    IILQprvProcValio((char *)"location_name",0,(short *)0,1,32,0,loc_name);
    IILQprvProcValio((char *)"access",0,(short *)0,1,30,4,&loc_stat);
    IILQprvProcValio((char *)"need_dbdir_flg",0,(short *)0,1,30,sizeof(1),
    (void *)IILQint(1));
    while (IILQprsProcStatus(0) != 0) {
      if (sqlca.sqlcode < 0) 
        IIsqPrint(&sqlca);
    } /* IILQprsProcStatus */
  }
/* # line 959 "locmenu.sc" */	/* host code */
		    }
		    if ( sqlca.sqlcode < 0 &&
			 sqlca.sqlcode != (-E_GE0032_WARNING) )
		    {    
			IIUGerr(E_IC0046_Errors_extending_db, UG_ERR_ERROR, 
				2, db_name, loc_name);
			badrow = rec;
/* # line 781 "locmenu.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 782 "locmenu.qsc" */	/* host code */
		    }
		    break;
		default:
		    break;
		}  
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 791 "locmenu.qsc" */	/* host code */
	return(badrow);
  }
static VOID
extnd_alter(locnm, newtag, droptype, addtype)
ER_MSGID newtag;
  char *locnm;
  i4 droptype;
  i4 addtype;
  {
  char db_name[FE_MAXNAME+1];
  i4 dbstat;
  i4 checkwloc =(DU_EXT_OPERATIVE | DU_EXT_WORK);
  i4 checkawloc =(DU_EXT_OPERATIVE | DU_EXT_AWORK);
  i4 checkcount =0;
    char *tagtext;
/* # line 814 "locmenu.qsc" */	/* getform */
    {
      if (IIfsetio(Extenddbfrm) != 0) {
        IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,db_name,(char *)"dbn\
ame");
      } /* IIfsetio */
    }
/* # line 816 "locmenu.qsc" */	/* host code */
    /* lookup location name */
/* # line 1010 "locmenu.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select e.status from iiextend_info e where e.location_name=");
    IIputdomio((short *)0,1,32,0,locnm);
    IIwritio(0,(short *)0,1,32,0,(char *)"and e.database_name=");
    IIputdomio((short *)0,1,32,0,db_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and(e.status=");
    IIputdomio((short *)0,1,30,4,&checkwloc);
    IIwritio(0,(short *)0,1,32,0,(char *)"or e.status=");
    IIputdomio((short *)0,1,30,4,&checkawloc);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dbstat);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1016 "locmenu.sc" */	/* host code */
    if ((FEinqerr() == OK && FEinqrows() != 0))
    {
		/* check that at least one work loc will still be defined: bug 75683*/
/* # line 1019 "locmenu.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiextend_info e where e.database_name=");
    IIputdomio((short *)0,1,32,0,db_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and e.status=");
    IIputdomio((short *)0,1,30,4,&checkwloc);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&checkcount);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1024 "locmenu.sc" */	/* host code */
		if ((checkcount == 1 && 
			 droptype == DU_EXT_WORK && addtype != DU_EXT_WORK) ||	
			( sqlca.sqlcode < 0 && sqlca.sqlcode != (-E_GE0032_WARNING) ))
		{    
	    	IIUGerr(E_IC005F_cant_alter, UG_ERR_ERROR, 0);
			return;
		}
/* # line 1031 "locmenu.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"iiqef_alter_extension");
    IILQprvProcValio((char *)"database_name",0,(short *)0,1,32,0,db_name);
    IILQprvProcValio((char *)"location_name",0,(short *)0,1,32,0,locnm);
    IILQprvProcValio((char *)"drop_loc_type",0,(short *)0,1,30,4,&droptype);
    IILQprvProcValio((char *)"add_loc_type",0,(short *)0,1,30,4,&addtype);
    while (IILQprsProcStatus(0) != 0) {
      if (sqlca.sqlcode < 0) 
        IIsqPrint(&sqlca);
    } /* IILQprsProcStatus */
  }
/* # line 1036 "locmenu.sc" */	/* host code */
	if ( sqlca.sqlcode < 0 &&
	     sqlca.sqlcode != (-E_GE0032_WARNING) )
	{    
	    IIUGerr(E_IC003C_cant_alter, UG_ERR_ERROR, 0);
	    return;
	}
    }
    tagtext = ERget(newtag);
/* # line 857 "locmenu.qsc" */	/* putrow */
    {
      if (IItbsetio(3,Extenddbfrm,Loc_tbl,-2) != 0) {
        IItcoputio((char *)"dbusage",(short *)0,1,32,0,tagtext);
        IITBceColEnd();
      } /* IItbsetio */
    }
  }
/* # line 861 "locmenu.qsc" */	/* host code */
/*
**	GOODDATA -- validates data in row
*/
static STATUS
gooddata (db, loc, uname)
  char *db;
  char *loc;
  char *uname;
{
  char temp[FE_MAXNAME+1];
  i4 access;
	if(!*db && !*loc)
	{
		return (OK);
	}
	if((!*db && *loc) || (*db && !*loc))
	{
		IIUGerr(E_IC0047_Missing_data, UG_ERR_ERROR, 0);
		return (FAIL);
	}
/* # line 1077 "locmenu.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select database_name from iidatabase_info where name=");
    IIputdomio((short *)0,1,32,0,db);
    IIwritio(0,(short *)0,1,32,0,(char *)"and database_owner=");
    IIputdomio((short *)0,1,32,0,uname);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,temp);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1082 "locmenu.sc" */	/* host code */
	if(FEinqerr() != OK || FEinqrows() <= 0)
	{
		IIUGerr(E_IC0048_db_not_belong_to_dba, UG_ERR_ERROR, 0);
		return (FAIL);
	}
/* # line 1087 "locmenu.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select status from iilocation_info where location_name=");
    IIputdomio((short *)0,1,32,0,loc);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&access);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 1091 "locmenu.sc" */	/* host code */
	if(FEinqerr() != OK || FEinqrows() <= 0)
	{
		IIUGerr(E_IC0049_Location_doesnt_exist, UG_ERR_ERROR, 
				1, (PTR) loc);
		return (FAIL);
	}
	if(!(access & (DU_DBS_LOC|DU_WORK_LOC|DU_AWORK_LOC)))
	{
		IIUGerr(E_IC004A_Location_is_not_avail, UG_ERR_ERROR, 
				1, (PTR) loc);
		return (FAIL);
	}
	return(OK);
}
