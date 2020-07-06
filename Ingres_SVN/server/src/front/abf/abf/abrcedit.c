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
#include        <dmchecks.h>
# include "abclinfo.h"
#include	"gncode.h"
#include	"erab.h"
/**
** Name:	abrcedit.qc -	ABF Record Definition Component Edit Module.
**
** Description:
**	Contains the definition of the component class edit information
**	structure for the application record definition component objects.  
**	Defines:
**
**	iiABrecords	ABF record definition component class edit information.
**
** History:
**	Revision 6.3/03/00  89/06  billc
**	Initial revision.
**
**	16-Nov-92 (donc)
**	Modifications to COMP_CLASS_INFO initialization for 44162 bug fix.
**/
GLOBALREF char *IIabExename;
STATUS	iiabPrint();
static VOID	_rc_display();
static STATUS	_rc_save();
static VOID	_rc_destroy();
/*:
** Name:	iiABrecords -	ABF record definition Class Edit Information.
**
** Description:
**	Defines additional class information for the record definition component
**	objects for use in editing.
**
** NOTE: This struct is never used by IIABeditComp, which can't handle 
** 	catalog-style forms.  However, various parts of ABF access these 
**	structs, so it's wise policy to have them around.
*/
#define _EditExpl	F_AB0339_EditRec_expl
#define _PrintExpl	F_AB0380_ObjPrint_expl
#define _EmptyExpl 	F_AB005E_Empty_expl
GLOBALDEF READONLY COMP_CLASS_INFO iiABrecords = {
	OC_RECORD,
	ERx("abfrecd"), ERx("abedtcls.hlp"), ERx(""), 2,
	F_AB006A_AReDef_title,
	F_AB004E_AReDet_title,
	0,	NULL,	/* no types legal */
	{NULL,	NULL},	NULL,	NULL, NULL, NULL, 	/* Fields */
	{NULL,		ER_OK,		_EmptyExpl,	0}, 	/* Go */
	{NULL,		ER_OK,		_EmptyExpl,	0},	/* Edit */
	{NULL,		ER_OK,		_EmptyExpl,	0}, 	/* Compile */
	{NULL,		ER_OK,		_EmptyExpl,	0}, 	/* FormEdit */
	{iiabPrint,	FE_Print,	_PrintExpl,	0},	/* Print */
	_rc_display, _rc_save, _rc_destroy, NULL
};
/*
** Name:	_rc_display() -	Display a Record Type Component Definition
**
** Description:
**	Displays the attributes of a application class type component on its
**	definition form.
**
** Inputs:
**	rdef	{RECDEF *}  The record definition component.
**
** History:
**	06/89 (billc)  Written.
*/
static VOID
  _rc_display( rdef )
  register RECDEF *rdef;
  {
    char *formname = iiABrecords.form;
    char title[63+1];
	IIUGfmt(title,sizeof(title)-1,ERget(iiABrecords.title), 1, IIabExename);
/* # line 106 "abrcedit.qsc" */	/* putform */
    {
      if (IIfsetio(formname) != 0) {
        IIputfldio((char *)"_title",(short *)0,1,32,0,title);
        IIputfldio((char *)"name",(short *)0,1,32,0,rdef->name);
      } /* IIfsetio */
    }
/* # line 111 "abrcedit.qsc" */	/* host code */
	/*
	** When editing, user can't twiddle the objects' name.  Also, we
	** may have set some fields invisible (we use this form for creation
	** as well as editing) so set them visible, just in case.
	*/
/* # line 116 "abrcedit.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(53,(char *)"name",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(17,(char *)"name",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
        IIstfsio(54,(char *)"_owner",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
        IIstfsio(54,(char *)"_create_date",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
        IIstfsio(54,(char *)"_altr_date",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
        IIstfsio(54,(char *)"_last_altered_by",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
  }
/* # line 126 "abrcedit.qsc" */	/* host code */
/*
** Name:	_rc_save() -	Save Record Definition Component Definition from
**					the Form.
** Description:
**	Saves the attributes for a record definition component object from the
**	component definition form for this class.  This checks if any values
**	changed before modifying the object's attributes and only then writing
**	the object to the database.
**
** Input:
**	app	{APPL *}  The application object.
**	rdef	{RECDEF *}  The record definition component object.
**
** Returns:
**	{STATUS}  OK, if no errors.
**
** History:
**	07/89 (billc)  Written.
*/
static STATUS
  _rc_save( app, rdef )
APPL		*app;
register RECDEF	*rdef;
  {
	STATUS	IIAMwrRecord();
	if ( rdef->data.dirty )
	{
		return iiabSvComp((APPL_COMP *)rdef, IIAMwrRecord);
	}
	return OK;
  }
/*
** Name:	_rc_destroy() -	Destroy a Record Definition Component Object.
**
** Description:
**	ABF specific destroy simply removes names for the object.
**
** Input:
**	rdef	{RECDEF *}  The record definition component.
**
** History:
**	08/89 (billc)  Written.
*/
static VOID
_rc_destroy ( rdef )
RECDEF	*rdef;
{
	if (rdef->recmems != NULL)
	{
		IIAMccClearClass(rdef);
		rdef->recmems = NULL;
	}
}
