# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	"rbftype.h"
# include	"rbfglob.h"
# include	<ug.h>
# include	<si.h>
# include	<cm.h>
# include	<te.h>
# include	<st.h>
# include       <flddesc.h>
# include       <uf.h>
/*
**   R_INDOPTS - display and control the INDENTOPTS form to select
**	the break columns for the an Indented Style report.
**
**	Note: that this fills in the fields of the "rfindop" form
**		  from the current COPT structure, which should be up to date.
**
**	Parameters:
**		none.
**
**	Returns:
**		TRUE if successful, FALSE if not successful.
**
**	Side Effects:
**		Fields in the COPT structure.
**
**	Called By:
**		r_IndentedSetUp.
**
**	Trace Flags:
**		None.
**
**	Error Messages:
**		none.
**
**	History:
**		1/2/90 (martym) (garnet) Derived from "rFcoptions()" for RBF.
**		1/10/90 (martym) Added check to make sure that default break 
**			column is gotten rid of if the user turns off the 
**			break on it.
**		1/13/90 (martym) 
**			Increased the size of tmp_buf [].
**		1/23/90 (martym)
**			Took out the "clear screen" since the form is now 
**			a popup.
**		22-feb-90 (sylviap)
**			Took out abbreviations for entering 'yes/no'.
**		4/24/90 (martym)
**			Added initializations for the new fields (sec_brkseq, 
**			and sec_brkord) of the Sec_node struct.
**		25-apr-90 (elein)
**			Dont display form "with style=popup." We specified this
**			as a popup in vifred--we don't have to and shouldnt
**			do it here
**		04-sep-90 (elein)
**			Added explanations to the activate menuitems. #32699
**		19-sep-90 (sylviap)
**			Change break columns pop-up to accept numbers.
**			Added routine IIRFdso_DupSortOrder.
**		30-oct-1992 (rdrane)
**			Ensure that unsupported datatype fields cannot be
**			selected.  Remove declaration of r_gt_att() since
**			already declared in included hdr files.
**	12/09/92 (dkh) - Added Choose Columns support for rbf.
**              02-Jul-1999 (hweho01)
**                      Included uf.h header file, it contains the function     
**                      prototype of IIUFlcfLocateForm(). Without the explicit
**                      declaration, the default int return value for a  
**                      function will truncate a 64-bit address on ris_u64 
**                      platform.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
FUNC_EXTERN bool IIRFdso_DupSortOrder();
FUNC_EXTERN VOID AdjustSections();
#define MAXSEQNUM       DB_GW2_MAX_COLS + 1
bool r_IndOpts(sort_array)
i4	sort_array[];	/* array to return sorted by sort sequence */
{
	/* 
	** external declarations:
	*/
  GLOBALREF char *Nindtopts;
  GLOBALREF bool alloc_indopts;
	/* 
	** internal declarations:
	*/
	register i4  i;				/* fast i4  counter */
	register COPT *copt;			/* COPT structure */
	register CS	*cs;
  char attbuf[FE_MAXNAME + 1];
  i4 sort_sequence;
  i4 j;
  i4 attord;
  char *attname;
  char *form;
	ATT	*att;
	i4 	NextSeq = 1;
	Sec_node *n;
	char 	tmp_buf[(FE_MAXNAME * 2) + 1];
	bool 	AtLeastOne = FALSE;
	bool 	Selected = TRUE;
	i4     srt_ord[MAXSEQNUM + 1]; 	/* used to get sort order */
  char *choices;
  char *help_scr;
  char *strtbl;
  char *sortseq;
  char *colname;
	/* 
	** Start of routine.  Set up field names of frame rfindopt.frm.
	*/
	strtbl = F_STRTBL;
	sortseq = F_SRTSEQ;
	colname = F_COLNM;
	Nindtopts = F_INDTOPTS;
	sort_sequence = 0;
	for (i=0; i <= MAXSEQNUM; i++)
	{
		srt_ord[i] = 0;
	}
	if (!alloc_indopts)
	{
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), form = Nindtopts) != OK)
		{
			IIUGerr(E_RF002D_Cannot_locate_form, 0, 1, form);
			return(FALSE);
		}
		alloc_indopts = TRUE;
	}
/* # line 157 "rfindopt.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpsetio(5,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(Nindtopts,(char *)"u") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 163 "rfindopt.qsc" */	/* inittable */
      {
        if (IItbinit(Nindtopts,strtbl,(char *)"u") != 0) {
          IItfill();
        } /* IItbinit */
      }
/* # line 165 "rfindopt.qsc" */	/* host code */
		/* 
		** First fill in the fields from the copt structure:
		*/
		for (i = 1; i <= En_n_attribs; i++)
		{
			cs = rFgt_cs(i);
			if (!cs)
				continue;
			/*
			** Guard against unsupported datatype.
			*/
			if  (((att = r_gt_att(i)) == (ATT *)NULL) ||
				att->pre_deleted)
			{
				continue;
			}
			attname = att->att_name;
			copt = rFgt_copt(i);
			sort_sequence = copt->copt_sequence;
			/*	
			** Load a row of the tablefield:
			*/
/* # line 189 "rfindopt.qsc" */	/* loadtable */
      {
        if (IItbact(Nindtopts,strtbl,1) != 0) {
          IItcoputio(colname,(short *)0,1,32,0,attname);
          IItcoputio(sortseq,(short *)0,1,30,4,&sort_sequence);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 192 "rfindopt.qsc" */	/* host code */
		}
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 198 "rfindopt.qsc" */	/* scroll */
          {
            if (IItbsetio(1,Nindtopts,strtbl,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 203 "rfindopt.qsc" */	/* scroll */
          {
            if (IItbsetio(1,Nindtopts,strtbl,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,-1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 209 "rfindopt.qsc" */	/* validate */
          {
            if (IIchkfrm() == 0) {
              goto IIfdB1;
            } /* IIchkfrm */
          }
/* # line 210 "rfindopt.qsc" */	/* unloadtable */
          {
            if (IItbact(Nindtopts,strtbl,0) == 0) goto IItbE1;
            while (IItunload() != 0) {
              IItcogetio((short *)0,1,30,4,&attord,(char *)"_RECORD");
              IItcogetio((short *)0,1,30,4,&sort_sequence,sortseq);
              IITBceColEnd();
              {
/* # line 214 "rfindopt.qsc" */	/* host code */
			/* 
			** Save in sort sequence. Will create the break headers
			** in this order.
			*/
			srt_ord[sort_sequence] = attord;
              }
            } /* IItunload */
IItbE1:
            IItunend();
          }
/* # line 221 "rfindopt.qsc" */	/* host code */
		/* 
		** Check for duplicate sort order and at least one break column
		** is selected.
		*/
		if (!IIRFdso_DupSortOrder(TRUE, &attord))
		{
/* # line 227 "rfindopt.qsc" */	/* scroll */
          {
            if (IItbsetio(1,Nindtopts,strtbl,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,attord) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 228 "rfindopt.qsc" */	/* resume */
          {
            IIrescol(strtbl,sortseq);
            if (1) goto IIfdB1;
          }
/* # line 229 "rfindopt.qsc" */	/* host code */
		}
/* # line 231 "rfindopt.qsc" */	/* enddisplay */
          {
            if (1) goto IIfdF1;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 237 "rfindopt.qsc" */	/* host code */
		Selected = FALSE;
/* # line 238 "rfindopt.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 245 "rfindopt.qsc" */	/* host code */
		/* Display popup witht he valid sort sequences. */
		choices   = ERget(S_RF006F_sort_seq);
/* # line 247 "rfindopt.qsc" */	/* message */
          {
            IIFRgpcontrol(1,0);
            IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
            IIFRgpcontrol(2,0);
            IImessage(choices);
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 255 "rfindopt.qsc" */	/* host code */
		FEhelp(H_INDOPT, ERget(F_RF0062_Break_Columns));
        }
      } else if (IIretval() == 7) {
        {
/* # line 260 "rfindopt.qsc" */	/* host code */
		/*	
		** Get the info from the current row:
		*/
/* # line 263 "rfindopt.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Nindtopts,strtbl,-2) != 0) {
              IItcogetio((short *)0,1,30,4,&attord,(char *)"_RECORD");
              IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,attbuf,colname);
              IItcogetio((short *)0,1,30,4,&sort_sequence,sortseq);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 268 "rfindopt.qsc" */	/* host code */
		 /*
		 ** If this attribute is a break column,
		 ** make sure attribute name is not a reserved word:
		 */
		 if (sort_sequence != 0)
		 {
			 if (STcompare(attbuf, NAM_PAGE) == 0 ||
			     STcompare(attbuf, NAM_REPORT) == 0 ||
			     STcompare(attbuf, NAM_DETAIL) == 0)
			 {
				IIUGerr(E_RF0043_This_column_name_is_a, 0, 0);
				sort_sequence = 0;
/* # line 280 "rfindopt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nindtopts,strtbl,-2) != 0) {
              IItcoputio(sortseq,(short *)0,1,30,4,&sort_sequence);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 282 "rfindopt.qsc" */	/* resume */
          {
            IIrescol((char *)"rfsrttbl",(char *)"srtseq");
            if (1) goto IIfdB1;
          }
/* # line 283 "rfindopt.qsc" */	/* host code */
			 }
		}
		copt = rFgt_copt(attord);
		/*	
		** Update the COPT:
		*/
		copt->copt_sequence = sort_sequence;
		if (sort_sequence == 0)
			copt->copt_break = 'n';
		else
			copt->copt_break = 'y';
/* # line 296 "rfindopt.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
        }
      } else {
        if (1) goto IIfdE1;
      } /* IIretval */
    } /* IIrunform */
IIfdF1:
    if (IIchkfrm() == 0) goto IIfdB1;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    goto IIfdE1;
IImuI1:
    IIFRInternal(0);
    if (IInfrskact(5,ERget(F_FE0105_ExplTop),0,2,1) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(6,ERget(F_FE0106_ExplBottom),0,2,2) == 0) goto IIfdE1;
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_OK),ERget(F_RF0075_indented_cols),2,2,3) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,0,4) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,0,0,4) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,5) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(10,(char *)0,0,0,5) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,2,6) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,2,6) == 0) goto IIfdE1;
    if (IITBcaClmAct(strtbl,sortseq,0,7) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 302 "rfindopt.qsc" */	/* host code */
	if (Selected)
	{
		NextSeq = 0;
		for (i = 1; i <= MAXSEQNUM; i++)
		{
			/*
			** Now build the break headers in the order of the
			** sort column.  srt_ord is sorted with the indices
			** of the columns.
			*/
			if ((j=srt_ord[i]) != 0)
			{
				copt = rFgt_copt(j);
				NextSeq++;
				if (!(n = sec_list_find(SEC_BRKHDR, 
					NextSeq, &Sections)))
            			{
            				n=sec_node_alloc(SEC_BRKHDR, NextSeq,0);
					attname = (r_gt_att(j))->att_name;
            				STcopy(attname, tmp_buf);
					_VOID_ CMtoupper(tmp_buf, tmp_buf);
            				STcat(tmp_buf, 
					     ERget(F_RF003E_Break_Header));
            				n->sec_name = STalloc(tmp_buf);
					n->sec_brkseq = NextSeq;
					n->sec_brkord = r_mtch_att(attname);
            				sec_list_insert(n, &Sections);
					sort_array[NextSeq] = j;
            			}
				copt->copt_order = 'a'; 
				copt->copt_brkhdr = 'y'; 
				copt->copt_brkftr = 'n';
				copt->copt_sequence = NextSeq;
			}
		}
	}
	return(Selected);
}
/*
**   Name: IIRFdso_DupSortOrder
**
**	Description:
**		Checks the COPT (column option structure) if any duplicate
**		sort sequences has been specified.  
**
**	Called by:
**		rFcoptions  
**		r_IndOpts
**	
**	Returns:
**		if need_one is TRUE:
**		   TRUE - no duplicate sequences are specifed AND at least
**			  one sort sequence is specified
**		   FALSE - if otherwise
**		if need_one is FALSE:
**		   TRUE - no duplicate sequences are specifed 
**		   FALSE - if otherwise
**
**		if duplicate sort: row = the offending row
**		if no sort is set: row = 1 
**
**	Parameters:
**		need_one = TRUE if need to check if at least one sort is set
*/
bool
IIRFdso_DupSortOrder(need_one, row)
bool 	need_one;	/* TRUE = check for at least one break */
i4	*row;		/* offending row */
{
	bool  	seqarray[MAXSEQNUM]; 	/* used to check for */
					/* duplicate seq #'s */
	i4	attord;			/* attribute ordinal */
	bool	found_break = FALSE;
	i4	i;
	register COPT *copt;		/* COPT structure */
	for ( i = 0; i < MAXSEQNUM; i++ )
		seqarray[i] = FALSE;
	/* check for duplicates */
	for ( attord = 1; attord <= En_n_attribs; attord++ )
	{
		*row = attord;
		copt = rFgt_copt(attord);
		/* no sort set */
		if ( copt->copt_sequence == 0 )
			continue;
		/* if seqarray = FALSE, then this is first sort*/
		if (seqarray[copt->copt_sequence] == FALSE )
		{
			seqarray[copt->copt_sequence] = TRUE;
			found_break = TRUE;
		}
		else
		/* if seqarray = TRUE, then there is a duplicate */
		{
			/*
			** Bad... Have a duplicate.
			** Output a warning and let calling routine scroll
			** to the offending row.
			*/
			IIUGerr(E_RF0042_Duplicate_sort_sequen, 0, 0);
			return (FALSE);
		}
	}
	/* Was there at least one sort set? */
	if (need_one && !found_break)
	{
		IIUGerr(E_RF0071_Must_select_a_field, 0, 0);
		*row = 1;
		return (FALSE);
	}
	else
	{
		return (TRUE);
	}
}
