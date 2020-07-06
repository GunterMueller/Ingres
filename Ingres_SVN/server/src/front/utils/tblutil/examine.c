# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ug.h>
# include	<uf.h>
# include <ui.h>
# include <uigdata.h>
/*
	this tells MING about ui.qsh and uigdata.qsh dependencies.
#include        <ui.h>
#include        <uigdata.h>
*/
# include	<erfe.h>
# include       <lo.h>
# include	"ertu.h"
# include	"tuconst.h"
# include "tutblinf.h"
/*
	this tells MING about tutblinf.qsh dependency.
#include        "tutblinf.h"
*/
/**
** Name:	examine.qsc -	The "Examine" frame for the Tables Utility.
**
** Description:
**	This is the Examine frame for the Tables Utility. The IITUtbiTableinfo
**	routine contains the menuitem activations for this frame, while
**	fillinfo is used to load the table field with information.
**
**	This file defines:
**	IITUtbiTableinfo  interface to the top frame, contains the menuitems.
**	fillinfo	for loading the table field with column info.
**
** History:
**	6/4/85 (prs)	Fix for bug 5190.
**	11/26/85 (prs)	Fix for bug 7033.
**	27-mar-1987	(daver) Extracted from tblutils.qc.
**	22-oct-1987 (peter)
**		Change FEmsg to IIUGerr.
**	6/88 (bobm)	pass in title of form.
**	9/89 (pete)	Fix so can be called directly (for Emerald).
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**	29-may-1997 (canor01)
**	    Cleaned up compiler warnings from CL prototypes.
**      02-Jul-1999 (hweho01)
**              Included uf.h header file, it contains the function     
**              prototype of IIUFlcfLocateForm(). Without the explicit
**              declaration, the default int return value for a  
**              function will truncate a 64-bit address on ris_u64 
**              platform.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
GLOBALREF bool    IituMainform;	/* used by tutblpik.qsc */
/* # define's */
# define	MAX_RETRIES	3
/* extern's */
/* static's */
static		bool	formready = FALSE;
  static char IITUtblname[FE_MAXTABNAME]ZERO_FILL;
static bool	Tblprmptformready = FALSE;
static STATUS	fillinfo();
/* following could be put in .roc file */
static READONLY
  char _Form[]= ERx("tuexaminefr"), _Tblfld[]= ERx("attributes"), _Deflkey[]= ERx("default_lkey")
  , _Col_name[]= ERx("name");
i4		tugetattr();
DB_STATUS	IIUIrel_help();
STATUS		tuchkname();
VOID		IITUstlSetTitle();
i4		IITUtsTblfldSearch();
char		*IITUtpTblPik();
char		*IITUprTablePrompt();
/*{
** Name:	 IITUtbiTableinfo - Driver for the "Examine" frame
**
** Description:
**	This routine is used to gather all of the table information and display
**	it on the "Examine" frame.
**
**	IITUtbiTableinfo() can be called directly by anyone who wants
**	a display of detailed table info.
**
** Sample call:
**	    IITUtbiTableinfo(ERx("myDBtable"), ERx("VISION"),
**				ERx("column_name_to_scrollto"));
**
** Inputs:
**	tblname		table whose information is to be displayed.
**	title_prefix	prefix the screen title with this.
** 	scrollto_col	After loading table field with info on every column in
**			the table, scroll to row with this column-name.
**
** Outputs:
**	usually none, but will change the 'tblname' argument if
**	a new table is selected in the course of running this frame.
**
** History:
**	27-mar-1987	(daver)		Extracted from tblutils.qc.
**	21-jul-1987	(danielt) changed DB_MAXNAME to FE_MAXNAME
**	8/89		(pete)	changed name from tableinfo to IITUtbiTableinfo.
**				Removed table-owner as second argument.
**				Added call to IITUstlSetTitle to build title.
**				Added third argument: scrollto_col.
**				Document how can be called directly.
**	27-dec-1989	(pete)	Don't show menuitems Top, Bottom, Find; add
**				(validate=0, activate=0) to Help and End.
**	17-jan-1993	(rogerl) Straighten out delim ident handling.
**	21-jan-1993	(mgw)	Don't ignore IIUGdlm_ChkdlmBEobject() return
**				value in NewTable case. Use it to print the
**				appropriate error message.
**	5-apr-1993	(rogerl) Move defaults list clearing to fillinfo() -
**				user might view consecutive tables.
*/
#define	_FindExpl	F_FE0104_ExplFind
#define	_TopExpl	F_FE0105_ExplTop
#define	_BottomExpl	F_FE0106_ExplBottom
#define	_CancelExpl	F_FE0103_ExplCancel
#define	_HelpExpl	F_FE0100_ExplFrameHelp
#define _EndExpl	F_FE0102_ExplEnd
VOID
IITUtbiTableinfo ( tblname, owner, title_prefix, scrollto_col )
char	tblname[DB_GW1_MAXNAME+1];	/* Name of table to examine. If user
			** examines another table, via menuitem "NewTable",
			** then this param gets reset to the
			** name of new table. (caller might want to
			** Scroll to row with this table name)
			*/
char	*owner;
  char *title_prefix;
char	*scrollto_col;	/* if set, scroll to table field row matching
			** this table column-name.
			*/
  {
    char title[TU_TITLE_SIZE +1];
	if (!formready)
	{
		if ( IIUFgtfGetForm(IIUFlcfLocateForm(), _Form) != OK )
		{
			IIUGerr( E_TU0041_Get_Form_Failed, UG_ERR_ERROR,
					1, _Form
			);
			return;
		}
		formready = TRUE ;
	}
/* # line 177 "examine.qsc" */	/* display */
    {
      if (IIdispfrm(_Form,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 180 "examine.qsc" */	/* host code */
		if (*tblname != EOS)
		{
			/* load form and table field with info on this table */
			if (fillinfo(tblname, owner) != OK)
/* # line 184 "examine.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 186 "examine.qsc" */	/* host code */
			if ( scrollto_col != NULL && *scrollto_col != EOS )
			{
			    /* caller wants us to scroll to row with table
			    ** column-name matching the 'scrollto_col' param. 
			    */
        i4 row;
			    if ( (row = IITUtsTblfldSearch(_Form, _Tblfld,
					_Col_name, scrollto_col)) > 0)
/* # line 195 "examine.qsc" */	/* scroll */
        {
          if (IItbsetio(1,_Form,_Tblfld,-3) != 0) {
            IItbsmode((char *)"to");
            if (IItscroll(0,row) != 0) {
            } /* IItscroll */
          } /* IItbsetio */
        }
/* # line 196 "examine.qsc" */	/* host code */
			}
		}
		/* build title for this screen */
		IITUstlSetTitle(title_prefix, ERget(F_TU0030_Examine), title);
/* # line 201 "examine.qsc" */	/* putform */
        {
          if (IIfsetio(_Form) != 0) {
            IIputfldio((char *)"utitle",(short *)0,1,32,0,title);
          } /* IIfsetio */
        }
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
            char *p_tbl = (char *)NULL;
/* # line 208 "examine.qsc" */	/* host code */
	    FE_RSLV_NAME	rname;
	    char		get_own[FE_UNRML_MAXNAME + 1];
	    char		get_name[FE_UNRML_MAXNAME + 1];
	    for (;;)	/* loop till good table name is entered */
	    {
		p_tbl = IITUprTablePrompt(p_tbl); /* prompt user for table name
						  ** with popup Help available
						  */
		if ( p_tbl == NULL || *p_tbl == EOS )
		{
		    break;	/* user selected Cancel */
		}
		else
		{
		    /* separate out owner.table syntax */
		    get_own[0] = get_name[0] = EOS;
		    rname.name = p_tbl;
		    rname.owner_dest = get_own;
		    rname.name_dest = get_name;
		    rname.is_nrml = FALSE;
		    FE_decompose(&rname);
		    /* strip the quotes */
		    if (IIUGdlm_ChkdlmBEobject(get_name, get_name, FALSE) ==
			UI_BOGUS_ID)
		    {
			IIUGerr(E_TU0037_Name_contain_bad_char, UG_ERR_ERROR,
				(i4) 1, get_name);
			continue;
		    }
		    if (get_own[0] != EOS && IIUGdlm_ChkdlmBEobject(get_own,
						get_own, FALSE) == UI_BOGUS_ID)
		    {
			IIUGerr(E_TU0009_Owner_name_illegal, UG_ERR_ERROR,
				(i4) 1, get_own);
			continue;
		    }
		    if (fillinfo(get_name, get_own) != OK)
			continue;	/* no such table. prompt again. */
		    STcopy(get_name, tblname);	/* save good table name */
		    STcopy(get_own, owner);	/* save good owner name */
		    break;			/* exit while() loop */
		}
	    }
          }
        } else if (IIretval() == 2) {
          {
            i4 ontab;
            i4 dkey;
            char defval[7];
            char colname[FE_MAXNAME+1];
/* # line 265 "examine.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&ontab,28,(char *)0,0);
              } /* IIiqset */
            }
/* # line 266 "examine.qsc" */	/* host code */
		if (ontab == 0)
		{
			IIUGerr(E_TU0002_Must_be_on_table_fiel, UG_ERR_ERROR,
					(i4) 0);
/* # line 270 "examine.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 271 "examine.qsc" */	/* host code */
		}
/* # line 273 "examine.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_Form,_Tblfld,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,colname,_Col_name);
                IItcogetio((short *)0,1,32,6,defval,ERx("defable"));
                IItcogetio((short *)0,1,30,4,&dkey,_Deflkey);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 277 "examine.qsc" */	/* host code */
		if (!STequal(defval, ERget(F_TU0074_Value2)))
		{
			IIUGerr(E_TU000C_Not_User_Def, UG_ERR_ERROR, (i4) 2,
				colname, defval);
/* # line 281 "examine.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 282 "examine.qsc" */	/* host code */
		}
		/* get the default value into the popup */
		IITUpdPutDefault( dkey );
          }
        } else if (IIretval() == 3) {
          {
/* # line 291 "examine.qsc" */	/* host code */
		FEtabfnd(_Form, _Tblfld);
          }
        } else if (IIretval() == 4) {
          {
/* # line 296 "examine.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_Form,_Tblfld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 301 "examine.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_Form,_Tblfld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 307 "examine.qsc" */	/* host code */
		FEhelp(ERx("tutblinf.hlp"), ERget(F_TU0029_Table_Information));
          }
        } else if (IIretval() == 7) {
          {
/* # line 313 "examine.qsc" */	/* host code */
		IITUcdlClearDefaultList();
/* # line 314 "examine.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF1;
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
      if (IInmuact(ERget(F_TU0026_NewTable),ERget(F_TU004B_ExplNewTable),2,2,1)
       == 0) goto IIfdE1;
      if (IInmuact(ERget(F_TU006F_ViewDefaults),ERget(F_TU0071_ExplViewDflt),2
      ,2,2) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,6) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,7) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,7) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 316 "examine.qsc" */	/* host code */
	return ;
  }
/*{
** Name:	fillinfo - display information on the tablefield.
**
** Description:
**	Fill in examine and create table field.  Creates defaults list and
**	populates the hidden column _Deflkey.
**
** Inputs:
**	tablename	name of table to fill info for.
**	owner		owner of that table
**
** Outputs:
**	Returns:
**		STATUS 	OK if filled, FAIL if anything wrong.
**
** Side Effects:
**
** History:
**	1-mar-1987 (daver)
**		Written.
**	1-dec-1987 (peter)
**		Rewritten.
**	8/89	(pete)	Made static.
**	18-jan-93 (rogerl)  Don't tuchkname.  Name is already OK, since it was
**		lifted from a form.
**	19-jan-93 (rogerl)  Check return value of FE_resolve.
**	21-jan-1993 (mgw)   FE_resolve() failure means table doesn't exist,
**			    not that it's an illegal name. Report it as such.
**	5-apr-1993 (rogerl) Destroy and create defaults list.  Add hidden
**		column default_lkey, and populate that column (via call to
**		tugetattr()).
**      3-mar-2004 (rodjo04) INGCBT514 (bug 111902) Table names could be in 
**              the form owner.tablename. If this is the case then we know 
**              the owner. FE_resolve() will not work with owner.table for
**              tablename. 
*/
static STATUS
  fillinfo(tablename, owner)
  char *tablename;
  char *owner;
  {
	i4	attrcount;
    char tupwidth[15];
    UI_HELP_INFO *rel_info = NULL;
    UI_HELP_INFO *tab_info = NULL;
	UI_HELP_INFO	syn_info;
	DB_STATUS  rval;
	TBLINFO *p_tblinfo;	/* pointer to array of TBLINFO structs */
	i4	tblinfo_cnt;	/* number of rows in array p_tblinfo pts to */
	i4	deadlock_retries = 0;
	FE_RSLV_NAME	rname;
	char		get_own[FE_UNRML_MAXNAME + 1];
	char		get_name[FE_UNRML_MAXNAME + 1];
	char		temp_name[FE_UNRML_MAXNAME + 1];
/* # line 375 "examine.qsc" */	/* message */
    {
      IImessage(ERget(F_TU0032_Working_dot_dot_dot));
    }
/* # line 377 "examine.qsc" */	/* host code */
				/* hidden column default_lkey is for 
				** correspondence between defaults list and
				** columns on the screen
				*/
/* # line 381 "examine.qsc" */	/* inittable */
    {
      if (IItbinit(_Form,_Tblfld,(char *)"u") != 0) {
        IIthidecol((char *)"default_lkey",(char *)"integer");
        IItfill();
      } /* IItbinit */
    }
/* # line 383 "examine.qsc" */	/* host code */
	IITUcdlClearDefaultList();
	get_own[0] = get_name[0] = temp_name[0] = EOS;
	rname.owner_dest = get_own;
	rname.name_dest = get_name;
        rname.name = temp_name;
        rname.is_nrml = TRUE;
        STcopy(tablename, rname.name);
        FE_decompose(&rname);
        if (rname.owner_spec)
          {
            tablename  = &rname.name_dest[0];
            owner = &rname.owner_dest[0];
          }
	if ( FE_resolve(&rname, tablename, owner) == FALSE )
	{
	    if (*owner == EOS)
	    {
		IIUGerr(E_TU0016_Table_doesnt_exist, UG_ERR_ERROR,
				(i4) 1, (PTR) tablename);
	    }
	    else
	    {
		IIUGerr(E_TU000E_Own_dot_Tab_notexist, UG_ERR_ERROR,
				(i4) 2, (PTR) tablename, (PTR) owner);
	    }
	    return(FAIL);
	}
	if (rname.is_syn)
	{
		/* Fill in some synonym info */
		STcopy(tablename, syn_info.table_name);
		if (owner[0] == EOS)
		{	/* Assume current user if owner is null */
			STcopy(IIUIdbdata()->suser, syn_info.table_owner);
		}
		else
		{
			STcopy(owner, syn_info.table_owner);
		}
		STcopy(ERx("S"), syn_info.table_type);
		syn_info.system_use[0] = EOS;
	}
rel_help:
	rval = IIUIrel_help(get_name, get_own, &tab_info);
	if (IIUIdeadlock ((i4) rval))
	{
		/* deadlock occurred. retry. */
		if (deadlock_retries < MAX_RETRIES)
		{
/* # line 439 "examine.qsc" */	/* message */
    {
      IImessage(ERget(S_TU0058_DoingDeadlockRetry));
    }
/* # line 440 "examine.qsc" */	/* host code */
		    deadlock_retries++;
		    goto rel_help;
		}
		else
		{   
		    IIUGerr(E_TU0057_TooManyDeadlockTries, UG_ERR_ERROR,
			(i4) 2, (PTR) &deadlock_retries, (PTR) tablename);
		    return (FAIL);
		}
	}
	else if (rval != OK)
	{
		/* other error */
		IIUGerr(E_TU004D_Error_in_IIUIrel_help, UG_ERR_ERROR,
			(i4) 1, (PTR) &rval);
		return(FAIL);
	}
	else if (tab_info == NULL)
	{
		/* this should no longer occur, since FE_resolve says
		** says this name resolved into something
		*/
		/* no error, but nothing selected */
		IIUGerr(E_TU0016_Table_doesnt_exist, UG_ERR_ERROR,
			(i4) 1, (PTR) get_name);
		return(FAIL);
	}
	else
	{
		attrcount = tugetattr(tab_info, _Form, _Tblfld, tupwidth);
		if (attrcount == 0)
		{
		    IIUGerr(E_TU0015_No_attributes_found,  UG_ERR_ERROR,
				(i4) 1, (PTR) tablename);
		    return(FAIL);
		}
		if (rname.is_syn)
		{
			rel_info = &syn_info;
	        	tutabdet(rel_info, &rname, attrcount, tupwidth,
					&p_tblinfo, &tblinfo_cnt);
		}
		else
		{
			rel_info = tab_info;
	        	tutabdet(rel_info, (FE_RSLV_NAME *)NULL, attrcount,
					tupwidth, &p_tblinfo, &tblinfo_cnt);
		}
		/*
		** Set up table info to display in simple fields
		** above table field.
		*/
		/* write info to simple fields above table field. */
		IITUputform (p_tblinfo, tblinfo_cnt, _Form);
	}
	return(OK);
  }
/*{
** Name:	IITUprTablePrompt - prompt user for table name.
**
** Description:
**	Display a popup screen to ask user for name of a table. Popup
**	help available via the Choices menuitem.
**
**	This routine is called by both Examine and Create-a-Table.
**
** Inputs:
**	char *tbl	Initial table name to display, or NULL.
**
** Outputs:
**	Returns:
**		Pointer to table name selected.
**
**	Exceptions:
**		NONE
**
** Side Effects:
**
** History:
**	10/10/89 (pete)	Initial Version.
**	3/14/90  (pete) Fixed to accept an initial table name.
**	3-18-93  (rogerl) Preserve what user typed before cancel of
**		ListChoices by checking for EOS - in which case
**		nothing was chosen, it's not a " " named table.  Bug 49411.
*/
char *
IITUprTablePrompt(tbl)
  char *tbl;
  {
    char *promptform = ERx("tutableprompt");
        if (!Tblprmptformready)
	{
            if ( IIUFgtfGetForm(IIUFlcfLocateForm(), promptform) != OK )
	    {
                IIUGerr (E_TU0041_Get_Form_Failed, UG_ERR_ERROR, 1,
				                         promptform);
                return (char *)NULL;
            }
            Tblprmptformready = TRUE;
        }
/* # line 545 "examine.qsc" */	/* display */
    {
      if (IIdispfrm(promptform,(char *)"f") == 0) goto IIfdE2;
      goto IImuI2;
IIfdI2:;
      if (IIfsetio((char *)0) == 0) goto IIfdE2;
      {
/* # line 548 "examine.qsc" */	/* host code */
		if (tbl != (char *)NULL)
/* # line 550 "examine.qsc" */	/* putform */
        {
          if (IIfsetio((char *)0) != 0) {
            IIputfldio((char *)"tablename",(short *)0,1,32,0,tbl);
          } /* IIfsetio */
        }
      }
IIfdB2:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 555 "examine.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXTABNAME-1,IITUtblname,
                (char *)"tablename");
              } /* IIfsetio */
            }
/* # line 557 "examine.qsc" */	/* host code */
                if (IITUtblname[0] == EOS)     /* if it's a null name */
                    IIUGerr(E_TU0051_No_Table_Entered, UG_ERR_ERROR,
			    2, (PTR) ERget(FE_OK), (PTR) ERget(FE_Cancel));
		else
/* # line 561 "examine.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 567 "examine.qsc" */	/* host code */
		IITUtblname[0] = EOS;
/* # line 568 "examine.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else if (IIretval() == 3) {
          {
            char *p_tbl;
            char tblname[FE_MAXNAME+1];
/* # line 576 "examine.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,tblname,(char *)"\
tablename");
              } /* IIfsetio */
            }
/* # line 578 "examine.qsc" */	/* host code */
                /* IITUtpTblPik treats 'tblname' as a wildcard, by tacking
                ** a wildcard character onto it before running the query.
                */
		IituMainform = FALSE;	/* signal that Create & Destroy
					** menuitems should not be displayed.
					*/
                p_tbl = IITUtpTblPik(tblname, (char *)NULL );
                if ( p_tbl != NULL && *p_tbl != EOS )
                { /* user Selected a row. */
/* # line 588 "examine.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio((char *)"tablename",(short *)0,1,32,0,p_tbl);
              } /* IIfsetio */
            }
/* # line 589 "examine.qsc" */	/* host code */
                }
		else if ( p_tbl != NULL && *p_tbl != EOS
				&& ( STtrmwhite( p_tbl ) == 0 ) )
		{ /* special case " " table name */
		    p_tbl[0] = ERx('"');
		    p_tbl[1] = ERx(' ');
		    p_tbl[2] = ERx('"');
/* # line 596 "examine.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio((char *)"tablename",(short *)0,1,32,0,p_tbl);
              } /* IIfsetio */
            }
/* # line 597 "examine.qsc" */	/* host code */
		}
		/* else, leave what, if any, user typed already */
          }
        } else if (IIretval() == 4) {
          {
/* # line 604 "examine.qsc" */	/* host code */
                FEhelp(ERx("tutblprm.hlp"), ERget(F_TU004A_Help_Table_Prompt));
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
      if (IInmuact(ERget(FE_OK),ERget(F_TU0059_ExplGetOk),2,2,1) == 0) goto 
      IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,2,2) == 0) 
      goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,0,2,2) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Lookup),ERget(F_FE0116_LookupExpl),0,2,3) == 0) 
      goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,2,3) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,2,4) == 0) 
      goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,4) == 0) goto IIfdE2;
      if (IIendmu() == 0) goto IIfdE2;
      goto IIfdI2;
IIfdE2:;
      IIendfrm();
    }
/* # line 608 "examine.qsc" */	/* host code */
	return IITUtblname;
  }
