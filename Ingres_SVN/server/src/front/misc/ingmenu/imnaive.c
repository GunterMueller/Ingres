# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<st.h>		/* 6-x_PC_80x86 */
#include	<ci.h>
#include	<lo.h>		/* shell: fix for jupbug 4641 */
#include	<nm.h>		/* shell: fix for jupbug 4641 */
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
#include	<ui.h>		/* needed for IIUIdml call - mdw */
#include	<adf.h>
#include	<ft.h>
#include	<fmt.h>
#include	<frame.h>
#include	"erim.h"
#include	"imconst.h"
# include	"imextern.h"
/**
** Name:	naive.qc -	main loop for ingmenu
**
** Description:
**	For historical reasons, the main loop in ingmenu is called
**	"naive" (as opposed to "expert", refering to modes in the
**	original menu program). This file defines:
**
**	IIIMnaive()
**
** History:
**	Revision 6.0  87/07/08  daver
**	07/08/87 (daver)
**		Print function key definition next to menu item
**		displayed on the form.
**	28-jan-1988 (peter)
**		Fix bug 1849, where help in languages not returning
**		to right place.
**
**	Revision 6.1
**	24-may-88 (bruceb)
**		If database name is > 70 characters long (space in
**		database field), then display the trailing 70 chars.
**	6/88 (bobm)
**		change logic to return to submenu rather than main
**		display loop.
**	04-oct-1988 (sandyd)
**		Restructured to implement some much needed user-interface
**		changes requested by Training (and others).  This is
**		currently a bare-bones version needed to meet the code
**		freeze tomorrow.  It will need to be upgraded later with
**		the work BruceB was doing in 6.2 to hide options not in
**		the authorization string.  Also, VQL and other possible
**		PC options will need some work.
**	24-oct-1988 (pete)
**		Integrate Bruce's work into what Sandy did.
**
**	Revision 6.2  89/03  (wong)
**		Modified and prefixed with 'IIIM' to be a callable interface.
**	5-oct-1988 (mdw)
**		Since QUEL is not supported on any gateway, suppress the
**		QUEL selection option on the QUERY MENU when running in
**		a gateway.
**	2/6/90	(pete)
**		Added support for Vision (Emerald). Also, removed replicated
**		menuitems in preparation for character scrolling.
**	27-apr-90 (bruceb)
**		Made listpick'ish by supporting CR-to-select (as well as
**		the equivalent mouse support).
**	21-Feb-1995 (canor01)
**		Removed references to vigraph, which is not supported
**		at this time (B61996).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      11-Jun-2004 (hanch04)
**          Removed reference to CI for the open source release.
**/
# define	MAXDBNAME_DISP	70	/* max db name display length */
/*	tells top_frame() how to behave */
# define	IM_TOP_MENU		2
# define	IM_QUERY_MENU		3
# define	IM_EXIT			4
/*}
** Name:	MENUS -	IngMenu Menu Items Structure.
*/
  typedef struct {
    ER_MSGID item;
	ER_MSGID	expl;
    char *menu;
  } MENUS;
/*
** Table of all possible top-level menu options and explanations.
** ERget(item) is used to turn on dynamic menuitems.
*/
  static MENUS im_top_list[]=
{
		{F_IM000D_Tables, 	F_IM0002_Tables_expl},
		{F_IM000E_Forms,	F_IM0003_Forms_expl},
		{F_IM000F_JoinDefs,	F_IM0004_JoinDefs_expl},
		{F_IM0010_Reports,	F_IM0005_Reports_expl},
#ifdef OMIT 
		{F_IM0011_Graphs, 	F_IM0006_Graphs_expl},
#endif 
		{F_IM0012_Applications,	F_IM0007_Applications_expl},
		{F_IM0031_VisionOnly,	F_IM0032_VisionOnly_expl},
		{F_IM0033_Vision_ABF,	F_IM0034_Vision_ABF_expl},
		{F_IM0013_Queries, 	F_IM0019_Queries_expl},
};
#define im_top_count (sizeof(im_top_list)/sizeof(MENUS))
/*
** Table of all possible second-level menu options and explanations.
*/
  static MENUS im_qry_list[]=
{
		{F_IM0016_SQL, 		F_IM000A_SQL_expl},
		{F_IM0017_QUEL, 	F_IM000B_QUEL_expl},
		{F_IM0015_QBF, 		F_IM0009_QBF_expl},
		{F_IM0025_Null,		F_IM0025_Null},
		{F_IM0025_Null,		F_IM0025_Null},
		{F_IM0025_Null,		F_IM0025_Null},
		{F_IM0025_Null,		F_IM0025_Null},
};
#define im_qry_count (sizeof(im_qry_list)/sizeof(MENUS))
/*	statics	*/
  static i4 Scrollto_row = 1;
/*	functions */
static		void	imchkauth();
static		i4	top_frame();
static		i4	load_top();
static		i4	search_menu();
static		VOID	runopt();
static		char	*setAbfItems();
static		i4	key_catch();
FUNC_EXTERN	VOID iiimAddForm();
FUNC_EXTERN	VOID IIIMmqs_mqbfstart();
FUNC_EXTERN	i4	(*IIFRkeystroke())();
/*{
** Name:	IIIMnaive()	- main display loop for INGMENU
**
** Description:
**	Drives the main display loop for INGMENU.
**
** Inputs:
**	dbname	{char *}  The database name.
**	user	{char *}  The user name flag.
**	noabf	{bool}  ABF calls not allowed
**	expert	{bool}  Run in `expert' mode.
**
** History:
**	04-oct-1988 (sandyd)
**		Restructured to implement user-interface changes.
**	03/89 (jhw)  Made into a callable interface with prefix 'IIIM'.
*/
/* following could be made READONLY & put in .roc file.	*/
static 
  char _Form[]= ERx("imtopfr"), _Choices[]= ERx("choices"), _item[]= ERx("item")
  , _explanation[]= ERx("explanation"), _itemcode[]= ERx("itemcode");
static bool	_FormInit = FALSE;
VOID
  IIIMnaive( dbname, user, noabf, expert )
  char *dbname;
char	*user;
bool	noabf;
bool	expert;
  {
    i4		len;
    i4		next_frame = IM_TOP_MENU;
    if ( !_FormInit )
    {
	iiimAddForm( _Form );
/* # line 200 "imnaive.qsc" */	/* inittable */
    {
      if (IItbinit(_Form,_Choices,(char *)"r") != 0) {
        IIthidecol(_itemcode,(char *)"i4");
        IItfill();
      } /* IItbinit */
    }
/* # line 201 "imnaive.qsc" */	/* host code */
	/*
	** Set authorization for each menuitem using authorization string.
	*/
	imchkauth(im_top_list, im_top_count, noabf);
	imchkauth(im_qry_list, im_qry_count, noabf);
	_FormInit = TRUE;
    }
    /*
    ** Can only display up to the last MAXDBNAME_DISP chars
    ** of the database name.
    */
    im_dbname = dbname;
    len = STlength(dbname);
    if ( len > MAXDBNAME_DISP )
	dbname += (len - MAXDBNAME_DISP);
/* # line 219 "imnaive.qsc" */	/* putform */
    {
      if (IIfsetio(_Form) != 0) {
        IIputfldio((char *)"dbname",(short *)0,1,32,0,dbname);
      } /* IIfsetio */
    }
/* # line 221 "imnaive.qsc" */	/* host code */
    /* Set globals */
    im_uflag = user;
    im_xpipe = noabf ? "-X" : "";
    im_empty_catalogs = expert;
    /*
    ** Loop through the menu frames until they want to Quit.
    */
    while (next_frame != IM_EXIT)
    {
	if (next_frame == IM_TOP_MENU)
	{
	    next_frame = top_frame(next_frame,im_top_list,im_top_count);
	}
	else
	{
	    next_frame = top_frame(next_frame,im_qry_list, im_qry_count);
	}
    }
  }
/*{
** Name:	top_frame() - display loop for both menu frames
**
** Description:
**	Controls display of the menus.
**
** Inputs:
**	behavior	indicates how this frame should behave: either
**			as the first invocation of the top menu, as
**			the top menu after returning from the queries
**			menu, or as the queries menu.
**	im_menu		address of menu array to use.
**	im_menu_cnt	number of items in menu array.
**
** Outputs:
**	Returns:
**	    code to indicate what to invoke next
**
**		IM_EXIT		- exit INGMENU
**		IM_TOP_MENU	- call this again as Query menu.
**		IM_QUERY_MENU	- call this again as Top menu.
**
**	Exceptions:
**		none
**
** Side Effects:
**	none
**
** History:
**	04-oct-1988 (sandyd)
**		Written to implement user-interface changes.
**	26-oct-1988 (pete)
**		changed to integrate Sandy's work on 04-oct and
**		Bruce's work on suppressing menuitems based on
**		authorization string.
**	24-feb-1989 (pete)
**		complete work from above. make code more general
**		and extendable.
**	19-Apr-1989 (pete)
**		Move "Go" to front of menuline.
**	6-feb-1990  (pete)
**		Get rid of menuitems that replicate selections in table
**		field; also remove wrap-around scrolling. All this in
**		preparation for character-scrolling that will be part of
**		all READ mode table fields as part of 6.4. Also change
**		menuitem "Go" to "Select".
*/
static i4
  top_frame( behavior, im_menu, im_menu_cnt )
  i4 behavior;
  MENUS *im_menu;
  i4 im_menu_cnt;
  {
	i4	next_frame = IM_TOP_MENU; /* this variable gets returned;
					    tells naive() what to do next */
    i4 row;
	i4	last_row;
	i4	(*fptr)();
/*  for dynamic menuitems */
    char *im_end;
    char *im_cancel;
    char *im_quit;
    char *smenu;
	ER_MSGID	help_expl;	/* help menuitem explanation */
/*	fix for jupbug 4641. */
#ifndef DGC_AOS
#ifndef CMS
#ifndef VMS
	smenu = ERget(FE_Shell);
#else
	smenu = ERget(FE_Spawn);
#endif
#else
	smenu = ERget(FE_CMSSubset);
#endif
#else
	smenu = ERget(FE_CLI);
#endif
/*	end fix for jupbug 4641 */
    /* turn menuitem "End" on or off; set help explanation */
    if ( behavior == IM_TOP_MENU )
    {
	help_expl = F_IM001E_Get_Help_on_Ingres_Me;
	im_end    = ERget(FE_End);
	im_cancel = NULL;
	im_quit	  = ERget(FE_Quit);
    }
    else
    { /* Queries menu:  Cancel only; no End or Quit. */
	help_expl = F_IM001C_Get_Help_on_Queries;
	im_end    = NULL;
	im_cancel = ERget(FE_Cancel);
	im_quit	  = NULL;
    }
    fptr = IIFRkeystroke(key_catch);
/* # line 343 "imnaive.qsc" */	/* display */
    {
      if (IIdispfrm(_Form,(char *)"u") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 346 "imnaive.qsc" */	/* host code */
	last_row = load_top(im_menu, im_menu_cnt);
	/* set frame subtitle (read only field)*/
    	if (behavior == IM_QUERY_MENU) {
/* # line 350 "imnaive.qsc" */	/* putform */
        {
          if (IIfsetio(_Form) != 0) {
            IIputfldio((char *)"subtitle",(short *)0,1,32,0,
            ERget(F_IM0013_Queries));
          } /* IIfsetio */
        }
/* # line 351 "imnaive.qsc" */	/* host code */
	}
	else {
/* # line 353 "imnaive.qsc" */	/* scroll */
        {
          if (IItbsetio(1,_Form,_Choices,-3) != 0) {
            IItbsmode((char *)"to");
            if (IItscroll(0,Scrollto_row) != 0) {
            } /* IItscroll */
          } /* IItbsetio */
        }
/* # line 354 "imnaive.qsc" */	/* clear */
        {
          IIfldclear((char *)"subtitle");
        }
/* # line 355 "imnaive.qsc" */	/* host code */
	}
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 359 "imnaive.qsc" */	/* host code */
	/*
	** If already on the bottom row, do nothing, to avoid "out of data" msg
	*/
/* # line 362 "imnaive.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&row,29,_Choices,0);
              } /* IIiqset */
            }
/* # line 363 "imnaive.qsc" */	/* host code */
	if (row < last_row)
	{
/* # line 365 "imnaive.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
/* # line 366 "imnaive.qsc" */	/* host code */
	}
	else
	    ;
          }
        } else if (IIretval() == 2) {
          {
/* # line 372 "imnaive.qsc" */	/* host code */
	/*
	** If already on the top row, do nothing, to avoid "out of data" msg
	*/
/* # line 375 "imnaive.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&row,29,_Choices,0);
              } /* IIiqset */
            }
/* # line 376 "imnaive.qsc" */	/* host code */
        if (row == 1)
            ;
        else
        {
/* # line 380 "imnaive.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
/* # line 381 "imnaive.qsc" */	/* host code */
        }
          }
        } else if (IIretval() == 3) {
          {
            ER_MSGID selection;
/* # line 389 "imnaive.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_Form,_Choices,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&selection,_itemcode);
                IItcogetio((short *)0,1,30,4,&row,(char *)"_record");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 391 "imnaive.qsc" */	/* host code */
	if ( selection == F_IM0013_Queries )
	{
	    next_frame = IM_QUERY_MENU;
	    Scrollto_row = row;	/* set global for return */
/* # line 395 "imnaive.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 396 "imnaive.qsc" */	/* host code */
	}
	else
	{
	    runopt( selection );
/* # line 400 "imnaive.qsc" */	/* redisplay */
            {
              IIredisp();
            }
/* # line 401 "imnaive.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 4) {
          {
/* # line 407 "imnaive.qsc" */	/* host code */
/*	next_frame = ( behavior == IM_QUERY_MENU ) ? IM_TOP_MENU : IM_EXIT; */
	if (behavior == IM_QUERY_MENU)
	{
	    next_frame = IM_TOP_MENU;
/* # line 412 "imnaive.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 413 "imnaive.qsc" */	/* host code */
	}
	else
	    FTbell();	/* do nothing; ignore the keystroke */
          }
        } else if (IIretval() == 5) {
          {
/* # line 420 "imnaive.qsc" */	/* call */
            {
              IIutsys(0,(char *)"system",(char *)0);
              IIutsys(1,(char *)"command",(char *)"");
              IIutsys(2,(char *)0,(char *)0);
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 425 "imnaive.qsc" */	/* host code */
	/* Top and Query menus have different help files */
	if (behavior == IM_TOP_MENU)
	    FEhelp(ERx("immain.hlp"), ERget(F_IM001F_INGRES_MENU));
	else
	    FEhelp(ERx("imlang.hlp"), ERget(F_IM0023_INGRES_MENU___QUERIES));
          }
        } else if (IIretval() == 7) {
          {
/* # line 435 "imnaive.qsc" */	/* host code */
	if (behavior == IM_TOP_MENU)
	{
	    /* exit ingmenu */
	    next_frame = IM_EXIT;
	    Scrollto_row = 1;	/* set re-entry row number for when
				** being called from ABF
				*/
/* # line 442 "imnaive.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 443 "imnaive.qsc" */	/* host code */
	}
	else
	    FTbell();	/* do nothing; ignore the keystroke */
          }
        } else if (IIretval() == 8) {
          {
/* # line 451 "imnaive.qsc" */	/* host code */
	if (behavior == IM_TOP_MENU)
	{
	    /* exit ingmenu */
	    next_frame = IM_EXIT;
	    Scrollto_row = 1;	/* set re-entry row number for when
				** being called from ABF
				*/
/* # line 458 "imnaive.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 459 "imnaive.qsc" */	/* host code */
	}
	else
	    FTbell();	/* do nothing; ignore the keystroke */
          }
        } else {
          if (1) goto IIfdE1;
        } /* IIretval */
      } /* IIrunform */
IIfdF1:
      if (IIchkfrm() == 0) goto IIfdB1;
      goto IIfdE1;
IImuI1:
      if (IIactscrl(_Choices,0,1) == 0) goto IIfdE1;
      if (IIactscrl(_Choices,1,2) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Select),ERget(F_FE0108_ExplSelect),2,2,3) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(0,(char *)0,2,2,3) == 0) goto IIfdE1;
      if (IInmuact(im_cancel,ERget(F_IM0022_Return_to_top_form),2,2,4) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,2,2,4) == 0) goto IIfdE1;
      if (IInmuact(smenu,ERget(F_FE010A_ExplShell),2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(help_expl),2,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,6) == 0) goto IIfdE1;
      if (IInmuact(im_end,ERget(F_IM0020_Quit_Ingres_Menu),2,2,7) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,7) == 0) goto IIfdE1;
      if (IInmuact(im_quit,ERget(F_IM0020_Quit_Ingres_Menu),2,2,8) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,2,2,8) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 464 "imnaive.qsc" */	/* host code */
    _VOID_ IIFRkeystroke(NULL);
    return (next_frame);
  }
#if 0
static i4
chk_menu ( menu )
register MENUS	*menu;
  {
    i4 row;
	row = search_menu( menu->item, _Form, _Choices );
	if ( menu->item == F_IM0013_Queries )
	{
		Scrollto_row = row;	/* set global for return */
		return IM_QUERY_MENU;
	}
	else if ( row > 0 )
	{
/* # line 485 "imnaive.qsc" */	/* scroll */
    {
      if (IItbsetio(1,_Form,_Choices,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,row) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 486 "imnaive.qsc" */	/* host code */
		runopt( menu->item );
/* # line 487 "imnaive.qsc" */	/* redisplay */
    {
      IIredisp();
    }
/* # line 488 "imnaive.qsc" */	/* host code */
	}
	else
	{ /* message contains name of routine called above */
		IIUGerr( E_IM0025_Missing_menuitem, UG_ERR_ERROR,
			3, ERget(menu->item), (PTR) &row, ERx("search_menu")
		);
	}
	return IM_TOP_MENU;
  }
#endif
/*{
** Name:	load_top() - loads form fields for the top menu
**
** Description:
**	Prepares the form for display of the top-level menu.
**	Loads the tablefield with the menu options and clears the
**	subtitle.
**
** Inputs:
**	none
**
** Outputs:
**	Returns:
**		Count of rows loaded into tablefield
**
**	Exceptions:
**		none
**
** Side Effects:
**	none
**
** History:
**	04-oct-1988 (sandyd)
**		Written to implement user-interface changes.
*/
static i4
  load_top( im_menu, im_count )
  register MENUS *im_menu;
  register i4 im_count;
  {
    register i4	rowcnt = 0;
/* # line 532 "imnaive.qsc" */	/* clear */
    {
      IIfldclear(_Choices);
    }
/* # line 533 "imnaive.qsc" */	/* host code */
    while ( --im_count >= 0 )
    {
	if ( im_menu->menu != 0 )	/* if 0, then not authorized to run */
	{
/* # line 537 "imnaive.qsc" */	/* loadtable */
    {
      if (IItbact(_Form,_Choices,1) != 0) {
        IItcoputio(_item,(short *)0,1,32,0,im_menu->menu);
        IItcoputio(_explanation,(short *)0,1,32,0,ERget( im_menu->expl ));
        IItcoputio(_itemcode,(short *)0,1,30,4,&im_menu->item);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 543 "imnaive.qsc" */	/* host code */
	    ++rowcnt;
	}
	++im_menu;
    }
    return rowcnt;
  }
/*
**  run an option selected from menu.
*/
static void
runopt ( item )
ER_MSGID	item;
{
	switch (item)
	{
	case F_IM000D_Tables:
	    tablestart();
	    break;
	case F_IM000E_Forms:
	    vifredstart();
	    break;
	case F_IM000F_JoinDefs:
	    IIIMmqs_mqbfstart(QBF_JOIN);
	    break;
	case F_IM0010_Reports:
	    rbfstart();
	    break;
#ifdef OMIT /* removed 21-Feb-1995 */
	case F_IM0011_Graphs:
	    vigstart();
	    break;
#endif /* OMIT */
	case F_IM0012_Applications:
	case F_IM0031_VisionOnly:
	case F_IM0033_Vision_ABF:
	    abfstart(item);
	    break;
	/* selections in IM_QUERY_MENU */
	case F_IM0016_SQL:
	    ingresstart(H_SQL);
	    break;
	case F_IM0017_QUEL:
	    ingresstart(H_INGRES);
	    break;
	case F_IM0015_QBF:
	    IIIMmqs_mqbfstart(QBF_START);
	    break;
	default:
	    break;
	}
	return ;
}
/*
**	find a row with matching ER_MSGID in menu table field.
**	returns the row number where match occurred, or 0 if no match.
*/
static i4
  search_menu( menuitem, form, tblfld )
ER_MSGID menuitem;
  char *form;
  char *tblfld;
  {
    ER_MSGID item;
	register i4	i = 0;
	i4		j = 0;
/* # line 621 "imnaive.qsc" */	/* unloadtable */
    {
      if (IItbact(form,tblfld,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,30,4,&item,_itemcode);
        IITBceColEnd();
        {
/* # line 623 "imnaive.qsc" */	/* host code */
		++i;
		if ( item == menuitem )
		{
			j = i;
/* # line 627 "imnaive.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 628 "imnaive.qsc" */	/* host code */
		}
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 631 "imnaive.qsc" */	/* host code */
	return j;
  }
static void
imchkauth ( im_menu, im_menu_cnt, noabf )
register MENUS	*im_menu;
register i4	im_menu_cnt;
bool		noabf;	/* TRUE when ingmenu is called from ABF */
{
#ifdef IM_DEBUG
    char	*ret_val;
#endif
    while ( --im_menu_cnt >= 0 )
    {
#ifndef IM_DEBUG
	switch (im_menu->item)
	{
	case F_IM000D_Tables:
	    /* Tables has same authorization as INGMENU, the currently
	    running program, so it's available. */
	    im_menu->menu = ERget(im_menu->item);
	    break;
	case F_IM000E_Forms:
	    im_menu->menu =  ERget(im_menu->item);
	    break;
	case F_IM000F_JoinDefs:
	    im_menu->menu = ERget(im_menu->item);
	    break;
	case F_IM0010_Reports:
	    im_menu->menu = ERget(im_menu->item);
	    break;
#ifdef OMIT /* removed 21-Feb-1995 */
	case F_IM0011_Graphs:
	    im_menu->menu = ERget(im_menu->item);
	    break;
#endif /* OMIT */
	case F_IM0012_Applications:
	case F_IM0031_VisionOnly:
	case F_IM0033_Vision_ABF:
	    im_menu->menu = setAbfItems ( im_menu->item, noabf);
	    break;
	case F_IM0013_Queries:
	    im_menu->menu = ERget(im_menu->item);
	    break;
	/* selections in IM_QUERY_MENU */
	/* note that QUEL is not supported in a GATEWAY world */
	case F_IM0016_SQL:
	    im_menu->menu = ERget(im_menu->item);
	    break;
	case F_IM0017_QUEL:		/* -- mdw -- */
	    if (IIUIdml() != UI_DML_GTWSQL)
	    {
	        im_menu->menu = ERget(im_menu->item);
	    }
	    else
	    {
		im_menu->menu = NULL;
	    }
	    break;
	case F_IM0015_QBF:
	    im_menu->menu = ERget(im_menu->item);
	    break;
	default:
	    break;
	}
#else 
	/* for debug, set up script file like (use file set.env):
	setenv IMqbf y			*/
	switch (im_menu->item)
	{
	case F_IM000D_Tables:
	    /* Tables has same authorization as INGMENU, the currently
	    running program, so it's available. */
	    im_menu->menu = ERget(im_menu->item);
	    break;
	case F_IM000E_Forms:
	    NMgtAt("IMvifred", &ret_val);
	    im_menu->menu = (ret_val == NULL || *ret_val == EOS)
				? NULL : ERget(im_menu->item);
	    break;
	case F_IM000F_JoinDefs:
	    NMgtAt("IMqbf", &ret_val);
	    im_menu->menu = (ret_val == NULL || *ret_val == EOS)
				? NULL : ERget(im_menu->item);
	    break;
	case F_IM0010_Reports:
	    NMgtAt("IMrbf", &ret_val);
	    im_menu->menu = (ret_val == NULL || *ret_val == EOS)
				? NULL : ERget(im_menu->item);
	    break;
#ifdef OMIT /* removed 21-Feb-1995 */
	case F_IM0011_Graphs:
	    NMgtAt("IMvigraph", &ret_val);
	    im_menu->menu = (ret_val == NULL || *ret_val == EOS)
				? NULL : ERget(im_menu->item);
	    break;
#endif /* OMIT */
	case F_IM0012_Applications:
	case F_IM0031_VisionOnly:
	case F_IM0033_Vision_ABF:
	    im_menu->menu = setAbfItems ( im_menu->item, noabf);
	    break;
	case F_IM0013_Queries:
	    im_menu->menu = NULL ;
	    NMgtAt("IMsql_tm", &ret_val);
	    if (ret_val != NULL && *ret_val != EOS)
		im_menu->menu = ERget(im_menu->item);
	    if (IIUIdml() != UI_DML_GTWSQL)	/* mdw */
	    {
	        NMgtAt("IMquel_tm", &ret_val);
	        if (ret_val != NULL && *ret_val != EOS)
	    	    im_menu->menu = ERget(im_menu->item);
	    }
	    NMgtAt("IMqbf", &ret_val);
	    if (ret_val != NULL && *ret_val != EOS)
		im_menu->menu = ERget(im_menu->item);
	    break;
	/* selections in IM_QUERY_MENU */
	case F_IM0016_SQL:
	    NMgtAt("IMsql_tm", &ret_val);
	    im_menu->menu = (ret_val == NULL || *ret_val == EOS)
				? NULL : ERget(im_menu->item);
	    break;
	case F_IM0017_QUEL:
	    if (IIUIdml() != UI_DML_GTWSQL)	/* mdw */
	    {
	        NMgtAt("IMquel_tm", &ret_val);
	        im_menu->menu = (ret_val == NULL || *ret_val == EOS)
				? NULL : ERget(im_menu->item);
	    }
	    else
	    {
		im_menu->menu = NULL;
	    }
	    break;
	case F_IM0015_QBF:
	    NMgtAt("IMqbf", &ret_val);
	    im_menu->menu = (ret_val == NULL || *ret_val == EOS)
				? NULL : ERget(im_menu->item);
	    break;
	default:
	    break;
	}
#endif	/* IM_DEBUG */
    	++im_menu;
    }
    return;
}
/*{
** Name:	setAbfItems - turn on or off ABF/Vision menuitems.
**
** Description:
**	Set the name of a ABF &/or Vision menuitem ('item') for Ingmenu based
**	on capability bits.
**
**	Return either ERget(the passed in argument: 'item') or NULL.
**	Return ERget('item') for TRUE cells in following truth table;
**	  otherwise return NULL.
**
**				-- value of argument: 'item' --
**	ABF/Vision
**	Capabilities		ABF	Vision	ABF&Vision
**	------------		--------------------------------
**	Both Set		False	False	TRUE
**	Both Clear		False	False	False
**	ABF Set, Vision Clear	TRUE	False	False
**	ABF Clear, Vision Set	False	TRUE	False
**
** Inputs:
**	ER_MSGID item;		the menuline in ingmenu being evaluated.
**	bool	 noabf;		TRUE if ingmenu was called from ABF.
**
** Outputs:
**
**	Returns:
**		pointer to menuitem string or NULL
**
**	Exceptions:
**		NONE
**
** Side Effects:
**
** History:
**	2/6/90	(pete)	Initial Version.
*/
static char
*setAbfItems (item, noabf)
ER_MSGID item;
bool	 noabf;
{
#ifndef IM_DEBUG
#else
	char	*ret_val;
#endif	/* IM_DEBUG */
	bool abf_set;
	bool vision_set;
	char *menu = (char *) NULL;
	if (noabf)
	    return NULL;	/*no ABF/Vision menuitems when called from ABF*/
#ifndef IM_DEBUG
	abf_set    = TRUE ;
	vision_set =  TRUE ;
#else
	NMgtAt("IMabf", &ret_val);
	abf_set = (ret_val == NULL || *ret_val == EOS) ? FALSE : TRUE ;
	NMgtAt("IMvision", &ret_val);
	vision_set = (ret_val == NULL || *ret_val == EOS) ? FALSE : TRUE ;
#endif	/* IM_DEBUG */
	if ( abf_set && vision_set)
	    if (item == F_IM0033_Vision_ABF)
		menu = ERget(item);
	    else
		menu = (char *) NULL;
	else if (abf_set)
	    if (item == F_IM0012_Applications)
		menu = ERget(item);
	    else
		menu = (char *) NULL;
	else if (vision_set)
	    if (item == F_IM0031_VisionOnly)
		menu = ERget(item);
	    else
		menu = (char *) NULL;
	else
	    menu = (char *) NULL;
	return menu;
}
/*{
** Name:	key_catch	- Process keystrokes to catch the <CR>.
**
** Description:
**	Determine whether the user 'selected' the current row of the
**	tablefield by either pressing a carriage return or by clicking
**	the mouse on the row.
**
** Inputs:
**	cmd	FRS fdop code.
**	keytype	Type of keystroke: control key, function key or normal key.
**	chr	If applicable, the key typed by the user.
**	on_menu	Whether or not the user was on the menu line.
**
** Outputs:
**
**	Returns:
**		None.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	27-apr-90 (bruceb)	Written.
*/
static i4
key_catch(cmd, keytype, chr, on_menu)
i4	cmd;
i4	keytype;
char	*chr;
bool	on_menu;
{
    switch (keytype)
    {
    case CONTROL_KEY:
	if (!on_menu && *chr == '\r')
	{
	    return(FRS_RETURN);
	}
	/* Fall through. */
    case FUNCTION_KEY:
	if (cmd == fdopSELECT)
	{
	    return(FRS_RETURN);
	}
	/* Fall through. */
    default:
	return(FRS_PROCESS);
    }
}
