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
#include	<oosymbol.h>
# include <oocat.h>
# include <abclass.h>
#include        <dmchecks.h>
# include "abclinfo.h"
#include	"abfglobs.h"
#include	"gncode.h"
#include	"erab.h"
/**
** Name:	abgledit.qc -	ABF Global Variable Component Edit Module.
**
** Description:
**	Contains the definition of the component class edit information
**	structure for the application variable component objects.  Defines:
**
**	iiABglobals	ABF global variable component class edit information.
**
** History:
**	Revision 6.3/03/00  89/06  billc
**	Initial revision.
**
**	02-dec-91 (leighb) DeskTop Porting Change:
**		Added routines to pass data across ABF/IAOM boundary.
** 	6-Feb-92 (blaise)
**		Added include abfglobs.h.
**	31-jul-92 (blaise)
**		Added DB_DEC_TYPE to list of valid datatypes.
**	16-nov-92 (donc)
**		Modified COMP_CLASS_INFO initialization for bug 44162 fix.
**	15-nov-93 (robf)
**              Add DB_SEC_TYPE to list of valid datatypes.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	11-sep-2006 (gupsh01)
**	    Added new ANSI date/time types as valid legal types.
**/
STATUS	iiabPrint();
bool iiabTG_TypeGet();
static VOID	_gl_display();
static STATUS	_gl_save();
static VOID	_type_help();
static STATUS	_gl_verifyType();
static READONLY
	DB_DT_ID	_types[] = { DB_INT_TYPE, DB_FLT_TYPE,
					DB_CHR_TYPE, DB_TXT_TYPE, DB_CHA_TYPE,
					DB_VCH_TYPE, DB_DTE_TYPE, DB_MNY_TYPE,
					DB_DEC_TYPE, DB_ADTE_TYPE, DB_TMWO_TYPE,
					DB_TMW_TYPE, DB_TME_TYPE, DB_TSW_TYPE, 
					DB_TSWO_TYPE, DB_TSW_TYPE, DB_TSTMP_TYPE,
					DB_INDS_TYPE, DB_INYM_TYPE
};
/*:
** Name:	iiABglobals -	ABF global variable Class Edit Information.
**
** Description:
**	Defines additional class information for the global var. component
**	objects for use in editing.
*/
#define _NewEditExpl	F_AB0052_NewEdit_expl
#define _EditExpl	F_AB0055_Edit4GL_expl
#define _PrintExpl	F_AB0380_ObjPrint_expl
#define _EmptyExpl 	F_AB005E_Empty_expl
GLOBALDEF READONLY COMP_CLASS_INFO iiABglobals = {
	OC_GLOBAL,
	ERx("abfpartcreate"), ERx("abfvars.hlp"), ERx(""), 2,
	F_AB0069_AGLDef_title,
	ER_OK,
	sizeof(_types)/sizeof(_types[0]), _types,	/* legal types */
	{NULL,	NULL},	_data_type,	NULL, NULL, NULL, 	/* Fields */
	/*{iiabNxtEdit,	AB_NewEdit,	_NewEditExpl,	0}, 	/* NextEdit */
	{NULL,		NULL,		_EmptyExpl,	0}, 	/* Go */
	{NULL,		NULL,		_EmptyExpl,	0},	/* Edit */
	{NULL,		NULL,		_EmptyExpl,	0}, 	/* Compile */
	{NULL,		NULL,		_EmptyExpl,	0}, 	/* FormEdit */
	{iiabPrint,	FE_Print,	_PrintExpl,	0},	/* Print */
	_gl_display, _gl_save, NULL, _gl_verifyType
};
/*
** Name:	_gl_display() -	Display a Global Variable Component Definition.
**
** Description:
**	Displays the attributes of a app. variable component on its component
**	definition form.
**
** Inputs:
**	gvar	{GLOBVAR *}  The application global variable component.
**
** History:
**	06/89 (billc)  Written.
*/
static VOID
  _gl_display( gvar )
  register GLOBVAR *gvar;
  {
    char *formname = iiABglobals.form;
    DB_DATA_DESC *dbdv = &gvar->data_type;
    char title[63+1];
	GLOBALREF char	*IIabExename;
	IIUGfmt(title,sizeof(title)-1,ERget(iiABglobals.title), 1, IIabExename);
/* # line 135 "abgledit.qsc" */	/* putform */
    {
      if (IIfsetio(formname) != 0) {
        IIputfldio(_title,(short *)0,1,32,0,title);
        IIputfldio((char *)"name",(short *)0,1,32,0,gvar->name);
      } /* IIfsetio */
    }
/* # line 140 "abgledit.qsc" */	/* host code */
	iiabTD_TypeDisplay(formname, dbdv, (i4) gvar->appl->dml);
        /*
	** When editing, user can't twiddle the objects' name.  Also, we
	** may have set some fields invisible (we use this form for creation
	** as well as editing) so set them visible, just in case.
	*/
/* # line 147 "abgledit.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(53,(char *)"name",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(17,(char *)"name",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
        IIstfsio(54,_owner,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(54,_create_date,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0
        ));
        IIstfsio(54,_altr_date,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(54,_last_altered_by,0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
  }
/* # line 158 "abgledit.qsc" */	/* host code */
/*
** Name:	_gl_save() -	Save Global Variable Component Definition from
**					the Form.
** Description:
**	Saves the attributes for a global variable component from the definition
**	form for this class.  This checks if any values changed before modifying
**	the component's attributes and only then writing it to the database.
**
** Input:
**	app	{APPL *}  The application object.
**	obj	{GLOBVAR *}  The global var. component object.
**
** Returns:
**	{STATUS}  OK, if no errors.
**
** History:
**	07/89 (billc)  Written.
**	05/90 (jhw)  Added long remark check for save.
*/
static STATUS
  _gl_save( app, obj )
APPL			*app;
register APPL_COMP	*obj;
  {
    DB_DATA_DESC *dbdv;
    i4 sremark_change;
    char *formname;
	STATUS		IIAMwrGlobal();
	dbdv = &((GLOBVAR *) obj)->data_type;
	formname = iiABglobals.form;
/* # line 191 "abgledit.qsc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&sremark_change,20,_short_remark,0);
      } /* IIiqset */
    }
/* # line 195 "abgledit.qsc" */	/* host code */
	if ( iiabTG_TypeGet( obj, formname, dbdv ) )
	{
                /* Invalidate our callers */
                IIAMxdsDepSearch((APPL_COMP *)obj, IIAM_zccCompChange,
                                 (PTR)NULL);
		obj->data.dirty = TRUE;
	}
	if ( sremark_change != 0 )
    {
		char	*cp;
      char short_remark[OOSHORTREMSIZE+1];
/* # line 209 "abgledit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,OOSHORTREMSIZE+1-1,short_remark,
          _short_remark);
        } /* IIfsetio */
      }
/* # line 210 "abgledit.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
          IIstfsio(20,_short_remark,0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
        } /* IIiqset */
      }
/* # line 212 "abgledit.qsc" */	/* host code */
		if ( ! STequal(obj->short_remark, short_remark)
		  && (cp = FEtsalloc(obj->data.tag, short_remark)) != NULL )
		{
			obj->short_remark = cp;
			obj->data.dirty = TRUE;
		}
    }
	_VOID_ IIOOckLongRemark((OO_OBJECT *)obj);
	if ( obj->data.dirty )
	{
		return iiabSvComp(obj, IIAMwrGlobal);
	}
	return OK;
  }
/*{
** Name:	_gl_verifyType() -	Verify Data Type on Form.
**
** Description:
**	Reads the data type specifiction (type only) from the form and
**	verifies that it's legit.
**
** Inputs:
**	comp		{APPL_COMP *}  The object checking the type.
**	desc		{char *}  	A string describing the type.
**
** Outputs:
**	dbdv		(DB_DATA_TYPE*} A dbv to represent the description.
**
** Returns:
**	{STATUS}  OK if datatype is valid, FAIL otherwise.
**
** Side Effects:
**	Turns the 'nullable' field invisible if the datatype is a record,
**	turns the 'array' field invisible if the datatype is an ingres type. 
**
** History:
**	08/89 (billc)  Written.
**	11/89 (jhw)  Modified to check for legal global types.
*/
static STATUS
_gl_verifyType ( comp, desc, dbdv )
  APPL_COMP *comp;
char		*desc;
DB_DATA_VALUE	*dbdv;
  {
    char *formname = iiABglobals.form;
	/* if we got a component, allow record definitions here. */
	if (comp != NULL
	  && OOsnd( OC_RECORD, _withName, desc, (char*)NULL, 
	       comp->appl->ooid 
	    ) != nil )
	{
		dbdv->db_datatype = DB_DMY_TYPE;
		dbdv->db_length = dbdv->db_prec = 0;
	}
	/* Note: 'Nullability' here is not the "true" value, but the default
	** to be used when the type field is toggled between a complex type and
	** an INGRES type.
	*/
	else if ( iiabCkTypeSpec(desc, (bool)(comp->appl->dml == DB_SQL), dbdv)
				!= OK 
			|| dbdv->db_datatype == DB_NODT
				|| dbdv->db_datatype == DB_DYC_TYPE )
	{ /* invalid data type */
		iiabTypeError(desc, E_AB0269_Data_type);
		return FAIL;
	}
	/* The dbdv started as stack junk -- we can twiddle all we like. */
	dbdv->db_data = (PTR) desc;
	iiabTD_TypeDisplay( formname, (DB_DATA_DESC *)dbdv, DB_NODT );
/* # line 287 "abgledit.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(20,_data_type,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 289 "abgledit.qsc" */	/* host code */
	return OK;
  }
