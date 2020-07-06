# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
# include	<me.h>		 
#include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
#include	<uf.h>
#include	<stdprmpt.h>
# include <abclass.h>
# include <metafrm.h>
#include	"erab.h"
/**
** Name:	defaults.qc -	ABF Application Defaults Frame.
**
** Description:
**	Contains the routine used to set the application defaults
**	information, such as the source directory, etc.  Defines:
**
**	IIABdefaults()	ABF application defaults frame.
**
** History:
**	Revision 6.2  88/12  wong
**	Renamed as 'IIABdefaults()' and moved here.
**
**	Revision 6.1  88/03  wong
**	Modified as 'IIABoptions()' adding new options and removed editor
**	option.  (But kept old version since these changes are not yet
**	"official.")
**
**	Revision 2.0  82/10/18  joe
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
/*{
** Name:	IIABdefaults() -	ABF Application Defaults Frame.
**
** Description:
**	Allows a user to set the default application information.  This
**	information is saved in the input application object.  If changes
**	were made, set the application object dirty bit.
**
** Input:
**	applid	{OOID}  Application object ID.
**	app	{APPL *}  Application object structure.
**
** Returns:
**	{STATUS}  OK
**
** Called by:
**	'IIOOdetail()' (from 'IIABcatalogs()',) 'IIABeditApp()'.
**
** Side Effects:
**	Changes the values of the application defaults.
**
** History:
**	Written 10/18/82 (jrc)
**	03/88 (jhw) -- Removed editor field; added source directory etc.
**	08/89 (jhw) -- Added activations on Start Frame and Start Procedure
**		to check for legal INGRES names.  JupBug #7330.
**	06/90 (mikes) -- Changed End menu-item to be OK since this is a pop-up.
**	06/90 (mikes) -- Check vailidity of source directory
**	11/90 (mikes) -- Ignore roles if they're not supported
**	2/91 (Mike S) -- Check for default menu style, if VISION.
**	23-jul-91 (blaise)
**		Only display ListChoices menuitem if Vision. Bug #37608.
**	6-aug-91 (blaise)
**		Menuitem lookupmenuitem, which is blank if we're not running
**		Vision, should be an empty string and not EOS.
**	04-mar-92 (leighb) DeskTop Porting Change:
**		Moved function declaration outside of calling function.
**		This is so function prototypes will work.
*/
GLOBALREF char	_iiOOempty[];
GLOBALREF bool	IIabKME;
GLOBALREF bool	IIabVision;
#define _HelpExpl	F_FE0100_ExplFrameHelp
#define _EndExpl	F_FE0102_ExplEnd
static READONLY
  char _DefForm[]= ERx("abfdefaults"), _StartFrame[]= ERx("startframe"), 
  _StartProc[]= ERx("startproc"), _AppRole[]= ERx("role"), _CompForm[]= ERx("compform")
  , _CompNewForm[]= ERx("newcompform"), _MenuStyle[]= ERx("menustyle");
static 
  char *yes, *no;
static bool _DefInit = FALSE;
static VOID change_frames();
static VOID change_styles();
FUNC_EXTERN VOID IIUFfieldHelp();
FUNC_EXTERN bool IIUGyn();
FUNC_EXTERN i4	IIFGmsmMenuStyleMask();
FUNC_EXTERN char 	*IIFGmsnMenuStyleName();
FUNC_EXTERN char	*FEtsalloc();		 
STATUS
IIABdefaults ( applid, app )
OOID		applid;
  register APPL *app;
{
  i2 change;
  char source[180+1];
  char linkopt[48+1];
  char image[48+1];
  char startframe[FE_MAXNAME+1];
  char startproc[FE_MAXNAME+1];
  char roleid[FE_MAXNAME+1];
  char defmenustyle[20];
  char compform[10];
  char compnewform[10];
  char *lookupmenuitem;
	bool	allcomp;	/* Compile forms for all frames */
	i4	defcomp;	/* Compile forms for new frames.  Only 
				   relevant if allcomp is FALSE. "nat" because
				   it must take an "I don't know" value */
	if ( !_DefInit )
	{
		_DefInit = TRUE;
		IIARiaForm(_DefForm);
		yes = ERget(F_UG0002_Yes2);
		no = ERget(F_UG0007_No2);	
	}
	/* Fetches the application object catalog entry for an application
	** object from the DBMS.  This entry is marked as unchanged by
	** clearing the application object 'dirty' bit.
	*/
	if ( app->ooid == OC_UNDEFINED || app->ooid != applid )
	{
		STATUS	status;
		app->ooid = applid;
		MEfill( sizeof(*app) - ((char *)&app->name - (char *)app), '\0',
			(PTR)&app->name
		);
		if ( (status = IIAMapFetch(app, applid, TRUE)) != OK )
			return status;
		_VOID_ OOhash(applid, (OO_OBJECT *)app);
	}
	/* The ListChoices menu item is only appropriate for Vision */
	lookupmenuitem = IIabVision ? ERget(FE_Lookup) : ERx("");
/* # line 168 "defaults.qsc" */	/* display */
  {
    if (IIdispfrm(_DefForm,(char *)"f") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    IIputfldio((char *)"srcdir",(short *)0,1,32,0,app->source);
    IIputfldio((char *)"linkopt",(short *)0,1,32,0,app->link_file);
    IIputfldio((char *)"image",(short *)0,1,32,0,app->executable);
    IIputfldio(_AppRole,(short *)0,1,32,0,app->roleid);
    {
      i4 _off;
/* # line 179 "defaults.qsc" */	/* host code */
		if ( !app->start_proc )
		{
			_off = 0;
/* # line 182 "defaults.qsc" */	/* putform */
      {
        if (IIfsetio((char *)0) != 0) {
          IIputfldio(_StartFrame,(short *)0,1,32,0,app->start_name);
        } /* IIfsetio */
      }
/* # line 183 "defaults.qsc" */	/* clear */
      {
        IIfldclear(_StartProc);
      }
/* # line 184 "defaults.qsc" */	/* host code */
		}
		else
      {
			_off = app->batch;
/* # line 188 "defaults.qsc" */	/* putform */
        {
          if (IIfsetio((char *)0) != 0) {
            IIputfldio(_StartProc,(short *)0,1,32,0,app->start_name);
          } /* IIfsetio */
        }
/* # line 189 "defaults.qsc" */	/* clear */
        {
          IIfldclear(_StartFrame);
        }
      }
/* # line 191 "defaults.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,_DefForm,(char *)0, (char *)0) != 0) {
          IIstfsio(54,_StartFrame,0,(short *)0,1,30,4,&_off);
        } /* IIiqset */
      }
/* # line 192 "defaults.qsc" */	/* set_frs */
      {
        if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIstfsio(21,(char *)0,0,(short *)0,1,32,0,(char *)"u");
        } /* IIiqset */
      }
/* # line 194 "defaults.qsc" */	/* host code */
		if (!IIabKME)
/* # line 195 "defaults.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,_DefForm,(char *)0, (char *)0) != 0) {
          IIstfsio(54,_AppRole,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        } /* IIiqset */
      }
/* # line 197 "defaults.qsc" */	/* host code */
		allcomp = (app->flags & APC_DBFORMS) == 0;
		defcomp = ((app->flags & APC_DEFDBFORMS) == 0) ? 1 : 0;
		if (allcomp)
		{
/* # line 201 "defaults.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,_DefForm,(char *)0, (char *)0) != 0) {
          IIstfsio(54,_CompNewForm,0,(short *)0,1,30,sizeof(1),
          (void *)IILQint(1));
        } /* IIiqset */
      }
/* # line 202 "defaults.qsc" */	/* putform */
      {
        if (IIfsetio(_DefForm) != 0) {
          IIputfldio(_CompForm,(short *)0,1,32,0,yes);
        } /* IIfsetio */
      }
/* # line 203 "defaults.qsc" */	/* host code */
		}
		else
		{
      char *defcompile;
			defcompile = (defcomp == 1) ? yes : no;
/* # line 209 "defaults.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,_DefForm,(char *)0, (char *)0) != 0) {
          IIstfsio(54,_CompNewForm,0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
        } /* IIiqset */
      }
/* # line 210 "defaults.qsc" */	/* putform */
      {
        if (IIfsetio(_DefForm) != 0) {
          IIputfldio(_CompForm,(short *)0,1,32,0,no);
          IIputfldio(_CompNewForm,(short *)0,1,32,0,defcompile);
        } /* IIfsetio */
      }
/* # line 212 "defaults.qsc" */	/* host code */
		}
		if (IIabVision)
		{
      char *style;
			style = IIFGmsnMenuStyleName(app->flags & APC_MS_MASK);
/* # line 219 "defaults.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,_DefForm,(char *)0, (char *)0) != 0) {
          IIstfsio(54,_MenuStyle,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0
          ));
        } /* IIiqset */
      }
/* # line 220 "defaults.qsc" */	/* putform */
      {
        if (IIfsetio(_DefForm) != 0) {
          IIputfldio(_MenuStyle,(short *)0,1,32,0,style);
        } /* IIfsetio */
      }
/* # line 221 "defaults.qsc" */	/* host code */
		}
		else
		{
/* # line 224 "defaults.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,_DefForm,(char *)0, (char *)0) != 0) {
          IIstfsio(54,_MenuStyle,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1
          ));
        } /* IIiqset */
      }
/* # line 225 "defaults.qsc" */	/* host code */
		}
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 230 "defaults.qsc" */	/* host code */
		bool new_answer;
/* # line 232 "defaults.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,9,compform,_CompForm);
            } /* IIfsetio */
          }
/* # line 233 "defaults.qsc" */	/* host code */
		new_answer = IIUGyn(compform, (STATUS *)NULL);
		if (new_answer && !allcomp)
		{
			/* We've just decided to compile all forms */
/* # line 237 "defaults.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,_DefForm,(char *)0, (char *)0) != 0) {
              IIstfsio(54,_CompNewForm,0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 238 "defaults.qsc" */	/* host code */
			allcomp = TRUE;
		}
		else if (!new_answer && allcomp)
		{
			/* We've just decided not to compile all forms */
/* # line 243 "defaults.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,_DefForm,(char *)0, (char *)0) != 0) {
              IIstfsio(54,_CompNewForm,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 244 "defaults.qsc" */	/* host code */
			allcomp = FALSE;
			/* Force him to answer the question about defaults */
			defcomp = -1;	/* I dont know */
/* # line 248 "defaults.qsc" */	/* clear */
          {
            IIfldclear(_CompNewForm);
          }
/* # line 249 "defaults.qsc" */	/* resume */
          {
            IIresfld(_CompNewForm);
            if (1) goto IIfdB1;
          }
/* # line 250 "defaults.qsc" */	/* host code */
		}
/* # line 251 "defaults.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 256 "defaults.qsc" */	/* host code */
		i4	new_default;
		i4	cc;
/* # line 259 "defaults.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,9,compnewform,_CompNewForm);
            } /* IIfsetio */
          }
/* # line 260 "defaults.qsc" */	/* host code */
		new_default = IIUGyn(compnewform, (STATUS *)NULL) ? 1 : 0;
		if (new_default != defcomp)
		{
			ER_MSGID yes_exp;
			ER_MSGID no_exp;
			ER_MSGID cc_title;
			if (new_default)
			{
				yes_exp = F_AB00D0_YesMakeComp;
				no_exp = F_AB00D1_LeaveAlone;
				cc_title = F_AB00D2_MakeComp;
			}
			else
			{
				yes_exp = F_AB00D3_YesMakeDB;
				no_exp = F_AB00D1_LeaveAlone;
				cc_title = F_AB00D4_MakeDB;
			}
			/* Ask whether to change all existing forms */
			cc = IIUFccConfirmChoice(
				CONF_GENERIC, ERx(""), ERx(""), 
				ERget(F_AB00D5_CompChangeBanner),
				ERx("abchcomp.hlp"), cc_title, yes_exp, no_exp,
				ERx(""), FALSE);
			if (cc == CONFCH_YES)
				change_frames(app, (bool)(new_default == 1));
			defcomp = new_default;
		}
/* # line 292 "defaults.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 297 "defaults.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,180+1-1,source,(char *)"srcdi\
r");
            } /* IIfsetio */
          }
/* # line 298 "defaults.qsc" */	/* host code */
		if (!IIABchkSrcDir(source, TRUE))
/* # line 299 "defaults.qsc" */	/* resume */
          {
            IIresfld((char *)"srcdir");
            if (1) goto IIfdB1;
          }
/* # line 300 "defaults.qsc" */	/* host code */
		else
/* # line 301 "defaults.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 4) {
        {
          i4 change;
/* # line 308 "defaults.qsc" */	/* inquire_frs */
          {
            if (IIiqset(2,0,_DefForm,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&change,20,_StartFrame,0);
            } /* IIiqset */
          }
/* # line 309 "defaults.qsc" */	/* host code */
		if ( change != 0 )
		{
/* # line 311 "defaults.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,startframe,_StartFrame);
              IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,startproc,_StartProc);
            } /* IIfsetio */
          }
/* # line 315 "defaults.qsc" */	/* host code */
			if ( *startframe != EOS )
			{
				if ( *startproc != EOS || app->batch )
				{ /* only one can be set */
					/*NOT REACHED*/
				}
				else if ( FEchkname(startframe) != OK )
				{
					i4	maxlen = FE_MAXNAME;
					IIUGerr( E_AB0131_BadFrame,
							UG_ERR_ERROR,
						2, startframe, (PTR)&maxlen
					);
/* # line 335 "defaults.qsc" */	/* resume */
          {
            IIresfld(_StartFrame);
            if (1) goto IIfdB1;
          }
/* # line 336 "defaults.qsc" */	/* host code */
				}
			}
		}
/* # line 339 "defaults.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 5) {
        {
          i4 change;
/* # line 346 "defaults.qsc" */	/* inquire_frs */
          {
            if (IIiqset(2,0,_DefForm,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&change,20,_StartProc,0);
            } /* IIiqset */
          }
/* # line 347 "defaults.qsc" */	/* host code */
		if ( change != 0 )
		{
/* # line 349 "defaults.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,startframe,_StartFrame);
              IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,startproc,_StartProc);
            } /* IIfsetio */
          }
/* # line 353 "defaults.qsc" */	/* host code */
			if ( *startproc != EOS )
			{
				if ( *startframe != EOS )
				{ /* only one can be set */
					/*NOT REACHED*/
				}
				else if ( FEchkname(startproc) != OK )
				{
					i4	maxlen = FE_MAXNAME;
					IIUGerr( E_AB0133_BadProcedure,
							UG_ERR_ERROR,
						2, startproc, (PTR)&maxlen
					);
/* # line 373 "defaults.qsc" */	/* resume */
          {
            IIresfld(_StartProc);
            if (1) goto IIfdB1;
          }
/* # line 374 "defaults.qsc" */	/* host code */
				}
			}
		}
/* # line 377 "defaults.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 383 "defaults.qsc" */	/* inquire_frs */
          {
            if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,2,&change,20,(char *)0,0);
            } /* IIiqset */
          }
/* # line 384 "defaults.qsc" */	/* host code */
		if ( change )
/* # line 385 "defaults.qsc" */	/* enddisplay */
          {
            if (1) goto IIfdF1;
          }
/* # line 386 "defaults.qsc" */	/* host code */
		else
/* # line 387 "defaults.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 7) {
        {
          char fld[FE_MAXNAME+1];
/* # line 395 "defaults.qsc" */	/* host code */
		FLD_VALUE	fv;
		fv._form = _DefForm;
		fv._field = fld;
		fv._column = NULL;
/* # line 401 "defaults.qsc" */	/* inquire_frs */
          {
            if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,fld,22,(char *)0,0);
            } /* IIiqset */
          }
/* # line 402 "defaults.qsc" */	/* host code */
		IIUFfldHelp(&fv);
        }
      } else if (IIretval() == 8) {
        {
/* # line 409 "defaults.qsc" */	/* host code */
		change = FALSE;
/* # line 410 "defaults.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 9) {
        {
          char title[40+1];
/* # line 419 "defaults.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,40+1-1,title,(char *)"title");
            } /* IIfsetio */
          }
/* # line 420 "defaults.qsc" */	/* host code */
		FEhelp(ERx("abdeflts.hlp"), title);
        }
      } else {
        if (1) goto IIfdE1;
      } /* IIretval */
    } /* IIrunform */
IIfdF1:
    if (IIchkfrm() == 0) goto IIfdB1;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    IIgetfldio((short *)0,1,32,180+1-1,source,(char *)"srcdir");
    IIgetfldio((short *)0,1,32,48+1-1,linkopt,(char *)"linkopt");
    IIgetfldio((short *)0,1,32,48+1-1,image,(char *)"image");
    IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,startframe,_StartFrame);
    IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,startproc,_StartProc);
    IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,roleid,_AppRole);
    IIgetfldio((short *)0,1,32,19,defmenustyle,_MenuStyle);
    goto IIfdE1;
IImuI1:
    if (IIFRafActFld(_CompForm,0,1) == 0) goto IIfdE1;
    if (IIFRafActFld(_CompNewForm,0,2) == 0) goto IIfdE1;
    if (IIFRafActFld((char *)"srcdir",0,3) == 0) goto IIfdE1;
    if (IIFRafActFld(_StartFrame,0,4) == 0) goto IIfdE1;
    if (IIFRafActFld(_StartProc,0,5) == 0) goto IIfdE1;
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_OK),(char *)0,2,2,6) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,6) == 0) goto IIfdE1;
    if (IInmuact(lookupmenuitem,ERget(FE_LookupExpl),0,0,7) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(10,(char *)0,0,0,7) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,0,8) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,0,0,8) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,0,9) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,0,9) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 433 "defaults.qsc" */	/* host code */
	if ( change )
	{
		bool was_allcomp = (app->flags & APC_DBFORMS) == 0;
		i4 was_defcomp = ((app->flags & APC_DEFDBFORMS) == 0) ? 1 : 0;
		/* See if flags changed */
		if (was_allcomp && !allcomp)
		{
			app->flags |= APC_DBFORMS;
			app->data.dirty = TRUE;
		}
		else if (!was_allcomp && allcomp)
		{
			app->flags  &= ~APC_DBFORMS;
			app->data.dirty = TRUE;
		}
		if (was_defcomp == 1 && defcomp == 0)
		{
			app->flags |= APC_DEFDBFORMS;
			app->data.dirty = TRUE;
		}
		else if (was_defcomp == 0 && defcomp == 1)
		{
			app->flags &= ~APC_DEFDBFORMS;
			app->data.dirty = TRUE;
		}
		/* If we're in Vision, check menu style */
		if (IIabVision)
		{
			i4 oldmask;
			i4 newmask;
			oldmask = app->flags & APC_MS_MASK;
			newmask = IIFGmsmMenuStyleMask(defmenustyle);
			if (oldmask != newmask)
			{
				i4 cc;
				app->flags &= ~APC_MS_MASK;
				app->flags |= newmask;
				app->data.dirty = TRUE;
				/* 
				** See whether to change existing menu frames
				** to the new default style.
				*/
				cc = IIUFccConfirmChoice(
				    CONF_GENERIC, ERx(""), ERx(""), 
				    ERget(F_AB00D9_StyleChangeBanner),
				    ERx("abchstyl.hlp"), F_AB00D8_ChangeStyle, 
				    F_AB00D6_YesChangeMenus, 
				    F_AB00D7_LeaveAlone, ERx(""), FALSE);
				if (cc == CONFCH_YES)
					change_styles(app, newmask);
			}
		}
		if ( !STequal(source, app->source) )
		{
			if ( *source == EOS )
				app->source = _iiOOempty;
			else
			{
				app->source = FEtsalloc(app->data.tag, source);
				if ( app->source == NULL )
					return FAIL;
			}
			app->data.dirty = TRUE;
		}
		if ( IIabKME && (!STequal(roleid, app->roleid)) )
		{
			if ( *roleid == EOS )
				app->roleid = _iiOOempty;
			else
			{
				/* Invalidate saved password */
				app->password = NULL;
				app->roleid = FEtsalloc(app->data.tag, roleid);
				if ( app->roleid == NULL )
					return FAIL;
			}
			app->data.dirty = TRUE;
		}
		if ( !STequal(linkopt, app->link_file) )
		{
			if ( *linkopt == EOS )
				app->link_file = _iiOOempty;
			else
			{
				app->link_file =
					FEtsalloc(app->data.tag, linkopt);
				if ( app->link_file == NULL )
					return FAIL;
			}
			app->data.dirty = TRUE;
		}
		if ( !STequal(image, app->executable) )
		{
			if ( *image == EOS )
				app->executable = _iiOOempty;
			else
			{
				app->executable =
					FEtsalloc(app->data.tag, image);
				if ( app->executable == NULL )
					return FAIL;
			}
			app->data.dirty = TRUE;
		}
		if ( *startframe != EOS )
		{
			/* assert:  ! app->batch */
			if ( !STequal(startframe, app->start_name) )
			{
				app->start_name =
					FEtsalloc(app->data.tag, startframe);
				if ( app->start_name == NULL )
					return FAIL;
				app->data.dirty = TRUE;
			}
			else if ( app->start_proc )
			{
				app->data.dirty = TRUE;
			}
			app->start_proc = FALSE;
		}
		else if ( *startproc != EOS )
		{
			if ( !STequal(startproc, app->start_name) )
			{
				app->start_name =
					FEtsalloc(app->data.tag, startproc);
				if ( app->start_name == NULL )
					return FAIL;
				app->data.dirty = TRUE;
			}
			else if ( !app->start_proc )
			{
				app->data.dirty = TRUE;
			}
			app->start_proc = TRUE;
		}
		else if ( *app->start_name != EOS )
		{ /* cleared both ... */
			app->start_name = _iiOOempty;
			app->data.dirty = TRUE;
			app->start_proc = app->batch;
		}
		if ( app->data.dirty )
		{
			/* Set parent form as changed.  This is for when this
			** frame is run from the catalog MoreInfo frame.  When
			** run from the Frames/Procedures catalog frame, setting
			** the form as changed will be ignored since the change
			** state is never tested.
			*/
/* # line 596 "defaults.qsc" */	/* set_frs */
  {
    if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 597 "defaults.qsc" */	/* host code */
		}
	}
	return OK;
}
/*
** change_frames
**
** Change all existing frames, either to use compiled forms, or to get forms 
** from the database.
*/
static VOID
change_frames(appl, compiled_forms)
APPL *appl;
bool compiled_forms;
{
	APPL_COMP *comp;
	/* Announce ourselves */
	IIUGmsg(ERget(S_AB02B0_ChangeCompForm), FALSE, 0);
	/* Load the whole application */
	if ( IIAMapFetch(appl, OC_UNDEFINED, TRUE) != OK )
			return;
	/* Go through all the user frames, changing the form type if need be */
	for (comp = (APPL_COMP *)appl->comps; comp != NULL; comp = comp->_next)
	{
		switch (comp->class)
		{
		    case OC_OSLFRAME:
		    case OC_OLDOSLFRAME:
		    case OC_MUFRAME:
		    case OC_APPFRAME:
		    case OC_UPDFRAME:
		    case OC_BRWFRAME:
			if (compiled_forms)
			{
				if ((comp->flags & APC_DBFORM) == 0)
					break;	/* Already correct */
				comp->flags &= ~APC_DBFORM;
			}
			else
			{
				if ((comp->flags & APC_DBFORM) != 0)
					break;	/* Already correct */
				comp->flags |= APC_DBFORM;
			}
			comp->data.dirty = TRUE;
			/* 
			** Save it.  We use IIAMufqUserFrameQuick instead of
			** iiabSvComp to avoid seeing the name of every frame 
			** as it's saved.  This doesn't update the alter date
			** for the frame, which is perhaps a pity.
			*/
			_VOID_ IIAMufqUserFrameQuick((OO_OBJECT *)comp);
			break;
		    default:
			break;
		}
	}
	return;
}
/*
** change_styles
**
** Change all existing menu frames to use the new default style
*/
static VOID
change_styles(appl, style_mask)
APPL *appl;
i4 style_mask;
{
	APPL_COMP *comp;
	/* Announce ourselves */
	IIUGmsg(ERget(S_AB02B1_ChangeMenuStyle), FALSE, 0);
	/* Load the whole application */
	if ( IIAMapFetch(appl, OC_UNDEFINED, TRUE) != OK )
			return;
	/* Go through all the menu frames, changing the menu style if need be */
	for (comp = (APPL_COMP *)appl->comps; comp != NULL; comp = comp->_next)
	{
		if (comp->class == OC_MUFRAME)
		{
			i4 oldmask = comp->flags & APC_MS_MASK;
			if (style_mask != oldmask)
			{
				comp->flags &= ~APC_MS_MASK;
				comp->flags |= style_mask;
				((USER_FRAME *)comp)->mf->state |= 
							MFST_DOGEN|MFST_DOFORM;
				comp->data.dirty = TRUE;
				/* 
				** See the note above about use of
				** IIAMufqUserFrameQuick.
				*/
				_VOID_ IIAMufqUserFrameQuick((OO_OBJECT *)comp);
			}
		}
	}
	return;
}
