# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include       <er.h>
# include       <st.h>
# include       <me.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include       <adf.h>
# include       <afe.h>
# include	<fglstr.h>
# include	<stdprmpt.h>
# include <ui.h>
/*
# include	<ui.h>
	...so ming knows about the dependency
*/
# include       <uigdata.h>
# include <abclass.h>
# include <metafrm.h>
# include      	"ervq.h"
/**
** Name:	vqrecfrm.qsc - Reconcile a Frame's Visual Query with database
**
** Description:
**	This allows the visual queries in a frame to be reconciled with the
**	database.
**
**	IIVQrfdReconcileFrameDiscreps - Reconcile visual queries with database
**	IIVQgnmGetNewMF 	      - Set up the new mf for viewing
**	IIVQgomGetOldMF 	      - Set up the old mf for viewing
**
** History:
**	1/7/90 (Mike S)	Initial Version
**	22-nov-1993 (daver)
**		Add COL_OPTLOCK to COL_COPY_FLAGS to fix bug 54258 (reconcile
**		does not copy over optim lock column info). This is not the end
**		of the optim lock problem, but it allows the other bugs to be 
**		flushed out (e.g, what to do when an optim lock column is 
**		dropped or datatype changed).
**	10-dec-93 (connie) Bug #43979
**		Before a frame is reconciled, check for "CUSTOM" and give
**		warning message just as we do for editing visual query of
**		customed frame
**	17-feb-94 (blaise)
**		Added code to handle optimistic locking: (i)if a column's
**		datatype changes, reset the flag so it's no longer used for
**		optimistic locking; (ii)if a reconciled frame no longer 
**		contains any optimistic locking columns, change the frame
**		behaviour back to 'none'.
**	17-feb-94 (blaise)
**		More code to handle optimistic locking; if an optimistic
**		locking column becomes a key column, reset the flag so it's no
**		longer used for optimistic locking.
**	24-jan-95 (forky01)
**		Added function "normalize_valid_name" to help fix bug#66529.
**		This also meant adding ui.qsh and uigdata.h to the include list.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	30-jul-2001 (toumi01)
**	    rename BUFSIZ to II_BUFSIZ to avoid conflict with system defines
**/
/* # define's */
# define II_BUFSIZ	256
# define NO_SUCH	-1	/* new index if table or 
				   column has been deleted */
/* Flags simply copied from old to new column */
# define COL_COPY_FLAGS	(COL_USED|COL_DETJOIN|COL_LUPJOIN|COL_SUBJOIN|\
			 COL_SEQUENCE|COL_DESCEND|COL_TYPECHANGE|COL_VARIABLE|\
			 COL_OPTLOCK)
/* Existing column hash table entry */
typedef struct
{
	MFCOL	*mfc;		/* Existing metaframe column structure */
	i4	newindex;	/* Index of column in new table */
} XMFCOL;
/* GLOBALDEF's */
/* extern's */
FUNC_EXTERN DB_STATUS 	afe_tyoutput();
FUNC_EXTERN ADF_CB 	*FEadfcb();
FUNC_EXTERN STATUS 	IIAMmpMetaPopulate();
FUNC_EXTERN STATUS 	IIAMmfMetaFree();
FUNC_EXTERN LSTR 	*IIFGals_allocLinkStr();
FUNC_EXTERN VOID 	IIFGglsGenLinkStr();
FUNC_EXTERN MFTAB	*IIFGpt_pritbl();
FUNC_EXTERN VOID 	IIUGbmaBadMemoryAllocation();
FUNC_EXTERN VOID 	IIUGerr();
FUNC_EXTERN char 	*IIUGfmt();
FUNC_EXTERN STATUS 	IIUGheHtabEnter();
FUNC_EXTERN STATUS 	IIUGhfHtabFind();
FUNC_EXTERN STATUS 	IIUGhiHtabInit();
FUNC_EXTERN VOID 	IIUGhrHtabRelease();
FUNC_EXTERN VOID 	IIVQcanColAliasNames();
FUNC_EXTERN VOID 	IIVQfvtFreeVqTag();
FUNC_EXTERN bool	IIVQjcJoinCheck();
FUNC_EXTERN bool	IIVQmmtMakeMfTable();
FUNC_EXTERN VOID 	IIVQveVqError();
FUNC_EXTERN STATUS	IIVQvvqValidateVQ();
FUNC_EXTERN PTR 	IIVQ_VAlloc();
/* static's */
static char	buffer[II_BUFSIZ];	/* General-purpose formatting buffer */
static READONLY char routine[] = ERx("IIVQrfdReconcileFrameDiscreps");
/* Forms system names */
static	READONLY
  char _form[]=       ERx("vqrecapp"), _tf[]=         ERx("report"), _tfcol[]=      ERx("report")
  ;
static READONLY	char	_nl[] 	 = ERx("\n");
static READONLY	char	_space[] = ERx(" ");
static READONLY	char	indent0[] = ERx("");
static READONLY	char	indent1[] = ERx("  ");
static READONLY	char	indent2[] = ERx("    ");
static READONLY	char	indent3[] = ERx("      ");
/* Commonly used short messages */
static	char	*quotes;
static	char	*quotes_tab_col;
static  bool	msg_init = FALSE;
static VOID	add_columns();
static LSTR 	*alloc_lstr();
static VOID	change_columns();
static VOID	change_keys();
static VOID	column_changed();
static bool 	changed_datatype();
static char 	*db_type_name();
static bool	delete_columns();
static VOID	delete_escape_code();
static VOID	delete_join();
static VOID	delete_one_escape();
static VOID	form_out_line();
static VOID     normalize_valid_name();
static VOID	output_line();
static VOID	realias_lookups();
static VOID	reconcile_joins();
static bool	reconcile_table();
static VOID 	resequence_columns();
static VOID	table_changed();
static i4 	true_length();
static bool	any_optlock_cols();
/* Structures for new and old metaframes */
static MFTAB	**nmftabs;	 	/* tables for new metaframe */
static MFJOIN	**nmfjoins;		/* join pointers for new metaframe */
static i4	nnumjoins;		/* Number of new joins */
static i4	nnumtabs;		/* Number of new tables */
static MFTAB	**omftabs;	 	/* tables for old metaframe */
static MFJOIN	**omfjoins;		/* join pointers for old metaframe */
static i4	onumjoins;		/* Number of old joins */
static i4	onumtabs;		/* Number of old tables */
/*{
** Name: IIVQrfdReconcileFrameDiscreps - Reconcile visual queries with database
**
** Description:
**	Create a new VQ by applying the table changes from the current contents
**	of the database, and allow the user to accept or reject the changes.
**
**	If there were changes, we will return with some of the metaframe
**	structures populated, and memory possibly allocated on the static 
**	VQ tag.  If there were no changes, or an error occured, we will
**	clean everything up.
**
**	If the frame was locked, we just put out a message saying so.
**	We don't try to reconcile it.
**
** Inputs:
**	frame	USER_FRAME *	Frame to reconcile
**	locked	bool		Was frame locked
**
** Outputs:
**	none
**
**	Returns:
**		bool		TRUE  	Changes were made, and the new VQ is 
**					in memory, but not saved to the 
**					database.
**				FALSE	No changes were made, or an error 
**					occured.  In either case, no new VQ
**					is available.
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	1/7/90 (Mike S)	Initial Version
*/
bool
IIVQrfdReconcileFrameDiscreps(frame, locked)
USER_FRAME		*frame;
bool			locked;
{
	i4	newindex[MF_MAXTABS]; 	/* New indices for tables in mf */
	METAFRAME *mf = frame->mf;	/* Existing metaframe */
	MFTAB 	  *omastab, *odettab;	/* Existing primary metaframe tables */
	MFTAB 	  *nmastab, *ndettab;	/* New primary metaframe tables */
	bool	  changes = FALSE;	/* Were any changes needed */
	bool	  wont_do = TRUE;	/* We won't reconcile this frame */
	i4	  i;
	ER_MSGID  msgid = FE_Update;
	char      tmpmaster[FE_UNRML_MAXNAME + 1];
	char      tmpdetail[FE_UNRML_MAXNAME + 1];
	char      tmptable[FE_UNRML_MAXNAME + 1];
	if (frame->flags & CUSTOM_EDIT)
	{
		if (IIUFccConfirmChoice(CONF_GENERIC,
			frame->name, ERget(msgid),
			ERget(F_VQ0121_CustFrEdHelp),
			ERx("vqcusted.hlp"),
			F_VQ0122_CustFrEdTitle,
			F_VQ0123_CustFrEdYes,
			F_VQ0124_CustFrEdNo,
			ERx(""), TRUE)     !=    CONFCH_YES) goto cleanup;
	}
	/* Cache messages */
	if (!msg_init)
	{
		quotes = STalloc(ERget(S_VQ00CA_Quote_Arg));
		quotes_tab_col = STalloc(ERget(S_VQ00CB_Quote_Table_Column));
		msg_init = TRUE;
	}
	/* Announce ourselves */
	form_out_line(ERget(S_VQ009B_Reconciling), indent0, 1, frame->name);
	if (locked)
	{
		/* Announce that we won't do it */
		form_out_line(ERget(S_VQ00C4_Locked), indent1, 1, frame->name);
		goto cleanup;
	}
	/* Retrieve the VQ and Escape code for the current frame */
	if (IIAMmpMetaPopulate(mf, MF_P_VQ | MF_P_ESC) != OK)
	{
		form_out_line(ERget(S_VQ00D4_CannotReconcile), indent1, 1, 
			      frame->name);
		goto cleanup;
	}
	/* Save definitions for old metaframe */
	omftabs = mf->tabs;
	omfjoins = mf->joins;
	onumtabs = mf->numtabs;
	onumjoins = mf->numjoins;
	/* Check that the primary tables still exist */
	omastab = IIFGpt_pritbl(mf, TAB_MASTER);
	if ((MFTAB *)NULL == omastab)
	{
		IIUGerr(E_VQ00A4_No_Primary_Master, 0, 1, frame->name);
		form_out_line(ERget(S_VQ00B4_Severe_Errors), indent1, 1, 
				frame->name);
		goto cleanup;
	}
	normalize_valid_name(omastab->name, tmpmaster);
	if (!IIVQmmtMakeMfTable(tmpmaster, omastab->usage, 
				omastab->tabsect, &nmastab))
	{
		IIUGerr(E_VQ00A2_Primary_Master_Gone, 0, 2, frame->name,
			omastab->name);
		form_out_line(ERget(S_VQ00A9_No_Master), indent1, 2,
			      omastab->name, frame->name);
		goto cleanup;
	}
	if ((mf->mode & MFMODE) == MF_MASDET)
	{
		odettab = IIFGpt_pritbl(mf, TAB_DETAIL);
		if ((MFTAB *)NULL == omastab)
		{
			IIUGerr(E_VQ00A5_No_Primary_Detail, 0, 1, frame->name);
			form_out_line(ERget(S_VQ00B4_Severe_Errors), indent1, 
				1, frame->name);
			goto cleanup;
		}
		normalize_valid_name(odettab->name, tmpdetail);
		if (!IIVQmmtMakeMfTable(tmpdetail, odettab->usage, 
					odettab->tabsect, &ndettab))
		{
			IIUGerr(E_VQ00A3_Primary_Detail_Gone, 0, 2, frame->name,
				odettab->name);
			form_out_line(ERget(S_VQ00AA_No_Detail), indent1, 2,
				      odettab->name, frame->name);
			goto cleanup;
		}
	}
	else
	{
		odettab = ndettab = NULL;
	}
	wont_do = FALSE;	/* Now we know that we can do this frame */
	/* Allocate space for the new table pointers */
	nmftabs = (MFTAB **)IIVQ_VAlloc(mf->numtabs * sizeof(MFTAB *), 
					(char *)NULL);
	/* Copy joins to new structures */
	if (mf->numjoins > 0)
	{
		MFJOIN	*mfjstructs;		/* joins for new metaframe */
		nmfjoins = (MFJOIN **)
			    IIVQ_VAlloc(mf->numjoins * sizeof(MFJOIN *), 
					(char *)NULL);
		mfjstructs = (MFJOIN *)
			       IIVQ_VAlloc(mf->numjoins * sizeof(MFJOIN), 
					   (char *)NULL);
		for (i = 0; i < mf->numjoins; i++)
		{
			nmfjoins[i] = mfjstructs + i;
			MEcopy((PTR)(mf->joins[i]), (u_i2)(sizeof (MFJOIN)), 
				(PTR)(nmfjoins[i]));
		}
	}
	else
	{
		nmfjoins = NULL;
	}
	/* Loop through tables in metaframe */
	for (i = 0, nnumtabs = 0; i < mf->numtabs; i++)
	{
		MFTAB	*tptr = mf->tabs[i];
		bool	tblchanges;
		if (tptr == omastab)
		{
			nmftabs[nnumtabs] = nmastab;
		}
		else if (tptr == odettab)
		{
			nmftabs[nnumtabs] = ndettab;
		}
		else
		{
		    normalize_valid_name(tptr->name, tmptable);
		    if (!IIVQmmtMakeMfTable(tmptable, tptr->usage,
					     tptr->tabsect, 
					     &(nmftabs[nnumtabs])))
		    {
			/* Output "table deleted" message */
			form_out_line(ERget(S_VQ00AB_Table_Deleted), indent1, 
					1, tptr->name);
			changes = TRUE;
			newindex[i] = NO_SUCH;
			/* Delete escape code for this table */
			delete_escape_code(mf, tptr, (XMFCOL *)NULL);
			continue;
		    }
		}
		/* At this point, we've got a table to reconcile */
		newindex[i] = nnumtabs;
		tblchanges = reconcile_table(mf, i, nmftabs[nnumtabs]);
		changes = changes || tblchanges;
		nnumtabs++;
	}
	if (changes)
	{
		/* Set up the new metaframe */
		mf->tabs = nmftabs;
		mf->numtabs = nnumtabs;
		mf->joins = nmfjoins;
		/* Now fixup all the joins which need it */
		reconcile_joins(mf, newindex);
		nnumjoins = mf->numjoins;
		/* Re-alias all the lookup tables */
		realias_lookups(mf);
		/*
		** If the frame uses optimistic locking, then check whether
		** there are any optimistic locking columns left, otherwise
		** we'll have to change the frame behaviour back to no locks
		** held on displayed data.
		*/
		if ((mf->mode & MF_OPTIM) && (any_optlock_cols(mf) == FALSE))
		{
			mf->mode &= MF_UNSET_LOCKS;
			form_out_line(ERget(S_VQ00F9_Changed_Locktype),
				indent2, 0);
		}
		/* And check the finished VQ for validity */
		if (IIVQvvqValidateVQ(mf, buffer, II_BUFSIZ) != OK)
		{
			/* It's bad -- put out a message */
			form_out_line(ERget(S_VQ00AC_VQ_Invalid), indent1, 1, 
					frame->name);
			IIVQveVqError(mf, buffer, II_BUFSIZ);
			output_line(buffer, (PTR)indent2);
		}
		mf->updmask |= MF_P_VQ;
	}
cleanup:	
	/* If we had no changes, clean up memory before returning */
	if (!changes)
	{
		IIVQfvtFreeVqTag();	/* Clean up dynamic memory */
		IIAMmfMetaFree(mf, mf->popmask);
		/* If we did this one, announce that no changes were needed */
		if (!wont_do)
			form_out_line(ERget(S_VQ00AD_No_Changes), indent1, 1, 
				      frame->name);
	}
	/* Skip a line */
	output_line(_space, (PTR)indent0);
	return(changes);
}
/*
**	Reconcile table
**
**	Reconcile a table with the current database definitions.
**
**	We use the column utility pointer to match up old and new versions
**	of columns:
**
**		old_column->utility -----> new_column
**		new_column->utility -----> xfmcols[i]
**					      .mfc  ---->   old_column
**					      .newindex ==  new_column's index
**
**	Returns:		TRUE	if changes were required
**				FALSE 	otherwise
*/
static bool
reconcile_table(mf, tblindx, newtbl)
METAFRAME	*mf;		/* Metaframe */
i4		tblindx;	/* Index of existing MFTAB structure */
MFTAB		*newtbl;	/* New table structure */
{
	XMFCOL	*xmfcols;	/* Column hash table entries */
	MFTAB	*oldtbl = mf->tabs[tblindx];
	PTR	hashtab;	/* Column hash table ID */
	bool	changed = FALSE; /* Has a change been made */
	bool	keychanged = FALSE;	/* Have the unique keys changed */
	bool	numdel = FALSE;	/* Has a numbered column be deleted */
	bool	added = FALSE;	/* Were columns added */
	i4	i;
	/* Initialize hash table */
	_VOID_ IIUGhiHtabInit(oldtbl->numcols, NULL, NULL, NULL, &hashtab);
	/* Create hash table entries for columns, and hash them */
	xmfcols = (XMFCOL *)IIVQ_VAlloc(oldtbl->numcols*sizeof(XMFCOL), 
					(char *)NULL);
	for (i = 0; i < oldtbl->numcols; i++)
	{
		xmfcols[i].mfc = oldtbl->cols[i];
		xmfcols[i].newindex = NO_SUCH;
		_VOID_ IIUGheHtabEnter(hashtab, oldtbl->cols[i]->name, 
					(PTR)(xmfcols + i));
		oldtbl->cols[i]->utility = (PTR)NULL;
	}
	/* Loop through new columns, looking for matches */
	for (i = 0; i < newtbl->numcols; i++)
	{
		MFCOL	*newcol;	/* New version of column */
		XMFCOL	*oldcol;	/* Old version of this column */
		PTR	ptr;
		newcol = newtbl->cols[i];
		if (IIUGhfHtabFind(hashtab, newcol->name, &ptr) == OK)
		{
			/* Found a match -- record it */
			oldcol = (XMFCOL *)ptr;
			oldcol->newindex = i;
			oldcol->mfc->utility = (PTR)newcol;
			newcol->utility = (PTR)oldcol;
		}
		else
		{
			/* No match */
			added = TRUE;
			newcol->utility = (PTR)NULL;
		}
	}
	/* Output deleted columns */
	if (delete_columns(oldtbl, xmfcols, 
			   &changed, &keychanged, &numdel))
		delete_escape_code(mf, oldtbl, xmfcols);
	/* Add new columns */
	if (added)
		add_columns(newtbl, &changed, &keychanged);
	/* Record columns which have changed */
	change_columns(newtbl, &changed, &keychanged);
	/* Output changes to unique keys, if any, if it's not a lookup */
	if (keychanged && (newtbl->usage != TAB_LOOKUP))
		change_keys(oldtbl, newtbl, &changed);
	/* Fix up join column numbers */
	for (i = 0; i < mf->numjoins; i++)
	{
		MFJOIN	*join = nmfjoins[i];
		if (join->tab_1 == tblindx)
			join->col_1 = xmfcols[join->col_1].newindex;
		if (join->tab_2 == tblindx)
			join->col_2 = xmfcols[join->col_2].newindex;
	}
	/* Copy table flags */
	newtbl->flags = oldtbl->flags;
	/* 
	** Resequence numbered columns, if any were deleted. 
	*/
	if (numdel)
		resequence_columns(newtbl);
	/* Clean up */
	IIUGhrHtabRelease(hashtab);
	return (changed);
}
/* 
**	Delete columns from a table 
*/
static bool
delete_columns(oldtbl, xmfcols, changed, key_changed, numdel)
MFTAB	*oldtbl;	/* Existing table */
XMFCOL	*xmfcols;	/* Existing columns */
bool    *changed;       /* (output) set to TRUE if we delete columns */
bool    *key_changed;   /* (output) set to TRUE if we delete unique key cols */
bool    *numdel;   	/* (output) set to TRUE if we delete a numbered column*/
{
        i4      i;
	bool    deleted = FALSE;  /* Have we deleted any yet */
	LSTR    *lstr0 = (LSTR *)NULL, *lstr = (LSTR *)NULL;
	for (i = 0; i < oldtbl->numcols; i++)
	{
		if (xmfcols[i].newindex < 0)
		{
			MFCOL	*col = xmfcols[i].mfc;
			/* One to delete */
			table_changed(oldtbl->name, changed);
			if (!deleted)
			{
				/* Output the "Columns deleted" line */
				form_out_line(ERget(S_VQ00AE_Columns_Deleted), 
					      indent2, 0);
				deleted = TRUE;
			}
			/* See if it's a unique key column */
			if ((col->flags & COL_UNIQKEY) != 0)
				*key_changed = TRUE;
			/* 
			** If it's a sort or lookup-order column, 
			** note the deletion
			*/
			if (col->corder > 0)
				*numdel = TRUE;
			/* Link in the new column's name */
			lstr = alloc_lstr(lstr, LSTR_COMMA);
			if (lstr0 == (LSTR *)NULL) lstr0 = lstr;
			lstr->string = col->name;
		}
	}
	/* Output the column names */
	if (lstr != NULL)
	{
		lstr->follow = LSTR_NONE;
		IIFGglsGenLinkStr(lstr0, indent3, output_line, (PTR)NULL);
	}
	return (deleted);
}
/*
** Delete escape code for deleted columns
**
** If xmfcols is non-NULL, it points to a table which cross-references old 
** to new column numbers.  If NULL, the table has been deleted and we must
** delete all escape code for the table.
*/
static VOID
delete_escape_code(mf, table, xmfcols)
METAFRAME *mf;		/* Metaframe */
MFTAB   *table;         /* Table with deleted columns */
XMFCOL	*xmfcols;	/* Existing columns */
{
	i4 i, j;
	char *colalias;
	for (i = 0; i < table->numcols; i++)
	{
		if ((xmfcols != (XMFCOL *)NULL) && (xmfcols[i].newindex >= 0))
			continue;
		/* This one has been deleted -- look for escape code */
		colalias = table->cols[i]->alias;
		for (j = 0; j < mf->numescs; j++)
		{
			MFESC *esc = mf->escs[j];
			/* Only pay attention to field escapes */
			switch (esc->type)
			{
			    case ESC_FLD_ENTRY:
			    case ESC_FLD_CHANGE:
			    case ESC_FLD_EXIT:
				break;
			    default:
				continue;
			}
			if (STcompare(esc->item, colalias) == 0)
			{
				/* 
				** We have escape code which will go away.
				** Delete it.
				*/
				delete_one_escape(
					mf, esc, 
					(ER_MSGID)S_VQ00BF_Escape_Code_Deleted,
					indent3);
			}
		}
	}
}
/*
** 	Delete one piece of escape code
*/
static VOID
delete_one_escape(mf, esc, msgid, ind)
METAFRAME	*mf;	/* Metaframe */
MFESC		*esc;	/* Escape code to delete */
ER_MSGID	msgid;	/* Header message for deletion */
char		*ind;	/* Indentation for header message */
{
	char	*esctype;	/* Escape type name */
	/* Get escape type name */
        switch (esc->type)
        {
            case ESC_FLD_ENTRY:
	        esctype = ERget(F_VQ00E6_Field_Enter);
       	 	break;
            case ESC_FLD_CHANGE:
	        esctype = ERget(F_VQ00E7_Field_Change);
       	 	break;
            case ESC_FLD_EXIT:
        	esctype = ERget(F_VQ00E8_Field_Exit);
        	break;
            default:
		/* This shouldn't happen */
        	esctype = _space;
        	break;
	} 
	/*
        ** We have escape code which will go away.
        ** Dump it out, and set the type to an illegal
        ** value, which will cause it to be deleted.
        */
        mf->updmask |= MF_P_ESC;
        esc->type += NUMESCTYPES;
        form_out_line(ERget(msgid), ind, 2, esc->item, esctype);
        output_line(ERget(S_VQ00C2_Begin_Escape_Code), (PTR)indent0);
        output_line(ERget(S_VQ00C3_End_Escape_Code), (PTR)indent0);
        /*
        ** Scroll to the last line, so we can insert  the
        ** escape code above it.
        */
/* # line 721 "vqrecfrm.qsc" */	/* scroll */
  {
    if (IItbsetio(1,_form,_tf,-3) != 0) {
      IItbsmode((char *)"to");
      if (IItscroll(0,-1) != 0) {
      } /* IItscroll */
    } /* IItbsetio */
  }
/* # line 722 "vqrecfrm.qsc" */	/* host code */
        _VOID_ IIUFtfsTblFldString(esc->text, _form, _tf, _tfcol);
}
/*
**	Add new columns to a table
*/
static VOID
add_columns(table, changed, key_changed)
MFTAB	*table;		/* Table with new columns */
bool	*changed;	/* (output) set to TRUE if we add new columns */
bool	*key_changed;	/* (output) set to TRUE if we add unique key columns */
{
	i4	i;
	bool	added = FALSE;	/* Have we added any yet */
	LSTR	*lstr0 = (LSTR *)NULL, *lstr = (LSTR *)NULL;
	for (i = 0; i < table->numcols; i++)
	{
		MFCOL	*col = table->cols[i];
		if (col->utility == NULL)
		{
			/* A new one */
			table_changed(table->name, changed);
			if (!added)
			{
				/* Output the "Columns added" line */
				form_out_line(ERget(S_VQ00AF_Columns_Added), 
						indent2, 0);
				added = TRUE;
			}
			/* See if it's a unique key column */
			if ((col->flags & COL_UNIQKEY) != 0)
				*key_changed = TRUE;
			/* Don't put it in the form or in a variable */
			col->flags &= ~(COL_USED|COL_VARIABLE);
			/* Link in the new column's name */
			lstr = alloc_lstr(lstr, LSTR_COMMA);
			if (lstr0 == (LSTR *)NULL) lstr0 = lstr;
			lstr->string = col->name;
		}
	}
	/* Output the column names */
	if (lstr != NULL)
	{
		lstr->follow = LSTR_NONE;
		IIFGglsGenLinkStr(lstr0, indent3, output_line, (PTR)NULL);
	}
}
/* 
**	Record columns which have changed 
*/
static VOID
change_columns(newtbl, changed, key_changed)
MFTAB *newtbl;		/* New version of table */
bool	*changed;	/* (output) set to TRUE if we change columns */
bool	*key_changed;	/* (output) set to TRUE if we change unique key cols */
{
	i4	i;
	bool	col_changed;
	/* Loop through columns which remain in table */
	for ( i = 0; i < newtbl->numcols; i++)
	{
		MFCOL *newcol = newtbl->cols[i];
		MFCOL *oldcol;
		/* Ensure that this column still exists */
		if (newcol->utility == (PTR)NULL)
			continue;
		oldcol = ((XMFCOL *)newcol->utility)->mfc;
		col_changed = FALSE;
		/* 
		** Check for a datatype change, including nullability 
		** and defaultability 
		*/
		if ((newcol->type.db_datatype != oldcol->type.db_datatype) ||
		    (newcol->type.db_length != oldcol->type.db_length) ||
		    (newcol->type.db_prec != oldcol->type.db_prec)  ||
		    ((newcol->flags & COL_DEFAULT) != 
						(oldcol->flags & COL_DEFAULT)))
		{
			LSTR	*lstr0, *lstr;
			bool def;
			column_changed(newtbl->name, newcol->name, changed, 
				       &col_changed);
			/* Format 'Datatype has changed' message */
			lstr0 = lstr = alloc_lstr((LSTR *)NULL, LSTR_NONE);
			STcopy(ERget(S_VQ00B5_Datatype_Changed1), 
			       lstr->buffer);
			lstr = alloc_lstr(lstr, LSTR_NONE);
			def = (oldcol->flags & COL_DEFAULT) != 0;
			_VOID_ IIUGfmt(lstr->buffer, sizeof(lstr->buffer), 
				quotes, 1, 
				db_type_name(&oldcol->type, def));
			lstr = alloc_lstr(lstr, LSTR_NONE);
			STcopy(ERget(S_VQ00B6_Datatype_Changed2), 
			       lstr->buffer);
			lstr = alloc_lstr(lstr, LSTR_NONE);
			def = (newcol->flags & COL_DEFAULT) != 0;
			_VOID_ IIUGfmt(lstr->buffer, sizeof(lstr->buffer), 
				quotes, 1, 
				db_type_name(&newcol->type, def));
			IIFGglsGenLinkStr(lstr0, indent3, output_line, 
					  (PTR)NULL);
		}
		/* Copy column flags */
		newcol->flags &= ~COL_COPY_FLAGS;
		newcol->flags |= (oldcol->flags & COL_COPY_FLAGS);
		/* 
		** If the datatype has really changed (not just nullability):
		** (i)if the column is USED, mark it TYPECHANGE to force a new
		** copy of it on the form;
		** (ii)if the column was used for optimistic locking, unset
		** its COL_OPTLOCK bit.
		*/
		if (changed_datatype(&newcol->type, &oldcol->type))
		{
			if ((newcol->flags & COL_USED) != 0)
				newcol->flags |= COL_TYPECHANGE;
			if ((newcol->flags & COL_OPTLOCK) != 0)
			{
				newcol->flags ^= COL_OPTLOCK;
				form_out_line(ERget(S_VQ00FA_Unset_Optimistic),
					indent3, 0);
			}
		}
		/* Copy info */
		newcol->info = oldcol->info;
		/* Copy order */
		newcol->corder = oldcol->corder;
		/* Check if a sequenced column is an i4 */
		if ((newcol->flags & COL_SEQUENCE) != 0)
		{
			if ((abs(newcol->type.db_datatype) != DB_INT_TYPE) ||
			    (true_length(&newcol->type) != 4))
			{
				column_changed(newtbl->name, newcol->name, 
						changed, &col_changed);
				form_out_line(ERget(S_VQ00B7_Bad_Seq_Datatype),
						indent3, 0);
				newcol->flags &= ~COL_SEQUENCE;
				newcol->info = ERx("");
			}
		}
		/* Check for a unique key change */
		if ((oldcol->flags & COL_UNIQKEY) != 
		    (newcol->flags & COL_UNIQKEY))
			*key_changed = TRUE;
	}
}
/* 
**	Output a change in the key columns
*/
static VOID
change_keys(oldtbl, newtbl, changed)
MFTAB	*oldtbl;	/* The existing table */
MFTAB	*newtbl;	/* The new table */
bool	*changed;	/* Whether the table had changed */
{
	LSTR *lstr0, *lstr;
	i4  i;
	bool optlock_cols = FALSE;
	/* Output the "table changed" message, if need be */
	table_changed(newtbl->name, changed);
	/* Output the old keys */
	lstr = lstr0 = (LSTR *)NULL;
	form_out_line(ERget(S_VQ00B8_Previous_Keys), indent2, 0);
	for ( i = 0; i < oldtbl->numcols; i++)
	{
		if ((oldtbl->cols[i]->flags & COL_UNIQKEY) != 0)	
		{
			lstr = alloc_lstr(lstr, LSTR_COMMA);
			if (lstr0 == NULL)
				lstr0 = lstr;
			lstr->string = oldtbl->cols[i]->name;
		}
	}
	lstr->follow = LSTR_NONE;
        IIFGglsGenLinkStr(lstr0, indent3, output_line, (PTR)NULL);
	/* Output the new keys */
	lstr = lstr0 = NULL;
	form_out_line(ERget(S_VQ00B9_Current_Keys), indent2, 0);
	for ( i = 0; i < newtbl->numcols; i++)
	{
		if ((newtbl->cols[i]->flags & COL_UNIQKEY) != 0)	
		{
			lstr = alloc_lstr(lstr, LSTR_COMMA);
			if (lstr0 == NULL)
				lstr0 = lstr;
			lstr->string = newtbl->cols[i]->name;
		}
		if ((newtbl->cols[i]->flags & COL_OPTLOCK) != 0)	
		{
			newtbl->cols[i]->flags ^= COL_OPTLOCK;
			optlock_cols = TRUE;
		}
	}
	lstr->follow = LSTR_NONE;
        IIFGglsGenLinkStr(lstr0, indent3, output_line, (PTR)NULL);
	if (optlock_cols == TRUE)
	{
		form_out_line(ERget(E_VQ00FC_Key_Column), indent2, 0);
	}
}
/*
**	Reconcile joins.  Joins will be
**	
**	1. Deleted silently if either table or column was deleted
**	2. Deleted with a message if the datatypes now conflict.
**	3. Kept otherwise.
**
**	Remember, at this point the join structures have their old
**	table indices, but their new column indices.
*/
static VOID
reconcile_joins(mf, newindex)
METAFRAME	*mf;		/* Metaframe */
i4		newindex[]; 	/* New indices for tables in mf */
{
	i4	i, j;			/* Loop counters */
	bool	deleted = FALSE;	/* Did we delete any joins */
	i4	newtab_1, newtab_2;	/* New table indices */
	register  MFJOIN	*joinptr;
	/* Loop through all the joins */
	for (i = 0; i < mf->numjoins; i++)
	{
		joinptr = mf->joins[i];
		/* See if either table or either column was deleted */
		newtab_1 = newindex[joinptr->tab_1];
		newtab_2 = newindex[joinptr->tab_2];
		if ((newtab_1 < 0) || (newtab_2 < 0) || 
		    (joinptr->col_1 < 0) || (joinptr->col_2 < 0))
		{
			/* delete it */
			delete_join(mf, joinptr, newtab_1, newtab_2);
			deleted = TRUE;
		}
		else
		{
			MFCOL	*col1, *col2;
			col1 = mf->tabs[newtab_1]->cols[joinptr->col_1];
			col2 = mf->tabs[newtab_2]->cols[joinptr->col_2];
			/* Check validity */
			if (!IIVQjcJoinCheck(col1, col2))
			{
				/* Annonunce our deletion */
				LSTR *lstr0, *lstr;
				lstr0 = lstr = 
					alloc_lstr((LSTR *)NULL, LSTR_NONE);
				STcopy(ERget(S_VQ00BA_Bad_Join1), lstr->buffer);
				lstr = alloc_lstr(lstr, LSTR_NONE);
				_VOID_ IIUGfmt(
					  lstr->buffer, 
					  sizeof(lstr->buffer), 
					  quotes_tab_col, 
					  2, 
					  mf->tabs[newtab_1]->name, 
					  col1->name);
				lstr = alloc_lstr(lstr, LSTR_NONE);
				STcopy(ERget(S_VQ00BB_Bad_Join2), lstr->buffer);
				lstr = alloc_lstr(lstr, LSTR_NONE);
				_VOID_ IIUGfmt(
					  lstr->buffer, 
					  sizeof(lstr->buffer), 
					  quotes_tab_col, 
					  2, 
					  mf->tabs[newtab_2]->name, 
					  col2->name);
				lstr = alloc_lstr(lstr, LSTR_NONE);
				STcopy(ERget(S_VQ00BC_Bad_Join3), lstr->buffer);
				IIFGglsGenLinkStr(lstr0, indent1, output_line, 
					  	  (PTR)NULL);
				delete_join(mf, joinptr, newtab_1, newtab_2);
				deleted = TRUE;
			}
			else
			{
				/* Fix up table indices */
				joinptr->tab_1 = newtab_1;
				joinptr->tab_2 = newtab_2;
			}
		}
	}
	/* Close up the gaps */
	if (deleted)
	{
		for (i = 0, j = 0; i < mf->numjoins; i++)
		{
			if (mf->joins[i]->tab_1 >= 0)
			{
				/* This one's OK; copy it, if need be */
				if (i != j)
				{
					mf->joins[j] = mf->joins[i];
				}
				j++;
			}
		}
		mf->numjoins = j;
	}
}
/*
**	Delete a join.  At this point, the join has the old table indices
**	and the new column indices.
*/
static	VOID
delete_join(mf, joinptr, newtab_1, newtab_2)
METAFRAME *mf;			/* Metaframe */
MFJOIN	*joinptr;		/* Join to delete */
i4	newtab_1, newtab_2;	/* New table indices */
{
	/* Clear column flag for any column which still exists */
	if ((newtab_1 >= 0) && (joinptr->col_1 >= 0))
	{
		u_i4	colflag;	/* Column flag to clear */
		colflag = (JOIN_MASDET == joinptr->type) ?
				COL_DETJOIN : COL_LUPJOIN;
		mf->tabs[newtab_1]->cols[joinptr->col_1]->flags &= ~colflag;
	}
	if ((newtab_2 >= 0) && (joinptr->col_2 >= 0))
	{
		mf->tabs[newtab_2]->cols[joinptr->col_2]->flags &= ~COL_SUBJOIN;
	}
	/* Mark join invalid */
	joinptr->tab_1 = NO_SUCH;
}
/*
**	Re-alias lookup tables
**
**	To guarantee unique alias names for lookup tables, we have to
**	do the following:
**
**	1.  Point the alias pointers for all lookup tables to the column
**	    name.
**	2.  Re-alias all the lookup tables.
**
**	This will possibly change alias names used in escape code, but
**	c'est la vie.  We will change the item name for all escape
**	code whose fieldname changes, and list the changes.
*/
static VOID
realias_lookups(mf)
METAFRAME *mf;
{
	i4 i,j;		/* Loop counters */
	MFTAB	*tptr;		/* Table pointer */
	bool	al_change;	/* Has an alias name changed */
	bool	tbl_al_change;	/* Has an alias name changed in this table */
	PTR	nhashtab;	/* Hash table ID for new aliases */
	PTR	ohashtab;	/* Hash table ID for old aliases */
	i4	totcols;	/* Total of columns in all tables */
	/* 
	** Initialize all lookup alias pointers.  Count columns while
	** we're at it.
	*/
	totcols = 0;
	for (i = 0; i < mf->numtabs; i++)
	{
		tptr = mf->tabs[i];
		totcols += tptr->numcols;
		if (TAB_LOOKUP == tptr->usage)
		{
			for (j = 0; j < tptr->numcols; j++)
			{
				tptr->cols[j]->alias = tptr->cols[j]->name;
			}
		}
	}
	/* Re-alias all lookup tables. */
	for (i = 0; i < mf->numtabs; i++) 
	{
		tptr = mf->tabs[i]; 
		if (TAB_LOOKUP == tptr->usage)
		{ 
			IIVQcanColAliasNames(mf, i);
		}
	}
	/* 
	** Dump the list of alias changes.  They can only occur in
	** lookup tables.  Hash all the old and new aliases while we're at it.
	*/
	al_change = FALSE;
	_VOID_ IIUGhiHtabInit(totcols, NULL, NULL, NULL, &nhashtab);
	_VOID_ IIUGhiHtabInit(totcols, NULL, NULL, NULL, &ohashtab);
	for (i = 0; i < mf->numtabs; i++)
	{
		MFCOL	*colptr;	/* Current column */
		MFCOL	*oldcol;	/* Older version of column */
		register char *oldalias;
		register char *newalias;
		tbl_al_change = FALSE;
		tptr = mf->tabs[i];
		for (j = 0; j < tptr->numcols; j++)
		{
			colptr = tptr->cols[j];
			newalias = colptr->alias;
			 _VOID_ IIUGheHtabEnter(
				nhashtab, newalias, (PTR)NULL);
			if ((PTR)NULL == colptr->utility)
				continue;	/* Brand new column */
			oldcol = ((XMFCOL *)colptr->utility)->mfc;
			oldalias = oldcol->alias;
			_VOID_ IIUGheHtabEnter(ohashtab, oldalias, newalias);
			if (TAB_LOOKUP != tptr->usage)
				continue;	/* Not a lookup table */
			if (STcompare(newalias, oldalias) == 0)
				continue;	/* No alias changes */
			/* We've found a change */
			if (!al_change)
			{
				form_out_line(ERget(S_VQ00B0_Alias_Changes),
						indent1, 0);
				al_change = TRUE;
			}
			if (!tbl_al_change)
			{
				form_out_line(ERget(S_VQ00B1_Tbl_Alias_Changes),
						indent2, 1, (PTR)tptr->name);
				tbl_al_change = TRUE;
			}
			form_out_line(ERget(S_VQ00B2_Alias_Change), indent3,
					2, (PTR)oldalias, 
					(PTR)newalias);
		}
	}
	/* 
	** Loop through the escape code, making silent changes 
	** where aliases have changed.  If we now have a conflict,
	** dump out the escape code.
	*/
	for ( i = 0; i < mf->numescs; i++)
	{
		MFESC *mfesc = mf->escs[i];
		PTR	ptr;
		switch (mfesc->type)
		{
		    case ESC_FLD_ENTRY:
		    case ESC_FLD_CHANGE:
		    case ESC_FLD_EXIT:
			/* 
			** See if this matches an old alias, a new alias, or
			** neither.
			*/
			if (IIUGhfHtabFind(ohashtab, mfesc->item, &ptr) == OK)
			{
				/* 
				** It matches an old alias.  Silently change it
				** to the new alias.
				*/
				mfesc->item = (char *)ptr;
			}
			else if (IIUGhfHtabFind(nhashtab, mfesc->item, &ptr) 
					== OK)
			{
				/*
				** It matches a new alias.  It must be a user
				** field which now conflicts.  Delete it.
				*/
				delete_one_escape(
					mf, mfesc, 
					(ER_MSGID)S_VQ00B3_User_Field_Conflict, 
					indent1);
			}
		}
	}
	/* Delete the hash tables. */
	IIUGhrHtabRelease(ohashtab);
	IIUGhrHtabRelease(nhashtab);
}
/*
**	If we've deleted a numbered column, we may have created a gap
**	in the numbering.  We'll close it up.
*/
static VOID
resequence_columns(table)
MFTAB	*table;		/* Table whose columns to resequence */
{
	i4	highnum;	/* Highest sequence number */
	i4	seqnum;		/* Current sequence number */
	MFCOL	**seqptrs;	/* pointers to sequenced columns */
	MFCOL	**cols = table->cols;
        i4      i;
	/* Get the highest sequence number */
	for (highnum = 0, i = 0; i < table->numcols; i++)
	{
		highnum = max(highnum, cols[i]->corder);
	}
	if (highnum <= 0)
	{
		/* No ordered columns */
		return;
	}
	/* Get space for seqenced column pointers */
	seqptrs = (MFCOL **)IIVQ_VAlloc((highnum + 1) * sizeof(MFCOL *),
					(char *)NULL);
	for (i = 1; i <= highnum; i++)
	{
		seqptrs[i] = (MFCOL *)NULL;
	}
	/* Fill in sequenced column pointers array */
	for (i = 0; i < table->numcols; i++)
	{
		i4 corder = cols[i]->corder;
		if (corder > 0)
			seqptrs[corder] = cols[i];
	}
	/* Fill in new sequence numbers */
	for (i = 1, seqnum = 1; i <= highnum; i++)	
	{
		if (seqptrs[i] != (MFCOL *)NULL)
			seqptrs[i]->corder = seqnum++;
	}
}
/*
**	Mark that a column has changed.  Output a report line if it's the
**	first time we've realized this.
*/
static VOID 
column_changed(tblname, colname, tblch, colch)
char	*tblname;	/* table's name */
char	*colname;	/* column's name */
bool	*tblch;		/* (input/output) -- whether table has changed */
bool	*colch;		/* (input/output) -- whether column has changed */
{
	/* If it hadn't changed, output a report line */
	if (!*colch)
	{
		/* See if table has changed */
		table_changed(tblname, tblch);
		form_out_line(ERget(S_VQ00BE_Column_Changed), indent2, 1, 
			      colname);
		*colch = TRUE;
	}
}
/*
**	Mark that a table has changed.  Output a report line if it's the
**	first time we've realized this.
*/
static VOID 
table_changed(name, changed)
char	*name;		/* table's name */
bool	*changed;	/* (input/output) -- whether table has changed */
{
	/* If it hadn't changed, output a report line */
	if (!*changed)
	{
		form_out_line(ERget(S_VQ00BD_Table_Changed), indent1, 1, name);
		*changed = TRUE;
	}
}
/*
** Change a DB_DATATYPE into a string, including nullability
*/
static char *
db_type_name(type, def)
DB_DATA_VALUE *type;	/* Type to convert */
bool	def;		/* Does it have a default */
{
	char typename[80];
	DB_USER_TYPE user_type;
	_VOID_ afe_tyoutput(FEadfcb(), type, &user_type);
	STcopy(user_type.dbut_name, typename);
	/* Add defaultability and nullability info */
	switch (user_type.dbut_kind)
	{
	    case DB_UT_NULL:
		STpolycat(3, user_type.dbut_name, _space, 
			  ERget(S_VQ00C7_with_null), typename);
		break;
	    case DB_UT_NOTNULL:
	    case DB_UT_DEF:
		/* We can't depend on afe_tyouput to get this right */
		if (def)
		{
			STpolycat(3, user_type.dbut_name, _space, 
			  ERget(S_VQ00C9_not_null_with_default), typename);
		}
		else
		{
			STpolycat(3, user_type.dbut_name, _space, 
			  ERget(S_VQ00C8_not_null), typename);
		}
		break;
	    default:
		/* This shouldn't happen, but we'll return the type name */
		STcopy(user_type.dbut_name, typename);
		break;
	}
	return (IIVQ_VAlloc(0, typename));
}
/*
**	Routine to format and output a line 
*/
/*VARARGS3*/
static VOID
form_out_line(fmt, indent, parcount, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
char            *fmt;
char		*indent;
i4              parcount;
PTR             a1;
PTR             a2;
PTR             a3;
PTR             a4;
PTR             a5;
PTR             a6;
PTR             a7;
PTR             a8;
PTR             a9;
PTR             a10;
{
	output_line(IIUGfmt(buffer, II_BUFSIZ, fmt, parcount, 
		    	    a1, a2, a3, a4, a5, a6, a7, a8, a9, a10), 
		    (PTR)indent);
}
/*
**	Routine to normalize name prior to checking for existence in 
**	IIVQmmtMakeMfTable.  Assumes the name is valid, since it was already
**	shipped through ff_CheckRel when the name was added to the metaframe
**
**	Inputs: 
**		unnormalized name
**
**	Outputs:
**		normalized name
**
**	History:
**		01-24-95 (forky01)
**		    Created to fix bug #66529.
*/
static VOID
normalize_valid_name(unrml_name, nrml_name)
char            *unrml_name;
char		*nrml_name;
{
        /* for 6.5 ["]owner["].["]tablename["] support */
        FE_RSLV_NAME    resolved_name;
        char    owner_part[FE_MAXNAME + 1];
        char    table_part[FE_MAXNAME + 1];
        resolved_name.name = unrml_name;
        resolved_name.owner_dest = owner_part;
        resolved_name.name_dest = table_part;
        resolved_name.is_nrml = FALSE;
        FE_fullresolve(&resolved_name);
	/*
	** We don't need to be concerned with the owner name if
	** there isn't one (!), or if the table owner is the
	** session user, or if we're running against a 6.4
	** database.
	*/
	if ( (resolved_name.owner_spec == FALSE) ||
	     (STcompare(IIUIcsl_CommonSQLLevel(),UI_LEVEL_65) < 0) ||
	     (STequal(owner_part, IIUIdbdata()->suser)) )
	{
		STcopy(resolved_name.name_dest, nrml_name);
	}
	else
	{
		STpolycat(3, owner_part, ".", table_part, nrml_name);
	}
}
/*
**	Function to write a line to the report tablefield.  This
** 	is used both directly and as a callback from IIFGglsGenLinkStr.
**	In the first case, we pass in the indentation string; in the
**	second, it's already been applied.
**
** 	We interpret embedded newlines as row separators.
*/
static VOID
output_line(line, p_indent)
char	*line;		/* line to output */
PTR	p_indent;	/* Indentation string to preced each line. */
{
  char row[II_BUFSIZ];
	char	*nl;
	char	*indent = (char *)p_indent;
	/* Loop, looking for newlines */
	while (line != NULL && *line != EOS)
	{
		nl = STindex(line, _nl, 0);
		/* If we find a newline, replace it with a null-terminator */
		if (nl != NULL)
			*nl = EOS;
		/* Apply indentation, if any */
		*row = EOS;
		if (indent != NULL)
		{
			STcopy(indent, row);
		}
		STcat(row, line);
/* # line 1482 "vqrecfrm.qsc" */	/* loadtable */
  {
    if (IItbact(_form,_tf,1) != 0) {
      IItcoputio(_tfcol,(short *)0,1,32,0,row);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 1483 "vqrecfrm.qsc" */	/* host code */
		if (nl == NULL)
			break;
		line = nl + 1;
	}
}
/*
**	Allocate a linked string.
*/
static LSTR *
alloc_lstr(parent, follow)
LSTR    *parent;
i4      follow;
{
	LSTR *ptr;
	ptr = IIFGals_allocLinkStr(parent, follow);
	if (ptr == NULL)
		IIUGbmaBadMemoryAllocation(routine);
	return (ptr);
}
/*
**	See if a datatype has changed.
**	That is, do we need to create a new form field for this column.
*/
static bool
changed_datatype(old, new)
DB_DATA_VALUE *old;
DB_DATA_VALUE *new;
{
	return (new->db_datatype != old->db_datatype ||
		new->db_length != old->db_length);	
}
/*
** Return the length of the datatype minus any null indicator
*/
static i4
true_length(dtype)
DB_DATA_VALUE *dtype;
{
	if (dtype->db_datatype >= 0)
		return dtype->db_length;
	else
		return dtype->db_length - 1;
}
/*{
** Name:	IIVQgnmGetNewMF               - Set up the new mf for viewing
**
** Description:
**	We've kept the join and table structures both for the original
**	metaframe and for the reconciled version.  Here we put the reconciled
**	ones into the metaframe prepratory to displaying or saving 
**	the reconciled VQ.
**
** Inputs:
**	mf		METAFRAME *	Metaframe
**
** Outputs:
**	none
**
**	Returns:
**		none
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	1/29/90 (Mike S)	Initial Version
*/
VOID
IIVQgnmGetNewMF(mf)
METAFRAME	*mf;
{
	mf->tabs = nmftabs;
	mf->numtabs = nnumtabs;
	mf->joins = nmfjoins;
	mf->numjoins = nnumjoins;
}
/*{
** Name:	IIVQgomGetOldMF               - Set up the old mf for viewing
**
** Description:
**	We've kept the join and table structures both for the original
**	metaframe and for the reconciled version.  Here we put the original
**	ones into the metaframe prepratory to displaying the original VQ.
**
** Inputs:
**	mf		METAFRAME *	Metaframe
**
** Outputs:
**	none
**
**	Returns:
**		none
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	1/29/90 (Mike S)	Initial Version
*/
VOID
IIVQgomGetOldMF(mf)
METAFRAME	*mf;
{
	mf->tabs = omftabs;
	mf->numtabs = onumtabs;
	mf->joins = omfjoins;
	mf->numjoins = onumjoins;
}
/*
** Check whether there are any columns in the new metaframe which are used
** for optimistic locking.
*/
static bool
any_optlock_cols(mf)
METAFRAME *mf;
{
	i4	i;
	i4	j;
	MFTAB	*tabptr;
	for (i = 0; i < mf->numtabs; i++)
	{
		tabptr = mf->tabs[i];
		/* only update tables can have optimistic locking columns */
		if (tabptr->usage == TAB_UPDATE)
		{
			/*
			** go through all columns looking for one which is
			** used for optimistic locking. Stop when we find one.
			*/
			for (j = 0; j < tabptr->numcols; j++)
			{
				if (tabptr->cols[j]->flags & COL_OPTLOCK)
				{
					return(TRUE);
				}
			}
		}
	}
	/* We didn't find any optimistic locking columns; return false */
	return(FALSE);
}