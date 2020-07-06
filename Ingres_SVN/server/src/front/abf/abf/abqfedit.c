# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<cm.h>
#include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<fdesc.h>
#include	<abfrts.h>
# include <abclass.h>
# include <oocat.h>
# include "abclinfo.h"
#include	"erab.h"
/**
** Name:	abqfedit.qc -	ABF QBF Frame Component Edit Module.
**
** Description:
**	Contains the definition of the component class information structure
**	for the QBF frame component object.  Defines:
**
**	iiABqbfFrame	ABF QBF frame component class edit information
**
** History:
**	Revision 6.4
**	03/22/91 (emerson)
**		Fix interoperability bug 36589:
**		Change all calls to abexeprog to remove the 3rd parameter
**		so that images generated in 6.3/02 will continue to work.
**		(Generated C code may contain calls to abexeprog).
**		This parameter was introduced in 6.3/03/00, but all calls
**		to abexeprog specified it as TRUE.  See abfrt/abrtexe.c
**		for further details.
**
**	Revision 6.2  89/02  wong
**	Initial revision.
**
**	16-Nov-92 (donc)
**	Modified COMP_CLASS_INFO initialization to fix bug 44162
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
FORM_REF	*IIAMfaFormAlloc();
STATUS	iiabFormEdit();
STATUS	iiabPrint();
STATUS	iiabVerifyType();
static STATUS	_test();
static STATUS	_edit();
static STATUS	_editform();
static VOID	_display();
static STATUS	_save();
/*:
** Name:	iiABqbfFrame -	ABF QBF Frame Component Class Edit Information.
**
** Description:
**	Defines additional class information for the QBF frame component
**	objects.
*/
#define _NewEditExpl	F_AB0052_NewEdit_expl
#define _GoExpl		F_AB0053_Go_expl
#define _EditExpl	F_AB0057_EditQBF_expl
#define _EmptyExpl	F_AB005E_Empty_expl
#define _FormEditExpl	F_AB005D_EditForm_expl
#define _PrintExpl	F_AB005F_Print_expl
static READONLY
  char _qtype[]= ERx("qtype"), _joindef[]= ERx("joindef");
GLOBALDEF READONLY COMP_CLASS_INFO iiABqbfFrame = {
	OC_QBFFRAME,
	ERx("abfqbfd"), ERx("abfqbfd.hlp"), ERx("abfqbfmi.hlp"), 2,
	ER_OK,
	F_AB0042_AQFDet_title,
	0,	NULL,	/* no types legal */
	{_joindef,	_formname},	NULL,	NULL,	NULL, NULL,	/* Fields */
	/*{iiabNxtEdit,	AB_NewEdit,	_NewEditExpl,	0},	/* NextEdit */
	{_test,		FE_Go,		_GoExpl,	4},    /* Go, FRSkey4 */
	{_edit,		FE_Edit,	_EditExpl,	0},	/* Edit */
	{NULL,		FE_Compile,	_EmptyExpl,	0},	/* Compile */
	{_editform,	AB_FormEdit,	_FormEditExpl,	0},	/* FormEdit */
	{iiabPrint,	FE_Print,	_PrintExpl,	0},	/* Print */
	_display, _save, NULL, iiabVerifyType
};
  static char *_Table = NULL;
  static char *_JoinDef = NULL;
/*
** Name:	_edit() -	Edit QBF Frame Query Object from Form.
**
** Description:
**	Edits the query object for a QBF frame component object as specified
**	on the component definition form.  The query object is either a table
**	or a join definition.
**
** Inputs:
**	app	{APPL *}  The application object.
**	form	{char *}  The component definition form name.
**
** Returns:
**	{STATUS}  OK, if no errors.
**
** History:
**	02/89 (jhw)  Written.
**	11/89 (jhw)  Corrected argument set-up to test 'formname' and to
**			set correct number of arguments to 'abexeprog()'.
**	14-feb-90 (kenl)
**		This is my second attempt at adding the -J flag.  To whomever
**		touches this file in the future: don't remove it again.
*/
static STATUS
  _edit( app, dummy, form )
register APPL	*app;
OO_OBJECT	*dummy;
  char *form;
  {
    char qtype[1+1];
    char qobj[FE_MAXNAME+1];
/* # line 135 "abqfedit.qsc" */	/* getform */
    {
      if (IIfsetio(form) != 0) {
        IIgetfldio((short *)0,1,32,1+1-1,qtype,_qtype);
        IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,qobj,_joindef);
      } /* IIfsetio */
    }
/* # line 136 "abqfedit.qsc" */	/* host code */
	if ( CMcmpnocase(qtype, _Table) == 0 )
	{ /* DB table */
		abexeprog(ERx("tables"), _iiOOempty, 0);
	}
	else
	{ /* *qtype == 'J' ==> JoinDef */
		abexeprog(ERx("qbf"), ERx("editjoindef = %S"), 1, qobj);
	}
	return OK;
  }
/*
** Name:	_display() -	Display a QBF Frame Component Definition.
**
** Description:
**	Displays the attributes of a QBF frame component on its component
**	definition form.
**
** Inputs:
**	frm	{QBF_FRAME *}  The QBF frame component object.
**
** History:
**	02/89 (jhw)  Written.
*/
static VOID
  _display( frm )
  register QBF_FRAME *frm;
  {
    char *formname = iiABqbfFrame.form;
	if ( _Table == NULL )
	{
		_Table = ERget(FE_Table);
		_JoinDef = ERget(FE_JoinDef);
	}
/* # line 172 "abqfedit.qsc" */	/* putform */
    {
      if (IIfsetio(formname) != 0) {
        IIputfldio(_joindef,(short *)0,1,32,0,frm->qry_object);
        IIputfldio(_qtype,(short *)0,1,32,0,
        ERx( frm->qry_type == OC_JOINDEF
							? _JoinDef : _Table
				));
        IIputfldio(_formname,(short *)0,1,32,0,
        ERx( frm->form != NULL 
							? frm->form->name : _
				));
        IIputfldio(_comline,(short *)0,1,32,0,frm->cmd_flags);
      } /* IIfsetio */
    }
  }
/* # line 183 "abqfedit.qsc" */	/* host code */
/*
** Name:	_editform() -	Edit QBF Frame Form from Name in Field.
**
** Description:
**	Edits the form for a QBF frame component object as
**	specified on the component definition form.
**
** Inputs:
**	app	{APPL *}  The application object.
**	class	{COMP_CLASS_INFO *}  The edit class information.
**	frm	{QBF_FRAME *}  The QBF frame component.
**
** Returns:
**	{STATUS}  OK, if no errors.
**
** History:
**	03/89 (jhw)  Written.
*/
static STATUS
  _editform( app, class, frm )
APPL			*app;
  COMP_CLASS_INFO *class;
QBF_FRAME		*frm;
  {
	FORM_REF	*form_ref = NULL;
    char form[FE_MAXNAME+1];
/* # line 210 "abqfedit.qsc" */	/* getform */
    {
      if (IIfsetio(class->form) != 0) {
        IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,form,_formname);
      } /* IIfsetio */
    }
/* # line 212 "abqfedit.qsc" */	/* host code */
	if ( STtrmwhite(form) > 0 )
	{
		if ( ( frm->form == NULL || !STequal(form, frm->form->name) ) &&
				_save(app, frm) != OK )
			return FAIL;
		form_ref = frm->form;
	}
	return iiabFormEdit(app, form_ref);
  }
/*
** Name:	_save() -	Save QBF Frame Component Definition from Form.
**
** Description:
**	Saves the attributes for a QBF frame component object from the component
**	definition form for this class.  This checks if any values changed
**	before modifying the object's attributes and only then writing the
**	object to the database.
**
**	The values that can be changed are the query object name, the query
**	object type, the optional form name, and the optional QBF command line.
**
** Input:
**	app	{APPL *}  The application object.
**	frm	{QBF_FRAME *}  The QBF frame component object.
**
** Returns:
**	{STATUS}  OK, if no errors.
**
** History:
**	02/89 (jhw)  Written.
**	05/90 (jhw)  Added long remark check for save.
*/
static STATUS
  _save( app, frm )
APPL			*app;
register QBF_FRAME	*frm;
  {
    register char *formname = iiABqbfFrame.form;
    i4 qryo_change;
    i4 qryt_change;
    i4 form_change;
    i4 cmd_change;
    i4 srem_change;
	STATUS		IIAMwrQBFFrame();
/* # line 260 "abqfedit.qsc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&qryo_change,20,_joindef,0);
        IIiqfsio((short *)0,1,30,4,&qryt_change,20,_qtype,0);
        IIiqfsio((short *)0,1,30,4,&form_change,20,_formname,0);
        IIiqfsio((short *)0,1,30,4,&cmd_change,20,_comline,0);
        IIiqfsio((short *)0,1,30,4,&srem_change,20,_short_remark,0);
      } /* IIiqset */
    }
/* # line 268 "abqfedit.qsc" */	/* host code */
	/* Fields should have been validated already. */
	if ( qryo_change == 1 )
    {
      char qry_obj[FE_MAXNAME+1];
/* # line 274 "abqfedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,qry_obj,_joindef);
        } /* IIfsetio */
      }
/* # line 275 "abqfedit.qsc" */	/* host code */
		/* Query object is a required DB reference */
		if ( !STequal(frm->qry_object, qry_obj) )
		{
			frm->qry_object = FEtsalloc( frm->data.tag, qry_obj );
			frm->data.dirty = TRUE;
		}
    }
	if ( qryt_change == 1 )
    {
      char qry_type[1+1];
/* # line 287 "abqfedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,1+1-1,qry_type,_qtype);
        } /* IIfsetio */
      }
/* # line 288 "abqfedit.qsc" */	/* host code */
		if ( ( CMcmpnocase(qry_type, _JoinDef) == 0 ) !=
				(frm->qry_type == OC_JOINDEF ) )
		{
			frm->qry_type = ( CMcmpnocase(qry_type, _JoinDef) == 0 )
						? OC_JOINDEF : 0;
			frm->data.dirty = TRUE;
		}
    }
	if ( form_change == 1 )
    {
      char form[FE_MAXNAME+1];
/* # line 301 "abqfedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,form,_formname);
        } /* IIfsetio */
      }
/* # line 302 "abqfedit.qsc" */	/* host code */
		/* Form name is optional DB reference */
		if ( ( frm->form == NULL && *form != EOS )  ||
				( frm->form != NULL &&
					!STequal(frm->form->name, form)
				) )
		{
			/* QBF Frames cannot use compiled forms but
			** use special Form References internally.
			*/
			if ( *form == EOS )
				frm->form = NULL;
			else if ( frm->form == NULL )
				frm->form = IIAMfaFormAlloc(app, form);
			else
				frm->form->name = FEtsalloc(frm->data.tag,form);
			frm->data.dirty = TRUE;
		}
    }
	if ( cmd_change == 1 )
    {
      char cmdline[48+1];
/* # line 325 "abqfedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,48+1-1,cmdline,_comline);
        } /* IIfsetio */
      }
/* # line 326 "abqfedit.qsc" */	/* host code */
		if ( !STequal(frm->cmd_flags, cmdline) )
		{
			frm->cmd_flags = FEtsalloc( frm->data.tag, cmdline );
			frm->data.dirty = TRUE;
		}
    }
	if ( srem_change != 0 )
    {
		char	*cp;
      char short_remark[OOSHORTREMSIZE+1];
/* # line 338 "abqfedit.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
          IIstfsio(20,_short_remark,0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
        } /* IIiqset */
      }
/* # line 339 "abqfedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,OOSHORTREMSIZE+1-1,short_remark,
          _short_remark);
        } /* IIfsetio */
      }
/* # line 340 "abqfedit.qsc" */	/* host code */
		if ( ! STequal(frm->short_remark, short_remark)
			&& (cp = FEtsalloc(frm->data.tag, short_remark))
				!= NULL )
		{
			frm->short_remark = cp;
			frm->data.dirty = TRUE;
		}
    }
	_VOID_ IIOOckLongRemark((OO_OBJECT *)frm);
	return frm->data.dirty ? iiabSvComp((APPL_COMP *)frm, IIAMwrQBFFrame)
				: OK;
  }
/*
** Name:	_test() -	Test QBF Frame Component Object.
**
** Description:
**	Runs the QBF frame for testing purposes.  This simply creates a run-time
**	frame structure from the frame definition and then runs it.
**
** Input:
**	app	{APPL *}  The application object.
**	fclass	{COMP_CLASS_INFO *}  The QBF frame component class edit
**					information.
**	frm	{QBF_FRAME *}  The QBF frame component object.
**
** Returns:
**	{STATUS}  OK, if no errors.
**
** History:
**	02/89 (jhw)  Written.
*/
static STATUS
  _test( app, fclass, frm )
APPL		*app;
COMP_CLASS_INFO	*fclass;
QBF_FRAME	*frm;
  {
    char *form = iiABqbfFrame.form;
    char formname[FE_MAXNAME+1];
    char qry_object[FE_MAXNAME+1];
    char qry_type[1+1];
    char comline[ABCOMSIZE+1];
	ABRTSFRM	rtsfrm;
	ABRTSVAR	rtsvar;
	ABRTSFO		rtsform;
	/*
	** Build an ABRTSFRM structure and pass it
	** to the run-time routine 'abrtsqbf()'.
	*/
/* # line 394 "abqfedit.qsc" */	/* getform */
    {
      if (IIfsetio(form) != 0) {
        IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,qry_object,_joindef);
        IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,formname,_formname);
        IIgetfldio((short *)0,1,32,ABCOMSIZE+1-1,comline,_comline);
        IIgetfldio((short *)0,1,32,1+1-1,qry_type,_qtype);
      } /* IIfsetio */
    }
/* # line 400 "abqfedit.qsc" */	/* host code */
	rtsfrm.abrfrname = frm->name;
	rtsfrm.abrfrtype = OC_QBFFRAME;
	rtsfrm.abrfrvar = &rtsvar;
	STcopy(qry_object, rtsvar.abrfrvqbf.abrqfrelid);
	STcopy(comline, rtsvar.abrfrvqbf.abrqfcomline);
	rtsvar.abrfrvqbf.abrqfjdef = CMcmpnocase(qry_type, _JoinDef) == 0;
	if ( STtrmwhite(formname) <= 0 )
		rtsfrm.abrform = NULL;
	else
	{
		rtsfrm.abrform = &rtsform;
		rtsform.abrforef = FALSE;
		rtsform.abrfoname = formname;
		rtsform.abrfosource = 0;
	}
	abrtsqbf(&rtsfrm, (ABRTSPRM *)NULL);
	return OK;
  }
