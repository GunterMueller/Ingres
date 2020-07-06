# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<adf.h>
#include	<ug.h>
#include	<uf.h>
#include	<ft.h>
#include	<fmt.h>
#include	<frame.h>
#include	<flddesc.h>
#include	"eruf.h"
/*
** Name:	ufhlptyp.qsc -	Front-End Help on DataType Selection Module.
**
** Description:
**	Contains the routine that runs the data type selection frame for the
**	Front-Ends.  Defines:
**
**	IIUFhlpType()	select list help for data type fields.
**
** History:
**	Revision 6.2  89/01  wong
**	Initial revision.
**	Revision 6.4  90/04  wong
**	Major re-work to use list choices.
**	03-aug-1989	(mgw)
**		adc_lenchk() interface changed to take DB_DATA_VALUE last
**		argument instead of nat.
**	23-apr-90 (rudyw)
**		Set NO_OPTIM for ps2_us5 to avoid incorrect optimization.
**	09-jul-1990 (sandyd)
**		DB_LN_FRM_PR_MACRO was renamed to DB_PREC_TO_LEN_MACRO in
**		the official 6.4 - 6.5  dbms.h.
**	31-jul-92 (blaise)
**		Added decimal to list of valid datatypes.
**	04/06/93 (dkh) - Fixed bug 47214.  Code was passing a FRAME pointer
**			 rather than the name of the form to delete.
**	06/20/93 (dkh) - Fixed bug 49638.  Fixed the logic in _getPrecison()
**			 so that selecting OK will properly detect the
**			 fact that no scale value has been entered.
**	14-sep-93 (dianeh)
**		Removed NO_OPTIM setting for absolete PS/2 port (ps2_us5).
**	11-nov-93 (teresal)
**		Allow "1" to be a valid precision value. Bug 54182.
**	19-nov-94 (HANCH04)
**	    	made _colType not READONLY because robf writes to the variable
**		in his change.
**      15-jan-1996 (toumi01; from 1.1 axp_osf port)
**              Added kchin's changes (from 6.4) for axp_osf
**              10-sep-93 (kchin)
**              Change type of frm from i4 to PTR in IIUFhlpType(),
**              _getLength(), and _getPrecision(), since they hold pointers.
**              Use of i4 can result in 64-bit address being truncated,
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	07-sep-2006 (gupsh01)
**	    Added support for ANSI date/time types.
*/
/* EQUEL Declaration:
  typedef struct {
    char *_form;
    char *_field;
    char *_column;
  } FLD_VALUE;
*/
# define	FRS_KEY		2	/* frs key caused activation */
# define	MENU_ITEM	3	/* menuitem caused activation */
/*{
** Name:	IIUFhlpType() -	Selection List Help for Data Type Fields.
**
** Description:
**	Displays a pop-up table field containing a list of legal data types
**	from which a user can select one to fill in in a field or column.
**
** Inputs:
**	types		{DB_DT_ID []}  Array of legal types to be selected,
**					NULL if all types.
**	ntypes		{nat}  Number of legal types in array.
**	need_length	{bool}  Whether a type length is required.
**	fldv		{FLD_VALUE *}  Field or column in which to place
**					selected type.
**
** History:
**	03/89 (jhw)  Written.
**	04/90 (jhw)  Major rewrite.
**	05-oct-2006 (gupsh01)
**	    Modify date to ingresdate.
*/
static READONLY
  char _types[]= ERx("types"), _type[]= ERx("type"), _desc[]= ERx("desc"), 
  _dt_type[]= ERx("dt_type"), _len_req[]= ERx("len_req"), _length[]= ERx("length")
  , _prec[]= ERx("prec"), _scale[]= ERx("scale");
  typedef struct {
    i2 dt_type;
		bool	len_req;		/* requires length */
		ER_MSGID desc;			/* description */
    char typename[FE_MAXNAME+1];
  } TYPE_NAMES;
static READONLY
  TYPE_NAMES _Types[]= {
		{DB_CHA_TYPE,	TRUE,	F_UF0070_char,	ERx("char")},
		{DB_DTE_TYPE,	FALSE,	F_UF0071_date,	ERx("ingresdate")},
		{DB_DEC_TYPE,	TRUE,	F_UF0072_decimal,ERx("decimal")},
		{DB_INT_TYPE,	FALSE,	F_UF0073_int,	ERx("integer")},
		{DB_FLT_TYPE,	FALSE,	F_UF0074_float,	ERx("float")},
		{DB_MNY_TYPE,	FALSE,	F_UF0075_money,	ERx("money")},
		{DB_NODT,	FALSE,	F_UF0076_none,	ERx("none")},
		{DB_LOGKEY_TYPE,FALSE,	F_UF0077_logical,ERx("object_key")},
		{DB_INT_TYPE,	FALSE,	F_UF0078_short,	ERx("smallint")},
		{DB_TABKEY_TYPE,FALSE,	F_UF0079_tab_key,ERx("table_key")},
		{DB_VCH_TYPE,	TRUE,	F_UF007A_varchar,ERx("varchar")},
		{DB_ADTE_TYPE,	FALSE,	F_UF0080_ansidate,ERx("ansidate")},
		{DB_TMWO_TYPE,	FALSE,	F_UF0081_ansitmwo,ERx("time without time zone")},
		{DB_TMW_TYPE,	FALSE,	F_UF0082_ansitmw,ERx("time with time zone")},
		{DB_TME_TYPE,	FALSE,	F_UF0083_ansitme,ERx("time with local time zone")},
		{DB_TSWO_TYPE,	FALSE,	F_UF0084_ansitswo,ERx("timestamp without time zone")},
		{DB_TSW_TYPE,	FALSE,	F_UF0085_ansitsw,ERx("timestamp with time zone")},
		{DB_TSTMP_TYPE,	FALSE,	F_UF0086_ansitstmp,ERx("timestamp with local timezone")},
		{DB_INYM_TYPE,	FALSE,	F_UF0087_ansiinym,ERx("interval year to month")},
		{DB_INDS_TYPE,	FALSE,	F_UF0088_ansiinds,ERx("interval day to second")},
};
#define _OKExpl		F_FE0108_ExplSelect
#define	_HelpExpl	F_FE0100_ExplFrameHelp
#define	_CancelExpl	F_FE0103_ExplCancel
static i4	_select();
VOID
IIUFhlpType ( types, ntypes, need_length, fldv )
DB_DT_ID	types[];
register i4	ntypes;
bool		need_length;
  FLD_VALUE *fldv;
  {
	static DB_DATA_VALUE	_colType = { NULL, 8, DB_CHR_TYPE, 0 };
	static DB_DATA_VALUE	_colDesc = { NULL, 0, DB_CHR_TYPE, 0 };
	static FLD_DESC		_colsDesc[] = {
						{ _type, NULL, &_colType, 0 },
						{ _desc, NULL, &_colDesc, 0 }
	};
	register i4		nlist;
    register TYPE_NAMES *tp;
    PTR frm;
    char typename[FE_MAXNAME+1];
	TYPE_NAMES		*type_list[sizeof(_Types)/sizeof(_Types[0])];
/* No titles, yet.
	if ( _colsDesc[0].title == NULL )
	{
		_colsDesc[0].title = ERget(F_UF0061_TypeTitle);
	}
*/
	/* Find legal data types to be displayed */
	nlist = 0;
	for ( tp = _Types ; tp < &_Types[sizeof(_Types)/sizeof(_Types[0])] ;
			++tp )
	{
		register DB_DT_ID	*type;
		if ( types == NULL )
		{ /* all types ...
			/* ... but logical keys */
			type = ( tp->dt_type == DB_LOGKEY_TYPE
					|| tp->dt_type == DB_TABKEY_TYPE )
				? NULL : &tp->dt_type;
		}
		else
		{
			type = &types[ntypes-1];
			do
			{
				if ( *type == tp->dt_type )
					break;
			} while ( --type >= types );
		}
		if ( type >= types && type != NULL )
		{
			i4	length;
			type_list[nlist++] = tp;
			length = STlength(ERget(tp->desc));
			_colDesc.db_length = max(length, _colDesc.db_length);
			length = STlength(tp->typename);
			_colType.db_length = max(length, _colType.db_length);
		}
	} /* end for */
	if ( IIFRmdfMakeDefaultForm( ERget(F_UF0060_DT_Instr), _types, FALSE,
					2, _colsDesc, nlist,
					(FRAME **)&frm ) != OK )
	{ /* couldn't make form */
		return;
	}
/* # line 224 "ufhlptyp.qsc" */	/* addform */
    {
      IIaddform(frm);
    }
/* # line 225 "ufhlptyp.qsc" */	/* inittable */
    {
      if (IItbinit(_types,_types,(char *)"r") != 0) {
        IIthidecol(_dt_type,(char *)"i2");
        IIthidecol(_len_req,(char *)"i1");
        IItfill();
      } /* IItbinit */
    }
/* # line 227 "ufhlptyp.qsc" */	/* host code */
	/* Load data types in table field */
	ntypes = nlist;
	while ( --nlist >= 0 )
    {
      i1 requires_length;
		tp = type_list[ntypes - nlist - 1];
		requires_length = need_length && tp->len_req;
/* # line 235 "ufhlptyp.qsc" */	/* loadtable */
      {
        if (IItbact(_types,_types,1) != 0) {
          IItcoputio(_type,(short *)0,1,32,0,tp->typename);
          IItcoputio(_desc,(short *)0,1,32,0,ERget(tp->desc));
          IItcoputio(_dt_type,(short *)0,1,30,2,&tp->dt_type);
          IItcoputio(_len_req,(short *)0,1,30,1,&requires_length);
          IITBceColEnd();
        } /* IItbact */
      }
    }
/* # line 243 "ufhlptyp.qsc" */	/* host code */
	/* Display table field */
	if ( IIFRrlpRunListPick( _types, _types, LPGR_FLOAT, LPGR_FLOAT,
				ERget(F_UF0065_DT_Title), ERx("ufdatype.hlp"),
				_select, (PTR)typename ) >= 0 )
	{ /* type selected */
		if ( fldv->_column == NULL || *fldv->_column == EOS )
		{
/* # line 250 "ufhlptyp.qsc" */	/* putform */
    {
      if (IIfsetio(fldv->_form) != 0) {
        IIputfldio(fldv->_field,(short *)0,1,32,0,typename);
      } /* IIfsetio */
    }
/* # line 251 "ufhlptyp.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,fldv->_form,(char *)0, (char *)0) != 0) {
        IIstfsio(20,fldv->_field,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1
        ));
      } /* IIiqset */
    }
/* # line 252 "ufhlptyp.qsc" */	/* host code */
		}
		else
		{
/* # line 255 "ufhlptyp.qsc" */	/* putrow */
    {
      if (IItbsetio(3,fldv->_form,fldv->_field,-2) != 0) {
        IItcoputio(fldv->_column,(short *)0,1,32,0,typename);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 258 "ufhlptyp.qsc" */	/* set_frs */
    {
      if (IIiqset(5,0,fldv->_form,fldv->_field,(char *)0) != 0) {
        IIstfsio(20,fldv->_column,0,(short *)0,1,30,sizeof(1),(void *)IILQint(
        1));
      } /* IIiqset */
    }
/* # line 261 "ufhlptyp.qsc" */	/* host code */
		}
/* # line 262 "ufhlptyp.qsc" */	/* set_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(20,fldv->_form,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 263 "ufhlptyp.qsc" */	/* host code */
	}
	IIdelfrm(((FRAME *)frm)->frname);
  }
/*
** Name:	_select() -	Handle Selected Type.
**
** Description:
**	Called by "list pick" module when a data type has been selected, this
**	routine checks if a length or precision and scale are required and
**	calls the appropriate `get' routine if they are.
**
** Inputs:
**	data	{PTR ==> char *}  The type name to be filled in.
**	recno	{nat}  The selected row of the data type table field list.
**
** Outputs:
**	data	{PTR ==> char *}  The specified type name.
**	resume	{bool *}  Whether to resume or not.  FALSE as passed in,
**				this will be set to TRUE if the `get'
**				routines FAIL.
** History:
**	04/90 (jhw) -- Written.
*/
static STATUS	_getLength();
static STATUS	_getPrecision();
static i4
_select ( data, recno, resume )
PTR	data;
i4	recno;
bool	*resume;
{
	if ( recno >= 0 )
	{ /* selected row */
  i2 dt_type;
  i1 len_req;
  char *type = (char *)data;
		/* assumes current row is the selected row */
/* # line 303 "ufhlptyp.qsc" */	/* getrow */
  {
    if (IItbsetio(2,_types,_types,-2) != 0) {
      IItcogetio((short *)0,1,32,0,type,_type);
      IItcogetio((short *)0,1,30,2,&dt_type,_dt_type);
      IItcogetio((short *)0,1,30,1,&len_req,_len_req);
      IITBceColEnd();
    } /* IItbsetio */
  }
/* # line 307 "ufhlptyp.qsc" */	/* host code */
		_VOID_ STtrmwhite(type);
		if ( len_req )
		{ /* length or precision/scale required */
			STATUS	(*get)();
			get = ( dt_type == DB_DEC_TYPE )
				? _getPrecision : _getLength;
			if ( (*get)( dt_type, type ) != OK )
				*resume = TRUE;
		}
	}
	return recno;
}
/*
** Name:	_getLength() -	Prompt for Length.
**
** Description:
**	Displays a pop-up frame that contains a table field column into which
**	a length can be entered.  Character string types require a length.
**
**	The table field column is Nullable to detect if no value was entered.
**	All numeric input is legal but will be verified through ADF (non-
**	positive numbers will fail here.)
**
** Input:
**	dt_type	{DB_DT_ID}  The data-type ID.
**
** Output:
**	typename	{char *}  The type name with the length specification
**					appended.
** Returns:
**	{STATUS}  OK if a valid length was entered,
**		  FAIL otherwise if an error occurs or Cancel is selected.
** History:
**	04/90 (jhw) -- Written.
*/
static STATUS
_getLength ( dt_type, typename )
DB_DT_ID	dt_type;
char		*typename;
  {
	static READONLY
		DB_DATA_VALUE	_lenType = {NULL, sizeof(i4)+1, -DB_INT_TYPE, 0};
	static FLD_DESC		_lenDesc = { _length, NULL, &_lenType, 0 };
    PTR frm;
    i4 row;
    i4 col;
    i4 svlength = 0;
	bool	_1stlength = TRUE;
	STATUS	rval;
	if ( _lenDesc.title == NULL )
		_lenDesc.title = ERget(F_UF0062_LenTitle);
	/* No form title, one column with title, and one row */
	if ( IIFRmdfMakeDefaultForm( (char *)NULL, _length, TRUE, 1, &_lenDesc,
					1, (FRAME **)&frm ) != OK )
		return FAIL;
/* # line 371 "ufhlptyp.qsc" */	/* addform */
    {
      IIaddform(frm);
    }
/* # line 373 "ufhlptyp.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&row,48,(char *)0,0);
        IIiqfsio((short *)0,1,30,4,&col,49,(char *)0,0);
      } /* IIiqset */
    }
/* # line 374 "ufhlptyp.qsc" */	/* host code */
	row -= 3;	/* size of boxed header */
	col += 8;	/* just past name */
/* # line 377 "ufhlptyp.qsc" */	/* display */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpsetio(1,(short *)0,1,30,4,&row);
      IIFRgpsetio(2,(short *)0,1,30,4,&col);
      IIFRgpsetio(5,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      IIFRgpcontrol(2,0);
      if (IIdispfrm(_length,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 382 "ufhlptyp.qsc" */	/* host code */
		/* Table field in UPDATE mode with one row to be filled in */
/* # line 383 "ufhlptyp.qsc" */	/* inittable */
        {
          if (IItbinit(_length,_length,(char *)"u") != 0) {
            IItfill();
          } /* IItbinit */
        }
/* # line 384 "ufhlptyp.qsc" */	/* insertrow */
        {
          if (IItbsetio(5,_length,_length,0) != 0) {
            if (IItinsert() != 0) {
              IITBceColEnd();
            } /* IItinsert */
          } /* IItbsetio */
        }
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 389 "ufhlptyp.qsc" */	/* resume */
            {
              IIresfld(_length);
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
            i4 length;
            i2 nulli;
/* # line 396 "ufhlptyp.qsc" */	/* host code */
		DB_DATA_VALUE	dblen;
/* # line 398 "ufhlptyp.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_length,_length,-2) != 0) {
                IItcogetio(&nulli,1,30,4,&length,_length);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 399 "ufhlptyp.qsc" */	/* host code */
		if ( nulli != DB_EMB_NULL )
		{ /* Length was entered */
			DB_DATA_VALUE	dbv;
			svlength = length;
			_1stlength = FALSE;
			dbv.db_datatype = dt_type;
			dbv.db_length = length;
			dbv.db_prec = 0;
			if ( adc_lenchk(FEadfcb(), TRUE, &dbv, &dblen) != OK )
			{
				FEafeerr(FEadfcb());
/* # line 411 "ufhlptyp.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 412 "ufhlptyp.qsc" */	/* host code */
			}
		}
		else
		{ /* Length was not entered */
			if ( dt_type == DB_CHR_TYPE || dt_type == DB_TXT_TYPE
					|| dt_type == DB_CHA_TYPE
						|| dt_type == DB_VCH_TYPE )
			{
				IIUGerr(E_AD2008_DT_IS_VARLEN, UG_ERR_ERROR, 0);
				if ( !_1stlength )
/* # line 422 "ufhlptyp.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_length,_length,-2) != 0) {
                IItcoputio(_length,(short *)0,1,30,4,&svlength);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 423 "ufhlptyp.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 424 "ufhlptyp.qsc" */	/* host code */
			}
		}
/* # line 426 "ufhlptyp.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 432 "ufhlptyp.qsc" */	/* host code */
		/* note:  the field activation will have already
		** validated the length for the data type.
		*/
            i4 length;
/* # line 437 "ufhlptyp.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_length,_length,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&length,_length);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 438 "ufhlptyp.qsc" */	/* host code */
		_VOID_ STprintf( typename+STlength(typename),
			ERx( dt_type == DB_INT_TYPE || dt_type == DB_FLT_TYPE
					? "%d" : "(%d)"
			),
			length
		);
		rval = OK;
/* # line 445 "ufhlptyp.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 452 "ufhlptyp.qsc" */	/* host code */
		rval = FAIL;
/* # line 453 "ufhlptyp.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 460 "ufhlptyp.qsc" */	/* host code */
		FEhelp(ERx("ufdatlen.hlp"), ERget(F_UF0065_DT_Title));
          }
        } else {
          if (1) goto IIfdE1;
        } /* IIretval */
      } /* IIrunform */
IIfdF1:
      if (IIchkfrm() == 0) goto IIfdB1;
      goto IIfdE1;
IImuI1:
      if (IIactscrl(_length,1,1) == 0) goto IIfdE1;
      if (IIactscrl(_length,0,1) == 0) goto IIfdE1;
      if (IITBcaClmAct(_length,_length,0,2) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_OK),ERget(_OKExpl),1,1,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,1,1,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Cancel),ERget(_CancelExpl),0,0,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,0,0,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,0,5) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,5) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 463 "ufhlptyp.qsc" */	/* host code */
	IIdelfrm(((FRAME *)frm)->frname);
	return rval;
  }
/*
** Name:	_getPrecision() -	Prompt for Precision and Scale.
**
** Description:
**	Displays a pop-up frame that contains two columns in a table field into
**	which a precision and scale can be entered.  This is for the "decimal"
**	type.  Values are mandatory.
**
**	The table field columns are Nullable to detect if no values were
**	entered.  All numeric input is legal as long as it is positive and meets
**	the requirements for precision and scale; they will also verified
**	through ADF.
**
** Input:
**	dt_type	{DB_DT_ID}  The data-type ID.
**
** Output:
**	typename	{char *}  The type name with the precision/scale
**					specification appended.
** Returns:
**	{STATUS}  OK if a valid precision and scale were entered,
**		  FAIL otherwise if an error occurs or Cancel is selected.
** History:
**	04/90 (jhw) -- Written.
*/
static STATUS
_getPrecision ( dt_type, typename )
DB_DT_ID	dt_type;
char		*typename;
  {
	static READONLY
		DB_DATA_VALUE	_precType = {NULL, sizeof(i4)+1,-DB_INT_TYPE,0},
				_scaleType ={NULL, sizeof(i4)+1,-DB_INT_TYPE,0};
	static FLD_DESC		_colDesc[] = {
					{ _prec, NULL, &_precType, 0 },
					{ _scale, NULL, &_scaleType, 0 }
	};
    PTR frm;
    i4 row;
    i4 col;
    i4 actcmd;
	STATUS	rval;
	i4	maxprec = DB_MAX_DECPREC;
	if ( _colDesc[0].title == NULL )
	{
		_colDesc[0].title = ERget(F_UF0063_PrecTitle);
		_colDesc[1].title = ERget(F_UF0064_ScaleTitle);
	}
	/* No form title, two columns with titles, and one row */
	if ( IIFRmdfMakeDefaultForm( (char *)NULL, _prec, TRUE, 2, _colDesc,
					1, (FRAME **)&frm ) != OK )
		return FAIL;
/* # line 524 "ufhlptyp.qsc" */	/* addform */
    {
      IIaddform(frm);
    }
/* # line 526 "ufhlptyp.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&row,48,(char *)0,0);
        IIiqfsio((short *)0,1,30,4,&col,49,(char *)0,0);
      } /* IIiqset */
    }
/* # line 527 "ufhlptyp.qsc" */	/* host code */
	row -= 3;	/* size of boxed header */
	col += 8;	/* just past name */
/* # line 530 "ufhlptyp.qsc" */	/* display */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpsetio(1,(short *)0,1,30,4,&row);
      IIFRgpsetio(2,(short *)0,1,30,4,&col);
      IIFRgpsetio(5,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      IIFRgpcontrol(2,0);
      if (IIdispfrm(_prec,(char *)"f") == 0) goto IIfdE2;
      goto IImuI2;
IIfdI2:;
      if (IIfsetio((char *)0) == 0) goto IIfdE2;
      {
/* # line 535 "ufhlptyp.qsc" */	/* host code */
		/* Table field in UPDATE mode with one row to be filled in */
/* # line 536 "ufhlptyp.qsc" */	/* inittable */
        {
          if (IItbinit(_prec,_prec,(char *)"u") != 0) {
            IItfill();
          } /* IItbinit */
        }
/* # line 537 "ufhlptyp.qsc" */	/* insertrow */
        {
          if (IItbsetio(5,_prec,_prec,0) != 0) {
            if (IItinsert() != 0) {
              IITBceColEnd();
            } /* IItinsert */
          } /* IItbsetio */
        }
      }
IIfdB2:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 542 "ufhlptyp.qsc" */	/* resume */
            {
              IIresfld(_prec);
              if (1) goto IIfdB2;
            }
          }
        } else if (IIretval() == 2) {
          {
            i4 prec;
            i4 scale;
            i2 nulli[2];
/* # line 551 "ufhlptyp.qsc" */	/* host code */
		DB_DATA_VALUE	dbprec;
/* # line 553 "ufhlptyp.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_prec,_prec,-2) != 0) {
                IItcogetio(&nulli[0],1,30,4,&prec,_prec);
                IItcogetio(&nulli[1],1,30,4,&scale,_scale);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 557 "ufhlptyp.qsc" */	/* host code */
		/* assert:  nulli[0] == DB_EMB_NULL ==> nulli[1] == DB_EMB_NULL
		*/
		if ( nulli[0] != DB_EMB_NULL )
		{ /* Precision or scale was entered */
			DB_DATA_VALUE	dbv;
			if ( prec <= 0 || prec > DB_MAX_DECPREC )
			{
				IIUGerr(E_UF005B_NO_DEC_PREC, UG_ERR_ERROR, 1,
					&maxprec);
/* # line 567 "ufhlptyp.qsc" */	/* resume */
            {
              IIrescol(_prec,_prec);
              if (1) goto IIfdB2;
            }
/* # line 568 "ufhlptyp.qsc" */	/* host code */
			}
			else
			{
            char fld[FE_MAXNAME+1];
/* # line 573 "ufhlptyp.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_prec,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,fld,34,_prec,0);
              } /* IIiqset */
            }
/* # line 574 "ufhlptyp.qsc" */	/* host code */
				if ( STequal(fld, _prec) )
				{ /* precision OK, now get scale */
/* # line 576 "ufhlptyp.qsc" */	/* inquire_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&actcmd,38,(char *)0,0);
              } /* IIiqset */
            }
/* # line 577 "ufhlptyp.qsc" */	/* host code */
					/*
					**  If user selected the OK menuitem
					**  and the scale field is still null,
					**  then we need to put out an error
					**  message and move the cursor to
					**  the scale field since no scale
					**  value has been entered yet.
					*/
					if ((actcmd == FRS_KEY ||
						actcmd == MENU_ITEM) &&
						(nulli[1] == DB_EMB_NULL))
					{
						IIUGerr( E_UF005C_NO_DEC_SCALE,
							UG_ERR_ERROR, 1,
							&maxprec);
/* # line 592 "ufhlptyp.qsc" */	/* resume */
            {
              IIrescol(_prec,_scale);
              if (1) goto IIfdB2;
            }
/* # line 593 "ufhlptyp.qsc" */	/* host code */
					}
					/*
					**  Resume next here will either
					**  move the cursor to the scale
					**  field or cause the OK menuitem
					**  block to get executed.
					*/
/* # line 600 "ufhlptyp.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB2;
            }
/* # line 601 "ufhlptyp.qsc" */	/* host code */
				}
				else if ( nulli[1] == DB_EMB_NULL
						|| scale < 0 || scale > prec )
				{
					IIUGerr( E_UF005C_NO_DEC_SCALE,
						UG_ERR_ERROR, 1, &maxprec);
/* # line 607 "ufhlptyp.qsc" */	/* resume */
            {
              IIrescol(_prec,_scale);
              if (1) goto IIfdB2;
            }
/* # line 608 "ufhlptyp.qsc" */	/* host code */
				}
			}
			dbv.db_datatype = dt_type;
			dbv.db_length = DB_PREC_TO_LEN_MACRO(prec);
			dbv.db_prec = DB_PS_ENCODE_MACRO(prec,scale);
			if ( adc_lenchk(FEadfcb(), TRUE, &dbv, &dbprec) != OK )
			{
				FEafeerr(FEadfcb());
/* # line 617 "ufhlptyp.qsc" */	/* resume */
            {
              if (1) goto IIfdB2;
            }
/* # line 618 "ufhlptyp.qsc" */	/* host code */
			}
		}
		else
		{ /* Precision/Scale were not entered */
			IIUGerr(E_UF005B_NO_DEC_PREC, UG_ERR_ERROR, 1,
				&maxprec);
/* # line 624 "ufhlptyp.qsc" */	/* resume */
            {
              if (1) goto IIfdB2;
            }
/* # line 625 "ufhlptyp.qsc" */	/* host code */
		}
/* # line 626 "ufhlptyp.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB2;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 632 "ufhlptyp.qsc" */	/* host code */
		/* note:  the field activation will have already
		** validated the precesion and scalE for the data type.
		*/
            i4 prec;
            i4 scale;
/* # line 638 "ufhlptyp.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_prec,_prec,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&prec,_prec);
                IItcogetio((short *)0,1,30,4,&scale,_scale);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 639 "ufhlptyp.qsc" */	/* host code */
		_VOID_ STprintf( typename+STlength(typename),
					ERx("(%d,%d)"), prec, scale
		);
		rval = OK;
/* # line 643 "ufhlptyp.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 650 "ufhlptyp.qsc" */	/* host code */
		rval = FAIL;
/* # line 651 "ufhlptyp.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 658 "ufhlptyp.qsc" */	/* host code */
		FEhelp(ERx("ufdatprc.hlp"), ERget(F_UF0065_DT_Title));
          }
        } else {
          if (1) goto IIfdE2;
        } /* IIretval */
      } /* IIrunform */
IIfdF2:
      if (IIchkfrm() == 0) goto IIfdB2;
      goto IIfdE2;
IImuI2:
      if (IIactscrl(_prec,1,1) == 0) goto IIfdE2;
      if (IIactscrl((char *)"prec",0,1) == 0) goto IIfdE2;
      if (IITBcaClmAct(_prec,_prec,0,2) == 0) goto IIfdE2;
      if (IITBcaClmAct(_prec,_scale,0,2) == 0) goto IIfdE2;
      if (IIinitmu() == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_OK),ERget(_OKExpl),1,1,3) == 0) goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,1,1,3) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Cancel),ERget(_CancelExpl),0,0,4) == 0) goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,0,0,4) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,0,5) == 0) goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,5) == 0) goto IIfdE2;
      if (IIendmu() == 0) goto IIfdE2;
      goto IIfdI2;
IIfdE2:;
      IIendfrm();
    }
/* # line 661 "ufhlptyp.qsc" */	/* host code */
	IIdelfrm(((FRAME *)frm)->frname);
	return rval;
  }
