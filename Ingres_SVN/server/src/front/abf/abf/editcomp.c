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
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
#include	<uf.h>
#include	<adf.h>
# include <abclass.h>
# include <oocatlog.h>
#include	<oodefine.h>
# include <oocat.h>
# include <uigdata.h>
# include "abclinfo.h"
#include	"abcatrow.h"
#include	"erab.h"
/*
fool ming:
#include        <abclass.h>
#include        <oocatlog.h>
#include        <oocat.h>
#include        "abclinfo.h"
*/
/**
** Name:	editcomp.qc -	ABF Application Edit Component Object Frame.
**
** Description:
**	Contains the routine the implements the edit frames for the application
**	component objects.  Defines:
**
**	iiabGetComp()	return an application component by ID.
**	IIABedtComp()	ABF application edit component frame.
**
** History:
**	Revision 6.2  89/02  wong
**	Initial revision.
**
**	Revision 6.2  89/09  wong
**		Allow identical source files for different components.  
**		JupBug #8106.
**	11-march-91 (blaise)
**	    Integrated desktop porting changes.
**
**	Revision 6.5
**	30-nov-92 (davel)
**		Modified _type_check() to conditionally set the nullable field 
**		based on the component type (e.g. Global Constants don't have
**		have this field).
**		
**	19-Feb-1993 (fredv)
**	    Completed the bug fix for #44162. Added return OK to _symbol_check
**		if the symbol name is not changed or is a valid symbol.
**	11-feb-93 (davel)
**		Fix bug 49494: handle pass_dec field correctly when called from
**		VISION.
**	22-feb-93 (davel)
**		Fix bug 49635 & 49720: do not prompt for length on ListChoices 
**		on the datatype (49635); raise an error when illegal values
**		in "pass decimal parameters as" field are entered (49720).
**	25-feb-93 (connie) Bug #40884
**		Check for pathname in the source file field & give error
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**    25-Oct-2005 (hanje04)
**        Add prototypes for _getMenuitem(), _name_check(), _symbol_check(),
**	  _type_check(), _passdec_check(), _src_check() & _library_check() to
**	  prevent compiler errors with GCC 4.0 due to conflict with implicit
**	  declaration.
*/
APPL_COMP *
iiabGetComp ( appl, id )
APPL	*appl;
OOID	id;
{
	if ( IIAMapFetch( appl, id, TRUE ) != OK )
		return NULL;
	return (APPL_COMP *)OOp(id);
}
/*{
** Name:	IIABedtComp() - ABF Application Edit Component Object Frame.
**
** Description:
**	Allows users to edit ABF application objects through their respective
**	ABF Edit Object Definition Frames.  The variant frame information comes
**	from static structures that are mapped.
**
** Input:
**	appl	{APPL *}  The application object.
**	comp	{APPL_COMP *}  The application component object to be edited.
**	cat	{OO_CATALOG *}  The application component object visual catalog.
**
** History:
**	02/89 (jhw)  Written.
**	11/89 (jhw)  Added legal file name check.  JupBug #8627.
**	30-nov-1989 (wolf)
**		Accept CMS-format file ID "fn ft" and convert to "fn.ft".
**	03/90 (jhw) -- Ignore unsupported host-languages.  JupBug #20907.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**      16-Nov-1992 (donc)
**          Extraordinary effort to fix a simple bug 44162 - disallow blanks in symbol names.
**	19-Feb-1993 (fredv)
**	    Completed the bug fix for #44162. Added return OK to _symbol_check
**		if the symbol name is not changed or is a valid symbol.
*/
APPL_COMP	*iiabNxtEdit();
COMP_CLASS_INFO	*iiabApCompClass();
typedef struct {	/* Sub-Object Existence Structure */
	bool	_src;		/* source file existence */
	bool	_form;		/* form existence */
	bool	_report;	/* report existence */
	bool	_graph;		/* graph existence */
	bool	_table;		/* table existence */
	bool	_qdef;		/* query (join) definition existence */
} SOBJ_EXISTS;
#define _NewEditExpl	F_AB0052_NewEdit_expl
#define _RenameExpl	F_AB0018_RenameObj_expl
#define _HelpExpl	F_FE0100_ExplFrameHelp
#define _LongRemExpl	F_FE0118_LngRemExpl
#define _EndExpl	F_FE0102_ExplEnd
static char  *_getMenuitem();
static STATUS _name_check();
static STATUS _symbol_check();
static STATUS _type_check();
static STATUS _passdec_check();
static STATUS _src_check();
static STATUS _library_check();
static READONLY DB_DT_ID _passdec_types[] = { DB_DEC_TYPE, DB_FLT_TYPE }; 
static READONLY i4	_passdec_ntypes = 
			sizeof(_passdec_types)/sizeof(_passdec_types[0]);
VOID
IIABedtComp ( appl, comp, cat )
APPL		*appl;
  APPL_COMP *comp;
  OO_CATALOG *cat;
  {
    UIDBDATA *uidbdata;
	uidbdata = IIUIdbdata();
	while ( comp != NULL )
	{
    char *_edit;
    char *_compile;
    char *_formedit;
    char *_newedit;
    char *_go;
    char *_print;
    char *passdec_fld;
    register COMP_CLASS_INFO *cp;
		if ( comp->class == OC_HLPROC
				&& ((HLPROC *)comp)->ol_lang < OLC )
		{ /* unsupported host-language cannot be edited */
			IIUGerr(E_AB0139_UnsupportedLang, UG_ERR_ERROR, 
					1, comp->name
			);
			return;
		}
		cp = iiabApCompClass(comp->class, TRUE /* form init */);
		if ( cp == NULL )
			return;
		_edit = _getMenuitem( &(cp->edit) );
		_compile = _getMenuitem( &(cp->compile) );
		_formedit = _getMenuitem( &(cp->formedit) );
		_go = _getMenuitem( &(cp->go) );
		_print = _getMenuitem( &(cp->print) );
		passdec_fld = (comp->class == OC_HLPROC) ? _passdec : NULL;
		/* perhaps arbitrary, but no go implies no newedit. */
		_newedit = ( _go != NULL ) ? ERget(AB_NewEdit) : NULL;
/* # line 195 "editcomp.qsc" */	/* display */
    {
      if (IIdispfrm(cp->form,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio(_name,(short *)0,1,32,0,comp->name);
      IIputfldio(_short_remark,(short *)0,1,32,0,comp->short_remark);
      IIputfldio(_create_date,(short *)0,1,32,0,comp->create_date);
      IIputfldio(_owner,(short *)0,1,32,0,comp->owner);
      IIputfldio(_altr_date,(short *)0,1,32,0,comp->alter_date);
      IIputfldio(_last_altered_by,(short *)0,1,32,0,comp->altered_by);
      {
/* # line 205 "editcomp.qsc" */	/* host code */
			(*cp->display)( comp );
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 210 "editcomp.qsc" */	/* host code */
			if ( _name_check(cp) == FAIL )
/* # line 211 "editcomp.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 212 "editcomp.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 217 "editcomp.qsc" */	/* host code */
			if ( _type_check(cp, comp) == FAIL )
			{ /* invalid Type */
/* # line 219 "editcomp.qsc" */	/* resume */
            {
              IIresfld(cp->type_check);
              if (1) goto IIfdB1;
            }
/* # line 220 "editcomp.qsc" */	/* host code */
			}
/* # line 221 "editcomp.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 226 "editcomp.qsc" */	/* host code */
			if ( _src_check(cp, appl, comp) == FAIL )
/* # line 227 "editcomp.qsc" */	/* resume */
            {
              IIresfld(_srcfile);
              if (1) goto IIfdB1;
            }
/* # line 228 "editcomp.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 233 "editcomp.qsc" */	/* host code */
			if ( _library_check(cp, comp) == FAIL )
/* # line 234 "editcomp.qsc" */	/* resume */
            {
              IIresfld(_srcfile);
              if (1) goto IIfdB1;
            }
/* # line 235 "editcomp.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 240 "editcomp.qsc" */	/* host code */
			if ( _symbol_check(cp) == FAIL )
/* # line 241 "editcomp.qsc" */	/* resume */
            {
              IIresfld(cp->symbol_check);
              if (1) goto IIfdB1;
            }
/* # line 242 "editcomp.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 247 "editcomp.qsc" */	/* host code */
			if ( _passdec_check(cp, comp) == FAIL )
/* # line 248 "editcomp.qsc" */	/* resume */
            {
              IIresfld(passdec_fld);
              if (1) goto IIfdB1;
            }
/* # line 249 "editcomp.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 7) {
          {
/* # line 254 "editcomp.qsc" */	/* host code */
			APPL_COMP	*new;
            i4 change;
/* # line 257 "editcomp.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&change,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 258 "editcomp.qsc" */	/* host code */
			if ( change != 0 && cp->save != NULL &&
					(*cp->save)( appl, comp ) == OK )
			{ /* update catalog */
/* # line 261 "editcomp.qsc" */	/* putrow */
            {
              if (IItbsetio(3,cat->c_form,cat->c_tfield,-2) != 0) {
                IItcoputio(_short_remark,(short *)0,1,32,0,comp->short_remark);
                IItcoputio(_altr_date,(short *)0,1,32,0,comp->alter_date);
                IItcoputio(_last_altered_by,(short *)0,1,32,0,uidbdata->user);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 266 "editcomp.qsc" */	/* host code */
			}
			if ( (new = (APPL_COMP*)iiabNxtEdit(appl,cat)) != NULL )
			{
				comp = new;
/* # line 270 "editcomp.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF1;
            }
/* # line 271 "editcomp.qsc" */	/* host code */
			}
          }
        } else if (IIretval() == 8) {
          {
/* # line 276 "editcomp.qsc" */	/* host code */
			if ( cp->go.func != NULL )
				(*cp->go.func)( appl, cp, comp );
          }
        } else if (IIretval() == 9) {
          {
/* # line 282 "editcomp.qsc" */	/* host code */
			if ( cp->edit.func != NULL )
				_VOID_ (*cp->edit.func)(appl, comp, cp->form);
          }
        } else if (IIretval() == 10) {
          {
/* # line 288 "editcomp.qsc" */	/* host code */
			if ( cp->compile.func != NULL )
				_VOID_ (*cp->compile.func)(appl, cp, comp);
          }
        } else if (IIretval() == 11) {
          {
/* # line 294 "editcomp.qsc" */	/* host code */
			if ( cp->formedit.func != NULL )
				_VOID_ (*cp->formedit.func)(appl, cp, comp);
          }
        } else if (IIretval() == 12) {
          {
/* # line 301 "editcomp.qsc" */	/* host code */
			IIOOedLongRemark( comp, TRUE );
          }
        } else if (IIretval() == 13) {
          {
/* # line 306 "editcomp.qsc" */	/* host code */
			if ( cp->print.func != NULL )
				_VOID_ (*cp->print.func)(appl, comp);
          }
        } else if (IIretval() == 14) {
          {
            char fld[FE_MAXNAME+1];
/* # line 315 "editcomp.qsc" */	/* host code */
			FLD_VALUE	fv;
			fv._form = cp->form;
			fv._field = fld;
			fv._column = NULL;
/* # line 321 "editcomp.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,fld,22,(char *)0,0);
              } /* IIiqset */
            }
/* # line 322 "editcomp.qsc" */	/* host code */
			if ( cp->type_check != NULL
					&& STequal(fld, cp->type_check) )
			{
				bool lenp = (comp->class == OC_CONST) 
							? FALSE : TRUE;
				IIUFhlpType( cp->types, cp->ntypes, lenp, &fv );
			}
			else if ( STequal(fld, _passdec) )
			{
				IIUFhlpType( _passdec_types, _passdec_ntypes, 
							FALSE, &fv );
			}
			else
			{
				IIUFfldHelp(&fv);
			}
          }
        } else if (IIretval() == 15) {
          {
/* # line 345 "editcomp.qsc" */	/* host code */
			comp = NULL;
/* # line 346 "editcomp.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 16) {
          {
            char title[63+1];
/* # line 355 "editcomp.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,63+1-1,title,_title);
              } /* IIfsetio */
            }
/* # line 356 "editcomp.qsc" */	/* host code */
			FEhelp( cp->help_file, title );
          }
        } else if (IIretval() == 17) {
          {
            i4 change;
/* # line 364 "editcomp.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&change,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 365 "editcomp.qsc" */	/* host code */
			if ( change == 0 || cp->save == NULL )
			{
				comp = NULL;
/* # line 368 "editcomp.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 369 "editcomp.qsc" */	/* host code */
			}
			else if ( (*cp->save)( appl, comp ) == OK )
			{ /* update catalog */
/* # line 372 "editcomp.qsc" */	/* putrow */
            {
              if (IItbsetio(3,cat->c_form,cat->c_tfield,-2) != 0) {
                IItcoputio(_short_remark,(short *)0,1,32,0,comp->short_remark);
                IItcoputio(_altr_date,(short *)0,1,32,0,comp->alter_date);
                IItcoputio(_last_altered_by,(short *)0,1,32,0,uidbdata->user);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 377 "editcomp.qsc" */	/* host code */
				comp = NULL;
/* # line 378 "editcomp.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 379 "editcomp.qsc" */	/* host code */
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
      if (IIFRafActFld(cp->name_check[0],0,1) == 0) goto IIfdE1;
      if (IIFRafActFld(cp->name_check[1],0,1) == 0) goto IIfdE1;
      if (IIFRafActFld(cp->type_check,0,2) == 0) goto IIfdE1;
      if (IIFRafActFld(cp->file_check,0,3) == 0) goto IIfdE1;
      if (IIFRafActFld(cp->library_check,0,4) == 0) goto IIfdE1;
      if (IIFRafActFld(cp->symbol_check,0,5) == 0) goto IIfdE1;
      if (IIFRafActFld(passdec_fld,0,6) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(_newedit,ERget(_NewEditExpl),2,2,7) == 0) goto IIfdE1;
      if (IInmuact(_go,ERget(cp->go.expl),2,2,8) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,8) == 0) goto IIfdE1;
      if (IInmuact(_edit,ERget(cp->edit.expl),2,2,9) == 0) goto IIfdE1;
      if (IInmuact(_compile,ERget(cp->compile.expl),2,2,10) == 0) goto IIfdE1;
      if (IInmuact(_formedit,ERget(cp->formedit.expl),2,2,11) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_LongRemark),ERget(_LongRemExpl),2,2,12) == 0) 
      goto IIfdE1;
      if (IInmuact(_print,ERget(cp->print.expl),2,2,13) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,14) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,14) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,0,15) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,0,0,15) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,0,16) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,16) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(_EndExpl),2,2,17) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,17) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 381 "editcomp.qsc" */	/* host code */
	} /* end while */
  }
/*
** _getMenuitem -- get a menuitem, based on whether we're in VISION.
**
** NOTE -- VISION has no effect now -- this thing could even be a macro --
**	but sooner or later this routine may have to handle some oddball
**	logic.
*/
static char *
_getMenuitem(mu)
MENUS	*mu;
{
	return ( mu->func == NULL ? NULL : ERget(mu->menu) ); 
}
/* Validate an INGRES name. */
static STATUS
  _name_check( cp )
  register COMP_CLASS_INFO *cp;
  {
    i4 change;
    char _fld[FE_MAXNAME+1];
/* # line 407 "editcomp.qsc" */	/* inquire_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,_fld,22,(char *)0,0);
      } /* IIiqset */
    }
/* # line 408 "editcomp.qsc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,_iiOOempty,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&change,20,_fld,0);
      } /* IIiqset */
    }
/* # line 409 "editcomp.qsc" */	/* host code */
	if ( change != 0 )
    {
      char name[FE_MAXNAME+1];
/* # line 413 "editcomp.qsc" */	/* getform */
      {
        if (IIfsetio(cp->form) != 0) {
          IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,name,_fld);
        } /* IIfsetio */
      }
/* # line 414 "editcomp.qsc" */	/* host code */
		/* Mandatory fields will be caught by the FRS */
		if ( STtrmwhite(name) > 0 && FEchkname(name) != OK )
		{
			IIUGerr(E_UG000E_BadName, UG_ERR_ERROR, 1, name);
			return FAIL;
		}
    }
	return OK;
  }
/* Validate a symbol name. */
static STATUS
  _symbol_check( cp )
  register COMP_CLASS_INFO *cp;
  {
    i4 change;
    char sym[FE_MAXNAME+1];
        char    *blanks;
/* # line 433 "editcomp.qsc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,_iiOOempty,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&change,20,cp->symbol_check,0);
      } /* IIiqset */
    }
/* # line 434 "editcomp.qsc" */	/* host code */
	if ( change != 0 )
    {
/* # line 436 "editcomp.qsc" */	/* getform */
      {
        if (IIfsetio(cp->form) != 0) {
          IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,sym,cp->symbol_check);
        } /* IIfsetio */
      }
/* # line 437 "editcomp.qsc" */	/* host code */
		/* Mandatory fields will be caught by the FRS */
		if ( (blanks = STindex( sym, " ", 0 )) != NULL )
		{
			IIUGerr(E_AB036B_Bad_SymbolName, UG_ERR_ERROR, 1, sym );
			return FAIL;
		}
    }
	return OK;
  }
/* Validate a Return Type */
static STATUS
  _type_check( cp, comp )
  register COMP_CLASS_INFO *cp;
  APPL_COMP *comp;
  {
    i4 change;
/* # line 455 "editcomp.qsc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,_iiOOempty,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&change,20,cp->type_check,0);
      } /* IIiqset */
    }
/* # line 456 "editcomp.qsc" */	/* host code */
	if ( change != 0 && cp->type_valid != NULL )
    {
      i4 notnullable;
      char desc[FE_MAXNAME+1];
		DB_DATA_VALUE	temp;
		/* Note:  The Nullable field relies on an internal validation
		** and is not relevant when validating the Return Type.
		*/
/* # line 465 "editcomp.qsc" */	/* getform */
      {
        if (IIfsetio(cp->form) != 0) {
          IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,desc,cp->type_check);
        } /* IIfsetio */
      }
/* # line 466 "editcomp.qsc" */	/* host code */
		if ( STtrmwhite(desc) <= 0
				|| (*cp->type_valid)(comp, desc, &temp) != OK )
		{ /* invalid Type */
			return FAIL;
		}
		if ( comp->class != OC_CONST )
		{
			notnullable = ( temp.db_datatype == DB_NODT );
/* # line 474 "editcomp.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,_iiOOempty,(char *)0, (char *)0) != 0) {
          IIstfsio(54,_nullable,0,(short *)0,1,30,4,&notnullable);
        } /* IIiqset */
      }
/* # line 476 "editcomp.qsc" */	/* host code */
		}
    }
	return OK;
  }
/* Validate a Type for passing decimal values */
static STATUS
  _passdec_check( cp, comp )
  register COMP_CLASS_INFO *cp;
  APPL_COMP *comp;
  {
    i4 change;
/* # line 489 "editcomp.qsc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,_iiOOempty,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&change,20,_passdec,0);
      } /* IIiqset */
    }
/* # line 490 "editcomp.qsc" */	/* host code */
	if ( change != 0 )
    {
      char desc[FE_MAXNAME+1];
		DB_DATA_VALUE	temp;
/* # line 495 "editcomp.qsc" */	/* getform */
      {
        if (IIfsetio(cp->form) != 0) {
          IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,desc,_passdec);
        } /* IIfsetio */
      }
/* # line 496 "editcomp.qsc" */	/* host code */
		if ( STtrmwhite(desc) <= 0 
		  || iiabCkTypeSpec(desc, FALSE, &temp) != OK
		   )
		{ 
			/* invalid syntax for a type */
			IIUGerr(E_AB036A_Bad_PassDecimal, UG_ERR_ERROR, 1,desc);
			return FAIL;
		}
		else
		{
			register i4  i;
			for ( i = _passdec_ntypes; --i >= 0 ; )
			{
				if ( _passdec_types[i] == temp.db_datatype )
				{
					return OK;
				}
			}
			/* not one of the valid types for passing decimals */
			IIUGerr(E_AB036A_Bad_PassDecimal, UG_ERR_ERROR, 1,desc);
			return FAIL;
		}
    }
	return OK;
  }
/* Validate a Source-Code File Name */
static STATUS
  _src_check( cp, appl, comp )
APPL				*appl;
  register APPL_COMP *comp;
  register COMP_CLASS_INFO *cp;
  {
    i4 change;
/* # line 532 "editcomp.qsc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,_iiOOempty,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&change,20,_srcfile,0);
      } /* IIiqset */
    }
/* # line 533 "editcomp.qsc" */	/* host code */
	if ( change != 0 )
    {
		i4	lang;
		char	*source;
      char srcfile[48+1];
/* # line 539 "editcomp.qsc" */	/* getform */
      {
        if (IIfsetio(cp->form) != 0) {
          IIgetfldio((short *)0,1,32,48+1-1,srcfile,_srcfile);
        } /* IIfsetio */
      }
/* # line 540 "editcomp.qsc" */	/* host code */
#ifdef CMS
		/* accept fn ft; convert to fn.ft */
		TOwscnme(srcfile);
/* # line 543 "editcomp.qsc" */	/* putform */
      {
        if (IIfsetio(cp->form) != 0) {
          IIputfldio(_srcfile,(short *)0,1,32,0,srcfile);
        } /* IIfsetio */
      }
/* # line 544 "editcomp.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,_iiOOempty,(char *)0, (char *)0) != 0) {
          IIstfsio(20,_srcfile,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        } /* IIiqset */
      }
/* # line 545 "editcomp.qsc" */	/* host code */
#endif
		if ( comp->class == OC_DBPROC )
		{
			lang = OLSQL;
		/* NOT YET
			source = ((DBPROC *)comp)->source;
		*/
		}
		else if ( comp->class == OC_HLPROC )
		{
			lang = ((HLPROC *)comp)->ol_lang;
			source = ((HLPROC *)comp)->source;
		}
		else
		{
			lang = OLOSL;
			source = ((_4GLPROC *)comp)->source;
		}
		/* Check for pathname
		** Check for valid extension and object-code ** name.  Note if
		** the file name is identical to another, it is allowed so skip
		** the object-code name check.  JupBug #8106.
		*/
		if ( !iiabCkSrcFile(appl->source, srcfile)
		    || IIABcpnCheckPathName(appl->source, srcfile)
		    || !iiabCkExtension(srcfile, lang)
			    || ( iiabCkFileName(srcfile)
				       && !iiabCkObjectName(source, srcfile) ) )
		{
			return FAIL;
		}
    }
	return OK;
  }
/* Toggle the Source-Code File Name field */
static STATUS
  _library_check( cp, comp )
  register COMP_CLASS_INFO *cp;
  register APPL_COMP *comp;
  {
    i4 change;
/* # line 590 "editcomp.qsc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,_iiOOempty,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&change,20,_library,0);
      } /* IIiqset */
    }
/* # line 591 "editcomp.qsc" */	/* host code */
	if ( change != 0 )
    {
      char library[10];
/* # line 595 "editcomp.qsc" */	/* getform */
      {
        if (IIfsetio(cp->form) != 0) {
          IIgetfldio((short *)0,1,32,9,library,_library);
        } /* IIfsetio */
      }
/* # line 596 "editcomp.qsc" */	/* host code */
		/* Note:  The Library field relies on an internal validation
		** and need not be validated here.
		*/
		if ( IIUGyn(library, (STATUS *)NULL) )
		{
/* # line 601 "editcomp.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,_iiOOempty,(char *)0, (char *)0) != 0) {
          IIstfsio(53,_srcfile,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
          IIstfsio(17,_srcfile,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        } /* IIiqset */
      }
/* # line 605 "editcomp.qsc" */	/* putform */
      {
        if (IIfsetio(cp->form) != 0) {
          IIputfldio(_srcfile,(short *)0,1,32,0,ERx("<none>"));
        } /* IIfsetio */
      }
/* # line 606 "editcomp.qsc" */	/* host code */
		}
		else
      {
        register char *source;
        i4 nosrc;
/* # line 612 "editcomp.qsc" */	/* inquire_frs */
        {
          if (IIiqset(2,0,_iiOOempty,(char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,30,4,&nosrc,53,_srcfile,0);
          } /* IIiqset */
        }
/* # line 614 "editcomp.qsc" */	/* host code */
			/* 
			** If source was display only, 
			** library just changed to 'No'.
			*/
			if (nosrc != 0)
			{
				source = ( comp->class == OC_RWFRAME )
					? ((REPORT_FRAME *)comp)->source
					: ((HLPROC *)comp)->source;
/* # line 624 "editcomp.qsc" */	/* set_frs */
        {
          if (IIiqset(2,0,_iiOOempty,(char *)0, (char *)0) != 0) {
            IIstfsio(53,_srcfile,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0
            ));
            IIstfsio(17,_srcfile,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1
            ));
          } /* IIiqset */
        }
/* # line 628 "editcomp.qsc" */	/* putform */
        {
          if (IIfsetio(cp->form) != 0) {
            IIputfldio(_srcfile,(short *)0,1,32,0,source);
          } /* IIfsetio */
        }
/* # line 629 "editcomp.qsc" */	/* host code */
				return FAIL;
			}
      }
    }
	return OK;
  }
/*{
** Name - IIABucvUpdCompVision
**
** Description:
**	Entry to update component from INGRES/VISION
**
** Inputs:
**	appl - application.
**	comp - component.
**	ctx - context pointer given to frame flow entry.
**		or NULL for calls from iiabCompile via "go" or "image".
**
** Returns:
**	{STATUS}
*/
STATUS
IIABucvUpdCompVision( appl, comp, ctx)
APPL		*appl;
OO_OBJECT	*comp;
PTR		*ctx;
{
	register COMP_CLASS_INFO	*cp;
	cp = iiabApCompClass(comp->class, TRUE /* form init */);
	if ( cp == NULL )
		return FAIL;
	IIABnsrNewShortRemark(appl, comp, ctx);
	(*(cp->display))(comp);
	comp->data.dirty = TRUE;
	return (*(cp->save))(appl,comp);
}
/* Titles for MoreInfo of HLprocs */
static READONLY ER_MSGID
        _infoTitles[] = {
                        F_AB0046_ACPDet_title,  /* C */
                        F_AB0047_AFPDet_title,  /* FORTRAN */
                        F_AB0048_APPDet_title,  /* Pascal */
                        F_AB004A_ABPDet_title,  /* BASIC */
                        F_AB0049_AcPDet_title,  /* COBOL */
                        F_AB004C_A1PDet_title,  /* PL/1 */
                        F_AB004B_AAPDet_title,  /* Ada */
                        0
};
/*{
** Name - IIABecvEditCompVision
**
** Description:
**	Entry to component edit from INGRES/VISION
**
** Inputs:
**	appl - application.
**	comp - component.
**	ctx - context pointer given to frame flow entry.
**
** History:
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
*/
VOID
  IIABecvEditCompVision( appl, comp, ctx )
APPL		*appl;
  APPL_COMP *comp;
PTR		ctx;
  {
    register COMP_CLASS_INFO *cp;
    OO_CATALOG *cat = (OO_CATALOG *)ctx;
    char *_look_up;
    char *_mode;
    char title[63+1];
    char *passdec_fld;
	ER_MSGID			msgid;
	GLOBALREF char	*IIabExename;
	GLOBALREF bool	IIabAbf;
	cp = iiabApCompClass(comp->class, TRUE /* form init */);
	if ( cp == NULL )
		return;
	passdec_fld = (comp->class == OC_HLPROC) ? _passdec : NULL;
	if (comp->class == OC_HLPROC)
		msgid = _infoTitles[((HLPROC *)comp)->ol_lang];
	else
		msgid = cp->detail;
	_VOID_ STprintf(title, ERget(msgid), IIabExename);
	if ( IIabAbf )
	{
		_mode = "f";	/* default:  FILL */
		_look_up = ERget(FE_Lookup);
	}
	else
	{
		_mode = "r";	/* READ */
		_look_up = NULL;
	}
/* # line 740 "editcomp.qsc" */	/* display */
    {
      if (IIdispfrm(cp->form,_mode) == 0) goto IIfdE2;
      goto IImuI2;
IIfdI2:;
      if (IIfsetio((char *)0) == 0) goto IIfdE2;
      IIputfldio(_name,(short *)0,1,32,0,comp->name);
      IIputfldio(_short_remark,(short *)0,1,32,0,comp->short_remark);
      IIputfldio(_create_date,(short *)0,1,32,0,comp->create_date);
      IIputfldio(_owner,(short *)0,1,32,0,comp->owner);
      IIputfldio(_altr_date,(short *)0,1,32,0,comp->alter_date);
      IIputfldio(_last_altered_by,(short *)0,1,32,0,comp->altered_by);
      {
/* # line 751 "editcomp.qsc" */	/* host code */
		(*cp->display)( comp );
		/* We nned to override the title from the display routine */
/* # line 754 "editcomp.qsc" */	/* putform */
        {
          if (IIfsetio(cp->form) != 0) {
            IIputfldio(_title,(short *)0,1,32,0,title);
          } /* IIfsetio */
        }
      }
IIfdB2:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 759 "editcomp.qsc" */	/* host code */
		if ( _name_check(cp) == FAIL )
/* # line 760 "editcomp.qsc" */	/* resume */
            {
              if (1) goto IIfdB2;
            }
/* # line 761 "editcomp.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB2;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 766 "editcomp.qsc" */	/* host code */
		if ( _type_check(cp, comp) == FAIL )
		{ /* invalid Type */
/* # line 768 "editcomp.qsc" */	/* resume */
            {
              IIresfld(cp->type_check);
              if (1) goto IIfdB2;
            }
/* # line 769 "editcomp.qsc" */	/* host code */
		}
/* # line 770 "editcomp.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB2;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 775 "editcomp.qsc" */	/* host code */
		if ( _src_check(cp, appl, comp) == FAIL )
/* # line 776 "editcomp.qsc" */	/* resume */
            {
              IIresfld(_srcfile);
              if (1) goto IIfdB2;
            }
/* # line 777 "editcomp.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB2;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 782 "editcomp.qsc" */	/* host code */
		if ( _library_check(cp, comp) == FAIL )
/* # line 783 "editcomp.qsc" */	/* resume */
            {
              IIresfld(_srcfile);
              if (1) goto IIfdB2;
            }
/* # line 784 "editcomp.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB2;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 789 "editcomp.qsc" */	/* host code */
		if ( _passdec_check(cp, comp) == FAIL )
/* # line 790 "editcomp.qsc" */	/* resume */
            {
              IIresfld(passdec_fld);
              if (1) goto IIfdB2;
            }
/* # line 791 "editcomp.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB2;
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 796 "editcomp.qsc" */	/* host code */
		if ( iiabCtFindComp(cat, comp->name, comp->class) > 0 )
		{
			iiabCtRenameComp(cat, appl);
/* # line 799 "editcomp.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(_name,(short *)0,1,32,0,comp->name);
              } /* IIfsetio */
            }
/* # line 800 "editcomp.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 7) {
          {
/* # line 805 "editcomp.qsc" */	/* host code */
		IIOOedLongRemark( comp, IIabAbf );
          }
        } else if (IIretval() == 8) {
          {
            char fld[FE_MAXNAME+1];
/* # line 813 "editcomp.qsc" */	/* host code */
		FLD_VALUE	fv;
		fv._form = cp->form;
		fv._field = fld;
		fv._column = NULL;
/* # line 819 "editcomp.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,fld,22,(char *)0,0);
              } /* IIiqset */
            }
/* # line 820 "editcomp.qsc" */	/* host code */
		if ( cp->type_check != NULL && STequal(fld, cp->type_check) )
		{
			IIUFhlpType( cp->types, cp->ntypes, TRUE, &fv );
		}
		else if ( STequal(fld, _passdec) )
		{
			IIUFhlpType( _passdec_types, _passdec_ntypes, 
					FALSE, &fv );
		}
		else
		{
			IIUFfldHelp(&fv);
		}
          }
        } else if (IIretval() == 9) {
          {
/* # line 840 "editcomp.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else if (IIretval() == 10) {
          {
            char title[63+1];
/* # line 849 "editcomp.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,63+1-1,title,_title);
              } /* IIfsetio */
            }
/* # line 850 "editcomp.qsc" */	/* host code */
		FEhelp( cp->mi_help_file, title );
          }
        } else if (IIretval() == 11) {
          {
            i4 change;
            UIDBDATA *uidbdata;
/* # line 858 "editcomp.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&change,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 859 "editcomp.qsc" */	/* host code */
		if ( change != 0 && cp->save != NULL
				&& (*cp->save)( appl, comp ) == OK )
		{ /* update catalog */
			uidbdata = IIUIdbdata();
/* # line 863 "editcomp.qsc" */	/* putrow */
            {
              if (IItbsetio(3,cat->c_form,cat->c_tfield,-2) != 0) {
                IItcoputio(_short_remark,(short *)0,1,32,0,comp->short_remark);
                IItcoputio(_altr_date,(short *)0,1,32,0,comp->alter_date);
                IItcoputio(_last_altered_by,(short *)0,1,32,0,uidbdata->user);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 868 "editcomp.qsc" */	/* host code */
		}
/* # line 869 "editcomp.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else {
          if (1) goto IIfdE2;
        } /* IIretval */
      } /* IIrunform */
IIfdF2:
      if (IIchkfrm() == 0) goto IIfdB2;
      goto IIfdE2;
IImuI2:
      if (IIFRafActFld(cp->name_check[0],0,1) == 0) goto IIfdE2;
      if (IIFRafActFld(cp->name_check[1],0,1) == 0) goto IIfdE2;
      if (IIFRafActFld(cp->type_check,0,2) == 0) goto IIfdE2;
      if (IIFRafActFld(cp->file_check,0,3) == 0) goto IIfdE2;
      if (IIFRafActFld(cp->library_check,0,4) == 0) goto IIfdE2;
      if (IIFRafActFld(passdec_fld,0,5) == 0) goto IIfdE2;
      if (IIinitmu() == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Rename),ERget(_RenameExpl),2,2,6) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_LongRemark),ERget(_LongRemExpl),2,2,7) == 0) goto 
      IIfdE2;
      if (IInmuact(_look_up,ERget(FE_LookupExpl),0,0,8) == 0) goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,8) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,0,9) == 0) 
      goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,0,0,9) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,0,10) == 0) goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,10) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_End),ERget(_EndExpl),2,2,11) == 0) goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,11) == 0) goto IIfdE2;
      if (IIendmu() == 0) goto IIfdE2;
      goto IIfdI2;
IIfdE2:;
      IIendfrm();
    }
  }
/* # line 873 "editcomp.qsc" */	/* host code */
/*{
** Name - IIABgfvGoCompVision
**
** Description:
**	Entry to do a test execution for a specific component
**	from Vision.
**
** Inputs:
**	appl - application.
**	comp - component.
**	ctx - context pointer given to frame flow entry.
**
** Outputs:
**	None.
*/
VOID
IIABgcvGoCompVision( appl, comp, ctx)
APPL		*appl;
OO_OBJECT	*comp;
PTR		*ctx;
{
	register COMP_CLASS_INFO	*cp;
	cp = iiabApCompClass(comp->class, TRUE /* form init */);
	if ( cp == NULL )
		return;
	(*(cp->display))(comp);
	if ( cp->go.func != NULL )
		(*cp->go.func)( appl, cp, comp );
}
#ifdef JUNK
/*
** set_exists() -- Set existence flags for sub-objects
**
**	This routine sets the existence flags for sub-objects of an application
**	object depending on the type of the object.
*/
static VOID
set_exists (object_type, edit_object, exists)
    register int		object_type;
    register struct edit_object	*edit_object;
    register SOBJ_EXISTS	*exists;
{
	if (object_type == ABFRGBF)
		exists->_graph = graph_status(edit_object->graph);
	else
	{	/* Procedures & ABFRUSER: ABFRNUSER: ABFRQBF: ABFRQDEF: ABFRRW: */
		exists->_form = form_status(edit_object->form);
		if (object_type == ABFRQBF)
			exists->_table = table_status(edit_object->table);
		else if (object_type == ABFRQDEF)
			exists->_qdef = qdef_status(edit_object->table);
		else
		{
			exists->_src = src_status(edit_object->srcfile);
			if (object_type == ABFRRW)
				exists->_report = report_status(edit_object->report);
		}
	}
}
#endif
