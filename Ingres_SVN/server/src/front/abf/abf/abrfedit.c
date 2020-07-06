# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<st.h>
#include	<lo.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
#include	<fdesc.h>
#include	<abfrts.h>
# include <abclass.h>
# include <oocat.h>
# include "abclinfo.h"
#include	"erab.h"
/**
** Name:	abrfedit.qc -	ABF Report Frame Component Edit Module.
**
** Description:
**	Contains the definition of the component class information structure
**	for the Report frame component object.  Defines:
**
**	iiABreportFrame	ABF report frame component class edit information.
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
**	11/16/92 (DonC)
**		Change COMP_CLASS_INFO initialization to fix for bug 44162
**
**	02/26/93 (DonC)
**		Modified _save so that when a user blanks out the report 
**		parameter form the form name is removed from the application
**		components list for the frame. This rectifies bug 40140.
**
**      17-jan-94 (connie) Bug #56909
**              Modified _save to complete the previous fix(40140) by marking
**              the dirty flag to TRUE for saving the update.
**
**	Revision 6.3/03/00  90/01/05  kenl
**	Added support for 'RBF Report?' field and toggling of source file field.
**
**	Revision 6.2  89/02  wong
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
FORM_REF	*IIAMformRef();
STATUS	iiabFileEdit();
STATUS	iiabFormEdit();
STATUS	iiabPrint();
STATUS	iiabVerifyType();
static STATUS	_test();
static STATUS	_edit();
static STATUS	_compile();
static STATUS	_editform();
static VOID	_display();
static STATUS	_save();
static VOID	_destroy();
/*:
** Name:	iiABreportFrame - ABF Report Frame Component Class Information.
**
** Description:
**	Defines additional class information for the report frame component
**	objects.
*/
#define _NewEditExpl	F_AB0052_NewEdit_expl
#define _GoExpl		F_AB0053_Go_expl
#define _EditExpl	F_AB0056_EditRep_expl
#define _CompileExpl	F_AB005B_CompileRep_expl
#define _FormEditExpl	F_AB005D_EditForm_expl
#define _PrintExpl	F_AB005F_Print_expl
static READONLY
  char _report[]= ERx("report"), _compform[]= ERx("compform");
GLOBALDEF READONLY COMP_CLASS_INFO iiABreportFrame = {
	OC_RWFRAME,
	ERx("abfreportd"), ERx("abfrepwd.hlp"), ERx("abfrepmi.hlp"), 3,
	ER_OK,
	F_AB0041_ARFDet_title,
	0,	NULL,	/* no types legal */
	{_report,	_formname}, NULL, NULL,	_library, NULL, 	/* Fields */
					/* note:  no activation on source */
	/*{iiabNxtEdit,	AB_NewEdit,	_NewEditExpl,	0},	/* NextEdit */
	{_test,		FE_Go,		_GoExpl,	4},    /* Go, FRSkey4 */
	{_edit,		FE_Edit,	_EditExpl,	0},	/* Edit */
	{_compile,	FE_Compile,	_CompileExpl,	0},	/* Compile */
	{_editform,	AB_FormEdit,	_FormEditExpl,	0},	/* FormEdit */
	{iiabPrint,	FE_Print,	_PrintExpl,	0},	/* Print */
	_display, _save, _destroy, NULL
};
/*
** Name:	_edit() -	Edit Report Frame Report Object from Form.
**
** Description:
**	Edits the report object for a Report frame component object as specified
**	on the component definition form.  The report object is either edited by
**	modifying the report commands in a file, or through RBF.
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
**	26-dec-91 (pearl)  bug 41647
**		Use 'edit_restrict' instead of 'report' as argument to 
**		abexeprog.  'edit_restrict' will pass the -edit_restrict 
**		flag to RBF, which signifies that it is being called 
**		as part of a restricted edit function.
**	21-jan-92 (pearl)  bug 41791
**		Change 'table' parameter to 'rpt_object'.
*/
static STATUS
  _edit( app, dummy, form )
register APPL	*app;
OO_OBJECT	*dummy;
  char *form;
  {
    char is_rbf[10];
/* # line 151 "abrfedit.qsc" */	/* getform */
    {
      if (IIfsetio(form) != 0) {
        IIgetfldio((short *)0,1,32,9,is_rbf,_library);
      } /* IIfsetio */
    }
/* # line 152 "abrfedit.qsc" */	/* host code */
	if ( !IIUGyn(is_rbf, (STATUS *)NULL))  
	{
    char srcfile[48+1];
/* # line 156 "abrfedit.qsc" */	/* getform */
    {
      if (IIfsetio(form) != 0) {
        IIgetfldio((short *)0,1,32,48+1-1,srcfile,_srcfile);
      } /* IIfsetio */
    }
/* # line 157 "abrfedit.qsc" */	/* host code */
		iiabFileEdit(app, NULL, srcfile);
	}
	else
	{
#ifdef PCINGRES
		IIUGerr(E_AB003D_no_file, 0, 0);
#else
    char report[FE_MAXNAME+1];
/* # line 166 "abrfedit.qsc" */	/* getform */
    {
      if (IIfsetio(form) != 0) {
        IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,report,_report);
      } /* IIfsetio */
    }
/* # line 167 "abrfedit.qsc" */	/* host code */
		abexeprog(ERx("rbf"), 
				ERx("rpt_object = %S, edit_restrict = %S"),
				2, report, ERx(""));
#endif
	}
  }
/*
** Name:	_display() -	Display a Report Frame Component Object on the
**					Component Definition Form.
** Description:
**	Displays the attributes of a report frame component on its component
**	definition form.
**
** Inputs:
**	frm	{REPORT_FRAME *}  The report frame component object.
**
** History:
**	02/89 (jhw)  Written.
*/
static VOID
  _display( frm )
  register REPORT_FRAME *frm;
  {
    char *formname = iiABreportFrame.form;
/* # line 193 "abrfedit.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(54,_srcfile,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 195 "abrfedit.qsc" */	/* putform */
    {
      if (IIfsetio(formname) != 0) {
        IIputfldio(_library,(short *)0,1,32,0,
        ERget( ( *frm->source == EOS )
						? F_UG0001_Yes1 : F_UG0006_No1));
        IIputfldio(_report,(short *)0,1,32,0,frm->report);
        IIputfldio(_srcfile,(short *)0,1,32,0,frm->source);
        IIputfldio(_formname,(short *)0,1,32,0,
        ERx( frm->form != NULL ?
						frm->form->name : _
			));
        IIputfldio(_outfile,(short *)0,1,32,0,frm->output);
        IIputfldio(_comline,(short *)0,1,32,0,frm->cmd_flags);
      } /* IIfsetio */
    }
/* # line 207 "abrfedit.qsc" */	/* host code */
	if ( *frm->source == EOS )
	{ /* RBF report ... */
		/* ... set 'srcfile' display-only, value "<none>". */
/* # line 210 "abrfedit.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(53,_srcfile,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 211 "abrfedit.qsc" */	/* putform */
    {
      if (IIfsetio(formname) != 0) {
        IIputfldio(_srcfile,(short *)0,1,32,0,ERx("<none>"));
      } /* IIfsetio */
    }
/* # line 212 "abrfedit.qsc" */	/* host code */
	}
	/* 
	** Make the "compile form" field invisible if the application uses
	** only compiled forms; otherwise set it from the frame definition.
	*/
	if ((frm->appl->flags & APC_DBFORMS) == 0)
	{
/* # line 220 "abrfedit.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(54,_compform,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 221 "abrfedit.qsc" */	/* host code */
	}
	else
	{
/* # line 224 "abrfedit.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(54,_compform,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 225 "abrfedit.qsc" */	/* putform */
    {
      if (IIfsetio(formname) != 0) {
        IIputfldio(_compform,(short *)0,1,32,0,
        ERget((frm->flags & APC_DBFORM) ?
						F_UG0007_No2 : F_UG0002_Yes2));
      } /* IIfsetio */
    }
/* # line 228 "abrfedit.qsc" */	/* host code */
	}	
  }
static STATUS
  _compile( app, class, comp )
register APPL		*app;
  COMP_CLASS_INFO *class;
APPL_COMP		*comp;
  {
    char is_rbf[10];
	LOCATION	*iiabMkLoc();
/* # line 242 "abrfedit.qsc" */	/* getform */
    {
      if (IIfsetio(class->form) != 0) {
        IIgetfldio((short *)0,1,32,9,is_rbf,_library);
      } /* IIfsetio */
    }
/* # line 243 "abrfedit.qsc" */	/* host code */
	if ( IIUGyn(is_rbf, (STATUS *)NULL) )
		IIUGerr(E_AB003E_NoRepFile, UG_ERR_ERROR, 0);
	else
	{
    char srcfile[48+1];
/* # line 249 "abrfedit.qsc" */	/* getform */
    {
      if (IIfsetio(class->form) != 0) {
        IIgetfldio((short *)0,1,32,48+1-1,srcfile,_srcfile);
      } /* IIfsetio */
    }
/* # line 250 "abrfedit.qsc" */	/* host code */
		IIUGmsg(ERget(S_AB0040_Compiling), FALSE, 1, srcfile);
		abexeprog( ERx("sreport"), ERx("file = %L"),
				1, iiabMkLoc(app->source, srcfile)
		);
#ifdef FT3270
		FTmore();
#endif
	}
  }
/*
** Name:	_editform() -	Edit Report Frame Parameter Form.
**
** Description:
**	Edits the parameter form for a Report frame component object as
**	specified on the component definition form.
**
** Inputs:
**	app	{APPL *}  The application object.
**	class	{COMP_CLASS_INFO *}  The edit class information.
**	frm	{REPORT_FRAME *}  The report frame component.
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
REPORT_FRAME		*frm;
  {
	FORM_REF	*form_ref = NULL;
    char form[FE_MAXNAME+1];
/* # line 287 "abrfedit.qsc" */	/* getform */
    {
      if (IIfsetio(class->form) != 0) {
        IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,form,_formname);
      } /* IIfsetio */
    }
/* # line 289 "abrfedit.qsc" */	/* host code */
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
** Name:	_save() -	Save Report Frame Component Definition from
**					the Form.
** Description:
**	Saves the attributes for a report frame component object from the
**	component definition form for this class.  This checks if any values
**	changed before modifying the object's attributes and only then writing
**	the object to the database.
**
**	The values that can be changed are the report object name, the optional
**	source-code file, the optional form name, the optional output file, and
**	the optional Report Writer command line.
**
** Input:
**	app	{APPL *}  The application object.
**	frm	{REPORT_FRAME *}  The report frame component object.
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
register REPORT_FRAME	*frm;
  {
	bool		is_rbf;
    register char *formname = iiABreportFrame.form;
    i4 rbf_change;
    i4 rep_change;
    i4 form_change;
    i4 outf_change;
    i4 cmd_change;
    i4 srem_change;
    i4 compform_change;
    char isrbf[10];
	FORM_REF	*IIAMfrChangeRef();
	STATUS		IIAMwrRepFrame();
/* # line 344 "abrfedit.qsc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&rbf_change,20,_library,0);
        IIiqfsio((short *)0,1,30,4,&rep_change,20,_report,0);
        IIiqfsio((short *)0,1,30,4,&form_change,20,_formname,0);
        IIiqfsio((short *)0,1,30,4,&outf_change,20,_outfile,0);
        IIiqfsio((short *)0,1,30,4,&cmd_change,20,_comline,0);
        IIiqfsio((short *)0,1,30,4,&srem_change,20,_short_remark,0);
        IIiqfsio((short *)0,1,30,4,&compform_change,20,_compform,0);
      } /* IIiqset */
    }
/* # line 354 "abrfedit.qsc" */	/* host code */
	/* Fields should have been validated already. */
	if ( rep_change == 1 )
    {
      char report[FE_MAXNAME+1];
/* # line 360 "abrfedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,report,_report);
        } /* IIfsetio */
      }
/* # line 361 "abrfedit.qsc" */	/* host code */
		/* Report is a required DB reference */
		if ( !STequal(frm->report, report) )
		{
			frm->report = FEtsalloc( frm->data.tag, report );
			frm->data.dirty = TRUE;
		}
    }
/* # line 369 "abrfedit.qsc" */	/* getform */
    {
      if (IIfsetio(formname) != 0) {
        IIgetfldio((short *)0,1,32,9,isrbf,_library);
      } /* IIfsetio */
    }
/* # line 370 "abrfedit.qsc" */	/* host code */
	is_rbf = IIUGyn(isrbf, (STATUS *)NULL);
	if ( rbf_change == 1 )
    {
		if ( is_rbf != ( *frm->source == EOS ) )
		{
			if ( is_rbf )
			{ /* clear object name */
				frm->source = _iiOOempty;
			}
			frm->data.dirty = TRUE;
		}
    }
	if ( *frm->source != EOS || ( rbf_change == 1 && !is_rbf ) )
    {
      i4 src_change;
/* # line 388 "abrfedit.qsc" */	/* inquire_frs */
      {
        if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&src_change,20,_srcfile,0);
        } /* IIiqset */
      }
/* # line 389 "abrfedit.qsc" */	/* host code */
		if ( src_change == 1 )
      {
        char source[48+1];
/* # line 393 "abrfedit.qsc" */	/* getform */
        {
          if (IIfsetio(formname) != 0) {
            IIgetfldio((short *)0,1,32,48+1-1,source,_srcfile);
          } /* IIfsetio */
        }
/* # line 394 "abrfedit.qsc" */	/* host code */
			if ( *frm->source == EOS ||
					!STequal(frm->source, source) )
			{
				frm->source = FEtsalloc(frm->data.tag, source);
				frm->data.dirty = TRUE;
			}
      }
    }
	if ( form_change == 1 )
    {
      char form[FE_MAXNAME+1];
/* # line 407 "abrfedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,form,_formname);
        } /* IIfsetio */
      }
/* # line 408 "abrfedit.qsc" */	/* host code */
		/* Form name is optional DB reference */
		if ( ( frm->form == NULL && *form != EOS )  ||
				( frm->form != NULL && *form != EOS &&
					!STequal(frm->form->name, form)
				) )
		{
			if ( frm->form != NULL )
				iiabFrRemoveNames(frm->form);
			frm->form = IIAMfrChangeRef( app, frm->form, form );
			/* If it's a new FORM_REF, generate new names */
			if (frm->form->refs == 1)
				iiabFoRefNames(frm->form);
			frm->data.dirty = TRUE;
		}
		/* This section was added in order to fix bug 40140
		** Remove the form name from the application component
		** list and remove the name (if this is the last
		** reference to it).
		*/
		else if ( *form == EOS && frm->form != NULL ) {
			iiabFrRemoveNames(frm->form);
			IIAMfrRemoveRef(frm->form);
			frm->form = NULL;
			frm->data.dirty = TRUE;
		}
    }
	if ( outf_change == 1 )
    {
      char output[48+1];
/* # line 439 "abrfedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,48+1-1,output,_outfile);
        } /* IIfsetio */
      }
/* # line 440 "abrfedit.qsc" */	/* host code */
		if ( !STequal(frm->output, output) )
		{
			frm->output = FEtsalloc( frm->data.tag, output );
			frm->data.dirty = TRUE;
		}
    }
	if ( cmd_change == 1 )
    {
      char cmdline[48+1];
/* # line 451 "abrfedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,48+1-1,cmdline,_comline);
        } /* IIfsetio */
      }
/* # line 452 "abrfedit.qsc" */	/* host code */
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
/* # line 464 "abrfedit.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
          IIstfsio(20,_short_remark,0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
        } /* IIiqset */
      }
/* # line 465 "abrfedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,OOSHORTREMSIZE+1-1,short_remark,
          _short_remark);
        } /* IIfsetio */
      }
/* # line 466 "abrfedit.qsc" */	/* host code */
		if ( ! STequal(frm->short_remark, short_remark)
			&& (cp = FEtsalloc(frm->data.tag, short_remark))
				!= NULL )
		{
			frm->short_remark = cp;
			frm->data.dirty = TRUE;
		}
    }
	if (compform_change != 0)
	{ /* Whether to compile the form */
		bool		is_compform;
		bool		was_compform = ((frm->flags & APC_DBFORM) == 0);
    char compform[10];
/* # line 481 "abrfedit.qsc" */	/* getform */
    {
      if (IIfsetio(formname) != 0) {
        IIgetfldio((short *)0,1,32,9,compform,_compform);
      } /* IIfsetio */
    }
/* # line 482 "abrfedit.qsc" */	/* host code */
		is_compform = IIUGyn(compform, (STATUS *)NULL);
		if (is_compform && !was_compform)
		{
			frm->flags &= ~APC_DBFORM;
			frm->data.dirty = TRUE;
		}
		else if (!is_compform && was_compform)
		{
			frm->flags |= APC_DBFORM;
			frm->data.dirty = TRUE;
		}
	}
	_VOID_ IIOOckLongRemark((OO_OBJECT *)frm);
	if ( frm->data.dirty )
		return iiabSvComp((APPL_COMP *)frm, IIAMwrRepFrame);
	else if ( frm->form != NULL && frm->form->data.dirty )
		return IIAMwrFormRef(frm->form);
	return OK;
  }
/*
** Name:	_destroy() -	Destroy a Report Frame Component Object.
**
** Description:
**	ABF specific destroy simply removes names for the object.
**
** Input:
**	frm	{REPORT_FRAME *}  The report frame component.
**
** History:
**	02/89 (jhw)  Written.
**	01/90 (jhw)  Removed 'frm->app' parameter to 'IIAMfrRemoveRef()',
**		since it is NULL by the time this routine is called.  #9533.
*/
static VOID
_destroy ( frm )
REPORT_FRAME	*frm;
{
	if ( frm->form != NULL )
	{
		iiabFrRemoveNames(frm->form);
		IIAMfrRemoveRef(frm->form);
	}
}
/*
** Name:	_test() -	Test Report Frame Component Object.
**
** Description:
**	Runs the report frame for testing purposes.  This simply creates a
**	run-time frame structure from the frame definition and then runs it.
**
** Input:
**	app	{APPL *}  The application object.
**	class	{COMP_CLASS_INFO *}  The report frame component class edit
**					information.
**	frm	{REPORT_FRAME *}  The report frame component object.
**
** Returns:
**	{STATUS}  OK, if no errors.
**
** History:
**	02/89 (jhw)  Written.
*/
static STATUS
  _test( app, class, frm )
APPL		*app;
COMP_CLASS_INFO	*class;
REPORT_FRAME	*frm;
  {
    char *form = iiABreportFrame.form;
    char report[FE_MAXNAME+1];
    char formname[FE_MAXNAME+1];
    char output[48+1];
    char comline[ABCOMSIZE+1];
	ABRTSFRM	rtsfrm;
	ABRTSVAR	rtsvar;
	ABRTSFO		rtsform;
	/* Evenutally, compile the source if necessary */
	/*
	** Build an ABRTSFRM structure and pass it
	** to the run-time routine 'IIARrwfReport()'.
	*/
/* # line 570 "abrfedit.qsc" */	/* getform */
    {
      if (IIfsetio(form) != 0) {
        IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,report,_report);
        IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,formname,_formname);
        IIgetfldio((short *)0,1,32,ABCOMSIZE+1-1,comline,_comline);
        IIgetfldio((short *)0,1,32,48+1-1,output,_outfile);
      } /* IIfsetio */
    }
/* # line 576 "abrfedit.qsc" */	/* host code */
	rtsfrm.abrfrname = frm->name;
	rtsfrm.abrfrtype = OC_REPORT;
	rtsfrm.abrfrvar = &rtsvar;
	STcopy(report, rtsvar.abrfrvrw.abrrwname);
	STcopy(comline, rtsvar.abrfrvrw.abrrwcomline);
	STcopy(output, rtsvar.abrfrvrw.abrrwoutput);
	if ( STtrmwhite(formname) <= 0 )
		rtsfrm.abrform = NULL;
	else
	{ /* Form name should be fetched from database */
		rtsfrm.abrform = &rtsform;
		rtsform.abrforef = FALSE;
		rtsform.abrfoname = formname;
		rtsform.abrfosource = ABFOSDB;
	}
	IIARrwfReport(&rtsfrm, (ABRTSPRM *)NULL);
	return OK;
  }
