# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include 	<st.h>
# include 	<me.h>
# include 	<cm.h>
# include	<adf.h>
# include	<ft.h>
# include	<fmt.h>
# include 	<frame.h>
# include 	<ooclass.h>
# include 	<er.h>
# include	<erug.h>
# include <metafrm.h>
/*
# include 	<metafrm.h>    
.......so ming knows about .h dependancy 
*/
# include	"tabinfo.h"
# include	"ervq.h"
/**
** Name:	vqmakfrm.qsh -	given a METAFRAME, create frame for display
**
** Description:
**	Given a METAFRAME create a frame structure which will contain
**	all of the visual elements of the visual query. This file
**	contains all of the language dependent elements of the 
**	the dynamic frame creation process.
**	
**	IIVQvmfVqMakeFrame	- given a metaframe, create the frame 
** 					to display it.
**	IIVQvlfVqLoadFrame	- load the table fields of the visual
**					query frame
**	IIVQlftLoadFrameTable	- load one table field of the visual
**					query frame
**	
** History:
**	2/16/89  (tom) - created
**      29-sep-96 (mcgem01)
**              Global data moved to vqdata.c
**
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/* # define's */
# define SIMP_FLD_ATTR ((i4) (fdLOWCASE | fdNOAUTOTAB))
# define TCWID 19	/* width of the table field column
			   that displays the table column
			   of in the visual query */ 
# define TITLN  FE_MAXNAME /* length of title column text for lookup table */
  GLOBALREF char *IIVQform;
  GLOBALREF char *IIVQcform;
GLOBALREF char *IIVQtfname_fmt; /* format string to form a table field name */
GLOBALREF char IIVQcol_col[4]; /* column names */
GLOBALREF char IIVQdir_col[4];
GLOBALREF char IIVQexp_col[4];
GLOBALREF char IIVQinc_col[4];
GLOBALREF char IIVQord_col[4];
GLOBALREF char IIVQsrt_col[4];
GLOBALREF i4  IIVQjtab;		/* index for join table and column... 
				   a value of -2 means that we are not
				   currently in a join insertion */
GLOBALREF i4  IIVQjcol;		/* ..used during a join insertion */
GLOBALREF char *IIVQ_d_text;	/* stands for descending and ascending */
GLOBALREF char *IIVQ_a_text;
GLOBALREF char *IIVQccCompCursor; /* field for the cursor to be on while 
				   on the compressed form */
/* extern's */
FUNC_EXTERN bool IIVQlftLoadFrameTable();
FUNC_EXTERN FRAME *IIFDofOpenFrame();
FUNC_EXTERN TBLFLD *IIFDatfAddTblFld();
FUNC_EXTERN REGFLD *IIFDacfAddCharFld();
FUNC_EXTERN FLDCOL *IIFDaccAddCharCol();
FUNC_EXTERN char *FEsalloc();
FUNC_EXTERN PTR IIVQ_VAlloc();
FUNC_EXTERN char *IIVQgivGetIncVal();
FUNC_EXTERN bool IIVQivsInitVQStrings();
/* static functions */
# define LOC static
LOC bool vq_MakeFrameTable();
LOC bool vq_LoadAppTable();
LOC bool vq_LoadUpdTable();
LOC bool vq_LoadDispTable();
LOC bool vq_LoadLupTable();
LOC VOID vq_LoadSimple();
LOC VOID vq_ColDisp();
LOC VOID vq_AddVal();
LOC bool vq_PutJoins();
LOC i4  vq_PostLab();
LOC VOID vq_JoinLab();
/*}
** Name:	JOIN_LAB - join label structure
**
** Description:
**	This structure defines a label which is posted
**	so as to identify joins in a visual query.
**	See the function vq_PostLab().
**
** History:
**	03/24/89  (tom) - written	
*/
typedef struct
{
	i4	line;		/* frame line of the label */
	i4	col;		/* frame column of the label */
	char	lab[6];		/* label text */
} JOIN_LAB;
/*}
** Name:	TABPOS	- position of table structure
**
** Description:
**	This structure gives the position of a visual query table
**	on the frame.
**	The only instances of this structure are found in the 
**	tabpos array in this file.
**
** History:
**	03/24/89 (tom) - written
*/
typedef struct
{
	i4	line;		/* line of the frame the first column
				   of the table is positioned on */
	i4	col;		/* the frame column that the table starts at */
} TABPOS;
LOC TABPOS tabpos[MF_MAXTABS];	/* an array of frame coordinate 
				   positions of each of the tables */
/* if there are too many join lines.. then we use trunk lines which
   run at the column positions given in the runcoltab table, which
   is indexed by the join type */
static i4  runcoltab[3] = { 0, 2, 4 }; 
/* various other language independent strings, and variables */
static char *_nullstring = ERx("");	/* null string */
/*************************
   the following items (plus the name member in the TABINFO structure) 
   are language dependent, and read in from the message file by 
   IIVQivsInitVQStrings 
*************************/
static char *_table_text;
static i2 _ins_tit_col;
static char *_ins_text = (char*)NULL;	/* nulled for first time test */
static i2 _del_tit_col;
static char *_del_text;
static i2 _qua_tit_col;
static char *_qua_text;
static char *_sort_text;
static char *_aexp_text;
static char *_asgn_text;
static char *_rest_text;
static char *_lord_text;
static char *_ltit_text;
static char *_luptit_text;
static char *_join_text;
static char *_detail_tf;
static char *_master_tf;
GLOBALDEF char *IIVQ_y_text;
GLOBALDEF char *IIVQ_n_text;
GLOBALDEF char *IIVQ_v_text;
static char *_incptr_text;
static char *_inccol_text;
static char *_alphatab;
static i4  _alphasize;
static char *_dir_val;
static char * _dir_vmsg;
static char *_yn_val;
static char *_yn_vmsg;
GLOBALDEF TABINFO tab_info[] = 
{    
    {
	(char*)NULL,					/* append table */
	3,						/* .. has 3 columns */	
	{   /*name         width   dlen    flags  */
	    { IIVQcol_col, TCWID, TCWID,  fdtfCOLREAD },
	    { IIVQinc_col,    1,   1,     SIMP_FLD_ATTR },
	    { IIVQexp_col,   29, 100,     fdNOAUTOTAB }
	}
    },
    {
	(char*)NULL,					/* upate table */
	5,						/* .. has 5 columns */	
	{   /*name         width   dlen    flags  */
	    { IIVQcol_col, TCWID, TCWID,  fdtfCOLREAD },
	    { IIVQinc_col,    1,   1,     SIMP_FLD_ATTR },
	    { IIVQexp_col,   29, 100,     fdNOAUTOTAB },
	    { IIVQsrt_col,    2,   2,     fdNOAUTOTAB },
	    { IIVQdir_col,    1,   1,     SIMP_FLD_ATTR }
	}
    },
    {
	(char*)NULL,					/* display table */
	5,						/* .. has 5 columns */	
	{   /*name         width   dlen    flags */
	    { IIVQcol_col, TCWID, TCWID,  fdtfCOLREAD },
	    { IIVQinc_col,    1,   1,     SIMP_FLD_ATTR },
	    { IIVQexp_col,   29, 100,     fdNOAUTOTAB },
	    { IIVQsrt_col,    2,   2,     fdNOAUTOTAB },
	    { IIVQdir_col,    1,   1,     SIMP_FLD_ATTR }
	}	
    },
    {
	(char*)NULL,					/* lookup table */
	4,						/* .. has 4 columns */	
	{   /*name         width   dlen    flags  */
	    { IIVQcol_col, TCWID, TCWID,  fdtfCOLREAD },
	    { IIVQinc_col,    1,   1,     SIMP_FLD_ATTR },
	    { IIVQord_col,    3,   3,     fdNOAUTOTAB },
	    { IIVQexp_col,   16,  TITLN,  fdNOAUTOTAB }
	}
    }
};
/*{
** Name:	IIVQvmfVqMakeFrame	- create a frame struct
**					to display a METAFRAME struct 
**
** Description:
**	This function allocates and initializes a frame structure
**	which will be presented to the developer. This constitutes
**	dynamic form/frame creation.
**	The frame struct is allocated under the current FE tag.
**
** Inputs:
**	METAFRAME *mf;  - the metaframe structure	
**	bool compressed;	- compressed view flag
**	i4 curtab;		- index of current table, so we
**				  can highlight the name 
**
** Outputs:
**	Returns:
**	FRAME *		- pointer to newly allocated and initialized
**			  frame structure.  NULL if there is a failure.
**
**	Exceptions:
**		Memory allocations can fail.	
**
** Side Effects:
**		None
**
** History:
**		
**	2/13/89 (tom) - created
*/
FRAME*
IIVQvmfVqMakeFrame(mf, compressed, curtab)
METAFRAME *mf;
bool compressed;
i4  curtab;
{
	register i4	i;
	FRAME	*fr;
	MFTAB	*tab;	
	i4	line;
	i4	col;
	i4	len;
	bool	seen_detail;
	static i4  id2_len = -1;
	char	buf[100];
	/* Initialize VQ operations */
	if (IIVQivsInitVQStrings() != TRUE)
	{
		return (NULL);
	}	
	/* open a dynamic frame */
	if ((fr = IIFDofOpenFrame(compressed ? IIVQcform : IIVQform, 
			(PTR)NULL)) == NULL) 
		return (NULL);
	/* if this is master data in a table field then indicate it */
	if ((mf->mode & MFMODE) == MF_MASTAB)
	{
		len = STlength(_master_tf);
		IIFDabtAddBoxTrim(fr, 0, 44, 
				0, 44+len+10, (i4)0);
		IIFDattAddTextTrim(fr, 0, 49, 0, _master_tf);
	}
	/* output frame identifiers */
	IIFDattAddTextTrim(fr, 0, 0, (i4)fd2COLOR, ERget(F_VQ00F9_VQ_ident));
	IIFDattAddTextTrim(fr, 1, 0, (i4)0, ERget(F_VQ00FA_VQ_id2));
	if (id2_len < 0)
	{
		id2_len = STlength(ERget(F_VQ00FA_VQ_id2));
	}
	IIFDattAddTextTrim(fr, 1, id2_len, (i4)fd1COLOR, mf->apobj->name);
	/* now we determine where to place the left side of the
	   tables.. this must correspond with the runcolumn 
	   computation in vq_PutJoin() */
	i = mf->numjoins;
	if (compressed || i >= 8) /* using branches to a trunk, "runcoltab" */ 
		col = 10;
	else if (i >= 5)	/* 1 col per join */
		col = i + 8; 
	else			/* 2 col per join, spread out */
		col = i * 2  + 8;
	/* set flag to say that we have not crossed over into detail section */
	seen_detail = FALSE;
	/* for each of the tables in the metaframe.. */
	for (line = 2, i = 0; i < mf->numtabs; i++)
	{
		tab = mf->tabs[i];		
		/* check for transition between  master and details */
		if (seen_detail == FALSE && tab->tabsect == TAB_DETAIL)
		{
			seen_detail = TRUE;
			/* paint the dividing line between the master
			   and detail section */
			len = STlength(_detail_tf);
			IIFDabtAddBoxTrim(fr, line-1, 44, 
					line-1, 44+len+10, (i4)0);
			IIFDattAddTextTrim(fr, line-1, 49, 0, _detail_tf);
			/* indent the detail tables */
			col += 6;
		}
		/* put a visual representation of the table onto the form */
		if (vq_MakeFrameTable(fr, tab, i, &line, col, 
				compressed, curtab, mf->mode & MFMODE) != TRUE)
			return (NULL);
	}
	/* put the join lines on the frame */
	vq_PutJoins(mf, fr, compressed);
	/* post frame struct version */
	fr->frversion = FRMVERSION;
	/* close the frame dynamic frame definition */
	if (IIFDcfCloseFrame(fr) == FALSE)
		return (NULL);
	return (fr);
}
/*{
** Name:	vq_MakeFrameTable	- add a table to the frame struct 
**
** Description:
**	This function adds and initializes a new table into the 
**	frame structure that is being built up by IIVQvqVqMakeFrame.
**	This function is called for each table in a visual query.
**	Tables which are part of the visual query are represented as
**	a titleless table field on the screen.
**	Different types of tables can be built and the tab_info
**	structure supplies information about their formatting.
**	This function expects that the tab_info structure has been
**	initialized.
**	
**
** Inputs:
**	FRAME *fr;  	- the frame to add to struct pointer
**			  (updated by calls to dynamic frame creation code)
**	MFTAB *tab;  	- the table to be added struct pointer (read only) 
**	i4 *lin;  	- line number to place the table field on
**			  (we update the caller's line number) 
**	i4 col;  	- # of columns to indent the table field
**	bool compressed;	- compressed view flag
**	i4 curtab;		- index of current table, so we
**				  can highlight the name 
**
** Outputs:
**	Returns:
**
**	bool		- TRUE if success, else FALSE
**
**	Exceptions:
**		Memory allocations may fail.
**
** Side Effects:
**		The frame structure is updated.
**
** History:
**		
**	2/13/89 (tom) - created
*/
LOC bool
vq_MakeFrameTable(fr, tab, tabnum, lin, col, compressed, curtab, mode)
reg FRAME *fr;
reg MFTAB *tab;	
i4  tabnum;
i4	*lin;
reg i4	col;
bool compressed;
i4  curtab;
i4  mode;
{
	TABINFO *ti;		/* ptr to the table info for a table type */
	TBLFLD *tf;		/* ptr to table field */
	TCINFO *tc;
	FLDCOL *fc;
	REGFLD *fl;
	i4	attr;
	register i4  line;
	i4	i;		/* loop index */
	i4	len;		/* loop index */
	char	tabid[TCWID+100];	/* string to form dummy names */
	line = *lin;
	/* get ptr to table type format info */
	ti = &tab_info[tab->usage];
	/* post the table's position, we want the position of the
	   first row of column info */
	tabpos[tabnum].line = line + (compressed ? 1 : 3);
	tabpos[tabnum].col = col;
	/* construct box trim at the top of the table field which
	   surrounds the table's name */
	if (IIFDabtAddBoxTrim(fr, line, col, line + 2, col + TCWID + 1, (i4)0)
			== FALSE)
		return (FALSE);
	/* truncate the table name */
	len = CMcopy(tab->name, TCWID - 2, tabid);
	if (len != STlength(tab->name))
	{
		tabid[len++] = ERx('.');
		tabid[len++] = ERx('.');
	}
	tabid[len] = EOS;
	/* if we are currently doing a compressed view, and this is 
	   the current table then we need to highlight the trim. */
	attr = (i4)(compressed && curtab == tabnum ? fdRVVID : 0); 
	/* put in the name of the table */
	if (IIFDattAddTextTrim(fr, line + 1, col + 1, attr, tabid)
			== FALSE)
		return (FALSE);
	/* put in the table type text */
	if (IIFDattAddTextTrim(fr, line, col + TCWID - STlength(ti->name) + 1,
				(i4)0, ti->name)
			== FALSE)
		return (FALSE);
	/* if it is a compressed view.. then we put in the 
	   field which will allow the frame to scroll and we are finished */
	if (compressed)
	{
		if (curtab == tabnum)
		{
			if ((fl =IIFDacfAddCharFld(fr, IIVQccCompCursor, 
				line + 1, col + TCWID + 3, _nullstring,
				line + 1, col + TCWID + 2, 1, 1, 1, 
				(i4)fdNOECHO)) == NULL)
				return (FALSE);
		}
		/* bump the line number that the next table starts on */
		*lin +=  4;
		return (TRUE);
	}
	/* put in trim to point to the form inclusion field, this
	   is the same for all table types */
	if (IIFDattAddTextTrim(fr, line + 1, col + TCWID + 3, (i4)0, 
			_incptr_text) == FALSE)
		return (FALSE);
	/* form a dummy table field name,
	   NOTE: we set up the field names so that we can know, upon
		 field activations, what table and field is being refered to,
		 this is done by encoding the table number into each field 
		 and then using further encodings to indicate insert, delete
		 qualification fields by posting 'i', 'd', or 'q'
		 into the tabid[4] character position. */
	STprintf(tabid, IIVQtfname_fmt, tabnum);
	tabid[5] = EOS; /* terminate encoded string */
	/* first put in the simple fields so that they are tabbed to first */
	/* switch on the type and add miscelaneous fields associated
	   with various kinds of tables */
	switch (tab->usage)
	{
	case TAB_UPDATE:
		/* add the "Insert (y/n):" field */
		tabid[4] = 'i'; 
		if ((fl =IIFDacfAddCharFld(fr, tabid, 
			line+0, col + _ins_tit_col, _ins_text,
			line+0, col + 56, 1, 1, 1, SIMP_FLD_ATTR))
				== NULL)
			return (FALSE);
		/* add the "Delete (y/n):" field */
		tabid[4] = 'd';
		if ((fl = IIFDacfAddCharFld(fr, tabid, 
			line+1, col + _del_tit_col, _del_text,
			line+1, col + 56, 1, 1, 1, SIMP_FLD_ATTR))
				== NULL)
			return (FALSE);
		/* fall thru */
	default:
		/* put in the "Display on Form" column label */
		if (IIFDattAddTextTrim(fr, line, col + TCWID + 4, (i4)0, 
				_inccol_text) == FALSE)
			return (FALSE);
		break;
	case TAB_LOOKUP:
		/* add the "Qualification" field */
		tabid[4] = 'q';
		if ((fl = IIFDacfAddCharFld(fr, tabid, 
			line+1, col + _qua_tit_col, _qua_text,
			line+1, col + 56, 1, 1, 1, SIMP_FLD_ATTR)) 
				== NULL)
			return (FALSE);
		/* put in more trim to point to the "Display" field */
		if (IIFDattAddTextTrim(fr, line, col + TCWID + 4, (i4)0, 
				_incptr_text) == FALSE)
			return (FALSE);
		/* put in the "Display on Form" column label */
		if (IIFDattAddTextTrim(fr, line - 1, col + TCWID + 5, (i4)0, 
				_inccol_text) == FALSE)
			return (FALSE);
		/* put in trim to point to the "Order in Popup" label */
		if (IIFDattAddTextTrim(fr, line + 1, col + TCWID + 6, (i4)0, 
				_incptr_text) == FALSE)
			return (FALSE);
		/* put in the "Order in Popup" label */
		if (IIFDattAddTextTrim(fr, line, col + TCWID + 7, (i4)0, 
				_lord_text) == FALSE)
			return (FALSE);
		break;
	}
	tabid[4] = EOS; /* terminate encoded string */
	/* create the table field which represents the table */
	if ((tf = IIFDatfAddTblFld(fr, tabid, line + 2, col, 
			tab->numcols, FALSE, FALSE)) == NULL)
		return (FALSE);
	/* for each column to be in the table field */
	for (i = 0; i < ti->numcols; i++)
	{
		/* get pointer to column formatting info */
		tc = &ti->cols[i];
		/* add the column to the table field */
		if ((fc = IIFDaccAddCharCol(fr, tf, tc->name, _nullstring, 
				tc->width, 1, tc->dlen, tc->flags)) == NULL)
			return (FALSE);
		if (STequal(tc->name, IIVQdir_col))
		{
			/* post validation */
			vq_AddVal(&fc->fltype, tabid, IIVQdir_col, 
					_dir_val, _dir_vmsg);
		}
	}
	/* now put in the column heading fields which must overlay the
	   box of the table field..
	   ---------------------------------------------------------
	   NOTE: these must be fields since if they are trim the form
	   system will paint the table field box on top.. in addition
	   they must be created after the table field is created so
	   that they will be on top.
	   --------------------------------------------------------- */
	switch (tab->usage)
	{
	case TAB_APPEND:
		/* add the assignment[/default] column heading */ 
		tabid[4] = 'e';
		if ((fl = IIFDacfAddCharFld(fr, tabid, 
			line+2, col + TCWID + 5, 
			tab->tabsect == TAB_MASTER && mode != MF_MASTAB
				? _aexp_text
				: _asgn_text,
			line+2, col + TCWID + 5, 1, 1, 1, SIMP_FLD_ATTR)) 
				== NULL)
			return (FALSE);
		fl->flhdr.fhd2flags = (i4)fdREADONLY;
		/* must reset the field color to 0, since the dynamic frame 
		   code has set it to 1 by default */
		fl->flhdr.fhdflags = SIMP_FLD_ATTR;
		break;
	case TAB_UPDATE:
	case TAB_DISPLAY:
		/* add the restriction column heading (see NOTE above) */
		tabid[4] = 'e';
		if ((fl = IIFDacfAddCharFld(fr, tabid, 
			line+2, col + TCWID + 5, _rest_text,
			line+2, col + TCWID + 5, 1, 1, 1, SIMP_FLD_ATTR))
				== NULL)
			return (FALSE);
		fl->flhdr.fhd2flags = (i4)fdREADONLY;
		/* must reset the field color to 0, since the dynamic frame 
		   code has set it to 1 by default */
		fl->flhdr.fhdflags = SIMP_FLD_ATTR;
		/* add the "sort" column heading (see NOTE above) */
		tabid[4] = 's';
		if ((fl = IIFDacfAddCharFld(fr, tabid, 
			line+2, col + TCWID + 34, _sort_text,
			line+2, col + TCWID + 34, 1, 1, 1, SIMP_FLD_ATTR)) 
				== NULL)
			return (FALSE);
		fl->flhdr.fhd2flags = (i4)fdREADONLY;
		/* must reset the field color to 0, since the dynamic frame 
		   code has set it to 1 by default */
		fl->flhdr.fhdflags = SIMP_FLD_ATTR;
		break;
	case TAB_LOOKUP:
		/* add the "Column title" column heading (see NOTE above) */
		tabid[4] = 'e';
		if ((fl = IIFDacfAddCharFld(fr, tabid, 
			line+2, col + TCWID + 11, _ltit_text,
			line+2, col + TCWID + 11, 1, 1, 1, SIMP_FLD_ATTR)) 
				== NULL) 
			return (FALSE);
		fl->flhdr.fhd2flags = (i4)fdREADONLY;
		/* must reset the field color to 0, since the dynamic frame 
		   code has set it to 1 by default */
		fl->flhdr.fhdflags = SIMP_FLD_ATTR;
		break;
	}
	/* bump the line number that the next table starts on*/
	*lin += tab->numcols + 5;
	return (TRUE);
}
/*{
** Name:	vq_AddVal	- add a validation to a field/col def 
**
** Description:
**	This function adds a validation to a dynamically created
**	field or column.
**	First we build up the validation string from the name
**	components.. then post the the structure.
**
** Inputs:
**	FLDTYPE	*tp;	- ptr to field/col type struct
**	char *fname;	- ptr to field name
**	char *cname;	- ptr to column name if non null
**	char *valstr;	- validation string (does not include
**				the field name)
**	char *valmsg;	- validation message
**
** Outputs:
**	Returns:
**		VOID	- should really return allocation status 
**	
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/24/89 (tom) - written
*/
LOC VOID 
vq_AddVal(tp, fname, cname, valstr, valmsg)
FLDTYPE	*tp;
char *fname;
char *cname;
char *valstr;
char *valmsg;
{
	char buf[100];
	/* put field name into validation */
	STcopy(fname, buf);
	/* if there is a column name then put it in to */
	if (cname != NULL)
	{
		STcat(buf, ".");
		STcat(buf, cname);
	}
	/* finally concatenate the validation string */
	STcat(buf, valstr);
	/* post to the structure */
	/* the frame structure which is being created is being 
	   allocated on the current frontend tag... */
	tp->ftvalstr = FEsalloc(buf);	
	tp->ftvalmsg = valmsg;	
}
/*{
** Name:	vq_PutJoins	- paint the vq joins on the display frame 
**
** Description:
**	This function paints box trim and join labels onto the
**	frame structure to represent the joins of the visual query.
**	Also if the global IIVQjtab is set to something other than
**	-1 then we paint an indicator of the first column the user
**	specified for the join.
**
** Inputs:
**	METAFRAME *mf;	- ptr to metaframe struct
**	FRAME *fr;	- ptr to frame to post into
**	bool compressed;	- compressed view flag
**
** Outputs:
**	Returns:
**		bool	TRUE=success, FALSE means allocation failed
**	
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/24/89 (tom) - written
*/
LOC bool
vq_PutJoins(mf, fr, compressed)
METAFRAME *mf;
FRAME *fr;	
bool compressed;	
{
	register i4	i;
	register i4	line_1;
	register i4	line_2;
	register i4	col_1;
	register i4	col_2;
	register MFJOIN	*jf;
	i4	runcol;		/* column that runs down the screen */
	i4	num;		/* number of joins */
	i4	numlabs;
	char	lab[3];
	JOIN_LAB labs[MF_MAXJOINS*2];
	num = mf->numjoins;
	for (numlabs = i = 0; i < num; i++)
	{
		jf = mf->joins[i];/* set pointer to join info */
		/* calculate the line numbers of the table.column */
		line_1 = tabpos[jf->tab_1].line + (compressed ? 0 : jf->col_1);	
		line_2 = tabpos[jf->tab_2].line + (compressed ? 0 : jf->col_2);	
		col_1 = tabpos[jf->tab_1].col;
		col_2 = tabpos[jf->tab_2].col;
		/* figure what column the verical column runs in */
		if (compressed || num >= 8) /* using branch/trunk */ 
			runcol = runcoltab[jf->type];
		else if (num >= 5)	/* 1 col per join */
			runcol = i; 
		else			/* 2 col per join, spread out */
			runcol = i * 2;
		/* post the 2 horizontal lines */
		if (IIFDabtAddBoxTrim(fr, line_1, runcol, line_1, col_1, (i4)0) 
				== FALSE)
			return (FALSE); 
		if (IIFDabtAddBoxTrim(fr, line_2, runcol, line_2, col_2, (i4)0)
				== FALSE)
			return (FALSE); 
		/* post the vertical connector, this is sometimes redundant
		   but not terribly wastefull */
		if (IIFDabtAddBoxTrim(fr, line_1, runcol, line_2, runcol,(i4)0)
				== FALSE)
			return (FALSE); 
		/* if we are painting the compressed view.. then we
		   don't need to bother with labels.. */
		if (compressed)
			continue;
		/* form the label for this join */
		vq_JoinLab(i, lab);
		/* post both ends of the join */
		numlabs += vq_PostLab(labs, numlabs, line_1, col_1, lab);
		numlabs += vq_PostLab(labs, numlabs, line_2, col_2, lab);
	}
	/* we have now painted all the lines.. 
	   and we will now paint the join labels we have accumulated. 
	   Note that for the compressed view numlabs is 0 so this
	   loop does nothing. */
	for (i = 0; i < numlabs; i++)
	{
		if (IIFDattAddTextTrim(fr, labs[i].line, 
				labs[i].col - STlength(labs[i].lab) - 1, 
				(i4)0, labs[i].lab) == FALSE)
			return (FALSE);
	}
	/* see if the user is in the middle of a join insertion sequence 
	   if so then paint the indicator showing where the first join
	   column is.. */
	if (IIVQjtab >= 0)
	{
		i4 len;
		i4 i;
		char str[4];
		line_1 = tabpos[IIVQjtab].line + IIVQjcol;	
		col_1 = tabpos[IIVQjtab].col;
		if (IIFDabtAddBoxTrim(fr, line_1, col_1-5, line_1, col_1-2, 
				(i4)0) == FALSE)
			return (FALSE); 
		if (IIFDattAddTextTrim(fr, line_1, 
				col_1 - 1,
				(i4)0, ERx(">")) == FALSE)
			return (FALSE);
		col_1 -= 6;
		line_1 = line_1 - (len = STlength(_join_text))/2 + 1;
		if (line_1 < 0)
			line_1 = 0;
		str[0] = ' ';
		str[2] = ' ';
		str[3] = EOS;
		/* paint the word vertically */
		/* !!! this assumes that the Japanese word for "Join" 
			is in kana not kanji */
		for (i = 0; i < len; i++)
		{	
			str[1] = _join_text[i];
			if (IIFDattAddTextTrim(fr, line_1++, col_1, 
					(i4)fdRVVID, str) == FALSE)
				return (FALSE);
		}
	}
	return (TRUE);
}
/*{
** Name:	vq_PostLab	- post a label to our list
**
** Description:
**	This function posts a label to the list of labels that
**	is built up as we put the joins on the frame.
**	This function is necessary so that we can check each
**	end of each join and know if it is the end point of 
**	another join.. using this method we avoid the n squared
**	number of comparisons necessary if we just checked 
**	each endpoint against every other endpoint.
**
** Inputs:
**	 JOIN_LABS *labs;	pointer to the label storage array
**	 i4  numlabs; 		number of labels posted so far
**	 i4  line;		line number that this label starts on
**	 i4  col;		column that this label starts on
**	 *lab;			ptr to the label text
**
** Outputs:
**	Returns:
**		bool	1 meaning that we inserted a new label
**			0 meaning that we didn't have to insert
**			  a new label because we found that another
**			  join has an endpoint simpilar to ours
**			  so we append the label text to the other one.
**	
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/24/89 (tom) - written
*/
LOC i4
vq_PostLab(labs, numlabs, line, col, lab)
register JOIN_LAB *labs;
register i4  numlabs;
register i4  line;
register i4  col;
register char *lab;
{
	register i4  i;
	/* go through all of the labels previously posted */
	for (i = 0; i < numlabs; i++, labs++)
	{
		/* if we have previously seen a label with the 
		   same line.. then we will append our
		   label string to it. Note that there can currently
		   be no more than 2 labels at the same line */
		if (line == labs->line) 
		{
			STcat(labs->lab, ERx("/"));
			STcat(labs->lab, lab);
			return 0;
		}
	}
	/* no label was found at the given line.. so we add one */
	labs->line = line;
	labs->col = col;
	STcopy(lab, labs->lab);
	return 1;
}
/*{
** Name:	vq_JoinLab	- form a join label
**
** Description:
**	This function forms a join label for display on the
**	Visual query display frame. Given a number this function
**	post a string into the callers buffer which is an alhpa
**	representaiton of the number.. (this is somewhat complicated
**	by the fact that we must be language, and alphabet independent.
**
** Inputs:
**	 i4  cur;		index of the current label to generate
**	 char *lab;		ptr to where to post the label text
**				Gaurenteed to point to a buffer of 
**				at least length = 3;
**
** Outputs:
**	Returns:
**		VOID
**	
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/24/89 (tom) - written
*/
LOC VOID 
vq_JoinLab(cur, lab)
i4  cur;
char *lab;
{
	if (cur > _alphasize)
	{
		*lab++ = _alphatab[cur / _alphasize]; 
		*lab++ = _alphatab[cur % _alphasize]; 
	}
	else
	{
		*lab++ = _alphatab[cur]; 
	}
	*lab = EOS;
}
/*{
** Name:	IIVQvlfVqLoadFrame	- load the table fields of a vq frame
**
** Description:
**	Load the table fields of a visual query frame, given the 
**	metaframe structure. This involves loading the column info 
**	into the tablefields which comprise the table definitions,
**	and setting the other fields to their current values.
**	This routine assumes that the frame has been add form'ed,
**	and that all of the elements of the metaframe definition are
**	in memory and valid.
**	
** Inputs:
**	METAFRAME *mf;	pointer to the metaframe strucuture
**
** Outputs:
**	Returns:
**		bool	- TRUE = success else FALSE
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	03/21/89 (tom) - written
*/
bool
IIVQvlfVqLoadFrame(mf)
register METAFRAME *mf;
{
	register i4  i;
	for (i = 0; i < mf->numtabs; i++)
	{
		if (IIVQlftLoadFrameTable(mf->tabs[i], i) == FALSE)
			return (FALSE);
	}
	return (TRUE);
}
/*{
** Name:	vq_LoadFrameTable	- load a table field of a vq frame
**
** Description:
**	Load a table field of a visual query frame.
**	This involves loading the column info into the tablefield
**	which comprise the table definitions, and setting the other 
**	fields to their current values.
**	
** Inputs:
**	MFTAB	*tab;	pointer to the table info
**	i4	num;	the table number
**
** Outputs:
**	Returns:
**		bool	- TRUE = success else FALSE
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	03/21/89 (tom) - written
*/
bool
IIVQlftLoadFrameTable(tab, num)
register MFTAB *tab;	
i4	num;
{
  char name[20];
	TABINFO	*ti;
	STprintf(name, IIVQtfname_fmt, num);
	ti = &tab_info[tab->usage];
	switch (tab->usage)
	{
	case TAB_APPEND:
		if (vq_LoadAppTable(tab, name, ti) == FALSE)
			return (FALSE);
		break;
	case TAB_UPDATE:
		if (vq_LoadUpdTable(tab, name, ti) == FALSE)
			return (FALSE);
		break;
	case TAB_DISPLAY:
		if (vq_LoadDispTable(tab, name, ti) == FALSE)
			return (FALSE);
		break;
	case TAB_LOOKUP:
		if (vq_LoadLupTable(tab, name, ti) == FALSE)
			return (FALSE);
		break;
	}
	return (TRUE);
}
/*{
** Name:	vq_LoadAppTable	- load an append table's values
**
** Description:
**	Load a table field of a visual query frame.
**	This involves loading the column info into the tablefield
**	which comprise the table definitions, and setting the other 
**	fields to their current values.
**	
** Inputs:
**	MFTAB	*tab;	pointer to the table info
**	char	*name;	name of the table (buffer is big enough for
**			 construction of the supporting field names.
**	TABINFO	*ti;	pointer to table formating info 	
**
** Outputs:
**	Returns:
**		bool	- TRUE = success else FALSE
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	03/21/89 (tom) - written
*/
LOC bool
  vq_LoadAppTable(tab, name, ti)
register MFTAB	*tab;
  char *name;
TABINFO *ti;
  {
    register i4 i;
    MFCOL *col;
    char *col1 = ti->cols[0].name;
    char *col2 = ti->cols[1].name;
    char *col3 = ti->cols[2].name;
    char cname[TCWID+1];
    char *inc;
	for (i = 0; i < tab->numcols; )
	{
		col = tab->cols[i];
		inc = IIVQgivGetIncVal(col);
		/* display the column name or the alias name */
		vq_ColDisp(col, cname, tab->usage == TAB_LOOKUP);
		i++; /* ones based frs row number */
/* # line 1175 "vqmakfrm.qsc" */	/* putrow */
    {
      if (IItbsetio(3,IIVQform,name,i) != 0) {
        IItcoputio(col1,(short *)0,1,32,0,cname);
        IItcoputio(col2,(short *)0,1,32,0,inc);
        IItcoputio(col3,(short *)0,1,32,0,col->info);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1182 "vqmakfrm.qsc" */	/* host code */
	}
	/* load the readonly trim field which overlaps
	   the border of the table field */	
	vq_LoadSimple(name, 'e', 
		tab->tabsect == TAB_MASTER
			? _aexp_text 
			: _asgn_text);
	return (TRUE);
  }
/*{
** Name:	vq_LoadUpdTable	- load an update table's values
**
** Description:
**	Load a table field of a visual query frame.
**	This involves loading the column info into the tablefield
**	which comprise the table definitions, and setting the other 
**	fields to their current values.
**	
** Inputs:
**	MFTAB	*tab;	pointer to the table info
**	char	*name;	name of the table (buffer is big enough for
**			 construction of the supporting field names.
**	TABINFO	*ti;	pointer to table formating info 	
**
** Outputs:
**	Returns:
**		bool	- TRUE = success else FALSE
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	03/21/89 (tom) - written
*/
LOC bool
vq_LoadUpdTable(tab, name, ti)
register MFTAB	*tab;
char *name;
TABINFO *ti;
{
	/* a display table is the basically the same as a 
	   update table in regard to the table field loading so we 
	   will call the display load routine */
	vq_LoadDispTable(tab, name, ti);
	/* now we must fill in the other supporting fields */
	vq_LoadSimple(name, 'i', 
		(tab->flags & TAB_INSFLG) ? IIVQ_y_text : IIVQ_n_text);
	vq_LoadSimple(name, 'd', 
		(tab->flags & TAB_DELFLG) ? IIVQ_y_text : IIVQ_n_text);
	return (TRUE);
}
/*{
** Name:	vq_LoadDispTable  	- load a display table's values 
**
** Description:
**	Load a table field of a visual query frame.
**	This involves loading the column info into the tablefield
**	which comprise the table definitions, and setting the other 
**	fields to their current values.
**	
** Inputs:
**	MFTAB	*tab;	pointer to the table info
**	char	*name;	name of the table (buffer is big enough for
**			 construction of the supporting field names.
**	TABINFO	*ti;	pointer to table formating info 	
**
** Outputs:
**	Returns:
**		bool	- TRUE = success else FALSE
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	03/21/89 (tom) - written
*/
LOC bool
  vq_LoadDispTable(tab, name, ti)
register MFTAB	*tab;
  char *name;
TABINFO *ti;
  {
    register i4 i;
    MFCOL *col;
    char *col1 = ti->cols[0].name;
    char *col2 = ti->cols[1].name;
    char *col3 = ti->cols[2].name;
    char *col4 = ti->cols[3].name;
    char *col5 = ti->cols[4].name;
    char *inc;
    char cname[TCWID+1];
    char srt[10];
    char *dir;
	for (i = 0; i < tab->numcols; )
	{
		col = tab->cols[i];
		inc = IIVQgivGetIncVal(col);
		/* if it's a sequenced field.. */
		if (col->corder)
		{
			/* post the sort order */
			STprintf(srt, ERx("%d"), col->corder);
			/* and the direction */
			dir = (col->flags & COL_DESCEND) 
				? IIVQ_d_text : IIVQ_a_text; 
		}
		else	/* otherwise use the null string */
		{
			srt[0] = EOS;
			dir = _nullstring;
		}
		/* display the column name or the alias name */
		vq_ColDisp(col, cname, tab->usage == TAB_LOOKUP);
		i++; /* ones based frs row number */
/* # line 1315 "vqmakfrm.qsc" */	/* putrow */
    {
      if (IItbsetio(3,IIVQform,name,i) != 0) {
        IItcoputio(col1,(short *)0,1,32,0,cname);
        IItcoputio(col2,(short *)0,1,32,0,inc);
        IItcoputio(col3,(short *)0,1,32,0,col->info);
        IItcoputio(col4,(short *)0,1,32,0,srt);
        IItcoputio(col5,(short *)0,1,32,0,dir);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1324 "vqmakfrm.qsc" */	/* host code */
	}
	/* load the readonly trim fields which overlap 
	   the border of the table field */	
	vq_LoadSimple(name, 'e', _rest_text);
	vq_LoadSimple(name, 's', _sort_text);
	return (TRUE);
  }
/*{
** Name:	vq_LoadLupTable	- load an display table's definitions
**
** Description:
**	Load a table field of a visual query frame.
**	This involves loading the column info into the tablefield
**	which comprise the table definitions, and setting the other 
**	fields to their current values.
**	
** Inputs:
**	MFTAB	*tab;	pointer to the table info
**	char	*name;	name of the table (buffer is big enough for
**			 construction of the supporting field names.
**	TABINFO	*ti;	pointer to table formating info 	
**
** Outputs:
**	Returns:
**		bool	- TRUE = success else FALSE
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	03/21/89 (tom) - written
*/
LOC bool
  vq_LoadLupTable(tab, name, ti)
register MFTAB	*tab;
  char *name;
TABINFO *ti;
  {
    register i4 i;
    MFCOL *col;
    char *col1 = ti->cols[0].name;
    char *col2 = ti->cols[1].name;
    char *col3 = ti->cols[2].name;
    char *col4 = ti->cols[3].name;
    char *inc;
    char cname[TCWID+1];
    char srt[10];
	for (i = 0; i < tab->numcols; )
	{
		col = tab->cols[i];
		inc = IIVQgivGetIncVal(col);
		/* if the column is to be included in the lookup display */
		if (col->corder)
		{
			/* if this column is ordered, but it doesn't have
			   any text then put in the default.. e.g. the
			   column name */
			if (col->info[0] == EOS)
			{
				col->info = (char *) IIVQ_VAlloc(0, col->name);
				IIFDftFormatTitle(col->info);
			}
			/* set the display order */
			STprintf(srt, ERx("%d"), col->corder);
		}
		else	/* otherwise just post null string */
		{
			col->info =  _nullstring;
			srt[0] = EOS;
		}
		/* display the column name or the alias name */
		vq_ColDisp(col, cname, tab->usage == TAB_LOOKUP);
		i++; /* ones based frs row number */
/* # line 1408 "vqmakfrm.qsc" */	/* putrow */
    {
      if (IItbsetio(3,IIVQform,name,i) != 0) {
        IItcoputio(col1,(short *)0,1,32,0,cname);
        IItcoputio(col2,(short *)0,1,32,0,inc);
        IItcoputio(col3,(short *)0,1,32,0,srt);
        IItcoputio(col4,(short *)0,1,32,0,col->info);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1416 "vqmakfrm.qsc" */	/* host code */
	}
	/* lookup tables have qualification fields */
	vq_LoadSimple(name, 'q', 
		(tab->flags & TAB_QUALLUP) ? IIVQ_y_text : IIVQ_n_text);
	/* load the readonly trim fields which overlap 
	   the border of the table field */	
	vq_LoadSimple(name, 'e', _ltit_text);
	return (TRUE);
  }
/*{
** Name:	vq_LoadSimple	- load an simple field
**
** Description:
**	Load a simple field of the visual query frame. This involves
**	constructing the name from the components given in the arguments
**	and then doing a putform.
**	
** Inputs:
**	char	*name;	name of the table (buffer is big enough for
**			 construction of the supporting field names.
**	char	type;	a character indicating the type of supporting
**			field to load..
**			the supporting fields are named by appending
**	   		'i', 'd', or 'q' etc.. to the name of the table,
**	   		the table names are always 4 long.. controlled by the
**	   		IIVQtfname_fmt string.. e.g. "_001" 
**	char	*val;	value to load the field with
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	03/21/89 (tom) - written
*/
LOC VOID 
vq_LoadSimple(name, type, val) 
char *name;
char type;
  char *val;
{
  char fname[10];
	STcopy(name, fname);
	fname[5] = EOS;
	fname[4] = type;
/* # line 1472 "vqmakfrm.qsc" */	/* putform */
  {
    if (IIfsetio(IIVQform) != 0) {
      IIputfldio(fname,(short *)0,1,32,0,val);
    } /* IIfsetio */
  }
/* # line 1473 "vqmakfrm.qsc" */	/* host code */
}
/*{
** Name:	vq_ColDisp	- set column name to be displayed
**
** Description:
**	Depending upon whether the column name is to be displayed or not
**	we return a buffer filled with an appropriate string to be
**	displayed in the table field of the visual query.
**
** Inputs:
**	MFCOL	*col;	column to get the name of 
**	char	*buf;	ptr to caller's buffer 
**	bool	luptab;	flag to say if it's a lookup table
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	09/28/89 (tom) - written
*/
LOC VOID 
vq_ColDisp(col, buf, luptab)
MFCOL *col;
char *buf;
bool luptab;
{
	/* if it's a lookup table and it's used or in a variable.. then we
	   want to use the alias string and maybe put in an indicator */
	if (luptab == TRUE && (col->flags & (COL_USED|COL_VARIABLE)))
	{
		STmove(col->alias, ERx(' '), TCWID, buf); 
		/* if the alias is different than the column name then
		   put in an indicator so the user sees that there is
		   something different about this column */
		if (STcompare(col->alias, col->name) != 0)
		{
			buf[TCWID - 1] = ERx('@');
		}
	}
	else
	{
		STmove(col->name, ERx(' '), TCWID, buf);
	}
	buf[TCWID] = EOS;
}
/*{
** Name:	IIVQivsInitVQStrings	- initialize visual query information
**
** Description:
**	If this initialization has not been called before during
**	this program invocation. then do the following:
**
**		Read Table Type text from the message file.
**
** Inputs:
**	none
**
** Outputs:
**
**	Returns:
**	bool		- TRUE if success, else FALSE
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	2/13/89	 (tom) - created
**	2/91 (Mike S) Made public
*/
bool
IIVQivsInitVQStrings()
{
	if (_ins_text == NULL)
	{
		_table_text = ERget(F_VQ001B_table_text);
		_ins_text = ERget(F_VQ00C1_ins_text);
		_ins_tit_col = 55 - STlength(_ins_text);
		_del_text = ERget(F_VQ00C2_del_text);
		_del_tit_col = 55 - STlength(_del_text);
		_qua_text = ERget(F_VQ00C3_qua_text);
		_qua_tit_col = 55 - STlength(_qua_text);
		_join_text = ERget(F_VQ00C4_join_text);
		_detail_tf = ERget(F_VQ00C5_detail_tf);
		_master_tf = ERget(F_VQ0139_master_tf);
		_aexp_text = ERget(F_VQ00C6_aexp_text);
		_asgn_text = ERget(F_VQ00C7_asgn_text);
		_sort_text = ERget(F_VQ00C8_sort_text);
		_rest_text = ERget(F_VQ00C9_rest_text);
		_luptit_text = ERget(F_VQ00CA_luptit_text);
		_lord_text = ERget(F_VQ00CB_lord_text);
		_ltit_text = ERget(F_VQ00CC_ltit_text);
		IIVQ_d_text = ERget(F_VQ00CD_d_text);
		IIVQ_a_text = ERget(F_VQ00CE_a_text);
		IIVQ_y_text = ERget(F_UG0001_Yes1); 
		IIVQ_n_text = ERget(F_UG0006_No1); 
		IIVQ_v_text = ERget(F_VQ013D_variable); 
		_incptr_text = ERget(F_VQ00D1_incptr_text);
		_inccol_text = ERget(F_VQ00D2_inccol_text);
		_alphatab = ERget(F_VQ00D3_alphatab);
		_alphasize = STlength(_alphatab);
		tab_info[TAB_APPEND].name = ERget(F_VQ0017_app_tab);
		tab_info[TAB_UPDATE].name = ERget(F_VQ0018_upd_tab);
		tab_info[TAB_DISPLAY].name = ERget(F_VQ0019_dsp_tab);
		tab_info[TAB_LOOKUP].name = ERget(F_VQ001A_lup_tab);
		_dir_val = ERget(F_VQ00D4_dir_val);
		_dir_vmsg = ERget(F_VQ00D5_dir_vmsg);
		_yn_val = ERget(F_VQ00D6_yn_val);
		_yn_vmsg = ERget(F_VQ00D7_yn_vmsg);
	}
	return (TRUE);
}
/*{
** Name:	IIVQgivGetIncVal - Get character representation for 
**				   Include column in VQ
**
** Description:
**	If the column appears on the form (COL_USED is set) "y" for 'yes'
**	Otherwise, if the column becomes a variable (COL_VARIABLE is set) "v" 
**	for 'variable'.
**	Otherwise, "n" for 'no'.
**
** Inputs:
**	col		MF_COL *	VQ column structure
**
** Outputs:
**
**	Returns:
**		char *		string shown above
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	1/91 (Mike S) Initial version
*/
char *
IIVQgivGetIncVal(col)
MFCOL *col;
{
	char *inc;
	_VOID_ IIVQivsInitVQStrings();
	if (col->flags & COL_USED)
		inc = IIVQ_y_text;
	else if (col->flags & COL_VARIABLE)
		inc = IIVQ_v_text;
	else
		inc = IIVQ_n_text; 
	return inc;
}
