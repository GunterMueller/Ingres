# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<st.h>
#include	<lo.h>
#include	<si.h>
#include	<er.h>
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
# include <metafrm.h>
#include        <dmchecks.h>
# include "abclinfo.h"
#include	"abfglobs.h"
#include	"gncode.h"
#include	"erab.h"
/**
** Name:	abufedit.qc -	ABF User Frame Component Edit Module.
**
** Description:
**	Contains the definition of the component class edit information
**	structure for the application user frame component objects.  Defines:
**
**	iiABuserFrame	ABF user frame component class edit information.
**
** History:
**	Revision 6.2  89/02  wong
**	Initial revision.
**
**	02-dec-91 (leighb) DeskTop Porting Change:
**		Added routines to pass data across ABF/IAOM boundary.
** 6-Feb-92 (blaise)
**	Added include abfglobs.h.
** 16-nov-1992 (DonC) - Changed COMP_CLASS_INFO initialization in re: bug 44162
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
FORM_REF	*IIAMformRef();
STATUS	iiabTstComp();
STATUS	iiabSrEdit();
STATUS	iiabFormEdit();
STATUS	iiabSrCompile();
STATUS	iiabSfilePrint();
STATUS	iiabVerifyType();
FUNC_EXTERN	COMP_CLASS_INFO	*iiabApCompClass();
FUNC_EXTERN	char 	*IIFGmsnMenuStyleName();
FUNC_EXTERN	i4	IIFGmsmMenuStyleMask();
FUNC_EXTERN 	char	*UGcat_now();
GLOBALREF bool	IIabVision;
static STATUS	uf_editform();
static STATUS	uf_print();
static VOID	uf_display();
static STATUS	uf_save();
static VOID	uf_destroy();
static READONLY char	basedate[] = "1970_01_01 00:00:01 GMT";
static READONLY
  char _compform[]= ERx("compform"), _MenuStyle[]= ERx("menustyle");
/*:
** Name:	iiABuserFrame -	ABF User Frame Component Class Edit Information.
**
** Description:
**	Defines additional class information for the user frame component
**	objects for use in editing.
*/
#define _NewEditExpl	F_AB0052_NewEdit_expl
#define _GoExpl		F_AB0053_Go_expl
#define _EditExpl	F_AB0055_Edit4GL_expl
#define _CompileExpl	F_AB005A_Compile4GL_expl
#define _FormEditExpl	F_AB005D_EditForm_expl
#define _PrintExpl	F_AB005F_Print_expl
/*
** for all the "metaframe" types, we set up identical
** COMP_CLASS_INFO structures to the user frame.  Somewhat
** wasteful of space, but this allows most of the existing
** code to work transparently, as well as allowing us to
** tailor them more specifically later on.
*/
GLOBALDEF READONLY COMP_CLASS_INFO iiABmuFrame = {
	OC_MUFRAME,
	ERx("abfuserd"), ERx("abfuserd.hlp"), ERx("abfusemi.hlp"), 2,
	F_AB0096_MENU,
	F_AB00C1_VMFDet_title,
	-1,	NULL,	/* all types legal */
	{_formname,	NULL},	_return_type,	_srcfile, NULL,	NULL, /* Fields */
	/*{iiabNxtEdit,	AB_NewEdit,	_NewEditExpl,	0},	/* NextEdt */
	{iiabTstComp,	FE_Go,		_GoExpl,	4},    /* Go, FRSkey4 */
	{iiabSrEdit,	FE_Edit,	_EditExpl,	0},	/* Edit */
	{iiabSrCompile,	FE_Compile,	_CompileExpl,	0},	/* Compile */
	{uf_editform,	AB_FormEdit,	_FormEditExpl,	0},	/* FormEdit */
	{uf_print,	FE_Print,	_PrintExpl,	0},	/* Print */
	uf_display, uf_save, uf_destroy, iiabVerifyType
};
GLOBALDEF READONLY COMP_CLASS_INFO iiABapFrame = {
	OC_APPFRAME,
	ERx("abfuserd"), ERx("abfuserd.hlp"), ERx("abfusemi.hlp"), 2,
	F_AB0097_APPEND,
	F_AB00C2_VAFDet_title,
	-1,	NULL,	/* all types legal */
	{_formname,	NULL},	_return_type,	_srcfile, NULL,	NULL, /* Fields */
	/*{iiabNxtEdit,	AB_NewEdit,	_NewEditExpl,	0},	/* NextEdt */
	{iiabTstComp,	FE_Go,		_GoExpl,	4},    /* Go, FRSkey4 */
	{iiabSrEdit,	FE_Edit,	_EditExpl,	0},	/* Edit */
	{iiabSrCompile,	FE_Compile,	_CompileExpl,	0},	/* Compile */
	{uf_editform,	AB_FormEdit,	_FormEditExpl,	0},	/* FormEdit */
	{uf_print,	FE_Print,	_PrintExpl,	0},	/* Print */
	uf_display, uf_save, uf_destroy, iiabVerifyType
};
GLOBALDEF READONLY COMP_CLASS_INFO iiABupdFrame = {
	OC_UPDFRAME,
	ERx("abfuserd"), ERx("abfuserd.hlp"), ERx("abfusemi.hlp"), 2,
	F_AB0098_UPDATE,
	F_AB00C3_VUFDet_title,
	-1,	NULL,	/* all types legal */
	{_formname,	NULL},	_return_type,	_srcfile, NULL, NULL,	/* Fields */
	/*{iiabNxtEdit,	AB_NewEdit,	_NewEditExpl,	0},	/* NextEdt */
	{iiabTstComp,	FE_Go,		_GoExpl,	4},    /* Go, FRSkey4 */
	{iiabSrEdit,	FE_Edit,	_EditExpl,	0},	/* Edit */
	{iiabSrCompile,	FE_Compile,	_CompileExpl,	0},	/* Compile */
	{uf_editform,	AB_FormEdit,	_FormEditExpl,	0},	/* FormEdit */
	{uf_print,	FE_Print,	_PrintExpl,	0},	/* Print */
	uf_display, uf_save, uf_destroy, iiabVerifyType
};
GLOBALDEF READONLY COMP_CLASS_INFO iiABbrwFrame = {
	OC_BRWFRAME,
	ERx("abfuserd"), ERx("abfuserd.hlp"), ERx("abfusemi.hlp"), 2,
	F_AB0095_BROWSE,
	F_AB00C0_VBFDet_title,
	-1,	NULL,	/* all types legal */
	{_formname,	NULL},	_return_type,	_srcfile, NULL,	 NULL, /* Fields */
	/*{iiabNxtEdit,	AB_NewEdit,	_NewEditExpl,	0},	/* NextEdt */
	{iiabTstComp,	FE_Go,		_GoExpl,	4},    /* Go, FRSkey4 */
	{iiabSrEdit,	FE_Edit,	_EditExpl,	0},	/* Edit */
	{iiabSrCompile,	FE_Compile,	_CompileExpl,	0},	/* Compile */
	{uf_editform,	AB_FormEdit,	_FormEditExpl,	0},	/* FormEdit */
	{uf_print,	FE_Print,	_PrintExpl,	0},	/* Print */
	uf_display, uf_save, uf_destroy, iiabVerifyType
};
GLOBALDEF READONLY COMP_CLASS_INFO iiABuserFrame = {
	OC_OSLFRAME,
	ERx("abfuserd"), ERx("abfuserd.hlp"), ERx("abfusemi.hlp"), 2,
	F_AB0094_USER,
	F_AB0040_AUFDet_title,
	-1,	NULL,	/* all types legal */
	{_formname,	NULL},	_return_type,	_srcfile, NULL,	 NULL, /* Fields */
	/*{iiabNxtEdit,	AB_NewEdit,	_NewEditExpl,	0},	/* NextEdt */
	{iiabTstComp,	FE_Go,		_GoExpl,	4},    /* Go, FRSkey4 */
	{iiabSrEdit,	FE_Edit,	_EditExpl,	0},	/* Edit */
	{iiabSrCompile,	FE_Compile,	_CompileExpl,	0},	/* Compile */
	{uf_editform,	AB_FormEdit,	_FormEditExpl,	0},	/* FormEdit */
	{uf_print,	FE_Print,	_PrintExpl,	0},	/* Print */
	uf_display, uf_save, uf_destroy, iiabVerifyType
};
/*
** Name:	uf_display() -	Display a User Frame Component Definition.
**
** Description:
**	Displays the attributes of a user frame component on its component
**	definition form.
**
** Inputs:
**	frm	{USER_FRAME *}  The user frame component object.
**
** History:
**	02/89 (jhw)  Written.
**	02/91 (Mike S)  Add menu-style processing. Currently only for menu 
**			frames
*/
static VOID
  uf_display( frm )
  register USER_FRAME *frm;
  {
    char *formname = iiABuserFrame.form;
    i4 notnullable;
    char title[63+1];
	COMP_CLASS_INFO        *cp;
	GLOBALREF char	*IIabExename;
	cp = iiabApCompClass(frm->class, FALSE);
	IIUGfmt(title,sizeof(title)-1,ERget(cp->title),1,IIabExename);
/* # line 214 "abufedit.qsc" */	/* putform */
    {
      if (IIfsetio(formname) != 0) {
        IIputfldio(_formname,(short *)0,1,32,0,frm->form->name);
        IIputfldio(_srcfile,(short *)0,1,32,0,frm->source);
        IIputfldio(_return_type,(short *)0,1,32,0,frm->return_type.db_data);
        IIputfldio(_nullable,(short *)0,1,32,0,
        ERget(
				    AFE_NULLABLE(frm->return_type.db_datatype)
					? F_UG0002_Yes2 : F_UG0007_No2 ));
        IIputfldio(_fstatic,(short *)0,1,32,0,
        ERget(frm->is_static 
					? F_UG0002_Yes2 : F_UG0007_No2));
        IIputfldio(_title,(short *)0,1,32,0,title);
      } /* IIfsetio */
    }
/* # line 224 "abufedit.qsc" */	/* host code */
	/* Turn off 'nullable' field for return type "none." */
	notnullable = ( frm->return_type.db_datatype == DB_NODT );
/* # line 226 "abufedit.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(54,_nullable,0,(short *)0,1,30,4,&notnullable);
      } /* IIiqset */
    }
/* # line 228 "abufedit.qsc" */	/* host code */
        /*
        ** Make the "compile form" field invisible if the application uses
        ** only compiled forms; otherwise set it from the frame definition.
        */
        if ((frm->appl->flags & APC_DBFORMS) == 0)
        {
/* # line 234 "abufedit.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(54,_compform,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 235 "abufedit.qsc" */	/* host code */
        }
        else
        {
/* # line 238 "abufedit.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(54,_compform,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 239 "abufedit.qsc" */	/* putform */
    {
      if (IIfsetio(formname) != 0) {
        IIputfldio(_compform,(short *)0,1,32,0,
        ERget((frm->flags & APC_DBFORM) ?
                                                F_UG0007_No2 : F_UG0002_Yes2));
      } /* IIfsetio */
    }
/* # line 242 "abufedit.qsc" */	/* host code */
        }
	/* If we're in Vision and this is a Menu Frame, display menu style */
	if (IIabVision && frm->class == OC_MUFRAME)
	{
    char *style = IIFGmsnMenuStyleName(frm->flags & APC_MS_MASK);
/* # line 249 "abufedit.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(54,_MenuStyle,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 250 "abufedit.qsc" */	/* putform */
    {
      if (IIfsetio(formname) != 0) {
        IIputfldio(_MenuStyle,(short *)0,1,32,0,style);
      } /* IIfsetio */
    }
/* # line 251 "abufedit.qsc" */	/* host code */
	}
	else
	{
/* # line 254 "abufedit.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(54,_MenuStyle,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 255 "abufedit.qsc" */	/* host code */
	}
  }
/*
** Name:	uf_editform() -	Edit User Frame Form from Form.
**
** Description:
**	Edits the form for a user frame component object as
**	specified on the component definition form.
**
** Inputs:
**	app	{APPL *}  The application object.
**	class	{COMP_CLASS_INFO *}  The edit class information.
**	frm	{USER_FRAME *}  The user frame component.
**
** Returns:
**	{STATUS}  OK, if no errors.
**
** History:
**	03/89 (jhw)  Written.
**	17-nov-1993 (connie) Bug #43892
**		Set the formgendate & state info if the VISION forms are
**		created using the ABF vifred
*/
static STATUS
  uf_editform( app, class, frm )
APPL			*app;
  COMP_CLASS_INFO *class;
USER_FRAME		*frm;
  {
	STATUS		rval;
	FORM_REF	*form_ref = NULL;
    char form[FE_MAXNAME+1];
/* # line 290 "abufedit.qsc" */	/* getform */
    {
      if (IIfsetio(class->form) != 0) {
        IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,form,_formname);
      } /* IIfsetio */
    }
/* # line 292 "abufedit.qsc" */	/* host code */
	if ( STtrmwhite(form) > 0 )
	{
		if ( !STequal(form, frm->form->name) 
		   && uf_save(app, frm) != OK )
		{
			return FAIL;
		}
		form_ref = frm->form;
	}
	if ((rval = iiabFormEdit(app, form_ref, FALSE )) != OK) return(rval);
	if (frm->mf != (METAFRAME *)NULL &&
		(frm->mf->formgendate==NULL || *(frm->mf->formgendate)==EOS))
	{ /* set these info only if using ABF vifred to CREATE vision form */
		frm->mf->formgendate = STtalloc(frm->data.tag,UGcat_now());
		frm->mf->state &= ~MFST_DOFORM;
		IIAMufqUserFrameQuick(frm);
	}
	return OK;
  }
/*
** Name:	uf_save() -	Save User Frame Component Definition from
**					the Form.
** Description:
**	Saves the attributes for a user frame component object from the
**	component definition form for this class.  This checks if any values
**	changed before modifying the object's attributes and only then writing
**	the object to the database.
**
**	The values that can be changed are the source-code file name, the form
**	name, and the return type.
**
** Input:
**	app	{APPL *}  The application object.
**	frm	{USER_FRAME *}  The user frame component object.
**
** Returns:
**	{STATUS}  OK, if no errors.
**
** History:
**	02/89 (jhw)  Written.
**	04/90 (Mike S)
**		If the source file name or formname is changed, set the
**		corresponding gen date to an ancient value to avoid
**		overwriting the new object.
**	05/90 (jhw)  Added long remark check for save.
**	08/90 (Mike S)  Invalidate frame if anything affecting compile changes
**	20-sep-93 (essi)
**		When editing frames and changing form names, if the old form
**		is used by multiple frames, we may need to create a new form 
**		reference (instead of changing it --bug 48247), but before 
**		creating a new one we need to check to make sure whether one 
**		is already available. If so, just change its attributes.
**	08-dec-93 (connie) Bug #50142
**		If there is change in short_remark from Vision, update the
**		frame's flag for regeneration.
**	4-oct-94 (mgw) Bug #65356 & #63046
**		Don't update the frame's flag for regeneration in the
**		short_remark case unless that flag exists.
*/
static STATUS
  uf_save( app, frm )
APPL			*app;
register USER_FRAME	*frm;
  {
    char *formname = iiABuserFrame.form;
    i4 form_change;
    i4 src_change;
    i4 srem_change;
    i4 stat_change;
    i4 compform_change;
    i4 menustyle_change;
	FORM_REF	*IIAMfrChangeRef();
	FORM_REF        *IIAMformRef();
	STATUS		IIAMwrUserFrame();
	METAFRAME *mf = frm->mf;
/* # line 373 "abufedit.qsc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&form_change,20,_formname,0);
        IIiqfsio((short *)0,1,30,4,&src_change,20,_srcfile,0);
        IIiqfsio((short *)0,1,30,4,&stat_change,20,_fstatic,0);
        IIiqfsio((short *)0,1,30,4,&srem_change,20,_short_remark,0);
        IIiqfsio((short *)0,1,30,4,&compform_change,20,_compform,0);
        IIiqfsio((short *)0,1,30,4,&menustyle_change,20,_MenuStyle,0);
      } /* IIiqset */
    }
/* # line 382 "abufedit.qsc" */	/* host code */
	/* Fields should have been validated already. */
	if ( src_change == 1 )
    {
      char srcfile[48+1];
/* # line 388 "abufedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,48+1-1,srcfile,_srcfile);
        } /* IIfsetio */
      }
/* # line 389 "abufedit.qsc" */	/* host code */
		if ( !STequal(frm->source, srcfile) )
		{
			iiabNewFileName(frm->source, srcfile);
			frm->source = FEtsalloc( frm->data.tag, srcfile );
			if (mf != (METAFRAME *)NULL)
				mf->gendate = basedate;
			if ((frm->flags & APC_RECOMPILE) == 0)
				frm->flags |= (APC_RECOMPILE|APC_DEFCHANGE);
			frm->data.dirty = TRUE;
		}
    }
	if ( form_change == 1 )
    {
      char form[FE_MAXNAME+1];
		bool	newref;
/* # line 406 "abufedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,form,_formname);
        } /* IIfsetio */
      }
/* # line 407 "abufedit.qsc" */	/* host code */
		/* Form name is a required DB reference */
		if ( !STequal(frm->form->name, form) )
		{
			/*
			** If the number of references to `current' form
			** (ie, frm->form->name) is more than one, then we
			** need to create a new form reference, but only if
			** the new form does not exist in other frames.
			** (bug 48247)
			*/
			newref = FALSE;
			_VOID_ IIAMcrCountRef(app, frm);
			if ( frm->form->refs > 1 )
			{
				frm->form = IIAMformRef(app,form);
				frm->form->refs++;
				newref = TRUE;
			}
			iiabFrRemoveNames(frm->form);
			if ( !newref )
				frm->form = IIAMfrChangeRef(app,frm->form,form);
			/* If it's a new FORM_REF, generate new names */
			if (frm->form->refs == 1)
				iiabFoRefNames(frm->form);
			if (mf != (METAFRAME *)NULL)
			{
				mf->formgendate = basedate;
				mf->state |= MFST_DOFORM;
			}
			if ((frm->flags & APC_RECOMPILE) == 0)
				frm->flags |= (APC_RECOMPILE|APC_DEFCHANGE);
			frm->data.dirty = TRUE;
		}
    }
	if ( srem_change == 1 )
    {
		char	*cp;
      char short_remark[OOSHORTREMSIZE+1];
/* # line 448 "abufedit.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
          IIstfsio(20,_short_remark,0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
        } /* IIiqset */
      }
/* # line 449 "abufedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,OOSHORTREMSIZE+1-1,short_remark,
          _short_remark);
        } /* IIfsetio */
      }
/* # line 450 "abufedit.qsc" */	/* host code */
		if ( ! STequal(frm->short_remark, short_remark)
		    && (cp = FEtsalloc(frm->data.tag, short_remark)) != NULL )
		{
			frm->short_remark = cp;
			if (IIabVision && frm->mf != (METAFRAME *) NULL)
				frm->mf->state |= MFST_DOGEN|MFST_DOFORM;
			frm->data.dirty = TRUE;
		}
    }
	if ( stat_change == 1 )
    {
      char statbuf[16];
/* ##              set_frs field formname ( change(_fstatic) = 0 ); */
/* # line 465 "abufedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,15,statbuf,_fstatic);
        } /* IIfsetio */
      }
/* # line 467 "abufedit.qsc" */	/* host code */
		/* Note:  The field must be validated "yes" or "no". */
		frm->is_static = IIUGyn(statbuf, (STATUS *)NULL);
		if ((frm->flags & APC_RECOMPILE) == 0)
			frm->flags |= (APC_RECOMPILE|APC_DEFCHANGE);
		frm->data.dirty = TRUE;
    }
        if (compform_change != 0)
        { /* Whether to compile the form */
                bool            is_compform;
                bool            was_compform = ((frm->flags & APC_DBFORM) == 0);
    char compform[10];
/* # line 480 "abufedit.qsc" */	/* getform */
    {
      if (IIfsetio(formname) != 0) {
        IIgetfldio((short *)0,1,32,9,compform,_compform);
      } /* IIfsetio */
    }
/* # line 481 "abufedit.qsc" */	/* host code */
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
        if (IIabVision && frm->class == OC_MUFRAME && menustyle_change != 0)
        { /* Which style of menu to use */
                i4         oldmask;
                i4         newmask;
    char menustyle[20];
/* # line 500 "abufedit.qsc" */	/* getform */
    {
      if (IIfsetio(formname) != 0) {
        IIgetfldio((short *)0,1,32,19,menustyle,_MenuStyle);
      } /* IIfsetio */
    }
/* # line 501 "abufedit.qsc" */	/* host code */
		oldmask = frm->flags & APC_MS_MASK;
		newmask = IIFGmsmMenuStyleMask(menustyle);
		if (oldmask != newmask)
		{
			frm->flags &= ~APC_MS_MASK;
			frm->flags |= newmask;
			frm->mf->state |= MFST_DOGEN|MFST_DOFORM;
                        frm->data.dirty = TRUE;
		}
        }
	if ( iiabGetRetType( iiABuserFrame.form, frm->data.tag,
				&frm->return_type ) )
	{
		/* Invalidate our callers */
		IIAMxdsDepSearch((APPL_COMP *)frm, IIAM_zccCompChange, 
				 (PTR)NULL);
		if ((frm->flags & APC_RECOMPILE) == 0)
			frm->flags |= (APC_RECOMPILE|APC_DEFCHANGE);
		frm->data.dirty = TRUE;
	}
	_VOID_ IIOOckLongRemark((OO_OBJECT *)frm);
	if ( frm->data.dirty )
		return iiabSvComp((APPL_COMP *)frm, IIAMwrUserFrame);
	else if ( frm->form->data.dirty )
		return IIAMwrFormRef(frm->form);
	return OK;
  }
/*
** Name:	uf_destroy() -	Destroy a User Frame Component Object.
**
** Description:
**	ABF specific destroy simply removes names for the object.
**
** Input:
**	frm	{USER_FRAME *}  The user frame component.
**
** History:
**	02/89 (jhw)  Written.
**	01/90 (jhw)  Removed 'frm->app' parameter to 'IIAMfrRemoveRef()',
**		since it is NULL by the time this routine is called.  #9533.
*/
static VOID
uf_destroy ( frm )
USER_FRAME	*frm;
{
	iiabRmObjLoc(frm->source);	/* remove object files */
	iiabNewFileName(frm->source, (char *)NULL);	/* clear object name */
	IIGNdnDeleteName(AGN_OFRAME, frm->symbol);
	iiabFrRemoveNames(frm->form);
	IIAMfrRemoveRef(frm->form);
}
static STATUS
uf_print ( appl, frm )
APPL		*appl;
USER_FRAME	*frm;
{
	return iiabSfilePrint(appl, frm->source);
}
