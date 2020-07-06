# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include <compat.h>
# include <ex.h>
# include <st.h>
# include <cm.h>
# include <er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include <fe.h>
# include <ug.h>
# include <uf.h>
# include <adf.h>
# include <afe.h>
# include <ft.h>
# include <fmt.h>
# include <frame.h>
# include <fsicnsts.h>
# include "eruf.h"
# include <flddesc.h>
/*
NO_OPTIM=dgi_us5
*/
/**
** Name:	listpck.qsc -	List choice frame
**
** Description:
**	Functions which allow callers to choose
**	from a list of items via a popup tablefield.
**
** Defines:
**
**	IIFDlpListPick	- Pick an item from a list.
**
** History:
**	24-apr-90 (bruceb)
**		Added mouse support for listpicks.  Clicking on a row in
**		a listpick will also select that row.  Also added explanatory
**		text for the Find, Top and Bottom menuitems.
**	25-apr-90 (bruceb)
**		No longer have do_find(), since keyfind is now in the FRS.
**	04/06/93 (dkh) - Fixed bug 47214.  Code was passing a FRAME pointer
**			 rather than the name of the form to delete.
**      24-nov-1993 (smc)
**		Bug #58882
**          	Made lfaddr a portable PTR rather than i4.
**	06-nov-1995 (canor01)
**		Quiet some compier warnings with obvious casts.
**	23-apr-1996 (chech02)
**		added function prototypes for windows 3.1 port.
**	06-aug-1999 (vande02)
**		added NO_OPTIM for dgi_us5 portability SIR 97548.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	13-May-2005 (kodse01)
**	    replace %ld with %d for old nat and long nat variables.
**    25-Oct-2005 (hanje04)
**        Add prototype for _runListPick() to prevent compiler
**        errors with GCC 4.0 due to conflict with implicit declaration.
**/
typedef i4	(*FUNCPTR)();
typedef struct {
	char	*title;
	char	*file;
} HELP;
typedef struct {
	i4	row;
	i4	col;
} POS;
FUNCPTR	IIFRkeystroke();
ADF_CB	*FEadfcb();
static FRAME	*build_list();
static i4	num_newl();
static bool	do_item();
static i4	key_catch();
#ifdef WIN16
static PTR      loc_alloc(TAGID, i4, char *);
static i4       _runListPick(PTR, PTR, PTR, PTR, PTR, i4  (*func)(), PTR);
#endif
static i4	Catch_cr;	/* equivalent menuitem to CR, < 0 for none */
static bool	Cr_select;	/* CR caught on FRSKEY 0 */
static bool	On_menu;
static i4	Om_count;	/* count of characters while on menu line */
static i4	Lcount = 0;
static READONLY char	_Empty[1]	= ERx("");
/* Activation Lists */
typedef struct {
	i4		n_items;	/* no. of items in list */
	LISTMENU	*items;
} ACT_LIST;
static READONLY LISTMENU	_cancelMenu[] =
{
    { FE_Select,	F_FE0108_ExplSelect,	LPB_CRSELECT,	4 },
/*  { 0,		0,			LPB_QUIT,	3 },*/
    { FE_Cancel,	F_FE0109_ExplForget,	LPB_QUIT,	9 },
    { 0,		F_FE0104_ExplFind,	LPB_FIND,	7 },
    { 0,		F_FE0105_ExplTop,	LPB_TOP,	5 },
    { 0,		F_FE0106_ExplBottom,	LPB_BOTTOM,	6 },
    { FE_Help,		F_FE0101_ExplKeyHelp,	LPB_HELP,	1 }
};
static ACT_LIST
	_cancel = { sizeof(_cancelMenu)/sizeof(_cancelMenu[0]), 
                    (LISTMENU *) _cancelMenu };
static READONLY LISTMENU	_endMenu[] =
{
    { FE_Select,	F_FE0108_ExplSelect,	LPB_CRSELECT,	4 },
    { 0,		F_FE0104_ExplFind,	LPB_FIND,	7 },
    { 0,		F_FE0105_ExplTop,	LPB_TOP,	5 },
    { 0,		F_FE0106_ExplBottom,	LPB_BOTTOM,	6 },
    { FE_Help,		F_FE0101_ExplKeyHelp,	LPB_HELP,	1 },
    { FE_End,		F_FE0102_ExplEnd,	LPB_QUIT,	3 }
};
static ACT_LIST
	_end = { sizeof(_endMenu)/sizeof(_endMenu[0]), 
                 (LISTMENU *) _endMenu };
static READONLY LISTMENU	_forceMenu[] =
{
    { FE_Select,	F_FE0108_ExplSelect,	LPB_CRSELECT,	4 },
    { 0,		F_FE0104_ExplFind,	LPB_FIND,	7 },
    { 0,		F_FE0105_ExplTop,	LPB_TOP,	5 },
    { 0,		F_FE0106_ExplBottom,	LPB_BOTTOM,	6 },
    { FE_Help,		F_FE0101_ExplKeyHelp,	LPB_HELP,	1 }
};
static ACT_LIST
	_force = { sizeof(_forceMenu)/sizeof(_forceMenu[0]), 
                   (LISTMENU *) _forceMenu };
/* prototype _runListPick here because it uses some of the above defines */
static i4 _runListPick(
			char	*form,
			char	*tblfld,
			POS	*pos,
			HELP	*help,
			ACT_LIST *actv,
			i4	(*func)(),
			PTR	data);
/*{
** Name:	IIFDlpListPick() -	Pick from a list.
**
** Description:
**    Chooses an item from a list of items provided as an encoded
**    string by the caller.  Provides a canned tablefield select
**    frame, essentially.
**
**   String format:
**	String consists of tablefield rows described by newline
**	separated fields.  Each of these fields has tablefield
**	column entries separated by semicolons.  Number and size of
**	tablefield columns determined by maximal rows.  Omitted
**	columns in a given row are assumed to be empty.  Empty
**	columns in the middle of a row may be indicated by ";;".
**	Embedded semicolons in the text cannot be expressed.
**
**   Display:
**	Display takes the form of a popup read-only tablefield with a
**	boxed, possibly multi-line title, and "highlight current row"
**	turned on.  Standard menuitems "Select", "Forget" and "Help"
**	appear.  After user chooses an item or "Forget", popup
**	goes away.  An empty string for the prompt results in a
**	a titleless tablefield.
**
**   Handler function:
**	Is used to allow further interaction underneath a choice:
**	Calling sequence:
**
**	(*func)(dat,choice,resume)
**	PTR dat;
**	i4 choice;
**	bool *resume;
**
**	Handler returns resume = TRUE if user is supposed to
**	stay on current choice list, FALSE if IIFDlpListPick
**	is to return.  If resume = FALSE, the return value of
**	handler will be returned, rather than the users original
**	choice.
**
**	resume is passed in FALSE, and may be ignored if that
**	is always the desired action.
**
**	NOTE: handler WILL get called for a user "forget" choice
**	to allow maximum flexibility.  In many cases, handler
**	doesn't really want this, but an
**	"if (choice < 0) return (choice);" isn't that much of an
**	imposition.
**    
** Inputs:
**    char *prompt	- prompt string, ie. tablefield title.
**    char *string	- string-encoded choice list.
**    i4   rows		- tablefield rows.  0 to specify display of as many
**			  rows as there are items.  If there are less rows
**			  in string than this number, the tablefield is
**			  that length, rather than this value.
**    i4  srow, scol	- row and column for placement of list. < 0 to
**			  float.  > 0 FRS 1 BASED ADDRESSING!!
**    char *htitle	- help screen title.
**    char *hfile	- help file.
**    i4  (*func)()	- handler function - may be NULL.
**    PTR dat		- data to pass to handler.
**
** Returns:
**	{nat}  numeric choice from list, or < 0 for "forget".
**
**	Syntax errors in the encoded string or serious forms system
**	errors preventing the display will result in explanatory
**	messages by this routine, and "forget" returned to caller.
**	
** Side effects:
**    allocates / frees memory.
**
** History:
**    3/89 (bobm)    written
*/
i4
IIFDlpListPick(prompt,string,rows,srow,scol,htitle,hfile,func,dat)
char	*prompt;
char	*string;
i4	rows;
i4	srow;
i4	scol;
char	*htitle;
char	*hfile;
i4	(*func)();
PTR	dat;
{
    FRAME	*frm;
    i4		choice;
    TAGID	tag;
    frm = build_list(&tag, prompt,string,rows);
    if (frm == NULL)
	return (-1);
    choice = IIFRrlpRunListPick(frm->frname, frm->frname,
			srow, scol, htitle, hfile, func, dat);
    IIUGtagFree(tag);
    IIdelfrm(frm->frname);
    return (choice);
}
/*{
** Name:	IIFDlpeListPickEnd() -	Pick from a list.
**
** Description:
**    This function is a cover for the regular listpick function
**    the only difference is that this one uses the "End" menuitem
**    instead of "Cancel".
**
** History:
**    2/13/90 (tom)    written
**	06-nov-1995 (canor01)
**	    Do not show 'help' menuitem if no helpfile.
*/
i4
IIFDlpeListPickEnd(prompt,string,rows,srow,scol,htitle,hfile,func,dat)
char	*prompt;
char	*string;
i4	rows;
i4	srow;
i4	scol;
char	*htitle;
char	*hfile;
i4	(*func)();
PTR	dat;
{
	FRAME	*frm;
	i4	choice;
	TAGID	tag;
	HELP	*help;
	HELP	_help;
	POS	start;
	if ( (frm = build_list(&tag, prompt, string, rows)) == NULL)
		return -1;
	if ( htitle == NULL || hfile == NULL )
		help = NULL;
	else
	{
		help = &_help;
		_help.title = htitle;
		_help.file = hfile;
	}
	start.row = srow;
	start.col = scol;
	choice = _runListPick( frm->frname, frm->frname, &start, help, &_end,
				func, dat
	);
	IIUGtagFree(tag);
	IIdelfrm(frm->frname);
	return choice;
}
/*{
** Name:	IIFDfpForcedPick() -	Pick from a list.
**
** Description:
**    Chooses an item from a list of items provided as an encoded
**    string by the caller.  An alternate version of IIFRlp* which does
**    not provide a Cancel key, so that user is forced to make a choice.
**
** Inputs:
**    char *prompt	- prompt string, ie. tablefield title.
**    char *string	- string-encoded choice list.
**    i4   rows		- tablefield rows.  0 to specify display of as many
**			  rows as there are items.  If there are less rows
**			  in string than this number, the tablefield is
**			  that length, rather than this value.
**    i4  srow, scol	- row and column for placement of list. < 0 to
**			  float.  > 0 FRS 1 BASED ADDRESSING!!
**    UF_HELP *help	- help title/file.
**    i4  (*func)()	- handler function - may be NULL.
**    PTR dat		- data to pass to handler.
**
** Outputs:
**    Returns:
**	numeric choice from list, or < 0 for "forget".
**
**	Syntax errors in the encoded string or serious forms system
**	errors preventing the display will result in explanatory
**	messages by this routine, and "forget" returned to caller.
**	
** Side effects:
**    allocates / frees memory.
**
*/
i4
IIFDfpForcedPick ( prompt, string, rows, srow, scol, help, func, dat )
char	*prompt;
char	*string;
i4	rows;
i4	srow;
i4	scol;
UF_HELP	*help;
i4	(*func)();
PTR	dat;
{
    FRAME	*frm;
    i4		choice;
    TAGID	tag;
    frm = build_list(&tag, prompt,string,rows);
    if (frm == NULL)
	return (-1);
    choice = IIFRrfpRunForcedPick(frm->frname, frm->frname,
			srow, scol, help, func, dat);
    IIUGtagFree(tag);
    IIdelfrm(frm->frname);
    return (choice);
}
/*{
** Name:	IIFRrlpRunListPick() -	run list pick form.
**
** Description:
**    Chooses an item from a list of items on the tablefield of
**    the passed in form.
**
** Inputs:
**    char *form	- name of form.
**    char *tbl		- name of tablefield.
**    i4  srow, scol	- row and column for placement of list. < 0 to
**			  float.  > 0 FRS 1 BASED ADDRESSING!!
**    char *htitle	- help screen title.
**    char *hfile	- help file.
**    i4  (*func)()	- handler function - may be NULL.
**    PTR dat		- data to pass to handler.
**
**   Handler function:
**	Is used to allow further interaction underneath a choice:
**	Calling sequence:
**
**	(*func)(dat,choice,resume)
**	PTR dat;
**	i4 choice;
**	bool *resume;
**
**	Handler returns resume = TRUE if user is supposed to
**	stay on current choice list, FALSE if IIFRlpListPick
**	is to return.  If resume = FALSE, the return value of
**	handler will be returned, rather than the users original
**	choice.
**
**	resume is passed in FALSE, and may be ignored if that
**	is always the desired action.
**
**	NOTE: handler WILL get called for a user "forget" choice
**	to allow maximum flexibility.  In many cases, handler
**	doesn't really want this, but an
**	"if (choice < 0) return (choice);" isn't that much of an
**	imposition.
**
**  NULL htitle may be used to indicate no "Help" item.
**
** Outputs:
**    Returns:
**	numeric choice from list, or < 0 for "forget".
**
** History:
**	06-nov-1995 (canor01)
**	    Do not show 'help' menuitem if no helpfile.
*/
i4
IIFRrlpRunListPick ( form, tbl, srow, scol, htitle, hfile, func, dat )
char *form;
char *tbl;
i4  srow;
i4  scol;
char *htitle;
char *hfile;
i4  (*func)();
PTR dat;
{
	HELP	*help;
	HELP	_help;
	POS	start;
	if ( htitle == NULL || hfile == NULL )
		help = NULL;
	else
	{
		help = &_help;
		_help.title = htitle;
		_help.file = hfile;
	}
	start.row = srow;
	start.col = scol;
	return _runListPick( form, tbl, &start, help, &_cancel, func, dat );
}
static i4
_runListPick ( form, tblfld, pos, help, actv, func, data )
char	*form;
char	*tblfld;
POS	*pos;
HELP	*help;
ACT_LIST *actv;
i4	(*func)();
PTR	data;
{
	LISTPICK lpk;
	lpk.form = form;
	lpk.tf = tblfld;
	lpk.srow = pos->row;
	lpk.scol = pos->col;
	lpk.lineno = 0;
	lpk.menuitems = actv->items;
	/*
	** this reflects an assumption that the last member
	** of the activations list is the help menu-item.
	*/
	lpk.nomenitems = actv->n_items;
	if ( help == NULL )
	{ /* no help */
		--(lpk.nomenitems);
		lpk.htitle = lpk.hfile = NULL;
	}
	else
	{
		lpk.htitle = help->title;
		lpk.hfile = help->file;
	}
	lpk.handler = func;
	lpk.data = data;
	lpk.flags = 0;
	return IIFRclpCallListPick(&lpk, (i4 *)NULL);
}
/*{
** Name:	IIFRrfpRunForcedPick() -	run forced list pick form.
**
** Description:
**	This is an alternate form of IIFRrlp* in which no cancel
**	item is available, in other words the user can't leave the
**	form without making a choice.
**
** Inputs:
**    char *form	- name of form.
**    char *tbl		- name of tablefield.
**    i4  srow, scol	- row and column for placement of list. < 0 to
**			  float.  > 0 FRS 1 BASED ADDRESSING!!
**    UF_HELP *help	- help title/file.
**    i4  (*func)()	- handler function - may be NULL.
**    PTR dat		- data to pass to handler.
**
**   Handler function:
**	Is used to allow further interaction underneath a choice:
**	Calling sequence:
**
**	(*func)(dat,choice,resume)
**	PTR dat;
**	i4 choice;
**	bool *resume;
**
**	Handler returns resume = TRUE if user is supposed to
**	stay on current choice list, FALSE if IIFRlpListPick
**	is to return.  If resume = FALSE, the return value of
**	handler will be returned, rather than the users original
**	choice.
**
**	resume is passed in FALSE, and may be ignored if that
**	is always the desired action.
**
**	NOTE: handler WILL get called for a user "forget" choice
**	to allow maximum flexibility.  In many cases, handler
**	doesn't really want this, but an
**	"if (choice < 0) return (choice);" isn't that much of an
**	imposition.
**
**  NULL help may be used to indicate no "Help" item.
**
** Returns:
**	{nat}  numeric choice from list, or < 0 for "forget".
**
** History:
**    2/13/90 (tom)    written
*/
i4
IIFRrfpRunForcedPick ( form, tbl, srow, scol, help, func, dat )
char	*form;
char	*tbl;
i4	srow;
i4	scol;
UF_HELP	*help;
i4  (*func)();
PTR dat;
{
    LISTPICK lpk;
    lpk.form = form;
    lpk.tf = tbl;
    lpk.srow = srow;
    lpk.scol = scol;
    lpk.lineno = 0;
    lpk.menuitems = _force.items;
    /*
    ** this reflects an assumption that the last member
    ** of the '_force' activations list is the help menu-item.
    */
    lpk.nomenitems = _force.n_items;
    if ( help == NULL )
    {
	--(lpk.nomenitems);
	lpk.htitle = NULL;
	lpk.hfile = NULL;
    }
    else
    {
	lpk.htitle = ERget(help->title);
	lpk.hfile = help->file;
    }
    lpk.handler = func;
    lpk.data = dat;
    lpk.flags = 0;
    return IIFRclpCallListPick(&lpk, (i4 *)NULL);
}
/*{
** Name:	IIFRclpCallListPick()
**
** Description:
**	Does underlying work for the ListPick routines, using LISTPICK
**	structure. May be called directly to allow customized menu items,
**	& control of starting row to put cursor on.
**	(See /front/utils/../tblutil/tutblpik.qsc for example
**	 code that calls this directly & uses custom menu items).
**
** Inputs:
**	lpk - LISTPICK structure defining options for listpick
**	i4 *menuitem	- gets set to menuitem number user selected (see
**			Outputs section below).
**
** Outputs:
**	menuitem - array index for number of the menuitem chosen (LISTMENU
**			array), if menuitem is non-NULL.
**
**	returns:
**		(_RECORD - 1) of the table field row the user Selected (or -1
**		if user selected Forget).
**		Note that it is actually the user-written handler routine
**		that sets the return code, so handler should be careful to
**		return something that will be returned later as a _RECORD -1
**		value.
**
** History:
**   	03/31/93 (DonC) Bug 44227
**		On TIMEOUT this routine was passing an uninitialized return
**		value back to the caller.  This caused the look_up function
**		to return a trashed return value to 3 and 4GL applications
**		and overwrote application fields with whatever the cursor was on
**		when the timeout occurred. 
*/
i4
IIFRclpCallListPick(lpk, menuitem)
LISTPICK *lpk;
i4  *menuitem;
  {
    i4		fret;
    i4		mi;
    i4		(*fptr)();
    i4 rows;
    i4 crec;
    i4 srow, scol;
    i4 hasbord;
    char *form, *tbl;
    char *mu[15];
    char *mexp[15];
    i4 frsk[15];
    form = lpk->form;
    tbl = lpk->tf;
    fret = -1;          /* Bug 44227 (DonC): Initialize the return value */
    hasbord = (lpk->flags & LPK_BORDER) != 0 ? FSBD_DEF : 0;
    /*
    ** set up menu items, explanations and frskeys.
    */
    Catch_cr = -1;
    {
	register i4	i;
	for ( i = 15 ; --i >= 0 ; )
	{
		register LISTMENU	*lm = &lpk->menuitems[i];
		if ( i >= lpk->nomenitems )
		{ /* extra */
			mu[i] = mexp[i] = NULL;
			frsk[i] = -1;
		}
		else
		{
			if (lm->behavior == LPB_CRSELECT && Catch_cr < 0)
				Catch_cr = i;
			mu[i] = ( lm->title != 0 ) ? ERget(lm->title) : NULL;
			mexp[i] = ( lm->explan != 0 )
					? ERget( lm->explan ) : NULL;
			frsk[i] = lm->frskey > 0 ? lm->frskey : -1;
		}
	}
    }
/* # line 686 "listpck.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,form,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&rows,32,tbl,0);
      } /* IIiqset */
    }
/* # line 687 "listpck.qsc" */	/* host code */
    if (lpk->srow < LPGR_FLOAT || lpk->scol < LPGR_FLOAT)
    {
      i4 drow, dcol;
      i4 frow, fcol;
/* # line 692 "listpck.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&drow,44,(char *)0,0);
          IIiqfsio((short *)0,1,30,4,&dcol,45,(char *)0,0);
        } /* IIiqset */
      }
/* # line 693 "listpck.qsc" */	/* inquire_frs */
      {
        if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&frow,44,form,0);
          IIiqfsio((short *)0,1,30,4,&fcol,45,form,0);
        } /* IIiqset */
      }
/* # line 695 "listpck.qsc" */	/* host code */
	srow = drow - frow;
	scol = dcol - fcol;
    }
    switch (lpk->srow)
    {
    case LPGR_CENTER:
	/* assert: srow == drow - frow */
	srow =  1 + srow/2;
	break;
    case LPGR_BOTTOM:
	/*
	** next to last line - we have to remember that there's a menu line.
	*/
	/* assert: srow == drow - frow */
	break;
    case LPGR_TOP:
	srow = 1;
	break;
    default:
	/* assert: lpk->srow == LPGR_FLOAT || lpk->srow > 0 */
	srow = lpk->srow;
	break;
    }
    switch (lpk->scol)
    {
    case LPGR_CENTER:
	/* assert: scol == dcol - fcol */
	scol = 1 + scol/2;
	break;
    case LPGR_RIGHT:
	/* assert: scol == dcol - fcol */
	scol += 1;
	break;
    case LPGR_LEFT:
	scol = 1;
	break;
    default:
	/* assert: lpk->scol == LPGR_FLOAT || lpk->scol > 0 */
	scol = lpk->scol;
	break;
    }
    On_menu = FALSE;
    fptr = IIFRkeystroke(key_catch);
/* # line 743 "listpck.qsc" */	/* display */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpsetio(5,(short *)0,1,30,4,&hasbord);
      IIFRgpsetio(1,(short *)0,1,30,4,&srow);
      IIFRgpsetio(2,(short *)0,1,30,4,&scol);
      IIFRgpcontrol(2,0);
      if (IIdispfrm(form,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 748 "listpck.qsc" */	/* host code */
	if ((crec = lpk->lineno) < rows && crec > 0)
	{
/* # line 750 "listpck.qsc" */	/* scroll */
        {
          if (IItbsetio(1,form,tbl,-3) != 0) {
            IItbsmode((char *)"to");
            if (IItscroll(0,crec) != 0) {
            } /* IItscroll */
          } /* IItbsetio */
        }
/* # line 751 "listpck.qsc" */	/* host code */
	}
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 756 "listpck.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form,tbl,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&crec,(char *)"_RECORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 757 "listpck.qsc" */	/* host code */
	if (crec == 1)
	{
	    FTbell();
	    /*
	    ** this resume is neccesary to prevent multiple bells when the
	    ** scroll keys are used.  Apparently multiple activations
	    ** happen, 1 per row, unless you do this.
	    */
/* # line 765 "listpck.qsc" */	/* resume */
            {
              IIresfld(tbl);
              if (1) goto IIfdB1;
            }
/* # line 766 "listpck.qsc" */	/* host code */
	}
	else
	{
/* # line 769 "listpck.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form,tbl,-3) != 0) {
                IItbsmode((char *)"down");
                if (IItscroll(0,-3) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 770 "listpck.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 2) {
          {
/* # line 775 "listpck.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form,tbl,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&crec,(char *)"_RECORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 777 "listpck.qsc" */	/* host code */
/* Note that 'rows' is set at startup. therefore, handler
** should not change the number of rows in the table field.
** (maybe do_item should reset 'rows' ?)
*/
	if (crec == rows)
	{
	    FTbell();
	    /*
	    ** this resume is neccesary to prevent multiple bells when the
	    ** scroll keys are used.  Apparently multiple activations
	    ** happen, 1 per row, unless you do this.
	    */
/* # line 789 "listpck.qsc" */	/* resume */
            {
              IIresfld(tbl);
              if (1) goto IIfdB1;
            }
/* # line 790 "listpck.qsc" */	/* host code */
	}
	else
	{
/* # line 793 "listpck.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form,tbl,-3) != 0) {
                IItbsmode((char *)"up");
                if (IItscroll(0,-3) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 794 "listpck.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 3) {
          {
/* # line 799 "listpck.qsc" */	/* host code */
	if ( Cr_select && ! do_item(lpk,mi = Catch_cr,&fret) )
	{
/* # line 801 "listpck.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 802 "listpck.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 4) {
          {
/* # line 807 "listpck.qsc" */	/* host code */
	if (! do_item(lpk,mi = 0,&fret))
	{
/* # line 809 "listpck.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 810 "listpck.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 5) {
          {
/* # line 815 "listpck.qsc" */	/* host code */
	if (! do_item(lpk,mi = 1,&fret))
	{
/* # line 817 "listpck.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 818 "listpck.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 6) {
          {
/* # line 823 "listpck.qsc" */	/* host code */
	if (! do_item(lpk,mi = 2,&fret))
	{
/* # line 825 "listpck.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 826 "listpck.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 7) {
          {
/* # line 831 "listpck.qsc" */	/* host code */
	if (! do_item(lpk,mi = 3,&fret))
	{
/* # line 833 "listpck.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 834 "listpck.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 8) {
          {
/* # line 839 "listpck.qsc" */	/* host code */
	if (! do_item(lpk,mi = 4,&fret))
	{
/* # line 841 "listpck.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 842 "listpck.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 9) {
          {
/* # line 847 "listpck.qsc" */	/* host code */
	if (! do_item(lpk,mi = 5,&fret))
	{
/* # line 849 "listpck.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 850 "listpck.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 10) {
          {
/* # line 855 "listpck.qsc" */	/* host code */
	if (! do_item(lpk,mi = 6,&fret))
	{
/* # line 857 "listpck.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 858 "listpck.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 11) {
          {
/* # line 863 "listpck.qsc" */	/* host code */
	if (! do_item(lpk,mi = 7,&fret))
	{
/* # line 865 "listpck.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 866 "listpck.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 12) {
          {
/* # line 871 "listpck.qsc" */	/* host code */
	if (! do_item(lpk,mi = 8,&fret))
	{
/* # line 873 "listpck.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 874 "listpck.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 13) {
          {
/* # line 879 "listpck.qsc" */	/* host code */
	if (! do_item(lpk,mi = 9,&fret))
	{
/* # line 881 "listpck.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 882 "listpck.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 14) {
          {
/* # line 887 "listpck.qsc" */	/* host code */
	if (! do_item(lpk,mi = 10,&fret))
	{
/* # line 889 "listpck.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 890 "listpck.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 15) {
          {
/* # line 895 "listpck.qsc" */	/* host code */
	if (! do_item(lpk,mi = 11,&fret))
	{
/* # line 897 "listpck.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 898 "listpck.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 16) {
          {
/* # line 903 "listpck.qsc" */	/* host code */
	if (! do_item(lpk,mi = 12,&fret))
	{
/* # line 905 "listpck.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 906 "listpck.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 17) {
          {
/* # line 911 "listpck.qsc" */	/* host code */
	if (! do_item(lpk,mi = 13,&fret))
	{
/* # line 913 "listpck.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 914 "listpck.qsc" */	/* host code */
	}
          }
        } else if (IIretval() == 18) {
          {
/* # line 919 "listpck.qsc" */	/* host code */
	if (! do_item(lpk,mi = 14,&fret))
	{
/* # line 921 "listpck.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 922 "listpck.qsc" */	/* host code */
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
      if (IIactscrl(tbl,1,1) == 0) goto IIfdE1;
      if (IIactscrl(tbl,0,2) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(0,(char *)0,2,2,3) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(mu[0],mexp[0],2,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(frsk[0],mexp[0],2,2,4) == 0) goto IIfdE1;
      if (IInmuact(mu[1],mexp[1],2,2,5) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(frsk[1],mexp[1],2,2,5) == 0) goto IIfdE1;
      if (IInmuact(mu[2],mexp[2],2,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(frsk[2],mexp[2],2,2,6) == 0) goto IIfdE1;
      if (IInmuact(mu[3],mexp[3],2,2,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(frsk[3],mexp[3],2,2,7) == 0) goto IIfdE1;
      if (IInmuact(mu[4],mexp[4],2,2,8) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(frsk[4],mexp[4],2,2,8) == 0) goto IIfdE1;
      if (IInmuact(mu[5],mexp[5],2,2,9) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(frsk[5],mexp[5],2,2,9) == 0) goto IIfdE1;
      if (IInmuact(mu[6],mexp[6],2,2,10) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(frsk[6],mexp[6],2,2,10) == 0) goto IIfdE1;
      if (IInmuact(mu[7],mexp[7],2,2,11) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(frsk[7],mexp[7],2,2,11) == 0) goto IIfdE1;
      if (IInmuact(mu[8],mexp[8],2,2,12) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(frsk[8],mexp[8],2,2,12) == 0) goto IIfdE1;
      if (IInmuact(mu[9],mexp[9],2,2,13) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(frsk[9],mexp[9],2,2,13) == 0) goto IIfdE1;
      if (IInmuact(mu[10],mexp[10],2,2,14) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(frsk[10],mexp[10],2,2,14) == 0) goto IIfdE1;
      if (IInmuact(mu[11],mexp[11],2,2,15) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(frsk[11],mexp[11],2,2,15) == 0) goto IIfdE1;
      if (IInmuact(mu[12],mexp[12],2,2,16) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(frsk[12],mexp[12],2,2,16) == 0) goto IIfdE1;
      if (IInmuact(mu[13],mexp[13],2,2,17) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(frsk[13],mexp[13],2,2,17) == 0) goto IIfdE1;
      if (IInmuact(mu[14],mexp[14],2,2,18) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(frsk[14],mexp[14],2,2,18) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 925 "listpck.qsc" */	/* host code */
    if (menuitem != NULL)
	*menuitem = mi;
    IIFRkeystroke(fptr);
    return fret;
  }
/*
** Name:	do_item() -	Do the real work of handling a menuitem.
**
** Description:
**	Call the handler routine ("user handler") pointed to by lpk->handler
**	(if not NULL). Call in one of two ways, based on the value of 
**	(lpk->menuitems).behavior) for the chosen menuitem in.
**
**      User's handler is passed the arguments:
**	lpk->data	(PTR) set by original ListPick caller. Data to be
**			used by handler.
**	*fret		_RECORD - 1 of tblfld row cursor was on when user
**			selected a menuitem (or -1 if user selected Forget).
**			Return code from handler is written here.
**			IIFRclpCallListPick() then uses this as its return
**			value. **So, handler should return an unchanged version
**			of this argument as its return value, unless it wants to
**			reset the table field row number.
**	&res		This pointer to a boolean should be set by handler
**			to TRUE if IIFRclpCallListPick() should
**			resume & set to FALSE if it should return to the
**			original ListPick caller. This becomes the return value.
**	idx		The menuitem number the user selected (0 based
**			addressing).
**
** Inputs:
**	LISTPICK *lpk;	Data structure used by ListPick.
**	i4 idx;	Menuitem number the user selected
**	i4 *fret;	Tablefield dataset row number -1 that user was on
**			before they selected a menuitem. return code from
**			user's handler will be put here
**
** Outputs:
**	Returns:
**		bool	(see above for description)
**		(also Writes a value to *fret)
**
**	Exceptions:
**		NONE
**
** Side Effects:
**	Calls the handler function in '*lpk', if it's not NULL.
**
** History:
*/
static bool
do_item(lpk,idx,fret)
LISTPICK *lpk;
i4  idx;	/* menuitem number the user selected */
i4  *fret;	/* return code from user's handler will be put here */
  {
	bool res;
    i4 crec;
    char *form, *tbl;
	i4 (*func)();
	i4 (*ifunc)();
	i4 s_catch;
	bool s_select;
	char s_c[3];
	i4 s_count;
	res = TRUE;
	form = lpk->form;
	tbl = lpk->tf;
	func = lpk->handler;
	/*
	** save keystroke capture status in case we have recursive calls.
	** Will restore after switch.
	*/
	ifunc = IIFRkeystroke(NULL);
	s_catch = Catch_cr;
	s_select = Cr_select;
	s_count = Om_count;
	switch ((lpk->menuitems)[idx].behavior)
	{
	 case LPB_SELECT:
	 case LPB_CRSELECT:
		/* intended to handle custom menuitems and "Select" */
		res = FALSE;
/* # line 1017 "listpck.qsc" */	/* getrow */
    {
      if (IItbsetio(2,form,tbl,-2) != 0) {
        IItcogetio((short *)0,1,30,4,&crec,(char *)"_RECORD");
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1018 "listpck.qsc" */	/* host code */
		*fret = crec - 1;
		if (func != NULL)
			*fret = (*func)(lpk->data,*fret,&res,idx);
		break;
	 case LPB_QUIT:
		/* intended to handle menuitem "Forget" */
		res = FALSE;
		*fret = -1;
		if (func != NULL)
			*fret = (*func)(lpk->data,-1,&res,idx);
		break;
	 case LPB_TOP:
/* # line 1030 "listpck.qsc" */	/* scroll */
    {
      if (IItbsetio(1,form,tbl,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,1) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 1031 "listpck.qsc" */	/* host code */
		break;
	 case LPB_BOTTOM:
/* # line 1033 "listpck.qsc" */	/* scroll */
    {
      if (IItbsetio(1,form,tbl,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,-1) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 1034 "listpck.qsc" */	/* host code */
		break;
	 case LPB_FIND:
		FEtabfnd(form, tbl);
		break;
	 case LPB_HELP:
		FEhelp(lpk->hfile, lpk->htitle);
		break;
	 default:
		break;
	}
	/* restore context */
	Catch_cr = s_catch;
	Cr_select = s_select;
	Om_count = s_count;
	On_menu = FALSE;
	IIFRkeystroke(ifunc);
	return res;
  }
static i4
ex_hdlr(ex_args)
EX_ARGS ex_args;
{
    if (ex_args.exarg_num == EXFEMEM)
	return (EXDECLARE);
    return (EXRESIGNAL);
}
/*
** allocator used under build_list - returns only if successful - generates
** exception on failure.  If str argument is non-NULL, a string allocation
** is desired, and size is irrelevent.
*/
static PTR
loc_alloc(tag, size,str)
TAGID	tag;
i4	size;
char	*str;
{
    PTR mem;
    if (str != NULL)
	size = STlength(str)+1;
    mem = FEreqmem((u_i4)tag, (u_i4)size, FALSE, (STATUS *)NULL);
    if (mem == NULL)
	EXsignal(EXFEMEM,0);
    if (str != NULL)
	STcopy(str,(char *) mem);
    return (mem);
}
static FRAME *
build_list(tag, prompt,string,rows)
TAGID *tag;
char *prompt;
char *string;
i4  rows;
  {
    EX_CONTEXT	ex;
    i4		rwcnt;
    i4		numcol;
    i4		i,j;
    i4		len;
    FRAME	*lf;
    PTR lfaddr;
    char	*tempstr;
    char	*sptr, **tptr;
    char	nbuf[FE_MAXNAME+1];
    char *name;
    char	**coldata;
    FLD_DESC	*fds;
    DB_DATA_VALUE	*dbd;
    if (EXdeclare(ex_hdlr, &ex) != OK)
    {
	IIUGtagFree(*tag);
	IIUGerr(E_UF0027_BadEXSet,0,0);
	EXdelete();
	return (NULL);
    }
    /*
    ** num_newl returns number of newlines,  and maximum number
    ** of semicolon delimiters in any one line
    */
    rwcnt = num_newl(string,&numcol);
    if (rows == 0 || rows > rwcnt)
	rows = rwcnt;
    if (rows == 0 || numcol == 0)
    {
	IIUGerr(E_UF0028_BadListParm,0,2,&rows,&numcol);
	EXdelete();
	return (NULL);
    }
    *tag = FEgettag();
    name = (char *) loc_alloc(*tag, 0, STprintf(nbuf,"iilp%d",Lcount++));
    fds = (FLD_DESC *) loc_alloc(*tag, sizeof(FLD_DESC)*numcol,NULL);
    dbd = (DB_DATA_VALUE *) loc_alloc(*tag, sizeof(DB_DATA_VALUE)*numcol,NULL);
    coldata = (char **) loc_alloc(*tag, sizeof(char *)*rwcnt*numcol,NULL);
    for (i=0; i < numcol; ++i)
    {
	fds[i].name = loc_alloc(*tag, 0, STprintf(nbuf,"c%d",i));
	fds[i].type = dbd+i;
	fds[i].maxwidth = 0;
	dbd[i].db_length = 1;
	dbd[i].db_prec = 0;
	dbd[i].db_datatype = DB_CHA_TYPE;
    }
    /*
    ** we allocate a copy of the string, then break it up into
    ** the individual column pieces, filling in the column widths
    ** based on maximum string length as we go.
    */
    tempstr = (char *) loc_alloc(*tag, 0,string);
    tptr = coldata;
    for (i = 0; i < rwcnt; ++i)
    {
	sptr = tempstr;
	tempstr = STindex(tempstr,ERx("\n"),0);
	/*
	** can only come up NULL on last iteration if our
	** counts are correct.  Set it empty for safety, though
	*/
	if (tempstr != NULL)
	    *tempstr++ = EOS;
	else
	    tempstr = (char *) _Empty;
	for ( j = 0 ; j < numcol ; ++j )
	{
	    *tptr = sptr;
	    sptr = STindex(sptr,ERx(";"),0);
	    if (sptr != NULL)
		*sptr++ = EOS;
	    else
		sptr = (char *) _Empty;
	    if ((len = STlength(*tptr)) > dbd[j].db_length)
		dbd[j].db_length = len;
	    ++tptr;
	}
    }
    if (IIFRmdfMakeDefaultForm(prompt,name,FALSE,numcol,fds,rows,&lf) != OK)
    {
    	EXdelete();
	return NULL;
    }
    lfaddr = (PTR) lf;
/* # line 1198 "listpck.qsc" */	/* addform */
    {
      IIaddform(lfaddr);
    }
/* # line 1200 "listpck.qsc" */	/* inittable */
    {
      if (IItbinit(name,name,(char *)"u") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 1202 "listpck.qsc" */	/* host code */
    /*
    ** all "legal" load tablefield syntax expects you to somehow
    ** package all the columns into one statement.  We cheat
    ** by calling the libq routines which let us stuff the tablefield
    ** a column at a time.  This is about what ## loadtable generates.
    */
    for ( i = rwcnt ; --i >= 0 ; )
    {
	IItbact(name,name,1);
	for (j=0; j < numcol; ++j)
	{
	    IItcoputio(fds[j].name,(i2 *)NULL,(i4)TRUE,DB_CHR_TYPE,0,*coldata);
	    ++coldata;
	}
    }
    EXdelete();
    return lf;
  }
/*
** local routine to count allocation requirements
** We don't count trailing semicolons on a line, and realize that
** the final newline may be left off.
*/
static i4
num_newl(str,semimax)
char	*str;
i4	*semimax;
{
    i4  nlcount;
    i4  cursemi;
    i4  nl_last;
    /*
    ** semimax really counts 1 plus the maximum number of semicolons, ie.
    ** the number of "column" fields.  We init nl_last as 1 so that we
    ** wind up treating an empty string as "no newlines", rather than a
    ** line with no trailing newline.
    */
    nlcount = 0;
    *semimax = 1;
    cursemi = nl_last = 1;
    for ( ; *str != EOS; CMnext(str))
    {
	switch(*str)
	{
	case '\n':
	    if (cursemi > *semimax)
		*semimax = cursemi;
	    cursemi = nl_last = 1;
	    ++nlcount;
	    break;
	case ';':
	    if (str[1] != EOS)
		++cursemi;
	    /* fall through */
	default:
	    nl_last = 0;
	    break;
	}
    }
    /*
    ** no trailing newline?
    */
    if (! nl_last)
    {
	if (cursemi > *semimax)
	    *semimax = cursemi;
	++nlcount;
    }
    return (nlcount);
}
/*
** keystroke intercept routine to support search characters and
** choice via "return" key.
*/
static i4
key_catch(cmd,ktype,buf)
i4  cmd;
i4  ktype;
char *buf;
{
	i4 len;
	if (cmd == fdopMENU)
	{
		On_menu = TRUE;
		Om_count = 0;
		return FRS_PROCESS;
	}
	switch(ktype)
	{
	case CONTROL_KEY:
		if (On_menu && (*buf == '\r' || *buf == '\n'))
		{
			if (Om_count == 0)
				On_menu = FALSE;
			Om_count = 0;
			return FRS_PROCESS;
		}
		if (Catch_cr >= 0 && *buf == '\r')
		{
			Cr_select = TRUE;
			return FRS_RETURN;
		}
		return FRS_PROCESS;
	case NORM_KEY:
		if (On_menu)
		{
			++Om_count;
		}
		return FRS_PROCESS;
	default:
		if ((cmd == fdopSELECT) && (Catch_cr >= 0))
		{
			Cr_select = TRUE;
			return FRS_RETURN;
		}
		return FRS_PROCESS;
	}
}
