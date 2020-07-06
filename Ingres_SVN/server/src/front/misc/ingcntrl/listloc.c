# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
*/
#include	<compat.h>
#include	<gl.h>
#include	<sl.h>
#include	<st.h>
#include	<iicommon.h>
#include	<fe.h>
#include	<er.h>
#include	<erfe.h>
#include	<ug.h>
#include	<uf.h>
#include        <stdprmpt.h>
#include        <generr.h>
#include	"eric.h"
#include	"global.h"
GLOBALREF       bool    Mgr;
GLOBALREF bool  IiicPrivSysadmin;  /* TRUE if user has SYSTEM_ADMIN privilege */
static VOID list_init();
  static char Loclist_frm[]= ERx("loclist");
  static char Listing_tbl[]= ERx("listing");
  static char *Create_menuitem;
  static char *Examine_menuitem;
  static char *Delete_menuitem;
  static char Loclist_title[80]= ERx("");
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/* # line 30 "listloc.sc" */	/* host code */
/*{
** Name:	loclst_form -- listing of all locationnames and related stuff
**
** History:
**	18-may-89 (mgw)
**		Put string into ERx() call for internationalization
** 	15-Dec-89 (pete) porting change 90971 - Form will core dump on PS/2 AIX
**		if compiled with optimization (added "NO_OPTIM" line above).
**	19-nov-90 (pete)
**		Add Top,Bottom,Find FRSkeys.
**	26-Aug-1993 (fredv)
**		included <st.h>.
**	20-sep-1993 (robf)
**              Query standard catalogs (iilocation_info) not internal catalog
**	        (iilocations). This allows users without QUERY_SYSCAT priv
**	        to use accessdb/catalogdb
**	05-may-1995 (harpa06)
**		Fixed the problem of not being able to delete delimited
**		location names.
**	30-may-1995 (harpa06)
**		If a user is trying to delete a location and it is being used
**		by one or more databases, different error messages for the
**		same illegal operation would be reported to the user. Now,
**		only ONE error message is reported to the user if ANY location
**		is used by one of more databases.
**	10-may-1999 (walro03)
**		Remove obsolete version string ps2_us5.
*/
loclst_form()
  {
  char lname[FE_MAXNAME + 1];
	if(Loclist_title[0] == EOS)
	{
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), Loclist_frm) != OK)
			IIUGerr(S_IC0059_Get_Form_Error, UG_ERR_FATAL, 0);
		if (Mgr)
		{
		    STlcopy(ERget(S_IC0084_Accessdb_Loc_Info), Loclist_title,
				sizeof(Loclist_title));
		}
		else
		{
		    STlcopy(ERget(S_IC0085_Catalogdb_Loc_Info), Loclist_title,
				sizeof(Loclist_title));
		}
		if (Mgr && IiicPrivSysadmin)
		{
		    Create_menuitem = ERget(FE_Create);
		    Examine_menuitem = ERget(FE_Edit);
		    Delete_menuitem = ERget(FE_Delete);
		}
		else
		{
		    Examine_menuitem = ERget(FE_Examine);
		    Create_menuitem = ERx("");
		    Delete_menuitem = ERx("");
		}
	}
/* # line 101 "listloc.qsc" */	/* display */
    {
      if (IIdispfrm(Loclist_frm,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio((char *)"title",(short *)0,1,32,0,Loclist_title);
      {
/* # line 104 "listloc.qsc" */	/* host code */
	    list_init();
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 109 "listloc.qsc" */	/* host code */
		if (addlocform() == OK)
			list_init();
          }
        } else if (IIretval() == 2) {
          {
/* # line 115 "listloc.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Loclist_frm,Listing_tbl,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,lname,(char *)"\
locname");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 116 "listloc.qsc" */	/* host code */
                if ( STtrmwhite(lname) > 0
                     && editlocform(lname) == OK )
		{
			list_init();
		}
          }
        } else if (IIretval() == 3) {
          {
/* # line 125 "listloc.qsc" */	/* host code */
  char sbuffer[512];
	    char qlname[FE_MAXNAME + 1];
/* # line 131 "listloc.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Loclist_frm,Listing_tbl,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,lname,(char *)"\
locname");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 132 "listloc.qsc" */	/* host code */
	    if ( STtrmwhite(lname) > 0 && lname[0] != EOS )
	    {
		if (CONFCH_YES != IIUFccConfirmChoice(CONF_DELETE,
			lname, ERget(F_IC0021_Location), NULL, NULL))
/* # line 136 "listloc.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
/* # line 138 "listloc.qsc" */	/* host code */
	        STpolycat(3, ERx("\""), lname, ERx("\""), qlname);
		STpolycat(2, ERx("DROP LOCATION "), qlname, sbuffer);
/* # line 154 "listloc.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sbuffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 155 "listloc.sc" */	/* host code */
		if (sqlca.sqlcode >= 0 || sqlca.sqlcode == (-E_GE0032_WARNING)) 
/* # line 142 "listloc.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,Loclist_frm,Listing_tbl,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
/* # line 143 "listloc.qsc" */	/* host code */
	    }
/* # line 145 "listloc.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 152 "listloc.qsc" */	/* host code */
	    if (Mgr)
		FEhelp(ERx("iclstfrm.hlp"), ERget(S_IC003D_Location_Listing));
	    else
		FEhelp(ERx("icelstfr.hlp"), ERget(S_IC003D_Location_Listing));
          }
        } else if (IIretval() == 5) {
          {
/* # line 161 "listloc.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 167 "listloc.qsc" */	/* host code */
            FEtabfnd(Loclist_frm, Listing_tbl);
          }
        } else if (IIretval() == 7) {
          {
/* # line 173 "listloc.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Loclist_frm,Listing_tbl,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 8) {
          {
/* # line 179 "listloc.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Loclist_frm,Listing_tbl,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
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
      if (IInmuact(Examine_menuitem,(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IInmuact(Delete_menuitem,(char *)0,2,2,3) == 0) goto IIfdE1;
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
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
  }
/* # line 185 "listloc.qsc" */	/* host code */
/* LIST_INIT -- initialize listing of locations */
static VOID
list_init()
  {
  char loc_name[DB_MAXNAME+1];
  char loc_area[DB_AREA_MAX+1];
/* # line 195 "listloc.qsc" */	/* inittable */
    {
      if (IItbinit(Loclist_frm,Listing_tbl,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 197 "listloc.qsc" */	/* host code */
	/* get information into form */
/* # line 263 "listloc.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select location_name, location_area from iilocation_info order by loc\
ation_name");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,loc_name);
      IIgetdomio((short *)0,1,32,DB_AREA_MAX+1-1,loc_area);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 268 "listloc.sc" */	/* host code */
	{
/* # line 205 "listloc.qsc" */	/* loadtable */
    {
      if (IItbact(Loclist_frm,Listing_tbl,1) != 0) {
        IItcoputio((char *)"locname",(short *)0,1,32,0,loc_name);
        IItcoputio((char *)"area",(short *)0,1,32,0,loc_area);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 209 "listloc.qsc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
  }
