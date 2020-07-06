# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<st.h>
#include	<er.h>
#include	<tm.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
#include	<adf.h>
#include	<afe.h>
# include <abclass.h>
# include <oocat.h>
#include        <dmchecks.h>
# include "abclinfo.h"
#include	"abfglobs.h"
#include	"gncode.h"
#include	"erab.h"
/**
** Name:	ab4pedit.qc -	ABF 4GL Procedure Component Edit Module.
**
** Description:
**	Contains the definition of the component class edit information
**	structure for the application 4GL procedure component objects.  Defines:
**
**	iiAB4GLProc	ABF 4GL procedure component class edit information.
**
** History:
**	Revision 6.2  89/02  wong
**	Initial revision.
**
**	02-dec-91 (leighb) DeskTop Porting Change:
**		Added routines to pass data across ABF/IAOM boundary.
** 6-Feb-92 (blaise)
**	Added include abfglobs.h.
**
** 16-Nov-92 (donc)
**	Modified COMP_CLASS_INFO initialilization to accomodate fix for bug 44162.
**
**	Revision 6.5
**
**	04-dec-92 (davel)
**		In _display(), make new _passdec field invisible.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
STATUS	iiabTstComp();
STATUS	iiabSrEdit();
STATUS	iiabSrCompile();
STATUS	iiabSfilePrint();
STATUS	iiabVerifyType();
static VOID	_display();
static STATUS	_save();
static VOID	_destroy();
static STATUS	_print();
/*:
** Name:	iiAB4GLProc -	ABF 4GL Procedure Component Class Information.
**
** Description:
**	Defines additional class information for the 4GL procedure component
**	objects for editing.
*/
#define _NewEditExpl	F_AB0052_NewEdit_expl
#define _GoExpl		F_AB0054_GoProc_expl
#define _EditExpl	F_AB0058_Edit_expl
#define _CompileExpl	F_AB005C_Compile_expl
#define _EmptyExpl	F_AB005E_Empty_expl
#define _PrintExpl	F_AB005F_Print_expl
GLOBALDEF READONLY COMP_CLASS_INFO iiAB4GLProc = {
	OC_OSLPROC,
	ERx("abfprocd"), ERx("ab4glpro.hlp"), ERx("ab4glpmi.hlp"), 2,
	F_AB0060_A4PDef_title,
	F_AB0044_A4PDet_title,
	-1,	NULL,	/* all types legal */
	{NULL,	NULL},	_return_type,	_srcfile,  NULL, NULL, 	/* Fields */
	/*{iiabNxtEdit,	AB_NewEdit,	_NewEditExpl,	0},	/* NextEdit */
	{iiabTstComp,	FE_Go,		_GoExpl,	4},    /* Go, FRSkey4 */
	{iiabSrEdit,	FE_Edit,	_EditExpl,	0},	/* Edit */
	{iiabSrCompile,	FE_Compile,	_CompileExpl,	0},	/* Compile */
	{NULL,		0,		_EmptyExpl,	0},	/* FormEdit */
	{_print,	FE_Print,	_PrintExpl,	0},	/* Print */
	_display, _save, _destroy, iiabVerifyType
};
static VOID
  _display( proc )
  register _4GLPROC *proc;
  {
    char *formname = iiAB4GLProc.form;
    i4 notnullable;
    i4 warning;
    char title[63+1];
	GLOBALREF char	*IIabExename;
	IIUGfmt(title,sizeof(title)-1,ERget(iiAB4GLProc.title), 1, IIabExename);
/* # line 112 "ab4pedit.qsc" */	/* putform */
    {
      if (IIfsetio(formname) != 0) {
        IIputfldio(_title,(short *)0,1,32,0,title);
        IIputfldio(_language,(short *)0,1,32,0,ERx("4GL"));
        IIputfldio(_srcfile,(short *)0,1,32,0,proc->source);
        IIputfldio(_return_type,(short *)0,1,32,0,proc->return_type.db_data);
        IIputfldio(_nullable,(short *)0,1,32,0,
        ERget(
			    AFE_NULLABLE_MACRO(proc->return_type.db_datatype)
					? F_UG0002_Yes2 : F_UG0007_No2
			));
        IIputfldio(_library,(short *)0,1,32,0,ERget(F_UG0007_No2));
        IIputfldio(_symbol,(short *)0,1,32,0,ERx("!"));
      } /* IIfsetio */
    }
/* # line 125 "ab4pedit.qsc" */	/* host code */
	/*
	** Turn off 'symbol' and 'library'; turn on 'srcfile', which may
	** also have been display-only for a library procedure.  Also, turn
	** off 'nullable' field for return type "none" but make it enterable
	** since host-language procedures may have made it display-only.
	** Turn off "pass decimal as what type" field.
	** Finally, toggle 'warning' depending on whether function name
	** conflicts with system names.
	*/
	notnullable = ( proc->return_type.db_datatype == DB_NODT );
	warning = !IIAFckFuncName(proc->name);
/* # line 136 "ab4pedit.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(54,_symbol,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(54,_library,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(54,_srcfile,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(53,_srcfile,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(17,_srcfile,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(54,_nullable,0,(short *)0,1,30,4,&notnullable);
        IIstfsio(53,_nullable,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(17,_nullable,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(54,_passdec,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(54,_warning,0,(short *)0,1,30,4,&warning);
      } /* IIiqset */
    }
  }
/* # line 150 "ab4pedit.qsc" */	/* host code */
/*
** Name:	_save() -	Save a 4GL Procedure Component Definition
**					from the Form.
** Description:
**	Saves the attributes for a 4GL procedure component object from the
**	component definition form for this class.  This checks if any values
**	changed before modifying the object's attributes and only then writing
**	the object to the database.
**
**	The values that can be changed are the source-code file and the return
**	type.
**
** Input:
**	app	{APPL *}  The application object.
**	proc	{_4GLPROC *}  The 4GL procedure component object.
**
** Returns:
**	{STATUS}  OK, if no errors.
**
** History:
**	02/89 (jhw)  Written.
**	05/90 (jhw)  Added long remark check for save.
*/
bool	iiabGetRetType();
static STATUS
  _save( app, proc )
APPL		*app;
  _4GLPROC *proc;
  {
    char *formname = iiAB4GLProc.form;
    i4 src_change;
    i4 srem_change;
	STATUS	IIAMwr4GLProc();
/* # line 186 "ab4pedit.qsc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&src_change,20,_srcfile,0);
        IIiqfsio((short *)0,1,30,4,&srem_change,20,_short_remark,0);
      } /* IIiqset */
    }
/* # line 191 "ab4pedit.qsc" */	/* host code */
	/* Fields should have been validated already. */
	if ( src_change == 1 )
    {
      char source[48+1];
/* # line 197 "ab4pedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,48+1-1,source,_srcfile);
        } /* IIfsetio */
      }
/* # line 198 "ab4pedit.qsc" */	/* host code */
		if ( !STequal(proc->source, source) )
		{
			iiabNewFileName(proc->source, source);
			proc->source = FEtsalloc( proc->data.tag, source );
			if ((proc->flags & APC_RECOMPILE) == 0)
				proc->flags |= (APC_RECOMPILE|APC_DEFCHANGE);
			proc->data.dirty = TRUE;
		}
    }
	if ( iiabGetRetType( iiAB4GLProc.form, proc->data.tag,
				&proc->return_type ) )
	{
		/* Invalidate our callers */
		IIAMxdsDepSearch((APPL_COMP *)proc, IIAM_zccCompChange,
				 (PTR)NULL);
		if ((proc->flags & APC_RECOMPILE) == 0)
			proc->flags |= (APC_RECOMPILE|APC_DEFCHANGE);
		proc->data.dirty = TRUE;
	}
	if ( srem_change != 0 )
    {
		char	*cp;
      char short_remark[OOSHORTREMSIZE+1];
/* # line 225 "ab4pedit.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
          IIstfsio(20,_short_remark,0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
        } /* IIiqset */
      }
/* # line 226 "ab4pedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,OOSHORTREMSIZE+1-1,short_remark,
          _short_remark);
        } /* IIfsetio */
      }
/* # line 227 "ab4pedit.qsc" */	/* host code */
		if ( ! STequal(proc->short_remark, short_remark)
			&& (cp = FEtsalloc(proc->data.tag, short_remark))
				!= NULL )
		{
			proc->short_remark = cp;
			proc->data.dirty = TRUE;
		}
    }
	_VOID_ IIOOckLongRemark((OO_OBJECT *)proc);
	return proc->data.dirty ? iiabSvComp((APPL_COMP *)proc, IIAMwr4GLProc )
				: OK;
  }
/*
** Name:	_destroy() -	Destroy a 4GL Procedure Component Object.
**
** Description:
**	ABF specific destroy simply removes names for the object.
**
** Input:
**	proc	{_4GLPROC *}  The 4GL procedure component.
**
** History:
**	02/89 (jhw)  Written.
*/
static VOID
_destroy ( proc )
_4GLPROC	*proc;
{
	iiabRmObjLoc(proc->source);	/* remove any objects or timestamps */
	iiabNewFileName(proc->source, (char *)NULL);	/* clear object name */
	IIGNdnDeleteName(AGN_OPROC, proc->symbol);
}
static STATUS
_print ( appl, proc )
APPL		*appl;
_4GLPROC	*proc;
{
	return iiabSfilePrint(appl, proc->source);
}
