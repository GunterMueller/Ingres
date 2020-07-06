# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<st.h>
#include	<ol.h>
#include	<er.h>
#include	<tm.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
#include	<adf.h>
#include	<afe.h>
#include	<fdesc.h>
#include	<abfrts.h>
# include <abclass.h>
# include <oocat.h>
#include	<dmchecks.h>
# include "abclinfo.h"
#include	"abfglobs.h"
#include	"erab.h"
/**
** Name:	abdpedit.qc - ABF Database Procedure Component Edit Module.
**
** Description:
**	Contains the definition of the component class edit information
**	structure for the application database procedure component objects.
**	Defines:
**
**	iiABDBProc	ABF database procedure component class edit information.
**
** History:
**	Revision 6.2  89/02  wong
**	Initial revision.
**
**	02-dec-91 (leighb) DeskTop Porting Change:
**		Added routines to pass data across ABF/IAOM boundary.
**	6-feb-92 (blaise)
**		Added include abfglobs.h.
**
**	Revision 6.5
**
**	26-aug-92 (davel)
**		Simplified _test to reflect changed interface to IIARdbProcCall.
**	16-Nov-92 (donc)
**		Modified COMP_CLASS_INFO initialization to accomodate fix for
**		bug 44162.
**	04-dec-92 (davel)
**		In _display(), make new _passdec field invisible.
**      08-dec-95 (toumi01; from 1.1 axp_osf port)
**		Added kchin's change (6.4) for axp_osf
**              07-Feb-94 (kchin) bug# 62079
**              Added argument declaration for parameter 'desc' of routine
**              _type_valid(), since it is a char *.  On 64-bit platform, the
**              default int data type for 'desc' not declared explicitly will
**              truncate an address which is 64-bit long.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
char	*IIUGdmlStr();
STATUS	iiabPrint();
static VOID	_display();
static STATUS	_save();
static STATUS	_test();
static STATUS	_type_valid();
static READONLY DB_DT_ID	_types[] = { DB_NODT, DB_INT_TYPE };
/*:
** Name:	iiABDBProc -	ABF Database Procedure Component Class
**					Edit Information.
** Description:
**	Defines additional class information for the database procedure
**	component objects for editing.
*/
#define _NewEditExpl	F_AB0052_NewEdit_expl
#define _GoExpl		F_AB0054_GoProc_expl
#define _EmptyExpl	F_AB005E_Empty_expl
#define _PrintExpl	F_AB005F_Print_expl
GLOBALDEF READONLY COMP_CLASS_INFO iiABDBProc = {
	OC_DBPROC,
	ERx("abfprocd"), ERx("abdbproc.hlp"), ERx("abdbprmi.hlp"), 1,
	F_AB0061_ADBDef_title,
	F_AB0045_ADBDet_title,
	sizeof(_types)/sizeof(_types[0]), _types,	/* legal types */
	{NULL,	NULL},	_return_type,	NULL,	_library, NULL,	/* Fields */
	/*{iiabNxtEdit,	AB_NewEdit,	_NewEditExpl,	0},	/* NextEdit */
	{_test,		FE_Go,		_GoExpl,	4},    /* Go, FRSkey4 */
	{NULL,		0,		_EmptyExpl,	0},	/* Edit */
	{NULL,		0,		_EmptyExpl,	0},	/* Compile */
	{NULL,		0,		_EmptyExpl,	0},	/* FormEdit */
	{iiabPrint,	FE_Print,	_PrintExpl,	0},	/* Print */
	_display, _save, NULL, _type_valid
};
/*
** Name:	_display() -	Display DB Procedure Component Definition.
**
** Description:
**	Displays the attributes for a DB procedure component object on the
**	edit/definition form for this class.
**
**	Note:  Also sets the relevant field display attributes ("displayonly",
**	"invisible") since the form is shared with the 4GL and host-language
**	procedure component classes.
**
** Input:
**	proc	{DBPROC *}  The DB procedure component object.
**
** History:
**	02/89 (jhw)  Written.
*/
static VOID
  _display( proc )
  register DBPROC *proc;
  {
    char *formname = iiABDBProc.form;
    i4 notnullable;
    i4 warning;
    char title[63+1];
	GLOBALREF char	*IIabExename;
	/* Turn off 'symbol' and 'srcfile' and turn on 'library' as
	** display-only.  Also, turn off 'nullable' field for return type
	** "none," but make it enterable since host-language procedures
	** may have turned it off.  Turn off "pass decimal as what type"
	** field.  Finally, toggle 'warning' depending on
	** whether function name conflicts with system names.
	*/
	notnullable = ( proc->return_type.db_datatype == DB_NODT );
	warning = !IIAFckFuncName(proc->name);
/* # line 146 "abdpedit.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(54,_symbol,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(54,_srcfile,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(54,_library,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(54,_nullable,0,(short *)0,1,30,4,&notnullable);
        IIstfsio(53,_nullable,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(17,_nullable,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(54,_passdec,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(54,_warning,0,(short *)0,1,30,4,&warning);
      } /* IIiqset */
    }
/* # line 157 "abdpedit.qsc" */	/* host code */
	IIUGfmt(title, sizeof(title)-1, ERget(iiABDBProc.title), 1, IIabExename);
/* # line 159 "abdpedit.qsc" */	/* putform */
    {
      if (IIfsetio(formname) != 0) {
        IIputfldio(_title,(short *)0,1,32,0,title);
        IIputfldio(_language,(short *)0,1,32,0,ERx(IIUGdmlStr(DB_SQL)));
        IIputfldio(_return_type,(short *)0,1,32,0,proc->return_type.db_data);
        IIputfldio(_nullable,(short *)0,1,32,0,
        ERget(
				    AFE_NULLABLE(proc->return_type.db_datatype)
					? F_UG0002_Yes2 : F_UG0007_No2
			));
        IIputfldio(_srcfile,(short *)0,1,32,0,ERx("!"));
        IIputfldio(_symbol,(short *)0,1,32,0,ERx("!"));
      } /* IIfsetio */
    }
  }
/* # line 174 "abdpedit.qsc" */	/* host code */
/*
** Name:	_save() -	Save DB Procedure Component Definition from
**					the Form.
** Description:
**	Saves the attributes for a database procedure component object from the
**	component definition form for this class.  This calls 'iiabGetRetType()'
**	to check if the return type was changed and to modify the object's
**	return type attributes.  Only then will it write the object to the
**	database.
**
** Input:
**	app	{APPL *}  The application object.
**	proc	{DBPROC *}  The DB procedure component object.
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
_save ( app, proc )
APPL		*app;
register DBPROC	*proc;
  {
    char *formname = iiABDBProc.form;
    i4 srem_change;
	STATUS	IIAMwrDBProc();
/* # line 207 "abdpedit.qsc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&srem_change,20,_short_remark,0);
      } /* IIiqset */
    }
/* # line 209 "abdpedit.qsc" */	/* host code */
	/* Fields should have been validated already. */
	if ( iiabGetRetType( iiABDBProc.form, proc->data.tag,
				&proc->return_type ) )
	{
		/* Invalidate our callers */ 
		IIAMxdsDepSearch((APPL_COMP *)proc, IIAM_zccCompChange,
				 (PTR)NULL);
		proc->data.dirty = TRUE;
	}
	if ( srem_change != 0 )
    {
		char	*cp;
      char short_remark[OOSHORTREMSIZE+1];
/* # line 225 "abdpedit.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
          IIstfsio(20,_short_remark,0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
        } /* IIiqset */
      }
/* # line 226 "abdpedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,OOSHORTREMSIZE+1-1,short_remark,
          _short_remark);
        } /* IIfsetio */
      }
/* # line 227 "abdpedit.qsc" */	/* host code */
		if ( ! STequal(proc->short_remark, short_remark)
			&& (cp = FEtsalloc(proc->data.tag, short_remark))
				!= NULL )
		{
			proc->short_remark = cp;
			proc->data.dirty = TRUE;
		}
    }
	_VOID_ IIOOckLongRemark((OO_OBJECT *)proc);
	return proc->data.dirty ? iiabSvComp((APPL_COMP *)proc, IIAMwrDBProc )
				: OK;
  }
/*
** Name:	_type_valid() -	Validate Return Type for Database Procedure.
**
** Description:
**	Validates the entered data type for the return value of a database
**	procedure.
**
** Inputs:
**      comp            {APPL_COMP *}  The object checking the type.
**      desc            {char *}        A string describing the type.
**
** Outputs:
**      dbv            (DB_DATA_TYPE*} A dbv to represent the description.
**
** Returns:
**	{STATUS}  OK, if the data type is valid for database procedures.
**
** History:
**	02/89 (jhw)  Written.
*/
static STATUS
_type_valid ( comp, desc, dbv )
  APPL_COMP *comp;
char            *desc;
register DB_DATA_VALUE	*dbv;
{
  i4 n_inviz = 0;
	STATUS iiabCkTypeSpec();
	if (iiabCkTypeSpec(desc, TRUE, dbv) != OK
	  || ( dbv->db_datatype != DB_NODT 
	      && dbv->db_datatype != DB_INT_TYPE 
	      && dbv->db_datatype != -DB_INT_TYPE )
	    )
	{
		iiabTypeError(desc, E_AB026C_Not_int_type);
		return FAIL;
	}
	/* If user entered 'none', turn off the 'nullable' field.  */
	if (dbv->db_datatype == DB_NODT)
		n_inviz = 1;
/* # line 284 "abdpedit.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,ERx(""),(char *)0, (char *)0) != 0) {
      IIstfsio(54,_nullable,0,(short *)0,1,30,4,&n_inviz);
    } /* IIiqset */
  }
/* # line 286 "abdpedit.qsc" */	/* host code */
	return OK;
}
/*
** Name:	_test() -	Test DB Proc Component Object.
**
** Description:
**	Runs the DB Proc for testing purposes.  This simply calls
**	the proper ABFRT module to run the DB Proc; no parameters
**	are passed.
**
** Input:
**	app	{APPL *}  		The application object <not used>
**	class	{COMP_CLASS_INFO *}	<not used>
**	proc	{DBPROC *}		The DB Proc component object.
**
** Returns:
**	{STATUS}  OK (all the time)
**
** History:
**	02/89 (jhw)  Written.
**	26-aug-92 (davel)
**		Fixed comments and simplified to reflect changed
**		calling interface for IIARdbProcCall().
*/
static STATUS
_test ( app, class, proc )
APPL		*app;
COMP_CLASS_INFO	*class;
DBPROC		*proc;
{
	/* Evenutally, compile the source if necessary */
	IIARdbProcCall(proc->name, (ABRTSPRM *)NULL);
	return OK;
}
