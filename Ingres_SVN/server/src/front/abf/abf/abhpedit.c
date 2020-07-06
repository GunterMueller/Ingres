# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<ol.h>
#include	<st.h>
#include	<er.h>
#include	<tm.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
# include <abclass.h>
# include <oocat.h>
#include        <dmchecks.h>
# include "abclinfo.h"
#include	"abfglobs.h"
#include	"erab.h"
GLOBALREF char	*IIutExtensions[_OLMAX+1][3];
#define IIUTextension(dml, ol_type) \
	((ol_type) < OLC || (ol_type) > _OLMAX || ((dml) < DB_NOLANG && (dml) > DB_SQL)\
		? NULL : IIutExtensions[ol_type][dml])
/**
** Name:	abhpedit.qc - ABF Host-Language Procedure Component Edit Module.
**
** Description:
**	Contains the definition of the component class edit information
**	structure for the application host-language procedure component
**	objects.  Defines:
**
**	iiABHLProc	ABF host-language procedure component class information.
**
** History:
**	Revision 6.2  89/02  wong
**	Initial revision.
**
**	02-dec-91 (leighb) DeskTop Porting Change:
**		Added routines to pass data across ABF/IAOM boundary.
**	03-feb-1992 (davel)
**		fixed bug 39920 in _display(): set displayonly and underline
**		attributes of srcfile when not a library proc.
**	6-Feb-92 (blaise)
**		Added include abfglobs.h.
**
**	Revision 6.5
**
**	6-aug-92 (blaise)
**		Added DB_DEC_TYPE to list of valid types.
**	11-Nov-92 (DonC)
**		Changes to COMP_CLASS_INFO initialization to accomodate fix
**		for bug 44162
**	4-dec-92 (davel)
**		Add support for "passing decimal as what type" field.
**	19-jan-93 (davel)
**		Removed DB_DEC_TYPE from list of valid types.
**	18-nov-93 (robf)
**              Added DB_SEC_TYPE to list  of valid type.
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
static STATUS	_edit();
static STATUS	_compile();
static STATUS	_print();
static VOID	_display();
static STATUS	_save();
static VOID	_destroy();
static STATUS	_type_valid();
static READONLY
	DB_DT_ID	_types[] = { DB_NODT, DB_DYC_TYPE, DB_INT_TYPE,
					DB_FLT_TYPE, DB_CHR_TYPE,
					DB_TXT_TYPE, DB_CHA_TYPE, DB_VCH_TYPE
};
/*:
** Name:	iiABHLProc -	ABF Host-Language Procedure Component Class
**					Edit Information.
** Description:
**	Defines additional class information for the host-language procedure
**	component objects for editing.
*/
#define _NewEditExpl	F_AB0052_NewEdit_expl
#define _GoExpl		F_AB0054_GoProc_expl
#define _EditExpl	F_AB0058_Edit_expl
#define _CompileExpl	F_AB005C_Compile_expl
#define _EmptyExpl	F_AB005E_Empty_expl
#define _PrintExpl	F_AB005F_Print_expl
GLOBALDEF READONLY COMP_CLASS_INFO iiABHLProc = {
	OC_HLPROC,
	ERx("abfprocd"), ERx("abhlproc.hlp"), ERx("abhlprmi.hlp"), 1,
	ER_OK,
	ER_OK,
	sizeof(_types)/sizeof(_types[0]), _types,	/* legal types */
	{NULL,	NULL},	_return_type,	_srcfile,	_library, _symbol, /* Fields */
	/*{iiabNxtEdit,	AB_NewEdit,	_NewEditExpl,	0},	/* NextEdit */
	{iiabTstComp,	FE_Go,		_GoExpl,	4},    /* Go, FRSkey4 */
	{_edit,		FE_Edit,	_EditExpl,	0},	/* Edit */
	{_compile,	FE_Compile,	_CompileExpl,	0},	/* Compile */
	{NULL,		0,		_EmptyExpl,	0},	/* FormEdit */
	{_print,	FE_Print,	_PrintExpl,	0},	/* Print */
	_display, _save, _destroy, _type_valid
};
static READONLY ER_MSGID
	_titles[] = {
			F_AB0062_ACPDef_title,	/* C */	
			F_AB0063_AFPDef_title,	/* FORTRAN */	
			F_AB0064_APPDef_title,	/* Pascal */	
			F_AB0066_ABPDef_title,	/* BASIC */	
			F_AB0065_AcPDef_title,	/* COBOL */	
			F_AB0068_A1PDef_title,	/* PL/1 */	
			F_AB0067_AAPDef_title,	/* Ada */	
			0
};
static VOID
  _display( proc )
  register HLPROC *proc;
  {
	char	*ext;
    char *formname = iiABHLProc.form;
    i4 library_only;
    i4 notnullable;
    i4 warning;
    i4 passdec;
    char *passdec_val;
    char title[63+1];
	GLOBALREF char	*IIabExename;
	/* Turn on 'symbol', 'srcfile' and 'library', which may also have
	** been display-only for DB procedures.  Make 'library' display-only
	** if compilable extensions are not recognized for the language type.
	** Also, turn off 'nullable' field for return type "none," and then
	** make it display-only since it can never be set for host-language
	** procedures.  Turn on "pass decimal as what type" field for
	** languages that support decimal.  Finally, toggle 'warning' 
	** depending on whether the name conflicts with a system name.
	*/
	ext = IIUTextension(DB_NOLANG, proc->ol_lang);
	library_only = ( ext == NULL || *ext == EOS );
	notnullable = ( proc->return_type.db_datatype == DB_NODT );
	warning = !IIAFckFuncName(proc->name);
	passdec = (OLang_info(proc->ol_lang) & OL_I_HAS_DECIMAL) ? 0 : 1;
/* # line 163 "abhpedit.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(54,_symbol,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(54,_srcfile,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(54,_library,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(53,_library,0,(short *)0,1,30,4,&library_only);
        IIstfsio(17,_library,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(54,_nullable,0,(short *)0,1,30,4,&notnullable);
        IIstfsio(53,_nullable,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(17,_nullable,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(54,_passdec,0,(short *)0,1,30,4,&passdec);
        IIstfsio(54,_warning,0,(short *)0,1,30,4,&warning);
      } /* IIiqset */
    }
/* # line 176 "abhpedit.qsc" */	/* host code */
	IIUGfmt(title,sizeof(title)-1,ERget(_titles[proc->ol_lang]),1,IIabExename);
	passdec_val = (proc->flags & APC_PASSDEC) 
				? ERx("decimal") : ERx("float");
/* # line 181 "abhpedit.qsc" */	/* putform */
    {
      if (IIfsetio(formname) != 0) {
        IIputfldio(_title,(short *)0,1,32,0,title);
        IIputfldio(_language,(short *)0,1,32,0,ERx(IIOLanguage(proc->ol_lang)));
        IIputfldio(_srcfile,(short *)0,1,32,0,proc->source);
        IIputfldio(_symbol,(short *)0,1,32,0,proc->symbol);
        IIputfldio(_library,(short *)0,1,32,0,
        ERget( *proc->source == EOS
						? F_UG0002_Yes2 : F_UG0007_No2
			));
        IIputfldio(_passdec,(short *)0,1,32,0,passdec_val);
        IIputfldio(_return_type,(short *)0,1,32,0,proc->return_type.db_data);
        IIputfldio(_nullable,(short *)0,1,32,0,ERget( F_UG0007_No2 ));
      } /* IIfsetio */
    }
/* # line 194 "abhpedit.qsc" */	/* host code */
	if ( *proc->source == EOS )
	{ /* library procedure ... */
		/* ... set 'srcfile' display-only, value "<none>". */
/* # line 197 "abhpedit.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(53,_srcfile,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(17,_srcfile,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 201 "abhpedit.qsc" */	/* putform */
    {
      if (IIfsetio(formname) != 0) {
        IIputfldio(_srcfile,(short *)0,1,32,0,ERx("<none>"));
      } /* IIfsetio */
    }
/* # line 202 "abhpedit.qsc" */	/* host code */
	}
	else
	{ /* not a library procedure ... */
		/* ... turn off display-only and turn on underline */
/* # line 206 "abhpedit.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(53,_srcfile,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(17,_srcfile,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 210 "abhpedit.qsc" */	/* host code */
	}
  }
/*
** Name:	_save() -	Save Host-Language Procedure Component
**					Definition from the Form.
** Description:
**	Saves the attributes for a host-language procedure component object from
**	the component definition form for this class.  This checks if any values
**	changed before modifying the object's attributes and only then writing
**	the object to the database.
**
**	The values that can be changed are the symbol, return type, whether
**	decimal values are passed as decimal or float, whether it
**	is a library procedure, and if not the last, the source-code file.
**
** Input:
**	app	{APPL *}  The application object.
**	proc	{HLPROC *}  The host-language procedure component object.
**
** Returns:
**	{STATUS}  OK, if no errors.
**
** History:
**	02/89 (jhw)  Written.
**	05/90 (jhw)  Added long remark check for save.
**	04-dec-92 (davel)
**		added save of "pass decimal as what type" field.
*/
bool	iiabGetRetType();
static STATUS
  _save( app, proc )
APPL		*app;
  HLPROC *proc;
  {
    char *formname = iiABHLProc.form;
    char islib[10];
    i4 sym_change;
    i4 lib_change;
    i4 srem_change;
    i4 passdec_change;
	bool	islibrary;
	STATUS	IIAMwrHLProc();
/* # line 256 "abhpedit.qsc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&sym_change,20,_symbol,0);
        IIiqfsio((short *)0,1,30,4,&lib_change,20,_library,0);
        IIiqfsio((short *)0,1,30,4,&srem_change,20,_short_remark,0);
        IIiqfsio((short *)0,1,30,4,&passdec_change,20,_passdec,0);
      } /* IIiqset */
    }
/* # line 263 "abhpedit.qsc" */	/* host code */
	/* Fields should have been validated already. */
	if ( sym_change == 1 )
    {
      char symbol[FE_MAXNAME+1];
/* # line 269 "abhpedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,symbol,_symbol);
        } /* IIfsetio */
      }
/* # line 270 "abhpedit.qsc" */	/* host code */
		if ( !STequal(proc->symbol, symbol) )
		{
			proc->symbol = ( *symbol == EOS ) ? _
					: FEtsalloc( proc->data.tag, symbol );
			if ((proc->flags & APC_RECOMPILE) == 0)
				proc->flags |= (APC_RECOMPILE|APC_DEFCHANGE);
			proc->data.dirty = TRUE;
		}
    }
	if ( passdec_change == 1 )
    {
      char desc[FE_MAXNAME+1];
		DB_DATA_VALUE	temp;
/* # line 285 "abhpedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,desc,_passdec);
        } /* IIfsetio */
      }
/* # line 286 "abhpedit.qsc" */	/* host code */
		if ( STtrmwhite(desc) > 0 
		  && iiabCkTypeSpec(desc, FALSE, &temp) == OK
		   )
		{
			i4 flags = proc->flags;
			if (temp.db_datatype == DB_DEC_TYPE)
				proc->flags |= APC_PASSDEC;
			else
				proc->flags &= ~APC_PASSDEC;
			if (proc->flags != flags)
			{
				proc->data.dirty = TRUE;
			}
		}
    }
/* # line 304 "abhpedit.qsc" */	/* getform */
    {
      if (IIfsetio(formname) != 0) {
        IIgetfldio((short *)0,1,32,9,islib,_library);
      } /* IIfsetio */
    }
/* # line 305 "abhpedit.qsc" */	/* host code */
	islibrary = IIUGyn(islib, (STATUS *)NULL);
	if ( lib_change == 1 )
    {
		if ( islibrary != ( *proc->source == EOS ))
		{
			if ( islibrary )
			{
				/* clear object name */
				iiabNewFileName(proc->source, (char *)NULL);
				proc->source = _iiOOempty;
			}
			proc->data.dirty = TRUE;
		}
    }
	if ( *proc->source != EOS || ( lib_change == 1 && !islibrary ) )
    {
      i4 src_change;
/* # line 326 "abhpedit.qsc" */	/* inquire_frs */
      {
        if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&src_change,20,_srcfile,0);
        } /* IIiqset */
      }
/* # line 327 "abhpedit.qsc" */	/* host code */
		if ( src_change == 1 )
      {
        char source[48+1];
/* # line 331 "abhpedit.qsc" */	/* getform */
        {
          if (IIfsetio(formname) != 0) {
            IIgetfldio((short *)0,1,32,48+1-1,source,_srcfile);
          } /* IIfsetio */
        }
/* # line 332 "abhpedit.qsc" */	/* host code */
			if ( *proc->source == EOS ||
					!STequal(proc->source, source) )
			{
				iiabNewFileName(proc->source, source);
				proc->source =
					FEtsalloc( proc->data.tag, source );
				if ((proc->flags & APC_RECOMPILE) == 0)
					proc->flags |= 
						(APC_RECOMPILE|APC_DEFCHANGE);
				proc->data.dirty = TRUE;
			}
      }
    }
	if ( iiabGetRetType( iiABHLProc.form, proc->data.tag,
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
/* # line 363 "abhpedit.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
          IIstfsio(20,_short_remark,0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
        } /* IIiqset */
      }
/* # line 364 "abhpedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,OOSHORTREMSIZE+1-1,short_remark,
          _short_remark);
        } /* IIfsetio */
      }
/* # line 365 "abhpedit.qsc" */	/* host code */
		if ( ! STequal(proc->short_remark, short_remark)
			&& (cp = FEtsalloc(proc->data.tag, short_remark))
				!= NULL )
		{
			proc->short_remark = cp;
			proc->data.dirty = TRUE;
		}
    }
	_VOID_ IIOOckLongRemark((OO_OBJECT *)proc);
	if (proc->data.dirty && ( islibrary || lib_change )) 
	{
		/* 
		** Either this is a lib rtn or we changed from lib to non-lib.
		** Since we don't have source to compare to, there's no way
		** to check the dependency, so we better remove the 
		** executable to force relinking.
		*/
		iiabRiRmInterp();
	}
	return proc->data.dirty ? iiabSvComp((APPL_COMP *)proc, IIAMwrHLProc )
				: OK;
  }
static STATUS
  _edit( app, proc, form )
APPL		*app;
register HLPROC	*proc;
  char *form;
  {
    char islib[10];
/* # line 400 "abhpedit.qsc" */	/* getform */
    {
      if (IIfsetio(form) != 0) {
        IIgetfldio((short *)0,1,32,9,islib,_library);
      } /* IIfsetio */
    }
/* # line 401 "abhpedit.qsc" */	/* host code */
	if ( !IIUGyn(islib, (STATUS *)NULL) )
		return iiabSrEdit(app, proc, form);	/* has source */
	else
	{ /* a `library' procedure */
		IIUGerr(E_AB0012_NotLibOper, UG_ERR_ERROR, 1, ERget(FE_Edit));
		return FAIL;
	}
  }
static STATUS
  _compile( app, class, proc )
APPL			*app;
  COMP_CLASS_INFO *class;
register HLPROC		*proc;
  {
    char islib[10];
/* # line 418 "abhpedit.qsc" */	/* getform */
    {
      if (IIfsetio(class->form) != 0) {
        IIgetfldio((short *)0,1,32,9,islib,_library);
      } /* IIfsetio */
    }
/* # line 419 "abhpedit.qsc" */	/* host code */
	if ( !IIUGyn(islib, (STATUS *)NULL) )
		return iiabSrCompile( app, class, proc );	/* has source */
	else
	{ /* a `library' procedure */
		IIUGerr( E_AB0012_NotLibOper, UG_ERR_ERROR,
				1, ERget(FE_Compile)
		);
		return FAIL;
	}
  }
/*
** Name:	_destroy() -	Destroy a Host-Language Procedure Component Object.
**
** Description:
**	ABF specific destroy simply removes names for the object.
**
** Input:
**	proc	{HLPROC *}  The host-language procedure component.
**
** History:
**	02/89 (jhw)  Written.
*/
static VOID
_destroy ( proc )
HLPROC	*proc;
{
	if (*proc->source == EOS)
	{
		/* remove the interpreter to force a re-link. */
		iiabRiRmInterp();
	}
	else
	{
		/* remove obj files and timestamps */
		iiabRmObjLoc(proc->source);	
		/* clear object name */
		iiabNewFileName(proc->source, (char *)NULL);
	}
}
static STATUS
_print ( appl, proc )
APPL	*appl;
HLPROC	*proc;
{
	return iiabSfilePrint(appl, proc->source);
}
/*
** Name:	_type_valid() -	Validate Return Type for Database Procedure.
**
** Description:
**	Validates the entered data type for the return value of a database
**	procedure.
**
** Inputs:
**	comp	{APPL_COMP *}  The object checking the type.
**	desc	{char *}  A string describing the type.
**
** Output:
**	dbdv	{DB_DATA_VALUE *}  The data value description to fill in for
**			the type.
** Returns:
**	{STATUS}  OK, if the data type is valid for database procedures.
**
** History:
**	02/89 (jhw)  Written.
*/
static STATUS
_type_valid ( comp, desc, dbv )
  APPL_COMP *comp;
char			*desc;
register DB_DATA_VALUE	*dbv;
{
	STATUS iiabCkTypeSpec();
	if (iiabCkTypeSpec(desc, TRUE, dbv) == OK)
	{
		register i4  i;
		for ( i = 0 ; i < (sizeof(_types)/sizeof(_types[0])) ; ++i )
		{
			if ( _types[i] == dbv->db_datatype 
			   || - _types[i] == dbv->db_datatype )
			{
				return OK;
			}
		}
	}
	iiabTypeError(desc, E_AB036C_Hproc_type);
	return FAIL;
}
