# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<st.h>
#include	<cm.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<uf.h>
#include	<adf.h>
#include	<afe.h>
#include	<frserrno.h>
#include	"erfr.h"
/*
**  States for an unloaded row of a table field.
*/
# define	stNEW		1
# define	stUNCHANGED	2
# define	stCHANGE	3
/**
** Name:	findrow.qc -	Front-End Find String in Table Field Routine.
**
** Description:
**	Contains a routine used to find a value in a table field column.  This
**	routine is used by the front-ends and can be called by users from OSL as
**	well.  Defines:
**
**	IIFRfind_row()	find value in a table field column.
**
** History:
**	Revision 6.4  89/08  wong
**	Changed to be 'IIFRfind_row()' and added value parameter, user
**	selectable pattern matching, etc.
**	29-jan-92 (davel)
**		Fixed bug 42139 in look_for_row().
**      16-Oct-92 (donc)
**              Fixed bug 40744 - SeqVio with SQL pattern matching
**      20-May-93 (donc) 
**		Fixed bug 51503 - Moved fix for 40744 above the call
**		to IIAFcvWildCard. That way the message displays the string
**		prior to conversion to QUEL pattern matching characters.
**
**	Revision 6.1  20-nov-1987 (wong)
**	Changed to be 'IIfind_row()' and added pattern-matching and ADT
**	comparison.  (Was originally 'FEtabfnd()'.)
**
**	Revision 6.0  22-oct-1986 (wong)
**	Optimized.
**	06/08/87 (dkh) - Put back 5.0 semantics.
**	08/14/87 (dkh) - ER changes.
**	11/16/87 (dkh) - Fixed jup bug 1218.
**	02/20/90 (dkh) - Changed positioning algorithm due to change in
**			 table field scrolling algorithm (Scrolling forward
**			 puts destination row at bottom of table field while
**			 scrolling backward puts destination row at top of
**			 table field.  Also, if desired row is already
**			 visible, then just move cursor there.
**	10/24/90 (dkh) - Dropped use of FEinqferr to remove backward
**			 reference problem.
**	11/07/90 (dkh) - Fixed bug 34335.
**	08/02/91 (dkh) - Fixed bug 38113.
**	4/21/92 (seg) - eliminate unused variable "errno" -- causes porting
**			problem in OS/2.
**	22-apr-93 (connie) Bug #49695
**		'%' is escaped before passing to function using printf
**		for display
**	03-may-93 (connie)
**		take out the previous change since Dave Hung has a better
**		solution by fixing IIUGmsg
**
**	Revision 4.0  30-sep-1985 (peter)
**	Initial revision.
**
**	06/30/93 (dkh) - Fixed bug 38562.  Do not automatically return
**			 a value of 1 if there is only one row of data.
**	08/31/93 (dkh) - Fixed up compiler complaints due to prototyping.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
ADI_FI_ID	IIAFfifConv();
DB_STATUS	IIAFpmEncode();
DB_STATUS	IIAFcvWildCard();
/*{
** Name:	IIFRfind_row() -	Find Value in a Table Field Column.
**
** Description:
**	Searches for a value in a column of a table field, and then positions
**	the cursor on the next row that has that value.  This prompts for the
**	value if one is not passed.  The form, table field, or column to search
**	can be input or the current of whichever is not specified will be used.
**
**	ADT comparison is used for most values, particularly when the column
**	type is not a string or when pattern-matching characters are present
**	in the string value.  For a non-pattern-matching string comparisons,
**	however, this performs a caseless prefix compare on the column values
**	(equivalent to a pattern of "string*".)  When found, this routine
**	scrolls the table to the row, one-quarter of the way down in the table.
**
**	If the input value is to perform a pattern-match it must contain QUEL
**	pattern-match characters.  Prompt values can be specified in either
**	query language, but will be translated as appropriate by the routine
**	'IIAFcvWildCard()' into a QUEL pattern-matching string.  Since this
**	performs the translation to an internal QUEL comparison string in
**	place, any input value will be overwritten.  The caller must save
**	the value if this is not desired.
**
**	This now performs an end around search, as in vi.  That is, if
**	it does not find the value below, it then starts at the top
**	and searches down.
**
**	If a user enters a single period, it means to use the last value
**	entered a second time.
**
** Inputs:
**	fv	{FLD_VALUE *}  Optional table field column.
**	value	{DB_DATA_VALUE *}  Optional value for which to search.
**	noscroll {bool}  Whether to scroll to record.
**
** Output:
**	value	{DB_DATA_VALUE *}  If specified (i.e., not NULL) and contains
**			.db_data {PTR}	a string type, may be overwritten.
**
** Returns:
**	{nat}  > 0 The data set row for which the value was found.
**
** Side Effects:
**	Positions cursor in table field.
**
** History:
**	30-sep-1985 (peter)	written.
**	10-oct-1985 (grant)	now prompts for string, looks in column on
**				which cursor is positioned, and gives error
**				message if not found.
**	21-oct-1985 (peter)	add end around search for string.  also, allow
**				a '.' to mean use last string.
**	22-oct-1986 (jhw)	Optimized.
**	06/08/87 (dkh) - Put back 5.0 semantics.
**	20-nov-1987 (jhw)  Added pattern-matching and ADT comparison.
**	07-aug-1989 (jhw)  Added optional input value and user selectable DML
**				pattern-match on prompt.
**	04/90 (jhw)  Form, field, column names should already be blank-trimmed.
**	06/90 (jhw)  Added 'noscroll' and changed to use FLD_VALUE.
*/
i4
IIfind_row ( form, table, tcolumn )
char		*form;
char		*table;
char		*tcolumn;
{
	FLD_VALUE	fld;
	fld._form = form;
	fld._field = table;
	fld._column = tcolumn;
	return IIFRfind_row(&fld, (DB_DATA_VALUE *)NULL, FALSE);
}
/* EQUEL Declaration:
  typedef struct {
    char *_form;
    char *_field;
    char *_column;
  } FLD_VALUE;
*/
/* Last value for prompt.
**
**	Note:  This does not account for the column that was last searched; the
**	last value will be applied regardless when the user enters a period '.'.
*/
static struct {
	u_i4	length;
	i4	meta;
	char	word[FE_PROMPTSIZE+1];
} last = {0, AFE_PM_NOT_FOUND, {EOS}};
static ADI_FI_ID	fiEqCmp();
static i4		look_for_row();
/*
** Name:	_frsError() -	EQL/FRS Error Handler.
**
** Description:
**	Handles certain FRS errors.
*/
#define _SIFRM	8152
#define _SITBL	8154
#define _SICOL	8155
#define _SIATTR	8156
static STATUS
_frsError ( err )
STATUS	*err;
{
	if ( *err == _SIFRM || *err == _SITBL || *err == _SICOL )
		return 0;
	else if ( *err == _SIATTR )
		return 0;	/* ignore derivative error */
	else
		return *err;
	/*NOT REACHED*/
}
i4
  IIFRfind_row( fv, value, noscroll )
FLD_VALUE /*const*/	*fv;
DB_DATA_VALUE /*const*/	*value;
bool			noscroll;
  {
    static READONLY char	_Period[]	= ERx(".");
    bool		is_string;
    STATUS		(*user_errh)();	/* User Error Handler */
    i4 errnum;
    i4 ndrows;
    i4 status;
    DB_DATA_VALUE prompt_s;
    char formname[FE_MAXNAME+1];
    char tablename[FE_MAXNAME+1];
    char colname[FE_MAXNAME+1];
    FLD_VALUE fldv;
    AFE_DCL_TXT_MACRO(FE_PROMPTSIZE+1)	buffer;
    STATUS	(*IIseterr())();
    if ( fv != NULL )
    {
	fldv._form = fv->_form;
	fldv._field = fv->_field;
	fldv._column = fv->_column;
    }
    else
    { /* get current names */
	fldv._form = fldv._field = fldv._column = NULL;
    }
    if ( fldv._form == NULL || *fldv._form == EOS )
    { /* get current form name */
/* # line 254 "findrow.qsc" */	/* inquire_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,formname,19,(char *)0,0);
      } /* IIiqset */
    }
/* # line 255 "findrow.qsc" */	/* host code */
	fldv._form = formname;
/* # line 256 "findrow.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&errnum,1,(char *)0,0);
      } /* IIiqset */
    }
/* # line 257 "findrow.qsc" */	/* host code */
	if ( errnum != 0 || *fldv._form == EOS )
	    return -1;	/* no form */
    }
    user_errh = IIseterr(_frsError);
    if ( fldv._field == NULL || *fldv._field == EOS )
    { /* get current table name */
/* # line 265 "findrow.qsc" */	/* inquire_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,tablename,22,fldv._form,0);
      } /* IIiqset */
    }
/* # line 266 "findrow.qsc" */	/* host code */
	fldv._field = tablename;
/* # line 267 "findrow.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&errnum,1,(char *)0,0);
      } /* IIiqset */
    }
/* # line 268 "findrow.qsc" */	/* host code */
	if ( errnum != 0 || *fldv._field == EOS )
	{
	    _VOID_ IIseterr(user_errh);
	    if ( errnum == _SIFRM )
	    	IIFDerror(FRMNOINIT, 1, fldv._form);
	    return -1;
	}
    }
    /* Check if field (specified or current) is a table field */
    {
      i4 istable;
/* # line 281 "findrow.qsc" */	/* inquire_frs */
      {
        if (IIiqset(2,0,fldv._form,(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&istable,28,fldv._field,0);
        } /* IIiqset */
      }
/* # line 282 "findrow.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&errnum,1,(char *)0,0);
        } /* IIiqset */
      }
/* # line 283 "findrow.qsc" */	/* host code */
	if ( errnum != 0 || istable == 0 )
	{
	    _VOID_ IIseterr(user_errh);
	    if ( errnum == _SIFRM )
	    	IIFDerror(FRMNOINIT, 1, fldv._form);
	    else if ( errnum == _SITBL || errnum == _SIATTR )
	    	IIFDerror(TBBAD, 2, fldv._field, fldv._form);
	    else
		IIFDerror(E_FR004D_Not_on_tf, 0);
	    return -1;
	}
    }
    _VOID_ IIseterr(user_errh);
    /* Get number of rows and column name */
/* # line 299 "findrow.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,fldv._form,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&ndrows,32,fldv._field,0);
        IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,colname,34,fldv._field,0);
      } /* IIiqset */
    }
/* # line 304 "findrow.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&errnum,1,(char *)0,0);
      } /* IIiqset */
    }
/* # line 305 "findrow.qsc" */	/* host code */
    if ( errnum != 0 )
    {
	return -1;
    }
    else if ( ndrows == 0 )
    {
	IIFDerror(E_FR004E_TF_is_empty, 0);
	return -1;
    }
    else if ( ndrows == 1 )
    {
/* # line 316 "findrow.qsc" */	/* getrow */
    {
      if (IItbsetio(2,fldv._form,fldv._field,1) != 0) {
        IItcogetio((short *)0,1,30,4,&status,(char *)"_STATE");
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 318 "findrow.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&errnum,1,(char *)0,0);
      } /* IIiqset */
    }
/* # line 319 "findrow.qsc" */	/* host code */
	if ( errnum != 0 || status == 0 )
	{
	    IIFDerror(E_FR004E_TF_is_empty, 0);
	    return -1;
	}
    }
    if ( fldv._column == NULL || *fldv._column == EOS )
    {
	fldv._column = colname;
	if ( *fldv._column == EOS )
	    return -1;
    }
    /* Get column type information */
    user_errh = IIseterr(_frsError);
/* # line 336 "findrow.qsc" */	/* inquire_frs */
    {
      if (IIiqset(4,0,fldv._form,fldv._field,(char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&status,25,fldv._column,0);
      } /* IIiqset */
    }
/* # line 337 "findrow.qsc" */	/* host code */
    _VOID_ IIseterr(user_errh);
/* # line 339 "findrow.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&errnum,1,(char *)0,0);
      } /* IIiqset */
    }
/* # line 340 "findrow.qsc" */	/* host code */
    if ( errnum  != 0 )
    {
	if ( errnum == _SIFRM )
    		IIFDerror(FRMNOINIT, 1, fldv._form);
	else if ( errnum == _SITBL )
		IIFDerror(TBBAD, 2, fldv._field, fldv._form);
	else if ( errnum == _SICOL || errnum == _SIATTR )
		IIFDerror(TBBADCOL, 2, fldv._field, fldv._column);
	return -1;
    }
    is_string = (bool)( status == 3 );
    /*
    ** Get search string
    */
    if ( value == NULL )
    { /* prompt */
	register DB_TEXT_STRING	*word;
	word = (DB_TEXT_STRING *)&buffer;
	prompt_s.db_datatype = DB_TXT_TYPE;
	prompt_s.db_length = FE_PROMPTSIZE + sizeof(word->db_t_count);
	prompt_s.db_data = (PTR)word;
/* # line 365 "findrow.qsc" */	/* prompt */
    {
      IIprmptio(0,ERget(S_FR002F_Enter_string_to_find),(short *)0,1,44,0,&
      prompt_s);
    }
/* # line 366 "findrow.qsc" */	/* host code */
	word->db_t_text[word->db_t_count] = EOS;
	if ( (word->db_t_count = STtrmwhite((char *) word->db_t_text)) <= 0 )
	    return -1;
	prompt_s.db_length = sizeof(buffer);
	if ( last.word[0] != EOS && STequal(word->db_t_text, _Period) )
	{ /* single . entered.  means to use last word again. */
	    word->db_t_count = last.length;
	    STcopy(last.word, word->db_t_text);
	}
	else if ( is_string && STequal(word->db_t_text, ERx("\\.")) )
	{ /* single . preceded by backslash.  means to use single . */
	    STcopy(_Period, last.word);
	    STcopy(_Period, word->db_t_text);
	    word->db_t_count = last.length = sizeof(_Period)-1;
	    last.meta = AFE_PM_NOT_FOUND;
	}
	else
	{ /* anything else.  simply copy to lastword */
	    last.length = word->db_t_count;
	    STcopy(word->db_t_text, last.word);
	    last.meta  = AFE_PM_FOUND;
	}
	value = &prompt_s;
	if ( last.meta != AFE_PM_NOT_FOUND )
	{ /* translate */
                /* Bug 51503 - Move the fix for 40744 above IIAFcvWildCard */ 
		/* Bug 40744 - last.word not set before issuing error pop-up */
		STcopy( word->db_t_text, last.word );
		_VOID_ IIAFcvWildCard(value, DB_QUEL, AFE_PM_CHECK, &last.meta);
		word->db_t_text[word->db_t_count] = EOS;
	}
    }
    /*
    ** Set up for comparison and unload table.
    */
    return look_for_row( &fldv, value, (bool)(value == &prompt_s), noscroll );
  }
/*
** Name:	look_for_row() -	Search Table Field for Matching Value.
**
** Description:
**	Unloads the table field searching for a value in a column.  An ADT
**	comparison will be performed if the column is not a string, or the
**	search value contains QUEL pattern-match characters.  Otherwise, an
**	unanchored case-less prefix comparison will be performed on the
**	column string value.
**
**	For a pattern-match comparison, the column value will be converted into
**	a string.  Also, the search value can only contain QUEL pattern-match
**	characters; SQL will not be recognized.  And finally, the value will be
**	translated in place to an internal QUEL pattern-match string so the
**	caller is responsible for saving it if it is not to be overwritten.
**
** Inputs:
**	fldv	{FLD_VALUE *}  The table field column.
**	value	{DB_DATA_VALUE *}  The value for which to search.
**	prompt	{bool}  Whether value was prompted for from user.
**	noscroll {bool} Whether to scroll to record.
**
** Output:
**	value	{DB_DATA_VALUE *}  If specified (i.e., not NULL) and contains
**			.db_data {PTR}	a string type, may be overwritten.
**
** Returns:
**	{nat}	> 0 The data set row for which the value was found.
**
** History:
**	08/89 (jhw) -- Abstracted from previous 'IIfind_row()'.
**	06/90 (jhw)  Added 'noscroll' and changed to use FLD_VALUE.
**	29-jan-92 (davel)  bug 42139
**		Convert tablefield column value to same string type as
**		input value whenever the value is program-supplied (i.e.
**		when prompt == FALSE).
**	07-sep-06 (gupsh01)
**		Added support for ANSI date/time types.
*/
static i4
  look_for_row( fldv, value, prompt, noscroll )
  FLD_VALUE *fldv;
DB_DATA_VALUE	*value;
bool		prompt;
bool		noscroll;
  {
	ADF_CB		*cb;
	i4		rowfound;	/* first matching row found */
	ADI_FI_ID	cmpfid;		/* comparison function instance ID */
	ADI_FI_ID	cvfid;		/* conversion function instance ID */
	bool		matched[4];	/* array since result may be nullable */
	i4		cvlen;
	u_char		*cvalue;
    i4 status;
    i4 currec;
    i4 startrec;
    char currfld[FE_MAXNAME+1];
    DB_DATA_VALUE dbv;
	DB_DATA_VALUE	vdbv;		/* string data value */
	ALIGN_RESTRICT	d[(DB_MAXSTRING+sizeof(i2))/sizeof(ALIGN_RESTRICT)+1];
	ALIGN_RESTRICT	vd[sizeof(d)/sizeof(ALIGN_RESTRICT)];
	AFE_OPERS	oplist;
	AFE_OPERS	cvoplist;
	DB_DATA_VALUE   *oparr[3];
	DB_DATA_VALUE   result;
	DB_TEXT_STRING	dbtext;
    i4 toprec;
    i4 botrec;
	ADF_CB	*FEadfcb();
	/* Get column data descriptor
	**	(use 'currec' and 'startrec' as temporaries.)
	*/
/* # line 485 "findrow.qsc" */	/* inquire_frs */
    {
      if (IIiqset(4,0,fldv->_form,fldv->_field,(char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&currec,42,fldv->_column,0);
        IIiqfsio((short *)0,1,30,4,&startrec,24,fldv->_column,0);
      } /* IIiqset */
    }
/* # line 489 "findrow.qsc" */	/* host code */
	dbv.db_datatype = (DB_DT_ID)currec;
	dbv.db_length = startrec;
	dbv.db_data = (PTR)d;
	/*
	**  Check to see if we the column datatype is nullable, if
	**  it is, need to increase the size of dbv.db_length
	**  since inquire_frs only returned the user length and
	**  does not account for the nullability byte.
	*/
	if (AFE_NULLABLE(dbv.db_datatype))
	{
		result.db_datatype = dbv.db_datatype;
		result.db_length = startrec;
		AFE_UNNULL(&result);
		/* Put back real length since AFE_UNNULL changed it */
		result.db_length = startrec;
		AFE_MKNULL(&result);
		dbv.db_length = result.db_length;
	}
	/*
	**  Add additional length when dealing with variable length
	**  string datatypes.
	*/
	switch(AFE_DATATYPE(&dbv))
	{
		case DB_TXT_TYPE:
		case DB_VCH_TYPE:
		case DB_LTXT_TYPE:
			dbv.db_length += sizeof(dbtext.db_t_count);
	}
	cb = FEadfcb();	/* get ADF control block */
	result.db_datatype = DB_BOO_TYPE;
	result.db_length = sizeof(bool);
	result.db_data = (PTR)&matched[0];
	cvfid = ADI_NOFI;
	switch ( AFE_DATATYPE(value) )
	{
	  case DB_CHR_TYPE:
	  case DB_CHA_TYPE:
	  case DB_TXT_TYPE:
	  case DB_VCH_TYPE:
	  case DB_LTXT_TYPE:
		if ( !AFE_ISNULL(value) )
		{ /* Check for pattern match */
			i4	pm;
			if ( IIAFpmEncode(value, TRUE, &pm) != OK )
			{
				FEafeerr(cb);
				return -1;
			}
			else if ( pm == AFE_PM_FOUND || !prompt )
			{ /* pattern match or program-supplied value */
				oplist.afe_ops = oparr;
				oplist.afe_opcnt = 2;
				oparr[0] = value;
				/* Note:  For string types, the operands of '='
				** must be the same type.  Coercibility is not
				** sufficient.  Hence, conversion is usually
				** the general case.
				*/
				if ( AFE_DATATYPE(&dbv) == AFE_DATATYPE(value) )
				{ /* comparable string types */
					oparr[1] = &dbv;
				}
				else
				{ /* force conversion to a string
						or string of like type */
					cvoplist.afe_ops = &oparr[2];
					cvoplist.afe_opcnt = 1;
					oparr[2] = &dbv;
					vdbv.db_datatype = AFE_DATATYPE(value);
					vdbv.db_length = DB_GW4_MAXSTRING/32;
					vdbv.db_data = (PTR)vd;
					cvfid = IIAFfifConv(cb,&cvoplist,&vdbv);
					if ( cvfid == ADI_NOFI )
					{
						FEafeerr(cb);
						return -1;
					}
					oparr[1] = &vdbv;
				}
				if ( (cmpfid = fiEqCmp(cb, &oplist, &result))
						== ADI_NOFI )
				{
					IIFDerror( E_FR004F_NotComparable, 0 );
					return -1;
				}
				break;
			}
			else
			{ /* value was prompted for, and doesn't contain
			  ** pattern-matching characters:
			  */
				switch ( AFE_DATATYPE(&dbv) )
				{
				  case DB_DTE_TYPE:
				  case DB_ADTE_TYPE:
				  case DB_TMWO_TYPE:
				  case DB_TMW_TYPE:
				  case DB_TME_TYPE:
				  case DB_TSWO_TYPE:
				  case DB_TSW_TYPE:
				  case DB_TSTMP_TYPE:
				  case DB_INDS_TYPE:
				  case DB_INYM_TYPE:
					if ( TRUE /*!chkDateMatch(value)*/ )
						goto _convert;
					else
					{ /* prefix match */
						dbv.db_datatype = DB_CHR_TYPE;
						dbv.db_length = AFE_DATESIZE;
					}
					/* fall through for prefix date match */
				  case DB_CHR_TYPE:
				  case DB_CHA_TYPE:
				  case DB_TXT_TYPE:
				  case DB_VCH_TYPE:
				  case DB_LTXT_TYPE:
					switch ( AFE_DATATYPE(value) )
					{
					  case DB_CHR_TYPE:
					  case DB_CHA_TYPE:
						cvlen = value->db_length;
						cvalue =(u_char*)value->db_data;
						break;
					  default:
					  {
						register DB_TEXT_STRING	*txt =
						(DB_TEXT_STRING*)value->db_data;
						cvlen = txt->db_t_count;
						cvalue = txt->db_t_text;
						break;
					  }
					}
					cmpfid = ADI_NOFI; /* prefix match */
					break;
				  default: /* convert prompt string to type */
				  _convert:
					oplist.afe_ops = &oparr[2];
					oplist.afe_opcnt = 1;
					oparr[2] = value;
					vdbv.db_datatype = dbv.db_datatype;
					vdbv.db_length = dbv.db_length;
	    				vdbv.db_data = (PTR)vd;
					cvfid = IIAFfifConv(cb, &oplist, &vdbv);
					if ( cvfid == ADI_NOFI
						|| afe_clinstd(cb, cvfid,
								&oplist, &vdbv)
							!= OK )
					{
						FEafeerr(cb);
	    					return -1;
					}
					cvfid = ADI_NOFI; /* just converted */
					value = &vdbv; /* new converted value */
					goto _default;
				} /* end column type switch */
				break;
			}
		}
		/* fall through when AFE_ISNULL(value) */
	  default:
	  _default:
		oplist.afe_ops = oparr;
		oplist.afe_opcnt = 2;
		oparr[0] = value;
		oparr[1] = &dbv;
		if ( (cmpfid = fiEqCmp( cb, &oplist, &result )) == ADI_NOFI )
		{
			IIFDerror( E_FR004F_NotComparable, 0 );
			return -1;
		}
		break;
	} /* end input value type switch */
/* # line 675 "findrow.qsc" */	/* inquire_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,currfld,22,fldv->_form,0);
      } /* IIiqset */
    }
/* # line 676 "findrow.qsc" */	/* host code */
	if ( !STequal(currfld, fldv->_field) )
		startrec = 1;
	else
/* # line 679 "findrow.qsc" */	/* getrow */
    {
      if (IItbsetio(2,fldv->_form,fldv->_field,-2) != 0) {
        IItcogetio((short *)0,1,30,4,&startrec,(char *)"_RECORD");
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 681 "findrow.qsc" */	/* host code */
	rowfound = -1;
/* # line 682 "findrow.qsc" */	/* unloadtable */
    {
      if (IItbact(fldv->_form,fldv->_field,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,44,0,&dbv,fldv->_column);
        IItcogetio((short *)0,1,30,4,&currec,(char *)"_RECORD");
        IItcogetio((short *)0,1,30,4,&status,(char *)"_STATE");
        IITBceColEnd();
        {
/* # line 686 "findrow.qsc" */	/* host code */
	    if (status == stUNCHANGED || status == stCHANGE || status == stNEW)
	    { /* good row */
		matched[0] = FALSE;
		if ( cmpfid != ADI_NOFI )
		{
		    if ( AFE_ISNULL(&dbv) && AFE_ISNULL(value) )
			matched[0] = TRUE;
		    else if ( AFE_ISNULL(&dbv) || AFE_ISNULL(value) )
			matched[0] = FALSE;
		    else if ( ( cvfid != ADI_NOFI && /* convert into string */
		    		afe_clinstd(cb, cvfid, &cvoplist, &vdbv) != OK )
		    	|| afe_clinstd(cb, cmpfid, &oplist, &result) != OK )
		    {
			FEafeerr(cb);
/* # line 700 "findrow.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 701 "findrow.qsc" */	/* host code */
		    }
		}
		else if ( !AFE_ISNULL(&dbv) )
		{ /* prefix match string (against non-Null column value)  */
		    i4			lbufr;
		    DB_EMBEDDED_DATA	edv;
		    /* Convert to C string. */
		    edv.ed_type = DB_CHR_TYPE;
		    edv.ed_null = (i2 *)NULL;
		    edv.ed_length = dbv.db_length + 1;
		    edv.ed_data = (PTR)vd;
		    if ( adh_dbcvtev(cb, &dbv, &edv) != E_DB_OK )
		    {
			FEafeerr(cb);
/* # line 716 "findrow.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 717 "findrow.qsc" */	/* host code */
		    }
		    if ( (lbufr = STtrmwhite((char*)vd)) > 0 && lbufr >= cvlen )
		    { /* perform unanchored, case-less match */
			register i4	i;
			register char	*cp = (char *)vd;
			for (i = 0 ; i <= lbufr - cvlen ; ++i, ++cp)
			{
			    if ( CMcmpnocase(cp, cvalue) == 0
				 && STbcompare(cp, lbufr - i, (char *) cvalue,
					cvlen, TRUE) == 0 )
			    {
				matched[0] = TRUE;
				break;
			    }
			} /* end for */
		    }
		}
		if ( matched[0] )
		{   /* Passes check.  If it is before the current row in table,
		    ** simply keep track of it in case none are found after it.
		    */
		    if ( currec <= startrec )
		    { /* Before current row.  Used if none found following. */
			if ( rowfound < 0 )	/* 1st one only */
			    rowfound = currec;
		    }
		    else
		    { /* After the current row.  This is it. */
			rowfound = currec;
/* # line 748 "findrow.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 749 "findrow.qsc" */	/* host code */
		    }
		}
	    }
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 754 "findrow.qsc" */	/* host code */
	if ( rowfound > 0 )
	{ /* Found a row with that one in it */
	    if ( ! noscroll )
    {
      i4 ndrows;
      i4 numrows;
/* # line 761 "findrow.qsc" */	/* inquire_frs */
      {
        if (IIiqset(3,0,fldv->_form,(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&ndrows,32,fldv->_field,0);
          IIiqfsio((short *)0,1,30,4,&numrows,31,fldv->_field,0);
        } /* IIiqset */
      }
/* # line 765 "findrow.qsc" */	/* getrow */
      {
        if (IItbsetio(2,fldv->_form,fldv->_field,1) != 0) {
          IItcogetio((short *)0,1,30,4,&toprec,(char *)"_RECORD");
          IITBceColEnd();
        } /* IItbsetio */
      }
/* # line 766 "findrow.qsc" */	/* getrow */
      {
        if (IItbsetio(2,fldv->_form,fldv->_field,numrows) != 0) {
          IItcogetio((short *)0,1,30,4,&botrec,(char *)"_RECORD");
          IITBceColEnd();
        } /* IItbsetio */
      }
/* # line 768 "findrow.qsc" */	/* host code */
		/*
		**  If the found row is already visible, just move
		**  the cursor there.  Otherwise, if the found row is
		**  above the top displayed row, then we need to
		**  calculate things differently due to a change
		**  in the table field scroll algorithm.
		*/
		if (rowfound < toprec)
		{
			currec = max(rowfound - (numrows*1/4), 1);
		}
		else if (rowfound > botrec)
		{
			currec = min(rowfound+(numrows*3/4), ndrows);
		}
		else
		{
			currec = 0;
			IIrowset( fldv->_form, fldv->_field,
					rowfound - toprec + 1, fldv->_column
			);
		}
		if ( currec != 0 )
		{
/* # line 792 "findrow.qsc" */	/* scroll */
      {
        if (IItbsetio(1,fldv->_form,fldv->_field,-3) != 0) {
          IItbsmode((char *)"to");
          if (IItscroll(0,currec) != 0) {
          } /* IItscroll */
        } /* IItbsetio */
      }
/* # line 793 "findrow.qsc" */	/* getrow */
      {
        if (IItbsetio(2,fldv->_form,fldv->_field,numrows) != 0) {
          IItcogetio((short *)0,1,30,4,&currec,(char *)"_RECORD");
          IITBceColEnd();
        } /* IItbsetio */
      }
/* # line 796 "findrow.qsc" */	/* host code */
			IIrowset( fldv->_form, fldv->_field,
				numrows - (currec - rowfound), fldv->_column
			);
		}
    }
	    return rowfound;
	}
	else
	{
	    if ( prompt )
	    	IIUGmsg(ERget(S_FR0030_Cannot_find), TRUE, 1, last.word);
	    return -1;
	}
  }
/*
** Name:	fiEqCmp() -	Get Comparison (Equality) Function Instance ID.
**
** Description:
**	Get the ADF function instance ID for comparing two data values for
**	equality.  The data values are input using an AFE operand list along
**	with the result, which should always be a boolean.
**
** Input:
**	cb	{ADF_CB *}  The ADF control block.
**	ops	{AFE_OPERS *}  The operands to be compared.
**	result	{DB_DATA_VALUE *}  The boolean result.
**
** Returns:
**	{ADI_FI_ID}  The equality comparison function instance ID;
**		     otherwise ADI_NOFI on error.
**
** History:
**	08/89 (jhw) -- Written.
*/
static ADI_FI_ID
fiEqCmp ( cb, ops, result )
ADF_CB		*cb;
AFE_OPERS	*ops;
DB_DATA_VALUE	*result;
{
	static ADI_OP_ID	eqopid = ADI_NOOP;	/* shared constant */
	ADI_FI_DESC	fdesc;
	/* Get ADI operator ID first and then the f.i. ID */
	if ( ( eqopid == ADI_NOOP && afe_opid(cb, "=", &eqopid) != OK )
			|| afe_fdesc(cb, eqopid, ops, (AFE_OPERS *)NULL,
					result, &fdesc) != OK )
	{
		if ( cb->adf_errcb.ad_errcode != E_AF6006_OPTYPE_MISMATCH )
			FEafeerr(cb);
		return ADI_NOFI;
	}
	return fdesc.adi_finstid;
}
