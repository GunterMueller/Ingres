# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<me.h>
# include	<st.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ft.h>
# include	<fmt.h>
# include	<adf.h>
# include	<frame.h>
# include	<valid.h>
# include	<afe.h>
# include	<uf.h>
# include	<ug.h>
# include	<runtime.h>
# include	"helphlp.h"
# include	"eruf.h"
/**
** Name:	fehvals.qc	- routines to get help/values list.
**
** Description:
**	This file defines:
**
**	IIUFfldHelp()	display help or selection list for field.
**
** History:
**	04/25/87 (dkh) -Resurrected to work with new validation structs.
**	13-jul-87 (bruceb) Changed memory allocation to use [FM]Ereqmem.
**	08/14/87 (dkh) - ER changes.
**	10/1/87 (daveb) -- remove erronerous declarations of CL
**		routines, conflicting with correct decls. in CLentry.h
**	10/05/87 (dkh) - Help file changes.
**	10/22/87 (dkh) - Fixed jup bug 1252 and 1270.
**	07-apr-88 (bruceb)
**		Changed from using sizeof(DB_TEXT_STRING)-1 to using
**		DB_CNTSIZE.  Previous calculation is in error.
**		Replaced an improper tab with spaces--ERx("     ");
**	5/88 (bobm) changes to use popup form.
**	03/22/89 (dkh) - Fixed venus bug 4776.
**	07/22/89 (dkh) - Fixed bug 6803.
**	08/01/89 (dkh) - Fixed bug 7290.
**	27-nov-89 (bruceb)
**		Find/top/bottom no longer visible menuitems.
**	16-jan-90 (bruceb)
**		Improved scroll code.
**	31-jan-90 (bruceb)
**		Cancel menuitem mapped only to frskey9 (not frskey3) as per FRC.
**	90/05  wong  Renamed from 'FEhvals()' with FLD_VALUE parameter.
**	03/28/91 (dkh) - Replaced old inquire_frs syntax with new.
**	09/18/92 (dkh) - Fixed bug 44454.  Listchoices will now correctly
**			 display field values for field's with multi-line
**			 display formats.
**	12/05/92 (dkh) - Added routine to adjust size of a table field.
**	07/16/93 (dkh) - Fixed bug 40693.  Added code so that the return
**			 key will select can be used to select the item
**			 that is highlighted (per FRC decision).
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**	15-jan-1996 (toumi01; from 1.1 axp_osf port)
**		Added kchin's changes (from 6.4) for axp_osf
**		01-feb-93 (kchin)
**		Change type of i4val from i4 to PTR in disp_form().  As i4val
**		is holding pointer value and not i4 any more, rename it as
**		ptrval.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	07-mar-2001 (somsa01)
**	    Changed maxcharwidth from 10000 to FE_FMTMAXCHARWIDTH.
*/
# define	TABLEWIDTH	78	/* Size of help table field */
# define	REPLYWIDTH	132	/* Max Size of 'word' for find */
FUNC_EXTERN	FRAME	*IIfindfrm();
FUNC_EXTERN	FIELD	*FDfndfld();
FUNC_EXTERN	FLDCOL	*FDfndcol();
FUNC_EXTERN	ADF_CB	*FEadfcb();
FUNC_EXTERN	i4	IIAFfpcFindPatternChars();
FUNC_EXTERN	void	IITBatsAdjustTfSize();
FUNC_EXTERN	i4	(*IIFRkeystroke())();
/* EQUEL Declaration:
  typedef struct {
    char *_form;
    char *_field;
    char *_column;
  } FLD_VALUE;
*/
static VOID	disp_message();
static i4	disp_form();
static bool	find_valid();
static STATUS	FEfillst();
static ADF_CB	*Adfcb = NULL;
static	bool	cursor_on_menu = FALSE;
static	bool	selected_with_return = FALSE;
static	i4	menu_input_count = 0;
static READONLY
  char _HlpForm[]= ERx("helpvmax"), _HlpTblFld[]= ERx("helptable");
/*{
** Name:	IIUFfldHelp() -	Display Help or Selection List for Field.
**
** Description:
**	Display the validation string, or list of values, for a field or column
**	in a form.  This will list the validation string, or will give a set of
**	values if the validation is a list (either <field in table.column> or
**	<field in [list]>); the user can then browse and select from the list
**	of values.
**
** Inputs:
**	fldv	{FLD_VALUE *}  Descriptor for field or column.
**
** History:
**	24-sep-1985 (peter)	Written.
**	30-jan-1986 (peter)	Removed redundant code after code review.
**	04-mar-1986 (dkh)	Fix for BUG 8430.
**	07-jan-1987 (peter)	Change RTfnd* to FDfnd*.
**	5/88 (bobm)		Redone with popup for 6.1
**	05/90 (jhw)  Renamed from 'FEhvals()' with FLD_VALUE parameter.
*/
i4	IIUFfldHelp();
VOID
FEhvals(frmnam, fldnam, colnam)
char *frmnam;
char *fldnam;
char *colnam;
{
	FLD_VALUE	fv;
	fv._form = frmnam;
	fv._field = fldnam;
	fv._column = colnam;
	_VOID_ IIUFfldHelp(&fv);
}
i4
IIUFfldHelp ( fldv )
FLD_VALUE	*fldv;
{
	bool	istable;
	char	*vstr;			/* validation string */
	char	*ffmt;			/* format string */
	bool	nullable;		/* TRUE if nullable */
	char	ftype[FE_PROMPTSIZE];	/* type description */
	if ( Adfcb == NULL )
	{
		if ( IIUFgtfGetForm(IIUFlcfLocateForm(), _HlpForm) != OK )
		{
			IIUGerr( E_UF0026_BadForm, UG_ERR_FATAL, 1, _HlpForm );
		}
		Adfcb = FEadfcb();
	}
	/* Fill in the validation information */
	if ( find_valid(fldv, &vstr, &ffmt, &nullable, ftype, &istable) )
		return disp_form(istable, fldv);
	else
	{
		disp_message(istable, fldv, vstr, ffmt, nullable, ftype);
		return 0;
	}
	/*NOT REACHED*/
}
/*
** Name:	disp_message - display validation string as message
**
** Description:
**	Validation string is not a list type.  Display validation as a popup
**	message.
**
** Inputs:
**	istab	TRUE if tablefield
**	fldv	{FLD_VALUE *}  Field/column value descriptor.
**	fname	name of field.
**	cname	name of column.
**	vstr	validation string.
**	fstr	display format string.
**	nullable,ftype	type.
**
** History:
**	5/88 (bobm)	Written.
*/
static VOID
disp_message(istab, fldv, vstr, fstr, nullable, ftype)
bool istab;
FLD_VALUE	*fldv;
char *vstr;
char *fstr;
bool nullable;
char *ftype;
  {
	i4		narg;
	ER_MSGID	mid;
	char		*name;
	char		*title;
    char msg[512];
	/*
	** get appropriate message.  We have many individual messages to avoid
	** problems in language translation.
	*/
	if (istab)
	{
		name = fldv->_column;
		title = ERget(F_UF0008_NoColChoices);
		if ( vstr == NULL || *vstr == EOS )
		{
			narg = 4;
			mid = nullable ? S_UF0049_cnoval_null : S_UF004A_cnoval;
			vstr = title;
		}
		else
		{
			narg = 5;
			mid = nullable ? S_UF004B_chasval_null : S_UF004C_chasval;
		}
	}
	else
	{
		name = fldv->_field;
		title = ERget(F_UF0009_NoFldChoices);
		if ( vstr == NULL || *vstr == EOS )
		{
			narg = 4;
			mid = nullable ? S_UF004D_fnoval_null : S_UF004E_fnoval;
			vstr = title;
		}
		else
		{
			narg = 5;
			mid = nullable ? S_UF004F_fhasval_null : S_UF0050_fhasval;
		}
	}
	IIUGfmt( msg, sizeof(msg), ERget(mid),
			narg, name, ftype, fstr, vstr, title
	);
/* # line 263 "fehvals.qsc" */	/* message */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      IImessage(msg);
    }
  }
/* # line 267 "fehvals.qsc" */	/* host code */
static i4
intercept_keystroke(command, keytype, buf)
i4	command;
i4	keytype;
char	*buf;
{
	i4	length;
	if (command == fdopMENU)
	{
		cursor_on_menu = TRUE;
		menu_input_count = 0;
		return(FRS_PROCESS);
	}
	switch(keytype)
	{
		case NORM_KEY:
			if (cursor_on_menu)
			{
				menu_input_count++;
			}
			return(FRS_PROCESS);
			break;
		case CONTROL_KEY:
			if (cursor_on_menu && (*buf == '\n' || *buf == '\r'))
			{
				if (menu_input_count == 0)
				{
					cursor_on_menu = FALSE;
				}
				menu_input_count = 0;
				return(FRS_PROCESS);
			}
			if (*buf == '\r')
			{
				selected_with_return = TRUE;
				return(FRS_RETURN);
			}
			return(FRS_PROCESS);
			break;
		default:
			if (command == fdopSELECT)
			{
				selected_with_return = TRUE;
				return(FRS_RETURN);
			}
			return(FRS_PROCESS);
			break;
	}
}
/*
** Name:	disp_form - display popup selection form
**
** Description:
**	Display popup form for list type validation.  find_valid() filled in
**	the form.
**
** Inputs:
**	istable	TRUE if tablefield.
**	fldv	{FLD_VALUE *}  Field/column value descriptor.
**			._form		{char *}  form name.
**			._field		{char *}  field name.
**			._column	{char *}  column name.
** Returns:
**	{nat}  Selected dataset record number, or zero if none.
**
** History:
**	5/88 (bobm)	Extracted from FEhval().
*/
#define _SelectExpl	F_FE0108_ExplSelect
#define _HelpExpl	F_FE0100_ExplFrameHelp
#define _CancelExpl	F_FE0103_ExplCancel
static i4
disp_form( istable, fldv )
bool		istable;
  FLD_VALUE *fldv;
  {
	bool	fld_help;
    i4 return_row;
    i4 numrecs, numrows;
    i4 currrec;
    char *nextmenu;
    char *prevmenu;
    char *topmenu;
    char *botmenu;
    char upmap[80];
    char downmap[80];
    char menu2map[80];
    char menu3map[80];
    char uplabel[80];
    char downlabel[80];
    char menu2label[80];
    char menu3label[80];
	i4	(*keystroke_func)();
	/* Table is loaded with list of values.	 Add
	** Top and Bottom Menuitems if enough rows
	*/
/* # line 375 "fehvals.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,_HlpForm,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&numrecs,32,_HlpTblFld,0);
        IIiqfsio((short *)0,1,30,4,&numrows,30,_HlpTblFld,0);
      } /* IIiqset */
    }
/* # line 377 "fehvals.qsc" */	/* host code */
	if ( numrecs > numrows )
	{ /* More rows than can be displayed */
		fld_help = (bool)( fldv->_form != NULL && *fldv->_form != EOS
					&& fldv->_field != NULL
					&& *fldv->_field != EOS
			);
		nextmenu = ERget(FE_NextPage);
		prevmenu = ERget(FE_PrevPage);
		topmenu = ERget(FE_Top);
		botmenu = ERget(FE_Bottom);
/* # line 388 "fehvals.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,79,upmap,8,(char *)0,2017);
        IIiqfsio((short *)0,1,32,79,downmap,8,(char *)0,2018);
        IIiqfsio((short *)0,1,32,79,menu2map,8,(char *)0,3001);
        IIiqfsio((short *)0,1,32,79,menu3map,8,(char *)0,3002);
        IIiqfsio((short *)0,1,32,79,uplabel,9,(char *)0,2017);
        IIiqfsio((short *)0,1,32,79,downlabel,9,(char *)0,2018);
        IIiqfsio((short *)0,1,32,79,menu2label,9,(char *)0,3001);
        IIiqfsio((short *)0,1,32,79,menu3label,9,(char *)0,3002);
      } /* IIiqset */
    }
/* # line 397 "fehvals.qsc" */	/* host code */
		if (upmap[0] != EOS)
		{
/* # line 399 "fehvals.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3001,(short *)0,1,32,0,upmap);
        IIstfsio(9,(char *)0,3001,(short *)0,1,32,0,uplabel);
      } /* IIiqset */
    }
/* # line 401 "fehvals.qsc" */	/* host code */
		}
		if (downmap[0] != EOS)
		{
/* # line 404 "fehvals.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3002,(short *)0,1,32,0,downmap);
        IIstfsio(9,(char *)0,3002,(short *)0,1,32,0,downlabel);
      } /* IIiqset */
    }
/* # line 406 "fehvals.qsc" */	/* host code */
		}
	}
	else
	{
		fld_help = FALSE;
		nextmenu = prevmenu = topmenu = botmenu = NULL;
		upmap[0] = downmap[0] = uplabel[0] = downlabel[0] = EOS;
		menu2map[0] = menu3map[0] = menu2label[0] = menu3label[0] = EOS;
	}
	cursor_on_menu = FALSE;
	selected_with_return = FALSE;
	menu_input_count = 0;
	keystroke_func = IIFRkeystroke(intercept_keystroke);
/* # line 423 "fehvals.qsc" */	/* display */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpsetio(5,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      IIFRgpcontrol(2,0);
      if (IIdispfrm(_HlpForm,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 426 "fehvals.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_HlpForm,_HlpTblFld,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&currrec,(char *)"_RECORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 427 "fehvals.qsc" */	/* host code */
		if (currrec == 1)
		{
/* # line 429 "fehvals.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_HlpForm,_HlpTblFld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 430 "fehvals.qsc" */	/* host code */
		}
		else
		{
/* # line 433 "fehvals.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_HlpForm,_HlpTblFld,-3) != 0) {
                IItbsmode((char *)"down");
                if (IItscroll(0,-3) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 434 "fehvals.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 2) {
          {
/* # line 438 "fehvals.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_HlpForm,_HlpTblFld,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&currrec,(char *)"_RECORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 439 "fehvals.qsc" */	/* host code */
		if (currrec == numrecs)
		{
/* # line 441 "fehvals.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_HlpForm,_HlpTblFld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 442 "fehvals.qsc" */	/* host code */
		}
		else
		{
/* # line 445 "fehvals.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_HlpForm,_HlpTblFld,-3) != 0) {
                IItbsmode((char *)"up");
                if (IItscroll(0,-3) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 446 "fehvals.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 3) {
          {
            PTR ptrval;
            DB_DATA_VALUE *dbvptr;
/* # line 454 "fehvals.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_HlpForm,_HlpTblFld,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&ptrval,(char *)"ptrfld");
                IItcogetio((short *)0,1,30,4,&return_row,(char *)"_REC\
ORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 455 "fehvals.qsc" */	/* host code */
		dbvptr = (DB_DATA_VALUE *) ptrval;
		if (istable)
		{
/* # line 458 "fehvals.qsc" */	/* putrow */
            {
              if (IItbsetio(3,fldv->_form,fldv->_field,-2) != 0) {
                IItcoputio(fldv->_column,(short *)0,1,44,0,dbvptr);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 461 "fehvals.qsc" */	/* set_frs */
            {
              if (IIiqset(5,0,fldv->_form,fldv->_field,(char *)0) != 0) {
                IIstfsio(20,fldv->_column,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 464 "fehvals.qsc" */	/* host code */
		}
		else
		{
/* # line 467 "fehvals.qsc" */	/* putform */
            {
              if (IIfsetio(fldv->_form) != 0) {
                IIputfldio(fldv->_field,(short *)0,1,44,0,dbvptr);
              } /* IIfsetio */
            }
/* # line 468 "fehvals.qsc" */	/* set_frs */
            {
              if (IIiqset(2,0,fldv->_form,(char *)0, (char *)0) != 0) {
                IIstfsio(20,fldv->_field,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 469 "fehvals.qsc" */	/* host code */
		}
/* # line 470 "fehvals.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,fldv->_form,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 471 "fehvals.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_HlpForm,_HlpTblFld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 472 "fehvals.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 476 "fehvals.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_HlpForm,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&numrecs,32,_HlpTblFld,0);
                IIiqfsio((short *)0,1,30,4,&numrows,30,_HlpTblFld,0);
              } /* IIiqset */
            }
/* # line 478 "fehvals.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_HlpForm,_HlpTblFld,1) != 0) {
                IItcogetio((short *)0,1,30,4,&currrec,(char *)"_RECORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 479 "fehvals.qsc" */	/* host code */
		currrec = min(currrec + 2 * (numrows - 1), numrecs);
/* # line 480 "fehvals.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_HlpForm,_HlpTblFld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,currrec) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 484 "fehvals.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_HlpForm,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&numrows,30,_HlpTblFld,0);
              } /* IIiqset */
            }
/* # line 485 "fehvals.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_HlpForm,_HlpTblFld,1) != 0) {
                IItcogetio((short *)0,1,30,4,&currrec,(char *)"_RECORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 486 "fehvals.qsc" */	/* host code */
		currrec = max(1, currrec - (numrows - 1));
/* # line 487 "fehvals.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_HlpForm,_HlpTblFld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,currrec) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 491 "fehvals.qsc" */	/* host code */
		/* Implement the Find command. */
		FEtabfnd(_HlpForm, _HlpTblFld);
          }
        } else if (IIretval() == 7) {
          {
/* # line 496 "fehvals.qsc" */	/* host code */
		/* Implement the Top command. */
/* # line 497 "fehvals.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_HlpForm,_HlpTblFld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 8) {
          {
/* # line 501 "fehvals.qsc" */	/* host code */
		/* Implement the Bottom command. */
/* # line 502 "fehvals.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_HlpForm,_HlpTblFld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 9) {
          {
/* # line 506 "fehvals.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_HlpForm,_HlpTblFld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 507 "fehvals.qsc" */	/* host code */
		return_row = 0;
/* # line 508 "fehvals.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 10) {
          {
/* # line 512 "fehvals.qsc" */	/* host code */
		if ( fld_help )
		{
			if (upmap[0] != EOS)
			{
/* # line 516 "fehvals.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(8,(char *)0,2017,(short *)0,1,32,0,upmap);
                IIstfsio(9,(char *)0,2017,(short *)0,1,32,0,uplabel);
              } /* IIiqset */
            }
/* # line 518 "fehvals.qsc" */	/* host code */
			}
			if (downmap[0] != EOS)
			{
/* # line 521 "fehvals.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(8,(char *)0,2018,(short *)0,1,32,0,downmap);
                IIstfsio(9,(char *)0,2018,(short *)0,1,32,0,downlabel);
              } /* IIiqset */
            }
/* # line 523 "fehvals.qsc" */	/* host code */
			}
			if (menu2map[0] != EOS)
			{
/* # line 526 "fehvals.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(8,(char *)0,3001,(short *)0,1,32,0,menu2map);
                IIstfsio(9,(char *)0,3001,(short *)0,1,32,0,menu2label);
              } /* IIiqset */
            }
/* # line 528 "fehvals.qsc" */	/* host code */
			}
			if (menu3map[0] != EOS)
			{
/* # line 531 "fehvals.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(8,(char *)0,3002,(short *)0,1,32,0,menu3map);
                IIstfsio(9,(char *)0,3002,(short *)0,1,32,0,menu3label);
              } /* IIiqset */
            }
/* # line 533 "fehvals.qsc" */	/* host code */
			}
		}
		FEhhelp(HLPVALS_HLP, ERget(S_UF003E_HELP_Field_Values));
		if ( fld_help )
		{
			if (upmap[0] != EOS)
			{
/* # line 542 "fehvals.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(8,(char *)0,3001,(short *)0,1,32,0,upmap);
                IIstfsio(9,(char *)0,3001,(short *)0,1,32,0,uplabel);
              } /* IIiqset */
            }
/* # line 544 "fehvals.qsc" */	/* host code */
			}
			if (downmap[0] != EOS)
			{
/* # line 547 "fehvals.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(8,(char *)0,3002,(short *)0,1,32,0,downmap);
                IIstfsio(9,(char *)0,3002,(short *)0,1,32,0,downlabel);
              } /* IIiqset */
            }
/* # line 549 "fehvals.qsc" */	/* host code */
			}
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
      if (IIactscrl(_HlpTblFld,1,1) == 0) goto IIfdE1;
      if (IIactscrl(_HlpTblFld,0,2) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Select),ERget(_SelectExpl),2,2,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(0,(char *)0,2,2,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,3) == 0) goto IIfdE1;
      if (IInmuact(nextmenu,(char *)0,2,2,4) == 0) goto IIfdE1;
      if (IInmuact(prevmenu,(char *)0,2,2,5) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,8) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Cancel),ERget(_CancelExpl),2,2,9) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,2,2,9) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),2,2,10) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,10) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 553 "fehvals.qsc" */	/* host code */
	if ( numrecs > numrows )
	{
		if (upmap[0] != EOS)
		{
/* # line 557 "fehvals.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,2017,(short *)0,1,32,0,upmap);
        IIstfsio(9,(char *)0,2017,(short *)0,1,32,0,uplabel);
      } /* IIiqset */
    }
/* # line 559 "fehvals.qsc" */	/* host code */
		}
		if (downmap[0] != EOS)
		{
/* # line 562 "fehvals.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,2018,(short *)0,1,32,0,downmap);
        IIstfsio(9,(char *)0,2018,(short *)0,1,32,0,downlabel);
      } /* IIiqset */
    }
/* # line 564 "fehvals.qsc" */	/* host code */
		}
		if (menu2map[0] != EOS)
		{
/* # line 567 "fehvals.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3001,(short *)0,1,32,0,menu2map);
        IIstfsio(9,(char *)0,3001,(short *)0,1,32,0,menu2label);
      } /* IIiqset */
    }
/* # line 569 "fehvals.qsc" */	/* host code */
		}
		if (menu3map[0] != EOS)
		{
/* # line 572 "fehvals.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3002,(short *)0,1,32,0,menu3map);
        IIstfsio(9,(char *)0,3002,(short *)0,1,32,0,menu3label);
      } /* IIiqset */
    }
/* # line 574 "fehvals.qsc" */	/* host code */
		}
	}
/* # line 577 "fehvals.qsc" */	/* inittable */
    {
      if (IItbinit(_HlpForm,_HlpTblFld,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 579 "fehvals.qsc" */	/* host code */
	(void) IIFRkeystroke(keystroke_func);
	return return_row;
  }
/*{
** Name:	find_valid	- find validation information.
**
** Description:
**	Fill in the validation string for the named column and
**	form.  Called by FEhvals to actually look things up in
**	the frame structurs.
**
** Inputs:
**	fldv	{FLD_VALUE *}  Field/column value descriptor.
**
** Returns:
**	{bool}	TRUE if table field filled with list. FALSE otherwise.
**
** Side Effects:
**
** History:
**	5/88 (bobm)	ripped out the guts of the old FEhvalid routine.
**	07-mar-2001 (somsa01)
**	    Changed maxcharwidth from 10000 to FE_FMTMAXCHARWIDTH.
*/
static bool
find_valid ( fldv, vstr, fstr, nullable, tdesc, istab )
FLD_VALUE	*fldv;
char	**vstr;
char	**fstr;
bool	*nullable;
char	*tdesc;
bool	*istab;
{
	DB_USER_TYPE	usertype;
	DB_DATA_VALUE	dbv;
	REGFLD	*rfld;		/* Temp point to regular field */
	FLDTYPE *fld;		/* Field Header for field or column */
	TBLFLD	*tfld;		/* Temp pointer to table field */
	bool	isnonseq;	/* TRUE if field is nonsequenced */
	FRAME	*frm;		/* FRAME Structure */
	char	*nptr;
	/* Get the right structures */
	frm = IIfindfrm(fldv->_form);
	if ( fldv->_column != NULL && *fldv->_column != EOS )
	{ /* Table field */
		*istab = TRUE;
		nptr = fldv->_column;
		tfld = (FDfndfld(frm, fldv->_field, &isnonseq))->fld_var.fltblfld;
		fld = &((FDfndcol(tfld, fldv->_column))->fltype);
	}
	else
	{ /* Regular field */
		*istab = FALSE;
		nptr = fldv->_field;
		rfld = (FDfndfld(frm, fldv->_field, &isnonseq))->fld_var.flregfld;
		fld = &(rfld->fltype);
	}
	*vstr = fld->ftvalstr;
	*fstr = fld->ftfmtstr;
	dbv.db_datatype = fld->ftdatatype;
	dbv.db_length = fld->ftlength;
	dbv.db_prec = fld->ftprec;
	if (afe_tyoutput(Adfcb, &dbv, &usertype) != OK)
	{
		STcopy(ERx("Unknown"),tdesc);
		*nullable = FALSE;
	}
	else
	{
		STcopy(usertype.dbut_name,tdesc);
		*nullable = (bool)( usertype.dbut_kind == DB_UT_NULL );
	}
	/* If there is no validation string, or the validation is not a list,
	** or 'FEfillst()' fails, return FALSE so normal informational field
	** help is displayed.
	*/
	return (bool) ( *vstr != NULL && **vstr != EOS
				&& fld->ftvalchk->v_nodetype == vLIST
					&& FEfillst(fld,nptr) == OK );
}
/*
** Name:	FEfillst	- fill in the help with list of values.
**
** Description:
**	Fill in help table field with a list of values from the
**	validation list.  This is given only when there is a
**	single list of valid values given.
**
**	This code is a bastardized version of chklist.c.
**
**	Note that this does a reinitialization of the tablefield.
**
** Inputs:
**	fld	FLDTYPE structure for field.
**
** Outputs:
**	Returns:
**		Status of OK or FAIL.
**
** Side Effects:
**
** History:
**	24-sep-1985 (peter)	Written.
**      24-nov-1993 (smc)
**		Bug #58882
**		Made ptrval a portable PTR.
*/
static STATUS
FEfillst(fld,nptr)
FLDTYPE *fld;
char *nptr;
  {
	static FMT	*lfmt = NULL;
	VTREE	*vlist;			/* validation tree for field */
	VALROOT *list;			/* root pointer of list */
	register VALLIST *node;		/* pointer to current element in list */
	FMT	*fmt;			/* output format for field */
	DB_DATA_VALUE	ltextdbv;
	DB_TEXT_STRING	*text;
	AFE_DCL_TXT_MACRO(TABLEWIDTH)	ltext;
	DB_DATA_VALUE	fltdbv;
	char	fmtstr[60];
	i4	fmtsize;
	i4	len;
	i4	x = 0;
	i4	y = 0;
	FMT	*dfmt = NULL;
	FRAME	*helpf;
	TBLFLD	*tfp;
	/* Set format structure information */
	if ( lfmt == NULL )
	{
		PTR	blk;
		_VOID_ STprintf(fmtstr, ERx("c%d"), TABLEWIDTH);
		if ( fmt_areasize(Adfcb, fmtstr, &fmtsize) != OK
			|| (blk = MEreqmem((u_i4)0, (u_i4)fmtsize,
						TRUE, (STATUS *)NULL)) == NULL
			|| fmt_setfmt(Adfcb, fmtstr, blk, &lfmt, &len) != OK )
		{ /* Could not set format structure information. */
			lfmt = NULL;
			return FAIL;
		}
	}
	/* Set up temp pointers */
	vlist = fld->ftvalchk;
	list = vlist->v_right.v_lstright;
	fmt = fld->ftfmt;
	if (fmt_size(Adfcb, fmt, NULL, &y, &x) != OK)
	{
		return(FAIL);
	}
	/*
	** modify width of form and tablefield.
	**
	** CAUTION:
	**
	** This reflects knowledge that the form consists of one tablefield,
	** which has one column.  Hence we know what is on the head of various
	** arrays, and what has to be changed.
	**
	** Method:
	**	->frmaxx = width of form (data width + 2)
	**	frfld is the array of fields, hence will be the tablefield.
	**	The tablefield header fhmaxx item is width of the tablefield
	**	( = form).  The tfflds item points to the array of columns,
	**	and since there is only one, we just set it's header fhmaxx
	**	item to the data width, and its datalength and ftwidth to
	**	the proper values.
	*/
	len = min(fld->ftwidth, TABLEWIDTH);
	helpf = IIfindfrm( _HlpForm );
	helpf->frmaxx = len+2;
	tfp = (*(helpf->frfld))->fld_var.fltblfld;
	tfp->tfhdr.fhmaxx = helpf->frmaxx;
	(*(tfp->tfflds))->flhdr.fhmaxx = len;
	(*(tfp->tfflds))->fltype.ftdataln = len;
	(*(tfp->tfflds))->fltype.ftwidth = len;
	/*
	** find needed length for tablefield.
	*/
	len = 0;
	for (node = list->listroot; len < 8 && node != NULL;
						node = node->listnext)
		++len;
	if (len == 0)
		return FAIL;
	/*
	** modify tablefield length.  Same cautions apply.
	*/
	helpf->frmaxy = len+2;
	tfp->tfhdr.fhmaxy = helpf->frmaxy;
	tfp->tfrows = len;
	/* Set up long text DB_DATA_VALUE. */
	text = (DB_TEXT_STRING *)&ltext;
	ltextdbv.db_datatype = DB_LTXT_TYPE;
	ltextdbv.db_prec = 0;
	ltextdbv.db_data = (PTR)text;
	ltextdbv.db_length = sizeof(ltext);
	fltdbv.db_datatype = DB_FLT_TYPE;
	fltdbv.db_length = sizeof(f8);
	fltdbv.db_prec = 0;
	/*
	**  Set up hidden column to hold the pointer to the
	**  DB_DATA_VALUE displayed on that row.
	*/
/* # line 803 "fehvals.qsc" */	/* inittable */
    {
      if (IItbinit(_HlpForm,_HlpTblFld,(char *)"r") != 0) {
        IIthidecol((char *)"ptrfld",(char *)"i4");
        IItfill();
      } /* IItbinit */
    }
/* # line 805 "fehvals.qsc" */	/* host code */
	for (node = list->listroot; node != NULL; node = node->listnext)
	{
		bool		fdcompat;
		i4		dtype;
		PTR		dblk;
		FMT		*fmttouse;
    PTR ptrval;
    DB_DATA_VALUE *dbvptr;
    char line[TABLEWIDTH+1];
		/*
		**  Just format into a single line long text value
		**  of width TABLEWIDTH.
		*/
		dbvptr = &(node->vl_dbv);
		/*
		**  If datatype is a string and there are pattern
		**  match characters, then return FAIL
		*/
		IIAFddcDetermineDatatypeClass(dbvptr->db_datatype, &dtype);
		if (dtype == CHAR_DTYPE)
		{
			if (IIAFfpcFindPatternChars(dbvptr))
			{
				return FAIL;
			}
		}
		if (fmt_isvalid(Adfcb, fmt, dbvptr, &fdcompat) != OK)
		{
			return FAIL;
		}
		/*
		**  Use the field's format structure only if its compatible
		**  AND that the format is a single line format.
		*/
		if (fdcompat && y == 1)
		{
			fmttouse = fmt;
		}
		else
		{
			bool	numeric;
			if ( afe_cancoerce(Adfcb, dbvptr, &fltdbv, &numeric)
					!= OK )
			{
				return FAIL;
			}
			/*
			**  If field is numeric, then create default
			**  format to use, else use the character format
			**  set up earlier.
			*/
			if (numeric)
			{
				if ( fmt_deffmt(Adfcb, dbvptr,
						FE_FMTMAXCHARWIDTH,
						(bool) FALSE, fmtstr) != OK )
				{
					return FAIL;
				}
				dblk = NULL;
				if ( fmt_areasize(Adfcb, fmtstr, &fmtsize) == OK
					&&
				    (dblk = MEreqmem((u_i4)0, (u_i4)fmtsize,
					TRUE, (STATUS *)NULL)) != NULL &&
				    fmt_setfmt(Adfcb, fmtstr, dblk, &dfmt, &len)
					== OK )
				{
					fmttouse = dfmt;
				}
				else
				{
					if ( dblk != NULL )
						MEfree(dblk);
					return FAIL;
				}
			}
			else
			{
				fmttouse = lfmt;
			}
		}
		/*
		**  Format value into a long text.
		*/
		if (fmt_format(Adfcb, fmttouse, dbvptr, &ltextdbv, TRUE) != OK)
		{ /* Formatting error. */
			if (dfmt != NULL)
			{
				MEfree(dblk);
			}
			return FAIL;
		}
		STlcopy(text->db_t_text, line, text->db_t_count);
		if (dfmt != NULL)
		{
			MEfree(dblk);
			dfmt = NULL;
		}
		/*
		**   Load information into table field.
		*/
		ptrval = (PTR) dbvptr;
/* # line 914 "fehvals.qsc" */	/* loadtable */
    {
      if (IItbact(_HlpForm,_HlpTblFld,1) != 0) {
        IItcoputio((char *)"text",(short *)0,1,32,0,line);
        IItcoputio((char *)"ptrfld",(short *)0,1,30,4,&ptrval);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 915 "fehvals.qsc" */	/* host code */
	}
	return OK;
  }
/*{
** Name:	IIUFatsAdjustTfSize - Adjust size of a table field.
**
** Description:
**	Adjust the size of the table field and the form that contains it
**	based on the passed in size information.  If the desired number
**	of rows is less than the maximum, then use that.  Otherwise,
**	use the maximum value.  This is obviously a hack since the
**	FRS does not support dynamic size adjustments formally.
**
** Inputs:
**	frm	Name of form to adjust.
**	tf	Name of table field to adjust.
**	maxrows	Max number of rows for the table field.
**	desired	The desired number of rows for the table field.
**
** Outputs:
**	TRUE	If everything worked.
**	FALSE	If the form or table field couldn't be found.
**
**	Returns:
**		None.
**	Exceptions:
**		None.
**
** Side Effects:
**
** History:
**	12/05/92 (dkh) - Initial version.
*/
bool
IIUFatsAdjustTfSize(frmname, tfname, maxrows, desired)
char	*frmname;
char	*tfname;
i4	maxrows;
i4	desired;
{
	FRAME	*frm;
	TBLFLD	*tf;
	FIELD	*fld;
	i4	newsize;
	bool	isnonseq;
	if ((frm = IIfindfrm(frmname)) == NULL)
	{
		return(FALSE);
	}
	if ((fld = FDfndfld(frm, tfname, &isnonseq)) == NULL)
	{
		return(FALSE);
	}
	tf = fld->fld_var.fltblfld;
	if (desired > maxrows)
	{
		newsize = maxrows;
	}
	else
	{
		newsize = desired;
	}
	/*
	**  Size of form is starting position of table field
	**  plus the number of rows plus one for the bottom
	**  border of the table field plus one for zero offset
	**  for the starting position of the table field.
	*/
	frm->frmaxy = tf->tfhdr.fhposy + newsize + 1 + 1;
	tf->tfhdr.fhmaxy = newsize + 2;
	tf->tfrows = newsize;
	IITBatsAdjustTfSize(frmname, tfname, newsize);
}
