/*
** Copyright (c) 1989, 2004 Ingres Corporation
*/

# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<adf.h>
# include	<afe.h>
# include	<er.h>
# include	<lo.h>
# include	<ug.h>
# include	<cv.h>
# include	<ooclass.h>
# include	<abclass.h>
# include	<metafrm.h>
# include	<erfg.h>
# include	<si.h>
# include	"framegen.h"

#if defined(i64_win)
#pragma optimize("s", on);
#endif

/**
** Name:	fghidfld.c -	declare hidden fields and table field columns.
**
** Description:
**	Generate hidden field/column declarations:
**	1. Print user defined hidden columns from MFVAR in METAFRAME.
**
**		uservarName=date_type nullability defaultability, ** comment**
**
**	2. Traverse MFTAB & MFCOL for every table in METAFRAME and write out
**	hidden field/column declarations for MFCOL entries that have
**	a non-NULL utility PTR. Format for 4gl hidden variable declarations:
**
**		iih_varname=data_type nullability defaultability,
**		tblfldname.iih_colname=data_type nullability defaultability,
**
**	3. For UPDATE frames, generate Flag fields to hold NULLable key
**	indicators. This is only done if a primary table has NULLable keys.
**	4. Create hidden fields for columns with the COL_VARIABLE flag set.
**
**	This file defines:
**
**	IIFGhidflds
**	gen_hid		Generate hidden fields from METAFRAME (hidden PKs, etc).
**	genu_hid (OBSOLETE) Generate user specified hidden fields (from MFVAR).
**	set_typebuf	Write data type info to a buffer.
**
** History:
**	5/31/89 (pete)	Written.
**	11/6/89 (pete)	Change code to reflect change of MFVAR.dtype from
**			a DB_DATA_VALUE to a (char *).
**	3/16/90	(pete)	Added support for iiNullKeyFlag fields.
**	1/91 	(Mike S) Process COL_VARIABLE flag.
**	6-dec-92 (blaise)
**		Added generation of hidden fields for optimistic locking
**		columns
**	9-sep-93 (cmr) Fix for bug #54257.  Get rid of superfluous optimistic
**		locking code (existing code covers *all* hidden field cases)
**		and clarified misleading comments & code.
**		
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	11-may-2004 (somsa01)
**	    Added LEVEL1_OPTIM for i64_win to prevent vis06.sep and vis11.sep
**	    from failing due to incorrect source code generation: an incorrect
**	    number of iiNullKeyFlag definitions are generated by IIFGhidflds().
**/

/* # define's */
/*
** when generating hidden field declarations, what type of hidden field are
** we generating:
*/
# define HID_FROMVQ	0	/* column marked 'v' on visual query */
# define HID_OTHER	1	/* hidden version of primary key, join fld, */
				/* seq fld, lookup, opt locking col, etc.   */

/* GLOBALDEF's */

/* extern's */
GLOBALREF FILE *outfp;
GLOBALREF METAFRAME *G_metaframe;
GLOBALREF i4    IifgMstr_NumNulKeys;	/* Nmbr Nullable keys; set in fginit.c*/
GLOBALREF i4    IifgDet_NumNulKeys;	/* Nmbr Nullable keys; set in fginit.c*/

FUNC_EXTERN VOID	IIFGbsBeginSection ();
FUNC_EXTERN VOID	IIFGesEndSection ();
FUNC_EXTERN bool	IIFGnkf_nullKeyFlags();
FUNC_EXTERN IITOKINFO	*IIFGftFindToken();

/* static's */
char type_buf[DB_TYPE_SIZE + 10];	/* Holds complete type name.
					** '+ 10' is length of " with null".
					*/
static READONLY char _optim[]	= ERx("optimistic");

static VOID gen_hid();
/*static VOID genu_hid(); */
static STATUS set_typebuf();
static i4     Hid_cnt;
static bool   vqLocals;

/*{
** Name:	IIFGhidflds - declare hidden fields & table field columns.
**
** Description:
**
** Inputs:
**	i4	nmbr_words;	** number of items in 'p_wordarray'
**	char	*p_wordarray[]; ** words in command line
**	char	*p_wbuf;	** left margin padding for output line
**	char	*infn;		** name of input file currently processing
**	i4	line_cnt;	** current line number in input file
**
** Outputs:
**	write generated declaration lines to 'outfp'.
**
**	Returns:
**		STATUS
**
**	Exceptions:
**		NONE
**
** Side Effects:
**
** History:
**	5/31/89 (pete)	Written.
**	26-aug-92 (blaise)
**		Added code to generate declarations for local procedures,
**		which are now also stored as MFVARs in the metaframe.
**	21-apr-94 (connie) Bug #62286
**		Moved code for generating the declarations for local
**		procedures to fgprocs.c
*/
STATUS
IIFGhidflds(nmbr_words, p_wordarray, p_wbuf, infn, line_cnt)
i4	nmbr_words;
char	*p_wordarray[];
char	*p_wbuf;
char	*infn;		/* name of input file currently processing */
i4	line_cnt;	/* current line number in input file */
{
	register i4  i;
	register MFTAB **p_mftab;
	register MFVAR **p_mfvar = G_metaframe->vars;
/*	USER_FRAME *p_uf = (USER_FRAME *) G_metaframe->apobj;	*/
	IITOKINFO	*p_iitok;


	if (nmbr_words != 0)	/* "##generate hidden_fields" already stripped*/
	{
	    IIFGerr (E_FG002A_WrongNmbrArgs, FGCONTINUE, 2,
				(PTR) &line_cnt, (PTR) infn);
	    /* A warning; do not return FAIL -- processing can continue. */
	}

	/*
	** Loop through all MFVARs in the metaframe, which are ordered with
	** all the local varables first, then the local procedures. Create
	** declaration statements for each variable and procedure.
	*/

	/* First the local variables */
	for (i=0; i < G_metaframe->numvars && (*p_mfvar)->vartype == VAR_LVAR;
				i++, p_mfvar++)
	{
	    if (i == 0)	/* precede first hidden field with comment */
	    {
		IIFGbsBeginSection (outfp, ERget(F_FG0014_Hidden),
					(char *)NULL, FALSE);
		SIfprintf(outfp, ERx("%s%s\n"), p_wbuf,
			ERget(F_FG0011_HidnFldComment));
	    }


	    /* write out a 4gl hidden field declaration statement. */

	    SIfprintf (outfp, ERx("%s%s = %s,\t/* %s */\n"),
	        p_wbuf, (*p_mfvar)->name, (*p_mfvar)->dtype,
		(*p_mfvar)->comment );
	}

	if (i > 0)	/* follow last hidden field with Section End */
	    IIFGesEndSection (outfp, ERget(F_FG0014_Hidden), 
			      (char *)NULL, FALSE);

	/*
	** Loop looking for columns marked 'v'
	*/
	for (i=0, Hid_cnt = 0, vqLocals = FALSE, p_mftab = G_metaframe->tabs;
	     i < G_metaframe->numtabs; 
	     i++, p_mftab++)
	{
	    	if (  (*p_mftab)->tabsect == TAB_MASTER
		   && (G_metaframe->mode & MFMODE) != MF_MASTAB
		   )
	    	{
			vqLocals = TRUE;
		        gen_hid(p_mftab, p_wbuf, ERx(""), HID_FROMVQ);
	    	}
	    	else	/* TAB_DETAIL */
	    	{
			vqLocals = TRUE;
		        gen_hid(p_mftab, p_wbuf, TFNAME, HID_FROMVQ);
	    	}
	}

	/*
	** Loop thru all MFTABs in METAFRAME & create hidden field 4gl stmts.
	** Skip LookUp tables, since we don't generate Hiddens for them.
	** Only do this loop for Append & Update frames.
	*/
	for (i=0, Hid_cnt = 0, p_mftab = G_metaframe->tabs; 
	     i < G_metaframe->numtabs; 
	     i++, p_mftab++)
	{
	    	if (  (*p_mftab)->tabsect == TAB_MASTER
		   && (G_metaframe->mode & MFMODE) != MF_MASTAB
		   )
	    	{
		        gen_hid(p_mftab, p_wbuf, ERx(""), HID_OTHER);
	    	}
	    	else	/* TAB_DETAIL */
	    	{
		        gen_hid(p_mftab, p_wbuf, TFNAME, HID_OTHER);
	    	}
	}


	/* if we need iiNullKeyFlag fields, then generate them */
	if (IIFGnkf_nullKeyFlags())
	{
	    i4  maxNulKeys;

	    /* Find how many NULLable-key Flag fields to declare.
	    ** (the IifgMstr, IifgDetNumNulKeys counts are set in fginit.c)
	    */
	    maxNulKeys = (IifgMstr_NumNulKeys > IifgDet_NumNulKeys)
		     ? IifgMstr_NumNulKeys : IifgDet_NumNulKeys;

	    /* create NULLable-key Flag fields (hidden fields) */
	    for (i=0; i < maxNulKeys; i++)
	    {
	        if (i == 0)
	        {
		    /* preceed first one with a comment */
		    SIfprintf(outfp, ERx("\n%s%s\n"), p_wbuf,
			ERget(F_FG0036_NullKeyComment));
	        }

	        SIfprintf (outfp, ERx("%s%s%d = INTEGER(2) NOT NULL,\n"),
	            p_wbuf, ERget(F_FG0037_FlagFieldName), i+1);
	    }
	}

	return OK;
}
/*{
** Name:    gen_hid - Generate hidden fields/columns for a table in METAFRAME.
**
** Description:
**	Generate hidden field declarations from MFTAB in METAFRAME. These
**	are either the hidden fields that are needed by the code generator 
**	(hidden versions of primary keys, etc) or the columns marked 'v' in the
**	VQ.
**
**	Write out lines like:
**		iih_varname = data_type nullability,	** master section
**   or		iitf.iih_varname = data_type nullability, ** detail section
**
** Inputs:
**	p_mftab		**pointer to MFTAB of table to process.
**	p_wbuf		pointer to whitespace chars to prepend on output.
**	p_tfname	pointer to table field name (if any) to prepend to
**			  variable name in generated declaration.
**	hid_type	what type of hidden fields are we generating:
**			HID_FROMVQ	columns marked 'v' on visual query
**			HID_OTHER	primary keys, join fields, etc.
**
** Outputs:
**	4gl hidden field declarations written to FILE 'outfp' (a global).
**
**	Returns:
**		VOID
**
**	Exceptions:
**		NONE
**
** Side Effects:
**
** History:
**	6/1/89	(pete)	Written.
**	1/25/90 (pete)  Write out comment prior to declarations, about
**			this batch of variables being hidden versions
**			of keys & join fields.
**	5/1/91 (pete)	Fix indenting of local variable definition comment.
**	6-dec-92 (blaise)
**		Support for optimistic locking: added the capability to
**		generate hidden versions of optimistic locking columns as a
**		separate declaration section. Changed fourth parameter from
**		bool to nat.
**	20-apr-93 (blaise)
**		Fixed bug #50320: incorrect if clause meant that declarations
**		weren't being generated for hidden versions of primary keys.
**	09-sep-93 (cmr) Don't special case optimistic locking.
*/
static VOID
gen_hid(p_mftab, p_wbuf, p_tfname, hid_type)
MFTAB	**p_mftab;
char	*p_wbuf;
char	*p_tfname;
i4	hid_type;
{
	register MFCOL **p_mfcol = (*p_mftab)->cols;
	register i4  i;
	char *p_varname;
	STATUS stat;

	/* loop thru all columns for this table */
	for (i=0; i < (*p_mftab)->numcols ; i++, p_mfcol++)
	{
		if (hid_type == HID_FROMVQ)
		{
			p_varname = (*p_mfcol)->alias;
			if (((*p_mfcol)->flags & (COL_USED|COL_VARIABLE)) 
				!= COL_VARIABLE)
			continue;	/* Displayed or wholly unused */
		}
		else if (hid_type == HID_OTHER)
		{
			/* utility ptr set in meta_init() in fginit.c */
			p_varname = (*p_mfcol)->utility;

			if ((p_varname == NULL) || (*p_varname == EOS))
			continue;    /* No utility pointer -- ergo no hidden */
		}

		if ( (stat = set_typebuf( &((*p_mfcol)->type )) ) != OK)
		{
			IIFGerr (E_FG001D_BadInternalType, FGFAIL | FGSETBIT,
				3, (PTR) (*p_mfcol)->name,
				(PTR) (*p_mftab)->name, (PTR) &stat );
		}

		/* Assertion: type_buf is all set.
		** Now write out a 4gl hidden field declaration statement.
		*/

		if (Hid_cnt == 0)	/* preceed first hidden with comment */
		{
			Hid_cnt++;
			if (hid_type == HID_FROMVQ)
			{
				SIfprintf(outfp, ERx("%s%s\n"),
					p_wbuf,
					ERget(F_FG003B_VQhidfldComnt));
			}
			else
			{
				SIfprintf(outfp, ERx("%s%s%s\n"),
					vqLocals ? ERx("\n"): ERx(""),
					p_wbuf,
					ERget( F_FG0031_HidnKeyFldComment));
			}
		}

		if ( (p_tfname == NULL) || (*p_tfname == EOS) )
		{
			/* generate simple hidden field */

			/* NOTE: SIfprintf doesn't return an error STATUS! */
			SIfprintf (outfp, ERx("%s%s = %s,\n"),
			p_wbuf, p_varname, type_buf);
		}
		else
		{
			/* generate table field hidden column */

			SIfprintf (outfp, ERx("%s%s.%s = %s,\n"),
				p_wbuf, p_tfname, p_varname, type_buf);
		}
	}
}

#if 0
/*{
** Name:	genu_hid - generate user defined hidden fields.
**
** Description:
**	Loop thru user defined hidden fields and columns in MFVAR array
**	and generate 4gl declarations. Generate 4gl statements like:
**
**	    uservarName=date_type nullability, ** comment **
**
** Inputs:
**	p_mfvar		pointer to MFVAR to process.
**	p_wbuf		pointer to white space buffer to prepend on output.
**
** Outputs:
**	4gl hidden field/column declarations written to FILE outfp (global).
**
**	Returns:
**		VOID
**
**	Exceptions:
**		NONE
**
** Side Effects:
**
** History:
**	6/1/89	(pete)	Written.
*/
static VOID
genu_hid(p_mfvar, p_wbuf)
MFVAR **p_mfvar;
char   *p_wbuf;
{
	STATUS stat;

	if (stat = set_typebuf( &((*p_mfvar)->dtype)) != OK)
	{
	    IIFGerr (E_FG001E_BadInternalType2, FGFAIL | FGSETBIT,
		2, (PTR) (*p_mfvar)->name, (PTR) &stat );
	}

	/* Assertion: type_buf is all set.
	** Now write out a 4gl hidden field declaration statement.
	*/
	SIfprintf (outfp, ERx("%s%s = %s,\t/* %s */\n"),
	    p_wbuf, (*p_mfvar)->name, type_buf, (*p_mfvar)->comment );
}
#endif

/*{
** Name:	set_typebuf - put complete type info into buffer.
**
** Description:
**	Generate a complete type description, including WITH NULL, or
**	NOT NULL, based on a DB_DATA_VALUE. Put the textual type
**	description into a global, static buffer.
**
** Inputs:
**	DB_DATA_VALUE *p_dbdv;		base the type description on this.
**
** Outputs:
**	sets global static buffer, 'type_buf'.
**
**	Returns:
**		STATUS
**
**	Exceptions:
**		NONE
**
** Side Effects:
**
** History:
**	6/1/89 (pete)	Written.
*/
static STATUS
set_typebuf(p_dbdv)
DB_DATA_VALUE *p_dbdv;
{
	DB_USER_TYPE utype;
	DB_STATUS stat;

	/* initialize values in 'utype' based on p_dbdv */
	if ((stat = afe_tyoutput(FEadfcb(), p_dbdv, &utype)) != OK)
	{
	    return (STATUS) stat;
	}

	/* tack "with/not null" onto the type name created above */

	if ( utype.dbut_kind == DB_UT_NULL )
	    STpolycat (2, utype.dbut_name, ERx(" WITH NULL"), type_buf);
	else
	    STpolycat (2, utype.dbut_name, ERx(" NOT NULL"), type_buf);

	CVupper (type_buf);	/* generate UPPER CASE 4gl keywords */

	return OK;
}